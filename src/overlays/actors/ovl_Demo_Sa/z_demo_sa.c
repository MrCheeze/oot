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

void Demo_Sa_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Sa_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Sa_Actor_main(Actor* thisx, PlayState* play);
void Demo_Sa_Actor_draw(Actor* thisx, PlayState* play);

void Demo_Sa_Actor_main_wait(DemoSa* this, PlayState* play);
void Demo_Sa_Actor_main_hide(DemoSa* this, PlayState* play);
void Demo_Sa_Actor_main_up(DemoSa* this, PlayState* play);
void Demo_Sa_Actor_main_greet(DemoSa* this, PlayState* play);
void Demo_Sa_Actor_main_handup(DemoSa* this, PlayState* play);
void Demo_Sa_Actor_main_cheer(DemoSa* this, PlayState* play);
void Demo_Sa_Actor_main_stop(DemoSa* this, PlayState* play);
void Demo_Sa_Seal_Actor_main_hide(DemoSa* this, PlayState* play);
void Demo_Sa_Seal_Actor_main_fade(DemoSa* this, PlayState* play);
void Demo_Sa_Seal_Actor_main_pray(DemoSa* this, PlayState* play);
void Demo_Sa_Stand_Actor_main_gaze(DemoSa* this, PlayState* play);
void Demo_Sa_inEnding_main_wait(DemoSa* this, PlayState* play);
void Demo_Sa_inEnding_main_alpha(DemoSa* this, PlayState* play);
void Demo_Sa_inEnding_main_stand(DemoSa* this, PlayState* play);
void Demo_Sa_inEnding_main_lookup(DemoSa* this, PlayState* play);
void Demo_Sa_inEnding_main_lookdown(DemoSa* this, PlayState* play);
void Demo_Sa_inPresent_main_wait(DemoSa* this, PlayState* play);
void Demo_Sa_inPresent_main_alpha(DemoSa* this, PlayState* play);
void Demo_Sa_inPresent_main_stand(DemoSa* this, PlayState* play);
void Demo_Sa_inPresent_main_hold(DemoSa* this, PlayState* play);
void Demo_Sa_inPresent_main_send(DemoSa* this, PlayState* play);

void Demo_Sa_Actor_draw_none(DemoSa* this, PlayState* play);
void Demo_Sa_Actor_draw_normal(DemoSa* this, PlayState* play);
void Demo_Sa_Actor_draw_alpha(DemoSa* this, PlayState* play);

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

static void* demo_sa_eye[] = {
    gSariaEyeOpenTex, gSariaEyeHalfTex, gSariaEyeClosedTex, gSariaEyeSuprisedTex, gSariaEyeSadTex,
};

static void* demo_sa_mouth[] = {
    gSariaMouthClosed2Tex,     gSariaMouthSuprisedTex, gSariaMouthClosedTex,
    gSariaMouthSmilingOpenTex, gSariaMouthFrowningTex,
};

#if DEBUG_FEATURES
static u32 D_80990108 = 0;
#endif

#include "Demodt_Kenjyanoma.inc.c"

void Demo_Sa_Actor_dt(Actor* thisx, PlayState* play) {
    DemoSa* this = (DemoSa*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
}

void Demo_Sa_set_eye_pattern(DemoSa* this) {
    s32 pad[2];
    s16* eyeIndex = &this->eyeIndex;
    s16* blinkTimer = &this->blinkTimer;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = get_random_timer(0x3C, 0x3C);
    }

    *eyeIndex = *blinkTimer;
    if (*eyeIndex >= 3) {
        *eyeIndex = 0;
    }
}

void Demo_Sa_set_eye_Num(DemoSa* this, s16 eyeIndex) {
    this->eyeIndex = eyeIndex;
}

void Demo_Sa_set_mouth_Num(DemoSa* this, s16 mouthIndex) {
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

void Demo_Sa_BGcheck(DemoSa* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 Demo_Sa_Animation_Base(DemoSa* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

CsCmdActorCue* Demo_Sa_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

s32 Demo_Sa_Check_npcdemopnt(DemoSa* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Sa_Get_npcdemopnt(play, cueChannel);

    if ((cue != NULL) && (cue->id == cueId)) {
        return true;
    }

    return false;
}

s32 Demo_Sa_Check2_npcdemopnt(DemoSa* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Sa_Get_npcdemopnt(play, cueChannel);

    if ((cue != NULL) && (cue->id != cueId)) {
        return true;
    }

    return false;
}

void Demo_Sa_Set_StartPos_npcdemopnt(DemoSa* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Sa_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;

        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void Demo_Sa_Change_Anime(DemoSa* this, AnimationHeader* animHeaderSeg, u8 arg2, f32 morphFrames, s32 arg4) {
    s32 pad[2];
    f32 frameCount = Si2_anime_end_frame(animHeaderSeg);
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

    Skeleton_Info2_init(&this->skelAnime, animHeaderSeg, playbackSpeed, unk0, fc, arg2, morphFrames);
}

#include "z_demo_sa_inKenjyanoma.inc.c"

#include "z_demo_sa_inKenjyanomaDemo02.inc.c"

#include "z_demo_sa_inStand.inc.c"

#include "z_demo_sa_inEnding.inc.c"

#include "z_demo_sa_inPresent.inc.c"

void Demo_Sa_Actor_main(Actor* thisx, PlayState* play) {
    static DemoSaActionFunc proc[] = {
        Demo_Sa_Actor_main_wait, Demo_Sa_Actor_main_hide, Demo_Sa_Actor_main_up, Demo_Sa_Actor_main_greet, Demo_Sa_Actor_main_handup, Demo_Sa_Actor_main_cheer, Demo_Sa_Actor_main_stop,
        Demo_Sa_Seal_Actor_main_hide, Demo_Sa_Seal_Actor_main_fade, Demo_Sa_Seal_Actor_main_pray, Demo_Sa_Stand_Actor_main_gaze, Demo_Sa_inEnding_main_wait, Demo_Sa_inEnding_main_alpha, Demo_Sa_inEnding_main_stand,
        Demo_Sa_inEnding_main_lookup, Demo_Sa_inEnding_main_lookdown, Demo_Sa_inPresent_main_wait, Demo_Sa_inPresent_main_alpha, Demo_Sa_inPresent_main_stand, Demo_Sa_inPresent_main_hold, Demo_Sa_inPresent_main_send,
    };

    DemoSa* this = (DemoSa*)thisx;

    if (this->action < 0 || this->action >= 21 || proc[this->action] == NULL) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->action](this, play);
}

void Demo_Sa_Actor_ct(Actor* thisx, PlayState* play) {
    DemoSa* this = (DemoSa*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);

    switch (this->actor.params) {
        case 2:
            Demo_Sa_KenjyanomaDemo02_Init(this, play);
            break;
        case 3:
            Demo_Sa_Stand_Init(this, play);
            break;
        case 4:
            Demo_Sa_Ending_Init(this, play);
            break;
        case 5:
            Demo_Sa_Present_Init(this, play);
            break;
        default:
            Demo_Sa_Kenjyanoma_Init(this, play);
    }
}

s32 Demo_Sa_beforedraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    DemoSa* this = (DemoSa*)thisx;

    if ((limbIndex == 15) && (this->unk_1B0 != 0)) {
        *dList = gSariaRightHandAndOcarinaDL;
    }
    return false;
}

void Demo_Sa_Actor_draw_none(DemoSa* this, PlayState* play) {
}

void Demo_Sa_Actor_draw_normal(DemoSa* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* eyeTex = demo_sa_eye[eyeIndex];
    s32 pad2;
    s16 mouthIndex = this->mouthIndex;
    void* mouthTex = demo_sa_mouth[mouthIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_sa.c", 602);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouthTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);

    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          Demo_Sa_beforedraw, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_sa.c", 626);
}

void Demo_Sa_Actor_draw(Actor* thisx, PlayState* play) {
    static DemoSaDrawFunc proc[] = {
        Demo_Sa_Actor_draw_none,
        Demo_Sa_Actor_draw_normal,
        Demo_Sa_Actor_draw_alpha,
    };

    DemoSa* this = (DemoSa*)thisx;

    if (this->drawConfig < 0 || this->drawConfig >= 3 || proc[this->drawConfig] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->drawConfig](this, play);
}

ActorProfile Demo_Sa_Profile = {
    /**/ ACTOR_DEMO_SA,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SA,
    /**/ sizeof(DemoSa),
    /**/ Demo_Sa_Actor_ct,
    /**/ Demo_Sa_Actor_dt,
    /**/ Demo_Sa_Actor_main,
    /**/ Demo_Sa_Actor_draw,
};
