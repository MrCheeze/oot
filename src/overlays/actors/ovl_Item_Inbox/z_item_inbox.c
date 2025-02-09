/*
 * File: z_item_inbox.c
 * Overlay: ovl_Item_Inbox
 * Description: Zelda's magic effect when opening gates in castle collapse
 */

#include "z_item_inbox.h"
#include "z64draw.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void Item_Inbox_Actor_ct(Actor* thisx, PlayState* play);
void Item_Inbox_Actor_dt(Actor* thisx, PlayState* play);
void Item_Inbox_Actor_move(Actor* thisx, PlayState* play);
void Item_Inbox_Actor_draw(Actor* thisx, PlayState* play);

static void move_wait(ItemInbox* this, PlayState* play);

ActorProfile Item_Inbox_Profile = {
    /**/ ACTOR_ITEM_INBOX,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(ItemInbox),
    /**/ Item_Inbox_Actor_ct,
    /**/ Item_Inbox_Actor_dt,
    /**/ Item_Inbox_Actor_move,
    /**/ Item_Inbox_Actor_draw,
};

void Item_Inbox_Actor_ct(Actor* thisx, PlayState* play) {
    ItemInbox* this = (ItemInbox*)thisx;

    this->actionFunc = move_wait;
    Actor_set_scale(&this->actor, 0.2);
}

void Item_Inbox_Actor_dt(Actor* thisx, PlayState* play) {
}

static void move_wait(ItemInbox* this, PlayState* play) {
    if (Actor_Environment_Tbox_Check(play, PARAMS_GET_U(this->actor.params, 8, 5))) {
        Actor_delete(&this->actor);
    }
}

void Item_Inbox_Actor_move(Actor* thisx, PlayState* play) {
    ItemInbox* this = (ItemInbox*)thisx;

    this->actionFunc(this, play);
}

void Item_Inbox_Actor_draw(Actor* thisx, PlayState* play) {
    ItemInbox* this = (ItemInbox*)thisx;

    Actor_HiliteReflect_set_init(&this->actor, play, 0);
    Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);
    Draw_GetItemType(play, PARAMS_GET_U(this->actor.params, 0, 8));
}
