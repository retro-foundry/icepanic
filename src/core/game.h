#ifndef ICEPANIC_GAME_H
#define ICEPANIC_GAME_H

#include <stdbool.h>
#include <stdint.h>

#define GAME_GRID_WIDTH 20
#define GAME_GRID_HEIGHT 12
#define GAME_TILE_SIZE 16
#define GAME_LOGICAL_WIDTH 320
#define GAME_LOGICAL_HEIGHT 200

#define GAME_FIXED_TICK_HZ 50

#define GAME_MAX_ENEMIES 16
#define GAME_MAX_MOVING_BLOCKS 8
#define GAME_MAX_SCORE_POPUPS 8
#define GAME_MAX_IMPACT_FX 12
#define GAME_MAX_ACTIVE_MINES 8
#define GAME_MAX_ACTIVE_ITEMS 8
#define GAME_PERK_CHOICE_COUNT 3
#define GAME_META_CHOICE_COUNT 4
#define GAME_META_UNLOCK_TIER2_PROGRESS 10u
#define GAME_META_UNLOCK_TIER3_PROGRESS 22u

typedef int32_t GamePixelFp;

typedef enum Terrain {
    TERRAIN_FLOOR = 0,
    TERRAIN_WALL = 1,
    TERRAIN_BORDER = 2
} Terrain;

typedef enum BlockType {
    BLOCK_NONE = 0,
    BLOCK_ICE = 1,
    BLOCK_SPECIAL = 2,
    BLOCK_CRACKED = 3,
    BLOCK_UNBREAKABLE = 4
} BlockType;

typedef enum ItemType {
    ITEM_NONE = 0,
    ITEM_FISH = 1,
    ITEM_EGG = 2,
    ITEM_GEM = 3,
    ITEM_FRUIT = 4,
    ITEM_MYSTERY = 5
} ItemType;

typedef enum Direction {
    DIR_NONE = 0,
    DIR_UP = 1,
    DIR_DOWN = 2,
    DIR_LEFT = 3,
    DIR_RIGHT = 4
} Direction;

typedef enum PlayerState {
    PLAYER_IDLE = 0,
    PLAYER_WALKING = 1,
    PLAYER_PUSHING = 2,
    PLAYER_STUNNED = 3,
    PLAYER_DYING = 4,
    PLAYER_VICTORY = 5
} PlayerState;

typedef enum EnemyState {
    ENEMY_SPAWNING = 0,
    ENEMY_ROAMING = 1,
    ENEMY_STUNNED = 2,
    ENEMY_RECOVERING = 3,
    ENEMY_CRUSHED = 4,
    ENEMY_DYING = 5
} EnemyState;

typedef enum EnemyType {
    ENEMY_TYPE_CHASER = 0,
    ENEMY_TYPE_WANDERER = 1
} EnemyType;

typedef enum GamePhase {
    GAME_PHASE_ROUND_INTRO = 0,
    GAME_PHASE_PLAYING = 1,
    GAME_PHASE_PLAYER_DYING = 2,
    GAME_PHASE_ROUND_CLEAR = 3,
    GAME_PHASE_GAME_OVER = 4,
    GAME_PHASE_PERK_CHOICE = 5,
    GAME_PHASE_META_UPGRADE = 6
} GamePhase;

typedef uint32_t GameEventFlags;

#define GAME_EVENT_NONE 0UL
#define GAME_EVENT_CRUSH (1UL << 0)
#define GAME_EVENT_ITEM_COLLECT (1UL << 1)
#define GAME_EVENT_TIMER_DANGER_PULSE (1UL << 2)
#define GAME_EVENT_SPECIAL_ALIGNMENT (1UL << 3)
#define GAME_EVENT_PLAYER_DIED (1UL << 4)
#define GAME_EVENT_ROUND_CLEAR (1UL << 5)
#define GAME_EVENT_ROUND_START (1UL << 6)
#define GAME_EVENT_GAME_OVER (1UL << 7)
#define GAME_EVENT_BLOCK_IMPACT (1UL << 8)
#define GAME_EVENT_META_BANKED (1UL << 9)
#define GAME_EVENT_STAGE_MODIFIER (1UL << 10)
#define GAME_EVENT_META_SPENT (1UL << 11)
#define GAME_EVENT_MINE_PLACED (1UL << 12)
#define GAME_EVENT_MINE_EXPLODED (1UL << 13)
#define GAME_EVENT_LIFE_LOST (1UL << 14)
#define GAME_EVENT_COMBO (1UL << 15)
#define GAME_EVENT_BLOCK_PUSH (1UL << 16)

typedef enum PerkType {
    PERK_NONE = 0,
    PERK_LIFE_BOOST = 1,
    PERK_TIME_BOOST = 2,
    PERK_SCORE_BOOST = 3,
    PERK_ENEMY_SLOW = 4,
    PERK_MINES = 5
} PerkType;

typedef enum StageModifierType {
    STAGE_MOD_NONE = 0,
    STAGE_MOD_SWARM = 1,
    STAGE_MOD_SHATTERED_ICE = 2,
    STAGE_MOD_BLACK_ICE = 3,
    STAGE_MOD_TURBO_SLIDE = 4,
    STAGE_MOD_FROST_SIREN = 5
} StageModifierType;

typedef enum MetaUpgradeType {
    META_UPGRADE_NONE = 0,
    META_UPGRADE_LIFE_PACK = 1,
    META_UPGRADE_TIME_PACK = 2,
    META_UPGRADE_SCORE_PACK = 3,
    META_UPGRADE_SLOW_PACK = 4
} MetaUpgradeType;

typedef enum ImpactFxStyle {
    IMPACT_FX_STYLE_STOP = 0,
    IMPACT_FX_STYLE_SPECIAL_STOP = 1,
    IMPACT_FX_STYLE_CRACKED_STOP = 2,
    IMPACT_FX_STYLE_CRUSH = 3,
    IMPACT_FX_STYLE_SPECIAL_CRUSH = 4,
    IMPACT_FX_STYLE_CRACKED_CRUSH = 5,
    IMPACT_FX_STYLE_MINE_BLAST = 6,
    IMPACT_FX_STYLE_ENEMY_DEATH_A = 7,
    IMPACT_FX_STYLE_ENEMY_DEATH_B = 8
} ImpactFxStyle;

typedef struct InputState {
    bool up;
    bool down;
    bool left;
    bool right;
    bool start;
    bool fire_pressed;
    bool fire_released;
    Direction newest_press;
} InputState;

typedef struct RoundConfig {
    int enemy_count;
    int enemy_wanderer_count;
    int enemy_speed_fp;
    int aggression_percent;
    int round_time_ticks;
} RoundConfig;

typedef struct Player {
    int tile_x;
    int tile_y;
    GamePixelFp pixel_fp_x;
    GamePixelFp pixel_fp_y;
    Direction facing;
    Direction move_dir;
    Direction buffered_dir;
    PlayerState state;
    int move_remaining_fp;
    int push_timer;
    int bump_timer;
    int respawn_invuln_ticks;
    int anim_frame;
    bool alive;
    bool mine_drop_queued;
} Player;

typedef struct Enemy {
    int tile_x;
    int tile_y;
    GamePixelFp pixel_fp_x;
    GamePixelFp pixel_fp_y;
    Direction direction;
    EnemyState state;
    int speed_fp;
    int move_remaining_fp;
    int anim_frame;
    EnemyType type;
    int spawn_ticks;
    int decision_cooldown_ticks;
    bool alive;
} Enemy;

typedef struct MovingBlock {
    int tile_x;
    int tile_y;
    GamePixelFp pixel_fp_x;
    GamePixelFp pixel_fp_y;
    Direction direction;
    int speed_fp;
    int intra_fp;
    int combo_count;
    int startup_ticks;
    BlockType type;
    bool active;
} MovingBlock;

typedef struct ScorePopup {
    GamePixelFp pixel_fp_x;
    GamePixelFp pixel_fp_y;
    int value;
    int ttl_ticks;
    bool active;
} ScorePopup;

typedef struct ImpactFx {
    GamePixelFp pixel_fp_x;
    GamePixelFp pixel_fp_y;
    int style;
    int base_ttl_ticks;
    int anchor_tile_x;
    int anchor_tile_y;
    Direction direction;
    int ttl_ticks;
    bool anchors_block;
    bool active;
} ImpactFx;

typedef struct GameState {
    int width;
    int height;

    Terrain terrain[GAME_GRID_HEIGHT][GAME_GRID_WIDTH];
    BlockType blocks[GAME_GRID_HEIGHT][GAME_GRID_WIDTH];
    ItemType items[GAME_GRID_HEIGHT][GAME_GRID_WIDTH];
    uint8_t active_item_x[GAME_MAX_ACTIVE_ITEMS];
    uint8_t active_item_y[GAME_MAX_ACTIVE_ITEMS];
    int active_item_count;
    bool mines[GAME_GRID_HEIGHT][GAME_GRID_WIDTH];
    uint8_t mine_fuse_ticks[GAME_GRID_HEIGHT][GAME_GRID_WIDTH];
    uint8_t active_mine_x[GAME_MAX_ACTIVE_MINES];
    uint8_t active_mine_y[GAME_MAX_ACTIVE_MINES];
    int active_mine_count;
    uint32_t dirty_static_rows[GAME_GRID_HEIGHT];

    Player player;
    Enemy enemies[GAME_MAX_ENEMIES];
    int enemy_count;
    int alive_enemy_count;
    uint16_t alive_enemy_mask;
    MovingBlock moving_blocks[GAME_MAX_MOVING_BLOCKS];
    uint16_t active_moving_block_mask;
    ScorePopup score_popups[GAME_MAX_SCORE_POPUPS];
    int active_score_popup_count;
    uint16_t active_score_popup_mask;
    ImpactFx impact_fx[GAME_MAX_IMPACT_FX];
    int active_impact_fx_count;
    uint16_t active_impact_fx_mask;

    int player_spawn_x;
    int player_spawn_y;

    uint32_t score;
    int lives;
    int round;
    int round_time_ticks;
    int round_clear_pending_ticks;
    int round_clear_bonus_score;
    int kills_this_round;
    int kills_since_item_spawn;
    int bonus_item_timer_ticks;
    int special_alignment_flash_ticks;
    bool special_alignment_awarded;
    bool timer_danger_active;
    int timer_danger_pulse_ticks;
    uint32_t pending_events;

    int run_score_mult_permille;
    int run_round_time_bonus_ticks;
    int run_enemy_speed_penalty_fp;
    int run_mine_capacity;
    int run_mine_stock;
    int run_shards;
    uint32_t meta_shards;
    uint32_t meta_progress_points;
    int meta_unlock_tier;
    StageModifierType stage_modifier;
    int stage_modifier_flash_ticks;
    int stage_modifier_cooldowns[STAGE_MOD_FROST_SIREN + 1];
    int perk_levels[PERK_MINES + 1];
    int perk_offer_cooldowns[PERK_MINES + 1];
    int recent_procgen_template_ids[3];
    int recent_procgen_template_count;
    int recent_procgen_template_cursor;
    PerkType perk_choices[GAME_PERK_CHOICE_COUNT];
    int perk_choice_count;
    int perk_choice_selected;
    MetaUpgradeType meta_choices[GAME_META_CHOICE_COUNT];
    int meta_choice_costs[GAME_META_CHOICE_COUNT];
    int meta_choice_count;
    int meta_choice_selected;
    int pending_round_after_choice;
    Direction mine_tap_dir;
    int mine_tap_ticks;
    StageModifierType debug_forced_stage_modifier;
    bool debug_force_stage_modifier_pending;
    int debug_forced_procgen_template_index;
    bool debug_force_procgen_template_pending;
    int debug_last_procgen_template_index;
    bool start_was_down;
    bool fire_was_down;
    bool fire_confirm_armed;
    bool start_title_pending;

    uint32_t rng_state;

    GamePhase phase;
    int phase_timer_ticks;

    RoundConfig round_config;
} GameState;

typedef GameState RenderState;

void game_init(GameState *gs, uint32_t seed);
void game_start_round(GameState *gs, int round_index);
void game_step(GameState *gs, const InputState *in);
void game_get_render_state(const GameState *gs, RenderState *out);
const uint32_t *game_dirty_static_rows(const GameState *gs);
void game_clear_dirty_static(GameState *gs);
uint32_t game_consume_events(GameState *gs);
void game_set_meta_shards(GameState *gs, uint32_t shards);
uint32_t game_get_meta_shards(const GameState *gs);
void game_set_meta_progress(GameState *gs, uint32_t progress_points);
uint32_t game_get_meta_progress(const GameState *gs);
int game_get_meta_unlock_tier(const GameState *gs);

/* Debug helper used by tests for deterministic replay checks. */
uint32_t game_debug_hash(const GameState *gs);
int game_debug_count_enemy_type(const GameState *gs, EnemyType type);
bool game_debug_validate_level_rows(const char rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]);
void game_debug_force_next_stage_modifier(GameState *gs, StageModifierType modifier);
int game_debug_procgen_template_count(void);
bool game_debug_get_procgen_template_rows(int template_index, char out_rows[GAME_GRID_HEIGHT][GAME_GRID_WIDTH + 1]);
int game_debug_last_procgen_template_index(const GameState *gs);
void game_debug_force_next_procgen_template(GameState *gs, int template_index);

#endif
