/*
 * File: z_en_rl.c
 * Overlay: En_Rl
 * Description: Rauru
 */

#include "z_en_rl.h"
#include "terminal.h"
#include "assets/objects/object_rl/object_rl.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void EnRl_Init(Actor* thisx, PlayState* play);
void EnRl_Destroy(Actor* thisx, PlayState* play);
void EnRl_Update(Actor* thisx, PlayState* play);
void EnRl_Draw(Actor* thisx, PlayState* play);

void func_80AE7798(EnRl* this, PlayState* play);
void func_80AE77B8(EnRl* this, PlayState* play);
void func_80AE77F8(EnRl* this, PlayState* play);
void func_80AE7838(EnRl* this, PlayState* play);
void func_80AE7C64(EnRl* this, PlayState* play);
void func_80AE7C94(EnRl* this, PlayState* play);
void func_80AE7CE8(EnRl* this, PlayState* play);
void func_80AE7D40(EnRl* this, PlayState* play);
void func_80AE7FD0(EnRl* this, PlayState* play);
void func_80AE7FDC(EnRl* this, PlayState* play);
void func_80AE7D94(EnRl* this, PlayState* play);

static void* D_80AE81A0[] = { object_rl_Tex_003620, object_rl_Tex_003960, object_rl_Tex_003B60 };

void EnRl_Destroy(Actor* thisx, PlayState* play) {
    EnRl* this = (EnRl*)thisx;

    SkelAnime_Free(&this->skelAnime, play);
}

void func_80AE72D0(EnRl* this) {
    s32 pad[3];
    s16* timer = &this->timer;
    s16* eyeTextureIndex = &this->eyeTextureIndex;

    if (DECR(*timer) == 0) {
        *timer = Rand_S16Offset(60, 60);
    }

    *eyeTextureIndex = *timer;
    if (*eyeTextureIndex > 2) {
        *eyeTextureIndex = 0;
    }
}

#if DEBUG_FEATURES
void func_80AE7358(EnRl* this) {
    Animation_Change(&this->skelAnime, &object_rl_Anim_000A3C, 1.0f, 0.0f,
                     Animation_GetLastFrame(&object_rl_Anim_000A3C), ANIMMODE_LOOP, 0.0f);
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

void func_80AE744C(EnRl* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 func_80AE7494(EnRl* this) {
    return SkelAnime_Update(&this->skelAnime);
}

s32 func_80AE74B4(EnRl* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[cueChannel];

        if (cue != NULL && cue->id == cueId) {
            return true;
        }
    }
    return false;
}

s32 func_80AE74FC(EnRl* this, PlayState* play, u16 cueId, s32 cueChannel) {
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

static EnRlActionFunc sActionFuncs[] = {
    func_80AE7798, func_80AE77B8, func_80AE77F8, func_80AE7838,
    func_80AE7C64, func_80AE7C94, func_80AE7CE8, func_80AE7D40,
};

void EnRl_Update(Actor* thisx, PlayState* play) {
    EnRl* this = (EnRl*)thisx;

    if ((this->action < 0) || (this->action > 7) || (sActionFuncs[this->action] == NULL)) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, play);
}

void EnRl_Init(Actor* thisx, PlayState* play) {
    EnRl* this = (EnRl*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 50.0f);
    if (this->actor.params == 2) {
        func_80AE7878(this, play);
    } else {
        func_80AE7544(this, play);
    }
}
void func_80AE7FD0(EnRl* this, PlayState* play) {
}

void func_80AE7FDC(EnRl* this, PlayState* play) {
    s32 pad[2];
    s16 temp = this->eyeTextureIndex;
    void* tex = D_80AE81A0[temp];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_rl.c", 416);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL,
                          &this->actor);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_rl.c", 437);
}

static EnRlDrawFunc sDrawFuncs[] = {
    func_80AE7FD0,
    func_80AE7FDC,
    func_80AE7D94,
};

void EnRl_Draw(Actor* thisx, PlayState* play) {
    EnRl* this = (EnRl*)thisx;

    if (this->drawConfig < 0 || this->drawConfig >= 3 || sDrawFuncs[this->drawConfig] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawConfig](this, play);
}

ActorProfile En_Rl_Profile = {
    /**/ ACTOR_EN_RL,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_RL,
    /**/ sizeof(EnRl),
    /**/ EnRl_Init,
    /**/ EnRl_Destroy,
    /**/ EnRl_Update,
    /**/ EnRl_Draw,
};
