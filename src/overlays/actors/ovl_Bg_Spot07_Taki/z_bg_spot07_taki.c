/*
 * File: z_bg_spot07_taki.c
 * Overlay: ovl_Bg_Spot07_Taki
 * Description: Zora's Domain Waterfall and Ice
 */

#include "z_bg_spot07_taki.h"
#include "versions.h"
#include "assets/objects/object_spot07_object/object_spot07_object.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Spot07_Taki_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot07_Taki_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot07_Taki_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot07_Taki_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgSpot07Taki* this, PlayState* play);

ActorProfile Bg_Spot07_Taki_Profile = {
    /**/ ACTOR_BG_SPOT07_TAKI,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT07_OBJECT,
    /**/ sizeof(BgSpot07Taki),
    /**/ Bg_Spot07_Taki_actor_ct,
    /**/ Bg_Spot07_Taki_actor_dt,
    /**/ Bg_Spot07_Taki_actor_move,
    /**/ Bg_Spot07_Taki_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Spot07_Taki_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot07Taki* this = (BgSpot07Taki*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    ValueSet_process(&this->dyna.actor, value_init);
    if (LINK_IS_ADULT) {
        if (this->dyna.actor.params == 0) {
            DynaPolyUty_bgdi_SG2KSG(&object_spot07_object_Col_002590, &colHeader);
        } else {
            DynaPolyUty_bgdi_SG2KSG(&object_spot07_object_Col_0038FC, &colHeader);
        }
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    }
    this->actionFunc = mode_wait;
}

void Bg_Spot07_Taki_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot07Taki* this = (BgSpot07Taki*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_wait(BgSpot07Taki* this, PlayState* play) {
}

void Bg_Spot07_Taki_actor_move(Actor* thisx, PlayState* play) {
    BgSpot07Taki* this = (BgSpot07Taki*)thisx;

    this->actionFunc(this, play);
}

void Bg_Spot07_Taki_actor_draw(Actor* thisx, PlayState* play) {
    BgSpot07Taki* this = (BgSpot07Taki*)thisx;
    u32 frames;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot07_taki.c", 169);
    frames = play->gameplayFrames;
    if (LINK_IS_ADULT) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_spot07_taki.c", 177);
        if (this->dyna.actor.params == 0) {
            gSPDisplayList(POLY_OPA_DISP++, object_spot07_object_DL_001CF0);
        } else {
            gSPDisplayList(POLY_OPA_DISP++, object_spot07_object_DL_003210);
        }
    }
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot07_taki.c", 191);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, ((frames * -1) & 0x7F), ((frames * 1) & 0x7F), 32,
                                32, 1, ((frames * 1) & 0x7F), ((frames * 1) & 0x7F), 32, 32));

    if (!LINK_IS_ADULT) {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);
        if (this->dyna.actor.params == 0) {
#if !OOT_PAL_N64
            //! @bug 64x64 texture is scrolled mod 128 instead of mod 256 (i.e. 64 << G_TEXTURE_IMAGE_FRAC),
            //  so there is a noticeable jump when the scrolling wraps around
            gSPSegment(POLY_XLU_DISP++, 0x09,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, ((frames * -1) & 0x7F),
                                        ((frames * -3) & 0xFF), 64, 64, 1, ((frames * 1) & 0x7F),
                                        ((frames * -3) & 0xFF), 64, 64));
#else
            gSPSegment(POLY_XLU_DISP++, 0x09,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, ((frames * -1) & 0xFF),
                                        ((frames * -3) & 0xFF), 64, 64, 1, ((frames * 1) & 0xFF),
                                        ((frames * -3) & 0xFF), 64, 64));
#endif
            gSPSegment(POLY_XLU_DISP++, 0x0A,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, frames * 0, ((frames * 3) & 0x1FF), 32,
                                        128, 1, frames * 0, ((frames * 3) & 0x1FF), 32, 128));
            gSPDisplayList(POLY_XLU_DISP++, object_spot07_object_DL_000460);
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x09,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, frames * 0, ((frames * -1) & 0x7F), 32, 32,
                                        1, frames * 0, ((frames * -1) & 0x7F), 32, 32));
            gSPSegment(POLY_XLU_DISP++, 0x0A,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, frames * 0, ((frames * 3) & 0x1FF), 32,
                                        128, 1, frames * 0, ((frames * 3) & 0x1FF), 32, 128));
            gSPDisplayList(POLY_XLU_DISP++, object_spot07_object_DL_000BE0);
        }
    } else if (this->dyna.actor.params == 0) {
        gSPDisplayList(POLY_XLU_DISP++, object_spot07_object_DL_001F68);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, object_spot07_object_DL_0032D8);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot07_taki.c", 272);
}
