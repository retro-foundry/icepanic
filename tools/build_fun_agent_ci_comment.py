#!/usr/bin/env python3
"""
Build one markdown CI/PR comment from fun-agent delta and trend JSON artifacts.

Inputs:
  - delta JSON from tools/compare_fun_agent_sweeps.py --out-json
  - trend JSON from tools/profile_fun_agent_history.py --out-json
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Dict, List, Optional

SEVERITY_PASS = 0
SEVERITY_WARN = 1
SEVERITY_FAIL = 2


def _safe_float(raw: object) -> Optional[float]:
    try:
        return float(raw)  # type: ignore[arg-type]
    except (TypeError, ValueError):
        return None


def _fmt(raw: object, precision: int = 2) -> str:
    value = _safe_float(raw)
    if value is None:
        return "n/a"
    return f"{value:.{precision}f}"


def _fmt_signed(raw: object, precision: int = 2) -> str:
    value = _safe_float(raw)
    if value is None:
        return "n/a"
    return f"{value:+.{precision}f}"


def load_json(path: Path) -> Dict[str, object]:
    data = json.loads(path.read_text(encoding="utf-8-sig"))
    if not isinstance(data, dict):
        raise ValueError(f"JSON root must be an object: {path}")
    return data


def _severity_name(severity: int) -> str:
    if severity >= SEVERITY_FAIL:
        return "FAIL"
    if severity >= SEVERITY_WARN:
        return "WARN"
    return "PASS"


def _apply_threshold(
    severity: int,
    reasons: List[str],
    value: Optional[float],
    warn_cmp: str,
    warn_threshold: float,
    fail_cmp: str,
    fail_threshold: float,
    label: str,
) -> int:
    if value is None:
        return severity

    def hit(cmp_name: str, lhs: float, rhs: float) -> bool:
        if cmp_name == "<":
            return lhs < rhs
        if cmp_name == ">":
            return lhs > rhs
        raise ValueError(f"unsupported comparator: {cmp_name}")

    if hit(fail_cmp, value, fail_threshold):
        reasons.append(f"{label} {_fmt(value)} {fail_cmp} {_fmt(fail_threshold)}")
        return max(severity, SEVERITY_FAIL)
    if hit(warn_cmp, value, warn_threshold):
        reasons.append(f"{label} {_fmt(value)} {warn_cmp} {_fmt(warn_threshold)}")
        return max(severity, SEVERITY_WARN)
    return severity


def _profile_badge(
    delta_item: Dict[str, object],
    trend_item: Dict[str, object],
    delta_warn_threshold: float,
    delta_fail_threshold: float,
    stdev_warn_threshold: float,
    stdev_fail_threshold: float,
    swing_warn_threshold: float,
    swing_fail_threshold: float,
    unstable_level: str,
) -> Dict[str, object]:
    severity = SEVERITY_PASS
    reasons: List[str] = []

    d_mean = _safe_float(delta_item.get("delta_mean_fun_score"))
    t_stdev = _safe_float(trend_item.get("window_stdev_fun_score"))
    t_swing = _safe_float(trend_item.get("window_swing_fun_score"))
    unstable = bool(trend_item.get("unstable"))

    severity = _apply_threshold(
        severity=severity,
        reasons=reasons,
        value=d_mean,
        warn_cmp="<",
        warn_threshold=delta_warn_threshold,
        fail_cmp="<",
        fail_threshold=delta_fail_threshold,
        label="delta",
    )
    severity = _apply_threshold(
        severity=severity,
        reasons=reasons,
        value=t_stdev,
        warn_cmp=">",
        warn_threshold=stdev_warn_threshold,
        fail_cmp=">",
        fail_threshold=stdev_fail_threshold,
        label="stdev",
    )
    severity = _apply_threshold(
        severity=severity,
        reasons=reasons,
        value=t_swing,
        warn_cmp=">",
        warn_threshold=swing_warn_threshold,
        fail_cmp=">",
        fail_threshold=swing_fail_threshold,
        label="swing",
    )
    if unstable:
        level = SEVERITY_FAIL if unstable_level == "fail" else SEVERITY_WARN
        severity = max(severity, level)
        reasons.append(f"trend_state unstable ({unstable_level})")

    return {
        "severity": severity,
        "badge": _severity_name(severity),
        "reasons": reasons,
    }


def build_markdown(
    delta: Dict[str, object],
    trend: Dict[str, object],
    max_profiles: int,
    title: str,
    badge_mode: str,
    badge_delta_warn_threshold: float,
    badge_delta_fail_threshold: float,
    badge_stdev_warn_threshold: float,
    badge_stdev_fail_threshold: float,
    badge_swing_warn_threshold: float,
    badge_swing_fail_threshold: float,
    badge_unstable_level: str,
) -> str:
    delta_block = delta.get("delta", {})
    if not isinstance(delta_block, dict):
        delta_block = {}

    baseline = delta.get("baseline", {})
    candidate = delta.get("candidate", {})
    profiles_delta = delta.get("profiles", {})
    if not isinstance(baseline, dict):
        baseline = {}
    if not isinstance(candidate, dict):
        candidate = {}
    if not isinstance(profiles_delta, dict):
        profiles_delta = {}

    trend_profiles = trend.get("per_profile", {})
    if not isinstance(trend_profiles, dict):
        trend_profiles = {}
    unstable_profiles = trend.get("unstable_profiles", [])
    if not isinstance(unstable_profiles, list):
        unstable_profiles = []

    overall_delta = _safe_float(delta_block.get("overall_mean_fun_score"))
    delta_state = "improved"
    if overall_delta is not None and overall_delta < 0:
        delta_state = "regressed"

    window = trend.get("window", "n/a")
    snapshots = trend.get("snapshot_count", "n/a")

    lines: List[str] = []
    profiles = sorted(set(profiles_delta.keys()) | set(trend_profiles.keys()))
    badge_summary = {
        "PASS": 0,
        "WARN": 0,
        "FAIL": 0,
    }
    badge_by_profile: Dict[str, Dict[str, object]] = {}
    for profile in profiles:
        delta_item = profiles_delta.get(profile, {})
        trend_item = trend_profiles.get(profile, {})
        if not isinstance(delta_item, dict):
            delta_item = {}
        if not isinstance(trend_item, dict):
            trend_item = {}
        badge = _profile_badge(
            delta_item=delta_item,
            trend_item=trend_item,
            delta_warn_threshold=badge_delta_warn_threshold,
            delta_fail_threshold=badge_delta_fail_threshold,
            stdev_warn_threshold=badge_stdev_warn_threshold,
            stdev_fail_threshold=badge_stdev_fail_threshold,
            swing_warn_threshold=badge_swing_warn_threshold,
            swing_fail_threshold=badge_swing_fail_threshold,
            unstable_level=badge_unstable_level,
        )
        badge_by_profile[profile] = badge
        badge_name = str(badge["badge"])
        if badge_name in badge_summary:
            badge_summary[badge_name] += 1

    lines.append(f"### {title}")
    if badge_mode == "text":
        lines.append(
            f"`overall_delta={_fmt_signed(overall_delta)} ({delta_state}) "
            f"unstable_profiles={len(unstable_profiles)} window={window} snapshots={snapshots} "
            f"badges=PASS:{badge_summary['PASS']} WARN:{badge_summary['WARN']} FAIL:{badge_summary['FAIL']}`"
        )
    else:
        lines.append(
            f"`overall_delta={_fmt_signed(overall_delta)} ({delta_state}) "
            f"unstable_profiles={len(unstable_profiles)} window={window} snapshots={snapshots}`"
        )
    lines.append("")
    lines.append(
        f"- Baseline: `{baseline.get('label', 'baseline')}` mean={_fmt(baseline.get('overall_mean_fun_score'))}"
    )
    lines.append(
        f"- Candidate: `{candidate.get('label', 'candidate')}` mean={_fmt(candidate.get('overall_mean_fun_score'))}"
    )
    lines.append("")
    if badge_mode == "text":
        lines.append("| profile | badge | delta_mean | trend_state | trend_stdev | trend_swing | spark |")
        lines.append("|---|---:|---:|---:|---:|---:|---|")
    else:
        lines.append("| profile | delta_mean | trend_state | trend_stdev | trend_swing | spark |")
        lines.append("|---|---:|---:|---:|---:|---|")

    shown = 0
    for profile in profiles:
        if shown >= max_profiles:
            break
        delta_item = profiles_delta.get(profile, {})
        trend_item = trend_profiles.get(profile, {})
        if not isinstance(delta_item, dict):
            delta_item = {}
        if not isinstance(trend_item, dict):
            trend_item = {}
        d_mean = _fmt_signed(delta_item.get("delta_mean_fun_score"))
        trend_state = "unstable" if bool(trend_item.get("unstable")) else "stable"
        t_stdev = _fmt(trend_item.get("window_stdev_fun_score"))
        t_swing = _fmt(trend_item.get("window_swing_fun_score"))
        spark = str(trend_item.get("sparkline", ""))
        if badge_mode == "text":
            badge = str(badge_by_profile.get(profile, {}).get("badge", "PASS"))
            lines.append(
                f"| `{profile}` | {badge} | {d_mean} | {trend_state} | {t_stdev} | {t_swing} | `{spark}` |"
            )
        else:
            lines.append(
                f"| `{profile}` | {d_mean} | {trend_state} | {t_stdev} | {t_swing} | `{spark}` |"
            )
        shown += 1
    if len(profiles) > shown:
        if badge_mode == "text":
            lines.append(f"| `...` | truncated | {shown}/{len(profiles)} |  |  |  |  |")
        else:
            lines.append(f"| `...` | truncated | {shown}/{len(profiles)} |  |  |  |")

    if unstable_profiles:
        lines.append("")
        lines.append("Unstable profiles:")
        for profile in unstable_profiles:
            p = str(profile)
            item = trend_profiles.get(p, {})
            if not isinstance(item, dict):
                item = {}
            reasons = item.get("unstable_reasons", [])
            if isinstance(reasons, list) and reasons:
                lines.append(f"- `{p}`: {str(reasons[0])}")
            else:
                lines.append(f"- `{p}`")

    return "\n".join(lines) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser(description="Build one markdown CI/PR comment from fun-agent delta + trend JSON.")
    parser.add_argument("--delta-json", required=True, help="Path to compare_fun_agent_sweeps delta JSON.")
    parser.add_argument("--trend-json", required=True, help="Path to profile_fun_agent_history trend JSON.")
    parser.add_argument("--max-profiles", type=int, default=12, help="Max profile rows in markdown table.")
    parser.add_argument("--title", default="Fun Agent CI Summary", help="Markdown heading text.")
    parser.add_argument(
        "--badge-mode",
        choices=["off", "text"],
        default="off",
        help="Optional profile row badge mode.",
    )
    parser.add_argument(
        "--badge-delta-warn-threshold",
        type=float,
        default=-1.0,
        help="Warn when per-profile delta mean is below this.",
    )
    parser.add_argument(
        "--badge-delta-fail-threshold",
        type=float,
        default=-3.0,
        help="Fail when per-profile delta mean is below this.",
    )
    parser.add_argument(
        "--badge-stdev-warn-threshold",
        type=float,
        default=4.0,
        help="Warn when profile trend stdev is above this.",
    )
    parser.add_argument(
        "--badge-stdev-fail-threshold",
        type=float,
        default=6.0,
        help="Fail when profile trend stdev is above this.",
    )
    parser.add_argument(
        "--badge-swing-warn-threshold",
        type=float,
        default=10.0,
        help="Warn when profile trend swing is above this.",
    )
    parser.add_argument(
        "--badge-swing-fail-threshold",
        type=float,
        default=14.0,
        help="Fail when profile trend swing is above this.",
    )
    parser.add_argument(
        "--badge-unstable-level",
        choices=["warn", "fail"],
        default="fail",
        help="Badge severity applied when trend marks profile unstable.",
    )
    parser.add_argument("--output", help="Optional markdown output file path.")
    args = parser.parse_args()

    if args.max_profiles <= 0:
        print("error: --max-profiles must be > 0", file=sys.stderr)
        return 2
    if not str(args.title).strip():
        print("error: --title must not be empty", file=sys.stderr)
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

    try:
        delta_path = Path(args.delta_json)
        trend_path = Path(args.trend_json)
        if not delta_path.exists():
            raise FileNotFoundError(f"delta JSON not found: {delta_path}")
        if not trend_path.exists():
            raise FileNotFoundError(f"trend JSON not found: {trend_path}")
        delta = load_json(delta_path)
        trend = load_json(trend_path)
        markdown = build_markdown(
            delta=delta,
            trend=trend,
            max_profiles=int(args.max_profiles),
            title=str(args.title),
            badge_mode=str(args.badge_mode),
            badge_delta_warn_threshold=float(args.badge_delta_warn_threshold),
            badge_delta_fail_threshold=float(args.badge_delta_fail_threshold),
            badge_stdev_warn_threshold=float(args.badge_stdev_warn_threshold),
            badge_stdev_fail_threshold=float(args.badge_stdev_fail_threshold),
            badge_swing_warn_threshold=float(args.badge_swing_warn_threshold),
            badge_swing_fail_threshold=float(args.badge_swing_fail_threshold),
            badge_unstable_level=str(args.badge_unstable_level),
        )
    except Exception as exc:  # noqa: BLE001
        print(f"error: {exc}", file=sys.stderr)
        return 2

    if args.output:
        out_path = Path(args.output)
        out_path.parent.mkdir(parents=True, exist_ok=True)
        out_path.write_text(markdown, encoding="utf-8")
        print(f"Wrote markdown -> {out_path}")
    else:
        sys.stdout.write(markdown)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
