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

END_OF_FILE
```
