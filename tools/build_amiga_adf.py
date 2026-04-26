#!/usr/bin/env python3
"""Build a bootable AmigaDOS ADF for the Icepanic Amiga executable."""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


DEFAULT_RELEASE_BASENAME = "icepanic-v0.9.0-beta.4-amiga-ecs-pal"


def run_xdftool(xdftool: str, args: list[str]) -> None:
    cmd = [xdftool, *args]
    try:
        subprocess.run(cmd, check=True)
    except FileNotFoundError:
        raise RuntimeError(
            f"'{xdftool}' was not found. Install amitools or set XDFTOOL to the xdftool executable."
        ) from None
    except subprocess.CalledProcessError as exc:
        raise RuntimeError(f"xdftool failed with exit code {exc.returncode}: {' '.join(cmd)}") from exc


def build_adf(
    exe_path: Path,
    output_path: Path,
    volume_name: str,
    xdftool: str,
    disk_info: Path | None,
    nfo_path: Path | None,
) -> None:
    if not exe_path.is_file():
        raise RuntimeError(f"Amiga executable not found: {exe_path}")

    output_path.parent.mkdir(parents=True, exist_ok=True)
    xdftool_path = shutil.which(xdftool) or xdftool

    with tempfile.TemporaryDirectory(prefix="icepanic_adf_") as tmp:
        tmp_dir = Path(tmp)
        startup_sequence = tmp_dir / "startup-sequence"
        startup_sequence.write_text("SYS:Icepanic\n", encoding="ascii", newline="\n")

        cmd = [
            "-f",
            str(output_path),
            "format",
            volume_name,
            "DOS0",
            "+",
            "boot",
            "install",
            "boot1x",
            "+",
            "makedir",
            "S",
            "+",
            "write",
            str(startup_sequence),
            "S/startup-sequence",
            "+",
            "write",
            str(exe_path),
            "Icepanic",
            "+",
            "protect",
            "Icepanic",
            "rwed",
        ]

        if disk_info and disk_info.is_file():
            cmd.extend(["+", "write", str(disk_info), "disk.info"])

        if nfo_path and nfo_path.is_file():
            cmd.extend(
                [
                    "+",
                    "write",
                    str(nfo_path),
                    "Icepanic.nfo",
                    "+",
                    "protect",
                    "Icepanic.nfo",
                    "rwed",
                ]
            )

        run_xdftool(xdftool_path, cmd)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--exe",
        default=f"build/amiga/{DEFAULT_RELEASE_BASENAME}",
        help="Amiga hunk executable to package.",
    )
    parser.add_argument(
        "--output",
        default=f"build/amiga/{DEFAULT_RELEASE_BASENAME}.adf",
        help="Output ADF image path.",
    )
    parser.add_argument("--volume", default="Icepanic", help="AmigaDOS volume name.")
    parser.add_argument("--xdftool", default="xdftool", help="xdftool executable from amitools.")
    parser.add_argument(
        "--disk-info",
        default="build/amiga/disk.info",
        help="Optional disk.info icon to include when present.",
    )
    parser.add_argument(
        "--nfo",
        default="ICEPANIC.NFO",
        help="Optional release NFO to include as Icepanic.nfo when present.",
    )
    args = parser.parse_args()

    try:
        build_adf(
            Path(args.exe),
            Path(args.output),
            args.volume,
            args.xdftool,
            Path(args.disk_info) if args.disk_info else None,
            Path(args.nfo) if args.nfo else None,
        )
    except RuntimeError as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 1

    print(f"Wrote {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
