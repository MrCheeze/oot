#include "z_demo_du.h"
#include "assets/objects/object_du/object_du.h"
#include "overlays/actors/ovl_Demo_Effect/z_demo_effect.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef void (*DemoDuActionFunc)(DemoDu*, PlayState*);
typedef void (*DemoDuDrawFunc)(Actor*, PlayState*);

void Demo_Du_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Du_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Du_Actor_main(Actor* thisx, PlayState* play);
void Demo_Du_Actor_draw(Actor* thisx, PlayState* play);

#include "Demodt_Kenjyanoma.inc.c"

static void* demo_du_eye[] = { gDaruniaEyeOpenTex, gDaruniaEyeOpeningTex, gDaruniaEyeShutTex, gDaruniaEyeClosingTex };
static void* demo_du_mouth[] = { gDaruniaMouthSeriousTex, gDaruniaMouthGrinningTex, gDaruniaMouthOpenTex,
                                  gDaruniaMouthHappyTex };

/**
 * Cs => Cutscene
 *
 * FM => Fire Medallion
 * GR => Goron's Ruby
 * AG => In the chamber of sages, just After the final blow on Ganon.
 * CR => Credits
 *
 */

// Each macro maps its argument to an index of proc.
#define CS_FIREMEDALLION_SUBSCENE(x) (0 + (x))      // DEMO_DU_CS_FIREMEDALLION
#define CS_GORONSRUBY_SUBSCENE(x) (7 + (x))         // DEMO_DU_CS_GORONS_RUBY
#define CS_CHAMBERAFTERGANON_SUBSCENE(x) (21 + (x)) // DEMO_DU_CS_CHAMBER_AFTER_GANON
#define CS_CREDITS_SUBSCENE(x) (24 + (x))           // DEMO_DU_CS_CREDITS

void Demo_Du_Actor_dt(Actor* thisx, PlayState* play) {
    DemoDu* this = (DemoDu*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
}

void Demo_Du_set_eye_pattern(DemoDu* this) {
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeTexIndex = &this->eyeTexIndex;
    s32 pad[3];

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = get_random_timer(60, 60);
    }

    *eyeTexIndex = *blinkTimer;
    if (*eyeTexIndex >= 3) {
        *eyeTexIndex = 0;
    }
}

void Demo_Du_set_eye_Num(DemoDu* this, s16 eyeTexIndex) {
    this->eyeTexIndex = eyeTexIndex;
}

void Demo_Du_set_mouth_Num(DemoDu* this, s16 mouthTexIndex) {
    this->mouthTexIndex = mouthTexIndex;
}

#if DEBUG_FEATURES
// Resets all the values used in this cutscene.
void DemoDu_CsAfterGanon_Reset(DemoDu* this) {
    this->updateIndex = CS_CHAMBERAFTERGANON_SUBSCENE(0);
    this->drawIndex = 0;
    this->shadowAlpha = 0;
    this->demo6KSpawned = 0;
    this->actor.shape.shadowAlpha = 0;
    this->unk_1A4 = 0.0f;
}

void DemoDu_CsAfterGanon_CheckIfShouldReset(DemoDu* this, PlayState* play) {
    static s32 D_8096CE94 = false;

    if (play->csCtx.state == CS_STATE_IDLE) {
        if (D_8096CE94) {
            if (this->actor.params == DEMO_DU_CS_CHAMBER_AFTER_GANON) {
                DemoDu_CsAfterGanon_Reset(this);
            }
            D_8096CE94 = false;
        }
    } else if (!D_8096CE94) {
        D_8096CE94 = true;
    }
}
#endif

s32 Demo_Du_Animation_Base(DemoDu* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

void Demo_Du_BGcheck(DemoDu* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

CsCmdActorCue* Demo_Du_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

s32 Demo_Du_Check_npcdemopnt(DemoDu* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Du_Get_npcdemopnt(play, cueChannel);

    if ((cue != NULL) && (cue->id == cueId)) {
        return true;
    }
    return false;
}

s32 Demo_Du_Check2_npcdemopnt(DemoDu* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Du_Get_npcdemopnt(play, cueChannel);

    if ((cue != NULL) && (cue->id != cueId)) {
        return true;
    }
    return false;
}

void Demo_Du_Set_StartPos_npcdemopnt(DemoDu* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Du_Get_npcdemopnt(play, cueChannel);
    s32 pad;

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;

        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void Demo_Du_Change_Anime(DemoDu* this, AnimationHeader* animation, u8 mode, f32 morphFrames, s32 arg4) {
    f32 startFrame;
    s16 lastFrame = Si2_anime_end_frame(animation);
    f32 endFrame;
    f32 playSpeed;

    if (arg4 == 0) {
        startFrame = 0.0f;
        endFrame = lastFrame;
        playSpeed = 1.0f;
    } else {
        endFrame = 0.0f;
        playSpeed = -1.0f;
        startFrame = lastFrame;
    }
    Skeleton_Info2_init(&this->skelAnime, animation, playSpeed, startFrame, endFrame, mode, morphFrames);
}

#include "z_demo_du_inKenjyanoma.inc.c"

#include "z_demo_du_inSpot16.inc.c"

#include "z_demo_du_inKenjyanomaDemo02.inc.c"

#include "z_demo_du_inEnding.inc.c"

void Demo_Du_Actor_main(Actor* thisx, PlayState* play) {
    static DemoDuActionFunc proc[] = {
        Demo_Du_Actor_main_wait, Demo_Du_Actor_main_hide, Demo_Du_Actor_main_up, Demo_Du_Actor_main_greet, Demo_Du_Actor_main_handup,
        Demo_Du_Actor_main_cheer, Demo_Du_Actor_main_stop, Demo_Du_Actor_main_cryst_wait, Demo_Du_Actor_main_cryst_hide, Demo_Du_Actor_main_cryst_down,
        Demo_Du_Actor_main_cryst_land, Demo_Du_Actor_main_cryst_standup, Demo_Du_Actor_main_cryst_greet, Demo_Du_Actor_main_cryst_impress, Demo_Du_Actor_main_cryst_thanks,
        Demo_Du_Actor_main_cryst_hit, Demo_Du_Actor_main_cryst_impress2, Demo_Du_Actor_main_cryst_ready, Demo_Du_Actor_main_cryst_handup, Demo_Du_Actor_main_cryst_recall,
        Demo_Du_Actor_main_cryst_stop, Demo_Du_Seal_Actor_main_hide, Demo_Du_Seal_Actor_main_fade, Demo_Du_Seal_Actor_main_pray, Demo_Du_inEnding_main_wait,
        Demo_Du_inEnding_main_alpha, Demo_Du_inEnding_main_stand, Demo_Du_inEnding_main_lookup, Demo_Du_inEnding_main_lookdown,
    };

    DemoDu* this = (DemoDu*)thisx;

    if (this->updateIndex < 0 || this->updateIndex >= 29 || proc[this->updateIndex] == NULL) {
        // "The main mode is abnormal!!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->updateIndex](this, play);
}

void Demo_Du_Actor_ct(Actor* thisx, PlayState* play) {
    DemoDu* this = (DemoDu*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    switch (this->actor.params) {
        case DEMO_DU_CS_GORONS_RUBY:
            Demo_Du_Actor_Cryst_Init(this, play);
            break;

        case DEMO_DU_CS_CHAMBER_AFTER_GANON:
            Demo_Du_KenjyanomaDemo02_Init(this, play);
            break;

        case DEMO_DU_CS_CREDITS:
            Demo_Du_Ending_Init(this, play);
            break;

        default:
            Demo_Du_Actor_Kenjyanoma_Init(this, play);
            break;
    }
}

void Demo_Du_Actor_draw_none(Actor* thisx, PlayState* play2) {
}

// Similar to Demo_Du_Actor_draw_alpha, but this uses POLY_OPA_DISP. Sets the env color to 255.
void Demo_Du_Actor_draw_normal(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DemoDu* this = (DemoDu*)thisx;
    s16 eyeTexIndex = this->eyeTexIndex;
    void* eyeTexture = demo_du_eye[eyeTexIndex];
    s32 pad;
    s16 mouthTexIndex = this->mouthTexIndex;
    void* mouthTexture = demo_du_mouth[mouthTexIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_du.c", 615);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTexture));
    gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gDaruniaNoseSeriousTex));

    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);

    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_du.c", 638);
}

void Demo_Du_Actor_draw(Actor* thisx, PlayState* play) {
    static DemoDuDrawFunc proc[] = {
        Demo_Du_Actor_draw_none,
        Demo_Du_Actor_draw_normal,
        Demo_Du_Actor_draw_alpha,
    };

    DemoDu* this = (DemoDu*)thisx;

    if (this->drawIndex < 0 || this->drawIndex >= 3 || proc[this->drawIndex] == NULL) {
        // "The drawing mode is abnormal!!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->drawIndex](thisx, play);
}

ActorProfile Demo_Du_Profile = {
    /**/ ACTOR_DEMO_DU,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DU,
    /**/ sizeof(DemoDu),
    /**/ Demo_Du_Actor_ct,
    /**/ Demo_Du_Actor_dt,
    /**/ Demo_Du_Actor_main,
    /**/ Demo_Du_Actor_draw,
};
