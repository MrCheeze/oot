/*
 * File: z_demo_geff.c
 * Overlay: Demo_Geff
 * Description: Ganon's Lair Rubble Fragment
 */

#include "z_demo_geff.h"
#include "assets/objects/object_geff/object_geff.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DemoGeff_Init(Actor* thisx, PlayState* play);
void DemoGeff_Destroy(Actor* thisx, PlayState* play);
void DemoGeff_Update(Actor* thisx, PlayState* play);
void DemoGeff_Draw(Actor* thisx, PlayState* play);

void func_80978030(DemoGeff* this, PlayState* play);

void func_809783D4(DemoGeff* this, PlayState* play);
void func_80978308(DemoGeff* this, PlayState* play);

void func_809784D4(DemoGeff* this, PlayState* play);
void func_80978344(DemoGeff* this, PlayState* play);

static s16 sObjectIds[] = {
    OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF, OBJECT_GEFF,
};

static DemoGeffInitFunc sInitFuncs[] = {
    func_80978030, func_80978030, func_80978030, func_80978030, func_80978030,
    func_80978030, func_80978030, func_80978030, func_80978030,
};

static DemoGeffActionFunc sActionFuncs[] = {
    func_809783D4,
    func_80978308,
};

static DemoGeffDrawFunc sDrawFuncs[] = {
    func_809784D4,
    func_80978344,
};

ActorProfile Demo_Geff_Profile = {
    /**/ ACTOR_DEMO_GEFF,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_GEFF,
    /**/ sizeof(DemoGeff),
    /**/ DemoGeff_Init,
    /**/ DemoGeff_Destroy,
    /**/ DemoGeff_Update,
    /**/ DemoGeff_Draw,
};

void DemoGeff_Destroy(Actor* thisx, PlayState* play) {
}

void DemoGeff_Init(Actor* thisx, PlayState* play) {
    DemoGeff* this = (DemoGeff*)thisx;

    if (this->actor.params < 0 || this->actor.params >= 9) {
        PRINTF(VT_FGCOL(RED) "Demo_Geff_Actor_ct:arg_dataがおかしい!!!!!!!!!!!!\n" VT_RST);
        Actor_Kill(&this->actor);
        return;
    }
    this->action = 0;
    this->drawConfig = 0;
}

void func_80977EA8(PlayState* play, Gfx* dlist) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_demo_geff.c", 181);

    Gfx_SetupDL_25Opa(gfxCtx);

    gSPMatrix(POLY_OPA_DISP++, MATRIX_FINALIZE(gfxCtx, "../z_demo_geff.c", 183),
              G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, dlist);
    gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_geff.c", 188);
}

void func_80977F80(DemoGeff* this, PlayState* play) {
    s32 pad[2];
    s32 objectSlot = this->objectSlot;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_demo_geff.c", 204);

    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[objectSlot].segment);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);

    // Necessary to match
    if (!play) {}

    CLOSE_DISPS(gfxCtx, "../z_demo_geff.c", 212);
}

#include "z_demo_geff_inHahen.inc.c"

void func_80978370(DemoGeff* this, PlayState* play) {
    s16 params = this->actor.params;
    DemoGeffInitFunc initFunc = sInitFuncs[params];
    if (initFunc == NULL) {
        PRINTF(VT_FGCOL(RED) " Demo_Geff_main_init:初期化処理がおかしいarg_data = %d!\n" VT_RST, params);
        Actor_Kill(&this->actor);
        return;
    }
    initFunc(this, play);
}

void func_809783D4(DemoGeff* this, PlayState* play) {
    ObjectContext* objCtx = &play->objectCtx;
    Actor* thisx = &this->actor;
    s32 params = thisx->params;
    s16 objectId = sObjectIds[params];
    s32 objectSlot = Object_GetSlot(objCtx, objectId);
    s32 pad;

    if (objectSlot < 0) {
        PRINTF(VT_FGCOL(RED) "Demo_Geff_main_bank:バンクを読めない arg_data = %d!\n" VT_RST, params);
        Actor_Kill(thisx);
        return;
    }
    if (Object_IsLoaded(objCtx, objectSlot)) {
        this->objectSlot = objectSlot;
        func_80978370(this, play);
    }
}

void DemoGeff_Update(Actor* thisx, PlayState* play) {
    DemoGeff* this = (DemoGeff*)thisx;

    if (this->action < 0 || this->action >= 2 || sActionFuncs[this->action] == NULL) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, play);
}

void func_809784D4(DemoGeff* this, PlayState* play) {
}

void DemoGeff_Draw(Actor* thisx, PlayState* play) {
    DemoGeff* this = (DemoGeff*)thisx;
    s32 drawConfig = this->drawConfig;

    if (drawConfig < 0 || drawConfig >= 2 || sDrawFuncs[drawConfig] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    if (drawConfig != 0) {
        func_80977F80(this, play);
    }
    sDrawFuncs[drawConfig](this, play);
}
