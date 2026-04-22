#!/usr/bin/env python3
"""
Check parity between source dumped index8 frames and packed IAB1 bundle frames.

The checker reconstructs each planar frame from the bundle (including delta-XOR mode),
decodes it back to index8 pixels, and compares against the corresponding source frame.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Dict, List, Sequence


IAB_FLAG_ROW_INTERLEAVED = 1
IAB_FLAG_DELTA_XOR = 2


def parse_key_value_lines(path: Path) -> Dict[str, str]:
    data: Dict[str, str] = {}
    if not path.exists():
        return data
    for raw_line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        k, v = line.split("=", 1)
        data[k.strip()] = v.strip()
    return data


def read_u16_le(data: bytes, off: int) -> int:
    return int(data[off] | (data[off + 1] << 8))


def read_u32_le(data: bytes, off: int) -> int:
    return int(data[off] | (data[off + 1] << 8) | (data[off + 2] << 16) | (data[off + 3] << 24))


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
        raise ValueError("crop must be positive and in-bounds")
    if crop_x + crop_width > src_width or crop_y + crop_height > src_height:
        raise ValueError("crop exceeds source bounds")

    out = bytearray(crop_width * crop_height)
    out_off = 0
    for y in range(crop_height):
        src_row = (crop_y + y) * src_width + crop_x
        out[out_off:out_off + crop_width] = bytes(indices[src_row:src_row + crop_width])
        out_off += crop_width
    return bytes(out)


def decode_planes_to_indices(
    packed: bytes,
    width: int,
    height: int,
    planes: int,
    layout: str,
) -> bytes:
    if width % 8 != 0:
        raise ValueError("bundle width must be divisible by 8")

    bytes_per_row = width // 8
    plane_size = bytes_per_row * height
    expected_size = plane_size * planes
    if len(packed) != expected_size:
        raise ValueError(f"packed frame size mismatch: got={len(packed)} expected={expected_size}")

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

    out = bytearray(width * height)
    for y in range(height):
        row_off = y * bytes_per_row
        out_row = y * width
        for xb in range(bytes_per_row):
            masks = [plane_buffers[p][row_off + xb] for p in range(planes)]
            for bit in range(8):
                mask = 1 << (7 - bit)
                idx = 0
                for p in range(planes):
                    if masks[p] & mask:
                        idx |= (1 << p)
                out[out_row + xb * 8 + bit] = idx
    return bytes(out)


def main() -> None:
    parser = argparse.ArgumentParser(description="Check source index8 frame parity against IAB1 bundle frames.")
    parser.add_argument("--bundle", default="assets/amiga_frame_bundle.iab", help="Input IAB1 bundle path.")
    parser.add_argument("--input-dir", default="assets/frame_dump", help="Source dump directory with frame_*.idx.")
    parser.add_argument("--frame-glob", default="frame_*.idx", help="Source frame glob.")
    parser.add_argument("--source-width", type=int, default=320, help="Fallback source width if frame_info is missing.")
    parser.add_argument("--source-height", type=int, default=200, help="Fallback source height if frame_info is missing.")
    parser.add_argument("--crop-x", type=int, default=0, help="Source crop X used for bundle generation.")
    parser.add_argument("--crop-y", type=int, default=0, help="Source crop Y used for bundle generation.")
    parser.add_argument("--start-frame", type=int, default=0, help="First frame index to compare.")
    parser.add_argument("--frame-step", type=int, default=1, help="Compare every Nth frame (>=1).")
    parser.add_argument("--max-frames", type=int, default=0, help="Max frames to compare (0 = all possible).")
    parser.add_argument("--report", default="", help="Optional output report text path.")
    parser.add_argument(
        "--report-format",
        choices=("full", "compact"),
        default="full",
        help="Report verbosity when --report is used.")
    parser.add_argument(
        "--report-max-mismatches",
        type=int,
        default=0,
        help="Cap mismatch lines included in report/console (0 = no cap).")
    parser.add_argument(
        "--script",
        action="store_true",
        help="Emit one-line machine-readable PARITY_OK/PARITY_FAIL/PARITY_ERROR output.")
    parser.add_argument(
        "--json",
        action="store_true",
        help="Emit a one-line JSON summary object for CI parsing.")
    parser.add_argument(
        "--pr-summary",
        action="store_true",
        help="Emit a compact PR-friendly one-line summary.")
    args = parser.parse_args()

    try:
        mode_count = int(args.script) + int(args.json) + int(args.pr_summary)
        if mode_count > 1:
            raise ValueError("choose at most one of --script, --json, or --pr-summary")

        bundle_path = Path(args.bundle)
        input_dir = Path(args.input_dir)
        if not bundle_path.exists():
            raise FileNotFoundError(f"bundle not found: {bundle_path}")
        if not input_dir.exists():
            raise FileNotFoundError(f"input dir not found: {input_dir}")

        frame_info = parse_key_value_lines(input_dir / "frame_info.txt")
        src_width = int(frame_info.get("width", args.source_width))
        src_height = int(frame_info.get("height", args.source_height))
        if src_width <= 0 or src_height <= 0:
            raise ValueError("source width/height must be positive")

        data = bundle_path.read_bytes()
        if len(data) < 28 + 64:
            raise RuntimeError("bundle is too small to be valid IAB1")
        if data[0:4] != b"IAB1":
            raise RuntimeError("invalid bundle magic (expected IAB1)")

        version = read_u16_le(data, 4)
        flags = read_u16_le(data, 6)
        width = read_u16_le(data, 8)
        height = read_u16_le(data, 10)
        planes = read_u16_le(data, 12)
        frame_count = read_u32_le(data, 16)
        frame_size = read_u32_le(data, 20)
        palette_count = read_u16_le(data, 24)
        if version != 1:
            raise RuntimeError(f"unsupported IAB version: {version}")
        if palette_count != 32:
            raise RuntimeError(f"unsupported palette count: {palette_count}")
        if width <= 0 or height <= 0 or planes <= 0 or planes > 8:
            raise RuntimeError("invalid bundle geometry or plane count")

        bytes_per_row = width // 8
        expected_frame_size = bytes_per_row * height * planes
        if frame_size != expected_frame_size:
            raise RuntimeError(
                f"bundle frame_size mismatch: header={frame_size} expected={expected_frame_size} from dimensions")

        frame_data_off = 28 + palette_count * 2
        required_size = frame_data_off + frame_count * frame_size
        if len(data) < required_size:
            raise RuntimeError(
                f"bundle truncated: bytes={len(data)} required={required_size}")

        layout = "row-interleaved" if (flags & IAB_FLAG_ROW_INTERLEAVED) != 0 else "plane-major"
        delta_xor = (flags & IAB_FLAG_DELTA_XOR) != 0

        source_frames = sorted(input_dir.glob(args.frame_glob))
        if not source_frames:
            raise FileNotFoundError(f"no source frames found: {input_dir / args.frame_glob}")

        start_frame = int(args.start_frame)
        frame_step = int(args.frame_step)
        if start_frame < 0:
            raise ValueError("start-frame must be >= 0")
        if frame_step <= 0:
            raise ValueError("frame-step must be >= 1")
        report_max_mismatches = int(args.report_max_mismatches)
        if report_max_mismatches < 0:
            raise ValueError("report-max-mismatches must be >= 0")

        max_common_frames = min(frame_count, len(source_frames))
        selected_indices: List[int] = []
        idx = start_frame
        while idx < max_common_frames:
            selected_indices.append(idx)
            idx += frame_step
        if args.max_frames > 0:
            selected_indices = selected_indices[: int(args.max_frames)]
        compare_count = len(selected_indices)
        if compare_count <= 0:
            raise RuntimeError("no frames selected for comparison")

        crop_x = int(args.crop_x)
        crop_y = int(args.crop_y)
        if crop_x < 0 or crop_y < 0:
            raise ValueError("crop values must be >= 0")
        if crop_x + width > src_width or crop_y + height > src_height:
            raise ValueError("bundle dimensions + crop exceed source frame bounds")

        mismatches = 0
        total_diff_pixels = 0
        mismatch_lines: List[str] = []

        selected_set = set(selected_indices)
        max_selected = selected_indices[-1]
        recon_frame: bytes | None = None

        for i in range(max_selected + 1):
            packed_off = frame_data_off + i * frame_size
            packed_blob = data[packed_off:packed_off + frame_size]
            if not delta_xor:
                current_packed = packed_blob
            elif i == 0:
                current_packed = packed_blob
            else:
                if recon_frame is None:
                    raise RuntimeError("delta reconstruction state is invalid")
                xor_out = bytearray(frame_size)
                for j in range(frame_size):
                    xor_out[j] = recon_frame[j] ^ packed_blob[j]
                current_packed = bytes(xor_out)

            recon_frame = current_packed
            if i not in selected_set:
                continue

            bundle_indices = decode_planes_to_indices(current_packed, width, height, planes, layout)

            src_blob = source_frames[i].read_bytes()
            expected_src_size = src_width * src_height
            if len(src_blob) != expected_src_size:
                raise RuntimeError(
                    f"source frame has wrong size: {source_frames[i].name} got={len(src_blob)} expected={expected_src_size}")
            src_cropped = crop_indices(src_blob, src_width, src_height, crop_x, crop_y, width, height)

            if bundle_indices != src_cropped:
                mismatches += 1
                frame_diff = sum(1 for a, b in zip(bundle_indices, src_cropped) if a != b)
                total_diff_pixels += frame_diff
                line = f"frame={i:05d} source={source_frames[i].name} diff_pixels={frame_diff}"
                if report_max_mismatches == 0 or len(mismatch_lines) < report_max_mismatches:
                    mismatch_lines.append(line)
                    print(f"[MISMATCH] {line}")

        mismatch_lines_truncated = mismatches - len(mismatch_lines)

        if args.report:
            report_path = Path(args.report)
            report_path.parent.mkdir(parents=True, exist_ok=True)
            if args.report_format == "compact":
                status = "ok" if mismatches == 0 else "fail"
                report_lines = [
                    f"PARITY_REPORT status={status} bundle={bundle_path.name} compared={compare_count} "
                    f"mismatches={mismatches} diff_pixels={total_diff_pixels} layout={layout} "
                    f"delta_xor={'1' if delta_xor else '0'} start={start_frame} step={frame_step}"
                ]
                if mismatch_lines:
                    sample_count = 5 if report_max_mismatches == 0 else report_max_mismatches
                    report_lines.append("mismatch_samples:")
                    report_lines.extend(mismatch_lines[:sample_count])
                    if mismatch_lines_truncated > 0:
                        report_lines.append(
                            f"mismatch_samples_truncated={mismatch_lines_truncated}")
            else:
                report_lines = [
                    "# IAB1 Parity Report",
                    f"bundle={bundle_path}",
                    f"input_dir={input_dir}",
                    f"layout={layout}",
                    f"delta_xor={'yes' if delta_xor else 'no'}",
                    f"bundle_width={width}",
                    f"bundle_height={height}",
                    f"bitplanes={planes}",
                    f"source_width={src_width}",
                    f"source_height={src_height}",
                    f"crop_x={crop_x}",
                    f"crop_y={crop_y}",
                    f"start_frame={start_frame}",
                    f"frame_step={frame_step}",
                    f"compared_frames={compare_count}",
                    f"mismatched_frames={mismatches}",
                    f"total_diff_pixels={total_diff_pixels}",
                    f"mismatch_lines_reported={len(mismatch_lines)}",
                    f"mismatch_lines_truncated={mismatch_lines_truncated}",
                ]
                if mismatch_lines:
                    report_lines.append("mismatches:")
                    report_lines.extend(mismatch_lines)
            report_path.write_text("\n".join(report_lines) + "\n", encoding="utf-8")
            print(f"[INFO] wrote report: {report_path}")

        if mismatches > 0:
            if args.pr_summary:
                print(
                    f"PARITY_SUMMARY status=FAIL bundle={bundle_path.name} compared={compare_count} "
                    f"mismatches={mismatches} diff_pixels={total_diff_pixels} layout={layout} "
                    f"delta_xor={'1' if delta_xor else '0'} start={start_frame} step={frame_step}")
                raise SystemExit(2)
            if args.json:
                print(json.dumps({
                    "status": "fail",
                    "mismatches": mismatches,
                    "compared": compare_count,
                    "diff_pixels": total_diff_pixels,
                    "layout": layout,
                    "delta_xor": 1 if delta_xor else 0,
                    "start": start_frame,
                    "step": frame_step,
                }, separators=(",", ":")))
                raise SystemExit(2)
            if args.script:
                print(
                    f"PARITY_FAIL mismatches={mismatches} compared={compare_count} diff_pixels={total_diff_pixels} "
                    f"layout={layout} delta_xor={'1' if delta_xor else '0'} start={start_frame} step={frame_step}")
                raise SystemExit(2)
            raise RuntimeError(
                f"parity failed: mismatched_frames={mismatches}/{compare_count} total_diff_pixels={total_diff_pixels}")

        if args.json:
            print(json.dumps({
                "status": "ok",
                "mismatches": 0,
                "compared": compare_count,
                "diff_pixels": 0,
                "layout": layout,
                "delta_xor": 1 if delta_xor else 0,
                "start": start_frame,
                "step": frame_step,
            }, separators=(",", ":")))
            return

        if args.pr_summary:
            print(
                f"PARITY_SUMMARY status=OK bundle={bundle_path.name} compared={compare_count} "
                f"mismatches=0 diff_pixels=0 layout={layout} "
                f"delta_xor={'1' if delta_xor else '0'} start={start_frame} step={frame_step}")
            return

        if args.script:
            print(
                f"PARITY_OK mismatches=0 compared={compare_count} diff_pixels=0 "
                f"layout={layout} delta_xor={'1' if delta_xor else '0'} start={start_frame} step={frame_step}")
            return

        print(
            f"[OK] parity passed for {compare_count} frame(s) "
            f"(layout={layout}, delta_xor={'yes' if delta_xor else 'no'}, size={width}x{height}x{planes}bpl)")
    except SystemExit:
        raise
    except Exception as exc:
        bundle_name = Path(getattr(args, "bundle", "bundle.iab")).name
        if args.pr_summary:
            reason = str(exc).replace("\n", " ").strip()
            print(f"PARITY_SUMMARY status=ERROR bundle={bundle_name} reason={reason}")
            raise SystemExit(1)
        if args.json:
            print(json.dumps({
                "status": "error",
                "reason": str(exc),
            }, separators=(",", ":")))
            raise SystemExit(1)
        if args.script:
            reason = str(exc).replace(" ", "_")
            print(f"PARITY_ERROR reason={reason}")
            raise SystemExit(1)
        raise


if __name__ == "__main__":
    main()
