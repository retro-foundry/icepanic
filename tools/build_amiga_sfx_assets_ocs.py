#!/usr/bin/env python3
"""Build the strict OCS beta Paula SFX bank.

The OCS beta uses the same generated Paula one-shot definitions as the ECS
version, but keeps the PCM payload in Icepanic.sfx instead of linking sample
bytes into the program hunk. The strict OCS bank is generated at a lower sample
rate so all samples still exist while the DMA pool stays small enough for the
first 512 KB Chip range.
"""

from __future__ import annotations

import sys
from pathlib import Path
from typing import List, Sequence, Tuple

sys.path.insert(0, str(Path(__file__).resolve().parent))
import build_amiga_sfx_assets as full  # noqa: E402


OUT_C = Path("src/platform_amiga/amiga_sfx_assets_ocs.c")
OUT_H = Path("src/platform_amiga/amiga_sfx_assets_ocs.h")
OUT_BIN = Path("src/platform_amiga/amiga_sfx_assets_ocs.bin")

OCS_SAMPLE_RATE = 6000
OCS_GUARD_TAIL_TICKS = 1
OCS_SAMPLE_PERIOD = full.PAL_CLOCK // OCS_SAMPLE_RATE

SELECTED_SAMPLE_IDS = tuple(range(1, len(full.SAMPLES) + 1))


def configure_ocs_sample_budget() -> None:
    full.SAMPLE_RATE = OCS_SAMPLE_RATE
    full.TICK_SAMPLES = OCS_SAMPLE_RATE // full.TICK_HZ
    full.GUARD_TAIL_TICKS = OCS_GUARD_TAIL_TICKS


def format_array(values: Sequence[int], per_line: int = 16, hex_bytes: bool = False) -> str:
    lines: List[str] = []
    for i in range(0, len(values), per_line):
        chunk = values[i : i + per_line]
        if hex_bytes:
            body = ", ".join(f"0x{value:02X}" for value in chunk)
        else:
            body = ", ".join(str(value) for value in chunk)
        lines.append(f"    {body}")
    return ",\n".join(lines)


def build_compact() -> Tuple[bytes, List[int], List[int], List[int]]:
    configure_ocs_sample_budget()
    source, source_offsets, source_lengths, source_ticks = full.build_samples()
    data = bytearray()
    offsets = [0] * (len(full.SAMPLES) + 1)
    lengths = [0] * len(offsets)
    ticks = [0] * len(offsets)

    for sample_id in SELECTED_SAMPLE_IDS:
        start = source_offsets[sample_id]
        byte_len = source_lengths[sample_id] * 2
        sample = source[start : start + byte_len]
        if len(data) & 1:
            data.append(0)
        offsets[sample_id] = len(data)
        lengths[sample_id] = len(sample) // 2
        ticks[sample_id] = source_ticks[sample_id]
        data.extend(sample)

    return bytes(data), offsets, lengths, ticks


def write_header(path: Path, data_bytes: int) -> None:
    path.write_text(
        f"""#ifndef ICEPANIC_AMIGA_SFX_ASSETS_OCS_H
#define ICEPANIC_AMIGA_SFX_ASSETS_OCS_H

#include "amiga_sfx_assets.h"

#define AMIGA_SFX_OCS_DATA_BYTES {data_bytes}UL
#define AMIGA_SFX_OCS_SAMPLE_RATE {OCS_SAMPLE_RATE}
#define AMIGA_SFX_OCS_SAMPLE_PERIOD {OCS_SAMPLE_PERIOD}

extern const ULONG g_amiga_sfx_ocs_offsets[AMIGA_SFX_SAMPLE_COUNT];
extern const UWORD g_amiga_sfx_ocs_lengths_words[AMIGA_SFX_SAMPLE_COUNT];
extern const UBYTE g_amiga_sfx_ocs_ticks[AMIGA_SFX_SAMPLE_COUNT];

#endif
""",
        encoding="ascii",
    )


def write_source(path: Path, data: bytes, offsets: Sequence[int], lengths: Sequence[int], ticks: Sequence[int]) -> None:
    path.write_text(
        f"""#include "amiga_sfx_assets_ocs.h"

const ULONG g_amiga_sfx_ocs_offsets[AMIGA_SFX_SAMPLE_COUNT] = {{
{format_array(offsets, per_line=8)}
}};

const UWORD g_amiga_sfx_ocs_lengths_words[AMIGA_SFX_SAMPLE_COUNT] = {{
{format_array(lengths, per_line=8)}
}};

const UBYTE g_amiga_sfx_ocs_ticks[AMIGA_SFX_SAMPLE_COUNT] = {{
{format_array(ticks, per_line=16)}
}};
""",
        encoding="ascii",
    )


def main() -> int:
    data, offsets, lengths, ticks = build_compact()
    OUT_C.parent.mkdir(parents=True, exist_ok=True)
    write_header(OUT_H, len(data))
    write_source(OUT_C, data, offsets, lengths, ticks)
    OUT_BIN.write_bytes(data)
    print(f"wrote {OUT_C}, {OUT_H}, and {OUT_BIN}: {len(data)} bytes")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
