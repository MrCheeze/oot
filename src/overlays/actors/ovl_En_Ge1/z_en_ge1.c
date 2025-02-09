/*
 * File: z_en_ge1.c
 * Overlay: ovl_En_Ge1
 * Description: White-clothed Gerudo
 */

#include "z_en_ge1.h"
#include "z64horse.h"
#include "terminal.h"
#include "assets/objects/object_ge1/object_ge1.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

#define GE1_STATE_TALKING (1 << 0)
#define GE1_STATE_GIVE_QUIVER (1 << 1)
#define GE1_STATE_IDLE_ANIM (1 << 2)
#define GE1_STATE_STOP_FIDGET (1 << 3)

typedef enum EnGe1Hairstyle {
    /* 00 */ GE1_HAIR_BOB,
    /* 01 */ GE1_HAIR_STRAIGHT,
    /* 02 */ GE1_HAIR_SPIKY
} EnGe1Hairstyle;

void En_Ge1_Actor_ct(Actor* thisx, PlayState* play);
void En_Ge1_Actor_dt(Actor* thisx, PlayState* play);
void En_Ge1_Actor_move(Actor* thisx, PlayState* play);
void En_Ge1_Actor_draw(Actor* thisx, PlayState* play);

s32 ge1_event_check_nakanaori(void);
void move_stand5(EnGe1* this, PlayState* play);
void move_stand6(EnGe1* this, PlayState* play);
void move_stand7(EnGe1* this, PlayState* play);
void move_stand8(EnGe1* this, PlayState* play);
void move_stand3(EnGe1* this, PlayState* play);
void move_stand4(EnGe1* this, PlayState* play);
void move_stand2(EnGe1* this, PlayState* play);
void move_stand1(EnGe1* this, PlayState* play);
void move_stand0(EnGe1* this, PlayState* play);
static void normal_anime_proc(EnGe1* this);
static void stop_anime_proc(EnGe1* this);

ActorProfile En_Ge1_Profile = {
    /**/ ACTOR_EN_GE1,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GE1,
    /**/ sizeof(EnGe1),
    /**/ En_Ge1_Actor_ct,
    /**/ En_Ge1_Actor_dt,
    /**/ En_Ge1_Actor_move,
    /**/ En_Ge1_Actor_draw,
};

static ColliderCylinderInit EnGe1OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000702, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

static Gfx* zura[] = {
    gGerudoWhiteHairstyleBobDL,
    gGerudoWhiteHairstyleStraightFringeDL,
    gGerudoWhiteHairstyleSpikyDL,
};

static Vec3f pos = { 600.0f, 700.0f, 0.0f };

static void* eye_txt[] = {
    gGerudoWhiteEyeOpenTex,
    gGerudoWhiteEyeHalfTex,
    gGerudoWhiteEyeClosedTex,
};

void En_Ge1_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGe1* this = (EnGe1*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGerudoWhiteSkel, &gGerudoWhiteIdleAnim, this->jointTable,
                       this->morphTable, GE1_LIMB_MAX);
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gGerudoWhiteIdleAnim);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnGe1OcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->animation = &gGerudoWhiteIdleAnim;
    this->animFunc = normal_anime_proc;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    Actor_set_scale(&this->actor, 0.01f);

    this->actor.cullingVolumeDistance = ((play->sceneId == SCENE_GERUDO_VALLEY) ? 1000.0f : 1200.0f);

    switch (PARAMS_GET_U(this->actor.params, 0, 8)) {

        case GE1_TYPE_GATE_GUARD:
            this->hairstyle = GE1_HAIR_SPIKY;
            this->actionFunc = move_stand2;
            break;

        case GE1_TYPE_GATE_OPERATOR:
            this->hairstyle = GE1_HAIR_STRAIGHT;

            if (ge1_event_check_nakanaori()) {
                this->actionFunc = move_stand4;
            } else {
                this->actionFunc = move_stand5;
            }
            break;

        case GE1_TYPE_NORMAL:
            this->hairstyle = GE1_HAIR_STRAIGHT;

            if (ge1_event_check_nakanaori()) {
                this->actionFunc = move_stand6;
            } else {
                this->actionFunc = move_stand7;
            }
            break;

        case GE1_TYPE_VALLEY_FLOOR:
            if (LINK_IS_ADULT) {
                // "Valley floor Gerudo withdrawal"
                PRINTF(VT_FGCOL(CYAN) "谷底 ゲルド 撤退 \n" VT_RST);
                Actor_delete(&this->actor);
                return;
            }
            this->hairstyle = GE1_HAIR_BOB;
            this->actionFunc = move_stand8;
            break;

        case GE1_TYPE_HORSEBACK_ARCHERY:
            if (INV_CONTENT(SLOT_BOW) == ITEM_NONE) {
                Actor_delete(&this->actor);
                return;
            }
            this->actor.attentionRangeType = ATTENTION_RANGE_3;
            this->hairstyle = GE1_HAIR_BOB;
            // "Horseback archery Gerudo EVENT_INF(0) ="
            PRINTF(VT_FGCOL(CYAN) "やぶさめ ゲルド EVENT_INF(0) = %x\n" VT_RST, z_common_data.eventInf[0]);

            if (GET_EVENTINF(EVENTINF_HORSES_08)) {
                this->actionFunc = move_stand1;
            } else if (ge1_event_check_nakanaori()) {
                this->actionFunc = move_stand0;
            } else {
                this->actionFunc = move_stand5;
            }
            break;

        case GE1_TYPE_TRAINING_GROUNDS_GUARD:
            this->hairstyle = GE1_HAIR_STRAIGHT;

            if (ge1_event_check_nakanaori()) {
                this->actionFunc = move_stand3;
            } else {
                this->actionFunc = move_stand5;
            }
            break;
    }

    this->stateFlags = 0;
}

void En_Ge1_Actor_dt(Actor* thisx, PlayState* play) {
    EnGe1* this = (EnGe1*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static s32 kihon_process(EnGe1* this, PlayState* play, u16 textId, f32 arg3, EnGe1ActionFunc actionFunc) {
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = actionFunc;
        this->animFunc = stop_anime_proc;
        this->stateFlags &= ~GE1_STATE_IDLE_ANIM;
        this->animation = &gGerudoWhiteIdleAnim;
        Skeleton_Info2_init(&this->skelAnime, &gGerudoWhiteIdleAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gGerudoWhiteIdleAnim), ANIMMODE_ONCE, -8.0f);
        return true;
    }

    this->actor.textId = textId;

    if (this->actor.xzDistToPlayer < arg3) {
        Actor_talk_request2(&this->actor, play, arg3);
    }

    return false;
}

void return_talk_set(EnGe1* this) {
    Skeleton_Info2_init(&this->skelAnime, &gGerudoWhiteIdleAnim, -1.0f, Si2_anime_end_frame(&gGerudoWhiteIdleAnim),
                     0.0f, ANIMMODE_ONCE, 8.0f);
    this->animation = &gGerudoWhiteIdleAnim;
    this->animFunc = normal_anime_proc;
}

s32 ge1_event_check_nakanaori(void) {
    if (!(GET_EVENTCHKINF(EVENTCHKINF_CARPENTER_0_RESCUED) && GET_EVENTCHKINF(EVENTCHKINF_CARPENTER_1_RESCUED) &&
          GET_EVENTCHKINF(EVENTCHKINF_CARPENTER_2_RESCUED) && GET_EVENTCHKINF(EVENTCHKINF_CARPENTER_3_RESCUED))) {
        return false;
    }
    return true;
}

/**
 * Sends player to different places depending on if has hookshot, and if this is the first time captured
 */
void ge1_catch(EnGe1* this, PlayState* play) {
    this->stateFlags |= GE1_STATE_TALKING;

    if (this->cutsceneTimer > 0) {
        this->cutsceneTimer--;
    } else {
        Horse_goto_Default(play);

        if ((INV_CONTENT(ITEM_HOOKSHOT) == ITEM_NONE) || (INV_CONTENT(ITEM_LONGSHOT) == ITEM_NONE)) {
            play->nextEntranceIndex = ENTR_GERUDO_VALLEY_1;
        } else if (GET_EVENTCHKINF(EVENTCHKINF_C7)) { // Caught previously
            play->nextEntranceIndex = ENTR_GERUDOS_FORTRESS_18;
        } else {
            play->nextEntranceIndex = ENTR_GERUDOS_FORTRESS_17;
        }

        play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST);
        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

static void tocatch(EnGe1* this, PlayState* play) {
    this->cutsceneTimer = 30;
    this->actionFunc = ge1_catch;
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_95);
    Na_StartSystemSe_F(NA_SE_SY_FOUND);
    message_set(play, 0x6000, &this->actor);
}

void move_stand5(EnGe1* this, PlayState* play) {
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((ABS(angleDiff) <= 0x4300) && (this->actor.xzDistToPlayer < 100.0f)) {
        tocatch(this, play);
    }

    if (this->collider.base.acFlags & AC_HIT) {
        tocatch(this, play);
    }

    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
}

void talk_end(EnGe1* this, PlayState* play) {
    this->stateFlags |= GE1_STATE_TALKING;

    if (Actor_talk_end_check(&this->actor, play)) {
        switch (this->actor.textId) {
            case 0x6001:
                this->actionFunc = move_stand6;
                break;

            case 0x601A:
            case 0x6019:
                this->actionFunc = move_stand8;
                break;

            case 0x6018:
                this->actionFunc = move_stand4;
                break;

            default:
                this->actionFunc = move_stand8;
                break;
        }
    }
}

void move_stand6(EnGe1* this, PlayState* play) {
    kihon_process(this, play, 0x6001, 100.0f, talk_end);
}

void move_stand7(EnGe1* this, PlayState* play) {
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((this->actor.xzDistToPlayer < 50.0f) || ((ABS(angleDiff) <= 0x4300) && (this->actor.xzDistToPlayer < 400.0f))) {
        tocatch(this, play);
    }

    if (this->collider.base.acFlags & AC_HIT) {
        tocatch(this, play);
    }
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
}

void move_stand8(EnGe1* this, PlayState* play) {
    u16 textId = get_mask_message(play, MASK_REACTION_SET_GERUDO_WHITE);

    if (textId == 0) {
        textId = 0x6019;
    }

    kihon_process(this, play, textId, 100.0f, talk_end);
}

// Gerudo Training Ground Guard functions

static void move_wait(EnGe1* this, PlayState* play) {
    if (this->cutsceneTimer > 0) {
        this->cutsceneTimer--;
    } else {
        return_talk_set(this);
        this->actionFunc = move_stand6;
    }

    this->stateFlags |= GE1_STATE_STOP_FIDGET;
}

void move_aizu(EnGe1* this, PlayState* play) {
    if (this->stateFlags & GE1_STATE_IDLE_ANIM) {
        this->actionFunc = move_wait;
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->actor.params, 8, 6));
        this->cutsceneTimer = 50;
        message_close(play);
    } else if ((this->skelAnime.curFrame == 15.0f) || (this->skelAnime.curFrame == 19.0f)) {
        Actor_SE_set(&this->actor, NA_SE_IT_HAND_CLAP);
    }
}

void talk3_3(EnGe1* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        this->actionFunc = move_aizu;
        Skeleton_Info2_init(&this->skelAnime, &gGerudoWhiteClapAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gGerudoWhiteClapAnim), ANIMMODE_ONCE, -3.0f);
        this->animation = &gGerudoWhiteClapAnim;
        this->animFunc = stop_anime_proc;
        this->stateFlags &= ~GE1_STATE_IDLE_ANIM;
    }
}

void talk3_2(EnGe1* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actionFunc = move_stand3;
        return_talk_set(this);
    }
}

void talk3_1(EnGe1* this, PlayState* play) {
    this->stateFlags |= GE1_STATE_TALKING;
    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        message_close(play);

        switch (play->msgCtx.choiceIndex) {
            case 0:
                if (z_common_data.save.info.playerData.rupees < 10) {
                    message_set2(play, 0x6016);
                    this->actionFunc = talk3_2;
                } else {
                    lupy_increase(-10);
                    message_set2(play, 0x6015);
                    this->actionFunc = talk3_3;
                }
                break;
            case 1:
                this->actionFunc = move_stand3;
                return_talk_set(this);
                break;
        }
    }
}

void talk3_0(EnGe1* this, PlayState* play) {
    this->stateFlags |= GE1_STATE_TALKING;
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = move_stand3;
        return_talk_set(this);
    }
}

void move_stand3(EnGe1* this, PlayState* play) {
    if (CHECK_QUEST_ITEM(QUEST_GERUDOS_CARD)) {
        kihon_process(this, play, 0x6014, 100.0f, talk3_1);
    } else {
        //! @bug This outcome is inaccessible in normal gameplay since this function it is unreachable without
        //! obtaining the card in the first place.
        kihon_process(this, play, 0x6013, 100.0f, talk3_0);
    }
}

// Gate Operator functions

void talk4_end(EnGe1* this, PlayState* play) {
    this->stateFlags |= GE1_STATE_TALKING;

    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        this->actionFunc = move_stand4;
        return_talk_set(this);
    }
}

void move_wait4(EnGe1* this, PlayState* play) {
    if (this->cutsceneTimer > 0) {
        this->cutsceneTimer--;
    } else {
        return_talk_set(this);
        this->actionFunc = move_stand4;
    }
    this->stateFlags |= GE1_STATE_STOP_FIDGET;
}

void move_aizu4(EnGe1* this, PlayState* play) {
    if (this->stateFlags & GE1_STATE_IDLE_ANIM) {
        this->actionFunc = move_wait4;
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->actor.params, 8, 6));
        this->cutsceneTimer = 50;
        message_close(play);
    } else if ((this->skelAnime.curFrame == 15.0f) || (this->skelAnime.curFrame == 19.0f)) {
        Actor_SE_set(&this->actor, NA_SE_IT_HAND_CLAP);
    }
}

void talk4_0(EnGe1* this, PlayState* play) {
    this->stateFlags |= GE1_STATE_TALKING;

    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        this->actionFunc = move_aizu4;
        Skeleton_Info2_init(&this->skelAnime, &gGerudoWhiteClapAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gGerudoWhiteClapAnim), ANIMMODE_ONCE, -3.0f);
        this->animation = &gGerudoWhiteClapAnim;
        this->animFunc = stop_anime_proc;
        this->stateFlags &= ~GE1_STATE_IDLE_ANIM;
    }
}

void move_stand4(EnGe1* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
        kihon_process(this, play, 0x6018, 100.0f, talk4_end);
    } else {
        kihon_process(this, play, 0x6017, 100.0f, talk4_0);
    }
}

// Gate guard functions

void talk2(EnGe1* this, PlayState* play) {
    this->stateFlags |= GE1_STATE_TALKING;

    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = move_stand2;
        return_talk_set(this);
    }
}

void move_stand2(EnGe1* this, PlayState* play) {
    u16 textId = get_mask_message(play, MASK_REACTION_SET_GERUDO_WHITE);

    if (textId == 0) {
        textId = 0x6069;
    }

    if (kihon_process(this, play, textId, 100.0f, talk2)) {
        this->animFunc = normal_anime_proc;
        this->animation = &gGerudoWhiteDismissiveAnim;
        Skeleton_Info2_init(&this->skelAnime, &gGerudoWhiteDismissiveAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gGerudoWhiteDismissiveAnim), ANIMMODE_ONCE, -8.0f);
    }
}

// Archery functions

void talk1_5(EnGe1* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = move_stand0;
        return_talk_set(this);
    }
}

void carry1(EnGe1* this, PlayState* play) {
    s32 getItemId;

    if (Actor_carry_check(&this->actor, play)) {
        this->actionFunc = talk1_5;
        if (this->stateFlags & GE1_STATE_GIVE_QUIVER) {
            SET_ITEMGETINF(ITEMGETINF_0F);
        } else {
            SET_INFTABLE(INFTABLE_190);
        }
    } else {
        if (this->stateFlags & GE1_STATE_GIVE_QUIVER) {
            switch (CUR_UPG_VALUE(UPG_QUIVER)) {
                //! @bug Asschest. See next function for details
                case 1:
                    getItemId = GI_QUIVER_40;
                    break;
                case 2:
                    getItemId = GI_QUIVER_50;
                    break;
            }
        } else {
            getItemId = GI_HEART_PIECE;
        }
        Actor_carry_request_set2(&this->actor, play, getItemId, 10000.0f, 50.0f);
    }
}

void talk1_4(EnGe1* this, PlayState* play) {
    s32 getItemId;

    if (Actor_talk_end_check(&this->actor, play)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = carry1;
    }

    if (this->stateFlags & GE1_STATE_GIVE_QUIVER) {
        switch (CUR_UPG_VALUE(UPG_QUIVER)) {
            //! @bug Asschest: the compiler inserts a default assigning *(sp+0x24) to getItemId, which is junk data left
            //! over from the previous function run in En_Ge1_Actor_move, namely normal_anime_proc. The top stack variable
            //! in that function is &this->skelAnime = thisx + 198, and depending on where this loads in memory, the
            //! getItemId changes.
            case 1:
                getItemId = GI_QUIVER_40;
                break;
            case 2:
                getItemId = GI_QUIVER_50;
                break;
        }
    } else {
        getItemId = GI_HEART_PIECE;
    }

    Actor_carry_request_set2(&this->actor, play, getItemId, 10000.0f, 50.0f);
}

void talk1_3(EnGe1* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = talk1_4;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else {
        Actor_talk_request2(&this->actor, play, 200.0f);
    }
}

void talk1_end(EnGe1* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        this->actionFunc = move_stand0;
        return_talk_set(this);
    }
}

static void wait(EnGe1* this, PlayState* play) {
}

void talk1_2(EnGe1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* horse;

    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;

        switch (play->msgCtx.choiceIndex) {
            case 0:
                if (z_common_data.save.info.playerData.rupees < 20) {
                    message_set2(play, 0x85);
                    this->actionFunc = talk1_end;
                } else {
                    lupy_increase(-20);
                    play->nextEntranceIndex = ENTR_GERUDOS_FORTRESS_0;
                    z_common_data.nextCutsceneIndex = 0xFFF0;
                    play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST);
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    SET_EVENTINF(EVENTINF_HORSES_08);
                    SET_EVENTCHKINF(EVENTCHKINF_68);

                    if (!(player->stateFlags1 & PLAYER_STATE1_23)) {
                        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
                    } else {
                        horse = ActorSearch(play, &player->actor, ACTOR_EN_HORSE, ACTORCAT_BG, 1200.0f);
                        player->actor.freezeTimer = 1200;

                        if (horse != NULL) {
                            horse->freezeTimer = 1200;
                        }
                    }

                    this->actionFunc = wait;
                }
                break;

            case 1:
                this->actionFunc = move_stand0;
                message_close(play);
                break;
        }
    }
}

void talk1_1(EnGe1* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_set2(play, 0x6041);
        this->actionFunc = talk1_2;
    }
}

void talk1_0(EnGe1* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = talk1_1;
    } else {
        Actor_talk_request2(&this->actor, play, 300.0f);
    }
}

void move_stand1(EnGe1* this, PlayState* play) {
    CLEAR_EVENTINF(EVENTINF_HORSES_08);
    LOG_NUM("z_common_data.yabusame_total", z_common_data.minigameScore, "../z_en_ge1.c", 1110);
    // With the current `SaveContext` struct definition, the expression in the debug string is an out-of-bounds read,
    // see the other occurrence of this for more details.
    LOG_NUM("z_common_data.memory.information.room_inf[127][ 0 ]", HIGH_SCORE(HS_HBA), "../z_en_ge1.c", 1111);
    this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;

    if (HIGH_SCORE(HS_HBA) < z_common_data.minigameScore) {
        HIGH_SCORE(HS_HBA) = z_common_data.minigameScore;
    }

    if (z_common_data.minigameScore < 1000) {
        this->actor.textId = 0x6045;
        this->actionFunc = talk1_0;
    } else if (!GET_INFTABLE(INFTABLE_190)) {
        this->actor.textId = 0x6046;
        this->actionFunc = talk1_3;
        this->stateFlags &= ~GE1_STATE_GIVE_QUIVER;
    } else if (z_common_data.minigameScore < 1500) {
        this->actor.textId = 0x6047;
        this->actionFunc = talk1_0;
    } else if (GET_ITEMGETINF(ITEMGETINF_0F)) {
        this->actor.textId = 0x6047;
        this->actionFunc = talk1_0;
    } else {
        this->actor.textId = 0x6044;
        this->actionFunc = talk1_3;
        this->stateFlags |= GE1_STATE_GIVE_QUIVER;
    }
}

void talk0(EnGe1* this, PlayState* play) {
    this->stateFlags |= GE1_STATE_TALKING;
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = move_stand0;
        return_talk_set(this);
    }
}

void move_stand0(EnGe1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u16 textId;

    if (!(player->stateFlags1 & PLAYER_STATE1_23)) {
        kihon_process(this, play, 0x603F, 100.0f, talk0);
    } else {
        if (GET_EVENTCHKINF(EVENTCHKINF_68)) {
            if (GET_INFTABLE(INFTABLE_190)) {
                textId = 0x6042;
            } else {
                textId = 0x6043;
            }
        } else {
            textId = 0x6040;
        }
        kihon_process(this, play, textId, 200.0f, talk1_1);
    }
}

// General functions

void local_search_furimuki(EnGe1* this, PlayState* play) {
    s32 pad;
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(angleDiff) <= 0x4000) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, 4000, 100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        eye_move2(play, &this->actor, &this->headRot, &this->unk_2A2, this->actor.focus.pos);
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

void local_search_normal(EnGe1* this, PlayState* play) {
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if ((ABS(angleDiff) <= 0x4300) && (this->actor.xzDistToPlayer < 100.0f)) {
        eye_move2(play, &this->actor, &this->headRot, &this->unk_2A2, this->actor.focus.pos);
    } else {
        add_calc_short_angle2(&this->headRot.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->headRot.y, 0, 6, 6200, 100);
    }
}

void En_Ge1_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGe1* this = (EnGe1*)thisx;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 40.0f, 25.0f, 40.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    this->animFunc(this);
    this->actionFunc(this, play);

    if (this->stateFlags & GE1_STATE_TALKING) {
        local_search_furimuki(this, play);
        this->stateFlags &= ~GE1_STATE_TALKING;
    } else {
        local_search_normal(this, play);
    }
    this->unk_2A2.x = this->unk_2A2.y = this->unk_2A2.z = 0;

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = get_random_timer(60, 60);
    }
    this->eyeIndex = this->blinkTimer;

    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

// Animation functions

static void normal_anime_proc(EnGe1* this) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        Skeleton_Info2_init_standard_stop(&this->skelAnime, this->animation);
    }
}

static void stop_anime_proc(EnGe1* this) {
    if (!(this->stateFlags & GE1_STATE_IDLE_ANIM)) {
        if (Skeleton_Info2_anime_play(&this->skelAnime)) {
            this->stateFlags |= GE1_STATE_IDLE_ANIM;
        }
        this->stateFlags |= GE1_STATE_STOP_FIDGET;
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    s32 pad;
    EnGe1* this = (EnGe1*)thisx;

    if (limbIndex == GE1_LIMB_HEAD) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
    }

    if (this->stateFlags & GE1_STATE_STOP_FIDGET) {
        this->stateFlags &= ~GE1_STATE_STOP_FIDGET;
        return 0;
    }

    // The purpose of the state flag GE1_STATE_STOP_FIDGET is to skip this code, which this actor has in lieu of an idle
    // animation.
    if ((limbIndex == GE1_LIMB_TORSO) || (limbIndex == GE1_LIMB_L_FOREARM) || (limbIndex == GE1_LIMB_R_FOREARM)) {
        rot->y += sin_s(play->state.frames * (limbIndex * FIDGET_FREQ_LIMB + FIDGET_FREQ_Y)) * FIDGET_AMPLITUDE;
        rot->z += cos_s(play->state.frames * (limbIndex * FIDGET_FREQ_LIMB + FIDGET_FREQ_Z)) * FIDGET_AMPLITUDE;
    }
    return 0;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnGe1* this = (EnGe1*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ge1.c", 1419);

    if (limbIndex == GE1_LIMB_HEAD) {
        gSPDisplayList(POLY_OPA_DISP++, zura[this->hairstyle]);
        Matrix_Position(&pos, &this->actor.focus.pos);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ge1.c", 1427);
}

void En_Ge1_Actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGe1* this = (EnGe1*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ge1.c", 1442);

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeIndex]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ge1.c", 1459);
}
