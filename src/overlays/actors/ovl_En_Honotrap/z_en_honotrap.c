/**
 * File: z_en_honotrap.c
 * Overlay: ovl_En_Honotrap
 * Description: Fake eye switches and Dampe flames
 */

#include "z_en_honotrap.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

#define HONOTRAP_AT_ACTIVE (1 << 0)
#define HONOTRAP_AC_ACTIVE (1 << 1)
#define HONOTRAP_OC_ACTIVE (1 << 2)

typedef enum EnHonotrapEyeState {
    /* 0 */ HONOTRAP_EYE_OPEN,
    /* 1 */ HONOTRAP_EYE_HALF,
    /* 2 */ HONOTRAP_EYE_CLOSE,
    /* 3 */ HONOTRAP_EYE_SHUT,
    /* 4 */ HONOTRAP_EYE_MAX
} EnHonotrapEyeState;

void En_Honotrap_actor_ct(Actor* thisx, PlayState* play);
void En_Honotrap_actor_dt(Actor* thisx, PlayState* play);
void En_Honotrap_actor_move(Actor* thisx, PlayState* play);
void En_Honotrap_actor_draw(Actor* thisx, PlayState* play);

void mv_spit_stop_init(EnHonotrap* this);
void mv_spit_stop(EnHonotrap* this, PlayState* play);
void mv_spit_open_init(EnHonotrap* this);
void mv_spit_open(EnHonotrap* this, PlayState* play);
void mv_spit_fire_init(EnHonotrap* this);
void mv_spit_fire(EnHonotrap* this, PlayState* play);
void mv_spit_close_init(EnHonotrap* this);
void mv_spit_close(EnHonotrap* this, PlayState* play);

void mv_hono_start_init(EnHonotrap* this);
void mv_hono_start(EnHonotrap* this, PlayState* play);
void mv_hono_wait_init(EnHonotrap* this);
void mv_hono_wait(EnHonotrap* this, PlayState* play);

void mv_hono_walk_init(EnHonotrap* this);
void mv_hono_walk(EnHonotrap* this, PlayState* play);
void mv_hono_search_init(EnHonotrap* this);
void mv_hono_search(EnHonotrap* this, PlayState* play);
void mv_hono_goOut_init(EnHonotrap* this);
void mv_hono_goOut(EnHonotrap* this, PlayState* play);

ActorProfile En_Honotrap_Profile = {
    /**/ ACTOR_EN_HONOTRAP,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_DANGEON_KEEP,
    /**/ sizeof(EnHonotrap),
    /**/ En_Honotrap_actor_ct,
    /**/ En_Honotrap_actor_dt,
    /**/ En_Honotrap_actor_move,
    /**/ En_Honotrap_actor_draw,
};

static ColliderTrisElementInit ClTrisElemDt_spit[2] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { -23.0f, 0.0f, 8.5f }, { 0.0f, -23.0f, 8.5f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { 0.0f, -23.0f, 8.5f }, { 23.0f, 0.0f, 8.5f } } },
    },
};

static ColliderTrisInit ClTrisDt_spit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    ClTrisElemDt_spit,
};

static ColliderCylinderInit ClPipeDt_hono = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x01, 0x04 },
        { 0x00100000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 10, 25, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit StatusDt_hono = { 0, 9, 23, 1 };

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

void set_collision_hono(EnHonotrap* this, PlayState* play) {
    s32 pad[3];

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider.cyl);
    CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.cyl.base);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.cyl.base);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.cyl.base);
    this->colChkFlags |= HONOTRAP_AT_ACTIVE;
    this->colChkFlags |= HONOTRAP_AC_ACTIVE;
    this->colChkFlags |= HONOTRAP_OC_ACTIVE;
}

static void get_unitVec(Vec3f* normal, Vec3f* vec) {
    f32 magnitude = Math3DVecLength(vec);

    if (magnitude < 0.001f) {
        PRINTF("Warning : vector size zero (%s %d)\n", "../z_en_honotrap.c", 328);

        normal->x = normal->y = 0.0f;
        normal->z = 1.0f;
    } else {
        normal->x = vec->x * (1.0f / magnitude);
        normal->y = vec->y * (1.0f / magnitude);
        normal->z = vec->z * (1.0f / magnitude);
    }
}

void spit_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHonotrap* this = (EnHonotrap*)thisx;
    s32 i;
    s32 j;
    Vec3f* vtx;
    Vec3f triangle[3];
    f32 cos;
    f32 sin;

    Actor_set_scale(thisx, 0.1f);
    sin = sin_s(thisx->home.rot.y);
    cos = cos_s(thisx->home.rot.y);
    ClObjTris_ct(play, &this->collider.tris);
    ClObjTris_set5_nzm(play, &this->collider.tris, thisx, &ClTrisDt_spit, this->collider.elements);

    for (i = 0; i < 2; i++) {
        for (j = 0, vtx = triangle; j < 3; j++, vtx++) {
            Vec3f* baseVtx = &ClTrisDt_spit.elements[i].dim.vtx[j];

            vtx->x = baseVtx->z * sin + baseVtx->x * cos;
            vtx->y = baseVtx->y;
            vtx->z = baseVtx->z * cos - baseVtx->x * sin;
            xyz_t_add(vtx, &thisx->world.pos, vtx);
        }
        CollisionCheck_Uty_setTrisPos(&this->collider.tris, i, &triangle[0], &triangle[1], &triangle[2]);
    }
    mv_spit_stop_init(this);
    Actor_world_to_eye(thisx, 0.0f);
}

void hono_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHonotrap* this = (EnHonotrap*)thisx;

    Actor_set_scale(&this->actor, 0.0001f);
    ClObjPipe_ct(play, &this->collider.cyl);
    ClObjPipe_set5(play, &this->collider.cyl, &this->actor, &ClPipeDt_hono);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider.cyl);
    this->actor.minVelocityY = -1.0f;
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &StatusDt_hono);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    this->actor.shape.shadowAlpha = 128;
    this->targetPos = GET_PLAYER(play)->actor.world.pos;
    this->targetPos.y += 10.0f;
    this->flameScroll = fqrand() * 511.0f;
    mv_hono_start_init(this);
    Actor_SE_set(&this->actor, NA_SE_EV_FLAME_IGNITION);
    if (this->actor.params == HONOTRAP_TYPE_FLAME_DROP) {
        this->actor.room = -1;
        this->collider.cyl.dim.radius = 12;
        this->collider.cyl.dim.height = 30;
        this->actor.shape.yOffset = -1000.0f;
    }
}

void En_Honotrap_actor_ct(Actor* thisx, PlayState* play) {
    ValueSet_process(thisx, value_init);
    if (thisx->params == HONOTRAP_TYPE_EYE) {
        spit_ct(thisx, play);
    } else {
        hono_ct(thisx, play);
    }
}

void En_Honotrap_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHonotrap* this = (EnHonotrap*)thisx;

    if (this->actor.params == HONOTRAP_TYPE_EYE) {
        ClObjTris_dt_nzf(play, &this->collider.tris);
    } else {
        ClObjPipe_dt(play, &this->collider.cyl);
    }
}

void mv_spit_stop_init(EnHonotrap* this) {
    this->actionFunc = mv_spit_stop;
    this->eyeState = HONOTRAP_EYE_SHUT;
}

void mv_spit_stop(EnHonotrap* this, PlayState* play) {
    if (this->actor.child != NULL) {
        this->timer = 200;
    } else if ((this->timer <= 0) && (this->actor.xzDistToPlayer < 750.0f)) {
        if ((this->actor.yDistToPlayer < 0.0f) && (this->actor.yDistToPlayer > -700.0f)) {
            s32 angle = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

            if ((angle > -0x4000) && (angle < 0x4000)) {
                mv_spit_open_init(this);
            }
        }
    }
}

void mv_spit_open_init(EnHonotrap* this) {
    this->actionFunc = mv_spit_open;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
    this->timer = 30;
    Actor_SE_set(&this->actor, NA_SE_EV_RED_EYE);
}

void mv_spit_open(EnHonotrap* this, PlayState* play) {
    this->eyeState--;
    if (this->eyeState <= HONOTRAP_EYE_OPEN) {
        mv_spit_fire_init(this);
        Actor_info_make_child_actor(
            &play->actorCtx, &this->actor, play, ACTOR_EN_HONOTRAP,
            (sin_s(this->actor.shape.rot.y) * 12.0f) + this->actor.home.pos.x, this->actor.home.pos.y - 10.0f,
            (cos_s(this->actor.shape.rot.y) * 12.0f) + this->actor.home.pos.z, this->actor.home.rot.x,
            this->actor.home.rot.y, this->actor.home.rot.z, HONOTRAP_TYPE_FLAME_MOVE);
    }
}

void mv_spit_fire_init(EnHonotrap* this) {
    this->actionFunc = mv_spit_fire;
    this->eyeState = HONOTRAP_EYE_OPEN;
}

void mv_spit_fire(EnHonotrap* this, PlayState* play) {
    if (this->timer <= 0) {
        mv_spit_close_init(this);
    }
}

void mv_spit_close_init(EnHonotrap* this) {
    this->actionFunc = mv_spit_close;
}

void mv_spit_close(EnHonotrap* this, PlayState* play) {
    this->eyeState++;
    if (this->eyeState >= HONOTRAP_EYE_SHUT) {
        mv_spit_stop_init(this);
        this->timer = 200;
    }
}

void mv_hono_start_init(EnHonotrap* this) {
    this->actionFunc = mv_hono_start;
}

void mv_hono_start(EnHonotrap* this, PlayState* play) {
    f32 targetScale = (this->actor.params == HONOTRAP_TYPE_FLAME_MOVE) ? 0.004f : 0.0048f;
    s32 targetReached = chase_f(&this->actor.scale.x, targetScale, 0.0006f);

    this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
    if (targetReached) {
        if (this->actor.params == HONOTRAP_TYPE_FLAME_MOVE) {
            mv_hono_walk_init(this);
        } else { // HONOTRAP_TYPE_FLAME_DROP
            mv_hono_wait_init(this);
        }
    }
}

void mv_hono_wait_init(EnHonotrap* this) {
    this->timer = 40;
    this->actor.velocity.y = 1.0f;
    this->actor.velocity.x = 2.0f * sin_s(this->actor.world.rot.y);
    this->actor.velocity.z = 2.0f * cos_s(this->actor.world.rot.y);
    this->actionFunc = mv_hono_wait;
}

void mv_hono_wait(EnHonotrap* this, PlayState* play) {
    if ((this->collider.cyl.base.atFlags & AT_HIT) || (this->timer <= 0)) {
        if ((this->collider.cyl.base.atFlags & AT_HIT) && !(this->collider.cyl.base.atFlags & AT_BOUNCED)) {
            Actor_player_power_damage_set(play, &this->actor, 5.0f, this->actor.yawTowardsPlayer, 0.0f);
        }
        this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;
        mv_hono_goOut_init(this);
        return;
    }
    if (this->actor.velocity.y > 0.0f) {
        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.z += this->actor.velocity.z;
        Actor_BGcheck2(play, &this->actor, 7.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    }
    if (!chase_f(&this->actor.world.pos.y, this->actor.floorHeight + 1.0f, this->actor.velocity.y)) {
        this->actor.velocity.y += 1.0f;
    } else {
        this->actor.velocity.y = 0.0f;
    }
    set_collision_hono(this, play);
}

void mv_hono_walk_init(EnHonotrap* this) {
    f32 distInverse;

    this->actionFunc = mv_hono_walk;
    distInverse = 1.0f / (Actor_search_position_distance(&this->actor, &this->targetPos) + 1.0f);
    this->actor.velocity.x = (this->targetPos.x - this->actor.world.pos.x) * distInverse;
    this->actor.velocity.y = (this->targetPos.y - this->actor.world.pos.y) * distInverse;
    this->actor.velocity.z = (this->targetPos.z - this->actor.world.pos.z) * distInverse;
    this->speedMod = 0.0f;
    this->timer = 160;
}

void mv_hono_walk(EnHonotrap* this, PlayState* play) {
    Actor* thisx = &this->actor;
    Vec3f speed;
    s32 targetReached;

    chase_f(&this->speedMod, 13.0f, 0.5f);
    speed.x = fabsf(this->speedMod * thisx->velocity.x);
    speed.y = fabsf(this->speedMod * thisx->velocity.y);
    speed.z = fabsf(this->speedMod * thisx->velocity.z);
    targetReached = true;
    targetReached &= chase_f(&thisx->world.pos.x, this->targetPos.x, speed.x);
    targetReached &= chase_f(&thisx->world.pos.y, this->targetPos.y, speed.y);
    targetReached &= chase_f(&thisx->world.pos.z, this->targetPos.z, speed.z);
    Actor_BGcheck2(play, &this->actor, 7.0f, 10.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);

    if (this->collider.tris.base.atFlags & AT_BOUNCED) {
        Player* player = GET_PLAYER(play);
        Vec3f shieldNorm;
        Vec3f tempVel;
        Vec3f shieldVec;

        shieldVec.x = -player->shieldMf.xz;
        shieldVec.y = -player->shieldMf.yz;
        shieldVec.z = -player->shieldMf.zz;
        get_unitVec(&shieldNorm, &shieldVec);

        tempVel = thisx->velocity;
        M3D_getRefVec(&tempVel, &shieldNorm, &thisx->velocity);
        thisx->speed = this->speedMod * 0.5f;
        thisx->world.rot.y = atans_table(thisx->velocity.z, thisx->velocity.x);
        mv_hono_goOut_init(this);
    } else if (this->collider.tris.base.atFlags & AT_HIT) {
        thisx->speed = 0.0f;
        thisx->velocity.y = 0.0f;
        mv_hono_goOut_init(this);
    } else if (this->timer <= 0) {
        mv_hono_goOut_init(this);
    } else {
        set_collision_hono(this, play);
        if (targetReached) {
            mv_hono_search_init(this);
        }
    }
}

void mv_hono_search_init(EnHonotrap* this) {
    this->actionFunc = mv_hono_search;
    this->actor.speed = 0.0f;
    this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;
    this->actor.world.rot.x = this->actor.world.rot.y = this->actor.world.rot.z = 0;
    this->timer = 100;
}

void mv_hono_search(EnHonotrap* this, PlayState* play) {
    s32 pad;

    chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x300);
    chase_f(&this->actor.speed, 3.0f, 0.1f);
    this->actor.gravity = (-this->actor.yDistToPlayer < 10.0f) ? 0.08f : -0.08f;
    Actor_position_speed_set(&this->actor);
    if (this->actor.velocity.y > 1.0f) {
        this->actor.velocity.y = 1.0f;
    }
    this->actor.velocity.y *= 0.95f;
    Actor_position_move(&this->actor);
    Actor_BGcheck2(play, &this->actor, 7.0f, 10.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    if (this->collider.cyl.base.atFlags & AT_BOUNCED) {
        Player* player = GET_PLAYER(play);
        Vec3s shieldRot;

        Matrix_to_rotate_new(&player->shieldMf, &shieldRot, false);
        this->actor.world.rot.y = ((shieldRot.y * 2) - this->actor.world.rot.y) + 0x8000;
        mv_hono_goOut_init(this);
    } else if (this->collider.cyl.base.atFlags & AT_HIT) {
        this->actor.speed *= 0.1f;
        this->actor.velocity.y *= 0.1f;
        mv_hono_goOut_init(this);
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->timer <= 0)) {
        mv_hono_goOut_init(this);
    } else {
        set_collision_hono(this, play);
    }
}

void mv_hono_goOut_init(EnHonotrap* this) {
    this->actionFunc = mv_hono_goOut;
}

void mv_hono_goOut(EnHonotrap* this, PlayState* play) {
    s32 pad;
    s32 targetReached = chase_f(&this->actor.scale.x, 0.0001f, 0.00015f);

    this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 7.0f, 10.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    if (targetReached) {
        Actor_delete(&this->actor);
    }
}

void En_Honotrap_actor_move(Actor* thisx, PlayState* play) {
    static Vec3f fire_vec = { 0.0f, 0.0f, 0.0f };
    static Vec3f bomb_acc = { 0.0f, 0.1f, 0.0f };
    EnHonotrap* this = (EnHonotrap*)thisx;

    if (this->timer > 0) {
        this->timer--;
    }
    if (this->actor.params == HONOTRAP_TYPE_EYE) {
        if ((this->actor.child != NULL) && (this->actor.child->update == NULL)) {
            this->actor.child = NULL;
        }
    } else {
        this->colChkFlags = 0;
        this->bobPhase += 0x640;
        this->actor.shape.yOffset = (sin_s(this->bobPhase) * 1000.0f) + 600.0f;
        Actor_world_to_eye(&this->actor, 5.0f);
        Actor_SE_set(&this->actor, NA_SE_EV_BURN_OUT - SFX_FLAG);
    }
    this->actionFunc(this, play);
    if (this->actor.params == HONOTRAP_TYPE_EYE) {
        if (this->collider.tris.base.acFlags & AC_HIT) {
            Effect_SS_Bomb2_2_ct(play, &this->actor.world.pos, &fire_vec, &bomb_acc, 15, 8);
            Actor_delete(&this->actor);
        } else if (this->eyeState < HONOTRAP_EYE_SHUT) {
            this->collider.tris.base.acFlags &= ~AC_HIT;
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.tris.base);
        }
    }
}

void disp_spit(Actor* thisx, PlayState* play) {
    static void* sw_txt_tbl[HONOTRAP_EYE_MAX] = {
        gEyeSwitchSilverOpenTex,
        gEyeSwitchSilverHalfTex,
        gEyeSwitchSilverClosedTex,
        gEyeSwitchSilverClosedTex,
    };
    EnHonotrap* this = (EnHonotrap*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_honotrap.c", 982);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sw_txt_tbl[this->eyeState]));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_honotrap.c", 987);
    gSPDisplayList(POLY_OPA_DISP++, gEyeSwitch2DL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_honotrap.c", 991);
}

void disp_hono(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHonotrap* this = (EnHonotrap*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_honotrap.c", 1000);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    this->flameScroll -= 20;
    this->flameScroll %= (128U << 2);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0, this->flameScroll, 32, 128));
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 200, 0, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    Matrix_rotateY(BINANG_TO_RAD((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y + 0x8000)),
                   MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_honotrap.c", 1024);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_honotrap.c", 1028);
}

void En_Honotrap_actor_draw(Actor* thisx, PlayState* play) {
    switch (thisx->params) {
        case HONOTRAP_TYPE_EYE:
            disp_spit(thisx, play);
            break;

        case HONOTRAP_TYPE_FLAME_MOVE:
        case HONOTRAP_TYPE_FLAME_DROP:
            disp_hono(thisx, play);
            break;
    }
}
