#!/usr/bin/env python3
"""
Validate Amiga-oriented planar assets produced by Icepanic tools.

Checks:
  - tile strip binary size against metadata
  - tile strip decode->encode round-trip for first tile
  - optional frame .bpl size and decode->encode round-trip
"""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Dict, List, Sequence


def parse_key_value_lines(path: Path) -> Dict[str, str]:
    data: Dict[str, str] = {}
    for raw_line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        k, v = line.split("=", 1)
        data[k.strip()] = v.strip()
    return data


def decode_planes_to_indices(
    packed: bytes,
    width: int,
    height: int,
    planes: int,
    layout: str,
) -> List[int]:
    if width % 8 != 0:
        raise ValueError("width must be divisible by 8")
    bytes_per_row = width // 8
    plane_size = bytes_per_row * height
    expected = plane_size * planes
    if len(packed) != expected:
        raise ValueError(f"packed data size mismatch: got={len(packed)} expected={expected}")

    plane_buffers: List[bytes] = []
    if layout == "plane-major":
        for p in range(planes):
            off = p * plane_size
            plane_buffers.append(packed[off:off + plane_size])
    elif layout == "row-interleaved":
        mutable = [bytearray(plane_size) for _ in range(planes)]
        read_off = 0
        for y in range(height):
            row_off = y * bytes_per_row
            for p in range(planes):
                mutable[p][row_off:row_off + bytes_per_row] = packed[read_off:read_off + bytes_per_row]
                read_off += bytes_per_row
        plane_buffers = [bytes(m) for m in mutable]
    else:
        raise ValueError(f"unsupported layout: {layout}")

    out = [0 for _ in range(width * height)]
    for y in range(height):
        row_off = y * bytes_per_row
        for xb in range(bytes_per_row):
            masks = [plane_buffers[p][row_off + xb] for p in range(planes)]
            for bit in range(8):
                x = xb * 8 + bit
                pix = 0
                mask = 1 << (7 - bit)
                for p in range(planes):
                    if masks[p] & mask:
                        pix |= (1 << p)
                out[y * width + x] = pix
    return out


def encode_indices_to_planes(
    indices: Sequence[int],
    width: int,
    height: int,
    planes: int,
    layout: str,
) -> bytes:
    if width % 8 != 0:
        raise ValueError("width must be divisible by 8")
    bytes_per_row = width // 8
    plane_size = bytes_per_row * height
    plane_buf = [bytearray(plane_size) for _ in range(planes)]

    for y in range(height):
        row_base = y * width
        row_off = y * bytes_per_row
        for xb in range(bytes_per_row):
            vals = [0 for _ in range(planes)]
            for bit in range(8):
                x = xb * 8 + bit
                idx = int(indices[row_base + x]) & ((1 << planes) - 1)
                mask = 1 << (7 - bit)
                for p in range(planes):
                    if (idx >> p) & 1:
                        vals[p] |= mask
            for p in range(planes):
                plane_buf[p][row_off + xb] = vals[p]

    if layout == "plane-major":
        return b"".join(bytes(p) for p in plane_buf)
    if layout == "row-interleaved":
        out = bytearray()
        for y in range(height):
            row_off = y * bytes_per_row
            for p in range(planes):
                out.extend(plane_buf[p][row_off:row_off + bytes_per_row])
        return bytes(out)
    raise ValueError(f"unsupported layout: {layout}")


def validate_tile_strip(tile_strip: Path, tile_info: Path) -> None:
    info = parse_key_value_lines(tile_info)
    tile_count = int(info.get("tile_count", "0"))
    tile_size_raw = info.get("tile_size", "16x16")
    w_str, h_str = tile_size_raw.lower().split("x", 1)
    width = int(w_str)
    height = int(h_str)
    planes = int(info.get("bitplanes", "5"))
    layout_field = info.get("layout", "tile-major,plane-major")
    layout = "plane-major"
    if "," in layout_field:
        layout = layout_field.split(",")[-1].strip()

    data = tile_strip.read_bytes()
    tile_bytes = (width // 8) * height * planes
    expected = tile_count * tile_bytes
    if len(data) != expected:
        raise RuntimeError(f"tile strip size mismatch: got={len(data)} expected={expected}")

    if tile_count > 0:
        first = data[:tile_bytes]
        idx = decode_planes_to_indices(first, width, height, planes, layout)
        rebuilt = encode_indices_to_planes(idx, width, height, planes, layout)
        if rebuilt != first:
            raise RuntimeError("tile strip round-trip failed on first tile")

    print(f"[OK] tile strip size and round-trip: {tile_strip} ({len(data)} bytes)")


def validate_frame_bpl(frame_bpl: Path, width: int, height: int, planes: int, layout: str) -> None:
    data = frame_bpl.read_bytes()
    expected = (width // 8) * height * planes
    if len(data) != expected:
        raise RuntimeError(f"frame size mismatch: got={len(data)} expected={expected}")

    idx = decode_planes_to_indices(data, width, height, planes, layout)
    rebuilt = encode_indices_to_planes(idx, width, height, planes, layout)
    if rebuilt != data:
        raise RuntimeError("frame round-trip failed")

    print(f"[OK] frame size and round-trip: {frame_bpl} ({len(data)} bytes)")


def main() -> None:
    parser = argparse.ArgumentParser(description="Validate planar tile/frame assets.")
    parser.add_argument("--tile-strip", default="assets/tile_strip_16x16_5bpl.bin", help="Tile strip binary path.")
    parser.add_argument("--tile-info", default="assets/tile_strip_16x16_5bpl.txt", help="Tile strip metadata path.")
    parser.add_argument("--frame-bpl", default="", help="Optional frame .bpl file to validate.")
    parser.add_argument("--width", type=int, default=320, help="Frame width for --frame-bpl.")
    parser.add_argument("--height", type=int, default=200, help="Frame height for --frame-bpl.")
    parser.add_argument("--planes", type=int, default=5, help="Frame bitplane count for --frame-bpl.")
    parser.add_argument(
        "--frame-layout",
        choices=("plane-major", "row-interleaved"),
        default="plane-major",
        help="Frame layout used by --frame-bpl.")
    args = parser.parse_args()

    tile_strip = Path(args.tile_strip)
    tile_info = Path(args.tile_info)
    if not tile_strip.exists():
        raise FileNotFoundError(f"Missing tile strip: {tile_strip}")
    if not tile_info.exists():
        raise FileNotFoundError(f"Missing tile info: {tile_info}")
    validate_tile_strip(tile_strip, tile_info)

    if args.frame_bpl:
        validate_frame_bpl(Path(args.frame_bpl), args.width, args.height, args.planes, args.frame_layout)

    print("All selected planar asset checks passed.")


if __name__ == "__main__":
    main()

