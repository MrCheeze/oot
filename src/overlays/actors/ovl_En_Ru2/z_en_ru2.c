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

void EnRu2_Init(Actor* thisx, PlayState* play);
void EnRu2_Destroy(Actor* thisx, PlayState* play);
void EnRu2_Update(Actor* thisx, PlayState* play);
void EnRu2_Draw(Actor* thisx, PlayState* play);

void EnRu2_SetupWaterMedallionCutscene(EnRu2* this, PlayState* play);
void EnRu2_AwaitBlueWarp(EnRu2* this, PlayState* play);
void EnRu2_RiseThroughBlueWarp(EnRu2* this, PlayState* play);
void EnRu2_SageOfWaterDialog(EnRu2* this, PlayState* play);
void EnRu2_RaiseArms(EnRu2* this, PlayState* play);
void EnRu2_AwaitWaterMedallion(EnRu2* this, PlayState* play);
void EnRu2_FinishWaterMedallionCutscene(EnRu2* this, PlayState* play);
void EnRu2_WaterTrialInvisible(EnRu2* this, PlayState* play);
void EnRu2_WaterTrialFade(EnRu2* this, PlayState* play);
void EnRu2_AwaitSpawnLightBall(EnRu2* this, PlayState* play);
void EnRu2_CreditsInvisible(EnRu2* this, PlayState* play);
void EnRu2_CreditsFadeIn(EnRu2* this, PlayState* play);
void EnRu2_CreditsVisible(EnRu2* this, PlayState* play);
void EnRu2_CreditsTurnHeadDownLeft(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleEncounterRangeCheck(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleEncounterUnconditional(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleEncounterBegin(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleEncounterDialog(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleEncounterEnd(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleSwimmingUp(EnRu2* this, PlayState* play);

void EnRu2_DrawNothing(EnRu2* this, PlayState* play);
void EnRu2_DrawOpa(EnRu2* this, PlayState* play);
void EnRu2_DrawXlu(EnRu2* this, PlayState* play);

void EnRu2_CheckWaterMedallionCutscene(EnRu2* this, PlayState* play);

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

static ColliderCylinderInitType1 sCylinderInit = {
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

static void* sEyeTextures[] = {
    gAdultRutoEyeOpenTex,
    gAdultRutoEyeHalfTex,
    gAdultRutoEyeClosedTex,
};

#if DEBUG_FEATURES
static UNK_TYPE D_80AF4118 = 0;
#endif

#include "Demodt_Kenjyanoma.inc.c"

static EnRu2ActionFunc sActionFuncs[] = {
    EnRu2_SetupWaterMedallionCutscene,
    EnRu2_AwaitBlueWarp,
    EnRu2_RiseThroughBlueWarp,
    EnRu2_SageOfWaterDialog,
    EnRu2_RaiseArms,
    EnRu2_AwaitWaterMedallion,
    EnRu2_FinishWaterMedallionCutscene,
    EnRu2_WaterTrialInvisible,
    EnRu2_WaterTrialFade,
    EnRu2_AwaitSpawnLightBall,
    EnRu2_CreditsInvisible,
    EnRu2_CreditsFadeIn,
    EnRu2_CreditsVisible,
    EnRu2_CreditsTurnHeadDownLeft,
    EnRu2_WaterTempleEncounterRangeCheck,
    EnRu2_WaterTempleEncounterUnconditional,
    EnRu2_WaterTempleEncounterBegin,
    EnRu2_WaterTempleEncounterDialog,
    EnRu2_WaterTempleEncounterEnd,
    EnRu2_WaterTempleSwimmingUp,
};

static EnRu2DrawFunc sDrawFuncs[] = {
    EnRu2_DrawNothing,
    EnRu2_DrawOpa,
    EnRu2_DrawXlu,
};

ActorProfile En_Ru2_Profile = {
    /**/ ACTOR_EN_RU2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_RU2,
    /**/ sizeof(EnRu2),
    /**/ EnRu2_Init,
    /**/ EnRu2_Destroy,
    /**/ EnRu2_Update,
    /**/ EnRu2_Draw,
};

void EnRu2_InitCollider(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
}

void EnRu2_UpdateCollider(EnRu2* this, PlayState* play) {
    s32 pad[5];

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void EnRu2_Destroy(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnRu2_UpdateEyes(EnRu2* this) {
    s32 pad[3];
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeIndex = &this->eyeIndex;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }

    *eyeIndex = *blinkTimer;
    if (*eyeIndex >= 3) {
        *eyeIndex = 0;
    }
}

s32 EnRu2_GetSwitchFlag(EnRu2* this) {
    s32 switchFlag = ENRU2_GET_SWITCH_FLAG(this);

    return switchFlag;
}

s32 EnRu2_GetType(EnRu2* this) {
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

void EnRu2_UpdateBgCheckInfo(EnRu2* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_2);
}

s32 EnRu2_UpdateSkelAnime(EnRu2* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorCue* EnRu2_GetCue(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

s32 EnRu2_CheckCueMatchingId(EnRu2* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = EnRu2_GetCue(play, cueChannel);

    if ((cue != NULL) && (cue->id == cueId)) {
        return true;
    }
    return false;
}

s32 EnRu2_CheckCueNotMatchingId(EnRu2* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = EnRu2_GetCue(play, cueChannel);

    if ((cue != NULL) && (cue->id != cueId)) {
        return true;
    }
    return false;
}

/**
 * Checks cutscene data and, if applicable, configures Ruto's position accordingly.
 */
void EnRu2_InitPositionFromCue(EnRu2* this, PlayState* play, u32 cueChannel) {
    CsCmdActorCue* cue = EnRu2_GetCue(play, cueChannel);
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
void EnRu2_AnimationChange(EnRu2* this, AnimationHeader* animation, u8 mode, f32 morphFrames, s32 direction) {
    f32 frameCount = Animation_GetLastFrame(animation);
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

    Animation_Change(&this->skelAnime, animation, playbackSpeed, startFrame, endFrame, mode, morphFrames);
}

#include "z_en_ru2_inKenjyanoma.inc.c"

#include "z_en_ru2_inKenjyanomaDemo02.inc.c"

#include "z_en_ru2_inEnding.inc.c"

#include "z_en_ru2_inMizusin.inc.c"

void EnRu2_Update(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    if ((this->action < 0) || (this->action >= ARRAY_COUNT(sActionFuncs)) || (sActionFuncs[this->action] == NULL)) {
        // "Main Mode is improper!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, play);
}

void EnRu2_Init(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    EnRu2_InitCollider(thisx, play);
    SkelAnime_InitFlex(play, &this->skelAnime, &gAdultRutoSkel, NULL, this->jointTable, this->morphTable, 23);

    switch (EnRu2_GetType(this)) {
        case 2:
            EnRu2_InitWaterTrial(this, play);
            break;
        case 3:
            EnRu2_InitCredits(this, play);
            break;
        case 4:
            EnRu2_InitWaterTempleEncounter(this, play);
            break;
        default:
            EnRu2_InitChamberOfSages(this, play);
            break;
    }

    this->textboxCount = 0;
    this->lastDialogState = TEXT_STATE_DONE_FADING;
}

void EnRu2_DrawNothing(EnRu2* this, PlayState* play) {
}

void EnRu2_DrawOpa(EnRu2* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* tex = sEyeTextures[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ru2.c", 642);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ru2.c", 663);
}

void EnRu2_Draw(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    if ((this->drawConfig < 0) || (this->drawConfig >= ARRAY_COUNT(sDrawFuncs)) ||
        (sDrawFuncs[this->drawConfig] == NULL)) {
        // "Draw Mode is improper!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawConfig](this, play);
}
