/*
 * File: z_bg_po_syokudai.c
 * Overlay: ovl_Bg_Po_Syokudai
 * Description: Golden Torch Stand (Poe Sisters)
 */

#include "z_bg_po_syokudai.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_syokudai/object_syokudai.h"

#define FLAGS 0

typedef enum PoeFlameColor {
    POE_FLAME_PURPLE, // Meg
    POE_FLAME_RED,    // Joelle
    POE_FLAME_BLUE,   // Beth
    POE_FLAME_GREEN   // Amy
} PoeFlameColor;

#define POE_TORCH_FLAG 0x1C

void Bg_Po_Syokudai_actor_ct(Actor* thisx, PlayState* play);
void Bg_Po_Syokudai_actor_dt(Actor* thisx, PlayState* play);
void Bg_Po_Syokudai_actor_move(Actor* thisx, PlayState* play);
void Bg_Po_Syokudai_actor_draw(Actor* thisx, PlayState* play);

static ColliderCylinderInit PoSyokudaiOcPipeData = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 12, 60, 0, { 0, 0, 0 } },
};

static Color_RGBA8 fire_prim[] = {
    { 255, 170, 255, 255 },
    { 255, 200, 0, 255 },
    { 0, 170, 255, 255 },
    { 170, 255, 0, 255 },
};

static Color_RGBA8 fire_env[] = {
    { 100, 0, 255, 255 },
    { 255, 0, 0, 255 },
    { 0, 0, 255, 255 },
    { 0, 150, 0, 255 },
};

ActorProfile Bg_Po_Syokudai_Profile = {
    /**/ ACTOR_BG_PO_SYOKUDAI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SYOKUDAI,
    /**/ sizeof(BgPoSyokudai),
    /**/ Bg_Po_Syokudai_actor_ct,
    /**/ Bg_Po_Syokudai_actor_dt,
    /**/ Bg_Po_Syokudai_actor_move,
    /**/ Bg_Po_Syokudai_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void Bg_Po_Syokudai_actor_ct(Actor* thisx, PlayState* play) {
    BgPoSyokudai* this = (BgPoSyokudai*)thisx;
    s32 pad;

    ValueSet_process(thisx, value_init);

    this->flameColor = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params &= 0x3F;

    thisx->colChkInfo.mass = MASS_IMMOVABLE;

    this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
    Light_point2_ct(&this->lightInfo, thisx->world.pos.x, (s16)thisx->world.pos.y + 65, thisx->world.pos.z, 0,
                            0, 0, 0);

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, thisx, &PoSyokudaiOcPipeData);

    this->collider.dim.pos.x = thisx->world.pos.x;
    this->collider.dim.pos.y = thisx->world.pos.y;
    this->collider.dim.pos.z = thisx->world.pos.z;

    if (this->flameColor == POE_FLAME_PURPLE && Actor_Environment_sw_Check(play, POE_TORCH_FLAG + POE_FLAME_GREEN) &&
        Actor_Environment_sw_Check(play, POE_TORCH_FLAG + POE_FLAME_BLUE) &&
        Actor_Environment_sw_Check(play, POE_TORCH_FLAG + POE_FLAME_RED) && !Actor_Environment_sw_Check(play, thisx->params)) {

        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_PO_SISTERS, 119.0f, 225.0f, -1566.0f, 0, 0, 0, thisx->params);
        play->envCtx.lightSettingOverride = 4;

    } else if (!Actor_Environment_sw_Check(play, POE_TORCH_FLAG + POE_FLAME_PURPLE) && !Actor_Environment_sw_Check(play, 0x1B)) {

        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_PO_SISTERS, thisx->world.pos.x, thisx->world.pos.y + 52.0f,
                    thisx->world.pos.z, 0, 0, 0, (this->flameColor << 8) + thisx->params + 0x1000);

    } else if (!Actor_Environment_sw_Check(play, thisx->params)) {
        if (play->envCtx.lightSettingOverride == LIGHT_SETTING_OVERRIDE_NONE) {
            play->envCtx.lightSettingOverride = 4;
        }
    }

    this->flameTextureScroll = (s16)(fqrand() * 20.0f);
}

void Bg_Po_Syokudai_actor_dt(Actor* thisx, PlayState* play) {
    BgPoSyokudai* this = (BgPoSyokudai*)thisx;

    Global_light_list_delete(play, &play->lightCtx, this->lightNode);
    ClObjPipe_dt(play, &this->collider);

    if (play->envCtx.lightSettingOverride != LIGHT_SETTING_OVERRIDE_NONE) {
        play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
    }
}

void Bg_Po_Syokudai_actor_move(Actor* thisx, PlayState* play) {
    BgPoSyokudai* this = (BgPoSyokudai*)thisx;
    s32 pad;

    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    if (Actor_Environment_sw_Check(play, this->actor.params)) {
        Actor_level_SE_set(&this->actor, NA_SE_EV_TORCH - SFX_FLAG);
    }
    this->flameTextureScroll++;
}

void Bg_Po_Syokudai_actor_draw(Actor* thisx, PlayState* play) {
    BgPoSyokudai* this = (BgPoSyokudai*)thisx;
    f32 lightBrightness;
    u8 red;
    u8 green;
    u8 blue;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_po_syokudai.c", 315);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_po_syokudai.c", 319);
    gSPDisplayList(POLY_OPA_DISP++, gGoldenTorchDL);

    if (Actor_Environment_sw_Check(play, this->actor.params)) {
        Color_RGBA8* primColor = &fire_prim[this->flameColor];
        Color_RGBA8* envColor = &fire_env[this->flameColor];

        lightBrightness = (0.3f * fqrand()) + 0.7f;

        red = (u8)(primColor->r * lightBrightness);
        green = (u8)(primColor->g * lightBrightness);
        blue = (u8)(primColor->b * lightBrightness);

        Light_point_color_set(&this->lightInfo, red, green, blue, 200);

        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0,
                                    (this->flameTextureScroll * -20) & 0x1FF, 32, 128));

        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, primColor->r, primColor->g, primColor->b, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, envColor->r, envColor->g, envColor->b, 255);

        Matrix_translate(0.0f, 52.0f, 0.0f, MTXMODE_APPLY);
        Matrix_rotateY(
            BINANG_TO_RAD((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y + 0x8000)),
            MTXMODE_APPLY);
        Matrix_scale(0.0027f, 0.0027f, 0.0027f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_po_syokudai.c", 368);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_po_syokudai.c", 373);
}
