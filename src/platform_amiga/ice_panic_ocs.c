#include <exec/types.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <libraries/dos.h>
#include <graphics/gfxbase.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <stddef.h>
#include <string.h>

#include "game.h"
#include "amiga_assets_ocs.h"
#include "amiga_sfx_assets_ocs.h"

#define CUSTOM_CHIP_BASE 0xDFF000UL
#define CREGS ((volatile struct Custom *)CUSTOM_CHIP_BASE)

#define CUSTOM_REG(member) ((UWORD)offsetof(struct Custom, member))
#define CUSTOM_PTR_HI(base, index) ((UWORD)(CUSTOM_REG(base) + ((index) * (int)sizeof(APTR))))
#define CUSTOM_PTR_LO(base, index) ((UWORD)(CUSTOM_PTR_HI(base, index) + (int)sizeof(UWORD)))
#define COPJMP1 CUSTOM_REG(copjmp1)
#define DIWSTRT CUSTOM_REG(diwstrt)
#define DIWSTOP CUSTOM_REG(diwstop)
#define DDFSTRT CUSTOM_REG(ddfstrt)
#define DDFSTOP CUSTOM_REG(ddfstop)
#define BPL1PTH CUSTOM_PTR_HI(bplpt, 0)
#define BPL1PTL CUSTOM_PTR_LO(bplpt, 0)
#define SPR0PTH CUSTOM_PTR_HI(sprpt, 0)
#define SPR0PTL CUSTOM_PTR_LO(sprpt, 0)
#define BPLCON0 CUSTOM_REG(bplcon0)
#define BPLCON1 CUSTOM_REG(bplcon1)
#define BPLCON2 CUSTOM_REG(bplcon2)
#define BPL1MOD CUSTOM_REG(bpl1mod)
#define BPL2MOD CUSTOM_REG(bpl2mod)
#define COLOR00 CUSTOM_REG(color)
#define COLOR01 ((UWORD)(CUSTOM_REG(color) + sizeof(UWORD)))

#define SCREEN_W 320
#define SCREEN_H 200
#define DEPTH 5
#define ROW_BYTES (SCREEN_W / 8)
#define ROW_STRIDE_BYTES (ROW_BYTES * DEPTH)
#define SCREEN_BYTES ((ULONG)ROW_STRIDE_BYTES * SCREEN_H)
#define OCS_DMA_SAFE_LIMIT 0x00080000UL
#define PROBE_TASK_PRIORITY 20
#define PROBE_MAX_PARKED_POOLS 8
#define PROBE_SPRITE_SLOTS ((ULONG)AMIGA_ASSET_SPRITE_COUNT)
#define PROBE_TILE_BYTES (PROBE_SPRITE_SLOTS * AMIGA_ASSET_TILE_BYTES)
#define PROBE_TILE_MASK_BYTES (PROBE_SPRITE_SLOTS * AMIGA_ASSET_TILE_MASK_BYTES)
#define PROBE_BOB_BYTES (PROBE_SPRITE_SLOTS * AMIGA_ASSET_BOB_BYTES)
#define PROBE_MASK_BYTES (PROBE_SPRITE_SLOTS * AMIGA_ASSET_MASK_BYTES)
#define PROBE_TITLE_CELL_TILE_BYTES ((ULONG)AMIGA_ASSET_TILE_BYTES)
#define PROBE_TITLE_CELL_TILE_MASK_BYTES ((ULONG)AMIGA_ASSET_TILE_MASK_BYTES)
#define PROBE_TITLE_CELL_SCRATCH_BYTES (PROBE_TITLE_CELL_TILE_BYTES + PROBE_TITLE_CELL_TILE_MASK_BYTES)
#define PROBE_ASSET_BYTES (PROBE_MASK_BYTES + PROBE_BOB_BYTES + PROBE_TILE_MASK_BYTES + PROBE_TILE_BYTES + PROBE_TITLE_CELL_SCRATCH_BYTES)
#define GAME_TILE_SIZE 16
#define PROBE_HW_SPRITE_COUNT 8
#define PROBE_HW_SPARKLE_FIRST_CHANNEL 6
#define PROBE_HW_SPARKLE_COUNT 2
#define PROBE_HW_ENEMY_FIRST_CHANNEL 2
#define PROBE_HW_ENEMY_COUNT (PROBE_HW_SPRITE_COUNT - PROBE_HW_ENEMY_FIRST_CHANNEL)
#define PROBE_HW_SPRITE_HEIGHT 16
#define PROBE_HW_FULL_SPRITE_WORDS (2UL + ((ULONG)PROBE_HW_SPRITE_HEIGHT * 2UL) + 2UL)
#define PROBE_HW_FULL_SPRITE_BYTES (PROBE_HW_FULL_SPRITE_WORDS * sizeof(UWORD))
#define PROBE_HW_NULL_SPRITE_WORDS 6UL
#define PROBE_HW_NULL_SPRITE_BYTES (PROBE_HW_NULL_SPRITE_WORDS * sizeof(UWORD))
#define PROBE_HW_NULL_SPRITE_COUNT (PROBE_HW_SPRITE_COUNT - PROBE_HW_SPARKLE_COUNT)
#define PROBE_HW_SPRITE_BANK_BYTES (((ULONG)PROBE_HW_SPARKLE_COUNT * PROBE_HW_FULL_SPRITE_BYTES) + ((ULONG)PROBE_HW_NULL_SPRITE_COUNT * PROBE_HW_NULL_SPRITE_BYTES))
#define PROBE_HW_PTR_MODE_NULL 0
#define PROBE_HW_PTR_MODE_TITLE 1
#define PROBE_HW_PTR_MODE_GAMEPLAY 2
#define PROBE_DISPLAY_BUFFER_COUNT 2
#define PROBE_COPPER_COUNT 2
#define PROBE_COPPER_SPARKLE_COUNT 7
#define PROBE_COPPER_SPARKLE_TOP_Y 10
#define PROBE_COPPER_SPARKLE_STEP_Y 25
#define PROBE_COPPER_SPARKLE_RESTORE_GAP 2
#define PROBE_COPPER_SPARKLE_WORDS ((ULONG)PROBE_COPPER_SPARKLE_COUNT * 20UL)
#define PROBE_PAL_FULL_DIWSTOP 0x2CC1
#define PROBE_PAL_REFLECTION_SETUP_LINE 0xF4
#define PROBE_PAL_REFLECTION_START_LINE 0xF5
#define PROBE_PAL_REFLECTION_LINES 55
#define PROBE_PAL_REFLECTION_SOURCE_STEP 2
#define PROBE_PAL_REFLECTION_BPLMOD ((UWORD)(0x10000UL - (((ULONG)ROW_STRIDE_BYTES * PROBE_PAL_REFLECTION_SOURCE_STEP) + ROW_BYTES)))
#define PROBE_COPPER_REFLECTION_WRAP_WORDS 2UL
#define PROBE_COPPER_REFLECTION_WORDS (2UL + 2UL + (32UL * 2UL) + 2UL + 4UL + ((ULONG)DEPTH * 4UL) + 2UL + 2UL + (((ULONG)PROBE_PAL_REFLECTION_LINES - 1UL) * 4UL) + PROBE_COPPER_REFLECTION_WRAP_WORDS)
#define PROBE_COPPER_USED_WORDS (8UL + ((ULONG)DEPTH * 4UL) + 10UL + ((ULONG)PROBE_HW_SPRITE_COUNT * 4UL) + 2UL + (31UL * 2UL) + PROBE_COPPER_SPARKLE_WORDS + PROBE_COPPER_REFLECTION_WORDS + 2UL)
#define PROBE_COPPER_WORDS 1024UL
#define PROBE_COPPER_BYTES (PROBE_COPPER_WORDS * sizeof(UWORD))
#define PROBE_HW_ENEMY_SPRITE_TAIL_WORDS ((ULONG)PROBE_HW_ENEMY_COUNT * PROBE_HW_FULL_SPRITE_WORDS)
#define PROBE_HW_ENEMY_SPRITE_TAIL_BYTES (PROBE_HW_ENEMY_SPRITE_TAIL_WORDS * sizeof(UWORD))
#define SCREEN_ALIGNED_BYTES SCREEN_BYTES
#define SCREEN_POOL_BYTES ((ULONG)PROBE_DISPLAY_BUFFER_COUNT * SCREEN_ALIGNED_BYTES)
#define PROBE_CONTROL_SPRITE_BANK_OFFSET 0UL
#define PROBE_CONTROL_COPPER1_OFFSET (PROBE_CONTROL_SPRITE_BANK_OFFSET + PROBE_HW_SPRITE_BANK_BYTES)
#define PROBE_CONTROL_POOL_BYTES (PROBE_CONTROL_COPPER1_OFFSET + ((ULONG)PROBE_COPPER_COUNT * PROBE_COPPER_BYTES))
#define PROBE_AUDIO_BYTES AMIGA_SFX_OCS_DATA_BYTES
#define PROBE_DMA_ARENA_BYTES (PROBE_ASSET_BYTES + PROBE_CONTROL_POOL_BYTES + SCREEN_POOL_BYTES)
#define PROBE_DYNAMIC_SLOT_MOVING_BLOCK 0
#define PROBE_DYNAMIC_SLOT_ENEMY (PROBE_DYNAMIC_SLOT_MOVING_BLOCK + GAME_MAX_MOVING_BLOCKS)
#define PROBE_DYNAMIC_SLOT_IMPACT (PROBE_DYNAMIC_SLOT_ENEMY + GAME_MAX_ENEMIES)
#define PROBE_DYNAMIC_SLOT_PLAYER (PROBE_DYNAMIC_SLOT_IMPACT + GAME_MAX_IMPACT_FX)
#define PROBE_DYNAMIC_CACHE_MAX (PROBE_DYNAMIC_SLOT_PLAYER + 1)
#define PROBE_SFX_VOICE_COUNT 4
#define PROBE_SFX_ROUND_COOLDOWN_TICKS 40
#define PROBE_SFX_PUSH_COOLDOWN_TICKS 8
#define PROBE_SFX_CRUSH_COOLDOWN_TICKS 4
#define PROBE_SFX_MAJOR_LOCKOUT_TICKS (2 * GAME_FIXED_TICK_HZ)
#define PROBE_SFX_MAJOR_BUSY_MIN_TICKS 32
#define PROBE_SFX_MAJOR_SUPPRESS_EVENTS \
    (GAME_EVENT_ROUND_START | GAME_EVENT_SPECIAL_ALIGNMENT | GAME_EVENT_COMBO | GAME_EVENT_ITEM_COLLECT | \
     GAME_EVENT_TIMER_DANGER_PULSE | GAME_EVENT_BLOCK_IMPACT | GAME_EVENT_META_BANKED | \
     GAME_EVENT_META_SPENT | GAME_EVENT_STAGE_MODIFIER | GAME_EVENT_CRUSH | GAME_EVENT_BLOCK_PUSH)
#define PROBE_MENU_REPEAT_INITIAL_TICKS 5
#define PROBE_MENU_REPEAT_NEXT_TICKS 2
#define PROBE_HI_SCORE_REPEAT_INITIAL_TICKS 2
#define PROBE_HI_SCORE_REPEAT_NEXT_TICKS 0
#define PROBE_HIGH_SCORE_COUNT 5
#define BPLCON2_SPRITES_FRONT 0x0024
#define BLT_USEA 0x0800
#define BLT_USEB 0x0400
#define BLT_USEC 0x0200
#define BLT_USED 0x0100
#define BLT_MINTERM_COOKIE 0x00CA
#define BLT_MINTERM_COPY_B 0x00CC
#define BLT_MINTERM_OR 0x00EE
#define PRA_FIR1_BIT (1 << 7)
#define GAME_FP_SHIFT 8
#define PROBE_MEMF_CLEAR 0x00010000UL
#ifndef DMAF_BLTDONE
#define DMAF_BLTDONE 0x4000
#endif

enum {
    PROBE_UI_TITLE_SCORES = 0,
    PROBE_UI_ROUND_START = 1,
    PROBE_UI_ROUND_CLEAR = 2,
    PROBE_UI_GAME_OVER = 3,
    PROBE_UI_ENTER_HI_SCORE = 4
};

static const AmigaSpriteId kProbeTerrainSprites[3] = {
    AMIGA_SPR_FLOOR,
    AMIGA_SPR_WALL,
    AMIGA_SPR_BORDER
};

static const AmigaSpriteId kProbeBlockSprites[5] = {
    AMIGA_SPR_FLOOR,
    AMIGA_SPR_BLOCK_ICE,
    AMIGA_SPR_BLOCK_SPECIAL,
    AMIGA_SPR_BLOCK_CRACKED,
    AMIGA_SPR_BLOCK_UNBREAKABLE
};

static const AmigaSpriteId kProbeItemSprites[6] = {
    AMIGA_SPR_FLOOR,
    AMIGA_SPR_ITEM_FISH,
    AMIGA_SPR_ITEM_EGG,
    AMIGA_SPR_ITEM_GEM,
    AMIGA_SPR_ITEM_FRUIT,
    AMIGA_SPR_ITEM_MYSTERY
};

typedef struct ProbeDynamicBob {
    WORD x;
    WORD y;
    AmigaSpriteId sprite;
    uint32_t tile_mask;
    UBYTE ty0;
    UBYTE ty1;
    UBYTE valid;
} ProbeDynamicBob;

typedef struct ProbeHudSnapshot {
    ULONG score;
    UWORD seconds;
    UWORD round;
    UWORD lives;
    UWORD mine_stock;
    UWORD mine_capacity;
    UBYTE bomb_blink;
    UBYTE meter_blink;
} ProbeHudSnapshot;

typedef struct ProbeRenderCache {
    ProbeDynamicBob prev[PROBE_DISPLAY_BUFFER_COUNT][PROBE_DYNAMIC_CACHE_MAX];
    uint32_t prev_dynamic_lo[PROBE_DISPLAY_BUFFER_COUNT];
    UBYTE prev_dynamic_hi[PROBE_DISPLAY_BUFFER_COUNT];
    uint32_t dirty_rows[PROBE_DISPLAY_BUFFER_COUNT][GAME_GRID_HEIGHT];
    ProbeHudSnapshot hud[PROBE_DISPLAY_BUFFER_COUNT];
    UWORD hud_raw_ticks[PROBE_DISPLAY_BUFFER_COUNT];
    UWORD hud_next_second_tick[PROBE_DISPLAY_BUFFER_COUNT];
    uint32_t hidden_block_rows[GAME_GRID_HEIGHT];
    UBYTE hidden_blocks_active;
    UBYTE valid[PROBE_DISPLAY_BUFFER_COUNT];
    UBYTE ui_valid[PROBE_DISPLAY_BUFFER_COUNT];
    UBYTE hud_valid[PROBE_DISPLAY_BUFFER_COUNT];
    WORD ui_mode[PROBE_DISPLAY_BUFFER_COUNT];
    ULONG ui_layout_key[PROBE_DISPLAY_BUFFER_COUNT];
    WORD ui_selection[PROBE_DISPLAY_BUFFER_COUNT];
} ProbeRenderCache;

static UWORD *g_probe_copper_sparkle_wait[PROBE_COPPER_COUNT][PROBE_COPPER_SPARKLE_COUNT];
static UWORD *g_probe_copper_sparkle_restore_wait[PROBE_COPPER_COUNT][PROBE_COPPER_SPARKLE_COUNT];
static UWORD *g_probe_copper_sparkle_color[PROBE_COPPER_COUNT][PROBE_COPPER_SPARKLE_COUNT][4];
static UWORD *g_probe_copper_sprite_hi[PROBE_COPPER_COUNT][PROBE_HW_SPRITE_COUNT];
static UWORD *g_probe_copper_sprite_lo[PROBE_COPPER_COUNT][PROBE_HW_SPRITE_COUNT];
static UWORD *g_probe_copper_reflection_first[PROBE_COPPER_COUNT];
static UWORD *g_probe_copper_reflection_lines[PROBE_COPPER_COUNT];
static UWORD *g_probe_copper_reflection_bplcon0[PROBE_COPPER_COUNT];
static WORD g_probe_hw_enemy_sprite_cache[PROBE_COPPER_COUNT][PROBE_HW_ENEMY_COUNT];
static UBYTE g_probe_hw_enemy_sprite_cache_valid[PROBE_COPPER_COUNT][PROBE_HW_ENEMY_COUNT];
static UBYTE g_probe_hw_enemy_active_mask[PROBE_COPPER_COUNT];
static const UBYTE g_probe_hw_enemy_color_lut[3][AMIGA_ASSET_PALETTE_COUNT] = {
    {0, 3, 3, 3, 3, 3, 1, 1, 1, 2, 3, 3, 2, 2, 3, 3, 2, 3, 3, 3, 1, 1, 2, 3, 1, 1, 3, 3, 3, 1, 2, 2},
    {0, 3, 3, 3, 3, 3, 1, 1, 1, 1, 3, 3, 2, 1, 3, 3, 3, 2, 2, 2, 1, 1, 1, 2, 1, 1, 2, 3, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 2, 3, 1, 1, 2, 3, 1, 1, 2, 1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 1, 1, 2, 3}
};
static UBYTE g_probe_font_fill_rows[128][5];
static UBYTE g_probe_font_outline_rows[128][7];
static UBYTE g_probe_font_ready;

typedef struct ProbeSfxState {
    UBYTE *samples;
    UBYTE channel_ticks[PROBE_SFX_VOICE_COUNT];
    UBYTE channel_sample[PROBE_SFX_VOICE_COUNT];
    UBYTE round_cooldown;
    UBYTE push_cooldown;
    UBYTE crush_cooldown;
    UBYTE major_cooldown;
    UBYTE major_busy_ticks;
    UBYTE major_sample;
    WORD major_phase;
    UBYTE major_phase_sample;
    UBYTE major_lockout[AMIGA_SFX_SAMPLE_COUNT];
    UBYTE enabled;
    UBYTE next_channel;
} ProbeSfxState;

typedef struct ProbeHiScoreEntry {
    char initials[4];
    uint32_t score;
} ProbeHiScoreEntry;

typedef struct ProbeHighScoreTable {
    ProbeHiScoreEntry entries[PROBE_HIGH_SCORE_COUNT];
} ProbeHighScoreTable;

typedef struct ProbeHighScoreEntryState {
    UBYTE active;
    UBYTE confirm_armed;
    WORD insert_index;
    uint32_t pending_score;
    char initials[4];
    WORD cursor;
    UWORD blink_ticks;
} ProbeHighScoreEntryState;

static const ProbeHiScoreEntry kProbeOpeningHiScores[PROBE_HIGH_SCORE_COUNT] = {
    {"ACE", 50000u},
    {"ICE", 35000u},
    {"PAN", 24000u},
    {"FOX", 16000u},
    {"JET", 9000u}
};

static ProbeHighScoreTable g_probe_high_scores;
static ProbeHighScoreEntryState g_probe_high_score_entry;
static char g_probe_last_high_score_initials[4] = "AAA";

static AmigaSpriteId probe_player_sprite(Direction d, BOOL alt);
static AmigaSpriteId probe_enemy_sprite(const Enemy *enemy);
static AmigaSpriteId probe_death_sprite_for_fx(const ImpactFx *fx);
static BOOL probe_enemy_visible(const Enemy *enemy, ULONG tick);
static WORD probe_fp_to_screen(GamePixelFp value, WORD max_value);
static void probe_poll_input_latch(InputState *latch, UBYTE *last_dirs, UBYTE *last_fire);

static InputState *g_probe_blitter_input_latch;
static UBYTE *g_probe_blitter_last_dirs;
static UBYTE *g_probe_blitter_last_fire;
static UBYTE g_probe_blitter_poll_phase;

static void write_custom_ptr_words(volatile APTR *reg, APTR ptr) {
    volatile UWORD *words = (volatile UWORD *)reg;
    ULONG addr = (ULONG)ptr;
    words[0] = (UWORD)(addr >> 16);
    words[1] = (UWORD)(addr & 0xFFFF);
}

static void wait_blitter(void) {
    (void)CREGS->dmaconr;
    (void)CREGS->dmaconr;
    while ((CREGS->dmaconr & DMAF_BLTDONE) != 0) {
        g_probe_blitter_poll_phase = (UBYTE)(g_probe_blitter_poll_phase + 1u);
        if ((g_probe_blitter_poll_phase & 15u) == 0u &&
            g_probe_blitter_input_latch &&
            g_probe_blitter_last_dirs &&
            g_probe_blitter_last_fire) {
            probe_poll_input_latch(g_probe_blitter_input_latch,
                                   g_probe_blitter_last_dirs,
                                   g_probe_blitter_last_fire);
        }
    }
}

static void probe_set_blitter_input_latch(InputState *latch, UBYTE *last_dirs, UBYTE *last_fire) {
    g_probe_blitter_input_latch = latch;
    g_probe_blitter_last_dirs = last_dirs;
    g_probe_blitter_last_fire = last_fire;
    g_probe_blitter_poll_phase = 0;
}

static void probe_sanitize_initials(char out_initials[4], const char *input) {
    WORD out_idx = 0;
    if (!out_initials) {
        return;
    }
    if (input) {
        WORD i;
        for (i = 0; input[i] != '\0' && out_idx < 3; ++i) {
            UBYTE ch = (UBYTE)input[i];
            if (ch >= (UBYTE)'a' && ch <= (UBYTE)'z') {
                out_initials[out_idx++] = (char)(ch - (UBYTE)'a' + (UBYTE)'A');
            } else if (ch >= (UBYTE)'A' && ch <= (UBYTE)'Z') {
                out_initials[out_idx++] = (char)ch;
            }
        }
    }
    while (out_idx < 3) {
        out_initials[out_idx++] = 'A';
    }
    out_initials[3] = '\0';
}

static void probe_remember_high_score_initials(const char initials[4]) {
    probe_sanitize_initials(g_probe_last_high_score_initials, initials);
}

static void probe_high_score_table_set_defaults(ProbeHighScoreTable *table) {
    WORD i;
    if (!table) {
        return;
    }
    memset(table, 0, sizeof(*table));
    for (i = 0; i < PROBE_HIGH_SCORE_COUNT; ++i) {
        table->entries[i] = kProbeOpeningHiScores[i];
    }
}

static WORD probe_high_score_find_insert_index(const ProbeHighScoreTable *table, uint32_t score) {
    WORD i;
    if (!table) {
        return -1;
    }
    for (i = 0; i < PROBE_HIGH_SCORE_COUNT; ++i) {
        if (score >= table->entries[i].score) {
            return i;
        }
    }
    return -1;
}

static void probe_high_score_insert(ProbeHighScoreTable *table, WORD index, const char initials[4], uint32_t score) {
    WORD i;
    if (!table || index < 0 || index >= PROBE_HIGH_SCORE_COUNT) {
        return;
    }
    for (i = (WORD)(PROBE_HIGH_SCORE_COUNT - 1); i > index; --i) {
        table->entries[i] = table->entries[i - 1];
    }
    probe_sanitize_initials(table->entries[index].initials, initials);
    table->entries[index].score = score;
}

static char probe_cycle_initial_letter(char letter, WORD delta) {
    WORD value;
    if (letter < 'A' || letter > 'Z') {
        letter = 'A';
    }
    value = (WORD)(letter - 'A');
    value = (WORD)((value + delta) % 26);
    if (value < 0) {
        value = (WORD)(value + 26);
    }
    return (char)('A' + value);
}

static void probe_begin_high_score_entry(ProbeHighScoreEntryState *entry, WORD insert_index, uint32_t score) {
    if (!entry) {
        return;
    }
    memset(entry, 0, sizeof(*entry));
    entry->active = 1;
    entry->confirm_armed = 1;
    entry->insert_index = insert_index;
    entry->pending_score = score;
    entry->cursor = 0;
    probe_sanitize_initials(entry->initials, g_probe_last_high_score_initials);
}

static BOOL probe_update_high_score_entry(ProbeHighScoreEntryState *entry, const InputState *in) {
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
            entry->initials[entry->cursor] = probe_cycle_initial_letter(entry->initials[entry->cursor], 1);
            break;
        case DIR_DOWN:
            entry->initials[entry->cursor] = probe_cycle_initial_letter(entry->initials[entry->cursor], -1);
            break;
        default:
            break;
    }
    if (!entry->confirm_armed) {
        if (!in->fire_pressed && !in->fire_released && !in->start) {
            entry->confirm_armed = 1;
        }
    } else if (in->fire_released || in->start) {
        if (entry->cursor < 2) {
            ++entry->cursor;
            entry->blink_ticks = 0;
            return FALSE;
        }
        entry->active = 0;
        return TRUE;
    }
    return FALSE;
}

static WORD probe_high_score_entry_dynamic_key(const ProbeHighScoreEntryState *entry) {
    ULONG key = 0UL;
    WORD i;
    if (!entry || !entry->active) {
        return -1;
    }
    for (i = 0; i < 3; ++i) {
        UBYTE ch = (UBYTE)entry->initials[i];
        UBYTE letter = 0;
        if (ch >= (UBYTE)'A' && ch <= (UBYTE)'Z') {
            letter = (UBYTE)(ch - (UBYTE)'A');
        }
        key = (key * 26UL) + (ULONG)letter;
    }
    if (entry->cursor < 0) {
        key = key * 3UL;
    } else if (entry->cursor > 2) {
        key = (key * 3UL) + 2UL;
    } else {
        key = (key * 3UL) + (ULONG)entry->cursor;
    }
    return (WORD)(UWORD)key;
}

static BOOL ocs_dma_range_safe(const void *ptr, ULONG bytes) {
    ULONG addr;
    if (!ptr || bytes == 0) {
        return FALSE;
    }
    addr = (ULONG)ptr;
    return (addr < OCS_DMA_SAFE_LIMIT && bytes <= OCS_DMA_SAFE_LIMIT - addr) ? TRUE : FALSE;
}

static UWORD probe_sfx_dma_bit(UBYTE channel) {
    return (UWORD)(DMAF_AUD0 << channel);
}

static void probe_sfx_disable_voice(UBYTE channel) {
    CREGS->aud[channel].ac_vol = 0;
    CREGS->dmacon = probe_sfx_dma_bit(channel);
}

static void probe_sfx_stop_all(void) {
    UBYTE i;
    for (i = 0; i < PROBE_SFX_VOICE_COUNT; ++i) {
        probe_sfx_disable_voice(i);
    }
}

static void probe_sfx_interrupt(ProbeSfxState *sfx) {
    if (!sfx) {
        return;
    }
    memset(sfx->channel_ticks, 0, sizeof(sfx->channel_ticks));
    memset(sfx->channel_sample, 0, sizeof(sfx->channel_sample));
    probe_sfx_stop_all();
}

static UBYTE probe_sfx_sample_ticks(UBYTE sample_id) {
    UBYTE ticks;
    if (sample_id == AMIGA_SFX_SAMPLE_NONE || sample_id >= AMIGA_SFX_SAMPLE_COUNT) {
        return 0;
    }
    ticks = g_amiga_sfx_ocs_ticks[sample_id];
    return ticks ? ticks : 1;
}

static void probe_sfx_play_sample_now(ProbeSfxState *sfx, UBYTE sample_id, UBYTE channel, UBYTE volume) {
    UBYTE *sample;
    UWORD sample_words;
    if (!sfx || !sfx->enabled || sample_id == AMIGA_SFX_SAMPLE_NONE || sample_id >= AMIGA_SFX_SAMPLE_COUNT) {
        return;
    }
    if (channel >= PROBE_SFX_VOICE_COUNT) {
        channel = 0;
    }
    sample = sfx->samples + g_amiga_sfx_ocs_offsets[sample_id];
    sample_words = g_amiga_sfx_ocs_lengths_words[sample_id];
    if (sample_words == 0) {
        return;
    }
    if (sfx->channel_ticks[channel] != 0 && sfx->channel_sample[channel] == sample_id) {
        return;
    }
    sfx->channel_ticks[channel] = 0;
    sfx->channel_sample[channel] = AMIGA_SFX_SAMPLE_NONE;
    probe_sfx_disable_voice(channel);
    CREGS->aud[channel].ac_ptr = (UWORD *)sample;
    CREGS->aud[channel].ac_len = sample_words;
    CREGS->aud[channel].ac_per = AMIGA_SFX_OCS_SAMPLE_PERIOD;
    CREGS->aud[channel].ac_vol = volume ? volume : 64;
    CREGS->dmacon = DMAF_SETCLR | probe_sfx_dma_bit(channel);
    sfx->channel_ticks[channel] = probe_sfx_sample_ticks(sample_id);
    sfx->channel_sample[channel] = sample_id;
}

static void probe_sfx_tick(ProbeSfxState *sfx) {
    UBYTE i;
    if (!sfx || !sfx->enabled) {
        return;
    }

    for (i = 0; i < PROBE_SFX_VOICE_COUNT; ++i) {
        if (sfx->channel_ticks[i] != 0) {
            --sfx->channel_ticks[i];
            if (sfx->channel_ticks[i] == 0) {
                probe_sfx_disable_voice(i);
                sfx->channel_sample[i] = AMIGA_SFX_SAMPLE_NONE;
            }
        }
    }
    if (sfx->round_cooldown != 0) {
        --sfx->round_cooldown;
    }
    if (sfx->push_cooldown != 0) {
        --sfx->push_cooldown;
    }
    if (sfx->crush_cooldown != 0) {
        --sfx->crush_cooldown;
    }
    if (sfx->major_cooldown != 0) {
        --sfx->major_cooldown;
    }
    if (sfx->major_busy_ticks != 0) {
        --sfx->major_busy_ticks;
    }
    for (i = 0; i < AMIGA_SFX_SAMPLE_COUNT; ++i) {
        if (sfx->major_lockout[i] != 0) {
            --sfx->major_lockout[i];
        }
    }
}

static BOOL probe_sfx_play_major(ProbeSfxState *sfx, UBYTE sample_id, UBYTE volume) {
    UBYTE ticks;
    UBYTE busy_ticks;
    if (!sfx || !sfx->enabled) {
        return FALSE;
    }
    if (sample_id == AMIGA_SFX_SAMPLE_NONE || sample_id >= AMIGA_SFX_SAMPLE_COUNT) {
        return FALSE;
    }
    if (sfx->major_lockout[sample_id] != 0) {
        if (sfx->major_busy_ticks < PROBE_SFX_MAJOR_BUSY_MIN_TICKS) {
            sfx->major_busy_ticks = PROBE_SFX_MAJOR_BUSY_MIN_TICKS;
        }
        return TRUE;
    }
    if (sfx->major_phase_sample == sample_id) {
        if (sfx->major_busy_ticks < PROBE_SFX_MAJOR_BUSY_MIN_TICKS) {
            sfx->major_busy_ticks = PROBE_SFX_MAJOR_BUSY_MIN_TICKS;
        }
        return TRUE;
    }
    if (sfx->major_cooldown != 0 && sfx->major_sample == sample_id) {
        if (sfx->major_busy_ticks < PROBE_SFX_MAJOR_BUSY_MIN_TICKS) {
            sfx->major_busy_ticks = PROBE_SFX_MAJOR_BUSY_MIN_TICKS;
        }
        return TRUE;
    }
    ticks = probe_sfx_sample_ticks(sample_id);
    busy_ticks = (UBYTE)(ticks + 8);
    if (busy_ticks < PROBE_SFX_MAJOR_BUSY_MIN_TICKS) {
        busy_ticks = PROBE_SFX_MAJOR_BUSY_MIN_TICKS;
    }
    probe_sfx_interrupt(sfx);
    probe_sfx_play_sample_now(sfx, sample_id, 0, volume);
    sfx->major_sample = sample_id;
    sfx->major_phase_sample = sample_id;
    sfx->major_cooldown = (UBYTE)(ticks + 8);
    sfx->major_busy_ticks = busy_ticks;
    sfx->major_lockout[sample_id] = PROBE_SFX_MAJOR_LOCKOUT_TICKS;
    return TRUE;
}

static void probe_sfx_dispatch_events(ProbeSfxState *sfx, uint32_t event_flags) {
    if (!sfx || !sfx->enabled || event_flags == 0) {
        return;
    }
    if (sfx->major_busy_ticks != 0) {
        event_flags &= (uint32_t)~PROBE_SFX_MAJOR_SUPPRESS_EVENTS;
    }

    if ((event_flags & GAME_EVENT_ROUND_START) != 0u && sfx->round_cooldown == 0) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_ROUND_START, 0, 58);
        sfx->round_cooldown = PROBE_SFX_ROUND_COOLDOWN_TICKS;
    }
    if ((event_flags & GAME_EVENT_BLOCK_PUSH) != 0u && sfx->push_cooldown == 0) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_BLOCK_PUSH_SWISH, 1, 50);
        sfx->push_cooldown = PROBE_SFX_PUSH_COOLDOWN_TICKS;
    }
    if ((event_flags & GAME_EVENT_CRUSH) != 0u && sfx->crush_cooldown == 0) {
        UBYTE channel = (UBYTE)(2 + (sfx->next_channel & 1u));
        sfx->next_channel = (UBYTE)(sfx->next_channel + 1u);
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_CRUSH, channel, 56);
        sfx->crush_cooldown = PROBE_SFX_CRUSH_COOLDOWN_TICKS;
    }
}

static void probe_sfx_dispatch_game_events(ProbeSfxState *sfx, const GameState *game, uint32_t event_flags) {
    WORD phase;
    BOOL major_handled = FALSE;
    if (!sfx || !sfx->enabled || event_flags == 0) {
        return;
    }
    phase = game ? (WORD)game->phase : (WORD)-1;
    if (sfx->major_phase != phase) {
        sfx->major_phase = phase;
        sfx->major_phase_sample = AMIGA_SFX_SAMPLE_NONE;
    }
    if ((event_flags & GAME_EVENT_GAME_OVER) != 0u) {
        (void)probe_sfx_play_major(sfx, AMIGA_SFX_SAMPLE_GAME_OVER_JINGLE, 64);
        return;
    }
    if ((event_flags & GAME_EVENT_LIFE_LOST) != 0u) {
        major_handled = probe_sfx_play_major(sfx, AMIGA_SFX_SAMPLE_LIFE_LOSS_FANFARE, 64) || major_handled;
        event_flags &= (uint32_t)~(PROBE_SFX_MAJOR_SUPPRESS_EVENTS | GAME_EVENT_PLAYER_DIED);
    }
    if ((event_flags & GAME_EVENT_PLAYER_DIED) != 0u) {
        major_handled = probe_sfx_play_major(sfx, AMIGA_SFX_SAMPLE_DEATH_JINGLE, 64) || major_handled;
        event_flags &= (uint32_t)~PROBE_SFX_MAJOR_SUPPRESS_EVENTS;
    }
    if ((event_flags & GAME_EVENT_ROUND_CLEAR) != 0u) {
        major_handled = probe_sfx_play_major(sfx, AMIGA_SFX_SAMPLE_LEVEL_UP_FANFARE, 64) || major_handled;
        event_flags &= (uint32_t)~PROBE_SFX_MAJOR_SUPPRESS_EVENTS;
    }
    if (major_handled || sfx->major_busy_ticks != 0) {
        event_flags &= (uint32_t)~PROBE_SFX_MAJOR_SUPPRESS_EVENTS;
    }
    if (event_flags == 0) {
        return;
    }
    if ((event_flags & GAME_EVENT_SPECIAL_ALIGNMENT) != 0u) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_REWARD_JINGLE, 0, 64);
    }
    if ((event_flags & GAME_EVENT_COMBO) != 0u) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_COMBO_JINGLE, 0, 64);
        event_flags &= (uint32_t)~GAME_EVENT_CRUSH;
    }
    if ((event_flags & GAME_EVENT_ITEM_COLLECT) != 0u) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_SLOT_CHIME, 2, 64);
    }
    if ((event_flags & GAME_EVENT_TIMER_DANGER_PULSE) != 0u) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_TIMER_DANGER, 2, 56);
    }
    if ((event_flags & GAME_EVENT_MINE_PLACED) != 0u) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_MINE_PLACED, 3, 52);
        event_flags &= (uint32_t)~GAME_EVENT_BLOCK_IMPACT;
    }
    if ((event_flags & GAME_EVENT_MINE_EXPLODED) != 0u) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_MINE_EXPLODED, 3, 56);
        event_flags &= (uint32_t)~GAME_EVENT_BLOCK_IMPACT;
    }
    if ((event_flags & GAME_EVENT_BLOCK_IMPACT) != 0u) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_BLOCK_IMPACT, 3, 52);
    }
    if ((event_flags & GAME_EVENT_META_BANKED) != 0u) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_REWARD_JINGLE, 0, 64);
    }
    if ((event_flags & GAME_EVENT_META_SPENT) != 0u) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_META_SPENT, 0, 58);
    }
    if ((event_flags & GAME_EVENT_STAGE_MODIFIER) != 0u) {
        probe_sfx_play_sample_now(sfx, AMIGA_SFX_SAMPLE_STAGE_MODIFIER, 0, 58);
    }
    probe_sfx_dispatch_events(sfx, event_flags);
}

static BOOL probe_sfx_init(ProbeSfxState *sfx, UBYTE *samples) {
    if (!sfx || !ocs_dma_range_safe(samples, PROBE_AUDIO_BYTES) || (((ULONG)samples & 1UL) != 0)) {
        return FALSE;
    }
    memset(sfx, 0, sizeof(*sfx));
    sfx->samples = samples;
    sfx->enabled = 1;
    sfx->major_phase = (WORD)-1;
    probe_sfx_stop_all();
    return TRUE;
}

static void probe_sfx_shutdown(ProbeSfxState *sfx) {
    if (!sfx) {
        return;
    }
    probe_sfx_stop_all();
    memset(sfx, 0, sizeof(*sfx));
}

static BOOL probe_load_sfx_bank(UBYTE *dst) {
    BPTR file;
    LONG got;
    if (!dst || !ocs_dma_range_safe(dst, PROBE_AUDIO_BYTES)) {
        return FALSE;
    }
    file = Open((STRPTR)"Icepanic.sfx", MODE_OLDFILE);
    if (!file) {
        return FALSE;
    }
    got = Read(file, dst, (LONG)PROBE_AUDIO_BYTES);
    Close(file);
    return (got == (LONG)PROBE_AUDIO_BYTES) ? TRUE : FALSE;
}

static BOOL probe_read_exact(BPTR file, UBYTE *dst, ULONG bytes) {
    LONG got;
    if (!file || !dst) {
        return FALSE;
    }
    got = Read(file, dst, (LONG)bytes);
    return (got == (LONG)bytes) ? TRUE : FALSE;
}

static BOOL probe_load_gfx_bank(UBYTE *chip_masks,
                                UBYTE *chip_bobs,
                                UBYTE *chip_tile_masks,
                                UBYTE *chip_tiles,
                                UBYTE *title_data,
                                UBYTE *title_mask) {
    BPTR file;
    BOOL ok;
    if (!ocs_dma_range_safe(chip_masks, PROBE_MASK_BYTES) ||
        !ocs_dma_range_safe(chip_bobs, PROBE_BOB_BYTES) ||
        !ocs_dma_range_safe(chip_tile_masks, PROBE_TILE_MASK_BYTES) ||
        !ocs_dma_range_safe(chip_tiles, PROBE_TILE_BYTES) ||
        !title_data || !title_mask) {
        return FALSE;
    }
    file = Open((STRPTR)"Icepanic.gfx", MODE_OLDFILE);
    if (!file) {
        return FALSE;
    }
    ok = probe_read_exact(file, chip_masks, PROBE_MASK_BYTES) &&
         probe_read_exact(file, chip_bobs, PROBE_BOB_BYTES) &&
         probe_read_exact(file, chip_tile_masks, PROBE_TILE_MASK_BYTES) &&
         probe_read_exact(file, chip_tiles, PROBE_TILE_BYTES) &&
         probe_read_exact(file, title_data, AMIGA_TITLE_OVERLAY_BYTES) &&
         probe_read_exact(file, title_mask, AMIGA_TITLE_OVERLAY_MASK_BYTES);
    Close(file);
    return ok;
}

static UBYTE *alloc_ocs_dma_pool_probe(ULONG bytes,
                                       UBYTE **parked,
                                       ULONG *parked_sizes,
                                       WORD *parked_count) {
    UBYTE *ptr;
    while (*parked_count < PROBE_MAX_PARKED_POOLS) {
        ptr = (UBYTE *)AllocMem(bytes, MEMF_CHIP);
        if (!ptr) {
            return 0;
        }
        if (ocs_dma_range_safe(ptr, bytes)) {
            return ptr;
        }
        parked[*parked_count] = ptr;
        parked_sizes[*parked_count] = bytes;
        *parked_count = (WORD)(*parked_count + 1);
    }
    ptr = (UBYTE *)AllocMem(bytes, MEMF_CHIP);
    if (ptr) {
        FreeMem(ptr, bytes);
    }
    return 0;
}

static void free_parked_pools_probe(UBYTE **parked, ULONG *parked_sizes, WORD *parked_count) {
    while (*parked_count > 0) {
        *parked_count = (WORD)(*parked_count - 1);
        FreeMem(parked[*parked_count], parked_sizes[*parked_count]);
        parked[*parked_count] = 0;
        parked_sizes[*parked_count] = 0;
    }
}

static int clamp_int_probe(int value, int min_value, int max_value) {
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static void blit_shifted_bob_twopass_probe_ex(UBYTE *screen, const UBYTE *bobs, const UBYTE *masks,
                                              AmigaSpriteId sprite, int x, int y, BOOL wait_end) {
    int shift = x & 15;
    UBYTE *dst = screen + ((ULONG)y * ROW_STRIDE_BYTES) + ((ULONG)(x >> 4) * 2);
    const UBYTE *src = bobs + ((ULONG)sprite * AMIGA_ASSET_BOB_BYTES);
    const UBYTE *mask = masks + ((ULONG)sprite * AMIGA_ASSET_MASK_BYTES);

    wait_blitter();
    if (shift == 0) {
        CREGS->bltcon0 = (UWORD)(BLT_USEA | BLT_USEB | BLT_USEC | BLT_USED | BLT_MINTERM_COOKIE);
        CREGS->bltcon1 = 0;
        CREGS->bltadat = 0;
        CREGS->bltbdat = 0;
        CREGS->bltafwm = 0xFFFF;
        CREGS->bltalwm = 0xFFFF;
        write_custom_ptr_words(&CREGS->bltapt, (APTR)mask);
        write_custom_ptr_words(&CREGS->bltbpt, (APTR)src);
        write_custom_ptr_words(&CREGS->bltcpt, (APTR)dst);
        write_custom_ptr_words(&CREGS->bltdpt, (APTR)dst);
        CREGS->bltamod = AMIGA_ASSET_ROW_BYTES_WIDE - AMIGA_ASSET_ROW_BYTES_TILE;
        CREGS->bltbmod = AMIGA_ASSET_ROW_BYTES_WIDE - AMIGA_ASSET_ROW_BYTES_TILE;
        CREGS->bltcmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_TILE;
        CREGS->bltdmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_TILE;
        CREGS->bltsize = (UWORD)(((GAME_TILE_SIZE * DEPTH) << 6) | AMIGA_ASSET_ROW_WORDS_TILE);
        if (wait_end) {
            wait_blitter();
        }
        return;
    }
    CREGS->bltcon0 = (UWORD)((shift << 12) | BLT_USEA | BLT_USEB | BLT_USEC | BLT_USED | BLT_MINTERM_COOKIE);
    CREGS->bltcon1 = (UWORD)(shift << 12);
    CREGS->bltadat = 0;
    CREGS->bltbdat = 0;
    CREGS->bltafwm = 0xFFFF;
    CREGS->bltalwm = 0xFFFF;
    write_custom_ptr_words(&CREGS->bltapt, (APTR)mask);
    write_custom_ptr_words(&CREGS->bltbpt, (APTR)src);
    write_custom_ptr_words(&CREGS->bltcpt, (APTR)dst);
    write_custom_ptr_words(&CREGS->bltdpt, (APTR)dst);
    CREGS->bltamod = 0;
    CREGS->bltbmod = 0;
    CREGS->bltcmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_WIDE;
    CREGS->bltdmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_WIDE;
    CREGS->bltsize = (UWORD)(((GAME_TILE_SIZE * DEPTH) << 6) | AMIGA_ASSET_ROW_WORDS_WIDE);
    if (wait_end) {
        wait_blitter();
    }
}

static void blit_shifted_bob_twopass_probe(UBYTE *screen, const UBYTE *bobs, const UBYTE *masks,
                                           AmigaSpriteId sprite, int x, int y) {
    blit_shifted_bob_twopass_probe_ex(screen, bobs, masks, sprite, x, y, TRUE);
}

static void blit_shifted_bob_twopass_probe_async(UBYTE *screen, const UBYTE *bobs, const UBYTE *masks,
                                                 AmigaSpriteId sprite, int x, int y) {
    blit_shifted_bob_twopass_probe_ex(screen, bobs, masks, sprite, x, y, FALSE);
}

static void blit_opaque_tile_probe_ex(UBYTE *screen, const UBYTE *tiles, AmigaSpriteId sprite, int x, int y, BOOL wait_end) {
    UBYTE *dst;
    const UBYTE *src;
    if (!screen || !tiles) {
        return;
    }
    x = clamp_int_probe(x, 0, SCREEN_W - GAME_TILE_SIZE);
    y = clamp_int_probe(y, 0, SCREEN_H - GAME_TILE_SIZE);
    x &= ~(GAME_TILE_SIZE - 1);
    dst = screen + ((ULONG)y * ROW_STRIDE_BYTES) + ((ULONG)(x >> 4) * 2);
    src = tiles + ((ULONG)sprite * AMIGA_ASSET_TILE_BYTES);

    wait_blitter();
    CREGS->bltcon0 = (UWORD)(BLT_USEB | BLT_USED | BLT_MINTERM_COPY_B);
    CREGS->bltcon1 = 0;
    CREGS->bltafwm = 0xFFFF;
    CREGS->bltalwm = 0xFFFF;
    write_custom_ptr_words(&CREGS->bltbpt, (APTR)src);
    write_custom_ptr_words(&CREGS->bltdpt, (APTR)dst);
    CREGS->bltbmod = 0;
    CREGS->bltdmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_TILE;
    CREGS->bltsize = (UWORD)(((GAME_TILE_SIZE * DEPTH) << 6) | AMIGA_ASSET_ROW_WORDS_TILE);
    if (wait_end) {
        wait_blitter();
    }
}

static void blit_opaque_tile_probe_async(UBYTE *screen, const UBYTE *tiles, AmigaSpriteId sprite, int x, int y) {
    blit_opaque_tile_probe_ex(screen, tiles, sprite, x, y, FALSE);
}

static void blit_full_screen_copy_probe(UBYTE *dst_screen, const UBYTE *src_screen, BOOL wait_end) {
    if (!dst_screen || !src_screen || dst_screen == src_screen) {
        return;
    }
    wait_blitter();
    CREGS->bltcon0 = (UWORD)(BLT_USEB | BLT_USED | BLT_MINTERM_COPY_B);
    CREGS->bltcon1 = 0;
    CREGS->bltafwm = 0xFFFF;
    CREGS->bltalwm = 0xFFFF;
    write_custom_ptr_words(&CREGS->bltbpt, (APTR)src_screen);
    write_custom_ptr_words(&CREGS->bltdpt, (APTR)dst_screen);
    CREGS->bltbmod = 0;
    CREGS->bltdmod = 0;
    CREGS->bltsize = (UWORD)(((SCREEN_H * DEPTH) << 6) | (ROW_BYTES / 2));
    if (wait_end) {
        wait_blitter();
    }
}

static void blit_screen_band_copy_probe(UBYTE *dst_screen, const UBYTE *src_screen, WORD y0, WORD y1, BOOL wait_end) {
    UBYTE *dst;
    const UBYTE *src;
    WORD h;
    if (!dst_screen || !src_screen || dst_screen == src_screen) {
        return;
    }
    if (y0 < 0) y0 = 0;
    if (y1 >= SCREEN_H) y1 = SCREEN_H - 1;
    if (y1 < y0) {
        return;
    }
    h = (WORD)(y1 - y0 + 1);
    src = src_screen + ((ULONG)y0 * ROW_STRIDE_BYTES);
    dst = dst_screen + ((ULONG)y0 * ROW_STRIDE_BYTES);
    wait_blitter();
    CREGS->bltcon0 = (UWORD)(BLT_USEB | BLT_USED | BLT_MINTERM_COPY_B);
    CREGS->bltcon1 = 0;
    CREGS->bltafwm = 0xFFFF;
    CREGS->bltalwm = 0xFFFF;
    write_custom_ptr_words(&CREGS->bltbpt, (APTR)src);
    write_custom_ptr_words(&CREGS->bltdpt, (APTR)dst);
    CREGS->bltbmod = 0;
    CREGS->bltdmod = 0;
    CREGS->bltsize = (UWORD)(((h * DEPTH) << 6) | (ROW_BYTES / 2));
    if (wait_end) {
        wait_blitter();
    }
}

static void put_pixel_probe(UBYTE *screen, WORD x, WORD y, UBYTE color);

static UWORD read_be_word_probe(const UBYTE *p) {
    return (UWORD)(((UWORD)p[0] << 8) | (UWORD)p[1]);
}

static void write_be_word_probe(UBYTE *p, UWORD word) {
    p[0] = (UBYTE)(word >> 8);
    p[1] = (UBYTE)(word & 0xFF);
}

static void blit_title_cell_tile_twopass_probe(UBYTE *screen, const UBYTE *tile, const UBYTE *mask, int x, int y) {
    UBYTE *dst = screen + ((ULONG)y * ROW_STRIDE_BYTES) + ((ULONG)(x >> 4) * 2);

    wait_blitter();
    CREGS->bltcon0 = (UWORD)(BLT_USEA | BLT_USEB | BLT_USEC | BLT_USED | BLT_MINTERM_COOKIE);
    CREGS->bltcon1 = 0;
    CREGS->bltafwm = 0xFFFF;
    CREGS->bltalwm = 0xFFFF;
    write_custom_ptr_words(&CREGS->bltapt, (APTR)mask);
    write_custom_ptr_words(&CREGS->bltbpt, (APTR)tile);
    write_custom_ptr_words(&CREGS->bltcpt, (APTR)dst);
    write_custom_ptr_words(&CREGS->bltdpt, (APTR)dst);
    CREGS->bltamod = 0;
    CREGS->bltbmod = 0;
    CREGS->bltcmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_TILE;
    CREGS->bltdmod = ROW_BYTES - AMIGA_ASSET_ROW_BYTES_TILE;
    CREGS->bltsize = (UWORD)(((GAME_TILE_SIZE * DEPTH) << 6) | AMIGA_ASSET_ROW_WORDS_TILE);
    wait_blitter();
}

static BOOL build_title_cell_tile_probe(const UBYTE *title, const UBYTE *title_mask,
                                        UBYTE *cell_tile, UBYTE *cell_mask,
                                        WORD cell_x, WORD cell_y) {
    WORD local_y;
    BOOL has_pixels = FALSE;
    for (local_y = 0; local_y < GAME_TILE_SIZE; ++local_y) {
        WORD sy = (WORD)(cell_y + local_y);
        WORD plane;
        UWORD row_mask = 0;
        UBYTE *mask_row = cell_mask + ((ULONG)local_y * DEPTH * AMIGA_ASSET_ROW_BYTES_TILE);
        if (sy < AMIGA_TITLE_OVERLAY_HEIGHT) {
            WORD local_x;
            ULONG title_row_base = (ULONG)sy * (ULONG)DEPTH * (ULONG)AMIGA_TITLE_OVERLAY_ROW_BYTES;
            for (local_x = 0; local_x < GAME_TILE_SIZE; ++local_x) {
                WORD sx = (WORD)(cell_x + local_x);
                if (sx < AMIGA_TITLE_OVERLAY_WIDTH) {
                    WORD word_index = (WORD)(sx >> 4);
                    UWORD bit = (UWORD)(0x8000u >> (sx & 15));
                    const UBYTE *mask_word = title_mask + title_row_base + ((ULONG)word_index * 2UL);
                    if ((read_be_word_probe(mask_word) & bit) != 0) {
                        row_mask = (UWORD)(row_mask | (0x8000u >> local_x));
                        has_pixels = TRUE;
                    }
                }
            }
        }
        for (plane = 0; plane < DEPTH; ++plane) {
            UWORD art_word = 0;
            UBYTE *tile_word = cell_tile + (((ULONG)local_y * DEPTH + (ULONG)plane) * AMIGA_ASSET_ROW_BYTES_TILE);
            if (sy < AMIGA_TITLE_OVERLAY_HEIGHT) {
                WORD local_x;
                ULONG title_row_base = (ULONG)sy * (ULONG)DEPTH * (ULONG)AMIGA_TITLE_OVERLAY_ROW_BYTES;
                for (local_x = 0; local_x < GAME_TILE_SIZE; ++local_x) {
                    WORD sx = (WORD)(cell_x + local_x);
                    if (sx < AMIGA_TITLE_OVERLAY_WIDTH) {
                        WORD word_index = (WORD)(sx >> 4);
                        UWORD bit = (UWORD)(0x8000u >> (sx & 15));
                        const UBYTE *art_src = title + title_row_base + ((ULONG)plane * AMIGA_TITLE_OVERLAY_ROW_BYTES) + ((ULONG)word_index * 2UL);
                        if ((read_be_word_probe(art_src) & bit) != 0) {
                            art_word = (UWORD)(art_word | (0x8000u >> local_x));
                        }
                    }
                }
            }
            write_be_word_probe(tile_word, art_word);
            write_be_word_probe(mask_row + ((ULONG)plane * AMIGA_ASSET_ROW_BYTES_TILE), row_mask);
        }
    }
    return has_pixels;
}

static void blit_title_overlay_probe(UBYTE *screen, const UBYTE *title, const UBYTE *title_mask,
                                     UBYTE *cell_tile, UBYTE *cell_mask, int x, int y) {
    WORD cell_y;
    if (!screen || !title || !title_mask || !cell_tile || !cell_mask) {
        return;
    }
    x = clamp_int_probe(x, 0, SCREEN_W - AMIGA_TITLE_OVERLAY_WIDTH);
    y = clamp_int_probe(y, 0, SCREEN_H - AMIGA_TITLE_OVERLAY_HEIGHT);
    wait_blitter();
    for (cell_y = 0; cell_y < AMIGA_TITLE_OVERLAY_HEIGHT; cell_y = (WORD)(cell_y + GAME_TILE_SIZE)) {
        WORD cell_x;
        for (cell_x = 0; cell_x < AMIGA_TITLE_OVERLAY_WIDTH; cell_x = (WORD)(cell_x + GAME_TILE_SIZE)) {
            if (build_title_cell_tile_probe(title, title_mask, cell_tile, cell_mask, cell_x, cell_y)) {
                blit_title_cell_tile_twopass_probe(screen, cell_tile, cell_mask, x + cell_x, y + cell_y);
            }
        }
    }
}

struct GfxBase *GfxBase;

static void install_copper(UWORD *cop) {
    CREGS->cop1lc = (ULONG)cop;
    CREGS->copjmp1 = 0;
}

static void queue_copper_next_frame(UWORD *cop) {
    CREGS->cop1lc = (ULONG)cop;
}

static UWORD read_raster_line_probe(void) {
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

static UBYTE *probe_hw_sparkle_slot(UBYTE *sprite_bank, WORD slot) {
    return sprite_bank + ((ULONG)slot * PROBE_HW_FULL_SPRITE_BYTES);
}

static UBYTE *probe_hw_null_slot(UBYTE *sprite_bank, WORD slot) {
    return sprite_bank +
           ((ULONG)PROBE_HW_SPARKLE_COUNT * PROBE_HW_FULL_SPRITE_BYTES) +
           ((ULONG)slot * PROBE_HW_NULL_SPRITE_BYTES);
}

static UBYTE *probe_hw_null_sprite_ptr(UBYTE *sprite_bank) {
    return probe_hw_null_slot(sprite_bank, 0);
}

static UBYTE *probe_hw_sprite_channel_ptr(UBYTE *sprite_bank, WORD channel) {
    if (channel >= PROBE_HW_SPARKLE_FIRST_CHANNEL) {
        return probe_hw_sparkle_slot(sprite_bank, (WORD)(channel - PROBE_HW_SPARKLE_FIRST_CHANNEL));
    }
    return probe_hw_null_slot(sprite_bank, channel);
}

static UBYTE *probe_hw_enemy_slot(UWORD *cop, WORD slot) {
    return ((UBYTE *)cop) +
           (PROBE_COPPER_BYTES - PROBE_HW_ENEMY_SPRITE_TAIL_BYTES) +
           ((ULONG)slot * PROBE_HW_FULL_SPRITE_BYTES);
}

static WORD probe_hw_enemy_slot_for_channel(WORD channel) {
    return (WORD)(channel - PROBE_HW_ENEMY_FIRST_CHANNEL);
}

static WORD probe_hw_enemy_channel_group(WORD channel) {
    if (channel >= 6) {
        return 2;
    }
    if (channel >= 4) {
        return 1;
    }
    return 0;
}

static BOOL probe_enemy_type_uses_hw_sprite(EnemyType type) {
    return (type == ENEMY_TYPE_CHASER) ? FALSE : TRUE;
}

static WORD probe_preferred_hw_enemy_channel(EnemyType type, WORD attempt) {
    static const UBYTE kWandererChannels[PROBE_HW_ENEMY_COUNT] = {4, 5, 2, 3, 6, 7};
    static const UBYTE kHunterChannels[PROBE_HW_ENEMY_COUNT] = {6, 7, 4, 5, 2, 3};
    static const UBYTE kGhostChannels[PROBE_HW_ENEMY_COUNT] = {2, 3, 6, 7, 4, 5};
    const UBYTE *channels = kGhostChannels;
    if (attempt < 0 || attempt >= PROBE_HW_ENEMY_COUNT) {
        return -1;
    }
    if (type == ENEMY_TYPE_WANDERER) {
        channels = kWandererChannels;
    } else if (type == ENEMY_TYPE_HUNTER) {
        channels = kHunterChannels;
    }
    return (WORD)channels[attempt];
}

static WORD probe_select_hw_enemy_channel(EnemyType type, UBYTE used_channels) {
    WORD attempt;
    for (attempt = 0; attempt < PROBE_HW_ENEMY_COUNT; ++attempt) {
        WORD channel = probe_preferred_hw_enemy_channel(type, attempt);
        if (channel >= PROBE_HW_ENEMY_FIRST_CHANNEL &&
            channel < PROBE_HW_SPRITE_COUNT &&
            (used_channels & (UBYTE)(1u << channel)) == 0u) {
            return channel;
        }
    }
    return -1;
}

static void write_probe_hw_sprite_control_ex(UWORD *dst, int x, int y, int height, BOOL attached) {
    int hwx = clamp_int_probe(x, 0, SCREEN_W - GAME_TILE_SIZE) + 0x80;
    int vstart = clamp_int_probe(y, 0, SCREEN_H - height) + 0x2C;
    int vstop = vstart + height;
    dst[0] = (UWORD)(((vstart & 0xFF) << 8) | ((hwx >> 1) & 0xFF));
    dst[1] = (UWORD)(((vstop & 0xFF) << 8) |
                     ((vstart & 0x100) ? 0x0004 : 0x0000) |
                     ((vstop & 0x100) ? 0x0002 : 0x0000) |
                     (hwx & 1) |
                     (attached ? 0x0080 : 0x0000));
}

static void write_probe_hw_sprite_control(UWORD *dst, int x, int y, int height) {
    write_probe_hw_sprite_control_ex(dst, x, y, height, FALSE);
}

static void clear_probe_hw_sprite_slot(UWORD *dst) {
    ULONG i;
    for (i = 0; i < PROBE_HW_FULL_SPRITE_WORDS; ++i) {
        dst[i] = 0x0000;
    }
}

static void build_probe_null_hw_sprite(UWORD *dst) {
    ULONG i;
    for (i = 0; i < PROBE_HW_NULL_SPRITE_WORDS; ++i) {
        dst[i] = 0x0000;
    }
    write_probe_hw_sprite_control(dst, 0, 184, 1);
    dst[2] = 0x0000;
    dst[3] = 0x0000;
    dst[4] = 0x0000;
    dst[5] = 0x0000;
}

static UBYTE probe_hw_sparkle_color(UBYTE source_color) {
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

static void write_probe_hw_sparkle_shape(UWORD *dst, const UBYTE *tiles) {
    WORD row;
    WORD x;
    WORD plane;
    for (row = 0; row < PROBE_HW_SPRITE_HEIGHT; ++row) {
        UWORD plane_words[DEPTH];
        dst[2 + (row * 2)] = 0x0000;
        dst[3 + (row * 2)] = 0x0000;
        for (plane = 0; plane < DEPTH; ++plane) {
            const UBYTE *src = tiles +
                               ((ULONG)AMIGA_SPR_EFFECT_SPARK * AMIGA_ASSET_TILE_BYTES) +
                               ((ULONG)row * DEPTH * AMIGA_ASSET_ROW_BYTES_TILE) +
                               ((ULONG)plane * AMIGA_ASSET_ROW_BYTES_TILE);
            plane_words[plane] = read_be_word_probe(src);
        }
        for (x = 0; x < GAME_TILE_SIZE; ++x) {
            UBYTE source_color = 0;
            UBYTE sprite_color;
            UWORD bit = (UWORD)(0x8000u >> x);
            for (plane = 0; plane < DEPTH; ++plane) {
                if ((plane_words[plane] & bit) != 0) {
                    source_color = (UBYTE)(source_color | (1u << plane));
                }
            }
            sprite_color = probe_hw_sparkle_color(source_color);
            if ((sprite_color & 2u) != 0) {
                dst[2 + (row * 2)] = (UWORD)(dst[2 + (row * 2)] | bit);
            }
            if ((sprite_color & 1u) != 0) {
                dst[3 + (row * 2)] = (UWORD)(dst[3 + (row * 2)] | bit);
            }
        }
    }
    dst[PROBE_HW_FULL_SPRITE_WORDS - 2] = 0x0000;
    dst[PROBE_HW_FULL_SPRITE_WORDS - 1] = 0x0000;
}

static void build_probe_sparkle_hw_sprite(UWORD *dst, const UBYTE *tiles) {
    clear_probe_hw_sprite_slot(dst);
    dst[0] = 0x0000;
    dst[1] = 0x0000;
    write_probe_hw_sparkle_shape(dst, tiles);
}

static void build_probe_hw_sprite_bank(UBYTE *sprite_bank, const UBYTE *tiles) {
    WORD p;
    for (p = 0; p < PROBE_HW_NULL_SPRITE_COUNT; ++p) {
        build_probe_null_hw_sprite((UWORD *)probe_hw_null_slot(sprite_bank, p));
    }
    for (p = 0; p < PROBE_HW_SPARKLE_COUNT; ++p) {
        build_probe_sparkle_hw_sprite((UWORD *)probe_hw_sparkle_slot(sprite_bank, p), tiles);
    }
}

static void probe_invalidate_hw_enemy_sprite_cache(WORD cop_index) {
    WORD p;
    if (cop_index < 0 || cop_index >= PROBE_COPPER_COUNT) {
        return;
    }
    for (p = 0; p < PROBE_HW_ENEMY_COUNT; ++p) {
        g_probe_hw_enemy_sprite_cache_valid[cop_index][p] = 0;
        g_probe_hw_enemy_sprite_cache[cop_index][p] = -1;
    }
    g_probe_hw_enemy_active_mask[cop_index] = 0;
}

static void clear_probe_gameplay_hw_enemy_sprites(WORD cop_index, UWORD *cop) {
    WORD p;
    if (!cop) {
        return;
    }
    for (p = 0; p < PROBE_HW_ENEMY_COUNT; ++p) {
        clear_probe_hw_sprite_slot((UWORD *)probe_hw_enemy_slot(cop, p));
    }
    probe_invalidate_hw_enemy_sprite_cache(cop_index);
}

static void write_probe_hw_enemy_shape(UWORD *dst,
                                       const UBYTE *tiles,
                                       AmigaSpriteId sprite,
                                       WORD channel) {
    WORD row;
    WORD x;
    WORD plane;
    WORD group;
    if (!dst || !tiles) {
        return;
    }
    group = probe_hw_enemy_channel_group(channel);
    for (row = 0; row < PROBE_HW_SPRITE_HEIGHT; ++row) {
        UWORD plane_words[DEPTH];
        dst[2 + (row * 2)] = 0x0000;
        dst[3 + (row * 2)] = 0x0000;
        for (plane = 0; plane < DEPTH; ++plane) {
            const UBYTE *src = tiles +
                               ((ULONG)sprite * AMIGA_ASSET_TILE_BYTES) +
                               ((ULONG)row * DEPTH * AMIGA_ASSET_ROW_BYTES_TILE) +
                               ((ULONG)plane * AMIGA_ASSET_ROW_BYTES_TILE);
            plane_words[plane] = read_be_word_probe(src);
        }
        for (x = 0; x < GAME_TILE_SIZE; ++x) {
            UBYTE source_color = 0;
            UBYTE sprite_color;
            UWORD bit = (UWORD)(0x8000u >> x);
            for (plane = 0; plane < DEPTH; ++plane) {
                if ((plane_words[plane] & bit) != 0) {
                    source_color = (UBYTE)(source_color | (1u << plane));
                }
            }
            sprite_color = g_probe_hw_enemy_color_lut[group][source_color];
            if ((sprite_color & 2u) != 0) {
                dst[2 + (row * 2)] = (UWORD)(dst[2 + (row * 2)] | bit);
            }
            if ((sprite_color & 1u) != 0) {
                dst[3 + (row * 2)] = (UWORD)(dst[3 + (row * 2)] | bit);
            }
        }
    }
    dst[PROBE_HW_FULL_SPRITE_WORDS - 2] = 0x0000;
    dst[PROBE_HW_FULL_SPRITE_WORDS - 1] = 0x0000;
}

static BOOL update_probe_gameplay_hw_enemy_sprites(WORD cop_index,
                                                   UWORD *cop,
                                                   const GameState *game,
                                                   const UBYTE *tiles,
                                                   ULONG tick) {
    UBYTE used_channels = 0;
    UBYTE used_slots = 0;
    UBYTE stale_slots;
    UWORD mask;
    WORD i;
    BOOL any = FALSE;
    if (!cop || !game || !tiles || cop_index < 0 || cop_index >= PROBE_COPPER_COUNT) {
        return FALSE;
    }
    mask = (UWORD)game->alive_enemy_mask;
    for (i = 0; mask != 0u && i < game->enemy_count && i < GAME_MAX_ENEMIES; ++i, mask >>= 1) {
        const Enemy *enemy;
        WORD channel;
        WORD slot;
        UWORD *sprite;
        AmigaSpriteId sprite_id;
        if ((mask & 1u) == 0u) {
            continue;
        }
        enemy = &game->enemies[i];
        if (!enemy->alive ||
            !probe_enemy_visible(enemy, tick) ||
            !probe_enemy_type_uses_hw_sprite(enemy->type)) {
            continue;
        }
        channel = probe_select_hw_enemy_channel(enemy->type, used_channels);
        if (channel < 0) {
            continue;
        }
        used_channels = (UBYTE)(used_channels | (UBYTE)(1u << channel));
        slot = probe_hw_enemy_slot_for_channel(channel);
        used_slots = (UBYTE)(used_slots | (UBYTE)(1u << slot));
        sprite = (UWORD *)probe_hw_enemy_slot(cop, slot);
        sprite_id = probe_enemy_sprite(enemy);
        if (!g_probe_hw_enemy_sprite_cache_valid[cop_index][slot] ||
            g_probe_hw_enemy_sprite_cache[cop_index][slot] != (WORD)sprite_id) {
            write_probe_hw_enemy_shape(sprite, tiles, sprite_id, channel);
            g_probe_hw_enemy_sprite_cache[cop_index][slot] = (WORD)sprite_id;
            g_probe_hw_enemy_sprite_cache_valid[cop_index][slot] = 1;
        }
        write_probe_hw_sprite_control(sprite,
                                      probe_fp_to_screen(enemy->pixel_fp_x, SCREEN_W - GAME_TILE_SIZE),
                                      probe_fp_to_screen(enemy->pixel_fp_y, SCREEN_H - GAME_TILE_SIZE),
                                      PROBE_HW_SPRITE_HEIGHT);
        any = TRUE;
    }
    stale_slots = (UBYTE)(g_probe_hw_enemy_active_mask[cop_index] & (UBYTE)~used_slots);
    for (i = 0; stale_slots != 0u && i < PROBE_HW_ENEMY_COUNT; ++i, stale_slots >>= 1) {
        if ((stale_slots & 1u) != 0u) {
            UWORD *sprite = (UWORD *)probe_hw_enemy_slot(cop, i);
            sprite[0] = 0x0000;
            sprite[1] = 0x0000;
        }
    }
    g_probe_hw_enemy_active_mask[cop_index] = used_slots;
    return any;
}

static void patch_probe_hw_sprite_ptrs(WORD cop_index, UBYTE *sprite_bank, UWORD *cop, WORD mode) {
    WORD p;
    for (p = 0; p < PROBE_HW_SPRITE_COUNT; ++p) {
        UBYTE *ptr = probe_hw_null_sprite_ptr(sprite_bank);
        ULONG addr;
        if (mode == PROBE_HW_PTR_MODE_TITLE && p >= PROBE_HW_SPARKLE_FIRST_CHANNEL) {
            ptr = probe_hw_sparkle_slot(sprite_bank, (WORD)(p - PROBE_HW_SPARKLE_FIRST_CHANNEL));
        } else if (mode == PROBE_HW_PTR_MODE_GAMEPLAY &&
                   cop &&
                   p >= PROBE_HW_ENEMY_FIRST_CHANNEL) {
            ptr = probe_hw_enemy_slot(cop, probe_hw_enemy_slot_for_channel(p));
        }
        addr = (ULONG)ptr;
        *g_probe_copper_sprite_hi[cop_index][p] = (UWORD)(addr >> 16);
        *g_probe_copper_sprite_lo[cop_index][p] = (UWORD)(addr & 0xFFFF);
    }
}

static void update_probe_title_hw_sparkles(UBYTE *sprite_bank, const UBYTE *tiles, ULONG tick, BOOL active) {
    static const WORD sparkle_anchors[5][2] = {
        {88, 16},
        {170, 14},
        {36, 30},
        {124, 31},
        {198, 32}
    };
    WORD title_x;
    WORD title_y;
    WORD count;
    WORD i;

    title_x = (WORD)(96 + AMIGA_TITLE_OVERLAY_ANCHOR_X);
    title_y = (WORD)(10 + AMIGA_TITLE_OVERLAY_ANCHOR_Y);
    for (count = 0; count < PROBE_HW_SPARKLE_COUNT; ++count) {
        UWORD *sprite = (UWORD *)probe_hw_sparkle_slot(sprite_bank, count);
        sprite[0] = 0x0000;
        sprite[1] = 0x0000;
    }
    if (!active) {
        return;
    }
    count = 0;
    for (i = 0; i < 5 && count < PROBE_HW_SPARKLE_COUNT; ++i) {
        WORD wave = (WORD)(((tick / 2UL) + (ULONG)(i * 5)) & 15UL);
        if (wave <= 4) {
            WORD sx = (WORD)(title_x + ((sparkle_anchors[i][0] * AMIGA_TITLE_OVERLAY_WIDTH) / 220));
            WORD sy = (WORD)(title_y + ((sparkle_anchors[i][1] * AMIGA_TITLE_OVERLAY_HEIGHT) / 48));
            UWORD *sprite = (UWORD *)probe_hw_sparkle_slot(sprite_bank, count);
            write_probe_hw_sprite_control(sprite,
                                          (WORD)(sx - 8),
                                          (WORD)(sy - 8),
                                          PROBE_HW_SPRITE_HEIGHT);
            count = (WORD)(count + 1);
        }
    }
}

static UWORD probe_icy_reflection_color(UWORD color) {
    UWORD luma;
    UWORD b = (UWORD)(color & 15u);
    if (color == 0) {
        return 0x001;
    }
    luma = (UWORD)((((color >> 8) & 15u) + ((color >> 4) & 15u) + b) / 3u);
    return (UWORD)(((luma >> 3) << 8) | (((luma + b) >> 3) << 4) | ((luma + b + 4u) >> 2));
}

static void patch_probe_copper_sparkles(WORD cop_index, ULONG tick) {
    WORD i;
    for (i = 0; i < PROBE_COPPER_SPARKLE_COUNT; ++i) {
        ULONG phase = (tick >> 2) + (ULONG)(i * 11);
        ULONG wave = phase & 31UL;
        UWORD y;
        UWORD line;
        UWORD restore_line;
        if (wave > 16UL) {
            wave = 32UL - wave;
        }
        y = (UWORD)(PROBE_COPPER_SPARKLE_TOP_Y + (i * PROBE_COPPER_SPARKLE_STEP_Y) + wave);
        line = (UWORD)(0x2C + y);
        restore_line = (UWORD)(line + PROBE_COPPER_SPARKLE_RESTORE_GAP);
        *g_probe_copper_sparkle_wait[cop_index][i] = (UWORD)(((line & 0xFFu) << 8) | 0x07);
        *g_probe_copper_sparkle_restore_wait[cop_index][i] = (UWORD)(((restore_line & 0xFFu) << 8) | 0x07);
        *g_probe_copper_sparkle_color[cop_index][i][0] = (UWORD)((((tick >> 3) + (ULONG)i) & 1UL) ? 0x8DF : 0x9EF);
        *g_probe_copper_sparkle_color[cop_index][i][1] = (UWORD)((((tick >> 2) + (ULONG)i) & 1UL) ? 0xAFF : 0xCFF);
        *g_probe_copper_sparkle_color[cop_index][i][2] = (UWORD)((((tick >> 3) + (ULONG)i) & 1UL) ? 0xEFF : 0xFFF);
        *g_probe_copper_sparkle_color[cop_index][i][3] = (UWORD)((((tick >> 2) + (ULONG)i) & 1UL) ? 0xDFF : 0xFFF);
    }
}

static void patch_probe_copper_reflection(WORD cop_index, ULONG tick) {
    UWORD *line_words;
    ULONG phase;
    UWORD scroll;
    UBYTE repeat;
    WORD line;
    phase = tick >> 2;
    scroll = (UWORD)(phase & 31UL);
    if (scroll > 16u) {
        scroll = (UWORD)(32u - scroll);
    }
    scroll = (UWORD)(scroll >> 2);
    *g_probe_copper_reflection_first[cop_index] = (UWORD)(scroll | (scroll << 4));
    line_words = g_probe_copper_reflection_lines[cop_index];
    repeat = 0;
    for (line = 1; line < PROBE_PAL_REFLECTION_LINES; ++line) {
        repeat = (UBYTE)(repeat + 1u);
        if (repeat == 3u) {
            repeat = 0;
            phase = phase + 1UL;
        }
        scroll = (UWORD)(phase & 31UL);
        if (scroll > 16u) {
            scroll = (UWORD)(32u - scroll);
        }
        scroll = (UWORD)(scroll >> 2);
        line_words[3] = (UWORD)(scroll | (scroll << 4));
        line_words += 4;
        if ((PROBE_PAL_REFLECTION_START_LINE + line + 1) == 0x100) {
            line_words += 2;
        }
    }
}

static void patch_probe_copper_reflection_enabled(WORD cop_index, BOOL enabled) {
    if (cop_index < 0 || cop_index >= PROBE_COPPER_COUNT ||
        !g_probe_copper_reflection_bplcon0[cop_index]) {
        return;
    }
    *g_probe_copper_reflection_bplcon0[cop_index] = enabled ? 0x5200 : 0x0000;
}

static BOOL build_probe_copper(WORD cop_index, UWORD *cop, UBYTE *screen, UBYTE *sprite_bank) {
    ULONG addr;
    ULONG clear_i;
    WORD i = 0;
    WORD p;
    if (PROBE_COPPER_USED_WORDS > PROBE_COPPER_WORDS ||
        (PROBE_COPPER_USED_WORDS + PROBE_HW_ENEMY_SPRITE_TAIL_WORDS) > PROBE_COPPER_WORDS) {
        return FALSE;
    }
    for (clear_i = 0; clear_i < PROBE_COPPER_WORDS; clear_i = clear_i + 2UL) {
        cop[clear_i] = 0xFFFF;
        cop[clear_i + 1UL] = 0xFFFE;
    }
    cop[i++] = DIWSTRT; cop[i++] = 0x2C81;
    cop[i++] = DIWSTOP; cop[i++] = PROBE_PAL_FULL_DIWSTOP;
    cop[i++] = DDFSTRT; cop[i++] = 0x0038;
    cop[i++] = DDFSTOP; cop[i++] = 0x00D0;
    for (p = 0; p < DEPTH; ++p) {
        addr = (ULONG)screen + ((ULONG)p * ROW_BYTES);
        cop[i++] = (UWORD)(BPL1PTH + (p * 4)); cop[i++] = (UWORD)(addr >> 16);
        cop[i++] = (UWORD)(BPL1PTL + (p * 4)); cop[i++] = (UWORD)(addr & 0xFFFF);
    }
    cop[i++] = BPLCON0; cop[i++] = 0x5200;
    cop[i++] = BPLCON1; cop[i++] = 0x0000;
    cop[i++] = BPLCON2; cop[i++] = BPLCON2_SPRITES_FRONT;
    cop[i++] = BPL1MOD; cop[i++] = ROW_STRIDE_BYTES - ROW_BYTES;
    cop[i++] = BPL2MOD; cop[i++] = ROW_STRIDE_BYTES - ROW_BYTES;
    for (p = 0; p < 8; ++p) {
        addr = (ULONG)probe_hw_sprite_channel_ptr(sprite_bank, p);
        cop[i++] = (UWORD)(SPR0PTH + (p * 4));
        g_probe_copper_sprite_hi[cop_index][p] = &cop[i];
        cop[i++] = (UWORD)(addr >> 16);
        cop[i++] = (UWORD)(SPR0PTL + (p * 4));
        g_probe_copper_sprite_lo[cop_index][p] = &cop[i];
        cop[i++] = (UWORD)(addr & 0xFFFF);
    }
    cop[i++] = COLOR00; cop[i++] = g_amiga_palette[0];
    for (p = 1; p < 32; ++p) {
        cop[i++] = (UWORD)(COLOR00 + (p * 2));
        cop[i++] = g_amiga_palette[p];
    }
    for (p = 0; p < PROBE_COPPER_SPARKLE_COUNT; ++p) {
        WORD c;
        UWORD y = (UWORD)(PROBE_COPPER_SPARKLE_TOP_Y + (p * PROBE_COPPER_SPARKLE_STEP_Y));
        UWORD line = (UWORD)(0x2C + y);
        cop[i++] = (UWORD)(((line & 0xFFu) << 8) | 0x07);
        g_probe_copper_sparkle_wait[cop_index][p] = &cop[i - 1];
        cop[i++] = 0xFFFE;
        for (c = 0; c < 4; ++c) {
            cop[i++] = (UWORD)(COLOR00 + ((6 + c) * 2));
            g_probe_copper_sparkle_color[cop_index][p][c] = &cop[i];
            cop[i++] = g_amiga_palette[6 + c];
        }
        line = (UWORD)(line + PROBE_COPPER_SPARKLE_RESTORE_GAP);
        cop[i++] = (UWORD)(((line & 0xFFu) << 8) | 0x07);
        g_probe_copper_sparkle_restore_wait[cop_index][p] = &cop[i - 1];
        cop[i++] = 0xFFFE;
        for (c = 0; c < 4; ++c) {
            cop[i++] = (UWORD)(COLOR00 + ((6 + c) * 2));
            cop[i++] = g_amiga_palette[6 + c];
        }
    }
    cop[i++] = (UWORD)((PROBE_PAL_REFLECTION_SETUP_LINE << 8) | 0x07);
    cop[i++] = 0xFFFE;
    cop[i++] = BPLCON0; cop[i++] = 0x0000;
    for (p = 0; p < 32; ++p) {
        cop[i++] = (UWORD)(COLOR00 + (p * 2));
        cop[i++] = probe_icy_reflection_color(g_amiga_palette[p]);
    }
    cop[i++] = BPLCON1;
    g_probe_copper_reflection_first[cop_index] = &cop[i];
    cop[i++] = 0x0000;
    cop[i++] = BPL1MOD; cop[i++] = PROBE_PAL_REFLECTION_BPLMOD;
    cop[i++] = BPL2MOD; cop[i++] = PROBE_PAL_REFLECTION_BPLMOD;
    for (p = 0; p < DEPTH; ++p) {
        addr = (ULONG)screen + ((ULONG)(SCREEN_H - 1) * ROW_STRIDE_BYTES) + ((ULONG)p * ROW_BYTES);
        cop[i++] = (UWORD)(BPL1PTH + (p * 4)); cop[i++] = (UWORD)(addr >> 16);
        cop[i++] = (UWORD)(BPL1PTL + (p * 4)); cop[i++] = (UWORD)(addr & 0xFFFF);
    }
    cop[i++] = (UWORD)((PROBE_PAL_REFLECTION_START_LINE << 8) | 0x07);
    cop[i++] = 0xFFFE;
    cop[i++] = BPLCON0;
    g_probe_copper_reflection_bplcon0[cop_index] = &cop[i];
    cop[i++] = 0x5200;
    g_probe_copper_reflection_lines[cop_index] = &cop[i];
    for (p = 1; p < PROBE_PAL_REFLECTION_LINES; ++p) {
        UWORD line = (UWORD)(PROBE_PAL_REFLECTION_START_LINE + p);
        if (line == 0x100u) {
            cop[i++] = 0xFFDF;
            cop[i++] = 0xFFFE;
        }
        cop[i++] = (UWORD)(((line & 0xFFu) << 8) | 0x07);
        cop[i++] = 0xFFFE;
        cop[i++] = BPLCON1;
        cop[i++] = 0x0000;
    }
    cop[i++] = 0xFFFF; cop[i++] = 0xFFFE;
    return ((ULONG)i == PROBE_COPPER_USED_WORDS) ? TRUE : FALSE;
}

static void draw_hline_probe(UBYTE *screen, WORD x0, WORD x1, WORD y, UBYTE color) {
    WORD plane;
    WORD start_byte;
    WORD end_byte;
    UBYTE left_mask;
    UBYTE right_mask;
    UBYTE mask;
    if (!screen || y < 0 || y >= SCREEN_H) {
        return;
    }
    if (x0 < 0) x0 = 0;
    if (x1 >= SCREEN_W) x1 = SCREEN_W - 1;
    if (x1 < x0) {
        return;
    }
    start_byte = (WORD)(x0 >> 3);
    end_byte = (WORD)(x1 >> 3);
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

static void draw_rect_probe(UBYTE *screen, WORD x0, WORD y0, WORD x1, WORD y1, UBYTE color) {
    WORD y;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 >= SCREEN_W) x1 = SCREEN_W - 1;
    if (y1 >= SCREEN_H) y1 = SCREEN_H - 1;
    if (x1 < x0 || y1 < y0) {
        return;
    }
    for (y = y0; y <= y1; ++y) {
        draw_hline_probe(screen, x0, x1, y, color);
        if ((y & 7) == 0 &&
            g_probe_blitter_input_latch &&
            g_probe_blitter_last_dirs &&
            g_probe_blitter_last_fire) {
            probe_poll_input_latch(g_probe_blitter_input_latch,
                                   g_probe_blitter_last_dirs,
                                   g_probe_blitter_last_fire);
        }
    }
}

static void fill_rect_probe(UBYTE *screen, WORD x, WORD y, WORD w, WORD h, UBYTE color) {
    if (w <= 0 || h <= 0) {
        return;
    }
    draw_rect_probe(screen, x, y, (WORD)(x + w - 1), (WORD)(y + h - 1), color);
}

static const UBYTE kProbeDigit3x5[10][5] = {
    {7, 5, 5, 5, 7},
    {2, 6, 2, 2, 7},
    {7, 1, 7, 4, 7},
    {7, 1, 7, 1, 7},
    {5, 5, 7, 1, 1},
    {7, 4, 7, 1, 7},
    {7, 4, 7, 5, 7},
    {7, 1, 1, 1, 1},
    {7, 5, 7, 5, 7},
    {7, 5, 7, 1, 7}
};

static void put_pixel_probe(UBYTE *screen, WORD x, WORD y, UBYTE color) {
    UBYTE mask;
    UBYTE inv_mask;
    UBYTE *p;
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) {
        return;
    }
    mask = (UBYTE)(0x80u >> (x & 7));
    inv_mask = (UBYTE)~mask;
    p = screen + ((ULONG)y * ROW_STRIDE_BYTES) + (ULONG)(x >> 3);
    if (color & 1u) p[0] |= mask; else p[0] &= inv_mask;
    if (color & 2u) p[ROW_BYTES] |= mask; else p[ROW_BYTES] &= inv_mask;
    if (color & 4u) p[ROW_BYTES * 2] |= mask; else p[ROW_BYTES * 2] &= inv_mask;
    if (color & 8u) p[ROW_BYTES * 3] |= mask; else p[ROW_BYTES * 3] &= inv_mask;
    if (color & 16u) p[ROW_BYTES * 4] |= mask; else p[ROW_BYTES * 4] &= inv_mask;
}

static void draw_probe_bits_row(UBYTE *screen, WORD x, WORD y, UBYTE bits, WORD width, UBYTE color) {
    WORD col = 0;
    if (bits == 0 || width <= 0) {
        return;
    }
    while (col < width) {
        WORD start;
        while (col < width && ((bits & (UBYTE)(1u << (width - 1 - col))) == 0)) {
            ++col;
        }
        if (col >= width) {
            break;
        }
        start = col;
        while (col < width && ((bits & (UBYTE)(1u << (width - 1 - col))) != 0)) {
            ++col;
        }
        if (col == start + 1) {
            put_pixel_probe(screen, (WORD)(x + start), y, color);
        } else {
            draw_hline_probe(screen, (WORD)(x + start), (WORD)(x + col - 1), y, color);
        }
    }
}

static void draw_probe_scaled_bits_row(UBYTE *screen, WORD x, WORD y, UBYTE bits, WORD width, UBYTE color, WORD scale) {
    WORD col = 0;
    if (bits == 0 || width <= 0 || scale <= 0) {
        return;
    }
    if (scale == 1) {
        draw_probe_bits_row(screen, x, y, bits, width, color);
        return;
    }
    while (col < width) {
        WORD start;
        while (col < width && ((bits & (UBYTE)(1u << (width - 1 - col))) == 0)) {
            ++col;
        }
        if (col >= width) {
            break;
        }
        start = col;
        while (col < width && ((bits & (UBYTE)(1u << (width - 1 - col))) != 0)) {
            ++col;
        }
        fill_rect_probe(screen,
                        (WORD)(x + start * scale),
                        y,
                        (WORD)((col - start) * scale),
                        scale,
                        color);
    }
}

static void draw_probe_3bit_row(UBYTE *screen, WORD x, WORD y, UBYTE bits, UBYTE color) {
    draw_probe_bits_row(screen, x, y, bits, 3, color);
}

static void draw_probe_5bit_row(UBYTE *screen, WORD x, WORD y, UBYTE bits, UBYTE color) {
    draw_probe_bits_row(screen, x, y, bits, 5, color);
}

static void probe_font_set_5bit(UBYTE *row, WORD x) {
    if (x >= 0 && x < 5) {
        *row = (UBYTE)(*row | (UBYTE)(0x10u >> x));
    }
}

static BOOL probe_glyph_rows(char ch, UBYTE out[5]) {
    if (ch >= 'a' && ch <= 'z') {
        ch = (char)(ch - 'a' + 'A');
    }
    if (ch >= '0' && ch <= '9') {
        WORD i;
        for (i = 0; i < 5; ++i) {
            out[i] = kProbeDigit3x5[ch - '0'][i];
        }
        return TRUE;
    }
    switch (ch) {
        case ' ': out[0] = 0; out[1] = 0; out[2] = 0; out[3] = 0; out[4] = 0; return TRUE;
        case ':': out[0] = 0; out[1] = 2; out[2] = 0; out[3] = 2; out[4] = 0; return TRUE;
        case '-': out[0] = 0; out[1] = 0; out[2] = 7; out[3] = 0; out[4] = 0; return TRUE;
        case '+': out[0] = 0; out[1] = 2; out[2] = 7; out[3] = 2; out[4] = 0; return TRUE;
        case '/': out[0] = 1; out[1] = 1; out[2] = 2; out[3] = 4; out[4] = 4; return TRUE;
        case '.': out[0] = 0; out[1] = 0; out[2] = 0; out[3] = 0; out[4] = 2; return TRUE;
        case '<': out[0] = 1; out[1] = 2; out[2] = 4; out[3] = 2; out[4] = 1; return TRUE;
        case '>': out[0] = 4; out[1] = 2; out[2] = 1; out[3] = 2; out[4] = 4; return TRUE;
        case 'A': out[0] = 7; out[1] = 5; out[2] = 7; out[3] = 5; out[4] = 5; return TRUE;
        case 'B': out[0] = 6; out[1] = 5; out[2] = 6; out[3] = 5; out[4] = 6; return TRUE;
        case 'C': out[0] = 7; out[1] = 4; out[2] = 4; out[3] = 4; out[4] = 7; return TRUE;
        case 'D': out[0] = 6; out[1] = 5; out[2] = 5; out[3] = 5; out[4] = 6; return TRUE;
        case 'E': out[0] = 7; out[1] = 4; out[2] = 6; out[3] = 4; out[4] = 7; return TRUE;
        case 'F': out[0] = 7; out[1] = 4; out[2] = 6; out[3] = 4; out[4] = 4; return TRUE;
        case 'G': out[0] = 7; out[1] = 4; out[2] = 5; out[3] = 5; out[4] = 7; return TRUE;
        case 'H': out[0] = 5; out[1] = 5; out[2] = 7; out[3] = 5; out[4] = 5; return TRUE;
        case 'I': out[0] = 7; out[1] = 2; out[2] = 2; out[3] = 2; out[4] = 7; return TRUE;
        case 'J': out[0] = 1; out[1] = 1; out[2] = 1; out[3] = 5; out[4] = 7; return TRUE;
        case 'K': out[0] = 5; out[1] = 5; out[2] = 6; out[3] = 5; out[4] = 5; return TRUE;
        case 'L': out[0] = 4; out[1] = 4; out[2] = 4; out[3] = 4; out[4] = 7; return TRUE;
        case 'M': out[0] = 5; out[1] = 7; out[2] = 5; out[3] = 5; out[4] = 5; return TRUE;
        case 'N': out[0] = 5; out[1] = 7; out[2] = 7; out[3] = 7; out[4] = 5; return TRUE;
        case 'O': out[0] = 7; out[1] = 5; out[2] = 5; out[3] = 5; out[4] = 7; return TRUE;
        case 'P': out[0] = 6; out[1] = 5; out[2] = 6; out[3] = 4; out[4] = 4; return TRUE;
        case 'Q': out[0] = 7; out[1] = 5; out[2] = 5; out[3] = 7; out[4] = 1; return TRUE;
        case 'R': out[0] = 6; out[1] = 5; out[2] = 6; out[3] = 5; out[4] = 5; return TRUE;
        case 'S': out[0] = 7; out[1] = 4; out[2] = 7; out[3] = 1; out[4] = 7; return TRUE;
        case 'T': out[0] = 7; out[1] = 2; out[2] = 2; out[3] = 2; out[4] = 2; return TRUE;
        case 'U': out[0] = 5; out[1] = 5; out[2] = 5; out[3] = 5; out[4] = 7; return TRUE;
        case 'V': out[0] = 5; out[1] = 5; out[2] = 5; out[3] = 5; out[4] = 2; return TRUE;
        case 'W': out[0] = 5; out[1] = 5; out[2] = 5; out[3] = 7; out[4] = 5; return TRUE;
        case 'X': out[0] = 5; out[1] = 5; out[2] = 2; out[3] = 5; out[4] = 5; return TRUE;
        case 'Y': out[0] = 5; out[1] = 5; out[2] = 2; out[3] = 2; out[4] = 2; return TRUE;
        case 'Z': out[0] = 7; out[1] = 1; out[2] = 2; out[3] = 4; out[4] = 7; return TRUE;
        default: return FALSE;
    }
}

static void probe_font_precompute(void) {
    WORD ch;
    if (g_probe_font_ready) {
        return;
    }
    for (ch = 0; ch < 128; ++ch) {
        UBYTE rows[5];
        WORD row;
        WORD col;
        if (!probe_glyph_rows((char)ch, rows)) {
            continue;
        }
        for (row = 0; row < 5; ++row) {
            g_probe_font_fill_rows[ch][row] = rows[row];
        }
        for (row = 0; row < 5; ++row) {
            UBYTE bits = rows[row];
            for (col = 0; col < 3; ++col) {
                if (((bits >> (2 - col)) & 1u) == 0) {
                    continue;
                }
                probe_font_set_5bit(&g_probe_font_outline_rows[ch][row + 1], col);
                probe_font_set_5bit(&g_probe_font_outline_rows[ch][row + 1], (WORD)(col + 2));
                probe_font_set_5bit(&g_probe_font_outline_rows[ch][row], (WORD)(col + 1));
                probe_font_set_5bit(&g_probe_font_outline_rows[ch][row + 2], (WORD)(col + 1));
            }
        }
        for (row = 0; row < 5; ++row) {
            g_probe_font_outline_rows[ch][row + 1] =
                (UBYTE)(g_probe_font_outline_rows[ch][row + 1] & (UBYTE)~(rows[row] << 1));
        }
    }
    g_probe_font_ready = 1;
}

static void draw_probe_char(UBYTE *screen, WORD x, WORD y, char ch, UBYTE color) {
    unsigned char idx = (unsigned char)ch;
    WORD row;
    if (idx >= 128u) {
        return;
    }
    probe_font_precompute();
    for (row = 0; row < 5; ++row) {
        draw_probe_3bit_row(screen, x, (WORD)(y + row), g_probe_font_fill_rows[idx][row], color);
    }
}

static void draw_probe_text(UBYTE *screen, WORD x, WORD y, const char *text, UBYTE color) {
    while (*text) {
        draw_probe_char(screen, x, y, *text, color);
        x = (WORD)(x + 4);
        ++text;
    }
}

static WORD probe_text_width(const char *text) {
    WORD len = 0;
    while (text[len] != '\0') {
        ++len;
    }
    return (len > 0) ? (WORD)((len * 4) - 1) : 0;
}

static void draw_probe_centered_text(UBYTE *screen, WORD x0, WORD x1, WORD y, const char *text, UBYTE color) {
    WORD w = probe_text_width(text);
    WORD x = (WORD)(x0 + (((x1 - x0 + 1) - w) / 2));
    draw_probe_text(screen, x, y, text, color);
}

static void draw_probe_char_outlined(UBYTE *screen, WORD x, WORD y, char ch, UBYTE color, UBYTE outline) {
    unsigned char idx = (unsigned char)ch;
    WORD row;
    if (idx >= 128u || ch == ' ') {
        return;
    }
    probe_font_precompute();
    for (row = 0; row < 7; ++row) {
        draw_probe_5bit_row(screen, (WORD)(x - 1), (WORD)(y - 1 + row), g_probe_font_outline_rows[idx][row], outline);
    }
    for (row = 0; row < 5; ++row) {
        draw_probe_3bit_row(screen, x, (WORD)(y + row), g_probe_font_fill_rows[idx][row], color);
    }
}

static void draw_probe_text_outlined(UBYTE *screen, WORD x, WORD y, const char *text, UBYTE color, UBYTE outline) {
    while (*text) {
        draw_probe_char_outlined(screen, x, y, *text, color, outline);
        x = (WORD)(x + 4);
        ++text;
    }
}

static void draw_probe_centered_text_outlined(UBYTE *screen,
                                              WORD x0,
                                              WORD x1,
                                              WORD y,
                                              const char *text,
                                              UBYTE color,
                                              UBYTE outline) {
    WORD w = probe_text_width(text);
    WORD x = (WORD)(x0 + (((x1 - x0 + 1) - w) / 2));
    draw_probe_text_outlined(screen, x, y, text, color, outline);
}

static void draw_probe_number(UBYTE *screen, WORD x, WORD y, ULONG value, WORD min_digits, UBYTE color) {
    char digits[10];
    WORD count = 0;
    WORD i;
    do {
        digits[count++] = (char)('0' + (value % 10UL));
        value /= 10UL;
    } while (value > 0 && count < 10);
    while (count < min_digits && count < 10) {
        digits[count++] = '0';
    }
    for (i = (WORD)(count - 1); i >= 0; --i) {
        draw_probe_char(screen, x, y, digits[i], color);
        x = (WORD)(x + 4);
    }
}

static WORD probe_number_width(ULONG value, WORD min_digits) {
    WORD count = 0;
    do {
        ++count;
        value /= 10UL;
    } while (value > 0 && count < 10);
    while (count < min_digits && count < 10) {
        ++count;
    }
    return (count > 0) ? (WORD)((count * 4) - 1) : 0;
}

static void draw_probe_number_right(UBYTE *screen, WORD right_x, WORD y, ULONG value, WORD min_digits, UBYTE color) {
    draw_probe_number(screen, (WORD)(right_x - probe_number_width(value, min_digits) + 1), y, value, min_digits, color);
}

static void draw_probe_char_scaled(UBYTE *screen, WORD x, WORD y, char ch, UBYTE color, WORD scale) {
    UBYTE rows[5];
    WORD row;
    scale = (WORD)clamp_int_probe(scale, 1, 4);
    if (!probe_glyph_rows(ch, rows)) {
        return;
    }
    for (row = 0; row < 5; ++row) {
        draw_probe_scaled_bits_row(screen, x, (WORD)(y + row * scale), rows[row], 3, color, scale);
    }
}

static void draw_probe_text_scaled(UBYTE *screen, WORD x, WORD y, const char *text, UBYTE color, WORD scale) {
    scale = (WORD)clamp_int_probe(scale, 1, 4);
    while (*text) {
        draw_probe_char_scaled(screen, x, y, *text, color, scale);
        x = (WORD)(x + 4 * scale);
        ++text;
    }
}

static WORD probe_text_width_scaled(const char *text, WORD scale) {
    WORD len = 0;
    scale = (WORD)clamp_int_probe(scale, 1, 4);
    while (text[len] != '\0') {
        ++len;
    }
    return (len > 0) ? (WORD)((len * 4 * scale) - scale) : 0;
}

static void draw_probe_text_scaled_outlined(UBYTE *screen,
                                            WORD x,
                                            WORD y,
                                            const char *text,
                                            UBYTE color,
                                            UBYTE outline,
                                            WORD scale) {
    draw_probe_text_scaled(screen, (WORD)(x - 1), y, text, outline, scale);
    draw_probe_text_scaled(screen, (WORD)(x + 1), y, text, outline, scale);
    draw_probe_text_scaled(screen, x, (WORD)(y - 1), text, outline, scale);
    draw_probe_text_scaled(screen, x, (WORD)(y + 1), text, outline, scale);
    draw_probe_text_scaled(screen, x, y, text, color, scale);
}

static void draw_probe_centered_text_scaled_outlined(UBYTE *screen,
                                                     WORD x0,
                                                     WORD x1,
                                                     WORD y,
                                                     const char *text,
                                                     UBYTE color,
                                                     UBYTE outline,
                                                     WORD scale) {
    WORD w = probe_text_width_scaled(text, scale);
    WORD x = (WORD)(x0 + (((x1 - x0 + 1) - w) / 2));
    draw_probe_text_scaled_outlined(screen, x, y, text, color, outline, scale);
}

static WORD probe_number_width_scaled(ULONG value, WORD min_digits, WORD scale) {
    WORD count = 0;
    scale = (WORD)clamp_int_probe(scale, 1, 4);
    do {
        ++count;
        value /= 10UL;
    } while (value > 0 && count < 10);
    while (count < min_digits && count < 10) {
        ++count;
    }
    return (count > 0) ? (WORD)((count * 4 * scale) - scale) : 0;
}

static void draw_probe_number_scaled_outlined(UBYTE *screen,
                                              WORD x,
                                              WORD y,
                                              ULONG value,
                                              WORD min_digits,
                                              UBYTE color,
                                              UBYTE outline,
                                              WORD scale) {
    char digits[10];
    WORD count = 0;
    WORD i;
    do {
        digits[count++] = (char)('0' + (value % 10UL));
        value /= 10UL;
    } while (value > 0 && count < 10);
    while (count < min_digits && count < 10) {
        digits[count++] = '0';
    }
    for (i = (WORD)(count - 1); i >= 0; --i) {
        draw_probe_char_scaled(screen, (WORD)(x - 1), y, digits[i], outline, scale);
        draw_probe_char_scaled(screen, (WORD)(x + 1), y, digits[i], outline, scale);
        draw_probe_char_scaled(screen, x, (WORD)(y - 1), digits[i], outline, scale);
        draw_probe_char_scaled(screen, x, (WORD)(y + 1), digits[i], outline, scale);
        draw_probe_char_scaled(screen, x, y, digits[i], color, scale);
        x = (WORD)(x + 4 * scale);
    }
}

static void draw_probe_number_outlined(UBYTE *screen, WORD x, WORD y, ULONG value, WORD min_digits, UBYTE color, UBYTE outline) {
    char digits[10];
    WORD count = 0;
    WORD i;
    do {
        digits[count++] = (char)('0' + (value % 10UL));
        value /= 10UL;
    } while (value > 0 && count < 10);
    while (count < min_digits && count < 10) {
        digits[count++] = '0';
    }
    for (i = (WORD)(count - 1); i >= 0; --i) {
        draw_probe_char_outlined(screen, x, y, digits[i], color, outline);
        x = (WORD)(x + 4);
    }
}

static UBYTE probe_ui_line_color(UBYTE color) {
    switch (color) {
        case 11:
        case 14:
        case 15:
            return 21;
        case 16:
        case 27:
            return 25;
        default:
            return color;
    }
}

static void draw_probe_panel(UBYTE *screen, WORD x0, WORD y0, WORD x1, WORD y1, UBYTE accent) {
    accent = probe_ui_line_color(accent);
    draw_rect_probe(screen, x0, y0, x1, y1, 1);
    draw_rect_probe(screen, x0, y0, x1, y0, 30);
    draw_rect_probe(screen, x0, y1, x1, y1, 30);
    draw_rect_probe(screen, x0, y0, x0, y1, 30);
    draw_rect_probe(screen, x1, y0, x1, y1, 30);
    draw_rect_probe(screen, (WORD)(x0 + 2), (WORD)(y0 + 2), (WORD)(x1 - 2), (WORD)(y0 + 2), accent);
    draw_rect_probe(screen, (WORD)(x0 + 2), (WORD)(y1 - 2), (WORD)(x1 - 2), (WORD)(y1 - 2), accent);
}

static void draw_probe_card_frame(UBYTE *screen, WORD x0, WORD y0, WORD w, WORD h, UBYTE accent, BOOL selected) {
    WORD x1 = (WORD)(x0 + w - 1);
    WORD y1 = (WORD)(y0 + h - 1);
    UBYTE outer;
    UBYTE inner;
    accent = probe_ui_line_color(accent);
    outer = selected ? 30 : 3;
    inner = selected ? accent : 2;
    draw_rect_probe(screen, x0, y0, x1, y1, 1);
    draw_rect_probe(screen, x0, y0, x1, y0, outer);
    draw_rect_probe(screen, x0, y1, x1, y1, outer);
    draw_rect_probe(screen, x0, y0, x0, y1, outer);
    draw_rect_probe(screen, x1, y0, x1, y1, outer);
    draw_rect_probe(screen, (WORD)(x0 + 1), (WORD)(y0 + 1), (WORD)(x1 - 1), (WORD)(y0 + 1), inner);
    draw_rect_probe(screen, (WORD)(x0 + 1), (WORD)(y1 - 1), (WORD)(x1 - 1), (WORD)(y1 - 1), inner);
    draw_rect_probe(screen, (WORD)(x0 + 1), (WORD)(y0 + 1), (WORD)(x0 + 1), (WORD)(y1 - 1), inner);
    draw_rect_probe(screen, (WORD)(x1 - 1), (WORD)(y0 + 1), (WORD)(x1 - 1), (WORD)(y1 - 1), inner);
}

static void draw_probe_card_selection_chrome(UBYTE *screen, WORD x0, WORD y0, WORD w, WORD h, UBYTE accent, BOOL selected) {
    WORD x1 = (WORD)(x0 + w - 1);
    WORD y1 = (WORD)(y0 + h - 1);
    UBYTE outer;
    UBYTE inner;
    accent = probe_ui_line_color(accent);
    outer = selected ? 30 : 3;
    inner = selected ? accent : 2;
    draw_hline_probe(screen, x0, x1, y0, outer);
    draw_hline_probe(screen, x0, x1, y1, outer);
    draw_rect_probe(screen, x0, y0, x0, y1, outer);
    draw_rect_probe(screen, x1, y0, x1, y1, outer);
    draw_rect_probe(screen, (WORD)(x0 + 1), (WORD)(y0 + 1), (WORD)(x0 + 1), (WORD)(y1 - 1), inner);
    draw_rect_probe(screen, (WORD)(x1 - 1), (WORD)(y0 + 1), (WORD)(x1 - 1), (WORD)(y1 - 1), inner);
    if (y0 >= 3) {
        WORD cx = (WORD)(x0 + (w / 2));
        draw_rect_probe(screen, (WORD)(cx - 2), (WORD)(y0 - 3), (WORD)(cx + 2), (WORD)(y0 - 1), 1);
        if (selected) {
            put_pixel_probe(screen, cx, (WORD)(y0 - 3), 30);
            put_pixel_probe(screen, (WORD)(cx - 1), (WORD)(y0 - 2), accent);
            put_pixel_probe(screen, cx, (WORD)(y0 - 2), accent);
            put_pixel_probe(screen, (WORD)(cx + 1), (WORD)(y0 - 2), accent);
            put_pixel_probe(screen, cx, (WORD)(y0 - 1), 30);
        }
    }
}

static void draw_probe_visual_arrow_glyph(UBYTE *screen, WORD x, WORD y, WORD dir, UBYTE color) {
    switch (dir) {
        case 0:
            put_pixel_probe(screen, x, (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 1), (WORD)(y + 1), color);
            put_pixel_probe(screen, (WORD)(x + 1), (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 1), (WORD)(y + 3), color);
            put_pixel_probe(screen, (WORD)(x + 2), (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 3), (WORD)(y + 2), color);
            break;
        case 1:
            put_pixel_probe(screen, x, (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 1), (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 2), (WORD)(y + 1), color);
            put_pixel_probe(screen, (WORD)(x + 2), (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 2), (WORD)(y + 3), color);
            put_pixel_probe(screen, (WORD)(x + 3), (WORD)(y + 2), color);
            break;
        case 2:
            put_pixel_probe(screen, (WORD)(x + 2), y, color);
            put_pixel_probe(screen, (WORD)(x + 1), (WORD)(y + 1), color);
            put_pixel_probe(screen, (WORD)(x + 2), (WORD)(y + 1), color);
            put_pixel_probe(screen, (WORD)(x + 3), (WORD)(y + 1), color);
            put_pixel_probe(screen, x, (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 1), (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 2), (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 3), (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 4), (WORD)(y + 2), color);
            break;
        default:
            put_pixel_probe(screen, x, (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 1), (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 2), (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 3), (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 4), (WORD)(y + 2), color);
            put_pixel_probe(screen, (WORD)(x + 1), (WORD)(y + 3), color);
            put_pixel_probe(screen, (WORD)(x + 2), (WORD)(y + 3), color);
            put_pixel_probe(screen, (WORD)(x + 3), (WORD)(y + 3), color);
            put_pixel_probe(screen, (WORD)(x + 2), (WORD)(y + 4), color);
            break;
    }
}

static void draw_probe_continue_chevrons(UBYTE *screen, WORD x, WORD y, UBYTE color) {
    draw_probe_visual_arrow_glyph(screen, x, y, 1, color);
    draw_probe_visual_arrow_glyph(screen, (WORD)(x + 5), y, 1, color);
    draw_probe_visual_arrow_glyph(screen, (WORD)(x + 10), y, 1, color);
}

static const char *probe_perk_label(PerkType perk) {
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
            return "BOMBS";
        default:
            return "MYST";
    }
}

static const char *probe_perk_effect_label(PerkType perk) {
    switch (perk) {
        case PERK_LIFE_BOOST:
            return "+1 LIFE";
        case PERK_TIME_BOOST:
            return "+TIME";
        case PERK_SCORE_BOOST:
            return "+SCORE";
        case PERK_ENEMY_SLOW:
            return "SLOW ENEMY";
        case PERK_MINES:
            return "FIRE BOMB";
        default:
            return "NO EFFECT";
    }
}

static UBYTE probe_perk_color(PerkType perk) {
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

static AmigaSpriteId probe_perk_sprite(PerkType perk) {
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

static WORD probe_perk_choice_count_clamped(const GameState *game) {
    WORD count = game ? (WORD)game->perk_choice_count : 1;
    if (count < 1) {
        count = 1;
    }
    if (count > GAME_PERK_CHOICE_COUNT) {
        count = GAME_PERK_CHOICE_COUNT;
    }
    return count;
}

static WORD probe_perk_choice_base_x(WORD count) {
    const WORD panel_x0 = 48;
    const WORD panel_x1 = 271;
    const WORD card_w = 58;
    const WORD spacing = 8;
    WORD total_w = (WORD)(count * card_w + (count - 1) * spacing);
    return (WORD)(panel_x0 + ((panel_x1 - panel_x0 + 1 - total_w) / 2));
}

static void draw_probe_opening_high_scores(UBYTE *screen, const ProbeHighScoreTable *table) {
    WORD i;
    const WORD x0 = 86;
    const WORD x1 = 233;
    const WORD y0 = 96;
    const WORD y1 = 144;
    const WORD left_x = (WORD)(x0 + 10);
    const WORD score_right = (WORD)(x1 - 10);
    const ProbeHighScoreTable *scores = table ? table : &g_probe_high_scores;

    draw_probe_panel(screen, x0, y0, x1, y1, 16);
    draw_probe_centered_text(screen, x0, x1, (WORD)(y0 + 6), "HI SCORES", 30);
    for (i = 0; i < PROBE_HIGH_SCORE_COUNT; ++i) {
        WORD row_y = (WORD)(y0 + 14 + i * 6);
        draw_probe_number(screen, left_x, row_y, (ULONG)(i + 1), 1, 29);
        draw_probe_text(screen, (WORD)(left_x + 8), row_y, scores->entries[i].initials, 29);
        draw_probe_number_right(screen, score_right, row_y, scores->entries[i].score, 5, 30);
    }
}

static void draw_probe_press_start_panel(UBYTE *screen) {
    draw_probe_panel(screen, 104, 151, 215, 177, 14);
    draw_probe_centered_text_outlined(screen, 104, 215, 160, "PRESS FIRE", 30, 1);
}

static void draw_probe_round_intro_visual_lane(UBYTE *screen,
                                               const UBYTE *chip_bobs,
                                               const UBYTE *chip_masks,
                                               const GameState *game,
                                               WORD lane_x0,
                                               WORD lane_y0,
                                               WORD lane_w,
                                               WORD lane_h,
                                               ULONG tick) {
    WORD lane_y = (WORD)(lane_y0 + ((lane_h - GAME_TILE_SIZE) / 2));
    WORD left_x = (WORD)(lane_x0 + 8);
    WORD right_x = (WORD)(lane_x0 + lane_w - GAME_TILE_SIZE - 8);
    WORD span = (WORD)(right_x - left_x);
    WORD block_x = (WORD)(left_x + (span / 3));
    WORD enemy_x = (WORD)(left_x + ((span * 2) / 3));
    WORD mine_x = right_x;
    UBYTE cue = (((tick / 5UL) & 1UL) == 0UL) ? 30 : 14;
    AmigaSpriteId enemy_sprite = (((tick / 8UL) & 1UL) != 0UL) ? AMIGA_SPR_ENEMY_A_ALT : AMIGA_SPR_ENEMY_A;
    AmigaSpriteId mine_sprite = (((tick / 6UL) & 1UL) == 0UL) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_2;

    blit_shifted_bob_twopass_probe(screen, chip_bobs, chip_masks, probe_player_sprite(DIR_RIGHT, FALSE), left_x, lane_y);
    draw_probe_visual_arrow_glyph(screen, (WORD)(left_x + 6), (WORD)(lane_y - 4), 2, cue);
    draw_probe_visual_arrow_glyph(screen, (WORD)(left_x + 6), (WORD)(lane_y + 16), 3, cue);
    draw_probe_visual_arrow_glyph(screen, (WORD)(left_x - 5), (WORD)(lane_y + 6), 0, cue);
    draw_probe_visual_arrow_glyph(screen, (WORD)(left_x + 17), (WORD)(lane_y + 6), 1, cue);

    blit_shifted_bob_twopass_probe(screen, chip_bobs, chip_masks, AMIGA_SPR_BLOCK_ICE, block_x, lane_y);
    draw_probe_visual_arrow_glyph(screen, (WORD)(((block_x + enemy_x) / 2) - 2), (WORD)(lane_y + 6), 1, cue);
    blit_shifted_bob_twopass_probe(screen, chip_bobs, chip_masks, enemy_sprite, enemy_x, lane_y);
    if (((tick / 4UL) & 1UL) == 0UL) {
        blit_shifted_bob_twopass_probe(screen, chip_bobs, chip_masks, AMIGA_SPR_EFFECT_SPARK, (WORD)(enemy_x + 2), (WORD)(lane_y - 2));
    }

    if (game && game->run_mine_capacity > 0) {
        blit_shifted_bob_twopass_probe(screen, chip_bobs, chip_masks, mine_sprite, mine_x, lane_y);
        draw_probe_visual_arrow_glyph(screen, (WORD)(mine_x - 7), (WORD)(lane_y + 6), 0, cue);
    } else {
        blit_shifted_bob_twopass_probe(screen, chip_bobs, chip_masks, AMIGA_SPR_MINE_0, mine_x, lane_y);
    }
}

static void draw_probe_round_start_panel(UBYTE *screen,
                                         const UBYTE *chip_bobs,
                                         const UBYTE *chip_masks,
                                         const GameState *game,
                                         ULONG tick) {
    const WORD panel_x0 = 48;
    const WORD panel_x1 = 271;
    const WORD panel_y0 = 58;
    const WORD panel_y1 = 143;
    const WORD panel_w = (WORD)(panel_x1 - panel_x0 + 1);
    ULONG round = game ? (ULONG)game->round : 1UL;
    const WORD round_w = probe_number_width(round, 2);
    const WORD round_x = (WORD)(panel_x0 + ((panel_w - round_w) / 2));
    const WORD lane_card_x = (WORD)(panel_x0 + 14);
    const WORD lane_card_y = (WORD)(panel_y0 + 24);
    const WORD lane_card_w = (WORD)(panel_w - 28);
    const WORD lane_card_h = 44;

    draw_probe_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 14);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, (WORD)(panel_y0 + 8), "ROUND", 31, 1);
    draw_probe_number_outlined(screen, round_x, (WORD)(panel_y0 + 16), round, 2, 31, 1);
    draw_probe_card_frame(screen, lane_card_x, lane_card_y, lane_card_w, lane_card_h, 14, TRUE);
    draw_probe_round_intro_visual_lane(screen, chip_bobs, chip_masks, game,
                                       (WORD)(lane_card_x + 6),
                                       (WORD)(lane_card_y + 4),
                                       (WORD)(lane_card_w - 12),
                                       (WORD)(lane_card_h - 8),
                                       tick);
    draw_probe_continue_chevrons(screen, (WORD)(panel_x0 + (panel_w / 2) - 7), (WORD)(panel_y1 - 11), 30);
}

static void draw_probe_perk_choice_card(UBYTE *screen,
                                        const UBYTE *chip_bobs,
                                        const UBYTE *chip_masks,
                                        const GameState *game,
                                        WORD index,
                                        WORD base_x) {
    const WORD card_w = 58;
    const WORD card_h = 44;
    const WORD spacing = 8;
    WORD px = (WORD)(base_x + index * (card_w + spacing));
    WORD py = 75;
    PerkType perk = PERK_NONE;
    BOOL selected = FALSE;
    UBYTE accent;
    if (game && index >= 0 && index < game->perk_choice_count && index < GAME_PERK_CHOICE_COUNT) {
        perk = game->perk_choices[index];
    }
    selected = (game && index == game->perk_choice_selected) ? TRUE : FALSE;
    accent = probe_perk_color(perk);
    draw_rect_probe(screen, (WORD)(px - 1), (WORD)(py - 4), (WORD)(px + card_w), (WORD)(py + card_h), 1);
    draw_probe_card_frame(screen, px, py, card_w, card_h, accent, selected);
    blit_shifted_bob_twopass_probe(screen, chip_bobs, chip_masks, probe_perk_sprite(perk),
                                   (WORD)(px + ((card_w - GAME_TILE_SIZE) / 2)),
                                   (WORD)(py + 5));
    draw_probe_centered_text_outlined(screen, (WORD)(px + 2), (WORD)(px + card_w - 3), (WORD)(py + 24), probe_perk_label(perk), accent, 1);
    draw_probe_text_outlined(screen, (WORD)(px + 18), (WORD)(py + 31), "LV", 30, 1);
    draw_probe_number_outlined(screen, (WORD)(px + 30), (WORD)(py + 31),
                               (perk > PERK_NONE && perk <= PERK_MINES && game) ? (ULONG)(game->perk_levels[perk] + 1) : 1UL,
                               1,
                               30,
                               1);
}

static void draw_probe_perk_choice_card_selection_delta(UBYTE *screen,
                                                       const GameState *game,
                                                       WORD index,
                                                       WORD base_x) {
    const WORD card_w = 58;
    const WORD card_h = 44;
    const WORD spacing = 8;
    WORD px = (WORD)(base_x + index * (card_w + spacing));
    WORD py = 75;
    PerkType perk = PERK_NONE;
    if (game && index >= 0 && index < game->perk_choice_count && index < GAME_PERK_CHOICE_COUNT) {
        perk = game->perk_choices[index];
    }
    draw_probe_card_selection_chrome(screen, px, py, card_w, card_h, probe_perk_color(perk),
                                     (game && index == game->perk_choice_selected) ? TRUE : FALSE);
}

static void draw_probe_perk_choice_footer(UBYTE *screen, const GameState *game, WORD count) {
    const WORD panel_x0 = 48;
    const WORD panel_x1 = 271;
    WORD selected_idx = game ? (WORD)game->perk_choice_selected : 0;
    PerkType selected = PERK_NONE;
    UBYTE selected_color;
    if (selected_idx < 0 || selected_idx >= count) {
        selected_idx = 0;
    }
    if (game && selected_idx < game->perk_choice_count && selected_idx < GAME_PERK_CHOICE_COUNT) {
        selected = game->perk_choices[selected_idx];
    }
    selected_color = probe_perk_color(selected);
    draw_rect_probe(screen, (WORD)(panel_x0 + 4), 122, (WORD)(panel_x1 - 4), 140, 1);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, 124, probe_perk_label(selected), selected_color, 1);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, 130, probe_perk_effect_label(selected), 30, 1);
    draw_probe_visual_arrow_glyph(screen, (WORD)(panel_x0 + 8), 135, 0, 30);
    draw_probe_visual_arrow_glyph(screen, (WORD)(panel_x1 - 12), 135, 1, 30);
    draw_probe_continue_chevrons(screen, (WORD)(panel_x0 + ((panel_x1 - panel_x0) / 2) - 7), 135, selected_color);
}

static void draw_probe_perk_choice_panel(UBYTE *screen,
                                         const UBYTE *chip_bobs,
                                         const UBYTE *chip_masks,
                                         const GameState *game) {
    const WORD panel_x0 = 48;
    const WORD panel_x1 = 271;
    const WORD panel_y0 = 58;
    const WORD panel_y1 = 143;
    WORD count = probe_perk_choice_count_clamped(game);
    WORD base_x = probe_perk_choice_base_x(count);
    WORD i;
    draw_probe_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 18);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, 64, "PICK PERK", 31, 1);
    for (i = 0; i < count; ++i) {
        draw_probe_perk_choice_card(screen, chip_bobs, chip_masks, game, i, base_x);
    }
    draw_probe_perk_choice_footer(screen, game, count);
}

static void draw_probe_perk_choice_delta(UBYTE *screen, const GameState *game, WORD previous_selection) {
    WORD count = probe_perk_choice_count_clamped(game);
    WORD base_x = probe_perk_choice_base_x(count);
    WORD selected = game ? (WORD)game->perk_choice_selected : 0;
    if (selected < 0 || selected >= count) {
        selected = 0;
    }
    if (previous_selection >= 0 && previous_selection < count && previous_selection != selected) {
        draw_probe_perk_choice_card_selection_delta(screen, game, previous_selection, base_x);
    }
    draw_probe_perk_choice_card_selection_delta(screen, game, selected, base_x);
    draw_probe_perk_choice_footer(screen, game, count);
}

static PerkType probe_meta_as_perk(MetaUpgradeType upgrade) {
    switch (upgrade) {
        case META_UPGRADE_LIFE_PACK:
            return PERK_LIFE_BOOST;
        case META_UPGRADE_TIME_PACK:
            return PERK_TIME_BOOST;
        case META_UPGRADE_SCORE_PACK:
            return PERK_SCORE_BOOST;
        case META_UPGRADE_SLOW_PACK:
            return PERK_ENEMY_SLOW;
        default:
            return PERK_NONE;
    }
}

static const char *probe_meta_label(MetaUpgradeType upgrade) {
    PerkType perk = probe_meta_as_perk(upgrade);
    return (perk == PERK_NONE) ? "NONE" : probe_perk_label(perk);
}

static const char *probe_meta_effect_label(MetaUpgradeType upgrade) {
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

static UBYTE probe_meta_color(MetaUpgradeType upgrade) {
    PerkType perk = probe_meta_as_perk(upgrade);
    return (perk == PERK_NONE) ? 30 : probe_perk_color(perk);
}

static AmigaSpriteId probe_meta_sprite(MetaUpgradeType upgrade) {
    PerkType perk = probe_meta_as_perk(upgrade);
    return (perk == PERK_NONE) ? AMIGA_SPR_ITEM_MYSTERY : probe_perk_sprite(perk);
}

static WORD probe_meta_choice_count_clamped(const GameState *game) {
    WORD count = game ? (WORD)game->meta_choice_count : 1;
    if (count < 1) {
        count = 1;
    }
    if (count > GAME_META_CHOICE_COUNT) {
        count = GAME_META_CHOICE_COUNT;
    }
    return count;
}

static WORD probe_meta_choice_base_x(WORD count) {
    const WORD panel_x0 = 36;
    const WORD panel_x1 = 283;
    const WORD card_w = 48;
    const WORD spacing = 8;
    WORD total_w = (WORD)(count * card_w + (count - 1) * spacing);
    return (WORD)(panel_x0 + ((panel_x1 - panel_x0 + 1 - total_w) / 2));
}

static void draw_probe_meta_choice_card(UBYTE *screen,
                                        const UBYTE *chip_bobs,
                                        const UBYTE *chip_masks,
                                        const GameState *game,
                                        WORD index,
                                        WORD base_x) {
    const WORD card_w = 48;
    const WORD card_h = 44;
    const WORD spacing = 8;
    WORD px = (WORD)(base_x + index * (card_w + spacing));
    WORD py = 75;
    MetaUpgradeType upgrade = META_UPGRADE_NONE;
    WORD item_cost = 0;
    BOOL selected = FALSE;
    BOOL affordable = TRUE;
    UBYTE accent;
    if (game && index >= 0 && index < game->meta_choice_count && index < GAME_META_CHOICE_COUNT) {
        upgrade = game->meta_choices[index];
        item_cost = (WORD)game->meta_choice_costs[index];
    }
    selected = (game && index == game->meta_choice_selected) ? TRUE : FALSE;
    affordable = (game && game->meta_shards < (ULONG)clamp_int_probe(item_cost, 0, 999)) ? FALSE : TRUE;
    accent = affordable ? probe_meta_color(upgrade) : 23;
    draw_rect_probe(screen, (WORD)(px - 1), (WORD)(py - 4), (WORD)(px + card_w), (WORD)(py + card_h), 1);
    draw_probe_card_frame(screen, px, py, card_w, card_h, accent, selected);
    blit_shifted_bob_twopass_probe(screen, chip_bobs, chip_masks, probe_meta_sprite(upgrade),
                                   (WORD)(px + ((card_w - GAME_TILE_SIZE) / 2)),
                                   (WORD)(py + 5));
    draw_probe_centered_text_outlined(screen, (WORD)(px + 2), (WORD)(px + card_w - 3), (WORD)(py + 24), probe_meta_label(upgrade), accent, 1);
    draw_probe_text_outlined(screen, (WORD)(px + 14), (WORD)(py + 31), "C", affordable ? 30 : 23, 1);
    draw_probe_number_outlined(screen, (WORD)(px + 22), (WORD)(py + 31), (ULONG)clamp_int_probe(item_cost, 0, 999), 1, affordable ? 30 : 23, 1);
}

static void draw_probe_meta_choice_card_selection_delta(UBYTE *screen,
                                                        const GameState *game,
                                                        WORD index,
                                                        WORD base_x) {
    const WORD card_w = 48;
    const WORD card_h = 44;
    const WORD spacing = 8;
    WORD px = (WORD)(base_x + index * (card_w + spacing));
    WORD py = 75;
    MetaUpgradeType upgrade = META_UPGRADE_NONE;
    WORD item_cost = 0;
    BOOL affordable = TRUE;
    UBYTE accent;
    if (game && index >= 0 && index < game->meta_choice_count && index < GAME_META_CHOICE_COUNT) {
        upgrade = game->meta_choices[index];
        item_cost = (WORD)game->meta_choice_costs[index];
    }
    affordable = (game && game->meta_shards < (ULONG)clamp_int_probe(item_cost, 0, 999)) ? FALSE : TRUE;
    accent = affordable ? probe_meta_color(upgrade) : 23;
    draw_probe_card_selection_chrome(screen, px, py, card_w, card_h, accent,
                                     (game && index == game->meta_choice_selected) ? TRUE : FALSE);
}

static void draw_probe_meta_choice_footer(UBYTE *screen, const GameState *game, WORD count) {
    const WORD panel_x0 = 36;
    const WORD panel_x1 = 283;
    WORD selected_idx = game ? (WORD)game->meta_choice_selected : 0;
    MetaUpgradeType selected = META_UPGRADE_NONE;
    WORD cost = 0;
    BOOL affordable;
    UBYTE selected_color;
    if (selected_idx < 0 || selected_idx >= count) {
        selected_idx = 0;
    }
    if (game && selected_idx < game->meta_choice_count && selected_idx < GAME_META_CHOICE_COUNT) {
        selected = game->meta_choices[selected_idx];
        cost = (WORD)game->meta_choice_costs[selected_idx];
    }
    affordable = (game && game->meta_shards < (ULONG)clamp_int_probe(cost, 0, 999)) ? FALSE : TRUE;
    selected_color = affordable ? 30 : 23;
    draw_rect_probe(screen, (WORD)(panel_x0 + 4), 122, (WORD)(panel_x1 - 4), 140, 1);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, 124, probe_meta_label(selected), affordable ? probe_meta_color(selected) : 23, 1);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, 130, probe_meta_effect_label(selected), selected_color, 1);
    draw_probe_visual_arrow_glyph(screen, (WORD)(panel_x0 + 8), 135, 0, 30);
    draw_probe_visual_arrow_glyph(screen, (WORD)(panel_x1 - 12), 135, 1, 30);
    draw_probe_continue_chevrons(screen, (WORD)(panel_x0 + ((panel_x1 - panel_x0) / 2) - 7), 135, selected_color);
}

static void draw_probe_meta_choice_panel(UBYTE *screen,
                                         const UBYTE *chip_bobs,
                                         const UBYTE *chip_masks,
                                         const GameState *game) {
    const WORD panel_x0 = 36;
    const WORD panel_x1 = 283;
    const WORD panel_y0 = 58;
    const WORD panel_y1 = 143;
    WORD count = probe_meta_choice_count_clamped(game);
    WORD base_x = probe_meta_choice_base_x(count);
    WORD i;
    draw_probe_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 16);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, 64, "META UPGRADE", 31, 1);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, 70, "SHARDS", 30, 1);
    draw_probe_number_right(screen, (WORD)(panel_x1 - 10), 70, game ? game->meta_shards : 0UL, 1, 8);
    for (i = 0; i < count; ++i) {
        draw_probe_meta_choice_card(screen, chip_bobs, chip_masks, game, i, base_x);
    }
    draw_probe_meta_choice_footer(screen, game, count);
}

static void draw_probe_meta_choice_delta(UBYTE *screen, const GameState *game, WORD previous_selection) {
    WORD count = probe_meta_choice_count_clamped(game);
    WORD base_x = probe_meta_choice_base_x(count);
    WORD selected = game ? (WORD)game->meta_choice_selected : 0;
    if (selected < 0 || selected >= count) {
        selected = 0;
    }
    if (previous_selection >= 0 && previous_selection < count && previous_selection != selected) {
        draw_probe_meta_choice_card_selection_delta(screen, game, previous_selection, base_x);
    }
    draw_probe_meta_choice_card_selection_delta(screen, game, selected, base_x);
    draw_probe_meta_choice_footer(screen, game, count);
}

static void probe_format_signed_bonus(ULONG bonus, char *out, WORD out_len) {
    char digits[10];
    WORD count = 0;
    WORD i;
    ULONG v = bonus;
    if (!out || out_len < 3) {
        return;
    }
    do {
        digits[count++] = (char)('0' + (v % 10UL));
        v /= 10UL;
    } while (v > 0UL && count < 10);
    out[0] = '+';
    for (i = (WORD)(count - 1); i >= 0 && (WORD)(count - i) < (WORD)(out_len - 1); --i) {
        out[count - i] = digits[i];
    }
    out[count + 1] = '\0';
}

static void draw_probe_round_clear_panel(UBYTE *screen, const GameState *game) {
    const WORD panel_x0 = 60;
    const WORD panel_x1 = 259;
    const WORD panel_y0 = 74;
    const WORD panel_y1 = 137;
    ULONG bonus = 0UL;
    char bonus_text[16];
    if (game && game->round_clear_bonus_score > 0) {
        bonus = (ULONG)game->round_clear_bonus_score;
    }
    probe_format_signed_bonus(bonus, bonus_text, (WORD)sizeof(bonus_text));
    draw_probe_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 11);
    draw_rect_probe(screen, (WORD)(panel_x0 + 5), (WORD)(panel_y0 + 5), (WORD)(panel_x1 - 5), (WORD)(panel_y0 + 20), 1);
    draw_probe_centered_text_scaled_outlined(screen, panel_x0, panel_x1, (WORD)(panel_y0 + 8), "ROUND CLEAR", 31, 1, 2);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, (WORD)(panel_y0 + 28), "TIME BONUS", 30, 1);
    draw_probe_centered_text_scaled_outlined(screen, panel_x0, panel_x1, (WORD)(panel_y0 + 39), bonus_text, 31, 1, 2);
    draw_probe_continue_chevrons(screen, (WORD)(panel_x0 + ((panel_x1 - panel_x0) / 2) - 7), (WORD)(panel_y1 - 10), 31);
}

static void draw_probe_game_over_panel(UBYTE *screen, const GameState *game) {
    const WORD panel_x0 = 56;
    const WORD panel_x1 = 263;
    const WORD panel_y0 = 70;
    const WORD panel_y1 = 145;
    ULONG score = game ? game->score : 0UL;
    const WORD score_scale = 3;
    WORD score_w = probe_number_width_scaled(score, 5, score_scale);
    WORD score_x = (WORD)(panel_x0 + (((panel_x1 - panel_x0 + 1) - score_w) / 2));
    draw_probe_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 21);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, (WORD)(panel_y0 + 7), "GAME OVER", 31, 1);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, (WORD)(panel_y0 + 16), "FINAL SCORE", 30, 1);
    draw_probe_number_scaled_outlined(screen, score_x, (WORD)(panel_y0 + 25), score, 5, 31, 1, score_scale);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, (WORD)(panel_y1 - 9), "FIRE RESTART", 31, 1);
}

static void draw_probe_enter_hi_score_slots(UBYTE *screen, const ProbeHighScoreEntryState *entry) {
    const WORD panel_x0 = 54;
    const WORD panel_x1 = 265;
    const WORD slot_w = 28;
    const WORD slot_h = 24;
    const WORD slot_spacing = 10;
    const WORD slots_total_w = (WORD)(slot_w * 3 + slot_spacing * 2);
    const WORD slots_x0 = (WORD)(panel_x0 + ((panel_x1 - panel_x0 + 1 - slots_total_w) / 2));
    const WORD slots_y = 98;
    WORD i;
    if (!entry || !entry->active) {
        return;
    }
    draw_rect_probe(screen,
                    (WORD)(slots_x0 - 5),
                    (WORD)(slots_y - 8),
                    (WORD)(slots_x0 + slots_total_w + 4),
                    (WORD)(slots_y + slot_h + 8),
                    1);
    for (i = 0; i < 3; ++i) {
        BOOL selected = (i == entry->cursor) ? TRUE : FALSE;
        WORD slot_x = (WORD)(slots_x0 + i * (slot_w + slot_spacing));
        char letter[2];
        WORD glyph_w;
        letter[0] = entry->initials[i];
        letter[1] = '\0';
        glyph_w = probe_text_width_scaled(letter, 3);
        draw_probe_card_frame(screen, slot_x, slots_y, slot_w, slot_h, selected ? 29 : 17, selected);
        draw_probe_text_scaled_outlined(screen,
                                        (WORD)(slot_x + ((slot_w - glyph_w) / 2)),
                                        (WORD)(slots_y + 5),
                                        letter,
                                        selected ? 31 : 30,
                                        1,
                                        3);
    }
    {
        WORD slot_x = (WORD)(slots_x0 + entry->cursor * (slot_w + slot_spacing));
        WORD center_x = (WORD)(slot_x + (slot_w / 2) - 2);
        draw_probe_visual_arrow_glyph(screen, center_x, (WORD)(slots_y - 7), 2, 30);
        draw_probe_visual_arrow_glyph(screen, center_x, (WORD)(slots_y + slot_h + 2), 3, 30);
    }
}

static void draw_probe_enter_hi_score_panel(UBYTE *screen, const ProbeHighScoreEntryState *entry) {
    const WORD panel_x0 = 54;
    const WORD panel_x1 = 265;
    const WORD panel_y0 = 50;
    const WORD panel_y1 = 149;

    if (!entry || !entry->active) {
        return;
    }

    draw_probe_panel(screen, panel_x0, panel_y0, panel_x1, panel_y1, 16);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, (WORD)(panel_y0 + 8), "NEW HI SCORE", 31, 1);
    draw_probe_text_outlined(screen, (WORD)(panel_x0 + 84), (WORD)(panel_y0 + 17), "RANK", 30, 1);
    draw_probe_number_outlined(screen, (WORD)(panel_x0 + 104), (WORD)(panel_y0 + 17), (ULONG)(entry->insert_index + 1), 1, 30, 1);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, (WORD)(panel_y0 + 25), "SCORE", 30, 1);
    draw_probe_number_outlined(screen,
                               (WORD)(panel_x0 + ((panel_x1 - panel_x0 + 1 - probe_number_width(entry->pending_score, 1)) / 2)),
                               (WORD)(panel_y0 + 33),
                               entry->pending_score,
                               1,
                               8,
                               1);
    draw_probe_enter_hi_score_slots(screen, entry);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, (WORD)(panel_y1 - 18), "ARROWS EDIT", 30, 1);
    draw_probe_centered_text_outlined(screen, panel_x0, panel_x1, (WORD)(panel_y1 - 10), "FIRE NEXT/SAVE", 31, 1);
}

static void draw_probe_ui_panel(UBYTE *screen,
                                const UBYTE *chip_bobs,
                                const UBYTE *chip_masks,
                                const GameState *game,
                                WORD ui_mode,
                                ULONG tick) {
    switch (ui_mode) {
        case PROBE_UI_ROUND_START:
            draw_probe_round_start_panel(screen, chip_bobs, chip_masks, game, tick);
            break;
        case PROBE_UI_ROUND_CLEAR:
            draw_probe_round_clear_panel(screen, game);
            break;
        case PROBE_UI_GAME_OVER:
            draw_probe_game_over_panel(screen, game);
            break;
        case PROBE_UI_ENTER_HI_SCORE:
            if (g_probe_high_score_entry.active) {
                draw_probe_enter_hi_score_panel(screen, &g_probe_high_score_entry);
            } else if (game->phase == GAME_PHASE_PERK_CHOICE) {
                draw_probe_perk_choice_panel(screen, chip_bobs, chip_masks, game);
            } else if (game->phase == GAME_PHASE_META_UPGRADE) {
                draw_probe_meta_choice_panel(screen, chip_bobs, chip_masks, game);
            } else {
                draw_probe_enter_hi_score_panel(screen, &g_probe_high_score_entry);
            }
            break;
        case PROBE_UI_TITLE_SCORES:
        default:
            draw_probe_opening_high_scores(screen, &g_probe_high_scores);
            draw_probe_press_start_panel(screen);
            break;
    }
}

static BOOL probe_ui_uses_title(WORD ui_mode) {
    return (ui_mode == PROBE_UI_TITLE_SCORES) ? TRUE : FALSE;
}

static WORD probe_ui_mode_for_game(const GameState *game) {
    if (g_probe_high_score_entry.active) {
        return PROBE_UI_ENTER_HI_SCORE;
    }
    if (!game) {
        return -1;
    }
    if (game->phase == GAME_PHASE_ROUND_INTRO && game->start_title_pending) {
        return PROBE_UI_TITLE_SCORES;
    }
    if (game->phase == GAME_PHASE_ROUND_INTRO) {
        return PROBE_UI_ROUND_START;
    }
    if (game->phase == GAME_PHASE_ROUND_CLEAR) {
        return PROBE_UI_ROUND_CLEAR;
    }
    if (game->phase == GAME_PHASE_GAME_OVER) {
        return PROBE_UI_GAME_OVER;
    }
    if (game->phase == GAME_PHASE_PERK_CHOICE || game->phase == GAME_PHASE_META_UPGRADE) {
        return PROBE_UI_ENTER_HI_SCORE;
    }
    return -1;
}

static WORD probe_ui_selection_for_game(const GameState *game) {
    if (g_probe_high_score_entry.active) {
        return probe_high_score_entry_dynamic_key(&g_probe_high_score_entry);
    }
    if (!game) {
        return -1;
    }
    if (game->phase == GAME_PHASE_PERK_CHOICE) {
        WORD count = probe_perk_choice_count_clamped(game);
        WORD selected = (WORD)game->perk_choice_selected;
        if (selected < 0 || selected >= count) {
            selected = 0;
        }
        return selected;
    }
    if (game->phase == GAME_PHASE_META_UPGRADE) {
        WORD count = (WORD)game->meta_choice_count;
        WORD selected = (WORD)game->meta_choice_selected;
        if (count < 1) count = 1;
        if (count > GAME_META_CHOICE_COUNT) count = GAME_META_CHOICE_COUNT;
        if (selected < 0 || selected >= count) {
            selected = 0;
        }
        return selected;
    }
    return -1;
}

static ULONG probe_ui_layout_key_for_game(const GameState *game, WORD ui_mode) {
    ULONG key = (ULONG)(ui_mode & 0xFF);
    WORD i;
    if (g_probe_high_score_entry.active) {
        key = (key * 33UL) ^ 0x48534CUL;
        key = (key * 33UL) ^ (ULONG)g_probe_high_score_entry.insert_index;
        key = (key * 33UL) ^ g_probe_high_score_entry.pending_score;
        return key;
    }
    if (ui_mode == PROBE_UI_TITLE_SCORES) {
        for (i = 0; i < PROBE_HIGH_SCORE_COUNT; ++i) {
            key = (key * 33UL) ^ g_probe_high_scores.entries[i].score;
            key = (key * 33UL) ^ (ULONG)(UBYTE)g_probe_high_scores.entries[i].initials[0];
            key = (key * 33UL) ^ (ULONG)(UBYTE)g_probe_high_scores.entries[i].initials[1];
            key = (key * 33UL) ^ (ULONG)(UBYTE)g_probe_high_scores.entries[i].initials[2];
        }
    }
    if (!game) {
        return key;
    }
    key = (key * 33UL) ^ (ULONG)(game->phase & 0xFF);
    key = (key * 33UL) ^ (ULONG)((game->start_title_pending != 0) ? 1u : 0u);
    switch (game->phase) {
        case GAME_PHASE_ROUND_INTRO:
            key = (key * 33UL) ^ (ULONG)(game->round & 0xFF);
            key = (key * 33UL) ^ (ULONG)(game->run_mine_capacity & 0xFF);
            break;
        case GAME_PHASE_ROUND_CLEAR:
            key = (key * 33UL) ^ (ULONG)(game->round_clear_bonus_score & 0xFFFF);
            break;
        case GAME_PHASE_PERK_CHOICE:
            key = (key * 33UL) ^ (ULONG)probe_perk_choice_count_clamped(game);
            for (i = 0; i < GAME_PERK_CHOICE_COUNT; ++i) {
                key = (key * 33UL) ^ (ULONG)(game->perk_choices[i] & 0xFF);
            }
            break;
        case GAME_PHASE_META_UPGRADE:
            key = (key * 33UL) ^ (ULONG)(game->meta_choice_count & 0xFF);
            key = (key * 33UL) ^ (ULONG)(game->meta_shards & 0xFFFF);
            for (i = 0; i < GAME_META_CHOICE_COUNT; ++i) {
                key = (key * 33UL) ^ (ULONG)(game->meta_choices[i] & 0xFF);
                key = (key * 33UL) ^ (ULONG)(game->meta_choice_costs[i] & 0xFF);
            }
            break;
        default:
            break;
    }
    return key;
}

static BOOL probe_ui_selection_delta_supported(const GameState *game) {
    if (g_probe_high_score_entry.active) {
        return TRUE;
    }
    return (game && (game->phase == GAME_PHASE_PERK_CHOICE ||
                     game->phase == GAME_PHASE_META_UPGRADE)) ? TRUE : FALSE;
}

static BOOL draw_probe_ui_delta(UBYTE *screen, const GameState *game, WORD previous_selection) {
    if (g_probe_high_score_entry.active) {
        draw_probe_enter_hi_score_slots(screen, &g_probe_high_score_entry);
        return TRUE;
    }
    if (game && game->phase == GAME_PHASE_PERK_CHOICE) {
        draw_probe_perk_choice_delta(screen, game, previous_selection);
        return TRUE;
    }
    if (game && game->phase == GAME_PHASE_META_UPGRADE) {
        draw_probe_meta_choice_delta(screen, game, previous_selection);
        return TRUE;
    }
    return FALSE;
}

static BOOL probe_ui_band_for_game(const GameState *game, WORD ui_mode, WORD *y0, WORD *y1) {
    if (!y0 || !y1) {
        return FALSE;
    }
    switch (ui_mode) {
        case PROBE_UI_TITLE_SCORES:
            *y0 = 0;
            *y1 = 183;
            return TRUE;
        case PROBE_UI_ROUND_START:
            *y0 = 54;
            *y1 = 148;
            return TRUE;
        case PROBE_UI_ROUND_CLEAR:
            *y0 = 70;
            *y1 = 144;
            return TRUE;
        case PROBE_UI_GAME_OVER:
            *y0 = 66;
            *y1 = 150;
            return TRUE;
        case PROBE_UI_ENTER_HI_SCORE:
            if (g_probe_high_score_entry.active) {
                *y0 = 46;
                *y1 = 154;
            } else if (game && game->phase == GAME_PHASE_META_UPGRADE) {
                *y0 = 54;
                *y1 = 148;
            } else {
                *y0 = 54;
                *y1 = 148;
            }
            return TRUE;
        default:
            break;
    }
    return FALSE;
}

static void draw_probe_hud_separator(UBYTE *screen, WORD x) {
    WORD y;
    for (y = 193; y < SCREEN_H - 1; ++y) {
        put_pixel_probe(screen, x, y, 2);
        put_pixel_probe(screen, (WORD)(x + 1), y, 3);
    }
}

static UBYTE probe_hud_bomb_color(const GameState *game) {
    if (game->run_mine_capacity <= 0) {
        return 3;
    }
    if (game->run_mine_stock <= 0) {
        return 23;
    }
    return 30;
}

static void draw_probe_hud_bomb_meter(UBYTE *screen,
                                      WORD x,
                                      WORD y,
                                      WORD mine_stock,
                                      WORD mine_capacity,
                                      WORD anim_phase) {
    WORD i;
    WORD stock = (WORD)clamp_int_probe(mine_stock, 0, 8);
    WORD capacity = (WORD)clamp_int_probe(mine_capacity, 0, 8);
    anim_phase = 0;
    for (i = 0; i < 8; ++i) {
        WORD cell_x = (WORD)(x + (i % 4) * 3);
        WORD cell_y = (WORD)(y + (i / 4) * 3);
        UBYTE color = 2;
        if (i < capacity) {
            color = 3;
        }
        if (i < stock) {
            color = 30;
        }
        fill_rect_probe(screen, cell_x, cell_y, 2, 2, color);
    }
}

static UWORD probe_hud_seconds_from_ticks_slow(WORD ticks) {
    UWORD clamped;
    if (ticks <= 0) {
        return 0;
    }
    clamped = (UWORD)ticks;
    return (UWORD)((clamped + (UWORD)(GAME_FIXED_TICK_HZ - 1)) / (UWORD)GAME_FIXED_TICK_HZ);
}

static UWORD probe_hud_next_second_tick(UWORD seconds) {
    if (seconds == 0) {
        return 0;
    }
    return (UWORD)((seconds - 1u) * (UWORD)GAME_FIXED_TICK_HZ);
}

static UWORD probe_hud_seconds_cached(ProbeRenderCache *cache, WORD buf, const GameState *game) {
    WORD ticks;
    if (!game) {
        return 0;
    }
    ticks = (WORD)game->round_time_ticks;
    if (ticks < 0) {
        ticks = 0;
    }
    if (cache && buf >= 0 && buf < PROBE_DISPLAY_BUFFER_COUNT && cache->hud_valid[buf]) {
        UWORD raw_ticks = (UWORD)ticks;
        if (raw_ticks == cache->hud_raw_ticks[buf]) {
            return cache->hud[buf].seconds;
        }
        if (raw_ticks < cache->hud_raw_ticks[buf] &&
            raw_ticks > cache->hud_next_second_tick[buf]) {
            return cache->hud[buf].seconds;
        }
    }
    return probe_hud_seconds_from_ticks_slow(ticks);
}

static ProbeHudSnapshot probe_make_hud_snapshot(ProbeRenderCache *cache, WORD buf, const GameState *game) {
    ProbeHudSnapshot hud;
    hud.score = (ULONG)game->score;
    if (hud.score > 99999999UL) {
        hud.score = 99999999UL;
    }
    hud.seconds = probe_hud_seconds_cached(cache, buf, game);
    if (hud.seconds > 99u) {
        hud.seconds = 99u;
    }
    hud.round = (UWORD)clamp_int_probe(game->round, 0, 99);
    hud.lives = (UWORD)clamp_int_probe(game->lives, 0, 99);
    hud.mine_stock = (UWORD)clamp_int_probe(game->run_mine_stock, 0, 99);
    hud.mine_capacity = (UWORD)clamp_int_probe(game->run_mine_capacity, 0, 99);
    hud.bomb_blink = 0;
    hud.meter_blink = 0;
    return hud;
}

static BOOL probe_hud_snapshot_equal(const ProbeHudSnapshot *a, const ProbeHudSnapshot *b) {
    return (a->score == b->score &&
            a->seconds == b->seconds &&
            a->round == b->round &&
            a->lives == b->lives &&
            a->mine_stock == b->mine_stock &&
            a->mine_capacity == b->mine_capacity &&
            a->bomb_blink == b->bomb_blink &&
            a->meter_blink == b->meter_blink) ? TRUE : FALSE;
}

static void draw_probe_hud(UBYTE *screen, const GameState *game) {
    ULONG score = (ULONG)game->score;
    ULONG seconds = (ULONG)probe_hud_seconds_from_ticks_slow((WORD)game->round_time_ticks);
    WORD mine_stock = (WORD)game->run_mine_stock;
    WORD mine_capacity = (WORD)game->run_mine_capacity;
    WORD lives = (WORD)game->lives;
    ULONG round = (ULONG)game->round;
    UBYTE bomb_color = probe_hud_bomb_color(game);
    WORD i;
    WORD y;
    wait_blitter();
    fill_rect_probe(screen, 0, 192, SCREEN_W, 8, 1);
    draw_hline_probe(screen, 0, (WORD)(SCREEN_W - 1), 192, 3);
    draw_hline_probe(screen, 0, (WORD)(SCREEN_W - 1), (WORD)(SCREEN_H - 1), 2);
    for (y = 193; y < SCREEN_H - 1; ++y) {
        if ((y & 1) == 0) {
            for (i = 0; i < SCREEN_W; i = (WORD)(i + 32)) {
                put_pixel_probe(screen, i, y, 2);
            }
        }
    }
    draw_probe_hud_separator(screen, 142);
    draw_probe_hud_separator(screen, 174);
    draw_probe_hud_separator(screen, 218);
    draw_probe_hud_separator(screen, 266);
    draw_probe_text_outlined(screen, 4, 193, "SCORE", 7, 1);
    draw_probe_number_outlined(screen, 34, 193, score > 99999999UL ? 99999999UL : score, 6, 29, 1);
    draw_probe_text_outlined(screen, 148, 193, "RD", 7, 1);
    draw_probe_number_outlined(screen, 161, 193, round > 99UL ? 99UL : round, 2, 18, 1);
    draw_probe_text_outlined(screen, 180, 193, "TIME", 7, 1);
    draw_probe_number_outlined(screen, 201, 193, seconds > 99UL ? 99UL : seconds, 2, 14, 1);
    draw_probe_text_outlined(screen, 224, 193, "BOMB", 7, 1);
    draw_probe_number_outlined(screen, 244, 193, (ULONG)clamp_int_probe(mine_stock, 0, 99), 1, bomb_color, 1);
    draw_probe_text_outlined(screen, 248, 193, ":", 7, 1);
    draw_probe_number_outlined(screen, 252, 193, (ULONG)clamp_int_probe(mine_capacity, 0, 99), 1, bomb_color, 1);
    draw_probe_hud_bomb_meter(screen,
                              255,
                              193,
                              mine_stock,
                              mine_capacity,
                              (WORD)game->round_time_ticks);
    draw_probe_text_outlined(screen, 271, 193, "LIV", 7, 1);
    draw_probe_number_outlined(screen, 287, 193, (ULONG)lives, 2, 23, 1);
}

static WORD probe_hud_format_number(ULONG value, WORD min_digits, char *out, WORD out_cap) {
    char tmp[10];
    WORD count = 0;
    WORD i;
    if (!out || out_cap <= 0) {
        return 0;
    }
    do {
        tmp[count++] = (char)('0' + (value % 10UL));
        value /= 10UL;
    } while (value > 0UL && count < 10);
    while (count < min_digits && count < 10) {
        tmp[count++] = '0';
    }
    if (count >= out_cap) {
        count = (WORD)(out_cap - 1);
    }
    for (i = 0; i < count; ++i) {
        out[i] = tmp[count - 1 - i];
    }
    out[count] = '\0';
    return count;
}

static void probe_hud_clear_field(UBYTE *screen, WORD x0, WORD x1) {
    if (x0 < 0) x0 = 0;
    if (x1 >= SCREEN_W) x1 = SCREEN_W - 1;
    if (x1 >= x0) {
        fill_rect_probe(screen, x0, 192, (WORD)(x1 - x0 + 1), 7, 1);
    }
}

static void probe_hud_clear_char_cell(UBYTE *screen, WORD x) {
    probe_hud_clear_field(screen, (WORD)(x - 1), (WORD)(x + 3));
}

static void draw_probe_hud_number_delta(UBYTE *screen,
                                        WORD x,
                                        ULONG old_value,
                                        ULONG new_value,
                                        WORD min_digits,
                                        UBYTE color,
                                        UBYTE outline,
                                        BOOL force_all) {
    char old_digits[11];
    char new_digits[11];
    UBYTE redraw[10];
    WORD old_len;
    WORD new_len;
    WORD i;
    old_len = probe_hud_format_number(old_value, min_digits, old_digits, (WORD)sizeof(old_digits));
    new_len = probe_hud_format_number(new_value, min_digits, new_digits, (WORD)sizeof(new_digits));
    if (force_all || old_len != new_len) {
        WORD max_len = (old_len > new_len) ? old_len : new_len;
        probe_hud_clear_field(screen, (WORD)(x - 1), (WORD)(x + max_len * 4));
        draw_probe_number_outlined(screen, x, 193, new_value, min_digits, color, outline);
        return;
    }
    for (i = 0; i < 10; ++i) {
        redraw[i] = 0;
    }
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
            probe_hud_clear_char_cell(screen, (WORD)(x + i * 4));
        }
    }
    for (i = 0; i < new_len; ++i) {
        if (redraw[i]) {
            draw_probe_char_outlined(screen, (WORD)(x + i * 4), 193, new_digits[i], color, outline);
        }
    }
}

static void draw_probe_hud_cached(ProbeRenderCache *cache, WORD buf, UBYTE *screen, const GameState *game) {
    ProbeHudSnapshot hud;
    ProbeHudSnapshot old_hud;
    UWORD raw_ticks;
    if (!game || !screen) {
        return;
    }
    if (!cache || buf < 0 || buf >= PROBE_DISPLAY_BUFFER_COUNT) {
        draw_probe_hud(screen, game);
        return;
    }
    hud = probe_make_hud_snapshot(cache, buf, game);
    raw_ticks = (game->round_time_ticks <= 0) ? 0u : (UWORD)game->round_time_ticks;
    if (cache->hud_valid[buf] && probe_hud_snapshot_equal(&cache->hud[buf], &hud)) {
        cache->hud_raw_ticks[buf] = raw_ticks;
        return;
    }
    if (!cache->hud_valid[buf]) {
        draw_probe_hud(screen, game);
    } else {
        wait_blitter();
        old_hud = cache->hud[buf];
        if (old_hud.score != hud.score) {
            draw_probe_hud_number_delta(screen, 34, old_hud.score, hud.score, 6, 29, 1, FALSE);
        }
        if (old_hud.round != hud.round) {
            draw_probe_hud_number_delta(screen, 161, (ULONG)old_hud.round, (ULONG)hud.round, 2, 18, 1, FALSE);
        }
        if (old_hud.seconds != hud.seconds) {
            draw_probe_hud_number_delta(screen, 201, (ULONG)old_hud.seconds, (ULONG)hud.seconds, 2, 14, 1, FALSE);
        }
        if (old_hud.mine_stock != hud.mine_stock ||
            old_hud.mine_capacity != hud.mine_capacity ||
            old_hud.bomb_blink != hud.bomb_blink ||
            old_hud.meter_blink != hud.meter_blink) {
            UBYTE bomb_color = probe_hud_bomb_color(game);
            probe_hud_clear_field(screen, 243, 266);
            draw_probe_number_outlined(screen, 244, 193, (ULONG)hud.mine_stock, 1, bomb_color, 1);
            draw_probe_text_outlined(screen, 248, 193, ":", 7, 1);
            draw_probe_number_outlined(screen, 252, 193, (ULONG)hud.mine_capacity, 1, bomb_color, 1);
            draw_probe_hud_bomb_meter(screen, 255, 193, (WORD)hud.mine_stock, (WORD)hud.mine_capacity, (WORD)game->round_time_ticks);
        }
        if (old_hud.lives != hud.lives) {
            draw_probe_hud_number_delta(screen, 287, (ULONG)old_hud.lives, (ULONG)hud.lives, 2, 23, 1, FALSE);
        }
    }
    cache->hud[buf] = hud;
    cache->hud_raw_ticks[buf] = raw_ticks;
    cache->hud_next_second_tick[buf] = probe_hud_next_second_tick(hud.seconds);
    cache->hud_valid[buf] = 1;
}


static AmigaSpriteId probe_player_sprite(Direction d, BOOL alt) {
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

static AmigaSpriteId probe_enemy_sprite(const Enemy *enemy) {
    BOOL alt = (enemy->anim_frame & 1) ? TRUE : FALSE;
    if (enemy->type == ENEMY_TYPE_GHOST) {
        return alt ? AMIGA_SPR_ENEMY_D_ALT : AMIGA_SPR_ENEMY_D;
    }
    if (enemy->type == ENEMY_TYPE_HUNTER) {
        return alt ? AMIGA_SPR_ENEMY_C_ALT : AMIGA_SPR_ENEMY_C;
    }
    if (enemy->type == ENEMY_TYPE_WANDERER) {
        return alt ? AMIGA_SPR_ENEMY_B_ALT : AMIGA_SPR_ENEMY_B;
    }
    return alt ? AMIGA_SPR_ENEMY_A_ALT : AMIGA_SPR_ENEMY_A;
}

static BOOL probe_enemy_visible(const Enemy *enemy, ULONG tick) {
    if (enemy->state != ENEMY_SPAWNING || enemy->spawn_ticks <= 0) {
        return TRUE;
    }
    return (((tick / 5UL) & 1UL) == 0UL) ? TRUE : FALSE;
}

static AmigaSpriteId probe_mine_sprite_for_fuse_ticks(int fuse_ticks) {
    if (fuse_ticks <= GAME_FIXED_TICK_HZ) {
        return (fuse_ticks & 1) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_2;
    }
    if (fuse_ticks <= 2 * GAME_FIXED_TICK_HZ) {
        return ((fuse_ticks & 2) == 0) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_0;
    }
    return ((fuse_ticks & 7) == 0) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_0;
}

static WORD probe_fp_to_screen(GamePixelFp value, WORD max_value) {
    WORD px = (WORD)(value >> GAME_FP_SHIFT);
    return (WORD)clamp_int_probe(px, 0, max_value);
}

static WORD probe_dir_dx(Direction d) {
    if (d == DIR_LEFT) return -1;
    if (d == DIR_RIGHT) return 1;
    return 0;
}

static WORD probe_dir_dy(Direction d) {
    if (d == DIR_UP) return -1;
    if (d == DIR_DOWN) return 1;
    return 0;
}

static BOOL probe_impact_style_is_stop(int style) {
    return (style == IMPACT_FX_STYLE_STOP ||
            style == IMPACT_FX_STYLE_SPECIAL_STOP ||
            style == IMPACT_FX_STYLE_CRACKED_STOP) ? TRUE : FALSE;
}

static BOOL probe_impact_style_is_enemy_death(int style) {
    return (style == IMPACT_FX_STYLE_ENEMY_DEATH_A ||
            style == IMPACT_FX_STYLE_ENEMY_DEATH_B ||
            style == IMPACT_FX_STYLE_ENEMY_DEATH_C ||
            style == IMPACT_FX_STYLE_ENEMY_DEATH_D) ? TRUE : FALSE;
}

static AmigaSpriteId probe_death_sprite_for_fx(const ImpactFx *fx) {
    WORD age;
    BOOL late;
    if (!fx) {
        return AMIGA_SPR_ENEMY_A_DEATH_0;
    }
    age = (WORD)(fx->base_ttl_ticks - fx->ttl_ticks);
    late = (age >= (fx->base_ttl_ticks / 2)) ? TRUE : FALSE;
    if (fx->style == IMPACT_FX_STYLE_ENEMY_DEATH_D) {
        return late ? AMIGA_SPR_ENEMY_D_DEATH_1 : AMIGA_SPR_ENEMY_D_DEATH_0;
    }
    if (fx->style == IMPACT_FX_STYLE_ENEMY_DEATH_C) {
        return late ? AMIGA_SPR_ENEMY_C_DEATH_1 : AMIGA_SPR_ENEMY_C_DEATH_0;
    }
    if (fx->style == IMPACT_FX_STYLE_ENEMY_DEATH_B) {
        return late ? AMIGA_SPR_ENEMY_B_DEATH_1 : AMIGA_SPR_ENEMY_B_DEATH_0;
    }
    return late ? AMIGA_SPR_ENEMY_A_DEATH_1 : AMIGA_SPR_ENEMY_A_DEATH_0;
}

static WORD probe_stop_bounce_displacement_px(const ImpactFx *fx) {
    static const WORD kStopBouncePx[12] = {
        0, -1, -2, -2, -2, -2, -2, -1, -1, -1, -1, 0
    };
    WORD age;
    if (!fx || !fx->active || !fx->anchors_block || fx->ttl_ticks <= 0) {
        return 0;
    }
    age = (WORD)(fx->base_ttl_ticks - fx->ttl_ticks);
    if (age < 0) {
        return 0;
    }
    if (fx->base_ttl_ticks == 12 && age < 12) {
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

static void probe_update_hidden_block_rows(ProbeRenderCache *cache, const GameState *game) {
    uint16_t mask;
    WORD y;
    WORD i;
    UBYTE active = 0;
    if (!cache) {
        return;
    }
    mask = game ? game->active_impact_fx_mask : 0u;
    if (mask == 0u && !cache->hidden_blocks_active) {
        return;
    }
    if (cache->hidden_blocks_active) {
        for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
            cache->hidden_block_rows[y] = 0u;
        }
    }
    cache->hidden_blocks_active = 0;
    if (!game || mask == 0u) {
        return;
    }
    for (i = 0; mask != 0u && i < GAME_MAX_IMPACT_FX; ++i, mask >>= 1) {
        const ImpactFx *fx = &game->impact_fx[i];
        if ((mask & 1u) == 0 || !fx->active || !fx->anchors_block) {
            continue;
        }
        if (!probe_impact_style_is_stop(fx->style)) {
            continue;
        }
        if (fx->anchor_tile_x >= 0 && fx->anchor_tile_x < GAME_GRID_WIDTH &&
            fx->anchor_tile_y >= 0 && fx->anchor_tile_y < GAME_GRID_HEIGHT) {
            cache->hidden_block_rows[fx->anchor_tile_y] |= ((uint32_t)1u << (uint32_t)fx->anchor_tile_x);
            active = 1;
        }
    }
    cache->hidden_blocks_active = active;
}

static BOOL probe_hidden_block_row_test(const uint32_t *rows, WORD tx, WORD ty) {
    if (!rows || tx < 0 || ty < 0 || tx >= GAME_GRID_WIDTH || ty >= GAME_GRID_HEIGHT) {
        return FALSE;
    }
    return ((rows[ty] & ((uint32_t)1u << (uint32_t)tx)) != 0u) ? TRUE : FALSE;
}

static void probe_mark_all_dirty(ProbeRenderCache *cache) {
    WORD b;
    WORD y;
    if (!cache) {
        return;
    }
    for (b = 0; b < PROBE_DISPLAY_BUFFER_COUNT; ++b) {
        cache->valid[b] = 0;
        cache->hud_valid[b] = 0;
        cache->prev_dynamic_lo[b] = 0u;
        cache->prev_dynamic_hi[b] = 0u;
        for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
            cache->dirty_rows[b][y] = (uint32_t)((1UL << GAME_GRID_WIDTH) - 1UL);
        }
    }
}

static void probe_reset_render_cache(ProbeRenderCache *cache) {
    if (!cache) {
        return;
    }
    memset(cache, 0, sizeof(*cache));
    probe_mark_all_dirty(cache);
}

static void probe_accumulate_core_dirty(ProbeRenderCache *cache, GameState *game) {
    const uint32_t *rows;
    WORD y;
    UBYTE any = 0;
    if (!cache || !game) {
        return;
    }
    rows = game_dirty_static_rows(game);
    if (!rows) {
        return;
    }
    for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
        uint32_t row = rows[y];
        if (row != 0u) {
            cache->dirty_rows[0][y] |= row;
            cache->dirty_rows[1][y] |= row;
            any = 1;
        }
    }
    if (any) {
        game_clear_dirty_static(game);
    }
}

static void draw_probe_static_cell_at_async(UBYTE *screen,
                                            const UBYTE *chip_tiles,
                                            const UBYTE *chip_bobs,
                                            const UBYTE *chip_masks,
                                            const GameState *game,
                                            const uint32_t *hidden_block_rows,
                                            WORD x,
                                            WORD y,
                                            WORD dst_y) {
    AmigaSpriteId terrain;
    BlockType block;
    ItemType item;
    if (!game || x < 0 || y < 0 || x >= GAME_GRID_WIDTH || y >= GAME_GRID_HEIGHT) {
        return;
    }
    terrain = kProbeTerrainSprites[(WORD)game->terrain[y][x]];
    block = game->blocks[y][x];
    if (block != BLOCK_NONE && probe_hidden_block_row_test(hidden_block_rows, x, y)) {
        block = BLOCK_NONE;
    }
    item = game->items[y][x];
    if (block != BLOCK_NONE) {
        blit_opaque_tile_probe_async(screen, chip_tiles, kProbeBlockSprites[(WORD)block],
                                     (int)x * GAME_TILE_SIZE,
                                     dst_y);
    } else {
        blit_opaque_tile_probe_async(screen, chip_tiles, terrain,
                                     (int)x * GAME_TILE_SIZE,
                                     dst_y);
    }
    if (item != ITEM_NONE && game->bonus_item_timer_ticks > 0) {
        blit_shifted_bob_twopass_probe_async(screen, chip_bobs, chip_masks, kProbeItemSprites[(WORD)item],
                                             (WORD)((int)x * GAME_TILE_SIZE),
                                             dst_y);
    }
    if (game->mines[y][x]) {
        blit_shifted_bob_twopass_probe_async(screen,
                                             chip_bobs,
                                             chip_masks,
                                             probe_mine_sprite_for_fuse_ticks((int)game->mine_fuse_ticks[y][x]),
                                             (WORD)((int)x * GAME_TILE_SIZE),
                                             dst_y);
    }
}

static void draw_probe_static_cell_async(UBYTE *screen,
                                         const UBYTE *chip_tiles,
                                         const UBYTE *chip_bobs,
                                         const UBYTE *chip_masks,
                                         const GameState *game,
                                         const uint32_t *hidden_block_rows,
                                         WORD x,
                                         WORD y) {
    draw_probe_static_cell_at_async(screen,
                                    chip_tiles,
                                    chip_bobs,
                                    chip_masks,
                                    game,
                                    hidden_block_rows,
                                    x,
                                    y,
                                    (WORD)(y * GAME_TILE_SIZE));
}

static BOOL probe_dynamic_bob_set(ProbeDynamicBob *bob, WORD px, WORD py, AmigaSpriteId sprite) {
    WORD shift;
    WORD x0;
    WORD tx0;
    WORD tx1;
    WORD ty0;
    WORD ty1;
    if (!bob || py >= 192 || px >= SCREEN_W || py >= SCREEN_H) {
        return FALSE;
    }
    shift = (WORD)(px & (GAME_TILE_SIZE - 1));
    x0 = (shift == 0) ? px : (WORD)(px & ~(GAME_TILE_SIZE - 1));
    tx0 = (WORD)clamp_int_probe(x0 >> 4, 0, GAME_GRID_WIDTH - 1);
    tx1 = (WORD)clamp_int_probe((x0 + ((shift == 0) ? GAME_TILE_SIZE : (GAME_TILE_SIZE * 2)) - 1) >> 4,
                                0,
                                GAME_GRID_WIDTH - 1);
    ty0 = (WORD)clamp_int_probe(py >> 4, 0, GAME_GRID_HEIGHT - 1);
    ty1 = (WORD)clamp_int_probe((py + GAME_TILE_SIZE - 1) >> 4, 0, GAME_GRID_HEIGHT - 1);
    bob->x = px;
    bob->y = py;
    bob->sprite = sprite;
    bob->tile_mask = (uint32_t)((((uint32_t)1u << (uint32_t)(tx1 - tx0 + 1)) - 1u) << (uint32_t)tx0);
    bob->ty0 = (UBYTE)ty0;
    bob->ty1 = (UBYTE)ty1;
    bob->valid = 1;
    return TRUE;
}

static void probe_dynamic_slot_set(WORD slot, uint32_t *lo, UBYTE *hi) {
    if (slot < 32) {
        if (lo) {
            *lo |= ((uint32_t)1u << (uint32_t)slot);
        }
    } else if (hi) {
        *hi |= (UBYTE)(1u << (UBYTE)(slot - 32));
    }
}

static void probe_mark_dynamic_bob_tiles_dirty(uint32_t *rows, const ProbeDynamicBob *bob) {
    WORD ty;
    if (!rows || !bob || !bob->valid) {
        return;
    }
    for (ty = (WORD)bob->ty0; ty <= (WORD)bob->ty1; ++ty) {
        rows[ty] |= bob->tile_mask;
    }
}

static BOOL probe_bob_overlaps_dirty_tiles(const uint32_t *rows, const ProbeDynamicBob *bob) {
    WORD ty;
    if (!rows || !bob || !bob->valid) {
        return FALSE;
    }
    for (ty = (WORD)bob->ty0; ty <= (WORD)bob->ty1; ++ty) {
        if ((rows[ty] & bob->tile_mask) != 0u) {
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL probe_dynamic_bob_same(const ProbeDynamicBob *a, const ProbeDynamicBob *b) {
    if (!a || !b) {
        return FALSE;
    }
    return (a->valid == b->valid &&
            a->x == b->x &&
            a->y == b->y &&
            a->sprite == b->sprite) ? TRUE : FALSE;
}

static void probe_redraw_dirty_rows(uint32_t *rows,
                                    UBYTE *screen,
                                    const UBYTE *chip_tiles,
                                    const UBYTE *chip_bobs,
                                    const UBYTE *chip_masks,
                                    const GameState *game,
                                    const uint32_t *hidden_block_rows) {
    WORD y;
    for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
        uint32_t row = rows[y];
        WORD x = 0;
        if (row == 0u) {
            continue;
        }
        while (row != 0u && x < GAME_GRID_WIDTH) {
            while ((row & 1u) == 0u) {
                row >>= 1;
                ++x;
            }
            if (x >= GAME_GRID_WIDTH) {
                break;
            }
            draw_probe_static_cell_async(screen, chip_tiles, chip_bobs, chip_masks, game, hidden_block_rows, x, y);
            row >>= 1;
            ++x;
        }
    }
}

static void probe_fill_all_gameplay_rows(uint32_t *rows) {
    WORD y;
    if (!rows) {
        return;
    }
    for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
        rows[y] = (uint32_t)((1UL << GAME_GRID_WIDTH) - 1UL);
    }
}

static void probe_collect_dynamic_bobs(const GameState *game,
                                       ProbeDynamicBob *out,
                                       ULONG tick,
                                       uint32_t *out_lo,
                                       UBYTE *out_hi) {
    WORD i;
    WORD slot;
    const MovingBlock *mb;
    const Enemy *enemy;
    const ImpactFx *fx;
    UWORD mask;
    BOOL moving;
    BOOL alt;
    if (!game || !out) {
        return;
    }
    if (out_lo) {
        *out_lo = 0u;
    }
    if (out_hi) {
        *out_hi = 0u;
    }
    mask = (UWORD)game->active_moving_block_mask;
    for (i = 0; mask != 0u && i < GAME_MAX_MOVING_BLOCKS; ++i, mask >>= 1) {
        if ((mask & 1u) == 0u) {
            continue;
        }
        mb = &game->moving_blocks[i];
        if (mb->active && mb->type != BLOCK_NONE) {
            slot = (WORD)(PROBE_DYNAMIC_SLOT_MOVING_BLOCK + i);
            if (probe_dynamic_bob_set(&out[slot],
                                      probe_fp_to_screen(mb->pixel_fp_x, SCREEN_W - 32),
                                      probe_fp_to_screen(mb->pixel_fp_y, SCREEN_H - GAME_TILE_SIZE),
                                      kProbeBlockSprites[(WORD)mb->type])) {
                probe_dynamic_slot_set(slot, out_lo, out_hi);
            }
        }
    }
    mask = (game->phase == GAME_PHASE_PLAYING && game->round_clear_pending_ticks > 0) ?
        0u : (UWORD)game->active_impact_fx_mask;
    for (i = 0; mask != 0u && i < GAME_MAX_IMPACT_FX; ++i, mask >>= 1) {
        if ((mask & 1u) == 0u) {
            continue;
        }
        fx = &game->impact_fx[i];
        slot = (WORD)(PROBE_DYNAMIC_SLOT_IMPACT + i);
        if (fx->active && fx->ttl_ticks > 0 && probe_impact_style_is_enemy_death(fx->style)) {
            WORD age = (WORD)(fx->base_ttl_ticks - fx->ttl_ticks);
            if (!(fx->ttl_ticks < 4 && ((fx->ttl_ticks & 1) != 0))) {
                WORD px = (WORD)((fx->pixel_fp_x >> GAME_FP_SHIFT) - 8);
                WORD py = (WORD)((fx->pixel_fp_y >> GAME_FP_SHIFT) - 8 + ((age > 2) ? ((age - 2) / 4) : 0));
                if (probe_dynamic_bob_set(&out[slot],
                                          (WORD)clamp_int_probe(px, 0, SCREEN_W - 32),
                                          (WORD)clamp_int_probe(py, 0, SCREEN_H - GAME_TILE_SIZE),
                                          probe_death_sprite_for_fx(fx))) {
                    probe_dynamic_slot_set(slot, out_lo, out_hi);
                }
            }
            continue;
        }
        if (fx->active && fx->ttl_ticks > 0 && fx->style == IMPACT_FX_STYLE_MINE_BLAST) {
            WORD age = (WORD)(fx->base_ttl_ticks - fx->ttl_ticks);
            WORD jitter = (age & 1) ? 1 : 0;
            WORD px = (WORD)((fx->pixel_fp_x >> GAME_FP_SHIFT) - 8 + jitter);
            WORD py = (WORD)((fx->pixel_fp_y >> GAME_FP_SHIFT) - 8 - jitter);
            if (probe_dynamic_bob_set(&out[slot],
                                      (WORD)clamp_int_probe(px, 0, SCREEN_W - 32),
                                      (WORD)clamp_int_probe(py, 0, SCREEN_H - GAME_TILE_SIZE),
                                      AMIGA_SPR_EFFECT_SPARK)) {
                probe_dynamic_slot_set(slot, out_lo, out_hi);
            }
            continue;
        }
        if (fx->active && fx->anchors_block &&
            probe_impact_style_is_stop(fx->style) &&
            fx->anchor_tile_x >= 0 && fx->anchor_tile_y >= 0 &&
            fx->anchor_tile_x < GAME_GRID_WIDTH && fx->anchor_tile_y < GAME_GRID_HEIGHT &&
            game->blocks[fx->anchor_tile_y][fx->anchor_tile_x] != BLOCK_NONE) {
            WORD disp = probe_stop_bounce_displacement_px(fx);
            BlockType block = game->blocks[fx->anchor_tile_y][fx->anchor_tile_x];
            WORD px = (WORD)((fx->anchor_tile_x * GAME_TILE_SIZE) + (probe_dir_dx(fx->direction) * disp));
            WORD py = (WORD)((fx->anchor_tile_y * GAME_TILE_SIZE) + (probe_dir_dy(fx->direction) * disp));
            if (probe_dynamic_bob_set(&out[slot],
                                      (WORD)clamp_int_probe(px, 0, SCREEN_W - 32),
                                      (WORD)clamp_int_probe(py, 0, SCREEN_H - GAME_TILE_SIZE),
                                      kProbeBlockSprites[(WORD)block])) {
                probe_dynamic_slot_set(slot, out_lo, out_hi);
            }
        }
    }
    mask = (UWORD)game->alive_enemy_mask;
    for (i = 0; mask != 0u && i < game->enemy_count && i < GAME_MAX_ENEMIES; ++i, mask >>= 1) {
        if ((mask & 1u) == 0u) {
            continue;
        }
        enemy = &game->enemies[i];
        if (enemy->alive && probe_enemy_visible(enemy, tick)) {
            slot = (WORD)(PROBE_DYNAMIC_SLOT_ENEMY + i);
            if (probe_dynamic_bob_set(&out[slot],
                                      probe_fp_to_screen(enemy->pixel_fp_x, SCREEN_W - 32),
                                      probe_fp_to_screen(enemy->pixel_fp_y, SCREEN_H - GAME_TILE_SIZE),
                                      probe_enemy_sprite(enemy))) {
                probe_dynamic_slot_set(slot, out_lo, out_hi);
            }
        }
    }
    if (game->player.alive) {
        moving = (game->player.state == PLAYER_WALKING || game->player.state == PLAYER_PUSHING) ? TRUE : FALSE;
        alt = (moving && ((game->player.anim_frame & 1) != 0)) ? TRUE : FALSE;
        slot = PROBE_DYNAMIC_SLOT_PLAYER;
        if (probe_dynamic_bob_set(&out[slot],
                                  probe_fp_to_screen(game->player.pixel_fp_x, SCREEN_W - 32),
                                  probe_fp_to_screen(game->player.pixel_fp_y, SCREEN_H - GAME_TILE_SIZE),
                                  probe_player_sprite(game->player.facing, alt))) {
            probe_dynamic_slot_set(slot, out_lo, out_hi);
        }
    }
}

static void probe_draw_dynamic_bobs_from_set(UBYTE *screen,
                                             const UBYTE *chip_bobs,
                                             const UBYTE *chip_masks,
                                             const ProbeDynamicBob *current,
                                             uint32_t current_lo,
                                             UBYTE current_hi,
                                             const uint32_t *only_rows) {
    uint32_t scan_lo;
    UBYTE scan_hi;
    WORD slot;
    if (!screen || !chip_bobs || !chip_masks || !current) {
        return;
    }
    scan_lo = current_lo;
    for (slot = 0; scan_lo != 0u && slot < 32; ++slot, scan_lo >>= 1) {
        if ((scan_lo & 1u) != 0u &&
            (!only_rows || probe_bob_overlaps_dirty_tiles(only_rows, &current[slot]))) {
            blit_shifted_bob_twopass_probe_async(screen, chip_bobs, chip_masks,
                                                 current[slot].sprite, current[slot].x, current[slot].y);
        }
    }
    scan_hi = current_hi;
    for (slot = 32; scan_hi != 0u && slot < PROBE_DYNAMIC_CACHE_MAX; ++slot, scan_hi >>= 1) {
        if ((scan_hi & 1u) != 0u &&
            (!only_rows || probe_bob_overlaps_dirty_tiles(only_rows, &current[slot]))) {
            blit_shifted_bob_twopass_probe_async(screen, chip_bobs, chip_masks,
                                                 current[slot].sprite, current[slot].x, current[slot].y);
        }
    }
}

static void draw_probe_gameplay_direct_full(ProbeRenderCache *cache,
                                            UBYTE *screen,
                                            const UBYTE *chip_tiles,
                                            const UBYTE *chip_bobs,
                                            const UBYTE *chip_masks,
                                            const GameState *game,
                                            ULONG tick,
                                            ProbeDynamicBob *current,
                                            uint32_t *current_lo,
                                            UBYTE *current_hi) {
    uint32_t all_rows[GAME_GRID_HEIGHT];
    probe_update_hidden_block_rows(cache, game);
    probe_fill_all_gameplay_rows(all_rows);
    probe_redraw_dirty_rows(all_rows, screen, chip_tiles, chip_bobs, chip_masks, game,
                            cache ? cache->hidden_block_rows : 0);
    probe_collect_dynamic_bobs(game, current, tick, current_lo, current_hi);
    probe_draw_dynamic_bobs_from_set(screen, chip_bobs, chip_masks, current, *current_lo, *current_hi, 0);
}

static void draw_probe_gameplay_dirty(ProbeRenderCache *cache,
                                      WORD buf,
                                      UBYTE *screen,
                                      const UBYTE *chip_tiles,
                                      const UBYTE *chip_bobs,
                                      const UBYTE *chip_masks,
                                      const GameState *game,
                                      ULONG tick) {
    ProbeDynamicBob current[PROBE_DYNAMIC_CACHE_MAX];
    uint32_t dirty_rows[GAME_GRID_HEIGHT];
    uint32_t draw_rows[GAME_GRID_HEIGHT];
    uint32_t current_lo;
    uint32_t scan_lo;
    UBYTE current_hi;
    UBYTE scan_hi;
    UBYTE hidden_was_active;
    UBYTE hidden_sensitive;
    UBYTE dirty_any;
    UBYTE draw_any;
    UWORD i;
    WORD slot;
    WORD y;
    if (!game) {
        return;
    }
    if (!cache || buf < 0 || buf >= PROBE_DISPLAY_BUFFER_COUNT || !cache->valid[buf]) {
        draw_probe_gameplay_direct_full(cache, screen, chip_tiles, chip_bobs, chip_masks,
                                       game, tick, current, &current_lo, &current_hi);
        if (cache && buf >= 0 && buf < PROBE_DISPLAY_BUFFER_COUNT) {
            scan_lo = current_lo;
            for (slot = 0; scan_lo != 0u && slot < 32; ++slot, scan_lo >>= 1) {
                if ((scan_lo & 1u) != 0u) {
                    cache->prev[buf][slot] = current[slot];
                }
            }
            scan_hi = current_hi;
            for (slot = 32; scan_hi != 0u && slot < PROBE_DYNAMIC_CACHE_MAX; ++slot, scan_hi >>= 1) {
                if ((scan_hi & 1u) != 0u) {
                    cache->prev[buf][slot] = current[slot];
                }
            }
            cache->prev_dynamic_lo[buf] = current_lo;
            cache->prev_dynamic_hi[buf] = current_hi;
            cache->valid[buf] = 1;
            for (i = 0; i < GAME_GRID_HEIGHT; ++i) {
                cache->dirty_rows[buf][i] = 0;
            }
        }
        return;
    }

    probe_collect_dynamic_bobs(game, current, tick, &current_lo, &current_hi);
    hidden_was_active = cache->hidden_blocks_active;
    dirty_any = 0;
    draw_any = 0;
    for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
        dirty_rows[y] = cache->dirty_rows[buf][y];
        draw_rows[y] = dirty_rows[y];
        cache->dirty_rows[buf][y] = 0;
    }

    scan_lo = cache->prev_dynamic_lo[buf] | current_lo;
    {
    uint32_t prev_scan = cache->prev_dynamic_lo[buf];
    uint32_t current_scan = current_lo;
    for (slot = 0; scan_lo != 0u && slot < 32; ++slot, scan_lo >>= 1, prev_scan >>= 1, current_scan >>= 1) {
        BOOL prev_valid;
        BOOL current_valid;
        if ((scan_lo & 1u) == 0u) {
            continue;
        }
        prev_valid = ((prev_scan & 1u) != 0u) ? TRUE : FALSE;
        current_valid = ((current_scan & 1u) != 0u) ? TRUE : FALSE;
        if (!prev_valid && !current_valid) {
            continue;
        }
        if (!prev_valid || !current_valid ||
            !probe_dynamic_bob_same(&cache->prev[buf][slot], &current[slot])) {
            if (prev_valid) {
                probe_mark_dynamic_bob_tiles_dirty(dirty_rows, &cache->prev[buf][slot]);
            }
            if (current_valid) {
                probe_mark_dynamic_bob_tiles_dirty(draw_rows, &current[slot]);
            }
        }
    }
    }
    scan_hi = (UBYTE)(cache->prev_dynamic_hi[buf] | current_hi);
    {
    UBYTE prev_scan = cache->prev_dynamic_hi[buf];
    UBYTE current_scan = current_hi;
    for (slot = 32; scan_hi != 0u && slot < PROBE_DYNAMIC_CACHE_MAX; ++slot, scan_hi >>= 1, prev_scan >>= 1, current_scan >>= 1) {
        BOOL prev_valid;
        BOOL current_valid;
        if ((scan_hi & 1u) == 0u) {
            continue;
        }
        prev_valid = ((prev_scan & 1u) != 0u) ? TRUE : FALSE;
        current_valid = ((current_scan & 1u) != 0u) ? TRUE : FALSE;
        if (!prev_valid && !current_valid) {
            continue;
        }
        if (!prev_valid || !current_valid ||
            !probe_dynamic_bob_same(&cache->prev[buf][slot], &current[slot])) {
            if (prev_valid) {
                probe_mark_dynamic_bob_tiles_dirty(dirty_rows, &cache->prev[buf][slot]);
            }
            if (current_valid) {
                probe_mark_dynamic_bob_tiles_dirty(draw_rows, &current[slot]);
            }
        }
    }
    }
    for (y = 0; y < GAME_GRID_HEIGHT; ++y) {
        if (dirty_rows[y] != 0u) {
            dirty_any = 1;
        }
        draw_rows[y] |= dirty_rows[y];
        if (draw_rows[y] != 0u) {
            draw_any = 1;
        }
    }
    probe_update_hidden_block_rows(cache, game);
    hidden_sensitive = (hidden_was_active || cache->hidden_blocks_active) ? 1u : 0u;
    if (hidden_sensitive) {
        if (draw_any) {
            probe_redraw_dirty_rows(draw_rows, screen, chip_tiles, chip_bobs, chip_masks, game, cache->hidden_block_rows);
        }
    } else if (dirty_any) {
        probe_redraw_dirty_rows(dirty_rows, screen, chip_tiles, chip_bobs, chip_masks, game, cache->hidden_block_rows);
    }
    if (draw_any) {
        probe_draw_dynamic_bobs_from_set(screen, chip_bobs, chip_masks, current, current_lo, current_hi, draw_rows);
    }
    scan_lo = current_lo;
    for (slot = 0; scan_lo != 0u && slot < 32; ++slot, scan_lo >>= 1) {
        if ((scan_lo & 1u) != 0u) {
            cache->prev[buf][slot] = current[slot];
        }
    }
    scan_hi = current_hi;
    for (slot = 32; scan_hi != 0u && slot < PROBE_DYNAMIC_CACHE_MAX; ++slot, scan_hi >>= 1) {
        if ((scan_hi & 1u) != 0u) {
            cache->prev[buf][slot] = current[slot];
        }
    }
    cache->prev_dynamic_lo[buf] = current_lo;
    cache->prev_dynamic_hi[buf] = current_hi;
}

static BOOL joystick_fire_down_probe(void) {
    volatile UBYTE *ciaa_pra = (volatile UBYTE *)0xBFE001;
    UBYTE pra = *ciaa_pra;
    return ((pra & PRA_FIR1_BIT) == 0) ? TRUE : FALSE;
}

static UBYTE dirs_from_joydat_probe(UWORD joy) {
    UBYTE dirs = 0;
    BOOL left = ((joy & 0x0200u) != 0) ? TRUE : FALSE;
    BOOL right = ((joy & 0x0002u) != 0) ? TRUE : FALSE;
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

static UBYTE probe_filter_opposite_dirs(UBYTE dirs) {
    if ((dirs & 3u) == 3u) {
        dirs &= (UBYTE)~3u;
    }
    if ((dirs & 12u) == 12u) {
        dirs &= (UBYTE)~12u;
    }
    return (UBYTE)(dirs & 0x0Fu);
}

static InputState read_input_probe(UBYTE *last_dirs, UBYTE *last_fire) {
    InputState in;
    UBYTE dirs;
    UBYTE fire;
    memset(&in, 0, sizeof(in));
    dirs = probe_filter_opposite_dirs(dirs_from_joydat_probe(CREGS->joy1dat));
    in.left = ((dirs & 1u) != 0);
    in.right = ((dirs & 2u) != 0);
    in.up = ((dirs & 4u) != 0);
    in.down = ((dirs & 8u) != 0);
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
    fire = joystick_fire_down_probe() ? 1u : 0u;
    in.start = FALSE;
    in.fire_pressed = (fire != 0 && *last_fire == 0);
    in.fire_released = (fire == 0 && *last_fire != 0);
    *last_fire = fire;
    return in;
}

static void probe_clear_input_latch(InputState *latch) {
    if (latch) {
        memset(latch, 0, sizeof(*latch));
        latch->newest_press = DIR_NONE;
    }
}

static void probe_poll_input_latch(InputState *latch, UBYTE *last_dirs, UBYTE *last_fire) {
    InputState sample;
    if (!latch || !last_dirs || !last_fire) {
        return;
    }
    sample = read_input_probe(last_dirs, last_fire);
    latch->left = sample.left;
    latch->right = sample.right;
    latch->up = sample.up;
    latch->down = sample.down;
    if (sample.newest_press != DIR_NONE) {
        latch->newest_press = sample.newest_press;
    }
    if (sample.fire_pressed) {
        latch->fire_pressed = TRUE;
    }
    if (sample.fire_released) {
        latch->fire_released = TRUE;
    }
}

static void wait_next_frame_start_probe(InputState *latch, UBYTE *last_dirs, UBYTE *last_fire) {
    while (read_raster_line_probe() < 250u) {
        probe_poll_input_latch(latch, last_dirs, last_fire);
    }
    while (read_raster_line_probe() >= 250u) {
        probe_poll_input_latch(latch, last_dirs, last_fire);
    }
}

static Direction probe_held_menu_direction(const InputState *in) {
    if (in->left) return DIR_LEFT;
    if (in->right) return DIR_RIGHT;
    if (in->up) return DIR_UP;
    if (in->down) return DIR_DOWN;
    return DIR_NONE;
}

static void probe_apply_menu_input_repeat(const GameState *game,
                                          InputState *in,
                                          Direction *repeat_dir,
                                          WORD *repeat_ticks) {
    Direction held;
    BOOL repeat_enabled;
    if (!game || !in || !repeat_dir || !repeat_ticks) {
        return;
    }
    repeat_enabled = (g_probe_high_score_entry.active ||
                      game->phase == GAME_PHASE_PERK_CHOICE ||
                      game->phase == GAME_PHASE_META_UPGRADE) ? TRUE : FALSE;
    if (!repeat_enabled) {
        *repeat_dir = DIR_NONE;
        *repeat_ticks = 0;
        return;
    }
    held = probe_held_menu_direction(in);
    if (held == DIR_NONE) {
        *repeat_dir = DIR_NONE;
        *repeat_ticks = 0;
        return;
    }
    if (in->newest_press != DIR_NONE) {
        *repeat_dir = in->newest_press;
        *repeat_ticks = g_probe_high_score_entry.active ?
            PROBE_HI_SCORE_REPEAT_INITIAL_TICKS : PROBE_MENU_REPEAT_INITIAL_TICKS;
        return;
    }
    if (*repeat_dir != held) {
        *repeat_dir = held;
        *repeat_ticks = g_probe_high_score_entry.active ?
            PROBE_HI_SCORE_REPEAT_INITIAL_TICKS : PROBE_MENU_REPEAT_INITIAL_TICKS;
        return;
    }
    if (*repeat_ticks > 0) {
        --(*repeat_ticks);
        return;
    }
    in->newest_press = held;
    *repeat_ticks = g_probe_high_score_entry.active ?
        PROBE_HI_SCORE_REPEAT_NEXT_TICKS : PROBE_MENU_REPEAT_NEXT_TICKS;
}

static ULONG startup_seed_probe(void) {
    ULONG seed = 0x1CE091UL;
    seed ^= ((ULONG)CREGS->joy1dat << 8);
    seed ^= ((ULONG)CREGS->vposr << 16);
    return seed;
}

static void draw_probe_scene(ProbeRenderCache *cache,
                             WORD buf,
                             UBYTE *screen,
                             WORD mirror_buf,
                             const UBYTE *mirror_screen,
                             const UBYTE *chip_tiles,
                             const UBYTE *chip_bobs,
                             const UBYTE *chip_masks,
                             const UBYTE *chip_title,
                             const UBYTE *chip_title_mask,
                             UBYTE *title_cell_tile,
                             UBYTE *title_cell_mask,
                             const GameState *game,
                             WORD ui_mode,
                             WORD tick) {
    BOOL selection_delta = FALSE;
    WORD previous_selection = -1;
    if (ui_mode >= 0) {
        BOOL rebuild = TRUE;
        BOOL panel_only = FALSE;
        BOOL target_same_layout = FALSE;
        ULONG ui_layout_key = probe_ui_layout_key_for_game(game, ui_mode);
        WORD ui_selection = probe_ui_selection_for_game(game);
        if (cache && buf >= 0 && buf < PROBE_DISPLAY_BUFFER_COUNT &&
            cache->ui_valid[buf] &&
            cache->ui_mode[buf] == ui_mode &&
            cache->ui_layout_key[buf] == ui_layout_key) {
            target_same_layout = TRUE;
            if (cache->ui_selection[buf] == ui_selection) {
                rebuild = FALSE;
            }
        }
        if (rebuild && target_same_layout && ui_mode != PROBE_UI_TITLE_SCORES) {
            previous_selection = cache->ui_selection[buf];
            panel_only = TRUE;
            if (cache->ui_selection[buf] != ui_selection && probe_ui_selection_delta_supported(game)) {
                selection_delta = TRUE;
            }
        }
        if (rebuild && cache &&
            mirror_screen &&
            mirror_buf >= 0 &&
            mirror_buf < PROBE_DISPLAY_BUFFER_COUNT &&
            mirror_buf != buf &&
            cache->ui_valid[mirror_buf] &&
            cache->ui_mode[mirror_buf] == ui_mode &&
            cache->ui_layout_key[mirror_buf] == ui_layout_key &&
            cache->ui_selection[mirror_buf] == ui_selection) {
            WORD band_y0;
            WORD band_y1;
            if (target_same_layout && probe_ui_band_for_game(game, ui_mode, &band_y0, &band_y1)) {
                blit_screen_band_copy_probe(screen, mirror_screen, band_y0, band_y1, FALSE);
                blit_screen_band_copy_probe(screen, mirror_screen, 192, (WORD)(SCREEN_H - 1), FALSE);
                cache->hud[buf] = cache->hud[mirror_buf];
                cache->hud_raw_ticks[buf] = cache->hud_raw_ticks[mirror_buf];
                cache->hud_next_second_tick[buf] = cache->hud_next_second_tick[mirror_buf];
                cache->hud_valid[buf] = cache->hud_valid[mirror_buf];
            } else {
                blit_full_screen_copy_probe(screen, mirror_screen, FALSE);
                cache->valid[buf] = cache->valid[mirror_buf];
                cache->prev_dynamic_lo[buf] = cache->prev_dynamic_lo[mirror_buf];
                cache->prev_dynamic_hi[buf] = cache->prev_dynamic_hi[mirror_buf];
                cache->hud[buf] = cache->hud[mirror_buf];
                cache->hud_raw_ticks[buf] = cache->hud_raw_ticks[mirror_buf];
                cache->hud_next_second_tick[buf] = cache->hud_next_second_tick[mirror_buf];
                cache->hud_valid[buf] = cache->hud_valid[mirror_buf];
            }
            cache->ui_valid[buf] = 1;
            cache->ui_mode[buf] = ui_mode;
            cache->ui_layout_key[buf] = ui_layout_key;
            cache->ui_selection[buf] = ui_selection;
            return;
        }
        if (rebuild) {
            if (!panel_only) {
                BOOL can_keep_gameplay = (cache &&
                                          buf >= 0 &&
                                          buf < PROBE_DISPLAY_BUFFER_COUNT &&
                                          cache->valid[buf] &&
                                          !cache->ui_valid[buf]) ? TRUE : FALSE;
                if (!can_keep_gameplay) {
                    UWORD i;
                    ProbeDynamicBob current[PROBE_DYNAMIC_CACHE_MAX];
                    uint32_t current_lo = 0u;
                    UBYTE current_hi = 0u;
                    draw_probe_gameplay_direct_full(cache, screen, chip_tiles, chip_bobs, chip_masks,
                                                   game, (ULONG)tick, current, &current_lo, &current_hi);
                    if (cache && buf >= 0 && buf < PROBE_DISPLAY_BUFFER_COUNT) {
                        WORD slot;
                        for (slot = 0; slot < PROBE_DYNAMIC_CACHE_MAX; ++slot) {
                            cache->prev[buf][slot] = current[slot];
                        }
                        cache->prev_dynamic_lo[buf] = current_lo;
                        cache->prev_dynamic_hi[buf] = current_hi;
                        cache->valid[buf] = 1;
                        cache->hud_valid[buf] = 0;
                        for (i = 0; i < GAME_GRID_HEIGHT; ++i) {
                            cache->dirty_rows[buf][i] = 0;
                        }
                    }
                }
            }
            if (cache && buf >= 0 && buf < PROBE_DISPLAY_BUFFER_COUNT) {
                cache->ui_valid[buf] = 1;
                cache->ui_mode[buf] = ui_mode;
                cache->ui_layout_key[buf] = ui_layout_key;
                cache->ui_selection[buf] = ui_selection;
            }
        }
        if (!rebuild) {
            return;
        }
    } else {
        if (cache && buf >= 0 && buf < PROBE_DISPLAY_BUFFER_COUNT) {
            if (cache->ui_valid[buf]) {
                cache->valid[buf] = 0;
                cache->hud_valid[buf] = 0;
                cache->prev_dynamic_lo[buf] = 0u;
                cache->prev_dynamic_hi[buf] = 0u;
            }
            cache->ui_valid[buf] = 0;
            cache->ui_layout_key[buf] = 0UL;
            cache->ui_selection[buf] = -1;
        }
        draw_probe_gameplay_dirty(cache, buf, screen, chip_tiles, chip_bobs, chip_masks, game, (ULONG)tick);
    }
    if (probe_ui_uses_title(ui_mode)) {
        WORD title_x = (WORD)(96 + AMIGA_TITLE_OVERLAY_ANCHOR_X);
        WORD title_y = (WORD)(10 + AMIGA_TITLE_OVERLAY_ANCHOR_Y);
        blit_title_overlay_probe(screen, chip_title, chip_title_mask,
                                 title_cell_tile, title_cell_mask,
                                 title_x,
                                 title_y);
        wait_blitter();
    }
    if (ui_mode >= 0) {
        BOOL drew_delta = FALSE;
        if (cache && buf >= 0 && buf < PROBE_DISPLAY_BUFFER_COUNT &&
            selection_delta &&
            (previous_selection >= 0 || g_probe_high_score_entry.active)) {
            drew_delta = draw_probe_ui_delta(screen, game, previous_selection);
        }
        if (!drew_delta) {
            draw_probe_ui_panel(screen, chip_bobs, chip_masks, game, ui_mode, (ULONG)tick);
        }
    }
    draw_probe_hud_cached(cache, buf, screen, game);
}

int main(void) {
    struct View *old_view;
    struct Task *this_task;
    UWORD *cop[PROBE_COPPER_COUNT];
    UBYTE *screen[PROBE_DISPLAY_BUFFER_COUNT];
    UBYTE *dma_arena;
    UBYTE *screen_pool;
    UBYTE *control_pool;
    UBYTE *audio_pool;
    UBYTE *sprite_bank;
    UBYTE *chip_asset_block;
    UBYTE *asset_end;
    UBYTE *screen_base;
    UBYTE *chip_tiles;
    UBYTE *chip_tile_masks;
    UBYTE *chip_bobs;
    UBYTE *chip_masks;
    UBYTE *title_source;
    UBYTE *chip_title;
    UBYTE *chip_title_mask;
    UBYTE *title_cell_tile;
    UBYTE *title_cell_mask;
    UBYTE *parked_pools[PROBE_MAX_PARKED_POOLS];
    ULONG parked_pool_sizes[PROBE_MAX_PARKED_POOLS];
    ProbeRenderCache render_cache;
    ProbeSfxState sfx;
    GameState *game;
    InputState input_latch;
    UBYTE last_dirs;
    UBYTE last_fire;
    UBYTE copper_phase[PROBE_COPPER_COUNT];
    UBYTE reflection_enabled[PROBE_COPPER_COUNT];
    UBYTE copper_anim_phase;
    WORD sprite_ptr_mode[PROBE_COPPER_COUNT];
    Direction menu_repeat_dir;
    WORD menu_repeat_ticks;
    WORD parked_pool_count;
    WORD p;
    WORD ui_mode;
    WORD front_index;
    WORD draw_index;
    BOOL title_jingle_queued;
    BOOL pending_post_game_over_resolution;
    BOOL sprite_dma_enabled;
    BOOL owns_blitter;
    BYTE old_task_pri;
    WORD pending_post_game_over_insert_index;
    WORD post_high_score_pause_ticks;
    ULONG tick;
    uint32_t pending_post_game_over_score;
    uint32_t boot_audio_events;

    GfxBase = 0;
    dma_arena = 0;
    audio_pool = 0;
    title_source = 0;
    game = 0;
    this_task = 0;
    old_task_pri = 0;
    owns_blitter = FALSE;
    probe_reset_render_cache(&render_cache);
    memset(&sfx, 0, sizeof(sfx));
    probe_clear_input_latch(&input_latch);
    parked_pool_count = 0;
    for (p = 0; p < PROBE_MAX_PARKED_POOLS; ++p) {
        parked_pools[p] = 0;
        parked_pool_sizes[p] = 0;
    }
    for (p = 0; p < PROBE_DISPLAY_BUFFER_COUNT; ++p) {
        screen[p] = 0;
    }
    for (p = 0; p < PROBE_COPPER_COUNT; ++p) {
        cop[p] = 0;
        copper_phase[p] = 0xFFu;
        reflection_enabled[p] = 0xFFu;
        sprite_ptr_mode[p] = -1;
    }
    copper_anim_phase = 0xFFu;

    dma_arena = alloc_ocs_dma_pool_probe(PROBE_DMA_ARENA_BYTES, parked_pools, parked_pool_sizes, &parked_pool_count);
    free_parked_pools_probe(parked_pools, parked_pool_sizes, &parked_pool_count);
    if (!dma_arena) {
        return 21;
    }

    chip_asset_block = dma_arena;
    control_pool = chip_asset_block + PROBE_ASSET_BYTES;
    screen_pool = control_pool + PROBE_CONTROL_POOL_BYTES;
    asset_end = chip_asset_block + PROBE_ASSET_BYTES;
    chip_masks = chip_asset_block;
    chip_bobs = chip_asset_block + PROBE_MASK_BYTES;
    chip_tile_masks = chip_bobs + PROBE_BOB_BYTES;
    chip_tiles = chip_tile_masks + PROBE_TILE_MASK_BYTES;
    title_cell_tile = chip_tiles + PROBE_TILE_BYTES;
    title_cell_mask = title_cell_tile + PROBE_TITLE_CELL_TILE_BYTES;
    chip_title = 0;
    chip_title_mask = 0;

    sprite_bank = control_pool + PROBE_CONTROL_SPRITE_BANK_OFFSET;
    for (p = 0; p < PROBE_COPPER_COUNT; ++p) {
        cop[p] = (UWORD *)(control_pool + PROBE_CONTROL_COPPER1_OFFSET + ((ULONG)p * PROBE_COPPER_BYTES));
    }

    screen_base = screen_pool;
    for (p = 0; p < PROBE_DISPLAY_BUFFER_COUNT; ++p) {
        screen[p] = screen_base + ((ULONG)p * SCREEN_ALIGNED_BYTES);
    }

    if (title_cell_mask + PROBE_TITLE_CELL_TILE_MASK_BYTES != asset_end) {
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        return 21;
    }
    if (!ocs_dma_range_safe(dma_arena, PROBE_DMA_ARENA_BYTES) ||
        !ocs_dma_range_safe(chip_asset_block, PROBE_ASSET_BYTES)) {
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        return 22;
    }
    if (!ocs_dma_range_safe(screen[0], SCREEN_ALIGNED_BYTES) ||
        !ocs_dma_range_safe(screen[1], SCREEN_ALIGNED_BYTES)) {
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        return 24;
    }
    if (screen[0] + SCREEN_ALIGNED_BYTES > screen[1] ||
        screen[1] + SCREEN_ALIGNED_BYTES > screen_pool + SCREEN_POOL_BYTES) {
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        return 27;
    }
    if (!ocs_dma_range_safe(control_pool, PROBE_CONTROL_POOL_BYTES) ||
        !ocs_dma_range_safe(cop[0], PROBE_COPPER_BYTES) ||
        !ocs_dma_range_safe(cop[1], PROBE_COPPER_BYTES)) {
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        return 26;
    }
    audio_pool = alloc_ocs_dma_pool_probe(PROBE_AUDIO_BYTES, parked_pools, parked_pool_sizes, &parked_pool_count);
    free_parked_pools_probe(parked_pools, parked_pool_sizes, &parked_pool_count);
    if (!audio_pool ||
        !ocs_dma_range_safe(audio_pool, PROBE_AUDIO_BYTES) ||
        (((ULONG)audio_pool & 1UL) != 0)) {
        if (audio_pool) FreeMem(audio_pool, PROBE_AUDIO_BYTES);
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        return 30;
    }
    title_source = (UBYTE *)AllocMem(AMIGA_TITLE_OVERLAY_BYTES + AMIGA_TITLE_OVERLAY_MASK_BYTES,
                                     ((ULONG)MEMF_PUBLIC) | PROBE_MEMF_CLEAR);
    if (!title_source) {
        FreeMem(audio_pool, PROBE_AUDIO_BYTES);
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        return 31;
    }
    chip_title = title_source;
    chip_title_mask = title_source + AMIGA_TITLE_OVERLAY_BYTES;
    game = (GameState *)AllocMem(sizeof(GameState), ((ULONG)MEMF_PUBLIC) | PROBE_MEMF_CLEAR);
    if (!game) {
        FreeMem(title_source, AMIGA_TITLE_OVERLAY_BYTES + AMIGA_TITLE_OVERLAY_MASK_BYTES);
        FreeMem(audio_pool, PROBE_AUDIO_BYTES);
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        return 29;
    }
    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0);
    if (!GfxBase) {
        FreeMem(game, sizeof(GameState));
        FreeMem(title_source, AMIGA_TITLE_OVERLAY_BYTES + AMIGA_TITLE_OVERLAY_MASK_BYTES);
        FreeMem(audio_pool, PROBE_AUDIO_BYTES);
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        return 20;
    }

    CREGS->dmacon = DMAF_SETCLR | DMAF_MASTER | DMAF_BLITTER;
    if (!probe_load_gfx_bank(chip_masks, chip_bobs, chip_tile_masks, chip_tiles, chip_title, chip_title_mask)) {
        FreeMem(game, sizeof(GameState));
        FreeMem(title_source, AMIGA_TITLE_OVERLAY_BYTES + AMIGA_TITLE_OVERLAY_MASK_BYTES);
        FreeMem(audio_pool, PROBE_AUDIO_BYTES);
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        CloseLibrary((struct Library *)GfxBase);
        return 31;
    }
    build_probe_hw_sprite_bank(sprite_bank, chip_tiles);
    if (!probe_load_sfx_bank(audio_pool) || !probe_sfx_init(&sfx, audio_pool)) {
        FreeMem(game, sizeof(GameState));
        FreeMem(title_source, AMIGA_TITLE_OVERLAY_BYTES + AMIGA_TITLE_OVERLAY_MASK_BYTES);
        FreeMem(audio_pool, PROBE_AUDIO_BYTES);
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        CloseLibrary((struct Library *)GfxBase);
        return 33;
    }
    this_task = FindTask(NULL);
    if (this_task) {
        old_task_pri = SetTaskPri(this_task, PROBE_TASK_PRIORITY);
    }
    OwnBlitter();
    owns_blitter = TRUE;
    tick = 0;
    boot_audio_events = 0;
    last_dirs = 0;
    last_fire = joystick_fire_down_probe() ? 1u : 0u;
    probe_set_blitter_input_latch(&input_latch, &last_dirs, &last_fire);
    menu_repeat_dir = DIR_NONE;
    menu_repeat_ticks = 0;
    probe_high_score_table_set_defaults(&g_probe_high_scores);
    memset(&g_probe_high_score_entry, 0, sizeof(g_probe_high_score_entry));
    game_init(game, (uint32_t)startup_seed_probe());
    boot_audio_events = game_consume_events(game);
    ui_mode = probe_ui_mode_for_game(game);
    update_probe_title_hw_sparkles(sprite_bank, chip_tiles, tick, probe_ui_uses_title(ui_mode));
    draw_probe_scene(&render_cache, 0, screen[0], -1, 0, chip_tiles, chip_bobs, chip_masks,
                     chip_title, chip_title_mask, title_cell_tile, title_cell_mask, game, ui_mode, (WORD)tick);
    wait_blitter();
    draw_probe_scene(&render_cache, 1, screen[1], 0, screen[0], chip_tiles, chip_bobs, chip_masks,
                     chip_title, chip_title_mask, title_cell_tile, title_cell_mask, game, ui_mode, (WORD)tick);
    wait_blitter();
    game_clear_dirty_static(game);

    if (!build_probe_copper(0, cop[0], screen[0], sprite_bank) ||
        !build_probe_copper(1, cop[1], screen[1], sprite_bank)) {
        if (owns_blitter) {
            DisownBlitter();
        }
        if (this_task) {
            SetTaskPri(this_task, old_task_pri);
        }
        if (game) FreeMem(game, sizeof(GameState));
        FreeMem(title_source, AMIGA_TITLE_OVERLAY_BYTES + AMIGA_TITLE_OVERLAY_MASK_BYTES);
        FreeMem(audio_pool, PROBE_AUDIO_BYTES);
        FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
        CloseLibrary((struct Library *)GfxBase);
        return 28;
    }
    clear_probe_gameplay_hw_enemy_sprites(0, cop[0]);
    clear_probe_gameplay_hw_enemy_sprites(1, cop[1]);
    patch_probe_hw_sprite_ptrs(0, sprite_bank, cop[0],
                               probe_ui_uses_title(ui_mode) ? PROBE_HW_PTR_MODE_TITLE : PROBE_HW_PTR_MODE_NULL);
    sprite_ptr_mode[0] = probe_ui_uses_title(ui_mode) ? PROBE_HW_PTR_MODE_TITLE : PROBE_HW_PTR_MODE_NULL;
    patch_probe_hw_sprite_ptrs(1, sprite_bank, cop[1],
                               probe_ui_uses_title(ui_mode) ? PROBE_HW_PTR_MODE_TITLE : PROBE_HW_PTR_MODE_NULL);
    sprite_ptr_mode[1] = sprite_ptr_mode[0];
    old_view = GfxBase->ActiView;
    LoadView(NULL);
    WaitTOF();
    WaitTOF();

    CREGS->dmacon = DMAF_ALL;
    patch_probe_copper_sparkles(0, tick);
    patch_probe_copper_sparkles(1, tick);
    patch_probe_copper_reflection(0, tick);
    patch_probe_copper_reflection(1, tick);
    patch_probe_copper_reflection_enabled(0, TRUE);
    patch_probe_copper_reflection_enabled(1, TRUE);
    reflection_enabled[0] = 1;
    reflection_enabled[1] = 1;
    copper_phase[0] = 0;
    copper_phase[1] = 0;
    install_copper(cop[0]);
    CREGS->dmacon = DMAF_SETCLR | DMAF_MASTER | DMAF_COPPER | DMAF_RASTER | DMAF_BLITTER;
    WaitTOF();
    CREGS->dmacon = DMAF_SETCLR | DMAF_SPRITE;
    front_index = 0;
    draw_index = 1;
    title_jingle_queued = FALSE;
    pending_post_game_over_resolution = FALSE;
    pending_post_game_over_insert_index = -1;
    pending_post_game_over_score = 0u;
    post_high_score_pause_ticks = 0;
    sprite_dma_enabled = TRUE;

    if (game->phase == GAME_PHASE_ROUND_INTRO && game->start_title_pending) {
        if ((boot_audio_events & GAME_EVENT_ROUND_START) != 0u) {
            (void)probe_sfx_play_major(&sfx, AMIGA_SFX_SAMPLE_TITLE_START_JINGLE, 64);
            title_jingle_queued = TRUE;
        }
        boot_audio_events &= (uint32_t)~GAME_EVENT_ROUND_START;
    }
    probe_sfx_dispatch_game_events(&sfx, game, boot_audio_events);
    boot_audio_events = 0;

    while (1) {
        InputState input;
        WORD next_ui_mode;
        uint32_t events;
        uint32_t audio_events;
        BOOL title_copper_active;
        tick = tick + 1UL;
        probe_sfx_tick(&sfx);
        probe_poll_input_latch(&input_latch, &last_dirs, &last_fire);
        input = input_latch;
        probe_clear_input_latch(&input_latch);
        probe_apply_menu_input_repeat(game, &input, &menu_repeat_dir, &menu_repeat_ticks);
        if (g_probe_high_score_entry.active) {
            if (probe_update_high_score_entry(&g_probe_high_score_entry, &input)) {
                probe_remember_high_score_initials(g_probe_high_score_entry.initials);
                probe_high_score_insert(&g_probe_high_scores,
                                        g_probe_high_score_entry.insert_index,
                                        g_probe_high_score_entry.initials,
                                        g_probe_high_score_entry.pending_score);
                post_high_score_pause_ticks = (WORD)((GAME_FIXED_TICK_HZ * 3) / 5);
                pending_post_game_over_resolution = FALSE;
                pending_post_game_over_insert_index = -1;
                pending_post_game_over_score = 0u;
                if (game->phase == GAME_PHASE_GAME_OVER) {
                    InputState resume_input;
                    memset(&resume_input, 0, sizeof(resume_input));
                    game->phase_timer_ticks = 0;
                    game_step(game, &resume_input);
                    events = game_consume_events(game);
                    audio_events = events;
                    if ((events & GAME_EVENT_ROUND_START) != 0u) {
                        probe_reset_render_cache(&render_cache);
                    }
                    if (game->phase == GAME_PHASE_ROUND_INTRO && game->start_title_pending) {
                        audio_events &= (uint32_t)~GAME_EVENT_ROUND_START;
                        title_jingle_queued = TRUE;
                    }
                    game->start_was_down = FALSE;
                    game->fire_was_down = FALSE;
                    game->fire_confirm_armed = FALSE;
                    probe_sfx_dispatch_game_events(&sfx, game, audio_events);
                }
            }
        } else if (post_high_score_pause_ticks > 0) {
            --post_high_score_pause_ticks;
        } else {
            if (input.fire_released &&
                sfx.major_busy_ticks == 0 &&
                game->phase != GAME_PHASE_PLAYING &&
                !(game->phase == GAME_PHASE_PERK_CHOICE && game->phase_timer_ticks > 0)) {
                probe_sfx_play_sample_now(&sfx, AMIGA_SFX_SAMPLE_TITLE_CONFIRM_CHIRP, 2, 48);
            }
            game_step(game, &input);
            probe_poll_input_latch(&input_latch, &last_dirs, &last_fire);
            events = game_consume_events(game);
            audio_events = events;
            if ((events & GAME_EVENT_ROUND_START) != 0u) {
                probe_reset_render_cache(&render_cache);
            }
            if ((events & GAME_EVENT_GAME_OVER) != 0u) {
                pending_post_game_over_resolution = TRUE;
                pending_post_game_over_score = game->score;
                pending_post_game_over_insert_index = probe_high_score_find_insert_index(&g_probe_high_scores, game->score);
                if (pending_post_game_over_insert_index >= 0) {
                    audio_events &= (uint32_t)~GAME_EVENT_GAME_OVER;
                    (void)probe_sfx_play_major(&sfx, AMIGA_SFX_SAMPLE_NEW_HIGH_SCORE_FANFARE, 64);
                }
            }
            if (pending_post_game_over_resolution &&
                pending_post_game_over_insert_index >= 0 &&
                game->phase != GAME_PHASE_GAME_OVER) {
                probe_begin_high_score_entry(&g_probe_high_score_entry,
                                             pending_post_game_over_insert_index,
                                             pending_post_game_over_score);
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
                    (void)probe_sfx_play_major(&sfx, AMIGA_SFX_SAMPLE_TITLE_START_JINGLE, 64);
                    title_jingle_queued = TRUE;
                }
                audio_events &= (uint32_t)~GAME_EVENT_ROUND_START;
            } else if (!g_probe_high_score_entry.active) {
                title_jingle_queued = FALSE;
            }
            probe_sfx_dispatch_game_events(&sfx, game, audio_events);
        }
        next_ui_mode = probe_ui_mode_for_game(game);
        if (next_ui_mode < 0) {
            probe_accumulate_core_dirty(&render_cache, game);
        }
        ui_mode = next_ui_mode;
        title_copper_active = probe_ui_uses_title(ui_mode);
        if (reflection_enabled[draw_index] != 1u) {
            patch_probe_copper_reflection_enabled(draw_index, TRUE);
            reflection_enabled[draw_index] = 1u;
        }
        copper_anim_phase = g_probe_high_score_entry.active ?
            copper_phase[draw_index] :
            ((ui_mode < 0) ? (UBYTE)(tick >> 3) : (UBYTE)(tick >> 2));
        if (copper_phase[draw_index] != copper_anim_phase) {
            patch_probe_copper_sparkles(draw_index, tick);
            patch_probe_copper_reflection(draw_index, tick);
            copper_phase[draw_index] = copper_anim_phase;
        }
        draw_probe_scene(&render_cache, draw_index, screen[draw_index], front_index, screen[front_index], chip_tiles, chip_bobs, chip_masks,
                         chip_title, chip_title_mask, title_cell_tile, title_cell_mask, game, ui_mode, (WORD)tick);
        wait_blitter();
        probe_poll_input_latch(&input_latch, &last_dirs, &last_fire);
        {
            BOOL title_sprites_active = title_copper_active;
            WORD ptr_mode = PROBE_HW_PTR_MODE_NULL;
            if (title_sprites_active) {
                update_probe_title_hw_sparkles(sprite_bank, chip_tiles, tick, TRUE);
                ptr_mode = PROBE_HW_PTR_MODE_TITLE;
            }
            if (sprite_ptr_mode[draw_index] != ptr_mode) {
                patch_probe_hw_sprite_ptrs(draw_index, sprite_bank, cop[draw_index], ptr_mode);
                sprite_ptr_mode[draw_index] = ptr_mode;
            }
            if (title_sprites_active != sprite_dma_enabled) {
                CREGS->dmacon = title_sprites_active ? (DMAF_SETCLR | DMAF_SPRITE) : DMAF_SPRITE;
                sprite_dma_enabled = title_sprites_active ? TRUE : FALSE;
            }
        }
        queue_copper_next_frame(cop[draw_index]);
        wait_next_frame_start_probe(&input_latch, &last_dirs, &last_fire);
        front_index = draw_index;
        draw_index = (WORD)(front_index ^ 1);
    }

    LoadView(old_view);
    WaitTOF();
    WaitTOF();
    probe_sfx_shutdown(&sfx);
    if (owns_blitter) {
        DisownBlitter();
    }
    if (this_task) {
        SetTaskPri(this_task, old_task_pri);
    }
    FreeMem(title_source, AMIGA_TITLE_OVERLAY_BYTES + AMIGA_TITLE_OVERLAY_MASK_BYTES);
    FreeMem(audio_pool, PROBE_AUDIO_BYTES);
    FreeMem(dma_arena, PROBE_DMA_ARENA_BYTES);
    if (game) FreeMem(game, sizeof(GameState));
    CloseLibrary((struct Library *)GfxBase);
    return 0;
}
