#include "global.h"
#include "region.h"
#include "versions.h"

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ntsc-1.0:176 ntsc-1.1:176 ntsc-1.2:176 pal-1.0:160 pal-1.1:160 hiratsu3:128"

ALIGNED(16) SaveContext z_common_data;
#if PLATFORM_IQUE
// Unknown bss padding, placed here for matching
char sUnknownBssPadding[0x20];
#endif
u32 z_common_data__newer_member;
u32 z_common_data__hayakawa_mode;

void z_common_data_init(void) {
    bzero(&z_common_data, sizeof(z_common_data));
    z_common_data__newer_member = 0;
    z_common_data__hayakawa_mode = 0;
    z_common_data.seqId = (u8)NA_BGM_DISABLED;
    z_common_data.natureAmbienceId = NATURE_ID_DISABLED;
    z_common_data.forcedSeqId = NA_BGM_GENERAL_SFX;
    z_common_data.nextCutsceneIndex = 0xFFEF;
    z_common_data.cutsceneTrigger = 0;
    z_common_data.chamberCutsceneNum = CHAMBER_CS_FOREST;
    z_common_data.nextDayTime = NEXT_TIME_NONE;
    z_common_data.skyboxTime = 0;
    z_common_data.dogIsLost = true;
    z_common_data.nextTransitionType = TRANS_NEXT_TYPE_DEFAULT;
    z_common_data.prevHudVisibilityMode = HUD_VISIBILITY_ALL;
#if OOT_NTSC && OOT_VERSION < GC_US || PLATFORM_IQUE || OOT_VERSION == HIRATSU3
    if (z_locale_mode == REGION_JP) {
        z_common_data.language = LANGUAGE_JPN;
    }
    if (z_locale_mode == REGION_US) {
        z_common_data.language = LANGUAGE_ENG;
    }
#elif OOT_VERSION == GC_US || OOT_VERSION == GC_US_MQ
    z_common_data.language = LANGUAGE_ENG;
#elif OOT_VERSION == GC_JP_CE
    z_common_data.language = LANGUAGE_JPN;
#endif
}
