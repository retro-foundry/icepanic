#!/usr/bin/env python3
"""
Generate deterministic index8 frame dumps for conversion/parity regression tests.

This produces the same dump shape as `icepanic_sdl --dump-indexed`:
  - palette.rgb
  - frame_info.txt
  - frame_00000.idx...
"""

from __future__ import annotations

import argparse
from pathlib import Path


DEFAULT_PALETTE = [
    (0, 0, 0), (23, 23, 23), (60, 64, 66), (124, 126, 128),
    (158, 159, 160), (95, 53, 10), (125, 71, 13), (229, 95, 33),
    (241, 176, 33), (222, 215, 201), (129, 184, 109), (46, 133, 58),
    (29, 93, 44), (52, 176, 196), (13, 39, 201), (9, 25, 109),
    (26, 57, 190), (70, 110, 210), (117, 79, 194), (97, 32, 146),
    (71, 16, 116), (123, 11, 62), (180, 27, 47), (226, 38, 40),
    (218, 114, 121), (36, 68, 184), (61, 107, 210), (229, 169, 164),
    (29, 48, 79), (85, 154, 255), (255, 255, 255), (14, 25, 40),
]


def put_pixel(buf: bytearray, width: int, height: int, x: int, y: int, color: int) -> None:
    if x < 0 or y < 0 or x >= width or y >= height:
        return
    buf[y * width + x] = color & 31


def rect(buf: bytearray, width: int, height: int, x0: int, y0: int, w: int, h: int, color: int) -> None:
    for y in range(y0, y0 + h):
        for x in range(x0, x0 + w):
            put_pixel(buf, width, height, x, y, color)


def outline(buf: bytearray, width: int, height: int, x0: int, y0: int, w: int, h: int, color: int) -> None:
    for x in range(x0, x0 + w):
        put_pixel(buf, width, height, x, y0, color)
        put_pixel(buf, width, height, x, y0 + h - 1, color)
    for y in range(y0, y0 + h):
        put_pixel(buf, width, height, x0, y, color)
        put_pixel(buf, width, height, x0 + w - 1, y, color)


def generate_frame(width: int, height: int, frame_index: int, seed: int) -> bytes:
    buf = bytearray(width * height)

    # Deterministic floor pattern.
    for y in range(height):
        for x in range(width):
            tile_mix = ((x // 16) * 3 + (y // 16) * 5 + seed) & 7
            c = 31 if tile_mix >= 3 else 28
            buf[y * width + x] = c

    # Sparse sparkle noise.
    for y in range(0, height, 2):
        for x in range(0, width, 2):
            v = (x * 131 + y * 71 + frame_index * 97 + seed * 17) & 255
            if v < 6:
                put_pixel(buf, width, height, x, y, 29)
            elif v == 255:
                put_pixel(buf, width, height, x, y, 30)

    # Border ring.
    outline(buf, width, height, 0, 0, width, height, 2)
    outline(buf, width, height, 1, 1, width - 2, height - 2, 3)

    # Sliding "ice block" surrogate.
    block_x = 16 + ((frame_index * 3) % max(1, width - 48))
    block_y = 64 + (((frame_index // 3) * 2) % max(1, height - 96))
    rect(buf, width, height, block_x, block_y, 16, 16, 16)
    outline(buf, width, height, block_x, block_y, 16, 16, 30)
    rect(buf, width, height, block_x + 2, block_y + 2, 11, 11, 29)

    # Moving enemy surrogate.
    enemy_x = 8 + ((frame_index * 5) % max(1, width - 16))
    enemy_y = 24 + ((frame_index * 2) % max(1, height - 48))
    rect(buf, width, height, enemy_x, enemy_y, 10, 8, 23)
    rect(buf, width, height, enemy_x + 2, enemy_y + 3, 2, 2, 30)
    rect(buf, width, height, enemy_x + 6, enemy_y + 3, 2, 2, 30)

    # Player surrogate with alternating color accent.
    player_x = width // 2 + ((frame_index % 8) - 4)
    player_y = (height * 2) // 3
    rect(buf, width, height, player_x - 4, player_y - 6, 8, 9, 30)
    rect(buf, width, height, player_x - 2, player_y - 8, 4, 3, 24 if (frame_index & 1) == 0 else 18)
    rect(buf, width, height, player_x - 5, player_y + 2, 10, 2, 8)

    return bytes(buf)


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate deterministic index8 frame dumps.")
    parser.add_argument("--output-dir", default="assets/frame_dump_synth", help="Output directory.")
    parser.add_argument("--width", type=int, default=320, help="Frame width.")
    parser.add_argument("--height", type=int, default=200, help="Frame height.")
    parser.add_argument("--frames", type=int, default=120, help="Frame count.")
    parser.add_argument("--seed", type=int, default=1337, help="Deterministic seed.")
    args = parser.parse_args()

    out_dir = Path(args.output_dir)
    width = int(args.width)
    height = int(args.height)
    frames = int(args.frames)
    seed = int(args.seed)

    if width <= 0 or height <= 0:
        raise ValueError("width/height must be positive")
    if frames <= 0:
        raise ValueError("frames must be positive")

    out_dir.mkdir(parents=True, exist_ok=True)

    palette_bytes = bytearray()
    for r, g, b in DEFAULT_PALETTE:
        palette_bytes.extend((r & 255, g & 255, b & 255))
    (out_dir / "palette.rgb").write_bytes(bytes(palette_bytes))

    (out_dir / "frame_info.txt").write_text(
        f"width={width}\n"
        f"height={height}\n"
        "pixel_format=index8_palette32\n"
        "frame_file_pattern=frame_%05d.idx\n",
        encoding="utf-8",
    )

    for i in range(frames):
        frame = generate_frame(width, height, i, seed)
        (out_dir / f"frame_{i:05d}.idx").write_bytes(frame)

    print(f"Generated {frames} deterministic frame(s) in {out_dir}")


if __name__ == "__main__":
    main()

