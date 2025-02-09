#include "z_demo_ik.h"
#include "terminal.h"
#include "assets/objects/object_ik/object_ik.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Demo_Ik_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Ik_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Ik_main(Actor* thisx, PlayState* play);
void Demo_Ik_draw(Actor* thisx, PlayState* play);

void Demo_Ik_inArmer_Init(DemoIk* this, PlayState* play);
void Demo_Ik_inFace_Init(DemoIk* this, PlayState* play);

void Demo_Ik_main_wait(DemoIk* this, PlayState* play);
void Demo_Ik_main_stick(DemoIk* this, PlayState* play);
void Demo_Ik_main_drop(DemoIk* this, PlayState* play);
void Demo_Ik_inFace_main_wait(DemoIk* this, PlayState* play);
void Demo_Ik_inFace_main_stick(DemoIk* this, PlayState* play);
void Demo_Ik_inFace_main_drop(DemoIk* this, PlayState* play);

void Demo_Ik_draw_none(DemoIk* this, PlayState* play);
void Demo_Ik_draw_normal(DemoIk* this, PlayState* play);
void Demo_Ik_inFace_draw_normal(DemoIk* this, PlayState* play);

void Demo_Ik_Actor_dt(Actor* thisx, PlayState* play) {
}

void Demo_Ik_BGcheck(DemoIk* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 Demo_Ik_Animation_Base(DemoIk* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

CsCmdActorCue* Demo_Ik_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

s32 Demo_Ik_Check_npcdemopnt(PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Ik_Get_npcdemopnt(play, cueChannel);

    if ((cue != NULL) && (cue->id == cueId)) {
        return 1;
    }
    return 0;
}

void Demo_Ik_Movement_byAnimation(DemoIk* this, PlayState* play) {
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ;
    Skeleton_Proc_Anime_Move_init(play, &this->actor, &this->skelAnime, 1.0f);
}

void En_Ik_End_Movement_byAnimation(DemoIk* this) {
    this->skelAnime.movementFlags &= ~ANIM_FLAG_UPDATE_XZ;
}

f32 Demo_Ik_Get_anime_frame(DemoIk* this) {
    return this->skelAnime.curFrame;
}

Gfx* Demo_Ik_Setcolor(GraphicsContext* gfxCtx, u8 primR, u8 primG, u8 primB, u8 envR, u8 envG, u8 envB) {
    Gfx* head = GRAPH_ALLOC(gfxCtx, 3 * sizeof(Gfx));
    Gfx* entry = head;

    gDPSetPrimColor(entry++, 0x00, 0x00, primR, primG, primB, 255);
    gDPSetEnvColor(entry++, envR, envG, envB, 255);
    gSPEndDisplayList(entry++);
    return head;
}

#include "z_demo_ik_inArmer.inc.c"

#include "z_demo_ik_inFace.inc.c"

void Demo_Ik_main(Actor* thisx, PlayState* play) {
    static DemoIkActionFunc proc[] = {
        Demo_Ik_main_wait, Demo_Ik_main_stick, Demo_Ik_main_drop,
        Demo_Ik_inFace_main_wait, Demo_Ik_inFace_main_stick, Demo_Ik_inFace_main_drop,
    };

    s32 pad;
    DemoIk* this = (DemoIk*)thisx;

    if (this->actionMode < 0 || this->actionMode >= ARRAY_COUNT(proc) ||
        proc[this->actionMode] == NULL) {
        // "The main mode is strange"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->actionMode](this, play);
}

void Demo_Ik_draw_none(DemoIk* this, PlayState* play) {
}

void Demo_Ik_draw(Actor* thisx, PlayState* play) {
    static DemoIkDrawFunc proc[] = {
        Demo_Ik_draw_none,
        Demo_Ik_draw_normal,
        Demo_Ik_inFace_draw_normal,
    };

    s32 pad;
    DemoIk* this = (DemoIk*)thisx;

    if (this->drawMode < 0 || this->drawMode >= ARRAY_COUNT(proc) || proc[this->drawMode] == NULL) {
        // "The draw mode is strange"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->drawMode](this, play);
}

ActorProfile Demo_Ik_Profile = {
    /**/ ACTOR_DEMO_IK,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_IK,
    /**/ sizeof(DemoIk),
    /**/ Demo_Ik_Actor_ct,
    /**/ Demo_Ik_Actor_dt,
    /**/ Demo_Ik_main,
    /**/ Demo_Ik_draw,
};

void Demo_Ik_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoIk* this = (DemoIk*)thisx;

    if (this->actor.params == 0 || this->actor.params == 1 || this->actor.params == 2) {
        Demo_Ik_inArmer_Init(this, play);
    } else {
        Demo_Ik_inFace_Init(this, play);
    }
}
