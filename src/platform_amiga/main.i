
#line 4 "stdbool.h"
 typedef unsigned char bool ;
#line 6 "stdint.h"
 typedef signed char int8_t ;
 typedef signed short int16_t ;
 typedef signed long int32_t ;
 typedef signed long long int64_t ;









 typedef unsigned char uint8_t ;
 typedef unsigned short uint16_t ;
 typedef unsigned long uint32_t ;
 typedef unsigned long long uint64_t ;





 typedef signed char int_least8_t ;
 typedef signed short int_least16_t ;
 typedef signed long int_least32_t ;
 typedef signed long long int_least64_t ;









 typedef unsigned char uint_least8_t ;
 typedef unsigned short uint_least16_t ;
 typedef unsigned long uint_least32_t ;
 typedef unsigned long long uint_least64_t ;





 typedef signed int int_fast8_t ;
 typedef signed int int_fast16_t ;
 typedef signed long int_fast32_t ;
 typedef signed long long int_fast64_t ;









 typedef unsigned int uint_fast8_t ;
 typedef unsigned int uint_fast16_t ;
 typedef unsigned long uint_fast32_t ;
 typedef unsigned long long uint_fast64_t ;






 typedef long intptr_t ;



 typedef unsigned long uintptr_t ;


 typedef long long intmax_t ;



 typedef unsigned long long uintmax_t ;
#line 24 "game.h"
 typedef int32_t GamePixelFp ;

 typedef enum Terrain {
 TERRAIN_FLOOR = 0 ,
 TERRAIN_WALL = 1 ,
 TERRAIN_BORDER = 2
 } Terrain ;

 typedef enum BlockType {
 BLOCK_NONE = 0 ,
 BLOCK_ICE = 1 ,
 BLOCK_SPECIAL = 2 ,
 BLOCK_CRACKED = 3 ,
 BLOCK_UNBREAKABLE = 4
 } BlockType ;

 typedef enum ItemType {
 ITEM_NONE = 0 ,
 ITEM_FISH = 1 ,
 ITEM_EGG = 2 ,
 ITEM_GEM = 3 ,
 ITEM_FRUIT = 4 ,
 ITEM_MYSTERY = 5
 } ItemType ;

 typedef enum Direction {
 DIR_NONE = 0 ,
 DIR_UP = 1 ,
 DIR_DOWN = 2 ,
 DIR_LEFT = 3 ,
 DIR_RIGHT = 4
 } Direction ;

 typedef enum PlayerState {
 PLAYER_IDLE = 0 ,
 PLAYER_WALKING = 1 ,
 PLAYER_PUSHING = 2 ,
 PLAYER_STUNNED = 3 ,
 PLAYER_DYING = 4 ,
 PLAYER_VICTORY = 5
 } PlayerState ;

 typedef enum EnemyState {
 ENEMY_SPAWNING = 0 ,
 ENEMY_ROAMING = 1 ,
 ENEMY_STUNNED = 2 ,
 ENEMY_RECOVERING = 3 ,
 ENEMY_CRUSHED = 4 ,
 ENEMY_DYING = 5
 } EnemyState ;

 typedef enum EnemyType {
 ENEMY_TYPE_CHASER = 0 ,
 ENEMY_TYPE_WANDERER = 1
 } EnemyType ;

 typedef enum GamePhase {
 GAME_PHASE_ROUND_INTRO = 0 ,
 GAME_PHASE_PLAYING = 1 ,
 GAME_PHASE_PLAYER_DYING = 2 ,
 GAME_PHASE_ROUND_CLEAR = 3 ,
 GAME_PHASE_GAME_OVER = 4 ,
 GAME_PHASE_PERK_CHOICE = 5 ,
 GAME_PHASE_META_UPGRADE = 6
 } GamePhase ;

 typedef enum GameEventFlags {
 GAME_EVENT_NONE = 0 ,
 GAME_EVENT_CRUSH = 1UL << 0 ,
 GAME_EVENT_ITEM_COLLECT = 1UL << 1 ,
 GAME_EVENT_TIMER_DANGER_PULSE = 1UL << 2 ,
 GAME_EVENT_SPECIAL_ALIGNMENT = 1UL << 3 ,
 GAME_EVENT_PLAYER_DIED = 1UL << 4 ,
 GAME_EVENT_ROUND_CLEAR = 1UL << 5 ,
 GAME_EVENT_ROUND_START = 1UL << 6 ,
 GAME_EVENT_GAME_OVER = 1UL << 7 ,
 GAME_EVENT_BLOCK_IMPACT = 1UL << 8 ,
 GAME_EVENT_META_BANKED = 1UL << 9 ,
 GAME_EVENT_STAGE_MODIFIER = 1UL << 10 ,
 GAME_EVENT_META_SPENT = 1UL << 11 ,
 GAME_EVENT_MINE_PLACED = 1UL << 12 ,
 GAME_EVENT_MINE_EXPLODED = 1UL << 13 ,
 GAME_EVENT_LIFE_LOST = 1UL << 14 ,
 GAME_EVENT_COMBO = 1UL << 15 ,
 GAME_EVENT_BLOCK_PUSH = 1UL << 16
 } GameEventFlags ;

 typedef enum PerkType {
 PERK_NONE = 0 ,
 PERK_LIFE_BOOST = 1 ,
 PERK_TIME_BOOST = 2 ,
 PERK_SCORE_BOOST = 3 ,
 PERK_ENEMY_SLOW = 4 ,
 PERK_MINES = 5
 } PerkType ;

 typedef enum StageModifierType {
 STAGE_MOD_NONE = 0 ,
 STAGE_MOD_SWARM = 1 ,
 STAGE_MOD_SHATTERED_ICE = 2 ,
 STAGE_MOD_BLACK_ICE = 3 ,
 STAGE_MOD_TURBO_SLIDE = 4 ,
 STAGE_MOD_FROST_SIREN = 5
 } StageModifierType ;

 typedef enum MetaUpgradeType {
 META_UPGRADE_NONE = 0 ,
 META_UPGRADE_LIFE_PACK = 1 ,
 META_UPGRADE_TIME_PACK = 2 ,
 META_UPGRADE_SCORE_PACK = 3 ,
 META_UPGRADE_SLOW_PACK = 4
 } MetaUpgradeType ;

 typedef enum ImpactFxStyle {
 IMPACT_FX_STYLE_STOP = 0 ,
 IMPACT_FX_STYLE_SPECIAL_STOP = 1 ,
 IMPACT_FX_STYLE_CRACKED_STOP = 2 ,
 IMPACT_FX_STYLE_CRUSH = 3 ,
 IMPACT_FX_STYLE_SPECIAL_CRUSH = 4 ,
 IMPACT_FX_STYLE_CRACKED_CRUSH = 5 ,
 IMPACT_FX_STYLE_MINE_BLAST = 6 ,
 IMPACT_FX_STYLE_ENEMY_DEATH_A = 7 ,
 IMPACT_FX_STYLE_ENEMY_DEATH_B = 8
 } ImpactFxStyle ;

 typedef struct InputState {
 bool up ;
 bool down ;
 bool left ;
 bool right ;
 bool start ;
 bool fire_pressed ;
 bool fire_released ;
 Direction newest_press ;
 } InputState ;

 typedef struct RoundConfig {
 int enemy_count ;
 int enemy_wanderer_count ;
 int enemy_speed_fp ;
 int aggression_percent ;
 int round_time_ticks ;
 } RoundConfig ;

 typedef struct Player {
 int tile_x ;
 int tile_y ;
 GamePixelFp pixel_fp_x ;
 GamePixelFp pixel_fp_y ;
 Direction facing ;
 Direction move_dir ;
 Direction buffered_dir ;
 PlayerState state ;
 int move_remaining_fp ;
 int push_timer ;
 int bump_timer ;
 int respawn_invuln_ticks ;
 int anim_frame ;
 bool alive ;
 bool mine_drop_queued ;
 } Player ;

 typedef struct Enemy {
 int tile_x ;
 int tile_y ;
 GamePixelFp pixel_fp_x ;
 GamePixelFp pixel_fp_y ;
 Direction direction ;
 EnemyState state ;
 int speed_fp ;
 int move_remaining_fp ;
 int anim_frame ;
 EnemyType type ;
 int spawn_ticks ;
 int decision_cooldown_ticks ;
 bool alive ;
 } Enemy ;

 typedef struct MovingBlock {
 int tile_x ;
 int tile_y ;
 GamePixelFp pixel_fp_x ;
 GamePixelFp pixel_fp_y ;
 Direction direction ;
 int speed_fp ;
 int intra_fp ;
 int combo_count ;
 int startup_ticks ;
 BlockType type ;
 bool active ;
 } MovingBlock ;

 typedef struct ScorePopup {
 GamePixelFp pixel_fp_x ;
 GamePixelFp pixel_fp_y ;
 int value ;
 int ttl_ticks ;
 bool active ;
 } ScorePopup ;

 typedef struct ImpactFx {
 GamePixelFp pixel_fp_x ;
 GamePixelFp pixel_fp_y ;
 int style ;
 int base_ttl_ticks ;
 int anchor_tile_x ;
 int anchor_tile_y ;
 Direction direction ;
 int ttl_ticks ;
 bool anchors_block ;
 bool active ;
 } ImpactFx ;

 typedef struct GameState {
 int width ;
 int height ;

 Terrain terrain [ 12 ] [ 20 ] ;
 BlockType blocks [ 12 ] [ 20 ] ;
 ItemType items [ 12 ] [ 20 ] ;
 bool mines [ 12 ] [ 20 ] ;
 uint8_t mine_fuse_ticks [ 12 ] [ 20 ] ;
 uint32_t dirty_static_rows [ 12 ] ;

 Player player ;
 Enemy enemies [ 16 ] ;
 int enemy_count ;
 MovingBlock moving_blocks [ 8 ] ;
 ScorePopup score_popups [ 8 ] ;
 ImpactFx impact_fx [ 12 ] ;

 int player_spawn_x ;
 int player_spawn_y ;

 uint32_t score ;
 int lives ;
 int round ;
 int round_time_ticks ;
 int round_clear_bonus_score ;
 int kills_this_round ;
 int kills_since_item_spawn ;
 int bonus_item_timer_ticks ;
 int special_alignment_flash_ticks ;
 bool special_alignment_awarded ;
 bool timer_danger_active ;
 int timer_danger_pulse_ticks ;
 uint32_t pending_events ;

 int run_score_mult_permille ;
 int run_round_time_bonus_ticks ;
 int run_enemy_speed_penalty_fp ;
 int run_mine_capacity ;
 int run_mine_stock ;
 int run_shards ;
 uint32_t meta_shards ;
 uint32_t meta_progress_points ;
 int meta_unlock_tier ;
 StageModifierType stage_modifier ;
 int stage_modifier_flash_ticks ;
 int stage_modifier_cooldowns [ STAGE_MOD_FROST_SIREN + 1 ] ;
 int perk_levels [ PERK_MINES + 1 ] ;
 int perk_offer_cooldowns [ PERK_MINES + 1 ] ;
 int recent_procgen_template_ids [ 3 ] ;
 int recent_procgen_template_count ;
 int recent_procgen_template_cursor ;
 PerkType perk_choices [ 3 ] ;
 int perk_choice_count ;
 int perk_choice_selected ;
 MetaUpgradeType meta_choices [ 4 ] ;
 int meta_choice_costs [ 4 ] ;
 int meta_choice_count ;
 int meta_choice_selected ;
 int pending_round_after_choice ;
 Direction mine_tap_dir ;
 int mine_tap_ticks ;
 StageModifierType debug_forced_stage_modifier ;
 bool debug_force_stage_modifier_pending ;
 int debug_forced_procgen_template_index ;
 bool debug_force_procgen_template_pending ;
 int debug_last_procgen_template_index ;
 bool start_was_down ;
 bool start_title_pending ;

 uint32_t rng_state ;

 GamePhase phase ;
 int phase_timer_ticks ;

 RoundConfig round_config ;
 } GameState ;

 typedef GameState RenderState ;

 void game_init ( GameState * gs , uint32_t seed ) ;
 void game_start_round ( GameState * gs , int round_index ) ;
 void game_step ( GameState * gs , const InputState * in ) ;
 void game_get_render_state ( const GameState * gs , RenderState * out ) ;
 const uint32_t * game_dirty_static_rows ( const GameState * gs ) ;
 void game_clear_dirty_static ( GameState * gs ) ;
 uint32_t game_consume_events ( GameState * gs ) ;
 void game_set_meta_shards ( GameState * gs , uint32_t shards ) ;
 uint32_t game_get_meta_shards ( const GameState * gs ) ;
 void game_set_meta_progress ( GameState * gs , uint32_t progress_points ) ;
 uint32_t game_get_meta_progress ( const GameState * gs ) ;
 int game_get_meta_unlock_tier ( const GameState * gs ) ;


 uint32_t game_debug_hash ( const GameState * gs ) ;
 int game_debug_count_enemy_type ( const GameState * gs , EnemyType type ) ;
 bool game_debug_validate_level_rows ( const char rows [ 12 ] [ 20 + 1 ] ) ;
 void game_debug_force_next_stage_modifier ( GameState * gs , StageModifierType modifier ) ;
 int game_debug_procgen_template_count ( void ) ;
 bool game_debug_get_procgen_template_rows ( int template_index , char out_rows [ 12 ] [ 20 + 1 ] ) ;
 int game_debug_last_procgen_template_index ( const GameState * gs ) ;
 void game_debug_force_next_procgen_template ( GameState * gs , int template_index ) ;
#line 29 "amiga_assets.h"
 typedef enum AmigaSpriteId {
 AMIGA_SPR_FLOOR = 0 ,
 AMIGA_SPR_WALL = 1 ,
 AMIGA_SPR_BORDER = 2 ,
 AMIGA_SPR_BLOCK_ICE = 3 ,
 AMIGA_SPR_BLOCK_SPECIAL = 4 ,
 AMIGA_SPR_BLOCK_CRACKED = 5 ,
 AMIGA_SPR_BLOCK_UNBREAKABLE = 6 ,
 AMIGA_SPR_PLAYER_UP = 7 ,
 AMIGA_SPR_PLAYER_UP_ALT = 8 ,
 AMIGA_SPR_PLAYER_DOWN = 9 ,
 AMIGA_SPR_PLAYER_DOWN_ALT = 10 ,
 AMIGA_SPR_PLAYER_LEFT = 11 ,
 AMIGA_SPR_PLAYER_LEFT_ALT = 12 ,
 AMIGA_SPR_PLAYER_RIGHT = 13 ,
 AMIGA_SPR_PLAYER_RIGHT_ALT = 14 ,
 AMIGA_SPR_ENEMY_A = 15 ,
 AMIGA_SPR_ENEMY_A_ALT = 16 ,
 AMIGA_SPR_ENEMY_B = 17 ,
 AMIGA_SPR_ENEMY_B_ALT = 18 ,
 AMIGA_SPR_ENEMY_A_DEATH_0 = 19 ,
 AMIGA_SPR_ENEMY_A_DEATH_1 = 20 ,
 AMIGA_SPR_ENEMY_B_DEATH_0 = 21 ,
 AMIGA_SPR_ENEMY_B_DEATH_1 = 22 ,
 AMIGA_SPR_ITEM_FISH = 23 ,
 AMIGA_SPR_ITEM_EGG = 24 ,
 AMIGA_SPR_ITEM_GEM = 25 ,
 AMIGA_SPR_ITEM_FRUIT = 26 ,
 AMIGA_SPR_ITEM_MYSTERY = 27 ,
 AMIGA_SPR_HUD_LIFE = 28 ,
 AMIGA_SPR_EFFECT_SPARK = 29 ,
 AMIGA_SPR_MINE_0 = 30 ,
 AMIGA_SPR_MINE_1 = 31 ,
 AMIGA_SPR_MINE_2 = 32 ,
 AMIGA_SPR_COUNT = 33
 } AmigaSpriteId ;

 extern const uint16_t g_amiga_palette [ 32 ] ;
 extern const uint8_t g_amiga_tile_data [ 33 ] [ 160 ] ;
 extern const uint8_t g_amiga_tile_mask [ 33 ] [ 160 ] ;
 extern const uint8_t g_amiga_bob_data [ 33 ] [ 320 ] ;
 extern const uint8_t g_amiga_bob_mask [ 33 ] [ 320 ] ;
 extern const uint8_t g_amiga_title_overlay_data [ 7470 ] ;
 extern const uint8_t g_amiga_title_overlay_mask [ 7470 ] ;
#line 8 "exec\types.h"
 typedef long LONG ;
 typedef unsigned long ULONG ;
 typedef unsigned long LONGBITS ;
 typedef short WORD ;
 typedef unsigned short UWORD ;
 typedef unsigned short WORDBITS ;
 typedef char BYTE ;
 typedef unsigned char UBYTE ;
 typedef unsigned char BYTEBITS ;
 typedef unsigned char * STRPTR ;
 typedef STRPTR * APTR ;
 typedef ULONG CPTR ;
 typedef short SHORT ;
 typedef unsigned short USHORT ;
 typedef float FLOAT ;
 typedef double DOUBLE ;
 typedef short COUNT ;
 typedef unsigned short UCOUNT ;
 typedef short BOOL ;
 typedef unsigned char TEXT ;
#line 6 "string.h"
 typedef unsigned long size_t ;





 void * memcpy ( void * , const void * , size_t ) ;
 void * memmove ( void * , const void * , size_t ) ;
 char * strcpy ( char * , const char * ) ;
 char * strncpy ( char * , const char * , size_t ) ;
 char * strcat ( char * , const char * ) ;
 char * strncat ( char * , const char * , size_t ) ;
 int memcmp ( const void * , const void * , size_t ) ;
 int strcmp ( const char * , const char * ) ;
 int strncmp ( const char * , const char * , size_t ) ;
 void * memchr ( const void * , int , size_t ) ;
 char * strchr ( const char * , int ) ;
 size_t strcspn ( const char * , const char * ) ;
 char * strpbrk ( const char * , const char * ) ;
 char * strrchr ( const char * , int ) ;
 size_t strspn ( const char * , const char * ) ;
 char * strstr ( const char * , const char * ) ;
 void * memset ( void * , int , size_t ) ;
 size_t strlen ( const char * ) ;
 char * strtok ( char * , const char * ) ;
 char * strerror ( int ) ;
 int strcoll ( const char * , const char * ) ;
 size_t strxfrm ( char * , const char * , size_t ) ;



 void * __asm_memcpy ( __reg ( "a0" ) void * , __reg ( "a1" ) const void * ,
 __reg ( "d2" ) size_t ) =
 "\tinline\n"
 "\tmove.l\ta0,d0\n"
 "\tcmp.l\t#16,d2\n"
 "\tblo\t.l5\n"
 "\tmoveq\t#1,d1\n"
 "\tand.b\td0,d1\n"
 "\tbeq\t.l1\n"
 "\tmove.b\t(a1)+,(a0)+\n"
 "\tsubq.l\t#1,d2\n"
 ".l1\n"
 "\tmove.l\ta1,d1\n"
 "\tand.b\t#1,d1\n"
 "\tbeq\t.l3\n"
 "\tcmp.l\t#$10000,d2\n"
 "\tblo\t.l5\n"
 ".l2\n"
 "\tmove.b\t(a1)+,(a0)+\n"
 "\tsubq.l\t#1,d2\n"
 "\tbne\t.l2\n"
 "\tbra\t.l7\n"
 ".l3\n"
 "\tmoveq\t#3,d1\n"
 "\tand.l\td2,d1\n"
 "\tsub.l\td1,d2\n"
 ".l4\n"
 "\tmove.l\t(a1)+,(a0)+\n"
 "\tsubq.l\t#4,d2\n"
 "\tbne\t.l4\n"
 "\tmove.w\td1,d2\n"
 ".l5\n"
 "\tsubq.w\t#1,d2\n"
 "\tblo\t.l7\n"
 ".l6\n"
 "\tmove.b\t(a1)+,(a0)+\n"
 "\tdbf\td2,.l6\n"
 ".l7\n"
 "\teinline" ;
 void * __asm_memcpy_desc ( __reg ( "a0" ) void * , __reg ( "a1" ) const void * ,
 __reg ( "d2" ) size_t ) =
 "\tinline\n"
 "\tcmp.l\t#16,d2\n"
 "\tblo\t.l5\n"
 "\tmoveq\t#1,d1\n"
 "\tmove.l\ta0,d0\n"
 "\tand.b\td1,d0\n"
 "\tbeq\t.l1\n"
 "\tmove.b\t-(a1),-(a0)\n"
 "\tsubq.l\t#1,d2\n"
 ".l1\n"
 "\tmove.l\ta1,d0\n"
 "\tand.b\td1,d0\n"
 "\tbeq\t.l3\n"
 "\tcmp.l\t#$10000,d2\n"
 "\tblo\t.l5\n"
 ".l2\n"
 "\tmove.b\t-(a1),-(a0)\n"
 "\tsubq.l\t#1,d2\n"
 "\tbne\t.l2\n"
 "\tbra\t.l7\n"
 ".l3\n"
 "\tmoveq\t#3,d1\n"
 "\tand.l\td2,d1\n"
 "\tsub.l\td1,d2\n"
 ".l4\n"
 "\tmove.l\t-(a1),-(a0)\n"
 "\tsubq.l\t#4,d2\n"
 "\tbne\t.l4\n"
 "\tmove.w\td1,d2\n"
 ".l5\n"
 "\tsubq.w\t#1,d2\n"
 "\tblo\t.l7\n"
 ".l6\n"
 "\tmove.b\t-(a1),-(a0)\n"
 "\tdbf\td2,.l6\n"
 ".l7\n"
 "\tmove.l\ta0,d0\n"
 "\teinline" ;
 void * __asm_memset ( __reg ( "a0" ) void * , __reg ( "d0" ) int , __reg ( "d2" ) size_t ) =
 "\tinline\n"
 "\tmove.l\ta0,a1\n"
 "\tcmp.l\t#16,d2\n"
 "\tblo\t.l3\n"
 "\tmove.l\ta0,d1\n"
 "\tand.b\t#1,d1\n"
 "\tbeq\t.l1\n"
 "\tmove.b\td0,(a0)+\n"
 "\tsubq.l\t#1,d2\n"
 ".l1\n"
 "\tmove.b\td0,d1\n"
 "\tlsl.w\t#8,d0\n"
 "\tmove.b\td1,d0\n"
 "\tmove.w\td0,d1\n"
 "\tswap\td0\n"
 "\tmove.w\td1,d0\n"
 "\tmoveq\t#3,d1\n"
 "\tand.l\td2,d1\n"
 "\tsub.l\td1,d2\n"
 ".l2\n"
 "\tmove.l\td0,(a0)+\n"
 "\tsubq.l\t#4,d2\n"
 "\tbne\t.l2\n"
 "\tmove.w\td1,d2\n"
 ".l3\n"
 "\tsubq.w\t#1,d2\n"
 "\tblo\t.l5\n"
 ".l4\n"
 "\tmove.b\td0,(a0)+\n"
 "\tdbf\td2,.l4\n"
 ".l5\n"
 "\tmove.l\ta1,d0\n"
 "\teinline" ;









































































































 __vattr ( "regused(" "d0/a0" ")" ) size_t __asm_strlen ( __reg ( "a0" ) const char * ) =
 "\tinline\n"
 "\tmove.l\ta0,d0\n"
 ".l1\n"
 "\ttst.b\t(a0)+\n"
 "\tbne\t.l1\n"
 "\tsub.l\ta0,d0\n"
 "\tnot.l\td0\n"
 "\teinline" ;
 __vattr ( "regused(" "d0/a0/a1" ")" ) char * __asm_strcpy ( __reg ( "a0" ) char * , __reg ( "a1" ) const char * ) =
 "\tinline\n"
 "\tmove.l\ta0,d0\n"
 ".l1\n"
 "\tmove.b\t(a1)+,(a0)+\n"
 "\tbne\t.l1\n"
 "\teinline" ;
 char * __asm_strncpy ( __reg ( "a0" ) char * , __reg ( "a1" ) const char * ,
 __reg ( "d1" ) size_t ) =
 "\tinline\n"
 "\tmove.l\ta0,d0\n"
 "\tbra\t.l2\n"
 ".l1\n"
 "\tmove.b\t(a1),(a0)+\n"
 "\tbeq\t.l2\n"
 "\taddq.l\t#1,a1\n"
 ".l2\n"
 "\tsubq.l\t#1,d1\n"
 "\tbpl\t.l1\n"
 "\teinline" ;
 int __asm_strcmp ( __reg ( "a0" ) const char * , __reg ( "a1" ) const char * ) =
 "\tinline\n"
 "\tmoveq\t#0,d0\n"
 "\tmoveq\t#0,d1\n"
 ".l1\n"
 "\tmove.b\t(a0)+,d0\n"
 "\tmove.b\t(a1)+,d1\n"
 "\tbeq\t.l2\n"
 "\tsub.l\td1,d0\n"
 "\tbeq\t.l1\n"
 "\tmoveq\t#0,d1\n"
 ".l2\n"
 "\tsub.l\td1,d0\n"
 "\teinline" ;
 int __asm_strncmp ( __reg ( "a0" ) const char * , __reg ( "a1" ) const char * ,
 __reg ( "d2" ) size_t ) =
 "\tinline\n"
 "\tmoveq\t#0,d0\n"
 "\tmoveq\t#0,d1\n"
 ".l1\n"
 "\tsubq.l\t#1,d2\n"
 "\tbmi\t.l3\n"
 "\tmove.b\t(a0)+,d0\n"
 "\tmove.b\t(a1)+,d1\n"
 "\tbeq\t.l2\n"
 "\tsub.l\td1,d0\n"
 "\tbeq\t.l1\n"
 "\tmoveq\t#0,d1\n"
 ".l2\n"
 "\tsub.l\td1,d0\n"
 ".l3\n"
 "\teinline" ;
 __vattr ( "regused(" "d0/a0/a1" ")" ) char * __asm_strcat ( __reg ( "a0" ) char * , __reg ( "a1" ) const char * ) =
 "\tinline\n"
 "\tmove.l\ta0,d0\n"
 ".l1\n"
 "\ttst.b\t(a0)+\n"
 "\tbne\t.l1\n"
 "\tsubq.l\t#1,a0\n"
 ".l2\n"
 "\tmove.b\t(a1)+,(a0)+\n"
 "\tbne\t.l2\n"
 "\teinline" ;
 char * __asm_strncat ( __reg ( "a0" ) char * , __reg ( "a1" ) const char * ,
 __reg ( "d1" ) size_t ) =
 "\tinline\n"
 "\tmove.l\ta0,d0\n"
 "\ttst.l\td1\n"
 "\tbeq\t.l4\n"
 ".l1\n"
 "\ttst.b\t(a0)+\n"
 "\tbne\t.l1\n"
 "\tsubq.l\t#1,a0\n"
 ".l2\n"
 "\tmove.b\t(a1)+,(a0)+\n"
 "\tbeq\t.l3\n"
 "\tsubq.l\t#1,d1\n"
 "\tbne\t.l2\n"
 ".l3\n"
 "\tclr.b\t(a0)\n"
 ".l4\n"
 "\teinline" ;
 __vattr ( "regused(" "d0/d1/a0" ")" ) char * __asm_strrchr ( __reg ( "a0" ) const char * , __reg ( "d1" ) int ) =
 "\tinline\n"
 "\tmoveq\t#0,d0\n"
 ".l1\n"
 "\tcmp.b\t(a0),d1\n"
 "\tbne\t.l2\n"
 "\tmove.l\ta0,d0\n"
 ".l2\n"
 "\ttst.b\t(a0)+\n"
 "\tbne\t.l1\n"
 "\teinline" ;
#line 3 "exec\nodes.h"
 struct Node {
 struct Node * ln_Succ ;
 struct Node * ln_Pred ;
 UBYTE ln_Type ;
 BYTE ln_Pri ;
 char * ln_Name ;
 } ;
 struct MinNode {
 struct MinNode * mln_Succ ;
 struct MinNode * mln_Pred ;
 } ;
#line 6 "exec\memory.h"
 struct MemChunk {
 struct MemChunk * mc_Next ;
 ULONG mc_Bytes ;
 } ;
 struct MemHeader {
 struct Node mh_Node ;
 UWORD mh_Attributes ;
 struct MemChunk * mh_First ;
 APTR mh_Lower ;
 APTR mh_Upper ;
 ULONG mh_Free ;
 } ;
 struct MemEntry {
 union {
 ULONG meu_Reqs ;
 APTR meu_Addr ;
 } me_Un ;
 ULONG me_Length ;
 } ;



 struct MemList {
 struct Node ml_Node ;
 UWORD ml_NumEntries ;
 struct MemEntry ml_ME [ 1 ] ;
 } ;
#line 6 "exec\lists.h"
 struct List {
 struct Node * lh_Head ;
 struct Node * lh_Tail ;
 struct Node * lh_TailPred ;
 UBYTE lh_Type ;
 UBYTE l_pad ;
 } ;
 struct MinList {
 struct MinNode * mlh_Head ;
 struct MinNode * mlh_Tail ;
 struct MinNode * mlh_TailPred ;
 } ;
#line 9 "exec\tasks.h"
 struct Task {
 struct Node tc_Node ;
 UBYTE tc_Flags ;
 UBYTE tc_State ;
 BYTE tc_IDNestCnt ;
 BYTE tc_TDNestCnt ;
 ULONG tc_SigAlloc ;
 ULONG tc_SigWait ;
 ULONG tc_SigRecvd ;
 ULONG tc_SigExcept ;
 UWORD tc_TrapAlloc ;
 UWORD tc_TrapAble ;
 APTR tc_ExceptData ;
 APTR tc_ExceptCode ;
 APTR tc_TrapData ;
 APTR tc_TrapCode ;
 APTR tc_SPReg ;
 APTR tc_SPLower ;
 APTR tc_SPUpper ;
 void ( * tc_Switch ) ( ) ;
 void ( * tc_Launch ) ( ) ;
 struct List tc_MemEntry ;
 APTR tc_UserData ;
 } ;
#line 15 "exec\libraries.h"
 struct Library {
 struct Node lib_Node ;
 UBYTE lib_Flags ;
 UBYTE lib_pad ;
 UWORD lib_NegSize ;
 UWORD lib_PosSize ;
 UWORD lib_Version ;
 UWORD lib_Revision ;
 APTR lib_IdString ;
 ULONG lib_Sum ;
 UWORD lib_OpenCnt ;
 } ;
#line 25 "libraries\dos.h"
 struct DateStamp {
 LONG ds_Days ;
 LONG ds_Minute ;
 LONG ds_Tick ;
 } ;

 struct FileInfoBlock {
 LONG fib_DiskKey ;
 LONG fib_DirEntryType ;
 char fib_FileName [ 108 ] ;
 LONG fib_Protection ;
 LONG fib_EntryType ;
 LONG fib_Size ;
 LONG fib_NumBlocks ;
 struct DateStamp fib_Date ;
 char fib_Comment [ 80 ] ;
 char fib_Reserved [ 36 ] ;
 } ;














 typedef long BPTR ;
 typedef long BSTR ;





 struct InfoData {
 LONG id_NumSoftErrors ;
 LONG id_UnitNumber ;
 LONG id_DiskState ;
 LONG id_NumBlocks ;
 LONG id_NumBlocksUsed ;
 LONG id_BytesPerBlock ;
 LONG id_DiskType ;
 BPTR id_VolumeNode ;
 LONG id_InUse ;
 } ;
#line 9 "exec\interrupts.h"
 struct Interrupt {
 struct Node is_Node ;
 APTR is_Data ;
 void ( * is_Code ) ( ) ;
 } ;
 struct IntVector {
 APTR iv_Data ;
 void ( * iv_Code ) ( ) ;
 struct Node * iv_Node ;
 } ;
 struct SoftIntList {
 struct List sh_List ;
 UWORD sh_Pad ;
 } ;
#line 12 "graphics\gfxbase.h"
 struct GfxBase
 {
 struct Library LibNode ;
 struct View * ActiView ;
 struct copinit * copinit ;
 long * cia ;
 long * blitter ;
 UWORD * LOFlist ;
 UWORD * SHFlist ;
 struct bltnode * blthd , * blttl ;
 struct bltnode * bsblthd , * bsblttl ;
 struct Interrupt vbsrv , timsrv , bltsrv ;
 struct List TextFonts ;
 struct TextFont * DefaultFont ;
 UWORD Modes ;
 BYTE VBlank ;
 BYTE Debug ;
 SHORT BeamSync ;
 SHORT system_bplcon0 ;
 UBYTE SpriteReserved ;
 UBYTE bytereserved ;
 USHORT Flags ;
 SHORT BlitLock ;
 short BlitNest ;
 struct List BlitWaitQ ;
 struct Task * BlitOwner ;
 struct List TOF_WaitQ ;
 UWORD DisplayFlags ;
 struct SimpleSprite * * SimpleSprites ;
 UWORD MaxDisplayRow ;
 UWORD MaxDisplayColumn ;
 UWORD NormalDisplayRows ;
 UWORD NormalDisplayColumns ;
 UWORD NormalDPMX ;
 UWORD NormalDPMY ;
 struct SignalSemaphore * LastChanceMemory ;
 UWORD * LCMptr ;
 UWORD MicrosPerLine ;
 UWORD MinDisplayColumn ;
 ULONG reserved [ 23 ] ;
 } ;
#line 11 "graphics\gfx.h"
 struct Rectangle
 {
 SHORT MinX , MinY ;
 SHORT MaxX , MaxY ;
 } ;
 typedef struct tPoint
 {
 WORD x , y ;
 } Point ;
 typedef UBYTE * PLANEPTR ;
 struct BitMap
 {
 UWORD BytesPerRow ;
 UWORD Rows ;
 UBYTE Flags ;
 UBYTE Depth ;
 UWORD pad ;
 PLANEPTR Planes [ 8 ] ;
 } ;
#line 12 "exec\ports.h"
 struct MsgPort {
 struct Node mp_Node ;
 UBYTE mp_Flags ;
 UBYTE mp_SigBit ;
 struct Task * mp_SigTask ;
 struct List mp_MsgList ;
 } ;





 struct Message {
 struct Node mn_Node ;
 struct MsgPort * mn_ReplyPort ;
 UWORD mn_Length ;
 } ;
#line 15 "exec\semaphores.h"
 struct Semaphore {
 struct MsgPort sm_MsgPort ;
 WORD sm_Bids ;
 } ;

 struct SemaphoreRequest {
 struct MinNode sr_Link ;
 struct Task * sr_Waiter ;
 } ;
 struct SignalSemaphore {
 struct Node ss_Link ;
 SHORT ss_NestCount ;
 struct MinList ss_WaitQueue ;
 struct SemaphoreRequest ss_MultipleLink ;
 struct Task * ss_Owner ;
 SHORT ss_QueueCount ;
 } ;
#line 10 "graphics\clip.h"
 struct Layer
 {
 struct Layer * front , * back ;
 struct ClipRect * ClipRect ;
 struct RastPort * rp ;
 struct Rectangle bounds ;
 UBYTE reserved [ 4 ] ;
 UWORD priority ;
 UWORD Flags ;
 struct BitMap * SuperBitMap ;
 struct ClipRect * SuperClipRect ;
 APTR Window ;
 SHORT Scroll_X , Scroll_Y ;
 struct ClipRect * cr , * cr2 , * crnew ;
 struct ClipRect * SuperSaveClipRects ;
 struct ClipRect * _cliprects ;
 struct Layer_Info * LayerInfo ;
 struct SignalSemaphore Lock ;
 UBYTE reserved3 [ 8 ] ;
 struct Region * ClipRegion ;
 struct Region * saveClipRects ;
 UBYTE reserved2 [ 22 ] ;
 struct Region * DamageList ;
 } ;
 struct ClipRect
 {
 struct ClipRect * Next ;
 struct ClipRect * prev ;
 struct Layer * lobs ;
 struct BitMap * BitMap ;
 struct Rectangle bounds ;
 struct ClipRect * _p1 , * _p2 ;
 LONG reserved ;



 } ;
#line 8 "graphics\copper.h"
 struct CopIns
 {
 short OpCode ;
 union
 {
 struct CopList * nxtlist ;
 struct
 {
 union
 {
 SHORT VWaitPos ;
 SHORT DestAddr ;
 } u1 ;
 union
 {
 SHORT HWaitPos ;
 SHORT DestData ;
 } u2 ;
 } u4 ;
 } u3 ;
 } ;





 struct cprlist
 {
 struct cprlist * Next ;
 UWORD * start ;
 SHORT MaxCount ;
 } ;
 struct CopList
 {
 struct CopList * Next ;
 struct CopList * _CopList ;
 struct ViewPort * _ViewPort ;
 struct CopIns * CopIns ;
 struct CopIns * CopPtr ;
 UWORD * CopLStart ;
 UWORD * CopSStart ;
 SHORT Count ;
 SHORT MaxCount ;
 SHORT DyOffset ;
 } ;
 struct UCopList
 {
 struct UCopList * Next ;
 struct CopList * FirstCopList ;
 struct CopList * CopList ;
 } ;
 struct copinit
 {
 UWORD diagstrt [ 4 ] ;
 UWORD sprstrtup [ ( 2 * 8 * 2 ) + 2 + ( 2 * 2 ) + 2 ] ;
 UWORD sprstop [ 2 ] ;
 } ;
#line 9 "graphics\view.h"
 struct ColorMap
 {
 UBYTE Flags ;
 UBYTE Type ;
 UWORD Count ;
 APTR ColorTable ;
 } ;
 struct ViewPort
 {
 struct ViewPort * Next ;
 struct ColorMap * ColorMap ;
 struct CopList * DspIns ;
 struct CopList * SprIns ;
 struct CopList * ClrIns ;
 struct UCopList * UCopIns ;
 SHORT DWidth , DHeight ;
 SHORT DxOffset , DyOffset ;
 UWORD Modes ;
 UBYTE SpritePriorities ;
 UBYTE reserved ;
 struct RasInfo * RasInfo ;
 } ;
 struct View
 {
 struct ViewPort * ViewPort ;
 struct cprlist * LOFCprList ;
 struct cprlist * SHFCprList ;
 short DyOffset , DxOffset ;
 UWORD Modes ;
 } ;










 struct RasInfo
 {
 struct RasInfo * Next ;
 struct BitMap * BitMap ;
 SHORT RxOffset , RyOffset ;
 } ;
#line 6 "graphics\rastport.h"
 struct AreaInfo
 {
 SHORT * VctrTbl ;
 SHORT * VctrPtr ;
 BYTE * FlagTbl ;
 BYTE * FlagPtr ;
 SHORT Count ;
 SHORT MaxCount ;
 SHORT FirstX , FirstY ;
 } ;
 struct TmpRas
 {
 BYTE * RasPtr ;
 LONG Size ;
 } ;
 struct GelsInfo
 {
 BYTE sprRsrvd ;
 UBYTE Flags ;
 struct VSprite * gelHead , * gelTail ;
 WORD * nextLine ;
 WORD * * lastColor ;
 struct collTable * collHandler ;
 short leftmost , rightmost , topmost , bottommost ;
 APTR firstBlissObj , lastBlissObj ;
 } ;
 struct RastPort
 {
 struct Layer * Layer ;
 struct BitMap * BitMap ;
 USHORT * AreaPtrn ;
 struct TmpRas * TmpRas ;
 struct AreaInfo * AreaInfo ;
 struct GelsInfo * GelsInfo ;
 UBYTE Mask ;
 BYTE FgPen ;
 BYTE BgPen ;
 BYTE AOlPen ;
 BYTE DrawMode ;
 BYTE AreaPtSz ;
 BYTE linpatcnt ;
 BYTE dummy ;
 USHORT Flags ;
 USHORT LinePtrn ;
 SHORT cp_x , cp_y ;
 UBYTE minterms [ 8 ] ;
 SHORT PenWidth ;
 SHORT PenHeight ;
 struct TextFont * Font ;
 UBYTE AlgoStyle ;
 UBYTE TxFlags ;
 UWORD TxHeight ;
 UWORD TxWidth ;
 UWORD TxBaseline ;
 WORD TxSpacing ;
 APTR * RP_User ;
 ULONG longreserved [ 2 ] ;

 UWORD wordreserved [ 7 ] ;
 UBYTE reserved [ 8 ] ;

 } ;
#line 17 "graphics\layers.h"
 struct Layer_Info
 {
 struct Layer * top_layer ;
 struct Layer * check_lp ;
 struct Layer * obs ;
 struct MinList FreeClipRects ;
 struct SignalSemaphore Lock ;
 struct List gs_Head ;
 LONG longreserved ;
 UWORD Flags ;
 BYTE fatten_count ;
 BYTE LockLayersCount ;
 UWORD LayerInfo_extra_size ;
 WORD * blitbuff ;
 struct LayerInfo_extra * LayerInfo_extra ;
 } ;
#line 31 "graphics\text.h"
 struct TextAttr {
 STRPTR ta_Name ;
 UWORD ta_YSize ;
 UBYTE ta_Style ;
 UBYTE ta_Flags ;
 } ;
 struct TextFont {
 struct Message tf_Message ;
 UWORD tf_YSize ;
 UBYTE tf_Style ;
 UBYTE tf_Flags ;
 UWORD tf_XSize ;
 UWORD tf_Baseline ;
 UWORD tf_BoldSmear ;
 UWORD tf_Accessors ;
 UBYTE tf_LoChar ;
 UBYTE tf_HiChar ;
 APTR tf_CharData ;
 UWORD tf_Modulo ;
 APTR tf_CharLoc ;
 APTR tf_CharSpace ;
 APTR tf_CharKern ;
 } ;
#line 6 "exec\io.h"
 struct IORequest {
 struct Message io_Message ;
 struct Device * io_Device ;
 struct Unit * io_Unit ;
 UWORD io_Command ;
 UBYTE io_Flags ;
 BYTE io_Error ;
 } ;
 struct IOStdReq {
 struct Message io_Message ;
 struct Device * io_Device ;
 struct Unit * io_Unit ;
 UWORD io_Command ;
 UBYTE io_Flags ;
 BYTE io_Error ;
 ULONG io_Actual ;
 ULONG io_Length ;
 APTR io_Data ;
 ULONG io_Offset ;
 } ;
#line 9 "devices\timer.h"
 struct timeval {
 ULONG tv_secs ;
 ULONG tv_micro ;
 } ;
 struct timerequest {
 struct IORequest tr_node ;
 struct timeval tr_time ;
 } ;
#line 78 "devices\inputevent.h"
 struct InputEvent {
 struct InputEvent * ie_NextEvent ;
 UBYTE ie_Class ;
 UBYTE ie_SubClass ;
 UWORD ie_Code ;
 UWORD ie_Qualifier ;
 union {
 struct {
 WORD ie_x ;
 WORD ie_y ;
 } ie_xy ;
 APTR ie_addr ;
 } ie_position ;
 struct timeval ie_TimeStamp ;
 } ;
#line 33 "intuition\intuition.h"
 struct Menu
 {
 struct Menu * NextMenu ;
 SHORT LeftEdge , TopEdge ;
 SHORT Width , Height ;
 USHORT Flags ;
 BYTE * MenuName ;
 struct MenuItem * FirstItem ;
 SHORT JazzX , JazzY , BeatX , BeatY ;
 } ;


 struct MenuItem
 {
 struct MenuItem * NextItem ;
 SHORT LeftEdge , TopEdge ;
 SHORT Width , Height ;
 USHORT Flags ;
 LONG MutualExclude ;
 APTR ItemFill ;
 APTR SelectFill ;
 BYTE Command ;
 struct MenuItem * SubItem ;
 USHORT NextSelect ;
 } ;














 struct Requester
 {
 struct Requester * OlderRequest ;
 SHORT LeftEdge , TopEdge ;
 SHORT Width , Height ;
 SHORT RelLeft , RelTop ;
 struct Gadget * ReqGadget ;
 struct Border * ReqBorder ;
 struct IntuiText * ReqText ;
 USHORT Flags ;
 UBYTE BackFill ;
 struct Layer * ReqLayer ;
 UBYTE ReqPad1 [ 32 ] ;
 struct BitMap * ImageBMap ;
 struct Window * RWindow ;
 UBYTE ReqPad2 [ 36 ] ;
 } ;







 struct Gadget
 {
 struct Gadget * NextGadget ;
 SHORT LeftEdge , TopEdge ;
 SHORT Width , Height ;
 USHORT Flags ;
 USHORT Activation ;
 USHORT GadgetType ;
 APTR GadgetRender ;
 APTR SelectRender ;
 struct IntuiText * GadgetText ;
 LONG MutualExclude ;
 APTR SpecialInfo ;
 USHORT GadgetID ;
 APTR UserData ;
 } ;











































 struct BoolInfo
 {
 USHORT Flags ;
 UWORD * Mask ;
 ULONG Reserved ;
 } ;

 struct PropInfo
 {
 USHORT Flags ;
 USHORT HorizPot ;
 USHORT VertPot ;
 USHORT HorizBody ;
 USHORT VertBody ;
 USHORT CWidth ;
 USHORT CHeight ;
 USHORT HPotRes , VPotRes ;
 USHORT LeftBorder ;
 USHORT TopBorder ;
 } ;









 struct StringInfo
 {
 UBYTE * Buffer ;
 UBYTE * UndoBuffer ;
 SHORT BufferPos ;
 SHORT MaxChars ;
 SHORT DispPos ;
 SHORT UndoPos ;
 SHORT NumChars ;
 SHORT DispCount ;
 SHORT CLeft , CTop ;
 struct Layer * LayerPtr ;
 LONG LongInt ;
 struct KeyMap * AltKeyMap ;
 } ;
 struct IntuiText
 {
 UBYTE FrontPen , BackPen ;
 UBYTE DrawMode ;
 SHORT LeftEdge ;
 SHORT TopEdge ;
 struct TextAttr * ITextFont ;
 UBYTE * IText ;
 struct IntuiText * NextText ;
 } ;
 struct Border
 {
 SHORT LeftEdge , TopEdge ;
 UBYTE FrontPen , BackPen ;
 UBYTE DrawMode ;
 BYTE Count ;
 SHORT * XY ;
 struct Border * NextBorder ;
 } ;
 struct Image
 {
 SHORT LeftEdge ;
 SHORT TopEdge ;
 SHORT Width ;
 SHORT Height , Depth ;
 USHORT * ImageData ;
 UBYTE PlanePick , PlaneOnOff ;
 struct Image * NextImage ;
 } ;
 struct IntuiMessage
 {
 struct Message ExecMessage ;
 ULONG Class ;
 USHORT Code ;
 USHORT Qualifier ;
 APTR IAddress ;
 SHORT MouseX , MouseY ;
 ULONG Seconds , Micros ;
 struct Window * IDCMPWindow ;
 struct IntuiMessage * SpecialLink ;
 } ;
































 struct Window
 {
 struct Window * NextWindow ;
 SHORT LeftEdge , TopEdge ;
 SHORT Width , Height ;
 SHORT MouseY , MouseX ;
 SHORT MinWidth , MinHeight ;
 USHORT MaxWidth , MaxHeight ;
 ULONG Flags ;
 struct Menu * MenuStrip ;
 UBYTE * Title ;
 struct Requester * FirstRequest ;
 struct Requester * DMRequest ;
 SHORT ReqCount ;
 struct Screen * WScreen ;
 struct RastPort * RPort ;
 BYTE BorderLeft , BorderTop , BorderRight , BorderBottom ;
 struct RastPort * BorderRPort ;
 struct Gadget * FirstGadget ;
 struct Window * Parent , * Descendant ;
 USHORT * Pointer ;
 BYTE PtrHeight ;
 BYTE PtrWidth ;
 BYTE XOffset , YOffset ;
 ULONG IDCMPFlags ;
 struct MsgPort * UserPort , * WindowPort ;
 struct IntuiMessage * MessageKey ;
 UBYTE DetailPen , BlockPen ;
 struct Image * CheckMark ;
 UBYTE * ScreenTitle ;
 SHORT GZZMouseX ;
 SHORT GZZMouseY ;
 SHORT GZZWidth ;
 SHORT GZZHeight ;
 UBYTE * ExtData ;
 BYTE * UserData ;
 struct Layer * WLayer ;
 struct TextFont * IFont ;
 } ;

























 struct NewWindow
 {
 SHORT LeftEdge , TopEdge ;
 SHORT Width , Height ;
 UBYTE DetailPen , BlockPen ;
 ULONG IDCMPFlags ;
 ULONG Flags ;
 struct Gadget * FirstGadget ;
 struct Image * CheckMark ;
 UBYTE * Title ;
 struct Screen * Screen ;
 struct BitMap * BitMap ;
 SHORT MinWidth , MinHeight ;
 USHORT MaxWidth , MaxHeight ;
 USHORT Type ;
 } ;
#line 21 "intuition\screens.h"
 struct Screen
 {
 struct Screen * NextScreen ;
 struct Window * FirstWindow ;
 SHORT LeftEdge , TopEdge ;
 SHORT Width , Height ;
 SHORT MouseY , MouseX ;
 USHORT Flags ;
 UBYTE * Title ;
 UBYTE * DefaultTitle ;
 BYTE BarHeight , BarVBorder , BarHBorder , MenuVBorder , MenuHBorder ;
 BYTE WBorTop , WBorLeft , WBorRight , WBorBottom ;
 struct TextAttr * Font ;
 struct ViewPort ViewPort ;
 struct RastPort RastPort ;
 struct BitMap BitMap ;
 struct Layer_Info LayerInfo ;
 struct Gadget * FirstGadget ;
 UBYTE DetailPen , BlockPen ;
 USHORT SaveColor0 ;
 struct Layer * BarLayer ;
 UBYTE * ExtData ;
 UBYTE * UserData ;
 } ;









 struct NewScreen
 {
 SHORT LeftEdge , TopEdge , Width , Height , Depth ;
 UBYTE DetailPen , BlockPen ;
 USHORT ViewModes ;
 USHORT Type ;
 struct TextAttr * Font ;
 UBYTE * DefaultTitle ;
 struct Gadget * Gadgets ;
 struct BitMap * CustomBitMap ;
 } ;
#line 13 "intuition\preferences.h"
 struct Preferences
 {
 BYTE FontHeight ;
 UBYTE PrinterPort ;
 USHORT BaudRate ;
 struct timeval KeyRptSpeed ;
 struct timeval KeyRptDelay ;
 struct timeval DoubleClick ;
 USHORT PointerMatrix [ ( 1 + 16 + 1 ) * 2 ] ;
 BYTE XOffset ;
 BYTE YOffset ;
 USHORT color17 ;
 USHORT color18 ;
 USHORT color19 ;
 USHORT PointerTicks ;
 USHORT color0 ;
 USHORT color1 ;
 USHORT color2 ;
 USHORT color3 ;
 BYTE ViewXOffset ;
 BYTE ViewYOffset ;
 WORD ViewInitX , ViewInitY ;
 BOOL EnableCLI ;
 USHORT PrinterType ;
 UBYTE PrinterFilename [ 30 ] ;
 USHORT PrintPitch ;
 USHORT PrintQuality ;
 USHORT PrintSpacing ;
 UWORD PrintLeftMargin ;
 UWORD PrintRightMargin ;
 USHORT PrintImage ;
 USHORT PrintAspect ;
 USHORT PrintShade ;
 WORD PrintThreshold ;
 USHORT PaperSize ;
 UWORD PaperLength ;
 USHORT PaperType ;
 UBYTE SerRWBits ;
 UBYTE SerStopBuf ;
 UBYTE SerParShk ;
 UBYTE LaceWB ;
 UBYTE WorkName [ 30 ] ;
 BYTE RowSizeChange ;
 BYTE ColumnSizeChange ;
 UWORD PrintFlags ;
 UWORD PrintMaxWidth ;
 UWORD PrintMaxHeight ;
 UBYTE PrintDensity ;
 UBYTE PrintXOffset ;
 UWORD wb_Width ;
 UWORD wb_Height ;
 UBYTE wb_Depth ;
 UBYTE ext_size ;
 } ;
#line 358 "intuition\intuition.h"
 struct Remember
 {
 struct Remember * NextRemember ;
 ULONG RememberSize ;
 UBYTE * Memory ;
 } ;
#line 78 "intuition\intuitionbase.h"
 struct IntuitionBase
 {
 struct Library LibNode ;
 struct View ViewLord ;
 struct Window * ActiveWindow ;
 struct Screen * ActiveScreen ;
 struct Screen * FirstScreen ;
 ULONG Flags ;
 WORD MouseY , MouseX ;
 ULONG Seconds ;
 ULONG Micros ;

































































































 } ;
#line 3 "hardware\custom.h"
 struct Custom {
 UWORD bltddat ;
 UWORD dmaconr ;
 UWORD vposr ;
 UWORD vhposr ;
 UWORD dskdatr ;
 UWORD joy0dat ;
 UWORD joy1dat ;
 UWORD clxdat ;
 UWORD adkconr ;
 UWORD pot0dat ;
 UWORD pot1dat ;
 UWORD potinp ;
 UWORD serdatr ;
 UWORD dskbytr ;
 UWORD intenar ;
 UWORD intreqr ;
 APTR dskpt ;
 UWORD dsklen ;
 UWORD dskdat ;
 UWORD refptr ;
 UWORD vposw ;
 UWORD vhposw ;
 UWORD copcon ;
 UWORD serdat ;
 UWORD serper ;
 UWORD potgo ;
 UWORD joytest ;
 UWORD strequ ;
 UWORD strvbl ;
 UWORD strhor ;
 UWORD strlong ;
 UWORD bltcon0 ;
 UWORD bltcon1 ;
 UWORD bltafwm ;
 UWORD bltalwm ;
 APTR bltcpt ;
 APTR bltbpt ;
 APTR bltapt ;
 APTR bltdpt ;
 UWORD bltsize ;
 UWORD pad2d [ 3 ] ;
 UWORD bltcmod ;
 UWORD bltbmod ;
 UWORD bltamod ;
 UWORD bltdmod ;
 UWORD pad34 [ 4 ] ;
 UWORD bltcdat ;
 UWORD bltbdat ;
 UWORD bltadat ;
 UWORD pad3b [ 4 ] ;
 UWORD dsksync ;
 ULONG cop1lc ;
 ULONG cop2lc ;
 UWORD copjmp1 ;
 UWORD copjmp2 ;
 UWORD copins ;
 UWORD diwstrt ;
 UWORD diwstop ;
 UWORD ddfstrt ;
 UWORD ddfstop ;
 UWORD dmacon ;
 UWORD clxcon ;
 UWORD intena ;
 UWORD intreq ;
 UWORD adkcon ;
 struct AudChannel {
 UWORD * ac_ptr ;
 UWORD ac_len ;
 UWORD ac_per ;
 UWORD ac_vol ;
 UWORD ac_dat ;
 UWORD ac_pad [ 2 ] ;
 } aud [ 4 ] ;
 APTR bplpt [ 6 ] ;
 UWORD pad7c [ 4 ] ;
 UWORD bplcon0 ;
 UWORD bplcon1 ;
 UWORD bplcon2 ;
 UWORD pad83 ;
 UWORD bpl1mod ;
 UWORD bpl2mod ;
 UWORD pad86 [ 2 ] ;
 UWORD bpldat [ 6 ] ;
 UWORD pad8e [ 2 ] ;
 APTR sprpt [ 8 ] ;
 struct SpriteDef {
 UWORD pos ;
 UWORD ctl ;
 UWORD dataa ;
 UWORD datab ;
 } spr [ 8 ] ;
 UWORD color [ 32 ] ;
 } ;
#line 8 "proto\exec.h"
#pragma stdargs-on
#line 9 "exec\devices.h"
 struct Device {
 struct Library dd_Library ;
 } ;
 struct Unit {
 struct MsgPort unit_MsgPort ;
 UBYTE unit_flags ;
 UBYTE unit_pad ;
 UWORD unit_OpenCnt ;
 } ;
#line 36 "clib\exec_protos.h"
 ULONG Supervisor ( ULONG ( * userFunction ) ( ) ) ;
 void InitCode ( ULONG startClass , ULONG version ) ;
 void InitStruct ( const void * initTable , void * memory , ULONG size ) ;
 struct Library * MakeLibrary ( const void * funcInit , const void * structInit ,
 ULONG ( * libInit ) ( ) , ULONG dataSize , ULONG segList ) ;
 void MakeFunctions ( void * target , const void * functionArray , const void * funcDispBase ) ;
 struct Resident * FindResident ( const STRPTR name ) ;
 void * InitResident ( const struct Resident * resident , ULONG segList ) ;
 void Alert ( ULONG alertNum , ULONG * parameter ) ;
 void Debug ( ULONG flags ) ;
 void Disable ( void ) ;
 void Enable ( void ) ;
 void Forbid ( void ) ;
 void Permit ( void ) ;
 ULONG SetSR ( ULONG newSR , ULONG mask ) ;
 void * SuperState ( void ) ;
 void UserState ( void * sysStack ) ;
 struct Interrupt * SetIntVector ( LONG intNumber , const struct Interrupt * interrupt ) ;
 void AddIntServer ( LONG intNumber , struct Interrupt * interrupt ) ;
 void RemIntServer ( LONG intNumber , struct Interrupt * interrupt ) ;
 void Cause ( struct Interrupt * interrupt ) ;
 void * Allocate ( struct MemHeader * freeList , ULONG byteSize ) ;
 void Deallocate ( struct MemHeader * freeList , void * memoryBlock , ULONG byteSize ) ;
 void * AllocMem ( ULONG byteSize , ULONG requirements ) ;
 void * AllocAbs ( ULONG byteSize , void * location ) ;
 void FreeMem ( void * memoryBlock , ULONG byteSize ) ;
 ULONG AvailMem ( ULONG requirements ) ;
 struct MemList * AllocEntry ( struct MemList * entry ) ;
 void FreeEntry ( struct MemList * entry ) ;
 void Insert ( struct List * list , struct Node * node , struct Node * pred ) ;
 void AddHead ( struct List * list , struct Node * node ) ;
 void AddTail ( struct List * list , struct Node * node ) ;
 void Remove ( struct Node * node ) ;
 struct Node * RemHead ( struct List * list ) ;
 struct Node * RemTail ( struct List * list ) ;
 void Enqueue ( struct List * list , struct Node * node ) ;
 struct Node * FindName ( struct List * list , const STRPTR name ) ;
 void * AddTask ( struct Task * task , const void * initPC , const void * finalPC ) ;
 void RemTask ( struct Task * task ) ;
 struct Task * FindTask ( const STRPTR name ) ;
 BYTE SetTaskPri ( struct Task * task , LONG priority ) ;
 ULONG SetSignal ( ULONG newSignals , ULONG signalSet ) ;
 ULONG SetExcept ( ULONG newSignals , ULONG signalSet ) ;
 ULONG Wait ( ULONG signalSet ) ;
 void Signal ( struct Task * task , ULONG signalSet ) ;
 BYTE AllocSignal ( LONG signalNum ) ;
 void FreeSignal ( LONG signalNum ) ;
 LONG AllocTrap ( LONG trapNum ) ;
 void FreeTrap ( LONG trapNum ) ;
 void AddPort ( struct MsgPort * port ) ;
 void RemPort ( struct MsgPort * port ) ;
 void PutMsg ( struct MsgPort * port , struct Message * message ) ;
 struct Message * GetMsg ( struct MsgPort * port ) ;
 void ReplyMsg ( struct Message * message ) ;
 struct Message * WaitPort ( struct MsgPort * port ) ;
 struct MsgPort * FindPort ( const STRPTR name ) ;
 void AddLibrary ( struct Library * library ) ;
 void RemLibrary ( struct Library * library ) ;
 struct Library * OldOpenLibrary ( const STRPTR libName ) ;
 void CloseLibrary ( struct Library * library ) ;
 void * SetFunction ( struct Library * library , LONG funcOffset , ULONG ( * newFunction ) ( ) ) ;
 void SumLibrary ( struct Library * library ) ;
 void AddDevice ( struct Device * device ) ;
 void RemDevice ( struct Device * device ) ;
 BYTE OpenDevice ( const STRPTR devName , ULONG unit , struct IORequest * ioRequest ,
 ULONG flags ) ;
 void CloseDevice ( struct IORequest * ioRequest ) ;
 BYTE DoIO ( struct IORequest * ioRequest ) ;
 void SendIO ( struct IORequest * ioRequest ) ;
 BOOL CheckIO ( struct IORequest * ioRequest ) ;
 BYTE WaitIO ( struct IORequest * ioRequest ) ;
 void AbortIO ( struct IORequest * ioRequest ) ;
 void AddResource ( void * resource ) ;
 void RemResource ( void * resource ) ;
 void * OpenResource ( const STRPTR resName ) ;
 void * RawDoFmt ( const STRPTR formatString , const void * dataStream ,
 void ( * putChProc ) ( ) , void * putChData ) ;
 ULONG GetCC ( void ) ;
 ULONG TypeOfMem ( const void * address ) ;
 ULONG Procure ( struct SignalSemaphore * sigSem , struct SemaphoreMessage * bidMsg ) ;
 void Vacate ( struct SignalSemaphore * sigSem , struct SemaphoreMessage * bidMsg ) ;
 struct Library * OpenLibrary ( const STRPTR libName , ULONG version ) ;
 void InitSemaphore ( struct SignalSemaphore * sigSem ) ;
 void ObtainSemaphore ( struct SignalSemaphore * sigSem ) ;
 void ReleaseSemaphore ( struct SignalSemaphore * sigSem ) ;
 ULONG AttemptSemaphore ( struct SignalSemaphore * sigSem ) ;
 void ObtainSemaphoreList ( struct List * sigSem ) ;
 void ReleaseSemaphoreList ( struct List * sigSem ) ;
 struct SignalSemaphore * FindSemaphore ( STRPTR name ) ;
 void AddSemaphore ( struct SignalSemaphore * sigSem ) ;
 void RemSemaphore ( struct SignalSemaphore * sigSem ) ;
 ULONG SumKickData ( void ) ;
 void AddMemList ( ULONG size , ULONG attributes , LONG pri , void * base , const STRPTR name ) ;
 void CopyMem ( const void * source , void * dest , ULONG size ) ;
 void CopyMemQuick ( const void * source , void * dest , ULONG size ) ;
#line 10 "proto\exec.h"
#pragma stdargs-off



 extern struct ExecBase * SysBase ;
#line 8 "inline\exec_protos.h"
 ULONG __Supervisor ( __reg ( "a6" ) void * , __reg ( "a5" ) ULONG ( * userFunction ) ( ) ) = "\tjsr\t-30(a6)" ;


 void __InitCode ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG startClass , __reg ( "d1" ) ULONG version ) = "\tjsr\t-72(a6)" ;


 void __InitStruct ( __reg ( "a6" ) void * , __reg ( "a1" ) const void * initTable , __reg ( "a2" ) void * memory , __reg ( "d0" ) ULONG size ) = "\tjsr\t-78(a6)" ;


 struct Library * __MakeLibrary ( __reg ( "a6" ) void * , __reg ( "a0" ) const void * funcInit , __reg ( "a1" ) const void * structInit , __reg ( "a2" ) ULONG ( * libInit ) ( ) , __reg ( "d0" ) ULONG dataSize , __reg ( "d1" ) ULONG segList ) = "\tjsr\t-84(a6)" ;


 void __MakeFunctions ( __reg ( "a6" ) void * , __reg ( "a0" ) void * target , __reg ( "a1" ) const void * functionArray , __reg ( "a2" ) const void * funcDispBase ) = "\tjsr\t-90(a6)" ;


 struct Resident * __FindResident ( __reg ( "a6" ) void * , __reg ( "a1" ) const STRPTR name ) = "\tjsr\t-96(a6)" ;


 void * __InitResident ( __reg ( "a6" ) void * , __reg ( "a1" ) const struct Resident * resident , __reg ( "d1" ) ULONG segList ) = "\tjsr\t-102(a6)" ;


 void __Alert ( __reg ( "a6" ) void * , __reg ( "d7" ) ULONG alertNum , __reg ( "a5" ) ULONG * parameter ) = "\tjsr\t-108(a6)" ;


 void __Debug ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG flags ) = "\tjsr\t-114(a6)" ;


 void __Disable ( __reg ( "a6" ) void * ) = "\tjsr\t-120(a6)" ;


 void __Enable ( __reg ( "a6" ) void * ) = "\tjsr\t-126(a6)" ;


 void __Forbid ( __reg ( "a6" ) void * ) = "\tjsr\t-132(a6)" ;


 void __Permit ( __reg ( "a6" ) void * ) = "\tjsr\t-138(a6)" ;


 ULONG __SetSR ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG newSR , __reg ( "d1" ) ULONG mask ) = "\tjsr\t-144(a6)" ;


 void * __SuperState ( __reg ( "a6" ) void * ) = "\tjsr\t-150(a6)" ;


 void __UserState ( __reg ( "a6" ) void * , __reg ( "d0" ) void * sysStack ) = "\tjsr\t-156(a6)" ;


 struct Interrupt * __SetIntVector ( __reg ( "a6" ) void * , __reg ( "d0" ) LONG intNumber , __reg ( "a1" ) const struct Interrupt * interrupt ) = "\tjsr\t-162(a6)" ;


 void __AddIntServer ( __reg ( "a6" ) void * , __reg ( "d0" ) LONG intNumber , __reg ( "a1" ) struct Interrupt * interrupt ) = "\tjsr\t-168(a6)" ;


 void __RemIntServer ( __reg ( "a6" ) void * , __reg ( "d0" ) LONG intNumber , __reg ( "a1" ) struct Interrupt * interrupt ) = "\tjsr\t-174(a6)" ;


 void __Cause ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Interrupt * interrupt ) = "\tjsr\t-180(a6)" ;


 void * __Allocate ( __reg ( "a6" ) void * , __reg ( "a0" ) struct MemHeader * freeList , __reg ( "d0" ) ULONG byteSize ) = "\tjsr\t-186(a6)" ;


 void __Deallocate ( __reg ( "a6" ) void * , __reg ( "a0" ) struct MemHeader * freeList , __reg ( "a1" ) void * memoryBlock , __reg ( "d0" ) ULONG byteSize ) = "\tjsr\t-192(a6)" ;


 void * __AllocMem ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG byteSize , __reg ( "d1" ) ULONG requirements ) = "\tjsr\t-198(a6)" ;


 void * __AllocAbs ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG byteSize , __reg ( "a1" ) void * location ) = "\tjsr\t-204(a6)" ;


 void __FreeMem ( __reg ( "a6" ) void * , __reg ( "a1" ) void * memoryBlock , __reg ( "d0" ) ULONG byteSize ) = "\tjsr\t-210(a6)" ;


 ULONG __AvailMem ( __reg ( "a6" ) void * , __reg ( "d1" ) ULONG requirements ) = "\tjsr\t-216(a6)" ;


 struct MemList * __AllocEntry ( __reg ( "a6" ) void * , __reg ( "a0" ) struct MemList * entry ) = "\tjsr\t-222(a6)" ;


 void __FreeEntry ( __reg ( "a6" ) void * , __reg ( "a0" ) struct MemList * entry ) = "\tjsr\t-228(a6)" ;


 void __Insert ( __reg ( "a6" ) void * , __reg ( "a0" ) struct List * list , __reg ( "a1" ) struct Node * node , __reg ( "a2" ) struct Node * pred ) = "\tjsr\t-234(a6)" ;


 void __AddHead ( __reg ( "a6" ) void * , __reg ( "a0" ) struct List * list , __reg ( "a1" ) struct Node * node ) = "\tjsr\t-240(a6)" ;


 void __AddTail ( __reg ( "a6" ) void * , __reg ( "a0" ) struct List * list , __reg ( "a1" ) struct Node * node ) = "\tjsr\t-246(a6)" ;


 void __Remove ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Node * node ) = "\tjsr\t-252(a6)" ;


 struct Node * __RemHead ( __reg ( "a6" ) void * , __reg ( "a0" ) struct List * list ) = "\tjsr\t-258(a6)" ;


 struct Node * __RemTail ( __reg ( "a6" ) void * , __reg ( "a0" ) struct List * list ) = "\tjsr\t-264(a6)" ;


 void __Enqueue ( __reg ( "a6" ) void * , __reg ( "a0" ) struct List * list , __reg ( "a1" ) struct Node * node ) = "\tjsr\t-270(a6)" ;


 struct Node * __FindName ( __reg ( "a6" ) void * , __reg ( "a0" ) struct List * list , __reg ( "a1" ) const STRPTR name ) = "\tjsr\t-276(a6)" ;


 void * __AddTask ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Task * task , __reg ( "a2" ) const void * initPC , __reg ( "a3" ) const void * finalPC ) = "\tjsr\t-282(a6)" ;


 void __RemTask ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Task * task ) = "\tjsr\t-288(a6)" ;


 struct Task * __FindTask ( __reg ( "a6" ) void * , __reg ( "a1" ) const STRPTR name ) = "\tjsr\t-294(a6)" ;


 BYTE __SetTaskPri ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Task * task , __reg ( "d0" ) LONG priority ) = "\tjsr\t-300(a6)" ;


 ULONG __SetSignal ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG newSignals , __reg ( "d1" ) ULONG signalSet ) = "\tjsr\t-306(a6)" ;


 ULONG __SetExcept ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG newSignals , __reg ( "d1" ) ULONG signalSet ) = "\tjsr\t-312(a6)" ;


 ULONG __Wait ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG signalSet ) = "\tjsr\t-318(a6)" ;


 void __Signal ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Task * task , __reg ( "d0" ) ULONG signalSet ) = "\tjsr\t-324(a6)" ;


 BYTE __AllocSignal ( __reg ( "a6" ) void * , __reg ( "d0" ) LONG signalNum ) = "\tjsr\t-330(a6)" ;


 void __FreeSignal ( __reg ( "a6" ) void * , __reg ( "d0" ) LONG signalNum ) = "\tjsr\t-336(a6)" ;


 LONG __AllocTrap ( __reg ( "a6" ) void * , __reg ( "d0" ) LONG trapNum ) = "\tjsr\t-342(a6)" ;


 void __FreeTrap ( __reg ( "a6" ) void * , __reg ( "d0" ) LONG trapNum ) = "\tjsr\t-348(a6)" ;


 void __AddPort ( __reg ( "a6" ) void * , __reg ( "a1" ) struct MsgPort * port ) = "\tjsr\t-354(a6)" ;


 void __RemPort ( __reg ( "a6" ) void * , __reg ( "a1" ) struct MsgPort * port ) = "\tjsr\t-360(a6)" ;


 void __PutMsg ( __reg ( "a6" ) void * , __reg ( "a0" ) struct MsgPort * port , __reg ( "a1" ) struct Message * message ) = "\tjsr\t-366(a6)" ;


 struct Message * __GetMsg ( __reg ( "a6" ) void * , __reg ( "a0" ) struct MsgPort * port ) = "\tjsr\t-372(a6)" ;


 void __ReplyMsg ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Message * message ) = "\tjsr\t-378(a6)" ;


 struct Message * __WaitPort ( __reg ( "a6" ) void * , __reg ( "a0" ) struct MsgPort * port ) = "\tjsr\t-384(a6)" ;


 struct MsgPort * __FindPort ( __reg ( "a6" ) void * , __reg ( "a1" ) const STRPTR name ) = "\tjsr\t-390(a6)" ;


 void __AddLibrary ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Library * library ) = "\tjsr\t-396(a6)" ;


 void __RemLibrary ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Library * library ) = "\tjsr\t-402(a6)" ;


 struct Library * __OldOpenLibrary ( __reg ( "a6" ) void * , __reg ( "a1" ) const STRPTR libName ) = "\tjsr\t-408(a6)" ;


 void __CloseLibrary ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Library * library ) = "\tjsr\t-414(a6)" ;


 void * __SetFunction ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Library * library , __reg ( "a0" ) void * funcOffset , __reg ( "d0" ) ULONG ( * newFunction ) ( ) ) = "\tjsr\t-420(a6)" ;


 void __SumLibrary ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Library * library ) = "\tjsr\t-426(a6)" ;


 void __AddDevice ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Device * device ) = "\tjsr\t-432(a6)" ;


 void __RemDevice ( __reg ( "a6" ) void * , __reg ( "a1" ) struct Device * device ) = "\tjsr\t-438(a6)" ;


 BYTE __OpenDevice ( __reg ( "a6" ) void * , __reg ( "a0" ) const STRPTR devName , __reg ( "d0" ) ULONG unit , __reg ( "a1" ) struct IORequest * ioRequest , __reg ( "d1" ) ULONG flags ) = "\tjsr\t-444(a6)" ;


 void __CloseDevice ( __reg ( "a6" ) void * , __reg ( "a1" ) struct IORequest * ioRequest ) = "\tjsr\t-450(a6)" ;


 BYTE __DoIO ( __reg ( "a6" ) void * , __reg ( "a1" ) struct IORequest * ioRequest ) = "\tjsr\t-456(a6)" ;


 void __SendIO ( __reg ( "a6" ) void * , __reg ( "a1" ) struct IORequest * ioRequest ) = "\tjsr\t-462(a6)" ;


 BOOL __CheckIO ( __reg ( "a6" ) void * , __reg ( "a1" ) struct IORequest * ioRequest ) = "\tjsr\t-468(a6)" ;


 BYTE __WaitIO ( __reg ( "a6" ) void * , __reg ( "a1" ) struct IORequest * ioRequest ) = "\tjsr\t-474(a6)" ;


 void __AbortIO ( __reg ( "a6" ) void * , __reg ( "a1" ) struct IORequest * ioRequest ) = "\tjsr\t-480(a6)" ;


 void __AddResource ( __reg ( "a6" ) void * , __reg ( "a1" ) void * resource ) = "\tjsr\t-486(a6)" ;


 void __RemResource ( __reg ( "a6" ) void * , __reg ( "a1" ) void * resource ) = "\tjsr\t-492(a6)" ;


 void * __OpenResource ( __reg ( "a6" ) void * , __reg ( "a1" ) const STRPTR resName ) = "\tjsr\t-498(a6)" ;


 void * __RawDoFmt ( __reg ( "a6" ) void * , __reg ( "a0" ) const STRPTR formatString , __reg ( "a1" ) const void * dataStream , __reg ( "a2" ) void ( * putChProc ) ( ) , __reg ( "a3" ) void * putChData ) = "\tjsr\t-522(a6)" ;


 ULONG __GetCC ( __reg ( "a6" ) void * ) = "\tjsr\t-528(a6)" ;


 ULONG __TypeOfMem ( __reg ( "a6" ) void * , __reg ( "a1" ) const void * address ) = "\tjsr\t-534(a6)" ;


 ULONG __Procure ( __reg ( "a6" ) void * , __reg ( "a0" ) struct SignalSemaphore * sigSem , __reg ( "a1" ) struct SemaphoreMessage * bidMsg ) = "\tjsr\t-540(a6)" ;


 void __Vacate ( __reg ( "a6" ) void * , __reg ( "a0" ) struct SignalSemaphore * sigSem , __reg ( "a1" ) struct SemaphoreMessage * bidMsg ) = "\tjsr\t-546(a6)" ;


 struct Library * __OpenLibrary ( __reg ( "a6" ) void * , __reg ( "a1" ) const STRPTR libName , __reg ( "d0" ) ULONG version ) = "\tjsr\t-552(a6)" ;


 void __InitSemaphore ( __reg ( "a6" ) void * , __reg ( "a0" ) struct SignalSemaphore * sigSem ) = "\tjsr\t-558(a6)" ;


 void __ObtainSemaphore ( __reg ( "a6" ) void * , __reg ( "a0" ) struct SignalSemaphore * sigSem ) = "\tjsr\t-564(a6)" ;


 void __ReleaseSemaphore ( __reg ( "a6" ) void * , __reg ( "a0" ) struct SignalSemaphore * sigSem ) = "\tjsr\t-570(a6)" ;


 ULONG __AttemptSemaphore ( __reg ( "a6" ) void * , __reg ( "a0" ) struct SignalSemaphore * sigSem ) = "\tjsr\t-576(a6)" ;


 void __ObtainSemaphoreList ( __reg ( "a6" ) void * , __reg ( "a0" ) struct List * sigSem ) = "\tjsr\t-582(a6)" ;


 void __ReleaseSemaphoreList ( __reg ( "a6" ) void * , __reg ( "a0" ) struct List * sigSem ) = "\tjsr\t-588(a6)" ;


 struct SignalSemaphore * __FindSemaphore ( __reg ( "a6" ) void * , __reg ( "a1" ) STRPTR name ) = "\tjsr\t-594(a6)" ;


 void __AddSemaphore ( __reg ( "a6" ) void * , __reg ( "a1" ) struct SignalSemaphore * sigSem ) = "\tjsr\t-600(a6)" ;


 void __RemSemaphore ( __reg ( "a6" ) void * , __reg ( "a1" ) struct SignalSemaphore * sigSem ) = "\tjsr\t-606(a6)" ;


 ULONG __SumKickData ( __reg ( "a6" ) void * ) = "\tjsr\t-612(a6)" ;


 void __AddMemList ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG size , __reg ( "d1" ) ULONG attributes , __reg ( "d2" ) LONG pri , __reg ( "a0" ) void * base , __reg ( "a1" ) const STRPTR name ) = "\tjsr\t-618(a6)" ;


 void __CopyMem ( __reg ( "a6" ) void * , __reg ( "a0" ) const void * source , __reg ( "a1" ) void * dest , __reg ( "d0" ) ULONG size ) = "\tjsr\t-624(a6)" ;


 void __CopyMemQuick ( __reg ( "a6" ) void * , __reg ( "a0" ) const void * source , __reg ( "a1" ) void * dest , __reg ( "d0" ) ULONG size ) = "\tjsr\t-630(a6)" ;
#line 8 "proto\dos.h"
#pragma stdargs-on
#line 18 "libraries\dosextens.h"
 struct Process {
 struct Task pr_Task ;
 struct MsgPort pr_MsgPort ;
 WORD pr_Pad ;
 BPTR pr_SegList ;
 LONG pr_StackSize ;
 APTR pr_GlobVec ;
 LONG pr_TaskNum ;
 BPTR pr_StackBase ;
 LONG pr_Result2 ;
 BPTR pr_CurrentDir ;
 BPTR pr_CIS ;
 BPTR pr_COS ;
 APTR pr_ConsoleTask ;
 APTR pr_FileSystemTask ;
 BPTR pr_CLI ;
 APTR pr_ReturnAddr ;
 APTR pr_PktWait ;
 APTR pr_WindowPtr ;
 } ;
 struct FileHandle {
 struct Message * fh_Link ;
 struct MsgPort * fh_Port ;
 struct MsgPort * fh_Type ;
 LONG fh_Buf ;
 LONG fh_Pos ;
 LONG fh_End ;
 LONG fh_Funcs ;

 LONG fh_Func2 ;
 LONG fh_Func3 ;
 LONG fh_Args ;

 LONG fh_Arg2 ;
 } ;
 struct DosPacket {
 struct Message * dp_Link ;
 struct MsgPort * dp_Port ;
 LONG dp_Type ;
 LONG dp_Res1 ;
 LONG dp_Res2 ;




 LONG dp_Arg1 ;
 LONG dp_Arg2 ;
 LONG dp_Arg3 ;
 LONG dp_Arg4 ;
 LONG dp_Arg5 ;
 LONG dp_Arg6 ;
 LONG dp_Arg7 ;
 } ;
 struct StandardPacket {
 struct Message sp_Msg ;
 struct DosPacket sp_Pkt ;
 } ;








































 struct DosLibrary {
 struct Library dl_lib ;
 APTR dl_Root ;
 APTR dl_GV ;
 LONG dl_A2 ;
 LONG dl_A5 ;
 LONG dl_A6 ;
 } ;
 struct RootNode {
 BPTR rn_TaskArray ;
 BPTR rn_ConsoleSegment ;
 struct DateStamp rn_Time ;
 LONG rn_RestartSeg ;
 BPTR rn_Info ;
 BPTR rn_FileHandlerSegment ;
 } ;
 struct DosInfo {
 BPTR di_McName ;
 BPTR di_DevInfo ;
 BPTR di_Devices ;
 BPTR di_Handlers ;
 APTR di_NetHand ;
 } ;
 struct CommandLineInterface {
 LONG cli_Result2 ;
 BSTR cli_SetName ;
 BPTR cli_CommandDir ;
 LONG cli_ReturnCode ;
 BSTR cli_CommandName ;
 LONG cli_FailLevel ;
 BSTR cli_Prompt ;
 BPTR cli_StandardInput ;
 BPTR cli_CurrentInput ;
 BSTR cli_CommandFile ;
 LONG cli_Interactive ;
 LONG cli_Background ;
 BPTR cli_CurrentOutput ;
 LONG cli_DefaultStack ;
 BPTR cli_StandardOutput ;
 BPTR cli_Module ;
 } ;
 struct DeviceList {
 BPTR dl_Next ;
 LONG dl_Type ;
 struct MsgPort * dl_Task ;
 BPTR dl_Lock ;
 struct DateStamp dl_VolumeDate ;
 BPTR dl_LockList ;
 LONG dl_DiskType ;
 LONG dl_unused ;
 BSTR * dl_Name ;
 } ;
 struct DevInfo {
 BPTR dvi_Next ;
 LONG dvi_Type ;
 APTR dvi_Task ;
 BPTR dvi_Lock ;
 BSTR dvi_Handler ;
 LONG dvi_StackSize ;
 LONG dvi_Priority ;
 LONG dvi_Startup ;
 BPTR dvi_SegList ;
 BPTR dvi_GlobVec ;
 BSTR dvi_Name ;
 } ;
 struct DosList {
 BPTR dol_Next ;
 LONG dol_Type ;
 struct MsgPort * dol_Task ;
 BPTR dol_Lock ;
 union {
 struct {
 BSTR dol_Handler ;
 LONG dol_StackSize ;
 LONG dol_Priority ;
 ULONG dol_Startup ;
 BPTR dol_SegList ;
 BPTR dol_GlobVec ;
 } dol_handler ;
 struct {
 struct DateStamp dol_VolumeDate ;
 BPTR dol_LockList ;
 LONG dol_DiskType ;
 } dol_volume ;
 } dol_misc ;
 BSTR dol_Name ;
 } ;



 struct FileLock {
 BPTR fl_Link ;
 LONG fl_Key ;
 LONG fl_Access ;
 struct MsgPort * fl_Task ;
 BPTR fl_Volume ;
 } ;
#line 24 "clib\dos_protos.h"
 BPTR Open ( const STRPTR name , LONG accessMode ) ;
 LONG Close ( BPTR file ) ;
 LONG Read ( BPTR file , void * buffer , LONG length ) ;
 LONG Write ( BPTR file , const void * buffer , LONG length ) ;
 BPTR Input ( void ) ;
 BPTR Output ( void ) ;
 LONG Seek ( BPTR file , LONG position , LONG offset ) ;
 LONG DeleteFile ( const STRPTR name ) ;
 LONG Rename ( const STRPTR oldName , const STRPTR newName ) ;
 BPTR Lock ( const STRPTR name , LONG type ) ;
 void UnLock ( BPTR lock ) ;
 BPTR DupLock ( BPTR lock ) ;
 LONG Examine ( BPTR lock , struct FileInfoBlock * fileInfoBlock ) ;
 LONG ExNext ( BPTR lock , struct FileInfoBlock * fileInfoBlock ) ;
 LONG Info ( BPTR lock , struct InfoData * parameterBlock ) ;
 BPTR CreateDir ( const STRPTR name ) ;
 BPTR CurrentDir ( BPTR lock ) ;
 LONG IoErr ( void ) ;
 struct MsgPort * CreateProc ( const STRPTR name , LONG pri , BPTR segList , LONG stackSize ) ;
 void Exit ( LONG returnCode ) ;
 BPTR LoadSeg ( const STRPTR name ) ;
 void UnLoadSeg ( BPTR seglist ) ;
 struct MsgPort * DeviceProc ( const STRPTR name ) ;
 LONG SetComment ( const STRPTR name , const STRPTR comment ) ;
 LONG SetProtection ( const STRPTR name , LONG protect ) ;
 struct DateStamp * DateStamp ( struct DateStamp * date ) ;
 void Delay ( LONG timeout ) ;
 LONG WaitForChar ( BPTR file , LONG timeout ) ;
 BPTR ParentDir ( BPTR lock ) ;
 LONG IsInteractive ( BPTR file ) ;
 LONG Execute ( const STRPTR string , BPTR file , BPTR file2 ) ;
#line 10 "proto\dos.h"
#pragma stdargs-off



 extern struct DosLibrary * DOSBase ;
#line 8 "inline\dos_protos.h"
 BPTR __Open ( __reg ( "a6" ) void * , __reg ( "d1" ) const STRPTR name , __reg ( "d2" ) LONG accessMode ) = "\tjsr\t-30(a6)" ;


 LONG __Close ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR file ) = "\tjsr\t-36(a6)" ;


 LONG __Read ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR file , __reg ( "d2" ) void * buffer , __reg ( "d3" ) LONG length ) = "\tjsr\t-42(a6)" ;


 LONG __Write ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR file , __reg ( "d2" ) const void * buffer , __reg ( "d3" ) LONG length ) = "\tjsr\t-48(a6)" ;


 BPTR __Input ( __reg ( "a6" ) void * ) = "\tjsr\t-54(a6)" ;


 BPTR __Output ( __reg ( "a6" ) void * ) = "\tjsr\t-60(a6)" ;


 LONG __Seek ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR file , __reg ( "d2" ) LONG position , __reg ( "d3" ) LONG offset ) = "\tjsr\t-66(a6)" ;


 LONG __DeleteFile ( __reg ( "a6" ) void * , __reg ( "d1" ) const STRPTR name ) = "\tjsr\t-72(a6)" ;


 LONG __Rename ( __reg ( "a6" ) void * , __reg ( "d1" ) const STRPTR oldName , __reg ( "d2" ) const STRPTR newName ) = "\tjsr\t-78(a6)" ;


 BPTR __Lock ( __reg ( "a6" ) void * , __reg ( "d1" ) const STRPTR name , __reg ( "d2" ) LONG type ) = "\tjsr\t-84(a6)" ;


 void __UnLock ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR lock ) = "\tjsr\t-90(a6)" ;


 BPTR __DupLock ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR lock ) = "\tjsr\t-96(a6)" ;


 LONG __Examine ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR lock , __reg ( "d2" ) struct FileInfoBlock * fileInfoBlock ) = "\tjsr\t-102(a6)" ;


 LONG __ExNext ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR lock , __reg ( "d2" ) struct FileInfoBlock * fileInfoBlock ) = "\tjsr\t-108(a6)" ;


 LONG __Info ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR lock , __reg ( "d2" ) struct InfoData * parameterBlock ) = "\tjsr\t-114(a6)" ;


 BPTR __CreateDir ( __reg ( "a6" ) void * , __reg ( "d1" ) const STRPTR name ) = "\tjsr\t-120(a6)" ;


 BPTR __CurrentDir ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR lock ) = "\tjsr\t-126(a6)" ;


 LONG __IoErr ( __reg ( "a6" ) void * ) = "\tjsr\t-132(a6)" ;


 struct MsgPort * __CreateProc ( __reg ( "a6" ) void * , __reg ( "d1" ) const STRPTR name , __reg ( "d2" ) LONG pri , __reg ( "d3" ) BPTR segList , __reg ( "d4" ) LONG stackSize ) = "\tjsr\t-138(a6)" ;


 void __Exit ( __reg ( "a6" ) void * , __reg ( "d1" ) LONG returnCode ) = "\tjsr\t-144(a6)" ;


 BPTR __LoadSeg ( __reg ( "a6" ) void * , __reg ( "d1" ) const STRPTR name ) = "\tjsr\t-150(a6)" ;


 void __UnLoadSeg ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR seglist ) = "\tjsr\t-156(a6)" ;


 struct MsgPort * __DeviceProc ( __reg ( "a6" ) void * , __reg ( "d1" ) const STRPTR name ) = "\tjsr\t-174(a6)" ;


 LONG __SetComment ( __reg ( "a6" ) void * , __reg ( "d1" ) const STRPTR name , __reg ( "d2" ) const STRPTR comment ) = "\tjsr\t-180(a6)" ;


 LONG __SetProtection ( __reg ( "a6" ) void * , __reg ( "d1" ) const STRPTR name , __reg ( "d2" ) LONG protect ) = "\tjsr\t-186(a6)" ;


 struct DateStamp * __DateStamp ( __reg ( "a6" ) void * , __reg ( "d1" ) struct DateStamp * date ) = "\tjsr\t-192(a6)" ;


 void __Delay ( __reg ( "a6" ) void * , __reg ( "d1" ) LONG timeout ) = "\tjsr\t-198(a6)" ;


 LONG __WaitForChar ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR file , __reg ( "d2" ) LONG timeout ) = "\tjsr\t-204(a6)" ;


 BPTR __ParentDir ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR lock ) = "\tjsr\t-210(a6)" ;


 LONG __IsInteractive ( __reg ( "a6" ) void * , __reg ( "d1" ) BPTR file ) = "\tjsr\t-216(a6)" ;


 LONG __Execute ( __reg ( "a6" ) void * , __reg ( "d1" ) const STRPTR string , __reg ( "d2" ) BPTR file , __reg ( "d3" ) BPTR file2 ) = "\tjsr\t-222(a6)" ;
#line 8 "proto\graphics.h"
#pragma stdargs-on
#line 33 "graphics\gels.h"
 struct VSprite
 {
 struct VSprite * NextVSprite ;
 struct VSprite * PrevVSprite ;
 struct VSprite * DrawPath ;
 struct VSprite * ClearPath ;
 WORD OldY , OldX ;
 WORD Flags ;
 WORD Y , X ;
 WORD Height ;
 WORD Width ;
 WORD Depth ;
 WORD MeMask ;
 WORD HitMask ;
 WORD * ImageData ;
 WORD * BorderLine ;
 WORD * CollMask ;
 WORD * SprColors ;
 struct Bob * VSBob ;
 BYTE PlanePick ;
 BYTE PlaneOnOff ;
 SHORT VUserExt ;
 } ;
 struct Bob
 {
 WORD Flags ;
 WORD * SaveBuffer ;
 WORD * ImageShadow ;
 struct Bob * Before ;
 struct Bob * After ;
 struct VSprite * BobVSprite ;
 struct AnimComp * BobComp ;
 struct DBufPacket * DBuffer ;
 SHORT BUserExt ;
 } ;
 struct AnimComp
 {
 WORD Flags ;
 WORD Timer ;
 WORD TimeSet ;
 struct AnimComp * NextComp ;
 struct AnimComp * PrevComp ;
 struct AnimComp * NextSeq ;
 struct AnimComp * PrevSeq ;
 WORD ( * AnimCRoutine ) ( ) ;
 WORD YTrans ;
 WORD XTrans ;
 struct AnimOb * HeadOb ;
 struct Bob * AnimBob ;
 } ;
 struct AnimOb
 {
 struct AnimOb * NextOb , * PrevOb ;
 LONG Clock ;
 WORD AnOldY , AnOldX ;
 WORD AnY , AnX ;
 WORD YVel , XVel ;
 WORD YAccel , XAccel ;
 WORD RingYTrans , RingXTrans ;
 WORD ( * AnimORoutine ) ( ) ;
 struct AnimComp * HeadComp ;
 SHORT AUserExt ;
 } ;
 struct DBufPacket
 {
 WORD BufY , BufX ;
 struct VSprite * BufPath ;
 WORD * BufBuffer ;
 } ;





 struct collTable
 {
 int ( * collPtrs [ 16 ] ) ( ) ;
 } ;
#line 6 "graphics\regions.h"
 struct RegionRectangle
 {
 struct RegionRectangle * Next , * Prev ;
 struct Rectangle bounds ;
 } ;
 struct Region
 {
 struct Rectangle bounds ;
 struct RegionRectangle * RegionRectangle ;
 } ;
#line 4 "graphics\sprite.h"
 struct SimpleSprite
 {
 UWORD * posctldata ;
 UWORD height ;
 UWORD x , y ;
 UWORD num ;
 } ;
#line 54 "hardware\blit.h"
 struct bltnode
 {
 struct bltnode * n ;
 int ( * function ) ( ) ;
 char stat ;
 short blitsize ;
 short beamsync ;
 int ( * cleanup ) ( ) ;
 } ;
#line 48 "clib\graphics_protos.h"
 LONG BltBitMap ( const struct BitMap * srcBitMap , LONG xSrc , LONG ySrc ,
 struct BitMap * destBitMap , LONG xDest , LONG yDest , LONG xSize ,
 LONG ySize , ULONG minterm , ULONG mask , PLANEPTR tempA ) ;
 void BltTemplate ( const PLANEPTR source , LONG xSrc , LONG srcMod , struct RastPort * destRP ,
 LONG xDest , LONG yDest , LONG xSize , LONG ySize ) ;
 void ClearEOL ( struct RastPort * rp ) ;
 void ClearScreen ( struct RastPort * rp ) ;
 WORD TextLength ( struct RastPort * rp , const STRPTR string , ULONG count ) ;
 LONG Text ( struct RastPort * rp , const STRPTR string , ULONG count ) ;
 LONG SetFont ( struct RastPort * rp , const struct TextFont * textFont ) ;
 struct TextFont * OpenFont ( struct TextAttr * textAttr ) ;
 void CloseFont ( struct TextFont * textFont ) ;
 ULONG AskSoftStyle ( struct RastPort * rp ) ;
 ULONG SetSoftStyle ( struct RastPort * rp , ULONG style , ULONG enable ) ;
 void AddBob ( struct Bob * bob , struct RastPort * rp ) ;
 void AddVSprite ( struct VSprite * vSprite , struct RastPort * rp ) ;
 void DoCollision ( struct RastPort * rp ) ;
 void DrawGList ( struct RastPort * rp , struct ViewPort * vp ) ;
 void InitGels ( struct VSprite * head , struct VSprite * tail ,
 struct GelsInfo * gelsInfo ) ;
 void InitMasks ( struct VSprite * vSprite ) ;
 void RemIBob ( struct Bob * bob , struct RastPort * rp , struct ViewPort * vp ) ;
 void RemVSprite ( struct VSprite * vSprite ) ;
 void SetCollision ( ULONG num , void ( * routine ) ( struct VSprite * gelA , struct VSprite * gelB ) ,
 struct GelsInfo * gelsInfo ) ;
 void SortGList ( struct RastPort * rp ) ;
 void AddAnimOb ( struct AnimOb * anOb , struct AnimOb * * anKey , struct RastPort * rp ) ;
 void Animate ( struct AnimOb * * anKey , struct RastPort * rp ) ;
 BOOL GetGBuffers ( struct AnimOb * anOb , struct RastPort * rp , LONG flag ) ;
 void InitGMasks ( struct AnimOb * anOb ) ;
 void DrawEllipse ( struct RastPort * rp , LONG xCenter , LONG yCenter , LONG a , LONG b ) ;
 LONG AreaEllipse ( struct RastPort * rp , LONG xCenter , LONG yCenter , LONG a , LONG b ) ;
 void LoadRGB4 ( struct ViewPort * vp , const UWORD * colors , LONG count ) ;
 void InitRastPort ( struct RastPort * rp ) ;
 void InitVPort ( struct ViewPort * vp ) ;
 ULONG MrgCop ( struct View * view ) ;
 ULONG MakeVPort ( struct View * view , struct ViewPort * vp ) ;
 void LoadView ( struct View * view ) ;
 void WaitBlit ( void ) ;
 void SetRast ( struct RastPort * rp , ULONG pen ) ;
 void Move ( struct RastPort * rp , LONG x , LONG y ) ;
 void Draw ( struct RastPort * rp , LONG x , LONG y ) ;
 LONG AreaMove ( struct RastPort * rp , LONG x , LONG y ) ;
 LONG AreaDraw ( struct RastPort * rp , LONG x , LONG y ) ;
 LONG AreaEnd ( struct RastPort * rp ) ;
 void WaitTOF ( void ) ;
 void QBlit ( struct bltnode * blit ) ;
 void InitArea ( struct AreaInfo * areaInfo , void * vectorBuffer , LONG maxVectors ) ;
 void SetRGB4 ( struct ViewPort * vp , LONG index , ULONG red , ULONG green , ULONG blue ) ;
 void QBSBlit ( struct bltnode * blit ) ;
 void BltClear ( PLANEPTR memBlock , ULONG byteCount , ULONG flags ) ;
 void RectFill ( struct RastPort * rp , LONG xMin , LONG yMin , LONG xMax , LONG yMax ) ;
 void BltPattern ( struct RastPort * rp , const PLANEPTR mask , LONG xMin , LONG yMin ,
 LONG xMax , LONG yMax , ULONG maskBPR ) ;
 ULONG ReadPixel ( struct RastPort * rp , LONG x , LONG y ) ;
 LONG WritePixel ( struct RastPort * rp , LONG x , LONG y ) ;
 BOOL Flood ( struct RastPort * rp , ULONG mode , LONG x , LONG y ) ;
 void PolyDraw ( struct RastPort * rp , LONG count , const WORD * polyTable ) ;
 void SetAPen ( struct RastPort * rp , ULONG pen ) ;
 void SetBPen ( struct RastPort * rp , ULONG pen ) ;
 void SetDrMd ( struct RastPort * rp , ULONG drawMode ) ;
 void InitView ( struct View * view ) ;
 void CBump ( struct UCopList * copList ) ;
 void CMove ( struct UCopList * copList , void * destination , LONG data ) ;
 void CWait ( struct UCopList * copList , LONG v , LONG h ) ;
 LONG VBeamPos ( void ) ;
 void InitBitMap ( struct BitMap * bitMap , LONG depth , LONG width , LONG height ) ;
 void ScrollRaster ( struct RastPort * rp , LONG dx , LONG dy , LONG xMin , LONG yMin , LONG xMax ,
 LONG yMax ) ;
 void WaitBOVP ( struct ViewPort * vp ) ;
 WORD GetSprite ( struct SimpleSprite * sprite , LONG num ) ;
 void FreeSprite ( LONG num ) ;
 void ChangeSprite ( struct ViewPort * vp , struct SimpleSprite * sprite , UWORD * newData ) ;
 void MoveSprite ( struct ViewPort * vp , struct SimpleSprite * sprite , LONG x , LONG y ) ;
 void LockLayerRom ( struct Layer * layer ) ;
 void UnlockLayerRom ( struct Layer * layer ) ;
 void SyncSBitMap ( struct Layer * layer ) ;
 void CopySBitMap ( struct Layer * layer ) ;
 void OwnBlitter ( void ) ;
 void DisownBlitter ( void ) ;
 struct TmpRas * InitTmpRas ( struct TmpRas * tmpRas , PLANEPTR buffer , LONG size ) ;
 void AskFont ( struct RastPort * rp , struct TextAttr * textAttr ) ;
 void AddFont ( struct TextFont * textFont ) ;
 void RemFont ( struct TextFont * textFont ) ;
 PLANEPTR AllocRaster ( ULONG width , ULONG height ) ;
 void FreeRaster ( PLANEPTR p , ULONG width , ULONG height ) ;
 void AndRectRegion ( struct Region * region , const struct Rectangle * rectangle ) ;
 BOOL OrRectRegion ( struct Region * region , const struct Rectangle * rectangle ) ;
 struct Region * NewRegion ( void ) ;
 BOOL ClearRectRegion ( struct Region * region , const struct Rectangle * rectangle ) ;
 void ClearRegion ( struct Region * region ) ;
 void DisposeRegion ( struct Region * region ) ;
 void FreeVPortCopLists ( struct ViewPort * vp ) ;
 void FreeCopList ( struct CopList * copList ) ;
 void ClipBlit ( struct RastPort * srcRP , LONG xSrc , LONG ySrc , struct RastPort * destRP ,
 LONG xDest , LONG yDest , LONG xSize , LONG ySize , ULONG minterm ) ;
 BOOL XorRectRegion ( struct Region * region , const struct Rectangle * rectangle ) ;
 void FreeCprList ( struct cprlist * cprList ) ;
 struct ColorMap * GetColorMap ( LONG entries ) ;
 void FreeColorMap ( struct ColorMap * colorMap ) ;
 ULONG GetRGB4 ( struct ColorMap * colorMap , LONG entry ) ;
 void ScrollVPort ( struct ViewPort * vp ) ;
 struct CopList * UCopperListInit ( struct UCopList * uCopList , LONG n ) ;
 void FreeGBuffers ( struct AnimOb * anOb , struct RastPort * rp , LONG flag ) ;
 void BltBitMapRastPort ( const struct BitMap * srcBitMap , LONG xSrc , LONG ySrc ,
 struct RastPort * destRP , LONG xDest , LONG yDest , LONG xSize ,
 LONG ySize , ULONG minterm ) ;
 BOOL OrRegionRegion ( const struct Region * srcRegion , struct Region * destRegion ) ;
 BOOL XorRegionRegion ( const struct Region * srcRegion , struct Region * destRegion ) ;
 BOOL AndRegionRegion ( const struct Region * srcRegion , struct Region * destRegion ) ;
 void SetRGB4CM ( struct ColorMap * colorMap , LONG index , ULONG red , ULONG green ,
 ULONG blue ) ;
 void BltMaskBitMapRastPort ( const struct BitMap * srcBitMap , LONG xSrc , LONG ySrc ,
 struct RastPort * destRP , LONG xDest , LONG yDest , LONG xSize ,
 LONG ySize , ULONG minterm , const PLANEPTR bltMask ) ;
 BOOL AttemptLockLayerRom ( struct Layer * layer ) ;
#line 10 "proto\graphics.h"
#pragma stdargs-off



 extern struct GfxBase * GfxBase ;
#line 8 "inline\graphics_protos.h"
 LONG __BltBitMap ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct BitMap * srcBitMap , __reg ( "d0" ) LONG xSrc , __reg ( "d1" ) LONG ySrc , __reg ( "a1" ) struct BitMap * destBitMap , __reg ( "d2" ) LONG xDest , __reg ( "d3" ) LONG yDest , __reg ( "d4" ) LONG xSize , __reg ( "d5" ) LONG ySize , __reg ( "d6" ) ULONG minterm , __reg ( "d7" ) ULONG mask , __reg ( "a2" ) PLANEPTR tempA ) = "\tjsr\t-30(a6)" ;


 void __BltTemplate ( __reg ( "a6" ) void * , __reg ( "a0" ) const PLANEPTR source , __reg ( "d0" ) LONG xSrc , __reg ( "d1" ) LONG srcMod , __reg ( "a1" ) struct RastPort * destRP , __reg ( "d2" ) LONG xDest , __reg ( "d3" ) LONG yDest , __reg ( "d4" ) LONG xSize , __reg ( "d5" ) LONG ySize ) = "\tjsr\t-36(a6)" ;


 void __ClearEOL ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp ) = "\tjsr\t-42(a6)" ;


 void __ClearScreen ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp ) = "\tjsr\t-48(a6)" ;


 WORD __TextLength ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "a0" ) const STRPTR string , __reg ( "d0" ) ULONG count ) = "\tjsr\t-54(a6)" ;


 LONG __Text ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "a0" ) const STRPTR string , __reg ( "d0" ) ULONG count ) = "\tjsr\t-60(a6)" ;


 LONG __SetFont ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "a0" ) const struct TextFont * textFont ) = "\tjsr\t-66(a6)" ;


 struct TextFont * __OpenFont ( __reg ( "a6" ) void * , __reg ( "a0" ) struct TextAttr * textAttr ) = "\tjsr\t-72(a6)" ;


 void __CloseFont ( __reg ( "a6" ) void * , __reg ( "a1" ) struct TextFont * textFont ) = "\tjsr\t-78(a6)" ;


 ULONG __AskSoftStyle ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp ) = "\tjsr\t-84(a6)" ;


 ULONG __SetSoftStyle ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) ULONG style , __reg ( "d1" ) ULONG enable ) = "\tjsr\t-90(a6)" ;


 void __AddBob ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Bob * bob , __reg ( "a1" ) struct RastPort * rp ) = "\tjsr\t-96(a6)" ;


 void __AddVSprite ( __reg ( "a6" ) void * , __reg ( "a0" ) struct VSprite * vSprite , __reg ( "a1" ) struct RastPort * rp ) = "\tjsr\t-102(a6)" ;


 void __DoCollision ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp ) = "\tjsr\t-108(a6)" ;


 void __DrawGList ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "a0" ) struct ViewPort * vp ) = "\tjsr\t-114(a6)" ;


 void __InitGels ( __reg ( "a6" ) void * , __reg ( "a0" ) struct VSprite * head , __reg ( "a1" ) struct VSprite * tail , __reg ( "a2" ) struct GelsInfo * gelsInfo ) = "\tjsr\t-120(a6)" ;


 void __InitMasks ( __reg ( "a6" ) void * , __reg ( "a0" ) struct VSprite * vSprite ) = "\tjsr\t-126(a6)" ;


 void __RemIBob ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Bob * bob , __reg ( "a1" ) struct RastPort * rp , __reg ( "a2" ) struct ViewPort * vp ) = "\tjsr\t-132(a6)" ;


 void __RemVSprite ( __reg ( "a6" ) void * , __reg ( "a0" ) struct VSprite * vSprite ) = "\tjsr\t-138(a6)" ;


 void __SetCollision ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG num , __reg ( "a0" ) void ( * routine ) ( struct VSprite * gelA , struct VSprite * gelB ) , __reg ( "a1" ) struct GelsInfo * gelsInfo ) = "\tjsr\t-144(a6)" ;


 void __SortGList ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp ) = "\tjsr\t-150(a6)" ;


 void __AddAnimOb ( __reg ( "a6" ) void * , __reg ( "a0" ) struct AnimOb * anOb , __reg ( "a1" ) struct AnimOb * * anKey , __reg ( "a2" ) struct RastPort * rp ) = "\tjsr\t-156(a6)" ;


 void __Animate ( __reg ( "a6" ) void * , __reg ( "a0" ) struct AnimOb * * anKey , __reg ( "a1" ) struct RastPort * rp ) = "\tjsr\t-162(a6)" ;


 BOOL __GetGBuffers ( __reg ( "a6" ) void * , __reg ( "a0" ) struct AnimOb * anOb , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG flag ) = "\tjsr\t-168(a6)" ;


 void __InitGMasks ( __reg ( "a6" ) void * , __reg ( "a0" ) struct AnimOb * anOb ) = "\tjsr\t-174(a6)" ;


 void __DrawEllipse ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG xCenter , __reg ( "d1" ) LONG yCenter , __reg ( "d2" ) LONG a , __reg ( "d3" ) LONG b ) = "\tjsr\t-180(a6)" ;


 LONG __AreaEllipse ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG xCenter , __reg ( "d1" ) LONG yCenter , __reg ( "d2" ) LONG a , __reg ( "d3" ) LONG b ) = "\tjsr\t-186(a6)" ;


 void __LoadRGB4 ( __reg ( "a6" ) void * , __reg ( "a0" ) struct ViewPort * vp , __reg ( "a1" ) const UWORD * colors , __reg ( "d0" ) LONG count ) = "\tjsr\t-192(a6)" ;


 void __InitRastPort ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp ) = "\tjsr\t-198(a6)" ;


 void __InitVPort ( __reg ( "a6" ) void * , __reg ( "a0" ) struct ViewPort * vp ) = "\tjsr\t-204(a6)" ;


 ULONG __MrgCop ( __reg ( "a6" ) void * , __reg ( "a1" ) struct View * view ) = "\tjsr\t-210(a6)" ;


 ULONG __MakeVPort ( __reg ( "a6" ) void * , __reg ( "a0" ) struct View * view , __reg ( "a1" ) struct ViewPort * vp ) = "\tjsr\t-216(a6)" ;


 void __LoadView ( __reg ( "a6" ) void * , __reg ( "a1" ) struct View * view ) = "\tjsr\t-222(a6)" ;


 void __WaitBlit ( __reg ( "a6" ) void * ) = "\tjsr\t-228(a6)" ;


 void __SetRast ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) ULONG pen ) = "\tjsr\t-234(a6)" ;


 void __Move ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG x , __reg ( "d1" ) LONG y ) = "\tjsr\t-240(a6)" ;


 void __Draw ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG x , __reg ( "d1" ) LONG y ) = "\tjsr\t-246(a6)" ;


 LONG __AreaMove ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG x , __reg ( "d1" ) LONG y ) = "\tjsr\t-252(a6)" ;


 LONG __AreaDraw ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG x , __reg ( "d1" ) LONG y ) = "\tjsr\t-258(a6)" ;


 LONG __AreaEnd ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp ) = "\tjsr\t-264(a6)" ;


 void __WaitTOF ( __reg ( "a6" ) void * ) = "\tjsr\t-270(a6)" ;


 void __QBlit ( __reg ( "a6" ) void * , __reg ( "a1" ) struct bltnode * blit ) = "\tjsr\t-276(a6)" ;


 void __InitArea ( __reg ( "a6" ) void * , __reg ( "a0" ) struct AreaInfo * areaInfo , __reg ( "a1" ) void * vectorBuffer , __reg ( "d0" ) LONG maxVectors ) = "\tjsr\t-282(a6)" ;


 void __SetRGB4 ( __reg ( "a6" ) void * , __reg ( "a0" ) struct ViewPort * vp , __reg ( "d0" ) LONG index , __reg ( "d1" ) ULONG red , __reg ( "d2" ) ULONG green , __reg ( "d3" ) ULONG blue ) = "\tjsr\t-288(a6)" ;


 void __QBSBlit ( __reg ( "a6" ) void * , __reg ( "a1" ) struct bltnode * blit ) = "\tjsr\t-294(a6)" ;


 void __BltClear ( __reg ( "a6" ) void * , __reg ( "a1" ) PLANEPTR memBlock , __reg ( "d0" ) ULONG byteCount , __reg ( "d1" ) ULONG flags ) = "\tjsr\t-300(a6)" ;


 void __RectFill ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG xMin , __reg ( "d1" ) LONG yMin , __reg ( "d2" ) LONG xMax , __reg ( "d3" ) LONG yMax ) = "\tjsr\t-306(a6)" ;


 void __BltPattern ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "a0" ) const PLANEPTR mask , __reg ( "d0" ) LONG xMin , __reg ( "d1" ) LONG yMin , __reg ( "d2" ) LONG xMax , __reg ( "d3" ) LONG yMax , __reg ( "d4" ) ULONG maskBPR ) = "\tjsr\t-312(a6)" ;


 ULONG __ReadPixel ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG x , __reg ( "d1" ) LONG y ) = "\tjsr\t-318(a6)" ;


 LONG __WritePixel ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG x , __reg ( "d1" ) LONG y ) = "\tjsr\t-324(a6)" ;


 BOOL __Flood ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d2" ) ULONG mode , __reg ( "d0" ) LONG x , __reg ( "d1" ) LONG y ) = "\tjsr\t-330(a6)" ;


 void __PolyDraw ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG count , __reg ( "a0" ) const WORD * polyTable ) = "\tjsr\t-336(a6)" ;


 void __SetAPen ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) ULONG pen ) = "\tjsr\t-342(a6)" ;


 void __SetBPen ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) ULONG pen ) = "\tjsr\t-348(a6)" ;


 void __SetDrMd ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) ULONG drawMode ) = "\tjsr\t-354(a6)" ;


 void __InitView ( __reg ( "a6" ) void * , __reg ( "a1" ) struct View * view ) = "\tjsr\t-360(a6)" ;


 void __CBump ( __reg ( "a6" ) void * , __reg ( "a1" ) struct UCopList * copList ) = "\tjsr\t-366(a6)" ;


 void __CMove ( __reg ( "a6" ) void * , __reg ( "a1" ) struct UCopList * copList , __reg ( "d0" ) void * destination , __reg ( "d1" ) LONG data ) = "\tjsr\t-372(a6)" ;


 void __CWait ( __reg ( "a6" ) void * , __reg ( "a1" ) struct UCopList * copList , __reg ( "d0" ) LONG v , __reg ( "d1" ) LONG h ) = "\tjsr\t-378(a6)" ;


 LONG __VBeamPos ( __reg ( "a6" ) void * ) = "\tjsr\t-384(a6)" ;


 void __InitBitMap ( __reg ( "a6" ) void * , __reg ( "a0" ) struct BitMap * bitMap , __reg ( "d0" ) LONG depth , __reg ( "d1" ) LONG width , __reg ( "d2" ) LONG height ) = "\tjsr\t-390(a6)" ;


 void __ScrollRaster ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG dx , __reg ( "d1" ) LONG dy , __reg ( "d2" ) LONG xMin , __reg ( "d3" ) LONG yMin , __reg ( "d4" ) LONG xMax , __reg ( "d5" ) LONG yMax ) = "\tjsr\t-396(a6)" ;


 void __WaitBOVP ( __reg ( "a6" ) void * , __reg ( "a0" ) struct ViewPort * vp ) = "\tjsr\t-402(a6)" ;


 WORD __GetSprite ( __reg ( "a6" ) void * , __reg ( "a0" ) struct SimpleSprite * sprite , __reg ( "d0" ) LONG num ) = "\tjsr\t-408(a6)" ;


 void __FreeSprite ( __reg ( "a6" ) void * , __reg ( "d0" ) LONG num ) = "\tjsr\t-414(a6)" ;


 void __ChangeSprite ( __reg ( "a6" ) void * , __reg ( "a0" ) struct ViewPort * vp , __reg ( "a1" ) struct SimpleSprite * sprite , __reg ( "a2" ) UWORD * newData ) = "\tjsr\t-420(a6)" ;


 void __MoveSprite ( __reg ( "a6" ) void * , __reg ( "a0" ) struct ViewPort * vp , __reg ( "a1" ) struct SimpleSprite * sprite , __reg ( "d0" ) LONG x , __reg ( "d1" ) LONG y ) = "\tjsr\t-426(a6)" ;


 void __LockLayerRom ( __reg ( "a6" ) void * , __reg ( "a5" ) struct Layer * layer ) = "\tjsr\t-432(a6)" ;


 void __UnlockLayerRom ( __reg ( "a6" ) void * , __reg ( "a5" ) struct Layer * layer ) = "\tjsr\t-438(a6)" ;


 void __SyncSBitMap ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Layer * layer ) = "\tjsr\t-444(a6)" ;


 void __CopySBitMap ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Layer * layer ) = "\tjsr\t-450(a6)" ;


 void __OwnBlitter ( __reg ( "a6" ) void * ) = "\tjsr\t-456(a6)" ;


 void __DisownBlitter ( __reg ( "a6" ) void * ) = "\tjsr\t-462(a6)" ;


 struct TmpRas * __InitTmpRas ( __reg ( "a6" ) void * , __reg ( "a0" ) struct TmpRas * tmpRas , __reg ( "a1" ) PLANEPTR buffer , __reg ( "d0" ) LONG size ) = "\tjsr\t-468(a6)" ;


 void __AskFont ( __reg ( "a6" ) void * , __reg ( "a1" ) struct RastPort * rp , __reg ( "a0" ) struct TextAttr * textAttr ) = "\tjsr\t-474(a6)" ;


 void __AddFont ( __reg ( "a6" ) void * , __reg ( "a1" ) struct TextFont * textFont ) = "\tjsr\t-480(a6)" ;


 void __RemFont ( __reg ( "a6" ) void * , __reg ( "a1" ) struct TextFont * textFont ) = "\tjsr\t-486(a6)" ;


 PLANEPTR __AllocRaster ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG width , __reg ( "d1" ) ULONG height ) = "\tjsr\t-492(a6)" ;


 void __FreeRaster ( __reg ( "a6" ) void * , __reg ( "a0" ) PLANEPTR p , __reg ( "d0" ) ULONG width , __reg ( "d1" ) ULONG height ) = "\tjsr\t-498(a6)" ;


 void __AndRectRegion ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Region * region , __reg ( "a1" ) const struct Rectangle * rectangle ) = "\tjsr\t-504(a6)" ;


 BOOL __OrRectRegion ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Region * region , __reg ( "a1" ) const struct Rectangle * rectangle ) = "\tjsr\t-510(a6)" ;


 struct Region * __NewRegion ( __reg ( "a6" ) void * ) = "\tjsr\t-516(a6)" ;


 BOOL __ClearRectRegion ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Region * region , __reg ( "a1" ) const struct Rectangle * rectangle ) = "\tjsr\t-522(a6)" ;


 void __ClearRegion ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Region * region ) = "\tjsr\t-528(a6)" ;


 void __DisposeRegion ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Region * region ) = "\tjsr\t-534(a6)" ;


 void __FreeVPortCopLists ( __reg ( "a6" ) void * , __reg ( "a0" ) struct ViewPort * vp ) = "\tjsr\t-540(a6)" ;


 void __FreeCopList ( __reg ( "a6" ) void * , __reg ( "a0" ) struct CopList * copList ) = "\tjsr\t-546(a6)" ;


 void __ClipBlit ( __reg ( "a6" ) void * , __reg ( "a0" ) struct RastPort * srcRP , __reg ( "d0" ) LONG xSrc , __reg ( "d1" ) LONG ySrc , __reg ( "a1" ) struct RastPort * destRP , __reg ( "d2" ) LONG xDest , __reg ( "d3" ) LONG yDest , __reg ( "d4" ) LONG xSize , __reg ( "d5" ) LONG ySize , __reg ( "d6" ) ULONG minterm ) = "\tjsr\t-552(a6)" ;


 BOOL __XorRectRegion ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Region * region , __reg ( "a1" ) const struct Rectangle * rectangle ) = "\tjsr\t-558(a6)" ;


 void __FreeCprList ( __reg ( "a6" ) void * , __reg ( "a0" ) struct cprlist * cprList ) = "\tjsr\t-564(a6)" ;


 struct ColorMap * __GetColorMap ( __reg ( "a6" ) void * , __reg ( "d0" ) LONG entries ) = "\tjsr\t-570(a6)" ;


 void __FreeColorMap ( __reg ( "a6" ) void * , __reg ( "a0" ) struct ColorMap * colorMap ) = "\tjsr\t-576(a6)" ;


 ULONG __GetRGB4 ( __reg ( "a6" ) void * , __reg ( "a0" ) struct ColorMap * colorMap , __reg ( "d0" ) LONG entry ) = "\tjsr\t-582(a6)" ;


 void __ScrollVPort ( __reg ( "a6" ) void * , __reg ( "a0" ) struct ViewPort * vp ) = "\tjsr\t-588(a6)" ;


 struct CopList * __UCopperListInit ( __reg ( "a6" ) void * , __reg ( "a0" ) struct UCopList * uCopList , __reg ( "d0" ) LONG n ) = "\tjsr\t-594(a6)" ;


 void __FreeGBuffers ( __reg ( "a6" ) void * , __reg ( "a0" ) struct AnimOb * anOb , __reg ( "a1" ) struct RastPort * rp , __reg ( "d0" ) LONG flag ) = "\tjsr\t-600(a6)" ;


 void __BltBitMapRastPort ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct BitMap * srcBitMap , __reg ( "d0" ) LONG xSrc , __reg ( "d1" ) LONG ySrc , __reg ( "a1" ) struct RastPort * destRP , __reg ( "d2" ) LONG xDest , __reg ( "d3" ) LONG yDest , __reg ( "d4" ) LONG xSize , __reg ( "d5" ) LONG ySize , __reg ( "d6" ) ULONG minterm ) = "\tjsr\t-606(a6)" ;


 BOOL __OrRegionRegion ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct Region * srcRegion , __reg ( "a1" ) struct Region * destRegion ) = "\tjsr\t-612(a6)" ;


 BOOL __XorRegionRegion ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct Region * srcRegion , __reg ( "a1" ) struct Region * destRegion ) = "\tjsr\t-618(a6)" ;


 BOOL __AndRegionRegion ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct Region * srcRegion , __reg ( "a1" ) struct Region * destRegion ) = "\tjsr\t-624(a6)" ;


 void __SetRGB4CM ( __reg ( "a6" ) void * , __reg ( "a0" ) struct ColorMap * colorMap , __reg ( "d0" ) LONG index , __reg ( "d1" ) ULONG red , __reg ( "d2" ) ULONG green , __reg ( "d3" ) ULONG blue ) = "\tjsr\t-630(a6)" ;


 void __BltMaskBitMapRastPort ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct BitMap * srcBitMap , __reg ( "d0" ) LONG xSrc , __reg ( "d1" ) LONG ySrc , __reg ( "a1" ) struct RastPort * destRP , __reg ( "d2" ) LONG xDest , __reg ( "d3" ) LONG yDest , __reg ( "d4" ) LONG xSize , __reg ( "d5" ) LONG ySize , __reg ( "d6" ) ULONG minterm , __reg ( "a2" ) const PLANEPTR bltMask ) = "\tjsr\t-636(a6)" ;


 BOOL __AttemptLockLayerRom ( __reg ( "a6" ) void * , __reg ( "a5" ) struct Layer * layer ) = "\tjsr\t-654(a6)" ;
#line 8 "proto\intuition.h"
#pragma stdargs-on
#line 21 "clib\intuition_protos.h"
 void OpenIntuition ( void ) ;
 void Intuition ( struct InputEvent * iEvent ) ;
 UWORD AddGadget ( struct Window * window , struct Gadget * gadget , ULONG position ) ;
 BOOL ClearDMRequest ( struct Window * window ) ;
 void ClearMenuStrip ( struct Window * window ) ;
 void ClearPointer ( struct Window * window ) ;
 BOOL CloseScreen ( struct Screen * screen ) ;
 void CloseWindow ( struct Window * window ) ;
 LONG CloseWorkBench ( void ) ;
 void CurrentTime ( ULONG * seconds , ULONG * micros ) ;
 BOOL DisplayAlert ( ULONG alertNumber , const STRPTR string , ULONG height ) ;
 void DisplayBeep ( struct Screen * screen ) ;
 BOOL DoubleClick ( ULONG sSeconds , ULONG sMicros , ULONG cSeconds , ULONG cMicros ) ;
 void DrawBorder ( struct RastPort * rp , const struct Border * border , LONG leftOffset ,
 LONG topOffset ) ;
 void DrawImage ( struct RastPort * rp , struct Image * image , LONG leftOffset ,
 LONG topOffset ) ;
 void EndRequest ( struct Requester * requester , struct Window * window ) ;
 struct Preferences * GetDefPrefs ( struct Preferences * preferences , LONG size ) ;
 struct Preferences * GetPrefs ( struct Preferences * preferences , LONG size ) ;
 void InitRequester ( struct Requester * requester ) ;
 struct MenuItem * ItemAddress ( const struct Menu * menuStrip , ULONG menuNumber ) ;
 BOOL ModifyIDCMP ( struct Window * window , ULONG flags ) ;
 void ModifyProp ( struct Gadget * gadget , struct Window * window ,
 struct Requester * requester , ULONG flags , ULONG horizPot ,
 ULONG vertPot , ULONG horizBody , ULONG vertBody ) ;
 void MoveScreen ( struct Screen * screen , LONG dx , LONG dy ) ;
 void MoveWindow ( struct Window * window , LONG dx , LONG dy ) ;
 void OffGadget ( struct Gadget * gadget , struct Window * window ,
 struct Requester * requester ) ;
 void OffMenu ( struct Window * window , ULONG menuNumber ) ;
 void OnGadget ( struct Gadget * gadget , struct Window * window ,
 struct Requester * requester ) ;
 void OnMenu ( struct Window * window , ULONG menuNumber ) ;
 struct Screen * OpenScreen ( const struct NewScreen * newScreen ) ;
 struct Window * OpenWindow ( const struct NewWindow * newWindow ) ;
 ULONG OpenWorkBench ( void ) ;
 void PrintIText ( struct RastPort * rp , const struct IntuiText * iText , LONG left ,
 LONG top ) ;
 void RefreshGadgets ( struct Gadget * gadgets , struct Window * window ,
 struct Requester * requester ) ;
 UWORD RemoveGadget ( struct Window * window , struct Gadget * gadget ) ;
 void ReportMouse ( LONG flag , struct Window * window ) ;
 void ReportMouse1 ( struct Window * flag , LONG window ) ;
 BOOL Request ( struct Requester * requester , struct Window * window ) ;
 void ScreenToBack ( struct Screen * screen ) ;
 void ScreenToFront ( struct Screen * screen ) ;
 BOOL SetDMRequest ( struct Window * window , struct Requester * requester ) ;
 BOOL SetMenuStrip ( struct Window * window , struct Menu * menu ) ;
 void SetPointer ( struct Window * window , UWORD * pointer , LONG height , LONG width ,
 LONG xOffset , LONG yOffset ) ;
 void SetWindowTitles ( struct Window * window , const STRPTR windowTitle ,
 const STRPTR screenTitle ) ;
 void ShowTitle ( struct Screen * screen , LONG showIt ) ;
 void SizeWindow ( struct Window * window , LONG dx , LONG dy ) ;
 struct View * ViewAddress ( void ) ;
 struct ViewPort * ViewPortAddress ( const struct Window * window ) ;
 void WindowToBack ( struct Window * window ) ;
 void WindowToFront ( struct Window * window ) ;
 BOOL WindowLimits ( struct Window * window , LONG widthMin , LONG heightMin , ULONG widthMax ,
 ULONG heightMax ) ;
 struct Preferences * SetPrefs ( const struct Preferences * preferences , LONG size ,
 LONG inform ) ;
 LONG IntuiTextLength ( const struct IntuiText * iText ) ;
 BOOL WBenchToBack ( void ) ;
 BOOL WBenchToFront ( void ) ;
 BOOL AutoRequest ( struct Window * window , const struct IntuiText * body ,
 const struct IntuiText * posText , const struct IntuiText * negText ,
 ULONG pFlag , ULONG nFlag , ULONG width , ULONG height ) ;
 void BeginRefresh ( struct Window * window ) ;
 struct Window * BuildSysRequest ( struct Window * window , const struct IntuiText * body ,
 const struct IntuiText * posText , const struct IntuiText * negText ,
 ULONG flags , ULONG width , ULONG height ) ;
 void EndRefresh ( struct Window * window , LONG complete ) ;
 void FreeSysRequest ( struct Window * window ) ;
 void MakeScreen ( struct Screen * screen ) ;
 void RemakeDisplay ( void ) ;
 void RethinkDisplay ( void ) ;
 void * AllocRemember ( struct Remember * * rememberKey , ULONG size , ULONG flags ) ;
 void FreeRemember ( struct Remember * * rememberKey , LONG reallyForget ) ;
 ULONG LockIBase ( ULONG dontknow ) ;
 void UnlockIBase ( ULONG ibLock ) ;
 LONG GetScreenData ( void * buffer , ULONG size , ULONG type , const struct Screen * screen ) ;
 void RefreshGList ( struct Gadget * gadgets , struct Window * window ,
 struct Requester * requester , LONG numGad ) ;
 UWORD AddGList ( struct Window * window , struct Gadget * gadget , ULONG position ,
 LONG numGad , struct Requester * requester ) ;
 UWORD RemoveGList ( struct Window * remPtr , struct Gadget * gadget , LONG numGad ) ;
 void ActivateWindow ( struct Window * window ) ;
 void RefreshWindowFrame ( struct Window * window ) ;
 BOOL ActivateGadget ( struct Gadget * gadgets , struct Window * window ,
 struct Requester * requester ) ;
 void NewModifyProp ( struct Gadget * gadget , struct Window * window ,
 struct Requester * requester , ULONG flags , ULONG horizPot ,
 ULONG vertPot , ULONG horizBody , ULONG vertBody , LONG numGad ) ;
#line 10 "proto\intuition.h"
#pragma stdargs-off



 extern struct IntuitionBase * IntuitionBase ;
#line 8 "inline\intuition_protos.h"
 void __OpenIntuition ( __reg ( "a6" ) void * ) = "\tjsr\t-30(a6)" ;


 void __Intuition ( __reg ( "a6" ) void * , __reg ( "a0" ) struct InputEvent * iEvent ) = "\tjsr\t-36(a6)" ;


 UWORD __AddGadget ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "a1" ) struct Gadget * gadget , __reg ( "d0" ) ULONG position ) = "\tjsr\t-42(a6)" ;


 BOOL __ClearDMRequest ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window ) = "\tjsr\t-48(a6)" ;


 void __ClearMenuStrip ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window ) = "\tjsr\t-54(a6)" ;


 void __ClearPointer ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window ) = "\tjsr\t-60(a6)" ;


 BOOL __CloseScreen ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Screen * screen ) = "\tjsr\t-66(a6)" ;


 void __CloseWindow ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window ) = "\tjsr\t-72(a6)" ;


 LONG __CloseWorkBench ( __reg ( "a6" ) void * ) = "\tjsr\t-78(a6)" ;


 void __CurrentTime ( __reg ( "a6" ) void * , __reg ( "a0" ) ULONG * seconds , __reg ( "a1" ) ULONG * micros ) = "\tjsr\t-84(a6)" ;


 BOOL __DisplayAlert ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG alertNumber , __reg ( "a0" ) const STRPTR string , __reg ( "d1" ) ULONG height ) = "\tjsr\t-90(a6)" ;


 void __DisplayBeep ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Screen * screen ) = "\tjsr\t-96(a6)" ;


 BOOL __DoubleClick ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG sSeconds , __reg ( "d1" ) ULONG sMicros , __reg ( "d2" ) ULONG cSeconds , __reg ( "d3" ) ULONG cMicros ) = "\tjsr\t-102(a6)" ;


 void __DrawBorder ( __reg ( "a6" ) void * , __reg ( "a0" ) struct RastPort * rp , __reg ( "a1" ) const struct Border * border , __reg ( "d0" ) LONG leftOffset , __reg ( "d1" ) LONG topOffset ) = "\tjsr\t-108(a6)" ;


 void __DrawImage ( __reg ( "a6" ) void * , __reg ( "a0" ) struct RastPort * rp , __reg ( "a1" ) struct Image * image , __reg ( "d0" ) LONG leftOffset , __reg ( "d1" ) LONG topOffset ) = "\tjsr\t-114(a6)" ;


 void __EndRequest ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Requester * requester , __reg ( "a1" ) struct Window * window ) = "\tjsr\t-120(a6)" ;


 struct Preferences * __GetDefPrefs ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Preferences * preferences , __reg ( "d0" ) LONG size ) = "\tjsr\t-126(a6)" ;


 struct Preferences * __GetPrefs ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Preferences * preferences , __reg ( "d0" ) LONG size ) = "\tjsr\t-132(a6)" ;


 void __InitRequester ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Requester * requester ) = "\tjsr\t-138(a6)" ;


 struct MenuItem * __ItemAddress ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct Menu * menuStrip , __reg ( "d0" ) ULONG menuNumber ) = "\tjsr\t-144(a6)" ;


 BOOL __ModifyIDCMP ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "d0" ) ULONG flags ) = "\tjsr\t-150(a6)" ;


 void __ModifyProp ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Gadget * gadget , __reg ( "a1" ) struct Window * window , __reg ( "a2" ) struct Requester * requester , __reg ( "d0" ) ULONG flags , __reg ( "d1" ) ULONG horizPot , __reg ( "d2" ) ULONG vertPot , __reg ( "d3" ) ULONG horizBody , __reg ( "d4" ) ULONG vertBody ) = "\tjsr\t-156(a6)" ;


 void __MoveScreen ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Screen * screen , __reg ( "d0" ) LONG dx , __reg ( "d1" ) LONG dy ) = "\tjsr\t-162(a6)" ;


 void __MoveWindow ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "d0" ) LONG dx , __reg ( "d1" ) LONG dy ) = "\tjsr\t-168(a6)" ;


 void __OffGadget ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Gadget * gadget , __reg ( "a1" ) struct Window * window , __reg ( "a2" ) struct Requester * requester ) = "\tjsr\t-174(a6)" ;


 void __OffMenu ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "d0" ) ULONG menuNumber ) = "\tjsr\t-180(a6)" ;


 void __OnGadget ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Gadget * gadget , __reg ( "a1" ) struct Window * window , __reg ( "a2" ) struct Requester * requester ) = "\tjsr\t-186(a6)" ;


 void __OnMenu ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "d0" ) ULONG menuNumber ) = "\tjsr\t-192(a6)" ;


 struct Screen * __OpenScreen ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct NewScreen * newScreen ) = "\tjsr\t-198(a6)" ;


 struct Window * __OpenWindow ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct NewWindow * newWindow ) = "\tjsr\t-204(a6)" ;


 ULONG __OpenWorkBench ( __reg ( "a6" ) void * ) = "\tjsr\t-210(a6)" ;


 void __PrintIText ( __reg ( "a6" ) void * , __reg ( "a0" ) struct RastPort * rp , __reg ( "a1" ) const struct IntuiText * iText , __reg ( "d0" ) LONG left , __reg ( "d1" ) LONG top ) = "\tjsr\t-216(a6)" ;


 void __RefreshGadgets ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Gadget * gadgets , __reg ( "a1" ) struct Window * window , __reg ( "a2" ) struct Requester * requester ) = "\tjsr\t-222(a6)" ;


 UWORD __RemoveGadget ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "a1" ) struct Gadget * gadget ) = "\tjsr\t-228(a6)" ;


 void __ReportMouse ( __reg ( "a6" ) void * , __reg ( "d0" ) LONG flag , __reg ( "a0" ) struct Window * window ) = "\tjsr\t-234(a6)" ;


 void __ReportMouse1 ( __reg ( "a6" ) void * , __reg ( "d0" ) struct Window * flag , __reg ( "a0" ) void * window ) = "\tjsr\t-234(a6)" ;


 BOOL __Request ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Requester * requester , __reg ( "a1" ) struct Window * window ) = "\tjsr\t-240(a6)" ;


 void __ScreenToBack ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Screen * screen ) = "\tjsr\t-246(a6)" ;


 void __ScreenToFront ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Screen * screen ) = "\tjsr\t-252(a6)" ;


 BOOL __SetDMRequest ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "a1" ) struct Requester * requester ) = "\tjsr\t-258(a6)" ;


 BOOL __SetMenuStrip ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "a1" ) struct Menu * menu ) = "\tjsr\t-264(a6)" ;


 void __SetPointer ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "a1" ) UWORD * pointer , __reg ( "d0" ) LONG height , __reg ( "d1" ) LONG width , __reg ( "d2" ) LONG xOffset , __reg ( "d3" ) LONG yOffset ) = "\tjsr\t-270(a6)" ;


 void __SetWindowTitles ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "a1" ) const STRPTR windowTitle , __reg ( "a2" ) const STRPTR screenTitle ) = "\tjsr\t-276(a6)" ;


 void __ShowTitle ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Screen * screen , __reg ( "d0" ) LONG showIt ) = "\tjsr\t-282(a6)" ;


 void __SizeWindow ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "d0" ) LONG dx , __reg ( "d1" ) LONG dy ) = "\tjsr\t-288(a6)" ;


 struct View * __ViewAddress ( __reg ( "a6" ) void * ) = "\tjsr\t-294(a6)" ;


 struct ViewPort * __ViewPortAddress ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct Window * window ) = "\tjsr\t-300(a6)" ;


 void __WindowToBack ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window ) = "\tjsr\t-306(a6)" ;


 void __WindowToFront ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window ) = "\tjsr\t-312(a6)" ;


 BOOL __WindowLimits ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "d0" ) LONG widthMin , __reg ( "d1" ) LONG heightMin , __reg ( "d2" ) ULONG widthMax , __reg ( "d3" ) ULONG heightMax ) = "\tjsr\t-318(a6)" ;


 struct Preferences * __SetPrefs ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct Preferences * preferences , __reg ( "d0" ) LONG size , __reg ( "d1" ) LONG inform ) = "\tjsr\t-324(a6)" ;


 LONG __IntuiTextLength ( __reg ( "a6" ) void * , __reg ( "a0" ) const struct IntuiText * iText ) = "\tjsr\t-330(a6)" ;


 BOOL __WBenchToBack ( __reg ( "a6" ) void * ) = "\tjsr\t-336(a6)" ;


 BOOL __WBenchToFront ( __reg ( "a6" ) void * ) = "\tjsr\t-342(a6)" ;


 BOOL __AutoRequest ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "a1" ) const struct IntuiText * body , __reg ( "a2" ) const struct IntuiText * posText , __reg ( "a3" ) const struct IntuiText * negText , __reg ( "d0" ) ULONG pFlag , __reg ( "d1" ) ULONG nFlag , __reg ( "d2" ) ULONG width , __reg ( "d3" ) ULONG height ) = "\tjsr\t-348(a6)" ;


 void __BeginRefresh ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window ) = "\tjsr\t-354(a6)" ;


 struct Window * __BuildSysRequest ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "a1" ) const struct IntuiText * body , __reg ( "a2" ) const struct IntuiText * posText , __reg ( "a3" ) const struct IntuiText * negText , __reg ( "d0" ) ULONG flags , __reg ( "d1" ) ULONG width , __reg ( "d2" ) ULONG height ) = "\tjsr\t-360(a6)" ;


 void __EndRefresh ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "d0" ) LONG complete ) = "\tjsr\t-366(a6)" ;


 void __FreeSysRequest ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window ) = "\tjsr\t-372(a6)" ;


 void __MakeScreen ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Screen * screen ) = "\tjsr\t-378(a6)" ;


 void __RemakeDisplay ( __reg ( "a6" ) void * ) = "\tjsr\t-384(a6)" ;


 void __RethinkDisplay ( __reg ( "a6" ) void * ) = "\tjsr\t-390(a6)" ;


 void * __AllocRemember ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Remember * * rememberKey , __reg ( "d0" ) ULONG size , __reg ( "d1" ) ULONG flags ) = "\tjsr\t-396(a6)" ;


 void __FreeRemember ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Remember * * rememberKey , __reg ( "d0" ) LONG reallyForget ) = "\tjsr\t-408(a6)" ;


 ULONG __LockIBase ( __reg ( "a6" ) void * , __reg ( "d0" ) ULONG dontknow ) = "\tjsr\t-414(a6)" ;


 void __UnlockIBase ( __reg ( "a6" ) void * , __reg ( "a0" ) void * ibLock ) = "\tjsr\t-420(a6)" ;


 LONG __GetScreenData ( __reg ( "a6" ) void * , __reg ( "a0" ) void * buffer , __reg ( "d0" ) ULONG size , __reg ( "d1" ) ULONG type , __reg ( "a1" ) const struct Screen * screen ) = "\tjsr\t-426(a6)" ;


 void __RefreshGList ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Gadget * gadgets , __reg ( "a1" ) struct Window * window , __reg ( "a2" ) struct Requester * requester , __reg ( "d0" ) LONG numGad ) = "\tjsr\t-432(a6)" ;


 UWORD __AddGList ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window , __reg ( "a1" ) struct Gadget * gadget , __reg ( "d0" ) ULONG position , __reg ( "d1" ) LONG numGad , __reg ( "a2" ) struct Requester * requester ) = "\tjsr\t-438(a6)" ;


 UWORD __RemoveGList ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * remPtr , __reg ( "a1" ) struct Gadget * gadget , __reg ( "d0" ) LONG numGad ) = "\tjsr\t-444(a6)" ;


 void __ActivateWindow ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window ) = "\tjsr\t-450(a6)" ;


 void __RefreshWindowFrame ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Window * window ) = "\tjsr\t-456(a6)" ;


 BOOL __ActivateGadget ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Gadget * gadgets , __reg ( "a1" ) struct Window * window , __reg ( "a2" ) struct Requester * requester ) = "\tjsr\t-462(a6)" ;


 void __NewModifyProp ( __reg ( "a6" ) void * , __reg ( "a0" ) struct Gadget * gadget , __reg ( "a1" ) struct Window * window , __reg ( "a2" ) struct Requester * requester , __reg ( "d0" ) ULONG flags , __reg ( "d1" ) ULONG horizPot , __reg ( "d2" ) ULONG vertPot , __reg ( "d3" ) ULONG horizBody , __reg ( "d4" ) ULONG vertBody , __reg ( "d5" ) LONG numGad ) = "\tjsr\t-468(a6)" ;
#line 124 "src/platform_amiga/main.c"
 enum {
 RENDER_FP_SHIFT = 8 ,
 RENDER_FP_ONE = 1 << RENDER_FP_SHIFT ,
 RENDER_TILE_FP = 16 * RENDER_FP_ONE
 } ;

 struct GfxBase * GfxBase = 0 ;
 struct IntuitionBase * IntuitionBase = 0 ;
 static struct View * g_old_view = 0 ;
 static ULONG g_ui_anim_tick = 0 ;

 typedef struct CopperState {
 UWORD * list ;
 ULONG bytes ;
 UWORD * bpl_hi [ 5 ] ;
 UWORD * bpl_lo [ 5 ] ;
 UWORD * spr_hi [ 8 ] ;
 UWORD * spr_lo [ 8 ] ;
 } CopperState ;

 typedef struct StaticCell {
 UBYTE terrain ;
 UBYTE block ;
 UBYTE item ;
 UBYTE mine_sprite ;
 } StaticCell ;

 typedef struct DirtyRect {
 WORD x ;
 WORD y ;
 WORD w ;
 WORD h ;
 } DirtyRect ;

 typedef struct ModalCache {
 UBYTE valid ;
 UBYTE active ;
 ULONG key ;
 DirtyRect rect ;
 } ModalCache ;

 typedef struct DynamicCache {
 UBYTE valid ;
 ULONG key ;
 DirtyRect rect ;
 } DynamicCache ;

 typedef struct FrameSync {
 UWORD last_line ;
 UBYTE wrapped ;
 } FrameSync ;

 typedef struct HudSnapshot {
 ULONG score ;
 UWORD round ;
 UWORD timer_seconds ;
 UWORD lives ;
 UWORD mine_stock ;
 UWORD mine_capacity ;
 UBYTE timer_danger ;
 UBYTE timer_flash_phase ;
 UBYTE bomb_flash_phase ;
 UBYTE special_border_phase ;
 } HudSnapshot ;

 typedef struct RendererState {
 UBYTE dirty [ 2 ] [ 12 ] [ 20 ] ;
 StaticCell prev_static [ 2 ] [ 12 ] [ 20 ] ;
 UBYTE prev_valid [ 2 ] ;
 UBYTE prev_hud_valid [ 2 ] ;
 HudSnapshot prev_hud [ 2 ] ;
 ModalCache modal [ 2 ] ;
 DynamicCache cached_player [ 2 ] ;
 DynamicCache cached_block [ 2 ] [ 8 ] ;
 DynamicCache cached_enemy [ 2 ] [ 16 ] ;
 UBYTE enemy_hw_sprite [ 2 ] [ 16 ] ;
 DirtyRect prev_dynamic [ 2 ] [ 64 ] ;
 WORD prev_dynamic_count [ 2 ] ;
 } RendererState ;

 typedef enum AmigaSfxWave {
 AMIGA_SFX_WAVE_SILENCE = 0 ,
 AMIGA_SFX_WAVE_TONE = 1 ,
 AMIGA_SFX_WAVE_NOISE = 2
 } AmigaSfxWave ;

 typedef struct AmigaSfxStep {
 UWORD period_start ;
 UWORD period_end ;
 UWORD ticks ;
 UBYTE volume ;
 UBYTE wave ;
 } AmigaSfxStep ;

 typedef struct AmigaSfxVoice {
 AmigaSfxStep steps [ 32 ] ;
 AmigaSfxStep current ;
 WORD period_current ;
 WORD period_step ;
 WORD period_adjust ;
 UWORD period_den ;
 UWORD period_remainder ;
 UWORD period_error ;
 UBYTE channel ;
 UBYTE head ;
 UBYTE count ;
 UBYTE active ;
 UWORD elapsed ;
 } AmigaSfxVoice ;

 typedef struct AmigaSfxState {
 UBYTE * samples ;
 AmigaSfxVoice voice [ 2 ] ;
 UBYTE enabled ;
 } AmigaSfxState ;

 typedef struct AmigaSfxPatternStep {
 UWORD hz ;
 UWORD duration_ms ;
 UWORD amplitude ;
 UWORD rest_ms ;
 } AmigaSfxPatternStep ;

 typedef struct AmigaApp {
 UBYTE * screen [ 2 ] ;
 UBYTE * chip_tiles ;
 UBYTE * chip_tile_masks ;
 UBYTE * chip_bobs ;
 UBYTE * chip_masks ;
 UBYTE * chip_title ;
 UBYTE * chip_title_mask ;
 UBYTE * hw_sprite_bank [ 2 ] ;
 UWORD hw_enemy_lines [ AMIGA_SPR_COUNT ] [ 16 ] [ 4 ] ;
 AmigaSfxState sfx ;
 CopperState copper ;
 RendererState renderer ;
 BOOL wb_was_open ;
 BOOL owns_blitter ;
 } AmigaApp ;

 static AmigaApp g_app ;
 static GameState g_game ;

 static const AmigaSpriteId kTerrainSprites [ 3 ] = {
 AMIGA_SPR_FLOOR ,
 AMIGA_SPR_WALL ,
 AMIGA_SPR_BORDER
 } ;

 static const AmigaSpriteId kBlockSprites [ 5 ] = {
 AMIGA_SPR_FLOOR ,
 AMIGA_SPR_BLOCK_ICE ,
 AMIGA_SPR_BLOCK_ICE ,
 AMIGA_SPR_BLOCK_ICE ,
 AMIGA_SPR_BLOCK_ICE
 } ;

 static const AmigaSpriteId kItemSprites [ 6 ] = {
 AMIGA_SPR_FLOOR ,
 AMIGA_SPR_ITEM_FISH ,
 AMIGA_SPR_ITEM_EGG ,
 AMIGA_SPR_ITEM_GEM ,
 AMIGA_SPR_ITEM_FRUIT ,
 AMIGA_SPR_ITEM_MYSTERY
 } ;

 typedef struct OpeningHiScore {
 const char * initials ;
 uint32_t score ;
 } OpeningHiScore ;

 static const OpeningHiScore kOpeningHiScores [ 5 ] = {
 { "ACE" , 50000u } ,
 { "ICE" , 35000u } ,
 { "PAN" , 24000u } ,
 { "FOX" , 16000u } ,
 { "JET" , 9000u }
 } ;

 static const UBYTE kDigit3x5 [ 10 ] [ 5 ] = {
 { 0x7 , 0x5 , 0x5 , 0x5 , 0x7 } ,
 { 0x2 , 0x6 , 0x2 , 0x2 , 0x7 } ,
 { 0x7 , 0x1 , 0x7 , 0x4 , 0x7 } ,
 { 0x7 , 0x1 , 0x7 , 0x1 , 0x7 } ,
 { 0x5 , 0x5 , 0x7 , 0x1 , 0x1 } ,
 { 0x7 , 0x4 , 0x7 , 0x1 , 0x7 } ,
 { 0x7 , 0x4 , 0x7 , 0x5 , 0x7 } ,
 { 0x7 , 0x1 , 0x1 , 0x1 , 0x1 } ,
 { 0x7 , 0x5 , 0x7 , 0x5 , 0x7 } ,
 { 0x7 , 0x5 , 0x7 , 0x1 , 0x7 }
 } ;

 static inline int clamp_int ( int value , int lo , int hi ) {
 if ( value < lo ) {
 return lo ;
 }
 if ( value > hi ) {
 return hi ;
 }
 return value ;
 }

 static inline int dir_dx ( Direction d ) {
 if ( d == DIR_LEFT ) {
 return - 1 ;
 }
 if ( d == DIR_RIGHT ) {
 return 1 ;
 }
 return 0 ;
 }

 static inline int dir_dy ( Direction d ) {
 if ( d == DIR_UP ) {
 return - 1 ;
 }
 if ( d == DIR_DOWN ) {
 return 1 ;
 }
 return 0 ;
 }

 static inline bool cells_equal ( const StaticCell * a , const StaticCell * b ) {
 return a -> terrain == b -> terrain &&
 a -> block == b -> block &&
 a -> item == b -> item &&
 a -> mine_sprite == b -> mine_sprite ;
 }

 static inline bool item_should_render ( int timer_ticks ) {
 if ( timer_ticks <= 0 ) {
 return 0 ;
 }
 if ( timer_ticks > 2 * 50 ) {
 return 1 ;
 }
 if ( timer_ticks > 50 ) {
 return ( ( timer_ticks / 8 ) & 1 ) == 0 ;
 }
 return ( ( timer_ticks / 4 ) & 1 ) == 0 ;
 }

 static AmigaSpriteId mine_sprite_for_fuse_ticks ( int fuse_ticks , int phase ) {
 if ( fuse_ticks <= 50 ) {
 return ( ( phase & 1 ) == 0 ) ? AMIGA_SPR_MINE_2 : AMIGA_SPR_MINE_1 ;
 }
 if ( fuse_ticks <= 2 * 50 ) {
 return ( ( phase & 2 ) == 0 ) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_0 ;
 }
 return ( ( phase & 7 ) == 0 ) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_0 ;
 }

 static AmigaSpriteId player_sprite_for_direction ( Direction d , bool alt ) {
 switch ( d ) {
 case DIR_UP :
 return alt ? AMIGA_SPR_PLAYER_UP_ALT : AMIGA_SPR_PLAYER_UP ;
 case DIR_DOWN :
 return alt ? AMIGA_SPR_PLAYER_DOWN_ALT : AMIGA_SPR_PLAYER_DOWN ;
 case DIR_LEFT :
 return alt ? AMIGA_SPR_PLAYER_LEFT_ALT : AMIGA_SPR_PLAYER_LEFT ;
 case DIR_RIGHT :
 default :
 return alt ? AMIGA_SPR_PLAYER_RIGHT_ALT : AMIGA_SPR_PLAYER_RIGHT ;
 }
 }

 static AmigaSpriteId enemy_sprite_for_state ( const Enemy * enemy ) {
 const int phase = ( enemy -> pixel_fp_x + enemy -> pixel_fp_y ) >> 9 ;
 const bool alt = ( ( phase & 1 ) != 0 ) ;
 if ( enemy -> type == ENEMY_TYPE_WANDERER ) {
 return alt ? AMIGA_SPR_ENEMY_B_ALT : AMIGA_SPR_ENEMY_B ;
 }
 return alt ? AMIGA_SPR_ENEMY_A_ALT : AMIGA_SPR_ENEMY_A ;
 }

 static AmigaSpriteId death_sprite_for_fx ( const ImpactFx * fx ) {
 const int age = fx -> base_ttl_ticks - fx -> ttl_ticks ;
 const bool late = ( age >= ( fx -> base_ttl_ticks / 2 ) ) ;
 if ( fx -> style == IMPACT_FX_STYLE_ENEMY_DEATH_B ) {
 return late ? AMIGA_SPR_ENEMY_B_DEATH_1 : AMIGA_SPR_ENEMY_B_DEATH_0 ;
 }
 return late ? AMIGA_SPR_ENEMY_A_DEATH_1 : AMIGA_SPR_ENEMY_A_DEATH_0 ;
 }

 static const char * perk_label ( PerkType perk ) {
 switch ( perk ) {
 case PERK_LIFE_BOOST :
 return "LIFE" ;
 case PERK_TIME_BOOST :
 return "TIME" ;
 case PERK_SCORE_BOOST :
 return "SCORE" ;
 case PERK_ENEMY_SLOW :
 return "SLOW" ;
 case PERK_MINES :
 return "MINES" ;
 default :
 return "MYST" ;
 }
 }

 static const char * perk_effect_label ( PerkType perk ) {
 switch ( perk ) {
 case PERK_LIFE_BOOST :
 return "GAIN 1 LIFE MAX 7" ;
 case PERK_TIME_BOOST :
 return "ADD TIME LESS EACH LV" ;
 case PERK_SCORE_BOOST :
 return "SCORE BONUS TAPERS" ;
 case PERK_ENEMY_SLOW :
 return "SLOW BONUS TAPERS" ;
 case PERK_MINES :
 return "FIRE DROP MINE" ;
 default :
 return "NO EFFECT" ;
 }
 }

 static UBYTE perk_color ( PerkType perk ) {
 switch ( perk ) {
 case PERK_LIFE_BOOST :
 return 23 ;
 case PERK_TIME_BOOST :
 return 14 ;
 case PERK_SCORE_BOOST :
 return 8 ;
 case PERK_ENEMY_SLOW :
 return 11 ;
 case PERK_MINES :
 return 22 ;
 default :
 return 30 ;
 }
 }

 static AmigaSpriteId perk_sprite ( PerkType perk ) {
 switch ( perk ) {
 case PERK_LIFE_BOOST :
 return AMIGA_SPR_HUD_LIFE ;
 case PERK_TIME_BOOST :
 return AMIGA_SPR_ITEM_EGG ;
 case PERK_SCORE_BOOST :
 return AMIGA_SPR_ITEM_GEM ;
 case PERK_ENEMY_SLOW :
 return AMIGA_SPR_ITEM_FISH ;
 case PERK_MINES :
 return AMIGA_SPR_EFFECT_SPARK ;
 default :
 return AMIGA_SPR_ITEM_MYSTERY ;
 }
 }

 static const char * meta_label ( MetaUpgradeType upgrade ) {
 switch ( upgrade ) {
 case META_UPGRADE_LIFE_PACK :
 return "LIFE" ;
 case META_UPGRADE_TIME_PACK :
 return "TIME" ;
 case META_UPGRADE_SCORE_PACK :
 return "SCORE" ;
 case META_UPGRADE_SLOW_PACK :
 return "SLOW" ;
 default :
 return "NONE" ;
 }
 }

 static const char * meta_effect_label ( MetaUpgradeType upgrade ) {
 switch ( upgrade ) {
 case META_UPGRADE_LIFE_PACK :
 return "NEW RUN 1 LIFE" ;
 case META_UPGRADE_TIME_PACK :
 return "NEW RUN TIME" ;
 case META_UPGRADE_SCORE_PACK :
 return "NEW RUN SCORE" ;
 case META_UPGRADE_SLOW_PACK :
 return "NEW RUN SLOW" ;
 default :
 return "NO EFFECT" ;
 }
 }

 static UBYTE meta_color ( MetaUpgradeType upgrade ) {
 switch ( upgrade ) {
 case META_UPGRADE_LIFE_PACK :
 return 23 ;
 case META_UPGRADE_TIME_PACK :
 return 14 ;
 case META_UPGRADE_SCORE_PACK :
 return 8 ;
 case META_UPGRADE_SLOW_PACK :
 return 11 ;
 default :
 return 30 ;
 }
 }

 static AmigaSpriteId meta_sprite ( MetaUpgradeType upgrade ) {
 switch ( upgrade ) {
 case META_UPGRADE_LIFE_PACK :
 return AMIGA_SPR_HUD_LIFE ;
 case META_UPGRADE_TIME_PACK :
 return AMIGA_SPR_ITEM_EGG ;
 case META_UPGRADE_SCORE_PACK :
 return AMIGA_SPR_ITEM_GEM ;
 case META_UPGRADE_SLOW_PACK :
 return AMIGA_SPR_ITEM_FISH ;
 default :
 return AMIGA_SPR_ITEM_MYSTERY ;
 }
 }

 static UBYTE stage_modifier_color ( StageModifierType modifier ) {
 switch ( modifier ) {
 case STAGE_MOD_SWARM :
 return 23 ;
 case STAGE_MOD_SHATTERED_ICE :
 return 27 ;
 case STAGE_MOD_BLACK_ICE :
 return 29 ;
 case STAGE_MOD_TURBO_SLIDE :
 return 8 ;
 case STAGE_MOD_FROST_SIREN :
 return 12 ;
 default :
 return 3 ;
 }
 }

 static inline void wait_blitter ( void ) {
 volatile UWORD * dmaconr = ( volatile UWORD * ) 0xDFF002 ;
 ( void ) * dmaconr ;
 while ( ( * dmaconr & 0x4000 ) != 0 ) {
 }
 }

 static inline UWORD read_raster_line ( void ) {
 UWORD vpos_hi ;
 UWORD vhpos_a ;
 UWORD vhpos_b ;
 do {
 vhpos_a = ( ( volatile struct Custom * ) 0xDFF000 ) -> vhposr ;
 vpos_hi = ( ( volatile struct Custom * ) 0xDFF000 ) -> vposr ;
 vhpos_b = ( ( volatile struct Custom * ) 0xDFF000 ) -> vhposr ;
 } while ( ( vhpos_a ^ vhpos_b ) & 0xFF00u ) ;
 return ( UWORD ) ( ( ( vpos_hi & 0x0001u ) << 8 ) | ( ( vhpos_b >> 8 ) & 0x00FFu ) ) ;
 }

 static void wait_frame_boundary_hw ( void ) {
 while ( read_raster_line ( ) < 250u ) {
 }
 while ( read_raster_line ( ) >= 250u ) {
 }
 }

 static inline void frame_sync_begin ( FrameSync * sync ) {
 sync -> last_line = read_raster_line ( ) ;
 sync -> wrapped = 0 ;
 }

 static inline void frame_sync_sample ( FrameSync * sync ) {
 UWORD line = read_raster_line ( ) ;
 if ( ( UWORD ) ( line + 8u ) < sync -> last_line ) {
 sync -> wrapped = 1 ;
 }
 sync -> last_line = line ;
 }

 static void frame_sync_finish ( FrameSync * sync ) {
 frame_sync_sample ( sync ) ;
 if ( ! sync -> wrapped ) {
 wait_frame_boundary_hw ( ) ;
 }
 }

 static inline const UBYTE * bob_plane_ptr ( const AmigaApp * app , AmigaSpriteId sprite , int plane ) {
 return app -> chip_bobs +
 ( ( ULONG ) sprite * 320 ) +
 ( ( ULONG ) plane * 4 ) ;
 }

 static inline const UBYTE * mask_ptr ( const AmigaApp * app , AmigaSpriteId sprite ) {
 return app -> chip_masks + ( ( ULONG ) sprite * 320 ) ;
 }

 static inline const UBYTE * tile_ptr ( const AmigaApp * app , AmigaSpriteId sprite ) {
 return app -> chip_tiles + ( ( ULONG ) sprite * 160 ) ;
 }

 static inline UBYTE * screen_plane_ptr ( UBYTE * screen , int x , int y , int plane ) {
 return screen + ( ( ULONG ) y * ( ( 320 / 8 ) * 5 ) ) + ( ( ULONG ) plane * ( 320 / 8 ) ) + ( ( ULONG ) ( x >> 4 ) * 2 ) ;
 }

 static APTR alloc_fast_or_public ( ULONG bytes ) {
 APTR p = __AllocMem ( SysBase , ( bytes ) , ( 0x00000004UL | 0x00010000UL ) ) ;
 if ( ! p ) {
 p = __AllocMem ( SysBase , ( bytes ) , ( 0x00000001UL | 0x00010000UL ) ) ;
 }
 return p ;
 }

 static void blit_opaque_tile ( const AmigaApp * app , UBYTE * screen , AmigaSpriteId sprite , int x , int y ) {
 const UBYTE * src ;
 UBYTE * dst ;
 x = clamp_int ( x , 0 , 320 - 16 ) ;
 y = clamp_int ( y , 0 , 200 - 16 ) ;
 x &= ~ 15 ;
 src = tile_ptr ( app , sprite ) ;
 dst = screen + ( ( ULONG ) y * ( ( 320 / 8 ) * 5 ) ) + ( ( ULONG ) ( x >> 4 ) * 2 ) ;

 wait_blitter ( ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltcon0 = 0x0400 | 0x0100 | 0x00CC ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltcon1 = 0 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltafwm = 0xFFFF ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltalwm = 0xFFFF ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltbpt = ( APTR ) src ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltdpt = ( APTR ) dst ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltbmod = 0 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltdmod = ( 320 / 8 ) - 2 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltsize = ( UWORD ) ( ( ( 16 * 5 ) << 6 ) | 1 ) ;
 }

 static void blit_cookie_bob ( const AmigaApp * app , UBYTE * screen , AmigaSpriteId sprite , int x , int y ) {
 int shift ;
 const UBYTE * mask ;
 const UBYTE * src ;
 UBYTE * dst ;
 x = clamp_int ( x , 0 , 320 - ( 16 * 2 ) ) ;
 y = clamp_int ( y , 0 , 200 - 16 ) ;
 shift = x & 15 ;
 mask = app -> chip_masks + ( ( ULONG ) sprite * 320 ) ;
 src = app -> chip_bobs + ( ( ULONG ) sprite * 320 ) ;
 dst = screen + ( ( ULONG ) y * ( ( 320 / 8 ) * 5 ) ) + ( ( ULONG ) ( x >> 4 ) * 2 ) ;

 wait_blitter ( ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltcon0 = ( UWORD ) ( ( shift << 12 ) | 0x0800 | 0x0400 | 0x0200 | 0x0100 | 0x00CA ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltcon1 = ( UWORD ) ( shift << 12 ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltafwm = 0xFFFF ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltalwm = 0xFFFF ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltapt = ( APTR ) mask ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltbpt = ( APTR ) src ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltcpt = ( APTR ) dst ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltdpt = ( APTR ) dst ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltamod = 0 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltbmod = 0 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltcmod = ( 320 / 8 ) - 4 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltdmod = ( 320 / 8 ) - 4 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltsize = ( UWORD ) ( ( ( 16 * 5 ) << 6 ) | 2 ) ;
 }

 static void blit_title_overlay ( const AmigaApp * app , UBYTE * screen , int x , int y ) {
 int shift ;
 UBYTE * dst ;
 if ( ! app -> chip_title || ! app -> chip_title_mask ) {
 return ;
 }
 x = clamp_int ( x , 0 , 320 - ( int ) ( 9 * 16 ) ) ;
 y = clamp_int ( y , 0 , 200 - 83 ) ;
 shift = x & 15 ;
 dst = screen + ( ( ULONG ) y * ( ( 320 / 8 ) * 5 ) ) + ( ( ULONG ) ( x >> 4 ) * 2 ) ;

 wait_blitter ( ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltcon0 = ( UWORD ) ( ( shift << 12 ) | 0x0800 | 0x0400 | 0x0200 | 0x0100 | 0x00CA ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltcon1 = ( UWORD ) ( shift << 12 ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltafwm = 0xFFFF ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltalwm = 0xFFFF ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltapt = ( APTR ) app -> chip_title_mask ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltbpt = ( APTR ) app -> chip_title ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltcpt = ( APTR ) dst ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltdpt = ( APTR ) dst ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltamod = 0 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltbmod = 0 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltcmod = ( 320 / 8 ) - 18 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltdmod = ( 320 / 8 ) - 18 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> bltsize = ( UWORD ) ( ( ( 83 * 5 ) << 6 ) | 9 ) ;
 }

 static inline UWORD read_be_word ( const UBYTE * p ) {
 return ( UWORD ) ( ( ( UWORD ) p [ 0 ] << 8 ) | p [ 1 ] ) ;
 }

 static bool enemy_sprite_can_use_hardware ( AmigaSpriteId sprite ) {
 return sprite == AMIGA_SPR_ENEMY_A || sprite == AMIGA_SPR_ENEMY_A_ALT ;
 }

 static void hw_enemy_layer_colors ( AmigaSpriteId sprite , UBYTE source_color , UBYTE * main_color , UBYTE * detail_color ) {
 * main_color = 0 ;
 * detail_color = 0 ;
 switch ( sprite ) {
 case AMIGA_SPR_ENEMY_A :
 case AMIGA_SPR_ENEMY_A_ALT :
 if ( source_color == 2 ) {
 * detail_color = 1 ;
 } else if ( source_color == 17 ) {
 * main_color = 1 ;
 } else if ( source_color == 18 ) {
 * main_color = 2 ;
 } else if ( source_color == 19 ) {
 * main_color = 3 ;
 } else if ( source_color == 31 ) {
 * detail_color = 3 ;
 }
 break ;
 default :
 break ;
 }
 }

 static void build_hw_enemy_sprite_lines ( AmigaApp * app ) {
 int sprite ;
 int y ;
 int x ;
 int plane ;

 __asm_memset ( app -> hw_enemy_lines , 0 , sizeof ( app -> hw_enemy_lines ) ) ;
 for ( sprite = 0 ; sprite < AMIGA_SPR_COUNT ; ++ sprite ) {
 for ( y = 0 ; y < 16 ; ++ y ) {
 UWORD plane_words [ 5 ] ;
 for ( plane = 0 ; plane < 5 ; ++ plane ) {
 const UBYTE * src = ( ( const UBYTE * ) g_amiga_tile_data ) +
 ( ( ULONG ) sprite * 160 ) +
 ( ( ULONG ) y * 5 * 2 ) +
 ( ( ULONG ) plane * 2 ) ;
 plane_words [ plane ] = read_be_word ( src ) ;
 }
 for ( x = 0 ; x < 16 ; ++ x ) {
 UBYTE source_color = 0 ;
 UBYTE main_color ;
 UBYTE detail_color ;
 UWORD bit = ( UWORD ) ( 1u << ( 15 - x ) ) ;
 int bit_index = 15 - x ;
 for ( plane = 0 ; plane < 5 ; ++ plane ) {
 if ( ( plane_words [ plane ] & bit ) != 0 ) {
 source_color |= ( UBYTE ) ( 1u << plane ) ;
 }
 }
 hw_enemy_layer_colors ( ( AmigaSpriteId ) sprite , source_color , & main_color , & detail_color ) ;
 if ( ( main_color & 2u ) != 0 ) app -> hw_enemy_lines [ sprite ] [ y ] [ 0 ] |= ( UWORD ) ( 1u << bit_index ) ;
 if ( ( main_color & 1u ) != 0 ) app -> hw_enemy_lines [ sprite ] [ y ] [ 1 ] |= ( UWORD ) ( 1u << bit_index ) ;
 if ( ( detail_color & 2u ) != 0 ) app -> hw_enemy_lines [ sprite ] [ y ] [ 2 ] |= ( UWORD ) ( 1u << bit_index ) ;
 if ( ( detail_color & 1u ) != 0 ) app -> hw_enemy_lines [ sprite ] [ y ] [ 3 ] |= ( UWORD ) ( 1u << bit_index ) ;
 }
 }
 }
 }

 static void clear_hw_sprite_slot ( UBYTE * bank , int slot ) {
 UWORD * dst = ( UWORD * ) ( bank + ( ( ULONG ) slot * ( ( ULONG ) ( 2 + ( 16 * 2 ) + 2 ) * sizeof ( UWORD ) ) ) ) ;
 dst [ 0 ] = 0 ;
 dst [ 1 ] = 0 ;
 dst [ ( 2 + ( 16 * 2 ) + 2 ) - 2 ] = 0 ;
 dst [ ( 2 + ( 16 * 2 ) + 2 ) - 1 ] = 0 ;
 }

 static void clear_hw_sprite_bank ( UBYTE * bank ) {
 int i ;
 for ( i = 0 ; i < 8 ; ++ i ) {
 clear_hw_sprite_slot ( bank , i ) ;
 }
 }

 static void write_hw_sprite_control ( UWORD * dst , int x , int y , bool attached ) {
 int hwx = clamp_int ( x , 0 , 320 - 16 ) + 0x80 ;
 int vstart = clamp_int ( y , 0 , ( 12 * 16 ) - 16 ) + 0x2C ;
 int vstop = vstart + 16 ;
 dst [ 0 ] = ( UWORD ) ( ( ( vstart & 0xFF ) << 8 ) | ( ( hwx >> 1 ) & 0xFF ) ) ;
 dst [ 1 ] = ( UWORD ) ( ( ( vstop & 0xFF ) << 8 ) |
 ( ( vstart & 0x100 ) ? 0x0004 : 0 ) |
 ( ( vstop & 0x100 ) ? 0x0002 : 0 ) |
 ( hwx & 1 ) |
 ( attached ? 0x0080 : 0 ) ) ;
 }

 static void write_hw_enemy_sprite_layers ( const AmigaApp * app , int buf , int layer_index , AmigaSpriteId sprite , int x , int y ) {
 UBYTE * bank = app -> hw_sprite_bank [ buf ] ;
 int main_slot = layer_index ;
 int detail_slot = 6 + layer_index ;
 UWORD * main_layer = ( UWORD * ) ( bank + ( ( ULONG ) main_slot * ( ( ULONG ) ( 2 + ( 16 * 2 ) + 2 ) * sizeof ( UWORD ) ) ) ) ;
 UWORD * detail_layer = ( UWORD * ) ( bank + ( ( ULONG ) detail_slot * ( ( ULONG ) ( 2 + ( 16 * 2 ) + 2 ) * sizeof ( UWORD ) ) ) ) ;
 int row ;

 write_hw_sprite_control ( main_layer , x , y , 0 ) ;
 write_hw_sprite_control ( detail_layer , x , y , 0 ) ;
 for ( row = 0 ; row < 16 ; ++ row ) {
 main_layer [ 2 + row * 2 ] = app -> hw_enemy_lines [ sprite ] [ row ] [ 0 ] ;
 main_layer [ 3 + row * 2 ] = app -> hw_enemy_lines [ sprite ] [ row ] [ 1 ] ;
 detail_layer [ 2 + row * 2 ] = app -> hw_enemy_lines [ sprite ] [ row ] [ 2 ] ;
 detail_layer [ 3 + row * 2 ] = app -> hw_enemy_lines [ sprite ] [ row ] [ 3 ] ;
 }
 main_layer [ ( 2 + ( 16 * 2 ) + 2 ) - 2 ] = 0 ;
 main_layer [ ( 2 + ( 16 * 2 ) + 2 ) - 1 ] = 0 ;
 detail_layer [ ( 2 + ( 16 * 2 ) + 2 ) - 2 ] = 0 ;
 detail_layer [ ( 2 + ( 16 * 2 ) + 2 ) - 1 ] = 0 ;
 }

 static UWORD sfx_dma_bit ( UBYTE channel ) {
 return ( UWORD ) ( 0x0001 << channel ) ;
 }

 static UWORD sfx_period_for_hz ( int hz , int wave_bytes ) {
 ULONG denom ;
 ULONG period ;
 if ( hz < 20 ) {
 hz = 20 ;
 }
 if ( wave_bytes <= 0 ) {
 wave_bytes = 8 ;
 }
 denom = ( ULONG ) hz * ( ULONG ) wave_bytes ;
 period = denom ? ( 3546895UL / denom ) : 65535UL ;
 if ( period < 124UL ) {
 period = 124UL ;
 }
 if ( period > 65535UL ) {
 period = 65535UL ;
 }
 return ( UWORD ) period ;
 }

 static UBYTE sfx_volume_from_amplitude ( int amplitude ) {
 int volume = ( amplitude + 180 ) / 260 ;
 return ( UBYTE ) clamp_int ( volume , 1 , 64 ) ;
 }

 static UWORD sfx_ticks_from_ms ( int ms ) {
 int ticks ;
 if ( ms <= 0 ) {
 return 0 ;
 }
 ticks = ( ms * 50 + 999 ) / 1000 ;
 return ( UWORD ) clamp_int ( ticks , 1 , 255 ) ;
 }

 static UBYTE * sfx_wave_ptr ( const AmigaSfxState * sfx , UBYTE wave ) {
 if ( ! sfx -> samples ) {
 return 0 ;
 }
 if ( wave == AMIGA_SFX_WAVE_NOISE ) {
 return sfx -> samples + 8 ;
 }
 return sfx -> samples + 0 ;
 }

 static UWORD sfx_wave_len_words ( UBYTE wave ) {
 if ( wave == AMIGA_SFX_WAVE_NOISE ) {
 return ( UWORD ) ( 64 / 2 ) ;
 }
 return ( UWORD ) ( 8 / 2 ) ;
 }

 static void sfx_disable_voice ( UBYTE channel ) {
 ( ( volatile struct Custom * ) 0xDFF000 ) -> aud [ channel ] . ac_vol = 0 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> dmacon = sfx_dma_bit ( channel ) ;
 }

 static void sfx_voice_clear ( AmigaSfxVoice * voice ) {
 UBYTE channel = voice -> channel ;
 __asm_memset ( voice , 0 , sizeof ( * voice ) ) ;
 voice -> channel = channel ;
 sfx_disable_voice ( channel ) ;
 }

 static void sfx_interrupt ( AmigaSfxState * sfx ) {
 if ( ! sfx || ! sfx -> enabled ) {
 return ;
 }
 sfx_voice_clear ( & sfx -> voice [ 0 ] ) ;
 sfx_voice_clear ( & sfx -> voice [ 1 ] ) ;
 }

 static BOOL sfx_voice_push ( AmigaSfxVoice * voice , const AmigaSfxStep * step ) {
 UBYTE tail ;
 if ( ! step || step -> ticks == 0 ) {
 return 0 ;
 }
 if ( voice -> count >= 32 ) {
 return 0 ;
 }
 tail = ( UBYTE ) ( voice -> head + voice -> count ) ;
 if ( tail >= 32 ) {
 tail = ( UBYTE ) ( tail - 32 ) ;
 }
 voice -> steps [ tail ] = * step ;
 ++ voice -> count ;
 return 1 ;
 }

 static void sfx_queue_silence_voice ( AmigaSfxVoice * voice , int duration_ms ) {
 AmigaSfxStep step ;
 __asm_memset ( & step , 0 , sizeof ( step ) ) ;
 step . wave = AMIGA_SFX_WAVE_SILENCE ;
 step . ticks = sfx_ticks_from_ms ( duration_ms ) ;
 ( void ) sfx_voice_push ( voice , & step ) ;
 }

 static void sfx_queue_tone_sweep (
 AmigaSfxState * sfx ,
 int hz_start ,
 int hz_end ,
 int duration_ms ,
 int amplitude ,
 int duty_percent ) {
 AmigaSfxStep step ;
 if ( ! sfx || ! sfx -> enabled ) {
 return ;
 }
 if ( hz_end <= 0 ) {
 hz_end = hz_start ;
 }
 __asm_memset ( & step , 0 , sizeof ( step ) ) ;
 step . wave = AMIGA_SFX_WAVE_TONE ;
 step . period_start = sfx_period_for_hz ( hz_start , 8 ) ;
 step . period_end = sfx_period_for_hz ( hz_end , 8 ) ;
 step . ticks = sfx_ticks_from_ms ( duration_ms ) ;
 amplitude = ( amplitude * clamp_int ( duty_percent , 20 , 80 ) + 25 ) / 50 ;
 step . volume = sfx_volume_from_amplitude ( amplitude ) ;
 ( void ) sfx_voice_push ( & sfx -> voice [ 0 ] , & step ) ;
 }

 static void sfx_queue_tone ( AmigaSfxState * sfx , int hz , int duration_ms , int amplitude ) {
 sfx_queue_tone_sweep ( sfx , hz , hz , duration_ms , amplitude , 50 ) ;
 }

 static void sfx_queue_silence ( AmigaSfxState * sfx , int duration_ms ) {
 if ( ! sfx || ! sfx -> enabled ) {
 return ;
 }
 sfx_queue_silence_voice ( & sfx -> voice [ 0 ] , duration_ms ) ;
 }

 static void sfx_queue_noise_burst ( AmigaSfxState * sfx , int duration_ms , int amplitude ) {
 AmigaSfxStep step ;
 if ( ! sfx || ! sfx -> enabled ) {
 return ;
 }
 __asm_memset ( & step , 0 , sizeof ( step ) ) ;
 step . wave = AMIGA_SFX_WAVE_NOISE ;
 step . period_start = sfx_period_for_hz ( 2200 , 64 ) ;
 step . period_end = sfx_period_for_hz ( 900 , 64 ) ;
 step . ticks = sfx_ticks_from_ms ( duration_ms ) ;
 step . volume = sfx_volume_from_amplitude ( amplitude ) ;
 ( void ) sfx_voice_push ( & sfx -> voice [ 1 ] , & step ) ;
 }

 static void sfx_queue_pattern ( AmigaSfxState * sfx , const AmigaSfxPatternStep * steps , int step_count ) {
 int i ;
 if ( ! sfx || ! sfx -> enabled || ! steps || step_count <= 0 ) {
 return ;
 }
 for ( i = 0 ; i < step_count ; ++ i ) {
 const AmigaSfxPatternStep * step = & steps [ i ] ;
 if ( step -> hz > 0 && step -> duration_ms > 0 ) {
 sfx_queue_tone ( sfx , step -> hz , step -> duration_ms , step -> amplitude ) ;
 } else if ( step -> duration_ms > 0 ) {
 sfx_queue_silence ( sfx , step -> duration_ms ) ;
 }
 if ( step -> rest_ms > 0 ) {
 sfx_queue_silence ( sfx , step -> rest_ms ) ;
 }
 }
 }

 static void sfx_queue_slot_chime ( AmigaSfxState * sfx ) {
 sfx_queue_tone_sweep ( sfx , 620 , 760 , 30 , 13600 , 36 ) ;
 sfx_queue_silence ( sfx , 6 ) ;
 sfx_queue_tone_sweep ( sfx , 820 , 1020 , 34 , 14600 , 34 ) ;
 sfx_queue_silence ( sfx , 6 ) ;
 sfx_queue_tone_sweep ( sfx , 1080 , 1360 , 38 , 15600 , 31 ) ;
 sfx_queue_silence ( sfx , 8 ) ;
 sfx_queue_tone_sweep ( sfx , 1420 , 1880 , 58 , 16800 , 28 ) ;
 }

 static void sfx_queue_reward_jingle ( AmigaSfxState * sfx ) {
 sfx_queue_tone_sweep ( sfx , 700 , 860 , 34 , 12800 , 32 ) ;
 sfx_queue_silence ( sfx , 6 ) ;
 sfx_queue_tone_sweep ( sfx , 920 , 1120 , 36 , 13600 , 30 ) ;
 sfx_queue_silence ( sfx , 6 ) ;
 sfx_queue_tone_sweep ( sfx , 1160 , 1460 , 40 , 14600 , 28 ) ;
 sfx_queue_silence ( sfx , 8 ) ;
 sfx_queue_tone_sweep ( sfx , 1540 , 1980 , 74 , 16600 , 24 ) ;
 }

 static void sfx_queue_level_up_fanfare ( AmigaSfxState * sfx ) {
 static const AmigaSfxPatternStep kLevelUp [ ] = {
 { 587 , 52 , 8600 , 5 } ,
 { 740 , 56 , 9000 , 5 } ,
 { 880 , 60 , 9500 , 6 } ,
 { 988 , 64 , 10000 , 6 } ,
 { 1175 , 74 , 10600 , 8 } ,
 { 1319 , 84 , 11400 , 10 }
 } ;
 sfx_queue_pattern ( sfx , kLevelUp , ( int ) ( sizeof ( kLevelUp ) / sizeof ( kLevelUp [ 0 ] ) ) ) ;
 sfx_queue_tone_sweep ( sfx , 1319 , 1760 , 86 , 13200 , 18 ) ;
 sfx_queue_silence ( sfx , 10 ) ;
 sfx_queue_tone_sweep ( sfx , 1175 , 1568 , 84 , 12600 , 20 ) ;
 sfx_queue_silence ( sfx , 12 ) ;
 sfx_queue_tone_sweep ( sfx , 1568 , 1976 , 96 , 13800 , 22 ) ;
 }

 static void sfx_queue_death_jingle ( AmigaSfxState * sfx ) {
 sfx_queue_noise_burst ( sfx , 44 , 11000 ) ;
 sfx_queue_tone_sweep ( sfx , 360 , 220 , 74 , 13800 , 48 ) ;
 sfx_queue_silence ( sfx , 8 ) ;
 sfx_queue_tone_sweep ( sfx , 220 , 110 , 118 , 14600 , 54 ) ;
 }

 static void sfx_queue_game_over_jingle ( AmigaSfxState * sfx ) {
 sfx_queue_noise_burst ( sfx , 56 , 11400 ) ;
 sfx_queue_tone_sweep ( sfx , 300 , 230 , 76 , 13400 , 50 ) ;
 sfx_queue_silence ( sfx , 6 ) ;
 sfx_queue_tone_sweep ( sfx , 230 , 160 , 92 , 13800 , 52 ) ;
 sfx_queue_silence ( sfx , 8 ) ;
 sfx_queue_tone_sweep ( sfx , 160 , 90 , 140 , 14600 , 56 ) ;
 }

 static void sfx_queue_life_loss_fanfare ( AmigaSfxState * sfx ) {
 static const AmigaSfxPatternStep kLifeLost [ ] = {
 { 392 , 66 , 9000 , 10 } ,
 { 330 , 74 , 9400 , 10 } ,
 { 262 , 84 , 9900 , 14 } ,
 { 196 , 108 , 10600 , 0 }
 } ;
 sfx_queue_pattern ( sfx , kLifeLost , ( int ) ( sizeof ( kLifeLost ) / sizeof ( kLifeLost [ 0 ] ) ) ) ;
 sfx_queue_silence ( sfx , 8 ) ;
 sfx_queue_tone_sweep ( sfx , 196 , 146 , 82 , 11000 , 36 ) ;
 }

 static void sfx_queue_combo_jingle ( AmigaSfxState * sfx ) {
 static const AmigaSfxPatternStep kCombo [ ] = {
 { 620 , 28 , 8200 , 4 } ,
 { 860 , 32 , 9000 , 5 } ,
 { 1180 , 42 , 9800 , 0 }
 } ;
 sfx_queue_pattern ( sfx , kCombo , ( int ) ( sizeof ( kCombo ) / sizeof ( kCombo [ 0 ] ) ) ) ;
 sfx_queue_tone_sweep ( sfx , 1320 , 1620 , 38 , 10400 , 22 ) ;
 }

 static void sfx_queue_block_push_swish ( AmigaSfxState * sfx ) {
 sfx_queue_tone_sweep ( sfx , 1240 , 820 , 24 , 6800 , 24 ) ;
 sfx_queue_tone_sweep ( sfx , 820 , 560 , 20 , 6200 , 20 ) ;
 }

 static void sfx_queue_title_start_jingle ( AmigaSfxState * sfx ) {
 static const AmigaSfxPatternStep kTitleStart [ ] = {
 { 1047 , 54 , 6200 , 4 } ,
 { 1319 , 56 , 6700 , 4 } ,
 { 1568 , 62 , 7100 , 8 } ,
 { 2093 , 92 , 7800 , 12 } ,
 { 1760 , 52 , 6600 , 4 } ,
 { 2093 , 68 , 7600 , 8 } ,
 { 2637 , 118 , 8400 , 0 }
 } ;
 sfx_interrupt ( sfx ) ;
 sfx_queue_pattern ( sfx , kTitleStart , ( int ) ( sizeof ( kTitleStart ) / sizeof ( kTitleStart [ 0 ] ) ) ) ;
 }

 static void sfx_queue_title_confirm_chirp ( AmigaSfxState * sfx ) {
 static const AmigaSfxPatternStep kConfirm [ ] = {
 { 1568 , 24 , 5600 , 4 } ,
 { 2093 , 34 , 6400 , 0 }
 } ;
 sfx_queue_pattern ( sfx , kConfirm , ( int ) ( sizeof ( kConfirm ) / sizeof ( kConfirm [ 0 ] ) ) ) ;
 }

 static void sfx_dispatch_events ( AmigaSfxState * sfx , uint32_t event_flags ) {
 if ( ! sfx || ! sfx -> enabled || event_flags == 0 ) {
 return ;
 }

 if ( ( event_flags & GAME_EVENT_GAME_OVER ) != 0u ) {
 sfx_interrupt ( sfx ) ;
 sfx_queue_game_over_jingle ( sfx ) ;
 return ;
 }
 if ( ( event_flags & GAME_EVENT_LIFE_LOST ) != 0u ) {
 sfx_interrupt ( sfx ) ;
 sfx_queue_life_loss_fanfare ( sfx ) ;
 event_flags &= ( uint32_t ) ~ ( GAME_EVENT_COMBO | GAME_EVENT_CRUSH | GAME_EVENT_ITEM_COLLECT | GAME_EVENT_BLOCK_IMPACT ) ;
 }
 if ( ( event_flags & GAME_EVENT_PLAYER_DIED ) != 0u ) {
 sfx_interrupt ( sfx ) ;
 sfx_queue_death_jingle ( sfx ) ;
 event_flags &= ( uint32_t ) ~ ( GAME_EVENT_COMBO | GAME_EVENT_CRUSH | GAME_EVENT_ITEM_COLLECT | GAME_EVENT_BLOCK_IMPACT ) ;
 }
 if ( ( event_flags & GAME_EVENT_ROUND_CLEAR ) != 0u ) {
 sfx_interrupt ( sfx ) ;
 sfx_queue_level_up_fanfare ( sfx ) ;
 event_flags &= ( uint32_t ) ~ ( GAME_EVENT_COMBO | GAME_EVENT_CRUSH | GAME_EVENT_ITEM_COLLECT | GAME_EVENT_BLOCK_IMPACT ) ;
 }
 if ( ( event_flags & GAME_EVENT_SPECIAL_ALIGNMENT ) != 0u ) {
 sfx_queue_reward_jingle ( sfx ) ;
 }
 if ( ( event_flags & GAME_EVENT_COMBO ) != 0u ) {
 sfx_queue_combo_jingle ( sfx ) ;
 event_flags &= ( uint32_t ) ~ GAME_EVENT_CRUSH ;
 }
 if ( ( event_flags & GAME_EVENT_CRUSH ) != 0u ) {
 sfx_queue_noise_burst ( sfx , 24 , 7000 ) ;
 sfx_queue_tone_sweep ( sfx , 300 , 230 , 36 , 13200 , 56 ) ;
 sfx_queue_tone_sweep ( sfx , 420 , 560 , 28 , 11200 , 30 ) ;
 }
 if ( ( event_flags & GAME_EVENT_ITEM_COLLECT ) != 0u ) {
 sfx_queue_slot_chime ( sfx ) ;
 }
 if ( ( event_flags & GAME_EVENT_TIMER_DANGER_PULSE ) != 0u ) {
 sfx_queue_tone_sweep ( sfx , 980 , 900 , 34 , 9000 , 44 ) ;
 sfx_queue_silence ( sfx , 8 ) ;
 sfx_queue_tone_sweep ( sfx , 880 , 760 , 42 , 8600 , 44 ) ;
 }
 if ( ( event_flags & GAME_EVENT_ROUND_START ) != 0u ) {
 sfx_queue_tone_sweep ( sfx , 420 , 560 , 34 , 10200 , 34 ) ;
 sfx_queue_silence ( sfx , 8 ) ;
 sfx_queue_tone_sweep ( sfx , 620 , 860 , 46 , 11200 , 30 ) ;
 }
 if ( ( event_flags & GAME_EVENT_BLOCK_PUSH ) != 0u ) {
 sfx_queue_block_push_swish ( sfx ) ;
 }
 if ( ( event_flags & GAME_EVENT_MINE_PLACED ) != 0u ) {
 sfx_queue_tone_sweep ( sfx , 520 , 720 , 32 , 9800 , 30 ) ;
 event_flags &= ( uint32_t ) ~ GAME_EVENT_BLOCK_IMPACT ;
 }
 if ( ( event_flags & GAME_EVENT_MINE_EXPLODED ) != 0u ) {
 sfx_queue_noise_burst ( sfx , 56 , 14400 ) ;
 sfx_queue_tone_sweep ( sfx , 280 , 150 , 96 , 15600 , 58 ) ;
 sfx_queue_tone_sweep ( sfx , 190 , 90 , 126 , 14800 , 62 ) ;
 event_flags &= ( uint32_t ) ~ GAME_EVENT_BLOCK_IMPACT ;
 }
 if ( ( event_flags & GAME_EVENT_BLOCK_IMPACT ) != 0u ) {
 sfx_queue_noise_burst ( sfx , 18 , 4600 ) ;
 sfx_queue_tone_sweep ( sfx , 240 , 170 , 26 , 8600 , 56 ) ;
 }
 if ( ( event_flags & GAME_EVENT_META_BANKED ) != 0u ) {
 sfx_queue_reward_jingle ( sfx ) ;
 }
 if ( ( event_flags & GAME_EVENT_META_SPENT ) != 0u ) {
 sfx_queue_tone_sweep ( sfx , 760 , 920 , 20 , 9800 , 30 ) ;
 sfx_queue_silence ( sfx , 4 ) ;
 sfx_queue_tone_sweep ( sfx , 940 , 1220 , 30 , 10800 , 26 ) ;
 }
 if ( ( event_flags & GAME_EVENT_STAGE_MODIFIER ) != 0u ) {
 sfx_queue_tone_sweep ( sfx , 540 , 500 , 22 , 8600 , 40 ) ;
 sfx_queue_silence ( sfx , 5 ) ;
 sfx_queue_tone_sweep ( sfx , 660 , 620 , 24 , 9000 , 40 ) ;
 sfx_queue_silence ( sfx , 6 ) ;
 sfx_queue_tone_sweep ( sfx , 760 , 900 , 32 , 9600 , 36 ) ;
 }
 }

 static void sfx_start_voice_step ( AmigaSfxState * sfx , AmigaSfxVoice * voice ) {
 UBYTE channel ;
 UBYTE * sample ;
 if ( ! sfx || ! voice || voice -> count == 0 ) {
 if ( voice ) {
 sfx_disable_voice ( voice -> channel ) ;
 }
 return ;
 }

 voice -> current = voice -> steps [ voice -> head ] ;
 ++ voice -> head ;
 if ( voice -> head >= 32 ) {
 voice -> head = 0 ;
 }
 -- voice -> count ;
 voice -> elapsed = 0 ;
 voice -> period_current = ( WORD ) voice -> current . period_start ;
 voice -> period_step = 0 ;
 voice -> period_adjust = 0 ;
 voice -> period_den = 1 ;
 voice -> period_remainder = 0 ;
 voice -> period_error = 0 ;
 if ( voice -> current . ticks > 1 ) {
 WORD delta = ( WORD ) ( voice -> current . period_end - voice -> current . period_start ) ;
 UWORD mag ;
 voice -> period_den = ( UWORD ) ( voice -> current . ticks - 1 ) ;
 voice -> period_step = ( WORD ) ( delta / ( WORD ) voice -> period_den ) ;
 if ( delta < 0 ) {
 mag = ( UWORD ) ( - delta ) ;
 voice -> period_adjust = - 1 ;
 } else {
 mag = ( UWORD ) delta ;
 voice -> period_adjust = 1 ;
 }
 voice -> period_remainder = ( UWORD ) ( mag % voice -> period_den ) ;
 }
 voice -> active = 1 ;
 channel = voice -> channel ;

 if ( voice -> current . wave == AMIGA_SFX_WAVE_SILENCE || voice -> current . volume == 0 ) {
 sfx_disable_voice ( channel ) ;
 return ;
 }

 sample = sfx_wave_ptr ( sfx , voice -> current . wave ) ;
 if ( ! sample ) {
 sfx_disable_voice ( channel ) ;
 voice -> active = 0 ;
 return ;
 }

 ( ( volatile struct Custom * ) 0xDFF000 ) -> dmacon = sfx_dma_bit ( channel ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> aud [ channel ] . ac_ptr = ( UWORD * ) sample ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> aud [ channel ] . ac_len = sfx_wave_len_words ( voice -> current . wave ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> aud [ channel ] . ac_per = voice -> current . period_start ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> aud [ channel ] . ac_vol = voice -> current . volume ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> dmacon = 0x8000 | sfx_dma_bit ( channel ) ;
 }

 static void sfx_update_voice ( AmigaSfxState * sfx , AmigaSfxVoice * voice ) {
 if ( ! voice -> active ) {
 sfx_start_voice_step ( sfx , voice ) ;
 }
 if ( ! voice -> active ) {
 return ;
 }

 if ( voice -> current . wave != AMIGA_SFX_WAVE_SILENCE && voice -> current . volume > 0 ) {
 ( ( volatile struct Custom * ) 0xDFF000 ) -> aud [ voice -> channel ] . ac_per = ( UWORD ) voice -> period_current ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> aud [ voice -> channel ] . ac_vol = voice -> current . volume ;
 if ( voice -> current . ticks > 1 ) {
 voice -> period_current = ( WORD ) ( voice -> period_current + voice -> period_step ) ;
 voice -> period_error = ( UWORD ) ( voice -> period_error + voice -> period_remainder ) ;
 if ( voice -> period_error >= voice -> period_den ) {
 voice -> period_error = ( UWORD ) ( voice -> period_error - voice -> period_den ) ;
 voice -> period_current = ( WORD ) ( voice -> period_current + voice -> period_adjust ) ;
 }
 }
 }

 ++ voice -> elapsed ;
 if ( voice -> elapsed >= voice -> current . ticks ) {
 voice -> active = 0 ;
 if ( voice -> count == 0 ) {
 sfx_disable_voice ( voice -> channel ) ;
 } else {
 sfx_start_voice_step ( sfx , voice ) ;
 }
 }
 }

 static void sfx_update ( AmigaSfxState * sfx ) {
 if ( ! sfx || ! sfx -> enabled ) {
 return ;
 }
 sfx_update_voice ( sfx , & sfx -> voice [ 0 ] ) ;
 sfx_update_voice ( sfx , & sfx -> voice [ 1 ] ) ;
 }

 static void sfx_build_samples ( UBYTE * samples ) {
 static const UBYTE tone [ 8 ] = {
 0x58 , 0x68 , 0x68 , 0x58 , 0xA8 , 0x98 , 0x98 , 0xA8
 } ;
 uint32_t noise = 0x6d2b79f5u ;
 int i ;
 if ( ! samples ) {
 return ;
 }
 __asm_memcpy ( samples + 0 , tone , 8 ) ;
 for ( i = 0 ; i < 64 ; ++ i ) {
 noise = noise * 1664525u + 1013904223u ;
 samples [ 8 + i ] = ( UBYTE ) ( noise >> 24 ) ;
 }
 }

 static BOOL sfx_init ( AmigaSfxState * sfx ) {
 __asm_memset ( sfx , 0 , sizeof ( * sfx ) ) ;



 sfx -> samples = ( UBYTE * ) __AllocMem ( SysBase , ( ( 8 + 64 ) ) , ( 0x00000002UL | 0x00010000UL ) ) ;
 if ( ! sfx -> samples ) {
 return 0 ;
 }
 sfx_build_samples ( sfx -> samples ) ;
 sfx -> voice [ 0 ] . channel = 0 ;
 sfx -> voice [ 1 ] . channel = 1 ;
 sfx -> enabled = 1 ;
 sfx_disable_voice ( 0 ) ;
 sfx_disable_voice ( 1 ) ;
 return 1 ;

 }

 static void sfx_shutdown ( AmigaSfxState * sfx ) {
 if ( ! sfx ) {
 return ;
 }
 sfx_disable_voice ( 0 ) ;
 sfx_disable_voice ( 1 ) ;
 if ( sfx -> samples ) {
 __FreeMem ( SysBase , ( sfx -> samples ) , ( ( 8 + 64 ) ) ) ;
 }
 __asm_memset ( sfx , 0 , sizeof ( * sfx ) ) ;
 }

 static inline void put_pixel_unchecked ( UBYTE * screen , int x , int y , UBYTE color ) {
 UBYTE mask ;
 UBYTE inv_mask ;
 UBYTE * p ;
 ULONG base ;
 mask = ( UBYTE ) ( 0x80u >> ( x & 7 ) ) ;
 inv_mask = ( UBYTE ) ~ mask ;
 base = ( ( ULONG ) y * ( ( 320 / 8 ) * 5 ) ) + ( ULONG ) ( x >> 3 ) ;
 p = screen + base ;
 if ( color & 1u ) p [ 0 ] = ( UBYTE ) ( p [ 0 ] | mask ) ; else p [ 0 ] = ( UBYTE ) ( p [ 0 ] & inv_mask ) ;
 if ( color & 2u ) p [ ( 320 / 8 ) ] = ( UBYTE ) ( p [ ( 320 / 8 ) ] | mask ) ; else p [ ( 320 / 8 ) ] = ( UBYTE ) ( p [ ( 320 / 8 ) ] & inv_mask ) ;
 if ( color & 4u ) p [ ( 320 / 8 ) * 2 ] = ( UBYTE ) ( p [ ( 320 / 8 ) * 2 ] | mask ) ; else p [ ( 320 / 8 ) * 2 ] = ( UBYTE ) ( p [ ( 320 / 8 ) * 2 ] & inv_mask ) ;
 if ( color & 8u ) p [ ( 320 / 8 ) * 3 ] = ( UBYTE ) ( p [ ( 320 / 8 ) * 3 ] | mask ) ; else p [ ( 320 / 8 ) * 3 ] = ( UBYTE ) ( p [ ( 320 / 8 ) * 3 ] & inv_mask ) ;
 if ( color & 16u ) p [ ( 320 / 8 ) * 4 ] = ( UBYTE ) ( p [ ( 320 / 8 ) * 4 ] | mask ) ; else p [ ( 320 / 8 ) * 4 ] = ( UBYTE ) ( p [ ( 320 / 8 ) * 4 ] & inv_mask ) ;
 }

 static inline void put_pixel ( UBYTE * screen , int x , int y , UBYTE color ) {
 if ( x < 0 || y < 0 || x >= 320 || y >= 200 ) {
 return ;
 }
 put_pixel_unchecked ( screen , x , y , color ) ;
 }

 static void draw_hline ( UBYTE * screen , int x0 , int x1 , int y , UBYTE color ) {
 int plane ;
 int start_byte ;
 int end_byte ;
 UBYTE left_mask ;
 UBYTE right_mask ;
 UBYTE mask ;
 if ( y < 0 || y >= 200 ) {
 return ;
 }
 x0 = clamp_int ( x0 , 0 , 320 - 1 ) ;
 x1 = clamp_int ( x1 , 0 , 320 - 1 ) ;
 if ( x1 < x0 ) {
 return ;
 }
 start_byte = x0 >> 3 ;
 end_byte = x1 >> 3 ;
 left_mask = ( UBYTE ) ( 0xFFu >> ( x0 & 7 ) ) ;
 right_mask = ( UBYTE ) ( 0xFFu << ( 7 - ( x1 & 7 ) ) ) ;
 for ( plane = 0 ; plane < 5 ; ++ plane ) {
 UBYTE * row = screen + ( ( ULONG ) y * ( ( 320 / 8 ) * 5 ) ) + ( ( ULONG ) plane * ( 320 / 8 ) ) ;
 UBYTE fill = ( ( color >> plane ) & 1u ) ? 0xFFu : 0x00u ;
 if ( start_byte == end_byte ) {
 mask = ( UBYTE ) ( left_mask & right_mask ) ;
 if ( fill ) {
 row [ start_byte ] = ( UBYTE ) ( row [ start_byte ] | mask ) ;
 } else {
 row [ start_byte ] = ( UBYTE ) ( row [ start_byte ] & ( UBYTE ) ~ mask ) ;
 }
 continue ;
 }
 if ( fill ) {
 row [ start_byte ] = ( UBYTE ) ( row [ start_byte ] | left_mask ) ;
 row [ end_byte ] = ( UBYTE ) ( row [ end_byte ] | right_mask ) ;
 } else {
 row [ start_byte ] = ( UBYTE ) ( row [ start_byte ] & ( UBYTE ) ~ left_mask ) ;
 row [ end_byte ] = ( UBYTE ) ( row [ end_byte ] & ( UBYTE ) ~ right_mask ) ;
 }
 if ( end_byte > start_byte + 1 ) {
 __asm_memset ( row + start_byte + 1 , fill , ( size_t ) ( end_byte - start_byte - 1 ) ) ;
 }
 }
 }

 static void draw_rect ( UBYTE * screen , int x0 , int y0 , int x1 , int y1 , UBYTE color ) {
 int y ;
 x0 = clamp_int ( x0 , 0 , 320 - 1 ) ;
 y0 = clamp_int ( y0 , 0 , 200 - 1 ) ;
 x1 = clamp_int ( x1 , 0 , 320 - 1 ) ;
 y1 = clamp_int ( y1 , 0 , 200 - 1 ) ;
 if ( x1 < x0 || y1 < y0 ) {
 return ;
 }
 for ( y = y0 ; y <= y1 ; ++ y ) {
 draw_hline ( screen , x0 , x1 , y , color ) ;
 }
 }

 static void draw_digit ( UBYTE * screen , int x , int y , int digit , UBYTE color ) {
 int row ;
 int col ;
 if ( digit < 0 || digit > 9 ) {
 return ;
 }
 for ( row = 0 ; row < 5 ; ++ row ) {
 UBYTE bits = kDigit3x5 [ digit ] [ row ] ;
 for ( col = 0 ; col < 3 ; ++ col ) {
 if ( ( bits >> ( 2 - col ) ) & 1u ) {
 put_pixel ( screen , x + col , y + row , color ) ;
 }
 }
 }
 }

 static bool letter_rows ( char ch , UBYTE out [ 5 ] ) {
 switch ( ch ) {
 case 'A' : out [ 0 ] = 7 ; out [ 1 ] = 5 ; out [ 2 ] = 7 ; out [ 3 ] = 5 ; out [ 4 ] = 5 ; return 1 ;
 case 'B' : out [ 0 ] = 6 ; out [ 1 ] = 5 ; out [ 2 ] = 6 ; out [ 3 ] = 5 ; out [ 4 ] = 6 ; return 1 ;
 case 'C' : out [ 0 ] = 7 ; out [ 1 ] = 4 ; out [ 2 ] = 4 ; out [ 3 ] = 4 ; out [ 4 ] = 7 ; return 1 ;
 case 'D' : out [ 0 ] = 6 ; out [ 1 ] = 5 ; out [ 2 ] = 5 ; out [ 3 ] = 5 ; out [ 4 ] = 6 ; return 1 ;
 case 'E' : out [ 0 ] = 7 ; out [ 1 ] = 4 ; out [ 2 ] = 6 ; out [ 3 ] = 4 ; out [ 4 ] = 7 ; return 1 ;
 case 'F' : out [ 0 ] = 7 ; out [ 1 ] = 4 ; out [ 2 ] = 6 ; out [ 3 ] = 4 ; out [ 4 ] = 4 ; return 1 ;
 case 'G' : out [ 0 ] = 7 ; out [ 1 ] = 4 ; out [ 2 ] = 5 ; out [ 3 ] = 5 ; out [ 4 ] = 7 ; return 1 ;
 case 'H' : out [ 0 ] = 5 ; out [ 1 ] = 5 ; out [ 2 ] = 7 ; out [ 3 ] = 5 ; out [ 4 ] = 5 ; return 1 ;
 case 'I' : out [ 0 ] = 7 ; out [ 1 ] = 2 ; out [ 2 ] = 2 ; out [ 3 ] = 2 ; out [ 4 ] = 7 ; return 1 ;
 case 'J' : out [ 0 ] = 1 ; out [ 1 ] = 1 ; out [ 2 ] = 1 ; out [ 3 ] = 5 ; out [ 4 ] = 7 ; return 1 ;
 case 'K' : out [ 0 ] = 5 ; out [ 1 ] = 5 ; out [ 2 ] = 6 ; out [ 3 ] = 5 ; out [ 4 ] = 5 ; return 1 ;
 case 'L' : out [ 0 ] = 4 ; out [ 1 ] = 4 ; out [ 2 ] = 4 ; out [ 3 ] = 4 ; out [ 4 ] = 7 ; return 1 ;
 case 'M' : out [ 0 ] = 5 ; out [ 1 ] = 7 ; out [ 2 ] = 5 ; out [ 3 ] = 5 ; out [ 4 ] = 5 ; return 1 ;
 case 'N' : out [ 0 ] = 5 ; out [ 1 ] = 7 ; out [ 2 ] = 7 ; out [ 3 ] = 7 ; out [ 4 ] = 5 ; return 1 ;
 case 'O' : out [ 0 ] = 7 ; out [ 1 ] = 5 ; out [ 2 ] = 5 ; out [ 3 ] = 5 ; out [ 4 ] = 7 ; return 1 ;
 case 'P' : out [ 0 ] = 6 ; out [ 1 ] = 5 ; out [ 2 ] = 6 ; out [ 3 ] = 4 ; out [ 4 ] = 4 ; return 1 ;
 case 'Q' : out [ 0 ] = 7 ; out [ 1 ] = 5 ; out [ 2 ] = 5 ; out [ 3 ] = 7 ; out [ 4 ] = 1 ; return 1 ;
 case 'R' : out [ 0 ] = 6 ; out [ 1 ] = 5 ; out [ 2 ] = 6 ; out [ 3 ] = 5 ; out [ 4 ] = 5 ; return 1 ;
 case 'S' : out [ 0 ] = 7 ; out [ 1 ] = 4 ; out [ 2 ] = 7 ; out [ 3 ] = 1 ; out [ 4 ] = 7 ; return 1 ;
 case 'T' : out [ 0 ] = 7 ; out [ 1 ] = 2 ; out [ 2 ] = 2 ; out [ 3 ] = 2 ; out [ 4 ] = 2 ; return 1 ;
 case 'U' : out [ 0 ] = 5 ; out [ 1 ] = 5 ; out [ 2 ] = 5 ; out [ 3 ] = 5 ; out [ 4 ] = 7 ; return 1 ;
 case 'V' : out [ 0 ] = 5 ; out [ 1 ] = 5 ; out [ 2 ] = 5 ; out [ 3 ] = 5 ; out [ 4 ] = 2 ; return 1 ;
 case 'W' : out [ 0 ] = 5 ; out [ 1 ] = 5 ; out [ 2 ] = 5 ; out [ 3 ] = 7 ; out [ 4 ] = 5 ; return 1 ;
 case 'X' : out [ 0 ] = 5 ; out [ 1 ] = 5 ; out [ 2 ] = 2 ; out [ 3 ] = 5 ; out [ 4 ] = 5 ; return 1 ;
 case 'Y' : out [ 0 ] = 5 ; out [ 1 ] = 5 ; out [ 2 ] = 2 ; out [ 3 ] = 2 ; out [ 4 ] = 2 ; return 1 ;
 case 'Z' : out [ 0 ] = 7 ; out [ 1 ] = 1 ; out [ 2 ] = 2 ; out [ 3 ] = 4 ; out [ 4 ] = 7 ; return 1 ;
 case ':' : out [ 0 ] = 0 ; out [ 1 ] = 2 ; out [ 2 ] = 0 ; out [ 3 ] = 2 ; out [ 4 ] = 0 ; return 1 ;
 case '-' : out [ 0 ] = 0 ; out [ 1 ] = 0 ; out [ 2 ] = 7 ; out [ 3 ] = 0 ; out [ 4 ] = 0 ; return 1 ;
 case '+' : out [ 0 ] = 0 ; out [ 1 ] = 2 ; out [ 2 ] = 7 ; out [ 3 ] = 2 ; out [ 4 ] = 0 ; return 1 ;
 case '/' : out [ 0 ] = 1 ; out [ 1 ] = 1 ; out [ 2 ] = 2 ; out [ 3 ] = 4 ; out [ 4 ] = 4 ; return 1 ;
 case '.' : out [ 0 ] = 0 ; out [ 1 ] = 0 ; out [ 2 ] = 0 ; out [ 3 ] = 0 ; out [ 4 ] = 2 ; return 1 ;
 case '<' : out [ 0 ] = 1 ; out [ 1 ] = 2 ; out [ 2 ] = 4 ; out [ 3 ] = 2 ; out [ 4 ] = 1 ; return 1 ;
 case '>' : out [ 0 ] = 4 ; out [ 1 ] = 2 ; out [ 2 ] = 1 ; out [ 3 ] = 2 ; out [ 4 ] = 4 ; return 1 ;
 default : return 0 ;
 }
 }

 static void draw_char ( UBYTE * screen , int x , int y , char ch , UBYTE color ) {
 UBYTE rows [ 5 ] ;
 int row ;
 int col ;
 if ( ch >= '0' && ch <= '9' ) {
 draw_digit ( screen , x , y , ch - '0' , color ) ;
 return ;
 }
 if ( ch >= 'a' && ch <= 'z' ) {
 ch = ( char ) ( ch - 'a' + 'A' ) ;
 }
 if ( ch == ' ' ) {
 return ;
 }
 if ( ! letter_rows ( ch , rows ) ) {
 return ;
 }
 for ( row = 0 ; row < 5 ; ++ row ) {
 for ( col = 0 ; col < 3 ; ++ col ) {
 if ( ( rows [ row ] >> ( 2 - col ) ) & 1u ) {
 put_pixel ( screen , x + col , y + row , color ) ;
 }
 }
 }
 }

 static void draw_text ( UBYTE * screen , int x , int y , const char * text , UBYTE color ) {
 while ( * text != '\0' ) {
 draw_char ( screen , x , y , * text , color ) ;
 x += 4 ;
 ++ text ;
 }
 }

 static int text_width ( const char * text ) {
 int len = 0 ;
 while ( text [ len ] != '\0' ) {
 ++ len ;
 }
 return ( len > 0 ) ? ( ( len * 4 ) - 1 ) : 0 ;
 }

 static void draw_centered_text ( UBYTE * screen , int x0 , int x1 , int y , const char * text , UBYTE color ) {
 int w = text_width ( text ) ;
 int x = x0 + ( ( ( x1 - x0 + 1 ) - w ) / 2 ) ;
 draw_text ( screen , x , y , text , color ) ;
 }

 static void draw_text_outlined ( UBYTE * screen , int x , int y , const char * text , UBYTE color , UBYTE outline ) {
 draw_text ( screen , x - 1 , y , text , outline ) ;
 draw_text ( screen , x + 1 , y , text , outline ) ;
 draw_text ( screen , x , y - 1 , text , outline ) ;
 draw_text ( screen , x , y + 1 , text , outline ) ;
 draw_text ( screen , x , y , text , color ) ;
 }

 static void draw_centered_text_outlined ( UBYTE * screen , int x0 , int x1 , int y , const char * text , UBYTE color , UBYTE outline ) {
 int w = text_width ( text ) ;
 int x = x0 + ( ( ( x1 - x0 + 1 ) - w ) / 2 ) ;
 draw_text_outlined ( screen , x , y , text , color , outline ) ;
 }

 static int text_width_scaled ( const char * text , int scale ) {
 int len = 0 ;
 scale = clamp_int ( scale , 1 , 4 ) ;
 while ( text [ len ] != '\0' ) {
 ++ len ;
 }
 return ( len > 0 ) ? ( ( len * 4 * scale ) - scale ) : 0 ;
 }

 static void draw_char_scaled ( UBYTE * screen , int x , int y , char ch , UBYTE color , int scale ) {
 UBYTE rows [ 5 ] ;
 int row ;
 int col ;
 int sx ;
 int sy ;
 scale = clamp_int ( scale , 1 , 4 ) ;
 if ( ch >= '0' && ch <= '9' ) {
 __asm_memcpy ( rows , kDigit3x5 [ ch - '0' ] , sizeof ( rows ) ) ;
 } else {
 if ( ch >= 'a' && ch <= 'z' ) {
 ch = ( char ) ( ch - 'a' + 'A' ) ;
 }
 if ( ch == ' ' ) {
 return ;
 }
 if ( ! letter_rows ( ch , rows ) ) {
 return ;
 }
 }
 for ( row = 0 ; row < 5 ; ++ row ) {
 for ( col = 0 ; col < 3 ; ++ col ) {
 if ( ( ( rows [ row ] >> ( 2 - col ) ) & 1u ) == 0 ) {
 continue ;
 }
 for ( sy = 0 ; sy < scale ; ++ sy ) {
 for ( sx = 0 ; sx < scale ; ++ sx ) {
 put_pixel ( screen , x + col * scale + sx , y + row * scale + sy , color ) ;
 }
 }
 }
 }
 }

 static void draw_text_scaled ( UBYTE * screen , int x , int y , const char * text , UBYTE color , int scale ) {
 scale = clamp_int ( scale , 1 , 4 ) ;
 while ( * text != '\0' ) {
 draw_char_scaled ( screen , x , y , * text , color , scale ) ;
 x += 4 * scale ;
 ++ text ;
 }
 }

 static void draw_text_scaled_outlined (
 UBYTE * screen ,
 int x ,
 int y ,
 const char * text ,
 UBYTE color ,
 UBYTE outline ,
 int scale ) {
 draw_text_scaled ( screen , x - 1 , y , text , outline , scale ) ;
 draw_text_scaled ( screen , x + 1 , y , text , outline , scale ) ;
 draw_text_scaled ( screen , x , y - 1 , text , outline , scale ) ;
 draw_text_scaled ( screen , x , y + 1 , text , outline , scale ) ;
 draw_text_scaled ( screen , x , y , text , color , scale ) ;
 }

 static void draw_centered_text_scaled_outlined (
 UBYTE * screen ,
 int x0 ,
 int x1 ,
 int y ,
 const char * text ,
 UBYTE color ,
 UBYTE outline ,
 int scale ) {
 int w = text_width_scaled ( text , scale ) ;
 int x = x0 + ( ( ( x1 - x0 + 1 ) - w ) / 2 ) ;
 draw_text_scaled_outlined ( screen , x , y , text , color , outline , scale ) ;
 }

 static void draw_number ( UBYTE * screen , int x , int y , uint32_t value , int min_digits , UBYTE color ) {
 int digits [ 10 ] ;
 int count = 0 ;
 int i ;
 do {
 digits [ count ++ ] = ( int ) ( value % 10u ) ;
 value /= 10u ;
 } while ( value > 0u && count < 10 ) ;
 while ( count < min_digits && count < 10 ) {
 digits [ count ++ ] = 0 ;
 }
 for ( i = count - 1 ; i >= 0 ; -- i ) {
 draw_digit ( screen , x , y , digits [ i ] , color ) ;
 x += 4 ;
 }
 }

 static void draw_number_outlined ( UBYTE * screen , int x , int y , uint32_t value , int min_digits , UBYTE color , UBYTE outline ) {
 draw_number ( screen , x - 1 , y , value , min_digits , outline ) ;
 draw_number ( screen , x + 1 , y , value , min_digits , outline ) ;
 draw_number ( screen , x , y - 1 , value , min_digits , outline ) ;
 draw_number ( screen , x , y + 1 , value , min_digits , outline ) ;
 draw_number ( screen , x , y , value , min_digits , color ) ;
 }

 static int number_width ( uint32_t value , int min_digits ) {
 int count = 0 ;
 do {
 ++ count ;
 value /= 10u ;
 } while ( value > 0u && count < 10 ) ;
 while ( count < min_digits && count < 10 ) {
 ++ count ;
 }
 return ( count > 0 ) ? ( ( count * 4 ) - 1 ) : 0 ;
 }

 static void draw_number_right ( UBYTE * screen , int right_x , int y , uint32_t value , int min_digits , UBYTE color ) {
 draw_number ( screen , right_x - number_width ( value , min_digits ) + 1 , y , value , min_digits , color ) ;
 }

 static void draw_digit_scaled ( UBYTE * screen , int x , int y , int digit , UBYTE color , int scale ) {
 int row ;
 int col ;
 int sx ;
 int sy ;
 if ( digit < 0 || digit > 9 ) {
 return ;
 }
 scale = clamp_int ( scale , 1 , 6 ) ;
 for ( row = 0 ; row < 5 ; ++ row ) {
 UBYTE bits = kDigit3x5 [ digit ] [ row ] ;
 for ( col = 0 ; col < 3 ; ++ col ) {
 if ( ( ( bits >> ( 2 - col ) ) & 1u ) == 0 ) {
 continue ;
 }
 for ( sy = 0 ; sy < scale ; ++ sy ) {
 for ( sx = 0 ; sx < scale ; ++ sx ) {
 put_pixel ( screen , x + col * scale + sx , y + row * scale + sy , color ) ;
 }
 }
 }
 }
 }

 static int number_width_scaled ( uint32_t value , int min_digits , int scale ) {
 int digits = 1 ;
 scale = clamp_int ( scale , 1 , 6 ) ;
 while ( value >= 10u && digits < 10 ) {
 value /= 10u ;
 ++ digits ;
 }
 if ( digits < min_digits ) {
 digits = min_digits ;
 }
 return ( digits * ( 4 * scale ) ) - scale ;
 }

 static void draw_number_scaled ( UBYTE * screen , int x , int y , uint32_t value , int min_digits , UBYTE color , int scale ) {
 int digits [ 10 ] ;
 int count = 0 ;
 int i ;
 scale = clamp_int ( scale , 1 , 6 ) ;
 do {
 digits [ count ++ ] = ( int ) ( value % 10u ) ;
 value /= 10u ;
 } while ( value > 0u && count < 10 ) ;
 while ( count < min_digits && count < 10 ) {
 digits [ count ++ ] = 0 ;
 }
 for ( i = count - 1 ; i >= 0 ; -- i ) {
 draw_digit_scaled ( screen , x , y , digits [ i ] , color , scale ) ;
 x += 4 * scale ;
 }
 }

 static void draw_number_scaled_outlined ( UBYTE * screen , int x , int y , uint32_t value , int min_digits , UBYTE color , UBYTE outline , int scale ) {
 draw_number_scaled ( screen , x - 1 , y , value , min_digits , outline , scale ) ;
 draw_number_scaled ( screen , x + 1 , y , value , min_digits , outline , scale ) ;
 draw_number_scaled ( screen , x , y - 1 , value , min_digits , outline , scale ) ;
 draw_number_scaled ( screen , x , y + 1 , value , min_digits , outline , scale ) ;
 draw_number_scaled ( screen , x , y , value , min_digits , color , scale ) ;
 }

 static void mark_all_dirty ( RendererState * rr , int buf ) {
 int x ;
 int y ;
 for ( y = 0 ; y < 12 ; ++ y ) {
 for ( x = 0 ; x < 20 ; ++ x ) {
 rr -> dirty [ buf ] [ y ] [ x ] = 1 ;
 }
 }
 }

 static void mark_rect_dirty ( RendererState * rr , int buf , int x , int y , int w , int h ) {
 int x0 ;
 int y0 ;
 int x1 ;
 int y1 ;
 int cx ;
 int cy ;
 if ( w <= 0 || h <= 0 ) {
 return ;
 }
 if ( x >= 320 || y >= ( 12 * 16 ) || x + w <= 0 || y + h <= 0 ) {
 return ;
 }
 x0 = clamp_int ( x , 0 , 320 - 1 ) / 16 ;
 y0 = clamp_int ( y , 0 , ( 12 * 16 ) - 1 ) / 16 ;
 x1 = clamp_int ( x + w - 1 , 0 , 320 - 1 ) / 16 ;
 y1 = clamp_int ( y + h - 1 , 0 , ( 12 * 16 ) - 1 ) / 16 ;
 for ( cy = y0 ; cy <= y1 ; ++ cy ) {
 for ( cx = x0 ; cx <= x1 ; ++ cx ) {
 rr -> dirty [ buf ] [ cy ] [ cx ] = 1 ;
 }
 }
 }

 static void mark_gameplay_border_dirty ( RendererState * rr , int buf ) {
 mark_rect_dirty ( rr , buf , 0 , 0 , 320 , 4 ) ;
 mark_rect_dirty ( rr , buf , 0 , ( 12 * 16 ) - 4 , 320 , 4 ) ;
 mark_rect_dirty ( rr , buf , 0 , 0 , 4 , ( 12 * 16 ) ) ;
 mark_rect_dirty ( rr , buf , 320 - 4 , 0 , 4 , ( 12 * 16 ) ) ;
 }

 static void mark_dirty_rect ( RendererState * rr , int buf , const DirtyRect * r ) {
 if ( r && r -> w > 0 && r -> h > 0 ) {
 mark_rect_dirty ( rr , buf , r -> x , r -> y , r -> w , r -> h ) ;
 }
 }

 static inline bool dirty_rects_same ( const DirtyRect * a , const DirtyRect * b ) {
 return a -> x == b -> x &&
 a -> y == b -> y &&
 a -> w == b -> w &&
 a -> h == b -> h ;
 }

 static bool dirty_rect_overlaps_pending_cells ( const RendererState * rr , int buf , const DirtyRect * r ) {
 int x0 ;
 int y0 ;
 int x1 ;
 int y1 ;
 int x ;
 int y ;

 if ( ! r || r -> w <= 0 || r -> h <= 0 ) {
 return 0 ;
 }

 x0 = clamp_int ( r -> x , 0 , 320 - 1 ) / 16 ;
 y0 = clamp_int ( r -> y , 0 , ( 12 * 16 ) - 1 ) / 16 ;
 x1 = clamp_int ( r -> x + r -> w - 1 , 0 , 320 - 1 ) / 16 ;
 y1 = clamp_int ( r -> y + r -> h - 1 , 0 , ( 12 * 16 ) - 1 ) / 16 ;
 for ( y = y0 ; y <= y1 ; ++ y ) {
 for ( x = x0 ; x <= x1 ; ++ x ) {
 if ( rr -> dirty [ buf ] [ y ] [ x ] ) {
 return 1 ;
 }
 }
 }
 return 0 ;
 }

 static void restore_dynamic_caches ( RendererState * rr , int buf ) {
 int i ;
 if ( rr -> cached_player [ buf ] . valid ) {
 mark_dirty_rect ( rr , buf , & rr -> cached_player [ buf ] . rect ) ;
 }
 rr -> cached_player [ buf ] . valid = 0 ;
 for ( i = 0 ; i < 8 ; ++ i ) {
 if ( rr -> cached_block [ buf ] [ i ] . valid ) {
 mark_dirty_rect ( rr , buf , & rr -> cached_block [ buf ] [ i ] . rect ) ;
 }
 rr -> cached_block [ buf ] [ i ] . valid = 0 ;
 }
 for ( i = 0 ; i < 16 ; ++ i ) {
 if ( rr -> cached_enemy [ buf ] [ i ] . valid ) {
 mark_dirty_rect ( rr , buf , & rr -> cached_enemy [ buf ] [ i ] . rect ) ;
 }
 rr -> cached_enemy [ buf ] [ i ] . valid = 0 ;
 }
 }

 static void prepare_dynamic_cache ( RendererState * rr , int buf , DynamicCache * cache , bool valid , ULONG key , const DirtyRect * rect ) {
 bool changed ;
 if ( ! cache -> valid ) {
 return ;
 }
 changed = ! valid || cache -> key != key || ! rect || ! dirty_rects_same ( & cache -> rect , rect ) ;
 if ( ! changed && dirty_rect_overlaps_pending_cells ( rr , buf , & cache -> rect ) ) {
 changed = 1 ;
 }
 if ( changed ) {
 mark_dirty_rect ( rr , buf , & cache -> rect ) ;
 cache -> valid = 0 ;
 }
 }

 static void save_dynamic_rect ( const AmigaApp * app , RendererState * rr , int buf , UBYTE * screen , int x , int y , int w , int h ) {
 WORD n ;
 DirtyRect rect ;
 if ( w <= 0 || h <= 0 || x >= 320 || y >= 200 || x + w <= 0 || y + h <= 0 ) {
 return ;
 }
 rect . x = ( WORD ) clamp_int ( x , 0 , 320 - 1 ) ;
 rect . y = ( WORD ) clamp_int ( y , 0 , 200 - 1 ) ;
 rect . w = ( WORD ) ( clamp_int ( x + w , 0 , 320 ) - rect . x ) ;
 rect . h = ( WORD ) ( clamp_int ( y + h , 0 , 200 ) - rect . y ) ;
 if ( rect . w <= 0 || rect . h <= 0 ) {
 return ;
 }
 n = rr -> prev_dynamic_count [ buf ] ;
 if ( n >= 64 ) {
 return ;
 }
 rr -> prev_dynamic [ buf ] [ n ] = rect ;
 rr -> prev_dynamic_count [ buf ] = ( WORD ) ( n + 1 ) ;
 }

 static void restore_previous_dynamic ( const AmigaApp * app , RendererState * rr , int buf , UBYTE * screen ) {
 WORD i ;
 for ( i = 0 ; i < rr -> prev_dynamic_count [ buf ] ; ++ i ) {
 mark_dirty_rect ( rr , buf , & rr -> prev_dynamic [ buf ] [ i ] ) ;
 }
 rr -> prev_dynamic_count [ buf ] = 0 ;
 }

 static inline ULONG dynamic_bob_key ( AmigaSpriteId sprite , int px , int py , ULONG extra ) {
 return ( ( ULONG ) ( px & 0x01FF ) ) |
 ( ( ( ULONG ) ( py & 0x00FF ) ) << 9 ) |
 ( ( ( ULONG ) sprite & 0x003F ) << 17 ) |
 ( ( extra & 0x01FFUL ) << 23 ) ;
 }

 static void make_bob_dirty_rect ( int px , int py , DirtyRect * rect ) {
 int x0 ;
 int x1 ;
 x0 = ( px & ( 16 - 1 ) ) == 0 ? px : ( px & ~ ( 16 - 1 ) ) ;
 x1 = x0 + ( ( ( px & ( 16 - 1 ) ) == 0 ) ? 16 : ( 16 * 2 ) ) ;
 rect -> x = ( WORD ) clamp_int ( x0 , 0 , 320 - 1 ) ;
 rect -> y = ( WORD ) clamp_int ( py , 0 , 200 - 1 ) ;
 rect -> w = ( WORD ) ( clamp_int ( x1 , 0 , 320 ) - rect -> x ) ;
 rect -> h = ( WORD ) ( clamp_int ( py + 16 , 0 , 200 ) - rect -> y ) ;
 }

 static bool make_block_dynamic ( const MovingBlock * mb , DirtyRect * rect , ULONG * key , int * out_px , int * out_py , AmigaSpriteId * out_sprite ) {
 int px ;
 int py ;
 AmigaSpriteId sprite ;
 if ( ! mb -> active || mb -> type == BLOCK_NONE ) {
 return 0 ;
 }
 px = clamp_int ( mb -> pixel_fp_x >> 8 , 0 , 320 - 32 ) ;
 py = clamp_int ( mb -> pixel_fp_y >> 8 , 0 , 200 - 16 ) ;
 sprite = kBlockSprites [ mb -> type ] ;
 make_bob_dirty_rect ( px , py , rect ) ;
 * key = dynamic_bob_key ( sprite , px , py , ( ULONG ) mb -> type ) ;
 if ( out_px ) * out_px = px ;
 if ( out_py ) * out_py = py ;
 if ( out_sprite ) * out_sprite = sprite ;
 return 1 ;
 }

 static bool make_enemy_dynamic ( const RendererState * rr , int buf , int index , const Enemy * enemy , DirtyRect * rect , ULONG * key , int * out_px , int * out_py , AmigaSpriteId * out_sprite ) {
 int px ;
 int py ;
 AmigaSpriteId sprite ;
 if ( ! enemy -> alive || rr -> enemy_hw_sprite [ buf ] [ index ] ) {
 return 0 ;
 }
 px = clamp_int ( enemy -> pixel_fp_x >> 8 , 0 , 320 - 32 ) ;
 py = clamp_int ( enemy -> pixel_fp_y >> 8 , 0 , 200 - 16 ) ;
 sprite = enemy_sprite_for_state ( enemy ) ;
 make_bob_dirty_rect ( px , py , rect ) ;
 * key = dynamic_bob_key ( sprite , px , py , ( ( ULONG ) enemy -> state & 0x0FUL ) | ( ( ( ULONG ) enemy -> type & 0x03UL ) << 4 ) ) ;
 if ( out_px ) * out_px = px ;
 if ( out_py ) * out_py = py ;
 if ( out_sprite ) * out_sprite = sprite ;
 return 1 ;
 }

 static bool make_player_dynamic ( const Player * player , DirtyRect * rect , ULONG * key , int * out_px , int * out_py , AmigaSpriteId * out_sprite ) {
 bool moving ;
 int phase ;
 bool alt ;
 int px ;
 int py ;
 AmigaSpriteId sprite ;
 if ( ! player -> alive ) {
 return 0 ;
 }
 if ( player -> respawn_invuln_ticks > 0 && ( ( player -> respawn_invuln_ticks / 4 ) & 1 ) != 0 ) {
 return 0 ;
 }
 moving = ( player -> state == PLAYER_WALKING || player -> state == PLAYER_PUSHING ) ;
 phase = ( player -> pixel_fp_x + player -> pixel_fp_y ) >> 9 ;
 alt = moving && ( ( phase & 1 ) != 0 ) ;
 px = player -> pixel_fp_x >> 8 ;
 py = player -> pixel_fp_y >> 8 ;
 if ( player -> state == PLAYER_PUSHING && player -> push_timer > 0 ) {
 px += dir_dx ( player -> facing ) ;
 py += dir_dy ( player -> facing ) ;
 }
 px = clamp_int ( px , 0 , 320 - 32 ) ;
 py = clamp_int ( py , 0 , 200 - 16 ) ;
 sprite = player_sprite_for_direction ( player -> facing , alt ) ;
 make_bob_dirty_rect ( px , py , rect ) ;
 * key = dynamic_bob_key ( sprite , px , py , ( ( ULONG ) player -> state & 0x0FUL ) | ( ( ( ULONG ) player -> facing & 0x07UL ) << 4 ) ) ;
 if ( out_px ) * out_px = px ;
 if ( out_py ) * out_py = py ;
 if ( out_sprite ) * out_sprite = sprite ;
 return 1 ;
 }

 static void prepare_dynamic_actors ( RendererState * rr , int buf , const RenderState * rs ) {
 int i ;
 DirtyRect rect ;
 ULONG key ;

 for ( i = 0 ; i < 8 ; ++ i ) {
 bool valid = make_block_dynamic ( & rs -> moving_blocks [ i ] , & rect , & key , 0 , 0 , 0 ) ;
 prepare_dynamic_cache ( rr , buf , & rr -> cached_block [ buf ] [ i ] , valid , key , valid ? & rect : 0 ) ;
 }

 for ( i = 0 ; i < 16 ; ++ i ) {
 bool valid = ( i < rs -> enemy_count ) &&
 make_enemy_dynamic ( rr , buf , i , & rs -> enemies [ i ] , & rect , & key , 0 , 0 , 0 ) ;
 prepare_dynamic_cache ( rr , buf , & rr -> cached_enemy [ buf ] [ i ] , valid , key , valid ? & rect : 0 ) ;
 }

 {
 bool valid = make_player_dynamic ( & rs -> player , & rect , & key , 0 , 0 , 0 ) ;
 prepare_dynamic_cache ( rr , buf , & rr -> cached_player [ buf ] , valid , key , valid ? & rect : 0 ) ;
 }
 }

 static StaticCell make_static_cell ( const RenderState * rs , int x , int y ) {
 StaticCell cell ;
 const bool show_item = item_should_render ( rs -> bonus_item_timer_ticks ) ;
 cell . terrain = ( UBYTE ) rs -> terrain [ y ] [ x ] ;
 cell . block = ( UBYTE ) rs -> blocks [ y ] [ x ] ;
 cell . item = ( UBYTE ) ( show_item ? rs -> items [ y ] [ x ] : ITEM_NONE ) ;
 cell . mine_sprite = 0xFFu ;
 if ( rs -> mines [ y ] [ x ] ) {
 int phase = rs -> round_time_ticks + ( rs -> timer_danger_pulse_ticks * 3 ) + x * 3 + y * 5 ;
 cell . mine_sprite = ( UBYTE ) mine_sprite_for_fuse_ticks ( rs -> mine_fuse_ticks [ y ] [ x ] , phase ) ;
 }
 return cell ;
 }

 static bool is_pushable_block ( BlockType block ) {
 return block == BLOCK_ICE || block == BLOCK_SPECIAL || block == BLOCK_CRACKED ;
 }

 static void draw_title_star_sparkle ( UBYTE * screen , int cx , int cy , int phase ) {
 put_pixel ( screen , cx , cy , 31 ) ;
 if ( phase >= 1 ) {
 put_pixel ( screen , cx - 1 , cy , 30 ) ;
 put_pixel ( screen , cx + 1 , cy , 30 ) ;
 put_pixel ( screen , cx , cy - 1 , 30 ) ;
 put_pixel ( screen , cx , cy + 1 , 30 ) ;
 }
 if ( phase >= 2 ) {
 put_pixel ( screen , cx - 2 , cy , 29 ) ;
 put_pixel ( screen , cx + 2 , cy , 29 ) ;
 put_pixel ( screen , cx , cy - 2 , 29 ) ;
 put_pixel ( screen , cx , cy + 2 , 29 ) ;
 put_pixel ( screen , cx - 1 , cy - 1 , 30 ) ;
 put_pixel ( screen , cx + 1 , cy - 1 , 30 ) ;
 put_pixel ( screen , cx - 1 , cy + 1 , 30 ) ;
 put_pixel ( screen , cx + 1 , cy + 1 , 30 ) ;
 }
 if ( phase >= 3 ) {
 put_pixel ( screen , cx - 3 , cy , 29 ) ;
 put_pixel ( screen , cx + 3 , cy , 29 ) ;
 put_pixel ( screen , cx , cy - 3 , 29 ) ;
 put_pixel ( screen , cx , cy + 3 , 29 ) ;
 put_pixel ( screen , cx - 2 , cy - 2 , 29 ) ;
 put_pixel ( screen , cx + 2 , cy - 2 , 29 ) ;
 put_pixel ( screen , cx - 2 , cy + 2 , 29 ) ;
 put_pixel ( screen , cx + 2 , cy + 2 , 29 ) ;
 }
 }

 static void draw_ice_block_sparkles ( UBYTE * screen , int px , int py , int phase_seed ) {
 int slow_phase = phase_seed / 3 ;
 int wave = slow_phase % 192 ;
 int sparkle_phase = 0 ;
 int cx = px + 6 ;
 int cy = py + 6 ;

 if ( wave < 4 ) {
 sparkle_phase = ( wave < 2 ) ? ( wave + 1 ) : ( 4 - wave ) ;
 } else if ( wave >= 96 && wave < 100 ) {
 sparkle_phase = ( wave < 98 ) ? ( wave - 95 ) : ( 100 - wave ) ;
 cx = px + 10 ;
 cy = py + 9 ;
 }
 if ( sparkle_phase > 0 ) {
 draw_title_star_sparkle ( screen , cx , cy , sparkle_phase ) ;
 }
 }

 static void draw_block_bevel ( UBYTE * screen , int x , int y ) {
 int i ;
 for ( i = 0 ; i < 16 ; ++ i ) {
 put_pixel ( screen , x + i , y , 31 ) ;
 put_pixel ( screen , x , y + i , 31 ) ;
 put_pixel ( screen , x + i , y + 16 - 1 , 1 ) ;
 put_pixel ( screen , x + 16 - 1 , y + i , 1 ) ;
 }
 for ( i = 1 ; i < 16 - 2 ; ++ i ) {
 put_pixel ( screen , x + i , y + 1 , 9 ) ;
 put_pixel ( screen , x + 1 , y + i , 9 ) ;
 put_pixel ( screen , x + i , y + 16 - 2 , 3 ) ;
 put_pixel ( screen , x + 16 - 2 , y + i , 3 ) ;
 }
 }

 static void draw_block_inner_state ( UBYTE * screen , BlockType block , int px , int py , bool animate ) {
 if ( block == BLOCK_NONE ) {
 return ;
 }
 put_pixel ( screen , px + 7 , py + 7 , 31 ) ;
 put_pixel ( screen , px + 8 , py + 8 , 31 ) ;
 put_pixel ( screen , px + 6 , py + 8 , 30 ) ;
 put_pixel ( screen , px + 9 , py + 7 , 30 ) ;
 if ( animate && block == BLOCK_ICE ) {
 draw_ice_block_sparkles ( screen , px , py , ( int ) g_ui_anim_tick + px * 5 + py * 3 ) ;
 }
 }

 static void draw_actor_drop_shadow ( UBYTE * screen , int px , int py ) {
 int x ;
 int sy = py + 16 - 2 ;
 for ( x = 3 ; x <= 12 ; ++ x ) {
 put_pixel ( screen , px + x , sy , ( x & 1 ) ? 2 : 1 ) ;
 }
 for ( x = 5 ; x <= 10 ; ++ x ) {
 put_pixel ( screen , px + x , sy + 1 , 1 ) ;
 }
 }

 static void draw_enemy_threat_corners ( UBYTE * screen , int px , int py , int phase ) {
 UBYTE color = ( ( phase & 1 ) == 0 ) ? 23 : 30 ;
 put_pixel ( screen , px + 1 , py + 1 , color ) ;
 put_pixel ( screen , px + 2 , py + 1 , color ) ;
 put_pixel ( screen , px + 1 , py + 2 , color ) ;
 put_pixel ( screen , px + 16 - 2 , py + 1 , color ) ;
 put_pixel ( screen , px + 16 - 3 , py + 1 , color ) ;
 put_pixel ( screen , px + 16 - 2 , py + 2 , color ) ;
 }

 static void draw_enemy_spawn_cue ( UBYTE * screen , int px , int py , int spawn_ticks ) {
 int i ;
 UBYTE ring ;
 int x0 ;
 int y0 ;
 int x1 ;
 int y1 ;
 if ( spawn_ticks <= 0 ) {
 return ;
 }
 ring = ( spawn_ticks & 4 ) ? 13 : 30 ;
 x0 = px ;
 y0 = py ;
 x1 = px + 16 - 1 ;
 y1 = py + 16 - 1 ;
 for ( i = 0 ; i < 4 ; ++ i ) {
 put_pixel ( screen , x0 + i , y0 , ring ) ;
 put_pixel ( screen , x0 , y0 + i , ring ) ;
 put_pixel ( screen , x1 - i , y0 , ring ) ;
 put_pixel ( screen , x1 , y0 + i , ring ) ;
 put_pixel ( screen , x0 + i , y1 , ring ) ;
 put_pixel ( screen , x0 , y1 - i , ring ) ;
 put_pixel ( screen , x1 - i , y1 , ring ) ;
 put_pixel ( screen , x1 , y1 - i , ring ) ;
 }
 if ( spawn_ticks < 10 ) {
 UBYTE core = ( spawn_ticks & 2 ) ? 30 : 8 ;
 put_pixel ( screen , px + 8 , py + 8 , core ) ;
 put_pixel ( screen , px + 7 , py + 8 , core ) ;
 put_pixel ( screen , px + 8 , py + 7 , core ) ;
 }
 }

 static void draw_push_skid_marks ( UBYTE * screen , int px , int py , Direction facing , int timer ) {
 int blink = ( ( timer & 1 ) == 0 ) ? 1 : 0 ;
 switch ( facing ) {
 case DIR_RIGHT :
 put_pixel ( screen , px + 2 , py + 12 , 2 ) ;
 put_pixel ( screen , px + 1 , py + 11 , 30 ) ;
 if ( blink ) put_pixel ( screen , px + 0 , py + 12 , 2 ) ;
 break ;
 case DIR_LEFT :
 put_pixel ( screen , px + 13 , py + 12 , 2 ) ;
 put_pixel ( screen , px + 14 , py + 11 , 30 ) ;
 if ( blink ) put_pixel ( screen , px + 15 , py + 12 , 2 ) ;
 break ;
 case DIR_UP :
 put_pixel ( screen , px + 7 , py + 14 , 2 ) ;
 put_pixel ( screen , px + 8 , py + 15 , 30 ) ;
 if ( blink ) put_pixel ( screen , px + 9 , py + 14 , 2 ) ;
 break ;
 case DIR_DOWN :
 put_pixel ( screen , px + 7 , py + 2 , 2 ) ;
 put_pixel ( screen , px + 8 , py + 1 , 30 ) ;
 if ( blink ) put_pixel ( screen , px + 9 , py + 2 , 2 ) ;
 break ;
 default :
 break ;
 }
 }

 static void render_static_cell ( const AmigaApp * app , UBYTE * screen , const StaticCell * cell , int tx , int ty ) {
 int px = tx * 16 ;
 int py = ty * 16 ;
 blit_opaque_tile ( app , screen , kTerrainSprites [ cell -> terrain ] , px , py ) ;
 if ( cell -> block != BLOCK_NONE ) {
 blit_opaque_tile ( app , screen , kBlockSprites [ cell -> block ] , px , py ) ;
 }
 if ( cell -> item != ITEM_NONE ) {
 blit_cookie_bob ( app , screen , kItemSprites [ cell -> item ] , px , py ) ;
 }
 if ( cell -> mine_sprite != 0xFFu ) {
 blit_cookie_bob ( app , screen , ( AmigaSpriteId ) cell -> mine_sprite , px , py ) ;
 }
 }

 static void update_static_dirty ( RendererState * rr , const RenderState * rs , int buf ) {
 if ( ! rr -> prev_valid [ buf ] ) {
 mark_all_dirty ( rr , buf ) ;
 rr -> prev_valid [ buf ] = 1 ;
 }
 }

 static void mark_core_dirty_static ( RendererState * rr , const GameState * gs ) {
 const uint32_t * rows = game_dirty_static_rows ( gs ) ;
 int buf ;
 int y ;
 if ( ! rows ) {
 return ;
 }
 for ( y = 0 ; y < 12 ; ++ y ) {
 uint32_t row = rows [ y ] ;
 if ( row == 0 ) {
 continue ;
 }
 for ( buf = 0 ; buf < 2 ; ++ buf ) {
 int x ;
 for ( x = 0 ; x < 20 ; ++ x ) {
 if ( ( row & ( ( uint32_t ) 1u << ( uint32_t ) x ) ) != 0u ) {
 rr -> dirty [ buf ] [ y ] [ x ] = 1 ;
 }
 }
 }
 }
 }

 static void render_dirty_static ( const AmigaApp * app , UBYTE * screen , int buf , const RenderState * rs ) {
 int x ;
 int y ;
 RendererState * rr = ( RendererState * ) & app -> renderer ;
 for ( y = 0 ; y < 12 ; ++ y ) {
 for ( x = 0 ; x < 20 ; ++ x ) {
 if ( ! rr -> dirty [ buf ] [ y ] [ x ] ) {
 continue ;
 }
 rr -> prev_static [ buf ] [ y ] [ x ] = make_static_cell ( rs , x , y ) ;
 render_static_cell ( app , screen , & rr -> prev_static [ buf ] [ y ] [ x ] , x , y ) ;
 rr -> dirty [ buf ] [ y ] [ x ] = 0 ;
 }
 }
 }

 static void prepare_enemy_hardware_sprites ( AmigaApp * app , RendererState * rr , int buf , const RenderState * rs , bool disabled ) {





























 __asm_memset ( rr -> enemy_hw_sprite [ buf ] , 0 , sizeof ( rr -> enemy_hw_sprite [ buf ] ) ) ;

 }

 static void draw_cpu_spark ( UBYTE * screen , int cx , int cy , UBYTE color ) {
 put_pixel ( screen , cx , cy , color ) ;
 put_pixel ( screen , cx - 1 , cy , color ) ;
 put_pixel ( screen , cx + 1 , cy , color ) ;
 put_pixel ( screen , cx , cy - 1 , color ) ;
 put_pixel ( screen , cx , cy + 1 , color ) ;
 }

 static void draw_score_popups ( const AmigaApp * app , RendererState * rr , int buf , UBYTE * screen , const RenderState * rs ) {
 int i ;
 wait_blitter ( ) ;
 for ( i = 0 ; i < 8 ; ++ i ) {
 const ScorePopup * popup = & rs -> score_popups [ i ] ;
 int value ;
 int width ;
 int drift_x ;
 int pop_y_offset ;
 int px ;
 int py ;
 if ( ! popup -> active || popup -> ttl_ticks <= 0 ) {
 continue ;
 }
 if ( popup -> ttl_ticks < 10 && ( ( popup -> ttl_ticks & 1 ) != 0 ) ) {
 continue ;
 }
 value = popup -> value ;
 if ( value < 0 ) {
 value = 0 ;
 }
 if ( value > 9999 ) {
 value = 9999 ;
 }
 width = number_width ( ( uint32_t ) value , 0 ) ;
 drift_x = ( ( i & 1 ) == 0 ) ? - 2 : 2 ;
 pop_y_offset = ( i & 1 ) + ( ( popup -> ttl_ticks / 6 ) & 1 ) ;
 px = clamp_int ( ( popup -> pixel_fp_x >> 8 ) - ( width / 2 ) + drift_x , 1 , 320 - width - 2 ) ;
 py = clamp_int ( ( popup -> pixel_fp_y >> 8 ) - 3 - pop_y_offset , 1 , ( 12 * 16 ) - 8 ) ;
 save_dynamic_rect ( app , rr , buf , screen , px - 2 , py - 2 , width + 4 , 10 ) ;
 draw_number_outlined ( screen , px , py , ( uint32_t ) value , 0 , ( popup -> value >= 800 ) ? 8 : ( ( popup -> value >= 400 ) ? 12 : 30 ) , 1 ) ;
 }
 }

 static void draw_impact_fx ( const AmigaApp * app , RendererState * rr , int buf , UBYTE * screen , const RenderState * rs ) {
 int i ;
 for ( i = 0 ; i < 12 ; ++ i ) {
 const ImpactFx * fx = & rs -> impact_fx [ i ] ;
 int cx ;
 int cy ;
 bool death_style ;
 if ( ! fx -> active || fx -> ttl_ticks <= 0 ) {
 continue ;
 }
 if ( fx -> ttl_ticks < 4 && ( ( fx -> ttl_ticks & 1 ) != 0 ) ) {
 continue ;
 }
 cx = fx -> pixel_fp_x >> 8 ;
 cy = fx -> pixel_fp_y >> 8 ;
 death_style = fx -> style == IMPACT_FX_STYLE_ENEMY_DEATH_A ||
 fx -> style == IMPACT_FX_STYLE_ENEMY_DEATH_B ;
 if ( death_style ) {
 int age_ticks = fx -> base_ttl_ticks - fx -> ttl_ticks ;
 int px = clamp_int ( cx - 8 , 0 , 320 - 32 ) ;
 int py = clamp_int ( cy - 8 + ( ( age_ticks > 2 ) ? ( ( age_ticks - 2 ) / 4 ) : 0 ) , 0 , 200 - 16 ) ;
 save_dynamic_rect ( app , rr , buf , screen , px , py , 32 , 16 ) ;
 blit_cookie_bob ( app , screen , death_sprite_for_fx ( fx ) , px , py ) ;
 } else {

 continue ;





































 }
 }
 }

 static void draw_dynamic_actors ( const AmigaApp * app , RendererState * rr , int buf , UBYTE * screen , const RenderState * rs ) {
 int i ;

 for ( i = 0 ; i < 8 ; ++ i ) {
 const MovingBlock * mb = & rs -> moving_blocks [ i ] ;
 DynamicCache * cache = & rr -> cached_block [ buf ] [ i ] ;
 DirtyRect rect ;
 ULONG key ;
 AmigaSpriteId sprite ;
 int px ;
 int py ;
 if ( ! make_block_dynamic ( mb , & rect , & key , & px , & py , & sprite ) ) {
 continue ;
 }
 if ( cache -> valid && cache -> key == key && dirty_rects_same ( & cache -> rect , & rect ) ) {
 continue ;
 }
 blit_cookie_bob ( app , screen , sprite , px , py ) ;
 cache -> rect = rect ;
 cache -> key = key ;
 cache -> valid = 1 ;
 }

 for ( i = 0 ; i < rs -> enemy_count ; ++ i ) {
 const Enemy * enemy = & rs -> enemies [ i ] ;
 DynamicCache * cache = & rr -> cached_enemy [ buf ] [ i ] ;
 DirtyRect rect ;
 AmigaSpriteId sprite ;
 ULONG key ;
 int px ;
 int py ;
 if ( ! make_enemy_dynamic ( rr , buf , i , enemy , & rect , & key , & px , & py , & sprite ) ) {
 continue ;
 }
 if ( cache -> valid && cache -> key == key && dirty_rects_same ( & cache -> rect , & rect ) ) {
 continue ;
 }
 blit_cookie_bob ( app , screen , sprite , px , py ) ;
 if ( enemy -> state == ENEMY_SPAWNING && enemy -> spawn_ticks > 0 ) {




 } else {




 }
 cache -> rect = rect ;
 cache -> key = key ;
 cache -> valid = 1 ;
 }

 {
 DynamicCache * cache = & rr -> cached_player [ buf ] ;
 DirtyRect rect ;
 ULONG key ;
 AmigaSpriteId sprite ;
 int px ;
 int py ;
 if ( make_player_dynamic ( & rs -> player , & rect , & key , & px , & py , & sprite ) ) {
 if ( cache -> valid && cache -> key == key && dirty_rects_same ( & cache -> rect , & rect ) ) {
 return ;
 }






 blit_cookie_bob ( app , screen , sprite , px , py ) ;
 cache -> rect = rect ;
 cache -> key = key ;
 cache -> valid = 1 ;
 }
 }
 }

 static void draw_hud_separator ( UBYTE * screen , int x , UBYTE dark , UBYTE light ) {
 int y ;
 for ( y = 193 ; y < 200 - 1 ; ++ y ) {
 put_pixel ( screen , x , y , dark ) ;
 put_pixel ( screen , x + 1 , y , light ) ;
 }
 }

 static void draw_hud_bomb_meter ( UBYTE * screen , int x , int y , int mine_stock , int mine_capacity , int anim_phase ) {
 int i ;
 int stock = clamp_int ( mine_stock , 0 , 8 ) ;
 int capacity = clamp_int ( mine_capacity , 0 , 8 ) ;
 for ( i = 0 ; i < 8 ; ++ i ) {
 int cell_x = x + ( i % 4 ) * 3 ;
 int cell_y = y + ( i / 4 ) * 3 ;
 UBYTE color = 2 ;
 if ( i < capacity ) {
 color = 3 ;
 }
 if ( i < stock ) {
 color = ( ( ( ( anim_phase / 4 ) + i ) & 1 ) == 0 ) ? 30 : 22 ;
 }
 put_pixel ( screen , cell_x , cell_y , color ) ;
 put_pixel ( screen , cell_x + 1 , cell_y , color ) ;
 put_pixel ( screen , cell_x , cell_y + 1 , color ) ;
 put_pixel ( screen , cell_x + 1 , cell_y + 1 , color ) ;
 }
 }

 static UBYTE hud_timer_color ( const RenderState * rs ) {
 return rs -> timer_danger_active ? ( ( ( rs -> round_time_ticks / 4 ) & 1 ) ? 23 : 12 ) : 14 ;
 }

 static UBYTE hud_bomb_color ( const RenderState * rs ) {
 if ( rs -> run_mine_capacity <= 0 ) {
 return 3 ;
 }
 if ( rs -> run_mine_stock <= 0 ) {
 return 23 ;
 }
 return ( ( ( rs -> round_time_ticks / 5 ) & 1 ) == 0 ) ? 30 : 22 ;
 }

 static void clear_hud_field ( UBYTE * screen , int x0 , int x1 ) {
 draw_rect ( screen , x0 , 192 , x1 , 200 - 2 , 1 ) ;
 }

 static void draw_hud_values ( UBYTE * screen , const RenderState * rs ) {
 int timer_seconds = rs -> round_time_ticks > 0 ? ( rs -> round_time_ticks / 50 ) : 0 ;
 UBYTE timer_color = hud_timer_color ( rs ) ;
 UBYTE bomb_color = hud_bomb_color ( rs ) ;

 draw_number_outlined ( screen , 34 , 193 , rs -> score > 99999999u ? 99999999u : rs -> score , 6 , 29 , 1 ) ;
 draw_number_outlined ( screen , 161 , 193 , ( uint32_t ) rs -> round , 2 , 18 , 1 ) ;
 draw_number_outlined ( screen , 201 , 193 , ( uint32_t ) timer_seconds , 2 , timer_color , 1 ) ;
 draw_number_outlined ( screen , 244 , 193 , ( uint32_t ) clamp_int ( rs -> run_mine_stock , 0 , 99 ) , 1 , bomb_color , 1 ) ;
 draw_text_outlined ( screen , 248 , 193 , ":" , 7 , 1 ) ;
 draw_number_outlined ( screen , 252 , 193 , ( uint32_t ) clamp_int ( rs -> run_mine_capacity , 0 , 99 ) , 1 , bomb_color , 1 ) ;
 draw_hud_bomb_meter ( screen , 255 , 193 , rs -> run_mine_stock , rs -> run_mine_capacity , rs -> round_time_ticks ) ;
 draw_number_outlined ( screen , 287 , 193 , ( uint32_t ) clamp_int ( rs -> lives , 0 , 99 ) , 2 , 23 , 1 ) ;
 }

 static void draw_hud_delta ( UBYTE * screen , const RenderState * rs , const HudSnapshot * old_hud , const HudSnapshot * new_hud ) {
 wait_blitter ( ) ;
 if ( old_hud -> score != new_hud -> score ) {
 clear_hud_field ( screen , 33 , 140 ) ;
 draw_number_outlined ( screen , 34 , 193 , new_hud -> score , 6 , 29 , 1 ) ;
 }
 if ( old_hud -> round != new_hud -> round ) {
 clear_hud_field ( screen , 160 , 173 ) ;
 draw_number_outlined ( screen , 161 , 193 , ( uint32_t ) new_hud -> round , 2 , 18 , 1 ) ;
 }
 if ( old_hud -> timer_seconds != new_hud -> timer_seconds ||
 old_hud -> timer_danger != new_hud -> timer_danger ||
 old_hud -> timer_flash_phase != new_hud -> timer_flash_phase ) {
 clear_hud_field ( screen , 200 , 216 ) ;
 draw_number_outlined ( screen , 201 , 193 , ( uint32_t ) new_hud -> timer_seconds , 2 , hud_timer_color ( rs ) , 1 ) ;
 }
 if ( old_hud -> mine_stock != new_hud -> mine_stock ||
 old_hud -> mine_capacity != new_hud -> mine_capacity ||
 old_hud -> bomb_flash_phase != new_hud -> bomb_flash_phase ) {
 UBYTE bomb_color = hud_bomb_color ( rs ) ;
 clear_hud_field ( screen , 243 , 265 ) ;
 draw_number_outlined ( screen , 244 , 193 , ( uint32_t ) new_hud -> mine_stock , 1 , bomb_color , 1 ) ;
 draw_text_outlined ( screen , 248 , 193 , ":" , 7 , 1 ) ;
 draw_number_outlined ( screen , 252 , 193 , ( uint32_t ) new_hud -> mine_capacity , 1 , bomb_color , 1 ) ;
 draw_hud_bomb_meter ( screen , 255 , 193 , rs -> run_mine_stock , rs -> run_mine_capacity , rs -> round_time_ticks ) ;
 }
 if ( old_hud -> lives != new_hud -> lives ) {
 clear_hud_field ( screen , 286 , 306 ) ;
 draw_number_outlined ( screen , 287 , 193 , ( uint32_t ) new_hud -> lives , 2 , 23 , 1 ) ;
 }
 }

 static void draw_hud ( UBYTE * screen , const RenderState * rs ) {
 int x ;
 int y ;

 wait_blitter ( ) ;
 draw_rect ( screen , 0 , 192 , 320 - 1 , 200 - 1 , 1 ) ;
 for ( x = 0 ; x < 320 ; ++ x ) {
 put_pixel ( screen , x , 192 , 3 ) ;
 put_pixel ( screen , x , 200 - 1 , 2 ) ;
 }
 for ( y = 193 ; y < 200 - 1 ; ++ y ) {
 if ( ( y & 1 ) == 0 ) {
 for ( x = 0 ; x < 320 ; x += 32 ) {
 put_pixel ( screen , x , y , 2 ) ;
 }
 }
 }
 draw_hud_separator ( screen , 142 , 2 , 3 ) ;
 draw_hud_separator ( screen , 174 , 2 , 3 ) ;
 draw_hud_separator ( screen , 218 , 2 , 3 ) ;
 draw_hud_separator ( screen , 266 , 2 , 3 ) ;

 draw_text_outlined ( screen , 4 , 193 , "SCORE" , 7 , 1 ) ;
 draw_text_outlined ( screen , 148 , 193 , "RD" , 7 , 1 ) ;
 draw_text_outlined ( screen , 180 , 193 , "TIME" , 7 , 1 ) ;
 draw_text_outlined ( screen , 224 , 193 , "BOMB" , 7 , 1 ) ;
 draw_text_outlined ( screen , 271 , 193 , "LIV" , 7 , 1 ) ;
 draw_hud_values ( screen , rs ) ;
 }

 static HudSnapshot make_hud_snapshot ( const RenderState * rs ) {
 HudSnapshot hud ;
 hud . score = rs -> score > 99999999u ? 99999999u : rs -> score ;
 hud . round = ( UWORD ) clamp_int ( rs -> round , 0 , 999 ) ;
 hud . timer_seconds = ( UWORD ) ( ( rs -> round_time_ticks > 0 ) ? ( rs -> round_time_ticks / 50 ) : 0 ) ;
 hud . lives = ( UWORD ) clamp_int ( rs -> lives , 0 , 99 ) ;
 hud . mine_stock = ( UWORD ) clamp_int ( rs -> run_mine_stock , 0 , 99 ) ;
 hud . mine_capacity = ( UWORD ) clamp_int ( rs -> run_mine_capacity , 0 , 99 ) ;
 hud . timer_danger = rs -> timer_danger_active ? 1u : 0u ;
 hud . timer_flash_phase = hud . timer_danger ? ( UBYTE ) ( ( rs -> round_time_ticks / 4 ) & 1 ) : 0u ;
 hud . bomb_flash_phase = ( rs -> run_mine_capacity > 0 && rs -> run_mine_stock > 0 )
 ? ( UBYTE ) ( ( ( ( rs -> round_time_ticks / 4 ) & 1 ) << 1 ) | ( ( rs -> round_time_ticks / 5 ) & 1 ) )
 : 0u ;
 hud . special_border_phase = 0u ;
 return hud ;
 }

 static inline bool hud_snapshot_equal ( const HudSnapshot * a , const HudSnapshot * b ) {
 return a -> score == b -> score &&
 a -> round == b -> round &&
 a -> timer_seconds == b -> timer_seconds &&
 a -> lives == b -> lives &&
 a -> mine_stock == b -> mine_stock &&
 a -> mine_capacity == b -> mine_capacity &&
 a -> timer_danger == b -> timer_danger &&
 a -> timer_flash_phase == b -> timer_flash_phase &&
 a -> bomb_flash_phase == b -> bomb_flash_phase &&
 a -> special_border_phase == b -> special_border_phase ;
 }

 static void draw_panel ( UBYTE * screen , int x0 , int y0 , int x1 , int y1 , UBYTE accent ) {
 int panel_w = x1 - x0 + 1 ;
 int panel_h = y1 - y0 + 1 ;
 int perimeter = ( ( panel_w + panel_h ) * 2 ) - 4 ;
 draw_rect ( screen , x0 , y0 , x1 , y1 , 1 ) ;
 draw_rect ( screen , x0 , y0 , x1 , y0 , 30 ) ;
 draw_rect ( screen , x0 , y1 , x1 , y1 , 30 ) ;
 draw_rect ( screen , x0 , y0 , x0 , y1 , 30 ) ;
 draw_rect ( screen , x1 , y0 , x1 , y1 , 30 ) ;
 draw_rect ( screen , x0 + 2 , y0 + 2 , x1 - 2 , y0 + 2 , accent ) ;
 draw_rect ( screen , x0 + 2 , y1 - 2 , x1 - 2 , y1 - 2 , accent ) ;

 if ( perimeter > 0 ) {
 int trail ;
 int head = ( int ) ( ( g_ui_anim_tick / 2 ) % ( ULONG ) perimeter ) ;
 for ( trail = 0 ; trail < 5 ; ++ trail ) {
 int t = head - trail ;
 int sx = x0 ;
 int sy = y0 ;
 UBYTE color = ( trail == 0 ) ? 31 : ( ( trail == 1 ) ? 30 : ( ( trail < 4 ) ? accent : 2 ) ) ;
 while ( t < 0 ) {
 t += perimeter ;
 }
 t %= perimeter ;
 if ( t < panel_w ) {
 sx = x0 + t ;
 sy = y0 ;
 } else if ( t < panel_w + panel_h - 1 ) {
 sx = x1 ;
 sy = y0 + ( t - panel_w ) + 1 ;
 } else if ( t < panel_w + panel_h + panel_w - 2 ) {
 sx = x1 - ( t - panel_w - ( panel_h - 1 ) ) - 1 ;
 sy = y1 ;
 } else {
 sx = x0 ;
 sy = y1 - ( t - panel_w - ( panel_h - 1 ) - ( panel_w - 1 ) ) - 1 ;
 }
 put_pixel ( screen , sx , sy , color ) ;
 }
 }
 }

 static void draw_card_frame ( UBYTE * screen , int x0 , int y0 , int w , int h , UBYTE accent , bool selected ) {
 int x ;
 int y ;
 int x1 = x0 + w - 1 ;
 int y1 = y0 + h - 1 ;
 UBYTE outer = selected ? 30 : 3 ;
 UBYTE inner = selected ? accent : 2 ;

 draw_rect ( screen , x0 , y0 , x1 , y1 , 1 ) ;
 draw_hline ( screen , x0 , x1 , y0 , outer ) ;
 draw_hline ( screen , x0 , x1 , y1 , outer ) ;
 draw_rect ( screen , x0 , y0 , x0 , y1 , outer ) ;
 draw_rect ( screen , x1 , y0 , x1 , y1 , outer ) ;

 draw_hline ( screen , x0 + 1 , x1 - 1 , y0 + 1 , inner ) ;
 draw_hline ( screen , x0 + 1 , x1 - 1 , y1 - 1 , inner ) ;
 draw_rect ( screen , x0 + 1 , y0 + 1 , x0 + 1 , y1 - 1 , inner ) ;
 draw_rect ( screen , x1 - 1 , y0 + 1 , x1 - 1 , y1 - 1 , inner ) ;

 for ( y = y0 + 5 ; y <= y1 - 5 ; y += 4 ) {
 int offset = ( ( y - y0 ) & 4 ) ? 6 : 2 ;
 for ( x = x0 + offset ; x <= x1 - 5 ; x += 8 ) {
 put_pixel ( screen , x , y , 2 ) ;
 }
 }

 if ( selected && y0 >= 3 ) {
 int cx = x0 + ( w / 2 ) ;
 put_pixel ( screen , cx , y0 - 3 , 30 ) ;
 put_pixel ( screen , cx - 1 , y0 - 2 , accent ) ;
 put_pixel ( screen , cx , y0 - 2 , accent ) ;
 put_pixel ( screen , cx + 1 , y0 - 2 , accent ) ;
 put_pixel ( screen , cx , y0 - 1 , 30 ) ;
 }
 }

 static void draw_opening_high_scores ( UBYTE * screen ) {
 int i ;
 const int x0 = 86 ;
 const int x1 = 233 ;
 const int y0 = 96 ;
 const int y1 = 144 ;
 const int left_x = x0 + 10 ;
 const int score_right = x1 - 10 ;

 draw_panel ( screen , x0 , y0 , x1 , y1 , 16 ) ;
 draw_centered_text ( screen , x0 , x1 , y0 + 6 , "HI SCORES" , 30 ) ;
 for ( i = 0 ; i < 5 ; ++ i ) {
 int row_y = y0 + 14 + i * 6 ;
 draw_number ( screen , left_x , row_y , ( uint32_t ) ( i + 1 ) , 1 , 29 ) ;
 draw_text ( screen , left_x + 8 , row_y , kOpeningHiScores [ i ] . initials , 29 ) ;
 draw_number_right ( screen , score_right , row_y , kOpeningHiScores [ i ] . score , 5 , 30 ) ;
 }
 }

 static void draw_visual_arrow_glyph ( UBYTE * screen , int x , int y , Direction dir , UBYTE color ) {
 switch ( dir ) {
 case DIR_LEFT :
 put_pixel ( screen , x + 0 , y + 2 , color ) ;
 put_pixel ( screen , x + 1 , y + 1 , color ) ;
 put_pixel ( screen , x + 1 , y + 2 , color ) ;
 put_pixel ( screen , x + 1 , y + 3 , color ) ;
 put_pixel ( screen , x + 2 , y + 2 , color ) ;
 put_pixel ( screen , x + 3 , y + 2 , color ) ;
 break ;
 case DIR_RIGHT :
 put_pixel ( screen , x + 0 , y + 2 , color ) ;
 put_pixel ( screen , x + 1 , y + 2 , color ) ;
 put_pixel ( screen , x + 2 , y + 1 , color ) ;
 put_pixel ( screen , x + 2 , y + 2 , color ) ;
 put_pixel ( screen , x + 2 , y + 3 , color ) ;
 put_pixel ( screen , x + 3 , y + 2 , color ) ;
 break ;
 case DIR_UP :
 put_pixel ( screen , x + 1 , y + 0 , color ) ;
 put_pixel ( screen , x + 2 , y + 0 , color ) ;
 put_pixel ( screen , x + 0 , y + 1 , color ) ;
 put_pixel ( screen , x + 1 , y + 1 , color ) ;
 put_pixel ( screen , x + 2 , y + 1 , color ) ;
 put_pixel ( screen , x + 3 , y + 1 , color ) ;
 put_pixel ( screen , x + 1 , y + 2 , color ) ;
 put_pixel ( screen , x + 2 , y + 2 , color ) ;
 put_pixel ( screen , x + 1 , y + 3 , color ) ;
 put_pixel ( screen , x + 2 , y + 3 , color ) ;
 break ;
 case DIR_DOWN :
 put_pixel ( screen , x + 1 , y + 0 , color ) ;
 put_pixel ( screen , x + 2 , y + 0 , color ) ;
 put_pixel ( screen , x + 1 , y + 1 , color ) ;
 put_pixel ( screen , x + 2 , y + 1 , color ) ;
 put_pixel ( screen , x + 0 , y + 2 , color ) ;
 put_pixel ( screen , x + 1 , y + 2 , color ) ;
 put_pixel ( screen , x + 2 , y + 2 , color ) ;
 put_pixel ( screen , x + 3 , y + 2 , color ) ;
 put_pixel ( screen , x + 1 , y + 3 , color ) ;
 put_pixel ( screen , x + 2 , y + 3 , color ) ;
 break ;
 default :
 break ;
 }
 }

 static void draw_continue_chevrons ( UBYTE * screen , int x , int y , UBYTE color_a , UBYTE color_b ) {
 UBYTE color = ( ( ( g_ui_anim_tick / 6 ) & 1u ) == 0u ) ? color_a : color_b ;
 draw_visual_arrow_glyph ( screen , x + 0 , y , DIR_RIGHT , color ) ;
 draw_visual_arrow_glyph ( screen , x + 5 , y , DIR_RIGHT , color ) ;
 draw_visual_arrow_glyph ( screen , x + 10 , y , DIR_RIGHT , color ) ;
 }

 static void draw_round_intro_visual_lane ( const AmigaApp * app , UBYTE * screen , const RenderState * rs , int lane_x0 , int lane_y0 , int lane_w , int lane_h ) {
 int lane_y = lane_y0 + ( ( lane_h - 16 ) / 2 ) ;
 int left_x = lane_x0 + 8 ;
 int right_x = lane_x0 + lane_w - 16 - 8 ;
 int span = right_x - left_x ;
 int block_x = left_x + ( span / 3 ) ;
 int enemy_x = left_x + ( ( span * 2 ) / 3 ) ;
 int mine_x = right_x ;
 UBYTE cue = ( ( ( ( g_ui_anim_tick / 5 ) & 1u ) == 0u ) ? 30 : 14 ) ;

 blit_cookie_bob ( app , screen , player_sprite_for_direction ( DIR_RIGHT , 0 ) , left_x , lane_y ) ;
 wait_blitter ( ) ;
 draw_visual_arrow_glyph ( screen , left_x + 6 , lane_y - 4 , DIR_UP , cue ) ;
 draw_visual_arrow_glyph ( screen , left_x + 6 , lane_y + 16 , DIR_DOWN , cue ) ;
 draw_visual_arrow_glyph ( screen , left_x - 5 , lane_y + 6 , DIR_LEFT , cue ) ;
 draw_visual_arrow_glyph ( screen , left_x + 17 , lane_y + 6 , DIR_RIGHT , cue ) ;

 blit_cookie_bob ( app , screen , AMIGA_SPR_BLOCK_ICE , block_x , lane_y ) ;
 wait_blitter ( ) ;
 draw_block_bevel ( screen , block_x , lane_y ) ;
 draw_ice_block_sparkles ( screen , block_x , lane_y , ( int ) g_ui_anim_tick + block_x * 5 + lane_y * 3 ) ;
 draw_visual_arrow_glyph ( screen , ( ( block_x + enemy_x ) / 2 ) - 2 , lane_y + 6 , DIR_RIGHT , cue ) ;

 blit_cookie_bob ( app , screen , ( ( ( g_ui_anim_tick / 8 ) & 1u ) != 0u ) ? AMIGA_SPR_ENEMY_A_ALT : AMIGA_SPR_ENEMY_A , enemy_x , lane_y ) ;
 if ( ( ( g_ui_anim_tick / 4 ) & 1u ) == 0u ) {
 blit_cookie_bob ( app , screen , AMIGA_SPR_EFFECT_SPARK , enemy_x + 2 , lane_y - 2 ) ;
 }

 if ( rs -> run_mine_capacity > 0 ) {
 AmigaSpriteId mine_sprite = ( ( ( g_ui_anim_tick / 6 ) & 1u ) == 0u ) ? AMIGA_SPR_MINE_1 : AMIGA_SPR_MINE_2 ;
 blit_cookie_bob ( app , screen , mine_sprite , mine_x , lane_y ) ;
 wait_blitter ( ) ;
 draw_visual_arrow_glyph ( screen , mine_x - 7 , lane_y + 6 , DIR_LEFT , cue ) ;
 } else {
 blit_cookie_bob ( app , screen , AMIGA_SPR_MINE_0 , mine_x , lane_y ) ;
 }
 wait_blitter ( ) ;
 }

 static void draw_perk_choice_panel ( const AmigaApp * app , UBYTE * screen , const RenderState * rs ) {
 int i ;
 const int panel_x0 = 48 ;
 const int panel_x1 = 271 ;
 const int panel_y0 = 58 ;
 const int panel_y1 = 143 ;
 const int card_w = 58 ;
 const int card_h = 44 ;
 const int spacing = 8 ;
 int count = rs -> perk_choice_count ;
 int total_w ;
 int base_x ;
 int selected_idx ;
 PerkType selected ;
 UBYTE selected_color ;

 if ( count < 1 ) {
 count = 1 ;
 }
 if ( count > 3 ) {
 count = 3 ;
 }
 total_w = count * card_w + ( count - 1 ) * spacing ;
 base_x = panel_x0 + ( ( panel_x1 - panel_x0 + 1 - total_w ) / 2 ) ;

 draw_panel ( screen , panel_x0 , panel_y0 , panel_x1 , panel_y1 , 18 ) ;
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , 64 , "PICK PERK" , 31 , 1 ) ;

 for ( i = 0 ; i < count ; ++ i ) {
 PerkType perk = rs -> perk_choices [ i ] ;
 bool selected_card = ( i == rs -> perk_choice_selected ) ;
 UBYTE accent = perk_color ( perk ) ;
 int px = base_x + i * ( card_w + spacing ) ;
 int py = 75 ;

 draw_card_frame ( screen , px , py , card_w , card_h , accent , selected_card ) ;
 blit_cookie_bob ( app , screen , perk_sprite ( perk ) , px + ( ( card_w - 16 ) / 2 ) , py + 5 ) ;
 wait_blitter ( ) ;
 draw_centered_text_outlined ( screen , px + 2 , px + card_w - 3 , py + 24 , perk_label ( perk ) , accent , 1 ) ;
 draw_text_outlined ( screen , px + 18 , py + 31 , "LV" , 30 , 1 ) ;
 draw_number_outlined ( screen , px + 30 , py + 31 , ( uint32_t ) ( rs -> perk_levels [ perk ] + 1 ) , 1 , 30 , 1 ) ;
 }

 selected_idx = rs -> perk_choice_selected ;
 if ( selected_idx < 0 || selected_idx >= count ) {
 selected_idx = 0 ;
 }
 selected = rs -> perk_choices [ selected_idx ] ;
 selected_color = perk_color ( selected ) ;
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , 124 , perk_label ( selected ) , selected_color , 1 ) ;
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , 130 , perk_effect_label ( selected ) , 30 , 1 ) ;
 draw_visual_arrow_glyph ( screen , panel_x0 + 8 , 135 , DIR_LEFT , 30 ) ;
 draw_visual_arrow_glyph ( screen , panel_x1 - 12 , 135 , DIR_RIGHT , 30 ) ;
 draw_continue_chevrons ( screen , panel_x0 + ( ( panel_x1 - panel_x0 ) / 2 ) - 7 , 135 , selected_color , 30 ) ;
 }

 static void draw_meta_choice_panel ( const AmigaApp * app , UBYTE * screen , const RenderState * rs ) {
 int i ;
 const int panel_x0 = 36 ;
 const int panel_x1 = 283 ;
 const int panel_y0 = 58 ;
 const int panel_y1 = 143 ;
 const int card_w = 48 ;
 const int card_h = 44 ;
 const int spacing = 8 ;
 int count = rs -> meta_choice_count ;
 int total_w ;
 int base_x ;
 int selected_idx ;
 MetaUpgradeType selected ;
 int cost ;
 bool affordable ;
 UBYTE selected_color ;

 if ( count < 1 ) {
 count = 1 ;
 }
 if ( count > 4 ) {
 count = 4 ;
 }
 total_w = count * card_w + ( count - 1 ) * spacing ;
 base_x = panel_x0 + ( ( panel_x1 - panel_x0 + 1 - total_w ) / 2 ) ;

 draw_panel ( screen , panel_x0 , panel_y0 , panel_x1 , panel_y1 , 16 ) ;
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , 64 , "META UPGRADE" , 31 , 1 ) ;
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , 70 , "SHARDS" , 30 , 1 ) ;
 draw_number_right ( screen , panel_x1 - 10 , 70 , rs -> meta_shards , 1 , 8 ) ;

 for ( i = 0 ; i < count ; ++ i ) {
 MetaUpgradeType upgrade = rs -> meta_choices [ i ] ;
 int item_cost = rs -> meta_choice_costs [ i ] ;
 bool card_selected = ( i == rs -> meta_choice_selected ) ;
 bool card_affordable = rs -> meta_shards >= ( uint32_t ) ( ( item_cost < 0 ) ? 0 : item_cost ) ;
 UBYTE accent = card_affordable ? meta_color ( upgrade ) : 23 ;
 int px = base_x + i * ( card_w + spacing ) ;
 int py = 75 ;

 draw_card_frame ( screen , px , py , card_w , card_h , accent , card_selected ) ;
 blit_cookie_bob ( app , screen , meta_sprite ( upgrade ) , px + ( ( card_w - 16 ) / 2 ) , py + 5 ) ;
 wait_blitter ( ) ;
 draw_centered_text_outlined ( screen , px + 2 , px + card_w - 3 , py + 24 , meta_label ( upgrade ) , accent , 1 ) ;
 draw_text_outlined ( screen , px + 14 , py + 31 , "C" , card_affordable ? 30 : 23 , 1 ) ;
 draw_number_outlined ( screen , px + 22 , py + 31 , ( uint32_t ) clamp_int ( item_cost , 0 , 999 ) , 1 , card_affordable ? 30 : 23 , 1 ) ;
 }

 selected_idx = rs -> meta_choice_selected ;
 if ( selected_idx < 0 || selected_idx >= count ) {
 selected_idx = 0 ;
 }
 selected = rs -> meta_choices [ selected_idx ] ;
 cost = rs -> meta_choice_costs [ selected_idx ] ;
 affordable = rs -> meta_shards >= ( uint32_t ) ( ( cost < 0 ) ? 0 : cost ) ;
 selected_color = affordable ? 30 : 23 ;
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , 124 , meta_label ( selected ) , affordable ? meta_color ( selected ) : 23 , 1 ) ;
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , 130 , meta_effect_label ( selected ) , selected_color , 1 ) ;
 draw_visual_arrow_glyph ( screen , panel_x0 + 8 , 135 , DIR_LEFT , 30 ) ;
 draw_visual_arrow_glyph ( screen , panel_x1 - 12 , 135 , DIR_RIGHT , 30 ) ;
 draw_continue_chevrons ( screen , panel_x0 + ( ( panel_x1 - panel_x0 ) / 2 ) - 7 , 135 , selected_color , affordable ? 16 : 2 ) ;
 }

 static void draw_title_overlay_art ( const AmigaApp * app , UBYTE * screen , int center_x , int top_y ) {
 static const int sparkle_anchors [ 5 ] [ 2 ] = {
 { 88 , 16 } ,
 { 170 , 14 } ,
 { 36 , 30 } ,
 { 124 , 31 } ,
 { 198 , 32 }
 } ;
 int bob = ( ( ( ( g_ui_anim_tick / 6 ) & 1u ) == 0u ) ? 0 : 1 ) ;
 int draw_x = center_x - ( 125 / 2 ) + 0 ;
 int draw_y = top_y + 0 + bob ;
 int i ;

 blit_title_overlay ( app , screen , draw_x , draw_y ) ;
 wait_blitter ( ) ;

 for ( i = 0 ; i < 5 ; ++ i ) {
 int sx = draw_x + ( ( sparkle_anchors [ i ] [ 0 ] * 125 ) / 220 ) ;
 int sy = draw_y + ( ( sparkle_anchors [ i ] [ 1 ] * 83 ) / 48 ) ;
 int wave = ( int ) ( ( ( g_ui_anim_tick / 2 ) + ( ULONG ) ( i * 5 ) ) & 15u ) ;
 int phase = 0 ;
 if ( wave < 3 ) {
 phase = wave + 1 ;
 } else if ( wave == 3 ) {
 phase = 2 ;
 } else if ( wave == 4 ) {
 phase = 1 ;
 }
 if ( phase > 0 ) {
 draw_title_star_sparkle ( screen , sx , sy , phase ) ;
 }
 }
 }

 static inline ULONG mix_key ( ULONG key , ULONG value ) {
 return ( key * 16777619UL ) ^ value ;
 }

 static inline ULONG modal_anim_key_for_state ( const RenderState * rs ) {
 if ( rs -> phase == GAME_PHASE_PLAYER_DYING ) {
 return 0 ;
 }
 if ( rs -> phase == GAME_PHASE_PLAYING && ! rs -> start_title_pending ) {
 return 0 ;
 }
 return ( ULONG ) ( ( g_ui_anim_tick / 6UL ) & 15UL ) ;
 }

 static DirtyRect modal_rect_for_state ( const RenderState * rs ) {
 DirtyRect r ;
 r . x = 0 ;
 r . y = 0 ;
 r . w = 0 ;
 r . h = 0 ;

 if ( rs -> phase == GAME_PHASE_ROUND_INTRO && rs -> start_title_pending ) {
 r . x = 60 ;
 r . y = 0 ;
 r . w = 228 ;
 r . h = 184 ;
 return r ;
 }

 switch ( rs -> phase ) {
 case GAME_PHASE_ROUND_INTRO :
 r . x = 44 ;
 r . y = 54 ;
 r . w = 232 ;
 r . h = 94 ;
 break ;
 case GAME_PHASE_PLAYER_DYING :
 r . x = 76 ;
 r . y = 78 ;
 r . w = 168 ;
 r . h = 40 ;
 break ;
 case GAME_PHASE_ROUND_CLEAR :
 r . x = 56 ;
 r . y = 70 ;
 r . w = 208 ;
 r . h = 74 ;
 break ;
 case GAME_PHASE_GAME_OVER :
 r . x = 52 ;
 r . y = 66 ;
 r . w = 216 ;
 r . h = 84 ;
 break ;
 case GAME_PHASE_PERK_CHOICE :
 r . x = 44 ;
 r . y = 54 ;
 r . w = 232 ;
 r . h = 94 ;
 break ;
 case GAME_PHASE_META_UPGRADE :
 r . x = 32 ;
 r . y = 54 ;
 r . w = 256 ;
 r . h = 94 ;
 break ;
 default :
 break ;
 }
 return r ;
 }

 static ULONG modal_key_for_state ( const RenderState * rs ) {
 ULONG key = 2166136261UL ;
 int i ;
 key = mix_key ( key , ( ULONG ) rs -> phase ) ;
 key = mix_key ( key , rs -> start_title_pending ? 1UL : 0UL ) ;
 key = mix_key ( key , ( ULONG ) rs -> round ) ;
 key = mix_key ( key , modal_anim_key_for_state ( rs ) ) ;

 if ( rs -> phase == GAME_PHASE_ROUND_INTRO && rs -> start_title_pending ) {
 return key ;
 }

 switch ( rs -> phase ) {
 case GAME_PHASE_ROUND_CLEAR :
 key = mix_key ( key , ( rs -> round_clear_bonus_score > 0 ) ? ( ULONG ) rs -> round_clear_bonus_score : 0UL ) ;
 break ;
 case GAME_PHASE_GAME_OVER :
 key = mix_key ( key , rs -> score > 99999999u ? 99999999u : rs -> score ) ;
 key = mix_key ( key , ( ULONG ) rs -> run_shards ) ;
 key = mix_key ( key , ( rs -> phase_timer_ticks > 0 ) ? 1UL : 0UL ) ;
 break ;
 case GAME_PHASE_PERK_CHOICE :
 key = mix_key ( key , ( ULONG ) rs -> perk_choice_count ) ;
 key = mix_key ( key , ( ULONG ) rs -> perk_choice_selected ) ;
 for ( i = 0 ; i < rs -> perk_choice_count && i < 3 ; ++ i ) {
 PerkType perk = rs -> perk_choices [ i ] ;
 key = mix_key ( key , ( ULONG ) perk ) ;
 key = mix_key ( key , ( ULONG ) rs -> perk_levels [ perk ] ) ;
 }
 break ;
 case GAME_PHASE_META_UPGRADE :
 key = mix_key ( key , ( ULONG ) rs -> meta_choice_count ) ;
 key = mix_key ( key , ( ULONG ) rs -> meta_choice_selected ) ;
 key = mix_key ( key , rs -> meta_shards ) ;
 for ( i = 0 ; i < rs -> meta_choice_count && i < 4 ; ++ i ) {
 key = mix_key ( key , ( ULONG ) rs -> meta_choices [ i ] ) ;
 key = mix_key ( key , ( ULONG ) rs -> meta_choice_costs [ i ] ) ;
 }
 break ;
 default :
 break ;
 }
 return key ;
 }

 static bool draw_gameplay_flash_borders ( UBYTE * screen , const RenderState * rs ) {
 bool drew = 0 ;
 int x ;
 int y ;
 if ( rs -> stage_modifier != STAGE_MOD_NONE &&
 rs -> stage_modifier_flash_ticks > 0 &&
 ( ( rs -> stage_modifier_flash_ticks / 3 ) & 1 ) == 0 ) {
 UBYTE color = stage_modifier_color ( rs -> stage_modifier ) ;
 for ( x = 2 ; x < 320 - 2 ; ++ x ) {
 put_pixel ( screen , x , 2 , color ) ;
 put_pixel ( screen , x , 189 , color ) ;
 }
 drew = 1 ;
 }
 if ( rs -> special_alignment_flash_ticks > 0 && ( ( rs -> special_alignment_flash_ticks / 3 ) & 1 ) == 0 ) {
 for ( x = 0 ; x < 320 ; ++ x ) {
 put_pixel ( screen , x , 0 , 8 ) ;
 put_pixel ( screen , x , 200 - 1 , 8 ) ;
 }
 for ( y = 0 ; y < 200 ; ++ y ) {
 put_pixel ( screen , 0 , y , 8 ) ;
 put_pixel ( screen , 320 - 1 , y , 8 ) ;
 }
 drew = 1 ;
 }
 if ( rs -> phase == GAME_PHASE_PLAYING && rs -> timer_danger_pulse_ticks > 0 ) {
 for ( x = 0 ; x < 320 ; ++ x ) {
 put_pixel ( screen , x , 0 , 23 ) ;
 put_pixel ( screen , x , ( 12 * 16 ) - 1 , 23 ) ;
 }
 for ( y = 0 ; y < ( 12 * 16 ) ; ++ y ) {
 put_pixel ( screen , 0 , y , 23 ) ;
 put_pixel ( screen , 320 - 1 , y , 23 ) ;
 }
 drew = 1 ;
 }
 return drew ;
 }

 static bool gameplay_flash_active ( const RenderState * rs ) {
 return ( rs -> stage_modifier != STAGE_MOD_NONE &&
 rs -> stage_modifier_flash_ticks > 0 &&
 ( ( rs -> stage_modifier_flash_ticks / 3 ) & 1 ) == 0 ) ||
 ( rs -> special_alignment_flash_ticks > 0 &&
 ( ( rs -> special_alignment_flash_ticks / 3 ) & 1 ) == 0 ) ||
 ( rs -> phase == GAME_PHASE_PLAYING && rs -> timer_danger_pulse_ticks > 0 ) ;
 }

 static bool overlay_active ( const RenderState * rs ) {
 return rs -> phase != GAME_PHASE_PLAYING || rs -> start_title_pending ;
 }

 static inline bool dirty_rect_equal ( const DirtyRect * a , const DirtyRect * b ) {
 return a -> x == b -> x &&
 a -> y == b -> y &&
 a -> w == b -> w &&
 a -> h == b -> h ;
 }

 static bool draw_overlay ( const AmigaApp * app , UBYTE * screen , const RenderState * rs ) {
 bool touched_gameplay ;

 if ( ! overlay_active ( rs ) ) {
 if ( ! gameplay_flash_active ( rs ) ) {
 return 0 ;
 }
 wait_blitter ( ) ;
 touched_gameplay = draw_gameplay_flash_borders ( screen , rs ) ;
 return touched_gameplay ;
 }

 wait_blitter ( ) ;
 if ( rs -> phase == GAME_PHASE_ROUND_INTRO && rs -> start_title_pending ) {
 draw_title_overlay_art ( app , screen , 320 / 2 , 10 ) ;
 draw_centered_text_outlined (
 screen ,
 0 ,
 320 - 1 ,
 84 ,
 "RETROFOUNDRY" ,
 ( ( ( ( g_ui_anim_tick / 14 ) & 1u ) == 0u ) ? 29 : 30 ) ,
 1 ) ;
 draw_opening_high_scores ( screen ) ;

 draw_panel ( screen , 70 , 148 , 249 , 178 , 14 ) ;
 draw_centered_text_outlined ( screen , 70 , 249 , 155 , "ROUND" , 30 , 1 ) ;
 draw_number_outlined ( screen , 164 , 155 , ( uint32_t ) rs -> round , 2 , 31 , 1 ) ;
 draw_centered_text_outlined ( screen , 70 , 249 , 163 , "FIRE START" , 31 , 1 ) ;
 draw_continue_chevrons ( screen , 70 + ( ( 249 - 70 ) / 2 ) - 7 , 171 , 30 , 14 ) ;
 return 1 ;
 }

 if ( rs -> phase == GAME_PHASE_ROUND_INTRO ) {
 const int panel_x0 = 48 ;
 const int panel_x1 = 271 ;
 const int panel_y0 = 58 ;
 const int panel_y1 = 143 ;
 const int panel_w = panel_x1 - panel_x0 + 1 ;
 const int round_w = number_width ( ( uint32_t ) rs -> round , 2 ) ;
 const int round_x = panel_x0 + ( ( panel_w - round_w ) / 2 ) ;
 const int lane_card_x = panel_x0 + 14 ;
 const int lane_card_y = panel_y0 + 24 ;
 const int lane_card_w = panel_w - 28 ;
 const int lane_card_h = 44 ;
 draw_panel ( screen , panel_x0 , panel_y0 , panel_x1 , panel_y1 , 14 ) ;
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , panel_y0 + 8 , "ROUND" , 31 , 1 ) ;
 draw_number_outlined ( screen , round_x , panel_y0 + 16 , ( uint32_t ) rs -> round , 2 , 31 , 1 ) ;
 draw_card_frame ( screen , lane_card_x , lane_card_y , lane_card_w , lane_card_h , 14 , 1 ) ;
 draw_round_intro_visual_lane (
 app ,
 screen ,
 rs ,
 lane_card_x + 6 ,
 lane_card_y + 4 ,
 lane_card_w - 12 ,
 lane_card_h - 8 ) ;
 draw_continue_chevrons ( screen , panel_x0 + ( panel_w / 2 ) - 7 , panel_y1 - 11 , 30 , 14 ) ;
 return 1 ;
 }

 if ( rs -> phase == GAME_PHASE_ROUND_CLEAR ) {
 const int panel_x0 = 60 ;
 const int panel_x1 = 259 ;
 const int panel_y0 = 74 ;
 const int panel_y1 = 137 ;
 const uint32_t bonus = ( rs -> round_clear_bonus_score > 0 ) ? ( uint32_t ) rs -> round_clear_bonus_score : 0u ;
 char bonus_text [ 16 ] ;
 int digits [ 10 ] ;
 int count = 0 ;
 int i ;
 uint32_t v = bonus ;

 do {
 digits [ count ++ ] = ( int ) ( v % 10u ) ;
 v /= 10u ;
 } while ( v > 0u && count < 10 ) ;
 bonus_text [ 0 ] = '+' ;
 for ( i = count - 1 ; i >= 0 ; -- i ) {
 bonus_text [ count - i ] = ( char ) ( '0' + digits [ i ] ) ;
 }
 bonus_text [ count + 1 ] = '\0' ;

 draw_panel ( screen , panel_x0 , panel_y0 , panel_x1 , panel_y1 , 11 ) ;
 draw_rect ( screen , panel_x0 + 5 , panel_y0 + 5 , panel_x1 - 5 , panel_y0 + 20 , 1 ) ;
 draw_centered_text_scaled_outlined ( screen , panel_x0 , panel_x1 , panel_y0 + 8 , "ROUND CLEAR" , 31 , 1 , 2 ) ;
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , panel_y0 + 28 , "TIME BONUS" , 30 , 1 ) ;
 draw_centered_text_scaled_outlined ( screen , panel_x0 , panel_x1 , panel_y0 + 39 , bonus_text , 31 , 1 , 2 ) ;
 draw_continue_chevrons ( screen , panel_x0 + ( ( panel_x1 - panel_x0 ) / 2 ) - 7 , panel_y1 - 10 , 31 , 11 ) ;
 return 1 ;
 }

 if ( rs -> phase == GAME_PHASE_GAME_OVER ) {
 const int panel_x0 = 56 ;
 const int panel_x1 = 263 ;
 const int panel_y0 = 70 ;
 const int panel_y1 = 145 ;
 uint32_t score = rs -> score > 99999999u ? 99999999u : rs -> score ;
 int score_scale = 3 ;
 int score_w = number_width_scaled ( score , 5 , score_scale ) ;
 int score_x = panel_x0 + ( ( ( panel_x1 - panel_x0 + 1 ) - score_w ) / 2 ) ;
 UBYTE score_color = ( ( ( ( g_ui_anim_tick / 4 ) & 1u ) == 0u ) ? 31 : 30 ) ;
 draw_panel ( screen , panel_x0 , panel_y0 , panel_x1 , panel_y1 , 21 ) ;
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , panel_y0 + 7 , "GAME OVER" , 31 , 1 ) ;
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , panel_y0 + 16 , "FINAL SCORE" , 30 , 1 ) ;
 draw_number_scaled_outlined ( screen , score_x , panel_y0 + 25 , score , 5 , score_color , 1 , score_scale ) ;
 if ( rs -> run_shards > 0 ) {
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , panel_y1 - 16 , "BANK SHARDS" , 8 , 1 ) ;
 draw_number_right ( screen , panel_x1 - 20 , panel_y1 - 16 , ( uint32_t ) rs -> run_shards , 1 , 8 ) ;
 }
 if ( rs -> phase_timer_ticks > 0 ) {
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , panel_y1 - 9 , "PLEASE WAIT" , 30 , 1 ) ;
 } else {
 draw_centered_text_outlined ( screen , panel_x0 , panel_x1 , panel_y1 - 9 , "FIRE RESTART" , 31 , 1 ) ;
 }
 return 1 ;
 }

 if ( rs -> phase == GAME_PHASE_PLAYER_DYING ) {
 draw_panel ( screen , 80 , 82 , 239 , 112 , 23 ) ;
 draw_centered_text_outlined ( screen , 80 , 239 , 92 , "DIED" , 31 , 1 ) ;
 draw_centered_text_outlined ( screen , 80 , 239 , 100 , "RESPAWNING" , 30 , 1 ) ;
 return 1 ;
 }

 if ( rs -> phase == GAME_PHASE_PERK_CHOICE ) {
 draw_perk_choice_panel ( app , screen , rs ) ;
 return 1 ;
 }

 if ( rs -> phase == GAME_PHASE_META_UPGRADE ) {
 draw_meta_choice_panel ( app , screen , rs ) ;
 return 1 ;
 }

 return 1 ;
 }

 static void draw_boot_screen ( UBYTE * screen ) {
 wait_blitter ( ) ;
 draw_rect ( screen , 0 , 0 , 320 - 1 , 200 - 1 , 1 ) ;
 draw_panel ( screen , 72 , 72 , 247 , 122 , 21 ) ;
 draw_centered_text ( screen , 72 , 247 , 88 , "ICEPANIC" , 31 ) ;
 draw_centered_text ( screen , 72 , 247 , 104 , "LOADING" , 30 ) ;
 }

 static void render_frame ( AmigaApp * app , int buf , UBYTE * screen , const RenderState * rs ) {
 RendererState * rr = & app -> renderer ;
 bool modal_active = overlay_active ( rs ) ;
 ModalCache * modal = & rr -> modal [ buf ] ;
 DirtyRect next_modal_rect ;
 ULONG next_modal_key = 0 ;
 bool restore_backdrop = 1 ;
 bool overlay_touched ;
 bool modal_was_active = ( modal -> valid && modal -> active ) ? 1 : 0 ;
 bool translucent_modal = ( rs -> phase == GAME_PHASE_ROUND_INTRO && rs -> start_title_pending ) ? 1 : 0 ;
 HudSnapshot hud ;

 prepare_enemy_hardware_sprites ( app , rr , buf , rs , modal_active ) ;

 if ( modal_active ) {
 next_modal_rect = modal_rect_for_state ( rs ) ;
 next_modal_key = modal_key_for_state ( rs ) ;
 if ( modal -> valid && modal -> active && modal -> key == next_modal_key ) {
 return ;
 }

 if ( ! translucent_modal && modal -> valid && modal -> active && dirty_rect_equal ( & modal -> rect , & next_modal_rect ) ) {
 restore_backdrop = 0 ;
 } else if ( modal -> valid && modal -> active ) {
 mark_dirty_rect ( rr , buf , & modal -> rect ) ;
 }
 } else if ( modal -> valid && modal -> active ) {
 mark_dirty_rect ( rr , buf , & modal -> rect ) ;
 modal -> active = 0 ;
 modal -> valid = 0 ;
 }

 if ( ! modal_active || restore_backdrop ) {
 if ( modal_active || modal_was_active || ! rr -> prev_valid [ buf ] ) {
 restore_dynamic_caches ( rr , buf ) ;
 }
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x220 ) ; } while ( 0 ) ;
 restore_previous_dynamic ( app , rr , buf , screen ) ;
 update_static_dirty ( rr , rs , buf ) ;
 prepare_dynamic_actors ( rr , buf , rs ) ;
 render_dirty_static ( app , screen , buf , rs ) ;
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x000 ) ; } while ( 0 ) ;
 }

 hud = make_hud_snapshot ( rs ) ;

 if ( modal_active ) {
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x088 ) ; } while ( 0 ) ;
 if ( ! rr -> prev_hud_valid [ buf ] ) {
 draw_hud ( screen , rs ) ;
 rr -> prev_hud [ buf ] = hud ;
 rr -> prev_hud_valid [ buf ] = 1 ;
 } else if ( ! hud_snapshot_equal ( & rr -> prev_hud [ buf ] , & hud ) ) {
 draw_hud_delta ( screen , rs , & rr -> prev_hud [ buf ] , & hud ) ;
 rr -> prev_hud [ buf ] = hud ;
 }
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x808 ) ; } while ( 0 ) ;
 ( void ) draw_overlay ( app , screen , rs ) ;
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x000 ) ; } while ( 0 ) ;
 modal -> rect = next_modal_rect ;
 modal -> key = next_modal_key ;
 modal -> active = 1 ;
 modal -> valid = 1 ;
 return ;
 }

 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x080 ) ; } while ( 0 ) ;
 draw_dynamic_actors ( app , rr , buf , screen , rs ) ;
 draw_impact_fx ( app , rr , buf , screen , rs ) ;



 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x088 ) ; } while ( 0 ) ;
 if ( ! rr -> prev_hud_valid [ buf ] ) {
 draw_hud ( screen , rs ) ;
 rr -> prev_hud [ buf ] = hud ;
 rr -> prev_hud_valid [ buf ] = 1 ;
 } else if ( ! hud_snapshot_equal ( & rr -> prev_hud [ buf ] , & hud ) ) {
 draw_hud_delta ( screen , rs , & rr -> prev_hud [ buf ] , & hud ) ;
 rr -> prev_hud [ buf ] = hud ;
 }
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x808 ) ; } while ( 0 ) ;
 overlay_touched = draw_overlay ( app , screen , rs ) ;
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x000 ) ; } while ( 0 ) ;
 if ( overlay_touched ) {
 mark_gameplay_border_dirty ( rr , buf ) ;
 }
 }

 static BOOL quit_combo_down ( void ) {
 volatile UBYTE * ciaa_pra = ( volatile UBYTE * ) 0xBFE001 ;
 UBYTE pra = * ciaa_pra ;
 return ( ( pra & ( ( 1 << 6 ) | ( 1 << 7 ) ) ) == 0 ) ;
 }

 static BOOL joystick_fire_down ( void ) {
 volatile UBYTE * ciaa_pra = ( volatile UBYTE * ) 0xBFE001 ;
 UBYTE pra = * ciaa_pra ;
 return ( ( pra & ( 1 << 7 ) ) == 0 ) ;
 }

 static UBYTE dirs_from_joydat ( UWORD joy ) {
 UBYTE dirs = 0 ;
 BOOL left = ( ( joy & 0x0200u ) != 0 ) ;
 BOOL right = ( ( joy & 0x0002u ) != 0 ) ;
 if ( left ) {
 dirs |= 1u ;
 }
 if ( right ) {
 dirs |= 2u ;
 }
 if ( ( ( ( joy & 0x0100u ) != 0 ) ^ left ) != 0 ) {
 dirs |= 4u ;
 }
 if ( ( ( ( joy & 0x0001u ) != 0 ) ^ right ) != 0 ) {
 dirs |= 8u ;
 }
 return dirs ;
 }

 static InputState read_input ( UBYTE * last_dirs , UBYTE * last_fire ) {
 InputState in ;
 UBYTE dirs = 0 ;
 UBYTE fire ;
 __asm_memset ( & in , 0 , sizeof ( in ) ) ;

 dirs = dirs_from_joydat ( ( ( volatile struct Custom * ) 0xDFF000 ) -> joy1dat ) ;

 if ( ( dirs & 1u ) != 0 ) {
 in . left = 1 ;
 }
 if ( ( dirs & 2u ) != 0 ) {
 in . right = 1 ;
 }
 if ( ( dirs & 4u ) != 0 ) {
 in . up = 1 ;
 }
 if ( ( dirs & 8u ) != 0 ) {
 in . down = 1 ;
 }

 in . newest_press = DIR_NONE ;
 if ( ( dirs & 1u ) != 0 && ( * last_dirs & 1u ) == 0 ) {
 in . newest_press = DIR_LEFT ;
 }
 if ( ( dirs & 2u ) != 0 && ( * last_dirs & 2u ) == 0 ) {
 in . newest_press = DIR_RIGHT ;
 }
 if ( ( dirs & 4u ) != 0 && ( * last_dirs & 4u ) == 0 ) {
 in . newest_press = DIR_UP ;
 }
 if ( ( dirs & 8u ) != 0 && ( * last_dirs & 8u ) == 0 ) {
 in . newest_press = DIR_DOWN ;
 }
 * last_dirs = dirs ;

 fire = joystick_fire_down ( ) ? 1u : 0u ;
 in . start = 0 ;
 in . fire_pressed = ( fire != 0 && * last_fire == 0 ) ;
 in . fire_released = ( fire == 0 && * last_fire != 0 ) ;
 * last_fire = fire ;
 return in ;
 }

 static BOOL build_copper_list ( CopperState * cs ) {
 UWORD * p ;
 UWORD * cop ;
 WORD i ;
 ULONG words = 192 ;
 cop = ( UWORD * ) __AllocMem ( SysBase , ( words * sizeof ( UWORD ) ) , ( 0x00000002UL | 0x00010000UL ) ) ;
 if ( ! cop ) {
 return 0 ;
 }
 cs -> list = cop ;
 cs -> bytes = words * sizeof ( UWORD ) ;
 p = cop ;

 * p ++ = 0x08E ; * p ++ = 0x2C81 ;
 * p ++ = 0x090 ; * p ++ = 0xF4C1 ;
 * p ++ = 0x092 ; * p ++ = 0x0038 ;
 * p ++ = 0x094 ; * p ++ = 0x00D0 ;
 * p ++ = 0x100 ; * p ++ = ( UWORD ) ( 0x0200 | ( 5 << 12 ) ) ;
 * p ++ = 0x102 ; * p ++ = 0x0000 ;
 * p ++ = 0x104 ;



 * p ++ = 0x0000 ;

 * p ++ = 0x108 ; * p ++ = ( ( 320 / 8 ) * 5 ) - ( 320 / 8 ) ;
 * p ++ = 0x10A ; * p ++ = ( ( 320 / 8 ) * 5 ) - ( 320 / 8 ) ;

 for ( i = 0 ; i < 5 ; ++ i ) {
 * p ++ = ( UWORD ) ( 0x0E0 + ( i * 4 ) ) ;
 cs -> bpl_hi [ i ] = p ;
 * p ++ = 0 ;
 * p ++ = ( UWORD ) ( 0x0E2 + ( i * 4 ) ) ;
 cs -> bpl_lo [ i ] = p ;
 * p ++ = 0 ;
 }

 for ( i = 0 ; i < 8 ; ++ i ) {
 * p ++ = ( UWORD ) ( 0x120 + ( i * 4 ) ) ;
 cs -> spr_hi [ i ] = p ;
 * p ++ = 0 ;
 * p ++ = ( UWORD ) ( 0x122 + ( i * 4 ) ) ;
 cs -> spr_lo [ i ] = p ;
 * p ++ = 0 ;
 }

 for ( i = 0 ; i < 32 ; ++ i ) {
 * p ++ = ( UWORD ) ( 0x180 + ( ( i ) << 1 ) ) ;



 * p ++ = g_amiga_palette [ i ] ;

 }

 * p ++ = ( UWORD ) ( ( 0xF4 << 8 ) | 0x07 ) ;
 * p ++ = 0xFFFE ;
 * p ++ = 0x100 ;
 * p ++ = 0x0000 ;
 * p ++ = 0x180 ;
 * p ++ = 0x0000 ;
 * p ++ = 0xFFFF ;
 * p ++ = 0xFFFE ;

 return 1 ;
 }

 static void patch_copper_bplptrs ( CopperState * cs , UBYTE * screen ) {
 WORD i ;
 for ( i = 0 ; i < 5 ; ++ i ) {
 ULONG addr = ( ULONG ) screen + ( ( ULONG ) i * ( 320 / 8 ) ) ;
 * ( cs -> bpl_hi [ i ] ) = ( UWORD ) ( addr >> 16 ) ;
 * ( cs -> bpl_lo [ i ] ) = ( UWORD ) ( addr & 0xFFFF ) ;
 }
 }

 static void patch_copper_spriteptrs ( CopperState * cs , UBYTE * bank ) {
 WORD i ;
 for ( i = 0 ; i < 8 ; ++ i ) {
 ULONG addr = ( ULONG ) bank + ( ( ULONG ) i * ( ( ULONG ) ( 2 + ( 16 * 2 ) + 2 ) * sizeof ( UWORD ) ) ) ;
 * ( cs -> spr_hi [ i ] ) = ( UWORD ) ( addr >> 16 ) ;
 * ( cs -> spr_lo [ i ] ) = ( UWORD ) ( addr & 0xFFFF ) ;
 }
 }

 static BOOL init_display ( void ) {
 g_old_view = ( ( struct GfxBase * ) GfxBase ) -> ActiView ;
 __LoadView ( GfxBase , ( ( ( void * ) 0 ) ) ) ;
 __WaitTOF ( GfxBase ) ;
 __WaitTOF ( GfxBase ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> dmacon = 0x01FF ;
 return ( ( ( ( struct GfxBase * ) GfxBase ) -> DisplayFlags & 4 ) == 4 ) ;
 }

 static void reset_display ( void ) {
 wait_blitter ( ) ;
 __LoadView ( GfxBase , ( g_old_view ) ) ;
 __WaitTOF ( GfxBase ) ;
 __WaitTOF ( GfxBase ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> cop1lc = ( ULONG ) ( ( struct GfxBase * ) GfxBase ) -> copinit ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> copjmp1 = 0 ;
 __RethinkDisplay ( IntuitionBase ) ;
 g_old_view = 0 ;
 }

 static BOOL app_alloc ( AmigaApp * app ) {
 __asm_memset ( app , 0 , sizeof ( * app ) ) ;
 app -> screen [ 0 ] = ( UBYTE * ) __AllocMem ( SysBase , ( ( ( ULONG ) ( ( 320 / 8 ) * 5 ) * 200 ) ) , ( 0x00000002UL | 0x00010000UL ) ) ;
 app -> screen [ 1 ] = ( UBYTE * ) __AllocMem ( SysBase , ( ( ( ULONG ) ( ( 320 / 8 ) * 5 ) * 200 ) ) , ( 0x00000002UL | 0x00010000UL ) ) ;
 app -> chip_tiles = ( UBYTE * ) __AllocMem ( SysBase , ( ( ( ULONG ) 33 * 160 ) ) , ( 0x00000002UL ) ) ;
 app -> chip_tile_masks = ( UBYTE * ) __AllocMem ( SysBase , ( ( ( ULONG ) 33 * 160 ) ) , ( 0x00000002UL ) ) ;
 app -> chip_bobs = ( UBYTE * ) __AllocMem ( SysBase , ( ( ( ULONG ) 33 * 320 ) ) , ( 0x00000002UL ) ) ;
 app -> chip_masks = ( UBYTE * ) __AllocMem ( SysBase , ( ( ( ULONG ) 33 * 320 ) ) , ( 0x00000002UL ) ) ;
 app -> chip_title = ( UBYTE * ) __AllocMem ( SysBase , ( ( ( ULONG ) 7470 ) ) , ( 0x00000002UL ) ) ;
 app -> chip_title_mask = ( UBYTE * ) __AllocMem ( SysBase , ( ( ( ULONG ) 7470 ) ) , ( 0x00000002UL ) ) ;
 app -> hw_sprite_bank [ 0 ] = ( UBYTE * ) __AllocMem ( SysBase , ( ( 8 * ( ( ULONG ) ( 2 + ( 16 * 2 ) + 2 ) * sizeof ( UWORD ) ) ) ) , ( 0x00000002UL | 0x00010000UL ) ) ;
 app -> hw_sprite_bank [ 1 ] = ( UBYTE * ) __AllocMem ( SysBase , ( ( 8 * ( ( ULONG ) ( 2 + ( 16 * 2 ) + 2 ) * sizeof ( UWORD ) ) ) ) , ( 0x00000002UL | 0x00010000UL ) ) ;
 if ( ! app -> screen [ 0 ] || ! app -> screen [ 1 ] || ! app -> chip_tiles || ! app -> chip_tile_masks ||
 ! app -> chip_bobs || ! app -> chip_masks || ! app -> chip_title || ! app -> chip_title_mask ||
 ! app -> hw_sprite_bank [ 0 ] || ! app -> hw_sprite_bank [ 1 ] ) {
 return 0 ;
 }
 __CopyMem ( SysBase , ( ( const void * ) g_amiga_tile_data ) , ( app -> chip_tiles ) , ( ( ( ULONG ) 33 * 160 ) ) ) ;
 __CopyMem ( SysBase , ( ( const void * ) g_amiga_tile_mask ) , ( app -> chip_tile_masks ) , ( ( ( ULONG ) 33 * 160 ) ) ) ;
 __CopyMem ( SysBase , ( ( const void * ) g_amiga_bob_data ) , ( app -> chip_bobs ) , ( ( ( ULONG ) 33 * 320 ) ) ) ;
 __CopyMem ( SysBase , ( ( const void * ) g_amiga_bob_mask ) , ( app -> chip_masks ) , ( ( ( ULONG ) 33 * 320 ) ) ) ;
 __CopyMem ( SysBase , ( ( const void * ) g_amiga_title_overlay_data ) , ( app -> chip_title ) , ( ( ( ULONG ) 7470 ) ) ) ;
 __CopyMem ( SysBase , ( ( const void * ) g_amiga_title_overlay_mask ) , ( app -> chip_title_mask ) , ( ( ( ULONG ) 7470 ) ) ) ;
 build_hw_enemy_sprite_lines ( app ) ;
 clear_hw_sprite_bank ( app -> hw_sprite_bank [ 0 ] ) ;
 clear_hw_sprite_bank ( app -> hw_sprite_bank [ 1 ] ) ;
 ( void ) sfx_init ( & app -> sfx ) ;
 if ( ! build_copper_list ( & app -> copper ) ) {
 return 0 ;
 }
 mark_all_dirty ( & app -> renderer , 0 ) ;
 mark_all_dirty ( & app -> renderer , 1 ) ;
 return 1 ;
 }

 static void app_free ( AmigaApp * app ) {
 sfx_shutdown ( & app -> sfx ) ;
 if ( app -> copper . list ) {
 __FreeMem ( SysBase , ( app -> copper . list ) , ( app -> copper . bytes ) ) ;
 app -> copper . list = 0 ;
 }
 if ( app -> chip_masks ) {
 __FreeMem ( SysBase , ( app -> chip_masks ) , ( ( ( ULONG ) 33 * 320 ) ) ) ;
 app -> chip_masks = 0 ;
 }
 if ( app -> chip_title_mask ) {
 __FreeMem ( SysBase , ( app -> chip_title_mask ) , ( ( ( ULONG ) 7470 ) ) ) ;
 app -> chip_title_mask = 0 ;
 }
 if ( app -> chip_title ) {
 __FreeMem ( SysBase , ( app -> chip_title ) , ( ( ( ULONG ) 7470 ) ) ) ;
 app -> chip_title = 0 ;
 }
 if ( app -> chip_tile_masks ) {
 __FreeMem ( SysBase , ( app -> chip_tile_masks ) , ( ( ( ULONG ) 33 * 160 ) ) ) ;
 app -> chip_tile_masks = 0 ;
 }
 if ( app -> hw_sprite_bank [ 0 ] ) {
 __FreeMem ( SysBase , ( app -> hw_sprite_bank [ 0 ] ) , ( ( 8 * ( ( ULONG ) ( 2 + ( 16 * 2 ) + 2 ) * sizeof ( UWORD ) ) ) ) ) ;
 app -> hw_sprite_bank [ 0 ] = 0 ;
 }
 if ( app -> hw_sprite_bank [ 1 ] ) {
 __FreeMem ( SysBase , ( app -> hw_sprite_bank [ 1 ] ) , ( ( 8 * ( ( ULONG ) ( 2 + ( 16 * 2 ) + 2 ) * sizeof ( UWORD ) ) ) ) ) ;
 app -> hw_sprite_bank [ 1 ] = 0 ;
 }
 if ( app -> chip_bobs ) {
 __FreeMem ( SysBase , ( app -> chip_bobs ) , ( ( ( ULONG ) 33 * 320 ) ) ) ;
 app -> chip_bobs = 0 ;
 }
 if ( app -> chip_tiles ) {
 __FreeMem ( SysBase , ( app -> chip_tiles ) , ( ( ( ULONG ) 33 * 160 ) ) ) ;
 app -> chip_tiles = 0 ;
 }
 if ( app -> screen [ 0 ] ) {
 __FreeMem ( SysBase , ( app -> screen [ 0 ] ) , ( ( ( ULONG ) ( ( 320 / 8 ) * 5 ) * 200 ) ) ) ;
 app -> screen [ 0 ] = 0 ;
 }
 if ( app -> screen [ 1 ] ) {
 __FreeMem ( SysBase , ( app -> screen [ 1 ] ) , ( ( ( ULONG ) ( ( 320 / 8 ) * 5 ) * 200 ) ) ) ;
 app -> screen [ 1 ] = 0 ;
 }
 }

 static void app_shutdown ( AmigaApp * app ) {
 wait_blitter ( ) ;
 sfx_shutdown ( & app -> sfx ) ;
 if ( app -> owns_blitter ) {
 __DisownBlitter ( GfxBase ) ;
 app -> owns_blitter = 0 ;
 }
 reset_display ( ) ;
 if ( app -> wb_was_open ) {
 __OpenWorkBench ( IntuitionBase ) ;
 app -> wb_was_open = 0 ;
 }
 app_free ( app ) ;
 }

 static uint32_t mix_seed ( uint32_t seed , uint32_t value ) {
 seed ^= value + 0x9e3779b9u + ( seed << 6 ) + ( seed >> 2 ) ;
 seed ^= seed >> 16 ;
 seed *= 0x7feb352du ;
 seed ^= seed >> 15 ;
 seed *= 0x846ca68bu ;
 seed ^= seed >> 16 ;
 return seed ;
 }

 static uint32_t startup_seed ( void ) {
 struct DateStamp ds ;
 uint32_t seed = 0xa53c9655u ;
 volatile UBYTE * ciaa_pra = ( volatile UBYTE * ) 0xBFE001 ;
 volatile UBYTE * ciaa_talo = ( volatile UBYTE * ) 0xBFE401 ;
 volatile UBYTE * ciaa_tahi = ( volatile UBYTE * ) 0xBFE501 ;
 volatile UBYTE * ciab_tblo = ( volatile UBYTE * ) 0xBFD600 ;
 volatile UBYTE * ciab_tbhi = ( volatile UBYTE * ) 0xBFD700 ;

 __DateStamp ( DOSBase , ( & ds ) ) ;
 seed = mix_seed ( seed , ( uint32_t ) ds . ds_Days ) ;
 seed = mix_seed ( seed , ( uint32_t ) ds . ds_Minute ) ;
 seed = mix_seed ( seed , ( uint32_t ) ds . ds_Tick ) ;
 seed = mix_seed ( seed , ( ( uint32_t ) ( ( volatile struct Custom * ) 0xDFF000 ) -> vposr << 16 ) | ( uint32_t ) ( ( volatile struct Custom * ) 0xDFF000 ) -> vhposr ) ;
 seed = mix_seed ( seed , ( ( uint32_t ) ( ( volatile struct Custom * ) 0xDFF000 ) -> joy1dat << 16 ) | ( uint32_t ) ( ( volatile struct Custom * ) 0xDFF000 ) -> pot0dat ) ;
 seed = mix_seed ( seed , ( ( uint32_t ) ( ( volatile struct Custom * ) 0xDFF000 ) -> pot1dat << 16 ) | ( uint32_t ) * ciaa_pra ) ;
 seed = mix_seed ( seed , ( ( uint32_t ) * ciaa_tahi << 24 ) |
 ( ( uint32_t ) * ciaa_talo << 16 ) |
 ( ( uint32_t ) * ciab_tbhi << 8 ) |
 ( uint32_t ) * ciab_tblo ) ;
 if ( seed == 0 ) {
 seed = 0x12345678u ;
 }
 return seed ;
 }

 int main ( void ) {
 AmigaApp * app = & g_app ;
 GameState * game = & g_game ;
 RenderState * rs = ( RenderState * ) game ;
 UBYTE last_dirs = 0 ;
 UBYTE last_fire = 0 ;
 WORD quit_hold = 0 ;
 WORD draw_idx = 1 ;
 WORD render_divider = 0 ;
 GamePhase last_rendered_phase = GAME_PHASE_ROUND_INTRO ;
 BOOL title_jingle_queued = 0 ;
 int exit_code = 0 ;
 FrameSync sync ;

 __asm_memset ( app , 0 , sizeof ( * app ) ) ;

 GfxBase = ( struct GfxBase * ) __OpenLibrary ( SysBase , ( "graphics.library" ) , ( 0 ) ) ;
 if ( ! GfxBase ) {
 return 20 ;
 }
 IntuitionBase = ( struct IntuitionBase * ) __OpenLibrary ( SysBase , ( "intuition.library" ) , ( 0 ) ) ;
 if ( ! IntuitionBase ) {
 __CloseLibrary ( SysBase , ( ( struct Library * ) GfxBase ) ) ;
 return 20 ;
 }

 __SetTaskPri ( SysBase , ( __FindTask ( SysBase , ( ( ( void * ) 0 ) ) ) ) , ( 20 ) ) ;
 app -> wb_was_open = __CloseWorkBench ( IntuitionBase ) ? 1 : 0 ;

 if ( ! init_display ( ) ) {
 reset_display ( ) ;
 if ( app -> wb_was_open ) {
 __OpenWorkBench ( IntuitionBase ) ;
 }
 __CloseLibrary ( SysBase , ( ( struct Library * ) IntuitionBase ) ) ;
 __CloseLibrary ( SysBase , ( ( struct Library * ) GfxBase ) ) ;
 return 20 ;
 }

 __OwnBlitter ( GfxBase ) ;
 app -> owns_blitter = 1 ;

 if ( ! app_alloc ( app ) ) {
 app_shutdown ( app ) ;
 __CloseLibrary ( SysBase , ( ( struct Library * ) IntuitionBase ) ) ;
 __CloseLibrary ( SysBase , ( ( struct Library * ) GfxBase ) ) ;
 return 20 ;
 }

 patch_copper_spriteptrs ( & app -> copper , app -> hw_sprite_bank [ 0 ] ) ;
 draw_boot_screen ( app -> screen [ 0 ] ) ;
 wait_blitter ( ) ;
 patch_copper_bplptrs ( & app -> copper , app -> screen [ 0 ] ) ;
 patch_copper_spriteptrs ( & app -> copper , app -> hw_sprite_bank [ 0 ] ) ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> cop1lc = ( ULONG ) app -> copper . list ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> copjmp1 = 0 ;
 ( ( volatile struct Custom * ) 0xDFF000 ) -> dmacon = ( 0x8000 | 0x0200 | 0x0100 | 0x0080 | 0x0040 ) ;

 game_init ( game , startup_seed ( ) ) ;
 {
 uint32_t boot_events = game_consume_events ( game ) ;
 if ( game -> phase == GAME_PHASE_ROUND_INTRO && game -> start_title_pending ) {
 if ( ( boot_events & GAME_EVENT_ROUND_START ) != 0u ) {
 sfx_queue_title_start_jingle ( & app -> sfx ) ;
 title_jingle_queued = 1 ;
 }
 boot_events &= ( uint32_t ) ~ GAME_EVENT_ROUND_START ;
 }
 sfx_dispatch_events ( & app -> sfx , boot_events ) ;
 sfx_update ( & app -> sfx ) ;
 }
 mark_core_dirty_static ( & app -> renderer , game ) ;
 game_clear_dirty_static ( game ) ;
 render_frame ( app , 0 , app -> screen [ 0 ] , rs ) ;
 render_frame ( app , 1 , app -> screen [ 1 ] , rs ) ;
 last_rendered_phase = rs -> phase ;
 wait_blitter ( ) ;
 patch_copper_bplptrs ( & app -> copper , app -> screen [ 0 ] ) ;
 patch_copper_spriteptrs ( & app -> copper , app -> hw_sprite_bank [ 0 ] ) ;
 wait_frame_boundary_hw ( ) ;

 while ( 1 ) {
 frame_sync_begin ( & sync ) ;
 InputState input = read_input ( & last_dirs , & last_fire ) ;
 if ( quit_combo_down ( ) ) {
 ++ quit_hold ;
 if ( quit_hold >= 50 ) {
 break ;
 }
 } else {
 quit_hold = 0 ;
 }
 if ( input . fire_released && game -> phase != GAME_PHASE_PLAYING ) {
 sfx_queue_title_confirm_chirp ( & app -> sfx ) ;
 }
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x400 ) ; } while ( 0 ) ;
 game_step ( game , & input ) ;
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x000 ) ; } while ( 0 ) ;
 frame_sync_sample ( & sync ) ;
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x840 ) ; } while ( 0 ) ;
 {
 uint32_t events = game_consume_events ( game ) ;
 uint32_t audio_events = events ;
 if ( game -> phase == GAME_PHASE_ROUND_INTRO && game -> start_title_pending ) {
 if ( ( audio_events & GAME_EVENT_ROUND_START ) != 0u && ! title_jingle_queued ) {
 sfx_queue_title_start_jingle ( & app -> sfx ) ;
 title_jingle_queued = 1 ;
 }
 audio_events &= ( uint32_t ) ~ GAME_EVENT_ROUND_START ;
 } else {
 title_jingle_queued = 0 ;
 }
 sfx_dispatch_events ( & app -> sfx , audio_events ) ;
 sfx_update ( & app -> sfx ) ;
 }
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x000 ) ; } while ( 0 ) ;
 frame_sync_sample ( & sync ) ;
 mark_core_dirty_static ( & app -> renderer , game ) ;
 game_clear_dirty_static ( game ) ;

 ++ render_divider ;
 if ( render_divider >= 1 || rs -> phase != last_rendered_phase ) {
 render_divider = 0 ;
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x040 ) ; } while ( 0 ) ;
 render_frame ( app , draw_idx , app -> screen [ draw_idx ] , rs ) ;
 last_rendered_phase = rs -> phase ;
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x004 ) ; } while ( 0 ) ;
 wait_blitter ( ) ;
 patch_copper_bplptrs ( & app -> copper , app -> screen [ draw_idx ] ) ;
 patch_copper_spriteptrs ( & app -> copper , app -> hw_sprite_bank [ draw_idx ] ) ;
 draw_idx ^= 1 ;
 frame_sync_sample ( & sync ) ;
 }
 do { ( ( volatile struct Custom * ) 0xDFF000 ) -> color [ 0 ] = ( UWORD ) ( 0x000 ) ; } while ( 0 ) ;
 frame_sync_finish ( & sync ) ;
 g_ui_anim_tick = ( g_ui_anim_tick + 1u ) & 0x7fffffffu ;
 }

 app_shutdown ( app ) ;
 __CloseLibrary ( SysBase , ( ( struct Library * ) IntuitionBase ) ) ;
 __CloseLibrary ( SysBase , ( ( struct Library * ) GfxBase ) ) ;
 return exit_code ;
 }
 
