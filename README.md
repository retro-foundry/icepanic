# Icepanic (C + SDL2)

`Icepanic` is an original Pengo-style single-screen arcade prototype built for deterministic gameplay and eventual Amiga-friendly porting.

Current release version: `v0.9.1-beta.2`.

Release artifacts use the pattern `icepanic-v<version>-<platform>`, for example:
- `icepanic-v0.9.1-beta.2-amiga-ocs-pal.adf`
- `icepanic-v0.9.1-beta.2-amiga-ocs-pal`
- `icepanic-v0.9.1-beta.2-pc.zip`

## Constraints
- Logical resolution: `320x200`
- Grid: `20x12` tiles at `16x16`
- Fixed update: `50 Hz`
- Palette: `32 colors`
- Logic uses integer/fixed-point movement only

## Amiga Hardware Notes
These notes came from the ECS/OCS blitter probe work and are worth preserving.

- OCS port target: A500/A1000/A2000-class OCS, 68000 CPU only, 512 KB chip-DMA-visible RAM, no fast RAM assumed, Kickstart 1.2/1.3 preferred, 15 kHz PAL/NTSC display only, and no ECS-only registers or display modes.
- Build Amiga runtime code with Kickstart 1.3-compatible headers and compiler targets. The current probe uses `+kick13s` for executable code, `+kick13` for asset objects, and NDK 1.3 headers from `NDK_1.3/INCLUDE-STRIP1.3/INCLUDE.H`.
- Prefer `struct Custom` field access for live custom-chip registers. Use `offsetof(struct Custom, ...)` to generate copper-list register offsets. Do not mix arbitrary hardcoded blitter register addresses with NDK `struct Custom` access.
- For custom pointer registers, write the high and low words explicitly through a helper. This avoids compiler/ABI surprises around `APTR` writes on old Amiga targets.
- Use `-O2` for the Amiga build unless a specific reason says otherwise. During probe work, `-O3` made hardware timing/debugging noisier.
- Always enable the relevant DMA bits before offscreen blitter work. The probe explicitly enables master + blitter DMA before drawing into hidden buffers.
- Wait for the blitter before changing blitter registers. Read `DMACONR` first, then poll `DMAF_BLTDONE`. Do not use arbitrary CPU delay loops for timing; frame pacing should use proper vertical blank waits such as `WaitTOF()` or an interrupt/vblank-driven equivalent.
- Runtime-shifted BOBs work on ECS, but channel enables and minterms must match the intended expression. Enabling a source channel is separate from referencing it in the minterm.
- The reliable shifted BOB path found in the probe is two-pass:
  - clear destination with the shifted mask,
  - OR shifted art into the cleared destination.
- For strict OCS compatibility, treat BOB graphics, masks, temporary save buffers, copperlists, sprite data, bitplanes, and audio samples as needing to live below `$00080000`, inside the first 512 KB Chip RAM DMA-visible area. A defensive runtime test is `if ((ULONG)ptr >= 0x00080000) { /* not safe for strict OCS blitter/DMA */ }`; range checks should also ensure `ptr + size` does not cross `$00080000`.
- Do not assume `MEMF_CHIP` alone is sufficient for strict OCS. Trapdoor/slow RAM must never be treated as chip DMA-visible memory, and code must not assume 1 MB or 2 MB Chip RAM.
- Chip memory placement matters. The blitter probe showed assets and buffers that worked in one allocation could fail or corrupt when allocated later. All strict-OCS DMA-visible data must come from deterministic Chip pools with declared offsets, sizes, and order. Do not add scattered runtime `AllocMem` calls for blitter assets, sprite banks, null sprites, copperlists, bitplanes, audio samples, disk DMA buffers, masks, save buffers, or temporary DMA scratch.
- NO ART DATA IN CODE for the strict OCS path. Do not embed generated art, masks, BOBs, tile sheets, title graphics, sprite payloads, PCM, frame bundles, or other bulk binary payloads as C arrays. Source files may contain logic and small metadata tables only: enums, offsets, lengths, dimensions, palette constants, and similar bookkeeping. Bulk payloads must be external binary files packaged on disk and loaded into named, range-checked pools. Large `const UBYTE[]` assets in the executable hunk can move startup memory and break otherwise-working below-`$00080000` allocations.
- Treat the OCS memory map as an ABI. If a new DMA-visible object is needed, add it deliberately to a named pool or create a new named pool with a fixed position in startup allocation order, then check every `ptr + size` range against `$00080000`. Do not append bytes to a proven pool casually, because changing its size can shift later allocations and break otherwise-correct blits.
- Amiga hardware does not require bitplane/screen buffers to start on 64K boundaries. The mistaken 64K screen-alignment rule caused oversized arenas and startup allocation failures. OCS/ECS bitplane pointers can address packed Chip RAM buffers; the hardware requirements are word-safe addresses, correct row/modulo setup, no overlap, and a DMA-visible address range.
- Keep DMA-visible BOB masks, BOB art, tile masks, tile art, and title scratch in an early deterministic chip allocation, in a known order. The working probe order was masks, BOBs, tile masks, tiles, then the 16x16 title conversion scratch. Preserve that order unless a new memory map is intentionally tested.
- OCS title migration lesson: do not blit the generated full title overlay blob directly. The CPU decoder proved the source title asset was valid, but direct full-title blits, row/plane blits, widened BOB scratch blits, and late separate title DMA allocations caused corruption or startup failures. The working OCS path is to keep the title source as CPU-read const data, convert one 16x16 cell at a time into normal tile-format art+mask scratch memory, keep that tiny scratch area inside the deterministic Chip asset pool, and cookie-cut blit each non-empty aligned cell.
- Title-cell scratch layout that worked: 160 bytes tile art plus 160 bytes tile mask, using the same row/plane layout as `g_amiga_tile_data` and `g_amiga_tile_mask`. Keep the title draw position 16-pixel aligned when using tile-format scratch; use widened BOB scratch only after a separate isolated test proves shifted cells are correct.
- Avoid large permanent converted title allocations on OCS. A full converted title costs too much Chip RAM and can perturb fragile display-buffer placement. Reuse one small conversion scratch cell, or pre-slice the title into known-good tile assets if memory is budgeted explicitly.
- Hardware sprite migration lesson: adding sprite banks to the working BOB/title asset pool changed Chip placement enough to corrupt blitter output, and separate sprite `AllocMem` calls still disturbed startup placement. Sprite banks, null sprites, and any copper-updated sprite pointer targets must be allocated through a deterministic OCS sprite pool with fixed startup order and below-`$00080000` range checks before sprite DMA is enabled.
- OCS null sprites must be deliberate sprite streams, not an accidental pointer into whatever allocation happened next. Avoid sharing one tiny zero stream across all channels. Use a documented per-channel null slot, preferably a transparent line below the visible display followed by `0,0`, and keep every unused `SPRxPT` target initialized before enabling sprite DMA.
- OCS normal hardware sprites are 16 pixels wide and 2 bitplanes: 3 visible colours plus transparent. The DMA stream is two control words, then two data words per visible line, then a `0,0` terminator. Keep the stream word-aligned in the deterministic sprite pool and below `$00080000`.
- OCS sprite palette groups are fixed by hardware channel. Sprites 0/1 use `COLOR17-COLOR19`, sprites 2/3 use `COLOR21-COLOR23`, sprites 4/5 use `COLOR25-COLOR27`, and sprites 6/7 use `COLOR29-COLOR31`. Pick the hardware channel to match the art palette; moving the title sparkles from channel 0 to channels 6/7 fixed the red-sparkle problem because channels 6/7 use the ECS sparkle colours.
- Do not hand-draw replacement hardware sparkle data. The ECS hardware sparkle path converts `AMIGA_SPR_EFFECT_SPARK` from `g_amiga_tile_data` into the 2-bitplane sprite stream, using the same colour reduction: source colour `31 -> sprite colour 3`, source `30` or `21 -> 2`, any other non-zero source -> `1`, and zero stays transparent. The OCS beta should use that same conversion so sparkle size and shape match the ECS version.
- The logical layout of the compact sprite bank does not have to match hardware channel order. It is safe to store the live sparkle streams first, then route hardware `SPR6PT`/`SPR7PT` to them in the copperlist, as long as every other hardware channel points at its own valid null stream in the same deterministic pool.
- OCS sprite positioning is lores-style and tied to the display window. For the current `DIWSTRT` values, the working conversion writes sprite control words with `x + 0x80` and `y + 0x2C`; do not assume ECS/AGA fine-positioning, SuperHires positioning, or palette-bank features.
- The first safe animation step is sprite-only: after `WaitTOF()`, update the live sprite's control words and small image payload inside the existing sprite pool. Do not allocate, rebuild the copperlist, or redraw bitplanes just to animate a hardware sprite.
- Hardware sprite DMA is currently used in the OCS beta for ECS title-sparkle parity and for a gameplay enemy-sprite test. On the title screen, sprite channels 6 and 7 hold up to two simultaneous 16x16 title sparkles, and channels 0-5 point at valid transparent one-line placeholders. During gameplay, red `ENEMY_A`/chaser enemies remain blitter BOBs, while non-red enemy types may use normal OCS hardware sprites on channels 2-7 only; channels 0/1 are deliberately avoided for gameplay enemies because their fixed `COLOR17-COLOR19` group is the red palette group. The live gameplay enemy sprite streams are stored in the unused tail of each already-reserved copperlist, so this does not resize or reorder the proven graphics arena. The OCS probe follows the ECS title sparkle scheduler: scan the five title anchors in order, show up to two sparkles where `((tick / 2) + i * 5) & 15` is `<= 4`, and hide unused sparkle streams. Channels 6/7 are deliberate for title sparkles because normal OCS sprites 6/7 use `COLOR29-COLOR31`, matching the ECS title sparkle palette.
- OCS audio migration lesson: Paula samples follow the same deterministic-pool rule as sprites and bitplanes. The OCS beta loads the generated ECS-derived `Icepanic.sfx` bank from disk into a named, separately allocated audio pool after the proven graphics DMA arena has been claimed and validated. Do not append audio bytes to the graphics arena or combine the two into one larger allocation: that changed the first low-Chip claim from `123992` bytes to the graphics+audio total and caused return `21` even though the executable was smaller. Keep PCM bytes out of the executable hunk; linking the bank into the program can shift low-Chip startup placement and regress the proven graphics arena allocation. Do not use late scattered `AllocMem(MEMF_CHIP)` for samples, and do not point Paula at CPU-only gameplay state or unvalidated program hunks. If the audio pool fails with return `30`, shrink or rebudget the external SFX bank before touching the graphics arena. The current strict-OCS audio bank contains the same generated one-shot sample IDs as the ECS version, including title screen fanfare, title confirm chirp, round start, block push, enemy killed/crush, round clear, player killed, respawn/life-loss fanfare, game over, enter-score/new-hi-score fanfare, and the other ECS gameplay event samples.
- Direct Paula sample playback must be actively stopped. The hardware repeats the programmed sample buffer until audio DMA for that channel is disabled, so even tiny OCS one-shot players need per-channel tick lifetimes from the generated SFX metadata and should disable the voice when the lifetime expires. Add small event cooldowns for noisy gameplay events instead of retriggering the same channel every frame. Major modal fanfares, such as title start, round clear, life lost, player death, game over, and new high score, also need phase latching plus a per-sample lockout that survives phase changes. Play each major fanfare once for the current `GamePhase`, then block the same sample for a short fixed window even if another transition event is emitted. A short phase-local cooldown alone is not ECS parity, because a repeated event can restart the fanfare while the same modal flow is still visible. A major fanfare must also create a short "busy" window: while that window is active, swallow secondary transition jingles such as round-start, meta, stage-modifier, reward, combo, timer, impact, and confirm-chirp events, otherwise a single modal flow can sound like several fanfares repeating.
- For OCS asset generators, use a binary payload plus a tiny C metadata object. The current graphics/SFX pattern is the model: `amiga_assets_ocs.bin` becomes `Icepanic.gfx` on the ADF, while `amiga_assets_ocs.c` contains only the palette; `amiga_sfx_assets_ocs.bin` becomes `Icepanic.sfx`, while `amiga_sfx_assets_ocs.c` contains only offsets, lengths, and ticks. Do not regenerate the old style of giant C byte arrays for OCS payloads. Any proposed exception must first prove why a binary file cannot work and must explicitly rebudget the executable hunk and startup placement.
- The OCS beta scoreboard should match the ECS opening HUD before gameplay is moved across: labels `SCORE`, `RD`, `TIME`, `BOMB`, `LIV`; separator columns at `142`, `174`, `218`, and `266`; stippled dark-blue HUD background; bomb stock/capacity meter at `255,193`; initial visible values `000000`, round `01`, time `126`, bombs `0:0`, and lives `04`.
- The OCS beta title high-score panel should match the ECS `draw_opening_high_scores()` layout: panel bounds `86,96` to `233,144`, accent colour `16`, centered `HI SCORES` at `y=102`, rows at `110,116,122,128,134`, initials `ACE`, `ICE`, `PAN`, `FOX`, `JET`, and scores `50000`, `35000`, `24000`, `16000`, `9000` right-aligned to `x=223`.
- The OCS beta now links the shared gameplay core and reads joystick input with the OCS-safe `JOY1DAT`/CIA fire path. Gameplay state is allocated after the deterministic DMA arena with `MEMF_PUBLIC|CLEAR`; it is CPU-only data, is never used as a copper/blitter/sprite/audio pointer target, and may live above the first-512 KB strict OCS DMA boundary if the machine has CPU-visible memory there.
- Menu confirmation must happen on button release, not button press. This matters on OCS because the single fire button is also used as the start/confirm input: a held button from round clear or game over can otherwise skip the next modal screen. The perk UI must gate both `fire_released` and `start_released` during its short entry lockout; blocking only `fire_released` is not enough when fire is mapped to start.
- For the OCS beta, compile the shared ECS gameplay core with `ICEPANIC_AMIGA_OCS_FIXED_LEVELS`. This preserves the real gameplay behavior while excluding the procedural WFC template table/support path during bring-up. Do not invent a separate OCS gameplay implementation; port ECS behavior directly and move/rebudget CPU-only data separately from the fixed custom-chip DMA arena.
- `ICEPANIC_AMIGA_OCS_FIXED_LEVELS` plus `ICEPANIC_AMIGA_SMALL_STACK` enables the OCS arcade-core slice inside the shared `game.c`: it keeps the real ECS fixed-level runtime for player movement, enemies, sliding blocks, collision, scoring, items, bombs/mines, round start/clear, death, game over, and the between-round perk draft. It still compiles out procedural generation and renderer-heavy systems that are not yet rendered/usable in the OCS beta. This is code-size budgeting of the existing game, not a replacement gameplay implementation.
- The fixed-level path uses 16 authored `20x12` layouts. Layout selection wraps after level 16 (`round 17` uses layout 1 again), but difficulty is still based on the absolute round number through `setup_round_config()`, so enemy count, speed, type mix, aggression, and timer pressure continue to rise after the authored layout loop.
- Current OCS gameplay hook: `game_init()` and `game_step()` come from the shared ECS gameplay core. The Amiga renderer uses a CPU-only per-buffer dirty cache for steady gameplay: mark old/new dynamic BOB tile coverage dirty only when an actor changes, redraw dirty static cells, then blit only dynamic BOBs overlapping those dirty cells. Red `ENEMY_A`/chaser enemies, player, blocks, effects, mines, and any enemy that cannot get one of the six non-red hardware sprite channels remain on the BOB path. HUD updates are field-delta based, so changed digits are patched instead of redrawing the full strip. Gameplay code/data is CPU-only unless a custom-chip pointer is explicitly aimed at it, so it may be placed above `$00080000` on machines with CPU-visible memory there.
- Current OCS beta DMA memory shape: claim the proven `123992` byte graphics arena first as its own deterministic below-`$00080000` allocation. The graphics arena carve order is unchanged: asset block first (`39680` bytes: masks, BOBs, tile masks, tiles, title-cell scratch), control block second (`4312` bytes: compact sprite bank plus two complete copperlists), then packed screen block third (`80000` bytes: two `40000` byte 320x200x5bpl screens). The asset block is loaded from `Icepanic.gfx` into the same proven masks/BOBs/tile-masks/tile slots; the title source portion of `Icepanic.gfx` is loaded into CPU-only memory and converted through the existing title-cell scratch. After the graphics arena is secured, claim a separate named audio pool for the generated ECS-parity Paula SFX bank loaded from `Icepanic.sfx`; the current OCS bank is `29640` bytes at `6000 Hz` with all ECS one-shot sample IDs retained as external data. Do not reorder, resize, append to, or combine the graphics arena when adding CPU-only gameplay code or audio; allocate gameplay only after DMA pools are secured. Each copperlist reserves `1024` words and currently emits `592` words including title sparkles plus the tinted per-line PAL reflection; six 16x16 gameplay enemy sprite streams fit in the unused copperlist tail, keeping copper effects and sprite DMA inside the existing control block without moving later DMA data. Amiga bitplane buffers do not need 64K alignment; screen 0 starts at the start of the screen block and screen 1 starts immediately after screen 0. The graphics arena, audio pool, and every carved subrange are checked for no overlap and first-512 KB OCS DMA safety before DMA is enabled.
- Extra Chip RAM above `$00080000` on an ECS/expanded emulator is not a safe place for strict-OCS DMA targets. It can hold CPU-only data, but Paula samples, copperlists, sprite streams, bitplanes, blitter sources/masks/save buffers, and disk DMA buffers must still be below `$00080000`. Return `30` can be caused by executable-hunk growth even when no deterministic DMA arena offsets moved, because the OS loader consumes memory before the later audio pool is allocated. Prefer trimming code/debug tables or moving non-DMA bulk data to external files before changing the proven DMA layout.
- Allocate and validate the deterministic OCS DMA arena before opening optional libraries or allocating CPU-only gameplay state. This claims the scarce first-512 KB DMA-visible Chip range before other startup paths can fragment it, without changing the proven arena carve order.
- On ECS/large-Chip configurations, `AllocMem(MEMF_CHIP)` can return Chip RAM above the strict OCS DMA-safe range. The OCS beta pool allocator may temporarily park high Chip allocations and retry; parked blocks must never be used for DMA and must be freed after the named below-`$00080000` pools are secured.
- OCS beta startup return codes while sprite DMA/double buffering/gameplay bring-up is under test: `21` graphics DMA arena allocation or asset carve failed, `22` DMA arena or asset range not below `$00080000`, `24` screen range not below `$00080000`, `26` control/copper range not below `$00080000`, `27` screen overlap or arena overflow, `28` copperlist build exceeded/mismatched the reserved emit budget, `29` CPU-only gameplay state allocation failed, `30` audio sample pool range/alignment/file-load/init failed, `31` graphics asset file/title-source allocation/load failed. Return `23` was a redundant screen-order guard and must not be reintroduced; the screen block is carved directly from the proven arena order.
- Keep display buffers in a deterministic Chip pool too. Amiga bitplane pointers do not require 64K-aligned screen starts; packed 40 KB 320x200x5bpl screen buffers are valid as long as every full buffer range is word-aligned, non-overlapping, and below `$00080000` for strict OCS.
- If a future change proposes screen alignment padding, treat it as suspect until backed by the Amiga hardware manual and an isolated test. Do not spend Chip budget on 64K screen padding by default.
- For the current double-buffer hardware-sprite bring-up, never redraw the visible buffer. Draw into `draw_index`, wait for the blitter, wait for vblank, then switch to that buffer's complete copperlist with `COP1LC/COPJMP1`. Keep `front_index` and `draw_index` explicit and separate from `tick`. Hardware sprite control-word changes are still allowed after `WaitTOF()` because they stay inside the deterministic sprite pool and do not touch bitplane memory.
- OCS gameplay rendering must not clear and redraw the full playfield every frame. The beta keeps a CPU-only per-buffer render cache, accumulates `game_dirty_static_rows()` for both buffers, marks old/new dynamic BOB tile coverage dirty only when the cached BOB changed, redraws only dirty static cells, then blits only current dynamic BOBs overlapping those cells. Hardware-sprite enemies must be removed from the BOB dirty cache; they should update only their pooled sprite control/data words. HUD values must use delta updates rather than full-strip redraws. Menus/overlays may use cached full redraws only when their layout changes, but the title screen should not redraw bitplanes every frame because the sparkles are hardware sprites.
- UI drawing must use the ECS-style cheap primitives: byte-wide horizontal fills for panels/rectangles, row-mask glyph drawing for text, field-delta HUD patches, and layout-keyed modal caches. UI selection changes must not rebuild the whole hidden screen; for same-layout modal changes such as perk selection, patch only the changed card chrome/footer and keep the cached playfield/background intact.
- OCS tile and BOB drawing should match the ECS cost model: terrain/block tiles are opaque copy blits, while items, mines, actors, title cells, and other shaped objects use the single-pass cookie minterm (`(source & mask) | (dest & ~mask)`) where the asset mask format allows it. Do not reintroduce the slower two-pass clear-then-OR path unless a specific asset proves incompatible; shifted BOB cookie blits must clear the A/B preload data words before starting the blit. Dirty actor restore regions should use the actual ECS 16-or-32-pixel BOB footprint, not a blanket three-tile span.
- The tinted per-line PAL reflection is part of the current presentation and remains active in the existing complete copperlists. Keep the copper animation patch cadence simple and deterministic: patch the sparkle colour waits and reflection scroll together when the copper animation phase changes. Do not allocate a separate copperlist or move the proven copper/control pool for reflection experiments.
- Keep the ECS stopped-block settle animation in the OCS arcade-core slice. Bombs/mines are part of the OCS gameplay budget now: do not compile them out unless an explicit `ICEPANIC_AMIGA_OCS_SKIP_MINES` experiment is being run. Sliding blocks still need the `ImpactFx` stop records so the renderer can hide the static tile and draw the block as a displaced dynamic BOB while it settles back into place.
- OCS bomb parity requires all three layers: shared ECS mine gameplay enabled in `game.c`, `GAME_EVENT_MINE_PLACED`/`GAME_EVENT_MINE_EXPLODED` mapped to the ECS-derived Paula samples, and renderer support for placed mine BOBs plus mine-blast impact sparks. The HUD must keep the ECS `stock:capacity` numbers and 8-cell meter at `255,193`.
- Do not patch bitplane pointer words inside the copper list that may currently be executing. For page flipping, build complete per-buffer copper lists, finish rendering the hidden buffer, wait for the blitter, wait for vblank, then switch with `COP1LC` and `COPJMP1`.
- The current copperlists are built as complete per-buffer lists and the unused reserved copper space is prefilled with copper end instructions. Do not reintroduce the old bitplane-pointer patch helper; grow `PROBE_COPPER_USED_WORDS` with any new emitted copper instructions and keep `PROBE_COPPER_WORDS` large enough before enabling DMA.
- The old blitter probe is now the primary OCS beta line, replacing the old ECS release path. Build it with `make -f Makefile.amiga` or `make -f Makefile.amiga ocs-beta`; the output is `build/amiga/icepanic-v0.9.1-beta.2-amiga-ocs-pal.adf`.

### OCS Port Audit Checklist

Use this checklist when moving ECS code into the strict OCS beta path.

- Chip RAM / DMA visibility: find every allocation for BOBs, masks, save buffers, bitplanes, copperlists, sprite data, audio samples, and disk DMA buffers. Anything touched by custom chips must be allocated from a deterministic named Chip pool, must have a documented offset and size, and must remain below `$00080000`, including the full `ptr + size` range. Scattered `AllocMem(MEMF_CHIP)` is not acceptable for strict OCS DMA data.
- BOB and blitter safety: verify word alignment, row strides, modulos, bitplane interleaving, restore/save buffer bounds, and 512 KB boundary crossings. Split oversized blits rather than relying on ECS-era large operations.
- Hardware sprite safety: sprite banks, null sprites, and every `SPRxPT` target must come from a deterministic sprite pool or another documented DMA pool. Unused sprite channels must point at a valid pooled null sprite before sprite DMA is enabled. Normal OCS sprites are 16 pixels wide, 2 bitplanes, 3 visible colours plus transparent; attached sprites must use only even/odd pairs with matching position and height.
- ECS-only custom-chip usage: search for `BEAMCON0`, `DIWHIGH`, `BPLCON3`, `BLTSIZV`, `BLTSIZH`, `BLTCON0L`, `DENISEID`, `HTOTAL`, `VTOTAL`, `HSSTRT`, `VSSTRT`, `HCENTER`, SuperHires/SHRES, Productivity/VGA/31 kHz modes, and software PAL/NTSC switching assumptions. Each occurrence needs an OCS fallback or removal.
- Display setup: keep to OCS-safe 15 kHz modes such as 320x256 PAL or 320x200 NTSC. Validate `DIWSTRT`, `DIWSTOP`, `DDFSTRT`, and `DDFSTOP` against conservative OCS values.
- PAL/NTSC timing: find assumptions that gameplay, music, animation, physics, or timers run at 50 Hz. Either explicitly mark the build PAL-only or add a PAL/NTSC timing strategy.
- Kickstart compatibility: avoid Kickstart 2.x+ APIs, ModeIDs, Display Database use, tag-based screen/window APIs, and `AllocBitMap()`. Check `graphics.library`, `intuition.library`, `dos.library`, and `exec.library` calls for Kickstart 1.2/1.3 safety.
- CPU compatibility: keep code 68000-clean. Avoid 68010/68020+ instructions, cache assumptions, fast RAM assumptions, and alignment-sensitive accesses that can fault or slow down badly on 68000.
- Performance and DMA contention: flag full-screen clears, full playfield copies, many BOBs per frame, wide overscan, 5-bitplane pressure, and CPU-heavy Chip RAM access. Prefer dirty rectangles, fewer bitplanes where possible, smaller buffers, blitter scheduling, and asset streaming.
- Audio: Paula sample data must be in first-512 KB OCS-safe Chip RAM. Check sample alignment, replay compatibility with 68000/Kickstart 1.x, and total chip footprint.
- Testing: use OCS, exact 68000, 512 KB Chip RAM, optional 512 KB slow RAM, no fast RAM, Kickstart 1.3, PAL and NTSC variants, cycle-exact mode, and immediate blitter disabled.

## Implemented Gameplay
- Sliding block crush core loop with combo scoring.
- Two enemy archetypes (`CHASER` + `WANDERER`) with round-based mix.
- Lives, death/respawn flow, round clear and progression.
- Roguelite run layer:
  - perk draft between rounds (choose 1 of 3),
  - weighted anti-repeat perk offers (less immediate duplicate drafts),
  - run modifiers (extra life, extra round time, score multiplier, enemy slow),
  - run shard currency and banked meta shards on run reset/game over,
  - pre-run shard-spend upgrade pick on new runs (life/time/score/slow options).
- Stage mutator events per round (deterministic draft with anti-repeat cooldowns):
  - `SWARM` (enemy pressure spike),
  - `SHATTERED ICE` (ice -> cracked conversions),
  - `BLACK ICE` (extra block injection into open lanes),
  - `TURBO SLIDE` (faster block movement and reduced push windup).
- Special block alignment bonus event (score/time boost).
- Bonus items that spawn from crush streaks and expire on timer.
- Timer danger-state pulsing near timeout.
- Floating combo score popups.
- Outlined/clamped popup rendering for better readability.
- Lightweight core-event driven SFX hooks (optional SDL queued-audio beeps).

## Build
```powershell
cmake -S . -B build
cmake --build build --config Release
```

Run:
```powershell
.\build\Release\icepanic_sdl.exe
```
Lower-flash capture/testing mode:
```powershell
.\build\Release\icepanic_sdl.exe --low-intensity-viz
```
If you build without multi-config generators:
```powershell
.\build\icepanic_sdl.exe
```

Autoplay fun-proxy evaluator:
```powershell
.\build\Release\icepanic_fun_agent.exe --runs 24 --ticks 18000 --seed 1337
```
Profile-specific evaluation:
```powershell
.\build\Release\icepanic_fun_agent.exe --runs 24 --ticks 18000 --seed 1337 --profile aggressive
```
Available profiles: `arcade`, `casual`, `survival`, `aggressive`.
Machine-readable output:
```powershell
.\build\Release\icepanic_fun_agent.exe --runs 24 --ticks 18000 --seed 1337 --profile arcade --out-format json
.\build\Release\icepanic_fun_agent.exe --runs 24 --ticks 18000 --seed 1337 --profile arcade --out-format csv
```

Batch sweep helper (profile/seed matrix -> one CSV):
```powershell
python tools/run_fun_agent_sweep.py --runs 24 --ticks 18000 --seed-count 8 --output assets/fun_agent_sweep.csv
```
Optional JSON summary (CI-friendly):
```powershell
python tools/run_fun_agent_sweep.py --runs 24 --ticks 18000 --seed-count 8 --output assets/fun_agent_sweep.csv --summary-json assets/fun_agent_sweep_summary.json
```
Custom profiles/seeds example:
```powershell
python tools/run_fun_agent_sweep.py --profiles arcade,aggressive --seeds 1337:1340,1401 --output assets/fun_agent_sweep_custom.csv
```

Compare two sweep outputs (CSV or summary JSON):
```powershell
python tools/compare_fun_agent_sweeps.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv
```
CI-style regression gate:
```powershell
python tools/compare_fun_agent_sweeps.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --fail-on-regression --overall-threshold 0.0 --profile-threshold -5.0
```
Trend history helper (rolling window stability by profile):
```powershell
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.csv --window 6 --out-json assets/fun_agent_trend.json --out-csv assets/fun_agent_trend.csv
```
CI gate example (fail if any profile is unstable under current thresholds):
```powershell
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.csv --window 6 --fail-on-unstable --max-unstable-profiles 0
```
PR/CI compact summary with sparkline history:
```powershell
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.csv --window 6 --pr-summary --pr-summary-include-reasons
```
PR markdown block (ready to paste directly into GitHub comments):
```powershell
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.csv --window 6 --pr-markdown --pr-summary-include-reasons
```
Combined compare+trend markdown helper (single CI comment artifact):
```powershell
python tools/compare_fun_agent_sweeps.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --out-json assets/fun_agent_delta.json
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_*.csv --window 6 --out-json assets/fun_agent_trend.json
python tools/build_fun_agent_ci_comment.py --delta-json assets/fun_agent_delta.json --trend-json assets/fun_agent_trend.json --output assets/fun_agent_ci_comment.md
```
Optional per-profile text badges in markdown rows:
```powershell
python tools/build_fun_agent_ci_comment.py --delta-json assets/fun_agent_delta.json --trend-json assets/fun_agent_trend.json --badge-mode text --badge-delta-warn-threshold -0.5 --badge-delta-fail-threshold -2.0 --output assets/fun_agent_ci_comment.md
```
Custom markdown title:
```powershell
python tools/build_fun_agent_ci_comment.py --delta-json assets/fun_agent_delta.json --trend-json assets/fun_agent_trend.json --title "Icepanic Balance Bot" --output assets/fun_agent_ci_comment.md
```
One-command wrapper for compare+trend+comment:
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets
```
Wrapper with row badges enabled:
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets --badge-mode text
```
Wrapper with custom markdown heading:
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets --markdown-title "Icepanic Balance Bot"
```
Wrapper with compact summary JSON artifact:
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets --badge-mode text --summary-json-name fun_agent_ci_summary.json
```
Quiet CI-log mode:
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets --quiet-subtools
```
Artifact-paths-only mode (for script consumption):
```powershell
python tools/run_fun_agent_ci_report.py --baseline assets/fun_agent_sweep_prev.csv --candidate assets/fun_agent_sweep_new.csv --trend-inputs assets/fun_agent_sweep_*.csv --window 6 --output-dir assets --quiet-subtools --paths-only
```
You can also point it at summary JSON snapshots:
```powershell
python tools/profile_fun_agent_history.py --inputs assets/fun_agent_sweep_summary_*.json --window 6
```

## Controls
- Move: Arrow keys or `WASD`
- Start/skip intro: `Space` or `Enter`
- Perk select: Arrow keys / `WASD`, confirm with `Space`/`Enter`
- Meta upgrade select (new run): Arrow keys / `WASD`, confirm with `Space`/`Enter`
- Quit: `Esc`

## Roguelite Persistence
- Banked meta shards are saved to:
  - `assets/meta_shards.sav` (with `../` and `../../` fallback search/write paths).
- Meta shards are saved on both bank (`GAME_EVENT_META_BANKED`) and spend (`GAME_EVENT_META_SPENT`).

## Frame Dump (Indexed)
For Amiga-style pipeline prototyping, dump the 8-bit indexed framebuffer directly:
```powershell
.\build\Release\icepanic_sdl.exe --dump-indexed assets\frame_dump --dump-max-frames 300
```
Writes:
- `palette.rgb` (`32 * RGB` bytes)
- `frame_00000.idx` ... raw `320*200` index8 frames
- `frame_info.txt`

Convert dumped index8 frames to Amiga-style 5-bitplane output:
```powershell
python tools/convert_frame_dump_to_bitplanes.py --input-dir assets\frame_dump --output-dir assets\frame_dump_5bpl
```
Writes:
- `frame_00000.bpl` ... planar frames (`40000` bytes each at `320x200x5bpl`)
- `palette_12bit.bin` / `palette_12bit.txt`
- `conversion_info.txt`

Optional crop mode for partial-screen blit benchmarking:
```powershell
python tools/convert_frame_dump_to_bitplanes.py --input-dir assets\frame_dump --output-dir assets\frame_dump_crop --crop-x 16 --crop-y 16 --crop-width 288 --crop-height 160
```

One-shot packed Amiga bundle for loader tests:
```powershell
python tools/build_amiga_resource_bundle.py --input-dir assets\frame_dump --output assets\amiga_frame_bundle.iab --info assets\amiga_frame_bundle.txt
```
Optional delta-XOR frame packing:
```powershell
python tools/build_amiga_resource_bundle.py --input-dir assets\frame_dump --output assets\amiga_frame_bundle_delta.iab --delta-xor
```

Inspect/reconstruct frames from an `IAB1` bundle with the C loader demo:
```powershell
.\build\Release\icepanic_iab_loader_demo.exe assets\amiga_frame_bundle.iab 0 assets\frame0_from_bundle.bpl
```

Parity-check bundle reconstruction against source index dumps:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump
```
Optional report output for CI artifacts:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --report assets\amiga_parity_report.txt
```
Compact report body (single-line summary + optional mismatch samples):
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --report assets\amiga_parity_report_compact.txt --report-format compact
```
Cap mismatch lines in console/report output (useful for failure spam control):
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --report assets\amiga_parity_report.txt --report-max-mismatches 20
```
Machine-readable script mode output:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --script
```
JSON summary mode output:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --json
```
Compact PR-friendly summary output:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --pr-summary
```
Optional frame sampling in parity checks:
```powershell
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump --start-frame 0 --frame-step 4 --max-frames 20
```

End-to-end sample (dump -> bundle -> inspect -> parity check):
```powershell
.\build\Release\icepanic_sdl.exe --dump-indexed assets\frame_dump --dump-max-frames 120
python tools/build_amiga_resource_bundle.py --input-dir assets\frame_dump --output assets\amiga_frame_bundle.iab
.\build\Release\icepanic_iab_loader_demo.exe assets\amiga_frame_bundle.iab 0 assets\frame0_from_bundle.bpl
python tools/check_iab_parity.py --bundle assets\amiga_frame_bundle.iab --input-dir assets\frame_dump
```

Headless deterministic dump generation (for regression/CI without opening SDL):
```powershell
python tools/generate_deterministic_frame_dump.py --output-dir assets\frame_dump_synth --frames 120 --seed 1337
```

One-command smoke for deterministic dump -> bundle -> parity (plane-major + row-interleaved, raw + delta):
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1
```
Faster sampled smoke lane example:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -SampleStep 4 -MaxCompareFrames 12
```
JSON-mode smoke output example:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -ParityMode json -SampleStep 4 -MaxCompareFrames 12
```
PR-summary smoke output example:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -ParityMode pr-summary -SampleStep 4 -MaxCompareFrames 12
```
Compact smoke report files example:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -ReportFormat compact -SampleStep 4 -MaxCompareFrames 12
```
Smoke with mismatch cap passed through to parity:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -ReportFormat compact -ReportMaxMismatches 20
```
Keep generated artifacts with a manifest file:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -KeepArtifacts
```
Keep artifacts with JSON manifest:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -KeepArtifacts -ArtifactManifestFormat json
```
Custom manifest output path:
```powershell
powershell -ExecutionPolicy Bypass -File .\tools\run_amiga_pipeline_smoke.ps1 -KeepArtifacts -ArtifactManifestPath assets\my_smoke_manifest.txt
```
The smoke script prints per-artifact SHA-256 hashes, one aggregate report checksum line, and a manifest payload checksum (`HASH_MANIFEST_PAYLOAD`) that excludes checksum self-reference.

Validate planar outputs (tile strip and optional frame):
```powershell
python tools/validate_planar_assets.py --frame-bpl assets\frame_dump_5bpl\frame_00000.bpl
```

## Tests
```powershell
ctest --test-dir build --output-on-failure
```

## Asset Pipeline
Generate the runtime tileset from handcrafted pixel art with:
```powershell
python tools/build_tileset.py
```
The generator creates all runtime tiles directly (no concept-sheet sampling) while keeping the 32-color indexed format.
Art is consolidated into one canonical atlas image.

This writes:
- `assets/tileset.bin`
- `assets/tileset_manifest.txt`
- `assets/sprite_atlas_16.png` (single consolidated `16x16` sprite/tile atlas)
- `assets/tile_strip_16x16_5bpl.bin` (tile-major, plane-major Amiga strip)
- `assets/tile_strip_16x16_5bpl.txt`

Optional tile-strip byte layout variant:
```powershell
python tools/build_tileset.py --tile-strip-layout row-interleaved
```

If `assets/tileset.bin` is missing, the SDL client falls back to built-in placeholder tiles.

## Amiga OCS Hardware Port
The Amiga port is a separate vbcc/Kickstart 1.3 build. It does not use CMake and does not affect the PC/SDL binaries.

Prerequisites:
- `VBCC` points at the Amiga vbcc install, for example `C:\Users\paula\amiga-dev`
- `%VBCC%\bin` is on `PATH`
- Python is available for asset generation
- `xdftool` from `amitools` is on `PATH` for ADF packaging (`python -m pip install amitools`)

Generate the Amiga ECS reference assets and the strict-OCS external graphics/SFX payloads from the canonical assets/PC SFX routines:
```powershell
python tools/build_amiga_bob_assets.py
python tools/build_amiga_sfx_assets.py
python tools/build_amiga_assets_ocs.py
python tools/build_amiga_sfx_assets_ocs.py
```

Validate the generated palette/BOB/mask data without writing files:
```powershell
python tools/build_amiga_bob_assets.py --validate-only
```

Build the Kickstart 1.3-compatible Amiga executable and bootable ADF with vbcc:
```powershell
make -f Makefile.amiga
```

The default Amiga build is now the strict OCS PAL beta. It uses vbcc's 16-bit-int Kickstart 1.3 target (`+kick13s`), 50 Hz fixed gameplay, 320x200 5bpl bitplanes, blitter BOB gameplay, OCS title hardware sprites, PAL copper effects, and external `Icepanic.gfx`/`Icepanic.sfx` payloads loaded into validated below-`$00080000` DMA pools.

This writes:
- `build/amiga/icepanic-v0.9.1-beta.2-amiga-ocs-pal` - Amiga hunk executable for PAL OCS/Kickstart 1.3
- `build/amiga/icepanic-v0.9.1-beta.2-amiga-ocs-pal.adf` - bootable DOS0/OFS disk image for WinUAE or a real Amiga
- `build/amiga/*.o` - intermediate vbcc objects

In WinUAE, map your joystick or keyboard joystick to Amiga port 2. Port 1 is left for the mouse and is ignored by gameplay input.

If `make` is not installed, run the equivalent PowerShell build directly:
```powershell
$ndk = Join-Path $env:VBCC 'NDK_1.3\INCLUDE-STRIP1.3\INCLUDE.H'
New-Item -ItemType Directory -Force -Path build\amiga | Out-Null
python tools/build_amiga_bob_assets.py
python tools/build_amiga_sfx_assets.py
python tools/build_amiga_assets_ocs.py
python tools/build_amiga_sfx_assets_ocs.py
vc +kick13s -O2 -DICEPANIC_AMIGA_SMALL_STACK -DICEPANIC_AMIGA_OCS_FIXED_LEVELS -Isrc/core -Isrc/platform_amiga "-I$ndk" -c src/core/game.c -o build/amiga/game_ocs_beta.o
vc +kick13 -c99 -O2 -Isrc/core -Isrc/platform_amiga "-I$ndk" -c src/platform_amiga/amiga_assets_ocs.c -o build/amiga/amiga_assets_ocs_beta.o
vc +kick13 -c99 -O2 -Isrc/core -Isrc/platform_amiga "-I$ndk" -c src/platform_amiga/amiga_sfx_assets_ocs.c -o build/amiga/amiga_sfx_assets_ocs_beta.o
vc +kick13s -O2 -Isrc/core -Isrc/platform_amiga "-I$ndk" -c src/platform_amiga/ice_panic_ocs.c -o build/amiga/ice_panic_ocs.o
vc +kick13s build/amiga/game_ocs_beta.o build/amiga/amiga_assets_ocs_beta.o build/amiga/amiga_sfx_assets_ocs_beta.o build/amiga/ice_panic_ocs.o -o build/amiga/icepanic-v0.9.1-beta.2-amiga-ocs-pal
python tools/build_amiga_adf.py --exe build/amiga/icepanic-v0.9.1-beta.2-amiga-ocs-pal --output build/amiga/icepanic-v0.9.1-beta.2-amiga-ocs-pal.adf --gfx src/platform_amiga/amiga_assets_ocs.bin --sfx src/platform_amiga/amiga_sfx_assets_ocs.bin
```

The ADF is bootable and runs `SYS:Icepanic` from `S/startup-sequence`.

This target builds the `src/platform_amiga/ice_panic_ocs.c` hardware path for PAL OCS Amiga machines. It uses a 320x200 5-bitplane display, renders gameplay actors as blitter BOBs except for the non-red hardware-sprite enemy test, consumes core dirty cell flags directly, uses OCS hardware sprites for title sparkles, and does not use C2P, ECS-only registers, or AGA features.
