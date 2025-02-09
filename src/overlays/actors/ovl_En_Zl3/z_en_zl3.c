/*
 * File: z_en_zl3.c
 * Overlay: ovl_En_Zl3
 * Description: Adult Zelda
 */

#include "z_en_zl3.h"

#include "libc64/math64.h"
#include "libc64/qrand.h"
#include "attributes.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "one_point_cutscene.h"
#include "regs.h"
#include "segmented_address.h"
#include "seqcmd.h"
#include "sequence.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "z_lib.h"
#include "z64audio.h"
#include "z64frame_advance.h"
#include "z64play.h"
#include "z64player.h"

#include "global.h"

#include "overlays/actors/ovl_En_Encount2/z_en_encount2.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "assets/objects/object_zl2/object_zl2.h"
#include "assets/objects/object_zl2_anime2/object_zl2_anime2.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void EnZl3_Init(Actor* thisx, PlayState* play);
void EnZl3_Destroy(Actor* thisx, PlayState* play);
void EnZl3_Update(Actor* thisx, PlayState* play);
void EnZl3_Draw(Actor* thisx, PlayState* play);
void func_80B59AD0(EnZl3* this, PlayState* play);

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 25, 80, 0, { 0, 0, 0 } },
};

static void* sEyeTextures[] = { gZelda2EyeOpenTex, gZelda2EyeHalfTex, gZelda2EyeShutTex, gZelda2Eye03Tex,
                                gZelda2Eye04Tex,   gZelda2Eye05Tex,   gZelda2Eye06Tex,   NULL };

static void* sMouthTextures[] = { gZelda2MouthSeriousTex, gZelda2MouthHappyTex, gZelda2MouthOpenTex };

static s32 D_80B5A468 = 0;

static Vec3f D_80B5A46C = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80B5A478 = { 0.0f, 10.0f, 0.0f };

void func_80B533B0(Actor* thisx, PlayState* play) {
    EnZl3* this = (EnZl3*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
}

void func_80B533FC(EnZl3* this, PlayState* play) {
    ColliderCylinder* collider = &this->collider;
    s32 pad[4];

    Collider_UpdateCylinder(&this->actor, collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &collider->base);
}

void EnZl3_Destroy(Actor* thisx, PlayState* play) {
    EnZl3* this = (EnZl3*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80B53468(void) {
    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_ESCAPE);
}

BossGanon2* func_80B53488(EnZl3* this, PlayState* play) {
    if (this->ganon == NULL) {
        Actor* actorIt = play->actorCtx.actorLists[ACTORCAT_BOSS].head;

        while (actorIt != NULL) {
            if (actorIt->id == ACTOR_BOSS_GANON2) {
                this->ganon = (BossGanon2*)actorIt;
                break;
            }
            actorIt = actorIt->next;
        }
    }
    return this->ganon;
}

void EnZl3_UpdateEyes(EnZl3* this) {
    s32 pad[2];
    s16* eyeTexIndex = &this->eyeTexIndex;
    s16* blinkTimer = &this->blinkTimer;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }
    *eyeTexIndex = *blinkTimer;
    if (*eyeTexIndex >= 3) {
        *eyeTexIndex = 0;
    }
}

void EnZl3_setEyeIndex(EnZl3* this, s16 index) {
    this->eyeTexIndex = index;
}

void EnZl3_setMouthIndex(EnZl3* this, s16 index) {
    this->mouthTexIndex = index;
}

void func_80B5357C(EnZl3* this, PlayState* play) {
    Vec3f* thisPos = &this->actor.world.pos;
    Vec3f sp20;

    sp20.x = thisPos->x + ((Rand_ZeroOne() - 0.5f) * 10.0f);
    sp20.y = thisPos->y;
    sp20.z = thisPos->z + ((Rand_ZeroOne() - 0.5f) * 10.0f);
    Item_DropCollectible(play, &sp20, ITEM00_RECOVERY_HEART);
}

void func_80B53614(EnZl3* this, PlayState* play) {
    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_RIVER_SOUND, -442.0f, 4102.0f, -371.0f, 0, 0, 0, 0x12);
}

void func_80B5366C(EnZl3* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

void func_80B536B4(EnZl3* this) {
    this->actor.bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_WALL);
}

void func_80B536C4(EnZl3* this) {
    s32 pad[2];
    Vec3s* headRot = &this->interactInfo.headRot;
    Vec3s* torsoRot = &this->interactInfo.torsoRot;

    Math_SmoothStepToS(&headRot->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&headRot->y, 0, 20, 6200, 100);
    Math_SmoothStepToS(&torsoRot->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&torsoRot->y, 0, 20, 6200, 100);
}

void func_80B53764(EnZl3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) - 16.0f;
    Npc_TrackPoint(&this->actor, &this->interactInfo, kREG(17) + 0xC, NPC_TRACKING_HEAD_AND_TORSO);
}

s32 func_80B537E8(EnZl3* this) {
    s16 yawTowardsPlayer = this->actor.yawTowardsPlayer;
    s16* rotY = &this->actor.world.rot.y;
    s16* unk_3D0 = &this->unk_3D0;
    s16 retVal;
    s16 pad[2];

    Math_SmoothStepToS(unk_3D0, ABS((s16)(yawTowardsPlayer - *rotY)), 5, 6200, 100);
    retVal = Math_SmoothStepToS(rotY, yawTowardsPlayer, 5, *unk_3D0, 100);
    this->actor.shape.rot.y = *rotY;
    return retVal;
}

void func_80B538B0(EnZl3* this) {
    s16 yawTowardsPlayer = this->actor.yawTowardsPlayer;
    s16* rotY = &this->actor.world.rot.y;

    if (ABS((s16)(yawTowardsPlayer - *rotY)) >= 0x1556) {
        D_80B5A468 = 1;
    }

    if (D_80B5A468 != 0) {
        if (!func_80B537E8(this)) {
            D_80B5A468 = 0;
        }
    } else {
        this->unk_3D0 = 0;
    }
}

s32 EnZl3_UpdateSkelAnime(EnZl3* this) {
    return SkelAnime_Update(&this->skelAnime);
}

s32 func_80B5396C(EnZl3* this) {
    return this->unk_3C8;
}

void func_80B53974(EnZl3* this, u8 arg1) {
    this->unk_3C8 = arg1;
}

void func_80B53980(EnZl3* thisx, s16 y, s32 idx) {
    EnZl3* this = (EnZl3*)thisx; // this function might take thisx
    s32 action = this->action;
    s16 y2 = y;
    s32 yTemp;
    f32 curFrame;
    f32 unk_3DC;

    if (this->unk_2FC != 0) {
        SkelAnime* skelAnime = &this->skelAnime;
        s32 temp25C = this->unk_25C[idx];
        s32 temp28C = (s16)(y + this->unk_28C[idx]);
        s32 temp2BC = y - this->unk_2BC[idx];

        if ((s32)fabsf(temp2BC) > 0x8000) {
            if (y2 > 0) {
                temp2BC -= 0x10000;
            } else {
                temp2BC += 0x10000;
            }
        }

        if ((idx != 0 || action != 4) && (temp2BC != 0)) {
            temp25C += (temp2BC - temp25C) / 16;
        }

        if (temp25C != 0) {
            temp25C -= temp25C / 10;
        }

        if ((s16)(temp28C - y) != 0) {
            temp25C -= (s16)(temp28C - y) / 50;
        }

        temp28C += temp25C;
        if (((this->unk_25C[idx] * temp25C) <= 0) && (((s16)(temp28C - y) > -0x64) && ((s16)(temp28C - y) < 0x64))) {
            temp28C = y;
            temp25C = 0;
        }
        if (idx == 0 && action == 3) {
            yTemp = y + -11000;
            if (skelAnime->mode == 2) {
                curFrame = skelAnime->curFrame;
                unk_3DC = this->unk_3DC;
                yTemp = (s32)((curFrame / unk_3DC) * -11000) + y;

                if (temp28C >= yTemp) {
                    temp28C = yTemp;
                    if (temp25C > 0) {
                        temp25C /= -2;
                    }
                }
            } else {
                if (temp28C >= yTemp) {
                    temp28C = yTemp;
                    if (temp25C > 0) {
                        temp25C /= -2;
                    }
                }
            }
        }
        this->unk_25C[idx] = temp25C;
        this->unk_28C[idx] = temp28C - y;
    }
    this->unk_2BC[idx] = y;
}

void func_80B53B64(EnZl3* this, s16 z, s32 idx) {
    SkelAnime* skelAnime = &this->skelAnime;
    s32 action = this->action;
    s32 phi_a1;
    s32 idx25C;
    s16 temp_t1;
    s32 temp_a0;
    s32 phi_v0;
    s32 phi_v1;

    if (idx == 2) {
        phi_a1 = 15000;
        idx25C = 0;
    } else if (idx == 5) {
        phi_a1 = 13000;
        idx25C = 3;
    } else if (idx == 8) {
        phi_a1 = 12000;
        idx25C = 6;
    } else if (idx == 11) {
        phi_a1 = 0x4000;
        idx25C = 9;
    } else if (idx == 14) {
        phi_a1 = 0x4000;
        idx25C = 12;
    } else if (idx == 17) {
        phi_a1 = 0x4000;
        idx25C = 15;
    } else {
        phi_a1 = 0x4000;
        idx25C = 18;
    }

    if (this->unk_2FC != 0) {
        phi_v0 = this->unk_25C[idx];
        temp_a0 = (s16)(z + this->unk_28C[idx]);
        phi_v1 = z - this->unk_2BC[idx];

        if ((s32)fabsf(phi_v1) > 0x8000) {
            if (z > 0) {
                phi_v1 -= 0x10000;
            } else {
                phi_v1 += 0x10000;
            }
        }

        if (idx25C >= 0) {
            phi_v1 += ABS(this->unk_25C[idx25C]) / 3;
        }

        if (idx == 2 && (action == 5 || action == 24)) {
            if (phi_v1 != 0) {
                s32 pad;

                phi_v0 -= (phi_v1 - phi_v0) / 10;
            }
        } else if (idx == 2 && action == 22 && skelAnime->mode == 2) {
            if (phi_v1 != 0) {
                s32 pad;

                phi_v0 -= (phi_v1 - phi_v0) / 10;
            }
        } else if (idx == 2 && (action == 20 || action == 21) && skelAnime->mode == 2) {
            if (phi_v1 != 0) {
                s32 pad;

                phi_v0 -= (phi_v1 - phi_v0) / 10;
            }
        } else {
            if (phi_v1 != 0) {
                s32 pad;

                phi_v0 += (phi_v1 - phi_v0) / 16;
            }
        }

        if (phi_v0 != 0) {
            phi_v0 -= phi_v0 / 10;
        }

        if ((s16)(temp_a0 - phi_a1) != 0) {
            phi_v0 -= (s16)(temp_a0 - phi_a1) / 50;
        }

        temp_a0 += phi_v0;
        phi_v1 = (s16)(temp_a0 - phi_a1);

        if (((this->unk_25C[idx] * phi_v0) <= 0) && (phi_v1 > -100) && (phi_v1 < 100)) {
            temp_a0 = phi_a1;
            phi_v0 = 0;
        }

        if (idx == 2) {
            if (action == 4) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3E0 = this->unk_3E0;
                    s32 phi_v1_2 = (s32)(((unk_3E0 - curFrame) / unk_3E0) * -2000.0f) + phi_a1;

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if (action == 5) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3E4 = this->unk_3E4;
                    s32 phi_v1_2 = (s32)((curFrame / unk_3E4) * -2000.0f) + phi_a1;

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    s32 phi_v1_2 = phi_a1 - 2000;

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if ((action == 20) || (action == 21)) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3F4 = this->unk_3F4;
                    s32 phi_v1_2;

                    if (curFrame <= 42.0f) {
                        phi_v1_2 = phi_a1 - 2000;
                    } else {
                        phi_v1_2 = (s32)((((curFrame - 42.0f) * 6200.0f) / (unk_3F4 - 42.0f)) + -2000.0f) + phi_a1;
                    }

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    s32 phi_v1_2 = phi_a1 + 4200;

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if (action == 22) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3EC = this->unk_3EC;
                    s32 phi_v1_2 = (s32)(((curFrame / unk_3EC) * -5200.0f) + 4200.0f) + phi_a1;

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    s32 phi_v1_2 = phi_a1 - 2000;

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if (action == 23) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3F0 = this->unk_3F0;
                    s32 phi_v1_2 = (s32)(((curFrame / unk_3F0) * -7600.0f) + -2000.0f) + phi_a1;

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    s32 phi_v1_2 = phi_a1 - 9600;

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if (action == 24) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3E8 = this->unk_3E8;
                    s32 phi_v1_2 = (s32)(((curFrame / unk_3E8) * 21000.0f) + -9600.0f) + phi_a1;

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    s32 phi_v1_2 = phi_a1 + 11400;

                    if (phi_v1_2 >= temp_a0) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 < 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            }
        } else if (idx == 11 || idx == 17) {
            if (action == 4) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3E0 = this->unk_3E0;
                    s32 phi_v1_2 = (s32)((curFrame / unk_3E0) * -7000.0f) + phi_a1;

                    if (temp_a0 >= phi_v1_2) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 > 0) {
                            phi_v0 /= -2;
                        }
                    }
                } else {
                    s32 phi_v1_2 = phi_a1 - 7000;

                    if (temp_a0 >= phi_v1_2) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 > 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            } else if (action == 5) {
                if (skelAnime->mode == 2) {
                    f32 curFrame = skelAnime->curFrame;
                    f32 unk_3E4 = this->unk_3E4;
                    s32 phi_v1_2 = (s32)(((unk_3E4 - curFrame) / unk_3E4) * -7000.0f) + phi_a1;

                    if (temp_a0 >= phi_v1_2) {
                        temp_a0 = phi_v1_2;
                        if (phi_v0 > 0) {
                            phi_v0 /= -2;
                        }
                    }
                }
            }
        }
        this->unk_25C[idx] = phi_v0;
        this->unk_28C[idx] = temp_a0 - z;
    }
    this->unk_2BC[idx] = z;
}

void func_80B54360(EnZl3* this, s16 arg1, s32 arg2) {
    if (this->unk_2FC != 0) {
        s32 temp_v1 = this->unk_28C[arg2] - arg1;
        s16 temp_t0 = temp_v1;
        s32 temp_t2 = temp_t0;
        s32 temp_t3 = this->unk_25C[arg2];
        s32 phi_v0 = temp_t3;
        s32 index25C;
        int phi_t5;

        temp_t3 = arg1 - this->unk_2BC[arg2];

        if (arg2 == 1) {
            index25C = 0;
            phi_t5 = this->unk_25C[index25C];
        } else if (arg2 == 4) {
            index25C = 3;
            phi_t5 = this->unk_25C[index25C];
        } else if (arg2 == 7) {
            index25C = 6;
            phi_t5 = this->unk_25C[index25C];
        } else if (arg2 == 10) {
            index25C = 9;
            phi_t5 = ABS(this->unk_25C[index25C]);
        } else if (arg2 == 13) {
            index25C = 12;
            phi_t5 = ABS(this->unk_25C[index25C]);
        } else if (arg2 == 16) {
            index25C = 15;
            phi_t5 = -ABS(this->unk_25C[index25C]);
        } else {
            index25C = 18;
            phi_t5 = -ABS(this->unk_25C[index25C]);
        }

        if ((s32)fabsf(temp_t3) > 0x8000) {
            if (arg1 > 0) {
                temp_t3 -= 0x10000;
            } else {
                temp_t3 += 0x10000;
            }
        }
        if (index25C >= 0) {
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
        if (((this->unk_25C[arg2] * phi_v0) <= 0) && (temp_t2 > -0x64) && (temp_t2 < 0x64)) {
            temp_v1 = 0;
            phi_v0 = 0;
        }
        this->unk_25C[arg2] = phi_v0;
        this->unk_28C[arg2] = arg1 + temp_v1;
    }
    this->unk_2BC[arg2] = arg1;
}

s32 func_80B5458C(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
    s32 pad[2];
    EnZl3* this = (EnZl3*)thisx;
    Vec3s* headRot = &this->interactInfo.headRot;
    Vec3s* torsoRot = &this->interactInfo.torsoRot;

    if (limbIndex == 14) {
        Mtx* sp78 = GRAPH_ALLOC(play->state.gfxCtx, sizeof(Mtx) * 7);
        MtxF sp38;
        Vec3s sp30;
        s16* unk_28C = this->unk_28C;
        s32 pad2;

        rot->x += headRot->y;
        rot->z += headRot->x;
        gSPSegment((*gfx)++, 0x0C, sp78);

        Matrix_Push();
        Matrix_Translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);
        Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
        Matrix_Push();
        Matrix_Translate(362.0f, -133.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B53980(this, sp30.y, 0);
            func_80B54360(this, sp30.x, 1);
            func_80B53B64(this, sp30.z, 2);
        }
        Matrix_RotateZYX(unk_28C[0] + kREG(31), unk_28C[1] + kREG(32), unk_28C[2] + kREG(33), MTXMODE_APPLY);
        Matrix_Translate(-188.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[0], "../z_en_zl3.c", 1490);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B53980(this, sp30.y, 3);
        }
        Matrix_RotateZYX(unk_28C[3] + kREG(34), unk_28C[4] + kREG(35), unk_28C[5] + kREG(36), MTXMODE_APPLY);
        Matrix_Translate(-410.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[1], "../z_en_zl3.c", 1534);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B54360(this, sp30.x, 7);
        }
        Matrix_RotateZYX(unk_28C[6] + kREG(37), unk_28C[7] + kREG(38), unk_28C[8] + kREG(39), MTXMODE_APPLY);
        Matrix_Translate(-1019.0f, -26.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[2], "../z_en_zl3.c", 1554);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(467.0f, 265.0f, 389.0f, MTXMODE_APPLY);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B53980(this, sp30.y, 9);
            func_80B54360(this, sp30.x, 10);
            func_80B53B64(this, sp30.z, 11);
        }
        Matrix_RotateZYX(unk_28C[9] + kREG(40), unk_28C[10] + kREG(41), unk_28C[11] + kREG(42), MTXMODE_APPLY);
        Matrix_Translate(-427.0f, -1.0f, -3.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[3], "../z_en_zl3.c", 1579);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B53980(this, sp30.y, 12);
            func_80B54360(this, sp30.x, 13);
            func_80B53B64(this, sp30.z, 14);
        }
        Matrix_RotateZYX(unk_28C[12] + kREG(43), unk_28C[13] + kREG(44), unk_28C[14] + kREG(45), MTXMODE_APPLY);
        Matrix_Translate(-446.0f, -52.0f, 84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[4], "../z_en_zl3.c", 1598);
        Matrix_Pop();
        Matrix_Push();
        Matrix_Translate(467.0f, 265.0f, -389.0f, MTXMODE_APPLY);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B53980(this, sp30.y, 15);
            func_80B54360(this, sp30.x, 16);
            func_80B53B64(this, sp30.z, 17);
        }
        Matrix_RotateZYX(unk_28C[15] + kREG(46), unk_28C[16] + kREG(47), unk_28C[17] + kREG(48), MTXMODE_APPLY);
        Matrix_Translate(-427.0f, -1.0f, 3.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[5], "../z_en_zl3.c", 1623);
        Matrix_Get(&sp38);
        Matrix_MtxFToYXZRotS(&sp38, &sp30, 0);
        if (!FrameAdvance_IsEnabled(play)) {
            func_80B53980(this, sp30.y, 18);
            func_80B54360(this, sp30.x, 19);
            func_80B53B64(this, sp30.z, 20);
        }
        Matrix_RotateZYX(unk_28C[18] + kREG(49), unk_28C[19] + kREG(50), unk_28C[20] + kREG(51), MTXMODE_APPLY);
        Matrix_Translate(-446.0f, -52.0f, -84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[6], "../z_en_zl3.c", 1642);
        Matrix_Pop();
        Matrix_Pop();
        this->unk_2FC = 1;
    } else if (limbIndex == 7) {
        rot->x += torsoRot->y;
        rot->y -= torsoRot->x;
    }
    return false;
}

void EnZl3_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnZl3* this = (EnZl3*)thisx;
    s32 pad;

    if (limbIndex == 13) {
        Vec3f sp34 = D_80B5A46C;
        s32 pad2;

        Matrix_MultVec3f(&sp34, &this->unk_31C);
    } else if (limbIndex == 14) {
        Vec3f sp24 = D_80B5A478;
        Vec3f sp18;

        Matrix_MultVec3f(&sp24, &sp18);
        this->actor.focus.pos.x = sp18.x;
        this->actor.focus.pos.y = sp18.y;
        this->actor.focus.pos.z = sp18.z;
        this->actor.focus.rot.x = this->actor.world.rot.x;
        this->actor.focus.rot.y = this->actor.world.rot.y;
        this->actor.focus.rot.z = this->actor.world.rot.z;
    }
}

s32 func_80B54DB4(EnZl3* this) {
    s32 params = PARAMS_GET_U(this->actor.params, 8, 8);

    return params;
}

s32 func_80B54DC4(EnZl3* this) {
    s32 params = PARAMS_GET_U(this->actor.params, 4, 4);

    return params;
}

s32 func_80B54DD4(EnZl3* this) {
    s32 params = PARAMS_GET_U(this->actor.params, 0, 4);

    return params;
}

void func_80B54DE0(EnZl3* this, PlayState* play) {
    s32 objectSlot = this->zl2Anime2ObjectSlot;

    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
}

void func_80B54E14(EnZl3* this, AnimationHeader* animation, u8 arg2, f32 morphFrames, s32 arg4) {
    f32 frameCount = Animation_GetLastFrame(animation);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (arg4 == 0) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        unk0 = frameCount;
        fc = 0.0f;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&this->skelAnime, animation, playbackSpeed, unk0, fc, arg2, morphFrames);
}

void func_80B54EA4(EnZl3* this, PlayState* play) {
    f32 posX = this->actor.world.pos.x;
    f32 posY = this->actor.world.pos.y;
    f32 posZ = this->actor.world.pos.z;

    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_EG, posX, posY, posZ, 0, 0, 0, 0);
}

#include "z_en_zl3_inFinal.inc.c"

#include "z_en_zl3_inFinal2.inc.c"

#include "z_en_zl3_inEscape.inc.c"

void func_80B59A80(EnZl3* this, PlayState* play) {
    if (func_80B59768(this, play)) {
        Audio_PlaySfxGeneral(NA_SE_OC_REVENGE, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                             &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
    }
}

void func_80B59AD0(EnZl3* this, PlayState* play) {
    Actor* thisx = &this->actor;

    Flags_SetSwitch(play, 0x36);
    Interface_SetSubTimer(180);
    func_80B54EA4(this, play);
    func_80B53614(this, play);
    CLEAR_EVENTCHKINF(EVENTCHKINF_C7);
    func_80B56F10(this, play);
    gSaveContext.healthAccumulator = 320;
    Magic_Fill(play);
    this->action = 27;
    this->drawConfig = 1;
}

void func_80B59B6C(EnZl3* this, PlayState* play) {
    s32 sp2C = func_80B54DD4(this);

    this->unk_3DC = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_0091D8));
    this->unk_3E0 = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_00A598));
    this->unk_3E4 = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_00A334));
    this->unk_3F4 = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_001110));
    this->unk_3EC = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_002348));
    this->unk_3F0 = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_002E54));
    this->unk_3E8 = Animation_GetLastFrame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_001D8C));

    switch (sp2C) {
        case 0:
            func_80B54FB4(this, play);
            break;
        case 1:
            func_80B55780(this, play);
            break;
        case 3:
            func_80B59828(this, play);
            break;
        default:
            PRINTF(VT_FGCOL(RED) " En_Oa3 の arg_data がおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
            Actor_Kill(&this->actor);
    }
}

void func_80B59DB8(EnZl3* this, PlayState* play) {
    s32 pad;
    ObjectContext* objectCtx = &play->objectCtx;
    s32 objectSlot = Object_GetSlot(objectCtx, OBJECT_ZL2_ANIME2);
    s32 pad2;

#if DEBUG_FEATURES
    if (objectSlot < 0) {
        PRINTF(VT_FGCOL(RED) "En_Zl3_main_bankアニメーションのバンクを読めない!!!!!!!!!!!!\n" VT_RST);
        return;
    }
#endif

    if (Object_IsLoaded(objectCtx, objectSlot)) {
        this->zl2Anime2ObjectSlot = objectSlot;
        func_80B54DE0(this, play);
        func_80B59B6C(this, play);
    }
}

static EnZl3ActionFunc sActionFuncs[] = {
    func_80B59DB8, func_80B55550, func_80B555A4, func_80B55604, func_80B5566C, func_80B556CC, func_80B5572C,
    func_80B56658, func_80B566AC, func_80B5670C, func_80B5676C, func_80B567CC, func_80B5682C, func_80B568B4,
    func_80B5691C, func_80B5697C, func_80B569E4, func_80B56A68, func_80B56AE0, func_80B56B54, func_80B56BA8,
    func_80B56C24, func_80B56C84, func_80B56CE4, func_80B56D44, func_80B58D50, func_80B58DB0, func_80B58E10,
    func_80B58E7C, func_80B58EF4, func_80B58F6C, func_80B58FDC, func_80B5904C, func_80B590BC, func_80B5912C,
    func_80B591BC, func_80B5922C, func_80B592A8, func_80B59340, func_80B593D0,
};

void EnZl3_Update(Actor* thisx, PlayState* play) {
    EnZl3* this = (EnZl3*)thisx;

    if (this->action < 0 || this->action >= ARRAY_COUNT(sActionFuncs) || sActionFuncs[this->action] == NULL) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, play);
}

void EnZl3_Init(Actor* thisx, PlayState* play) {
    EnZl3* this = (EnZl3*)thisx;
    ActorShape* shape = &this->actor.shape;
    s32 pad;

    PRINTF("ゼルダ姫のEn_Zl3_Actor_ct通すよ!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    ActorShape_Init(shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    shape->shadowAlpha = 0;
    func_80B533B0(thisx, play);
    SkelAnime_InitFlex(play, &this->skelAnime, &gZelda2Skel, NULL, this->jointTable, this->morphTable, 15);

    switch (func_80B54DD4(this)) {
        case 1:
            gSaveContext.subTimerState = SUBTIMER_STATE_OFF;
            break;
        case 3:
            func_80B59A80(this, play);
            break;
    }

    PRINTF("ゼルダ姫のEn_Zl3_Actor_ctは通った!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

static OverrideLimbDraw sOverrideLimbDrawFuncs[] = {
    func_80B5458C,
    func_80B5944C,
};

s32 EnZl3_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfx) {
    EnZl3* this = (EnZl3*)thisx;

    if (this->unk_308 < 0 || this->unk_308 >= ARRAY_COUNT(sOverrideLimbDrawFuncs) ||
        sOverrideLimbDrawFuncs[this->unk_308] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画前処理モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return 0;
    }
    return sOverrideLimbDrawFuncs[this->unk_308](play, limbIndex, dList, pos, rot, thisx, gfx);
}

void func_80B59FE8(EnZl3* this, PlayState* play) {
}

void func_80B59FF4(EnZl3* this, PlayState* play) {
    s32 pad[2];
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTex = sEyeTextures[eyeTexIndex];
    s16 mouthTexIndex = this->mouthTexIndex;
    SkelAnime* skelAnime = &this->skelAnime;
    void* mouthTex = sMouthTextures[mouthTexIndex];
    s32 pad2;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zl3.c", 2165);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x9, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0xA, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0xB, &D_80116280[2]);

    POLY_OPA_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       EnZl3_OverrideLimbDraw, EnZl3_PostLimbDraw, this, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zl3.c", 2190);
}

void func_80B5A1D0(EnZl3* this, PlayState* play) {
    s32 pad[2];
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTex = sEyeTextures[eyeTexIndex];
    s16 mouthTexIndex = this->mouthTexIndex;
    SkelAnime* skelAnime = &this->skelAnime;
    void* mouthTex = sMouthTextures[mouthTexIndex];
    s32 pad2;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zl3.c", 2205);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 8, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 9, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 10, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 11, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       EnZl3_OverrideLimbDraw, NULL, this, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zl3.c", 2234);
}

static EnZl3DrawFunc sDrawFuncs[] = {
    func_80B59FE8,
    func_80B59FF4,
    func_80B5A1D0,
};

void EnZl3_Draw(Actor* thisx, PlayState* play) {
    EnZl3* this = (EnZl3*)thisx;

    if (this->drawConfig < 0 || this->drawConfig >= 3 || sDrawFuncs[this->drawConfig] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawConfig](this, play);
}

ActorProfile En_Zl3_Profile = {
    /**/ ACTOR_EN_ZL3,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZL2,
    /**/ sizeof(EnZl3),
    /**/ EnZl3_Init,
    /**/ EnZl3_Destroy,
    /**/ EnZl3_Update,
    /**/ EnZl3_Draw,
};
