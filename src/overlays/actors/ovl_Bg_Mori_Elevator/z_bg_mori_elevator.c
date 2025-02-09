#include "z_bg_mori_elevator.h"
#include "assets/objects/object_mori_objects/object_mori_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Mori_Elevator_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mori_Elevator_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mori_Elevator_actor_move(Actor* thisx, PlayState* play);

static void mvSet_dmaWait(BgMoriElevator* this);
static void mv_dmaWait(BgMoriElevator* this, PlayState* play);
static void mvSet_stop(BgMoriElevator* this);
static void mv_stop(BgMoriElevator* this, PlayState* play);
void Bg_Mori_Elevator_actor_draw(Actor* thisx, PlayState* play);
static void mvSet_updown(BgMoriElevator* this);
static void mv_updown(BgMoriElevator* this, PlayState* play);
void mv_dropDemo(BgMoriElevator* this, PlayState* play);
void mv_rizeDemo(BgMoriElevator* this, PlayState* play);

static s16 Mori_Elv_state = false;

ActorProfile Bg_Mori_Elevator_Profile = {
    /**/ ACTOR_BG_MORI_ELEVATOR,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MORI_OBJECTS,
    /**/ sizeof(BgMoriElevator),
    /**/ Bg_Mori_Elevator_actor_ct,
    /**/ Bg_Mori_Elevator_actor_dt,
    /**/ Bg_Mori_Elevator_actor_move,
    /**/ NULL,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 3000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

f32 add_calcNEW(f32* pValue, f32 target, f32 scale, f32 maxStep, f32 minStep) {
    f32 var = (target - *pValue) * scale;

    if (*pValue < target) {
        if (maxStep < var) {
            var = maxStep;
        } else {
            if (var < minStep) {
                var = minStep;
            }
        }
        *pValue = (*pValue + var);

        if (target < *pValue) {
            *pValue = target;
        }
    } else {
        if (target < *pValue) {
            if (var < (-maxStep)) {
                var = (-maxStep);
            } else {
                if ((-minStep) < var) {
                    var = (-minStep);
                }
            }
            *pValue = (*pValue + var);
            if (*pValue < target) {
                *pValue = target;
            }
        } else {
            var = 0.0f;
        }
    }
    return var;
}

void SeSet_elevatorMove(BgMoriElevator* this, f32 distTo) {
    f32 temp;

    temp = fabsf(distTo) * 0.09f;
    Na_SetMotorSe(&this->dyna.actor.projectedPos, NA_SE_EV_ELEVATOR_MOVE2 - SFX_FLAG, CLAMP(temp, 0.0f, 1.0f));
}

void Bg_Mori_Elevator_actor_ct(Actor* thisx, PlayState* play) {
    BgMoriElevator* this = (BgMoriElevator*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    this->unk_172 = Mori_Elv_state;
    this->moriTexObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_TEX);

#if DEBUG_FEATURES
    if (this->moriTexObjectSlot < 0) {
        Actor_delete(thisx);
        // "Forest Temple obj elevator Bank Danger!"
        PRINTF("Error : 森の神殿 obj elevator バンク危険！(%s %d)\n", "../z_bg_mori_elevator.c", 277);
        return;
    }
#endif

    switch (Mori_Elv_state) {
        case false:
            // "Forest Temple elevator CT"
            PRINTF("森の神殿 elevator CT\n");
            Mori_Elv_state = true;
            this->dyna.actor.room = -1;
            ValueSet_process(&this->dyna.actor, value_init);
            MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
            DynaPolyUty_bgdi_SG2KSG(&gMoriElevatorCol, &colHeader);
            this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
            mvSet_dmaWait(this);
            break;
        case true:
            Actor_delete(thisx);
            break;
    }
}

void Bg_Mori_Elevator_actor_dt(Actor* thisx, PlayState* play) {
    BgMoriElevator* this = (BgMoriElevator*)thisx;

    if (this->unk_172 == 0) {
        // "Forest Temple elevator DT"
        PRINTF("森の神殿 elevator DT\n");
        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
        Mori_Elv_state = false;
    }
}

int checkRideElevator(BgMoriElevator* this, PlayState* play) {
    return ((this->dyna.interactFlags & DYNA_INTERACT_PLAYER_ON_TOP) &&
            !(this->unk_170 & DYNA_INTERACT_PLAYER_ON_TOP) &&
            ((GET_PLAYER(play)->actor.world.pos.y - this->dyna.actor.world.pos.y) < 80.0f));
}

static void mvSet_dmaWait(BgMoriElevator* this) {
    this->actionFunc = mv_dmaWait;
}

static void mv_dmaWait(BgMoriElevator* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->moriTexObjectSlot)) {
        if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
            if (play->roomCtx.curRoom.num == 2) {
                this->dyna.actor.world.pos.y = 73.0f;
                mvSet_stop(this);
            } else {
                // "Error: Forest Temple obj elevator Room setting is dangerous"
                PRINTF("Error : 森の神殿 obj elevator 部屋設定が危険(%s %d)\n", "../z_bg_mori_elevator.c", 371);
            }
        } else {
            mvSet_stop(this);
        }
        this->dyna.actor.draw = Bg_Mori_Elevator_actor_draw;
    }
}

void mvSet_dropDemo(BgMoriElevator* this) {
    this->actionFunc = mv_dropDemo;
}

void mv_dropDemo(BgMoriElevator* this, PlayState* play) {
    f32 distToTarget;

    add_calcNEW(&this->dyna.actor.velocity.y, 2.0f, 0.05f, 1.0f, 0.0f);
    distToTarget = add_calcNEW(&this->dyna.actor.world.pos.y, 73.0f, 0.08f, this->dyna.actor.velocity.y, 1.5f);
    if (fabsf(distToTarget) < 0.001f) {
        mvSet_stop(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_STOP);
    } else {
        SeSet_elevatorMove(this, distToTarget);
    }
}

void mvSet_rizeDemo(BgMoriElevator* this, PlayState* play) {
    this->actionFunc = mv_rizeDemo;
    makeOnepointDemo(play, 3230, 70, &this->dyna.actor, CAM_ID_MAIN);
    makeOnepointDemo(play, 1020, 15, &this->dyna.actor, CAM_ID_MAIN);
}

void mv_rizeDemo(BgMoriElevator* this, PlayState* play) {
    f32 distToTarget;

    add_calcNEW(&this->dyna.actor.velocity.y, 2.0f, 0.05f, 1.0f, 0.0f);
    distToTarget = add_calcNEW(&this->dyna.actor.world.pos.y, 233.0f, 0.08f, this->dyna.actor.velocity.y, 1.5f);
    if (fabsf(distToTarget) < 0.001f) {
        mvSet_stop(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_STOP);
    } else {
        SeSet_elevatorMove(this, distToTarget);
    }
}

static void mvSet_stop(BgMoriElevator* this) {
    this->actionFunc = mv_stop;
}

static void mv_stop(BgMoriElevator* this, PlayState* play) {
    s32 pad;

    if (checkRideElevator(this, play)) {
        if (play->roomCtx.curRoom.num == 2) {
            this->targetY = -779.0f;
            mvSet_updown(this);
        } else if (play->roomCtx.curRoom.num == 17) {
            this->targetY = 233.0f;
            mvSet_updown(this);
        } else {
            // "Error:Forest Temple obj elevator Room setting is dangerous(%s %d)"
            PRINTF("Error : 森の神殿 obj elevator 部屋設定が危険(%s %d)\n", "../z_bg_mori_elevator.c", 479);
        }
    } else if ((play->roomCtx.curRoom.num == 2) && (this->dyna.actor.world.pos.y < -275.0f)) {
        this->targetY = 233.0f;
        mvSet_updown(this);
    } else if ((play->roomCtx.curRoom.num == 17) && (-275.0f < this->dyna.actor.world.pos.y)) {
        this->targetY = -779.0f;
        mvSet_updown(this);
    } else if ((play->roomCtx.curRoom.num == 2) && Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6)) &&
               (this->unk_16C == 0)) {
        this->targetY = 73.0f;
        mvSet_dropDemo(this);
    } else if ((play->roomCtx.curRoom.num == 2) &&
               !Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6)) && (this->unk_16C != 0)) {
        this->targetY = 233.0f;
        mvSet_rizeDemo(this, play);
    }
}

static void mvSet_updown(BgMoriElevator* this) {
    this->actionFunc = mv_updown;
    this->dyna.actor.velocity.y = 0.0f;
}

static void mv_updown(BgMoriElevator* this, PlayState* play) {
    f32 distTo;

    add_calcNEW(&this->dyna.actor.velocity.y, 12.0f, 0.1f, 1.0f, 0.0f);
    distTo = add_calcNEW(&this->dyna.actor.world.pos.y, this->targetY, 0.1f, this->dyna.actor.velocity.y, 0.3f);
    if (fabsf(distTo) < 0.001f) {
        mvSet_stop(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_STOP);

    } else {
        SeSet_elevatorMove(this, distTo);
    }
}

void Bg_Mori_Elevator_actor_move(Actor* thisx, PlayState* play) {
    BgMoriElevator* this = (BgMoriElevator*)thisx;

    this->actionFunc(this, play);
    this->unk_170 = this->dyna.interactFlags;
    this->unk_16C = Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 0, 6));
}

void Bg_Mori_Elevator_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriElevator* this = (BgMoriElevator*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_mori_elevator.c", 575);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, play->objectCtx.slots[this->moriTexObjectSlot].segment);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mori_elevator.c", 580);
    gSPDisplayList(POLY_OPA_DISP++, gMoriElevatorDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_mori_elevator.c", 584);
}
