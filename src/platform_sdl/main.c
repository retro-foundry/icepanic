#include "game.h"

#include <SDL.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#endif

typedef enum SpriteId {
    SPR_FLOOR = 0,
    SPR_WALL = 1,
    SPR_BORDER = 2,
    SPR_BLOCK_ICE = 3,
    SPR_BLOCK_SPECIAL = 4,
    SPR_BLOCK_CRACKED = 5,
    SPR_BLOCK_UNBREAKABLE = 6,
    SPR_PLAYER_UP = 7,
    SPR_PLAYER_UP_ALT = 8,
    SPR_PLAYER_DOWN = 9,
    SPR_PLAYER_DOWN_ALT = 10,
    SPR_PLAYER_LEFT = 11,
    SPR_PLAYER_LEFT_ALT = 12,
    SPR_PLAYER_RIGHT = 13,
    SPR_PLAYER_RIGHT_ALT = 14,
    SPR_ENEMY_A = 15,
    SPR_ENEMY_A_ALT = 16,
    SPR_ENEMY_B = 17,
    SPR_ENEMY_B_ALT = 18,
    SPR_ENEMY_A_DEATH_0 = 19,
    SPR_ENEMY_A_DEATH_1 = 20,
    SPR_ENEMY_B_DEATH_0 = 21,
    SPR_ENEMY_B_DEATH_1 = 22,
    SPR_ITEM_FISH = 23,
    SPR_ITEM_EGG = 24,
    SPR_ITEM_GEM = 25,
    SPR_ITEM_FRUIT = 26,
    SPR_ITEM_MYSTERY = 27,
    SPR_HUD_LIFE = 28,
    SPR_EFFECT_SPARK = 29,
    SPR_MINE_0 = 30,
    SPR_MINE_1 = 31,
    SPR_MINE_2 = 32,
    SPR_COUNT = 33
} SpriteId;

typedef struct TileSet {
    uint8_t palette[32][3];
    uint8_t tiles[SPR_COUNT][GAME_TILE_SIZE * GAME_TILE_SIZE];
} TileSet;

typedef struct TitleOverlay {
    bool loaded;
    int width;
    int height;
    int anchor_x;
    int anchor_y;
    uint8_t pixels[GAME_LOGICAL_WIDTH * GAME_LOGICAL_HEIGHT];
} TitleOverlay;

typedef struct InputCapture {
    bool up;
    bool down;
    bool left;
    bool right;
    bool start;
    bool fire_pressed;
    bool fire_released;
    bool toggle_upgrade_panel;
    bool trigger_sound_test;
    Direction newest_press;
    bool quit;
} InputCapture;

typedef struct FrameDumpConfig {
    bool enabled;
    bool show_help_only;
    bool initialized;
    bool failed;
    bool low_intensity_viz;
    bool session_metrics_enabled;
    int max_frames;
    int frames_written;
    char out_dir[260];
    char session_metrics_path[260];
} FrameDumpConfig;

typedef struct SessionMetrics {
    uint64_t ticks_simulated;
    uint64_t playing_ticks;
    uint64_t active_input_ticks;
    uint64_t direction_changes;
    uint64_t crush_events;
    uint64_t item_collect_events;
    uint64_t timer_pulse_events;
    uint64_t special_alignment_events;
    uint64_t player_died_events;
    uint64_t round_clear_events;
    uint64_t round_start_events;
    uint64_t game_over_events;
    uint64_t block_impact_events;
    uint64_t block_push_events;
    uint64_t meta_banked_events;
    uint64_t stage_modifier_events;
    uint64_t meta_spent_events;
    uint64_t mine_placed_events;
    uint64_t mine_exploded_events;
    uint64_t life_lost_events;
    uint64_t combo_events;
    uint32_t stage_modifier_mask;
    int max_round_reached;
    Direction last_pressed_dir;
} SessionMetrics;

typedef struct SfxState {
    SDL_AudioDeviceID device;
    SDL_AudioSpec spec;
    bool enabled;
    bool owns_audio_subsystem;
} SfxState;

typedef struct SfxStep {
    int hz;
    int duration_ms;
    int amplitude;
    int rest_ms;
} SfxStep;

typedef struct HiScoreEntry {
    char initials[4];
    uint32_t score;
} HiScoreEntry;

typedef struct HighScoreTable {
    HiScoreEntry entries[5];
} HighScoreTable;

typedef struct HighScoreEntryState {
    bool active;
    bool confirm_armed;
    int insert_index;
    uint32_t pending_score;
    char initials[4];
    int cursor;
    int blink_ticks;
} HighScoreEntryState;

typedef struct MetaProfile {
    uint32_t shards;
    uint32_t progress_points;
} MetaProfile;

enum {
    HUD_FP_ONE = 1 << 8,
    RENDER_FP_SHIFT = 8,
    RENDER_FP_ONE = 1 << RENDER_FP_SHIFT,
    RENDER_TILE_FP = GAME_TILE_SIZE * RENDER_FP_ONE,
    RENDER_SNAP_THRESHOLD_FP = RENDER_TILE_FP - (RENDER_FP_ONE / 4),
    TITLE_OVERLAY_TRANSPARENT_INDEX = 255
};

static const uint8_t kDefaultPalette[32][3] = {
    {0, 0, 0},       {23, 23, 23},   {60, 64, 66},   {124, 126, 128},
    {158, 159, 160}, {95, 53, 10},   {125, 71, 13},  {229, 95, 33},
    {241, 176, 33},  {222, 215, 201}, {129, 184, 109}, {46, 133, 58},
    {29, 93, 44},    {52, 176, 196}, {13, 39, 201},  {9, 25, 109},
    {26, 57, 190},   {70, 110, 210}, {117, 79, 194}, {97, 32, 146},
    {71, 16, 116},   {123, 11, 62},  {180, 27, 47},  {226, 38, 40},
    {218, 114, 121}, {36, 68, 184},  {61, 107, 210}, {229, 169, 164},
    {29, 48, 79},    {85, 154, 255}, {255, 255, 255}, {14, 25, 40},
};

static const SpriteId kTerrainSprites[3] = {
    SPR_FLOOR,
    SPR_WALL,
    SPR_BORDER,
};

static const SpriteId kBlockSprites[5] = {
    SPR_FLOOR,
    SPR_BLOCK_ICE,
    SPR_BLOCK_ICE,
    SPR_BLOCK_ICE,
    SPR_BLOCK_ICE,
};

static const SpriteId kItemSprites[6] = {
    SPR_FLOOR,
    SPR_ITEM_FISH,
    SPR_ITEM_EGG,
    SPR_ITEM_GEM,
    SPR_ITEM_FRUIT,
    SPR_ITEM_MYSTERY,
};

static const HiScoreEntry kOpeningHiScores[] = {
    {"ACE", 50000u},
    {"ICE", 35000u},
    {"PAN", 24000u},
    {"FOX", 16000u},
    {"JET", 9000u},
};

static const char *kHighScorePaths[] = {
    "assets/high_scores.sav",
    "../assets/high_scores.sav",
    "../../assets/high_scores.sav",
};

static const uint8_t kDigit3x5[10][5] = {
    {0x7, 0x5, 0x5, 0x5, 0x7}, /* 0 */
    {0x2, 0x6, 0x2, 0x2, 0x7}, /* 1 */
    {0x7, 0x1, 0x7, 0x4, 0x7}, /* 2 */
    {0x7, 0x1, 0x7, 0x1, 0x7}, /* 3 */
    {0x5, 0x5, 0x7, 0x1, 0x1}, /* 4 */
    {0x7, 0x4, 0x7, 0x1, 0x7}, /* 5 */
    {0x7, 0x4, 0x7, 0x5, 0x7}, /* 6 */
    {0x7, 0x1, 0x1, 0x1, 0x1}, /* 7 */
    {0x7, 0x5, 0x7, 0x5, 0x7}, /* 8 */
    {0x7, 0x5, 0x7, 0x1, 0x7}, /* 9 */
};

static const char *kMetaShardPaths[] = {
    "assets/meta_shards.sav",
    "../assets/meta_shards.sav",
    "../../assets/meta_shards.sav",
};

typedef enum HudAccentId {
    HUD_ACCENT_RULE_TOP = 0,
    HUD_ACCENT_RULE_BOTTOM,
    HUD_ACCENT_BG_FILL,
    HUD_ACCENT_BG_STRIPE,
    HUD_ACCENT_SEPARATOR_DARK,
    HUD_ACCENT_SEPARATOR_LIGHT,
    HUD_ACCENT_OUTLINE,
    HUD_ACCENT_LABEL,
    HUD_ACCENT_SCORE,
    HUD_ACCENT_ROUND,
    HUD_ACCENT_TIMER,
    HUD_ACCENT_TIMER_DANGER_A,
    HUD_ACCENT_TIMER_DANGER_B,
    HUD_ACCENT_TIMER_DANGER_OUTLINE_A,
    HUD_ACCENT_TIMER_DANGER_OUTLINE_B,
    HUD_ACCENT_BOMB,
    HUD_ACCENT_LIFE,
    HUD_ACCENT_GAIN_PRIMARY,
    HUD_ACCENT_GAIN_FLASH,
    HUD_ACCENT_UPGRADE_PANEL_BORDER,
    HUD_ACCENT_UPGRADE_PANEL_FILL,
    HUD_ACCENT_UPGRADE_TEXT,
    HUD_ACCENT_UPGRADE_HINT,
    HUD_ACCENT_DETAIL_TITLE,
    HUD_ACCENT_COUNT
} HudAccentId;

static const uint8_t kHudAccentsDefault[HUD_ACCENT_COUNT] = {
    3,  /* HUD_ACCENT_RULE_TOP */
    2,  /* HUD_ACCENT_RULE_BOTTOM */
    1,  /* HUD_ACCENT_BG_FILL */
    2,  /* HUD_ACCENT_BG_STRIPE */
    2,  /* HUD_ACCENT_SEPARATOR_DARK */
    3,  /* HUD_ACCENT_SEPARATOR_LIGHT */
    3,  /* HUD_ACCENT_OUTLINE */
    7,  /* HUD_ACCENT_LABEL */
    29, /* HUD_ACCENT_SCORE */
    18, /* HUD_ACCENT_ROUND */
    14, /* HUD_ACCENT_TIMER */
    23, /* HUD_ACCENT_TIMER_DANGER_A */
    12, /* HUD_ACCENT_TIMER_DANGER_B */
    30, /* HUD_ACCENT_TIMER_DANGER_OUTLINE_A */
    3,  /* HUD_ACCENT_TIMER_DANGER_OUTLINE_B */
    12, /* HUD_ACCENT_BOMB */
    23, /* HUD_ACCENT_LIFE */
    8,  /* HUD_ACCENT_GAIN_PRIMARY */
    30, /* HUD_ACCENT_GAIN_FLASH */
    3,  /* HUD_ACCENT_UPGRADE_PANEL_BORDER */
    1,  /* HUD_ACCENT_UPGRADE_PANEL_FILL */
    29, /* HUD_ACCENT_UPGRADE_TEXT */
    30, /* HUD_ACCENT_UPGRADE_HINT */
    30, /* HUD_ACCENT_DETAIL_TITLE */
};

static const uint8_t kHudAccentsLowIntensity[HUD_ACCENT_COUNT] = {
    2,  /* HUD_ACCENT_RULE_TOP */
    2,  /* HUD_ACCENT_RULE_BOTTOM */
    1,  /* HUD_ACCENT_BG_FILL */
    1,  /* HUD_ACCENT_BG_STRIPE */
    2,  /* HUD_ACCENT_SEPARATOR_DARK */
    2,  /* HUD_ACCENT_SEPARATOR_LIGHT */
    3,  /* HUD_ACCENT_OUTLINE */
    7,  /* HUD_ACCENT_LABEL */
    29, /* HUD_ACCENT_SCORE */
    18, /* HUD_ACCENT_ROUND */
    14, /* HUD_ACCENT_TIMER */
    23, /* HUD_ACCENT_TIMER_DANGER_A */
    12, /* HUD_ACCENT_TIMER_DANGER_B */
    30, /* HUD_ACCENT_TIMER_DANGER_OUTLINE_A */
    3,  /* HUD_ACCENT_TIMER_DANGER_OUTLINE_B */
    12, /* HUD_ACCENT_BOMB */
    23, /* HUD_ACCENT_LIFE */
    8,  /* HUD_ACCENT_GAIN_PRIMARY */
    8,  /* HUD_ACCENT_GAIN_FLASH */
    3,  /* HUD_ACCENT_UPGRADE_PANEL_BORDER */
    1,  /* HUD_ACCENT_UPGRADE_PANEL_FILL */
    29, /* HUD_ACCENT_UPGRADE_TEXT */
    30, /* HUD_ACCENT_UPGRADE_HINT */
    30, /* HUD_ACCENT_DETAIL_TITLE */
};

static const uint8_t *hud_accents(bool low_intensity_viz) {
    return low_intensity_viz ? kHudAccentsLowIntensity : kHudAccentsDefault;
}

static int g_ui_anim_tick = 0;
static void draw_title_star_sparkle(uint8_t *index_buffer, int cx, int cy, int phase, bool low_intensity_viz);

static int clamp_int(int value, int lo, int hi) {
    if (value < lo) {
        return lo;
    }
    if (value > hi) {
        return hi;
    }
    return value;
}

static void tile_fill(TileSet *ts, SpriteId id, uint8_t color) {
    for (int i = 0; i < GAME_TILE_SIZE * GAME_TILE_SIZE; ++i) {
        ts->tiles[id][i] = color;
    }
}

static void tile_rect(TileSet *ts, SpriteId id, int x0, int y0, int w, int h, uint8_t color) {
    for (int y = y0; y < y0 + h; ++y) {
        for (int x = x0; x < x0 + w; ++x) {
            if (x < 0 || y < 0 || x >= GAME_TILE_SIZE || y >= GAME_TILE_SIZE) {
                continue;
            }
            ts->tiles[id][y * GAME_TILE_SIZE + x] = color;
        }
    }
}

static void tile_outline(TileSet *ts, SpriteId id, uint8_t color) {
    for (int x = 0; x < GAME_TILE_SIZE; ++x) {
        ts->tiles[id][x] = color;
        ts->tiles[id][(GAME_TILE_SIZE - 1) * GAME_TILE_SIZE + x] = color;
    }
    for (int y = 0; y < GAME_TILE_SIZE; ++y) {
        ts->tiles[id][y * GAME_TILE_SIZE] = color;
        ts->tiles[id][y * GAME_TILE_SIZE + (GAME_TILE_SIZE - 1)] = color;
    }
}

static void tile_line(TileSet *ts, SpriteId id, int x0, int y0, int x1, int y1, uint8_t color) {
    int dx = abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (true) {
        if (x0 >= 0 && y0 >= 0 && x0 < GAME_TILE_SIZE && y0 < GAME_TILE_SIZE) {
            ts->tiles[id][y0 * GAME_TILE_SIZE + x0] = color;
        }
        if (x0 == x1 && y0 == y1) {
            break;
        }
        const int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

static void build_fallback_tileset(TileSet *ts) {
    memset(ts, 0, sizeof(*ts));
    memcpy(ts->palette, kDefaultPalette, sizeof(kDefaultPalette));

    tile_fill(ts, SPR_FLOOR, 2);
    for (int y = 0; y < GAME_TILE_SIZE; ++y) {
        for (int x = 0; x < GAME_TILE_SIZE; ++x) {
            const int r = (x * 17 + y * 29 + x * y * 3) & 255;
            if (r == 0) {
                ts->tiles[SPR_FLOOR][y * GAME_TILE_SIZE + x] = 4;
            } else if (r == 1) {
                ts->tiles[SPR_FLOOR][y * GAME_TILE_SIZE + x] = 5;
            }
        }
    }
    ts->tiles[SPR_FLOOR][5 * GAME_TILE_SIZE + 3] = 5;
    ts->tiles[SPR_FLOOR][4 * GAME_TILE_SIZE + 11] = 5;
    ts->tiles[SPR_FLOOR][10 * GAME_TILE_SIZE + 7] = 4;

    tile_fill(ts, SPR_WALL, 4);
    for (int y = 0; y < GAME_TILE_SIZE; y += 4) {
        tile_rect(ts, SPR_WALL, 0, y, GAME_TILE_SIZE, 1, 2);
    }
    for (int x = 0; x < GAME_TILE_SIZE; x += 4) {
        tile_rect(ts, SPR_WALL, x, 0, 1, GAME_TILE_SIZE, 2);
    }
    for (int y = 0; y < GAME_TILE_SIZE; ++y) {
        for (int x = 0; x < GAME_TILE_SIZE; ++x) {
            if (((x * 5 + y * 3) & 7) == 0) {
                ts->tiles[SPR_WALL][y * GAME_TILE_SIZE + x] = 5;
            }
        }
    }
    tile_line(ts, SPR_WALL, 3, 2, 12, 11, 7);
    tile_line(ts, SPR_WALL, 6, 3, 5, 13, 6);
    tile_line(ts, SPR_WALL, 10, 1, 14, 5, 6);
    ts->tiles[SPR_WALL][2 * GAME_TILE_SIZE + 2] = 31;
    ts->tiles[SPR_WALL][2 * GAME_TILE_SIZE + 3] = 9;
    ts->tiles[SPR_WALL][3 * GAME_TILE_SIZE + 2] = 9;
    ts->tiles[SPR_WALL][6 * GAME_TILE_SIZE + 10] = 31;
    ts->tiles[SPR_WALL][6 * GAME_TILE_SIZE + 9] = 9;
    ts->tiles[SPR_WALL][7 * GAME_TILE_SIZE + 9] = 9;
    ts->tiles[SPR_WALL][10 * GAME_TILE_SIZE + 6] = 31;
    ts->tiles[SPR_WALL][10 * GAME_TILE_SIZE + 5] = 9;
    ts->tiles[SPR_WALL][11 * GAME_TILE_SIZE + 5] = 9;
    for (int sx = 1; sx < GAME_TILE_SIZE; sx += 4) {
        ts->tiles[SPR_WALL][1 * GAME_TILE_SIZE + sx] = 9;
    }
    for (int sy = 1; sy < GAME_TILE_SIZE; sy += 4) {
        ts->tiles[SPR_WALL][sy * GAME_TILE_SIZE + 1] = 9;
    }

    tile_fill(ts, SPR_BORDER, 7);
    tile_outline(ts, SPR_BORDER, 30);

    tile_fill(ts, SPR_BLOCK_ICE, 8);
    tile_outline(ts, SPR_BLOCK_ICE, 31);
    tile_rect(ts, SPR_BLOCK_ICE, 2, 2, 12, 12, 7);
    tile_rect(ts, SPR_BLOCK_ICE, 4, 4, 8, 8, 9);

    tile_fill(ts, SPR_BLOCK_SPECIAL, 8);
    tile_outline(ts, SPR_BLOCK_SPECIAL, 31);
    tile_rect(ts, SPR_BLOCK_SPECIAL, 5, 5, 6, 6, 29);

    tile_fill(ts, SPR_BLOCK_CRACKED, 8);
    tile_outline(ts, SPR_BLOCK_CRACKED, 31);
    tile_rect(ts, SPR_BLOCK_CRACKED, 3, 7, 10, 1, 25);
    tile_rect(ts, SPR_BLOCK_CRACKED, 8, 3, 1, 10, 25);

    tile_fill(ts, SPR_BLOCK_UNBREAKABLE, 5);
    tile_outline(ts, SPR_BLOCK_UNBREAKABLE, 31);
    tile_rect(ts, SPR_BLOCK_UNBREAKABLE, 1, 1, 14, 14, 7);
    tile_rect(ts, SPR_BLOCK_UNBREAKABLE, 2, 2, 12, 12, 6);
    tile_rect(ts, SPR_BLOCK_UNBREAKABLE, 5, 5, 6, 6, 5);
    ts->tiles[SPR_BLOCK_UNBREAKABLE][3 * GAME_TILE_SIZE + 3] = 31;
    ts->tiles[SPR_BLOCK_UNBREAKABLE][3 * GAME_TILE_SIZE + 4] = 31;
    ts->tiles[SPR_BLOCK_UNBREAKABLE][4 * GAME_TILE_SIZE + 3] = 31;
    ts->tiles[SPR_BLOCK_UNBREAKABLE][10 * GAME_TILE_SIZE + 10] = 9;
    ts->tiles[SPR_BLOCK_UNBREAKABLE][10 * GAME_TILE_SIZE + 11] = 9;
    ts->tiles[SPR_BLOCK_UNBREAKABLE][11 * GAME_TILE_SIZE + 10] = 9;
    ts->tiles[SPR_BLOCK_UNBREAKABLE][8 * GAME_TILE_SIZE + 8] = 9;

    tile_fill(ts, SPR_PLAYER_UP, 0);
    tile_rect(ts, SPR_PLAYER_UP, 4, 3, 8, 9, 30);
    tile_rect(ts, SPR_PLAYER_UP, 6, 1, 4, 3, 24);
    tile_rect(ts, SPR_PLAYER_UP, 3, 10, 10, 2, 8);
    memcpy(ts->tiles[SPR_PLAYER_UP_ALT], ts->tiles[SPR_PLAYER_UP], sizeof(ts->tiles[SPR_PLAYER_UP]));
    tile_rect(ts, SPR_PLAYER_UP_ALT, 3, 10, 10, 2, 0);
    tile_rect(ts, SPR_PLAYER_UP_ALT, 2, 11, 4, 2, 8);
    tile_rect(ts, SPR_PLAYER_UP_ALT, 10, 11, 4, 2, 8);

    tile_fill(ts, SPR_PLAYER_DOWN, 0);
    tile_rect(ts, SPR_PLAYER_DOWN, 4, 3, 8, 9, 30);
    tile_rect(ts, SPR_PLAYER_DOWN, 6, 8, 4, 3, 24);
    tile_rect(ts, SPR_PLAYER_DOWN, 3, 10, 10, 2, 8);
    memcpy(ts->tiles[SPR_PLAYER_DOWN_ALT], ts->tiles[SPR_PLAYER_DOWN], sizeof(ts->tiles[SPR_PLAYER_DOWN]));
    tile_rect(ts, SPR_PLAYER_DOWN_ALT, 3, 10, 10, 2, 0);
    tile_rect(ts, SPR_PLAYER_DOWN_ALT, 2, 11, 4, 2, 8);
    tile_rect(ts, SPR_PLAYER_DOWN_ALT, 10, 11, 4, 2, 8);

    tile_fill(ts, SPR_PLAYER_LEFT, 0);
    tile_rect(ts, SPR_PLAYER_LEFT, 4, 3, 8, 9, 30);
    tile_rect(ts, SPR_PLAYER_LEFT, 3, 6, 3, 3, 24);
    tile_rect(ts, SPR_PLAYER_LEFT, 3, 10, 10, 2, 8);
    memcpy(ts->tiles[SPR_PLAYER_LEFT_ALT], ts->tiles[SPR_PLAYER_LEFT], sizeof(ts->tiles[SPR_PLAYER_LEFT]));
    tile_rect(ts, SPR_PLAYER_LEFT_ALT, 3, 10, 10, 2, 0);
    tile_rect(ts, SPR_PLAYER_LEFT_ALT, 2, 11, 3, 2, 8);
    tile_rect(ts, SPR_PLAYER_LEFT_ALT, 9, 11, 4, 2, 8);
    tile_rect(ts, SPR_PLAYER_LEFT_ALT, 10, 6, 2, 2, 10);

    tile_fill(ts, SPR_PLAYER_RIGHT, 0);
    tile_rect(ts, SPR_PLAYER_RIGHT, 4, 3, 8, 9, 30);
    tile_rect(ts, SPR_PLAYER_RIGHT, 10, 6, 3, 3, 24);
    tile_rect(ts, SPR_PLAYER_RIGHT, 3, 10, 10, 2, 8);
    memcpy(ts->tiles[SPR_PLAYER_RIGHT_ALT], ts->tiles[SPR_PLAYER_RIGHT], sizeof(ts->tiles[SPR_PLAYER_RIGHT]));
    tile_rect(ts, SPR_PLAYER_RIGHT_ALT, 3, 10, 10, 2, 0);
    tile_rect(ts, SPR_PLAYER_RIGHT_ALT, 3, 11, 4, 2, 8);
    tile_rect(ts, SPR_PLAYER_RIGHT_ALT, 11, 11, 3, 2, 8);
    tile_rect(ts, SPR_PLAYER_RIGHT_ALT, 4, 6, 2, 2, 10);

    tile_fill(ts, SPR_ENEMY_A, 0);
    tile_rect(ts, SPR_ENEMY_A, 3, 4, 10, 8, 23);
    tile_rect(ts, SPR_ENEMY_A, 5, 8, 2, 2, 30);
    tile_rect(ts, SPR_ENEMY_A, 9, 8, 2, 2, 30);
    memcpy(ts->tiles[SPR_ENEMY_A_ALT], ts->tiles[SPR_ENEMY_A], sizeof(ts->tiles[SPR_ENEMY_A]));
    tile_rect(ts, SPR_ENEMY_A_ALT, 4, 11, 2, 2, 0);
    tile_rect(ts, SPR_ENEMY_A_ALT, 10, 11, 2, 2, 0);
    tile_rect(ts, SPR_ENEMY_A_ALT, 3, 11, 2, 2, 23);
    tile_rect(ts, SPR_ENEMY_A_ALT, 11, 11, 2, 2, 23);
    ts->tiles[SPR_ENEMY_A_ALT][6 * GAME_TILE_SIZE + 8] = 19;

    tile_fill(ts, SPR_ENEMY_B, 0);
    tile_rect(ts, SPR_ENEMY_B, 3, 4, 10, 8, 22);
    tile_rect(ts, SPR_ENEMY_B, 5, 8, 2, 2, 30);
    tile_rect(ts, SPR_ENEMY_B, 9, 8, 2, 2, 30);
    memcpy(ts->tiles[SPR_ENEMY_B_ALT], ts->tiles[SPR_ENEMY_B], sizeof(ts->tiles[SPR_ENEMY_B]));
    tile_rect(ts, SPR_ENEMY_B_ALT, 4, 11, 2, 2, 0);
    tile_rect(ts, SPR_ENEMY_B_ALT, 10, 11, 2, 2, 0);
    tile_rect(ts, SPR_ENEMY_B_ALT, 3, 11, 2, 2, 22);
    tile_rect(ts, SPR_ENEMY_B_ALT, 11, 11, 2, 2, 22);
    ts->tiles[SPR_ENEMY_B_ALT][6 * GAME_TILE_SIZE + 8] = 16;

    tile_fill(ts, SPR_ENEMY_A_DEATH_0, 0);
    tile_rect(ts, SPR_ENEMY_A_DEATH_0, 3, 9, 10, 4, 18);
    tile_rect(ts, SPR_ENEMY_A_DEATH_0, 4, 8, 8, 2, 19);
    ts->tiles[SPR_ENEMY_A_DEATH_0][9 * GAME_TILE_SIZE + 6] = 31;
    ts->tiles[SPR_ENEMY_A_DEATH_0][9 * GAME_TILE_SIZE + 9] = 31;
    ts->tiles[SPR_ENEMY_A_DEATH_0][10 * GAME_TILE_SIZE + 6] = 2;
    ts->tiles[SPR_ENEMY_A_DEATH_0][10 * GAME_TILE_SIZE + 9] = 2;
    tile_fill(ts, SPR_ENEMY_A_DEATH_1, 0);
    tile_rect(ts, SPR_ENEMY_A_DEATH_1, 5, 10, 6, 2, 18);
    tile_rect(ts, SPR_ENEMY_A_DEATH_1, 4, 11, 8, 1, 19);
    ts->tiles[SPR_ENEMY_A_DEATH_1][8 * GAME_TILE_SIZE + 5] = 23;
    ts->tiles[SPR_ENEMY_A_DEATH_1][9 * GAME_TILE_SIZE + 10] = 23;
    ts->tiles[SPR_ENEMY_A_DEATH_1][10 * GAME_TILE_SIZE + 4] = 30;
    ts->tiles[SPR_ENEMY_A_DEATH_1][11 * GAME_TILE_SIZE + 11] = 30;

    tile_fill(ts, SPR_ENEMY_B_DEATH_0, 0);
    tile_rect(ts, SPR_ENEMY_B_DEATH_0, 3, 9, 10, 4, 15);
    tile_rect(ts, SPR_ENEMY_B_DEATH_0, 4, 8, 8, 2, 16);
    ts->tiles[SPR_ENEMY_B_DEATH_0][9 * GAME_TILE_SIZE + 6] = 31;
    ts->tiles[SPR_ENEMY_B_DEATH_0][9 * GAME_TILE_SIZE + 9] = 31;
    ts->tiles[SPR_ENEMY_B_DEATH_0][10 * GAME_TILE_SIZE + 6] = 3;
    ts->tiles[SPR_ENEMY_B_DEATH_0][10 * GAME_TILE_SIZE + 9] = 3;
    tile_fill(ts, SPR_ENEMY_B_DEATH_1, 0);
    tile_rect(ts, SPR_ENEMY_B_DEATH_1, 5, 10, 6, 2, 15);
    tile_rect(ts, SPR_ENEMY_B_DEATH_1, 4, 11, 8, 1, 16);
    ts->tiles[SPR_ENEMY_B_DEATH_1][8 * GAME_TILE_SIZE + 5] = 15;
    ts->tiles[SPR_ENEMY_B_DEATH_1][9 * GAME_TILE_SIZE + 10] = 15;
    ts->tiles[SPR_ENEMY_B_DEATH_1][10 * GAME_TILE_SIZE + 4] = 30;
    ts->tiles[SPR_ENEMY_B_DEATH_1][11 * GAME_TILE_SIZE + 11] = 30;

    tile_fill(ts, SPR_ITEM_FISH, 0);
    tile_rect(ts, SPR_ITEM_FISH, 3, 6, 9, 4, 13);
    tile_rect(ts, SPR_ITEM_FISH, 10, 7, 4, 2, 13);
    tile_rect(ts, SPR_ITEM_FISH, 4, 7, 2, 1, 30);

    tile_fill(ts, SPR_ITEM_EGG, 0);
    tile_rect(ts, SPR_ITEM_EGG, 5, 4, 6, 8, 30);
    tile_rect(ts, SPR_ITEM_EGG, 6, 5, 4, 6, 9);

    tile_fill(ts, SPR_ITEM_GEM, 0);
    tile_rect(ts, SPR_ITEM_GEM, 6, 3, 4, 2, 18);
    tile_rect(ts, SPR_ITEM_GEM, 4, 5, 8, 6, 18);
    tile_rect(ts, SPR_ITEM_GEM, 6, 11, 4, 2, 19);

    tile_fill(ts, SPR_ITEM_FRUIT, 0);
    tile_rect(ts, SPR_ITEM_FRUIT, 5, 5, 6, 6, 23);
    tile_rect(ts, SPR_ITEM_FRUIT, 7, 3, 2, 2, 10);
    tile_rect(ts, SPR_ITEM_FRUIT, 9, 4, 2, 1, 11);

    tile_fill(ts, SPR_ITEM_MYSTERY, 0);
    tile_rect(ts, SPR_ITEM_MYSTERY, 4, 4, 8, 8, 8);
    tile_rect(ts, SPR_ITEM_MYSTERY, 7, 5, 2, 3, 1);
    tile_rect(ts, SPR_ITEM_MYSTERY, 7, 9, 2, 1, 1);

    tile_fill(ts, SPR_HUD_LIFE, 0);
    tile_rect(ts, SPR_HUD_LIFE, 4, 5, 8, 6, 23);
    tile_rect(ts, SPR_HUD_LIFE, 5, 4, 2, 2, 23);
    tile_rect(ts, SPR_HUD_LIFE, 9, 4, 2, 2, 23);

    tile_fill(ts, SPR_EFFECT_SPARK, 0);
    tile_rect(ts, SPR_EFFECT_SPARK, 7, 3, 2, 10, 30);
    tile_rect(ts, SPR_EFFECT_SPARK, 3, 7, 10, 2, 30);

    tile_fill(ts, SPR_MINE_0, 0);
    tile_rect(ts, SPR_MINE_0, 5, 5, 6, 6, 3);
    tile_rect(ts, SPR_MINE_0, 6, 6, 4, 4, 29);
    tile_rect(ts, SPR_MINE_0, 8, 3, 1, 2, 23);
    tile_rect(ts, SPR_MINE_0, 9, 2, 2, 1, 23);
    ts->tiles[SPR_MINE_0][6 * GAME_TILE_SIZE + 7] = 31;
    ts->tiles[SPR_MINE_0][6 * GAME_TILE_SIZE + 8] = 31;
    ts->tiles[SPR_MINE_0][7 * GAME_TILE_SIZE + 7] = 2;
    ts->tiles[SPR_MINE_0][7 * GAME_TILE_SIZE + 8] = 2;
    ts->tiles[SPR_MINE_0][1 * GAME_TILE_SIZE + 10] = 12;

    memcpy(ts->tiles[SPR_MINE_1], ts->tiles[SPR_MINE_0], sizeof(ts->tiles[SPR_MINE_0]));
    tile_rect(ts, SPR_MINE_1, 5, 5, 6, 6, 4);
    tile_rect(ts, SPR_MINE_1, 6, 6, 4, 4, 30);
    ts->tiles[SPR_MINE_1][1 * GAME_TILE_SIZE + 10] = 8;
    ts->tiles[SPR_MINE_1][2 * GAME_TILE_SIZE + 11] = 8;
    ts->tiles[SPR_MINE_1][3 * GAME_TILE_SIZE + 10] = 12;

    memcpy(ts->tiles[SPR_MINE_2], ts->tiles[SPR_MINE_1], sizeof(ts->tiles[SPR_MINE_1]));
    tile_rect(ts, SPR_MINE_2, 5, 5, 6, 6, 18);
    tile_rect(ts, SPR_MINE_2, 6, 6, 4, 4, 19);
    ts->tiles[SPR_MINE_2][1 * GAME_TILE_SIZE + 10] = 8;
    ts->tiles[SPR_MINE_2][2 * GAME_TILE_SIZE + 11] = 8;
    ts->tiles[SPR_MINE_2][2 * GAME_TILE_SIZE + 12] = 30;
    ts->tiles[SPR_MINE_2][3 * GAME_TILE_SIZE + 10] = 30;
}

static bool load_tileset_file(TileSet *ts, const char *path) {
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return false;
    }

    uint8_t header[14];
    if (fread(header, 1, sizeof(header), f) != sizeof(header)) {
        fclose(f);
        return false;
    }

    if (memcmp(header, "ITLS", 4) != 0) {
        fclose(f);
        return false;
    }

    const uint16_t version = (uint16_t)(header[4] | (header[5] << 8));
    const uint16_t tile_count = (uint16_t)(header[6] | (header[7] << 8));
    const uint16_t tile_w = (uint16_t)(header[8] | (header[9] << 8));
    const uint16_t tile_h = (uint16_t)(header[10] | (header[11] << 8));
    const uint16_t palette_count = (uint16_t)(header[12] | (header[13] << 8));

    if (version != 1 || tile_w != GAME_TILE_SIZE || tile_h != GAME_TILE_SIZE || palette_count != 32) {
        fclose(f);
        return false;
    }

    uint8_t palette_bytes[32 * 3];
    if (fread(palette_bytes, 1, sizeof(palette_bytes), f) != sizeof(palette_bytes)) {
        fclose(f);
        return false;
    }
    for (int i = 0; i < 32; ++i) {
        ts->palette[i][0] = palette_bytes[i * 3 + 0];
        ts->palette[i][1] = palette_bytes[i * 3 + 1];
        ts->palette[i][2] = palette_bytes[i * 3 + 2];
    }

    const int to_read = (tile_count < SPR_COUNT) ? tile_count : SPR_COUNT;
    for (int i = 0; i < to_read; ++i) {
        if (fread(ts->tiles[i], 1, GAME_TILE_SIZE * GAME_TILE_SIZE, f) != GAME_TILE_SIZE * GAME_TILE_SIZE) {
            fclose(f);
            return false;
        }
    }

    fclose(f);
    return true;
}

static void load_tileset(TileSet *ts) {
    build_fallback_tileset(ts);
    const char *paths[] = {
        "assets/tileset.bin",
        "../assets/tileset.bin",
        "../../assets/tileset.bin",
    };
    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); ++i) {
        if (load_tileset_file(ts, paths[i])) {
            return;
        }
    }
}

static bool load_title_overlay_file(TitleOverlay *overlay, const char *path) {
    FILE *f = fopen(path, "rb");
    uint8_t header[16];
    size_t pixel_count = 0;
    if (f == NULL) {
        return false;
    }

    if (fread(header, 1, sizeof(header), f) != sizeof(header)) {
        fclose(f);
        return false;
    }
    if (memcmp(header, "ITOV", 4) != 0) {
        fclose(f);
        return false;
    }

    {
        const uint16_t version = (uint16_t)(header[4] | (header[5] << 8));
        const uint16_t width = (uint16_t)(header[6] | (header[7] << 8));
        const uint16_t height = (uint16_t)(header[8] | (header[9] << 8));
        const int16_t anchor_x = (int16_t)(header[10] | (header[11] << 8));
        const int16_t anchor_y = (int16_t)(header[12] | (header[13] << 8));
        if (version != 1 || width == 0 || height == 0 || width > GAME_LOGICAL_WIDTH || height > GAME_LOGICAL_HEIGHT) {
            fclose(f);
            return false;
        }
        overlay->width = (int)width;
        overlay->height = (int)height;
        overlay->anchor_x = (int)anchor_x;
        overlay->anchor_y = (int)anchor_y;
    }

    pixel_count = (size_t)overlay->width * (size_t)overlay->height;
    if (pixel_count > sizeof(overlay->pixels)) {
        fclose(f);
        return false;
    }

    memset(overlay->pixels, TITLE_OVERLAY_TRANSPARENT_INDEX, sizeof(overlay->pixels));
    if (fread(overlay->pixels, 1, pixel_count, f) != pixel_count) {
        fclose(f);
        return false;
    }

    fclose(f);
    overlay->loaded = true;
    return true;
}

static void load_title_overlay(TitleOverlay *overlay) {
    const char *paths[] = {
        "assets/title_overlay.bin",
        "../assets/title_overlay.bin",
        "../../assets/title_overlay.bin",
    };
    memset(overlay, 0, sizeof(*overlay));
    memset(overlay->pixels, TITLE_OVERLAY_TRANSPARENT_INDEX, sizeof(overlay->pixels));
    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); ++i) {
        if (load_title_overlay_file(overlay, paths[i])) {
            return;
        }
    }
}

static void put_pixel(uint8_t *index_buffer, int x, int y, uint8_t color) {
    if (x < 0 || y < 0 || x >= GAME_LOGICAL_WIDTH || y >= GAME_LOGICAL_HEIGHT) {
        return;
    }
    index_buffer[y * GAME_LOGICAL_WIDTH + x] = color;
}

static void draw_tile(const TileSet *ts, SpriteId sprite, int x, int y, bool transparent, uint8_t *index_buffer) {
    for (int py = 0; py < GAME_TILE_SIZE; ++py) {
        for (int px = 0; px < GAME_TILE_SIZE; ++px) {
            const uint8_t color = ts->tiles[sprite][py * GAME_TILE_SIZE + px];
            if (transparent && color == 0) {
                continue;
            }
            put_pixel(index_buffer, x + px, y + py, color);
        }
    }
}

static void draw_block_bevel(uint8_t *index_buffer, int x, int y) {
    const uint8_t hi = 31;
    const uint8_t lo = 1;
    const uint8_t inner = 9;
    const uint8_t inner_shadow = 3;

    for (int i = 0; i < GAME_TILE_SIZE; ++i) {
        put_pixel(index_buffer, x + i, y, hi);
        put_pixel(index_buffer, x, y + i, hi);
        put_pixel(index_buffer, x + i, y + GAME_TILE_SIZE - 1, lo);
        put_pixel(index_buffer, x + GAME_TILE_SIZE - 1, y + i, lo);
    }
    for (int i = 1; i < GAME_TILE_SIZE - 2; ++i) {
        put_pixel(index_buffer, x + i, y + 1, inner);
        put_pixel(index_buffer, x + 1, y + i, inner);
        put_pixel(index_buffer, x + i, y + GAME_TILE_SIZE - 2, inner_shadow);
        put_pixel(index_buffer, x + GAME_TILE_SIZE - 2, y + i, inner_shadow);
    }
}

static bool is_pushable_block(BlockType block) {
    return block == BLOCK_ICE || block == BLOCK_SPECIAL || block == BLOCK_CRACKED;
}

static void draw_block_contact_shadow(uint8_t *index_buffer, int x, int y, bool moving) {
    const uint8_t shadow = moving ? 1 : 2;
    const uint8_t contact = moving ? 30 : 29;
    for (int i = 2; i < GAME_TILE_SIZE - 1; ++i) {
        put_pixel(index_buffer, x + i, y + GAME_TILE_SIZE - 1, contact);
        if (moving || ((i & 1) == 0)) {
            put_pixel(index_buffer, x + i, y + GAME_TILE_SIZE, shadow);
            put_pixel(index_buffer, x + GAME_TILE_SIZE, y + i, shadow);
        }
    }
    if (moving) {
        for (int i = 4; i < GAME_TILE_SIZE - 4; i += 2) {
            put_pixel(index_buffer, x + i, y + GAME_TILE_SIZE + 1, shadow);
        }
    }
}

static void draw_ice_block_title_sparkles(uint8_t *index_buffer, int px, int py, int phase_seed) {
    const int slow_phase = phase_seed / 3;
    const int wave = slow_phase % 192;
    int sparkle_phase = 0;
    int cx = px + 6;
    int cy = py + 6;

    if (wave < 4) {
        sparkle_phase = (wave < 2) ? (wave + 1) : (4 - wave);
    } else if (wave >= 96 && wave < 100) {
        sparkle_phase = (wave < 98) ? (wave - 95) : (100 - wave);
        cx = px + 10;
        cy = py + 9;
    }

    if (sparkle_phase > 0) {
        draw_title_star_sparkle(index_buffer, cx, cy, sparkle_phase, false);
    }
}

static void draw_stationary_block_state_overlay(uint8_t *index_buffer, BlockType block, int px, int py) {
    if (block == BLOCK_NONE) {
        return;
    }
    put_pixel(index_buffer, px + 7, py + 7, 31);
    put_pixel(index_buffer, px + 8, py + 8, 31);
    put_pixel(index_buffer, px + 6, py + 8, 30);
    put_pixel(index_buffer, px + 9, py + 7, 30);

    if (block == BLOCK_ICE) {
        const int sparkle_seed = g_ui_anim_tick + (px * 5) + (py * 3);
        draw_ice_block_title_sparkles(index_buffer, px, py, sparkle_seed);
    }
}

static SpriteId mine_sprite_for_fuse_ticks(int fuse_ticks, int phase) {
    if (fuse_ticks <= GAME_FIXED_TICK_HZ) {
        return ((phase & 1) == 0) ? SPR_MINE_2 : SPR_MINE_1;
    }
    if (fuse_ticks <= 2 * GAME_FIXED_TICK_HZ) {
        return ((phase & 2) == 0) ? SPR_MINE_1 : SPR_MINE_0;
    }
    return ((phase & 7) == 0) ? SPR_MINE_1 : SPR_MINE_0;
}

static void draw_actor_drop_shadow(uint8_t *index_buffer, int px, int py) {
    const int sy = py + GAME_TILE_SIZE - 2;
    for (int x = 3; x <= 12; ++x) {
        if ((x & 1) == 0) {
            put_pixel(index_buffer, px + x, sy, 1);
        } else {
            put_pixel(index_buffer, px + x, sy, 2);
        }
    }
    for (int x = 5; x <= 10; ++x) {
        put_pixel(index_buffer, px + x, sy + 1, 1);
    }
}

static void draw_enemy_threat_corners(uint8_t *index_buffer, int px, int py, int phase) {
    const uint8_t color = ((phase & 1) == 0) ? 23 : 30;
    put_pixel(index_buffer, px + 1, py + 1, color);
    put_pixel(index_buffer, px + 2, py + 1, color);
    put_pixel(index_buffer, px + 1, py + 2, color);

    put_pixel(index_buffer, px + GAME_TILE_SIZE - 2, py + 1, color);
    put_pixel(index_buffer, px + GAME_TILE_SIZE - 3, py + 1, color);
    put_pixel(index_buffer, px + GAME_TILE_SIZE - 2, py + 2, color);
}

static int dir_dx(Direction d) {
    switch (d) {
        case DIR_LEFT:
            return -1;
        case DIR_RIGHT:
            return 1;
        default:
            return 0;
    }
}

static int dir_dy(Direction d) {
    switch (d) {
        case DIR_UP:
            return -1;
        case DIR_DOWN:
            return 1;
        default:
            return 0;
    }
}

static int tile_to_render_fp(int tile) {
    return tile * RENDER_TILE_FP;
}

static int eased_tile_progress_fp(int progress_fp, bool snap_end) {
    int clamped = clamp_int(progress_fp, 0, RENDER_TILE_FP);
    if (clamped <= 0) {
        return 0;
    }
    if (clamped >= RENDER_TILE_FP) {
        return RENDER_TILE_FP;
    }
    if (snap_end && clamped >= RENDER_SNAP_THRESHOLD_FP) {
        return RENDER_TILE_FP;
    }

    const int64_t t_q16 = ((int64_t)clamped << 16) / RENDER_TILE_FP;
    const int64_t t2_q16 = ((t_q16 * t_q16) + 0x8000) >> 16;
    const int64_t t3_q16 = ((t2_q16 * t_q16) + 0x8000) >> 16;
    int64_t eased_q16 = (3 * t2_q16) - (2 * t3_q16);
    if (eased_q16 < 0) {
        eased_q16 = 0;
    } else if (eased_q16 > 65536) {
        eased_q16 = 65536;
    }

    int eased_fp = (int)((eased_q16 * RENDER_TILE_FP + 0x8000) >> 16);
    if (snap_end && eased_fp >= RENDER_SNAP_THRESHOLD_FP) {
        return RENDER_TILE_FP;
    }
    return clamp_int(eased_fp, 0, RENDER_TILE_FP);
}

static void compute_moving_block_render_pos(const MovingBlock *mb, int *out_px, int *out_py) {
    int render_fp_x = mb->pixel_fp_x;
    int render_fp_y = mb->pixel_fp_y;

    if (mb->active && mb->startup_ticks <= 0 && mb->direction != DIR_NONE) {
        /* Never pre-snap to the next tile; it can visually overshoot at stop boundaries. */
        const int eased_fp = eased_tile_progress_fp(mb->intra_fp, false);
        render_fp_x = tile_to_render_fp(mb->tile_x) + dir_dx(mb->direction) * eased_fp;
        render_fp_y = tile_to_render_fp(mb->tile_y) + dir_dy(mb->direction) * eased_fp;
    }

    *out_px = render_fp_x >> RENDER_FP_SHIFT;
    *out_py = render_fp_y >> RENDER_FP_SHIFT;
}

static bool impact_style_is_stop(int style) {
    return style == IMPACT_FX_STYLE_STOP ||
           style == IMPACT_FX_STYLE_SPECIAL_STOP ||
           style == IMPACT_FX_STYLE_CRACKED_STOP;
}

static int clamp_q16(int value) {
    return clamp_int(value, 0, 65536);
}

static int smoothstep_q16(int t_q16) {
    const int64_t t = clamp_q16(t_q16);
    const int64_t t2 = (t * t + 0x8000) >> 16;
    const int64_t t3 = (t2 * t + 0x8000) >> 16;
    return (int)clamp_q16((int)((3 * t2) - (2 * t3)));
}

static int ease_out_cubic_q16(int t_q16) {
    const int64_t t = clamp_q16(t_q16);
    const int64_t one_minus = 65536 - t;
    const int64_t one_minus2 = (one_minus * one_minus + 0x8000) >> 16;
    const int64_t one_minus3 = (one_minus2 * one_minus + 0x8000) >> 16;
    return (int)clamp_q16((int)(65536 - one_minus3));
}

static int stop_bounce_displacement_fp(const ImpactFx *fx) {
    enum {
        RECOIL_PEAK_Q16 = 22938, /* ~0.35 */
        RECOIL_AMPLITUDE_FP = 2 * RENDER_FP_ONE
    };

    if (!fx->active || !fx->anchors_block || fx->ttl_ticks <= 0 || fx->base_ttl_ticks <= 1) {
        return 0;
    }

    const int age_ticks = fx->base_ttl_ticks - fx->ttl_ticks;
    if (age_ticks < 0 || age_ticks >= fx->base_ttl_ticks) {
        return 0;
    }

    const int t_q16 = clamp_q16((int)(((int64_t)age_ticks << 16) / fx->base_ttl_ticks));
    if (t_q16 < RECOIL_PEAK_Q16) {
        /* Recoil away from the collision in the first part of the stop impact. */
        const int u_q16 = clamp_q16((int)(((int64_t)t_q16 << 16) / RECOIL_PEAK_Q16));
        const int eased_q16 = ease_out_cubic_q16(u_q16);
        const int recoil_fp = (int)(((int64_t)RECOIL_AMPLITUDE_FP * eased_q16 + 0x8000) >> 16);
        return -recoil_fp;
    }

    if (t_q16 < 65536) {
        /* Smoothly settle back to rest without forward overshoot into the blocked tile. */
        const int tail_q16 = 65536 - RECOIL_PEAK_Q16;
        const int u_q16 = clamp_q16((int)(((int64_t)(t_q16 - RECOIL_PEAK_Q16) << 16) / tail_q16));
        const int eased_q16 = smoothstep_q16(u_q16);
        const int recoil_fp = (int)(((int64_t)RECOIL_AMPLITUDE_FP * (65536 - eased_q16) + 0x8000) >> 16);
        return -recoil_fp;
    }

    return 0;
}

static void apply_stop_bounce_to_stationary_block(
    const RenderState *rs,
    int tile_x,
    int tile_y,
    int *out_px,
    int *out_py) {
    int best_abs_fp = 0;
    int best_offset_x_fp = 0;
    int best_offset_y_fp = 0;
    const int base_x_fp = tile_to_render_fp(tile_x);
    const int base_y_fp = tile_to_render_fp(tile_y);

    for (int i = 0; i < GAME_MAX_IMPACT_FX; ++i) {
        const ImpactFx *fx = &rs->impact_fx[i];
        if (!fx->active || !fx->anchors_block) {
            continue;
        }
        if (!impact_style_is_stop(fx->style)) {
            continue;
        }
        if (fx->anchor_tile_x != tile_x || fx->anchor_tile_y != tile_y) {
            continue;
        }

        const int disp_fp = stop_bounce_displacement_fp(fx);
        const int abs_fp = abs(disp_fp);
        if (abs_fp <= best_abs_fp) {
            continue;
        }

        best_abs_fp = abs_fp;
        best_offset_x_fp = dir_dx(fx->direction) * disp_fp;
        best_offset_y_fp = dir_dy(fx->direction) * disp_fp;
    }

    *out_px = (base_x_fp + best_offset_x_fp) >> RENDER_FP_SHIFT;
    *out_py = (base_y_fp + best_offset_y_fp) >> RENDER_FP_SHIFT;
}

static void draw_push_skid_marks(uint8_t *index_buffer, int px, int py, Direction facing, int timer) {
    const uint8_t dark = 2;
    const uint8_t light = 30;
    const int blink = ((timer & 1) == 0) ? 1 : 0;

    switch (facing) {
        case DIR_RIGHT:
            put_pixel(index_buffer, px + 2, py + 12, dark);
            put_pixel(index_buffer, px + 1, py + 11, light);
            if (blink) {
                put_pixel(index_buffer, px + 0, py + 12, dark);
            }
            break;
        case DIR_LEFT:
            put_pixel(index_buffer, px + 13, py + 12, dark);
            put_pixel(index_buffer, px + 14, py + 11, light);
            if (blink) {
                put_pixel(index_buffer, px + 15, py + 12, dark);
            }
            break;
        case DIR_UP:
            put_pixel(index_buffer, px + 7, py + 14, dark);
            put_pixel(index_buffer, px + 8, py + 15, light);
            if (blink) {
                put_pixel(index_buffer, px + 9, py + 14, dark);
            }
            break;
        case DIR_DOWN:
            put_pixel(index_buffer, px + 7, py + 2, dark);
            put_pixel(index_buffer, px + 8, py + 1, light);
            if (blink) {
                put_pixel(index_buffer, px + 9, py + 2, dark);
            }
            break;
        default:
            break;
    }
}

static void draw_special_motion_overlay(uint8_t *index_buffer, int px, int py, int phase) {
    const uint8_t core = 30;
    const uint8_t glow = 8;
    const int cx = px + 8;
    const int cy = py + 8;
    put_pixel(index_buffer, cx, cy, core);
    put_pixel(index_buffer, cx - 1, cy, glow);
    put_pixel(index_buffer, cx + 1, cy, glow);
    put_pixel(index_buffer, cx, cy - 1, glow);
    put_pixel(index_buffer, cx, cy + 1, glow);

    if ((phase & 1) == 0) {
        put_pixel(index_buffer, px + 4, py + 4, core);
        put_pixel(index_buffer, px + 11, py + 11, core);
    } else {
        put_pixel(index_buffer, px + 11, py + 4, core);
        put_pixel(index_buffer, px + 4, py + 11, core);
    }
}

static void draw_cracked_motion_overlay(uint8_t *index_buffer, int px, int py, int phase) {
    const uint8_t crack_a = 21;
    const uint8_t crack_b = 22;
    const int offset = phase & 3;
    for (int i = 2; i < GAME_TILE_SIZE - 2; ++i) {
        const int y = py + ((i + offset) % 6) + 4;
        const uint8_t color = ((i + phase) & 1) == 0 ? crack_a : crack_b;
        put_pixel(index_buffer, px + i, y, color);
    }
    for (int i = 3; i < GAME_TILE_SIZE - 3; i += 2) {
        const int y = py + GAME_TILE_SIZE - 2 - ((i + offset) % 5);
        put_pixel(index_buffer, px + i, y, crack_b);
    }
}

static void draw_moving_block_state_overlay(uint8_t *index_buffer, const MovingBlock *mb, int px, int py) {
    if (mb == NULL) {
        return;
    }
    if (mb->type == BLOCK_ICE) {
        const int sparkle_seed = g_ui_anim_tick + ((mb->pixel_fp_x + mb->pixel_fp_y) >> 7);
        draw_ice_block_title_sparkles(index_buffer, px, py, sparkle_seed);
    }
}

static void draw_enemy_spawn_cue(uint8_t *index_buffer, int px, int py, int spawn_ticks) {
    if (spawn_ticks <= 0 || (spawn_ticks & 1) != 0) {
        return;
    }

    const uint8_t ring = (spawn_ticks & 4) ? 13 : 30;
    const int inset = (spawn_ticks > 12) ? 1 : 0;
    const int x0 = px - inset;
    const int y0 = py - inset;
    const int x1 = px + GAME_TILE_SIZE - 1 + inset;
    const int y1 = py + GAME_TILE_SIZE - 1 + inset;

    for (int i = 0; i < 4; ++i) {
        put_pixel(index_buffer, x0 + i, y0, ring);
        put_pixel(index_buffer, x0, y0 + i, ring);
        put_pixel(index_buffer, x1 - i, y0, ring);
        put_pixel(index_buffer, x1, y0 + i, ring);
        put_pixel(index_buffer, x0 + i, y1, ring);
        put_pixel(index_buffer, x0, y1 - i, ring);
        put_pixel(index_buffer, x1 - i, y1, ring);
        put_pixel(index_buffer, x1, y1 - i, ring);
    }

    if (spawn_ticks < 10) {
        const uint8_t core = (spawn_ticks & 2) ? 30 : 8;
        put_pixel(index_buffer, px + 8, py + 8, core);
        put_pixel(index_buffer, px + 7, py + 8, core);
        put_pixel(index_buffer, px + 8, py + 7, core);
    }
}

static bool impact_overlaps_spawn_cue(const RenderState *rs, int px, int py) {
    for (int i = 0; i < rs->enemy_count; ++i) {
        const Enemy *enemy = &rs->enemies[i];
        if (!enemy->alive || enemy->state != ENEMY_SPAWNING || enemy->spawn_ticks <= 0) {
            continue;
        }

        const int ex = enemy->pixel_fp_x >> 8;
        const int ey = enemy->pixel_fp_y >> 8;
        if (px >= ex - 2 && px <= ex + GAME_TILE_SIZE + 1 &&
            py >= ey - 2 && py <= ey + GAME_TILE_SIZE + 1) {
            return true;
        }
    }
    return false;
}

static SpriteId player_sprite_for_direction(Direction d, bool animate_alt) {
    switch (d) {
        case DIR_UP:
            return animate_alt ? SPR_PLAYER_UP_ALT : SPR_PLAYER_UP;
        case DIR_DOWN:
            return animate_alt ? SPR_PLAYER_DOWN_ALT : SPR_PLAYER_DOWN;
        case DIR_LEFT:
            return animate_alt ? SPR_PLAYER_LEFT_ALT : SPR_PLAYER_LEFT;
        case DIR_RIGHT:
        default:
            return animate_alt ? SPR_PLAYER_RIGHT_ALT : SPR_PLAYER_RIGHT;
    }
}

static SpriteId enemy_sprite_for_state(const Enemy *enemy) {
    const int phase = (enemy->pixel_fp_x + enemy->pixel_fp_y) >> 9;
    const bool animate_alt = ((phase & 1) != 0);
    if (enemy->type == ENEMY_TYPE_WANDERER) {
        return animate_alt ? SPR_ENEMY_B_ALT : SPR_ENEMY_B;
    }
    return animate_alt ? SPR_ENEMY_A_ALT : SPR_ENEMY_A;
}

static SpriteId perk_sprite_for_type(PerkType perk) {
    switch (perk) {
        case PERK_LIFE_BOOST:
            return SPR_HUD_LIFE;
        case PERK_TIME_BOOST:
            return SPR_ITEM_EGG;
        case PERK_SCORE_BOOST:
            return SPR_ITEM_GEM;
        case PERK_ENEMY_SLOW:
            return SPR_ITEM_FISH;
        case PERK_MINES:
            return SPR_EFFECT_SPARK;
        default:
            return SPR_ITEM_MYSTERY;
    }
}

static uint8_t perk_color_for_type(PerkType perk) {
    switch (perk) {
        case PERK_LIFE_BOOST:
            return 23;
        case PERK_TIME_BOOST:
            return 14;
        case PERK_SCORE_BOOST:
            return 8;
        case PERK_ENEMY_SLOW:
            return 11;
        case PERK_MINES:
            return 22;
        default:
            return 30;
    }
}

static const char *perk_label_for_type(PerkType perk) {
    switch (perk) {
        case PERK_LIFE_BOOST:
            return "LIFE";
        case PERK_TIME_BOOST:
            return "TIME";
        case PERK_SCORE_BOOST:
            return "SCORE";
        case PERK_ENEMY_SLOW:
            return "SLOW";
        case PERK_MINES:
            return "MINES";
        default:
            return "MYST";
    }
}

static const char *perk_effect_for_type(PerkType perk) {
    switch (perk) {
        case PERK_LIFE_BOOST:
            return "GAIN 1 LIFE MAX 7";
        case PERK_TIME_BOOST:
            return "ADD TIME LESS EACH LV";
        case PERK_SCORE_BOOST:
            return "SCORE BONUS TAPERS";
        case PERK_ENEMY_SLOW:
            return "SLOW BONUS TAPERS";
        case PERK_MINES:
            return "SPACE/FIRE DROP MINE";
        default:
            return "NO EFFECT";
    }
}

static SpriteId meta_sprite_for_type(MetaUpgradeType upgrade) {
    switch (upgrade) {
        case META_UPGRADE_LIFE_PACK:
            return SPR_HUD_LIFE;
        case META_UPGRADE_TIME_PACK:
            return SPR_ITEM_EGG;
        case META_UPGRADE_SCORE_PACK:
            return SPR_ITEM_GEM;
        case META_UPGRADE_SLOW_PACK:
            return SPR_ITEM_FISH;
        default:
            return SPR_ITEM_MYSTERY;
    }
}

static const char *meta_label_for_type(MetaUpgradeType upgrade) {
    switch (upgrade) {
        case META_UPGRADE_LIFE_PACK:
            return "LIFE";
        case META_UPGRADE_TIME_PACK:
            return "TIME";
        case META_UPGRADE_SCORE_PACK:
            return "SCORE";
        case META_UPGRADE_SLOW_PACK:
            return "SLOW";
        default:
            return "NONE";
    }
}

static const char *meta_effect_for_type(MetaUpgradeType upgrade) {
    switch (upgrade) {
        case META_UPGRADE_LIFE_PACK:
            return "NEW RUN +1 LIFE MAX 7";
        case META_UPGRADE_TIME_PACK:
            return "NEW RUN TIME BONUS";
        case META_UPGRADE_SCORE_PACK:
            return "NEW RUN SCORE BONUS";
        case META_UPGRADE_SLOW_PACK:
            return "NEW RUN ENEMY SLOW";
        default:
            return "NO EFFECT";
    }
}

static uint8_t meta_color_for_type(MetaUpgradeType upgrade) {
    switch (upgrade) {
        case META_UPGRADE_LIFE_PACK:
            return 23;
        case META_UPGRADE_TIME_PACK:
            return 14;
        case META_UPGRADE_SCORE_PACK:
            return 8;
        case META_UPGRADE_SLOW_PACK:
            return 11;
        default:
            return 30;
    }
}

static uint8_t stage_modifier_color(StageModifierType modifier) {
    switch (modifier) {
        case STAGE_MOD_SWARM:
            return 23;
        case STAGE_MOD_SHATTERED_ICE:
            return 27;
        case STAGE_MOD_BLACK_ICE:
            return 29;
        case STAGE_MOD_TURBO_SLIDE:
            return 8;
        case STAGE_MOD_FROST_SIREN:
            return 12;
        default:
            return 3;
    }
}

static const char *stage_modifier_name(StageModifierType modifier) {
    switch (modifier) {
        case STAGE_MOD_SWARM:
            return "SWARM";
        case STAGE_MOD_SHATTERED_ICE:
            return "SHATTERED_ICE";
        case STAGE_MOD_BLACK_ICE:
            return "BLACK_ICE";
        case STAGE_MOD_TURBO_SLIDE:
            return "TURBO_SLIDE";
        case STAGE_MOD_FROST_SIREN:
            return "FROST_SIREN";
        default:
            return "NONE";
    }
}

static const char *phase_name(GamePhase phase) {
    switch (phase) {
        case GAME_PHASE_ROUND_INTRO:
            return "ROUND_INTRO";
        case GAME_PHASE_PLAYING:
            return "PLAYING";
        case GAME_PHASE_PLAYER_DYING:
            return "PLAYER_DYING";
        case GAME_PHASE_ROUND_CLEAR:
            return "ROUND_CLEAR";
        case GAME_PHASE_GAME_OVER:
            return "GAME_OVER";
        case GAME_PHASE_PERK_CHOICE:
            return "PERK_CHOICE";
        case GAME_PHASE_META_UPGRADE:
            return "META_UPGRADE";
        default:
            return "UNKNOWN";
    }
}

static void draw_stage_modifier_glyph(
    uint8_t *index_buffer,
    int x,
    int y,
    StageModifierType modifier,
    uint8_t color) {
    switch (modifier) {
        case STAGE_MOD_SWARM:
            put_pixel(index_buffer, x + 0, y + 1, color);
            put_pixel(index_buffer, x + 1, y + 1, color);
            put_pixel(index_buffer, x + 2, y + 2, color);
            put_pixel(index_buffer, x + 3, y + 2, color);
            put_pixel(index_buffer, x + 1, y + 3, color);
            put_pixel(index_buffer, x + 2, y + 3, color);
            break;
        case STAGE_MOD_SHATTERED_ICE:
            put_pixel(index_buffer, x + 0, y + 0, color);
            put_pixel(index_buffer, x + 3, y + 0, color);
            put_pixel(index_buffer, x + 1, y + 1, color);
            put_pixel(index_buffer, x + 2, y + 1, color);
            put_pixel(index_buffer, x + 2, y + 2, color);
            put_pixel(index_buffer, x + 1, y + 2, color);
            put_pixel(index_buffer, x + 0, y + 3, color);
            put_pixel(index_buffer, x + 3, y + 3, color);
            break;
        case STAGE_MOD_BLACK_ICE:
            for (int gy = 0; gy < 4; ++gy) {
                for (int gx = 0; gx < 4; ++gx) {
                    if (((gx + gy) & 1) == 0) {
                        put_pixel(index_buffer, x + gx, y + gy, color);
                    }
                }
            }
            break;
        case STAGE_MOD_TURBO_SLIDE:
            put_pixel(index_buffer, x + 0, y + 2, color);
            put_pixel(index_buffer, x + 1, y + 1, color);
            put_pixel(index_buffer, x + 1, y + 2, color);
            put_pixel(index_buffer, x + 1, y + 3, color);
            put_pixel(index_buffer, x + 2, y + 0, color);
            put_pixel(index_buffer, x + 2, y + 1, color);
            put_pixel(index_buffer, x + 2, y + 2, color);
            put_pixel(index_buffer, x + 2, y + 3, color);
            put_pixel(index_buffer, x + 3, y + 1, color);
            put_pixel(index_buffer, x + 3, y + 2, color);
            break;
        case STAGE_MOD_FROST_SIREN:
            put_pixel(index_buffer, x + 1, y + 0, color);
            put_pixel(index_buffer, x + 2, y + 0, color);
            put_pixel(index_buffer, x + 1, y + 1, color);
            put_pixel(index_buffer, x + 2, y + 1, color);
            put_pixel(index_buffer, x + 1, y + 2, color);
            put_pixel(index_buffer, x + 2, y + 2, color);
            put_pixel(index_buffer, x + 1, y + 3, color);
            break;
        default:
            break;
    }
}

static void draw_digit_3x5(uint8_t *index_buffer, int x, int y, int digit, uint8_t color) {
    if (digit < 0 || digit > 9) {
        return;
    }
    for (int row = 0; row < 5; ++row) {
        const uint8_t bits = kDigit3x5[digit][row];
        for (int col = 0; col < 3; ++col) {
            if ((bits >> (2 - col)) & 1u) {
                put_pixel(index_buffer, x + col, y + row, color);
            }
        }
    }
}

static void draw_digit_3x5_scaled(
    uint8_t *index_buffer,
    int x,
    int y,
    int digit,
    uint8_t color,
    int scale) {
    if (digit < 0 || digit > 9) {
        return;
    }
    scale = clamp_int(scale, 1, 6);
    for (int row = 0; row < 5; ++row) {
        const uint8_t bits = kDigit3x5[digit][row];
        for (int col = 0; col < 3; ++col) {
            if (((bits >> (2 - col)) & 1u) == 0u) {
                continue;
            }
            const int sx = x + col * scale;
            const int sy = y + row * scale;
            for (int py = 0; py < scale; ++py) {
                for (int px = 0; px < scale; ++px) {
                    put_pixel(index_buffer, sx + px, sy + py, color);
                }
            }
        }
    }
}

static bool letter_3x5(char ch, uint8_t out_rows[5]) {
    switch (ch) {
        case 'A':
            out_rows[0] = 0x7; out_rows[1] = 0x5; out_rows[2] = 0x7; out_rows[3] = 0x5; out_rows[4] = 0x5;
            return true;
        case 'B':
            out_rows[0] = 0x6; out_rows[1] = 0x5; out_rows[2] = 0x6; out_rows[3] = 0x5; out_rows[4] = 0x6;
            return true;
        case 'C':
            out_rows[0] = 0x7; out_rows[1] = 0x4; out_rows[2] = 0x4; out_rows[3] = 0x4; out_rows[4] = 0x7;
            return true;
        case 'D':
            out_rows[0] = 0x6; out_rows[1] = 0x5; out_rows[2] = 0x5; out_rows[3] = 0x5; out_rows[4] = 0x6;
            return true;
        case 'E':
            out_rows[0] = 0x7; out_rows[1] = 0x4; out_rows[2] = 0x6; out_rows[3] = 0x4; out_rows[4] = 0x7;
            return true;
        case 'F':
            out_rows[0] = 0x7; out_rows[1] = 0x4; out_rows[2] = 0x6; out_rows[3] = 0x4; out_rows[4] = 0x4;
            return true;
        case 'G':
            out_rows[0] = 0x7; out_rows[1] = 0x4; out_rows[2] = 0x5; out_rows[3] = 0x5; out_rows[4] = 0x7;
            return true;
        case 'H':
            out_rows[0] = 0x5; out_rows[1] = 0x5; out_rows[2] = 0x7; out_rows[3] = 0x5; out_rows[4] = 0x5;
            return true;
        case 'I':
            out_rows[0] = 0x7; out_rows[1] = 0x2; out_rows[2] = 0x2; out_rows[3] = 0x2; out_rows[4] = 0x7;
            return true;
        case 'J':
            out_rows[0] = 0x1; out_rows[1] = 0x1; out_rows[2] = 0x1; out_rows[3] = 0x5; out_rows[4] = 0x7;
            return true;
        case 'K':
            out_rows[0] = 0x5; out_rows[1] = 0x5; out_rows[2] = 0x6; out_rows[3] = 0x5; out_rows[4] = 0x5;
            return true;
        case 'L':
            out_rows[0] = 0x4; out_rows[1] = 0x4; out_rows[2] = 0x4; out_rows[3] = 0x4; out_rows[4] = 0x7;
            return true;
        case 'M':
            out_rows[0] = 0x5; out_rows[1] = 0x7; out_rows[2] = 0x5; out_rows[3] = 0x5; out_rows[4] = 0x5;
            return true;
        case 'N':
            out_rows[0] = 0x5; out_rows[1] = 0x7; out_rows[2] = 0x7; out_rows[3] = 0x7; out_rows[4] = 0x5;
            return true;
        case 'O':
            out_rows[0] = 0x7; out_rows[1] = 0x5; out_rows[2] = 0x5; out_rows[3] = 0x5; out_rows[4] = 0x7;
            return true;
        case 'P':
            out_rows[0] = 0x6; out_rows[1] = 0x5; out_rows[2] = 0x6; out_rows[3] = 0x4; out_rows[4] = 0x4;
            return true;
        case 'Q':
            out_rows[0] = 0x7; out_rows[1] = 0x5; out_rows[2] = 0x5; out_rows[3] = 0x7; out_rows[4] = 0x1;
            return true;
        case 'R':
            out_rows[0] = 0x6; out_rows[1] = 0x5; out_rows[2] = 0x6; out_rows[3] = 0x5; out_rows[4] = 0x5;
            return true;
        case 'S':
            out_rows[0] = 0x7; out_rows[1] = 0x4; out_rows[2] = 0x7; out_rows[3] = 0x1; out_rows[4] = 0x7;
            return true;
        case 'T':
            out_rows[0] = 0x7; out_rows[1] = 0x2; out_rows[2] = 0x2; out_rows[3] = 0x2; out_rows[4] = 0x2;
            return true;
        case 'U':
            out_rows[0] = 0x5; out_rows[1] = 0x5; out_rows[2] = 0x5; out_rows[3] = 0x5; out_rows[4] = 0x7;
            return true;
        case 'V':
            out_rows[0] = 0x5; out_rows[1] = 0x5; out_rows[2] = 0x5; out_rows[3] = 0x5; out_rows[4] = 0x2;
            return true;
        case 'W':
            out_rows[0] = 0x5; out_rows[1] = 0x5; out_rows[2] = 0x5; out_rows[3] = 0x7; out_rows[4] = 0x5;
            return true;
        case 'X':
            out_rows[0] = 0x5; out_rows[1] = 0x5; out_rows[2] = 0x2; out_rows[3] = 0x5; out_rows[4] = 0x5;
            return true;
        case 'Y':
            out_rows[0] = 0x5; out_rows[1] = 0x5; out_rows[2] = 0x2; out_rows[3] = 0x2; out_rows[4] = 0x2;
            return true;
        case 'Z':
            out_rows[0] = 0x7; out_rows[1] = 0x1; out_rows[2] = 0x2; out_rows[3] = 0x4; out_rows[4] = 0x7;
            return true;
        case '-':
            out_rows[0] = 0x0; out_rows[1] = 0x0; out_rows[2] = 0x7; out_rows[3] = 0x0; out_rows[4] = 0x0;
            return true;
        case '+':
            out_rows[0] = 0x0; out_rows[1] = 0x2; out_rows[2] = 0x7; out_rows[3] = 0x2; out_rows[4] = 0x0;
            return true;
        case ':':
            out_rows[0] = 0x0; out_rows[1] = 0x2; out_rows[2] = 0x0; out_rows[3] = 0x2; out_rows[4] = 0x0;
            return true;
        default:
            return false;
    }
}

static void draw_char_3x5(uint8_t *index_buffer, int x, int y, char ch, uint8_t color) {
    if (ch >= '0' && ch <= '9') {
        draw_digit_3x5(index_buffer, x, y, ch - '0', color);
        return;
    }

    if (ch >= 'a' && ch <= 'z') {
        ch = (char)(ch - 'a' + 'A');
    }
    if (ch == ' ') {
        return;
    }

    uint8_t rows[5] = {0, 0, 0, 0, 0};
    if (!letter_3x5(ch, rows)) {
        return;
    }

    for (int row = 0; row < 5; ++row) {
        const uint8_t bits = rows[row];
        for (int col = 0; col < 3; ++col) {
            if ((bits >> (2 - col)) & 1u) {
                put_pixel(index_buffer, x + col, y + row, color);
            }
        }
    }
}

static void draw_text_3x5(uint8_t *index_buffer, int x, int y, const char *text, uint8_t color) {
    int cursor = x;
    while (*text != '\0') {
        draw_char_3x5(index_buffer, cursor, y, *text, color);
        cursor += 4;
        ++text;
    }
}

static void draw_text_3x5_outlined(
    uint8_t *index_buffer,
    int x,
    int y,
    const char *text,
    uint8_t color,
    uint8_t outline_color) {
    draw_text_3x5(index_buffer, x - 1, y, text, outline_color);
    draw_text_3x5(index_buffer, x + 1, y, text, outline_color);
    draw_text_3x5(index_buffer, x, y - 1, text, outline_color);
    draw_text_3x5(index_buffer, x, y + 1, text, outline_color);
    draw_text_3x5(index_buffer, x, y, text, color);
}

static int text_width_3x5(const char *text) {
    int len = 0;
    while (text[len] != '\0') {
        ++len;
    }
    if (len <= 0) {
        return 0;
    }
    return (len * 4) - 1;
}

static void draw_centered_text_3x5_outlined(
    uint8_t *index_buffer,
    int panel_x0,
    int panel_x1,
    int y,
    const char *text,
    uint8_t color,
    uint8_t outline_color) {
    const int width = text_width_3x5(text);
    const int panel_w = panel_x1 - panel_x0 + 1;
    const int x = panel_x0 + ((panel_w - width) / 2);
    draw_text_3x5_outlined(index_buffer, x, y, text, color, outline_color);
}

static void draw_number(uint8_t *index_buffer, int x, int y, int value, int min_digits, uint8_t color) {
    if (value < 0) {
        value = 0;
    }
    int digits[10];
    int count = 0;
    do {
        digits[count++] = value % 10;
        value /= 10;
    } while (value > 0 && count < 10);

    while (count < min_digits && count < 10) {
        digits[count++] = 0;
    }

    int cursor = x;
    for (int i = count - 1; i >= 0; --i) {
        draw_digit_3x5(index_buffer, cursor, y, digits[i], color);
        cursor += 4;
    }
}

static int number_width_pixels_scaled(int value, int min_digits, int scale) {
    int digits = 1;
    scale = clamp_int(scale, 1, 6);
    if (value < 0) {
        value = 0;
    }
    while (value >= 10) {
        value /= 10;
        ++digits;
    }
    if (digits < min_digits) {
        digits = min_digits;
    }
    return (digits * (4 * scale)) - scale;
}

static void draw_number_scaled(
    uint8_t *index_buffer,
    int x,
    int y,
    int value,
    int min_digits,
    uint8_t color,
    int scale) {
    int digits[10];
    int count = 0;
    int cursor = x;

    scale = clamp_int(scale, 1, 6);
    if (value < 0) {
        value = 0;
    }
    do {
        digits[count++] = value % 10;
        value /= 10;
    } while (value > 0 && count < 10);

    while (count < min_digits && count < 10) {
        digits[count++] = 0;
    }

    for (int i = count - 1; i >= 0; --i) {
        draw_digit_3x5_scaled(index_buffer, cursor, y, digits[i], color, scale);
        cursor += 4 * scale;
    }
}

static void draw_number_scaled_outlined(
    uint8_t *index_buffer,
    int x,
    int y,
    int value,
    int min_digits,
    uint8_t color,
    uint8_t outline_color,
    int scale) {
    draw_number_scaled(index_buffer, x - 1, y, value, min_digits, outline_color, scale);
    draw_number_scaled(index_buffer, x + 1, y, value, min_digits, outline_color, scale);
    draw_number_scaled(index_buffer, x, y - 1, value, min_digits, outline_color, scale);
    draw_number_scaled(index_buffer, x, y + 1, value, min_digits, outline_color, scale);
    draw_number_scaled(index_buffer, x, y, value, min_digits, color, scale);
}

static int count_digits(int value) {
    int n = 1;
    while (value >= 10) {
        value /= 10;
        ++n;
    }
    return n;
}

static int number_width_pixels(int value, int min_digits) {
    int digits = count_digits(value);
    if (digits < min_digits) {
        digits = min_digits;
    }
    return (digits * 4) - 1;
}

static bool bonus_item_should_render(int timer_ticks, bool low_intensity_viz) {
    if (timer_ticks <= 0) {
        return false;
    }
    if (low_intensity_viz) {
        return true;
    }
    if (timer_ticks > 2 * GAME_FIXED_TICK_HZ) {
        return true;
    }
    if (timer_ticks > GAME_FIXED_TICK_HZ) {
        return ((timer_ticks / 8) & 1) == 0;
    }
    return ((timer_ticks / 4) & 1) == 0;
}

static void draw_number_outlined(
    uint8_t *index_buffer,
    int x,
    int y,
    int value,
    int min_digits,
    uint8_t color,
    uint8_t outline_color) {
    draw_number(index_buffer, x - 1, y, value, min_digits, outline_color);
    draw_number(index_buffer, x + 1, y, value, min_digits, outline_color);
    draw_number(index_buffer, x, y - 1, value, min_digits, outline_color);
    draw_number(index_buffer, x, y + 1, value, min_digits, outline_color);
    draw_number(index_buffer, x, y, value, min_digits, color);
}

static void draw_hud_separator(uint8_t *index_buffer, int x, uint8_t dark, uint8_t light) {
    for (int y = 193; y < GAME_LOGICAL_HEIGHT - 1; ++y) {
        put_pixel(index_buffer, x, y, dark);
        put_pixel(index_buffer, x + 1, y, light);
    }
}

static void draw_mini_heart(uint8_t *index_buffer, int x, int y, uint8_t color) {
    put_pixel(index_buffer, x + 1, y + 0, color);
    put_pixel(index_buffer, x + 3, y + 0, color);
    for (int i = 0; i < 5; ++i) {
        put_pixel(index_buffer, x + i, y + 1, color);
    }
    for (int i = 0; i < 5; ++i) {
        put_pixel(index_buffer, x + i, y + 2, color);
    }
    put_pixel(index_buffer, x + 1, y + 3, color);
    put_pixel(index_buffer, x + 2, y + 3, color);
    put_pixel(index_buffer, x + 3, y + 3, color);
    put_pixel(index_buffer, x + 2, y + 4, color);
}

static void draw_hud_text(
    uint8_t *index_buffer,
    int x,
    int y,
    const char *text,
    bool low_intensity_viz,
    uint8_t color,
    uint8_t outline_color) {
    if (low_intensity_viz) {
        draw_text_3x5(index_buffer, x, y, text, color);
        return;
    }
    draw_text_3x5_outlined(index_buffer, x, y, text, color, outline_color);
}

static void draw_hud_number(
    uint8_t *index_buffer,
    int x,
    int y,
    int value,
    int min_digits,
    bool low_intensity_viz,
    uint8_t color,
    uint8_t outline_color) {
    if (low_intensity_viz) {
        draw_number(index_buffer, x, y, value, min_digits, color);
        return;
    }
    draw_number_outlined(index_buffer, x, y, value, min_digits, color, outline_color);
}

static void draw_hud_bomb_meter(
    uint8_t *index_buffer,
    int x,
    int y,
    int mine_stock,
    int mine_capacity,
    bool low_intensity_viz,
    int anim_phase,
    uint8_t full_color,
    uint8_t empty_capacity_color,
    uint8_t locked_color) {
    const int slots = 8;
    const int stock = clamp_int(mine_stock, 0, slots);
    const int capacity = clamp_int(mine_capacity, 0, slots);

    for (int i = 0; i < slots; ++i) {
        const int cell_x = x + (i % 4) * 3;
        const int cell_y = y + (i / 4) * 3;
        uint8_t color = locked_color;
        if (i < capacity) {
            color = empty_capacity_color;
        }
        if (i < stock) {
            color = full_color;
            if (!low_intensity_viz && (((anim_phase / 4) + i) & 1) == 0) {
                color = 30;
            }
        }
        put_pixel(index_buffer, cell_x, cell_y, color);
        put_pixel(index_buffer, cell_x + 1, cell_y, color);
        put_pixel(index_buffer, cell_x, cell_y + 1, color);
        put_pixel(index_buffer, cell_x + 1, cell_y + 1, color);
    }
}

static void render_hud(
    const RenderState *rs,
    const TileSet *ts,
    uint8_t *index_buffer,
    bool low_intensity_viz,
    int score_gain_value,
    int score_gain_ticks) {
    const uint8_t *accent = hud_accents(low_intensity_viz);
    (void)ts;
    enum {
        HUD_SCORE_LABEL_X = 4,
        HUD_SCORE_VALUE_X = 34,
        HUD_ROUND_LABEL_X = 148,
        HUD_ROUND_VALUE_X = 161,
        HUD_TIME_LABEL_X = 180,
        HUD_TIME_VALUE_X = 201,
        HUD_MINE_LABEL_X = 224,
        HUD_MINE_VALUE_X = 244,
        HUD_LIFE_LABEL_X = 271,
        HUD_LIFE_VALUE_X = 287
    };
    const int hud_y0 = 193;
    const int hud_y1 = GAME_LOGICAL_HEIGHT - 1;
    const int timer_seconds = (rs->round_time_ticks > 0) ? (rs->round_time_ticks / GAME_FIXED_TICK_HZ) : 0;
    const int life_count = clamp_int(rs->lives, 0, 99);
    const int mine_capacity = clamp_int(rs->run_mine_capacity, 0, 99);
    const int mine_stock = clamp_int(rs->run_mine_stock, 0, 99);
    const int score_display = (rs->score > 99999999u) ? 99999999 : (int)rs->score;
    const int gain_display = clamp_int(score_gain_value, 0, 9999);
    uint8_t bomb_color = accent[HUD_ACCENT_BOMB];
    char bombs_text[8];

    for (int x = 0; x < GAME_LOGICAL_WIDTH; ++x) {
        index_buffer[192 * GAME_LOGICAL_WIDTH + x] = accent[HUD_ACCENT_RULE_TOP];
    }
    for (int y = hud_y0; y <= hud_y1; ++y) {
        for (int x = 0; x < GAME_LOGICAL_WIDTH; ++x) {
            uint8_t color = accent[HUD_ACCENT_BG_FILL];
            if (!low_intensity_viz && ((x & 31) == 0)) {
                color = accent[HUD_ACCENT_BG_STRIPE];
            }
            index_buffer[y * GAME_LOGICAL_WIDTH + x] = color;
        }
    }
    for (int x = 0; x < GAME_LOGICAL_WIDTH; ++x) {
        put_pixel(index_buffer, x, hud_y1, accent[HUD_ACCENT_RULE_BOTTOM]);
    }

    draw_hud_separator(index_buffer, 142, accent[HUD_ACCENT_SEPARATOR_DARK], accent[HUD_ACCENT_SEPARATOR_LIGHT]);
    draw_hud_separator(index_buffer, 174, accent[HUD_ACCENT_SEPARATOR_DARK], accent[HUD_ACCENT_SEPARATOR_LIGHT]);
    draw_hud_separator(index_buffer, 218, accent[HUD_ACCENT_SEPARATOR_DARK], accent[HUD_ACCENT_SEPARATOR_LIGHT]);
    draw_hud_separator(index_buffer, 266, accent[HUD_ACCENT_SEPARATOR_DARK], accent[HUD_ACCENT_SEPARATOR_LIGHT]);

    snprintf(bombs_text, sizeof(bombs_text), "%d:%d", mine_stock, mine_capacity);
    if (mine_capacity <= 0) {
        bomb_color = accent[HUD_ACCENT_SEPARATOR_LIGHT];
    } else if (mine_stock <= 0) {
        bomb_color = 23;
    } else if (!low_intensity_viz && ((rs->round_time_ticks / 5) & 1) == 0) {
        bomb_color = 30;
    }

    {
        uint8_t timer_color = accent[HUD_ACCENT_TIMER];
        uint8_t timer_outline = accent[HUD_ACCENT_OUTLINE];
        if (rs->timer_danger_active) {
            timer_color = (((rs->round_time_ticks / 4) & 1) == 0) ? accent[HUD_ACCENT_TIMER_DANGER_A] : accent[HUD_ACCENT_TIMER_DANGER_B];
            timer_outline = (((rs->round_time_ticks / 6) & 1) == 0) ? accent[HUD_ACCENT_TIMER_DANGER_OUTLINE_A] : accent[HUD_ACCENT_TIMER_DANGER_OUTLINE_B];
        }
        draw_hud_text(index_buffer, HUD_SCORE_LABEL_X, hud_y0, "SCORE", low_intensity_viz, accent[HUD_ACCENT_LABEL], accent[HUD_ACCENT_OUTLINE]);
        draw_hud_number(
            index_buffer,
            HUD_SCORE_VALUE_X,
            hud_y0,
            score_display,
            6,
            low_intensity_viz,
            accent[HUD_ACCENT_SCORE],
            accent[HUD_ACCENT_OUTLINE]);

        draw_hud_text(index_buffer, HUD_ROUND_LABEL_X, hud_y0, "RD", low_intensity_viz, accent[HUD_ACCENT_LABEL], accent[HUD_ACCENT_OUTLINE]);
        draw_hud_number(
            index_buffer,
            HUD_ROUND_VALUE_X,
            hud_y0,
            rs->round,
            2,
            low_intensity_viz,
            accent[HUD_ACCENT_ROUND],
            accent[HUD_ACCENT_OUTLINE]);

        draw_hud_text(index_buffer, HUD_TIME_LABEL_X, hud_y0, "TIME", low_intensity_viz, accent[HUD_ACCENT_LABEL], accent[HUD_ACCENT_OUTLINE]);
        draw_hud_number(
            index_buffer,
            HUD_TIME_VALUE_X,
            hud_y0,
            timer_seconds,
            2,
            low_intensity_viz,
            timer_color,
            timer_outline);
    }

    draw_hud_text(index_buffer, HUD_MINE_LABEL_X, hud_y0, "BOMB", low_intensity_viz, accent[HUD_ACCENT_LABEL], accent[HUD_ACCENT_OUTLINE]);
    draw_hud_text(index_buffer, HUD_MINE_VALUE_X, hud_y0, bombs_text, low_intensity_viz, bomb_color, accent[HUD_ACCENT_OUTLINE]);
    draw_hud_bomb_meter(
        index_buffer,
        255,
        hud_y0,
        mine_stock,
        mine_capacity,
        low_intensity_viz,
        rs->round_time_ticks,
        accent[HUD_ACCENT_BOMB],
        accent[HUD_ACCENT_SEPARATOR_LIGHT],
        accent[HUD_ACCENT_SEPARATOR_DARK]);

    draw_hud_text(index_buffer, HUD_LIFE_LABEL_X, hud_y0, "LIV", low_intensity_viz, accent[HUD_ACCENT_LABEL], accent[HUD_ACCENT_OUTLINE]);
    draw_hud_number(
        index_buffer,
        HUD_LIFE_VALUE_X,
        hud_y0,
        life_count,
        2,
        low_intensity_viz,
        accent[HUD_ACCENT_LIFE],
        accent[HUD_ACCENT_OUTLINE]);

    if (gain_display > 0 && score_gain_ticks > 0) {
        uint8_t gain_color = accent[HUD_ACCENT_GAIN_PRIMARY];
        if (!low_intensity_viz && (score_gain_ticks & 1) == 0) {
            gain_color = accent[HUD_ACCENT_GAIN_FLASH];
        }
        {
            char gain_text[8];
            snprintf(gain_text, sizeof(gain_text), "+%d", gain_display);
            draw_hud_text(index_buffer, 84, hud_y0, gain_text, low_intensity_viz, gain_color, accent[HUD_ACCENT_OUTLINE]);
        }
    }
}

static void draw_score_popups(const RenderState *rs, uint8_t *index_buffer, bool low_intensity_viz) {
    int draw_x[GAME_MAX_SCORE_POPUPS];
    int draw_y[GAME_MAX_SCORE_POPUPS];
    int draw_w[GAME_MAX_SCORE_POPUPS];
    int draw_value[GAME_MAX_SCORE_POPUPS];
    uint8_t draw_color[GAME_MAX_SCORE_POPUPS];
    int draw_count = 0;

    for (int i = 0; i < GAME_MAX_SCORE_POPUPS; ++i) {
        const ScorePopup *popup = &rs->score_popups[i];
        if (!popup->active) {
            continue;
        }
        if (popup->ttl_ticks <= 0) {
            continue;
        }
        if (!low_intensity_viz && popup->ttl_ticks < 10 && ((popup->ttl_ticks & 1) != 0)) {
            continue;
        }

        int value = popup->value;
        if (value < 0) {
            value = 0;
        }
        const int width = number_width_pixels(value, 0);
        const int drift_x = low_intensity_viz ? 0 : (((i & 1) == 0) ? -2 : 2);
        const int pop_y_offset = low_intensity_viz ? 0 : ((i & 1) + ((popup->ttl_ticks / 6) & 1));
        int px = (popup->pixel_fp_x >> 8) - (width / 2) + drift_x;
        int py = (popup->pixel_fp_y >> 8) - 3 - pop_y_offset;
        px = clamp_int(px, 1, GAME_LOGICAL_WIDTH - width - 2);
        py = clamp_int(py, 1, 187);

        uint8_t color = 30;
        if (popup->value >= 800) {
            color = 8;
        } else if (popup->value >= 400) {
            color = 12;
        }

        draw_x[draw_count] = px;
        draw_y[draw_count] = py;
        draw_w[draw_count] = width;
        draw_value[draw_count] = value;
        draw_color[draw_count] = color;
        ++draw_count;
    }

    for (int i = 0; i < draw_count; ++i) {
        enum {
            POPUP_OUTLINE_PAD = 1,
            POPUP_BOX_HEIGHT = 7,
            POPUP_STACK_STEP = 6,
            POPUP_MAX_STACK_TRIES = 4
        };
        for (int attempt = 0; attempt < POPUP_MAX_STACK_TRIES; ++attempt) {
            bool overlapped = false;
            const int ax0 = draw_x[i] - POPUP_OUTLINE_PAD;
            const int ax1 = draw_x[i] + draw_w[i] + POPUP_OUTLINE_PAD;
            const int ay0 = draw_y[i] - POPUP_OUTLINE_PAD;
            const int ay1 = ay0 + POPUP_BOX_HEIGHT - 1;
            for (int j = 0; j < i; ++j) {
                const int bx0 = draw_x[j] - POPUP_OUTLINE_PAD;
                const int bx1 = draw_x[j] + draw_w[j] + POPUP_OUTLINE_PAD;
                const int by0 = draw_y[j] - POPUP_OUTLINE_PAD;
                const int by1 = by0 + POPUP_BOX_HEIGHT - 1;
                if (ax1 < bx0 || bx1 < ax0 || ay1 < by0 || by1 < ay0) {
                    continue;
                }
                draw_y[i] -= POPUP_STACK_STEP;
                if (draw_y[i] < 1) {
                    draw_y[i] = 1;
                }
                overlapped = true;
                break;
            }
            if (!overlapped) {
                break;
            }
        }
    }

    for (int i = 0; i < draw_count; ++i) {
        draw_number_outlined(index_buffer, draw_x[i], draw_y[i], draw_value[i], 0, draw_color[i], 1);
    }
}

static SpriteId death_sprite_for_fx(const ImpactFx *fx) {
    const int age_ticks = fx->base_ttl_ticks - fx->ttl_ticks;
    const bool late_frame = age_ticks >= (fx->base_ttl_ticks / 2);
    if (fx->style == IMPACT_FX_STYLE_ENEMY_DEATH_B) {
        return late_frame ? SPR_ENEMY_B_DEATH_1 : SPR_ENEMY_B_DEATH_0;
    }
    return late_frame ? SPR_ENEMY_A_DEATH_1 : SPR_ENEMY_A_DEATH_0;
}

static void draw_impact_fx(const RenderState *rs, const TileSet *ts, uint8_t *index_buffer, bool low_intensity_viz) {
    if (low_intensity_viz) {
        return;
    }
    for (int i = 0; i < GAME_MAX_IMPACT_FX; ++i) {
        const ImpactFx *fx = &rs->impact_fx[i];
        if (!fx->active || fx->ttl_ticks <= 0) {
            continue;
        }
        if (fx->ttl_ticks < 4 && ((fx->ttl_ticks & 1) != 0)) {
            continue;
        }

        const int cx = fx->pixel_fp_x >> 8;
        const int cy = fx->pixel_fp_y >> 8;
        const bool enemy_death_style =
            fx->style == IMPACT_FX_STYLE_ENEMY_DEATH_A ||
            fx->style == IMPACT_FX_STYLE_ENEMY_DEATH_B;
        if (enemy_death_style) {
            const int age_ticks = fx->base_ttl_ticks - fx->ttl_ticks;
            int px = cx - (GAME_TILE_SIZE / 2);
            int py = cy - (GAME_TILE_SIZE / 2);
            if (age_ticks > 2) {
                py += (age_ticks - 2) / 4;
            }
            if (fx->ttl_ticks <= 3 && ((fx->ttl_ticks & 1) != 0)) {
                continue;
            }
            draw_tile(ts, death_sprite_for_fx(fx), px, py, true, index_buffer);
            continue;
        }
        const bool overlap_spawn_cue = impact_overlaps_spawn_cue(rs, cx, cy);
        const bool crush_style = (
            fx->style == IMPACT_FX_STYLE_CRUSH ||
            fx->style == IMPACT_FX_STYLE_SPECIAL_CRUSH ||
            fx->style == IMPACT_FX_STYLE_CRACKED_CRUSH);
        const bool mine_blast_style = fx->style == IMPACT_FX_STYLE_MINE_BLAST;
        if (overlap_spawn_cue && !crush_style && !mine_blast_style) {
            continue;
        }

        int spread = 1 + ((12 - fx->ttl_ticks) / 3);
        uint8_t hot = (fx->ttl_ticks > 6) ? 30 : 8;
        uint8_t cool = (fx->ttl_ticks > 5) ? 29 : 24;

        if (fx->style == IMPACT_FX_STYLE_SPECIAL_STOP || fx->style == IMPACT_FX_STYLE_SPECIAL_CRUSH) {
            hot = (fx->ttl_ticks > 6) ? 8 : 30;
            cool = (fx->ttl_ticks > 5) ? 13 : 24;
            spread += 1;
        } else if (fx->style == IMPACT_FX_STYLE_CRACKED_STOP || fx->style == IMPACT_FX_STYLE_CRACKED_CRUSH) {
            hot = (fx->ttl_ticks > 5) ? 21 : 22;
            cool = 2;
        } else if (mine_blast_style) {
            spread = 2 + ((fx->base_ttl_ticks - fx->ttl_ticks) / 2);
            spread = clamp_int(spread, 2, 5);
            hot = ((fx->ttl_ticks & 1) == 0) ? 8 : 30;
            cool = (fx->ttl_ticks > 6) ? 29 : 24;
        }

        if (overlap_spawn_cue) {
            spread = 1;
            hot = 30;
            cool = 3;
        }

        put_pixel(index_buffer, cx, cy, hot);
        put_pixel(index_buffer, cx - spread, cy, hot);
        put_pixel(index_buffer, cx + spread, cy, hot);
        put_pixel(index_buffer, cx, cy - spread, cool);
        put_pixel(index_buffer, cx, cy + spread, cool);

        if (!overlap_spawn_cue && (fx->ttl_ticks & 1) == 0) {
            put_pixel(index_buffer, cx - spread, cy - spread, cool);
            put_pixel(index_buffer, cx + spread, cy + spread, cool);
            put_pixel(index_buffer, cx + spread, cy - spread, cool);
            put_pixel(index_buffer, cx - spread, cy + spread, cool);
        }

        if (mine_blast_style) {
            const int ring = spread + 1;
            put_pixel(index_buffer, cx - ring, cy, hot);
            put_pixel(index_buffer, cx + ring, cy, hot);
            put_pixel(index_buffer, cx, cy - ring, hot);
            put_pixel(index_buffer, cx, cy + ring, hot);
            if ((fx->ttl_ticks & 1) == 0) {
                put_pixel(index_buffer, cx - ring, cy - ring, cool);
                put_pixel(index_buffer, cx + ring, cy + ring, cool);
                put_pixel(index_buffer, cx + ring, cy - ring, cool);
                put_pixel(index_buffer, cx - ring, cy + ring, cool);
            }
        }

        if (crush_style) {
            put_pixel(index_buffer, cx - 1, cy, 30);
            put_pixel(index_buffer, cx + 1, cy, 30);
            put_pixel(index_buffer, cx, cy - 1, 30);
            put_pixel(index_buffer, cx, cy + 1, 30);
        }
    }
}

static void sfx_shutdown(SfxState *sfx) {
    if (sfx->device != 0) {
        SDL_ClearQueuedAudio(sfx->device);
        SDL_CloseAudioDevice(sfx->device);
    }
    if (sfx->owns_audio_subsystem) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
    memset(sfx, 0, sizeof(*sfx));
}

static void sfx_init(SfxState *sfx) {
    SDL_AudioSpec want;
    memset(sfx, 0, sizeof(*sfx));

    if ((SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
            fprintf(stderr, "Audio subsystem unavailable: %s\n", SDL_GetError());
            return;
        }
        sfx->owns_audio_subsystem = true;
    }

    SDL_zero(want);
    want.freq = 22050;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 512;

    sfx->device = SDL_OpenAudioDevice(NULL, 0, &want, &sfx->spec, 0);
    if (sfx->device == 0) {
        fprintf(stderr, "Audio device unavailable: %s\n", SDL_GetError());
        if (sfx->owns_audio_subsystem) {
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
            sfx->owns_audio_subsystem = false;
        }
        return;
    }

    sfx->enabled = true;
    SDL_PauseAudioDevice(sfx->device, 0);
}

static bool sfx_has_queue_headroom(const SfxState *sfx, int duration_ms) {
    uint32_t queued_bytes = 0;
    uint32_t max_queued_bytes = 0;
    uint32_t requested_bytes = 0;
    int requested_samples = 0;

    if (!sfx->enabled || sfx->device == 0) {
        return false;
    }
    if (sfx->spec.freq <= 0 || sfx->spec.channels != 1 || sfx->spec.format != AUDIO_S16SYS) {
        return false;
    }

    queued_bytes = SDL_GetQueuedAudioSize(sfx->device);
    max_queued_bytes = (uint32_t)(sfx->spec.freq * (int)sizeof(int16_t) * 2);
    requested_samples = (sfx->spec.freq * clamp_int(duration_ms, 1, 1000)) / 1000;
    requested_bytes = (uint32_t)(requested_samples * (int)sizeof(int16_t));
    return (queued_bytes + requested_bytes) <= max_queued_bytes;
}

static void sfx_queue_silence(SfxState *sfx, int duration_ms) {
    enum {
        SFX_MAX_SAMPLES = 8192,
    };
    int16_t buffer[SFX_MAX_SAMPLES];
    int samples = 0;

    if (!sfx_has_queue_headroom(sfx, duration_ms)) {
        return;
    }

    samples = (sfx->spec.freq * duration_ms) / 1000;
    samples = clamp_int(samples, 1, SFX_MAX_SAMPLES);
    memset(buffer, 0, (size_t)samples * sizeof(int16_t));
    (void)SDL_QueueAudio(sfx->device, buffer, (uint32_t)(samples * (int)sizeof(int16_t)));
}

static void sfx_queue_tone_sweep(
    SfxState *sfx,
    int hz_start,
    int hz_end,
    int duration_ms,
    int amplitude,
    int duty_percent) {
    enum {
        SFX_MAX_SAMPLES = 8192,
    };
    int16_t buffer[SFX_MAX_SAMPLES];
    int samples = 0;
    int attack_samples = 0;
    int release_samples = 0;
    int hz_fp = 0;
    int hz_step_fp = 0;
    uint32_t phase_fp = 0;
    uint32_t duty_fp = 0;
    const int env_scale = 256;

    if (!sfx->enabled || sfx->device == 0) {
        return;
    }
    if (hz_start <= 0 || duration_ms <= 0) {
        return;
    }
    if (hz_end <= 0) {
        hz_end = hz_start;
    }
    if (sfx->spec.freq <= 0 || sfx->spec.channels != 1 || sfx->spec.format != AUDIO_S16SYS) {
        return;
    }

    if (!sfx_has_queue_headroom(sfx, duration_ms)) {
        return;
    }

    samples = (sfx->spec.freq * duration_ms) / 1000;
    samples = clamp_int(samples, 1, SFX_MAX_SAMPLES);
    amplitude = clamp_int((amplitude * 17) / 10, 200, 30000);
    duty_percent = clamp_int(duty_percent, 10, 90);
    duty_fp = (uint32_t)((duty_percent * 65535) / 100);
    attack_samples = clamp_int(samples / 8, 1, 40);
    release_samples = clamp_int(samples / 3, 1, samples);
    hz_fp = hz_start << 16;
    hz_step_fp = (samples > 1) ? (((hz_end - hz_start) << 16) / (samples - 1)) : 0;

    for (int i = 0; i < samples; ++i) {
        int env = env_scale;
        int hz_now = hz_fp >> 16;
        uint32_t phase_inc_fp = 0;
        int sample = 0;

        if (i < attack_samples) {
            env = (i * env_scale) / attack_samples;
        }
        if (i > samples - release_samples) {
            const int tail = ((samples - i) * env_scale) / release_samples;
            if (tail < env) {
                env = tail;
            }
        }
        hz_now = clamp_int(hz_now, 40, 6000);
        phase_inc_fp = (uint32_t)(((uint64_t)(uint32_t)hz_now * 65536u) / (uint64_t)(uint32_t)sfx->spec.freq);
        if (phase_inc_fp == 0u) {
            phase_inc_fp = 1u;
        }
        phase_fp = (phase_fp + phase_inc_fp) & 0xFFFFu;
        sample = (phase_fp < duty_fp) ? amplitude : -amplitude;
        sample = (sample * env) / env_scale;
        buffer[i] = (int16_t)sample;
        hz_fp += hz_step_fp;
    }

    (void)SDL_QueueAudio(sfx->device, buffer, (uint32_t)(samples * (int)sizeof(int16_t)));
}

static void sfx_queue_noise_burst(SfxState *sfx, int duration_ms, int amplitude) {
    enum {
        SFX_MAX_SAMPLES = 8192,
    };
    int16_t buffer[SFX_MAX_SAMPLES];
    static uint32_t noise_state = 0x9e3779b9u;
    int samples = 0;
    int release_samples = 0;
    int prev = 0;

    if (!sfx->enabled || sfx->device == 0) {
        return;
    }
    if (duration_ms <= 0) {
        return;
    }
    if (sfx->spec.freq <= 0 || sfx->spec.channels != 1 || sfx->spec.format != AUDIO_S16SYS) {
        return;
    }

    if (!sfx_has_queue_headroom(sfx, duration_ms)) {
        return;
    }

    samples = (sfx->spec.freq * duration_ms) / 1000;
    samples = clamp_int(samples, 1, SFX_MAX_SAMPLES);
    amplitude = clamp_int((amplitude * 17) / 10, 600, 30000);
    release_samples = clamp_int(samples, 1, samples);

    for (int i = 0; i < samples; ++i) {
        int sample = 0;
        int env = ((samples - i) * 256) / release_samples;
        noise_state = noise_state * 1664525u + 1013904223u;
        sample = (int)((noise_state >> 16) & 0x7fffu) - 16384;
        sample = (sample * amplitude) / 16384;
        sample = (sample + (prev * 3)) / 4;
        sample = (sample * env) / 256;
        prev = sample;
        buffer[i] = (int16_t)sample;
    }

    (void)SDL_QueueAudio(sfx->device, buffer, (uint32_t)(samples * (int)sizeof(int16_t)));
}

static void sfx_queue_tone(SfxState *sfx, int hz, int duration_ms, int amplitude) {
    sfx_queue_tone_sweep(sfx, hz, hz, duration_ms, amplitude, 50);
}

static void sfx_queue_pattern(SfxState *sfx, const SfxStep *steps, int step_count) {
    if (steps == NULL || step_count <= 0) {
        return;
    }
    for (int i = 0; i < step_count; ++i) {
        const SfxStep *step = &steps[i];
        if (step->hz > 0 && step->duration_ms > 0) {
            sfx_queue_tone(sfx, step->hz, step->duration_ms, step->amplitude);
        } else if (step->duration_ms > 0) {
            sfx_queue_silence(sfx, step->duration_ms);
        }
        if (step->rest_ms > 0) {
            sfx_queue_silence(sfx, step->rest_ms);
        }
    }
}

static void sfx_queue_slot_chime(SfxState *sfx) {
    sfx_queue_tone_sweep(sfx, 620, 760, 30, 13600, 36);
    sfx_queue_silence(sfx, 6);
    sfx_queue_tone_sweep(sfx, 820, 1020, 34, 14600, 34);
    sfx_queue_silence(sfx, 6);
    sfx_queue_tone_sweep(sfx, 1080, 1360, 38, 15600, 31);
    sfx_queue_silence(sfx, 8);
    sfx_queue_tone_sweep(sfx, 1420, 1880, 58, 16800, 28);
}

static void sfx_queue_reward_jingle(SfxState *sfx) {
    sfx_queue_tone_sweep(sfx, 700, 860, 34, 12800, 32);
    sfx_queue_silence(sfx, 6);
    sfx_queue_tone_sweep(sfx, 920, 1120, 36, 13600, 30);
    sfx_queue_silence(sfx, 6);
    sfx_queue_tone_sweep(sfx, 1160, 1460, 40, 14600, 28);
    sfx_queue_silence(sfx, 8);
    sfx_queue_tone_sweep(sfx, 1540, 1980, 74, 16600, 24);
}

static void sfx_queue_stage_clear_jingle(SfxState *sfx) {
    static const SfxStep kPickup[] = {
        {520, 26, 9800, 5},
        {700, 30, 10600, 5},
        {880, 34, 11400, 5},
        {1120, 40, 12200, 9},
    };
    sfx_queue_pattern(sfx, kPickup, (int)(sizeof(kPickup) / sizeof(kPickup[0])));
    sfx_queue_tone_sweep(sfx, 1360, 1640, 44, 12800, 23);
    sfx_queue_silence(sfx, 8);
    sfx_queue_tone_sweep(sfx, 1740, 2140, 86, 13800, 20);
}

static void sfx_queue_level_up_fanfare(SfxState *sfx) {
    static const SfxStep kLevelUp[] = {
        {587, 52, 8600, 5},
        {740, 56, 9000, 5},
        {880, 60, 9500, 6},
        {988, 64, 10000, 6},
        {1175, 74, 10600, 8},
        {1319, 84, 11400, 10},
    };

    sfx_queue_pattern(sfx, kLevelUp, (int)(sizeof(kLevelUp) / sizeof(kLevelUp[0])));
    sfx_queue_tone_sweep(sfx, 1319, 1760, 86, 13200, 18);
    sfx_queue_silence(sfx, 10);
    sfx_queue_tone_sweep(sfx, 1175, 1568, 84, 12600, 20);
    sfx_queue_silence(sfx, 12);
    sfx_queue_tone_sweep(sfx, 1568, 1976, 96, 13800, 22);
}

static void sfx_queue_death_jingle(SfxState *sfx) {
    sfx_queue_noise_burst(sfx, 44, 11000);
    sfx_queue_tone_sweep(sfx, 360, 220, 74, 13800, 48);
    sfx_queue_silence(sfx, 8);
    sfx_queue_tone_sweep(sfx, 220, 110, 118, 14600, 54);
}

static void sfx_queue_game_over_jingle(SfxState *sfx) {
    sfx_queue_noise_burst(sfx, 56, 11400);
    sfx_queue_tone_sweep(sfx, 300, 230, 76, 13400, 50);
    sfx_queue_silence(sfx, 6);
    sfx_queue_tone_sweep(sfx, 230, 160, 92, 13800, 52);
    sfx_queue_silence(sfx, 8);
    sfx_queue_tone_sweep(sfx, 160, 90, 140, 14600, 56);
}

static void sfx_queue_life_loss_fanfare(SfxState *sfx) {
    static const SfxStep kLifeLost[] = {
        {392, 66, 9000, 10},
        {330, 74, 9400, 10},
        {262, 84, 9900, 14},
        {196, 108, 10600, 0},
    };
    sfx_queue_pattern(sfx, kLifeLost, (int)(sizeof(kLifeLost) / sizeof(kLifeLost[0])));
    sfx_queue_silence(sfx, 8);
    sfx_queue_tone_sweep(sfx, 196, 146, 82, 11000, 36);
}

static void sfx_queue_combo_jingle(SfxState *sfx) {
    static const SfxStep kCombo[] = {
        {620, 28, 8200, 4},
        {860, 32, 9000, 5},
        {1180, 42, 9800, 0},
    };
    sfx_queue_pattern(sfx, kCombo, (int)(sizeof(kCombo) / sizeof(kCombo[0])));
    sfx_queue_tone_sweep(sfx, 1320, 1620, 38, 10400, 22);
}

static void sfx_queue_block_push_swish(SfxState *sfx) {
    sfx_queue_tone_sweep(sfx, 1240, 820, 24, 6800, 24);
    sfx_queue_tone_sweep(sfx, 820, 560, 20, 6200, 20);
}

static void sfx_interrupt(SfxState *sfx) {
    if (!sfx->enabled || sfx->device == 0) {
        return;
    }
    SDL_ClearQueuedAudio(sfx->device);
}

static void sfx_queue_title_start_jingle(SfxState *sfx) {
    static const SfxStep kTitleStart[] = {
        {1047, 54, 6200, 4},
        {1319, 56, 6700, 4},
        {1568, 62, 7100, 8},
        {2093, 92, 7800, 12},
        {1760, 52, 6600, 4},
        {2093, 68, 7600, 8},
        {2637, 118, 8400, 0},
    };
    sfx_interrupt(sfx);
    sfx_queue_pattern(sfx, kTitleStart, (int)(sizeof(kTitleStart) / sizeof(kTitleStart[0])));
}

static void sfx_queue_title_confirm_chirp(SfxState *sfx) {
    static const SfxStep kConfirm[] = {
        {1568, 24, 5600, 4},
        {2093, 34, 6400, 0},
    };
    sfx_queue_pattern(sfx, kConfirm, (int)(sizeof(kConfirm) / sizeof(kConfirm[0])));
}

static void sfx_queue_new_high_score_fanfare(SfxState *sfx) {
    static const SfxStep kFanfare[] = {
        {784, 60, 7600, 5},
        {988, 62, 8200, 5},
        {1175, 66, 8600, 6},
        {1568, 92, 9200, 8},
        {1319, 58, 8200, 5},
        {1568, 62, 8800, 6},
        {1976, 132, 9800, 0},
    };
    sfx_interrupt(sfx);
    sfx_queue_pattern(sfx, kFanfare, (int)(sizeof(kFanfare) / sizeof(kFanfare[0])));
}

static void sfx_queue_audition_demo(SfxState *sfx) {
    sfx_interrupt(sfx);
    sfx_queue_tone_sweep(sfx, 420, 560, 34, 10200, 34); /* round-start lift */
    sfx_queue_silence(sfx, 24);
    sfx_queue_block_push_swish(sfx);                    /* block push swish */
    sfx_queue_silence(sfx, 20);
    sfx_queue_noise_burst(sfx, 24, 7000);              /* crush */
    sfx_queue_tone_sweep(sfx, 300, 230, 36, 13200, 56);
    sfx_queue_silence(sfx, 24);
    sfx_queue_noise_burst(sfx, 18, 4600);              /* block impact */
    sfx_queue_tone_sweep(sfx, 240, 170, 26, 8600, 56);
    sfx_queue_silence(sfx, 24);
    sfx_queue_slot_chime(sfx);                         /* item collect */
    sfx_queue_silence(sfx, 36);
    sfx_queue_tone_sweep(sfx, 520, 720, 32, 9800, 30);/* mine placed */
    sfx_queue_silence(sfx, 18);
    sfx_queue_noise_burst(sfx, 56, 14400);             /* mine exploded */
    sfx_queue_tone_sweep(sfx, 280, 150, 96, 15600, 58);
    sfx_queue_silence(sfx, 30);
    sfx_queue_level_up_fanfare(sfx);                  /* level-up fanfare */
}

static void sfx_dispatch_events(SfxState *sfx, uint32_t event_flags) {
    if (event_flags == 0) {
        return;
    }

    if ((event_flags & GAME_EVENT_GAME_OVER) != 0u) {
        sfx_interrupt(sfx);
        sfx_queue_game_over_jingle(sfx);
        return;
    }

    if ((event_flags & GAME_EVENT_LIFE_LOST) != 0u) {
        sfx_interrupt(sfx);
        sfx_queue_life_loss_fanfare(sfx);
        event_flags &= (uint32_t)~(GAME_EVENT_COMBO | GAME_EVENT_CRUSH | GAME_EVENT_ITEM_COLLECT | GAME_EVENT_BLOCK_IMPACT);
    }

    if ((event_flags & GAME_EVENT_PLAYER_DIED) != 0u) {
        sfx_interrupt(sfx);
        sfx_queue_death_jingle(sfx);
        event_flags &= (uint32_t)~(GAME_EVENT_COMBO | GAME_EVENT_CRUSH | GAME_EVENT_ITEM_COLLECT | GAME_EVENT_BLOCK_IMPACT);
    }

    if ((event_flags & GAME_EVENT_ROUND_CLEAR) != 0u) {
        sfx_interrupt(sfx);
        sfx_queue_level_up_fanfare(sfx);
        event_flags &= (uint32_t)~(GAME_EVENT_COMBO | GAME_EVENT_CRUSH | GAME_EVENT_ITEM_COLLECT | GAME_EVENT_BLOCK_IMPACT);
    }

    if ((event_flags & GAME_EVENT_SPECIAL_ALIGNMENT) != 0u) {
        sfx_queue_reward_jingle(sfx);
    }
    if ((event_flags & GAME_EVENT_COMBO) != 0u) {
        sfx_queue_combo_jingle(sfx);
        event_flags &= (uint32_t)~GAME_EVENT_CRUSH;
    }
    if ((event_flags & GAME_EVENT_CRUSH) != 0u) {
        sfx_queue_noise_burst(sfx, 24, 7000);
        sfx_queue_tone_sweep(sfx, 300, 230, 36, 13200, 56);
        sfx_queue_tone_sweep(sfx, 420, 560, 28, 11200, 30);
    }
    if ((event_flags & GAME_EVENT_ITEM_COLLECT) != 0u) {
        sfx_queue_slot_chime(sfx);
    }
    if ((event_flags & GAME_EVENT_TIMER_DANGER_PULSE) != 0u) {
        sfx_queue_tone_sweep(sfx, 980, 900, 34, 9000, 44);
        sfx_queue_silence(sfx, 8);
        sfx_queue_tone_sweep(sfx, 880, 760, 42, 8600, 44);
    }
    if ((event_flags & GAME_EVENT_ROUND_START) != 0u) {
        sfx_queue_tone_sweep(sfx, 420, 560, 34, 10200, 34);
        sfx_queue_silence(sfx, 8);
        sfx_queue_tone_sweep(sfx, 620, 860, 46, 11200, 30);
    }
    if ((event_flags & GAME_EVENT_BLOCK_PUSH) != 0u) {
        sfx_queue_block_push_swish(sfx);
    }
    if ((event_flags & GAME_EVENT_MINE_PLACED) != 0u) {
        sfx_queue_tone_sweep(sfx, 520, 720, 32, 9800, 30);
        event_flags &= (uint32_t)~GAME_EVENT_BLOCK_IMPACT;
    }
    if ((event_flags & GAME_EVENT_MINE_EXPLODED) != 0u) {
        sfx_queue_noise_burst(sfx, 56, 14400);
        sfx_queue_tone_sweep(sfx, 280, 150, 96, 15600, 58);
        sfx_queue_tone_sweep(sfx, 190, 90, 126, 14800, 62);
        event_flags &= (uint32_t)~GAME_EVENT_BLOCK_IMPACT;
    }
    if ((event_flags & GAME_EVENT_BLOCK_IMPACT) != 0u) {
        sfx_queue_noise_burst(sfx, 18, 4600);
        sfx_queue_tone_sweep(sfx, 240, 170, 26, 8600, 56);
    }
    if ((event_flags & GAME_EVENT_META_BANKED) != 0u) {
        sfx_queue_reward_jingle(sfx);
    }
    if ((event_flags & GAME_EVENT_META_SPENT) != 0u) {
        sfx_queue_tone_sweep(sfx, 760, 920, 20, 9800, 30);
        sfx_queue_silence(sfx, 4);
        sfx_queue_tone_sweep(sfx, 940, 1220, 30, 10800, 26);
    }
    if ((event_flags & GAME_EVENT_STAGE_MODIFIER) != 0u) {
        sfx_queue_tone_sweep(sfx, 540, 500, 22, 8600, 40);
        sfx_queue_silence(sfx, 5);
        sfx_queue_tone_sweep(sfx, 660, 620, 24, 9000, 40);
        sfx_queue_silence(sfx, 6);
        sfx_queue_tone_sweep(sfx, 760, 900, 32, 9600, 36);
    }
}

static int get_active_upgrade_values(
    const RenderState *rs,
    int *out_life_perk_level,
    int *out_time_bonus_seconds,
    int *out_score_bonus_percent,
    int *out_slow_bonus_percent,
    int *out_mine_capacity,
    int *out_mine_stock) {
    int active_count = 0;
    const int life_perk_level = rs->perk_levels[PERK_LIFE_BOOST];
    const int time_bonus_seconds = (rs->run_round_time_bonus_ticks > 0) ? (rs->run_round_time_bonus_ticks / GAME_FIXED_TICK_HZ) : 0;
    const int score_bonus_percent =
        (rs->run_score_mult_permille > 1000) ? ((rs->run_score_mult_permille - 1000) / 10) : 0;
    const int slow_bonus_percent =
        (rs->run_enemy_speed_penalty_fp > 0) ? ((rs->run_enemy_speed_penalty_fp * 100 + (HUD_FP_ONE / 2)) / HUD_FP_ONE) : 0;
    const int mine_capacity = rs->run_mine_capacity;
    const int mine_stock = rs->run_mine_stock;

    *out_life_perk_level = life_perk_level;
    *out_time_bonus_seconds = time_bonus_seconds;
    *out_score_bonus_percent = score_bonus_percent;
    *out_slow_bonus_percent = slow_bonus_percent;
    *out_mine_capacity = mine_capacity;
    *out_mine_stock = mine_stock;

    if (life_perk_level > 0) {
        ++active_count;
    }
    if (time_bonus_seconds > 0) {
        ++active_count;
    }
    if (score_bonus_percent > 0) {
        ++active_count;
    }
    if (slow_bonus_percent > 0) {
        ++active_count;
    }
    if (mine_capacity > 0) {
        ++active_count;
    }
    return active_count;
}

static void draw_upgrade_badge(
    uint8_t *index_buffer,
    int x,
    int y,
    char glyph,
    uint8_t fill_color,
    uint8_t glyph_color) {
    for (int py = 0; py < 8; ++py) {
        for (int px = 0; px < 8; ++px) {
            const int sx = x + px;
            const int sy = y + py;
            if (px == 0 || py == 0 || px == 7 || py == 7) {
                put_pixel(index_buffer, sx, sy, 30);
            } else {
                put_pixel(index_buffer, sx, sy, fill_color);
            }
        }
    }
    draw_char_3x5(index_buffer, x + 2, y + 2, glyph, glyph_color);
}

static void draw_active_upgrade_badges(
    const RenderState *rs,
    uint8_t *index_buffer,
    bool low_intensity_viz,
    int base_x,
    int y) {
    int life_perk_level = 0;
    int time_bonus_seconds = 0;
    int score_bonus_percent = 0;
    int slow_bonus_percent = 0;
    int mine_capacity = 0;
    int mine_stock = 0;
    int x = base_x;

    (void)low_intensity_viz;

    (void)get_active_upgrade_values(
        rs,
        &life_perk_level,
        &time_bonus_seconds,
        &score_bonus_percent,
        &slow_bonus_percent,
        &mine_capacity,
        &mine_stock);

    if (life_perk_level > 0) {
        draw_upgrade_badge(index_buffer, x, y, 'L', 23, 30);
        x += 10;
    }
    if (time_bonus_seconds > 0) {
        draw_upgrade_badge(index_buffer, x, y, 'T', 14, 30);
        x += 10;
    }
    if (score_bonus_percent > 0) {
        draw_upgrade_badge(index_buffer, x, y, 'S', 8, 30);
        x += 10;
    }
    if (slow_bonus_percent > 0) {
        draw_upgrade_badge(index_buffer, x, y, 'E', 11, 30);
        x += 10;
    }
    if (mine_capacity > 0) {
        draw_upgrade_badge(index_buffer, x, y, 'M', 22, 30);
    }
}

static void draw_active_run_buffs(
    const RenderState *rs,
    uint8_t *index_buffer,
    bool low_intensity_viz,
    bool show_upgrade_details) {
    const uint8_t *accent = hud_accents(low_intensity_viz);
    int life_perk_level = 0;
    int time_bonus_seconds = 0;
    int score_bonus_percent = 0;
    int slow_bonus_percent = 0;
    int mine_capacity = 0;
    int mine_stock = 0;
    const int active_count = get_active_upgrade_values(
        rs,
        &life_perk_level,
        &time_bonus_seconds,
        &score_bonus_percent,
        &slow_bonus_percent,
        &mine_capacity,
        &mine_stock);
    char buff_line[64];
    int text_width = 0;
    int panel_x0 = 0;
    int panel_x1 = 0;

    if (active_count <= 0) {
        return;
    }

    snprintf(
        buff_line,
        sizeof(buff_line),
        "UP L+%d T+%d S+%d E+%d M%d/%d",
        life_perk_level,
        time_bonus_seconds,
        score_bonus_percent,
        slow_bonus_percent,
        mine_stock,
        mine_capacity);

    text_width = text_width_3x5(buff_line);
    panel_x0 = 4;
    panel_x1 = clamp_int(panel_x0 + text_width + 43, panel_x0 + 8, GAME_LOGICAL_WIDTH - 5);

    if (!low_intensity_viz) {
        for (int y = 3; y <= 20; ++y) {
            for (int x = panel_x0; x <= panel_x1; ++x) {
                if (x == panel_x0 || x == panel_x1 || y == 3 || y == 20) {
                    put_pixel(index_buffer, x, y, accent[HUD_ACCENT_UPGRADE_PANEL_BORDER]);
                } else if (((x + y) & 1) == 0) {
                    put_pixel(index_buffer, x, y, accent[HUD_ACCENT_UPGRADE_PANEL_FILL]);
                }
            }
        }
    }

    draw_text_3x5_outlined(index_buffer, panel_x0 + 3, 4, buff_line, accent[HUD_ACCENT_UPGRADE_TEXT], accent[HUD_ACCENT_UPGRADE_PANEL_FILL]);
    draw_active_upgrade_badges(rs, index_buffer, low_intensity_viz, panel_x0 + 3, 11);
    if (show_upgrade_details) {
        draw_text_3x5_outlined(index_buffer, panel_x0 + 43, 12, "TAB HIDE", accent[HUD_ACCENT_UPGRADE_HINT], accent[HUD_ACCENT_UPGRADE_PANEL_FILL]);
    } else {
        draw_text_3x5_outlined(index_buffer, panel_x0 + 43, 12, "TAB DETAIL", accent[HUD_ACCENT_UPGRADE_HINT], accent[HUD_ACCENT_UPGRADE_PANEL_FILL]);
    }
}

static void draw_detailed_upgrade_panel(const RenderState *rs, uint8_t *index_buffer, bool low_intensity_viz) {
    const uint8_t *accent = hud_accents(low_intensity_viz);
    int life_perk_level = 0;
    int time_bonus_seconds = 0;
    int score_bonus_percent = 0;
    int slow_bonus_percent = 0;
    int mine_capacity = 0;
    int mine_stock = 0;
    const int active_count = get_active_upgrade_values(
        rs,
        &life_perk_level,
        &time_bonus_seconds,
        &score_bonus_percent,
        &slow_bonus_percent,
        &mine_capacity,
        &mine_stock);
    const int panel_x0 = 152;
    const int panel_x1 = 315;
    const int panel_y0 = 4;
    const int panel_y1 = 36;
    char line[64];
    int row = 0;

    if (active_count <= 0) {
        return;
    }

    if (!low_intensity_viz) {
        for (int y = panel_y0; y <= panel_y1; ++y) {
            for (int x = panel_x0; x <= panel_x1; ++x) {
                if (x == panel_x0 || x == panel_x1 || y == panel_y0 || y == panel_y1) {
                    put_pixel(index_buffer, x, y, accent[HUD_ACCENT_UPGRADE_PANEL_BORDER]);
                } else if (((x + y) & 1) == 0) {
                    put_pixel(index_buffer, x, y, accent[HUD_ACCENT_UPGRADE_PANEL_FILL]);
                }
            }
        }
    }

    draw_text_3x5_outlined(
        index_buffer,
        panel_x0 + 3,
        panel_y0 + 3,
        "ACTIVE UPGRADES",
        accent[HUD_ACCENT_DETAIL_TITLE],
        accent[HUD_ACCENT_UPGRADE_PANEL_FILL]);

    if (life_perk_level > 0) {
        snprintf(line, sizeof(line), "LIFE +%d", life_perk_level);
        draw_text_3x5_outlined(index_buffer, panel_x0 + 3, panel_y0 + 10 + row * 6, line, 23, 1);
        ++row;
    }
    if (time_bonus_seconds > 0) {
        snprintf(line, sizeof(line), "TIME +%d", time_bonus_seconds);
        draw_text_3x5_outlined(index_buffer, panel_x0 + 3, panel_y0 + 10 + row * 6, line, 14, 1);
        ++row;
    }
    if (score_bonus_percent > 0) {
        snprintf(line, sizeof(line), "SCORE +%d", score_bonus_percent);
        draw_text_3x5_outlined(index_buffer, panel_x0 + 3, panel_y0 + 10 + row * 6, line, 8, 1);
        ++row;
    }
    if (slow_bonus_percent > 0) {
        snprintf(line, sizeof(line), "ENEMY -%d", slow_bonus_percent);
        draw_text_3x5_outlined(index_buffer, panel_x0 + 3, panel_y0 + 10 + row * 6, line, 11, 1);
        ++row;
    }
    if (mine_capacity > 0) {
        snprintf(line, sizeof(line), "MINES %d/%d", mine_stock, mine_capacity);
        draw_text_3x5_outlined(index_buffer, panel_x0 + 3, panel_y0 + 10 + row * 6, line, 22, 1);
    }
}

static void draw_modal_panel(
    uint8_t *index_buffer,
    int x0,
    int y0,
    int x1,
    int y1,
    uint8_t accent,
    bool low_intensity_viz) {
    int panel_w = 0;
    int panel_h = 0;
    int perimeter = 0;

    panel_w = x1 - x0 + 1;
    panel_h = y1 - y0 + 1;
    perimeter = ((panel_w + panel_h) * 2) - 4;

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            uint8_t color = 1;
            if (x == x0 || x == x1 || y == y0 || y == y1) {
                color = 30;
            } else if (x == x0 + 1 || x == x1 - 1 || y == y0 + 1 || y == y1 - 1) {
                color = accent;
            } else if (!low_intensity_viz && (((x + (y * 3)) & 15) == 0)) {
                color = 2;
            }
            put_pixel(index_buffer, x, y, color);
        }
    }

    if (!low_intensity_viz && perimeter > 0) {
        const int head = (g_ui_anim_tick / 2) % perimeter;
        static const int kTrailOffsets[5] = {0, -1, -2, -3, -4};
        const uint8_t kTrailColors[5] = {31, 30, accent, accent, 2};

        for (int i = 0; i < 5; ++i) {
            int t = head + kTrailOffsets[i];
            int sx = x0;
            int sy = y0;
            while (t < 0) {
                t += perimeter;
            }
            t %= perimeter;

            if (t < panel_w) {
                sx = x0 + t;
                sy = y0;
            } else if (t < panel_w + (panel_h - 1)) {
                sx = x1;
                sy = y0 + (t - panel_w) + 1;
            } else if (t < panel_w + (panel_h - 1) + (panel_w - 1)) {
                sx = x1 - (t - panel_w - (panel_h - 1)) - 1;
                sy = y1;
            } else {
                sx = x0;
                sy = y1 - (t - panel_w - (panel_h - 1) - (panel_w - 1)) - 1;
            }

            put_pixel(index_buffer, sx, sy, kTrailColors[i]);
        }
    }
}

static void draw_choice_card_frame(
    uint8_t *index_buffer,
    int x0,
    int y0,
    int w,
    int h,
    uint8_t accent,
    bool selected,
    bool low_intensity_viz) {
    const int x1 = x0 + w - 1;
    const int y1 = y0 + h - 1;
    const uint8_t outer = selected ? 30 : 3;
    const uint8_t inner = selected ? accent : 2;

    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            uint8_t color = 1;
            if (x == x0 || x == x1 || y == y0 || y == y1) {
                color = outer;
            } else if (x == x0 + 1 || x == x1 - 1 || y == y0 + 1 || y == y1 - 1) {
                color = inner;
            } else if (!low_intensity_viz && (((x + y) & 7) == 0)) {
                color = 2;
            }
            put_pixel(index_buffer, x, y, color);
        }
    }

    if (selected && y0 >= 3) {
        const int cx = x0 + (w / 2);
        put_pixel(index_buffer, cx, y0 - 3, 30);
        put_pixel(index_buffer, cx - 1, y0 - 2, accent);
        put_pixel(index_buffer, cx, y0 - 2, accent);
        put_pixel(index_buffer, cx + 1, y0 - 2, accent);
        put_pixel(index_buffer, cx, y0 - 1, 30);
    }
}

static void draw_visual_arrow_glyph(
    uint8_t *index_buffer,
    int x,
    int y,
    Direction dir,
    uint8_t color) {
    switch (dir) {
        case DIR_LEFT:
            put_pixel(index_buffer, x + 0, y + 2, color);
            put_pixel(index_buffer, x + 1, y + 1, color);
            put_pixel(index_buffer, x + 1, y + 2, color);
            put_pixel(index_buffer, x + 1, y + 3, color);
            put_pixel(index_buffer, x + 2, y + 2, color);
            put_pixel(index_buffer, x + 3, y + 2, color);
            break;
        case DIR_RIGHT:
            put_pixel(index_buffer, x + 0, y + 2, color);
            put_pixel(index_buffer, x + 1, y + 2, color);
            put_pixel(index_buffer, x + 2, y + 1, color);
            put_pixel(index_buffer, x + 2, y + 2, color);
            put_pixel(index_buffer, x + 2, y + 3, color);
            put_pixel(index_buffer, x + 3, y + 2, color);
            break;
        case DIR_UP:
            put_pixel(index_buffer, x + 1, y + 0, color);
            put_pixel(index_buffer, x + 2, y + 0, color);
            put_pixel(index_buffer, x + 0, y + 1, color);
            put_pixel(index_buffer, x + 1, y + 1, color);
            put_pixel(index_buffer, x + 2, y + 1, color);
            put_pixel(index_buffer, x + 3, y + 1, color);
            put_pixel(index_buffer, x + 1, y + 2, color);
            put_pixel(index_buffer, x + 2, y + 2, color);
            put_pixel(index_buffer, x + 1, y + 3, color);
            put_pixel(index_buffer, x + 2, y + 3, color);
            break;
        case DIR_DOWN:
            put_pixel(index_buffer, x + 1, y + 0, color);
            put_pixel(index_buffer, x + 2, y + 0, color);
            put_pixel(index_buffer, x + 1, y + 1, color);
            put_pixel(index_buffer, x + 2, y + 1, color);
            put_pixel(index_buffer, x + 0, y + 2, color);
            put_pixel(index_buffer, x + 1, y + 2, color);
            put_pixel(index_buffer, x + 2, y + 2, color);
            put_pixel(index_buffer, x + 3, y + 2, color);
            put_pixel(index_buffer, x + 1, y + 3, color);
            put_pixel(index_buffer, x + 2, y + 3, color);
            break;
        default:
            break;
    }
}

static void draw_continue_chevrons(
    uint8_t *index_buffer,
    int x,
    int y,
    int tick,
    bool low_intensity_viz,
    uint8_t color_a,
    uint8_t color_b) {
    const bool flash = (((tick / 6) & 1) == 0);
    const uint8_t color = low_intensity_viz ? color_a : (flash ? color_a : color_b);
    draw_visual_arrow_glyph(index_buffer, x + 0, y, DIR_RIGHT, color);
    draw_visual_arrow_glyph(index_buffer, x + 5, y, DIR_RIGHT, color);
    draw_visual_arrow_glyph(index_buffer, x + 10, y, DIR_RIGHT, color);
}

static void draw_round_intro_visual_lane(
    const RenderState *rs,
    const TileSet *ts,
    uint8_t *index_buffer,
    bool low_intensity_viz,
    int anim_tick,
    int lane_x0,
    int lane_y0,
    int lane_w,
    int lane_h) {
    const int lane_y = lane_y0 + ((lane_h - GAME_TILE_SIZE) / 2);
    const int left_x = lane_x0 + 8;
    const int right_x = lane_x0 + lane_w - GAME_TILE_SIZE - 8;
    const int span = right_x - left_x;
    const int block_x = left_x + (span / 3);
    const int enemy_x = left_x + ((span * 2) / 3);
    const int mine_x = right_x;
    const int pulse = anim_tick;
    const uint8_t cue = low_intensity_viz ? 30 : ((((pulse / 5) & 1) == 0) ? 30 : 14);

    draw_tile(ts, player_sprite_for_direction(DIR_RIGHT, false), left_x, lane_y, true, index_buffer);
    draw_visual_arrow_glyph(index_buffer, left_x + 6, lane_y - 4, DIR_UP, cue);
    draw_visual_arrow_glyph(index_buffer, left_x + 6, lane_y + 16, DIR_DOWN, cue);
    draw_visual_arrow_glyph(index_buffer, left_x - 5, lane_y + 6, DIR_LEFT, cue);
    draw_visual_arrow_glyph(index_buffer, left_x + 17, lane_y + 6, DIR_RIGHT, cue);

    draw_tile(ts, SPR_BLOCK_ICE, block_x, lane_y, false, index_buffer);
    if (!low_intensity_viz) {
        const int sparkle_seed = g_ui_anim_tick + (block_x * 5) + (lane_y * 3);
        draw_ice_block_title_sparkles(index_buffer, block_x, lane_y, sparkle_seed);
    }
    draw_visual_arrow_glyph(index_buffer, ((block_x + enemy_x) / 2) - 2, lane_y + 6, DIR_RIGHT, cue);
    draw_tile(ts, (((pulse / 8) & 1) != 0) ? SPR_ENEMY_A_ALT : SPR_ENEMY_A, enemy_x, lane_y, true, index_buffer);
    if (!low_intensity_viz && ((pulse / 4) & 1) == 0) {
        draw_tile(ts, SPR_EFFECT_SPARK, enemy_x + 2, lane_y - 2, true, index_buffer);
    }

    if (rs->run_mine_capacity > 0) {
        const SpriteId mine_sprite = ((pulse / 6) & 1) == 0 ? SPR_MINE_1 : SPR_MINE_2;
        draw_tile(ts, mine_sprite, mine_x, lane_y, true, index_buffer);
        draw_visual_arrow_glyph(index_buffer, mine_x - 7, lane_y + 6, DIR_LEFT, cue);
    } else {
        draw_tile(ts, SPR_MINE_0, mine_x, lane_y, true, index_buffer);
    }
}

static void draw_opening_high_score_table(
    uint8_t *index_buffer,
    bool low_intensity_viz,
    const HighScoreTable *high_scores) {
    const int panel_x0 = 86;
    const int panel_x1 = 233;
    const int panel_y0 = 96;
    const int panel_y1 = 144;
    const int left_x = panel_x0 + 10;
    const int score_right = panel_x1 - 10;

    draw_modal_panel(index_buffer, panel_x0, panel_y0, panel_x1, panel_y1, 16, low_intensity_viz);
    draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y0 + 6, "HI SCORES", 30, 1);

    if (high_scores == NULL) {
        return;
    }

    for (int i = 0; i < (int)(sizeof(high_scores->entries) / sizeof(high_scores->entries[0])); ++i) {
        char rank_name[10];
        const int row_y = panel_y0 + 14 + i * 6;
        const HiScoreEntry *entry = &high_scores->entries[i];
        const int score_w = number_width_pixels((int)entry->score, 5);
        const int score_x = score_right - score_w;
        snprintf(rank_name, sizeof(rank_name), "%d %s", i + 1, entry->initials);
        draw_hud_text(index_buffer, left_x, row_y, rank_name, low_intensity_viz, 29, 1);
        draw_hud_number(index_buffer, score_x, row_y, (int)entry->score, 5, low_intensity_viz, 30, 1);
    }
}

static void draw_high_score_entry_overlay(
    uint8_t *index_buffer,
    bool low_intensity_viz,
    const HighScoreEntryState *entry_state) {
    const int panel_x0 = 54;
    const int panel_x1 = 265;
    const int panel_y0 = 50;
    const int panel_y1 = 149;
    const int slot_w = 28;
    const int slot_h = 24;
    const int slot_spacing = 10;
    const int slots_total_w = slot_w * 3 + slot_spacing * 2;
    const int slots_x0 = panel_x0 + ((panel_x1 - panel_x0 + 1 - slots_total_w) / 2);
    const int slots_y = 91;
    char rank_line[24];
    char score_line[24];
    bool blink_on = true;

    if (entry_state == NULL || !entry_state->active) {
        return;
    }
    blink_on = low_intensity_viz || (((entry_state->blink_ticks / 6) & 1) == 0);

    draw_modal_panel(index_buffer, panel_x0, panel_y0, panel_x1, panel_y1, 16, low_intensity_viz);
    draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y0 + 8, "NEW HI SCORE", 31, 1);
    snprintf(rank_line, sizeof(rank_line), "RANK %d", entry_state->insert_index + 1);
    draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y0 + 16, rank_line, 30, 1);
    snprintf(score_line, sizeof(score_line), "%u", (unsigned int)entry_state->pending_score);
    draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y0 + 24, score_line, 8, 1);
    draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y0 + 34, "ENTER INITIALS", 30, 1);

    for (int i = 0; i < 3; ++i) {
        const bool selected = (i == entry_state->cursor);
        const int slot_x = slots_x0 + i * (slot_w + slot_spacing);
        const int glyph_l = slot_x + 2;
        const int glyph_r = slot_x + slot_w - 3;
        char letter[2];

        draw_choice_card_frame(index_buffer, slot_x, slots_y, slot_w, slot_h, selected ? 29 : 17, selected, low_intensity_viz);
        letter[0] = entry_state->initials[i];
        letter[1] = '\0';
        if (!selected || blink_on) {
            draw_centered_text_3x5_outlined(index_buffer, glyph_l, glyph_r, slots_y + 9, letter, selected ? 31 : 30, 1);
        }
    }

    {
        const int slot_x = slots_x0 + entry_state->cursor * (slot_w + slot_spacing);
        const int center_x = slot_x + (slot_w / 2) - 1;
        draw_visual_arrow_glyph(index_buffer, center_x - 1, slots_y - 7, DIR_UP, 30);
        draw_visual_arrow_glyph(index_buffer, center_x - 1, slots_y + slot_h + 2, DIR_DOWN, 30);
    }
    draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y1 - 18, "ARROWS EDIT", 30, 1);
    draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y1 - 10, "START SAVE", 31, 1);
}

static void draw_title_star_sparkle(
    uint8_t *index_buffer,
    int cx,
    int cy,
    int phase,
    bool low_intensity_viz) {
    const uint8_t core = low_intensity_viz ? 30 : 31;
    const uint8_t arm = low_intensity_viz ? 29 : 30;
    const uint8_t diag = low_intensity_viz ? 17 : 29;

    put_pixel(index_buffer, cx, cy, core);
    if (phase >= 1) {
        put_pixel(index_buffer, cx - 1, cy, arm);
        put_pixel(index_buffer, cx + 1, cy, arm);
        put_pixel(index_buffer, cx, cy - 1, arm);
        put_pixel(index_buffer, cx, cy + 1, arm);
    }
    if (phase >= 2) {
        put_pixel(index_buffer, cx - 2, cy, diag);
        put_pixel(index_buffer, cx + 2, cy, diag);
        put_pixel(index_buffer, cx, cy - 2, diag);
        put_pixel(index_buffer, cx, cy + 2, diag);
        put_pixel(index_buffer, cx - 1, cy - 1, arm);
        put_pixel(index_buffer, cx + 1, cy - 1, arm);
        put_pixel(index_buffer, cx - 1, cy + 1, arm);
        put_pixel(index_buffer, cx + 1, cy + 1, arm);
    }
    if (phase >= 3) {
        put_pixel(index_buffer, cx - 3, cy, diag);
        put_pixel(index_buffer, cx + 3, cy, diag);
        put_pixel(index_buffer, cx, cy - 3, diag);
        put_pixel(index_buffer, cx, cy + 3, diag);
        put_pixel(index_buffer, cx - 2, cy - 2, diag);
        put_pixel(index_buffer, cx + 2, cy - 2, diag);
        put_pixel(index_buffer, cx - 2, cy + 2, diag);
        put_pixel(index_buffer, cx + 2, cy + 2, diag);
    }
}

static void draw_title_overlay(
    const TitleOverlay *overlay,
    uint8_t *index_buffer,
    int center_x,
    int top_y,
    int tick,
    bool low_intensity_viz) {
    if (overlay == NULL || !overlay->loaded || overlay->width <= 0 || overlay->height <= 0) {
        return;
    }

    const int bob = low_intensity_viz ? 0 : ((((tick / 6) & 1) == 0) ? 0 : 1);
    const int draw_x = center_x - (overlay->width / 2) + overlay->anchor_x;
    const int draw_y = top_y + overlay->anchor_y + bob;

    for (int y = 0; y < overlay->height; ++y) {
        const int sy = draw_y + y;
        if (sy < 0 || sy >= GAME_LOGICAL_HEIGHT) {
            continue;
        }
        for (int x = 0; x < overlay->width; ++x) {
            uint8_t color = overlay->pixels[y * overlay->width + x];
            if (color == TITLE_OVERLAY_TRANSPARENT_INDEX) {
                continue;
            }
            color &= 31u;
            if (!low_intensity_viz && color == 30 && (((tick / 10) + x + y) & 31) == 0) {
                color = 31;
            }
            put_pixel(index_buffer, draw_x + x, sy, color);
        }
    }

    if (!low_intensity_viz) {
        static const int kSparkleAnchors[5][2] = {
            {88, 16},  /* between ICE letters */
            {170, 14}, /* right of E */
            {36, 30},  /* left side PANIC */
            {124, 31}, /* center PANIC */
            {198, 32}, /* right edge PANIC */
        };
        const int t = tick / 2;
        for (int i = 0; i < 5; ++i) {
            const int sx = draw_x + ((kSparkleAnchors[i][0] * overlay->width) / 220);
            const int sy = draw_y + ((kSparkleAnchors[i][1] * overlay->height) / 48);
            const int wave = (t + i * 5) & 15;
            int phase = 0;
            if (wave < 3) {
                phase = wave + 1;
            } else if (wave == 3) {
                phase = 2;
            } else if (wave == 4) {
                phase = 1;
            } else {
                continue;
            }
            draw_title_star_sparkle(index_buffer, sx, sy, phase, low_intensity_viz);
        }
    }
}

static void render_overlay(
    const RenderState *rs,
    const TileSet *ts,
    const TitleOverlay *title_overlay,
    const HighScoreTable *high_scores,
    uint8_t *index_buffer,
    bool low_intensity_viz,
    bool show_upgrade_details) {
    const int ui_anim_tick = g_ui_anim_tick;

    if (title_overlay != NULL &&
        title_overlay->loaded &&
        rs->phase == GAME_PHASE_ROUND_INTRO &&
        rs->start_title_pending) {
        const int tick = ui_anim_tick;
        const uint8_t studio_color = low_intensity_viz ? 30 : ((((tick / 14) & 1) == 0) ? 29 : 30);
        draw_title_overlay(
            title_overlay,
            index_buffer,
            GAME_LOGICAL_WIDTH / 2,
            10,
            tick,
            low_intensity_viz);
        draw_centered_text_3x5_outlined(
            index_buffer,
            0,
            GAME_LOGICAL_WIDTH - 1,
            84,
            "RETROFOUNDRY",
            studio_color,
            1);
        draw_opening_high_score_table(index_buffer, low_intensity_viz, high_scores);
    }

    if (!low_intensity_viz &&
        rs->stage_modifier != STAGE_MOD_NONE &&
        rs->stage_modifier_flash_ticks > 0 &&
        ((rs->stage_modifier_flash_ticks / 3) & 1) == 0) {
        const uint8_t color = stage_modifier_color(rs->stage_modifier);
        for (int x = 2; x < GAME_LOGICAL_WIDTH - 2; ++x) {
            put_pixel(index_buffer, x, 2, color);
            put_pixel(index_buffer, x, 189, color);
        }
    }

    if (!low_intensity_viz && rs->special_alignment_flash_ticks > 0 && ((rs->special_alignment_flash_ticks / 3) & 1) == 0) {
        for (int x = 0; x < GAME_LOGICAL_WIDTH; ++x) {
            index_buffer[x] = 8;
            index_buffer[(GAME_LOGICAL_HEIGHT - 1) * GAME_LOGICAL_WIDTH + x] = 8;
        }
        for (int y = 0; y < GAME_LOGICAL_HEIGHT; ++y) {
            index_buffer[y * GAME_LOGICAL_WIDTH] = 8;
            index_buffer[y * GAME_LOGICAL_WIDTH + (GAME_LOGICAL_WIDTH - 1)] = 8;
        }
    }

    if (!low_intensity_viz && rs->phase == GAME_PHASE_PLAYING && rs->timer_danger_pulse_ticks > 0) {
        for (int x = 0; x < GAME_LOGICAL_WIDTH; ++x) {
            index_buffer[x] = 23;
            index_buffer[(192 - 1) * GAME_LOGICAL_WIDTH + x] = 23;
        }
        for (int y = 0; y < 192; ++y) {
            index_buffer[y * GAME_LOGICAL_WIDTH] = 23;
            index_buffer[y * GAME_LOGICAL_WIDTH + (GAME_LOGICAL_WIDTH - 1)] = 23;
        }
    }

    if (rs->phase == GAME_PHASE_PLAYING) {
        draw_active_run_buffs(rs, index_buffer, low_intensity_viz, show_upgrade_details);
        if (show_upgrade_details) {
            draw_detailed_upgrade_panel(rs, index_buffer, low_intensity_viz);
        }
    }

    if (rs->phase == GAME_PHASE_PERK_CHOICE && rs->perk_choice_count > 0) {
        const int panel_x0 = 48;
        const int panel_x1 = 271;
        const int panel_y0 = 58;
        const int panel_y1 = 143;
        const int card_w = 58;
        const int card_h = 44;
        const int spacing = 8;
        const int choice_count = clamp_int(rs->perk_choice_count, 1, GAME_PERK_CHOICE_COUNT);
        const int total_w = (choice_count * card_w) + ((choice_count - 1) * spacing);
        const int base_x = panel_x0 + ((panel_x1 - panel_x0 + 1 - total_w) / 2);
        const int card_y = 75;

        draw_modal_panel(index_buffer, panel_x0, panel_y0, panel_x1, panel_y1, 18, low_intensity_viz);
        draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, 64, "PICK PERK", 31, 1);

        for (int i = 0; i < choice_count; ++i) {
            const int px = base_x + i * (card_w + spacing);
            const int py = card_y;
            const PerkType perk = rs->perk_choices[i];
            const bool selected = (i == rs->perk_choice_selected);
            const uint8_t accent = perk_color_for_type(perk);
            char level_text[16];

            draw_choice_card_frame(index_buffer, px, py, card_w, card_h, accent, selected, low_intensity_viz);
            draw_tile(ts, perk_sprite_for_type(perk), px + ((card_w - GAME_TILE_SIZE) / 2), py + 5, true, index_buffer);
            draw_centered_text_3x5_outlined(index_buffer, px + 2, px + card_w - 3, py + 24, perk_label_for_type(perk), accent, 1);
            snprintf(level_text, sizeof(level_text), "LV %d", rs->perk_levels[perk] + 1);
            draw_centered_text_3x5_outlined(index_buffer, px + 2, px + card_w - 3, py + 31, level_text, 30, 1);
        }
        {
            const int sel_idx = (rs->perk_choice_selected < 0) ? 0 : rs->perk_choice_selected;
            const PerkType selected = rs->perk_choices[(sel_idx >= rs->perk_choice_count) ? 0 : sel_idx];
            const uint8_t accent = perk_color_for_type(selected);
            char info[64];
            snprintf(info, sizeof(info), "%s LV %d", perk_label_for_type(selected), rs->perk_levels[selected] + 1);
            draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, 124, info, accent, 1);
            draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, 130, perk_effect_for_type(selected), 30, 1);
            draw_visual_arrow_glyph(index_buffer, panel_x0 + 8, 135, DIR_LEFT, 30);
            draw_visual_arrow_glyph(index_buffer, panel_x1 - 12, 135, DIR_RIGHT, 30);
            draw_continue_chevrons(index_buffer, panel_x0 + ((panel_x1 - panel_x0) / 2) - 7, 135, ui_anim_tick, low_intensity_viz, accent, 30);
        }
        return;
    }

    if (rs->phase == GAME_PHASE_META_UPGRADE && rs->meta_choice_count > 0) {
        const int panel_x0 = 36;
        const int panel_x1 = 283;
        const int panel_y0 = 58;
        const int panel_y1 = 143;
        const int card_w = 48;
        const int card_h = 44;
        const int spacing = 8;
        const int choice_count = clamp_int(rs->meta_choice_count, 1, GAME_META_CHOICE_COUNT);
        const int total_w = (choice_count * card_w) + ((choice_count - 1) * spacing);
        const int base_x = panel_x0 + ((panel_x1 - panel_x0 + 1 - total_w) / 2);
        const int card_y = 75;

        draw_modal_panel(index_buffer, panel_x0, panel_y0, panel_x1, panel_y1, 16, low_intensity_viz);
        {
            char title[32];
            char tier_status[32];
            const int tier = clamp_int(rs->meta_unlock_tier, 1, 3);
            snprintf(title, sizeof(title), "META TIER %d", tier);
            if (tier >= 3) {
                snprintf(tier_status, sizeof(tier_status), "POOL MAXED");
            } else {
                const uint32_t target = (tier == 1) ? GAME_META_UNLOCK_TIER2_PROGRESS : GAME_META_UNLOCK_TIER3_PROGRESS;
                uint32_t progress = rs->meta_progress_points;
                if (progress > target) {
                    progress = target;
                }
                snprintf(tier_status, sizeof(tier_status), "NEXT %u/%u", (unsigned int)progress, (unsigned int)target);
            }
            draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, 64, title, 31, 1);
            draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, 70, tier_status, 30, 1);
        }

        for (int i = 0; i < choice_count; ++i) {
            const int px = base_x + i * (card_w + spacing);
            const int py = card_y;
            const MetaUpgradeType upgrade = rs->meta_choices[i];
            const int cost = rs->meta_choice_costs[i];
            const bool selected = (i == rs->meta_choice_selected);
            const bool affordable = rs->meta_shards >= (uint32_t)((cost < 0) ? 0 : cost);
            const uint8_t accent = affordable ? meta_color_for_type(upgrade) : 23;
            char cost_text[16];

            draw_choice_card_frame(index_buffer, px, py, card_w, card_h, accent, selected, low_intensity_viz);
            draw_tile(ts, meta_sprite_for_type(upgrade), px + ((card_w - GAME_TILE_SIZE) / 2), py + 5, true, index_buffer);
            draw_centered_text_3x5_outlined(index_buffer, px + 2, px + card_w - 3, py + 24, meta_label_for_type(upgrade), accent, 1);
            snprintf(cost_text, sizeof(cost_text), "C %d", cost);
            draw_centered_text_3x5_outlined(index_buffer, px + 2, px + card_w - 3, py + 31, cost_text, affordable ? 30 : 23, 1);
        }
        {
            const int sel_idx = (rs->meta_choice_selected < 0) ? 0 : rs->meta_choice_selected;
            const int bounded_sel = (sel_idx >= rs->meta_choice_count) ? 0 : sel_idx;
            const MetaUpgradeType selected = rs->meta_choices[bounded_sel];
            const int cost = rs->meta_choice_costs[bounded_sel];
            const bool affordable = rs->meta_shards >= (uint32_t)((cost < 0) ? 0 : cost);
            char info[64];
            snprintf(info, sizeof(info), "%s COST %d", meta_label_for_type(selected), cost);
            draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, 124, info, affordable ? 30 : 23, 1);
            draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, 130, meta_effect_for_type(selected), affordable ? 30 : 23, 1);
            draw_visual_arrow_glyph(index_buffer, panel_x0 + 8, 135, DIR_LEFT, 30);
            draw_visual_arrow_glyph(index_buffer, panel_x1 - 12, 135, DIR_RIGHT, 30);
            draw_continue_chevrons(
                index_buffer,
                panel_x0 + ((panel_x1 - panel_x0) / 2) - 7,
                135,
                ui_anim_tick,
                low_intensity_viz,
                affordable ? 30 : 23,
                affordable ? 16 : 2);
        }
        return;
    }

    if (rs->phase == GAME_PHASE_ROUND_INTRO) {
        const bool opening_round_prompt = rs->start_title_pending;
        if (opening_round_prompt) {
            const int panel_x0 = 70;
            const int panel_x1 = 249;
            const int panel_y0 = 148;
            const int panel_y1 = 178;
            const int panel_w = panel_x1 - panel_x0 + 1;
            char round_line[24];
            snprintf(round_line, sizeof(round_line), "ROUND %02d", rs->round);
            draw_modal_panel(index_buffer, panel_x0, panel_y0, panel_x1, panel_y1, 14, low_intensity_viz);
            draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y0 + 7, round_line, 30, 1);
            draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y0 + 15, "PRESS START", 31, 1);
            draw_continue_chevrons(
                index_buffer,
                panel_x0 + (panel_w / 2) - 7,
                panel_y1 - 11,
                ui_anim_tick,
                low_intensity_viz,
                30,
                14);
            return;
        }

        const int panel_x0 = 48;
        const int panel_x1 = 271;
        const int panel_y0 = 58;
        const int panel_y1 = 143;
        const int panel_w = panel_x1 - panel_x0 + 1;
        const int round_width = number_width_pixels(rs->round, 2);
        const int round_x = panel_x0 + ((panel_w - round_width) / 2);
        const int lane_card_x = panel_x0 + 14;
        const int lane_card_y = panel_y0 + 24;
        const int lane_card_w = panel_w - 28;
        const int lane_card_h = 44;
        const int lane_inner_x = lane_card_x + 6;
        const int lane_inner_y = lane_card_y + 4;
        const int lane_inner_w = lane_card_w - 12;
        const int lane_inner_h = lane_card_h - 8;
        const int chevron_x = panel_x0 + (panel_w / 2) - 7;

        draw_modal_panel(index_buffer, panel_x0, panel_y0, panel_x1, panel_y1, 14, low_intensity_viz);
        draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y0 + 8, "ROUND", 31, 1);
        draw_number_outlined(index_buffer, round_x, panel_y0 + 16, rs->round, 2, 31, 1);
        draw_choice_card_frame(index_buffer, lane_card_x, lane_card_y, lane_card_w, lane_card_h, 14, true, true);
        draw_round_intro_visual_lane(
            rs,
            ts,
            index_buffer,
            low_intensity_viz,
            ui_anim_tick,
            lane_inner_x,
            lane_inner_y,
            lane_inner_w,
            lane_inner_h);
        draw_continue_chevrons(index_buffer, chevron_x, panel_y1 - 11, ui_anim_tick, low_intensity_viz, 30, 14);
        return;
    }

    uint8_t color = 0;
    int panel_x0 = 80;
    int panel_x1 = 239;
    int panel_y0 = 82;
    int panel_y1 = 109;
    switch (rs->phase) {
        case GAME_PHASE_ROUND_INTRO:
            color = 13;
            panel_y1 = 126;
            break;
        case GAME_PHASE_PLAYER_DYING:
            color = 23;
            panel_y1 = 112;
            break;
        case GAME_PHASE_ROUND_CLEAR:
            color = 11;
            panel_y1 = 126;
            break;
        case GAME_PHASE_GAME_OVER:
            color = 21;
            panel_x0 = 56;
            panel_x1 = 263;
            panel_y0 = 70;
            panel_y1 = 145;
            break;
        default:
            return;
    }

    draw_modal_panel(index_buffer, panel_x0, panel_y0, panel_x1, panel_y1, color, low_intensity_viz);

    if (rs->phase == GAME_PHASE_PLAYER_DYING) {
        draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, 92, "DIED", 31, 1);
        draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, 100, "RESPAWNING", 30, 1);
        return;
    }
    if (rs->phase == GAME_PHASE_ROUND_CLEAR) {
        char bonus_line[32];
        draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, 86, "ROUND CLEAR", 31, 1);
        snprintf(
            bonus_line,
            sizeof(bonus_line),
            "TIME BONUS +%d",
            clamp_int(rs->round_clear_bonus_score, 0, 99999999));
        draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, 98, bonus_line, 30, 1);
        draw_continue_chevrons(index_buffer, panel_x0 + ((panel_x1 - panel_x0) / 2) - 7, 114, ui_anim_tick, low_intensity_viz, 31, 11);
        return;
    }
    if (rs->phase == GAME_PHASE_GAME_OVER) {
        const int panel_w = panel_x1 - panel_x0 + 1;
        const int score_scale = 3;
        const int score = clamp_int((int)rs->score, 0, 99999999);
        const int score_w = number_width_pixels_scaled(score, 5, score_scale);
        const int score_x = panel_x0 + ((panel_w - score_w) / 2);
        const uint8_t score_color = low_intensity_viz ? 31 : ((((ui_anim_tick / 4) & 1) == 0) ? 31 : 30);

        draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y0 + 7, "GAME OVER", 31, 1);
        draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y0 + 16, "FINAL SCORE", 30, 1);
        draw_number_scaled_outlined(index_buffer, score_x, panel_y0 + 25, score, 5, score_color, 1, score_scale);
        if (rs->run_shards > 0) {
            char shard_line[32];
            snprintf(shard_line, sizeof(shard_line), "BANK %d SHARDS", rs->run_shards);
            draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y1 - 16, shard_line, 8, 1);
        }
        if (rs->phase_timer_ticks > 0) {
            draw_centered_text_3x5_outlined(index_buffer, panel_x0, panel_x1, panel_y1 - 9, "PLEASE WAIT", 30, 1);
        }
        return;
    }

}

static void render_scene(
    const RenderState *rs,
    const TileSet *ts,
    const TitleOverlay *title_overlay,
    const HighScoreTable *high_scores,
    uint8_t *index_buffer,
    bool low_intensity_viz,
    bool show_upgrade_details,
    int score_gain_value,
    int score_gain_ticks) {
    memset(index_buffer, 0, GAME_LOGICAL_WIDTH * GAME_LOGICAL_HEIGHT);

    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            const Terrain terrain = rs->terrain[y][x];
            draw_tile(ts, kTerrainSprites[terrain], x * GAME_TILE_SIZE, y * GAME_TILE_SIZE, false, index_buffer);
        }
    }

    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            const BlockType block = rs->blocks[y][x];
            if (block == BLOCK_NONE) {
                continue;
            }
            int px = x * GAME_TILE_SIZE;
            int py = y * GAME_TILE_SIZE;
            if (is_pushable_block(block)) {
                apply_stop_bounce_to_stationary_block(rs, x, y, &px, &py);
            }
            draw_tile(ts, kBlockSprites[block], px, py, false, index_buffer);
            if (is_pushable_block(block)) {
                draw_block_bevel(index_buffer, px, py);
                draw_block_contact_shadow(index_buffer, px, py, false);
            }
            if (!low_intensity_viz) {
                draw_stationary_block_state_overlay(index_buffer, block, px, py);
            }
        }
    }

    const bool show_items = bonus_item_should_render(rs->bonus_item_timer_ticks, low_intensity_viz);
    if (show_items) {
        for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
            for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
                const ItemType item = rs->items[y][x];
                if (item == ITEM_NONE) {
                    continue;
                }
                draw_tile(ts, kItemSprites[item], x * GAME_TILE_SIZE, y * GAME_TILE_SIZE, true, index_buffer);
            }
        }
    }

    for (int y = 0; y < GAME_GRID_HEIGHT; ++y) {
        for (int x = 0; x < GAME_GRID_WIDTH; ++x) {
            if (!rs->mines[y][x]) {
                continue;
            }
            {
                const int phase = rs->round_time_ticks + (rs->timer_danger_pulse_ticks * 3) + x * 3 + y * 5;
                const int fuse_ticks = rs->mine_fuse_ticks[y][x];
                const SpriteId mine_sprite = mine_sprite_for_fuse_ticks(fuse_ticks, phase);
                draw_tile(ts, mine_sprite, x * GAME_TILE_SIZE, y * GAME_TILE_SIZE, true, index_buffer);
            }
        }
    }

    for (int i = 0; i < GAME_MAX_MOVING_BLOCKS; ++i) {
        const MovingBlock *mb = &rs->moving_blocks[i];
        if (!mb->active) {
            continue;
        }
        int px = 0;
        int py = 0;
        compute_moving_block_render_pos(mb, &px, &py);
        draw_tile(ts, kBlockSprites[mb->type], px, py, false, index_buffer);
        if (is_pushable_block(mb->type)) {
            draw_block_bevel(index_buffer, px, py);
            draw_block_contact_shadow(index_buffer, px, py, true);
        }
        if (!low_intensity_viz) {
            draw_moving_block_state_overlay(index_buffer, mb, px, py);
        }
    }

    for (int i = 0; i < rs->enemy_count; ++i) {
        const Enemy *enemy = &rs->enemies[i];
        if (!enemy->alive) {
            continue;
        }
        const int px = enemy->pixel_fp_x >> 8;
        const int py = enemy->pixel_fp_y >> 8;
        draw_actor_drop_shadow(index_buffer, px, py);
        const SpriteId enemy_sprite = enemy_sprite_for_state(enemy);
        draw_tile(ts, enemy_sprite, px, py, true, index_buffer);
        if (!low_intensity_viz && enemy->state == ENEMY_SPAWNING && enemy->spawn_ticks > 0) {
            draw_enemy_spawn_cue(index_buffer, px, py, enemy->spawn_ticks);
        } else if (!low_intensity_viz) {
            draw_enemy_threat_corners(index_buffer, px, py, enemy->anim_frame + enemy->tile_x + enemy->tile_y);
        }
    }

    if (rs->player.alive) {
        int px = rs->player.pixel_fp_x >> 8;
        int py = rs->player.pixel_fp_y >> 8;
        const bool player_is_moving = (rs->player.state == PLAYER_WALKING || rs->player.state == PLAYER_PUSHING);
        const int player_phase = (rs->player.pixel_fp_x + rs->player.pixel_fp_y) >> 9;
        const bool player_alt_frame = player_is_moving && ((player_phase & 1) != 0);
        const SpriteId player_sprite = player_sprite_for_direction(rs->player.facing, player_alt_frame);
        if (rs->player.respawn_invuln_ticks <= 0 || ((rs->player.respawn_invuln_ticks / 4) & 1) == 0) {
            if (!low_intensity_viz && rs->player.state == PLAYER_PUSHING && rs->player.push_timer > 0) {
                px += dir_dx(rs->player.facing);
                py += dir_dy(rs->player.facing);
                draw_push_skid_marks(index_buffer, px, py, rs->player.facing, rs->player.push_timer);
            }
            draw_actor_drop_shadow(index_buffer, px, py);
            draw_tile(ts, player_sprite, px, py, true, index_buffer);
        }
    }

    draw_impact_fx(rs, ts, index_buffer, low_intensity_viz);
    draw_score_popups(rs, index_buffer, low_intensity_viz);
    render_hud(rs, ts, index_buffer, low_intensity_viz, score_gain_value, score_gain_ticks);
    render_overlay(rs, ts, title_overlay, high_scores, index_buffer, low_intensity_viz, show_upgrade_details);
}

static void indices_to_argb(const TileSet *ts, const uint8_t *index_buffer, uint32_t *argb_buffer) {
    for (int i = 0; i < GAME_LOGICAL_WIDTH * GAME_LOGICAL_HEIGHT; ++i) {
        const uint8_t idx = index_buffer[i] & 31u;
        const uint8_t r = ts->palette[idx][0];
        const uint8_t g = ts->palette[idx][1];
        const uint8_t b = ts->palette[idx][2];
        argb_buffer[i] = 0xff000000u | ((uint32_t)r << 16u) | ((uint32_t)g << 8u) | (uint32_t)b;
    }
}

static void handle_event(const SDL_Event *event, InputCapture *input) {
    if (event->type == SDL_QUIT) {
        input->quit = true;
        return;
    }

    if (event->type == SDL_CONTROLLERBUTTONDOWN || event->type == SDL_CONTROLLERBUTTONUP) {
        const bool down = (event->type == SDL_CONTROLLERBUTTONDOWN);
        switch (event->cbutton.button) {
            case SDL_CONTROLLER_BUTTON_DPAD_UP:
                input->up = down;
                if (down) {
                    input->newest_press = DIR_UP;
                }
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                input->down = down;
                if (down) {
                    input->newest_press = DIR_DOWN;
                }
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                input->left = down;
                if (down) {
                    input->newest_press = DIR_LEFT;
                }
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                input->right = down;
                if (down) {
                    input->newest_press = DIR_RIGHT;
                }
                break;
            case SDL_CONTROLLER_BUTTON_A:
            case SDL_CONTROLLER_BUTTON_B:
            case SDL_CONTROLLER_BUTTON_X:
            case SDL_CONTROLLER_BUTTON_Y:
                if (down) {
                    input->fire_pressed = true;
                } else {
                    input->fire_released = true;
                }
                break;
            case SDL_CONTROLLER_BUTTON_START:
                input->start = down;
                break;
            default:
                break;
        }
        return;
    }

    if (event->type != SDL_KEYDOWN && event->type != SDL_KEYUP) {
        return;
    }

    const bool down = (event->type == SDL_KEYDOWN);
    const SDL_Keycode key = event->key.keysym.sym;
    if (event->type == SDL_KEYDOWN && event->key.repeat != 0) {
        return;
    }

    switch (key) {
        case SDLK_ESCAPE:
            if (down) {
                input->quit = true;
            }
            break;
        case SDLK_UP:
        case SDLK_w:
            input->up = down;
            if (down) {
                input->newest_press = DIR_UP;
            }
            break;
        case SDLK_DOWN:
        case SDLK_s:
            input->down = down;
            if (down) {
                input->newest_press = DIR_DOWN;
            }
            break;
        case SDLK_LEFT:
        case SDLK_a:
            input->left = down;
            if (down) {
                input->newest_press = DIR_LEFT;
            }
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            input->right = down;
            if (down) {
                input->newest_press = DIR_RIGHT;
            }
            break;
        case SDLK_SPACE:
            input->start = down;
            if (down) {
                input->fire_pressed = true;
            } else {
                input->fire_released = true;
            }
            break;
        case SDLK_LCTRL:
        case SDLK_RCTRL:
        case SDLK_z:
        case SDLK_x:
            if (down) {
                input->fire_pressed = true;
            } else {
                input->fire_released = true;
            }
            break;
        case SDLK_RETURN:
            input->start = down;
            break;
        case SDLK_TAB:
            if (down) {
                input->toggle_upgrade_panel = true;
            }
            break;
        case SDLK_F9:
            if (down) {
                input->trigger_sound_test = true;
            }
            break;
        default:
            break;
    }
}

static InputState consume_input(InputCapture *input) {
    InputState in = {0};
    in.up = input->up;
    in.down = input->down;
    in.left = input->left;
    in.right = input->right;
    in.start = input->start;
    in.fire_pressed = input->fire_pressed;
    in.fire_released = input->fire_released;
    in.newest_press = input->newest_press;
    input->fire_pressed = false;
    input->fire_released = false;
    input->newest_press = DIR_NONE;
    return in;
}

static void print_usage(const char *exe_name) {
    fprintf(
        stderr,
        "Usage: %s [--dump-indexed <dir>] [--dump-max-frames <count>] [--session-metrics-out <path>] [--low-intensity-viz] [--help]\n",
        exe_name);
}

static bool parse_positive_int(const char *text, int *out_value) {
    char *end = NULL;
    long value = strtol(text, &end, 10);
    if (end == text || *end != '\0') {
        return false;
    }
    if (value <= 0 || value > 1000000L) {
        return false;
    }
    *out_value = (int)value;
    return true;
}

static bool make_dir_if_needed(const char *path) {
#ifdef _WIN32
    if (_mkdir(path) == 0 || errno == EEXIST) {
        return true;
    }
#else
    if (mkdir(path, 0777) == 0 || errno == EEXIST) {
        return true;
    }
#endif
    return false;
}

static bool ensure_dir_recursive(const char *path) {
    char tmp[260];
    const size_t len = strlen(path);
    if (len == 0 || len >= sizeof(tmp)) {
        return false;
    }
    memcpy(tmp, path, len + 1);

    for (size_t i = 0; i < len; ++i) {
        if (tmp[i] != '/' && tmp[i] != '\\') {
            continue;
        }
        if (i == 0) {
            continue;
        }
        if (i == 2 && tmp[1] == ':') {
            continue; /* Windows drive root */
        }

        const char saved = tmp[i];
        tmp[i] = '\0';
        if (strlen(tmp) > 0 && !make_dir_if_needed(tmp)) {
            tmp[i] = saved;
            return false;
        }
        tmp[i] = saved;
    }

    return make_dir_if_needed(tmp);
}

static bool ensure_parent_dir_for_file(const char *path) {
    char parent[260];
    size_t len = 0;
    char *slash = NULL;
    char *backslash = NULL;
    char *sep = NULL;

    if (path == NULL || path[0] == '\0') {
        return false;
    }
    len = strlen(path);
    if (len >= sizeof(parent)) {
        return false;
    }
    memcpy(parent, path, len + 1);

    slash = strrchr(parent, '/');
    backslash = strrchr(parent, '\\');
    sep = slash;
    if (backslash != NULL && (sep == NULL || backslash > sep)) {
        sep = backslash;
    }
    if (sep == NULL) {
        return true;
    }
    if (sep == parent) {
        return true;
    }
    *sep = '\0';
    if (parent[0] == '\0') {
        return true;
    }
    return ensure_dir_recursive(parent);
}

static bool input_is_active(const InputState *in) {
    if (in == NULL) {
        return false;
    }
    return in->up || in->down || in->left || in->right || in->start ||
           in->fire_pressed || in->fire_released || in->newest_press != DIR_NONE;
}

static void session_metrics_record_events(SessionMetrics *metrics, const GameState *game, uint32_t events) {
    if (metrics == NULL || events == GAME_EVENT_NONE) {
        return;
    }
    if ((events & GAME_EVENT_CRUSH) != 0u) {
        ++metrics->crush_events;
    }
    if ((events & GAME_EVENT_ITEM_COLLECT) != 0u) {
        ++metrics->item_collect_events;
    }
    if ((events & GAME_EVENT_TIMER_DANGER_PULSE) != 0u) {
        ++metrics->timer_pulse_events;
    }
    if ((events & GAME_EVENT_SPECIAL_ALIGNMENT) != 0u) {
        ++metrics->special_alignment_events;
    }
    if ((events & GAME_EVENT_PLAYER_DIED) != 0u) {
        ++metrics->player_died_events;
    }
    if ((events & GAME_EVENT_ROUND_CLEAR) != 0u) {
        ++metrics->round_clear_events;
    }
    if ((events & GAME_EVENT_ROUND_START) != 0u) {
        ++metrics->round_start_events;
    }
    if ((events & GAME_EVENT_GAME_OVER) != 0u) {
        ++metrics->game_over_events;
    }
    if ((events & GAME_EVENT_BLOCK_IMPACT) != 0u) {
        ++metrics->block_impact_events;
    }
    if ((events & GAME_EVENT_BLOCK_PUSH) != 0u) {
        ++metrics->block_push_events;
    }
    if ((events & GAME_EVENT_META_BANKED) != 0u) {
        ++metrics->meta_banked_events;
    }
    if ((events & GAME_EVENT_STAGE_MODIFIER) != 0u) {
        ++metrics->stage_modifier_events;
        if (game != NULL && game->stage_modifier >= STAGE_MOD_SWARM && game->stage_modifier <= STAGE_MOD_FROST_SIREN) {
            metrics->stage_modifier_mask |= (uint32_t)(1u << game->stage_modifier);
        }
    }
    if ((events & GAME_EVENT_META_SPENT) != 0u) {
        ++metrics->meta_spent_events;
    }
    if ((events & GAME_EVENT_MINE_PLACED) != 0u) {
        ++metrics->mine_placed_events;
    }
    if ((events & GAME_EVENT_MINE_EXPLODED) != 0u) {
        ++metrics->mine_exploded_events;
    }
    if ((events & GAME_EVENT_LIFE_LOST) != 0u) {
        ++metrics->life_lost_events;
    }
    if ((events & GAME_EVENT_COMBO) != 0u) {
        ++metrics->combo_events;
    }
}

static bool write_session_metrics_json(const char *path, const SessionMetrics *metrics, const GameState *game) {
    FILE *f = NULL;
    bool first = true;
    const double seconds = (double)metrics->ticks_simulated / (double)GAME_FIXED_TICK_HZ;
    const double active_ratio = (metrics->playing_ticks > 0u)
                                    ? ((double)metrics->active_input_ticks / (double)metrics->playing_ticks)
                                    : 0.0;
    const double direction_change_rate = (metrics->playing_ticks > 0u)
                                             ? ((double)metrics->direction_changes / (double)metrics->playing_ticks)
                                             : 0.0;

    if (!ensure_parent_dir_for_file(path)) {
        return false;
    }
    f = fopen(path, "wb");
    if (f == NULL) {
        return false;
    }

    fprintf(f, "{\n");
    fprintf(f, "  \"schema_version\": 1,\n");
    fprintf(f, "  \"source\": \"icepanic_sdl_session\",\n");
    fprintf(f, "  \"tick_hz\": %d,\n", GAME_FIXED_TICK_HZ);
    fprintf(f, "  \"ticks_simulated\": %llu,\n", (unsigned long long)metrics->ticks_simulated);
    fprintf(f, "  \"seconds_simulated\": %.3f,\n", seconds);
    fprintf(f, "  \"playing_ticks\": %llu,\n", (unsigned long long)metrics->playing_ticks);
    fprintf(f, "  \"active_input_ticks\": %llu,\n", (unsigned long long)metrics->active_input_ticks);
    fprintf(f, "  \"active_ratio\": %.6f,\n", active_ratio);
    fprintf(f, "  \"direction_changes\": %llu,\n", (unsigned long long)metrics->direction_changes);
    fprintf(f, "  \"direction_change_rate\": %.6f,\n", direction_change_rate);
    fprintf(f, "  \"max_round_reached\": %d,\n", metrics->max_round_reached);
    fprintf(f, "  \"stage_modifiers_seen\": [");
    for (int modifier = STAGE_MOD_SWARM; modifier <= STAGE_MOD_FROST_SIREN; ++modifier) {
        const uint32_t bit = (uint32_t)(1u << modifier);
        if ((metrics->stage_modifier_mask & bit) == 0u) {
            continue;
        }
        if (!first) {
            fprintf(f, ", ");
        }
        fprintf(f, "\"%s\"", stage_modifier_name((StageModifierType)modifier));
        first = false;
    }
    fprintf(f, "],\n");
    fprintf(f, "  \"events\": {\n");
    fprintf(f, "    \"crush\": %llu,\n", (unsigned long long)metrics->crush_events);
    fprintf(f, "    \"item_collect\": %llu,\n", (unsigned long long)metrics->item_collect_events);
    fprintf(f, "    \"timer_pulse\": %llu,\n", (unsigned long long)metrics->timer_pulse_events);
    fprintf(f, "    \"special_alignment\": %llu,\n", (unsigned long long)metrics->special_alignment_events);
    fprintf(f, "    \"player_died\": %llu,\n", (unsigned long long)metrics->player_died_events);
    fprintf(f, "    \"round_clear\": %llu,\n", (unsigned long long)metrics->round_clear_events);
    fprintf(f, "    \"round_start\": %llu,\n", (unsigned long long)metrics->round_start_events);
    fprintf(f, "    \"game_over\": %llu,\n", (unsigned long long)metrics->game_over_events);
    fprintf(f, "    \"block_impact\": %llu,\n", (unsigned long long)metrics->block_impact_events);
    fprintf(f, "    \"block_push\": %llu,\n", (unsigned long long)metrics->block_push_events);
    fprintf(f, "    \"meta_banked\": %llu,\n", (unsigned long long)metrics->meta_banked_events);
    fprintf(f, "    \"stage_modifier\": %llu,\n", (unsigned long long)metrics->stage_modifier_events);
    fprintf(f, "    \"meta_spent\": %llu,\n", (unsigned long long)metrics->meta_spent_events);
    fprintf(f, "    \"mine_placed\": %llu,\n", (unsigned long long)metrics->mine_placed_events);
    fprintf(f, "    \"mine_exploded\": %llu,\n", (unsigned long long)metrics->mine_exploded_events);
    fprintf(f, "    \"life_lost\": %llu,\n", (unsigned long long)metrics->life_lost_events);
    fprintf(f, "    \"combo\": %llu\n", (unsigned long long)metrics->combo_events);
    fprintf(f, "  },\n");
    fprintf(f, "  \"final_state\": {\n");
    fprintf(f, "    \"score\": %u,\n", (unsigned int)game->score);
    fprintf(f, "    \"round\": %d,\n", game->round);
    fprintf(f, "    \"lives\": %d,\n", game->lives);
    fprintf(f, "    \"phase\": \"%s\",\n", phase_name(game->phase));
    fprintf(f, "    \"meta_shards\": %u,\n", (unsigned int)game->meta_shards);
    fprintf(f, "    \"meta_progress\": %u,\n", (unsigned int)game->meta_progress_points);
    fprintf(f, "    \"run_shards\": %d\n", game->run_shards);
    fprintf(f, "  }\n");
    fprintf(f, "}\n");

    fclose(f);
    return true;
}

static bool parse_prefixed_u32(const char *line, const char *prefix, uint32_t *out_value) {
    const size_t prefix_len = strlen(prefix);
    if (strncmp(line, prefix, prefix_len) != 0) {
        return false;
    }
    {
        char *end = NULL;
        const unsigned long parsed = strtoul(line + prefix_len, &end, 10);
        if (end == line + prefix_len) {
            return false;
        }
        *out_value = (uint32_t)parsed;
        return true;
    }
}

static void sanitize_initials(char out_initials[4], const char *input) {
    int out_idx = 0;
    if (out_initials == NULL) {
        return;
    }
    if (input != NULL) {
        for (int i = 0; input[i] != '\0' && out_idx < 3; ++i) {
            const unsigned char ch = (unsigned char)input[i];
            if (ch >= 'a' && ch <= 'z') {
                out_initials[out_idx++] = (char)(ch - 'a' + 'A');
            } else if (ch >= 'A' && ch <= 'Z') {
                out_initials[out_idx++] = (char)ch;
            }
        }
    }
    while (out_idx < 3) {
        out_initials[out_idx++] = 'A';
    }
    out_initials[3] = '\0';
}

static void high_score_table_set_defaults(HighScoreTable *table) {
    if (table == NULL) {
        return;
    }
    memset(table, 0, sizeof(*table));
    for (int i = 0; i < (int)(sizeof(table->entries) / sizeof(table->entries[0])); ++i) {
        table->entries[i] = kOpeningHiScores[i];
    }
}

static void high_score_table_normalize(HighScoreTable *table) {
    if (table == NULL) {
        return;
    }
    for (int i = 0; i < (int)(sizeof(table->entries) / sizeof(table->entries[0])); ++i) {
        sanitize_initials(table->entries[i].initials, table->entries[i].initials);
    }
    for (int i = 1; i < (int)(sizeof(table->entries) / sizeof(table->entries[0])); ++i) {
        const HiScoreEntry key = table->entries[i];
        int j = i - 1;
        while (j >= 0 && key.score > table->entries[j].score) {
            table->entries[j + 1] = table->entries[j];
            --j;
        }
        table->entries[j + 1] = key;
    }
}

static int high_score_find_insert_index(const HighScoreTable *table, uint32_t score) {
    if (table == NULL) {
        return -1;
    }
    for (int i = 0; i < (int)(sizeof(table->entries) / sizeof(table->entries[0])); ++i) {
        if (score >= table->entries[i].score) {
            return i;
        }
    }
    return -1;
}

static void high_score_insert(HighScoreTable *table, int index, const char initials[4], uint32_t score) {
    if (table == NULL) {
        return;
    }
    if (index < 0 || index >= (int)(sizeof(table->entries) / sizeof(table->entries[0]))) {
        return;
    }
    for (int i = (int)(sizeof(table->entries) / sizeof(table->entries[0])) - 1; i > index; --i) {
        table->entries[i] = table->entries[i - 1];
    }
    sanitize_initials(table->entries[index].initials, initials);
    table->entries[index].score = score;
    high_score_table_normalize(table);
}

static void load_high_score_table(HighScoreTable *table) {
    char line[96];
    high_score_table_set_defaults(table);
    if (table == NULL) {
        return;
    }
    for (size_t i = 0; i < sizeof(kHighScorePaths) / sizeof(kHighScorePaths[0]); ++i) {
        FILE *f = fopen(kHighScorePaths[i], "rb");
        int loaded_count = 0;
        HighScoreTable loaded;
        if (f == NULL) {
            continue;
        }
        if (fgets(line, sizeof(line), f) == NULL) {
            fclose(f);
            continue;
        }
        if (strncmp(line, "ICEPANIC_HISCORES_V1", 20) != 0) {
            fclose(f);
            continue;
        }
        loaded = *table;
        while (loaded_count < (int)(sizeof(loaded.entries) / sizeof(loaded.entries[0])) &&
               fgets(line, sizeof(line), f) != NULL) {
            char parsed_initials[16];
            unsigned int parsed_score = 0u;
            if (sscanf(line, " %15[^,],%u", parsed_initials, &parsed_score) == 2) {
                sanitize_initials(loaded.entries[loaded_count].initials, parsed_initials);
                loaded.entries[loaded_count].score = (uint32_t)parsed_score;
                ++loaded_count;
            }
        }
        fclose(f);
        if (loaded_count > 0) {
            high_score_table_normalize(&loaded);
            *table = loaded;
            return;
        }
    }
}

static bool save_high_score_table(const HighScoreTable *table) {
    if (table == NULL) {
        return false;
    }
    {
        HighScoreTable normalized = *table;
        high_score_table_normalize(&normalized);
        for (size_t i = 0; i < sizeof(kHighScorePaths) / sizeof(kHighScorePaths[0]); ++i) {
            FILE *f = NULL;
            if (!ensure_parent_dir_for_file(kHighScorePaths[i])) {
                continue;
            }
            f = fopen(kHighScorePaths[i], "wb");
            if (f == NULL) {
                continue;
            }
            fprintf(f, "ICEPANIC_HISCORES_V1\n");
            for (int idx = 0; idx < (int)(sizeof(normalized.entries) / sizeof(normalized.entries[0])); ++idx) {
                fprintf(
                    f,
                    "%s,%u\n",
                    normalized.entries[idx].initials,
                    (unsigned int)normalized.entries[idx].score);
            }
            fclose(f);
            return true;
        }
    }
    return false;
}

static char cycle_initial_letter(char letter, int delta) {
    int value = 0;
    if (letter < 'A' || letter > 'Z') {
        letter = 'A';
    }
    value = (int)(letter - 'A');
    value = (value + delta) % 26;
    if (value < 0) {
        value += 26;
    }
    return (char)('A' + value);
}

static void begin_high_score_entry(
    HighScoreEntryState *entry_state,
    int insert_index,
    uint32_t score,
    const HighScoreTable *table) {
    if (entry_state == NULL) {
        return;
    }
    memset(entry_state, 0, sizeof(*entry_state));
    entry_state->active = true;
    entry_state->insert_index = insert_index;
    entry_state->pending_score = score;
    entry_state->cursor = 0;
    entry_state->blink_ticks = 0;
    entry_state->confirm_armed = false;
    if (table != NULL &&
        insert_index >= 0 &&
        insert_index < (int)(sizeof(table->entries) / sizeof(table->entries[0]))) {
        sanitize_initials(entry_state->initials, table->entries[insert_index].initials);
    } else {
        sanitize_initials(entry_state->initials, "AAA");
    }
}

static bool update_high_score_entry(HighScoreEntryState *entry_state, const InputState *in) {
    if (entry_state == NULL || !entry_state->active || in == NULL) {
        return false;
    }
    entry_state->blink_ticks = (entry_state->blink_ticks + 1) % (GAME_FIXED_TICK_HZ * 4);
    switch (in->newest_press) {
        case DIR_LEFT:
            entry_state->cursor = (entry_state->cursor + 2) % 3;
            break;
        case DIR_RIGHT:
            entry_state->cursor = (entry_state->cursor + 1) % 3;
            break;
        case DIR_UP:
            entry_state->initials[entry_state->cursor] = cycle_initial_letter(entry_state->initials[entry_state->cursor], +1);
            break;
        case DIR_DOWN:
            entry_state->initials[entry_state->cursor] = cycle_initial_letter(entry_state->initials[entry_state->cursor], -1);
            break;
        default:
            break;
    }
    if (!entry_state->confirm_armed) {
        if (!in->start && !in->fire_pressed) {
            entry_state->confirm_armed = true;
        }
    } else if (in->start || in->fire_pressed) {
        entry_state->active = false;
        return true;
    }
    return false;
}

static MetaProfile load_meta_profile(void) {
    MetaProfile profile = {0u, 0u};
    char line[96];
    for (size_t i = 0; i < sizeof(kMetaShardPaths) / sizeof(kMetaShardPaths[0]); ++i) {
        FILE *f = fopen(kMetaShardPaths[i], "rb");
        if (f == NULL) {
            continue;
        }
        if (fgets(line, sizeof(line), f) == NULL) {
            fclose(f);
            continue;
        }

        if (strncmp(line, "ICEPANIC_META_V2", 16) == 0) {
            bool have_shards = false;
            bool have_progress = false;
            while (fgets(line, sizeof(line), f) != NULL) {
                uint32_t parsed = 0u;
                if (parse_prefixed_u32(line, "shards=", &parsed)) {
                    profile.shards = parsed;
                    have_shards = true;
                    continue;
                }
                if (parse_prefixed_u32(line, "progress=", &parsed)) {
                    profile.progress_points = parsed;
                    have_progress = true;
                    continue;
                }
            }
            fclose(f);
            if (have_shards || have_progress) {
                if (!have_progress) {
                    profile.progress_points = profile.shards;
                }
                return profile;
            }
            continue;
        }

        {
            char *end = NULL;
            const unsigned long parsed = strtoul(line, &end, 10);
            fclose(f);
            if (end != line) {
                profile.shards = (uint32_t)parsed;
                profile.progress_points = profile.shards;
                return profile;
            }
        }
    }
    return profile;
}

static bool save_meta_profile(const MetaProfile *profile) {
    for (size_t i = 0; i < sizeof(kMetaShardPaths) / sizeof(kMetaShardPaths[0]); ++i) {
        FILE *f = fopen(kMetaShardPaths[i], "wb");
        if (f == NULL) {
            continue;
        }
        fprintf(f, "ICEPANIC_META_V2\n");
        fprintf(f, "shards=%u\n", (unsigned int)profile->shards);
        fprintf(f, "progress=%u\n", (unsigned int)profile->progress_points);
        fclose(f);
        return true;
    }
    return false;
}

static bool frame_dump_write_palette(const FrameDumpConfig *dump_cfg, const TileSet *ts) {
    char path[512];
    FILE *f = NULL;

    if (snprintf(path, sizeof(path), "%s/palette.rgb", dump_cfg->out_dir) >= (int)sizeof(path)) {
        return false;
    }
    f = fopen(path, "wb");
    if (f == NULL) {
        return false;
    }
    for (int i = 0; i < 32; ++i) {
        if (fwrite(ts->palette[i], 1, 3, f) != 3) {
            fclose(f);
            return false;
        }
    }
    fclose(f);

    if (snprintf(path, sizeof(path), "%s/frame_info.txt", dump_cfg->out_dir) >= (int)sizeof(path)) {
        return false;
    }
    f = fopen(path, "wb");
    if (f == NULL) {
        return false;
    }
    fprintf(f, "width=%d\n", GAME_LOGICAL_WIDTH);
    fprintf(f, "height=%d\n", GAME_LOGICAL_HEIGHT);
    fprintf(f, "pixel_format=index8_palette32\n");
    fprintf(f, "frame_file_pattern=frame_%%05d.idx\n");
    fclose(f);

    return true;
}

static bool frame_dump_write_frame(const FrameDumpConfig *dump_cfg, const uint8_t *index_buffer, int frame_index) {
    char path[512];
    FILE *f = NULL;

    if (snprintf(path, sizeof(path), "%s/frame_%05d.idx", dump_cfg->out_dir, frame_index) >= (int)sizeof(path)) {
        return false;
    }
    f = fopen(path, "wb");
    if (f == NULL) {
        return false;
    }
    if (fwrite(index_buffer, 1, GAME_LOGICAL_WIDTH * GAME_LOGICAL_HEIGHT, f) != GAME_LOGICAL_WIDTH * GAME_LOGICAL_HEIGHT) {
        fclose(f);
        return false;
    }
    fclose(f);
    return true;
}

static void maybe_dump_indexed_frame(FrameDumpConfig *dump_cfg, const TileSet *ts, const uint8_t *index_buffer) {
    if (!dump_cfg->enabled || dump_cfg->failed) {
        return;
    }
    if (dump_cfg->frames_written >= dump_cfg->max_frames) {
        return;
    }

    if (!dump_cfg->initialized) {
        if (!ensure_dir_recursive(dump_cfg->out_dir) || !frame_dump_write_palette(dump_cfg, ts)) {
            fprintf(stderr, "Frame dump init failed for '%s'.\n", dump_cfg->out_dir);
            dump_cfg->failed = true;
            return;
        }
        dump_cfg->initialized = true;
    }

    if (!frame_dump_write_frame(dump_cfg, index_buffer, dump_cfg->frames_written)) {
        fprintf(stderr, "Frame dump write failed for '%s'.\n", dump_cfg->out_dir);
        dump_cfg->failed = true;
        return;
    }

    ++dump_cfg->frames_written;
    if (dump_cfg->frames_written == dump_cfg->max_frames) {
        fprintf(stderr, "Frame dump complete: %d frames written to %s\n", dump_cfg->frames_written, dump_cfg->out_dir);
    }
}

static bool parse_cli_args(int argc, char **argv, FrameDumpConfig *dump_cfg) {
    memset(dump_cfg, 0, sizeof(*dump_cfg));
    dump_cfg->max_frames = 300;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            dump_cfg->show_help_only = true;
            return true;
        }

        if (strcmp(argv[i], "--dump-indexed") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--dump-indexed requires a directory path.\n");
                print_usage(argv[0]);
                return false;
            }
            ++i;
            dump_cfg->enabled = true;
            if (snprintf(dump_cfg->out_dir, sizeof(dump_cfg->out_dir), "%s", argv[i]) >= (int)sizeof(dump_cfg->out_dir)) {
                fprintf(stderr, "Dump path too long.\n");
                return false;
            }
            continue;
        }

        if (strcmp(argv[i], "--dump-max-frames") == 0) {
            int value = 0;
            if (i + 1 >= argc || !parse_positive_int(argv[i + 1], &value)) {
                fprintf(stderr, "--dump-max-frames requires a positive integer.\n");
                print_usage(argv[0]);
                return false;
            }
            ++i;
            dump_cfg->max_frames = value;
            continue;
        }

        if (strcmp(argv[i], "--session-metrics-out") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "--session-metrics-out requires a file path.\n");
                print_usage(argv[0]);
                return false;
            }
            ++i;
            dump_cfg->session_metrics_enabled = true;
            if (snprintf(dump_cfg->session_metrics_path, sizeof(dump_cfg->session_metrics_path), "%s", argv[i]) >=
                (int)sizeof(dump_cfg->session_metrics_path)) {
                fprintf(stderr, "Session metrics path too long.\n");
                return false;
            }
            continue;
        }

        if (strcmp(argv[i], "--low-intensity-viz") == 0) {
            dump_cfg->low_intensity_viz = true;
            continue;
        }

        fprintf(stderr, "Unknown argument: %s\n", argv[i]);
        print_usage(argv[0]);
        return false;
    }

    if (dump_cfg->enabled && dump_cfg->out_dir[0] == '\0') {
        if (snprintf(dump_cfg->out_dir, sizeof(dump_cfg->out_dir), "frame_dump") >= (int)sizeof(dump_cfg->out_dir)) {
            return false;
        }
    }

    return true;
}

int main(int argc, char **argv) {
    int exit_code = 0;
    FrameDumpConfig dump_cfg;
    if (!parse_cli_args(argc, argv, &dump_cfg)) {
        return 1;
    }
    if (dump_cfg.show_help_only) {
        return 0;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Window *window = SDL_CreateWindow(
        "Icepanic V1",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        GAME_LOGICAL_WIDTH * 3,
        GAME_LOGICAL_HEIGHT * 3,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        GAME_LOGICAL_WIDTH,
        GAME_LOGICAL_HEIGHT);
    if (texture == NULL) {
        fprintf(stderr, "SDL_CreateTexture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TileSet tileset;
    TitleOverlay title_overlay;
    SfxState sfx;
    SDL_GameController *game_controller = NULL;
    load_tileset(&tileset);
    load_title_overlay(&title_overlay);
    sfx_init(&sfx);
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (!SDL_IsGameController(i)) {
            continue;
        }
        game_controller = SDL_GameControllerOpen(i);
        if (game_controller != NULL) {
            break;
        }
    }

    GameState game;
    RenderState render_state;
    HighScoreTable high_scores;
    HighScoreEntryState high_score_entry = {0};
    int post_high_score_pause_ticks = 0;
    bool pending_post_game_over_resolution = false;
    int pending_post_game_over_insert_index = -1;
    uint32_t pending_post_game_over_score = 0u;
    bool title_jingle_queued = false;
    bool was_title_pending = false;
    const MetaProfile loaded_meta = load_meta_profile();
    MetaProfile saved_meta = loaded_meta;
    load_high_score_table(&high_scores);
    game_init(&game, 0x00c0ffeeu);
    game_set_meta_shards(&game, loaded_meta.shards);
    game_set_meta_progress(&game, loaded_meta.progress_points);
    {
        uint32_t boot_events = game_consume_events(&game);
        if (game.phase == GAME_PHASE_ROUND_INTRO && game.start_title_pending) {
            if ((boot_events & GAME_EVENT_ROUND_START) != 0u) {
                sfx_queue_title_start_jingle(&sfx);
                title_jingle_queued = true;
            }
            boot_events &= (uint32_t)~GAME_EVENT_ROUND_START;
        }
        sfx_dispatch_events(&sfx, boot_events);
    }

    uint8_t index_buffer[GAME_LOGICAL_WIDTH * GAME_LOGICAL_HEIGHT];
    uint32_t argb_buffer[GAME_LOGICAL_WIDTH * GAME_LOGICAL_HEIGHT];

    InputCapture input = {0};
    SessionMetrics session_metrics = {0};
    bool show_upgrade_details = false;
    int hud_score_gain_value = 0;
    int hud_score_gain_ticks = 0;
    input.newest_press = DIR_NONE;
    session_metrics.last_pressed_dir = DIR_NONE;
    session_metrics.max_round_reached = game.round;

    const uint64_t freq = SDL_GetPerformanceFrequency();
    uint64_t previous = SDL_GetPerformanceCounter();
    double accumulator = 0.0;
    const double tick_seconds = 1.0 / (double)GAME_FIXED_TICK_HZ;

    while (!input.quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handle_event(&event, &input);
        }
        if (input.toggle_upgrade_panel) {
            show_upgrade_details = !show_upgrade_details;
            input.toggle_upgrade_panel = false;
        }
        if (input.trigger_sound_test) {
            sfx_queue_audition_demo(&sfx);
            input.trigger_sound_test = false;
        }

        const uint64_t now = SDL_GetPerformanceCounter();
        double delta = (double)(now - previous) / (double)freq;
        previous = now;
        if (delta > 0.25) {
            delta = 0.25;
        }
        accumulator += delta;

        while (accumulator >= tick_seconds) {
            g_ui_anim_tick = (g_ui_anim_tick + 1) & 0x7fffffff;
            if (high_score_entry.active) {
                const InputState entry_input = consume_input(&input);
                if (update_high_score_entry(&high_score_entry, &entry_input)) {
                    high_score_insert(
                        &high_scores,
                        high_score_entry.insert_index,
                        high_score_entry.initials,
                        high_score_entry.pending_score);
                    if (!save_high_score_table(&high_scores)) {
                        fprintf(stderr, "Failed to save high score table.\n");
                    }
                    post_high_score_pause_ticks = (GAME_FIXED_TICK_HZ * 3) / 5;
                    pending_post_game_over_resolution = false;
                    pending_post_game_over_insert_index = -1;
                    pending_post_game_over_score = 0u;
                    if (game.phase == GAME_PHASE_GAME_OVER) {
                        {
                            const InputState resume_input = {.start = true};
                            game_step(&game, &resume_input);
                        }
                        {
                            const uint32_t events = game_consume_events(&game);
                            uint32_t audio_events = events;
                            if (game.phase == GAME_PHASE_ROUND_INTRO && game.start_title_pending) {
                                if ((audio_events & GAME_EVENT_ROUND_START) != 0u && !title_jingle_queued) {
                                    sfx_queue_title_start_jingle(&sfx);
                                    title_jingle_queued = true;
                                }
                                audio_events &= (uint32_t)~GAME_EVENT_ROUND_START;
                            }
                            sfx_dispatch_events(&sfx, audio_events);
                            if (dump_cfg.session_metrics_enabled) {
                                session_metrics_record_events(&session_metrics, &game, events);
                                if (game.round > session_metrics.max_round_reached) {
                                    session_metrics.max_round_reached = game.round;
                                }
                            }
                            if ((events & (GAME_EVENT_META_BANKED | GAME_EVENT_META_SPENT)) != 0u) {
                                const MetaProfile latest_meta = {
                                    game_get_meta_shards(&game),
                                    game_get_meta_progress(&game),
                                };
                                if (latest_meta.shards != saved_meta.shards ||
                                    latest_meta.progress_points != saved_meta.progress_points) {
                                    if (!save_meta_profile(&latest_meta)) {
                                        fprintf(stderr, "Failed to save meta profile.\n");
                                    } else {
                                        saved_meta = latest_meta;
                                    }
                                }
                            }
                        }
                    }
                }
                accumulator -= tick_seconds;
                continue;
            }
            if (post_high_score_pause_ticks > 0) {
                (void)consume_input(&input);
                input.up = false;
                input.down = false;
                input.left = false;
                input.right = false;
                input.start = false;
                input.fire_pressed = false;
                input.fire_released = false;
                input.newest_press = DIR_NONE;
                --post_high_score_pause_ticks;
                accumulator -= tick_seconds;
                continue;
            }
            const InputState frame_input = consume_input(&input);
            InputState step_input = frame_input;
            if (game.phase == GAME_PHASE_GAME_OVER && game.phase_timer_ticks > 0) {
                step_input.start = false;
                step_input.fire_pressed = false;
                step_input.fire_released = false;
                input.start = false;
                input.fire_pressed = false;
                input.fire_released = false;
            }
            const uint32_t score_before = game.score;
            if (dump_cfg.session_metrics_enabled) {
                ++session_metrics.ticks_simulated;
                if (game.phase == GAME_PHASE_PLAYING) {
                    ++session_metrics.playing_ticks;
                }
                if (input_is_active(&step_input)) {
                    ++session_metrics.active_input_ticks;
                }
                if (step_input.newest_press != DIR_NONE) {
                    if (session_metrics.last_pressed_dir != DIR_NONE &&
                        session_metrics.last_pressed_dir != step_input.newest_press) {
                        ++session_metrics.direction_changes;
                    }
                    session_metrics.last_pressed_dir = step_input.newest_press;
                }
            }
            game_step(&game, &step_input);
            if (game.score > score_before) {
                uint32_t gain = game.score - score_before;
                if (gain > 9999u) {
                    gain = 9999u;
                }
                hud_score_gain_value = (int)gain;
                hud_score_gain_ticks = GAME_FIXED_TICK_HZ;
            } else if (hud_score_gain_ticks > 0) {
                --hud_score_gain_ticks;
            }
            {
                const uint32_t events = game_consume_events(&game);
                uint32_t audio_events = events;
                const bool suppress_round_start_jingle =
                    pending_post_game_over_resolution &&
                    pending_post_game_over_insert_index >= 0 &&
                    !high_score_entry.active;

                if ((events & GAME_EVENT_GAME_OVER) != 0u && !high_score_entry.active) {
                    pending_post_game_over_resolution = true;
                    pending_post_game_over_score = game.score;
                    pending_post_game_over_insert_index = high_score_find_insert_index(&high_scores, game.score);
                    if (pending_post_game_over_insert_index >= 0) {
                        /* New high score run: use celebratory fanfare on the score showcase. */
                        audio_events &= (uint32_t)~GAME_EVENT_GAME_OVER;
                    }
                }

                if (suppress_round_start_jingle) {
                    audio_events &= (uint32_t)~GAME_EVENT_ROUND_START;
                }
                if (game.phase == GAME_PHASE_ROUND_INTRO && game.start_title_pending) {
                    if ((audio_events & GAME_EVENT_ROUND_START) != 0u && !title_jingle_queued) {
                        sfx_queue_title_start_jingle(&sfx);
                        title_jingle_queued = true;
                    }
                    audio_events &= (uint32_t)~GAME_EVENT_ROUND_START;
                }
                sfx_dispatch_events(&sfx, audio_events);
                if ((events & GAME_EVENT_GAME_OVER) != 0u &&
                    pending_post_game_over_insert_index >= 0) {
                    sfx_queue_new_high_score_fanfare(&sfx);
                }
                if (pending_post_game_over_resolution &&
                    !high_score_entry.active &&
                    game.phase != GAME_PHASE_GAME_OVER) {
                    if (pending_post_game_over_insert_index >= 0) {
                        begin_high_score_entry(
                            &high_score_entry,
                            pending_post_game_over_insert_index,
                            pending_post_game_over_score,
                            &high_scores);
                        /* Keep title-start jingle muted while name entry is active. */
                        title_jingle_queued = true;
                    }
                    pending_post_game_over_resolution = false;
                    pending_post_game_over_insert_index = -1;
                    pending_post_game_over_score = 0u;
                }
                if (dump_cfg.session_metrics_enabled) {
                    session_metrics_record_events(&session_metrics, &game, events);
                    if (game.round > session_metrics.max_round_reached) {
                        session_metrics.max_round_reached = game.round;
                    }
                }
                if ((events & (GAME_EVENT_META_BANKED | GAME_EVENT_META_SPENT)) != 0u) {
                    const MetaProfile latest_meta = {
                        game_get_meta_shards(&game),
                        game_get_meta_progress(&game),
                    };
                    if (latest_meta.shards != saved_meta.shards ||
                        latest_meta.progress_points != saved_meta.progress_points) {
                        if (!save_meta_profile(&latest_meta)) {
                            fprintf(stderr, "Failed to save meta profile.\n");
                        } else {
                            saved_meta = latest_meta;
                        }
                    }
                }
            }
            accumulator -= tick_seconds;
        }

        game_get_render_state(&game, &render_state);
        {
            const bool title_pending = (render_state.phase == GAME_PHASE_ROUND_INTRO && render_state.start_title_pending);
            const bool suppress_title_jingle =
                high_score_entry.active ||
                post_high_score_pause_ticks > 0 ||
                (pending_post_game_over_resolution && pending_post_game_over_insert_index >= 0);
            if (title_pending && !title_jingle_queued && !suppress_title_jingle) {
                sfx_queue_title_start_jingle(&sfx);
                title_jingle_queued = true;
            } else if (!title_pending) {
                title_jingle_queued = false;
            }
            if (!suppress_title_jingle && was_title_pending && !title_pending) {
                sfx_queue_title_confirm_chirp(&sfx);
            }
            was_title_pending = title_pending;
        }
        render_scene(
            &render_state,
            &tileset,
            &title_overlay,
            &high_scores,
            index_buffer,
            dump_cfg.low_intensity_viz,
            show_upgrade_details,
            hud_score_gain_value,
            hud_score_gain_ticks);
        draw_high_score_entry_overlay(index_buffer, dump_cfg.low_intensity_viz, &high_score_entry);
        maybe_dump_indexed_frame(&dump_cfg, &tileset, index_buffer);
        indices_to_argb(&tileset, index_buffer, argb_buffer);

        SDL_UpdateTexture(texture, NULL, argb_buffer, GAME_LOGICAL_WIDTH * (int)sizeof(uint32_t));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        int window_w = 0;
        int window_h = 0;
        SDL_GetWindowSize(window, &window_w, &window_h);
        int scale = window_w / GAME_LOGICAL_WIDTH;
        const int scale_y = window_h / GAME_LOGICAL_HEIGHT;
        if (scale_y < scale) {
            scale = scale_y;
        }
        if (scale < 1) {
            scale = 1;
        }

        SDL_Rect dst;
        dst.w = GAME_LOGICAL_WIDTH * scale;
        dst.h = GAME_LOGICAL_HEIGHT * scale;
        dst.x = (window_w - dst.w) / 2;
        dst.y = (window_h - dst.h) / 2;
        SDL_RenderCopy(renderer, texture, NULL, &dst);
        SDL_RenderPresent(renderer);
    }

    if (dump_cfg.session_metrics_enabled) {
        if (!write_session_metrics_json(dump_cfg.session_metrics_path, &session_metrics, &game)) {
            fprintf(stderr, "Failed to write session metrics to %s\n", dump_cfg.session_metrics_path);
            exit_code = 1;
        } else {
            fprintf(stderr, "Session metrics written: %s\n", dump_cfg.session_metrics_path);
        }
    }

    SDL_DestroyTexture(texture);
    if (game_controller != NULL) {
        SDL_GameControllerClose(game_controller);
    }
    sfx_shutdown(&sfx);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return exit_code;
}
