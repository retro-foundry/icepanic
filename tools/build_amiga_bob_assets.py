#!/usr/bin/env python3
"""
Build Amiga ECS blitter BOB assets from Icepanic's indexed tileset.

The Amiga runtime consumes pre-planar 16x16 art:
  - 5 bitplanes
  - 2 words per row so the blitter can draw shifted 16px BOBs
  - one widened transparency mask per tile, using palette index 0 as transparent
  - Amiga 12-bit palette words
"""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import List, Sequence, Tuple


TILE_SIZE = 16
DEPTH = 5
PALETTE_COUNT = 32
ROW_WORDS_WIDE = 2
ROW_BYTES_WIDE = ROW_WORDS_WIDE * 2
ROW_WORDS_TILE = 1
ROW_BYTES_TILE = ROW_WORDS_TILE * 2
TILE_BYTES = TILE_SIZE * DEPTH * ROW_BYTES_TILE
TILE_MASK_BYTES = TILE_SIZE * DEPTH * ROW_BYTES_TILE
PLANE_BYTES_WIDE = TILE_SIZE * ROW_BYTES_WIDE
BOB_BYTES = DEPTH * PLANE_BYTES_WIDE
MASK_BYTES = DEPTH * TILE_SIZE * ROW_BYTES_WIDE
TITLE_TRANSPARENT_INDEX = 255

SPRITE_ORDER: List[str] = [
    "SPR_FLOOR",
    "SPR_WALL",
    "SPR_BORDER",
    "SPR_BLOCK_ICE",
    "SPR_BLOCK_SPECIAL",
    "SPR_BLOCK_CRACKED",
    "SPR_BLOCK_UNBREAKABLE",
    "SPR_PLAYER_UP",
    "SPR_PLAYER_UP_ALT",
    "SPR_PLAYER_DOWN",
    "SPR_PLAYER_DOWN_ALT",
    "SPR_PLAYER_LEFT",
    "SPR_PLAYER_LEFT_ALT",
    "SPR_PLAYER_RIGHT",
    "SPR_PLAYER_RIGHT_ALT",
    "SPR_ENEMY_A",
    "SPR_ENEMY_A_ALT",
    "SPR_ENEMY_B",
    "SPR_ENEMY_B_ALT",
    "SPR_ENEMY_A_DEATH_0",
    "SPR_ENEMY_A_DEATH_1",
    "SPR_ENEMY_B_DEATH_0",
    "SPR_ENEMY_B_DEATH_1",
    "SPR_ITEM_FISH",
    "SPR_ITEM_EGG",
    "SPR_ITEM_GEM",
    "SPR_ITEM_FRUIT",
    "SPR_ITEM_MYSTERY",
    "SPR_HUD_LIFE",
    "SPR_EFFECT_SPARK",
    "SPR_MINE_0",
    "SPR_MINE_1",
    "SPR_MINE_2",
    "SPR_ENEMY_C",
    "SPR_ENEMY_C_ALT",
    "SPR_ENEMY_C_DEATH_0",
    "SPR_ENEMY_C_DEATH_1",
    "SPR_ENEMY_D",
    "SPR_ENEMY_D_ALT",
    "SPR_ENEMY_D_DEATH_0",
    "SPR_ENEMY_D_DEATH_1",
]


def read_u16_le(data: bytes, offset: int) -> int:
    return data[offset] | (data[offset + 1] << 8)


def read_s16_le(data: bytes, offset: int) -> int:
    value = read_u16_le(data, offset)
    return value - 0x10000 if value >= 0x8000 else value


def load_tileset(path: Path) -> Tuple[List[Tuple[int, int, int]], List[List[int]]]:
    data = path.read_bytes()
    if len(data) < 14 or data[:4] != b"ITLS":
        raise RuntimeError(f"{path} is not an ITLS tileset")

    version = read_u16_le(data, 4)
    tile_count = read_u16_le(data, 6)
    tile_w = read_u16_le(data, 8)
    tile_h = read_u16_le(data, 10)
    palette_count = read_u16_le(data, 12)
    if version != 1 or tile_w != TILE_SIZE or tile_h != TILE_SIZE or palette_count != PALETTE_COUNT:
        raise RuntimeError(
            f"unsupported tileset header: version={version} size={tile_w}x{tile_h} palette={palette_count}"
        )
    if tile_count < len(SPRITE_ORDER):
        raise RuntimeError(f"tileset has {tile_count} tiles; expected at least {len(SPRITE_ORDER)}")

    palette: List[Tuple[int, int, int]] = []
    offset = 14
    for _ in range(PALETTE_COUNT):
        palette.append((data[offset], data[offset + 1], data[offset + 2]))
        offset += 3

    tile_bytes = TILE_SIZE * TILE_SIZE
    expected = offset + tile_count * tile_bytes
    if len(data) < expected:
        raise RuntimeError(f"tileset truncated: got {len(data)} bytes, expected {expected}")

    tiles: List[List[int]] = []
    for i in range(len(SPRITE_ORDER)):
        start = offset + i * tile_bytes
        tiles.append([p & 31 for p in data[start : start + tile_bytes]])
    return palette, tiles


def load_title_overlay(path: Path) -> Tuple[int, int, int, int, bytes]:
    data = path.read_bytes()
    if len(data) < 16 or data[:4] != b"ITOV":
        raise RuntimeError(f"{path} is not an ITOV title overlay")

    version = read_u16_le(data, 4)
    width = read_u16_le(data, 6)
    height = read_u16_le(data, 8)
    anchor_x = read_s16_le(data, 10)
    anchor_y = read_s16_le(data, 12)
    if version != 1 or width <= 0 or height <= 0 or width > 320 or height > 200:
        raise RuntimeError(f"unsupported title overlay header: version={version} size={width}x{height}")

    pixel_count = width * height
    pixels = data[16 : 16 + pixel_count]
    if len(pixels) != pixel_count:
        raise RuntimeError(f"title overlay truncated: got {len(pixels)} pixels, expected {pixel_count}")
    for i, pixel in enumerate(pixels):
        if pixel != TITLE_TRANSPARENT_INDEX and pixel >= PALETTE_COUNT:
            raise RuntimeError(f"title overlay pixel {i} has invalid palette index {pixel}")
    return width, height, anchor_x, anchor_y, bytes(pixels)


def rgb8_to_amiga12_word(rgb: Tuple[int, int, int]) -> int:
    r, g, b = rgb
    return ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4)


def row_word_for_plane(indices: Sequence[int], y: int, plane: int) -> int:
    word = 0
    base = y * TILE_SIZE
    for x in range(TILE_SIZE):
        idx = indices[base + x] & 31
        if (idx >> plane) & 1:
            word |= 1 << (15 - x)
    return word


def row_word_for_mask(indices: Sequence[int], y: int) -> int:
    word = 0
    base = y * TILE_SIZE
    for x in range(TILE_SIZE):
        if indices[base + x] != 0:
            word |= 1 << (15 - x)
    return word


def append_word_be(out: bytearray, word: int) -> None:
    out.append((word >> 8) & 0xFF)
    out.append(word & 0xFF)


def build_bob_data(tile: Sequence[int]) -> bytes:
    out = bytearray()
    for y in range(TILE_SIZE):
        for plane in range(DEPTH):
            append_word_be(out, row_word_for_plane(tile, y, plane))
            append_word_be(out, 0)
    if len(out) != BOB_BYTES:
        raise AssertionError("BOB byte count mismatch")
    return bytes(out)


def build_tile_data(tile: Sequence[int]) -> bytes:
    out = bytearray()
    for y in range(TILE_SIZE):
        for plane in range(DEPTH):
            append_word_be(out, row_word_for_plane(tile, y, plane))
    if len(out) != TILE_BYTES:
        raise AssertionError("tile byte count mismatch")
    return bytes(out)


def build_mask_data(tile: Sequence[int]) -> bytes:
    out = bytearray()
    for y in range(TILE_SIZE):
        row_mask = row_word_for_mask(tile, y)
        for _plane in range(DEPTH):
            append_word_be(out, row_mask)
            append_word_be(out, 0)
    if len(out) != MASK_BYTES:
        raise AssertionError("mask byte count mismatch")
    return bytes(out)


def build_tile_mask_data(tile: Sequence[int]) -> bytes:
    out = bytearray()
    for y in range(TILE_SIZE):
        row_mask = row_word_for_mask(tile, y)
        for _plane in range(DEPTH):
            append_word_be(out, row_mask)
    if len(out) != TILE_MASK_BYTES:
        raise AssertionError("tile mask byte count mismatch")
    return bytes(out)


def title_row_word_for_plane(pixels: Sequence[int], width: int, y: int, plane: int, word_index: int) -> int:
    word = 0
    base = y * width
    start_x = word_index * 16
    for bit in range(16):
        x = start_x + bit
        if x >= width:
            continue
        idx = pixels[base + x]
        if idx != TITLE_TRANSPARENT_INDEX and ((idx & 31) >> plane) & 1:
            word |= 1 << (15 - bit)
    return word


def title_row_word_for_mask(pixels: Sequence[int], width: int, y: int, word_index: int) -> int:
    word = 0
    base = y * width
    start_x = word_index * 16
    for bit in range(16):
        x = start_x + bit
        if x >= width:
            continue
        if pixels[base + x] != TITLE_TRANSPARENT_INDEX:
            word |= 1 << (15 - bit)
    return word


def title_planar_sizes(width: int, height: int) -> Tuple[int, int, int]:
    row_words = ((width + 15) // 16) + 1
    row_bytes = row_words * 2
    return row_words, row_bytes, height * DEPTH * row_bytes


def build_title_planar_data(title: Tuple[int, int, int, int, bytes]) -> bytes:
    width, height, _, _, pixels = title
    row_words, _, total_bytes = title_planar_sizes(width, height)
    out = bytearray()
    for y in range(height):
        for plane in range(DEPTH):
            for word_index in range(row_words):
                append_word_be(out, title_row_word_for_plane(pixels, width, y, plane, word_index))
    if len(out) != total_bytes:
        raise AssertionError("title data byte count mismatch")
    return bytes(out)


def build_title_planar_mask(title: Tuple[int, int, int, int, bytes]) -> bytes:
    width, height, _, _, pixels = title
    row_words, _, total_bytes = title_planar_sizes(width, height)
    out = bytearray()
    for y in range(height):
        for _plane in range(DEPTH):
            for word_index in range(row_words):
                append_word_be(out, title_row_word_for_mask(pixels, width, y, word_index))
    if len(out) != total_bytes:
        raise AssertionError("title mask byte count mismatch")
    return bytes(out)


def bake_amiga_static_tile_details(tiles: Sequence[Sequence[int]]) -> List[List[int]]:
    baked = [list(tile) for tile in tiles]
    for sprite_index in (3, 4, 5, 6):
        tile = baked[sprite_index]
        for i in range(TILE_SIZE):
            tile[i] = 31
            tile[i * TILE_SIZE] = 31
            tile[(TILE_SIZE - 1) * TILE_SIZE + i] = 1
            tile[i * TILE_SIZE + TILE_SIZE - 1] = 1
        for i in range(1, TILE_SIZE - 2):
            tile[1 * TILE_SIZE + i] = 9
            tile[i * TILE_SIZE + 1] = 9
            tile[(TILE_SIZE - 2) * TILE_SIZE + i] = 3
            tile[i * TILE_SIZE + TILE_SIZE - 2] = 3
        if sprite_index != 0:
            tile[7 * TILE_SIZE + 7] = 31
            tile[8 * TILE_SIZE + 8] = 31
            tile[8 * TILE_SIZE + 6] = 30
            tile[7 * TILE_SIZE + 9] = 30
    return baked


def c_array_bytes(data: bytes, indent: str = "        ") -> str:
    lines: List[str] = []
    for i in range(0, len(data), 16):
        chunk = data[i : i + 16]
        lines.append(indent + ", ".join(f"0x{b:02X}" for b in chunk))
    return ",\n".join(lines)


def c_array_words(words: Sequence[int], indent: str = "    ") -> str:
    lines: List[str] = []
    for i in range(0, len(words), 8):
        chunk = words[i : i + 8]
        lines.append(indent + ", ".join(f"0x{w:03X}" for w in chunk))
    return ",\n".join(lines)


def write_header(path: Path, title: Tuple[int, int, int, int, bytes]) -> None:
    title_w, title_h, title_anchor_x, title_anchor_y, _ = title
    title_row_words, title_row_bytes, title_bytes = title_planar_sizes(title_w, title_h)
    enum_lines = []
    for i, name in enumerate(SPRITE_ORDER):
        enum_lines.append(f"    AMIGA_{name} = {i},")
    enum_text = "\n".join(enum_lines)
    path.write_text(
        f"""#ifndef ICEPANIC_AMIGA_ASSETS_H
#define ICEPANIC_AMIGA_ASSETS_H

#include <stdint.h>

#define AMIGA_ASSET_DEPTH {DEPTH}
#define AMIGA_ASSET_PALETTE_COUNT {PALETTE_COUNT}
#define AMIGA_ASSET_TILE_SIZE {TILE_SIZE}
#define AMIGA_ASSET_ROW_WORDS_TILE {ROW_WORDS_TILE}
#define AMIGA_ASSET_ROW_BYTES_TILE {ROW_BYTES_TILE}
#define AMIGA_ASSET_ROW_WORDS_WIDE {ROW_WORDS_WIDE}
#define AMIGA_ASSET_ROW_BYTES_WIDE {ROW_BYTES_WIDE}
#define AMIGA_ASSET_TILE_BYTES {TILE_BYTES}
#define AMIGA_ASSET_TILE_MASK_BYTES {TILE_MASK_BYTES}
#define AMIGA_ASSET_PLANE_BYTES {PLANE_BYTES_WIDE}
#define AMIGA_ASSET_BOB_BYTES {BOB_BYTES}
#define AMIGA_ASSET_MASK_BYTES {MASK_BYTES}
#define AMIGA_ASSET_SPRITE_COUNT {len(SPRITE_ORDER)}
#define AMIGA_TITLE_OVERLAY_WIDTH {title_w}
#define AMIGA_TITLE_OVERLAY_HEIGHT {title_h}
#define AMIGA_TITLE_OVERLAY_ANCHOR_X {title_anchor_x}
#define AMIGA_TITLE_OVERLAY_ANCHOR_Y {title_anchor_y}
#define AMIGA_TITLE_OVERLAY_TRANSPARENT_INDEX {TITLE_TRANSPARENT_INDEX}
#define AMIGA_TITLE_OVERLAY_ROW_WORDS {title_row_words}
#define AMIGA_TITLE_OVERLAY_ROW_BYTES {title_row_bytes}
#define AMIGA_TITLE_OVERLAY_BYTES {title_bytes}
#define AMIGA_TITLE_OVERLAY_MASK_BYTES {title_bytes}

typedef enum AmigaSpriteId {{
{enum_text}
    AMIGA_SPR_COUNT = {len(SPRITE_ORDER)}
}} AmigaSpriteId;

extern const uint16_t g_amiga_palette[AMIGA_ASSET_PALETTE_COUNT];
extern const uint8_t g_amiga_tile_data[AMIGA_ASSET_SPRITE_COUNT][AMIGA_ASSET_TILE_BYTES];
extern const uint8_t g_amiga_tile_mask[AMIGA_ASSET_SPRITE_COUNT][AMIGA_ASSET_TILE_MASK_BYTES];
extern const uint8_t g_amiga_bob_data[AMIGA_ASSET_SPRITE_COUNT][AMIGA_ASSET_BOB_BYTES];
extern const uint8_t g_amiga_bob_mask[AMIGA_ASSET_SPRITE_COUNT][AMIGA_ASSET_MASK_BYTES];
extern const uint8_t g_amiga_title_overlay_data[AMIGA_TITLE_OVERLAY_BYTES];
extern const uint8_t g_amiga_title_overlay_mask[AMIGA_TITLE_OVERLAY_MASK_BYTES];

#endif
""",
        encoding="utf-8",
    )


def write_source(
    path: Path,
    header_name: str,
    palette: Sequence[Tuple[int, int, int]],
    tiles: Sequence[Sequence[int]],
    title: Tuple[int, int, int, int, bytes],
) -> None:
    palette_words = [rgb8_to_amiga12_word(rgb) for rgb in palette]
    baked_tiles = bake_amiga_static_tile_details(tiles)
    tile_data = [build_tile_data(tile) for tile in baked_tiles]
    tile_masks = [build_tile_mask_data(tile) for tile in baked_tiles]
    bobs = [build_bob_data(tile) for tile in baked_tiles]
    masks = [build_mask_data(tile) for tile in baked_tiles]
    title_data = build_title_planar_data(title)
    title_mask = build_title_planar_mask(title)

    parts: List[str] = [
        f'#include "{header_name}"\n\n',
        "const uint16_t g_amiga_palette[AMIGA_ASSET_PALETTE_COUNT] = {\n",
        c_array_words(palette_words),
        "\n};\n\n",
        "const uint8_t g_amiga_tile_data[AMIGA_ASSET_SPRITE_COUNT][AMIGA_ASSET_TILE_BYTES] = {\n",
    ]
    for i, data in enumerate(tile_data):
        parts.append(f"    /* {i:02d} {SPRITE_ORDER[i]} */\n")
        parts.append("    {\n")
        parts.append(c_array_bytes(data))
        parts.append("\n    }")
        parts.append(",\n" if i != len(tile_data) - 1 else "\n")
    parts.append("};\n\n")
    parts.append("const uint8_t g_amiga_tile_mask[AMIGA_ASSET_SPRITE_COUNT][AMIGA_ASSET_TILE_MASK_BYTES] = {\n")
    for i, data in enumerate(tile_masks):
        parts.append(f"    /* {i:02d} {SPRITE_ORDER[i]} */\n")
        parts.append("    {\n")
        parts.append(c_array_bytes(data))
        parts.append("\n    }")
        parts.append(",\n" if i != len(tile_masks) - 1 else "\n")
    parts.extend([
        "};\n\n",
        "const uint8_t g_amiga_bob_data[AMIGA_ASSET_SPRITE_COUNT][AMIGA_ASSET_BOB_BYTES] = {\n",
    ])
    for i, data in enumerate(bobs):
        parts.append(f"    /* {i:02d} {SPRITE_ORDER[i]} */\n")
        parts.append("    {\n")
        parts.append(c_array_bytes(data))
        parts.append("\n    }")
        parts.append(",\n" if i != len(bobs) - 1 else "\n")
    parts.append("};\n\n")
    parts.append("const uint8_t g_amiga_bob_mask[AMIGA_ASSET_SPRITE_COUNT][AMIGA_ASSET_MASK_BYTES] = {\n")
    for i, data in enumerate(masks):
        parts.append(f"    /* {i:02d} {SPRITE_ORDER[i]} */\n")
        parts.append("    {\n")
        parts.append(c_array_bytes(data))
        parts.append("\n    }")
        parts.append(",\n" if i != len(masks) - 1 else "\n")
    parts.append("};\n\n")
    parts.append("const uint8_t g_amiga_title_overlay_data[AMIGA_TITLE_OVERLAY_BYTES] = {\n")
    parts.append(c_array_bytes(title_data, indent="    "))
    parts.append("\n};\n\n")
    parts.append("const uint8_t g_amiga_title_overlay_mask[AMIGA_TITLE_OVERLAY_MASK_BYTES] = {\n")
    parts.append(c_array_bytes(title_mask, indent="    "))
    parts.append("\n};\n")
    path.write_text("".join(parts), encoding="utf-8")


def validate_assets(palette: Sequence[Tuple[int, int, int]], tiles: Sequence[Sequence[int]]) -> None:
    if len(palette) != PALETTE_COUNT:
        raise RuntimeError("palette count mismatch")
    if len(tiles) != len(SPRITE_ORDER):
        raise RuntimeError("tile count mismatch")
    for i, tile in enumerate(tiles):
        if len(tile) != TILE_SIZE * TILE_SIZE:
            raise RuntimeError(f"tile {i} size mismatch")
        bob = build_bob_data(tile)
        mask = build_mask_data(tile)
        tile_data = build_tile_data(tile)
        tile_mask = build_tile_mask_data(tile)
        if len(tile_data) != TILE_BYTES:
            raise RuntimeError(f"tile {i} tile bytes mismatch")
        if len(tile_mask) != TILE_MASK_BYTES:
            raise RuntimeError(f"tile {i} tile mask bytes mismatch")
        if len(bob) != BOB_BYTES:
            raise RuntimeError(f"tile {i} BOB bytes mismatch")
        if len(mask) != MASK_BYTES:
            raise RuntimeError(f"tile {i} mask bytes mismatch")

        nonzero_pixels = sum(1 for p in tile if p != 0)
        mask_bits = 0
        for y in range(TILE_SIZE):
            mask_bits += row_word_for_mask(tile, y).bit_count()
        if nonzero_pixels != mask_bits:
            raise RuntimeError(f"tile {i} mask bit count mismatch")


def validate_title_overlay(title: Tuple[int, int, int, int, bytes]) -> None:
    width, height, _, _, pixels = title
    title_data = build_title_planar_data(title)
    title_mask = build_title_planar_mask(title)
    _, _, title_bytes = title_planar_sizes(width, height)
    if width <= 0 or height <= 0 or len(pixels) != width * height:
        raise RuntimeError("title overlay dimensions mismatch")
    opaque = sum(1 for pixel in pixels if pixel != TITLE_TRANSPARENT_INDEX)
    if opaque <= 0:
        raise RuntimeError("title overlay has no opaque pixels")
    if len(title_data) != title_bytes or len(title_mask) != title_bytes:
        raise RuntimeError("title overlay planar size mismatch")


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate Amiga planar BOB C assets.")
    parser.add_argument("--tileset", default="assets/tileset.bin", help="Input ITLS tileset.")
    parser.add_argument("--title-overlay", default="assets/title_overlay.bin", help="Input ITOV title overlay.")
    parser.add_argument("--out-c", default="src/platform_amiga/amiga_assets.c", help="Output C source.")
    parser.add_argument("--out-h", default="src/platform_amiga/amiga_assets.h", help="Output header.")
    parser.add_argument("--validate-only", action="store_true", help="Validate generated data without writing files.")
    args = parser.parse_args()

    tileset_path = Path(args.tileset)
    out_c = Path(args.out_c)
    out_h = Path(args.out_h)
    palette, tiles = load_tileset(tileset_path)
    title = load_title_overlay(Path(args.title_overlay))
    validate_assets(palette, tiles)
    validate_title_overlay(title)

    if args.validate_only:
        print(
            f"Amiga BOB assets OK: palette={len(palette)} sprites={len(tiles)} "
            f"tile_bytes={TILE_BYTES} bob_bytes={BOB_BYTES} mask_bytes={MASK_BYTES} "
            f"title={title[0]}x{title[1]} title_bytes={title_planar_sizes(title[0], title[1])[2]}"
        )
        return

    out_c.parent.mkdir(parents=True, exist_ok=True)
    out_h.parent.mkdir(parents=True, exist_ok=True)
    write_header(out_h, title)
    write_source(out_c, out_h.name, palette, tiles, title)
    print(f"Wrote {out_h}")
    print(f"Wrote {out_c}")
    print(
        f"Amiga BOB assets: palette={len(palette)} sprites={len(tiles)} "
        f"tile_bytes={TILE_BYTES} bob_bytes={BOB_BYTES} mask_bytes={MASK_BYTES} "
        f"title={title[0]}x{title[1]} title_bytes={title_planar_sizes(title[0], title[1])[2]}"
    )


if __name__ == "__main__":
    main()
