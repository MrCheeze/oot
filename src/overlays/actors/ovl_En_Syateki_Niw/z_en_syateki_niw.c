/*
 * File: z_en_syateki_niw.c
 * Overlay: ovl_En_Syateki_Niw
 * Description: Hopping Cucco
 */

#include "z_en_syateki_niw.h"

#include "libc64/math64.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "rand.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "z64.h"
#include "assets/objects/object_niw/object_niw.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Syateki_Niw_actor_ct(Actor* thisx, PlayState* play);
void En_Syateki_Niw_actor_dt(Actor* thisx, PlayState* play);
void En_Syateki_Niw_actor_move(Actor* thisx, PlayState* play);
void En_Syateki_Niw_actor_draw(Actor* thisx, PlayState* play);

static void mode_walk_init(EnSyatekiNiw* this, PlayState* play);
void niw_syateki_eff_move(EnSyatekiNiw* this, PlayState* play);
static void mode_run(EnSyatekiNiw* this, PlayState* play);
void niw_syateki_eff_disp(EnSyatekiNiw* this, PlayState* play);
void mode_jumpping_move(EnSyatekiNiw* this, PlayState* play);
static void mode_walk(EnSyatekiNiw* this, PlayState* play);
void mode_syateki_attack(EnSyatekiNiw* this, PlayState* play);
void mode_syateki_ende(EnSyatekiNiw* this, PlayState* play);

void niw_syateki_eff_set(EnSyatekiNiw* this, Vec3f* pos, Vec3f* vel, Vec3f* accel, f32 scale);

ActorProfile En_Syateki_Niw_Profile = {
    /**/ ACTOR_EN_SYATEKI_NIW,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_NIW,
    /**/ sizeof(EnSyatekiNiw),
    /**/ En_Syateki_Niw_actor_ct,
    /**/ En_Syateki_Niw_actor_dt,
    /**/ En_Syateki_Niw_actor_move,
    /**/ En_Syateki_Niw_actor_draw,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 10, 20, 4, { 0, 0, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_1, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 0, ICHAIN_STOP),
};

void En_Syateki_Niw_actor_ct(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 25.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gCuccoSkel, &gCuccoAnim, this->jointTable, this->morphTable, 16);

    this->minigameType = this->actor.params;
    if (this->minigameType < 0) {
        this->minigameType = SYATEKI_MINIGAME_ARCHERY;
    }

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
    if (this->minigameType == SYATEKI_MINIGAME_ARCHERY) {
        PRINTF("\n\n");
        // "Archery range chicken"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 射的場鶏 ☆☆☆☆☆ \n" VT_RST);
        Actor_set_scale(&this->actor, 0.01f);
    } else {
        PRINTF("\n\n");
        // "Bomb chicken"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ ボムにわ！ ☆☆☆☆☆ \n" VT_RST);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        Actor_set_scale(&this->actor, 0.01f);
    }

    this->initPos = this->actor.world.pos;
    this->targetPos = this->actor.world.pos;
    this->actionFunc = mode_walk_init;
}

void En_Syateki_Niw_actor_dt(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void move_parts_change(EnSyatekiNiw* this, PlayState* play, s16 animationType) {
    if (this->peckTimer == 0) {
        if (animationType == 0) {
            this->headRotXTarget = 0.0f;
        } else {
            this->headRotXTarget = -10000.0f;
        }

        this->headRotXState++;
        this->peckTimer = 3;
        if (!(this->headRotXState & 1)) {
            this->headRotXTarget = 0.0f;
            if (animationType == 0) {
                this->peckTimer = rnd_f(30.0f);
            }
        }
    }

    if (this->flapTimer == 0) {
        this->wingsRotState++;
        this->wingsRotState &= 1;
        switch (animationType) {
            case 0:
                this->leftWingRotXTarget = 0.0f;
                this->rightWingRotXTarget = 0.0f;
                break;

            case 1:
                this->flapTimer = 3;
                this->leftWingRotXTarget = 7000.0f;
                this->rightWingRotXTarget = 7000.0f;
                if (this->wingsRotState == 0) {
                    this->leftWingRotXTarget = 0.0f;
                    this->rightWingRotXTarget = 0.0f;
                }
                break;

            case 2:
                this->flapTimer = 2;
                this->rightWingRotXTarget = this->leftWingRotXTarget = -10000.0f;
                this->leftWingRotYTarget = this->rightWingRotYTarget = 25000.0f;
                this->leftWingRotZTarget = this->rightWingRotZTarget = 6000.0f;
                if (this->wingsRotState == 0) {
                    this->rightWingRotYTarget = 8000.0f;
                    this->leftWingRotYTarget = 8000.0f;
                }
                break;

            case 3:
                this->flapTimer = 2;
                this->rightWingRotYTarget = 10000.0f;
                this->leftWingRotYTarget = 10000.0f;
                if (this->wingsRotState == 0) {
                    this->rightWingRotYTarget = 3000.0f;
                    this->leftWingRotYTarget = 3000.0f;
                }
                break;

            case 4:
                this->peckTimer = this->timer1 = 5;
                break;

            case 5:
                this->flapTimer = 5;
                this->rightWingRotYTarget = 14000.0f;
                this->leftWingRotYTarget = 14000.0f;
                if (this->wingsRotState == 0) {
                    this->rightWingRotYTarget = 10000.0f;
                    this->leftWingRotYTarget = 10000.0f;
                }
                break;
        }
    }

    if (this->headRotXTarget != this->headRot.x) {
        add_calc2(&this->headRot.x, this->headRotXTarget, 0.5f, 4000.0f);
    }

    if (this->leftWingRotXTarget != this->leftWingRot.x) {
        add_calc2(&this->leftWingRot.x, this->leftWingRotXTarget, 0.8f, 7000.0f);
    }

    if (this->leftWingRotYTarget != this->leftWingRot.y) {
        add_calc2(&this->leftWingRot.y, this->leftWingRotYTarget, 0.8f, 7000.0f);
    }

    if (this->leftWingRotZTarget != this->leftWingRot.z) {
        add_calc2(&this->leftWingRot.z, this->leftWingRotZTarget, 0.8f, 7000.0f);
    }

    if (this->rightWingRotXTarget != this->rightWingRot.x) {
        add_calc2(&this->rightWingRot.x, this->rightWingRotXTarget, 0.8f, 7000.0f);
    }

    if (this->rightWingRotYTarget != this->rightWingRot.y) {
        add_calc2(&this->rightWingRot.y, this->rightWingRotYTarget, 0.8f, 7000.0f);
    }

    if (this->rightWingRotZTarget != this->rightWingRot.z) {
        add_calc2(&this->rightWingRot.z, this->rightWingRotZTarget, 0.8f, 7000.0f);
    }
}

static void mode_walk_init(EnSyatekiNiw* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gCuccoAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gCuccoAnim), ANIMMODE_LOOP,
                     -10.0f);
    if (this->minigameType != SYATEKI_MINIGAME_ARCHERY) {
        Actor_set_scale(&this->actor, this->scale);
    }

    this->actionFunc = mode_walk;
}

static void mode_walk(EnSyatekiNiw* this, PlayState* play) {
    Vec3f dustVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f dustAccel = { 0.0f, 0.2f, 0.0f };
    Color_RGBA8 dustPrimColor = { 0, 0, 0, 255 };
    Color_RGBA8 dustEnvColor = { 0, 0, 0, 255 };
    Vec3f dustPos;
    f32 posZDiff;
    f32 posZMod;
    f32 posXMod;
    f32 posXDiff;
    s16 animationType;

    if (this->isFalling && (this->minigameType == SYATEKI_MINIGAME_ARCHERY) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->isFalling = false;
        this->actionFunc = mode_jumpping_move;
        return;
    }

    animationType = 0;
    if ((this->movementTimer == 0) && (this->hopTimer == 0)) {
        this->targetPosTimer++;
        if (this->targetPosTimer >= 8) {
            this->movementTimer = rnd_f(30.0f);
            this->targetPosTimer = rnd_f(3.99f);

            switch (this->minigameType) {
                case SYATEKI_MINIGAME_ARCHERY:
                    posXMod = rnd_fx(100.0f);
                    if (posXMod < 0.0f) {
                        posXMod -= 100.0f;
                    } else {
                        posXMod += 100.0f;
                    }

                    posZMod = rnd_fx(100.0f);
                    if (posZMod < 0.0f) {
                        posZMod -= 100.0f;
                    } else {
                        posZMod += 100.0f;
                    }

                    this->targetPos.x = this->initPos.x + posXMod;
                    this->targetPos.z = this->initPos.z + posZMod;

                    if (this->targetPos.x < -150.0f) {
                        this->targetPos.x = -150.0f;
                    }

                    if (this->targetPos.x > 150.0f) {
                        this->targetPos.x = 150.0f;
                    }

                    if (this->targetPos.z < -60.0f) {
                        this->targetPos.z = -60.0f;
                    }

                    if (this->targetPos.z > -40.0f) {
                        this->targetPos.z = -40.0f;
                    }
                    break;

                case SYATEKI_MINIGAME_ALLEY:
                    posXMod = rnd_fx(50.0f);
                    if (posXMod < 0.0f) {
                        posXMod -= 50.0f;
                    } else {
                        posXMod += 50.0f;
                    }

                    posZMod = rnd_fx(30.0f);
                    if (posZMod < 0.0f) {
                        posZMod -= 30.0f;
                    } else {
                        posZMod += 30.0f;
                    }

                    this->targetPos.x = this->initPos.x + posXMod;
                    this->targetPos.z = this->initPos.z + posZMod;
                    break;
            }
        } else {
            this->hopTimer = 4;
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                this->actor.velocity.y = 2.5f;
                if ((rnd_f(10.0f) < 1.0f) && (this->minigameType == SYATEKI_MINIGAME_ARCHERY)) {
                    this->hopTimer = 12;
                    this->actor.velocity.y = 10.0f;
                }
            }
        }
    }
    if (this->hopTimer != 0) {
        animationType = 1;
        add_calc2(&this->actor.world.pos.x, this->targetPos.x, 1.0f, this->posRotStep.y);
        add_calc2(&this->actor.world.pos.z, this->targetPos.z, 1.0f, this->posRotStep.y);
        add_calc2(&this->posRotStep.y, 3.0f, 1.0f, 0.3f);
        posXDiff = this->targetPos.x - this->actor.world.pos.x;
        posZDiff = this->targetPos.z - this->actor.world.pos.z;

        if (fabsf(posXDiff) < 10.0f) {
            posXDiff = 0;
        }

        if (fabsf(posZDiff) < 10.0f) {
            posZDiff = 0.0f;
        }

        if ((posXDiff == 0.0f) && (posZDiff == 0.0f)) {
            this->hopTimer = 0;
            this->targetPosTimer = 7;
        }

        add_calc_short_angle2(&this->actor.world.rot.y, RAD_TO_BINANG(fatan2(posXDiff, posZDiff)), 3,
                           this->posRotStep.z, 0);
        add_calc2(&this->posRotStep.z, 10000.0f, 1.0f, 1000.0f);
    }

    if (this->sootTimer == 0) {
        move_parts_change(this, play, animationType);
    } else if ((play->gameplayFrames % 4) == 0) { // draw smoke from bombchu hit
        dustVelocity.y = rnd_fx(5.0f);
        dustAccel.y = 0.2f;
        dustPos = this->actor.world.pos;
        Effect_SS_Dust_sc_cl_co_ct(play, &dustPos, &dustVelocity, &dustAccel, &dustPrimColor, &dustEnvColor, 600, 40, 30);
    }
}

void mode_jumpping_move(EnSyatekiNiw* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gCuccoAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gCuccoAnim), ANIMMODE_LOOP,
                     -10.0f);
    this->rightWingRotZTarget = 6000.0f;
    this->unkArcheryFloat = -10000.0f;
    this->rightWingRot.z = 6000.0f;
    this->rightWingRot.y = 10000.0f;
    this->actionFunc = mode_syateki_attack;
    this->leftWingRot.z = 6000.0f;
    this->leftWingRotZTarget = 6000.0f;
    this->rightWingRot.x = -10000.0f;
    this->rightWingRotXTarget = -10000.0f;
    this->leftWingRot.y = -10000.0f;
    this->leftWingRot.x = -10000.0f;
    this->leftWingRotXTarget = -10000.0f;
}

void mode_syateki_attack(EnSyatekiNiw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 rotYTargetOffset = 0.0f;

    player->actor.freezeTimer = 10;
    switch (this->archeryState) {
        case 0:
            this->archeryAnimationType = 2;
            this->posRotStep.y = 0.0f;
            this->archeryState = 1;
            break;

        case 1:
            this->actor.speed = 2.0f;
            if (this->hopTimer == 0) {
                this->hopTimer = 3;
                this->actor.velocity.y = 3.5f;
            }

            if (this->archeryTimer == 0) {
                this->rotYFlip++;
                this->rotYFlip &= 1;
                this->archeryTimer = 5;
            }

            rotYTargetOffset = (this->rotYFlip == 0) ? 5000.0f : -5000.0f;
            if (this->actor.world.pos.z > 100.0f) {
                this->actor.speed = 2.0f;
                this->actor.gravity = -0.3f;
                this->actor.velocity.y = 5.0f;
                this->archeryState = 2;
            }
            break;

        case 2:
            if ((player->actor.world.pos.z - 40.0f) < this->actor.world.pos.z) {
                this->actor.speed = 0.0f;
            }

            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.world.pos.z > 110.0f)) {
                this->actor.velocity.y = 0.0f;
                this->actor.gravity = 0.0f;
                this->leftWingRotZTarget = 0.0f;
                this->rightWingRotZTarget = 0.0f;
                this->rightWingRotYTarget = 0.0f;
                this->leftWingRotYTarget = 0.0f;
                this->unkArcheryFloat = 0.0f;
                this->actor.speed = 0.5f;
                this->peckTimer = this->timer1 = 0;
                this->headRotXState = this->unk_290 = 0;
                this->archeryAnimationType = 1;
                this->archeryState = 3;
            }
            break;

        case 3:
            if ((player->actor.world.pos.z - 50.0f) < this->actor.world.pos.z) {
                this->actor.speed = 0.0f;
                this->cluckTimer = 60;
                this->archeryTimer = 20;
                this->headRotXTarget = 10000.0f;
                this->archeryState = 4;
            }
            break;

        case 4:
            if (this->archeryTimer == 0) {
                this->archeryAnimationType = 4;
                this->headRotXTarget = 5000.0f;
                this->leftWingRotXTarget = 0.0f;
                this->rightWingRotXTarget = 0.0f;
                this->leftWingRotZTarget = 0.0f;
                this->rightWingRotZTarget = 0.0f;
                this->leftWingRotYTarget = 14000.0f;
                this->rightWingRotYTarget = 14000.0f;
                Actor_SE_set(&this->actor, NA_SE_EV_CHICKEN_CRY_M);
                this->peckTimer = this->timer1 = this->archeryTimer = 30;
                this->archeryState = 5;
            }
            break;

        case 5:
            if (this->archeryTimer == 1) {
                this->flapTimer = 0;
                this->archeryAnimationType = 5;
                this->timer1 = this->flapTimer;
                this->peckTimer = this->flapTimer;
                this->actor.speed = 1.0f;
            }

            if ((this->archeryTimer == 0) && ((player->actor.world.pos.z - 30.0f) < this->actor.world.pos.z)) {
                Nai_FxFlagEntry(NA_SE_VO_LI_DOWN, &this->actor.projectedPos, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
                this->movementTimer = 20;
                this->archeryState = 6;
                this->actor.speed = 0.0f;
            }
            break;

        case 6:
            if (this->movementTimer == 1) {
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->nextEntranceIndex = z_common_data.save.entranceIndex;
                play->shootingGalleryStatus = 0;
                player->actor.freezeTimer = 20;
                this->movementTimer = 20;
                this->actionFunc = mode_syateki_ende;
            }
            break;
    }

    add_calc_short_angle2(&this->actor.world.rot.y,
                       RAD_TO_BINANG(fatan2(player->actor.world.pos.x - this->actor.world.pos.x,
                                                  player->actor.world.pos.z - this->actor.world.pos.z)) +
                           rotYTargetOffset,
                       5, this->posRotStep.y, 0);
    add_calc2(&this->posRotStep.y, 3000.0f, 1.0f, 500.0f);
    if (this->archeryAnimationType == 2) {
        this->peckTimer = this->timer1 = 10;
    }

    move_parts_change(this, play, this->archeryAnimationType);
}

void mode_syateki_ende(EnSyatekiNiw* this, PlayState* play) {
    if (this->movementTimer == 1) {
        z_common_data.timerState = TIMER_STATE_OFF;
    }
}

void mode_run_wait(EnSyatekiNiw* this, PlayState* play) {
    s16 screenX;
    s16 screenY;

    Actor_world_to_eye(&this->actor, this->focusYOffset);
    Actor_display_position_set(play, &this->actor, &screenX, &screenY);
    if ((this->actor.projectedPos.z > 200.0f) && (this->actor.projectedPos.z < 800.0f) && (screenX > 0) &&
        (screenX < SCREEN_WIDTH) && (screenY > 0) && (screenY < SCREEN_HEIGHT)) {
        this->actor.speed = 5.0f;
        this->rotYFlip = rnd_f(1.99f);
        this->removeStateYaw = rnd_fx(8000.0f) + -10000.0f;
        this->cluckTimer = 30;
        this->movementTimer = 100;
        this->actionFunc = mode_run;
    }
}

static void mode_run(EnSyatekiNiw* this, PlayState* play) {
    s32 pad;
    f32 rotYTargetOffset;
    s16 screenX;
    s16 screenY;
    f32 rotYTarget;

    Actor_world_to_eye(&this->actor, this->focusYOffset);
    Actor_display_position_set(play, &this->actor, &screenX, &screenY);
    if ((this->movementTimer == 0) || (this->actor.projectedPos.z < -70.0f) || (screenX < 0) ||
        (screenX > SCREEN_WIDTH) || (screenY < 0) || (screenY > SCREEN_HEIGHT)) {
        Actor_delete(&this->actor);
        return;
    }

    this->spawnFeathers = true;
    if (this->hopTimer == 0) {
        this->rotYFlip++;
        this->rotYFlip &= 1;
        this->hopTimer = (s16)rnd_fx(4.0f) + 5;
        if ((rnd_f(5.0f) < 1.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            this->actor.velocity.y = 4.0f;
        }
    }

    rotYTargetOffset = (this->rotYFlip == 0) ? 5000.0f : -5000.0f;
    rotYTarget = this->removeStateYaw + rotYTargetOffset;
    add_calc_short_angle2(&this->actor.world.rot.y, rotYTarget, 3, this->posRotStep.y, 0);
    add_calc2(&this->posRotStep.y, 3000.0f, 1.0f, 500.0f);
    move_parts_change(this, play, 2);
}

void En_Syateki_Niw_damage_proc(EnSyatekiNiw* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        switch (this->minigameType) {
            case SYATEKI_MINIGAME_ARCHERY:
                if (!this->isFalling) {
                    this->cluckTimer = 30;
                    Actor_SE_set(&this->actor, NA_SE_EV_CHICKEN_CRY_A);
                    this->isFalling = true;
                    this->spawnFeathers = true;
                    this->actionFunc = mode_jumpping_move;
                    this->actor.gravity = -3.0f;
                }
                break;

            case SYATEKI_MINIGAME_ALLEY:
                this->cluckTimer = 30;
                this->unkAlleyHitByte = 1;
                Actor_SE_set(&this->actor, NA_SE_EV_CHICKEN_CRY_A);
                this->sootTimer = 100;
                this->spawnFeathers = true;
                this->movementTimer = this->sootTimer;
                break;
        }
    }
}

void En_Syateki_Niw_actor_move(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;
    s32 pad;
    s16 i;
    Vec3f sp90 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp84 = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    Vec3f vel;
    Vec3f accel;

    if (1) {}
    if (1) {}
    if (1) {}

    niw_syateki_eff_move(this, play);
    this->lifetime++;
    if (this->peckTimer != 0) {
        this->peckTimer--;
    }

    if (this->flapTimer != 0) {
        this->flapTimer--;
    }

    if (this->archeryTimer != 0) {
        this->archeryTimer--;
    }

    if (this->hopTimer != 0) {
        this->hopTimer--;
    }

    if (this->movementTimer != 0) {
        this->movementTimer--;
    }

    if (this->cluckTimer != 0) {
        this->cluckTimer--;
    }

    if (this->sootTimer != 0) {
        this->sootTimer--;
    }

    this->actor.shape.rot = this->actor.world.rot;
    this->actor.shape.shadowScale = 15.0f;

    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);

    if (this->spawnFeathers) {
        for (i = 0; i < 20; i++) {
            pos.x = rnd_fx(10.0f) + this->actor.world.pos.x;
            pos.y = rnd_fx(10.0f) + (this->actor.world.pos.y + 20.0f);
            pos.z = rnd_fx(10.0f) + this->actor.world.pos.z;
            vel.x = rnd_fx(3.0f);
            vel.y = (rnd_f(2.0f) * 0.5f) + 2.0f;
            vel.z = rnd_fx(3.0f);
            accel.z = accel.x = 0.0f;
            accel.y = -0.15f;
            niw_syateki_eff_set(this, &pos, &vel, &accel, rnd_f(8.0f) + 8.0f);
        }

        this->spawnFeathers = false;
    }

    En_Syateki_Niw_damage_proc(this, play);
    if (this->cluckTimer == 0) {
        if (this->actionFunc == mode_walk) {
            this->cluckTimer = 300;
            Actor_SE_set(&this->actor, NA_SE_EV_CHICKEN_CRY_N);
        } else {
            this->cluckTimer = 30;
            Actor_SE_set(&this->actor, NA_SE_EV_CHICKEN_CRY_A);
        }
    }

    // here, "i" doubles as boolean value.
    i = false;
    switch (this->minigameType) {
        case SYATEKI_MINIGAME_ARCHERY:
            if (play->shootingGalleryStatus != 0) {
                i = true;
            }
            break;

        case SYATEKI_MINIGAME_ALLEY:
            i = true;
            break;
    }

    if (i) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 En_Syateki_Niw_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;
    Vec3f unusedZeroVec = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == 13) {
        rot->y += (s16)this->headRot.x;
    }

    if (limbIndex == 11) {
        rot->x += (s16)this->rightWingRot.z;
        rot->y += (s16)this->rightWingRot.y;
        rot->z += (s16)this->rightWingRot.x;
    }

    if (limbIndex == 7) {
        rot->x += (s16)this->leftWingRot.z;
        rot->y += (s16)this->leftWingRot.y;
        rot->z += (s16)this->leftWingRot.x;
    }

    return false;
}

void En_Syateki_Niw_actor_draw(Actor* thisx, PlayState* play) {
    EnSyatekiNiw* this = (EnSyatekiNiw*)thisx;
    Color_RGBA8 sootShade = { 0, 0, 0, 255 };

    if (this->actionFunc != mode_run_wait) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        if (this->sootTimer != 0) {
            Eff_Set_Fog2(play, &sootShade, 0, 20);
        }

        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              En_Syateki_Niw_draw_sub, NULL, this);
        Eff_Off_Fog(play);
        niw_syateki_eff_disp(this, play);
    }
}

void niw_syateki_eff_set(EnSyatekiNiw* this, Vec3f* pos, Vec3f* vel, Vec3f* accel, f32 scale) {
    s16 i;
    EnSyatekiNiwEffect* effect = &this->effects[0];

    for (i = 0; i < EN_SYATEKI_NIW_EFFECT_COUNT; i++, effect++) {
        if (effect->state == 0) {
            effect->state = 1;
            effect->pos = *pos;
            effect->vel = *vel;
            effect->accel = *accel;
            effect->timer = 0;
            effect->scale = (scale / 1000.0f);
            effect->lifespan = (s16)rnd_f(20.0f) + 40;
            effect->rotPulse = rnd_f(1000.0f);
            return;
        }
    }
}

void niw_syateki_eff_move(EnSyatekiNiw* this, PlayState* play) {
    s16 i;
    EnSyatekiNiwEffect* effect = &this->effects[0];

    for (i = 0; i < EN_SYATEKI_NIW_EFFECT_COUNT; i++, effect++) {
        if (effect->state != 0) {
            effect->pos.x += effect->vel.x;
            effect->pos.y += effect->vel.y;
            effect->pos.z += effect->vel.z;
            effect->timer++;
            effect->vel.x += effect->accel.x;
            effect->vel.y += effect->accel.y;
            effect->vel.z += effect->accel.z;
            if (effect->state == 1) {
                effect->rotPulse++;
                add_calc2(&effect->vel.x, 0.0f, 1.0f, 0.05f);
                add_calc2(&effect->vel.z, 0.0f, 1.0f, 0.05f);
                if (effect->vel.y < -0.5f) {
                    effect->vel.y = 0.5f;
                }

                effect->rot = (sin_s(effect->rotPulse * 3000) * M_PI) * 0.2f;
                if (effect->lifespan < effect->timer) {
                    effect->state = 0;
                }
            }
        }
    }
}

void niw_syateki_eff_disp(EnSyatekiNiw* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;
    EnSyatekiNiwEffect* effect;
    u8 materialFlag = 0;

    effect = &this->effects[0];

    OPEN_DISPS(gfxCtx, "../z_en_syateki_niw.c", 1234);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < EN_SYATEKI_NIW_EFFECT_COUNT; i++, effect++) {
        if (effect->state == 1) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gCuccoEffectFeatherMaterialDL);
                materialFlag++;
            }

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            Matrix_rotateZ(effect->rot, MTXMODE_APPLY);
            Matrix_translate(0.0f, -1000.0f, 0.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_syateki_niw.c", 1251);
            gSPDisplayList(POLY_XLU_DISP++, gCuccoEffectFeatherModelDL);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_en_syateki_niw.c", 1257);
}
