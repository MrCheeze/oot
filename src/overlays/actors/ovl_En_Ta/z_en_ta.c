/*
 * File: z_en_ta.c
 * Overlay: ovl_En_Ta
 * Description: Talon
 */

#include "z_en_ta.h"
#include "terminal.h"
#include "assets/objects/object_ta/object_ta.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

#define TALON_STATE_FLAG_TRACKING_PLAYER (1 << 0)
#define TALON_STATE_FLAG_GIVING_MILK_REFILL (1 << 1)
#define TALON_STATE_FLAG_SUPPRESS_BLINK (1 << 2)
// This has no effect, see before_display for details
#define TALON_STATE_FLAG_SUPPRESS_ROCKING_ANIM (1 << 3)
#define TALON_STATE_FLAG_ANIMATION_FINISHED (1 << 4)
#define TALON_STATE_FLAG_CUCCO_GAME_START_EVENT_TRIGGERED (1 << 5)
#define TALON_STATE_FLAG_FLOOR_CAMERA_ACTIVE (1 << 7)
#define TALON_STATE_FLAG_RAISING_HANDS (1 << 8)
#define TALON_STATE_FLAG_RESTORE_BGM_ON_DESTROY (1 << 9)

typedef enum TalonEyeIndex {
    /* 0 */ TALON_EYE_INDEX_OPEN,
    /* 1 */ TALON_EYE_INDEX_HALF,
    /* 2 */ TALON_EYE_INDEX_CLOSED,
    /* 3 */ TALON_EYE_INDEX_MAX
} TalonEyeIndex;

typedef enum TalonCanBuyMilkResult {
    /* 0 */ TALON_CANBUYMILK_NOT_ENOUGH_RUPEES,
    /* 1 */ TALON_CANBUYMILK_NO_EMPTY_BOTTLE,
    /* 2 */ TALON_CANBUYMILK_SUCCESS
} TalonCanBuyMilkResult;

void En_Ta_Actor_ct(Actor* thisx, PlayState* play2);
void En_Ta_Actor_dt(Actor* thisx, PlayState* play);
void En_Ta_Actor_move(Actor* thisx, PlayState* play);
void En_Ta_Actor_draw(Actor* thisx, PlayState* play);

void neteru0(EnTa* this, PlayState* play);
void neteru1(EnTa* this, PlayState* play);
void neteru2(EnTa* this, PlayState* play);
static void matsu(EnTa* this, PlayState* play);
static void matsu2(EnTa* this, PlayState* play);
void matsu3(EnTa* this, PlayState* play);
void ta_game_play(EnTa* this, PlayState* play);
void hajimete(EnTa* this, PlayState* play);
void kessan(EnTa* this, PlayState* play);

static void Ossan_glare(EnTa* this);
static void Ossan_wink(EnTa* this);

static void normal_anime_proc(EnTa* this);
void sleep_anime_proc(EnTa* this);
void inemuri_anime_proc(EnTa* this);
static void stop_anime_proc(EnTa* this);

ActorProfile En_Ta_Profile = {
    /**/ ACTOR_EN_TA,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TA,
    /**/ sizeof(EnTa),
    /**/ En_Ta_Actor_ct,
    /**/ En_Ta_Actor_dt,
    /**/ En_Ta_Actor_move,
    /**/ En_Ta_Actor_draw,
};

static ColliderCylinderInit EnTaOcInfoData = {
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
        { 0x00000004, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0, 0, 0 } },
};

void En_Ta_actor_set_process(EnTa* this, EnTaActionFunc actionFunc, EnTaAnimFunc animFunc) {
    this->actionFunc = actionFunc;
    this->animFunc = animFunc;
}

void set_hajimete_messaeg(EnTa* this, PlayState* play) {
    u16 maskReactionTextId = get_mask_message(play, MASK_REACTION_SET_TALON);

    // Check if cucco game was just finished
    if (GET_EVENTINF(EVENTINF_CUCCO_GAME_FINISHED)) {
        // Check if the game was won
        if (GET_EVENTINF(EVENTINF_CUCCO_GAME_WON)) {
            if (GET_ITEMGETINF(ITEMGETINF_TALON_BOTTLE)) {
                // Game won already before
                this->actor.textId = 0x2088;
            } else {
                // First time winning
                this->actor.textId = 0x2086;
            }
        } else {
            // Try again
            this->actor.textId = 0x2085;
        }
        CLEAR_EVENTINF(EVENTINF_CUCCO_GAME_WON);
    } else if (maskReactionTextId == 0) {
        if (GET_INFTABLE(INFTABLE_TALKED_TO_TALON_IN_RANCH_HOUSE)) {
            if (GET_ITEMGETINF(ITEMGETINF_TALON_BOTTLE)) {
                // Play cucco game or buy milk
                this->actor.textId = 0x208B;
            } else {
                // Play cucco game?
                this->actor.textId = 0x207F;
            }
        } else {
            // First time talking to Talon in Lon Lon house
            this->actor.textId = 0x207E;
        }
    } else {
        this->actor.textId = maskReactionTextId;
    }
}

void En_Ta_Actor_ct(Actor* thisx, PlayState* play2) {
    EnTa* this = (EnTa*)thisx;
    PlayState* play = play2;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 36.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gTalonSkel, &gTalonStandAnim, this->jointTable, this->morphTable,
                       ENTA_LIMB_MAX);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnTaOcInfoData);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->stateFlags = 0;
    this->rapidBlinks = 0;
    this->nodOffTimer = 0;
    this->blinkTimer = 20;
    this->blinkFunc = Ossan_glare;
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.velocity.y = -4.0f;
    this->actor.minVelocityY = -4.0f;
    this->actor.gravity = -1.0f;

    switch (this->actor.params) {
        case ENTA_IN_KAKARIKO:
            // "Exile Talon"
            PRINTF(VT_FGCOL(CYAN) " 追放タロン \n" VT_RST);
            if (GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO)) {
                Actor_delete(&this->actor);
            } else if (!LINK_IS_ADULT) {
                Actor_delete(&this->actor);
            } else if (GET_EVENTCHKINF(EVENTCHKINF_TALON_WOKEN_IN_KAKARIKO)) {
                En_Ta_actor_set_process(this, matsu2, normal_anime_proc);
                this->eyeIndex = TALON_EYE_INDEX_OPEN;
                Skeleton_Info2_init_standard_stop(&this->skelAnime, &gTalonStandAnim);
                this->currentAnimation = &gTalonStandAnim;
            } else {
                En_Ta_actor_set_process(this, neteru2, sleep_anime_proc);
                this->eyeIndex = TALON_EYE_INDEX_CLOSED;
                Skeleton_Info2_init_standard_stop(&this->skelAnime, &gTalonSleepAnim);
                this->currentAnimation = &gTalonSleepAnim;
                this->actor.shape.shadowScale = 54.0f;
            }
            break;

        case ENTA_RETURNED_FROM_KAKARIKO:
            // "Return Talon"
            PRINTF(VT_FGCOL(CYAN) " 出戻りタロン \n" VT_RST);
            if (!GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO)) {
                Actor_delete(&this->actor);
            } else if (!LINK_IS_ADULT) {
                Actor_delete(&this->actor);
            } else if (play->sceneId == SCENE_STABLE && !IS_DAY) {
                Actor_delete(&this->actor);
                PRINTF(VT_FGCOL(CYAN) " 夜はいない \n" VT_RST);
            } else {
                En_Ta_actor_set_process(this, matsu3, normal_anime_proc);
                this->eyeIndex = TALON_EYE_INDEX_OPEN;
                Skeleton_Info2_init_standard_stop(&this->skelAnime, &gTalonStandAnim);
                this->currentAnimation = &gTalonStandAnim;
            }
            break;

        default: // Child era Talon
            // "Other Talon"
            PRINTF(VT_FGCOL(CYAN) " その他のタロン \n" VT_RST);
            if (play->sceneId == SCENE_HYRULE_CASTLE) {
                if (GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                    Actor_delete(&this->actor);
                } else if (GET_EVENTCHKINF(EVENTCHKINF_TALON_WOKEN_IN_CASTLE)) {
                    En_Ta_actor_set_process(this, matsu, normal_anime_proc);
                    this->eyeIndex = TALON_EYE_INDEX_OPEN;
                    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gTalonStandAnim);
                    this->currentAnimation = &gTalonStandAnim;
                } else {
                    En_Ta_actor_set_process(this, neteru0, sleep_anime_proc);
                    this->eyeIndex = TALON_EYE_INDEX_CLOSED;
                    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gTalonSleepAnim);
                    this->currentAnimation = &gTalonSleepAnim;
                    this->actor.shape.shadowScale = 54.0f;
                }
            } else if (play->sceneId == SCENE_LON_LON_BUILDINGS) {
                PRINTF(VT_FGCOL(CYAN) " ロンロン牧場の倉庫 の タロン\n" VT_RST);
                if (!GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                    Actor_delete(&this->actor);
                } else if (LINK_IS_ADULT) {
                    Actor_delete(&this->actor);
                } else {
                    if (IS_DAY) {
                        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
                        this->superCuccoTimers[0] = this->superCuccoTimers[1] = this->superCuccoTimers[2] = 7;
                        this->superCuccos[0] = (EnNiw*)Actor_info_make_actor(
                            &play->actorCtx, play, ACTOR_EN_NIW, this->actor.world.pos.x + 5.0f,
                            this->actor.world.pos.y + 3.0f, this->actor.world.pos.z + 26.0f, 0, 0, 0, 0xD);
                        this->superCuccos[1] = (EnNiw*)Actor_info_make_actor(
                            &play->actorCtx, play, ACTOR_EN_NIW, this->actor.world.pos.x - 20.0f,
                            this->actor.world.pos.y + 40.0f, this->actor.world.pos.z - 30.0f, 0, 0, 0, 0xD);
                        this->superCuccos[2] = (EnNiw*)Actor_info_make_actor(
                            &play->actorCtx, play, ACTOR_EN_NIW, this->actor.world.pos.x + 20.0f,
                            this->actor.world.pos.y + 40.0f, this->actor.world.pos.z - 30.0f, 0, 0, 0, 0xD);
                        set_hajimete_messaeg(this, play);

                        if (GET_EVENTINF(EVENTINF_CUCCO_GAME_FINISHED)) {
                            En_Ta_actor_set_process(this, kessan, stop_anime_proc);
                            Skeleton_Info2_init(&this->skelAnime, &gTalonSitWakeUpAnim, 1.0f,
                                             Si2_anime_end_frame(&gTalonSitWakeUpAnim) - 1.0f,
                                             Si2_anime_end_frame(&gTalonSitWakeUpAnim), ANIMMODE_ONCE, 0.0f);
                            CLEAR_EVENTINF(EVENTINF_CUCCO_GAME_FINISHED);
                        } else {
                            En_Ta_actor_set_process(this, hajimete, inemuri_anime_proc);
                            this->eyeIndex = TALON_EYE_INDEX_OPEN;
                            Skeleton_Info2_init_standard_stop(&this->skelAnime, &gTalonSitSleepingAnim);
                            this->currentAnimation = &gTalonSitSleepingAnim;
                        }
                    } else {
                        En_Ta_actor_set_process(this, neteru1, sleep_anime_proc);
                        this->eyeIndex = TALON_EYE_INDEX_CLOSED;
                        Skeleton_Info2_init_standard_stop(&this->skelAnime, &gTalonSleepAnim);
                        this->currentAnimation = &gTalonSleepAnim;
                        this->actor.shape.shadowScale = 54.0f;
                    }
                }
            } else {
                En_Ta_actor_set_process(this, neteru0, sleep_anime_proc);
                this->eyeIndex = TALON_EYE_INDEX_CLOSED;
                Skeleton_Info2_init_standard_stop(&this->skelAnime, &gTalonSleepAnim);
                this->currentAnimation = &gTalonSleepAnim;
                this->actor.shape.shadowScale = 54.0f;
            }
            break;
    }
}

void to_stand_shadow_size(EnTa* this) {
    if (this->actor.shape.shadowScale > 36.0f) {
        this->actor.shape.shadowScale -= 0.8f;
    }
}

void En_Ta_Actor_dt(Actor* thisx, PlayState* play) {
    EnTa* this = (EnTa*)thisx;

    ClObjPipe_dt(play, &this->collider);

    if (this->actor.params != ENTA_IN_KAKARIKO && this->actor.params != ENTA_RETURNED_FROM_KAKARIKO &&
        play->sceneId == SCENE_LON_LON_BUILDINGS) {
        z_common_data.timerState = TIMER_STATE_OFF;
    }

    if (this->stateFlags & TALON_STATE_FLAG_RESTORE_BGM_ON_DESTROY) {
        Na_StopMiddleBossBgm();
    }
}

static s32 kihon_process(EnTa* this, PlayState* play, u16 textId) {
    if (Actor_talk_check(&this->actor, play)) {
        return true;
    }

    this->actor.textId = textId;

    if ((ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x4300) &&
        (this->actor.xzDistToPlayer < 100.0f)) {
        this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
        Actor_talk_request2(&this->actor, play, 100.0f);
    }
    return false;
}

void talk_neteru2(EnTa* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        En_Ta_actor_set_process(this, neteru2, sleep_anime_proc);
    }
}

void talk_neteru1(EnTa* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        En_Ta_actor_set_process(this, neteru1, sleep_anime_proc);
    }
}

static void to_matsu(EnTa* this) {
    if (!LINK_IS_ADULT) {
        En_Ta_actor_set_process(this, matsu, normal_anime_proc);
        SET_EVENTCHKINF(EVENTCHKINF_TALON_WOKEN_IN_CASTLE);
    } else {
        En_Ta_actor_set_process(this, matsu2, normal_anime_proc);
        SET_EVENTCHKINF(EVENTCHKINF_TALON_WOKEN_IN_KAKARIKO);
    }
}

void talk_matsu_3(EnTa* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        to_matsu(this);
    }
    to_stand_shadow_size(this);
    this->stateFlags |= TALON_STATE_FLAG_SUPPRESS_BLINK;
}

void talk_matsu_2(EnTa* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        to_matsu(this);
        this->blinkTimer = 1;
        this->blinkFunc = Ossan_wink;
    }

    if (message_check(&play->msgCtx) == TEXT_STATE_DONE) {
        // Half-open eyes once the textbox reaches its end
        this->eyeIndex = TALON_EYE_INDEX_HALF;
        En_Ta_actor_set_process(this, talk_matsu_3, normal_anime_proc);
    }
    to_stand_shadow_size(this);
    this->stateFlags |= TALON_STATE_FLAG_SUPPRESS_BLINK;
}

static void talk_matsu(EnTa* this, PlayState* play) {
    this->stateFlags |= TALON_STATE_FLAG_SUPPRESS_BLINK;

    if (this->timer == 0) {
        En_Ta_actor_set_process(this, talk_matsu_2, normal_anime_proc);
        this->rapidBlinks = 3;
        this->timer = 60;
        Skeleton_Info2_init_standard_stop(&this->skelAnime, &gTalonWakeUpAnim);
        this->currentAnimation = &gTalonStandAnim;
        Actor_SE_set(&this->actor, NA_SE_VO_TA_SURPRISE);
    }
}

void talk_neteru0(EnTa* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        En_Ta_actor_set_process(this, neteru0, sleep_anime_proc);
    }
}

void neteru0(EnTa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_talk_check(&this->actor, play)) {
        s32 exchangeItemId = Actor_get_item_check(play);

        switch (exchangeItemId) {
            case EXCH_ITEM_CHICKEN:
                player->actor.textId = 0x702B;
                En_Ta_actor_set_process(this, talk_matsu, normal_anime_proc);
                this->timer = 40;
                break;

            default:
                if (exchangeItemId != EXCH_ITEM_NONE) {
                    player->actor.textId = 0x702A;
                }
                En_Ta_actor_set_process(this, talk_neteru0, sleep_anime_proc);
                break;
        }
    } else {
        this->actor.textId = 0x702A;
        Actor_talk_request_get_item(&this->actor, play, 100.0f, EXCH_ITEM_CHICKEN);
    }
}

void neteru1(EnTa* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        En_Ta_actor_set_process(this, talk_neteru1, sleep_anime_proc);
    }

    this->actor.textId = 0x204B;
    Actor_talk_request2(&this->actor, play, 100.0f);
}

void neteru2(EnTa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Actor_talk_check(&this->actor, play)) {
        s32 exchangeItemId = Actor_get_item_check(play);

        switch (exchangeItemId) {
            case EXCH_ITEM_POCKET_CUCCO:
                player->actor.textId = 0x702B;
                En_Ta_actor_set_process(this, talk_matsu, normal_anime_proc);
                this->timer = 40;
                break;

            default:
                if (exchangeItemId != EXCH_ITEM_NONE) {
                    player->actor.textId = 0x5015;
                }
                En_Ta_actor_set_process(this, talk_neteru2, sleep_anime_proc);
                break;
        }
    } else {
        this->actor.textId = 0x5015;
        Actor_talk_request_get_item(&this->actor, play, 100.0f, EXCH_ITEM_POCKET_CUCCO);
    }
}

void move_before(EnTa* this, PlayState* play) {
    s32 framesMod12 = (s32)play->state.frames % 12;

    if (framesMod12 == 0 || framesMod12 == 6) {
        Actor_SE_set(&this->actor, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_DIRT);
    }
    if (this->actor.speed < 6.0f) {
        this->actor.speed += 0.4f;
    }
    Actor_position_moveF(&this->actor);
}

void move2(EnTa* this, PlayState* play) {
    // Spawn dust particles
    dust_fly_set2(play, &this->actor.world.pos, 50.0f, 2, 250, 20, 1);
    move_before(this, play);

    if (this->timer == 0) {
        Actor_delete(&this->actor);
    }
}

void turn2(EnTa* this, PlayState* play) {
    this->actor.world.rot.y += 0xC00;
    this->actor.shape.rot.y += 0xC00;

    if (this->timer == 0) {
        En_Ta_actor_set_process(this, move2, normal_anime_proc);
        this->timer = 60;
    }
}

void move1(EnTa* this, PlayState* play) {
    // Spawn dust particles
    dust_fly_set2(play, &this->actor.world.pos, 50.0f, 2, 250, 20, 1);
    move_before(this, play);

    if (this->timer == 0) {
        En_Ta_actor_set_process(this, turn2, normal_anime_proc);
        this->timer = 5;
    }
}

void turn1(EnTa* this, PlayState* play) {
    this->actor.world.rot.y -= 0xD00;
    this->actor.shape.rot.y -= 0xD00;

    if (this->timer == 0) {
        En_Ta_actor_set_process(this, move1, normal_anime_proc);
        this->timer = 65;
    }
}

void move0(EnTa* this, PlayState* play) {
    // Spawn dust particles
    dust_fly_set2(play, &this->actor.world.pos, 50.0f, 2, 250, 20, 1);

    move_before(this, play);

    if (this->timer == 20) {
        message_close(play);
    }
    if (this->timer == 0) {
        this->timer = 5;
        En_Ta_actor_set_process(this, turn1, normal_anime_proc);
    }
}

void turn0(EnTa* this, PlayState* play) {
    this->actor.world.rot.y -= 0xC00;
    this->actor.shape.rot.y -= 0xC00;

    if (this->timer == 0) {
        Actor_SE_set(&this->actor, NA_SE_VO_TA_CRY_1);
        En_Ta_actor_set_process(this, move0, normal_anime_proc);
        this->timer = 65;
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }
}

void matsu_2(EnTa* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT) {
        // Start the running away cutscene
        makeOnepointDemo(play, 4175, -99, &this->actor, CAM_ID_MAIN);
        En_Ta_actor_set_process(this, turn0, normal_anime_proc);
        this->timer = 5;
        SET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE);
        Skeleton_Info2_init_standard_stop(&this->skelAnime, &gTalonRunTransitionAnim);
        this->currentAnimation = &gTalonRunAnim;
    }
    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

static void matsu(EnTa* this, PlayState* play) {
    if (kihon_process(this, play, 0x702C)) {
        En_Ta_actor_set_process(this, matsu_2, normal_anime_proc);
    }
    to_stand_shadow_size(this);
}

static void talk_matsu2(EnTa* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        En_Ta_actor_set_process(this, matsu2, normal_anime_proc);
    }
    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

static void matsu2(EnTa* this, PlayState* play) {
    if (GET_EVENTCHKINF(EVENTCHKINF_EPONA_OBTAINED)) {
        if (kihon_process(this, play, 0x5017)) {
            En_Ta_actor_set_process(this, talk_matsu2, normal_anime_proc);
            SET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO);
        }
    } else if (kihon_process(this, play, 0x5016)) {
        En_Ta_actor_set_process(this, talk_matsu2, normal_anime_proc);
    }
    to_stand_shadow_size(this);
}

void talk_matsu3(EnTa* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        En_Ta_actor_set_process(this, matsu3, normal_anime_proc);
    }
    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

void matsu3(EnTa* this, PlayState* play) {
    if (kihon_process(this, play, 0x2055)) {
        En_Ta_actor_set_process(this, talk_matsu3, normal_anime_proc);
    }
}

static s32 RRMilkCheck(void) {
    if (z_common_data.save.info.playerData.rupees < 30) {
        return TALON_CANBUYMILK_NOT_ENOUGH_RUPEES;
    } else if (!findEmptyBottle()) {
        return TALON_CANBUYMILK_NO_EMPTY_BOTTLE;
    } else {
        return TALON_CANBUYMILK_SUCCESS;
    }
}

void ta_demo_ct(EnTa* this, PlayState* play) {
    Vec3f subCamEye;
    Vec3f subCamAt;

    this->subCamId = Gama_play_make_camera(play);
    this->returnToCamId = play->activeCamId;
    Gama_play_set_camera_status(play, this->returnToCamId, CAM_STAT_WAIT);
    Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);

    subCamEye.x = 1053.0f;
    subCamEye.y = 11.0f;
    subCamEye.z = 22.0f;

    subCamAt.x = 1053.0f;
    subCamAt.y = 45.0f;
    subCamAt.z = -40.0f;

    Gama_play_camera_setting(play, this->subCamId, &subCamAt, &subCamEye);
}

void ta_demo_dt(EnTa* this, PlayState* play) {
    Gama_play_set_camera_status(play, this->returnToCamId, CAM_STAT_ACTIVE);
    Gama_play_clear_camera(play, this->subCamId);
}

void to_inemuri(EnTa* this, EnTaActionFunc actionFunc) {
    En_Ta_actor_set_process(this, actionFunc, inemuri_anime_proc);
    this->eyeIndex = TALON_EYE_INDEX_CLOSED;
    Skeleton_Info2_init(&this->skelAnime, &gTalonSitSleepingAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gTalonSitSleepingAnim), ANIMMODE_ONCE, -5.0f);
    this->nodOffTimer = 0;
    this->currentAnimation = &gTalonSitSleepingAnim;
}

void to_okiru(EnTa* this, EnTaActionFunc actionFunc) {
    this->eyeIndex = TALON_EYE_INDEX_HALF;
    En_Ta_actor_set_process(this, actionFunc, stop_anime_proc);
    this->stateFlags &= ~TALON_STATE_FLAG_ANIMATION_FINISHED;
    Skeleton_Info2_init(&this->skelAnime, &gTalonSitWakeUpAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gTalonSitWakeUpAnim),
                     ANIMMODE_ONCE, -5.0f);
}

void Ta_talk_end(EnTa* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        to_inemuri(this, hajimete);
        set_hajimete_messaeg(this, play);
    }
    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

s32 check_kin_niw(EnTa* this, PlayState* play, s32 cuccoIdx) {
    Player* player = GET_PLAYER(play);
    Actor* interactRangeActor;

    if (player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        interactRangeActor = player->interactRangeActor;
        if (interactRangeActor != NULL && interactRangeActor->id == ACTOR_EN_NIW &&
            interactRangeActor == &this->superCuccos[cuccoIdx]->actor) {
            return true;
        }
    }
    return false;
}

void ta_game_talk_end(EnTa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        s32 lastFoundSuperCuccoIdx;

        Skeleton_Info2_init(&this->skelAnime, &gTalonSitWakeUpAnim, 1.0f,
                         Si2_anime_end_frame(&gTalonSitWakeUpAnim) - 1.0f,
                         Si2_anime_end_frame(&gTalonSitWakeUpAnim), ANIMMODE_ONCE, 10.0f);
        this->stateFlags &= ~TALON_STATE_FLAG_ANIMATION_FINISHED;
        message_close(play);
        lastFoundSuperCuccoIdx = this->lastFoundSuperCuccoIdx;
        this->actionFunc = ta_game_play;

        // Make the found cucco fly directly upwards and then forget about it
        this->superCuccos[lastFoundSuperCuccoIdx]->actor.gravity = 0.1f;
        this->superCuccos[lastFoundSuperCuccoIdx]->actor.velocity.y = 0.0f;
        this->superCuccos[lastFoundSuperCuccoIdx]->actor.speed = 0.0f;
        this->superCuccos[lastFoundSuperCuccoIdx]->actor.parent = NULL;

        if (player->interactRangeActor == &this->superCuccos[lastFoundSuperCuccoIdx]->actor) {
            player->interactRangeActor = NULL;
        }
        if (player->heldActor == &this->superCuccos[lastFoundSuperCuccoIdx]->actor) {
            player->heldActor = NULL;
        }
        player->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
        this->superCuccos[lastFoundSuperCuccoIdx] = NULL;
    }
    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

void ta_game_talk(EnTa* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = ta_game_talk_end;
        // Unset auto-talking
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else {
        Actor_talk_request2(&this->actor, play, 1000.0f);
    }
    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

s32 check_kin_niwa_num(EnTa* this, PlayState* play) {
    s32 count;
    s32 i;

    for (count = 0, i = 0; i < ARRAY_COUNT(this->superCuccos); i++) {
        if (this->superCuccos[i] != NULL) {
            count++;
        }
    }
    return count;
}

void banzai(EnTa* this) {
    if (this->stateFlags & TALON_STATE_FLAG_ANIMATION_FINISHED) {
        if (this->stateFlags & TALON_STATE_FLAG_RAISING_HANDS) {
            Skeleton_Info2_init(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 17.0f, 22.0f, ANIMMODE_ONCE, 0.0f);
            this->stateFlags &= ~TALON_STATE_FLAG_RAISING_HANDS;
        } else {
            Skeleton_Info2_init(&this->skelAnime, &gTalonSitHandsUpAnim, -1.0f, 21.0f, 16.0f, ANIMMODE_ONCE, 3.0f);
            this->stateFlags |= TALON_STATE_FLAG_RAISING_HANDS;
        }
        this->stateFlags &= ~TALON_STATE_FLAG_ANIMATION_FINISHED;
    }
}

void to_next_ta(EnTa* this, PlayState* play) {
    banzai(this);

    if (this->timer == 0) {
        if (this->stateFlags & TALON_STATE_FLAG_FLOOR_CAMERA_ACTIVE) {
            this->stateFlags &= ~TALON_STATE_FLAG_FLOOR_CAMERA_ACTIVE;
            ta_demo_dt(this, play);
        }
    }
}

void ta_game_over(EnTa* this, PlayState* play) {
    banzai(this);

    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        play->nextEntranceIndex = ENTR_LON_LON_BUILDINGS_2;

        if (GET_EVENTINF(EVENTINF_CUCCO_GAME_WON)) {
            play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
            z_common_data.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        } else {
            play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST);
            z_common_data.nextTransitionType = TRANS_TYPE_FADE_BLACK;
        }

        play->transitionTrigger = TRANS_TRIGGER_START;
        SET_EVENTINF(EVENTINF_CUCCO_GAME_FINISHED);
        this->actionFunc = to_next_ta;
        this->timer = 22;
    }
}

void ta_game_play(EnTa* this, PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->superCuccos); i++) {
        if (this->superCuccos[i] != NULL) {
            if (this->superCuccos[i]->actor.gravity > -2.0f) {
                this->superCuccos[i]->actor.gravity -= 0.03f;
            }

            if (check_kin_niw(this, play, i)) {
                if (this->superCuccoTimers[i] > 0) {
                    // Wait until the cucco's timer runs out after
                    // acknowledging that the player picked it up.
                    this->superCuccoTimers[i]--;
                } else {
                    this->lastFoundSuperCuccoIdx = i;
                    Skeleton_Info2_init(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE, -10.0f);
                    this->stateFlags &= ~TALON_STATE_FLAG_ANIMATION_FINISHED;

                    switch (check_kin_niwa_num(this, play)) {
                        case 1:
                            // Last cucco found, end the game
                            z_common_data.timerState = TIMER_STATE_OFF;
                            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);

                            message_set(play, 0x2084, &this->actor);
                            this->actionFunc = ta_game_over;
                            Skeleton_Info2_init(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE,
                                             -10.0f);
                            this->stateFlags &= ~TALON_STATE_FLAG_ANIMATION_FINISHED;
                            this->stateFlags &= ~TALON_STATE_FLAG_RAISING_HANDS;
                            SET_EVENTINF(EVENTINF_CUCCO_GAME_WON);
                            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
                            this->stateFlags &= ~TALON_STATE_FLAG_RESTORE_BGM_ON_DESTROY;
                            Na_StartFanfare(NA_BGM_SMALL_ITEM_GET);
                            return;

                        case 2:
                            // One cucco remaining
                            this->actor.textId = 0x2083;
                            Actor_SE_set(&this->actor, NA_SE_VO_TA_CRY_1);
                            break;

                        case 3:
                            // Two cuccos remaining
                            this->actor.textId = 0x2082;
                            Actor_SE_set(&this->actor, NA_SE_VO_TA_SURPRISE);
                            break;
                    }
                    this->actionFunc = ta_game_talk;

                    // Automatically talk to player
                    this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                    Actor_talk_request2(&this->actor, play, 1000.0f);
                    return;
                }
            } else {
                this->superCuccoTimers[i] = 7;
            }
        }
    }

    if (z_common_data.timerSeconds == 10) {
        Na_SetMiniGameBgmTempoUp();
    }

    if ((z_common_data.timerSeconds == 0) && !Game_play_demo_mode_check(play)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
        this->stateFlags &= ~TALON_STATE_FLAG_RESTORE_BGM_ON_DESTROY;
        Na_StartSystemSe_F(NA_SE_SY_FOUND);
        z_common_data.timerState = TIMER_STATE_OFF;
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);

        // Time's up text
        message_set(play, 0x2081, &this->actor);
        this->actionFunc = ta_game_over;
        ta_demo_ct(this, play);
        CLEAR_EVENTINF(EVENTINF_CUCCO_GAME_WON);
        this->stateFlags |= TALON_STATE_FLAG_FLOOR_CAMERA_ACTIVE;
        Skeleton_Info2_init(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE, -10.0f);
        this->stateFlags &= ~TALON_STATE_FLAG_ANIMATION_FINISHED;
        this->stateFlags &= ~TALON_STATE_FLAG_RAISING_HANDS;
    }

    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

void ta_game_start(EnTa* this, PlayState* play) {
    s32 i;

    if (this->timer > 35) {
        // During the first part of the throw animation,
        // just turn them (on the table or the floor)
        for (i = 1; i < ARRAY_COUNT(this->superCuccos); i++) {
            if (this->superCuccos[i] != NULL) {
                add_calc_short_angle2(&this->superCuccos[i]->actor.world.rot.y, i * -10000 - 3000, 2, 0x800, 0x100);
                this->superCuccos[i]->actor.shape.rot.y = this->superCuccos[i]->actor.world.rot.y;
            }
        }
    } else if (this->timer == 35) {
        // At this point, prepare each super cucco for flight
        for (i = 0; i < ARRAY_COUNT(this->superCuccos); i++) {
            // Set a 7-12 frame zero-gravity time for each super cucco
            this->superCuccoTimers[i] = (s32)(rnd_fx(6.0f) + 10.0f);

            if (this->superCuccos[i] != NULL) {
                EnNiw* niw = this->superCuccos[i];

                // Mark the cucco as a super cucco, this will cause the cucco
                // to set random xz and y velocities
                niw->unk_308 = 1;
                niw->actor.gravity = 0.0f;
            }
        }
    } else {
        for (i = 0; i < ARRAY_COUNT(this->superCuccos); i++) {
            // If the zero-gravity time of the cucco is over,
            // add gravity to it
            if (this->timer < 35 - this->superCuccoTimers[i]) {
                if (this->superCuccos[i] != NULL) {
                    if (this->superCuccos[i]->actor.gravity > -2.0f) {
                        this->superCuccos[i]->actor.gravity -= 0.03f;
                    }
                }
            }
        }
    }

    if (this->timer == 0) {
        En_Ta_actor_set_process(this, ta_game_play, stop_anime_proc);
        this->stateFlags &= ~TALON_STATE_FLAG_ANIMATION_FINISHED;
        Skeleton_Info2_init(&this->skelAnime, &gTalonSitWakeUpAnim, 1.0f,
                         Si2_anime_end_frame(&gTalonSitWakeUpAnim) - 1.0f,
                         Si2_anime_end_frame(&gTalonSitWakeUpAnim), ANIMMODE_ONCE, 10.0f);
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
    }
}

void ta_game_ready(EnTa* this, PlayState* play) {
    if (this->timer == 0 && this->stateFlags & TALON_STATE_FLAG_CUCCO_GAME_START_EVENT_TRIGGERED) {
        En_Ta_actor_set_process(this, ta_game_start, stop_anime_proc);
        this->stateFlags &= ~TALON_STATE_FLAG_ANIMATION_FINISHED;
        // Play hand raise animation again so that it looks like Talon throws the cuccos
        Skeleton_Info2_init(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 1.0f,
                         Si2_anime_end_frame(&gTalonSitHandsUpAnim), ANIMMODE_ONCE, 0.0f);
        this->timer = 50;

        event_timer_set(30);
        Na_StartMiddleBossBgm(NA_BGM_TIMED_MINI_GAME);
        this->stateFlags |= TALON_STATE_FLAG_RESTORE_BGM_ON_DESTROY;
        message_close(play);
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
    }

    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        this->stateFlags |= TALON_STATE_FLAG_CUCCO_GAME_START_EVENT_TRIGGERED;
    }

    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

void ta_game_ready1(EnTa* this, PlayState* play) {
    if (this->stateFlags & TALON_STATE_FLAG_ANIMATION_FINISHED) {
        En_Ta_actor_set_process(this, ta_game_ready, stop_anime_proc);
        this->stateFlags &= ~TALON_STATE_FLAG_ANIMATION_FINISHED;
        Skeleton_Info2_init(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 0.0f, 1.0f, ANIMMODE_ONCE, 0.0f);
        this->timer = 5;
    }
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        this->stateFlags |= TALON_STATE_FLAG_CUCCO_GAME_START_EVENT_TRIGGERED;
    }
    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

void ta_game_ready0(EnTa* this, PlayState* play) {
    if (this->stateFlags & TALON_STATE_FLAG_ANIMATION_FINISHED) {
        En_Ta_actor_set_process(this, ta_game_ready1, stop_anime_proc);
        this->stateFlags &= ~TALON_STATE_FLAG_ANIMATION_FINISHED;
        Skeleton_Info2_init(&this->skelAnime, &gTalonSitHandsUpAnim, -1.0f, 29.0f, 0.0f, ANIMMODE_ONCE, 10.0f);
    }
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        this->stateFlags |= TALON_STATE_FLAG_CUCCO_GAME_START_EVENT_TRIGGERED;
    }
    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

void to_game_start(EnTa* this, PlayState* play) {
    En_Ta_actor_set_process(this, ta_game_ready0, stop_anime_proc);
    this->stateFlags &= ~TALON_STATE_FLAG_ANIMATION_FINISHED;
    Skeleton_Info2_init(&this->skelAnime, &gTalonSitHandsUpAnim, 1.0f, 8.0f, 29.0f, ANIMMODE_ONCE, -10.0f);
    message_set2(play, 0x2080);
    this->stateFlags &= ~TALON_STATE_FLAG_CUCCO_GAME_START_EVENT_TRIGGERED;
}

void ataeru_wait(EnTa* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        to_inemuri(this, hajimete);
        set_hajimete_messaeg(this, play);
    }
    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

void ataeru(EnTa* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = ataeru_wait;
        if (!(this->stateFlags & TALON_STATE_FLAG_GIVING_MILK_REFILL)) {
            SET_ITEMGETINF(ITEMGETINF_TALON_BOTTLE);
        }
        this->stateFlags &= ~TALON_STATE_FLAG_GIVING_MILK_REFILL;
    } else if (this->stateFlags & TALON_STATE_FLAG_GIVING_MILK_REFILL) {
        Actor_carry_request_set2(&this->actor, play, GI_MILK, 10000.0f, 50.0f);
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_BOTTLE_MILK_FULL, 10000.0f, 50.0f);
    }
    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

void ataeru_mae(EnTa* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        this->stateFlags &= ~TALON_STATE_FLAG_GIVING_MILK_REFILL;
        En_Ta_actor_set_process(this, ataeru, stop_anime_proc);
        Actor_carry_request_set2(&this->actor, play, GI_BOTTLE_MILK_FULL, 10000.0f, 50.0f);
    }
}

void sentaku0(EnTa* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // Buy milk
                switch (RRMilkCheck()) {
                    case TALON_CANBUYMILK_NOT_ENOUGH_RUPEES:
                        message_set2(play, 0x85);
                        En_Ta_actor_set_process(this, Ta_talk_end, stop_anime_proc);
                        break;

                    case TALON_CANBUYMILK_NO_EMPTY_BOTTLE:
                        message_set2(play, 0x208A);
                        En_Ta_actor_set_process(this, ataeru_wait, stop_anime_proc);
                        break;

                    case TALON_CANBUYMILK_SUCCESS:
                        this->stateFlags |= TALON_STATE_FLAG_GIVING_MILK_REFILL;
                        En_Ta_actor_set_process(this, ataeru, stop_anime_proc);
                        lupy_increase(-30);
                        Actor_carry_request_set2(&this->actor, play, GI_MILK, 10000.0f, 50.0f);
                        break;
                }
                break;

            case 1: // Play cucco game
                if (z_common_data.save.info.playerData.rupees < 10) {
                    message_set2(play, 0x85);
                    En_Ta_actor_set_process(this, Ta_talk_end, stop_anime_proc);
                } else {
                    lupy_increase(-10);
                    to_game_start(this, play);
                }
                break;

            case 2: // Cancel
                to_inemuri(this, hajimete);
                set_hajimete_messaeg(this, play);
                break;
        }
    }

    if (this->stateFlags & TALON_STATE_FLAG_ANIMATION_FINISHED) {
        this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
    }
}

void sentaku1(EnTa* this, PlayState* play) {
    s32 price;

    // Check if this is a retry
    if (this->actor.textId == 0x2085) {
        price = 5;
    } else {
        price = 10;
    }

    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                if (z_common_data.save.info.playerData.rupees < price) {
                    message_set2(play, 0x85);
                    En_Ta_actor_set_process(this, Ta_talk_end, stop_anime_proc);
                } else {
                    lupy_increase(-price);
                    to_game_start(this, play);
                }
                break;

            case 1:
                to_inemuri(this, hajimete);
                set_hajimete_messaeg(this, play);
                break;
        }
    }

    if (this->stateFlags & TALON_STATE_FLAG_ANIMATION_FINISHED) {
        this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
    }
}

void sentaku2(EnTa* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        message_set2(play, 0x2087);
        En_Ta_actor_set_process(this, ataeru_mae, stop_anime_proc);
    }

    if (this->stateFlags & TALON_STATE_FLAG_ANIMATION_FINISHED) {
        this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
    }
}

void bunki(EnTa* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        SET_INFTABLE(INFTABLE_TALKED_TO_TALON_IN_RANCH_HOUSE);
        if (GET_ITEMGETINF(ITEMGETINF_TALON_BOTTLE)) {
            // Play cucco game or buy milk
            message_set2(play, 0x208B);
            En_Ta_actor_set_process(this, sentaku0, stop_anime_proc);
        } else {
            // Play cucco game?
            message_set2(play, 0x207F);
            En_Ta_actor_set_process(this, sentaku1, stop_anime_proc);
        }
    }

    if (this->stateFlags & TALON_STATE_FLAG_ANIMATION_FINISHED) {
        this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
    }
}

void bunki2(EnTa* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        if (findEmptyBottle()) {
            message_close(play);
            this->stateFlags |= TALON_STATE_FLAG_GIVING_MILK_REFILL;
            En_Ta_actor_set_process(this, ataeru, stop_anime_proc);
            Actor_carry_request_set2(&this->actor, play, GI_MILK, 10000.0f, 50.0f);
        } else {
            message_set2(play, 0x208A);
            En_Ta_actor_set_process(this, ataeru_wait, stop_anime_proc);
        }
    }
}

void hajimete(EnTa* this, PlayState* play) {
    u16 maskReactionTextId = get_mask_message(play, MASK_REACTION_SET_TALON);

    set_hajimete_messaeg(this, play);

    if (kihon_process(this, play, this->actor.textId)) {
        Actor_SE_set(&this->actor, NA_SE_VO_TA_SURPRISE);

        if (maskReactionTextId != 0) {
            to_okiru(this, ataeru_wait);
        } else {
            SET_INFTABLE(INFTABLE_TALKED_TO_TALON_IN_RANCH_HOUSE);

            switch (this->actor.textId) {
                case 0x207E: // First time talking in Lon Lon House
                case 0x207F: // Play cucco game
                    to_okiru(this, sentaku1);
                    break;

                case 0x208B: // Play cucco game or buy milk
                    to_okiru(this, sentaku0);
                    break;

                default:
                    to_okiru(this, bunki);
                    break;
            }
        }
    }
    this->stateFlags &= ~TALON_STATE_FLAG_TRACKING_PLAYER;
}

void kessan(EnTa* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        switch (this->actor.textId) {
            case 0x2085: // Retry?
                this->actionFunc = sentaku1;
                break;

            case 0x2086: // Initial win
                this->actionFunc = sentaku2;
                break;

            case 0x2088: // Later win
                this->actionFunc = bunki2;
                break;
        }
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_talk_request2(&this->actor, play, 1000.0f);
    }
    this->stateFlags |= TALON_STATE_FLAG_TRACKING_PLAYER;
}

static void Ossan_glare(EnTa* this) {
    s16 blinkTimer = this->blinkTimer - 1;

    if (blinkTimer != 0) {
        this->blinkTimer = blinkTimer;
    } else {
        this->blinkFunc = Ossan_wink;
    }
}

static void Ossan_wink(EnTa* this) {
    s16 blinkTimer = this->blinkTimer - 1;

    if (blinkTimer != 0) {
        this->blinkTimer = blinkTimer;
    } else {
        // Next towards closed eyes: open -> half, half -> closed
        s16 nextEyeIndex = this->eyeIndex + 1;

        // If the eyes were already closed, set the eyes open and set a new blink timer.
        // If just woken up (rapidBlinks > 0), blink a few times
        // in quick succession before starting the normal blink cycle.
        if (nextEyeIndex >= TALON_EYE_INDEX_MAX) {
            this->eyeIndex = TALON_EYE_INDEX_OPEN;
            if (this->rapidBlinks > 0) {
                this->rapidBlinks--;
                blinkTimer = 1;
            } else {
                blinkTimer = (s32)(fqrand() * 60.0f) + 20;
            }
            this->blinkTimer = blinkTimer;
            this->blinkFunc = Ossan_glare;
        } else {
            this->eyeIndex = nextEyeIndex;
            this->blinkTimer = 1;
        }
    }
}

static void normal_anime_proc(EnTa* this) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        Skeleton_Info2_init_standard_stop(&this->skelAnime, this->currentAnimation);
    }
}

void sleep_anime_proc(EnTa* this) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        Skeleton_Info2_init_standard_stop(&this->skelAnime, this->currentAnimation);
        Actor_SE_set(&this->actor, NA_SE_VO_TA_SLEEP);
    }
    this->stateFlags |= TALON_STATE_FLAG_SUPPRESS_ROCKING_ANIM | TALON_STATE_FLAG_SUPPRESS_BLINK;
}

void inemuri_anime_proc(EnTa* this) {
    if (this->nodOffTimer > 0) {
        // Pause the sleep animation until the nodding timer is at zero.
        // The torso and arms will still do the rocking movement because of before_display
        this->nodOffTimer--;
    } else {
        if (Skeleton_Info2_anime_play(&this->skelAnime)) {
            Skeleton_Info2_init_standard_stop(&this->skelAnime, this->currentAnimation);
            this->nodOffTimer = rnd_f(100.0f) + 100.0f;
        }

        if (this->skelAnime.curFrame < 96.0f && this->skelAnime.curFrame >= 53.0f) {
            // Half-open eyes during the part of the sleeping animation where Talon
            // raises his head after nodding off
            this->eyeIndex = TALON_EYE_INDEX_HALF;
        } else {
            // Otherwise keep the eyes closed
            this->eyeIndex = TALON_EYE_INDEX_CLOSED;
        }
        this->stateFlags |= TALON_STATE_FLAG_SUPPRESS_ROCKING_ANIM;
    }
    this->stateFlags |= TALON_STATE_FLAG_SUPPRESS_BLINK;
}

static void stop_anime_proc(EnTa* this) {
    if (!(this->stateFlags & TALON_STATE_FLAG_ANIMATION_FINISHED)) {
        if (Skeleton_Info2_anime_play(&this->skelAnime)) {
            this->stateFlags |= TALON_STATE_FLAG_ANIMATION_FINISHED;
        }
        this->stateFlags |= TALON_STATE_FLAG_SUPPRESS_ROCKING_ANIM;
    }
}

void En_Ta_Actor_move(Actor* thisx, PlayState* play) {
    EnTa* this = (EnTa*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    this->animFunc(this);
    this->actionFunc(this, play);

    if (!(this->stateFlags & TALON_STATE_FLAG_SUPPRESS_BLINK)) {
        this->blinkFunc(this);
    }

    if (this->stateFlags & TALON_STATE_FLAG_TRACKING_PLAYER) {
        eye_move2(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    } else {
        add_calc_short_angle2(&this->headRot.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->headRot.y, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->torsoRot.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->torsoRot.y, 0, 6, 6200, 100);
    }

    this->stateFlags &= ~(TALON_STATE_FLAG_TRACKING_PLAYER | TALON_STATE_FLAG_SUPPRESS_BLINK);

    if (this->timer > 0) {
        this->timer--;
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnTa* this = (EnTa*)thisx;

    // Turn head and chest towards the target (the rotation steps are calculated in En_Ta_Actor_move)
    switch (limbIndex) {
        case ENTA_LIMB_CHEST:
            rot->x += this->torsoRot.y;
            rot->y -= this->torsoRot.x;
            break;

        case ENTA_LIMB_HEAD:
            rot->x += this->headRot.y;
            rot->z += this->headRot.x;
            break;
    }

    // Rocking/wobbling animation for the torso and arms
    if (this->stateFlags & TALON_STATE_FLAG_SUPPRESS_ROCKING_ANIM) {
        // TALON_STATE_FLAG_SUPPRESS_ROCKING_ANIM might have been supposed to prevent
        // the rocking in some situations, (e.g. while sleeping).
        // But because this function is run first for the root limb (limbIndex 1),
        // and the flag is immediately unset, all subsequent calls end up
        // in the else if branch below and rocking always occurs.
        // So this flag has no effect.
        this->stateFlags &= ~TALON_STATE_FLAG_SUPPRESS_ROCKING_ANIM;
    } else if ((limbIndex == ENTA_LIMB_CHEST) || (limbIndex == ENTA_LIMB_LEFT_ARM) ||
               (limbIndex == ENTA_LIMB_RIGHT_ARM)) {
        s32 fidgetFrequency = limbIndex * FIDGET_FREQ_LIMB;

        rot->y += sin_s(play->state.frames * (fidgetFrequency + FIDGET_FREQ_Y)) * FIDGET_AMPLITUDE;
        rot->z += cos_s(play->state.frames * (fidgetFrequency + FIDGET_FREQ_Z)) * FIDGET_AMPLITUDE;
    }

    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 1100.0f, 1000.0f, 0.0f };
    EnTa* this = (EnTa*)thisx;

    if (limbIndex == ENTA_LIMB_HEAD) {
        Matrix_Position(&pos, &this->actor.focus.pos);
    }
}

void En_Ta_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = {
        gTalonEyeOpenTex,
        gTalonEyeHalfTex,
        gTalonEyeClosedTex,
    };
    EnTa* this = (EnTa*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ta.c", 2381);

    _polygon_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x9, SEGMENTED_TO_VIRTUAL(gTalonHeadSkinTex));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ta.c", 2400);
}
