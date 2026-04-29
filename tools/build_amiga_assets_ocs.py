#!/usr/bin/env python3
"""Build external graphics payloads for the strict OCS beta."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Sequence, Tuple

sys.path.insert(0, str(Path(__file__).resolve().parent))
import build_amiga_bob_assets as full  # noqa: E402


OUT_C = Path("src/platform_amiga/amiga_assets_ocs.c")
OUT_H = Path("src/platform_amiga/amiga_assets_ocs.h")
OUT_BIN = Path("src/platform_amiga/amiga_assets_ocs.bin")


def write_source(path: Path, palette: Sequence[Tuple[int, int, int]]) -> None:
    palette_words = [full.rgb8_to_amiga12_word(rgb) for rgb in palette]
    body = ",\n".join(
        "    " + ", ".join(f"0x{word:03X}" for word in palette_words[i : i + 8])
        for i in range(0, len(palette_words), 8)
    )
    path.write_text(
        f"""#include "amiga_assets_ocs.h"

const uint16_t g_amiga_palette[AMIGA_ASSET_PALETTE_COUNT] = {{
{body}
}};
""",
        encoding="ascii",
    )


def write_header(path: Path) -> None:
    path.write_text(
        """#ifndef ICEPANIC_AMIGA_ASSETS_OCS_H
#define ICEPANIC_AMIGA_ASSETS_OCS_H

#include "amiga_assets.h"

#define AMIGA_OCS_GFX_MASK_OFFSET 0UL
#define AMIGA_OCS_GFX_BOB_OFFSET (AMIGA_OCS_GFX_MASK_OFFSET + ((unsigned long)AMIGA_ASSET_SPRITE_COUNT * AMIGA_ASSET_MASK_BYTES))
#define AMIGA_OCS_GFX_TILE_MASK_OFFSET (AMIGA_OCS_GFX_BOB_OFFSET + ((unsigned long)AMIGA_ASSET_SPRITE_COUNT * AMIGA_ASSET_BOB_BYTES))
#define AMIGA_OCS_GFX_TILE_OFFSET (AMIGA_OCS_GFX_TILE_MASK_OFFSET + ((unsigned long)AMIGA_ASSET_SPRITE_COUNT * AMIGA_ASSET_TILE_MASK_BYTES))
#define AMIGA_OCS_GFX_TITLE_OFFSET (AMIGA_OCS_GFX_TILE_OFFSET + ((unsigned long)AMIGA_ASSET_SPRITE_COUNT * AMIGA_ASSET_TILE_BYTES))
#define AMIGA_OCS_GFX_TITLE_MASK_OFFSET (AMIGA_OCS_GFX_TITLE_OFFSET + AMIGA_TITLE_OVERLAY_BYTES)
#define AMIGA_OCS_GFX_BYTES (AMIGA_OCS_GFX_TITLE_MASK_OFFSET + AMIGA_TITLE_OVERLAY_MASK_BYTES)

#endif
""",
        encoding="ascii",
    )


def build_payload(tiles: Sequence[Sequence[int]], title: Tuple[int, int, int, int, bytes]) -> bytes:
    baked_tiles = full.bake_amiga_static_tile_details(tiles)
    payload = bytearray()
    for tile in baked_tiles:
        payload.extend(full.build_mask_data(tile))
    for tile in baked_tiles:
        payload.extend(full.build_bob_data(tile))
    for tile in baked_tiles:
        payload.extend(full.build_tile_mask_data(tile))
    for tile in baked_tiles:
        payload.extend(full.build_tile_data(tile))
    payload.extend(full.build_title_planar_data(title))
    payload.extend(full.build_title_planar_mask(title))
    return bytes(payload)


def main() -> int:
    palette, tiles = full.load_tileset(Path("assets/tileset.bin"))
    title = full.load_title_overlay(Path("assets/title_overlay.bin"))
    full.validate_assets(palette, tiles)
    full.validate_title_overlay(title)
    OUT_C.parent.mkdir(parents=True, exist_ok=True)
    write_header(OUT_H)
    write_source(OUT_C, palette)
    OUT_BIN.write_bytes(build_payload(tiles, title))
    print(f"wrote {OUT_C}, {OUT_H}, and {OUT_BIN}: {OUT_BIN.stat().st_size} bytes")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
