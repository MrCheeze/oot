/*
 * File: z_bg_hidan_hrock.c
 * Overlay: ovl_Bg_Hidan_Hrock
 * Description: Huge stone spike platform (Fire Temple)
 */

#include "z_bg_hidan_hrock.h"

#include "ichain.h"
#include "rumble.h"
#include "sfx.h"
#include "z_lib.h"
#include "z64play.h"

#include "assets/objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void Bg_Hidan_Hrock_actor_ct(Actor* thisx, PlayState* play);
void Bg_Hidan_Hrock_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Hrock_actor_move(Actor* thisx, PlayState* play);
void Bg_Hidan_Hrock_actor_draw(Actor* thisx, PlayState* play);

static void mode_drop(BgHidanHrock* this, PlayState* play);
void mode_damage_wait(BgHidanHrock* this, PlayState* play);
static void mode_wait(BgHidanHrock* this, PlayState* play);

ActorProfile Bg_Hidan_Hrock_Profile = {
    /**/ ACTOR_BG_HIDAN_HROCK,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HIDAN_OBJECTS,
    /**/ sizeof(BgHidanHrock),
    /**/ Bg_Hidan_Hrock_actor_ct,
    /**/ Bg_Hidan_Hrock_actor_dt,
    /**/ Bg_Hidan_Hrock_actor_move,
    /**/ Bg_Hidan_Hrock_actor_draw,
};

static ColliderTrisElementInit HidanHrockAcTrisElemData[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_NO_AT_INFO | ACELEM_NO_DAMAGE | ACELEM_NO_SWORD_SFX | ACELEM_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { -40.0f, 3.0f, -40.0f }, { -40.0f, 3.0f, 40.0f }, { 40.0f, 3.0f, 40.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_NO_AT_INFO | ACELEM_NO_DAMAGE | ACELEM_NO_SWORD_SFX | ACELEM_NO_HITMARK,
            OCELEM_NONE,
        },
        { { { 40.0f, 3.0f, 40.0f }, { 40.0f, 3.0f, -40.0f }, { -40.0f, 3.0f, -40.0f } } },
    },
};

static ColliderTrisInit HidanHrockAcTrisData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    HidanHrockAcTrisElemData,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_STOP),
};

void Bg_Hidan_Hrock_actor_ct(Actor* thisx, PlayState* play) {
    BgHidanHrock* this = (BgHidanHrock*)thisx;
    ColliderTrisElementInit* colliderElementInit;
    Vec3f vertices[3];
    f32 cosRotY;
    f32 sinRotY;
    s32 i;
    s32 j;
    CollisionHeader* collisionHeader = NULL;
    Vec3f* vtx;

    ValueSet_process(thisx, value_init);
    this->unk_16A = PARAMS_GET_U(thisx->params, 0, 6);
    thisx->params = PARAMS_GET_U(thisx->params, 8, 8);
    ClObjTris_ct(play, &this->collider);
    ClObjTris_set5_nzm(play, &this->collider, thisx, &HidanHrockAcTrisData, this->colliderItems);
    MoveBG_ct(&this->dyna, 0);

    sinRotY = sin_s(thisx->shape.rot.y);
    cosRotY = cos_s(thisx->shape.rot.y);

    if (thisx->params == 0) {
        sinRotY *= 1.5f;
        cosRotY *= 1.5f;
    }

    for (i = 0; i < 2; i++) {
        colliderElementInit = &HidanHrockAcTrisData.elements[i];

        if (1) {
            for (j = 0; j < 3; j++) {
                vtx = &colliderElementInit->dim.vtx[j];

                vertices[j].x = vtx->z * sinRotY + (thisx->home.pos.x + vtx->x * cosRotY);
                vertices[j].y = vtx->y + thisx->home.pos.y;
                vertices[j].z = vtx->z * cosRotY + (thisx->home.pos.z - vtx->x * sinRotY);
            }
        }
        CollisionCheck_Uty_setTrisPos(&this->collider, i, &vertices[0], &vertices[1], &vertices[2]);
    }

    if (Actor_Environment_sw_Check(play, this->unk_16A)) {
        this->actionFunc = mode_wait;
        if (thisx->params == 0) {
            thisx->world.pos.y -= 2800.0f;
            thisx->cullingVolumeDistance = 3000.0f;
        } else if (thisx->params == 1) {
            thisx->world.pos.y -= 800.0f;
        } else if (thisx->params == 2) {
            thisx->world.pos.y -= 240.0f;
        }
    } else {
        if (thisx->params == 0) {
            thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
            thisx->cullingVolumeDistance = 3000.0f;
        }
        this->actionFunc = mode_damage_wait;
    }

    if (thisx->params == 0) {
        DynaPolyUty_bgdi_SG2KSG(&gFireTempleTallestPillarAboveRoomBeforeBossCol, &collisionHeader);
    } else {
        DynaPolyUty_bgdi_SG2KSG(&gFireTemplePillarInsertedInGroundCol, &collisionHeader);
    }

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, collisionHeader);
}

void Bg_Hidan_Hrock_actor_dt(Actor* thisx, PlayState* play) {
    BgHidanHrock* this = (BgHidanHrock*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjTris_dt_nzf(play, &this->collider);
}

static void mode_wait(BgHidanHrock* this, PlayState* play) {
}

static void mode_yure(BgHidanHrock* this, PlayState* play) {
    if (this->unk_168 != 0) {
        this->unk_168--;
    }

    this->dyna.actor.world.pos.x =
        (sin_s(this->dyna.actor.world.rot.y + (this->unk_168 << 0xE)) * 5.0f) + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z =
        (cos_s(this->dyna.actor.world.rot.y + (this->unk_168 << 0xE)) * 5.0f) + this->dyna.actor.home.pos.z;

    if (!(this->unk_168 % 4)) {
        z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 180, 10, 100);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
    }

    if (this->unk_168 == 0) {
        if (this->dyna.actor.params == 0) {
            this->dyna.actor.home.pos.y -= 2800.0f;
        } else if (this->dyna.actor.params == 1) {
            this->dyna.actor.home.pos.y -= 800.0f;
        } else {
            this->dyna.actor.home.pos.y -= 240.0f;
        }

        this->actionFunc = mode_drop;
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;
    }
}

static void mode_drop(BgHidanHrock* this, PlayState* play) {
    this->dyna.actor.velocity.y++;

    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, this->dyna.actor.velocity.y)) {
        this->dyna.actor.flags &= ~(ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);

        if (this->dyna.actor.params == 0) {
            if (play->roomCtx.curRoom.num == 10) {
                this->dyna.actor.room = 10;
            } else {
                Actor_delete(&this->dyna.actor);
            }
        }

        this->actionFunc = mode_wait;
    }
}

void mode_damage_wait(BgHidanHrock* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        this->actionFunc = mode_yure;
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;

        if (this->dyna.actor.params == 0) {
            this->dyna.actor.room = -1;
        }

        this->unk_168 = 20;
        Actor_Environment_sw_On(play, this->unk_16A);
    } else {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 5.0f, 1.0f);
    } else {
        chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 1.0f);
    }
}

void Bg_Hidan_Hrock_actor_move(Actor* thisx, PlayState* play) {
    BgHidanHrock* this = (BgHidanHrock*)thisx;

    this->actionFunc(this, play);
}

void Bg_Hidan_Hrock_actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* display_model[] = {
        gFireTempleTallestPillarAboveRoomBeforeBossDL,
        gFireTemplePillarInsertedInGroundDL,
        gFireTemplePillarInsertedInGroundDL,
    };

    Cheap_gfx_display(play, display_model[thisx->params]);
}
