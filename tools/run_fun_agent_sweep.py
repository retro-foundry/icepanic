#!/usr/bin/env python3
"""
Run `icepanic_fun_agent` across profile/seed combinations and export one CSV.

Example:
  python tools/run_fun_agent_sweep.py --runs 24 --ticks 18000 --seed-count 12 --output assets/fun_sweep.csv
"""

from __future__ import annotations

import argparse
import csv
import io
import json
import os
import statistics
import subprocess
import sys
import tempfile
from datetime import datetime, timezone
from pathlib import Path
from typing import Dict, List


DEFAULT_PROFILES = ["arcade", "casual", "survival", "aggressive"]


def parse_profiles(raw: str) -> List[str]:
    parts = [p.strip() for p in raw.split(",")]
    return [p for p in parts if p]


def parse_seeds(raw: str) -> List[int]:
    out: List[int] = []
    for part in raw.split(","):
        token = part.strip()
        if not token:
            continue
        if ":" in token:
            bits = token.split(":")
            if len(bits) != 2:
                raise ValueError(f"invalid seed range token: {token}")
            start = int(bits[0])
            end = int(bits[1])
            step = 1 if end >= start else -1
            cur = start
            while True:
                out.append(cur)
                if cur == end:
                    break
                cur += step
        else:
            out.append(int(token))
    if not out:
        raise ValueError("no seeds parsed")
    return out


def resolve_agent_path(raw_path: str | None) -> Path:
    if raw_path:
        p = Path(raw_path)
        if not p.exists():
            raise FileNotFoundError(f"fun agent executable not found: {p}")
        return p

    candidates = [
        Path("build/Release/icepanic_fun_agent.exe"),
        Path("build/icepanic_fun_agent.exe"),
        Path("build/icepanic_fun_agent"),
    ]
    for c in candidates:
        if c.exists():
            return c
    raise FileNotFoundError(
        "could not find icepanic_fun_agent executable. "
        "Build first (cmake --build build --config Release) "
        "or pass --agent."
    )


def parse_agent_csv(stdout_text: str) -> Dict[str, str]:
    text = stdout_text.strip()
    if not text:
        raise ValueError("agent returned empty output")
    rows = list(csv.DictReader(io.StringIO(text)))
    if len(rows) != 1:
        raise ValueError(f"expected single CSV row from agent, got {len(rows)} rows")
    return rows[0]


def _safe_float(value: str, default: float = 0.0) -> float:
    try:
        return float(value)
    except (TypeError, ValueError):
        return default


def _safe_int(value: str, default: int = 0) -> int:
    try:
        return int(value)
    except (TypeError, ValueError):
        return default


def build_summary_payload(
    sweep_rows: List[Dict[str, str]],
    profiles: List[str],
    seeds: List[int],
    runs_per_eval: int,
    ticks_per_run: int,
    csv_path: Path,
    agent_path: Path,
) -> Dict[str, object]:
    per_profile: Dict[str, List[float]] = {}
    all_scores: List[float] = []

    for row in sweep_rows:
        profile = row.get("profile", "")
        if not profile:
            continue
        score = _safe_float(row.get("fun_score", "0"))
        all_scores.append(score)
        per_profile.setdefault(profile, []).append(score)

    profile_stats: Dict[str, Dict[str, object]] = {}
    for profile in profiles:
        scores = per_profile.get(profile, [])
        if not scores:
            profile_stats[profile] = {
                "count": 0,
                "mean_fun_score": None,
                "min_fun_score": None,
                "max_fun_score": None,
                "stdev_fun_score": None,
            }
            continue
        stdev = statistics.pstdev(scores) if len(scores) > 1 else 0.0
        profile_stats[profile] = {
            "count": len(scores),
            "mean_fun_score": round(statistics.fmean(scores), 6),
            "min_fun_score": min(scores),
            "max_fun_score": max(scores),
            "stdev_fun_score": round(stdev, 6),
        }

    best_row = max(sweep_rows, key=lambda r: _safe_float(r.get("fun_score", "0")))
    worst_row = min(sweep_rows, key=lambda r: _safe_float(r.get("fun_score", "0")))

    payload: Dict[str, object] = {
        "generated_utc": datetime.now(timezone.utc).isoformat(),
        "agent_executable": str(agent_path),
        "csv_output": str(csv_path),
        "rows": len(sweep_rows),
        "profiles": profiles,
        "seeds": seeds,
        "runs_per_eval": runs_per_eval,
        "ticks_per_run": ticks_per_run,
        "overall": {
            "mean_fun_score": round(statistics.fmean(all_scores), 6) if all_scores else None,
            "min_fun_score": min(all_scores) if all_scores else None,
            "max_fun_score": max(all_scores) if all_scores else None,
        },
        "per_profile": profile_stats,
        "best_case": {
            "profile": best_row.get("profile", ""),
            "seed": _safe_int(best_row.get("seed", "0")),
            "fun_score": _safe_float(best_row.get("fun_score", "0")),
            "avg_rounds": _safe_float(best_row.get("avg_rounds", "0")),
            "avg_score": _safe_float(best_row.get("avg_score", "0")),
        },
        "worst_case": {
            "profile": worst_row.get("profile", ""),
            "seed": _safe_int(worst_row.get("seed", "0")),
            "fun_score": _safe_float(worst_row.get("fun_score", "0")),
            "avg_rounds": _safe_float(worst_row.get("avg_rounds", "0")),
            "avg_score": _safe_float(worst_row.get("avg_score", "0")),
        },
    }
    return payload


def main() -> int:
    parser = argparse.ArgumentParser(description="Batch sweep helper for icepanic_fun_agent.")
    parser.add_argument("--agent", help="Path to icepanic_fun_agent executable.")
    parser.add_argument("--profiles", default=",".join(DEFAULT_PROFILES), help="Comma list of profiles.")
    parser.add_argument("--seeds", help="Comma list of seeds, supports ranges like 1337:1340.")
    parser.add_argument("--seed-start", type=int, default=1337, help="Start seed (used if --seeds omitted).")
    parser.add_argument("--seed-count", type=int, default=8, help="Number of seeds (used if --seeds omitted).")
    parser.add_argument("--seed-step", type=int, default=1, help="Seed step (used if --seeds omitted).")
    parser.add_argument("--runs", type=int, default=24, help="Runs per evaluation.")
    parser.add_argument("--ticks", type=int, default=18000, help="Max ticks per run.")
    parser.add_argument("--output", default="assets/fun_agent_sweep.csv", help="Output CSV path.")
    parser.add_argument("--summary-json", help="Optional JSON summary output path.")
    parser.add_argument(
        "--round-map-samples-out",
        help="Optional JSONL output path merging per-round map samples from each agent evaluation.",
    )
    parser.add_argument(
        "--summary-json-stdout",
        action="store_true",
        help="Also print JSON summary payload to stdout.",
    )
    parser.add_argument("--timeout", type=int, default=120, help="Subprocess timeout in seconds.")
    parser.add_argument("--echo-commands", action="store_true", help="Print each spawned command.")
    args = parser.parse_args()

    try:
        agent_path = resolve_agent_path(args.agent)
        profiles = parse_profiles(args.profiles)
        if not profiles:
            raise ValueError("no profiles configured")

        if args.seeds:
            seeds = parse_seeds(args.seeds)
        else:
            if args.seed_count <= 0:
                raise ValueError("--seed-count must be > 0")
            seeds = [args.seed_start + i * args.seed_step for i in range(args.seed_count)]
    except Exception as exc:  # noqa: BLE001
        print(f"error: {exc}", file=sys.stderr)
        return 2

    output_path = Path(args.output)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    merged_round_samples: List[str] = []
    round_samples_output_path: Path | None = None
    if args.round_map_samples_out:
        round_samples_output_path = Path(args.round_map_samples_out)
        round_samples_output_path.parent.mkdir(parents=True, exist_ok=True)

    sweep_rows: List[Dict[str, str]] = []
    sweep_index = 0

    for profile in profiles:
        for seed in seeds:
            temp_round_sample_path: str | None = None
            cmd = [
                str(agent_path),
                "--runs",
                str(args.runs),
                "--ticks",
                str(args.ticks),
                "--seed",
                str(seed),
                "--profile",
                profile,
                "--out-format",
                "csv",
            ]
            if round_samples_output_path is not None:
                fd, temp_round_sample_path = tempfile.mkstemp(prefix="icepanic_round_samples_", suffix=".jsonl")
                os.close(fd)
                cmd.extend(["--round-map-samples-out", temp_round_sample_path])
            if args.echo_commands:
                print("+", " ".join(cmd))

            try:
                completed = subprocess.run(
                    cmd,
                    check=True,
                    capture_output=True,
                    text=True,
                    timeout=args.timeout,
                )
            except subprocess.CalledProcessError as exc:
                print(f"error: agent command failed for profile={profile}, seed={seed}", file=sys.stderr)
                if exc.stdout:
                    print(exc.stdout, file=sys.stderr)
                if exc.stderr:
                    print(exc.stderr, file=sys.stderr)
                if temp_round_sample_path is not None:
                    Path(temp_round_sample_path).unlink(missing_ok=True)
                return 1
            except subprocess.TimeoutExpired:
                print(f"error: timeout for profile={profile}, seed={seed}", file=sys.stderr)
                if temp_round_sample_path is not None and not Path(temp_round_sample_path).exists():
                    temp_round_sample_path = None
                elif temp_round_sample_path is not None:
                    Path(temp_round_sample_path).unlink(missing_ok=True)
                return 1

            try:
                row = parse_agent_csv(completed.stdout)
            except Exception as exc:  # noqa: BLE001
                print(f"error: could not parse CSV for profile={profile}, seed={seed}: {exc}", file=sys.stderr)
                print("raw output:", file=sys.stderr)
                print(completed.stdout, file=sys.stderr)
                if temp_round_sample_path is not None:
                    Path(temp_round_sample_path).unlink(missing_ok=True)
                return 1

            sweep_index += 1
            row["sweep_index"] = str(sweep_index)
            row["requested_profile"] = profile
            row["requested_seed"] = str(seed)
            sweep_rows.append(row)

            if temp_round_sample_path is not None:
                temp_path = Path(temp_round_sample_path)
                if temp_path.exists():
                    for line in temp_path.read_text(encoding="utf-8").splitlines():
                        line = line.strip()
                        if line:
                            merged_round_samples.append(line)
                temp_path.unlink(missing_ok=True)

    if not sweep_rows:
        print("error: no sweep rows produced", file=sys.stderr)
        return 1

    base_field_order = [
        "sweep_index",
        "requested_profile",
        "requested_seed",
        "profile",
        "runs",
        "max_ticks_per_run",
        "seed",
        "fun_score",
        "fun_label",
        "max_round_reached",
        "game_over_runs",
        "avg_score",
        "avg_rounds",
        "avg_crushes",
        "avg_items",
        "avg_deaths",
        "active_ratio",
        "direction_change_rate",
        "stage_variety_ratio",
        "stage_unique_per_run",
        "stage_coverage_ratio",
        "score_target",
        "stage_variety_target_ratio",
        "comp_rounds",
        "comp_crushes",
        "comp_items",
        "comp_score",
        "comp_stages",
        "comp_agency",
        "comp_activity",
        "comp_challenge",
    ]

    seen = set()
    fieldnames: List[str] = []
    for name in base_field_order:
        if any(name in row for row in sweep_rows):
            fieldnames.append(name)
            seen.add(name)
    for row in sweep_rows:
        for k in row.keys():
            if k not in seen:
                fieldnames.append(k)
                seen.add(k)

    with output_path.open("w", encoding="utf-8", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(sweep_rows)

    if round_samples_output_path is not None:
        with round_samples_output_path.open("w", encoding="utf-8", newline="\n") as f:
            for line in merged_round_samples:
                f.write(line)
                f.write("\n")

    summary_payload = build_summary_payload(
        sweep_rows=sweep_rows,
        profiles=profiles,
        seeds=seeds,
        runs_per_eval=args.runs,
        ticks_per_run=args.ticks,
        csv_path=output_path,
        agent_path=agent_path,
    )

    if args.summary_json:
        summary_path = Path(args.summary_json)
        summary_path.parent.mkdir(parents=True, exist_ok=True)
        summary_path.write_text(
            json.dumps(summary_payload, indent=2, sort_keys=True) + "\n",
            encoding="utf-8",
        )
        print(f"Wrote summary JSON -> {summary_path}")

    if args.summary_json_stdout:
        print(json.dumps(summary_payload, indent=2, sort_keys=True))

    per_profile_scores: Dict[str, List[float]] = {}
    for row in sweep_rows:
        p = row.get("profile", "")
        if not p:
            continue
        try:
            score = float(row.get("fun_score", "0"))
        except ValueError:
            continue
        per_profile_scores.setdefault(p, []).append(score)

    print(f"Wrote {len(sweep_rows)} rows -> {output_path}")
    if round_samples_output_path is not None:
        print(f"Wrote {len(merged_round_samples)} round map samples -> {round_samples_output_path}")
    print(f"Profiles: {', '.join(profiles)}")
    print(f"Seeds: {len(seeds)} ({seeds[0]}..{seeds[-1]})")
    for profile in profiles:
        scores = per_profile_scores.get(profile, [])
        if not scores:
            print(f"  {profile}: no data")
            continue
        print(
            f"  {profile}: avg_fun={statistics.fmean(scores):.2f} "
            f"min={min(scores):.0f} max={max(scores):.0f}"
        )

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
