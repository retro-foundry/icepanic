```txt
# agents.txt
# Implementation specification for an original Pengo-style arcade game
# Target: single-screen tile-based action puzzle game, suitable for Amiga-style design

PROJECT_NAME: Original Pengo-Style Arcade Game
PROJECT_TYPE: Grid-based single-screen arcade action game
PRIMARY_GOAL: Implement a clean, original block-pushing arcade game with sliding blocks, enemy crush logic, bonus items, round progression, and tight deterministic controls.

## 1. CORE DESIGN SUMMARY

The game is a fixed single-screen tilemap.
The player moves on a strict grid.
The main interaction is pushing blocks so they slide across the map.
Sliding blocks can crush enemies.
The round ends when all enemies are defeated.

The game should feel:
- fast
- readable
- deterministic
- arcade-like
- suitable for 320x200-era presentation

Recommended default grid:
- 20 columns
- 12 rows
- 16x16 tiles

This fits a 320x192 gameplay area with room for HUD.

## 2. CORE GAME LOOP

Each round should follow this structure:

1. Build level
2. Place walls and floor
3. Place pushable blocks
4. Place special blocks
5. Spawn enemies
6. Spawn player
7. Display round intro
8. Run gameplay
9. Detect death or round clear
10. Award bonuses
11. Advance to next round

Main gameplay loop:
- read input
- update player state
- update moving blocks
- resolve crushes and collisions
- update enemies
- resolve player/enemy contact
- resolve item collection
- check special block alignment
- check clear condition
- update effects and animation

## 3. MAP MODEL

Use separate logical layers.

### Terrain Layer
Contains:
- FLOOR
- WALL
- BORDER

This layer is static.

### Block Layer
Contains:
- NONE
- ICE_BLOCK
- SPECIAL_BLOCK
- CRACKED_BLOCK
- UNBREAKABLE_BLOCK

This layer handles pushable and interactive map objects.

### Actor Layer
Contains:
- PLAYER
- ENEMY
- TEMPORARY_EFFECTS

### Item Layer
Contains:
- BONUS_ITEM
- FISH
- EGG
- GEM
- FRUIT
- SCORE_POPUP

Keep these layers logically separate.

## 4. PLAYER RULES

The player moves one tile at a time.

Movement rules:
- if next tile is empty, move
- if next tile is blocked by wall, fail
- if next tile contains pushable block, attempt push
- if next tile contains active enemy, player dies

Recommended implementation:
- tile position is authoritative for logic
- pixel position interpolates for rendering

Player states:
- IDLE
- WALKING
- PUSHING
- STUNNED
- DYING
- VICTORY

Recommended features:
- directional input buffering
- clear facing direction
- bump feedback on failed move
- short push anticipation frame before sliding starts

## 5. BLOCK PUSHING AND SLIDING

This is the core mechanic.

When the player attempts to move into a block:
1. inspect the tile beyond the block
2. if movement is allowed, start slide
3. player enters PUSHING state
4. block slides continuously in that direction
5. block stops when it hits an obstacle

A moving block should continue until:
- it hits a wall
- it hits the map border
- it hits another stationary block
- it hits an unbreakable object

A moving block should not stop after only one tile unless the design intentionally requires that.

### Moving block state data
Each moving block should track:
- current tile x
- current tile y
- current pixel x
- current pixel y
- direction
- speed
- active flag
- combo count for crushes during this slide

## 6. BLOCK COLLISION RULES

### Moving block vs wall
- stop block
- snap to previous valid tile

### Moving block vs stationary block
- stop block
- snap to previous valid tile

### Moving block vs enemy
- enemy is crushed
- combo count increases
- score awarded
- block continues moving unless design says otherwise

### Moving block vs player
- player dies

### Moving block vs item
- either ignore item or destroy item depending on design
- simplest implementation: items are removed if crushed by a block

## 7. ENEMY RULES

Enemies should be simple and readable.
Use tile-based movement like the player.

Enemy base states:
- SPAWNING
- ROAMING
- STUNNED
- RECOVERING
- CRUSHED
- DYING

Recommended base behavior:
- enemy picks a direction
- continues until blocked or at decision point
- avoids instant reversing unless blocked
- at intersections, chooses from valid directions
- may bias toward the player

Recommended simple AI:
- random walk with pursuit bias

## 8. ENEMY TYPES

Start with 1 enemy type for minimum viable implementation.
Expand later to 3-4 types.

Suggested archetypes:

### BASIC_CHASER
- average speed
- prefers movement toward player

### WANDERER
- random movement
- creates chaos

### FAST_ENEMY
- higher speed
- fragile and easy to trap

### HEAVY_ENEMY
- slower
- resistant to first stun or special effects

Keep all enemy silhouettes simple and readable.

## 9. PLAYER VS ENEMY CONTACT

Default rule:
- contact with active enemy causes player death

Optional advanced rule:
- stunned enemies may be safely touched or finished for bonus

For first implementation:
- active enemy contact always kills player
- only sliding blocks defeat enemies

## 10. STUN SYSTEM

Optional but useful.

A stun system can work like this:
- enemy hit by special event or near-impact enters STUNNED state
- stunned enemy cannot harm player for a short duration
- stunned enemy flashes before recovering
- player may finish stunned enemies through a follow-up interaction

Simplest first version:
- skip stun entirely
- moving block crushes immediately
- enemy otherwise remains dangerous

## 11. SPECIAL BLOCKS

Include a few special blocks to add strategy.

Suggested types:
- DIAMOND_BLOCK
- SHOCK_BLOCK
- BONUS_BLOCK
- BREAKABLE_BLOCK

Recommended initial implementation:
- include exactly 3 special marked blocks in each level
- if aligned in a row or column, trigger a bonus event

Possible triggered effects:
- stun all enemies
- bonus score
- spawn treasure
- slow enemies temporarily
- clear some blocks

## 12. SPECIAL BLOCK ALIGNMENT CHECK

Run this check after any block movement completes.

Implementation rule:
- find positions of all special blocks
- if 3 share same row or same column, trigger alignment event

Only check after block movement ends.
No need to check every frame.

## 13. BONUS ITEMS

Bonus items prevent the game from being only survival-based.

Suggested items:
- fish
- egg
- gem
- fruit
- mystery item

Spawn rules:
- after N enemy kills
- after special block alignment
- occasionally on timer
- on selected rounds

Item behavior:
- appears on free tile
- lasts for limited time
- flashes before expiry
- grants score when collected

## 14. LEVEL COMPLETION

Round clear condition:
- all enemies defeated

Optional end-of-round bonuses:
- time bonus
- item bonus
- special block bonus
- perfect clear bonus
- combo bonus

Round clear flow:
1. freeze gameplay
2. play round-clear animation
3. tally bonuses
4. advance to next round

## 15. PLAYER DEATH FLOW

When player dies:
1. freeze gameplay briefly
2. play death animation
3. decrement life counter
4. check game over
5. respawn player

Choose one of these models:

### Preserve map state
- blocks remain moved
- defeated enemies stay dead
- surviving enemies remain
- player respawns

### Reset round state
- rebuild map
- respawn all enemies
- reset blocks

Recommended:
- preserve map state

## 16. DIFFICULTY SCALING

Increase difficulty using a small set of variables:
- enemy count
- enemy speed
- enemy aggression
- map tightness
- item scarcity
- bonus timer speed
- number of advanced enemies

Suggested progression:
- rounds 1-3: slow enemies, open maps
- rounds 4-6: tighter layouts, more enemies
- rounds 7+: mixed enemy types, more pressure

## 17. TIMING MODEL

Use a fixed timestep.
Do not tie logic directly to rendering.

Recommended:
- fixed 50 Hz for PAL-style feel
or
- fixed 60 Hz for general cross-platform logic

Benefits:
- deterministic movement
- predictable collisions
- easy replay/debug support
- arcade-accurate feel

## 18. COLLISION TABLE

### Player vs Floor
- allowed

### Player vs Wall
- blocked

### Player vs Block
- attempt push

### Player vs Enemy
- death

### Player vs Bonus Item
- collect item

### Enemy vs Floor
- allowed

### Enemy vs Wall
- blocked, pick new direction

### Enemy vs Block
- blocked

### Enemy vs Moving Block
- crushed

### Enemy vs Enemy
- no overlap
- either wait, deflect, or choose new direction

### Moving Block vs Wall
- stop

### Moving Block vs Stationary Block
- stop

### Moving Block vs Enemy
- crush

### Moving Block vs Player
- kill player

## 19. UPDATE ORDER

Recommended order per frame:

1. Read player input
2. Update player intent
3. Update player movement
4. Update moving blocks
5. Resolve moving block collisions
6. Update enemies
7. Resolve enemy/player collision
8. Resolve item collection
9. Check special block alignment
10. Check round clear
11. Update effects and animation timers

Order matters.
Do not update enemy logic before resolving moving block collisions.

## 20. SCORING SYSTEM

Suggested scoring:
- first enemy crushed in one slide: 100
- second in same slide: 200
- third: 400
- fourth+: 800 or more
- bonus item: 500 to 2000
- special alignment: 1000+
- round clear bonus: remaining time x multiplier

Recommended:
- make multi-crush scoring escalate fast
- make item scores visible with popups

## 21. LEVEL DESIGN RULES

Each level should ensure:
- player spawn is safe
- at least two escape routes exist
- enemies do not start adjacent to player
- some blocks have room to slide multiple tiles
- no immediate unavoidable deaths
- special block alignment is possible but not trivial

Good levels should have:
- central combat space
- edge trap opportunities
- distinct routes
- readable structure

## 22. DEADLOCK POLICY

Decide whether the player can softlock the map.

Options:
- allow softlocks as a skill/punishment system
- provide reshuffle
- let some blocks break permanently
- inject recovery mechanics

Recommended for an original version:
- avoid permanent hard softlocks

## 23. MINIMUM VIABLE VERSION

Implement in this order.

### V1
- fixed grid
- player movement
- one enemy type
- pushable sliding blocks
- crush logic
- player death
- round clear

### V2
- more enemy types
- bonus items
- score combo system
- special blocks
- better HUD

### V3
- stun/recovery states
- special alignment mechanic
- attract mode
- polish
- sound and juice
- high score system

## 24. RECOMMENDED DATA STRUCTURES

GameState:
- width
- height
- terrain[width][height]
- blocks[width][height]
- items[width][height]
- player
- enemies[]
- movingBlocks[]
- effects[]
- score
- lives
- round
- timer

Player fields:
- tileX
- tileY
- pixelX
- pixelY
- direction
- state
- animFrame
- alive

Enemy fields:
- tileX
- tileY
- pixelX
- pixelY
- direction
- state
- speed
- animFrame
- type
- alive

MovingBlock fields:
- tileX
- tileY
- pixelX
- pixelY
- direction
- speed
- comboCount
- active

## 25. ENUMS

Terrain:
- FLOOR
- WALL
- BORDER

BlockType:
- NONE
- ICE
- SPECIAL
- CRACKED
- UNBREAKABLE

ActorState:
- IDLE
- WALKING
- PUSHING
- STUNNED
- DYING
- VICTORY
- ROAMING
- SPAWNING
- RECOVERING
- CRUSHED

Direction:
- UP
- DOWN
- LEFT
- RIGHT

ItemType:
- NONE
- FISH
- EGG
- GEM
- FRUIT
- MYSTERY

## 26. FEEL / POLISH DETAILS

Add these once the base logic works:
- tiny delay before push starts
- sliding sound while block moves
- squish effect on enemy crush
- score popup on kill
- player idle animation
- enemy flash before recovery
- sparkle effect on bonus pickup
- small screen shake on heavy impact

These are optional but highly recommended.

## 27. PLAYER PUSH PSEUDOCODE

onPlayerAttemptMove(direction):
  nextTile = adjacent tile in direction

  if terrain[nextTile] blocks movement:
    fail move
    return

  if enemy occupies nextTile:
    kill player
    return

  if block exists in nextTile:
    if canStartSlide(nextTile, direction):
      startBlockSlide(nextTile, direction)
      player.state = PUSHING
    else:
      fail move
    return

  move player into nextTile

## 28. MOVING BLOCK PSEUDOCODE

updateMovingBlock(block):
  advance block pixel position using speed and direction

  if block crosses into next tile:
    if next tile contains enemy:
      kill enemy
      block.comboCount += 1
      add score based on combo count

    if next tile contains player:
      kill player

    if next tile is blocked by wall or stationary block:
      snap block to last valid tile
      stop block
    else:
      update block logical tile position

## 29. ENEMY AI PSEUDOCODE

updateEnemy(enemy):
  if enemy.state is STUNNED:
    decrement stun timer
    return

  if enemy is currently moving between tiles:
    continue movement
    return

  validChoices = getValidDirections(enemy)

  if validChoices is empty:
    reverse or wait
    return

  if player is near:
    choose weighted direction toward player
  else:
    choose random direction from valid choices, avoiding reverse if possible

  start movement

## 30. DEFAULT IMPLEMENTATION RECOMMENDATION

Use this baseline configuration:

- grid: 20x12
- tile size: 16x16
- single screen only
- deterministic fixed update
- player sprite around 16x16 or 16x24
- 4 enemy types maximum
- sliding ice blocks as central mechanic
- 3 special blocks per level
- preserve map state after death
- hand-authored levels first

## 31. FINAL DEVELOPMENT PRIORITY

Priority order:
1. grid movement
2. collision
3. block sliding
4. enemy update
5. crush resolution
6. round clear / death loop
7. score and HUD
8. bonus items
9. special blocks
10. polish and presentation

## 32. AMIGA OCS PORT HARD RULES

These rules override convenience during the OCS beta port.

- Do not add scattered `AllocMem(MEMF_CHIP)` calls for anything used by custom-chip DMA.
- Bob art, masks, save buffers, bitplanes, copperlists, sprite data, audio samples, disk DMA buffers, and temporary DMA scratch must live in named deterministic Chip pools with declared offsets and sizes.
- `MEMF_CHIP` alone is not enough for strict OCS. Every DMA-visible pool and sub-range must be checked to remain below `$00080000`.
- On ECS or large-Chip configurations, `AllocMem(MEMF_CHIP)` may return Chip RAM above `$00080000`. If allocation must run there, use a named OCS pool allocator that rejects or temporarily parks high Chip allocations until the real DMA pool is below `$00080000`.
- The first check before enabling blitter, sprite, copper, bitplane, audio, or disk DMA is: is the data in the named pool, is it word-aligned where required, and does `ptr + size` stay below `$00080000`?
- NO ART DATA IN CODE for the strict OCS path. Do not embed generated art, masks, BOBs, tile sheets, title graphics, sprite payloads, PCM, frame bundles, or other bulk binary payloads as C arrays. Keep source code to logic plus small metadata tables only, such as offsets, lengths, dimensions, palettes, enums, and constants. Store bulk bytes as external binary files on disk, then load them into named validated pools. Large `const UBYTE[]` arrays in the executable hunk can change startup memory placement and cause return `21` regressions even when runtime pool order looks unchanged.
- Hardware sprites must use all 8 pointer channels deliberately. Unused channels point to valid null sprite streams in the deterministic sprite pool before sprite DMA is enabled.
- OCS hardware sprites are normal 16-pixel-wide, 2-bitplane streams unless explicitly proven otherwise. Use two control words, two data words per line, and a `0,0` terminator; keep the stream word-aligned and below `$00080000`.
- Pick sprite hardware channels by their fixed OCS palette groups. Sprites 0/1 use `COLOR17-COLOR19`, 2/3 use `COLOR21-COLOR23`, 4/5 use `COLOR25-COLOR27`, and 6/7 use `COLOR29-COLOR31`. For title sparkles, use channels 6/7 rather than 0/1 so they match the white ECS sparkle colours instead of the red palette group.
- For title sparkles, use the ECS source data. Convert `AMIGA_SPR_EFFECT_SPARK` from `g_amiga_tile_data` into the OCS 2-bitplane sprite stream with the same colour reduction; do not replace it with a hand-drawn procedural star unless explicitly requested.
- For the OCS gameplay enemy hardware-sprite test, red `ENEMY_A`/chaser enemies stay on blitter BOBs. Only non-red enemy types may use normal OCS hardware sprites, and gameplay enemy sprites must use channels 2-7 only; do not put gameplay enemies on channels 0/1 because that is the red fixed palette group.
- The live gameplay enemy sprite streams are stored in the unused tail of each already-reserved copperlist. This is deliberate: it keeps the proven graphics arena size and allocation order unchanged while still keeping sprite DMA data below `$00080000`. Do not grow or move the compact sprite bank for this path unless the memory map is explicitly rebudgeted.
- Match the ECS title-sparkle scheduler before inventing new sparkle timing: scan the five title anchors in order, show up to two sparkles where `((tick / 2) + i * 5) & 15` is `<= 4`, and hide unused sparkle streams.
- The compact sprite pool may store live sprite data in logical slots and route hardware `SPRxPT` registers to them through the copperlist. Even when doing that, every hardware sprite channel must still have an intentional pointer target, and every unused channel must point at a per-channel null stream.
- Animate OCS sprites one step at a time: after `WaitTOF()`, update only the existing pooled sprite control/data words first. Do not add allocations, rebuild copperlists, redraw visible bitplanes, or multiplex sprites unless that exact step is being tested.
- Use OCS lores positioning that matches the active display window. The current probe's working conversion is `x + 0x80`, `y + 0x2C`; do not use ECS/AGA fine positioning, SHRES assumptions, `BPLCON3`, or palette-bank tricks for the OCS beta.
- Do not move a working OCS allocation layout casually. Treat the memory map as an ABI: document offsets, preserve proven order, and test one hardware change at a time.
- Amiga hardware does not require bitplane/screen buffers to start on 64K boundaries. That rule was a false assumption and caused allocation failures. Packed screen buffers are valid when word-safe, correctly moduloed, non-overlapping, and DMA-visible; for strict OCS they must also remain fully below `$00080000`.
- For strict 512 KB OCS bring-up, keep the memory layout as small as the current test allows. Do not carry ECS-era double buffers, source buffers, full converted titles, or oversized sprite banks into a sprite/blitter probe unless that specific object is being tested and budgeted.
- For OCS double buffering, preserve the proven single deterministic below-`$00080000` Chip arena. Carve assets first, control/sprite/copper second, then packed screens third. Do not reorder these blocks when adding CPU-only gameplay code or state. Amiga screen buffers do not need 64K starts; validate the full arena, every subrange, and screen overlap before enabling DMA.
- Once a DMA memory layout is confirmed working, do not move, resize, append to, or reorder the graphics arena to add an unrelated feature. In particular, copperlist content changes must stay inside the already reserved copperlist space; they must not change the graphics arena size, control pool size, screen positions, audio pool allocation, or CPU-only allocation order. Any unavoidable memory-layout change needs explicit user approval and a before/after memory map.
- Extra Chip above `$00080000` is acceptable only for CPU-only data in this strict OCS port. Do not put Paula samples, copperlists, sprite streams, bitplanes, blitter sources/masks/save buffers, or disk DMA buffers there. If a feature causes return `30`, first check whether the separate named audio pool still fits below `$00080000`, and trim/rebudget the external SFX bank before touching the proven graphics carve order.
- For OCS SFX, Paula samples must be loaded from `Icepanic.sfx` into a named deterministic audio pool before playback. Do not reorder, resize, append to, or combine the proven graphics arena to add audio; keep assets first, control/sprite/copper second, and packed screens third in the graphics arena, then claim the audio pool separately after the graphics arena is secured. A previous combined graphics+audio allocation caused return `21`: the executable got smaller, but the first low-Chip DMA allocation got much larger. Do not link PCM bytes into the OCS executable hunk, because that can shift low-Chip startup placement before the DMA arena is claimed. Use the generated ECS-derived `amiga_sfx_assets_ocs` metadata tables and disk bank for the strict OCS beta. The OCS bank should contain the same generated one-shot sample IDs as ECS, but as an external binary payload, not a C byte array. The current OCS SFX bank is `29640` bytes at `6000 Hz`; shrink the external bank further if return `30` comes back. Paula must still point only at the validated pooled copy, not at an unvalidated hunk or CPU-only gameplay allocation.
- Direct Paula one-shots are not self-stopping. Track each active channel with the generated SFX tick count and disable that channel's DMA when the count expires, otherwise short samples can loop or chatter until another sound overwrites the voice. Noisy gameplay events should also have small cooldowns so repeated core events do not retrigger the same Paula voice every frame. Major modal fanfares must be phase-latched and per-sample locked out for ECS parity: title start, round clear, life lost, player death, game over, and new high score play once for the current `GamePhase`, and the same sample remains blocked briefly even if a phase transition emits another event. Do not rely only on a short phase-local cooldown for fanfares. Major fanfares must also own a short busy window that swallows lower-priority transition sounds, including round-start, meta, stage-modifier, reward, combo, timer, impact, and confirm-chirp events; otherwise stacked core events can make one transition sound like repeated fanfares.
- The required OCS asset-generator pattern is: binary payload file for data, small C table for metadata. The graphics/SFX generators are the template: `amiga_assets_ocs.bin` is packaged as `Icepanic.gfx`, and `amiga_assets_ocs.c` contains only the palette; `amiga_sfx_assets_ocs.bin` is packaged as `Icepanic.sfx`, and `amiga_sfx_assets_ocs.c` contains only offsets, lengths, and tick counts. Do not add or regenerate giant C byte arrays for OCS payloads unless the user explicitly approves a memory-map rebudget after a failed binary-file approach.
- Do not add a separate return-`23` screen-order guard for the OCS beta arena. The screen block is carved by construction from the proven arena order; validate real hardware requirements instead: full range below `$00080000`, no overlap, and correct copper bitplane pointers.
- Use two complete copperlists for page flipping. Render only into the hidden screen, wait for the blitter, wait for `WaitTOF()`, then switch the whole copperlist with `COP1LC/COPJMP1`; do not patch the visible/executing copperlist's bitplane pointers.
- Preallocate copperlists generously and keep them complete. The OCS beta reserves `1024` words per copperlist while the current title/UI list emits `592` words including title sparkles plus the tinted per-line PAL reflection; unused words are initialized as copper end instructions. If adding copper commands, update the emitted-word budget and never revive live bitplane-pointer patching.
- The tinted per-line PAL reflection is part of the current presentation and stays enabled in the existing complete copperlists. Keep the copper animation patch cadence simple and deterministic: patch sparkle colour waits and reflection scroll together when the copper animation phase changes. Do not allocate another list or move the proven control pool for reflection experiments.
- OCS gameplay rendering is dirty-only. Keep the CPU-only per-buffer render cache: mark old/new dynamic BOB tile coverage dirty only when the cached BOB changed, redraw only dirty static cells, and blit only current dynamic BOBs that overlap those dirty cells. Hardware-sprite enemies must be excluded from the BOB dirty cache and updated only by editing their pooled sprite control/data words before the buffer's copperlist is queued. HUD values must be field-delta patched, not full-strip redrawn. Do not restore/redraw every actor every frame, and do not clear/redraw the full playfield while gameplay is active.
- UI performance must follow the ECS renderer: byte-wide horizontal fills for panels/rectangles, row-mask glyph drawing for text, field-delta HUD patches, and layout-keyed modal caches. UI selection changes are overlay-only when the layout key is unchanged; for supported modals such as perk choice, patch only the changed card chrome/footer and keep the cached playfield/background intact.
- Prefer ECS-parity blit costs: terrain/block tiles are opaque copy blits, while items, mines, actors, title cells, and other shaped objects use single-pass cookie blits when using the generated mask format. A two-pass clear-then-OR blit doubles blitter work and should not come back unless an isolated asset test proves the cookie path wrong. For shifted BOB cookie blits, clear the A/B preload data words before starting the blit, and dirty/restore only the actual 16-or-32-pixel BOB footprint rather than an over-wide tile span.
- Gameplay state and other CPU-only allocations are not custom-chip DMA objects. Allocate them after the deterministic DMA arena with normal public memory; they may live above `$00080000` if CPU-visible memory exists there, as long as copper, blitter, sprites, Paula, and disk DMA never point at them.
- Menus must confirm on button release, not button press. On the OCS beta, the joystick fire button is also the start/confirm input, so release gates must handle both `fire_released` and `start_released`. Perk choice must ignore both release paths during its entry lockout to prevent a held confirm from round clear or game over from immediately choosing a perk.
- Claim and validate the deterministic OCS DMA arena before opening optional libraries or allocating CPU-only gameplay state. Do not change the internal arena carve order to solve program-footprint problems.
- For OCS beta gameplay, compile `game.c` with `ICEPANIC_AMIGA_OCS_FIXED_LEVELS` and use the shared ECS gameplay core as the source of truth. This excludes the procedural WFC template table/support path during bring-up without inventing alternate gameplay behavior.
- With `ICEPANIC_AMIGA_OCS_FIXED_LEVELS` and `ICEPANIC_AMIGA_SMALL_STACK`, the shared core may compile its OCS arcade-core slice to reduce the loaded hunk: keep fixed ECS-authored levels, player/enemy/block runtime, collisions, scoring, items, bombs/mines, round/death/game-over flow, and the between-round perk draft. Exclude procedural generation and renderer-heavy systems until those systems are deliberately budgeted and rendered on OCS.
- Bombs/mines are in the OCS gameplay budget. Do not stub them out under `ICEPANIC_AMIGA_OCS_ARCADE_CORE` unless an explicit `ICEPANIC_AMIGA_OCS_SKIP_MINES` experiment is requested. OCS parity means shared ECS mine placement/fuse/explosion logic, placed mine BOB rendering, mine-blast impact sparks, ECS HUD `stock:capacity` plus meter, and `GAME_EVENT_MINE_PLACED`/`GAME_EVENT_MINE_EXPLODED` mapped to Paula SFX.
- Keep the fixed-level table at 16 authored `20x12` layouts. The layout index may wrap with `(round - 1) % 16`, but difficulty must continue to use the absolute round number so enemy count, speed, type mix, aggression, and timer pressure still climb after round 16.
- Do not add a separate tiny or placeholder OCS gameplay path. If ECS gameplay data is too large for the current executable footprint, move/rebudget CPU-only data after the proven DMA arena or load it separately; do not move working custom-chip allocations to make room.
- The old OCS probe is now the primary Amiga beta line and replaces the old ECS release path. Build it with `make -f Makefile.amiga`; the output is `build/amiga/icepanic-v0.9.1-beta.2-amiga-ocs-pal.adf`.
- OCS gameplay rendering is dirty-only in the beta. Do not add a separate backdrop/save-buffer allocation unless it is explicitly budgeted and tested; a separate backdrop pool caused return `32`, and combining it into the graphics arena caused return `21`. Redraw only dirty static cells into the hidden draw buffer, then blit only current dynamic BOBs overlapping those dirty cells.

END_OF_FILE
```
