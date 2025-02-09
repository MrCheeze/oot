/*
 * File: z_en_heishi3.c
 * Overlay: ovl_En_Heishi3
 * Description: Hyrule Castle Guard
 */

#include "z_en_heishi3.h"
#include "assets/objects/object_sd/object_sd.h"
#include "terminal.h"
#include "versions.h"

#define FLAGS 0

void En_Heishi3_actor_ct(Actor* thisx, PlayState* play);
void En_Heishi3_actor_dt(Actor* thisx, PlayState* play);
void En_Heishi3_actor_move(Actor* thisx, PlayState* play);
void En_Heishi3_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait_init(EnHeishi3* this, PlayState* play);
static void mode_wait(EnHeishi3* this, PlayState* play);
void mode_wait_special(EnHeishi3* this, PlayState* play);
static void mode_catch_init(EnHeishi3* this, PlayState* play);
static void mode_retry_init(EnHeishi3* this, PlayState* play);
static void mode_retry(EnHeishi3* this, PlayState* play);
static void mode_catch(EnHeishi3* this, PlayState* play);

static s16 look_at_me = 0;

ActorProfile En_Heishi3_Profile = {
    /**/ ACTOR_EN_HEISHI3,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SD,
    /**/ sizeof(EnHeishi3),
    /**/ En_Heishi3_actor_ct,
    /**/ En_Heishi3_actor_dt,
    /**/ En_Heishi3_actor_move,
    /**/ En_Heishi3_actor_draw,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 15, 70, 0, { 0, 0, 0 } },
};

void En_Heishi3_actor_ct(Actor* thisx, PlayState* play) {
    EnHeishi3* this = (EnHeishi3*)thisx;

    look_at_me = 0;
    if (this->actor.params <= 0) {
        this->unk_278 = 0;
    } else {
        this->unk_278 = 1;
        if (this->actor.world.pos.x < -290.0f) {
            this->unk_278 = 2;
        }
    }
    Actor_set_scale(&this->actor, 0.01f);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gEnHeishiSkel, &gEnHeishiIdleAnim, this->jointTable, this->morphTable, 17);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
    // "Castle Gate Soldier - Power Up"
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 城門兵パワーアップ ☆☆☆☆☆ \n" VT_RST);

    this->actor.gravity = -3.0f;
    this->actor.focus.pos = this->actor.world.pos;
    this->actionFunc = mode_wait_init;
}

void En_Heishi3_actor_dt(Actor* thisx, PlayState* play) {
    EnHeishi3* this = (EnHeishi3*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void mode_wait_init(EnHeishi3* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gEnHeishiIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    if (this->unk_278 == 0) {
        this->actionFunc = mode_wait;
    } else {
        this->actionFunc = mode_wait_special;
    }
}

/**
 * Handles the guards standing on Hyrule Castle Grounds.
 **/
static void mode_wait(EnHeishi3* this, PlayState* play) {
    Player* player;
    s16 yawDiff;
    s16 yawDiffNew;
    f32 sightRange;

    player = GET_PLAYER(play);
    Skeleton_Info2_anime_play(&this->skelAnime);
    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    yawDiffNew = ABS(yawDiff);
    if (yawDiffNew < 0x4300) {
        if (IS_DAY) {
            sightRange = 250.0f;
        } else {
            sightRange = 200.0f;
        }
    } else {
        if (IS_DAY) {
            sightRange = 150.0f;
        } else {
            sightRange = 100.0f;
        }
    }
    if ((this->actor.xzDistToPlayer < sightRange) &&
        (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 100.0f) && (look_at_me == 0)) {
        look_at_me = 1;
        message_set(play, 0x702D, &this->actor);
        Na_StartSystemSe_F(NA_SE_SY_FOUND);
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 発見！ ☆☆☆☆☆ \n" VT_RST); // "Discovered!"
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
#if OOT_PAL_N64
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED;
#endif
        this->actionFunc = mode_catch_init;
    }
}

/**
 * Handles the guards standing in front of Hyrule Castle.
 **/
void mode_wait_special(EnHeishi3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((player->actor.world.pos.x < -190.0f) && (player->actor.world.pos.x > -380.0f) &&
        (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 100.0f) &&
        (player->actor.world.pos.z < 1020.0f) && (player->actor.world.pos.z > 700.0f) && (look_at_me == 0)) {
        if (this->unk_278 == 1) {
            if (player->actor.world.pos.x < -290.0f) {
                return;
            }
        } else {
            if (player->actor.world.pos.x > -290.0f) {
                return;
            }
        }
        look_at_me = 1;
        message_set(play, 0x702D, &this->actor);
        Na_StartSystemSe_F(NA_SE_SY_FOUND);
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 発見！ ☆☆☆☆☆ \n" VT_RST); // "Discovered!"
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
#if OOT_PAL_N64
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED;
#endif
        this->actionFunc = mode_catch_init;
    }
}

static void mode_catch_init(EnHeishi3* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gEnHeishiWalkAnim);

    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiWalkAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->caughtTimer = 20;
    this->actionFunc = mode_catch;
    this->actor.speed = 2.5f;
}

static void mode_catch(EnHeishi3* this, PlayState* play) {

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 1.0f) || Skeleton_Info_frame_check(&this->skelAnime, 17.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EV_KNIGHT_WALK);
    }
    if (this->caughtTimer == 0) {
        this->actionFunc = mode_retry_init;
        this->actor.speed = 0.0f;
    } else {
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 5, 3000, 0);
    }
}

static void mode_retry_init(EnHeishi3* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gEnHeishiIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_retry;
}

// This function initiates the respawn after the player gets caught.
static void mode_retry(EnHeishi3* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play) &&
        (this->respawnFlag == 0)) {
        SET_EVENTCHKINF(EVENTCHKINF_CAUGHT_BY_CASTLE_GUARDS);
        play->nextEntranceIndex = ENTR_HYRULE_CASTLE_4;
        play->transitionTrigger = TRANS_TRIGGER_START;
        this->respawnFlag = 1;
        play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
        z_common_data.nextTransitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
    }
}

void En_Heishi3_actor_move(Actor* thisx, PlayState* play) {
    EnHeishi3* this = (EnHeishi3*)thisx;
    s32 pad;

    Actor_world_to_eye(&this->actor, 60.0f);
    this->unk_274++;
    if (this->caughtTimer != 0) {
        this->caughtTimer--;
    }
    this->actionFunc(this, play);
    this->actor.shape.rot = this->actor.world.rot;
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

s32 En_Heishi3_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHeishi3* this = (EnHeishi3*)thisx;

    if (limbIndex == 9) {
        rot->x += this->unk_26E;
    }

    if (limbIndex == 16) {
        rot->x += this->unk_262;
        rot->z += this->unk_264;
    }

    return false;
}

void En_Heishi3_actor_draw(Actor* thisx, PlayState* play) {
    EnHeishi3* this = (EnHeishi3*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, En_Heishi3_draw_sub, NULL,
                      this);
}
