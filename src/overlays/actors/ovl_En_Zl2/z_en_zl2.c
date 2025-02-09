/*
 * File: z_en_zl2.c
 * Overlay: ovl_En_Zl2
 * Description: Adult Zelda (Cutscenes)
 */

#include "z_en_zl2.h"
#include "terminal.h"

#include "z64frame_advance.h"

#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "assets/objects/object_zl2/object_zl2.h"
#include "assets/objects/object_zl2_anime1/object_zl2_anime1.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Zl2_Actor_ct(Actor* thisx, PlayState* play);
void En_Zl2_Actor_dt(Actor* thisx, PlayState* play);
void En_Zl2_Actor_main(Actor* thisx, PlayState* play);
void En_Zl2_Actor_draw(Actor* thisx, PlayState* play);

s32 En_Zl2_SetWaveHear(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx);

void En_Zl2_inAgain_main_wait(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_greet(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_approach(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_confess(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_gloom(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_waver(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_forward(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_encourage(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_handup(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_cheer(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_give(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_effect(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_gaze(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_turn(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_around(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_raise(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_attention(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_arrest(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_prison(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_collapse(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_faint(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_fly(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_fade(EnZl2* this, PlayState* play);
void En_Zl2_inAgain_main_disappear(EnZl2* this, PlayState* play);
void En_Zl2_inEnding_main_wait(EnZl2* this, PlayState* play);
void En_Zl2_inEnding_main_stand(EnZl2* this, PlayState* play);
void En_Zl2_inEnding_main_require(EnZl2* this, PlayState* play);
void En_Zl2_inEnding_main_pile(EnZl2* this, PlayState* play);
void En_Zl2_inEnding_main_chest(EnZl2* this, PlayState* play);
void En_Zl2_inEnding_main_hesitate(EnZl2* this, PlayState* play);
void En_Zl2_inEnding_main_downforward(EnZl2* this, PlayState* play);
void En_Zl2_inEnding_main_play(EnZl2* this, PlayState* play);
void En_Zl2_inRunning_main_wait(EnZl2* this, PlayState* play);
void En_Zl2_inRunning_main_run(EnZl2* this, PlayState* play);
void En_Zl2_inRunning_main_stand(EnZl2* this, PlayState* play);
void En_Zl2_main_bank(EnZl2* this, PlayState* play);
void En_Zl2_Actor_draw_none(EnZl2* this, PlayState* play);
void En_Zl2_Actor_draw_normal(EnZl2* this, PlayState* play);
void En_Zl2_Actor_draw_alpha(EnZl2* this, PlayState* play);

static void* en_zl2_eye[] = { gZelda2EyeOpenTex, gZelda2EyeHalfTex, gZelda2EyeShutTex,
                                gZelda2Eye03Tex,   gZelda2Eye04Tex,   gZelda2Eye05Tex,
                                gZelda2Eye06Tex,   gZelda2Eye07Tex,   gZelda2Eye08Tex };

static void* en_zl2_mouth[] = { gZelda2MouthSeriousTex, gZelda2MouthHappyTex, gZelda2MouthOpenTex };

void En_Zl2_Actor_dt(Actor* thisx, PlayState* play) {
    EnZl2* this = (EnZl2*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
}

void En_Zl2_set_eye_pattern(EnZl2* this) {
    s32 pad[4];
    s16* eyeTexIndex2 = &this->eyeTexIndex2;
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeTexIndex = &this->eyeTexIndex;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = get_random_timer(60, 60);
    }
    *eyeTexIndex = *blinkTimer;
    if (*eyeTexIndex >= 3) {
        *eyeTexIndex = 0;
    }
    *eyeTexIndex2 = *eyeTexIndex;
}

void En_Zl2_set_NormalToClose_eye_pattern(EnZl2* this) {
    s16* eyeTexIndex = &this->eyeTexIndex;
    f32* unk_27C = &this->unk_27C;

    if (*unk_27C < 3.0f) {
        *eyeTexIndex = 0;
    } else if (*unk_27C < 6.0f) {
        *eyeTexIndex = 1;
    } else if (*unk_27C < 9.0f) {
        *eyeTexIndex = 2;
    } else {
        *eyeTexIndex = 5;
        this->eyeTexIndex2 = *eyeTexIndex;
        return;
    }
    *unk_27C += 1.0f;
    this->eyeTexIndex2 = *eyeTexIndex;
}

void En_Zl2_set_CloseToNormal_eye_pattern(EnZl2* this) {
    s16* eyeTexIndex = &this->eyeTexIndex;
    f32* unk_27C = &this->unk_27C;

    if (*unk_27C < 2.0f) {
        *eyeTexIndex = 5;
    } else if (*unk_27C < 4.0f) {
        *eyeTexIndex = 2;
    } else if (*unk_27C < 6.0f) {
        *eyeTexIndex = 1;
    } else {
        En_Zl2_set_eye_pattern(this);
        return;
    }
    *unk_27C += 1.0f;
    this->eyeTexIndex2 = *eyeTexIndex;
}

void En_Zl2_set_NormalToClose2_eye_pattern(EnZl2* this) {
    s16* eyeTexIndex = &this->eyeTexIndex;
    f32* unk_27C = &this->unk_27C;

    if (*unk_27C < 2.0f) {
        *eyeTexIndex = 0;
    } else if (*unk_27C < 4.0f) {
        *eyeTexIndex = 1;
    } else {
        *eyeTexIndex = 2;
        this->eyeTexIndex2 = *eyeTexIndex;
        return;
    }
    *unk_27C += 1.0f;
    this->eyeTexIndex2 = *eyeTexIndex;
}

void En_Zl2_set_Close2ToNormal_eye_pattern(EnZl2* this) {
    s16* eyeTexIndex = &this->eyeTexIndex;
    f32* unk_27C = &this->unk_27C;

    if (*unk_27C < 2.0f) {
        *eyeTexIndex = 2;
    } else if (*unk_27C < 4.0f) {
        *eyeTexIndex = 1;
    } else {
        En_Zl2_set_eye_pattern(this);
        return;
    }
    *unk_27C += 1.0f;
    this->eyeTexIndex2 = *eyeTexIndex;
}

void En_Zl2_setNum_eye_pattern(EnZl2* this, s16 index) {
    this->eyeTexIndex = index;
    this->eyeTexIndex2 = this->eyeTexIndex;
}

void En_Zl2_setNum_eye_pattern2(EnZl2* this, s16 index) {
    this->eyeTexIndex2 = index;
}

void En_Zl2_set_mouth_pattern(EnZl2* this, s16 index) {
    this->mouthTexIndex = index;
}

void En_Zl2_BGcheck(EnZl2* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 En_Zl2_Animation_Base(EnZl2* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

CsCmdActorCue* En_Zl2_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

void En_Zl2_Set_DemoStartPosAngle(EnZl2* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = En_Zl2_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;

        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void En_Zl2_CalcWaveHearV(EnZl2* this, s16 arg1, s32 arg2) {
    s32 phi_a3;
    s32 temp_v1;
    s32 phi_v0 = arg2;

    if (this->unk_24C != 0) {
        temp_v1 = (s16)(arg1 + this->unk_1DC[phi_v0]);
        phi_a3 = arg1 - this->unk_20C[phi_v0];
        phi_v0 = this->unk_1AC[phi_v0];

        if ((s32)fabsf((f32)phi_a3) > 0x8000) {
            if (arg1 > 0) {
                phi_a3 -= 0x10000;
            } else {
                phi_a3 += 0x10000;
            }
        }
        if (phi_a3 != 0) {
            phi_v0 += (phi_a3 - phi_v0) / 16;
        }
        if (phi_v0 != 0) {
            phi_v0 -= (phi_v0 / 10);
        }
        if ((s16)(temp_v1 - arg1) != 0) {
            phi_v0 -= ((s16)(temp_v1 - arg1) / 50);
        }
        temp_v1 += phi_v0;
        if (((this->unk_1AC[arg2] * phi_v0) <= 0) && ((s16)(temp_v1 - arg1) > -0x64) &&
            ((s16)(temp_v1 - arg1) < 0x64)) {
            temp_v1 = arg1;
            phi_v0 = 0;
        }
        this->unk_1AC[arg2] = phi_v0;
        this->unk_1DC[arg2] = temp_v1 - arg1;
    }
    this->unk_20C[arg2] = arg1;
}

void En_Zl2_CalcWaveHearH(EnZl2* this, s16 arg1, s32 arg2) {
    s32 temp_t0 = arg2;
    s32 temp_t2;
    s32 temp_v1;
    s32 phi_t1;
    s32 phi_v0;
    s32 phi_a0;
    f32 curFrame;
    f32 unk_278;

    if (temp_t0 == 2) {
        phi_a0 = 0x3A98;
        phi_t1 = 0;
    } else if (temp_t0 == 5) {
        phi_a0 = 0x32C8;
        phi_t1 = 3;
    } else if (temp_t0 == 8) {
        phi_a0 = 0x2EE0;
        phi_t1 = 6;
    } else if (temp_t0 == 11) {
        phi_a0 = 0x4000;
        phi_t1 = 9;
    } else if (temp_t0 == 14) {
        phi_a0 = 0x4000;
        phi_t1 = 12;
    } else if (temp_t0 == 17) {
        phi_a0 = 0x4000;
        phi_t1 = 15;
    } else {
        phi_a0 = 0x4000;
        phi_t1 = 18;
    }

    if (this->unk_24C != 0) {
        phi_v0 = this->unk_1DC[temp_t0] + arg1;
        temp_v1 = (s16)(phi_v0 & 0xFFFF);
        temp_t2 = arg1 - this->unk_20C[temp_t0];
        phi_v0 = this->unk_1AC[temp_t0];

        if ((s32)fabsf((f32)temp_t2) > 0x8000) {
            if (arg1 > 0) {
                temp_t2 -= 0x10000;
            } else {
                temp_t2 += 0x10000;
            }
        }
        if (phi_t1 >= 0) {
            temp_t2 += (ABS(this->unk_1AC[phi_t1]) / 3);
        }
        if (temp_t2 != 0) {
            phi_v0 += ((temp_t2 - phi_v0) / 16);
        }
        if (phi_v0 != 0) {
            phi_v0 -= phi_v0 / 10;
        }
        if ((s16)(temp_v1 - phi_a0) != 0) {
            phi_v0 -= (s16)(temp_v1 - phi_a0) / 50;
        }
        temp_v1 += phi_v0;

        if (((this->unk_1AC[arg2] * phi_v0) <= 0) && ((s16)(temp_v1 - phi_a0) > -0x64) &&
            ((s16)(temp_v1 - phi_a0) < 0x64)) {
            temp_v1 = phi_a0;
            phi_v0 = 0;
        }

        if (arg2 == 2) {
            s32 pad;

            if ((this->action == 5) || (this->action == 30)) {
                s32 temp_t0;

                curFrame = this->skelAnime.curFrame;
                unk_278 = this->unk_278;
                temp_t0 = (s32)((3500.0f * curFrame) / unk_278) + phi_a0;
                if (temp_t0 >= temp_v1) {
                    temp_v1 = temp_t0;
                    phi_v0 /= -2;
                }
            } else if ((this->action == 6) || (this->action == 31)) {
                s32 temp_t0 = phi_a0 + 0xDAC;

                if (temp_t0 >= temp_v1) {
                    temp_v1 = temp_t0;
                    phi_v0 /= -2;
                }
            } else if (this->action == 20) {
                s32 temp_t0 = phi_a0 - 0x3E8;

                if (temp_t0 >= temp_v1) {
                    temp_v1 = temp_t0;
                    phi_v0 /= -2;
                }
            }
        }
        this->unk_1AC[arg2] = phi_v0;
        this->unk_1DC[arg2] = temp_v1 - arg1;
    }
    this->unk_20C[arg2] = arg1;
}

void En_Zl2_CalcWaveHearD(EnZl2* this, s16 arg1, s32 arg2) {
    s32 temp_v1;
    s16 temp_t0;
    s32 temp_t2;
    s32 temp_t3;
    s32 phi_v0;
    s32 index1AC;
    int phi_t5;

    if (this->unk_24C != 0) {
        temp_v1 = this->unk_1DC[arg2] - arg1;
        temp_t0 = temp_v1;
        temp_t2 = temp_t0;
        temp_t3 = this->unk_1AC[arg2];
        phi_v0 = temp_t3;
        temp_t3 = arg1 - this->unk_20C[arg2];

        if (arg2 == 1) {
            index1AC = 0;
            phi_t5 = this->unk_1AC[index1AC];
        } else if (arg2 == 4) {
            index1AC = 3;
            phi_t5 = this->unk_1AC[index1AC];
        } else if (arg2 == 7) {
            index1AC = 6;
            phi_t5 = this->unk_1AC[index1AC];
        } else if (arg2 == 10) {
            index1AC = 9;
            phi_t5 = ABS(this->unk_1AC[index1AC]);
        } else if (arg2 == 13) {
            index1AC = 12;
            phi_t5 = ABS(this->unk_1AC[index1AC]);
        } else if (arg2 == 16) {
            index1AC = 15;
            phi_t5 = -ABS(this->unk_1AC[index1AC]);
        } else {
            index1AC = 18;
            phi_t5 = -ABS(this->unk_1AC[index1AC]);
        }

        if ((s32)fabsf(temp_t3) > 0x8000) {
            if (arg1 > 0) {
                temp_t3 -= 0x10000;
            } else {
                temp_t3 += 0x10000;
            }
        }
        if (index1AC >= 0) {
            temp_t3 += phi_t5 / 3;
        }

        if (temp_t3 != 0) {
            phi_v0 += (temp_t3 - phi_v0) / 16;
        }
        if (phi_v0 != 0) {
            phi_v0 -= phi_v0 / 10;
        }
        if (temp_t0 != 0) {
            phi_v0 -= temp_t0 / 50;
        }
        temp_v1 += phi_v0;
        if (((this->unk_1AC[arg2] * phi_v0) <= 0) && (temp_t2 > -0x64) && (temp_t2 < 0x64)) {
            temp_v1 = 0;
            phi_v0 = 0;
        }
        this->unk_1AC[arg2] = phi_v0;
        this->unk_1DC[arg2] = arg1 + temp_v1;
    }
    this->unk_20C[arg2] = arg1;
}

s32 En_Zl2_SetWaveHear(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
    s32 pad;
    EnZl2* this = (EnZl2*)thisx;

    if (limbIndex == 14) {
        Mtx* sp74 = GRAPH_ALLOC(play->state.gfxCtx, sizeof(Mtx) * 7);
        MtxF sp34;
        Vec3s sp2C;
        s16 pad2;
        s16* unk_1DC = this->unk_1DC;

        gSPSegment((*gfx)++, 0x0C, sp74);

        Matrix_push();

#if PLATFORM_N64
        // Anti-piracy check, Zelda's hair is misshapen if the check fails
        if (osCicId != 6105) {
            Matrix_scale(2.0f, 0.5f, 2.0f, MTXMODE_APPLY);
        }
#endif

        Matrix_translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);
        Matrix_rotateXYZ(rot->x, rot->y, rot->z, MTXMODE_APPLY);
        Matrix_push();
        Matrix_translate(362.0f, -133.0f, 0.0f, MTXMODE_APPLY);
        Matrix_get(&sp34);
        Matrix_to_rotate_new(&sp34, &sp2C, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl2_CalcWaveHearV(this, sp2C.y, 0);
            En_Zl2_CalcWaveHearD(this, sp2C.x, 1);
            En_Zl2_CalcWaveHearH(this, sp2C.z, 2);
        }
        Matrix_rotateXYZ(unk_1DC[0] + kREG(31), unk_1DC[1] + kREG(32), unk_1DC[2] + kREG(33), MTXMODE_APPLY);
        Matrix_translate(-188.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[0], "../z_en_zl2.c", 1056);
        Matrix_get(&sp34);
        Matrix_to_rotate_new(&sp34, &sp2C, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl2_CalcWaveHearV(this, sp2C.y, 3);
            En_Zl2_CalcWaveHearD(this, sp2C.x, 4);
        }
        Matrix_rotateXYZ(unk_1DC[3] + kREG(34), unk_1DC[4] + kREG(35), unk_1DC[5] + kREG(36), MTXMODE_APPLY);
        Matrix_translate(-410.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[1], "../z_en_zl2.c", 1100);
        Matrix_get(&sp34);
        Matrix_to_rotate_new(&sp34, &sp2C, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl2_CalcWaveHearV(this, sp2C.y, 6);
            En_Zl2_CalcWaveHearD(this, sp2C.x, 7);
        }
        Matrix_rotateXYZ(unk_1DC[6] + kREG(37), unk_1DC[7] + kREG(38), unk_1DC[8] + kREG(39), MTXMODE_APPLY);
        Matrix_translate(-1019.0f, -26.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[2], "../z_en_zl2.c", 1120);
        Matrix_pull();
        Matrix_push();
        Matrix_translate(467.0f, 265.0f, 389.0f, MTXMODE_APPLY);
        Matrix_get(&sp34);
        Matrix_to_rotate_new(&sp34, &sp2C, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl2_CalcWaveHearV(this, sp2C.y, 9);
            En_Zl2_CalcWaveHearD(this, sp2C.x, 10);
            En_Zl2_CalcWaveHearH(this, sp2C.z, 11);
        }
        Matrix_rotateXYZ(unk_1DC[9] + kREG(40), unk_1DC[10] + kREG(41), unk_1DC[11] + kREG(42), MTXMODE_APPLY);
        Matrix_translate(-427.0f, -1.0f, -3.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[3], "../z_en_zl2.c", 1145);
        Matrix_get(&sp34);
        Matrix_to_rotate_new(&sp34, &sp2C, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl2_CalcWaveHearV(this, sp2C.y, 12);
            En_Zl2_CalcWaveHearD(this, sp2C.x, 13);
            En_Zl2_CalcWaveHearH(this, sp2C.z, 14);
        }
        Matrix_rotateXYZ(unk_1DC[12] + kREG(43), unk_1DC[13] + kREG(44), unk_1DC[14] + kREG(45), MTXMODE_APPLY);
        Matrix_translate(-446.0f, -52.0f, 84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[4], "../z_en_zl2.c", 1164);
        Matrix_pull();
        Matrix_push();
        Matrix_translate(467.0f, 265.0f, -389.0f, MTXMODE_APPLY);
        Matrix_get(&sp34);
        Matrix_to_rotate_new(&sp34, &sp2C, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl2_CalcWaveHearV(this, sp2C.y, 15);
            En_Zl2_CalcWaveHearD(this, sp2C.x, 16);
            En_Zl2_CalcWaveHearH(this, sp2C.z, 17);
        }
        Matrix_rotateXYZ(unk_1DC[15] + kREG(46), unk_1DC[16] + kREG(47), unk_1DC[17] + kREG(48), MTXMODE_APPLY);
        Matrix_translate(-427.0f, -1.0f, 3.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[5], "../z_en_zl2.c", 1189);
        Matrix_get(&sp34);
        Matrix_to_rotate_new(&sp34, &sp2C, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl2_CalcWaveHearV(this, sp2C.y, 18);
            En_Zl2_CalcWaveHearD(this, sp2C.x, 19);
            En_Zl2_CalcWaveHearH(this, sp2C.z, 20);
        }
        Matrix_rotateXYZ(unk_1DC[18] + kREG(49), unk_1DC[19] + kREG(50), unk_1DC[20] + kREG(51), MTXMODE_APPLY);
        Matrix_translate(-446.0f, -52.0f, -84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[6], "../z_en_zl2.c", 1208);
        Matrix_pull();
        Matrix_pull();
        this->unk_24C = 1;
    }
    return false;
}

void En_Zl2_SetOcarina(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnZl2* this = (EnZl2*)thisx;
    s32 pad[2];

    if (limbIndex == 10) {
        if ((this->unk_254 != 0) && (play->csCtx.curFrame >= 900)) {
            gSPDisplayList((*gfx)++, gZelda2OcarinaDL);
        }

        {
            Player* player = GET_PLAYER(play);
            Matrix_push();
            if (player->rightHandType == PLAYER_MODELTYPE_RH_FF) {
                Matrix_put(&player->shieldMf);
                Matrix_translate(180.0f, 979.0f, -375.0f, MTXMODE_APPLY);
                Matrix_rotateXYZ(-0x5DE7, -0x53E9, 0x3333, MTXMODE_APPLY);
                Matrix_scale(1.2f, 1.2f, 1.2f, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD((*gfx)++, play->state.gfxCtx, "../z_en_zl2.c", 1253);
                gSPDisplayList((*gfx)++, gZelda2OcarinaDL);
            }
            Matrix_pull();
        }
    }
}

void En_Zl2_Change_Bank(EnZl2* this, PlayState* play) {
    s32 objectSlot = this->zl2Anime1ObjectSlot;

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
}

void En_Zl2_Change_Anime(EnZl2* this, AnimationHeader* animation, u8 arg2, f32 morphFrames, s32 arg4) {
    f32 frameCount = Si2_anime_end_frame(animation);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (arg4 == 0) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        fc = 0.0f;
        unk0 = frameCount;
        playbackSpeed = -1.0f;
    }

    Skeleton_Info2_init(&this->skelAnime, animation, playbackSpeed, unk0, fc, arg2, morphFrames);
}

#include "z_en_zl2_inAgain.inc.c"

#include "z_en_zl2_inEnding.inc.c"

#include "z_en_zl2_inRunning.inc.c"

void En_Zl2_main_init(EnZl2* this, PlayState* play) {
    switch (this->actor.params) {
        case 1:
            En_Zl2_Actor_inEnding_Init(this, play);
            break;
        case 4:
            En_Zl2_Actor_inRunning_Init(this, play);
            break;
#if DEBUG_FEATURES
        case 0:
            En_Zl2_Actor_inAgain_Init(this, play);
            break;
#endif
        default:
            PRINTF(VT_FGCOL(RED) " En_Oa2 の arg_data がおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
            En_Zl2_Actor_inAgain_Init(this, play);
    }
}

void En_Zl2_main_bank(EnZl2* this, PlayState* play) {
    s32 pad;
    ObjectContext* objectCtx = &play->objectCtx;
    s32 objectSlot = Object_Exchange_bank_check(objectCtx, OBJECT_ZL2_ANIME1);
    s32 pad2;

#if DEBUG_FEATURES
    if (objectSlot < 0) {
        PRINTF(VT_FGCOL(RED) "En_Zl2_main_bankアニメーションのバンクを読めない!!!!!!!!!!!!\n" VT_RST);
        return;
    }
#endif

    if (Object_Exchange_bank_dma_check(objectCtx, objectSlot)) {
        this->zl2Anime1ObjectSlot = objectSlot;
        En_Zl2_Change_Bank(this, play);
        this->unk_278 = Si2_anime_end_frame(&gZelda2Anime1Anim_0022D0);
        En_Zl2_main_init(this, play);
    }
}

void En_Zl2_Actor_main(Actor* thisx, PlayState* play) {
    static EnZl2ActionFunc proc[] = {
        En_Zl2_main_bank, En_Zl2_inAgain_main_wait, En_Zl2_inAgain_main_greet, En_Zl2_inAgain_main_approach, En_Zl2_inAgain_main_confess, En_Zl2_inAgain_main_gloom,
        En_Zl2_inAgain_main_waver, En_Zl2_inAgain_main_forward, En_Zl2_inAgain_main_encourage, En_Zl2_inAgain_main_handup, En_Zl2_inAgain_main_cheer, En_Zl2_inAgain_main_give,
        En_Zl2_inAgain_main_effect, En_Zl2_inAgain_main_gaze, En_Zl2_inAgain_main_turn, En_Zl2_inAgain_main_around, En_Zl2_inAgain_main_raise, En_Zl2_inAgain_main_attention,
        En_Zl2_inAgain_main_arrest, En_Zl2_inAgain_main_prison, En_Zl2_inAgain_main_collapse, En_Zl2_inAgain_main_faint, En_Zl2_inAgain_main_fly, En_Zl2_inAgain_main_fade,
        En_Zl2_inAgain_main_disappear, En_Zl2_inEnding_main_wait, En_Zl2_inEnding_main_stand, En_Zl2_inEnding_main_require, En_Zl2_inEnding_main_pile, En_Zl2_inEnding_main_chest,
        En_Zl2_inEnding_main_hesitate, En_Zl2_inEnding_main_downforward, En_Zl2_inEnding_main_play, En_Zl2_inRunning_main_wait, En_Zl2_inRunning_main_run, En_Zl2_inRunning_main_stand,
    };

    EnZl2* this = (EnZl2*)thisx;

    if (this->action < 0 || this->action >= 0x24 || proc[this->action] == NULL) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->action](this, play);
}

void En_Zl2_Actor_ct(Actor* thisx, PlayState* play) {
    EnZl2* this = (EnZl2*)thisx;
    ActorShape* shape = &thisx->shape;
    s32 pad;

    Shape_Info_init(shape, 0.0f, Actor_shadow_circle, 30.0f);
    shape->shadowAlpha = 0;
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gZelda2Skel, NULL, NULL, NULL, 0);

    switch (thisx->params) {
        case 1:
            Nai_FxSetLockFlag(0x6F);
            break;
        case 4:
            z_common_data.subTimerState = SUBTIMER_STATE_OFF;
            break;
    }
}

s32 En_Zl2_Actor_beforedraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfx) {
    static OverrideLimbDraw proc[] = {
        En_Zl2_SetWaveHear,
    };

    EnZl2* this = (EnZl2*)thisx;

    if (this->overrideLimbDrawConfig < 0 || this->overrideLimbDrawConfig > 0 ||
        proc[this->overrideLimbDrawConfig] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画前処理モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return 0;
    }
    return proc[this->overrideLimbDrawConfig](play, limbIndex, dList, pos, rot, thisx, gfx);
}

void En_Zl2_Actor_draw_none(EnZl2* this, PlayState* play) {
}

void En_Zl2_Actor_draw_normal(EnZl2* this, PlayState* play) {
    s32 pad[3];
    s16 eyeTexIndex = this->eyeTexIndex;
    s16 eyeTexIndex2 = this->eyeTexIndex2;
    void* eyeTex = en_zl2_eye[eyeTexIndex];
    void* eyeTex2 = en_zl2_eye[eyeTexIndex2];
    SkelAnime* skelAnime = &this->skelAnime;
    s16 mouthTexIndex = this->mouthTexIndex;
    void* mouthTex = en_zl2_mouth[mouthTexIndex];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zl2.c", 1623);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex2));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0B, &Actor_change_render_mode[2]);

    POLY_OPA_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       En_Zl2_Actor_beforedraw, En_Zl2_SetOcarina, this, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zl2.c", 1648);
}

void En_Zl2_Actor_draw_alpha(EnZl2* this, PlayState* play) {
    s32 pad[2];
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTex = en_zl2_eye[eyeTexIndex];
    s16 mouthTexIndex = this->mouthTexIndex;
    SkelAnime* skelAnime = &this->skelAnime;
    void* mouthTex = en_zl2_mouth[mouthTexIndex];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zl2.c", 1663);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0B, &Actor_change_render_mode[0]);

    POLY_XLU_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       En_Zl2_Actor_beforedraw, NULL, this, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zl2.c", 1692);
}

void En_Zl2_Actor_draw(Actor* thisx, PlayState* play) {
    static EnZl2DrawFunc proc[] = {
        En_Zl2_Actor_draw_none,
        En_Zl2_Actor_draw_normal,
        En_Zl2_Actor_draw_alpha,
    };

    EnZl2* this = (EnZl2*)thisx;

    if ((this->drawConfig < 0) || (this->drawConfig >= 3) || (proc[this->drawConfig] == NULL)) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->drawConfig](this, play);
}

ActorProfile En_Zl2_Profile = {
    /**/ ACTOR_EN_ZL2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZL2,
    /**/ sizeof(EnZl2),
    /**/ En_Zl2_Actor_ct,
    /**/ En_Zl2_Actor_dt,
    /**/ En_Zl2_Actor_main,
    /**/ En_Zl2_Actor_draw,
};
