# Prompt Templates

## Full character animation sheet

```text
Create a single pixel-art sprite sheet for [CHARACTER].

Layout: [ROWS] rows by [COLUMNS] columns, equal cells, even spacing, stable baseline, no labels.
Rows:
1. Idle/front-facing animation, [N] subtle frames.
2. Walk right, [N] side-view frames.
3. Walk up, [N] back-view frames.
4. Walk down, [N] front-facing movement frames.

Keep the same character design in every frame: [COLORS], [PROPORTIONS], [KEY FEATURES].
Make every frame game-ready and aligned: same scale, same feet/contact line, centered in each cell.

Style: crisp retro pixel art, limited [PALETTE SIZE]-colour feel, readable silhouettes, hand-placed highlights, subtle dithering, no blur, no painterly rendering.
Background: [BLACK / TRANSPARENT / CHROMA KEY].
No extra objects, no text, no labels, no UI, no frame numbers.
```

## Repaint an existing sheet

```text
Edit Image A directly. Image A is the content authority.

Repaint the sprite sheet in a cleaner [STYLE] pixel-art style while preserving the exact layout:
- same canvas shape
- same rows and columns
- same sprite positions
- same occupied cells
- same approximate sprite sizes
- same black/transparent background

This is a repaint, not a redesign. Do not add, remove, merge, or shuffle sprites.
Improve silhouette, outline, cluster cleanliness, limited-palette shading, and readability.
No text, labels, or new objects.
```

## Focused walk-cycle fix

```text
Edit only the [DIRECTION] walk row. Preserve all other rows and sprites exactly.

Fix the [DIRECTION] walk cycle so each frame is readable and game-ready:
- clear facing direction
- alternating feet
- stable baseline
- consistent proportions
- natural flipper/arm swing
- slight body bob without changing scale
- no frame drift

Keep the character design, colors, and pixel-art style consistent with the rest of the sheet.
```

## Individual frame request

```text
Create a single isolated pixel-art animation frame for [CHARACTER], [ANIMATION], frame [INDEX] of [COUNT], facing [DIRECTION].

The frame must match the same character design, scale, palette, and baseline as the rest of the set.
Pose: [CONTACT / PASSING / RECOIL / OPPOSITE CONTACT].
Background: transparent or solid chroma key.
No extra objects, no labels, no border.
```
