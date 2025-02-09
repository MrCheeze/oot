/*
 * File: z_oceff_wipe4.c
 * Overlay: ovl_Oceff_Wipe4
 * Description: Scarecrow's Song and an unused Ocarina Effect
 */

#include "z_oceff_wipe4.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void Oceff_Wipe4_Actor_ct(Actor* thisx, PlayState* play);
void Oceff_Wipe4_Actor_dt(Actor* thisx, PlayState* play);
void Oceff_Wipe4_Actor_move(Actor* thisx, PlayState* play);
void Oceff_Wipe4_Actor_draw(Actor* thisx, PlayState* play);

ActorProfile Oceff_Wipe4_Profile = {
    /**/ ACTOR_OCEFF_WIPE4,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(OceffWipe4),
    /**/ Oceff_Wipe4_Actor_ct,
    /**/ Oceff_Wipe4_Actor_dt,
    /**/ Oceff_Wipe4_Actor_move,
    /**/ Oceff_Wipe4_Actor_draw,
};

void Oceff_Wipe4_Actor_ct(Actor* thisx, PlayState* play) {
    OceffWipe4* this = (OceffWipe4*)thisx;

    Actor_set_scale(&this->actor, 0.1f);
    this->timer = 0;
    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    PRINTF(VT_FGCOL(CYAN) " WIPE4 arg_data = %d\n" VT_RST, this->actor.params);
}

void Oceff_Wipe4_Actor_dt(Actor* thisx, PlayState* play) {
    OceffWipe4* this = (OceffWipe4*)thisx;

    magic_cancel_check(play);

    //! @bug `PLAYER_STATE3_RESTORE_NAYRUS_LOVE` is not set, unlike other Oceff actors.
    //! This means playing Scarecrow's Song interrupts Nayru's Love without restoring it later.
}

void Oceff_Wipe4_Actor_move(Actor* thisx, PlayState* play) {
    OceffWipe4* this = (OceffWipe4*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->timer < 50) {
        this->timer++;
    } else {
        Actor_delete(&this->actor);
    }
}

#include "assets/overlays/ovl_Oceff_Wipe4/z_oceff_wipe4.c"

void Oceff_Wipe4_Actor_draw(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFFF;
    OceffWipe4* this = (OceffWipe4*)thisx;
    f32 z;
    u8 alpha;
    s32 pad[2];
    Vec3f eye;
    Vtx* vtxPtr;
    Vec3f quakeOffset;

    eye = GET_ACTIVE_CAM(play)->eye;
    quakeOffset = getCameraGap(GET_ACTIVE_CAM(play));
    if (this->timer < 16) {
        z = sin_s(this->timer * 1024) * 1330.0f;
    } else {
        z = 1330.0f;
    }

    vtxPtr = efc_ocarina_5_v;
    if (this->timer >= 30) {
        alpha = 12 * (50 - this->timer);
    } else {
        alpha = 255;
    }

    vtxPtr[1].v.cn[3] = vtxPtr[3].v.cn[3] = vtxPtr[5].v.cn[3] = vtxPtr[7].v.cn[3] = vtxPtr[9].v.cn[3] =
        vtxPtr[11].v.cn[3] = vtxPtr[13].v.cn[3] = vtxPtr[15].v.cn[3] = vtxPtr[17].v.cn[3] = vtxPtr[19].v.cn[3] =
            vtxPtr[21].v.cn[3] = alpha;

    OPEN_DISPS(play->state.gfxCtx, "../z_oceff_wipe4.c", 314);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    Matrix_translate(eye.x + quakeOffset.x, eye.y + quakeOffset.y, eye.z + quakeOffset.z, MTXMODE_NEW);
    Matrix_scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Matrix_rotate_scale_exchange(&play->billboardMtxF);
    Matrix_translate(0.0f, 0.0f, -z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_oceff_wipe4.c", 324);

    if (this->actor.params == OCEFF_WIPE4_UNUSED) {
        gSPDisplayList(POLY_XLU_DISP++, efc_ocarina_5_modelT20);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, efc_ocarina_5_modelT21);
    }

    gSPDisplayList(POLY_XLU_DISP++, efc_ocarina_5_modelT);
    gSPDisplayList(POLY_XLU_DISP++, two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, scroll * 2, scroll * (-2), 32,
                                                     64, 1, scroll * (-1), scroll, 32, 32));
    gSPDisplayList(POLY_XLU_DISP++, &efc_ocarina_5_modelT[11]);

    CLOSE_DISPS(play->state.gfxCtx, "../z_oceff_wipe4.c", 344);
}
