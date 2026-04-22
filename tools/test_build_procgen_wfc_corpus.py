#!/usr/bin/env python3
from __future__ import annotations

import json
import sys
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import build_procgen_wfc_corpus as wfc  # noqa: E402


GOOD_ROWS = [
    "####################",
    "#.........S...I....#",
    "#.....I.......I....#",
    "#..S....I..I##.I...#",
    "#......I.....II..I.#",
    "#.......I.I..S..I..#",
    "#....I........I...I#",
    "#..........I.......#",
    "#.................##",
    "#.....I.I.I.......##",
    "#......I........##.#",
    "####################",
]

ALT_ROWS = [
    "####################",
    "#.........#...I..I.#",
    "#.........#.....I.S#",
    "#....I....#....I...#",
    "#.........I........#",
    "#...I......I.......#",
    "#...........S.I..I.#",
    "#......SI..I.....I.#",
    "#I.............#I..#",
    "#..............#...#",
    "#..........I...#...#",
    "####################",
]


def make_sample(rows: list[str], profile: str, quality: float) -> dict:
    return {
        "rows": rows,
        "rows_original": rows,
        "profile": profile,
        "quality_score": quality,
        "cleared": True,
        "deaths": 0,
        "crushes": 3,
        "items": 1,
        "ticks_in_round": 1200,
        "time_start_ticks": 6000,
        "time_end_ticks": 4200,
        "seed": 1337,
        "run_index": 1,
        "round_index": 1,
        "stage_modifier": 0,
    }


def write_jsonl(path: Path, records: list[dict]) -> None:
    lines = [json.dumps(record, sort_keys=True) for record in records]
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


class BuildProcgenWfcCorpusTests(unittest.TestCase):
    def test_load_samples_validates_schema_and_normalizes_rows(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            valid_path = tmp / "valid.jsonl"
            invalid_path = tmp / "invalid.jsonl"

            rows_with_actor_markers = GOOD_ROWS.copy()
            rows_with_actor_markers[2] = "#.PI...E#....I.....#"
            rows_with_actor_markers[6] = "#......E...I.......#"

            write_jsonl(valid_path, [make_sample(rows_with_actor_markers, "arcade", 75.0)])
            valid = wfc.load_samples(valid_path)
            self.assertEqual(len(valid), 1)
            self.assertEqual(valid[0]["rows"][2][2], ".")
            self.assertEqual(valid[0]["rows"][2][7], ".")
            self.assertIn("map_id", valid[0])
            self.assertGreaterEqual(valid[0]["quality_score"], 0.0)
            self.assertLessEqual(valid[0]["quality_score"], 100.0)

            write_jsonl(invalid_path, [{"profile": "arcade"}])
            with self.assertRaises(ValueError):
                wfc.load_samples(invalid_path)

    def test_balanced_gate_filters_by_profile_means_and_counts(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            samples_path = tmp / "samples.jsonl"
            samples: list[dict] = []

            for profile in wfc.PROFILES:
                for _ in range(3):
                    samples.append(make_sample(GOOD_ROWS, profile, 72.0))

            for profile in wfc.PROFILES:
                bad_quality = 35.0 if profile == "arcade" else 70.0
                for _ in range(3):
                    samples.append(make_sample(ALT_ROWS, profile, bad_quality))

            write_jsonl(samples_path, samples)
            loaded = wfc.load_samples(samples_path)
            stats = wfc.build_map_stats(loaded)
            gated = wfc.gate_balanced_maps(
                stats,
                min_samples_per_profile=3,
                min_overall=50.0,
                min_profile_mean=40.0,
            )

            self.assertEqual(len(stats), 2)
            self.assertEqual(len(gated), 1)
            self.assertEqual(gated[0].rows, wfc.normalize_rows(GOOD_ROWS))

    def test_deterministic_model_and_output_generation(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            tmp = Path(td)
            samples_path = tmp / "samples.jsonl"
            header_a = tmp / "templates_a.h"
            header_b = tmp / "templates_b.h"

            samples: list[dict] = []
            for profile in wfc.PROFILES:
                for _ in range(3):
                    samples.append(make_sample(GOOD_ROWS, profile, 71.0))
                for _ in range(3):
                    samples.append(make_sample(ALT_ROWS, profile, 66.0))
            write_jsonl(samples_path, samples)

            loaded = wfc.load_samples(samples_path)
            stats = wfc.build_map_stats(loaded)
            gated = wfc.gate_balanced_maps(
                stats,
                min_samples_per_profile=3,
                min_overall=50.0,
                min_profile_mean=40.0,
            )
            self.assertGreaterEqual(len(gated), 1)

            model = wfc.build_pattern_model(gated, pattern_size=wfc.PATTERN_SIZE)
            templates_a = wfc.synthesize_templates(gated, model=model, target_templates=1, seed=2026)
            templates_b = wfc.synthesize_templates(gated, model=model, target_templates=1, seed=2026)
            self.assertEqual(templates_a, templates_b)

            wfc.write_header(header_a, templates_a)
            wfc.write_header(header_b, templates_b)
            self.assertEqual(
                header_a.read_text(encoding="utf-8"),
                header_b.read_text(encoding="utf-8"),
            )

            gate_cfg = {
                "mode": "balanced",
                "min_samples_per_profile": 3,
                "min_overall_quality": 50.0,
                "min_profile_quality": 40.0,
                "profiles_required": list(wfc.PROFILES),
                "learning_mode": "offline_corpus",
                "pattern_mode": "overlapping_wfc_3x3",
            }
            payload_a = wfc.build_corpus_payload(samples_path, stats, gated, model, templates_a, gate_cfg)
            payload_b = wfc.build_corpus_payload(samples_path, stats, gated, model, templates_b, gate_cfg)
            payload_a.pop("generated_utc", None)
            payload_b.pop("generated_utc", None)
            self.assertEqual(payload_a, payload_b)


if __name__ == "__main__":
    unittest.main()
