/*
 * File: z_boss_fd.c
 * Overlay: ovl_Boss_Fd
 * Description: Volvagia, flying form
 */

#include "z_boss_fd.h"
#include "assets/objects/object_fd/object_fd.h"
#include "overlays/actors/ovl_En_Vb_Ball/z_en_vb_ball.h"
#include "overlays/actors/ovl_Bg_Vb_Sima/z_bg_vb_sima.h"
#include "overlays/actors/ovl_Boss_Fd2/z_boss_fd2.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

typedef enum BossFdIntroFlyState {
    /* 0 */ INTRO_FLY_EMERGE,
    /* 1 */ INTRO_FLY_HOLE,
    /* 2 */ INTRO_FLY_CAMERA,
    /* 3 */ INTRO_FLY_RETRAT
} BossFdIntroFlyState;

typedef enum BossFdManeIndex {
    /* 0 */ MANE_CENTER,
    /* 1 */ MANE_RIGHT,
    /* 2 */ MANE_LEFT
} BossFdManeIndex;

typedef enum BossFdEyeState {
    /* 0 */ EYE_OPEN,
    /* 1 */ EYE_HALF,
    /* 2 */ EYE_CLOSED
} BossFdEyeState;

void Boss_Fd_actor_ct(Actor* thisx, PlayState* play);
void Boss_Fd_actor_dt(Actor* thisx, PlayState* play);
void Boss_Fd_actor_move(Actor* thisx, PlayState* play);
void Boss_Fd_actor_draw(Actor* thisx, PlayState* play);

static void mode_fly_init(BossFd* this, PlayState* play);
static void mode_fly(BossFd* this, PlayState* play);
static void mode_wait(BossFd* this, PlayState* play);
static void Boss_Eff_move(BossFd* this, PlayState* play);
void VB_draw(PlayState* play, BossFd* this);

ActorProfile Boss_Fd_Profile = {
    /**/ ACTOR_BOSS_FD,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_FD,
    /**/ sizeof(BossFd),
    /**/ Boss_Fd_actor_ct,
    /**/ Boss_Fd_actor_dt,
    /**/ Boss_Fd_actor_move,
    /**/ Boss_Fd_actor_draw,
};

#include "z_boss_fd.inc.c"

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_VOLVAGIA, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 0, ICHAIN_STOP),
};

void Effect_vb_hinoko_ct_IN(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 150; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_EMBER;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->alpha = 255;
            effect->timer1 = (s16)rnd_f(10.0f);
            break;
        }
    }
}

void Effect_Hahen_ct_IN(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 150; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DEBRIS;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->vFdFxRotX = rnd_f(100.0f);
            effect->vFdFxRotY = rnd_f(100.0f);
            break;
        }
    }
}

void Effect_vb_smoke_ct_IN(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 150; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DUST;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->timer2 = 0;
            effect->scale = scale / 400.0f;
            break;
        }
    }
}

void Effect_vb_fire_ct_IN(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale,
                            s16 alpha, s16 kbAngle) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_FIRE_BREATH;
            effect->timer1 = 0;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->pos.x -= effect->velocity.x;
            effect->pos.y -= effect->velocity.y;
            effect->pos.z -= effect->velocity.z;
            effect->vFdFxScaleMod = 0.0f;
            effect->alpha = alpha;
            effect->vFdFxYStop = rnd_f(10.0f);
            effect->timer2 = 0;
            effect->scale = scale / 400.0f;
            effect->kbAngle = kbAngle;
            break;
        }
    }
}

void demo_spd_set(BossFd* this, f32 velFactor) {
    this->subCamEyeVel.x = fabsf(this->subCamEye.x - this->subCamEyeNext.x) * velFactor;
    this->subCamEyeVel.y = fabsf(this->subCamEye.y - this->subCamEyeNext.y) * velFactor;
    this->subCamEyeVel.z = fabsf(this->subCamEye.z - this->subCamEyeNext.z) * velFactor;
    this->subCamAtVel.x = fabsf(this->subCamAt.x - this->subCamAtNext.x) * velFactor;
    this->subCamAtVel.y = fabsf(this->subCamAt.y - this->subCamAtNext.y) * velFactor;
    this->subCamAtVel.z = fabsf(this->subCamAt.z - this->subCamAtNext.z) * velFactor;
}

static void demo_camera_set(BossFd* this, PlayState* play) {
    if (this->subCamId != SUB_CAM_ID_DONE) {
        add_calc2(&this->subCamEye.x, this->subCamEyeNext.x, this->subCamEyeMaxVelFrac.x,
                       this->subCamEyeVel.x * this->subCamVelFactor);
        add_calc2(&this->subCamEye.y, this->subCamEyeNext.y, this->subCamEyeMaxVelFrac.y,
                       this->subCamEyeVel.y * this->subCamVelFactor);
        add_calc2(&this->subCamEye.z, this->subCamEyeNext.z, this->subCamEyeMaxVelFrac.z,
                       this->subCamEyeVel.z * this->subCamVelFactor);
        add_calc2(&this->subCamAt.x, this->subCamAtNext.x, this->subCamAtMaxVelFrac.x,
                       this->subCamAtVel.x * this->subCamVelFactor);
        add_calc2(&this->subCamAt.y, this->subCamAtNext.y, this->subCamAtMaxVelFrac.y,
                       this->subCamAtVel.y * this->subCamVelFactor);
        add_calc2(&this->subCamAt.z, this->subCamAtNext.z, this->subCamAtMaxVelFrac.z,
                       this->subCamAtVel.z * this->subCamVelFactor);
        add_calc2(&this->subCamVelFactor, 1.0f, 1.0f, this->subCamAccel);
        this->subCamAt.y += this->subCamAtYOffset;
        Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
        add_calc0(&this->subCamAtYOffset, 1.0f, 0.1f);
    }
}

void Boss_Fd_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BossFd* this = (BossFd*)thisx;
    s16 i;

    Actor_Environment_sw_On(play, 0x14);
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BG_VB_SIMA, 680.0f, -100.0f, 0.0f, 0, 0, 0, 100);
    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, NULL, 0.0f);
    Actor_set_scale(&this->actor, 0.05f);
    Skeleton_Info2_M_ct(play, &this->skelAnimeHead, &gVolvagiaHeadSkel, &gVolvagiaHeadEmergeAnim, NULL, NULL, 0);
    Skeleton_Info2_M_ct(play, &this->skelAnimeRightArm, &gVolvagiaRightArmSkel, &gVolvagiaRightArmEmergeAnim, NULL, NULL, 0);
    Skeleton_Info2_M_ct(play, &this->skelAnimeLeftArm, &gVolvagiaLeftArmSkel, &gVolvagiaLeftArmEmergeAnim, NULL, NULL, 0);
    this->introState = BFD_CS_WAIT;
    if (this->introState == BFD_CS_NONE) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_FIRE_BOSS);
    }

    this->actor.world.pos.x = this->actor.world.pos.z = 0.0f;
    this->actor.world.pos.y = -200.0f;
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &FdAcOcInfoJntSphData, this->elements);

    for (i = 0; i < 100; i++) {
        this->bodySegsPos[i].x = this->actor.world.pos.x;
        this->bodySegsPos[i].y = this->actor.world.pos.y;
        this->bodySegsPos[i].z = this->actor.world.pos.z;
        if (i < 30) {
            this->centerMane.pos[i].x = this->actor.world.pos.x;
            this->centerMane.pos[i].y = this->actor.world.pos.y;
            this->centerMane.pos[i].z = this->actor.world.pos.z;
        }
    }

    this->actor.colChkInfo.health = 24;
    this->skinSegments = 18;
    if (this->introState == BFD_CS_NONE) {
        this->actionFunc = mode_wait;
    } else {
        mode_fly_init(this, play);
    }

    if (Actor_Environment_room_clear_Check(play, play->roomCtx.curRoom.num)) {
        Actor_delete(&this->actor);
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, 0.0f, 100.0f, 0.0f, 0, 0, 0,
                           WARP_DUNGEON_ADULT);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, 0.0f, 100.0f, 200.0f, 0, 0, 0, 0);
    } else {
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_FD2, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, this->introState);
    }
}

void Boss_Fd_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BossFd* this = (BossFd*)thisx;

    Skeleton_Info_dt(&this->skelAnimeHead, play);
    Skeleton_Info_dt(&this->skelAnimeRightArm, play);
    Skeleton_Info_dt(&this->skelAnimeLeftArm, play);
    ClObjJntSph_dt_nzf(play, &this->collider);
}

s32 fd_angle_check(BossFd* this) {
    return ABS((s16)(this->actor.yawTowardsPlayer - this->actor.world.rot.y)) < 0x2000;
}

static void mode_fly_init(BossFd* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop(&this->skelAnimeHead, &gVolvagiaHeadEmergeAnim);
    Skeleton_Info2_init_standard_stop(&this->skelAnimeRightArm, &gVolvagiaRightArmEmergeAnim);
    Skeleton_Info2_init_standard_stop(&this->skelAnimeLeftArm, &gVolvagiaLeftArmEmergeAnim);
    this->actionFunc = mode_fly;
    this->fwork[BFD_TURN_RATE_MAX] = 1000.0f;
}

static Vec3f hole_pos[] = {
    { 0.0f, 90.0f, -243.0f },    { 0.0f, 90.0f, 0.0f },    { 0.0f, 90.0f, 243.0f },
    { -243.0f, 90.0f, -243.0f }, { -243.0f, 90.0f, 0.0f }, { -243.0f, 90.0f, 243.0f },
    { 243.0f, 90.0f, -243.0f },  { 243.0f, 90.0f, 0.0f },  { 243.0f, 90.0f, 243.0f },
};

static Vec3f roof_pos[] = {
    { 0.0f, 900.0f, -243.0f }, { 243.0, 900.0f, -100.0f },  { 243.0f, 900.0f, 100.0f },
    { 0.0f, 900.0f, 243.0f },  { -243.0f, 900.0f, 100.0f }, { -243.0, 900.0f, -100.0f },
};

static void mode_fly(BossFd* this, PlayState* play) {
    u8 sp1CF = false;
    s16 i1;
    s16 i2;
    s16 i3;
    f32 dx;
    f32 dy;
    f32 dz;
    Player* player = GET_PLAYER(play);
    f32 angleToTarget;
    f32 pitchToTarget;
    Vec3f* holePosition1;
    f32 temp_y;
    f32 temp_x;
    f32 temp_z;

    Skeleton_Info2_anime_play(&this->skelAnimeHead);
    Skeleton_Info2_anime_play(&this->skelAnimeRightArm);
    Skeleton_Info2_anime_play(&this->skelAnimeLeftArm);
    dx = this->targetPosition.x - this->actor.world.pos.x;
    dy = this->targetPosition.y - this->actor.world.pos.y;
    dz = this->targetPosition.z - this->actor.world.pos.z;
    dx += sin_s((2096.0f + this->fwork[BFD_FLY_WOBBLE_RATE]) * this->work[BFD_MOVE_TIMER]) *
          this->fwork[BFD_FLY_WOBBLE_AMP];
    dy += sin_s((1096.0f + this->fwork[BFD_FLY_WOBBLE_RATE]) * this->work[BFD_MOVE_TIMER]) *
          this->fwork[BFD_FLY_WOBBLE_AMP];
    dz += sin_s((1796.0f + this->fwork[BFD_FLY_WOBBLE_RATE]) * this->work[BFD_MOVE_TIMER]) *
          this->fwork[BFD_FLY_WOBBLE_AMP];
    angleToTarget = RAD_TO_BINANG(fatan2(dx, dz));
    pitchToTarget = RAD_TO_BINANG(fatan2(dy, sqrtf(SQ(dx) + SQ(dz))));

    PRINTF("MODE %d\n", this->work[BFD_ACTION_STATE]);

    add_calc2(&this->fwork[BFD_BODY_PULSE], 0.1f, 1.0f, 0.02);

    //                                        Boss Intro Cutscene

    if (this->introState != BFD_CS_NONE) {
        Player* player2 = GET_PLAYER(play);
        Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

        switch (this->introState) {
            case BFD_CS_WAIT:
                this->fogMode = 3;
                this->targetPosition.x = 0.0f;
                this->targetPosition.y = -110.0f;
                this->targetPosition.z = 0.0;
                this->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
                this->work[BFD_ACTION_STATE] = BOSSFD_WAIT_INTRO;
                if ((fabsf(player2->actor.world.pos.z) < 80.0f) &&
                    (fabsf(player2->actor.world.pos.x - 340.0f) < 60.0f)) {

                    this->introState = BFD_CS_START;
                    Demo_play_start(play, &play->csCtx);
                    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
                    this->subCamId = Gama_play_make_camera(play);
                    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
                    Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
                    player2->actor.world.pos.x = 380.0f;
                    player2->actor.world.pos.y = 100.0f;
                    player2->actor.world.pos.z = 0.0f;
                    player2->actor.shape.rot.y = player2->actor.world.rot.y = -0x4000;
                    player2->actor.speed = 0.0f;
                    this->subCamEye.x = player2->actor.world.pos.x - 70.0f;
                    this->subCamEye.y = player2->actor.world.pos.y + 40.0f;
                    this->subCamEye.z = player2->actor.world.pos.z + 70.0f;
                    this->subCamAt.x = player2->actor.world.pos.x;
                    this->subCamAt.y = player2->actor.world.pos.y + 30.0f;
                    this->subCamAt.z = player2->actor.world.pos.z;
                    this->subCamEyeNext.x = player2->actor.world.pos.x - 50.0f + 18.0f;
                    this->subCamEyeNext.y = player2->actor.world.pos.y + 40;
                    this->subCamEyeNext.z = player2->actor.world.pos.z + 50.0f - 18.0f;
                    this->subCamAtNext.x = player2->actor.world.pos.x;
                    this->subCamAtNext.y = player2->actor.world.pos.y + 50.0f;
                    this->subCamAtNext.z = player2->actor.world.pos.z;
                    demo_spd_set(this, 1.0f);
                    this->subCamAtMaxVelFrac.x = this->subCamAtMaxVelFrac.y = this->subCamAtMaxVelFrac.z = 0.05f;
                    this->subCamEyeMaxVelFrac.x = this->subCamEyeMaxVelFrac.y = this->subCamEyeMaxVelFrac.z = 0.05f;
                    this->timers[0] = 0;
                    this->subCamVelFactor = 0.0f;
                    this->subCamAccel = 0.0f;
                    if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_VOLVAGIA_BATTLE)) {
                        this->introState = BFD_CS_EMERGE;
                        this->subCamEyeNext.x = player2->actor.world.pos.x + 100.0f + 300.0f - 600.0f;
                        this->subCamEyeNext.y = player2->actor.world.pos.y + 100.0f - 50.0f;
                        this->subCamEyeNext.z = player2->actor.world.pos.z + 200.0f - 150.0f;
                        this->subCamAtNext.x = 0.0f;
                        this->subCamAtNext.y = 120.0f;
                        this->subCamAtNext.z = 0.0f;
                        demo_spd_set(this, 0.5f);
                        this->subCamEyeMaxVelFrac.x = this->subCamEyeMaxVelFrac.y = this->subCamEyeMaxVelFrac.z = 0.1f;
                        this->subCamAtMaxVelFrac.x = this->subCamAtMaxVelFrac.y = this->subCamAtMaxVelFrac.z = 0.1f;
                        this->subCamAccel = 0.005f;
                        this->timers[0] = 0;
                        this->holeIndex = 1;
                        this->targetPosition.x = hole_pos[this->holeIndex].x;
                        this->targetPosition.y = hole_pos[this->holeIndex].y - 200.0f;
                        this->targetPosition.z = hole_pos[this->holeIndex].z;
                        this->timers[0] = 50;
                        this->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
                        this->actor.world.rot.x = 0x4000;
                        this->work[BFD_MOVE_TIMER] = 0;
                        this->timers[3] = 250;
                        this->timers[2] = 470;
                        this->fwork[BFD_FLY_SPEED] = 5.0f;
                    }
                }
                break;
            case BFD_CS_START:
                if (this->timers[0] == 0) {
                    this->subCamAccel = 0.0010000002f;
                    this->timers[0] = 100;
                    this->introState = BFD_CS_LOOK_LINK;
                }
                FALLTHROUGH;
            case BFD_CS_LOOK_LINK:
                player2->actor.world.pos.x = 380.0f;
                player2->actor.world.pos.y = 100.0f;
                player2->actor.world.pos.z = 0.0f;
                player2->actor.speed = 0.0f;
                player2->actor.shape.rot.y = player2->actor.world.rot.y = -0x4000;
                if (this->timers[0] == 50) {
                    this->fogMode = 1;
                }
                if (this->timers[0] < 50) {
                    Nai_FxFlagEntry(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &this->actor.projectedPos, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                    this->subCamAtYOffset = cos_s(this->work[BFD_MOVE_TIMER] * 0x8000) * this->subCamShake;
                    add_calc2(&this->subCamShake, 2.0f, 1.0f, 0.8 * 0.01f);
                }
                if (this->timers[0] == 40) {
                    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_19);
                }
                if (this->timers[0] == 0) {
                    this->introState = BFD_CS_LOOK_GROUND;
                    this->subCamAtNext.y = player2->actor.world.pos.y + 10.0f;
                    this->subCamAtMaxVelFrac.y = 0.2f;
                    this->subCamVelFactor = 0.0f;
                    this->subCamAccel = 0.02f;
                    this->timers[0] = 70;
                    this->work[BFD_MOVE_TIMER] = 0;
                }
                break;
            case BFD_CS_LOOK_GROUND:
                this->subCamAtYOffset = cos_s(this->work[BFD_MOVE_TIMER] * 0x8000) * this->subCamShake;
                add_calc2(&this->subCamShake, 2.0f, 1.0f, 0.8 * 0.01f);
                Nai_FxFlagEntry(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                if (this->timers[0] == 0) {
                    this->introState = BFD_CS_COLLAPSE;
                    this->subCamEyeNext.x = player2->actor.world.pos.x + 100.0f + 300.0f;
                    this->subCamEyeNext.y = player2->actor.world.pos.y + 100.0f;
                    this->subCamEyeNext.z = player2->actor.world.pos.z + 200.0f;
                    this->subCamAtNext.x = player2->actor.world.pos.x;
                    this->subCamAtNext.y = player2->actor.world.pos.y - 150.0f;
                    this->subCamAtNext.z = player2->actor.world.pos.z - 50.0f;
                    demo_spd_set(this, 0.1f);
                    this->timers[0] = 170;
                    this->subCamVelFactor = 0.0f;
                    this->subCamAccel = 0.0f;
                    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_20);
                }
                break;
            case BFD_CS_COLLAPSE:
                this->subCamAccel = 0.005f;
                this->subCamAtYOffset = cos_s(this->work[BFD_MOVE_TIMER] * 0x8000) * this->subCamShake;
                add_calc2(&this->subCamShake, 2.0f, 1.0f, 0.8 * 0.01f);
                Nai_FxFlagEntry(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                if (this->timers[0] == 100) {
                    this->platformSignal = VBSIMA_COLLAPSE;
                }
                if (this->timers[0] == 0) {
                    this->introState = BFD_CS_EMERGE;
                    this->subCamVelFactor = 0.0f;
                    this->subCamEyeNext.x = player2->actor.world.pos.x + 100.0f + 300.0f - 600.0f;
                    this->subCamEyeNext.y = player2->actor.world.pos.y + 100.0f - 50.0f;
                    this->subCamEyeNext.z = player2->actor.world.pos.z + 200.0f - 150.0f;
                    this->subCamAtNext.x = 0.0f;
                    this->subCamAtNext.y = 120.0f;
                    this->subCamAtNext.z = 0.0f;
                    demo_spd_set(this, 0.5f);
                    this->subCamAtMaxVelFrac.x = this->subCamAtMaxVelFrac.y = this->subCamAtMaxVelFrac.z = 0.1f;
                    this->subCamEyeMaxVelFrac.x = this->subCamEyeMaxVelFrac.y = this->subCamEyeMaxVelFrac.z = 0.1f;
                    this->subCamAccel = 0.005f;
                    this->timers[0] = 0;
                    this->holeIndex = 1;
                    this->targetPosition.x = hole_pos[this->holeIndex].x;
                    this->targetPosition.y = hole_pos[this->holeIndex].y - 200.0f;
                    this->targetPosition.z = hole_pos[this->holeIndex].z;
                    this->timers[0] = 50;
                    this->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
                    this->actor.world.rot.x = 0x4000;
                    this->work[BFD_MOVE_TIMER] = 0;
                    this->timers[3] = 250;
                    this->timers[2] = 470;
                    this->fwork[BFD_FLY_SPEED] = 5.0f;
                }
                break;
            case BFD_CS_EMERGE:
                PRINTF("WAY_SPD X = %f\n", this->subCamAtVel.x);
                PRINTF("WAY_SPD Y = %f\n", this->subCamAtVel.y);
                PRINTF("WAY_SPD Z = %f\n", this->subCamAtVel.z);
                if ((this->timers[3] > 190) && !GET_EVENTCHKINF(EVENTCHKINF_BEGAN_VOLVAGIA_BATTLE)) {
                    Nai_FxFlagEntry(NA_SE_EN_DODO_K_ROLL - SFX_FLAG, &this->actor.projectedPos, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                }
                if (this->timers[3] == 190) {
                    this->subCamAtMaxVelFrac.x = this->subCamAtMaxVelFrac.y = this->subCamAtMaxVelFrac.z = 0.05f;
                    this->platformSignal = VBSIMA_KILL;
                    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
                }
                if (this->actor.world.pos.y > 120.0f) {
                    this->subCamAtNext = this->actor.world.pos;
                    this->subCamAtVel.x = 190.0f;
                    this->subCamAtVel.y = 85.56f;
                    this->subCamAtVel.z = 25.0f;
                } else {
                    // the following `temp_z` stuff is probably fake but is required to match
                    // it's optimized to 1.0f because sp1CF is false at this point, but the 0.1f ends up in rodata
                    temp_z = 0.1f;
                    if (!sp1CF) {
                        temp_z = 1.0f;
                    }
                    add_calc2(&this->subCamShake, 2.0f, temp_z, 0.1 * 0.08f);
                    this->subCamAtYOffset = cos_s(this->work[BFD_MOVE_TIMER] * 0x8000) * this->subCamShake;
                }
                if (this->timers[3] == 160) {
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_FIRE_BOSS);
                }
                if ((this->timers[3] == 130) && !GET_EVENTCHKINF(EVENTCHKINF_BEGAN_VOLVAGIA_BATTLE)) {
                    Actor_Name_Disp_Set(play, &play->actorCtx.titleCtx, SEGMENTED_TO_VIRTUAL(gVolvagiaTitleCardTex),
                                           160, 180, 128, 40);
                }
                if (this->timers[3] <= 100) {
                    this->subCamEyeVel.x = this->subCamEyeVel.y = this->subCamEyeVel.z = 2.0f;
                    this->subCamEyeNext.x = player2->actor.world.pos.x + 50.0f;
                    this->subCamEyeNext.y = player2->actor.world.pos.y + 50.0f;
                    this->subCamEyeNext.z = player2->actor.world.pos.z + 50.0f;
                }
                if (this->work[BFD_ACTION_STATE] == BOSSFD_FLY_HOLE) {
                    switch (this->introFlyState) {
                        case INTRO_FLY_EMERGE:
                            this->timers[5] = 100;
                            this->introFlyState = INTRO_FLY_HOLE;
                            FALLTHROUGH;
                        case INTRO_FLY_HOLE:
                            if (this->timers[5] == 0) {
                                this->introFlyState = INTRO_FLY_CAMERA;
                                this->timers[5] = 75;
                            }
                            break;
                        case INTRO_FLY_CAMERA:
                            this->targetPosition = this->subCamEye;
                            if (this->timers[5] == 0) {
                                this->timers[0] = 0;
                                this->holeIndex = 7;
                                this->targetPosition.x = hole_pos[this->holeIndex].x;
                                this->targetPosition.y = hole_pos[this->holeIndex].y + 200.0f + 50.0f;
                                this->targetPosition.z = hole_pos[this->holeIndex].z;
                                this->introFlyState = INTRO_FLY_RETRAT;
                            }
                            if (this->timers[5] == 30) {
                                this->work[BFD_ROAR_TIMER] = 40;
                                this->fireBreathTimer = 20;
                            }
                        case INTRO_FLY_RETRAT:
                            break;
                    }
                }
                PRINTF("this->timer[2] = %d\n", this->timers[2]);
                PRINTF("this->timer[5] = %d\n", this->timers[5]);
                if (this->timers[2] == 0) {
                    mainCam->eye = this->subCamEye;
                    mainCam->eyeNext = this->subCamEye;
                    mainCam->at = this->subCamAt;
                    Gama_play_shift2main_camera(play, this->subCamId, 0);
                    // BFD_CS_NONE / BOSSFD_FLY_MAIN / SUB_CAM_ID_DONE
                    this->introState = this->introFlyState = this->subCamId = 0;
                    Demo_play_end(play, &play->csCtx);
                    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                    this->actionFunc = mode_wait;
                    this->handoffSignal = FD2_SIGNAL_GROUND;
                    SET_EVENTCHKINF(EVENTCHKINF_BEGAN_VOLVAGIA_BATTLE);
                }
                break;
        }
        demo_camera_set(this, play);
    } else {
        this->fwork[BFD_FLY_SPEED] = 5.0f;
    }

    //                             Attacks and Death Cutscene

    switch (this->work[BFD_ACTION_STATE]) {
        case BOSSFD_FLY_MAIN:
            sp1CF = true;
            if (this->timers[0] == 0) {
                if (this->actor.colChkInfo.health == 0) {
                    this->work[BFD_ACTION_STATE] = BOSSFD_DEATH_START;
                    this->timers[0] = 0;
                    this->timers[1] = 100;
                } else {
                    u8 temp_rand;

                    if (this->introState != BFD_CS_NONE) {
                        this->holeIndex = 6;
                    } else {
                        do {
                            temp_rand = rnd_f(8.9f);
                        } while (temp_rand == this->holeIndex);
                        this->holeIndex = temp_rand;
                    }
                    this->targetPosition.x = hole_pos[this->holeIndex].x;
                    this->targetPosition.y = hole_pos[this->holeIndex].y + 200.0f + 50.0f;
                    this->targetPosition.z = hole_pos[this->holeIndex].z;
                    this->fwork[BFD_TURN_RATE] = 0.0f;
                    this->fwork[BFD_TURN_RATE_MAX] = 1000.0f;
                    if (this->introState != BFD_CS_NONE) {
                        this->timers[0] = 10050;
                    } else {
                        this->timers[0] = 20;
                    }
                    this->fwork[BFD_FLY_WOBBLE_AMP] = 100.0f;
                    this->work[BFD_ACTION_STATE] = BOSSFD_FLY_HOLE;

                    if (this->work[BFD_START_ATTACK]) {
                        this->work[BFD_START_ATTACK] = false;
                        this->work[BFD_FLY_COUNT]++;
                        if (this->work[BFD_FLY_COUNT] & 1) {
                            this->work[BFD_ACTION_STATE] = BOSSFD_FLY_CHASE;
                            this->timers[0] = 300;
                            this->fwork[BFD_TURN_RATE_MAX] = 900.0f;
                            this->fwork[BFD_TARGET_Y_OFFSET] = 300.0f;
                            this->work[BFD_UNK_234] = this->work[BFD_UNK_236] = 0;
                        } else {
                            this->work[BFD_ACTION_STATE] = BOSSFD_FLY_CEILING;
                        }
                    }
                }
            }
            break;
        case BOSSFD_FLY_HOLE:
            if ((this->timers[0] == 0) && (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 100.0f)) {
                this->work[BFD_ACTION_STATE] = BOSSFD_BURROW;
                this->targetPosition.y = hole_pos[this->holeIndex].y - 70.0f;
                this->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
                this->fwork[BFD_FLY_WOBBLE_AMP] = 0.0f;
                this->timers[0] = 150;
                this->work[BFD_ROAR_TIMER] = 40;
                this->holePosition.x = this->targetPosition.x;
                this->holePosition.z = this->targetPosition.z;
            }
            break;
        case BOSSFD_BURROW:
            sp1CF = true;
            if (this->timers[0] == 0) {
                this->actionFunc = mode_wait;
                this->handoffSignal = FD2_SIGNAL_GROUND;
            }
            break;
        case BOSSFD_EMERGE:
            if ((this->timers[0] == 0) && (sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 100.0f)) {
                this->actor.world.pos = this->targetPosition;
                this->work[BFD_ACTION_STATE] = BOSSFD_FLY_MAIN;
                this->actor.world.rot.x = 0x4000;
                this->targetPosition.y = hole_pos[this->holeIndex].y + 200.0f;
                this->timers[4] = 80;
                this->fwork[BFD_TURN_RATE_MAX] = 1000.0f;
                this->fwork[BFD_FLY_WOBBLE_AMP] = 0.0f;
                this->holePosition.x = this->targetPosition.x;
                this->holePosition.z = this->targetPosition.z;

                set_jisin_2(play, 1, 80, 0x5000);
                if (this->introState != BFD_CS_NONE) {
                    this->timers[0] = 50;
                } else {
                    this->timers[0] = 50;
                }
            }
            break;
        case BOSSFD_FLY_CEILING:
            this->fwork[BFD_FLY_SPEED] = 8;
            this->targetPosition.x = 0.0f;
            this->targetPosition.y = 700.0f;
            this->targetPosition.z = -300.0f;
            this->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            this->fwork[BFD_TURN_RATE_MAX] = 3000.0f;
            if (this->actor.world.pos.y > 700.0f) {
                this->work[BFD_ACTION_STATE] = BOSSFD_DROP_ROCKS;
                this->timers[0] = 25;
                this->timers[2] = 150;
                this->work[BFD_CEILING_TARGET] = 0;
            }
            break;
        case BOSSFD_DROP_ROCKS:
            this->fwork[BFD_FLY_SPEED] = 8;
            this->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            this->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
            this->targetPosition.x = roof_pos[this->work[BFD_CEILING_TARGET]].x;
            this->targetPosition.y = roof_pos[this->work[BFD_CEILING_TARGET]].y + 900.0f;
            this->targetPosition.z = roof_pos[this->work[BFD_CEILING_TARGET]].z;
            if (this->timers[0] == 0) {
                this->timers[0] = 25;
                this->work[BFD_CEILING_TARGET]++;
                if (this->work[BFD_CEILING_TARGET] >= 6) {
                    this->work[BFD_CEILING_TARGET] = 0;
                }
            }
            Actor_BGcheck2(play, &this->actor, 50.0f, 50.0f, 100.0f, UPDBGCHECKINFO_FLAG_1);
            if (this->timers[1] == 0) {
                PRINTF("BGCHECKKKKKKKKKKKKKKKKKKKKKKK\n");
                if (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) {
                    this->fwork[BFD_CEILING_BOUNCE] = -18384.0f;
                    this->timers[1] = 10;
                    Nai_FxFlagEntry(NA_SE_EV_EXPLOSION, &this->actor.projectedPos, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    set_jisin_2(play, 3, 10, 0x7530);
                    this->work[BFD_ROCK_TIMER] = 300;
                }
            } else {
                pitchToTarget = this->fwork[BFD_CEILING_BOUNCE];
                add_calc0(&this->fwork[BFD_CEILING_BOUNCE], 1.0f, 1000.0f);
            }
            if (this->timers[2] == 0) {
                this->work[BFD_ACTION_STATE] = BOSSFD_FLY_MAIN;
                this->timers[0] = 0;
                this->work[BFD_START_ATTACK] = false;
            }
            break;
        case BOSSFD_FLY_CHASE:
            this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
            temp_y = sin_s(this->work[BFD_MOVE_TIMER] * 2396.0f) * 30.0f + this->fwork[BFD_TARGET_Y_OFFSET];
            this->targetPosition.x = player->actor.world.pos.x;
            this->targetPosition.y = player->actor.world.pos.y + temp_y + 30.0f;
            this->targetPosition.z = player->actor.world.pos.z;
            this->fwork[BFD_FLY_WOBBLE_AMP] = 0.0f;
            if (((this->timers[0] % 64) == 0) && (this->timers[0] < 450)) {
                this->work[BFD_ROAR_TIMER] = 40;
                if (fd_angle_check(this)) {
                    this->fireBreathTimer = 20;
                }
            }
            if ((this->work[BFD_DAMAGE_FLASH_TIMER] != 0) || (this->timers[0] == 0) ||
                (player->actor.world.pos.y < 70.0f)) {
                this->work[BFD_ACTION_STATE] = BOSSFD_FLY_MAIN;
                this->timers[0] = 0;
                this->work[BFD_START_ATTACK] = false;
            } else {
                add_calc2(&this->fwork[BFD_TARGET_Y_OFFSET], 50.0, 1.0f, 2.0f);
            }
            break;
        case BOSSFD_DEATH_START:
            if (sqrtf(SQ(dx) + SQ(dz)) < 50.0f) {
                this->timers[0] = 0;
            }
            if (this->timers[0] == 0) {
                this->timers[0] = (s16)rnd_f(10.0f) + 10;
                do {
                    this->targetPosition.x = rnd_fx(200.0f);
                    this->targetPosition.y = 390.0f;
                    this->targetPosition.z = rnd_fx(200.0f);
                    temp_x = this->targetPosition.x - this->actor.world.pos.x;
                    temp_z = this->targetPosition.z - this->actor.world.pos.z;
                } while (!(sqrtf(SQ(temp_x) + SQ(temp_z)) > 100.0f));
            }
            this->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            this->fwork[BFD_FLY_WOBBLE_RATE] = 1000.0f;
            this->fwork[BFD_TURN_RATE_MAX] = 10000.0f;
            add_calc2(&this->fwork[BFD_BODY_PULSE], 0.3f, 1.0f, 0.05f);
            if (this->timers[1] == 0) {
                this->work[BFD_ACTION_STATE] = BOSSFD_SKIN_BURN;
                this->timers[0] = 30;
            }
            break;
        case BOSSFD_SKIN_BURN:
            this->targetPosition.x = 0.0f;
            this->targetPosition.y = 390.0f;
            this->targetPosition.z = 0.0f;
            this->fwork[BFD_FLY_WOBBLE_AMP] = 200.0f;
            this->fwork[BFD_FLY_WOBBLE_RATE] = 1000.0f;
            this->fwork[BFD_TURN_RATE_MAX] = 2000.0f;
            add_calc2(&this->fwork[BFD_BODY_PULSE], 0.3f, 1.0f, 0.05f);
            if ((this->timers[0] == 0) && ((this->work[BFD_MOVE_TIMER] % 4) == 0)) {
                if (this->skinSegments != 0) {
                    this->skinSegments--;
                    if (this->skinSegments == 0) {
                        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS_CLEAR);
                    }
                } else {
                    this->work[BFD_ACTION_STATE] = BOSSFD_BONES_FALL;
                    this->timers[0] = 30;
                }
            }
            if ((this->work[BFD_MOVE_TIMER] % 32) == 0) {
                this->work[BFD_ROAR_TIMER] = 40;
            }

            if (this->skinSegments != 0) {
                Vec3f sp188;
                Vec3f sp17C = { 0.0f, 0.0f, 0.0f };
                Vec3f sp170;
                Vec3f sp164 = { 0.0f, 0.03f, 0.0f };
                Vec3f sp158;
                f32 pad154;
                s16 temp_rand2;
                s16 sp150;

                if (this->fogMode == 0) {
                    play->envCtx.lightBlend = 0;
                }
                this->fogMode = 0xA;

                sp150 = 1;
                if (this->work[BFD_MOVE_TIMER] & 0x1C) {
                    Nai_FxFlagEntry(NA_SE_EN_VALVAISA_BURN - SFX_FLAG, &this->actor.projectedPos, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                }
                for (i1 = 0; i1 < sp150; i1++) {
                    if (sp150) { // Needed for matching
                        temp_rand2 = rnd_f(99.9f);

                        sp188.x = this->bodySegsPos[temp_rand2].x;
                        sp188.y = this->bodySegsPos[temp_rand2].y - 10.0f;
                        sp188.z = this->bodySegsPos[temp_rand2].z;

                        sp164.y = 0.03f;

                        Effect_k_fire_ct(play, &sp188, &sp17C, &sp164, (s16)rnd_f(20.0f) + 40, 0x64);

                        for (i2 = 0; i2 < 15; i2++) {
                            sp170.x = rnd_fx(20.0f);
                            sp170.y = rnd_fx(20.0f);
                            sp170.z = rnd_fx(20.0f);

                            sp158.y = 0.4f;
                            sp158.x = rnd_fx(0.5f);
                            sp158.z = rnd_fx(0.5f);

                            Effect_vb_hinoko_ct_IN(this->effects, &sp188, &sp170, &sp158, (s16)rnd_f(3.0f) + 8);
                        }
                    }
                }
            }
            break;
        case BOSSFD_BONES_FALL:
            this->work[BFD_STOP_FLAG] = true;
            this->fogMode = 3;
            if (this->timers[0] < 18) {
                this->bodyFallApart[this->timers[0]] = 1;
            }
            if (this->timers[0] == 0) {
                this->work[BFD_ACTION_STATE] = BOSSFD_SKULL_PAUSE;
                this->timers[0] = 15;
                this->work[BFD_CEILING_TARGET] = 0;
                player->actor.world.pos.y = 90.0f;
                player->actor.world.pos.x = 40.0f;
                player->actor.world.pos.z = 150.0f;
            }
            break;
        case BOSSFD_SKULL_PAUSE:
            if (this->timers[0] == 0) {
                this->work[BFD_ACTION_STATE] = BOSSFD_SKULL_FALL;
                this->timers[0] = 20;
                this->work[BFD_STOP_FLAG] = false;
            }
            break;
        case BOSSFD_SKULL_FALL:
            this->fwork[BFD_TURN_RATE] = this->fwork[BFD_TURN_RATE_MAX] = this->actor.speed =
                this->fwork[BFD_FLY_SPEED] = 0;

            if (this->timers[0] == 1) {
                this->actor.world.pos.x = 0;
                this->actor.world.pos.y = 900.0f;
                this->actor.world.pos.z = 150.0f;
                this->actor.world.rot.x = this->actor.world.rot.y = 0;
                this->actor.shape.rot.z = 0x1200;
                this->actor.velocity.x = 0;
                this->actor.velocity.z = 0;
            }
            if (this->timers[0] == 0) {
                if (this->actor.world.pos.y <= 110.0f) {
                    this->actor.world.pos.y = 110.0f;
                    this->actor.velocity.y = 0;
                    if (this->work[BFD_CEILING_TARGET] == 0) {
                        this->work[BFD_CEILING_TARGET]++;
                        this->timers[1] = 60;
                        this->work[BFD_CAM_SHAKE_TIMER] = 20;
                        Nai_FxFlagEntry(NA_SE_EN_VALVAISA_LAND2, &this->actor.projectedPos, 4,
                                             &_dummy_one, &_dummy_one,
                                             &_dummy_zero_s8);
                        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_5);
                        for (i1 = 0; i1 < 15; i1++) {
                            Vec3f sp144 = { 0.0f, 0.0f, 0.0f };
                            Vec3f sp138 = { 0.0f, 0.0f, 0.0f };
                            Vec3f sp12C;

                            sp144.x = rnd_fx(8.0f);
                            sp144.y = rnd_f(1.0f);
                            sp144.z = rnd_fx(8.0f);

                            sp138.y = 0.3f;

                            sp12C.x = rnd_fx(10.0f) + this->actor.world.pos.x;
                            sp12C.y = rnd_fx(10.0f) + this->actor.world.pos.y;
                            sp12C.z = rnd_fx(10.0f) + this->actor.world.pos.z;
                            Effect_vb_smoke_ct_IN(this->effects, &sp12C, &sp144, &sp138, rnd_f(100.0f) + 300);
                        }
                    }
                } else {
                    this->actor.velocity.y -= 1.0f;
                }
            } else {
                this->actor.velocity.y = 0;
            }
            if (this->timers[1] == 1) {
                this->work[BFD_ACTION_STATE] = BOSSFD_SKULL_BURN;
                this->timers[0] = 70;
            }
            break;
        case BOSSFD_SKULL_BURN:
            this->actor.velocity.y = 0.0f;
            this->actor.world.pos.y = 110.0f;
            this->fwork[BFD_TURN_RATE] = this->fwork[BFD_TURN_RATE_MAX] = this->actor.speed =
                this->fwork[BFD_FLY_SPEED] = 0.0f;

            if ((50 > this->timers[0]) && (this->timers[0] > 0)) {
                Vec3f sp120;
                Vec3f sp114 = { 0.0f, 0.0f, 0.0f };
                Vec3f sp108 = { 0.0f, 0.03f, 0.0f };

                Nai_FxFlagEntry(NA_SE_EN_GOMA_LAST - SFX_FLAG, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);

                sp120.x = rnd_fx(40.0f) + this->actor.world.pos.x;
                sp120.y = (rnd_fx(10.0f) + this->actor.world.pos.y) - 10.0f;
                sp120.z = (rnd_fx(40.0f) + this->actor.world.pos.z) + 5.0f;

                sp108.y = 0.03f;

                Effect_k_fire_ct(play, &sp120, &sp114, &sp108, (s16)rnd_f(15.0f) + 30, 0);
            }
            if (this->timers[0] < 20) {
                add_calc0(&this->actor.scale.x, 1.0f, 0.0025f);
                Actor_set_scale(&this->actor, this->actor.scale.x);
            }
            if (this->timers[0] == 0) {
                this->actionFunc = mode_wait;
                this->actor.world.pos.y -= 1000.0f;
            }
            if (this->timers[0] == 7) {
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, 0, 0, 0);
            }
            break;
        case BOSSFD_WAIT_INTRO:
            break;
    }

    //                                 Update body segments and mane

    if (!this->work[BFD_STOP_FLAG]) {
        s16 i4;

        adds(&this->actor.world.rot.y, angleToTarget, 0xA, this->fwork[BFD_TURN_RATE]);

        if (((this->work[BFD_ACTION_STATE] == BOSSFD_FLY_CHASE) ||
             (this->work[BFD_ACTION_STATE] == BOSSFD_FLY_UNUSED)) &&
            (this->actor.world.pos.y < 110.0f) && (pitchToTarget < 0)) {
            pitchToTarget = 0;
            add_calc2(&this->actor.world.pos.y, 110.0f, 1.0f, 5.0f);
        }

        adds(&this->actor.world.rot.x, pitchToTarget, 0xA, this->fwork[BFD_TURN_RATE]);
        add_calc2(&this->fwork[BFD_TURN_RATE], this->fwork[BFD_TURN_RATE_MAX], 1.0f, 20000.0f);
        add_calc2(&this->actor.speed, this->fwork[BFD_FLY_SPEED], 1.0f, 0.1f);
        if (this->work[BFD_ACTION_STATE] < BOSSFD_SKULL_FALL) {
            Actor_position_speed_set_XY(&this->actor);
        }
        Actor_position_move(&this->actor);

        this->work[BFD_LEAD_BODY_SEG]++;
        if (this->work[BFD_LEAD_BODY_SEG] >= 100) {
            this->work[BFD_LEAD_BODY_SEG] = 0;
        }
        i4 = this->work[BFD_LEAD_BODY_SEG];
        this->bodySegsPos[i4].x = this->actor.world.pos.x;
        this->bodySegsPos[i4].y = this->actor.world.pos.y;
        this->bodySegsPos[i4].z = this->actor.world.pos.z;
        this->bodySegsRot[i4].x = BINANG_TO_RAD_ALT(this->actor.world.rot.x);
        this->bodySegsRot[i4].y = BINANG_TO_RAD_ALT(this->actor.world.rot.y);
        this->bodySegsRot[i4].z = BINANG_TO_RAD_ALT(this->actor.world.rot.z);

        this->work[BFD_LEAD_MANE_SEG]++;
        if (this->work[BFD_LEAD_MANE_SEG] >= 30) {
            this->work[BFD_LEAD_MANE_SEG] = 0;
        }
        i4 = this->work[BFD_LEAD_MANE_SEG];
        this->centerMane.scale[i4] = (sin_s(this->work[BFD_MOVE_TIMER] * 5596.0f) * 0.3f) + 1.0f;
        this->rightMane.scale[i4] = (sin_s(this->work[BFD_MOVE_TIMER] * 5496.0f) * 0.3f) + 1.0f;
        this->leftMane.scale[i4] = (cos_s(this->work[BFD_MOVE_TIMER] * 5696.0f) * 0.3f) + 1.0f;
        this->centerMane.pos[i4] = this->centerMane.head;
        this->fireManeRot[i4].x = BINANG_TO_RAD_ALT(this->actor.world.rot.x);
        this->fireManeRot[i4].y = BINANG_TO_RAD_ALT(this->actor.world.rot.y);
        this->fireManeRot[i4].z = BINANG_TO_RAD_ALT(this->actor.world.rot.z);
        this->rightMane.pos[i4] = this->rightMane.head;
        this->leftMane.pos[i4] = this->leftMane.head;

        if ((0x3000 > this->actor.world.rot.x) && (this->actor.world.rot.x > -0x3000)) {
            add_calc2(&this->flattenMane, 1.0f, 1.0f, 0.05f);
        } else {
            add_calc2(&this->flattenMane, 0.5f, 1.0f, 0.05f);
        }

        if (this->work[BFD_ACTION_STATE] < BOSSFD_SKULL_FALL) {
            if ((this->actor.prevPos.y < 90.0f) && (90.0f <= this->actor.world.pos.y)) {
                this->timers[4] = 80;
                set_jisin_2(play, 1, 80, 0x5000);
                this->work[BFD_ROAR_TIMER] = 40;
                this->work[BFD_MANE_EMBERS_TIMER] = 30;
                this->work[BFD_SPLASH_TIMER] = 10;
            }
            if ((this->actor.prevPos.y > 90.0f) && (90.0f >= this->actor.world.pos.y)) {
                this->timers[4] = 80;
                set_jisin_2(play, 1, 80, 0x5000);
                this->work[BFD_MANE_EMBERS_TIMER] = 30;
                this->work[BFD_SPLASH_TIMER] = 10;
            }
        }

        if (!sp1CF) {
            Vec3f spE0[3];
            Vec3f spBC[3];
            spE0[0].x = spE0[0].y = sin_s(this->work[BFD_MOVE_TIMER] * 1500.0f) * 3000.0f;
            spE0[1].x = sin_s(this->work[BFD_MOVE_TIMER] * 2000.0f) * 4000.0f;
            spE0[1].y = sin_s(this->work[BFD_MOVE_TIMER] * 2200.0f) * 4000.0f;
            spE0[2].x = sin_s(this->work[BFD_MOVE_TIMER] * 1700.0f) * 2000.0f;
            spE0[2].y = sin_s(this->work[BFD_MOVE_TIMER] * 1900.0f) * 2000.0f;
            spBC[0].x = spBC[0].y = sin_s(this->work[BFD_MOVE_TIMER] * 1500.0f) * -3000.0f;
            spBC[1].x = sin_s(this->work[BFD_MOVE_TIMER] * 2200.0f) * -4000.0f;
            spBC[1].y = sin_s(this->work[BFD_MOVE_TIMER] * 2000.0f) * -4000.0f;
            spBC[2].x = sin_s(this->work[BFD_MOVE_TIMER] * 1900.0f) * -2000.0f;
            spBC[2].y = sin_s(this->work[BFD_MOVE_TIMER] * 1700.0f) * -2000.0f;

            for (i3 = 0; i3 < 3; i3++) {
                add_calc2(&this->rightArmRot[i3].x, spE0[i3].x, 1.0f, 1000.0f);
                add_calc2(&this->rightArmRot[i3].y, spE0[i3].y, 1.0f, 1000.0f);
                add_calc2(&this->leftArmRot[i3].x, spBC[i3].x, 1.0f, 1000.0f);
                add_calc2(&this->leftArmRot[i3].y, spBC[i3].y, 1.0f, 1000.0f);
            }
        } else {
            for (i2 = 0; i2 < 3; i2++) {
                f32 phi_f20 = 0.0f;
                f32 padB4;
                f32 padB0;
                f32 padAC;

                add_calc0(&this->rightArmRot[i2].y, 0.1f, 100.0f);
                add_calc0(&this->leftArmRot[i2].y, 0.1f, 100.0f);
                if (i2 == 0) {
                    phi_f20 = -3000.0f;
                }
                add_calc2(&this->rightArmRot[i2].x, phi_f20, 0.1f, 100.0f);
                add_calc2(&this->leftArmRot[i2].x, -phi_f20, 0.1f, 100.0f);
            }
        }
    }
}

static void mode_wait(BossFd* this, PlayState* play) {
    if (this->handoffSignal == FD2_SIGNAL_FLY) { // Set by BossFd2
        this->handoffSignal = FD2_SIGNAL_NONE;
        mode_fly_init(this, play);
        {
            u8 temp_rand;

            do {
                temp_rand = rnd_f(8.9f);
            } while (temp_rand == this->holeIndex);
            this->holeIndex = temp_rand;
        }
        if (1) {} // Needed for matching
        this->targetPosition.x = hole_pos[this->holeIndex].x;
        this->targetPosition.y = hole_pos[this->holeIndex].y - 200.0f;
        this->targetPosition.z = hole_pos[this->holeIndex].z;
        this->actor.world.pos = this->targetPosition;

        this->timers[0] = 10;
        this->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
        this->work[BFD_START_ATTACK] = true;
    }
    if (this->handoffSignal == FD2_SIGNAL_DEATH) {
        this->handoffSignal = FD2_SIGNAL_NONE;
        mode_fly_init(this, play);
        this->holeIndex = 1;
        this->targetPosition.x = hole_pos[1].x;
        this->targetPosition.y = hole_pos[1].y - 200.0f;
        this->targetPosition.z = hole_pos[1].z;
        this->actor.world.pos = this->targetPosition;
        this->timers[0] = 10;
        this->work[BFD_ACTION_STATE] = BOSSFD_EMERGE;
    }
}

static Vec3f fd_se_pos = { 0.0f, 0.0f, 50.0f };

static void another_cont(BossFd* this, PlayState* play) {
    static Color_RGBA8 prim = { 255, 255, 0, 255 };
    static Color_RGBA8 env = { 255, 10, 0, 255 };
    s16 breathOpacity = 0;
    f32 jawAngle;
    f32 jawSpeed;
    f32 emberRate;
    f32 emberSpeed;
    s16 eyeStates[] = { EYE_OPEN, EYE_HALF, EYE_CLOSED, EYE_CLOSED, EYE_HALF };
    f32 temp_x;
    f32 temp_z;

    if (1) {} // Needed for match

    if (this->fogMode == 0) {
        play->envCtx.lightSettingOverride = 0;
        play->envCtx.lightBlend = 0.5f + 0.5f * sin_s(this->work[BFD_VAR_TIMER] * 0x500);
        play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_FULL_CONTROL;
        play->envCtx.lightSetting = 1;
        play->envCtx.prevLightSetting = 0;
    } else if (this->fogMode == 3) {
        play->envCtx.lightSettingOverride = 0;
        play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_FULL_CONTROL;
        play->envCtx.lightSetting = 2;
        play->envCtx.prevLightSetting = 0;
        add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.05f);
    } else if (this->fogMode == 2) {
        s16 pad;

        this->fogMode--;
        play->envCtx.lightSettingOverride = 0;
        add_calc2(&play->envCtx.lightBlend, 0.55f + 0.05f * sin_s(this->work[BFD_VAR_TIMER] * 0x3E00), 1.0f,
                       0.15f);
        play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_FULL_CONTROL;
        play->envCtx.lightSetting = 3;
        play->envCtx.prevLightSetting = 0;
    } else if (this->fogMode == 10) {
        s16 pad;

        this->fogMode = 1;
        play->envCtx.lightSettingOverride = 0;
        add_calc2(&play->envCtx.lightBlend, 0.21f + 0.07f * sin_s(this->work[BFD_VAR_TIMER] * 0xC00), 1.0f,
                       0.05f);
        play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_FULL_CONTROL;
        play->envCtx.lightSetting = 3;
        play->envCtx.prevLightSetting = 0;
    } else if (this->fogMode == 1) {
        add_calc2(&play->envCtx.lightBlend, 0.0f, 1.0f, 0.03f);
        if (play->envCtx.lightBlend <= 0.01f) {
            this->fogMode = 0;
        }
    }

    if (this->work[BFD_MANE_EMBERS_TIMER] != 0) {
        this->work[BFD_MANE_EMBERS_TIMER]--;
        emberSpeed = emberRate = 20.0f;
    } else {
        emberRate = 3.0f;
        emberSpeed = 5.0f;
    }
    add_calc2(&this->fwork[BFD_MANE_EMBER_RATE], emberRate, 1.0f, 0.1f);
    add_calc2(&this->fwork[BFD_MANE_EMBER_SPEED], emberSpeed, 1.0f, 0.5f);

    if (((this->work[BFD_VAR_TIMER] % 8) == 0) && (fqrand() < 0.3f)) {
        this->work[BFD_BLINK_TIMER] = 4;
    }
    this->eyeState = eyeStates[this->work[BFD_BLINK_TIMER]];

    if (this->work[BFD_BLINK_TIMER] != 0) {
        this->work[BFD_BLINK_TIMER]--;
    }

    if (this->work[BFD_ROAR_TIMER] != 0) {
        if (this->work[BFD_ROAR_TIMER] == 37) {
            Nai_FxFlagEntry(NA_SE_EN_VALVAISA_ROAR, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
        jawAngle = 6000.0f;
        jawSpeed = 1300.0f;
    } else {
        jawAngle = (this->work[BFD_VAR_TIMER] & 0x10) ? 0.0f : 1000.0f;
        jawSpeed = 500.0f;
    }
    add_calc2(&this->jawOpening, jawAngle, 0.3f, jawSpeed);

    if (this->work[BFD_ROAR_TIMER] != 0) {
        this->work[BFD_ROAR_TIMER]--;
    }

    if (this->timers[4] != 0) {
        Vec3f spawnVel1;
        Vec3f spawnAccel1;
        Vec3f spawnPos1;
        s16 i;

        Nai_FxFlagEntry(NA_SE_EN_VALVAISA_APPEAR - SFX_FLAG, &this->actor.projectedPos, 4,
                             &_dummy_one, &_dummy_one, &_dummy_zero_s8);
        if (this->work[BFD_SPLASH_TIMER] != 0) {
            this->work[BFD_SPLASH_TIMER]--;
            if ((this->actor.colChkInfo.health == 0) ||
                ((this->introState == BFD_CS_EMERGE) && (this->actor.world.rot.x > 0x3000))) {
                if ((u8)this->fogMode == 0) {
                    play->envCtx.lightBlend = 0.0f;
                }
                this->fogMode = 2;
            }
            for (i = 0; i < 5; i++) {
                spawnVel1.x = rnd_fx(20.0f);
                spawnVel1.y = rnd_f(5.0f) + 4.0f;
                spawnVel1.z = rnd_fx(20.0f);

                spawnAccel1.x = spawnAccel1.z = 0.0f;
                spawnAccel1.y = -0.3f;

                temp_x = (spawnVel1.x * 20) / 10.0f;
                temp_z = (spawnVel1.z * 20) / 10.0f;
                spawnPos1.x = temp_x + this->holePosition.x;
                spawnPos1.y = 100.0f;
                spawnPos1.z = temp_z + this->holePosition.z;

                Effect_SS_Dust_sc_cl_co_ct(play, &spawnPos1, &spawnVel1, &spawnAccel1, &prim, &env,
                              (s16)rnd_f(150.0f) + 800, 10, (s16)rnd_f(5.0f) + 17);
            }
        } else {
            for (i = 0; i < 2; i++) {
                spawnVel1.x = rnd_fx(10.0f);
                spawnVel1.y = rnd_f(3.0f) + 3.0f;
                spawnVel1.z = rnd_fx(10.0f);

                spawnAccel1.x = spawnAccel1.z = 0.0f;
                spawnAccel1.y = -0.3f;
                temp_x = (spawnVel1.x * 50) / 10.0f;
                temp_z = (spawnVel1.z * 50) / 10.0f;

                spawnPos1.x = temp_x + this->holePosition.x;
                spawnPos1.y = 100.0f;
                spawnPos1.z = temp_z + this->holePosition.z;

                Effect_SS_Dust_sc_cl_co_ct(play, &spawnPos1, &spawnVel1, &spawnAccel1, &prim, &env, 500, 10, 20);
            }
        }

        for (i = 0; i < 8; i++) {
            spawnVel1.x = rnd_fx(20.0f);
            spawnVel1.y = rnd_f(10.0f);
            spawnVel1.z = rnd_fx(20.0f);

            spawnAccel1.y = 0.4f;
            spawnAccel1.x = rnd_fx(0.5f);
            spawnAccel1.z = rnd_fx(0.5f);

            spawnPos1.x = rnd_fx(60.0) + this->holePosition.x;
            spawnPos1.y = rnd_f(40.0f) + 100.0f;
            spawnPos1.z = rnd_fx(60.0) + this->holePosition.z;

            Effect_vb_hinoko_ct_IN(this->effects, &spawnPos1, &spawnVel1, &spawnAccel1, (s16)rnd_f(1.5f) + 6);
        }
    }

    if ((this->fireBreathTimer != 0) && (this->fireBreathTimer < 17)) {
        breathOpacity = (this->fireBreathTimer >= 6) ? 255 : this->fireBreathTimer * 50;
    }
    if (breathOpacity != 0) {
        s16 i;
        f32 spawnAngleX;
        f32 spawnAngleY;
        Vec3f spawnSpeed2 = { 0.0f, 0.0f, 0.0f };
        Vec3f spawnVel2;
        Vec3f spawnAccel2 = { 0.0f, 0.0f, 0.0f };
        Vec3f spawnPos2;

        this->fogMode = 2;
        spawnSpeed2.z = 30.0f;

        Nai_FxFlagEntry(NA_SE_EN_VALVAISA_FIRE - SFX_FLAG, &fd_se_pos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        spawnPos2 = this->headPos;

        spawnAngleY = BINANG_TO_RAD_ALT(this->actor.world.rot.y);
        spawnAngleX = BINANG_TO_RAD_ALT(-this->actor.world.rot.x) + 0.3f;
        Matrix_rotateY(spawnAngleY, MTXMODE_NEW);
        Matrix_rotateX(spawnAngleX, MTXMODE_APPLY);
        Matrix_Position(&spawnSpeed2, &spawnVel2);

        Effect_vb_fire_ct_IN(this->effects, &spawnPos2, &spawnVel2, &spawnAccel2,
                               50.0f * sin_s(this->work[BFD_VAR_TIMER] * 0x2000) + 300.0f, breathOpacity,
                               this->actor.world.rot.y);

        spawnPos2.x += spawnVel2.x * 0.5f;
        spawnPos2.y += spawnVel2.y * 0.5f;
        spawnPos2.z += spawnVel2.z * 0.5f;

        Effect_vb_fire_ct_IN(this->effects, &spawnPos2, &spawnVel2, &spawnAccel2,
                               50.0f * sin_s(this->work[BFD_VAR_TIMER] * 0x2000) + 300.0f, breathOpacity,
                               this->actor.world.rot.y);
        spawnSpeed2.x = 0.0f;
        spawnSpeed2.y = 17.0f;
        spawnSpeed2.z = 0.0f;

        for (i = 0; i < 6; i++) {
            spawnAngleY = rnd_f(2.0f * M_PI);
            spawnAngleX = rnd_f(2.0f * M_PI);
            Matrix_rotateY(spawnAngleY, MTXMODE_NEW);
            Matrix_rotateX(spawnAngleX, MTXMODE_APPLY);
            Matrix_Position(&spawnSpeed2, &spawnVel2);

            spawnAccel2.x = (spawnVel2.x * -10) / 100;
            spawnAccel2.y = (spawnVel2.y * -10) / 100;
            spawnAccel2.z = (spawnVel2.z * -10) / 100;

            Effect_vb_hinoko_ct_IN(this->effects, &this->headPos, &spawnVel2, &spawnAccel2, (s16)rnd_f(2.0f) + 8);
        }
    }

    if ((this->actor.world.pos.y < 90.0f) || (700.0f < this->actor.world.pos.y) || (this->actionFunc == mode_wait)) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

void Boss_Fd_Damage_check(BossFd* this, PlayState* play) {
    ColliderJntSphElement* headCollider = &this->collider.elements[0];
    ColliderElement* acHitElem;

    if (headCollider->base.acElemFlags & ACELEM_HIT) {
        headCollider->base.acElemFlags &= ~ACELEM_HIT;
        acHitElem = headCollider->base.acHitElem;
        this->actor.colChkInfo.health -= 2;
        if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_ICE) {
            this->actor.colChkInfo.health -= 2;
        }
        if ((s8)this->actor.colChkInfo.health <= 2) {
            this->actor.colChkInfo.health = 2;
        }
        this->work[BFD_DAMAGE_FLASH_TIMER] = 10;
        this->work[BFD_INVINC_TIMER] = 20;
        Nai_FxFlagEntry(NA_SE_EN_VALVAISA_DAMAGE1, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

void Boss_Fd_actor_move(Actor* thisx, PlayState* play) {
    s16 i;
    BossFd* this = (BossFd*)thisx;

    PRINTF("FD MOVE START \n");
    this->work[BFD_VAR_TIMER]++;
    this->work[BFD_MOVE_TIMER]++;
    this->actionFunc(this, play);

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }
    if (this->fireBreathTimer != 0) {
        this->fireBreathTimer--;
    }
    if (this->work[BFD_DAMAGE_FLASH_TIMER] != 0) {
        this->work[BFD_DAMAGE_FLASH_TIMER]--;
    }
    if (this->work[BFD_INVINC_TIMER] != 0) {
        this->work[BFD_INVINC_TIMER]--;
    }
    if (this->work[BFD_ACTION_STATE] < BOSSFD_DEATH_START) {
        if (this->work[BFD_INVINC_TIMER] == 0) {
            Boss_Fd_Damage_check(this, play);
        }
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }

    another_cont(this, play);
    this->fwork[BFD_TEX1_SCROLL_X] += 4.0f;
    this->fwork[BFD_TEX1_SCROLL_Y] = 120.0f;
    this->fwork[BFD_TEX2_SCROLL_X] += 3.0f;
    this->fwork[BFD_TEX2_SCROLL_Y] -= 2.0f;

    if (1) {
        f32 headGlow;
        f32 rManeGlow;
        f32 lManeGlow;
        s32 pad;

        add_calc2(&this->fwork[BFD_BODY_TEX2_ALPHA], (this->work[BFD_VAR_TIMER] & 0x10) ? 30.0f : 158.0f, 1.0f,
                       8.0f);
        if (this->skinSegments == 0) {
            this->fwork[BFD_HEAD_TEX2_ALPHA] = this->fwork[BFD_BODY_TEX2_ALPHA];
        } else {
            headGlow = (this->work[BFD_VAR_TIMER] & 4) ? 0.0f : 255.0f;
            add_calc2(&this->fwork[BFD_HEAD_TEX2_ALPHA], headGlow, 1.0f, 64.0f);
        }

        headGlow = (this->work[BFD_VAR_TIMER] & 8) ? 128.0f : 255.0f;
        rManeGlow = ((this->work[BFD_VAR_TIMER] + 3) & 8) ? 128.0f : 255.0f;
        lManeGlow = ((this->work[BFD_VAR_TIMER] + 6) & 8) ? 128.0f : 255.0f;

        add_calc2(&this->fwork[BFD_MANE_COLOR_CENTER], headGlow, 1.0f, 16.0f);
        add_calc2(&this->fwork[BFD_MANE_COLOR_RIGHT], rManeGlow, 1.0f, 16.0f);
        add_calc2(&this->fwork[BFD_MANE_COLOR_LEFT], lManeGlow, 1.0f, 16.0f);

        if (this->work[BFD_ROCK_TIMER] != 0) {
            this->work[BFD_ROCK_TIMER]--;
            if ((this->work[BFD_ROCK_TIMER] % 16) == 0) {
                EnVbBall* bossFdRock = (EnVbBall*)Actor_info_make_child_actor(
                    &play->actorCtx, &this->actor, play, ACTOR_EN_VB_BALL, this->actor.world.pos.x, 1000.0f,
                    this->actor.world.pos.z, 0, 0, (s16)rnd_f(50.0f) + 130, 100);

                if (bossFdRock != NULL) {
                    for (i = 0; i < 10; i++) {
                        Vec3f debrisVel = { 0.0f, 0.0f, 0.0f };
                        Vec3f debrisAccel = { 0.0f, -1.0f, 0.0f };
                        Vec3f debrisPos;

                        debrisPos.x = rnd_fx(300.0f) + bossFdRock->actor.world.pos.x;
                        debrisPos.y = rnd_fx(300.0f) + bossFdRock->actor.world.pos.y;
                        debrisPos.z = rnd_fx(300.0f) + bossFdRock->actor.world.pos.z;

                        Effect_Hahen_ct_IN(this->effects, &debrisPos, &debrisVel, &debrisAccel,
                                           (s16)rnd_f(15.0f) + 20);
                    }
                }
            }
        }
    }

    if (1) {
        Vec3f emberVel = { 0.0f, 0.0f, 0.0f };
        Vec3f emberAccel = { 0.0f, 0.0f, 0.0f };
        Vec3f emberPos;
        s16 temp_rand;

        for (i = 0; i < 6; i++) {
            emberAccel.y = 0.4f;
            emberAccel.x = rnd_fx(0.5f);
            emberAccel.z = rnd_fx(0.5f);

            temp_rand = rnd_f(8.9f);

            emberPos.x = hole_pos[temp_rand].x + rnd_fx(60.0f);
            emberPos.y = (hole_pos[temp_rand].y + 10.0f) + rnd_f(40.0f);
            emberPos.z = hole_pos[temp_rand].z + rnd_fx(60.0f);

            Effect_vb_hinoko_ct_IN(this->effects, &emberPos, &emberVel, &emberAccel, (s16)rnd_f(2.0f) + 6);
        }

        if (this->skinSegments != 0) {
            for (i = 0; i < (s16)this->fwork[BFD_MANE_EMBER_RATE]; i++) {
                temp_rand = rnd_f(29.9f);
                emberPos.y = this->centerMane.pos[temp_rand].y + rnd_fx(20.0f);

                if (emberPos.y >= 90.0f) {
                    emberPos.x = this->centerMane.pos[temp_rand].x + rnd_fx(20.0f);
                    emberPos.z = this->centerMane.pos[temp_rand].z + rnd_fx(20.0f);

                    emberVel.x = rnd_fx(this->fwork[BFD_MANE_EMBER_SPEED]);
                    emberVel.y = rnd_fx(this->fwork[BFD_MANE_EMBER_SPEED]);
                    emberVel.z = rnd_fx(this->fwork[BFD_MANE_EMBER_SPEED]);

                    emberAccel.y = 0.4f;
                    emberAccel.x = rnd_fx(0.5f);
                    emberAccel.z = rnd_fx(0.5f);

                    Effect_vb_hinoko_ct_IN(this->effects, &emberPos, &emberVel, &emberAccel, (s16)rnd_f(2.0f) + 8);
                }
            }
        }
    }
    PRINTF("FD MOVE END 1\n");
    Boss_Eff_move(this, play);
    PRINTF("FD MOVE END 2\n");
}

static void Boss_Eff_move(BossFd* this, PlayState* play) {
    BossFdEffect* effect = this->effects;
    Player* player = GET_PLAYER(play);
    Color_RGB8 colors[4] = { { 255, 128, 0 }, { 255, 0, 0 }, { 255, 255, 0 }, { 255, 0, 0 } };
    Vec3f diff;
    s16 i1;
    s16 cInd;

    for (i1 = 0; i1 < BOSSFD_EFFECT_COUNT; i1++, effect++) {
        if (effect->type != BFD_FX_NONE) {
            effect->timer1++;

            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;

            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;
            if (effect->type == BFD_FX_EMBER) {
                cInd = effect->timer1 % 4;

                effect->color.r = colors[cInd].r;
                effect->color.g = colors[cInd].g;
                effect->color.b = colors[cInd].b;
                effect->alpha -= 20;
                if (effect->alpha <= 0) {
                    effect->alpha = 0;
                    effect->type = 0;
                }
            } else if ((effect->type == BFD_FX_DEBRIS) || (effect->type == BFD_FX_SKULL_PIECE)) {
                effect->vFdFxRotX += 0.55f;
                effect->vFdFxRotY += 0.1f;
                if (effect->pos.y <= 100.0f) {
                    effect->type = 0;
                }
            } else if (effect->type == BFD_FX_DUST) {
                if (effect->timer2 >= 8) {
                    effect->timer2 = 8;
                    effect->type = 0;
                } else if (((effect->timer1 % 2) != 0) || (fqrand() < 0.3f)) {
                    effect->timer2++;
                }
            } else if (effect->type == BFD_FX_FIRE_BREATH) {
                diff.x = player->actor.world.pos.x - effect->pos.x;
                diff.y = player->actor.world.pos.y + 30.0f - effect->pos.y;
                diff.z = player->actor.world.pos.z - effect->pos.z;
                if ((this->timers[3] == 0) && (sqrtf(SQ(diff.x) + SQ(diff.y) + SQ(diff.z)) < 20.0f)) {
                    this->timers[3] = 50;
                    Actor_player_power_damage_AT_set(play, NULL, 5.0f, effect->kbAngle, 0.0f, 0x30);
                    if (!player->bodyIsBurning) {
                        s16 i2;

                        for (i2 = 0; i2 < PLAYER_BODYPART_MAX; i2++) {
                            player->bodyFlameTimers[i2] = get_random_timer(0, 200);
                        }
                        player->bodyIsBurning = true;
                    }
                }
                if (effect->timer2 == 0) {
                    if (effect->scale < 2.5f) {
                        effect->scale += effect->vFdFxScaleMod;
                        effect->vFdFxScaleMod += 0.08f;
                    }
                    if ((effect->pos.y <= (effect->vFdFxYStop + 130.0f)) || (effect->timer1 >= 10)) {
                        effect->accel.y = 5.0f;
                        effect->timer2++;
                        effect->velocity.y = 0.0f;
                        effect->accel.x = (effect->velocity.x * -25.0f) / 100.0f;
                        effect->accel.z = (effect->velocity.z * -25.0f) / 100.0f;
                    }
                } else {
                    if (effect->scale < 2.5f) {
                        add_calc2(&effect->scale, 2.5f, 0.5f, 0.5f);
                    }
                    effect->timer2++;
                    if (effect->timer2 >= 9) {
                        effect->type = 0;
                    }
                }
            }
        }
    }
}

static void Boss_Eff_disp(BossFdEffect* effect, PlayState* play) {
    static void* vb_smoke_txt[] = {
        gDust1Tex, gDust1Tex, gDust2Tex, gDust3Tex, gDust4Tex, gDust5Tex, gDust6Tex, gDust7Tex, gDust8Tex,
    };
    u8 materialFlag = 0;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 i;
    BossFdEffect* firstEffect = effect;

    OPEN_DISPS(gfxCtx, "../z_boss_fd.c", 4023);

    for (i = 0; i < BOSSFD_EFFECT_COUNT; i++, effect++) {
        if (effect->type == BFD_FX_EMBER) {
            if (materialFlag == 0) {
                _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaEmberMaterialDL);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, effect->color.r, effect->color.g, effect->color.b, effect->alpha);
            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_fd.c", 4046);
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaEmberModelDL);
        }
    }

    effect = firstEffect;
    materialFlag = 0;
    for (i = 0; i < BOSSFD_EFFECT_COUNT; i++, effect++) {
        if (effect->type == BFD_FX_DEBRIS) {
            if (materialFlag == 0) {
                _texture_z_light_fog_prim(play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gVolvagiaDebrisMaterialDL);
                materialFlag++;
            }

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotateY(effect->vFdFxRotY, MTXMODE_APPLY);
            Matrix_rotateX(effect->vFdFxRotX, MTXMODE_APPLY);
            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx, "../z_boss_fd.c", 4068);
            gSPDisplayList(POLY_OPA_DISP++, gVolvagiaDebrisModelDL);
        }
    }

    effect = firstEffect;
    materialFlag = 0;
    for (i = 0; i < BOSSFD_EFFECT_COUNT; i++, effect++) {
        if (effect->type == BFD_FX_DUST) {
            if (materialFlag == 0) {
                POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_0);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustMaterialDL);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 90, 30, 0, 255);
                gDPSetEnvColor(POLY_XLU_DISP++, 90, 30, 0, 0);
                materialFlag++;
            }

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_fd.c", 4104);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(vb_smoke_txt[effect->timer2]));
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustModelDL);
        }
    }

    effect = firstEffect;
    materialFlag = 0;
    for (i = 0; i < BOSSFD_EFFECT_COUNT; i++, effect++) {
        if (effect->type == BFD_FX_FIRE_BREATH) {
            if (materialFlag == 0) {
                POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_0);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 10, 0, 255);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, effect->alpha);
            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_fd.c", 4154);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(vb_smoke_txt[effect->timer2]));
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaDustModelDL);
        }
    }

    effect = firstEffect;
    materialFlag = 0;
    for (i = 0; i < BOSSFD_EFFECT_COUNT; i++, effect++) {
        if (effect->type == BFD_FX_SKULL_PIECE) {
            if (materialFlag == 0) {
                _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gVolvagiaSkullPieceMaterialDL);
                materialFlag++;
            }

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotateY(effect->vFdFxRotY, MTXMODE_APPLY);
            Matrix_rotateX(effect->vFdFxRotX, MTXMODE_APPLY);
            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_fd.c", 4192);
            gSPDisplayList(POLY_XLU_DISP++, gVolvagiaSkullPieceModelDL);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_boss_fd.c", 4198);
}

void Boss_Fd_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BossFd* this = (BossFd*)thisx;

    PRINTF("FD DRAW START\n");
    if (this->actionFunc != mode_wait) {
        OPEN_DISPS(play->state.gfxCtx, "../z_boss_fd.c", 4217);
        _texture_z_light_fog_prim(play->state.gfxCtx);
        if (this->work[BFD_DAMAGE_FLASH_TIMER] & 2) {
            POLY_OPA_DISP = gfx_set_fog_nosync(POLY_OPA_DISP, 255, 255, 255, 0, 900, 1099);
        }

        VB_draw(play, this);
        POLY_OPA_DISP = game_play_set_fog(play, POLY_OPA_DISP);
        CLOSE_DISPS(play->state.gfxCtx, "../z_boss_fd.c", 4243);
    }

    PRINTF("FD DRAW END\n");
    Boss_Eff_disp(this->effects, play);
    PRINTF("FD DRAW END2\n");
}

s32 Fd_Rarm_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossFd* this = (BossFd*)thisx;

    switch (limbIndex) {
        case 1:
            rot->y += 4000.0f + this->rightArmRot[0].x;
            break;
        case 2:
            rot->y += this->rightArmRot[1].x;
            rot->z += this->rightArmRot[1].y;
            break;
        case 3:
            rot->y += this->rightArmRot[2].x;
            rot->z += this->rightArmRot[2].y;
            break;
    }
    if (this->skinSegments < limbIndex) {
        *dList = NULL;
    }
    return false;
}

s32 Fd_Larm_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossFd* this = (BossFd*)thisx;

    switch (limbIndex) {
        case 1:
            rot->y += -4000.0f + this->leftArmRot[0].x;
            break;
        case 2:
            rot->y += this->leftArmRot[1].x;
            rot->z += this->leftArmRot[1].y;
            break;
        case 3:
            rot->y += this->leftArmRot[2].x;
            rot->z += this->leftArmRot[2].y;
            break;
    }
    if (this->skinSegments < limbIndex) {
        *dList = NULL;
    }
    return false;
}

static s16 SB_snakeAD[] = { 0, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 35, 30, 25, 20, 15, 10, 5 };
static s16 SB_sokAD[] = { 0, 28, 26, 24, 22, 20, 18, 16, 14, 12, 10 }; // Unused

void shok_disp_2(PlayState* play, BossFd* this, Vec3f* manePos, Vec3f* maneRot, f32* maneScale, u8 mode) {
    f32 sp140[] = { 0.0f, 10.0f, 17.0f, 20.0f, 19.5f, 18.0f, 17.0f, 15.0f, 15.0f, 15.0f };
    f32 sp118[] = { 0.0f, 10.0f, 17.0f, 20.0f, 21.0f, 21.0f, 21.0f, 21.0f, 21.0f, 21.0f };
    f32 spF0[] = { 0.4636457f, 0.3366129f, 0.14879614f, 0.04995025f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    // arctan of {0.5, 0.35, 0.15, 0.05, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
    f32 spC8[] = { -0.4636457f, -0.3366129f, -0.14879614f, 0.024927188f, 0.07478157f,
                   0.04995025f, 0.09961288f, 0.0f,         0.0f,         0.0f };
    // arctan of {-0.5, -0.35, -0.15, 0.025, 0.075, 0.05, 0.1, 0.0, 0.0}
    s16 maneIndex;
    s16 i;
    s16 maneLength;
    Vec3f spB4;
    Vec3f spA8;
    f32 phi_f20;
    f32 phi_f22;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_fd.c", 4419);

    maneLength = this->skinSegments;
    maneLength = CLAMP_MAX(maneLength, 10);

    for (i = 0; i < maneLength; i++) {
        maneIndex = (this->work[BFD_LEAD_MANE_SEG] - (i * 2) + 30) % 30;

        if (mode == 0) {
            spB4.x = spB4.z = 0.0f;
            spB4.y = ((sp140[i] * 0.1f) * 10.0f) * this->flattenMane;
            phi_f20 = 0.0f;
            phi_f22 = spC8[i] * this->flattenMane;
        } else if (mode == 1) {
            phi_f22 = (spC8[i] * this->flattenMane) * 0.7f;
            phi_f20 = spF0[i] * this->flattenMane;

            spB4.y = (sp140[i] * this->flattenMane) * 0.7f;
            spB4.x = -sp118[i] * this->flattenMane;
            spB4.z = 0.0f;
        } else {
            phi_f22 = (spC8[i] * this->flattenMane) * 0.7f;
            phi_f20 = -spF0[i] * this->flattenMane;

            spB4.y = (sp140[i] * this->flattenMane) * 0.7f;
            spB4.x = sp118[i] * this->flattenMane;
            spB4.z = 0.0f;
        }

        Matrix_rotateY((maneRot + maneIndex)->y, MTXMODE_NEW);
        Matrix_rotateX(-(maneRot + maneIndex)->x, MTXMODE_APPLY);

        Matrix_Position(&spB4, &spA8);

        Matrix_translate((manePos + maneIndex)->x + spA8.x, (manePos + maneIndex)->y + spA8.y,
                         (manePos + maneIndex)->z + spA8.z, MTXMODE_NEW);
        Matrix_rotateY((maneRot + maneIndex)->y + phi_f20, MTXMODE_APPLY);
        Matrix_rotateX(-((maneRot + maneIndex)->x + phi_f22), MTXMODE_APPLY);
        Matrix_scale(maneScale[maneIndex] * (0.01f - (i * 0.0008f)), maneScale[maneIndex] * (0.01f - (i * 0.0008f)),
                     0.01f, MTXMODE_APPLY);
        Matrix_rotateX(-M_PI / 2.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_fd.c", 4480);
        gSPDisplayList(POLY_XLU_DISP++, gVolvagiaManeModelDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_fd.c", 4483);
}

s32 vb_head_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossFd* this = (BossFd*)thisx;

    switch (limbIndex) {
        case 5:
        case 6:
            rot->z -= this->jawOpening * 0.1f;
            break;
        case 2:
            rot->z += this->jawOpening;
            break;
    }
    if ((this->faceExposed == true) && (limbIndex == 5)) {
        *dList = gVolvagiaBrokenFaceDL;
    }
    if (this->skinSegments == 0) {
        if (limbIndex == 6) {
            *dList = gVolvagiaSkullDL;
        } else if (limbIndex == 2) {
            *dList = gVolvagiaJawboneDL;
        } else {
            *dList = NULL;
        }
    }
    return false;
}

void vb_head_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f center_p = { 4500.0f, 0.0f, 0.0f };
    static Vec3f fire_p = { 4000.0f, 0.0f, 0.0f };
    BossFd* this = (BossFd*)thisx;

    if (limbIndex == 5) {
        Matrix_Position(&center_p, &this->actor.focus.pos);
        Matrix_Position(&fire_p, &this->headPos);
    }
}

static void* eye_tex_no[] = {
    gVolvagiaEyeOpenTex,
    gVolvagiaEyeHalfTex,
    gVolvagiaEyeClosedTex,
};

static Gfx* fd_body_gfx[] = {
    gVolvagiaBodySeg1DL,  gVolvagiaBodySeg2DL,  gVolvagiaBodySeg3DL,  gVolvagiaBodySeg4DL,  gVolvagiaBodySeg5DL,
    gVolvagiaBodySeg6DL,  gVolvagiaBodySeg7DL,  gVolvagiaBodySeg8DL,  gVolvagiaBodySeg9DL,  gVolvagiaBodySeg10DL,
    gVolvagiaBodySeg11DL, gVolvagiaBodySeg12DL, gVolvagiaBodySeg13DL, gVolvagiaBodySeg14DL, gVolvagiaBodySeg15DL,
    gVolvagiaBodySeg16DL, gVolvagiaBodySeg17DL, gVolvagiaBodySeg18DL,
};

void VB_draw(PlayState* play, BossFd* this) {
    s16 segIndex;
    s16 i;
    f32 temp_float;
    Mtx* tempMat = GRAPH_ALLOC(play->state.gfxCtx, 18 * sizeof(Mtx));

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_fd.c", 4589);
    if (this->skinSegments != 0) {
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eye_tex_no[this->eyeState]));
    }
    gSPSegment(POLY_OPA_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (s16)this->fwork[BFD_TEX1_SCROLL_X],
                                (s16)this->fwork[BFD_TEX1_SCROLL_Y], 0x20, 0x20, 1, (s16)this->fwork[BFD_TEX2_SCROLL_X],
                                (s16)this->fwork[BFD_TEX2_SCROLL_Y], 0x20, 0x20));
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, (s8)this->fwork[BFD_BODY_TEX2_ALPHA]);

    PRINTF("LH\n");
    Matrix_push();
    segIndex = (this->work[BFD_LEAD_BODY_SEG] + SB_snakeAD[2]) % 100;
    Matrix_translate(this->bodySegsPos[segIndex].x, this->bodySegsPos[segIndex].y, this->bodySegsPos[segIndex].z,
                     MTXMODE_NEW);
    Matrix_rotateY(this->bodySegsRot[segIndex].y, MTXMODE_APPLY);
    Matrix_rotateX(-this->bodySegsRot[segIndex].x, MTXMODE_APPLY);
    Matrix_translate(-13.0f, -5.0f, 13.0f, MTXMODE_APPLY);
    Matrix_scale(this->actor.scale.x * 0.1f, this->actor.scale.y * 0.1f, this->actor.scale.z * 0.1f, MTXMODE_APPLY);
    Si2_draw(play, this->skelAnimeRightArm.skeleton, this->skelAnimeRightArm.jointTable,
                      Fd_Rarm_sub, NULL, this);
    Matrix_pull();
    PRINTF("RH\n");
    Matrix_push();
    segIndex = (this->work[BFD_LEAD_BODY_SEG] + SB_snakeAD[2]) % 100;
    Matrix_translate(this->bodySegsPos[segIndex].x, this->bodySegsPos[segIndex].y, this->bodySegsPos[segIndex].z,
                     MTXMODE_NEW);
    Matrix_rotateY(this->bodySegsRot[segIndex].y, MTXMODE_APPLY);
    Matrix_rotateX(-this->bodySegsRot[segIndex].x, MTXMODE_APPLY);
    Matrix_translate(13.0f, -5.0f, 13.0f, MTXMODE_APPLY);
    Matrix_scale(this->actor.scale.x * 0.1f, this->actor.scale.y * 0.1f, this->actor.scale.z * 0.1f, MTXMODE_APPLY);
    Si2_draw(play, this->skelAnimeLeftArm.skeleton, this->skelAnimeLeftArm.jointTable,
                      Fd_Larm_sub, NULL, this);
    Matrix_pull();
    PRINTF("BD\n");
    gSPSegment(POLY_OPA_DISP++, 0x0D, tempMat);

    Matrix_push();
    for (i = 0; i < 18; i++, tempMat++) {
        segIndex = (this->work[BFD_LEAD_BODY_SEG] + SB_snakeAD[i + 1]) % 100;
        Matrix_translate(this->bodySegsPos[segIndex].x, this->bodySegsPos[segIndex].y, this->bodySegsPos[segIndex].z,
                         MTXMODE_NEW);
        Matrix_rotateY(this->bodySegsRot[segIndex].y, MTXMODE_APPLY);
        Matrix_rotateX(-this->bodySegsRot[segIndex].x, MTXMODE_APPLY);
        Matrix_translate(0.0f, 0.0f, 35.0f, MTXMODE_APPLY);
        Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        if (i < this->skinSegments) {
            Matrix_scale(1.0f + (sin_s((this->work[BFD_LEAD_BODY_SEG] * 5000.0f) + (i * 7000.0f)) *
                                 this->fwork[BFD_BODY_PULSE]),
                         1.0f + (sin_s((this->work[BFD_LEAD_BODY_SEG] * 5000.0f) + (i * 7000.0f)) *
                                 this->fwork[BFD_BODY_PULSE]),
                         1.0f, MTXMODE_APPLY);
            Matrix_rotateY(M_PI / 2.0f, MTXMODE_APPLY);
            MATRIX_TO_MTX(tempMat, "../z_boss_fd.c", 4719);
            gSPMatrix(POLY_OPA_DISP++, tempMat, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, fd_body_gfx[i]);
        } else {
            MtxF spFC;
            Vec3f spF0 = { 0.0f, 0.0f, 0.0f };
            Vec3f spE4;
            Vec3s spDC;
            f32 spD8;
            f32 spD4;

            if (this->bodyFallApart[i] < 2) {
                spD8 = spD4 = 0.1f;

                Matrix_translate(0.0f, 0.0f, -1100.0f, MTXMODE_APPLY);
                Matrix_rotateY(-M_PI, MTXMODE_APPLY);
                if (i >= 14) {
                    f32 sp84 = 1.0f - ((i - 14) * 0.2f);

                    Matrix_scale(sp84, sp84, 1.0f, MTXMODE_APPLY);
                    spD4 = spD8 = spD8 * sp84;
                }
                Matrix_scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_boss_fd.c", 4768);
                gSPDisplayList(POLY_OPA_DISP++, gVolvagiaRibsDL);

                if (this->bodyFallApart[i] == 1) {
                    EnVbBall* bones;

                    this->bodyFallApart[i] = 2;
                    Matrix_Position(&spF0, &spE4);
                    Matrix_get(&spFC);
                    Matrix_to_rotate_new(&spFC, &spDC, 0);
                    bones = (EnVbBall*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_VB_BALL, spE4.x,
                                                          spE4.y, spE4.z, spDC.x, spDC.y, spDC.z, i + 200);

                    bones->actor.scale.x = this->actor.scale.x * spD8;
                    bones->actor.scale.y = this->actor.scale.y * spD4;
                    bones->actor.scale.z = this->actor.scale.z * 0.1f;
                }
            }
        }
        if (i > 0) {
            CollisionCheck_Uty_convJntSphL2G(i + 1, &this->collider);
        }
    }
    Matrix_pull();
    PRINTF("BH\n");

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, (s8)this->fwork[BFD_HEAD_TEX2_ALPHA]);
    Matrix_push();
    temp_float =
        (this->work[BFD_ACTION_STATE] >= BOSSFD_SKULL_FALL) ? -20.0f : -10.0f - ((this->actor.speed - 5.0f) * 10.0f);
    segIndex = (this->work[BFD_LEAD_BODY_SEG] + SB_snakeAD[0]) % 100;
    Matrix_translate(this->bodySegsPos[segIndex].x, this->bodySegsPos[segIndex].y, this->bodySegsPos[segIndex].z,
                     MTXMODE_NEW);
    Matrix_rotateY(this->bodySegsRot[segIndex].y, MTXMODE_APPLY);
    Matrix_rotateX(-this->bodySegsRot[segIndex].x, MTXMODE_APPLY);
    Matrix_rotateZ(BINANG_TO_RAD_ALT(this->actor.shape.rot.z), MTXMODE_APPLY);
    Matrix_translate(0.0f, 0.0f, temp_float, MTXMODE_APPLY);
    Matrix_push();
    Matrix_translate(0.0f, 0.0f, 25.0f, MTXMODE_APPLY);
    PRINTF("BHC\n");
    CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
    Matrix_pull();
    PRINTF("BHCE\n");
    Matrix_scale(this->actor.scale.x * 0.1f, this->actor.scale.y * 0.1f, this->actor.scale.z * 0.1f, MTXMODE_APPLY);
    Si2_draw(play, this->skelAnimeHead.skeleton, this->skelAnimeHead.jointTable, vb_head_sub,
                      vb_head_sub2, &this->actor);
    PRINTF("SK\n");
    if (1) {
        Vec3f spB0 = { 0.0f, 1700.0f, 7000.0f };
        Vec3f spA4 = { -1000.0f, 700.0f, 7000.0f };

        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gVolvagiaManeMaterialDL);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, this->fwork[BFD_MANE_COLOR_CENTER], 0, 255);
        Matrix_push();
        Matrix_Position(&spB0, &this->centerMane.head);
        shok_disp_2(play, this, this->centerMane.pos, this->fireManeRot, this->centerMane.scale, MANE_CENTER);
        Matrix_pull();
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, this->fwork[BFD_MANE_COLOR_RIGHT], 0, 255);
        Matrix_push();
        Matrix_Position(&spA4, &this->rightMane.head);
        shok_disp_2(play, this, this->rightMane.pos, this->fireManeRot, this->rightMane.scale, MANE_RIGHT);
        Matrix_pull();
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, this->fwork[BFD_MANE_COLOR_LEFT], 0, 255);
        Matrix_push();
        spA4.x *= -1.0f;
        Matrix_Position(&spA4, &this->leftMane.head);
        shok_disp_2(play, this, this->leftMane.pos, this->fireManeRot, this->leftMane.scale, MANE_LEFT);
        Matrix_pull();
    }

    Matrix_pull();
    PRINTF("END\n");
    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_fd.c", 4987);
}
