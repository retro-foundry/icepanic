#!/usr/bin/env python3
"""
Build an indexed transparent title overlay blob for Icepanic.

Output format (little-endian):
  magic[4] = "ITOV"
  u16 version = 1
  u16 width
  u16 height
  s16 anchor_x
  s16 anchor_y
  u16 reserved = 0
  u8 pixels[width * height]  (0..31 = palette index, 255 = transparent)
"""

from __future__ import annotations

import argparse
import struct
from pathlib import Path
from typing import Iterable, List, Optional, Sequence, Tuple

from PIL import Image

MAGIC = b"ITOV"
VERSION = 1
TRANSPARENT = 255

DEFAULT_PALETTE: List[Tuple[int, int, int]] = [
    (0, 0, 0),
    (23, 23, 23),
    (60, 64, 66),
    (124, 126, 128),
    (158, 159, 160),
    (95, 53, 10),
    (125, 71, 13),
    (229, 95, 33),
    (241, 176, 33),
    (222, 215, 201),
    (129, 184, 109),
    (46, 133, 58),
    (29, 93, 44),
    (52, 176, 196),
    (13, 39, 201),
    (9, 25, 109),
    (26, 57, 190),
    (70, 110, 210),
    (117, 79, 194),
    (97, 32, 146),
    (71, 16, 116),
    (123, 11, 62),
    (180, 27, 47),
    (226, 38, 40),
    (218, 114, 121),
    (36, 68, 184),
    (61, 107, 210),
    (229, 169, 164),
    (29, 48, 79),
    (85, 154, 255),
    (255, 255, 255),
    (14, 25, 40),
]


def parse_crop(text: str) -> Tuple[int, int, int, int]:
    parts = [p.strip() for p in text.split(",")]
    if len(parts) != 4:
        raise argparse.ArgumentTypeError("crop must be x,y,w,h")
    x, y, w, h = (int(v) for v in parts)
    if w <= 0 or h <= 0:
        raise argparse.ArgumentTypeError("crop width/height must be > 0")
    return x, y, w, h


def parse_rgb(text: str) -> Tuple[int, int, int]:
    parts = [p.strip() for p in text.split(",")]
    if len(parts) != 3:
        raise argparse.ArgumentTypeError("rgb must be r,g,b")
    r, g, b = (int(v) for v in parts)
    if not all(0 <= v <= 255 for v in (r, g, b)):
        raise argparse.ArgumentTypeError("rgb values must be 0..255")
    return r, g, b


def load_palette_from_tileset(path: Path) -> Optional[List[Tuple[int, int, int]]]:
    try:
        data = path.read_bytes()
    except OSError:
        return None
    if len(data) < 14 + 32 * 3:
        return None
    if data[:4] != b"ITLS":
        return None
    version = int.from_bytes(data[4:6], "little")
    palette_count = int.from_bytes(data[12:14], "little")
    if version != 1 or palette_count != 32:
        return None
    off = 14
    palette: List[Tuple[int, int, int]] = []
    for i in range(32):
        r = data[off + i * 3 + 0]
        g = data[off + i * 3 + 1]
        b = data[off + i * 3 + 2]
        palette.append((r, g, b))
    return palette


def nearest_palette_index(rgb: Tuple[int, int, int], palette: Sequence[Tuple[int, int, int]]) -> int:
    r, g, b = rgb
    best_idx = 0
    best_dist = 1 << 30
    for i, (pr, pg, pb) in enumerate(palette):
        dr = r - pr
        dg = g - pg
        db = b - pb
        dist = dr * dr + dg * dg + db * db
        if dist < best_dist:
            best_dist = dist
            best_idx = i
    return best_idx


def trim_transparent_bounds(
    pixels: List[int], width: int, height: int, transparent: int
) -> Tuple[List[int], int, int]:
    min_x = width
    min_y = height
    max_x = -1
    max_y = -1
    for y in range(height):
        row = y * width
        for x in range(width):
            if pixels[row + x] == transparent:
                continue
            if x < min_x:
                min_x = x
            if y < min_y:
                min_y = y
            if x > max_x:
                max_x = x
            if y > max_y:
                max_y = y

    if max_x < min_x or max_y < min_y:
        raise ValueError("overlay cutout removed all pixels; adjust crop/threshold settings")

    out_w = max_x - min_x + 1
    out_h = max_y - min_y + 1
    out = [transparent] * (out_w * out_h)
    for y in range(out_h):
        src_row = (min_y + y) * width
        dst_row = y * out_w
        for x in range(out_w):
            out[dst_row + x] = pixels[src_row + (min_x + x)]
    return out, out_w, out_h


def build_overlay_pixels(
    image: Image.Image,
    palette: Sequence[Tuple[int, int, int]],
    alpha_cutoff: int,
    chroma_key: Optional[Tuple[int, int, int]],
    chroma_tolerance: int,
    luma_cutoff: int,
) -> Tuple[List[int], int, int]:
    rgba = image.convert("RGBA")
    width, height = rgba.size
    out: List[int] = [TRANSPARENT] * (width * height)
    src = list(rgba.getdata())

    for i, (r, g, b, a) in enumerate(src):
        if a < alpha_cutoff:
            continue
        if chroma_key is not None:
            kr, kg, kb = chroma_key
            if max(abs(r - kr), abs(g - kg), abs(b - kb)) <= chroma_tolerance:
                continue
        if luma_cutoff > 0:
            luma = (299 * r + 587 * g + 114 * b) // 1000
            if luma < luma_cutoff:
                continue
        out[i] = nearest_palette_index((r, g, b), palette)

    return out, width, height


def main() -> int:
    parser = argparse.ArgumentParser(description="Build `assets/title_overlay.bin` from a source image.")
    parser.add_argument("--input", required=True, help="Source title art image path (png/jpg/etc).")
    parser.add_argument("--output", default="assets/title_overlay.bin", help="Overlay binary output path.")
    parser.add_argument(
        "--tileset",
        default="assets/tileset.bin",
        help="Tileset binary used for palette extraction (fallbacks to built-in palette if missing).",
    )
    parser.add_argument("--crop", type=parse_crop, help="Optional crop rectangle x,y,w,h before processing.")
    parser.add_argument("--fit-width", type=int, default=280, help="Optional max width; <=0 disables fit.")
    parser.add_argument("--fit-height", type=int, default=96, help="Optional max height; <=0 disables fit.")
    parser.add_argument("--alpha-cutoff", type=int, default=8, help="Alpha threshold (0..255).")
    parser.add_argument("--chroma-key", type=parse_rgb, help="Optional chroma key color r,g,b to treat as transparent.")
    parser.add_argument("--chroma-tolerance", type=int, default=24, help="Chroma key tolerance.")
    parser.add_argument(
        "--luma-cutoff",
        type=int,
        default=0,
        help="Optional luma cutoff (0 disables). Useful for aggressive background removal.",
    )
    parser.add_argument("--auto-trim", action="store_true", help="Trim transparent borders after cutout.")
    parser.add_argument("--anchor-x", type=int, default=0, help="Horizontal draw offset in pixels.")
    parser.add_argument("--anchor-y", type=int, default=0, help="Vertical draw offset in pixels.")
    parser.add_argument("--preview", help="Optional RGBA preview PNG of processed cutout.")
    args = parser.parse_args()

    if args.alpha_cutoff < 0 or args.alpha_cutoff > 255:
        raise SystemExit("--alpha-cutoff must be 0..255")
    if args.chroma_tolerance < 0 or args.chroma_tolerance > 255:
        raise SystemExit("--chroma-tolerance must be 0..255")
    if args.luma_cutoff < 0 or args.luma_cutoff > 255:
        raise SystemExit("--luma-cutoff must be 0..255")

    in_path = Path(args.input)
    out_path = Path(args.output)
    tileset_path = Path(args.tileset)

    img = Image.open(in_path).convert("RGBA")
    if args.crop is not None:
        x, y, w, h = args.crop
        img = img.crop((x, y, x + w, y + h))

    if args.fit_width > 0 or args.fit_height > 0:
        max_w = args.fit_width if args.fit_width > 0 else img.width
        max_h = args.fit_height if args.fit_height > 0 else img.height
        sx = max_w / float(img.width)
        sy = max_h / float(img.height)
        scale = min(sx, sy, 1.0)
        if scale < 1.0:
            new_w = max(1, int(round(img.width * scale)))
            new_h = max(1, int(round(img.height * scale)))
            img = img.resize((new_w, new_h), Image.Resampling.LANCZOS)

    palette = load_palette_from_tileset(tileset_path) or DEFAULT_PALETTE
    pixels, width, height = build_overlay_pixels(
        img,
        palette=palette,
        alpha_cutoff=args.alpha_cutoff,
        chroma_key=args.chroma_key,
        chroma_tolerance=args.chroma_tolerance,
        luma_cutoff=args.luma_cutoff,
    )

    if args.auto_trim:
        pixels, width, height = trim_transparent_bounds(pixels, width, height, TRANSPARENT)
    else:
        if all(px == TRANSPARENT for px in pixels):
            raise SystemExit("overlay cutout removed all pixels; adjust settings")

    if width <= 0 or height <= 0 or width > 320 or height > 200:
        raise SystemExit(f"invalid overlay size after processing: {width}x{height} (must fit <=320x200)")

    out_path.parent.mkdir(parents=True, exist_ok=True)
    header = struct.pack(
        "<4sHHHhhH",
        MAGIC,
        VERSION,
        width,
        height,
        int(args.anchor_x),
        int(args.anchor_y),
        0,
    )
    out_path.write_bytes(header + bytes(pixels))

    if args.preview:
        preview_path = Path(args.preview)
        preview_path.parent.mkdir(parents=True, exist_ok=True)
        rgba: List[Tuple[int, int, int, int]] = []
        for px in pixels:
            if px == TRANSPARENT:
                rgba.append((0, 0, 0, 0))
            else:
                r, g, b = palette[px]
                rgba.append((r, g, b, 255))
        preview = Image.new("RGBA", (width, height))
        preview.putdata(rgba)
        preview.save(preview_path)

    print(f"Wrote {out_path} ({width}x{height})")
    if args.preview:
        print(f"Wrote {args.preview}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
