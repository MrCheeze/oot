#include "z64sfx_source.h"

#include "global.h"

void Effect_SE_Info_ct(PlayState* play) {
    SfxSource* sources = &play->sfxSources[0];
    s32 i;

    // clang-format off
    for (i = 0; i < ARRAY_COUNT(play->sfxSources); i++) { sources[i].countdown = 0; }
    // clang-format on
}

void Effect_SE_Info_proc(PlayState* play) {
    SfxSource* source = &play->sfxSources[0];
    s32 i;

    for (i = 0; i < ARRAY_COUNT(play->sfxSources); i++) {
        if (source->countdown != 0) {
            if (DECR(source->countdown) == 0) {
                Nai_StopAllObjFx(&source->projectedPos);
            } else {
                Skin_Matrix_MulVector(&play->viewProjectionMtxF, &source->worldPos, &source->projectedPos);
            }
        }

        source++;
    }
}

void Effect_SE_Info_new(PlayState* play, Vec3f* worldPos, s32 duration, u16 sfxId) {
    s32 countdown;
    SfxSource* source;
    s32 smallestCountdown = 0xFFFF;
    SfxSource* backupSource;
    s32 i;

    source = &play->sfxSources[0];
    for (i = 0; i < ARRAY_COUNT(play->sfxSources); i++) {
        if (source->countdown == 0) {
            break;
        }

        // Store the sfx source with the smallest remaining countdown
        countdown = source->countdown;
        if (countdown < smallestCountdown) {
            smallestCountdown = countdown;
            backupSource = source;
        }
        source++;
    }

    // If no sfx source is available, replace the sfx source with the smallest remaining countdown
    if (i >= ARRAY_COUNT(play->sfxSources)) {
        source = backupSource;
        Nai_StopAllObjFx(&source->projectedPos);
    }

    source->worldPos = *worldPos;
    source->countdown = duration;

    Skin_Matrix_MulVector(&play->viewProjectionMtxF, &source->worldPos, &source->projectedPos);
    Nai_FxFlagEntry(sfxId, &source->projectedPos, 4, &_dummy_one, &_dummy_one,
                         &_dummy_zero_s8);
}
