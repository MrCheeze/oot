/*
 * File: z_en_ms.c
 * Overlay: ovl_En_Ms
 * Description: Magic Bean Salesman
 */

#include "z_en_ms.h"
#include "assets/objects/object_ms/object_ms.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Ms_actor_ct(Actor* thisx, PlayState* play);
void En_Ms_actor_dt(Actor* thisx, PlayState* play);
void En_Ms_actor_move(Actor* thisx, PlayState* play);
void En_Ms_actor_draw(Actor* thisx, PlayState* play);

void Ms_MessageSet(EnMs* this, PlayState* play);
void Ms_talk_wait(EnMs* this, PlayState* play);
void Ms_talk_start(EnMs* this, PlayState* play);
void Ms_carry_wait(EnMs* this, PlayState* play);
void Ms_carry_start(EnMs* this, PlayState* play);

ActorProfile En_Ms_Profile = {
    /**/ ACTOR_EN_MS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MS,
    /**/ sizeof(EnMs),
    /**/ En_Ms_actor_ct,
    /**/ En_Ms_actor_dt,
    /**/ En_Ms_actor_move,
    /**/ En_Ms_actor_draw,
};

static ColliderCylinderInitType1 MsPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0xFFCFFFFF, 0x00, 0x00 }, 0x00, 0x01, 0x01 },
    { 22, 37, 0, { 0 } },
};

static s16 seed_price[] = {
    10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
};

static u16 msg[] = {
    0x405E, 0x405F, 0x4060, 0x4061, 0x4062, 0x4063, 0x4064, 0x4065, 0x4066, 0x4067,
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 500, ICHAIN_STOP),
};

void Ms_MessageSet(EnMs* this, PlayState* play) {
    this->actor.textId = get_mask_message(play, MASK_REACTION_SET_MAGIC_BEAN_SALESMAN);
    if (this->actor.textId == 0) {
        if (BEANS_BOUGHT >= 10) {
            this->actor.textId = 0x406B;
        } else {
            this->actor.textId = msg[BEANS_BOUGHT];
        }
    }
}

void En_Ms_actor_ct(Actor* thisx, PlayState* play) {
    EnMs* this = (EnMs*)thisx;
    s32 pad;

    if (LINK_AGE_IN_YEARS != YEARS_CHILD) {
        Actor_delete(&this->actor);
        return;
    }
    ValueSet_process(&this->actor, value_init);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBeanSalesmanSkel, &gBeanSalesmanEatingAnim, this->jointTable,
                       this->morphTable, 9);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set3(play, &this->collider, &this->actor, &MsPipeData);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 35.0f);
    Actor_set_scale(&this->actor, 0.015f);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -1.0f;

    Ms_MessageSet(this, play);

    this->actionFunc = Ms_talk_wait;
}

void En_Ms_actor_dt(Actor* thisx, PlayState* play) {
    EnMs* this = (EnMs*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void Ms_talk_wait(EnMs* this, PlayState* play) {
    s16 yawDiff;

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    Ms_MessageSet(this, play);

    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = Ms_talk_start;
    } else if ((this->actor.xzDistToPlayer < 90.0f) && (ABS(yawDiff) < 0x2000)) { // talk range
        Actor_talk_request2(&this->actor, play, 90.0f);
    }
}

void Ms_talk_start(EnMs* this, PlayState* play) {
    u8 dialogState;

    dialogState = message_check(&play->msgCtx);
    if (dialogState != TEXT_STATE_CHOICE) {
        if ((dialogState == TEXT_STATE_DONE) && pad_on_check(play)) { // advanced final textbox
            this->actionFunc = Ms_talk_wait;
        }
    } else if (pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // yes
                if (z_common_data.save.info.playerData.rupees < seed_price[BEANS_BOUGHT]) {
                    message_set2(play, 0x4069); // not enough rupees text
                    return;
                }
                Actor_carry_request_set2(&this->actor, play, GI_MAGIC_BEAN, 90.0f, 10.0f);
                this->actionFunc = Ms_carry_wait;
                return;
            case 1: // no
                message_set2(play, 0x4068);
            default:
                return;
        }
    }
}

void Ms_carry_wait(EnMs* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        lupy_increase(-seed_price[BEANS_BOUGHT]);
        this->actor.parent = NULL;
        this->actionFunc = Ms_carry_start;
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_MAGIC_BEAN, 90.0f, 10.0f);
    }
}

void Ms_carry_start(EnMs* this, PlayState* play) {
    // if dialog state is 6 and player responded to textbox
    if ((message_check(&play->msgCtx)) == TEXT_STATE_DONE && pad_on_check(play)) {
        message_set2(play, 0x406C);
        this->actionFunc = Ms_talk_start;
    }
}

void En_Ms_actor_move(Actor* thisx, PlayState* play) {
    EnMs* this = (EnMs*)thisx;
    s32 pad;

    this->activeTimer++;
    Actor_world_to_eye(&this->actor, 20.0f);
    this->actor.lockOnArrowOffset = 500.0f;
    Actor_set_scale(&this->actor, 0.015f);
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actionFunc(this, play);

    if (z_common_data.save.entranceIndex == ENTR_LON_LON_RANCH_0 &&
        z_common_data.sceneLayer == 8) { // ride carpet if in credits
        Actor_position_moveF(&this->actor);
        PRINTF("OOOHHHHHH %f\n", this->actor.velocity.y);
        Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    }
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void En_Ms_actor_draw(Actor* thisx, PlayState* play) {
    EnMs* this = (EnMs*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, this);
}
