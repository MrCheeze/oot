/*
 * File: z_eff_ss_dead_sound.c
 * Overlay: ovl_Effect_Ss_Dead_Sound
 * Description: Plays a sound effect
 */

#include "z_eff_ss_dead_sound.h"
#include "sfx.h"
#include "z64effect.h"
#include "z64play.h"

#define rSfxId regs[10]
#define rRepeatMode regs[11] // sound is replayed every update. unused in the original game

u32 Effect_Ss_Dead_Sound_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void Effect_sound_func_proc(PlayState* play, u32 index, EffectSs* this);

EffectSsProfile Effect_Ss_Dead_Sound_Profile = {
    EFFECT_SS_DEAD_SOUND,
    Effect_Ss_Dead_Sound_ct,
};

u32 Effect_Ss_Dead_Sound_ct(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsDeadSoundInitParams* initParams = (EffectSsDeadSoundInitParams*)initParamsx;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->accel = initParams->accel;
    this->flags = 2;
    this->life = initParams->life;
    this->draw = NULL;
    this->update = Effect_sound_func_proc;
    this->rRepeatMode = initParams->repeatMode;
    this->rSfxId = initParams->sfxId;
    PRINTF("コンストラクター3\n"); // "constructor 3"

    return 1;
}

void Effect_sound_func_proc(PlayState* play, u32 index, EffectSs* this) {
    switch (this->rRepeatMode) {
        case DEADSOUND_REPEAT_MODE_OFF:
            this->rRepeatMode--; // decrement to 0 so sound only plays once
            break;
        case DEADSOUND_REPEAT_MODE_ON:
            break;
        default:
            return;
    }

    Nai_FxFlagEntry(this->rSfxId, &this->pos, 4, &_dummy_one, &_dummy_one,
                         &_dummy_zero_s8);
}
