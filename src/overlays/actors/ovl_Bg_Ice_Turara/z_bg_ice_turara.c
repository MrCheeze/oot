/*
 * File: z_bg_ice_turara.c
 * Overlay: ovl_Bg_Ice_Turara
 * Description: Icicles
 */

#include "z_bg_ice_turara.h"
#include "assets/objects/object_ice_objects/object_ice_objects.h"

#define FLAGS 0

void Bg_Ice_Turara_actor_ct(Actor* thisx, PlayState* play);
void Bg_Ice_Turara_actor_dt(Actor* thisx, PlayState* play);
void Bg_Ice_Turara_actor_move(Actor* thisx, PlayState* play);
void Bg_Ice_Turara_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgIceTurara* this, PlayState* play);
void mode_drop_wait(BgIceTurara* this, PlayState* play);
static void mode_yure(BgIceTurara* this, PlayState* play);
static void mode_drop(BgIceTurara* this, PlayState* play);
static void mode_restart(BgIceTurara* this, PlayState* play);

static ColliderCylinderInit IceObjectsAcPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0x4FC007CA, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 13, 120, 0, { 0, 0, 0 } },
};

ActorProfile Bg_Ice_Turara_Profile = {
    /**/ ACTOR_BG_ICE_TURARA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_ICE_OBJECTS,
    /**/ sizeof(BgIceTurara),
    /**/ Bg_Ice_Turara_actor_ct,
    /**/ Bg_Ice_Turara_actor_dt,
    /**/ Bg_Ice_Turara_actor_move,
    /**/ Bg_Ice_Turara_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 600, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -3, ICHAIN_CONTINUE),
    ICHAIN_F32(minVelocityY, -30, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Ice_Turara_actor_ct(Actor* thisx, PlayState* play) {
    BgIceTurara* this = (BgIceTurara*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&object_ice_objects_Col_002594, &colHeader);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->dyna.actor, &IceObjectsAcPipeData);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->collider);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if (this->dyna.actor.params == TURARA_STALAGMITE) {
        this->actionFunc = mode_wait;
    } else {
        this->dyna.actor.shape.rot.x = -0x8000;
        this->dyna.actor.shape.yOffset = 1200.0f;
        this->actionFunc = mode_drop_wait;
    }
}

void Bg_Ice_Turara_actor_dt(Actor* thisx, PlayState* play) {
    BgIceTurara* this = (BgIceTurara*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjPipe_dt(play, &this->collider);
}

void ice_turara_set_ice_piece(BgIceTurara* this, PlayState* play, f32 arg2) {
    static Vec3f acc = { 0.0f, -1.0f, 0.0f };
    static Color_RGBA8 ice_prim = { 170, 255, 255, 255 };
    static Color_RGBA8 ice_env = { 0, 50, 100, 255 };
    Vec3f vel;
    Vec3f pos;
    s32 j;
    s32 i;

    Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 30, NA_SE_EV_ICE_BROKEN);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 10; j++) {
            pos.x = this->dyna.actor.world.pos.x + rnd_fx(8.0f);
            pos.y = this->dyna.actor.world.pos.y + (fqrand() * arg2) + (i * arg2);
            pos.z = this->dyna.actor.world.pos.z + rnd_fx(8.0f);

            vel.x = rnd_fx(7.0f);
            vel.z = rnd_fx(7.0f);
            vel.y = (fqrand() * 4.0f) + 8.0f;

            Effect_SS_Ice_Piece2_ct(play, &pos, (fqrand() * 0.2f) + 0.1f, &vel, &acc, &ice_prim, &ice_env, 30);
        }
    }
}

static void mode_wait(BgIceTurara* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        ice_turara_set_ice_piece(this, play, 50.0f);
        Actor_delete(&this->dyna.actor);
        return;
    }
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
}

void mode_drop_wait(BgIceTurara* this, PlayState* play) {
    if (this->dyna.actor.xzDistToPlayer < 60.0f) {
        this->shiverTimer = 10;
        this->actionFunc = mode_yure;
    }
}

static void mode_yure(BgIceTurara* this, PlayState* play) {
    s16 phi_v0_3;
    s16 phi_v0_2;
    f32 sp28;

    if (this->shiverTimer != 0) {
        this->shiverTimer--;
    }
    if (!(this->shiverTimer % 4)) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_ICE_SWING);
    }
    if (this->shiverTimer == 0) {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->collider);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        this->actionFunc = mode_drop;
    } else {
        sp28 = fqrand();
        phi_v0_2 = (fqrand() < 0.5f ? -1 : 1);
        this->dyna.actor.world.pos.x = (phi_v0_2 * ((0.5f * sp28) + 0.5f)) + this->dyna.actor.home.pos.x;
        sp28 = fqrand();
        phi_v0_3 = (fqrand() < 0.5f ? -1 : 1);
        this->dyna.actor.world.pos.z = (phi_v0_3 * ((0.5f * sp28) + 0.5f)) + this->dyna.actor.home.pos.z;
    }
}

static void mode_drop(BgIceTurara* this, PlayState* play) {
    if ((this->collider.base.atFlags & AT_HIT) || (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->collider.base.atFlags &= ~AT_HIT;
        this->dyna.actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        if (this->dyna.actor.world.pos.y < this->dyna.actor.floorHeight) {
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
        }
        ice_turara_set_ice_piece(this, play, 40.0f);
        if (this->dyna.actor.params == TURARA_STALACTITE_REGROW) {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 120.0f;
            DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
            this->actionFunc = mode_restart;
        } else {
            Actor_delete(&this->dyna.actor);
            return;
        }
    } else {
        Actor_position_moveF(&this->dyna.actor);
        this->dyna.actor.world.pos.y += 40.0f;
        Actor_BGcheck2(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
        this->dyna.actor.world.pos.y -= 40.0f;
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->collider);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }
}

static void mode_restart(BgIceTurara* this, PlayState* play) {
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 1.0f)) {
        this->actionFunc = mode_drop_wait;
        this->dyna.actor.velocity.y = 0.0f;
    }
}

void Bg_Ice_Turara_actor_move(Actor* thisx, PlayState* play) {
    BgIceTurara* this = (BgIceTurara*)thisx;

    this->actionFunc(this, play);
}

void Bg_Ice_Turara_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, object_ice_objects_DL_0023D0);
}
