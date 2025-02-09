/*
 * File: z_bg_hidan_sima.c
 * Overlay: ovl_Bg_Hidan_Sima
 * Description: Stone platform (Fire Temple)
 */

#include "z_bg_hidan_sima.h"
#include "assets/objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void Bg_Hidan_Sima_actor_ct(Actor* thisx, PlayState* play);
void Bg_Hidan_Sima_actor_dt(Actor* thisx, PlayState* play);
void Bg_Hidan_Sima_actor_move(Actor* thisx, PlayState* play);
void Bg_Hidan_Sima_actor_draw(Actor* thisx, PlayState* play);

void mode_sizu_wait(BgHidanSima* this, PlayState* play);
static void mode_yure(BgHidanSima* this, PlayState* play);
static void mode_sizumu(BgHidanSima* this, PlayState* play);
void mode_uki_wait(BgHidanSima* this, PlayState* play);
static void mode_move(BgHidanSima* this, PlayState* play);
void set_sima_uki_at(BgHidanSima* this);

ActorProfile Bg_Hidan_Sima_Profile = {
    /**/ ACTOR_BG_HIDAN_SIMA,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HIDAN_OBJECTS,
    /**/ sizeof(BgHidanSima),
    /**/ Bg_Hidan_Sima_actor_ct,
    /**/ Bg_Hidan_Sima_actor_dt,
    /**/ Bg_Hidan_Sima_actor_move,
    /**/ Bg_Hidan_Sima_actor_draw,
};

static ColliderJntSphElementInit HidanSimaUkiAtJntSphElemData[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 1, { { 0, 40, 100 }, 22 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 1, { { 0, 40, 145 }, 30 }, 100 },
    },
};

static ColliderJntSphInit HidanSimaUkiAtJntSphData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(HidanSimaUkiAtJntSphElemData),
    HidanSimaUkiAtJntSphElemData,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static void* fire_txt[] = {
    gFireTempleFireball0Tex, gFireTempleFireball1Tex, gFireTempleFireball2Tex, gFireTempleFireball3Tex,
    gFireTempleFireball4Tex, gFireTempleFireball5Tex, gFireTempleFireball6Tex, gFireTempleFireball7Tex,
};

void Bg_Hidan_Sima_actor_ct(Actor* thisx, PlayState* play) {
    BgHidanSima* this = (BgHidanSima*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 i;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    if (this->dyna.actor.params == 0) {
        DynaPolyUty_bgdi_SG2KSG(&gFireTempleStonePlatform1Col, &colHeader);
    } else {
        DynaPolyUty_bgdi_SG2KSG(&gFireTempleStonePlatform2Col, &colHeader);
    }
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->dyna.actor, &HidanSimaUkiAtJntSphData, this->elements);
    for (i = 0; i < ARRAY_COUNT(HidanSimaUkiAtJntSphElemData); i++) {
        this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius;
    }
    if (this->dyna.actor.params == 0) {
        this->actionFunc = mode_sizu_wait;
    } else {
        this->actionFunc = mode_uki_wait;
    }
}

void Bg_Hidan_Sima_actor_dt(Actor* thisx, PlayState* play) {
    BgHidanSima* this = (BgHidanSima*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjJntSph_dt_nzf(play, &this->collider);
}

void mode_sizu_wait(BgHidanSima* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 3.4f);
    if (MoveBG_checkRidePlayerStatus(&this->dyna) && !(player->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14))) {
        this->timer = 20;
        this->dyna.actor.world.rot.y = getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4000;
        if (this->dyna.actor.home.pos.y <= this->dyna.actor.world.pos.y) {
            this->actionFunc = mode_yure;
        } else {
            this->actionFunc = mode_sizumu;
        }
    }
}

static void mode_yure(BgHidanSima* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer != 0) {
        this->dyna.actor.world.pos.x =
            sin_s(this->dyna.actor.world.rot.y + (this->timer * 0x4000)) * 5.0f + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z =
            cos_s(this->dyna.actor.world.rot.y + (this->timer * 0x4000)) * 5.0f + this->dyna.actor.home.pos.z;
    } else {
        this->actionFunc = mode_sizumu;
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;
    }
    if (!(this->timer % 4)) {
        z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 180, 10, 100);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
    }
}

static void mode_sizumu(BgHidanSima* this, PlayState* play) {
    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        this->timer = 20;
    } else if (this->timer != 0) {
        this->timer--;
    }
    chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 100.0f, 1.7f);
    if (this->timer == 0) {
        this->actionFunc = mode_sizu_wait;
    }
}

void mode_uki_wait(BgHidanSima* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        this->dyna.actor.world.rot.y += 0x8000;
        this->timer = 60;
        this->actionFunc = mode_move;
    }
}

static void mode_move(BgHidanSima* this, PlayState* play) {
    f32 temp;

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->dyna.actor.world.rot.y != this->dyna.actor.home.rot.y) {
        temp = (sinf(((60 - this->timer) * 0.01667 - 0.5) * M_PI) + 1) * 200;
    } else {
        temp = (sinf((this->timer * 0.01667 - 0.5) * M_PI) + 1) * -200;
    }
    this->dyna.actor.world.pos.x = sin_s(this->dyna.actor.world.rot.y) * temp + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = cos_s(this->dyna.actor.world.rot.y) * temp + this->dyna.actor.home.pos.z;
    if (this->timer == 0) {
        this->timer = 20;
        this->actionFunc = mode_uki_wait;
    }
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_FIRE_PILLAR - SFX_FLAG);
}

void set_sima_uki_at(BgHidanSima* this) {
    ColliderJntSphElement* elem;
    s32 i;
    f32 cos = cos_s(this->dyna.actor.world.rot.y + 0x8000);
    f32 sin = sin_s(this->dyna.actor.world.rot.y + 0x8000);

    for (i = 0; i < 2; i++) {
        elem = &this->collider.elements[i];
        elem->dim.worldSphere.center.x = this->dyna.actor.world.pos.x + sin * elem->dim.modelSphere.center.z;
        elem->dim.worldSphere.center.y = (s16)this->dyna.actor.world.pos.y + elem->dim.modelSphere.center.y;
        elem->dim.worldSphere.center.z = this->dyna.actor.world.pos.z + cos * elem->dim.modelSphere.center.z;
    }
}

void Bg_Hidan_Sima_actor_move(Actor* thisx, PlayState* play) {
    BgHidanSima* this = (BgHidanSima*)thisx;
    s32 pad;

    this->actionFunc(this, play);
    if (this->dyna.actor.params != 0) {
        s32 temp = (this->dyna.actor.world.rot.y == this->dyna.actor.shape.rot.y) ? this->timer : (this->timer + 80);

        if (this->actionFunc == mode_move) {
            temp += 20;
        }
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - ((1.0f - cosf(temp * (M_PI / 20))) * 5.0f);
        if (this->actionFunc == mode_move) {
            set_sima_uki_at(this);
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

static Gfx* draw_fire(PlayState* play, BgHidanSima* this, Gfx* gfx) {
    MtxF mtxF;
    s32 s3;
    s32 v0;
    s32 phi_s5;
    f32 cos;
    f32 sin;
    s32 pad[2];

    Matrix_copy_MtxF(&mtxF, &MtxF_clear);
    cos = cos_s(this->dyna.actor.world.rot.y + 0x8000);
    sin = sin_s(this->dyna.actor.world.rot.y + 0x8000);

    phi_s5 = (60 - this->timer) >> 1;
    phi_s5 = CLAMP_MAX(phi_s5, 3);

    v0 = 3 - (this->timer >> 1);
    v0 = CLAMP_MIN(v0, 0);

    mtxF.xw = this->dyna.actor.world.pos.x + ((79 - ((this->timer % 6) * 4)) + v0 * 25) * sin;
    mtxF.zw = this->dyna.actor.world.pos.z + ((79 - ((this->timer % 6) * 4)) + v0 * 25) * cos;
    mtxF.yw = this->dyna.actor.world.pos.y + 40.0f;
    mtxF.zz = v0 * 0.4f + 1.0f;
    mtxF.yy = v0 * 0.4f + 1.0f;
    mtxF.xx = v0 * 0.4f + 1.0f;

    for (s3 = v0; s3 < phi_s5; s3++) {
        mtxF.xw += 25.0f * sin;
        mtxF.zw += 25.0f * cos;
        mtxF.xx += 0.4f;
        mtxF.yy += 0.4f;
        mtxF.zz += 0.4f;

        gSPSegment(gfx++, 0x09, SEGMENTED_TO_VIRTUAL(fire_txt[(this->timer + s3) % 7]));
        gSPMatrix(gfx++,
                  _MtxF_to_Mtx(MATRIX_CHECK_FLOATS(&mtxF, "../z_bg_hidan_sima.c", 611),
                                   GRAPH_ALLOC(play->state.gfxCtx, sizeof(Mtx))),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(gfx++, gFireTempleFireballDL);
    }
    mtxF.xw = this->dyna.actor.world.pos.x + (phi_s5 * 25 + 80) * sin;
    mtxF.zw = this->dyna.actor.world.pos.z + (phi_s5 * 25 + 80) * cos;
    gSPSegment(gfx++, 0x09, SEGMENTED_TO_VIRTUAL(fire_txt[(this->timer + s3) % 7]));
    gSPMatrix(gfx++,
              _MtxF_to_Mtx(MATRIX_CHECK_FLOATS(&mtxF, "../z_bg_hidan_sima.c", 624),
                               GRAPH_ALLOC(play->state.gfxCtx, sizeof(Mtx))),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(gfx++, gFireTempleFireballDL);
    return gfx;
}

void Bg_Hidan_Sima_actor_draw(Actor* thisx, PlayState* play) {
    BgHidanSima* this = (BgHidanSima*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_hidan_sima.c", 641);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_hidan_sima.c", 645);
    if (this->dyna.actor.params == 0) {
        gSPDisplayList(POLY_OPA_DISP++, gFireTempleStonePlatform1DL);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, gFireTempleStonePlatform2DL);
        if (this->actionFunc == mode_move) {
            POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_20);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 1, 255, 255, 0, 150);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 255);
            POLY_XLU_DISP = draw_fire(play, this, POLY_XLU_DISP);
        }
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_hidan_sima.c", 668);
}
