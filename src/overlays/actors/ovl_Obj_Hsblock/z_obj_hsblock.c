/*
 * File: z_obj_hsblock.c
 * Overlay: ovl_Obj_Hsblock
 * Description: Stone Hookshot Target
 */

#include "z_obj_hsblock.h"
#include "assets/objects/object_d_hsblock/object_d_hsblock.h"

#define FLAGS 0

void Obj_Hsblock_actor_ct(Actor* thisx, PlayState* play);
void Obj_Hsblock_actor_dt(Actor* thisx, PlayState* play);
void Obj_Hsblock_actor_move(Actor* thisx, PlayState* play);
void Obj_Hsblock_actor_draw(Actor* thisx, PlayState* play);

static void mv_wait(ObjHsblock* this, PlayState* play);
void mv_appear(ObjHsblock* this, PlayState* play);

static void mv_stop_init(ObjHsblock* this);
static void mv_wait_init(ObjHsblock* this);
void mv_appear_init(ObjHsblock* this);

ActorProfile Obj_Hsblock_Profile = {
    /**/ ACTOR_OBJ_HSBLOCK,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_D_HSBLOCK,
    /**/ sizeof(ObjHsblock),
    /**/ Obj_Hsblock_actor_ct,
    /**/ Obj_Hsblock_actor_dt,
    /**/ Obj_Hsblock_actor_move,
    /**/ Obj_Hsblock_actor_draw,
};

static f32 EyeOffset[] = { 85.0f, 85.0f, 0.0f };

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2000, ICHAIN_STOP),
};

static CollisionHeader* BGDT_info[] = { &gHookshotPostCol, &gHookshotPostCol, &gHookshotTargetCol };

static Color_RGB8 col_dt_HIDAN = { 165, 125, 55 };

static Gfx* model[] = { gHookshotPostDL, gHookshotPostDL, gHookshotTargetDL };

static void set_moveProc(ObjHsblock* this, ObjHsblockActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static void set_dynaPoly(ObjHsblock* this, PlayState* play, CollisionHeader* collision, s32 transformFlags) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, transformFlags);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_obj_hsblock.c", 163,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void set_ice_poly(ObjHsblock* this, PlayState* play) {
    if (PARAMS_GET_U(this->dyna.actor.params, 5, 1)) {
        Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_OBJ_ICE_POLY, this->dyna.actor.world.pos.x,
                           this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, this->dyna.actor.world.rot.x,
                           this->dyna.actor.world.rot.y, this->dyna.actor.world.rot.z, 1);
    }
}

void Obj_Hsblock_actor_ct(Actor* thisx, PlayState* play) {
    ObjHsblock* this = (ObjHsblock*)thisx;

    set_dynaPoly(this, play, BGDT_info[PARAMS_GET_U(thisx->params, 0, 2)], 0);
    ValueSet_process(thisx, value_init);
    set_ice_poly(this, play);

    switch (PARAMS_GET_U(thisx->params, 0, 2)) {
        case 0:
        case 2:
            mv_stop_init(this);
            break;
        case 1:
            if (Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 8, 6))) {
                mv_stop_init(this);
            } else {
                mv_wait_init(this);
            }
    }

#if DEBUG_FEATURES
    mREG(13) = 255;
    mREG(14) = 255;
    mREG(15) = 255;
#endif
}

void Obj_Hsblock_actor_dt(Actor* thisx, PlayState* play) {
    ObjHsblock* this = (ObjHsblock*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mv_stop_init(ObjHsblock* this) {
    set_moveProc(this, NULL);
}

static void mv_wait_init(ObjHsblock* this) {
    this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 105.0f;
    set_moveProc(this, mv_wait);
}

static void mv_wait(ObjHsblock* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
        mv_appear_init(this);
    }
}

void mv_appear_init(ObjHsblock* this) {
    set_moveProc(this, mv_appear);
}

void mv_appear(ObjHsblock* this, PlayState* play) {
    add_calc(&this->dyna.actor.velocity.y, 16.0f, 0.1f, 0.8f, 0.0f);
    if (fabsf(add_calc(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 0.3f,
                                 this->dyna.actor.velocity.y, 0.3f)) < 0.001f) {
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
        mv_stop_init(this);
        this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }
}

void Obj_Hsblock_actor_move(Actor* thisx, PlayState* play) {
    ObjHsblock* this = (ObjHsblock*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
    Actor_world_to_eye(thisx, EyeOffset[PARAMS_GET_U(thisx->params, 0, 2)]);
}

void Obj_Hsblock_actor_draw(Actor* thisx, PlayState* play) {
    Color_RGB8* color;
    Color_RGB8 defaultColor;

    OPEN_DISPS(play->state.gfxCtx, "../z_obj_hsblock.c", 365);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_obj_hsblock.c", 369);

    if (play->sceneId == SCENE_FIRE_TEMPLE) {
        color = &col_dt_HIDAN;
    } else {
#if DEBUG_FEATURES
        defaultColor.r = mREG(13);
        defaultColor.g = mREG(14);
        defaultColor.b = mREG(15);
#else
        defaultColor.r = 255;
        defaultColor.g = 255;
        defaultColor.b = 255;
#endif

        color = &defaultColor;
    }

    gDPSetEnvColor(POLY_OPA_DISP++, color->r, color->g, color->b, 255);
    gSPDisplayList(POLY_OPA_DISP++, model[PARAMS_GET_U(thisx->params, 0, 2)]);

    CLOSE_DISPS(play->state.gfxCtx, "../z_obj_hsblock.c", 399);
}
