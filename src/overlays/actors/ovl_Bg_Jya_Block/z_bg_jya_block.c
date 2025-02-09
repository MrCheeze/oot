/*
 * File: z_bg_jya_block.c
 * Overlay: ovl_Bg_Jya_Block
 * Description: Silver Block (Child Link)
 */

#include "z_bg_jya_block.h"
#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

#define FLAGS 0

void Bg_Jya_Block_actor_ct(Actor* thisx, PlayState* play);
void Bg_Jya_Block_actor_dt(Actor* thisx, PlayState* play);
void Bg_Jya_Block_actor_move(Actor* thisx, PlayState* play);
void Bg_Jya_Block_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Jya_Block_Profile = {
    /**/ ACTOR_BG_JYA_BLOCK,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_DANGEON_KEEP,
    /**/ sizeof(BgJyaBlock),
    /**/ Bg_Jya_Block_actor_ct,
    /**/ Bg_Jya_Block_actor_dt,
    /**/ Bg_Jya_Block_actor_move,
    /**/ Bg_Jya_Block_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 333, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1500, ICHAIN_STOP),
};

void Bg_Jya_Block_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgJyaBlock* this = (BgJyaBlock*)thisx;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gPushBlockCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    ValueSet_process(&this->dyna.actor, value_init);

    if ((LINK_AGE_IN_YEARS != YEARS_CHILD) || !Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 0, 6))) {
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Jya_Block_actor_dt(Actor* thisx, PlayState* play) {
    BgJyaBlock* this = (BgJyaBlock*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Bg_Jya_Block_actor_move(Actor* thisx, PlayState* play) {
    BgJyaBlock* this = (BgJyaBlock*)thisx;
    Player* player = GET_PLAYER(play);

    player->stateFlags2 &= ~PLAYER_STATE2_4;
    this->dyna.unk_150 = 0.0f;
}

void Bg_Jya_Block_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_jya_block.c", 145);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gPushBlockGrayTex));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_jya_block.c", 153);
    gDPSetEnvColor(POLY_OPA_DISP++, 232, 210, 176, 255);
    gSPDisplayList(POLY_OPA_DISP++, gPushBlockDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_jya_block.c", 158);
}
