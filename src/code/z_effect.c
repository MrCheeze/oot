#include "global.h"

#include "z64frame_advance.h"

EffectContext EffectMoveArea;

EffectInfo EffectTypeInfoTable[] = {
    {
        sizeof(EffectSpark),
        EffectSparkInfo_ct,
        EffectSparkInfo_dt,
        EffectSparkInfo_proc,
        EffectSparkInfo_disp,
    },
    {
        sizeof(EffectBlure),
        EffectBlureInfo_ct,
        EffectBlureInfo_dt,
        EffectBlureInfo_proc,
        EffectBlureInfo_disp,
    },
    {
        sizeof(EffectBlure),
        EffectBlureInfo_v1_ct,
        EffectBlureInfo_dt,
        EffectBlureInfo_proc,
        EffectBlureInfo_disp,
    },
    {
        sizeof(EffectShieldParticle),
        EffectShieldParticle_ct,
        EffectShieldParticle_dt,
        EffectShieldParticle_proc,
        EffectShieldParticle_disp,
    },
};

PlayState* Effect_GetGamePointer(void) {
    return EffectMoveArea.play;
}

void* Effect_GetEffectMemoryPointer(s32 index) {
    if (index == TOTAL_EFFECT_COUNT) {
        return NULL;
    }

    if (index < SPARK_COUNT) {
        if (EffectMoveArea.sparks[index].status.active == true) {
            return &EffectMoveArea.sparks[index].effect;
        } else {
            return NULL;
        }
    }

    index -= SPARK_COUNT;
    if (index < BLURE_COUNT) {
        if (EffectMoveArea.blures[index].status.active == true) {
            return &EffectMoveArea.blures[index].effect;
        } else {
            return NULL;
        }
    }

    index -= BLURE_COUNT;
    if (index < SHIELD_PARTICLE_COUNT) {
        if (EffectMoveArea.shieldParticles[index].status.active == true) {
            return &EffectMoveArea.shieldParticles[index].effect;
        } else {
            return NULL;
        }
    }

    return NULL;
}

void EffectBase_ct(EffectStatus* status) {
    status->active = false;
    status->unk_01 = 0;
    status->unk_02 = 0;
}

void EffectInit(PlayState* play) {
    s32 i;

    for (i = 0; i < SPARK_COUNT; i++) {
        EffectBase_ct(&EffectMoveArea.sparks[i].status);
    }

    for (i = 0; i < BLURE_COUNT; i++) {
        EffectBase_ct(&EffectMoveArea.blures[i].status);
    }

    for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
        //! @bug This is supposed to initialize shieldParticles, not blures again
        EffectBase_ct(&EffectMoveArea.blures[i].status);
    }

    EffectMoveArea.play = play;
}

void EffectAdd(PlayState* play, s32* pIndex, s32 type, u8 arg3, u8 arg4, void* initParams) {
    s32 i;
    u32 slotFound;
    void* effect = NULL;
    EffectStatus* status = NULL;

    *pIndex = TOTAL_EFFECT_COUNT;

    if (_Game_play_isPause(play) != true) {
        slotFound = false;
        switch (type) {
            case EFFECT_SPARK:
                for (i = 0; i < SPARK_COUNT; i++) {
                    if (!EffectMoveArea.sparks[i].status.active) {
                        slotFound = true;
                        *pIndex = i;
                        effect = &EffectMoveArea.sparks[i].effect;
                        status = &EffectMoveArea.sparks[i].status;
                        break;
                    }
                }
                break;
            case EFFECT_BLURE1:
            case EFFECT_BLURE2:
                for (i = 0; i < BLURE_COUNT; i++) {
                    if (!EffectMoveArea.blures[i].status.active) {
                        slotFound = true;
                        *pIndex = i + SPARK_COUNT;
                        effect = &EffectMoveArea.blures[i].effect;
                        status = &EffectMoveArea.blures[i].status;
                        break;
                    }
                }
                break;
            case EFFECT_SHIELD_PARTICLE:
                for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
                    if (!EffectMoveArea.shieldParticles[i].status.active) {
                        slotFound = true;
                        *pIndex = i + SPARK_COUNT + BLURE_COUNT;
                        effect = &EffectMoveArea.shieldParticles[i].effect;
                        status = &EffectMoveArea.shieldParticles[i].status;
                        break;
                    }
                }
                break;
        }

        if (!slotFound) {
            PRINTF(T("EffectAdd():確保できません。注意してください。Type%d\n",
                     "EffectAdd(): I cannot secure it. Be careful. Type %d\n"),
                   type);
            PRINTF(T("エフェクト追加せずに終了します。\n", "Exit without adding the effect.\n"));
        } else {
            EffectTypeInfoTable[type].init(effect, initParams);
            status->unk_02 = arg3;
            status->unk_01 = arg4;
            status->active = true;
        }
    }
}

void EffectDisp(GraphicsContext* gfxCtx) {
    s32 i;

    for (i = 0; i < SPARK_COUNT; i++) {
        if (!EffectMoveArea.sparks[i].status.active) {
            continue;
        }
        EffectTypeInfoTable[EFFECT_SPARK].draw(&EffectMoveArea.sparks[i].effect, gfxCtx);
    }

    for (i = 0; i < BLURE_COUNT; i++) {
        if (!EffectMoveArea.blures[i].status.active) {
            continue;
        }
        EffectTypeInfoTable[EFFECT_BLURE1].draw(&EffectMoveArea.blures[i].effect, gfxCtx);
    }

    for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
        if (!EffectMoveArea.shieldParticles[i].status.active) {
            continue;
        }
        EffectTypeInfoTable[EFFECT_SHIELD_PARTICLE].draw(&EffectMoveArea.shieldParticles[i].effect, gfxCtx);
    }
}

void EffectProc(PlayState* play) {
    s32 i;

    for (i = 0; i < SPARK_COUNT; i++) {
        if (EffectMoveArea.sparks[i].status.active) {
            if (EffectTypeInfoTable[EFFECT_SPARK].update(&EffectMoveArea.sparks[i].effect) == 1) {
                EffectFreeIndex(play, i);
            }
        }
    }

    for (i = 0; i < BLURE_COUNT; i++) {
        if (EffectMoveArea.blures[i].status.active) {
            if (EffectTypeInfoTable[EFFECT_BLURE1].update(&EffectMoveArea.blures[i].effect) == 1) {
                EffectFreeIndex(play, i + SPARK_COUNT);
            }
        }
    }

    for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
        if (EffectMoveArea.shieldParticles[i].status.active) {
            if (EffectTypeInfoTable[EFFECT_SHIELD_PARTICLE].update(&EffectMoveArea.shieldParticles[i].effect) == 1) {
                EffectFreeIndex(play, i + SPARK_COUNT + BLURE_COUNT);
            }
        }
    }
}

void EffectFreeIndex(PlayState* play, s32 index) {
    if (index == TOTAL_EFFECT_COUNT) {
        return;
    }

    if (index < SPARK_COUNT) {
        EffectMoveArea.sparks[index].status.active = false;
        EffectTypeInfoTable[EFFECT_SPARK].destroy(&EffectMoveArea.sparks[index].effect);
        return;
    }

    index -= SPARK_COUNT;
    if (index < BLURE_COUNT) {
        EffectMoveArea.blures[index].status.active = false;
        EffectTypeInfoTable[EFFECT_BLURE1].destroy(&EffectMoveArea.blures[index].effect);
        return;
    }

    index -= BLURE_COUNT;
    if (index < SHIELD_PARTICLE_COUNT) {
        EffectMoveArea.shieldParticles[index].status.active = false;
        EffectTypeInfoTable[EFFECT_SHIELD_PARTICLE].destroy(&EffectMoveArea.shieldParticles[index].effect);
        return;
    }
}

void EffectFreeAllIndex(PlayState* play) {
    s32 i;

    PRINTF(T("エフェクト総て解放\n", "All effects release\n"));

    for (i = 0; i < SPARK_COUNT; i++) {
        EffectMoveArea.sparks[i].status.active = false;
        EffectTypeInfoTable[EFFECT_SPARK].destroy(&EffectMoveArea.sparks[i].effect);
    }

    for (i = 0; i < BLURE_COUNT; i++) {
        EffectMoveArea.blures[i].status.active = false;
        EffectTypeInfoTable[EFFECT_BLURE1].destroy(&EffectMoveArea.blures[i].effect);
    }

    for (i = 0; i < SHIELD_PARTICLE_COUNT; i++) {
        EffectMoveArea.shieldParticles[i].status.active = false;
        EffectTypeInfoTable[EFFECT_SHIELD_PARTICLE].destroy(&EffectMoveArea.shieldParticles[i].effect);
    }

    PRINTF(T("エフェクト総て解放 終了\n", "All effects release End\n"));
}
