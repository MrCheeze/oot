/*
 * File: z_en_light.c
 * Overlay: ovl_En_Light
 * Description: Flame
 */

#include "z_en_light.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

#define FLAGS 0

void En_Light_Actor_ct(Actor* thisx, PlayState* play);
void En_Light_Actor_dt(Actor* thisx, PlayState* play);
void En_Light_Actor_move(Actor* thisx, PlayState* play);
void En_Light_Actor_draw(Actor* thisx, PlayState* play);
void En_Light_Actor_move_sw(Actor* thisx, PlayState* play);

ActorProfile En_Light_Profile = {
    /**/ ACTOR_EN_LIGHT,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnLight),
    /**/ En_Light_Actor_ct,
    /**/ En_Light_Actor_dt,
    /**/ En_Light_Actor_move,
    /**/ En_Light_Actor_draw,
};

typedef struct FlameParams {
    /* 0x00 */ Color_RGBA8 primColor;
    /* 0x04 */ Color_RGB8 envColor;
    /* 0x07 */ u8 scale;
} FlameParams;

static FlameParams fire_color_data[] = {
    { { 255, 200, 0, 255 }, { 255, 0, 0 }, 75 },     { { 255, 200, 0, 255 }, { 255, 0, 0 }, 75 },
    { { 0, 170, 255, 255 }, { 0, 0, 255 }, 75 },     { { 170, 255, 0, 255 }, { 0, 150, 0 }, 75 },
    { { 255, 200, 0, 255 }, { 255, 0, 0 }, 40 },     { { 255, 200, 0, 255 }, { 255, 0, 0 }, 75 },
    { { 170, 255, 0, 255 }, { 0, 150, 0 }, 75 },     { { 0, 170, 255, 255 }, { 0, 0, 255 }, 75 },
    { { 255, 0, 170, 255 }, { 200, 0, 0 }, 75 },     { { 255, 255, 170, 255 }, { 255, 50, 0 }, 75 },
    { { 255, 255, 170, 255 }, { 255, 255, 0 }, 75 }, { { 255, 255, 170, 255 }, { 100, 255, 0 }, 75 },
    { { 255, 170, 255, 255 }, { 255, 0, 100 }, 75 }, { { 255, 170, 255, 255 }, { 100, 0, 255 }, 75 },
    { { 170, 255, 255, 255 }, { 0, 0, 255 }, 75 },   { { 170, 255, 255, 255 }, { 0, 150, 255 }, 75 },
};

void En_Light_Actor_ct(Actor* thisx, PlayState* play) {
    EnLight* this = (EnLight*)thisx;
    s16 yOffset;

    if (z_common_data.gameMode == GAMEMODE_END_CREDITS) {
        yOffset = (this->actor.params < 0) ? 1 : 40;
        Light_point_ct(&this->lightInfo, this->actor.world.pos.x, yOffset + (s16)this->actor.world.pos.y,
                                  this->actor.world.pos.z, 255, 255, 180, -1);
    } else {
        yOffset = (this->actor.params < 0) ? 1 : 40;
        Light_point2_ct(&this->lightInfo, this->actor.world.pos.x, yOffset + (s16)this->actor.world.pos.y,
                                this->actor.world.pos.z, 255, 255, 180, -1);
    }

    this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
    Actor_set_scale(&this->actor, fire_color_data[PARAMS_GET_S(this->actor.params, 0, 4)].scale * 0.0001f);
    this->timer = (s32)(fqrand() * 255.0f);

    if (PARAMS_GET_NOSHIFT(this->actor.params, 10, 1)) {
        this->actor.update = En_Light_Actor_move_sw;
    }
}

void En_Light_Actor_dt(Actor* thisx, PlayState* play) {
    EnLight* this = (EnLight*)thisx;

    Global_light_list_delete(play, &play->lightCtx, this->lightNode);
}

void En_Light_Actor_move_common(EnLight* this, PlayState* play) {
    // update yaw for billboard effect
    this->actor.shape.rot.y = getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000;

    if (this->actor.parent != NULL) {
        xyz_t_move(&this->actor.world.pos, &(this->actor.parent)->world.pos);
        this->actor.world.pos.y += 17.0f;
    }

    this->timer++;
}

void En_Light_Actor_move(Actor* thisx, PlayState* play) {
    f32 intensity;
    FlameParams* flameParams;
    s16 radius;
    EnLight* this = (EnLight*)thisx;

    flameParams = &fire_color_data[PARAMS_GET_S(this->actor.params, 0, 4)];
    intensity = (fqrand() * 0.5f) + 0.5f;
    radius = (this->actor.params < 0) ? 100 : 300;
    Light_point_color_set(&this->lightInfo, (flameParams->primColor.r * intensity),
                                  (flameParams->primColor.g * intensity), (flameParams->primColor.b * intensity),
                                  radius);
    En_Light_Actor_move_common(this, play);

    if (this->actor.params >= 0) {
        Actor_SE_set(&this->actor, NA_SE_EV_TORCH - SFX_FLAG);
    }
}

void En_Light_Actor_move_sw(Actor* thisx, PlayState* play) {
    f32 intensity;
    FlameParams* flameParams;
    EnLight* this = (EnLight*)thisx;
    f32 scale;

    flameParams = &fire_color_data[PARAMS_GET_S(this->actor.params, 0, 4)];
    scale = this->actor.scale.x / ((f32)flameParams->scale * 0.0001);

    if (PARAMS_GET_NOSHIFT(this->actor.params, 11, 1)) {
        if (Actor_Environment_sw_Check(play, PARAMS_GET_S(this->actor.params, 4, 6))) {
            chase_f(&scale, 1.0f, 0.05f);
        } else {
            if (scale < 0.1f) {
                Actor_set_scale(&this->actor, 0.0f);
                return;
            }
            chase_f(&scale, 0.0f, 0.05f);
        }
    } else {
        if (Actor_Environment_sw_Check(play, PARAMS_GET_S(this->actor.params, 4, 6))) {
            if (scale < 0.1f) {
                Actor_set_scale(&this->actor, 0.0f);
                return;
            }
            chase_f(&scale, 0.0f, 0.05f);
        } else {
            chase_f(&scale, 1.0f, 0.05f);
        }
    }

    Actor_set_scale(&this->actor, ((f32)flameParams->scale * 0.0001) * scale);
    intensity = (fqrand() * 0.5f) + 0.5f;
    Light_point_color_set(&this->lightInfo, (flameParams->primColor.r * intensity),
                                  (flameParams->primColor.g * intensity), (flameParams->primColor.b * intensity),
                                  300.0f * scale);
    En_Light_Actor_move_common(this, play);

    if (this->actor.params >= 0) {
        Actor_SE_set(&this->actor, NA_SE_EV_TORCH - SFX_FLAG);
    }
}

void En_Light_Actor_draw(Actor* thisx, PlayState* play) {
    EnLight* this = (EnLight*)thisx;
    s32 pad;
    FlameParams* flameParams;
    Gfx* dList;

    if (1) {}

    flameParams = &fire_color_data[PARAMS_GET_S(this->actor.params, 0, 4)];

    OPEN_DISPS(play->state.gfxCtx, "../z_en_light.c", 441);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    if (this->actor.params >= 0) {
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0, (this->timer * -20) & 511,
                                    32, 128));

        dList = gEffFire1DL;
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, flameParams->primColor.r, flameParams->primColor.g,
                        flameParams->primColor.b, flameParams->primColor.a);
        gDPSetEnvColor(POLY_XLU_DISP++, flameParams->envColor.r, flameParams->envColor.g, flameParams->envColor.b, 0);
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 16, 32, 1, ((this->timer * 2) & 63),
                                    (this->timer * -6) & 127 * 1, 16, 32));

        dList = gUnusedCandleDL;
        gDPSetPrimColor(POLY_XLU_DISP++, 0xC0, 0xC0, 255, 200, 0, 0);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    }

    Matrix_rotateY(BINANG_TO_RAD((s16)((getRealCameraAngleY(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y) + 0x8000)),
                   MTXMODE_APPLY);

    if (PARAMS_GET_S(this->actor.params, 0, 1)) {
        Matrix_rotateY(M_PI, MTXMODE_APPLY);
    }

    Matrix_scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_light.c", 488);
    gSPDisplayList(POLY_XLU_DISP++, dList);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_light.c", 491);
}
