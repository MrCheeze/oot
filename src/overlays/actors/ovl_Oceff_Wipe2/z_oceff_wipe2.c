/*
 * File: z_oceff_wipe2.c
 * Overlay: ovl_Oceff_Wipe2
 * Description: Epona's Song Effect
 */

#include "z_oceff_wipe2.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void Oceff_Wipe2_Actor_ct(Actor* thisx, PlayState* play);
void Oceff_Wipe2_Actor_dt(Actor* thisx, PlayState* play);
void Oceff_Wipe2_Actor_move(Actor* thisx, PlayState* play);
void Oceff_Wipe2_Actor_draw(Actor* thisx, PlayState* play);

ActorProfile Oceff_Wipe2_Profile = {
    /**/ ACTOR_OCEFF_WIPE2,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(OceffWipe2),
    /**/ Oceff_Wipe2_Actor_ct,
    /**/ Oceff_Wipe2_Actor_dt,
    /**/ Oceff_Wipe2_Actor_move,
    /**/ Oceff_Wipe2_Actor_draw,
};

void Oceff_Wipe2_Actor_ct(Actor* thisx, PlayState* play) {
    OceffWipe2* this = (OceffWipe2*)thisx;

    Actor_set_scale(&this->actor, 0.1f);
    this->timer = 0;
    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    PRINTF(VT_FGCOL(CYAN) " WIPE2 arg_data = %d\n" VT_RST, this->actor.params);
}

void Oceff_Wipe2_Actor_dt(Actor* thisx, PlayState* play) {
    OceffWipe2* this = (OceffWipe2*)thisx;
    Player* player = GET_PLAYER(play);

    magic_cancel_check(play);
    if (z_common_data.nayrusLoveTimer != 0) {
        player->stateFlags3 |= PLAYER_STATE3_RESTORE_NAYRUS_LOVE;
    }
}

void Oceff_Wipe2_Actor_move(Actor* thisx, PlayState* play) {
    OceffWipe2* this = (OceffWipe2*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->timer < 100) {
        this->timer++;
    } else {
        Actor_delete(&this->actor);
    }
}

#include "assets/overlays/ovl_Oceff_Wipe2/z_oceff_wipe2.c"

void Oceff_Wipe2_Actor_draw(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFF;
    OceffWipe2* this = (OceffWipe2*)thisx;
    f32 z;
    u8 alpha;
    s32 pad[2];
    Vec3f eye;
    Vtx* vtxPtr;
    Vec3f quakeOffset;

    eye = GET_ACTIVE_CAM(play)->eye;
    quakeOffset = getCameraGap(GET_ACTIVE_CAM(play));
    if (this->timer < 32) {
        z = sin_s(this->timer << 9) * 1330;
    } else {
        z = 1330;
    }

    vtxPtr = efc_ocarina_41_v;
    if (this->timer >= 80) {
        alpha = 12 * (100 - this->timer);
    } else {
        alpha = 255;
    }

    vtxPtr[1].v.cn[3] = vtxPtr[3].v.cn[3] = vtxPtr[5].v.cn[3] = vtxPtr[7].v.cn[3] = vtxPtr[9].v.cn[3] =
        vtxPtr[11].v.cn[3] = vtxPtr[13].v.cn[3] = vtxPtr[15].v.cn[3] = vtxPtr[16].v.cn[3] = vtxPtr[18].v.cn[3] =
            vtxPtr[20].v.cn[3] = alpha;

    OPEN_DISPS(play->state.gfxCtx, "../z_oceff_wipe2.c", 390);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    Matrix_translate(eye.x + quakeOffset.x, eye.y + quakeOffset.y, eye.z + quakeOffset.z, MTXMODE_NEW);
    Matrix_scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Matrix_rotate_scale_exchange(&play->billboardMtxF);
    Matrix_translate(0.0f, 0.0f, -z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_oceff_wipe2.c", 400);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 100, 0, 128);
    gSPDisplayList(POLY_XLU_DISP++, efc_ocarina_41_modelT);
    gSPDisplayList(POLY_XLU_DISP++, two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, scroll * 6, scroll * (-6), 64,
                                                     64, 1, scroll * (-6), 0, 64, 64));
    gSPDisplayList(POLY_XLU_DISP++, efc_ocarina_41_modelT2);

    CLOSE_DISPS(play->state.gfxCtx, "../z_oceff_wipe2.c", 417);
}
