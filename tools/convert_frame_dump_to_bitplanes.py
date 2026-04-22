#!/usr/bin/env python3
"""
Convert index8 frame dumps into Amiga-style planar frame files.

Input is expected from `icepanic_sdl --dump-indexed`:
  - palette.rgb      (32 * RGB bytes)
  - frame_XXXXX.idx  (width*height bytes each, index8 values 0..31)
  - frame_info.txt   (optional metadata)

Output:
  - frame_XXXXX.bpl           (planar data, configurable layout)
  - conversion_info.txt
  - palette_12bit.bin         (32 big-endian words, 0x0RGB Amiga nibble format)
  - palette_12bit.txt         (text view of palette words)
"""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Dict, Iterable, List, Sequence, Tuple


def parse_key_value_lines(path: Path) -> Dict[str, str]:
    data: Dict[str, str] = {}
    if not path.exists():
        return data
    for raw_line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        if "=" not in line:
            continue
        k, v = line.split("=", 1)
        data[k.strip()] = v.strip()
    return data


def indices_to_planes(indices: Sequence[int], width: int, height: int, planes: int) -> List[bytes]:
    if width % 8 != 0:
        raise ValueError("width must be divisible by 8")
    bytes_per_row = width // 8
    plane_buffers = [bytearray(bytes_per_row * height) for _ in range(planes)]

    for y in range(height):
        row_base = y * width
        out_row = y * bytes_per_row
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
                plane_buffers[p][out_row + xb] = vals[p]

    return [bytes(p) for p in plane_buffers]


def flatten_planes(
    planes_data: Sequence[bytes],
    width: int,
    height: int,
    layout: str,
) -> bytes:
    bytes_per_row = width // 8
    if layout == "plane-major":
        return b"".join(planes_data)

    if layout == "row-interleaved":
        out = bytearray()
        for y in range(height):
            row_off = y * bytes_per_row
            for plane in planes_data:
                out.extend(plane[row_off:row_off + bytes_per_row])
        return bytes(out)

    raise ValueError(f"unsupported layout: {layout}")


def rgb8_to_amiga12_word(r: int, g: int, b: int) -> int:
    rn = (r >> 4) & 0xF
    gn = (g >> 4) & 0xF
    bn = (b >> 4) & 0xF
    return (rn << 8) | (gn << 4) | bn


def write_amiga_palette(rgb_path: Path, out_dir: Path) -> Tuple[Path, Path]:
    raw = rgb_path.read_bytes()
    if len(raw) < 32 * 3:
        raise ValueError(f"palette.rgb is too short: {len(raw)} bytes")
    raw = raw[: 32 * 3]

    words: List[int] = []
    for i in range(32):
        r = raw[i * 3 + 0]
        g = raw[i * 3 + 1]
        b = raw[i * 3 + 2]
        words.append(rgb8_to_amiga12_word(r, g, b))

    bin_path = out_dir / "palette_12bit.bin"
    txt_path = out_dir / "palette_12bit.txt"

    with bin_path.open("wb") as f:
        for w in words:
            f.write(bytes(((w >> 8) & 0xFF, w & 0xFF)))

    lines = ["# index -> 0xRGB"]
    for i, w in enumerate(words):
        lines.append(f"{i:02d} -> 0x{w:03X}")
    txt_path.write_text("\n".join(lines) + "\n", encoding="utf-8")
    return bin_path, txt_path


def iter_frame_files(input_dir: Path, frame_glob: str) -> Iterable[Path]:
    return sorted(input_dir.glob(frame_glob))


def crop_indices(
    indices: Sequence[int],
    src_width: int,
    src_height: int,
    crop_x: int,
    crop_y: int,
    crop_width: int,
    crop_height: int,
) -> bytes:
    if crop_x < 0 or crop_y < 0 or crop_width <= 0 or crop_height <= 0:
        raise ValueError("crop region must be positive and in-bounds")
    if crop_x + crop_width > src_width or crop_y + crop_height > src_height:
        raise ValueError("crop region exceeds source frame bounds")

    out = bytearray(crop_width * crop_height)
    out_off = 0
    for y in range(crop_height):
        src_row = (crop_y + y) * src_width + crop_x
        out[out_off:out_off + crop_width] = bytes(indices[src_row:src_row + crop_width])
        out_off += crop_width
    return bytes(out)


def main() -> None:
    parser = argparse.ArgumentParser(description="Convert dumped index8 frames to Amiga planar files.")
    parser.add_argument("--input-dir", default="assets/frame_dump", help="Directory with frame_*.idx and palette.rgb.")
    parser.add_argument("--output-dir", default="assets/frame_dump_5bpl", help="Output directory for planar frames.")
    parser.add_argument("--frame-glob", default="frame_*.idx", help="Glob pattern for input frame files.")
    parser.add_argument("--width", type=int, default=320, help="Frame width in pixels.")
    parser.add_argument("--height", type=int, default=200, help="Frame height in pixels.")
    parser.add_argument("--planes", type=int, default=5, help="Number of bitplanes (default 5).")
    parser.add_argument(
        "--layout",
        choices=("plane-major", "row-interleaved"),
        default="plane-major",
        help="Output planar layout.")
    parser.add_argument("--crop-x", type=int, default=0, help="Crop origin X in source frame.")
    parser.add_argument("--crop-y", type=int, default=0, help="Crop origin Y in source frame.")
    parser.add_argument("--crop-width", type=int, default=0, help="Crop width (0 = full width from crop-x).")
    parser.add_argument("--crop-height", type=int, default=0, help="Crop height (0 = full height from crop-y).")
    parser.add_argument("--max-frames", type=int, default=0, help="Limit converted frames (0 = all).")
    args = parser.parse_args()

    input_dir = Path(args.input_dir)
    output_dir = Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    info = parse_key_value_lines(input_dir / "frame_info.txt")
    src_width = int(info.get("width", args.width))
    src_height = int(info.get("height", args.height))
    planes = int(args.planes)
    if src_width <= 0 or src_height <= 0:
        raise ValueError("width/height must be positive")
    if planes <= 0 or planes > 8:
        raise ValueError("planes must be in 1..8")

    crop_x = int(args.crop_x)
    crop_y = int(args.crop_y)
    out_width = int(args.crop_width) if int(args.crop_width) > 0 else (src_width - crop_x)
    out_height = int(args.crop_height) if int(args.crop_height) > 0 else (src_height - crop_y)
    if crop_x < 0 or crop_y < 0 or out_width <= 0 or out_height <= 0:
        raise ValueError("invalid crop values")
    if crop_x + out_width > src_width or crop_y + out_height > src_height:
        raise ValueError("crop region is out of bounds")
    if out_width % 8 != 0:
        raise ValueError("output width must be divisible by 8")

    palette_path = input_dir / "palette.rgb"
    if not palette_path.exists():
        raise FileNotFoundError(f"Missing palette: {palette_path}")
    pal_bin, pal_txt = write_amiga_palette(palette_path, output_dir)

    frames = list(iter_frame_files(input_dir, args.frame_glob))
    if args.max_frames > 0:
        frames = frames[: args.max_frames]
    if not frames:
        raise FileNotFoundError(f"No frames found with glob '{args.frame_glob}' in {input_dir}")

    expected_size = src_width * src_height
    converted = 0
    first_out = ""
    for frame_path in frames:
        raw = frame_path.read_bytes()
        if len(raw) != expected_size:
            print(f"Skipping {frame_path.name}: expected {expected_size} bytes, got {len(raw)}")
            continue

        cropped = crop_indices(raw, src_width, src_height, crop_x, crop_y, out_width, out_height)
        planes_data = indices_to_planes(cropped, out_width, out_height, planes)
        packed = flatten_planes(planes_data, out_width, out_height, args.layout)
        out_name = frame_path.stem + ".bpl"
        out_path = output_dir / out_name
        out_path.write_bytes(packed)
        if converted == 0:
            first_out = out_name
        converted += 1

    if converted == 0:
        raise RuntimeError("No frames were converted.")

    bytes_per_row = out_width // 8
    plane_size = bytes_per_row * out_height
    out_size = plane_size * planes if args.layout == "plane-major" else plane_size * planes
    info_lines = [
        "# Frame Dump Planar Conversion",
        f"input_dir={input_dir}",
        f"output_dir={output_dir}",
        f"source_width={src_width}",
        f"source_height={src_height}",
        f"crop_x={crop_x}",
        f"crop_y={crop_y}",
        f"width={out_width}",
        f"height={out_height}",
        f"bitplanes={planes}",
        f"layout={args.layout}",
        f"bytes_per_row={bytes_per_row}",
        f"bytes_per_plane={plane_size}",
        f"bytes_per_output_frame={out_size}",
        f"frames_converted={converted}",
        f"first_output_file={first_out}",
        f"palette_12bit_bin={pal_bin.name}",
        f"palette_12bit_txt={pal_txt.name}",
    ]
    (output_dir / "conversion_info.txt").write_text("\n".join(info_lines) + "\n", encoding="utf-8")

    print(f"Converted {converted} frame(s) to {output_dir}")
    print(f"Wrote {pal_bin.name}, {pal_txt.name}, conversion_info.txt")


if __name__ == "__main__":
    main()
