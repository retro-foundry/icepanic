#!/usr/bin/env python3
"""
Build the runtime Icepanic tileset from handcrafted pixel art.

This script intentionally does not sample `concept.png`.
It generates all required 16x16 sprites directly so art ownership and
Amiga-style constraints remain clean and deterministic.

Output format (little-endian):
  magic[4]      = b"ITLS"
  version u16   = 1
  tile_count u16
  tile_w u16    = 16
  tile_h u16    = 16
  palette u16   = 32
  palette bytes = 32 * (r,g,b)
  tile bytes    = tile_count * 16 * 16 (palette indices)

Also emits optional Amiga-oriented tile strip data:
  - 5 bitplanes
  - 16x16 tiles
  - plane-major blocks per tile
"""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Iterable, List, Sequence, Tuple

from PIL import Image

TILE_SIZE = 16

PALETTE_32: List[Tuple[int, int, int]] = [
    (0, 0, 0),
    (10, 16, 28),
    (15, 30, 58),
    (25, 47, 84),
    (39, 71, 114),
    (58, 96, 146),
    (83, 129, 188),
    (120, 170, 224),
    (182, 218, 245),
    (238, 248, 255),
    (28, 41, 70),
    (72, 92, 152),
    (255, 208, 90),
    (255, 239, 169),
    (24, 92, 39),
    (40, 144, 63),
    (111, 214, 113),
    (114, 24, 29),
    (182, 36, 48),
    (241, 92, 99),
    (34, 121, 194),
    (87, 178, 244),
    (201, 187, 149),
    (159, 126, 87),
    (47, 77, 198),
    (123, 170, 255),
    (205, 54, 76),
    (78, 159, 79),
    (109, 70, 180),
    (168, 124, 235),
    (190, 200, 210),
    (255, 255, 255),
]

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
]

SPRITE_NOTES = {
    "SPR_FLOOR": "Dark ice-cave floor tile with speckles.",
    "SPR_WALL": "Solid wall tile for interior blockers.",
    "SPR_BORDER": "Bright perimeter wall tile.",
    "SPR_BLOCK_ICE": "Pushable ice block tile.",
    "SPR_BLOCK_SPECIAL": "Deprecated visual slot (mirrors SPR_BLOCK_ICE).",
    "SPR_BLOCK_CRACKED": "Deprecated visual slot (mirrors SPR_BLOCK_ICE).",
    "SPR_BLOCK_UNBREAKABLE": "Deprecated visual slot (mirrors SPR_BLOCK_ICE).",
    "SPR_PLAYER_UP": "Penguin player facing up.",
    "SPR_PLAYER_UP_ALT": "Penguin player facing up (step frame).",
    "SPR_PLAYER_DOWN": "Penguin player facing down.",
    "SPR_PLAYER_DOWN_ALT": "Penguin player facing down (step frame).",
    "SPR_PLAYER_LEFT": "Penguin player facing left.",
    "SPR_PLAYER_LEFT_ALT": "Penguin player facing left (step frame).",
    "SPR_PLAYER_RIGHT": "Penguin player facing right.",
    "SPR_PLAYER_RIGHT_ALT": "Penguin player facing right (step frame).",
    "SPR_ENEMY_A": "Chaser enemy sprite.",
    "SPR_ENEMY_A_ALT": "Chaser enemy sprite (bobbing frame).",
    "SPR_ENEMY_B": "Wanderer enemy sprite.",
    "SPR_ENEMY_B_ALT": "Wanderer enemy sprite (bobbing frame).",
    "SPR_ENEMY_A_DEATH_0": "Chaser death animation frame 1.",
    "SPR_ENEMY_A_DEATH_1": "Chaser death animation frame 2.",
    "SPR_ENEMY_B_DEATH_0": "Wanderer death animation frame 1.",
    "SPR_ENEMY_B_DEATH_1": "Wanderer death animation frame 2.",
    "SPR_ITEM_FISH": "Fish bonus item.",
    "SPR_ITEM_EGG": "Egg bonus item.",
    "SPR_ITEM_GEM": "Gem bonus item.",
    "SPR_ITEM_FRUIT": "Fruit bonus item.",
    "SPR_ITEM_MYSTERY": "Mystery bonus item.",
    "SPR_HUD_LIFE": "Life icon sprite.",
    "SPR_EFFECT_SPARK": "Spark impact effect.",
    "SPR_MINE_0": "Bomb sprite (steady fuse frame).",
    "SPR_MINE_1": "Bomb sprite (warning fuse frame).",
    "SPR_MINE_2": "Bomb sprite (critical fuse frame).",
}


def new_tile(fill: int) -> List[List[int]]:
    return [[fill for _ in range(TILE_SIZE)] for _ in range(TILE_SIZE)]


def set_px(tile: List[List[int]], x: int, y: int, color: int) -> None:
    if 0 <= x < TILE_SIZE and 0 <= y < TILE_SIZE:
        tile[y][x] = color


def rect(tile: List[List[int]], x: int, y: int, w: int, h: int, color: int) -> None:
    for yy in range(y, y + h):
        for xx in range(x, x + w):
            set_px(tile, xx, yy, color)


def hline(tile: List[List[int]], x0: int, x1: int, y: int, color: int) -> None:
    if x0 > x1:
        x0, x1 = x1, x0
    for x in range(x0, x1 + 1):
        set_px(tile, x, y, color)


def vline(tile: List[List[int]], x: int, y0: int, y1: int, color: int) -> None:
    if y0 > y1:
        y0, y1 = y1, y0
    for y in range(y0, y1 + 1):
        set_px(tile, x, y, color)


def line(tile: List[List[int]], x0: int, y0: int, x1: int, y1: int, color: int) -> None:
    dx = abs(x1 - x0)
    sx = 1 if x0 < x1 else -1
    dy = -abs(y1 - y0)
    sy = 1 if y0 < y1 else -1
    err = dx + dy

    x = x0
    y = y0
    while True:
        set_px(tile, x, y, color)
        if x == x1 and y == y1:
            break
        e2 = 2 * err
        if e2 >= dy:
            err += dy
            x += sx
        if e2 <= dx:
            err += dx
            y += sy


def fill_ellipse(tile: List[List[int]], cx: int, cy: int, rx: int, ry: int, color: int) -> None:
    rx2 = rx * rx
    ry2 = ry * ry
    rr = rx2 * ry2
    for y in range(cy - ry, cy + ry + 1):
        for x in range(cx - rx, cx + rx + 1):
            dx = x - cx
            dy = y - cy
            if dx * dx * ry2 + dy * dy * rx2 <= rr:
                set_px(tile, x, y, color)


def outline_ellipse(tile: List[List[int]], cx: int, cy: int, rx: int, ry: int, color: int) -> None:
    rx2 = rx * rx
    ry2 = ry * ry
    rr = rx2 * ry2
    for y in range(cy - ry - 1, cy + ry + 2):
        for x in range(cx - rx - 1, cx + rx + 2):
            dx = x - cx
            dy = y - cy
            inside = dx * dx * ry2 + dy * dy * rx2 <= rr
            if not inside:
                continue
            edge = False
            for ox, oy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                ndx = x + ox - cx
                ndy = y + oy - cy
                n_inside = ndx * ndx * ry2 + ndy * ndy * rx2 <= rr
                if not n_inside:
                    edge = True
                    break
            if edge:
                set_px(tile, x, y, color)


def add_floor_stars(tile: List[List[int]]) -> None:
    for y in range(TILE_SIZE):
        for x in range(TILE_SIZE):
            r = (x * 17 + y * 29 + x * y * 3) & 255
            if r == 0:
                set_px(tile, x, y, 4)
            elif r == 1:
                set_px(tile, x, y, 5)


def draw_floor() -> List[List[int]]:
    t = new_tile(2)

    # Sparse sparkle-only floor for clear gameplay readability.
    add_floor_stars(t)
    for x, y, c in ((3, 5, 5), (11, 4, 5), (7, 10, 4)):
        set_px(t, x, y, c)
    return t


def draw_wall() -> List[List[int]]:
    t = new_tile(4)

    for y in range(0, TILE_SIZE, 4):
        hline(t, 0, TILE_SIZE - 1, y, 2)
    for x in range(0, TILE_SIZE, 4):
        vline(t, x, 0, TILE_SIZE - 1, 2)

    for y in range(TILE_SIZE):
        for x in range(TILE_SIZE):
            if ((x * 5 + y * 3) & 7) == 0:
                set_px(t, x, y, 5)

    line(t, 3, 2, 12, 11, 7)
    line(t, 6, 3, 5, 13, 6)
    line(t, 10, 1, 14, 5, 6)

    # Tiny white/cyan glints so it stays icy.
    set_px(t, 2, 2, 31)
    set_px(t, 3, 2, 9)
    set_px(t, 2, 3, 9)
    set_px(t, 10, 6, 31)
    set_px(t, 9, 6, 9)
    set_px(t, 9, 7, 9)
    set_px(t, 6, 10, 31)
    set_px(t, 5, 10, 9)
    set_px(t, 5, 11, 9)
    for sx in (1, 5, 9, 13):
        set_px(t, sx, 1, 9)
    for sy in (1, 5, 9, 13):
        set_px(t, 1, sy, 9)
    return t


def draw_border() -> List[List[int]]:
    t = new_tile(6)
    for y in range(TILE_SIZE):
        for x in range(TILE_SIZE):
            if ((x + y) & 3) == 0:
                set_px(t, x, y, 7)
    hline(t, 0, TILE_SIZE - 1, 0, 9)
    hline(t, 0, TILE_SIZE - 1, 1, 8)
    hline(t, 0, TILE_SIZE - 1, TILE_SIZE - 1, 4)
    hline(t, 0, TILE_SIZE - 1, TILE_SIZE - 2, 5)
    vline(t, 0, 0, TILE_SIZE - 1, 9)
    vline(t, 1, 0, TILE_SIZE - 1, 8)
    vline(t, TILE_SIZE - 1, 0, TILE_SIZE - 1, 4)
    vline(t, TILE_SIZE - 2, 0, TILE_SIZE - 1, 5)
    for x, y in ((3, 4), (11, 5), (6, 10), (13, 12)):
        set_px(t, x, y, 31)
    return t


def draw_ice_base(base: int = 6, bright: int = 8, mid: int = 7, shadow: int = 4) -> List[List[int]]:
    t = new_tile(base)
    rect(t, 1, 1, 14, 14, bright)
    rect(t, 2, 2, 12, 12, mid)
    for y in range(1, 15):
        for x in range(1, 15):
            if ((x + y) & 1) == 0:
                set_px(t, x, y, 9)
            if ((x * 3 + y * 5) & 7) == 0:
                set_px(t, x, y, bright)
    hline(t, 0, 15, 0, 31)
    hline(t, 1, 14, 1, 9)
    vline(t, 0, 0, 15, 31)
    vline(t, 1, 1, 14, 9)
    hline(t, 0, 15, 15, 1)
    hline(t, 1, 14, 14, shadow)
    vline(t, 15, 0, 15, 1)
    vline(t, 14, 1, 14, shadow)
    set_px(t, 2, 2, 31)
    set_px(t, 3, 3, 30)
    set_px(t, 12, 12, 3)
    set_px(t, 13, 13, 1)
    return t


def draw_block_ice() -> List[List[int]]:
    t = draw_ice_base()
    line(t, 3, 6, 8, 3, 9)
    line(t, 7, 11, 12, 8, 8)
    return t


def draw_block_special() -> List[List[int]]:
    t = draw_ice_base(base=6, bright=8, mid=7, shadow=4)
    for yy in range(5):
        span = 2 + yy
        hline(t, 8 - span, 8 + span, 4 + yy, 24)
        hline(t, 8 - span, 8 + span, 12 - yy, 24)
    for yy in range(3):
        span = 4 + yy
        hline(t, 8 - span, 8 + span, 6 + yy, 25)
        hline(t, 8 - span, 8 + span, 10 - yy, 25)
    outline_ellipse(t, 8, 8, 4, 4, 31)
    return t


def draw_block_cracked() -> List[List[int]]:
    t = draw_ice_base(base=6, bright=8, mid=7, shadow=4)
    line(t, 3, 3, 12, 12, 3)
    line(t, 4, 10, 9, 6, 3)
    line(t, 9, 12, 12, 8, 2)
    line(t, 5, 4, 2, 7, 2)
    return t


def draw_block_unbreakable() -> List[List[int]]:
    t = draw_ice_base(base=5, bright=7, mid=6, shadow=3)
    # Keep it icy: subtle darker core + tiny glints.
    rect(t, 5, 5, 6, 6, 5)
    for x, y, c in (
        (3, 3, 31), (4, 3, 31), (3, 4, 31),
        (10, 10, 9), (11, 10, 9), (10, 11, 9),
    ):
        set_px(t, x, y, c)
    set_px(t, 8, 8, 9)
    return t


def draw_penguin(direction: str, pose: int = 0) -> List[List[int]]:
    t = new_tile(0)

    fill_ellipse(t, 8, 9, 5, 5, 10)
    fill_ellipse(t, 8, 9, 4, 4, 11)
    fill_ellipse(t, 8, 10, 2, 3, 31)

    fill_ellipse(t, 8, 5, 4, 3, 10)
    fill_ellipse(t, 8, 5, 3, 2, 11)

    if pose == 0:
        rect(t, 5, 13, 3, 2, 12)
        rect(t, 9, 13, 3, 2, 12)
    else:
        rect(t, 4, 13, 3, 2, 12)
        rect(t, 10, 13, 3, 2, 12)
        set_px(t, 8, 13, 31)

    if direction == "up":
        set_px(t, 6, 4, 31)
        set_px(t, 9, 4, 31)
        set_px(t, 6, 5, 2)
        set_px(t, 9, 5, 2)
        rect(t, 7, 6, 2, 1, 12)
        if pose != 0:
            rect(t, 3, 8, 2, 3, 10)
            rect(t, 11, 8, 2, 3, 10)
    elif direction == "down":
        set_px(t, 6, 5, 31)
        set_px(t, 9, 5, 31)
        set_px(t, 6, 6, 2)
        set_px(t, 9, 6, 2)
        rect(t, 7, 7, 2, 2, 12)
        if pose != 0:
            rect(t, 3, 7, 2, 3, 10)
            rect(t, 11, 7, 2, 3, 10)
    elif direction == "left":
        set_px(t, 6, 5, 31)
        set_px(t, 6, 6, 2)
        set_px(t, 9, 5, 9)
        rect(t, 4, 6, 2, 2, 12)
        rect(t, 11, 8, 2, 3, 10)
        if pose != 0:
            rect(t, 11, 8, 2, 3, 0)
            rect(t, 10, 7, 2, 3, 10)
    else:
        set_px(t, 9, 5, 31)
        set_px(t, 9, 6, 2)
        set_px(t, 6, 5, 9)
        rect(t, 10, 6, 2, 2, 12)
        rect(t, 3, 8, 2, 3, 10)
        if pose != 0:
            rect(t, 3, 8, 2, 3, 0)
            rect(t, 4, 7, 2, 3, 10)

    return t


def draw_blob_enemy(kind: str, pose: int = 0) -> List[List[int]]:
    t = new_tile(0)
    if kind == "chaser":
        dark, mid, light = 17, 18, 19
    else:
        dark, mid, light = 14, 15, 16

    yoff = 0
    fill_ellipse(t, 8, 9 + yoff, 5, 4, dark)
    fill_ellipse(t, 8, 8 + yoff, 4, 4, mid)
    fill_ellipse(t, 8, 7 + yoff, 3, 2, light)

    if pose == 0:
        rect(t, 4, 11, 2, 2, dark)
        rect(t, 10, 11, 2, 2, dark)
    else:
        rect(t, 3, 11, 2, 2, dark)
        rect(t, 11, 11, 2, 2, dark)
        set_px(t, 8, 6, light)

    set_px(t, 6, 8 + yoff, 31)
    set_px(t, 9, 8 + yoff, 31)
    if kind == "chaser":
        set_px(t, 6, 9 + yoff, 2)
        set_px(t, 9, 9 + yoff, 2)
        rect(t, 7, 10 + yoff, 2, 1, 31)
    else:
        set_px(t, 6, 9 + yoff, 3)
        set_px(t, 9, 9 + yoff, 3)
        rect(t, 7, 10 + yoff, 2, 1, 9)
    return t


def draw_blob_enemy_death(kind: str, frame: int = 0) -> List[List[int]]:
    t = new_tile(0)
    if kind == "chaser":
        dark, mid, light = 17, 18, 19
        eye = 2
    else:
        dark, mid, light = 14, 15, 16
        eye = 3

    if frame == 0:
        fill_ellipse(t, 8, 10, 5, 3, dark)
        fill_ellipse(t, 8, 9, 4, 2, mid)
        fill_ellipse(t, 8, 8, 3, 2, light)
        line(t, 5, 9, 7, 11, 31)
        line(t, 7, 9, 5, 11, 31)
        line(t, 9, 9, 11, 11, 31)
        line(t, 11, 9, 9, 11, 31)
        set_px(t, 5, 8, eye)
        set_px(t, 10, 8, eye)
    else:
        fill_ellipse(t, 8, 11, 4, 2, dark)
        fill_ellipse(t, 8, 10, 3, 1, mid)
        set_px(t, 4, 10, light)
        set_px(t, 5, 9, light)
        set_px(t, 10, 9, light)
        set_px(t, 11, 10, light)
        set_px(t, 4, 11, 31)
        set_px(t, 11, 11, 31)
        set_px(t, 8, 9, 30)
        set_px(t, 7, 10, 30)
        set_px(t, 9, 10, 30)
    return t


def draw_item_fish() -> List[List[int]]:
    t = new_tile(0)
    fill_ellipse(t, 7, 8, 4, 3, 20)
    fill_ellipse(t, 6, 8, 3, 2, 21)
    line(t, 10, 8, 13, 6, 20)
    line(t, 10, 8, 13, 10, 20)
    set_px(t, 13, 8, 12)
    set_px(t, 5, 8, 31)
    set_px(t, 5, 9, 2)
    set_px(t, 8, 6, 12)
    return t


def draw_item_egg() -> List[List[int]]:
    t = new_tile(0)
    fill_ellipse(t, 8, 8, 3, 5, 22)
    fill_ellipse(t, 7, 8, 2, 4, 13)
    line(t, 10, 6, 10, 11, 23)
    set_px(t, 6, 5, 31)
    return t


def draw_item_gem() -> List[List[int]]:
    t = new_tile(0)
    for i in range(5):
        hline(t, 8 - i, 8 + i, 3 + i, 24)
        hline(t, 8 - i, 8 + i, 12 - i, 24)
    for i in range(3):
        hline(t, 7 - i, 9 + i, 5 + i, 25)
        hline(t, 7 - i, 9 + i, 10 - i, 25)
    outline_ellipse(t, 8, 8, 4, 4, 31)
    return t


def draw_item_fruit() -> List[List[int]]:
    t = new_tile(0)
    fill_ellipse(t, 8, 9, 4, 4, 26)
    fill_ellipse(t, 7, 8, 2, 2, 19)
    rect(t, 7, 3, 1, 3, 23)
    rect(t, 8, 3, 3, 2, 27)
    set_px(t, 6, 6, 31)
    return t


def draw_item_mystery() -> List[List[int]]:
    t = new_tile(0)
    fill_ellipse(t, 8, 8, 5, 5, 28)
    fill_ellipse(t, 8, 8, 4, 4, 29)
    set_px(t, 7, 5, 31)
    set_px(t, 8, 5, 31)
    set_px(t, 9, 5, 31)
    set_px(t, 9, 6, 31)
    set_px(t, 8, 7, 31)
    set_px(t, 8, 9, 31)
    set_px(t, 8, 11, 31)
    return t


def draw_hud_life() -> List[List[int]]:
    t = new_tile(0)
    fill_ellipse(t, 8, 8, 5, 5, 10)
    fill_ellipse(t, 8, 8, 4, 4, 11)
    fill_ellipse(t, 8, 9, 2, 2, 31)
    set_px(t, 6, 7, 31)
    set_px(t, 9, 7, 31)
    set_px(t, 6, 8, 2)
    set_px(t, 9, 8, 2)
    rect(t, 7, 10, 2, 1, 12)
    return t


def draw_effect_spark() -> List[List[int]]:
    t = new_tile(0)
    vline(t, 8, 2, 13, 31)
    hline(t, 2, 13, 8, 31)
    line(t, 4, 4, 12, 12, 29)
    line(t, 12, 4, 4, 12, 29)
    vline(t, 8, 4, 11, 21)
    hline(t, 4, 11, 8, 21)
    return t


def draw_mine(frame: int = 0) -> List[List[int]]:
    t = new_tile(0)
    shell = 3
    shell_hi = 29
    core = 30
    fuse = 23
    spark = 12

    if frame == 1:
        shell = 4
        shell_hi = 30
        core = 31
        spark = 8
    elif frame >= 2:
        shell = 18
        shell_hi = 19
        core = 30
        spark = 8

    fill_ellipse(t, 8, 8, 4, 4, shell)
    fill_ellipse(t, 8, 8, 3, 3, shell_hi)
    rect(t, 8, 3, 1, 2, fuse)
    rect(t, 9, 2, 2, 1, fuse)
    set_px(t, 10, 1, spark)
    set_px(t, 7, 6, 31)
    set_px(t, 8, 6, 31)
    set_px(t, 7, 7, 2)
    set_px(t, 8, 7, 2)

    if frame >= 1:
        set_px(t, 11, 2, spark)
    if frame >= 2:
        set_px(t, 12, 2, 30)
        set_px(t, 10, 3, 30)

    return t


def tile_to_linear_bytes(tile: Sequence[Sequence[int]]) -> List[int]:
    out: List[int] = []
    for y in range(TILE_SIZE):
        for x in range(TILE_SIZE):
            out.append(int(tile[y][x]) & 31)
    return out


def indices_to_amiga_planes(indices: Sequence[int], width: int, height: int, planes: int = 5) -> List[bytes]:
    if width % 8 != 0:
        raise ValueError("width must be divisible by 8 for planar packing")
    bytes_per_row = width // 8
    out = [bytearray(bytes_per_row * height) for _ in range(planes)]

    for y in range(height):
        row_base = y * width
        row_out = y * bytes_per_row
        for xb in range(bytes_per_row):
            vals = [0 for _ in range(planes)]
            for bit in range(8):
                x = xb * 8 + bit
                idx = int(indices[row_base + x]) & ((1 << planes) - 1)
                mask = 1 << (7 - bit)
                for plane in range(planes):
                    if (idx >> plane) & 1:
                        vals[plane] |= mask
            for plane in range(planes):
                out[plane][row_out + xb] = vals[plane]

    return [bytes(p) for p in out]


def flatten_planes_for_layout(
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


def build_tiles() -> List[List[int]]:
    tile_maps = {
        "SPR_FLOOR": draw_floor(),
        "SPR_WALL": draw_wall(),
        "SPR_BORDER": draw_border(),
        "SPR_BLOCK_ICE": draw_block_ice(),
        "SPR_BLOCK_SPECIAL": draw_block_ice(),
        "SPR_BLOCK_CRACKED": draw_block_ice(),
        "SPR_BLOCK_UNBREAKABLE": draw_block_ice(),
        "SPR_PLAYER_UP": draw_penguin("up", pose=0),
        "SPR_PLAYER_UP_ALT": draw_penguin("up", pose=1),
        "SPR_PLAYER_DOWN": draw_penguin("down", pose=0),
        "SPR_PLAYER_DOWN_ALT": draw_penguin("down", pose=1),
        "SPR_PLAYER_LEFT": draw_penguin("left", pose=0),
        "SPR_PLAYER_LEFT_ALT": draw_penguin("left", pose=1),
        "SPR_PLAYER_RIGHT": draw_penguin("right", pose=0),
        "SPR_PLAYER_RIGHT_ALT": draw_penguin("right", pose=1),
        "SPR_ENEMY_A": draw_blob_enemy("chaser", pose=0),
        "SPR_ENEMY_A_ALT": draw_blob_enemy("chaser", pose=1),
        "SPR_ENEMY_B": draw_blob_enemy("wanderer", pose=0),
        "SPR_ENEMY_B_ALT": draw_blob_enemy("wanderer", pose=1),
        "SPR_ENEMY_A_DEATH_0": draw_blob_enemy_death("chaser", frame=0),
        "SPR_ENEMY_A_DEATH_1": draw_blob_enemy_death("chaser", frame=1),
        "SPR_ENEMY_B_DEATH_0": draw_blob_enemy_death("wanderer", frame=0),
        "SPR_ENEMY_B_DEATH_1": draw_blob_enemy_death("wanderer", frame=1),
        "SPR_ITEM_FISH": draw_item_fish(),
        "SPR_ITEM_EGG": draw_item_egg(),
        "SPR_ITEM_GEM": draw_item_gem(),
        "SPR_ITEM_FRUIT": draw_item_fruit(),
        "SPR_ITEM_MYSTERY": draw_item_mystery(),
        "SPR_HUD_LIFE": draw_hud_life(),
        "SPR_EFFECT_SPARK": draw_effect_spark(),
        "SPR_MINE_0": draw_mine(frame=0),
        "SPR_MINE_1": draw_mine(frame=1),
        "SPR_MINE_2": draw_mine(frame=2),
    }
    return [tile_to_linear_bytes(tile_maps[name]) for name in SPRITE_ORDER]


def write_tileset_bin(out_path: Path, tile_indices: List[List[int]]) -> None:
    with out_path.open("wb") as f:
        f.write(b"ITLS")
        f.write((1).to_bytes(2, "little"))
        f.write((len(tile_indices)).to_bytes(2, "little"))
        f.write((TILE_SIZE).to_bytes(2, "little"))
        f.write((TILE_SIZE).to_bytes(2, "little"))
        f.write((len(PALETTE_32)).to_bytes(2, "little"))
        for r, g, b in PALETTE_32:
            f.write(bytes((r, g, b)))
        for tile in tile_indices:
            f.write(bytes(tile))


def write_manifest(out_path: Path, sprite_names: Iterable[str], atlas_columns: int) -> None:
    lines = [
        "# Icepanic handcrafted tileset mapping",
        "# sprite_index -> sprite_name -> atlas_cell(col,row) -> note",
    ]
    for i, name in enumerate(sprite_names):
        col = i % atlas_columns
        row = i // atlas_columns
        lines.append(
            f"{i:02d} -> {name} -> ({col},{row}) -> {SPRITE_NOTES.get(name, 'n/a')}")
    out_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def write_amiga_tilestrip(
    out_path: Path,
    tile_indices: List[List[int]],
    planes: int = 5,
    layout: str = "plane-major",
) -> Tuple[int, int]:
    bytes_per_tile = 0
    with out_path.open("wb") as f:
        for tile in tile_indices:
            tile_planes = indices_to_amiga_planes(tile, TILE_SIZE, TILE_SIZE, planes=planes)
            packed = flatten_planes_for_layout(tile_planes, TILE_SIZE, TILE_SIZE, layout)
            f.write(packed)
            bytes_per_tile = len(packed)
    return len(tile_indices), bytes_per_tile


def write_amiga_tilestrip_info(
    out_path: Path,
    tile_count: int,
    bytes_per_tile: int,
    planes: int = 5,
    layout: str = "plane-major",
) -> None:
    bytes_per_plane = TILE_SIZE * (TILE_SIZE // 8)
    lines = [
        "# Icepanic Amiga Tile Strip",
        f"tile_count={tile_count}",
        f"tile_size={TILE_SIZE}x{TILE_SIZE}",
        f"bitplanes={planes}",
        f"layout=tile-major,{layout}",
        f"bytes_per_plane_per_tile={bytes_per_plane}",
        f"bytes_per_tile={bytes_per_tile}",
        f"total_bytes={tile_count * bytes_per_tile}",
        "tile_order=" + ",".join(SPRITE_ORDER),
    ]
    out_path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def write_atlas_png(out_path: Path, tile_indices: List[List[int]], columns: int) -> None:
    rows = (len(tile_indices) + columns - 1) // columns
    atlas = Image.new("RGB", (columns * TILE_SIZE, rows * TILE_SIZE), (0, 0, 0))

    for i, tile in enumerate(tile_indices):
        tx = (i % columns) * TILE_SIZE
        ty = (i // columns) * TILE_SIZE
        img = Image.new("RGB", (TILE_SIZE, TILE_SIZE), (0, 0, 0))
        px = img.load()
        for y in range(TILE_SIZE):
            for x in range(TILE_SIZE):
                idx = tile[y * TILE_SIZE + x] & 31
                px[x, y] = PALETTE_32[idx]
        atlas.paste(img, (tx, ty))

    atlas.save(out_path)


def write_sprite_pngs(out_dir: Path, tile_indices: List[List[int]]) -> None:
    out_dir.mkdir(parents=True, exist_ok=True)
    for i, name in enumerate(SPRITE_ORDER):
        tile = tile_indices[i]
        img = Image.new("RGB", (TILE_SIZE, TILE_SIZE), (0, 0, 0))
        px = img.load()
        for y in range(TILE_SIZE):
            for x in range(TILE_SIZE):
                idx = tile[y * TILE_SIZE + x] & 31
                px[x, y] = PALETTE_32[idx]
        img.save(out_dir / f"{name}.png")


def main() -> None:
    parser = argparse.ArgumentParser(description="Build handcrafted runtime tileset.")
    parser.add_argument("--output", default="assets/tileset.bin", help="Output tileset binary.")
    parser.add_argument("--manifest", default="assets/tileset_manifest.txt", help="Output manifest path.")
    parser.add_argument("--atlas", default="assets/sprite_atlas_16.png", help="Single atlas PNG output path.")
    parser.add_argument(
        "--amiga-tilestrip",
        default="assets/tile_strip_16x16_5bpl.bin",
        help="Output Amiga-style 5-bitplane tile strip binary.")
    parser.add_argument(
        "--amiga-tilestrip-info",
        default="assets/tile_strip_16x16_5bpl.txt",
        help="Output metadata for the Amiga-style tile strip.")
    parser.add_argument(
        "--tile-strip-layout",
        choices=("plane-major", "row-interleaved"),
        default="plane-major",
        help="Per-tile planar byte layout in the exported tile strip.")
    parser.add_argument("--atlas-columns", type=int, default=8, help="Number of columns in atlas PNG.")
    args = parser.parse_args()

    output_path = Path(args.output)
    manifest_path = Path(args.manifest)
    atlas_path = Path(args.atlas)
    amiga_tilestrip_path = Path(args.amiga_tilestrip)
    amiga_tilestrip_info_path = Path(args.amiga_tilestrip_info)
    atlas_columns = max(1, int(args.atlas_columns))
    tile_strip_layout = args.tile_strip_layout

    output_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    atlas_path.parent.mkdir(parents=True, exist_ok=True)
    amiga_tilestrip_path.parent.mkdir(parents=True, exist_ok=True)
    amiga_tilestrip_info_path.parent.mkdir(parents=True, exist_ok=True)
    tiles = build_tiles()
    write_tileset_bin(output_path, tiles)
    write_manifest(manifest_path, SPRITE_ORDER, atlas_columns)
    write_atlas_png(atlas_path, tiles, atlas_columns)
    tile_count, bytes_per_tile = write_amiga_tilestrip(
        amiga_tilestrip_path,
        tiles,
        planes=5,
        layout=tile_strip_layout)
    write_amiga_tilestrip_info(
        amiga_tilestrip_info_path,
        tile_count,
        bytes_per_tile,
        planes=5,
        layout=tile_strip_layout)
    print(f"Wrote {output_path} with {len(tiles)} tiles.")
    print(f"Wrote {manifest_path}.")
    print(f"Wrote {atlas_path} (single consolidated atlas).")
    print(f"Wrote {amiga_tilestrip_path} (5-bitplane tile strip, {tile_strip_layout}).")
    print(f"Wrote {amiga_tilestrip_info_path}.")


if __name__ == "__main__":
    main()
