/*
 * File: z_item_b_heart.c
 * Overlay: ovl_Item_B_Heart
 * Description: Heart Container
 */

#include "z_item_b_heart.h"
#include "assets/objects/object_gi_hearts/object_gi_hearts.h"

#define FLAGS 0

void Item_B_Heart_actor_ct(Actor* thisx, PlayState* play);
void Item_B_Heart_actor_dt(Actor* thisx, PlayState* play);
void Item_B_Heart_actor_move(Actor* thisx, PlayState* play);
void Item_B_Heart_actor_draw(Actor* thisx, PlayState* play);

void heart_proc(ItemBHeart* this, PlayState* play);

ActorProfile Item_B_Heart_Profile = {
    /**/ ACTOR_ITEM_B_HEART,
    /**/ ACTORCAT_MISC,
    /**/ FLAGS,
    /**/ OBJECT_GI_HEARTS,
    /**/ sizeof(ItemBHeart),
    /**/ Item_B_Heart_actor_ct,
    /**/ Item_B_Heart_actor_dt,
    /**/ Item_B_Heart_actor_move,
    /**/ Item_B_Heart_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
};

void Item_B_Heart_actor_ct(Actor* thisx, PlayState* play) {
    ItemBHeart* this = (ItemBHeart*)thisx;

    if (Actor_Environment_item_Check(play, 0x1F)) {
        Actor_delete(&this->actor);
    } else {
        ValueSet_process(&this->actor, value_init);
        Shape_Info_init(&this->actor.shape, 0.0f, NULL, 0.8f);
    }
}

void Item_B_Heart_actor_dt(Actor* thisx, PlayState* play) {
}

void Item_B_Heart_actor_move(Actor* thisx, PlayState* play) {
    ItemBHeart* this = (ItemBHeart*)thisx;

    heart_proc(this, play);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    if (Actor_carry_check(&this->actor, play)) {
        Actor_Environment_item_On(play, 0x1F);
        Actor_delete(&this->actor);
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_HEART_CONTAINER_2, 30.0f, 40.0f);
    }
}

void heart_proc(ItemBHeart* this, PlayState* play) {
    f32 yOffset;

    this->unk_164++;
    yOffset = (sin_s(this->unk_164 * 0x60C) * 5.0f) + 20.0f;
    add_calc2(&this->actor.world.pos.y, this->actor.home.pos.y + yOffset, 0.1f, this->unk_158);
    add_calc2(&this->unk_158, 2.0f, 1.0f, 0.1f);
    this->actor.shape.rot.y += 0x400;

    add_calc2(&this->actor.scale.x, 0.4f, 0.1f, 0.01f);
    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;
}

void Item_B_Heart_actor_draw(Actor* thisx, PlayState* play) {
    ItemBHeart* this = (ItemBHeart*)thisx;
    Actor* actorIt;
    u8 flag = false;

    OPEN_DISPS(play->state.gfxCtx, "../z_item_b_heart.c", 506);

    actorIt = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;

    while (actorIt != NULL) {
        if ((actorIt->id == ACTOR_DOOR_WARP1) && (actorIt->projectedPos.z > this->actor.projectedPos.z)) {
            flag = true;
            break;
        }
        actorIt = actorIt->next;
    }

    if (flag) {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_item_b_heart.c", 551);
        gSPDisplayList(POLY_XLU_DISP++, gGiHeartBorderDL);
        gSPDisplayList(POLY_XLU_DISP++, gGiHeartContainerDL);
    } else {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_item_b_heart.c", 557);
        gSPDisplayList(POLY_OPA_DISP++, gGiHeartBorderDL);
        gSPDisplayList(POLY_OPA_DISP++, gGiHeartContainerDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_item_b_heart.c", 561);
}
