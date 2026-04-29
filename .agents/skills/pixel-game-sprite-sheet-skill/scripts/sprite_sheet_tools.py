#!/usr/bin/env python3
"""Utility tools for making generated pixel-art sprite sheets game-ready.

Dependencies: Pillow

Examples:
  python sprite_sheet_tools.py crop sheet.png out_frames --rows 4 --cols 4
  python sprite_sheet_tools.py repack out_frames sheet_clean.png --rows 4 --cols 4 --cell 64 --scale 1
  python sprite_sheet_tools.py gif out_frames preview.gif --duration 120
  python sprite_sheet_tools.py quantize sheet.png sheet_32.png --colors 32
  python sprite_sheet_tools.py remove-bg sheet.png sheet_alpha.png --color 0,0,0 --tolerance 8
"""

from __future__ import annotations

import argparse
import json
import math
import os
from pathlib import Path
from typing import Iterable, List, Tuple

from PIL import Image

RGBA = Tuple[int, int, int, int]
RGB = Tuple[int, int, int]


def parse_rgb(value: str) -> RGB:
    parts = value.split(",")
    if len(parts) != 3:
        raise argparse.ArgumentTypeError("Expected R,G,B")
    try:
        rgb = tuple(int(p.strip()) for p in parts)
    except ValueError as exc:
        raise argparse.ArgumentTypeError("Expected integer R,G,B") from exc
    if any(v < 0 or v > 255 for v in rgb):
        raise argparse.ArgumentTypeError("RGB values must be 0..255")
    return rgb  # type: ignore[return-value]


def ensure_dir(path: Path) -> None:
    path.mkdir(parents=True, exist_ok=True)


def load_image(path: Path) -> Image.Image:
    if not path.exists():
        raise FileNotFoundError(path)
    return Image.open(path).convert("RGBA")


def crop_grid(
    src: Path,
    out_dir: Path,
    rows: int,
    cols: int,
    margin_x: int = 0,
    margin_y: int = 0,
    gap_x: int = 0,
    gap_y: int = 0,
    cell_w: int | None = None,
    cell_h: int | None = None,
    prefix: str = "frame",
) -> dict:
    img = load_image(src)
    ensure_dir(out_dir)

    if cell_w is None:
        cell_w = (img.width - (2 * margin_x) - ((cols - 1) * gap_x)) // cols
    if cell_h is None:
        cell_h = (img.height - (2 * margin_y) - ((rows - 1) * gap_y)) // rows

    metadata = {
        "source": str(src),
        "rows": rows,
        "cols": cols,
        "cell_w": cell_w,
        "cell_h": cell_h,
        "margin_x": margin_x,
        "margin_y": margin_y,
        "gap_x": gap_x,
        "gap_y": gap_y,
        "frames": [],
    }

    for r in range(rows):
        for c in range(cols):
            x = margin_x + c * (cell_w + gap_x)
            y = margin_y + r * (cell_h + gap_y)
            crop = img.crop((x, y, x + cell_w, y + cell_h))
            name = f"{prefix}_r{r:02d}_c{c:02d}.png"
            crop.save(out_dir / name)
            metadata["frames"].append({"row": r, "col": c, "file": name, "rect": [x, y, cell_w, cell_h]})

    with open(out_dir / "frames.json", "w", encoding="utf-8") as f:
        json.dump(metadata, f, indent=2)
    return metadata


def trim_alpha(img: Image.Image, padding: int = 0) -> Image.Image:
    if img.mode != "RGBA":
        img = img.convert("RGBA")
    alpha = img.getchannel("A")
    bbox = alpha.getbbox()
    if bbox is None:
        return img
    left, top, right, bottom = bbox
    left = max(0, left - padding)
    top = max(0, top - padding)
    right = min(img.width, right + padding)
    bottom = min(img.height, bottom + padding)
    return img.crop((left, top, right, bottom))


def read_frames(path: Path) -> List[Path]:
    if path.is_dir():
        files = sorted([p for p in path.iterdir() if p.suffix.lower() in {".png", ".webp", ".jpg", ".jpeg"}])
    else:
        files = [path]
    if not files:
        raise ValueError(f"No image frames found in {path}")
    return files


def repack(
    frames_dir: Path,
    out_path: Path,
    rows: int,
    cols: int,
    cell: int | None = None,
    cell_w: int | None = None,
    cell_h: int | None = None,
    gap: int = 0,
    margin: int = 0,
    bg: RGB | None = (0, 0, 0),
    scale: int = 1,
    trim: bool = False,
) -> None:
    files = read_frames(frames_dir)
    imgs = [load_image(p) for p in files]
    if trim:
        imgs = [trim_alpha(i, padding=1) for i in imgs]

    if cell is not None:
        cell_w = cell_h = cell
    if cell_w is None:
        cell_w = max(i.width for i in imgs)
    if cell_h is None:
        cell_h = max(i.height for i in imgs)

    canvas_w = margin * 2 + cols * cell_w + (cols - 1) * gap
    canvas_h = margin * 2 + rows * cell_h + (rows - 1) * gap
    background: RGBA = (0, 0, 0, 0) if bg is None else (*bg, 255)
    sheet = Image.new("RGBA", (canvas_w, canvas_h), background)

    for idx, img in enumerate(imgs[: rows * cols]):
        r = idx // cols
        c = idx % cols
        x = margin + c * (cell_w + gap) + (cell_w - img.width) // 2
        y = margin + r * (cell_h + gap) + (cell_h - img.height) // 2
        sheet.alpha_composite(img, (x, y))

    if scale != 1:
        sheet = sheet.resize((sheet.width * scale, sheet.height * scale), Image.Resampling.NEAREST)
    sheet.save(out_path)


def make_gif(frames_dir: Path, out_path: Path, duration: int = 120, loop: int = 0, scale: int = 1) -> None:
    files = read_frames(frames_dir)
    frames = [load_image(p) for p in files]
    if scale != 1:
        frames = [f.resize((f.width * scale, f.height * scale), Image.Resampling.NEAREST) for f in frames]
    # GIF needs palette mode; composite on transparent-safe black preview.
    pal_frames = []
    for f in frames:
        bg = Image.new("RGBA", f.size, (0, 0, 0, 255))
        bg.alpha_composite(f)
        pal_frames.append(bg.convert("P", palette=Image.Palette.ADAPTIVE, colors=256))
    pal_frames[0].save(out_path, save_all=True, append_images=pal_frames[1:], duration=duration, loop=loop)


def quantize_image(src: Path, out_path: Path, colors: int = 32, dither: bool = False) -> None:
    img = load_image(src)
    alpha = img.getchannel("A")
    rgb = Image.new("RGB", img.size, (0, 0, 0))
    rgb.paste(img.convert("RGB"), mask=alpha)
    q = rgb.quantize(colors=colors, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.FLOYDSTEINBERG if dither else Image.Dither.NONE)
    out = q.convert("RGBA")
    out.putalpha(alpha)
    out.save(out_path)


def remove_bg(src: Path, out_path: Path, color: RGB, tolerance: int = 0) -> None:
    img = load_image(src)
    pixels = img.load()
    tr, tg, tb = color
    for y in range(img.height):
        for x in range(img.width):
            r, g, b, a = pixels[x, y]
            if abs(r - tr) <= tolerance and abs(g - tg) <= tolerance and abs(b - tb) <= tolerance:
                pixels[x, y] = (r, g, b, 0)
    img.save(out_path)


def main() -> None:
    parser = argparse.ArgumentParser(description="Sprite sheet helper tools")
    sub = parser.add_subparsers(dest="cmd", required=True)

    p = sub.add_parser("crop", help="Crop a regular grid into individual frames")
    p.add_argument("src", type=Path)
    p.add_argument("out_dir", type=Path)
    p.add_argument("--rows", type=int, required=True)
    p.add_argument("--cols", type=int, required=True)
    p.add_argument("--margin-x", type=int, default=0)
    p.add_argument("--margin-y", type=int, default=0)
    p.add_argument("--gap-x", type=int, default=0)
    p.add_argument("--gap-y", type=int, default=0)
    p.add_argument("--cell-w", type=int)
    p.add_argument("--cell-h", type=int)
    p.add_argument("--prefix", default="frame")

    p = sub.add_parser("repack", help="Pack frames into a clean aligned sheet")
    p.add_argument("frames_dir", type=Path)
    p.add_argument("out", type=Path)
    p.add_argument("--rows", type=int, required=True)
    p.add_argument("--cols", type=int, required=True)
    p.add_argument("--cell", type=int)
    p.add_argument("--cell-w", type=int)
    p.add_argument("--cell-h", type=int)
    p.add_argument("--gap", type=int, default=0)
    p.add_argument("--margin", type=int, default=0)
    p.add_argument("--transparent", action="store_true")
    p.add_argument("--bg", type=parse_rgb, default=(0, 0, 0))
    p.add_argument("--scale", type=int, default=1)
    p.add_argument("--trim", action="store_true")

    p = sub.add_parser("gif", help="Create a GIF preview from frames")
    p.add_argument("frames_dir", type=Path)
    p.add_argument("out", type=Path)
    p.add_argument("--duration", type=int, default=120)
    p.add_argument("--loop", type=int, default=0)
    p.add_argument("--scale", type=int, default=1)

    p = sub.add_parser("quantize", help="Quantize image to a limited number of colours")
    p.add_argument("src", type=Path)
    p.add_argument("out", type=Path)
    p.add_argument("--colors", type=int, default=32)
    p.add_argument("--dither", action="store_true")

    p = sub.add_parser("remove-bg", help="Make a solid colour transparent")
    p.add_argument("src", type=Path)
    p.add_argument("out", type=Path)
    p.add_argument("--color", type=parse_rgb, required=True)
    p.add_argument("--tolerance", type=int, default=0)

    args = parser.parse_args()
    if args.cmd == "crop":
        crop_grid(args.src, args.out_dir, args.rows, args.cols, args.margin_x, args.margin_y, args.gap_x, args.gap_y, args.cell_w, args.cell_h, args.prefix)
    elif args.cmd == "repack":
        repack(args.frames_dir, args.out, args.rows, args.cols, args.cell, args.cell_w, args.cell_h, args.gap, args.margin, None if args.transparent else args.bg, args.scale, args.trim)
    elif args.cmd == "gif":
        make_gif(args.frames_dir, args.out, args.duration, args.loop, args.scale)
    elif args.cmd == "quantize":
        quantize_image(args.src, args.out, args.colors, args.dither)
    elif args.cmd == "remove-bg":
        remove_bg(args.src, args.out, args.color, args.tolerance)


if __name__ == "__main__":
    main()
