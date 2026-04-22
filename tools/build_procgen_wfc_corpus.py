#!/usr/bin/env python3
"""
Build a bot-informed pattern-WFC corpus and runtime template header for Icepanic.

Inputs:
  - JSONL round-map samples from `icepanic_fun_agent --round-map-samples-out`.

Outputs:
  - Machine-readable corpus JSON.
  - Human-readable markdown notes.
  - Generated C header with deterministic template bank.
"""

from __future__ import annotations

import argparse
import json
import math
import random
from collections import defaultdict, deque
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Dict, Iterable, List, Sequence, Tuple


GRID_W = 20
GRID_H = 12
INTERIOR_W = GRID_W - 2
INTERIOR_H = GRID_H - 2
PATTERN_SIZE = 3
PROFILES = ("arcade", "casual", "survival", "aggressive")
ALLOWED_TEMPLATE_SYMBOLS = {"#", ".", "I", "S", "C"}


@dataclass
class MapStats:
    map_id: str
    rows: List[str]
    rows_original: List[str]
    sample_count: int
    profile_counts: Dict[str, int]
    profile_mean_quality: Dict[str, float]
    overall_mean_quality: float
    quality_sum: float


def canonicalize_symbol(ch: str) -> str:
    if ch in ("P", "E"):
        return "."
    if ch == "U":
        return "#"
    if ch == "C":
        return "C"
    if ch in ("#", ".", "I", "S"):
        return ch
    return "."


def normalize_rows(rows: Sequence[str]) -> List[str]:
    if len(rows) != GRID_H:
        raise ValueError(f"expected {GRID_H} rows, got {len(rows)}")
    out: List[str] = []
    for row in rows:
        if len(row) != GRID_W:
            raise ValueError(f"expected row width {GRID_W}, got {len(row)}")
        out.append("".join(canonicalize_symbol(c) for c in row))
    return out


def hash_rows(rows: Sequence[str]) -> str:
    h = 1469598103934665603
    for row in rows:
        for ch in row:
            h ^= ord(ch) & 0xFF
            h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
        h ^= ord("\n")
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return f"{h:016x}"


def clamp(v: float, lo: float, hi: float) -> float:
    return max(lo, min(hi, v))


def compute_quality_score(sample: dict) -> float:
    if "quality_score" in sample:
        try:
            return clamp(float(sample["quality_score"]), 0.0, 100.0)
        except (TypeError, ValueError):
            pass

    cleared = bool(sample.get("cleared", False))
    deaths = int(sample.get("deaths", 0))
    crushes = int(sample.get("crushes", 0))
    items = int(sample.get("items", 0))
    time_start = max(int(sample.get("time_start_ticks", 1)), 1)
    time_end = int(sample.get("time_end_ticks", 0))

    score = 0.0
    if cleared:
        score += 45.0
    score += 20.0 * clamp(crushes / 6.0, 0.0, 1.0)
    score += 10.0 * clamp(items / 2.0, 0.0, 1.0)
    score += 20.0 * clamp(time_end / float(time_start), 0.0, 1.0)
    if deaths == 0:
        score += 5.0
    return clamp(score, 0.0, 100.0)


def load_samples(path: Path) -> List[dict]:
    samples: List[dict] = []
    for line_no, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        text = raw.strip()
        if not text:
            continue
        try:
            sample = json.loads(text)
        except json.JSONDecodeError as exc:
            raise ValueError(f"{path}:{line_no}: invalid JSON: {exc}") from exc

        rows = sample.get("rows")
        rows_original = sample.get("rows_original", rows)
        if not isinstance(rows, list):
            raise ValueError(f"{path}:{line_no}: missing rows array")
        if not isinstance(rows_original, list):
            raise ValueError(f"{path}:{line_no}: missing rows_original array")

        sample["rows"] = normalize_rows(rows)
        sample["rows_original"] = normalize_rows(rows_original)
        sample["profile"] = str(sample.get("profile", "")).strip()
        if sample["profile"] not in PROFILES:
            raise ValueError(f"{path}:{line_no}: invalid profile '{sample['profile']}'")
        sample["quality_score"] = compute_quality_score(sample)
        if "map_id" not in sample or not str(sample["map_id"]).strip():
            sample["map_id"] = hash_rows(sample["rows"])
        else:
            sample["map_id"] = str(sample["map_id"]).lower()
        samples.append(sample)

    if not samples:
        raise ValueError(f"{path}: no samples found")
    return samples


def build_map_stats(samples: Sequence[dict]) -> List[MapStats]:
    grouped: Dict[str, dict] = {}
    for sample in samples:
        map_id = sample["map_id"]
        entry = grouped.setdefault(
            map_id,
            {
                "rows": sample["rows"],
                "rows_original": sample["rows_original"],
                "qualities": [],
                "per_profile": defaultdict(list),
            },
        )
        entry["qualities"].append(float(sample["quality_score"]))
        entry["per_profile"][sample["profile"]].append(float(sample["quality_score"]))

    out: List[MapStats] = []
    for map_id, entry in grouped.items():
        qualities = entry["qualities"]
        profile_counts: Dict[str, int] = {}
        profile_means: Dict[str, float] = {}
        for profile in PROFILES:
            values = entry["per_profile"].get(profile, [])
            profile_counts[profile] = len(values)
            profile_means[profile] = (sum(values) / len(values)) if values else 0.0
        out.append(
            MapStats(
                map_id=map_id,
                rows=list(entry["rows"]),
                rows_original=list(entry["rows_original"]),
                sample_count=len(qualities),
                profile_counts=profile_counts,
                profile_mean_quality=profile_means,
                overall_mean_quality=(sum(qualities) / len(qualities)),
                quality_sum=sum(qualities),
            )
        )

    out.sort(key=lambda m: (m.overall_mean_quality, m.sample_count, m.map_id), reverse=True)
    return out


def gate_balanced_maps(
    maps: Sequence[MapStats],
    min_samples_per_profile: int,
    min_overall: float,
    min_profile_mean: float,
) -> List[MapStats]:
    gated: List[MapStats] = []
    for m in maps:
        if m.overall_mean_quality < min_overall:
            continue
        ok = True
        for profile in PROFILES:
            if m.profile_counts.get(profile, 0) < min_samples_per_profile:
                ok = False
                break
            if m.profile_mean_quality.get(profile, 0.0) < min_profile_mean:
                ok = False
                break
        if ok:
            gated.append(m)
    return gated


def count_walkable_neighbors(rows: Sequence[str], x: int, y: int) -> int:
    count = 0
    for dx, dy in ((0, -1), (0, 1), (-1, 0), (1, 0)):
        nx = x + dx
        ny = y + dy
        if nx < 1 or ny < 1 or nx >= GRID_W - 1 or ny >= GRID_H - 1:
            continue
        if rows[ny][nx] == ".":
            count += 1
    return count


def count_adjacent_breakable(rows: Sequence[str], x: int, y: int) -> int:
    count = 0
    for dx, dy in ((0, -1), (0, 1), (-1, 0), (1, 0)):
        nx = x + dx
        ny = y + dy
        if nx < 1 or ny < 1 or nx >= GRID_W - 1 or ny >= GRID_H - 1:
            continue
        if rows[ny][nx] in ("I", "S", "C"):
            count += 1
    return count


def count_mine_anchor_opportunities(rows: Sequence[str]) -> int:
    opportunities = 0
    for y in range(1, GRID_H - 1):
        for x in range(1, GRID_W - 1):
            if rows[y][x] != ".":
                continue
            walkable = count_walkable_neighbors(rows, x, y)
            breakable = count_adjacent_breakable(rows, x, y)
            if breakable >= 2 and walkable >= 2:
                opportunities += 1
    return opportunities


def has_slide_lane_from_block(rows: Sequence[str], x: int, y: int, min_clear_tiles: int = 2) -> bool:
    if rows[y][x] not in ("I", "S", "C"):
        return False
    for dx, dy in ((0, -1), (0, 1), (-1, 0), (1, 0)):
        clear = 0
        cx, cy = x, y
        while True:
            nx, ny = cx + dx, cy + dy
            if nx <= 0 or ny <= 0 or nx >= GRID_W - 1 or ny >= GRID_H - 1:
                break
            if rows[ny][nx] != ".":
                break
            clear += 1
            cx, cy = nx, ny
        if clear >= min_clear_tiles:
            return True
    return False


def choose_spawn_candidate(rows: Sequence[str]) -> Tuple[int, int] | None:
    cx = GRID_W // 2
    cy = GRID_H // 2
    candidates: List[Tuple[int, int, int, int]] = []
    for y in range(1, GRID_H - 1):
        for x in range(1, GRID_W - 1):
            if rows[y][x] != ".":
                continue
            exits = count_walkable_neighbors(rows, x, y)
            if exits < 2:
                continue
            dist = abs(x - cx) + abs(y - cy)
            candidates.append((dist, y, x, exits))
    if not candidates:
        return None
    candidates.sort()
    _, y, x, _ = candidates[0]
    return (x, y)


def analyze_reachable_floor(rows: Sequence[str], start_x: int, start_y: int) -> Tuple[int, int]:
    q: deque[Tuple[int, int]] = deque()
    visited = [[False for _ in range(GRID_W)] for _ in range(GRID_H)]
    if rows[start_y][start_x] != ".":
        return (0, 0)
    q.append((start_x, start_y))
    visited[start_y][start_x] = True
    reachable = 0
    junctions = 0
    while q:
        x, y = q.popleft()
        neighbors = 0
        reachable += 1
        for dx, dy in ((0, -1), (0, 1), (-1, 0), (1, 0)):
            nx, ny = x + dx, y + dy
            if nx <= 0 or ny <= 0 or nx >= GRID_W - 1 or ny >= GRID_H - 1:
                continue
            if rows[ny][nx] != ".":
                continue
            neighbors += 1
            if not visited[ny][nx]:
                visited[ny][nx] = True
                q.append((nx, ny))
        if neighbors >= 3:
            junctions += 1
    return (reachable, junctions)


def validate_template_rows(rows: Sequence[str]) -> bool:
    if len(rows) != GRID_H:
        return False
    for y, row in enumerate(rows):
        if len(row) != GRID_W:
            return False
        for x, c in enumerate(row):
            if c not in ALLOWED_TEMPLATE_SYMBOLS:
                return False
            if x == 0 or y == 0 or x == GRID_W - 1 or y == GRID_H - 1:
                if c != "#":
                    return False
    special_count = sum(row.count("S") for row in rows)
    pushable_count = sum(row.count("I") + row.count("S") + row.count("C") for row in rows)
    if special_count < 3 or pushable_count < 8:
        return False

    lane_blocks = 0
    for y in range(1, GRID_H - 1):
        for x in range(1, GRID_W - 1):
            if has_slide_lane_from_block(rows, x, y, 2):
                lane_blocks += 1
    if lane_blocks < 4:
        return False

    if count_mine_anchor_opportunities(rows) < 3:
        return False

    spawn = choose_spawn_candidate(rows)
    if spawn is None:
        return False
    reachable, junctions = analyze_reachable_floor(rows, spawn[0], spawn[1])
    if reachable < 40 or junctions < 5:
        return False

    enemy_candidates = 0
    sx, sy = spawn
    for y in range(1, GRID_H - 1):
        for x in range(1, GRID_W - 1):
            if rows[y][x] != ".":
                continue
            if abs(x - sx) + abs(y - sy) < 6:
                continue
            if count_walkable_neighbors(rows, x, y) >= 2:
                enemy_candidates += 1
    if enemy_candidates <= 0:
        return False

    return True


def extract_interior(rows: Sequence[str]) -> List[str]:
    return [rows[y][1 : GRID_W - 1] for y in range(1, GRID_H - 1)]


def extract_patterns(interior_rows: Sequence[str], pattern_size: int) -> Iterable[str]:
    h = len(interior_rows)
    w = len(interior_rows[0])
    for y in range(0, h - pattern_size + 1):
        for x in range(0, w - pattern_size + 1):
            chars: List[str] = []
            for dy in range(pattern_size):
                chars.extend(interior_rows[y + dy][x : x + pattern_size])
            yield "".join(chars)


def pattern_compatible_right(a: str, b: str, pattern_size: int) -> bool:
    for y in range(pattern_size):
        for x in range(pattern_size - 1):
            if a[y * pattern_size + (x + 1)] != b[y * pattern_size + x]:
                return False
    return True


def pattern_compatible_down(a: str, b: str, pattern_size: int) -> bool:
    for y in range(pattern_size - 1):
        for x in range(pattern_size):
            if a[(y + 1) * pattern_size + x] != b[y * pattern_size + x]:
                return False
    return True


def build_pattern_model(gated_maps: Sequence[MapStats], pattern_size: int) -> dict:
    weight_by_pattern: Dict[str, float] = defaultdict(float)

    for m in gated_maps:
        interior = extract_interior(m.rows)
        map_weight = max(m.overall_mean_quality, 1.0) * max(m.sample_count, 1)
        for pattern in extract_patterns(interior, pattern_size):
            weight_by_pattern[pattern] += map_weight

    if not weight_by_pattern:
        raise ValueError("no patterns extracted from gated maps")

    patterns = sorted(weight_by_pattern.keys())
    pattern_index = {p: i for i, p in enumerate(patterns)}
    weights = [weight_by_pattern[p] for p in patterns]
    n = len(patterns)

    adj_r = [set() for _ in range(n)]
    adj_l = [set() for _ in range(n)]
    adj_d = [set() for _ in range(n)]
    adj_u = [set() for _ in range(n)]

    for i, a in enumerate(patterns):
        for j, b in enumerate(patterns):
            if pattern_compatible_right(a, b, pattern_size):
                adj_r[i].add(j)
                adj_l[j].add(i)
            if pattern_compatible_down(a, b, pattern_size):
                adj_d[i].add(j)
                adj_u[j].add(i)

    return {
        "pattern_size": pattern_size,
        "patterns": patterns,
        "pattern_index": pattern_index,
        "weights": weights,
        "adj": {"R": adj_r, "L": adj_l, "D": adj_d, "U": adj_u},
    }


def weighted_choice(rng: random.Random, choices: Sequence[int], weights: Sequence[float]) -> int:
    total = 0.0
    for c in choices:
        total += max(weights[c], 0.0)
    if total <= 0.0:
        return choices[rng.randrange(len(choices))]
    roll = rng.random() * total
    accum = 0.0
    for c in choices:
        accum += max(weights[c], 0.0)
        if roll <= accum:
            return c
    return choices[-1]


def domain_entropy(domain: set[int], weights: Sequence[float]) -> float:
    if len(domain) <= 1:
        return 0.0
    total = 0.0
    for idx in domain:
        total += max(weights[idx], 0.0)
    if total <= 0.0:
        return float(len(domain))
    entropy = 0.0
    for idx in domain:
        p = max(weights[idx], 0.0) / total
        if p > 0.0:
            entropy -= p * math.log(p)
    return entropy


def collapse_wfc_to_rows(model: dict, rng: random.Random) -> List[str] | None:
    pattern_size = int(model["pattern_size"])
    patterns: List[str] = model["patterns"]
    weights: List[float] = model["weights"]
    adj = model["adj"]

    ph = INTERIOR_H - pattern_size + 1
    pw = INTERIOR_W - pattern_size + 1
    if ph <= 0 or pw <= 0:
        return None

    all_pattern_ids = set(range(len(patterns)))
    domains: List[set[int]] = [set(all_pattern_ids) for _ in range(ph * pw)]

    def idx(x: int, y: int) -> int:
        return y * pw + x

    def propagate(seed_cells: Sequence[int]) -> bool:
        q: deque[int] = deque(seed_cells)
        while q:
            cur = q.popleft()
            cx = cur % pw
            cy = cur // pw
            cur_domain = domains[cur]
            for key, dx, dy in (("R", 1, 0), ("L", -1, 0), ("D", 0, 1), ("U", 0, -1)):
                nx = cx + dx
                ny = cy + dy
                if nx < 0 or ny < 0 or nx >= pw or ny >= ph:
                    continue
                nidx = idx(nx, ny)
                allowed: set[int] = set()
                for p in cur_domain:
                    allowed.update(adj[key][p])
                new_domain = domains[nidx].intersection(allowed)
                if not new_domain:
                    return False
                if len(new_domain) < len(domains[nidx]):
                    domains[nidx] = new_domain
                    q.append(nidx)
        return True

    while True:
        unresolved = [i for i, d in enumerate(domains) if len(d) > 1]
        if not unresolved:
            break

        min_entropy = None
        best_cells: List[int] = []
        for cell in unresolved:
            e = domain_entropy(domains[cell], weights)
            if min_entropy is None or e < min_entropy - 1e-12:
                min_entropy = e
                best_cells = [cell]
            elif abs(e - min_entropy) <= 1e-12:
                best_cells.append(cell)

        chosen_cell = best_cells[rng.randrange(len(best_cells))]
        options = sorted(domains[chosen_cell])
        chosen_pattern = weighted_choice(rng, options, weights)
        domains[chosen_cell] = {chosen_pattern}
        if not propagate([chosen_cell]):
            return None

    interior = [["?" for _ in range(INTERIOR_W)] for _ in range(INTERIOR_H)]
    for y in range(ph):
        for x in range(pw):
            pidx = next(iter(domains[idx(x, y)]))
            p = patterns[pidx]
            for dy in range(pattern_size):
                for dx in range(pattern_size):
                    ch = p[dy * pattern_size + dx]
                    ix = x + dx
                    iy = y + dy
                    current = interior[iy][ix]
                    if current == "?" or current == ch:
                        interior[iy][ix] = ch
                    else:
                        return None

    for y in range(INTERIOR_H):
        for x in range(INTERIOR_W):
            if interior[y][x] == "?":
                return None

    rows: List[str] = []
    rows.append("#" * GRID_W)
    for y in range(INTERIOR_H):
        rows.append("#" + "".join(interior[y]) + "#")
    rows.append("#" * GRID_W)
    return rows


def synthesize_templates(
    gated_maps: Sequence[MapStats],
    model: dict,
    target_templates: int,
    seed: int,
) -> List[dict]:
    rng = random.Random(seed)
    templates: List[dict] = []
    seen: set[str] = set()

    for m in gated_maps:
        if not validate_template_rows(m.rows):
            continue
        mid = hash_rows(m.rows)
        if mid in seen:
            continue
        seen.add(mid)
        templates.append(
            {
                "map_id": mid,
                "source": "gated_map",
                "rows": m.rows,
            }
        )
        if len(templates) >= target_templates:
            break

    max_attempts = max(1200, target_templates * 120)
    attempts = 0
    while len(templates) < target_templates and attempts < max_attempts:
        attempts += 1
        rows = collapse_wfc_to_rows(model, rng)
        if rows is None:
            continue
        if not validate_template_rows(rows):
            continue
        mid = hash_rows(rows)
        if mid in seen:
            continue
        seen.add(mid)
        templates.append(
            {
                "map_id": mid,
                "source": "wfc",
                "rows": rows,
            }
        )

    templates.sort(key=lambda t: t["map_id"])
    if len(templates) > target_templates:
        templates = templates[:target_templates]
    return templates


def write_notes(
    path: Path,
    samples_path: Path,
    all_maps: Sequence[MapStats],
    gated_maps: Sequence[MapStats],
    templates: Sequence[dict],
    gate_cfg: dict,
) -> None:
    top = list(gated_maps[:12])
    lines: List[str] = []
    lines.append("# Procgen WFC Good Level Notes")
    lines.append("")
    lines.append(f"- Generated UTC: {datetime.now(timezone.utc).isoformat()}")
    lines.append(f"- Samples input: `{samples_path}`")
    lines.append(f"- Unique map IDs seen: {len(all_maps)}")
    lines.append(f"- Gated good maps: {len(gated_maps)}")
    lines.append(f"- Synthesized templates: {len(templates)}")
    lines.append("- Gate criteria:")
    lines.append(f"  - min samples per profile: {gate_cfg['min_samples_per_profile']}")
    lines.append(f"  - overall mean quality >= {gate_cfg['min_overall_quality']}")
    lines.append(f"  - per-profile mean quality >= {gate_cfg['min_profile_quality']}")
    lines.append("")
    lines.append("## Top Gated Maps")
    lines.append("")
    lines.append("| map_id | samples | overall | arcade | casual | survival | aggressive |")
    lines.append("|---|---:|---:|---:|---:|---:|---:|")
    for m in top:
        lines.append(
            "| {mid} | {samples} | {overall:.2f} | {arc:.2f} | {cas:.2f} | {sur:.2f} | {agg:.2f} |".format(
                mid=m.map_id,
                samples=m.sample_count,
                overall=m.overall_mean_quality,
                arc=m.profile_mean_quality.get("arcade", 0.0),
                cas=m.profile_mean_quality.get("casual", 0.0),
                sur=m.profile_mean_quality.get("survival", 0.0),
                agg=m.profile_mean_quality.get("aggressive", 0.0),
            )
        )
    lines.append("")
    lines.append("## Top Template IDs")
    lines.append("")
    for t in templates[:16]:
        lines.append(f"- `{t['map_id']}` ({t['source']})")
    lines.append("")
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def write_header(path: Path, templates: Sequence[dict]) -> None:
    count = len(templates)
    rows_lines: List[str] = []
    ids_lines: List[str] = []
    for i, tpl in enumerate(templates):
        rows = tpl["rows"]
        rows_lines.append("    {")
        for row in rows:
            rows_lines.append(f'        "{row}",')
        rows_lines.append("    },")
        ids_lines.append(f"    0x{tpl['map_id']}ULL,")

    body = []
    body.append("#ifndef ICEPANIC_PROCGEN_WFC_TEMPLATES_H")
    body.append("#define ICEPANIC_PROCGEN_WFC_TEMPLATES_H")
    body.append("")
    body.append(f"#define PROCGEN_WFC_TEMPLATE_COUNT {count}")
    body.append("")
    if count <= 0:
        body.append("static const char kProcgenWfcTemplates[1][GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1] = {{{")
        body.append('    "####################",')
        body.append('    "#..................#",')
        body.append('    "#..................#",')
        body.append('    "#..................#",')
        body.append('    "#..................#",')
        body.append('    "#..................#",')
        body.append('    "#..................#",')
        body.append('    "#..................#",')
        body.append('    "#..................#",')
        body.append('    "#..................#",')
        body.append('    "#..................#",')
        body.append('    "####################",')
        body.append("}};")
        body.append("static const unsigned long long kProcgenWfcTemplateMapIds[1] = {0ULL};")
    else:
        body.append("static const char kProcgenWfcTemplates[PROCGEN_WFC_TEMPLATE_COUNT][GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1] = {")
        body.extend(rows_lines)
        body.append("};")
        body.append("")
        body.append("static const unsigned long long kProcgenWfcTemplateMapIds[PROCGEN_WFC_TEMPLATE_COUNT] = {")
        body.extend(ids_lines)
        body.append("};")
    body.append("")
    body.append("#endif")
    body.append("")
    path.write_text("\n".join(body), encoding="utf-8")


def build_corpus_payload(
    samples_path: Path,
    all_maps: Sequence[MapStats],
    gated_maps: Sequence[MapStats],
    model: dict,
    templates: Sequence[dict],
    gate_cfg: dict,
) -> dict:
    gated_payload = []
    for m in gated_maps:
        gated_payload.append(
            {
                "map_id": m.map_id,
                "sample_count": m.sample_count,
                "overall_mean_quality": round(m.overall_mean_quality, 6),
                "profile_counts": m.profile_counts,
                "profile_mean_quality": {k: round(v, 6) for k, v in m.profile_mean_quality.items()},
                "rows": m.rows,
            }
        )

    template_payload = []
    for idx, tpl in enumerate(templates):
        template_payload.append(
            {
                "template_index": idx,
                "map_id": tpl["map_id"],
                "source": tpl["source"],
                "rows": tpl["rows"],
            }
        )

    return {
        "schema_version": 1,
        "generated_utc": datetime.now(timezone.utc).isoformat(),
        "samples_input": str(samples_path),
        "gate": gate_cfg,
        "totals": {
            "unique_maps_seen": len(all_maps),
            "gated_maps": len(gated_maps),
            "templates": len(templates),
            "pattern_size": int(model["pattern_size"]),
            "pattern_count": len(model["patterns"]),
        },
        "gated_maps": gated_payload,
        "templates": template_payload,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description="Build bot-informed WFC corpus/header from round map samples.")
    parser.add_argument("--samples", required=True, help="Input JSONL from --round-map-samples-out.")
    parser.add_argument("--out-corpus", required=True, help="Output corpus JSON path.")
    parser.add_argument("--out-notes", required=True, help="Output markdown notes path.")
    parser.add_argument("--out-header", required=True, help="Output generated C header path.")
    parser.add_argument("--target-templates", type=int, default=64, help="Target number of synthesized templates.")
    parser.add_argument("--seed", type=int, default=1337, help="Deterministic seed for synthesis.")
    parser.add_argument("--min-samples-per-profile", type=int, default=3, help="Balanced gate: minimum samples per profile.")
    parser.add_argument("--min-overall-quality", type=float, default=50.0, help="Balanced gate: minimum overall mean quality.")
    parser.add_argument("--min-profile-quality", type=float, default=40.0, help="Balanced gate: minimum per-profile mean quality.")
    args = parser.parse_args()

    samples_path = Path(args.samples)
    out_corpus = Path(args.out_corpus)
    out_notes = Path(args.out_notes)
    out_header = Path(args.out_header)
    out_corpus.parent.mkdir(parents=True, exist_ok=True)
    out_notes.parent.mkdir(parents=True, exist_ok=True)
    out_header.parent.mkdir(parents=True, exist_ok=True)

    try:
        samples = load_samples(samples_path)
        all_maps = build_map_stats(samples)
        gate_cfg = {
            "mode": "balanced",
            "min_samples_per_profile": int(args.min_samples_per_profile),
            "min_overall_quality": float(args.min_overall_quality),
            "min_profile_quality": float(args.min_profile_quality),
            "profiles_required": list(PROFILES),
            "learning_mode": "offline_corpus",
            "pattern_mode": "overlapping_wfc_3x3",
        }
        gated_maps = gate_balanced_maps(
            all_maps,
            min_samples_per_profile=int(args.min_samples_per_profile),
            min_overall=float(args.min_overall_quality),
            min_profile_mean=float(args.min_profile_quality),
        )
        if not gated_maps:
            raise ValueError("no maps passed the balanced gate; relax thresholds or collect more samples")

        model = build_pattern_model(gated_maps, pattern_size=PATTERN_SIZE)
        templates = synthesize_templates(
            gated_maps,
            model=model,
            target_templates=max(1, int(args.target_templates)),
            seed=int(args.seed),
        )

        corpus = build_corpus_payload(samples_path, all_maps, gated_maps, model, templates, gate_cfg)
        out_corpus.write_text(json.dumps(corpus, indent=2, sort_keys=True) + "\n", encoding="utf-8")
        write_notes(out_notes, samples_path, all_maps, gated_maps, templates, gate_cfg)
        write_header(out_header, templates)
    except Exception as exc:  # noqa: BLE001
        print(f"error: {exc}")
        return 1

    print(f"Wrote corpus JSON -> {out_corpus}")
    print(f"Wrote notes      -> {out_notes}")
    print(f"Wrote header     -> {out_header}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

