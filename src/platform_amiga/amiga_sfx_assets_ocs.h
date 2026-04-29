#ifndef ICEPANIC_AMIGA_SFX_ASSETS_OCS_H
#define ICEPANIC_AMIGA_SFX_ASSETS_OCS_H

#include "amiga_sfx_assets.h"

#define AMIGA_SFX_OCS_DATA_BYTES 29640UL
#define AMIGA_SFX_OCS_SAMPLE_RATE 6000
#define AMIGA_SFX_OCS_SAMPLE_PERIOD 591

extern const ULONG g_amiga_sfx_ocs_offsets[AMIGA_SFX_SAMPLE_COUNT];
extern const UWORD g_amiga_sfx_ocs_lengths_words[AMIGA_SFX_SAMPLE_COUNT];
extern const UBYTE g_amiga_sfx_ocs_ticks[AMIGA_SFX_SAMPLE_COUNT];

#endif
