param(
    [int]$Frames = 24,
    [int]$Seed = 1337,
    [int]$StartFrame = 0,
    [int]$SampleStep = 1,
    [int]$MaxCompareFrames = 0,
    [int]$ReportMaxMismatches = 0,
    [ValidateSet("script", "json", "pr-summary")]
    [string]$ParityMode = "script",
    [ValidateSet("full", "compact")]
    [string]$ReportFormat = "full",
    [string]$ParitySummaryPath = "",
    [switch]$FailOnFirstMismatch,
    [switch]$KeepArtifacts,
    [string]$ArtifactManifestPath = "",
    [ValidateSet("text", "json")]
    [string]$ArtifactManifestFormat = "text"
)

$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path -Parent $PSScriptRoot
$ToolsDir = Join-Path $RepoRoot "tools"
$AssetsDir = Join-Path $RepoRoot "assets"

$DumpDir = Join-Path $AssetsDir "frame_dump_synth_smoke"
$OutPrefix = Join-Path $AssetsDir "smoke_amiga_pipeline"
if ([string]::IsNullOrWhiteSpace($ArtifactManifestPath)) {
    $ArtifactManifestPath = "$OutPrefix`_manifest.txt"
}

if ($StartFrame -lt 0) {
    throw "StartFrame must be >= 0"
}
if ($SampleStep -lt 1) {
    throw "SampleStep must be >= 1"
}
if ($MaxCompareFrames -lt 0) {
    throw "MaxCompareFrames must be >= 0"
}
if ($ReportMaxMismatches -lt 0) {
    throw "ReportMaxMismatches must be >= 0"
}

$Artifacts = @(
    $DumpDir,
    "$OutPrefix`_pm_raw.iab",
    "$OutPrefix`_pm_raw.txt",
    "$OutPrefix`_pm_raw_report.txt",
    "$OutPrefix`_pm_delta.iab",
    "$OutPrefix`_pm_delta.txt",
    "$OutPrefix`_pm_delta_report.txt",
    "$OutPrefix`_ri_raw.iab",
    "$OutPrefix`_ri_raw.txt",
    "$OutPrefix`_ri_raw_report.txt",
    "$OutPrefix`_ri_delta.iab",
    "$OutPrefix`_ri_delta.txt",
    "$OutPrefix`_ri_delta_report.txt"
)

function Invoke-Step {
    param(
        [string]$Cmd,
        [switch]$AllowFailure
    )
    Write-Host ">> $Cmd"
    & powershell -NoProfile -Command $Cmd | Out-Host
    $exitCode = [int]$LASTEXITCODE
    if ($exitCode -ne 0 -and -not $AllowFailure) {
        throw "Step failed: $Cmd"
    }
    return $exitCode
}

function Convert-ToIntOrDefault {
    param(
        [object]$Value,
        [int]$Default = 0
    )
    $parsed = 0
    if ([int]::TryParse([string]$Value, [ref]$parsed)) {
        return $parsed
    }
    return $Default
}

function Read-ParityReportData {
    param([string]$ReportPath)
    $data = [ordered]@{}
    if (-not (Test-Path -LiteralPath $ReportPath)) {
        return $data
    }

    $lines = Get-Content -LiteralPath $ReportPath
    if ($lines.Count -eq 0) {
        return $data
    }

    $firstNonEmpty = $lines | Where-Object { -not [string]::IsNullOrWhiteSpace($_) } | Select-Object -First 1
    if ($null -ne $firstNonEmpty -and $firstNonEmpty.Trim().StartsWith("PARITY_REPORT")) {
        $tokens = $firstNonEmpty.Trim().Split(" ")
        foreach ($token in $tokens) {
            if ($token -notmatch "=") {
                continue
            }
            $parts = $token.Split("=", 2)
            if ($parts.Count -eq 2) {
                $data[$parts[0]] = $parts[1]
            }
        }
        return $data
    }

    foreach ($raw in $lines) {
        $line = $raw.Trim()
        if ([string]::IsNullOrWhiteSpace($line) -or $line.StartsWith("#") -or $line -notmatch "=") {
            continue
        }
        $parts = $line.Split("=", 2)
        if ($parts.Count -eq 2) {
            $data[$parts[0].Trim()] = $parts[1].Trim()
        }
    }

    if (-not $data.Contains("status") -and $data.Contains("mismatched_frames")) {
        $mismatched = Convert-ToIntOrDefault -Value $data["mismatched_frames"] -Default -1
        if ($mismatched -eq 0) {
            $data["status"] = "ok"
        } elseif ($mismatched -gt 0) {
            $data["status"] = "fail"
        }
    }
    if (-not $data.Contains("mismatches") -and $data.Contains("mismatched_frames")) {
        $data["mismatches"] = $data["mismatched_frames"]
    }
    if (-not $data.Contains("diff_pixels") -and $data.Contains("total_diff_pixels")) {
        $data["diff_pixels"] = $data["total_diff_pixels"]
    }
    if (-not $data.Contains("compared") -and $data.Contains("compared_frames")) {
        $data["compared"] = $data["compared_frames"]
    }
    return $data
}

function Run-BundleCase {
    param(
        [string]$Suffix,
        [string]$Layout,
        [bool]$DeltaXor
    )

    $bundle = "$OutPrefix`_$Suffix.iab"
    $info = "$OutPrefix`_$Suffix.txt"
    $report = "$OutPrefix`_$Suffix`_report.txt"
    $sampleArgs = " --start-frame $StartFrame --frame-step $SampleStep"
    $buildCmd = ""
    $parityCmd = ""
    $buildExit = 0
    $parityExit = 0
    $reportData = [ordered]@{}
    $status = "ok"
    $statusReason = ""

    $deltaArg = ""
    if ($DeltaXor) {
        $deltaArg = " --delta-xor"
    }

    $buildCmd = "python `"$ToolsDir\build_amiga_resource_bundle.py`" --input-dir `"$DumpDir`" --output `"$bundle`" --info `"$info`" --layout $Layout$deltaArg"
    $buildExit = Invoke-Step -Cmd $buildCmd -AllowFailure
    if ($buildExit -ne 0) {
        return [pscustomobject]@{
            case_id = $Suffix
            layout = $Layout
            delta_xor = [bool]$DeltaXor
            status = "error"
            reason = "bundle_build_failed"
            build_exit_code = $buildExit
            parity_exit_code = $null
            bundle_path = $bundle
            info_path = $info
            report_path = $report
            compared = 0
            mismatches = 0
            diff_pixels = 0
        }
    }

    if ($MaxCompareFrames -gt 0) {
        $sampleArgs += " --max-frames $MaxCompareFrames"
    }

    $modeArg = " --script"
    if ($ParityMode -eq "json") {
        $modeArg = " --json"
    } elseif ($ParityMode -eq "pr-summary") {
        $modeArg = " --pr-summary"
    }

    $parityCmd = "python `"$ToolsDir\check_iab_parity.py`" --bundle `"$bundle`" --input-dir `"$DumpDir`" --report `"$report`" --report-format $ReportFormat --report-max-mismatches $ReportMaxMismatches$modeArg$sampleArgs"
    $parityExit = Invoke-Step -Cmd $parityCmd -AllowFailure
    $reportData = Read-ParityReportData -ReportPath $report

    if ($reportData.Contains("status")) {
        $status = [string]$reportData["status"]
    } elseif ($parityExit -eq 0) {
        $status = "ok"
    } else {
        $status = "fail"
    }
    if ($status -ne "ok" -and $status -ne "fail" -and $status -ne "error") {
        $status = if ($parityExit -eq 0) { "ok" } else { "error" }
    }
    if ($parityExit -ne 0 -and -not $reportData.Contains("status") -and -not $reportData.Contains("mismatches")) {
        $status = "error"
    }
    if ($parityExit -ne 0 -and $status -eq "ok") {
        $status = "error"
    }
    if ($status -eq "error") {
        $statusReason = "parity_check_error"
    } elseif ($status -eq "fail") {
        $statusReason = "parity_mismatch"
    }

    return [pscustomobject]@{
        case_id = $Suffix
        layout = $Layout
        delta_xor = [bool]$DeltaXor
        status = $status
        reason = $statusReason
        build_exit_code = $buildExit
        parity_exit_code = $parityExit
        bundle_path = $bundle
        info_path = $info
        report_path = $report
        compared = Convert-ToIntOrDefault -Value $reportData["compared"] -Default 0
        mismatches = Convert-ToIntOrDefault -Value $reportData["mismatches"] -Default 0
        diff_pixels = Convert-ToIntOrDefault -Value $reportData["diff_pixels"] -Default 0
    }
}

function Get-Sha256HexFromUtf8Text {
    param([string]$Text)
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($Text)
    $sha = [System.Security.Cryptography.SHA256]::Create()
    try {
        $digest = $sha.ComputeHash($bytes)
    } finally {
        $sha.Dispose()
    }
    return ([System.BitConverter]::ToString($digest)).Replace("-", "").ToLower()
}

function Get-ArtifactRecords {
    param([string[]]$Paths)
    $records = @()
    foreach ($path in $Paths) {
        if (-not (Test-Path -LiteralPath $path)) {
            continue
        }
        if ((Get-Item -LiteralPath $path).PSIsContainer) {
            continue
        }
        $resolved = (Resolve-Path -LiteralPath $path).Path
        $hash = Get-FileHash -LiteralPath $path -Algorithm SHA256
        $name = Split-Path -Leaf $path
        $size = (Get-Item -LiteralPath $path).Length
        $records += [pscustomobject]@{
            Name = $name
            Path = $resolved
            Bytes = [int64]$size
            Sha256 = $hash.Hash.ToLower()
        }
    }
    return ,$records
}

function Write-ArtifactChecksums {
    param([object[]]$Records)
    Write-Host "Artifact checksums:"
    foreach ($rec in $Records) {
        Write-Host "  HASH file=$($rec.Name) bytes=$($rec.Bytes) sha256=$($rec.Sha256)"
    }
}

function Write-AggregateReportChecksum {
    $reportFiles = @(
        "$OutPrefix`_pm_raw_report.txt",
        "$OutPrefix`_pm_delta_report.txt",
        "$OutPrefix`_ri_raw_report.txt",
        "$OutPrefix`_ri_delta_report.txt"
    ) | Where-Object { Test-Path -LiteralPath $_ }

    if ($reportFiles.Count -eq 0) {
        return [pscustomobject]@{
            Hash = ""
            Count = 0
        }
    }

    $lines = @()
    foreach ($path in ($reportFiles | Sort-Object)) {
        $hash = Get-FileHash -LiteralPath $path -Algorithm SHA256
        $name = Split-Path -Leaf $path
        $size = (Get-Item -LiteralPath $path).Length
        $lines += "$name|$size|$($hash.Hash.ToLower())"
    }
    $joined = [string]::Join("`n", $lines)
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($joined)
    $sha = [System.Security.Cryptography.SHA256]::Create()
    try {
        $agg = $sha.ComputeHash($bytes)
    } finally {
        $sha.Dispose()
    }
    $aggHex = ([System.BitConverter]::ToString($agg)).Replace("-", "").ToLower()
    Write-Host "  HASH_AGG reports_sha256=$aggHex count=$($reportFiles.Count)"
    return [pscustomobject]@{
        Hash = $aggHex
        Count = $reportFiles.Count
    }
}

function Write-ParitySummaryJson {
    param(
        [string]$SummaryPath,
        [object[]]$CaseResults,
        [object]$ReportAggregate,
        [bool]$ArtifactsKept
    )
    if ([string]::IsNullOrWhiteSpace($SummaryPath)) {
        return
    }

    $summaryDir = Split-Path -Parent $SummaryPath
    if (-not [string]::IsNullOrWhiteSpace($summaryDir)) {
        New-Item -ItemType Directory -Force -Path $summaryDir | Out-Null
    }

    $passed = @($CaseResults | Where-Object { $_.status -eq "ok" }).Count
    $failed = @($CaseResults | Where-Object { $_.status -eq "fail" }).Count
    $errors = @($CaseResults | Where-Object { $_.status -eq "error" }).Count
    $totalCompared = 0
    $totalMismatches = 0
    $totalDiffPixels = 0
    foreach ($case in $CaseResults) {
        $totalCompared += Convert-ToIntOrDefault -Value $case.compared -Default 0
        $totalMismatches += Convert-ToIntOrDefault -Value $case.mismatches -Default 0
        $totalDiffPixels += Convert-ToIntOrDefault -Value $case.diff_pixels -Default 0
    }
    $overallStatus = if ($failed -gt 0 -or $errors -gt 0) { "fail" } else { "ok" }

    $summary = [ordered]@{
        generated_utc = [DateTime]::UtcNow.ToString("o")
        frames = $Frames
        seed = $Seed
        start_frame = $StartFrame
        sample_step = $SampleStep
        max_compare_frames = $MaxCompareFrames
        report_max_mismatches = $ReportMaxMismatches
        parity_mode = $ParityMode
        report_format = $ReportFormat
        fail_on_first_mismatch = [bool]$FailOnFirstMismatch
        artifacts_kept = [bool]$ArtifactsKept
        overall_status = $overallStatus
        totals = [ordered]@{
            cases = [int]$CaseResults.Count
            passed = [int]$passed
            failed = [int]$failed
            errors = [int]$errors
            compared = [int]$totalCompared
            mismatches = [int]$totalMismatches
            diff_pixels = [int]$totalDiffPixels
        }
        report_aggregate = [ordered]@{
            sha256 = $ReportAggregate.Hash
            count = [int]$ReportAggregate.Count
        }
        cases = @()
    }
    foreach ($case in $CaseResults) {
        $summary.cases += [ordered]@{
            id = $case.case_id
            layout = $case.layout
            delta_xor = [bool]$case.delta_xor
            status = $case.status
            reason = $case.reason
            compared = [int](Convert-ToIntOrDefault -Value $case.compared -Default 0)
            mismatches = [int](Convert-ToIntOrDefault -Value $case.mismatches -Default 0)
            diff_pixels = [int](Convert-ToIntOrDefault -Value $case.diff_pixels -Default 0)
            build_exit_code = [int](Convert-ToIntOrDefault -Value $case.build_exit_code -Default 0)
            parity_exit_code = if ($null -eq $case.parity_exit_code) { $null } else { [int](Convert-ToIntOrDefault -Value $case.parity_exit_code -Default 0) }
            bundle_path = $case.bundle_path
            info_path = $case.info_path
            report_path = $case.report_path
        }
    }

    $utf8NoBom = [System.Text.UTF8Encoding]::new($false)
    $json = $summary | ConvertTo-Json -Depth 8
    [System.IO.File]::WriteAllText($SummaryPath, $json + "`n", $utf8NoBom)
    Write-Host "Wrote parity summary JSON: $SummaryPath"
}

function Write-ArtifactManifest {
    param(
        [string]$ManifestPath,
        [object[]]$Records,
        [object]$ReportAggregate
    )
    $manifestDir = Split-Path -Parent $ManifestPath
    if (-not [string]::IsNullOrWhiteSpace($manifestDir)) {
        New-Item -ItemType Directory -Force -Path $manifestDir | Out-Null
    }
    $utf8NoBom = [System.Text.UTF8Encoding]::new($false)

    $timestampUtc = [DateTime]::UtcNow.ToString("o")
    if ($ArtifactManifestFormat -eq "json") {
        $manifest = [ordered]@{
            generated_utc = $timestampUtc
            frames = $Frames
            seed = $Seed
            start_frame = $StartFrame
            sample_step = $SampleStep
            max_compare_frames = $MaxCompareFrames
            report_max_mismatches = $ReportMaxMismatches
            parity_mode = $ParityMode
            report_format = $ReportFormat
            manifest_format = "json"
            report_aggregate = [ordered]@{
                sha256 = $ReportAggregate.Hash
                count = [int]$ReportAggregate.Count
            }
            artifacts_count = [int]$Records.Count
            artifacts = @()
        }
        foreach ($rec in ($Records | Sort-Object Name, Path)) {
            $manifest.artifacts += [ordered]@{
                file = $rec.Name
                bytes = [int64]$rec.Bytes
                sha256 = $rec.Sha256
                path = $rec.Path
            }
        }
        $manifestPayloadJson = $manifest | ConvertTo-Json -Depth 6 -Compress
        $manifestPayloadHash = Get-Sha256HexFromUtf8Text -Text $manifestPayloadJson
        $manifest["manifest_payload_sha256"] = $manifestPayloadHash
        $json = $manifest | ConvertTo-Json -Depth 6
        [System.IO.File]::WriteAllText($ManifestPath, $json + "`n", $utf8NoBom)
        Write-Host "Wrote artifact manifest: $ManifestPath (json)"
        Write-Host "  HASH_MANIFEST_PAYLOAD sha256=$manifestPayloadHash format=json"
        return
    }

    $lines = @(
        "# Icepanic Amiga pipeline smoke artifact manifest",
        "generated_utc=$timestampUtc",
        "frames=$Frames",
        "seed=$Seed",
        "start_frame=$StartFrame",
        "sample_step=$SampleStep",
        "max_compare_frames=$MaxCompareFrames",
        "report_max_mismatches=$ReportMaxMismatches",
        "parity_mode=$ParityMode",
        "report_format=$ReportFormat",
        "manifest_format=text",
        "report_aggregate_sha256=$($ReportAggregate.Hash)",
        "report_count=$($ReportAggregate.Count)",
        "artifacts_count=$($Records.Count)"
    )
    foreach ($rec in ($Records | Sort-Object Name, Path)) {
        $lines += "artifact|file=$($rec.Name)|bytes=$($rec.Bytes)|sha256=$($rec.Sha256)|path=$($rec.Path)"
    }
    $payloadContent = [string]::Join("`n", $lines) + "`n"
    $manifestPayloadHash = Get-Sha256HexFromUtf8Text -Text $payloadContent
    $finalLines = $lines + "manifest_payload_sha256=$manifestPayloadHash"
    $content = [string]::Join("`n", $finalLines) + "`n"
    [System.IO.File]::WriteAllText($ManifestPath, $content, $utf8NoBom)
    Write-Host "Wrote artifact manifest: $ManifestPath (text)"
    Write-Host "  HASH_MANIFEST_PAYLOAD sha256=$manifestPayloadHash format=text"
}

$dumpCmd = "python `"$ToolsDir\generate_deterministic_frame_dump.py`" --output-dir `"$DumpDir`" --frames $Frames --seed $Seed"
$dumpExit = Invoke-Step -Cmd $dumpCmd -AllowFailure
if ($dumpExit -ne 0) {
    throw "Deterministic frame dump generation failed."
}

$caseMatrix = @(
    [ordered]@{ Suffix = "pm_raw"; Layout = "plane-major"; DeltaXor = $false },
    [ordered]@{ Suffix = "pm_delta"; Layout = "plane-major"; DeltaXor = $true },
    [ordered]@{ Suffix = "ri_raw"; Layout = "row-interleaved"; DeltaXor = $false },
    [ordered]@{ Suffix = "ri_delta"; Layout = "row-interleaved"; DeltaXor = $true }
)

$caseResults = @()
$stoppedEarly = $false
foreach ($case in $caseMatrix) {
    $result = Run-BundleCase -Suffix $case.Suffix -Layout $case.Layout -DeltaXor:$case.DeltaXor
    $caseResults += $result
    if ($result.status -ne "ok") {
        Write-Host "Case $($result.case_id) status=$($result.status) reason=$($result.reason)"
        if ($FailOnFirstMismatch) {
            Write-Host "Fail-on-first-mismatch enabled; stopping after first failing case."
            $stoppedEarly = $true
            break
        }
    }
}

$passedCount = @($caseResults | Where-Object { $_.status -eq "ok" }).Count
$failedCount = @($caseResults | Where-Object { $_.status -eq "fail" }).Count
$errorCount = @($caseResults | Where-Object { $_.status -eq "error" }).Count

if ($failedCount -eq 0 -and $errorCount -eq 0 -and -not $stoppedEarly) {
    Write-Host "Smoke pipeline passed for all layout/delta combinations."
} elseif ($failedCount -eq 0 -and $errorCount -eq 0 -and $stoppedEarly) {
    Write-Host "Smoke pipeline stopped early with only passing cases."
} else {
    Write-Host "Smoke pipeline completed with parity issues: pass=$passedCount fail=$failedCount error=$errorCount"
}

$artifactRecords = Get-ArtifactRecords -Paths $Artifacts
Write-ArtifactChecksums -Records $artifactRecords
$reportAgg = Write-AggregateReportChecksum

$preserveArtifacts = [bool]$KeepArtifacts -or ($failedCount -gt 0) -or ($errorCount -gt 0)
if ($FailOnFirstMismatch -and ($failedCount -gt 0 -or $errorCount -gt 0)) {
    $preserveArtifacts = $true
}

$writeManifest = [bool]$KeepArtifacts -or ($FailOnFirstMismatch -and ($failedCount -gt 0 -or $errorCount -gt 0))
if ($writeManifest) {
    Write-ArtifactManifest -ManifestPath $ArtifactManifestPath -Records $artifactRecords -ReportAggregate $reportAgg
    $manifestHash = Get-FileHash -LiteralPath $ArtifactManifestPath -Algorithm SHA256
    $manifestName = Split-Path -Leaf $ArtifactManifestPath
    $manifestSize = (Get-Item -LiteralPath $ArtifactManifestPath).Length
    Write-Host "  HASH file=$manifestName bytes=$manifestSize sha256=$($manifestHash.Hash.ToLower())"
}

Write-ParitySummaryJson -SummaryPath $ParitySummaryPath -CaseResults $caseResults -ReportAggregate $reportAgg -ArtifactsKept:$preserveArtifacts

if (-not $preserveArtifacts) {
    foreach ($path in $Artifacts) {
        if (Test-Path -LiteralPath $path) {
            Remove-Item -LiteralPath $path -Recurse -Force
        }
    }
    Write-Host "Cleaned smoke artifacts."
} else {
    Write-Host "Kept smoke artifacts under assets/."
}

if ($failedCount -gt 0 -or $errorCount -gt 0) {
    throw "Smoke pipeline detected parity mismatches/errors (pass=$passedCount fail=$failedCount error=$errorCount)."
}
