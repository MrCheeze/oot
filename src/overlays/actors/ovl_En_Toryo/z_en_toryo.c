/*
 * File: z_en_tory.c
 * Overlay: ovl_En_Toryo
 * Description: Boss Carpenter
 */

#include "z_en_toryo.h"
#include "assets/objects/object_toryo/object_toryo.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Toryo_Actor_ct(Actor* thisx, PlayState* play);
void En_Toryo_Actor_dt(Actor* thisx, PlayState* play);
void En_Toryo_Actor_move(Actor* thisx, PlayState* play);
void En_Toryo_Actor_draw(Actor* thisx, PlayState* play);

static void mode_move_wait(EnToryo* this, PlayState* play);
static s32 func_before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
static void func_after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx);

ActorProfile En_Toryo_Profile = {
    /**/ ACTOR_EN_TORYO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TORYO,
    /**/ sizeof(EnToryo),
    /**/ En_Toryo_Actor_ct,
    /**/ En_Toryo_Actor_dt,
    /**/ En_Toryo_Actor_move,
    /**/ En_Toryo_Actor_draw,
};

static ColliderCylinderInit atinfodata = {
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
    { 18, 63, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 statusdata = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable btldata = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static AnimationSpeedInfo anime_ct_data = { &object_toryo_Anim_000E50, 1.0f, 0, 0 };

static Vec3f pos = { 800.0f, 1000.0f, 0.0f };

void En_Toryo_Actor_ct(Actor* thisx, PlayState* play) {
    EnToryo* this = (EnToryo*)thisx;
    s32 pad;

    switch (play->sceneId) {
        case SCENE_GERUDO_VALLEY:
            if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
                this->stateFlags |= 1;
            }
            break;
        case SCENE_KAKARIKO_VILLAGE:
            if ((LINK_AGE_IN_YEARS == YEARS_CHILD) && IS_DAY) {
                this->stateFlags |= 2;
            }
            break;
        case SCENE_KAKARIKO_CENTER_GUEST_HOUSE:
            if ((LINK_AGE_IN_YEARS == YEARS_CHILD) && IS_NIGHT) {
                this->stateFlags |= 4;
            }
            break;
    }

    if ((this->stateFlags & 7) == 0) {
        Actor_delete(&this->actor);
    }

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 42.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_toryo_Skel_007150, NULL, this->jointTable, this->morphTable, 17);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &atinfodata);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, &btldata, &statusdata);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    Skeleton_Info2_init(&this->skelAnime, anime_ct_data.animation, 1.0f, 0.0f,
                     Si2_anime_end_frame(anime_ct_data.animation), anime_ct_data.mode,
                     anime_ct_data.morphFrames);
    this->stateFlags |= 8;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actionFunc = mode_move_wait;
}

void En_Toryo_Actor_dt(Actor* thisx, PlayState* play) {
    EnToryo* this = (EnToryo*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

s32 toryo_endmsg_chk(EnToryo* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 ret = 1;

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_EVENT:
            ret = 1;
            break;
        case TEXT_STATE_CHOICE:
            if (pad_on_check(play)) {
                if (play->msgCtx.choiceIndex == 0) {
                    message_close(play);
                    this->actor.parent = NULL;
                    player->exchangeItemId = EXCH_ITEM_NONE;
                    play->msgCtx.msgMode = MSGMODE_PAUSED;
                    this->actor.textId = 0x601B;
                    ret = 3;
                } else {
                    this->actor.textId = 0x606F;
                    ret = 2;
                }
            }
            break;
        case TEXT_STATE_DONE:
            switch (this->actor.textId) {
                case 0x5028:
                    ret = 1;
                    if (pad_on_check(play)) {
                        SET_INFTABLE(INFTABLE_172);
                        ret = 0;
                    }
                    break;
                case 0x601B:
                    ret = 1;
                    if (pad_on_check(play)) {
                        ret = 4;
                    }
                    break;
                case 0x606F:
                    ret = 1;
                    if (pad_on_check(play)) {
                        SET_INFTABLE(INFTABLE_171);
                        ret = 0;
                    }
                    break;
                case 0x606A:
                    ret = 1;
                    if (pad_on_check(play)) {
                        SET_INFTABLE(INFTABLE_170);
                        ret = 0;
                    }
                    break;
                case 0x606B:
                case 0x606C:
                case 0x606D:
                case 0x606E:
                default:
                    ret = 1;
                    if (pad_on_check(play)) {
                        ret = 0;
                    }
                    break;
            }
            break;
    }
    return ret;
}

s32 toryo_endcry_chk(EnToryo* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 ret = 5;

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            ret = 5;
            break;
        case TEXT_STATE_DONE:
            if (pad_on_check(play)) {
                ret = 0;
            }
            break;
    }
    return ret;
}

u32 toryo_set_msg_get_item(EnToryo* this, PlayState* play) {
    u32 ret;

    if (this->exchangeItemId != EXCH_ITEM_NONE) {
        if (this->exchangeItemId == EXCH_ITEM_POACHERS_SAW) {
            Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
            if (GET_INFTABLE(INFTABLE_171)) {
                ret = 0x606E;
            } else {
                ret = 0x606D;
            }
        } else {
            ret = 0x200F;
        }
    }
    //! @bug return value may be uninitialized
    return ret;
}

s32 toryo_set_msg(EnToryo* this, PlayState* play) {
    s32 textId = get_mask_message(play, MASK_REACTION_SET_CARPENTER_BOSS);
    s32 ret = textId;

    if (textId == 0) {
        if (this->stateFlags & 1) {
            if (GET_EVENTCHKINF_CARPENTERS_ALL_RESCUED()) {
                ret = 0x606C;
            } else if (GET_INFTABLE(INFTABLE_170)) {
                ret = 0x606B;
            } else {
                ret = 0x606A;
            }
        } else if (this->stateFlags & 2) {
            if (GET_INFTABLE(INFTABLE_172)) {
                ret = 0x5029;
            } else {
                ret = 0x5028;
            }
        } else {
            ret = textId;
            if (this->stateFlags & 4) {
                ret = 0x506C;
            }
        }
    }
    return ret;
}

static void func_talk(EnToryo* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 posX;
    s16 posY;

    if (this->messageState == 3) {
        Actor_talk_check(&this->actor, play);
        message_set2(play, this->actor.textId);
        this->messageState = 1;
    }

    if (this->messageState == 1) {
        this->messageState = toryo_endmsg_chk(this, play);
    }

    if (this->messageState == 5) {
        this->messageState = toryo_endcry_chk(this, play);
        return;
    }

    if (this->messageState == 2) {
        message_set2(play, this->actor.textId);
        this->messageState = 1;
    }

    if (this->messageState == 4) {
        if (Actor_carry_check(&this->actor, play)) {
            this->actor.parent = NULL;
            this->messageState = 5;
        } else {
            Actor_carry_request_set2(&this->actor, play, GI_BROKEN_GORONS_SWORD, 100.0f, 10.0f);
        }
        return;
    }

    if (this->messageState == 0) {
        if (Actor_talk_check(&this->actor, play)) {
            this->exchangeItemId = Actor_get_item_check(play);
            if (this->exchangeItemId != EXCH_ITEM_NONE) {
                player->actor.textId = toryo_set_msg_get_item(this, play);
                this->actor.textId = player->actor.textId;
            }
            this->messageState = 1;
            return;
        }

        Actor_display_position_set(play, &this->actor, &posX, &posY);
        if ((posX >= 0) && (posX <= SCREEN_WIDTH) && (posY >= 0) && (posY <= SCREEN_HEIGHT)) {
            this->actor.textId = toryo_set_msg(this, play);
            Actor_talk_request_get_item(&this->actor, play, 100.0f, EXCH_ITEM_POACHERS_SAW);
        }
    }
}

static void mode_move_wait(EnToryo* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    func_talk(this, play);
    if (this->messageState != 0) {
        this->stateFlags |= 0x10;
    } else {
        this->stateFlags &= ~0x10;
    }
}

void En_Toryo_Actor_move(Actor* thisx, PlayState* play) {
    EnToryo* this = (EnToryo*)thisx;
    ColliderCylinder* collider = &this->collider;
    Player* player = GET_PLAYER(play);
    f32 rot;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, collider);
    CollisionCheck_setOC(play, &play->colChkCtx, (Collider*)collider);

    this->actionFunc(this, play);

    if (this->stateFlags & 8) {
        this->interactInfo.trackPos.x = player->actor.focus.pos.x;
        this->interactInfo.trackPos.y = player->actor.focus.pos.y;
        this->interactInfo.trackPos.z = player->actor.focus.pos.z;

        if (this->stateFlags & 0x10) {
            eye_moveM(thisx, &this->interactInfo, 0, NPC_TRACKING_FULL_BODY);
            return;
        }

        rot = thisx->yawTowardsPlayer - thisx->shape.rot.y;
        if ((rot < DEG_TO_BINANG2(80.0f)) && (rot > DEG_TO_BINANG2(-80.0f))) {
            eye_moveM(thisx, &this->interactInfo, 0, NPC_TRACKING_HEAD_AND_TORSO);
        } else {
            eye_moveM(thisx, &this->interactInfo, 0, NPC_TRACKING_NONE);
        }
    }
}

void En_Toryo_Actor_draw(Actor* thisx, PlayState* play) {
    EnToryo* this = (EnToryo*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          func_before_display, func_after_display, this);
}

static s32 func_before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnToryo* this = (EnToryo*)thisx;

    if (this->stateFlags & 8) {
        switch (limbIndex) {
            case 8:
                rot->x += this->interactInfo.torsoRot.y;
                rot->y -= this->interactInfo.torsoRot.x;
                break;
            case 15:
                rot->x += this->interactInfo.headRot.y;
                rot->z += this->interactInfo.headRot.x;
                break;
        }
    }
    return 0;
}

static void func_after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnToryo* this = (EnToryo*)thisx;

    switch (limbIndex) {
        case 15:
            Matrix_Position(&pos, &this->actor.focus.pos);
            break;
    }
}
