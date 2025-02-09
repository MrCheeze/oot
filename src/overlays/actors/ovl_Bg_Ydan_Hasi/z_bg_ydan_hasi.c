/*
 * File: z_bg_ydan_hasi.c
 * Overlay: ovl_Bg_Ydan_Hasi
 * Description: Deku Tree Puzzle elements. Water plane and floating block in B1, and 3 blocks on 2F
 */

#include "z_bg_ydan_hasi.h"
#include "assets/objects/object_ydan_objects/object_ydan_objects.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Ydan_Hasi_actor_ct(Actor* thisx, PlayState* play);
void Bg_Ydan_Hasi_actor_dt(Actor* thisx, PlayState* play);
void Bg_Ydan_Hasi_actor_move(Actor* thisx, PlayState* play);
void Bg_Ydan_Hasi_actor_draw(Actor* thisx, PlayState* play);

static void mode_water_wait(BgYdanHasi* this, PlayState* play);
void mode_lift_move(BgYdanHasi* this, PlayState* play);
void mode_maruta_wait(BgYdanHasi* this, PlayState* play);
static void mode_water_move(BgYdanHasi* this, PlayState* play);
static void mode_water_stop(BgYdanHasi* this, PlayState* play);
void mode_maruta_move(BgYdanHasi* this, PlayState* play);

ActorProfile Bg_Ydan_Hasi_Profile = {
    /**/ ACTOR_BG_YDAN_HASI,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_YDAN_OBJECTS,
    /**/ sizeof(BgYdanHasi),
    /**/ Bg_Ydan_Hasi_actor_ct,
    /**/ Bg_Ydan_Hasi_actor_dt,
    /**/ Bg_Ydan_Hasi_actor_move,
    /**/ Bg_Ydan_Hasi_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Ydan_Hasi_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgYdanHasi* this = (BgYdanHasi*)thisx;
    CollisionHeader* colHeader = NULL;
    WaterBox* waterBox;

    ValueSet_process(thisx, value_init);
    this->type = PARAMS_GET_U(thisx->params, 8, 6);
    thisx->params = PARAMS_GET_U(thisx->params, 0, 8);
    waterBox = &play->colCtx.colHeader->waterBoxes[1];
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    if (thisx->params == HASI_WATER) {
        // Water the moving platform floats on in B1. Never runs in Master Quest
        waterBox->ySurface = thisx->world.pos.y = thisx->home.pos.y += -5.0f;
        this->actionFunc = mode_water_wait;
    } else {
        if (thisx->params == HASI_WATER_BLOCK) {
            // Moving platform on the water in B1
            DynaPolyUty_bgdi_SG2KSG(&gDTSlidingPlatformCol, &colHeader);
            thisx->scale.z = 0.15f;
            thisx->scale.x = 0.15f;
            thisx->world.pos.y = (waterBox->ySurface + 20.0f);
            this->actionFunc = mode_lift_move;
        } else {
            // 3 platforms on 2F
            DynaPolyUty_bgdi_SG2KSG(&gDTRisingPlatformsCol, &colHeader);
            thisx->draw = NULL;
            this->actionFunc = mode_maruta_wait;
            Actor_world_to_eye(&this->dyna.actor, 40.0f);
        }
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    }
    this->timer = 0;
}

void Bg_Ydan_Hasi_actor_dt(Actor* thisx, PlayState* play) {
    BgYdanHasi* this = (BgYdanHasi*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void mode_lift_move(BgYdanHasi* this, PlayState* play) {
    WaterBox* waterBox;
    f32 framesAfterMath;

    framesAfterMath = sinf((play->gameplayFrames & 0xFF) * (M_PI / 128)) * 165.0f;
    this->dyna.actor.world.pos.x =
        ((sin_s(this->dyna.actor.world.rot.y) * framesAfterMath) + this->dyna.actor.home.pos.x);
    this->dyna.actor.world.pos.z =
        ((cos_s(this->dyna.actor.world.rot.y) * framesAfterMath) + this->dyna.actor.home.pos.z);
    waterBox = &play->colCtx.colHeader->waterBoxes[1];
    this->dyna.actor.world.pos.y = waterBox->ySurface + 20.0f;
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        this->timer = 50;
    }
    this->dyna.actor.world.pos.y += 2.0f * sinf(this->timer * (M_PI / 25));
}

static void mode_water_wait(BgYdanHasi* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->type)) {
        this->timer = 600;
        this->actionFunc = mode_water_move;
    }
}

static void mode_water_move(BgYdanHasi* this, PlayState* play) {
    WaterBox* waterBox;

    if (this->timer == 0) {
        if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 1.0f) != 0) {
            Actor_Environment_sw_Off(play, this->type);
            this->actionFunc = mode_water_wait;
        }
        Actor_fix_level_SE_set(&this->dyna.actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
    } else {
        if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 47.0f, 0.5f)) {
            this->actionFunc = mode_water_stop;
        }
        Actor_fix_level_SE_set(&this->dyna.actor, NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
    }
    waterBox = &play->colCtx.colHeader->waterBoxes[1];
    waterBox->ySurface = this->dyna.actor.world.pos.y;
}

static void mode_water_stop(BgYdanHasi* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    Actor_timer_level_SE_set(&this->dyna.actor, this->timer);
    if (this->timer == 0) {
        this->actionFunc = mode_water_move;
    }
}

void mode_maruta_wait(BgYdanHasi* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->type)) {
        this->timer = 260;
        this->dyna.actor.draw = Bg_Ydan_Hasi_actor_draw;
        this->actionFunc = mode_maruta_move;
        makeOnepointDemo(play, 3040, 30, &this->dyna.actor, CAM_ID_MAIN);
    }
}

void mode_maruta_move(BgYdanHasi* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 3.0f) != 0) {
            Actor_Environment_sw_Off(play, this->type);
            this->dyna.actor.draw = NULL;
            this->actionFunc = mode_maruta_wait;
        } else {
            Actor_fix_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
        }
    } else if (!chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 120.0f, 3.0f)) {
        Actor_fix_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);

    } else {
        Actor_timer_level_SE_set(&this->dyna.actor, this->timer);
    }
}

void Bg_Ydan_Hasi_actor_move(Actor* thisx, PlayState* play) {
    BgYdanHasi* this = (BgYdanHasi*)thisx;

    this->actionFunc(this, play);
}

void Bg_Ydan_Hasi_actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* shape_model[] = { gDTSlidingPlatformDL, gDTWaterPlaneDL, gDTRisingPlatformsDL };
    s16 params = thisx->params;

    if (params == HASI_WATER_BLOCK || params == HASI_THREE_BLOCKS) {
        Cheap_gfx_display(play, shape_model[params]);
    } else {
        OPEN_DISPS(play->state.gfxCtx, "../z_bg_ydan_hasi.c", 577);

        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, -play->gameplayFrames % 128,
                                    play->gameplayFrames % 128, 0x20, 0x20, 1, play->gameplayFrames % 128,
                                    play->gameplayFrames % 128, 0x20, 0x20));
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_ydan_hasi.c", 592);
        gSPDisplayList(POLY_XLU_DISP++, gDTWaterPlaneDL);

        CLOSE_DISPS(play->state.gfxCtx, "../z_bg_ydan_hasi.c", 597);
    }
}
