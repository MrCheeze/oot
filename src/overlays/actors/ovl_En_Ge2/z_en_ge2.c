/*
 * File: z_en_ge2.c
 * Overlay: ovl_En_Ge2
 * Description: Purple-clothed Gerudo
 */

#include "z_en_ge2.h"
#include "z64horse.h"
#include "terminal.h"
#include "assets/objects/object_gla/object_gla.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define GE2_STATE_ANIMCOMPLETE (1 << 1)
#define GE2_STATE_KO (1 << 2)
#define GE2_STATE_CAPTURING (1 << 3)
#define GE2_STATE_TALKED (1 << 4)

typedef enum EnGe2Type {
    /* 0 */ GE2_TYPE_PATROLLING,
    /* 1 */ GE2_TYPE_STATIONARY,
    /* 2 */ GE2_TYPE_GERUDO_CARD_GIVER
} EnGe2Type;

typedef enum EnGe2Action {
    /* 0 */ GE2_ACTION_WALK,
    /* 1 */ GE2_ACTION_ABOUTTURN,
    /* 2 */ GE2_ACTION_TURNPLAYERSPOTTED,
    /* 3 */ GE2_ACTION_KNOCKEDOUT,
    /* 4 */ GE2_ACTION_CAPTURETURN,
    /* 5 */ GE2_ACTION_CAPTURECHARGE,
    /* 6 */ GE2_ACTION_CAPTURECLOSE,
    /* 7 */ GE2_ACTION_STAND,
    /* 8 */ GE2_ACTION_WAITLOOKATPLAYER
} EnGe2Action;

void En_Ge2_Actor_ct(Actor* thisx, PlayState* play);
void En_Ge2_Actor_dt(Actor* thisx, PlayState* play);
void En_Ge2_Actor_move(Actor* thisx, PlayState* play);
void En_Ge2_Actor_draw(Actor* thisx, PlayState* play);

s32 ge2_event_check_nakanaori(void);
void catch_player_near(EnGe2* this, PlayState* play);
void catch_player_walk(EnGe2* this, PlayState* play);
void catch_player_turn(EnGe2* this, PlayState* play);
static void move_down(EnGe2* this, PlayState* play);
void move_search(EnGe2* this, PlayState* play);
static void move_turn(EnGe2* this, PlayState* play);
void move_walk(EnGe2* this, PlayState* play);
void stand_search(EnGe2* this, PlayState* play);
static void move_frend(EnGe2* this, PlayState* play);
static void move_give(EnGe2* this, PlayState* play);

// Update functions
void En_Ge2_Actor_move_Frend(Actor* thisx, PlayState* play);
void En_Ge2_Actor_move_Talk(Actor* thisx, PlayState* play);
void En_Ge2_Actor_move_Stop(Actor* thisx, PlayState* play2);

ActorProfile En_Ge2_Profile = {
    /**/ ACTOR_EN_GE2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GLA,
    /**/ sizeof(EnGe2),
    /**/ En_Ge2_Actor_ct,
    /**/ En_Ge2_Actor_dt,
    /**/ En_Ge2_Actor_move,
    /**/ En_Ge2_Actor_draw,
};

static ColliderCylinderInit EnGe2OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x000007A2, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 60, 0, { 0, 0, 0 } },
};

static EnGe2ActionFunc prc_tbl[] = {
    move_walk,         move_turn,   move_search,
    move_down,   catch_player_turn, catch_player_walk,
    catch_player_near, stand_search,       move_frend,
};

static AnimationHeader* soft_anime_tbl[] = {
    &gGerudoPurpleWalkingAnim,         &gGerudoPurpleLookingAboutAnim, &gGerudoPurpleLookingAboutAnim,
    &gGerudoPurpleFallingToGroundAnim, &gGerudoPurpleLookingAboutAnim, &gGerudoPurpleChargingAnim,
    &gGerudoPurpleLookingAboutAnim,    &gGerudoPurpleLookingAboutAnim, &gGerudoPurpleLookingAboutAnim,
};

static u8 anime_frame_index[] = {
    ANIMMODE_LOOP, ANIMMODE_ONCE, ANIMMODE_LOOP, ANIMMODE_ONCE, ANIMMODE_LOOP,
    ANIMMODE_LOOP, ANIMMODE_LOOP, ANIMMODE_LOOP, ANIMMODE_ONCE,
};

void En_Ge2_actor_set_process(EnGe2* this, s32 i) {
    this->actionFunc = prc_tbl[i];
    Skeleton_Info2_init(&this->skelAnime, soft_anime_tbl[i], 1.0f, 0.0f, Si2_anime_end_frame(soft_anime_tbl[i]),
                     anime_frame_index[i], -8.0f);
    this->stateFlags &= ~GE2_STATE_ANIMCOMPLETE;
}

void En_Ge2_Actor_ct(Actor* thisx, PlayState* play) {
    EnGe2* this = (EnGe2*)thisx;
    s16 params = this->actor.params;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 36.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGerudoPurpleSkel, NULL, this->jointTable, this->morphTable, 22);
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gGerudoPurpleWalkingAnim);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnGe2OcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_set_scale(&this->actor, 0.01f);

    if (play->sceneId == SCENE_GERUDO_VALLEY) {
        this->actor.cullingVolumeDistance = 1000.0f;
    } else {
        this->actor.cullingVolumeDistance = 1200.0f;
    }

    this->yDetectRange = (this->actor.world.rot.z + 1) * 40.0f;
    this->actor.world.rot.z = 0;
    this->actor.shape.rot.z = 0;

    switch (PARAMS_GET_S(thisx->params, 0, 8)) {
        case GE2_TYPE_PATROLLING:
            En_Ge2_actor_set_process(this, GE2_ACTION_WALK);
            if (ge2_event_check_nakanaori()) {
                this->actor.update = En_Ge2_Actor_move_Frend;
                this->actor.attentionRangeType = ATTENTION_RANGE_6;
            }
            break;
        case GE2_TYPE_STATIONARY:
            En_Ge2_actor_set_process(this, GE2_ACTION_STAND);
            if (ge2_event_check_nakanaori()) {
                this->actor.update = En_Ge2_Actor_move_Frend;
                this->actor.attentionRangeType = ATTENTION_RANGE_6;
            }
            break;
        case GE2_TYPE_GERUDO_CARD_GIVER:
            En_Ge2_actor_set_process(this, GE2_ACTION_WAITLOOKATPLAYER);
            this->actor.update = En_Ge2_Actor_move_Talk;
            this->actionFunc = move_give;
            this->actor.attentionRangeType = ATTENTION_RANGE_6;
            break;
        default:
            ASSERT(0, "0", "../z_en_ge2.c", 418);
            break;
    }

    this->stateFlags = 0;
    this->unk_304 = 0; // Set and not used
    this->walkTimer = 0;
    this->playerSpottedParam = 0;
    this->actor.minVelocityY = -4.0f;
    this->actor.gravity = -1.0f;
    this->walkDirection = this->actor.world.rot.y;
    this->walkDuration = PARAMS_GET_S(thisx->params, 8, 8) * 10;
}

void En_Ge2_Actor_dt(Actor* thisx, PlayState* play) {
    EnGe2* this = (EnGe2*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

// Detection/check functions

s32 turn_link_ge2(PlayState* play, EnGe2* this) {
    f32 visionScale;

    visionScale = (!IS_DAY ? 0.75f : 1.5f);

    if ((250.0f * visionScale) < this->actor.xzDistToPlayer) {
        return 0;
    }

    if (this->actor.xzDistToPlayer < 50.0f) {
        return 2;
    }

    if (player_big_SE_action_check(play)) {
        return 1;
    }
    return 0;
}

s32 search_link_ge2(PlayState* play, EnGe2* this, Vec3f* pos, s16 yRot, f32 yDetectRange) {
    Player* player = GET_PLAYER(play);
    Vec3f posResult;
    CollisionPoly* outPoly;
    f32 visionScale;

    visionScale = (!IS_DAY ? 0.75f : 1.5f);

    if ((250.0f * visionScale) < this->actor.xzDistToPlayer) {
        return 0;
    }

    if (yDetectRange < ABS(this->actor.yDistToPlayer)) {
        return 0;
    }

    if (ABS((s16)(this->actor.yawTowardsPlayer - yRot)) > 0x2000) {
        return 0;
    }

    if (T_BGCheck_LineCheck_poly(&play->colCtx, pos, &player->bodyPartsPos[PLAYER_BODYPART_HEAD], &posResult, &outPoly,
                             0)) {
        return 0;
    }
    return 1;
}

s32 ge2_event_check_nakanaori(void) {
    if (GET_EVENTCHKINF_CARPENTERS_ALL_RESCUED2()) {
        return true;
    }
    return false;
}

// Actions

void catch_player_near(EnGe2* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else {
        Horse_goto_Default(play);

        if ((INV_CONTENT(ITEM_HOOKSHOT) == ITEM_NONE) || (INV_CONTENT(ITEM_LONGSHOT) == ITEM_NONE)) {
            play->nextEntranceIndex = ENTR_GERUDO_VALLEY_1;
        } else if (GET_EVENTCHKINF(EVENTCHKINF_C7)) {
            play->nextEntranceIndex = ENTR_GERUDOS_FORTRESS_18;
        } else {
            play->nextEntranceIndex = ENTR_GERUDOS_FORTRESS_17;
        }

        play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST);
        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

void catch_player_walk(EnGe2* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 2, 0x400, 0x100);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (this->actor.xzDistToPlayer < 50.0f) {
        En_Ge2_actor_set_process(this, GE2_ACTION_CAPTURECLOSE);
        this->actor.speed = 0.0f;
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        Horse_goto_Default(play);

        if ((INV_CONTENT(ITEM_HOOKSHOT) == ITEM_NONE) || (INV_CONTENT(ITEM_LONGSHOT) == ITEM_NONE)) {
            play->nextEntranceIndex = ENTR_GERUDO_VALLEY_1;
        } else if (GET_EVENTCHKINF(EVENTCHKINF_C7)) {
            play->nextEntranceIndex = ENTR_GERUDOS_FORTRESS_18;
        } else {
            play->nextEntranceIndex = ENTR_GERUDOS_FORTRESS_17;
        }

        play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST);
        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

void catch_player_turn(EnGe2* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 2, 0x400, 0x100);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->actor.world.rot.y == this->actor.yawTowardsPlayer) {
        En_Ge2_actor_set_process(this, GE2_ACTION_CAPTURECHARGE);
        this->timer = 50;
        this->actor.speed = 4.0f;
    }
}

static void move_down(EnGe2* this, PlayState* play) {
    static Vec3f kirakira_vec = { 0.0f, -0.05f, 0.0f };
    static Vec3f kirakira_acc = { 0.0f, -0.025f, 0.0f };
    static Color_RGBA8 kirakira_prim = { 255, 255, 255, 0 };
    static Color_RGBA8 kirakira_env = { 255, 150, 0, 0 };
    s32 effectAngle;
    Vec3f effectPos;

    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (this->stateFlags & GE2_STATE_ANIMCOMPLETE) {
        effectAngle = (play->state.frames) * 0x2800;
        effectPos.x = this->actor.focus.pos.x + (cos_s(effectAngle) * 5.0f);
        effectPos.y = this->actor.focus.pos.y + 10.0f;
        effectPos.z = this->actor.focus.pos.z + (sin_s(effectAngle) * 5.0f);
        Effect_SS_KiraKira_sc_ct_ct(play, &effectPos, &kirakira_vec, &kirakira_acc, &kirakira_prim,
                                        &kirakira_env, 1000, 16);
    }
}

void move_search(EnGe2* this, PlayState* play) {
    s32 playerSpotted;

    this->actor.speed = 0.0f;

    if (this->stateFlags & GE2_STATE_TALKED) {
        this->stateFlags &= ~GE2_STATE_TALKED;
    } else {
        playerSpotted = turn_link_ge2(play, this);

        if (playerSpotted != 0) {
            this->timer = 100;
            this->yawTowardsPlayer = this->actor.yawTowardsPlayer;

            if (this->playerSpottedParam < playerSpotted) {
                this->playerSpottedParam = playerSpotted;
            }
        } else if (this->actor.world.rot.y == this->yawTowardsPlayer) {
            this->playerSpottedParam = 0;
            En_Ge2_actor_set_process(this, GE2_ACTION_ABOUTTURN);
            return;
        }
    }

    switch (this->playerSpottedParam) {
        case 1:
            add_calc_short_angle2(&this->actor.world.rot.y, this->yawTowardsPlayer, 2, 0x200, 0x100);
            break;
        case 2:
            add_calc_short_angle2(&this->actor.world.rot.y, this->yawTowardsPlayer, 2, 0x600, 0x180);
            break;
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
}

static void move_turn(EnGe2* this, PlayState* play) {
    s32 playerSpotted;

    this->actor.speed = 0.0f;
    playerSpotted = turn_link_ge2(play, this);

    if (playerSpotted != 0) {
        En_Ge2_actor_set_process(this, GE2_ACTION_TURNPLAYERSPOTTED);
        this->timer = 100;
        this->playerSpottedParam = playerSpotted;
        this->yawTowardsPlayer = this->actor.yawTowardsPlayer;
    } else if (this->stateFlags & GE2_STATE_ANIMCOMPLETE) {
        add_calc_short_angle2(&this->actor.world.rot.y, this->walkDirection, 2, 0x400, 0x200);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if (this->actor.shape.rot.y == this->walkDirection) {
        En_Ge2_actor_set_process(this, GE2_ACTION_WALK);
    }
}

void move_walk(EnGe2* this, PlayState* play) {
    u8 playerSpotted;

    playerSpotted = turn_link_ge2(play, this);
    if (playerSpotted != 0) {
        this->actor.speed = 0.0f;
        En_Ge2_actor_set_process(this, GE2_ACTION_TURNPLAYERSPOTTED);
        this->timer = 100;
        this->playerSpottedParam = playerSpotted;
        this->yawTowardsPlayer = this->actor.yawTowardsPlayer;
    } else if (this->walkTimer >= this->walkDuration) {
        this->walkTimer = 0;
        this->walkDirection += 0x8000;
        En_Ge2_actor_set_process(this, GE2_ACTION_ABOUTTURN);
        this->actor.speed = 0.0f;
    } else {
        this->walkTimer++;
        this->actor.speed = 2.0f;
    }
}

void stand_search(EnGe2* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.world.rot.y, this->walkDirection, 2, 0x400, 0x200);
}

void ge2_search_furimuki(EnGe2* this, PlayState* play) {
    s32 pad;
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(angleDiff) <= 0x4000) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, 4000, 100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        eye_move2(play, &this->actor, &this->headRot, &this->unk_2EE, this->actor.focus.pos);
    } else {
        if (angleDiff < 0) {
            add_calc_short_angle2(&this->headRot.y, -0x2000, 6, 6200, 0x100);
        } else {
            add_calc_short_angle2(&this->headRot.y, 0x2000, 6, 6200, 0x100);
        }

        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 12, 1000, 100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void ge2_search_normal(EnGe2* this, PlayState* play) {
    if ((ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4300) &&
        (this->actor.xzDistToPlayer < 200.0f)) {
        eye_move2(play, &this->actor, &this->headRot, &this->unk_2EE, this->actor.focus.pos);
    } else {
        add_calc_short_angle2(&this->headRot.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->headRot.y, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_2EE.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_2EE.y, 0, 6, 6200, 100);
    }
}

static void talk_frend(EnGe2* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {

        switch (PARAMS_GET_S(this->actor.params, 0, 8)) {
            case GE2_TYPE_PATROLLING:
                En_Ge2_actor_set_process(this, GE2_ACTION_ABOUTTURN);
                break;
            case GE2_TYPE_STATIONARY:
                En_Ge2_actor_set_process(this, GE2_ACTION_STAND);
                break;
            case GE2_TYPE_GERUDO_CARD_GIVER:
                this->actionFunc = move_frend;
                break;
        }
        this->actor.update = En_Ge2_Actor_move_Frend;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
    ge2_search_furimuki(this, play);
}

static void move_frend(EnGe2* this, PlayState* play) {
    ge2_search_normal(this, play);
}

static void move_give3(EnGe2* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = talk_frend;
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_GERUDOS_CARD, 10000.0f, 50.0f);
    }
}

static void move_give2(EnGe2* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = move_give3;
        Actor_carry_request_set2(&this->actor, play, GI_GERUDOS_CARD, 10000.0f, 50.0f);
    }
}

static void move_give(EnGe2* this, PlayState* play) {

    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = move_give2;
    } else {
        this->actor.textId = 0x6004;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_talk_request_get_item_set(&this->actor, play, 300.0f, 300.0f, EXCH_ITEM_NONE);
    }
    ge2_search_normal(this, play);
}

static void tocatch(EnGe2* this, PlayState* play) {
    this->stateFlags |= GE2_STATE_CAPTURING;
    this->actor.speed = 0.0f;
    En_Ge2_actor_set_process(this, GE2_ACTION_CAPTURETURN);
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_95);
    Na_StartSystemSe_F(NA_SE_SY_FOUND);
    message_set(play, 0x6000, &this->actor);
}

static void before_process(EnGe2* this, PlayState* play) {
    s32 pad;
    s32 pad2;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_BGcheck2(play, &this->actor, 40.0f, 25.0f, 40.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);

    if (!(this->stateFlags & GE2_STATE_ANIMCOMPLETE) && Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->stateFlags |= GE2_STATE_ANIMCOMPLETE;
    }
}

static void after_process(EnGe2* this, PlayState* play) {
    Actor_position_moveF(&this->actor);

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = get_random_timer(60, 60);
    }
    this->eyeIndex = this->blinkTimer;

    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

// Update functions

void En_Ge2_Actor_move_Frend(Actor* thisx, PlayState* play) {
    EnGe2* this = (EnGe2*)thisx;

    before_process(this, play);
    this->actionFunc(this, play);

    if (Actor_talk_check(&this->actor, play)) {
        if (PARAMS_GET_S(this->actor.params, 0, 8) == GE2_TYPE_PATROLLING) {
            this->actor.speed = 0.0f;
            En_Ge2_actor_set_process(this, GE2_ACTION_WAITLOOKATPLAYER);
        }
        this->actionFunc = talk_frend;
        this->actor.update = En_Ge2_Actor_move_Talk;
    } else {
        this->actor.textId = 0x6005;

        if (this->actor.xzDistToPlayer < 100.0f) {
            Actor_talk_request2(&this->actor, play, 100.0f);
        }
    }
    after_process(this, play);
}

void En_Ge2_Actor_move_Talk(Actor* thisx, PlayState* play) {
    EnGe2* this = (EnGe2*)thisx;

    this->stateFlags |= GE2_STATE_TALKED;
    before_process(this, play);
    this->actionFunc(this, play);
    after_process(this, play);
}

void En_Ge2_Actor_move(Actor* thisx, PlayState* play) {
    EnGe2* this = (EnGe2*)thisx;
    s32 paramsType;

    before_process(this, play);

    if ((this->stateFlags & GE2_STATE_KO) || (this->stateFlags & GE2_STATE_CAPTURING)) {
        this->actionFunc(this, play);
    } else if (this->collider.base.acFlags & AC_HIT) {
        if ((this->collider.elem.acHitElem != NULL) &&
            (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & DMG_HOOKSHOT)) {
            //! @bug duration parameter is larger than 255 which messes with the internal bitpacking of the colorfilter.
            //! Because of the duration being tracked as an unsigned byte it ends up being truncated to 144
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 400);
            this->actor.update = En_Ge2_Actor_move_Stop;
            return;
        }

        En_Ge2_actor_set_process(this, GE2_ACTION_KNOCKEDOUT);
        this->timer = 100;
        this->stateFlags |= GE2_STATE_KO;
        this->actor.speed = 0.0f;
        Actor_SE_set(&this->actor, NA_SE_VO_SK_CRASH);
    } else {
        this->actionFunc(this, play);

        if (search_link_ge2(play, this, &this->actor.focus.pos, this->actor.shape.rot.y, this->yDetectRange)) {
            // "Discovered!"
            PRINTF(VT_FGCOL(GREEN) "発見!!!!!!!!!!!!\n" VT_RST);
            tocatch(this, play);
        }

        if ((PARAMS_GET_S(this->actor.params, 0, 8) == GE2_TYPE_STATIONARY) && (this->actor.xzDistToPlayer < 100.0f)) {
            // "Discovered!"
            PRINTF(VT_FGCOL(GREEN) "発見!!!!!!!!!!!!\n" VT_RST);
            tocatch(this, play);
        }
    }

    if (!(this->stateFlags & GE2_STATE_KO)) {
        paramsType = PARAMS_GET_S(this->actor.params, 0, 8); // Not necessary, but looks a bit nicer
        if ((paramsType == GE2_TYPE_PATROLLING) || (paramsType == GE2_TYPE_STATIONARY)) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
    after_process(this, play);

    if (ge2_event_check_nakanaori() && !(this->stateFlags & GE2_STATE_KO)) {
        this->actor.update = En_Ge2_Actor_move_Frend;
        this->actor.attentionRangeType = ATTENTION_RANGE_6;
    }
}

void En_Ge2_Actor_move_Stop(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnGe2* this = (EnGe2*)thisx;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_BGcheck2(play, &this->actor, 40.0f, 25.0f, 40.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);

    if ((this->collider.base.acFlags & AC_HIT) &&
        ((this->collider.elem.acHitElem == NULL) ||
         !(this->collider.elem.acHitElem->atDmgInfo.dmgFlags & DMG_HOOKSHOT))) {
        this->actor.colorFilterTimer = 0;
        En_Ge2_actor_set_process(this, GE2_ACTION_KNOCKEDOUT);
        this->timer = 100;
        this->stateFlags |= GE2_STATE_KO;
        this->actor.speed = 0.0f;
        Actor_SE_set(&this->actor, NA_SE_VO_SK_CRASH);
    }
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);

    if (ge2_event_check_nakanaori()) {
        this->actor.update = En_Ge2_Actor_move_Frend;
        this->actor.attentionRangeType = ATTENTION_RANGE_6;
        this->actor.colorFilterTimer = 0;
    } else if (this->actor.colorFilterTimer == 0) {
        this->actor.update = En_Ge2_Actor_move;
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGe2* this = (EnGe2*)thisx;

    if (limbIndex == 3) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
    }
    return 0;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 600.0f, 700.0f, 0.0f };
    EnGe2* this = (EnGe2*)thisx;

    if (limbIndex == 6) {
        Matrix_Position(&pos, &this->actor.focus.pos);
    }
}

void En_Ge2_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = { gGerudoPurpleEyeOpenTex, gGerudoPurpleEyeHalfTex, gGerudoPurpleEyeClosedTex };
    s32 pad;
    EnGe2* this = (EnGe2*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ge2.c", 1274);

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeIndex]));
    Actor_HiliteReflect_set_init(&this->actor, play, 0);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ge2.c", 1291);
}
