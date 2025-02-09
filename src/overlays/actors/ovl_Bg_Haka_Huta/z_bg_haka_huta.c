/*
 * File: z_bg_haka_huta.c
 * Overlay: ovl_Bg_Haka_Huta
 * Description: Coffin Lid
 */

#include "z_bg_haka_huta.h"
#include "overlays/actors/ovl_En_Rd/z_en_rd.h"

#include "libc64/qrand.h"
#include "ichain.h"
#include "one_point_cutscene.h"
#include "quake.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_hakach_objects/object_hakach_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Haka_Huta_actor_ct(Actor* thisx, PlayState* play);
void Bg_Haka_Huta_actor_dt(Actor* thisx, PlayState* play);
void Bg_Haka_Huta_actor_move(Actor* thisx, PlayState* play);
void Bg_Haka_Huta_actor_draw(Actor* thisx, PlayState* play);

void set_haka_huta_dust(BgHakaHuta* this, PlayState* play);
void haka_huta_set_se(BgHakaHuta* this, PlayState* play, u16 sfx);
static void mode_wait(BgHakaHuta* this, PlayState* play);
static void mode_zure(BgHakaHuta* this, PlayState* play);
static void mode_move(BgHakaHuta* this, PlayState* play);
static void mode_rotate(BgHakaHuta* this, PlayState* play);
static void mode_stop(BgHakaHuta* this, PlayState* play);

ActorProfile Bg_Haka_Huta_Profile = {
    /**/ ACTOR_BG_HAKA_HUTA,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKACH_OBJECTS,
    /**/ sizeof(BgHakaHuta),
    /**/ Bg_Haka_Huta_actor_ct,
    /**/ Bg_Haka_Huta_actor_dt,
    /**/ Bg_Haka_Huta_actor_move,
    /**/ Bg_Haka_Huta_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Haka_Huta_actor_ct(Actor* thisx, PlayState* play) {
    BgHakaHuta* this = (BgHakaHuta*)thisx;
    s16 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(thisx, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyUty_bgdi_SG2KSG(&gBotwCoffinLidCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
    this->unk_16A = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params &= 0xFF;
    if (Actor_Environment_sw_Check(play, thisx->params)) {
        this->counter = -1;
        this->actionFunc = mode_rotate;
    } else {
        this->actionFunc = mode_wait;
    }
}

void Bg_Haka_Huta_actor_dt(Actor* thisx, PlayState* play) {
    BgHakaHuta* this = (BgHakaHuta*)thisx;
    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void set_haka_huta_dust(BgHakaHuta* this, PlayState* play) {
    static Vec3f zero_vec[] = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 prim = { 30, 20, 50, 255 };
    static Color_RGBA8 env = { 0, 0, 0, 255 };
    f32 scale;
    f32 phi_f20;
    Vec3f effectPos;
    Vec3f effectVel;
    s32 i;
    f32 new_Xpos;
    f32 xPosOffset;

    phi_f20 = (this->dyna.actor.world.rot.y == 0) ? 1.0f : -1.0f;
    effectVel.y = 0.0f;
    effectVel.z = 0.0f;
    effectVel.x = -0.5f * phi_f20;
    effectPos.y = this->dyna.actor.world.pos.y;
    effectPos.z = this->dyna.actor.world.pos.z;
    new_Xpos = 50 - ((this->dyna.actor.world.pos.x - this->dyna.actor.home.pos.x) * phi_f20);
    xPosOffset = new_Xpos * phi_f20;

    for (i = 0; i < 4; i++) {
        if (i == 2) {
            effectPos.z += 120.0f * phi_f20;
        }
        effectPos.x = this->dyna.actor.home.pos.x - (fqrand() * xPosOffset);
        scale = ((fqrand() * 10.0f) + 50.0f);
        Effect_SS_Dust_sc_cl_ct(play, &effectPos, &effectVel, zero_vec, &prim, &env, scale, 0xA);
    }
}

void haka_huta_set_se(BgHakaHuta* this, PlayState* play, u16 sfx) {
    Vec3f pos;

    pos.z = (this->dyna.actor.shape.rot.y == 0) ? this->dyna.actor.world.pos.z + 120.0f
                                                : this->dyna.actor.world.pos.z - 120.0f;
    pos.x = this->dyna.actor.world.pos.x;
    pos.y = this->dyna.actor.world.pos.y;
    Effect_SE_Info_new(play, &pos, 30, sfx);
}

static void mode_wait(BgHakaHuta* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->dyna.actor.params) && !player_demo_check(play)) {
        this->counter = 25;
        this->actionFunc = mode_zure;
        makeOnepointDemo(play, 6001, 999, &this->dyna.actor, CAM_ID_MAIN);
        if (this->unk_16A == 2) {
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_FIREFLY,
                        (this->dyna.actor.world.pos.x + (-25.0f) * cos_s(this->dyna.actor.shape.rot.y) +
                         40.0f * sin_s(this->dyna.actor.shape.rot.y)),
                        this->dyna.actor.world.pos.y - 10.0f,
                        (this->dyna.actor.world.pos.z - (-25.0f) * sin_s(this->dyna.actor.shape.rot.y) +
                         cos_s(this->dyna.actor.shape.rot.y) * 40.0f),
                        0, this->dyna.actor.shape.rot.y + 0x8000, 0, 2);

            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_FIREFLY,
                        (this->dyna.actor.world.pos.x + (-25.0f) * (cos_s(this->dyna.actor.shape.rot.y)) +
                         sin_s(this->dyna.actor.shape.rot.y) * 80.0f),
                        this->dyna.actor.world.pos.y - 10.0f,
                        (this->dyna.actor.world.pos.z - (-25.0f) * (sin_s(this->dyna.actor.shape.rot.y)) +
                         cos_s(this->dyna.actor.shape.rot.y) * 80.0f),
                        0, this->dyna.actor.shape.rot.y, 0, 2);

        } else if (this->unk_16A == 1) {
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_RD,
                        (this->dyna.actor.home.pos.x + (-25.0f) * (cos_s(this->dyna.actor.shape.rot.y)) +
                         sin_s(this->dyna.actor.shape.rot.y) * 100.0f),
                        this->dyna.actor.home.pos.y - 40.0f,
                        (this->dyna.actor.home.pos.z - (-25.0f) * (sin_s(this->dyna.actor.shape.rot.y)) +
                         cos_s(this->dyna.actor.shape.rot.y) * 100.0f),
                        0, this->dyna.actor.shape.rot.y, 0, (u8)REDEAD_TYPE_GIBDO_RISING_OUT_OF_COFFIN);
        }
    }
}

static void mode_zure(BgHakaHuta* this, PlayState* play) {
    f32 posOffset;

    if (this->counter != 0) {
        this->counter--;
    }
    posOffset = (this->dyna.actor.world.rot.y == 0) ? 4.0f : -4.0f;
    chase_f(&this->dyna.actor.world.pos.x, this->dyna.actor.home.pos.x + posOffset, 2.0f);
    if (this->counter == 0) {
        this->counter = 37;
        haka_huta_set_se(this, play, NA_SE_EV_COFFIN_CAP_OPEN);
        this->actionFunc = mode_move;
    }
}

static void mode_move(BgHakaHuta* this, PlayState* play) {
    f32 posOffset;

    if (this->counter != 0) {
        this->counter--;
    }
    posOffset = (this->dyna.actor.world.rot.y == 0) ? 24.0f : -24.0f;
    if (!chase_f(&this->dyna.actor.world.pos.x, this->dyna.actor.home.pos.x + posOffset, 0.5f)) {
        set_haka_huta_dust(this, play);
    }
    if (this->counter == 0) {
        haka_huta_set_se(this, play, NA_SE_EV_COFFIN_CAP_BOUND);
        this->actionFunc = mode_rotate;
    }
}

static void mode_rotate(BgHakaHuta* this, PlayState* play) {
    static Vec3f local_pos = { 30.0f, 0.0f, 0.0f };
    static Vec3f rotate_vec = { 0.03258f, 0.3258f, -0.9449f };
    MtxF mtx;
    Vec3f vec;
    s32 quakeIndex;

    this->counter++;
    if (this->counter == 6) {
        this->actionFunc = mode_stop;
        quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
        setSpeedQuake(quakeIndex, 0x7530);
        setScaleQuake(quakeIndex, 4, 0, 0, 0);
        setTimerQuake(quakeIndex, 2);
    } else if (this->counter == 0) {
        this->counter = 6;
        this->actionFunc = mode_stop;
    }

    local_pos.x = this->counter + 24.0f;
    if (local_pos.x > 30.0f) {
        local_pos.x = 30.0f;
    }
    Matrix_rotateY(BINANG_TO_RAD(this->dyna.actor.world.rot.y), MTXMODE_NEW);
    Matrix_rotateVector(this->counter * (191 * M_PI / 3750), &rotate_vec, MTXMODE_APPLY);
    Matrix_Position(&local_pos, &vec);
    this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x + vec.x;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + vec.y;
    this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z + vec.z;
    Matrix_get(&mtx);
    Matrix_to_rotate_new(&mtx, &this->dyna.actor.shape.rot, 0);
}

static void mode_stop(BgHakaHuta* this, PlayState* play) {
}

void Bg_Haka_Huta_actor_move(Actor* thisx, PlayState* play) {
    BgHakaHuta* this = (BgHakaHuta*)thisx;

    this->actionFunc(this, play);
}

void Bg_Haka_Huta_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gBotwCoffinLidDL);
}
