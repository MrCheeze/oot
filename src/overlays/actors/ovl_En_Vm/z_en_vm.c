/*
 * File: z_en_vm.c
 * Overlay: ovl_En_Vm
 * Description: Beamos
 */

#include "z_en_vm.h"
#include "assets/objects/object_vm/object_vm.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_vm_Actor_ct(Actor* thisx, PlayState* play);
void En_vm_Actor_dt(Actor* thisx, PlayState* play);
void En_vm_move(Actor* thisx, PlayState* play);
void En_vm_display(Actor* thisx, PlayState* play2);

void En_vm_Actor_mode_non_move_init(EnVm* this);
void En_vm_Actor_mode_non_move(EnVm* this, PlayState* play);
void En_vm_Actor_mode_beam_shot_init(EnVm* this);
void En_vm_Actor_mode_beam_shot(EnVm* this, PlayState* play);
void En_vm_Actor_mode_damage(EnVm* this, PlayState* play);
void En_vm_Actor_mode_dead(EnVm* this, PlayState* play);

ActorProfile En_Vm_Profile = {
    /**/ ACTOR_EN_VM,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_VM,
    /**/ sizeof(EnVm),
    /**/ En_vm_Actor_ct,
    /**/ En_vm_Actor_dt,
    /**/ En_vm_move,
    /**/ En_vm_display,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 25, 70, 0, { 0, 0, 0 } },
};

static ColliderQuadInit AtInfoData = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderQuadInit AcInfoData = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static Vec3f dam = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80B2EAF8 = { 0.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB04 = { 500.0f, 0.0f, 0.0f };

static Vec3f D_80B2EB10 = { -500.0f, 0.0f, 0.0f };

static Vec3f local_head = { 0.0f, 0.0f, 0.0f };

static Vec3f local_head2 = { 0.0f, 0.0f, 1600.0f };

static Vec3f local_eye = { 1000.0f, 700.0f, 2000.0f };

static Vec3f local_eye2 = { 1000.0f, -700.0f, 2000.0f };

static Vec3f local_eye3 = { -1000.0f, 700.0f, 1500.0f };

static Vec3f local_eye4 = { -1000.0f, -700.0f, 1500.0f };

static Vec3f sword_top = { 500.0f, 0.0f, 0.0f };

static Vec3f sword_root = { -500.0f, 0.0f, 0.0f };

static Vec3f shadow_scale = { 0.4f, 0.4f, 0.4f };

static void* db_txt[] = {
    gEffEnemyDeathFlame1Tex, gEffEnemyDeathFlame2Tex,  gEffEnemyDeathFlame3Tex, gEffEnemyDeathFlame4Tex,
    gEffEnemyDeathFlame5Tex, gEffEnemyDeathFlame6Tex,  gEffEnemyDeathFlame7Tex, gEffEnemyDeathFlame8Tex,
    gEffEnemyDeathFlame9Tex, gEffEnemyDeathFlame10Tex,
};

void En_Vm_actor_set_process(EnVm* this, EnVmActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_vm_Actor_ct(Actor* thisx, PlayState* play) {
    EnVm* this = (EnVm*)thisx;

    Skeleton_Info2_M_ct(play, &this->skelAnime, &gBeamosSkel, &gBeamosAnim, this->jointTable, this->morphTable, 11);
    Shape_Info_init(&thisx->shape, 0.0f, NULL, 0.0f);
    ClObjPipe_ct(play, &this->colliderCylinder);
    ClObjPipe_set5(play, &this->colliderCylinder, thisx, &OcInfoData);
    ClObjSwrd_ct(play, &this->colliderQuad1);
    ClObjSwrd_set5(play, &this->colliderQuad1, thisx, &AtInfoData);
    ClObjSwrd_ct(play, &this->colliderQuad2);
    ClObjSwrd_set5(play, &this->colliderQuad2, thisx, &AcInfoData);
    this->beamSightRange = PARAMS_GET_NOMASK(thisx->params, 8) * 40.0f;
    thisx->params &= 0xFF;
    thisx->naviEnemyId = NAVI_ENEMY_BEAMOS;

    if (thisx->params == BEAMOS_LARGE) {
        thisx->colChkInfo.health = 2;
        Actor_set_scale(thisx, 0.014f);
    } else {
        thisx->colChkInfo.health = 1;
        Actor_set_scale(thisx, 0.01f);
    }

    En_vm_Actor_mode_non_move_init(this);
}

void En_vm_Actor_dt(Actor* thisx, PlayState* play) {
    EnVm* this = (EnVm*)thisx;

    ClObjPipe_dt(play, &this->colliderCylinder);
}

void En_vm_Actor_mode_non_move_init(EnVm* this) {
    f32 frameCount = Si2_anime_end_frame(&gBeamosAnim);

    Skeleton_Info2_init(&this->skelAnime, &gBeamosAnim, 1.0f, frameCount, frameCount, ANIMMODE_ONCE, 0.0f);
    this->unk_25E = this->unk_260 = 0;
    this->unk_21C = 0;
    this->timer = 10;
    En_Vm_actor_set_process(this, En_vm_Actor_mode_non_move);
}

void En_vm_Actor_mode_non_move(EnVm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 dist;
    s16 headRot;
    s16 pad;
    s16 pitch;

    switch (this->unk_25E) {
        case 0:
            add_calc_short_angle2(&this->beamRot.x, 0, 10, 1500, 0);
            headRot = this->actor.yawTowardsPlayer - this->headRotY - this->actor.shape.rot.y;
            pitch = search_position_angleX(&this->beamPos1, &player->actor.world.pos);

            if (pitch > 0x1B91) {
                pitch = 0x1B91;
            }

            dist = this->beamSightRange - this->actor.xzDistToPlayer;

            if (this->actor.xzDistToPlayer <= this->beamSightRange && ABS(headRot) <= 0x2710 && pitch >= 0xE38 &&
                this->actor.yDistToPlayer <= 80.0f && this->actor.yDistToPlayer >= -160.0f) {
                add_calc_short_angle2(&this->beamRot.x, pitch, 10, 0xFA0, 0);
                if (add_calc_short_angle2(&this->headRotY, this->actor.yawTowardsPlayer - this->actor.shape.rot.y, 1,
                                       (ABS((s16)(dist * 180.0f)) / 3) + 0xFA0, 0) <= 5460) {
                    this->timer--;
                    if (this->timer == 0) {
                        this->unk_25E++;
                        this->skelAnime.curFrame = 0.0f;
                        this->skelAnime.startFrame = 0.0f;
                        this->skelAnime.playSpeed = 2.0f;
                        Actor_SE_set(&this->actor, NA_SE_EN_BIMOS_AIM);
                    }
                }
            } else {
                this->headRotY -= 0x1F4;
            }

            Skeleton_Info2_anime_play(&this->skelAnime);
            return;
        case 1:
            break;
        default:
            return;
    }

    add_calc_short_angle2(&this->headRotY, this->actor.yawTowardsPlayer - this->actor.shape.rot.y, 1, 0x1F40, 0);

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->unk_260++;
        this->skelAnime.curFrame = 0.0f;
    }

    if (this->unk_260 == 2) {
        this->beamRot.y = this->actor.yawTowardsPlayer;
        this->beamRot.x = search_position_angleX(&this->beamPos1, &player->actor.world.pos);

        if (this->beamRot.x > 0x1B91) {
            this->beamRot.x = 0x1B91;
        }

        if (this->beamRot.x < 0xAAA) {
            this->skelAnime.startFrame = this->skelAnime.curFrame = this->skelAnime.endFrame;
            this->unk_25E = this->unk_260 = 0;
            this->timer = 10;
            this->skelAnime.playSpeed = 1.0f;
        } else {
            this->skelAnime.curFrame = 6.0f;
            Effect_SS_Dd_ct0(play, &this->beamPos2, &dam, &dam, 150, -25, 0, 0, 255, 0, 255, 255, 255,
                                 16, 20);
            En_vm_Actor_mode_beam_shot_init(this);
        }
    }
}

void En_vm_Actor_mode_beam_shot_init(EnVm* this) {
    Skeleton_Info2_init(&this->skelAnime, &gBeamosAnim, 3.0f, 3.0f, 7.0f, ANIMMODE_ONCE, 0.0f);
    this->timer = 305;
    this->beamScale.x = 0.6f;
    this->beamSpeed = 40.0f;
    this->unk_21C = 1;
    this->colliderQuad1.base.atFlags &= ~AT_HIT;
    En_Vm_actor_set_process(this, En_vm_Actor_mode_beam_shot);
}

void En_vm_Actor_mode_beam_shot(EnVm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 pitch = search_position_angleX(&this->beamPos1, &player->actor.world.pos);
    f32 dist;
    Vec3f playerPos;

    if (pitch > 0x1B91) {
        pitch = 0x1B91;
    }

    if (this->colliderQuad1.base.atFlags & AT_HIT) {
        this->colliderQuad1.base.atFlags &= ~AT_HIT;
        this->timer = 0;

        if (this->beamScale.x > 0.1f) {
            this->beamScale.x = 0.1f;
        }
    }

    if (this->beamRot.x < 0xAAA || this->timer == 0) {
        add_calc(&this->beamScale.x, 0.0f, 1.0f, 0.03f, 0.0f);
        this->unk_260 = 0;

        if (this->beamScale.x == 0.0f) {
            this->beamScale.y = this->beamScale.z = 0.0f;
            En_vm_Actor_mode_non_move_init(this);
        }
    } else {
        if (--this->timer > 300) {
            return;
        }

        add_calc_short_angle2(&this->headRotY, -this->actor.shape.rot.y + this->actor.yawTowardsPlayer, 10, 0xDAC, 0);
        add_calc_short_angle2(&this->beamRot.y, this->actor.yawTowardsPlayer, 10, 0xDAC, 0);
        add_calc_short_angle2(&this->beamRot.x, pitch, 10, 0xDAC, 0);
        playerPos = player->actor.world.pos;

        if (player->actor.floorHeight > BGCHECK_Y_MIN) {
            playerPos.y = player->actor.floorHeight;
        }

        dist = search_position_distance(&this->beamPos1, &playerPos);
        add_calc(&this->beamScale.z, dist, 1.0f, this->beamSpeed, 0.0f);
        add_calc(&this->beamScale.x, 0.1f, 1.0f, 0.12f, 0.0f);
        Actor_SE_set(&this->actor, NA_SE_EN_BIMOS_LAZER - SFX_FLAG);

        if (this->unk_260 > 2) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderQuad1.base);
        }

        this->unk_260 = 3;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->skelAnime.curFrame = this->skelAnime.startFrame;
    }
}

void En_vm_Actor_mode_damage_init(EnVm* this) {
    Skeleton_Info2_init(&this->skelAnime, &gBeamosAnim, -1.0f, Si2_anime_end_frame(&gBeamosAnim), 0.0f, ANIMMODE_ONCE,
                     0.0f);
    this->unk_260 = 0;
    this->timer = 180;
    this->unk_25E = this->unk_260;
    this->unk_21C = 2;
    this->beamScale.z = 0.0f;
    this->beamScale.y = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    En_Vm_actor_set_process(this, En_vm_Actor_mode_damage);
}

void En_vm_Actor_mode_damage(EnVm* this, PlayState* play) {
    if (this->timer == 0) {
        if (Skeleton_Info2_anime_play(&this->skelAnime)) {
            this->unk_25E++;
            if (this->unk_25E == 3) {
                En_vm_Actor_mode_non_move_init(this);
            } else if (this->unk_25E == 1) {
                Skeleton_Info2_init(&this->skelAnime, &gBeamosAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gBeamosAnim),
                                 ANIMMODE_ONCE, 0.0f);
            } else {
                this->timer = 10;
                this->skelAnime.curFrame = 0.0f;
                this->skelAnime.playSpeed = 2.0f;
            }
        }
    } else {
        add_calc_short_angle2(&this->beamRot.x, 0, 10, 0x5DC, 0);
        this->timer--;
        Skeleton_Info2_anime_play(&this->skelAnime);
    }
}

void En_vm_Actor_mode_dead_init(EnVm* this) {
    Skeleton_Info2_init(&this->skelAnime, &gBeamosAnim, -1.0f, Si2_anime_end_frame(&gBeamosAnim), 0.0f, ANIMMODE_ONCE,
                     0.0f);
    this->timer = 33;
    this->unk_25E = this->unk_260 = 0;
    this->unk_21C = 3;
    this->beamScale.z = 0.0f;
    this->beamScale.y = 0.0f;
    this->actor.shape.yOffset = -5000.0f;
    this->actor.world.pos.y += 5000.0f * this->actor.scale.y;
    this->actor.velocity.y = 8.0f;
    this->actor.gravity = -0.5f;
    this->actor.speed = fqrand() + 1.0f;
    this->actor.world.rot.y = rnd_fx(65535.0f);
    En_Vm_actor_set_process(this, En_vm_Actor_mode_dead);
}

void En_vm_Actor_mode_dead(EnVm* this, PlayState* play) {
    this->beamRot.x += 0x5DC;
    this->headRotY += 0x9C4;
    Actor_position_moveF(&this->actor);

    if (--this->timer == 0) {
        EnBom* bomb = (EnBom*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                          this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0x6FF, BOMB_BODY);

        if (bomb != NULL) {
            bomb->timer = 0;
        }

        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xA0);
        Actor_delete(&this->actor);
    }
}

void En_Vm_damage_proc(EnVm* this, PlayState* play) {
    EnBom* bomb;

    if (BlastVsMyCheck(play, &this->colliderCylinder.base) != NULL) {
        this->actor.colChkInfo.health--;
        PRINTF("hp down %d\n", this->actor.colChkInfo.health);
    } else {
        if (!(this->colliderQuad2.base.acFlags & AC_HIT) || this->unk_21C == 2) {
            return;
        }
        this->colliderQuad2.base.acFlags &= ~AC_HIT;
    }

    if (this->actor.colChkInfo.health != 0) {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
        En_vm_Actor_mode_damage_init(this);
    } else {
        bomb = (EnBom*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                   this->actor.world.pos.y + 20.0f, this->actor.world.pos.z, 0, 0, 0x601, BOMB_BODY);

        if (bomb != NULL) {
            bomb->timer = 0;
        }

        En_vm_Actor_mode_dead_init(this);
    }
}

void En_vm_move(Actor* thisx, PlayState* play) {
    EnVm* this = (EnVm*)thisx;
    CollisionCheckContext* colChkCtx = &play->colChkCtx;

    if (this->actor.colChkInfo.health != 0) {
        En_Vm_damage_proc(this, play);
    }

    if (this->unk_260 == 4) {
        Effect_SS_Ds_ct1(play, &this->beamPos3, 20, -1, 255, 20);
        dust_fly_set2(play, &this->beamPos3, 6.0f, 1, 120, 20, 1);
        Actor_SE_set(&this->actor, NA_SE_EN_BIMOS_LAZER_GND - SFX_FLAG);
    }

    this->actionFunc(this, play);
    this->beamTexScroll += 3 << 2;

    if (this->actor.colChkInfo.health != 0 && this->unk_21C != 2) {
        Actor_SE_set(&this->actor, NA_SE_EN_BIMOS_ROLL_HEAD - SFX_FLAG);
    }

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colliderCylinder);
    CollisionCheck_setOC(play, colChkCtx, &this->colliderCylinder.base);

    if (this->actor.colorFilterTimer == 0 && this->actor.colChkInfo.health != 0) {
        CollisionCheck_setAC(play, colChkCtx, &this->colliderCylinder.base);
    }

    CollisionCheck_setAC(play, colChkCtx, &this->colliderQuad2.base);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += (6500.0f + this->actor.shape.yOffset) * this->actor.scale.y;
}

s32 en_vm_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnVm* this = (EnVm*)thisx;

    if (limbIndex == 2) {
        rot->x += this->beamRot.x;
        rot->y += this->headRotY;
    } else if (limbIndex == 10) {
        if (this->unk_21C == 3) {
            *dList = NULL;
        }
    }

    return false;
}

void en_vm_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnVm* this = (EnVm*)thisx;
    Vec3f sp80 = D_80B2EAF8;
    Vec3f sp74 = D_80B2EB04;
    Vec3f sp68 = D_80B2EB10;
    s32 pad;
    Vec3f posResult;
    CollisionPoly* poly;
    s32 bgId;
    f32 dist;

    if (limbIndex == 2) {
        Matrix_Position(&local_head, &this->beamPos1);
        Matrix_Position(&local_head2, &this->beamPos2);

        if (this->unk_260 >= 3) {
            poly = NULL;
            sp80.z = (this->beamScale.z + 500.0f) * (this->actor.scale.y * 10000.0f);
            Matrix_Position(&sp80, &this->beamPos3);

            if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->beamPos1, &this->beamPos3, &posResult, &poly, true, true,
                                        false, true, &bgId) == true) {
                this->beamScale.z = search_position_distance(&this->beamPos1, &posResult) - 5.0f;
                this->unk_260 = 4;
                this->beamPos3 = posResult;
            }
            if (this->beamScale.z != 0.0f) {
                dist = 100.0f;
                if (this->actor.scale.y > 0.01f) {
                    dist = 70.0f;
                }
                sp74.z = sp68.z = search_position_distance(&this->beamPos1, &this->beamPos3) * dist;
                Matrix_Position(&sword_top, &this->colliderQuad1.dim.quad[3]);
                Matrix_Position(&sword_root, &this->colliderQuad1.dim.quad[2]);
                Matrix_Position(&sp74, &this->colliderQuad1.dim.quad[1]);
                Matrix_Position(&sp68, &this->colliderQuad1.dim.quad[0]);
                CollisionCheck_Uty_setSword4Pos(&this->colliderQuad1, &this->colliderQuad1.dim.quad[0],
                                         &this->colliderQuad1.dim.quad[1], &this->colliderQuad1.dim.quad[2],
                                         &this->colliderQuad1.dim.quad[3]);
            }
        }
        Matrix_Position(&local_eye, &this->colliderQuad2.dim.quad[1]);
        Matrix_Position(&local_eye2, &this->colliderQuad2.dim.quad[0]);
        Matrix_Position(&local_eye3, &this->colliderQuad2.dim.quad[3]);
        Matrix_Position(&local_eye4, &this->colliderQuad2.dim.quad[2]);
        CollisionCheck_Uty_setSword4Pos(&this->colliderQuad2, &this->colliderQuad2.dim.quad[0],
                                 &this->colliderQuad2.dim.quad[1], &this->colliderQuad2.dim.quad[2],
                                 &this->colliderQuad2.dim.quad[3]);
    }
}

void En_vm_display(Actor* thisx, PlayState* play2) {
    EnVm* this = (EnVm*)thisx;
    PlayState* play = play2;
    Vec3f actorPos;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_vm.c", 1014);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_vm_display1,
                      en_vm_display2, this);
    actorPos = this->actor.world.pos;
    Shadow_draw(&actorPos, &shadow_scale, 255, play);

    if (this->unk_260 >= 3) {
        Matrix_translate(this->beamPos3.x, this->beamPos3.y + 10.0f, this->beamPos3.z, MTXMODE_NEW);
        Matrix_scale(0.8f, 0.8f, 0.8f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_vm.c", 1033);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 168);
        texture_z_cld_poly_xlu(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, 0);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(db_txt[play->gameplayFrames % 8]));
        gSPDisplayList(POLY_XLU_DISP++, gEffEnemyDeathFlameDL);
        Matrix_rotateY(32767.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_vm.c", 1044);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(db_txt[(play->gameplayFrames + 4) % 8]));
        gSPDisplayList(POLY_XLU_DISP++, gEffEnemyDeathFlameDL);
    }
    gSPSegment(POLY_OPA_DISP++, 0x08, tex_scroll(play->state.gfxCtx, 0, this->beamTexScroll));
    Matrix_translate(this->beamPos1.x, this->beamPos1.y, this->beamPos1.z, MTXMODE_NEW);
    Matrix_rotateXYZ(this->beamRot.x, this->beamRot.y, this->beamRot.z, MTXMODE_APPLY);
    Matrix_scale(this->beamScale.x * 0.1f, this->beamScale.x * 0.1f, this->beamScale.z * 0.0015f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_vm.c", 1063);
    gSPDisplayList(POLY_OPA_DISP++, gBeamosLaserDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_vm.c", 1068);
}
