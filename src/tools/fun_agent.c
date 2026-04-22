#include "game.h"

#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct BotState {
    Direction hold_dir;
    int hold_ticks;
    int think_cooldown_ticks;
    int idle_ticks;
    Direction mine_tap_dir;
    int mine_tap_stage;
    uint32_t rng_state;
} BotState;

typedef enum EvalProfileId {
    EVAL_PROFILE_ARCADE = 0,
    EVAL_PROFILE_CASUAL = 1,
    EVAL_PROFILE_SURVIVAL = 2,
    EVAL_PROFILE_AGGRESSIVE = 3
} EvalProfileId;

typedef enum OutputFormat {
    OUTPUT_FORMAT_TEXT = 0,
    OUTPUT_FORMAT_JSON = 1,
    OUTPUT_FORMAT_CSV = 2
} OutputFormat;

typedef struct EvalProfile {
    const char *name;
    int escape_trigger_dist;
    int hold_min_ticks;
    int hold_max_ticks;
    int think_cooldown_min_ticks;
    int think_cooldown_max_ticks;
    int hold_press_permille;
    int idle_chance_permille;
    int idle_min_ticks;
    int idle_max_ticks;
    double round_weight;
    double crush_weight;
    double item_weight;
    double score_weight;
    double stage_weight;
    double agency_weight;
    double activity_weight;
    double challenge_weight;
    double challenge_target_deaths;
    double activity_target_ratio;
    double agency_target_rate;
    double score_target;
    double stage_variety_target_ratio;
} EvalProfile;

typedef struct RunMetrics {
    uint64_t ticks_simulated;
    uint64_t playing_ticks;
    uint64_t active_input_ticks;
    uint64_t direction_changes;
    uint64_t rounds_cleared;
    uint64_t crush_events;
    uint64_t item_events;
    uint64_t special_events;
    uint64_t death_events;
    uint64_t stage_events;
    uint64_t meta_spent_events;
    uint64_t push_entries;
    uint64_t score;
    int max_round_reached;
    uint32_t stage_seen_mask;
    bool ended_by_game_over;
} RunMetrics;

typedef struct RoundMapSample {
    char profile[16];
    uint32_t seed;
    int run_index;
    int round_index;
    int stage_modifier;
    char stage_modifier_name[24];
    char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1];
    char rows_original[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1];
    uint64_t map_id;
    bool cleared;
    int deaths;
    int crushes;
    int items;
    int ticks_in_round;
    int time_start_ticks;
    int time_end_ticks;
    double quality_score;
} RoundMapSample;

typedef struct RoundMapSampleList {
    RoundMapSample *items;
    int count;
    int capacity;
} RoundMapSampleList;

typedef struct RoundTracker {
    bool active;
    RoundMapSample sample;
} RoundTracker;

typedef struct AggregateMetrics {
    uint64_t runs;
    uint64_t ticks_simulated;
    uint64_t playing_ticks;
    uint64_t active_input_ticks;
    uint64_t direction_changes;
    uint64_t rounds_cleared;
    uint64_t crush_events;
    uint64_t item_events;
    uint64_t special_events;
    uint64_t death_events;
    uint64_t stage_events;
    uint64_t meta_spent_events;
    uint64_t push_entries;
    uint64_t score;
    int max_round_reached;
    uint32_t stage_seen_mask;
    uint64_t stage_unique_sum;
    uint64_t game_over_runs;
} AggregateMetrics;

typedef struct EvalResult {
    double fun_score;
    double round_component;
    double crush_component;
    double item_component;
    double score_component;
    double stage_component;
    double agency_component;
    double active_component;
    double challenge_component;
    double avg_rounds;
    double avg_crushes;
    double avg_items;
    double avg_deaths;
    double avg_score;
    double direction_change_rate;
    double active_ratio;
    double stage_variety_ratio;
    double stage_unique_per_run;
    double stage_coverage_ratio;
} EvalResult;

enum {
    DEFAULT_RUNS = 24,
    DEFAULT_MAX_TICKS = 18000,
    DEFAULT_SEED = 1337u,
    EVAL_FP_ONE = 1 << 8,
    EVAL_MAX_LIVES = 7,
    EVAL_RUN_TIME_BONUS_MAX_TICKS = 35 * GAME_FIXED_TICK_HZ,
    EVAL_RUN_SCORE_MULT_MAX_PERMILLE = 1800,
    EVAL_RUN_ENEMY_SLOW_MAX_FP = (3 * EVAL_FP_ONE) / 4,
    EVAL_RUN_MINE_CAPACITY_MAX = 8,
    EVAL_MINE_PERK_STEP = 2,
    EVAL_STAGE_VARIANT_COUNT = 5,
};

static const EvalProfile kEvalProfiles[] = {
    {
        "arcade",
        2,
        5,
        10,
        0,
        1,
        900,
        15,
        1,
        2,
        25.0,
        20.0,
        10.0,
        15.0,
        10.0,
        8.0,
        5.0,
        7.0,
        2.0,
        0.55,
        0.08,
        42000.0,
        0.62,
    },
    {
        "casual",
        3,
        4,
        8,
        0,
        2,
        780,
        90,
        1,
        3,
        22.0,
        14.0,
        14.0,
        10.0,
        8.0,
        6.0,
        9.0,
        17.0,
        1.4,
        0.40,
        0.06,
        36000.0,
        0.46,
    },
    {
        "survival",
        3,
        4,
        8,
        0,
        2,
        820,
        70,
        1,
        2,
        28.0,
        12.0,
        8.0,
        8.0,
        8.0,
        6.0,
        5.0,
        25.0,
        1.0,
        0.45,
        0.06,
        34000.0,
        0.42,
    },
    {
        "aggressive",
        1,
        6,
        12,
        0,
        1,
        950,
        10,
        1,
        1,
        18.0,
        30.0,
        6.0,
        18.0,
        8.0,
        12.0,
        4.0,
        4.0,
        2.7,
        0.62,
        0.10,
        52000.0,
        0.68,
    },
};

static const uint32_t kEvalStageVariantMask =
    ((1u << STAGE_MOD_SWARM) |
     (1u << STAGE_MOD_SHATTERED_ICE) |
     (1u << STAGE_MOD_BLACK_ICE) |
     (1u << STAGE_MOD_TURBO_SLIDE) |
     (1u << STAGE_MOD_FROST_SIREN));

static const int kDirDx[5] = {0, 0, 0, -1, 1};
static const int kDirDy[5] = {0, -1, 1, 0, 0};

static void print_usage(const char *exe_name) {
    printf("Usage: %s [--runs N] [--ticks N] [--seed N] [--profile NAME] [--out-format text|json|csv] [--round-map-samples-out PATH] [--verbose]\n", exe_name);
    printf("Profiles: arcade, casual, survival, aggressive\n");
    printf("Output formats: text (default), json, csv\n");
    printf("Autoplay Icepanic and compute a gameplay-fun proxy score.\n");
}

static bool parse_int_arg(const char *arg, int *out_value) {
    char *end = NULL;
    long value = strtol(arg, &end, 10);
    if (arg == NULL || *arg == '\0' || end == NULL || *end != '\0') {
        return false;
    }
    if (value < 0 || value > 1000000000L) {
        return false;
    }
    *out_value = (int)value;
    return true;
}

static const EvalProfile *find_profile_by_name(const char *name) {
    size_t i = 0;
    for (i = 0; i < sizeof(kEvalProfiles) / sizeof(kEvalProfiles[0]); ++i) {
        if (strcmp(kEvalProfiles[i].name, name) == 0) {
            return &kEvalProfiles[i];
        }
    }
    return NULL;
}

static bool parse_output_format(const char *name, OutputFormat *out_format) {
    if (strcmp(name, "text") == 0) {
        *out_format = OUTPUT_FORMAT_TEXT;
        return true;
    }
    if (strcmp(name, "json") == 0) {
        *out_format = OUTPUT_FORMAT_JSON;
        return true;
    }
    if (strcmp(name, "csv") == 0) {
        *out_format = OUTPUT_FORMAT_CSV;
        return true;
    }
    return false;
}

static const char *stage_modifier_name(StageModifierType modifier) {
    switch (modifier) {
        case STAGE_MOD_SWARM:
            return "swarm";
        case STAGE_MOD_SHATTERED_ICE:
            return "shattered_ice";
        case STAGE_MOD_BLACK_ICE:
            return "black_ice";
        case STAGE_MOD_TURBO_SLIDE:
            return "turbo_slide";
        case STAGE_MOD_FROST_SIREN:
            return "frost_siren";
        case STAGE_MOD_NONE:
        default:
            break;
    }
    return "none";
}

static char map_char_for_state_tile(const GameState *gs, int x, int y) {
    char c = '.';
    if (gs->terrain[y][x] == TERRAIN_WALL || gs->terrain[y][x] == TERRAIN_BORDER) {
        c = '#';
    }
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
            break;
    }
    return c;
}

static void build_map_rows_from_state(
    const GameState *gs,
    char out_original[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1],
    char out_canonical[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]) {
    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            out_original[y][x] = map_char_for_state_tile(gs, x, y);
        }
        out_original[y][GAME_GRID_WIDTH] = '\0';
    }

    if (gs->player_spawn_x >= 0 &&
        gs->player_spawn_x < GAME_GRID_WIDTH &&
        gs->player_spawn_y >= 0 &&
        gs->player_spawn_y < GAME_GRID_HEIGHT) {
        out_original[gs->player_spawn_y][gs->player_spawn_x] = 'P';
    }

    for (int i = 0; i < gs->enemy_count; ++i) {
        const Enemy *enemy = &gs->enemies[i];
        if (!enemy->alive) {
            continue;
        }
        if (enemy->tile_x < 0 || enemy->tile_x >= GAME_GRID_WIDTH || enemy->tile_y < 0 || enemy->tile_y >= GAME_GRID_HEIGHT) {
            continue;
        }
        out_original[enemy->tile_y][enemy->tile_x] = 'E';
    }

    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            char c = out_original[y][x];
            if (c == 'P' || c == 'E') {
                c = '.';
            } else if (c == 'U') {
                c = '#';
            }
            out_canonical[y][x] = c;
        }
        out_canonical[y][GAME_GRID_WIDTH] = '\0';
    }
}

static uint64_t hash_map_rows(const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]) {
    uint64_t h = 1469598103934665603ULL; /* FNV-1a 64-bit. */
    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            h ^= (uint8_t)rows[y][x];
            h *= 1099511628211ULL;
        }
        h ^= (uint8_t)'\n';
        h *= 1099511628211ULL;
    }
    return h;
}

static double clamp_double(double v, double lo, double hi) {
    if (v < lo) {
        return lo;
    }
    if (v > hi) {
        return hi;
    }
    return v;
}

static double compute_round_quality_score(const RoundMapSample *sample) {
    double score = 0.0;
    const double crush_norm = clamp_double((double)sample->crushes / 6.0, 0.0, 1.0);
    const double item_norm = clamp_double((double)sample->items / 2.0, 0.0, 1.0);
    const int start_ticks = (sample->time_start_ticks > 0) ? sample->time_start_ticks : 1;
    const double time_ratio = clamp_double((double)sample->time_end_ticks / (double)start_ticks, 0.0, 1.0);

    if (sample->cleared) {
        score += 45.0;
    }
    score += 20.0 * crush_norm;
    score += 10.0 * item_norm;
    score += 20.0 * time_ratio;
    if (sample->deaths == 0) {
        score += 5.0;
    }
    return clamp_double(score, 0.0, 100.0);
}

static void round_samples_list_init(RoundMapSampleList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

static void round_samples_list_free(RoundMapSampleList *list) {
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

static bool round_samples_list_append(RoundMapSampleList *list, const RoundMapSample *sample) {
    if (list->count >= list->capacity) {
        const int new_cap = (list->capacity <= 0) ? 64 : (list->capacity * 2);
        RoundMapSample *new_items = (RoundMapSample *)realloc(list->items, (size_t)new_cap * sizeof(*new_items));
        if (new_items == NULL) {
            return false;
        }
        list->items = new_items;
        list->capacity = new_cap;
    }
    list->items[list->count++] = *sample;
    return true;
}

static void round_tracker_reset(RoundTracker *tracker) {
    memset(tracker, 0, sizeof(*tracker));
    tracker->active = false;
}

static void round_tracker_begin(
    RoundTracker *tracker,
    const GameState *gs,
    const char *profile_name,
    uint32_t run_seed,
    int run_index) {
    RoundMapSample sample;
    memset(&sample, 0, sizeof(sample));
    strncpy(sample.profile, profile_name, sizeof(sample.profile) - 1);
    sample.seed = run_seed;
    sample.run_index = run_index;
    sample.round_index = gs->round;
    sample.stage_modifier = (int)gs->stage_modifier;
    strncpy(sample.stage_modifier_name, stage_modifier_name(gs->stage_modifier), sizeof(sample.stage_modifier_name) - 1);
    sample.time_start_ticks = gs->round_time_ticks;
    sample.time_end_ticks = gs->round_time_ticks;
    build_map_rows_from_state(gs, sample.rows_original, sample.rows);
    sample.map_id = hash_map_rows(sample.rows);
    sample.quality_score = 0.0;

    tracker->sample = sample;
    tracker->active = true;
}

static bool round_tracker_finalize(RoundTracker *tracker, RoundMapSampleList *out_samples) {
    if (!tracker->active) {
        return true;
    }
    tracker->sample.quality_score = compute_round_quality_score(&tracker->sample);
    if (!round_samples_list_append(out_samples, &tracker->sample)) {
        return false;
    }
    round_tracker_reset(tracker);
    return true;
}

static bool round_tracker_on_tick(
    RoundTracker *tracker,
    const GameState *prev,
    const GameState *next,
    uint32_t events,
    const char *profile_name,
    uint32_t run_seed,
    int run_index,
    RoundMapSampleList *out_samples) {
    if (!tracker->active) {
        round_tracker_begin(tracker, next, profile_name, run_seed, run_index);
    }

    if (tracker->active) {
        RoundMapSample *sample = &tracker->sample;
        if (sample->round_index == prev->round || sample->round_index == next->round) {
            ++sample->ticks_in_round;
        }
        if ((events & GAME_EVENT_CRUSH) != 0u) {
            ++sample->crushes;
        }
        if ((events & GAME_EVENT_ITEM_COLLECT) != 0u) {
            ++sample->items;
        }
        if ((events & GAME_EVENT_PLAYER_DIED) != 0u) {
            ++sample->deaths;
        }
        if ((events & GAME_EVENT_ROUND_CLEAR) != 0u) {
            sample->cleared = true;
        }
        if (sample->round_index == prev->round) {
            sample->time_end_ticks = prev->round_time_ticks;
        }
        if (sample->round_index == next->round) {
            sample->time_end_ticks = next->round_time_ticks;
        }
    }

    if ((events & GAME_EVENT_GAME_OVER) != 0u) {
        return round_tracker_finalize(tracker, out_samples);
    }

    if ((events & GAME_EVENT_ROUND_START) != 0u) {
        if (tracker->active && tracker->sample.round_index != next->round) {
            if (!round_tracker_finalize(tracker, out_samples)) {
                return false;
            }
        }
        if (!tracker->active || tracker->sample.round_index != next->round) {
            round_tracker_begin(tracker, next, profile_name, run_seed, run_index);
        }
    }

    return true;
}

static bool write_round_map_samples_jsonl(const char *path, const RoundMapSampleList *samples) {
    FILE *f = NULL;
    f = fopen(path, "wb");
    if (f == NULL) {
        return false;
    }

    for (int i = 0; i < samples->count; ++i) {
        char map_id_hex[24];
        const RoundMapSample *sample = &samples->items[i];
        snprintf(map_id_hex, sizeof(map_id_hex), "%016llx", (unsigned long long)sample->map_id);
        fprintf(f, "{");
        fprintf(f, "\"map_id\":\"%s\",", map_id_hex);
        fprintf(f, "\"rows\":[");
        for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
            fprintf(f, "%s\"%s\"", (y == 0) ? "" : ",", sample->rows[y]);
        }
        fprintf(f, "],");
        fprintf(f, "\"rows_original\":[");
        for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
            fprintf(f, "%s\"%s\"", (y == 0) ? "" : ",", sample->rows_original[y]);
        }
        fprintf(f, "],");
        fprintf(f, "\"profile\":\"%s\",", sample->profile);
        fprintf(f, "\"seed\":%u,", sample->seed);
        fprintf(f, "\"run_index\":%d,", sample->run_index);
        fprintf(f, "\"round_index\":%d,", sample->round_index);
        fprintf(f, "\"stage_modifier\":%d,", sample->stage_modifier);
        fprintf(f, "\"stage_modifier_name\":\"%s\",", sample->stage_modifier_name);
        fprintf(f, "\"cleared\":%s,", sample->cleared ? "true" : "false");
        fprintf(f, "\"deaths\":%d,", sample->deaths);
        fprintf(f, "\"crushes\":%d,", sample->crushes);
        fprintf(f, "\"items\":%d,", sample->items);
        fprintf(f, "\"ticks_in_round\":%d,", sample->ticks_in_round);
        fprintf(f, "\"time_start_ticks\":%d,", sample->time_start_ticks);
        fprintf(f, "\"time_end_ticks\":%d,", sample->time_end_ticks);
        fprintf(f, "\"quality_score\":%.3f", sample->quality_score);
        fprintf(f, "}\n");
    }

    fclose(f);
    return true;
}

static uint32_t rng_next(BotState *bot) {
    uint32_t x = bot->rng_state;
    if (x == 0u) {
        x = 0x6a09e667u;
    }
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    bot->rng_state = x;
    return x;
}

static int clamp_int(int value, int lo, int hi) {
    if (value < lo) {
        return lo;
    }
    if (value > hi) {
        return hi;
    }
    return value;
}

static int perk_time_step_ticks_for_level(int level) {
    if (level <= 0) {
        return 5 * GAME_FIXED_TICK_HZ;
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
        return 100;
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
        return EVAL_FP_ONE / 4;
    }
    if (level <= 2) {
        return EVAL_FP_ONE / 8;
    }
    return EVAL_FP_ONE / 16;
}

static int remaining_time_bonus_ticks(const GameState *gs) {
    if (gs->run_round_time_bonus_ticks >= EVAL_RUN_TIME_BONUS_MAX_TICKS) {
        return 0;
    }
    return EVAL_RUN_TIME_BONUS_MAX_TICKS - gs->run_round_time_bonus_ticks;
}

static int remaining_score_bonus_permille(const GameState *gs) {
    if (gs->run_score_mult_permille >= EVAL_RUN_SCORE_MULT_MAX_PERMILLE) {
        return 0;
    }
    return EVAL_RUN_SCORE_MULT_MAX_PERMILLE - gs->run_score_mult_permille;
}

static int remaining_enemy_slow_fp(const GameState *gs) {
    if (gs->run_enemy_speed_penalty_fp >= EVAL_RUN_ENEMY_SLOW_MAX_FP) {
        return 0;
    }
    return EVAL_RUN_ENEMY_SLOW_MAX_FP - gs->run_enemy_speed_penalty_fp;
}

static int mine_capacity_step_for_level(int level) {
    if (level <= 1) {
        return EVAL_MINE_PERK_STEP;
    }
    return 1;
}

static int remaining_mine_capacity(const GameState *gs) {
    if (gs->run_mine_capacity >= EVAL_RUN_MINE_CAPACITY_MAX) {
        return 0;
    }
    return EVAL_RUN_MINE_CAPACITY_MAX - gs->run_mine_capacity;
}

static int rng_range(BotState *bot, int lo, int hi_inclusive) {
    uint32_t span = 0u;
    if (hi_inclusive <= lo) {
        return lo;
    }
    span = (uint32_t)(hi_inclusive - lo + 1);
    return lo + (int)(rng_next(bot) % span);
}

static bool in_bounds(int x, int y) {
    return x >= 0 && y >= 0 && x < GAME_GRID_WIDTH && y < GAME_GRID_HEIGHT;
}

static bool terrain_blocks(Terrain terrain) {
    return terrain == TERRAIN_WALL || terrain == TERRAIN_BORDER;
}

static bool is_pushable_block(BlockType block) {
    return block == BLOCK_ICE || block == BLOCK_SPECIAL || block == BLOCK_CRACKED;
}

static bool tile_has_enemy(const GameState *gs, int x, int y) {
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

static int alive_enemy_count(const GameState *gs) {
    int count = 0;
    for (int i = 0; i < gs->enemy_count; ++i) {
        if (gs->enemies[i].alive) {
            ++count;
        }
    }
    return count;
}

static bool tile_has_moving_block(const GameState *gs, int x, int y) {
    for (int i = 0; i < GAME_MAX_MOVING_BLOCKS; ++i) {
        if (!gs->moving_blocks[i].active) {
            continue;
        }
        if (gs->moving_blocks[i].tile_x == x && gs->moving_blocks[i].tile_y == y) {
            return true;
        }
    }
    return false;
}

static bool walkable_for_player(const GameState *gs, int x, int y, bool avoid_enemies) {
    if (!in_bounds(x, y)) {
        return false;
    }
    if (terrain_blocks(gs->terrain[y][x])) {
        return false;
    }
    if (gs->blocks[y][x] != BLOCK_NONE) {
        return false;
    }
    if (tile_has_moving_block(gs, x, y)) {
        return false;
    }
    if (avoid_enemies && tile_has_enemy(gs, x, y)) {
        return false;
    }
    return true;
}

static bool blocked_for_slide(const GameState *gs, int x, int y) {
    if (!in_bounds(x, y)) {
        return true;
    }
    if (terrain_blocks(gs->terrain[y][x])) {
        return true;
    }
    if (gs->blocks[y][x] != BLOCK_NONE) {
        return true;
    }
    if (tile_has_moving_block(gs, x, y)) {
        return true;
    }
    return false;
}

static bool can_push_block_dir(const GameState *gs, int block_x, int block_y, Direction dir) {
    if (dir == DIR_NONE) {
        return false;
    }
    if (!in_bounds(block_x, block_y)) {
        return false;
    }
    if (!is_pushable_block(gs->blocks[block_y][block_x])) {
        return false;
    }
    {
        const int nx = block_x + kDirDx[dir];
        const int ny = block_y + kDirDy[dir];
        if (blocked_for_slide(gs, nx, ny)) {
            return false;
        }
    }
    return true;
}

static int estimate_slide_kills(const GameState *gs, int block_x, int block_y, Direction dir) {
    int kills = 0;
    int cx = block_x;
    int cy = block_y;
    while (true) {
        const int nx = cx + kDirDx[dir];
        const int ny = cy + kDirDy[dir];
        if (blocked_for_slide(gs, nx, ny)) {
            break;
        }
        cx = nx;
        cy = ny;
        if (tile_has_enemy(gs, cx, cy)) {
            ++kills;
        }
    }
    return kills;
}

static int nearest_enemy_distance(const GameState *gs, int x, int y) {
    int best = 9999;
    for (int i = 0; i < gs->enemy_count; ++i) {
        if (!gs->enemies[i].alive) {
            continue;
        }
        {
            const int dist = abs(gs->enemies[i].tile_x - x) + abs(gs->enemies[i].tile_y - y);
            if (dist < best) {
                best = dist;
            }
        }
    }
    return best;
}

static Direction bfs_next_direction_to(
    const GameState *gs,
    int start_x,
    int start_y,
    int target_x,
    int target_y,
    bool avoid_enemies) {
    bool visited[GAME_GRID_HEIGHT][GAME_GRID_WIDTH];
    Direction first_step[GAME_GRID_HEIGHT][GAME_GRID_WIDTH];
    int qx[GAME_GRID_HEIGHT * GAME_GRID_WIDTH];
    int qy[GAME_GRID_HEIGHT * GAME_GRID_WIDTH];
    int head = 0;
    int tail = 0;

    if (!in_bounds(start_x, start_y) || !in_bounds(target_x, target_y)) {
        return DIR_NONE;
    }
    if (start_x == target_x && start_y == target_y) {
        return DIR_NONE;
    }
    if (!walkable_for_player(gs, target_x, target_y, avoid_enemies)) {
        return DIR_NONE;
    }

    memset(visited, 0, sizeof(visited));
    memset(first_step, 0, sizeof(first_step));

    visited[start_y][start_x] = true;
    qx[tail] = start_x;
    qy[tail] = start_y;
    ++tail;

    while (head < tail) {
        const int cx = qx[head];
        const int cy = qy[head];
        ++head;

        for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
            const int nx = cx + kDirDx[d];
            const int ny = cy + kDirDy[d];
            Direction step_dir = d;
            if (!in_bounds(nx, ny) || visited[ny][nx]) {
                continue;
            }
            if (!walkable_for_player(gs, nx, ny, avoid_enemies)) {
                continue;
            }
            if (cx != start_x || cy != start_y) {
                step_dir = first_step[cy][cx];
            }

            visited[ny][nx] = true;
            first_step[ny][nx] = step_dir;
            if (nx == target_x && ny == target_y) {
                return step_dir;
            }

            qx[tail] = nx;
            qy[tail] = ny;
            ++tail;
        }
    }

    return DIR_NONE;
}

static void set_input_direction(InputState *in, Direction dir, bool new_press) {
    if (dir == DIR_NONE) {
        return;
    }
    if (new_press) {
        in->newest_press = dir;
    }
    if (dir == DIR_UP) {
        in->up = true;
    } else if (dir == DIR_DOWN) {
        in->down = true;
    } else if (dir == DIR_LEFT) {
        in->left = true;
    } else if (dir == DIR_RIGHT) {
        in->right = true;
    }
}

static Direction choose_escape_direction(const GameState *gs, int escape_trigger_dist) {
    const int px = gs->player.tile_x;
    const int py = gs->player.tile_y;
    int threat_dist = nearest_enemy_distance(gs, px, py);
    Direction best_dir = DIR_NONE;
    int best_safety = threat_dist;

    if (threat_dist > escape_trigger_dist) {
        return DIR_NONE;
    }

    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = px + kDirDx[d];
        const int ny = py + kDirDy[d];
        int safety = 0;
        if (!walkable_for_player(gs, nx, ny, true)) {
            continue;
        }
        safety = nearest_enemy_distance(gs, nx, ny);
        if (safety > best_safety) {
            best_safety = safety;
            best_dir = d;
        }
    }
    return best_dir;
}

static Direction choose_push_or_path_direction(const GameState *gs) {
    const int px = gs->player.tile_x;
    const int py = gs->player.tile_y;
    Direction best_move = DIR_NONE;
    int best_score = -1000000;

    for (int by = 1; by < GAME_GRID_HEIGHT - 1; ++by) {
        for (int bx = 1; bx < GAME_GRID_WIDTH - 1; ++bx) {
            if (!is_pushable_block(gs->blocks[by][bx])) {
                continue;
            }

            for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
                const int stand_x = bx - kDirDx[d];
                const int stand_y = by - kDirDy[d];
                Direction next_move = DIR_NONE;
                int score = 0;
                int kills = 0;
                int dist = 0;

                if (!can_push_block_dir(gs, bx, by, d)) {
                    continue;
                }
                if (!in_bounds(stand_x, stand_y)) {
                    continue;
                }
                if (!walkable_for_player(gs, stand_x, stand_y, true)) {
                    continue;
                }

                kills = estimate_slide_kills(gs, bx, by, d);
                dist = abs(px - stand_x) + abs(py - stand_y);

                if (px == stand_x && py == stand_y) {
                    next_move = d;
                } else {
                    next_move = bfs_next_direction_to(gs, px, py, stand_x, stand_y, true);
                }
                if (next_move == DIR_NONE) {
                    continue;
                }

                score = 0;
                score += kills * 120;
                score -= dist * 3;
                if (gs->blocks[by][bx] == BLOCK_SPECIAL) {
                    score += 20;
                } else if (gs->blocks[by][bx] == BLOCK_CRACKED) {
                    score += 8;
                }

                if (score > best_score) {
                    best_score = score;
                    best_move = next_move;
                }
            }
        }
    }

    return best_move;
}

static Direction choose_item_direction(const GameState *gs) {
    const int px = gs->player.tile_x;
    const int py = gs->player.tile_y;
    Direction best_move = DIR_NONE;
    int best_dist = 9999;

    for (int y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        for (int x = 1; x < GAME_GRID_WIDTH - 1; ++x) {
            Direction move = DIR_NONE;
            int dist = 0;

            if (gs->items[y][x] == ITEM_NONE) {
                continue;
            }

            dist = abs(px - x) + abs(py - y);
            move = bfs_next_direction_to(gs, px, py, x, y, true);
            if (move == DIR_NONE) {
                continue;
            }

            if (dist < best_dist) {
                best_dist = dist;
                best_move = move;
            }
        }
    }
    return best_move;
}

static Direction choose_hunt_direction(const GameState *gs) {
    const int px = gs->player.tile_x;
    const int py = gs->player.tile_y;
    Direction best_move = DIR_NONE;
    int best_dist = 9999;

    for (int i = 0; i < gs->enemy_count; ++i) {
        const Enemy *enemy = &gs->enemies[i];
        if (!enemy->alive) {
            continue;
        }
        for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
            Direction move = DIR_NONE;
            const int tx = enemy->tile_x + kDirDx[d];
            const int ty = enemy->tile_y + kDirDy[d];
            int dist = 0;

            if (!walkable_for_player(gs, tx, ty, true)) {
                continue;
            }
            dist = abs(px - tx) + abs(py - ty);
            move = bfs_next_direction_to(gs, px, py, tx, ty, true);
            if (move == DIR_NONE) {
                continue;
            }
            if (dist < best_dist) {
                best_dist = dist;
                best_move = move;
            }
        }
    }
    return best_move;
}

static Direction choose_wander_direction(const GameState *gs, BotState *bot) {
    Direction candidates[4];
    int candidate_count = 0;

    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = gs->player.tile_x + kDirDx[d];
        const int ny = gs->player.tile_y + kDirDy[d];
        if (!walkable_for_player(gs, nx, ny, true)) {
            continue;
        }
        candidates[candidate_count++] = d;
    }

    if (candidate_count == 0) {
        return DIR_NONE;
    }

    if (bot->hold_dir != DIR_NONE && bot->hold_ticks > 0) {
        for (int i = 0; i < candidate_count; ++i) {
            if (candidates[i] == bot->hold_dir) {
                return bot->hold_dir;
            }
        }
    }

    return candidates[rng_next(bot) % (uint32_t)candidate_count];
}

static Direction first_walkable_direction(const GameState *gs) {
    const int px = gs->player.tile_x;
    const int py = gs->player.tile_y;
    for (Direction d = DIR_UP; d <= DIR_RIGHT; d = (Direction)(d + 1)) {
        const int nx = px + kDirDx[d];
        const int ny = py + kDirDy[d];
        if (walkable_for_player(gs, nx, ny, true)) {
            return d;
        }
    }
    return DIR_NONE;
}

static bool can_drop_mine_here(const GameState *gs) {
    const int x = gs->player.tile_x;
    const int y = gs->player.tile_y;
    if (gs->run_mine_stock <= 0) {
        return false;
    }
    if (!in_bounds(x, y)) {
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
    if (tile_has_moving_block(gs, x, y)) {
        return false;
    }
    return true;
}

static Direction choose_mine_tap_direction(const GameState *gs, BotState *bot, const EvalProfile *profile) {
    Direction dir = choose_escape_direction(gs, profile->escape_trigger_dist + 1);
    if (dir != DIR_NONE) {
        return dir;
    }
    if (bot->hold_dir != DIR_NONE) {
        dir = bot->hold_dir;
        if (walkable_for_player(gs, gs->player.tile_x + kDirDx[dir], gs->player.tile_y + kDirDy[dir], true)) {
            return dir;
        }
    }
    dir = choose_wander_direction(gs, bot);
    if (dir != DIR_NONE) {
        return dir;
    }
    return first_walkable_direction(gs);
}

static int pick_perk_choice_index(const GameState *gs, EvalProfileId profile_id) {
    int best_idx = 0;
    int best_score = -100000;

    for (int i = 0; i < gs->perk_choice_count; ++i) {
        const PerkType perk = gs->perk_choices[i];
        int score = 0;
        if (perk == PERK_LIFE_BOOST) {
            if (gs->lives >= EVAL_MAX_LIVES) {
                score = -2000;
            } else if (gs->lives <= 1) {
                score = 220;
            } else if (gs->lives <= 3) {
                score = 150;
            } else if (gs->lives <= 5) {
                score = 105;
            } else {
                score = 70;
            }
            score -= gs->perk_levels[perk] * 6;
        } else if (perk == PERK_TIME_BOOST) {
            int gain = perk_time_step_ticks_for_level(gs->perk_levels[perk]);
            const int room = remaining_time_bonus_ticks(gs);
            if (room <= 0) {
                score = -2000;
            } else {
                if (gain > room) {
                    gain = room;
                }
                score = 70 + (gain * 90) / (5 * GAME_FIXED_TICK_HZ);
            }
        } else if (perk == PERK_ENEMY_SLOW) {
            int gain = perk_enemy_slow_step_fp_for_level(gs->perk_levels[perk]);
            const int room = remaining_enemy_slow_fp(gs);
            if (room <= 0) {
                score = -2000;
            } else {
                if (gain > room) {
                    gain = room;
                }
                score = 80 + (gain * 90) / (EVAL_FP_ONE / 4);
            }
        } else if (perk == PERK_SCORE_BOOST) {
            int gain = perk_score_step_permille_for_level(gs->perk_levels[perk]);
            const int room = remaining_score_bonus_permille(gs);
            if (room <= 0) {
                score = -2000;
            } else {
                if (gain > room) {
                    gain = room;
                }
                score = 68 + (gain * 82) / 100;
            }
        } else if (perk == PERK_MINES) {
            const int capacity_room = remaining_mine_capacity(gs);
            const int stock_room = gs->run_mine_capacity - gs->run_mine_stock;
            int capacity_gain = mine_capacity_step_for_level(gs->perk_levels[perk]);
            if (capacity_room <= 0 && stock_room <= 0) {
                score = -2000;
            } else {
                if (capacity_gain > capacity_room) {
                    capacity_gain = capacity_room;
                }
                score = 70;
                if (gs->run_mine_capacity == 0) {
                    score += 70;
                }
                score += (stock_room * 75) / EVAL_RUN_MINE_CAPACITY_MAX;
                score += (capacity_gain * 60) / EVAL_MINE_PERK_STEP;
            }
        }
        if (profile_id == EVAL_PROFILE_AGGRESSIVE && perk == PERK_SCORE_BOOST) {
            score += 28;
        } else if (profile_id == EVAL_PROFILE_SURVIVAL && perk == PERK_LIFE_BOOST) {
            score += 34;
        } else if (profile_id == EVAL_PROFILE_SURVIVAL && perk == PERK_ENEMY_SLOW) {
            score += 16;
        } else if (profile_id == EVAL_PROFILE_CASUAL && perk == PERK_TIME_BOOST) {
            score += 20;
        } else if (profile_id == EVAL_PROFILE_AGGRESSIVE && perk == PERK_MINES) {
            score += 16;
        } else if (profile_id == EVAL_PROFILE_CASUAL && perk == PERK_MINES) {
            score += 8;
        }
        if (score > best_score) {
            best_score = score;
            best_idx = i;
        }
    }
    return clamp_int(best_idx, 0, gs->perk_choice_count - 1);
}

static int pick_meta_choice_index(const GameState *gs, EvalProfileId profile_id) {
    int best_idx = 0;
    int best_score = -100000;
    bool has_affordable = false;

    for (int i = 0; i < gs->meta_choice_count; ++i) {
        const MetaUpgradeType meta = gs->meta_choices[i];
        const int cost = gs->meta_choice_costs[i];
        int score = -5000;
        bool affordable = false;

        if (cost <= 0) {
            continue;
        }
        affordable = gs->meta_shards >= (uint32_t)cost;
        if (!affordable) {
            continue;
        }

        has_affordable = true;
        if (meta == META_UPGRADE_LIFE_PACK) {
            score = (gs->lives <= 1) ? 220 : 130;
            if (gs->lives >= EVAL_MAX_LIVES) {
                score = -2000;
            }
        } else if (meta == META_UPGRADE_TIME_PACK) {
            const int room = remaining_time_bonus_ticks(gs);
            score = (room <= 0) ? -2000 : 118 + (room * 22) / EVAL_RUN_TIME_BONUS_MAX_TICKS;
        } else if (meta == META_UPGRADE_SLOW_PACK) {
            const int room = remaining_enemy_slow_fp(gs);
            score = (room <= 0) ? -2000 : 124 + (room * 24) / EVAL_RUN_ENEMY_SLOW_MAX_FP;
        } else if (meta == META_UPGRADE_SCORE_PACK) {
            const int room = remaining_score_bonus_permille(gs);
            score = (room <= 0) ? -2000 : 112 + (room * 22) / EVAL_RUN_SCORE_MULT_MAX_PERMILLE;
        }
        if (profile_id == EVAL_PROFILE_AGGRESSIVE && meta == META_UPGRADE_SCORE_PACK) {
            score += 30;
        } else if (profile_id == EVAL_PROFILE_CASUAL && meta == META_UPGRADE_TIME_PACK) {
            score += 20;
        } else if (profile_id == EVAL_PROFILE_SURVIVAL && meta == META_UPGRADE_LIFE_PACK) {
            score += 36;
        } else if (profile_id == EVAL_PROFILE_SURVIVAL && meta == META_UPGRADE_SLOW_PACK) {
            score += 18;
        }
        score -= cost * 2;

        if (score > best_score) {
            best_score = score;
            best_idx = i;
        }
    }

    if (!has_affordable) {
        return 0;
    }
    return clamp_int(best_idx, 0, gs->meta_choice_count - 1);
}

static void bot_build_input(
    const GameState *gs,
    BotState *bot,
    const EvalProfile *profile,
    EvalProfileId profile_id,
    InputState *out_input) {
    memset(out_input, 0, sizeof(*out_input));

    if (gs->phase == GAME_PHASE_ROUND_INTRO) {
        bot->mine_tap_dir = DIR_NONE;
        bot->mine_tap_stage = 0;
        /* Round intro now has two confirms (title then round card). Fire advances both without start-edge stalls. */
        out_input->fire_pressed = true;
        return;
    }

    if (gs->phase == GAME_PHASE_ROUND_CLEAR ||
        gs->phase == GAME_PHASE_GAME_OVER) {
        bot->mine_tap_dir = DIR_NONE;
        bot->mine_tap_stage = 0;
        out_input->start = true;
        return;
    }

    if (gs->phase == GAME_PHASE_PERK_CHOICE) {
        bot->mine_tap_dir = DIR_NONE;
        bot->mine_tap_stage = 0;
        const int target_idx = pick_perk_choice_index(gs, profile_id);
        if (gs->perk_choice_count <= 0) {
            out_input->start = true;
            return;
        }
        if (gs->perk_choice_selected == target_idx) {
            out_input->start = true;
        } else {
            const int diff = target_idx - gs->perk_choice_selected;
            if (diff > 0) {
                set_input_direction(out_input, DIR_RIGHT, true);
            } else {
                set_input_direction(out_input, DIR_LEFT, true);
            }
        }
        return;
    }

    if (gs->phase == GAME_PHASE_META_UPGRADE) {
        bot->mine_tap_dir = DIR_NONE;
        bot->mine_tap_stage = 0;
        const int target_idx = pick_meta_choice_index(gs, profile_id);
        if (gs->meta_choice_count <= 0) {
            out_input->start = true;
            return;
        }
        if (gs->meta_choice_selected == target_idx) {
            out_input->start = true;
        } else {
            const int diff = target_idx - gs->meta_choice_selected;
            if (diff > 0) {
                set_input_direction(out_input, DIR_RIGHT, true);
            } else {
                set_input_direction(out_input, DIR_LEFT, true);
            }
        }
        return;
    }

    if (gs->phase != GAME_PHASE_PLAYING || !gs->player.alive) {
        bot->mine_tap_dir = DIR_NONE;
        bot->mine_tap_stage = 0;
        return;
    }

    if (bot->mine_tap_stage > 0 && bot->mine_tap_dir != DIR_NONE) {
        const Direction tap_dir = bot->mine_tap_dir;
        set_input_direction(out_input, tap_dir, true);
        --bot->mine_tap_stage;
        if (bot->mine_tap_stage <= 0) {
            bot->mine_tap_stage = 0;
            bot->mine_tap_dir = DIR_NONE;
            bot->hold_dir = tap_dir;
            if (bot->hold_ticks < 2) {
                bot->hold_ticks = 2;
            }
        }
        return;
    }

    if (can_drop_mine_here(gs)) {
        const int threat_dist = nearest_enemy_distance(gs, gs->player.tile_x, gs->player.tile_y);
        const int mine_trigger_dist = profile->escape_trigger_dist + 1;
        int drop_bias_permille = 520;
        if (profile_id == EVAL_PROFILE_AGGRESSIVE) {
            drop_bias_permille = 650;
        } else if (profile_id == EVAL_PROFILE_SURVIVAL) {
            drop_bias_permille = 560;
        } else if (profile_id == EVAL_PROFILE_CASUAL) {
            drop_bias_permille = 430;
        }
        if (threat_dist <= mine_trigger_dist && (int)(rng_next(bot) % 1000u) < drop_bias_permille) {
            Direction tap_dir = choose_mine_tap_direction(gs, bot, profile);
            if (tap_dir != DIR_NONE) {
                bot->mine_tap_dir = tap_dir;
                bot->mine_tap_stage = 1;
                set_input_direction(out_input, tap_dir, true);
                return;
            }
        }
    }

    if (bot->idle_ticks > 0) {
        --bot->idle_ticks;
        return;
    }

    if (gs->player.state == PLAYER_WALKING || gs->player.state == PLAYER_PUSHING) {
        if (bot->hold_dir != DIR_NONE) {
            if ((int)(rng_next(bot) % 1000u) < profile->hold_press_permille) {
                set_input_direction(out_input, bot->hold_dir, false);
            }
            if (bot->hold_ticks > 0) {
                --bot->hold_ticks;
            }
            if (bot->hold_ticks <= 0) {
                bot->hold_dir = DIR_NONE;
                bot->hold_ticks = 0;
            }
        }
        return;
    }

    if (bot->think_cooldown_ticks > 0) {
        --bot->think_cooldown_ticks;
        if (bot->hold_dir != DIR_NONE && bot->hold_ticks > 0) {
            if ((int)(rng_next(bot) % 1000u) < profile->hold_press_permille) {
                set_input_direction(out_input, bot->hold_dir, false);
            }
            --bot->hold_ticks;
            if (bot->hold_ticks <= 0) {
                bot->hold_dir = DIR_NONE;
                bot->hold_ticks = 0;
            }
        }
        return;
    }

    {
        Direction chosen = DIR_NONE;
        Direction escape = choose_escape_direction(gs, profile->escape_trigger_dist);
        int threat_dist = nearest_enemy_distance(gs, gs->player.tile_x, gs->player.tile_y);
        if (escape != DIR_NONE) {
            chosen = escape;
        }
        if (chosen == DIR_NONE) {
            chosen = choose_push_or_path_direction(gs);
        }
        if (chosen == DIR_NONE && alive_enemy_count(gs) > 0) {
            if (profile_id == EVAL_PROFILE_AGGRESSIVE) {
                chosen = choose_hunt_direction(gs);
                if (chosen == DIR_NONE) {
                    chosen = choose_item_direction(gs);
                }
            } else {
                chosen = choose_item_direction(gs);
                if (chosen == DIR_NONE) {
                    chosen = choose_hunt_direction(gs);
                }
            }
        }
        if (chosen == DIR_NONE) {
            chosen = choose_wander_direction(gs, bot);
        }

        if (escape == DIR_NONE &&
            chosen != DIR_NONE &&
            threat_dist > profile->escape_trigger_dist + 1 &&
            (int)(rng_next(bot) % 1000u) < profile->idle_chance_permille) {
            bot->idle_ticks = rng_range(bot, profile->idle_min_ticks, profile->idle_max_ticks);
            return;
        }

        if (chosen != DIR_NONE) {
            bot->hold_dir = chosen;
            bot->hold_ticks = rng_range(bot, profile->hold_min_ticks, profile->hold_max_ticks);
            bot->think_cooldown_ticks = rng_range(
                bot,
                profile->think_cooldown_min_ticks,
                profile->think_cooldown_max_ticks);
            set_input_direction(out_input, chosen, true);
        } else {
            bot->hold_dir = DIR_NONE;
            bot->hold_ticks = 0;
            bot->think_cooldown_ticks = 0;
        }
    }
}

static void metrics_init(RunMetrics *m) {
    memset(m, 0, sizeof(*m));
}

static void metrics_on_tick(
    RunMetrics *m,
    const GameState *prev,
    const GameState *next,
    const InputState *input,
    uint32_t events) {
    m->ticks_simulated += 1;
    if (next->phase == GAME_PHASE_PLAYING) {
        m->playing_ticks += 1;
        if (input->up || input->down || input->left || input->right) {
            m->active_input_ticks += 1;
        }
    }
    if (input->newest_press != DIR_NONE && input->newest_press != prev->player.buffered_dir) {
        m->direction_changes += 1;
    }
    if (prev->player.state != PLAYER_PUSHING && next->player.state == PLAYER_PUSHING) {
        m->push_entries += 1;
    }
    if ((events & GAME_EVENT_ROUND_CLEAR) != 0u) {
        m->rounds_cleared += 1;
    }
    if ((events & GAME_EVENT_CRUSH) != 0u) {
        m->crush_events += 1;
    }
    if ((events & GAME_EVENT_ITEM_COLLECT) != 0u) {
        m->item_events += 1;
    }
    if ((events & GAME_EVENT_SPECIAL_ALIGNMENT) != 0u) {
        m->special_events += 1;
    }
    if ((events & GAME_EVENT_PLAYER_DIED) != 0u) {
        m->death_events += 1;
    }
    if ((events & GAME_EVENT_STAGE_MODIFIER) != 0u) {
        m->stage_events += 1;
    }
    if ((events & GAME_EVENT_META_SPENT) != 0u) {
        m->meta_spent_events += 1;
    }
    if ((events & GAME_EVENT_GAME_OVER) != 0u) {
        m->ended_by_game_over = true;
    }
    if (next->round > m->max_round_reached) {
        m->max_round_reached = next->round;
    }
    if (next->stage_modifier >= STAGE_MOD_SWARM && next->stage_modifier <= STAGE_MOD_FROST_SIREN) {
        m->stage_seen_mask |= (1u << (unsigned)next->stage_modifier);
    }
    m->score = next->score;
}

static void aggregate_add(AggregateMetrics *agg, const RunMetrics *run) {
    uint32_t run_stage_mask = 0u;
    int run_unique_count = 0;
    agg->runs += 1;
    agg->ticks_simulated += run->ticks_simulated;
    agg->playing_ticks += run->playing_ticks;
    agg->active_input_ticks += run->active_input_ticks;
    agg->direction_changes += run->direction_changes;
    agg->rounds_cleared += run->rounds_cleared;
    agg->crush_events += run->crush_events;
    agg->item_events += run->item_events;
    agg->special_events += run->special_events;
    agg->death_events += run->death_events;
    agg->stage_events += run->stage_events;
    agg->meta_spent_events += run->meta_spent_events;
    agg->push_entries += run->push_entries;
    agg->score += run->score;
    if (run->max_round_reached > agg->max_round_reached) {
        agg->max_round_reached = run->max_round_reached;
    }
    agg->stage_seen_mask |= run->stage_seen_mask;
    run_stage_mask = run->stage_seen_mask & kEvalStageVariantMask;
    while (run_stage_mask != 0u) {
        run_stage_mask &= (run_stage_mask - 1u);
        ++run_unique_count;
    }
    agg->stage_unique_sum += (uint64_t)run_unique_count;
    if (run->ended_by_game_over) {
        agg->game_over_runs += 1;
    }
}

static int popcount_u32(uint32_t x) {
    int c = 0;
    while (x != 0u) {
        x &= (x - 1u);
        ++c;
    }
    return c;
}

static double clamp01(double v) {
    if (v < 0.0) {
        return 0.0;
    }
    if (v > 1.0) {
        return 1.0;
    }
    return v;
}

static EvalResult evaluate_fun(const AggregateMetrics *agg, const EvalProfile *profile) {
    EvalResult out;
    double runs = (agg->runs > 0u) ? (double)agg->runs : 1.0;
    double stage_seen_union = (double)popcount_u32(agg->stage_seen_mask & kEvalStageVariantMask);
    double avg_stage_unique = (double)agg->stage_unique_sum / runs;
    double score_target = profile->score_target;
    double stage_target = profile->stage_variety_target_ratio;
    double avg_deaths = (double)agg->death_events / runs;
    double challenge = 0.0;
    double direction_change_rate = 0.0;
    double active_ratio = 0.0;
    double fun = 0.0;

    memset(&out, 0, sizeof(out));

    if (agg->playing_ticks > 0u) {
        direction_change_rate = (double)agg->direction_changes / (double)agg->playing_ticks;
        active_ratio = (double)agg->active_input_ticks / (double)agg->playing_ticks;
    }

    out.avg_rounds = (double)agg->rounds_cleared / runs;
    out.avg_crushes = (double)agg->crush_events / runs;
    out.avg_items = (double)agg->item_events / runs;
    out.avg_deaths = avg_deaths;
    out.avg_score = (double)agg->score / runs;
    out.direction_change_rate = direction_change_rate;
    out.active_ratio = active_ratio;
    out.stage_unique_per_run = avg_stage_unique;
    out.stage_coverage_ratio = clamp01(stage_seen_union / (double)EVAL_STAGE_VARIANT_COUNT);
    out.stage_variety_ratio = clamp01(avg_stage_unique / (double)EVAL_STAGE_VARIANT_COUNT);

    if (score_target <= 1.0) {
        score_target = 15000.0;
    }
    if (stage_target <= 0.01) {
        stage_target = 1.0;
    }

    challenge = 1.0 - clamp01(fabs(avg_deaths - profile->challenge_target_deaths) / (profile->challenge_target_deaths + 1.0));

    out.round_component = profile->round_weight * clamp01(out.avg_rounds / 4.0);
    out.crush_component = profile->crush_weight * clamp01(out.avg_crushes / 10.0);
    out.item_component = profile->item_weight * clamp01(out.avg_items / 4.0);
    out.score_component = profile->score_weight * clamp01(out.avg_score / score_target);
    out.stage_component = profile->stage_weight * clamp01(out.stage_variety_ratio / stage_target);
    out.agency_component = profile->agency_weight * clamp01(direction_change_rate / profile->agency_target_rate);
    out.active_component = profile->activity_weight * clamp01(active_ratio / profile->activity_target_ratio);
    out.challenge_component = profile->challenge_weight * challenge;

    fun += out.round_component;
    fun += out.crush_component;
    fun += out.item_component;
    fun += out.score_component;
    fun += out.stage_component;
    fun += out.agency_component;
    fun += out.active_component;
    fun += out.challenge_component;

    if (out.avg_rounds < 0.5) {
        fun -= 10.0;
    }
    if (out.active_ratio < 0.20) {
        fun -= 8.0;
    }
    out.fun_score = clamp_int((int)lround(fun), 0, 100);
    return out;
}

static const char *fun_label(double score) {
    if (score >= 75.0) {
        return "strong fun signal";
    }
    if (score >= 55.0) {
        return "promising";
    }
    if (score >= 35.0) {
        return "needs tuning";
    }
    return "weak fun signal";
}

static void print_run_summary(int run_index, const RunMetrics *m) {
    printf(
        "run %02d  score=%6llu  rounds=%2llu  crush=%2llu  items=%2llu  deaths=%2llu  max_round=%2d  game_over=%s\n",
        run_index + 1,
        (unsigned long long)m->score,
        (unsigned long long)m->rounds_cleared,
        (unsigned long long)m->crush_events,
        (unsigned long long)m->item_events,
        (unsigned long long)m->death_events,
        m->max_round_reached,
        m->ended_by_game_over ? "yes" : "no");
}

static void print_eval_text(
    const AggregateMetrics *agg,
    const EvalResult *eval,
    const EvalProfile *profile,
    int max_ticks,
    int seed) {
    printf("Icepanic Fun Eval\n");
    printf("================\n");
    printf("runs: %llu  max_ticks_per_run: %d  seed: %d  profile: %s\n", (unsigned long long)agg->runs, max_ticks, seed, profile->name);
    printf("avg score: %.1f  avg rounds cleared: %.2f  max round reached: %d\n", eval->avg_score, eval->avg_rounds, agg->max_round_reached);
    printf("avg crushes: %.2f  avg items: %.2f  avg deaths: %.2f\n", eval->avg_crushes, eval->avg_items, eval->avg_deaths);
    printf("input activity: %.2f%%  direction-change rate: %.2f%%\n", eval->active_ratio * 100.0, eval->direction_change_rate * 100.0);
    printf(
        "stage variety: avg %.2f/5 (norm %.2f, union %.0f/5)  game-over runs: %llu/%llu\n",
        eval->stage_unique_per_run,
        eval->stage_variety_ratio,
        eval->stage_coverage_ratio * 5.0,
        (unsigned long long)agg->game_over_runs,
        (unsigned long long)agg->runs);
    printf("\n");
    printf("fun score: %.0f / 100  (%s)\n", eval->fun_score, fun_label(eval->fun_score));
    printf("components:\n");
    printf("  rounds    %+5.1f\n", eval->round_component);
    printf("  crushes   %+5.1f\n", eval->crush_component);
    printf("  items     %+5.1f\n", eval->item_component);
    printf("  score     %+5.1f\n", eval->score_component);
    printf("  stages    %+5.1f\n", eval->stage_component);
    printf("  agency    %+5.1f\n", eval->agency_component);
    printf("  activity  %+5.1f\n", eval->active_component);
    printf("  challenge %+5.1f\n", eval->challenge_component);
    printf("targets:\n");
    printf("  score target  %.0f\n", profile->score_target);
    printf("  stage target  %.2f\n", profile->stage_variety_target_ratio);
}

static void print_eval_json(
    const AggregateMetrics *agg,
    const EvalResult *eval,
    const EvalProfile *profile,
    int max_ticks,
    int seed) {
    printf("{\n");
    printf("  \"profile\": \"%s\",\n", profile->name);
    printf("  \"runs\": %llu,\n", (unsigned long long)agg->runs);
    printf("  \"max_ticks_per_run\": %d,\n", max_ticks);
    printf("  \"seed\": %d,\n", seed);
    printf("  \"fun_score\": %.0f,\n", eval->fun_score);
    printf("  \"fun_label\": \"%s\",\n", fun_label(eval->fun_score));
    printf("  \"max_round_reached\": %d,\n", agg->max_round_reached);
    printf("  \"game_over_runs\": %llu,\n", (unsigned long long)agg->game_over_runs);
    printf("  \"avg_score\": %.3f,\n", eval->avg_score);
    printf("  \"avg_rounds\": %.3f,\n", eval->avg_rounds);
    printf("  \"avg_crushes\": %.3f,\n", eval->avg_crushes);
    printf("  \"avg_items\": %.3f,\n", eval->avg_items);
    printf("  \"avg_deaths\": %.3f,\n", eval->avg_deaths);
    printf("  \"active_ratio\": %.6f,\n", eval->active_ratio);
    printf("  \"direction_change_rate\": %.6f,\n", eval->direction_change_rate);
    printf("  \"stage_variety_ratio\": %.6f,\n", eval->stage_variety_ratio);
    printf("  \"stage_unique_per_run\": %.6f,\n", eval->stage_unique_per_run);
    printf("  \"stage_coverage_ratio\": %.6f,\n", eval->stage_coverage_ratio);
    printf("  \"score_target\": %.3f,\n", profile->score_target);
    printf("  \"stage_variety_target_ratio\": %.6f,\n", profile->stage_variety_target_ratio);
    printf("  \"components\": {\n");
    printf("    \"rounds\": %.3f,\n", eval->round_component);
    printf("    \"crushes\": %.3f,\n", eval->crush_component);
    printf("    \"items\": %.3f,\n", eval->item_component);
    printf("    \"score\": %.3f,\n", eval->score_component);
    printf("    \"stages\": %.3f,\n", eval->stage_component);
    printf("    \"agency\": %.3f,\n", eval->agency_component);
    printf("    \"activity\": %.3f,\n", eval->active_component);
    printf("    \"challenge\": %.3f\n", eval->challenge_component);
    printf("  }\n");
    printf("}\n");
}

static void print_eval_csv(
    const AggregateMetrics *agg,
    const EvalResult *eval,
    const EvalProfile *profile,
    int max_ticks,
    int seed) {
    printf("profile,runs,max_ticks_per_run,seed,fun_score,fun_label,max_round_reached,game_over_runs,avg_score,avg_rounds,avg_crushes,avg_items,avg_deaths,active_ratio,direction_change_rate,stage_variety_ratio,stage_unique_per_run,stage_coverage_ratio,score_target,stage_variety_target_ratio,comp_rounds,comp_crushes,comp_items,comp_score,comp_stages,comp_agency,comp_activity,comp_challenge\n");
    printf(
        "%s,%llu,%d,%d,%.0f,%s,%d,%llu,%.3f,%.3f,%.3f,%.3f,%.3f,%.6f,%.6f,%.6f,%.6f,%.6f,%.3f,%.6f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n",
        profile->name,
        (unsigned long long)agg->runs,
        max_ticks,
        seed,
        eval->fun_score,
        fun_label(eval->fun_score),
        agg->max_round_reached,
        (unsigned long long)agg->game_over_runs,
        eval->avg_score,
        eval->avg_rounds,
        eval->avg_crushes,
        eval->avg_items,
        eval->avg_deaths,
        eval->active_ratio,
        eval->direction_change_rate,
        eval->stage_variety_ratio,
        eval->stage_unique_per_run,
        eval->stage_coverage_ratio,
        profile->score_target,
        profile->stage_variety_target_ratio,
        eval->round_component,
        eval->crush_component,
        eval->item_component,
        eval->score_component,
        eval->stage_component,
        eval->agency_component,
        eval->active_component,
        eval->challenge_component);
}

int main(int argc, char **argv) {
    int runs = DEFAULT_RUNS;
    int max_ticks = DEFAULT_MAX_TICKS;
    int seed = DEFAULT_SEED;
    const char *round_map_samples_out = NULL;
    bool verbose = false;
    OutputFormat out_format = OUTPUT_FORMAT_TEXT;
    EvalProfileId profile_id = EVAL_PROFILE_ARCADE;
    const EvalProfile *profile = &kEvalProfiles[EVAL_PROFILE_ARCADE];
    AggregateMetrics agg;
    RoundMapSampleList round_samples;

    memset(&agg, 0, sizeof(agg));
    round_samples_list_init(&round_samples);

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--runs") == 0) {
            if (i + 1 >= argc || !parse_int_arg(argv[i + 1], &runs) || runs <= 0) {
                fprintf(stderr, "Invalid --runs value.\n");
                return 2;
            }
            ++i;
        } else if (strcmp(argv[i], "--ticks") == 0) {
            if (i + 1 >= argc || !parse_int_arg(argv[i + 1], &max_ticks) || max_ticks <= 0) {
                fprintf(stderr, "Invalid --ticks value.\n");
                return 2;
            }
            ++i;
        } else if (strcmp(argv[i], "--seed") == 0) {
            if (i + 1 >= argc || !parse_int_arg(argv[i + 1], &seed)) {
                fprintf(stderr, "Invalid --seed value.\n");
                return 2;
            }
            ++i;
        } else if (strcmp(argv[i], "--profile") == 0) {
            const EvalProfile *found = NULL;
            size_t profile_idx = 0;
            if (i + 1 >= argc) {
                fprintf(stderr, "Missing --profile value.\n");
                return 2;
            }
            found = find_profile_by_name(argv[i + 1]);
            if (found == NULL) {
                fprintf(stderr, "Unknown profile: %s\n", argv[i + 1]);
                return 2;
            }
            profile = found;
            for (profile_idx = 0; profile_idx < sizeof(kEvalProfiles) / sizeof(kEvalProfiles[0]); ++profile_idx) {
                if (&kEvalProfiles[profile_idx] == found) {
                    profile_id = (EvalProfileId)profile_idx;
                    break;
                }
            }
            ++i;
        } else if (strcmp(argv[i], "--out-format") == 0) {
            if (i + 1 >= argc || !parse_output_format(argv[i + 1], &out_format)) {
                fprintf(stderr, "Invalid --out-format value.\n");
                return 2;
            }
            ++i;
        } else if (strcmp(argv[i], "--round-map-samples-out") == 0) {
            if (i + 1 >= argc || argv[i + 1] == NULL || argv[i + 1][0] == '\0') {
                fprintf(stderr, "Invalid --round-map-samples-out value.\n");
                round_samples_list_free(&round_samples);
                return 2;
            }
            round_map_samples_out = argv[i + 1];
            ++i;
        } else if (strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            round_samples_list_free(&round_samples);
            return 0;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            print_usage(argv[0]);
            round_samples_list_free(&round_samples);
            return 2;
        }
    }

    for (int run = 0; run < runs; ++run) {
        GameState gs;
        BotState bot;
        RunMetrics metrics;
        RoundTracker round_tracker;
        uint32_t run_seed = (uint32_t)seed ^ (0x9e3779b9u * (uint32_t)(run + 1));

        game_init(&gs, run_seed);
        bot.hold_dir = DIR_NONE;
        bot.hold_ticks = 0;
        bot.think_cooldown_ticks = 0;
        bot.idle_ticks = 0;
        bot.mine_tap_dir = DIR_NONE;
        bot.mine_tap_stage = 0;
        bot.rng_state = run_seed ^ 0xa5a5a5a5u;
        metrics_init(&metrics);
        metrics.max_round_reached = gs.round;
        round_tracker_reset(&round_tracker);
        if (round_map_samples_out != NULL) {
            round_tracker_begin(&round_tracker, &gs, profile->name, run_seed, run + 1);
        }

        for (int tick = 0; tick < max_ticks; ++tick) {
            GameState prev = gs;
            InputState in;
            uint32_t events = 0u;

            bot_build_input(&gs, &bot, profile, profile_id, &in);
            game_step(&gs, &in);
            events = game_consume_events(&gs);
            metrics_on_tick(&metrics, &prev, &gs, &in, events);
            if (round_map_samples_out != NULL) {
                if (!round_tracker_on_tick(
                        &round_tracker,
                        &prev,
                        &gs,
                        events,
                        profile->name,
                        run_seed,
                        run + 1,
                        &round_samples)) {
                    fprintf(stderr, "Failed to append round map samples.\n");
                    round_samples_list_free(&round_samples);
                    return 1;
                }
            }

            if ((events & GAME_EVENT_GAME_OVER) != 0u) {
                break;
            }
        }

        if (round_map_samples_out != NULL) {
            if (!round_tracker_finalize(&round_tracker, &round_samples)) {
                fprintf(stderr, "Failed to finalize round map samples.\n");
                round_samples_list_free(&round_samples);
                return 1;
            }
        }

        if (verbose) {
            print_run_summary(run, &metrics);
        }
        aggregate_add(&agg, &metrics);
    }

    {
        EvalResult eval = evaluate_fun(&agg, profile);
        if (out_format == OUTPUT_FORMAT_JSON) {
            print_eval_json(&agg, &eval, profile, max_ticks, seed);
        } else if (out_format == OUTPUT_FORMAT_CSV) {
            print_eval_csv(&agg, &eval, profile, max_ticks, seed);
        } else {
            print_eval_text(&agg, &eval, profile, max_ticks, seed);
        }
    }

    if (round_map_samples_out != NULL) {
        if (!write_round_map_samples_jsonl(round_map_samples_out, &round_samples)) {
            fprintf(stderr, "Failed to write round map samples JSONL (%s): %s\n", round_map_samples_out, strerror(errno));
            round_samples_list_free(&round_samples);
            return 1;
        }
    }

    round_samples_list_free(&round_samples);

    return 0;
}
