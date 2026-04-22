#!/usr/bin/env python3
"""
Compare two fun-agent sweep outputs (CSV or summary JSON) and report deltas.

Examples:
  python tools/compare_fun_agent_sweeps.py --baseline assets/fun_old.csv --candidate assets/fun_new.csv
  python tools/compare_fun_agent_sweeps.py --baseline assets/fun_old_summary.json --candidate assets/fun_new_summary.json --out-json assets/fun_delta.json
"""

from __future__ import annotations

import argparse
import csv
import json
import statistics
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple


class Snapshot:
    def __init__(self, source: Path) -> None:
        self.source = source
        self.rows = 0
        self.profiles: Dict[str, Dict[str, float]] = {}
        self.overall_mean_fun: Optional[float] = None
        self.overall_min_fun: Optional[float] = None
        self.overall_max_fun: Optional[float] = None


def _safe_float(raw: object) -> Optional[float]:
    try:
        return float(raw)  # type: ignore[arg-type]
    except (TypeError, ValueError):
        return None


def _load_csv_snapshot(path: Path) -> Snapshot:
    snapshot = Snapshot(path)
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
            snapshot.rows += 1

    for profile, scores in by_profile.items():
        stdev = statistics.pstdev(scores) if len(scores) > 1 else 0.0
        snapshot.profiles[profile] = {
            "count": float(len(scores)),
            "mean_fun_score": statistics.fmean(scores),
            "min_fun_score": min(scores),
            "max_fun_score": max(scores),
            "stdev_fun_score": stdev,
        }

    if all_scores:
        snapshot.overall_mean_fun = statistics.fmean(all_scores)
        snapshot.overall_min_fun = min(all_scores)
        snapshot.overall_max_fun = max(all_scores)

    return snapshot


def _load_json_snapshot(path: Path) -> Snapshot:
    snapshot = Snapshot(path)
    data = json.loads(path.read_text(encoding="utf-8-sig"))

    if not isinstance(data, dict):
        raise ValueError("summary JSON root must be an object")

    rows = data.get("rows")
    if isinstance(rows, int):
        snapshot.rows = rows

    overall = data.get("overall")
    if isinstance(overall, dict):
        snapshot.overall_mean_fun = _safe_float(overall.get("mean_fun_score"))
        snapshot.overall_min_fun = _safe_float(overall.get("min_fun_score"))
        snapshot.overall_max_fun = _safe_float(overall.get("max_fun_score"))

    per_profile = data.get("per_profile")
    if isinstance(per_profile, dict):
        for profile, raw_stats in per_profile.items():
            if not isinstance(profile, str) or not isinstance(raw_stats, dict):
                continue
            mean = _safe_float(raw_stats.get("mean_fun_score"))
            min_score = _safe_float(raw_stats.get("min_fun_score"))
            max_score = _safe_float(raw_stats.get("max_fun_score"))
            count = _safe_float(raw_stats.get("count"))
            stdev = _safe_float(raw_stats.get("stdev_fun_score"))

            if mean is None and min_score is None and max_score is None:
                continue

            snapshot.profiles[profile] = {
                "count": 0.0 if count is None else count,
                "mean_fun_score": 0.0 if mean is None else mean,
                "min_fun_score": 0.0 if min_score is None else min_score,
                "max_fun_score": 0.0 if max_score is None else max_score,
                "stdev_fun_score": 0.0 if stdev is None else stdev,
            }

    return snapshot


def load_snapshot(path: Path) -> Snapshot:
    if not path.exists():
        raise FileNotFoundError(f"snapshot not found: {path}")

    if path.suffix.lower() == ".json":
        return _load_json_snapshot(path)
    if path.suffix.lower() == ".csv":
        return _load_csv_snapshot(path)

    try:
        return _load_json_snapshot(path)
    except Exception:
        return _load_csv_snapshot(path)


def _fmt_optional(value: Optional[float], precision: int = 3) -> str:
    if value is None:
        return "n/a"
    return f"{value:.{precision}f}"


def _delta(a: Optional[float], b: Optional[float]) -> Optional[float]:
    if a is None or b is None:
        return None
    return b - a


def build_delta_payload(
    baseline: Snapshot,
    candidate: Snapshot,
    baseline_label: str,
    candidate_label: str,
) -> Dict[str, object]:
    profiles = sorted(set(baseline.profiles.keys()) | set(candidate.profiles.keys()))

    per_profile: Dict[str, Dict[str, object]] = {}
    for profile in profiles:
        b_stats = baseline.profiles.get(profile)
        c_stats = candidate.profiles.get(profile)
        b_mean = None if b_stats is None else b_stats.get("mean_fun_score")
        c_mean = None if c_stats is None else c_stats.get("mean_fun_score")
        per_profile[profile] = {
            "baseline_mean_fun_score": b_mean,
            "candidate_mean_fun_score": c_mean,
            "delta_mean_fun_score": _delta(
                None if b_mean is None else float(b_mean),
                None if c_mean is None else float(c_mean),
            ),
            "baseline_count": None if b_stats is None else b_stats.get("count"),
            "candidate_count": None if c_stats is None else c_stats.get("count"),
        }

    payload: Dict[str, object] = {
        "baseline": {
            "label": baseline_label,
            "path": str(baseline.source),
            "rows": baseline.rows,
            "overall_mean_fun_score": baseline.overall_mean_fun,
            "overall_min_fun_score": baseline.overall_min_fun,
            "overall_max_fun_score": baseline.overall_max_fun,
        },
        "candidate": {
            "label": candidate_label,
            "path": str(candidate.source),
            "rows": candidate.rows,
            "overall_mean_fun_score": candidate.overall_mean_fun,
            "overall_min_fun_score": candidate.overall_min_fun,
            "overall_max_fun_score": candidate.overall_max_fun,
        },
        "delta": {
            "overall_mean_fun_score": _delta(baseline.overall_mean_fun, candidate.overall_mean_fun),
            "rows": candidate.rows - baseline.rows,
        },
        "profiles": per_profile,
    }
    return payload


def print_text_report(payload: Dict[str, object]) -> None:
    baseline = payload["baseline"]  # type: ignore[index]
    candidate = payload["candidate"]  # type: ignore[index]
    delta = payload["delta"]  # type: ignore[index]
    profiles = payload["profiles"]  # type: ignore[index]

    print("Fun Sweep Delta")
    print("==============")
    print(
        f"baseline: {baseline['label']} ({baseline['path']}) rows={baseline['rows']} "
        f"overall_mean={_fmt_optional(baseline.get('overall_mean_fun_score'))}"
    )
    print(
        f"candidate: {candidate['label']} ({candidate['path']}) rows={candidate['rows']} "
        f"overall_mean={_fmt_optional(candidate.get('overall_mean_fun_score'))}"
    )
    overall_delta = _fmt_optional(delta.get("overall_mean_fun_score"), precision=3)
    print(f"delta overall_mean: {overall_delta}")
    print("")
    print("Per Profile Mean Delta")
    print("----------------------")
    for profile in sorted(profiles.keys()):
        item = profiles[profile]
        b_mean = _fmt_optional(item.get("baseline_mean_fun_score"), precision=3)
        c_mean = _fmt_optional(item.get("candidate_mean_fun_score"), precision=3)
        d_mean = _fmt_optional(item.get("delta_mean_fun_score"), precision=3)
        print(f"{profile:>10}: {b_mean} -> {c_mean}  delta={d_mean}")


def evaluate_regression(
    payload: Dict[str, object],
    overall_threshold: float,
    per_profile_threshold: float,
) -> Tuple[bool, List[str]]:
    errors: List[str] = []
    delta = payload["delta"]  # type: ignore[index]
    profiles = payload["profiles"]  # type: ignore[index]

    overall_delta = _safe_float(delta.get("overall_mean_fun_score"))
    if overall_delta is not None and overall_delta < overall_threshold:
        errors.append(
            f"overall mean delta {overall_delta:.3f} is below threshold {overall_threshold:.3f}"
        )

    for profile, item in profiles.items():
        d = _safe_float(item.get("delta_mean_fun_score"))
        if d is None:
            continue
        if d < per_profile_threshold:
            errors.append(
                f"profile '{profile}' mean delta {d:.3f} is below threshold {per_profile_threshold:.3f}"
            )

    return (len(errors) == 0, errors)


def main() -> int:
    parser = argparse.ArgumentParser(description="Compare two fun-agent sweep snapshots (CSV or summary JSON).")
    parser.add_argument("--baseline", required=True, help="Baseline snapshot path (CSV or JSON).")
    parser.add_argument("--candidate", required=True, help="Candidate snapshot path (CSV or JSON).")
    parser.add_argument("--baseline-label", default="baseline", help="Label for baseline report.")
    parser.add_argument("--candidate-label", default="candidate", help="Label for candidate report.")
    parser.add_argument("--out-json", help="Optional JSON output path for delta payload.")
    parser.add_argument("--json-stdout", action="store_true", help="Print JSON payload to stdout.")
    parser.add_argument("--fail-on-regression", action="store_true", help="Exit non-zero when thresholds are violated.")
    parser.add_argument(
        "--overall-threshold",
        type=float,
        default=0.0,
        help="Fail threshold for overall mean delta (candidate - baseline).",
    )
    parser.add_argument(
        "--profile-threshold",
        type=float,
        default=0.0,
        help="Fail threshold for per-profile mean delta (candidate - baseline).",
    )
    args = parser.parse_args()

    try:
        baseline_path = Path(args.baseline)
        candidate_path = Path(args.candidate)
        baseline = load_snapshot(baseline_path)
        candidate = load_snapshot(candidate_path)
        payload = build_delta_payload(
            baseline=baseline,
            candidate=candidate,
            baseline_label=args.baseline_label,
            candidate_label=args.candidate_label,
        )
    except Exception as exc:  # noqa: BLE001
        print(f"error: {exc}", file=sys.stderr)
        return 2

    if args.out_json:
        out_path = Path(args.out_json)
        out_path.parent.mkdir(parents=True, exist_ok=True)
        out_path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")
        print(f"Wrote delta JSON -> {out_path}")

    if args.json_stdout:
        print(json.dumps(payload, indent=2, sort_keys=True))
    else:
        print_text_report(payload)

    if args.fail_on_regression:
        ok, errors = evaluate_regression(
            payload=payload,
            overall_threshold=args.overall_threshold,
            per_profile_threshold=args.profile_threshold,
        )
        if not ok:
            for err in errors:
                print(f"regression: {err}", file=sys.stderr)
            return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
