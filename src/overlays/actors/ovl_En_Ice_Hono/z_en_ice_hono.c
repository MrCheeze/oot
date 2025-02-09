/*
 * File: z_en_ice_hono.c
 * Overlay: ovl_En_Ice_Hono
 * Description: The various types of Blue Fire
 */

#include "z_en_ice_hono.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS 0

void En_Ice_Hono_actor_ct(Actor* thisx, PlayState* play);
void En_Ice_Hono_actor_dt(Actor* thisx, PlayState* play);
void En_Ice_Hono_actor_move(Actor* thisx, PlayState* play);
void En_Ice_Hono_actor_draw(Actor* thisx, PlayState* play);

void mv_stop_normal(EnIceHono* this, PlayState* play);
void mv_drop_bin(EnIceHono* this, PlayState* play);
void mv_ground_bin(EnIceHono* this, PlayState* play);
void mv_walk_eff(EnIceHono* this, PlayState* play);

void mvSet_stop_normal(EnIceHono* this);
void mvSet_drop_bin(EnIceHono* this);
void mvSet_ground_bin(EnIceHono* this);
void mvSet_walk_eff(EnIceHono* this);

ActorProfile En_Ice_Hono_Profile = {
    /**/ ACTOR_EN_ICE_HONO,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnIceHono),
    /**/ En_Ice_Hono_actor_ct,
    /**/ En_Ice_Hono_actor_dt,
    /**/ En_Ice_Hono_actor_move,
    /**/ En_Ice_Hono_actor_draw,
};

static ColliderCylinderInit ClPipeDt_normal = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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

static ColliderCylinderInit ClPipeDt_bin = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_OTHER,
        AC_NONE,
        OC1_ON | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 12, 60, 0, { 0, 0, 0 } },
};

static f32 get_distXZ_2(Vec3f* v1, Vec3f* v2) {
    return SQ(v1->x - v2->x) + SQ(v1->z - v2->z);
}

void hono_normal_ct(Actor* thisx, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_0, ICHAIN_CONTINUE),
        ICHAIN_F32(lockOnArrowOffset, 60, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
    };

    EnIceHono* this = (EnIceHono*)thisx;

    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, 0.0074f);
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_world_to_eye(&this->actor, 10.0f);

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &ClPipeDt_normal);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    mvSet_stop_normal(this);
}

void hono_bin_ct(Actor* thisx, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
    };

    EnIceHono* this = (EnIceHono*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actor.scale.x = this->actor.scale.z = this->actor.scale.y = 0.00002f;
    this->actor.gravity = -0.3f;
    this->actor.minVelocityY = -4.0f;
    this->actor.shape.yOffset = 0.0f;
    this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = this->actor.world.rot.x =
        this->actor.world.rot.y = this->actor.world.rot.z = 0;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &ClPipeDt_bin);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

    this->collider.dim.radius = this->actor.scale.x * 4000.4f;
    this->collider.dim.height = this->actor.scale.y * 8000.2f;
    this->actor.colChkInfo.mass = 253;
    mvSet_drop_bin(this);
}

void hono_eff_ct(Actor* thisx, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
    };

    EnIceHono* this = (EnIceHono*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actor.scale.x = this->actor.scale.z = this->actor.scale.y = 0.0008f;
    this->actor.gravity = -0.3f;
    this->actor.minVelocityY = -4.0f;
    this->actor.shape.yOffset = 0.0f;

    mvSet_walk_eff(this);
}

void En_Ice_Hono_actor_ct(Actor* thisx, PlayState* play) {
    EnIceHono* this = (EnIceHono*)thisx;
    s16 params = this->actor.params;

    switch (this->actor.params) {
        case -1:
            hono_normal_ct(&this->actor, play);
            break;
        case 0:
            hono_bin_ct(&this->actor, play);
            break;
        case 1:
        case 2:
            hono_eff_ct(&this->actor, play);
            break;
    }

    if ((this->actor.params == -1) || (this->actor.params == 0)) {
        Light_point_ct(&this->lightInfo, this->actor.world.pos.x, (s16)this->actor.world.pos.y + 10,
                                  this->actor.world.pos.z, 155, 210, 255, 0);
        this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
        this->unk_154 = fqrand() * (0x1FFFF / 2.0f);
        this->unk_156 = fqrand() * (0x1FFFF / 2.0f);
        PRINTF("(ice 炎)(arg_data 0x%04x)\n", this->actor.params); // "(ice flame)"
    }
}

void En_Ice_Hono_actor_dt(Actor* thisx, PlayState* play) {
    EnIceHono* this = (EnIceHono*)thisx;

    if ((this->actor.params == -1) || (this->actor.params == 0)) {
        Global_light_list_delete(play, &play->lightCtx, this->lightNode);
        ClObjPipe_dt(play, &this->collider);
    }
}

static u32 check_carryArea(EnIceHono* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.xzDistToPlayer < 60.0f) {
        Vec3f tempPos;
        tempPos.x = sin_s(this->actor.yawTowardsPlayer + 0x8000) * 40.0f + player->actor.world.pos.x;
        tempPos.y = player->actor.world.pos.y;
        tempPos.z = cos_s(this->actor.yawTowardsPlayer + 0x8000) * 40.0f + player->actor.world.pos.z;

        //! @bug: this check is superfluous: it is automatically satisfied if the coarse check is satisfied. It may have
        //! been intended to check the actor is in front of Player, but yawTowardsPlayer does not depend on Player's
        //! world rotation.
        if (get_distXZ_2(&tempPos, &this->actor.world.pos) <= SQ(40.0f)) {
            return true;
        }
    }
    return false;
}

void mvSet_stop_normal(EnIceHono* this) {
    this->actionFunc = mv_stop_normal;
    this->alpha = 255;
    this->actor.shape.yOffset = -1000.0f;
}

void mv_stop_normal(EnIceHono* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
    } else if (check_carryArea(this, play)) {
        // GI_MAX in this case allows the player to catch the actor in a bottle
        Actor_carry_request_set2(&this->actor, play, GI_MAX, 60.0f, 100.0f);
    }

    if (this->actor.xzDistToPlayer < 200.0f) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
    Actor_player_level_SE_set(&this->actor, NA_SE_EV_FIRE_PILLAR_S - SFX_FLAG);
}

void mvSet_drop_bin(EnIceHono* this) {
    this->actionFunc = mv_drop_bin;
    this->timer = 200;
    this->alpha = 255;
}

void mv_drop_bin(EnIceHono* this, PlayState* play) {
    u32 bgFlag = this->actor.bgCheckFlags & BGCHECKFLAG_GROUND;

    chase_f(&this->actor.scale.x, 0.0017f, 0.00008f);
    this->actor.scale.z = this->actor.scale.x;
    chase_f(&this->actor.scale.y, 0.0017f, 0.00008f);

    if (bgFlag != 0) {
        s32 i;
        for (i = 0; i < 8; i++) {
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ICE_HONO, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, ((s32)(fqrand() * 1000.0f) + i * 0x2000) - 0x1F4, 0, 1);
        }
        mvSet_ground_bin(this);
    }
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 10.0f, this->actor.scale.x * 3500.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    this->collider.dim.radius = this->actor.scale.x * 4000.0f;
    this->collider.dim.height = this->actor.scale.y * 8000.0f;
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

    if (this->timer <= 0) {
        Actor_delete(&this->actor);
    }
}

void mvSet_ground_bin(EnIceHono* this) {
    this->actionFunc = mv_ground_bin;
    this->timer = 60;
    this->alpha = 255;
}

void mv_ground_bin(EnIceHono* this, PlayState* play) {
    if (this->timer > 20) {
        chase_f(&this->actor.scale.x, 0.011f, 0.00014f);
        chase_f(&this->actor.scale.y, 0.006f, 0.00012f);
    } else {
        chase_f(&this->actor.scale.x, 0.0001f, 0.00015f);
        chase_f(&this->actor.scale.y, 0.0001f, 0.00015f);
    }
    this->actor.scale.z = this->actor.scale.x;
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 10.0f, this->actor.scale.x * 3500.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    if (this->timer < 25) {
        this->alpha -= 10;
        this->alpha = CLAMP(this->alpha, 0, 255);
    }

    if ((this->alpha > 100) && (this->timer < 40)) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        this->collider.dim.radius = this->actor.scale.x * 6000.0f;
        this->collider.dim.height = this->actor.scale.y * 8000.0f;
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }
    if (this->timer == 46) {
        s32 i;
        for (i = 0; i < 10; i++) {
            s32 rot = i * 0x1999;
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ICE_HONO, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, ((s32)(fqrand() * 1000.0f) + rot) - 0x1F4, 0, 2);
        }
    }

    if (this->timer <= 0) {
        Actor_delete(&this->actor);
    }
}

void mvSet_walk_eff(EnIceHono* this) {
    this->actionFunc = mv_walk_eff;
    this->timer = 44;
    this->alpha = 255;
    if (this->actor.params == 1) {
        this->smallFlameTargetYScale = (fqrand() * 0.005f) + 0.004f;
        this->actor.speed = (fqrand() * 1.6f) + 0.5f;
    } else {
        this->smallFlameTargetYScale = (fqrand() * 0.005f) + 0.003f;
        this->actor.speed = (fqrand() * 2.0f) + 0.5f;
    }
}

void mv_walk_eff(EnIceHono* this, PlayState* play) {
    if (this->timer > 20) {
        chase_f(&this->actor.scale.x, 0.006f, 0.00016f);
        chase_f(&this->actor.scale.y, this->smallFlameTargetYScale * 0.667f, 0.00014f);
    } else {
        chase_f(&this->actor.scale.x, 0.0001f, 0.00015f);
        chase_f(&this->actor.scale.y, 0.0001f, 0.00015f);
    }
    this->actor.scale.z = this->actor.scale.x;
    chase_f(&this->actor.speed, 0, 0.06f);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 10.0f, 10.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);

    if (this->timer < 25) {
        this->alpha -= 10;
        this->alpha = CLAMP(this->alpha, 0, 255);
    }
    if (this->timer <= 0) {
        Actor_delete(&this->actor);
    }
}

void En_Ice_Hono_actor_move(Actor* thisx, PlayState* play) {
    EnIceHono* this = (EnIceHono*)thisx;
    s32 pad1;
    f32 intensity;
    s32 pad2;
    f32 sin154;
    f32 sin156;

    if (this->timer > 0) {
        this->timer--;
    }
    if (this->actor.params == 0) {
        Actor_player_level_SE_set(&this->actor, NA_SE_IT_FLAME - SFX_FLAG);
    }
    if ((this->actor.params == -1) || (this->actor.params == 0)) {
        this->unk_154 += 0x1111;
        this->unk_156 += 0x4000;
        sin156 = sin_s(this->unk_156);
        sin154 = sin_s(this->unk_154);
        intensity = (fqrand() * 0.05f) + ((sin154 * 0.125f) + (sin156 * 0.1f)) + 0.425f;

#if DEBUG_FEATURES
        if ((intensity > 0.7f) || (intensity < 0.2f)) {
            PRINTF("ありえない値(ratio = %f)\n", intensity); // "impossible value(ratio = %f)"
        }
#endif

        Light_point_ct(&this->lightInfo, this->actor.world.pos.x, (s16)this->actor.world.pos.y + 10,
                                  this->actor.world.pos.z, (s32)(155.0f * intensity), (s32)(210.0f * intensity),
                                  (s32)(255.0f * intensity), 1400);
    }

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void En_Ice_Hono_actor_draw(Actor* thisx, PlayState* play) {
    EnIceHono* this = (EnIceHono*)thisx;
    u32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ice_hono.c", 695);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0,
                                (play->state.frames * -20) % 512, 32, 128));

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 170, 255, 255, this->alpha);

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 150, 255, 0);

    Matrix_rotateY(BINANG_TO_RAD((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y + 0x8000)),
                   MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_ice_hono.c", 718);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ice_hono.c", 722);
}
