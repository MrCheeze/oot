/*
 * File: z_obj_kibako2.c
 * Overlay: ovl_Obj_Kibako2
 * Description: Large crate
 */

#include "z_obj_kibako2.h"
#include "assets/objects/object_kibako2/object_kibako2.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"

#define FLAGS 0

void Obj_Kibako2_actor_ct(Actor* thisx, PlayState* play);
void Obj_Kibako2_actor_dt(Actor* thisx, PlayState* play);
void Obj_Kibako2_actor_move(Actor* thisx, PlayState* play);
void Obj_Kibako2_actor_draw(Actor* thisx, PlayState* play);
static void mv_wait(ObjKibako2* this, PlayState* play);
void mv_make(ObjKibako2* this, PlayState* play);

ActorProfile Obj_Kibako2_Profile = {
    /**/ ACTOR_OBJ_KIBAKO2,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_KIBAKO2,
    /**/ sizeof(ObjKibako2),
    /**/ Obj_Kibako2_actor_ct,
    /**/ Obj_Kibako2_actor_dt,
    /**/ Obj_Kibako2_actor_move,
    /**/ Obj_Kibako2_actor_draw,
};

static ColliderCylinderInit ClPipeDt_kibako2 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x40000040, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 31, 48, 0, { 0, 0, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

void set_collision_data_kibako2(Actor* thisx, PlayState* play) {
    ObjKibako2* this = (ObjKibako2*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->dyna.actor, &ClPipeDt_kibako2);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->collider);
}

void eff_obj_kibako2(ObjKibako2* this, PlayState* play) {
    s32 pad[2];
    Vec3f* thisPos;
    Vec3f pos;
    Vec3f velocity;
    s16 angle;
    s32 i;

    thisPos = &this->dyna.actor.world.pos;
    for (i = 0, angle = 0; i < 0x10; i++, angle += 0x4E20) {
        f32 sn = sin_s(angle);
        f32 cs = cos_s(angle);
        f32 temp_rand;
        s32 phi_s0;

        temp_rand = fqrand() * 30.0f;
        pos.x = sn * temp_rand;
        pos.y = (fqrand() * 10.0f) + 2.0f;
        pos.z = cs * temp_rand;
        velocity.x = pos.x * 0.2f;
        velocity.y = (fqrand() * 10.0f) + 2.0f;
        velocity.z = pos.z * 0.2f;
        pos.x += thisPos->x;
        pos.y += thisPos->y;
        pos.z += thisPos->z;
        temp_rand = fqrand();
        if (temp_rand < 0.05f) {
            phi_s0 = 0x60;
        } else if (temp_rand < 0.7f) {
            phi_s0 = 0x40;
        } else {
            phi_s0 = 0x20;
        }
        Effect_Kakera_ct2(play, &pos, &velocity, &pos, -200, phi_s0, 28, 2, 0, (fqrand() * 30.0f) + 5.0f, 0, 0,
                             70, KAKERA_COLOR_NONE, OBJECT_KIBAKO2, gLargeCrateFragmentDL);
    }
    dust_fly_set2(play, thisPos, 90.0f, 6, 100, 160, 1);
}

void set_item_kibako2(ObjKibako2* this, PlayState* play) {
    s16 itemDropped;
    s16 collectibleFlagTemp;

    collectibleFlagTemp = this->collectibleFlag;
    itemDropped = this->dyna.actor.home.rot.x;
    if (itemDropped >= 0 && itemDropped < ITEM00_MAX) {
        Item_set0(play, &this->dyna.actor.world.pos, itemDropped | (collectibleFlagTemp << 8));
    }
}

void Obj_Kibako2_actor_ct(Actor* thisx, PlayState* play) {
    ObjKibako2* this = (ObjKibako2*)thisx;
    s16 pad;
    CollisionHeader* colHeader = NULL;
    u32 bgId;

    MoveBG_ct(&this->dyna, 0);
    ValueSet_process(&this->dyna.actor, value_init);
    set_collision_data_kibako2(thisx, play);
    DynaPolyUty_bgdi_SG2KSG(&gLargeCrateCol, &colHeader);
    bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->dyna.bgId = bgId;
    this->collectibleFlag = this->dyna.actor.home.rot.z & 0x3F;
    this->actionFunc = mv_wait;
    this->dyna.actor.home.rot.z = this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z =
        this->dyna.actor.world.rot.x = this->dyna.actor.shape.rot.x = 0;
    // "Wooden box (stationary)"
    PRINTF("木箱(据置)(arg %04xH)(item %04xH %d)\n", this->dyna.actor.params, this->collectibleFlag,
           this->dyna.actor.home.rot.x);
}

void Obj_Kibako2_actor_dt(Actor* thisx, PlayState* play) {
    ObjKibako2* this = (ObjKibako2*)thisx;

    ClObjPipe_dt(play, &this->collider);
    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mv_wait(ObjKibako2* this, PlayState* play) {
    if ((this->collider.base.acFlags & AC_HIT) || (this->dyna.actor.home.rot.z != 0) ||
        BlastVsMyCheck_c(play, &this->dyna.actor) != NULL) {
        eff_obj_kibako2(this, play);
        Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 20, NA_SE_EV_WOODBOX_BREAK);
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        this->dyna.actor.draw = NULL;
        this->actionFunc = mv_make;
    } else if (this->dyna.actor.xzDistToPlayer < 600.0f) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void mv_make(ObjKibako2* this, PlayState* play) {
    s16 params = this->dyna.actor.params;

    if (PARAMS_GET_NOSHIFT(params, 15, 1) == 0) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_SW, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                    this->dyna.actor.world.pos.z, 0, this->dyna.actor.shape.rot.y, 0, params | 0x8000);
    }
    set_item_kibako2(this, play);
    Actor_delete(&this->dyna.actor);
}

void Obj_Kibako2_actor_move(Actor* thisx, PlayState* play) {
    ObjKibako2* this = (ObjKibako2*)thisx;

    this->actionFunc(this, play);
}

void Obj_Kibako2_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gLargeCrateDL);
}
