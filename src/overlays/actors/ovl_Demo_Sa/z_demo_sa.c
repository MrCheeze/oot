/*
 * File: z_demo_sa.c
 * Overlay: Demo_Sa
 * Description: Saria (Cutscene)
 */

#include "z_demo_sa.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "assets/objects/object_sa/object_sa.h"

#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void DemoSa_Init(Actor* thisx, PlayState* play);
void DemoSa_Destroy(Actor* thisx, PlayState* play);
void DemoSa_Update(Actor* thisx, PlayState* play);
void DemoSa_Draw(Actor* thisx, PlayState* play);

void func_8098EBB8(DemoSa* this, PlayState* play);
void func_8098EBD8(DemoSa* this, PlayState* play);
void func_8098EBF8(DemoSa* this, PlayState* play);
void func_8098EC28(DemoSa* this, PlayState* play);
void func_8098EC60(DemoSa* this, PlayState* play);
void func_8098EC94(DemoSa* this, PlayState* play);
void func_8098ECCC(DemoSa* this, PlayState* play);
void func_8098F0E8(DemoSa* this, PlayState* play);
void func_8098F118(DemoSa* this, PlayState* play);
void func_8098F16C(DemoSa* this, PlayState* play);
void func_8098F3F0(DemoSa* this, PlayState* play);
void func_8098F714(DemoSa* this, PlayState* play);
void func_8098F734(DemoSa* this, PlayState* play);
void func_8098F77C(DemoSa* this, PlayState* play);
void func_8098F7BC(DemoSa* this, PlayState* play);
void func_8098F7FC(DemoSa* this, PlayState* play);
void func_8098FC44(DemoSa* this, PlayState* play);
void func_8098FC64(DemoSa* this, PlayState* play);
void func_8098FC9C(DemoSa* this, PlayState* play);
void func_8098FCD4(DemoSa* this, PlayState* play);
void func_8098FD0C(DemoSa* this, PlayState* play);

void DemoSa_DrawNothing(DemoSa* this, PlayState* play);
void DemoSa_DrawOpa(DemoSa* this, PlayState* play);
void DemoSa_DrawXlu(DemoSa* this, PlayState* play);

typedef enum SariaEyeState {
    /* 0 */ SARIA_EYE_OPEN,
    /* 1 */ SARIA_EYE_HALF,
    /* 2 */ SARIA_EYE_CLOSED,
    /* 3 */ SARIA_EYE_SUPRISED,
    /* 4 */ SARIA_EYE_SAD
} SariaEyeState;

typedef enum SariaMouthState {
    /* 0 */ SARIA_MOUTH_CLOSED2,
    /* 1 */ SARIA_MOUTH_SUPRISED,
    /* 2 */ SARIA_MOUTH_CLOSED,
    /* 3 */ SARIA_MOUTH_SMILING_OPEN,
    /* 4 */ SARIA_MOUTH_FROWNING
} SariaMouthState;

static void* sEyeTextures[] = {
    gSariaEyeOpenTex, gSariaEyeHalfTex, gSariaEyeClosedTex, gSariaEyeSuprisedTex, gSariaEyeSadTex,
};

static void* sMouthTextures[] = {
    gSariaMouthClosed2Tex,     gSariaMouthSuprisedTex, gSariaMouthClosedTex,
    gSariaMouthSmilingOpenTex, gSariaMouthFrowningTex,
};

#if DEBUG_FEATURES
static u32 D_80990108 = 0;
#endif

#include "Demodt_Kenjyanoma.inc.c"

static DemoSaActionFunc sActionFuncs[] = {
    func_8098EBB8, func_8098EBD8, func_8098EBF8, func_8098EC28, func_8098EC60, func_8098EC94, func_8098ECCC,
    func_8098F0E8, func_8098F118, func_8098F16C, func_8098F3F0, func_8098F714, func_8098F734, func_8098F77C,
    func_8098F7BC, func_8098F7FC, func_8098FC44, func_8098FC64, func_8098FC9C, func_8098FCD4, func_8098FD0C,
};

static DemoSaDrawFunc sDrawFuncs[] = {
    DemoSa_DrawNothing,
    DemoSa_DrawOpa,
    DemoSa_DrawXlu,
};

ActorProfile Demo_Sa_Profile = {
    /**/ ACTOR_DEMO_SA,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SA,
    /**/ sizeof(DemoSa),
    /**/ DemoSa_Init,
    /**/ DemoSa_Destroy,
    /**/ DemoSa_Update,
    /**/ DemoSa_Draw,
};

void DemoSa_Destroy(Actor* thisx, PlayState* play) {
    DemoSa* this = (DemoSa*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
}

void func_8098E480(DemoSa* this) {
    s32 pad[2];
    s16* eyeIndex = &this->eyeIndex;
    s16* blinkTimer = &this->blinkTimer;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(0x3C, 0x3C);
    }

    *eyeIndex = *blinkTimer;
    if (*eyeIndex >= 3) {
        *eyeIndex = 0;
    }
}

void DemoSa_SetEyeIndex(DemoSa* this, s16 eyeIndex) {
    this->eyeIndex = eyeIndex;
}

void DemoSa_SetMouthIndex(DemoSa* this, s16 mouthIndex) {
    this->mouthIndex = mouthIndex;
}

#if DEBUG_FEATURES
void func_8098E530(DemoSa* this) {
    this->action = 7;
    this->drawConfig = 0;
    this->alpha = 0;
    this->unk_1A8 = 0;
    this->actor.shape.shadowAlpha = 0;
    this->unk_1A0 = 0.0f;
}

void func_8098E554(DemoSa* this, PlayState* play) {
    u32* something = &D_80990108;

    if (play->csCtx.state == CS_STATE_IDLE) {
        if (*something != 0) {
            if (this->actor.params == 2) {
                func_8098E530(this);
            }
            *something = 0;
        }
    } else if (*something == 0) {
        *something = 1;
    }
}
#endif

void func_8098E5C8(DemoSa* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 DemoSa_UpdateSkelAnime(DemoSa* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorCue* DemoSa_GetCue(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

s32 func_8098E654(DemoSa* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = DemoSa_GetCue(play, cueChannel);

    if ((cue != NULL) && (cue->id == cueId)) {
        return true;
    }

    return false;
}

s32 func_8098E6A0(DemoSa* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = DemoSa_GetCue(play, cueChannel);

    if ((cue != NULL) && (cue->id != cueId)) {
        return true;
    }

    return false;
}

void func_8098E6EC(DemoSa* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = DemoSa_GetCue(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;

        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void func_8098E76C(DemoSa* this, AnimationHeader* animHeaderSeg, u8 arg2, f32 morphFrames, s32 arg4) {
    s32 pad[2];
    f32 frameCount = Animation_GetLastFrame(animHeaderSeg);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (arg4 == 0) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        fc = 0.0f;
        unk0 = frameCount;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&this->skelAnime, animHeaderSeg, playbackSpeed, unk0, fc, arg2, morphFrames);
}

#include "z_demo_sa_inKenjyanoma.inc.c"

#include "z_demo_sa_inKenjyanomaDemo02.inc.c"

#include "z_demo_sa_inStand.inc.c"

#include "z_demo_sa_inEnding.inc.c"

#include "z_demo_sa_inPresent.inc.c"

void DemoSa_Update(Actor* thisx, PlayState* play) {
    DemoSa* this = (DemoSa*)thisx;

    if (this->action < 0 || this->action >= 21 || sActionFuncs[this->action] == NULL) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, play);
}

void DemoSa_Init(Actor* thisx, PlayState* play) {
    DemoSa* this = (DemoSa*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);

    switch (this->actor.params) {
        case 2:
            func_8098ECF4(this, play);
            break;
        case 3:
            func_8098F390(this, play);
            break;
        case 4:
            func_8098F420(this, play);
            break;
        case 5:
            func_8098F83C(this, play);
            break;
        default:
            func_8098E7FC(this, play);
    }
}

s32 DemoSa_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    DemoSa* this = (DemoSa*)thisx;

    if ((limbIndex == 15) && (this->unk_1B0 != 0)) {
        *dList = gSariaRightHandAndOcarinaDL;
    }
    return false;
}

void DemoSa_DrawNothing(DemoSa* this, PlayState* play) {
}

void DemoSa_DrawOpa(DemoSa* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* eyeTex = sEyeTextures[eyeIndex];
    s32 pad2;
    s16 mouthIndex = this->mouthIndex;
    void* mouthTex = sMouthTextures[mouthIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_sa.c", 602);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          DemoSa_OverrideLimbDraw, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_sa.c", 626);
}

void DemoSa_Draw(Actor* thisx, PlayState* play) {
    DemoSa* this = (DemoSa*)thisx;

    if (this->drawConfig < 0 || this->drawConfig >= 3 || sDrawFuncs[this->drawConfig] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawConfig](this, play);
}
