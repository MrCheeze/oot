/*
 * File: z_en_fw.c
 * Overlay: ovl_En_Fw
 * Description: Flare Dancer Core
 */

#include "z_en_fw.h"
#include "assets/objects/object_fw/object_fw.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR)

void En_Fw_Actor_ct(Actor* thisx, PlayState* play);
void En_Fw_Actor_dt(Actor* thisx, PlayState* play);
void En_Fw_Actor_move(Actor* thisx, PlayState* play);
void En_Fw_Actor_draw(Actor* thisx, PlayState* play);
void fw_eff_dust_mv(EnFw* this);
void fw_eff_dust_dr(EnFw* this, PlayState* play);
void fw_eff_dust_ct(EnFw* this, Vec3f* initialPos, Vec3f* initialSpeed, Vec3f* accel, u8 initialTimer, f32 scale,
                          f32 scaleStep);
static void greeting(EnFw* this, PlayState* play);
static void run(EnFw* this, PlayState* play);
static void jump(EnFw* this, PlayState* play);
void stop(EnFw* this, PlayState* play);

ActorProfile En_Fw_Profile = {
    /**/ ACTOR_EN_FW,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_FW,
    /**/ sizeof(EnFw),
    /**/ En_Fw_Actor_ct,
    /**/ En_Fw_Actor_dt,
    /**/ En_Fw_Actor_move,
    /**/ En_Fw_Actor_draw,
};

static ColliderJntSphElementInit FwJntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x04 },
            { 0xFFCFFFFE, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 2, { { 1200, 0, 0 }, 16 }, 100 },
    },
};

static ColliderJntSphInit FwAllJntSphData = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    FwJntSphElemData,
};

static CollisionCheckInfoInit2 FwStatusData = { 8, 2, 25, 25, MASS_IMMOVABLE };

typedef enum EnFwAnimation {
    /* 0 */ ENFW_ANIM_0,
    /* 1 */ ENFW_ANIM_1,
    /* 2 */ ENFW_ANIM_2
} EnFwAnimation;

static AnimationInfo anime_ct_data[] = {
    { &gFlareDancerCoreInitRunCycleAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, 0.0f },
    { &gFlareDancerCoreRunCycleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -8.0f },
    { &gFlareDancerCoreEndRunCycleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
};

s32 fw_bound(EnFw* this, s32 totalBounces, f32 yVelocity) {
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.velocity.y > 0.0f)) {
        // not on the ground or moving upwards.
        return false;
    }

    Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
    this->bounceCnt--;
    if (this->bounceCnt <= 0) {
        if (this->bounceCnt == 0) {
            this->bounceCnt = 0;
            this->actor.velocity.y = 0.0f;
            return true;
        }
        this->bounceCnt = totalBounces;
    }
    this->actor.velocity.y = yVelocity;
    this->actor.velocity.y *= (f32)this->bounceCnt / totalBounces;
    return true;
}

s32 player_search(EnFw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    CollisionPoly* poly;
    s32 bgId;
    Vec3f collisionPos;

    if (this->actor.xzDistToPlayer > 300.0f) {
        return false;
    }

    if (ABS((s16)((f32)this->actor.yawTowardsPlayer - (f32)this->actor.shape.rot.y)) > 0x1C70) {
        return false;
    }

    if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &player->actor.world.pos, &collisionPos, &poly,
                                true, false, false, true, &bgId)) {
        return false;
    }

    return true;
}

static Vec3f* go_to_next(Vec3f* dst, EnFw* this, f32 radius, s16 dir) {
    s16 angle;
    Vec3f posAdj;

    // increase rotation around circle ~30 degrees.
    angle = search_position_angleY(&this->actor.parent->home.pos, &this->actor.world.pos) + (dir * 0x1554);
    posAdj.x = (sin_s(angle) * radius) + this->actor.parent->home.pos.x;
    posAdj.z = (cos_s(angle) * radius) + this->actor.parent->home.pos.z;
    posAdj.x -= this->actor.world.pos.x;
    posAdj.z -= this->actor.world.pos.z;
    *dst = posAdj;
    return dst;
}

static s32 hitcheck(EnFw* this, PlayState* play) {
    ColliderElement* elem;

    if (this->collider.base.acFlags & AC_HIT) {
        elem = &this->collider.elements[0].base;
        if (elem->acHitElem->atDmgInfo.dmgFlags & DMG_HOOKSHOT) {
            this->lastDmgHook = true;
        } else {
            this->lastDmgHook = false;
        }
        this->collider.base.acFlags &= ~AC_HIT;
        if (hp_down(&this->actor) <= 0) {
            if (this->actor.parent->colChkInfo.health <= 8) {
                Actor_info_finish(play, &this->actor);
                this->actor.parent->colChkInfo.health = 0;
            } else {
                this->actor.parent->colChkInfo.health -= 8;
            }
            this->returnToParentTimer = 0;
        }
        return true;
    } else {
        return false;
    }
}

static s32 set_dust_effect(EnFw* this, u8 timer, f32 scale, f32 scaleStep, s32 dustCnt, f32 radius, f32 xzAccel, f32 yAccel) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    s16 angle;
    s32 i;

    pos = this->actor.world.pos;
    pos.y = this->actor.floorHeight + 2.0f;
    angle = ((fqrand() - 0.5f) * 0x10000);
    i = dustCnt;
    while (i >= 0) {
        accel.x = (fqrand() - 0.5f) * xzAccel;
        accel.y = yAccel;
        accel.z = (fqrand() - 0.5f) * xzAccel;
        pos.x = (sin_s(angle) * radius) + this->actor.world.pos.x;
        pos.z = (cos_s(angle) * radius) + this->actor.world.pos.z;
        fw_eff_dust_ct(this, &pos, &velocity, &accel, timer, scale, scaleStep);
        angle += (s16)(0x10000 / dustCnt);
        i--;
    }
    return 0;
}

void En_Fw_Actor_ct(Actor* thisx, PlayState* play) {
    EnFw* this = (EnFw*)thisx;

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gFlareDancerCoreSkel, NULL, this->jointTable, this->morphTable, 11);
    npc_anime_ct(&this->skelAnime, anime_ct_data, ENFW_ANIM_0);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 20.0f);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &FwAllJntSphData, this->sphs);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(0x10), &FwStatusData);
    Actor_set_scale(&this->actor, 0.01f);
    this->runDirection = -this->actor.params;
    this->actionFunc = greeting;
    this->actor.gravity = -1.0f;
}

void En_Fw_Actor_dt(Actor* thisx, PlayState* play) {
    EnFw* this = (EnFw*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

static void greeting(EnFw* this, PlayState* play) {
    if (fw_bound(this, 3, 8.0f) && this->bounceCnt == 0) {
        this->returnToParentTimer = get_random_timer(300, 150);
        this->actionFunc = run;
    }
}

static void run(EnFw* this, PlayState* play) {
    f32 tmpAngle;
    s16 curFrame;
    f32 facingDir;
    EnBom* bomb;
    Actor* flareDancer;

    add_calc(&this->skelAnime.playSpeed, 1.0f, 0.1f, 1.0f, 0.0f);
    if (this->skelAnime.animation == &gFlareDancerCoreInitRunCycleAnim) {
        if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame) == 0) {
            this->runRadius = search_position_distance(&this->actor.world.pos, &this->actor.parent->world.pos);
            npc_anime_ct(&this->skelAnime, anime_ct_data, ENFW_ANIM_2);
        }
        return;
    }

    if (this->damageTimer == 0 && this->explosionTimer == 0 && hitcheck(this, play)) {
        if (this->actor.parent->colChkInfo.health > 0) {
            if (!this->lastDmgHook) {
                this->actor.velocity.y = 6.0f;
            }
            Actor_SE_set(&this->actor, NA_SE_EN_FLAME_MAN_DAMAGE);
            this->damageTimer = 20;
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_FLAME_MAN_DAMAGE);
            this->explosionTimer = 6;
        }
        this->actor.speed = 0.0f;
    }

    if (this->explosionTimer != 0) {
        this->skelAnime.playSpeed = 0.0f;
        add_calc(&this->actor.scale.x, 0.024999999f, 0.08f, 0.6f, 0.0f);
        Actor_set_scale(&this->actor, this->actor.scale.x);
        if (this->actor.colorFilterTimer == 0) {
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA,
                                 this->explosionTimer);
            this->explosionTimer--;
        }

        if (this->explosionTimer == 0) {
            bomb = (EnBom*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOM, this->bompPos.x, this->bompPos.y,
                                       this->bompPos.z, 0, 0, 0x600, 0);
            if (bomb != NULL) {
                bomb->timer = 0;
            }
            flareDancer = this->actor.parent;
            flareDancer->params |= 0x4000;
            Item_Set_Std(play, NULL, &this->actor.world.pos, 0xA0);
            Actor_delete(&this->actor);
            return;
        }
    } else {
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || this->actor.velocity.y > 0.0f) {
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA,
                                 this->damageTimer);
            return;
        }
        DECR(this->damageTimer);
        if ((200.0f - this->runRadius) < 0.9f) {
            if (DECR(this->returnToParentTimer) == 0) {
                this->actor.speed = 0.0f;
                this->actionFunc = stop;
                return;
            }
        }

        // Run outwards until the radius of the run circle is 200
        add_calc(&this->runRadius, 200.0f, 0.3f, 100.0f, 0.0f);

        if (this->turnAround) {
            add_calc(&this->actor.speed, 0.0f, 0.1f, 1.0f, 0.0f);
            tmpAngle = (s16)(this->actor.world.rot.y ^ 0x8000);
            facingDir = this->actor.shape.rot.y;
            tmpAngle = add_calc(&facingDir, tmpAngle, 0.1f, 10000.0f, 0.0f);
            this->actor.shape.rot.y = facingDir;
            if (tmpAngle > 0x1554) {
                return;
            }
            this->turnAround = false;
        } else {
            Vec3f sp48;

            go_to_next(&sp48, this, this->runRadius, this->runDirection);
            add_calc_short_angle2(&this->actor.shape.rot.y, RAD_TO_BINANG(fatan2(sp48.x, sp48.z)), 4, 0xFA0, 1);
        }

        this->actor.world.rot = this->actor.shape.rot;

        if (this->slideTimer == 0 && player_search(this, play)) {
            Actor_SE_set(&this->actor, NA_SE_EN_FLAME_MAN_SURP);
            this->slideSfxTimer = 8;
            this->slideTimer = 8;
        }

        if (this->slideTimer != 0) {
            if (DECR(this->slideSfxTimer) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_FLAME_MAN_SLIDE);
                this->slideSfxTimer = 4;
            }
            add_calc(&this->actor.speed, 0.0f, 0.1f, 1.0f, 0.0f);
            this->skelAnime.playSpeed = 0.0f;
            set_dust_effect(this, 8, 0.16f, 0.2f, 3, 8.0f, 20.0f, ((fqrand() - 0.5f) * 0.2f) + 0.3f);
            this->slideTimer--;
            if (this->slideTimer == 0) {
                this->turnAround = true;
                this->runDirection = -this->runDirection;
            }
        } else {
            add_calc(&this->actor.speed, 6.0f, 0.1f, 1.0f, 0.0f);
            curFrame = this->skelAnime.curFrame;
            if (curFrame == 1 || curFrame == 4) {
                Actor_SE_set(&this->actor, NA_SE_EN_FLAME_MAN_RUN);
                set_dust_effect(this, 8, 0.16f, 0.1f, 1, 0.0f, 20.0f, 0.0f);
            }
        }
    }
}

void stop(EnFw* this, PlayState* play) {
    s16 angleToParentInit;

    angleToParentInit = search_position_angleY(&this->actor.world.pos, &this->actor.parent->home.pos);
    add_calc_short_angle2(&this->actor.shape.rot.y, angleToParentInit, 4, 0xFA0, 1);
    if (ABS(angleToParentInit - this->actor.shape.rot.y) < 0x65) {
        // angle to parent init pos is ~0.5 degrees
        this->actor.world.rot = this->actor.shape.rot;
        this->actor.velocity.y = 14.0f;
        this->actor.home.pos = this->actor.world.pos;
        Actor_SE_set(&this->actor, NA_SE_EN_STAL_JUMP);
        npc_anime_ct(&this->skelAnime, anime_ct_data, ENFW_ANIM_1);
        this->actionFunc = jump;
    }
}

static void jump(EnFw* this, PlayState* play) {
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && this->actor.velocity.y <= 0.0f) {
        this->actor.parent->params |= 0x8000;
        Actor_delete(&this->actor);
    } else {
        add_calc(&this->actor.world.pos.x, this->actor.parent->home.pos.x, 0.6f, 8.0f, 0.0f);
        add_calc(&this->actor.world.pos.z, this->actor.parent->home.pos.z, 0.6f, 8.0f, 0.0f);
    }
}

void En_Fw_Actor_move(Actor* thisx, PlayState* play) {
    EnFw* this = (EnFw*)thisx;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        Actor_position_moveF(&this->actor);
        Actor_BGcheck2(play, &this->actor, 10.0f, 20.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        this->actionFunc(this, play);
        if (this->damageTimer == 0 && this->explosionTimer == 0 && this->actionFunc == run) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnFw* this = (EnFw*)thisx;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == 2) {
        // body
        Matrix_Position(&zeroVec, &this->bompPos);
    }

    if (limbIndex == 3) {
        // head
        Matrix_Position(&zeroVec, &this->actor.focus.pos);
    }

    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->collider);
}

void En_Fw_Actor_draw(Actor* thisx, PlayState* play) {
    EnFw* this = (EnFw*)thisx;

    fw_eff_dust_mv(this);
    Matrix_push();
    fw_eff_dust_dr(this, play);
    Matrix_pull();
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);
}

void fw_eff_dust_ct(EnFw* this, Vec3f* initialPos, Vec3f* initialSpeed, Vec3f* accel, u8 initialTimer, f32 scale,
                          f32 scaleStep) {
    EnFwEffect* eff = this->effects;
    s16 i;

    for (i = 0; i < EN_FW_EFFECT_COUNT; i++, eff++) {
        if (eff->type != 1) {
            eff->scale = scale;
            eff->scaleStep = scaleStep;
            eff->initialTimer = eff->timer = initialTimer;
            eff->type = 1;
            eff->pos = *initialPos;
            eff->accel = *accel;
            eff->velocity = *initialSpeed;
            return;
        }
    }
}

void fw_eff_dust_mv(EnFw* this) {
    EnFwEffect* eff = this->effects;
    s16 i;

    for (i = 0; i < EN_FW_EFFECT_COUNT; i++, eff++) {
        if (eff->type != 0) {
            if ((--eff->timer) == 0) {
                eff->type = 0;
            }
            eff->accel.x = (fqrand() * 0.4f) - 0.2f;
            eff->accel.z = (fqrand() * 0.4f) - 0.2f;
            eff->pos.x += eff->velocity.x;
            eff->pos.y += eff->velocity.y;
            eff->pos.z += eff->velocity.z;
            eff->velocity.x += eff->accel.x;
            eff->velocity.y += eff->accel.y;
            eff->velocity.z += eff->accel.z;
            eff->scale += eff->scaleStep;
        }
    }
}

void fw_eff_dust_dr(EnFw* this, PlayState* play) {
    static void* smoke_txt[] = {
        gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex, gDust4Tex, gDust3Tex, gDust2Tex, gDust1Tex,
    };
    EnFwEffect* eff = this->effects;
    s16 materialFlag;
    s16 alpha;
    s16 i;
    s16 idx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_fw.c", 1191);

    materialFlag = false;
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < EN_FW_EFFECT_COUNT; i++, eff++) {
        if (eff->type == 0) {
            continue;
        }

        if (!materialFlag) {
            POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_0);
            gSPDisplayList(POLY_XLU_DISP++, gFlareDancerDL_7928);
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 60, 20, 0);
            materialFlag = true;
        }

        alpha = eff->timer * (255.0f / eff->initialTimer);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 130, 90, alpha);
        gDPPipeSync(POLY_XLU_DISP++);
        Matrix_translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_fw.c", 1229);
        idx = eff->timer * (8.0f / eff->initialTimer);
        gSPSegment(POLY_XLU_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(smoke_txt[idx]));
        gSPDisplayList(POLY_XLU_DISP++, gFlareDancerSquareParticleDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_fw.c", 1243);
}
