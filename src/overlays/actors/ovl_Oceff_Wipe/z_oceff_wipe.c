/*
 * File: z_oceff_wipe.c
 * Overlay: ovl_Oceff_Wipe
 * Description: Zelda's Lullaby and Song of Time Ocarina Effect
 */

#include "z_oceff_wipe.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void Oceff_Wipe_Actor_ct(Actor* thisx, PlayState* play);
void Oceff_Wipe_Actor_dt(Actor* thisx, PlayState* play);
void Oceff_Wipe_Actor_move(Actor* thisx, PlayState* play);
void Oceff_Wipe_Actor_draw(Actor* thisx, PlayState* play);

ActorProfile Oceff_Wipe_Profile = {
    /**/ ACTOR_OCEFF_WIPE,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(OceffWipe),
    /**/ Oceff_Wipe_Actor_ct,
    /**/ Oceff_Wipe_Actor_dt,
    /**/ Oceff_Wipe_Actor_move,
    /**/ Oceff_Wipe_Actor_draw,
};

void Oceff_Wipe_Actor_ct(Actor* thisx, PlayState* play) {
    OceffWipe* this = (OceffWipe*)thisx;

    Actor_set_scale(&this->actor, 0.1f);
    this->timer = 0;
    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    PRINTF(VT_FGCOL(CYAN) " WIPE arg_data = %d\n" VT_RST, this->actor.params);
}

void Oceff_Wipe_Actor_dt(Actor* thisx, PlayState* play) {
    OceffWipe* this = (OceffWipe*)thisx;
    Player* player = GET_PLAYER(play);

    magic_cancel_check(play);
    if (z_common_data.nayrusLoveTimer != 0) {
        player->stateFlags3 |= PLAYER_STATE3_RESTORE_NAYRUS_LOVE;
    }
}

void Oceff_Wipe_Actor_move(Actor* thisx, PlayState* play) {
    OceffWipe* this = (OceffWipe*)thisx;

    this->actor.world.pos = GET_ACTIVE_CAM(play)->eye;
    if (this->timer < 100) {
        this->timer++;
    } else {
        Actor_delete(&this->actor);
    }
}

#include "assets/overlays/ovl_Oceff_Wipe/z_oceff_wipe.c"

static u8 tbl[] = {
    0x01, 0x10, 0x22, 0x01, 0x20, 0x12, 0x01, 0x20, 0x12, 0x01,
    0x10, 0x22, 0x01, 0x20, 0x12, 0x01, 0x12, 0x21, 0x01, 0x02,
};

void Oceff_Wipe_Actor_draw(Actor* thisx, PlayState* play) {
    u32 scroll = play->state.frames & 0xFF;
    OceffWipe* this = (OceffWipe*)thisx;
    f32 z;
    s32 pad;
    u8 alphaTable[3];
    s32 i;
    Vec3f eye;
    Vtx* vtxPtr;
    Vec3f quakeOffset;

    eye = GET_ACTIVE_CAM(play)->eye;
    quakeOffset = getCameraGap(GET_ACTIVE_CAM(play));

    OPEN_DISPS(play->state.gfxCtx, "../z_oceff_wipe.c", 346);

    if (this->timer < 32) {
        z = sin_s(this->timer << 9) * 1400;
    } else {
        z = 1400;
    }

    if (this->timer >= 80) {
        alphaTable[0] = 0;
        alphaTable[1] = (0x64 - this->timer) * 8;
        alphaTable[2] = (0x64 - this->timer) * 12;
    } else {
        alphaTable[0] = 0;
        alphaTable[1] = 0xA0;
        alphaTable[2] = 0xFF;
    }

    for (i = 0; i < 20; i++) {
        vtxPtr = efc_ocarina_3_v;
        vtxPtr[i * 2 + 0].v.cn[3] = alphaTable[(tbl[i] & 0xF0) >> 4];
        vtxPtr[i * 2 + 1].v.cn[3] = alphaTable[tbl[i] & 0xF];
    }

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    Matrix_translate(eye.x + quakeOffset.x, eye.y + quakeOffset.y, eye.z + quakeOffset.z, MTXMODE_NEW);
    Matrix_scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Matrix_rotate_scale_exchange(&play->billboardMtxF);
    Matrix_translate(0.0f, 0.0f, -z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_oceff_wipe.c", 375);

    if (this->actor.params != OCEFF_WIPE_ZL) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 150, 255, 128);
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 100, 0, 255, 128);
    }

    gSPDisplayList(POLY_XLU_DISP++, efc_ocarina_3_modelT);
    gSPDisplayList(POLY_XLU_DISP++, two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0 - scroll, scroll * (-2), 32,
                                                     32, 1, 0 - scroll, scroll * (-2), 32, 32));
    gSPDisplayList(POLY_XLU_DISP++, efc_ocarina_3_modelT2);

    CLOSE_DISPS(play->state.gfxCtx, "../z_oceff_wipe.c", 398);
}
