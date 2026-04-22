#!/usr/bin/env python3
"""
One-shot helper: convert dumped index8 frames into one packed Amiga test bundle.

Bundle format (little-endian header, then big-endian palette words):
  u8[4]  magic = "IAB1"
  u16    version = 1
  u16    flags   (bit0: row-interleaved layout, bit1: delta-xor frames)
  u16    width
  u16    height
  u16    bitplanes
  u16    reserved0
  u32    frame_count
  u32    frame_size_bytes
  u16    palette_count (always 32)
  u16    reserved1
  u16[32] palette words (big-endian 0x0RGB)
  u8[]    frame_data (frame_count * frame_size_bytes)
"""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import List

from convert_frame_dump_to_bitplanes import (
    crop_indices,
    flatten_planes,
    indices_to_planes,
    parse_key_value_lines,
    rgb8_to_amiga12_word,
)


def read_palette_words(palette_rgb_path: Path) -> List[int]:
    raw = palette_rgb_path.read_bytes()
    if len(raw) < 32 * 3:
        raise ValueError(f"palette.rgb is too short: {len(raw)} bytes")
    raw = raw[: 32 * 3]
    words: List[int] = []
    for i in range(32):
        r = raw[i * 3 + 0]
        g = raw[i * 3 + 1]
        b = raw[i * 3 + 2]
        words.append(rgb8_to_amiga12_word(r, g, b))
    return words


def main() -> None:
    parser = argparse.ArgumentParser(description="Build packed Amiga frame bundle from dumped index8 frames.")
    parser.add_argument("--input-dir", default="assets/frame_dump", help="Directory with frame_*.idx and palette.rgb.")
    parser.add_argument("--output", default="assets/amiga_frame_bundle.iab", help="Output packed bundle path.")
    parser.add_argument("--info", default="assets/amiga_frame_bundle.txt", help="Output metadata text path.")
    parser.add_argument("--frame-glob", default="frame_*.idx", help="Glob pattern for dumped index frames.")
    parser.add_argument("--width", type=int, default=320, help="Source frame width.")
    parser.add_argument("--height", type=int, default=200, help="Source frame height.")
    parser.add_argument("--planes", type=int, default=5, help="Bitplane count.")
    parser.add_argument(
        "--layout",
        choices=("plane-major", "row-interleaved"),
        default="plane-major",
        help="Packed frame planar layout.")
    parser.add_argument(
        "--delta-xor",
        action="store_true",
        help="Store frame 0 raw and subsequent frames as XOR deltas vs previous frame.")
    parser.add_argument("--crop-x", type=int, default=0, help="Crop origin X in source frame.")
    parser.add_argument("--crop-y", type=int, default=0, help="Crop origin Y in source frame.")
    parser.add_argument("--crop-width", type=int, default=0, help="Crop width (0 = full width from crop-x).")
    parser.add_argument("--crop-height", type=int, default=0, help="Crop height (0 = full height from crop-y).")
    parser.add_argument("--max-frames", type=int, default=0, help="Limit frames (0 = all).")
    args = parser.parse_args()

    input_dir = Path(args.input_dir)
    output_path = Path(args.output)
    info_path = Path(args.info)
    output_path.parent.mkdir(parents=True, exist_ok=True)
    info_path.parent.mkdir(parents=True, exist_ok=True)

    frame_info = parse_key_value_lines(input_dir / "frame_info.txt")
    src_width = int(frame_info.get("width", args.width))
    src_height = int(frame_info.get("height", args.height))
    planes = int(args.planes)
    if src_width <= 0 or src_height <= 0:
        raise ValueError("source width/height must be positive")
    if planes <= 0 or planes > 8:
        raise ValueError("planes must be in 1..8")

    crop_x = int(args.crop_x)
    crop_y = int(args.crop_y)
    out_width = int(args.crop_width) if int(args.crop_width) > 0 else (src_width - crop_x)
    out_height = int(args.crop_height) if int(args.crop_height) > 0 else (src_height - crop_y)
    if crop_x < 0 or crop_y < 0 or out_width <= 0 or out_height <= 0:
        raise ValueError("invalid crop values")
    if crop_x + out_width > src_width or crop_y + out_height > src_height:
        raise ValueError("crop region out of source bounds")
    if out_width % 8 != 0:
        raise ValueError("output width must be divisible by 8")

    palette_path = input_dir / "palette.rgb"
    if not palette_path.exists():
        raise FileNotFoundError(f"Missing palette file: {palette_path}")
    palette_words = read_palette_words(palette_path)

    frame_paths = sorted(input_dir.glob(args.frame_glob))
    if args.max_frames > 0:
        frame_paths = frame_paths[: args.max_frames]
    if not frame_paths:
        raise FileNotFoundError(f"No frames found with glob '{args.frame_glob}' in {input_dir}")

    expected_src_size = src_width * src_height
    frame_blobs: List[bytes] = []
    kept_names: List[str] = []
    for p in frame_paths:
        raw = p.read_bytes()
        if len(raw) != expected_src_size:
            print(f"Skipping {p.name}: expected {expected_src_size}, got {len(raw)}")
            continue
        cropped = crop_indices(raw, src_width, src_height, crop_x, crop_y, out_width, out_height)
        planes_data = indices_to_planes(cropped, out_width, out_height, planes)
        packed = flatten_planes(planes_data, out_width, out_height, args.layout)
        frame_blobs.append(packed)
        kept_names.append(p.name)

    if not frame_blobs:
        raise RuntimeError("No valid frames to pack.")

    frame_size = len(frame_blobs[0])
    for blob in frame_blobs:
        if len(blob) != frame_size:
            raise RuntimeError("inconsistent packed frame sizes")

    stored_blobs: List[bytes] = []
    delta_nonzero_bytes = 0
    if args.delta_xor:
        prev = frame_blobs[0]
        stored_blobs.append(prev)
        for i in range(1, len(frame_blobs)):
            cur = frame_blobs[i]
            delta = bytearray(frame_size)
            nz = 0
            for j in range(frame_size):
                v = cur[j] ^ prev[j]
                delta[j] = v
                if v != 0:
                    nz += 1
            delta_nonzero_bytes += nz
            stored_blobs.append(bytes(delta))
            prev = cur
    else:
        stored_blobs = frame_blobs

    flags = 1 if args.layout == "row-interleaved" else 0
    if args.delta_xor:
        flags |= 2
    with output_path.open("wb") as f:
        f.write(b"IAB1")
        f.write((1).to_bytes(2, "little"))  # version
        f.write(flags.to_bytes(2, "little"))
        f.write(out_width.to_bytes(2, "little"))
        f.write(out_height.to_bytes(2, "little"))
        f.write(planes.to_bytes(2, "little"))
        f.write((0).to_bytes(2, "little"))  # reserved0
        f.write(len(frame_blobs).to_bytes(4, "little"))
        f.write(frame_size.to_bytes(4, "little"))
        f.write((32).to_bytes(2, "little"))
        f.write((0).to_bytes(2, "little"))  # reserved1
        for w in palette_words:
            f.write(bytes(((w >> 8) & 0xFF, w & 0xFF)))  # big-endian Amiga word
        for blob in stored_blobs:
            f.write(blob)

    bytes_per_row = out_width // 8
    bytes_per_plane = bytes_per_row * out_height
    info_lines = [
        "# Icepanic Amiga Bundle",
        f"bundle={output_path}",
        "magic=IAB1",
        "version=1",
        f"layout={args.layout}",
        f"flags={flags}",
        f"source_width={src_width}",
        f"source_height={src_height}",
        f"crop_x={crop_x}",
        f"crop_y={crop_y}",
        f"width={out_width}",
        f"height={out_height}",
        f"bitplanes={planes}",
        f"bytes_per_row={bytes_per_row}",
        f"bytes_per_plane={bytes_per_plane}",
        f"frame_size={frame_size}",
        f"frame_count={len(frame_blobs)}",
        f"palette_words={len(palette_words)}",
        f"frame_storage={'delta-xor' if args.delta_xor else 'raw'}",
        f"delta_nonzero_bytes={delta_nonzero_bytes}",
        "frames=" + ",".join(kept_names),
    ]
    info_path.write_text("\n".join(info_lines) + "\n", encoding="utf-8")

    print(f"Packed {len(frame_blobs)} frame(s) into {output_path}")
    print(f"Wrote {info_path}")


if __name__ == "__main__":
    main()
