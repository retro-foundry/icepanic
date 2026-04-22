# Icepanic (C + SDL2)

`Icepanic` is an original Pengo-style single-screen arcade prototype built for deterministic gameplay and eventual Amiga-friendly porting.

## Constraints
- Logical resolution: `320x200`
- Grid: `20x12` tiles at `16x16`
- Fixed update: `50 Hz`
- Palette: `32 colors`
- Logic uses integer/fixed-point movement only

## Implemented Gameplay
- Sliding block crush core loop with combo scoring.
- Two enemy archetypes (`CHASER` + `WANDERER`) with round-based mix.
- Lives, death/respawn flow, round clear and progression.
- Roguelite run layer:
  - perk draft between rounds (choose 1 of 3),
  - weighted anti-repeat perk offers (less immediate duplicate drafts),
  - run modifiers (extra life, extra round time, score multiplier, enemy slow),
  - run shard currency and banked meta shards on run reset/game over,
  - pre-run shard-spend upgrade pick on new runs (life/time/score/slow options).
- Stage mutator events per round (deterministic draft with anti-repeat cooldowns):
  - `SWARM` (enemy pressure spike),
  - `SHATTERED ICE` (ice -> cracked conversions),
  - `BLACK ICE` (extra block injection into open lanes),
  - `TURBO SLIDE` (faster block movement and reduced push windup).
- Special block alignment bonus event (score/time boost).
- Bonus items that spawn from crush streaks and expire on timer.
- Timer danger-state pulsing near timeout.
- Floating combo score popups.
- Outlined/clamped popup rendering for better readability.
- Lightweight core-event driven SFX hooks (optional SDL queued-audio beeps).

## Build
```powershell
cmake -S . -B build
cmake --build build --config Release
```

Run:
```powershell
.\build\Release\icepanic_sdl.exe
```
Lower-flash capture/testing mode:
```powershell
.\build\Release\icepanic_sdl.exe --low-intensity-viz
```
If you build without multi-config generators:
```powershell
.\build\icepanic_sdl.exe
```

Autoplay fun-proxy evaluator:
```powershell
.\build\Release\icepanic_fun_agent.exe --runs 24 --ticks 18000 --seed 1337
```
Profile-specific evaluation:
```powershell
.\build\Release\icepanic_fun_agent.exe --runs 24 --ticks 18000 --seed 1337 --profile aggressive
```
Available profiles: `arcade`, `casual`, `survival`, `aggressive`.
Machine-readable output:
```powershell
.\build\Release\icepanic_fun_agent.exe --runs 24 --ticks 18000 --seed 1337 --profile arcade --out-format json
.\build\Release\icepanic_fun_agent.exe --runs 24 --ticks 18000 --seed 1337 --profile arcade --out-format csv
```

Batch sweep helper (profile/seed matrix -> one CSV):
```powershell
python tools/run_fun_agent_sweep.py --runs 24 --ticks 18000 --seed-count 8 --output assets/fun_agent_sweep.csv
```
Optional JSON summary (CI-friendly):
```powershell
python tools/run_fun_agent_sweep.py --runs 24 --ticks 18000 --seed-count 8 --output assets/fun_agent_sweep.csv --summary-json assets/fun_agent_sweep_summary.json
```
Custom profiles/seeds example:
```powershell
python tools/run_fun_agent_sweep.py --profiles arcade,aggressive --seeds 1337:1340,1401 --output assets/fun_agent_sweep_custom.csv
```

Compare two sweep outputs (CSV or summary JSON):
```powershell
python tools/compare_fun_agent_sweeps.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv
```
CI-style regression gate:
```powershell
python tools/compare_fun_agent_sweeps.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --fail-on-regression --overall-threshold 0.0 --profile-threshold -5.0
```
Trend history helper (rolling window stability by profile):
```powershell
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.csv --window 6 --out-json assets/fun_agent_trend.json --out-csv assets/fun_agent_trend.csv
```
CI gate example (fail if any profile is unstable under current thresholds):
```powershell
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.csv --window 6 --fail-on-unstable --max-unstable-profiles 0
```
PR/CI compact summary with sparkline history:
```powershell
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.csv --window 6 --pr-summary --pr-summary-include-reasons
```
PR markdown block (ready to paste directly into GitHub comments):
```powershell
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.csv --window 6 --pr-markdown --pr-summary-include-reasons
```
Combined compare+trend markdown helper (single CI comment artifact):
```powershell
python tools/compare_fun_agent_sweeps.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --out-json assets/fun_agent_delta.json
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.csv --window 6 --out-json assets/fun_agent_trend.json
python tools/build_fun_agent_ci_comment.py --delta-json assets/fun_agent_delta.json --trend-json assets/fun_agent_trend.json --output assets/fun_agent_ci_comment.md
```
Optional per-profile text badges in markdown rows:
```powershell
python tools/build_fun_agent_ci_comment.py --delta-json assets/fun_agent_delta.json --trend-json assets/fun_agent_trend.json --badge-mode text --badge-delta-warn-threshold -0.5 --badge-delta-fail-threshold -2.0 --output assets/fun_agent_ci_comment.md
```
Custom markdown title:
```powershell
python tools/build_fun_agent_ci_comment.py --delta-json assets/fun_agent_delta.json --trend-json assets/fun_agent_trend.json --title "Icepanic Balance Bot" --output assets/fun_agent_ci_comment.md
```
One-command wrapper for compare+trend+comment:
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets
```
Wrapper with row badges enabled:
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets --badge-mode text
```
Wrapper with custom markdown heading:
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets --markdown-title "Icepanic Balance Bot"
```
Wrapper with compact summary JSON artifact:
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets --badge-mode text --summary-json-name fun_agent_ci_summary.json
```
Quiet CI-log mode:
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets --quiet-subtools
```
Artifact-paths-only mode (for script consumption):
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets --quiet-subtools --paths-only
```
You can also point it at summary JSON snapshots:
```powershell
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_summary_*.json --window 6
```

## Controls
- Move: Arrow keys or `WASD`
- Start/skip intro: `Space` or `Enter`
- Perk select: Arrow keys / `WASD`, confirm with `Space`/`Enter`
- Meta upgrade select (new run): Arrow keys / `WASD`, confirm with `Space`/`Enter`
- Quit: `Esc`

## Roguelite Persistence
- Banked meta shards are saved to:
  - `assets/meta_shards.sav` (with `../` and `../../` fallback search/write paths).
- Meta shards are saved on both bank (`GAME_EVENT_META_BANKED`) and spend (`GAME_EVENT_META_SPENT`).

## Frame Dump (Indexed)
For Amiga-style pipeline prototyping, dump the 8-bit indexed framebuffer directly:
```powershell
.\build\Release\icepanic_sdl.exe --dump-indexed assets\frame_dump --dump-max-frames 300
```
Writes:
- `palette.rgb` (`32 * RGB` bytes)
- `frame_00000.idx` ... raw `320*200` index8 frames
- `frame_info.txt`

Convert dumped index8 frames to Amiga-style 5-bitplane output:
```powershell
python tools/convert_frame_dump_to_bitplanes.py --input-dir assets\frame_dump --output-dir assets\frame_dump_5bpl
```
Writes:
- `frame_00000.bpl` ... planar frames (`40000` bytes each at `320x200x5bpl`)
- `palette_12bit.bin` / `palette_12bit.txt`
- `conversion_info.txt`

Optional crop mode for partial-screen blit benchmarking:
```powershell
python tools/convert_frame_dump_to_bitplanes.py --input-dir assets\frame_dump --output-dir assets\frame_dump_crop --crop-x 16 --crop-y 16 --crop-width 288 --crop-height 160
```

One-shot packed Amiga bundle for loader tests:
```powershell
python tools/build_amiga_resource_bundle.py --input-dir assets\frame_dump --output assets\amiga_frame_bundle.iab --info assets\amiga_frame_bundle.txt
```
Optional delta-XOR frame packing:
```powershell
python tools/build_amiga_resource_bundle.py --input-dir assets\frame_dump --output assets\amiga_frame_bundle_delta.iab --delta-xor
```

Inspect/reconstruct frames from an `IAB1` bundle with the C loader demo:
```powershell
.\build\Release\icepanic_iab_loader_demo.exe assets\amiga_frame_bundle.iab 0 assets\frame0_from_bundle.bpl
```

Parity-check bundle reconstruction against source index dumps:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump
```
Optional report output for CI artifacts:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --report assets\amiga_parity_report.txt
```
Compact report body (single-line summary + optional mismatch samples):
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --report assets\amiga_parity_report_compact.txt --report-format compact
```
Cap mismatch lines in console/report output (useful for failure spam control):
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --report assets\amiga_parity_report.txt --report-max-mismatches 20
```
Machine-readable script mode output:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --script
```
JSON summary mode output:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --json
```
Compact PR-friendly summary output:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --pr-summary
```
Optional frame sampling in parity checks:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --start-frame 0 --frame-step 4 --max-frames 20
```

End-to-end sample (dump -> bundle -> inspect -> parity check):
```powershell
.\build\Release\icepanic_sdl.exe --dump-indexed assets\frame_dump --dump-max-frames 120
python tools/build_amiga_resource_bundle.py --input-dir assets\frame_dump --output assets\amiga_frame_bundle.iab
.\build\Release\icepanic_iab_loader_demo.exe assets\amiga_frame_bundle.iab 0 assets\frame0_from_bundle.bpl
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump
```

Headless deterministic dump generation (for regression/CI without opening SDL):
```powershell
python tools/generate_deterministic_frame_dump.py --output-dir assets\frame_dump_synth --frames 120 --seed 1337
```

One-command smoke for deterministic dump -> bundle -> parity (plane-major + row-interleaved, raw + delta):
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1
```
Faster sampled smoke lane example:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -SampleStep 4 -MaxCompareFrames 12
```
JSON-mode smoke output example:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -ParityMode json -SampleStep 4 -MaxCompareFrames 12
```
PR-summary smoke output example:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -ParityMode pr-summary -SampleStep 4 -MaxCompareFrames 12
```
Compact smoke report files example:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -ReportFormat compact -SampleStep 4 -MaxCompareFrames 12
```
Smoke with mismatch cap passed through to parity:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -ReportFormat compact -ReportMaxMismatches 20
```
Keep generated artifacts with a manifest file:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -KeepArtifacts
```
Keep artifacts with JSON manifest:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -KeepArtifacts -ArtifactManifestFormat json
```
Custom manifest output path:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -KeepArtifacts -ArtifactManifestPath assets\my_smoke_manifest.txt
```
The smoke script prints per-artifact SHA-256 hashes, one aggregate report checksum line, and a manifest payload checksum (`HASH_MANIFEST_PAYLOAD`) that excludes checksum self-reference.

Validate planar outputs (tile strip and optional frame):
```powershell
python tools/validate_planar_assets.py --frame-bpl assets\frame_dump_5bpl\frame_00000.bpl
```

## Tests
```powershell
ctest --test-dir build --output-on-failure
```

## Asset Pipeline
Generate the runtime tileset from handcrafted pixel art with:
```powershell
python tools/build_tileset.py
```
The generator creates all runtime tiles directly (no concept-sheet sampling) while keeping the 32-color indexed format.
Art is consolidated into one canonical atlas image.

This writes:
- `assets/tileset.bin`
- `assets/tileset_manifest.txt`
- `assets/sprite_atlas_16.png` (single consolidated `16x16` sprite/tile atlas)
- `assets/tile_strip_16x16_5bpl.bin` (tile-major, plane-major Amiga strip)
- `assets/tile_strip_16x16_5bpl.txt`

Optional tile-strip byte layout variant:
```powershell
python tools/build_tileset.py --tile-strip-layout row-interleaved
```

If `assets/tileset.bin` is missing, the SDL client falls back to built-in placeholder tiles.
