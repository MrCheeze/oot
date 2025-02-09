/*
 * File: z_en_fhg.c
 * Overlay: ovl_En_fHG
 * Description: Phantom Ganon's Horse
 */

#include "z_en_fhg.h"
#include "assets/objects/object_fhg/object_fhg.h"
#include "overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "overlays/actors/ovl_Boss_Ganondrof/z_boss_ganondrof.h"
#include "overlays/actors/ovl_En_Fhg_Fire/z_en_fhg_fire.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef struct EnfHGPainting {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yRot;
} EnfHGPainting; // size = 0x10

typedef enum EnfHGIntroState {
    /*  0 */ INTRO_WAIT,
    /*  1 */ INTRO_START,
    /*  2 */ INTRO_FENCE,
    /*  3 */ INTRO_BACK,
    /*  4 */ INTRO_REVEAL,
    /*  5 */ INTRO_CUT,
    /*  6 */ INTRO_LAUGH,
    /*  7 */ INTRO_TITLE,
    /*  8 */ INTRO_RETREAT,
    /*  9 */ INTRO_FINISH,
    /* 15 */ INTRO_READY = 15
} EnfHGIntroState;

void En_fHG_Actor_ct(Actor* thisx, PlayState* play2);
void En_fHG_Actor_dt(Actor* thisx, PlayState* play2);
void En_fHG_move(Actor* thisx, PlayState* play);
void En_fHG_display(Actor* thisx, PlayState* play);

static void mode_startdemo_init(EnfHG* this, PlayState* play);
static void mode_startdemo(EnfHG* this, PlayState* play);
void mode_kabeout_standby_init(EnfHG* this, PlayState* play, s16 paintingIndex);
void mode_kabeout_standby(EnfHG* this, PlayState* play);
void mode_kabeout(EnfHG* this, PlayState* play);
void mode_revise(EnfHG* this, PlayState* play);
void mode_kabein(EnfHG* this, PlayState* play);
void mode_ganon_wait(EnfHG* this, PlayState* play);

ActorProfile En_fHG_Profile = {
    /**/ ACTOR_EN_FHG,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_FHG,
    /**/ sizeof(EnfHG),
    /**/ En_fHG_Actor_ct,
    /**/ En_fHG_Actor_dt,
    /**/ En_fHG_move,
    /**/ En_fHG_display,
};

static EnfHGPainting kabe_position_data[] = {
    { { 0.0f, 60.0f, -315.0f }, 0x0000 },   { { -260.0f, 60.0f, -145.0f }, 0x2AAA },
    { { -260.0f, 60.0f, 165.0f }, 0x5554 }, { { 0.0f, 60.0f, 315.0f }, 0x7FFE },
    { { 260.0f, 60.0f, 155.0f }, 0xAAA8 },  { { 260.0f, 60.0f, -155.0f }, 0xD552 },
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_PHANTOM_GANON_PHASE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_STOP),
};

void En_fHG_Actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnfHG* this = (EnfHG*)thisx;

    ValueSet_process(&this->actor, value_init);
    Actor_Environment_sw_On(play, 0x14);
    Actor_set_scale(&this->actor, 0.011499999f);
    this->actor.gravity = -3.5f;
    Shape_Info_init(&this->actor.shape, -2600.0f, NULL, 20.0f);
    this->actor.speed = 0.0f;
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    Skin_AnimationWorkBuffer2_ct(play, &this->skin, &gPhantomHorseSkel, &gPhantomHorseRunningAnim);

    if (this->actor.params >= GND_FAKE_BOSS) {
        mode_kabeout_standby_init(this, play, this->actor.params - GND_FAKE_BOSS);
    } else {
        mode_startdemo_init(this, play);
    }
}

void En_fHG_Actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = (PlayState*)play2;
    EnfHG* this = (EnfHG*)thisx;

    PRINTF("F DT1\n");
    Skin_AnimationWorkBuffer2_dt(play, &this->skin);
    PRINTF("F DT2\n");
}

static void mode_startdemo_init(EnfHG* this, PlayState* play) {
    Skeleton_Info2_init_standard_repeat(&this->skin.skelAnime, &gPhantomHorseIdleAnim);
    this->actionFunc = mode_startdemo;
    this->actor.world.pos.x = GND_BOSSROOM_CENTER_X;
    this->actor.world.pos.y = GND_BOSSROOM_CENTER_Y - 267.0f;
    this->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
}

static void mode_startdemo(EnfHG* this, PlayState* play) {
    static Vec3f fhg_se_pos = { 0.0f, 0.0f, 50.0f };
    s32 pad64;
    Player* player = GET_PLAYER(play);
    BossGanondrof* bossGnd = (BossGanondrof*)this->actor.parent;
    s32 pad58;
    s32 pad54;

    if (this->cutsceneState != INTRO_FINISH) {
        Skeleton_Info2_anime_play(&this->skin.skelAnime);
    }
    switch (this->cutsceneState) {
        case INTRO_WAIT:
            if ((fabsf(player->actor.world.pos.x - (GND_BOSSROOM_CENTER_X + 0.0f)) < 150.0f) &&
                (fabsf(player->actor.world.pos.z - (GND_BOSSROOM_CENTER_Z + 0.0f)) < 150.0f)) {
                this->cutsceneState = INTRO_READY;
            }
            break;
        case INTRO_READY:
            if ((fabsf(player->actor.world.pos.x - (GND_BOSSROOM_CENTER_X + 0.0f)) < 100.0f) &&
                (fabsf(player->actor.world.pos.z - (GND_BOSSROOM_CENTER_Z + 315.0f)) < 100.0f)) {
                this->cutsceneState = INTRO_START;
                if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_PHANTOM_GANON_BATTLE)) {
                    this->timers[0] = 57;
                }
            }
            break;
        case INTRO_START:
            if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_PHANTOM_GANON_BATTLE)) {
                if (this->timers[0] == 55) {
                    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_SHUTTER,
                                       GND_BOSSROOM_CENTER_X + 0.0f, GND_BOSSROOM_CENTER_Y - 97.0f,
                                       GND_BOSSROOM_CENTER_Z + 308.0f, 0, 0, 0, DOORSHUTTER_PARAMS(SHUTTER_PG_BARS, 0));
                }
                if (this->timers[0] == 51) {
                    Actor_SE_set(this->actor.child, NA_SE_EV_SPEAR_FENCE);
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS);
                }
                if (this->timers[0] == 0) {
                    mode_kabeout_standby_init(this, play, fqrand() * 5.99f);
                    this->bossGndSignal = FHG_START_FIGHT;
                }
                break;
            }
            Demo_play_start(play, &play->csCtx);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            this->subCamId = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            this->cutsceneState = INTRO_FENCE;
            this->timers[0] = 60;
            this->actor.world.pos.y = GND_BOSSROOM_CENTER_Y - 7.0f;
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
            SET_EVENTCHKINF(EVENTCHKINF_BEGAN_PHANTOM_GANON_BATTLE);
            Actor_Environment_sw_On(play, 0x23);
            FALLTHROUGH;
        case INTRO_FENCE:
            player->actor.world.pos.x = GND_BOSSROOM_CENTER_X + 0.0f;
            player->actor.world.pos.y = GND_BOSSROOM_CENTER_Y + 7.0f;
            player->actor.world.pos.z = GND_BOSSROOM_CENTER_Z + 155.0f;
            player->actor.world.rot.y = player->actor.shape.rot.y = 0;
            player->actor.speed = 0.0f;
            this->subCamEye.x = GND_BOSSROOM_CENTER_X + 0.0f;
            this->subCamEye.y = GND_BOSSROOM_CENTER_Y + 37.0f;
            this->subCamEye.z = GND_BOSSROOM_CENTER_Z + 170.0f;
            this->subCamAt.x = GND_BOSSROOM_CENTER_X + 0.0f;
            this->subCamAt.y = GND_BOSSROOM_CENTER_Y + 47.0f;
            this->subCamAt.z = GND_BOSSROOM_CENTER_Z + 315.0f;
            if (this->timers[0] == 25) {
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_SHUTTER,
                                   GND_BOSSROOM_CENTER_X + 0.0f, GND_BOSSROOM_CENTER_Y - 97.0f,
                                   GND_BOSSROOM_CENTER_Z + 308.0f, 0, 0, 0, DOORSHUTTER_PARAMS(SHUTTER_PG_BARS, 0));
            }
            if (this->timers[0] == 21) {
                Actor_SE_set(this->actor.child, NA_SE_EV_SPEAR_FENCE);
            }
            if (this->timers[0] == 0) {
                this->cutsceneState = INTRO_BACK;
                this->timers[0] = 80;
            }
            break;
        case INTRO_BACK:
            if (this->timers[0] == 25) {
                Actor_SE_set(&this->actor, NA_SE_EV_GANON_HORSE_GROAN);
            }
            if (this->timers[0] == 20) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_9);
            }
            if (this->timers[0] == 1) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_OPENING_GANON);
            }
            add_calc2(&this->subCamEye.x, GND_BOSSROOM_CENTER_X + 40.0f, 0.05f, this->subCamVelFactor * 20.0f);
            add_calc2(&this->subCamEye.y, GND_BOSSROOM_CENTER_Y + 37.0f, 0.05f, this->subCamVelFactor * 20.0f);
            add_calc2(&this->subCamEye.z, GND_BOSSROOM_CENTER_Z + 80.0f, 0.05f, this->subCamVelFactor * 20.0f);
            add_calc2(&this->subCamAt.x, GND_BOSSROOM_CENTER_X - 100.0f, 0.05f, this->subCamVelFactor * 20.0f);
            add_calc2(&this->subCamAt.y, GND_BOSSROOM_CENTER_Y + 47.0f, 0.05f, this->subCamVelFactor * 20.0f);
            add_calc2(&this->subCamAt.z, GND_BOSSROOM_CENTER_Z + 335.0f, 0.05f, this->subCamVelFactor * 20.0f);
            add_calc2(&this->subCamVelFactor, 1.0f, 1.0f, 0.01f);
            if (this->timers[0] == 0) {
                this->cutsceneState = INTRO_REVEAL;
                this->timers[0] = 50;
                this->subCamVelFactor = 0.0f;
            }
            break;
        case INTRO_REVEAL:
            add_calc2(&this->subCamEye.x, GND_BOSSROOM_CENTER_X + 70.0f, 0.1f, this->subCamVelFactor * 20.0f);
            add_calc2(&this->subCamEye.y, GND_BOSSROOM_CENTER_Y + 7.0f, 0.1f, this->subCamVelFactor * 20.0f);
            add_calc2(&this->subCamEye.z, GND_BOSSROOM_CENTER_Z + 200.0f, 0.1f, this->subCamVelFactor * 20.0f);
            add_calc2(&this->subCamAt.x, GND_BOSSROOM_CENTER_X - 150.0f, 0.1f, this->subCamVelFactor * 20.0f);
            add_calc2(&this->subCamAt.y, GND_BOSSROOM_CENTER_Y + 107.0f, 0.1f, this->subCamVelFactor * 20.0f);
            add_calc2(&this->subCamAt.z, GND_BOSSROOM_CENTER_Z - 65.0f, 0.1f, this->subCamVelFactor * 40.0f);
            add_calc2(&this->subCamVelFactor, 1.0f, 1.0f, 0.05f);
            if (this->timers[0] == 5) {
                Actor_SE_set(&this->actor, NA_SE_EV_HORSE_SANDDUST);
            }
            if (this->timers[0] == 0) {
                this->cutsceneState = INTRO_CUT;
                this->timers[0] = 50;
                this->subCamVelFactor = 0.0f;
            }
            break;
        case INTRO_CUT:
            this->cutsceneState = INTRO_LAUGH;
            this->subCamEye.x = GND_BOSSROOM_CENTER_X + 50.0f;
            this->subCamEye.y = GND_BOSSROOM_CENTER_Y + 17.0f;
            this->subCamEye.z = GND_BOSSROOM_CENTER_Z + 110.0f;
            this->subCamAt.x = GND_BOSSROOM_CENTER_X - 150.0f;
            this->subCamAt.y = GND_BOSSROOM_CENTER_Y + 207.0f;
            this->subCamAt.z = GND_BOSSROOM_CENTER_Z - 155.0f;
            this->subCamEyeVel.x = fabsf(this->subCamEye.x - (GND_BOSSROOM_CENTER_X + 20.0f));
            this->subCamEyeVel.y = fabsf(this->subCamEye.y - (GND_BOSSROOM_CENTER_Y + 102.0f));
            this->subCamEyeVel.z = fabsf(this->subCamEye.z - (GND_BOSSROOM_CENTER_Z + 25.0f));
            this->subCamAtVel.x = fabsf(this->subCamAt.x - (GND_BOSSROOM_CENTER_X - 150.0f));
            this->subCamAtVel.y = fabsf(this->subCamAt.y - (GND_BOSSROOM_CENTER_Y + 197.0f));
            this->subCamAtVel.z = fabsf(this->subCamAt.z - (GND_BOSSROOM_CENTER_Z - 65.0f));
            this->timers[0] = 250;
            FALLTHROUGH;
        case INTRO_LAUGH:
            add_calc2(&this->subCamEye.x, GND_BOSSROOM_CENTER_X + 20.0f, 0.05f,
                           this->subCamVelFactor * this->subCamEyeVel.x);
            add_calc2(&this->subCamEye.y, GND_BOSSROOM_CENTER_Y + 102.0f, 0.05f,
                           this->subCamVelFactor * this->subCamEyeVel.y);
            add_calc2(&this->subCamEye.z, GND_BOSSROOM_CENTER_Z + 25.0f, 0.05f,
                           this->subCamVelFactor * this->subCamEyeVel.z);
            add_calc2(&this->subCamAt.x, GND_BOSSROOM_CENTER_X - 150.0f, 0.05f,
                           this->subCamVelFactor * this->subCamAtVel.x);
            add_calc2(&this->subCamAt.y, GND_BOSSROOM_CENTER_Y + 197.0f, 0.05f,
                           this->subCamVelFactor * this->subCamAtVel.y);
            add_calc2(&this->subCamAt.z, GND_BOSSROOM_CENTER_Z - 65.0f, 0.05f,
                           this->subCamVelFactor * this->subCamAtVel.z);
            add_calc2(&this->subCamVelFactor, 0.01f, 1.0f, 0.001f);
            if ((this->timers[0] == 245) || (this->timers[0] == 3)) {
                Skeleton_Info2_init_standard_stop_morf(&this->skin.skelAnime, &gPhantomHorseRearingAnim, -8.0f);
                this->bossGndSignal = FHG_REAR;
                Actor_SE_set(&this->actor, NA_SE_EV_GANON_HORSE_NEIGH);
                if (this->timers[0] == 3) {
                    Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_VOICE);
                }
            }
            if (this->timers[0] == 192) {
                Actor_SE_set(&this->actor, NA_SE_EV_HORSE_SANDDUST);
            }
            if (this->timers[0] == 212) {
                Actor_SE_set(&this->actor, NA_SE_EV_HORSE_LAND2);
                Skeleton_Info2_init(&this->skin.skelAnime, &gPhantomHorseIdleAnim, 0.3f, 0.0f, 5.0f, ANIMMODE_LOOP_INTERP,
                                 -10.0f);
            }
            if (this->timers[0] == 90) {
                play->envCtx.lightSettingOverride = 2;
                play->envCtx.lightBlendRateOverride = 20;
            }
            if (this->timers[0] == 100) {
                this->bossGndSignal = FHG_LIGHTNING;
            }
            if (this->timers[0] == 60) {
                this->bossGndSignal = FHG_RIDE;
            }
            if (this->timers[0] == 130) {
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 80);
            }
            if (this->timers[0] == 30) {
                bossGnd->work[GND_EYE_STATE] = GND_EYESTATE_BRIGHTEN;
            }
            if (this->timers[0] == 35) {
                Na_StartObjectSe_F(&fhg_se_pos, NA_SE_EN_FANTOM_EYE);
            }
            if (this->timers[0] == 130) {
                bossGnd->work[GND_EYE_STATE] = GND_EYESTATE_FADE;
                Na_StartObjectSe_F(&fhg_se_pos, NA_SE_EN_FANTOM_ST_LAUGH);
            }
            if (this->timers[0] == 20) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS);
            }
            if (this->timers[0] == 2) {
                this->subCamVelFactor = 0.0f;
                this->cutsceneState = INTRO_TITLE;
                this->subCamEyeVel.x = fabsf(this->subCamEye.x - (GND_BOSSROOM_CENTER_X + 180.0f));
                this->subCamEyeVel.y = fabsf(this->subCamEye.y - (GND_BOSSROOM_CENTER_Y + 7.0f));
                this->subCamEyeVel.z = fabsf(this->subCamEye.z - (GND_BOSSROOM_CENTER_Z + 140.0f));
                this->timers[0] = 100;
                this->timers[1] = 34;
                this->subCamAtVel.x = fabsf(this->subCamAt.x - this->actor.world.pos.x);
                this->subCamAtVel.y = fabsf(this->subCamAt.y - ((this->actor.world.pos.y + 70.0f) - 20.0f));
                this->subCamAtVel.z = fabsf(this->subCamAt.z - this->actor.world.pos.z);
            }
            break;
        case INTRO_TITLE:
            if (this->timers[1] == 1) {
                Skeleton_Info2_init(&this->skin.skelAnime, &gPhantomHorseIdleAnim, 0.5f, 0.0f,
                                 Si2_anime_end_frame(&gPhantomHorseIdleAnim), ANIMMODE_LOOP_INTERP, -3.0f);
            }
            add_calc2(&this->subCamEye.x, GND_BOSSROOM_CENTER_X + 180.0f, 0.1f,
                           this->subCamVelFactor * this->subCamEyeVel.x);
            add_calc2(&this->subCamEye.y, GND_BOSSROOM_CENTER_Y + 7.0f, 0.1f,
                           this->subCamVelFactor * this->subCamEyeVel.y);
            add_calc2(&this->subCamEye.z, this->subCamPanZ + (GND_BOSSROOM_CENTER_Z + 140.0f), 0.1f,
                           this->subCamVelFactor * this->subCamEyeVel.z);
            add_calc2(&this->subCamPanZ, -100.0f, 0.1f, 1.0f);
            add_calc2(&this->subCamAt.x, this->actor.world.pos.x, 0.1f, this->subCamVelFactor * 10.0f);
            add_calc2(&this->subCamAt.y, (this->actor.world.pos.y + 70.0f) - 20.0f, 0.1f,
                           this->subCamVelFactor * 10.0f);
            add_calc2(&this->subCamAt.z, this->actor.world.pos.z, 0.1f, this->subCamVelFactor * 10.0f);
            add_calc2(&this->actor.world.pos.y, 60.0f, 0.1f, 2.0f);
            this->actor.world.pos.y += 2.0f * sin_s(this->gallopTimer * 0x5DC);
            add_calc2(&this->subCamVelFactor, 1.0f, 1.0f, 0.05f);
            if (this->timers[0] == 75) {
                Actor_Name_Disp_Set(play, &play->actorCtx.titleCtx, SEGMENTED_TO_VIRTUAL(gPhantomGanonTitleCardTex),
                                       160, 180, 128, 40);
            }
            if (this->timers[0] == 0) {
                this->cutsceneState = INTRO_RETREAT;
                this->timers[0] = 200;
                this->timers[1] = 23;
                this->subCamVelFactor = 0.0f;
                Skeleton_Info2_init(&this->skin.skelAnime, &gPhantomHorseLeapAnim, 1.0f, 0.0f,
                                 Si2_anime_end_frame(&gPhantomHorseLeapAnim), ANIMMODE_ONCE_INTERP, -4.0f);
                this->bossGndSignal = FHG_SPUR;
                Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_VOICE);
                Actor_SE_set(&this->actor, NA_SE_EV_GANON_HORSE_NEIGH);
            }
            break;
        case INTRO_RETREAT:
            if (this->timers[1] == 1) {
                Skeleton_Info2_init(&this->skin.skelAnime, &gPhantomHorseLandAnim, 0.5f, 0.0f,
                                 Si2_anime_end_frame(&gPhantomHorseLandAnim), ANIMMODE_ONCE_INTERP, -3.0f);
                this->bossGndSignal = FHG_FINISH;
            }
            if (this->timers[0] == 170) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
                Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_MASIC2);
            }
            add_calc2(&this->subCamEye.z, this->subCamPanZ + (GND_BOSSROOM_CENTER_Z + 100.0f), 0.1f,
                           this->subCamVelFactor * 1.5f);
            add_calc2(&this->subCamPanZ, -100.0f, 0.1f, 1.0f);
            add_calc2(&this->actor.world.pos.z, GND_BOSSROOM_CENTER_Z + 400.0f - 0.5f, 1.0f,
                           this->subCamVelFactor * 10.0f);
            add_calc2(&this->subCamVelFactor, 1.0f, 1.0f, 0.05f);
            if ((fabsf(this->actor.world.pos.z - (GND_BOSSROOM_CENTER_Z + 400.0f - 0.5f)) < 300.0f) &&
                !this->spawnedWarp) {
                this->spawnedWarp = true;
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, GND_BOSSROOM_CENTER_X + 0.0f,
                                   this->actor.world.pos.y + 50.0f, GND_BOSSROOM_CENTER_Z + 400.0f - 0.5f, 0,
                                   this->actor.shape.rot.y, 0, FHGFIRE_WARP_RETREAT);
                this->fhgFireKillWarp = true;
            }
            add_calc2(&this->subCamAt.x, this->actor.world.pos.x, 0.2f, 50.0f);
            add_calc2(&this->subCamAt.z, this->actor.world.pos.z, 0.2f, 50.0f);
            PRINTF("TIME %d-------------------------------------------------\n", this->timers[0]);
            if (fabsf(this->actor.world.pos.z - (GND_BOSSROOM_CENTER_Z + 400.0f - 0.5f)) < 1.0f) {
                play->envCtx.lightSettingOverride = 0;
                play->envCtx.lightBlendRateOverride = 20;
                this->cutsceneState = INTRO_FINISH;
                Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gPhantomHorseRunningAnim, -3.0f);
                this->bossGndSignal = FHG_START_FIGHT;
                this->timers[1] = 75;
                this->timers[0] = 140;
            }
            break;
        case INTRO_FINISH:
            mode_kabein(this, play);
            add_calc2(&this->subCamEye.z, this->subCamPanZ + (GND_BOSSROOM_CENTER_Z + 100.0f), 0.1f,
                           this->subCamVelFactor * 1.5f);
            add_calc2(&this->subCamPanZ, -100.0f, 0.1f, 1.0f);
            add_calc2(&this->subCamAt.y, (this->actor.world.pos.y + 70.0f) - 20.0f, 0.1f,
                           this->subCamVelFactor * 10.0f);
            if (this->timers[1] == 0) {
                Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                this->actionFunc = mode_kabein;
            }
            break;
    }
    if (this->subCamId != SUB_CAM_ID_DONE) {
        Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

void mode_kabeout_standby_init(EnfHG* this, PlayState* play, s16 paintingIndex) {
    s16 oppositeIndex[6] = { 3, 4, 5, 0, 1, 2 };

    Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gPhantomHorseRunningAnim, 0.0f);
    this->actionFunc = mode_kabeout_standby;
    this->curPainting = paintingIndex;
    this->targetPainting = oppositeIndex[this->curPainting];

    PRINTF("KABE NO 1 = %d\n", this->curPainting);
    PRINTF("KABE NO 2 = %d\n", this->targetPainting);

    this->actor.world.pos.x = (1.3f * kabe_position_data[this->curPainting].pos.x) + (GND_BOSSROOM_CENTER_X - 4.0f);
    this->actor.world.pos.y = kabe_position_data[this->curPainting].pos.y + (GND_BOSSROOM_CENTER_Y + 153.0f);
    this->actor.world.pos.z = (1.3f * kabe_position_data[this->curPainting].pos.z) - -(GND_BOSSROOM_CENTER_Z - 10.0f);
    this->actor.world.rot.y = kabe_position_data[this->curPainting].yRot;

    PRINTF("XP1  = %f\n", this->actor.world.pos.x);
    PRINTF("ZP1  = %f\n", this->actor.world.pos.z);

    this->inPaintingPos.x = (kabe_position_data[this->targetPainting].pos.x * 1.3f) + (GND_BOSSROOM_CENTER_X - 4.0f);
    this->inPaintingPos.y = kabe_position_data[this->targetPainting].pos.y + (GND_BOSSROOM_CENTER_Y + 33.0f);
    this->inPaintingPos.z = (kabe_position_data[this->targetPainting].pos.z * 1.3f) - -(GND_BOSSROOM_CENTER_Z - 10.0f);
    this->inPaintingVelX = (fabsf(this->inPaintingPos.x - this->actor.world.pos.x) * 2) * 0.01f;

    if (this->inPaintingVelX < 1.0f) {
        this->inPaintingVelX = 1.0f;
    }
    this->inPaintingVelZ = (fabsf(this->inPaintingPos.z - this->actor.world.pos.z) * 2) * 0.01f;
    if (this->inPaintingVelZ < 1.0f) {
        this->inPaintingVelZ = 1.0f;
    }

    this->timers[0] = 100;
    this->actor.scale.x = 0.002f;
    this->actor.scale.y = 0.002f;
    this->actor.scale.z = 0.001f;
    this->approachRate = 0.0f;

    this->warpColorFilterR = play->lightCtx.fogColor[0];
    this->warpColorFilterG = play->lightCtx.fogColor[1];
    this->warpColorFilterB = play->lightCtx.fogColor[2];
    this->warpColorFilterUnk1 = 0.0f;
    this->warpColorFilterUnk2 = 0.0f;
    this->turnRot = 0;
    this->turnTarget = 0;
    this->spawnedWarp = false;
}

void mode_kabeout_standby(EnfHG* this, PlayState* play) {
    PRINTF("STANDBY !!\n");
    PRINTF("XP2  = %f\n", this->actor.world.pos.x);
    PRINTF("ZP2  = %f\n", this->actor.world.pos.z);
    if (this->actor.params == GND_REAL_BOSS) {
        this->hoofSfxPos.x = this->actor.projectedPos.x / (this->actor.scale.x * 100.0f);
        this->hoofSfxPos.y = this->actor.projectedPos.y / (this->actor.scale.x * 100.0f);
        this->hoofSfxPos.z = this->actor.projectedPos.z / (this->actor.scale.x * 100.0f);
        if ((this->gallopTimer % 8) == 0) {
            Na_StartObjectSe_F(&this->hoofSfxPos, NA_SE_EV_HORSE_RUN);
        }
    }
    Skeleton_Info2_anime_play(&this->skin.skelAnime);
    add_calc2(&this->actor.scale.x, 0.011499999f, 1.0f, this->approachRate);
    add_calc2(&this->approachRate, 0.0002f, 1.0f, 0.0000015f);
    add_calc2(&this->actor.world.pos.y, 60.0f, 0.1f, 1.0f);
    this->actor.scale.y = this->actor.scale.x;
    if (this->timers[0] == 0) {
        PRINTF("arg_data ------------------------------------>%d\n", this->actor.params);
        if (this->actor.params != GND_REAL_BOSS) {
            this->timers[0] = 140;
            this->actionFunc = mode_kabein;
            Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gPhantomHorseRunningAnim, 0.0f);
            this->turnTarget = -0x8000;
        } else {
            this->actionFunc = mode_kabeout;
            Actor_SE_set(&this->actor, NA_SE_EV_GANON_HORSE_NEIGH);
            this->timers[0] = 40;
            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->actor.world.pos.x,
                               this->actor.world.pos.y + 50.0f, this->actor.world.pos.z, 0,
                               this->actor.shape.rot.y + 0x8000, 0, FHGFIRE_WARP_EMERGE);
            this->fhgFireKillWarp = false;
        }
    }
}

void mode_kabeout(EnfHG* this, PlayState* play) {
    PRINTF("KABE OUT !!\n");
    this->bossGndInPainting = false;
    Skeleton_Info2_anime_play(&this->skin.skelAnime);
    if (this->timers[0] != 0) {
        add_calc2(&this->actor.scale.z, 0.011499999f, 1.0f, 0.0002f);
        if (this->timers[0] == 1) {
            this->bossGndSignal = FHG_RAISE_SPEAR;
            this->timers[1] = 50;
            Skeleton_Info2_init_standard_stop_morf(&this->skin.skelAnime, &gPhantomHorseLeapAnim, 0.0f);
        }
        add_calc2(&this->warpColorFilterR, 255.0f, 1.0f, 10.0f);
        add_calc2(&this->warpColorFilterG, 255.0f, 1.0f, 10.0f);
        add_calc2(&this->warpColorFilterB, 255.0f, 1.0f, 10.0f);
        add_calc2(&this->warpColorFilterUnk1, -60.0f, 1.0f, 5.0f);
    } else {
        add_calc2(&this->warpColorFilterR, play->lightCtx.fogColor[0], 1.0f, 10.0f);
        add_calc2(&this->warpColorFilterG, play->lightCtx.fogColor[0], 1.0f, 10.0f);
        add_calc2(&this->warpColorFilterB, play->lightCtx.fogColor[0], 1.0f, 10.0f);
        add_calc2(&this->warpColorFilterUnk1, 0.0f, 1.0f, 5.0f);
        if (this->timers[1] == 29) {
            Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_MASIC2);
            Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_VOICE);
        }
        if (this->hitTimer == 0) {
            if (this->timers[1] == 24) {
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->actor.world.pos.x,
                                   (this->actor.world.pos.y + 100.0f) + 25.0f, this->actor.world.pos.z, 0, 0, 0,
                                   FHGFIRE_LIGHTNING_STRIKE);
            }
            if (this->timers[1] == 45) {
                Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gPhantomHorseAirAnim, 0.0f);
            }
            if (this->timers[1] == 38) {
                this->bossGndSignal = FHG_LIGHTNING;
            }
            if (this->timers[1] == 16) {
                Skeleton_Info2_init_standard_stop_morf(&this->skin.skelAnime, &gPhantomHorseLandAnim, 0.0f);
                this->bossGndSignal = FHG_RESET;
            }
        }
        add_calc2(&this->actor.scale.z, 0.011499999f, 1.0f, 0.002f);
        add_calc2(&this->actor.world.pos.x, this->inPaintingPos.x, 1.0f, this->inPaintingVelX);
        add_calc2(&this->actor.world.pos.y, 60.0f, 0.1f, 1.0f);
        add_calc2(&this->actor.world.pos.z, this->inPaintingPos.z, 1.0f, this->inPaintingVelZ);
    }
    if (this->hitTimer == 20) {
        this->actionFunc = mode_revise;
        this->spawnedWarp = false;
        Skeleton_Info2_init(&this->skin.skelAnime, &gPhantomHorseLandAnim, -1.0f, 0.0f,
                         Si2_anime_end_frame(&gPhantomHorseLandAnim), ANIMMODE_ONCE, -5.0f);
        this->timers[0] = 10;
        this->bossGndSignal = FHG_RESET;
        this->damageSpeedMod = 1.0f;
    } else {
        f32 dx = this->actor.world.pos.x - this->inPaintingPos.x;
        f32 dz = this->actor.world.pos.z - this->inPaintingPos.z;
        f32 dxz = sqrtf(SQ(dx) + SQ(dz));

        if (dxz < 350.0f) {
            this->bossGndInPainting = true;
        }
        if ((dxz < 300.0f) && !this->spawnedWarp) {
            this->spawnedWarp = true;
            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->inPaintingPos.x,
                               this->actor.world.pos.y + 50.0f, this->inPaintingPos.z, 0, this->actor.shape.rot.y, 0,
                               FHGFIRE_WARP_RETREAT);
            this->fhgFireKillWarp = true;
        }
        PRINTF("SPD X %f\n", this->inPaintingVelX);
        PRINTF("SPD Z %f\n", this->inPaintingVelZ);
        PRINTF("X=%f\n", dx);
        PRINTF("Z=%f\n", dz);
        if (dxz == 0.0f) {
            this->timers[0] = 140;
            this->actionFunc = mode_kabein;
            Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gPhantomHorseRunningAnim, 0.0f);
            this->bossGndSignal = FHG_RIDE;
        }
    }
}

void mode_revise(EnfHG* this, PlayState* play) {
    f32 dx;
    f32 dz;
    f32 dxz2;

    PRINTF("REVISE !!\n");
    Skeleton_Info2_anime_play(&this->skin.skelAnime);
    add_calc2(&this->warpColorFilterR, play->lightCtx.fogColor[0], 1.0f, 10.0f);
    add_calc2(&this->warpColorFilterG, play->lightCtx.fogColor[0], 1.0f, 10.0f);
    add_calc2(&this->warpColorFilterB, play->lightCtx.fogColor[0], 1.0f, 10.0f);
    add_calc2(&this->warpColorFilterUnk1, 0.0f, 1.0f, 5.0f);
    add_calc2(&this->actor.scale.z, 0.011499999f, 1.0f, 0.002f);
    if (this->timers[0] != 0) {
        add_calc0(&this->damageSpeedMod, 1.0f, 0.1f);
        if (this->timers[0] == 1) {
            this->targetPainting = this->curPainting;
            this->inPaintingPos.x = (kabe_position_data[this->targetPainting].pos.x * 1.3f) + (GND_BOSSROOM_CENTER_X - 4.0f);
            this->inPaintingPos.y = kabe_position_data[this->targetPainting].pos.y;
            this->inPaintingPos.z = (kabe_position_data[this->targetPainting].pos.z * 1.3f) - -(GND_BOSSROOM_CENTER_Z - 10.0f);
        }
    } else {
        add_calc2(&this->damageSpeedMod, 1.0f, 1.0f, 0.1f);
    }
    add_calc2(&this->actor.world.pos.x, this->inPaintingPos.x, 1.0f, this->damageSpeedMod * this->inPaintingVelX);
    add_calc2(&this->actor.world.pos.y, 60.0f, 0.1f, 1.0f);
    add_calc2(&this->actor.world.pos.z, this->inPaintingPos.z, 1.0f, this->damageSpeedMod * this->inPaintingVelZ);
    dx = this->actor.world.pos.x - this->inPaintingPos.x;
    dz = this->actor.world.pos.z - this->inPaintingPos.z;
    dxz2 = sqrtf(SQ(dx) + SQ(dz));
    if ((dxz2 < 300.0f) && (!this->spawnedWarp)) {
        this->spawnedWarp = true;
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->inPaintingPos.x,
                           this->actor.world.pos.y + 50.0f, this->inPaintingPos.z, 0, this->actor.shape.rot.y + 0x8000,
                           0, FHGFIRE_WARP_RETREAT);
    }
    if (dxz2 == 0.0f) {
        BossGanondrof* bossGnd = (BossGanondrof*)this->actor.parent;

        this->timers[0] = 140;
        this->actionFunc = mode_kabein;
        Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gPhantomHorseRunningAnim, 0.0f);
        if (bossGnd->actor.colChkInfo.health > 24) {
            this->bossGndSignal = FHG_RIDE;
        } else {
            bossGnd->flyMode = GND_FLY_NEUTRAL;
        }
        this->turnTarget = -0x8000;
    }
}

void mode_kabein(EnfHG* this, PlayState* play) {
    PRINTF("KABE IN !!\n");
    if (this->turnTarget != 0) {
        adds(&this->turnRot, this->turnTarget, 5, 2000);
    }
    if (this->actor.params == GND_REAL_BOSS) {
        this->hoofSfxPos.x = this->actor.projectedPos.x / (this->actor.scale.x * 100.0f);
        this->hoofSfxPos.y = this->actor.projectedPos.y / (this->actor.scale.x * 100.0f);
        this->hoofSfxPos.z = this->actor.projectedPos.z / (this->actor.scale.x * 100.0f);
        if ((this->gallopTimer % 8) == 0) {
            Na_StartObjectSe_F(&this->hoofSfxPos, NA_SE_EV_HORSE_RUN);
        }
    }
    Skeleton_Info2_anime_play(&this->skin.skelAnime);
    add_calc2(&this->actor.scale.z, 0.001f, 1.0f, 0.001f);
    add_calc2(&this->actor.scale.x, 0.002f, 0.05f, 0.0001f);
    add_calc2(&this->actor.world.pos.y, 200.0f, 0.05f, 1.0f);
    this->actor.scale.y = this->actor.scale.x;
    if ((this->timers[0] == 80) && (this->actor.params == GND_REAL_BOSS)) {
        Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_LAUGH);
    }
    if (this->timers[0] == 0) {
        BossGanondrof* bossGnd = (BossGanondrof*)this->actor.parent;
        s16 paintingIdxReal;
        s16 paintingIdxFake;
        UNUSED_NDEBUG Actor* child;

        if (this->actor.params != GND_REAL_BOSS) {
            this->killActor = true;
            bossGnd->killActor = true;
        } else if (bossGnd->flyMode != GND_FLY_PAINTING) {
            this->actionFunc = mode_ganon_wait;
            this->actor.draw = NULL;
        } else {
            paintingIdxReal = fqrand() * 5.99f;
            mode_kabeout_standby_init(this, play, paintingIdxReal);
            do {
                paintingIdxFake = fqrand() * 5.99f;
            } while (paintingIdxFake == paintingIdxReal);

            child = Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_GANONDROF,
                                       this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, 0, 0,
                                       0, paintingIdxFake + GND_FAKE_BOSS);
            PRINTF("ac1 = %x `````````````````````````````````````````````````\n", child);
        }
    }
}

void mode_ganon_wait(EnfHG* this, PlayState* play) {
    this->bossGndInPainting = false;
}

void En_fHG_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnfHG* this = (EnfHG*)thisx;
    u8 i;

    if (this->killActor) {
        Actor_delete(&this->actor);
        return;
    }
    this->gallopTimer++;
    this->bossGndInPainting = true;
    for (i = 0; i < 5; i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }

    this->actionFunc(this, play);

    if (this->hitTimer != 0) {
        this->hitTimer--;
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    this->actor.shape.rot.y = this->actor.world.rot.y;

    this->actor.shape.yOffset = sin_s(this->hitTimer * 0x9000) * 700.0f * (this->hitTimer / 20.0f);
    this->actor.shape.rot.z = (s16)(sin_s(this->hitTimer * 0x7000) * 1500.0f) * (this->hitTimer / 20.0f);
}

void En_fHG_calc(Actor* thisx, PlayState* play, Skin* skin) {
}

void En_fHG_display(Actor* thisx, PlayState* play) {
    EnfHG* this = (EnfHG*)thisx;
    BossGanondrof* bossGnd = (BossGanondrof*)this->actor.parent;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_fhg.c", 2439);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    POLY_OPA_DISP = ((bossGnd->work[GND_INVINC_TIMER] & 4) && (bossGnd->flyMode == GND_FLY_PAINTING))
                        ? gfx_set_fog_nosync(POLY_OPA_DISP, 255, 50, 0, 0, 900, 1099)
                        : gfx_set_fog_nosync(POLY_OPA_DISP, (u32)this->warpColorFilterR, (u32)this->warpColorFilterG,
                                     (u32)this->warpColorFilterB, 0, (s32)this->warpColorFilterUnk1 + 995,
                                     (s32)this->warpColorFilterUnk2 + 1000);
    Skin_disp2(&this->actor, play, &this->skin, En_fHG_calc, SKIN_TRANSFORM_IS_FHG);
    POLY_OPA_DISP = game_play_set_fog(play, POLY_OPA_DISP);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_fhg.c", 2480);
}
