/*
 * File: z_en_rl.c
 * Overlay: En_Rl
 * Description: Rauru
 */

#include "z_en_rl.h"
#include "terminal.h"
#include "assets/objects/object_rl/object_rl.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Rl_Actor_ct(Actor* thisx, PlayState* play);
void En_Rl_Actor_dt(Actor* thisx, PlayState* play);
void En_Rl_Actor_main(Actor* thisx, PlayState* play);
void En_Rl_Actor_draw(Actor* thisx, PlayState* play);

void En_Rl_Actor_main_wait(EnRl* this, PlayState* play);
void En_Rl_Actor_main_greet(EnRl* this, PlayState* play);
void En_Rl_Actor_main_handup(EnRl* this, PlayState* play);
void En_Rl_Actor_main_cheer(EnRl* this, PlayState* play);
void En_Rl_Seal_Actor_main_hide(EnRl* this, PlayState* play);
void En_Rl_Seal_Actor_main_fade(EnRl* this, PlayState* play);
void En_Rl_Seal_Actor_main_handup(EnRl* this, PlayState* play);
void En_Rl_Seal_Actor_main_pray(EnRl* this, PlayState* play);
void En_Rl_Actor_draw_none(EnRl* this, PlayState* play);
void En_Rl_Actor_draw_normal(EnRl* this, PlayState* play);
void En_Rl_Actor_draw_alpha(EnRl* this, PlayState* play);

static void* en_rl_eye[] = { object_rl_Tex_003620, object_rl_Tex_003960, object_rl_Tex_003B60 };

void En_Rl_Actor_dt(Actor* thisx, PlayState* play) {
    EnRl* this = (EnRl*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
}

void En_Rl_set_eye_pattern(EnRl* this) {
    s32 pad[3];
    s16* timer = &this->timer;
    s16* eyeTextureIndex = &this->eyeTextureIndex;

    if (DECR(*timer) == 0) {
        *timer = get_random_timer(60, 60);
    }

    *eyeTextureIndex = *timer;
    if (*eyeTextureIndex > 2) {
        *eyeTextureIndex = 0;
    }
}

#if DEBUG_FEATURES
void func_80AE7358(EnRl* this) {
    Skeleton_Info2_init(&this->skelAnime, &object_rl_Anim_000A3C, 1.0f, 0.0f,
                     Si2_anime_end_frame(&object_rl_Anim_000A3C), ANIMMODE_LOOP, 0.0f);
    this->action = 4;
    this->drawConfig = 0;
    this->alpha = 0;
    this->lightBallSpawned = 0;
    this->actor.shape.shadowAlpha = 0;
    this->unk_19C = 0.0f;
}

void func_80AE73D8(EnRl* this, PlayState* play) {
    static s32 D_80AE81AC = 0;

    if (play->csCtx.state == CS_STATE_IDLE) {
        if (D_80AE81AC) {
            if (this->actor.params == 2) {
                func_80AE7358(this);
            }
            D_80AE81AC = 0;
        }
    } else if (!D_80AE81AC) {
        D_80AE81AC = 1;
    }
}
#endif

void En_Rl_BGcheck(EnRl* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 En_Rl_Animation_Base(EnRl* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

s32 En_Rl_Check_npcdemopnt(EnRl* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[cueChannel];

        if (cue != NULL && cue->id == cueId) {
            return true;
        }
    }
    return false;
}

s32 En_Rl_Check2_npcdemopnt(EnRl* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[cueChannel];

        if (cue != NULL && cue->id != cueId) {
            return true;
        }
    }
    return false;
}

#include "z_en_rl_inKenjyanoma.inc.c"

#include "z_en_rl_inKenjyanomaDemo02.inc.c"

void En_Rl_Actor_main(Actor* thisx, PlayState* play) {
    static EnRlActionFunc proc[] = {
        En_Rl_Actor_main_wait, En_Rl_Actor_main_greet, En_Rl_Actor_main_handup, En_Rl_Actor_main_cheer,
        En_Rl_Seal_Actor_main_hide, En_Rl_Seal_Actor_main_fade, En_Rl_Seal_Actor_main_handup, En_Rl_Seal_Actor_main_pray,
    };

    EnRl* this = (EnRl*)thisx;

    if ((this->action < 0) || (this->action > 7) || (proc[this->action] == NULL)) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->action](this, play);
}

void En_Rl_Actor_ct(Actor* thisx, PlayState* play) {
    EnRl* this = (EnRl*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 50.0f);
    if (this->actor.params == 2) {
        En_Rl_KenjyanomaDemo02_Init(this, play);
    } else {
        En_Rl_Kenjyanoma_Init(this, play);
    }
}
void En_Rl_Actor_draw_none(EnRl* this, PlayState* play) {
}

void En_Rl_Actor_draw_normal(EnRl* this, PlayState* play) {
    s32 pad[2];
    s16 temp = this->eyeTextureIndex;
    void* tex = en_rl_eye[temp];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_rl.c", 416);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);

    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL,
                          &this->actor);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_rl.c", 437);
}

void En_Rl_Actor_draw(Actor* thisx, PlayState* play) {
    static EnRlDrawFunc proc[] = {
        En_Rl_Actor_draw_none,
        En_Rl_Actor_draw_normal,
        En_Rl_Actor_draw_alpha,
    };

    EnRl* this = (EnRl*)thisx;

    if (this->drawConfig < 0 || this->drawConfig >= 3 || proc[this->drawConfig] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->drawConfig](this, play);
}

ActorProfile En_Rl_Profile = {
    /**/ ACTOR_EN_RL,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_RL,
    /**/ sizeof(EnRl),
    /**/ En_Rl_Actor_ct,
    /**/ En_Rl_Actor_dt,
    /**/ En_Rl_Actor_main,
    /**/ En_Rl_Actor_draw,
};
