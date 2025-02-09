#include "global.h"

void Eff_Set_Fog2(PlayState* play, Color_RGBA8* color, s16 arg2, s16 arg3) {
    f32 cos;
    Gfx* displayListHead;
    f32 absCos;

    OPEN_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 113);

    displayListHead = POLY_OPA_DISP;
    cos = cos_s((0x8000 / arg3) * arg2);
    absCos = ABS(cos);

    gDPPipeSync(displayListHead++);

    if (color == NULL) {
        gDPSetFogColor(displayListHead++, 255, 0, 0, 0);
    } else {
        gDPSetFogColor(displayListHead++, color->r, color->g, color->b, color->a);
    }

    gSPFogPosition(displayListHead++, 0, (s16)(absCos * 3000.0f) + 1500);

    POLY_OPA_DISP = displayListHead;

    CLOSE_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 129);
}

void Eff_Set_Fog3(PlayState* play, Color_RGBA8* color, s16 arg2, s16 arg3) {
    Gfx* displayListHead;
    f32 cos;

#if PLATFORM_GC && OOT_VERSION != HIRATSU3
    if (arg3 == 0) {
        return;
    }
#endif

    OPEN_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 141);

    cos = cos_s((0x4000 / arg3) * arg2);
    displayListHead = POLY_OPA_DISP;

    gDPPipeSync(displayListHead++);
    gDPSetFogColor(displayListHead++, color->r, color->g, color->b, color->a);
    gSPFogPosition(displayListHead++, 0, (s16)(2800.0f * ABS(cos)) + 1700);

    POLY_OPA_DISP = displayListHead;

    CLOSE_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 153);
}

void Eff_Off_Fog(PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 159);

    gDPPipeSync(POLY_OPA_DISP++);
    POLY_OPA_DISP = game_play_set_fog(play, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 164);
}

void Eff_Set_Fog2_xlu(PlayState* play, Color_RGBA8* color, s16 arg2, s16 arg3) {
    f32 cos;
    Gfx* displayListHead;
    f32 absCos;

    OPEN_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 178);

    displayListHead = POLY_XLU_DISP;
    cos = cos_s((0x8000 / arg3) * arg2);
    absCos = ABS(cos);

    gDPPipeSync(displayListHead++);

    if (color == NULL) {
        gDPSetFogColor(displayListHead++, 255, 0, 0, 0);
    } else {
        gDPSetFogColor(displayListHead++, color->r, color->g, color->b, color->a);
    }

    gSPFogPosition(displayListHead++, 0, (s16)(absCos * 3000.0f) + 1500);

    POLY_XLU_DISP = displayListHead;

    CLOSE_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 194);
}

void Eff_Set_Fog3_xlu(PlayState* play, Color_RGBA8* color, s16 arg2, s16 arg3) {
    f32 cos;
    Gfx* displayListHead;

    OPEN_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 201);

    displayListHead = POLY_XLU_DISP;
    cos = cos_s((0x4000 / arg3) * arg2);

    gDPPipeSync(displayListHead++);
    gDPSetFogColor(displayListHead++, color->r, color->g, color->b, color->a);
    gSPFogPosition(displayListHead++, 0, (s16)(2800.0f * ABS(cos)) + 1700);

    POLY_XLU_DISP = displayListHead;

    CLOSE_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 212);
}

void Eff_Off_Fog_xlu(PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 217);

    gDPPipeSync(POLY_XLU_DISP++);
    POLY_XLU_DISP = game_play_set_fog(play, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_eff_ss_dead.c", 222);
}
