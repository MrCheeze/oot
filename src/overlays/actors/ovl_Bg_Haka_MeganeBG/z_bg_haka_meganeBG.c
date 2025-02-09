/*
 * File: z_bg_haka_meganebg.c
 * Overlay: ovl_Bg_Haka_MeganeBG
 * Description:
 */

#include "z_bg_haka_meganebg.h"

#include "ichain.h"
#include "one_point_cutscene.h"
#include "sfx.h"
#include "z_lib.h"
#include "z64play.h"

#include "assets/objects/object_haka_objects/object_haka_objects.h"

#define FLAGS 0

void Bg_Haka_MeganeBG_actor_ct(Actor* thisx, PlayState* play);
void Bg_Haka_MeganeBG_actor_dt(Actor* thisx, PlayState* play);
void Bg_Haka_MeganeBG_actor_move(Actor* thisx, PlayState* play);
void Bg_Haka_MeganeBG_actor_draw(Actor* thisx, PlayState* play);

void mode_slide_stop(BgHakaMeganeBG* this, PlayState* play);
void mode_slide_move(BgHakaMeganeBG* this, PlayState* play);
static void mode_lift_drop(BgHakaMeganeBG* this, PlayState* play);
void mode_lift_up(BgHakaMeganeBG* this, PlayState* play);
void mode_rtubo_rotate(BgHakaMeganeBG* this, PlayState* play);
void mode_coinshutter_wait(BgHakaMeganeBG* this, PlayState* play);
void mode_coinshutter_move(BgHakaMeganeBG* this, PlayState* play);
void mode_coinshutter_stop(BgHakaMeganeBG* this, PlayState* play);

ActorProfile Bg_Haka_MeganeBG_Profile = {
    /**/ ACTOR_BG_HAKA_MEGANEBG,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKA_OBJECTS,
    /**/ sizeof(BgHakaMeganeBG),
    /**/ Bg_Haka_MeganeBG_actor_ct,
    /**/ Bg_Haka_MeganeBG_actor_dt,
    /**/ Bg_Haka_MeganeBG_actor_move,
    /**/ Bg_Haka_MeganeBG_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

// Unused
static u32 zero_vec[] = {
    0x00000000, 0x00000000, 0x00000000, 0xC8C800FF, 0xFF0000FF,
};

static Gfx* shape_model[] = {
    object_haka_objects_DL_008EB0,
    object_haka_objects_DL_00A1A0,
    object_haka_objects_DL_005000,
    object_haka_objects_DL_000040,
};

void Bg_Haka_MeganeBG_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakaMeganeBG* this = (BgHakaMeganeBG*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    this->unk_168 = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params &= 0xFF;

    if (thisx->params == 2) {
        MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
        thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_005334, &colHeader);
        this->actionFunc = mode_rtubo_rotate;
    } else {
        MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);

        if (thisx->params == 0) {
            DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_009168, &colHeader);
            thisx->flags |= ACTOR_FLAG_REACT_TO_LENS;
            this->unk_16A = 20;
            this->actionFunc = mode_slide_stop;
        } else if (thisx->params == 3) {
            DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_000118, &colHeader);
            thisx->home.pos.y += 100.0f;

            if (Actor_Environment_sw_Check(play, this->unk_168)) {
                this->actionFunc = mode_coinshutter_stop;
                thisx->world.pos.y = thisx->home.pos.y;
            } else {
                thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
                this->actionFunc = mode_coinshutter_wait;
            }
        } else {
            DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_00A7F4, &colHeader);
            this->unk_16A = 80;
            this->actionFunc = mode_lift_drop;
            thisx->cullingVolumeScale = 3000.0f;
            thisx->cullingVolumeDownward = 3000.0f;
        }
    }

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void Bg_Haka_MeganeBG_actor_dt(Actor* thisx, PlayState* play) {
    BgHakaMeganeBG* this = (BgHakaMeganeBG*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void mode_slide_stop(BgHakaMeganeBG* this, PlayState* play) {
    if (this->unk_16A != 0) {
        this->unk_16A--;
    }

    if (this->unk_16A == 0) {
        this->unk_16A = 40;
        this->dyna.actor.world.rot.y += 0x8000;
        this->actionFunc = mode_slide_move;
    }
}

void mode_slide_move(BgHakaMeganeBG* this, PlayState* play) {
    f32 xSub;

    if (this->unk_16A != 0) {
        this->unk_16A--;
    }

    xSub = (sinf(((this->unk_16A * 0.025f) + 0.5f) * M_PI) + 1.0f) * 160.0f;

    if (this->dyna.actor.world.rot.y != this->dyna.actor.shape.rot.y) {
        xSub = 320.0f - xSub;
    }

    this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x - xSub;

    if (this->unk_16A == 0) {
        this->unk_16A = 20;
        this->actionFunc = mode_slide_stop;
    }
}

static void mode_lift_drop(BgHakaMeganeBG* this, PlayState* play) {
    this->dyna.actor.velocity.y += 1.0f;
    this->dyna.actor.velocity.y = CLAMP_MAX(this->dyna.actor.velocity.y, 20.0f);

    if (this->unk_16A != 0) {
        this->unk_16A--;
    }

    if (!chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 640.0f,
                      this->dyna.actor.velocity.y)) {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_CHINETRAP_DOWN - SFX_FLAG);
    }

    if (this->unk_16A == 0) {
        this->unk_16A = 120;
        this->actionFunc = mode_lift_up;
        this->dyna.actor.velocity.y = 0.0f;
    }
}

void mode_lift_up(BgHakaMeganeBG* this, PlayState* play) {
    chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 16.0f / 3.0f);
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_BRIDGE_CLOSE - SFX_FLAG);

    if (this->unk_16A != 0) {
        this->unk_16A--;
    }

    if (this->unk_16A == 0) {
        this->unk_16A = 80;
        this->actionFunc = mode_lift_drop;
    }
}

void mode_rtubo_rotate(BgHakaMeganeBG* this, PlayState* play) {
    this->dyna.actor.shape.rot.y += 0x180;
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
}

void mode_coinshutter_wait(BgHakaMeganeBG* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->unk_168)) {
        makeActorAttentionDemo(play, &this->dyna.actor);
        this->actionFunc = mode_coinshutter_move;
    }
}

void mode_coinshutter_move(BgHakaMeganeBG* this, PlayState* play) {
    chase_f(&this->dyna.actor.speed, 30.0f, 2.0f);

    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, this->dyna.actor.speed)) {
        Actor_world_to_eye(&this->dyna.actor, 50.0f);
        this->actionFunc = mode_coinshutter_stop;
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_OPEN);
    }
}

void mode_coinshutter_stop(BgHakaMeganeBG* this, PlayState* play) {
}

void Bg_Haka_MeganeBG_actor_move(Actor* thisx, PlayState* play) {
    BgHakaMeganeBG* this = (BgHakaMeganeBG*)thisx;

    this->actionFunc(this, play);
}

void Bg_Haka_MeganeBG_actor_draw(Actor* thisx, PlayState* play) {
    BgHakaMeganeBG* this = (BgHakaMeganeBG*)thisx;
    s16 params = this->dyna.actor.params;

    if (params == 0) {
        Cheap_gfx_display_xlu(play, object_haka_objects_DL_008EB0);
    } else {
        Cheap_gfx_display(play, shape_model[params]);
    }
}
