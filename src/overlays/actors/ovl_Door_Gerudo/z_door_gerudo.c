/*
 * File: z_door_gerudo.c
 * Overlay: ovl_Door_Gerudo
 * Description: Metal grate door
 */

#include "z_door_gerudo.h"
#include "assets/objects/object_door_gerudo/object_door_gerudo.h"

#define FLAGS 0

void Door_Gerudo_Actor_ct(Actor* thisx, PlayState* play);
void Door_Gerudo_Actor_dt(Actor* thisx, PlayState* play);
void Door_Gerudo_Actor_move(Actor* thisx, PlayState* play);
void Door_Gerudo_Actor_draw(Actor* thisx, PlayState* play);

static void move_wait(DoorGerudo* this, PlayState* play);
static s32 player_door_open_pos_check(DoorGerudo* this, PlayState* play);
static void move_open(DoorGerudo* this, PlayState* play);
static void move_open_wait(DoorGerudo* this, PlayState* play);

ActorProfile Door_Gerudo_Profile = {
    /**/ ACTOR_DOOR_GERUDO,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_DOOR_GERUDO,
    /**/ sizeof(DoorGerudo),
    /**/ Door_Gerudo_Actor_ct,
    /**/ Door_Gerudo_Actor_dt,
    /**/ Door_Gerudo_Actor_move,
    /**/ Door_Gerudo_Actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_STOP),
};

void Door_Gerudo_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    DoorGerudo* this = (DoorGerudo*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    DynaPolyUty_bgdi_SG2KSG(&gGerudoCellDoorCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 0, 6))) {
        this->actionFunc = move_wait;
        thisx->world.pos.y = thisx->home.pos.y + 200.0f;
    } else {
        this->actionFunc = move_wait;
        this->unk_166 = 10;
    }
}

void Door_Gerudo_Actor_dt(Actor* thisx, PlayState* play) {
    DoorGerudo* this = (DoorGerudo*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static f32 player_distance_check(PlayState* play, DoorGerudo* this, f32 arg2, f32 arg3, f32 arg4) {
    Player* player = GET_PLAYER(play);
    Vec3f playerPos;
    Vec3f sp1C;

    playerPos.x = player->actor.world.pos.x;
    playerPos.y = player->actor.world.pos.y + arg2;
    playerPos.z = player->actor.world.pos.z;
    Actor_search_position_project_distanceXZ(&this->dyna.actor, &sp1C, &playerPos);

    if ((arg3 < fabsf(sp1C.x)) || (arg4 < fabsf(sp1C.y))) {
        return MAXFLOAT;
    } else {
        return sp1C.z;
    }
}

static s32 player_door_open_pos_check(DoorGerudo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 temp_f0;
    s16 rotYDiff;

    if (!player_demo_check(play)) {
        temp_f0 = player_distance_check(play, this, 0.0f, 20.0f, 15.0f);
        if (fabsf(temp_f0) < 40.0f) {
            rotYDiff = player->actor.shape.rot.y - this->dyna.actor.shape.rot.y;
            if (temp_f0 > 0.0f) {
                rotYDiff = 0x8000 - rotYDiff;
            }
            if (ABS(rotYDiff) < 0x2000) {
                return (temp_f0 >= 0.0f) ? 1.0f : -1.0f;
            }
        }
    }
    return 0;
}

static void move_wait(DoorGerudo* this, PlayState* play) {
    if (this->isActive) {
        this->actionFunc = move_open;
        z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex] -= 1;
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_CHAIN_KEY_UNLOCK);
    } else {
        s32 direction = player_door_open_pos_check(this, play);

        if (direction != 0) {
            Player* player = GET_PLAYER(play);

            if (z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex] <= 0) {
                player->naviTextId = -0x203;
            } else if (!Actor_Environment_item_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 5))) {
                player->naviTextId = -0x225;
            } else {
                player->doorType = PLAYER_DOORTYPE_SLIDING;
                player->doorDirection = direction;
                player->doorActor = &this->dyna.actor;
                player->doorTimer = 10;
            }
        }
    }
}

static void move_open(DoorGerudo* this, PlayState* play) {
    if (DECR(this->unk_166) == 0) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_SLIDE_DOOR_OPEN);
        this->actionFunc = move_open_wait;
    }
}

static void move_open_wait(DoorGerudo* this, PlayState* play) {
    chase_f(&this->dyna.actor.velocity.y, 15.0f, 3.0f);
    chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 200.0f, this->dyna.actor.velocity.y);
}

void Door_Gerudo_Actor_move(Actor* thisx, PlayState* play) {
    DoorGerudo* this = (DoorGerudo*)thisx;

    this->actionFunc(this, play);
}

void Door_Gerudo_Actor_draw(Actor* thisx, PlayState* play) {
    DoorGerudo* this = (DoorGerudo*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_door_gerudo.c", 361);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_door_gerudo.c", 365);
    gSPDisplayList(POLY_OPA_DISP++, gGerudoCellDoorDL);

    if (this->unk_166 != 0) {
        Matrix_scale(0.01f, 0.01f, 0.025f, MTXMODE_APPLY);
        key_draw(play, this->unk_166, DOORLOCK_NORMAL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_door_gerudo.c", 377);
}
