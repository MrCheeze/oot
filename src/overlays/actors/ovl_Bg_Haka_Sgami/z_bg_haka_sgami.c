/*
 * File: z_bg_haka_sgami.c
 * Overlay: ovl_Bg_Haka_Sgami
 * Description: Spinning Scythe Trap
 */

#include "z_bg_haka_sgami.h"

#include "ichain.h"
#include "sfx.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_haka_objects/object_haka_objects.h"
#include "assets/objects/object_ice_objects/object_ice_objects.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

typedef enum SpinningScytheTrapMode {
    /* 0 */ SCYTHE_TRAP_SHADOW_TEMPLE,
    /* 1 */ SCYTHE_TRAP_SHADOW_TEMPLE_INVISIBLE,
    /* 2 */ SCYTHE_TRAP_ICE_CAVERN
} SpinningScytheTrapMode;

#define SCYTHE_SPIN_TIME 32

void Bg_Haka_Sgami_actor_ct(Actor* thisx, PlayState* play);
void Bg_Haka_Sgami_actor_dt(Actor* thisx, PlayState* play);
void Bg_Haka_Sgami_actor_move(Actor* thisx, PlayState* play);
void Bg_Haka_Sgami_actor_draw(Actor* thisx, PlayState* play);

static void mode_dma_wait(BgHakaSgami* this, PlayState* play);
static void mode_rotate(BgHakaSgami* this, PlayState* play);

ActorProfile Bg_Haka_Sgami_Profile = {
    /**/ ACTOR_BG_HAKA_SGAMI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(BgHakaSgami),
    /**/ Bg_Haka_Sgami_actor_ct,
    /**/ Bg_Haka_Sgami_actor_dt,
    /**/ Bg_Haka_Sgami_actor_move,
    /**/ NULL,
};

static ColliderTrisElementInit HakaSgamiAtTrisElemData[4] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { { { 365.0f, 45.0f, 27.0f }, { 130.0f, 45.0f, 150.0f }, { 290.0f, 45.0f, 145.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { { { 250.0f, 45.0f, 90.0f }, { 50.0f, 45.0f, 80.0f }, { 160.0f, 45.0f, 160.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { { { -305.0f, 33.0f, -7.0f }, { -220.0f, 33.0f, 40.0f }, { -130.0f, 33.0f, -5.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { { { -190.0f, 33.0f, 40.0f }, { -30.0f, 33.0f, 15.0f }, { -70.0f, 33.0f, -30.0f } } },
    },
};

static ColliderTrisInit HakaSgamiAtTrisData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    4,
    HakaSgamiAtTrisElemData,
};

static ColliderCylinderInit HakaSgamiOcPipeData = {
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
    { 80, 130, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit HakaSgamiStatusData = { 0, 80, 130, MASS_IMMOVABLE };

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_4, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Haka_Sgami_actor_ct(Actor* thisx, PlayState* play) {
    static u8 start_top[] = { 250, 250, 250, 200 };
    static u8 start_root[] = { 200, 200, 200, 130 };
    static u8 end_top[] = { 200, 200, 200, 60 };
    static u8 end_root[] = { 150, 150, 150, 20 };
    BgHakaSgami* this = (BgHakaSgami*)thisx;
    EffectBlureInit1 blureInit;
    s32 i;
    ColliderTris* colliderScythe = &this->colliderScythe;

    ValueSet_process(thisx, value_init);

    this->unk_151 = PARAMS_GET_U(thisx->params, 0, 8);
    thisx->params = PARAMS_GET_U(thisx->params, 8, 8);

    if (this->unk_151 != 0) {
        thisx->flags |= ACTOR_FLAG_REACT_TO_LENS;
    }

    ClObjTris_ct(play, colliderScythe);
    ClObjTris_set5_nzm(play, colliderScythe, thisx, &HakaSgamiAtTrisData, this->colliderScytheItems);
    ClObjPipe_ct(play, &this->colliderScytheCenter);
    ClObjPipe_set5(play, &this->colliderScytheCenter, thisx, &HakaSgamiOcPipeData);

    this->colliderScytheCenter.dim.pos.x = thisx->world.pos.x;
    this->colliderScytheCenter.dim.pos.y = thisx->world.pos.y;
    this->colliderScytheCenter.dim.pos.z = thisx->world.pos.z;

    CollisionCheck_Status_set2(&thisx->colChkInfo, NULL, &HakaSgamiStatusData);

    for (i = 0; i < 4; i++) {
        blureInit.p1StartColor[i] = start_top[i];
        blureInit.p2StartColor[i] = start_root[i];
        blureInit.p1EndColor[i] = end_top[i];
        blureInit.p2EndColor[i] = end_root[i];
    }
    blureInit.elemDuration = 10;
    blureInit.unkFlag = false;
    blureInit.calcMode = 2;
    EffectAdd(play, &this->blureEffectIndex[0], EFFECT_BLURE1, 0, 0, &blureInit);
    EffectAdd(play, &this->blureEffectIndex[1], EFFECT_BLURE1, 0, 0, &blureInit);

    if (thisx->params == SCYTHE_TRAP_SHADOW_TEMPLE) {
        this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_HAKA_OBJECTS);
        thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_ICE_OBJECTS);
        this->colliderScytheCenter.dim.radius = 30;
        this->colliderScytheCenter.dim.height = 70;
        Actor_world_to_eye(thisx, 40.0f);
    }

    if (this->requiredObjectSlot < 0) {
        Actor_delete(thisx);
        return;
    }

    this->actionFunc = mode_dma_wait;
}

void Bg_Haka_Sgami_actor_dt(Actor* thisx, PlayState* play) {
    BgHakaSgami* this = (BgHakaSgami*)thisx;

    EffectFreeIndex(play, this->blureEffectIndex[0]);
    EffectFreeIndex(play, this->blureEffectIndex[1]);
    ClObjTris_dt_nzf(play, &this->colliderScythe);
    ClObjPipe_dt(play, &this->colliderScytheCenter);
}

static void mode_dma_wait(BgHakaSgami* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->actor.objectSlot = this->requiredObjectSlot;
        this->actor.draw = Bg_Haka_Sgami_actor_draw;
        this->timer = SCYTHE_SPIN_TIME;
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actionFunc = mode_rotate;
    }
}

static void mode_rotate(BgHakaSgami* this, PlayState* play) {
    static Vec3f blure_root[] = {
        { -20.0f, 50.0f, 130.0f },
        { -50.0f, 33.0f, 20.0f },
    };
    static Vec3f blure_top[] = {
        { 380.0f, 50.0f, 50.0f },
        { 310.0f, 33.0f, 0.0f },
    };
    s32 i;
    s32 j;
    Vec3f scytheVertices[3];
    f32 actorRotYSin;
    f32 actorRotYCos;
    s32 iterateCount;
    ColliderTrisElementInit* elementInit;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.shape.rot.y += ((s16)(512.0f * sinf(this->timer * (M_PI / 16.0f))) + 0x400) >> 1;

    if (this->timer == 0) {
        this->timer = SCYTHE_SPIN_TIME;
    }

    actorRotYSin = sin_s(this->actor.shape.rot.y);
    actorRotYCos = cos_s(this->actor.shape.rot.y);

    iterateCount = (this->actor.params != 0) ? 4 : 2;

    for (i = iterateCount - 2; i < iterateCount; i++) {
        elementInit = &HakaSgamiAtTrisData.elements[i];

        for (j = 0; j < 3; j++) {
            scytheVertices[j].x = this->actor.world.pos.x + elementInit->dim.vtx[j].z * actorRotYSin +
                                  elementInit->dim.vtx[j].x * actorRotYCos;
            scytheVertices[j].y = this->actor.world.pos.y + elementInit->dim.vtx[j].y;
            scytheVertices[j].z = this->actor.world.pos.z + elementInit->dim.vtx[j].z * actorRotYCos -
                                  elementInit->dim.vtx[j].x * actorRotYSin;
        }

        CollisionCheck_Uty_setTrisPos(&this->colliderScythe, i, &scytheVertices[0], &scytheVertices[1], &scytheVertices[2]);

        for (j = 0; j < 3; j++) {
            scytheVertices[j].x = (2 * this->actor.world.pos.x) - scytheVertices[j].x;
            scytheVertices[j].z = (2 * this->actor.world.pos.z) - scytheVertices[j].z;
        }

        CollisionCheck_Uty_setTrisPos(&this->colliderScythe, (i + 2) % 4, &scytheVertices[0], &scytheVertices[1],
                                 &scytheVertices[2]);
    }

    if ((this->unk_151 == 0) || play->actorCtx.lensActive) {
        scytheVertices[0].x = this->actor.world.pos.x + blure_top[this->actor.params].z * actorRotYSin +
                              blure_top[this->actor.params].x * actorRotYCos;
        scytheVertices[0].y = this->actor.world.pos.y + blure_top[this->actor.params].y;
        scytheVertices[0].z = this->actor.world.pos.z + blure_top[this->actor.params].z * actorRotYCos -
                              blure_top[this->actor.params].x * actorRotYSin;
        scytheVertices[1].x = this->actor.world.pos.x + blure_root[this->actor.params].z * actorRotYSin +
                              blure_root[this->actor.params].x * actorRotYCos;
        scytheVertices[1].y = this->actor.world.pos.y + blure_root[this->actor.params].y;
        scytheVertices[1].z = this->actor.world.pos.z + blure_root[this->actor.params].z * actorRotYCos -
                              blure_root[this->actor.params].x * actorRotYSin;
        EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->blureEffectIndex[0]), &scytheVertices[0], &scytheVertices[1]);

        for (j = 0; j < 2; j++) {
            scytheVertices[j].x = (2 * this->actor.world.pos.x) - scytheVertices[j].x;
            scytheVertices[j].z = (2 * this->actor.world.pos.z) - scytheVertices[j].z;
        }

        EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->blureEffectIndex[1]), &scytheVertices[0], &scytheVertices[1]);
    }

    CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderScythe.base);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderScytheCenter.base);
    Actor_level_SE_set(&this->actor, NA_SE_EV_ROLLCUTTER_MOTOR - SFX_FLAG);
}

void Bg_Haka_Sgami_actor_move(Actor* thisx, PlayState* play) {
    BgHakaSgami* this = (BgHakaSgami*)thisx;
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & (PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_28 | PLAYER_STATE1_29)) ||
        (this->actionFunc == mode_dma_wait)) {
        this->actionFunc(this, play);
    }
}

void Bg_Haka_Sgami_actor_draw(Actor* thisx, PlayState* play) {
    BgHakaSgami* this = (BgHakaSgami*)thisx;

    if (this->unk_151 != 0) {
        Cheap_gfx_display_xlu(play, object_haka_objects_DL_00BF20);
    } else if (this->actor.params == SCYTHE_TRAP_SHADOW_TEMPLE) {
        Cheap_gfx_display(play, object_haka_objects_DL_00BF20);
    } else {
        Cheap_gfx_display(play, object_ice_objects_DL_0021F0);
    }
}
