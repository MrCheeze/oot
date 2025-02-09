/*
 * File: z_en_ru2.c
 * Overlay: En_Ru2
 * Description: Adult Ruto
 */

#include "z_en_ru2.h"
#include "assets/objects/object_ru2/object_ru2.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Ru2_Actor_ct(Actor* thisx, PlayState* play);
void En_Ru2_Actor_dt(Actor* thisx, PlayState* play);
void En_Ru2_Actor_main(Actor* thisx, PlayState* play);
void En_Ru2_Actor_draw(Actor* thisx, PlayState* play);

void En_Ru2_Actor_main_wait(EnRu2* this, PlayState* play);
void En_Ru2_Actor_main_hide(EnRu2* this, PlayState* play);
void En_Ru2_Actor_main_up(EnRu2* this, PlayState* play);
void En_Ru2_Actor_main_greet(EnRu2* this, PlayState* play);
void En_Ru2_Actor_main_handup(EnRu2* this, PlayState* play);
void En_Ru2_Actor_main_cheer(EnRu2* this, PlayState* play);
void En_Ru2_Actor_main_stop(EnRu2* this, PlayState* play);
void En_Ru2_Seal_Actor_main_hide(EnRu2* this, PlayState* play);
void En_Ru2_Seal_Actor_main_fade(EnRu2* this, PlayState* play);
void En_Ru2_Seal_Actor_main_pray(EnRu2* this, PlayState* play);
void En_Ru2_inEnding_main_wait(EnRu2* this, PlayState* play);
void En_Ru2_inEnding_main_alpha(EnRu2* this, PlayState* play);
void En_Ru2_inEnding_main_stand(EnRu2* this, PlayState* play);
void En_Ru2_inEnding_main_lookup(EnRu2* this, PlayState* play);
void En_Ru2_inMizusin_main_wait(EnRu2* this, PlayState* play);
void En_Ru2_inMizusin_main_stand(EnRu2* this, PlayState* play);
void En_Ru2_inMizusin_main_greet(EnRu2* this, PlayState* play);
void En_Ru2_inMizusin_main_greeting(EnRu2* this, PlayState* play);
void En_Ru2_inMizusin_main_gooby(EnRu2* this, PlayState* play);
void En_Ru2_inMizusin_main_swim(EnRu2* this, PlayState* play);

void En_Ru2_Actor_draw_none(EnRu2* this, PlayState* play);
void En_Ru2_Actor_draw_normal(EnRu2* this, PlayState* play);
void En_Ru2_Actor_draw_alpha(EnRu2* this, PlayState* play);

void En_Ru2_check_WaitToHide(EnRu2* this, PlayState* play);

typedef enum {
    /* 00 */ ENRU2_SETUP_WATER_MEDALLION_CS,
    /* 01 */ ENRU2_AWAIT_BLUE_WARP,
    /* 02 */ ENRU2_RISE_THROUGH_BLUE_WARP,
    /* 03 */ ENRU2_SAGE_OF_WATER_DIALOG,
    /* 04 */ ENRU2_RAISE_ARMS,
    /* 05 */ ENRU2_AWAIT_SPAWN_WATER_MEDALLION,
    /* 06 */ ENRU2_FINISH_WATER_MEDALLION_CS,
    /* 07 */ ENRU2_WATER_TRIAL_INVISIBLE,
    /* 08 */ ENRU2_WATER_TRIAL_FADE,
    /* 09 */ ENRU2_AWAIT_SPAWN_LIGHT_BALL,
    /* 10 */ ENRU2_CREDITS_INVISIBLE,
    /* 11 */ ENRU2_CREDITS_FADE_IN,
    /* 12 */ ENRU2_CREDITS_VISIBLE,
    /* 13 */ ENRU2_CREDITS_TURN_HEAD_DOWN_LEFT,
    /* 14 */ ENRU2_WATER_TEMPLE_ENCOUNTER_RANGE_CHECK,
    /* 15 */ ENRU2_WATER_TEMPLE_ENCOUNTER_UNCONDITIONAL, // unused
    /* 16 */ ENRU2_WATER_TEMPLE_ENCOUNTER_BEGINNING,
    /* 17 */ ENRU2_WATER_TEMPLE_ENCOUNTER_DIALOG,
    /* 18 */ ENRU2_WATER_TEMPLE_ENCOUNTER_END,
    /* 19 */ ENRU2_WATER_TEMPLE_SWIMMING_UP
} EnRu2Action;

typedef enum {
    /* 00 */ ENRU2_DRAW_NOTHING,
    /* 01 */ ENRU2_DRAW_OPA,
    /* 02 */ ENRU2_DRAW_XLU
} EnRu2DrawConfig;

static ColliderCylinderInitType1 En_Ru2_OcInfoData_forStand = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0x00000080, 0x00, 0x00 }, 0x00, 0x01, 0x00 },
    { 30, 100, 0, { 0 } },
};

static void* en_ru2_eye[] = {
    gAdultRutoEyeOpenTex,
    gAdultRutoEyeHalfTex,
    gAdultRutoEyeClosedTex,
};

#if DEBUG_FEATURES
static UNK_TYPE D_80AF4118 = 0;
#endif

#include "Demodt_Kenjyanoma.inc.c"

void En_Ru2_ct_forCorect(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set3(play, &this->collider, &this->actor, &En_Ru2_OcInfoData_forStand);
}

void En_Ru2_Excute_Corect_forStand(EnRu2* this, PlayState* play) {
    s32 pad[5];

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
}

void En_Ru2_Actor_dt(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void En_Ru2_set_eye_pattern(EnRu2* this) {
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

s32 En_Ru2_GetUpper_arg_data(EnRu2* this) {
    s32 switchFlag = ENRU2_GET_SWITCH_FLAG(this);

    return switchFlag;
}

s32 En_Ru2_GetLower_arg_data(EnRu2* this) {
    s32 type = ENRU2_GET_TYPE(this);

    return type;
}

#if DEBUG_FEATURES
void func_80AF26AC(EnRu2* this) {
    this->action = ENRU2_WATER_TRIAL_INVISIBLE;
    this->drawConfig = ENRU2_DRAW_NOTHING;
    this->alpha = 0;
    this->isLightBall = false;
    this->actor.shape.shadowAlpha = 0;
    this->fadeTimer = 0.0f;
}

void func_80AF26D0(EnRu2* this, PlayState* play) {
    s32 one; // Needed to match

    if (play->csCtx.state == CS_STATE_IDLE) {
        if (D_80AF4118 != 0) {
            if (this->actor.params == 2) {
                func_80AF26AC(this);
            }
            D_80AF4118 = 0;
            return;
        }
    } else {
        one = 1;
        if (D_80AF4118 == 0) {
            D_80AF4118 = one;
        }
    }
}
#endif

void En_Ru2_BGcheck(EnRu2* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_2);
}

s32 En_Ru2_Animation_Base(EnRu2* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

CsCmdActorCue* En_Ru2_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

s32 En_Ru2_Check_npcdemopnt(EnRu2* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = En_Ru2_Get_npcdemopnt(play, cueChannel);

    if ((cue != NULL) && (cue->id == cueId)) {
        return true;
    }
    return false;
}

s32 En_Ru2_Check2_npcdemopnt(EnRu2* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = En_Ru2_Get_npcdemopnt(play, cueChannel);

    if ((cue != NULL) && (cue->id != cueId)) {
        return true;
    }
    return false;
}

/**
 * Checks cutscene data and, if applicable, configures Ruto's position accordingly.
 */
void En_Ru2_Set_StartPos_npcdemopnt(EnRu2* this, PlayState* play, u32 cueChannel) {
    CsCmdActorCue* cue = En_Ru2_Get_npcdemopnt(play, cueChannel);
    Actor* thisx = &this->actor;

    if (cue != NULL) {
        thisx->world.pos.x = cue->startPos.x;
        thisx->world.pos.y = cue->startPos.y;
        thisx->world.pos.z = cue->startPos.z;

        thisx->world.rot.y = thisx->shape.rot.y = cue->rot.y;
    }
}

/**
 * Changes the animation for Ruto's actor. The direction argument decides whether to play the animation
 * forwards (if 0) or backwards (otherwise).
 */
void En_Ru2_Change_Anime(EnRu2* this, AnimationHeader* animation, u8 mode, f32 morphFrames, s32 direction) {
    f32 frameCount = Si2_anime_end_frame(animation);
    f32 playbackSpeed;
    f32 startFrame;
    f32 endFrame;

    if (direction == 0) {
        startFrame = 0.0f;
        endFrame = frameCount;
        playbackSpeed = 1.0f;
    } else {
        startFrame = frameCount;
        endFrame = 0.0f;
        playbackSpeed = -1.0f;
    }

    Skeleton_Info2_init(&this->skelAnime, animation, playbackSpeed, startFrame, endFrame, mode, morphFrames);
}

#include "z_en_ru2_inKenjyanoma.inc.c"

#include "z_en_ru2_inKenjyanomaDemo02.inc.c"

#include "z_en_ru2_inEnding.inc.c"

#include "z_en_ru2_inMizusin.inc.c"

void En_Ru2_Actor_main(Actor* thisx, PlayState* play) {
    static EnRu2ActionFunc proc[] = {
        En_Ru2_Actor_main_wait,
        En_Ru2_Actor_main_hide,
        En_Ru2_Actor_main_up,
        En_Ru2_Actor_main_greet,
        En_Ru2_Actor_main_handup,
        En_Ru2_Actor_main_cheer,
        En_Ru2_Actor_main_stop,
        En_Ru2_Seal_Actor_main_hide,
        En_Ru2_Seal_Actor_main_fade,
        En_Ru2_Seal_Actor_main_pray,
        En_Ru2_inEnding_main_wait,
        En_Ru2_inEnding_main_alpha,
        En_Ru2_inEnding_main_stand,
        En_Ru2_inEnding_main_lookup,
        En_Ru2_inMizusin_main_wait,
        En_Ru2_inMizusin_main_stand,
        En_Ru2_inMizusin_main_greet,
        En_Ru2_inMizusin_main_greeting,
        En_Ru2_inMizusin_main_gooby,
        En_Ru2_inMizusin_main_swim,
    };

    EnRu2* this = (EnRu2*)thisx;

    if ((this->action < 0) || (this->action >= ARRAY_COUNT(proc)) || (proc[this->action] == NULL)) {
        // "Main Mode is improper!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->action](this, play);
}

void En_Ru2_Actor_ct(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    En_Ru2_ct_forCorect(thisx, play);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gAdultRutoSkel, NULL, this->jointTable, this->morphTable, 23);

    switch (En_Ru2_GetLower_arg_data(this)) {
        case 2:
            En_Ru2_KenjyanomaDemo02_Init(this, play);
            break;
        case 3:
            En_Ru2_Ending_Init(this, play);
            break;
        case 4:
            En_Ru2_Mizusin_Init(this, play);
            break;
        default:
            En_Ru2_Kenjyanoma_Init(this, play);
            break;
    }

    this->textboxCount = 0;
    this->lastDialogState = TEXT_STATE_DONE_FADING;
}

void En_Ru2_Actor_draw_none(EnRu2* this, PlayState* play) {
}

void En_Ru2_Actor_draw_normal(EnRu2* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* tex = en_ru2_eye[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ru2.c", 642);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);

    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ru2.c", 663);
}

void En_Ru2_Actor_draw(Actor* thisx, PlayState* play) {
    static EnRu2DrawFunc proc[] = {
        En_Ru2_Actor_draw_none,
        En_Ru2_Actor_draw_normal,
        En_Ru2_Actor_draw_alpha,
    };

    EnRu2* this = (EnRu2*)thisx;

    if ((this->drawConfig < 0) || (this->drawConfig >= ARRAY_COUNT(proc)) ||
        (proc[this->drawConfig] == NULL)) {
        // "Draw Mode is improper!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->drawConfig](this, play);
}

ActorProfile En_Ru2_Profile = {
    /**/ ACTOR_EN_RU2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_RU2,
    /**/ sizeof(EnRu2),
    /**/ En_Ru2_Actor_ct,
    /**/ En_Ru2_Actor_dt,
    /**/ En_Ru2_Actor_main,
    /**/ En_Ru2_Actor_draw,
};
