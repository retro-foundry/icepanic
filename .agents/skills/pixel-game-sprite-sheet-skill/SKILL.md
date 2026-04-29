---
name: pixel-game-sprite-sheet
summary: Create, refine, align, and export usable retro pixel-art game spritesheets and animation frames from image generation outputs.
version: 1.0.0
---

# Pixel Game Sprite Sheet Skill

Use this skill when the user wants game-ready pixel-art sprites, animation frames, sprite sheets, or a repaint of an existing sheet. It is especially useful for retro console, Amiga, arcade, C64, Spectrum, Game Boy Advance, or 16-bit style assets.

The main goal is not just to make a pretty image. The goal is to produce **usable game sprites**: consistent character design, fixed cell alignment, stable baseline, clean silhouettes, limited palette, and extractable frames.

## Use cases

Use this skill when the user asks for any of these:

- redraw or repaint a sprite sheet
- make a pixel-art animation sheet
- generate idle, walk, run, jump, attack, damage, death, or pickup frames
- keep frames aligned or preserve a grid
- extract individual frames from a sheet
- convert a generated sheet into game-ready PNG frames
- make retro Amiga-style, 16-bit-style, or old-school arcade sprites
- improve generated sprites that drift, mutate, blur, or lose the player character

## Style safety

If the user names a living artist, do not ask the image model to copy that artist's style. Translate the request into observable traits instead.

Example:

- Avoid: "in Stoo Cambridge's exact style"
- Use: "classic early-1990s British Amiga pixel-art feel: compact readable sprites, bright limited palette, hand-placed highlights, tasteful dithering, playful silhouettes, arcade readability"

## Core workflow

### 1. Confirm the production target internally

Before generating or editing, infer:

- sprite subject: player, enemy, pickup, effect, tile, UI icon
- game perspective: front, side, top-down, three-quarter, isometric
- sheet layout: rows, columns, cell size, spacing, background
- required animations: idle, walk right, walk left, walk up, walk down, attack, etc.
- output format: one sheet, individual frames, contact sheet, GIF preview, transparent PNGs

Ask a follow-up only if the missing information prevents useful output. Otherwise choose sensible defaults.

Default animation layout:

- 4 frames per animation
- consistent cell size
- stable baseline
- black or transparent background
- no labels unless requested

### 2. Make the prompt production-focused

For image generation or image editing, prompts must emphasize:

- exact row and column count
- fixed grid alignment
- equal cell size
- no drifting, no resizing between frames
- same character design in every frame
- clear foot positions and pose changes
- no extra objects, labels, text, UI, watermarks, or merged cells
- crisp hard-edged pixels, no blur, no painterly rendering
- limited palette and classic game-readability

Use this structure:

```text
Create a single pixel-art sprite sheet for [subject].

Layout: [rows] rows by [columns] columns, equal cells, even spacing, stable baseline.
Rows:
1. [animation name], [frame count] frames, [view/direction].
2. [animation name], [frame count] frames, [view/direction].
...

Character/object consistency:
- same proportions, palette, outline, lighting, and scale across all frames
- no added objects or labels
- keep feet/contact line aligned

Style:
- crisp retro pixel art
- limited palette, [platform/style] feel
- readable silhouettes, hand-placed highlights, subtle dithering
- no blur, no smooth modern anti-aliasing
```

### 3. For editing an existing sheet, lock the layout

When the user provides a sheet and wants a repaint, make the existing sheet the **content authority**.

Prompt rules:

- "Edit Image A directly."
- "Preserve exact sprite positions, rows, spacing, cell sizes, and occupied cells."
- "Do not add, remove, merge, or shuffle sprites."
- "Change only [target sprites]" when doing focused fixes.
- "Keep the player sprite / original subject present."

For a full repaint, say:

```text
This is a repaint, not a redesign. Preserve every sprite identity and cell placement, but improve clusters, palette cohesion, highlights, silhouette, and shading.
```

### 4. Evaluate generated sheets like a game artist

After generation, inspect for these failure modes:

- player or important sprite removed
- side-walk frames not readable
- up/down rows confused
- inconsistent character proportions
- feet or baseline shift between frames
- different scales across frames
- frames too close to crop safely
- blurred or painterly pixels
- row/column count wrong
- text, labels, shadows, backgrounds, or UI added unexpectedly
- left/right frames are accidental duplicates or not mirrored clearly

If issues appear, do a focused edit rather than regenerating everything.

Example focused fix:

```text
Edit only row 2, columns 2-4. Improve the side-walk frames. Preserve all other sprites exactly.
```

### 5. Make game-ready files

Use `scripts/sprite_sheet_tools.py` to:

- crop a sheet into frames
- repack frames into a clean sheet
- make a contact sheet
- make an animated GIF preview
- quantize to a fixed number of colors
- optionally remove solid-color backgrounds

Prefer nearest-neighbor scaling for pixel art.

## Recommended animation rows

For a small character sheet:

| Row | Animation | View | Frames |
|---|---|---|---|
| 1 | idle | front | 4 |
| 2 | walk right | side | 4 |
| 3 | walk up | back | 4 |
| 4 | walk down | front | 4 |

Optional rows:

| Row | Animation | View | Frames |
|---|---|---|---|
| 5 | walk left | side mirrored or hand-edited | 4 |
| 6 | attack | direction-specific | 4 |
| 7 | hit / hurt | front or side | 2-4 |
| 8 | death / vanish | any | 4-8 |

## Pixel-art prompt details

Good wording:

- "hard-edged pixel clusters"
- "stable baseline"
- "same sprite scale in every cell"
- "limited 32-colour palette feel"
- "readable at native size"
- "black background for preview" or "transparent background for export"
- "no labels, no frame numbers"
- "game-ready sprite sheet"

Avoid vague wording:

- "make it better"
- "more professional"
- "cooler"
- "HD pixel art"

Replace with concrete requests:

- stronger silhouette
- cleaner outline
- better foot alternation
- more distinct pose arcs
- less blur
- clearer direction
- tighter palette
- consistent lighting

## Handling transparent backgrounds

Image generators may show a checkerboard instead of real alpha. If real transparency is required, use `sprite_sheet_tools.py remove-bg` after generation, or request a solid chroma key background and remove that color.

Best practice:

- Generate on black for visual inspection.
- Generate or post-process to transparent PNG for engine import.
- Keep each frame inside a uniform bounding box.

## Deliverables to offer

Depending on the user's request, provide one or more:

- sprite sheet PNG
- individual frame PNGs
- ZIP of frames
- GIF preview
- JSON metadata with frame rectangles
- Aseprite-friendly layout notes
- Godot/Unity import notes

## Example response language

When successful:

```text
Made a 4x4 sheet: idle, walk right, walk up, walk down. The rows are aligned with a stable baseline and equal spacing.
```

When the model output is not quite right:

```text
The sheet is usable, but row 2 has some foot drift. I would do one focused pass on the walk-right row and preserve everything else.
```
