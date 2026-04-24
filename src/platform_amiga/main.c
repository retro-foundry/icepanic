#include "game.h"
#include "amiga_assets.h"

#include <exec/types.h>

#include "amiga_sfx_assets.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <exec/memory.h>
#include <exec/tasks.h>
#include <exec/libraries.h>
#include <libraries/dos.h>
#include <graphics/gfxbase.h>
#include <intuition/intuitionbase.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#define CREGS ((volatile struct Custom *)0xDFF000)

#define TASK_PRIORITY 20

#define SCREEN_W GAME_LOGICAL_WIDTH
#define SCREEN_H GAME_LOGICAL_HEIGHT
#ifndef DEPTH
#define DEPTH 5
#endif
#ifndef AMIGA_USE_HW_SPRITES
#define AMIGA_USE_HW_SPRITES 0
#endif
#ifndef AMIGA_USE_HW_SPARKLES
#define AMIGA_USE_HW_SPARKLES 1
#endif
#ifndef AMIGA_USE_TITLE_HW_SPARKLES
#define AMIGA_USE_TITLE_HW_SPARKLES AMIGA_USE_HW_SPARKLES
#endif
#ifndef AMIGA_USE_GAMEPLAY_HW_SPARKLES
#define AMIGA_USE_GAMEPLAY_HW_SPARKLES 0
#endif
#define AMIGA_HW_SPARKLES_ENABLED (AMIGA_USE_TITLE_HW_SPARKLES || AMIGA_USE_GAMEPLAY_HW_SPARKLES)
#define AMIGA_HW_SPRITES_ENABLED (AMIGA_USE_HW_SPRITES || AMIGA_HW_SPARKLES_ENABLED)
#ifndef AMIGA_FAST_RENDER
#define AMIGA_FAST_RENDER 1
#endif
#ifndef AMIGA_RENDER_DIVISOR
#define AMIGA_RENDER_DIVISOR 1
#endif
#ifndef AMIGA_SMOOTH_VSYNC
#define AMIGA_SMOOTH_VSYNC 1
#endif
#if defined(AMIGA_PROFILE_ON)
#undef AMIGA_PROFILE
#define AMIGA_PROFILE 1
#elif !defined(AMIGA_PROFILE)
#define AMIGA_PROFILE 0
#endif
#if defined(AMIGA_NO_SFX)
#undef AMIGA_SFX
#define AMIGA_SFX 0
#elif !defined(AMIGA_SFX)
#define AMIGA_SFX 1
#endif
#define ROW_BYTES (SCREEN_W / 8)
#define ROW_STRIDE_BYTES (ROW_BYTES * DEPTH)
#define SCREEN_BYTES ((ULONG)ROW_STRIDE_BYTES * SCREEN_H)
#define GAMEPLAY_H (GAME_GRID_HEIGHT * GAME_TILE_SIZE)

#define TILE_CHIP_BYTES ((ULONG)AMIGA_ASSET_SPRITE_COUNT * AMIGA_ASSET_TILE_BYTES)
#define TILE_MASK_CHIP_BYTES ((ULONG)AMIGA_ASSET_SPRITE_COUNT * AMIGA_ASSET_TILE_MASK_BYTES)
#define BOB_CHIP_BYTES ((ULONG)AMIGA_ASSET_SPRITE_COUNT * AMIGA_ASSET_BOB_BYTES)
#define MASK_CHIP_BYTES ((ULONG)AMIGA_ASSET_SPRITE_COUNT * AMIGA_ASSET_MASK_BYTES)
#define TITLE_CHIP_BYTES ((ULONG)AMIGA_TITLE_OVERLAY_BYTES)
#define TITLE_MASK_CHIP_BYTES ((ULONG)AMIGA_TITLE_OVERLAY_MASK_BYTES)

#define SFX_MAX_STEPS 32

#define PRA_FIR0_BIT (1 << 6)
#define PRA_FIR1_BIT (1 << 7)

#define COPJMP1 0x088
#define DIWSTRT 0x08E
#define DIWSTOP 0x090
#define DDFSTRT 0x092
#define DDFSTOP 0x094
#define BPL1PTH 0x0E0
#define BPL1PTL 0x0E2
#define BPLCON0 0x100
#define BPLCON1 0x102
#define BPLCON2 0x104
#define BPL1MOD 0x108
#define BPL2MOD 0x10A
#define SPR0PTH 0x120
#define SPR0PTL 0x122
#define COLOR00 0x180
#define COLOR_REG(n) (COLOR00 + ((n) << 1))

#define BLT_USEA 0x0800
#define BLT_USEB 0x0400
#define BLT_USEC 0x0200
#define BLT_USED 0x0100
#define BLT_MINTERM_COPY_B 0x00CC
#define BLT_MINTERM_COOKIE 0x00CA
#ifndef DMAF_BLTDONE
#define DMAF_BLTDONE 0x4000
#endif

#define HW_SPRITE_COUNT 8
#define HW_LAYERED_ENEMY_COUNT 2
#define HW_SPARKLE_FIRST_SLOT 6
#define HW_SPARKLE_COUNT 2
#define HW_SPARKLE_STATIC_PROBES 24
#define HW_SPRITE_HEIGHT GAME_TILE_SIZE
#define HW_SPRITE_WORDS (2 + (HW_SPRITE_HEIGHT * 2) + 2)
#define HW_SPRITE_BYTES ((ULONG)HW_SPRITE_WORDS * sizeof(UWORD))
#define HW_SPRITE_BANK_BYTES (HW_SPRITE_COUNT * HW_SPRITE_BYTES)
#define HW_SPRITE_BANKS 2
#define BPLCON2_SPRITES_FRONT 0x0024
#define STATIC_SPARKLE_CELL_MAX (GAME_GRID_WIDTH * GAME_GRID_HEIGHT)
#define ICE_MEMF_PUBLIC 0x00000001UL
#define ICE_MEMF_CHIP 0x00000002UL
#define ICE_MEMF_FAST 0x00000004UL
#define ICE_MEMF_CLEAR 0x00010000UL
#define DYNAMIC_DIRTY_SLOTS 64
#define DISPLAY_DMA_BITS (DMAF_SETCLR | DMAF_MASTER | DMAF_RASTER | DMAF_COPPER | DMAF_BLITTER)
#define RENDER_FRAME_SCREEN 0x01u
#define RENDER_FRAME_SPRITES 0x02u
#define HIGH_SCORE_COUNT 5
#define HIGH_SCORE_FILE "icepanic.hi"

#define ICE_INLINE static inline

#if AMIGA_PROFILE
#define PROFILE_COLOR(c) do { CREGS->color[0] = (UWORD)(c); } while (0)
#else
#define PROFILE_COLOR(c)
#endif

ICE_INLINE UWORD amiga_slot_bit(int index) {
    return (UWORD)((UWORD)1u << (UWORD)index);
}

enum {
    RENDER_FP_SHIFT = 8,
    RENDER_FP_ONE = 1 << RENDER_FP_SHIFT,
    RENDER_TILE_FP = GAME_TILE_SIZE * RENDER_FP_ONE,
    AMIGA_STOP_BOUNCE_TICKS = 12
};

struct GfxBase *GfxBase = 0;
struct IntuitionBase *IntuitionBase = 0;
static struct View *g_old_view = 0;
static ULONG g_ui_anim_tick = 0;
static BOOL g_sprite_dma_enabled = FALSE;

typedef struct CopperState {
    UWORD *list;
    ULONG bytes;
    UWORD *bpl_hi[DEPTH];
    UWORD *bpl_lo[DEPTH];
    UWORD *spr_hi[HW_SPRITE_COUNT];
    UWORD *spr_lo[HW_SPRITE_COUNT];
} CopperState;

typedef struct StaticCell {
    UBYTE terrain;
    UBYTE block;
    UBYTE item;
    UBYTE mine_sprite;
} StaticCell;

typedef struct DirtyRect {
    WORD x;
    WORD y;
    WORD w;
    WORD h;
} DirtyRect;

typedef struct ModalCache {
    UBYTE valid;
    UBYTE active;
    UBYTE phase;
    WORD selection;
    ULONG key;
    ULONG layout_key;
    DirtyRect rect;
} ModalCache;

typedef struct DynamicCache {
    UBYTE valid;
    UBYTE next_valid;
    UWORD next_sprite;
    WORD next_x;
    WORD next_y;
    ULONG key;
    ULONG next_key;
    DirtyRect rect;
    DirtyRect next_rect;
} DynamicCache;

typedef struct FrameSync {
    UWORD last_line;
    UBYTE wrapped;
} FrameSync;

typedef struct HudSnapshot {
    ULONG score;
    UWORD round;
    UWORD timer_seconds;
    UWORD lives;
    UWORD mine_stock;
    UWORD mine_capacity;
    UBYTE timer_danger;
    UBYTE timer_flash_phase;
    UBYTE bomb_flash_phase;
    UBYTE special_border_phase;
} HudSnapshot;

typedef struct RendererState {
    ULONG dirty_rows[2][GAME_GRID_HEIGHT];
    ULONG static_dirty_rows[2][GAME_GRID_HEIGHT];
    UBYTE dirty_any[2];
    StaticCell prev_static[2][GAME_GRID_HEIGHT][GAME_GRID_WIDTH];
    UBYTE prev_valid[2];
    UBYTE prev_hud_valid[2];
    HudSnapshot prev_hud[2];
    ModalCache modal[2];
    DynamicCache cached_player[2];
    DynamicCache cached_settle_block[2][GAME_MAX_IMPACT_FX];
    DynamicCache cached_block[2][GAME_MAX_MOVING_BLOCKS];
    DynamicCache cached_enemy[2][GAME_MAX_ENEMIES];
    UBYTE enemy_hw_sprite[2][GAME_MAX_ENEMIES];
    UBYTE cached_settle_live[2];
    UWORD cached_settle_mask[2];
    UWORD cached_block_mask[2];
    UWORD cached_enemy_mask[2];
    UBYTE backdrop_valid;
    ULONG hidden_block_rows[GAME_GRID_HEIGHT];
    UBYTE sparkle_cell_x[STATIC_SPARKLE_CELL_MAX];
    UBYTE sparkle_cell_y[STATIC_SPARKLE_CELL_MAX];
    UWORD sparkle_cell_count;
    UWORD sparkle_cursor;
    UBYTE sparkle_hw_count[2];
    DirtyRect prev_dynamic[2][DYNAMIC_DIRTY_SLOTS];
    WORD prev_dynamic_count[2];
} RendererState;

typedef enum AmigaSfxWave {
    AMIGA_SFX_WAVE_SILENCE = 0,
    AMIGA_SFX_WAVE_SAMPLE = 1
} AmigaSfxWave;

typedef struct AmigaSfxStep {
    UWORD ticks;
    UBYTE volume;
    UBYTE wave;
    UBYTE sample_id;
} AmigaSfxStep;

typedef struct AmigaSfxVoice {
    AmigaSfxStep steps[SFX_MAX_STEPS];
    AmigaSfxStep current;
    UBYTE channel;
    UBYTE head;
    UBYTE count;
    UBYTE active;
    UWORD elapsed;
} AmigaSfxVoice;

typedef struct AmigaSfxState {
    UBYTE *samples;
    AmigaSfxVoice voice[2];
    UBYTE enabled;
} AmigaSfxState;

typedef struct AmigaApp {
    UBYTE *screen[2];
    UBYTE *chip_tiles;
    UBYTE *chip_tile_masks;
    UBYTE *chip_bobs;
    UBYTE *chip_masks;
    UBYTE *chip_backdrop;
    UBYTE *chip_title;
    UBYTE *chip_title_mask;
    UBYTE *hw_sprite_bank[HW_SPRITE_BANKS];
    UWORD hw_enemy_lines[AMIGA_SPR_COUNT][HW_SPRITE_HEIGHT][4];
    UWORD hw_sparkle_lines[HW_SPRITE_HEIGHT][2];
    AmigaSfxState sfx;
    CopperState copper;
    RendererState renderer;
    BOOL wb_was_open;
    BOOL owns_blitter;
} AmigaApp;

static AmigaApp g_app;
static GameState g_game;

static const AmigaSpriteId kTerrainSprites[3] = {
    AMIGA_SPR_FLOOR,
    AMIGA_SPR_WALL,
    AMIGA_SPR_BORDER
};

static const AmigaSpriteId kBlockSprites[5] = {
    AMIGA_SPR_FLOOR,
    AMIGA_SPR_BLOCK_ICE,
    AMIGA_SPR_BLOCK_ICE,
    AMIGA_SPR_BLOCK_ICE,
    AMIGA_SPR_BLOCK_ICE
};

static const AmigaSpriteId kItemSprites[6] = {
    AMIGA_SPR_FLOOR,
    AMIGA_SPR_ITEM_FISH,
    AMIGA_SPR_ITEM_EGG,
    AMIGA_SPR_ITEM_GEM,
    AMIGA_SPR_ITEM_FRUIT,
    AMIGA_SPR_ITEM_MYSTERY
};

typedef struct HiScoreEntry {
    char initials[4];
    uint32_t score;
} HiScoreEntry;

typedef struct HighScoreTable {
    HiScoreEntry entries[HIGH_SCORE_COUNT];
} HighScoreTable;

typedef struct HighScoreEntryState {
    UBYTE active;
    UBYTE confirm_armed;
    WORD insert_index;
    uint32_t pending_score;
    char initials[4];
    WORD cursor;
    UWORD blink_ticks;
} HighScoreEntryState;

typedef struct HighScoreFileImage {
    char magic[8];
    HiScoreEntry entries[HIGH_SCORE_COUNT];
} HighScoreFileImage;

static const HiScoreEntry kOpeningHiScores[HIGH_SCORE_COUNT] = {
    {"ACE", 50000u},
    {"ICE", 35000u},
    {"PAN", 24000u},
    {"FOX", 16000u},
    {"JET", 9000u}
};

static const char kHighScoreMagic[8] = {'I', 'C', 'E', 'H', 'S', '1', 0, 0};
static HighScoreTable g_high_scores;
static HighScoreEntryState g_high_score_entry;

static void sanitize_initials(char out_initials[4], const char *input) {
    int out_idx = 0;
    if (!out_initials) {
        return;
    }
    if (input) {
        int i;
        for (i = 0; input[i] != '\0' && out_idx < 3; ++i) {
            unsigned char ch = (unsigned char)input[i];
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
    int i;
    if (!table) {
        return;
    }
    memset(table, 0, sizeof(*table));
    for (i = 0; i < HIGH_SCORE_COUNT; ++i) {
        table->entries[i] = kOpeningHiScores[i];
    }
}

static void high_score_table_normalize(HighScoreTable *table) {
    int i;
    if (!table) {
        return;
    }
    for (i = 0; i < HIGH_SCORE_COUNT; ++i) {
        sanitize_initials(table->entries[i].initials, table->entries[i].initials);
    }
    for (i = 1; i < HIGH_SCORE_COUNT; ++i) {
        HiScoreEntry key = table->entries[i];
        int j = i - 1;
        while (j >= 0 && key.score > table->entries[j].score) {
            table->entries[j + 1] = table->entries[j];
            --j;
        }
        table->entries[j + 1] = key;
    }
}

static WORD high_score_find_insert_index(const HighScoreTable *table, uint32_t score) {
    WORD i;
    if (!table) {
        return -1;
    }
    for (i = 0; i < HIGH_SCORE_COUNT; ++i) {
        if (score >= table->entries[i].score) {
            return i;
        }
    }
    return -1;
}

static void high_score_insert(HighScoreTable *table, WORD index, const char initials[4], uint32_t score) {
    WORD i;
    if (!table || index < 0 || index >= HIGH_SCORE_COUNT) {
        return;
    }
    for (i = HIGH_SCORE_COUNT - 1; i > index; --i) {
        table->entries[i] = table->entries[i - 1];
    }
    sanitize_initials(table->entries[index].initials, initials);
    table->entries[index].score = score;
    high_score_table_normalize(table);
}

static BOOL load_high_score_table(HighScoreTable *table) {
    BPTR file;
    LONG got;
    HighScoreFileImage image;
    if (!table) {
        return FALSE;
    }
    high_score_table_set_defaults(table);
    file = Open((STRPTR)HIGH_SCORE_FILE, MODE_OLDFILE);
    if (!file) {
        return FALSE;
    }
    got = Read(file, &image, (LONG)sizeof(image));
    Close(file);
    if (got != (LONG)sizeof(image)) {
        return FALSE;
    }
    if (memcmp(image.magic, kHighScoreMagic, sizeof(kHighScoreMagic)) != 0) {
        return FALSE;
    }
    memcpy(table->entries, image.entries, sizeof(table->entries));
    high_score_table_normalize(table);
    return TRUE;
}

static BOOL save_high_score_table(const HighScoreTable *table) {
    BPTR file;
    LONG wrote;
    HighScoreFileImage image;
    if (!table) {
        return FALSE;
    }
    memset(&image, 0, sizeof(image));
    memcpy(image.magic, kHighScoreMagic, sizeof(kHighScoreMagic));
    memcpy(image.entries, table->entries, sizeof(image.entries));
    file = Open((STRPTR)HIGH_SCORE_FILE, MODE_NEWFILE);
    if (!file) {
        return FALSE;
    }
    wrote = Write(file, &image, (LONG)sizeof(image));
    Close(file);
    return wrote == (LONG)sizeof(image);
}

static char cycle_initial_letter(char letter, int delta) {
    int value;
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

static void begin_high_score_entry(HighScoreEntryState *entry, WORD insert_index, uint32_t score, const HighScoreTable *table) {
    if (!entry) {
        return;
    }
    memset(entry, 0, sizeof(*entry));
    entry->active = 1;
    entry->insert_index = insert_index;
    entry->pending_score = score;
    entry->cursor = 0;
    if (table && insert_index >= 0 && insert_index < HIGH_SCORE_COUNT) {
        sanitize_initials(entry->initials, table->entries[insert_index].initials);
    } else {
        sanitize_initials(entry->initials, "AAA");
    }
}

static BOOL update_high_score_entry(HighScoreEntryState *entry, const InputState *in) {
    if (!entry || !entry->active || !in) {
        return FALSE;
    }
    entry->blink_ticks = (UWORD)((entry->blink_ticks + 1u) % (GAME_FIXED_TICK_HZ * 4u));
    switch (in->newest_press) {
        case DIR_LEFT:
            entry->cursor = (WORD)((entry->cursor + 2) % 3);
            break;
        case DIR_RIGHT:
            entry->cursor = (WORD)((entry->cursor + 1) % 3);
            break;
        case DIR_UP:
            entry->initials[entry->cursor] = cycle_initial_letter(entry->initials[entry->cursor], 1);
            break;
        case DIR_DOWN:
            entry->initials[entry->cursor] = cycle_initial_letter(entry->initials[entry->cursor], -1);
            break;
        default:
            break;
    }
    if (!entry->confirm_armed) {
        if (!in->fire_pressed && !in->fire_released && !in->start) {
            entry->confirm_armed = 1;
        }
    } else if (in->fire_released || in->start) {
        entry->active = 0;
        return TRUE;
    }
    return FALSE;
}

static const UBYTE kDigit3x5[10][5] = {
    {0x7, 0x5, 0x5, 0x5, 0x7},
    {0x2, 0x6, 0x2, 0x2, 0x7},
    {0x7, 0x1, 0x7, 0x4, 0x7},
    {0x7, 0x1, 0x7, 0x1, 0x7},
    {0x5, 0x5, 0x7, 0x1, 0x1},
    {0x7, 0x4, 0x7, 0x1, 0x7},
    {0x7, 0x4, 0x7, 0x5, 0x7},
    {0x7, 0x1, 0x1, 0x1, 0x1},
    {0x7, 0x5, 0x7, 0x5, 0x7},
    {0x7, 0x5, 0x7, 0x1, 0x7}
};

static UBYTE g_font_fill_rows[128][5];
static UBYTE g_font_outline_rows[128][7];
static UBYTE g_font_ready = 0;

ICE_INLINE int clamp_int(int value, int lo, int hi) {
    if (value < lo) {
        return lo;
    }
    if (value > hi) {
        return hi;
    }
    return value;
}

ICE_INLINE int dir_dx(Direction d) {
    if (d == DIR_LEFT) {
        return -1;
    }
    if (d == DIR_RIGHT) {
        return 1;
    }
    return 0;
}

ICE_INLINE int dir_dy(Direction d) {
    if (d == DIR_UP) {
        return -1;
    }
    if (d == DIR_DOWN) {
        return 1;
    }
    return 0;
}

ICE_INLINE bool cells_equal(const StaticCell *a, const StaticCell *b) {
    return a->terrain == b->terrain &&
           a->block == b->block &&
           a->item == b->item &&
           a->mine_sprite == b->mine_sprite;
}

ICE_INLINE bool item_should_render(int timer_ticks) {
    if (timer_ticks <= 0) {
        return false;
    }
    if (timer_ticks > 2 * GAME_FIXED_TICK_HZ) {
        return true;
    }
    if (timer_ticks > GAME_FIXED_TICK_HZ) {
        return ((timer_ticks / 8) & 1) == 0;
    }
    return ((timer_ticks / 4) & 1) == 0;
}

static AmigaSpriteId mine_sprite_for_fuse_ticks(int fuse_ticks, int phase) {
#if AMIGA_FAST_RENDER
    if (fuse_ticks <= GAME_FIXED_TICK_HZ) {
        return (fuse_ticks & 1) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_2;
    }
    if (fuse_ticks <= 2 * GAME_FIXED_TICK_HZ) {
        return ((fuse_ticks & 2) == 0) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_0;
    }
    return ((fuse_ticks & 7) == 0) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_0;
#else
    if (fuse_ticks <= GAME_FIXED_TICK_HZ) {
        return ((phase & 1) == 0) ? AMIGA_SPR_MINE_2 : AMIGA_SPR_MINE_1;
    }
    if (fuse_ticks <= 2 * GAME_FIXED_TICK_HZ) {
        return ((phase & 2) == 0) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_0;
    }
    return ((phase & 7) == 0) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_0;
#endif
}

static AmigaSpriteId player_sprite_for_direction(Direction d, bool alt) {
    switch (d) {
        case DIR_UP:
            return alt ? AMIGA_SPR_PLAYER_UP_ALT : AMIGA_SPR_PLAYER_UP;
        case DIR_DOWN:
            return alt ? AMIGA_SPR_PLAYER_DOWN_ALT : AMIGA_SPR_PLAYER_DOWN;
        case DIR_LEFT:
            return alt ? AMIGA_SPR_PLAYER_LEFT_ALT : AMIGA_SPR_PLAYER_LEFT;
        case DIR_RIGHT:
        default:
            return alt ? AMIGA_SPR_PLAYER_RIGHT_ALT : AMIGA_SPR_PLAYER_RIGHT;
    }
}

static AmigaSpriteId enemy_sprite_for_state(const Enemy *enemy) {
    const int phase = (enemy->pixel_fp_x + enemy->pixel_fp_y) >> 9;
    const bool alt = ((phase & 1) != 0);
    if (enemy->type == ENEMY_TYPE_WANDERER) {
        return alt ? AMIGA_SPR_ENEMY_B_ALT : AMIGA_SPR_ENEMY_B;
    }
    return alt ? AMIGA_SPR_ENEMY_A_ALT : AMIGA_SPR_ENEMY_A;
}

static AmigaSpriteId death_sprite_for_fx(const ImpactFx *fx) {
    const int age = fx->base_ttl_ticks - fx->ttl_ticks;
    const bool late = (age >= (fx->base_ttl_ticks / 2));
    if (fx->style == IMPACT_FX_STYLE_ENEMY_DEATH_B) {
        return late ? AMIGA_SPR_ENEMY_B_DEATH_1 : AMIGA_SPR_ENEMY_B_DEATH_0;
    }
    return late ? AMIGA_SPR_ENEMY_A_DEATH_1 : AMIGA_SPR_ENEMY_A_DEATH_0;
}

static const char *perk_label(PerkType perk) {
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

static const char *perk_effect_label(PerkType perk) {
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
            return "FIRE DROP MINE";
        default:
            return "NO EFFECT";
    }
}

static UBYTE perk_color(PerkType perk) {
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

static AmigaSpriteId perk_sprite(PerkType perk) {
    switch (perk) {
        case PERK_LIFE_BOOST:
            return AMIGA_SPR_HUD_LIFE;
        case PERK_TIME_BOOST:
            return AMIGA_SPR_ITEM_EGG;
        case PERK_SCORE_BOOST:
            return AMIGA_SPR_ITEM_GEM;
        case PERK_ENEMY_SLOW:
            return AMIGA_SPR_ITEM_FISH;
        case PERK_MINES:
            return AMIGA_SPR_MINE_1;
        default:
            return AMIGA_SPR_ITEM_MYSTERY;
    }
}

static const char *meta_label(MetaUpgradeType upgrade) {
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

static const char *meta_effect_label(MetaUpgradeType upgrade) {
    switch (upgrade) {
        case META_UPGRADE_LIFE_PACK:
            return "NEW RUN 1 LIFE";
        case META_UPGRADE_TIME_PACK:
            return "NEW RUN TIME";
        case META_UPGRADE_SCORE_PACK:
            return "NEW RUN SCORE";
        case META_UPGRADE_SLOW_PACK:
            return "NEW RUN SLOW";
        default:
            return "NO EFFECT";
    }
}

static UBYTE meta_color(MetaUpgradeType upgrade) {
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

static AmigaSpriteId meta_sprite(MetaUpgradeType upgrade) {
    switch (upgrade) {
        case META_UPGRADE_LIFE_PACK:
            return AMIGA_SPR_HUD_LIFE;
        case META_UPGRADE_TIME_PACK:
            return AMIGA_SPR_ITEM_EGG;
        case META_UPGRADE_SCORE_PACK:
            return AMIGA_SPR_ITEM_GEM;
        case META_UPGRADE_SLOW_PACK:
            return AMIGA_SPR_ITEM_FISH;
        default:
            return AMIGA_SPR_ITEM_MYSTERY;
    }
}

static UBYTE stage_modifier_color(StageModifierType modifier) {
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

ICE_INLINE void wait_blitter(void) {
    volatile UWORD *dmaconr = (volatile UWORD *)0xDFF002;
    (void)*dmaconr;
    while ((*dmaconr & DMAF_BLTDONE) != 0) {
    }
}

ICE_INLINE UWORD read_raster_line(void) {
    UWORD vpos_hi;
    UWORD vhpos_a;
    UWORD vhpos_b;
    do {
        vhpos_a = CREGS->vhposr;
        vpos_hi = CREGS->vposr;
        vhpos_b = CREGS->vhposr;
    } while ((vhpos_a ^ vhpos_b) & 0xFF00u);
    return (UWORD)(((vpos_hi & 0x0001u) << 8) | ((vhpos_b >> 8) & 0x00FFu));
}

static void wait_frame_boundary_hw(void) {
    while (read_raster_line() < 250u) {
    }
    while (read_raster_line() >= 250u) {
    }
}

ICE_INLINE BOOL title_hw_sparkles_active(const RenderState *rs) {
#if AMIGA_USE_TITLE_HW_SPARKLES
    return (rs->phase == GAME_PHASE_ROUND_INTRO && rs->start_title_pending) ? TRUE : FALSE;
#else
    (void)rs;
    return FALSE;
#endif
}

static BOOL hardware_sprites_active_for_state(const RenderState *rs) {
    BOOL active = FALSE;
#if AMIGA_USE_HW_SPRITES
    if (rs->phase == GAME_PHASE_PLAYING) {
        active = TRUE;
    }
#endif
#if AMIGA_USE_TITLE_HW_SPARKLES
    if (title_hw_sparkles_active(rs)) {
        active = TRUE;
    }
#endif
#if AMIGA_USE_GAMEPLAY_HW_SPARKLES
    if (rs->phase == GAME_PHASE_PLAYING) {
        active = TRUE;
    }
#endif
    return active;
}

static void set_sprite_dma_for_state(const RenderState *rs) {
#if AMIGA_HW_SPRITES_ENABLED
    BOOL enable = hardware_sprites_active_for_state(rs);
    if (g_sprite_dma_enabled == enable) {
        return;
    }
    CREGS->dmacon = enable ? (DMAF_SETCLR | DMAF_SPRITE) : DMAF_SPRITE;
    g_sprite_dma_enabled = enable;
#else
    (void)rs;
#endif
}

ICE_INLINE void frame_sync_begin(FrameSync *sync) {
    sync->last_line = read_raster_line();
    sync->wrapped = 0;
}

ICE_INLINE void frame_sync_sample(FrameSync *sync) {
    UWORD line = read_raster_line();
    if ((UWORD)(line + 8u) < sync->last_line) {
        sync->wrapped = 1;
    }
    sync->last_line = line;
}

static void frame_sync_finish(FrameSync *sync) {
    frame_sync_sample(sync);
    if (!sync->wrapped) {
        wait_frame_boundary_hw();
    }
}

ICE_INLINE const UBYTE *bob_plane_ptr(const AmigaApp *app, AmigaSpriteId sprite, int plane) {
    return app->chip_bobs +
           ((ULONG)sprite * AMIGA_ASSET_BOB_BYTES) +
           ((ULONG)plane * AMIGA_ASSET_ROW_BYTES_WIDE);
}

ICE_INLINE const UBYTE *mask_ptr(const AmigaApp *app, AmigaSpriteId sprite) {
    return app->chip_masks + ((ULONG)sprite * AMIGA_ASSET_MASK_BYTES);
}

ICE_INLINE const UBYTE *tile_ptr(const AmigaApp *app, AmigaSpriteId sprite) {
    return app->chip_tiles + ((ULONG)sprite * AMIGA_ASSET_TILE_BYTES);
}

ICE_INLINE UBYTE *screen_plane_ptr(UBYTE *screen, int x, int y, int plane) {
    return screen + ((ULONG)y * ROW_STRIDE_BYTES) + ((ULONG)plane * ROW_BYTES) + ((ULONG)(x >> 4) * 2);
}

static APTR alloc_fast_or_public(ULONG bytes) {
    APTR p = AllocMem(bytes, ICE_MEMF_FAST | ICE_MEMF_CLEAR);
    if (!p) {
        p = AllocMem(bytes, ICE_MEMF_PUBLIC | ICE_MEMF_CLEAR);
    }
    return p;
}

static void blit_opaque_tile(const AmigaApp *app, UBYTE *screen, AmigaSpriteId sprite, int x, int y) {
    const UBYTE *src;
    UBYTE *dst;
    x = clamp_int(x, 0, SCREEN_W - GAME_TILE_SIZE);
    y = clamp_int(y, 0, SCREEN_H - GAME_TILE_SIZE);
    x &= ~15;
    src = tile_ptr(app, sprite);
    dst = screen + ((ULONG)y * ROW_STRIDE_BYTES) + ((ULONG)(x >> 4) * 2);

    wait_blitter();
    CREGS->bltcon0 = BLT_USEB | BLT_USED | BLT_MINTERM_COPY_B;
    CREGS->bltcon1 = 0;
    CREGS->bltafwm = 0xFFFF;
    CREGS->bltalwm = 0xFFFF;
    CREGS->bltbpt = (APTR)src;
    CREGS->bltdpt = (APTR)dst;
    CREGS->bltbmod = 0;
    CREGS->bltdmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_TILE;
    CREGS->bltsize = (UWORD)(((GAME_TILE_SIZE * DEPTH) << 6) | AMIGA_ASSET_ROW_WORDS_TILE);
}

static void blit_cookie_bob(const AmigaApp *app, UBYTE *screen, AmigaSpriteId sprite, int x, int y) {
    int shift;
    const UBYTE *mask;
    const UBYTE *src;
    UBYTE *dst;
    x = clamp_int(x, 0, SCREEN_W - (GAME_TILE_SIZE * 2));
    y = clamp_int(y, 0, SCREEN_H - GAME_TILE_SIZE);
    shift = x & 15;
    dst = screen + ((ULONG)y * ROW_STRIDE_BYTES) + ((ULONG)(x >> 4) * 2);

    wait_blitter();
    if (shift == 0) {
        mask = app->chip_tile_masks + ((ULONG)sprite * AMIGA_ASSET_TILE_MASK_BYTES);
        src = app->chip_tiles + ((ULONG)sprite * AMIGA_ASSET_TILE_BYTES);
        CREGS->bltcon0 = BLT_USEA | BLT_USEB | BLT_USEC | BLT_USED | BLT_MINTERM_COOKIE;
        CREGS->bltcon1 = 0;
        CREGS->bltafwm = 0xFFFF;
        CREGS->bltalwm = 0xFFFF;
        CREGS->bltapt = (APTR)mask;
        CREGS->bltbpt = (APTR)src;
        CREGS->bltcpt = (APTR)dst;
        CREGS->bltdpt = (APTR)dst;
        CREGS->bltamod = 0;
        CREGS->bltbmod = 0;
        CREGS->bltcmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_TILE;
        CREGS->bltdmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_TILE;
        CREGS->bltsize = (UWORD)(((GAME_TILE_SIZE * DEPTH) << 6) | AMIGA_ASSET_ROW_WORDS_TILE);
        return;
    }

    mask = app->chip_masks + ((ULONG)sprite * AMIGA_ASSET_MASK_BYTES);
    src = app->chip_bobs + ((ULONG)sprite * AMIGA_ASSET_BOB_BYTES);
    CREGS->bltcon0 = (UWORD)((shift << 12) | BLT_USEA | BLT_USEB | BLT_USEC | BLT_USED | BLT_MINTERM_COOKIE);
    CREGS->bltcon1 = (UWORD)(shift << 12);
    CREGS->bltafwm = 0xFFFF;
    CREGS->bltalwm = 0xFFFF;
    CREGS->bltapt = (APTR)mask;
    CREGS->bltbpt = (APTR)src;
    CREGS->bltcpt = (APTR)dst;
    CREGS->bltdpt = (APTR)dst;
    CREGS->bltamod = 0;
    CREGS->bltbmod = 0;
    CREGS->bltcmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_WIDE;
    CREGS->bltdmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_WIDE;
    CREGS->bltsize = (UWORD)(((GAME_TILE_SIZE * DEPTH) << 6) | AMIGA_ASSET_ROW_WORDS_WIDE);
}

static void blit_title_overlay(const AmigaApp *app, UBYTE *screen, int x, int y) {
    int shift;
    UBYTE *dst;
    if (!app->chip_title || !app->chip_title_mask) {
        return;
    }
    x = clamp_int(x, 0, SCREEN_W - (int)(AMIGA_TITLE_OVERLAY_ROW_WORDS * 16));
    y = clamp_int(y, 0, SCREEN_H - AMIGA_TITLE_OVERLAY_HEIGHT);
    shift = x & 15;
    dst = screen + ((ULONG)y * ROW_STRIDE_BYTES) + ((ULONG)(x >> 4) * 2);

    wait_blitter();
    CREGS->bltcon0 = (UWORD)((shift << 12) | BLT_USEA | BLT_USEB | BLT_USEC | BLT_USED | BLT_MINTERM_COOKIE);
    CREGS->bltcon1 = (UWORD)(shift << 12);
    CREGS->bltafwm = 0xFFFF;
    CREGS->bltalwm = 0xFFFF;
    CREGS->bltapt = (APTR)app->chip_title_mask;
    CREGS->bltbpt = (APTR)app->chip_title;
    CREGS->bltcpt = (APTR)dst;
    CREGS->bltdpt = (APTR)dst;
    CREGS->bltamod = 0;
    CREGS->bltbmod = 0;
    CREGS->bltcmod = ROW_BYTES - AMIGA_TITLE_OVERLAY_ROW_BYTES;
    CREGS->bltdmod = ROW_BYTES - AMIGA_TITLE_OVERLAY_ROW_BYTES;
    CREGS->bltsize = (UWORD)(((AMIGA_TITLE_OVERLAY_HEIGHT * DEPTH) << 6) | AMIGA_TITLE_OVERLAY_ROW_WORDS);
}

static void blit_screen_rect(UBYTE *dst_screen, const UBYTE *src_screen, int x, int y, int w, int h) {
    int x0;
    int x1;
    int y0;
    int y1;
    int row_words;
    int row_bytes;
    const UBYTE *src;
    UBYTE *dst;

    if (!dst_screen || !src_screen || w <= 0 || h <= 0) {
        return;
    }

    x0 = clamp_int(x, 0, SCREEN_W);
    x1 = clamp_int(x + w, 0, SCREEN_W);
    y0 = clamp_int(y, 0, SCREEN_H);
    y1 = clamp_int(y + h, 0, SCREEN_H);
    if (x1 <= x0 || y1 <= y0) {
        return;
    }

    x0 &= ~15;
    x1 = (x1 + 15) & ~15;
    if (x1 > SCREEN_W) {
        x1 = SCREEN_W;
    }
    row_words = (x1 - x0) >> 4;
    if (row_words <= 0) {
        return;
    }
    row_bytes = row_words << 1;
    src = src_screen + ((ULONG)y0 * ROW_STRIDE_BYTES) + (ULONG)(x0 >> 3);
    dst = dst_screen + ((ULONG)y0 * ROW_STRIDE_BYTES) + (ULONG)(x0 >> 3);

    wait_blitter();
    CREGS->bltcon0 = BLT_USEB | BLT_USED | BLT_MINTERM_COPY_B;
    CREGS->bltcon1 = 0;
    CREGS->bltafwm = 0xFFFF;
    CREGS->bltalwm = 0xFFFF;
    CREGS->bltbpt = (APTR)src;
    CREGS->bltdpt = (APTR)dst;
    CREGS->bltbmod = (WORD)(ROW_BYTES - row_bytes);
    CREGS->bltdmod = (WORD)(ROW_BYTES - row_bytes);
    CREGS->bltsize = (UWORD)((((y1 - y0) * DEPTH) << 6) | row_words);
}

ICE_INLINE UWORD read_be_word(const UBYTE *p) {
    return (UWORD)(((UWORD)p[0] << 8) | p[1]);
}

static bool enemy_sprite_can_use_hardware(AmigaSpriteId sprite) {
    return sprite == AMIGA_SPR_ENEMY_A || sprite == AMIGA_SPR_ENEMY_A_ALT;
}

static void hw_enemy_layer_colors(AmigaSpriteId sprite, UBYTE source_color, UBYTE *main_color, UBYTE *detail_color) {
    *main_color = 0;
    *detail_color = 0;
    switch (sprite) {
        case AMIGA_SPR_ENEMY_A:
        case AMIGA_SPR_ENEMY_A_ALT:
            if (source_color == 2) {
                *detail_color = 1;
            } else if (source_color == 17) {
                *main_color = 1;
            } else if (source_color == 18) {
                *main_color = 2;
            } else if (source_color == 19) {
                *main_color = 3;
            } else if (source_color == 31) {
                *detail_color = 3;
            }
            break;
        default:
            break;
    }
}

static void build_hw_enemy_sprite_lines(AmigaApp *app) {
    int sprite;
    int y;
    int x;
    int plane;

    memset(app->hw_enemy_lines, 0, sizeof(app->hw_enemy_lines));
    for (sprite = 0; sprite < AMIGA_SPR_COUNT; ++sprite) {
        for (y = 0; y < HW_SPRITE_HEIGHT; ++y) {
            UWORD plane_words[DEPTH];
            for (plane = 0; plane < DEPTH; ++plane) {
                const UBYTE *src = ((const UBYTE *)g_amiga_tile_data) +
                                   ((ULONG)sprite * AMIGA_ASSET_TILE_BYTES) +
                                   ((ULONG)y * DEPTH * AMIGA_ASSET_ROW_BYTES_TILE) +
                                   ((ULONG)plane * AMIGA_ASSET_ROW_BYTES_TILE);
                plane_words[plane] = read_be_word(src);
            }
            for (x = 0; x < GAME_TILE_SIZE; ++x) {
                UBYTE source_color = 0;
                UBYTE main_color;
                UBYTE detail_color;
                UWORD bit = (UWORD)(1u << (15 - x));
                int bit_index = 15 - x;
                for (plane = 0; plane < DEPTH; ++plane) {
                    if ((plane_words[plane] & bit) != 0) {
                        source_color |= (UBYTE)(1u << plane);
                    }
                }
                hw_enemy_layer_colors((AmigaSpriteId)sprite, source_color, &main_color, &detail_color);
                if ((main_color & 2u) != 0) app->hw_enemy_lines[sprite][y][0] |= (UWORD)(1u << bit_index);
                if ((main_color & 1u) != 0) app->hw_enemy_lines[sprite][y][1] |= (UWORD)(1u << bit_index);
                if ((detail_color & 2u) != 0) app->hw_enemy_lines[sprite][y][2] |= (UWORD)(1u << bit_index);
                if ((detail_color & 1u) != 0) app->hw_enemy_lines[sprite][y][3] |= (UWORD)(1u << bit_index);
            }
        }
    }
}

static UBYTE hw_sparkle_color(UBYTE source_color) {
    if (source_color == 31) {
        return 3;
    }
    if (source_color == 30 || source_color == 21) {
        return 2;
    }
    if (source_color != 0) {
        return 1;
    }
    return 0;
}

static void build_hw_sparkle_sprite_lines(AmigaApp *app) {
    int y;
    int x;
    int plane;

    memset(app->hw_sparkle_lines, 0, sizeof(app->hw_sparkle_lines));
    for (y = 0; y < HW_SPRITE_HEIGHT; ++y) {
        UWORD plane_words[DEPTH];
        for (plane = 0; plane < DEPTH; ++plane) {
            const UBYTE *src = ((const UBYTE *)g_amiga_tile_data) +
                               ((ULONG)AMIGA_SPR_EFFECT_SPARK * AMIGA_ASSET_TILE_BYTES) +
                               ((ULONG)y * DEPTH * AMIGA_ASSET_ROW_BYTES_TILE) +
                               ((ULONG)plane * AMIGA_ASSET_ROW_BYTES_TILE);
            plane_words[plane] = read_be_word(src);
        }
        for (x = 0; x < GAME_TILE_SIZE; ++x) {
            UBYTE source_color = 0;
            UBYTE sprite_color;
            UWORD bit = (UWORD)(1u << (15 - x));
            int bit_index = 15 - x;
            for (plane = 0; plane < DEPTH; ++plane) {
                if ((plane_words[plane] & bit) != 0) {
                    source_color |= (UBYTE)(1u << plane);
                }
            }
            sprite_color = hw_sparkle_color(source_color);
            if ((sprite_color & 2u) != 0) app->hw_sparkle_lines[y][0] |= (UWORD)(1u << bit_index);
            if ((sprite_color & 1u) != 0) app->hw_sparkle_lines[y][1] |= (UWORD)(1u << bit_index);
        }
    }
}

static void clear_hw_sprite_slot(UBYTE *bank, int slot) {
    UWORD *dst = (UWORD *)(bank + ((ULONG)slot * HW_SPRITE_BYTES));
    dst[0] = 0;
    dst[1] = 0;
    dst[HW_SPRITE_WORDS - 2] = 0;
    dst[HW_SPRITE_WORDS - 1] = 0;
}

static void clear_hw_sprite_bank(UBYTE *bank) {
    int i;
    for (i = 0; i < HW_SPRITE_COUNT; ++i) {
        clear_hw_sprite_slot(bank, i);
    }
}

static void write_hw_sprite_control(UWORD *dst, int x, int y, bool attached) {
    int hwx = clamp_int(x, 0, SCREEN_W - GAME_TILE_SIZE) + 0x80;
    int vstart = clamp_int(y, 0, SCREEN_H - HW_SPRITE_HEIGHT) + 0x2C;
    int vstop = vstart + HW_SPRITE_HEIGHT;
    dst[0] = (UWORD)(((vstart & 0xFF) << 8) | ((hwx >> 1) & 0xFF));
    dst[1] = (UWORD)(((vstop & 0xFF) << 8) |
                     ((vstart & 0x100) ? 0x0004 : 0) |
                     ((vstop & 0x100) ? 0x0002 : 0) |
                     (hwx & 1) |
                     (attached ? 0x0080 : 0));
}

static void write_hw_enemy_sprite_layers(const AmigaApp *app, int buf, int layer_index, AmigaSpriteId sprite, int x, int y) {
    UBYTE *bank = app->hw_sprite_bank[buf];
    int main_slot = layer_index;
    int detail_slot = 6 + layer_index;
    UWORD *main_layer = (UWORD *)(bank + ((ULONG)main_slot * HW_SPRITE_BYTES));
    UWORD *detail_layer = (UWORD *)(bank + ((ULONG)detail_slot * HW_SPRITE_BYTES));
    int row;

    write_hw_sprite_control(main_layer, x, y, false);
    write_hw_sprite_control(detail_layer, x, y, false);
    for (row = 0; row < HW_SPRITE_HEIGHT; ++row) {
        main_layer[2 + row * 2] = app->hw_enemy_lines[sprite][row][0];
        main_layer[3 + row * 2] = app->hw_enemy_lines[sprite][row][1];
        detail_layer[2 + row * 2] = app->hw_enemy_lines[sprite][row][2];
        detail_layer[3 + row * 2] = app->hw_enemy_lines[sprite][row][3];
    }
    main_layer[HW_SPRITE_WORDS - 2] = 0;
    main_layer[HW_SPRITE_WORDS - 1] = 0;
    detail_layer[HW_SPRITE_WORDS - 2] = 0;
    detail_layer[HW_SPRITE_WORDS - 1] = 0;
}

static void write_hw_sparkle_sprite(const AmigaApp *app, int buf, int slot, int x, int y) {
    UBYTE *bank = app->hw_sprite_bank[buf];
    UWORD *dst = (UWORD *)(bank + ((ULONG)slot * HW_SPRITE_BYTES));
    int row;

    write_hw_sprite_control(dst, x, y, false);
    for (row = 0; row < HW_SPRITE_HEIGHT; ++row) {
        dst[2 + row * 2] = app->hw_sparkle_lines[row][0];
        dst[3 + row * 2] = app->hw_sparkle_lines[row][1];
    }
    dst[HW_SPRITE_WORDS - 2] = 0;
    dst[HW_SPRITE_WORDS - 1] = 0;
}

static UWORD sfx_dma_bit(UBYTE channel) {
    return (UWORD)(DMAF_AUD0 << channel);
}

static UBYTE *sfx_sample_ptr(const AmigaSfxState *sfx, UBYTE sample_id) {
    if (!sfx || !sfx->samples || sample_id == AMIGA_SFX_SAMPLE_NONE || sample_id >= AMIGA_SFX_SAMPLE_COUNT) {
        return 0;
    }
    return sfx->samples + g_amiga_sfx_offsets[sample_id];
}

static UWORD sfx_sample_len_words(UBYTE sample_id) {
    if (sample_id == AMIGA_SFX_SAMPLE_NONE || sample_id >= AMIGA_SFX_SAMPLE_COUNT) {
        return 0;
    }
    return g_amiga_sfx_lengths_words[sample_id];
}

static void sfx_disable_voice(UBYTE channel) {
    CREGS->aud[channel].ac_vol = 0;
    CREGS->dmacon = sfx_dma_bit(channel);
}

static void sfx_voice_clear(AmigaSfxVoice *voice) {
    UBYTE channel = voice->channel;
    memset(voice, 0, sizeof(*voice));
    voice->channel = channel;
    sfx_disable_voice(channel);
}

static void sfx_interrupt(AmigaSfxState *sfx) {
    if (!sfx || !sfx->enabled) {
        return;
    }
    sfx_voice_clear(&sfx->voice[0]);
    sfx_voice_clear(&sfx->voice[1]);
}

static BOOL sfx_voice_push(AmigaSfxVoice *voice, const AmigaSfxStep *step) {
    UBYTE tail;
    if (!step || step->ticks == 0) {
        return FALSE;
    }
    if (voice->count >= SFX_MAX_STEPS) {
        return FALSE;
    }
    tail = (UBYTE)(voice->head + voice->count);
    if (tail >= SFX_MAX_STEPS) {
        tail = (UBYTE)(tail - SFX_MAX_STEPS);
    }
    voice->steps[tail] = *step;
    ++voice->count;
    return TRUE;
}

static BOOL sfx_queue_sample_voice(AmigaSfxVoice *voice, UBYTE sample_id, UBYTE volume) {
    AmigaSfxStep step;
    if (!voice || sample_id == AMIGA_SFX_SAMPLE_NONE || sample_id >= AMIGA_SFX_SAMPLE_COUNT) {
        return FALSE;
    }
    memset(&step, 0, sizeof(step));
    step.wave = AMIGA_SFX_WAVE_SAMPLE;
    step.sample_id = sample_id;
    step.ticks = g_amiga_sfx_ticks[sample_id];
    step.volume = volume ? volume : 64;
    return sfx_voice_push(voice, &step);
}

static void sfx_queue_sample(AmigaSfxState *sfx, UBYTE sample_id, UBYTE channel, UBYTE volume) {
    if (!sfx || !sfx->enabled || sample_id == AMIGA_SFX_SAMPLE_NONE || sample_id >= AMIGA_SFX_SAMPLE_COUNT) {
        return;
    }
    if (channel > 1) {
        channel = 0;
    }
    (void)sfx_queue_sample_voice(&sfx->voice[channel], sample_id, volume);
}

static void sfx_queue_slot_chime(AmigaSfxState *sfx) {
    sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_SLOT_CHIME, 0, 64);
}

static void sfx_queue_reward_jingle(AmigaSfxState *sfx) {
    sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_REWARD_JINGLE, 0, 64);
}

static void sfx_queue_level_up_fanfare(AmigaSfxState *sfx) {
    sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_LEVEL_UP_FANFARE, 0, 64);
}

static void sfx_queue_death_jingle(AmigaSfxState *sfx) {
    sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_DEATH_JINGLE, 0, 64);
}

static void sfx_queue_game_over_jingle(AmigaSfxState *sfx) {
    sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_GAME_OVER_JINGLE, 0, 64);
}

static void sfx_queue_life_loss_fanfare(AmigaSfxState *sfx) {
    sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_LIFE_LOSS_FANFARE, 0, 64);
}

static void sfx_queue_combo_jingle(AmigaSfxState *sfx) {
    sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_COMBO_JINGLE, 0, 64);
}

static void sfx_queue_block_push_swish(AmigaSfxState *sfx) {
    sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_BLOCK_PUSH_SWISH, 1, 58);
}

static void sfx_queue_title_start_jingle(AmigaSfxState *sfx) {
    sfx_interrupt(sfx);
    sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_TITLE_START_JINGLE, 0, 64);
}

static void sfx_queue_title_confirm_chirp(AmigaSfxState *sfx) {
    sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_TITLE_CONFIRM_CHIRP, 1, 56);
}

static void sfx_queue_new_high_score_fanfare(AmigaSfxState *sfx) {
    sfx_interrupt(sfx);
    sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_NEW_HIGH_SCORE_FANFARE, 0, 64);
}

static void sfx_dispatch_events(AmigaSfxState *sfx, uint32_t event_flags) {
    if (!sfx || !sfx->enabled || event_flags == 0) {
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
        sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_CRUSH, 1, 64);
    }
    if ((event_flags & GAME_EVENT_ITEM_COLLECT) != 0u) {
        sfx_queue_slot_chime(sfx);
    }
    if ((event_flags & GAME_EVENT_TIMER_DANGER_PULSE) != 0u) {
        sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_TIMER_DANGER, 0, 56);
    }
    if ((event_flags & GAME_EVENT_ROUND_START) != 0u) {
        sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_ROUND_START, 0, 58);
    }
    if ((event_flags & GAME_EVENT_BLOCK_PUSH) != 0u) {
        sfx_queue_block_push_swish(sfx);
    }
    if ((event_flags & GAME_EVENT_MINE_PLACED) != 0u) {
        sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_MINE_PLACED, 1, 58);
        event_flags &= (uint32_t)~GAME_EVENT_BLOCK_IMPACT;
    }
    if ((event_flags & GAME_EVENT_MINE_EXPLODED) != 0u) {
        sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_MINE_EXPLODED, 1, 64);
        event_flags &= (uint32_t)~GAME_EVENT_BLOCK_IMPACT;
    }
    if ((event_flags & GAME_EVENT_BLOCK_IMPACT) != 0u) {
        sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_BLOCK_IMPACT, 1, 58);
    }
    if ((event_flags & GAME_EVENT_META_BANKED) != 0u) {
        sfx_queue_reward_jingle(sfx);
    }
    if ((event_flags & GAME_EVENT_META_SPENT) != 0u) {
        sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_META_SPENT, 0, 58);
    }
    if ((event_flags & GAME_EVENT_STAGE_MODIFIER) != 0u) {
        sfx_queue_sample(sfx, AMIGA_SFX_SAMPLE_STAGE_MODIFIER, 0, 58);
    }
}

static void sfx_start_voice_step(AmigaSfxState *sfx, AmigaSfxVoice *voice) {
    UBYTE channel;
    UBYTE *sample;
    UWORD sample_words;
    if (!sfx || !voice || voice->count == 0) {
        if (voice) {
            sfx_disable_voice(voice->channel);
        }
        return;
    }

    voice->current = voice->steps[voice->head];
    ++voice->head;
    if (voice->head >= SFX_MAX_STEPS) {
        voice->head = 0;
    }
    --voice->count;
    voice->elapsed = 0;
    voice->active = 1;
    channel = voice->channel;

    if (voice->current.wave == AMIGA_SFX_WAVE_SILENCE || voice->current.volume == 0) {
        sfx_disable_voice(channel);
        return;
    }

    if (voice->current.wave != AMIGA_SFX_WAVE_SAMPLE) {
        sfx_disable_voice(channel);
        voice->active = 0;
        return;
    }
    sample = sfx_sample_ptr(sfx, voice->current.sample_id);
    sample_words = sfx_sample_len_words(voice->current.sample_id);
    if (!sample || sample_words == 0) {
        sfx_disable_voice(channel);
        voice->active = 0;
        return;
    }

    CREGS->dmacon = sfx_dma_bit(channel);
    CREGS->aud[channel].ac_ptr = (UWORD *)sample;
    CREGS->aud[channel].ac_len = sample_words;
    CREGS->aud[channel].ac_per = AMIGA_SFX_SAMPLE_PERIOD;
    CREGS->aud[channel].ac_vol = voice->current.volume;
    CREGS->dmacon = DMAF_SETCLR | sfx_dma_bit(channel);
}

static void sfx_update_voice(AmigaSfxState *sfx, AmigaSfxVoice *voice) {
    if (!voice->active) {
        sfx_start_voice_step(sfx, voice);
    }
    if (!voice->active) {
        return;
    }

    ++voice->elapsed;
    if (voice->elapsed >= voice->current.ticks) {
        voice->active = 0;
        if (voice->count == 0) {
            sfx_disable_voice(voice->channel);
        } else {
            sfx_start_voice_step(sfx, voice);
        }
    }
}

static void sfx_update(AmigaSfxState *sfx) {
    if (!sfx || !sfx->enabled) {
        return;
    }
    sfx_update_voice(sfx, &sfx->voice[0]);
    sfx_update_voice(sfx, &sfx->voice[1]);
}

static BOOL sfx_init(AmigaSfxState *sfx) {
    memset(sfx, 0, sizeof(*sfx));
#if !AMIGA_SFX
    return FALSE;
#else
    sfx->samples = (UBYTE *)AllocMem(AMIGA_SFX_DATA_BYTES, ICE_MEMF_CHIP | ICE_MEMF_CLEAR);
    if (!sfx->samples) {
        return FALSE;
    }
    CopyMem((APTR)g_amiga_sfx_data, (APTR)sfx->samples, AMIGA_SFX_DATA_BYTES);
    sfx->voice[0].channel = 0;
    sfx->voice[1].channel = 1;
    sfx->enabled = 1;
    sfx_disable_voice(0);
    sfx_disable_voice(1);
    return TRUE;
#endif
}

static void sfx_shutdown(AmigaSfxState *sfx) {
    if (!sfx) {
        return;
    }
    sfx_disable_voice(0);
    sfx_disable_voice(1);
    if (sfx->samples) {
        FreeMem(sfx->samples, AMIGA_SFX_DATA_BYTES);
    }
    memset(sfx, 0, sizeof(*sfx));
}

ICE_INLINE void put_pixel_unchecked(UBYTE *screen, int x, int y, UBYTE color) {
    UBYTE mask;
    UBYTE inv_mask;
    UBYTE *p;
    ULONG base;
    mask = (UBYTE)(0x80u >> (x & 7));
    inv_mask = (UBYTE)~mask;
    base = ((ULONG)y * ROW_STRIDE_BYTES) + (ULONG)(x >> 3);
    p = screen + base;
    if (color & 1u) p[0] = (UBYTE)(p[0] | mask); else p[0] = (UBYTE)(p[0] & inv_mask);
    if (color & 2u) p[ROW_BYTES] = (UBYTE)(p[ROW_BYTES] | mask); else p[ROW_BYTES] = (UBYTE)(p[ROW_BYTES] & inv_mask);
    if (color & 4u) p[ROW_BYTES * 2] = (UBYTE)(p[ROW_BYTES * 2] | mask); else p[ROW_BYTES * 2] = (UBYTE)(p[ROW_BYTES * 2] & inv_mask);
    if (color & 8u) p[ROW_BYTES * 3] = (UBYTE)(p[ROW_BYTES * 3] | mask); else p[ROW_BYTES * 3] = (UBYTE)(p[ROW_BYTES * 3] & inv_mask);
    if (color & 16u) p[ROW_BYTES * 4] = (UBYTE)(p[ROW_BYTES * 4] | mask); else p[ROW_BYTES * 4] = (UBYTE)(p[ROW_BYTES * 4] & inv_mask);
}

ICE_INLINE void put_pixel(UBYTE *screen, int x, int y, UBYTE color) {
    if (x < 0 || y < 0 || x >= SCREEN_W || y >= SCREEN_H) {
        return;
    }
    put_pixel_unchecked(screen, x, y, color);
}

static void draw_hline(UBYTE *screen, int x0, int x1, int y, UBYTE color) {
    int plane;
    int start_byte;
    int end_byte;
    UBYTE left_mask;
    UBYTE right_mask;
    UBYTE mask;
    if (y < 0 || y >= SCREEN_H) {
        return;
    }
    x0 = clamp_int(x0, 0, SCREEN_W - 1);
    x1 = clamp_int(x1, 0, SCREEN_W - 1);
    if (x1 < x0) {
        return;
    }
    start_byte = x0 >> 3;
    end_byte = x1 >> 3;
    left_mask = (UBYTE)(0xFFu >> (x0 & 7));
    right_mask = (UBYTE)(0xFFu << (7 - (x1 & 7)));
    for (plane = 0; plane < DEPTH; ++plane) {
        UBYTE *row = screen + ((ULONG)y * ROW_STRIDE_BYTES) + ((ULONG)plane * ROW_BYTES);
        UBYTE fill = ((color >> plane) & 1u) ? 0xFFu : 0x00u;
        if (start_byte == end_byte) {
            mask = (UBYTE)(left_mask & right_mask);
            if (fill) {
                row[start_byte] = (UBYTE)(row[start_byte] | mask);
            } else {
                row[start_byte] = (UBYTE)(row[start_byte] & (UBYTE)~mask);
            }
            continue;
        }
        if (fill) {
            row[start_byte] = (UBYTE)(row[start_byte] | left_mask);
            row[end_byte] = (UBYTE)(row[end_byte] | right_mask);
        } else {
            row[start_byte] = (UBYTE)(row[start_byte] & (UBYTE)~left_mask);
            row[end_byte] = (UBYTE)(row[end_byte] & (UBYTE)~right_mask);
        }
        if (end_byte > start_byte + 1) {
            memset(row + start_byte + 1, fill, (size_t)(end_byte - start_byte - 1));
        }
    }
}

static void draw_rect(UBYTE *screen, int x0, int y0, int x1, int y1, UBYTE color) {
    int y;
    x0 = clamp_int(x0, 0, SCREEN_W - 1);
    y0 = clamp_int(y0, 0, SCREEN_H - 1);
    x1 = clamp_int(x1, 0, SCREEN_W - 1);
    y1 = clamp_int(y1, 0, SCREEN_H - 1);
    if (x1 < x0 || y1 < y0) {
        return;
    }
    for (y = y0; y <= y1; ++y) {
        draw_hline(screen, x0, x1, y, color);
    }
}

static void draw_digit(UBYTE *screen, int x, int y, int digit, UBYTE color) {
    int row;
    int col;
    if (digit < 0 || digit > 9) {
        return;
    }
    for (row = 0; row < 5; ++row) {
        UBYTE bits = kDigit3x5[digit][row];
        for (col = 0; col < 3; ++col) {
            if ((bits >> (2 - col)) & 1u) {
                put_pixel(screen, x + col, y + row, color);
            }
        }
    }
}

static bool letter_rows(char ch, UBYTE out[5]) {
    switch (ch) {
        case 'A': out[0] = 7; out[1] = 5; out[2] = 7; out[3] = 5; out[4] = 5; return true;
        case 'B': out[0] = 6; out[1] = 5; out[2] = 6; out[3] = 5; out[4] = 6; return true;
        case 'C': out[0] = 7; out[1] = 4; out[2] = 4; out[3] = 4; out[4] = 7; return true;
        case 'D': out[0] = 6; out[1] = 5; out[2] = 5; out[3] = 5; out[4] = 6; return true;
        case 'E': out[0] = 7; out[1] = 4; out[2] = 6; out[3] = 4; out[4] = 7; return true;
        case 'F': out[0] = 7; out[1] = 4; out[2] = 6; out[3] = 4; out[4] = 4; return true;
        case 'G': out[0] = 7; out[1] = 4; out[2] = 5; out[3] = 5; out[4] = 7; return true;
        case 'H': out[0] = 5; out[1] = 5; out[2] = 7; out[3] = 5; out[4] = 5; return true;
        case 'I': out[0] = 7; out[1] = 2; out[2] = 2; out[3] = 2; out[4] = 7; return true;
        case 'J': out[0] = 1; out[1] = 1; out[2] = 1; out[3] = 5; out[4] = 7; return true;
        case 'K': out[0] = 5; out[1] = 5; out[2] = 6; out[3] = 5; out[4] = 5; return true;
        case 'L': out[0] = 4; out[1] = 4; out[2] = 4; out[3] = 4; out[4] = 7; return true;
        case 'M': out[0] = 5; out[1] = 7; out[2] = 5; out[3] = 5; out[4] = 5; return true;
        case 'N': out[0] = 5; out[1] = 7; out[2] = 7; out[3] = 7; out[4] = 5; return true;
        case 'O': out[0] = 7; out[1] = 5; out[2] = 5; out[3] = 5; out[4] = 7; return true;
        case 'P': out[0] = 6; out[1] = 5; out[2] = 6; out[3] = 4; out[4] = 4; return true;
        case 'Q': out[0] = 7; out[1] = 5; out[2] = 5; out[3] = 7; out[4] = 1; return true;
        case 'R': out[0] = 6; out[1] = 5; out[2] = 6; out[3] = 5; out[4] = 5; return true;
        case 'S': out[0] = 7; out[1] = 4; out[2] = 7; out[3] = 1; out[4] = 7; return true;
        case 'T': out[0] = 7; out[1] = 2; out[2] = 2; out[3] = 2; out[4] = 2; return true;
        case 'U': out[0] = 5; out[1] = 5; out[2] = 5; out[3] = 5; out[4] = 7; return true;
        case 'V': out[0] = 5; out[1] = 5; out[2] = 5; out[3] = 5; out[4] = 2; return true;
        case 'W': out[0] = 5; out[1] = 5; out[2] = 5; out[3] = 7; out[4] = 5; return true;
        case 'X': out[0] = 5; out[1] = 5; out[2] = 2; out[3] = 5; out[4] = 5; return true;
        case 'Y': out[0] = 5; out[1] = 5; out[2] = 2; out[3] = 2; out[4] = 2; return true;
        case 'Z': out[0] = 7; out[1] = 1; out[2] = 2; out[3] = 4; out[4] = 7; return true;
        case ':': out[0] = 0; out[1] = 2; out[2] = 0; out[3] = 2; out[4] = 0; return true;
        case '-': out[0] = 0; out[1] = 0; out[2] = 7; out[3] = 0; out[4] = 0; return true;
        case '+': out[0] = 0; out[1] = 2; out[2] = 7; out[3] = 2; out[4] = 0; return true;
        case '/': out[0] = 1; out[1] = 1; out[2] = 2; out[3] = 4; out[4] = 4; return true;
        case '.': out[0] = 0; out[1] = 0; out[2] = 0; out[3] = 0; out[4] = 2; return true;
        case '<': out[0] = 1; out[1] = 2; out[2] = 4; out[3] = 2; out[4] = 1; return true;
        case '>': out[0] = 4; out[1] = 2; out[2] = 1; out[3] = 2; out[4] = 4; return true;
        default: return false;
    }
}

ICE_INLINE char font_normalized_char(char ch) {
    if (ch >= 'a' && ch <= 'z') {
        ch = (char)(ch - 'a' + 'A');
    }
    return ch;
}

static bool glyph_rows(char ch, UBYTE out[5]) {
    ch = font_normalized_char(ch);
    if (ch >= '0' && ch <= '9') {
        memcpy(out, kDigit3x5[ch - '0'], 5);
        return true;
    }
    if (ch == ' ') {
        memset(out, 0, 5);
        return true;
    }
    return letter_rows(ch, out);
}

ICE_INLINE void font_set_5bit(UBYTE *row, int x) {
    if (x >= 0 && x < 5) {
        *row = (UBYTE)(*row | (UBYTE)(0x10u >> x));
    }
}

static void font_precompute(void) {
    int ch;
    int row;
    int col;
    if (g_font_ready) {
        return;
    }
    memset(g_font_fill_rows, 0, sizeof(g_font_fill_rows));
    memset(g_font_outline_rows, 0, sizeof(g_font_outline_rows));
    for (ch = 0; ch < 128; ++ch) {
        UBYTE rows[5];
        if (!glyph_rows((char)ch, rows)) {
            continue;
        }
        for (row = 0; row < 5; ++row) {
            UBYTE bits = (UBYTE)(rows[row] & 0x07u);
            g_font_fill_rows[ch][row] = bits;
            for (col = 0; col < 3; ++col) {
                if (((bits >> (2 - col)) & 1u) == 0) {
                    continue;
                }
                font_set_5bit(&g_font_outline_rows[ch][row + 1], col);
                font_set_5bit(&g_font_outline_rows[ch][row + 1], col + 2);
                font_set_5bit(&g_font_outline_rows[ch][row], col + 1);
                font_set_5bit(&g_font_outline_rows[ch][row + 2], col + 1);
            }
        }
        for (row = 0; row < 5; ++row) {
            g_font_outline_rows[ch][row + 1] =
                (UBYTE)(g_font_outline_rows[ch][row + 1] & (UBYTE)~(g_font_fill_rows[ch][row] << 1));
        }
    }
    g_font_ready = 1;
}

ICE_INLINE unsigned int font_index(char ch) {
    ch = font_normalized_char(ch);
    if ((unsigned char)ch >= 128u) {
        return 0u;
    }
    return (unsigned int)(unsigned char)ch;
}

ICE_INLINE void draw_3bit_row(UBYTE *screen, int x, int y, UBYTE bits, UBYTE color) {
    if ((bits & 0x04u) != 0) put_pixel(screen, x + 0, y, color);
    if ((bits & 0x02u) != 0) put_pixel(screen, x + 1, y, color);
    if ((bits & 0x01u) != 0) put_pixel(screen, x + 2, y, color);
}

ICE_INLINE void draw_5bit_row(UBYTE *screen, int x, int y, UBYTE bits, UBYTE color) {
    if ((bits & 0x10u) != 0) put_pixel(screen, x + 0, y, color);
    if ((bits & 0x08u) != 0) put_pixel(screen, x + 1, y, color);
    if ((bits & 0x04u) != 0) put_pixel(screen, x + 2, y, color);
    if ((bits & 0x02u) != 0) put_pixel(screen, x + 3, y, color);
    if ((bits & 0x01u) != 0) put_pixel(screen, x + 4, y, color);
}

static void draw_char_outlined_fast(UBYTE *screen, int x, int y, char ch, UBYTE color, UBYTE outline) {
    unsigned int idx;
    int row;
    font_precompute();
    idx = font_index(ch);
    if (idx == (unsigned int)' ') {
        return;
    }
    for (row = 0; row < 7; ++row) {
        draw_5bit_row(screen, x - 1, y - 1 + row, g_font_outline_rows[idx][row], outline);
    }
    for (row = 0; row < 5; ++row) {
        draw_3bit_row(screen, x, y + row, g_font_fill_rows[idx][row], color);
    }
}

static void draw_text_outlined_fast(UBYTE *screen, int x, int y, const char *text, UBYTE color, UBYTE outline) {
    while (*text != '\0') {
        draw_char_outlined_fast(screen, x, y, *text, color, outline);
        x += 4;
        ++text;
    }
}

static void draw_scaled_bit(UBYTE *screen, int x, int y, int scale, UBYTE color) {
    int sx;
    int sy;
    for (sy = 0; sy < scale; ++sy) {
        for (sx = 0; sx < scale; ++sx) {
            put_pixel(screen, x + sx, y + sy, color);
        }
    }
}

static void draw_scaled_mask_row(UBYTE *screen, int x, int y, UBYTE bits, int width, int scale, UBYTE color) {
    int col;
    for (col = 0; col < width; ++col) {
        UBYTE mask = (UBYTE)(1u << (width - 1 - col));
        if ((bits & mask) != 0) {
            draw_scaled_bit(screen, x + col * scale, y, scale, color);
        }
    }
}

static void draw_char_scaled_outlined_fast(UBYTE *screen, int x, int y, char ch, UBYTE color, UBYTE outline, int scale) {
    unsigned int idx;
    int row;
    scale = clamp_int(scale, 1, 6);
    font_precompute();
    idx = font_index(ch);
    if (idx == (unsigned int)' ') {
        return;
    }
    for (row = 0; row < 7; ++row) {
        draw_scaled_mask_row(screen, x - scale, y - scale + row * scale, g_font_outline_rows[idx][row], 5, scale, outline);
    }
    for (row = 0; row < 5; ++row) {
        draw_scaled_mask_row(screen, x, y + row * scale, g_font_fill_rows[idx][row], 3, scale, color);
    }
}

static void draw_text_scaled_outlined_fast(
    UBYTE *screen,
    int x,
    int y,
    const char *text,
    UBYTE color,
    UBYTE outline,
    int scale) {
    scale = clamp_int(scale, 1, 6);
    while (*text != '\0') {
        draw_char_scaled_outlined_fast(screen, x, y, *text, color, outline, scale);
        x += 4 * scale;
        ++text;
    }
}

static void draw_char(UBYTE *screen, int x, int y, char ch, UBYTE color) {
    UBYTE rows[5];
    int row;
    int col;
    if (ch >= '0' && ch <= '9') {
        draw_digit(screen, x, y, ch - '0', color);
        return;
    }
    if (ch >= 'a' && ch <= 'z') {
        ch = (char)(ch - 'a' + 'A');
    }
    if (ch == ' ') {
        return;
    }
    if (!letter_rows(ch, rows)) {
        return;
    }
    for (row = 0; row < 5; ++row) {
        for (col = 0; col < 3; ++col) {
            if ((rows[row] >> (2 - col)) & 1u) {
                put_pixel(screen, x + col, y + row, color);
            }
        }
    }
}

static void draw_text(UBYTE *screen, int x, int y, const char *text, UBYTE color) {
    while (*text != '\0') {
        draw_char(screen, x, y, *text, color);
        x += 4;
        ++text;
    }
}

static int text_width(const char *text) {
    int len = 0;
    while (text[len] != '\0') {
        ++len;
    }
    return (len > 0) ? ((len * 4) - 1) : 0;
}

static void draw_centered_text(UBYTE *screen, int x0, int x1, int y, const char *text, UBYTE color) {
    int w = text_width(text);
    int x = x0 + (((x1 - x0 + 1) - w) / 2);
    draw_text(screen, x, y, text, color);
}

static void draw_text_outlined(UBYTE *screen, int x, int y, const char *text, UBYTE color, UBYTE outline) {
#if AMIGA_FAST_RENDER
    draw_text_outlined_fast(screen, x, y, text, color, outline);
#else
    draw_text(screen, x - 1, y, text, outline);
    draw_text(screen, x + 1, y, text, outline);
    draw_text(screen, x, y - 1, text, outline);
    draw_text(screen, x, y + 1, text, outline);
    draw_text(screen, x, y, text, color);
#endif
}

static void draw_centered_text_outlined(UBYTE *screen, int x0, int x1, int y, const char *text, UBYTE color, UBYTE outline) {
    int w = text_width(text);
    int x = x0 + (((x1 - x0 + 1) - w) / 2);
    draw_text_outlined(screen, x, y, text, color, outline);
}

static int text_width_scaled(const char *text, int scale) {
    int len = 0;
    scale = clamp_int(scale, 1, 4);
    while (text[len] != '\0') {
        ++len;
    }
    return (len > 0) ? ((len * 4 * scale) - scale) : 0;
}

static void draw_char_scaled(UBYTE *screen, int x, int y, char ch, UBYTE color, int scale) {
    UBYTE rows[5];
    int row;
    int col;
    int sx;
    int sy;
    scale = clamp_int(scale, 1, 4);
    if (ch >= '0' && ch <= '9') {
        memcpy(rows, kDigit3x5[ch - '0'], sizeof(rows));
    } else {
        if (ch >= 'a' && ch <= 'z') {
            ch = (char)(ch - 'a' + 'A');
        }
        if (ch == ' ') {
            return;
        }
        if (!letter_rows(ch, rows)) {
            return;
        }
    }
    for (row = 0; row < 5; ++row) {
        for (col = 0; col < 3; ++col) {
            if (((rows[row] >> (2 - col)) & 1u) == 0) {
                continue;
            }
            for (sy = 0; sy < scale; ++sy) {
                for (sx = 0; sx < scale; ++sx) {
                    put_pixel(screen, x + col * scale + sx, y + row * scale + sy, color);
                }
            }
        }
    }
}

static void draw_text_scaled(UBYTE *screen, int x, int y, const char *text, UBYTE color, int scale) {
    scale = clamp_int(scale, 1, 4);
    while (*text != '\0') {
        draw_char_scaled(screen, x, y, *text, color, scale);
        x += 4 * scale;
        ++text;
    }
}

static void draw_text_scaled_outlined(
    UBYTE *screen,
    int x,
    int y,
    const char *text,
    UBYTE color,
    UBYTE outline,
    int scale) {
#if AMIGA_FAST_RENDER
    draw_text_scaled_outlined_fast(screen, x, y, text, color, outline, scale);
#else
    draw_text_scaled(screen, x - 1, y, text, outline, scale);
    draw_text_scaled(screen, x + 1, y, text, outline, scale);
    draw_text_scaled(screen, x, y - 1, text, outline, scale);
    draw_text_scaled(screen, x, y + 1, text, outline, scale);
    draw_text_scaled(screen, x, y, text, color, scale);
#endif
}

static void draw_centered_text_scaled_outlined(
    UBYTE *screen,
    int x0,
    int x1,
    int y,
    const char *text,
    UBYTE color,
    UBYTE outline,
    int scale) {
    int w = text_width_scaled(text, scale);
    int x = x0 + (((x1 - x0 + 1) - w) / 2);
    draw_text_scaled_outlined(screen, x, y, text, color, outline, scale);
}

static void draw_number(UBYTE *screen, int x, int y, uint32_t value, int min_digits, UBYTE color) {
    int digits[10];
    int count = 0;
    int i;
    do {
        digits[count++] = (int)(value % 10u);
        value /= 10u;
    } while (value > 0u && count < 10);
    while (count < min_digits && count < 10) {
        digits[count++] = 0;
    }
    for (i = count - 1; i >= 0; --i) {
        draw_digit(screen, x, y, digits[i], color);
        x += 4;
    }
}

static void draw_number_outlined(UBYTE *screen, int x, int y, uint32_t value, int min_digits, UBYTE color, UBYTE outline) {
#if AMIGA_FAST_RENDER
    int digits[10];
    int count = 0;
    int i;
    do {
        digits[count++] = (int)(value % 10u);
        value /= 10u;
    } while (value > 0u && count < 10);
    while (count < min_digits && count < 10) {
        digits[count++] = 0;
    }
    for (i = count - 1; i >= 0; --i) {
        draw_char_outlined_fast(screen, x, y, (char)('0' + digits[i]), color, outline);
        x += 4;
    }
#else
    draw_number(screen, x - 1, y, value, min_digits, outline);
    draw_number(screen, x + 1, y, value, min_digits, outline);
    draw_number(screen, x, y - 1, value, min_digits, outline);
    draw_number(screen, x, y + 1, value, min_digits, outline);
    draw_number(screen, x, y, value, min_digits, color);
#endif
}

static int number_width(uint32_t value, int min_digits) {
    int count = 0;
    do {
        ++count;
        value /= 10u;
    } while (value > 0u && count < 10);
    while (count < min_digits && count < 10) {
        ++count;
    }
    return (count > 0) ? ((count * 4) - 1) : 0;
}

static void draw_number_right(UBYTE *screen, int right_x, int y, uint32_t value, int min_digits, UBYTE color) {
    draw_number(screen, right_x - number_width(value, min_digits) + 1, y, value, min_digits, color);
}

static void draw_digit_scaled(UBYTE *screen, int x, int y, int digit, UBYTE color, int scale) {
    int row;
    int col;
    int sx;
    int sy;
    if (digit < 0 || digit > 9) {
        return;
    }
    scale = clamp_int(scale, 1, 6);
    for (row = 0; row < 5; ++row) {
        UBYTE bits = kDigit3x5[digit][row];
        for (col = 0; col < 3; ++col) {
            if (((bits >> (2 - col)) & 1u) == 0) {
                continue;
            }
            for (sy = 0; sy < scale; ++sy) {
                for (sx = 0; sx < scale; ++sx) {
                    put_pixel(screen, x + col * scale + sx, y + row * scale + sy, color);
                }
            }
        }
    }
}

static int number_width_scaled(uint32_t value, int min_digits, int scale) {
    int digits = 1;
    scale = clamp_int(scale, 1, 6);
    while (value >= 10u && digits < 10) {
        value /= 10u;
        ++digits;
    }
    if (digits < min_digits) {
        digits = min_digits;
    }
    return (digits * (4 * scale)) - scale;
}

static void draw_number_scaled(UBYTE *screen, int x, int y, uint32_t value, int min_digits, UBYTE color, int scale) {
    int digits[10];
    int count = 0;
    int i;
    scale = clamp_int(scale, 1, 6);
    do {
        digits[count++] = (int)(value % 10u);
        value /= 10u;
    } while (value > 0u && count < 10);
    while (count < min_digits && count < 10) {
        digits[count++] = 0;
    }
    for (i = count - 1; i >= 0; --i) {
        draw_digit_scaled(screen, x, y, digits[i], color, scale);
        x += 4 * scale;
    }
}

static void draw_number_scaled_outlined(UBYTE *screen, int x, int y, uint32_t value, int min_digits, UBYTE color, UBYTE outline, int scale) {
#if AMIGA_FAST_RENDER
    int digits[10];
    int count = 0;
    int i;
    scale = clamp_int(scale, 1, 6);
    do {
        digits[count++] = (int)(value % 10u);
        value /= 10u;
    } while (value > 0u && count < 10);
    while (count < min_digits && count < 10) {
        digits[count++] = 0;
    }
    for (i = count - 1; i >= 0; --i) {
        draw_char_scaled_outlined_fast(screen, x, y, (char)('0' + digits[i]), color, outline, scale);
        x += 4 * scale;
    }
#else
    draw_number_scaled(screen, x - 1, y, value, min_digits, outline, scale);
    draw_number_scaled(screen, x + 1, y, value, min_digits, outline, scale);
    draw_number_scaled(screen, x, y - 1, value, min_digits, outline, scale);
    draw_number_scaled(screen, x, y + 1, value, min_digits, outline, scale);
    draw_number_scaled(screen, x, y, value, min_digits, color, scale);
#endif
}

static void mark_all_dirty(RendererState *rr, int buf) {
    int y;
    for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
        ULONG row = (ULONG)((1UL << GAME_GRID_WIDTH) - 1UL);
        rr->dirty_rows[buf][y] = row;
        rr->static_dirty_rows[buf][y] = row;
    }
    rr->dirty_any[buf] = 1;
}

static void renderer_reset_for_round(RendererState *rr) {
    memset(rr, 0, sizeof(*rr));
}

static void mark_rect_dirty(RendererState *rr, int buf, int x, int y, int w, int h) {
    int x0;
    int y0;
    int x1;
    int y1;
    int cy;
    ULONG mask;
    if (w <= 0 || h <= 0) {
        return;
    }
    if (x >= SCREEN_W || y >= GAMEPLAY_H || x + w <= 0 || y + h <= 0) {
        return;
    }
    x0 = clamp_int(x, 0, SCREEN_W - 1) >> 4;
    y0 = clamp_int(y, 0, GAMEPLAY_H - 1) >> 4;
    x1 = clamp_int(x + w - 1, 0, SCREEN_W - 1) >> 4;
    y1 = clamp_int(y + h - 1, 0, GAMEPLAY_H - 1) >> 4;
    mask = (ULONG)(((1UL << (x1 - x0 + 1)) - 1UL) << x0);
    for (cy = y0; cy <= y1; ++cy) {
        rr->dirty_rows[buf][cy] |= mask;
    }
    rr->dirty_any[buf] = 1;
}

static void mark_gameplay_border_dirty(RendererState *rr, int buf) {
    int y;
    const ULONG all = (ULONG)((1UL << GAME_GRID_WIDTH) - 1UL);
    rr->dirty_rows[buf][0] |= all;
    rr->dirty_rows[buf][GAME_GRID_HEIGHT - 1] |= all;
    for (y = 1; y < GAME_GRID_HEIGHT - 1; ++y) {
        rr->dirty_rows[buf][y] |= 1UL;
        rr->dirty_rows[buf][y] |= (1UL << (GAME_GRID_WIDTH - 1));
    }
    rr->dirty_any[buf] = 1;
}

static void mark_dirty_rect(RendererState *rr, int buf, const DirtyRect *r) {
    if (r && r->w > 0 && r->h > 0) {
        mark_rect_dirty(rr, buf, r->x, r->y, r->w, r->h);
    }
}

ICE_INLINE bool dirty_rects_same(const DirtyRect *a, const DirtyRect *b) {
    return a->x == b->x &&
           a->y == b->y &&
           a->w == b->w &&
           a->h == b->h;
}

static bool dirty_rect_overlaps_pending_cells(const RendererState *rr, int buf, const DirtyRect *r) {
    int x0;
    int y0;
    int x1;
    int y1;
    int y;
    ULONG mask;

    if (!r || r->w <= 0 || r->h <= 0) {
        return false;
    }
    if (!rr->dirty_any[buf]) {
        return false;
    }

    x0 = clamp_int(r->x, 0, SCREEN_W - 1) >> 4;
    y0 = clamp_int(r->y, 0, GAMEPLAY_H - 1) >> 4;
    x1 = clamp_int(r->x + r->w - 1, 0, SCREEN_W - 1) >> 4;
    y1 = clamp_int(r->y + r->h - 1, 0, GAMEPLAY_H - 1) >> 4;
    mask = (ULONG)(((1UL << (x1 - x0 + 1)) - 1UL) << x0);
    for (y = y0; y <= y1; ++y) {
        if ((rr->dirty_rows[buf][y] & mask) != 0) {
            return true;
        }
    }
    return false;
}

static void restore_dynamic_caches(RendererState *rr, int buf) {
    int i;
    UWORD mask;
    if (rr->cached_player[buf].valid) {
        mark_dirty_rect(rr, buf, &rr->cached_player[buf].rect);
    }
    rr->cached_player[buf].valid = 0;
    mask = rr->cached_settle_mask[buf];
    for (i = 0; mask != 0u && i < GAME_MAX_IMPACT_FX; ++i, mask >>= 1) {
        if ((mask & 1u) == 0u) {
            continue;
        }
        if (rr->cached_settle_block[buf][i].valid) {
            mark_dirty_rect(rr, buf, &rr->cached_settle_block[buf][i].rect);
        }
        rr->cached_settle_block[buf][i].valid = 0;
    }
    rr->cached_settle_live[buf] = 0;
    rr->cached_settle_mask[buf] = 0;
    mask = rr->cached_block_mask[buf];
    for (i = 0; mask != 0u && i < GAME_MAX_MOVING_BLOCKS; ++i, mask >>= 1) {
        if ((mask & 1u) == 0u) {
            continue;
        }
        if (rr->cached_block[buf][i].valid) {
            mark_dirty_rect(rr, buf, &rr->cached_block[buf][i].rect);
        }
        rr->cached_block[buf][i].valid = 0;
    }
    rr->cached_block_mask[buf] = 0;
    mask = rr->cached_enemy_mask[buf];
    for (i = 0; mask != 0u && i < GAME_MAX_ENEMIES; ++i, mask >>= 1) {
        if ((mask & 1u) == 0u) {
            continue;
        }
        if (rr->cached_enemy[buf][i].valid) {
            mark_dirty_rect(rr, buf, &rr->cached_enemy[buf][i].rect);
        }
        rr->cached_enemy[buf][i].valid = 0;
    }
    rr->cached_enemy_mask[buf] = 0;
}

static void prepare_dynamic_cache(
    RendererState *rr,
    int buf,
    DynamicCache *cache,
    bool valid,
    ULONG key,
    const DirtyRect *rect,
    int px,
    int py,
    AmigaSpriteId sprite) {
    bool changed;
    cache->next_valid = valid ? 1u : 0u;
    if (valid && rect) {
        cache->next_key = key;
        cache->next_rect = *rect;
        cache->next_x = (WORD)px;
        cache->next_y = (WORD)py;
        cache->next_sprite = (UWORD)sprite;
    }
    if (!cache->valid) {
        return;
    }
    changed = !valid || cache->key != key || !rect || !dirty_rects_same(&cache->rect, rect);
    if (!changed && dirty_rect_overlaps_pending_cells(rr, buf, &cache->rect)) {
        changed = true;
    }
    if (changed) {
        mark_dirty_rect(rr, buf, &cache->rect);
        cache->valid = 0;
    }
}

static void invalidate_moving_blocks_overlapping_dirty(RendererState *rr, int buf) {
    int i;
    UWORD mask;

    if (!rr->dirty_any[buf]) {
        return;
    }
    mask = rr->cached_block_mask[buf];
    for (i = 0; mask != 0u && i < GAME_MAX_MOVING_BLOCKS; ++i, mask >>= 1) {
        if ((mask & 1u) == 0u) {
            continue;
        }
        if (rr->cached_block[buf][i].valid &&
            dirty_rect_overlaps_pending_cells(rr, buf, &rr->cached_block[buf][i].rect)) {
            rr->cached_block[buf][i].valid = 0;
        }
    }
}

static void save_dynamic_rect(const AmigaApp *app, RendererState *rr, int buf, UBYTE *screen, int x, int y, int w, int h) {
    WORD n;
    DirtyRect rect;
    if (w <= 0 || h <= 0 || x >= SCREEN_W || y >= SCREEN_H || x + w <= 0 || y + h <= 0) {
        return;
    }
    rect.x = (WORD)clamp_int(x, 0, SCREEN_W - 1);
    rect.y = (WORD)clamp_int(y, 0, SCREEN_H - 1);
    rect.w = (WORD)(clamp_int(x + w, 0, SCREEN_W) - rect.x);
    rect.h = (WORD)(clamp_int(y + h, 0, SCREEN_H) - rect.y);
    if (rect.w <= 0 || rect.h <= 0) {
        return;
    }
    n = rr->prev_dynamic_count[buf];
    if (n >= DYNAMIC_DIRTY_SLOTS) {
        return;
    }
    rr->prev_dynamic[buf][n] = rect;
    rr->prev_dynamic_count[buf] = (WORD)(n + 1);
}

static void restore_previous_dynamic(const AmigaApp *app, RendererState *rr, int buf, UBYTE *screen) {
    WORD i;
    for (i = 0; i < rr->prev_dynamic_count[buf]; ++i) {
        mark_dirty_rect(rr, buf, &rr->prev_dynamic[buf][i]);
    }
    rr->prev_dynamic_count[buf] = 0;
}

ICE_INLINE ULONG dynamic_bob_key(AmigaSpriteId sprite, int px, int py, ULONG extra) {
    return ((ULONG)(px & 0x01FF)) |
           (((ULONG)(py & 0x00FF)) << 9) |
           (((ULONG)sprite & 0x003F) << 17) |
           ((extra & 0x01FFUL) << 23);
}

static void make_bob_dirty_rect(int px, int py, DirtyRect *rect) {
    int x0;
    int shift;
    shift = px & (GAME_TILE_SIZE - 1);
    x0 = shift == 0 ? px : (px & ~(GAME_TILE_SIZE - 1));
    rect->x = (WORD)x0;
    rect->y = (WORD)py;
    rect->w = (WORD)(shift == 0 ? GAME_TILE_SIZE : (GAME_TILE_SIZE * 2));
    rect->h = GAME_TILE_SIZE;
}

static bool is_pushable_block(BlockType block) {
    return block == BLOCK_ICE || block == BLOCK_SPECIAL || block == BLOCK_CRACKED;
}

static bool impact_style_is_stop(int style) {
    return style == IMPACT_FX_STYLE_STOP ||
           style == IMPACT_FX_STYLE_SPECIAL_STOP ||
           style == IMPACT_FX_STYLE_CRACKED_STOP;
}

static int stop_bounce_displacement_px(const ImpactFx *fx) {
    static const WORD kStopBouncePx[AMIGA_STOP_BOUNCE_TICKS] = {
        0, -1, -2, -2, -2, -2, -2, -1, -1, -1, -1, 0
    };
    int age;
    if (!fx->active || !fx->anchors_block || fx->ttl_ticks <= 0) {
        return 0;
    }
    age = fx->base_ttl_ticks - fx->ttl_ticks;
    if (age < 0) {
        return 0;
    }
    if (fx->base_ttl_ticks == AMIGA_STOP_BOUNCE_TICKS && age < AMIGA_STOP_BOUNCE_TICKS) {
        return kStopBouncePx[age];
    }
    if (age < (fx->base_ttl_ticks / 3)) {
        return -2;
    }
    if (age < fx->base_ttl_ticks) {
        return -1;
    }
    return 0;
}

static void update_hidden_block_rows(RendererState *rr, const RenderState *rs) {
    int i;
    int y;
    UWORD mask;
    for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
        rr->hidden_block_rows[y] = 0;
    }
    mask = (UWORD)rs->active_impact_fx_mask;
    if (mask == 0u) {
        return;
    }
    for (i = 0; mask != 0u && i < GAME_MAX_IMPACT_FX; ++i, mask >>= 1) {
        const ImpactFx *fx = &rs->impact_fx[i];
        if ((mask & 1u) == 0u || !fx->active || !fx->anchors_block) {
            continue;
        }
        if (!impact_style_is_stop(fx->style)) {
            continue;
        }
        if (fx->anchor_tile_x >= 0 && fx->anchor_tile_x < GAME_GRID_WIDTH &&
            fx->anchor_tile_y >= 0 && fx->anchor_tile_y < GAME_GRID_HEIGHT) {
            rr->hidden_block_rows[fx->anchor_tile_y] |= (ULONG)1UL << (ULONG)fx->anchor_tile_x;
        }
    }
}

ICE_INLINE bool settle_block_hidden_at(const RendererState *rr, int tile_x, int tile_y) {
    return (rr->hidden_block_rows[tile_y] & ((ULONG)1UL << (ULONG)tile_x)) != 0;
}

static bool make_settle_block_dynamic(const RenderState *rs, int fx_index, DirtyRect *rect, ULONG *key, int *out_px, int *out_py, AmigaSpriteId *out_sprite) {
    const ImpactFx *fx;
    BlockType block;
    AmigaSpriteId sprite;
    int disp;
    int px;
    int py;

    if (fx_index < 0 || fx_index >= GAME_MAX_IMPACT_FX) {
        return false;
    }
    fx = &rs->impact_fx[fx_index];
    if (!fx->active || !fx->anchors_block || !impact_style_is_stop(fx->style)) {
        return false;
    }
    if (fx->anchor_tile_x < 0 || fx->anchor_tile_y < 0 ||
        fx->anchor_tile_x >= GAME_GRID_WIDTH || fx->anchor_tile_y >= GAME_GRID_HEIGHT) {
        return false;
    }
    block = rs->blocks[fx->anchor_tile_y][fx->anchor_tile_x];
    if (!is_pushable_block(block)) {
        return false;
    }

    disp = stop_bounce_displacement_px(fx);
    px = (fx->anchor_tile_x * GAME_TILE_SIZE) + (dir_dx(fx->direction) * disp);
    py = (fx->anchor_tile_y * GAME_TILE_SIZE) + (dir_dy(fx->direction) * disp);
    px = clamp_int(px, 0, SCREEN_W - (GAME_TILE_SIZE * 2));
    py = clamp_int(py, 0, SCREEN_H - GAME_TILE_SIZE);
    sprite = kBlockSprites[block];
    make_bob_dirty_rect(px, py, rect);
    *key = dynamic_bob_key(
        sprite,
        px,
        py,
        ((ULONG)block & 0x0FUL) | (((ULONG)fx->style & 0x0FUL) << 4));
    if (out_px) *out_px = px;
    if (out_py) *out_py = py;
    if (out_sprite) *out_sprite = sprite;
    return true;
}

static bool make_block_dynamic(const MovingBlock *mb, DirtyRect *rect, ULONG *key, int *out_px, int *out_py, AmigaSpriteId *out_sprite) {
    int px;
    int py;
    AmigaSpriteId sprite;
    if (!mb->active || mb->type == BLOCK_NONE) {
        return false;
    }
    px = clamp_int(mb->pixel_fp_x >> 8, 0, SCREEN_W - 32);
    py = clamp_int(mb->pixel_fp_y >> 8, 0, SCREEN_H - GAME_TILE_SIZE);
    sprite = kBlockSprites[mb->type];
    make_bob_dirty_rect(px, py, rect);
    *key = dynamic_bob_key(sprite, px, py, (ULONG)mb->type);
    if (out_px) *out_px = px;
    if (out_py) *out_py = py;
    if (out_sprite) *out_sprite = sprite;
    return true;
}

static bool make_enemy_dynamic(const RendererState *rr, int buf, int index, const Enemy *enemy, DirtyRect *rect, ULONG *key, int *out_px, int *out_py, AmigaSpriteId *out_sprite) {
    int px;
    int py;
    AmigaSpriteId sprite;
    if (!enemy->alive || rr->enemy_hw_sprite[buf][index]) {
        return false;
    }
    px = clamp_int(enemy->pixel_fp_x >> 8, 0, SCREEN_W - 32);
    py = clamp_int(enemy->pixel_fp_y >> 8, 0, SCREEN_H - GAME_TILE_SIZE);
    sprite = enemy_sprite_for_state(enemy);
    make_bob_dirty_rect(px, py, rect);
    *key = dynamic_bob_key(sprite, px, py, ((ULONG)enemy->state & 0x0FUL) | (((ULONG)enemy->type & 0x03UL) << 4));
    if (out_px) *out_px = px;
    if (out_py) *out_py = py;
    if (out_sprite) *out_sprite = sprite;
    return true;
}

static bool make_player_dynamic(const Player *player, DirtyRect *rect, ULONG *key, int *out_px, int *out_py, AmigaSpriteId *out_sprite) {
    bool moving;
    int phase;
    bool alt;
    int px;
    int py;
    AmigaSpriteId sprite;
    if (!player->alive) {
        return false;
    }
    if (player->respawn_invuln_ticks > 0 && ((player->respawn_invuln_ticks / 4) & 1) != 0) {
        return false;
    }
    moving = (player->state == PLAYER_WALKING || player->state == PLAYER_PUSHING);
    phase = (player->pixel_fp_x + player->pixel_fp_y) >> 9;
    alt = moving && ((phase & 1) != 0);
    px = player->pixel_fp_x >> 8;
    py = player->pixel_fp_y >> 8;
    if (player->state == PLAYER_PUSHING && player->push_timer > 0) {
        px += dir_dx(player->facing);
        py += dir_dy(player->facing);
    }
    px = clamp_int(px, 0, SCREEN_W - 32);
    py = clamp_int(py, 0, SCREEN_H - GAME_TILE_SIZE);
    sprite = player_sprite_for_direction(player->facing, alt);
    make_bob_dirty_rect(px, py, rect);
    *key = dynamic_bob_key(sprite, px, py, ((ULONG)player->state & 0x0FUL) | (((ULONG)player->facing & 0x07UL) << 4));
    if (out_px) *out_px = px;
    if (out_py) *out_py = py;
    if (out_sprite) *out_sprite = sprite;
    return true;
}

static void prepare_dynamic_actors(RendererState *rr, int buf, const RenderState *rs) {
    int i;
    DirtyRect rect;
    ULONG key;
    AmigaSpriteId sprite;
    int px;
    int py;
    UBYTE settle_live = 0;
    UWORD settle_mask = (UWORD)(rs->active_impact_fx_mask | rr->cached_settle_mask[buf]);
    UWORD block_mask = (UWORD)(rs->active_moving_block_mask | rr->cached_block_mask[buf]);
    UWORD enemy_mask = (UWORD)(rs->alive_enemy_mask | rr->cached_enemy_mask[buf]);
    UWORD new_settle_mask = 0;
    UWORD new_block_mask = 0;
    UWORD new_enemy_mask = 0;

    if (settle_mask != 0u || rr->cached_settle_live[buf] != 0) {
        for (i = 0; settle_mask != 0u && i < GAME_MAX_IMPACT_FX; ++i, settle_mask >>= 1) {
            bool valid;
            if ((settle_mask & 1u) == 0u) {
                continue;
            }
            valid = make_settle_block_dynamic(rs, i, &rect, &key, &px, &py, &sprite);
            if (valid) {
                ++settle_live;
                new_settle_mask |= amiga_slot_bit(i);
            }
            prepare_dynamic_cache(rr, buf, &rr->cached_settle_block[buf][i], valid, key, valid ? &rect : 0, px, py, sprite);
        }
        rr->cached_settle_live[buf] = settle_live;
        rr->cached_settle_mask[buf] = new_settle_mask;
    }

    for (i = 0; block_mask != 0u && i < GAME_MAX_MOVING_BLOCKS; ++i, block_mask >>= 1) {
        bool valid;
        if ((block_mask & 1u) == 0u) {
            continue;
        }
        valid = make_block_dynamic(&rs->moving_blocks[i], &rect, &key, &px, &py, &sprite);
        if (valid) {
            new_block_mask |= amiga_slot_bit(i);
        }
        prepare_dynamic_cache(rr, buf, &rr->cached_block[buf][i], valid, key, valid ? &rect : 0, px, py, sprite);
    }
    rr->cached_block_mask[buf] = new_block_mask;

    for (i = 0; enemy_mask != 0u && i < rs->enemy_count; ++i, enemy_mask >>= 1) {
        bool valid;
        if ((enemy_mask & 1u) == 0u) {
            continue;
        }
        valid = make_enemy_dynamic(rr, buf, i, &rs->enemies[i], &rect, &key, &px, &py, &sprite);
        if (valid) {
            new_enemy_mask |= amiga_slot_bit(i);
        }
        prepare_dynamic_cache(rr, buf, &rr->cached_enemy[buf][i], valid, key, valid ? &rect : 0, px, py, sprite);
    }
    rr->cached_enemy_mask[buf] = new_enemy_mask;

    {
        bool valid = make_player_dynamic(&rs->player, &rect, &key, &px, &py, &sprite);
        prepare_dynamic_cache(rr, buf, &rr->cached_player[buf], valid, key, valid ? &rect : 0, px, py, sprite);
    }
}

static StaticCell make_static_cell(const RenderState *rs, const RendererState *rr, int x, int y) {
    StaticCell cell;
    const bool show_item = item_should_render(rs->bonus_item_timer_ticks);
    cell.terrain = (UBYTE)rs->terrain[y][x];
    cell.block = (UBYTE)rs->blocks[y][x];
    if (is_pushable_block((BlockType)cell.block) && settle_block_hidden_at(rr, x, y)) {
        cell.block = BLOCK_NONE;
    }
    cell.item = (UBYTE)(show_item ? rs->items[y][x] : ITEM_NONE);
    cell.mine_sprite = 0xFFu;
    if (rs->mines[y][x]) {
#if AMIGA_FAST_RENDER
        cell.mine_sprite = (UBYTE)mine_sprite_for_fuse_ticks(rs->mine_fuse_ticks[y][x], 0);
#else
        int phase = rs->round_time_ticks + (rs->timer_danger_pulse_ticks * 3) + x * 3 + y * 5;
        cell.mine_sprite = (UBYTE)mine_sprite_for_fuse_ticks(rs->mine_fuse_ticks[y][x], phase);
#endif
    }
    return cell;
}

static void draw_title_star_sparkle(UBYTE *screen, int cx, int cy, int phase) {
    put_pixel(screen, cx, cy, 31);
    if (phase >= 1) {
        put_pixel(screen, cx - 1, cy, 30);
        put_pixel(screen, cx + 1, cy, 30);
        put_pixel(screen, cx, cy - 1, 30);
        put_pixel(screen, cx, cy + 1, 30);
    }
    if (phase >= 2) {
        put_pixel(screen, cx - 2, cy, 29);
        put_pixel(screen, cx + 2, cy, 29);
        put_pixel(screen, cx, cy - 2, 29);
        put_pixel(screen, cx, cy + 2, 29);
        put_pixel(screen, cx - 1, cy - 1, 30);
        put_pixel(screen, cx + 1, cy - 1, 30);
        put_pixel(screen, cx - 1, cy + 1, 30);
        put_pixel(screen, cx + 1, cy + 1, 30);
    }
    if (phase >= 3) {
        put_pixel(screen, cx - 3, cy, 29);
        put_pixel(screen, cx + 3, cy, 29);
        put_pixel(screen, cx, cy - 3, 29);
        put_pixel(screen, cx, cy + 3, 29);
        put_pixel(screen, cx - 2, cy - 2, 29);
        put_pixel(screen, cx + 2, cy - 2, 29);
        put_pixel(screen, cx - 2, cy + 2, 29);
        put_pixel(screen, cx + 2, cy + 2, 29);
    }
}

static void draw_ice_block_sparkles(UBYTE *screen, int px, int py, int phase_seed) {
    int slow_phase = phase_seed >> 2;
    int wave = slow_phase & 127;
    int sparkle_phase = 0;
    int cx = px + 6;
    int cy = py + 6;

    if (wave < 4) {
        sparkle_phase = (wave < 2) ? (wave + 1) : (4 - wave);
    } else if (wave >= 64 && wave < 68) {
        sparkle_phase = (wave < 66) ? (wave - 63) : (68 - wave);
        cx = px + 10;
        cy = py + 9;
    }
    if (sparkle_phase > 0) {
        draw_title_star_sparkle(screen, cx, cy, sparkle_phase);
    }
}

static void draw_block_bevel(UBYTE *screen, int x, int y) {
    int i;
    for (i = 0; i < GAME_TILE_SIZE; ++i) {
        put_pixel(screen, x + i, y, 31);
        put_pixel(screen, x, y + i, 31);
        put_pixel(screen, x + i, y + GAME_TILE_SIZE - 1, 1);
        put_pixel(screen, x + GAME_TILE_SIZE - 1, y + i, 1);
    }
    for (i = 1; i < GAME_TILE_SIZE - 2; ++i) {
        put_pixel(screen, x + i, y + 1, 9);
        put_pixel(screen, x + 1, y + i, 9);
        put_pixel(screen, x + i, y + GAME_TILE_SIZE - 2, 3);
        put_pixel(screen, x + GAME_TILE_SIZE - 2, y + i, 3);
    }
}

static void draw_block_inner_state(UBYTE *screen, BlockType block, int px, int py, bool animate) {
    if (block == BLOCK_NONE) {
        return;
    }
    put_pixel(screen, px + 7, py + 7, 31);
    put_pixel(screen, px + 8, py + 8, 31);
    put_pixel(screen, px + 6, py + 8, 30);
    put_pixel(screen, px + 9, py + 7, 30);
    if (animate && block == BLOCK_ICE) {
        draw_ice_block_sparkles(screen, px, py, (int)g_ui_anim_tick + px * 5 + py * 3);
    }
}

static void draw_actor_drop_shadow(UBYTE *screen, int px, int py) {
    int x;
    int sy = py + GAME_TILE_SIZE - 2;
    for (x = 3; x <= 12; ++x) {
        put_pixel(screen, px + x, sy, (x & 1) ? 2 : 1);
    }
    for (x = 5; x <= 10; ++x) {
        put_pixel(screen, px + x, sy + 1, 1);
    }
}

static void draw_enemy_threat_corners(UBYTE *screen, int px, int py, int phase) {
    UBYTE color = ((phase & 1) == 0) ? 23 : 30;
    put_pixel(screen, px + 1, py + 1, color);
    put_pixel(screen, px + 2, py + 1, color);
    put_pixel(screen, px + 1, py + 2, color);
    put_pixel(screen, px + GAME_TILE_SIZE - 2, py + 1, color);
    put_pixel(screen, px + GAME_TILE_SIZE - 3, py + 1, color);
    put_pixel(screen, px + GAME_TILE_SIZE - 2, py + 2, color);
}

static void draw_enemy_spawn_cue(UBYTE *screen, int px, int py, int spawn_ticks) {
    int i;
    UBYTE ring;
    int x0;
    int y0;
    int x1;
    int y1;
    if (spawn_ticks <= 0) {
        return;
    }
    ring = (spawn_ticks & 4) ? 13 : 30;
    x0 = px;
    y0 = py;
    x1 = px + GAME_TILE_SIZE - 1;
    y1 = py + GAME_TILE_SIZE - 1;
    for (i = 0; i < 4; ++i) {
        put_pixel(screen, x0 + i, y0, ring);
        put_pixel(screen, x0, y0 + i, ring);
        put_pixel(screen, x1 - i, y0, ring);
        put_pixel(screen, x1, y0 + i, ring);
        put_pixel(screen, x0 + i, y1, ring);
        put_pixel(screen, x0, y1 - i, ring);
        put_pixel(screen, x1 - i, y1, ring);
        put_pixel(screen, x1, y1 - i, ring);
    }
    if (spawn_ticks < 10) {
        UBYTE core = (spawn_ticks & 2) ? 30 : 8;
        put_pixel(screen, px + 8, py + 8, core);
        put_pixel(screen, px + 7, py + 8, core);
        put_pixel(screen, px + 8, py + 7, core);
    }
}

static void draw_push_skid_marks(UBYTE *screen, int px, int py, Direction facing, int timer) {
    int blink = ((timer & 1) == 0) ? 1 : 0;
    switch (facing) {
        case DIR_RIGHT:
            put_pixel(screen, px + 2, py + 12, 2);
            put_pixel(screen, px + 1, py + 11, 30);
            if (blink) put_pixel(screen, px + 0, py + 12, 2);
            break;
        case DIR_LEFT:
            put_pixel(screen, px + 13, py + 12, 2);
            put_pixel(screen, px + 14, py + 11, 30);
            if (blink) put_pixel(screen, px + 15, py + 12, 2);
            break;
        case DIR_UP:
            put_pixel(screen, px + 7, py + 14, 2);
            put_pixel(screen, px + 8, py + 15, 30);
            if (blink) put_pixel(screen, px + 9, py + 14, 2);
            break;
        case DIR_DOWN:
            put_pixel(screen, px + 7, py + 2, 2);
            put_pixel(screen, px + 8, py + 1, 30);
            if (blink) put_pixel(screen, px + 9, py + 2, 2);
            break;
        default:
            break;
    }
}

static void render_static_cell(const AmigaApp *app, UBYTE *screen, const StaticCell *cell, int tx, int ty) {
    int px = tx * GAME_TILE_SIZE;
    int py = ty * GAME_TILE_SIZE;
    blit_opaque_tile(app, screen, kTerrainSprites[cell->terrain], px, py);
    if (cell->block != BLOCK_NONE) {
        blit_opaque_tile(app, screen, kBlockSprites[cell->block], px, py);
    }
    if (cell->item != ITEM_NONE) {
        blit_cookie_bob(app, screen, kItemSprites[cell->item], px, py);
    }
    if (cell->mine_sprite != 0xFFu) {
        blit_cookie_bob(app, screen, (AmigaSpriteId)cell->mine_sprite, px, py);
    }
}

static void update_static_dirty(RendererState *rr, const RenderState *rs, int buf) {
    if (!rr->prev_valid[buf]) {
        mark_all_dirty(rr, buf);
        rr->prev_valid[buf] = 1;
    }
}

static void mark_core_dirty_static(RendererState *rr, const GameState *gs) {
    const uint32_t *rows = game_dirty_static_rows(gs);
    int buf;
    int y;
    if (!rows) {
        return;
    }
    for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
        uint32_t row = rows[y];
        if (row == 0) {
            continue;
        }
        for (buf = 0; buf < 2; ++buf) {
            rr->dirty_rows[buf][y] |= (ULONG)row;
            rr->static_dirty_rows[buf][y] |= (ULONG)row;
            rr->dirty_any[buf] = 1;
        }
    }
}

static int find_sparkle_cell(const RendererState *rr, int x, int y) {
    UWORD i;
    for (i = 0; i < rr->sparkle_cell_count; ++i) {
        if ((int)rr->sparkle_cell_x[i] == x && (int)rr->sparkle_cell_y[i] == y) {
            return (int)i;
        }
    }
    return -1;
}

static void set_sparkle_cell(RendererState *rr, int x, int y, bool enabled) {
    int slot = find_sparkle_cell(rr, x, y);
    if (enabled) {
        if (slot >= 0 || rr->sparkle_cell_count >= STATIC_SPARKLE_CELL_MAX) {
            return;
        }
        rr->sparkle_cell_x[rr->sparkle_cell_count] = (UBYTE)x;
        rr->sparkle_cell_y[rr->sparkle_cell_count] = (UBYTE)y;
        ++rr->sparkle_cell_count;
        return;
    }
    if (slot >= 0) {
        UWORD last = (UWORD)(rr->sparkle_cell_count - 1);
        if ((UWORD)slot != last) {
            rr->sparkle_cell_x[slot] = rr->sparkle_cell_x[last];
            rr->sparkle_cell_y[slot] = rr->sparkle_cell_y[last];
        }
        rr->sparkle_cell_x[last] = 0;
        rr->sparkle_cell_y[last] = 0;
        --rr->sparkle_cell_count;
        if (rr->sparkle_cursor >= rr->sparkle_cell_count) {
            rr->sparkle_cursor = 0;
        }
    }
}

static void render_dirty_static(const AmigaApp *app, UBYTE *screen, int buf, const RenderState *rs) {
    int x;
    int y;
    RendererState *rr = (RendererState *)&app->renderer;
    bool force_backdrop = !rr->backdrop_valid;
    if (!force_backdrop && !rr->dirty_any[buf]) {
        return;
    }
    for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
        ULONG row = rr->dirty_rows[buf][y];
        ULONG static_row = rr->static_dirty_rows[buf][y];
        if (row == 0) {
            continue;
        }
        rr->dirty_rows[buf][y] = 0;
        rr->static_dirty_rows[buf][y] = 0;
        x = 0;
        while (row != 0 && x < GAME_GRID_WIDTH) {
            int start_x;
            int end_x;
            while (x < GAME_GRID_WIDTH && (row & 1UL) == 0) {
                row >>= 1;
                ++x;
            }
            if (x >= GAME_GRID_WIDTH || row == 0) {
                break;
            }
            start_x = x;
            do {
                if (force_backdrop || (static_row & ((ULONG)1UL << (ULONG)x)) != 0) {
                    StaticCell cell = make_static_cell(rs, rr, x, y);
                    if (force_backdrop || !cells_equal(&rr->prev_static[buf][y][x], &cell)) {
                        render_static_cell(app, app->chip_backdrop, &cell, x, y);
#if AMIGA_USE_GAMEPLAY_HW_SPARKLES
                        set_sparkle_cell(rr, x, y, cell.block == BLOCK_ICE);
#endif
                        rr->prev_static[buf][y][x] = cell;
                    }
                }
                row >>= 1;
                ++x;
            } while (x < GAME_GRID_WIDTH && (row & 1UL) != 0);
            end_x = x - 1;
            blit_screen_rect(
                screen,
                app->chip_backdrop,
                start_x * GAME_TILE_SIZE,
                y * GAME_TILE_SIZE,
                (end_x - start_x + 1) * GAME_TILE_SIZE,
                GAME_TILE_SIZE);
        }
    }
    rr->dirty_any[buf] = 0;
    rr->backdrop_valid = 1;
}

#if AMIGA_USE_GAMEPLAY_HW_SPARKLES
static bool sparkle_sprite_pos_for_ice_block(int px, int py, ULONG phase_seed, int *out_x, int *out_y) {
    int slow_phase = (int)(phase_seed >> 2);
    int wave = slow_phase & 127;
    int cx = px + 6;
    int cy = py + 6;

    if (wave < 4) {
        /* Visible on the same ticks as the PC star sparkle; the hardware sprite carries the shape. */
    } else if (wave >= 64 && wave < 68) {
        cx = px + 10;
        cy = py + 9;
    } else {
        return false;
    }

    *out_x = clamp_int(cx - 8, 0, SCREEN_W - HW_SPRITE_HEIGHT);
    *out_y = clamp_int(cy - 8, 0, SCREEN_H - HW_SPRITE_HEIGHT);
    return true;
}
#endif

static int add_hw_sparkle_sprite(const AmigaApp *app, int buf, int count, int x, int y) {
#if AMIGA_HW_SPARKLES_ENABLED
    if (count >= HW_SPARKLE_COUNT) {
        return count;
    }
    write_hw_sparkle_sprite(app, buf, HW_SPARKLE_FIRST_SLOT + count, x, y);
    return count + 1;
#else
    (void)app;
    (void)buf;
    (void)x;
    (void)y;
    return count;
#endif
}

#if AMIGA_USE_TITLE_HW_SPARKLES
static int prepare_title_hardware_sparkles(const AmigaApp *app, int buf) {
    static const int sparkle_anchors[5][2] = {
        {88, 16},
        {170, 14},
        {36, 30},
        {124, 31},
        {198, 32}
    };
    int count = 0;
    int draw_x = (SCREEN_W / 2) - (AMIGA_TITLE_OVERLAY_WIDTH / 2) + AMIGA_TITLE_OVERLAY_ANCHOR_X;
    int draw_y = 10 + AMIGA_TITLE_OVERLAY_ANCHOR_Y;
    int i;

    for (i = 0; i < 5 && count < HW_SPARKLE_COUNT; ++i) {
        int wave = (int)(((g_ui_anim_tick / 2UL) + (ULONG)(i * 5)) & 15UL);
        if (wave > 4) {
            continue;
        }
        {
            int sx = draw_x + ((sparkle_anchors[i][0] * AMIGA_TITLE_OVERLAY_WIDTH) / 220) - 8;
            int sy = draw_y + ((sparkle_anchors[i][1] * AMIGA_TITLE_OVERLAY_HEIGHT) / 48) - 8;
            count = add_hw_sparkle_sprite(app, buf, count, sx, sy);
        }
    }
    return count;
}
#endif

#if AMIGA_USE_GAMEPLAY_HW_SPARKLES
static int prepare_gameplay_hardware_sparkles(const AmigaApp *app, RendererState *rr, int buf, const RenderState *rs) {
    int count = 0;
    int i;
    UWORD moving_mask = (UWORD)rs->active_moving_block_mask;

    for (i = 0; moving_mask != 0u && i < GAME_MAX_MOVING_BLOCKS && count < HW_SPARKLE_COUNT; ++i, moving_mask >>= 1) {
        const MovingBlock *mb = &rs->moving_blocks[i];
        int px;
        int py;
        int sx;
        int sy;
        if ((moving_mask & 1u) == 0u || !mb->active || mb->type != BLOCK_ICE) {
            continue;
        }
        px = mb->pixel_fp_x >> 8;
        py = mb->pixel_fp_y >> 8;
        if (sparkle_sprite_pos_for_ice_block(px, py, g_ui_anim_tick + (ULONG)((mb->pixel_fp_x + mb->pixel_fp_y) >> 7), &sx, &sy)) {
            count = add_hw_sparkle_sprite(app, buf, count, sx, sy);
        }
    }

    {
        UWORD idx = rr->sparkle_cursor;
        UWORD probes = 0;
        UWORD max_probes = rr->sparkle_cell_count;
        if (max_probes > HW_SPARKLE_STATIC_PROBES) {
            max_probes = HW_SPARKLE_STATIC_PROBES;
        }
        if (idx >= rr->sparkle_cell_count) {
            idx = 0;
        }
        while (probes < max_probes && count < HW_SPARKLE_COUNT) {
            int x = (int)rr->sparkle_cell_x[idx];
            int y = (int)rr->sparkle_cell_y[idx];
            int px;
            int py;
            int sx;
            int sy;
            if (rs->blocks[y][x] == BLOCK_ICE &&
                (rs->active_impact_fx_count <= 0 || !settle_block_hidden_at(rr, x, y))) {
                px = x * GAME_TILE_SIZE;
                py = y * GAME_TILE_SIZE;
                if (sparkle_sprite_pos_for_ice_block(px, py, g_ui_anim_tick + (ULONG)(px * 5 + py * 3), &sx, &sy)) {
                    count = add_hw_sparkle_sprite(app, buf, count, sx, sy);
                }
            }
            ++idx;
            if (idx >= rr->sparkle_cell_count) {
                idx = 0;
            }
            ++probes;
        }
        rr->sparkle_cursor = idx;
    }
    return count;
}
#endif

static BOOL prepare_enemy_hardware_sprites(AmigaApp *app, RendererState *rr, int buf, const RenderState *rs, bool disabled) {
#if AMIGA_HW_SPRITES_ENABLED
#if !AMIGA_USE_HW_SPRITES && !AMIGA_USE_GAMEPLAY_HW_SPARKLES
    if (!title_hw_sparkles_active(rs) && rr->sparkle_hw_count[buf] == 0) {
        return FALSE;
    }
#endif
#if AMIGA_USE_HW_SPRITES
    clear_hw_sprite_bank(app->hw_sprite_bank[buf]);
    memset(rr->enemy_hw_sprite[buf], 0, sizeof(rr->enemy_hw_sprite[buf]));
    if (!disabled) {
    int i;
    int layer_index = 0;
    UWORD enemy_mask = (UWORD)rs->alive_enemy_mask;
    for (i = 0; enemy_mask != 0u && i < rs->enemy_count && layer_index < HW_LAYERED_ENEMY_COUNT; ++i, enemy_mask >>= 1) {
        const Enemy *enemy = &rs->enemies[i];
        AmigaSpriteId sprite;
        int px;
        int py;
        if ((enemy_mask & 1u) == 0u || !enemy->alive) {
            continue;
        }
        sprite = enemy_sprite_for_state(enemy);
        if (!enemy_sprite_can_use_hardware(sprite)) {
            continue;
        }
        px = clamp_int(enemy->pixel_fp_x >> 8, 0, SCREEN_W - GAME_TILE_SIZE);
        py = clamp_int(enemy->pixel_fp_y >> 8, 0, GAMEPLAY_H - GAME_TILE_SIZE);
        write_hw_enemy_sprite_layers(app, buf, layer_index, sprite, px, py);
        rr->enemy_hw_sprite[buf][i] = 1;
        ++layer_index;
    }
    }
#elif AMIGA_HW_SPARKLES_ENABLED
    {
        UBYTE i;
        for (i = 0; i < rr->sparkle_hw_count[buf]; ++i) {
            clear_hw_sprite_slot(app->hw_sprite_bank[buf], HW_SPARKLE_FIRST_SLOT + (int)i);
        }
        rr->sparkle_hw_count[buf] = 0;
    }
#endif

#if AMIGA_USE_TITLE_HW_SPARKLES
    if (title_hw_sparkles_active(rs)) {
        rr->sparkle_hw_count[buf] = (UBYTE)prepare_title_hardware_sparkles(app, buf);
        return TRUE;
    }
#endif
#if AMIGA_USE_GAMEPLAY_HW_SPARKLES
    if (!disabled && rs->phase == GAME_PHASE_PLAYING) {
        rr->sparkle_hw_count[buf] = (UBYTE)prepare_gameplay_hardware_sparkles(app, rr, buf, rs);
        return TRUE;
    }
#endif
    return TRUE;
#else
    /* Hardware sprite path is compiled out for the A500 BOB-only build. */
    (void)app;
    (void)rr;
    (void)buf;
    (void)rs;
    (void)disabled;
    return FALSE;
#endif
}

static void draw_cpu_spark(UBYTE *screen, int cx, int cy, UBYTE color) {
    put_pixel(screen, cx, cy, color);
    put_pixel(screen, cx - 1, cy, color);
    put_pixel(screen, cx + 1, cy, color);
    put_pixel(screen, cx, cy - 1, color);
    put_pixel(screen, cx, cy + 1, color);
}

static void draw_score_popups(const AmigaApp *app, RendererState *rr, int buf, UBYTE *screen, const RenderState *rs) {
    int i;
    UWORD mask = (UWORD)rs->active_score_popup_mask;
    if (mask == 0u) {
        return;
    }
    wait_blitter();
    for (i = 0; mask != 0u && i < GAME_MAX_SCORE_POPUPS; ++i, mask >>= 1) {
        const ScorePopup *popup = &rs->score_popups[i];
        int value;
        int width;
        int drift_x;
        int pop_y_offset;
        int px;
        int py;
        if ((mask & 1u) == 0u || !popup->active || popup->ttl_ticks <= 0) {
            continue;
        }
        if (popup->ttl_ticks < 10 && ((popup->ttl_ticks & 1) != 0)) {
            continue;
        }
        value = popup->value;
        if (value < 0) {
            value = 0;
        }
        if (value > 9999) {
            value = 9999;
        }
        width = number_width((uint32_t)value, 0);
        drift_x = ((i & 1) == 0) ? -2 : 2;
        pop_y_offset = (i & 1) + ((popup->ttl_ticks / 6) & 1);
        px = clamp_int((popup->pixel_fp_x >> 8) - (width / 2) + drift_x, 1, SCREEN_W - width - 2);
        py = clamp_int((popup->pixel_fp_y >> 8) - 3 - pop_y_offset, 1, GAMEPLAY_H - 8);
        save_dynamic_rect(app, rr, buf, screen, px - 2, py - 2, width + 4, 10);
        draw_number_outlined(screen, px, py, (uint32_t)value, 0, (popup->value >= 800) ? 8 : ((popup->value >= 400) ? 12 : 30), 1);
    }
}

static void draw_impact_fx(const AmigaApp *app, RendererState *rr, int buf, UBYTE *screen, const RenderState *rs) {
    int i;
    UWORD mask = (UWORD)rs->active_impact_fx_mask;
    if (mask == 0u) {
        return;
    }
    for (i = 0; mask != 0u && i < GAME_MAX_IMPACT_FX; ++i, mask >>= 1) {
        const ImpactFx *fx = &rs->impact_fx[i];
        int cx;
        int cy;
        bool death_style;
        if ((mask & 1u) == 0u || !fx->active || fx->ttl_ticks <= 0) {
            continue;
        }
        if (fx->ttl_ticks < 4 && ((fx->ttl_ticks & 1) != 0)) {
            continue;
        }
        cx = fx->pixel_fp_x >> 8;
        cy = fx->pixel_fp_y >> 8;
        death_style = fx->style == IMPACT_FX_STYLE_ENEMY_DEATH_A ||
                      fx->style == IMPACT_FX_STYLE_ENEMY_DEATH_B;
        if (death_style) {
            int age_ticks = fx->base_ttl_ticks - fx->ttl_ticks;
            int px = clamp_int(cx - 8, 0, SCREEN_W - 32);
            int py = clamp_int(cy - 8 + ((age_ticks > 2) ? ((age_ticks - 2) / 4) : 0), 0, SCREEN_H - GAME_TILE_SIZE);
            save_dynamic_rect(app, rr, buf, screen, px, py, 32, 16);
            blit_cookie_bob(app, screen, death_sprite_for_fx(fx), px, py);
        } else {
#if AMIGA_FAST_RENDER
            if (fx->style == IMPACT_FX_STYLE_MINE_BLAST) {
                int age_ticks = fx->base_ttl_ticks - fx->ttl_ticks;
                int jitter = (age_ticks & 1) ? 1 : 0;
                int px = clamp_int(cx - 8 + jitter, 0, SCREEN_W - 32);
                int py = clamp_int(cy - 8 - jitter, 0, SCREEN_H - GAME_TILE_SIZE);
                DirtyRect rect;
                make_bob_dirty_rect(px, py, &rect);
                save_dynamic_rect(app, rr, buf, screen, rect.x, rect.y, rect.w, rect.h);
                blit_cookie_bob(app, screen, AMIGA_SPR_EFFECT_SPARK, px, py);
            }
            continue;
#else
            int spread = 1 + ((12 - fx->ttl_ticks) / 3);
            UBYTE hot = (fx->ttl_ticks > 6) ? 30 : 8;
            UBYTE cool = (fx->ttl_ticks > 5) ? 29 : 24;
            if (fx->style == IMPACT_FX_STYLE_SPECIAL_STOP || fx->style == IMPACT_FX_STYLE_SPECIAL_CRUSH) {
                hot = (fx->ttl_ticks > 6) ? 8 : 30;
                cool = (fx->ttl_ticks > 5) ? 13 : 24;
                spread += 1;
            } else if (fx->style == IMPACT_FX_STYLE_CRACKED_STOP || fx->style == IMPACT_FX_STYLE_CRACKED_CRUSH) {
                hot = (fx->ttl_ticks > 5) ? 21 : 22;
                cool = 2;
            } else if (fx->style == IMPACT_FX_STYLE_MINE_BLAST) {
                spread = clamp_int(2 + ((fx->base_ttl_ticks - fx->ttl_ticks) / 2), 2, 5);
                hot = ((fx->ttl_ticks & 1) == 0) ? 8 : 30;
                cool = (fx->ttl_ticks > 6) ? 29 : 24;
            }
            save_dynamic_rect(app, rr, buf, screen, cx - 8, cy - 8, 17, 17);
            wait_blitter();
            put_pixel(screen, cx, cy, hot);
            put_pixel(screen, cx - spread, cy, hot);
            put_pixel(screen, cx + spread, cy, hot);
            put_pixel(screen, cx, cy - spread, cool);
            put_pixel(screen, cx, cy + spread, cool);
            if ((fx->ttl_ticks & 1) == 0) {
                put_pixel(screen, cx - spread, cy - spread, cool);
                put_pixel(screen, cx + spread, cy + spread, cool);
                put_pixel(screen, cx + spread, cy - spread, cool);
                put_pixel(screen, cx - spread, cy + spread, cool);
            }
            if (fx->style == IMPACT_FX_STYLE_MINE_BLAST) {
                int ring = spread + 1;
                put_pixel(screen, cx - ring, cy, hot);
                put_pixel(screen, cx + ring, cy, hot);
                put_pixel(screen, cx, cy - ring, hot);
                put_pixel(screen, cx, cy + ring, hot);
            }
#endif
        }
    }
}

static void draw_dynamic_actors(const AmigaApp *app, RendererState *rr, int buf, UBYTE *screen, const RenderState *rs) {
    int i;
    UWORD mask;

    mask = rr->cached_settle_mask[buf];
    for (i = 0; mask != 0u && i < GAME_MAX_IMPACT_FX; ++i, mask >>= 1) {
        DynamicCache *cache = &rr->cached_settle_block[buf][i];
        if ((mask & 1u) == 0u || !cache->next_valid) {
            continue;
        }
        if (cache->valid &&
            cache->key == cache->next_key &&
            dirty_rects_same(&cache->rect, &cache->next_rect)) {
            continue;
        }
        blit_cookie_bob(app, screen, (AmigaSpriteId)cache->next_sprite, cache->next_x, cache->next_y);
        cache->rect = cache->next_rect;
        cache->key = cache->next_key;
        cache->valid = 1;
    }

    mask = rr->cached_block_mask[buf];
    for (i = 0; mask != 0u && i < GAME_MAX_MOVING_BLOCKS; ++i, mask >>= 1) {
        DynamicCache *cache = &rr->cached_block[buf][i];
        if ((mask & 1u) == 0u || !cache->next_valid) {
            continue;
        }
        if (cache->valid &&
            cache->key == cache->next_key &&
            dirty_rects_same(&cache->rect, &cache->next_rect)) {
            continue;
        }
        blit_cookie_bob(app, screen, (AmigaSpriteId)cache->next_sprite, cache->next_x, cache->next_y);
        cache->rect = cache->next_rect;
        cache->key = cache->next_key;
        cache->valid = 1;
    }

    mask = rr->cached_enemy_mask[buf];
    for (i = 0; mask != 0u && i < rs->enemy_count; ++i, mask >>= 1) {
        const Enemy *enemy = &rs->enemies[i];
        DynamicCache *cache = &rr->cached_enemy[buf][i];
        if ((mask & 1u) == 0u || !cache->next_valid) {
            continue;
        }
        if (cache->valid &&
            cache->key == cache->next_key &&
            dirty_rects_same(&cache->rect, &cache->next_rect)) {
            continue;
        }
        blit_cookie_bob(app, screen, (AmigaSpriteId)cache->next_sprite, cache->next_x, cache->next_y);
        if (enemy->state == ENEMY_SPAWNING && enemy->spawn_ticks > 0) {
#if !AMIGA_FAST_RENDER
            wait_blitter();
            draw_enemy_spawn_cue(screen, cache->next_x, cache->next_y, enemy->spawn_ticks);
#endif
        } else {
#if !AMIGA_FAST_RENDER
            wait_blitter();
            draw_enemy_threat_corners(screen, cache->next_x, cache->next_y, enemy->anim_frame + enemy->tile_x + enemy->tile_y);
#endif
        }
        cache->rect = cache->next_rect;
        cache->key = cache->next_key;
        cache->valid = 1;
    }

    {
        DynamicCache *cache = &rr->cached_player[buf];
        if (cache->next_valid) {
            if (cache->valid &&
                cache->key == cache->next_key &&
                dirty_rects_same(&cache->rect, &cache->next_rect)) {
                return;
            }
#if !AMIGA_FAST_RENDER
            wait_blitter();
            if (rs->player.state == PLAYER_PUSHING && rs->player.push_timer > 0) {
                draw_push_skid_marks(screen, cache->next_x, cache->next_y, rs->player.facing, rs->player.push_timer);
            }
#endif
            blit_cookie_bob(app, screen, (AmigaSpriteId)cache->next_sprite, cache->next_x, cache->next_y);
            cache->rect = cache->next_rect;
            cache->key = cache->next_key;
            cache->valid = 1;
        }
    }
}

static void draw_hud_separator(UBYTE *screen, int x, UBYTE dark, UBYTE light) {
    int y;
    for (y = 193; y < SCREEN_H - 1; ++y) {
        put_pixel(screen, x, y, dark);
        put_pixel(screen, x + 1, y, light);
    }
}

static void draw_hud_bomb_meter(UBYTE *screen, int x, int y, int mine_stock, int mine_capacity, int anim_phase) {
    int i;
    int stock = clamp_int(mine_stock, 0, 8);
    int capacity = clamp_int(mine_capacity, 0, 8);
    for (i = 0; i < 8; ++i) {
        int cell_x = x + (i % 4) * 3;
        int cell_y = y + (i / 4) * 3;
        UBYTE color = 2;
        if (i < capacity) {
            color = 3;
        }
        if (i < stock) {
#if AMIGA_FAST_RENDER
            color = 30;
#else
            color = ((((anim_phase / 4) + i) & 1) == 0) ? 30 : 22;
#endif
        }
        put_pixel(screen, cell_x, cell_y, color);
        put_pixel(screen, cell_x + 1, cell_y, color);
        put_pixel(screen, cell_x, cell_y + 1, color);
        put_pixel(screen, cell_x + 1, cell_y + 1, color);
    }
}

static UBYTE hud_timer_color(const RenderState *rs) {
    return rs->timer_danger_active ? (((rs->round_time_ticks / 4) & 1) ? 23 : 12) : 14;
}

static UBYTE hud_bomb_color(const RenderState *rs) {
    if (rs->run_mine_capacity <= 0) {
        return 3;
    }
    if (rs->run_mine_stock <= 0) {
        return 23;
    }
#if AMIGA_FAST_RENDER
    return 30;
#else
    return (((rs->round_time_ticks / 5) & 1) == 0) ? 30 : 22;
#endif
}

static void clear_hud_field(UBYTE *screen, int x0, int x1) {
    draw_rect(screen, x0, 192, x1, SCREEN_H - 2, 1);
}

static int format_hud_number(uint32_t value, int min_digits, char *out, int out_cap) {
    char tmp[10];
    int count = 0;
    int i;
    if (out_cap <= 0) {
        return 0;
    }
    do {
        tmp[count++] = (char)('0' + (value % 10u));
        value /= 10u;
    } while (value > 0u && count < (int)sizeof(tmp));
    while (count < min_digits && count < (int)sizeof(tmp)) {
        tmp[count++] = '0';
    }
    if (count >= out_cap) {
        count = out_cap - 1;
    }
    for (i = 0; i < count; ++i) {
        out[i] = tmp[count - 1 - i];
    }
    out[count] = '\0';
    return count;
}

static void clear_hud_char_cell(UBYTE *screen, int x) {
    draw_rect(screen, x - 1, 192, x + 3, 198, 1);
}

static void draw_hud_number_delta(
    UBYTE *screen,
    int x,
    uint32_t old_value,
    uint32_t new_value,
    int min_digits,
    UBYTE color,
    UBYTE outline,
    bool force_all) {
    char old_digits[11];
    char new_digits[11];
    UBYTE redraw[10];
    int old_len;
    int new_len;
    int i;

    old_len = format_hud_number(old_value, min_digits, old_digits, (int)sizeof(old_digits));
    new_len = format_hud_number(new_value, min_digits, new_digits, (int)sizeof(new_digits));
    if (force_all || old_len != new_len) {
        int max_len = old_len > new_len ? old_len : new_len;
        clear_hud_field(screen, x - 1, x + max_len * 4);
        draw_number_outlined(screen, x, 193, new_value, min_digits, color, outline);
        return;
    }

    memset(redraw, 0, sizeof(redraw));
    for (i = 0; i < new_len; ++i) {
        if (old_digits[i] != new_digits[i]) {
            if (i > 0) {
                redraw[i - 1] = 1;
            }
            redraw[i] = 1;
            if (i + 1 < new_len) {
                redraw[i + 1] = 1;
            }
        }
    }
    for (i = 0; i < new_len; ++i) {
        if (redraw[i]) {
            clear_hud_char_cell(screen, x + i * 4);
        }
    }
    for (i = 0; i < new_len; ++i) {
        if (redraw[i]) {
            draw_char_outlined_fast(screen, x + i * 4, 193, new_digits[i], color, outline);
        }
    }
}

static void draw_hud_values(UBYTE *screen, const RenderState *rs) {
    int timer_seconds = rs->round_time_ticks > 0 ? (rs->round_time_ticks / GAME_FIXED_TICK_HZ) : 0;
    UBYTE timer_color = hud_timer_color(rs);
    UBYTE bomb_color = hud_bomb_color(rs);

    draw_number_outlined(screen, 34, 193, rs->score > 99999999u ? 99999999u : rs->score, 6, 29, 1);
    draw_number_outlined(screen, 161, 193, (uint32_t)rs->round, 2, 18, 1);
    draw_number_outlined(screen, 201, 193, (uint32_t)timer_seconds, 2, timer_color, 1);
    draw_number_outlined(screen, 244, 193, (uint32_t)clamp_int(rs->run_mine_stock, 0, 99), 1, bomb_color, 1);
    draw_text_outlined(screen, 248, 193, ":", 7, 1);
    draw_number_outlined(screen, 252, 193, (uint32_t)clamp_int(rs->run_mine_capacity, 0, 99), 1, bomb_color, 1);
    draw_hud_bomb_meter(screen, 255, 193, rs->run_mine_stock, rs->run_mine_capacity, rs->round_time_ticks);
    draw_number_outlined(screen, 287, 193, (uint32_t)clamp_int(rs->lives, 0, 99), 2, 23, 1);
}

static void draw_hud_delta(UBYTE *screen, const RenderState *rs, const HudSnapshot *old_hud, const HudSnapshot *new_hud) {
    wait_blitter();
    if (old_hud->score != new_hud->score) {
        draw_hud_number_delta(screen, 34, old_hud->score, new_hud->score, 6, 29, 1, false);
    }
    if (old_hud->round != new_hud->round) {
        draw_hud_number_delta(screen, 161, (uint32_t)old_hud->round, (uint32_t)new_hud->round, 2, 18, 1, false);
    }
    if (old_hud->timer_seconds != new_hud->timer_seconds ||
        old_hud->timer_danger != new_hud->timer_danger ||
        old_hud->timer_flash_phase != new_hud->timer_flash_phase) {
        draw_hud_number_delta(
            screen,
            201,
            (uint32_t)old_hud->timer_seconds,
            (uint32_t)new_hud->timer_seconds,
            2,
            hud_timer_color(rs),
            1,
            (old_hud->timer_danger != new_hud->timer_danger ||
             old_hud->timer_flash_phase != new_hud->timer_flash_phase));
    }
    if (old_hud->mine_stock != new_hud->mine_stock ||
        old_hud->mine_capacity != new_hud->mine_capacity ||
        old_hud->bomb_flash_phase != new_hud->bomb_flash_phase) {
        UBYTE bomb_color = hud_bomb_color(rs);
        clear_hud_field(screen, 243, 265);
        draw_number_outlined(screen, 244, 193, (uint32_t)new_hud->mine_stock, 1, bomb_color, 1);
        draw_text_outlined(screen, 248, 193, ":", 7, 1);
        draw_number_outlined(screen, 252, 193, (uint32_t)new_hud->mine_capacity, 1, bomb_color, 1);
        draw_hud_bomb_meter(screen, 255, 193, rs->run_mine_stock, rs->run_mine_capacity, rs->round_time_ticks);
    }
    if (old_hud->lives != new_hud->lives) {
        draw_hud_number_delta(screen, 287, (uint32_t)old_hud->lives, (uint32_t)new_hud->lives, 2, 23, 1, false);
    }
}

static void draw_hud(UBYTE *screen, const RenderState *rs) {
    int x;
    int y;

    wait_blitter();
    draw_rect(screen, 0, 192, SCREEN_W - 1, SCREEN_H - 1, 1);
    for (x = 0; x < SCREEN_W; ++x) {
        put_pixel(screen, x, 192, 3);
        put_pixel(screen, x, SCREEN_H - 1, 2);
    }
    for (y = 193; y < SCREEN_H - 1; ++y) {
        if ((y & 1) == 0) {
            for (x = 0; x < SCREEN_W; x += 32) {
                put_pixel(screen, x, y, 2);
            }
        }
    }
    draw_hud_separator(screen, 142, 2, 3);
    draw_hud_separator(screen, 174, 2, 3);
    draw_hud_separator(screen, 218, 2, 3);
    draw_hud_separator(screen, 266, 2, 3);

    draw_text_outlined(screen, 4, 193, "SCORE", 7, 1);
    draw_text_outlined(screen, 148, 193, "RD", 7, 1);
    draw_text_outlined(screen, 180, 193, "TIME", 7, 1);
    draw_text_outlined(screen, 224, 193, "BOMB", 7, 1);
    draw_text_outlined(screen, 271, 193, "LIV", 7, 1);
    draw_hud_values(screen, rs);
}

static HudSnapshot make_hud_snapshot(const RenderState *rs) {
    HudSnapshot hud;
    hud.score = rs->score > 99999999u ? 99999999u : rs->score;
    hud.round = (UWORD)clamp_int(rs->round, 0, 999);
    hud.timer_seconds = (UWORD)((rs->round_time_ticks > 0) ? (rs->round_time_ticks / GAME_FIXED_TICK_HZ) : 0);
    hud.lives = (UWORD)clamp_int(rs->lives, 0, 99);
    hud.mine_stock = (UWORD)clamp_int(rs->run_mine_stock, 0, 99);
    hud.mine_capacity = (UWORD)clamp_int(rs->run_mine_capacity, 0, 99);
    hud.timer_danger = rs->timer_danger_active ? 1u : 0u;
    hud.timer_flash_phase = hud.timer_danger ? (UBYTE)((rs->round_time_ticks / 4) & 1) : 0u;
#if AMIGA_FAST_RENDER
    hud.bomb_flash_phase = 0u;
#else
    hud.bomb_flash_phase = (rs->run_mine_capacity > 0 && rs->run_mine_stock > 0)
                               ? (UBYTE)((((rs->round_time_ticks / 4) & 1) << 1) | ((rs->round_time_ticks / 5) & 1))
                               : 0u;
#endif
    hud.special_border_phase = 0u;
    return hud;
}

ICE_INLINE bool hud_snapshot_equal(const HudSnapshot *a, const HudSnapshot *b) {
    return a->score == b->score &&
           a->round == b->round &&
           a->timer_seconds == b->timer_seconds &&
           a->lives == b->lives &&
           a->mine_stock == b->mine_stock &&
           a->mine_capacity == b->mine_capacity &&
           a->timer_danger == b->timer_danger &&
           a->timer_flash_phase == b->timer_flash_phase &&
           a->bomb_flash_phase == b->bomb_flash_phase &&
           a->special_border_phase == b->special_border_phase;
}

static void draw_panel(UBYTE *screen, int x0, int y0, int x1, int y1, UBYTE accent) {
    int panel_w = x1 - x0 + 1;
    int panel_h = y1 - y0 + 1;
#if !AMIGA_FAST_RENDER
    int perimeter = ((panel_w + panel_h) * 2) - 4;
#endif
    draw_rect(screen, x0, y0, x1, y1, 1);
    draw_rect(screen, x0, y0, x1, y0, 30);
    draw_rect(screen, x0, y1, x1, y1, 30);
    draw_rect(screen, x0, y0, x0, y1, 30);
    draw_rect(screen, x1, y0, x1, y1, 30);
    draw_rect(screen, x0 + 2, y0 + 2, x1 - 2, y0 + 2, accent);
    draw_rect(screen, x0 + 2, y1 - 2, x1 - 2, y1 - 2, accent);

#if !AMIGA_FAST_RENDER
    if (perimeter > 0) {
        int trail;
        int head = (int)((g_ui_anim_tick / 2) % (ULONG)perimeter);
        for (trail = 0; trail < 5; ++trail) {
            int t = head - trail;
            int sx = x0;
            int sy = y0;
            UBYTE color = (trail == 0) ? 31 : ((trail == 1) ? 30 : ((trail < 4) ? accent : 2));
            while (t < 0) {
                t += perimeter;
            }
            t %= perimeter;
            if (t < panel_w) {
                sx = x0 + t;
                sy = y0;
            } else if (t < panel_w + panel_h - 1) {
                sx = x1;
                sy = y0 + (t - panel_w) + 1;
            } else if (t < panel_w + panel_h + panel_w - 2) {
                sx = x1 - (t - panel_w - (panel_h - 1)) - 1;
                sy = y1;
            } else {
                sx = x0;
                sy = y1 - (t - panel_w - (panel_h - 1) - (panel_w - 1)) - 1;
            }
            put_pixel(screen, sx, sy, color);
        }
    }
#endif
}

static void draw_card_frame(UBYTE *screen, int x0, int y0, int w, int h, UBYTE accent, bool selected) {
    int x;
    int y;
    int x1 = x0 + w - 1;
    int y1 = y0 + h - 1;
    UBYTE outer = selected ? 30 : 3;
    UBYTE inner = selected ? accent : 2;

    draw_rect(screen, x0, y0, x1, y1, 1);
    draw_hline(screen, x0, x1, y0, outer);
    draw_hline(screen, x0, x1, y1, outer);
    draw_rect(screen, x0, y0, x0, y1, outer);
    draw_rect(screen, x1, y0, x1, y1, outer);

    draw_hline(screen, x0 + 1, x1 - 1, y0 + 1, inner);
    draw_hline(screen, x0 + 1, x1 - 1, y1 - 1, inner);
    draw_rect(screen, x0 + 1, y0 + 1, x0 + 1, y1 - 1, inner);
    draw_rect(screen, x1 - 1, y0 + 1, x1 - 1, y1 - 1, inner);

#if !AMIGA_FAST_RENDER
    for (y = y0 + 5; y <= y1 - 5; y += 4) {
        int offset = ((y - y0) & 4) ? 6 : 2;
        for (x = x0 + offset; x <= x1 - 5; x += 8) {
            put_pixel(screen, x, y, 2);
        }
    }
#endif

    if (selected && y0 >= 3) {
        int cx = x0 + (w / 2);
        put_pixel(screen, cx, y0 - 3, 30);
        put_pixel(screen, cx - 1, y0 - 2, accent);
        put_pixel(screen, cx, y0 - 2, accent);
        put_pixel(screen, cx + 1, y0 - 2, accent);
        put_pixel(screen, cx, y0 - 1, 30);
    }
}

static void draw_card_selection_chrome(UBYTE *screen, int x0, int y0, int w, int h, UBYTE accent, bool selected) {
    int x1 = x0 + w - 1;
    int y1 = y0 + h - 1;
    UBYTE outer = selected ? 30 : 3;
    UBYTE inner = selected ? accent : 2;

    draw_hline(screen, x0, x1, y0, outer);
    draw_hline(screen, x0, x1, y1, outer);
    draw_rect(screen, x0, y0, x0, y1, outer);
    draw_rect(screen, x1, y0, x1, y1, outer);
    draw_rect(screen, x0 + 1, y0 + 1, x0 + 1, y1 - 1, inner);
    draw_rect(screen, x1 - 1, y0 + 1, x1 - 1, y1 - 1, inner);

    if (y0 >= 3) {
        int cx = x0 + (w / 2);
        draw_rect(screen, cx - 2, y0 - 3, cx + 2, y0 - 1, 1);
        if (selected) {
            put_pixel(screen, cx, y0 - 3, 30);
            put_pixel(screen, cx - 1, y0 - 2, accent);
            put_pixel(screen, cx, y0 - 2, accent);
            put_pixel(screen, cx + 1, y0 - 2, accent);
            put_pixel(screen, cx, y0 - 1, 30);
        }
    }
}

static void draw_opening_high_scores(UBYTE *screen, const HighScoreTable *table) {
    int i;
    const int x0 = 86;
    const int x1 = 233;
    const int y0 = 96;
    const int y1 = 144;
    const int left_x = x0 + 10;
    const int score_right = x1 - 10;
    const HighScoreTable *scores = table ? table : &g_high_scores;

    draw_panel(screen, x0, y0, x1, y1, 16);
    draw_centered_text(screen, x0, x1, y0 + 6, "HI SCORES", 30);
    for (i = 0; i < HIGH_SCORE_COUNT; ++i) {
        int row_y = y0 + 14 + i * 6;
        draw_number(screen, left_x, row_y, (uint32_t)(i + 1), 1, 29);
        draw_text(screen, left_x + 8, row_y, scores->entries[i].initials, 29);
        draw_number_right(screen, score_right, row_y, scores->entries[i].score, 5, 30);
    }
}

static void draw_visual_arrow_glyph(UBYTE *screen, int x, int y, Direction dir, UBYTE color) {
    switch (dir) {
        case DIR_LEFT:
            put_pixel(screen, x + 0, y + 2, color);
            put_pixel(screen, x + 1, y + 1, color);
            put_pixel(screen, x + 1, y + 2, color);
            put_pixel(screen, x + 1, y + 3, color);
            put_pixel(screen, x + 2, y + 2, color);
            put_pixel(screen, x + 3, y + 2, color);
            break;
        case DIR_RIGHT:
            put_pixel(screen, x + 0, y + 2, color);
            put_pixel(screen, x + 1, y + 2, color);
            put_pixel(screen, x + 2, y + 1, color);
            put_pixel(screen, x + 2, y + 2, color);
            put_pixel(screen, x + 2, y + 3, color);
            put_pixel(screen, x + 3, y + 2, color);
            break;
        case DIR_UP:
            put_pixel(screen, x + 1, y + 0, color);
            put_pixel(screen, x + 2, y + 0, color);
            put_pixel(screen, x + 0, y + 1, color);
            put_pixel(screen, x + 1, y + 1, color);
            put_pixel(screen, x + 2, y + 1, color);
            put_pixel(screen, x + 3, y + 1, color);
            put_pixel(screen, x + 1, y + 2, color);
            put_pixel(screen, x + 2, y + 2, color);
            put_pixel(screen, x + 1, y + 3, color);
            put_pixel(screen, x + 2, y + 3, color);
            break;
        case DIR_DOWN:
            put_pixel(screen, x + 1, y + 0, color);
            put_pixel(screen, x + 2, y + 0, color);
            put_pixel(screen, x + 1, y + 1, color);
            put_pixel(screen, x + 2, y + 1, color);
            put_pixel(screen, x + 0, y + 2, color);
            put_pixel(screen, x + 1, y + 2, color);
            put_pixel(screen, x + 2, y + 2, color);
            put_pixel(screen, x + 3, y + 2, color);
            put_pixel(screen, x + 1, y + 3, color);
            put_pixel(screen, x + 2, y + 3, color);
            break;
        default:
            break;
    }
}

static void draw_high_score_entry_overlay(UBYTE *screen, const HighScoreEntryState *entry) {
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
    int i;
    bool blink_on;

    if (!entry || !entry->active) {
        return;
    }
    blink_on = (((entry->blink_ticks / 6u) & 1u) == 0u) ? true : false;

    draw_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 16);
    draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y0 + 8, "NEW HI SCORE", 31, 1);
    draw_text_outlined(screen, panel_x0 + 84, panel_y0 + 17, "RANK", 30, 1);
    draw_number_outlined(screen, panel_x0 + 104, panel_y0 + 17, (uint32_t)(entry->insert_index + 1), 1, 30, 1);
    draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y0 + 25, "SCORE", 30, 1);
    draw_number_outlined(
        screen,
        panel_x0 + ((panel_x1 - panel_x0 + 1 - number_width(entry->pending_score, 1)) / 2),
        panel_y0 + 33,
        entry->pending_score,
        1,
        8,
        1);
    draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y0 + 43, "ENTER INITIALS", 30, 1);

    for (i = 0; i < 3; ++i) {
        bool selected = (i == entry->cursor);
        int slot_x = slots_x0 + i * (slot_w + slot_spacing);
        char letter[2];
        draw_card_frame(screen, slot_x, slots_y, slot_w, slot_h, selected ? 29 : 17, selected);
        letter[0] = entry->initials[i];
        letter[1] = '\0';
        if (!selected || blink_on) {
            int glyph_w = text_width_scaled(letter, 3);
            draw_text_scaled_outlined(
                screen,
                slot_x + ((slot_w - glyph_w) / 2),
                slots_y + 5,
                letter,
                selected ? 31 : 30,
                1,
                3);
        }
    }

    {
        int slot_x = slots_x0 + entry->cursor * (slot_w + slot_spacing);
        int center_x = slot_x + (slot_w / 2) - 2;
        draw_visual_arrow_glyph(screen, center_x, slots_y - 7, DIR_UP, 30);
        draw_visual_arrow_glyph(screen, center_x, slots_y + slot_h + 2, DIR_DOWN, 30);
    }
    draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y1 - 18, "ARROWS EDIT", 30, 1);
    draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y1 - 10, "FIRE SAVE", 31, 1);
}

static void draw_continue_chevrons(UBYTE *screen, int x, int y, UBYTE color_a, UBYTE color_b) {
#if AMIGA_FAST_RENDER
    UBYTE color = (UBYTE)(color_a | (color_b & 0u));
#else
    UBYTE color = (((g_ui_anim_tick / 6) & 1u) == 0u) ? color_a : color_b;
#endif
    draw_visual_arrow_glyph(screen, x + 0, y, DIR_RIGHT, color);
    draw_visual_arrow_glyph(screen, x + 5, y, DIR_RIGHT, color);
    draw_visual_arrow_glyph(screen, x + 10, y, DIR_RIGHT, color);
}

static void draw_round_intro_visual_lane(const AmigaApp *app, UBYTE *screen, const RenderState *rs, int lane_x0, int lane_y0, int lane_w, int lane_h) {
    int lane_y = lane_y0 + ((lane_h - GAME_TILE_SIZE) / 2);
    int left_x = lane_x0 + 8;
    int right_x = lane_x0 + lane_w - GAME_TILE_SIZE - 8;
    int span = right_x - left_x;
    int block_x = left_x + (span / 3);
    int enemy_x = left_x + ((span * 2) / 3);
    int mine_x = right_x;
#if AMIGA_FAST_RENDER
    UBYTE cue = 30;
#else
    UBYTE cue = ((((g_ui_anim_tick / 5) & 1u) == 0u) ? 30 : 14);
#endif

    blit_cookie_bob(app, screen, player_sprite_for_direction(DIR_RIGHT, false), left_x, lane_y);
    wait_blitter();
    draw_visual_arrow_glyph(screen, left_x + 6, lane_y - 4, DIR_UP, cue);
    draw_visual_arrow_glyph(screen, left_x + 6, lane_y + 16, DIR_DOWN, cue);
    draw_visual_arrow_glyph(screen, left_x - 5, lane_y + 6, DIR_LEFT, cue);
    draw_visual_arrow_glyph(screen, left_x + 17, lane_y + 6, DIR_RIGHT, cue);

    blit_cookie_bob(app, screen, AMIGA_SPR_BLOCK_ICE, block_x, lane_y);
    wait_blitter();
#if !AMIGA_FAST_RENDER
    draw_block_bevel(screen, block_x, lane_y);
    draw_ice_block_sparkles(screen, block_x, lane_y, (int)g_ui_anim_tick + block_x * 5 + lane_y * 3);
#endif
    draw_visual_arrow_glyph(screen, ((block_x + enemy_x) / 2) - 2, lane_y + 6, DIR_RIGHT, cue);

#if AMIGA_FAST_RENDER
    blit_cookie_bob(app, screen, AMIGA_SPR_ENEMY_A, enemy_x, lane_y);
#else
    blit_cookie_bob(app, screen, (((g_ui_anim_tick / 8) & 1u) != 0u) ? AMIGA_SPR_ENEMY_A_ALT : AMIGA_SPR_ENEMY_A, enemy_x, lane_y);
    if (((g_ui_anim_tick / 4) & 1u) == 0u) {
        blit_cookie_bob(app, screen, AMIGA_SPR_EFFECT_SPARK, enemy_x + 2, lane_y - 2);
    }
#endif

    if (rs->run_mine_capacity > 0) {
#if AMIGA_FAST_RENDER
        AmigaSpriteId mine_sprite = AMIGA_SPR_MINE_1;
#else
        AmigaSpriteId mine_sprite = (((g_ui_anim_tick / 6) & 1u) == 0u) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_2;
#endif
        blit_cookie_bob(app, screen, mine_sprite, mine_x, lane_y);
        wait_blitter();
        draw_visual_arrow_glyph(screen, mine_x - 7, lane_y + 6, DIR_LEFT, cue);
    } else {
        blit_cookie_bob(app, screen, AMIGA_SPR_MINE_0, mine_x, lane_y);
    }
    wait_blitter();
}

ICE_INLINE int perk_choice_count_clamped(const RenderState *rs) {
    int count = rs->perk_choice_count;
    if (count < 1) {
        count = 1;
    }
    if (count > GAME_PERK_CHOICE_COUNT) {
        count = GAME_PERK_CHOICE_COUNT;
    }
    return count;
}

ICE_INLINE int perk_choice_base_x(int count) {
    const int panel_x0 = 48;
    const int panel_x1 = 271;
    const int card_w = 58;
    const int spacing = 8;
    int total_w = count * card_w + (count - 1) * spacing;
    return panel_x0 + ((panel_x1 - panel_x0 + 1 - total_w) / 2);
}

static void draw_perk_choice_card(const AmigaApp *app, UBYTE *screen, const RenderState *rs, int index, int base_x) {
    const int card_w = 58;
    const int card_h = 44;
    const int spacing = 8;
    int px = base_x + index * (card_w + spacing);
    int py = 75;
    PerkType perk = rs->perk_choices[index];
    bool selected_card = (index == rs->perk_choice_selected);
    UBYTE accent = perk_color(perk);

    draw_rect(screen, px - 1, py - 4, px + card_w, py + card_h, 1);
    draw_card_frame(screen, px, py, card_w, card_h, accent, selected_card);
    blit_cookie_bob(app, screen, perk_sprite(perk), px + ((card_w - GAME_TILE_SIZE) / 2), py + 5);
    wait_blitter();
    draw_centered_text_outlined(screen, px + 2, px + card_w - 3, py + 24, perk_label(perk), accent, 1);
    draw_text_outlined(screen, px + 18, py + 31, "LV", 30, 1);
    draw_number_outlined(screen, px + 30, py + 31, (uint32_t)(rs->perk_levels[perk] + 1), 1, 30, 1);
}

static void draw_perk_choice_card_selection_delta(UBYTE *screen, const RenderState *rs, int index, int base_x) {
    const int card_w = 58;
    const int card_h = 44;
    const int spacing = 8;
    int px = base_x + index * (card_w + spacing);
    int py = 75;
    PerkType perk = rs->perk_choices[index];
    draw_card_selection_chrome(screen, px, py, card_w, card_h, perk_color(perk), index == rs->perk_choice_selected);
}

static void draw_perk_choice_footer(UBYTE *screen, const RenderState *rs, int count) {
    const int panel_x0 = 48;
    const int panel_x1 = 271;
    int selected_idx = rs->perk_choice_selected;
    PerkType selected;
    UBYTE selected_color;

    if (selected_idx < 0 || selected_idx >= count) {
        selected_idx = 0;
    }
    selected = rs->perk_choices[selected_idx];
    selected_color = perk_color(selected);
    draw_rect(screen, panel_x0 + 4, 122, panel_x1 - 4, 140, 1);
    draw_centered_text_outlined(screen, panel_x0, panel_x1, 124, perk_label(selected), selected_color, 1);
    draw_centered_text_outlined(screen, panel_x0, panel_x1, 130, perk_effect_label(selected), 30, 1);
    draw_visual_arrow_glyph(screen, panel_x0 + 8, 135, DIR_LEFT, 30);
    draw_visual_arrow_glyph(screen, panel_x1 - 12, 135, DIR_RIGHT, 30);
    draw_continue_chevrons(screen, panel_x0 + ((panel_x1 - panel_x0) / 2) - 7, 135, selected_color, 30);
}

static void draw_perk_choice_panel(const AmigaApp *app, UBYTE *screen, const RenderState *rs) {
    int i;
    const int panel_x0 = 48;
    const int panel_x1 = 271;
    const int panel_y0 = 58;
    const int panel_y1 = 143;
    int count = perk_choice_count_clamped(rs);
    int base_x = perk_choice_base_x(count);

    draw_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 18);
    draw_centered_text_outlined(screen, panel_x0, panel_x1, 64, "PICK PERK", 31, 1);

    for (i = 0; i < count; ++i) {
        draw_perk_choice_card(app, screen, rs, i, base_x);
    }

    draw_perk_choice_footer(screen, rs, count);
}

static void draw_perk_choice_delta(UBYTE *screen, const RenderState *rs, int previous_selection) {
    int count = perk_choice_count_clamped(rs);
    int base_x = perk_choice_base_x(count);
    int selected = rs->perk_choice_selected;

    if (selected < 0 || selected >= count) {
        selected = 0;
    }
    if (previous_selection >= 0 && previous_selection < count && previous_selection != selected) {
        draw_perk_choice_card_selection_delta(screen, rs, previous_selection, base_x);
    }
    draw_perk_choice_card_selection_delta(screen, rs, selected, base_x);
    draw_perk_choice_footer(screen, rs, count);
}

ICE_INLINE int meta_choice_count_clamped(const RenderState *rs) {
    int count = rs->meta_choice_count;
    if (count < 1) {
        count = 1;
    }
    if (count > GAME_META_CHOICE_COUNT) {
        count = GAME_META_CHOICE_COUNT;
    }
    return count;
}

ICE_INLINE int meta_choice_base_x(int count) {
    const int panel_x0 = 36;
    const int panel_x1 = 283;
    const int card_w = 48;
    const int spacing = 8;
    int total_w = count * card_w + (count - 1) * spacing;
    return panel_x0 + ((panel_x1 - panel_x0 + 1 - total_w) / 2);
}

static void draw_meta_choice_card(const AmigaApp *app, UBYTE *screen, const RenderState *rs, int index, int base_x) {
    const int card_w = 48;
    const int card_h = 44;
    const int spacing = 8;
    int px = base_x + index * (card_w + spacing);
    int py = 75;
    MetaUpgradeType upgrade = rs->meta_choices[index];
    int item_cost = rs->meta_choice_costs[index];
    bool card_selected = (index == rs->meta_choice_selected);
    bool card_affordable = rs->meta_shards >= (uint32_t)((item_cost < 0) ? 0 : item_cost);
    UBYTE accent = card_affordable ? meta_color(upgrade) : 23;

    draw_rect(screen, px - 1, py - 4, px + card_w, py + card_h, 1);
    draw_card_frame(screen, px, py, card_w, card_h, accent, card_selected);
    blit_cookie_bob(app, screen, meta_sprite(upgrade), px + ((card_w - GAME_TILE_SIZE) / 2), py + 5);
    wait_blitter();
    draw_centered_text_outlined(screen, px + 2, px + card_w - 3, py + 24, meta_label(upgrade), accent, 1);
    draw_text_outlined(screen, px + 14, py + 31, "C", card_affordable ? 30 : 23, 1);
    draw_number_outlined(screen, px + 22, py + 31, (uint32_t)clamp_int(item_cost, 0, 999), 1, card_affordable ? 30 : 23, 1);
}

static void draw_meta_choice_card_selection_delta(UBYTE *screen, const RenderState *rs, int index, int base_x) {
    const int card_w = 48;
    const int card_h = 44;
    const int spacing = 8;
    int px = base_x + index * (card_w + spacing);
    int py = 75;
    MetaUpgradeType upgrade = rs->meta_choices[index];
    int item_cost = rs->meta_choice_costs[index];
    bool card_affordable = rs->meta_shards >= (uint32_t)((item_cost < 0) ? 0 : item_cost);
    UBYTE accent = card_affordable ? meta_color(upgrade) : 23;
    draw_card_selection_chrome(screen, px, py, card_w, card_h, accent, index == rs->meta_choice_selected);
}

static void draw_meta_choice_footer(UBYTE *screen, const RenderState *rs, int count) {
    const int panel_x0 = 36;
    const int panel_x1 = 283;
    int selected_idx = rs->meta_choice_selected;
    MetaUpgradeType selected;
    int cost;
    bool affordable;
    UBYTE selected_color;

    if (selected_idx < 0 || selected_idx >= count) {
        selected_idx = 0;
    }
    selected = rs->meta_choices[selected_idx];
    cost = rs->meta_choice_costs[selected_idx];
    affordable = rs->meta_shards >= (uint32_t)((cost < 0) ? 0 : cost);
    selected_color = affordable ? 30 : 23;
    draw_rect(screen, panel_x0 + 4, 122, panel_x1 - 4, 140, 1);
    draw_centered_text_outlined(screen, panel_x0, panel_x1, 124, meta_label(selected), affordable ? meta_color(selected) : 23, 1);
    draw_centered_text_outlined(screen, panel_x0, panel_x1, 130, meta_effect_label(selected), selected_color, 1);
    draw_visual_arrow_glyph(screen, panel_x0 + 8, 135, DIR_LEFT, 30);
    draw_visual_arrow_glyph(screen, panel_x1 - 12, 135, DIR_RIGHT, 30);
    draw_continue_chevrons(screen, panel_x0 + ((panel_x1 - panel_x0) / 2) - 7, 135, selected_color, affordable ? 16 : 2);
}

static void draw_meta_choice_panel(const AmigaApp *app, UBYTE *screen, const RenderState *rs) {
    int i;
    const int panel_x0 = 36;
    const int panel_x1 = 283;
    const int panel_y0 = 58;
    const int panel_y1 = 143;
    int count = meta_choice_count_clamped(rs);
    int base_x = meta_choice_base_x(count);

    draw_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 16);
    draw_centered_text_outlined(screen, panel_x0, panel_x1, 64, "META UPGRADE", 31, 1);
    draw_centered_text_outlined(screen, panel_x0, panel_x1, 70, "SHARDS", 30, 1);
    draw_number_right(screen, panel_x1 - 10, 70, rs->meta_shards, 1, 8);

    for (i = 0; i < count; ++i) {
        draw_meta_choice_card(app, screen, rs, i, base_x);
    }

    draw_meta_choice_footer(screen, rs, count);
}

static void draw_meta_choice_delta(UBYTE *screen, const RenderState *rs, int previous_selection) {
    int count = meta_choice_count_clamped(rs);
    int base_x = meta_choice_base_x(count);
    int selected = rs->meta_choice_selected;

    if (selected < 0 || selected >= count) {
        selected = 0;
    }
    if (previous_selection >= 0 && previous_selection < count && previous_selection != selected) {
        draw_meta_choice_card_selection_delta(screen, rs, previous_selection, base_x);
    }
    draw_meta_choice_card_selection_delta(screen, rs, selected, base_x);
    draw_meta_choice_footer(screen, rs, count);
}

static void draw_title_overlay_art(const AmigaApp *app, UBYTE *screen, int center_x, int top_y) {
#if !AMIGA_FAST_RENDER
    static const int sparkle_anchors[5][2] = {
        {88, 16},
        {170, 14},
        {36, 30},
        {124, 31},
        {198, 32}
    };
    int bob = ((((g_ui_anim_tick / 6) & 1u) == 0u) ? 0 : 1);
#endif
    int draw_x = center_x - (AMIGA_TITLE_OVERLAY_WIDTH / 2) + AMIGA_TITLE_OVERLAY_ANCHOR_X;
#if AMIGA_FAST_RENDER
    int draw_y = top_y + AMIGA_TITLE_OVERLAY_ANCHOR_Y;
#else
    int draw_y = top_y + AMIGA_TITLE_OVERLAY_ANCHOR_Y + bob;
    int i;
#endif

    blit_title_overlay(app, screen, draw_x, draw_y);
    wait_blitter();

#if !AMIGA_FAST_RENDER
    for (i = 0; i < 5; ++i) {
        int sx = draw_x + ((sparkle_anchors[i][0] * AMIGA_TITLE_OVERLAY_WIDTH) / 220);
        int sy = draw_y + ((sparkle_anchors[i][1] * AMIGA_TITLE_OVERLAY_HEIGHT) / 48);
        int wave = (int)(((g_ui_anim_tick / 2) + (ULONG)(i * 5)) & 15u);
        int phase = 0;
        if (wave < 3) {
            phase = wave + 1;
        } else if (wave == 3) {
            phase = 2;
        } else if (wave == 4) {
            phase = 1;
        }
        if (phase > 0) {
            draw_title_star_sparkle(screen, sx, sy, phase);
        }
    }
#endif
}

ICE_INLINE ULONG mix_key(ULONG key, ULONG value) {
    return (key * 16777619UL) ^ value;
}

ICE_INLINE ULONG modal_anim_key_for_state(const RenderState *rs) {
#if AMIGA_FAST_RENDER
    return ((ULONG)rs->phase) & 0UL;
#else
    if (rs->phase == GAME_PHASE_PLAYER_DYING) {
        return 0;
    }
    if (rs->phase == GAME_PHASE_PLAYING && !rs->start_title_pending) {
        return 0;
    }
    return (ULONG)((g_ui_anim_tick / 6UL) & 15UL);
#endif
}

static DirtyRect modal_rect_for_state(const RenderState *rs) {
    DirtyRect r;
    r.x = 0;
    r.y = 0;
    r.w = 0;
    r.h = 0;

    if (g_high_score_entry.active) {
        r.x = 50;
        r.y = 46;
        r.w = 220;
        r.h = 108;
        return r;
    }

    if (rs->phase == GAME_PHASE_ROUND_INTRO && rs->start_title_pending) {
        r.x = 60;
        r.y = 0;
        r.w = 228;
        r.h = 184;
        return r;
    }

    switch (rs->phase) {
        case GAME_PHASE_ROUND_INTRO:
            r.x = 44;
            r.y = 54;
            r.w = 232;
            r.h = 94;
            break;
        case GAME_PHASE_PLAYER_DYING:
            r.x = 76;
            r.y = 78;
            r.w = 168;
            r.h = 40;
            break;
        case GAME_PHASE_ROUND_CLEAR:
            r.x = 56;
            r.y = 70;
            r.w = 208;
            r.h = 74;
            break;
        case GAME_PHASE_GAME_OVER:
            r.x = 52;
            r.y = 66;
            r.w = 216;
            r.h = 84;
            break;
        case GAME_PHASE_PERK_CHOICE:
            r.x = 44;
            r.y = 54;
            r.w = 232;
            r.h = 94;
            break;
        case GAME_PHASE_META_UPGRADE:
            r.x = 32;
            r.y = 54;
            r.w = 256;
            r.h = 94;
            break;
        default:
            break;
    }
    return r;
}

static ULONG modal_key_for_state(const RenderState *rs) {
    ULONG key = 2166136261UL;
    int i;
    if (g_high_score_entry.active) {
        key = mix_key(key, 0x48534cUL);
        key = mix_key(key, (ULONG)g_high_score_entry.insert_index);
        key = mix_key(key, g_high_score_entry.pending_score);
        key = mix_key(key, (ULONG)g_high_score_entry.cursor);
        key = mix_key(key, (ULONG)(UBYTE)g_high_score_entry.initials[0]);
        key = mix_key(key, (ULONG)(UBYTE)g_high_score_entry.initials[1]);
        key = mix_key(key, (ULONG)(UBYTE)g_high_score_entry.initials[2]);
        return key;
    }
    key = mix_key(key, (ULONG)rs->phase);
    key = mix_key(key, rs->start_title_pending ? 1UL : 0UL);
    key = mix_key(key, (ULONG)rs->round);
    key = mix_key(key, modal_anim_key_for_state(rs));

    if (rs->phase == GAME_PHASE_ROUND_INTRO && rs->start_title_pending) {
        return key;
    }

    switch (rs->phase) {
        case GAME_PHASE_ROUND_CLEAR:
            key = mix_key(key, (rs->round_clear_bonus_score > 0) ? (ULONG)rs->round_clear_bonus_score : 0UL);
            break;
        case GAME_PHASE_GAME_OVER:
            key = mix_key(key, rs->score > 99999999u ? 99999999u : rs->score);
            key = mix_key(key, (ULONG)rs->run_shards);
            key = mix_key(key, (rs->phase_timer_ticks > 0) ? 1UL : 0UL);
            break;
        case GAME_PHASE_PERK_CHOICE:
            key = mix_key(key, (ULONG)rs->perk_choice_count);
            key = mix_key(key, (ULONG)rs->perk_choice_selected);
            for (i = 0; i < rs->perk_choice_count && i < GAME_PERK_CHOICE_COUNT; ++i) {
                PerkType perk = rs->perk_choices[i];
                key = mix_key(key, (ULONG)perk);
                key = mix_key(key, (ULONG)rs->perk_levels[perk]);
            }
            break;
        case GAME_PHASE_META_UPGRADE:
            key = mix_key(key, (ULONG)rs->meta_choice_count);
            key = mix_key(key, (ULONG)rs->meta_choice_selected);
            key = mix_key(key, rs->meta_shards);
            for (i = 0; i < rs->meta_choice_count && i < GAME_META_CHOICE_COUNT; ++i) {
                key = mix_key(key, (ULONG)rs->meta_choices[i]);
                key = mix_key(key, (ULONG)rs->meta_choice_costs[i]);
            }
            break;
        default:
            break;
    }
    return key;
}

static ULONG modal_layout_key_for_state(const RenderState *rs) {
    ULONG key = 2166136261UL;
    int i;
    if (g_high_score_entry.active) {
        key = mix_key(key, 0x48534cUL);
        key = mix_key(key, (ULONG)g_high_score_entry.insert_index);
        key = mix_key(key, g_high_score_entry.pending_score);
        return key;
    }
    key = mix_key(key, (ULONG)rs->phase);
    key = mix_key(key, rs->start_title_pending ? 1UL : 0UL);
    key = mix_key(key, (ULONG)rs->round);
    key = mix_key(key, modal_anim_key_for_state(rs));

    if (rs->phase == GAME_PHASE_ROUND_INTRO && rs->start_title_pending) {
        return key;
    }

    switch (rs->phase) {
        case GAME_PHASE_ROUND_CLEAR:
            key = mix_key(key, (rs->round_clear_bonus_score > 0) ? (ULONG)rs->round_clear_bonus_score : 0UL);
            break;
        case GAME_PHASE_GAME_OVER:
            key = mix_key(key, rs->score > 99999999u ? 99999999u : rs->score);
            key = mix_key(key, (ULONG)rs->run_shards);
            key = mix_key(key, (rs->phase_timer_ticks > 0) ? 1UL : 0UL);
            break;
        case GAME_PHASE_PERK_CHOICE:
            key = mix_key(key, (ULONG)rs->perk_choice_count);
            for (i = 0; i < rs->perk_choice_count && i < GAME_PERK_CHOICE_COUNT; ++i) {
                PerkType perk = rs->perk_choices[i];
                key = mix_key(key, (ULONG)perk);
                key = mix_key(key, (ULONG)rs->perk_levels[perk]);
            }
            break;
        case GAME_PHASE_META_UPGRADE:
            key = mix_key(key, (ULONG)rs->meta_choice_count);
            key = mix_key(key, rs->meta_shards);
            for (i = 0; i < rs->meta_choice_count && i < GAME_META_CHOICE_COUNT; ++i) {
                key = mix_key(key, (ULONG)rs->meta_choices[i]);
                key = mix_key(key, (ULONG)rs->meta_choice_costs[i]);
            }
            break;
        default:
            break;
    }
    return key;
}

static WORD modal_selection_for_state(const RenderState *rs) {
    int count;
    int selection;
    if (g_high_score_entry.active) {
        return g_high_score_entry.cursor;
    }
    switch (rs->phase) {
        case GAME_PHASE_PERK_CHOICE:
            count = perk_choice_count_clamped(rs);
            selection = rs->perk_choice_selected;
            break;
        case GAME_PHASE_META_UPGRADE:
            count = meta_choice_count_clamped(rs);
            selection = rs->meta_choice_selected;
            break;
        default:
            return -1;
    }
    if (selection < 0 || selection >= count) {
        selection = 0;
    }
    return (WORD)selection;
}

ICE_INLINE bool modal_selection_delta_supported(const RenderState *rs) {
    return rs->phase == GAME_PHASE_PERK_CHOICE || rs->phase == GAME_PHASE_META_UPGRADE;
}

static bool draw_gameplay_flash_borders(UBYTE *screen, const RenderState *rs) {
#if AMIGA_FAST_RENDER
    return false;
#else
    bool drew = false;
    int x;
    int y;
    if (rs->stage_modifier != STAGE_MOD_NONE &&
        rs->stage_modifier_flash_ticks > 0 &&
        ((rs->stage_modifier_flash_ticks / 3) & 1) == 0) {
        UBYTE color = stage_modifier_color(rs->stage_modifier);
        for (x = 2; x < SCREEN_W - 2; ++x) {
            put_pixel(screen, x, 2, color);
            put_pixel(screen, x, 189, color);
        }
        drew = true;
    }
    if (rs->special_alignment_flash_ticks > 0 && ((rs->special_alignment_flash_ticks / 3) & 1) == 0) {
        for (x = 0; x < SCREEN_W; ++x) {
            put_pixel(screen, x, 0, 8);
            put_pixel(screen, x, SCREEN_H - 1, 8);
        }
        for (y = 0; y < SCREEN_H; ++y) {
            put_pixel(screen, 0, y, 8);
            put_pixel(screen, SCREEN_W - 1, y, 8);
        }
        drew = true;
    }
    if (rs->phase == GAME_PHASE_PLAYING && rs->timer_danger_pulse_ticks > 0) {
        for (x = 0; x < SCREEN_W; ++x) {
            put_pixel(screen, x, 0, 23);
            put_pixel(screen, x, GAMEPLAY_H - 1, 23);
        }
        for (y = 0; y < GAMEPLAY_H; ++y) {
            put_pixel(screen, 0, y, 23);
            put_pixel(screen, SCREEN_W - 1, y, 23);
        }
        drew = true;
    }
    return drew;
#endif
}

static bool gameplay_flash_active(const RenderState *rs) {
#if AMIGA_FAST_RENDER
    return false;
#else
    return (rs->stage_modifier != STAGE_MOD_NONE &&
            rs->stage_modifier_flash_ticks > 0 &&
            ((rs->stage_modifier_flash_ticks / 3) & 1) == 0) ||
           (rs->special_alignment_flash_ticks > 0 &&
            ((rs->special_alignment_flash_ticks / 3) & 1) == 0) ||
           (rs->phase == GAME_PHASE_PLAYING && rs->timer_danger_pulse_ticks > 0);
#endif
}

static bool overlay_active(const RenderState *rs) {
    return g_high_score_entry.active || rs->phase != GAME_PHASE_PLAYING || rs->start_title_pending;
}

ICE_INLINE bool dirty_rect_equal(const DirtyRect *a, const DirtyRect *b) {
    return a->x == b->x &&
           a->y == b->y &&
           a->w == b->w &&
           a->h == b->h;
}

static bool draw_overlay(const AmigaApp *app, UBYTE *screen, const RenderState *rs) {
    bool touched_gameplay;

    if (!overlay_active(rs)) {
        if (!gameplay_flash_active(rs)) {
            return false;
        }
        wait_blitter();
        touched_gameplay = draw_gameplay_flash_borders(screen, rs);
        return touched_gameplay;
    }

    wait_blitter();
    if (g_high_score_entry.active) {
        draw_high_score_entry_overlay(screen, &g_high_score_entry);
        return true;
    }

    if (rs->phase == GAME_PHASE_ROUND_INTRO && rs->start_title_pending) {
        draw_title_overlay_art(app, screen, SCREEN_W / 2, 10);
        draw_centered_text_outlined(
            screen,
            0,
            SCREEN_W - 1,
            84,
            "RETROFOUNDRY",
#if AMIGA_FAST_RENDER
            30,
#else
            ((((g_ui_anim_tick / 14) & 1u) == 0u) ? 29 : 30),
#endif
            1);
        draw_opening_high_scores(screen, &g_high_scores);

        draw_panel(screen, 70, 148, 249, 178, 14);
        draw_centered_text_outlined(screen, 70, 249, 155, "ROUND", 30, 1);
        draw_number_outlined(screen, 164, 155, (uint32_t)rs->round, 2, 31, 1);
        draw_centered_text_outlined(screen, 70, 249, 163, "FIRE START", 31, 1);
        draw_continue_chevrons(screen, 70 + ((249 - 70) / 2) - 7, 171, 30, 14);
        return true;
    }

    if (rs->phase == GAME_PHASE_ROUND_INTRO) {
        const int panel_x0 = 48;
        const int panel_x1 = 271;
        const int panel_y0 = 58;
        const int panel_y1 = 143;
        const int panel_w = panel_x1 - panel_x0 + 1;
        const int round_w = number_width((uint32_t)rs->round, 2);
        const int round_x = panel_x0 + ((panel_w - round_w) / 2);
        const int lane_card_x = panel_x0 + 14;
        const int lane_card_y = panel_y0 + 24;
        const int lane_card_w = panel_w - 28;
        const int lane_card_h = 44;
        draw_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 14);
        draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y0 + 8, "ROUND", 31, 1);
        draw_number_outlined(screen, round_x, panel_y0 + 16, (uint32_t)rs->round, 2, 31, 1);
        draw_card_frame(screen, lane_card_x, lane_card_y, lane_card_w, lane_card_h, 14, true);
        draw_round_intro_visual_lane(
            app,
            screen,
            rs,
            lane_card_x + 6,
            lane_card_y + 4,
            lane_card_w - 12,
            lane_card_h - 8);
        draw_continue_chevrons(screen, panel_x0 + (panel_w / 2) - 7, panel_y1 - 11, 30, 14);
        return true;
    }

    if (rs->phase == GAME_PHASE_ROUND_CLEAR) {
        const int panel_x0 = 60;
        const int panel_x1 = 259;
        const int panel_y0 = 74;
        const int panel_y1 = 137;
        const uint32_t bonus = (rs->round_clear_bonus_score > 0) ? (uint32_t)rs->round_clear_bonus_score : 0u;
        char bonus_text[16];
        int digits[10];
        int count = 0;
        int i;
        uint32_t v = bonus;

        do {
            digits[count++] = (int)(v % 10u);
            v /= 10u;
        } while (v > 0u && count < 10);
        bonus_text[0] = '+';
        for (i = count - 1; i >= 0; --i) {
            bonus_text[count - i] = (char)('0' + digits[i]);
        }
        bonus_text[count + 1] = '\0';

        draw_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 11);
        draw_rect(screen, panel_x0 + 5, panel_y0 + 5, panel_x1 - 5, panel_y0 + 20, 1);
        draw_centered_text_scaled_outlined(screen, panel_x0, panel_x1, panel_y0 + 8, "ROUND CLEAR", 31, 1, 2);
        draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y0 + 28, "TIME BONUS", 30, 1);
        draw_centered_text_scaled_outlined(screen, panel_x0, panel_x1, panel_y0 + 39, bonus_text, 31, 1, 2);
        draw_continue_chevrons(screen, panel_x0 + ((panel_x1 - panel_x0) / 2) - 7, panel_y1 - 10, 31, 11);
        return true;
    }

    if (rs->phase == GAME_PHASE_GAME_OVER) {
        const int panel_x0 = 56;
        const int panel_x1 = 263;
        const int panel_y0 = 70;
        const int panel_y1 = 145;
        uint32_t score = rs->score > 99999999u ? 99999999u : rs->score;
        int score_scale = 3;
        int score_w = number_width_scaled(score, 5, score_scale);
        int score_x = panel_x0 + (((panel_x1 - panel_x0 + 1) - score_w) / 2);
#if AMIGA_FAST_RENDER
        UBYTE score_color = 31;
#else
        UBYTE score_color = ((((g_ui_anim_tick / 4) & 1u) == 0u) ? 31 : 30);
#endif
        draw_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 21);
        draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y0 + 7, "GAME OVER", 31, 1);
        draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y0 + 16, "FINAL SCORE", 30, 1);
        draw_number_scaled_outlined(screen, score_x, panel_y0 + 25, score, 5, score_color, 1, score_scale);
        if (rs->run_shards > 0) {
            draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y1 - 16, "BANK SHARDS", 8, 1);
            draw_number_right(screen, panel_x1 - 20, panel_y1 - 16, (uint32_t)rs->run_shards, 1, 8);
        }
        if (rs->phase_timer_ticks > 0) {
            draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y1 - 9, "PLEASE WAIT", 30, 1);
        } else {
            draw_centered_text_outlined(screen, panel_x0, panel_x1, panel_y1 - 9, "FIRE RESTART", 31, 1);
        }
        return true;
    }

    if (rs->phase == GAME_PHASE_PLAYER_DYING) {
        draw_panel(screen, 80, 82, 239, 112, 23);
        draw_centered_text_outlined(screen, 80, 239, 92, "DIED", 31, 1);
        draw_centered_text_outlined(screen, 80, 239, 100, "RESPAWNING", 30, 1);
        return true;
    }

    if (rs->phase == GAME_PHASE_PERK_CHOICE) {
        draw_perk_choice_panel(app, screen, rs);
        return true;
    }

    if (rs->phase == GAME_PHASE_META_UPGRADE) {
        draw_meta_choice_panel(app, screen, rs);
        return true;
    }

    return true;
}

static bool draw_overlay_delta(const AmigaApp *app, UBYTE *screen, const RenderState *rs, WORD previous_selection) {
    if (rs->phase == GAME_PHASE_PERK_CHOICE) {
        wait_blitter();
        draw_perk_choice_delta(screen, rs, previous_selection);
        return true;
    }
    if (rs->phase == GAME_PHASE_META_UPGRADE) {
        wait_blitter();
        draw_meta_choice_delta(screen, rs, previous_selection);
        return true;
    }
    return draw_overlay(app, screen, rs);
}

static bool blit_overlay_delta_to_buffer(
    AmigaApp *app,
    int dst_buf,
    int src_buf,
    const RenderState *rs,
    WORD previous_selection) {
    int count;
    int base_x;
    int selected;

    if (rs->phase == GAME_PHASE_PERK_CHOICE) {
        const int card_w = 58;
        const int card_h = 47;
        const int spacing = 8;
        const int card_y = 72;
        count = perk_choice_count_clamped(rs);
        base_x = perk_choice_base_x(count);
        selected = rs->perk_choice_selected;
        if (selected < 0 || selected >= count) {
            selected = 0;
        }
        if (previous_selection >= 0 && previous_selection < count && previous_selection != selected) {
            int px = base_x + previous_selection * (card_w + spacing) - 1;
            blit_screen_rect(app->screen[dst_buf], app->screen[src_buf], px, card_y, card_w + 3, card_h);
        }
        {
            int px = base_x + selected * (card_w + spacing) - 1;
            blit_screen_rect(app->screen[dst_buf], app->screen[src_buf], px, card_y, card_w + 3, card_h);
        }
        blit_screen_rect(app->screen[dst_buf], app->screen[src_buf], 52, 122, 216, 20);
        return true;
    }

    if (rs->phase == GAME_PHASE_META_UPGRADE) {
        const int card_w = 48;
        const int card_h = 47;
        const int spacing = 8;
        const int card_y = 72;
        count = meta_choice_count_clamped(rs);
        base_x = meta_choice_base_x(count);
        selected = rs->meta_choice_selected;
        if (selected < 0 || selected >= count) {
            selected = 0;
        }
        if (previous_selection >= 0 && previous_selection < count && previous_selection != selected) {
            int px = base_x + previous_selection * (card_w + spacing) - 1;
            blit_screen_rect(app->screen[dst_buf], app->screen[src_buf], px, card_y, card_w + 3, card_h);
        }
        {
            int px = base_x + selected * (card_w + spacing) - 1;
            blit_screen_rect(app->screen[dst_buf], app->screen[src_buf], px, card_y, card_w + 3, card_h);
        }
        blit_screen_rect(app->screen[dst_buf], app->screen[src_buf], 40, 122, 240, 20);
        return true;
    }

    return false;
}

static void mirror_modal_screen_to_other_buffer(
    AmigaApp *app,
    RendererState *rr,
    int src_buf,
    const DirtyRect *modal_rect,
    ULONG modal_key,
    ULONG modal_layout_key,
    WORD modal_selection,
    GamePhase phase,
    const HudSnapshot *hud,
    const RenderState *rs,
    bool selection_delta_only,
    WORD previous_selection) {
    int dst_buf = src_buf ^ 1;
    ModalCache *dst_modal = &rr->modal[dst_buf];

    if (selection_delta_only) {
        if (!blit_overlay_delta_to_buffer(app, dst_buf, src_buf, rs, previous_selection)) {
            (void)draw_overlay_delta(app, app->screen[dst_buf], rs, previous_selection);
        }
    } else {
        blit_screen_rect(app->screen[dst_buf], app->screen[src_buf], 0, 0, SCREEN_W, SCREEN_H);
    }

    dst_modal->rect = *modal_rect;
    dst_modal->key = modal_key;
    dst_modal->layout_key = modal_layout_key;
    dst_modal->phase = (UBYTE)phase;
    dst_modal->selection = modal_selection;
    dst_modal->active = 1;
    dst_modal->valid = 1;

    rr->prev_hud[dst_buf] = *hud;
    rr->prev_hud_valid[dst_buf] = 1;
}

static void draw_boot_screen(UBYTE *screen) {
    wait_blitter();
    draw_rect(screen, 0, 0, SCREEN_W - 1, SCREEN_H - 1, 1);
    draw_panel(screen, 72, 72, 247, 122, 21);
    draw_centered_text(screen, 72, 247, 88, "ICEPANIC", 31);
    draw_centered_text(screen, 72, 247, 104, "LOADING", 30);
}

static UBYTE render_frame(AmigaApp *app, int buf, UBYTE *screen, const RenderState *rs) {
    RendererState *rr = &app->renderer;
    bool modal_active = overlay_active(rs);
    ModalCache *modal = &rr->modal[buf];
    DirtyRect next_modal_rect;
    ULONG next_modal_key = 0;
    ULONG next_modal_layout_key = 0;
    WORD next_modal_selection = -1;
    bool restore_backdrop = true;
    bool partial_modal = false;
    bool overlay_touched;
    bool modal_was_active = (modal->valid && modal->active) ? true : false;
    bool translucent_modal = (!g_high_score_entry.active && rs->phase == GAME_PHASE_ROUND_INTRO && rs->start_title_pending) ? true : false;
    bool hud_changed = false;
    HudSnapshot hud;
    UBYTE frame_flags;

    update_hidden_block_rows(rr, rs);
    frame_flags = prepare_enemy_hardware_sprites(app, rr, buf, rs, modal_active) ? RENDER_FRAME_SPRITES : 0u;

    if (modal_active) {
        next_modal_rect = modal_rect_for_state(rs);
        next_modal_key = modal_key_for_state(rs);
        next_modal_layout_key = modal_layout_key_for_state(rs);
        next_modal_selection = modal_selection_for_state(rs);
        if (modal->valid && modal->active && modal->key == next_modal_key) {
            return frame_flags;
        }

        if (!translucent_modal && modal->valid && modal->active && dirty_rect_equal(&modal->rect, &next_modal_rect)) {
            restore_backdrop = false;
            if (modal_selection_delta_supported(rs) &&
                modal->phase == (UBYTE)rs->phase &&
                modal->layout_key == next_modal_layout_key &&
                modal->selection != next_modal_selection) {
                partial_modal = true;
            }
        } else if (modal->valid && modal->active) {
            mark_dirty_rect(rr, buf, &modal->rect);
        }
    } else if (modal->valid && modal->active) {
        mark_dirty_rect(rr, buf, &modal->rect);
        modal->active = 0;
        modal->valid = 0;
        modal->phase = 0;
        modal->selection = -1;
        modal->layout_key = 0;
    }

    if (!modal_active || restore_backdrop) {
        if (modal_active || modal_was_active || !rr->prev_valid[buf]) {
            restore_dynamic_caches(rr, buf);
        }
        PROFILE_COLOR(0x220);
        restore_previous_dynamic(app, rr, buf, screen);
        update_static_dirty(rr, rs, buf);
        prepare_dynamic_actors(rr, buf, rs);
        invalidate_moving_blocks_overlapping_dirty(rr, buf);
        render_dirty_static(app, screen, buf, rs);
        PROFILE_COLOR(0x000);
    }

    hud = make_hud_snapshot(rs);

    if (modal_active) {
        PROFILE_COLOR(0x088);
        if (!rr->prev_hud_valid[buf]) {
            draw_hud(screen, rs);
            rr->prev_hud[buf] = hud;
            rr->prev_hud_valid[buf] = 1;
            hud_changed = true;
        } else if (!hud_snapshot_equal(&rr->prev_hud[buf], &hud)) {
            draw_hud_delta(screen, rs, &rr->prev_hud[buf], &hud);
            rr->prev_hud[buf] = hud;
            hud_changed = true;
        }
        PROFILE_COLOR(0x808);
        if (partial_modal) {
            (void)draw_overlay_delta(app, screen, rs, modal->selection);
        } else {
            (void)draw_overlay(app, screen, rs);
        }
        mirror_modal_screen_to_other_buffer(
            app,
            rr,
            buf,
            &next_modal_rect,
            next_modal_key,
            next_modal_layout_key,
            next_modal_selection,
            rs->phase,
            &hud,
            rs,
            partial_modal && !hud_changed,
            modal->selection);
        PROFILE_COLOR(0x000);
        modal->rect = next_modal_rect;
        modal->key = next_modal_key;
        modal->layout_key = next_modal_layout_key;
        modal->phase = (UBYTE)rs->phase;
        modal->selection = next_modal_selection;
        modal->active = 1;
        modal->valid = 1;
        return (UBYTE)(frame_flags | RENDER_FRAME_SCREEN);
    }

    PROFILE_COLOR(0x080);
    draw_dynamic_actors(app, rr, buf, screen, rs);
    draw_impact_fx(app, rr, buf, screen, rs);
#if !AMIGA_FAST_RENDER
    draw_score_popups(app, rr, buf, screen, rs);
#endif
    PROFILE_COLOR(0x088);
    if (!rr->prev_hud_valid[buf]) {
        draw_hud(screen, rs);
        rr->prev_hud[buf] = hud;
        rr->prev_hud_valid[buf] = 1;
    } else if (!hud_snapshot_equal(&rr->prev_hud[buf], &hud)) {
        draw_hud_delta(screen, rs, &rr->prev_hud[buf], &hud);
        rr->prev_hud[buf] = hud;
    }
    PROFILE_COLOR(0x808);
    overlay_touched = draw_overlay(app, screen, rs);
    PROFILE_COLOR(0x000);
    if (overlay_touched) {
        mark_gameplay_border_dirty(rr, buf);
    }
    return (UBYTE)(frame_flags | RENDER_FRAME_SCREEN);
}

static BOOL quit_combo_down(void) {
    volatile UBYTE *ciaa_pra = (volatile UBYTE *)0xBFE001;
    UBYTE pra = *ciaa_pra;
    return ((pra & (PRA_FIR0_BIT | PRA_FIR1_BIT)) == 0);
}

static BOOL joystick_fire_down(void) {
    volatile UBYTE *ciaa_pra = (volatile UBYTE *)0xBFE001;
    UBYTE pra = *ciaa_pra;
    return ((pra & PRA_FIR1_BIT) == 0);
}

static UBYTE dirs_from_joydat(UWORD joy) {
    UBYTE dirs = 0;
    BOOL left = ((joy & 0x0200u) != 0);
    BOOL right = ((joy & 0x0002u) != 0);
    if (left) {
        dirs |= 1u;
    }
    if (right) {
        dirs |= 2u;
    }
    if ((((joy & 0x0100u) != 0) ^ left) != 0) {
        dirs |= 4u;
    }
    if ((((joy & 0x0001u) != 0) ^ right) != 0) {
        dirs |= 8u;
    }
    return dirs;
}

static InputState read_input(UBYTE *last_dirs, UBYTE *last_fire) {
    InputState in;
    UBYTE dirs = 0;
    UBYTE fire;
    memset(&in, 0, sizeof(in));

    dirs = dirs_from_joydat(CREGS->joy1dat);

    if ((dirs & 1u) != 0) {
        in.left = true;
    }
    if ((dirs & 2u) != 0) {
        in.right = true;
    }
    if ((dirs & 4u) != 0) {
        in.up = true;
    }
    if ((dirs & 8u) != 0) {
        in.down = true;
    }

    in.newest_press = DIR_NONE;
    if ((dirs & 1u) != 0 && (*last_dirs & 1u) == 0) {
        in.newest_press = DIR_LEFT;
    }
    if ((dirs & 2u) != 0 && (*last_dirs & 2u) == 0) {
        in.newest_press = DIR_RIGHT;
    }
    if ((dirs & 4u) != 0 && (*last_dirs & 4u) == 0) {
        in.newest_press = DIR_UP;
    }
    if ((dirs & 8u) != 0 && (*last_dirs & 8u) == 0) {
        in.newest_press = DIR_DOWN;
    }
    *last_dirs = dirs;

    fire = joystick_fire_down() ? 1u : 0u;
    in.start = false;
    in.fire_pressed = (fire != 0 && *last_fire == 0);
    in.fire_released = (fire == 0 && *last_fire != 0);
    *last_fire = fire;
    return in;
}

static Direction held_menu_direction(const InputState *in) {
    if (in->left) return DIR_LEFT;
    if (in->right) return DIR_RIGHT;
    if (in->up) return DIR_UP;
    if (in->down) return DIR_DOWN;
    return DIR_NONE;
}

static void apply_menu_input_repeat(
    const GameState *game,
    InputState *in,
    Direction *repeat_dir,
    WORD *repeat_ticks) {
    Direction held;
    BOOL repeat_enabled;

    repeat_enabled = (g_high_score_entry.active ||
                      game->phase == GAME_PHASE_PERK_CHOICE ||
                      game->phase == GAME_PHASE_META_UPGRADE) ? TRUE : FALSE;
    if (!repeat_enabled) {
        *repeat_dir = DIR_NONE;
        *repeat_ticks = 0;
        return;
    }

    held = held_menu_direction(in);
    if (held == DIR_NONE) {
        *repeat_dir = DIR_NONE;
        *repeat_ticks = 0;
        return;
    }

    if (in->newest_press != DIR_NONE) {
        *repeat_dir = in->newest_press;
        *repeat_ticks = 9;
        return;
    }

    if (*repeat_dir != held) {
        *repeat_dir = held;
        *repeat_ticks = 9;
        return;
    }

    if (*repeat_ticks > 0) {
        --(*repeat_ticks);
        return;
    }

    in->newest_press = held;
    *repeat_ticks = 4;
}

static BOOL build_copper_list(CopperState *cs) {
    UWORD *p;
    UWORD *cop;
    WORD i;
    ULONG words = 192;
    cop = (UWORD *)AllocMem(words * sizeof(UWORD), ICE_MEMF_CHIP | ICE_MEMF_CLEAR);
    if (!cop) {
        return FALSE;
    }
    cs->list = cop;
    cs->bytes = words * sizeof(UWORD);
    p = cop;

    *p++ = DIWSTRT; *p++ = 0x2C81;
    *p++ = DIWSTOP; *p++ = 0xF4C1;
    *p++ = DDFSTRT; *p++ = 0x0038;
    *p++ = DDFSTOP; *p++ = 0x00D0;
    *p++ = BPLCON0; *p++ = (UWORD)(0x0200 | (DEPTH << 12));
    *p++ = BPLCON1; *p++ = 0x0000;
    *p++ = BPLCON2;
#if AMIGA_HW_SPRITES_ENABLED
    *p++ = BPLCON2_SPRITES_FRONT;
#else
    *p++ = 0x0000;
#endif
    *p++ = BPL1MOD; *p++ = ROW_STRIDE_BYTES - ROW_BYTES;
    *p++ = BPL2MOD; *p++ = ROW_STRIDE_BYTES - ROW_BYTES;

    for (i = 0; i < DEPTH; ++i) {
        *p++ = (UWORD)(BPL1PTH + (i * 4));
        cs->bpl_hi[i] = p;
        *p++ = 0;
        *p++ = (UWORD)(BPL1PTL + (i * 4));
        cs->bpl_lo[i] = p;
        *p++ = 0;
    }

    for (i = 0; i < HW_SPRITE_COUNT; ++i) {
        *p++ = (UWORD)(SPR0PTH + (i * 4));
        cs->spr_hi[i] = p;
        *p++ = 0;
        *p++ = (UWORD)(SPR0PTL + (i * 4));
        cs->spr_lo[i] = p;
        *p++ = 0;
    }

    for (i = 0; i < AMIGA_ASSET_PALETTE_COUNT; ++i) {
        *p++ = (UWORD)COLOR_REG(i);
#if AMIGA_USE_HW_SPRITES
        *p++ = (i == 29) ? g_amiga_palette[2] : g_amiga_palette[i];
#else
        *p++ = g_amiga_palette[i];
#endif
    }

    *p++ = (UWORD)((0xF4 << 8) | 0x07);
    *p++ = 0xFFFE;
    *p++ = BPLCON0;
    *p++ = 0x0000;
    *p++ = COLOR00;
    *p++ = 0x0000;
    *p++ = 0xFFFF;
    *p++ = 0xFFFE;

    return TRUE;
}

static void patch_copper_bplptrs(CopperState *cs, UBYTE *screen) {
    WORD i;
    for (i = 0; i < DEPTH; ++i) {
        ULONG addr = (ULONG)screen + ((ULONG)i * ROW_BYTES);
        *(cs->bpl_hi[i]) = (UWORD)(addr >> 16);
        *(cs->bpl_lo[i]) = (UWORD)(addr & 0xFFFF);
    }
}

static void patch_copper_spriteptrs(CopperState *cs, UBYTE *bank) {
    WORD i;
    for (i = 0; i < HW_SPRITE_COUNT; ++i) {
        ULONG addr = (ULONG)bank + ((ULONG)i * HW_SPRITE_BYTES);
        *(cs->spr_hi[i]) = (UWORD)(addr >> 16);
        *(cs->spr_lo[i]) = (UWORD)(addr & 0xFFFF);
    }
}

static BOOL init_display(void) {
    g_old_view = ((struct GfxBase *)GfxBase)->ActiView;
    LoadView(NULL);
    WaitTOF();
    WaitTOF();
    CREGS->dmacon = DMAF_ALL;
    g_sprite_dma_enabled = FALSE;
    return ((((struct GfxBase *)GfxBase)->DisplayFlags & PAL) == PAL);
}

static void reset_display(void) {
    wait_blitter();
    LoadView(g_old_view);
    WaitTOF();
    WaitTOF();
    CREGS->cop1lc = (ULONG)((struct GfxBase *)GfxBase)->copinit;
    CREGS->copjmp1 = 0;
    RethinkDisplay();
    g_old_view = 0;
}

static BOOL app_alloc(AmigaApp *app) {
    memset(app, 0, sizeof(*app));
    app->screen[0] = (UBYTE *)AllocMem(SCREEN_BYTES, ICE_MEMF_CHIP | ICE_MEMF_CLEAR);
    app->screen[1] = (UBYTE *)AllocMem(SCREEN_BYTES, ICE_MEMF_CHIP | ICE_MEMF_CLEAR);
    app->chip_tiles = (UBYTE *)AllocMem(TILE_CHIP_BYTES, ICE_MEMF_CHIP);
    app->chip_tile_masks = (UBYTE *)AllocMem(TILE_MASK_CHIP_BYTES, ICE_MEMF_CHIP);
    app->chip_bobs = (UBYTE *)AllocMem(BOB_CHIP_BYTES, ICE_MEMF_CHIP);
    app->chip_masks = (UBYTE *)AllocMem(MASK_CHIP_BYTES, ICE_MEMF_CHIP);
    app->chip_backdrop = (UBYTE *)AllocMem(SCREEN_BYTES, ICE_MEMF_CHIP | ICE_MEMF_CLEAR);
    app->chip_title = (UBYTE *)AllocMem(TITLE_CHIP_BYTES, ICE_MEMF_CHIP);
    app->chip_title_mask = (UBYTE *)AllocMem(TITLE_MASK_CHIP_BYTES, ICE_MEMF_CHIP);
#if AMIGA_HW_SPRITES_ENABLED
    app->hw_sprite_bank[0] = (UBYTE *)AllocMem(HW_SPRITE_BANK_BYTES, ICE_MEMF_CHIP | ICE_MEMF_CLEAR);
    app->hw_sprite_bank[1] = (UBYTE *)AllocMem(HW_SPRITE_BANK_BYTES, ICE_MEMF_CHIP | ICE_MEMF_CLEAR);
#endif
    if (!app->screen[0] || !app->screen[1] || !app->chip_tiles || !app->chip_tile_masks ||
        !app->chip_bobs || !app->chip_masks || !app->chip_backdrop || !app->chip_title || !app->chip_title_mask
#if AMIGA_HW_SPRITES_ENABLED
        || !app->hw_sprite_bank[0] || !app->hw_sprite_bank[1]
#endif
    ) {
        return FALSE;
    }
    CopyMem((const void *)g_amiga_tile_data, app->chip_tiles, TILE_CHIP_BYTES);
    CopyMem((const void *)g_amiga_tile_mask, app->chip_tile_masks, TILE_MASK_CHIP_BYTES);
    CopyMem((const void *)g_amiga_bob_data, app->chip_bobs, BOB_CHIP_BYTES);
    CopyMem((const void *)g_amiga_bob_mask, app->chip_masks, MASK_CHIP_BYTES);
    CopyMem((const void *)g_amiga_title_overlay_data, app->chip_title, TITLE_CHIP_BYTES);
    CopyMem((const void *)g_amiga_title_overlay_mask, app->chip_title_mask, TITLE_MASK_CHIP_BYTES);
#if AMIGA_USE_HW_SPRITES
    build_hw_enemy_sprite_lines(app);
#endif
#if AMIGA_HW_SPARKLES_ENABLED
    build_hw_sparkle_sprite_lines(app);
#endif
#if AMIGA_HW_SPRITES_ENABLED
    clear_hw_sprite_bank(app->hw_sprite_bank[0]);
    clear_hw_sprite_bank(app->hw_sprite_bank[1]);
#endif
    (void)sfx_init(&app->sfx);
    if (!build_copper_list(&app->copper)) {
        return FALSE;
    }
    mark_all_dirty(&app->renderer, 0);
    mark_all_dirty(&app->renderer, 1);
    return TRUE;
}

static void app_free(AmigaApp *app) {
    sfx_shutdown(&app->sfx);
    if (app->copper.list) {
        FreeMem(app->copper.list, app->copper.bytes);
        app->copper.list = 0;
    }
    if (app->chip_masks) {
        FreeMem(app->chip_masks, MASK_CHIP_BYTES);
        app->chip_masks = 0;
    }
    if (app->chip_title_mask) {
        FreeMem(app->chip_title_mask, TITLE_MASK_CHIP_BYTES);
        app->chip_title_mask = 0;
    }
    if (app->chip_title) {
        FreeMem(app->chip_title, TITLE_CHIP_BYTES);
        app->chip_title = 0;
    }
    if (app->chip_backdrop) {
        FreeMem(app->chip_backdrop, SCREEN_BYTES);
        app->chip_backdrop = 0;
    }
    if (app->chip_tile_masks) {
        FreeMem(app->chip_tile_masks, TILE_MASK_CHIP_BYTES);
        app->chip_tile_masks = 0;
    }
    if (app->hw_sprite_bank[0]) {
        FreeMem(app->hw_sprite_bank[0], HW_SPRITE_BANK_BYTES);
        app->hw_sprite_bank[0] = 0;
    }
    if (app->hw_sprite_bank[1]) {
        FreeMem(app->hw_sprite_bank[1], HW_SPRITE_BANK_BYTES);
        app->hw_sprite_bank[1] = 0;
    }
    if (app->chip_bobs) {
        FreeMem(app->chip_bobs, BOB_CHIP_BYTES);
        app->chip_bobs = 0;
    }
    if (app->chip_tiles) {
        FreeMem(app->chip_tiles, TILE_CHIP_BYTES);
        app->chip_tiles = 0;
    }
    if (app->screen[0]) {
        FreeMem(app->screen[0], SCREEN_BYTES);
        app->screen[0] = 0;
    }
    if (app->screen[1]) {
        FreeMem(app->screen[1], SCREEN_BYTES);
        app->screen[1] = 0;
    }
}

static void app_shutdown(AmigaApp *app) {
    wait_blitter();
    sfx_shutdown(&app->sfx);
    if (app->owns_blitter) {
        DisownBlitter();
        app->owns_blitter = FALSE;
    }
    reset_display();
    if (app->wb_was_open) {
        OpenWorkBench();
        app->wb_was_open = FALSE;
    }
    app_free(app);
}

static uint32_t mix_seed(uint32_t seed, uint32_t value) {
    seed ^= value + 0x9e3779b9u + (seed << 6) + (seed >> 2);
    seed ^= seed >> 16;
    seed *= 0x7feb352du;
    seed ^= seed >> 15;
    seed *= 0x846ca68bu;
    seed ^= seed >> 16;
    return seed;
}

static uint32_t startup_seed(void) {
    struct DateStamp ds;
    uint32_t seed = 0xa53c9655u;
    volatile UBYTE *ciaa_pra = (volatile UBYTE *)0xBFE001;
    volatile UBYTE *ciaa_talo = (volatile UBYTE *)0xBFE401;
    volatile UBYTE *ciaa_tahi = (volatile UBYTE *)0xBFE501;
    volatile UBYTE *ciab_tblo = (volatile UBYTE *)0xBFD600;
    volatile UBYTE *ciab_tbhi = (volatile UBYTE *)0xBFD700;

    DateStamp(&ds);
    seed = mix_seed(seed, (uint32_t)ds.ds_Days);
    seed = mix_seed(seed, (uint32_t)ds.ds_Minute);
    seed = mix_seed(seed, (uint32_t)ds.ds_Tick);
    seed = mix_seed(seed, ((uint32_t)CREGS->vposr << 16) | (uint32_t)CREGS->vhposr);
    seed = mix_seed(seed, ((uint32_t)CREGS->joy1dat << 16) | (uint32_t)CREGS->pot0dat);
    seed = mix_seed(seed, ((uint32_t)CREGS->pot1dat << 16) | (uint32_t)*ciaa_pra);
    seed = mix_seed(seed, ((uint32_t)*ciaa_tahi << 24) |
                         ((uint32_t)*ciaa_talo << 16) |
                         ((uint32_t)*ciab_tbhi << 8) |
                         (uint32_t)*ciab_tblo);
    if (seed == 0) {
        seed = 0x12345678u;
    }
    return seed;
}

int main(void) {
    AmigaApp *app = &g_app;
    GameState *game = &g_game;
    RenderState *rs = (RenderState *)game;
    UBYTE last_dirs = 0;
    UBYTE last_fire = 0;
    Direction menu_repeat_dir = DIR_NONE;
    WORD menu_repeat_ticks = 0;
    WORD quit_hold = 0;
    WORD draw_idx = 1;
    WORD sprite_draw_idx = 1;
    WORD render_divider = 0;
    GamePhase last_rendered_phase = GAME_PHASE_ROUND_INTRO;
    BOOL title_jingle_queued = FALSE;
    BOOL pending_post_game_over_resolution = FALSE;
    WORD pending_post_game_over_insert_index = -1;
    uint32_t pending_post_game_over_score = 0u;
    WORD post_high_score_pause_ticks = 0;
    int exit_code = 0;
    FrameSync sync;

    memset(app, 0, sizeof(*app));

    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
    if (!GfxBase) {
        return 20;
    }
    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
    if (!IntuitionBase) {
        CloseLibrary((struct Library *)GfxBase);
        return 20;
    }

    SetTaskPri(FindTask(NULL), TASK_PRIORITY);
    app->wb_was_open = CloseWorkBench() ? TRUE : FALSE;

    if (!init_display()) {
        reset_display();
        if (app->wb_was_open) {
            OpenWorkBench();
        }
        CloseLibrary((struct Library *)IntuitionBase);
        CloseLibrary((struct Library *)GfxBase);
        return 20;
    }

    OwnBlitter();
    app->owns_blitter = TRUE;

    if (!app_alloc(app)) {
        app_shutdown(app);
        CloseLibrary((struct Library *)IntuitionBase);
        CloseLibrary((struct Library *)GfxBase);
        return 20;
    }

#if AMIGA_HW_SPRITES_ENABLED
    patch_copper_spriteptrs(&app->copper, app->hw_sprite_bank[0]);
#endif
    draw_boot_screen(app->screen[0]);
    wait_blitter();
    patch_copper_bplptrs(&app->copper, app->screen[0]);
#if AMIGA_HW_SPRITES_ENABLED
    patch_copper_spriteptrs(&app->copper, app->hw_sprite_bank[0]);
#endif
    CREGS->cop1lc = (ULONG)app->copper.list;
    CREGS->copjmp1 = 0;
    CREGS->dmacon = DISPLAY_DMA_BITS;
    g_sprite_dma_enabled = FALSE;

    load_high_score_table(&g_high_scores);
    memset(&g_high_score_entry, 0, sizeof(g_high_score_entry));

    game_init(game, startup_seed());
    {
        uint32_t boot_events = game_consume_events(game);
        if (game->phase == GAME_PHASE_ROUND_INTRO && game->start_title_pending) {
            if ((boot_events & GAME_EVENT_ROUND_START) != 0u) {
                sfx_queue_title_start_jingle(&app->sfx);
                title_jingle_queued = TRUE;
            }
            boot_events &= (uint32_t)~GAME_EVENT_ROUND_START;
        }
        sfx_dispatch_events(&app->sfx, boot_events);
        sfx_update(&app->sfx);
    }
    mark_core_dirty_static(&app->renderer, game);
    game_clear_dirty_static(game);
    render_frame(app, 0, app->screen[0], rs);
    render_frame(app, 1, app->screen[1], rs);
    last_rendered_phase = rs->phase;
    wait_blitter();
    patch_copper_bplptrs(&app->copper, app->screen[0]);
#if AMIGA_HW_SPRITES_ENABLED
    patch_copper_spriteptrs(&app->copper, app->hw_sprite_bank[0]);
#endif
    set_sprite_dma_for_state(rs);
    wait_frame_boundary_hw();

    while (TRUE) {
        frame_sync_begin(&sync);
        InputState input = read_input(&last_dirs, &last_fire);
        apply_menu_input_repeat(game, &input, &menu_repeat_dir, &menu_repeat_ticks);
        if (quit_combo_down()) {
            ++quit_hold;
            if (quit_hold >= GAME_FIXED_TICK_HZ) {
                break;
            }
        } else {
            quit_hold = 0;
        }
        if (g_high_score_entry.active) {
            if (update_high_score_entry(&g_high_score_entry, &input)) {
                high_score_insert(
                    &g_high_scores,
                    g_high_score_entry.insert_index,
                    g_high_score_entry.initials,
                    g_high_score_entry.pending_score);
                save_high_score_table(&g_high_scores);
                post_high_score_pause_ticks = (WORD)((GAME_FIXED_TICK_HZ * 3) / 5);
                pending_post_game_over_resolution = FALSE;
                pending_post_game_over_insert_index = -1;
                pending_post_game_over_score = 0u;
                if (game->phase == GAME_PHASE_GAME_OVER) {
                    InputState resume_input;
                    uint32_t events;
                    uint32_t audio_events;
                    memset(&resume_input, 0, sizeof(resume_input));
                    resume_input.start = true;
                    game_step(game, &resume_input);
                    events = game_consume_events(game);
                    audio_events = events;
                    if ((events & GAME_EVENT_ROUND_START) != 0u) {
                        renderer_reset_for_round(&app->renderer);
                    }
                    if (game->phase == GAME_PHASE_ROUND_INTRO && game->start_title_pending) {
                        audio_events &= (uint32_t)~GAME_EVENT_ROUND_START;
                        title_jingle_queued = TRUE;
                    }
                    sfx_dispatch_events(&app->sfx, audio_events);
                }
            }
            sfx_update(&app->sfx);
        } else if (post_high_score_pause_ticks > 0) {
            --post_high_score_pause_ticks;
            sfx_update(&app->sfx);
        } else {
            if (input.fire_released && game->phase != GAME_PHASE_PLAYING) {
                sfx_queue_title_confirm_chirp(&app->sfx);
            }
            PROFILE_COLOR(0x400);
            game_step(game, &input);
            PROFILE_COLOR(0x000);
            frame_sync_sample(&sync);
            PROFILE_COLOR(0x840);
            {
                uint32_t events = game_consume_events(game);
                uint32_t audio_events = events;
                if ((events & GAME_EVENT_GAME_OVER) != 0u) {
                    pending_post_game_over_resolution = TRUE;
                    pending_post_game_over_score = game->score;
                    pending_post_game_over_insert_index = high_score_find_insert_index(&g_high_scores, game->score);
                    if (pending_post_game_over_insert_index >= 0) {
                        audio_events &= (uint32_t)~GAME_EVENT_GAME_OVER;
                        sfx_queue_new_high_score_fanfare(&app->sfx);
                    }
                }
                if ((events & GAME_EVENT_ROUND_START) != 0u) {
                    renderer_reset_for_round(&app->renderer);
                }
                if (pending_post_game_over_resolution &&
                    pending_post_game_over_insert_index >= 0 &&
                    game->phase != GAME_PHASE_GAME_OVER) {
                    begin_high_score_entry(
                        &g_high_score_entry,
                        pending_post_game_over_insert_index,
                        pending_post_game_over_score,
                        &g_high_scores);
                    pending_post_game_over_resolution = FALSE;
                    pending_post_game_over_insert_index = -1;
                    pending_post_game_over_score = 0u;
                    audio_events &= (uint32_t)~GAME_EVENT_ROUND_START;
                    title_jingle_queued = TRUE;
                } else if (pending_post_game_over_resolution && game->phase != GAME_PHASE_GAME_OVER) {
                    pending_post_game_over_resolution = FALSE;
                    pending_post_game_over_insert_index = -1;
                    pending_post_game_over_score = 0u;
                }
                if (game->phase == GAME_PHASE_ROUND_INTRO && game->start_title_pending) {
                    if ((audio_events & GAME_EVENT_ROUND_START) != 0u && !title_jingle_queued) {
                        sfx_queue_title_start_jingle(&app->sfx);
                        title_jingle_queued = TRUE;
                    }
                    audio_events &= (uint32_t)~GAME_EVENT_ROUND_START;
                } else if (!g_high_score_entry.active) {
                    title_jingle_queued = FALSE;
                }
                sfx_dispatch_events(&app->sfx, audio_events);
                sfx_update(&app->sfx);
            }
        }
        PROFILE_COLOR(0x000);
        frame_sync_sample(&sync);
        mark_core_dirty_static(&app->renderer, game);
        game_clear_dirty_static(game);

        ++render_divider;
        if (render_divider >= AMIGA_RENDER_DIVISOR || rs->phase != last_rendered_phase) {
            UBYTE render_flags;
            WORD render_buf = draw_idx;
            BOOL phase_changed = (rs->phase != last_rendered_phase) ? TRUE : FALSE;
#if AMIGA_HW_SPRITES_ENABLED
            if (title_hw_sparkles_active(rs)) {
                ULONG title_key = modal_key_for_state(rs);
                ModalCache *sprite_modal = &app->renderer.modal[sprite_draw_idx];
                if (sprite_modal->valid && sprite_modal->active && sprite_modal->key == title_key) {
                    render_buf = sprite_draw_idx;
                }
            }
#endif
            render_divider = 0;
            PROFILE_COLOR(0x040);
            render_flags = render_frame(app, render_buf, app->screen[render_buf], rs);
            last_rendered_phase = rs->phase;
            PROFILE_COLOR(0x004);
            if (render_flags != 0u) {
                wait_blitter();
            }
            if ((render_flags & RENDER_FRAME_SCREEN) != 0u) {
                patch_copper_bplptrs(&app->copper, app->screen[render_buf]);
                draw_idx = (WORD)(render_buf ^ 1);
                sprite_draw_idx = draw_idx;
            }
#if AMIGA_HW_SPRITES_ENABLED
            if ((render_flags & RENDER_FRAME_SPRITES) != 0u) {
                patch_copper_spriteptrs(&app->copper, app->hw_sprite_bank[render_buf]);
                if ((render_flags & RENDER_FRAME_SCREEN) == 0u) {
                    sprite_draw_idx = (WORD)(render_buf ^ 1);
                }
            }
#endif
            if (render_flags != 0u || phase_changed) {
                set_sprite_dma_for_state(rs);
            }
            frame_sync_sample(&sync);
        }
        PROFILE_COLOR(0x000);
        frame_sync_finish(&sync);
        g_ui_anim_tick = (g_ui_anim_tick + 1u) & 0x7fffffffu;
    }

    app_shutdown(app);
    CloseLibrary((struct Library *)IntuitionBase);
    CloseLibrary((struct Library *)GfxBase);
    return exit_code;
}
