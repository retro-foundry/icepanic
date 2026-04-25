#!/usr/bin/env python3
"""
Generate low-fi Paula sample assets from the PC SDL sound routines.

The PC build synthesizes its arcade sounds at runtime with square-wave sweeps
and filtered noise.  The Amiga build should not spend frame time doing that, so
this script bakes the same patterns into signed 8-bit PCM one-shots that Paula
can DMA directly from chip RAM.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Dict, Iterable, List, Sequence, Tuple


SAMPLE_RATE = 8000
PAL_CLOCK = 3546895
TICK_HZ = 50
TICK_SAMPLES = SAMPLE_RATE // TICK_HZ
GUARD_TAIL_TICKS = 2
MAX_S16 = 32767
MIN_S16 = -32768


def clamp(value: int, lo: int, hi: int) -> int:
    return lo if value < lo else hi if value > hi else value


def s16_to_s8(value: int) -> int:
    value = clamp(value, MIN_S16, MAX_S16)
    return clamp(int(round(value / 256.0)), -127, 127)


class SampleBuilder:
    def __init__(self) -> None:
        self.samples: List[int] = []
        self.noise_state = 0x9E3779B9

    def silence(self, duration_ms: int) -> None:
        count = max(1, (SAMPLE_RATE * duration_ms) // 1000)
        self.samples.extend([0] * count)

    def tone_sweep(
        self,
        hz_start: int,
        hz_end: int,
        duration_ms: int,
        amplitude: int,
        duty_percent: int,
    ) -> None:
        if duration_ms <= 0 or hz_start <= 0:
            return
        if hz_end <= 0:
            hz_end = hz_start

        count = max(1, (SAMPLE_RATE * duration_ms) // 1000)
        amplitude = clamp((amplitude * 17) // 10, 200, 30000)
        duty_percent = clamp(duty_percent, 10, 90)
        duty_fp = (duty_percent * 65535) // 100
        attack_samples = clamp(count // 8, 1, 40)
        release_samples = clamp(count // 3, 1, count)
        hz_fp = hz_start << 16
        hz_step_fp = (((hz_end - hz_start) << 16) // (count - 1)) if count > 1 else 0
        phase_fp = 0

        for i in range(count):
            env = 256
            if i < attack_samples:
                env = (i * 256) // attack_samples
            if i > count - release_samples:
                tail = ((count - i) * 256) // release_samples
                if tail < env:
                    env = tail

            hz_now = clamp(hz_fp >> 16, 40, 6000)
            phase_inc_fp = ((hz_now * 65536) // SAMPLE_RATE) or 1
            phase_fp = (phase_fp + phase_inc_fp) & 0xFFFF
            sample = amplitude if phase_fp < duty_fp else -amplitude
            sample = (sample * env) // 256
            self.samples.append(s16_to_s8(sample))
            hz_fp += hz_step_fp

    def tone(self, hz: int, duration_ms: int, amplitude: int) -> None:
        self.tone_sweep(hz, hz, duration_ms, amplitude, 50)

    def noise_burst(self, duration_ms: int, amplitude: int) -> None:
        if duration_ms <= 0:
            return

        count = max(1, (SAMPLE_RATE * duration_ms) // 1000)
        amplitude = clamp((amplitude * 17) // 10, 600, 30000)
        release_samples = clamp(count, 1, count)
        prev = 0

        for i in range(count):
            env = ((count - i) * 256) // release_samples
            self.noise_state = (self.noise_state * 1664525 + 1013904223) & 0xFFFFFFFF
            sample = ((self.noise_state >> 16) & 0x7FFF) - 16384
            sample = (sample * amplitude) // 16384
            sample = (sample + (prev * 3)) // 4
            sample = (sample * env) // 256
            prev = sample
            self.samples.append(s16_to_s8(sample))

    def pattern(self, steps: Sequence[Tuple[int, int, int, int]]) -> None:
        for hz, duration_ms, amplitude, rest_ms in steps:
            if hz > 0 and duration_ms > 0:
                self.tone(hz, duration_ms, amplitude)
            elif duration_ms > 0:
                self.silence(duration_ms)
            if rest_ms > 0:
                self.silence(rest_ms)

    def finalize(self) -> bytes:
        if not self.samples:
            self.samples.append(0)

        self.samples.extend([0] * (TICK_SAMPLES * GUARD_TAIL_TICKS))
        while len(self.samples) % TICK_SAMPLES:
            self.samples.append(0)
        if len(self.samples) & 1:
            self.samples.append(0)

        out = bytearray()
        for sample in self.samples:
            out.append(sample & 0xFF)
        return bytes(out)


BuilderFn = Callable[[SampleBuilder], None]


@dataclass(frozen=True)
class SampleDef:
    enum_name: str
    builder: BuilderFn


def slot_chime(b: SampleBuilder) -> None:
    b.tone_sweep(620, 760, 30, 13600, 36)
    b.silence(6)
    b.tone_sweep(820, 1020, 34, 14600, 34)
    b.silence(6)
    b.tone_sweep(1080, 1360, 38, 15600, 31)
    b.silence(8)
    b.tone_sweep(1420, 1880, 58, 16800, 28)


def reward_jingle(b: SampleBuilder) -> None:
    b.tone_sweep(700, 860, 34, 12800, 32)
    b.silence(6)
    b.tone_sweep(920, 1120, 36, 13600, 30)
    b.silence(6)
    b.tone_sweep(1160, 1460, 40, 14600, 28)
    b.silence(8)
    b.tone_sweep(1540, 1980, 74, 16600, 24)


def level_up_fanfare(b: SampleBuilder) -> None:
    b.pattern(
        (
            (587, 52, 8600, 5),
            (740, 56, 9000, 5),
            (880, 60, 9500, 6),
            (988, 64, 10000, 6),
            (1175, 74, 10600, 8),
            (1319, 84, 11400, 10),
        )
    )
    b.tone_sweep(1319, 1760, 86, 13200, 18)
    b.silence(10)
    b.tone_sweep(1175, 1568, 84, 12600, 20)
    b.silence(12)
    b.tone_sweep(1568, 1976, 96, 13800, 22)


def death_jingle(b: SampleBuilder) -> None:
    b.noise_burst(44, 11000)
    b.tone_sweep(360, 220, 74, 13800, 48)
    b.silence(8)
    b.tone_sweep(220, 110, 118, 14600, 54)


def game_over_jingle(b: SampleBuilder) -> None:
    b.noise_burst(56, 11400)
    b.tone_sweep(300, 230, 76, 13400, 50)
    b.silence(6)
    b.tone_sweep(230, 160, 92, 13800, 52)
    b.silence(8)
    b.tone_sweep(160, 90, 140, 14600, 56)


def life_loss_fanfare(b: SampleBuilder) -> None:
    b.pattern(
        (
            (392, 66, 9000, 10),
            (330, 74, 9400, 10),
            (262, 84, 9900, 14),
            (196, 108, 10600, 0),
        )
    )
    b.silence(8)
    b.tone_sweep(196, 146, 82, 11000, 36)


def combo_jingle(b: SampleBuilder) -> None:
    b.pattern(((620, 28, 8200, 4), (860, 32, 9000, 5), (1180, 42, 9800, 0)))
    b.tone_sweep(1320, 1620, 38, 10400, 22)


def block_push_swish(b: SampleBuilder) -> None:
    b.noise_burst(18, 8200)
    b.tone_sweep(980, 520, 54, 15000, 28)
    b.tone_sweep(620, 320, 42, 12600, 34)


def title_start_jingle(b: SampleBuilder) -> None:
    b.pattern(
        (
            (1047, 54, 6200, 4),
            (1319, 56, 6700, 4),
            (1568, 62, 7100, 8),
            (2093, 92, 7800, 12),
            (1760, 52, 6600, 4),
            (2093, 68, 7600, 8),
            (2637, 118, 8400, 0),
        )
    )


def title_confirm_chirp(b: SampleBuilder) -> None:
    b.pattern(((1568, 24, 5600, 4), (2093, 34, 6400, 0)))


def new_high_score_fanfare(b: SampleBuilder) -> None:
    b.pattern(
        (
            (784, 60, 7600, 5),
            (988, 62, 8200, 5),
            (1175, 66, 8600, 6),
            (1568, 92, 9200, 8),
            (1319, 58, 8200, 5),
            (1568, 62, 8800, 6),
            (1976, 132, 9800, 0),
        )
    )


def crush(b: SampleBuilder) -> None:
    b.noise_burst(24, 7000)
    b.tone_sweep(300, 230, 36, 13200, 56)
    b.tone_sweep(420, 560, 28, 11200, 30)


def timer_danger(b: SampleBuilder) -> None:
    b.tone_sweep(980, 900, 34, 9000, 44)
    b.silence(8)
    b.tone_sweep(880, 760, 42, 8600, 44)


def round_start(b: SampleBuilder) -> None:
    b.tone_sweep(420, 560, 34, 10200, 34)
    b.silence(8)
    b.tone_sweep(620, 860, 46, 11200, 30)


def mine_placed(b: SampleBuilder) -> None:
    b.tone_sweep(520, 720, 32, 9800, 30)


def mine_exploded(b: SampleBuilder) -> None:
    b.noise_burst(56, 14400)
    b.tone_sweep(280, 150, 96, 15600, 58)
    b.tone_sweep(190, 90, 126, 14800, 62)


def block_impact(b: SampleBuilder) -> None:
    b.noise_burst(18, 4600)
    b.tone_sweep(240, 170, 26, 8600, 56)


def meta_spent(b: SampleBuilder) -> None:
    b.tone_sweep(760, 920, 20, 9800, 30)
    b.silence(4)
    b.tone_sweep(940, 1220, 30, 10800, 26)


def stage_modifier(b: SampleBuilder) -> None:
    b.tone_sweep(540, 500, 22, 8600, 40)
    b.silence(5)
    b.tone_sweep(660, 620, 24, 9000, 40)
    b.silence(6)
    b.tone_sweep(760, 900, 32, 9600, 36)


SAMPLES: Sequence[SampleDef] = (
    SampleDef("SLOT_CHIME", slot_chime),
    SampleDef("REWARD_JINGLE", reward_jingle),
    SampleDef("LEVEL_UP_FANFARE", level_up_fanfare),
    SampleDef("DEATH_JINGLE", death_jingle),
    SampleDef("GAME_OVER_JINGLE", game_over_jingle),
    SampleDef("LIFE_LOSS_FANFARE", life_loss_fanfare),
    SampleDef("COMBO_JINGLE", combo_jingle),
    SampleDef("BLOCK_PUSH_SWISH", block_push_swish),
    SampleDef("TITLE_START_JINGLE", title_start_jingle),
    SampleDef("TITLE_CONFIRM_CHIRP", title_confirm_chirp),
    SampleDef("NEW_HIGH_SCORE_FANFARE", new_high_score_fanfare),
    SampleDef("CRUSH", crush),
    SampleDef("TIMER_DANGER", timer_danger),
    SampleDef("ROUND_START", round_start),
    SampleDef("MINE_PLACED", mine_placed),
    SampleDef("MINE_EXPLODED", mine_exploded),
    SampleDef("BLOCK_IMPACT", block_impact),
    SampleDef("META_SPENT", meta_spent),
    SampleDef("STAGE_MODIFIER", stage_modifier),
)


def build_samples() -> Tuple[bytes, List[int], List[int], List[int]]:
    data = bytearray()
    offsets = [0]
    lengths_words = [0]
    ticks = [0]

    for sample_def in SAMPLES:
        builder = SampleBuilder()
        sample_def.builder(builder)
        sample = builder.finalize()
        if len(data) & 1:
            data.append(0)
        offsets.append(len(data))
        lengths_words.append(len(sample) // 2)
        ticks.append(max(1, len(sample) // TICK_SAMPLES))
        data.extend(sample)

    return bytes(data), offsets, lengths_words, ticks


def format_array(values: Sequence[int], per_line: int = 16, hex_bytes: bool = False) -> str:
    lines: List[str] = []
    for i in range(0, len(values), per_line):
        chunk = values[i : i + per_line]
        if hex_bytes:
            body = ", ".join(f"0x{value:02X}" for value in chunk)
        else:
            body = ", ".join(str(value) for value in chunk)
        lines.append(f"    {body}")
    return ",\n".join(lines)


def write_header(path: Path, data_bytes: int) -> None:
    names = ["NONE"] + [sample.enum_name for sample in SAMPLES]
    enum_lines = [f"    AMIGA_SFX_SAMPLE_{name} = {i}" for i, name in enumerate(names)]
    enum_body = ",\n".join(enum_lines)
    period = PAL_CLOCK // SAMPLE_RATE
    path.write_text(
        f"""#ifndef ICEPANIC_AMIGA_SFX_ASSETS_H
#define ICEPANIC_AMIGA_SFX_ASSETS_H

#include <exec/types.h>

#define AMIGA_SFX_SAMPLE_RATE {SAMPLE_RATE}
#define AMIGA_SFX_SAMPLE_PERIOD {period}
#define AMIGA_SFX_DATA_BYTES {data_bytes}UL

typedef enum AmigaSfxSampleId {{
{enum_body},
    AMIGA_SFX_SAMPLE_COUNT
}} AmigaSfxSampleId;

extern const UBYTE g_amiga_sfx_data[AMIGA_SFX_DATA_BYTES];
extern const ULONG g_amiga_sfx_offsets[AMIGA_SFX_SAMPLE_COUNT];
extern const UWORD g_amiga_sfx_lengths_words[AMIGA_SFX_SAMPLE_COUNT];
extern const UBYTE g_amiga_sfx_ticks[AMIGA_SFX_SAMPLE_COUNT];

#endif
""",
        encoding="utf-8",
    )


def write_source(path: Path, data: bytes, offsets: Sequence[int], lengths: Sequence[int], ticks: Sequence[int]) -> None:
    path.write_text(
        f"""#include "amiga_sfx_assets.h"

const UBYTE g_amiga_sfx_data[AMIGA_SFX_DATA_BYTES] = {{
{format_array(list(data), per_line=16, hex_bytes=True)}
}};

const ULONG g_amiga_sfx_offsets[AMIGA_SFX_SAMPLE_COUNT] = {{
{format_array(offsets, per_line=8)}
}};

const UWORD g_amiga_sfx_lengths_words[AMIGA_SFX_SAMPLE_COUNT] = {{
{format_array(lengths, per_line=8)}
}};

const UBYTE g_amiga_sfx_ticks[AMIGA_SFX_SAMPLE_COUNT] = {{
{format_array(ticks, per_line=16)}
}};
""",
        encoding="utf-8",
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--out-c", type=Path, default=Path("src/platform_amiga/amiga_sfx_assets.c"))
    parser.add_argument("--out-h", type=Path, default=Path("src/platform_amiga/amiga_sfx_assets.h"))
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    data, offsets, lengths, ticks = build_samples()
    args.out_c.parent.mkdir(parents=True, exist_ok=True)
    args.out_h.parent.mkdir(parents=True, exist_ok=True)
    write_header(args.out_h, len(data))
    write_source(args.out_c, data, offsets, lengths, ticks)
    print(
        f"wrote {args.out_c} and {args.out_h}: "
        f"{len(SAMPLES)} samples, {len(data)} bytes, {SAMPLE_RATE} Hz"
    )


if __name__ == "__main__":
    main()
