/*
 * File: z_en_ganon_organ.c
 * Overlay: ovl_En_Ganon_Organ
 * Description: The organ that Ganondorf plays in the cutscene before the fight. Includes carpet and scenery as well.
 */

#include "z_en_ganon_organ.h"
#include "overlays/actors/ovl_Boss_Ganon/z_boss_ganon.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Ganon_Organ_Actor_ct(Actor* thisx, PlayState* play);
void En_Ganon_Organ_Actor_dt(Actor* thisx, PlayState* play);
void En_Ganon_Organ_Actor_move(Actor* thisx, PlayState* play);
void En_Ganon_Organ_Actor_draw(Actor* thisx, PlayState* play);

ActorProfile En_Ganon_Organ_Profile = {
    /**/ ACTOR_EN_GANON_ORGAN,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_GANON,
    /**/ sizeof(EnGanonOrgan),
    /**/ En_Ganon_Organ_Actor_ct,
    /**/ En_Ganon_Organ_Actor_dt,
    /**/ En_Ganon_Organ_Actor_move,
    /**/ En_Ganon_Organ_Actor_draw,
};

static u64 dammy = 0;

#include "assets/overlays/ovl_En_Ganon_Organ/z_en_ganon_organ.c"

void En_Ganon_Organ_Actor_ct(Actor* thisx, PlayState* play) {
    thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void En_Ganon_Organ_Actor_dt(Actor* thisx, PlayState* play) {
}

void En_Ganon_Organ_Actor_move(Actor* thisx, PlayState* play) {
    BossGanon* dorf;

    PRINTF("ORGAN MOVE 1\n");
    if (thisx->params == 1) {
        dorf = (BossGanon*)thisx->parent;
        if (dorf->organAlpha == 0) {
            Actor_delete(thisx);
        }
    }
    PRINTF("ORGAN MOVE 2\n");
}

Gfx* organ_mode_normal(GraphicsContext* gfxCtx) {
    Gfx* displayList;

    displayList = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));
    gSPEndDisplayList(displayList);
    return displayList;
}

Gfx* organ_mode_1(GraphicsContext* gfxCtx, BossGanon* dorf) {
    Gfx* displayList;
    Gfx* displayListHead;

    displayList = GRAPH_ALLOC(gfxCtx, 4 * sizeof(Gfx));
    displayListHead = displayList;
    gDPPipeSync(displayListHead++);
    gDPSetEnvColor(displayListHead++, 25, 20, 0, dorf->organAlpha);
    gDPSetRenderMode(displayListHead++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
    gSPEndDisplayList(displayListHead++);
    return displayList;
}

Gfx* organ_mode_2(GraphicsContext* gfxCtx, BossGanon* dorf) {
    Gfx* displayList;
    Gfx* displayListHead;

    displayList = GRAPH_ALLOC(gfxCtx, 4 * sizeof(Gfx));
    displayListHead = displayList;
    gDPPipeSync(displayListHead++);
    gDPSetEnvColor(displayListHead++, 0, 0, 0, dorf->organAlpha);
    gDPSetRenderMode(displayListHead++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2);
    gSPEndDisplayList(displayListHead++);
    return displayList;
}

void En_Ganon_Organ_Actor_draw(Actor* thisx, PlayState* play) {
    BossGanon* dorf = (BossGanon*)thisx->parent;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ganon_organ.c", 205);

    PRINTF("ORGAN DRAW  1\n");
    _texture_z_light_fog_prim(play->state.gfxCtx);
    if ((thisx->params == 1) && (dorf->organAlpha != 255)) {
        gSPSegment(POLY_OPA_DISP++, 0x08, organ_mode_1(play->state.gfxCtx, dorf));
        gSPSegment(POLY_OPA_DISP++, 0x09, organ_mode_2(play->state.gfxCtx, dorf));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, organ_mode_normal(play->state.gfxCtx));
        gSPSegment(POLY_OPA_DISP++, 0x09, organ_mode_normal(play->state.gfxCtx));
    }
    Matrix_translate(0.0f, 0.0f, 0.0f, MTXMODE_NEW);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_ganon_organ.c", 221);

    gSPDisplayList(POLY_OPA_DISP++, gnr_organ_model);
    gSPDisplayList(POLY_OPA_DISP++, gnr_decolations_model);

    PRINTF("ORGAN DRAW  2\n");

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ganon_organ.c", 230);
}
