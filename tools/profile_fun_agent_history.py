#!/usr/bin/env python3
"""
Analyze fun-agent sweep history and flag unstable profile variance.

Supports inputs from:
  - sweep CSV files (from run_fun_agent_sweep.py --output)
  - summary JSON files (from run_fun_agent_sweep.py --summary-json)

Examples:
  python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.csv
  python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.json --window 6 --out-json assets/fun_trend.json
"""

from __future__ import annotations

import argparse
import csv
import glob
import json
import statistics
import sys
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Dict, List, Optional, Sequence, Tuple

SPARK_CHARS = "._-:=+*#%@"


def _safe_float(raw: object) -> Optional[float]:
    try:
        return float(raw)  # type: ignore[arg-type]
    except (TypeError, ValueError):
        return None


def _safe_int(raw: object, default: int = 0) -> int:
    try:
        return int(raw)  # type: ignore[arg-type]
    except (TypeError, ValueError):
        return default


def _parse_iso_utc(raw: object) -> Optional[datetime]:
    if not isinstance(raw, str):
        return None
    text = raw.strip()
    if not text:
        return None
    if text.endswith("Z"):
        text = text[:-1] + "+00:00"
    try:
        dt = datetime.fromisoformat(text)
    except ValueError:
        return None
    if dt.tzinfo is None:
        dt = dt.replace(tzinfo=timezone.utc)
    return dt.astimezone(timezone.utc)


@dataclass
class ProfileStats:
    count: int
    mean_fun_score: Optional[float]
    stdev_fun_score: Optional[float]
    min_fun_score: Optional[float]
    max_fun_score: Optional[float]


@dataclass
class Snapshot:
    path: Path
    label: str
    order_ts_utc: float
    generated_utc: str
    overall_mean_fun_score: Optional[float]
    per_profile: Dict[str, ProfileStats]


def _build_csv_profile_stats(path: Path) -> Tuple[Dict[str, ProfileStats], Optional[float]]:
    by_profile: Dict[str, List[float]] = {}
    all_scores: List[float] = []

    with path.open("r", encoding="utf-8-sig", newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            profile = (row.get("profile") or "").strip()
            score = _safe_float(row.get("fun_score"))
            if not profile or score is None:
                continue
            by_profile.setdefault(profile, []).append(score)
            all_scores.append(score)

    per_profile: Dict[str, ProfileStats] = {}
    for profile, scores in by_profile.items():
        stdev = statistics.pstdev(scores) if len(scores) > 1 else 0.0
        per_profile[profile] = ProfileStats(
            count=len(scores),
            mean_fun_score=statistics.fmean(scores),
            stdev_fun_score=stdev,
            min_fun_score=min(scores),
            max_fun_score=max(scores),
        )

    overall_mean = statistics.fmean(all_scores) if all_scores else None
    return per_profile, overall_mean


def _load_csv_snapshot(path: Path) -> Snapshot:
    per_profile, overall_mean = _build_csv_profile_stats(path)
    mtime = path.stat().st_mtime
    dt = datetime.fromtimestamp(mtime, tz=timezone.utc)
    return Snapshot(
        path=path,
        label=path.stem,
        order_ts_utc=dt.timestamp(),
        generated_utc=dt.isoformat(),
        overall_mean_fun_score=overall_mean,
        per_profile=per_profile,
    )


def _load_json_snapshot(path: Path) -> Snapshot:
    data = json.loads(path.read_text(encoding="utf-8-sig"))
    if not isinstance(data, dict):
        raise ValueError(f"summary JSON root must be object: {path}")

    generated = _parse_iso_utc(data.get("generated_utc"))
    if generated is None:
        generated = datetime.fromtimestamp(path.stat().st_mtime, tz=timezone.utc)

    overall_mean = None
    overall = data.get("overall")
    if isinstance(overall, dict):
        overall_mean = _safe_float(overall.get("mean_fun_score"))

    per_profile: Dict[str, ProfileStats] = {}
    raw_profiles = data.get("per_profile")
    if isinstance(raw_profiles, dict):
        for profile, raw_stats in raw_profiles.items():
            if not isinstance(profile, str) or not isinstance(raw_stats, dict):
                continue
            per_profile[profile] = ProfileStats(
                count=_safe_int(raw_stats.get("count"), default=0),
                mean_fun_score=_safe_float(raw_stats.get("mean_fun_score")),
                stdev_fun_score=_safe_float(raw_stats.get("stdev_fun_score")),
                min_fun_score=_safe_float(raw_stats.get("min_fun_score")),
                max_fun_score=_safe_float(raw_stats.get("max_fun_score")),
            )

    return Snapshot(
        path=path,
        label=path.stem,
        order_ts_utc=generated.timestamp(),
        generated_utc=generated.isoformat(),
        overall_mean_fun_score=overall_mean,
        per_profile=per_profile,
    )


def load_snapshot(path: Path) -> Snapshot:
    suffix = path.suffix.lower()
    if suffix == ".csv":
        return _load_csv_snapshot(path)
    if suffix == ".json":
        return _load_json_snapshot(path)

    try:
        return _load_json_snapshot(path)
    except Exception:
        return _load_csv_snapshot(path)


def _expand_input_token(token: str) -> List[Path]:
    resolved: List[Path] = []
    for raw_path in glob.glob(token):
        p = Path(raw_path)
        if p.is_file():
            resolved.append(p)
    if not resolved:
        p = Path(token)
        if p.is_file():
            resolved.append(p)
    return resolved


def resolve_input_paths(inputs: Sequence[str]) -> List[Path]:
    out: List[Path] = []
    seen = set()
    for token in inputs:
        token = token.strip()
        if not token:
            continue
        for p in _expand_input_token(token):
            key = str(p.resolve())
            if key in seen:
                continue
            seen.add(key)
            out.append(p)
    if not out:
        raise ValueError("no input files found")
    return out


def parse_profiles(raw: str) -> List[str]:
    values = [p.strip() for p in raw.split(",")]
    return [p for p in values if p]


def mean_abs_step(values: Sequence[float]) -> float:
    if len(values) < 2:
        return 0.0
    diffs = [abs(values[i] - values[i - 1]) for i in range(1, len(values))]
    return statistics.fmean(diffs)


def compute_window(values: Sequence[float]) -> Dict[str, float]:
    stdev = statistics.pstdev(values) if len(values) > 1 else 0.0
    return {
        "window_points": float(len(values)),
        "window_mean_fun_score": statistics.fmean(values),
        "window_stdev_fun_score": stdev,
        "window_min_fun_score": min(values),
        "window_max_fun_score": max(values),
        "window_swing_fun_score": max(values) - min(values),
        "window_delta_fun_score": values[-1] - values[0],
        "window_mean_abs_step": mean_abs_step(values),
    }


def downsample_series(values: Sequence[float], max_points: int) -> List[float]:
    if max_points <= 0:
        return []
    if len(values) <= max_points:
        return [float(v) for v in values]
    if max_points == 1:
        return [float(values[-1])]

    result: List[float] = []
    n = len(values)
    for i in range(max_points):
        idx = round(i * (n - 1) / (max_points - 1))
        result.append(float(values[idx]))
    return result


def build_ascii_sparkline(values: Sequence[float]) -> str:
    if not values:
        return ""
    if len(values) == 1:
        return "*"

    low = min(values)
    high = max(values)
    if high <= low:
        return "=" * len(values)

    chars: List[str] = []
    span = high - low
    bins = len(SPARK_CHARS) - 1
    for v in values:
        norm = (v - low) / span
        idx = int(round(norm * bins))
        if idx < 0:
            idx = 0
        if idx > bins:
            idx = bins
        chars.append(SPARK_CHARS[idx])
    return "".join(chars)


def _fmt(value: Optional[float], precision: int = 3) -> str:
    if value is None:
        return "n/a"
    return f"{value:.{precision}f}"


def _fmt_signed(value: Optional[float], precision: int = 2) -> str:
    if value is None:
        return "n/a"
    return f"{value:+.{precision}f}"


def build_payload(
    snapshots: Sequence[Snapshot],
    profiles_filter: Sequence[str],
    window: int,
    unstable_stdev_threshold: float,
    unstable_step_threshold: float,
    unstable_swing_threshold: float,
    spark_max_points: int,
    include_series: bool,
) -> Dict[str, object]:
    all_profiles = sorted(
        {
            profile
            for snap in snapshots
            for profile, stats in snap.per_profile.items()
            if stats.mean_fun_score is not None
        }
    )
    profiles = sorted(set(profiles_filter)) if profiles_filter else all_profiles

    per_profile: Dict[str, Dict[str, object]] = {}
    unstable_profiles: List[str] = []

    for profile in profiles:
        points: List[Tuple[str, float]] = []
        for snap in snapshots:
            stats = snap.per_profile.get(profile)
            if stats is None or stats.mean_fun_score is None:
                continue
            points.append((snap.label, float(stats.mean_fun_score)))

        values = [v for _, v in points]
        if not values:
            per_profile[profile] = {
                "points": 0,
                "status": "no-data",
                "unstable": False,
                "unstable_reasons": [],
            }
            continue

        used = values[-window:] if len(values) > window else values
        spark_values = downsample_series(used, spark_max_points)
        spark = build_ascii_sparkline(spark_values)
        metrics = compute_window(used)
        latest_mean = values[-1]
        reasons: List[str] = []

        if metrics["window_stdev_fun_score"] > unstable_stdev_threshold:
            reasons.append(
                f"rolling stdev {_fmt(metrics['window_stdev_fun_score'])} > {_fmt(unstable_stdev_threshold)}"
            )
        if metrics["window_mean_abs_step"] > unstable_step_threshold:
            reasons.append(
                f"rolling mean-step {_fmt(metrics['window_mean_abs_step'])} > {_fmt(unstable_step_threshold)}"
            )
        if metrics["window_swing_fun_score"] > unstable_swing_threshold:
            reasons.append(
                f"rolling swing {_fmt(metrics['window_swing_fun_score'])} > {_fmt(unstable_swing_threshold)}"
            )

        unstable = len(reasons) > 0
        if unstable:
            unstable_profiles.append(profile)

        profile_entry: Dict[str, object] = {
            "points": len(values),
            "latest_mean_fun_score": round(latest_mean, 6),
            "window_used": int(metrics["window_points"]),
            "window_mean_fun_score": round(metrics["window_mean_fun_score"], 6),
            "window_stdev_fun_score": round(metrics["window_stdev_fun_score"], 6),
            "window_min_fun_score": round(metrics["window_min_fun_score"], 6),
            "window_max_fun_score": round(metrics["window_max_fun_score"], 6),
            "window_swing_fun_score": round(metrics["window_swing_fun_score"], 6),
            "window_delta_fun_score": round(metrics["window_delta_fun_score"], 6),
            "window_mean_abs_step": round(metrics["window_mean_abs_step"], 6),
            "sparkline": spark,
            "spark_points": len(spark_values),
            "unstable": unstable,
            "unstable_reasons": reasons,
        }
        if include_series:
            profile_entry["series"] = [
                {"snapshot": snap_label, "mean_fun_score": round(score, 6)}
                for snap_label, score in points
            ]
        per_profile[profile] = profile_entry

    snapshot_entries = [
        {
            "label": snap.label,
            "path": str(snap.path),
            "generated_utc": snap.generated_utc,
            "overall_mean_fun_score": snap.overall_mean_fun_score,
            "profiles_present": sorted(snap.per_profile.keys()),
        }
        for snap in snapshots
    ]

    payload: Dict[str, object] = {
        "generated_utc": datetime.now(timezone.utc).isoformat(),
        "window": window,
        "thresholds": {
            "unstable_stdev_threshold": unstable_stdev_threshold,
            "unstable_step_threshold": unstable_step_threshold,
            "unstable_swing_threshold": unstable_swing_threshold,
        },
        "snapshot_count": len(snapshots),
        "profiles_analyzed": profiles,
        "unstable_profiles": sorted(unstable_profiles),
        "snapshots": snapshot_entries,
        "per_profile": per_profile,
    }
    return payload


def print_text_report(payload: Dict[str, object]) -> None:
    window = payload["window"]  # type: ignore[index]
    snapshot_count = payload["snapshot_count"]  # type: ignore[index]
    unstable_profiles = payload["unstable_profiles"]  # type: ignore[index]
    per_profile = payload["per_profile"]  # type: ignore[index]

    print("Profile Trend History")
    print("=====================")
    print(f"snapshots: {snapshot_count}")
    print(f"rolling window: {window}")
    print(f"unstable profiles: {len(unstable_profiles)}")
    if unstable_profiles:
        print("  " + ", ".join(unstable_profiles))
    print("")
    print("Per Profile")
    print("-----------")

    for profile in sorted(per_profile.keys()):
        item = per_profile[profile]
        if item.get("status") == "no-data":
            print(f"{profile:>10}: no data")
            continue

        latest = _fmt(item.get("latest_mean_fun_score"), precision=3)
        wmean = _fmt(item.get("window_mean_fun_score"), precision=3)
        wstdev = _fmt(item.get("window_stdev_fun_score"), precision=3)
        wstep = _fmt(item.get("window_mean_abs_step"), precision=3)
        swing = _fmt(item.get("window_swing_fun_score"), precision=3)
        marker = "UNSTABLE" if bool(item.get("unstable")) else "stable"
        spark = item.get("sparkline", "")
        print(
            f"{profile:>10}: latest={latest} roll_mean={wmean} "
            f"roll_stdev={wstdev} roll_step={wstep} roll_swing={swing} spark={spark} [{marker}]"
        )
        reasons = item.get("unstable_reasons")
        if isinstance(reasons, list):
            for reason in reasons:
                print(f"            - {reason}")


def write_csv_summary(path: Path, payload: Dict[str, object]) -> None:
    per_profile = payload["per_profile"]  # type: ignore[index]

    fieldnames = [
        "profile",
        "points",
        "latest_mean_fun_score",
        "window_used",
        "window_mean_fun_score",
        "window_stdev_fun_score",
        "window_min_fun_score",
        "window_max_fun_score",
        "window_swing_fun_score",
        "window_delta_fun_score",
        "window_mean_abs_step",
        "sparkline",
        "spark_points",
        "unstable",
        "unstable_reasons",
    ]

    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        for profile in sorted(per_profile.keys()):
            item = per_profile[profile]
            reasons = item.get("unstable_reasons", [])
            reason_text = ""
            if isinstance(reasons, list):
                reason_text = " | ".join(str(x) for x in reasons)
            writer.writerow(
                {
                    "profile": profile,
                    "points": item.get("points", 0),
                    "latest_mean_fun_score": item.get("latest_mean_fun_score"),
                    "window_used": item.get("window_used"),
                    "window_mean_fun_score": item.get("window_mean_fun_score"),
                    "window_stdev_fun_score": item.get("window_stdev_fun_score"),
                    "window_min_fun_score": item.get("window_min_fun_score"),
                    "window_max_fun_score": item.get("window_max_fun_score"),
                    "window_swing_fun_score": item.get("window_swing_fun_score"),
                    "window_delta_fun_score": item.get("window_delta_fun_score"),
                    "window_mean_abs_step": item.get("window_mean_abs_step"),
                    "sparkline": item.get("sparkline", ""),
                    "spark_points": item.get("spark_points", 0),
                    "unstable": "1" if bool(item.get("unstable")) else "0",
                    "unstable_reasons": reason_text,
                }
            )


def evaluate_unstable_gate(
    payload: Dict[str, object],
    max_unstable_profiles: int,
    allowed_unstable_profiles: Sequence[str],
) -> Tuple[bool, List[str]]:
    per_profile = payload["per_profile"]  # type: ignore[index]
    allow = set(allowed_unstable_profiles)
    errors: List[str] = []
    unstable_count = 0

    for profile in sorted(per_profile.keys()):
        item = per_profile[profile]
        if not bool(item.get("unstable")):
            continue
        if profile in allow:
            continue
        unstable_count += 1
        reasons = item.get("unstable_reasons", [])
        reason_text = ""
        if isinstance(reasons, list):
            reason_text = "; ".join(str(x) for x in reasons)
        if not reason_text:
            reason_text = "no reason text"
        errors.append(f"profile '{profile}' unstable: {reason_text}")

    if unstable_count > max_unstable_profiles:
        errors.insert(
            0,
            f"unstable profile count {unstable_count} exceeds allowed max {max_unstable_profiles}",
        )
    else:
        errors = []

    return (len(errors) == 0, errors)


def print_pr_summary(
    payload: Dict[str, object],
    gate_ok: bool,
    gate_enabled: bool,
    max_unstable_profiles: int,
    allowed_unstable_profiles: Sequence[str],
    max_profiles: int,
    include_reasons: bool,
) -> None:
    window = payload["window"]  # type: ignore[index]
    snapshot_count = payload["snapshot_count"]  # type: ignore[index]
    unstable_profiles = payload["unstable_profiles"]  # type: ignore[index]
    per_profile = payload["per_profile"]  # type: ignore[index]

    status = "PASS"
    if gate_enabled:
        status = "PASS" if gate_ok else "FAIL"
    elif unstable_profiles:
        status = "WARN"

    allow = ",".join(allowed_unstable_profiles) if allowed_unstable_profiles else "-"
    print(
        f"FUN_TREND status={status} unstable={len(unstable_profiles)} "
        f"profiles={len(per_profile)} window={window} snapshots={snapshot_count} "
        f"gate_max={max_unstable_profiles} allow={allow}"
    )
    print("profile,state,latest,delta,stdev,swing,spark")

    shown = 0
    for profile in sorted(per_profile.keys()):
        if shown >= max_profiles:
            break
        item = per_profile[profile]
        if item.get("status") == "no-data":
            print(f"{profile},no-data,n/a,n/a,n/a,n/a,")
            shown += 1
            continue
        state = "unstable" if bool(item.get("unstable")) else "stable"
        latest = _fmt(item.get("latest_mean_fun_score"), precision=2)
        delta = _fmt(item.get("window_delta_fun_score"), precision=2)
        stdev = _fmt(item.get("window_stdev_fun_score"), precision=2)
        swing = _fmt(item.get("window_swing_fun_score"), precision=2)
        spark = item.get("sparkline", "")
        print(f"{profile},{state},{latest},{delta},{stdev},{swing},{spark}")
        shown += 1
        if include_reasons and bool(item.get("unstable")):
            reasons = item.get("unstable_reasons", [])
            if isinstance(reasons, list):
                for reason in reasons:
                    print(f"{profile},reason,,,,,{str(reason).replace(',', ';')}")

    if len(per_profile) > shown:
        print(f"...,truncated,{shown}/{len(per_profile)},,,,")


def print_pr_markdown(
    payload: Dict[str, object],
    gate_ok: bool,
    gate_enabled: bool,
    max_unstable_profiles: int,
    allowed_unstable_profiles: Sequence[str],
    max_profiles: int,
    include_reasons: bool,
) -> None:
    window = payload["window"]  # type: ignore[index]
    snapshot_count = payload["snapshot_count"]  # type: ignore[index]
    unstable_profiles = payload["unstable_profiles"]  # type: ignore[index]
    per_profile = payload["per_profile"]  # type: ignore[index]

    status = "PASS"
    if gate_enabled:
        status = "PASS" if gate_ok else "FAIL"
    elif unstable_profiles:
        status = "WARN"

    allow = ",".join(allowed_unstable_profiles) if allowed_unstable_profiles else "-"
    print("### Fun Trend")
    print(
        f"`status={status} unstable={len(unstable_profiles)}/{len(per_profile)} "
        f"window={window} snapshots={snapshot_count} gate_max={max_unstable_profiles} allow={allow}`"
    )
    print("")
    print("| profile | state | latest | delta | stdev | swing | spark |")
    print("|---|---:|---:|---:|---:|---:|---|")

    shown = 0
    for profile in sorted(per_profile.keys()):
        if shown >= max_profiles:
            break
        item = per_profile[profile]
        if item.get("status") == "no-data":
            print(f"| `{profile}` | no-data | n/a | n/a | n/a | n/a |  |")
            shown += 1
            continue
        state = "unstable" if bool(item.get("unstable")) else "stable"
        latest = _fmt(item.get("latest_mean_fun_score"), precision=2)
        delta = _fmt_signed(item.get("window_delta_fun_score"), precision=2)
        stdev = _fmt(item.get("window_stdev_fun_score"), precision=2)
        swing = _fmt(item.get("window_swing_fun_score"), precision=2)
        spark = str(item.get("sparkline", ""))
        print(f"| `{profile}` | {state} | {latest} | {delta} | {stdev} | {swing} | `{spark}` |")
        shown += 1

    if len(per_profile) > shown:
        print(f"| `...` | truncated | {shown}/{len(per_profile)} |  |  |  |  |")

    if include_reasons:
        unstable_reason_lines: List[str] = []
        for profile in sorted(per_profile.keys()):
            item = per_profile[profile]
            if not bool(item.get("unstable")):
                continue
            reasons = item.get("unstable_reasons", [])
            if isinstance(reasons, list):
                for reason in reasons:
                    unstable_reason_lines.append(f"- `{profile}`: {reason}")
        if unstable_reason_lines:
            print("")
            print("Unstable reasons:")
            for line in unstable_reason_lines:
                print(line)


def main() -> int:
    parser = argparse.ArgumentParser(description="Analyze profile trend history across fun-agent sweep snapshots.")
    parser.add_argument(
        "--inputs",
        required=True,
        help="Comma-separated input files/globs (CSV and/or summary JSON).",
    )
    parser.add_argument("--profiles", help="Optional comma list of profiles to include.")
    parser.add_argument("--window", type=int, default=5, help="Rolling window size (snapshot count).")
    parser.add_argument(
        "--sort",
        choices=["time", "name"],
        default="time",
        help="Sort snapshots by generated time or file name.",
    )
    parser.add_argument(
        "--unstable-stdev-threshold",
        type=float,
        default=5.0,
        help="Flag profile unstable when rolling stdev exceeds this.",
    )
    parser.add_argument(
        "--unstable-step-threshold",
        type=float,
        default=4.0,
        help="Flag profile unstable when rolling mean absolute step exceeds this.",
    )
    parser.add_argument(
        "--unstable-swing-threshold",
        type=float,
        default=12.0,
        help="Flag profile unstable when rolling max-min exceeds this.",
    )
    parser.add_argument(
        "--spark-max-points",
        type=int,
        default=16,
        help="Max points used for per-profile sparkline rendering.",
    )
    parser.add_argument("--out-json", help="Optional JSON output file.")
    parser.add_argument("--out-csv", help="Optional CSV summary output file.")
    parser.add_argument("--json-stdout", action="store_true", help="Print JSON payload to stdout.")
    parser.add_argument(
        "--pr-summary",
        action="store_true",
        help="Print compact CI/PR summary output with sparkline history.",
    )
    parser.add_argument(
        "--pr-markdown",
        action="store_true",
        help="Print compact markdown summary suitable for direct PR comments.",
    )
    parser.add_argument(
        "--pr-summary-max-profiles",
        type=int,
        default=12,
        help="Max profiles printed in --pr-summary/--pr-markdown output.",
    )
    parser.add_argument(
        "--pr-summary-include-reasons",
        action="store_true",
        help="Include unstable reasons in --pr-summary/--pr-markdown output.",
    )
    parser.add_argument(
        "--fail-on-unstable",
        action="store_true",
        help="Exit non-zero when unstable profile count exceeds --max-unstable-profiles.",
    )
    parser.add_argument(
        "--max-unstable-profiles",
        type=int,
        default=0,
        help="Allowed unstable profile count for --fail-on-unstable checks.",
    )
    parser.add_argument(
        "--allow-unstable-profiles",
        help="Comma list of profiles to ignore in unstable CI gating.",
    )
    parser.add_argument(
        "--include-series",
        action="store_true",
        help="Include full per-profile history series in JSON payload.",
    )
    args = parser.parse_args()

    if args.window <= 0:
        print("error: --window must be > 0", file=sys.stderr)
        return 2
    if args.max_unstable_profiles < 0:
        print("error: --max-unstable-profiles must be >= 0", file=sys.stderr)
        return 2
    if args.spark_max_points <= 0:
        print("error: --spark-max-points must be > 0", file=sys.stderr)
        return 2
    if args.pr_summary_max_profiles <= 0:
        print("error: --pr-summary-max-profiles must be > 0", file=sys.stderr)
        return 2
    if args.pr_summary and args.pr_markdown:
        print("error: use only one of --pr-summary or --pr-markdown", file=sys.stderr)
        return 2

    try:
        tokens = [t.strip() for t in args.inputs.split(",") if t.strip()]
        paths = resolve_input_paths(tokens)
        snapshots = [load_snapshot(path) for path in paths]
        if args.sort == "time":
            snapshots.sort(key=lambda s: (s.order_ts_utc, s.path.name))
        else:
            snapshots.sort(key=lambda s: s.path.name.lower())
    except Exception as exc:  # noqa: BLE001
        print(f"error: {exc}", file=sys.stderr)
        return 2

    profiles_filter = parse_profiles(args.profiles) if args.profiles else []

    payload = build_payload(
        snapshots=snapshots,
        profiles_filter=profiles_filter,
        window=args.window,
        unstable_stdev_threshold=float(args.unstable_stdev_threshold),
        unstable_step_threshold=float(args.unstable_step_threshold),
        unstable_swing_threshold=float(args.unstable_swing_threshold),
        spark_max_points=int(args.spark_max_points),
        include_series=bool(args.include_series),
    )

    allowed = parse_profiles(args.allow_unstable_profiles) if args.allow_unstable_profiles else []
    gate_ok = True
    gate_errors: List[str] = []
    if args.fail_on_unstable:
        gate_ok, gate_errors = evaluate_unstable_gate(
            payload=payload,
            max_unstable_profiles=int(args.max_unstable_profiles),
            allowed_unstable_profiles=allowed,
        )

    if args.pr_summary:
        print_pr_summary(
            payload=payload,
            gate_ok=gate_ok,
            gate_enabled=bool(args.fail_on_unstable),
            max_unstable_profiles=int(args.max_unstable_profiles),
            allowed_unstable_profiles=allowed,
            max_profiles=int(args.pr_summary_max_profiles),
            include_reasons=bool(args.pr_summary_include_reasons),
        )
    elif args.pr_markdown:
        print_pr_markdown(
            payload=payload,
            gate_ok=gate_ok,
            gate_enabled=bool(args.fail_on_unstable),
            max_unstable_profiles=int(args.max_unstable_profiles),
            allowed_unstable_profiles=allowed,
            max_profiles=int(args.pr_summary_max_profiles),
            include_reasons=bool(args.pr_summary_include_reasons),
        )
    else:
        print_text_report(payload)

    if args.out_json:
        out_path = Path(args.out_json)
        out_path.parent.mkdir(parents=True, exist_ok=True)
        out_path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")
        print(f"Wrote trend JSON -> {out_path}")

    if args.out_csv:
        out_path = Path(args.out_csv)
        write_csv_summary(out_path, payload)
        print(f"Wrote trend CSV -> {out_path}")

    if args.json_stdout:
        print(json.dumps(payload, indent=2, sort_keys=True))

    if args.fail_on_unstable:
        if not gate_ok:
            for err in gate_errors:
                print(f"unstable-gate: {err}", file=sys.stderr)
            return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
