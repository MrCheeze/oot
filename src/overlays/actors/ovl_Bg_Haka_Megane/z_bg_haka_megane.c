/*
 * File: z_bg_haka_megane.c
 * Overlay: ovl_Bg_Haka_Megane
 * Description: Shadow Temple Fake Walls
 */

#include "z_bg_haka_megane.h"

#include "ichain.h"
#include "z64play.h"

#include "z64.h"

#include "assets/objects/object_hakach_objects/object_hakach_objects.h"
#include "assets/objects/object_haka_objects/object_haka_objects.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_REACT_TO_LENS)

void Bg_Haka_Megane_actor_ct(Actor* thisx, PlayState* play);
void Bg_Haka_Megane_actor_dt(Actor* thisx, PlayState* play);
void Bg_Haka_Megane_actor_move(Actor* thisx, PlayState* play);
void Bg_Haka_Megane_actor_draw(Actor* thisx, PlayState* play);

static void mode_dma_wait(BgHakaMegane* this, PlayState* play);
static void mode_wait(BgHakaMegane* this, PlayState* play);
static void mode_stop(BgHakaMegane* this, PlayState* play);

ActorProfile Bg_Haka_Megane_Profile = {
    /**/ ACTOR_BG_HAKA_MEGANE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(BgHakaMegane),
    /**/ Bg_Haka_Megane_actor_ct,
    /**/ Bg_Haka_Megane_actor_dt,
    /**/ Bg_Haka_Megane_actor_move,
    /**/ NULL,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static CollisionHeader* bg_data[] = {
    &gBotw1Col,
    &gBotw2Col,
    NULL,
    &object_haka_objects_Col_004330,
    &object_haka_objects_Col_0044D0,
    NULL,
    &object_haka_objects_Col_004780,
    &object_haka_objects_Col_004940,
    NULL,
    &object_haka_objects_Col_004B00,
    NULL,
    &object_haka_objects_Col_004CC0,
    NULL,
};

static Gfx* shape_model[] = {
    gBotwFakeWallsAndFloorsDL,     gBotwThreeFakeFloorsDL,        gBotwHoleTrap2DL,
    object_haka_objects_DL_0040F0, object_haka_objects_DL_0043B0, object_haka_objects_DL_001120,
    object_haka_objects_DL_0045A0, object_haka_objects_DL_0047F0, object_haka_objects_DL_0018F0,
    object_haka_objects_DL_0049B0, object_haka_objects_DL_003CF0, object_haka_objects_DL_004B70,
    object_haka_objects_DL_002ED0,
};

void Bg_Haka_Megane_actor_ct(Actor* thisx, PlayState* play) {
    BgHakaMegane* this = (BgHakaMegane*)thisx;

    ValueSet_process(thisx, value_init);
    MoveBG_ct(&this->dyna, 0);

    if (thisx->params < 3) {
        this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_HAKACH_OBJECTS);
    } else {
        this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_HAKA_OBJECTS);
    }

    if (this->requiredObjectSlot < 0) {
        Actor_delete(thisx);
    } else {
        this->actionFunc = mode_dma_wait;
    }
}

void Bg_Haka_Megane_actor_dt(Actor* thisx, PlayState* play) {
    BgHakaMegane* this = (BgHakaMegane*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_dma_wait(BgHakaMegane* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->dyna.actor.objectSlot = this->requiredObjectSlot;
        this->dyna.actor.draw = Bg_Haka_Megane_actor_draw;
        Actor_set_segment(play, &this->dyna.actor);
        if (play->roomCtx.curRoom.lensMode != LENS_MODE_SHOW_ACTORS) {
            CollisionHeader* colHeader;
            CollisionHeader* collision;

            this->actionFunc = mode_wait;
            collision = bg_data[this->dyna.actor.params];
            if (collision != NULL) {
                DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
                this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            }
        } else {
            this->actionFunc = mode_stop;
        }
    }
}

static void mode_wait(BgHakaMegane* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (play->actorCtx.lensActive) {
        thisx->flags |= ACTOR_FLAG_REACT_TO_LENS;
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    } else {
        thisx->flags &= ~ACTOR_FLAG_REACT_TO_LENS;
        DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

static void mode_stop(BgHakaMegane* this, PlayState* play) {
}

void Bg_Haka_Megane_actor_move(Actor* thisx, PlayState* play) {
    BgHakaMegane* this = (BgHakaMegane*)thisx;

    this->actionFunc(this, play);
}

void Bg_Haka_Megane_actor_draw(Actor* thisx, PlayState* play) {
    BgHakaMegane* this = (BgHakaMegane*)thisx;

    if (CHECK_FLAG_ALL(thisx->flags, ACTOR_FLAG_REACT_TO_LENS)) {
        Cheap_gfx_display_xlu(play, shape_model[thisx->params]);
    } else {
        Cheap_gfx_display(play, shape_model[thisx->params]);
    }

    if (thisx->params == 0) {
        Cheap_gfx_display_xlu(play, gBotwBloodSplatterDL);
    }
}
