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

void En_Zl3_Actor_ct(Actor* thisx, PlayState* play);
void En_Zl3_Actor_dt(Actor* thisx, PlayState* play);
void En_Zl3_Actor_main(Actor* thisx, PlayState* play);
void En_Zl3_Actor_draw(Actor* thisx, PlayState* play);
void En_Zl3_Setup_FinalToEscape(EnZl3* this, PlayState* play);

static ColliderCylinderInitType1 En_Zl3_OcInfoData_forStand = {
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

static void* en_zl3_eye[] = { gZelda2EyeOpenTex, gZelda2EyeHalfTex, gZelda2EyeShutTex, gZelda2Eye03Tex,
                                gZelda2Eye04Tex,   gZelda2Eye05Tex,   gZelda2Eye06Tex,   NULL };

static void* en_zl3_mouth[] = { gZelda2MouthSeriousTex, gZelda2MouthHappyTex, gZelda2MouthOpenTex };

static s32 move = 0;

static Vec3f D_80B5A46C = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80B5A478 = { 0.0f, 10.0f, 0.0f };

void En_Zl3_ct_forCorect(Actor* thisx, PlayState* play) {
    EnZl3* this = (EnZl3*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set3(play, &this->collider, &this->actor, &En_Zl3_OcInfoData_forStand);
}

void En_Zl3_Excute_Corect_forStand(EnZl3* this, PlayState* play) {
    ColliderCylinder* collider = &this->collider;
    s32 pad[4];

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &collider->base);
}

void En_Zl3_Actor_dt(Actor* thisx, PlayState* play) {
    EnZl3* this = (EnZl3*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void En_Zl3_Set_BGMStart(void) {
    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_ESCAPE);
}

BossGanon2* En_Zl3_Search_Boss_Ganon2(EnZl3* this, PlayState* play) {
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

void En_Zl3_set_eye_pattern(EnZl3* this) {
    s32 pad[2];
    s16* eyeTexIndex = &this->eyeTexIndex;
    s16* blinkTimer = &this->blinkTimer;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = get_random_timer(60, 60);
    }
    *eyeTexIndex = *blinkTimer;
    if (*eyeTexIndex >= 3) {
        *eyeTexIndex = 0;
    }
}

void En_Zl3_setNum_eye_pattern(EnZl3* this, s16 index) {
    this->eyeTexIndex = index;
}

void En_Zl3_set_mouth_pattern(EnZl3* this, s16 index) {
    this->mouthTexIndex = index;
}

void Birth_Item2_In_En_Zl3(EnZl3* this, PlayState* play) {
    Vec3f* thisPos = &this->actor.world.pos;
    Vec3f sp20;

    sp20.x = thisPos->x + ((fqrand() - 0.5f) * 10.0f);
    sp20.y = thisPos->y;
    sp20.z = thisPos->z + ((fqrand() - 0.5f) * 10.0f);
    Item_set0(play, &sp20, ITEM00_RECOVERY_HEART);
}

void Birth_En_River_Sound_In_En_Zl3(EnZl3* this, PlayState* play) {
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_RIVER_SOUND, -442.0f, 4102.0f, -371.0f, 0, 0, 0, 0x12);
}

void En_Zl3_BGcheck(EnZl3* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

void En_Zl3_BGcheck_bitclr(EnZl3* this) {
    this->actor.bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_WALL);
}

void En_Zl3_Calc_turn_front(EnZl3* this) {
    s32 pad[2];
    Vec3s* headRot = &this->interactInfo.headRot;
    Vec3s* torsoRot = &this->interactInfo.torsoRot;

    add_calc_short_angle2(&headRot->x, 0, 20, 6200, 100);
    add_calc_short_angle2(&headRot->y, 0, 20, 6200, 100);
    add_calc_short_angle2(&torsoRot->x, 0, 20, 6200, 100);
    add_calc_short_angle2(&torsoRot->y, 0, 20, 6200, 100);
}

void En_Zl3_Calc_turn_link(EnZl3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) - 16.0f;
    eye_moveM(&this->actor, &this->interactInfo, kREG(17) + 0xC, NPC_TRACKING_HEAD_AND_TORSO);
}

s32 En_Zl3_SetAngle_forLink(EnZl3* this) {
    s16 yawTowardsPlayer = this->actor.yawTowardsPlayer;
    s16* rotY = &this->actor.world.rot.y;
    s16* unk_3D0 = &this->unk_3D0;
    s16 retVal;
    s16 pad[2];

    add_calc_short_angle2(unk_3D0, ABS((s16)(yawTowardsPlayer - *rotY)), 5, 6200, 100);
    retVal = add_calc_short_angle2(rotY, yawTowardsPlayer, 5, *unk_3D0, 100);
    this->actor.shape.rot.y = *rotY;
    return retVal;
}

void En_Zl3_SetAngle_forLink2(EnZl3* this) {
    s16 yawTowardsPlayer = this->actor.yawTowardsPlayer;
    s16* rotY = &this->actor.world.rot.y;

    if (ABS((s16)(yawTowardsPlayer - *rotY)) >= 0x1556) {
        move = 1;
    }

    if (move != 0) {
        if (!En_Zl3_SetAngle_forLink(this)) {
            move = 0;
        }
    } else {
        this->unk_3D0 = 0;
    }
}

s32 En_Zl3_Animation_Base(EnZl3* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

s32 En_Zl3_Get_Command_fromGannon(EnZl3* this) {
    return this->unk_3C8;
}

void En_Zl3_Reset_Command_fromGannon(EnZl3* this, u8 arg1) {
    this->unk_3C8 = arg1;
}

void En_Zl3_CalcWaveHearV(EnZl3* thisx, s16 y, s32 idx) {
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

void En_Zl3_CalcWaveHearH(EnZl3* this, s16 z, s32 idx) {
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

void En_Zl3_CalcWaveHearD(EnZl3* this, s16 arg1, s32 arg2) {
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

s32 En_Zl3_SetWaveHear(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
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

        Matrix_push();
        Matrix_translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);
        Matrix_rotateXYZ(rot->x, rot->y, rot->z, MTXMODE_APPLY);
        Matrix_push();
        Matrix_translate(362.0f, -133.0f, 0.0f, MTXMODE_APPLY);
        Matrix_get(&sp38);
        Matrix_to_rotate_new(&sp38, &sp30, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl3_CalcWaveHearV(this, sp30.y, 0);
            En_Zl3_CalcWaveHearD(this, sp30.x, 1);
            En_Zl3_CalcWaveHearH(this, sp30.z, 2);
        }
        Matrix_rotateXYZ(unk_28C[0] + kREG(31), unk_28C[1] + kREG(32), unk_28C[2] + kREG(33), MTXMODE_APPLY);
        Matrix_translate(-188.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[0], "../z_en_zl3.c", 1490);
        Matrix_get(&sp38);
        Matrix_to_rotate_new(&sp38, &sp30, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl3_CalcWaveHearV(this, sp30.y, 3);
        }
        Matrix_rotateXYZ(unk_28C[3] + kREG(34), unk_28C[4] + kREG(35), unk_28C[5] + kREG(36), MTXMODE_APPLY);
        Matrix_translate(-410.0f, -184.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[1], "../z_en_zl3.c", 1534);
        Matrix_get(&sp38);
        Matrix_to_rotate_new(&sp38, &sp30, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl3_CalcWaveHearD(this, sp30.x, 7);
        }
        Matrix_rotateXYZ(unk_28C[6] + kREG(37), unk_28C[7] + kREG(38), unk_28C[8] + kREG(39), MTXMODE_APPLY);
        Matrix_translate(-1019.0f, -26.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[2], "../z_en_zl3.c", 1554);
        Matrix_pull();
        Matrix_push();
        Matrix_translate(467.0f, 265.0f, 389.0f, MTXMODE_APPLY);
        Matrix_get(&sp38);
        Matrix_to_rotate_new(&sp38, &sp30, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl3_CalcWaveHearV(this, sp30.y, 9);
            En_Zl3_CalcWaveHearD(this, sp30.x, 10);
            En_Zl3_CalcWaveHearH(this, sp30.z, 11);
        }
        Matrix_rotateXYZ(unk_28C[9] + kREG(40), unk_28C[10] + kREG(41), unk_28C[11] + kREG(42), MTXMODE_APPLY);
        Matrix_translate(-427.0f, -1.0f, -3.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[3], "../z_en_zl3.c", 1579);
        Matrix_get(&sp38);
        Matrix_to_rotate_new(&sp38, &sp30, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl3_CalcWaveHearV(this, sp30.y, 12);
            En_Zl3_CalcWaveHearD(this, sp30.x, 13);
            En_Zl3_CalcWaveHearH(this, sp30.z, 14);
        }
        Matrix_rotateXYZ(unk_28C[12] + kREG(43), unk_28C[13] + kREG(44), unk_28C[14] + kREG(45), MTXMODE_APPLY);
        Matrix_translate(-446.0f, -52.0f, 84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[4], "../z_en_zl3.c", 1598);
        Matrix_pull();
        Matrix_push();
        Matrix_translate(467.0f, 265.0f, -389.0f, MTXMODE_APPLY);
        Matrix_get(&sp38);
        Matrix_to_rotate_new(&sp38, &sp30, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl3_CalcWaveHearV(this, sp30.y, 15);
            En_Zl3_CalcWaveHearD(this, sp30.x, 16);
            En_Zl3_CalcWaveHearH(this, sp30.z, 17);
        }
        Matrix_rotateXYZ(unk_28C[15] + kREG(46), unk_28C[16] + kREG(47), unk_28C[17] + kREG(48), MTXMODE_APPLY);
        Matrix_translate(-427.0f, -1.0f, 3.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[5], "../z_en_zl3.c", 1623);
        Matrix_get(&sp38);
        Matrix_to_rotate_new(&sp38, &sp30, 0);
        if (!_Game_play_isPause(play)) {
            En_Zl3_CalcWaveHearV(this, sp30.y, 18);
            En_Zl3_CalcWaveHearD(this, sp30.x, 19);
            En_Zl3_CalcWaveHearH(this, sp30.z, 20);
        }
        Matrix_rotateXYZ(unk_28C[18] + kREG(49), unk_28C[19] + kREG(50), unk_28C[20] + kREG(51), MTXMODE_APPLY);
        Matrix_translate(-446.0f, -52.0f, -84.0f, MTXMODE_APPLY);
        MATRIX_TO_MTX(&sp78[6], "../z_en_zl3.c", 1642);
        Matrix_pull();
        Matrix_pull();
        this->unk_2FC = 1;
    } else if (limbIndex == 7) {
        rot->x += torsoRot->y;
        rot->y -= torsoRot->x;
    }
    return false;
}

void En_Zl3_SetOcarina(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnZl3* this = (EnZl3*)thisx;
    s32 pad;

    if (limbIndex == 13) {
        Vec3f sp34 = D_80B5A46C;
        s32 pad2;

        Matrix_Position(&sp34, &this->unk_31C);
    } else if (limbIndex == 14) {
        Vec3f sp24 = D_80B5A478;
        Vec3f sp18;

        Matrix_Position(&sp24, &sp18);
        this->actor.focus.pos.x = sp18.x;
        this->actor.focus.pos.y = sp18.y;
        this->actor.focus.pos.z = sp18.z;
        this->actor.focus.rot.x = this->actor.world.rot.x;
        this->actor.focus.rot.y = this->actor.world.rot.y;
        this->actor.focus.rot.z = this->actor.world.rot.z;
    }
}

s32 En_Zl3_GetUpper_arg_data(EnZl3* this) {
    s32 params = PARAMS_GET_U(this->actor.params, 8, 8);

    return params;
}

s32 En_Zl3_GetMidder_arg_data(EnZl3* this) {
    s32 params = PARAMS_GET_U(this->actor.params, 4, 4);

    return params;
}

s32 En_Zl3_GetLower_arg_data(EnZl3* this) {
    s32 params = PARAMS_GET_U(this->actor.params, 0, 4);

    return params;
}

void En_Zl3_Change_Bank(EnZl3* this, PlayState* play) {
    s32 objectSlot = this->zl2Anime2ObjectSlot;

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
}

void En_Zl3_Change_Anime(EnZl3* this, AnimationHeader* animation, u8 arg2, f32 morphFrames, s32 arg4) {
    f32 frameCount = Si2_anime_end_frame(animation);
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

    Skeleton_Info2_init(&this->skelAnime, animation, playbackSpeed, unk0, fc, arg2, morphFrames);
}

void Birth_Eg_In_En_Zl3(EnZl3* this, PlayState* play) {
    f32 posX = this->actor.world.pos.x;
    f32 posY = this->actor.world.pos.y;
    f32 posZ = this->actor.world.pos.z;

    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_EG, posX, posY, posZ, 0, 0, 0, 0);
}

#include "z_en_zl3_inFinal.inc.c"

#include "z_en_zl3_inFinal2.inc.c"

#include "z_en_zl3_inEscape.inc.c"

void En_Zl3_Set_RevengeSound(EnZl3* this, PlayState* play) {
    if (En_Zl3_inEscape_Check_TimeOnly(this, play)) {
        Nai_FxFlagEntry(NA_SE_OC_REVENGE, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

void En_Zl3_Setup_FinalToEscape(EnZl3* this, PlayState* play) {
    Actor* thisx = &this->actor;

    Actor_Environment_sw_On(play, 0x36);
    total_event_timer_set(180);
    Birth_Eg_In_En_Zl3(this, play);
    Birth_En_River_Sound_In_En_Zl3(this, play);
    CLEAR_EVENTCHKINF(EVENTCHKINF_C7);
    En_Zl3_Init_path_info(this, play);
    z_common_data.healthAccumulator = 320;
    magic_mode_check(play);
    this->action = 27;
    this->drawConfig = 1;
}

void En_Zl3_main_init(EnZl3* this, PlayState* play) {
    s32 sp2C = En_Zl3_GetLower_arg_data(this);

    this->unk_3DC = Si2_anime_end_frame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_0091D8));
    this->unk_3E0 = Si2_anime_end_frame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_00A598));
    this->unk_3E4 = Si2_anime_end_frame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_00A334));
    this->unk_3F4 = Si2_anime_end_frame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_001110));
    this->unk_3EC = Si2_anime_end_frame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_002348));
    this->unk_3F0 = Si2_anime_end_frame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_002E54));
    this->unk_3E8 = Si2_anime_end_frame(SEGMENTED_TO_VIRTUAL(&gZelda2Anime2Anim_001D8C));

    switch (sp2C) {
        case 0:
            En_Zl3_Actor_inFinal_Init(this, play);
            break;
        case 1:
            En_Zl3_Actor_inFinal2_Init(this, play);
            break;
        case 3:
            En_Zl3_Actor_inEscape_Init(this, play);
            break;
        default:
            PRINTF(VT_FGCOL(RED) " En_Oa3 の arg_data がおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
            Actor_delete(&this->actor);
    }
}

void En_Zl3_main_bank(EnZl3* this, PlayState* play) {
    s32 pad;
    ObjectContext* objectCtx = &play->objectCtx;
    s32 objectSlot = Object_Exchange_bank_check(objectCtx, OBJECT_ZL2_ANIME2);
    s32 pad2;

#if DEBUG_FEATURES
    if (objectSlot < 0) {
        PRINTF(VT_FGCOL(RED) "En_Zl3_main_bankアニメーションのバンクを読めない!!!!!!!!!!!!\n" VT_RST);
        return;
    }
#endif

    if (Object_Exchange_bank_dma_check(objectCtx, objectSlot)) {
        this->zl2Anime2ObjectSlot = objectSlot;
        En_Zl3_Change_Bank(this, play);
        En_Zl3_main_init(this, play);
    }
}

void En_Zl3_Actor_main(Actor* thisx, PlayState* play) {
    static EnZl3ActionFunc proc[] = {
        En_Zl3_main_bank, En_Zl3_inFinal_main_lookhand, En_Zl3_inFinal_main_swoop, En_Zl3_inFinal_main_lookganon, En_Zl3_inFinal_main_lookup, En_Zl3_inFinal_main_stand, En_Zl3_inFinal_main_walk,
        En_Zl3_inFinal2_main_stand, En_Zl3_inFinal2_main_press, En_Zl3_inFinal2_main_lookup, En_Zl3_inFinal2_main_frighten, En_Zl3_inFinal2_main_relief, En_Zl3_inFinal2_main_handdown, En_Zl3_inFinal2_main_greet,
        En_Zl3_inFinal2_main_surprise, En_Zl3_inFinal2_main_looksword, En_Zl3_inFinal2_main_watch, En_Zl3_inFinal2_main_avert, En_Zl3_inFinal2_main_watch_advice, En_Zl3_inFinal2_main_Magic, En_Zl3_inFinal2_main_Endmagic,
        En_Zl3_inFinal2_main_Advice, En_Zl3_inFinal2_main_Callsage, En_Zl3_inFinal2_main_Seal, En_Zl3_inFinal2_main_Disheart, En_Zl3_inEscape_main_start, En_Zl3_inEscape_main_greet, En_Zl3_inEscape_main_turn,
        En_Zl3_inEscape_main_stand, En_Zl3_inEscape_main_encourage, En_Zl3_inEscape_main_avert, En_Zl3_inEscape_main_surprise, En_Zl3_inEscape_main_prizon, En_Zl3_inEscape_main_thanks, En_Zl3_inEscape_main_walk,
        En_Zl3_inEscape_main_anxious, En_Zl3_inEscape_main_keep, En_Zl3_inEscape_main_pray, En_Zl3_inEscape_main_handdown, En_Zl3_inEscape_main_runaway,
    };

    EnZl3* this = (EnZl3*)thisx;

    if (this->action < 0 || this->action >= ARRAY_COUNT(proc) || proc[this->action] == NULL) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->action](this, play);
}

void En_Zl3_Actor_ct(Actor* thisx, PlayState* play) {
    EnZl3* this = (EnZl3*)thisx;
    ActorShape* shape = &this->actor.shape;
    s32 pad;

    PRINTF("ゼルダ姫のEn_Zl3_Actor_ct通すよ!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    Shape_Info_init(shape, 0.0f, Actor_shadow_circle, 30.0f);
    shape->shadowAlpha = 0;
    En_Zl3_ct_forCorect(thisx, play);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gZelda2Skel, NULL, this->jointTable, this->morphTable, 15);

    switch (En_Zl3_GetLower_arg_data(this)) {
        case 1:
            z_common_data.subTimerState = SUBTIMER_STATE_OFF;
            break;
        case 3:
            En_Zl3_Set_RevengeSound(this, play);
            break;
    }

    PRINTF("ゼルダ姫のEn_Zl3_Actor_ctは通った!!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

s32 En_Zl3_Actor_beforedraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfx) {
    static OverrideLimbDraw proc[] = {
        En_Zl3_SetWaveHear,
        En_Zl3_DrawHeadAngle,
    };

    EnZl3* this = (EnZl3*)thisx;

    if (this->unk_308 < 0 || this->unk_308 >= ARRAY_COUNT(proc) ||
        proc[this->unk_308] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画前処理モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return 0;
    }
    return proc[this->unk_308](play, limbIndex, dList, pos, rot, thisx, gfx);
}

void En_Zl3_Actor_draw_none(EnZl3* this, PlayState* play) {
}

void En_Zl3_Actor_draw_normal(EnZl3* this, PlayState* play) {
    s32 pad[2];
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTex = en_zl3_eye[eyeTexIndex];
    s16 mouthTexIndex = this->mouthTexIndex;
    SkelAnime* skelAnime = &this->skelAnime;
    void* mouthTex = en_zl3_mouth[mouthTexIndex];
    s32 pad2;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zl3.c", 2165);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x9, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0xA, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0xB, &Actor_change_render_mode[2]);

    POLY_OPA_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       En_Zl3_Actor_beforedraw, En_Zl3_SetOcarina, this, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zl3.c", 2190);
}

void En_Zl3_Actor_draw_alpha(EnZl3* this, PlayState* play) {
    s32 pad[2];
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTex = en_zl3_eye[eyeTexIndex];
    s16 mouthTexIndex = this->mouthTexIndex;
    SkelAnime* skelAnime = &this->skelAnime;
    void* mouthTex = en_zl3_mouth[mouthTexIndex];
    s32 pad2;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zl3.c", 2205);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 8, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 9, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 10, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 11, &Actor_change_render_mode[0]);

    POLY_XLU_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       En_Zl3_Actor_beforedraw, NULL, this, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zl3.c", 2234);
}

static EnZl3DrawFunc proc[] = {
    En_Zl3_Actor_draw_none,
    En_Zl3_Actor_draw_normal,
    En_Zl3_Actor_draw_alpha,
};

void En_Zl3_Actor_draw(Actor* thisx, PlayState* play) {
    EnZl3* this = (EnZl3*)thisx;

    if (this->drawConfig < 0 || this->drawConfig >= 3 || proc[this->drawConfig] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->drawConfig](this, play);
}

ActorProfile En_Zl3_Profile = {
    /**/ ACTOR_EN_ZL3,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZL2,
    /**/ sizeof(EnZl3),
    /**/ En_Zl3_Actor_ct,
    /**/ En_Zl3_Actor_dt,
    /**/ En_Zl3_Actor_main,
    /**/ En_Zl3_Actor_draw,
};
