#include "z_demo_ik.h"
#include "terminal.h"
#include "assets/objects/object_ik/object_ik.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void DemoIk_Init(Actor* thisx, PlayState* play);
void DemoIk_Destroy(Actor* thisx, PlayState* play);
void DemoIk_Update(Actor* thisx, PlayState* play);
void DemoIk_Draw(Actor* thisx, PlayState* play);

void DemoIk_Type1Init(DemoIk* this, PlayState* play);
void DemoIk_Type2Init(DemoIk* this, PlayState* play);

void DemoIk_Type1Action0(DemoIk* this, PlayState* play);
void DemoIk_Type1Action1(DemoIk* this, PlayState* play);
void DemoIk_Type1Action2(DemoIk* this, PlayState* play);
void DemoIk_Type2Action0(DemoIk* this, PlayState* play);
void DemoIk_Type2Action1(DemoIk* this, PlayState* play);
void DemoIk_Type2Action2(DemoIk* this, PlayState* play);

void DemoIk_DrawNothing(DemoIk* this, PlayState* play);
void DemoIk_Type1Draw(DemoIk* this, PlayState* play);
void DemoIk_Type2Draw(DemoIk* this, PlayState* play);

void DemoIk_Destroy(Actor* thisx, PlayState* play) {
}

void DemoIk_BgCheck(DemoIk* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 DemoIk_UpdateSkelAnime(DemoIk* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorCue* DemoIk_GetCue(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

s32 DemoIk_CheckForCue(PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = DemoIk_GetCue(play, cueChannel);

    if ((cue != NULL) && (cue->id == cueId)) {
        return 1;
    }
    return 0;
}

void DemoIk_SetMove(DemoIk* this, PlayState* play) {
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ;
    AnimTaskQueue_AddActorMovement(play, &this->actor, &this->skelAnime, 1.0f);
}

void DemoIk_EndMove(DemoIk* this) {
    this->skelAnime.movementFlags &= ~ANIM_FLAG_UPDATE_XZ;
}

f32 DemoIk_GetCurFrame(DemoIk* this) {
    return this->skelAnime.curFrame;
}

Gfx* DemoIk_SetColors(GraphicsContext* gfxCtx, u8 primR, u8 primG, u8 primB, u8 envR, u8 envG, u8 envB) {
    Gfx* head = GRAPH_ALLOC(gfxCtx, 3 * sizeof(Gfx));
    Gfx* entry = head;

    gDPSetPrimColor(entry++, 0x00, 0x00, primR, primG, primB, 255);
    gDPSetEnvColor(entry++, envR, envG, envB, 255);
    gSPEndDisplayList(entry++);
    return head;
}

#include "z_demo_ik_inArmer.inc.c"

#include "z_demo_ik_inFace.inc.c"

static DemoIkActionFunc sActionFuncs[] = {
    DemoIk_Type1Action0, DemoIk_Type1Action1, DemoIk_Type1Action2,
    DemoIk_Type2Action0, DemoIk_Type2Action1, DemoIk_Type2Action2,
};

void DemoIk_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoIk* this = (DemoIk*)thisx;

    if (this->actionMode < 0 || this->actionMode >= ARRAY_COUNT(sActionFuncs) ||
        sActionFuncs[this->actionMode] == NULL) {
        // "The main mode is strange"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->actionMode](this, play);
}

void DemoIk_DrawNothing(DemoIk* this, PlayState* play) {
}

static DemoIkDrawFunc sDrawFuncs[] = {
    DemoIk_DrawNothing,
    DemoIk_Type1Draw,
    DemoIk_Type2Draw,
};

void DemoIk_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoIk* this = (DemoIk*)thisx;

    if (this->drawMode < 0 || this->drawMode >= ARRAY_COUNT(sDrawFuncs) || sDrawFuncs[this->drawMode] == NULL) {
        // "The draw mode is strange"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawMode](this, play);
}

ActorProfile Demo_Ik_Profile = {
    /**/ ACTOR_DEMO_IK,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_IK,
    /**/ sizeof(DemoIk),
    /**/ DemoIk_Init,
    /**/ DemoIk_Destroy,
    /**/ DemoIk_Update,
    /**/ DemoIk_Draw,
};

void DemoIk_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoIk* this = (DemoIk*)thisx;

    if (this->actor.params == 0 || this->actor.params == 1 || this->actor.params == 2) {
        DemoIk_Type1Init(this, play);
    } else {
        DemoIk_Type2Init(this, play);
    }
}
