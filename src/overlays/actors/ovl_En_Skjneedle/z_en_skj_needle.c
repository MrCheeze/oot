/*
 * File: z_en_skjneedle.c
 * Overlay: ovl_En_Skjneedle
 * Description: Skullkid Needle Attack
 */

#include "z_en_skjneedle.h"
#include "assets/objects/object_skj/object_skj.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR)

void En_Skjneedle_actor_ct(Actor* thisx, PlayState* play);
void En_Skjneedle_actor_dt(Actor* thisx, PlayState* play);
void En_Skjneedle_actor_move(Actor* thisx, PlayState* play2);
void En_Skjneedle_actor_draw(Actor* thisx, PlayState* play);

s32 Skjneedle_Cross(EnSkjneedle* this);

ActorProfile En_Skjneedle_Profile = {
    /**/ ACTOR_EN_SKJNEEDLE,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_SKJ,
    /**/ sizeof(EnSkjneedle),
    /**/ En_Skjneedle_actor_ct,
    /**/ En_Skjneedle_actor_dt,
    /**/ En_Skjneedle_actor_move,
    /**/ En_Skjneedle_actor_draw,
};

static ColliderCylinderInitType1 EnSkjOcInfoData = {
    {
        COL_MATERIAL_HIT1,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 10, 4, -2, { 0, 0, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

void En_Skjneedle_actor_ct(Actor* thisx, PlayState* play) {
    EnSkjneedle* this = (EnSkjneedle*)thisx;

    ValueSet_process(&this->actor, value_init);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set3(play, &this->collider, &this->actor, &EnSkjOcInfoData);
    Shape_Info_init(&this->actor.shape, 0, Actor_shadow_circle, 20.0f);
    thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_set_scale(&this->actor, 0.01f);
}

void En_Skjneedle_actor_dt(Actor* thisx, PlayState* play) {
    EnSkjneedle* this = (EnSkjneedle*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

s32 Skjneedle_Cross(EnSkjneedle* this) {
    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        return 1;
    }
    return 0;
}

void En_Skjneedle_actor_move(Actor* thisx, PlayState* play2) {
    EnSkjneedle* this = (EnSkjneedle*)thisx;
    PlayState* play = play2;

    this->unusedTimer1++;
    if (this->killTimer != 0) {
        this->killTimer--;
    }
    if (Skjneedle_Cross(this) || this->killTimer == 0) {
        Actor_delete(&this->actor);
    } else {
        Actor_set_scale(&this->actor, 0.01f);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        Actor_position_moveF(&this->actor);
        Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 20.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    }
}

void En_Skjneedle_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_skj_needle.c", 200);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_skj_needle.c", 205);
    gSPDisplayList(POLY_OPA_DISP++, gSkullKidNeedleDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_skj_needle.c", 210);
}
