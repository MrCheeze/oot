/*
 * File: z_en_scene_change.c
 * Overlay: ovl_En_Scene_Change
 * Description: Unknown (Broken Actor)
 */

#include "z_en_scene_change.h"

#define FLAGS 0

void En_Scene_Change_actor_ct(Actor* thisx, PlayState* play);
void En_Scene_Change_actor_dt(Actor* thisx, PlayState* play);
void En_Scene_Change_actor_move(Actor* thisx, PlayState* play);
void En_Scene_Change_actor_draw(Actor* thisx, PlayState* play);

static void move_wait(EnSceneChange* this, PlayState* play);

ActorProfile En_Scene_Change_Profile = {
    /**/ ACTOR_EN_SCENE_CHANGE,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_JJ,
    /**/ sizeof(EnSceneChange),
    /**/ En_Scene_Change_actor_ct,
    /**/ En_Scene_Change_actor_dt,
    /**/ En_Scene_Change_actor_move,
    /**/ En_Scene_Change_actor_draw,
};

void En_Scene_Change_actor_set_process(EnSceneChange* this, EnSceneChangeActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Scene_Change_actor_ct(Actor* thisx, PlayState* play) {
    EnSceneChange* this = (EnSceneChange*)thisx;

    En_Scene_Change_actor_set_process(this, move_wait);
}

void En_Scene_Change_actor_dt(Actor* thisx, PlayState* play) {
}

static void move_wait(EnSceneChange* this, PlayState* play) {
}

void En_Scene_Change_actor_move(Actor* thisx, PlayState* play) {
    EnSceneChange* this = (EnSceneChange*)thisx;

    this->actionFunc(this, play);
}

void En_Scene_Change_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad[2];
    Gfx* displayList;
    s32 pad2[2];
    Gfx* displayListHead;

    displayList = GRAPH_ALLOC(play->state.gfxCtx, 0x3C0);

    OPEN_DISPS(play->state.gfxCtx, "../z_en_scene_change.c", 290);

    displayListHead = displayList;
    gSPSegment(POLY_OPA_DISP++, 0x0C, displayListHead);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_scene_change.c", 386);
}
