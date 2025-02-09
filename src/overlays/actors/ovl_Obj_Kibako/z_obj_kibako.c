/*
 * File: z_obj_kibako.c
 * Overlay: ovl_Obj_Kibako
 * Description: Small wooden box
 */

#include "z_obj_kibako.h"
#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_CAN_PRESS_SWITCHES)

void Obj_Kibako_actor_ct(Actor* thisx, PlayState* play);
void Obj_Kibako_actor_dt(Actor* thisx, PlayState* play2);
void Obj_Kibako_actor_move(Actor* thisx, PlayState* play);
void Obj_Kibako_actor_draw(Actor* thisx, PlayState* play);

static void mv_stop_init(ObjKibako* this);
static void mv_stop(ObjKibako* this, PlayState* play);
static void mv_carry_init(ObjKibako* this);
static void mv_carry(ObjKibako* this, PlayState* play);
static void mv_drop_init(ObjKibako* this);
static void mv_drop(ObjKibako* this, PlayState* play);

ActorProfile Obj_Kibako_Profile = {
    /**/ ACTOR_OBJ_KIBAKO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_DANGEON_KEEP,
    /**/ sizeof(ObjKibako),
    /**/ Obj_Kibako_actor_ct,
    /**/ Obj_Kibako_actor_dt,
    /**/ Obj_Kibako_actor_move,
    /**/ Obj_Kibako_actor_draw,
};

static ColliderCylinderInit ClPipeDt_kibako = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000002, 0x00, 0x01 },
        { 0x4FC00748, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 12, 27, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit StatusDt_kibako = { 0, 12, 60, MASS_HEAVY };

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 60, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

void setItem_kibako(ObjKibako* this, PlayState* play) {
    s16 collectible;

    collectible = PARAMS_GET_U(this->actor.params, 0, 5);
    if ((collectible >= 0) && (collectible < ITEM00_MAX)) {
        Item_set0(play, &this->actor.world.pos, collectible | (PARAMS_GET_U(this->actor.params, 8, 6) << 8));
    }
}

static void setSpd_gravity(ObjKibako* this) {
    this->actor.velocity.y += this->actor.gravity;
    if (this->actor.velocity.y < this->actor.minVelocityY) {
        this->actor.velocity.y = this->actor.minVelocityY;
    }
}

void set_collision_kibako(Actor* thisx, PlayState* play) {
    ObjKibako* this = (ObjKibako*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &ClPipeDt_kibako);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
}

void Obj_Kibako_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjKibako* this = (ObjKibako*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actor.gravity = -1.2f;
    this->actor.minVelocityY = -13.0f;
    set_collision_kibako(&this->actor, play);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &StatusDt_kibako);
    mv_stop_init(this);
    // "wooden box"
    PRINTF("(dungeon keep 木箱)(arg_data 0x%04x)\n", this->actor.params);
}

void Obj_Kibako_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjKibako* this = (ObjKibako*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void eff_obj_kibako(ObjKibako* this, PlayState* play) {
    s16 angle;
    s32 i;
    Vec3f* breakPos = &this->actor.world.pos;
    Vec3f pos;
    Vec3f velocity;

    for (i = 0, angle = 0; i < 12; i++, angle += 0x4E20) {
        f32 sn = sin_s(angle);
        f32 cs = cos_s(angle);
        f32 temp_rand;
        s16 phi_s0;

        pos.x = sn * 16.0f;
        pos.y = (fqrand() * 5.0f) + 2.0f;
        pos.z = cs * 16.0f;
        velocity.x = pos.x * 0.2f;
        velocity.y = (fqrand() * 6.0f) + 2.0f;
        velocity.z = pos.z * 0.2f;
        pos.x += breakPos->x;
        pos.y += breakPos->y;
        pos.z += breakPos->z;
        temp_rand = fqrand();
        if (temp_rand < 0.1f) {
            phi_s0 = 0x60;
        } else if (temp_rand < 0.7f) {
            phi_s0 = 0x40;
        } else {
            phi_s0 = 0x20;
        }
        Effect_Kakera_ct2(play, &pos, &velocity, breakPos, -200, phi_s0, 10, 10, 0, (fqrand() * 30.0f) + 10.0f,
                             0, 32, 60, KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_DANGEON_KEEP, gSmallWoodenBoxFragmentDL);
    }
    dust_fly_set2(play, &this->actor.world.pos, 40.0f, 3, 50, 140, 1);
}

void eff_obj_kibako_inWater(ObjKibako* this, PlayState* play) {
    s16 angle;
    s32 i;
    Vec3f* breakPos = &this->actor.world.pos;
    Vec3f pos;
    Vec3f velocity;

    pos = *breakPos;
    pos.y += this->actor.depthInWater;
    Effect_SS_G_Splash_sc_cl_ct(play, &pos, NULL, NULL, 0, 500);

    for (i = 0, angle = 0; i < 12; i++, angle += 0x4E20) {
        f32 sn = sin_s(angle);
        f32 cs = cos_s(angle);
        f32 temp_rand;
        s16 phi_s0;

        pos.x = sn * 16.0f;
        pos.y = (fqrand() * 5.0f) + 2.0f;
        pos.z = cs * 16.0f;
        velocity.x = pos.x * 0.18f;
        velocity.y = (fqrand() * 4.0f) + 2.0f;
        velocity.z = pos.z * 0.18f;
        pos.x += breakPos->x;
        pos.y += breakPos->y;
        pos.z += breakPos->z;
        temp_rand = fqrand();
        phi_s0 = (temp_rand < 0.2f) ? 0x40 : 0x20;
        Effect_Kakera_ct2(play, &pos, &velocity, breakPos, -180, phi_s0, 30, 30, 0, (fqrand() * 30.0f) + 10.0f,
                             0, 32, 70, KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_DANGEON_KEEP, gSmallWoodenBoxFragmentDL);
    }
}

static void mv_stop_init(ObjKibako* this) {
    this->actionFunc = mv_stop;
    this->actor.colChkInfo.mass = MASS_HEAVY;
}

static void mv_stop(ObjKibako* this, PlayState* play) {
    s32 pad;

    if (Actor_carry_check(&this->actor, play)) {
        mv_carry_init(this);
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->actor.depthInWater > 19.0f)) {
        eff_obj_kibako_inWater(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EV_WOODBOX_BREAK);
        setItem_kibako(this, play);
        Actor_delete(&this->actor);
    } else if (this->collider.base.acFlags & AC_HIT) {
        eff_obj_kibako(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EV_WOODBOX_BREAK);
        setItem_kibako(this, play);
        Actor_delete(&this->actor);
    } else {
        Actor_position_moveF(&this->actor);
        Actor_BGcheck2(play, &this->actor, 19.0f, 20.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        if (!(this->collider.base.ocFlags1 & OC1_TYPE_PLAYER) && (this->actor.xzDistToPlayer > 28.0f)) {
            this->collider.base.ocFlags1 |= OC1_TYPE_PLAYER;
        }
        if (this->actor.xzDistToPlayer < 600.0f) {
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
            if (this->actor.xzDistToPlayer < 180.0f) {
                CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
            }
        }
        if (this->actor.xzDistToPlayer < 100.0f) {
            Actor_carry_request(&this->actor, play);
        }
    }
}

static void mv_carry_init(ObjKibako* this) {
    this->actionFunc = mv_carry;
    this->actor.room = -1;
    //! @bug: This is an unsafe cast, although the sound effect will still play
    player_SE_set((Player*)&this->actor, NA_SE_PL_PULL_UP_WOODBOX);
}

static void mv_carry(ObjKibako* this, PlayState* play) {
    if (Actor_carry_end_check(&this->actor, play)) {
        this->actor.room = play->roomCtx.curRoom.num;
        if (fabsf(this->actor.speed) < 0.1f) {
            Actor_SE_set(&this->actor, NA_SE_EV_PUT_DOWN_WOODBOX);
            mv_stop_init(this);
            this->collider.base.ocFlags1 &= ~OC1_TYPE_PLAYER;
        } else {
            mv_drop_init(this);
            setSpd_gravity(this);
            Actor_position_move(&this->actor);
        }
        Actor_BGcheck2(play, &this->actor, 19.0f, 20.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    }
}

static void mv_drop_init(ObjKibako* this) {
    this->actor.velocity.x = sin_s(this->actor.world.rot.y) * this->actor.speed;
    this->actor.velocity.z = cos_s(this->actor.world.rot.y) * this->actor.speed;
    this->actor.colChkInfo.mass = 240;
    this->actionFunc = mv_drop;
}

static void mv_drop(ObjKibako* this, PlayState* play) {
    s32 pad;
    s32 pad2;

    if ((this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_WALL)) ||
        (this->collider.base.atFlags & AT_HIT)) {
        eff_obj_kibako(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EV_WOODBOX_BREAK);
        setItem_kibako(this, play);
        Actor_delete(&this->actor);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
        eff_obj_kibako_inWater(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EV_WOODBOX_BREAK);
        setItem_kibako(this, play);
        Actor_delete(&this->actor);
    } else {
        setSpd_gravity(this);
        Actor_position_move(&this->actor);
        Actor_BGcheck2(play, &this->actor, 19.0f, 20.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }
}

void Obj_Kibako_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjKibako* this = (ObjKibako*)thisx;

    this->actionFunc(this, play);
}

void Obj_Kibako_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjKibako* this = (ObjKibako*)thisx;

    Cheap_gfx_display(play, gSmallWoodenBoxDL);
}
