/*
 * File: z_bg_toki_hikari.c
 * Overlay: ovl_Toki_Hikari
 * Description: Temple of Time Windows
 */

#include "z_bg_toki_hikari.h"
#include "assets/objects/object_toki_objects/object_toki_objects.h"

#define FLAGS ACTOR_FLAG_DRAW_CULLING_DISABLED

void Bg_Toki_Hikari_actor_ct(Actor* thisx, PlayState* play);
void Bg_Toki_Hikari_actor_dt(Actor* thisx, PlayState* play);
void Bg_Toki_Hikari_actor_move(Actor* thisx, PlayState* play);
void Bg_Toki_Hikari_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgTokiHikari* this, PlayState* play);
void Toki_Hikari_draw(Actor* thisx, PlayState* play);
void mode_flash_wait(BgTokiHikari* this, PlayState* play);
void mode_flash_move(BgTokiHikari* this, PlayState* play);
void mode_flash_move2(BgTokiHikari* this, PlayState* play);
void Tri_flash_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Toki_Hikari_Profile = {
    /**/ ACTOR_BG_TOKI_HIKARI,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_TOKI_OBJECTS,
    /**/ sizeof(BgTokiHikari),
    /**/ Bg_Toki_Hikari_actor_ct,
    /**/ Bg_Toki_Hikari_actor_dt,
    /**/ Bg_Toki_Hikari_actor_move,
    /**/ Bg_Toki_Hikari_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void Bg_Toki_Hikari_actor_ct(Actor* thisx, PlayState* play) {
    BgTokiHikari* this = (BgTokiHikari*)thisx;

    switch (this->actor.params) {
        case 0:
            ValueSet_process(&this->actor, value_init);
            this->actionFunc = mode_wait;
            break;
        case 1:
            if (!GET_EVENTCHKINF(EVENTCHKINF_OPENED_DOOR_OF_TIME)) {
                this->actionFunc = mode_flash_wait;
                this->unk_14C = 0.0f;
            } else {
                Actor_delete(&this->actor);
            }
            break;
    }
}

void Bg_Toki_Hikari_actor_dt(Actor* thisx, PlayState* play) {
}

static void mode_wait(BgTokiHikari* this, PlayState* play) {
}

void Bg_Toki_Hikari_actor_move(Actor* thisx, PlayState* play) {
    BgTokiHikari* this = (BgTokiHikari*)thisx;

    this->actionFunc(this, play);
}

void Bg_Toki_Hikari_actor_draw(Actor* thisx, PlayState* play) {
    switch (thisx->params) {
        case 0:
            Toki_Hikari_draw(thisx, play);
            break;
        case 1:
            Tri_flash_draw(thisx, play);
            break;
    }
}

void Toki_Hikari_draw(Actor* thisx, PlayState* play) {
    PlayState* play2 = (PlayState*)play;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_toki_hikari.c", 246);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_toki_hikari.c", 252);

    if (LINK_IS_ADULT) {
        gSPDisplayList(POLY_OPA_DISP++, object_toki_objects_DL_008190);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, object_toki_objects_DL_007E20);
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 8, tex_scroll2(play->state.gfxCtx, 0, play2->gameplayFrames % 128, 64, 32));

        gSPSegment(POLY_XLU_DISP++, 9, tex_scroll2(play->state.gfxCtx, 0, play2->gameplayFrames % 128, 64, 32));

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_toki_hikari.c", 278);

        gSPDisplayList(POLY_XLU_DISP++, object_toki_objects_DL_007EE0);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_toki_hikari.c", 284);
}

void mode_flash_wait(BgTokiHikari* this, PlayState* play) {
    if (play->roomCtx.drawParams[1] != 0) {
        this->actionFunc = mode_flash_move;
    }
}

void mode_flash_move(BgTokiHikari* this, PlayState* play) {
    if (this->unk_14C < 1.0f) {
        this->unk_14C += 0.05f;
    } else {
        this->unk_14C = 1.0f;
        this->actionFunc = mode_flash_move2;
    }
}

void mode_flash_move2(BgTokiHikari* this, PlayState* play) {
    if (this->unk_14C > 0.2f) {
        this->unk_14C -= 0.025f;
    } else {
        this->unk_14C = 0.0f;
        Actor_delete(&this->actor);
    }
}

void Tri_flash_draw(Actor* thisx, PlayState* play) {
    BgTokiHikari* this = (BgTokiHikari*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_toki_hikari.c", 350);
    Matrix_translate(0.0f, 276.0f, 1122.0f, MTXMODE_NEW);
    Matrix_scale(0.32f, 0.32f, this->unk_14C * 7.0f, MTXMODE_APPLY);
    Matrix_rotateZ(M_PI, MTXMODE_APPLY);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Matrix_push();
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, this->unk_14C * 255.0f, (u8)(155.0f * this->unk_14C) + 100,
                    this->unk_14C * 255.0f, this->unk_14C * 255.0f);

    gDPSetEnvColor(POLY_XLU_DISP++, (u8)(this->unk_14C * 155.0f) + 100, (u8)(255.0f * this->unk_14C), 0, 128);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_toki_hikari.c", 382);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, -2 * (play->gameplayFrames & 0x7F), 0, 0x20, 0x40,
                                1, (play->gameplayFrames & 0x7F) * 4, 0, 0x20, 0x40));

    gSPDisplayList(POLY_XLU_DISP++, object_toki_objects_DL_000880);
    Matrix_pull();
    Matrix_push();
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)(this->unk_14C * 200.0f));

    gDPSetEnvColor(POLY_XLU_DISP++, (u8)(this->unk_14C * 255.0f), (u8)(this->unk_14C * 255.0f),
                   (u8)(this->unk_14C * 255.0f), (u8)(200.0f * this->unk_14C));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_toki_hikari.c", 415);

    gSPDisplayList(POLY_XLU_DISP++, object_toki_objects_DL_0009C0);
    Matrix_pull();
    Matrix_push();
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)(this->unk_14C * 200.0f));

    gDPSetEnvColor(POLY_XLU_DISP++, (u8)(this->unk_14C * 255.0f), (u8)(this->unk_14C * 255.0f),
                   (u8)(this->unk_14C * 255.0f), (u8)(200.0f * this->unk_14C));

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_toki_hikari.c", 437);

    gSPDisplayList(POLY_XLU_DISP++, &object_toki_objects_DL_0009C0[10]);
    Matrix_pull();
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_toki_hikari.c", 443);
}
