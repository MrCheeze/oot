#include "z_bg_mizu_shutter.h"
#include "assets/objects/object_mizu_objects/object_mizu_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Mizu_Shutter_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mizu_Shutter_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mizu_Shutter_actor_move(Actor* thisx, PlayState* play);
void Bg_Mizu_Shutter_actor_draw(Actor* thisx, PlayState* play);

void mode_shutter00_open_wait(BgMizuShutter* this, PlayState* play);
void mode_shutter00_close_wait(BgMizuShutter* this, PlayState* play);
void mode_shutter00_move(BgMizuShutter* this, PlayState* play);
void mode_shutter00_demo_open(BgMizuShutter* this, PlayState* play);

ActorProfile Bg_Mizu_Shutter_Profile = {
    /**/ ACTOR_BG_MIZU_SHUTTER,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_MIZU_OBJECTS,
    /**/ sizeof(BgMizuShutter),
    /**/ Bg_Mizu_Shutter_actor_ct,
    /**/ Bg_Mizu_Shutter_actor_dt,
    /**/ Bg_Mizu_Shutter_actor_move,
    /**/ Bg_Mizu_Shutter_actor_draw,
};

static Gfx* shape_model[] = { gObjectMizuObjectsShutterDL_007130, gObjectMizuObjectsShutterDL_0072D0 };

static CollisionHeader* pbgdata_from_data[] = {
    &gObjectMizuObjectsShutterCol_007250,
    &gObjectMizuObjectsShutterCol_0073F0,
};

static Vec3f mtx_src[] = {
    { 0.0f, 100.0f, 0.0f },
    { 0.0f, 140.0f, 0.0f },
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Mizu_Shutter_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMizuShutter* this = (BgMizuShutter*)thisx;
    s32 pad2;
    CollisionHeader* colHeader = NULL;
    s32 pad3;

    ValueSet_process(&this->dyna.actor, value_init);
    this->displayList = shape_model[BGMIZUSHUTTER_GET_SIZE(&this->dyna.actor)];
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyUty_bgdi_SG2KSG(pbgdata_from_data[BGMIZUSHUTTER_GET_SIZE(&this->dyna.actor)], &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if ((BGMIZUSHUTTER_GET_SIZE(&this->dyna.actor) == BGMIZUSHUTTER_SMALL) ||
        (BGMIZUSHUTTER_GET_SIZE(&this->dyna.actor) == BGMIZUSHUTTER_LARGE)) {
        this->closedPos = this->dyna.actor.world.pos;
        this->timer = 0;
        this->timerMax = BGMIZUSHUTTER_GET_TIMER(&this->dyna.actor) * 20;

        Matrix_rotateY(BINANG_TO_RAD(this->dyna.actor.world.rot.y), MTXMODE_NEW);
        Matrix_rotateX(BINANG_TO_RAD(this->dyna.actor.world.rot.x), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD(this->dyna.actor.world.rot.z), MTXMODE_APPLY);
        Matrix_Position(&mtx_src[BGMIZUSHUTTER_GET_SIZE(&this->dyna.actor)], &this->openPos);
        this->openPos.x += this->dyna.actor.world.pos.x;
        this->openPos.y += this->dyna.actor.world.pos.y;
        this->openPos.z += this->dyna.actor.world.pos.z;

        if (this->timerMax != 0x3F * 20) {
            Actor_Environment_sw_Off(play, BGMIZUSHUTTER_GET_SWITCH(&this->dyna.actor));
            this->dyna.actor.world.pos = this->closedPos;
        }
        if (Actor_Environment_sw_Check(play, BGMIZUSHUTTER_GET_SWITCH(&this->dyna.actor))) {
            this->dyna.actor.world.pos = this->openPos;
            this->actionFunc = mode_shutter00_open_wait;
        } else {
            this->actionFunc = mode_shutter00_close_wait;
        }
    }
}

void Bg_Mizu_Shutter_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMizuShutter* this = (BgMizuShutter*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void mode_shutter00_close_wait(BgMizuShutter* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, BGMIZUSHUTTER_GET_SWITCH(&this->dyna.actor))) {
        if (ABS(this->dyna.actor.world.rot.x) > 0x2C60) {
            makeOnepointDemo(play, 4510, -99, &this->dyna.actor, CAM_ID_MAIN);
        } else {
            makeActorAttentionDemo(play, &this->dyna.actor);
        }
        this->actionFunc = mode_shutter00_demo_open;
        this->timer = 30;
    }
}

void mode_shutter00_demo_open(BgMizuShutter* this, PlayState* play) {
    if (this->timer-- == 0) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_OPEN);
        this->actionFunc = mode_shutter00_move;
    }
}

void mode_shutter00_move(BgMizuShutter* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, BGMIZUSHUTTER_GET_SWITCH(&this->dyna.actor))) {
        add_calc(&this->dyna.actor.world.pos.x, this->openPos.x, 1.0f, 4.0f, 0.1f);
        add_calc(&this->dyna.actor.world.pos.y, this->openPos.y, 1.0f, 4.0f, 0.1f);
        add_calc(&this->dyna.actor.world.pos.z, this->openPos.z, 1.0f, 4.0f, 0.1f);
        if ((this->dyna.actor.world.pos.x == this->openPos.x) && (this->dyna.actor.world.pos.y == this->openPos.y) &&
            (this->dyna.actor.world.pos.z == this->openPos.z)) {
            this->timer = this->timerMax;
            this->actionFunc = mode_shutter00_open_wait;
        }
    } else {
        add_calc(&this->maxSpeed, 20.0f, 1.0f, 3.0f, 0.1f);
        add_calc(&this->dyna.actor.world.pos.x, this->closedPos.x, 1.0f, this->maxSpeed, 0.1f);
        add_calc(&this->dyna.actor.world.pos.y, this->closedPos.y, 1.0f, this->maxSpeed, 0.1f);
        add_calc(&this->dyna.actor.world.pos.z, this->closedPos.z, 1.0f, this->maxSpeed, 0.1f);
        if ((this->dyna.actor.world.pos.x == this->closedPos.x) &&
            (this->dyna.actor.world.pos.y == this->closedPos.y) &&
            (this->dyna.actor.world.pos.z == this->closedPos.z)) {
            z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 120, 20, 10);
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_STONE_BOUND);
            this->actionFunc = mode_shutter00_close_wait;
        }
    }
}

void mode_shutter00_open_wait(BgMizuShutter* this, PlayState* play) {
    if (this->timerMax != 0x3F * 20) {
        this->timer--;
        Actor_timer_level_SE_set(&this->dyna.actor, this->timer);
        if (this->timer == 0) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_CLOSE);
            Actor_Environment_sw_Off(play, BGMIZUSHUTTER_GET_SWITCH(&this->dyna.actor));
            this->actionFunc = mode_shutter00_move;
        }
    }
}

void Bg_Mizu_Shutter_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMizuShutter* this = (BgMizuShutter*)thisx;

    this->actionFunc(this, play);
}

void Bg_Mizu_Shutter_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMizuShutter* this = (BgMizuShutter*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_mizu_shutter.c", 410);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mizu_shutter.c", 415);

    if (this->displayList != NULL) {
        gSPDisplayList(POLY_OPA_DISP++, this->displayList);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_mizu_shutter.c", 422);
}
