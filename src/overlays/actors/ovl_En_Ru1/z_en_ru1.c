/*
 * File: z_en_ru1.c
 * Overlay: En_Ru1
 * Description: Ruto (child)
 */

#include "z_en_ru1.h"
#include "assets/objects/object_ru1/object_ru1.h"
#include "terminal.h"
#include "versions.h"
#include "overlays/actors/ovl_Demo_Effect/z_demo_effect.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_CAN_PRESS_SWITCHES)

void En_Ru1_Actor_ct(Actor* thisx, PlayState* play);
void En_Ru1_Actor_dt(Actor* thisx, PlayState* play);
void En_Ru1_Actor_main(Actor* thisx, PlayState* play);
void En_Ru1_Actor_draw(Actor* thisx, PlayState* play);

void En_Ru1_Cryst_Actor_main_kiss(EnRu1* this, PlayState* play);
void En_Ru1_Cryst_Actor_main_jump(EnRu1* this, PlayState* play);
void En_Ru1_Cryst_Actor_main_float(EnRu1* this, PlayState* play);
void En_Ru1_Cryst_Actor_main_approach(EnRu1* this, PlayState* play);
void En_Ru1_Cryst_Actor_main_sink(EnRu1* this, PlayState* play);
void En_Ru1_Cryst_Actor_main_leave(EnRu1* this, PlayState* play);
void En_Ru1_Cryst_Actor_main_brake(EnRu1* this, PlayState* play);
void En_Ru1_Bdan00_Actor_main_wait(EnRu1* this, PlayState* play);
void En_Ru1_Bdan00_Actor_main_hide(EnRu1* this, PlayState* play);
void En_Ru1_Bdan00_Actor_main_refuse(EnRu1* this, PlayState* play);
void En_Ru1_Bdan00_Actor_main_turn(EnRu1* this, PlayState* play);
void En_Ru1_Bdan00_Actor_main_run_accel(EnRu1* this, PlayState* play);
void En_Ru1_Bdan00_Actor_main_run_move(EnRu1* this, PlayState* play);
void En_Ru1_Bdan00_Actor_main_step(EnRu1* this, PlayState* play);
void En_Ru1_Bdan00_Actor_main_disappear(EnRu1* this, PlayState* play);
void En_Ru1_Bdanboss_Actor_main_wait(EnRu1* this, PlayState* play);
void En_Ru1_Bdanboss_Actor_main_hide(EnRu1* this, PlayState* play);
void En_Ru1_Bdanboss_Actor_main_up(EnRu1* this, PlayState* play);
void En_Ru1_Bdanboss_Actor_main_irritate(EnRu1* this, PlayState* play);
void En_Ru1_Bdanboss_Actor_main_correct(EnRu1* this, PlayState* play);
void En_Ru1_Bdanboss_Actor_main_greet(EnRu1* this, PlayState* play);
void En_Ru1_Bdanboss_Actor_main_fly(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_wait(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_hide(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_greet(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_greeting(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_sitdown(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_alone_stop(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_alone_move(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_alone_float(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_alone_sink(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_curry(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_strech(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_alpha(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_cheer(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_hurry(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_standup(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_errand(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_handup(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_handdown(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_order(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_search_stand(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_waitlink(EnRu1* this, PlayState* play);
void En_Ru1_Option_Actor_main_scream(EnRu1* this, PlayState* play);
void En_Ru1_Spot07_main_stand(EnRu1* this, PlayState* play);
void En_Ru1_Spot07_main_greet(EnRu1* this, PlayState* play);

void En_Ru1_turn_link(EnRu1* this, PlayState* play, s32 limbIndex, Vec3s* rot);

void En_Ru1_Actor_draw_none(EnRu1* this, PlayState* play);
void En_Ru1_Actor_draw_normal(EnRu1* this, PlayState* play);
void En_Ru1_Actor_draw_alpha(EnRu1* this, PlayState* play);

static ColliderCylinderInitType1 En_Ru1_Option_OcInfoData_forStand = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, 0x00, 0x00, 0x01 },
    { 25, 80, 0, { 0 } },
};

static ColliderCylinderInitType1 En_Ru1_Option_OcInfoData_forOption = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000101, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, 0x01, 0x00, 0x01 },
    { 20, 30, 0, { 0 } },
};

static void* en_ru1_eye[] = {
    gRutoChildEyeOpenTex,     gRutoChildEyeHalfTex,  gRutoChildEyeClosedTex,
    gRutoChildEyeRollLeftTex, gRutoChildEyeHalf2Tex, gRutoChildEyeHalfWithBlushTex,
};

static void* en_ru1_mouth[] = {
    gRutoChildMouthClosedTex,
    gRutoChildMouthFrownTex,
    gRutoChildMouthOpenTex,
};

#include "Demodt_BdanDemo00.inc.c"

#include "Demodt_BdanDemoOption.inc.c"

#include "Demodt_BdanDemoFind.inc.c"

void En_Ru1_Option_Excute_Corect_forStand(EnRu1* this, PlayState* play) {
    s32 pad[5];

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void En_Ru1_Option_Excute_Corect_forOption(EnRu1* this, PlayState* play) {
    s32 pad[5];

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider2);
    if (this->unk_34C != 0) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider2.base);
    } else if (this->actor.xzDistToPlayer > 32.0f) {
        this->unk_34C = 1;
    }
}

void En_Ru1_Option_Excute_Attck_forOption(EnRu1* this, PlayState* play) {
    s32 pad[5];

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider2);
    CollisionCheck_setAT(play, &play->colChkCtx, &this->collider2.base);
}

void En_Ru1_Option_ct_forCorect(Actor* thisx, PlayState* play) {
    EnRu1* this = (EnRu1*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set3(play, &this->collider, &this->actor, &En_Ru1_Option_OcInfoData_forStand);

    ClObjPipe_ct(play, &this->collider2);
    ClObjPipe_set3(play, &this->collider2, &this->actor, &En_Ru1_Option_OcInfoData_forOption);
}

void En_Ru1_Option_dt_forCorect(EnRu1* this, PlayState* play) {
    ClObjPipe_dt(play, &this->collider);
    ClObjPipe_dt(play, &this->collider2);
}

void En_Ru1_Option_Reset_Corect_forOption(EnRu1* this) {
    this->unk_34C = 0;
}

u8 En_Ru1_GetUpper_arg_data(EnRu1* this) {
    u8 params = PARAMS_GET_U(this->actor.params, 8, 8);

    return params;
}

u8 En_Ru1_GetLower_arg_data(EnRu1* this) {
    u8 params = PARAMS_GET_U(this->actor.params, 0, 8);

    return params;
}

void En_Ru1_Actor_dt(Actor* thisx, PlayState* play) {
    EnRu1* this = (EnRu1*)thisx;

    En_Ru1_Option_dt_forCorect(this, play);
}

void En_Ru1_set_eye_pattern(EnRu1* this) {
    s32 pad[3];
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeIndex = &this->eyeIndex;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = get_random_timer(60, 60);
    }

    *eyeIndex = *blinkTimer;
    if (*eyeIndex >= 3) {
        *eyeIndex = 0;
    }
}

void En_Ru1_set_eye_Num(EnRu1* this, s16 eyeIndex) {
    this->eyeIndex = eyeIndex;
}

void En_Ru1_set_mouth_Num(EnRu1* this, s16 mouthIndex) {
    this->mouthIndex = mouthIndex;
}

void En_Ru1_BGcheck(EnRu1* this, PlayState* play) {
    f32* velocityY = &this->actor.velocity.y;
    f32 velocityYHeld = *velocityY;

    *velocityY = -4.0f;
    Actor_BGcheck2(play, &this->actor, 19.0f, 25.0f, 30.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    *velocityY = velocityYHeld;
}

s32 En_Ru1_Check_EndDemoMode(PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        return true;
    }
    return false;
}

CsCmdActorCue* En_Ru1_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    s32 pad[2];
    CsCmdActorCue* cue = NULL;

    if (!En_Ru1_Check_EndDemoMode(play)) {
        cue = play->csCtx.actorCues[cueChannel];
    }
    return cue;
}

s32 En_Ru1_Check_npcdemopnt(PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = En_Ru1_Get_npcdemopnt(play, cueChannel);

    if ((cue != NULL) && (cue->id == cueId)) {
        return true;
    }
    return false;
}

s32 En_Ru1_Check2_npcdemopnt(PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = En_Ru1_Get_npcdemopnt(play, cueChannel);

    if ((cue != NULL) && (cue->id != cueId)) {
        return true;
    }
    return false;
}

s32 En_Ru1_Search_Ru1(EnRu1* this, PlayState* play) {
    Actor* actorIt = play->actorCtx.actorLists[ACTORCAT_NPC].head;
    EnRu1* someEnRu1;

    while (actorIt != NULL) {
        if (actorIt->id == ACTOR_EN_RU1) {
            someEnRu1 = (EnRu1*)actorIt;
            if (someEnRu1 != this) {
                if ((someEnRu1->action == 31) || (someEnRu1->action == 32) || (someEnRu1->action == 24)) {
                    return true;
                }
            }
        }
        actorIt = actorIt->next;
    }
    return false;
}

BgBdanObjects* En_Ru1_Search_Stand_Actor(PlayState* play) {
    Actor* actorIt = play->actorCtx.actorLists[ACTORCAT_BG].head;

    while (actorIt != NULL) {
        if (actorIt->id == ACTOR_BG_BDAN_OBJECTS && actorIt->params == 0) {
            return (BgBdanObjects*)actorIt;
        }
        actorIt = actorIt->next;
    }
    // "There is no stand"
    PRINTF(VT_FGCOL(RED) "お立ち台が無い!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    return NULL;
}

void En_Ru1_SetConect_inStand(EnRu1* this, s32 cameraSetting) {
    if (this->unk_28C != NULL) {
        this->unk_28C->cameraSetting = cameraSetting;
    }
}

s32 En_Ru1_CheckConect_inStand(EnRu1* this) {
    if (this->unk_28C != NULL) {
        return this->unk_28C->cameraSetting;
    } else {
        return 0;
    }
}

Actor* En_Ru1_Search_Juwel_Actor(PlayState* play) {
    Actor* actorIt = play->actorCtx.actorLists[ACTORCAT_BOSS].head;

    while (actorIt != NULL) {
        if ((actorIt->id == ACTOR_DEMO_EFFECT) && (PARAMS_GET_U(actorIt->params, 0, 8) == DEMO_EFFECT_JEWEL_ZORA)) {
            return actorIt;
        }
        actorIt = actorIt->next;
    }
    return NULL;
}

int En_Ru1_ConversationAndPad_Trigger(PlayState* play) {
    return (message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play);
}

s32 En_Ru1_ConversationEnd(PlayState* play) {
    return message_check(&play->msgCtx) == TEXT_STATE_CLOSING;
}

#if DEBUG_FEATURES
void func_80AEB1D8(EnRu1* this) {
    this->action = 36;
    this->drawConfig = 0;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.velocity.z = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.minVelocityY = 0.0f;
    En_Ru1_SetConect_inStand(this, 0);
}

void func_80AEB220(EnRu1* this, PlayState* play) {
    if ((En_Ru1_Check_EndDemoMode(play)) && (this->actor.params == 0xA)) {
        func_80AEB1D8(this);
    }
}
#endif

void En_Ru1_Change_Anime(EnRu1* this, AnimationHeader* animation, u8 arg2, f32 morphFrames, s32 arg4) {
    s32 pad[2];
    AnimationHeader* animHeader = SEGMENTED_TO_VIRTUAL(animation);
    f32 frameCount = Si2_anime_end_frame(animHeader);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (arg4 == 0) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        unk0 = frameCount;
        fc = 0.0f;
        playbackSpeed = -1.0f;
    }

    Skeleton_Info2_init(&this->skelAnime, animHeader, playbackSpeed, unk0, fc, arg2, morphFrames);
}

s32 En_Ru1_Animation_Base(EnRu1* this) {
    // why?
    if (this->action != 32) {
        return Skeleton_Info2_anime_play(&this->skelAnime);
    } else {
        return Skeleton_Info2_anime_play(&this->skelAnime);
    }
}

void En_Ru1_Movement_byAnimation(EnRu1* this, PlayState* play) {
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ;
    Skeleton_Proc_Anime_Move_init(play, &this->actor, &this->skelAnime, 1.0f);
}

void En_Ru1_Start_Movement_byAnimation(EnRu1* this, PlayState* play) {
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ;
    En_Ru1_Movement_byAnimation(this, play);
}

void En_Ru1_End_Movement_byAnimation(EnRu1* this) {
    this->skelAnime.movementFlags &= ~ANIM_FLAG_UPDATE_XZ;
}

#include "z_en_ru1_inSpot08.inc.c"

#include "z_en_ru1_inBdanDemo00.inc.c"

#include "z_en_ru1_inBdanDemoboss.inc.c"

#include "z_en_ru1_inOption.inc.c"

#include "z_en_ru1_inGreet.inc.c"

#include "z_en_ru1_inErrand.inc.c"

#include "z_en_ru1_inStand.inc.c"

#include "z_en_ru1_inSpot07.inc.c"

#include "z_en_ru1_inSwitch.inc.c"

#if DEBUG_FEATURES
void func_80AF0050(EnRu1* this, PlayState* play) {
    En_Ru1_Change_Anime(this, &gRutoChildWait2Anim, 0, 0, 0);
    this->action = 36;
    this->roomNum1 = this->actor.room;
    this->unk_28C = En_Ru1_Search_Stand_Actor(play);
    this->actor.room = -1;
}
#endif

void En_Ru1_Actor_main(Actor* thisx, PlayState* play) {
    static EnRu1ActionFunc proc[] = {
        En_Ru1_Cryst_Actor_main_kiss, En_Ru1_Cryst_Actor_main_jump, En_Ru1_Cryst_Actor_main_float, En_Ru1_Cryst_Actor_main_approach, En_Ru1_Cryst_Actor_main_sink, En_Ru1_Cryst_Actor_main_leave, En_Ru1_Cryst_Actor_main_brake,
        En_Ru1_Bdan00_Actor_main_wait, En_Ru1_Bdan00_Actor_main_hide, En_Ru1_Bdan00_Actor_main_refuse, En_Ru1_Bdan00_Actor_main_turn, En_Ru1_Bdan00_Actor_main_run_accel, En_Ru1_Bdan00_Actor_main_run_move, En_Ru1_Bdan00_Actor_main_step,
        En_Ru1_Bdan00_Actor_main_disappear, En_Ru1_Bdanboss_Actor_main_wait, En_Ru1_Bdanboss_Actor_main_hide, En_Ru1_Bdanboss_Actor_main_up, En_Ru1_Bdanboss_Actor_main_irritate, En_Ru1_Bdanboss_Actor_main_correct, En_Ru1_Bdanboss_Actor_main_greet,
        En_Ru1_Bdanboss_Actor_main_fly, En_Ru1_Option_Actor_main_wait, En_Ru1_Option_Actor_main_hide, En_Ru1_Option_Actor_main_greet, En_Ru1_Option_Actor_main_greeting, En_Ru1_Option_Actor_main_sitdown, En_Ru1_Option_Actor_main_alone_stop,
        En_Ru1_Option_Actor_main_alone_move, En_Ru1_Option_Actor_main_alone_float, En_Ru1_Option_Actor_main_alone_sink, En_Ru1_Option_Actor_main_curry, En_Ru1_Option_Actor_main_strech, En_Ru1_Option_Actor_main_alpha, En_Ru1_Option_Actor_main_cheer,
        En_Ru1_Option_Actor_main_hurry, En_Ru1_Option_Actor_main_standup, En_Ru1_Option_Actor_main_errand, En_Ru1_Option_Actor_main_handup, En_Ru1_Option_Actor_main_handdown, En_Ru1_Option_Actor_main_order, En_Ru1_Option_Actor_main_search_stand,
        En_Ru1_Option_Actor_main_waitlink, En_Ru1_Option_Actor_main_scream, En_Ru1_Spot07_main_stand, En_Ru1_Spot07_main_greet,
    };

    EnRu1* this = (EnRu1*)thisx;

    if (this->action < 0 || this->action >= ARRAY_COUNT(proc) || proc[this->action] == NULL) {
        // "Main mode is improper!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    proc[this->action](this, play);
}

void En_Ru1_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRu1* this = (EnRu1*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gRutoChildSkel, NULL, this->jointTable, this->morphTable, 17);
    En_Ru1_Option_ct_forCorect(&this->actor, play);
    switch (En_Ru1_GetLower_arg_data(this)) {
        case 0:
            En_Ru1_Actor_Bdanboss_Init(this, play);
            break;
        case 1:
            En_Ru1_Actor_Spot08_Init(this, play);
            break;
        case 2:
            En_Ru1_Actor_Ban00_Init(this, play);
            break;
        case 3:
            En_Ru1_Actor_Option_Init(this, play);
            break;
        case 4:
            En_Ru1_Actor_Stand_Init(this, play);
            break;
        case 5:
            En_Ru1_Actor_Spot07_Init(this, play);
            break;
        case 6:
            En_Ru1_Actor_Switch_Init(this, play);
            break;
#if DEBUG_FEATURES
        case 10:
            func_80AF0050(this, play);
            break;
#endif
        default:
            Actor_delete(&this->actor);
            // "Relevant arge_data = %d unacceptable"
            PRINTF("該当 arge_data = %d 無し\n", En_Ru1_GetLower_arg_data(this));
            break;
    }
}

void En_Ru1_turn_link(EnRu1* this, PlayState* play, s32 limbIndex, Vec3s* rot) {
    Vec3s* torsoRot = &this->interactInfo.torsoRot;
    Vec3s* headRot = &this->interactInfo.headRot;

    switch (limbIndex) {
        case RUTO_CHILD_CHEST:
            rot->x += torsoRot->y;
            rot->y -= torsoRot->x;
            break;
        case RUTO_CHILD_HEAD:
            rot->x += headRot->y;
            rot->z += headRot->x;
            break;
    }
}

s32 En_Ru1_Actor_before_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfx) {
    static EnRu1PreLimbDrawFunc proc[] = {
        En_Ru1_turn_link,
    };

    EnRu1* this = (EnRu1*)thisx;

    if ((this->unk_290 < 0) || (this->unk_290 > 0) || (*proc[this->unk_290] == NULL)) {
        // "Neck rotation mode is improper!"
        PRINTF(VT_FGCOL(RED) "首回しモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        proc[this->unk_290](this, play, limbIndex, rot);
    }
    return false;
}

void En_Ru1_Actor_after_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    static Vec3f pos = { 0.0f, 10.0f, 0.0f };

    EnRu1* this = (EnRu1*)thisx;

    if (limbIndex == RUTO_CHILD_HEAD) {
        Vec3f vec1;
        Vec3f vec2;

        vec1 = pos;
        Matrix_Position(&vec1, &vec2);
        this->actor.focus.pos.x = vec2.x;
        this->actor.focus.pos.y = vec2.y;
        this->actor.focus.pos.z = vec2.z;
        this->actor.focus.rot.x = this->actor.world.rot.x;
        this->actor.focus.rot.y = this->actor.world.rot.y;
        this->actor.focus.rot.z = this->actor.world.rot.z;
    }
}

void En_Ru1_Actor_draw_none(EnRu1* this, PlayState* play) {
}

void En_Ru1_Actor_draw_normal(EnRu1* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* eyeTex = en_ru1_eye[eyeIndex];
    s16 mouthIndex = this->mouthIndex;
    SkelAnime* skelAnime = &this->skelAnime;
    void* mouthTex = en_ru1_mouth[mouthIndex];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ru1.c", 1282);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);

    POLY_OPA_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       En_Ru1_Actor_before_draw, En_Ru1_Actor_after_draw, this, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ru1.c", 1309);
}

void En_Ru1_Actor_draw_alpha(EnRu1* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* eyeTex = en_ru1_eye[eyeIndex];
    s16 mouthIndex = this->mouthIndex;
    SkelAnime* skelAnime = &this->skelAnime;
    void* mouthTex = en_ru1_mouth[mouthIndex];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ru1.c", 1324);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &Actor_change_render_mode[0]);

    POLY_XLU_DISP = Si2_draw2_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       En_Ru1_Actor_before_draw, NULL, this, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ru1.c", 1353);
}

void En_Ru1_Actor_draw(Actor* thisx, PlayState* play) {
    static EnRu1DrawFunc proc[] = {
        En_Ru1_Actor_draw_none,
        En_Ru1_Actor_draw_normal,
        En_Ru1_Actor_draw_alpha,
    };

    EnRu1* this = (EnRu1*)thisx;

    if (this->drawConfig < 0 || this->drawConfig >= ARRAY_COUNT(proc) || proc[this->drawConfig] == NULL) {
        // "Draw mode is improper!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->drawConfig](this, play);
}

ActorProfile En_Ru1_Profile = {
    /**/ ACTOR_EN_RU1,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_RU1,
    /**/ sizeof(EnRu1),
    /**/ En_Ru1_Actor_ct,
    /**/ En_Ru1_Actor_dt,
    /**/ En_Ru1_Actor_main,
    /**/ En_Ru1_Actor_draw,
};
