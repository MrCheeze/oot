/*
 * File: z_item_ocarina.c
 * Overlay: ovl_Item_Ocarina
 * Description: Ocarina of Time
 */

#include "z_item_ocarina.h"
#include "z64draw.h"
#include "assets/scenes/overworld/spot00/spot00_scene.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Item_Ocarina_Actor_ct(Actor* thisx, PlayState* play);
void Item_Ocarina_Actor_dt(Actor* thisx, PlayState* play);
void Item_Ocarina_Actor_move(Actor* thisx, PlayState* play);
void Item_Ocarina_Actor_draw(Actor* thisx, PlayState* play);

static void move_wait(ItemOcarina* this, PlayState* play);
void move_next(ItemOcarina* this, PlayState* play);
void move_get(ItemOcarina* this, PlayState* play);
void to_demo(ItemOcarina* this, PlayState* play);
void move_next2(ItemOcarina* this, PlayState* play);
static void move_wait2(ItemOcarina* this, PlayState* play);
void move_test(ItemOcarina* this, PlayState* play);

ActorProfile Item_Ocarina_Profile = {
    /**/ ACTOR_ITEM_OCARINA,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GI_OCARINA,
    /**/ sizeof(ItemOcarina),
    /**/ Item_Ocarina_Actor_ct,
    /**/ Item_Ocarina_Actor_dt,
    /**/ Item_Ocarina_Actor_move,
    /**/ Item_Ocarina_Actor_draw,
};

void Item_Ocarina_actor_set_process(ItemOcarina* this, ItemOcarinaActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Item_Ocarina_Actor_ct(Actor* thisx, PlayState* play) {
    ItemOcarina* this = (ItemOcarina*)thisx;
    s32 params = thisx->params;

    Shape_Info_init(&this->actor.shape, 0, NULL, 0);
    Actor_set_scale(&this->actor, 0.1f);

    switch (params) {
        case 0:
            Item_Ocarina_actor_set_process(this, move_wait);
            break;
        case 1:
            Item_Ocarina_actor_set_process(this, move_wait2);
            break;
        case 2:
            Item_Ocarina_actor_set_process(this, move_test);
            break;
        case 3:
            Item_Ocarina_actor_set_process(this, move_get);
            if (!GET_EVENTCHKINF(EVENTCHKINF_80) || GET_EVENTCHKINF(EVENTCHKINF_43)) {
                Actor_delete(thisx);
                return;
            }
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_ELF_MSG2, 299.0f, -140.0f, 884.0f, 0, 4, 1, 0x3800);
            Actor_set_scale(thisx, 0.2f);
            break;
        default:
            Actor_delete(thisx);
            return;
    }

    LOG_NUM("no", params, "../z_item_ocarina.c", 210);
    this->spinRotOffset = 0x400;
}

void Item_Ocarina_Actor_dt(Actor* thisx, PlayState* play) {
}

void move_next(ItemOcarina* this, PlayState* play) {
    Vec3f ripplePos;

    Actor_position_move(&this->actor);
    this->actor.shape.rot.x += this->spinRotOffset * 2;
    this->actor.shape.rot.y += this->spinRotOffset * 3;

    if (this->actor.minVelocityY < this->actor.velocity.y) {
        this->actor.velocity.y += this->actor.gravity;
        if (this->actor.velocity.y < this->actor.minVelocityY) {
            this->actor.velocity.y = this->actor.minVelocityY;
        }
    }

    if (play->csCtx.curFrame == 881) {
        this->actor.world.pos.x = 250.0f;
        this->actor.world.pos.y = 60.0f;
        this->actor.world.pos.z = 1075.0f;
        this->actor.velocity.x = 1.0f;
        this->actor.velocity.y = -5.0f;
        this->actor.velocity.z = -7.0f;
    }

    if (play->csCtx.curFrame == 897) {
        Effect_SS_G_Ripple_ct2(play, &this->actor.world.pos, 100, 500, 0);
        Effect_SS_G_Splash_sc_cl_ct(play, &this->actor.world.pos, NULL, NULL, 1, 0);
        this->actor.velocity.x = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.z = 0.0f;
        this->actor.gravity = -0.1f;
        this->actor.minVelocityY = -0.5f;
        this->spinRotOffset = 0;
        Actor_SE_set(&this->actor, NA_SE_EV_BOMB_DROP_WATER);
    }

    // landed in water
    if (play->csCtx.curFrame == 906) {
        ripplePos.x = 274.0f;
        ripplePos.y = -60.0f;
        ripplePos.z = 907.0f;
        Effect_SS_G_Ripple_ct2(play, &ripplePos, 100, 500, 0);
    }
}

static void move_wait(ItemOcarina* this, PlayState* play) {
    this->actor.gravity = -0.3f;
    this->actor.minVelocityY = -5.0f;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 6.0f;
    this->actor.velocity.z = 0.0f;
    Item_Ocarina_actor_set_process(this, move_next);
}

void move_next2(ItemOcarina* this, PlayState* play) {
    Actor_position_move(&this->actor);
    this->actor.shape.rot.x += this->spinRotOffset * 2;
    this->actor.shape.rot.y += this->spinRotOffset * 3;

    if (this->actor.minVelocityY < this->actor.velocity.y) {
        this->actor.velocity.y += this->actor.gravity;

        if (this->actor.velocity.y < this->actor.minVelocityY) {
            this->actor.velocity.y = this->actor.minVelocityY;
        }
    }

    if (play->csCtx.curFrame == 220) {
        this->actor.world.pos.x = 144.0f;
        this->actor.world.pos.y = 80.0f;
        this->actor.world.pos.z = 1686.0f;
        this->actor.velocity.x = 1.0f;
        this->actor.velocity.y = 2.0f;
        this->actor.velocity.z = -7.0f;
        this->actor.gravity = -0.15f;
        this->actor.minVelocityY = -5.0f;
    }
}

static void move_wait2(ItemOcarina* this, PlayState* play) {
    this->actor.gravity = -0.3f;
    this->actor.minVelocityY = -5.0f;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 4.0f;
    this->actor.velocity.z = 6.0f;
    Item_Ocarina_actor_set_process(this, move_next2);
}

void move_test(ItemOcarina* this, PlayState* play) {
}

void to_demo(ItemOcarina* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gHyruleFieldZeldaSongOfTimeCs);
        z_common_data.cutsceneTrigger = 1;
    }
}

void move_get(ItemOcarina* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        SET_EVENTCHKINF(EVENTCHKINF_43);
        Actor_Environment_sw_On(play, 3);
        this->actionFunc = to_demo;
        this->actor.draw = NULL;
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_OCARINA_OF_TIME, 30.0f, 50.0f);

        if ((play->gameplayFrames & 13) == 0) {
            Effect_SS_Bubble_ct(play, &this->actor.world.pos, 0.0f, 0.0f, 10.0f, 0.13f);
        }
    }
}

void Item_Ocarina_Actor_move(Actor* thisx, PlayState* play) {
    ItemOcarina* this = (ItemOcarina*)thisx;

    this->actionFunc(this, play);
}

void Item_Ocarina_Actor_draw(Actor* thisx, PlayState* play) {
    ItemOcarina* this = (ItemOcarina*)thisx;

    Actor_HiliteReflect_set_init(thisx, play, 0);
    Actor_HiliteReflect_xlu_set_init(thisx, play, 0);
    Draw_GetItemType(play, GID_OCARINA_OF_TIME);
}
