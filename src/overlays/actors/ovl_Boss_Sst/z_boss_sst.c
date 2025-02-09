/*
 * File: z_boss_sst.c
 * Overlay: ovl_Boss_Sst
 * Description: Bongo Bongo
 */

#include "z_boss_sst.h"
#include "versions.h"
#include "assets/objects/object_sst/object_sst.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Bg_Sst_Floor/z_bg_sst_floor.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "pal-1.0:128 pal-1.1:128 hiratsu3:128"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

#define vParity actionVar
#define vVanish actionVar

#define LEFT 0
#define RIGHT 1
#define OTHER_HAND(hand) ((BossSst*)hand->actor.child)
#define HAND_STATE(hand) sst_hand_mode[hand->actor.params]

#define ROOM_CENTER_X -50.0f
#define ROOM_CENTER_Y 0.0f
#define ROOM_CENTER_Z 0.0f

typedef enum BossSstHandState {
    /*  0 */ HAND_WAIT,
    /*  1 */ HAND_BEAT,
    /*  2 */ HAND_RETREAT,
    /*  3 */ HAND_SLAM,
    /*  4 */ HAND_SWEEP,
    /*  5 */ HAND_PUNCH,
    /*  6 */ HAND_CLAP,
    /*  7 */ HAND_GRAB,
    /*  8 */ HAND_DAMAGED,
    /*  9 */ HAND_FROZEN,
    /* 10 */ HAND_BREAK_ICE,
    /* 11 */ HAND_DEATH
} BossSstHandState;

typedef enum BossSstEffectMode {
    /* 0 */ BONGO_NULL,
    /* 1 */ BONGO_ICE,
    /* 2 */ BONGO_SHOCKWAVE,
    /* 3 */ BONGO_SHADOW
} BossSstEffectMode;

void Boss_Sst_actor_ct(Actor* thisx, PlayState* play2);
void Boss_Sst_actor_dt(Actor* thisx, PlayState* play);
void Boss_Sst_hand_actor_move(Actor* thisx, PlayState* play);
void Boss_Sst_body_actor_move(Actor* thisx, PlayState* play2);
void Boss_Sst_hand_actor_draw(Actor* thisx, PlayState* play);
void Boss_Sst_body_actor_draw(Actor* thisx, PlayState* play);
void Boss_Sst_eff_move(Actor* thisx, PlayState* play);
void Boss_Sst_eff_draw(Actor* thisx, PlayState* play);

void sst_body_SE_set(BossSst* this, u16 sfxId);

void mode_B_start_demo_wait_init(BossSst* this);
void mode_B_start_demo_wait(BossSst* this, PlayState* play);
void mode_B_start_demo_init(BossSst* this, PlayState* play);
void mode_B_start_demo(BossSst* this, PlayState* play);
void mode_B_move_init(BossSst* this);
void mode_B_move(BossSst* this, PlayState* play);
void mode_B_wait(BossSst* this, PlayState* play);

void mode_B_h_damage_init(BossSst* this, s32 bothHands);
void mode_B_h_damage(BossSst* this, PlayState* play);
void mode_B_attack_ready_init(BossSst* this);
void mode_B_attack_ready(BossSst* this, PlayState* play);
void mode_B_attack_init(BossSst* this);
void mode_B_attack(BossSst* this, PlayState* play);
void mode_B_attack_after_init(BossSst* this);
void mode_B_attack_after(BossSst* this, PlayState* play);

void mode_B_h_freeze_wait_init(BossSst* this);
void mode_B_h_freeze_wait(BossSst* this, PlayState* play);
void mode_B_close_eye_init(BossSst* this);
void mode_B_close_eye(BossSst* this, PlayState* play);

void mode_B_arrow_hit(BossSst* this, PlayState* play);
void mode_B_damage_wait_init(BossSst* this);
void mode_B_damage_wait(BossSst* this, PlayState* play);
void mode_B_sword_hit(BossSst* this, PlayState* play);
void mode_B_return_init(BossSst* this);
void mode_B_return(BossSst* this, PlayState* play);

void mode_B_final_damage(BossSst* this, PlayState* play);
void mode_B_dead_move_init(BossSst* this);
void mode_B_dead_move(BossSst* this, PlayState* play);
void mode_B_dead_stop_init(BossSst* this);
void mode_B_dead_stop(BossSst* this, PlayState* play);
void mode_B_dead_drop_init(BossSst* this);
void mode_B_dead_drop(BossSst* this, PlayState* play);
void mode_B_dead_scale_init(BossSst* this);
void mode_B_dead_scale(BossSst* this, PlayState* play);
void mode_B_dead_shadow_init(BossSst* this);
void mode_B_dead_shadow(BossSst* this, PlayState* play);

void sst_hand_set_player_catch(BossSst* this, PlayState* play);
void sst_hand_reset_player_catch(BossSst* this, PlayState* play, s32 dropPlayer);
void sst_hand_change_attack(BossSst* this);
void sst_hand_set_at_check(BossSst* this, s32 damage);
void sst_set_ac_shield(BossSst* this, s32 isInv);

void mode_H_wait_init(BossSst* this);
void mode_H_wait(BossSst* this, PlayState* play);
void mode_H_r_drum_move_init(BossSst* this);
void mode_H_r_drum_move(BossSst* this, PlayState* play);
void mode_H_l_drum_move_init(BossSst* this);
void mode_H_l_drum_move(BossSst* this, PlayState* play);
void mode_H_r_drum_wait_init(BossSst* this);
void mode_H_r_drum_wait(BossSst* this, PlayState* play);
void mode_H_l_drum_wait_init(BossSst* this);
void mode_H_l_drum_wait(BossSst* this, PlayState* play);

void mode_H_press_ready(BossSst* this, PlayState* play);
void mode_H_press_init(BossSst* this);
void mode_H_press(BossSst* this, PlayState* play);
void mode_H_return_ready(BossSst* this, PlayState* play);

void mode_H_chop_ready(BossSst* this, PlayState* play);
void mode_H_chop_init(BossSst* this);
void mode_H_chop(BossSst* this, PlayState* play);

void mode_H_punch_ready(BossSst* this, PlayState* play);
void mode_H_punch_init(BossSst* this);
void mode_H_punch(BossSst* this, PlayState* play);

void mode_H_crush_ready(BossSst* this, PlayState* play);
void mode_H_crush_init(BossSst* this);
void mode_H_crush(BossSst* this, PlayState* play);
void mode_H_crush_after_init(BossSst* this);
void mode_H_crush_after(BossSst* this, PlayState* play);

void mode_H_catch_ready(BossSst* this, PlayState* play);
void mode_H_catch_init(BossSst* this);
void mode_H_catch(BossSst* this, PlayState* play);
void mode_H_grip_init(BossSst* this);
void mode_H_grip(BossSst* this, PlayState* play);
void mode_H_grip_after_init(BossSst* this);
void mode_H_grip_after(BossSst* this, PlayState* play);
void mode_H_shake_throw_init(BossSst* this);
void mode_H_shake_throw(BossSst* this, PlayState* play);

void mode_H_return_init(BossSst* this);
void mode_H_return(BossSst* this, PlayState* play);

void mode_H_damage_init(BossSst* this);
void mode_H_damage(BossSst* this, PlayState* play);
void mode_H_damage_after_init(BossSst* this);
void mode_H_damage_after(BossSst* this, PlayState* play);
void mode_H_swing_init(BossSst* this);
void mode_H_swing(BossSst* this, PlayState* play);
void mode_H_b_attack_init(BossSst* this);
void mode_H_b_attack(BossSst* this, PlayState* play);

void mode_H_freeze_init(BossSst* this);
void mode_H_freeze(BossSst* this, PlayState* play);
void mode_H_f_punch_ready_init(BossSst* this);
void mode_H_f_punch_ready(BossSst* this, PlayState* play);
void mode_H_f_punch_init(BossSst* this);
void mode_H_f_punch(BossSst* this, PlayState* play);

void mode_H_b_damage_wait(BossSst* this, PlayState* play);
void mode_H_b_damage(BossSst* this, PlayState* play);
void mode_H_b_return_init(BossSst* this);
void mode_H_b_return(BossSst* this, PlayState* play);

void mode_H_dead_move_init(BossSst* this);
void mode_H_dead_move(BossSst* this, PlayState* play);
void mode_H_dead_stop_init(BossSst* this);
void mode_H_dead_stop(BossSst* this, PlayState* play);
void mode_H_dead_drop_init(BossSst* this);
void mode_H_dead_drop(BossSst* this, PlayState* play);
void mode_H_dead_scale_init(BossSst* this);
void mode_H_dead_scale(BossSst* this, PlayState* play);
void mode_H_dead_shadow_init(BossSst* this);
void mode_H_dead_shadow(BossSst* this, PlayState* play);

void sst_set_eff_body_dead_shadow(BossSst* this);
void sst_set_eff_hand_dead_shadow(BossSst* this);
void sst_set_eff_blast(BossSst* this);
void sst_set_eff_ice(BossSst* this, s32 index);
void sst_set_eff_ice_piece(BossSst* this);
void sst_eff_ice_broken(BossSst* this);

#include "assets/overlays/ovl_Boss_Sst/z_boss_sst.c"

static BossSst* sst_body;
static BossSst* sst_hand[2];
static BgSstFloor* bg_floor;

static Vec3f map_center_pos = { ROOM_CENTER_X, ROOM_CENTER_Y, ROOM_CENTER_Z };
static Vec3f hand_pos[2];
static s16 hand_shape_offset_angle[2];

static s16 demo_camera;
static Vec3f center_pos = { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 0.0f, ROOM_CENTER_Z + 0.0f };
static Vec3f eye_pos = { ROOM_CENTER_X + 150.0f, ROOM_CENTER_Y + 100.0f, ROOM_CENTER_Z + 0.0f };
static Vec3f inc_center = { 0.0f, 0.0f, 0.0f };
static Vec3f inc_eye = { 0.0f, 0.0f, 0.0f };

static Vec3f end_demo_center[] = {
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 300.0f, ROOM_CENTER_Z + 0.0f },
    { ROOM_CENTER_X + 150.0f, ROOM_CENTER_Y + 300.0f, ROOM_CENTER_Z + 100.0f },
    { ROOM_CENTER_X + 0.0f, ROOM_CENTER_Y + 600.0f, ROOM_CENTER_Z + 100.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 400.0f, ROOM_CENTER_Z + 200.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 200.0f },
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 0.0f, ROOM_CENTER_Z + 200.0f },
    { ROOM_CENTER_X - 150.0f, ROOM_CENTER_Y + 0.0f, ROOM_CENTER_Z + 100.0f },
    { ROOM_CENTER_X - 60.0f, ROOM_CENTER_Y + 180.0f, ROOM_CENTER_Z + 730.0f },
};

static Vec3f end_demo_eye[] = {
    { ROOM_CENTER_X + 250.0f, ROOM_CENTER_Y + 800.0f, ROOM_CENTER_Z + 800.0f },
    { ROOM_CENTER_X - 150.0f, ROOM_CENTER_Y + 700.0f, ROOM_CENTER_Z + 1400.0f },
    { ROOM_CENTER_X + 250.0f, ROOM_CENTER_Y + 100.0f, ROOM_CENTER_Z + 750.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 900.0f },
    { ROOM_CENTER_X + 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 900.0f },
    { ROOM_CENTER_X + 350.0f, ROOM_CENTER_Y + 400.0f, ROOM_CENTER_Z + 1200.0f },
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 800.0f },
    { ROOM_CENTER_X - 50.0f, ROOM_CENTER_Y + 200.0f, ROOM_CENTER_Z + 800.0f },
};

static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };
static u32 tex_change_flg = false;

// Unreferenced. Maybe two zero vectors?
static u32 old_hand_angle[] = { 0, 0, 0, 0, 0, 0 };

static Color_RGBA8 dead_prim_color = { 255, 255, 255, 255 };
static Color_RGBA8 dead_env_color = { 0, 0, 0, 255 };
static s32 sst_hand_mode[] = { HAND_WAIT, HAND_WAIT };

ActorProfile Boss_Sst_Profile = {
    /**/ ACTOR_BOSS_SST,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_SST,
    /**/ sizeof(BossSst),
    /**/ Boss_Sst_actor_ct,
    /**/ Boss_Sst_actor_dt,
    /**/ Boss_Sst_hand_actor_move,
    /**/ Boss_Sst_hand_actor_draw,
};

#include "z_boss_sst.inc.c"

static AnimationHeader* sst_hand_pose_a[] = { &gBongoLeftHandIdleAnim, &gBongoRightHandIdleAnim };
static AnimationHeader* sst_hand_pose_b[] = { &gBongoLeftHandFlatPoseAnim, &gBongoRightHandFlatPoseAnim };
static AnimationHeader* sst_hand_pose_c[] = { &gBongoLeftHandOpenPoseAnim, &gBongoRightHandOpenPoseAnim };
static AnimationHeader* sst_hand_pose_d[] = { &gBongoLeftHandFistPoseAnim, &gBongoRightHandFistPoseAnim };
static AnimationHeader* sst_hand_pose_e[] = { &gBongoLeftHandClenchAnim, &gBongoRightHandClenchAnim };
static AnimationHeader* sst_hand_pose_f[] = { &gBongoLeftHandDamagePoseAnim, &gBongoRightHandDamagePoseAnim };
static AnimationHeader* sst_hand_pose_g[] = { &gBongoLeftHandPushoffPoseAnim, &gBongoRightHandPushoffPoseAnim };
static AnimationHeader* sst_hand_pose_h[] = { &gBongoLeftHandHangPoseAnim, &gBongoRightHandHangPoseAnim };

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_BONGO_BONGO, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 20, ICHAIN_STOP),
};

void Boss_Sst_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossSst* this = (BossSst*)thisx;

    ValueSet_process(&this->actor, value_init);
    ClObjPipe_ct(play, &this->colliderCyl);
    ClObjJntSph_ct(play, &this->colliderJntSph);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, &SstBtlData, &SstStatusData);
    Actor_Environment_sw_On(play, 0x14);
    if (this->actor.params == BONGO_HEAD) {
        bg_floor = (BgSstFloor*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_SST_FLOOR, map_center_pos.x, map_center_pos.y,
                                          map_center_pos.z, 0, 0, 0, BONGOFLOOR_REST);
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBongoHeadSkel, &gBongoHeadEyeOpenIdleAnim, this->jointTable,
                           this->morphTable, 45);
        Shape_Info_init(&this->actor.shape, 70000.0f, Actor_shadow_circle, 95.0f);
        ClObjJntSph_set5_nzm(play, &this->colliderJntSph, &this->actor, &SstBodyAllJntSphData, this->colliderItems);
        ClObjPipe_set5(play, &this->colliderCyl, &this->actor, &SstBodyEyeAcOcPipeData);
        sst_body = this;
        this->actor.world.pos.x = ROOM_CENTER_X + 50.0f;
        this->actor.world.pos.y = ROOM_CENTER_Y + 0.0f;
        this->actor.world.pos.z = ROOM_CENTER_Z - 650.0f;
        this->actor.home.pos = this->actor.world.pos;
        this->actor.shape.rot.y = 0;
        if (Actor_Environment_room_clear_Check(play, play->roomCtx.curRoom.num)) {
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_WARP1, ROOM_CENTER_X, ROOM_CENTER_Y, ROOM_CENTER_Z + 400.0f,
                        0, 0, 0, WARP_DUNGEON_ADULT);
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, ROOM_CENTER_X, ROOM_CENTER_Y, ROOM_CENTER_Z - 200.0f,
                        0, 0, 0, 0);
            Actor_delete(&this->actor);
        } else {
            sst_hand[LEFT] = (BossSst*)Actor_info_make_actor(
                &play->actorCtx, play, ACTOR_BOSS_SST, this->actor.world.pos.x + 200.0f, this->actor.world.pos.y,
                this->actor.world.pos.z + 400.0f, 0, this->actor.shape.rot.y, 0, BONGO_LEFT_HAND);
            sst_hand[RIGHT] = (BossSst*)Actor_info_make_actor(
                &play->actorCtx, play, ACTOR_BOSS_SST, this->actor.world.pos.x + (-200.0f), this->actor.world.pos.y,
                this->actor.world.pos.z + 400.0f, 0, this->actor.shape.rot.y, 0, BONGO_RIGHT_HAND);
            sst_hand[LEFT]->actor.child = &sst_hand[RIGHT]->actor;
            sst_hand[RIGHT]->actor.child = &sst_hand[LEFT]->actor;

            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.update = Boss_Sst_body_actor_move;
            this->actor.draw = Boss_Sst_body_actor_draw;
            this->radius = -650.0f;
            this->actor.lockOnArrowOffset = 4000.0f;
            mode_B_start_demo_wait_init(this);
            Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_BOSS);
        }
    } else {
        ClObjJntSph_set5_nzm(play, &this->colliderJntSph, &this->actor, &SstHandAllJntSphData, this->colliderItems);
        ClObjPipe_set5(play, &this->colliderCyl, &this->actor, &SstBlastAtPipeData);
        if (this->actor.params == BONGO_LEFT_HAND) {
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBongoLeftHandSkel, &gBongoLeftHandIdleAnim, this->jointTable,
                               this->morphTable, 27);
            this->vParity = -1;
            this->colliderJntSph.elements[0].dim.modelSphere.center.z *= -1;
        } else {
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBongoRightHandSkel, &gBongoRightHandIdleAnim, this->jointTable,
                               this->morphTable, 27);
            this->vParity = 1;
        }

        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 95.0f);
        this->handZPosMod = -3500;
        this->actor.lockOnArrowOffset = 5000.0f;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        mode_H_wait_init(this);
    }
}

void Boss_Sst_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BossSst* this = (BossSst*)thisx;

    ClObjJntSph_dt_nzf(play, &this->colliderJntSph);
    ClObjPipe_dt(play, &this->colliderCyl);
    Nai_StopAllObjFx(&this->center);
}

void mode_B_start_demo_wait_init(BossSst* this) {
    this->actor.draw = NULL;
    sst_hand[LEFT]->actor.draw = NULL;
    sst_hand[RIGHT]->actor.draw = NULL;
    this->vVanish = false;
    this->actionFunc = mode_B_start_demo_wait;
}

void mode_B_start_demo_wait(BossSst* this, PlayState* play) {
    if (this->actor.yDistToPlayer < 1000.0f) {
        mode_B_start_demo_init(this, play);
    }
}

void mode_B_start_demo_init(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->timer = 611;
    this->ready = false;
    player->actor.world.pos.x = map_center_pos.x;
    player->actor.world.pos.y = ROOM_CENTER_Y + 1000.0f;
    player->actor.world.pos.z = map_center_pos.z;
    player->speedXZ = 0.0f;
    player->actor.shape.rot.y = -0x8000;
    player->parallelYaw = -0x8000;
    player->yaw = -0x8000;
    player->actor.velocity.y = 0.0f;
    player->fallStartHeight = 0;
    player->stateFlags1 |= PLAYER_STATE1_5;

    Demo_play_start(play, &play->csCtx);
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
    demo_camera = Gama_play_make_camera(play);
    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
    Gama_play_set_camera_status(play, demo_camera, CAM_STAT_ACTIVE);
    xyz_t_move(&center_pos, &player->actor.world.pos);
    if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_BONGO_BONGO_BATTLE)) {
        eye_pos.z = ROOM_CENTER_Z - 100.0f;
    }

    Gama_play_camera_setting(play, demo_camera, &center_pos, &eye_pos);
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
    this->actionFunc = mode_B_start_demo;
}

void mode_B_start_demo(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 tempo;
    s32 introStateTimer;
    s32 revealStateTimer;

    if (this->timer != 0) {
        this->timer--;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gBongoHeadEyeCloseIdleAnim, -3.0f);
    }

    if (this->timer == 0) {
        sst_hand[RIGHT]->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        sst_hand[LEFT]->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        player->stateFlags1 &= ~PLAYER_STATE1_5;
        Demo_play_end(play, &play->csCtx);
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
        center_pos.y += 30.0f;
        center_pos.z += 300.0f;
        Gama_play_camera_setting(play, demo_camera, &center_pos, &eye_pos);
        Gama_play_copy_camera_position(play, CAM_ID_MAIN, demo_camera);
        Gama_play_set_camera_status(play, demo_camera, CAM_STAT_WAIT);
        Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_ACTIVE);
        Gama_play_clear_camera(play, demo_camera);
        SET_EVENTCHKINF(EVENTCHKINF_BEGAN_BONGO_BONGO_BATTLE);
        mode_B_move_init(this);
        this->colliderJntSph.base.ocFlags1 |= OC1_ON;
        sst_hand[LEFT]->colliderJntSph.base.ocFlags1 |= OC1_ON;
        sst_hand[RIGHT]->colliderJntSph.base.ocFlags1 |= OC1_ON;
        this->timer = 112;
    } else if (this->timer >= 546) {
        if (player->actor.world.pos.y > 100.0f) {
            player->actor.world.pos.x = map_center_pos.x;
            player->actor.world.pos.z = map_center_pos.z;
            player->speedXZ = 0;
            player->actor.shape.rot.y = -0x8000;
            player->parallelYaw = -0x8000;
            player->yaw = -0x8000;
        }

        xyz_t_move(&center_pos, &player->actor.world.pos);
        if (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            if (!this->ready) {
                bg_floor->dyna.actor.params = BONGOFLOOR_HIT;
                this->ready = true;
                z_vibctl2_vib_setQ(this->actor.xyzDistToPlayerSq, 255, 20, 150);
                Actor_SE_set(&bg_floor->dyna.actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
            } else if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_BONGO_BONGO_BATTLE)) {
                //! @bug This condition assumes that the second bounce on the ground will occur before frame 545 on the
                //! timer. However, it is possible to delay Player's descent to the ground by, for example, jumpslashing
                //! on the last possible frame before the cutscene takes control. This delays Player's fall to the
                //! ground by enough time such that the second bounce will occur after the timer has decremented past
                //! 546. The end result is that the cutscene will not be shortened like it should even though the flag
                //! is set.
                sst_hand[RIGHT]->actor.draw = Boss_Sst_hand_actor_draw;
                sst_hand[LEFT]->actor.draw = Boss_Sst_hand_actor_draw;
                this->actor.draw = Boss_Sst_body_actor_draw;
                this->timer = 178;
                center_pos.x = ROOM_CENTER_X - 23.0f;
                center_pos.y = ROOM_CENTER_Y + 0.0f;
                center_pos.z = ROOM_CENTER_Z + 0.0f;
            } else {
                this->timer = 546;
            }
        }
    } else if (this->timer >= 478) {
        eye_pos.x += 10.0f;
        eye_pos.y += 10.0f;
        eye_pos.z -= 10.0f;
    } else if (this->timer >= 448) {
        if (this->timer == 460) {
            sst_hand[RIGHT]->actor.draw = Boss_Sst_hand_actor_draw;
            sst_hand[LEFT]->actor.draw = Boss_Sst_hand_actor_draw;
            this->actor.draw = Boss_Sst_body_actor_draw;
            player->actor.world.pos.x = map_center_pos.x;
            player->actor.world.pos.z = map_center_pos.z;
            mode_H_r_drum_move_init(sst_hand[RIGHT]);
        }
        if (this->timer > 460) {
            eye_pos.x -= 40.0f;
            eye_pos.y -= 40.0f;
            eye_pos.z += 20.0f;
        } else if (this->timer == 460) {
            center_pos.x = sst_hand[RIGHT]->actor.home.pos.x + 0.0f;
            center_pos.y = sst_hand[RIGHT]->actor.home.pos.y - 20.0f;
            center_pos.z = sst_hand[RIGHT]->actor.home.pos.z + 10.0f;
            eye_pos.x = sst_hand[RIGHT]->actor.home.pos.x + 150.0f;
            eye_pos.y = sst_hand[RIGHT]->actor.home.pos.y + 100.0f;
            eye_pos.z = sst_hand[RIGHT]->actor.home.pos.z + 80.0f;
        }
    } else {
        if (this->timer >= 372) {
            introStateTimer = this->timer - 372;
            tempo = 6;
            if (this->timer == 447) {
                center_pos = player->actor.world.pos;
                eye_pos.x = ROOM_CENTER_X - 200.0f;
                eye_pos.y = ROOM_CENTER_Y + 160.0f;
                eye_pos.z = ROOM_CENTER_Z - 190.0f;
            } else if (introStateTimer == 11) {
                center_pos.x = sst_hand[RIGHT]->actor.home.pos.x + 30.0f;
                center_pos.y = sst_hand[RIGHT]->actor.home.pos.y + 0.0f;
                center_pos.z = sst_hand[RIGHT]->actor.home.pos.z + 20.0f;
                eye_pos.x = sst_hand[RIGHT]->actor.home.pos.x + 100.0f;
                eye_pos.y = sst_hand[RIGHT]->actor.home.pos.y + 10.0f;
                eye_pos.z = sst_hand[RIGHT]->actor.home.pos.z - 210.0f;
            } else if (introStateTimer == 62) {
                center_pos.x = sst_hand[LEFT]->actor.home.pos.x + 0.0f;
                center_pos.y = sst_hand[LEFT]->actor.home.pos.y + 50.0f;
                center_pos.z = sst_hand[LEFT]->actor.home.pos.z + 100.0f;
                eye_pos.x = sst_hand[LEFT]->actor.home.pos.x + 110.0f;
                eye_pos.y = sst_hand[LEFT]->actor.home.pos.y + 180.0f;
                eye_pos.z = sst_hand[LEFT]->actor.home.pos.z - 70.0f;
            }
        } else if (this->timer >= 304) {
            introStateTimer = this->timer - 304;
            tempo = 5;
            if (introStateTimer == 11) {
                center_pos.x = sst_hand[RIGHT]->actor.home.pos.x + 40.0f;
                center_pos.y = sst_hand[RIGHT]->actor.home.pos.y - 90.0f;
                center_pos.z = sst_hand[RIGHT]->actor.home.pos.z - 40.0f;
                eye_pos.x = sst_hand[RIGHT]->actor.home.pos.x - 20.0f;
                eye_pos.y = sst_hand[RIGHT]->actor.home.pos.y + 210.0f;
                eye_pos.z = sst_hand[RIGHT]->actor.home.pos.z + 170.0f;
            } else if (this->timer == 368) {
                center_pos.x = sst_hand[LEFT]->actor.home.pos.x - 20.0f;
                center_pos.y = sst_hand[LEFT]->actor.home.pos.y + 0.0f;
                center_pos.z = sst_hand[LEFT]->actor.home.pos.z + 0.0f;
                eye_pos.x = sst_hand[LEFT]->actor.home.pos.x - 70.0f;
                eye_pos.y = sst_hand[LEFT]->actor.home.pos.y + 170.0f;
                eye_pos.z = sst_hand[LEFT]->actor.home.pos.z + 150.0f;
            }
        } else if (this->timer >= 244) {
            introStateTimer = this->timer - 244;
            tempo = 4;
            if (introStateTimer == 11) {
                center_pos.x = sst_hand[RIGHT]->actor.home.pos.x + 30.0f;
                center_pos.y = sst_hand[RIGHT]->actor.home.pos.y + 70.0f;
                center_pos.z = sst_hand[RIGHT]->actor.home.pos.z + 40.0f;
                eye_pos.x = sst_hand[RIGHT]->actor.home.pos.x + 110.0f;
                eye_pos.y = sst_hand[RIGHT]->actor.home.pos.y - 140.0f;
                eye_pos.z = sst_hand[RIGHT]->actor.home.pos.z - 10.0f;
            } else if (this->timer == 300) {
                center_pos.x = sst_hand[LEFT]->actor.home.pos.x - 20.0f;
                center_pos.y = sst_hand[LEFT]->actor.home.pos.y - 80.0f;
                center_pos.z = sst_hand[LEFT]->actor.home.pos.z + 320.0f;
                eye_pos.x = sst_hand[LEFT]->actor.home.pos.x - 130.0f;
                eye_pos.y = sst_hand[LEFT]->actor.home.pos.y + 130.0f;
                eye_pos.z = sst_hand[LEFT]->actor.home.pos.z - 150.0f;
            }
        } else if (this->timer >= 192) {
            introStateTimer = this->timer - 192;
            tempo = 3;
            if (this->timer == 240) {
                center_pos.x = sst_hand[LEFT]->actor.home.pos.x - 190.0f;
                center_pos.y = sst_hand[LEFT]->actor.home.pos.y - 110.0f;
                center_pos.z = sst_hand[LEFT]->actor.home.pos.z + 40.0f;
                eye_pos.x = sst_hand[LEFT]->actor.home.pos.x + 120.0f;
                eye_pos.y = sst_hand[LEFT]->actor.home.pos.y + 130.0f;
                eye_pos.z = sst_hand[LEFT]->actor.home.pos.z + 50.0f;
            } else if (introStateTimer == 12) {
                center_pos.x = map_center_pos.x + 50.0f;
                center_pos.y = map_center_pos.y - 90.0f;
                center_pos.z = map_center_pos.z - 200.0f;
                eye_pos.x = map_center_pos.x + 50.0f;
                eye_pos.y = map_center_pos.y + 350.0f;
                eye_pos.z = map_center_pos.z + 150.0f;
            }
        } else if (this->timer >= 148) {
            introStateTimer = this->timer - 148;
            tempo = 2;
        } else if (this->timer >= 112) {
            introStateTimer = this->timer - 112;
            tempo = 1;
        } else {
            introStateTimer = this->timer % 28;
            tempo = 0;
        }
        if (this->timer <= 198) {
            revealStateTimer = 198 - this->timer;
            if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_BONGO_BONGO_BATTLE) && (revealStateTimer <= 44)) {
                center_pos.x += 492.0f * 0.01f;
                center_pos.y += 200.0f * 0.01f;
                eye_pos.x -= 80.0f * 0.01f;
                eye_pos.y -= 360.0f * 0.01f;
                eye_pos.z += 1000.0f * 0.01f;
            } else if (this->timer <= 20) {
                center_pos.y -= 700.0f * 0.01f;
                center_pos.z += 900.0f * 0.01f;
                eye_pos.x += 650.0f * 0.01f;
                eye_pos.y += 400.0f * 0.01f;
                eye_pos.z += 1550.0f * 0.01f;
                this->vVanish = true;
                this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
            } else if (revealStateTimer < 40) {
                center_pos.x += 125.0f * 0.01f;
                center_pos.y += 350.0f * 0.01f;
                center_pos.z += 500.0f * 0.01f;
                eye_pos.x += 200.0f * 0.01f;
                eye_pos.y -= 850.0f * 0.01f;
            } else if (revealStateTimer >= 45) {
                if (revealStateTimer < 85) {
                    center_pos.x -= 250.0f * 0.01f;
                    center_pos.y += 425.0f * 0.01f;
                    center_pos.z -= 1200.0f * 0.01f;
                    eye_pos.x -= 650.0f * 0.01f;
                    eye_pos.y += 125.0f * 0.01f;
                    eye_pos.z -= 350.0f * 0.01f;
                } else if (revealStateTimer == 85) {
                    if (!GET_EVENTCHKINF(EVENTCHKINF_BEGAN_BONGO_BONGO_BATTLE)) {
                        Actor_Name_Disp_Set(play, &play->actorCtx.titleCtx, SEGMENTED_TO_VIRTUAL(gBongoTitleCardTex),
                                               160, 180, 128, 40);
                    }
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS);
                    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gBongoHeadEyeCloseAnim, -5.0f);
                    sst_body_SE_set(this, NA_SE_EN_SHADEST_DISAPPEAR);
                }
            }
        }
        if (introStateTimer == 12) {
            mode_H_r_drum_move_init(sst_hand[RIGHT]);
        }
        if ((introStateTimer != 5) && ((introStateTimer % ((tempo * 2) + 7)) == 5)) {
            mode_H_l_drum_move_init(sst_hand[LEFT]);
        }
    }

    if (this->actionFunc != mode_B_move) {
        Gama_play_camera_setting(play, demo_camera, &center_pos, &eye_pos);
    }
}

void mode_B_wait_init(BossSst* this) {
    if (this->skelAnime.animation != &gBongoHeadEyeCloseIdleAnim) {
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gBongoHeadEyeCloseIdleAnim, -5.0f);
    }
    this->actionFunc = mode_B_wait;
}

void mode_B_wait(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((HAND_STATE(sst_hand[LEFT]) == HAND_WAIT) && (HAND_STATE(sst_hand[RIGHT]) == HAND_WAIT)) {
        mode_B_move_init(this);
    }
}

void mode_B_move_init(BossSst* this) {
    this->timer = 127;
    this->ready = false;
    this->actionFunc = mode_B_move;
}

void mode_B_move(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (!this->ready && ((HAND_STATE(sst_hand[LEFT]) == HAND_BEAT) || (HAND_STATE(sst_hand[LEFT]) == HAND_WAIT)) &&
        ((HAND_STATE(sst_hand[RIGHT]) == HAND_BEAT) || (HAND_STATE(sst_hand[RIGHT]) == HAND_WAIT))) {
        this->ready = true;
    }

    if (this->ready) {
        if (this->timer != 0) {
            this->timer--;
        }
    }

    if (this->timer == 0) {
        Player* player = GET_PLAYER(play);

        if ((player->actor.world.pos.y > -50.0f) &&
            !(player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_13 | PLAYER_STATE1_14))) {
            sst_hand[fqrand() <= 0.5f]->ready = true;
            mode_B_wait_init(this);
        } else {
            this->timer = 28;
        }
    } else {
        adds(&this->actor.shape.rot.y,
                       Actor_search_position_angleY(&GET_PLAYER(play)->actor, &map_center_pos) + 0x8000, 4, 0x400);
        if ((this->timer == 28) || (this->timer == 84)) {
            sst_body_SE_set(this, NA_SE_EN_SHADEST_PRAY);
        }
    }
}

void mode_B_h_damage_init(BossSst* this, s32 bothHands) {
    if (bothHands) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gBongoHeadEyeOpenAnim, -5.0f);
    } else {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gBongoHeadDamagedHandAnim, -5.0f);
    }
    this->actionFunc = mode_B_h_damage;
}

void mode_B_h_damage(BossSst* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if ((HAND_STATE(sst_hand[LEFT]) == HAND_DAMAGED) && (HAND_STATE(sst_hand[RIGHT]) == HAND_DAMAGED)) {
            mode_B_attack_ready_init(this);
        } else if ((HAND_STATE(sst_hand[LEFT]) == HAND_FROZEN) || (HAND_STATE(sst_hand[RIGHT]) == HAND_FROZEN)) {
            mode_B_h_freeze_wait_init(this);
        } else if (this->skelAnime.animation == &gBongoHeadEyeOpenAnim) {
            mode_B_close_eye_init(this);
        } else {
            mode_B_wait_init(this);
        }
    }
}

void mode_B_attack_ready_init(BossSst* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    this->actor.speed = 0.0f;
    this->colliderCyl.base.acFlags |= AC_ON;
    this->actionFunc = mode_B_attack_ready;
}

void mode_B_attack_ready(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (sst_hand[LEFT]->ready && (sst_hand[LEFT]->actionFunc == mode_H_b_attack) && sst_hand[RIGHT]->ready &&
        (sst_hand[RIGHT]->actionFunc == mode_H_b_attack)) {
        mode_B_attack_init(this);
    } else {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0x800, 0x400);
    }
}

void mode_B_attack_init(BossSst* this) {
    Skeleton_Info2_init(&this->skelAnime, &gBongoHeadChargeAnim, 0.5f, 0.0f, Si2_anime_end_frame(&gBongoHeadChargeAnim),
                     ANIMMODE_ONCE_INTERP, -5.0f);
    sst_hand_set_at_check(sst_hand[LEFT], 0x20);
    sst_hand_set_at_check(sst_hand[RIGHT], 0x20);
    this->colliderJntSph.base.atFlags |= AT_ON;
    this->actor.speed = 3.0f;
    this->radius = -650.0f;
    this->ready = false;
    this->actionFunc = mode_B_attack;
}

void mode_B_attack(BossSst* this, PlayState* play) {
    f32 chargeDist;
    s32 animFinish = Skeleton_Info2_anime_play(&this->skelAnime);

    if (!this->ready && Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
        this->ready = true;
        this->actor.speed = 0.25f;
        this->skelAnime.playSpeed = 0.2f;
    }

    this->actor.speed *= 1.25f;
    this->actor.speed = CLAMP_MAX(this->actor.speed, 45.0f);

    if (this->ready) {
        if (add_calc(&this->radius, 650.0f, 0.4f, this->actor.speed, 1.0f) < 10.0f) {
            this->radius = 650.0f;
            mode_B_attack_after_init(this);
        } else {
            chargeDist = (650.0f - this->radius) * 3.0f;
            if (chargeDist > 180.0f) {
                chargeDist = 180.0f;
            }

            this->actor.world.pos.y = this->actor.home.pos.y - chargeDist;
        }

        if (!animFinish) {
            hand_pos[LEFT].z += 5.0f;
            hand_pos[RIGHT].z += 5.0f;
        }
    } else {
        add_calc2(&this->radius, -700.0f, 0.4f, this->actor.speed);
        chase_f(&this->actor.world.pos.y, this->actor.home.pos.y - 180.0f, 20.0f);
        hand_pos[LEFT].y += 5.0f;
        hand_pos[RIGHT].y += 5.0f;
    }

    if (this->colliderJntSph.base.atFlags & AT_HIT) {
        this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        sst_hand[LEFT]->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        sst_hand[RIGHT]->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        Actor_player_power_damage_set(play, &this->actor, 10.0f, this->actor.shape.rot.y, 5.0f);
        player_SE_set(GET_PLAYER(play), NA_SE_PL_BODY_HIT);
    }
}

void mode_B_attack_after_init(BossSst* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gBongoHeadEyeCloseIdleAnim, -20.0f);
    this->targetYaw = Actor_search_position_angleY(&this->actor, &map_center_pos);
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->radius *= -1.0f;
    this->actionFunc = mode_B_attack_after;
}

void mode_B_attack_after(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (add_calc_short_angle2(&this->actor.shape.rot.y, this->targetYaw, 4, 0x800, 0x100) == 0) {
        mode_H_return_init(sst_hand[LEFT]);
        mode_H_return_init(sst_hand[RIGHT]);
        mode_B_move_init(this);
    }
}

void mode_B_h_freeze_wait_init(BossSst* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    this->ready = false;
    this->colliderCyl.base.acFlags |= AC_ON;
    this->actionFunc = mode_B_h_freeze_wait;
}

void mode_B_h_freeze_wait(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->ready) {
        mode_B_close_eye_init(this);
    }
}

void mode_B_close_eye_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gBongoHeadEyeCloseAnim, -5.0f);
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_B_close_eye;
}

void mode_B_close_eye(BossSst* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_B_wait_init(this);
    }
}

void mode_B_arrow_hit_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gBongoHeadKnockoutAnim, -5.0f);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                         Si2_anime_end_frame(&gBongoHeadKnockoutAnim));
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->vVanish = false;
    this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
    sst_body_SE_set(this, NA_SE_EN_SHADEST_FREEZE);
    this->actionFunc = mode_B_arrow_hit;
}

void mode_B_arrow_hit(BossSst* this, PlayState* play) {
    f32 currentFrame;
    s32 animFinish;

    chase_f(&hand_pos[LEFT].z, 600.0f, 20.0f);
    chase_f(&hand_pos[RIGHT].z, 600.0f, 20.0f);
    chase_f(&hand_pos[LEFT].x, 200.0f, 20.0f);
    chase_f(&hand_pos[RIGHT].x, -200.0f, 20.0f);
    this->actor.velocity.y += this->actor.gravity;
    animFinish = Skeleton_Info2_anime_play(&this->skelAnime);
    currentFrame = this->skelAnime.curFrame;
    if (currentFrame <= 6.0f) {
        f32 bounce = (sinf((M_PI / 11) * currentFrame) * 100.0f) + (this->actor.home.pos.y - 180.0f);

        if (this->actor.world.pos.y < bounce) {
            this->actor.world.pos.y = bounce;
        }
    } else if (currentFrame <= 11.0f) {
        this->actor.world.pos.y = (sinf((M_PI / 11) * currentFrame) * 170.0f) + (this->actor.home.pos.y - 250.0f);
    } else {
        this->actor.world.pos.y =
            (sinf((currentFrame - 11.0f) * (M_PI / 5)) * 50.0f) + (this->actor.home.pos.y - 250.0f);
    }

    if ((animFinish) || Skeleton_Info_frame_check(&this->skelAnime, 11.0f)) {
        sst_body_SE_set(this, NA_SE_EN_SHADEST_LAND);
    }

    if (this->radius < -500.0f) {
        add_calc(&this->radius, -500.0f, 1.0f, 50.0f, 5.0f);
    } else {
        add_calc(&this->actor.speed, 0.0f, 0.5f, 15.0f, 3.0f);
        this->radius += this->actor.speed;
    }

    this->radius = CLAMP_MAX(this->radius, 400.0f);

    this->actor.world.pos.y += this->actor.velocity.y;
    if (animFinish) {
        mode_B_damage_wait_init(this);
    }
}

void mode_B_damage_wait_init(BossSst* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gBongoHeadStunnedAnim, -5.0f);
    this->colliderCyl.base.acFlags |= AC_ON;
    this->colliderCyl.elem.acDmgInfo.dmgFlags = DMG_SWORD | DMG_DEKU_STICK;
    this->actor.speed = 0.0f;
    this->colliderJntSph.elements[10].base.acElemFlags |= (ACELEM_ON | ACELEM_HOOKABLE);
    this->colliderJntSph.elements[0].base.acElemFlags &= ~ACELEM_ON;
    if (this->actionFunc != mode_B_sword_hit) {
        this->timer = 50;
    }

    this->actionFunc = mode_B_damage_wait;
}

void mode_B_damage_wait(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_f(&hand_pos[LEFT].z, 600.0f, 20.0f);
    chase_f(&hand_pos[RIGHT].z, 600.0f, 20.0f);
    chase_f(&hand_pos[LEFT].x, 200.0f, 20.0f);
    chase_f(&hand_pos[RIGHT].x, -200.0f, 20.0f);
    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        this->timer += 2;
        this->timer = CLAMP_MAX(this->timer, 50);
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            mode_H_b_return_init(sst_hand[LEFT]);
            mode_H_b_return_init(sst_hand[RIGHT]);
            mode_B_return_init(this);
        }
    }
}

void mode_B_sword_hit_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gBongoHeadDamageAnim, -3.0f);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA,
                         Si2_anime_end_frame(&gBongoHeadDamageAnim));
    Set_Fog(&sst_hand[LEFT]->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA,
                         Si2_anime_end_frame(&gBongoHeadDamageAnim));
    Set_Fog(&sst_hand[RIGHT]->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA,
                         Si2_anime_end_frame(&gBongoHeadDamageAnim));
    this->colliderCyl.base.acFlags &= ~AC_ON;
    sst_body_SE_set(this, NA_SE_EN_SHADEST_DAMAGE);
    this->actionFunc = mode_B_sword_hit;
}

void mode_B_sword_hit(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_B_damage_wait_init(this);
    }
}

void mode_B_return_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gBongoHeadRecoverAnim, -5.0f);
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->colliderCyl.elem.acDmgInfo.dmgFlags = DMG_DEFAULT;
    this->colliderJntSph.elements[10].base.acElemFlags &= ~(ACELEM_ON | ACELEM_HOOKABLE);
    this->colliderJntSph.elements[0].base.acElemFlags |= ACELEM_ON;
    this->vVanish = true;
    this->actor.speed = 5.0f;
    this->actionFunc = mode_B_return;
}

void mode_B_return(BossSst* this, PlayState* play) {
    s32 animFinish;
    f32 currentFrame;
    f32 diff;

    animFinish = Skeleton_Info2_anime_play(&this->skelAnime);
    currentFrame = this->skelAnime.curFrame;
    if (currentFrame < 10.0f) {
        this->actor.world.pos.y += 10.0f;
        hand_pos[LEFT].y -= 10.0f;
        hand_pos[RIGHT].y -= 10.0f;
        add_calc(&this->radius, -750.0f, 1.0f, this->actor.speed, 2.0f);
    } else {
        this->actor.speed *= 1.25f;
        this->actor.speed = CLAMP_MAX(this->actor.speed, 50.0f);
        diff = add_calc(&this->radius, -650.0f, 1.0f, this->actor.speed, 2.0f);
        diff += add_calc(&this->actor.world.pos.y, this->actor.home.pos.y, 0.5f, 30.0f, 3.0f);
    }
    if (animFinish && (diff < 10.0f)) {
        this->actor.world.pos.y = this->actor.home.pos.y;
        this->radius = -650.0f;
        mode_H_return_init(sst_hand[LEFT]);
        mode_H_return_init(sst_hand[RIGHT]);
        mode_B_move_init(this);
    }
}

void set_end_demo_inc(f32 subCamVelFactor, s32 targetIndex) {
    Vec3f* subCamAtNext = &end_demo_center[targetIndex];
    Vec3f* subCamEyeNext = &end_demo_eye[targetIndex];

    if (targetIndex != 0) {
        xyz_t_move(&center_pos, &end_demo_center[targetIndex - 1]);
        xyz_t_move(&eye_pos, &end_demo_eye[targetIndex - 1]);
    }

    inc_center.x = (subCamAtNext->x - center_pos.x) * subCamVelFactor;
    inc_center.y = (subCamAtNext->y - center_pos.y) * subCamVelFactor;
    inc_center.z = (subCamAtNext->z - center_pos.z) * subCamVelFactor;

    inc_eye.x = (subCamEyeNext->x - eye_pos.x) * subCamVelFactor;
    inc_eye.y = (subCamEyeNext->y - eye_pos.y) * subCamVelFactor;
    inc_eye.z = (subCamEyeNext->z - eye_pos.z) * subCamVelFactor;
}

void set_end_demo_camera_pos(BossSst* this, PlayState* play) {
    Vec3f subCamAt;
    Vec3f subCamEye;
    f32 sn;
    f32 cs;

    center_pos.x += inc_center.x;
    center_pos.y += inc_center.y;
    center_pos.z += inc_center.z;
    eye_pos.x += inc_eye.x;
    eye_pos.y += inc_eye.y;
    eye_pos.z += inc_eye.z;

    sn = sin_s(this->actor.shape.rot.y);
    cs = cos_s(this->actor.shape.rot.y);
    subCamAt.x = this->actor.world.pos.x + (center_pos.z * sn) + (center_pos.x * cs);
    subCamAt.y = this->actor.home.pos.y - 140.0f + center_pos.y;
    subCamAt.z = this->actor.world.pos.z + (center_pos.z * cs) - (center_pos.x * sn);
    subCamEye.x = this->actor.world.pos.x + (eye_pos.z * sn) + (eye_pos.x * cs);
    subCamEye.y = this->actor.home.pos.y - 140.0f + eye_pos.y;
    subCamEye.z = this->actor.world.pos.z + (eye_pos.z * cs) - (eye_pos.x * sn);
    Gama_play_camera_setting(play, demo_camera, &subCamAt, &subCamEye);
}

void mode_B_final_damage_init(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    sst_body_SE_set(this, NA_SE_EN_SHADEST_DEAD);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 60);
    Set_Fog(&sst_hand[LEFT]->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 60);
    Set_Fog(&sst_hand[RIGHT]->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 60);
    this->timer = 60;
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
    sst_hand[LEFT]->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
    sst_hand[RIGHT]->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
    demo_camera = Gama_play_make_camera(play);
    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
    Gama_play_set_camera_status(play, demo_camera, CAM_STAT_ACTIVE);
    Gama_play_copy_camera_position(play, demo_camera, CAM_ID_MAIN);
    player_demo_mode_set(play, &player->actor, PLAYER_CSACTION_8);
    Demo_play_start(play, &play->csCtx);
    xyz_t_move(&eye_pos, &GET_ACTIVE_CAM(play)->eye);
    this->actionFunc = mode_B_final_damage;
}

void mode_B_final_damage(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    chase_f(&this->actor.world.pos.y, this->actor.home.pos.y - 140.0f, 20.0f);
    if (this->timer == 0) {
        mode_H_dead_move_init(sst_hand[LEFT]);
        mode_H_dead_move_init(sst_hand[RIGHT]);
        mode_B_dead_move_init(this);
    } else if (this->timer > 48) {
        Gama_play_camera_setting(play, demo_camera, &this->actor.focus.pos, &eye_pos);
        chase_f(&this->radius, -350.0f, 10.0f);
    } else if (this->timer == 48) {
        Player* player = GET_PLAYER(play);

        player->actor.world.pos.x = map_center_pos.x + (400.0f * sin_s(this->actor.shape.rot.y)) +
                                    (cos_s(this->actor.shape.rot.y) * -120.0f);
        player->actor.world.pos.z = map_center_pos.z + (400.0f * cos_s(this->actor.shape.rot.y)) -
                                    (sin_s(this->actor.shape.rot.y) * -120.0f);
        player->actor.shape.rot.y = Actor_search_position_angleY(&player->actor, &map_center_pos);
        Actor_search_position_project_distanceXZ(&this->actor, &eye_pos, &GET_ACTIVE_CAM(play)->eye);
        Actor_search_position_project_distanceXZ(&this->actor, &center_pos, &GET_ACTIVE_CAM(play)->at);
        this->radius = -350.0f;
        this->actor.world.pos.x = map_center_pos.x - (sin_s(this->actor.shape.rot.y) * 350.0f);
        this->actor.world.pos.z = map_center_pos.z - (cos_s(this->actor.shape.rot.y) * 350.0f);
        set_end_demo_inc(1.0 / 48, 0);
        set_end_demo_camera_pos(this, play);
    } else {
        set_end_demo_camera_pos(this, play);
    }
}

void mode_B_dead_move_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gBongoHeadEyeOpenIdleAnim, -5.0f);
    this->timer = 160;
    this->targetYaw = this->actor.shape.rot.y;
    set_end_demo_inc(1.0 / 80, 1);
    this->actionFunc = mode_B_dead_move;
}

void mode_B_dead_move(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    if ((this->timer == 0) && (this->actor.shape.rot.y == this->targetYaw)) {
        mode_B_dead_stop_init(this);
    } else if (this->timer >= 80) {
        set_end_demo_camera_pos(this, play);
    }
}

void mode_B_dead_stop_init(BossSst* this) {
    this->timer = 160;
    set_end_demo_inc(1.0 / 80, 2);
    this->actionFunc = mode_B_dead_stop;
}

void mode_B_dead_stop(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (1) {}

    if (this->timer >= 80) {
        if (this->timer == 80) {
            tex_change_flg = true;
        }
        set_end_demo_camera_pos(this, play);
        dead_prim_color.r = dead_prim_color.g = dead_prim_color.b = (this->timer * 3) - 240;
        if (this->timer == 80) {
            set_end_demo_inc(1.0 / 80, 3);
        }
    } else {
        dead_prim_color.b = (80 - this->timer) / 1.0f;
        dead_prim_color.r = dead_prim_color.g = dead_env_color.r = dead_env_color.g = dead_env_color.b = (80 - this->timer) / 8.0f;
        set_end_demo_camera_pos(this, play);
        if (this->timer == 0) {
            mode_B_dead_drop_init(this);
        }
    }
}

void mode_B_dead_drop_init(BossSst* this) {
    this->actor.speed = 1.0f;
    xyz_t_move(&center_pos, &end_demo_center[3]);
    xyz_t_move(&eye_pos, &end_demo_eye[3]);
    inc_center.x = 0.0f;
    inc_center.z = 0.0f;
    inc_center.y = -50.0f;
    xyz_t_move(&inc_eye, &zero_vec);
    this->actionFunc = mode_B_dead_drop;
}

void mode_B_dead_drop(BossSst* this, PlayState* play) {
    this->actor.speed *= 1.5f;
    if (chase_f(&this->actor.world.pos.y, this->actor.home.pos.y - 230.0f, this->actor.speed)) {
        mode_B_dead_scale_init(this);
    }

    if (center_pos.y > 200.0f) {
        set_end_demo_camera_pos(this, play);
    }
}

void mode_B_dead_scale_init(BossSst* this) {
    sst_set_eff_body_dead_shadow(this);
    this->timer = 80;
    set_end_demo_inc(1.0 / 60, 5);
    this->actionFunc = mode_B_dead_scale;
}

void mode_B_dead_scale(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.scale.y -= 0.00025f;
    this->actor.scale.x += 0.000075f;
    this->actor.scale.z += 0.000075f;
    this->actor.world.pos.y = this->actor.home.pos.y - 11500.0f * this->actor.scale.y;
    if (this->timer == 0) {
        mode_B_dead_shadow_init(this);
    } else if (this->timer >= 20.0f) {
        set_end_demo_camera_pos(this, play);
    }
}

void mode_B_dead_shadow_init(BossSst* this) {
    this->actor.draw = Boss_Sst_eff_draw;
    this->timer = 40;
    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS_CLEAR);
    set_end_demo_inc(1.0 / 40, 6);
    this->actionFunc = mode_B_dead_shadow;
}

void mode_B_dead_shadow(BossSst* this, PlayState* play) {
    static Color_RGBA8 prim = { 80, 80, 150, 255 };
    static Color_RGBA8 env = { 40, 40, 80, 255 };
    static Color_RGBA8 blast_prim[2] = {
        { 0, 0, 0, 255 },
        { 100, 100, 100, 0 },
    };
    Vec3f spawnPos;
    s32 i;

    this->timer--;
    if (this->effectMode == BONGO_NULL) {
        if (this->timer < -170) {
            set_end_demo_camera_pos(this, play);
            Gama_play_copy_camera_position(play, CAM_ID_MAIN, demo_camera);
            Gama_play_set_camera_status(play, demo_camera, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_ACTIVE);
            Gama_play_clear_camera(play, demo_camera);
            player_demo_mode_set(play, &GET_PLAYER(play)->actor, PLAYER_CSACTION_7);
            Demo_play_end(play, &play->csCtx);
            Actor_delete(&this->actor);
            Actor_delete(&sst_hand[LEFT]->actor);
            Actor_delete(&sst_hand[RIGHT]->actor);
            Actor_Environment_room_clear_On(play, play->roomCtx.curRoom.num);
        }
    } else if (this->effects[0].alpha == 0) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_WARP1, ROOM_CENTER_X, ROOM_CENTER_Y, ROOM_CENTER_Z, 0, 0, 0,
                    WARP_DUNGEON_ADULT);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART,
                    (sin_s(this->actor.shape.rot.y) * 200.0f) + ROOM_CENTER_X, ROOM_CENTER_Y,
                    cos_s(this->actor.shape.rot.y) * 200.0f + ROOM_CENTER_Z, 0, 0, 0, 0);
        set_end_demo_inc(1.0f, 7);
        this->effectMode = BONGO_NULL;
    } else if (this->timer == 0) {
        this->effects[0].status = 0;
        this->effects[1].status = -1;
        this->effects[2].status = -1;
    } else if (this->timer > 0) {
        this->effects[0].status += 5;
        set_end_demo_camera_pos(this, play);
    }

    prim.a = this->effects[0].alpha;
    env.a = this->effects[0].alpha;

    for (i = 0; i < 5; i++) {
        spawnPos.x = map_center_pos.x + 0.0f + rnd_fx(800.0f);
        spawnPos.y = map_center_pos.y + (-28.0f) + (fqrand() * 5.0f);
        spawnPos.z = map_center_pos.z + 0.0f + rnd_fx(800.0f);
        Effect_SS_G_Splash_sc_cl_ct(play, &spawnPos, &prim, &env, 0, 0x3E8);
    }
}

void mode_H_wait_init(BossSst* this) {
    HAND_STATE(this) = HAND_WAIT;
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, sst_hand_pose_a[this->actor.params], 5.0f);
    this->ready = false;
    this->timer = 20;
    this->actionFunc = mode_H_wait;
}

void mode_H_wait(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_f(&this->actor.world.pos.y, this->actor.floorHeight, 20.0f);
    chase_f(&this->actor.world.pos.x, this->actor.home.pos.x, 1.0f);
    chase_f(&this->actor.world.pos.z, this->actor.home.pos.z, 1.0f);
    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        Player* player = GET_PLAYER(play);

        if (this->timer != 0) {
            this->timer--;
        }

        if ((this->timer == 0) && (player->actor.world.pos.y > -50.0f) &&
            !(player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_13 | PLAYER_STATE1_14))) {
            sst_hand_change_attack(this);
        }
    } else if (sst_body->actionFunc == mode_B_move) {
        if ((this->actor.params == BONGO_RIGHT_HAND) && ((sst_body->timer % 28) == 12)) {
            mode_H_r_drum_move_init(this);
        } else if ((this->actor.params == BONGO_LEFT_HAND) && ((sst_body->timer % 7) == 5) && (sst_body->timer < 112)) {
            mode_H_l_drum_move_init(this);
        }
    }
}

void mode_H_r_drum_move_init(BossSst* this) {
    HAND_STATE(this) = HAND_BEAT;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_c[this->actor.params], 5.0f);
    this->actor.shape.rot.x = 0;
    this->timer = 12;
    this->actionFunc = mode_H_r_drum_move;
}

void mode_H_r_drum_move(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        mode_H_wait_init(this);
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer >= 3) {
            this->actor.shape.rot.x -= 0x100;
            chase_f(&this->actor.world.pos.y, ROOM_CENTER_Y + 180.0f, 20.0f);
        } else {
            this->actor.shape.rot.x += 0x300;
            chase_f(&this->actor.world.pos.y, ROOM_CENTER_Y + 0.0f, 60.0f);
        }

        if (this->timer == 0) {
            bg_floor->dyna.actor.params = BONGOFLOOR_HIT;
            if (sst_body->actionFunc == mode_B_wait) {
                if (this->ready) {
                    sst_hand_change_attack(this);
                } else {
                    mode_H_wait_init(this);
                }
            } else {
                mode_H_r_drum_wait_init(this);
            }
            z_vibctl2_vib_setQ(this->actor.xyzDistToPlayerSq, 255, 20, 150);
            Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
        }
    }
}

void mode_H_r_drum_wait_init(BossSst* this) {
    bg_floor->dyna.actor.params = BONGOFLOOR_HIT;
    Skeleton_Info2_init_standard_stop(&this->skelAnime, sst_hand_pose_b[this->actor.params]);
    this->actionFunc = mode_H_r_drum_wait;
}

void mode_H_r_drum_wait(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        mode_H_wait_init(this);
    } else {
        add_calc(&this->actor.world.pos.y, ROOM_CENTER_Y + 40.0f, 0.5f, 20.0f, 3.0f);
        chase_angle(&this->actor.shape.rot.x, -0x800, 0x100);
        chase_f(&this->actor.world.pos.x, this->actor.home.pos.x, 1.0f);
        chase_f(&this->actor.world.pos.z, this->actor.home.pos.z, 1.0f);
        if ((sst_body->actionFunc != mode_B_start_demo) && ((sst_body->timer % 28) == 12)) {
            mode_H_r_drum_move_init(this);
        }
    }
}

void mode_H_l_drum_move_init(BossSst* this) {
    HAND_STATE(this) = HAND_BEAT;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_c[this->actor.params], 5.0f);
    this->actor.shape.rot.x = 0;
    this->timer = 5;
    this->actionFunc = mode_H_l_drum_move;
}

void mode_H_l_drum_move(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        mode_H_wait_init(this);
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer != 0) {
            this->actor.shape.rot.x -= 0x140;
            chase_f(&this->actor.world.pos.y, ROOM_CENTER_Y + 60.0f, 15.0f);
        } else {
            this->actor.shape.rot.x += 0x500;
            chase_f(&this->actor.world.pos.y, ROOM_CENTER_Y + 0.0f, 60.0f);
        }

        if (this->timer == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_TAIKO_LOW);
            mode_H_l_drum_wait_init(this);
        }
    }
}

void mode_H_l_drum_wait_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, sst_hand_pose_b[this->actor.params]);
    this->actionFunc = mode_H_l_drum_wait;
}

void mode_H_l_drum_wait(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        mode_H_wait_init(this);
    } else {
        add_calc(&this->actor.world.pos.y, ROOM_CENTER_Y + 40.0f, 0.5f, 20.0f, 3.0f);
        chase_angle(&this->actor.shape.rot.x, -0x400, 0xA0);
        chase_f(&this->actor.world.pos.x, this->actor.home.pos.x, 1.0f);
        chase_f(&this->actor.world.pos.z, this->actor.home.pos.z, 1.0f);
        if (sst_body->actionFunc == mode_B_wait) {
            if (this->ready) {
                sst_hand_change_attack(this);
            } else {
                mode_H_wait_init(this);
            }
        } else if ((sst_body->actionFunc != mode_B_start_demo) && ((sst_body->timer % 7) == 5) &&
                   ((sst_body->timer % 28) != 5)) {
            mode_H_l_drum_move_init(this);
        }
    }
}

void mode_H_return_ready_init(BossSst* this) {
    HAND_STATE(this) = HAND_RETREAT;
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_g[this->actor.params], 6.0f);
    this->actionFunc = mode_H_return_ready;
}

void mode_H_return_ready(BossSst* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_H_return_init(this);
    }
}

void mode_H_return_init(BossSst* this) {
    HAND_STATE(this) = HAND_RETREAT;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_h[this->actor.params], 10.0f);
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    this->colliderJntSph.base.acFlags |= AC_ON;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    sst_set_ac_shield(this, false);
    this->timer = 0;
    this->actionFunc = mode_H_return;
    this->actor.speed = 3.0f;
}

void mode_H_return(BossSst* this, PlayState* play) {
    f32 diff;
    s32 inPosition;

    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actor.speed *= 1.2f;
    this->actor.speed = CLAMP_MAX(this->actor.speed, 50.0f);

    diff = add_calc(&this->actor.world.pos.x, this->actor.home.pos.x, 0.3f, this->actor.speed, 1.0f);
    diff += add_calc(&this->actor.world.pos.z, this->actor.home.pos.z, 0.3f, this->actor.speed, 1.0f);
    if (this->timer != 0) {
        if (this->timer != 0) {
            this->timer--;
        }

        this->actor.world.pos.y = (sinf((this->timer * M_PI) / 16.0f) * 250.0f) + this->actor.home.pos.y;
        if (this->timer == 0) {
            mode_H_wait_init(this);
        } else if (this->timer == 4) {
            Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, sst_hand_pose_a[this->actor.params], 4.0f);
        }
    } else {
        inPosition = chase_angle(&this->actor.shape.rot.y, this->actor.home.rot.y, 0x200);
        inPosition &= chase_angle(&this->actor.shape.rot.z, this->actor.home.rot.z, 0x200);
        inPosition &= chase_angle(&this->handYRotMod, 0, 0x800);
        Actor_level_SE_set(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
        if ((add_calc(&this->actor.world.pos.y, ROOM_CENTER_Y + 250.0f, 0.5f, 70.0f, 5.0f) < 1.0f) &&
            inPosition && (diff < 10.0f)) {
            this->timer = 8;
        }
    }
}

void mode_H_press_ready_init(BossSst* this) {
    HAND_STATE(this) = HAND_SLAM;
    this->timer = 0;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_c[this->actor.params], 10.0f);
    this->actionFunc = mode_H_press_ready;
}

void mode_H_press_ready(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            mode_H_press_init(this);
        }
    } else {
        Player* player = GET_PLAYER(play);

        if (chase_f(&this->actor.world.pos.y, ROOM_CENTER_Y + 300.0f, 30.0f) &&
            (this->actor.xzDistToPlayer < 140.0f)) {
            this->timer = 20;
        }
        chase_angle(&this->actor.shape.rot.x, -0x1000, 0x100);
        add_calc2(&this->actor.world.pos.x, player->actor.world.pos.x, 0.5f, 40.0f);
        add_calc2(&this->actor.world.pos.z, player->actor.world.pos.z, 0.5f, 40.0f);
        Actor_level_SE_set(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }
}

void mode_H_press_init(BossSst* this) {
    HAND_STATE(this) = HAND_SLAM;
    this->actor.velocity.y = 1.0f;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_b[this->actor.params], 10.0f);
    sst_hand_set_at_check(this, 0x20);
    this->ready = false;
    Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_FLY_ATTACK);
    this->actionFunc = mode_H_press;
}

void mode_H_press(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_s(&this->handZPosMod, -0xDAC, 0x1F4);
    chase_angle(&this->actor.shape.rot.x, 0, 0x1000);
    chase_angle(&this->handYRotMod, 0, 0x1000);
    if (this->timer != 0) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            if (this->colliderJntSph.base.acFlags & AC_ON) {
                mode_H_return_ready_init(this);
            } else {
                this->colliderJntSph.base.acFlags |= AC_ON;
                mode_H_wait_init(this);
            }
        }
    } else {
        if (this->ready) {
            this->timer = 30;
            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        } else {
            this->actor.velocity.y *= 1.5f;
            if (chase_f(&this->actor.world.pos.y, this->actor.floorHeight, this->actor.velocity.y)) {
                this->ready = true;
                Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_TAIKO_LOW);
                sst_set_eff_blast(this);
                this->colliderCyl.base.atFlags |= AT_ON;
                CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colliderCyl);
                this->colliderCyl.dim.radius = SstBlastAtPipeData.dim.radius;
            }
        }

        if (this->colliderJntSph.base.atFlags & AT_HIT) {
            Player* player = GET_PLAYER(play);

            player->actor.world.pos.x = (sin_s(this->actor.yawTowardsPlayer) * 100.0f) + this->actor.world.pos.x;
            player->actor.world.pos.z = (cos_s(this->actor.yawTowardsPlayer) * 100.0f) + this->actor.world.pos.z;

            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            Actor_player_power_damage_set(play, &this->actor, 5.0f, this->actor.yawTowardsPlayer, 0.0f);
        }

        chase_angle(&this->actor.shape.rot.x, 0, 0x200);
    }
}

void mode_H_chop_ready_init(BossSst* this) {
    HAND_STATE(this) = HAND_SWEEP;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_c[this->actor.params], 10.0f);
    this->radius = Actor_search_position_distanceXZ(&this->actor, &sst_body->actor.world.pos);
    this->actor.world.rot.y = Actor_search_position_angleY(&sst_body->actor, &this->actor.world.pos);
    this->targetYaw = this->actor.home.rot.y + (this->vParity * 0x2000);
    this->actionFunc = mode_H_chop_ready;
}

void mode_H_chop_ready(BossSst* this, PlayState* play) {
    s32 inPosition;

    Skeleton_Info2_anime_play(&this->skelAnime);
    inPosition = chase_f(&this->actor.world.pos.y, ROOM_CENTER_Y + 50.0f, 4.0f);
    inPosition &= chase_angle(&this->actor.shape.rot.y, this->targetYaw, 0x200);
    inPosition &= chase_angle(&this->actor.world.rot.y, this->targetYaw, 0x400);
    inPosition &= (add_calc(&this->radius, sst_body->actor.xzDistToPlayer, 0.5f, 60.0f, 1.0f) < 10.0f);

    this->actor.world.pos.x = (sin_s(this->actor.world.rot.y) * this->radius) + sst_body->actor.world.pos.x;
    this->actor.world.pos.z = (cos_s(this->actor.world.rot.y) * this->radius) + sst_body->actor.world.pos.z;
    if (inPosition) {
        mode_H_chop_init(this);
    } else {
        Actor_level_SE_set(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }
}

void mode_H_chop_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_b[this->actor.params], 5.0f);
    sst_hand_set_at_check(this, 0x10);
    this->targetYaw = this->actor.home.rot.y - (this->vParity * 0x2000);
    this->handMaxSpeed = 0x300;
    this->handAngSpeed = 0;
    this->ready = false;
    Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_FLY_ATTACK);
    this->actionFunc = mode_H_chop;
}

void mode_H_chop(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);
    this->handAngSpeed += 0x60;
    this->handAngSpeed = CLAMP_MAX(this->handAngSpeed, this->handMaxSpeed);

    if (!add_calc_short_angle2(&this->actor.shape.rot.y, this->targetYaw, 4, this->handAngSpeed, 0x10)) {
        this->colliderJntSph.base.ocFlags1 &= ~OC1_NO_PUSH;
        mode_H_return_init(this);
    } else if (this->colliderJntSph.base.atFlags & AT_HIT) {
        s16 newTargetYaw;

        this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        this->ready = true;
        Actor_player_power_damage_set(play, &this->actor, 5.0f,
                                              this->actor.shape.rot.y - (this->vParity * 0x3800), 0.0f);
        player_SE_set(player, NA_SE_PL_BODY_HIT);
        newTargetYaw = this->actor.shape.rot.y - (this->vParity * 0x1400);
        if (((s16)(newTargetYaw - this->targetYaw) * this->vParity) > 0) {
            this->targetYaw = newTargetYaw;
        }
    }

    if (!this->ready && ((player->cylinder.dim.height > 40.0f) || (player->actor.world.pos.y > 1.0f))) {
        this->colliderJntSph.base.atFlags |= AT_ON;
        this->colliderJntSph.base.ocFlags1 &= ~OC1_NO_PUSH;
    } else {
        this->colliderJntSph.base.atFlags &= ~AT_ON;
        this->colliderJntSph.base.ocFlags1 |= OC1_NO_PUSH;
    }

    this->actor.world.pos.x = (sin_s(this->actor.shape.rot.y) * this->radius) + sst_body->actor.world.pos.x;
    this->actor.world.pos.z = (cos_s(this->actor.shape.rot.y) * this->radius) + sst_body->actor.world.pos.z;
}

void mode_H_punch_ready_init(BossSst* this) {
    HAND_STATE(this) = HAND_PUNCH;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_g[this->actor.params], 10.0f);
    this->actionFunc = mode_H_punch_ready;
}

void mode_H_punch_ready(BossSst* this, PlayState* play) {
    s32 inPosition = chase_angle(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x400);

    if (Skeleton_Info2_anime_play(&this->skelAnime) && inPosition) {
        mode_H_punch_init(this);
    }
}

void mode_H_punch_init(BossSst* this) {
    this->actor.speed = 0.5f;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_d[this->actor.params], 5.0f);
    sst_set_ac_shield(this, true);
    this->targetRoll = this->vParity * 0x3F00;
    sst_hand_set_at_check(this, 0x10);
    this->actionFunc = mode_H_punch;
}

void mode_H_punch(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_f(&this->actor.world.pos.y, ROOM_CENTER_Y + 80.0f, 20.0f);
    if (chase_angle(&this->actor.shape.rot.z, this->targetRoll, 0x400)) {
        this->targetRoll *= -1;
    }

    this->actor.speed *= 1.25f;
    this->actor.speed = CLAMP_MAX(this->actor.speed, 50.0f);

    this->actor.world.pos.x += this->actor.speed * sin_s(this->actor.shape.rot.y);
    this->actor.world.pos.z += this->actor.speed * cos_s(this->actor.shape.rot.y);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        mode_H_return_init(this);
    } else if (this->colliderJntSph.base.atFlags & AT_HIT) {
        player_SE_set(GET_PLAYER(play), NA_SE_PL_BODY_HIT);
        Actor_player_power_damage_set(play, &this->actor, 10.0f, this->actor.shape.rot.y, 5.0f);
        mode_H_return_init(this);
    }

    Actor_level_SE_set(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void mode_H_crush_ready_init(BossSst* this) {
    HAND_STATE(this) = HAND_CLAP;
    if (HAND_STATE(OTHER_HAND(this)) != HAND_CLAP) {
        mode_H_crush_ready_init(OTHER_HAND(this));
    }

    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_c[this->actor.params], 10.0f);
    this->radius = Actor_search_position_distanceXZ(&this->actor, &sst_body->actor.world.pos);
    this->actor.world.rot.y = Actor_search_position_angleY(&sst_body->actor, &this->actor.world.pos);
    this->targetYaw = this->actor.home.rot.y - (this->vParity * 0x1800);
    this->targetRoll = this->vParity * 0x4000;
    this->timer = 0;
    this->ready = false;
    OTHER_HAND(this)->ready = false;
    this->actionFunc = mode_H_crush_ready;
}

void mode_H_crush_ready(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            mode_H_crush_init(this);
            mode_H_crush_init(OTHER_HAND(this));
            OTHER_HAND(this)->radius = this->radius;
        }
    } else if (!this->ready) {
        this->ready = Skeleton_Info2_anime_play(&this->skelAnime);
        this->ready &= chase_angle(&this->actor.shape.rot.x, 0, 0x600);
        this->ready &= chase_angle(&this->actor.shape.rot.z, this->targetRoll, 0x600);
        this->ready &= chase_angle(&this->actor.shape.rot.y, this->targetYaw, 0x200);
        this->ready &= chase_angle(&this->actor.world.rot.y, this->targetYaw, 0x400);
        this->ready &= add_calc(&this->radius, sst_body->actor.xzDistToPlayer, 0.5f, 50.0f, 1.0f) < 10.0f;
        this->ready &= add_calc(&this->actor.world.pos.y, ROOM_CENTER_Y + 95.0f, 0.5f, 30.0f, 1.0f) < 1.0f;

        this->actor.world.pos.x = sin_s(this->actor.world.rot.y) * this->radius + sst_body->actor.world.pos.x;
        this->actor.world.pos.z = cos_s(this->actor.world.rot.y) * this->radius + sst_body->actor.world.pos.z;
    } else if (OTHER_HAND(this)->ready) {
        this->timer = 20;
    }
}

void mode_H_crush_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_b[this->actor.params], 3.0f);
    this->timer = 0;
    this->handMaxSpeed = 0x240;
    this->handAngSpeed = 0;
    this->ready = false;
    sst_hand_set_at_check(this, 0x20);
    this->actionFunc = mode_H_crush;
}

void mode_H_crush(BossSst* this, PlayState* play) {
    static s32 item_flg = false;
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            if (item_flg) {
                Item_set0(play, &this->actor.world.pos,
                                     (fqrand() < 0.5f) ? ITEM00_ARROWS_SMALL : ITEM00_MAGIC_SMALL);
                item_flg = false;
            }

            sst_hand_reset_player_catch(this, play, true);
            mode_H_crush_after_init(this);
        }
    } else {
        if (this->colliderJntSph.base.atFlags & AT_HIT) {
            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            OTHER_HAND(this)->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            sst_hand_set_player_catch(this, play);
        }

        if (this->ready) {
            this->timer = 30;
            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            if (!(player->stateFlags2 & PLAYER_STATE2_7)) {
                item_flg = true;
            }
        } else {
            this->handAngSpeed += 0x40;
            this->handAngSpeed = CLAMP_MAX(this->handAngSpeed, this->handMaxSpeed);

            if (chase_angle(&this->actor.shape.rot.y, this->actor.home.rot.y, this->handAngSpeed)) {
                if (this->actor.params == BONGO_LEFT_HAND) {
                    Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_CLAP);
                }
                this->ready = true;
            } else {
                Actor_level_SE_set(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
            }

            this->actor.world.pos.x = (sin_s(this->actor.shape.rot.y) * this->radius) + sst_body->actor.world.pos.x;
            this->actor.world.pos.z = (cos_s(this->actor.shape.rot.y) * this->radius) + sst_body->actor.world.pos.z;
        }
    }

    if (player->actor.parent == &this->actor) {
        player->av2.actionVar2 = 0;
        player->actor.world.pos = this->actor.world.pos;
    }
}

void mode_H_crush_after_init(BossSst* this) {
    this->targetYaw = this->actor.home.rot.y - (this->vParity * 0x1000);
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_c[this->actor.params], 10.0f);
    this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    this->actionFunc = mode_H_crush_after;
}

void mode_H_crush_after(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_angle(&this->actor.shape.rot.z, 0, 0x200);
    if (chase_angle(&this->actor.shape.rot.y, this->targetYaw, 0x100)) {
        mode_H_return_init(this);
    }
    this->actor.world.pos.x = (sin_s(this->actor.shape.rot.y) * this->radius) + sst_body->actor.world.pos.x;
    this->actor.world.pos.z = (cos_s(this->actor.shape.rot.y) * this->radius) + sst_body->actor.world.pos.z;
}

void mode_H_catch_ready_init(BossSst* this) {
    HAND_STATE(this) = HAND_GRAB;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_c[this->actor.params], 10.0f);
    this->targetYaw = this->vParity * -0x5000;
    this->targetRoll = this->vParity * 0x4000;
    this->actionFunc = mode_H_catch_ready;
}

void mode_H_catch_ready(BossSst* this, PlayState* play) {
    s32 inPosition;

    Skeleton_Info2_anime_play(&this->skelAnime);
    inPosition = add_calc_short_angle2(&this->actor.shape.rot.z, this->targetRoll, 4, 0x800, 0x100) == 0;
    inPosition &= chase_angle(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer + this->targetYaw, 0xA00);
    add_calc2(&this->actor.world.pos.y, ROOM_CENTER_Y + 95.0f, 0.5f, 20.0f);
    if (inPosition) {
        mode_H_catch_init(this);
    }
}

void mode_H_catch_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_d[this->actor.params], 5.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y + (this->vParity * 0x4000);
    this->targetYaw = this->actor.world.rot.y;
    this->timer = 30;
    this->actor.speed = 0.5f;
    sst_hand_set_at_check(this, 0x20);
    this->actionFunc = mode_H_catch;
}

void mode_H_catch(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.world.rot.y =
        ((1.0f - sinf(this->timer * (M_PI / 60.0f))) * (this->vParity * 0x2000)) + this->targetYaw;
    this->actor.shape.rot.y = this->actor.world.rot.y - (this->vParity * 0x4000);
    if (this->timer < 5) {
        add_calc(&this->actor.speed, 0.0f, 0.5f, 25.0f, 5.0f);
        if (Skeleton_Info2_anime_play(&this->skelAnime)) {
            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            this->actor.speed = 0.0f;
            if (player->stateFlags2 & PLAYER_STATE2_7) {
                if (fqrand() < 0.5f) {
                    mode_H_grip_init(this);
                } else {
                    mode_H_shake_throw_init(this);
                }
            } else {
                Item_set0(play, &this->actor.world.pos,
                                     (fqrand() < 0.5f) ? ITEM00_ARROWS_SMALL : ITEM00_MAGIC_SMALL);
                mode_H_return_init(this);
            }
        }
    } else {
        this->actor.speed *= 1.26f;
        this->actor.speed = CLAMP_MAX(this->actor.speed, 70.0f);
        Actor_level_SE_set(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }

    if (this->colliderJntSph.base.atFlags & AT_HIT) {
        this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_CATCH);
        sst_hand_set_player_catch(this, play);
        this->timer = CLAMP_MAX(this->timer, 5);
    }

    this->actor.world.pos.x += this->actor.speed * sin_s(this->actor.world.rot.y);
    this->actor.world.pos.z += this->actor.speed * cos_s(this->actor.world.rot.y);
    if (player->stateFlags2 & PLAYER_STATE2_7) {
        player->av2.actionVar2 = 0;
        player->actor.world.pos = this->actor.world.pos;
        player->actor.shape.rot.y = this->actor.shape.rot.y;
    }
}

void mode_H_grip_init(BossSst* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, sst_hand_pose_e[this->actor.params], -10.0f);
    this->timer = 20;
    this->actionFunc = mode_H_grip;
}

void mode_H_grip(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    if (!(player->stateFlags2 & PLAYER_STATE2_7)) {
        sst_hand_reset_player_catch(this, play, true);
        mode_H_grip_after_init(this);
    } else {
        player->actor.world.pos = this->actor.world.pos;
        if (this->timer == 0) {
            this->timer = 20;
            if (!LINK_IS_ADULT) {
                player_SE_set(player, NA_SE_VO_LI_DAMAGE_S_KID);
            } else {
                player_SE_set(player, NA_SE_VO_LI_DAMAGE_S);
            }

            play->damagePlayer(play, -8);
        }
        if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f)) {
            Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_CATCH);
        }
    }
}

void mode_H_grip_after_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_b[this->actor.params], 10.0f);
    this->actionFunc = mode_H_grip_after;
}

void mode_H_grip_after(BossSst* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_H_return_init(this);
    }
}

void mode_H_shake_throw_init(BossSst* this) {
    this->amplitude = -0x4000;
    this->timer = 1;
    this->center.x = this->actor.world.pos.x - (sin_s(this->actor.shape.rot.y) * 200.0f);
    this->center.y = this->actor.world.pos.y;
    this->center.z = this->actor.world.pos.z - (cos_s(this->actor.shape.rot.y) * 200.0f);
    this->actionFunc = mode_H_shake_throw;
}

void mode_H_shake_throw(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 offXZ;

    if (chase_angle(&this->actor.shape.rot.x, this->amplitude, this->timer * 0xE4 + 0x1C8)) {
        if (this->amplitude != 0) {
            this->amplitude = 0;
            if (this->timer == 4) {
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_b[this->actor.params], 4.0f);
            }
        } else {
            if (this->timer == 4) {
                player->actor.shape.rot.x = 0;
                player->actor.shape.rot.z = 0;
                mode_H_return_init(this);
                return;
            }
            this->amplitude = (this->timer == 3) ? -0x6000 : -0x4000;
            this->timer++;
        }
    }

    this->actor.world.pos.y = (cos_s(this->actor.shape.rot.x + 0x4000) * 200.0f) + this->center.y;
    offXZ = sin_s(this->actor.shape.rot.x + 0x4000) * 200.0f;
    this->actor.world.pos.x = (sin_s(this->actor.shape.rot.y) * offXZ) + this->center.x;
    this->actor.world.pos.z = (cos_s(this->actor.shape.rot.y) * offXZ) + this->center.z;
    if (this->timer != 4) {
        this->actor.shape.rot.z = (this->actor.shape.rot.x + 0x4000) * this->vParity;
    } else {
        chase_angle(&this->actor.shape.rot.z, 0, 0x800);
    }

    if (player->stateFlags2 & PLAYER_STATE2_7) {
        player->av2.actionVar2 = 0;
        xyz_t_move(&player->actor.world.pos, &this->actor.world.pos);
        player->actor.shape.rot.x = this->actor.shape.rot.x;
        player->actor.shape.rot.z = (this->vParity * -0x4000) + this->actor.shape.rot.z;
    } else {
        chase_angle(&player->actor.shape.rot.x, 0, 0x600);
        chase_angle(&player->actor.shape.rot.z, 0, 0x600);
        player->actor.world.pos.x += 20.0f * sin_s(this->actor.shape.rot.y);
        player->actor.world.pos.z += 20.0f * cos_s(this->actor.shape.rot.y);
    }

    if ((this->timer == 4) && (this->amplitude == 0) && Skeleton_Info2_anime_play(&this->skelAnime) &&
        (player->stateFlags2 & PLAYER_STATE2_7)) {
        sst_hand_reset_player_catch(this, play, false);
        player->actor.world.pos.x += 70.0f * sin_s(this->actor.shape.rot.y);
        player->actor.world.pos.z += 70.0f * cos_s(this->actor.shape.rot.y);
        Actor_player_power_damage_set(play, &this->actor, 15.0f, this->actor.shape.rot.y, 2.0f);
        player_SE_set(player, NA_SE_PL_BODY_HIT);
    }

    Actor_level_SE_set(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void mode_H_damage_init(BossSst* this) {
    HAND_STATE(this) = HAND_DAMAGED;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_b[this->actor.params], 4.0f);
    this->timer = 36;
    xyz_t_move(&this->center, &this->actor.world.pos);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 200);
    this->actionFunc = mode_H_damage;
}

void mode_H_damage(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    if (!(this->timer % 4)) {
        if (this->timer % 8) {
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_b[this->actor.params], 4.0f);
        } else {
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_d[this->actor.params], 6.0f);
        }
    }

    this->actor.colorFilterTimer = 200;
    this->actor.world.pos.x += rnd_fx(20.0f);
    this->actor.world.pos.y += rnd_fx(20.0f);
    this->actor.world.pos.z += rnd_fx(20.0f);

    if (this->actor.world.pos.y < (this->actor.floorHeight + 100.0f)) {
        chase_f(&this->actor.world.pos.y, this->actor.floorHeight + 100.0f, 20.0f);
    }

    if (this->timer == 0) {
        mode_H_damage_after_init(this);
    }
}

void mode_H_damage_after_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_f[this->actor.params], 8.0f);
    this->actionFunc = mode_H_damage_after;
}

void mode_H_damage_after(BossSst* this, PlayState* play) {
    f32 diff;
    s32 inPosition;

    diff = add_calc(&this->actor.world.pos.x, this->actor.home.pos.x, 0.5f, 25.0f, 1.0f);
    diff += add_calc(&this->actor.world.pos.z, this->actor.home.pos.z, 0.5f, 25.0f, 1.0f);
    diff += add_calc(&this->actor.world.pos.y, this->actor.home.pos.y + 200.0f, 0.2f, 30.0f, 1.0f);
    inPosition = chase_angle(&this->actor.shape.rot.x, 0x4000, 0x400);
    inPosition &= chase_angle(&this->actor.shape.rot.z, 0, 0x1000);
    inPosition &= chase_angle(&this->actor.shape.rot.y, this->actor.home.rot.y, 0x800);
    inPosition &= chase_s(&this->handZPosMod, -0x5DC, 0x1F4);
    inPosition &= chase_angle(&this->handYRotMod, this->vParity * -0x2000, 0x800);
    this->actor.colorFilterTimer = 200;
    if ((diff < 30.0f) && inPosition) {
        mode_H_swing_init(this);
    } else {
        Actor_level_SE_set(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
    }
}

void mode_H_swing_init(BossSst* this) {
    this->timer = 200;
    this->actionFunc = mode_H_swing;
}

void mode_H_swing(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.shape.rot.x = 0x4000 + (sinf(this->timer * (M_PI / 5)) * 0x2000);
    this->handYRotMod = (this->vParity * -0x2000) + (sinf(this->timer * (M_PI / 4)) * 0x2800);

    if (!(this->timer % 8)) {
        Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_SHAKEHAND);
    }

    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        if ((OTHER_HAND(this)->actionFunc == mode_H_swing) ||
            (OTHER_HAND(this)->actionFunc == mode_H_b_attack)) {
            mode_H_b_attack_init(this);
        } else if (this->timer == 0) {
            this->timer = 80;
        }
    } else if (this->timer == 0) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        mode_H_press_init(this);
    }
}

void mode_H_b_attack_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_d[this->actor.params], 10.0f);
    this->ready = false;
    this->actionFunc = mode_H_b_attack;
}

void mode_H_b_attack(BossSst* this, PlayState* play) {
    if (!this->ready) {
        this->ready = Skeleton_Info2_anime_play(&this->skelAnime);
        this->ready &= chase_angle(&this->actor.shape.rot.x, 0, 0x800);
        this->ready &=
            chase_angle(&this->actor.shape.rot.y, this->actor.home.rot.y + (this->vParity * 0x1000), 0x800);
        this->ready &= chase_angle(&this->handYRotMod, 0, 0x800);
        this->ready &= chase_angle(&this->actor.shape.rot.z, this->vParity * 0x2800, 0x800);
        this->ready &= chase_s(&this->handZPosMod, -0xDAC, 0x1F4);
        if (this->ready) {
            this->actor.colorFilterTimer = 0;
        }
    } else if (this->colliderJntSph.base.atFlags & AT_HIT) {
        this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        OTHER_HAND(this)->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        sst_body->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
        Actor_player_power_damage_set(play, &this->actor, 10.0f, this->actor.shape.rot.y, 5.0f);
        player_SE_set(GET_PLAYER(play), NA_SE_PL_BODY_HIT);
    }
}

void mode_H_b_damage_wait_init(BossSst* hand) {
    Skeleton_Info2_init_standard_stop_morf(&hand->skelAnime, sst_hand_pose_a[hand->actor.params], 10.0f);
    if (hand->actionFunc != mode_H_b_damage) {
        hand->ready = false;
    }

    hand->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
    hand->colliderJntSph.base.acFlags |= AC_ON;
    sst_set_ac_shield(hand, true);
    Set_Fog(&hand->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA,
                         Si2_anime_end_frame(&gBongoHeadKnockoutAnim));
    hand->actionFunc = mode_H_b_damage_wait;
}

void mode_H_b_damage_wait(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc2(&this->actor.world.pos.z, (cos_s(sst_body->actor.shape.rot.y) * 200.0f) + this->actor.home.pos.z,
                   0.5f, 25.0f);
    add_calc2(&this->actor.world.pos.x, (sin_s(sst_body->actor.shape.rot.y) * 200.0f) + this->actor.home.pos.x,
                   0.5f, 25.0f);
    if (!this->ready) {
        chase_angle(&this->handYRotMod, 0, 0x800);
        chase_s(&this->handZPosMod, -0xDAC, 0x1F4);
        chase_angle(&this->actor.shape.rot.x, this->actor.home.rot.x, 0x800);
        chase_angle(&this->actor.shape.rot.z, this->actor.home.rot.z, 0x800);
        chase_angle(&this->actor.shape.rot.y, this->actor.home.rot.y, 0x800);
        if (sst_body->actionFunc == mode_B_damage_wait) {
            this->ready = true;
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_f[this->actor.params], 10.0f);
        }
    } else {
        chase_f(&this->actor.world.pos.y, this->actor.floorHeight, 30.0f);
    }
}

void mode_H_b_damage_init(BossSst* hand) {
    hand->actor.shape.rot.x = 0;
    Skeleton_Info2_init_standard_stop_morf(&hand->skelAnime, sst_hand_pose_c[hand->actor.params], 3.0f);
    hand->timer = 6;
    hand->actionFunc = mode_H_b_damage;
}

void mode_H_b_damage(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer >= 2) {
        this->actor.shape.rot.x -= 0x200;
        chase_f(&this->actor.world.pos.y, this->actor.floorHeight + 200.0f, 50.0f);
    } else {
        this->actor.shape.rot.x += 0x400;
        chase_f(&this->actor.world.pos.y, this->actor.floorHeight, 100.0f);
    }

    if (this->timer == 0) {
        if (this->actor.floorHeight >= 0.0f) {
            Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
        }
        mode_H_b_damage_wait_init(this);
    }
}

void mode_H_dead_move_init(BossSst* this) {
    HAND_STATE(this) = HAND_DEATH;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_c[this->actor.params], 2.0f);
    this->actor.shape.rot.x = 0;
    this->timer = 160;
    if (this->actor.params == BONGO_LEFT_HAND) {
        this->amplitude = -0x800;
    } else {
        this->amplitude = 0;
        this->actor.shape.rot.x = -0x800;
    }

    this->handAngSpeed = 0x180;
    this->actionFunc = mode_H_dead_move;
}

void mode_H_dead_move(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc2(&this->actor.world.pos.z, (cos_s(sst_body->actor.shape.rot.y) * 200.0f) + this->actor.home.pos.z,
                   0.5f, 25.0f);
    add_calc2(&this->actor.world.pos.x, (sin_s(sst_body->actor.shape.rot.y) * 200.0f) + this->actor.home.pos.x,
                   0.5f, 25.0f);
    if (chase_angle(&this->actor.shape.rot.x, this->amplitude, this->handAngSpeed)) {
        if (this->amplitude != 0) {
            this->amplitude = 0;
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_b[this->actor.params], 5.0f);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
            this->amplitude = -0x800;
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_c[this->actor.params], 5.0f);
        }

        if (this->timer < 80.0f) {
            this->handAngSpeed -= 0x40;
            this->handAngSpeed = CLAMP_MIN(this->handAngSpeed, 0x40);
        }
    }

    this->actor.world.pos.y =
        (((this->handAngSpeed / 256.0f) + 0.5f) * 150.0f) * (-1.0f / 0x800) * this->actor.shape.rot.x;
    if (this->timer == 0) {
        mode_H_dead_stop_init(this);
    }
}

void mode_H_dead_stop_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_b[this->actor.params], 5.0f);
    this->actionFunc = mode_H_dead_stop;
}

void mode_H_dead_stop(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_angle(&this->actor.shape.rot.x, -0x800, this->handAngSpeed);
    chase_f(&this->actor.world.pos.y, ROOM_CENTER_Y + 90.0f, 5.0f);
    if (sst_body->actionFunc == mode_B_dead_drop) {
        mode_H_dead_drop_init(this);
    }
}

void mode_H_dead_drop_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_b[this->actor.params], 3.0f);
    this->actionFunc = mode_H_dead_drop;
}

void mode_H_dead_drop(BossSst* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_angle(&this->actor.shape.rot.x, 0, 0x400);
    this->actor.world.pos.y = sst_body->actor.world.pos.y + 230.0f;
    if (sst_body->actionFunc == mode_B_dead_scale) {
        mode_H_dead_scale_init(this);
    }
}

void mode_H_dead_scale_init(BossSst* this) {
    sst_set_eff_hand_dead_shadow(this);
    this->actor.shape.shadowDraw = NULL;
    this->timer = 80;
    this->actionFunc = mode_H_dead_scale;
}

void mode_H_dead_scale(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.scale.y -= 0.00025f;
    this->actor.scale.x += 0.000025f;
    this->actor.scale.z += 0.000025f;
    this->actor.world.pos.y = ROOM_CENTER_Y + 0.0f;
    if (this->timer == 0) {
        mode_H_dead_shadow_init(this);
    }
}

void mode_H_dead_shadow_init(BossSst* this) {
    this->actor.draw = Boss_Sst_eff_draw;
    this->timer = 20;
    this->effects[0].status = 0;
    this->actionFunc = mode_H_dead_shadow;
}

void mode_H_dead_shadow(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        this->effectMode = BONGO_NULL;
    }
}

void mode_H_b_return_init(BossSst* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_g[this->actor.params], 10.0f);
    this->ready = false;
    this->actionFunc = mode_H_b_return;
}

void mode_H_b_return(BossSst* this, PlayState* play) {
    add_calc(&this->actor.world.pos.y, ROOM_CENTER_Y + 250.0f, 0.5f, 70.0f, 5.0f);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (!this->ready) {
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_h[this->actor.params], 10.0f);
            this->ready = true;
        }
    }
    Actor_level_SE_set(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void mode_H_freeze_init(BossSst* this) {
    s32 i;

    HAND_STATE(this) = HAND_FROZEN;
    xyz_t_move(&this->center, &this->actor.world.pos);
    mode_H_f_punch_ready_init(OTHER_HAND(this));
    this->ready = false;
    this->effectMode = BONGO_ICE;
    this->timer = 35;
    for (i = 0; i < 18; i++) {
        this->effects[i].move = false;
    }

    sst_set_eff_ice(this, 0);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 10);
    this->handAngSpeed = 0;
    this->actionFunc = mode_H_freeze;
}

void mode_H_freeze(BossSst* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if ((this->timer % 2) != 0) {
        sst_set_eff_ice(this, (this->timer >> 1) + 1);
    }

    if (this->ready) {
        sst_eff_ice_broken(this);
        mode_H_return_init(this);
        sst_body->ready = true;
    } else {
        this->actor.colorFilterTimer = 10;
        if (this->handAngSpeed != 0) {
            f32 offY = sin_s(OTHER_HAND(this)->actor.shape.rot.x) * 5.0f;
            f32 offXZ = cos_s(OTHER_HAND(this)->actor.shape.rot.x) * 5.0f;

            if ((this->handAngSpeed % 2) != 0) {
                offY *= -1.0f;
                offXZ *= -1.0f;
            }

            this->actor.world.pos.x = this->center.x + (cos_s(OTHER_HAND(this)->actor.shape.rot.y) * offXZ);
            this->actor.world.pos.y = this->center.y + offY;
            this->actor.world.pos.z = this->center.z + (sin_s(OTHER_HAND(this)->actor.shape.rot.y) * offXZ);
            this->handAngSpeed--;
        }
    }
}

void mode_H_f_punch_ready_init(BossSst* this) {
    HAND_STATE(this) = HAND_BREAK_ICE;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, sst_hand_pose_d[this->actor.params], 5.0f);
    this->ready = false;
    this->actor.colorFilterTimer = 0;
    if (this->effectMode == BONGO_ICE) {
        this->effectMode = BONGO_NULL;
    }

    this->radius = Actor_search_position_distanceXZ(&this->actor, &OTHER_HAND(this)->center);
    this->targetYaw = Actor_search_position_angleY(&this->actor, &OTHER_HAND(this)->center);
    sst_set_ac_shield(this, true);
    this->actionFunc = mode_H_f_punch_ready;
}

void mode_H_f_punch_ready(BossSst* this, PlayState* play) {
    s32 inPosition;

    inPosition = chase_angle(&this->actor.shape.rot.y, this->targetYaw, 0x400);
    inPosition &= chase_angle(&this->actor.shape.rot.x, 0x1000, 0x400);
    inPosition &= chase_angle(&this->actor.shape.rot.z, 0, 0x800);
    inPosition &= chase_angle(&this->handYRotMod, 0, 0x400);
    inPosition &= chase_f(&this->actor.world.pos.y, OTHER_HAND(this)->center.y + 200.0f, 50.0f);
    inPosition &= chase_f(&this->radius, 400.0f, 60.0f);
    this->actor.world.pos.x = OTHER_HAND(this)->center.x - (sin_s(this->targetYaw) * this->radius);
    this->actor.world.pos.z = OTHER_HAND(this)->center.z - (cos_s(this->targetYaw) * this->radius);
    if (Skeleton_Info2_anime_play(&this->skelAnime) && inPosition) {
        mode_H_f_punch_init(this);
    }
}

void mode_H_f_punch_init(BossSst* this) {
    this->timer = 9;
    this->actionFunc = mode_H_f_punch;
    this->actor.speed = 0.5f;
}

void mode_H_f_punch(BossSst* this, PlayState* play) {
    if ((this->timer % 2) != 0) {
        this->actor.speed *= 1.5f;
        this->actor.speed = CLAMP_MAX(this->actor.speed, 60.0f);

        if (chase_f(&this->radius, 100.0f, this->actor.speed)) {
            sst_set_eff_ice_piece(this);
            if (this->timer != 0) {
                this->timer--;
            }

            if (this->timer != 0) {
                Actor_SE_set(&this->actor, NA_SE_EV_ICE_BROKEN);
            }

            OTHER_HAND(this)->handAngSpeed = 5;
        }
    } else {
        this->actor.speed *= 0.8f;
        chase_f(&this->radius, 500.0f, this->actor.speed);
        if (this->actor.speed < 2.0f) {
            if (this->timer != 0) {
                this->timer--;
            }
        }
    }

    this->actor.world.pos.x = OTHER_HAND(this)->center.x - (sin_s(this->targetYaw) * this->radius);
    this->actor.world.pos.z = OTHER_HAND(this)->center.z - (cos_s(this->targetYaw) * this->radius);
    this->actor.world.pos.y = OTHER_HAND(this)->center.y + (this->radius * 0.4f);
    if (this->timer == 0) {
        OTHER_HAND(this)->ready = true;
        mode_H_return_init(this);
    }

    Actor_level_SE_set(&this->actor, NA_SE_EN_SHADEST_HAND_FLY - SFX_FLAG);
}

void sst_hand_set_player_catch(BossSst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->grabPlayer(play, player)) {
        player->actor.parent = &this->actor;
        if (player->actor.colChkInfo.health > 0) {
            this->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
            if (HAND_STATE(this) == HAND_CLAP) {
                OTHER_HAND(this)->colliderJntSph.base.ocFlags1 &= ~OC1_ON;
            }
        }
    }
}

void sst_hand_reset_player_catch(BossSst* this, PlayState* play, s32 dropPlayer) {
    Player* player = GET_PLAYER(play);

    if (player->actor.parent == &this->actor) {
        player->actor.parent = NULL;
        player->av2.actionVar2 = 100;
        this->colliderJntSph.base.ocFlags1 |= OC1_ON;
        OTHER_HAND(this)->colliderJntSph.base.ocFlags1 |= OC1_ON;
        if (dropPlayer) {
            Actor_player_power_damage_set(play, &this->actor, 0.0f, this->actor.shape.rot.y, 0.0f);
        }
    }
}

void sst_move_hand_from_body(BossSst* this) {
    BossSst* hand;
    Vec3f* vec;
    f32 sn;
    f32 cs;
    s32 i;

    sn = sin_s(this->actor.shape.rot.y);
    cs = cos_s(this->actor.shape.rot.y);
    if (this->actionFunc != mode_B_attack_after) {
        this->actor.world.pos.x = map_center_pos.x + (this->radius * sn);
        this->actor.world.pos.z = map_center_pos.z + (this->radius * cs);
    }

    for (i = 0; i < 2; i++) {
        hand = sst_hand[i];
        vec = &hand_pos[i];

        hand->actor.world.pos.x = this->actor.world.pos.x + (vec->z * sn) + (vec->x * cs);
        hand->actor.world.pos.y = this->actor.world.pos.y + vec->y;
        hand->actor.world.pos.z = this->actor.world.pos.z + (vec->z * cs) - (vec->x * sn);

        hand->actor.home.pos.x = this->actor.world.pos.x + (400.0f * sn) + (-200.0f * hand->vParity * cs);
        hand->actor.home.pos.y = this->actor.world.pos.y;
        hand->actor.home.pos.z = this->actor.world.pos.z + (400.0f * cs) - (-200.0f * hand->vParity * sn);

        hand->actor.home.rot.y = this->actor.shape.rot.y;
        hand->actor.shape.rot.y = hand_shape_offset_angle[i] + this->actor.shape.rot.y;

        if (hand->actor.world.pos.y < hand->actor.floorHeight) {
            hand->actor.world.pos.y = hand->actor.floorHeight;
        }
    }
}

void sst_hand_change_attack(BossSst* this) {
    f32 rand = fqrand() * 6.0f;
    s32 randInt;

    if (HAND_STATE(OTHER_HAND(this)) == HAND_DAMAGED) {
        rand *= 5.0f / 6;
        if (rand > 4.0f) {
            rand = 4.0f;
        }
    }

    randInt = rand;
    if (randInt == 0) {
        mode_H_press_ready_init(this);
    } else if (randInt == 1) {
        mode_H_chop_ready_init(this);
    } else if (randInt == 2) {
        mode_H_punch_ready_init(this);
    } else if (randInt == 5) {
        mode_H_crush_ready_init(this);
    } else { // randInt == 3 || randInt == 4
        mode_H_catch_ready_init(this);
    }
}

void sst_hand_set_at_check(BossSst* this, s32 damage) {
    s32 i;

    this->colliderJntSph.base.atFlags |= AT_ON;
    for (i = 0; i < 11; i++) {
        this->colliderJntSph.elements[i].base.atDmgInfo.damage = damage;
    }
}

void sst_set_ac_shield(BossSst* this, s32 isInv) {
    this->colliderJntSph.base.acFlags &= ~AC_HIT;
    if (isInv) {
        this->colliderJntSph.base.colMaterial = COL_MATERIAL_HARD;
        this->colliderJntSph.base.acFlags |= AC_HARD;
    } else {
        this->colliderJntSph.base.colMaterial = COL_MATERIAL_HIT0;
        this->colliderJntSph.base.acFlags &= ~AC_HARD;
    }
}

void sst_body_SE_set(BossSst* this, u16 sfxId) {
    Na_StartObjectSe_F(&this->center, sfxId);
}

void Boss_Sst_hand_damage_proc(BossSst* this, PlayState* play) {
    if ((this->colliderJntSph.base.acFlags & AC_HIT) && (this->colliderJntSph.base.colMaterial != COL_MATERIAL_HARD)) {
        s32 bothHands = true;

        this->colliderJntSph.base.acFlags &= ~AC_HIT;
        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            this->colliderJntSph.base.atFlags &= ~(AT_ON | AT_HIT);
            this->colliderJntSph.base.acFlags &= ~AC_ON;
            this->colliderJntSph.base.ocFlags1 &= ~OC1_NO_PUSH;
            sst_hand_reset_player_catch(this, play, true);
            if (HAND_STATE(OTHER_HAND(this)) == HAND_CLAP) {
                sst_hand_reset_player_catch(OTHER_HAND(this), play, true);
                mode_H_return_init(OTHER_HAND(this));
            }

            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            if (this->actor.colChkInfo.damageEffect == 3) {
                mode_H_freeze_init(this);
            } else {
                mode_H_damage_init(this);
                if (HAND_STATE(OTHER_HAND(this)) != HAND_DAMAGED) {
                    bothHands = false;
                }
            }

            mode_B_h_damage_init(sst_body, bothHands);
            Item_set0(play, &this->actor.world.pos,
                                 (fqrand() < 0.5f) ? ITEM00_ARROWS_SMALL : ITEM00_MAGIC_SMALL);
            Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_DAMAGE_HAND);
        }
    }
}

void Boss_Sst_body_damage_proc(BossSst* this, PlayState* play) {
    if (this->colliderCyl.base.acFlags & AC_HIT) {
        this->colliderCyl.base.acFlags &= ~AC_HIT;
        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (this->actionFunc == mode_B_damage_wait) {
                if (hp_down(&this->actor) == 0) {
                    Actor_info_finish(play, &this->actor);
                    mode_B_final_damage_init(this, play);
                } else {
                    mode_B_sword_hit_init(this);
                }

                mode_H_b_damage_init(sst_hand[LEFT]);
                mode_H_b_damage_init(sst_hand[RIGHT]);
            } else {
                mode_B_arrow_hit_init(this);
                if (HAND_STATE(sst_hand[RIGHT]) == HAND_FROZEN) {
                    sst_eff_ice_broken(sst_hand[RIGHT]);
                } else if (HAND_STATE(sst_hand[LEFT]) == HAND_FROZEN) {
                    sst_eff_ice_broken(sst_hand[LEFT]);
                }

                mode_H_b_damage_wait_init(sst_hand[RIGHT]);
                mode_H_b_damage_wait_init(sst_hand[LEFT]);
            }
        }
    }
}

void Boss_Sst_hand_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BossSst* this = (BossSst*)thisx;
    BossSstHandTrail* trail;

    if (this->colliderCyl.base.atFlags & AT_ON) {
        if ((this->effects[0].move < 5) ||
            (this->actor.xzDistToPlayer < ((this->effects[2].scale * 0.01f) * SstBlastAtPipeData.dim.radius)) ||
            (this->colliderCyl.base.atFlags & AT_HIT)) {
            this->colliderCyl.base.atFlags &= ~(AT_ON | AT_HIT);
        } else {
            this->colliderCyl.dim.radius = (this->effects[0].scale * 0.01f) * SstBlastAtPipeData.dim.radius;
        }
    }

    Boss_Sst_hand_damage_proc(this, play);
    this->actionFunc(this, play);
    Actor_BGcheck2(play, &this->actor, 50.0f, 130.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    Actor_world_to_eye(&this->actor, 0.0f);
    if (this->colliderJntSph.base.atFlags & AT_ON) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderJntSph.base);
    }

#if OOT_VERSION < NTSC_1_2
    if (this->colliderJntSph.base.acFlags & AC_ON) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderJntSph.base);
    }
#else
    if ((sst_body->actionFunc != mode_B_start_demo_wait) && (sst_body->actionFunc != mode_B_start_demo) &&
        (this->colliderJntSph.base.acFlags & AC_ON)) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderJntSph.base);
    }
#endif

    if (this->colliderJntSph.base.ocFlags1 & OC1_ON) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderJntSph.base);
    }

    if (this->colliderCyl.base.atFlags & AT_ON) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderCyl.base);
    }

    if ((HAND_STATE(this) != HAND_DEATH) && (HAND_STATE(this) != HAND_WAIT) && (HAND_STATE(this) != HAND_BEAT) &&
        (HAND_STATE(this) != HAND_FROZEN)) {
        this->trailCount++;
        this->trailCount = CLAMP_MAX(this->trailCount, 7);
    } else {
        this->trailCount--;
        this->trailCount = CLAMP_MIN(this->trailCount, 0);
    }

    trail = &this->handTrails[this->trailIndex];
    xyz_t_move(&trail->world.pos, &this->actor.world.pos);
    trail->world.rot = this->actor.shape.rot;
    trail->zPosMod = this->handZPosMod;
    trail->yRotMod = this->handYRotMod;

    this->trailIndex = (this->trailIndex + 1) % 7;
    Boss_Sst_eff_move(&this->actor, play);
}

void Boss_Sst_body_actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = (PlayState*)play2;
    BossSst* this = (BossSst*)thisx;

    Actor_search_position_project_distanceXZ(&this->actor, &hand_pos[RIGHT], &sst_hand[RIGHT]->actor.world.pos);
    Actor_search_position_project_distanceXZ(&this->actor, &hand_pos[LEFT], &sst_hand[LEFT]->actor.world.pos);

    hand_shape_offset_angle[LEFT] = sst_hand[LEFT]->actor.shape.rot.y - thisx->shape.rot.y;
    hand_shape_offset_angle[RIGHT] = sst_hand[RIGHT]->actor.shape.rot.y - thisx->shape.rot.y;

    Boss_Sst_body_damage_proc(this, play);
    this->actionFunc(this, play);
    if (this->vVanish) {
        if (!play->actorCtx.lensActive || (thisx->colorFilterTimer != 0)) {
            this->actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;
        } else {
            this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
        }
    }

    if (this->colliderJntSph.base.atFlags & AT_ON) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderJntSph.base);
    }

#if OOT_VERSION < NTSC_1_2
    if (this->colliderCyl.base.acFlags & AC_ON) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderCyl.base);
    }
    CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderJntSph.base);
#else
    if ((this->actionFunc != mode_B_start_demo_wait) && (this->actionFunc != mode_B_start_demo)) {
        if (this->colliderCyl.base.acFlags & AC_ON) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderCyl.base);
        }
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderJntSph.base);
    }
#endif

    if (this->colliderJntSph.base.ocFlags1 & OC1_ON) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderJntSph.base);
    }

    sst_move_hand_from_body(this);
    if ((!this->vVanish || CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) &&
        ((this->actionFunc == mode_B_attack_ready) || (this->actionFunc == mode_B_attack) ||
         (this->actionFunc == mode_B_h_freeze_wait) || (this->actionFunc == mode_B_arrow_hit) ||
         (this->actionFunc == mode_B_damage_wait) || (this->actionFunc == mode_B_sword_hit))) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }

    if (this->actionFunc == mode_B_attack) {
        sst_body_SE_set(this, NA_SE_EN_SHADEST_MOVE - SFX_FLAG);
    }

    Boss_Sst_eff_move(&this->actor, play);
}

s32 sst_hand_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossSst* this = (BossSst*)thisx;

    if (limbIndex == 1) {
        pos->z += this->handZPosMod;
        rot->y += this->handYRotMod;
    }
    return false;
}

void sst_hand_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    BossSst* this = (BossSst*)thisx;

    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->colliderJntSph);
}

s32 sst_hand_blure_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* data,
                                  Gfx** gfx) {
    BossSstHandTrail* trail = (BossSstHandTrail*)data;

    if (limbIndex == 1) {
        pos->z += trail->zPosMod;
        rot->y += trail->yRotMod;
    }
    return false;
}

void Boss_Sst_hand_actor_draw(Actor* thisx, PlayState* play) {
    BossSst* this = (BossSst*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_sst.c", 6563);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x80, dead_prim_color.r, dead_prim_color.g, dead_prim_color.b, 255);

    if (!tex_change_flg) {
        gSPSegment(POLY_OPA_DISP++, 0x08, &Actor_change_render_mode[2]);
    } else {
        gDPSetEnvColor(POLY_OPA_DISP++, dead_env_color.r, dead_env_color.g, dead_env_color.b, 0);
        gSPSegment(POLY_OPA_DISP++, 0x08, sst_tex_change);
    }

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          sst_hand_display1, sst_hand_display2, this);
    if (this->trailCount >= 2) {
        BossSstHandTrail* trail;
        BossSstHandTrail* trail2;
        s32 i;
        s32 idx;
        s32 end;
        s32 pad;

        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

        end = this->trailCount >> 1;
        idx = (this->trailIndex + 4) % 7;
        trail = &this->handTrails[idx];
        trail2 = &this->handTrails[(idx + 2) % 7];

        for (i = 0; i < end; i++) {
            if (Math3DLengthSquare(&trail2->world.pos, &trail->world.pos) > SQ(30.0f)) {
                Matrix_softcv3_load(trail->world.pos.x, trail->world.pos.y, trail->world.pos.z,
                                             &trail->world.rot);
                Matrix_scale(0.02f, 0.02f, 0.02f, MTXMODE_APPLY);

                gSPSegment(POLY_XLU_DISP++, 0x08, sst_blure);
                gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, ((3 - i) * 10) + 20, 0, ((3 - i) * 20) + 50,
                                ((3 - i) * 30) + 70);

                POLY_XLU_DISP = Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                                   this->skelAnime.dListCount, sst_hand_blure_display1, NULL,
                                                   trail, POLY_XLU_DISP);
            }
            idx = (idx + 5) % 7;
            trail2 = trail;
            trail = &this->handTrails[idx];
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_sst.c", 6654);

    Boss_Sst_eff_draw(&this->actor, play);
}

s32 sst_body_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                             Gfx** gfx) {
    BossSst* this = (BossSst*)thisx;

    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS) && this->vVanish) {
        *dList = NULL;
    } else if (this->actionFunc == mode_B_dead_move) { // Animation modifications for death cutscene
        s32 shakeAmp = (this->timer / 10) + 1;

        if ((limbIndex == 3) || (limbIndex == 39) || (limbIndex == 42)) {
            rot->x += ((0x500 * fqrand() + 0xA00) / 0x10) * shakeAmp * sinf(this->timer * (M_PI / 5));
            rot->z -=
                ((0x800 * fqrand() + 0x1000) / 0x10) * shakeAmp * sinf((this->timer % 5) * (M_PI / 5)) + 0x1000;

            if (limbIndex == 3) {
                rot->y += ((0x500 * fqrand() + 0xA00) / 0x10) * shakeAmp * sinf(this->timer * (M_PI / 5));
            }
        } else if ((limbIndex == 5) || (limbIndex == 6)) {
            rot->z -=
                ((0x280 * fqrand() + 0x500) / 0x10) * shakeAmp * sinf((this->timer % 5) * (M_PI / 5)) + 0x500;

            if (limbIndex == 5) {
                rot->x += ((0x500 * fqrand() + 0xA00) / 0x10) * shakeAmp * sinf(this->timer * (M_PI / 5));
                rot->y += ((0x500 * fqrand() + 0xA00) / 0x10) * shakeAmp * sinf(this->timer * (M_PI / 5));
            }
        } else if (limbIndex == 2) {
            rot->x += ((0x200 * fqrand() + 0x400) / 0x10) * shakeAmp * sinf(this->timer * (M_PI / 5));
            rot->y += ((0x200 * fqrand() + 0x400) / 0x10) * shakeAmp * sinf(this->timer * (M_PI / 5));
            rot->z -=
                ((0x100 * fqrand() + 0x200) / 0x10) * shakeAmp * sinf((this->timer % 5) * (M_PI / 5)) + 0x200;
        }
    } else if (this->actionFunc == mode_B_final_damage) {
        s32 timer12;

        if (this->timer > 48) {
            timer12 = this->timer - 36;
        } else {
            timer12 = ((this->timer > 6) ? 6 : this->timer) * 2;
        }

        if ((limbIndex == 3) || (limbIndex == 39) || (limbIndex == 42)) {
            rot->z -= 0x2000 * sinf(timer12 * (M_PI / 24));
        } else if ((limbIndex == 5) || (limbIndex == 6)) {
            rot->z -= 0xA00 * sinf(timer12 * (M_PI / 24));
        } else if (limbIndex == 2) {
            rot->z -= 0x400 * sinf(timer12 * (M_PI / 24));
        }
    } else if ((this->actionFunc == mode_B_dead_stop) || (this->actionFunc == mode_B_dead_drop) ||
               (this->actionFunc == mode_B_dead_scale)) {
        if ((limbIndex == 3) || (limbIndex == 39) || (limbIndex == 42)) {
            rot->z -= 0x1000;
        } else if ((limbIndex == 5) || (limbIndex == 6)) {
            rot->z -= 0x500;
        } else if (limbIndex == 2) {
            rot->z -= 0x200;
        }
    }
    return false;
}

void sst_body_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    static Vec3f local_eye = { 0.0f, 0.0f, 0.0f };
    static Vec3f local_pipe_c = { 1000.0f, 0.0f, 0.0f };
    BossSst* this = (BossSst*)thisx;
    Vec3f headPos;

    if (limbIndex == 8) {
        Matrix_Position(&local_eye, &this->actor.focus.pos);
        Matrix_Position(&local_pipe_c, &headPos);
        this->colliderCyl.dim.pos.x = headPos.x;
        this->colliderCyl.dim.pos.y = headPos.y;
        this->colliderCyl.dim.pos.z = headPos.z;
    }

    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->colliderJntSph);
}

void Boss_Sst_body_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BossSst* this = (BossSst*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_sst.c", 6810);

    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0x00, 0x80, dead_prim_color.r, dead_prim_color.g, dead_prim_color.b, 255);
        if (!tex_change_flg) {
            gSPSegment(POLY_OPA_DISP++, 0x08, &Actor_change_render_mode[2]);
        } else {
            gDPSetEnvColor(POLY_OPA_DISP++, dead_env_color.r, dead_env_color.g, dead_env_color.b, 0);
            gSPSegment(POLY_OPA_DISP++, 0x08, sst_tex_change);
        }
    } else {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 255, 255, 255);
        gSPSegment(POLY_XLU_DISP++, 0x08, &Actor_change_render_mode[2]);
    }

    if (this->actionFunc == mode_B_dead_move) {
        f32 randPitch = fqrand() * (2 * M_PI);
        f32 randYaw = fqrand() * (2 * M_PI);

        Matrix_rotateY(randYaw, MTXMODE_APPLY);
        Matrix_rotateX(randPitch, MTXMODE_APPLY);
        Matrix_scale((this->timer * 0.000375f) + 1.0f, 1.0f - (this->timer * 0.00075f),
                     (this->timer * 0.000375f) + 1.0f, MTXMODE_APPLY);
        Matrix_rotateX(-randPitch, MTXMODE_APPLY);
        Matrix_rotateY(-randYaw, MTXMODE_APPLY);
    }

    if (!CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        POLY_OPA_DISP =
            Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               sst_body_display1, sst_body_display2, this, POLY_OPA_DISP);
    } else {
        POLY_XLU_DISP =
            Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               sst_body_display1, sst_body_display2, this, POLY_XLU_DISP);
    }

    if ((this->actionFunc == mode_B_start_demo) && (113 >= this->timer) && (this->timer > 20)) {
        s32 yOffset;
        Vec3f vanishMaskPos;
        Vec3f vanishMaskOffset;

        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x00, 0, 0, 18, 255);

        yOffset = 113 * 8 - this->timer * 8;
        vanishMaskPos.x = ROOM_CENTER_X + 85.0f;
        vanishMaskPos.y = ROOM_CENTER_Y - 250.0f + yOffset;
        vanishMaskPos.z = ROOM_CENTER_Z + 190.0f;
        if (vanishMaskPos.y > 450.0f) {
            vanishMaskPos.y = 450.0f;
        }

        Matrix_MtxF_Position2(&vanishMaskPos, &vanishMaskOffset, &play->billboardMtxF);
        Matrix_translate(this->actor.world.pos.x + vanishMaskOffset.x, this->actor.world.pos.y + vanishMaskOffset.y,
                         this->actor.world.pos.z + vanishMaskOffset.z, MTXMODE_NEW);
        Matrix_scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_sst.c", 6934);
        gSPDisplayList(POLY_XLU_DISP++, sst_mask_model);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_sst.c", 6941);

    Skin_Matrix_MulVector(&play->viewProjectionMtxF, &this->actor.focus.pos, &this->center);
    Boss_Sst_eff_draw(&this->actor, play);
}

void sst_set_eff_body_dead_shadow(BossSst* this) {
    static Vec3f local_shadow_pos[] = {
        { 0.0f, 0.0f, 340.0f },
        { -160.0f, 0.0f, 250.0f },
        { 160.0f, 0.0f, 250.0f },
    };
    s32 pad;
    s32 i;
    f32 sn;
    f32 cs;
    BossSstEffect* shadow;
    Vec3f* offset;

    this->effectMode = BONGO_SHADOW;
    sn = sin_s(this->actor.shape.rot.y);
    cs = cos_s(this->actor.shape.rot.y);

    for (i = 0; i < 3; i++) {
        shadow = &this->effects[i];
        offset = &local_shadow_pos[i];

        shadow->pos.x = this->actor.world.pos.x + (sn * offset->z) + (cs * offset->x);
        shadow->pos.y = 0.0f;
        shadow->pos.z = this->actor.world.pos.z + (cs * offset->z) - (sn * offset->x);

        shadow->scale = 1450;
        shadow->alpha = 254;
        shadow->status = 65;
    }

    this->effects[3].status = -1;
}

void sst_set_eff_hand_dead_shadow(BossSst* this) {
    this->effectMode = BONGO_SHADOW;
    this->effects[0].pos.x = this->actor.world.pos.x + (cos_s(this->actor.shape.rot.y) * 30.0f * this->vParity);
    this->effects[0].pos.z = this->actor.world.pos.z - (sin_s(this->actor.shape.rot.y) * 30.0f * this->vParity);
    this->effects[0].pos.y = this->actor.world.pos.y;
    this->effects[0].scale = 2300;
    this->effects[0].alpha = 254;
    this->effects[0].status = 5;
    this->effects[1].status = -1;
}

void sst_set_eff_blast(BossSst* this) {
    s32 i;
    s32 scale = 120;
    s32 alpha = 250;
    BossSstEffect* shockwave;

    Actor_SE_set(&this->actor, NA_SE_EN_SHADEST_HAND_WAVE);
    this->effectMode = BONGO_SHOCKWAVE;

    for (i = 0; i < 3; i++) {
        shockwave = &this->effects[i];

        xyz_t_move(&shockwave->pos, &this->actor.world.pos);
        shockwave->move = (i + 9) * 2;
        shockwave->scale = scale;
        shockwave->alpha = alpha / shockwave->move;
        scale -= 50;
        alpha -= 25;
    }
}

void sst_set_eff_ice(BossSst* this, s32 index) {
    BossSstEffect* ice = &this->effects[index];
    Sphere16* sphere;

    if (index < 11) {
        sphere = &this->colliderJntSph.elements[index].dim.worldSphere;

        ice->pos.x = sphere->center.x;
        ice->pos.y = sphere->center.y;
        ice->pos.z = sphere->center.z;
        if (index == 0) {
            ice->pos.x -= 25.0f;
            ice->pos.y -= 25.0f;
            ice->pos.z -= 25.0f;
        }
    } else {
        sphere = &this->colliderJntSph.elements[0].dim.worldSphere;

        ice->pos.x = ((((index - 11) & 1) ? 1 : -1) * 25.0f) + sphere->center.x;
        ice->pos.y = ((((index - 11) & 2) ? 1 : -1) * 25.0f) + sphere->center.y;
        ice->pos.z = ((((index - 11) & 4) ? 1 : -1) * 25.0f) + sphere->center.z;
    }

    ice->pos.x -= this->actor.world.pos.x;
    ice->pos.y -= this->actor.world.pos.y;
    ice->pos.z -= this->actor.world.pos.z;

    ice->status = 0;

    ice->rot.x = fqrand() * 0x10000;
    ice->rot.y = fqrand() * 0x10000;
    ice->rot.z = fqrand() * 0x10000;

    ice->alpha = 120;
    ice->move = true;

    ice->vel.x = (fqrand() * 0.06f + 0.12f) * ice->pos.x;
    ice->vel.y = (fqrand() * 15.0f + 5.0f);
    ice->vel.z = (fqrand() * 0.06f + 0.12f) * ice->pos.z;
    ice->scale = 4000;

    if ((index % 2) == 0) {
        Actor_SE_set(&this->actor, NA_SE_PL_FREEZE_S);
    }
}

void sst_set_eff_ice_piece(BossSst* this) {
    s32 i;
    Vec3f spawnPos;
    f32 offXZ;
    BossSstEffect* ice;

    this->effectMode = BONGO_ICE;
    offXZ = cos_s(this->actor.shape.rot.x) * 50.0f;
    spawnPos.x = cos_s(this->actor.shape.rot.y) * offXZ + this->actor.world.pos.x;
    spawnPos.y = sin_s(this->actor.shape.rot.x) * 50.0f + this->actor.world.pos.y - 10.0f;
    spawnPos.z = sin_s(this->actor.shape.rot.y) * offXZ + this->actor.world.pos.z;

    for (i = 0; i < 18; i++) {
        ice = &this->effects[i];

        xyz_t_move(&ice->pos, &spawnPos);
        ice->status = 1;
        ice->rot.x = fqrand() * 0x10000;
        ice->rot.y = fqrand() * 0x10000;
        ice->rot.z = fqrand() * 0x10000;

        ice->alpha = 120;
        ice->move = true;

        ice->vel.x = rnd_fx(20.0f);
        ice->vel.y = fqrand() * 10.0f + 3.0f;
        ice->vel.z = rnd_fx(20.0f);

        ice->scale = fqrand() * 200.0f + 400.0f;
    }
}

void sst_eff_ice_broken(BossSst* this) {
    s32 i;

    this->effects[0].status = 1;
    Actor_SE_set(&this->actor, NA_SE_PL_ICE_BROKEN);

    for (i = 0; i < 18; i++) {
        BossSstEffect* ice = &this->effects[i];

        if (ice->move) {
            ice->pos.x += this->actor.world.pos.x;
            ice->pos.y += this->actor.world.pos.y;
            ice->pos.z += this->actor.world.pos.z;
        }
    }
}

void Boss_Sst_eff_move(Actor* thisx, PlayState* play) {
    BossSst* this = (BossSst*)thisx;
    BossSstEffect* effect;
    s32 i;

    if (this->effectMode != BONGO_NULL) {
        if (this->effectMode == BONGO_ICE) {
            if (this->effects[0].status) {
                for (i = 0; i < 18; i++) {
                    effect = &this->effects[i];

                    if (effect->move) {
                        effect->pos.x += effect->vel.x;
                        effect->pos.y += effect->vel.y;
                        effect->pos.z += effect->vel.z;
                        effect->alpha -= 3;
                        effect->vel.y -= 1.0f;
                        effect->rot.x += 0xD00;
                        effect->rot.y += 0x1100;
                        effect->rot.z += 0x1500;
                    }
                }
            }
            if (this->effects[0].alpha == 0) {
                this->effectMode = BONGO_NULL;
            }
        } else if (this->effectMode == BONGO_SHOCKWAVE) {
            BossSstEffect* effect2;
            s32 scale;

            for (i = 0; i < 3; i++) {
                effect2 = &this->effects[i];
                scale = effect2->move * 2;

                effect2->scale += CLAMP_MAX(scale, 20) + i;
                if (effect2->move != 0) {
                    effect2->move--;
                }
            }

            if (this->effects[0].move == 0) {
                this->effectMode = BONGO_NULL;
            }
        } else if (this->effectMode == BONGO_SHADOW) {
            effect = &this->effects[0];

            if (this->actor.params == BONGO_HEAD) {
                Skin_Matrix_MulVector(&play->viewProjectionMtxF, &this->actor.focus.pos, &this->center);
                sst_body_SE_set(this, NA_SE_EN_SHADEST_LAST - SFX_FLAG);
            }
            while (effect->status != -1) {
                if (effect->status == 0) {
                    effect->alpha -= 2;
                } else {
                    effect->scale += effect->status;
                }

                effect->scale = CLAMP_MAX(effect->scale, 10000);
                effect++;
            }
        }
    }
}

void Boss_Sst_eff_draw(Actor* thisx, PlayState* play) {
    PlayState* play2 = (PlayState*)play;
    BossSst* this = (BossSst*)thisx;
    s32 i;
    BossSstEffect* effect;

    if (this->effectMode != BONGO_NULL) {
        OPEN_DISPS(play->state.gfxCtx, "../z_boss_sst.c", 7302);

        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        if (this->effectMode == BONGO_ICE) {
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, play2->gameplayFrames % 256, 0x20, 0x10,
                                        1, 0, (play2->gameplayFrames * 2) % 256, 0x40, 0x20));
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, this->effects[0].alpha);
            gSPDisplayList(POLY_XLU_DISP++, gBongoIceCrystalDL);

            for (i = 0; i < 18; i++) {
                effect = &this->effects[i];
                if (effect->move) {
                    Setpos_HiliteReflect_xlu_init(&effect->pos, play2);
                    if (this->effects[0].status != 0) {
                        Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                    } else {
                        Matrix_translate(effect->pos.x + this->actor.world.pos.x,
                                         effect->pos.y + this->actor.world.pos.y,
                                         effect->pos.z + this->actor.world.pos.z, MTXMODE_NEW);
                    }

                    Matrix_rotateXYZ(effect->rot.x, effect->rot.y, effect->rot.z, MTXMODE_APPLY);
                    Matrix_scale(effect->scale * 0.001f, effect->scale * 0.001f, effect->scale * 0.001f, MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_sst.c", 7350);
                    gSPDisplayList(POLY_XLU_DISP++, gBongoIceShardDL);
                }
            }
        } else if (this->effectMode == BONGO_SHOCKWAVE) {
            f32 scaleY = 0.005f;

            gDPPipeSync(POLY_XLU_DISP++);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, play2->gameplayFrames % 128, 0, 0x20, 0x40,
                                        1, 0, (play2->gameplayFrames * -15) % 256, 0x20, 0x40));

            for (i = 0; i < 3; i++, scaleY -= 0.001f) {
                effect = &this->effects[i];

                if (effect->move != 0) {
                    Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                    Matrix_scale(effect->scale * 0.001f, scaleY, effect->scale * 0.001f, MTXMODE_APPLY);

                    gDPPipeSync(POLY_XLU_DISP++);
                    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 30, 0, 30, effect->alpha * effect->move);
                    gDPSetEnvColor(POLY_XLU_DISP++, 30, 0, 30, 0);
                    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_sst.c", 7396);
                    gSPDisplayList(POLY_XLU_DISP++, gEffFireCircleDL);
                }
            }
        } else if (this->effectMode == BONGO_SHADOW) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 10, 10, 80, 0);
            gDPSetEnvColor(POLY_XLU_DISP++, 10, 10, 10, this->effects[0].alpha);

            effect = &this->effects[0];
            while (effect->status != -1) {
                Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                Matrix_scale(effect->scale * 0.001f, 1.0f, effect->scale * 0.001f, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_sst.c", 7423);
                gSPDisplayList(POLY_XLU_DISP++, sst_dead_shadow_model);
                effect++;
            }
        }

        CLOSE_DISPS(play->state.gfxCtx, "../z_boss_sst.c", 7433);
    }
}
