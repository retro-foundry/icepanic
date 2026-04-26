# Changelog

## v0.9.0-beta.4

Fourth Amiga ECS beta release candidate.

- Fixed a timeout edge case where respawn invulnerability could leave the game stuck at zero time.
- Regenerated the current map when the player dies because time runs out.
- Added PAL copper presentation polish: icy reflection, slow reflection wave, and gameplay scanline sparkles.

## v0.9.0-beta.3

Third Amiga ECS beta release candidate.

- Added short Paula tracker-style intro music for the cracktro screen.
- Fixed title branding to Retro Foundry in the Amiga intro.
- Improved player respawn safety so respawns prefer genuinely open tiles.
- Continued Amiga gameplay, high-score, enemy, menu, sound, and performance polish.

## v0.9.0-beta.2

Second Amiga ECS beta release candidate.

- Added Amiga cracktro-style intro with copper wave and fire-to-start hold.
- Added release NFO packaging into the bootable ADF.
- Included Retro Foundry Patreon and project URLs in release materials.
- Continued Amiga gameplay, menu, audio, spawn-safety, and performance polish.

## v0.9.0-beta.1

First user-facing beta release candidate for the Amiga ECS port.

- Added PAL ECS/Kickstart 1.3 Amiga build and bootable ADF output.
- Added 320x200 5bpl Amiga hardware renderer with blitter BOB gameplay rendering.
- Added Paula sample playback generated from the PC SFX routines.
- Added high score entry/table flow on Amiga.
- Added versioned release artifact naming: `icepanic-v0.9.0-beta.1-<platform>`.
- Kept PC/SDL build and deterministic core tests working.

Known focus areas before `v1.0.0`:

- More cycle-exact A500/ECS performance testing on real hardware and WinUAE.
- Menu responsiveness polish on Amiga.
- Wider compatibility testing for joystick/fire handling and display restore.
