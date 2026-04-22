#!/usr/bin/env python3
"""
Run compare + trend + markdown comment generation in one command.
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Dict, List, Optional


def run_cmd(cmd: List[str], echo: bool, quiet: bool) -> None:
    if echo:
        print("+", " ".join(cmd))
    completed = subprocess.run(cmd, text=True, capture_output=True)
    if completed.returncode != 0:
        if completed.stdout:
            print(completed.stdout, file=sys.stderr, end="" if completed.stdout.endswith("\n") else "\n")
        if completed.stderr:
            print(completed.stderr, file=sys.stderr, end="" if completed.stderr.endswith("\n") else "\n")
        raise RuntimeError(f"command failed with exit code {completed.returncode}: {' '.join(cmd)}")
    if (not quiet) and completed.stdout:
        print(completed.stdout, end="" if completed.stdout.endswith("\n") else "\n")


def _load_json(path: Path) -> Dict[str, object]:
    data = json.loads(path.read_text(encoding="utf-8-sig"))
    if not isinstance(data, dict):
        raise ValueError(f"JSON root must be object: {path}")
    return data


def _safe_float(raw: object) -> Optional[float]:
    try:
        return float(raw)  # type: ignore[arg-type]
    except (TypeError, ValueError):
        return None


def _classify_badge_severity(
    delta_mean: Optional[float],
    trend_stdev: Optional[float],
    trend_swing: Optional[float],
    unstable: bool,
    delta_warn_threshold: float,
    delta_fail_threshold: float,
    stdev_warn_threshold: float,
    stdev_fail_threshold: float,
    swing_warn_threshold: float,
    swing_fail_threshold: float,
    unstable_level: str,
) -> int:
    severity = 0
    if delta_mean is not None:
        if delta_mean < delta_fail_threshold:
            severity = max(severity, 2)
        elif delta_mean < delta_warn_threshold:
            severity = max(severity, 1)
    if trend_stdev is not None:
        if trend_stdev > stdev_fail_threshold:
            severity = max(severity, 2)
        elif trend_stdev > stdev_warn_threshold:
            severity = max(severity, 1)
    if trend_swing is not None:
        if trend_swing > swing_fail_threshold:
            severity = max(severity, 2)
        elif trend_swing > swing_warn_threshold:
            severity = max(severity, 1)
    if unstable:
        severity = max(severity, 2 if unstable_level == "fail" else 1)
    return severity


def build_compact_summary(
    delta_path: Path,
    trend_path: Path,
    markdown_path: Path,
    badge_mode: str,
    badge_delta_warn_threshold: float,
    badge_delta_fail_threshold: float,
    badge_stdev_warn_threshold: float,
    badge_stdev_fail_threshold: float,
    badge_swing_warn_threshold: float,
    badge_swing_fail_threshold: float,
    badge_unstable_level: str,
) -> Dict[str, object]:
    delta = _load_json(delta_path)
    trend = _load_json(trend_path)

    delta_block = delta.get("delta", {})
    if not isinstance(delta_block, dict):
        delta_block = {}
    baseline = delta.get("baseline", {})
    candidate = delta.get("candidate", {})
    if not isinstance(baseline, dict):
        baseline = {}
    if not isinstance(candidate, dict):
        candidate = {}

    trend_profiles = trend.get("per_profile", {})
    unstable_profiles = trend.get("unstable_profiles", [])
    if not isinstance(trend_profiles, dict):
        trend_profiles = {}
    if not isinstance(unstable_profiles, list):
        unstable_profiles = []

    badge_counts = {"PASS": 0, "WARN": 0, "FAIL": 0}
    delta_profiles = delta.get("profiles", {})
    if not isinstance(delta_profiles, dict):
        delta_profiles = {}

    for profile in sorted(set(delta_profiles.keys()) | set(trend_profiles.keys())):
        d_item = delta_profiles.get(profile, {})
        t_item = trend_profiles.get(profile, {})
        if not isinstance(d_item, dict):
            d_item = {}
        if not isinstance(t_item, dict):
            t_item = {}
        sev = _classify_badge_severity(
            delta_mean=_safe_float(d_item.get("delta_mean_fun_score")),
            trend_stdev=_safe_float(t_item.get("window_stdev_fun_score")),
            trend_swing=_safe_float(t_item.get("window_swing_fun_score")),
            unstable=bool(t_item.get("unstable")),
            delta_warn_threshold=badge_delta_warn_threshold,
            delta_fail_threshold=badge_delta_fail_threshold,
            stdev_warn_threshold=badge_stdev_warn_threshold,
            stdev_fail_threshold=badge_stdev_fail_threshold,
            swing_warn_threshold=badge_swing_warn_threshold,
            swing_fail_threshold=badge_swing_fail_threshold,
            unstable_level=badge_unstable_level,
        )
        if sev >= 2:
            badge_counts["FAIL"] += 1
        elif sev == 1:
            badge_counts["WARN"] += 1
        else:
            badge_counts["PASS"] += 1

    summary: Dict[str, object] = {
        "generated_utc": datetime.now(timezone.utc).isoformat(),
        "artifacts": {
            "delta_json": str(delta_path),
            "trend_json": str(trend_path),
            "markdown": str(markdown_path),
        },
        "overall": {
            "delta_mean_fun_score": _safe_float(delta_block.get("overall_mean_fun_score")),
            "baseline_mean_fun_score": _safe_float(baseline.get("overall_mean_fun_score")),
            "candidate_mean_fun_score": _safe_float(candidate.get("overall_mean_fun_score")),
            "unstable_profile_count": len(unstable_profiles),
            "profile_count": len(trend_profiles),
        },
        "unstable_profiles": [str(p) for p in unstable_profiles],
        "badge_mode": badge_mode,
        "badge_counts": badge_counts,
        "badge_thresholds": {
            "delta_warn_threshold": badge_delta_warn_threshold,
            "delta_fail_threshold": badge_delta_fail_threshold,
            "stdev_warn_threshold": badge_stdev_warn_threshold,
            "stdev_fail_threshold": badge_stdev_fail_threshold,
            "swing_warn_threshold": badge_swing_warn_threshold,
            "swing_fail_threshold": badge_swing_fail_threshold,
            "unstable_level": badge_unstable_level,
        },
    }
    return summary


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate one fun-agent CI markdown report from snapshots.")
    parser.add_argument("--baseline", required=True, help="Baseline sweep snapshot path (CSV or JSON summary).")
    parser.add_argument("--candidate", required=True, help="Candidate sweep snapshot path (CSV or JSON summary).")
    parser.add_argument(
        "--trend-inputs",
        required=True,
        help="Comma-separated trend inputs for profile_fun_agent_history.py --inputs.",
    )
    parser.add_argument("--window", type=int, default=6, help="Rolling window for trend history.")
    parser.add_argument("--max-profiles", type=int, default=12, help="Max profile rows in markdown output.")
    parser.add_argument("--output-dir", default="assets", help="Output directory for generated artifacts.")
    parser.add_argument("--delta-json-name", default="fun_agent_delta.json", help="Delta JSON output filename.")
    parser.add_argument("--trend-json-name", default="fun_agent_trend.json", help="Trend JSON output filename.")
    parser.add_argument("--markdown-name", default="fun_agent_ci_comment.md", help="Markdown output filename.")
    parser.add_argument("--markdown-title", default="Fun Agent CI Summary", help="Markdown heading text.")
    parser.add_argument(
        "--summary-json-name",
        help="Optional compact summary JSON output filename.",
    )
    parser.add_argument(
        "--paths-only",
        action="store_true",
        help="Print only artifact path lines at the end.",
    )
    parser.add_argument(
        "--badge-mode",
        choices=["off", "text"],
        default="off",
        help="Optional badge mode passed to build_fun_agent_ci_comment.py.",
    )
    parser.add_argument("--badge-delta-warn-threshold", type=float, default=-1.0)
    parser.add_argument("--badge-delta-fail-threshold", type=float, default=-3.0)
    parser.add_argument("--badge-stdev-warn-threshold", type=float, default=4.0)
    parser.add_argument("--badge-stdev-fail-threshold", type=float, default=6.0)
    parser.add_argument("--badge-swing-warn-threshold", type=float, default=10.0)
    parser.add_argument("--badge-swing-fail-threshold", type=float, default=14.0)
    parser.add_argument(
        "--badge-unstable-level",
        choices=["warn", "fail"],
        default="fail",
        help="Badge severity used for unstable profiles.",
    )
    parser.add_argument("--echo-commands", action="store_true", help="Echo spawned command lines.")
    parser.add_argument("--quiet-subtools", action="store_true", help="Suppress successful subtool stdout.")
    args = parser.parse_args()

    if args.window <= 0:
        print("error: --window must be > 0", file=sys.stderr)
        return 2
    if args.max_profiles <= 0:
        print("error: --max-profiles must be > 0", file=sys.stderr)
        return 2
    if not str(args.markdown_title).strip():
        print("error: --markdown-title must not be empty", file=sys.stderr)
        return 2
    if args.badge_delta_fail_threshold > args.badge_delta_warn_threshold:
        print("error: --badge-delta-fail-threshold should be <= --badge-delta-warn-threshold", file=sys.stderr)
        return 2
    if args.badge_stdev_fail_threshold < args.badge_stdev_warn_threshold:
        print("error: --badge-stdev-fail-threshold should be >= --badge-stdev-warn-threshold", file=sys.stderr)
        return 2
    if args.badge_swing_fail_threshold < args.badge_swing_warn_threshold:
        print("error: --badge-swing-fail-threshold should be >= --badge-swing-warn-threshold", file=sys.stderr)
        return 2

    repo_root = Path(__file__).resolve().parent.parent
    tools_dir = repo_root / "tools"
    python_exe = sys.executable

    compare_script = tools_dir / "compare_fun_agent_sweeps.py"
    trend_script = tools_dir / "profile_fun_agent_history.py"
    comment_script = tools_dir / "build_fun_agent_ci_comment.py"

    output_dir = Path(args.output_dir)
    if not output_dir.is_absolute():
        output_dir = (repo_root / output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    delta_json = output_dir / args.delta_json_name
    trend_json = output_dir / args.trend_json_name
    markdown = output_dir / args.markdown_name
    summary_json = output_dir / args.summary_json_name if args.summary_json_name else None

    baseline = Path(args.baseline)
    candidate = Path(args.candidate)
    if not baseline.is_absolute():
        baseline = (repo_root / baseline).resolve()
    if not candidate.is_absolute():
        candidate = (repo_root / candidate).resolve()

    try:
        run_cmd(
            [
                python_exe,
                str(compare_script),
                "--baseline",
                str(baseline),
                "--candidate",
                str(candidate),
                "--out-json",
                str(delta_json),
            ],
            echo=bool(args.echo_commands),
            quiet=bool(args.quiet_subtools),
        )

        run_cmd(
            [
                python_exe,
                str(trend_script),
                "--inputs",
                str(args.trend_inputs),
                "--window",
                str(args.window),
                "--out-json",
                str(trend_json),
            ],
            echo=bool(args.echo_commands),
            quiet=bool(args.quiet_subtools),
        )

        run_cmd(
            [
                python_exe,
                str(comment_script),
                "--delta-json",
                str(delta_json),
                "--trend-json",
                str(trend_json),
                "--max-profiles",
                str(args.max_profiles),
                "--title",
                str(args.markdown_title),
                "--badge-mode",
                str(args.badge_mode),
                "--badge-delta-warn-threshold",
                str(args.badge_delta_warn_threshold),
                "--badge-delta-fail-threshold",
                str(args.badge_delta_fail_threshold),
                "--badge-stdev-warn-threshold",
                str(args.badge_stdev_warn_threshold),
                "--badge-stdev-fail-threshold",
                str(args.badge_stdev_fail_threshold),
                "--badge-swing-warn-threshold",
                str(args.badge_swing_warn_threshold),
                "--badge-swing-fail-threshold",
                str(args.badge_swing_fail_threshold),
                "--badge-unstable-level",
                str(args.badge_unstable_level),
                "--output",
                str(markdown),
            ],
            echo=bool(args.echo_commands),
            quiet=bool(args.quiet_subtools),
        )
    except Exception as exc:  # noqa: BLE001
        print(f"error: {exc}", file=sys.stderr)
        return 1

    if summary_json is not None:
        try:
            summary_payload = build_compact_summary(
                delta_path=delta_json,
                trend_path=trend_json,
                markdown_path=markdown,
                badge_mode=str(args.badge_mode),
                badge_delta_warn_threshold=float(args.badge_delta_warn_threshold),
                badge_delta_fail_threshold=float(args.badge_delta_fail_threshold),
                badge_stdev_warn_threshold=float(args.badge_stdev_warn_threshold),
                badge_stdev_fail_threshold=float(args.badge_stdev_fail_threshold),
                badge_swing_warn_threshold=float(args.badge_swing_warn_threshold),
                badge_swing_fail_threshold=float(args.badge_swing_fail_threshold),
                badge_unstable_level=str(args.badge_unstable_level),
            )
            summary_json.write_text(
                json.dumps(summary_payload, indent=2, sort_keys=True) + "\n",
                encoding="utf-8",
            )
            if not args.paths_only:
                print(f"Summary JSON: {summary_json}")
        except Exception as exc:  # noqa: BLE001
            print(f"error: failed writing summary JSON: {exc}", file=sys.stderr)
            return 1

    if args.paths_only:
        print(f"CI_COMMENT_MARKDOWN={markdown}")
        print(f"CI_DELTA_JSON={delta_json}")
        print(f"CI_TREND_JSON={trend_json}")
        if summary_json is not None:
            print(f"CI_SUMMARY_JSON={summary_json}")
    else:
        print(f"CI comment markdown: {markdown}")
        print(f"Delta JSON: {delta_json}")
        print(f"Trend JSON: {trend_json}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
