#include "z_en_go.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_oF1d_map/object_oF1d_map.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Go_Actor_ct(Actor* thisx, PlayState* play);
void En_Go_Actor_dt(Actor* thisx, PlayState* play);
void En_Go_Actor_move(Actor* thisx, PlayState* play);
void En_Go_Actor_draw(Actor* thisx, PlayState* play);

void go_demo(EnGo* this, PlayState* play);
void go_demo_run(EnGo* this, PlayState* play);
void go_demo_explosion(EnGo* this, PlayState* play);
void go_escape(EnGo* this, PlayState* play);
void go_help(EnGo* this, PlayState* play);
void go_wait(EnGo* this, PlayState* play);
void go_wake(EnGo* this, PlayState* play);

void go_change_iwa(EnGo* this, PlayState* play);
static void go_stand_up(EnGo* this, PlayState* play);
void go_stand_wait(EnGo* this, PlayState* play);
void go_sit(EnGo* this, PlayState* play);

void go_guard(EnGo* this, PlayState* play);
static void go_carry_request(EnGo* this, PlayState* play);
static void go_carry_end(EnGo* this, PlayState* play);
void go_get_demo_start(EnGo* this, PlayState* play);
void go_get_demo_end(EnGo* this, PlayState* play);

static void go_eff_dust_ct(EnGo* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 initialTimer, f32 scale,
                          f32 scaleStep);
static void go_eff_dust_mv(EnGo* this);
static void go_eff_dust_dr(EnGo* this, PlayState* play);

ActorProfile En_Go_Profile = {
    /**/ ACTOR_EN_GO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OF1D_MAP,
    /**/ sizeof(EnGo),
    /**/ En_Go_Actor_ct,
    /**/ En_Go_Actor_dt,
    /**/ En_Go_Actor_move,
    /**/ En_Go_Actor_draw,
};

static ColliderCylinderInit EnGoAtInfoData = {
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
    { 20, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 GoStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnGoAnimation {
    /* 0 */ ENGO_ANIM_0,
    /* 1 */ ENGO_ANIM_1,
    /* 2 */ ENGO_ANIM_2,
    /* 3 */ ENGO_ANIM_3
} EnGoAnimation;

static AnimationSpeedInfo anime_ct_data[] = {
    { &gGoronAnim_004930, 0.0f, ANIMMODE_LOOP_INTERP, 0.0f },
    { &gGoronAnim_004930, 0.0f, ANIMMODE_LOOP_INTERP, -10.0f },
    { &gGoronAnim_0029A8, 1.0f, ANIMMODE_LOOP_INTERP, -10.0f },
    { &gGoronAnim_010590, 1.0f, ANIMMODE_LOOP_INTERP, -10.0f },
};

void En_Go_actor_set_process(EnGo* this, EnGoActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

u16 go_set_message(PlayState* play, Actor* thisx) {
    Player* player = GET_PLAYER(play);

    switch (PARAMS_GET_NOSHIFT(thisx->params, 4, 4)) {
        case 0x90:
            if (z_common_data.save.info.playerData.bgsFlag) {
                return 0x305E;
            } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_CLAIM_CHECK) {
                if (Get_Eventday() >= 3) {
                    return 0x305E;
                } else {
                    return 0x305D;
                }
            } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_EYE_DROPS) {
                player->exchangeItemId = EXCH_ITEM_EYE_DROPS;
                return 0x3059;
            } else if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_PRESCRIPTION) {
                return 0x3058;
            } else {
                player->exchangeItemId = EXCH_ITEM_BROKEN_GORONS_SWORD;
                return 0x3053;
            }
        case 0x00:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) {
                if (GET_INFTABLE(INFTABLE_10F)) {
                    return 0x3042;
                } else {
                    return 0x3041;
                }
            } else if (CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_GORON) || GET_INFTABLE(INFTABLE_10D)) {
                if (GET_INFTABLE(INFTABLE_10E)) {
                    return 0x3038;
                } else {
                    return 0x3037;
                }
            } else {
                if (GET_INFTABLE(INFTABLE_109)) {
                    if (GET_INFTABLE(INFTABLE_10A)) {
                        return 0x3033;
                    } else {
                        return 0x3032;
                    }
                } else {
                    return 0x3030;
                }
            }
        case 0x10:
            if (Actor_Environment_sw_Check(play, PARAMS_GET_NOMASK(thisx->params, 8))) {
                return 0x3052;
            } else {
                return 0x3051;
            }
        case 0x20:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (GET_EVENTCHKINF(EVENTCHKINF_23)) {
                return 0x3021;
            } else if (GET_INFTABLE(INFTABLE_E0)) {
                return 0x302A;
            } else {
                return 0x3008;
            }
        case 0x30:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (GET_EVENTCHKINF(EVENTCHKINF_23)) {
                return 0x3026;
            } else {
                return 0x3009;
            }
        case 0x40:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (GET_EVENTCHKINF(EVENTCHKINF_23)) {
                return 0x3026;
            } else {
                return 0x300A;
            }
        case 0x50:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (GET_INFTABLE(INFTABLE_F0)) {
                return 0x3015;
            } else {
                return 0x3014;
            }
        case 0x60:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (GET_INFTABLE(INFTABLE_F4)) {
                return 0x3017;
            } else {
                return 0x3016;
            }
        case 0x70:
            if (CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                return 0x3027;
            } else if (GET_INFTABLE(INFTABLE_F8)) {
                return 0x3019;
            } else {
                return 0x3018;
            }
        default:
            return 0x0000;
    }
}

s16 go_end_message(PlayState* play, Actor* thisx) {
    s16 talkState = NPC_TALK_STATE_TALKING;
    f32 xzRange;
    f32 yRange = fabsf(thisx->yDistToPlayer) + 1.0f;

    xzRange = thisx->xzDistToPlayer + 1.0f;
    switch (message_check(&play->msgCtx)) {
        if (play) {}
        case TEXT_STATE_CLOSING:
            switch (thisx->textId) {
                case 0x3008:
                    SET_INFTABLE(INFTABLE_E0);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x300B:
                    SET_INFTABLE(INFTABLE_EB);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3014:
                    SET_INFTABLE(INFTABLE_F0);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3016:
                    SET_INFTABLE(INFTABLE_F4);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3018:
                    SET_INFTABLE(INFTABLE_F8);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3036:
                    Actor_carry_request_set2(thisx, play, GI_TUNIC_GORON, xzRange, yRange);
                    SET_INFTABLE(INFTABLE_10D); // EnGo exclusive flag
                    talkState = NPC_TALK_STATE_ACTION;
                    break;
                case 0x3037:
                    SET_INFTABLE(INFTABLE_10E);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3041:
                    SET_INFTABLE(INFTABLE_10F);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x3059:
                    talkState = NPC_TALK_STATE_ACTION;
                    break;
                case 0x3052:
                case 0x3054:
                case 0x3055:
                case 0x305A:
                    talkState = NPC_TALK_STATE_ACTION;
                    break;
                case 0x305E:
                    talkState = NPC_TALK_STATE_ACTION;
                    break;
                default:
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
            }
            break;
        case TEXT_STATE_CHOICE:
            if (pad_on_check(play)) {
                switch (thisx->textId) {
                    case 0x300A:
                        if (play->msgCtx.choiceIndex == 0) {
                            if (CUR_UPG_VALUE(UPG_STRENGTH) != 0 || GET_INFTABLE(INFTABLE_EB)) {
                                thisx->textId = 0x300B;
                            } else {
                                thisx->textId = 0x300C;
                            }
                        } else {
                            thisx->textId = 0x300D;
                        }
                        message_set2(play, thisx->textId);
                        talkState = NPC_TALK_STATE_TALKING;
                        break;
                    case 0x3034:
                        if (play->msgCtx.choiceIndex == 0) {
                            if (GET_INFTABLE(INFTABLE_10B)) {
                                thisx->textId = 0x3033;
                            } else {
                                thisx->textId = 0x3035;
                            }
                        } else if (GET_INFTABLE(INFTABLE_10B)) {
                            thisx->textId = 0x3036;
                        } else {
                            thisx->textId = 0x3033;
                        }
                        message_set2(play, thisx->textId);
                        talkState = NPC_TALK_STATE_TALKING;
                        break;
                    case 0x3054:
                    case 0x3055:
                        if (play->msgCtx.choiceIndex == 0) {
                            talkState = NPC_TALK_STATE_ACTION;
                        } else {
                            thisx->textId = 0x3056;
                            message_set2(play, thisx->textId);
                            talkState = NPC_TALK_STATE_TALKING;
                        }
                        SET_INFTABLE(INFTABLE_B4);
                        break;
                }
            }
            break;
        case TEXT_STATE_EVENT:
            if (pad_on_check(play)) {
                switch (thisx->textId) {
                    case 0x3035:
                        SET_INFTABLE(INFTABLE_10B);
                        FALLTHROUGH;
                    case 0x3032:
                    case 0x3033:
                        thisx->textId = 0x3034;
                        message_set2(play, thisx->textId);
                        talkState = NPC_TALK_STATE_TALKING;
                        break;
                    default:
                        talkState = NPC_TALK_STATE_ACTION;
                        break;
                }
            }
            break;
        case TEXT_STATE_DONE:
            if (pad_on_check(play)) {
                talkState = NPC_TALK_STATE_ITEM_GIVEN;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_9:
            break;
    }
    return talkState;
}

s32 big_go_talk(PlayState* play, Actor* thisx, s16* talkState, f32 interactRange, NpcGetTextIdFunc getTextId,
                       NpcUpdateTalkStateFunc updateTalkState) {
    if (*talkState != NPC_TALK_STATE_IDLE) {
        *talkState = updateTalkState(play, thisx);
        return false;
    } else if (Actor_talk_check(thisx, play)) {
        *talkState = NPC_TALK_STATE_TALKING;
        return true;
    } else if (!Actor_talk_request2(thisx, play, interactRange)) {
        return false;
    } else {
        thisx->textId = getTextId(play, thisx);
        return false;
    }
}

void go_anime_ct(EnGo* this, s32 index) {
    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation,
                     anime_ct_data[index].playSpeed *
                         (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90 ? 0.5f : 1.0f),
                     0.0f, Si2_anime_end_frame(anime_ct_data[index].animation), anime_ct_data[index].mode,
                     anime_ct_data[index].morphFrames);
}

s32 go_appearance_check(EnGo* this, PlayState* play) {
    if (((this->actor.params) & 0xF0) == 0x90) {
        return true;
    } else if (play->sceneId == SCENE_FIRE_TEMPLE && !Actor_Environment_sw_Check(play, PARAMS_GET_NOMASK(this->actor.params, 8)) &&
               LINK_IS_ADULT && PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x10) {
        return true;
    } else if (play->sceneId == SCENE_GORON_CITY && LINK_IS_ADULT &&
               PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x00) {
        return true;
    } else if (play->sceneId == SCENE_DEATH_MOUNTAIN_TRAIL && LINK_IS_CHILD &&
               (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x20 ||
                PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x30 ||
                PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x40)) {
        return true;
    } else if (play->sceneId == SCENE_GORON_CITY && LINK_IS_CHILD &&
               (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x50 ||
                PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x60 ||
                PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x70)) {
        return true;
    } else {
        return false;
    }
}

f32 get_eye_point(EnGo* this) {
    switch (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4)) {
        case 0x00:
            return 10.0f;
        case 0x20:
        case 0x30:
        case 0x50:
        case 0x60:
        case 0x70:
            return 20.0f;
        case 0x40:
            return 60.0f;
        default:
            return 20.0f;
    }
}

void go_eye_move(EnGo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode;

    if (this->actionFunc != go_stand_wait && this->actionFunc != go_help &&
        this->actionFunc != go_guard) {
        trackingMode = NPC_TRACKING_NONE;
    }

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = get_eye_point(this);
    eye_moveM(&this->actor, &this->interactInfo, 4, trackingMode);
}

void go_eye_control(EnGo* this) {
    if (DECR(this->unk_214) == 0) {
        this->unk_216++;
        if (this->unk_216 >= 3) {
            this->unk_214 = get_random_timer(30, 30);
            this->unk_216 = 0;
        }
    }
}

s32 go_player_search(EnGo* this, PlayState* play) {
    f32 xyzDistSq;
    s16 yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    Camera* mainCam = play->cameraPtrs[CAM_ID_MAIN];

    if (fabsf(yawDiff) > 10920.0f) {
        return 0;
    }

    xyzDistSq = (this->actor.scale.x / 0.01f) * SQ(100.0f);
    if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90) {
        changeCameraSet(mainCam, CAM_SET_DIRECTED_YAW);
        xyzDistSq *= 4.8f;
    }

    if (fabsf(this->actor.xyzDistToPlayerSq) > xyzDistSq) {
        if (mainCam->setting == CAM_SET_DIRECTED_YAW) {
            changeCameraSet(mainCam, CAM_SET_NORMAL0);
        }
        return 0;
    } else {
        return 1;
    }
}

void go_anime_revers(EnGo* this) {
    f32 startFrame = this->skelAnime.startFrame;

    this->skelAnime.startFrame = this->skelAnime.endFrame;
    this->skelAnime.endFrame = startFrame;
}

void go_shadow_control(EnGo* this) {
    s16 shadowAlpha;
    f32 currentFrame = this->skelAnime.curFrame;
    s16 shadowAlphaTarget = (this->skelAnime.animation == &gGoronAnim_004930 && currentFrame > 32.0f) ||
                                    this->skelAnime.animation != &gGoronAnim_004930
                                ? 255
                                : 0;

    shadowAlpha = this->actor.shape.shadowAlpha;
    add_calc_short_angle2(&shadowAlpha, shadowAlphaTarget, 10, 60, 1);
    this->actor.shape.shadowAlpha = shadowAlpha;
}

static s32 go_path_move(EnGo* this, PlayState* play) {
    Path* path;
    Vec3s* pointPos;
    f32 xDist;
    f32 zDist;

    if (PARAMS_GET_U(this->actor.params, 0, 4) == 15) {
        return false;
    }

    path = &play->pathList[PARAMS_GET_U(this->actor.params, 0, 4)];
    pointPos = SEGMENTED_TO_VIRTUAL(path->points);
    pointPos += this->unk_218;
    xDist = pointPos->x - this->actor.world.pos.x;
    zDist = pointPos->z - this->actor.world.pos.z;
    add_calc_short_angle2(&this->actor.world.rot.y, RAD_TO_BINANG(fatan2(xDist, zDist)), 10, 1000, 1);

    if ((SQ(xDist) + SQ(zDist)) < 600.0f) {
        this->unk_218++;
        if (this->unk_218 >= path->count) {
            this->unk_218 = 0;
        }

        if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) != 0x00) {
            return true;
        } else if (Actor_Environment_sw_Check(play, PARAMS_GET_NOMASK(this->actor.params, 8))) {
            return true;
        } else if (this->unk_218 >= this->actor.shape.rot.z) {
            this->unk_218 = 0;
        }

        return true;
    }

    return false;
}

s32 go_set_path_end_pos(EnGo* this, PlayState* play) {
    Path* path;
    Vec3s* pointPos;

    if (PARAMS_GET_U(this->actor.params, 0, 4) == 0xF) {
        return false;
    } else {
        path = &play->pathList[PARAMS_GET_U(this->actor.params, 0, 4)];
        pointPos = SEGMENTED_TO_VIRTUAL(path->points);
        pointPos += (path->count - 1);
        this->actor.world.pos.x = pointPos->x;
        this->actor.world.pos.y = pointPos->y;
        this->actor.world.pos.z = pointPos->z;
        this->actor.home.pos = this->actor.world.pos;
        return true;
    }
}

static s32 set_dust_effect(EnGo* this, u8 initialTimer, f32 scale, f32 scaleStep, s32 numDustEffects, f32 radius, f32 xzAccel) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    s16 angle;
    s32 i;

    pos = this->actor.world.pos; // Overwrites pos
    pos.y = this->actor.floorHeight;
    angle = (fqrand() - 0.5f) * 0x10000;
    i = numDustEffects;
    while (i >= 0) {
        accel.x = (fqrand() - 0.5f) * xzAccel;
        accel.z = (fqrand() - 0.5f) * xzAccel;
        pos.x = (sin_s(angle) * radius) + this->actor.world.pos.x;
        pos.z = (cos_s(angle) * radius) + this->actor.world.pos.z;
        go_eff_dust_ct(this, &pos, &velocity, &accel, initialTimer, scale, scaleStep);
        angle += (s16)(0x10000 / numDustEffects);
        i--;
    }
    return 0;
}

static s32 go_bound(EnGo* this, s16 unkArg1, f32 unkArg2) {
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || this->actor.velocity.y > 0.0f) {
        return false;
    } else if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        return true;
    } else if (DECR(this->unk_21C)) {
        if (this->unk_21C & 1) {
            this->actor.world.pos.y += 1.5f;
        } else {
            this->actor.world.pos.y -= 1.5f;
        }
        return true;
    } else {
        this->unk_21A--;
        if (this->unk_21A <= 0) {
            if (this->unk_21A == 0) {
                this->unk_21C = get_random_timer(60, 30);
                this->unk_21A = 0;
                this->actor.velocity.y = 0.0f;
                return true;
            }
            this->unk_21A = unkArg1;
        }
        this->actor.velocity.y = ((f32)this->unk_21A / (f32)unkArg1) * unkArg2;
        return true;
    }
}

void go_talk(EnGo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 interactRange;
    s32 dialogStarted;

    if (this->actionFunc == go_stand_wait || this->actionFunc == go_escape ||
        this->actionFunc == go_help || this->actionFunc == go_get_demo_start ||
        this->actionFunc == go_get_demo_end || this->actionFunc == go_carry_request || this->actionFunc == go_carry_end ||
        this->actionFunc == go_guard) {

        interactRange = (this->collider.dim.radius + 30.0f);
        interactRange *= (this->actor.scale.x / 0.01f);
        if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90) {
            interactRange *= 4.8f;
        }

        if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90) {
            dialogStarted = big_go_talk(play, &this->actor, &this->interactInfo.talkState, interactRange,
                                               go_set_message, go_end_message);
        } else {
            dialogStarted = npc_talk(play, &this->actor, &this->interactInfo.talkState, interactRange,
                                              go_set_message, go_end_message);
        }

        if ((PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90) && (dialogStarted == true)) {
            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_BROKEN_GORONS_SWORD) {
                if (Actor_get_item_check(play) == EXCH_ITEM_BROKEN_GORONS_SWORD) {
                    if (GET_INFTABLE(INFTABLE_B4)) {
                        this->actor.textId = 0x3055;
                    } else {
                        this->actor.textId = 0x3054;
                    }
                } else {
                    this->actor.textId = 0x3053;
                }
                player->actor.textId = this->actor.textId;
            }

            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_EYE_DROPS) {
                if (Actor_get_item_check(play) == EXCH_ITEM_EYE_DROPS) {
                    this->actor.textId = 0x3059;
                } else {
                    this->actor.textId = 0x3058;
                }
                player->actor.textId = this->actor.textId;
            }
        }
    }
}

void En_Go_Actor_ct(Actor* thisx, PlayState* play) {
    EnGo* this = (EnGo*)thisx;
    s32 pad;
    Vec3f D_80A41B9C = { 0.0f, 0.0f, 0.0f }; // unused
    Vec3f D_80A41BA8 = { 0.0f, 0.0f, 0.0f }; // unused

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGoronSkel, NULL, NULL, NULL, 0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnGoAtInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(0x16), &GoStatusData);

    if (!go_appearance_check(this, play)) {
        Actor_delete(&this->actor);
        return;
    }

    if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) && (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) != 0x90)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.flags &= ~ACTOR_FLAG_DRAW_CULLING_DISABLED;
    }

    go_anime_ct(this, ENGO_ANIM_0);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    this->actor.gravity = -1.0f;

    switch (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4)) {
        case 0x00:
            Actor_set_scale(&this->actor, 0.008f);
            if (CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_GORON)) {
                go_set_path_end_pos(this, play);
                En_Go_actor_set_process(this, go_wait);
            } else {
                this->actor.shape.yOffset = 1400.0f;
                this->actor.speed = 3.0f;
                En_Go_actor_set_process(this, go_escape);
            }
            break;
        case 0x10:
            this->skelAnime.curFrame = Si2_anime_end_frame(&gGoronAnim_004930);
            Actor_set_scale(&this->actor, 0.01f);
            En_Go_actor_set_process(this, go_help);
            break;
        case 0x40:
            if (GET_INFTABLE(INFTABLE_EB)) {
                go_set_path_end_pos(this, play);
            }
            Actor_set_scale(&this->actor, 0.015f);
            En_Go_actor_set_process(this, go_wait);
            break;
        case 0x30:
            this->actor.shape.yOffset = 1400.0f;
            Actor_set_scale(&this->actor, 0.01f);
            En_Go_actor_set_process(this, go_demo);
            break;
        case 0x90:
            this->actor.attentionRangeType = ATTENTION_RANGE_5;
            Actor_set_scale(&this->actor, 0.16f);
            En_Go_actor_set_process(this, go_wait);
            break;
        case 0x20:
        case 0x50:
        case 0x60:
        case 0x70:
            Actor_set_scale(&this->actor, 0.01f);
            En_Go_actor_set_process(this, go_wait);
            break;
        default:
            Actor_delete(&this->actor);
    }
}

void En_Go_Actor_dt(Actor* thisx, PlayState* play) {
    EnGo* this = (EnGo*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjPipe_dt(play, &this->collider);
}

void go_demo(EnGo* this, PlayState* play) {
    if (!(this->actor.xyzDistToPlayerSq > SQ(1200.0f))) {
        En_Go_actor_set_process(this, go_demo_run);
    }
}

void go_demo_run(EnGo* this, PlayState* play) {
    EnBom* bomb;

    if (DECR(this->unk_20E) == 0) {
        if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
            this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
            play->damagePlayer(play, -4);
            Actor_player_power_damage_set(play, &this->actor, 4.0f, this->actor.yawTowardsPlayer, 6.0f);
            this->unk_20E = 0x10;
        }
    }

    this->actor.speed = 3.0f;
    if ((go_path_move(this, play) == true) && (this->unk_218 == 0)) {
        bomb = (EnBom*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                   this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
        if (bomb != NULL) {
            bomb->timer = 0;
        }

        this->actor.speed = 0.0f;
        En_Go_actor_set_process(this, go_demo_explosion);
    }

    this->actor.shape.rot = this->actor.world.rot;

    if (go_bound(this, 3, 6.0f)) {
        set_dust_effect(this, 12, 0.16f, 0.1f, 1, 10.0f, 20.0f);
    }
}

void go_demo_explosion(EnGo* this, PlayState* play) {
    if (go_bound(this, 3, 6.0f)) {
        if (this->unk_21A == 0) {
            this->actor.shape.yOffset = 0.0f;
            En_Go_actor_set_process(this, go_wait);
        } else {
            set_dust_effect(this, 12, 0.16f, 0.1f, 1, 10.0f, 20.0f);
        }
    }
}

void go_escape(EnGo* this, PlayState* play) {
    if ((go_path_move(this, play) == true) && Actor_Environment_sw_Check(play, PARAMS_GET_NOMASK(this->actor.params, 8)) &&
        (this->unk_218 == 0)) {
        this->actor.speed = 0.0f;
        En_Go_actor_set_process(this, go_demo_explosion);
        SET_INFTABLE(INFTABLE_109);
    }

    this->actor.shape.rot = this->actor.world.rot;

    if (go_bound(this, 3, 6.0f)) {
        set_dust_effect(this, 12, 0.18f, 0.2f, 2, 13.0f, 20.0f);
    }
}

void go_help(EnGo* this, PlayState* play) {
}

void go_wait(EnGo* this, PlayState* play) {
    if ((DECR(this->unk_210) == 0) && go_player_search(this, play)) {
        Nai_FxFlagEntry(NA_SE_EN_GOLON_WAKE_UP, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);

        this->skelAnime.playSpeed = 0.1f;
        this->skelAnime.playSpeed *= PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90 ? 0.5f : 1.0f;

        En_Go_actor_set_process(this, go_wake);
        if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90) {
            makeOnepointDemo(play, 4200, -99, &this->actor, CAM_ID_MAIN);
        }
    }
}

void go_wake(EnGo* this, PlayState* play) {
    f32 frame;

    if (this->skelAnime.playSpeed != 0.0f) {
        add_calc(&this->skelAnime.playSpeed,
                           (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90 ? 0.5f : 1.0f) * 0.5f, 0.1f, 1000.0f,
                           0.1f);
        frame = this->skelAnime.curFrame;
        frame += this->skelAnime.playSpeed;

        if (frame <= 12.0f) {
            return;
        } else {
            this->skelAnime.curFrame = 12.0f;
            this->skelAnime.playSpeed = 0.0f;
            if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) != 0x90) {
                this->unk_212 = 30;
                return;
            }
        }
    }

    if (DECR(this->unk_212) == 0) {
        Nai_FxFlagEntry(NA_SE_EN_GOLON_SIT_DOWN, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        En_Go_actor_set_process(this, go_stand_up);
    } else if (!go_player_search(this, play)) {
        go_anime_revers(this);
        this->skelAnime.playSpeed = 0.0f;
        En_Go_actor_set_process(this, go_change_iwa);
    }
}

void go_change_iwa(EnGo* this, PlayState* play) {
    f32 frame;

    add_calc(&this->skelAnime.playSpeed,
                       (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90 ? 0.5f : 1.0f) * -0.5f, 0.1f, 1000.0f,
                       0.1f);
    frame = this->skelAnime.curFrame;
    frame += this->skelAnime.playSpeed;

    if (!(frame >= 0.0f)) {
        Nai_FxFlagEntry(NA_SE_EN_DODO_M_GND, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        set_dust_effect(this, 10, 0.4f, 0.1f, 16, 26.0f, 2.0f);
        go_anime_revers(this);
        this->skelAnime.playSpeed = 0.0f;
        this->skelAnime.curFrame = 0.0f;
        this->unk_210 = get_random_timer(30, 30);
        En_Go_actor_set_process(this, go_wait);
    }
}

static void go_stand_up(EnGo* this, PlayState* play) {
    f32 lastFrame;
    f32 frame;

    lastFrame = Si2_anime_end_frame(&gGoronAnim_004930);
    add_calc(&this->skelAnime.playSpeed, PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90 ? 0.5f : 1.0f,
                       0.1f, 1000.0f, 0.1f);

    frame = this->skelAnime.curFrame;
    frame += this->skelAnime.playSpeed;

    if (!(frame < lastFrame)) {
        this->skelAnime.curFrame = lastFrame;
        this->skelAnime.playSpeed = 0.0f;
        this->unk_212 = get_random_timer(30, 30);
        if ((PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x40) && !GET_INFTABLE(INFTABLE_EB)) {
            En_Go_actor_set_process(this, go_guard);
        } else {
            En_Go_actor_set_process(this, go_stand_wait);
        }
    }
}

void go_stand_wait(EnGo* this, PlayState* play) {
    if ((PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90) &&
        (this->interactInfo.talkState == NPC_TALK_STATE_ACTION)) {
        if (z_common_data.save.info.playerData.bgsFlag) {
            this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        } else {
            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_EYE_DROPS) {
                go_anime_ct(this, ENGO_ANIM_2);
                this->unk_21E = 100;
                this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
                En_Go_actor_set_process(this, go_get_demo_start);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                z_common_data.subTimerState = SUBTIMER_STATE_OFF;
                makeOnepointDemo(play, 4190, -99, &this->actor, CAM_ID_MAIN);
            } else {
                this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
                En_Go_actor_set_process(this, go_carry_request);
                message_close(play);
                go_carry_request(this, play);
            }
        }
    } else if ((PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0) &&
               (this->interactInfo.talkState == NPC_TALK_STATE_ACTION)) {
        En_Go_actor_set_process(this, go_carry_request);
        play->msgCtx.stateTimer = 4;
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    } else {
        if ((DECR(this->unk_212) == 0) && !go_player_search(this, play)) {
            go_anime_revers(this);
            this->skelAnime.playSpeed = -0.1f;
            this->skelAnime.playSpeed *= PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90 ? 0.5f : 1.0f;
            En_Go_actor_set_process(this, go_sit);
        }
    }
}

void go_sit(EnGo* this, PlayState* play) {
    f32 frame;

    if (this->skelAnime.playSpeed != 0.0f) {
        add_calc(&this->skelAnime.playSpeed,
                           (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90 ? 0.5f : 1.0f) * -1.0f, 0.1f, 1000.0f,
                           0.1f);
        frame = this->skelAnime.curFrame;
        frame += this->skelAnime.playSpeed;
        if (frame >= 12.0f) {
            return;
        } else {
            this->skelAnime.curFrame = 12.0f;
            this->skelAnime.playSpeed = 0.0f;
            if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) != 0x90) {
                this->unk_212 = 30;
                return;
            }
        }
    }

    if (DECR(this->unk_212) == 0) {
        En_Go_actor_set_process(this, go_change_iwa);
    } else if (go_player_search(this, play)) {
        go_anime_revers(this);
        Nai_FxFlagEntry(NA_SE_EN_GOLON_SIT_DOWN, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->skelAnime.playSpeed = 0.0f;
        En_Go_actor_set_process(this, go_stand_up);
    }
}

void go_yokoaruki(EnGo* this, PlayState* play) {
    f32 float1 = ((f32)0x8000 / Si2_anime_end_frame(&gGoronAnim_010590));
    f32 float2 = this->skelAnime.curFrame * float1;

    this->actor.speed = sin_s((s16)float2);
    if (go_path_move(this, play) && this->unk_218 == 0) {
        go_anime_ct(this, ENGO_ANIM_1);
        this->skelAnime.curFrame = Si2_anime_end_frame(&gGoronAnim_004930);
        this->actor.speed = 0.0f;
        En_Go_actor_set_process(this, go_stand_wait);
    }
}

void go_guard(EnGo* this, PlayState* play) {
    if (GET_INFTABLE(INFTABLE_EB)) {
        go_anime_ct(this, ENGO_ANIM_3);
        En_Go_actor_set_process(this, go_yokoaruki);
    } else {
        go_stand_wait(this, play);
    }
}

static void go_carry_request(EnGo* this, PlayState* play) {
    f32 xzDist;
    f32 yDist;
    s32 getItemId;

    if (Actor_carry_check(&this->actor, play)) {
        this->interactInfo.talkState = NPC_TALK_STATE_ACTION;
        this->actor.parent = NULL;
        En_Go_actor_set_process(this, go_carry_end);
    } else {
        this->unk_20C = 0;
        if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x90) {
            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_CLAIM_CHECK) {
                getItemId = GI_SWORD_BIGGORON;
                this->unk_20C = 1;
            }
            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_EYE_DROPS) {
                getItemId = GI_CLAIM_CHECK;
            }
            if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_BROKEN_GORONS_SWORD) {
                getItemId = GI_PRESCRIPTION;
            }
        }

        if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0) {
            getItemId = GI_TUNIC_GORON;
        }

        yDist = fabsf(this->actor.yDistToPlayer) + 1.0f;
        xzDist = this->actor.xzDistToPlayer + 1.0f;
        Actor_carry_request_set2(&this->actor, play, getItemId, xzDist, yDist);
    }
}

static void go_carry_end(EnGo* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ITEM_GIVEN) {
        En_Go_actor_set_process(this, go_stand_wait);
        if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) != 0x90) {
            this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        } else if (this->unk_20C) {
            this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
            z_common_data.save.info.playerData.bgsFlag = true;
        } else if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_PRESCRIPTION) {
            this->actor.textId = 0x3058;
            message_set2(play, this->actor.textId);
            this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
        } else if (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_CLAIM_CHECK) {
            this->actor.textId = 0x305C;
            message_set2(play, this->actor.textId);
            this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
            Clr_Eventday();
        }
    }
}

void go_get_demo_start(EnGo* this, PlayState* play) {
    if (DECR(this->unk_21E) == 0) {
        this->actor.textId = 0x305A;
        message_set2(play, this->actor.textId);
        this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
        En_Go_actor_set_process(this, go_get_demo_end);
    }
}

void go_get_demo_end(EnGo* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        go_anime_ct(this, ENGO_ANIM_1);
        this->skelAnime.curFrame = Si2_anime_end_frame(&gGoronAnim_004930);
        message_close(play);
        En_Go_actor_set_process(this, go_carry_request);
        go_carry_request(this, play);
    }
}

void En_Go_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGo* this = (EnGo*)thisx;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actionFunc == go_stand_wait || this->actionFunc == go_help ||
        this->actionFunc == go_guard) {
        program_wait(play, this->fidgetTableY, this->fidgetTableZ, 18);
    }

    go_shadow_control(this);

    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        Actor_position_moveF(&this->actor);
    }

    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    go_eye_control(this);
    go_talk(this, play);
    this->actionFunc(this, play);
    go_eye_move(this, play);
}

static void go_iwa1_disp(EnGo* this, PlayState* play) {
    Vec3f D_80A41BB4 = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_go.c", 2320);

    Matrix_push();
    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_go.c", 2326);

    gSPDisplayList(POLY_OPA_DISP++, gGoronDL_00BD80);

    Matrix_Position(&D_80A41BB4, &this->actor.focus.pos);
    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_go.c", 2341);
}

static void go_iwa2_disp(EnGo* this, PlayState* play) {
    Vec3f D_80A41BC0 = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_go.c", 2355);

    Matrix_push();
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Matrix_rotateXYZ((s16)(play->state.frames * ((s16)this->actor.speed * 1400)), 0, this->actor.shape.rot.z,
                     MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_go.c", 2368);
    gSPDisplayList(POLY_OPA_DISP++, gGoronDL_00C140);
    Matrix_Position(&D_80A41BC0, &this->actor.focus.pos);
    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_go.c", 2383);
}

static s32 before_display(PlayState* play, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGo* this = (EnGo*)thisx;
    Vec3s limbRot;

    if (limb == 17) {
        Matrix_translate(2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        limbRot = this->interactInfo.headRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_translate(-2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limb == 10) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateY(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
    }

    if ((limb == 10) || (limb == 11) || (limb == 14)) {
        rot->y += sin_s(this->fidgetTableY[limb]) * FIDGET_AMPLITUDE;
        rot->z += cos_s(this->fidgetTableZ[limb]) * FIDGET_AMPLITUDE;
    }

    return 0;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnGo* this = (EnGo*)thisx;
    Vec3f D_80A41BCC = { 600.0f, 0.0f, 0.0f };

    if (limbIndex == 17) {
        Matrix_Position(&D_80A41BCC, &this->actor.focus.pos);
    }
}

void En_Go_Actor_draw(Actor* thisx, PlayState* play) {
    EnGo* this = (EnGo*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_go.c", 2479);

    go_eff_dust_mv(this);
    Matrix_push();
    go_eff_dust_dr(this, play);
    Matrix_pull();

    if (this->actionFunc == go_wait) {
        go_iwa1_disp(this, play);
        return; // needed for match?
    } else if (this->actionFunc == go_escape || this->actionFunc == go_demo ||
               this->actionFunc == go_demo_run || this->actionFunc == go_demo) {
        go_iwa2_disp(this, play);
        return; // needed for match?
    } else {
        _polygon_z_light_fog_prim(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gGoronCsEyeOpenTex));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gGoronCsMouthNeutralTex));

        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              before_display, after_display, &this->actor);
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_go.c", 2525);
        go_eff_dust_dr(this, play);
    }
}

static void go_eff_dust_ct(EnGo* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 initialTimer, f32 scale,
                          f32 scaleStep) {
    EnGoEffect* dustEffect = this->effects;
    s16 i;

    for (i = 0; i < EN_GO_EFFECT_COUNT; i++, dustEffect++) {
        if (dustEffect->type != 1) {
            dustEffect->scale = scale;
            dustEffect->scaleStep = scaleStep;
            dustEffect->initialTimer = dustEffect->timer = initialTimer;
            dustEffect->type = 1;
            dustEffect->pos = *pos;
            dustEffect->accel = *accel;
            dustEffect->velocity = *velocity;
            break;
        }
    }
}

static void go_eff_dust_mv(EnGo* this) {
    EnGoEffect* dustEffect = this->effects;
    f32 randomNumber;
    s16 i;

    for (i = 0; i < EN_GO_EFFECT_COUNT; i++, dustEffect++) {
        if (dustEffect->type) {
            dustEffect->timer--;
            if (dustEffect->timer == 0) {
                dustEffect->type = 0;
            }

            dustEffect->accel.x = (fqrand() * 0.4f) - 0.2f;
            randomNumber = fqrand() * 0.4f;
            dustEffect->accel.z = randomNumber - 0.2f;
            dustEffect->pos.x += dustEffect->velocity.x;
            dustEffect->pos.y += dustEffect->velocity.y;
            dustEffect->pos.z += dustEffect->velocity.z;
            dustEffect->velocity.x += dustEffect->accel.x;
            dustEffect->velocity.y += dustEffect->accel.y;
            dustEffect->velocity.z += randomNumber - 0.2f;
            dustEffect->scale += dustEffect->scaleStep;
        }
    }
}

static void go_eff_dust_dr(EnGo* this, PlayState* play) {
    static void* smoke_txt[] = { gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex, gDust4Tex, gDust3Tex, gDust2Tex, gDust1Tex };
    EnGoEffect* dustEffect = this->effects;
    s16 alpha;
    s16 materialFlag;
    s16 index;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_go.c", 2626);
    materialFlag = false;
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < EN_GO_EFFECT_COUNT; i++, dustEffect++) {
        if (dustEffect->type == 0) {
            continue;
        }

        if (!materialFlag) {
            POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_0);
            gSPDisplayList(POLY_XLU_DISP++, gGoronDL_00FD40);
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 60, 20, 0);
            materialFlag = true;
        }

        alpha = dustEffect->timer * (255.0f / dustEffect->initialTimer);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 130, 90, alpha);
        gDPPipeSync(POLY_XLU_DISP++);
        Matrix_translate(dustEffect->pos.x, dustEffect->pos.y, dustEffect->pos.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(dustEffect->scale, dustEffect->scale, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_go.c", 2664);

        index = dustEffect->timer * (8.0f / dustEffect->initialTimer);
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(smoke_txt[index]));
        gSPDisplayList(POLY_XLU_DISP++, gGoronDL_00FD50);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_go.c", 2678);
}
