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

void EnZl2_Init(Actor* thisx, PlayState* play);
void EnZl2_Destroy(Actor* thisx, PlayState* play);
void EnZl2_Update(Actor* thisx, PlayState* play);
void EnZl2_Draw(Actor* thisx, PlayState* play);

s32 func_80B4F45C(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx);

void func_80B50BBC(EnZl2* this, PlayState* play);
void func_80B50BEC(EnZl2* this, PlayState* play);
void func_80B50C40(EnZl2* this, PlayState* play);
void func_80B50CA8(EnZl2* this, PlayState* play);
void func_80B50CFC(EnZl2* this, PlayState* play);
void func_80B50D50(EnZl2* this, PlayState* play);
void func_80B50D94(EnZl2* this, PlayState* play);
void func_80B50DE8(EnZl2* this, PlayState* play);
void func_80B50E3C(EnZl2* this, PlayState* play);
void func_80B50E90(EnZl2* this, PlayState* play);
void func_80B50EE4(EnZl2* this, PlayState* play);
void func_80B50F38(EnZl2* this, PlayState* play);
void func_80B50F8C(EnZl2* this, PlayState* play);
void func_80B50FE8(EnZl2* this, PlayState* play);
void func_80B51034(EnZl2* this, PlayState* play);
void func_80B51080(EnZl2* this, PlayState* play);
void func_80B510CC(EnZl2* this, PlayState* play);
void func_80B51118(EnZl2* this, PlayState* play);
void func_80B51164(EnZl2* this, PlayState* play);
void func_80B511B0(EnZl2* this, PlayState* play);
void func_80B511FC(EnZl2* this, PlayState* play);
void func_80B51250(EnZl2* this, PlayState* play);
void func_80B512B8(EnZl2* this, PlayState* play);
void func_80B51310(EnZl2* this, PlayState* play);
void func_80B51A5C(EnZl2* this, PlayState* play);
void func_80B51A8C(EnZl2* this, PlayState* play);
void func_80B51AE4(EnZl2* this, PlayState* play);
void func_80B51B44(EnZl2* this, PlayState* play);
void func_80B51BA8(EnZl2* this, PlayState* play);
void func_80B51C0C(EnZl2* this, PlayState* play);
void func_80B51C64(EnZl2* this, PlayState* play);
void func_80B51CA8(EnZl2* this, PlayState* play);
void func_80B52068(EnZl2* this, PlayState* play);
void func_80B52098(EnZl2* this, PlayState* play);
void func_80B52108(EnZl2* this, PlayState* play);
void func_80B521A0(EnZl2* this, PlayState* play);
void func_80B523BC(EnZl2* this, PlayState* play);
void func_80B523C8(EnZl2* this, PlayState* play);
void func_80B525D4(EnZl2* this, PlayState* play);

static void* sEyeTextures[] = { gZelda2EyeOpenTex, gZelda2EyeHalfTex, gZelda2EyeShutTex,
                                gZelda2Eye03Tex,   gZelda2Eye04Tex,   gZelda2Eye05Tex,
                                gZelda2Eye06Tex,   gZelda2Eye07Tex,   gZelda2Eye08Tex };

static void* sMouthTextures[] = { gZelda2MouthSeriousTex, gZelda2MouthHappyTex, gZelda2MouthOpenTex };

static EnZl2ActionFunc sActionFuncs[] = {
    func_80B521A0, func_80B50BBC, func_80B50BEC, func_80B50C40, func_80B50CA8, func_80B50CFC,
    func_80B50D50, func_80B50D94, func_80B50DE8, func_80B50E3C, func_80B50E90, func_80B50EE4,
    func_80B50F38, func_80B50F8C, func_80B50FE8, func_80B51034, func_80B51080, func_80B510CC,
    func_80B51118, func_80B51164, func_80B511B0, func_80B511FC, func_80B51250, func_80B512B8,
    func_80B51310, func_80B51A5C, func_80B51A8C, func_80B51AE4, func_80B51B44, func_80B51BA8,
    func_80B51C0C, func_80B51C64, func_80B51CA8, func_80B52068, func_80B52098, func_80B52108,
};

static OverrideLimbDraw sOverrideLimbDrawFuncs[] = {
    func_80B4F45C,
};

static EnZl2DrawFunc sDrawFuncs[] = {
    func_80B523BC,
    func_80B523C8,
    func_80B525D4,
};

ActorProfile En_Zl2_Profile = {
    /**/ ACTOR_EN_ZL2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZL2,
    /**/ sizeof(EnZl2),
    /**/ EnZl2_Init,
    /**/ EnZl2_Destroy,
    /**/ EnZl2_Update,
    /**/ EnZl2_Draw,
};

void EnZl2_Destroy(Actor* thisx, PlayState* play) {
    EnZl2* this = (EnZl2*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
}

void EnZl2_UpdateEyes(EnZl2* this) {
    s32 pad[4];
    s16* eyeTexIndex2 = &this->eyeTexIndex2;
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeTexIndex = &this->eyeTexIndex;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }
    *eyeTexIndex = *blinkTimer;
    if (*eyeTexIndex >= 3) {
        *eyeTexIndex = 0;
    }
    *eyeTexIndex2 = *eyeTexIndex;
}

void func_80B4EA40(EnZl2* this) {
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

void func_80B4EAF4(EnZl2* this) {
    s16* eyeTexIndex = &this->eyeTexIndex;
    f32* unk_27C = &this->unk_27C;

    if (*unk_27C < 2.0f) {
        *eyeTexIndex = 5;
    } else if (*unk_27C < 4.0f) {
        *eyeTexIndex = 2;
    } else if (*unk_27C < 6.0f) {
        *eyeTexIndex = 1;
    } else {
        EnZl2_UpdateEyes(this);
        return;
    }
    *unk_27C += 1.0f;
    this->eyeTexIndex2 = *eyeTexIndex;
}

void func_80B4EBB8(EnZl2* this) {
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

void func_80B4EC48(EnZl2* this) {
    s16* eyeTexIndex = &this->eyeTexIndex;
    f32* unk_27C = &this->unk_27C;

    if (*unk_27C < 2.0f) {
        *eyeTexIndex = 2;
    } else if (*unk_27C < 4.0f) {
        *eyeTexIndex = 1;
    } else {
        EnZl2_UpdateEyes(this);
        return;
    }
    *unk_27C += 1.0f;
    this->eyeTexIndex2 = *eyeTexIndex;
}

void EnZl2_setEyesIndex(EnZl2* this, s16 index) {
    this->eyeTexIndex = index;
    this->eyeTexIndex2 = this->eyeTexIndex;
}

void EnZl2_setEyeIndex2(EnZl2* this, s16 index) {
    this->eyeTexIndex2 = index;
}

void EnZl2_setMouthIndex(EnZl2* this, s16 index) {
    this->mouthTexIndex = index;
}

void func_80B4ED2C(EnZl2* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 EnZl2_UpdateSkelAnime(EnZl2* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorCue* EnZl2_GetCue(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

void func_80B4EDB8(EnZl2* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = EnZl2_GetCue(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;

        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void func_80B4EE38(EnZl2* this, s16 arg1, s32 arg2) {
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

void func_80B4EF64(EnZl2* this, s16 arg1, s32 arg2) {
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

void func_80B4F230(EnZl2* this, s16 arg1, s32 arg2) {
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

s32 func_80B4F45C(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
    s32 pad;
    EnZl2* this = (EnZl2*)thisx;

    if (limbIndex == 14) {
        Mtx* sp74 = GRAPH_ALLOC(play->state.gfxCtx, sizeof(Mtx) * 7);
        MtxF sp34;
        Vec3s sp2C;
        s16 pad2;
        s16* unk_1DC = this->unk_1DC;

        gSPSegment((*gfx)++, 0x0C, sp74);

        Matrix_Push();

#if PLATFORM_N64
        // Anti-piracy check, Zelda's hair is misshapen if the check fails
        if (osCicId != 6105) {
            Matrix_Scale(2.0f, 0.5f, 2.0f, MTXMODE_APPLY);
        }
#endif

        Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);
        Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
        Matrix_Push();
        Matrix_Translate(362.0f, -133.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 0);
            func_80B4F230(this, sp2C.x, 1);
            func_80B4EF64(this, sp2C.z, 2);
        }
        Matrix_RotateZYX(unk_1DC[0] + kREG(31), unk_1DC[1] + kREG(32), unk_1DC[2] + kREG(33), MTXMODE_APPLY);
        Matrix_Translate(-188.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[0], "../z_en_zl2.c", 1056);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 3);
            func_80B4F230(this, sp2C.x, 4);
        }
        Matrix_RotateZYX(unk_1DC[3] + kREG(34), unk_1DC[4] + kREG(35), unk_1DC[5] + kREG(36), MTXMODE_APPLY);
        Matrix_Translate(-410.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[1], "../z_en_zl2.c", 1100);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 6);
            func_80B4F230(this, sp2C.x, 7);
        }
        Matrix_RotateZYX(unk_1DC[6] + kREG(37), unk_1DC[7] + kREG(38), unk_1DC[8] + kREG(39), MTXMODE_APPLY);
        Matrix_Translate(-1019.0f, -26.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[2], "../z_en_zl2.c", 1120);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(467.0f, 265.0f, 389.0f, MTXMODE_APPLY);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 9);
            func_80B4F230(this, sp2C.x, 10);
            func_80B4EF64(this, sp2C.z, 11);
        }
        Matrix_RotateZYX(unk_1DC[9] + kREG(40), unk_1DC[10] + kREG(41), unk_1DC[11] + kREG(42), MTXMODE_APPLY);
        Matrix_Translate(-427.0f, -1.0f, -3.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[3], "../z_en_zl2.c", 1145);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 12);
            func_80B4F230(this, sp2C.x, 13);
            func_80B4EF64(this, sp2C.z, 14);
        }
        Matrix_RotateZYX(unk_1DC[12] + kREG(43), unk_1DC[13] + kREG(44), unk_1DC[14] + kREG(45), MTXMODE_APPLY);
        Matrix_Translate(-446.0f, -52.0f, 84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[4], "../z_en_zl2.c", 1164);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(467.0f, 265.0f, -389.0f, MTXMODE_APPLY);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 15);
            func_80B4F230(this, sp2C.x, 16);
            func_80B4EF64(this, sp2C.z, 17);
        }
        Matrix_RotateZYX(unk_1DC[15] + kREG(46), unk_1DC[16] + kREG(47), unk_1DC[17] + kREG(48), MTXMODE_APPLY);
        Matrix_Translate(-427.0f, -1.0f, 3.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[5], "../z_en_zl2.c", 1189);
        Matrix_Get(&sp34);
        Matrix_MtxFToYXZRotS(&sp34, &sp2C, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B4EE38(this, sp2C.y, 18);
            func_80B4F230(this, sp2C.x, 19);
            func_80B4EF64(this, sp2C.z, 20);
        }
        Matrix_RotateZYX(unk_1DC[18] + kREG(49), unk_1DC[19] + kREG(50), unk_1DC[20] + kREG(51), MTXMODE_APPLY);
        Matrix_Translate(-446.0f, -52.0f, -84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp74[6], "../z_en_zl2.c", 1208);
        Matrix_Pop();
        Matrix_Pop();
        this->unk_24C = 1;
    }
    return false;
}

void EnZl2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnZl2* this = (EnZl2*)thisx;
    s32 pad[2];

    if (limbIndex == 10) {
        if ((this->unk_254 != 0) && (play->csCtx.curFrame >= 900)) {
            gSPDisplayList((*gfx)++, gZelda2OcarinaDL);
        }

        {
            Player* player = GET_PLAYER(play);
            Matrix_Push();
            if (player->rightHandType == PLAYER_MODELTYPE_RH_FF) {
                Matrix_Put(&player->shieldMf);
                Matrix_Translate(180.0f, 979.0f, -375.0f, MTXMODE_APPLY);
                Matrix_RotateZYX(-0x5DE7, -0x53E9, 0x3333, MTXMODE_APPLY);
                Matrix_Scale(1.2f, 1.2f, 1.2f, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD((*gfx)++, play->state.gfxCtx, "../z_en_zl2.c", 1253);
                gSPDisplayList((*gfx)++, gZelda2OcarinaDL);
            }
            Matrix_Pop();
        }
    }
}

void func_80B4FCCC(EnZl2* this, PlayState* play) {
    s32 objectSlot = this->zl2Anime1ObjectSlot;

    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
}

void func_80B4FD00(EnZl2* this, AnimationHeader* animation, u8 arg2, f32 morphFrames, s32 arg4) {
    f32 frameCount = Animation_GetLastFrame(animation);
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

    Animation_Change(&this->skelAnime, animation, playbackSpeed, unk0, fc, arg2, morphFrames);
}

#include "z_en_zl2_inAgain.inc.c"

#include "z_en_zl2_inEnding.inc.c"

#include "z_en_zl2_inRunning.inc.c"

void func_80B52114(EnZl2* this, PlayState* play) {
    switch (this->actor.params) {
        case 1:
            func_80B5135C(this, play);
            break;
        case 4:
            func_80B51D0C(this, play);
            break;
#if DEBUG_FEATURES
        case 0:
            func_80B4FD90(this, play);
            break;
#endif
        default:
            PRINTF(VT_FGCOL(RED) " En_Oa2 の arg_data がおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
            func_80B4FD90(this, play);
    }
}

void func_80B521A0(EnZl2* this, PlayState* play) {
    s32 pad;
    ObjectContext* objectCtx = &play->objectCtx;
    s32 objectSlot = Object_GetSlot(objectCtx, OBJECT_ZL2_ANIME1);
    s32 pad2;

#if DEBUG_FEATURES
    if (objectSlot < 0) {
        PRINTF(VT_FGCOL(RED) "En_Zl2_main_bankアニメーションのバンクを読めない!!!!!!!!!!!!\n" VT_RST);
        return;
    }
#endif

    if (Object_IsLoaded(objectCtx, objectSlot)) {
        this->zl2Anime1ObjectSlot = objectSlot;
        func_80B4FCCC(this, play);
        this->unk_278 = Animation_GetLastFrame(&gZelda2Anime1Anim_0022D0);
        func_80B52114(this, play);
    }
}

void EnZl2_Update(Actor* thisx, PlayState* play) {
    EnZl2* this = (EnZl2*)thisx;

    if (this->action < 0 || this->action >= 0x24 || sActionFuncs[this->action] == NULL) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, play);
}

void EnZl2_Init(Actor* thisx, PlayState* play) {
    EnZl2* this = (EnZl2*)thisx;
    ActorShape* shape = &thisx->shape;
    s32 pad;

    ActorShape_Init(shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    shape->shadowAlpha = 0;
    SkelAnime_InitFlex(play, &this->skelAnime, &gZelda2Skel, NULL, NULL, NULL, 0);

    switch (thisx->params) {
        case 1:
            Audio_SetSfxBanksMute(0x6F);
            break;
        case 4:
            gSaveContext.subTimerState = SUBTIMER_STATE_OFF;
            break;
    }
}

s32 EnZl2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfx) {
    EnZl2* this = (EnZl2*)thisx;

    if (this->overrideLimbDrawConfig < 0 || this->overrideLimbDrawConfig > 0 ||
        sOverrideLimbDrawFuncs[this->overrideLimbDrawConfig] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画前処理モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return 0;
    }
    return sOverrideLimbDrawFuncs[this->overrideLimbDrawConfig](play, limbIndex, dList, pos, rot, thisx, gfx);
}

void func_80B523BC(EnZl2* this, PlayState* play) {
}

void func_80B523C8(EnZl2* this, PlayState* play) {
    s32 pad[3];
    s16 eyeTexIndex = this->eyeTexIndex;
    s16 eyeTexIndex2 = this->eyeTexIndex2;
    void* eyeTex = sEyeTextures[eyeTexIndex];
    void* eyeTex2 = sEyeTextures[eyeTexIndex2];
    SkelAnime* skelAnime = &this->skelAnime;
    s16 mouthTexIndex = this->mouthTexIndex;
    void* mouthTex = sMouthTextures[mouthTexIndex];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zl2.c", 1623);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex2));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0B, &D_80116280[2]);

    POLY_OPA_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       EnZl2_OverrideLimbDraw, EnZl2_PostLimbDraw, this, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zl2.c", 1648);
}

void func_80B525D4(EnZl2* this, PlayState* play) {
    s32 pad[2];
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTex = sEyeTextures[eyeTexIndex];
    s16 mouthTexIndex = this->mouthTexIndex;
    SkelAnime* skelAnime = &this->skelAnime;
    void* mouthTex = sMouthTextures[mouthTexIndex];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zl2.c", 1663);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0B, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       EnZl2_OverrideLimbDraw, NULL, this, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zl2.c", 1692);
}

void EnZl2_Draw(Actor* thisx, PlayState* play) {
    EnZl2* this = (EnZl2*)thisx;

    if ((this->drawConfig < 0) || (this->drawConfig >= 3) || (sDrawFuncs[this->drawConfig] == NULL)) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawConfig](this, play);
}
