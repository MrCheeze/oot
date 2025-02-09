/*
 * File: z_item_etcetera.c
 * Overlay: ovl_Item_Etcetera
 * Description: Collectible Items
 */

#include "z_item_etcetera.h"
#include "z64draw.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void item_etcetera_Actor_ct(Actor* thisx, PlayState* play);
void item_etcetera_Actor_dt(Actor* thisx, PlayState* play);
void item_etcetera_Actor_move(Actor* thisx, PlayState* play);
void draw_inbox(Actor* thisx, PlayState* play);
void draw_normal(Actor* thisx, PlayState* play);

static void move_wait(ItemEtcetera* this, PlayState* play);
static void move_normal(ItemEtcetera* this, PlayState* play);
void move_awaawa(ItemEtcetera* this, PlayState* play);
void item_etcetera_kirakira(ItemEtcetera* this, PlayState* play);
static void move_fall(ItemEtcetera* this, PlayState* play);
void move_inbox(ItemEtcetera* this, PlayState* play);
static void move_demo(ItemEtcetera* this, PlayState* play);

ActorProfile Item_Etcetera_Profile = {
    /**/ ACTOR_ITEM_ETCETERA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(ItemEtcetera),
    /**/ item_etcetera_Actor_ct,
    /**/ item_etcetera_Actor_dt,
    /**/ item_etcetera_Actor_move,
    /**/ NULL,
};

static s16 demo_effect_shape_data[] = {
    OBJECT_GI_BOTTLE,        // ITEM_ETC_BOTTLE
    OBJECT_GI_BOTTLE_LETTER, // ITEM_ETC_LETTER
    OBJECT_GI_SHIELD_2,      // ITEM_ETC_SHIELD_HYLIAN
    OBJECT_GI_ARROWCASE,     // ITEM_ETC_QUIVER
    OBJECT_GI_SCALE,         // ITEM_ETC_SCALE_SILVER
    OBJECT_GI_SCALE,         // ITEM_ETC_SCALE_GOLD
    OBJECT_GI_KEY,           // ITEM_ETC_KEY_SMALL
    OBJECT_GI_M_ARROW,       // ITEM_ETC_ARROW_FIRE
    OBJECT_GI_RUPY,          // ITEM_ETC_RUPEE_GREEN_CHEST_GAME
    OBJECT_GI_RUPY,          // ITEM_ETC_RUPEE_BLUE_CHEST_GAME
    OBJECT_GI_RUPY,          // ITEM_ETC_RUPEE_RED_CHEST_GAME
    OBJECT_GI_RUPY,          // ITEM_ETC_RUPEE_PURPLE_CHEST_GAME
    OBJECT_GI_HEARTS,        // ITEM_ETC_HEART_PIECE_CHEST_GAME
    OBJECT_GI_KEY,           // ITEM_ETC_KEY_SMALL_CHEST_GAME
};

// Indices passed to the item table in z_draw.c
static s16 demo_effect_gi_code[] = {
    GID_BOTTLE_EMPTY,        // ITEM_ETC_BOTTLE
    GID_BOTTLE_RUTOS_LETTER, // ITEM_ETC_LETTER
    GID_SHIELD_HYLIAN,       // ITEM_ETC_SHIELD_HYLIAN
    GID_QUIVER_40,           // ITEM_ETC_QUIVER
    GID_SCALE_SILVER,        // ITEM_ETC_SCALE_SILVER
    GID_SCALE_GOLDEN,        // ITEM_ETC_SCALE_GOLD
    GID_SMALL_KEY,           // ITEM_ETC_KEY_SMALL
    GID_ARROW_FIRE,          // ITEM_ETC_ARROW_FIRE
    GID_RUPEE_GREEN,         // ITEM_ETC_RUPEE_GREEN_CHEST_GAME
    GID_RUPEE_BLUE,          // ITEM_ETC_RUPEE_BLUE_CHEST_GAME
    GID_RUPEE_RED,           // ITEM_ETC_RUPEE_RED_CHEST_GAME
    GID_RUPEE_PURPLE,        // ITEM_ETC_RUPEE_PURPLE_CHEST_GAME
    GID_HEART_PIECE,         // ITEM_ETC_HEART_PIECE_CHEST_GAME
    GID_SMALL_KEY,           // ITEM_ETC_KEY_SMALL_CHEST_GAME
};

static s16 demo_effect_rq_code[] = {
    GI_BOTTLE_EMPTY,        // ITEM_ETC_BOTTLE
    GI_BOTTLE_RUTOS_LETTER, // ITEM_ETC_LETTER
    GI_SHIELD_HYLIAN,       // ITEM_ETC_SHIELD_HYLIAN
    GI_QUIVER_40,           // ITEM_ETC_QUIVER
    GI_SCALE_SILVER,        // ITEM_ETC_SCALE_SILVER
    GI_SCALE_GOLDEN,        // ITEM_ETC_SCALE_GOLD
    GI_SMALL_KEY,           // ITEM_ETC_KEY_SMALL
    GI_ARROW_FIRE,          // ITEM_ETC_ARROW_FIRE
    GI_NONE,                // ITEM_ETC_RUPEE_GREEN_CHEST_GAME
    GI_NONE,                // ITEM_ETC_RUPEE_BLUE_CHEST_GAME
    GI_NONE,                // ITEM_ETC_RUPEE_RED_CHEST_GAME
    GI_NONE,                // ITEM_ETC_RUPEE_PURPLE_CHEST_GAME
    GI_NONE,                // ITEM_ETC_HEART_PIECE_CHEST_GAME
    GI_NONE,                // ITEM_ETC_KEY_SMALL_CHEST_GAME
};

void item_etcetera_actor_set_process(ItemEtcetera* this, ItemEtceteraActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void item_etcetera_Actor_ct(Actor* thisx, PlayState* play) {
    ItemEtcetera* this = (ItemEtcetera*)thisx;
    s32 pad;
    s32 type;
    s32 objectSlot;

    type = PARAMS_GET_U(this->actor.params, 0, 8);
    PRINTF("no = %d\n", type);
    objectSlot = Object_Exchange_bank_check(&play->objectCtx, demo_effect_shape_data[type]);
    PRINTF("bank_ID = %d\n", objectSlot);
    if (objectSlot < 0) {
        ASSERT(0, "0", "../z_item_etcetera.c", 241);
    } else {
        this->requiredObjectSlot = objectSlot;
    }
    this->giDrawId = demo_effect_gi_code[type];
    this->getItemId = demo_effect_rq_code[type];
    this->futureActionFunc = move_normal;
    this->drawFunc = draw_normal;
    Actor_set_scale(&this->actor, 0.25f);
    item_etcetera_actor_set_process(this, move_wait);
    switch (type) {
        case ITEM_ETC_LETTER:
            Actor_set_scale(&this->actor, 0.5f);
            this->futureActionFunc = move_awaawa;
            if (GET_EVENTCHKINF(EVENTCHKINF_31)) {
                Actor_delete(&this->actor);
            }
            break;
        case ITEM_ETC_ARROW_FIRE:
            this->futureActionFunc = move_demo;
            Actor_set_scale(&this->actor, 0.5f);
            this->actor.draw = NULL;
            this->actor.shape.yOffset = 50.0f;
            break;
        case ITEM_ETC_RUPEE_GREEN_CHEST_GAME:
        case ITEM_ETC_RUPEE_BLUE_CHEST_GAME:
        case ITEM_ETC_RUPEE_RED_CHEST_GAME:
        case ITEM_ETC_RUPEE_PURPLE_CHEST_GAME:
        case ITEM_ETC_HEART_PIECE_CHEST_GAME:
        case ITEM_ETC_KEY_SMALL_CHEST_GAME:
            Actor_set_scale(&this->actor, 0.5f);
            this->futureActionFunc = move_inbox;
            this->drawFunc = draw_inbox;
            this->actor.world.pos.y += 15.0f;
            break;
    }
}

void item_etcetera_Actor_dt(Actor* thisx, PlayState* play) {
}

static void move_wait(ItemEtcetera* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->actor.objectSlot = this->requiredObjectSlot;
        this->actor.draw = this->drawFunc;
        this->actionFunc = this->futureActionFunc;
    }
}

static void move_normal(ItemEtcetera* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        if (PARAMS_GET_U(this->actor.params, 0, 8) == 1) {
            SET_EVENTCHKINF(EVENTCHKINF_31);
            Actor_Environment_sw_On(play, 0xB);
        }
        Actor_delete(&this->actor);
    } else {
        Actor_carry_request_set2(&this->actor, play, this->getItemId, 30.0f, 50.0f);
    }
}

void move_awaawa(ItemEtcetera* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        if (PARAMS_GET_U(this->actor.params, 0, 8) == 1) {
            SET_EVENTCHKINF(EVENTCHKINF_31);
            Actor_Environment_sw_On(play, 0xB);
        }
        Actor_delete(&this->actor);
    } else {
        if (0) {} // Necessary to match
        Actor_carry_request_set2(&this->actor, play, this->getItemId, 30.0f, 50.0f);
        if ((play->gameplayFrames & 0xD) == 0) {
            Effect_SS_Bubble_ct(play, &this->actor.world.pos, 0.0f, 0.0f, 10.0f, 0.13f);
        }
    }
}

void item_etcetera_kirakira(ItemEtcetera* this, PlayState* play) {
    static Vec3f kirakira_vec = { 0.0f, 0.2f, 0.0f };
    static Vec3f kirakira_acc = { 0.0f, 0.05f, 0.0f };
    static Color_RGBA8 kirakira_prim = { 255, 255, 255, 0 };
    static Color_RGBA8 kirakira_env = { 255, 50, 50, 0 };
    Vec3f pos;

    kirakira_vec.x = rnd_fx(3.0f);
    kirakira_vec.z = rnd_fx(3.0f);
    kirakira_vec.y = -0.05f;
    kirakira_acc.y = -0.025f;
    pos.x = rnd_fx(12.0f) + this->actor.world.pos.x;
    pos.y = (fqrand() * 6.0f) + this->actor.world.pos.y;
    pos.z = rnd_fx(12.0f) + this->actor.world.pos.z;
    Effect_SS_KiraKira_sc_ct_ct(play, &pos, &kirakira_vec, &kirakira_acc, &kirakira_prim, &kirakira_env, 5000, 16);
}

static void move_fall(ItemEtcetera* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 10.0f, 10.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    Actor_position_moveF(&this->actor);
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        item_etcetera_kirakira(this, play);
    }
    this->actor.shape.rot.y += 0x400;
    move_normal(this, play);
}

void move_inbox(ItemEtcetera* this, PlayState* play) {
    if (Actor_Environment_Tbox_Check(play, PARAMS_GET_U(this->actor.params, 8, 5))) {
        Actor_delete(&this->actor);
    }
}

static void move_demo(ItemEtcetera* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[0] != NULL)) {
        LOG_NUM("(game_play->demo_play.npcdemopnt[0]->dousa)", play->csCtx.actorCues[0]->id, "../z_item_etcetera.c",
                441);
        if (play->csCtx.actorCues[0]->id == 2) {
            this->actor.draw = draw_normal;
            this->actor.gravity = -0.1f;
            this->actor.minVelocityY = -4.0f;
            this->actionFunc = move_fall;
        }
    } else {
        this->actor.gravity = -0.1f;
        this->actor.minVelocityY = -4.0f;
        this->actionFunc = move_fall;
    }
}

void item_etcetera_Actor_move(Actor* thisx, PlayState* play) {
    ItemEtcetera* this = (ItemEtcetera*)thisx;
    this->actionFunc(this, play);
}

void draw_inbox(Actor* thisx, PlayState* play) {
    ItemEtcetera* this = (ItemEtcetera*)thisx;
    if (play->actorCtx.lensActive) {
        Actor_HiliteReflect_set_init(&this->actor, play, 0);
        Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);
        Draw_GetItemType(play, this->giDrawId);
    }
}

void draw_normal(Actor* thisx, PlayState* play) {
    ItemEtcetera* this = (ItemEtcetera*)thisx;

    Actor_HiliteReflect_set_init(&this->actor, play, 0);
    Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);
    Draw_GetItemType(play, this->giDrawId);
}
