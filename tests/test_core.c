#include "game.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum {
    FP_SHIFT_TEST = 8,
    FP_ONE_TEST = 1 << FP_SHIFT_TEST,
    PUSH_SLIDE_TEST_TICKS = 84,
    PUSH_IMPACT_TEST_TICKS = 60,
};

static int tile_to_fp_test(int tile) {
    return tile * GAME_TILE_SIZE * FP_ONE_TEST;
}

static int count_items_on_map(const GameState *gs) {
    int count = 0;
    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            if (gs->items[y][x] != ITEM_NONE) {
                ++count;
            }
        }
    }
    return count;
}

static int count_active_popups(const GameState *gs) {
    int count = 0;
    for (int i = 0; i < GAME_MAX_SCORE_POPUPS; ++i) {
        if (gs->score_popups[i].active) {
            ++count;
        }
    }
    return count;
}

static int count_active_impact_fx(const GameState *gs) {
    int count = 0;
    for (int i = 0; i < GAME_MAX_IMPACT_FX; ++i) {
        if (gs->impact_fx[i].active) {
            ++count;
        }
    }
    return count;
}

static bool has_active_impact_fx_style(const GameState *gs, int style, int *out_base_ttl) {
    for (int i = 0; i < GAME_MAX_IMPACT_FX; ++i) {
        if (!gs->impact_fx[i].active) {
            continue;
        }
        if (gs->impact_fx[i].style != style) {
            continue;
        }
        if (out_base_ttl != NULL) {
            *out_base_ttl = gs->impact_fx[i].base_ttl_ticks;
        }
        return true;
    }
    return false;
}

static int count_blocks_of_type(const GameState *gs, BlockType type) {
    int count = 0;
    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            if (gs->blocks[y][x] == type) {
                ++count;
            }
        }
    }
    return count;
}

static int count_non_empty_blocks(const GameState *gs) {
    int count = 0;
    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            if (gs->blocks[y][x] != BLOCK_NONE) {
                ++count;
            }
        }
    }
    return count;
}

static bool block_has_slide_lane(const GameState *gs, int x, int y, int min_clear_tiles) {
    static const int kDx[5] = {0, 0, 0, -1, 1};
    static const int kDy[5] = {0, -1, 1, 0, 0};
    const BlockType block = gs->blocks[y][x];
    if (block != BLOCK_ICE && block != BLOCK_SPECIAL && block != BLOCK_CRACKED) {
        return false;
    }

    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        int cx = x;
        int cy = y;
        int clear = 0;
        while (true) {
            const int nx = cx + kDx[d];
            const int ny = cy + kDy[d];
            if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
                break;
            }
            if (gs->terrain[ny][nx] != TERRAIN_FLOOR || gs->blocks[ny][nx] != BLOCK_NONE) {
                break;
            }
            ++clear;
            cx = nx;
            cy = ny;
        }
        if (clear >= min_clear_tiles) {
            return true;
        }
    }

    return false;
}

static bool tile_has_enemy_test(const GameState *gs, int x, int y) {
    for (int i = 0; i < gs->enemy_count; ++i) {
        if (!gs->enemies[i].alive) {
            continue;
        }
        if (gs->enemies[i].tile_x == x && gs->enemies[i].tile_y == y) {
            return true;
        }
    }
    return false;
}

static bool tile_has_push_option_from_test(const GameState *gs, int from_x, int from_y) {
    static const int kDx[5] = {0, 0, 0, -1, 1};
    static const int kDy[5] = {0, -1, 1, 0, 0};
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int bx = from_x + kDx[d];
        const int by = from_y + kDy[d];
        const int tx = bx + kDx[d];
        const int ty = by + kDy[d];
        BlockType block = BLOCK_NONE;
        if (bx <= 0 || by <= 0 || bx >= GAME_GRID_WIDTH - 1 || by >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        if (tx <= 0 || ty <= 0 || tx >= GAME_GRID_WIDTH - 1 || ty >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        block = gs->blocks[by][bx];
        if (block != BLOCK_ICE && block != BLOCK_SPECIAL && block != BLOCK_CRACKED) {
            continue;
        }
        if (gs->terrain[ty][tx] != TERRAIN_FLOOR || gs->blocks[ty][tx] != BLOCK_NONE) {
            continue;
        }
        if (tile_has_enemy_test(gs, bx, by) || tile_has_enemy_test(gs, tx, ty)) {
            continue;
        }
        return true;
    }
    return false;
}

static bool spawn_has_nearby_push_option_test(const GameState *gs) {
    static const int kDx[5] = {0, 0, 0, -1, 1};
    static const int kDy[5] = {0, -1, 1, 0, 0};
    const int sx = gs->player_spawn_x;
    const int sy = gs->player_spawn_y;

    if (tile_has_push_option_from_test(gs, sx, sy)) {
        return true;
    }

    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = sx + kDx[d];
        const int ny = sy + kDy[d];
        if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        if (gs->terrain[ny][nx] != TERRAIN_FLOOR || gs->blocks[ny][nx] != BLOCK_NONE) {
            continue;
        }
        if (tile_has_enemy_test(gs, nx, ny)) {
            continue;
        }
        if (tile_has_push_option_from_test(gs, nx, ny)) {
            return true;
        }
    }

    return false;
}

static bool test_floor_walkable(const GameState *gs, int x, int y) {
    if (x <= 0 || y <= 0 || x >= GAME_GRID_WIDTH - 1 || y >= GAME_GRID_HEIGHT - 1) {
        return false;
    }
    return gs->terrain[y][x] == TERRAIN_FLOOR && gs->blocks[y][x] == BLOCK_NONE;
}

static void analyze_reachable_floor_region_test(
    const GameState *gs,
    int start_x,
    int start_y,
    int *out_reachable,
    int *out_junctions) {
    enum {
        MAX_QUEUE = GAME_GRID_WIDTH * GAME_GRID_HEIGHT
    };
    static const int kDx[5] = {0, 0, 0, -1, 1};
    static const int kDy[5] = {0, -1, 1, 0, 0};
    bool visited[GAME_GRID_HEIGHT][GAME_GRID_WIDTH] = {{false}};
    int queue_x[MAX_QUEUE];
    int queue_y[MAX_QUEUE];
    int head = 0;
    int tail = 0;
    int reachable = 0;
    int junctions = 0;

    if (out_reachable == NULL || out_junctions == NULL) {
        return;
    }
    *out_reachable = 0;
    *out_junctions = 0;

    if (!test_floor_walkable(gs, start_x, start_y)) {
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
            const int nx = x + kDx[d];
            const int ny = y + kDy[d];
            if (!test_floor_walkable(gs, nx, ny)) {
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

static bool perk_list_contains(const PerkType *perks, int count, PerkType target) {
    for (int i = 0; i < count; ++i) {
        if (perks[i] == target) {
            return true;
        }
    }
    return false;
}

static bool meta_list_contains(const MetaUpgradeType *upgrades, int count, MetaUpgradeType target) {
    for (int i = 0; i < count; ++i) {
        if (upgrades[i] == target) {
            return true;
        }
    }
    return false;
}

static int count_mine_anchor_opportunities_test(const GameState *gs) {
    static const int kDx[5] = {0, 0, 0, -1, 1};
    static const int kDy[5] = {0, -1, 1, 0, 0};
    int opportunities = 0;
    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            int breakable_neighbors = 0;
            int walkable_neighbors = 0;
            if (gs->terrain[y][x] != TERRAIN_FLOOR || gs->blocks[y][x] != BLOCK_NONE) {
                continue;
            }
            for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
                const int nx = x + kDx[d];
                const int ny = y + kDy[d];
                const BlockType block = gs->blocks[ny][nx];
                if (block == BLOCK_ICE || block == BLOCK_SPECIAL || block == BLOCK_CRACKED) {
                    ++breakable_neighbors;
                }
                if (gs->terrain[ny][nx] == TERRAIN_FLOOR && gs->blocks[ny][nx] == BLOCK_NONE) {
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

static void force_apply_perk_choice(GameState *gs, PerkType perk) {
    InputState press = {0};
    InputState release = {0};
    press.start = true;
    gs->phase = GAME_PHASE_PERK_CHOICE;
    gs->perk_choice_count = 1;
    gs->perk_choice_selected = 0;
    gs->perk_choices[0] = perk;
    gs->pending_round_after_choice = gs->round + 1;
    game_step(gs, &press);
    game_step(gs, &release);
}

static bool require_true(bool condition, const char *test_name, const char *message, int line) {
    if (condition) {
        return true;
    }
    fprintf(stderr, "[FAIL] %s (line %d): %s\n", test_name, line, message);
    return false;
}

#define REQUIRE(test_name, cond, msg)                           \
    do {                                                         \
        if (!require_true((cond), (test_name), (msg), __LINE__)) { \
            return false;                                       \
        }                                                        \
    } while (0)

static void init_empty_playing_arena(GameState *gs) {
    memset(gs, 0, sizeof(*gs));
    gs->width = GAME_GRID_WIDTH;
    gs->height = GAME_GRID_HEIGHT;
    gs->phase = GAME_PHASE_PLAYING;
    gs->lives = 3;
    gs->round = 1;
    gs->rng_state = 0xabc12345u;
    gs->round_time_ticks = 99 * GAME_FIXED_TICK_HZ;
    gs->round_config.enemy_count = 1;
    gs->round_config.enemy_speed_fp = 2 * FP_ONE_TEST;
    gs->round_config.aggression_percent = 50;
    gs->round_config.round_time_ticks = gs->round_time_ticks;
    gs->run_score_mult_permille = 1000;

    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            if (x == 0 || y == 0 || x == GAME_GRID_WIDTH - 1 || y == GAME_GRID_HEIGHT - 1) {
                gs->terrain[y][x] = TERRAIN_BORDER;
            } else {
                gs->terrain[y][x] = TERRAIN_FLOOR;
            }
            gs->blocks[y][x] = BLOCK_NONE;
            gs->items[y][x] = ITEM_NONE;
        }
    }

    gs->player_spawn_x = 2;
    gs->player_spawn_y = 2;
    gs->player.tile_x = 2;
    gs->player.tile_y = 2;
    gs->player.pixel_fp_x = tile_to_fp_test(2);
    gs->player.pixel_fp_y = tile_to_fp_test(2);
    gs->player.facing = DIR_RIGHT;
    gs->player.move_dir = DIR_NONE;
    gs->player.buffered_dir = DIR_NONE;
    gs->player.state = PLAYER_IDLE;
    gs->player.move_remaining_fp = 0;
    gs->player.push_timer = 0;
    gs->player.bump_timer = 0;
    gs->player.respawn_invuln_ticks = 0;
    gs->player.alive = true;
}

static void build_level_rows_from_state_test(
    const GameState *gs,
    char out_rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]) {
    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            char c = '.';
            if (gs->terrain[y][x] == TERRAIN_BORDER || gs->terrain[y][x] == TERRAIN_WALL) {
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

    for (int i = 0; i < gs->enemy_count; ++i) {
        const Enemy *enemy = &gs->enemies[i];
        if (!enemy->alive) {
            continue;
        }
        if (enemy->tile_x < 0 || enemy->tile_x >= GAME_GRID_WIDTH ||
            enemy->tile_y < 0 || enemy->tile_y >= GAME_GRID_HEIGHT) {
            continue;
        }
        out_rows[enemy->tile_y][enemy->tile_x] = 'E';
    }
}

static int find_viable_procgen_templates_for_round(int round_index, int *out_indices, int max_count) {
    const int template_count = game_debug_procgen_template_count();
    int found = 0;
    if (out_indices == NULL || max_count <= 0) {
        return 0;
    }
    for (int i = 0; i < template_count && found < max_count; ++i) {
        GameState probe;
        game_init(&probe, 0x5a17u + (uint32_t)(i * 17));
        game_debug_force_next_procgen_template(&probe, i);
        game_start_round(&probe, round_index);
        if (game_debug_last_procgen_template_index(&probe) == i) {
            out_indices[found++] = i;
        }
    }
    return found;
}

static int count_walkable_neighbors_rows(
    const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    int x,
    int y) {
    static const int kDx[5] = {0, 0, 0, -1, 1};
    static const int kDy[5] = {0, -1, 1, 0, 0};
    int count = 0;
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = x + kDx[d];
        const int ny = y + kDy[d];
        if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
            continue;
        }
        if (rows[ny][nx] == '.') {
            ++count;
        }
    }
    return count;
}

static bool choose_spawn_candidate_rows(
    const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    int *out_x,
    int *out_y) {
    int best_dist = 1 << 30;
    int best_x = -1;
    int best_y = -1;
    const int cx = GAME_GRID_WIDTH / 2;
    const int cy = GAME_GRID_HEIGHT / 2;

    if (out_x == NULL || out_y == NULL) {
        return false;
    }

    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            int dist = 0;
            if (rows[y][x] != '.') {
                continue;
            }
            if (count_walkable_neighbors_rows(rows, x, y) < 2) {
                continue;
            }
            dist = abs(x - cx) + abs(y - cy);
            if (dist < best_dist) {
                best_dist = dist;
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

static bool choose_enemy_candidate_rows(
    const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    int player_x,
    int player_y,
    int *out_x,
    int *out_y) {
    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            if (rows[y][x] != '.') {
                continue;
            }
            if (abs(x - player_x) + abs(y - player_y) < 6) {
                continue;
            }
            if (count_walkable_neighbors_rows(rows, x, y) < 2) {
                continue;
            }
            *out_x = x;
            *out_y = y;
            return true;
        }
    }
    return false;
}

static bool run_to_playing_phase(GameState *gs) {
    InputState release = {0};
    InputState press = {0};
    press.start = true;
    for (int i = 0; i < 300 && gs->phase != GAME_PHASE_PLAYING; ++i) {
        game_step(gs, &press);
        if (gs->phase == GAME_PHASE_PLAYING) {
            break;
        }
        game_step(gs, &release);
    }
    return gs->phase == GAME_PHASE_PLAYING;
}

static bool test_round_intro_waits_for_input(void) {
    const char *name = "round_intro_waits_for_input";
    GameState gs;
    game_init(&gs, 0x1324u);
    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_INTRO, "game should start in round intro");
    REQUIRE(name, gs.start_title_pending, "start title gate should be active for opening round");

    {
        InputState none = {0};
        for (int i = 0; i < 240; ++i) {
            game_step(&gs, &none);
        }
    }
    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_INTRO, "round intro should not auto-advance");
    REQUIRE(name, gs.start_title_pending, "opening title gate should remain active without confirm");

    {
        InputState dir_only = {0};
        dir_only.newest_press = DIR_RIGHT;
        game_step(&gs, &dir_only);
    }
    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_INTRO, "direction press should not bypass title gate");
    REQUIRE(name, gs.start_title_pending, "title gate should require start/fire input");

    {
        InputState press = {0};
        InputState release = {0};
        press.start = true;
        game_step(&gs, &press);
        game_step(&gs, &release);
    }
    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_INTRO, "first start release should reveal round box before gameplay");
    REQUIRE(name, !gs.start_title_pending, "title gate should clear after start release");

    {
        InputState press = {0};
        InputState release = {0};
        press.start = true;
        game_step(&gs, &press);
        game_step(&gs, &release);
    }
    REQUIRE(name, gs.phase == GAME_PHASE_PLAYING, "second release confirm should enter gameplay");
    return true;
}

static int first_active_moving_block(const GameState *gs) {
    for (int i = 0; i < GAME_MAX_MOVING_BLOCKS; ++i) {
        if (gs->moving_blocks[i].active) {
            return i;
        }
    }
    return -1;
}

static bool test_determinism_replay(void) {
    const char *name = "determinism_replay";
    GameState a;
    GameState b;
    game_init(&a, 0x1234u);
    game_init(&b, 0x1234u);

    REQUIRE(name, run_to_playing_phase(&a), "game A did not enter playing phase");
    REQUIRE(name, run_to_playing_phase(&b), "game B did not enter playing phase");

    for (int i = 0; i < 1200; ++i) {
        InputState in = {0};
        if ((i % 37) == 0) {
            in.newest_press = DIR_RIGHT;
            in.right = true;
        } else if ((i % 53) == 0) {
            in.newest_press = DIR_LEFT;
            in.left = true;
        } else if ((i % 29) == 0) {
            in.newest_press = DIR_UP;
            in.up = true;
        } else if ((i % 47) == 0) {
            in.newest_press = DIR_DOWN;
            in.down = true;
        } else {
            in.newest_press = DIR_NONE;
            in.right = (i % 11) < 3;
            in.left = (i % 17) < 2;
            in.up = (i % 19) < 2;
            in.down = (i % 23) < 2;
        }

        game_step(&a, &in);
        game_step(&b, &in);
    }

    const uint32_t ha = game_debug_hash(&a);
    const uint32_t hb = game_debug_hash(&b);
    REQUIRE(name, ha == hb, "state hashes differ for identical replay");
    return true;
}

static bool test_push_slide_and_crush(void) {
    const char *name = "push_slide_and_crush";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][8] = TERRAIN_WALL;
    gs.enemy_count = 2;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 6;
    gs.enemies[0].tile_y = 2;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(6);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(2);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;
    gs.enemies[1].alive = true;
    gs.enemies[1].state = ENEMY_SPAWNING;
    gs.enemies[1].tile_x = 10;
    gs.enemies[1].tile_y = 9;
    gs.enemies[1].pixel_fp_x = tile_to_fp_test(10);
    gs.enemies[1].pixel_fp_y = tile_to_fp_test(9);
    gs.enemies[1].direction = DIR_LEFT;
    gs.enemies[1].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[1].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
    }

    REQUIRE(name, gs.enemies[0].alive == false, "enemy was not crushed by sliding block");
    if (gs.blocks[2][7] != BLOCK_ICE) {
        int active_index = -1;
        for (int i = 0; i < GAME_MAX_MOVING_BLOCKS; ++i) {
            if (gs.moving_blocks[i].active) {
                active_index = i;
                break;
            }
        }
        if (active_index >= 0) {
            fprintf(
                stderr,
                "[FAIL] %s: block@2,7=%d and active moving block at %d,%d (intra=%d)\n",
                name,
                (int)gs.blocks[2][7],
                gs.moving_blocks[active_index].tile_x,
                gs.moving_blocks[active_index].tile_y,
                gs.moving_blocks[active_index].intra_fp);
        } else {
            fprintf(stderr, "[FAIL] %s: block@2,7=%d and no active moving block\n", name, (int)gs.blocks[2][7]);
        }
        return false;
    }
    REQUIRE(name, gs.score >= 100u, "crush did not award score");
    return true;
}

static bool test_push_slide_crushes_interpolated_enemy_overlap(void) {
    const char *name = "push_slide_crushes_interpolated_enemy_overlap";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][8] = TERRAIN_WALL;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_SPAWNING;
    gs.enemies[0].tile_x = 5;
    gs.enemies[0].tile_y = 2;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(5) + (GAME_TILE_SIZE * FP_ONE_TEST) / 2;
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(2);
    gs.enemies[0].direction = DIR_RIGHT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
    }

    REQUIRE(name, gs.enemies[0].alive == false, "interpolated enemy overlap was not crushed by sliding block");
    REQUIRE(name, gs.score >= 100u, "interpolated crush did not award score");
    return true;
}

static bool test_death_preserves_map_state(void) {
    const char *name = "death_preserves_map_state";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.blocks[5][5] = BLOCK_ICE;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_SPAWNING;
    gs.enemies[0].tile_x = 2;
    gs.enemies[0].tile_y = 2;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(2);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(2);
    gs.enemies[0].direction = DIR_RIGHT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 0;
    gs.player.respawn_invuln_ticks = 0;

    InputState none = {0};
    game_step(&gs, &none);
    REQUIRE(name, gs.phase == GAME_PHASE_PLAYER_DYING, "player death phase did not trigger");
    (void)game_consume_events(&gs);

    for (int i = 0; i < 300 && gs.phase == GAME_PHASE_PLAYER_DYING; ++i) {
        game_step(&gs, &none);
    }

    REQUIRE(name, gs.lives == 2, "lives not decremented after death");
    REQUIRE(name, gs.blocks[5][5] == BLOCK_ICE, "block layer was not preserved across death");
    REQUIRE(name, gs.enemies[0].alive == true, "surviving enemy should remain alive");
    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_INTRO || gs.phase == GAME_PHASE_PLAYING, "unexpected phase after respawn");
    {
        const uint32_t events = game_consume_events(&gs);
        REQUIRE(name, (events & GAME_EVENT_LIFE_LOST) != 0u, "life-lost event was not emitted on non-final death");
        REQUIRE(name, (events & GAME_EVENT_GAME_OVER) == 0u, "non-final death should not emit game-over event");
    }
    return true;
}

static bool test_round_clear_advances_round(void) {
    const char *name = "round_clear_advances_round";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][8] = TERRAIN_WALL;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 6;
    gs.enemies[0].tile_y = 2;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(6);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(2);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
    }
    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_CLEAR, "round clear phase did not trigger");

    const int old_round = gs.round;
    for (int i = 0; i < 300 && gs.round == old_round; ++i) {
        InputState in = {0};
        in.start = ((i & 1) == 0);
        game_step(&gs, &in);
    }
    REQUIRE(name, gs.round == old_round + 1, "round did not advance after clear");
    return true;
}

static bool test_round_clear_does_not_require_points(void) {
    const char *name = "round_clear_does_not_require_points";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.score = 99999999u; /* At score cap: round-clear bonus cannot add points. */
    gs.enemy_count = 0;

    {
        InputState none = {0};
        game_step(&gs, &none);
    }
    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_CLEAR, "enemy wipe should trigger round clear even when score cannot increase");
    REQUIRE(name, gs.score == 99999999u, "score should remain capped during this check");

    {
        const int old_round = gs.round;
        for (int i = 0; i < 300 && gs.round == old_round; ++i) {
            InputState in = {0};
            in.start = ((i & 1) == 0);
            game_step(&gs, &in);
        }
        REQUIRE(name, gs.round == old_round + 1, "round should still advance without scoring additional points");
    }

    return true;
}

static bool test_round_clear_tracks_time_bonus_score(void) {
    const char *name = "round_clear_tracks_time_bonus_score";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.enemy_count = 0;
    gs.round_time_ticks = 50;
    gs.run_score_mult_permille = 1300;
    gs.score = 0u;

    {
        InputState none = {0};
        game_step(&gs, &none);
    }

    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_CLEAR, "enemy wipe did not trigger round clear");
    {
        const int expected = ((gs.round_time_ticks * 2) * gs.run_score_mult_permille) / 1000;
        REQUIRE(name, gs.round_clear_bonus_score == expected, "round clear did not record expected time bonus score");
        REQUIRE(name, gs.score == (uint32_t)expected, "recorded time bonus did not match awarded score");
    }
    return true;
}

static bool test_perk_choice_applies_selected_perk(void) {
    const char *name = "perk_choice_applies_selected_perk";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.phase = GAME_PHASE_PERK_CHOICE;
    gs.perk_choice_count = 3;
    gs.perk_choice_selected = 0;
    gs.perk_choices[0] = PERK_LIFE_BOOST;
    gs.perk_choices[1] = PERK_TIME_BOOST;
    gs.perk_choices[2] = PERK_ENEMY_SLOW;
    gs.pending_round_after_choice = 2;

    {
        InputState press = {0};
        InputState release = {0};
        press.start = true;
        press.newest_press = DIR_NONE;
        game_step(&gs, &press);
        game_step(&gs, &release);
    }

    REQUIRE(name, gs.round == 2, "perk confirm did not start next round");
    REQUIRE(name, gs.lives == 4, "life perk was not applied");
    REQUIRE(name, gs.perk_levels[PERK_LIFE_BOOST] == 1, "life perk level did not increment");
    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_INTRO || gs.phase == GAME_PHASE_PLAYING, "unexpected phase after perk confirm");
    return true;
}

static bool test_perk_draft_anti_repeat_has_new_offer(void) {
    const char *name = "perk_draft_anti_repeat_has_new_offer";
    GameState gs;
    init_empty_playing_arena(&gs);

    {
        InputState none = {0};
        gs.phase = GAME_PHASE_ROUND_CLEAR;
        gs.phase_timer_ticks = 0;
        game_step(&gs, &none);
    }
    REQUIRE(name, gs.phase == GAME_PHASE_PERK_CHOICE, "first perk draft phase did not start");
    REQUIRE(name, gs.perk_choice_count == 3, "first perk draft should provide 3 choices");

    PerkType first[GAME_PERK_CHOICE_COUNT] = {PERK_NONE, PERK_NONE, PERK_NONE};
    for (int i = 0; i < gs.perk_choice_count; ++i) {
        first[i] = gs.perk_choices[i];
    }

    {
        InputState none = {0};
        gs.phase = GAME_PHASE_ROUND_CLEAR;
        gs.phase_timer_ticks = 0;
        game_step(&gs, &none);
    }
    REQUIRE(name, gs.phase == GAME_PHASE_PERK_CHOICE, "second perk draft phase did not start");

    int new_offers = 0;
    for (int i = 0; i < gs.perk_choice_count; ++i) {
        if (!perk_list_contains(first, GAME_PERK_CHOICE_COUNT, gs.perk_choices[i])) {
            ++new_offers;
        }
    }
    REQUIRE(name, new_offers >= 1, "second draft should include at least one new perk offer");
    return true;
}

static bool test_perk_bonus_scaling_applies_caps(void) {
    const char *name = "perk_bonus_scaling_applies_caps";
    GameState gs;
    init_empty_playing_arena(&gs);
    const int time_cap_ticks = 35 * GAME_FIXED_TICK_HZ;
    const int score_cap_permille = 1800;
    const int slow_cap_fp = (3 * FP_ONE_TEST) / 4;

    int first_time_gain = 0;
    int second_time_gain = 0;
    int first_score_gain = 0;
    int second_score_gain = 0;
    int first_slow_gain = 0;
    int second_slow_gain = 0;

    {
        const int before = gs.run_round_time_bonus_ticks;
        force_apply_perk_choice(&gs, PERK_TIME_BOOST);
        first_time_gain = gs.run_round_time_bonus_ticks - before;
    }
    {
        const int before = gs.run_round_time_bonus_ticks;
        force_apply_perk_choice(&gs, PERK_TIME_BOOST);
        second_time_gain = gs.run_round_time_bonus_ticks - before;
    }
    REQUIRE(name, first_time_gain > 0, "first time perk should add bonus ticks");
    REQUIRE(name, second_time_gain > 0, "second time perk should add bonus ticks");
    REQUIRE(name, second_time_gain < first_time_gain, "time perk should diminish after level 1");

    {
        const int before = gs.run_score_mult_permille;
        force_apply_perk_choice(&gs, PERK_SCORE_BOOST);
        first_score_gain = gs.run_score_mult_permille - before;
    }
    {
        const int before = gs.run_score_mult_permille;
        force_apply_perk_choice(&gs, PERK_SCORE_BOOST);
        second_score_gain = gs.run_score_mult_permille - before;
    }
    REQUIRE(name, first_score_gain > 0, "first score perk should increase multiplier");
    REQUIRE(name, second_score_gain > 0, "second score perk should increase multiplier");
    REQUIRE(name, second_score_gain < first_score_gain, "score perk should diminish after level 1");

    {
        const int before = gs.run_enemy_speed_penalty_fp;
        force_apply_perk_choice(&gs, PERK_ENEMY_SLOW);
        first_slow_gain = gs.run_enemy_speed_penalty_fp - before;
    }
    {
        const int before = gs.run_enemy_speed_penalty_fp;
        force_apply_perk_choice(&gs, PERK_ENEMY_SLOW);
        second_slow_gain = gs.run_enemy_speed_penalty_fp - before;
    }
    REQUIRE(name, first_slow_gain > 0, "first slow perk should apply");
    REQUIRE(name, second_slow_gain > 0, "second slow perk should apply");
    REQUIRE(name, second_slow_gain < first_slow_gain, "slow perk should diminish after level 1");

    for (int i = 0; i < 20; ++i) {
        force_apply_perk_choice(&gs, PERK_TIME_BOOST);
        force_apply_perk_choice(&gs, PERK_SCORE_BOOST);
        force_apply_perk_choice(&gs, PERK_ENEMY_SLOW);
        force_apply_perk_choice(&gs, PERK_LIFE_BOOST);
    }

    REQUIRE(name, gs.run_round_time_bonus_ticks <= time_cap_ticks, "time bonus exceeded cap");
    REQUIRE(name, gs.run_score_mult_permille <= score_cap_permille, "score multiplier exceeded cap");
    REQUIRE(name, gs.run_enemy_speed_penalty_fp <= slow_cap_fp, "enemy slow exceeded cap");
    REQUIRE(name, gs.lives <= 7, "life perk exceeded max lives cap");
    return true;
}

static bool test_perk_draft_can_skip_when_all_perks_capped(void) {
    const char *name = "perk_draft_can_skip_when_all_perks_capped";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.lives = 7;
    gs.run_round_time_bonus_ticks = 35 * GAME_FIXED_TICK_HZ;
    gs.run_score_mult_permille = 1800;
    gs.run_enemy_speed_penalty_fp = (3 * FP_ONE_TEST) / 4;
    gs.run_mine_capacity = 8;
    gs.run_mine_stock = 8;
    gs.phase = GAME_PHASE_ROUND_CLEAR;
    gs.phase_timer_ticks = 0;

    {
        InputState none = {0};
        game_step(&gs, &none);
    }

    REQUIRE(name, gs.phase == GAME_PHASE_PERK_CHOICE, "perk phase did not start");
    REQUIRE(name, gs.perk_choice_count == 0, "perk draft should offer no options when every perk is capped");
    return true;
}

static bool test_perk_draft_emergency_life_overrides_cooldown(void) {
    const char *name = "perk_draft_emergency_life_overrides_cooldown";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.lives = 1;
    gs.run_round_time_bonus_ticks = 35 * GAME_FIXED_TICK_HZ;
    gs.run_score_mult_permille = 1800;
    gs.run_enemy_speed_penalty_fp = (3 * FP_ONE_TEST) / 4;
    gs.run_mine_capacity = 8;
    gs.run_mine_stock = 8;
    gs.perk_offer_cooldowns[PERK_LIFE_BOOST] = 2;
    gs.phase = GAME_PHASE_ROUND_CLEAR;
    gs.phase_timer_ticks = 0;

    {
        InputState none = {0};
        game_step(&gs, &none);
    }

    REQUIRE(name, gs.phase == GAME_PHASE_PERK_CHOICE, "perk phase did not start");
    REQUIRE(name, gs.perk_choice_count == 1, "emergency draft should keep a single life option");
    REQUIRE(name, gs.perk_choices[0] == PERK_LIFE_BOOST, "life perk should be forced when player is critical");
    return true;
}

static bool test_early_perk_draft_offers_mines_when_unlocked(void) {
    const char *name = "early_perk_draft_offers_mines_when_unlocked";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.lives = 3;
    gs.run_mine_capacity = 0;
    gs.run_mine_stock = 0;
    gs.phase = GAME_PHASE_ROUND_CLEAR;
    gs.phase_timer_ticks = 0;

    {
        InputState none = {0};
        game_step(&gs, &none);
    }

    REQUIRE(name, gs.phase == GAME_PHASE_PERK_CHOICE, "perk phase did not start");
    REQUIRE(name, gs.perk_choice_count > 0, "perk phase should provide at least one option");
    REQUIRE(
        name,
        perk_list_contains(gs.perk_choices, gs.perk_choice_count, PERK_MINES),
        "early perk draft should include mines when run mine capacity is zero");
    return true;
}

static bool test_direction_press_does_not_drop_mine_without_fire_release(void) {
    const char *name = "direction_press_does_not_drop_mine_without_fire_release";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.player.tile_x = 1;
    gs.player.tile_y = 4;
    gs.player.pixel_fp_x = tile_to_fp_test(1);
    gs.player.pixel_fp_y = tile_to_fp_test(4);
    gs.run_mine_capacity = 2;
    gs.run_mine_stock = 2;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 17;
    gs.enemies[0].tile_y = 9;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(17);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(9);
    gs.enemies[0].direction = DIR_NONE;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].move_remaining_fp = 0;

    {
        InputState press = {0};
        press.left = true;
        press.newest_press = DIR_LEFT;
        game_step(&gs, &press);
    }

    REQUIRE(name, !gs.mines[4][1], "direction press should not place a mine without fire release");
    REQUIRE(name, gs.run_mine_stock == 2, "mine stock should not change on direction press");
    return true;
}

static bool test_fire_release_drops_mine_and_consumes_stock(void) {
    const char *name = "fire_release_drops_mine_and_consumes_stock";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.player.tile_x = 6;
    gs.player.tile_y = 6;
    gs.player.pixel_fp_x = tile_to_fp_test(6);
    gs.player.pixel_fp_y = tile_to_fp_test(6);
    gs.run_mine_capacity = 2;
    gs.run_mine_stock = 2;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 17;
    gs.enemies[0].tile_y = 9;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(17);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(9);
    gs.enemies[0].direction = DIR_NONE;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].move_remaining_fp = 0;

    {
        InputState press = {0};
        press.fire_released = true;
        game_step(&gs, &press);
    }

    REQUIRE(name, gs.mines[6][6], "fire release should place a mine on current tile");
    REQUIRE(name, gs.run_mine_stock == 1, "mine stock did not decrement on fire release");
    return true;
}

static bool test_fire_release_while_moving_queues_mine_drop(void) {
    const char *name = "fire_release_while_moving_queues_mine_drop";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.player.tile_x = 6;
    gs.player.tile_y = 6;
    gs.player.pixel_fp_x = tile_to_fp_test(6);
    gs.player.pixel_fp_y = tile_to_fp_test(6);
    gs.run_mine_capacity = 2;
    gs.run_mine_stock = 2;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 17;
    gs.enemies[0].tile_y = 9;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(17);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(9);
    gs.enemies[0].direction = DIR_NONE;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].move_remaining_fp = 0;

    {
        InputState start_move = {0};
        start_move.right = true;
        start_move.newest_press = DIR_RIGHT;
        game_step(&gs, &start_move);
    }
    REQUIRE(name, gs.player.state == PLAYER_WALKING, "setup did not start player movement");

    {
        InputState release_fire = {0};
        release_fire.fire_released = true;
        game_step(&gs, &release_fire);
    }
    REQUIRE(name, !gs.mines[6][6], "queued mine should not drop immediately in source tile");
    REQUIRE(name, gs.run_mine_stock == 2, "queued mine should not consume stock before landing");

    for (int i = 0; i < 24 && gs.player.state == PLAYER_WALKING; ++i) {
        InputState none = {0};
        game_step(&gs, &none);
    }

    REQUIRE(name, gs.player.tile_x == 7 && gs.player.tile_y == 6, "player did not reach expected destination tile");
    REQUIRE(name, gs.mines[6][7], "queued mine did not drop on landed tile");
    REQUIRE(name, gs.run_mine_stock == 1, "queued mine drop did not consume stock on landing");
    return true;
}

static bool test_enemy_triggered_mine_explosion_kills_and_breaks_blocks(void) {
    const char *name = "enemy_triggered_mine_explosion_kills_and_breaks_blocks";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.mines[5][5] = true;
    gs.blocks[5][6] = BLOCK_ICE;
    gs.blocks[6][5] = BLOCK_SPECIAL;
    gs.blocks[5][7] = BLOCK_ICE;

    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 5;
    gs.enemies[0].tile_y = 4;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(5);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(4);
    gs.enemies[0].direction = DIR_DOWN;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].move_remaining_fp = GAME_TILE_SIZE * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 0;

    for (int i = 0; i < 32; ++i) {
        InputState none = {0};
        game_step(&gs, &none);
        if (!gs.enemies[0].alive) {
            break;
        }
    }

    REQUIRE(name, !gs.mines[5][5], "mine should be consumed by explosion");
    REQUIRE(name, !gs.enemies[0].alive, "enemy should die from mine trigger");
    REQUIRE(name, gs.blocks[5][6] == BLOCK_NONE, "ice block adjacent to mine should be destroyed");
    REQUIRE(name, gs.blocks[6][5] == BLOCK_NONE, "special block adjacent to mine should be destroyed");
    REQUIRE(name, gs.blocks[5][7] == BLOCK_ICE, "block outside mine cross radius should remain");
    return true;
}

static bool test_mine_auto_explodes_after_fuse_timeout(void) {
    const char *name = "mine_auto_explodes_after_fuse_timeout";
    GameState gs;
    bool exploded = false;
    bool explosion_event_seen = false;

    init_empty_playing_arena(&gs);
    gs.player.tile_x = 6;
    gs.player.tile_y = 6;
    gs.player.pixel_fp_x = tile_to_fp_test(6);
    gs.player.pixel_fp_y = tile_to_fp_test(6);
    gs.run_mine_capacity = 1;
    gs.run_mine_stock = 1;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_SPAWNING;
    gs.enemies[0].tile_x = 17;
    gs.enemies[0].tile_y = 9;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(17);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(9);
    gs.enemies[0].direction = DIR_NONE;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].move_remaining_fp = 0;
    gs.enemies[0].spawn_ticks = 200;

    {
        InputState press = {0};
        press.fire_released = true;
        game_step(&gs, &press);
    }

    REQUIRE(name, gs.mines[6][6], "fire press should place a timed mine");
    REQUIRE(name, gs.mine_fuse_ticks[6][6] > 0, "placed mine should start with a fuse timer");

    for (int i = 0; i < GAME_FIXED_TICK_HZ * 8; ++i) {
        InputState none = {0};
        game_step(&gs, &none);
        if ((game_consume_events(&gs) & GAME_EVENT_MINE_EXPLODED) != 0u) {
            explosion_event_seen = true;
        }
        if (!gs.mines[6][6]) {
            exploded = true;
            break;
        }
    }

    REQUIRE(name, exploded, "timed mine did not auto-explode");
    REQUIRE(name, explosion_event_seen, "timed mine explosion did not emit event");
    REQUIRE(name, gs.mine_fuse_ticks[6][6] == 0, "mine fuse should be cleared after explosion");
    return true;
}

static bool test_mine_kill_scores_less_than_slide_crush(void) {
    const char *name = "mine_kill_scores_less_than_slide_crush";
    GameState mine;
    GameState slide;
    int mine_score = 0;
    int slide_score = 0;

    init_empty_playing_arena(&mine);
    mine.mines[5][5] = true;
    mine.mine_fuse_ticks[5][5] = 12;
    mine.enemy_count = 2;

    mine.enemies[0].alive = true;
    mine.enemies[0].state = ENEMY_SPAWNING;
    mine.enemies[0].tile_x = 5;
    mine.enemies[0].tile_y = 5;
    mine.enemies[0].pixel_fp_x = tile_to_fp_test(5);
    mine.enemies[0].pixel_fp_y = tile_to_fp_test(5);
    mine.enemies[0].direction = DIR_LEFT;
    mine.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    mine.enemies[0].spawn_ticks = 1;

    mine.enemies[1].alive = true;
    mine.enemies[1].state = ENEMY_SPAWNING;
    mine.enemies[1].tile_x = 15;
    mine.enemies[1].tile_y = 8;
    mine.enemies[1].pixel_fp_x = tile_to_fp_test(15);
    mine.enemies[1].pixel_fp_y = tile_to_fp_test(8);
    mine.enemies[1].direction = DIR_LEFT;
    mine.enemies[1].speed_fp = 2 * FP_ONE_TEST;
    mine.enemies[1].spawn_ticks = 200;

    {
        InputState none = {0};
        game_step(&mine, &none);
    }

    mine_score = (int)mine.score;
    REQUIRE(name, !mine.enemies[0].alive, "mine setup did not kill target enemy");
    REQUIRE(name, mine_score > 0, "mine kill should still award points");
    REQUIRE(name, mine_score < 100, "mine kill score should be lower than baseline crush score");

    init_empty_playing_arena(&slide);
    slide.blocks[2][3] = BLOCK_ICE;
    slide.terrain[2][8] = TERRAIN_WALL;
    slide.enemy_count = 2;

    slide.enemies[0].alive = true;
    slide.enemies[0].state = ENEMY_ROAMING;
    slide.enemies[0].tile_x = 6;
    slide.enemies[0].tile_y = 2;
    slide.enemies[0].pixel_fp_x = tile_to_fp_test(6);
    slide.enemies[0].pixel_fp_y = tile_to_fp_test(2);
    slide.enemies[0].direction = DIR_LEFT;
    slide.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    slide.enemies[0].spawn_ticks = 200;

    slide.enemies[1].alive = true;
    slide.enemies[1].state = ENEMY_SPAWNING;
    slide.enemies[1].tile_x = 15;
    slide.enemies[1].tile_y = 8;
    slide.enemies[1].pixel_fp_x = tile_to_fp_test(15);
    slide.enemies[1].pixel_fp_y = tile_to_fp_test(8);
    slide.enemies[1].direction = DIR_LEFT;
    slide.enemies[1].speed_fp = 2 * FP_ONE_TEST;
    slide.enemies[1].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&slide, &in);
        if (!slide.enemies[0].alive) {
            break;
        }
    }

    slide_score = (int)slide.score;
    REQUIRE(name, !slide.enemies[0].alive, "slide setup did not kill target enemy");
    REQUIRE(name, slide_score >= 100, "slide crush should retain baseline combo score");
    REQUIRE(name, mine_score < slide_score, "mine kills should score lower than slide crushes");
    return true;
}

static bool test_stage_modifier_swarm_boosts_round_pressure(void) {
    const char *name = "stage_modifier_swarm_boosts_round_pressure";
    GameState gs;
    game_init(&gs, 0x445566u);

    game_debug_force_next_stage_modifier(&gs, STAGE_MOD_SWARM);
    game_start_round(&gs, 4);

    REQUIRE(name, gs.stage_modifier == STAGE_MOD_SWARM, "forced stage modifier was not applied");
    REQUIRE(name, gs.round_config.enemy_count == 4, "swarm modifier did not increase enemy count");
    REQUIRE(name, gs.enemy_count == gs.round_config.enemy_count, "enemy_count does not match round config");
    REQUIRE(name, gs.round_config.enemy_wanderer_count == 2, "swarm modifier did not update wanderer mix");
    REQUIRE(name, gs.round_config.aggression_percent == 38, "swarm modifier did not increase aggression");
    REQUIRE(
        name,
        gs.round_config.enemy_speed_fp == (10 * FP_ONE_TEST) / 8,
        "swarm modifier did not increase enemy speed");
    REQUIRE(
        name,
        (game_consume_events(&gs) & GAME_EVENT_STAGE_MODIFIER) != 0u,
        "stage modifier event flag not emitted");
    return true;
}

static bool test_round_scaling_increases_pressure(void) {
    const char *name = "round_scaling_increases_pressure";
    GameState early;
    GameState late;

    game_init(&early, 0x91a2u);
    game_debug_force_next_stage_modifier(&early, STAGE_MOD_NONE);
    game_start_round(&early, 3);

    game_init(&late, 0x91a2u);
    game_debug_force_next_stage_modifier(&late, STAGE_MOD_NONE);
    game_start_round(&late, 8);

    REQUIRE(name, late.round_config.enemy_count > early.round_config.enemy_count, "late round enemy count did not increase");
    REQUIRE(name, late.round_config.enemy_speed_fp > early.round_config.enemy_speed_fp, "late round enemy speed did not increase");
    REQUIRE(name, late.round_config.aggression_percent > early.round_config.aggression_percent, "late round aggression did not increase");
    REQUIRE(name, late.round_config.round_time_ticks < early.round_config.round_time_ticks, "late round timer did not tighten");
    return true;
}

static bool test_stage_modifier_shattered_converts_ice(void) {
    const char *name = "stage_modifier_shattered_converts_ice";
    GameState gs;
    game_init(&gs, 0x661199u);

    game_debug_force_next_stage_modifier(&gs, STAGE_MOD_SHATTERED_ICE);
    game_start_round(&gs, 2);

    REQUIRE(name, gs.stage_modifier == STAGE_MOD_SHATTERED_ICE, "forced shattered modifier was not applied");
    REQUIRE(name, count_blocks_of_type(&gs, BLOCK_CRACKED) > 0, "shattered modifier did not convert any ice blocks");
    REQUIRE(name, count_blocks_of_type(&gs, BLOCK_ICE) > 0, "shattered modifier removed all ice blocks");
    return true;
}

static bool test_stage_modifier_black_ice_adds_blocks(void) {
    const char *name = "stage_modifier_black_ice_adds_blocks";
    GameState baseline;
    GameState modded;

    game_init(&baseline, 0x7722aau);
    game_debug_force_next_stage_modifier(&baseline, STAGE_MOD_NONE);
    game_start_round(&baseline, 2);
    const int baseline_blocks = count_non_empty_blocks(&baseline);

    game_init(&modded, 0x7722aau);
    game_debug_force_next_stage_modifier(&modded, STAGE_MOD_BLACK_ICE);
    game_start_round(&modded, 2);
    const int modded_blocks = count_non_empty_blocks(&modded);

    REQUIRE(name, modded.stage_modifier == STAGE_MOD_BLACK_ICE, "forced black-ice modifier was not applied");
    REQUIRE(name, modded_blocks > baseline_blocks, "black-ice modifier did not add block pressure");
    return true;
}

static bool test_stage_modifier_turbo_slide_speeds_blocks(void) {
    const char *name = "stage_modifier_turbo_slide_speeds_blocks";
    GameState baseline;
    GameState turbo;
    InputState push_right = {0};

    push_right.right = true;
    push_right.newest_press = DIR_RIGHT;

    init_empty_playing_arena(&baseline);
    baseline.stage_modifier = STAGE_MOD_NONE;
    baseline.blocks[2][3] = BLOCK_ICE;
    baseline.terrain[2][8] = TERRAIN_WALL;
    game_step(&baseline, &push_right);

    const int baseline_idx = first_active_moving_block(&baseline);
    REQUIRE(name, baseline_idx >= 0, "baseline moving block did not spawn");

    init_empty_playing_arena(&turbo);
    turbo.stage_modifier = STAGE_MOD_TURBO_SLIDE;
    turbo.blocks[2][3] = BLOCK_ICE;
    turbo.terrain[2][8] = TERRAIN_WALL;
    game_step(&turbo, &push_right);

    const int turbo_idx = first_active_moving_block(&turbo);
    REQUIRE(name, turbo_idx >= 0, "turbo moving block did not spawn");

    REQUIRE(
        name,
        turbo.moving_blocks[turbo_idx].speed_fp > baseline.moving_blocks[baseline_idx].speed_fp,
        "turbo-slide modifier did not increase block speed");
    REQUIRE(
        name,
        turbo.moving_blocks[turbo_idx].startup_ticks < baseline.moving_blocks[baseline_idx].startup_ticks,
        "turbo-slide modifier did not reduce block startup delay");
    REQUIRE(
        name,
        turbo.moving_blocks[turbo_idx].intra_fp > baseline.moving_blocks[baseline_idx].intra_fp,
        "turbo-slide modifier did not advance block movement on first tick");
    return true;
}

static bool test_stage_modifier_frost_siren_increases_time_pressure(void) {
    const char *name = "stage_modifier_frost_siren_increases_time_pressure";
    GameState baseline;
    GameState siren;

    game_init(&baseline, 0x3399aau);
    game_debug_force_next_stage_modifier(&baseline, STAGE_MOD_NONE);
    game_start_round(&baseline, 4);

    game_init(&siren, 0x3399aau);
    game_debug_force_next_stage_modifier(&siren, STAGE_MOD_FROST_SIREN);
    game_start_round(&siren, 4);

    REQUIRE(name, siren.stage_modifier == STAGE_MOD_FROST_SIREN, "forced frost-siren modifier was not applied");
    REQUIRE(
        name,
        siren.round_config.round_time_ticks < baseline.round_config.round_time_ticks,
        "frost-siren modifier did not increase time pressure");
    REQUIRE(
        name,
        siren.round_config.aggression_percent > baseline.round_config.aggression_percent,
        "frost-siren modifier did not increase aggression");
    return true;
}

static bool test_stage_modifier_frost_siren_accelerates_danger_pulses(void) {
    const char *name = "stage_modifier_frost_siren_accelerates_danger_pulses";
    GameState baseline;
    GameState siren;
    InputState none = {0};
    int baseline_pulses = 0;
    int siren_pulses = 0;

    init_empty_playing_arena(&baseline);
    init_empty_playing_arena(&siren);

    baseline.stage_modifier = STAGE_MOD_NONE;
    siren.stage_modifier = STAGE_MOD_FROST_SIREN;
    baseline.round_time_ticks = 20 * GAME_FIXED_TICK_HZ;
    siren.round_time_ticks = 20 * GAME_FIXED_TICK_HZ;

    baseline.enemy_count = 1;
    baseline.enemies[0].alive = true;
    baseline.enemies[0].state = ENEMY_SPAWNING;
    baseline.enemies[0].tile_x = 15;
    baseline.enemies[0].tile_y = 8;
    baseline.enemies[0].pixel_fp_x = tile_to_fp_test(15);
    baseline.enemies[0].pixel_fp_y = tile_to_fp_test(8);
    baseline.enemies[0].direction = DIR_LEFT;
    baseline.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    baseline.enemies[0].spawn_ticks = 200;

    siren.enemy_count = 1;
    siren.enemies[0].alive = true;
    siren.enemies[0].state = ENEMY_SPAWNING;
    siren.enemies[0].tile_x = 15;
    siren.enemies[0].tile_y = 8;
    siren.enemies[0].pixel_fp_x = tile_to_fp_test(15);
    siren.enemies[0].pixel_fp_y = tile_to_fp_test(8);
    siren.enemies[0].direction = DIR_LEFT;
    siren.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    siren.enemies[0].spawn_ticks = 200;

    for (int i = 0; i < 200; ++i) {
        game_step(&baseline, &none);
        game_step(&siren, &none);
        if ((game_consume_events(&baseline) & GAME_EVENT_TIMER_DANGER_PULSE) != 0u) {
            ++baseline_pulses;
        }
        if ((game_consume_events(&siren) & GAME_EVENT_TIMER_DANGER_PULSE) != 0u) {
            ++siren_pulses;
        }
    }

    REQUIRE(name, !baseline.timer_danger_active, "baseline should not be in danger state this early");
    REQUIRE(name, siren.timer_danger_active, "frost-siren should activate danger state earlier");
    REQUIRE(name, siren_pulses > baseline_pulses, "frost-siren did not emit more danger pulses");
    REQUIRE(name, siren_pulses >= 2, "frost-siren pulse cadence was lower than expected");
    return true;
}

static bool test_bonus_item_spawns_after_crush_threshold(void) {
    const char *name = "bonus_item_spawns_after_crush_threshold";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][10] = TERRAIN_WALL;
    gs.enemy_count = 4;
    for (int i = 0; i < 3; ++i) {
        gs.enemies[i].alive = true;
        gs.enemies[i].state = ENEMY_SPAWNING;
        gs.enemies[i].tile_x = 5 + i;
        gs.enemies[i].tile_y = 2;
        gs.enemies[i].pixel_fp_x = tile_to_fp_test(5 + i);
        gs.enemies[i].pixel_fp_y = tile_to_fp_test(2);
        gs.enemies[i].direction = DIR_LEFT;
        gs.enemies[i].speed_fp = 2 * FP_ONE_TEST;
        gs.enemies[i].spawn_ticks = 200;
    }
    gs.enemies[3].alive = true;
    gs.enemies[3].state = ENEMY_SPAWNING;
    gs.enemies[3].tile_x = 15;
    gs.enemies[3].tile_y = 8;
    gs.enemies[3].pixel_fp_x = tile_to_fp_test(15);
    gs.enemies[3].pixel_fp_y = tile_to_fp_test(8);
    gs.enemies[3].direction = DIR_LEFT;
    gs.enemies[3].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[3].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
    }

    REQUIRE(name, count_items_on_map(&gs) >= 1, "bonus item did not spawn after crush threshold");
    REQUIRE(name, gs.bonus_item_timer_ticks > 0, "bonus item timer was not started");
    return true;
}

static bool test_bonus_item_threshold_lower_in_early_round(void) {
    const char *name = "bonus_item_threshold_lower_in_early_round";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.round = 1;
    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][10] = TERRAIN_WALL;
    gs.enemy_count = 3;
    for (int i = 0; i < 2; ++i) {
        gs.enemies[i].alive = true;
        gs.enemies[i].state = ENEMY_SPAWNING;
        gs.enemies[i].tile_x = 5 + i;
        gs.enemies[i].tile_y = 2;
        gs.enemies[i].pixel_fp_x = tile_to_fp_test(5 + i);
        gs.enemies[i].pixel_fp_y = tile_to_fp_test(2);
        gs.enemies[i].direction = DIR_LEFT;
        gs.enemies[i].speed_fp = 2 * FP_ONE_TEST;
        gs.enemies[i].spawn_ticks = 200;
    }
    gs.enemies[2].alive = true;
    gs.enemies[2].state = ENEMY_SPAWNING;
    gs.enemies[2].tile_x = 15;
    gs.enemies[2].tile_y = 8;
    gs.enemies[2].pixel_fp_x = tile_to_fp_test(15);
    gs.enemies[2].pixel_fp_y = tile_to_fp_test(8);
    gs.enemies[2].direction = DIR_LEFT;
    gs.enemies[2].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[2].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
    }

    REQUIRE(name, gs.kills_this_round >= 2, "setup did not produce two crushes in one slide");
    REQUIRE(name, count_items_on_map(&gs) >= 1, "early-round threshold did not spawn a bonus item");
    return true;
}

static bool test_bonus_item_threshold_tightens_in_late_rounds(void) {
    const char *name = "bonus_item_threshold_tightens_in_late_rounds";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.round = 6;
    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][10] = TERRAIN_WALL;
    gs.enemy_count = 4;
    for (int i = 0; i < 3; ++i) {
        gs.enemies[i].alive = true;
        gs.enemies[i].state = ENEMY_SPAWNING;
        gs.enemies[i].tile_x = 5 + i;
        gs.enemies[i].tile_y = 2;
        gs.enemies[i].pixel_fp_x = tile_to_fp_test(5 + i);
        gs.enemies[i].pixel_fp_y = tile_to_fp_test(2);
        gs.enemies[i].direction = DIR_LEFT;
        gs.enemies[i].speed_fp = 2 * FP_ONE_TEST;
        gs.enemies[i].spawn_ticks = 200;
    }
    gs.enemies[3].alive = true;
    gs.enemies[3].state = ENEMY_SPAWNING;
    gs.enemies[3].tile_x = 15;
    gs.enemies[3].tile_y = 8;
    gs.enemies[3].pixel_fp_x = tile_to_fp_test(15);
    gs.enemies[3].pixel_fp_y = tile_to_fp_test(8);
    gs.enemies[3].direction = DIR_LEFT;
    gs.enemies[3].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[3].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
    }

    REQUIRE(name, gs.kills_this_round >= 3, "setup did not produce three crushes in one slide");
    REQUIRE(name, count_items_on_map(&gs) == 0, "late-round threshold should require more crushes before spawning an item");
    return true;
}

static bool test_special_alignment_awards_bonus(void) {
    const char *name = "special_alignment_awards_bonus";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_SPAWNING;
    gs.enemies[0].tile_x = 16;
    gs.enemies[0].tile_y = 9;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(16);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(9);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    gs.blocks[4][4] = BLOCK_SPECIAL;
    gs.blocks[4][6] = BLOCK_SPECIAL;
    gs.blocks[4][8] = BLOCK_SPECIAL;

    gs.moving_blocks[0].active = true;
    gs.moving_blocks[0].type = BLOCK_ICE;
    gs.moving_blocks[0].tile_x = 2;
    gs.moving_blocks[0].tile_y = 2;
    gs.moving_blocks[0].pixel_fp_x = tile_to_fp_test(2);
    gs.moving_blocks[0].pixel_fp_y = tile_to_fp_test(2);
    gs.moving_blocks[0].direction = DIR_RIGHT;
    gs.moving_blocks[0].speed_fp = 4 * FP_ONE_TEST;
    gs.moving_blocks[0].startup_ticks = 0;
    gs.moving_blocks[0].intra_fp = 16 * FP_ONE_TEST;
    gs.terrain[2][3] = TERRAIN_WALL;

    const uint32_t before = gs.score;
    InputState none = {0};
    game_step(&gs, &none);

    REQUIRE(name, gs.special_alignment_awarded, "special alignment event did not trigger");
    REQUIRE(name, gs.score >= before + 1000u, "special alignment did not grant bonus score");
    REQUIRE(name, gs.special_alignment_flash_ticks > 0, "special alignment flash timer not set");
    REQUIRE(name, count_items_on_map(&gs) >= 1, "special alignment did not spawn a bonus item");
    return true;
}

static bool test_round_enemy_mix_includes_wanderers(void) {
    const char *name = "round_enemy_mix_includes_wanderers";
    GameState gs;
    game_init(&gs, 0x55aa1234u);

    REQUIRE(name, game_debug_count_enemy_type(&gs, ENEMY_TYPE_WANDERER) == 0, "round 1 should not spawn wanderers");
    REQUIRE(
        name,
        game_debug_count_enemy_type(&gs, ENEMY_TYPE_CHASER) == gs.enemy_count,
        "round 1 should spawn only chasers");

    game_start_round(&gs, 6);
    const int wanderer_count = game_debug_count_enemy_type(&gs, ENEMY_TYPE_WANDERER);
    const int chaser_count = game_debug_count_enemy_type(&gs, ENEMY_TYPE_CHASER);
    REQUIRE(name, wanderer_count > 0, "later rounds should include wanderers");
    REQUIRE(name, wanderer_count == gs.round_config.enemy_wanderer_count, "wanderer mix does not match round config");
    REQUIRE(name, wanderer_count + chaser_count == gs.enemy_count, "enemy type totals do not match enemy count");
    return true;
}

static bool test_enemy_decision_cooldown_delays_first_step(void) {
    const char *name = "enemy_decision_cooldown_delays_first_step";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 10;
    gs.enemies[0].tile_y = 6;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(10);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(6);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].move_remaining_fp = 0;
    gs.enemies[0].spawn_ticks = 0;
    gs.enemies[0].decision_cooldown_ticks = 3;

    {
        InputState none = {0};
        game_step(&gs, &none);
    }

    REQUIRE(name, gs.enemies[0].tile_x == 10 && gs.enemies[0].tile_y == 6, "enemy moved before cooldown elapsed");
    REQUIRE(name, gs.enemies[0].move_remaining_fp == 0, "enemy started movement during cooldown");
    REQUIRE(name, gs.enemies[0].decision_cooldown_ticks == 2, "enemy cooldown did not decrement by one tick");
    return true;
}

static bool test_level_rows_validation_rules(void) {
    const char *name = "level_rows_validation_rules";

    const char valid_rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1] = {
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
    };

    char bad_adjacent_enemy[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1];
    memcpy(bad_adjacent_enemy, valid_rows, sizeof(bad_adjacent_enemy));
    bad_adjacent_enemy[2][3] = 'E'; /* Adjacent to P at [2][2]. */

    char bad_no_player[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1];
    memcpy(bad_no_player, valid_rows, sizeof(bad_no_player));
    bad_no_player[2][2] = '.';

    REQUIRE(name, game_debug_validate_level_rows(valid_rows), "valid level rows were rejected");
    REQUIRE(
        name,
        !game_debug_validate_level_rows(bad_adjacent_enemy),
        "level with adjacent enemy should fail validation");
    REQUIRE(name, !game_debug_validate_level_rows(bad_no_player), "level without player should fail validation");
    return true;
}

static bool test_procedural_round_generation_invariants(void) {
    const char *name = "procedural_round_generation_invariants";
    const uint32_t seeds[] = {0x1234u, 0x445566u, 0x9911u, 0xabcdu};
    static const int kDx[5] = {0, 0, 0, -1, 1};
    static const int kDy[5] = {0, -1, 1, 0, 0};

    for (int si = 0; si < (int)(sizeof(seeds) / sizeof(seeds[0])); ++si) {
        GameState gs;
        game_init(&gs, seeds[si]);

        for (int round = 1; round <= 10; ++round) {
            int exits = 0;
            int lane_blocks = 0;
            int px = 0;
            int py = 0;
            int special_count = 0;
            int pushable_count = 0;
            int unbreakable_count = 0;
            int mine_anchor_opportunities = 0;
            int reachable_floor = 0;
            int reachable_junctions = 0;
            int nearest_enemy_dist = GAME_GRID_WIDTH + GAME_GRID_HEIGHT;
            const int min_reachable_floor = (round <= 4) ? 40 : 42;
            const int min_reachable_junctions = (round <= 5) ? 6 : 5;
            const int min_mine_anchor_opportunities = (round <= 6) ? 3 : 4;
            const int min_unbreakable_count = (round <= 2) ? 2 : ((round <= 5) ? 3 : 4);

            game_start_round(&gs, round);

            px = gs.player_spawn_x;
            py = gs.player_spawn_y;
            special_count = count_blocks_of_type(&gs, BLOCK_SPECIAL);
            pushable_count =
                count_blocks_of_type(&gs, BLOCK_ICE) +
                count_blocks_of_type(&gs, BLOCK_SPECIAL) +
                count_blocks_of_type(&gs, BLOCK_CRACKED);
            unbreakable_count = count_blocks_of_type(&gs, BLOCK_UNBREAKABLE);

            if (special_count < 3) {
                fprintf(
                    stderr,
                    "[FAIL] %s: seed=%u round=%d specials=%d template_idx=%d stage_mod=%d\n",
                    name,
                    seeds[si],
                    round,
                    special_count,
                    game_debug_last_procgen_template_index(&gs),
                    (int)gs.stage_modifier);
                return false;
            }

            REQUIRE(name, special_count >= 3, "generated level should keep at least three special blocks");
            REQUIRE(name, pushable_count >= 8, "generated level should keep enough pushable block density");
            REQUIRE(
                name,
                unbreakable_count >= min_unbreakable_count,
                "generated level should include visible immovable-unbreakable landmarks");

            for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
                const int nx = px + kDx[d];
                const int ny = py + kDy[d];
                if (nx <= 0 || ny <= 0 || nx >= GAME_GRID_WIDTH - 1 || ny >= GAME_GRID_HEIGHT - 1) {
                    continue;
                }
                if (gs.terrain[ny][nx] == TERRAIN_FLOOR && gs.blocks[ny][nx] == BLOCK_NONE) {
                    ++exits;
                }
            }
            REQUIRE(name, exits >= 2, "player spawn should have at least two immediate floor exits");

            for (int i = 0; i < gs.enemy_count; ++i) {
                const Enemy *enemy = &gs.enemies[i];
                if (!enemy->alive) {
                    continue;
                }
                {
                    const int dist = abs(enemy->tile_x - px) + abs(enemy->tile_y - py);
                    if (dist < nearest_enemy_dist) {
                        nearest_enemy_dist = dist;
                    }
                }
                REQUIRE(
                    name,
                    abs(enemy->tile_x - px) + abs(enemy->tile_y - py) > 1,
                    "enemy spawned adjacent to player");
            }
            REQUIRE(name, nearest_enemy_dist >= 4, "enemy spawn should stay well away from protected opening");
            REQUIRE(name, spawn_has_nearby_push_option_test(&gs), "spawn should expose a nearby push option");

            for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
                for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
                    if (block_has_slide_lane(&gs, x, y, 2)) {
                        ++lane_blocks;
                    }
                }
            }
            REQUIRE(name, lane_blocks >= 4, "generated level should expose several slide lanes");

            mine_anchor_opportunities = count_mine_anchor_opportunities_test(&gs);
            REQUIRE(
                name,
                mine_anchor_opportunities >= min_mine_anchor_opportunities,
                "generated level should expose mine-friendly blast pockets");

            analyze_reachable_floor_region_test(&gs, px, py, &reachable_floor, &reachable_junctions);
            REQUIRE(
                name,
                reachable_floor >= min_reachable_floor,
                "generated level should expose enough connected floor around spawn");
            REQUIRE(
                name,
                reachable_junctions >= min_reachable_junctions,
                "generated level should expose enough branching routes");
        }
    }

    return true;
}

static bool test_procgen_wfc_templates_validate(void) {
    const char *name = "procgen_wfc_templates_validate";
    const int template_count = game_debug_procgen_template_count();
    char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1];
    char level_rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1];
    int player_x = -1;
    int player_y = -1;
    int enemy_x = -1;
    int enemy_y = -1;

    REQUIRE(name, template_count > 0, "expected generated WFC templates");
    for (int i = 0; i < template_count; ++i) {
        REQUIRE(name, game_debug_get_procgen_template_rows(i, rows), "failed to fetch template rows");
        memcpy(level_rows, rows, sizeof(level_rows));
        REQUIRE(
            name,
            choose_spawn_candidate_rows(rows, &player_x, &player_y),
            "template missing viable spawn candidate");
        REQUIRE(
            name,
            choose_enemy_candidate_rows(rows, player_x, player_y, &enemy_x, &enemy_y),
            "template missing viable enemy candidate");
        level_rows[player_y][player_x] = 'P';
        level_rows[enemy_y][enemy_x] = 'E';
        REQUIRE(
            name,
            game_debug_validate_level_rows(level_rows),
            "embedded WFC template failed reconstructed row validation");
    }
    return true;
}

static bool test_procgen_template_selection_avoids_recent_repetition(void) {
    const char *name = "procgen_template_selection_avoids_recent_repetition";
    GameState gs;
    int template_ids[4] = {-1, -1, -1, -1};
    int chosen = -1;
    const int template_count = game_debug_procgen_template_count();
    const int viable = find_viable_procgen_templates_for_round(4, template_ids, 4);

    REQUIRE(name, template_count >= 4, "need at least four templates for non-repetition smoke test");
    REQUIRE(name, viable >= 4, "not enough viable templates found for non-repetition smoke test");

    game_init(&gs, 0x42bad1deu);
    for (int i = 0; i < 3; ++i) {
        game_debug_force_next_procgen_template(&gs, template_ids[i]);
        game_start_round(&gs, i + 1);
        REQUIRE(
            name,
            game_debug_last_procgen_template_index(&gs) == template_ids[i],
            "forced template selection did not apply");
    }

    game_start_round(&gs, 4);
    chosen = game_debug_last_procgen_template_index(&gs);
    if (chosen >= 0) {
        REQUIRE(
            name,
            chosen != template_ids[0] && chosen != template_ids[1] && chosen != template_ids[2],
            "unforced template repeated one of the three most recent templates");
    }
    return true;
}

static bool test_procgen_template_invalid_force_falls_back_to_valid_level(void) {
    const char *name = "procgen_template_invalid_force_falls_back_to_valid_level";
    GameState gs;
    char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1];
    const int template_count = game_debug_procgen_template_count();
    const int invalid_template_index = template_count + 99;

    game_init(&gs, 0x66cc22u);
    game_debug_force_next_procgen_template(&gs, invalid_template_index);
    game_start_round(&gs, 5);

    REQUIRE(
        name,
        game_debug_last_procgen_template_index(&gs) == -1,
        "invalid forced template index should fall back to non-template generation");
    build_level_rows_from_state_test(&gs, rows);
    REQUIRE(
        name,
        game_debug_validate_level_rows(rows),
        "fallback generation path produced an invalid level layout");
    return true;
}

static bool test_timer_danger_state_pulses_near_timeout(void) {
    const char *name = "timer_danger_state_pulses_near_timeout";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.round_time_ticks = 4 * GAME_FIXED_TICK_HZ;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_SPAWNING;
    gs.enemies[0].tile_x = 15;
    gs.enemies[0].tile_y = 8;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(15);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(8);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    InputState none = {0};
    game_step(&gs, &none);
    REQUIRE(name, gs.timer_danger_active, "danger state did not activate near timeout");
    REQUIRE(name, gs.timer_danger_pulse_ticks > 0, "danger pulse ticks were not set");
    return true;
}

static bool test_combo_crush_spawns_score_popup(void) {
    const char *name = "combo_crush_spawns_score_popup";
    GameState gs;
    init_empty_playing_arena(&gs);
    bool popup_seen = false;

    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][8] = TERRAIN_WALL;
    gs.enemy_count = 2;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 6;
    gs.enemies[0].tile_y = 2;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(6);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(2);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    gs.enemies[1].alive = true;
    gs.enemies[1].state = ENEMY_SPAWNING;
    gs.enemies[1].tile_x = 12;
    gs.enemies[1].tile_y = 9;
    gs.enemies[1].pixel_fp_x = tile_to_fp_test(12);
    gs.enemies[1].pixel_fp_y = tile_to_fp_test(9);
    gs.enemies[1].direction = DIR_LEFT;
    gs.enemies[1].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[1].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
        if (count_active_popups(&gs) > 0) {
            popup_seen = true;
            break;
        }
    }

    REQUIRE(name, popup_seen, "no score popup spawned on crush");
    for (int i = 0; i < GAME_MAX_SCORE_POPUPS; ++i) {
        if (!gs.score_popups[i].active) {
            continue;
        }
        REQUIRE(name, gs.score_popups[i].value >= 100, "score popup value should be combo score");
        REQUIRE(name, gs.score_popups[i].ttl_ticks > 0, "score popup ttl should be active");
        return true;
    }
    REQUIRE(name, false, "no active popup found after crush");
    return false;
}

static bool test_popup_spawn_offsets_from_existing_cluster(void) {
    const char *name = "popup_spawn_offsets_from_existing_cluster";
    GameState gs;
    init_empty_playing_arena(&gs);
    const int target_x = 4;
    const int target_y = 2;
    const int anchor_fp_x = tile_to_fp_test(target_x) + ((GAME_TILE_SIZE / 2) * FP_ONE_TEST);
    const int anchor_fp_y = tile_to_fp_test(target_y) + ((GAME_TILE_SIZE / 2) * FP_ONE_TEST);
    const int sentinel_value = 7777;

    gs.score_popups[0].active = true;
    gs.score_popups[0].ttl_ticks = 1000;
    gs.score_popups[0].value = sentinel_value;
    gs.score_popups[0].pixel_fp_x = anchor_fp_x;
    gs.score_popups[0].pixel_fp_y = anchor_fp_y;

    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_SPAWNING;
    gs.enemies[0].tile_x = target_x;
    gs.enemies[0].tile_y = target_y;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(target_x);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(target_y);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    gs.terrain[target_y][target_x + 1] = TERRAIN_WALL;
    gs.moving_blocks[0].active = true;
    gs.moving_blocks[0].type = BLOCK_ICE;
    gs.moving_blocks[0].direction = DIR_RIGHT;
    gs.moving_blocks[0].startup_ticks = 0;
    gs.moving_blocks[0].speed_fp = tile_to_fp_test(1);
    gs.moving_blocks[0].intra_fp = 0;
    gs.moving_blocks[0].combo_count = 0;
    gs.moving_blocks[0].tile_x = target_x - 1;
    gs.moving_blocks[0].tile_y = target_y;
    gs.moving_blocks[0].pixel_fp_x = tile_to_fp_test(target_x - 1);
    gs.moving_blocks[0].pixel_fp_y = tile_to_fp_test(target_y);

    {
        InputState none = {0};
        game_step(&gs, &none);
    }

    REQUIRE(name, !gs.enemies[0].alive, "setup did not crush enemy for popup offset test");

    {
        int active_count = 0;
        int new_idx = -1;
        for (int i = 0; i < GAME_MAX_SCORE_POPUPS; ++i) {
            const ScorePopup *popup = &gs.score_popups[i];
            if (!popup->active) {
                continue;
            }
            ++active_count;
            if (popup->value != sentinel_value) {
                new_idx = i;
            }
        }
        REQUIRE(name, active_count >= 2, "expected existing + new popup cluster");
        REQUIRE(name, new_idx >= 0, "failed to locate spawned popup");
        REQUIRE(
            name,
            gs.score_popups[new_idx].pixel_fp_x != anchor_fp_x ||
                gs.score_popups[new_idx].pixel_fp_y != anchor_fp_y,
            "new popup should offset away from existing popup anchor");
        REQUIRE(
            name,
            abs(gs.score_popups[new_idx].pixel_fp_x - anchor_fp_x) +
                    abs(gs.score_popups[new_idx].pixel_fp_y - anchor_fp_y) >=
                2 * FP_ONE_TEST,
            "new popup offset should provide visible separation");
    }

    return true;
}

static bool test_score_caps_without_wrapping(void) {
    const char *name = "score_caps_without_wrapping";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.score = 99999990u;
    gs.run_score_mult_permille = 1800;
    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][8] = TERRAIN_WALL;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 6;
    gs.enemies[0].tile_y = 2;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(6);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(2);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
        if (!gs.enemies[0].alive) {
            break;
        }
    }

    REQUIRE(name, !gs.enemies[0].alive, "setup did not crush enemy for score cap test");
    REQUIRE(name, gs.score == 99999999u, "score should cap at 99,999,999 without wrapping");
    return true;
}

static bool test_timer_danger_event_emits_and_consumes(void) {
    const char *name = "timer_danger_event_emits_and_consumes";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.round_time_ticks = 4 * GAME_FIXED_TICK_HZ;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_SPAWNING;
    gs.enemies[0].tile_x = 15;
    gs.enemies[0].tile_y = 8;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(15);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(8);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    InputState none = {0};
    game_step(&gs, &none);

    {
        const uint32_t events = game_consume_events(&gs);
        REQUIRE(
            name,
            (events & GAME_EVENT_TIMER_DANGER_PULSE) != 0u,
            "danger pulse event flag was not emitted");
    }
    REQUIRE(name, game_consume_events(&gs) == 0u, "event consume should clear pending flags");
    return true;
}

static bool test_crush_event_flag_emits(void) {
    const char *name = "crush_event_flag_emits";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][8] = TERRAIN_WALL;
    gs.enemy_count = 2;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 6;
    gs.enemies[0].tile_y = 2;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(6);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(2);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    gs.enemies[1].alive = true;
    gs.enemies[1].state = ENEMY_SPAWNING;
    gs.enemies[1].tile_x = 14;
    gs.enemies[1].tile_y = 9;
    gs.enemies[1].pixel_fp_x = tile_to_fp_test(14);
    gs.enemies[1].pixel_fp_y = tile_to_fp_test(9);
    gs.enemies[1].direction = DIR_LEFT;
    gs.enemies[1].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[1].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
    }

    {
        const uint32_t events = game_consume_events(&gs);
        REQUIRE(name, (events & GAME_EVENT_CRUSH) != 0u, "crush event flag was not emitted");
        REQUIRE(name, (events & GAME_EVENT_COMBO) == 0u, "single crush should not emit combo event flag");
    }
    return true;
}

static bool test_combo_event_flag_emits(void) {
    const char *name = "combo_event_flag_emits";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][10] = TERRAIN_WALL;
    gs.enemy_count = 3;
    for (int i = 0; i < 2; ++i) {
        gs.enemies[i].alive = true;
        gs.enemies[i].state = ENEMY_SPAWNING;
        gs.enemies[i].tile_x = 5 + i;
        gs.enemies[i].tile_y = 2;
        gs.enemies[i].pixel_fp_x = tile_to_fp_test(5 + i);
        gs.enemies[i].pixel_fp_y = tile_to_fp_test(2);
        gs.enemies[i].direction = DIR_LEFT;
        gs.enemies[i].speed_fp = 2 * FP_ONE_TEST;
        gs.enemies[i].spawn_ticks = 200;
    }
    gs.enemies[2].alive = true;
    gs.enemies[2].state = ENEMY_SPAWNING;
    gs.enemies[2].tile_x = 15;
    gs.enemies[2].tile_y = 8;
    gs.enemies[2].pixel_fp_x = tile_to_fp_test(15);
    gs.enemies[2].pixel_fp_y = tile_to_fp_test(8);
    gs.enemies[2].direction = DIR_LEFT;
    gs.enemies[2].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[2].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
    }

    {
        const uint32_t events = game_consume_events(&gs);
        REQUIRE(name, (events & GAME_EVENT_CRUSH) != 0u, "combo setup should still emit crush event flag");
        REQUIRE(name, (events & GAME_EVENT_COMBO) != 0u, "combo crush should emit combo event flag");
    }
    return true;
}

static bool test_block_impact_event_emits_on_slide_stop(void) {
    const char *name = "block_impact_event_emits_on_slide_stop";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][6] = TERRAIN_WALL;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_SPAWNING;
    gs.enemies[0].tile_x = 14;
    gs.enemies[0].tile_y = 9;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(14);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(9);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    for (int i = 0; i < PUSH_IMPACT_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
    }

    {
        const uint32_t events = game_consume_events(&gs);
        REQUIRE(name, (events & GAME_EVENT_BLOCK_IMPACT) != 0u, "block impact event flag was not emitted");
    }
    return true;
}

static bool test_slide_lookahead_stops_before_blocked_cell(void) {
    const char *name = "slide_lookahead_stops_before_blocked_cell";
    GameState gs;
    init_empty_playing_arena(&gs);

    gs.terrain[4][5] = TERRAIN_WALL;
    gs.moving_blocks[0].active = true;
    gs.moving_blocks[0].type = BLOCK_ICE;
    gs.moving_blocks[0].tile_x = 4;
    gs.moving_blocks[0].tile_y = 4;
    gs.moving_blocks[0].pixel_fp_x = tile_to_fp_test(4) + (8 * FP_ONE_TEST);
    gs.moving_blocks[0].pixel_fp_y = tile_to_fp_test(4);
    gs.moving_blocks[0].direction = DIR_RIGHT;
    gs.moving_blocks[0].speed_fp = 2 * FP_ONE_TEST;
    gs.moving_blocks[0].startup_ticks = 0;
    gs.moving_blocks[0].intra_fp = 8 * FP_ONE_TEST;
    gs.moving_blocks[0].combo_count = 0;
    gs.blocks[4][4] = BLOCK_NONE;
    gs.enemy_count = 0;

    {
        InputState none = {0};
        game_step(&gs, &none);
    }

    REQUIRE(name, !gs.moving_blocks[0].active, "lookahead stop should deactivate block before blocked cell");
    REQUIRE(name, gs.moving_blocks[0].tile_x == 4 && gs.moving_blocks[0].tile_y == 4, "block tile should remain in previous cell");
    REQUIRE(name, gs.moving_blocks[0].intra_fp == 0, "lookahead stop should clear residual intra progress");
    REQUIRE(name, gs.blocks[4][4] == BLOCK_ICE, "stopped block should be restored on previous tile");
    {
        const uint32_t events = game_consume_events(&gs);
        REQUIRE(name, (events & GAME_EVENT_BLOCK_IMPACT) != 0u, "lookahead stop should emit block-impact event");
    }
    return true;
}

static bool test_crush_spawns_impact_fx(void) {
    const char *name = "crush_spawns_impact_fx";
    GameState gs;
    init_empty_playing_arena(&gs);
    bool impact_seen = false;

    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][8] = TERRAIN_WALL;
    gs.enemy_count = 2;
    gs.enemies[0].alive = true;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 6;
    gs.enemies[0].tile_y = 2;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(6);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(2);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    gs.enemies[1].alive = true;
    gs.enemies[1].state = ENEMY_SPAWNING;
    gs.enemies[1].tile_x = 14;
    gs.enemies[1].tile_y = 9;
    gs.enemies[1].pixel_fp_x = tile_to_fp_test(14);
    gs.enemies[1].pixel_fp_y = tile_to_fp_test(9);
    gs.enemies[1].direction = DIR_LEFT;
    gs.enemies[1].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[1].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
        if (count_active_impact_fx(&gs) > 0) {
            impact_seen = true;
            break;
        }
    }

    REQUIRE(name, impact_seen, "no impact fx spawned after crush/stop");
    return true;
}

static bool test_enemy_death_fx_styles_spawn_by_enemy_type(void) {
    const char *name = "enemy_death_fx_styles_spawn_by_enemy_type";
    GameState gs;
    bool chaser_fx_seen = false;
    bool wanderer_fx_seen = false;
    int chaser_base_ttl = 0;
    int wanderer_base_ttl = 0;

    init_empty_playing_arena(&gs);
    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][8] = TERRAIN_WALL;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].type = ENEMY_TYPE_CHASER;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 6;
    gs.enemies[0].tile_y = 2;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(6);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(2);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
        if (has_active_impact_fx_style(&gs, IMPACT_FX_STYLE_ENEMY_DEATH_A, &chaser_base_ttl)) {
            chaser_fx_seen = true;
            break;
        }
    }

    REQUIRE(name, chaser_fx_seen, "chaser crush did not spawn enemy death style A fx");
    REQUIRE(name, chaser_base_ttl == 16, "enemy death style A ttl changed unexpectedly");

    init_empty_playing_arena(&gs);
    gs.blocks[2][3] = BLOCK_ICE;
    gs.terrain[2][8] = TERRAIN_WALL;
    gs.enemy_count = 1;
    gs.enemies[0].alive = true;
    gs.enemies[0].type = ENEMY_TYPE_WANDERER;
    gs.enemies[0].state = ENEMY_ROAMING;
    gs.enemies[0].tile_x = 6;
    gs.enemies[0].tile_y = 2;
    gs.enemies[0].pixel_fp_x = tile_to_fp_test(6);
    gs.enemies[0].pixel_fp_y = tile_to_fp_test(2);
    gs.enemies[0].direction = DIR_LEFT;
    gs.enemies[0].speed_fp = 2 * FP_ONE_TEST;
    gs.enemies[0].spawn_ticks = 200;

    for (int i = 0; i < PUSH_SLIDE_TEST_TICKS; ++i) {
        InputState in = {0};
        in.right = true;
        in.newest_press = (i == 0) ? DIR_RIGHT : DIR_NONE;
        game_step(&gs, &in);
        if (has_active_impact_fx_style(&gs, IMPACT_FX_STYLE_ENEMY_DEATH_B, &wanderer_base_ttl)) {
            wanderer_fx_seen = true;
            break;
        }
    }

    REQUIRE(name, wanderer_fx_seen, "wanderer crush did not spawn enemy death style B fx");
    REQUIRE(name, wanderer_base_ttl == 16, "enemy death style B ttl changed unexpectedly");
    return true;
}

static bool test_meta_shards_bank_on_new_run(void) {
    const char *name = "meta_shards_bank_on_new_run";
    GameState gs;
    game_init(&gs, 0x9abcu);
    gs.phase = GAME_PHASE_GAME_OVER;
    gs.phase_timer_ticks = 0;
    gs.run_shards = 7;
    gs.meta_shards = 12;

    {
        InputState none = {0};
        game_step(&gs, &none);
    }

    REQUIRE(name, gs.meta_shards == 19u, "meta shards did not bank run shards on reset");
    REQUIRE(name, gs.run_shards == 0, "run shards were not cleared after banking");
    REQUIRE(name, (game_consume_events(&gs) & GAME_EVENT_META_BANKED) != 0u, "meta banked event not emitted");
    return true;
}

static bool test_game_over_start_edge_restarts_once(void) {
    const char *name = "game_over_start_edge_restarts_once";
    GameState gs;
    game_init(&gs, 0x44a1u);
    const int default_lives = gs.lives;

    gs.phase = GAME_PHASE_GAME_OVER;
    gs.phase_timer_ticks = 40;
    gs.score = 12345u;
    gs.lives = 1;
    gs.round = 5;
    gs.run_shards = 6;
    gs.meta_shards = 0u;

    {
        InputState hold_start = {0};
        hold_start.start = true;
        game_step(&gs, &hold_start);
    }

    REQUIRE(
        name,
        gs.phase == GAME_PHASE_ROUND_INTRO || gs.phase == GAME_PHASE_PLAYING,
        "start press from game over should restart run once");
    REQUIRE(name, gs.score == 0u, "new run reset did not clear score");
    REQUIRE(name, gs.round == 1, "new run reset did not restart at round 1");
    REQUIRE(name, gs.lives == default_lives, "new run reset did not restore default lives");
    REQUIRE(name, gs.run_shards == 0, "new run reset did not clear run shards");
    REQUIRE(name, gs.meta_shards == 6u, "run shards were not banked into meta shards on restart");
    REQUIRE(name, gs.phase != GAME_PHASE_META_UPGRADE, "restart should not enter meta upgrade phase");

    {
        const uint32_t shards_before_hold = gs.meta_shards;
        const uint32_t progress_before_hold = game_get_meta_progress(&gs);
        const GamePhase phase_before_hold = gs.phase;
        InputState hold_start = {0};
        hold_start.start = true;
        game_step(&gs, &hold_start);
        REQUIRE(name, gs.phase == phase_before_hold, "holding start should not skip intro state");
        REQUIRE(name, gs.meta_shards == shards_before_hold, "holding start should not spend meta shards");
        REQUIRE(name, game_get_meta_progress(&gs) == progress_before_hold, "holding start should not change meta progress");
    }

    {
        InputState release = {0};
        InputState press = {0};
        game_step(&gs, &release);
        press.start = true;
        game_step(&gs, &press);
        REQUIRE(
            name,
            gs.phase == GAME_PHASE_ROUND_INTRO || gs.phase == GAME_PHASE_PLAYING,
            "second start edge should continue normal intro/playing flow");
    }

    return true;
}

static bool test_meta_upgrade_purchase_spends_and_applies(void) {
    const char *name = "meta_upgrade_purchase_spends_and_applies";
    GameState gs;
    game_init(&gs, 0x7412u);
    gs.phase = GAME_PHASE_GAME_OVER;
    gs.phase_timer_ticks = 0;
    gs.meta_shards = 20;
    gs.run_shards = 0;
    const uint32_t before_meta = gs.meta_shards;
    const uint32_t before_progress = game_get_meta_progress(&gs);
    const int before_lives = gs.lives;

    {
        InputState press = {0};
        press.start = true;
        game_step(&gs, &press);
    }

    REQUIRE(name, gs.phase != GAME_PHASE_META_UPGRADE, "run reset should bypass meta upgrade phase");
    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_INTRO || gs.phase == GAME_PHASE_PLAYING, "run reset should start round flow");
    REQUIRE(name, gs.meta_shards == before_meta, "meta shards should not be spent by restart");
    REQUIRE(name, game_get_meta_progress(&gs) == before_progress, "meta progress should not change on restart");
    REQUIRE(name, gs.lives == before_lives, "restart should not apply meta life bonuses");
    REQUIRE(name, (game_consume_events(&gs) & GAME_EVENT_META_SPENT) == 0u, "meta-spent event should not emit in skill-only mode");
    return true;
}

static bool test_meta_unlock_tiers_expand_choice_pool(void) {
    const char *name = "meta_unlock_tiers_expand_choice_pool";
    GameState gs;
    game_init(&gs, 0x5311u);
    game_set_meta_progress(&gs, 0u);
    REQUIRE(name, game_get_meta_unlock_tier(&gs) == 1, "tier mapping should start at tier 1");
    game_set_meta_progress(&gs, GAME_META_UNLOCK_TIER2_PROGRESS);
    REQUIRE(name, game_get_meta_unlock_tier(&gs) == 2, "tier mapping should enter tier 2 at threshold");
    game_set_meta_progress(&gs, GAME_META_UNLOCK_TIER3_PROGRESS);
    REQUIRE(name, game_get_meta_unlock_tier(&gs) == 3, "tier mapping should enter tier 3 at threshold");
    return true;
}

static bool test_meta_upgrade_unaffordable_starts_round_without_spend(void) {
    const char *name = "meta_upgrade_unaffordable_starts_round_without_spend";
    GameState gs;
    game_init(&gs, 0x8811u);
    gs.phase = GAME_PHASE_GAME_OVER;
    gs.phase_timer_ticks = 0;
    gs.meta_shards = 0;
    gs.run_shards = 0;

    {
        InputState none = {0};
        game_step(&gs, &none);
    }

    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_INTRO || gs.phase == GAME_PHASE_PLAYING, "zero-shard run should bypass meta phase");
    REQUIRE(name, gs.meta_shards == 0u, "meta shards changed unexpectedly");
    REQUIRE(name, (game_consume_events(&gs) & GAME_EVENT_META_SPENT) == 0u, "meta-spent event should not emit");
    return true;
}

static bool test_game_over_restarts_without_meta_menu(void) {
    const char *name = "game_over_restarts_without_meta_menu";
    GameState gs;
    game_init(&gs, 0x8b21u);

    gs.phase = GAME_PHASE_GAME_OVER;
    gs.phase_timer_ticks = 0;
    gs.score = 54321u;
    gs.round = 8;
    gs.lives = 1;
    gs.run_shards = 9;
    gs.meta_shards = 30u;

    {
        InputState press = {0};
        press.start = true;
        game_step(&gs, &press);
    }

    REQUIRE(name, gs.phase != GAME_PHASE_META_UPGRADE, "restart should not enter post-run meta menu");
    REQUIRE(name, gs.phase == GAME_PHASE_ROUND_INTRO || gs.phase == GAME_PHASE_PLAYING, "restart should return to round flow");
    REQUIRE(name, gs.round == 1, "restart should begin from round 1");
    REQUIRE(name, gs.score == 0u, "restart should clear score");
    return true;
}

int main(void) {
    int failed = 0;
    failed += test_determinism_replay() ? 0 : 1;
    failed += test_round_intro_waits_for_input() ? 0 : 1;
    failed += test_push_slide_and_crush() ? 0 : 1;
    failed += test_push_slide_crushes_interpolated_enemy_overlap() ? 0 : 1;
    failed += test_death_preserves_map_state() ? 0 : 1;
    failed += test_round_clear_advances_round() ? 0 : 1;
    failed += test_round_clear_does_not_require_points() ? 0 : 1;
    failed += test_round_clear_tracks_time_bonus_score() ? 0 : 1;
    failed += test_perk_choice_applies_selected_perk() ? 0 : 1;
    failed += test_perk_draft_anti_repeat_has_new_offer() ? 0 : 1;
    failed += test_perk_bonus_scaling_applies_caps() ? 0 : 1;
    failed += test_perk_draft_can_skip_when_all_perks_capped() ? 0 : 1;
    failed += test_perk_draft_emergency_life_overrides_cooldown() ? 0 : 1;
    failed += test_early_perk_draft_offers_mines_when_unlocked() ? 0 : 1;
    failed += test_direction_press_does_not_drop_mine_without_fire_release() ? 0 : 1;
    failed += test_fire_release_drops_mine_and_consumes_stock() ? 0 : 1;
    failed += test_fire_release_while_moving_queues_mine_drop() ? 0 : 1;
    failed += test_enemy_triggered_mine_explosion_kills_and_breaks_blocks() ? 0 : 1;
    failed += test_mine_auto_explodes_after_fuse_timeout() ? 0 : 1;
    failed += test_mine_kill_scores_less_than_slide_crush() ? 0 : 1;
    failed += test_stage_modifier_swarm_boosts_round_pressure() ? 0 : 1;
    failed += test_round_scaling_increases_pressure() ? 0 : 1;
    failed += test_stage_modifier_shattered_converts_ice() ? 0 : 1;
    failed += test_stage_modifier_black_ice_adds_blocks() ? 0 : 1;
    failed += test_stage_modifier_turbo_slide_speeds_blocks() ? 0 : 1;
    failed += test_stage_modifier_frost_siren_increases_time_pressure() ? 0 : 1;
    failed += test_stage_modifier_frost_siren_accelerates_danger_pulses() ? 0 : 1;
    failed += test_bonus_item_spawns_after_crush_threshold() ? 0 : 1;
    failed += test_bonus_item_threshold_lower_in_early_round() ? 0 : 1;
    failed += test_bonus_item_threshold_tightens_in_late_rounds() ? 0 : 1;
    failed += test_special_alignment_awards_bonus() ? 0 : 1;
    failed += test_round_enemy_mix_includes_wanderers() ? 0 : 1;
    failed += test_enemy_decision_cooldown_delays_first_step() ? 0 : 1;
    failed += test_level_rows_validation_rules() ? 0 : 1;
    failed += test_procedural_round_generation_invariants() ? 0 : 1;
    failed += test_procgen_wfc_templates_validate() ? 0 : 1;
    failed += test_procgen_template_selection_avoids_recent_repetition() ? 0 : 1;
    failed += test_procgen_template_invalid_force_falls_back_to_valid_level() ? 0 : 1;
    failed += test_timer_danger_state_pulses_near_timeout() ? 0 : 1;
    failed += test_combo_crush_spawns_score_popup() ? 0 : 1;
    failed += test_popup_spawn_offsets_from_existing_cluster() ? 0 : 1;
    failed += test_score_caps_without_wrapping() ? 0 : 1;
    failed += test_timer_danger_event_emits_and_consumes() ? 0 : 1;
    failed += test_crush_event_flag_emits() ? 0 : 1;
    failed += test_combo_event_flag_emits() ? 0 : 1;
    failed += test_block_impact_event_emits_on_slide_stop() ? 0 : 1;
    failed += test_slide_lookahead_stops_before_blocked_cell() ? 0 : 1;
    failed += test_crush_spawns_impact_fx() ? 0 : 1;
    failed += test_enemy_death_fx_styles_spawn_by_enemy_type() ? 0 : 1;
    failed += test_meta_shards_bank_on_new_run() ? 0 : 1;
    failed += test_game_over_start_edge_restarts_once() ? 0 : 1;
    failed += test_meta_upgrade_purchase_spends_and_applies() ? 0 : 1;
    failed += test_meta_unlock_tiers_expand_choice_pool() ? 0 : 1;
    failed += test_meta_upgrade_unaffordable_starts_round_without_spend() ? 0 : 1;
    failed += test_game_over_restarts_without_meta_menu() ? 0 : 1;

    if (failed != 0) {
        fprintf(stderr, "\n%d test(s) failed.\n", failed);
        return 1;
    }

    printf("All core tests passed.\n");
    return 0;
}
