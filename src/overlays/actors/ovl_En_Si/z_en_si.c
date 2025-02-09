/*
 * File: z_en_si.c
 * Overlay: En_Si
 * Description: Gold Skulltula token
 */

#include "z_en_si.h"
#include "z64draw.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR)

void En_Si_actor_ct(Actor* thisx, PlayState* play);
void En_Si_actor_dt(Actor* thisx, PlayState* play);
void En_Si_actor_move(Actor* thisx, PlayState* play);
void En_Si_actor_draw(Actor* thisx, PlayState* play);

static s32 hitcheck(EnSi* this, PlayState* play);
static void wait(EnSi* this, PlayState* play);
void pull(EnSi* this, PlayState* play);
static void carry_end(EnSi* this, PlayState* play);

static ColliderCylinderInit EnSiAtInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000090, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 18, 2, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 SiStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

ActorProfile En_Si_Profile = {
    /**/ ACTOR_EN_SI,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_ST,
    /**/ sizeof(EnSi),
    /**/ En_Si_actor_ct,
    /**/ En_Si_actor_dt,
    /**/ En_Si_actor_move,
    /**/ En_Si_actor_draw,
};

void En_Si_actor_ct(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnSiAtInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &SiStatusData);
    Actor_set_scale(&this->actor, 0.025f);
    this->unk_19C = 0;
    this->actionFunc = wait;
    this->actor.shape.yOffset = 42.0f;
}

void En_Si_actor_dt(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static s32 hitcheck(EnSi* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
    }
    return 0;
}

static void wait(EnSi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        this->actionFunc = pull;
    } else {
        add_calc(&this->actor.scale.x, 0.25f, 0.4f, 1.0f, 0.0f);
        Actor_set_scale(&this->actor, this->actor.scale.x);
        this->actor.shape.rot.y += 0x400;

        if (!player_demo_check(play)) {
            hitcheck(this, play);

            if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
                this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
                item_get_setting(play, ITEM_SKULL_TOKEN);
                player->actor.freezeTimer = 10;
                message_set(play, 0xB4, NULL);
                Na_StartFanfare(NA_BGM_SMALL_ITEM_GET);
                this->actionFunc = carry_end;
            } else {
                CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
                CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
                CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
            }
        }
    }
}

void pull(EnSi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    add_calc(&this->actor.scale.x, 0.25f, 0.4f, 1.0f, 0.0f);
    Actor_set_scale(&this->actor, this->actor.scale.x);
    this->actor.shape.rot.y += 0x400;

    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        item_get_setting(play, ITEM_SKULL_TOKEN);
        player->actor.freezeTimer = 10;
        message_set(play, 0xB4, NULL);
        Na_StartFanfare(NA_BGM_SMALL_ITEM_GET);
        this->actionFunc = carry_end;
    }
}

static void carry_end(EnSi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (message_check(&play->msgCtx) != TEXT_STATE_CLOSING) {
        player->actor.freezeTimer = 10;
    } else {
        SET_GS_FLAGS(PARAMS_GET_S(this->actor.params, 8, 5), PARAMS_GET_S(this->actor.params, 0, 8));
        Actor_delete(&this->actor);
    }
}

void En_Si_actor_move(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    this->actionFunc(this, play);
    Actor_world_to_eye(&this->actor, 16.0f);
}

void En_Si_actor_draw(Actor* thisx, PlayState* play) {
    EnSi* this = (EnSi*)thisx;

    if (this->actionFunc != carry_end) {
        Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);
        Actor_HiliteReflect_set_init(&this->actor, play, 0);
        Draw_GetItemType(play, GID_SKULL_TOKEN_2);
    }
}
