#include "game.h"

#ifndef ICEPANIC_AMIGA_OCS_FIXED_LEVELS
#include "procgen_wfc_templates.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(ICEPANIC_AMIGA_OCS_FIXED_LEVELS) && defined(ICEPANIC_AMIGA_SMALL_STACK)
#define ICEPANIC_AMIGA_OCS_ARCADE_CORE 1
#endif

enum {
    FP_SHIFT = 8,
    FP_ONE = 1 << FP_SHIFT,
    TILE_FP = GAME_TILE_SIZE * FP_ONE,

#if defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE)
    PLAYER_SPEED_FP = 2 * FP_ONE,
#else
    PLAYER_SPEED_FP = (3 * FP_ONE) / 2,
#endif
    BLOCK_SPEED_FP = (9 * FP_ONE) / 4,
    PLAYER_DIR_BUFFER_TICKS = 14,

    PUSH_ANTICIPATION_TICKS = 4,
    ROUND_INTRO_TICKS = 50,
    RESPAWN_INTRO_TICKS = 30,
    RESPAWN_INVULN_TICKS = 75,
    PLAYER_DYING_TICKS = 45,
    ROUND_CLEAR_TICKS = 70,
    ROUND_CLEAR_PENDING_TICKS = 12,
    PERK_CHOICE_FIRE_LOCK_TICKS = GAME_FIXED_TICK_HZ / 4,
    GAME_OVER_TICKS = 130,
    DEFAULT_LIVES = 4,
    MAX_LIVES = 7,

    BONUS_ITEM_LIFETIME_TICKS = 9 * GAME_FIXED_TICK_HZ,
    BONUS_ITEM_FLASH_TICKS = 2 * GAME_FIXED_TICK_HZ,
    BONUS_ITEM_KILL_THRESHOLD = 3,
    BONUS_ITEM_KILL_THRESHOLD_MIN = 2,
    BONUS_ITEM_KILL_THRESHOLD_MAX = 6,
    MINE_KILL_SCORE_PERMILLE = 400,
    MINE_KILL_SCORE_MIN = 40,
    SPECIAL_ALIGNMENT_BONUS_SCORE = 1000,
    SPECIAL_ALIGNMENT_TIME_BONUS_TICKS = 8 * GAME_FIXED_TICK_HZ,
    SPECIAL_ALIGNMENT_FLASH_TICKS = 60,
    TIMER_DANGER_THRESHOLD_TICKS = 12 * GAME_FIXED_TICK_HZ,
    TIMER_DANGER_PULSE_TICKS = 8,
    SCORE_POPUP_TICKS = 40,
    SCORE_POPUP_RISE_FP = FP_ONE / 2,
    IMPACT_FX_TICKS = 12,
    IMPACT_FX_RISE_FP = FP_ONE / 3,
    ENEMY_DEATH_FX_TICKS = 16,

    RUN_SCORE_MULT_BASE_PERMILLE = 1000,
    RUN_SCORE_MULT_MAX_PERMILLE = 1800,
    RUN_TIME_BONUS_MAX_TICKS = 35 * GAME_FIXED_TICK_HZ,
    RUN_ENEMY_SLOW_MAX_FP = (3 * FP_ONE) / 4,
    RUN_MINE_CAPACITY_MAX = GAME_MAX_ACTIVE_MINES,
    MINE_PERK_STEP = 2,
    MINE_FUSE_TICKS = 4 * GAME_FIXED_TICK_HZ,
    MINE_EXPLOSION_TTL_TICKS = IMPACT_FX_TICKS + 4,
    PERK_SCORE_STEP_PERMILLE = 100,
    PERK_TIME_STEP_TICKS = 5 * GAME_FIXED_TICK_HZ,
    PERK_ENEMY_SLOW_STEP_FP = FP_ONE / 4,
    PERK_RECENT_OFFER_ROUNDS = 2,
    STAGE_MODIFIER_FLASH_TICKS = 50,
    STAGE_MODIFIER_RECENT_ROUNDS = 2,
    STAGE_MOD_SWARM_ENEMY_BONUS = 1,
    STAGE_MOD_SWARM_AGGRESSION_BONUS = 6,
    STAGE_MOD_SWARM_SPEED_BONUS_FP = FP_ONE / 8,
    STAGE_MOD_SHATTERED_CONVERT_BASE = 4,
    STAGE_MOD_BLACK_ICE_ADD_BASE = 2,
    STAGE_MOD_TURBO_SLIDE_SPEED_BONUS_FP = FP_ONE / 2,
    STAGE_MOD_TURBO_SLIDE_AGGRESSION_BONUS = 5,
    STAGE_MOD_TURBO_SLIDE_TIME_PENALTY_TICKS = 2 * GAME_FIXED_TICK_HZ,
    STAGE_MOD_TURBO_SLIDE_STARTUP_TICKS = 0,
    STAGE_MOD_FROST_SIREN_TIME_PENALTY_TICKS = 2 * GAME_FIXED_TICK_HZ,
    STAGE_MOD_FROST_SIREN_AGGRESSION_BONUS = 3,
    STAGE_MOD_FROST_SIREN_DANGER_THRESHOLD_BONUS_TICKS = 6 * GAME_FIXED_TICK_HZ,
    STAGE_MOD_FROST_SIREN_PULSE_INTERVAL_TICKS = (4 * GAME_FIXED_TICK_HZ) / 5,
    STAGE_MOD_FROST_SIREN_PULSE_TICKS = 8,
    META_COST_LIFE_PACK = 5,
    META_COST_TIME_PACK = 5,
    META_COST_SCORE_PACK = 6,
    META_COST_SLOW_PACK = 6,
    META_TIME_STEP_TICKS = 3 * GAME_FIXED_TICK_HZ,
    META_SCORE_STEP_PERMILLE = 50,
    META_ENEMY_SLOW_STEP_FP = FP_ONE / 8,

    PLAYER_HITBOX_LEFT_FP = 3 * FP_ONE,
    PLAYER_HITBOX_TOP_FP = 2 * FP_ONE,
    PLAYER_HITBOX_RIGHT_FP = 13 * FP_ONE,
    PLAYER_HITBOX_BOTTOM_FP = 14 * FP_ONE,
    ENEMY_HITBOX_LEFT_FP = 3 * FP_ONE,
    ENEMY_HITBOX_TOP_FP = 4 * FP_ONE,
    ENEMY_HITBOX_RIGHT_FP = 13 * FP_ONE,
    ENEMY_HITBOX_BOTTOM_FP = 13 * FP_ONE,

    SHARDS_PER_CRUSH = 1,
    SHARDS_PER_ITEM = 1,
    SHARDS_PER_MYSTERY_ITEM = 2,
    SHARDS_PER_ROUND_CLEAR = 3,

    OCS_BOB_ENEMY_SOFT_CAP = 6
};

static const uint32_t SCORE_CAP = 99999999u;

typedef struct LevelDef {
    char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1];
} LevelDef;

#ifdef ICEPANIC_AMIGA_SMALL_STACK
#define ICEPANIC_SCRATCH static
#else
#define ICEPANIC_SCRATCH
#endif

static const LevelDef kDefaultLevels[] = {
    {{
        "####################",
        "#....I..S...I......#",
        "#.PI..E.#....I.....#",
        "#..###....I.....E..#",
        "#......I....S......#",
        "#..I.....####...I..#",
        "#......E....I......#",
        "#..####......###...#",
        "#.....I............#",
        "#...E......I..S.I..#",
        "#........I.........#",
        "####################",
    }},
    {{
        "####################",
        "#...I....S..E......#",
        "#.P..###..I.....I..#",
        "#..I......I........#",
        "#......E.....###...#",
        "#..####....I..S....#",
        "#....I........I....#",
        "#..E....###.....E..#",
        "#.....I.........I..#",
        "#...####....I...S..#",
        "#......I...........#",
        "####################",
    }},
    {{
        "####################",
        "#...I..S.E....I....#",
        "#.P....###.........#",
        "#..I.........I..S..#",
        "#....E....I....E...#",
        "#..####.....####...#",
        "#....I............I#",
        "#..E....I..I.......#",
        "#......####.....I..#",
        "#...I...S......E...#",
        "#........I.........#",
        "####################",
    }},
    {{
        "####################",
        "#..I...S....I....E.#",
        "#.P..##....I.......#",
        "#.....I....###..I..#",
        "#.I.....E....S.....#",
        "#..####......I..E..#",
        "#......I..##.......#",
        "#..E.......I..###..#",
        "#....S..I..........#",
        "#..I.....####...I..#",
        "#..............I...#",
        "####################",
    }},
    {{
        "####################",
        "#....I....S.....E..#",
        "#.P.###..I....I....#",
        "#......I......###..#",
        "#..I......E........#",
        "#....####....S..I..#",
        "#..E.....I.........#",
        "#......###..I......#",
        "#.I..S......E..I...#",
        "#..........####....#",
        "#...I..............#",
        "####################",
    }},
    {{
        "####################",
        "#..S...I....E...I..#",
        "#.P....I..###......#",
        "#..###.......I..E..#",
        "#......####........#",
        "#.I..E......S..I...#",
        "#.......I..###.....#",
        "#..####.......I....#",
        "#.....S..E........I#",
        "#..I.......####....#",
        "#..........I.......#",
        "####################",
    }},
    {{
        "####################",
        "#...I..E....S...I..#",
        "#.P..##..I.........#",
        "#....##......###...#",
        "#.I......E.....I...#",
        "#..###..S..###.....#",
        "#......I......E....#",
        "#..I...####........#",
        "#.....E....I..S....#",
        "#...###.......I....#",
        "#........I.........#",
        "####################",
    }},
    {{
        "####################",
        "#.I....S....E....I.#",
        "#.P..I...####......#",
        "#....###......I....#",
        "#..E......I.....E..#",
        "#.....U..S..U......#",
        "#..I......I....###.#",
        "#......####........#",
        "#.E...I......S..I..#",
        "#....###...........#",
        "#........I.........#",
        "####################",
    }},
    {{
        "####################",
        "#...S.....I....E...#",
        "#.P....##....I.....#",
        "#..I...##..E....I..#",
        "#......####........#",
        "#.I..U......U..S...#",
        "#.......I..###.....#",
        "#..E..###......I...#",
        "#.....I....E.......#",
        "#..S......####..I..#",
        "#.................I#",
        "####################",
    }},
    {{
        "####################",
        "#..I..E..S.....I...#",
        "#.P..###.....I.....#",
        "#......I..###...E..#",
        "#..I.......U.......#",
        "#....####..S..####.#",
        "#.E......I.........#",
        "#.....###....I.....#",
        "#..S.....E....I....#",
        "#....I......###....#",
        "#........I.........#",
        "####################",
    }},
    {{
        "####################",
        "#...I....E..S...I..#",
        "#.P....I..##.......#",
        "#..###.......I..E..#",
        "#.....U..I..U......#",
        "#.I......S.....I...#",
        "#...####.....###...#",
        "#..E....I.........E#",
        "#......###..I......#",
        "#.S..I.......####..#",
        "#.................I#",
        "####################",
    }},
    {{
        "####################",
        "#.S....I....E...I..#",
        "#.P..##..I.....##..#",
        "#....##.....E......#",
        "#..I....####....I..#",
        "#.....U..S..U......#",
        "#.E.......I....E...#",
        "#...####......I....#",
        "#..I.....###.......#",
        "#......S.....I.....#",
        "#...........I......#",
        "####################",
    }},
    {{
        "####################",
        "#..I..E....S....I..#",
        "#.P....###....I....#",
        "#.I.......E..###...#",
        "#....U..I..U.......#",
        "#..###....S....###.#",
        "#......I.....E.....#",
        "#.E..####.........I#",
        "#.....I....###.....#",
        "#..S......I....E...#",
        "#........I.........#",
        "####################",
    }},
    {{
        "####################",
        "#...S..I...E....I..#",
        "#.P..I..##.....E...#",
        "#....I..##..I......#",
        "#..###......U..###.#",
        "#.E....I..S........#",
        "#.....####....I....#",
        "#..I.......E....I..#",
        "#......U......S....#",
        "#.E....###.........#",
        "#...........I......#",
        "####################",
    }},
    {{
        "####################",
        "#.I....E..S....I...#",
        "#.P..##.....I..##..#",
        "#....##..E.........#",
        "#..I....###....I...#",
        "#....U..S..U..E....#",
        "#.E......I.........#",
        "#...###......###...#",
        "#..S....I..E....I..#",
        "#.....I.....####...#",
        "#.................I#",
        "####################",
    }},
    {{
        "####################",
        "#..S..E....I....I..#",
        "#.P....I..###......#",
        "#..###......I..E...#",
        "#.I....U......U....#",
        "#....####..S.......#",
        "#..E.....I....###..#",
        "#......###..E......#",
        "#.I..S.......I.....#",
        "#...E....####...I..#",
        "#...........I......#",
        "####################",
    }}
};

static const int kDirDx[5] = {0, 0, 0, -1, 1};
static const int kDirDy[5] = {0, -1, 1, 0, 0};

static uint32_t rng_next(GameState *gs);
static int clampi(int v, int lo, int hi);

static int abs_int_fast(int v) {
    return (v < 0) ? -v : v;
}

static int rng_pick_small(GameState *gs, int count) {
    uint32_t r;
    if (count <= 1) {
        return 0;
    }
    r = rng_next(gs);
    if (count == 2) {
        return (int)(r & 1u);
    }
    if (count == 3) {
        return (int)(((r & 0xFFu) * 3u) >> 8);
    }
    return (int)(r & 3u);
}

static uint16_t slot_bit(int index) {
    return (uint16_t)((uint16_t)1u << (uint16_t)index);
}

static uint16_t live_enemy_mask_for_game(const GameState *gs) {
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    return gs->alive_enemy_mask;
#else
    uint16_t mask = 0;
    for (int i = 0; i < gs->enemy_count && i < GAME_MAX_ENEMIES; ++i) {
        if (gs->enemies[i].alive) {
            mask |= slot_bit(i);
        }
    }
    return mask;
#endif
}

static uint16_t active_moving_block_mask_for_game(const GameState *gs) {
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    return gs->active_moving_block_mask;
#else
    uint16_t mask = 0;
    for (int i = 0; i < GAME_MAX_MOVING_BLOCKS; ++i) {
        if (gs->moving_blocks[i].active) {
            mask |= slot_bit(i);
        }
    }
    return mask;
#endif
}

static uint16_t active_score_popup_mask_for_game(const GameState *gs) {
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    return gs->active_score_popup_mask;
#else
    uint16_t mask = 0;
    for (int i = 0; i < GAME_MAX_SCORE_POPUPS; ++i) {
        if (gs->score_popups[i].active) {
            mask |= slot_bit(i);
        }
    }
    return mask;
#endif
}

static uint16_t active_impact_fx_mask_for_game(const GameState *gs) {
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    return gs->active_impact_fx_mask;
#else
    uint16_t mask = 0;
    for (int i = 0; i < GAME_MAX_IMPACT_FX; ++i) {
        if (gs->impact_fx[i].active) {
            mask |= slot_bit(i);
        }
    }
    return mask;
#endif
}

static void emit_event(GameState *gs, GameEventFlags event_flag) {
    gs->pending_events |= (uint32_t)event_flag;
}

static void mark_static_cell_dirty(GameState *gs, int x, int y) {
    if (!gs || x < 0 || y < 0 || x >= GAME_GRID_WIDTH || y >= GAME_GRID_HEIGHT) {
        return;
    }
    gs->dirty_static_rows[y] |= (uint32_t)1u << (uint32_t)x;
}

static void mark_all_static_dirty(GameState *gs) {
    int y;
    if (!gs) {
        return;
    }
    for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
        gs->dirty_static_rows[y] = (uint32_t)((1UL << GAME_GRID_WIDTH) - 1UL);
    }
}

static void copy_level(LevelDef *dst, const LevelDef *src) {
    memcpy(dst, src, sizeof(*dst));
}

#if !defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE)
static bool level_char_allowed(char c) {
    switch (c) {
        case '#':
        case '.':
        case 'I':
        case 'S':
        case 'C':
        case 'U':
        case 'P':
        case 'E':
            return true;
        default:
            return false;
    }
}

static bool validate_level_rows_internal(const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]) {
    int player_count = 0;
    int enemy_count = 0;
    int player_x = -1;
    int player_y = -1;
    int enemies_x[GAME_MAX_ENEMIES];
    int enemies_y[GAME_MAX_ENEMIES];
    int enemies_stored = 0;

    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            const char c = rows[y][x];
            if (!level_char_allowed(c)) {
                return false;
            }

            if (c == 'P') {
                ++player_count;
                player_x = x;
                player_y = y;
            } else if (c == 'E') {
                ++enemy_count;
                if (enemies_stored < GAME_MAX_ENEMIES) {
                    enemies_x[enemies_stored] = x;
                    enemies_y[enemies_stored] = y;
                    ++enemies_stored;
                }
            }
        }
        if (rows[y][GAME_GRID_WIDTH] != '\0') {
            return false;
        }
    }

    if (player_count != 1) {
        return false;
    }
    if (enemy_count < 1) {
        return false;
    }

    for (int i = 0; i < enemies_stored; ++i) {
        const int dist = abs(enemies_x[i] - player_x) + abs(enemies_y[i] - player_y);
        if (dist <= 1) {
            return false;
        }
    }

    int exits = 0;
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = player_x + kDirDx[d];
        const int ny = player_y + kDirDy[d];
        if (nx < 0 || ny < 0 || nx >= GAME_GRID_WIDTH || ny >= GAME_GRID_HEIGHT) {
            continue;
        }
        const char c = rows[ny][nx];
        if (c == '.' || c == 'P' || c == 'E') {
            ++exits;
        }
    }
    if (exits < 2) {
        return false;
    }

    return true;
}

static void procedural_level_reset(LevelDef *out_level) {
    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            if (x == 0 || y == 0 || x == GAME_GRID_WIDTH - 1 || y == GAME_GRID_HEIGHT - 1) {
                out_level->rows[y][x] = '#';
            } else {
                out_level->rows[y][x] = '.';
            }
        }
        out_level->rows[y][GAME_GRID_WIDTH] = '\0';
    }
}

static int random_range_inclusive(GameState *gs, int lo, int hi) {
    if (hi <= lo) {
        return lo;
    }
    const uint32_t span = (uint32_t)(hi - lo + 1);
    return lo + (int)(rng_next(gs) % span);
}

static bool is_level_floor_char(char c) {
    return c == '.' || c == 'P' || c == 'E';
}

static bool is_level_breakable_block_char(char c) {
    return c == 'I' || c == 'S' || c == 'C';
}

static void analyze_reachable_floor_region(
    const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    int start_x,
    int start_y,
    int *out_reachable,
    int *out_junctions) {
    enum {
        MAX_QUEUE = GAME_GRID_WIDTH * GAME_GRID_HEIGHT
    };
    ICEPANIC_SCRATCH bool visited[GAME_GRID_HEIGHT][GAME_GRID_WIDTH];
    ICEPANIC_SCRATCH int queue_x[MAX_QUEUE];
    ICEPANIC_SCRATCH int queue_y[MAX_QUEUE];
    int head = 0;
    int tail = 0;
    int reachable = 0;
    int junctions = 0;

    memset(visited, 0, sizeof(visited));

    if (out_reachable == NULL || out_junctions == NULL) {
        return;
    }
    *out_reachable = 0;
    *out_junctions = 0;

    if (start_x <= 0 || start_y <= 0 || start_x >= GAME_GRID_WIDTH - 1 || start_y >= GAME_GRID_HEIGHT - 1) {
        return;
    }
    if (!is_level_floor_char(rows[start_y][start_x])) {
        return;
    }

    queue_x[tail] = start_x;
    queue_y[tail] = start_y;
    visited[start_y][start_x] = true;
    ++tail;

    while (head < tail) {
        const int x = queue_x[head];
        const int y = queue_y[head];
        int local_neighbors = 0;
        ++head;
        ++reachable;

        for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
            const int nx = x + kDirDx[d];
            const int ny = y + kDirDy[d];
            if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
                continue;
            }
            if (!is_level_floor_char(rows[ny][nx])) {
                continue;
            }
            ++local_neighbors;
            if (visited[ny][nx]) {
                continue;
            }
            if (tail >= MAX_QUEUE) {
                continue;
            }
            visited[ny][nx] = true;
            queue_x[tail] = nx;
            queue_y[tail] = ny;
            ++tail;
        }

        if (local_neighbors >= 3) {
            ++junctions;
        }
    }

    *out_reachable = reachable;
    *out_junctions = junctions;
}

static int count_level_walkable_neighbors(const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1], int x, int y) {
    int exits = 0;
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = x + kDirDx[d];
        const int ny = y + kDirDy[d];
        if (nx < 1 || ny < 1 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        const char c = rows[ny][nx];
        if (c == '.' || c == 'P' || c == 'E') {
            ++exits;
        }
    }
    return exits;
}

static int count_adjacent_breakable_level_blocks(
    const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    int x,
    int y) {
    int breakable = 0;
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = x + kDirDx[d];
        const int ny = y + kDirDy[d];
        if (nx < 1 || ny < 1 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        if (is_level_breakable_block_char(rows[ny][nx])) {
            ++breakable;
        }
    }
    return breakable;
}

static int count_mine_anchor_opportunities(
    const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]) {
    int opportunities = 0;
    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            int breakable_neighbors = 0;
            int walkable_neighbors = 0;
            const char here = rows[y][x];
            if (here != '.' && here != 'P') {
                continue;
            }

            for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
                const int nx = x + kDirDx[d];
                const int ny = y + kDirDy[d];
                const char c = rows[ny][nx];
                if (is_level_breakable_block_char(c)) {
                    ++breakable_neighbors;
                }
                if (is_level_floor_char(c)) {
                    ++walkable_neighbors;
                }
            }

            if (breakable_neighbors >= 2 && walkable_neighbors >= 2) {
                ++opportunities;
            }
        }
    }
    return opportunities;
}

static void add_mine_cluster_pockets(
    GameState *gs,
    char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    int spawn_x,
    int spawn_y,
    int target_opportunities) {
    int opportunities = count_mine_anchor_opportunities(rows);
    int tries = 0;
    while (opportunities < target_opportunities && tries < 900) {
        Direction dirs[4];
        int dir_count = 0;
        int placements = 0;
        const int x = random_range_inclusive(gs, 1, GAME_GRID_WIDTH - 2);
        const int y = random_range_inclusive(gs, 1, GAME_GRID_HEIGHT - 2);
        ++tries;

        if (rows[y][x] != '.') {
            continue;
        }
        if (abs(x - spawn_x) + abs(y - spawn_y) <= 3) {
            continue;
        }
        if (count_level_walkable_neighbors(rows, x, y) < 2) {
            continue;
        }

        for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
            const int nx = x + kDirDx[d];
            const int ny = y + kDirDy[d];
            if (nx < 1 || ny < 1 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
                continue;
            }
            if (rows[ny][nx] != '.') {
                continue;
            }
            if (abs(nx - spawn_x) + abs(ny - spawn_y) <= 2) {
                continue;
            }
            if (count_level_walkable_neighbors(rows, nx, ny) < 2) {
                continue;
            }
            dirs[dir_count++] = d;
        }

        if (dir_count < 2) {
            continue;
        }

        while (dir_count > 0 && placements < 2) {
            const int pick = (int)(rng_next(gs) % (uint32_t)dir_count);
            const Direction d = dirs[pick];
            const int bx = x + kDirDx[d];
            const int by = y + kDirDy[d];
            rows[by][bx] = 'I';
            ++placements;
            dirs[pick] = dirs[dir_count - 1];
            --dir_count;
            if ((rng_next(gs) & 3u) == 0u) {
                break;
            }
        }

        if (placements > 0) {
            opportunities = count_mine_anchor_opportunities(rows);
        }
    }
}

static void build_level_rows_from_runtime_map(
    const GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count,
    char out_rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]) {
    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            char c = '.';
            if (gs->terrain[y][x] == TERRAIN_WALL || gs->terrain[y][x] == TERRAIN_BORDER) {
                c = '#';
            } else {
                switch (gs->blocks[y][x]) {
                    case BLOCK_ICE:
                        c = 'I';
                        break;
                    case BLOCK_SPECIAL:
                        c = 'S';
                        break;
                    case BLOCK_CRACKED:
                        c = 'C';
                        break;
                    case BLOCK_UNBREAKABLE:
                        c = 'U';
                        break;
                    case BLOCK_NONE:
                    default:
                        c = '.';
                        break;
                }
            }
            out_rows[y][x] = c;
        }
        out_rows[y][GAME_GRID_WIDTH] = '\0';
    }

    if (gs->player_spawn_x >= 0 && gs->player_spawn_x < GAME_GRID_WIDTH &&
        gs->player_spawn_y >= 0 && gs->player_spawn_y < GAME_GRID_HEIGHT) {
        out_rows[gs->player_spawn_y][gs->player_spawn_x] = 'P';
    }
    for (int i = 0; i < enemy_spawn_count; ++i) {
        const int ex = enemy_spawns[i][0];
        const int ey = enemy_spawns[i][1];
        if (ex <= 0 || ey <= 0 || ex >= GAME_GRID_WIDTH - 1 || ey >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        if (out_rows[ey][ex] == '.') {
            out_rows[ey][ex] = 'E';
        }
    }
}

static void apply_runtime_blocks_from_level_rows(
    GameState *gs,
    const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]) {
    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            BlockType next = BLOCK_NONE;
            switch (rows[y][x]) {
                case 'I':
                    next = BLOCK_ICE;
                    break;
                case 'S':
                    next = BLOCK_SPECIAL;
                    break;
                case 'C':
                    next = BLOCK_CRACKED;
                    break;
                case 'U':
                    next = BLOCK_UNBREAKABLE;
                    break;
                default:
                    next = BLOCK_NONE;
                    break;
            }
            if (gs->blocks[y][x] != next) {
                gs->blocks[y][x] = next;
                mark_static_cell_dirty(gs, x, y);
            }
        }
    }
}

static bool has_slide_lane_from_empty(
    const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    int x,
    int y,
    int min_clear_tiles) {
    if (rows[y][x] != '.') {
        return false;
    }
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        int cx = x;
        int cy = y;
        int clear_tiles = 0;
        while (true) {
            const int nx = cx + kDirDx[d];
            const int ny = cy + kDirDy[d];
            if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
                break;
            }
            if (rows[ny][nx] != '.') {
                break;
            }
            ++clear_tiles;
            cx = nx;
            cy = ny;
        }
        if (clear_tiles >= min_clear_tiles) {
            return true;
        }
    }
    return false;
}

static bool has_slide_lane_from_block(
    const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    int x,
    int y,
    int min_clear_tiles) {
    if (rows[y][x] != 'I' && rows[y][x] != 'S' && rows[y][x] != 'C') {
        return false;
    }
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        int cx = x;
        int cy = y;
        int clear_tiles = 0;
        while (true) {
            const int nx = cx + kDirDx[d];
            const int ny = cy + kDirDy[d];
            if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
                break;
            }
            if (rows[ny][nx] != '.') {
                break;
            }
            ++clear_tiles;
            cx = nx;
            cy = ny;
        }
        if (clear_tiles >= min_clear_tiles) {
            return true;
        }
    }
    return false;
}

static int count_level_pushable_blocks(const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]) {
    int count = 0;
    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            if (rows[y][x] == 'I' || rows[y][x] == 'S' || rows[y][x] == 'C') {
                ++count;
            }
        }
    }
    return count;
}

static bool level_tile_is_far_from_enemies(
    int x,
    int y,
    const int enemy_x[GAME_MAX_ENEMIES],
    const int enemy_y[GAME_MAX_ENEMIES],
    int enemy_count,
    int min_dist) {
    for (int i = 0; i < enemy_count; ++i) {
        if (abs(x - enemy_x[i]) + abs(y - enemy_y[i]) < min_dist) {
            return false;
        }
    }
    return true;
}

static void remove_level_point_from_candidates(
    int candidate_x[GAME_GRID_WIDTH * GAME_GRID_HEIGHT],
    int candidate_y[GAME_GRID_WIDTH * GAME_GRID_HEIGHT],
    int *candidate_count,
    int x,
    int y) {
    if (candidate_count == NULL) {
        return;
    }
    for (int i = 0; i < *candidate_count; ++i) {
        if (candidate_x[i] != x || candidate_y[i] != y) {
            continue;
        }
        candidate_x[i] = candidate_x[*candidate_count - 1];
        candidate_y[i] = candidate_y[*candidate_count - 1];
        --(*candidate_count);
        return;
    }
}

static bool can_convert_ice_to_unbreakable(
    const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    int x,
    int y,
    int spawn_x,
    int spawn_y,
    const int enemy_x[GAME_MAX_ENEMIES],
    const int enemy_y[GAME_MAX_ENEMIES],
    int enemy_count,
    int spawn_guard_dist) {
    if (x <= 0 || y <= 0 || x >= GAME_GRID_WIDTH - 1 || y >= GAME_GRID_HEIGHT - 1) {
        return false;
    }
    if (rows[y][x] != 'I') {
        return false;
    }
    if (abs(x - spawn_x) + abs(y - spawn_y) <= spawn_guard_dist) {
        return false;
    }
    for (int i = 0; i < enemy_count; ++i) {
        if (abs(x - enemy_x[i]) + abs(y - enemy_y[i]) <= 1) {
            return false;
        }
    }
    return true;
}

static bool unbreakable_landmark_has_spacing(
    const int landmark_x[8],
    const int landmark_y[8],
    int landmark_count,
    int x,
    int y,
    int min_dist) {
    for (int i = 0; i < landmark_count; ++i) {
        if (abs(landmark_x[i] - x) + abs(landmark_y[i] - y) < min_dist) {
            return false;
        }
    }
    return true;
}

static int place_unbreakable_signature_blocks(
    GameState *gs,
    char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    int round_index,
    int spawn_x,
    int spawn_y,
    const int enemy_x[GAME_MAX_ENEMIES],
    const int enemy_y[GAME_MAX_ENEMIES],
    int enemy_count) {
    enum {
        MAX_CANDIDATES = GAME_GRID_WIDTH * GAME_GRID_HEIGHT
    };
    ICEPANIC_SCRATCH int candidate_x[MAX_CANDIDATES];
    ICEPANIC_SCRATCH int candidate_y[MAX_CANDIDATES];
    int candidate_count = 0;
    int converted = 0;
    int target = 2;
    int pushable_count = count_level_pushable_blocks(rows);
    int min_pushable_remaining = (round_index <= 5) ? 9 : 8;
    int max_convert_by_pushable = pushable_count - min_pushable_remaining;
    const int center_x = GAME_GRID_WIDTH / 2;
    const int center_y = GAME_GRID_HEIGHT / 2;
    const int spawn_guard_dist = (round_index <= 4) ? 4 : 3;
    const int landmark_spacing = (round_index <= 3) ? 4 : 3;
    int motif_targets_x[6];
    int motif_targets_y[6];
    int motif_target_count = 0;
    int motif = 0;
    int landmark_x[8];
    int landmark_y[8];
    int landmark_count = 0;

    if (round_index >= 3) {
        target = 3;
    }
    if (round_index >= 6) {
        target = 4;
    }
    if (round_index >= 9) {
        target = 5;
    }
    if (gs->stage_modifier == STAGE_MOD_BLACK_ICE) {
        ++target;
    }

    if (max_convert_by_pushable <= 0) {
        return 0;
    }
    if (target > max_convert_by_pushable) {
        target = max_convert_by_pushable;
    }
    if (target <= 0) {
        return 0;
    }

    /* Deterministic motif anchors are introduced in later rounds. */
    if (round_index >= 4) {
        motif = (int)(rng_next(gs) % 4u);
        switch (motif) {
            case 0: /* Gate */
                motif_targets_x[motif_target_count] = center_x - 3;
                motif_targets_y[motif_target_count++] = center_y;
                motif_targets_x[motif_target_count] = center_x + 3;
                motif_targets_y[motif_target_count++] = center_y;
                motif_targets_x[motif_target_count] = center_x;
                motif_targets_y[motif_target_count++] = center_y - 2;
                motif_targets_x[motif_target_count] = center_x;
                motif_targets_y[motif_target_count++] = center_y + 2;
                break;
            case 1: /* Twin columns */
                motif_targets_x[motif_target_count] = center_x - 4;
                motif_targets_y[motif_target_count++] = center_y - 2;
                motif_targets_x[motif_target_count] = center_x - 4;
                motif_targets_y[motif_target_count++] = center_y + 2;
                motif_targets_x[motif_target_count] = center_x + 4;
                motif_targets_y[motif_target_count++] = center_y - 2;
                motif_targets_x[motif_target_count] = center_x + 4;
                motif_targets_y[motif_target_count++] = center_y + 2;
                break;
            case 2: /* Spine */
                motif_targets_x[motif_target_count] = center_x;
                motif_targets_y[motif_target_count++] = center_y - 3;
                motif_targets_x[motif_target_count] = center_x;
                motif_targets_y[motif_target_count++] = center_y;
                motif_targets_x[motif_target_count] = center_x;
                motif_targets_y[motif_target_count++] = center_y + 3;
                motif_targets_x[motif_target_count] = center_x - 2;
                motif_targets_y[motif_target_count++] = center_y;
                motif_targets_x[motif_target_count] = center_x + 2;
                motif_targets_y[motif_target_count++] = center_y;
                break;
            case 3: /* Offset forks */
            default:
                motif_targets_x[motif_target_count] = center_x - 3;
                motif_targets_y[motif_target_count++] = center_y - 1;
                motif_targets_x[motif_target_count] = center_x - 1;
                motif_targets_y[motif_target_count++] = center_y + 2;
                motif_targets_x[motif_target_count] = center_x + 2;
                motif_targets_y[motif_target_count++] = center_y - 2;
                motif_targets_x[motif_target_count] = center_x + 4;
                motif_targets_y[motif_target_count++] = center_y + 1;
                break;
        }
    }

    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            if (!can_convert_ice_to_unbreakable(
                    rows,
                    x,
                    y,
                    spawn_x,
                    spawn_y,
                    enemy_x,
                    enemy_y,
                    enemy_count,
                    spawn_guard_dist)) {
                continue;
            }
            candidate_x[candidate_count] = x;
            candidate_y[candidate_count] = y;
            ++candidate_count;
        }
    }

    if (candidate_count <= 0) {
        return 0;
    }

    while (converted < target && candidate_count > 0) {
        int best_pick = -1;
        int best_score = -32767;

        for (int i = 0; i < candidate_count; ++i) {
            const int cx = candidate_x[i];
            const int cy = candidate_y[i];
            const int center_score = (GAME_GRID_WIDTH + GAME_GRID_HEIGHT) - (abs(cx - center_x) + abs(cy - center_y));
            const int adjacent_breakable = count_adjacent_breakable_level_blocks(rows, cx, cy);
            const int lane_score = has_slide_lane_from_block(rows, cx, cy, 2) ? 7 : 0;
            int score = (adjacent_breakable * 12) + (center_score * 2) + lane_score;

            if (motif_target_count > 0) {
                const int target_idx = converted % motif_target_count;
                const int motif_dist =
                    abs(cx - motif_targets_x[target_idx]) + abs(cy - motif_targets_y[target_idx]);
                score += clampi(10 - (motif_dist * 2), -6, 10);
            }

            if (landmark_count > 0 &&
                !unbreakable_landmark_has_spacing(landmark_x, landmark_y, landmark_count, cx, cy, landmark_spacing)) {
                score -= 40;
            }

            if (score > best_score || (score == best_score && ((rng_next(gs) & 1u) == 0u))) {
                best_score = score;
                best_pick = i;
            }
        }

        if (best_pick < 0) {
            break;
        }

        {
            const int ax = candidate_x[best_pick];
            const int ay = candidate_y[best_pick];
            rows[ay][ax] = 'U';
            remove_level_point_from_candidates(candidate_x, candidate_y, &candidate_count, ax, ay);
            if (landmark_count < (int)(sizeof(landmark_x) / sizeof(landmark_x[0]))) {
                landmark_x[landmark_count] = ax;
                landmark_y[landmark_count] = ay;
                ++landmark_count;
            }
        }
        ++converted;

        /* Pair nearby unbreakables so each round has clearer "immovable landmarks". */
        if (converted < target && round_index >= 2) {
            int neighbor_best_x = -1;
            int neighbor_best_y = -1;
            int neighbor_best_score = -32767;
            const int ax = landmark_x[landmark_count - 1];
            const int ay = landmark_y[landmark_count - 1];
            const int dir_bias = (int)(rng_next(gs) % 4u);

            for (int step = 0; step < 4; ++step) {
                const Direction d = (Direction)(1 + ((dir_bias + step) % 4));
                const int nx = ax + kDirDx[d];
                const int ny = ay + kDirDy[d];
                const int neighbor_score =
                    count_adjacent_breakable_level_blocks(rows, nx, ny) * 12 +
                    (((GAME_GRID_WIDTH + GAME_GRID_HEIGHT) - (abs(nx - center_x) + abs(ny - center_y))) * 2);

                if (!can_convert_ice_to_unbreakable(
                        rows,
                        nx,
                        ny,
                        spawn_x,
                        spawn_y,
                        enemy_x,
                        enemy_y,
                        enemy_count,
                        spawn_guard_dist)) {
                    continue;
                }
                if (neighbor_score > neighbor_best_score) {
                    neighbor_best_score = neighbor_score;
                    neighbor_best_x = nx;
                    neighbor_best_y = ny;
                }
            }

            if (neighbor_best_x >= 0 && neighbor_best_y >= 0) {
                rows[neighbor_best_y][neighbor_best_x] = 'U';
                remove_level_point_from_candidates(candidate_x, candidate_y, &candidate_count, neighbor_best_x, neighbor_best_y);
                ++converted;
            }
        }
    }

    return converted;
}

static int add_landmark_breakable_escorts(
    GameState *gs,
    char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    int round_index,
    int spawn_x,
    int spawn_y,
    const int enemy_x[GAME_MAX_ENEMIES],
    const int enemy_y[GAME_MAX_ENEMIES],
    int enemy_count) {
    int added = 0;
    int max_add = 0;

    if (round_index >= 6) {
        max_add = 1;
    }
    if (round_index >= 9) {
        max_add = 2;
    }
    if (max_add <= 0) {
        return 0;
    }

    for (int y = 1; y < GAME_GRID_HEIGHT - 1 && added < max_add; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1 && added < max_add; ++x) {
            const int dir_bias = (int)(rng_next(gs) % 4u);
            if (rows[y][x] != 'U') {
                continue;
            }
            for (int step = 0; step < 4; ++step) {
                const Direction d = (Direction)(1 + ((dir_bias + step) % 4));
                const int nx = x + kDirDx[d];
                const int ny = y + kDirDy[d];
                if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
                    continue;
                }
                if (rows[ny][nx] != '.') {
                    continue;
                }
                if (abs(nx - spawn_x) + abs(ny - spawn_y) <= 2) {
                    continue;
                }
                if (!level_tile_is_far_from_enemies(nx, ny, enemy_x, enemy_y, enemy_count, 2)) {
                    continue;
                }
                if (count_level_walkable_neighbors(rows, nx, ny) < 2) {
                    continue;
                }
                rows[ny][nx] = 'I';
                ++added;
                break;
            }
        }
    }

    return added;
}

#ifndef ICEPANIC_AMIGA_OCS_FIXED_LEVELS
static bool procgen_template_is_recent(const GameState *gs, int template_index) {
    for (int i = 0; i < gs->recent_procgen_template_count; ++i) {
        if (gs->recent_procgen_template_ids[i] == template_index) {
            return true;
        }
    }
    return false;
}

static void procgen_template_record_recent(GameState *gs, int template_index) {
    if (template_index < 0) {
        return;
    }
    if (gs->recent_procgen_template_count < (int)(sizeof(gs->recent_procgen_template_ids) / sizeof(gs->recent_procgen_template_ids[0]))) {
        gs->recent_procgen_template_ids[gs->recent_procgen_template_count++] = template_index;
        return;
    }
    gs->recent_procgen_template_ids[gs->recent_procgen_template_cursor] = template_index;
    gs->recent_procgen_template_cursor =
        (gs->recent_procgen_template_cursor + 1) %
        (int)(sizeof(gs->recent_procgen_template_ids) / sizeof(gs->recent_procgen_template_ids[0]));
}

static bool finalize_generated_level_candidate(
    GameState *gs,
    int round_index,
    LevelDef *level,
    int spawn_x,
    int spawn_y,
    const int enemy_x[GAME_MAX_ENEMIES],
    const int enemy_y[GAME_MAX_ENEMIES],
    int enemy_markers) {
    int spawn_region_reachable = 0;
    int spawn_region_junctions = 0;
    int nearest_enemy_dist = GAME_GRID_WIDTH + GAME_GRID_HEIGHT;
    int total_slide_blocks = 0;
    const int min_spawn_enemy_dist = (round_index <= 5) ? 7 : 6;
    const int min_reachable_floor = clampi(40 + (round_index / 4), 40, 52);
    const int min_region_junctions = (round_index <= 5) ? 6 : 5;
    const int min_mine_anchor_opportunities = clampi(3 + (round_index / 5), 3, 6);

    if (enemy_markers <= 0) {
        return false;
    }

    if (count_level_walkable_neighbors(level->rows, spawn_x, spawn_y) < 2) {
        for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
            const int nx = spawn_x + kDirDx[d];
            const int ny = spawn_y + kDirDy[d];
            if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
                continue;
            }
            if (level->rows[ny][nx] == 'E') {
                continue;
            }
            level->rows[ny][nx] = '.';
        }
    }

    add_mine_cluster_pockets(
        gs,
        level->rows,
        spawn_x,
        spawn_y,
        min_mine_anchor_opportunities);
    if (count_mine_anchor_opportunities(level->rows) < min_mine_anchor_opportunities) {
        return false;
    }

    if (!validate_level_rows_internal(level->rows)) {
        return false;
    }

    analyze_reachable_floor_region(
        level->rows,
        spawn_x,
        spawn_y,
        &spawn_region_reachable,
        &spawn_region_junctions);
    if (spawn_region_reachable < min_reachable_floor || spawn_region_junctions < min_region_junctions) {
        return false;
    }

    for (int i = 0; i < enemy_markers; ++i) {
        const int dist = abs(enemy_x[i] - spawn_x) + abs(enemy_y[i] - spawn_y);
        if (dist < nearest_enemy_dist) {
            nearest_enemy_dist = dist;
        }
    }
    if (nearest_enemy_dist < min_spawn_enemy_dist) {
        return false;
    }

    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            if (has_slide_lane_from_block(level->rows, x, y, 2)) {
                ++total_slide_blocks;
            }
        }
    }
    if (total_slide_blocks < 5) {
        return false;
    }

    return true;
}

static bool generate_procedural_level_from_template(
    GameState *gs,
    int round_index,
    int template_index,
    LevelDef *out_level) {
    enum {
        MAX_INTERIOR = GAME_GRID_WIDTH * GAME_GRID_HEIGHT
    };
    int spawn_x = 2;
    int spawn_y = 2;
    int enemy_markers = 0;
    ICEPANIC_SCRATCH int candidate_x[MAX_INTERIOR];
    ICEPANIC_SCRATCH int candidate_y[MAX_INTERIOR];
    int candidate_count = 0;
    ICEPANIC_SCRATCH int enemy_x[GAME_MAX_ENEMIES];
    ICEPANIC_SCRATCH int enemy_y[GAME_MAX_ENEMIES];
    const int min_spawn_enemy_dist = (round_index <= 5) ? 7 : 6;

    if (template_index < 0 || template_index >= PROCGEN_WFC_TEMPLATE_COUNT) {
        return false;
    }

    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        memcpy(out_level->rows[y], kProcgenWfcTemplates[template_index][y], GAME_GRID_WIDTH + 1);
    }

    for (int y = 2; y < GAME_GRID_HEIGHT - 2; ++y) {
        for (int x = 2; x < GAME_GRID_WIDTH - 2; ++x) {
            if (out_level->rows[y][x] != '.') {
                continue;
            }
            if (count_level_walkable_neighbors(out_level->rows, x, y) < 3) {
                continue;
            }
            candidate_x[candidate_count] = x;
            candidate_y[candidate_count] = y;
            ++candidate_count;
        }
    }
    if (candidate_count <= 0) {
        return false;
    }
    {
        const int pick = (int)(rng_next(gs) % (uint32_t)candidate_count);
        spawn_x = candidate_x[pick];
        spawn_y = candidate_y[pick];
        out_level->rows[spawn_y][spawn_x] = 'P';
    }

    candidate_count = 0;
    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            if (out_level->rows[y][x] != '.') {
                continue;
            }
            if (abs(x - spawn_x) + abs(y - spawn_y) < min_spawn_enemy_dist) {
                continue;
            }
            if (count_level_walkable_neighbors(out_level->rows, x, y) < 3) {
                continue;
            }
            candidate_x[candidate_count] = x;
            candidate_y[candidate_count] = y;
            ++candidate_count;
        }
    }
    {
        const int spawn_target = random_range_inclusive(gs, 3, 4 + (round_index / 4));
        int max_spawns = spawn_target;
        if (max_spawns > 6) {
            max_spawns = 6;
        }
        while (enemy_markers < max_spawns && candidate_count > 0) {
            const int pick = (int)(rng_next(gs) % (uint32_t)candidate_count);
            const int ex = candidate_x[pick];
            const int ey = candidate_y[pick];
            bool too_close = false;

            for (int i = 0; i < enemy_markers; ++i) {
                if (abs(enemy_x[i] - ex) + abs(enemy_y[i] - ey) <= 2) {
                    too_close = true;
                    break;
                }
            }

            if (!too_close) {
                out_level->rows[ey][ex] = 'E';
                enemy_x[enemy_markers] = ex;
                enemy_y[enemy_markers] = ey;
                ++enemy_markers;
            }

            candidate_x[pick] = candidate_x[candidate_count - 1];
            candidate_y[pick] = candidate_y[candidate_count - 1];
            --candidate_count;
        }
    }

    if (enemy_markers <= 0) {
        const int ex = (spawn_x < GAME_GRID_WIDTH / 2) ? (GAME_GRID_WIDTH - 3) : 2;
        const int ey = (spawn_y < GAME_GRID_HEIGHT / 2) ? (GAME_GRID_HEIGHT - 3) : 2;
        if (out_level->rows[ey][ex] == '.') {
            out_level->rows[ey][ex] = 'E';
            enemy_x[enemy_markers] = ex;
            enemy_y[enemy_markers] = ey;
            ++enemy_markers;
        }
    }

    {
        const int sanctuary_radius = (round_index <= 1) ? 3 : ((round_index <= 4) ? 2 : 1);
        for (int y = spawn_y - sanctuary_radius; y <= spawn_y + sanctuary_radius; ++y) {
            for (int x = spawn_x - sanctuary_radius; x <= spawn_x + sanctuary_radius; ++x) {
                if (x <= 0 || y <= 0 || x >= GAME_GRID_WIDTH - 1 || y >= GAME_GRID_HEIGHT - 1) {
                    continue;
                }
                if (abs(x - spawn_x) + abs(y - spawn_y) > sanctuary_radius) {
                    continue;
                }
                if (out_level->rows[y][x] == 'P' || out_level->rows[y][x] == 'E' || out_level->rows[y][x] == 'S') {
                    continue;
                }
                out_level->rows[y][x] = '.';
            }
        }
    }

    (void)place_unbreakable_signature_blocks(
        gs,
        out_level->rows,
        round_index,
        spawn_x,
        spawn_y,
        enemy_x,
        enemy_y,
        enemy_markers);
    (void)add_landmark_breakable_escorts(
        gs,
        out_level->rows,
        round_index,
        spawn_x,
        spawn_y,
        enemy_x,
        enemy_y,
        enemy_markers);

    return finalize_generated_level_candidate(
        gs,
        round_index,
        out_level,
        spawn_x,
        spawn_y,
        enemy_x,
        enemy_y,
        enemy_markers);
}

static bool generate_procedural_level_from_wfc_templates(
    GameState *gs,
    int round_index,
    LevelDef *out_level,
    int *out_template_index) {
    enum {
        MAX_TEMPLATE_ORDER = 256
    };
    int count = PROCGEN_WFC_TEMPLATE_COUNT;
    ICEPANIC_SCRATCH int order[MAX_TEMPLATE_ORDER];

    *out_template_index = -1;
    if (count <= 0) {
        return false;
    }
    if (count > MAX_TEMPLATE_ORDER) {
        count = MAX_TEMPLATE_ORDER;
    }

    if (gs->debug_force_procgen_template_pending) {
        const int forced_idx = gs->debug_forced_procgen_template_index;
        gs->debug_force_procgen_template_pending = false;
        if (generate_procedural_level_from_template(gs, round_index, forced_idx, out_level)) {
            *out_template_index = forced_idx;
            return true;
        }
        return false;
    }

    for (int i = 0; i < count; ++i) {
        order[i] = i;
    }
    for (int i = count - 1; i > 0; --i) {
        const int swap_idx = (int)(rng_next(gs) % (uint32_t)(i + 1));
        const int tmp = order[i];
        order[i] = order[swap_idx];
        order[swap_idx] = tmp;
    }

    for (int i = 0; i < count; ++i) {
        const int candidate = order[i];
        if (procgen_template_is_recent(gs, candidate)) {
            continue;
        }
        if (generate_procedural_level_from_template(gs, round_index, candidate, out_level)) {
            *out_template_index = candidate;
            return true;
        }
    }

    for (int i = 0; i < count; ++i) {
        const int candidate = order[i];
        if (generate_procedural_level_from_template(gs, round_index, candidate, out_level)) {
            *out_template_index = candidate;
            return true;
        }
    }

    return false;
}

static bool generate_procedural_level(GameState *gs, int round_index, LevelDef *out_level) {
    enum {
        MAX_PROC_ATTEMPTS = 48,
        MAX_INTERIOR = GAME_GRID_WIDTH * GAME_GRID_HEIGHT
    };

    for (int attempt = 0; attempt < MAX_PROC_ATTEMPTS; ++attempt) {
        int spawn_x = 2;
        int spawn_y = 2;
        int enemy_markers = 0;
        ICEPANIC_SCRATCH int candidate_x[MAX_INTERIOR];
        ICEPANIC_SCRATCH int candidate_y[MAX_INTERIOR];
        int candidate_count = 0;
        ICEPANIC_SCRATCH int enemy_x[GAME_MAX_ENEMIES];
        ICEPANIC_SCRATCH int enemy_y[GAME_MAX_ENEMIES];
        int special_target = 3;
        int specials_placed = 0;
        int ice_placed = 0;
        int total_slide_blocks = 0;
        int placed_wall_cells = 0;
        int spawn_region_reachable = 0;
        int spawn_region_junctions = 0;
        int nearest_enemy_dist = GAME_GRID_WIDTH + GAME_GRID_HEIGHT;
        const int max_wall_cells = clampi(12 + round_index, 12, 26);
        const int min_spawn_enemy_dist = (round_index <= 5) ? 7 : 6;
        const int min_reachable_floor = clampi(40 + (round_index / 4), 40, 52);
        const int min_region_junctions = (round_index <= 5) ? 6 : 5;
        const int min_mine_anchor_opportunities = clampi(3 + (round_index / 5), 3, 6);

        procedural_level_reset(out_level);

        /* Pick player spawn first, then keep nearby tiles clear for fairness. */
        {
            int spawn_candidates = 0;
            for (int y = 2; y < GAME_GRID_HEIGHT - 2; ++y) {
                for (int x = 2; x < GAME_GRID_WIDTH - 2; ++x) {
                    if (count_level_walkable_neighbors(out_level->rows, x, y) < 3) {
                        continue;
                    }
                    candidate_x[spawn_candidates] = x;
                    candidate_y[spawn_candidates] = y;
                    ++spawn_candidates;
                }
            }
            if (spawn_candidates > 0) {
                const int pick = (int)(rng_next(gs) % (uint32_t)spawn_candidates);
                spawn_x = candidate_x[pick];
                spawn_y = candidate_y[pick];
            } else {
                spawn_x = GAME_GRID_WIDTH / 2;
                spawn_y = GAME_GRID_HEIGHT / 2;
            }
            out_level->rows[spawn_y][spawn_x] = 'P';
        }

        /* Add random wall segments to vary routes while keeping spawn area open. */
        {
            const int segment_count = random_range_inclusive(gs, 2, 3 + (round_index / 4));
            for (int seg = 0; seg < segment_count; ++seg) {
                const bool horizontal = (rng_next(gs) & 1u) != 0u;
                const int len = random_range_inclusive(gs, 2, 3 + (round_index / 6));
                int x = random_range_inclusive(gs, 1, GAME_GRID_WIDTH - 2);
                int y = random_range_inclusive(gs, 1, GAME_GRID_HEIGHT - 2);
                int dx = 0;
                int dy = 0;

                if (horizontal) {
                    dx = (rng_next(gs) & 1u) ? 1 : -1;
                } else {
                    dy = (rng_next(gs) & 1u) ? 1 : -1;
                }

                for (int i = 0; i < len; ++i) {
                    if (x <= 0 || y <= 0 || x >= GAME_GRID_WIDTH - 1 || y >= GAME_GRID_HEIGHT - 1) {
                        break;
                    }
                    if (placed_wall_cells >= max_wall_cells) {
                        break;
                    }
                    if (abs(x - spawn_x) + abs(y - spawn_y) > 2 && out_level->rows[y][x] == '.') {
                        out_level->rows[y][x] = '#';
                        ++placed_wall_cells;
                    }
                    x += dx;
                    y += dy;
                }
            }
        }

        /* Keep a readable combat pocket around center. */
        {
            const int cx = GAME_GRID_WIDTH / 2;
            const int cy = GAME_GRID_HEIGHT / 2;
            const int pocket_radius = 2;
            for (int y = cy - pocket_radius; y <= cy + pocket_radius; ++y) {
                for (int x = cx - pocket_radius; x <= cx + pocket_radius; ++x) {
                    if (x <= 0 || y <= 0 || x >= GAME_GRID_WIDTH - 1 || y >= GAME_GRID_HEIGHT - 1) {
                        continue;
                    }
                    if (out_level->rows[y][x] == '#') {
                        out_level->rows[y][x] = '.';
                    }
                }
            }
        }

        /* Place enemy spawn markers far enough from player and from each other. */
        {
            const int spawn_target = random_range_inclusive(gs, 3, 4 + (round_index / 4));
            int max_spawns = spawn_target;
            if (max_spawns > 6) {
                max_spawns = 6;
            }

            for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
                for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
                    if (out_level->rows[y][x] != '.') {
                        continue;
                    }
                    if (abs(x - spawn_x) + abs(y - spawn_y) < min_spawn_enemy_dist) {
                        continue;
                    }
                    if (count_level_walkable_neighbors(out_level->rows, x, y) < 3) {
                        continue;
                    }
                    candidate_x[candidate_count] = x;
                    candidate_y[candidate_count] = y;
                    ++candidate_count;
                }
            }

            while (enemy_markers < max_spawns && candidate_count > 0) {
                const int pick = (int)(rng_next(gs) % (uint32_t)candidate_count);
                const int ex = candidate_x[pick];
                const int ey = candidate_y[pick];
                bool too_close = false;

                for (int i = 0; i < enemy_markers; ++i) {
                    if (abs(enemy_x[i] - ex) + abs(enemy_y[i] - ey) <= 2) {
                        too_close = true;
                        break;
                    }
                }

                if (!too_close) {
                    out_level->rows[ey][ex] = 'E';
                    enemy_x[enemy_markers] = ex;
                    enemy_y[enemy_markers] = ey;
                    ++enemy_markers;
                }

                candidate_x[pick] = candidate_x[candidate_count - 1];
                candidate_y[pick] = candidate_y[candidate_count - 1];
                --candidate_count;
            }
        }

        if (enemy_markers <= 0) {
            /* Fallback enemy marker far from spawn. */
            const int ex = (spawn_x < GAME_GRID_WIDTH / 2) ? (GAME_GRID_WIDTH - 3) : 2;
            const int ey = (spawn_y < GAME_GRID_HEIGHT / 2) ? (GAME_GRID_HEIGHT - 3) : 2;
            if (out_level->rows[ey][ex] == '.') {
                out_level->rows[ey][ex] = 'E';
                enemy_x[enemy_markers] = ex;
                enemy_y[enemy_markers] = ey;
                enemy_markers = 1;
            }
        }

        if (enemy_markers <= 0) {
            continue;
        }

        /* Place exactly three special blocks on useful slide lanes. */
        {
            int tries = 0;
            while (specials_placed < special_target && tries < 400) {
                const int x = random_range_inclusive(gs, 1, GAME_GRID_WIDTH - 2);
                const int y = random_range_inclusive(gs, 1, GAME_GRID_HEIGHT - 2);
                ++tries;
                if (out_level->rows[y][x] != '.') {
                    continue;
                }
                if (abs(x - spawn_x) + abs(y - spawn_y) <= 2) {
                    continue;
                }
                if (!has_slide_lane_from_empty(out_level->rows, x, y, 2)) {
                    continue;
                }
                out_level->rows[y][x] = 'S';
                ++specials_placed;
            }
        }

        if (specials_placed < special_target) {
            continue;
        }

        /* Fill pushable ice blocks with lane bias for readability and strategy. */
        {
            const int target_ice = random_range_inclusive(gs, 13 + round_index, 19 + (round_index * 2));
            const int capped_target_ice = (target_ice > 50) ? 50 : target_ice;
            int tries = 0;
            while (ice_placed < capped_target_ice && tries < 1200) {
                const int x = random_range_inclusive(gs, 1, GAME_GRID_WIDTH - 2);
                const int y = random_range_inclusive(gs, 1, GAME_GRID_HEIGHT - 2);
                ++tries;
                if (out_level->rows[y][x] != '.') {
                    continue;
                }
                if (abs(x - spawn_x) + abs(y - spawn_y) <= 2) {
                    continue;
                }

                const bool lane = has_slide_lane_from_empty(out_level->rows, x, y, 2);
                const int adjacent_breakable = count_adjacent_breakable_level_blocks(out_level->rows, x, y);
                if (!lane) {
                    const uint32_t roll = rng_next(gs) % 100u;
                    if (adjacent_breakable > 0) {
                        if (roll >= 40u) {
                            continue;
                        }
                    } else {
                        if (roll >= 22u) {
                            continue;
                        }
                    }
                }
                out_level->rows[y][x] = 'I';
                ++ice_placed;
            }
        }

        /* Keep a small sanctuary around spawn for fair starts in roguelite runs. */
        {
            const int sanctuary_radius = (round_index <= 1) ? 3 : ((round_index <= 4) ? 2 : 1);
            for (int y = spawn_y - sanctuary_radius; y <= spawn_y + sanctuary_radius; ++y) {
                for (int x = spawn_x - sanctuary_radius; x <= spawn_x + sanctuary_radius; ++x) {
                    if (x <= 0 || y <= 0 || x >= GAME_GRID_WIDTH - 1 || y >= GAME_GRID_HEIGHT - 1) {
                        continue;
                    }
                    if (abs(x - spawn_x) + abs(y - spawn_y) > sanctuary_radius) {
                        continue;
                    }
                    if (out_level->rows[y][x] == 'P' || out_level->rows[y][x] == 'E' || out_level->rows[y][x] == 'S') {
                        continue;
                    }
                    out_level->rows[y][x] = '.';
                }
            }
        }

        (void)place_unbreakable_signature_blocks(
            gs,
            out_level->rows,
            round_index,
            spawn_x,
            spawn_y,
            enemy_x,
            enemy_y,
            enemy_markers);
        (void)add_landmark_breakable_escorts(
            gs,
            out_level->rows,
            round_index,
            spawn_x,
            spawn_y,
            enemy_x,
            enemy_y,
            enemy_markers);

        /* Ensure at least two immediate exits at spawn for fairness. */
        if (count_level_walkable_neighbors(out_level->rows, spawn_x, spawn_y) < 2) {
            for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
                const int nx = spawn_x + kDirDx[d];
                const int ny = spawn_y + kDirDy[d];
                if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
                    continue;
                }
                if (out_level->rows[ny][nx] == 'E') {
                    continue;
                }
                out_level->rows[ny][nx] = '.';
            }
        }

        add_mine_cluster_pockets(
            gs,
            out_level->rows,
            spawn_x,
            spawn_y,
            min_mine_anchor_opportunities);
        if (count_mine_anchor_opportunities(out_level->rows) < min_mine_anchor_opportunities) {
            continue;
        }

        if (!validate_level_rows_internal(out_level->rows)) {
            continue;
        }

        analyze_reachable_floor_region(
            out_level->rows,
            spawn_x,
            spawn_y,
            &spawn_region_reachable,
            &spawn_region_junctions);
        if (spawn_region_reachable < min_reachable_floor || spawn_region_junctions < min_region_junctions) {
            continue;
        }

        for (int i = 0; i < enemy_markers; ++i) {
            const int dist = abs(enemy_x[i] - spawn_x) + abs(enemy_y[i] - spawn_y);
            if (dist < nearest_enemy_dist) {
                nearest_enemy_dist = dist;
            }
        }
        if (nearest_enemy_dist < min_spawn_enemy_dist) {
            continue;
        }

        for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
            for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
                if (has_slide_lane_from_block(out_level->rows, x, y, 2)) {
                    ++total_slide_blocks;
                }
            }
        }

        if (total_slide_blocks < 5) {
            continue;
        }

        return true;
    }

    return false;
}
#endif
#endif

static void resolve_level_for_round(GameState *gs, int round_index, LevelDef *out_level) {
#ifdef ICEPANIC_AMIGA_OCS_FIXED_LEVELS
    const int level_count = (int)(sizeof(kDefaultLevels) / sizeof(kDefaultLevels[0]));
    const int level_idx = (round_index - 1) % level_count;
    copy_level(out_level, &kDefaultLevels[(level_idx < 0) ? 0 : level_idx]);
    gs->debug_last_procgen_template_index = -1;
#else
    const uint32_t rng_before_template_attempt = gs->rng_state;
    int template_index = -1;
    const bool wfc_enabled_for_round = (round_index >= 1) || gs->debug_force_procgen_template_pending;
    if (wfc_enabled_for_round) {
        if (generate_procedural_level_from_wfc_templates(gs, round_index, out_level, &template_index)) {
            gs->debug_last_procgen_template_index = template_index;
            procgen_template_record_recent(gs, template_index);
            return;
        }
    }
    gs->rng_state = rng_before_template_attempt;
    if (generate_procedural_level(gs, round_index, out_level)) {
        gs->debug_last_procgen_template_index = -1;
        return;
    }

    const int level_count = (int)(sizeof(kDefaultLevels) / sizeof(kDefaultLevels[0]));
    const int level_idx = (round_index - 1) % level_count;
    copy_level(out_level, &kDefaultLevels[(level_idx < 0) ? 0 : level_idx]);
    gs->debug_last_procgen_template_index = -1;
#endif
}

static uint32_t rng_next(GameState *gs) {
    uint32_t x = gs->rng_state;
    if (x == 0) {
        x = 0x6ac690c5u;
    }
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    gs->rng_state = x;
    return x;
}

static Direction opposite_direction(Direction d) {
    switch (d) {
        case DIR_UP:
            return DIR_DOWN;
        case DIR_DOWN:
            return DIR_UP;
        case DIR_LEFT:
            return DIR_RIGHT;
        case DIR_RIGHT:
            return DIR_LEFT;
        default:
            return DIR_NONE;
    }
}

static int clampi(int v, int lo, int hi) {
    if (v < lo) {
        return lo;
    }
    if (v > hi) {
        return hi;
    }
    return v;
}

static int min_int(int a, int b) {
    return (a < b) ? a : b;
}

static GamePixelFp abs_fp(GamePixelFp v) {
    return (v < 0) ? -v : v;
}

static uint32_t add_u32_saturating(uint32_t base, uint32_t amount) {
    const uint32_t room = UINT32_MAX - base;
    if (amount > room) {
        return UINT32_MAX;
    }
    return base + amount;
}

static GamePixelFp tile_to_fp(int tile) {
    return (GamePixelFp)tile * (GamePixelFp)GAME_TILE_SIZE * (GamePixelFp)FP_ONE;
}

static bool inside_grid(int x, int y) {
    return x >= 0 && x < GAME_GRID_WIDTH && y >= 0 && y < GAME_GRID_HEIGHT;
}

static bool terrain_blocks(Terrain t) {
    return t == TERRAIN_WALL || t == TERRAIN_BORDER;
}

static bool direction_is_held(const InputState *in, Direction dir) {
    switch (dir) {
        case DIR_UP:
            return in->up;
        case DIR_DOWN:
            return in->down;
        case DIR_LEFT:
            return in->left;
        case DIR_RIGHT:
            return in->right;
        default:
            return false;
    }
}

static Direction choose_held_player_direction(const Player *player, const InputState *in) {
    if (player && player->buffered_dir != DIR_NONE && direction_is_held(in, player->buffered_dir)) {
        return player->buffered_dir;
    }
    if (player && player->facing != DIR_NONE && direction_is_held(in, player->facing)) {
        return player->facing;
    }
    if (in->up) {
        return DIR_UP;
    }
    if (in->down) {
        return DIR_DOWN;
    }
    if (in->left) {
        return DIR_LEFT;
    }
    if (in->right) {
        return DIR_RIGHT;
    }
    return DIR_NONE;
}

static void reset_fire_confirm_gate(GameState *gs) {
    gs->fire_confirm_armed = false;
}

static bool consume_fire_confirm_release(GameState *gs, const InputState *in) {
    if (in->fire_released) {
        const bool confirm = gs->fire_confirm_armed;
        gs->fire_confirm_armed = false;
        return confirm;
    }
    if (!gs->fire_was_down) {
        gs->fire_confirm_armed = true;
    }
    return false;
}

static int count_alive_enemies(const GameState *gs) {
    int alive = 0;
    uint16_t mask = live_enemy_mask_for_game(gs);
    for (int i = 0; mask != 0u && i < gs->enemy_count; ++i, mask >>= 1) {
        if ((mask & 1u) != 0u && gs->enemies[i].alive && gs->enemies[i].type != ENEMY_TYPE_GHOST) {
            ++alive;
        }
    }
    if (alive > 0 || gs->alive_enemy_count <= 0) {
        return alive;
    }
    for (int i = 0; i < gs->enemy_count; ++i) {
        if (gs->enemies[i].alive && gs->enemies[i].type != ENEMY_TYPE_GHOST) {
            ++alive;
        }
    }
    return alive;
}

static bool enemy_is_killable(const Enemy *enemy) {
    return enemy->type != ENEMY_TYPE_GHOST;
}

static bool enemy_can_trigger_mine(const Enemy *enemy) {
    return enemy->type != ENEMY_TYPE_GHOST;
}

static bool any_live_enemy_active(const GameState *gs) {
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    return gs->alive_enemy_mask != 0u;
#else
    uint16_t mask = live_enemy_mask_for_game(gs);
    for (int i = 0; mask != 0u && i < gs->enemy_count; ++i, mask >>= 1) {
        if ((mask & 1u) != 0u && gs->enemies[i].alive) {
            return true;
        }
    }
    for (int i = 0; i < gs->enemy_count; ++i) {
        if (gs->enemies[i].alive) {
            return true;
        }
    }
    return false;
#endif
}

static void clear_enemy_alive_bit(GameState *gs, int enemy_idx) {
    gs->alive_enemy_mask &= (uint16_t)~slot_bit(enemy_idx);
}

static void mark_enemy_defeated(GameState *gs, int enemy_idx) {
    Enemy *enemy = &gs->enemies[enemy_idx];
    if (!enemy->alive) {
        clear_enemy_alive_bit(gs, enemy_idx);
        return;
    }
    enemy->alive = false;
    clear_enemy_alive_bit(gs, enemy_idx);
    if (enemy_is_killable(enemy) && gs->alive_enemy_count > 0) {
        --gs->alive_enemy_count;
    }
}

static int live_enemy_count_fast(const GameState *gs) {
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    return gs->alive_enemy_count;
#endif
    return count_alive_enemies(gs);
}

static bool round_clear_pending_active(const GameState *gs) {
    return gs->phase == GAME_PHASE_PLAYING && gs->round_clear_pending_ticks > 0;
}

static bool tile_has_enemy(const GameState *gs, int x, int y, int ignore_idx) {
    int i = 0;
    uint16_t mask = live_enemy_mask_for_game(gs);
    while (mask != 0u) {
        if (i == ignore_idx) {
            mask >>= 1;
            ++i;
            continue;
        }
        if ((mask & 1u) != 0u && gs->enemies[i].tile_x == x && gs->enemies[i].tile_y == y) {
            return true;
        }
        mask >>= 1;
        ++i;
    }
    if (live_enemy_mask_for_game(gs) != 0u || gs->alive_enemy_count <= 0) {
        return false;
    }
    for (i = 0; i < gs->enemy_count; ++i) {
        if (i != ignore_idx && gs->enemies[i].alive &&
            gs->enemies[i].tile_x == x && gs->enemies[i].tile_y == y) {
            return true;
        }
    }
    return false;
}

static void build_enemy_occupancy(const GameState *gs, uint32_t occupancy[GAME_GRID_HEIGHT]) {
    int i = 0;
    uint16_t mask = live_enemy_mask_for_game(gs);
    memset(occupancy, 0, sizeof(uint32_t) * (size_t)GAME_GRID_HEIGHT);
    while (mask != 0u) {
        const Enemy *enemy = &gs->enemies[i];
        if ((mask & 1u) != 0u && inside_grid(enemy->tile_x, enemy->tile_y)) {
            occupancy[enemy->tile_y] |= (uint32_t)1u << (uint32_t)enemy->tile_x;
        }
        mask >>= 1;
        ++i;
    }
}

static void build_moving_block_occupancy(const GameState *gs, uint32_t occupancy[GAME_GRID_HEIGHT]) {
    int i = 0;
    uint16_t mask = active_moving_block_mask_for_game(gs);
    memset(occupancy, 0, sizeof(uint32_t) * (size_t)GAME_GRID_HEIGHT);
    while (mask != 0u) {
        const MovingBlock *mb = &gs->moving_blocks[i];
        if ((mask & 1u) != 0u && inside_grid(mb->tile_x, mb->tile_y)) {
            occupancy[mb->tile_y] |= (uint32_t)1u << (uint32_t)mb->tile_x;
        }
        mask >>= 1;
        ++i;
    }
}

static bool enemy_occupancy_has(
    const uint32_t occupancy[GAME_GRID_HEIGHT],
    int x,
    int y) {
    if (!inside_grid(x, y)) {
        return false;
    }
    return (occupancy[y] & ((uint32_t)1u << (uint32_t)x)) != 0;
}

static void enemy_occupancy_move(
    uint32_t occupancy[GAME_GRID_HEIGHT],
    int old_x,
    int old_y,
    int new_x,
    int new_y) {
    if (inside_grid(old_x, old_y)) {
        occupancy[old_y] &= ~((uint32_t)1u << (uint32_t)old_x);
    }
    if (inside_grid(new_x, new_y)) {
        occupancy[new_y] |= (uint32_t)1u << (uint32_t)new_x;
    }
}

static bool tile_has_moving_block(const GameState *gs, int x, int y, int ignore_idx) {
    int i = 0;
    uint16_t mask = active_moving_block_mask_for_game(gs);
    while (mask != 0u) {
        if (i == ignore_idx) {
            mask >>= 1;
            ++i;
            continue;
        }
        if ((mask & 1u) != 0u && gs->moving_blocks[i].tile_x == x && gs->moving_blocks[i].tile_y == y) {
            return true;
        }
        mask >>= 1;
        ++i;
    }
    return false;
}

static bool occupancy_has_tile(
    const uint32_t occupancy[GAME_GRID_HEIGHT],
    int x,
    int y) {
    if (occupancy == NULL) {
        return false;
    }
    if (!inside_grid(x, y)) {
        return false;
    }
    return (occupancy[y] & ((uint32_t)1u << (uint32_t)x)) != 0;
}

static bool tile_walkable_for_enemy_with_occupancy(
    const GameState *gs,
    const uint32_t occupancy[GAME_GRID_HEIGHT],
    const uint32_t moving_block_occupancy[GAME_GRID_HEIGHT],
    EnemyType enemy_type,
    int x,
    int y) {
    if (!inside_grid(x, y)) {
        return false;
    }
    if (terrain_blocks(gs->terrain[y][x])) {
        return false;
    }
    if (enemy_type != ENEMY_TYPE_GHOST && gs->blocks[y][x] != BLOCK_NONE) {
        return false;
    }
    if (occupancy_has_tile(moving_block_occupancy, x, y)) {
        return false;
    }
    if (enemy_occupancy_has(occupancy, x, y)) {
        return false;
    }
    return true;
}

static bool slide_target_blocked(const GameState *gs, int x, int y, int moving_idx) {
    if (!inside_grid(x, y)) {
        return true;
    }
    if (terrain_blocks(gs->terrain[y][x])) {
        return true;
    }
    if (gs->blocks[y][x] != BLOCK_NONE) {
        return true;
    }
    if (tile_has_moving_block(gs, x, y, moving_idx)) {
        return true;
    }
    return false;
}

static int score_for_combo(int combo_count) {
    if (combo_count <= 1) {
        return 100;
    }
    if (combo_count == 2) {
        return 250;
    }
    if (combo_count == 3) {
        return 550;
    }
    if (combo_count == 4) {
        return 900;
    }
    return 1300;
}

static int score_for_mine_combo(int combo_count) {
    int score = (int)(((int32_t)score_for_combo(combo_count) * (int32_t)MINE_KILL_SCORE_PERMILLE) /
                      (int32_t)RUN_SCORE_MULT_BASE_PERMILLE);
    if (score < MINE_KILL_SCORE_MIN) {
        score = MINE_KILL_SCORE_MIN;
    }
    return score;
}

static int score_for_item(ItemType type) {
    switch (type) {
        case ITEM_FISH:
            return 500;
        case ITEM_EGG:
            return 800;
        case ITEM_GEM:
            return 1200;
        case ITEM_FRUIT:
            return 1600;
        case ITEM_MYSTERY:
            return 2000;
        default:
            return 0;
    }
}

static int round_clear_time_bonus_base_score(int round_time_ticks) {
    int remaining_seconds;
    if (round_time_ticks <= 0) {
        return 0;
    }
    remaining_seconds = (round_time_ticks + GAME_FIXED_TICK_HZ - 1) / GAME_FIXED_TICK_HZ;
    return remaining_seconds * 100;
}

static int award_score(GameState *gs, int base_score) {
    if (base_score <= 0) {
        return 0;
    }
    if (gs->score >= SCORE_CAP) {
        return 0;
    }
    int32_t scaled32 = ((int32_t)base_score * (int32_t)gs->run_score_mult_permille) /
                       (int32_t)RUN_SCORE_MULT_BASE_PERMILLE;
    int scaled = scaled32 > 32767 ? 32767 : (int)scaled32;
    if (scaled <= 0) {
        scaled = 1;
    }
    {
        uint32_t awarded = (uint32_t)scaled;
        const uint32_t room = SCORE_CAP - gs->score;
        if (awarded > room) {
            awarded = room;
        }
        gs->score += awarded;
        return (int)awarded;
    }
}

static void add_run_shards(GameState *gs, int amount) {
    if (amount <= 0) {
        return;
    }
    const int next = gs->run_shards + amount;
    gs->run_shards = (next < gs->run_shards) ? gs->run_shards : next;
}

#if defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE)
static StageModifierType pick_stage_modifier_for_round(GameState *gs, int round_index) {
    return STAGE_MOD_NONE;
}

static void apply_stage_modifier_to_round_config(GameState *gs) {
}

static void apply_stage_modifier_to_map(
    GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count) {
}
#else
static const StageModifierType kStageModifiers[] = {
    STAGE_MOD_SWARM,
    STAGE_MOD_SHATTERED_ICE,
    STAGE_MOD_BLACK_ICE,
    STAGE_MOD_TURBO_SLIDE,
    STAGE_MOD_FROST_SIREN,
};

static void age_stage_modifier_cooldowns(GameState *gs) {
    for (int mod = STAGE_MOD_SWARM; mod <= STAGE_MOD_FROST_SIREN; ++mod) {
        if (gs->stage_modifier_cooldowns[mod] > 0) {
            --gs->stage_modifier_cooldowns[mod];
        }
    }
}

static int stage_modifier_weight_for_draft(
    const GameState *gs,
    StageModifierType modifier,
    int round_index,
    bool allow_cooldown) {
    int weight = 100;
    if (modifier <= STAGE_MOD_NONE || modifier > STAGE_MOD_FROST_SIREN) {
        return 0;
    }
    if (modifier == STAGE_MOD_SWARM) {
        weight += clampi((round_index - 2) * 4, 0, 32);
    } else if (modifier == STAGE_MOD_SHATTERED_ICE) {
        weight += 10;
    } else if (modifier == STAGE_MOD_BLACK_ICE) {
        weight += clampi((round_index - 3) * 3, 0, 20);
    } else if (modifier == STAGE_MOD_TURBO_SLIDE) {
        weight += clampi((round_index - 4) * 4, 0, 24);
    } else if (modifier == STAGE_MOD_FROST_SIREN) {
        weight += clampi((round_index - 4) * 4, 0, 24);
    }

    if (gs->stage_modifier_cooldowns[modifier] > 0) {
        if (!allow_cooldown) {
            return 0;
        }
        weight /= 5;
        if (weight < 1) {
            weight = 1;
        }
    }

    return weight;
}

static StageModifierType pick_stage_modifier_for_round(GameState *gs, int round_index) {
    if (gs->debug_force_stage_modifier_pending) {
        const StageModifierType forced = gs->debug_forced_stage_modifier;
        gs->debug_force_stage_modifier_pending = false;
        gs->debug_forced_stage_modifier = STAGE_MOD_NONE;
        if (forced != STAGE_MOD_NONE) {
            gs->stage_modifier_cooldowns[forced] = STAGE_MODIFIER_RECENT_ROUNDS;
        }
        return forced;
    }

    if (round_index <= 2) {
        return STAGE_MOD_NONE;
    }

    age_stage_modifier_cooldowns(gs);

    int fresh_count = 0;
    for (size_t i = 0; i < sizeof(kStageModifiers) / sizeof(kStageModifiers[0]); ++i) {
        if (gs->stage_modifier_cooldowns[kStageModifiers[i]] == 0) {
            ++fresh_count;
        }
    }

    const bool allow_cooldown = (fresh_count == 0);
    int total_weight = 0;
    int weights[STAGE_MOD_FROST_SIREN + 1] = {0};
    for (size_t i = 0; i < sizeof(kStageModifiers) / sizeof(kStageModifiers[0]); ++i) {
        const StageModifierType modifier = kStageModifiers[i];
        const int weight = stage_modifier_weight_for_draft(gs, modifier, round_index, allow_cooldown);
        weights[modifier] = weight;
        total_weight += weight;
    }

    if (total_weight <= 0) {
        return STAGE_MOD_NONE;
    }

    int roll = (int)(rng_next(gs) % (uint32_t)total_weight);
    for (size_t i = 0; i < sizeof(kStageModifiers) / sizeof(kStageModifiers[0]); ++i) {
        const StageModifierType modifier = kStageModifiers[i];
        const int weight = weights[modifier];
        if (weight <= 0) {
            continue;
        }
        if (roll < weight) {
            gs->stage_modifier_cooldowns[modifier] = STAGE_MODIFIER_RECENT_ROUNDS;
            return modifier;
        }
        roll -= weight;
    }

    return STAGE_MOD_NONE;
}

static bool tile_reserved_for_spawn(
    int tile_x,
    int tile_y,
    int player_x,
    int player_y,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count) {
    if (tile_x == player_x && tile_y == player_y) {
        return true;
    }
    if (abs(tile_x - player_x) + abs(tile_y - player_y) <= 1) {
        return true;
    }
    for (int i = 0; i < enemy_spawn_count; ++i) {
        if (tile_x == enemy_spawns[i][0] && tile_y == enemy_spawns[i][1]) {
            return true;
        }
    }
    return false;
}

static void apply_stage_modifier_to_round_config(GameState *gs) {
    switch (gs->stage_modifier) {
        case STAGE_MOD_SWARM:
            gs->round_config.enemy_count = clampi(
                gs->round_config.enemy_count + STAGE_MOD_SWARM_ENEMY_BONUS,
                1,
                GAME_MAX_ENEMIES);
            gs->round_config.aggression_percent = clampi(
                gs->round_config.aggression_percent + STAGE_MOD_SWARM_AGGRESSION_BONUS,
                28,
                80);
            gs->round_config.enemy_speed_fp = clampi(
                gs->round_config.enemy_speed_fp + STAGE_MOD_SWARM_SPEED_BONUS_FP,
                FP_ONE,
                (9 * FP_ONE) / 4);
            gs->round_config.enemy_wanderer_count = clampi(
                gs->round_config.enemy_wanderer_count + 1,
                0,
                gs->round_config.enemy_count - 1);
            break;
        case STAGE_MOD_BLACK_ICE:
            gs->round_config.aggression_percent = clampi(
                gs->round_config.aggression_percent + 4,
                28,
                80);
            break;
        case STAGE_MOD_SHATTERED_ICE:
            gs->round_config.round_time_ticks = clampi(
                gs->round_config.round_time_ticks + 3 * GAME_FIXED_TICK_HZ,
                25 * GAME_FIXED_TICK_HZ,
                120 * GAME_FIXED_TICK_HZ);
            break;
        case STAGE_MOD_TURBO_SLIDE:
            gs->round_config.aggression_percent = clampi(
                gs->round_config.aggression_percent + STAGE_MOD_TURBO_SLIDE_AGGRESSION_BONUS,
                28,
                80);
            gs->round_config.round_time_ticks = clampi(
                gs->round_config.round_time_ticks - STAGE_MOD_TURBO_SLIDE_TIME_PENALTY_TICKS,
                20 * GAME_FIXED_TICK_HZ,
                120 * GAME_FIXED_TICK_HZ);
            break;
        case STAGE_MOD_FROST_SIREN:
            gs->round_config.aggression_percent = clampi(
                gs->round_config.aggression_percent + STAGE_MOD_FROST_SIREN_AGGRESSION_BONUS,
                28,
                80);
            gs->round_config.round_time_ticks = clampi(
                gs->round_config.round_time_ticks - STAGE_MOD_FROST_SIREN_TIME_PENALTY_TICKS,
                20 * GAME_FIXED_TICK_HZ,
                120 * GAME_FIXED_TICK_HZ);
            break;
        case STAGE_MOD_NONE:
        default:
            break;
    }
#if defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE)
    if (gs->round_config.enemy_count > OCS_BOB_ENEMY_SOFT_CAP) {
        const int overflow = gs->round_config.enemy_count - OCS_BOB_ENEMY_SOFT_CAP;
        gs->round_config.enemy_count = OCS_BOB_ENEMY_SOFT_CAP;
        gs->round_config.aggression_percent = clampi(
            gs->round_config.aggression_percent + overflow * 4,
            28,
            86);
        gs->round_config.enemy_speed_fp = clampi(
            gs->round_config.enemy_speed_fp + overflow * (FP_ONE / 16),
            FP_ONE,
            (5 * FP_ONE) / 2);
    }
#endif
    {
        const int special_slots = (gs->round_config.enemy_count > 1) ? (gs->round_config.enemy_count - 1) : 0;
        gs->round_config.enemy_ghost_count = clampi(
            gs->round_config.enemy_ghost_count,
            0,
            min_int(1, special_slots));
        gs->round_config.enemy_hunter_count = clampi(
            gs->round_config.enemy_hunter_count,
            0,
            special_slots - gs->round_config.enemy_ghost_count);
        gs->round_config.enemy_wanderer_count = clampi(
            gs->round_config.enemy_wanderer_count,
            0,
            special_slots - gs->round_config.enemy_ghost_count - gs->round_config.enemy_hunter_count);
    }
}

static void apply_stage_modifier_to_map(
    GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count) {
    ICEPANIC_SCRATCH int candidates_x[GAME_GRID_WIDTH * GAME_GRID_HEIGHT];
    ICEPANIC_SCRATCH int candidates_y[GAME_GRID_WIDTH * GAME_GRID_HEIGHT];
    int candidate_count = 0;

    if (gs->stage_modifier == STAGE_MOD_SHATTERED_ICE) {
        for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
            for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
                if (gs->blocks[y][x] != BLOCK_ICE) {
                    continue;
                }
                if (tile_reserved_for_spawn(
                        x,
                        y,
                        gs->player_spawn_x,
                        gs->player_spawn_y,
                        enemy_spawns,
                        enemy_spawn_count)) {
                    continue;
                }
                candidates_x[candidate_count] = x;
                candidates_y[candidate_count] = y;
                ++candidate_count;
            }
        }

        const int max_convert = clampi(
            STAGE_MOD_SHATTERED_CONVERT_BASE + gs->round / 3,
            2,
            10);
        int convert_count = min_int(max_convert, candidate_count);
        while (convert_count > 0 && candidate_count > 0) {
            const int pick = (int)(rng_next(gs) % (uint32_t)candidate_count);
            const int x = candidates_x[pick];
            const int y = candidates_y[pick];
            gs->blocks[y][x] = BLOCK_CRACKED;
            mark_static_cell_dirty(gs, x, y);
            candidates_x[pick] = candidates_x[candidate_count - 1];
            candidates_y[pick] = candidates_y[candidate_count - 1];
            --candidate_count;
            --convert_count;
        }
        return;
    }

    if (gs->stage_modifier == STAGE_MOD_BLACK_ICE) {
        int existing_unbreakable = 0;
        const int spawn_guard_dist = (gs->round <= 4) ? 3 : 2;

        for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
            for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
                if (gs->blocks[y][x] == BLOCK_UNBREAKABLE) {
                    ++existing_unbreakable;
                }
            }
        }

        for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
            for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
                int adjacent_solid = 0;
                if (terrain_blocks(gs->terrain[y][x])) {
                    continue;
                }
                if (gs->blocks[y][x] != BLOCK_NONE) {
                    continue;
                }
                if (abs(x - gs->player_spawn_x) + abs(y - gs->player_spawn_y) <= spawn_guard_dist) {
                    continue;
                }
                if (tile_reserved_for_spawn(
                        x,
                        y,
                        gs->player_spawn_x,
                        gs->player_spawn_y,
                        enemy_spawns,
                        enemy_spawn_count)) {
                    continue;
                }
                for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
                    const int nx = x + kDirDx[d];
                    const int ny = y + kDirDy[d];
                    if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
                        continue;
                    }
                    if (terrain_blocks(gs->terrain[ny][nx]) || gs->blocks[ny][nx] != BLOCK_NONE) {
                        ++adjacent_solid;
                    }
                }
                if (adjacent_solid <= 0) {
                    continue;
                }
                candidates_x[candidate_count] = x;
                candidates_y[candidate_count] = y;
                ++candidate_count;
            }
        }

        const int density_penalty = existing_unbreakable / 3;
        const int max_add = clampi(
            STAGE_MOD_BLACK_ICE_ADD_BASE + gs->round / 4 - density_penalty,
            1,
            6);
        int add_count = min_int(max_add, candidate_count);
        while (add_count > 0 && candidate_count > 0) {
            const int pick = (int)(rng_next(gs) % (uint32_t)candidate_count);
            const int x = candidates_x[pick];
            const int y = candidates_y[pick];
            gs->blocks[y][x] = BLOCK_ICE;
            mark_static_cell_dirty(gs, x, y);
            candidates_x[pick] = candidates_x[candidate_count - 1];
            candidates_y[pick] = candidates_y[candidate_count - 1];
            --candidate_count;
            --add_count;
        }
    }
}
#endif

#if defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE) && defined(ICEPANIC_AMIGA_OCS_SKIP_PERK_UI)
static int meta_unlock_tier_from_progress(uint32_t progress_points) {
    return 1;
}

static void enter_perk_choice(GameState *gs, int next_round) {
    game_start_round(gs, next_round);
}
#else
static int meta_upgrade_cost(MetaUpgradeType upgrade) {
    switch (upgrade) {
        case META_UPGRADE_LIFE_PACK:
            return META_COST_LIFE_PACK;
        case META_UPGRADE_TIME_PACK:
            return META_COST_TIME_PACK;
        case META_UPGRADE_SCORE_PACK:
            return META_COST_SCORE_PACK;
        case META_UPGRADE_SLOW_PACK:
            return META_COST_SLOW_PACK;
        default:
            return 0;
    }
}

static int meta_unlock_tier_from_progress(uint32_t progress_points) {
    if (progress_points >= GAME_META_UNLOCK_TIER3_PROGRESS) {
        return 3;
    }
    if (progress_points >= GAME_META_UNLOCK_TIER2_PROGRESS) {
        return 2;
    }
    return 1;
}

static int meta_choice_pool_count_for_progress(uint32_t progress_points) {
    const int tier = meta_unlock_tier_from_progress(progress_points);
    return clampi(1 + tier, 2, GAME_META_CHOICE_COUNT);
}

static int perk_time_step_ticks_for_level(int level) {
    if (level <= 0) {
        return PERK_TIME_STEP_TICKS;
    }
    if (level == 1) {
        return 4 * GAME_FIXED_TICK_HZ;
    }
    if (level <= 3) {
        return 3 * GAME_FIXED_TICK_HZ;
    }
    return 3 * GAME_FIXED_TICK_HZ;
}

static int perk_score_step_permille_for_level(int level) {
    if (level <= 0) {
        return PERK_SCORE_STEP_PERMILLE;
    }
    if (level == 1) {
        return 80;
    }
    if (level == 2) {
        return 70;
    }
    if (level == 3) {
        return 65;
    }
    return 60;
}

static int perk_enemy_slow_step_fp_for_level(int level) {
    if (level <= 0) {
        return PERK_ENEMY_SLOW_STEP_FP;
    }
    if (level <= 2) {
        return FP_ONE / 8;
    }
    return FP_ONE / 16;
}

static int remaining_time_bonus_ticks(const GameState *gs) {
    if (gs->run_round_time_bonus_ticks >= RUN_TIME_BONUS_MAX_TICKS) {
        return 0;
    }
    return RUN_TIME_BONUS_MAX_TICKS - gs->run_round_time_bonus_ticks;
}

static int remaining_score_bonus_permille(const GameState *gs) {
    if (gs->run_score_mult_permille >= RUN_SCORE_MULT_MAX_PERMILLE) {
        return 0;
    }
    return RUN_SCORE_MULT_MAX_PERMILLE - gs->run_score_mult_permille;
}

static int remaining_enemy_slow_fp(const GameState *gs) {
    if (gs->run_enemy_speed_penalty_fp >= RUN_ENEMY_SLOW_MAX_FP) {
        return 0;
    }
    return RUN_ENEMY_SLOW_MAX_FP - gs->run_enemy_speed_penalty_fp;
}

static int mine_capacity_step_for_level(int level) {
    if (level <= 1) {
        return MINE_PERK_STEP;
    }
    return 1;
}

static int mine_refill_step_for_level(int level) {
    if (level <= 2) {
        return MINE_PERK_STEP;
    }
    return 1;
}

static int remaining_mine_capacity(const GameState *gs) {
    if (gs->run_mine_capacity >= RUN_MINE_CAPACITY_MAX) {
        return 0;
    }
    return RUN_MINE_CAPACITY_MAX - gs->run_mine_capacity;
}

static bool apply_life_bonus(GameState *gs, int life_count) {
    const int before = gs->lives;
    if (life_count <= 0) {
        return false;
    }
    gs->lives = clampi(gs->lives + life_count, 1, MAX_LIVES);
    return gs->lives > before;
}

static bool apply_round_time_bonus_ticks(GameState *gs, int ticks) {
    const int before = gs->run_round_time_bonus_ticks;
    if (ticks <= 0) {
        return false;
    }
    gs->run_round_time_bonus_ticks = clampi(
        gs->run_round_time_bonus_ticks + ticks,
        0,
        RUN_TIME_BONUS_MAX_TICKS);
    return gs->run_round_time_bonus_ticks > before;
}

static bool apply_score_multiplier_bonus(GameState *gs, int permille) {
    const int before = gs->run_score_mult_permille;
    if (permille <= 0) {
        return false;
    }
    gs->run_score_mult_permille = clampi(
        gs->run_score_mult_permille + permille,
        RUN_SCORE_MULT_BASE_PERMILLE,
        RUN_SCORE_MULT_MAX_PERMILLE);
    return gs->run_score_mult_permille > before;
}

static bool apply_enemy_slow_bonus(GameState *gs, int slow_fp) {
    const int before = gs->run_enemy_speed_penalty_fp;
    if (slow_fp <= 0) {
        return false;
    }
    gs->run_enemy_speed_penalty_fp = clampi(
        gs->run_enemy_speed_penalty_fp + slow_fp,
        0,
        RUN_ENEMY_SLOW_MAX_FP);
    return gs->run_enemy_speed_penalty_fp > before;
}

static bool apply_mine_bonus(GameState *gs, int capacity_gain, int stock_gain) {
    const int before_capacity = gs->run_mine_capacity;
    const int before_stock = gs->run_mine_stock;

    if (capacity_gain > 0) {
        gs->run_mine_capacity = clampi(
            gs->run_mine_capacity + capacity_gain,
            0,
            RUN_MINE_CAPACITY_MAX);
    }

    if (stock_gain > 0) {
        gs->run_mine_stock = clampi(
            gs->run_mine_stock + stock_gain,
            0,
            gs->run_mine_capacity);
    }

    return gs->run_mine_capacity > before_capacity || gs->run_mine_stock > before_stock;
}

static bool apply_meta_upgrade_choice(GameState *gs, MetaUpgradeType upgrade) {
    switch (upgrade) {
        case META_UPGRADE_LIFE_PACK:
            return apply_life_bonus(gs, 1);
        case META_UPGRADE_TIME_PACK:
            return apply_round_time_bonus_ticks(gs, META_TIME_STEP_TICKS);
        case META_UPGRADE_SCORE_PACK:
            return apply_score_multiplier_bonus(gs, META_SCORE_STEP_PERMILLE);
        case META_UPGRADE_SLOW_PACK:
            return apply_enemy_slow_bonus(gs, META_ENEMY_SLOW_STEP_FP);
        case META_UPGRADE_NONE:
        default:
            break;
    }
    return false;
}

static void build_meta_choices(GameState *gs) {
    static const MetaUpgradeType kOptions[] = {
        META_UPGRADE_LIFE_PACK,
        META_UPGRADE_TIME_PACK,
        META_UPGRADE_SCORE_PACK,
        META_UPGRADE_SLOW_PACK,
    };
    MetaUpgradeType ordered[GAME_META_CHOICE_COUNT];
    const int unlocked_count = meta_choice_pool_count_for_progress(gs->meta_progress_points);

    gs->meta_choice_count = unlocked_count;
    gs->meta_choice_selected = 0;
    for (int i = 0; i < unlocked_count; ++i) {
        ordered[i] = kOptions[i];
    }

    for (int i = unlocked_count - 1; i > 0; --i) {
        const int swap_idx = (int)(rng_next(gs) % (uint32_t)(i + 1));
        const MetaUpgradeType tmp = ordered[i];
        ordered[i] = ordered[swap_idx];
        ordered[swap_idx] = tmp;
    }

    for (int i = 0; i < unlocked_count; ++i) {
        const MetaUpgradeType upgrade = ordered[i];
        gs->meta_choices[i] = upgrade;
        gs->meta_choice_costs[i] = meta_upgrade_cost(upgrade);
    }
    for (int i = unlocked_count; i < GAME_META_CHOICE_COUNT; ++i) {
        gs->meta_choices[i] = META_UPGRADE_NONE;
        gs->meta_choice_costs[i] = 0;
    }
}

static void enter_meta_upgrade(GameState *gs) {
    gs->phase = GAME_PHASE_META_UPGRADE;
    gs->phase_timer_ticks = 0;
    gs->pending_round_after_choice = 1;
    reset_fire_confirm_gate(gs);
    build_meta_choices(gs);
}

static const PerkType kDraftPerks[] = {
    PERK_LIFE_BOOST,
    PERK_TIME_BOOST,
    PERK_SCORE_BOOST,
    PERK_ENEMY_SLOW,
    PERK_MINES,
};

static void age_perk_offer_cooldowns(GameState *gs) {
    for (int perk = PERK_LIFE_BOOST; perk <= PERK_MINES; ++perk) {
        if (gs->perk_offer_cooldowns[perk] > 0) {
            --gs->perk_offer_cooldowns[perk];
        }
    }
}

static bool perk_in_choices(const GameState *gs, PerkType perk, int used_count) {
    for (int i = 0; i < used_count; ++i) {
        if (gs->perk_choices[i] == perk) {
            return true;
        }
    }
    return false;
}

static int perk_weight_for_draft(const GameState *gs, PerkType perk, bool allow_cooldown) {
    const bool emergency_life = (perk == PERK_LIFE_BOOST && gs->lives <= 2);

    if (perk <= PERK_NONE || perk > PERK_MINES) {
        return 0;
    }

    int weight = 120 - gs->perk_levels[perk] * 18;
    if (weight < 12) {
        weight = 12;
    }
    if (gs->perk_levels[perk] == 0) {
        weight += 30;
    }

    switch (perk) {
        case PERK_LIFE_BOOST:
            if (gs->lives >= MAX_LIVES) {
                return 0;
            }
            if (gs->lives <= 2) {
                weight += 90;
            } else if (gs->lives <= 4) {
                weight += 25;
            } else if (gs->lives >= MAX_LIVES - 1) {
                weight -= 20;
            }
            break;
        case PERK_TIME_BOOST: {
            int step = perk_time_step_ticks_for_level(gs->perk_levels[PERK_TIME_BOOST]);
            int room = remaining_time_bonus_ticks(gs);
            if (room <= 0) {
                return 0;
            }
            if (step > room) {
                step = room;
            }
            weight = (weight * step) / PERK_TIME_STEP_TICKS;
            break;
        }
        case PERK_SCORE_BOOST: {
            int step = perk_score_step_permille_for_level(gs->perk_levels[PERK_SCORE_BOOST]);
            int room = remaining_score_bonus_permille(gs);
            if (room <= 0) {
                return 0;
            }
            if (step > room) {
                step = room;
            }
            weight = (weight * step) / PERK_SCORE_STEP_PERMILLE;
            break;
        }
        case PERK_ENEMY_SLOW: {
            int step = perk_enemy_slow_step_fp_for_level(gs->perk_levels[PERK_ENEMY_SLOW]);
            int room = remaining_enemy_slow_fp(gs);
            if (room <= 0) {
                return 0;
            }
            if (step > room) {
                step = room;
            }
            weight = (weight * step) / PERK_ENEMY_SLOW_STEP_FP;
            break;
        }
        case PERK_MINES: {
            const int level = gs->perk_levels[PERK_MINES];
            int capacity_gain = mine_capacity_step_for_level(level);
            int capacity_room = remaining_mine_capacity(gs);
            int stock_room = gs->run_mine_capacity - gs->run_mine_stock;

            if (capacity_room <= 0 && stock_room <= 0) {
                return 0;
            }
            if (capacity_gain > capacity_room) {
                capacity_gain = capacity_room;
            }

            if (gs->run_mine_capacity == 0) {
                weight += 85;
            } else if (gs->run_mine_stock == 0) {
                weight += 55;
            } else if (gs->run_mine_stock < gs->run_mine_capacity / 2) {
                weight += 25;
            }

            weight += (stock_room * 30) / RUN_MINE_CAPACITY_MAX;
            if (capacity_gain <= 0) {
                weight = (weight * 2) / 3;
            } else {
                weight += (capacity_gain * 35) / MINE_PERK_STEP;
            }
            break;
        }
        default:
            break;
    }

    if (gs->perk_offer_cooldowns[perk] > 0) {
        if (!allow_cooldown) {
            if (!emergency_life) {
                return 0;
            }
        }
        if (!emergency_life) {
            weight /= 6;
            if (weight < 1) {
                weight = 1;
            }
        }
    }
    if (weight <= 0) {
        return 0;
    }
    return weight;
}

static PerkType weighted_pick_perk(GameState *gs, int used_count) {
    const int remaining_slots = GAME_PERK_CHOICE_COUNT - used_count;
    int fresh_unseen = 0;
    for (size_t i = 0; i < sizeof(kDraftPerks) / sizeof(kDraftPerks[0]); ++i) {
        const PerkType perk = kDraftPerks[i];
        if (perk_in_choices(gs, perk, used_count)) {
            continue;
        }
        if (gs->perk_offer_cooldowns[perk] == 0 && perk_weight_for_draft(gs, perk, false) > 0) {
            ++fresh_unseen;
        }
    }

    const bool allow_cooldown = (fresh_unseen < remaining_slots);
    int total_weight = 0;
    int weights[PERK_MINES + 1] = {0};
    for (size_t i = 0; i < sizeof(kDraftPerks) / sizeof(kDraftPerks[0]); ++i) {
        const PerkType perk = kDraftPerks[i];
        if (perk_in_choices(gs, perk, used_count)) {
            continue;
        }
        const int w = perk_weight_for_draft(gs, perk, allow_cooldown);
        weights[perk] = w;
        total_weight += w;
    }

    if (total_weight <= 0) {
        return PERK_NONE;
    }

    int roll = (int)(rng_next(gs) % (uint32_t)total_weight);
    for (size_t i = 0; i < sizeof(kDraftPerks) / sizeof(kDraftPerks[0]); ++i) {
        const PerkType perk = kDraftPerks[i];
        const int w = weights[perk];
        if (w <= 0) {
            continue;
        }
        if (roll < w) {
            return perk;
        }
        roll -= w;
    }
    return PERK_NONE;
}

static void build_perk_choices(GameState *gs) {
    gs->perk_choice_count = GAME_PERK_CHOICE_COUNT;
    gs->perk_choice_selected = 0;
    for (int i = 0; i < GAME_PERK_CHOICE_COUNT; ++i) {
        gs->perk_choices[i] = PERK_NONE;
    }

    age_perk_offer_cooldowns(gs);

    for (int i = 0; i < GAME_PERK_CHOICE_COUNT; ++i) {
        const PerkType perk = weighted_pick_perk(gs, i);
        if (perk == PERK_NONE) {
            gs->perk_choice_count = i;
            break;
        }
        gs->perk_choices[i] = perk;
    }

    for (int i = 0; i < gs->perk_choice_count; ++i) {
        const PerkType perk = gs->perk_choices[i];
        if (perk > PERK_NONE && perk <= PERK_MINES) {
            gs->perk_offer_cooldowns[perk] = PERK_RECENT_OFFER_ROUNDS;
        }
    }
}

static void ensure_early_mine_perk_offer(GameState *gs) {
    if (gs->run_mine_capacity > 0) {
        return;
    }
    if (gs->pending_round_after_choice < 1 || gs->pending_round_after_choice > 2) {
        return;
    }
    if (gs->lives <= 1) {
        return;
    }
    if (gs->perk_choice_count <= 0) {
        return;
    }
    if (perk_in_choices(gs, PERK_MINES, gs->perk_choice_count)) {
        return;
    }

    int replace_idx = gs->perk_choice_count - 1;
    for (int i = 0; i < gs->perk_choice_count; ++i) {
        if (gs->perk_choices[i] != PERK_LIFE_BOOST) {
            replace_idx = i;
            break;
        }
    }

    const PerkType replaced = gs->perk_choices[replace_idx];
    gs->perk_choices[replace_idx] = PERK_MINES;
    gs->perk_offer_cooldowns[PERK_MINES] = PERK_RECENT_OFFER_ROUNDS;
    if (replaced > PERK_NONE && replaced <= PERK_MINES && replaced != PERK_MINES) {
        if (!perk_in_choices(gs, replaced, gs->perk_choice_count)) {
            gs->perk_offer_cooldowns[replaced] = 0;
        }
    }
}

static bool apply_perk_choice(GameState *gs, PerkType perk) {
    switch (perk) {
        case PERK_LIFE_BOOST:
            if (apply_life_bonus(gs, 1)) {
                ++gs->perk_levels[PERK_LIFE_BOOST];
                return true;
            }
            return false;
        case PERK_TIME_BOOST: {
            const int step = perk_time_step_ticks_for_level(gs->perk_levels[PERK_TIME_BOOST]);
            if (apply_round_time_bonus_ticks(gs, step)) {
                ++gs->perk_levels[PERK_TIME_BOOST];
                return true;
            }
            return false;
        }
        case PERK_SCORE_BOOST:
            if (apply_score_multiplier_bonus(gs, perk_score_step_permille_for_level(gs->perk_levels[PERK_SCORE_BOOST]))) {
                ++gs->perk_levels[PERK_SCORE_BOOST];
                return true;
            }
            return false;
        case PERK_ENEMY_SLOW:
            if (apply_enemy_slow_bonus(gs, perk_enemy_slow_step_fp_for_level(gs->perk_levels[PERK_ENEMY_SLOW]))) {
                ++gs->perk_levels[PERK_ENEMY_SLOW];
                return true;
            }
            return false;
        case PERK_MINES: {
            const int level = gs->perk_levels[PERK_MINES];
            const int cap_gain = mine_capacity_step_for_level(level);
            const int stock_gain = mine_refill_step_for_level(level);
            if (apply_mine_bonus(gs, cap_gain, stock_gain)) {
                ++gs->perk_levels[PERK_MINES];
                return true;
            }
            return false;
        }
        default:
            break;
    }
    return false;
}

static void enter_perk_choice(GameState *gs, int next_round) {
    gs->phase = GAME_PHASE_PERK_CHOICE;
    gs->phase_timer_ticks = PERK_CHOICE_FIRE_LOCK_TICKS;
    gs->pending_round_after_choice = (next_round < 1) ? 1 : next_round;
    reset_fire_confirm_gate(gs);
    build_perk_choices(gs);
    ensure_early_mine_perk_offer(gs);
}
#endif

static void clear_active_item_list(GameState *gs) {
    gs->active_item_count = 0;
    memset(gs->active_item_x, 0, sizeof(gs->active_item_x));
    memset(gs->active_item_y, 0, sizeof(gs->active_item_y));
}

static int active_item_slot(const GameState *gs, int x, int y) {
    int count = gs->active_item_count;
    if (count > GAME_MAX_ACTIVE_ITEMS) {
        count = GAME_MAX_ACTIVE_ITEMS;
    }
    for (int i = 0; i < count; ++i) {
        if ((int)gs->active_item_x[i] == x && (int)gs->active_item_y[i] == y) {
            return i;
        }
    }
    return -1;
}

static bool register_active_item(GameState *gs, int x, int y) {
    int count = gs->active_item_count;
    if (active_item_slot(gs, x, y) >= 0) {
        return true;
    }
    if (count < 0 || count >= GAME_MAX_ACTIVE_ITEMS) {
        return false;
    }
    gs->active_item_x[count] = (uint8_t)x;
    gs->active_item_y[count] = (uint8_t)y;
    gs->active_item_count = count + 1;
    return true;
}

static bool unregister_active_item(GameState *gs, int x, int y) {
    int slot;
    int last;
    if (gs->active_item_count > GAME_MAX_ACTIVE_ITEMS) {
        gs->active_item_count = GAME_MAX_ACTIVE_ITEMS;
    }
    slot = active_item_slot(gs, x, y);
    if (slot < 0) {
        return false;
    }
    last = gs->active_item_count - 1;
    if (slot != last) {
        gs->active_item_x[slot] = gs->active_item_x[last];
        gs->active_item_y[slot] = gs->active_item_y[last];
    }
    gs->active_item_x[last] = 0;
    gs->active_item_y[last] = 0;
    gs->active_item_count = last;
    return true;
}

static bool has_any_items(const GameState *gs) {
    if (gs->active_item_count > 0) {
        return true;
    }
    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            if (gs->items[y][x] != ITEM_NONE) {
                return true;
            }
        }
    }
    return false;
}

static bool tile_is_free_for_item(const GameState *gs, int x, int y) {
    if (!inside_grid(x, y)) {
        return false;
    }
    if (terrain_blocks(gs->terrain[y][x])) {
        return false;
    }
    if (gs->blocks[y][x] != BLOCK_NONE) {
        return false;
    }
    if (gs->items[y][x] != ITEM_NONE) {
        return false;
    }
    if (gs->mines[y][x]) {
        return false;
    }
    if (tile_has_moving_block(gs, x, y, -1)) {
        return false;
    }
    if (tile_has_enemy(gs, x, y, -1)) {
        return false;
    }
    if (gs->player.alive && gs->player.tile_x == x && gs->player.tile_y == y) {
        return false;
    }
    return true;
}

static ItemType choose_random_item_type(GameState *gs) {
    static const ItemType kTypes[] = {
        ITEM_FISH,
        ITEM_EGG,
        ITEM_GEM,
        ITEM_FRUIT,
        ITEM_MYSTERY,
    };
    return kTypes[rng_next(gs) % (uint32_t)(sizeof(kTypes) / sizeof(kTypes[0]))];
}

static bool spawn_bonus_item(GameState *gs, ItemType forced_type) {
    if (has_any_items(gs)) {
        return false;
    }

    ICEPANIC_SCRATCH int free_x[GAME_GRID_WIDTH * GAME_GRID_HEIGHT];
    ICEPANIC_SCRATCH int free_y[GAME_GRID_WIDTH * GAME_GRID_HEIGHT];
    int free_count = 0;
    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            if (!tile_is_free_for_item(gs, x, y)) {
                continue;
            }
            free_x[free_count] = x;
            free_y[free_count] = y;
            ++free_count;
        }
    }
    if (free_count == 0) {
        return false;
    }

    const int slot = (int)(rng_next(gs) % (uint32_t)free_count);
    const ItemType type = (forced_type == ITEM_NONE) ? choose_random_item_type(gs) : forced_type;
    gs->items[free_y[slot]][free_x[slot]] = type;
    register_active_item(gs, free_x[slot], free_y[slot]);
    mark_static_cell_dirty(gs, free_x[slot], free_y[slot]);
    gs->bonus_item_timer_ticks = BONUS_ITEM_LIFETIME_TICKS;
    return true;
}

static void clear_bonus_items(GameState *gs) {
    if (gs->active_item_count > 0) {
        while (gs->active_item_count > 0) {
            const int x = (int)gs->active_item_x[gs->active_item_count - 1];
            const int y = (int)gs->active_item_y[gs->active_item_count - 1];
            if (inside_grid(x, y) && gs->items[y][x] != ITEM_NONE) {
                gs->items[y][x] = ITEM_NONE;
                mark_static_cell_dirty(gs, x, y);
            }
            --gs->active_item_count;
        }
        memset(gs->active_item_x, 0, sizeof(gs->active_item_x));
        memset(gs->active_item_y, 0, sizeof(gs->active_item_y));
    } else {
        for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
            for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
                if (gs->items[y][x] != ITEM_NONE) {
                    gs->items[y][x] = ITEM_NONE;
                    mark_static_cell_dirty(gs, x, y);
                }
            }
        }
    }
    gs->bonus_item_timer_ticks = 0;
}

static void mark_bonus_items_dirty(GameState *gs) {
    if (gs->active_item_count > 0) {
        int i = 0;
        while (i < gs->active_item_count) {
            const int x = (int)gs->active_item_x[i];
            const int y = (int)gs->active_item_y[i];
            if (!inside_grid(x, y) || gs->items[y][x] == ITEM_NONE) {
                if (i != gs->active_item_count - 1) {
                    gs->active_item_x[i] = gs->active_item_x[gs->active_item_count - 1];
                    gs->active_item_y[i] = gs->active_item_y[gs->active_item_count - 1];
                }
                --gs->active_item_count;
                gs->active_item_x[gs->active_item_count] = 0;
                gs->active_item_y[gs->active_item_count] = 0;
                continue;
            }
            mark_static_cell_dirty(gs, x, y);
            ++i;
        }
        return;
    }

    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            if (gs->items[y][x] != ITEM_NONE) {
                mark_static_cell_dirty(gs, x, y);
            }
        }
    }
}

static bool bonus_item_visible_for_timer(int timer_ticks) {
    if (timer_ticks <= 0) {
        return false;
    }
    if (timer_ticks > BONUS_ITEM_FLASH_TICKS) {
        return true;
    }
    if (timer_ticks > GAME_FIXED_TICK_HZ) {
        return ((timer_ticks / 8) & 1) == 0;
    }
    return ((timer_ticks / 4) & 1) == 0;
}

static bool has_special_block_alignment(const GameState *gs) {
    int sx[16];
    int sy[16];
    int count = 0;

    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            if (gs->blocks[y][x] != BLOCK_SPECIAL) {
                continue;
            }
            if (count < (int)(sizeof(sx) / sizeof(sx[0]))) {
                sx[count] = x;
                sy[count] = y;
                ++count;
            }
        }
    }

    if (count < 3) {
        return false;
    }

    for (int i = 0; i < count - 2; ++i) {
        for (int j = i + 1; j < count - 1; ++j) {
            for (int k = j + 1; k < count; ++k) {
                if (sy[i] == sy[j] && sy[j] == sy[k]) {
                    return true;
                }
                if (sx[i] == sx[j] && sx[j] == sx[k]) {
                    return true;
                }
            }
        }
    }
    return false;
}

static void check_special_alignment(GameState *gs) {
    if (gs->special_alignment_awarded) {
        return;
    }
    if (!has_special_block_alignment(gs)) {
        return;
    }

    gs->special_alignment_awarded = true;
    gs->special_alignment_flash_ticks = SPECIAL_ALIGNMENT_FLASH_TICKS;
    emit_event(gs, GAME_EVENT_SPECIAL_ALIGNMENT);
    (void)award_score(gs, SPECIAL_ALIGNMENT_BONUS_SCORE);
    gs->round_time_ticks = clampi(
        gs->round_time_ticks + SPECIAL_ALIGNMENT_TIME_BONUS_TICKS,
        0,
        120 * GAME_FIXED_TICK_HZ);
    if (!has_any_items(gs)) {
        (void)spawn_bonus_item(gs, ITEM_MYSTERY);
    }
}

static int bonus_item_kill_threshold_for_state(const GameState *gs) {
    int threshold = BONUS_ITEM_KILL_THRESHOLD;

    if (gs->round <= 2) {
        threshold -= 1;
    } else if (gs->round >= 4) {
        threshold += 1;
    }

    if (gs->round >= 8) {
        threshold += 1;
    }

    if (gs->stage_modifier == STAGE_MOD_SWARM && gs->round >= 4) {
        threshold += 1;
    }

    return clampi(threshold, BONUS_ITEM_KILL_THRESHOLD_MIN, BONUS_ITEM_KILL_THRESHOLD_MAX);
}

static void on_enemy_crushed(GameState *gs) {
    const int bonus_threshold = bonus_item_kill_threshold_for_state(gs);
    ++gs->kills_this_round;
    ++gs->kills_since_item_spawn;
    add_run_shards(gs, SHARDS_PER_CRUSH);
    emit_event(gs, GAME_EVENT_CRUSH);
    if (gs->kills_since_item_spawn >= bonus_threshold) {
        if (spawn_bonus_item(gs, ITEM_NONE)) {
            gs->kills_since_item_spawn = 0;
        }
    }
}

#if defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE)
static void spawn_score_popup(GameState *gs, int tile_x, int tile_y, int value) {
    if (gs && tile_x == -9999 && tile_y == -9999 && value == -9999) {
        gs->active_score_popup_count = 0;
    }
}

static ImpactFx *spawn_impact_fx(GameState *gs, GamePixelFp pixel_fp_x, GamePixelFp pixel_fp_y, ImpactFxStyle style, int ttl_ticks) {
    int slot = -1;
    int weakest_ttl = 32767;
    int weakest_index = 0;
    uint16_t active_mask = active_impact_fx_mask_for_game(gs);
    for (int i = 0; i < GAME_MAX_IMPACT_FX; ++i) {
        if ((active_mask & slot_bit(i)) == 0u || !gs->impact_fx[i].active) {
            slot = i;
            break;
        }
        if (gs->impact_fx[i].ttl_ticks < weakest_ttl) {
            weakest_ttl = gs->impact_fx[i].ttl_ticks;
            weakest_index = i;
        }
    }
    if (slot < 0) {
        slot = weakest_index;
    }
    {
        ImpactFx *fx = &gs->impact_fx[slot];
        if (fx->active && fx->anchors_block && inside_grid(fx->anchor_tile_x, fx->anchor_tile_y)) {
            mark_static_cell_dirty(gs, fx->anchor_tile_x, fx->anchor_tile_y);
        }
        if (!fx->active) {
            ++gs->active_impact_fx_count;
        }
        gs->active_impact_fx_mask |= slot_bit(slot);
        fx->pixel_fp_x = pixel_fp_x;
        fx->pixel_fp_y = pixel_fp_y;
        fx->style = (int)style;
        fx->base_ttl_ticks = ttl_ticks;
        fx->anchor_tile_x = -1;
        fx->anchor_tile_y = -1;
        fx->direction = DIR_NONE;
        fx->ttl_ticks = ttl_ticks;
        fx->anchors_block = false;
        fx->active = true;
        return fx;
    }
}

static void spawn_impact_fx_on_tile(GameState *gs, int tile_x, int tile_y, ImpactFxStyle style, int ttl_ticks) {
    if (style != IMPACT_FX_STYLE_MINE_BLAST &&
        (style < IMPACT_FX_STYLE_ENEMY_DEATH_A || style > IMPACT_FX_STYLE_ENEMY_DEATH_D)) {
        return;
    }
    (void)spawn_impact_fx(
        gs,
        tile_to_fp(tile_x) + (GamePixelFp)((GAME_TILE_SIZE / 2) * FP_ONE),
        tile_to_fp(tile_y) + (GamePixelFp)((GAME_TILE_SIZE / 2) * FP_ONE),
        style,
        ttl_ticks);
}

static void update_score_popups(GameState *gs) {
    if (gs && gs->active_score_popup_count < 0) {
        gs->active_score_popup_count = 0;
    }
}

static void update_impact_fx(GameState *gs) {
    uint16_t mask = active_impact_fx_mask_for_game(gs);
    for (int i = 0; mask != 0u && i < GAME_MAX_IMPACT_FX; ++i, mask >>= 1) {
        uint16_t bit = slot_bit(i);
        ImpactFx *fx = &gs->impact_fx[i];
        if ((mask & 1u) == 0u) {
            continue;
        }
        if (!fx->active) {
            gs->active_impact_fx_mask &= (uint16_t)~bit;
            if (gs->active_impact_fx_count > 0) {
                --gs->active_impact_fx_count;
            }
            continue;
        }
        --fx->ttl_ticks;
        if (fx->ttl_ticks <= 0) {
            if (fx->anchors_block && inside_grid(fx->anchor_tile_x, fx->anchor_tile_y)) {
                mark_static_cell_dirty(gs, fx->anchor_tile_x, fx->anchor_tile_y);
            }
            fx->active = false;
            gs->active_impact_fx_mask &= (uint16_t)~bit;
            if (gs->active_impact_fx_count > 0) {
                --gs->active_impact_fx_count;
            }
        }
    }
}
#else
static void spawn_score_popup(GameState *gs, int tile_x, int tile_y, int value) {
    static const int kPopupOffsetPx[][2] = {
        {0, 0},
        {-4, -2},
        {4, -2},
        {0, -5},
        {-8, -4},
        {8, -4},
        {-12, -6},
        {12, -6},
    };
    int slot = -1;
    int weakest_ttl = 32767;
    int weakest_index = 0;
    GamePixelFp base_fp_x = 0;
    GamePixelFp base_fp_y = 0;
    int best_offset_idx = 0;
    int best_score = -1;
    int best_bias = 32767;
    uint16_t active_mask = active_score_popup_mask_for_game(gs);
    for (int i = 0; i < GAME_MAX_SCORE_POPUPS; ++i) {
        if ((active_mask & slot_bit(i)) == 0u || !gs->score_popups[i].active) {
            slot = i;
            break;
        }
        if (gs->score_popups[i].ttl_ticks < weakest_ttl) {
            weakest_ttl = gs->score_popups[i].ttl_ticks;
            weakest_index = i;
        }
    }
    if (slot < 0) {
        slot = weakest_index;
    }

    base_fp_x = tile_to_fp(tile_x) + (GamePixelFp)((GAME_TILE_SIZE / 2) * FP_ONE);
    base_fp_y = tile_to_fp(tile_y) + (GamePixelFp)((GAME_TILE_SIZE / 2) * FP_ONE);

    for (int candidate = 0; candidate < (int)(sizeof(kPopupOffsetPx) / sizeof(kPopupOffsetPx[0])); ++candidate) {
        const GamePixelFp cand_fp_x = base_fp_x + ((GamePixelFp)kPopupOffsetPx[candidate][0] * (GamePixelFp)FP_ONE);
        const GamePixelFp cand_fp_y = base_fp_y + ((GamePixelFp)kPopupOffsetPx[candidate][1] * (GamePixelFp)FP_ONE);
        GamePixelFp min_dist = 9999999;
        bool found_other = false;
        const int bias = abs(kPopupOffsetPx[candidate][0]) + abs(kPopupOffsetPx[candidate][1]);
        uint16_t popup_mask = active_mask;

        for (int i = 0; popup_mask != 0u; ++i, popup_mask >>= 1) {
            const ScorePopup *other = &gs->score_popups[i];
            GamePixelFp dist = 0;
            if ((popup_mask & 1u) == 0u || i == slot || !other->active || other->ttl_ticks <= 0) {
                continue;
            }
            dist = abs_fp(cand_fp_x - other->pixel_fp_x) + abs_fp(cand_fp_y - other->pixel_fp_y);
            if (dist < min_dist) {
                min_dist = dist;
            }
            found_other = true;
        }

        if (!found_other) {
            min_dist = 9999999;
        }
        if (min_dist > best_score || (min_dist == best_score && bias < best_bias)) {
            best_score = min_dist;
            best_offset_idx = candidate;
            best_bias = bias;
        }
    }

    ScorePopup *popup = &gs->score_popups[slot];
    if (!popup->active) {
        ++gs->active_score_popup_count;
    }
    gs->active_score_popup_mask |= slot_bit(slot);
    popup->pixel_fp_x = base_fp_x + ((GamePixelFp)kPopupOffsetPx[best_offset_idx][0] * (GamePixelFp)FP_ONE);
    popup->pixel_fp_y = base_fp_y + ((GamePixelFp)kPopupOffsetPx[best_offset_idx][1] * (GamePixelFp)FP_ONE);
    popup->value = value;
    popup->ttl_ticks = SCORE_POPUP_TICKS;
    popup->active = true;
}

static ImpactFx *spawn_impact_fx(GameState *gs, GamePixelFp pixel_fp_x, GamePixelFp pixel_fp_y, ImpactFxStyle style, int ttl_ticks) {
    int slot = -1;
    int weakest_ttl = 32767;
    int weakest_index = 0;
    uint16_t active_mask = active_impact_fx_mask_for_game(gs);
    for (int i = 0; i < GAME_MAX_IMPACT_FX; ++i) {
        if ((active_mask & slot_bit(i)) == 0u || !gs->impact_fx[i].active) {
            slot = i;
            break;
        }
        if (gs->impact_fx[i].ttl_ticks < weakest_ttl) {
            weakest_ttl = gs->impact_fx[i].ttl_ticks;
            weakest_index = i;
        }
    }
    if (slot < 0) {
        slot = weakest_index;
    }

    ImpactFx *fx = &gs->impact_fx[slot];
    if (fx->active &&
        fx->anchors_block &&
        inside_grid(fx->anchor_tile_x, fx->anchor_tile_y)) {
        mark_static_cell_dirty(gs, fx->anchor_tile_x, fx->anchor_tile_y);
    }
    if (!fx->active) {
        ++gs->active_impact_fx_count;
    }
    gs->active_impact_fx_mask |= slot_bit(slot);
    fx->pixel_fp_x = pixel_fp_x;
    fx->pixel_fp_y = pixel_fp_y;
    fx->style = (int)style;
    fx->base_ttl_ticks = ttl_ticks;
    fx->anchor_tile_x = -1;
    fx->anchor_tile_y = -1;
    fx->direction = DIR_NONE;
    fx->ttl_ticks = ttl_ticks;
    fx->anchors_block = false;
    fx->active = true;
    return fx;
}

static void spawn_impact_fx_on_tile(GameState *gs, int tile_x, int tile_y, ImpactFxStyle style, int ttl_ticks) {
    (void)spawn_impact_fx(
        gs,
        tile_to_fp(tile_x) + (GamePixelFp)((GAME_TILE_SIZE / 2) * FP_ONE),
        tile_to_fp(tile_y) + (GamePixelFp)((GAME_TILE_SIZE / 2) * FP_ONE),
        style,
        ttl_ticks);
}

static void update_score_popups(GameState *gs) {
    uint16_t mask = active_score_popup_mask_for_game(gs);
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    if (mask == 0u) {
        return;
    }
#endif
    for (int i = 0; mask != 0u && i < GAME_MAX_SCORE_POPUPS; ++i, mask >>= 1) {
        uint16_t bit = slot_bit(i);
        ScorePopup *popup = &gs->score_popups[i];
        if ((mask & 1u) == 0u) {
            continue;
        }
        if (!popup->active) {
            gs->active_score_popup_mask &= (uint16_t)~bit;
            if (gs->active_score_popup_count > 0) {
                --gs->active_score_popup_count;
            }
            continue;
        }
        --popup->ttl_ticks;
        if (popup->ttl_ticks <= 0) {
            popup->active = false;
            gs->active_score_popup_mask &= (uint16_t)~bit;
            if (gs->active_score_popup_count > 0) {
                --gs->active_score_popup_count;
            }
            continue;
        }
        popup->pixel_fp_y -= SCORE_POPUP_RISE_FP;
    }
}

static void update_impact_fx(GameState *gs) {
    uint16_t mask = active_impact_fx_mask_for_game(gs);
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    if (mask == 0u) {
        return;
    }
#endif
    for (int i = 0; mask != 0u && i < GAME_MAX_IMPACT_FX; ++i, mask >>= 1) {
        uint16_t bit = slot_bit(i);
        ImpactFx *fx = &gs->impact_fx[i];
        if ((mask & 1u) == 0u) {
            continue;
        }
        if (!fx->active) {
            gs->active_impact_fx_mask &= (uint16_t)~bit;
            if (gs->active_impact_fx_count > 0) {
                --gs->active_impact_fx_count;
            }
            continue;
        }
        --fx->ttl_ticks;
        if (fx->ttl_ticks <= 0) {
            if (fx->anchors_block &&
                inside_grid(fx->anchor_tile_x, fx->anchor_tile_y)) {
                mark_static_cell_dirty(gs, fx->anchor_tile_x, fx->anchor_tile_y);
            }
            fx->active = false;
            gs->active_impact_fx_mask &= (uint16_t)~bit;
            if (gs->active_impact_fx_count > 0) {
                --gs->active_impact_fx_count;
            }
            continue;
        }
        fx->pixel_fp_y -= IMPACT_FX_RISE_FP;
    }
}
#endif

static int timer_danger_threshold_ticks_for_stage(const GameState *gs) {
    if (gs->stage_modifier == STAGE_MOD_FROST_SIREN) {
        return TIMER_DANGER_THRESHOLD_TICKS + STAGE_MOD_FROST_SIREN_DANGER_THRESHOLD_BONUS_TICKS;
    }
    return TIMER_DANGER_THRESHOLD_TICKS;
}

static int timer_danger_pulse_interval_ticks_for_stage(const GameState *gs) {
    if (gs->stage_modifier == STAGE_MOD_FROST_SIREN) {
        return STAGE_MOD_FROST_SIREN_PULSE_INTERVAL_TICKS;
    }
    return GAME_FIXED_TICK_HZ;
}

static int timer_danger_pulse_ticks_for_stage(const GameState *gs) {
    if (gs->stage_modifier == STAGE_MOD_FROST_SIREN) {
        return STAGE_MOD_FROST_SIREN_PULSE_TICKS;
    }
    return TIMER_DANGER_PULSE_TICKS;
}

static void update_timer_danger_state(GameState *gs) {
    const int danger_threshold_ticks = timer_danger_threshold_ticks_for_stage(gs);
    const int pulse_interval_ticks = clampi(timer_danger_pulse_interval_ticks_for_stage(gs), 1, GAME_FIXED_TICK_HZ);
    const int pulse_ticks = clampi(timer_danger_pulse_ticks_for_stage(gs), 1, 2 * GAME_FIXED_TICK_HZ);

    if (gs->round_time_ticks <= 0) {
        gs->timer_danger_active = false;
        gs->timer_danger_pulse_ticks = 0;
        return;
    }

    if (gs->round_time_ticks <= danger_threshold_ticks) {
        if (!gs->timer_danger_active) {
            gs->timer_danger_active = true;
            gs->timer_danger_pulse_ticks = pulse_ticks;
            emit_event(gs, GAME_EVENT_TIMER_DANGER_PULSE);
        } else if ((gs->round_time_ticks % pulse_interval_ticks) == 0) {
            gs->timer_danger_pulse_ticks = pulse_ticks;
            emit_event(gs, GAME_EVENT_TIMER_DANGER_PULSE);
        } else if (gs->timer_danger_pulse_ticks > 0) {
            --gs->timer_danger_pulse_ticks;
        }
        return;
    }

    gs->timer_danger_active = false;
    gs->timer_danger_pulse_ticks = 0;
}

static void setup_round_config(GameState *gs, int round_index) {
    const int round = (round_index < 1) ? 1 : round_index;
    int round_seconds = 120 - (round - 1) * 2;
    if (round <= 3) {
        /* Skill-first onboarding: give opening rounds a small time cushion without flattening later pressure. */
        round_seconds += (4 - round) * 2;
    }
    if (round >= 4) {
        /* Mid/late rounds tighten a bit faster to keep pressure rising. */
        round_seconds -= (round - 3);
    }

    {
        int enemy_count = 1 + round / 2;
        if (round >= 5) {
            enemy_count += 1 + ((round - 5) / 5);
        }
        gs->round_config.enemy_count = clampi(enemy_count, 1, GAME_MAX_ENEMIES);
    }
    {
        int wanderers = 0;
        int hunters = 0;
        int ghosts = 0;
        int special_slots;
        if (round >= 4) {
            wanderers = 1 + (round - 4) / 4;
        }
        if (round >= 8) {
            hunters = 1 + (round - 8) / 5;
        }
        if (round >= 12) {
            ghosts = 1 + (round - 12) / 6;
        }
        special_slots = (gs->round_config.enemy_count > 1) ? (gs->round_config.enemy_count - 1) : 0;
        gs->round_config.enemy_ghost_count = clampi(
            ghosts,
            0,
            min_int(1, special_slots));
        gs->round_config.enemy_hunter_count = clampi(
            hunters,
            0,
            special_slots - gs->round_config.enemy_ghost_count);
        gs->round_config.enemy_wanderer_count = clampi(
            wanderers,
            0,
            special_slots - gs->round_config.enemy_ghost_count - gs->round_config.enemy_hunter_count);
    }
    gs->round_config.enemy_speed_fp = FP_ONE + (((round - 1) / 3) * (FP_ONE / 8));
    gs->round_config.enemy_speed_fp = clampi(gs->round_config.enemy_speed_fp, FP_ONE, (9 * FP_ONE) / 4);
    gs->round_config.aggression_percent = clampi(20 + round * 3, 20, 60);
    gs->round_config.round_time_ticks = clampi(
        round_seconds * GAME_FIXED_TICK_HZ,
        60 * GAME_FIXED_TICK_HZ,
        126 * GAME_FIXED_TICK_HZ);
    gs->round_config.enemy_speed_fp = clampi(
        gs->round_config.enemy_speed_fp - gs->run_enemy_speed_penalty_fp,
        FP_ONE,
        5 * FP_ONE);
    gs->round_config.round_time_ticks = clampi(
        gs->round_config.round_time_ticks + gs->run_round_time_bonus_ticks,
        25 * GAME_FIXED_TICK_HZ,
        150 * GAME_FIXED_TICK_HZ);
}

static void reset_player_runtime_state(GameState *gs, int tile_x, int tile_y) {
    gs->player.tile_x = tile_x;
    gs->player.tile_y = tile_y;
    gs->player.pixel_fp_x = tile_to_fp(tile_x);
    gs->player.pixel_fp_y = tile_to_fp(tile_y);
    gs->player.facing = DIR_RIGHT;
    gs->player.move_dir = DIR_NONE;
    gs->player.buffered_dir = DIR_NONE;
    gs->player.buffered_dir_ticks = 0;
    gs->player.state = PLAYER_IDLE;
    gs->player.move_remaining_fp = 0;
    gs->player.push_timer = 0;
    gs->player.bump_timer = 0;
    gs->player.respawn_invuln_ticks = RESPAWN_INVULN_TICKS;
    gs->player.anim_frame = 0;
    gs->player.alive = true;
    gs->player.mine_drop_queued = false;
}

static bool moving_block_overlaps_tile(const MovingBlock *mb, int tile_x, int tile_y) {
    const GamePixelFp tile_left = tile_to_fp(tile_x);
    const GamePixelFp tile_top = tile_to_fp(tile_y);
    const GamePixelFp tile_right = tile_left + (GamePixelFp)TILE_FP;
    const GamePixelFp tile_bottom = tile_top + (GamePixelFp)TILE_FP;
    const GamePixelFp block_left = mb->pixel_fp_x;
    const GamePixelFp block_top = mb->pixel_fp_y;
    const GamePixelFp block_right = block_left + (GamePixelFp)TILE_FP;
    const GamePixelFp block_bottom = block_top + (GamePixelFp)TILE_FP;

    return block_left < tile_right &&
           block_right > tile_left &&
           block_top < tile_bottom &&
           block_bottom > tile_top;
}

static bool respawn_tile_has_moving_block_overlap(const GameState *gs, int x, int y) {
    uint16_t mask = active_moving_block_mask_for_game(gs);
    for (int i = 0; mask != 0u && i < GAME_MAX_MOVING_BLOCKS; ++i, mask >>= 1) {
        const MovingBlock *mb = &gs->moving_blocks[i];
        if ((mask & 1u) == 0u || !mb->active) {
            continue;
        }
        if (mb->tile_x == x && mb->tile_y == y) {
            return true;
        }
        if (moving_block_overlaps_tile(mb, x, y)) {
            return true;
        }
    }
    return false;
}

static bool respawn_candidate_base_open(const GameState *gs, int x, int y) {
    if (!inside_grid(x, y)) {
        return false;
    }
    if (terrain_blocks(gs->terrain[y][x])) {
        return false;
    }
    if (gs->blocks[y][x] != BLOCK_NONE) {
        return false;
    }
    if (gs->mines[y][x]) {
        return false;
    }
    if (respawn_tile_has_moving_block_overlap(gs, x, y)) {
        return false;
    }
    return true;
}

static bool respawn_player_would_touch_enemy(const Enemy *enemy, int tile_x, int tile_y) {
    const GamePixelFp player_x = tile_to_fp(tile_x);
    const GamePixelFp player_y = tile_to_fp(tile_y);
    const GamePixelFp player_left = player_x + (GamePixelFp)PLAYER_HITBOX_LEFT_FP;
    const GamePixelFp player_top = player_y + (GamePixelFp)PLAYER_HITBOX_TOP_FP;
    const GamePixelFp player_right = player_x + (GamePixelFp)PLAYER_HITBOX_RIGHT_FP;
    const GamePixelFp player_bottom = player_y + (GamePixelFp)PLAYER_HITBOX_BOTTOM_FP;
    const GamePixelFp enemy_left = enemy->pixel_fp_x + (GamePixelFp)ENEMY_HITBOX_LEFT_FP;
    const GamePixelFp enemy_top = enemy->pixel_fp_y + (GamePixelFp)ENEMY_HITBOX_TOP_FP;
    const GamePixelFp enemy_right = enemy->pixel_fp_x + (GamePixelFp)ENEMY_HITBOX_RIGHT_FP;
    const GamePixelFp enemy_bottom = enemy->pixel_fp_y + (GamePixelFp)ENEMY_HITBOX_BOTTOM_FP;

    return player_left < enemy_right &&
           player_right > enemy_left &&
           player_top < enemy_bottom &&
           player_bottom > enemy_top;
}

static int respawn_min_enemy_distance(const GameState *gs, int x, int y) {
    int min_dist = 9999;
    uint16_t enemy_mask = live_enemy_mask_for_game(gs);
    for (int i = 0; enemy_mask != 0u && i < gs->enemy_count; ++i, enemy_mask >>= 1) {
        const Enemy *enemy = &gs->enemies[i];
        int dist;
        if ((enemy_mask & 1u) == 0u || !enemy->alive) {
            continue;
        }
        if (respawn_player_would_touch_enemy(enemy, x, y)) {
            return -1;
        }
        dist = abs(enemy->tile_x - x) + abs(enemy->tile_y - y);
        if (dist < min_dist) {
            min_dist = dist;
        }
    }
    return min_dist;
}

static int respawn_floor_exit_count(const GameState *gs, int x, int y) {
    int exits = 0;
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = x + kDirDx[d];
        const int ny = y + kDirDy[d];
        if (!respawn_candidate_base_open(gs, nx, ny)) {
            continue;
        }
        if (tile_has_enemy(gs, nx, ny, -1)) {
            continue;
        }
        ++exits;
    }
    return exits;
}

static bool tile_is_safe_respawn(const GameState *gs, int x, int y, int min_enemy_dist, int min_exits) {
    int enemy_dist;
    if (!respawn_candidate_base_open(gs, x, y)) {
        return false;
    }
    if (tile_has_enemy(gs, x, y, -1)) {
        return false;
    }
    enemy_dist = respawn_min_enemy_distance(gs, x, y);
    if (enemy_dist >= 0 && enemy_dist < min_enemy_dist) {
        return false;
    }
    if (respawn_floor_exit_count(gs, x, y) < min_exits) {
        return false;
    }
    return true;
}

static int respawn_tile_score(const GameState *gs, int x, int y, int sx, int sy) {
    const int exits = respawn_floor_exit_count(gs, x, y);
    int enemy_dist = respawn_min_enemy_distance(gs, x, y);
    if (enemy_dist < 0) {
        return -30000;
    }
    if (enemy_dist > 12) {
        enemy_dist = 12;
    }
    return enemy_dist * 100 + exits * 20 - (abs(x - sx) + abs(y - sy));
}

static void find_safe_respawn_tile(const GameState *gs, int *out_x, int *out_y) {
    enum {
        RESPAWN_IDEAL_ENEMY_DISTANCE = 4,
        RESPAWN_HARD_ENEMY_DISTANCE = 2
    };
    const int sx = gs->player_spawn_x;
    const int sy = gs->player_spawn_y;
    int best_x = sx;
    int best_y = sy;
    int best_score = -30000;

    for (int min_dist = RESPAWN_IDEAL_ENEMY_DISTANCE; min_dist >= RESPAWN_HARD_ENEMY_DISTANCE; --min_dist) {
        for (int min_exits = 2; min_exits >= 0; --min_exits) {
            for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
                for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
                    int score;
                    if (!tile_is_safe_respawn(gs, x, y, min_dist, min_exits)) {
                        continue;
                    }
                    score = respawn_tile_score(gs, x, y, sx, sy);
                    if (score > best_score) {
                        best_score = score;
                        best_x = x;
                        best_y = y;
                    }
                }
            }
            if (best_score > -30000) {
                *out_x = best_x;
                *out_y = best_y;
                return;
            }
        }
    }

    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            int score;
            if (!respawn_candidate_base_open(gs, x, y) || tile_has_enemy(gs, x, y, -1)) {
                continue;
            }
            score = respawn_tile_score(gs, x, y, sx, sy);
            if (score > best_score) {
                best_score = score;
                best_x = x;
                best_y = y;
            }
        }
    }

    if (best_score > -30000) {
        *out_x = best_x;
        *out_y = best_y;
        return;
    }

    *out_x = sx;
    *out_y = sy;
}

static void kill_player_internal(GameState *gs, bool ignore_respawn_invuln, bool from_timeout) {
    if (!gs->player.alive) {
        return;
    }
    if (gs->phase != GAME_PHASE_PLAYING) {
        return;
    }
    if (round_clear_pending_active(gs)) {
        return;
    }
    if (!ignore_respawn_invuln && gs->player.respawn_invuln_ticks > 0) {
        return;
    }

    gs->player.alive = false;
    gs->player.state = PLAYER_DYING;
    gs->player.move_remaining_fp = 0;
    gs->phase = GAME_PHASE_PLAYER_DYING;
    gs->phase_timer_ticks = PLAYER_DYING_TICKS;
    gs->player_death_from_timeout = from_timeout;
    emit_event(gs, GAME_EVENT_PLAYER_DIED);
}

static void kill_player(GameState *gs) {
    kill_player_internal(gs, false, false);
}

static void kill_player_for_timeout(GameState *gs) {
    kill_player_internal(gs, true, true);
}

static ImpactFxStyle crush_style_for_block(BlockType type) {
    if (type == BLOCK_SPECIAL) {
        return IMPACT_FX_STYLE_SPECIAL_CRUSH;
    }
    if (type == BLOCK_CRACKED) {
        return IMPACT_FX_STYLE_CRACKED_CRUSH;
    }
    return IMPACT_FX_STYLE_CRUSH;
}

static ImpactFxStyle death_style_for_enemy_type(EnemyType type) {
    if (type == ENEMY_TYPE_GHOST) {
        return IMPACT_FX_STYLE_ENEMY_DEATH_D;
    }
    if (type == ENEMY_TYPE_HUNTER) {
        return IMPACT_FX_STYLE_ENEMY_DEATH_C;
    }
    if (type == ENEMY_TYPE_WANDERER) {
        return IMPACT_FX_STYLE_ENEMY_DEATH_B;
    }
    return IMPACT_FX_STYLE_ENEMY_DEATH_A;
}

static ImpactFxStyle stop_style_for_block(BlockType type) {
    if (type == BLOCK_SPECIAL) {
        return IMPACT_FX_STYLE_SPECIAL_STOP;
    }
    if (type == BLOCK_CRACKED) {
        return IMPACT_FX_STYLE_CRACKED_STOP;
    }
    return IMPACT_FX_STYLE_STOP;
}

static bool enemy_overlaps_tile(const Enemy *enemy, int tile_x, int tile_y) {
    const GamePixelFp tile_left = tile_to_fp(tile_x);
    const GamePixelFp tile_top = tile_to_fp(tile_y);
    const GamePixelFp tile_right = tile_left + (GamePixelFp)TILE_FP;
    const GamePixelFp tile_bottom = tile_top + (GamePixelFp)TILE_FP;

    const GamePixelFp enemy_left = enemy->pixel_fp_x;
    const GamePixelFp enemy_top = enemy->pixel_fp_y;
    const GamePixelFp enemy_right = enemy_left + (GamePixelFp)TILE_FP;
    const GamePixelFp enemy_bottom = enemy_top + (GamePixelFp)TILE_FP;

    return enemy_left < tile_right &&
           enemy_right > tile_left &&
           enemy_top < tile_bottom &&
           enemy_bottom > tile_top;
}

static bool player_overlaps_tile(const Player *player, int tile_x, int tile_y) {
    const GamePixelFp tile_left = tile_to_fp(tile_x);
    const GamePixelFp tile_top = tile_to_fp(tile_y);
    const GamePixelFp tile_right = tile_left + (GamePixelFp)TILE_FP;
    const GamePixelFp tile_bottom = tile_top + (GamePixelFp)TILE_FP;

    const GamePixelFp player_left = player->pixel_fp_x;
    const GamePixelFp player_top = player->pixel_fp_y;
    const GamePixelFp player_right = player_left + (GamePixelFp)TILE_FP;
    const GamePixelFp player_bottom = player_top + (GamePixelFp)TILE_FP;

    return player_left < tile_right &&
           player_right > tile_left &&
           player_top < tile_bottom &&
           player_bottom > tile_top;
}

static bool player_touches_enemy(const Player *player, const Enemy *enemy) {
    const GamePixelFp player_left = player->pixel_fp_x + (GamePixelFp)PLAYER_HITBOX_LEFT_FP;
    const GamePixelFp player_top = player->pixel_fp_y + (GamePixelFp)PLAYER_HITBOX_TOP_FP;
    const GamePixelFp player_right = player->pixel_fp_x + (GamePixelFp)PLAYER_HITBOX_RIGHT_FP;
    const GamePixelFp player_bottom = player->pixel_fp_y + (GamePixelFp)PLAYER_HITBOX_BOTTOM_FP;
    const GamePixelFp enemy_left = enemy->pixel_fp_x + (GamePixelFp)ENEMY_HITBOX_LEFT_FP;
    const GamePixelFp enemy_top = enemy->pixel_fp_y + (GamePixelFp)ENEMY_HITBOX_TOP_FP;
    const GamePixelFp enemy_right = enemy->pixel_fp_x + (GamePixelFp)ENEMY_HITBOX_RIGHT_FP;
    const GamePixelFp enemy_bottom = enemy->pixel_fp_y + (GamePixelFp)ENEMY_HITBOX_BOTTOM_FP;

    return player_left < enemy_right &&
           player_right > enemy_left &&
           player_top < enemy_bottom &&
           player_bottom > enemy_top;
}

static void crush_enemies_on_tile(
    GameState *gs,
    int x,
    int y,
    int *combo_count,
    BlockType source_block_type,
    bool reduced_score) {
    uint16_t enemy_mask = live_enemy_mask_for_game(gs);
    for (int i = 0; enemy_mask != 0u && i < gs->enemy_count; ++i, enemy_mask >>= 1) {
        Enemy *enemy = &gs->enemies[i];
        if ((enemy_mask & 1u) == 0u || !enemy->alive) {
            continue;
        }
        if (!enemy_is_killable(enemy)) {
            continue;
        }
        if ((enemy->tile_x != x || enemy->tile_y != y) && !enemy_overlaps_tile(enemy, x, y)) {
            continue;
        }
        mark_enemy_defeated(gs, i);
        enemy->state = ENEMY_CRUSHED;
        spawn_impact_fx_on_tile(
            gs,
            x,
            y,
            death_style_for_enemy_type(enemy->type),
            ENEMY_DEATH_FX_TICKS);
        ++(*combo_count);
        {
            const int combo_score = reduced_score
                                        ? score_for_mine_combo(*combo_count)
                                        : score_for_combo(*combo_count);
            const int awarded_score = award_score(gs, combo_score);
            if (awarded_score > 0) {
                spawn_score_popup(gs, x, y, awarded_score);
            }
            spawn_impact_fx_on_tile(
                gs,
                x,
                y,
                crush_style_for_block(source_block_type),
                IMPACT_FX_TICKS + 2);
        }
        on_enemy_crushed(gs);
        if (!reduced_score && *combo_count >= 2) {
            emit_event(gs, GAME_EVENT_COMBO);
        }
    }
}

static int moving_block_speed_for_stage(const GameState *gs) {
    if (gs->stage_modifier == STAGE_MOD_TURBO_SLIDE) {
        return BLOCK_SPEED_FP + STAGE_MOD_TURBO_SLIDE_SPEED_BONUS_FP;
    }
    return BLOCK_SPEED_FP;
}

static int moving_block_startup_ticks_for_stage(const GameState *gs) {
    if (gs->stage_modifier == STAGE_MOD_TURBO_SLIDE) {
        return STAGE_MOD_TURBO_SLIDE_STARTUP_TICKS;
    }
    return PUSH_ANTICIPATION_TICKS;
}

static bool spawn_moving_block(GameState *gs, int tile_x, int tile_y, Direction dir, BlockType type) {
    uint16_t active_mask = active_moving_block_mask_for_game(gs);
    for (int i = 0; i < GAME_MAX_MOVING_BLOCKS; ++i) {
        if ((active_mask & slot_bit(i)) != 0u && gs->moving_blocks[i].active) {
            continue;
        }
        MovingBlock *mb = &gs->moving_blocks[i];
        mb->tile_x = tile_x;
        mb->tile_y = tile_y;
        mb->pixel_fp_x = tile_to_fp(tile_x);
        mb->pixel_fp_y = tile_to_fp(tile_y);
        mb->direction = dir;
        mb->speed_fp = moving_block_speed_for_stage(gs);
        mb->intra_fp = 0;
        mb->combo_count = 0;
        mb->startup_ticks = moving_block_startup_ticks_for_stage(gs);
        mb->type = type;
        mb->active = true;
        gs->active_moving_block_mask |= slot_bit(i);
        return true;
    }
    return false;
}

static bool can_start_slide(const GameState *gs, int block_x, int block_y, Direction dir) {
    const int nx = block_x + kDirDx[dir];
    const int ny = block_y + kDirDy[dir];
    if (!inside_grid(nx, ny)) {
        return false;
    }
    if (slide_target_blocked(gs, nx, ny, -1)) {
        return false;
    }
    return true;
}

#if defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE) && defined(ICEPANIC_AMIGA_OCS_SKIP_MINES)
static void clear_active_mine_list(GameState *gs) {
    gs->active_mine_count = 0;
}

static void consume_queued_mine_drop(GameState *gs) {
    gs->player.mine_drop_queued = false;
}

static bool try_drop_mine(GameState *gs) {
    gs->player.mine_drop_queued = false;
    return false;
}

static void trigger_mine_explosion(GameState *gs, int start_x, int start_y) {
    if (gs && inside_grid(start_x, start_y)) {
        gs->mines[start_y][start_x] = false;
        gs->mine_fuse_ticks[start_y][start_x] = 0;
        mark_static_cell_dirty(gs, start_x, start_y);
    }
}

static void update_mine_fuses(GameState *gs) {
    if (gs && gs->active_mine_count < 0) {
        gs->active_mine_count = 0;
    }
}
#else
static void clear_active_mine_list(GameState *gs) {
    gs->active_mine_count = 0;
    memset(gs->active_mine_x, 0, sizeof(gs->active_mine_x));
    memset(gs->active_mine_y, 0, sizeof(gs->active_mine_y));
}

static int active_mine_slot(const GameState *gs, int x, int y) {
    int count = gs->active_mine_count;
    if (count > GAME_MAX_ACTIVE_MINES) {
        count = GAME_MAX_ACTIVE_MINES;
    }
    for (int i = 0; i < count; ++i) {
        if ((int)gs->active_mine_x[i] == x && (int)gs->active_mine_y[i] == y) {
            return i;
        }
    }
    return -1;
}

static bool register_active_mine(GameState *gs, int x, int y) {
    int count = gs->active_mine_count;
    if (active_mine_slot(gs, x, y) >= 0) {
        return true;
    }
    if (count < 0 || count >= GAME_MAX_ACTIVE_MINES) {
        return false;
    }
    gs->active_mine_x[count] = (uint8_t)x;
    gs->active_mine_y[count] = (uint8_t)y;
    gs->active_mine_count = count + 1;
    return true;
}

static bool unregister_active_mine(GameState *gs, int x, int y) {
    int slot;
    int last;
    if (gs->active_mine_count > GAME_MAX_ACTIVE_MINES) {
        gs->active_mine_count = GAME_MAX_ACTIVE_MINES;
    }
    slot = active_mine_slot(gs, x, y);
    if (slot < 0) {
        return false;
    }
    last = gs->active_mine_count - 1;
    if (slot != last) {
        gs->active_mine_x[slot] = gs->active_mine_x[last];
        gs->active_mine_y[slot] = gs->active_mine_y[last];
    }
    gs->active_mine_x[last] = 0;
    gs->active_mine_y[last] = 0;
    gs->active_mine_count = last;
    return true;
}

static bool mine_already_queued(const int *queue_x, const int *queue_y, int count, int x, int y) {
    for (int i = 0; i < count; ++i) {
        if (queue_x[i] == x && queue_y[i] == y) {
            return true;
        }
    }
    return false;
}

static bool try_drop_mine(GameState *gs) {
    const int x = gs->player.tile_x;
    const int y = gs->player.tile_y;

    if (!gs->player.alive || gs->phase != GAME_PHASE_PLAYING) {
        return false;
    }
    if (gs->run_mine_stock <= 0) {
        return false;
    }
    if (!inside_grid(x, y) || terrain_blocks(gs->terrain[y][x])) {
        return false;
    }
    if (gs->blocks[y][x] != BLOCK_NONE) {
        return false;
    }
    if (gs->items[y][x] != ITEM_NONE) {
        return false;
    }
    if (gs->mines[y][x]) {
        return false;
    }
    if (tile_has_moving_block(gs, x, y, -1)) {
        return false;
    }

    gs->mines[y][x] = true;
    gs->mine_fuse_ticks[y][x] = (uint8_t)MINE_FUSE_TICKS;
    if (!register_active_mine(gs, x, y)) {
        gs->mines[y][x] = false;
        gs->mine_fuse_ticks[y][x] = 0;
        return false;
    }
    mark_static_cell_dirty(gs, x, y);
    gs->run_mine_stock = clampi(gs->run_mine_stock - 1, 0, gs->run_mine_capacity);
    spawn_impact_fx_on_tile(gs, x, y, IMPACT_FX_STYLE_SPECIAL_STOP, IMPACT_FX_TICKS);
    emit_event(gs, GAME_EVENT_MINE_PLACED);
    emit_event(gs, GAME_EVENT_BLOCK_IMPACT);
    return true;
}

static void consume_queued_mine_drop(GameState *gs) {
    Player *player = &gs->player;
    if (!player->mine_drop_queued) {
        return;
    }
    player->mine_drop_queued = false;
    (void)try_drop_mine(gs);
}

static void trigger_mine_explosion(GameState *gs, int start_x, int start_y) {
    enum {
        MINE_QUEUE_MAX = GAME_MAX_ACTIVE_MINES
    };
    static const int blast_dx[5] = {0, 0, 0, -1, 1};
    static const int blast_dy[5] = {0, -1, 1, 0, 0};
    ICEPANIC_SCRATCH int queue_x[MINE_QUEUE_MAX];
    ICEPANIC_SCRATCH int queue_y[MINE_QUEUE_MAX];

    int head = 0;
    int tail = 0;
    int combo_count = 0;
    bool any_special_change = false;
    bool exploded = false;

    if (!inside_grid(start_x, start_y)) {
        return;
    }
    if (!gs->mines[start_y][start_x]) {
        return;
    }

    queue_x[tail] = start_x;
    queue_y[tail] = start_y;
    ++tail;

    while (head < tail) {
        const int mx = queue_x[head];
        const int my = queue_y[head];
        ++head;

        if (!inside_grid(mx, my) || !gs->mines[my][mx]) {
            continue;
        }
        gs->mines[my][mx] = false;
        gs->mine_fuse_ticks[my][mx] = 0;
        unregister_active_mine(gs, mx, my);
        mark_static_cell_dirty(gs, mx, my);
        exploded = true;

        for (int i = 0; i < 5; ++i) {
            const int bx = mx + blast_dx[i];
            const int by = my + blast_dy[i];
            if (!inside_grid(bx, by)) {
                continue;
            }
            if (terrain_blocks(gs->terrain[by][bx])) {
                continue;
            }

            spawn_impact_fx_on_tile(gs, bx, by, IMPACT_FX_STYLE_MINE_BLAST, MINE_EXPLOSION_TTL_TICKS);

            if (gs->player.alive && player_overlaps_tile(&gs->player, bx, by)) {
                kill_player(gs);
            }

            if (gs->items[by][bx] != ITEM_NONE) {
                gs->items[by][bx] = ITEM_NONE;
                unregister_active_item(gs, bx, by);
                mark_static_cell_dirty(gs, bx, by);
            }
            if (gs->blocks[by][bx] == BLOCK_ICE ||
                gs->blocks[by][bx] == BLOCK_SPECIAL ||
                gs->blocks[by][bx] == BLOCK_CRACKED) {
                if (gs->blocks[by][bx] == BLOCK_SPECIAL) {
                    any_special_change = true;
                }
                gs->blocks[by][bx] = BLOCK_NONE;
                mark_static_cell_dirty(gs, bx, by);
            }

            crush_enemies_on_tile(gs, bx, by, &combo_count, BLOCK_SPECIAL, true);

            if (gs->mines[by][bx] && tail < MINE_QUEUE_MAX &&
                !mine_already_queued(queue_x, queue_y, tail, bx, by)) {
                queue_x[tail] = bx;
                queue_y[tail] = by;
                ++tail;
            }
        }
    }

    if (!has_any_items(gs)) {
        gs->bonus_item_timer_ticks = 0;
    }
    if (exploded) {
        emit_event(gs, GAME_EVENT_MINE_EXPLODED);
    }
    if (any_special_change) {
        check_special_alignment(gs);
    }
    emit_event(gs, GAME_EVENT_BLOCK_IMPACT);
}

#if defined(ICEPANIC_AMIGA_SMALL_STACK)
static int amiga_mine_visual_frame_for_fuse(int fuse_ticks);
#endif

static void update_mine_fuses(GameState *gs) {
    int i = 0;

    if (gs->active_mine_count <= 0) {
        return;
    }
    if (gs->active_mine_count > GAME_MAX_ACTIVE_MINES) {
        gs->active_mine_count = GAME_MAX_ACTIVE_MINES;
    }

    while (i < gs->active_mine_count) {
        const int x = (int)gs->active_mine_x[i];
        const int y = (int)gs->active_mine_y[i];
        int fuse;

        if (!inside_grid(x, y) || !gs->mines[y][x]) {
            if (i != gs->active_mine_count - 1) {
                gs->active_mine_x[i] = gs->active_mine_x[gs->active_mine_count - 1];
                gs->active_mine_y[i] = gs->active_mine_y[gs->active_mine_count - 1];
            }
            --gs->active_mine_count;
            gs->active_mine_x[gs->active_mine_count] = 0;
            gs->active_mine_y[gs->active_mine_count] = 0;
            continue;
        }

        fuse = gs->mine_fuse_ticks[y][x];
        if (fuse <= 0) {
            fuse = MINE_FUSE_TICKS;
        }
        {
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
            const int old_fuse = fuse;
#endif
            --fuse;
            gs->mine_fuse_ticks[y][x] = (uint8_t)clampi(fuse, 0, 255);
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
            if (amiga_mine_visual_frame_for_fuse(old_fuse) != amiga_mine_visual_frame_for_fuse(fuse)) {
                mark_static_cell_dirty(gs, x, y);
            }
#else
            mark_static_cell_dirty(gs, x, y);
#endif
        }

        if (fuse <= 0) {
            trigger_mine_explosion(gs, x, y);
            continue;
        }
        ++i;
    }
}

#if defined(ICEPANIC_AMIGA_SMALL_STACK)
static int amiga_mine_visual_frame_for_fuse(int fuse_ticks) {
    if (fuse_ticks <= 0) {
        return -1;
    }
    if (fuse_ticks <= GAME_FIXED_TICK_HZ) {
        return (fuse_ticks & 1) ? 1 : 2;
    }
    if (fuse_ticks <= 2 * GAME_FIXED_TICK_HZ) {
        return ((fuse_ticks & 2) == 0) ? 1 : 0;
    }
    return ((fuse_ticks & 7) == 0) ? 1 : 0;
}
#endif
#endif

static void player_bump(Player *player, Direction dir) {
    player->facing = dir;
    player->bump_timer = 5;
}

static bool try_start_player_action(GameState *gs, Direction dir) {
    Player *player = &gs->player;
    const int nx = player->tile_x + kDirDx[dir];
    const int ny = player->tile_y + kDirDy[dir];
    player->facing = dir;

    if (!inside_grid(nx, ny) || terrain_blocks(gs->terrain[ny][nx])) {
        player_bump(player, dir);
        return false;
    }

    if (tile_has_moving_block(gs, nx, ny, -1)) {
        player_bump(player, dir);
        return false;
    }

    if (gs->blocks[ny][nx] != BLOCK_NONE) {
        if (!can_start_slide(gs, nx, ny, dir)) {
            player_bump(player, dir);
            return false;
        }
        const BlockType pushed_type = gs->blocks[ny][nx];
        if (!spawn_moving_block(gs, nx, ny, dir, pushed_type)) {
            player_bump(player, dir);
            return false;
        }
        gs->blocks[ny][nx] = BLOCK_NONE;
        mark_static_cell_dirty(gs, nx, ny);
        player->state = PLAYER_PUSHING;
        player->move_dir = DIR_NONE;
        player->move_remaining_fp = 0;
        player->push_timer = PUSH_ANTICIPATION_TICKS;
        emit_event(gs, GAME_EVENT_BLOCK_PUSH);
        return true;
    }

    player->state = PLAYER_WALKING;
    player->move_dir = dir;
    player->move_remaining_fp = TILE_FP;
    return true;
}

static void update_player_intent(GameState *gs, const InputState *in) {
    Player *player = &gs->player;
    gs->mine_tap_ticks = 0;
    gs->mine_tap_dir = DIR_NONE;

    if (!player->alive) {
        return;
    }

    if (in->fire_released) {
        if (player->state == PLAYER_WALKING || player->state == PLAYER_PUSHING) {
            player->mine_drop_queued = true;
        } else if (player->state != PLAYER_DYING) {
            (void)try_drop_mine(gs);
        }
    }

    if (in->newest_press != DIR_NONE) {
        player->buffered_dir = in->newest_press;
        player->buffered_dir_ticks = PLAYER_DIR_BUFFER_TICKS;
    } else if (direction_is_held(in, player->buffered_dir)) {
        player->buffered_dir = choose_held_player_direction(player, in);
        player->buffered_dir_ticks = PLAYER_DIR_BUFFER_TICKS;
    } else if (in->up || in->down || in->left || in->right) {
        player->buffered_dir = choose_held_player_direction(player, in);
        player->buffered_dir_ticks = PLAYER_DIR_BUFFER_TICKS;
    } else if (player->buffered_dir_ticks > 0) {
        --player->buffered_dir_ticks;
    } else {
        player->buffered_dir = DIR_NONE;
    }

    if (player->state == PLAYER_WALKING || player->state == PLAYER_PUSHING || player->state == PLAYER_DYING) {
        return;
    }

    if (player->buffered_dir == DIR_NONE) {
        return;
    }

    if (!direction_is_held(in, player->buffered_dir) &&
        in->newest_press == DIR_NONE &&
        player->buffered_dir_ticks <= 0) {
        return;
    }

    if (try_start_player_action(gs, player->buffered_dir)) {
        player->buffered_dir = DIR_NONE;
        player->buffered_dir_ticks = 0;
    } else if (!direction_is_held(in, player->buffered_dir)) {
        player->buffered_dir = DIR_NONE;
        player->buffered_dir_ticks = 0;
    }
}

static void update_player_motion(GameState *gs) {
    Player *player = &gs->player;
    if (player->bump_timer > 0) {
        --player->bump_timer;
    }
    if (player->respawn_invuln_ticks > 0) {
        --player->respawn_invuln_ticks;
    }
    if (!player->alive) {
        player->mine_drop_queued = false;
        return;
    }

    if (player->state == PLAYER_PUSHING) {
        if (player->push_timer > 0) {
            --player->push_timer;
        }
        if (player->push_timer <= 0) {
            player->state = PLAYER_IDLE;
            consume_queued_mine_drop(gs);
        }
        return;
    }

    if (player->state != PLAYER_WALKING || player->move_remaining_fp <= 0 || player->move_dir == DIR_NONE) {
        consume_queued_mine_drop(gs);
        return;
    }

    const int step = min_int(PLAYER_SPEED_FP, player->move_remaining_fp);
    player->move_remaining_fp -= step;
    player->pixel_fp_x += (GamePixelFp)kDirDx[player->move_dir] * (GamePixelFp)step;
    player->pixel_fp_y += (GamePixelFp)kDirDy[player->move_dir] * (GamePixelFp)step;

    if (player->move_remaining_fp == 0) {
        player->tile_x += kDirDx[player->move_dir];
        player->tile_y += kDirDy[player->move_dir];
        player->pixel_fp_x = tile_to_fp(player->tile_x);
        player->pixel_fp_y = tile_to_fp(player->tile_y);
        player->state = PLAYER_IDLE;
        player->anim_frame ^= 1;
        consume_queued_mine_drop(gs);
    }
}

static void update_moving_blocks(GameState *gs) {
    bool any_block_stopped = false;
    uint16_t block_mask = active_moving_block_mask_for_game(gs);

    for (int i = 0; block_mask != 0u && i < GAME_MAX_MOVING_BLOCKS; ++i, block_mask >>= 1) {
        uint16_t bit = slot_bit(i);
        MovingBlock *mb = &gs->moving_blocks[i];
        if ((block_mask & 1u) == 0u) {
            continue;
        }
        if (!mb->active) {
            gs->active_moving_block_mask &= (uint16_t)~bit;
            continue;
        }

        if (mb->startup_ticks > 0) {
            --mb->startup_ticks;
            continue;
        }

        mb->intra_fp += mb->speed_fp;
        while (mb->active) {
            const int nx = mb->tile_x + kDirDx[mb->direction];
            const int ny = mb->tile_y + kDirDy[mb->direction];

            if (slide_target_blocked(gs, nx, ny, i)) {
                mb->intra_fp = 0;
                gs->blocks[mb->tile_y][mb->tile_x] = mb->type;
                mark_static_cell_dirty(gs, mb->tile_x, mb->tile_y);
                ImpactFx *stop_fx = spawn_impact_fx(
                    gs,
                    tile_to_fp(mb->tile_x) + (GamePixelFp)((GAME_TILE_SIZE / 2) * FP_ONE) + (GamePixelFp)kDirDx[mb->direction] * (GamePixelFp)((GAME_TILE_SIZE / 2) * FP_ONE),
                    tile_to_fp(mb->tile_y) + (GamePixelFp)((GAME_TILE_SIZE / 2) * FP_ONE) + (GamePixelFp)kDirDy[mb->direction] * (GamePixelFp)((GAME_TILE_SIZE / 2) * FP_ONE),
                    stop_style_for_block(mb->type),
                    IMPACT_FX_TICKS);
                stop_fx->anchor_tile_x = mb->tile_x;
                stop_fx->anchor_tile_y = mb->tile_y;
                stop_fx->direction = mb->direction;
                stop_fx->anchors_block = true;
                emit_event(gs, GAME_EVENT_BLOCK_IMPACT);
                mb->active = false;
                gs->active_moving_block_mask &= (uint16_t)~bit;
                any_block_stopped = true;
                break;
            }
            if (mb->intra_fp < TILE_FP) {
                break;
            }
            mb->intra_fp -= TILE_FP;

            if (gs->items[ny][nx] != ITEM_NONE) {
                gs->items[ny][nx] = ITEM_NONE;
                unregister_active_item(gs, nx, ny);
                mark_static_cell_dirty(gs, nx, ny);
            }

            crush_enemies_on_tile(gs, nx, ny, &mb->combo_count, mb->type, false);
            if (!round_clear_pending_active(gs) &&
                gs->player.alive && gs->player.tile_x == nx && gs->player.tile_y == ny) {
                kill_player(gs);
            }

            mb->tile_x = nx;
            mb->tile_y = ny;
        }

        if (mb->active) {
            mb->pixel_fp_x = tile_to_fp(mb->tile_x) + (GamePixelFp)kDirDx[mb->direction] * (GamePixelFp)mb->intra_fp;
            mb->pixel_fp_y = tile_to_fp(mb->tile_y) + (GamePixelFp)kDirDy[mb->direction] * (GamePixelFp)mb->intra_fp;
        } else {
            mb->pixel_fp_x = tile_to_fp(mb->tile_x);
            mb->pixel_fp_y = tile_to_fp(mb->tile_y);
        }
    }

    if (any_block_stopped) {
        check_special_alignment(gs);
    }
}

static int enemy_decision_pause_ticks(
    const GameState *gs,
    int enemy_idx,
    const uint32_t occupancy[GAME_GRID_HEIGHT],
    const uint32_t moving_block_occupancy[GAME_GRID_HEIGHT]) {
#if defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE)
    if (gs->round > 2 ||
        gs->stage_modifier == STAGE_MOD_SWARM ||
        gs->stage_modifier == STAGE_MOD_FROST_SIREN) {
        return 0;
    }
#endif
    const Enemy *enemy = &gs->enemies[enemy_idx];
    Direction valid[4];
    int valid_count = 0;
    int non_reverse_count = 0;
    bool forward_open = false;
    const Direction reverse = opposite_direction(enemy->direction);

    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = enemy->tile_x + kDirDx[d];
        const int ny = enemy->tile_y + kDirDy[d];
        if (!tile_walkable_for_enemy_with_occupancy(gs, occupancy, moving_block_occupancy, enemy->type, nx, ny)) {
            continue;
        }
        valid[valid_count++] = d;
    }

    if (valid_count <= 1) {
        return 0;
    }

    for (int i = 0; i < valid_count; ++i) {
        if (valid[i] == enemy->direction) {
            forward_open = true;
        }
        if (valid[i] != reverse) {
            ++non_reverse_count;
        }
    }

    if (non_reverse_count <= 1 && forward_open) {
        return 0;
    }

    if (gs->stage_modifier == STAGE_MOD_SWARM || gs->stage_modifier == STAGE_MOD_FROST_SIREN) {
        return 0;
    }
    if (gs->round <= 2) {
        return 1;
    }
    return 0;
}

static void enemy_step_movement(Enemy *enemy) {
    if (enemy->move_remaining_fp <= 0 || enemy->direction == DIR_NONE) {
        return;
    }
    const int step = min_int(enemy->speed_fp, enemy->move_remaining_fp);
    enemy->move_remaining_fp -= step;
    enemy->pixel_fp_x += (GamePixelFp)kDirDx[enemy->direction] * (GamePixelFp)step;
    enemy->pixel_fp_y += (GamePixelFp)kDirDy[enemy->direction] * (GamePixelFp)step;

    if (enemy->move_remaining_fp == 0) {
        enemy->tile_x += kDirDx[enemy->direction];
        enemy->tile_y += kDirDy[enemy->direction];
        enemy->pixel_fp_x = tile_to_fp(enemy->tile_x);
        enemy->pixel_fp_y = tile_to_fp(enemy->tile_y);
        enemy->anim_frame ^= 1;
    }
}

static bool direction_in_list(const Direction *dirs, int count, Direction dir) {
    if (dir == DIR_NONE) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        if (dirs[i] == dir) {
            return true;
        }
    }
    return false;
}

static Direction pick_direction_from_list(GameState *gs, const Direction *dirs, int count) {
    if (count <= 0) {
        return DIR_NONE;
    }
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    return dirs[rng_pick_small(gs, count)];
#else
    return dirs[rng_next(gs) % (uint32_t)count];
#endif
}

static Direction choose_hunter_direction(GameState *gs, const Enemy *enemy, const Direction *valid, int valid_count) {
    Direction best[4];
    int best_count = 0;
    int best_distance = 9999;

    if (valid_count <= 1) {
        return valid_count == 1 ? valid[0] : DIR_NONE;
    }
    if (!gs->player.alive) {
        return pick_direction_from_list(gs, valid, valid_count);
    }

    for (int i = 0; i < valid_count; ++i) {
        const Direction d = valid[i];
        const int nx = enemy->tile_x + kDirDx[d];
        const int ny = enemy->tile_y + kDirDy[d];
        const int dist = abs_int_fast(gs->player.tile_x - nx) + abs_int_fast(gs->player.tile_y - ny);
        if (dist < best_distance) {
            best_distance = dist;
            best[0] = d;
            best_count = 1;
        } else if (dist == best_distance && best_count < 4) {
            best[best_count++] = d;
        }
    }

    if (direction_in_list(best, best_count, enemy->direction)) {
        return enemy->direction;
    }
    return pick_direction_from_list(gs, best, best_count);
}

static Direction choose_ghost_direction(GameState *gs, const Enemy *enemy, const Direction *valid, int valid_count) {
    return choose_hunter_direction(gs, enemy, valid, valid_count);
}

static Direction choose_chaser_style_direction(GameState *gs, const Enemy *enemy, Direction *valid, int valid_count, int chase_percent) {
    const Direction reverse = opposite_direction(enemy->direction);

    if (valid_count <= 0) {
        return DIR_NONE;
    }

    if (valid_count > 1 && reverse != DIR_NONE) {
        int compact = 0;
        for (int i = 0; i < valid_count; ++i) {
            if (valid[i] == reverse) {
                continue;
            }
            valid[compact++] = valid[i];
        }
        if (compact > 0) {
            valid_count = compact;
        }
    }

    chase_percent = clampi(chase_percent, 0, 100);

#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    {
        const int chase_roll = (int)(rng_next(gs) & 0xFFu);
        const int chase_threshold = (chase_percent >= 100) ? 256 : ((chase_percent <= 0) ? 0 : ((chase_percent * 41) >> 4));
        if (gs->player.alive && chase_roll < chase_threshold) {
#else
    {
        const int chase_roll = (int)(rng_next(gs) % 100u);
        if (gs->player.alive && chase_roll < chase_percent) {
#endif
            int best_distance = 9999;
            Direction best_dir = valid[0];
            for (int i = 0; i < valid_count; ++i) {
                const Direction d = valid[i];
                const int nx = enemy->tile_x + kDirDx[d];
                const int ny = enemy->tile_y + kDirDy[d];
                const int dist = abs_int_fast(gs->player.tile_x - nx) + abs_int_fast(gs->player.tile_y - ny);
                if (dist < best_distance) {
                    best_distance = dist;
                    best_dir = d;
                }
            }
            return best_dir;
        }
    }

#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    return valid[rng_pick_small(gs, valid_count)];
#else
    return valid[rng_next(gs) % (uint32_t)valid_count];
#endif
}

static Direction choose_enemy_direction(
    GameState *gs,
    int enemy_idx,
    const uint32_t occupancy[GAME_GRID_HEIGHT],
    const uint32_t moving_block_occupancy[GAME_GRID_HEIGHT]) {
    Enemy *enemy = &gs->enemies[enemy_idx];
    Direction valid[4];
    int valid_count = 0;
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = enemy->tile_x + kDirDx[d];
        const int ny = enemy->tile_y + kDirDy[d];
        if (!tile_walkable_for_enemy_with_occupancy(gs, occupancy, moving_block_occupancy, enemy->type, nx, ny)) {
            continue;
        }
        valid[valid_count++] = d;
    }

    if (valid_count == 0) {
        return DIR_NONE;
    }

    if (enemy->type == ENEMY_TYPE_GHOST) {
        return choose_ghost_direction(gs, enemy, valid, valid_count);
    }

    if (enemy->type == ENEMY_TYPE_HUNTER) {
        return choose_hunter_direction(gs, enemy, valid, valid_count);
    }

    if (enemy->type == ENEMY_TYPE_WANDERER) {
        return choose_chaser_style_direction(
            gs,
            enemy,
            valid,
            valid_count,
            gs->round_config.aggression_percent + 35);
    }

    return choose_chaser_style_direction(gs, enemy, valid, valid_count, gs->round_config.aggression_percent);
}

static void update_enemies(GameState *gs) {
    ICEPANIC_SCRATCH uint32_t enemy_occupancy[GAME_GRID_HEIGHT];
    ICEPANIC_SCRATCH uint32_t moving_block_occupancy[GAME_GRID_HEIGHT];
    const uint32_t *moving_block_occupancy_ptr = NULL;
    uint16_t enemy_mask;
    bool enemy_occupancy_ready = false;
    bool moving_block_occupancy_ready = false;
#define ENSURE_ENEMY_OCCUPANCY() \
    do { \
        if (!enemy_occupancy_ready) { \
            build_enemy_occupancy(gs, enemy_occupancy); \
            enemy_occupancy_ready = true; \
        } \
    } while (0)
#define ENSURE_MOVING_BLOCK_OCCUPANCY() \
    do { \
        if (!moving_block_occupancy_ready) { \
            if (active_moving_block_mask_for_game(gs) != 0u) { \
                build_moving_block_occupancy(gs, moving_block_occupancy); \
                moving_block_occupancy_ptr = moving_block_occupancy; \
            } else { \
                moving_block_occupancy_ptr = NULL; \
            } \
            moving_block_occupancy_ready = true; \
        } \
    } while (0)
    if (!any_live_enemy_active(gs)) {
        return;
    }
    enemy_mask = live_enemy_mask_for_game(gs);

    for (int i = 0; enemy_mask != 0u && i < gs->enemy_count; ++i, enemy_mask >>= 1) {
        Enemy *enemy = &gs->enemies[i];
        if ((enemy_mask & 1u) == 0u || !enemy->alive) {
            if (!enemy->alive) {
                clear_enemy_alive_bit(gs, i);
            }
            continue;
        }

        if (enemy->spawn_ticks > 0) {
            --enemy->spawn_ticks;
            if (enemy->spawn_ticks == 0) {
                enemy->state = ENEMY_ROAMING;
                enemy->decision_cooldown_ticks = 0;
                if (enemy_can_trigger_mine(enemy) && gs->mines[enemy->tile_y][enemy->tile_x]) {
                    trigger_mine_explosion(gs, enemy->tile_x, enemy->tile_y);
                    enemy_occupancy_ready = false;
                    moving_block_occupancy_ready = false;
                    moving_block_occupancy_ptr = NULL;
                }
            }
            continue;
        }

        if (enemy->move_remaining_fp > 0) {
            const int old_tile_x = enemy->tile_x;
            const int old_tile_y = enemy->tile_y;
            enemy_step_movement(enemy);
            if (old_tile_x != enemy->tile_x || old_tile_y != enemy->tile_y) {
                if (enemy_occupancy_ready) {
                    enemy_occupancy_move(
                        enemy_occupancy,
                        old_tile_x,
                        old_tile_y,
                        enemy->tile_x,
                        enemy->tile_y);
                }
            }
            if (enemy->move_remaining_fp == 0) {
                if (enemy_can_trigger_mine(enemy) && gs->mines[enemy->tile_y][enemy->tile_x]) {
                    trigger_mine_explosion(gs, enemy->tile_x, enemy->tile_y);
                    enemy_occupancy_ready = false;
                    moving_block_occupancy_ready = false;
                    moving_block_occupancy_ptr = NULL;
                    if (!enemy->alive) {
                        continue;
                    }
                }
                ENSURE_ENEMY_OCCUPANCY();
                ENSURE_MOVING_BLOCK_OCCUPANCY();
                enemy->decision_cooldown_ticks = enemy_decision_pause_ticks(gs, i, enemy_occupancy, moving_block_occupancy_ptr);
            }
            continue;
        }

        if (enemy->decision_cooldown_ticks > 0) {
            --enemy->decision_cooldown_ticks;
            continue;
        }

        ENSURE_ENEMY_OCCUPANCY();
        ENSURE_MOVING_BLOCK_OCCUPANCY();
        enemy->direction = choose_enemy_direction(gs, i, enemy_occupancy, moving_block_occupancy_ptr);
        if (enemy->direction == DIR_NONE) {
            enemy->decision_cooldown_ticks = 0;
            continue;
        }
        enemy->move_remaining_fp = TILE_FP;
        enemy_step_movement(enemy);
    }
#undef ENSURE_ENEMY_OCCUPANCY
#undef ENSURE_MOVING_BLOCK_OCCUPANCY
}

static void update_bonus_item_timer(GameState *gs) {
    if (gs->bonus_item_timer_ticks > 0) {
        const int old_timer = gs->bonus_item_timer_ticks;
        --gs->bonus_item_timer_ticks;
        if (gs->bonus_item_timer_ticks <= 0) {
            clear_bonus_items(gs);
        } else if (old_timer <= BONUS_ITEM_FLASH_TICKS &&
                   bonus_item_visible_for_timer(old_timer) != bonus_item_visible_for_timer(gs->bonus_item_timer_ticks)) {
            mark_bonus_items_dirty(gs);
        }
    }
    if (gs->special_alignment_flash_ticks > 0) {
        --gs->special_alignment_flash_ticks;
    }
}

static void resolve_player_enemy_collision(GameState *gs) {
    if (!gs->player.alive || gs->player.respawn_invuln_ticks > 0) {
        return;
    }
    if (round_clear_pending_active(gs)) {
        return;
    }
    if (!any_live_enemy_active(gs)) {
        return;
    }

    {
    uint16_t enemy_mask = live_enemy_mask_for_game(gs);
    for (int i = 0; enemy_mask != 0u && i < gs->enemy_count; ++i, enemy_mask >>= 1) {
        const Enemy *enemy = &gs->enemies[i];
        if ((enemy_mask & 1u) == 0u || !enemy->alive) {
            continue;
        }
        if (abs_int_fast(enemy->tile_x - gs->player.tile_x) > 1 ||
            abs_int_fast(enemy->tile_y - gs->player.tile_y) > 1) {
            continue;
        }

        if (player_touches_enemy(&gs->player, enemy)) {
            kill_player(gs);
            return;
        }
    }
    }
}

static void resolve_item_collection(GameState *gs) {
    if (!gs->player.alive) {
        return;
    }
    ItemType *item = &gs->items[gs->player.tile_y][gs->player.tile_x];
    if (*item == ITEM_NONE) {
        return;
    }

    {
        const ItemType collected = *item;
        (void)award_score(gs, score_for_item(collected));
        if (collected == ITEM_MYSTERY) {
            add_run_shards(gs, SHARDS_PER_MYSTERY_ITEM);
        } else {
            add_run_shards(gs, SHARDS_PER_ITEM);
        }
    }
    emit_event(gs, GAME_EVENT_ITEM_COLLECT);
    *item = ITEM_NONE;
    unregister_active_item(gs, gs->player.tile_x, gs->player.tile_y);
    mark_static_cell_dirty(gs, gs->player.tile_x, gs->player.tile_y);
    if (!has_any_items(gs)) {
        gs->bonus_item_timer_ticks = 0;
    }
}

static void check_round_clear(GameState *gs) {
    if (gs->phase != GAME_PHASE_PLAYING) {
        return;
    }
    if (live_enemy_count_fast(gs) != 0) {
        gs->round_clear_pending_ticks = 0;
        return;
    }
    if (gs->round_clear_pending_ticks <= 0) {
        gs->round_clear_pending_ticks = ROUND_CLEAR_PENDING_TICKS;
        return;
    }
    --gs->round_clear_pending_ticks;
    if (gs->round_clear_pending_ticks > 0) {
        return;
    }

    gs->phase = GAME_PHASE_ROUND_CLEAR;
    gs->phase_timer_ticks = ROUND_CLEAR_TICKS;
    gs->player.state = PLAYER_VICTORY;
    gs->round_clear_bonus_score = award_score(gs, round_clear_time_bonus_base_score(gs->round_time_ticks));
    add_run_shards(gs, SHARDS_PER_ROUND_CLEAR);
    reset_fire_confirm_gate(gs);
    emit_event(gs, GAME_EVENT_ROUND_CLEAR);
}

#if defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE) && defined(ICEPANIC_AMIGA_OCS_SKIP_PERK_UI)
static void update_perk_choice(GameState *gs, const InputState *in, bool start_released, bool fire_confirm_released) {
    game_start_round(gs, gs->pending_round_after_choice);
}

static void update_meta_upgrade_choice(GameState *gs, const InputState *in, bool start_released, bool fire_confirm_released) {
    game_start_round(gs, 1);
}
#else
static void update_perk_choice(GameState *gs, const InputState *in, bool start_released, bool fire_confirm_released) {
    if (gs->perk_choice_count <= 0) {
        game_start_round(gs, gs->pending_round_after_choice);
        return;
    }

    if (gs->phase_timer_ticks > 0) {
        --gs->phase_timer_ticks;
    }

    if (in->newest_press == DIR_LEFT || in->newest_press == DIR_UP) {
        gs->perk_choice_selected -= 1;
        if (gs->perk_choice_selected < 0) {
            gs->perk_choice_selected = gs->perk_choice_count - 1;
        }
    } else if (in->newest_press == DIR_RIGHT || in->newest_press == DIR_DOWN) {
        gs->perk_choice_selected += 1;
        if (gs->perk_choice_selected >= gs->perk_choice_count) {
            gs->perk_choice_selected = 0;
        }
    }

    if (gs->phase_timer_ticks > 0) {
        start_released = false;
        fire_confirm_released = false;
    }

    if (!start_released && !fire_confirm_released) {
        return;
    }

    if (gs->perk_choice_selected < 0 || gs->perk_choice_selected >= gs->perk_choice_count) {
        gs->perk_choice_selected = 0;
    }
    apply_perk_choice(gs, gs->perk_choices[gs->perk_choice_selected]);
    game_start_round(gs, gs->pending_round_after_choice);
}

static void update_meta_upgrade_choice(GameState *gs, const InputState *in, bool start_released, bool fire_confirm_released) {
    if (gs->meta_choice_count <= 0 || gs->meta_shards == 0u) {
        game_start_round(gs, 1);
        return;
    }
#if !defined(ICEPANIC_AMIGA_SMALL_STACK)
    (void)fire_confirm_released;
#endif

    if (in->newest_press == DIR_LEFT || in->newest_press == DIR_UP) {
        gs->meta_choice_selected -= 1;
        if (gs->meta_choice_selected < 0) {
            gs->meta_choice_selected = gs->meta_choice_count - 1;
        }
    } else if (in->newest_press == DIR_RIGHT || in->newest_press == DIR_DOWN) {
        gs->meta_choice_selected += 1;
        if (gs->meta_choice_selected >= gs->meta_choice_count) {
            gs->meta_choice_selected = 0;
        }
    }

#if defined(ICEPANIC_AMIGA_SMALL_STACK)
    if (!start_released && !fire_confirm_released) {
        return;
    }
#else
    if (!start_released) {
        return;
    }
#endif

    if (gs->meta_choice_selected < 0 || gs->meta_choice_selected >= gs->meta_choice_count) {
        gs->meta_choice_selected = 0;
    }

    bool spent = false;
    {
        const int idx = gs->meta_choice_selected;
        const MetaUpgradeType upgrade = gs->meta_choices[idx];
        const int cost = gs->meta_choice_costs[idx];
        if (upgrade != META_UPGRADE_NONE && cost > 0 && gs->meta_shards >= (uint32_t)cost) {
            if (apply_meta_upgrade_choice(gs, upgrade)) {
                gs->meta_shards -= (uint32_t)cost;
                gs->meta_progress_points = add_u32_saturating(gs->meta_progress_points, (uint32_t)cost);
                gs->meta_unlock_tier = meta_unlock_tier_from_progress(gs->meta_progress_points);
                spent = true;
            }
        }
    }
    game_start_round(gs, 1);
    if (spent) {
        emit_event(gs, GAME_EVENT_META_SPENT);
    }
}
#endif

static void handle_player_death_transition(GameState *gs) {
    const bool timeout_death = gs->player_death_from_timeout;
    gs->player_death_from_timeout = false;

    --gs->lives;
    if (gs->lives <= 0) {
        gs->phase = GAME_PHASE_GAME_OVER;
        gs->phase_timer_ticks = GAME_OVER_TICKS;
        reset_fire_confirm_gate(gs);
        emit_event(gs, GAME_EVENT_GAME_OVER);
        return;
    }
    emit_event(gs, GAME_EVENT_LIFE_LOST);

    if (timeout_death) {
        const int round = gs->round;
        game_start_round(gs, round);
        gs->phase_timer_ticks = RESPAWN_INTRO_TICKS;
        gs->start_title_pending = false;
        emit_event(gs, GAME_EVENT_LIFE_LOST);
        return;
    }

    int spawn_x = gs->player_spawn_x;
    int spawn_y = gs->player_spawn_y;
    find_safe_respawn_tile(gs, &spawn_x, &spawn_y);
    reset_player_runtime_state(gs, spawn_x, spawn_y);
    gs->phase = GAME_PHASE_ROUND_INTRO;
    gs->phase_timer_ticks = RESPAWN_INTRO_TICKS;
    gs->start_title_pending = false;
    reset_fire_confirm_gate(gs);
}

static bool bank_run_shards(GameState *gs) {
    if (gs->run_shards <= 0) {
        return false;
    }
    gs->meta_shards += (uint32_t)gs->run_shards;
    gs->run_shards = 0;
    return true;
}

static bool runtime_block_is_pushable(BlockType block) {
    return block == BLOCK_ICE || block == BLOCK_SPECIAL || block == BLOCK_CRACKED;
}

static int opening_safe_enemy_distance_for_round(int round) {
    return (round <= 3) ? 8 : ((round <= 7) ? 7 : 6);
}

enum {
    OPENING_HARD_ENEMY_DISTANCE = 6
};

static bool enemy_spawn_marker_contains(
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count,
    int x,
    int y,
    int ignore_idx) {
    for (int i = 0; i < enemy_spawn_count; ++i) {
        if (i == ignore_idx) {
            continue;
        }
        if (enemy_spawns[i][0] == x && enemy_spawns[i][1] == y) {
            return true;
        }
    }
    return false;
}

static int count_runtime_walkable_neighbors(
    const GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count,
    int x,
    int y) {
    int exits = 0;
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = x + kDirDx[d];
        const int ny = y + kDirDy[d];
        if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        if (terrain_blocks(gs->terrain[ny][nx])) {
            continue;
        }
        if (gs->blocks[ny][nx] != BLOCK_NONE) {
            continue;
        }
        if (enemy_spawn_marker_contains(enemy_spawns, enemy_spawn_count, nx, ny, -1)) {
            continue;
        }
        ++exits;
    }
    return exits;
}

static int nearest_enemy_spawn_distance(
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count,
    int x,
    int y) {
    int nearest = GAME_GRID_WIDTH + GAME_GRID_HEIGHT;
    for (int i = 0; i < enemy_spawn_count; ++i) {
        const int d = abs(enemy_spawns[i][0] - x) + abs(enemy_spawns[i][1] - y);
        if (d < nearest) {
            nearest = d;
        }
    }
    return nearest;
}

static bool runtime_tile_can_host_enemy_spawn(
    const GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count,
    int marker_idx,
    int x,
    int y,
    int spawn_x,
    int spawn_y,
    int min_spawn_enemy_dist,
    int min_spawn_separation) {
    if (x <= 0 || y <= 0 || x >= GAME_GRID_WIDTH - 1 || y >= GAME_GRID_HEIGHT - 1) {
        return false;
    }
    if (terrain_blocks(gs->terrain[y][x])) {
        return false;
    }
    if (gs->blocks[y][x] != BLOCK_NONE) {
        return false;
    }
    if (abs(x - spawn_x) + abs(y - spawn_y) < min_spawn_enemy_dist) {
        return false;
    }
    if (enemy_spawn_marker_contains(enemy_spawns, enemy_spawn_count, x, y, marker_idx)) {
        return false;
    }
    if (count_runtime_walkable_neighbors(gs, enemy_spawns, enemy_spawn_count, x, y) < 2) {
        return false;
    }

    for (int i = 0; i < enemy_spawn_count; ++i) {
        if (i == marker_idx) {
            continue;
        }
        if (abs(enemy_spawns[i][0] - x) + abs(enemy_spawns[i][1] - y) < min_spawn_separation) {
            return false;
        }
    }
    return true;
}

static bool find_runtime_enemy_spawn_relocation(
    const GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count,
    int marker_idx,
    int spawn_x,
    int spawn_y,
    int min_spawn_enemy_dist,
    int min_spawn_separation,
    int *out_x,
    int *out_y) {
    int best_score = -32767;
    int best_x = -1;
    int best_y = -1;

    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            int nearest_other = GAME_GRID_WIDTH + GAME_GRID_HEIGHT;
            int openness = 0;
            int score = 0;

            if (!runtime_tile_can_host_enemy_spawn(
                    gs,
                    enemy_spawns,
                    enemy_spawn_count,
                    marker_idx,
                    x,
                    y,
                    spawn_x,
                    spawn_y,
                    min_spawn_enemy_dist,
                    min_spawn_separation)) {
                continue;
            }

            for (int i = 0; i < enemy_spawn_count; ++i) {
                if (i == marker_idx) {
                    continue;
                }
                {
                    const int d = abs(enemy_spawns[i][0] - x) + abs(enemy_spawns[i][1] - y);
                    if (d < nearest_other) {
                        nearest_other = d;
                    }
                }
            }
            if (enemy_spawn_count <= 1) {
                nearest_other = 6;
            }
            openness = count_runtime_walkable_neighbors(gs, enemy_spawns, enemy_spawn_count, x, y);
            score = (abs(x - spawn_x) + abs(y - spawn_y)) * 16 + nearest_other * 8 + openness * 2;
            if (score > best_score) {
                best_score = score;
                best_x = x;
                best_y = y;
            }
        }
    }

    if (best_x < 0 || best_y < 0) {
        return false;
    }
    *out_x = best_x;
    *out_y = best_y;
    return true;
}

static void ensure_spawn_runtime_floor_exits(
    GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count,
    int min_exits) {
    const int spawn_x = gs->player_spawn_x;
    const int spawn_y = gs->player_spawn_y;

    if (min_exits < 1) {
        min_exits = 1;
    }

    if (count_runtime_walkable_neighbors(gs, enemy_spawns, enemy_spawn_count, spawn_x, spawn_y) >= min_exits) {
        return;
    }

    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = spawn_x + kDirDx[d];
        const int ny = spawn_y + kDirDy[d];
        if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        if (enemy_spawn_marker_contains(enemy_spawns, enemy_spawn_count, nx, ny, -1)) {
            continue;
        }
        gs->terrain[ny][nx] = TERRAIN_FLOOR;
        gs->blocks[ny][nx] = BLOCK_NONE;
        mark_static_cell_dirty(gs, nx, ny);

        if (count_runtime_walkable_neighbors(gs, enemy_spawns, enemy_spawn_count, spawn_x, spawn_y) >= min_exits) {
            return;
        }
    }
}

static bool runtime_has_push_option_from_tile(
    const GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count,
    int from_x,
    int from_y) {
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int bx = from_x + kDirDx[d];
        const int by = from_y + kDirDy[d];
        const int tx = bx + kDirDx[d];
        const int ty = by + kDirDy[d];
        if (bx <= 0 || by <= 0 || bx >= GAME_GRID_WIDTH - 1 || by >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        if (tx <= 0 || ty <= 0 || tx >= GAME_GRID_WIDTH - 1 || ty >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        if (!runtime_block_is_pushable(gs->blocks[by][bx])) {
            continue;
        }
        if (terrain_blocks(gs->terrain[ty][tx])) {
            continue;
        }
        if (gs->blocks[ty][tx] != BLOCK_NONE) {
            continue;
        }
        if (enemy_spawn_marker_contains(enemy_spawns, enemy_spawn_count, bx, by, -1)) {
            continue;
        }
        if (enemy_spawn_marker_contains(enemy_spawns, enemy_spawn_count, tx, ty, -1)) {
            continue;
        }
        return true;
    }
    return false;
}

static bool runtime_tile_open_for_spawn_setup(
    const GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count,
    int x,
    int y) {
    if (x <= 0 || y <= 0 || x >= GAME_GRID_WIDTH - 1 || y >= GAME_GRID_HEIGHT - 1) {
        return false;
    }
    if (terrain_blocks(gs->terrain[y][x])) {
        return false;
    }
    if (gs->blocks[y][x] != BLOCK_NONE) {
        return false;
    }
    if (enemy_spawn_marker_contains(enemy_spawns, enemy_spawn_count, x, y, -1)) {
        return false;
    }
    return true;
}

static bool find_runtime_player_spawn_relocation(
    const GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count,
    int min_enemy_dist,
    bool require_push_option,
    int *out_x,
    int *out_y) {
    int best_score = -32767;
    int best_x = -1;
    int best_y = -1;
    const int old_x = gs->player_spawn_x;
    const int old_y = gs->player_spawn_y;

    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            int nearest = 0;
            int exits = 0;
            int score = 0;
            bool has_push = false;

            if (!runtime_tile_open_for_spawn_setup(gs, enemy_spawns, enemy_spawn_count, x, y)) {
                continue;
            }
            nearest = nearest_enemy_spawn_distance(enemy_spawns, enemy_spawn_count, x, y);
            if (nearest < min_enemy_dist) {
                continue;
            }
            exits = count_runtime_walkable_neighbors(gs, enemy_spawns, enemy_spawn_count, x, y);
            if (exits < 2) {
                continue;
            }
            has_push = runtime_has_push_option_from_tile(gs, enemy_spawns, enemy_spawn_count, x, y);
            if (require_push_option && !has_push) {
                continue;
            }

            score = nearest * 32 + exits * 8 + (has_push ? 64 : 0) -
                    (abs(x - old_x) + abs(y - old_y));
            if (score > best_score) {
                best_score = score;
                best_x = x;
                best_y = y;
            }
        }
    }

    if (best_x < 0 || best_y < 0) {
        return false;
    }
    *out_x = best_x;
    *out_y = best_y;
    return true;
}

static void ensure_player_spawn_opening_position(
    GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count) {
    const int safe_dist = opening_safe_enemy_distance_for_round(gs->round);
    const int sx = gs->player_spawn_x;
    const int sy = gs->player_spawn_y;

    if (runtime_tile_open_for_spawn_setup(gs, enemy_spawns, enemy_spawn_count, sx, sy) &&
        nearest_enemy_spawn_distance(enemy_spawns, enemy_spawn_count, sx, sy) >= safe_dist) {
        return;
    }

    for (int min_dist = safe_dist; min_dist >= OPENING_HARD_ENEMY_DISTANCE; --min_dist) {
        int nx = 0;
        int ny = 0;
        if (find_runtime_player_spawn_relocation(
                gs,
                enemy_spawns,
                enemy_spawn_count,
                min_dist,
                true,
                &nx,
                &ny)) {
            gs->player_spawn_x = nx;
            gs->player_spawn_y = ny;
            return;
        }
    }

    for (int min_dist = safe_dist; min_dist >= OPENING_HARD_ENEMY_DISTANCE; --min_dist) {
        int nx = 0;
        int ny = 0;
        if (find_runtime_player_spawn_relocation(
                gs,
                enemy_spawns,
                enemy_spawn_count,
                min_dist,
                false,
                &nx,
                &ny)) {
            gs->player_spawn_x = nx;
            gs->player_spawn_y = ny;
            return;
        }
    }
}

static bool try_place_runtime_push_block_near_tile(
    GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count,
    int from_x,
    int from_y,
    bool require_two_spawn_exits) {
    const int spawn_x = gs->player_spawn_x;
    const int spawn_y = gs->player_spawn_y;
    const int spawn_exits_before =
        count_runtime_walkable_neighbors(gs, enemy_spawns, enemy_spawn_count, spawn_x, spawn_y);

    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int bx = from_x + kDirDx[d];
        const int by = from_y + kDirDy[d];
        const int tx = bx + kDirDx[d];
        const int ty = by + kDirDy[d];
        int spawn_exit_cost = 0;

        if (bx <= 0 || by <= 0 || bx >= GAME_GRID_WIDTH - 1 || by >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        if (tx <= 0 || ty <= 0 || tx >= GAME_GRID_WIDTH - 1 || ty >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        if (terrain_blocks(gs->terrain[by][bx]) || terrain_blocks(gs->terrain[ty][tx])) {
            continue;
        }
        if (gs->blocks[by][bx] != BLOCK_NONE) {
            continue;
        }
        if (gs->blocks[ty][tx] != BLOCK_NONE) {
            continue;
        }
        if (enemy_spawn_marker_contains(enemy_spawns, enemy_spawn_count, bx, by, -1) ||
            enemy_spawn_marker_contains(enemy_spawns, enemy_spawn_count, tx, ty, -1)) {
            continue;
        }

        if (from_x == spawn_x && from_y == spawn_y) {
            spawn_exit_cost = 1;
        }
        if (require_two_spawn_exits && (spawn_exits_before - spawn_exit_cost) < 2) {
            continue;
        }

        gs->blocks[by][bx] = BLOCK_ICE;
        mark_static_cell_dirty(gs, bx, by);
        return true;
    }
    return false;
}

static void ensure_spawn_has_nearby_push_option(
    GameState *gs,
    const int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count) {
    const int spawn_x = gs->player_spawn_x;
    const int spawn_y = gs->player_spawn_y;

    if (runtime_has_push_option_from_tile(gs, enemy_spawns, enemy_spawn_count, spawn_x, spawn_y)) {
        return;
    }
    if (try_place_runtime_push_block_near_tile(
            gs,
            enemy_spawns,
            enemy_spawn_count,
            spawn_x,
            spawn_y,
            true)) {
        return;
    }

    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = spawn_x + kDirDx[d];
        const int ny = spawn_y + kDirDy[d];
        if (!runtime_tile_open_for_spawn_setup(gs, enemy_spawns, enemy_spawn_count, nx, ny)) {
            continue;
        }
        if (runtime_has_push_option_from_tile(gs, enemy_spawns, enemy_spawn_count, nx, ny)) {
            return;
        }
    }

    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = spawn_x + kDirDx[d];
        const int ny = spawn_y + kDirDy[d];
        if (!runtime_tile_open_for_spawn_setup(gs, enemy_spawns, enemy_spawn_count, nx, ny)) {
            continue;
        }
        if (try_place_runtime_push_block_near_tile(
                gs,
                enemy_spawns,
                enemy_spawn_count,
                nx,
                ny,
                false)) {
            return;
        }
    }

    if (try_place_runtime_push_block_near_tile(
            gs,
            enemy_spawns,
            enemy_spawn_count,
            spawn_x,
            spawn_y,
            false)) {
        ensure_spawn_runtime_floor_exits(gs, enemy_spawns, enemy_spawn_count, 2);
    }
}

static int enforce_spawn_opening_protection(
    GameState *gs,
    int enemy_spawns[GAME_MAX_ENEMIES][2],
    int enemy_spawn_count) {
    ensure_player_spawn_opening_position(gs, enemy_spawns, enemy_spawn_count);
    ensure_spawn_runtime_floor_exits(gs, enemy_spawns, enemy_spawn_count, 2);
    ensure_spawn_has_nearby_push_option(gs, enemy_spawns, enemy_spawn_count);
    return enemy_spawn_count;
}

static void ensure_enemy_spawn_capacity(
    GameState *gs,
    int enemy_spawns[GAME_MAX_ENEMIES][2],
    int *enemy_spawn_count,
    int target_count) {
    const int base_min_dist = opening_safe_enemy_distance_for_round(gs->round);
    const int hard_min_dist = OPENING_HARD_ENEMY_DISTANCE;
    const int spawn_x = gs->player_spawn_x;
    const int spawn_y = gs->player_spawn_y;

    while (*enemy_spawn_count < target_count && *enemy_spawn_count < GAME_MAX_ENEMIES) {
        int nx = 0;
        int ny = 0;
        bool found = false;

        for (int min_dist = base_min_dist; min_dist >= hard_min_dist && !found; --min_dist) {
            for (int min_sep = 3; min_sep >= 1 && !found; --min_sep) {
                found = find_runtime_enemy_spawn_relocation(
                    gs,
                    enemy_spawns,
                    *enemy_spawn_count,
                    -1,
                    spawn_x,
                    spawn_y,
                    min_dist,
                    min_sep,
                    &nx,
                    &ny);
            }
        }

        if (!found) {
            break;
        }

        enemy_spawns[*enemy_spawn_count][0] = nx;
        enemy_spawns[*enemy_spawn_count][1] = ny;
        ++(*enemy_spawn_count);
    }
}

static void reset_to_new_game(GameState *gs) {
    const bool banked = bank_run_shards(gs);
    memset(gs->items, 0, sizeof(gs->items));
    clear_active_item_list(gs);
    memset(gs->mines, 0, sizeof(gs->mines));
    memset(gs->mine_fuse_ticks, 0, sizeof(gs->mine_fuse_ticks));
    clear_active_mine_list(gs);
    gs->score = 0;
    gs->lives = DEFAULT_LIVES;
    gs->round = 1;
    gs->round_clear_pending_ticks = 0;
    gs->run_score_mult_permille = RUN_SCORE_MULT_BASE_PERMILLE;
    gs->run_round_time_bonus_ticks = 0;
    gs->run_enemy_speed_penalty_fp = 0;
    gs->run_mine_capacity = 0;
    gs->run_mine_stock = 0;
    gs->perk_choice_count = 0;
    gs->perk_choice_selected = 0;
    gs->meta_choice_count = 0;
    gs->meta_choice_selected = 0;
    memset(gs->meta_choices, 0, sizeof(gs->meta_choices));
    memset(gs->meta_choice_costs, 0, sizeof(gs->meta_choice_costs));
    gs->pending_round_after_choice = 0;
    gs->mine_tap_dir = DIR_NONE;
    gs->mine_tap_ticks = 0;
    gs->stage_modifier = STAGE_MOD_NONE;
    gs->stage_modifier_flash_ticks = 0;
    gs->debug_forced_stage_modifier = STAGE_MOD_NONE;
    gs->debug_force_stage_modifier_pending = false;
    gs->debug_forced_procgen_template_index = -1;
    gs->debug_force_procgen_template_pending = false;
    gs->debug_last_procgen_template_index = -1;
    gs->recent_procgen_template_count = 0;
    gs->recent_procgen_template_cursor = 0;
    memset(gs->recent_procgen_template_ids, 0, sizeof(gs->recent_procgen_template_ids));
    memset(gs->stage_modifier_cooldowns, 0, sizeof(gs->stage_modifier_cooldowns));
    memset(gs->meta_choices, 0, sizeof(gs->meta_choices));
    memset(gs->meta_choice_costs, 0, sizeof(gs->meta_choice_costs));
    memset(gs->perk_levels, 0, sizeof(gs->perk_levels));
    memset(gs->perk_offer_cooldowns, 0, sizeof(gs->perk_offer_cooldowns));
    /* Skill-first loop: always restart straight into round one without a post-run meta menu. */
    game_start_round(gs, gs->round);
    if (banked) {
        emit_event(gs, GAME_EVENT_META_BANKED);
    }
}

void game_init(GameState *gs, uint32_t seed) {
    memset(gs, 0, sizeof(*gs));
    gs->width = GAME_GRID_WIDTH;
    gs->height = GAME_GRID_HEIGHT;
    gs->rng_state = (seed == 0) ? 0x12345678u : seed;
    gs->score = 0;
    gs->lives = DEFAULT_LIVES;
    gs->round = 1;
    gs->round_clear_pending_ticks = 0;
    gs->run_score_mult_permille = RUN_SCORE_MULT_BASE_PERMILLE;
    gs->run_round_time_bonus_ticks = 0;
    gs->run_enemy_speed_penalty_fp = 0;
    gs->run_mine_capacity = 0;
    gs->run_mine_stock = 0;
    gs->run_shards = 0;
    gs->meta_shards = 0;
    gs->meta_progress_points = 0;
    gs->meta_unlock_tier = meta_unlock_tier_from_progress(gs->meta_progress_points);
    gs->perk_choice_count = 0;
    gs->perk_choice_selected = 0;
    gs->meta_choice_count = 0;
    gs->meta_choice_selected = 0;
    gs->pending_round_after_choice = 0;
    gs->mine_tap_dir = DIR_NONE;
    gs->mine_tap_ticks = 0;
    gs->stage_modifier = STAGE_MOD_NONE;
    gs->stage_modifier_flash_ticks = 0;
    gs->debug_forced_stage_modifier = STAGE_MOD_NONE;
    gs->debug_force_stage_modifier_pending = false;
    gs->debug_forced_procgen_template_index = -1;
    gs->debug_force_procgen_template_pending = false;
    gs->debug_last_procgen_template_index = -1;
    gs->recent_procgen_template_count = 0;
    gs->recent_procgen_template_cursor = 0;
    memset(gs->recent_procgen_template_ids, 0, sizeof(gs->recent_procgen_template_ids));
    memset(gs->stage_modifier_cooldowns, 0, sizeof(gs->stage_modifier_cooldowns));
    memset(gs->meta_choices, 0, sizeof(gs->meta_choices));
    memset(gs->meta_choice_costs, 0, sizeof(gs->meta_choice_costs));
    memset(gs->perk_levels, 0, sizeof(gs->perk_levels));
    memset(gs->perk_offer_cooldowns, 0, sizeof(gs->perk_offer_cooldowns));
    game_start_round(gs, gs->round);
}

void game_start_round(GameState *gs, int round_index) {
    LevelDef level;
    int enemy_spawns[GAME_MAX_ENEMIES][2];
    int enemy_spawn_count = 0;
    int unbreakable_count = 0;
    int enemy_spawn_ticks_base = 60;
    int map_pressure_speed_bonus_fp = 0;

    memset(gs->terrain, 0, sizeof(gs->terrain));
    memset(gs->blocks, 0, sizeof(gs->blocks));
    memset(gs->items, 0, sizeof(gs->items));
    clear_active_item_list(gs);
    memset(gs->mines, 0, sizeof(gs->mines));
    memset(gs->mine_fuse_ticks, 0, sizeof(gs->mine_fuse_ticks));
    clear_active_mine_list(gs);
    memset(gs->dirty_static_rows, 0, sizeof(gs->dirty_static_rows));
    memset(gs->moving_blocks, 0, sizeof(gs->moving_blocks));
    gs->active_moving_block_mask = 0;
    memset(gs->enemies, 0, sizeof(gs->enemies));
    gs->alive_enemy_count = 0;
    gs->alive_enemy_mask = 0;
    memset(gs->score_popups, 0, sizeof(gs->score_popups));
    gs->active_score_popup_count = 0;
    gs->active_score_popup_mask = 0;
    memset(gs->impact_fx, 0, sizeof(gs->impact_fx));
    gs->active_impact_fx_count = 0;
    gs->active_impact_fx_mask = 0;

    gs->round = (round_index < 1) ? 1 : round_index;
    enemy_spawn_ticks_base = clampi(
        60 - (((gs->round - 1) / 2) * 3),
        36,
        60);
    resolve_level_for_round(gs, gs->round, &level);
    setup_round_config(gs, gs->round);
    gs->stage_modifier = pick_stage_modifier_for_round(gs, gs->round);
    apply_stage_modifier_to_round_config(gs);
    gs->round_time_ticks = gs->round_config.round_time_ticks;
    gs->round_clear_bonus_score = 0;
    gs->kills_this_round = 0;
    gs->kills_since_item_spawn = 0;
    gs->bonus_item_timer_ticks = 0;
    gs->round_clear_pending_ticks = 0;
    gs->special_alignment_flash_ticks = 0;
    gs->special_alignment_awarded = false;
    gs->timer_danger_active = false;
    gs->timer_danger_pulse_ticks = 0;
    gs->pending_events = 0;
    gs->perk_choice_count = 0;
    gs->perk_choice_selected = 0;
    gs->meta_choice_count = 0;
    gs->meta_choice_selected = 0;
    gs->pending_round_after_choice = 0;
    gs->mine_tap_dir = DIR_NONE;
    gs->mine_tap_ticks = 0;
    gs->stage_modifier_flash_ticks = (gs->stage_modifier == STAGE_MOD_NONE) ? 0 : STAGE_MODIFIER_FLASH_TICKS;
    gs->run_mine_stock = gs->run_mine_capacity;
    gs->meta_unlock_tier = meta_unlock_tier_from_progress(gs->meta_progress_points);

    gs->player_spawn_x = 1;
    gs->player_spawn_y = 1;

    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            const char c = level.rows[y][x];

            Terrain t = TERRAIN_FLOOR;
            if (x == 0 || y == 0 || x == GAME_GRID_WIDTH - 1 || y == GAME_GRID_HEIGHT - 1) {
                t = TERRAIN_BORDER;
            } else if (c == '#') {
                t = TERRAIN_WALL;
            }
            gs->terrain[y][x] = t;

            if (c == 'I') {
                gs->blocks[y][x] = BLOCK_ICE;
            } else if (c == 'S') {
                gs->blocks[y][x] = BLOCK_SPECIAL;
            } else if (c == 'C') {
                gs->blocks[y][x] = BLOCK_CRACKED;
            } else if (c == 'U') {
                gs->blocks[y][x] = BLOCK_UNBREAKABLE;
            } else {
                gs->blocks[y][x] = BLOCK_NONE;
            }

            if (c == 'P') {
                gs->player_spawn_x = x;
                gs->player_spawn_y = y;
            } else if (c == 'E' && enemy_spawn_count < GAME_MAX_ENEMIES) {
                enemy_spawns[enemy_spawn_count][0] = x;
                enemy_spawns[enemy_spawn_count][1] = y;
                ++enemy_spawn_count;
            }
        }
    }

    if (enemy_spawn_count == 0) {
        enemy_spawns[0][0] = GAME_GRID_WIDTH - 3;
        enemy_spawns[0][1] = GAME_GRID_HEIGHT - 3;
        enemy_spawn_count = 1;
    }

    apply_stage_modifier_to_map(gs, enemy_spawns, enemy_spawn_count);
#if !defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE)
    {
        char patched_rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1];
        const int min_mine_anchor_opportunities = clampi(3 + (gs->round / 5), 3, 6);
        build_level_rows_from_runtime_map(gs, enemy_spawns, enemy_spawn_count, patched_rows);
        add_mine_cluster_pockets(
            gs,
            patched_rows,
            gs->player_spawn_x,
            gs->player_spawn_y,
            min_mine_anchor_opportunities);
        apply_runtime_blocks_from_level_rows(gs, patched_rows);
    }
#endif

    gs->enemy_count = clampi(gs->round_config.enemy_count, 1, GAME_MAX_ENEMIES);
    if (enemy_spawn_count > gs->enemy_count) {
        enemy_spawn_count = gs->enemy_count;
    }
    ensure_enemy_spawn_capacity(gs, enemy_spawns, &enemy_spawn_count, gs->enemy_count);
    enemy_spawn_count = enforce_spawn_opening_protection(gs, enemy_spawns, enemy_spawn_count);
    if (enemy_spawn_count < gs->enemy_count) {
        gs->enemy_count = enemy_spawn_count;
    }
    {
        const int special_slots = (gs->enemy_count > 1) ? (gs->enemy_count - 1) : 0;
        gs->round_config.enemy_ghost_count = clampi(
            gs->round_config.enemy_ghost_count,
            0,
            min_int(1, special_slots));
        gs->round_config.enemy_hunter_count = clampi(
            gs->round_config.enemy_hunter_count,
            0,
            special_slots - gs->round_config.enemy_ghost_count);
        gs->round_config.enemy_wanderer_count = clampi(
            gs->round_config.enemy_wanderer_count,
            0,
            special_slots - gs->round_config.enemy_ghost_count - gs->round_config.enemy_hunter_count);
    }

    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            if (gs->blocks[y][x] == BLOCK_UNBREAKABLE) {
                ++unbreakable_count;
            }
        }
    }
    if (gs->round >= 3 && gs->stage_modifier == STAGE_MOD_NONE && unbreakable_count >= 3) {
        map_pressure_speed_bonus_fp = (unbreakable_count >= 5) ? (FP_ONE / 6) : (FP_ONE / 8);
        enemy_spawn_ticks_base = min_int(enemy_spawn_ticks_base, 52);
    }

    gs->alive_enemy_count = 0;
    gs->alive_enemy_mask = 0;
    for (int i = 0; i < gs->enemy_count; ++i) {
        const int spawn_idx = i;
        const int ghost_start = gs->enemy_count - gs->round_config.enemy_ghost_count;
        const int hunter_start = ghost_start - gs->round_config.enemy_hunter_count;
        const int wanderer_start = hunter_start - gs->round_config.enemy_wanderer_count;
        Enemy *enemy = &gs->enemies[i];
        enemy->tile_x = enemy_spawns[spawn_idx][0];
        enemy->tile_y = enemy_spawns[spawn_idx][1];
        enemy->pixel_fp_x = tile_to_fp(enemy->tile_x);
        enemy->pixel_fp_y = tile_to_fp(enemy->tile_y);
        enemy->direction = (Direction)(DIR_UP + (Direction)(rng_next(gs) % 4u));
        enemy->state = ENEMY_SPAWNING;
        if (i >= ghost_start) {
            enemy->type = ENEMY_TYPE_GHOST;
            enemy->speed_fp = clampi(
                gs->round_config.enemy_speed_fp + map_pressure_speed_bonus_fp - (FP_ONE / 2),
                FP_ONE / 2,
                (7 * FP_ONE) / 4);
        } else if (i >= hunter_start) {
            enemy->type = ENEMY_TYPE_HUNTER;
            enemy->speed_fp = clampi(
                gs->round_config.enemy_speed_fp + map_pressure_speed_bonus_fp + (FP_ONE / 4),
                FP_ONE,
                5 * FP_ONE);
        } else if (i >= wanderer_start) {
            enemy->type = ENEMY_TYPE_WANDERER;
            enemy->speed_fp = clampi(
                gs->round_config.enemy_speed_fp + map_pressure_speed_bonus_fp - (FP_ONE / 2),
                FP_ONE,
                5 * FP_ONE);
        } else {
            enemy->type = ENEMY_TYPE_CHASER;
            enemy->speed_fp = clampi(
                gs->round_config.enemy_speed_fp + map_pressure_speed_bonus_fp,
                FP_ONE,
                (9 * FP_ONE) / 4);
        }
        enemy->move_remaining_fp = 0;
        enemy->anim_frame = 0;
        enemy->spawn_ticks = enemy_spawn_ticks_base + (int)(rng_next(gs) % 35u);
        enemy->decision_cooldown_ticks = 0;
        enemy->alive = true;
        gs->alive_enemy_mask |= slot_bit(i);
        if (enemy_is_killable(enemy)) {
            ++gs->alive_enemy_count;
        }
    }

    reset_player_runtime_state(gs, gs->player_spawn_x, gs->player_spawn_y);
    gs->phase = GAME_PHASE_ROUND_INTRO;
    gs->phase_timer_ticks = ROUND_INTRO_TICKS;
    gs->start_title_pending = (gs->round == 1 && gs->score == 0u);
    gs->player_death_from_timeout = false;
    reset_fire_confirm_gate(gs);
    emit_event(gs, GAME_EVENT_ROUND_START);
    if (gs->stage_modifier != STAGE_MOD_NONE) {
        emit_event(gs, GAME_EVENT_STAGE_MODIFIER);
    }
    mark_all_static_dirty(gs);
}

void game_step(GameState *gs, const InputState *in) {
    InputState input = {0};
    bool start_released = false;
    bool fire_confirm_released = false;
    if (in != NULL) {
        input = *in;
    }
    {
        const bool start_down = input.start;
        start_released = !start_down && gs->start_was_down;
        input.start = start_down && !gs->start_was_down;
        gs->start_was_down = start_down;
    }
    if (input.fire_pressed) {
        gs->fire_was_down = true;
    }
    if (input.fire_released) {
        gs->fire_was_down = false;
    }
    fire_confirm_released = consume_fire_confirm_release(gs, &input);
    if (gs->stage_modifier_flash_ticks > 0) {
        --gs->stage_modifier_flash_ticks;
    }

    switch (gs->phase) {
        case GAME_PHASE_ROUND_INTRO:
        {
            const bool confirm_title = start_released || fire_confirm_released;
            const bool direction_held = input.up || input.down || input.left || input.right;
            const bool confirm_intro = start_released ||
                                       fire_confirm_released ||
                                       input.newest_press != DIR_NONE ||
                                       (gs->phase_timer_ticks <= 1 && direction_held);
            if (gs->start_title_pending) {
                if (gs->phase_timer_ticks > 0) {
                    --gs->phase_timer_ticks;
                } else {
                    gs->phase_timer_ticks = ROUND_INTRO_TICKS;
                }
                if (confirm_title) {
                    gs->start_title_pending = false;
                    gs->phase_timer_ticks = ROUND_INTRO_TICKS;
                }
                update_player_motion(gs);
                update_score_popups(gs);
                update_impact_fx(gs);
                break;
            }
            if (gs->phase_timer_ticks > 0) {
                --gs->phase_timer_ticks;
            }
            if (confirm_intro) {
                gs->phase_timer_ticks = 0;
                gs->phase = GAME_PHASE_PLAYING;
            }
            update_player_motion(gs);
            update_score_popups(gs);
            update_impact_fx(gs);
            break;
        }

        case GAME_PHASE_PLAYER_DYING:
            if (gs->phase_timer_ticks > 0) {
                --gs->phase_timer_ticks;
            }
            if (gs->phase_timer_ticks <= 0) {
                handle_player_death_transition(gs);
            }
            update_player_motion(gs);
            update_score_popups(gs);
            update_impact_fx(gs);
            break;

        case GAME_PHASE_ROUND_CLEAR:
            if (start_released
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
                || fire_confirm_released
#endif
            ) {
                gs->phase_timer_ticks = 0;
            }
            if (gs->phase_timer_ticks > 0) {
                --gs->phase_timer_ticks;
            }
            if (gs->phase_timer_ticks <= 0) {
                enter_perk_choice(gs, gs->round + 1);
            }
            update_player_motion(gs);
            update_score_popups(gs);
            update_impact_fx(gs);
            break;

        case GAME_PHASE_GAME_OVER:
            if (start_released
#if defined(ICEPANIC_AMIGA_SMALL_STACK)
                || fire_confirm_released
#endif
            ) {
                gs->phase_timer_ticks = 0;
            }
            if (gs->phase_timer_ticks > 0) {
                --gs->phase_timer_ticks;
            }
            if (gs->phase_timer_ticks <= 0) {
                reset_to_new_game(gs);
            }
            update_player_motion(gs);
            update_score_popups(gs);
            update_impact_fx(gs);
            break;

        case GAME_PHASE_PERK_CHOICE:
            update_perk_choice(gs, &input, start_released, fire_confirm_released);
            update_score_popups(gs);
            update_impact_fx(gs);
            break;

        case GAME_PHASE_META_UPGRADE:
            update_meta_upgrade_choice(gs, &input, start_released, fire_confirm_released);
            update_score_popups(gs);
            update_impact_fx(gs);
            break;

        case GAME_PHASE_PLAYING:
        {
            const bool clear_pending = round_clear_pending_active(gs);
            if (!clear_pending && gs->round_time_ticks > 0) {
                --gs->round_time_ticks;
            }
            if (!clear_pending) {
                update_timer_danger_state(gs);
                update_bonus_item_timer(gs);
            }
            if (!clear_pending && gs->round_time_ticks <= 0) {
                kill_player_for_timeout(gs);
                update_score_popups(gs);
                break;
            }

            if (!clear_pending) {
                update_player_intent(gs, &input);
            }
            update_player_motion(gs);
            update_moving_blocks(gs);
            if (!clear_pending) {
                update_mine_fuses(gs);
                update_enemies(gs);
                resolve_player_enemy_collision(gs);
                resolve_item_collection(gs);
            }
            check_round_clear(gs);
            update_score_popups(gs);
            update_impact_fx(gs);
            break;
        }
    }
}

void game_get_render_state(const GameState *gs, RenderState *out) {
    *out = *gs;
    out->meta_unlock_tier = meta_unlock_tier_from_progress(gs->meta_progress_points);
}

const uint32_t *game_dirty_static_rows(const GameState *gs) {
    return gs ? gs->dirty_static_rows : 0;
}

void game_clear_dirty_static(GameState *gs) {
    if (gs) {
        memset(gs->dirty_static_rows, 0, sizeof(gs->dirty_static_rows));
    }
}

uint32_t game_consume_events(GameState *gs) {
    const uint32_t events = gs->pending_events;
    gs->pending_events = 0;
    return events;
}

void game_set_meta_shards(GameState *gs, uint32_t shards) {
    gs->meta_shards = shards;
}

uint32_t game_get_meta_shards(const GameState *gs) {
    return gs->meta_shards;
}

void game_set_meta_progress(GameState *gs, uint32_t progress_points) {
    gs->meta_progress_points = progress_points;
    gs->meta_unlock_tier = meta_unlock_tier_from_progress(gs->meta_progress_points);
}

uint32_t game_get_meta_progress(const GameState *gs) {
    return gs->meta_progress_points;
}

int game_get_meta_unlock_tier(const GameState *gs) {
    return meta_unlock_tier_from_progress(gs->meta_progress_points);
}

#if !defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE)
static uint32_t hash_u32(uint32_t h, uint32_t v) {
    h ^= v;
    h *= 16777619u;
    return h;
}

static uint32_t hash_bytes(uint32_t h, const void *data, size_t size) {
    const uint8_t *bytes = (const uint8_t *)data;
    for (size_t i = 0; i < size; ++i) {
        h ^= bytes[i];
        h *= 16777619u;
    }
    return h;
}

uint32_t game_debug_hash(const GameState *gs) {
    uint32_t h = 2166136261u;
    h = hash_u32(h, (uint32_t)gs->width);
    h = hash_u32(h, (uint32_t)gs->height);
    h = hash_bytes(h, gs->terrain, sizeof(gs->terrain));
    h = hash_bytes(h, gs->blocks, sizeof(gs->blocks));
    h = hash_bytes(h, gs->items, sizeof(gs->items));
    h = hash_bytes(h, gs->active_item_x, sizeof(gs->active_item_x));
    h = hash_bytes(h, gs->active_item_y, sizeof(gs->active_item_y));
    h = hash_u32(h, (uint32_t)gs->active_item_count);
    h = hash_bytes(h, gs->mines, sizeof(gs->mines));
    h = hash_bytes(h, gs->mine_fuse_ticks, sizeof(gs->mine_fuse_ticks));
    h = hash_bytes(h, gs->active_mine_x, sizeof(gs->active_mine_x));
    h = hash_bytes(h, gs->active_mine_y, sizeof(gs->active_mine_y));
    h = hash_u32(h, (uint32_t)gs->active_mine_count);
    h = hash_bytes(h, &gs->player, sizeof(gs->player));
    h = hash_bytes(h, gs->enemies, sizeof(gs->enemies));
    h = hash_u32(h, (uint32_t)gs->alive_enemy_count);
    h = hash_u32(h, (uint32_t)gs->alive_enemy_mask);
    h = hash_bytes(h, gs->moving_blocks, sizeof(gs->moving_blocks));
    h = hash_u32(h, (uint32_t)gs->active_moving_block_mask);
    h = hash_bytes(h, gs->score_popups, sizeof(gs->score_popups));
    h = hash_u32(h, (uint32_t)gs->active_score_popup_count);
    h = hash_u32(h, (uint32_t)gs->active_score_popup_mask);
    h = hash_bytes(h, gs->impact_fx, sizeof(gs->impact_fx));
    h = hash_u32(h, (uint32_t)gs->active_impact_fx_count);
    h = hash_u32(h, (uint32_t)gs->active_impact_fx_mask);
    h = hash_u32(h, gs->score);
    h = hash_u32(h, (uint32_t)gs->lives);
    h = hash_u32(h, (uint32_t)gs->round);
    h = hash_u32(h, (uint32_t)gs->round_time_ticks);
    h = hash_u32(h, (uint32_t)gs->round_clear_pending_ticks);
    h = hash_u32(h, (uint32_t)gs->round_clear_bonus_score);
    h = hash_u32(h, (uint32_t)gs->kills_this_round);
    h = hash_u32(h, (uint32_t)gs->kills_since_item_spawn);
    h = hash_u32(h, (uint32_t)gs->bonus_item_timer_ticks);
    h = hash_u32(h, (uint32_t)gs->special_alignment_flash_ticks);
    h = hash_u32(h, gs->special_alignment_awarded ? 1u : 0u);
    h = hash_u32(h, gs->timer_danger_active ? 1u : 0u);
    h = hash_u32(h, (uint32_t)gs->timer_danger_pulse_ticks);
    h = hash_u32(h, gs->pending_events);
    h = hash_u32(h, (uint32_t)gs->run_score_mult_permille);
    h = hash_u32(h, (uint32_t)gs->run_round_time_bonus_ticks);
    h = hash_u32(h, (uint32_t)gs->run_enemy_speed_penalty_fp);
    h = hash_u32(h, (uint32_t)gs->run_mine_capacity);
    h = hash_u32(h, (uint32_t)gs->run_mine_stock);
    h = hash_u32(h, (uint32_t)gs->run_shards);
    h = hash_u32(h, gs->meta_shards);
    h = hash_u32(h, gs->meta_progress_points);
    h = hash_u32(h, (uint32_t)gs->stage_modifier);
    h = hash_u32(h, (uint32_t)gs->stage_modifier_flash_ticks);
    h = hash_bytes(h, gs->stage_modifier_cooldowns, sizeof(gs->stage_modifier_cooldowns));
    h = hash_bytes(h, gs->perk_levels, sizeof(gs->perk_levels));
    h = hash_bytes(h, gs->perk_offer_cooldowns, sizeof(gs->perk_offer_cooldowns));
    h = hash_bytes(h, gs->perk_choices, sizeof(gs->perk_choices));
    h = hash_u32(h, (uint32_t)gs->perk_choice_count);
    h = hash_u32(h, (uint32_t)gs->perk_choice_selected);
    h = hash_bytes(h, gs->meta_choices, sizeof(gs->meta_choices));
    h = hash_bytes(h, gs->meta_choice_costs, sizeof(gs->meta_choice_costs));
    h = hash_u32(h, (uint32_t)gs->meta_choice_count);
    h = hash_u32(h, (uint32_t)gs->meta_choice_selected);
    h = hash_u32(h, (uint32_t)gs->pending_round_after_choice);
    h = hash_u32(h, (uint32_t)gs->mine_tap_dir);
    h = hash_u32(h, (uint32_t)gs->mine_tap_ticks);
    h = hash_bytes(h, gs->recent_procgen_template_ids, sizeof(gs->recent_procgen_template_ids));
    h = hash_u32(h, (uint32_t)gs->recent_procgen_template_count);
    h = hash_u32(h, (uint32_t)gs->recent_procgen_template_cursor);
    h = hash_u32(h, (uint32_t)gs->debug_forced_stage_modifier);
    h = hash_u32(h, gs->debug_force_stage_modifier_pending ? 1u : 0u);
    h = hash_u32(h, (uint32_t)gs->debug_forced_procgen_template_index);
    h = hash_u32(h, gs->debug_force_procgen_template_pending ? 1u : 0u);
    h = hash_u32(h, (uint32_t)gs->debug_last_procgen_template_index);
    h = hash_u32(h, gs->start_was_down ? 1u : 0u);
    h = hash_u32(h, gs->fire_was_down ? 1u : 0u);
    h = hash_u32(h, gs->fire_confirm_armed ? 1u : 0u);
    h = hash_u32(h, gs->start_title_pending ? 1u : 0u);
    h = hash_u32(h, gs->rng_state);
    h = hash_u32(h, (uint32_t)gs->phase);
    h = hash_u32(h, (uint32_t)gs->phase_timer_ticks);
    h = hash_bytes(h, &gs->round_config, sizeof(gs->round_config));
    return h;
}

int game_debug_count_enemy_type(const GameState *gs, EnemyType type) {
    int count = 0;
    uint16_t enemy_mask = live_enemy_mask_for_game(gs);
    for (int i = 0; enemy_mask != 0u && i < gs->enemy_count; ++i, enemy_mask >>= 1) {
        if ((enemy_mask & 1u) == 0u || !gs->enemies[i].alive) {
            continue;
        }
        if (gs->enemies[i].type == type) {
            ++count;
        }
    }
    return count;
}

bool game_debug_validate_level_rows(const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]) {
#if defined(ICEPANIC_AMIGA_OCS_ARCADE_CORE)
    return rows != NULL;
#else
    return validate_level_rows_internal(rows);
#endif
}

void game_debug_force_next_stage_modifier(GameState *gs, StageModifierType modifier) {
    if (gs == NULL) {
        return;
    }
    if (modifier < STAGE_MOD_NONE || modifier > STAGE_MOD_FROST_SIREN) {
        gs->debug_forced_stage_modifier = STAGE_MOD_NONE;
        gs->debug_force_stage_modifier_pending = false;
        return;
    }
    gs->debug_forced_stage_modifier = modifier;
    gs->debug_force_stage_modifier_pending = true;
}

int game_debug_procgen_template_count(void) {
#ifdef ICEPANIC_AMIGA_OCS_FIXED_LEVELS
    return 0;
#else
    return PROCGEN_WFC_TEMPLATE_COUNT;
#endif
}

bool game_debug_get_procgen_template_rows(int template_index, char out_rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]) {
#ifdef ICEPANIC_AMIGA_OCS_FIXED_LEVELS
    if (out_rows == NULL || template_index < 0) {
        return false;
    }
    return false;
#else
    if (out_rows == NULL) {
        return false;
    }
    if (template_index < 0 || template_index >= PROCGEN_WFC_TEMPLATE_COUNT) {
        return false;
    }
    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        memcpy(out_rows[y], kProcgenWfcTemplates[template_index][y], GAME_GRID_WIDTH + 1);
    }
    return true;
#endif
}

int game_debug_last_procgen_template_index(const GameState *gs) {
    if (gs == NULL) {
        return -1;
    }
    return gs->debug_last_procgen_template_index;
}

void game_debug_force_next_procgen_template(GameState *gs, int template_index) {
    if (gs == NULL) {
        return;
    }
#ifdef ICEPANIC_AMIGA_OCS_FIXED_LEVELS
    gs->debug_forced_procgen_template_index = template_index;
    gs->debug_force_procgen_template_pending = false;
#else
    gs->debug_forced_procgen_template_index = template_index;
    gs->debug_force_procgen_template_pending = true;
#endif
}
#endif
