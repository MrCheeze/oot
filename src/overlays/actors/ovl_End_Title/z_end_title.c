/*
 * File: z_end_title.c
 * Overlay: ovl_End_Title
 * Description: "The End" message
 */

#include "z_end_title.h"
#include "versions.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void End_Title_Actor_ct(Actor* thisx, PlayState* play);
void End_Title_Actor_dt(Actor* thisx, PlayState* play);
void End_Title_Actor_move(Actor* thisx, PlayState* play);
void End_Title_Actor_draw(Actor* thisx, PlayState* play);
void End_Title_Actor_draw2(Actor* thisx, PlayState* play);

ActorProfile End_Title_Profile = {
    /**/ ACTOR_END_TITLE,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EndTitle),
    /**/ End_Title_Actor_ct,
    /**/ End_Title_Actor_dt,
    /**/ End_Title_Actor_move,
    /**/ End_Title_Actor_draw,
};

#include "assets/overlays/ovl_End_Title/z_end_title.c"

void End_Title_Actor_ct(Actor* thisx, PlayState* play) {
    EndTitle* this = (EndTitle*)thisx;

    this->endAlpha = this->tlozAlpha = this->ootAlpha = 0;
    if (this->actor.params == 1) {
        this->actor.draw = End_Title_Actor_draw2;
    }
}

void End_Title_Actor_dt(Actor* thisx, PlayState* play) {
}

void End_Title_Actor_move(Actor* thisx, PlayState* play) {
}

// Used in the castle courtyard
void End_Title_Actor_draw(Actor* thisx, PlayState* play) {
    PlayState* play2 = (PlayState*)play;
    EndTitle* this = (EndTitle*)thisx;
    s32 csCurFrame = play2->csCtx.curFrame;
    Player* player = GET_PLAYER(play2);

    OPEN_DISPS(play->state.gfxCtx, "../z_end_title.c", 403);

    // Draw the Triforce on Link's left hand
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Matrix_mult(&player->mf_9E0, MTXMODE_NEW);
    Matrix_translate(0.0f, 150.0f, 170.0f, MTXMODE_APPLY);
    Matrix_scale(0.13f, 0.13f, 0.13f, MTXMODE_APPLY);
    Matrix_rotateX(BINANG_TO_RAD(0xBB8), MTXMODE_APPLY);
    Matrix_rotateY(0.0f, MTXMODE_APPLY);
    Matrix_rotateZ(0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_end_title.c", 412);
    gSPDisplayList(POLY_XLU_DISP++, end_triforce_modelT0);

    CLOSE_DISPS(play->state.gfxCtx, "../z_end_title.c", 417);

    OPEN_DISPS(play->state.gfxCtx, "../z_end_title.c", 419);

    // Draw title cards on the screen
    if ((csCurFrame > FRAMERATE_CONST(890, 740)) && (this->endAlpha < 200)) {
        this->endAlpha += FRAMERATE_CONST(7, 8);
    }
    if ((csCurFrame > FRAMERATE_CONST(810, 675)) && (this->tlozAlpha < 200)) {
        this->tlozAlpha += FRAMERATE_CONST(15, 18);
    }
    if ((csCurFrame > FRAMERATE_CONST(850, 710)) && (this->ootAlpha < 200)) {
        this->ootAlpha += FRAMERATE_CONST(15, 18);
    }

    OVERLAY_DISP = gfx_xlu_rectangle_2c(OVERLAY_DISP);
    gDPSetTextureLUT(OVERLAY_DISP++, G_TT_NONE);
    gDPSetEnvColor(OVERLAY_DISP++, 255, 120, 30, 0);
    gDPSetRenderMode(OVERLAY_DISP++, G_RM_PASS, G_RM_XLU_SURF2);
    gSPClearGeometryMode(OVERLAY_DISP++,
                         G_TEXTURE_ENABLE | G_CULL_BACK | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);
    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0,
                      COMBINED, 0, 0, 0, COMBINED);
    gDPSetPrimColor(OVERLAY_DISP++, 0x00, 0x80, 0, 0, 0, this->endAlpha);
    gDPLoadTextureTile(OVERLAY_DISP++, h_end_theend_txt, G_IM_FMT_IA, G_IM_SIZ_8b, 80, 24, 0, 0, 80, 24, 0,
                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 0, 0, 0, 0);
    gSPTextureRectangle(OVERLAY_DISP++, 120 << 2, 90 << 2, 200 << 2, 113 << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0x00, 0x80, 0, 0, 0, this->tlozAlpha);
    gDPLoadTextureTile(OVERLAY_DISP++, h_end_title1_txt, G_IM_FMT_IA, G_IM_SIZ_8b, 120, 24, 0, 0, 120, 24, 0,
                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 0, 0, 0, 0);
    gSPTextureRectangle(OVERLAY_DISP++, 100 << 2, 160 << 2, 220 << 2, 183 << 2, G_TX_RENDERTILE, 0, 0, 1 << 10,
                        1 << 10);
    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0x00, 0x80, 0, 0, 0, this->ootAlpha);
    gDPLoadTextureTile(OVERLAY_DISP++, h_end_title2_txt, G_IM_FMT_IA, G_IM_SIZ_8b, 112, 16, 0, 0, 112, 16, 0,
                       G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 0, 0, 0, 0);
    gSPTextureRectangle(OVERLAY_DISP++, 104 << 2, 177 << 2, 216 << 2, 192 << 2, G_TX_RENDERTILE, 0, 0, 1 << 10,
                        1 << 10);

    CLOSE_DISPS(play->state.gfxCtx, "../z_end_title.c", 515);
}

// Used in the Temple of Time
void End_Title_Actor_draw2(Actor* thisx, PlayState* play) {
    EndTitle* this = (EndTitle*)thisx;
    s32 pad;
    s32 csCurFrame = play->csCtx.curFrame;

    if ((csCurFrame > FRAMERATE_CONST(1100, 950)) && (this->endAlpha < 255)) {
        this->endAlpha += 3;
    }

    OPEN_DISPS(play->state.gfxCtx, "../z_end_title.c", 594);

    OVERLAY_DISP = gfx_xlu_rectangle_2c(OVERLAY_DISP);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0x80, 0, 0, 0, this->endAlpha);
    gSPDisplayList(OVERLAY_DISP++, h_end_nintendo_model);

    CLOSE_DISPS(play->state.gfxCtx, "../z_end_title.c", 600);
}
