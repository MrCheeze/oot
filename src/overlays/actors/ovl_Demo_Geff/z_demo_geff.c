/*
 * File: z_demo_geff.c
 * Overlay: Demo_Geff
 * Description: Ganon's Lair Rubble Fragment
 */

#include "z_demo_geff.h"
#include "assets/objects/object_geff/object_geff.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Demo_Geff_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Geff_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Geff_main(Actor* thisx, PlayState* play);
void Demo_Geff_draw(Actor* thisx, PlayState* play);

void Demo_Geff_main_init_Hahen(DemoGeff* this, PlayState* play);

void Demo_Geff_main_bank(DemoGeff* this, PlayState* play);
void Demo_Geff_main_Hahen_Wait(DemoGeff* this, PlayState* play);

void Demo_Geff_draw_none(DemoGeff* this, PlayState* play);
void Demo_Geff_draw_Hahen(DemoGeff* this, PlayState* play);

static s16 Demo_Geff_Shape_Bank[] = {
    OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF,
};

void Demo_Geff_Actor_dt(Actor* thisx, PlayState* play) {
}

void Demo_Geff_Actor_ct(Actor* thisx, PlayState* play) {
    DemoGeff* this = (DemoGeff*)thisx;

    if (this->actor.params < 0 || this->actor.params >= 9) {
        PRINTF(VT_FGCOL(RED) "Demo_Geff_Actor_ct:arg_dataがおかしい!!!!!!!!!!!!\n" VT_RST);
        Actor_delete(&this->actor);
        return;
    }
    this->action = 0;
    this->drawConfig = 0;
}

void Demo_Geff_draw_normal_1(PlayState* play, Gfx* dlist) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_demo_geff.c", 181);

    _texture_z_light_fog_prim(gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_FINALIZE(gfxCtx, "../z_demo_geff.c", 183),
              G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, dlist);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_geff.c", 188);
}

void Demo_Geff_Change_ShapeBank(DemoGeff* this, PlayState* play) {
    s32 pad[2];
    s32 objectSlot = this->objectSlot;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_demo_geff.c", 204);

    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[objectSlot].segment);
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);

    // Necessary to match
    if (!play) {}

    CLOSE_DISPS(gfxCtx, "../z_demo_geff.c", 212);
}

#include "z_demo_geff_inHahen.inc.c"

void Demo_Geff_main_init(DemoGeff* this, PlayState* play) {
    static DemoGeffInitFunc proc[] = {
        Demo_Geff_main_init_Hahen, Demo_Geff_main_init_Hahen, Demo_Geff_main_init_Hahen, Demo_Geff_main_init_Hahen, Demo_Geff_main_init_Hahen,
        Demo_Geff_main_init_Hahen, Demo_Geff_main_init_Hahen, Demo_Geff_main_init_Hahen, Demo_Geff_main_init_Hahen,
    };

    s16 params = this->actor.params;
    DemoGeffInitFunc initFunc = proc[params];
    if (initFunc == NULL) {
        PRINTF(VT_FGCOL(RED) " Demo_Geff_main_init:初期化処理がおかしいarg_data = %d!\n" VT_RST, params);
        Actor_delete(&this->actor);
        return;
    }
    initFunc(this, play);
}

void Demo_Geff_main_bank(DemoGeff* this, PlayState* play) {
    ObjectContext* objCtx = &play->objectCtx;
    Actor* thisx = &this->actor;
    s32 params = thisx->params;
    s16 objectId = Demo_Geff_Shape_Bank[params];
    s32 objectSlot = Object_Exchange_bank_check(objCtx, objectId);
    s32 pad;

    if (objectSlot < 0) {
        PRINTF(VT_FGCOL(RED) "Demo_Geff_main_bank:バンクを読めない arg_data = %d!\n" VT_RST, params);
        Actor_delete(thisx);
        return;
    }
    if (Object_Exchange_bank_dma_check(objCtx, objectSlot)) {
        this->objectSlot = objectSlot;
        Demo_Geff_main_init(this, play);
    }
}

void Demo_Geff_main(Actor* thisx, PlayState* play) {
    static DemoGeffActionFunc proc[] = {
        Demo_Geff_main_bank,
        Demo_Geff_main_Hahen_Wait,
    };

    DemoGeff* this = (DemoGeff*)thisx;

    if (this->action < 0 || this->action >= 2 || proc[this->action] == NULL) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->action](this, play);
}

void Demo_Geff_draw_none(DemoGeff* this, PlayState* play) {
}

void Demo_Geff_draw(Actor* thisx, PlayState* play) {
    static DemoGeffDrawFunc proc[] = {
        Demo_Geff_draw_none,
        Demo_Geff_draw_Hahen,
    };

    DemoGeff* this = (DemoGeff*)thisx;
    s32 drawConfig = this->drawConfig;

    if (drawConfig < 0 || drawConfig >= 2 || proc[drawConfig] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    if (drawConfig != 0) {
        Demo_Geff_Change_ShapeBank(this, play);
    }
    proc[drawConfig](this, play);
}

ActorProfile Demo_Geff_Profile = {
    /**/ ACTOR_DEMO_GEFF,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_GEFF,
    /**/ sizeof(DemoGeff),
    /**/ Demo_Geff_Actor_ct,
    /**/ Demo_Geff_Actor_dt,
    /**/ Demo_Geff_main,
    /**/ Demo_Geff_draw,
};
