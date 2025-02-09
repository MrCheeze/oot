/*
 * File: z_door_ana.c
 * Overlay: ovl_Door_Ana
 * Description: Grotto
 */

#include "z_door_ana.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS ACTOR_FLAG_UPDATE_DURING_OCARINA

void Door_Ana_actor_ct(Actor* thisx, PlayState* play);
void Door_Ana_actor_dt(Actor* thisx, PlayState* play);
void Door_Ana_actor_move(Actor* thisx, PlayState* play);
void Door_Ana_actor_draw(Actor* thisx, PlayState* play);

void move_bom_wait(DoorAna* this, PlayState* play);
static void move_wait(DoorAna* this, PlayState* play);
static void move_demo_wait(DoorAna* this, PlayState* play);

ActorProfile Door_Ana_Profile = {
    /**/ ACTOR_DOOR_ANA,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(DoorAna),
    /**/ Door_Ana_actor_ct,
    /**/ Door_Ana_actor_dt,
    /**/ Door_Ana_actor_move,
    /**/ Door_Ana_actor_draw,
};

static ColliderCylinderInit AcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000048, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 50, 10, 0, { 0 } },
};

static s16 ana_warp_scene[] = {
    ENTR_FAIRYS_FOUNTAIN_0, ENTR_GROTTOS_0,  ENTR_GROTTOS_1,  ENTR_GROTTOS_2,  ENTR_GROTTOS_3,
    ENTR_GROTTOS_4,         ENTR_GROTTOS_5,  ENTR_GROTTOS_6,  ENTR_GROTTOS_7,  ENTR_GROTTOS_8,
    ENTR_GROTTOS_9,         ENTR_GROTTOS_10, ENTR_GROTTOS_11, ENTR_GROTTOS_12, ENTR_GROTTOS_13,
};

void Door_Ana_actor_set_process(DoorAna* this, DoorAnaActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Door_Ana_actor_ct(Actor* thisx, PlayState* play) {
    DoorAna* this = (DoorAna*)thisx;

    this->actor.shape.rot.z = 0;
    this->actor.shape.rot.y = this->actor.shape.rot.z;
    // init block for grottos that are initially "hidden" (require explosives/hammer/song of storms to open)
    if (PARAMS_GET_NOSHIFT(this->actor.params, 8, 2) != 0) {
        // only allocate collider for grottos that need bombing/hammering open
        if (PARAMS_GET_NOSHIFT(this->actor.params, 9, 1) != 0) {
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set5(play, &this->collider, &this->actor, &AcInfoData);
        } else {
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        }
        Actor_set_scale(&this->actor, 0);
        Door_Ana_actor_set_process(this, move_bom_wait);
    } else {
        Door_Ana_actor_set_process(this, move_wait);
    }
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
}

void Door_Ana_actor_dt(Actor* thisx, PlayState* play) {
    DoorAna* this = (DoorAna*)thisx;

    // free collider if it has one
    if (PARAMS_GET_NOSHIFT(this->actor.params, 9, 1) != 0) {
        ClObjPipe_dt(play, &this->collider);
    }
}

// update routine for grottos that are currently "hidden"/unopened
void move_bom_wait(DoorAna* this, PlayState* play) {
    u32 openGrotto = false;

    if (!PARAMS_GET_NOSHIFT(this->actor.params, 9, 1)) {
        // opening with song of storms
        if (this->actor.xyzDistToPlayerSq < SQ(200.0f) && eventbit_check(play, 5)) {
            openGrotto = true;
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        }
    } else {
        // bombing/hammering open a grotto
        if (this->collider.base.acFlags & AC_HIT) {
            openGrotto = true;
            ClObjPipe_dt(play, &this->collider);
        } else {
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
    // open the grotto
    if (openGrotto) {
        this->actor.params &= ~0x0300;
        Door_Ana_actor_set_process(this, move_wait);
        Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
    Actor_dowsing_request(&this->actor, play);
}

// update routine for grottos that are open
static void move_wait(DoorAna* this, PlayState* play) {
    Player* player;
    s32 destinationIdx;

    player = GET_PLAYER(play);
    if (chase_f(&this->actor.scale.x, 0.01f, 0.001f)) {
        if ((this->actor.attentionRangeType != 0) && (play->transitionTrigger == TRANS_TRIGGER_OFF) &&
            (player->stateFlags1 & PLAYER_STATE1_31) && (player->av1.actionVar1 == 0)) {
            destinationIdx = PARAMS_GET_U(this->actor.params, 12, 3) - 1;
            Game_play_restart_set(play, RESPAWN_MODE_RETURN,
                                   PLAYER_PARAMS(PLAYER_START_MODE_GROTTO, PLAYER_START_BG_CAM_DEFAULT));
            z_common_data.respawn[RESPAWN_MODE_RETURN].pos.y = this->actor.world.pos.y;
            z_common_data.respawn[RESPAWN_MODE_RETURN].yaw = this->actor.home.rot.y;
            z_common_data.respawn[RESPAWN_MODE_RETURN].data = PARAMS_GET_U(this->actor.params, 0, 16);
            if (destinationIdx < 0) {
                destinationIdx = this->actor.home.rot.z + 1;
            }
            play->nextEntranceIndex = ana_warp_scene[destinationIdx];
            Door_Ana_actor_set_process(this, move_demo_wait);
        } else {
            if (!player_demo_check(play) && !(player->stateFlags1 & (PLAYER_STATE1_23 | PLAYER_STATE1_27)) &&
                this->actor.xzDistToPlayer <= 15.0f && -50.0f <= this->actor.yDistToPlayer &&
                this->actor.yDistToPlayer <= 15.0f) {
                player->stateFlags1 |= PLAYER_STATE1_31;
                this->actor.attentionRangeType = ATTENTION_RANGE_1;
            } else {
                this->actor.attentionRangeType = ATTENTION_RANGE_0;
            }
        }
    }
    Actor_set_scale(&this->actor, this->actor.scale.x);
}

// update function for after the player has triggered the grotto
static void move_demo_wait(DoorAna* this, PlayState* play) {
    Player* player;

    if (this->actor.yDistToPlayer <= 0.0f && 15.0f < this->actor.xzDistToPlayer) {
        player = GET_PLAYER(play);
        player->actor.world.pos.x = sin_s(this->actor.yawTowardsPlayer) * 15.0f + this->actor.world.pos.x;
        player->actor.world.pos.z = cos_s(this->actor.yawTowardsPlayer) * 15.0f + this->actor.world.pos.z;
    }
}

void Door_Ana_actor_move(Actor* thisx, PlayState* play) {
    DoorAna* this = (DoorAna*)thisx;

    this->actionFunc(this, play);
    // changes the grottos facing angle based on camera angle
    this->actor.shape.rot.y = getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000;
}

void Door_Ana_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_door_ana.c", 440);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_door_ana.c", 446);
    gSPDisplayList(POLY_XLU_DISP++, gGrottoDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_door_ana.c", 449);
}
