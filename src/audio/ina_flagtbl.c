#include "ultra64.h"
#include "global.h"
#include "versions.h"

#define DEFINE_SFX(_0, _1, importance, distParam, randParam, flags)            \
    { importance, ((((distParam) << SFX_PARAM_01_SHIFT) & SFX_PARAM_01_MASK) | \
                   (((randParam) << SFX_PARAM_67_SHIFT) & SFX_PARAM_67_MASK) | (flags)) },

#include "mae_flagtbl.inc.c"

SfxParams PlayerInfo[] = {
#include "tables/sfx/playerbank_table.h"
};

SfxParams ItemInfo[] = {
#include "tables/sfx/itembank_table.h"
};

SfxParams EnvInfo[] = {
#include "tables/sfx/environmentbank_table.h"
};

SfxParams SystemInfo[] = {
#include "tables/sfx/systembank_table.h"
};

SfxParams OcarinaInfo[] = {
#include "tables/sfx/ocarinabank_table.h"
};

SfxParams VoiceInfo[] = {
#include "tables/sfx/voicebank_table.h"
};

#undef DEFINE_SFX

SfxParams* se_indi_info_adrs[7] = {
    PlayerInfo, ItemInfo,    EnvInfo,   EnemyInfo,
    SystemInfo, OcarinaInfo, VoiceInfo,
};
