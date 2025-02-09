/*
 * File: z_en_anubice_fire.c
 * Overlay: ovl_En_Anubice_Fire
 * Description: Anubis Fire Attack
 */

#include "z_en_anubice_fire.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_anubice/object_anubice.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Anubice_Fire_actor_ct(Actor* thisx, PlayState* play);
void En_Anubice_Fire_actor_dt(Actor* thisx, PlayState* play);
void En_Anubice_Fire_actor_move(Actor* thisx, PlayState* play);
void En_Anubice_Fire_actor_draw(Actor* thisx, PlayState* play);

static void mode_move_init(EnAnubiceFire* this, PlayState* play);
static void mode_move(EnAnubiceFire* this, PlayState* play);
static void mode_clear(EnAnubiceFire* this, PlayState* play);

ActorProfile En_Anubice_Fire_Profile = {
    /**/ ACTOR_EN_ANUBICE_FIRE,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_ANUBICE,
    /**/ sizeof(EnAnubiceFire),
    /**/ En_Anubice_Fire_actor_ct,
    /**/ En_Anubice_Fire_actor_dt,
    /**/ En_Anubice_Fire_actor_move,
    /**/ En_Anubice_Fire_actor_draw,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x01, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 0, 0, 0, { 0, 0, 0 } },
};

void En_Anubice_Fire_actor_ct(Actor* thisx, PlayState* play) {
    EnAnubiceFire* this = (EnAnubiceFire*)thisx;
    s32 i;

    ClObjPipe_ct(play, &this->cylinder);
    ClObjPipe_set5(play, &this->cylinder, &this->actor, &OcInfoData);

    this->unk_15A = 30;
    this->unk_154 = 2.0f;
    this->scale = 0.0f;

    for (i = 0; i < 6; i++) {
        this->unk_160[i] = this->actor.world.pos;
    }

    this->unk_15E = 0;
    this->actionFunc = mode_move_init;
}

void En_Anubice_Fire_actor_dt(Actor* thisx, PlayState* play) {
    EnAnubiceFire* this = (EnAnubiceFire*)thisx;

    ClObjPipe_dt(play, &this->cylinder);
}

static void mode_move_init(EnAnubiceFire* this, PlayState* play) {
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };

    Matrix_push();
    Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.world.rot.y), MTXMODE_NEW);
    Matrix_rotateX(BINANG_TO_RAD_ALT(this->actor.world.rot.x), MTXMODE_APPLY);
    velocity.z = 15.0f;
    Matrix_Position(&velocity, &this->actor.velocity);
    Matrix_pull();

    this->actionFunc = mode_move;
    this->actor.world.rot.x = this->actor.world.rot.y = this->actor.world.rot.z = 0;
}

static void mode_move(EnAnubiceFire* this, PlayState* play) {
    s32 pad;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    Color_RGBA8 primColor = { 255, 255, 0, 255 };
    Color_RGBA8 envColor = { 255, 0, 0, 255 };
    Vec3f sp84 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp78 = { 0.0f, 0.0f, 0.0f };

    this->actor.world.rot.z += 5000;
    if (this->unk_15A == 0) {
        this->unk_154 = 0.0f;
    }

    add_calc2(&this->scale, this->unk_154, 0.2f, 0.4f);
    if ((this->unk_15A == 0) && (this->scale < 0.1f)) {
        Actor_delete(&this->actor);
    } else if ((this->actor.params == 0) && (this->cylinder.base.atFlags & AT_BOUNCED)) {
        if (mirror_shield_check(play)) {
            Actor_SE_set(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
            this->cylinder.base.atFlags &= ~(AT_HIT | AT_BOUNCED | AT_TYPE_ENEMY);
            this->cylinder.base.atFlags |= AT_TYPE_PLAYER;
            this->cylinder.elem.atDmgInfo.dmgFlags = DMG_DEKU_STICK;
            this->unk_15A = 30;
            this->actor.params = 1;
            this->actor.velocity.x *= -1.0f;
            this->actor.velocity.y *= -0.5f;
            this->actor.velocity.z *= -1.0f;
        } else {
            this->unk_15A = 0;
            Effect_SS_Bomb2_2_ct(play, &this->actor.world.pos, &sp78, &sp84, 10, 5);
            this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;
            Actor_SE_set(&this->actor, NA_SE_EN_ANUBIS_FIREBOMB);
            this->actionFunc = mode_clear;
        }
    } else if (!(this->scale < .4f)) {
        f32 scale = 1000.0f;
        f32 life = 10.0f;
        s32 i;

        for (i = 0; i < 10; i++) {
            pos.x = this->actor.world.pos.x + (fqrand() - 0.5f) * (this->scale * 20.0f);
            pos.y = this->actor.world.pos.y + (fqrand() - 0.5f) * (this->scale * 20.0f);
            pos.z = this->actor.world.pos.z;
            Effect_SS_KiraKira_sc_ct_ct(play, &pos, &velocity, &accel, &primColor, &envColor, scale, life);
        }
        Actor_SE_set(&this->actor, NA_SE_EN_ANUBIS_FIRE - SFX_FLAG);
    }
}

static void mode_clear(EnAnubiceFire* this, PlayState* play) {
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    Color_RGBA8 primColor = { 255, 255, 0, 255 };
    Color_RGBA8 envColor = { 255, 0, 0, 255 };
    s32 pad;
    s32 i;

    if (this->unk_15C == 0) {
        for (i = 0; i < 20; i++) {
            pos.x = this->actor.world.pos.x;
            pos.y = this->actor.world.pos.y;
            pos.z = this->actor.world.pos.z;
            accel.x = rnd_fx(8.0f);
            accel.y = rnd_fx(2.0f);
            accel.z = rnd_fx(8.0f);
            Effect_SS_KiraKira_sc_ct_ct(play, &pos, &velocity, &accel, &primColor, &envColor, 2000, 10);
        }

        this->unk_15C = 2;
        this->unk_15E++;
        if (this->unk_15E >= 6) {
            Actor_delete(&this->actor);
        }
    }
}

void En_Anubice_Fire_actor_move(Actor* thisx, PlayState* play) {
    EnAnubiceFire* this = (EnAnubiceFire*)thisx;
    s32 pad;
    s32 i;

    Actor_set_scale(&this->actor, this->scale);
    this->actionFunc(this, play);
    Actor_position_move(&this->actor);
    this->unk_160[0] = this->actor.world.pos;

    for (i = 4; i >= 0; i--) {
        this->unk_160[i + 1] = this->unk_160[i];
    }

    if (this->unk_15A != 0) {
        this->unk_15A--;
    }

    if (this->unk_15C != 0) {
        this->unk_15C--;
    }

    Actor_BGcheck2(play, &this->actor, 5.0f, 5.0f, 10.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    if (!(this->scale < 0.6f || this->actionFunc == mode_clear)) {
        this->cylinder.dim.radius = this->scale * 15.0f + 5.0f;
        this->cylinder.dim.height = this->scale * 15.0f + 5.0f;
        this->cylinder.dim.yShift = this->scale * -0.75f + -15.0f;

        if (this->unk_15A != 0) {
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->cylinder);
            CollisionCheck_setAT(play, &play->colChkCtx, &this->cylinder.base);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->cylinder.base);
        }

        if (T_BGCheck_SimpleCheck(&play->colCtx, &this->actor.world.pos, 30.0f)) {
            this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;
            Actor_SE_set(&this->actor, NA_SE_EN_ANUBIS_FIREBOMB);
            this->actionFunc = mode_clear;
        }
    }
}

void En_Anubice_Fire_actor_draw(Actor* thisx, PlayState* play) {
    static void* SEXan_txt[] = {
        gDust4Tex, gDust5Tex, gDust6Tex, gDust7Tex, gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex,
    };
    EnAnubiceFire* this = (EnAnubiceFire*)thisx;
    s32 pad[2];
    s32 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_anubice_fire.c", 503);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    gDPPipeSync(POLY_XLU_DISP++);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(SEXan_txt[0]));

    Matrix_push();
    for (i = this->unk_15E; i < 6; ++i) {
        f32 scale = this->actor.scale.x - (i * 0.2f);

        if (scale < 0.0f) {
            scale = 0.0f;
        }

        if (scale >= 0.1f) {
            Matrix_translate(this->unk_160[i].x, this->unk_160[i].y, this->unk_160[i].z, MTXMODE_NEW);
            Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_rotateZ(this->actor.world.rot.z + i * 1000.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_anubice_fire.c", 546);

            gSPDisplayList(POLY_XLU_DISP++, gAnubiceFireAttackDL);
        }

        if (this->scale < 0.1f) {
            break;
        }
    }
    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_anubice_fire.c", 556);
}
