/*
 * File: z_demo_ext.c
 * Overlay: Demo_Ext
 * Description: Magic Vortex in Silver Gauntlets Cutscene
 */

#include "z_demo_ext.h"
#include "terminal.h"
#include "assets/objects/object_fhg/object_fhg.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef enum DemoExtAction {
    /* 0x00 */ EXT_WAIT,
    /* 0x01 */ EXT_MAINTAIN,
    /* 0x02 */ EXT_DISPELL
} DemoExtAction;

typedef enum DemoExtDrawMode {
    /* 0x00 */ EXT_DRAW_NOTHING,
    /* 0x01 */ EXT_DRAW_VORTEX
} DemoExtDrawMode;

void Demo_Ext_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Ext_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Ext_main(Actor* thisx, PlayState* play);
void Demo_Ext_draw(Actor* thisx, PlayState* play);

void Demo_Ext_Actor_dt(Actor* thisx, PlayState* play) {
}

void Demo_Ext_Actor_ct(Actor* thisx, PlayState* play) {
    DemoExt* this = (DemoExt*)thisx;

    this->scrollIncr[0] = 25;
    this->scrollIncr[1] = 40;
    this->scrollIncr[2] = 5;
    this->scrollIncr[3] = 30;
    this->primAlpha = kREG(28) + 255;
    this->envAlpha = kREG(32) + 255;
    this->scale.x = kREG(19) + 400.0f;
    this->scale.y = kREG(20) + 100.0f;
    this->scale.z = kREG(21) + 400.0f;
}

void Demo_Ext_Set_SoundGrown(DemoExt* this) {
    if (this->alphaTimer <= (kREG(35) + 40.0f) - 15.0f) {
        Nai_FxFlagEntry(NA_SE_EV_FANTOM_WARP_L - SFX_FLAG, &this->actor.projectedPos, 4,
                             &_dummy_one, &_dummy_one, &_dummy_zero_s8);
    }
}

CsCmdActorCue* Demo_Ext_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

void Demo_Ext_setup_Wait(DemoExt* this) {
    this->action = EXT_WAIT;
    this->drawMode = EXT_DRAW_NOTHING;
}

void Demo_Ext_setup_Display(DemoExt* this, PlayState* play) {
    CsCmdActorCue* cue = Demo_Ext_Get_npcdemopnt(play, 5);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
    this->action = EXT_MAINTAIN;
    this->drawMode = EXT_DRAW_VORTEX;
}

void Demo_Ext_setup_Fade(DemoExt* this) {
    this->action = EXT_DISPELL;
    this->drawMode = EXT_DRAW_VORTEX;
}

void Demo_Ext_setup_FadeToDisappear(DemoExt* this) {
    this->alphaTimer += 1.0f;
    if ((kREG(35) + 40.0f) <= this->alphaTimer) {
        Actor_delete(&this->actor);
    }
}

void Demo_Ext_Check_DemoMode(DemoExt* this, PlayState* play) {
    CsCmdActorCue* cue = Demo_Ext_Get_npcdemopnt(play, 5);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->id;
        currentCueId = this->cueId;

        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 1:
                    Demo_Ext_setup_Wait(this);
                    break;
                case 2:
                    Demo_Ext_setup_Display(this, play);
                    break;
                case 3:
                    Demo_Ext_setup_Fade(this);
                    break;
                default:
                    // "Demo_Ext_Check_DemoMode: there is no such action!"
                    PRINTF("Demo_Ext_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->cueId = nextCueId;
        }
    }
}

void Demo_Ext_Scroll_Texture(DemoExt* this) {
    s16* scrollIncr = this->scrollIncr;
    s16* curScroll = this->curScroll;
    s32 i;

    for (i = 3; i != 0; i--) {
        curScroll[i] += scrollIncr[i];
    }
    this->rotationPitch += (s16)(kREG(34) + 1000);
}

void Demo_Ext_Calc_fade(DemoExt* this) {
    Vec3f* scale = &this->scale;
    f32 shrinkFactor;

    shrinkFactor = ((kREG(35) + 40.0f) - this->alphaTimer) / (kREG(35) + 40.0f);
    if (shrinkFactor < 0.0f) {
        shrinkFactor = 0.0f;
    }

    this->primAlpha = (u32)(kREG(28) + 255) * shrinkFactor;
    this->envAlpha = (u32)(kREG(32) + 255) * shrinkFactor;
    scale->x = (kREG(19) + 400.0f) * shrinkFactor;
    scale->y = (kREG(20) + 100.0f) * shrinkFactor;
    scale->z = (kREG(21) + 400.0f) * shrinkFactor;
}

void Demo_Ext_main_wait(DemoExt* this, PlayState* play) {
    Demo_Ext_Check_DemoMode(this, play);
}

void Demo_Ext_main_display(DemoExt* this, PlayState* play) {
    Demo_Ext_Set_SoundGrown(this);
    Demo_Ext_Scroll_Texture(this);
    Demo_Ext_Check_DemoMode(this, play);
}

void Demo_Ext_main_fade(DemoExt* this, PlayState* play) {
    Demo_Ext_Set_SoundGrown(this);
    Demo_Ext_Scroll_Texture(this);
    Demo_Ext_Calc_fade(this);
    Demo_Ext_setup_FadeToDisappear(this);
}

void Demo_Ext_main(Actor* thisx, PlayState* play) {
    static DemoExtActionFunc proc[] = {
        Demo_Ext_main_wait,
        Demo_Ext_main_display,
        Demo_Ext_main_fade,
    };

    DemoExt* this = (DemoExt*)thisx;

    if ((this->action < EXT_WAIT) || (this->action > EXT_DISPELL) || proc[this->action] == NULL) {
        // "Main mode is abnormal!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        proc[this->action](this, play);
    }
}

void Demo_Ext_draw_none(Actor* thisx, PlayState* play) {
}

void Demo_Ext_draw_normal(Actor* thisx, PlayState* play) {
    DemoExt* this = (DemoExt*)thisx;
    Mtx* mtx;
    GraphicsContext* gfxCtx;
    s16* curScroll;
    Vec3f* scale;

    curScroll = this->curScroll;
    scale = &this->scale;
    gfxCtx = play->state.gfxCtx;
    mtx = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));

    OPEN_DISPS(gfxCtx, "../z_demo_ext.c", 460);
    Matrix_push();
    Matrix_scale(scale->x, scale->y, scale->z, MTXMODE_APPLY);
    Matrix_rotateXYZ((s16)(kREG(16) + 0x4000), this->rotationPitch, kREG(18), MTXMODE_APPLY);
    Matrix_translate(kREG(22), kREG(23), kREG(24), MTXMODE_APPLY);
    MATRIX_TO_MTX(mtx, "../z_demo_ext.c", 476);
    Matrix_pull();
    _texture_z_light_fog_prim_xlu(gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, kREG(33) + 128, kREG(25) + 140, kREG(26) + 80, kREG(27) + 140, this->primAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, kREG(29) + 90, kREG(30) + 50, kREG(31) + 95, this->envAlpha);
    gSPSegment(
        POLY_XLU_DISP++, 0x08,
        two_tex_scroll(gfxCtx, 0, curScroll[0], curScroll[1], 0x40, 0x40, 1, curScroll[2], curScroll[3], 0x40, 0x40));

    gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_PUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gPhantomWarpDL);
    gSPPopMatrix(POLY_XLU_DISP++, G_MTX_MODELVIEW);

    CLOSE_DISPS(gfxCtx, "../z_demo_ext.c", 512);
}

void Demo_Ext_draw(Actor* thisx, PlayState* play) {
    static DemoExtDrawFunc proc[] = {
        Demo_Ext_draw_none,
        Demo_Ext_draw_normal,
    };

    DemoExt* this = (DemoExt*)thisx;

    if ((this->drawMode < EXT_DRAW_NOTHING) || (this->drawMode > EXT_DRAW_VORTEX) ||
        proc[this->drawMode] == NULL) {
        // "Draw mode is abnormal!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        proc[this->drawMode](thisx, play);
    }
}

ActorProfile Demo_Ext_Profile = {
    /**/ ACTOR_DEMO_EXT,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_FHG,
    /**/ sizeof(DemoExt),
    /**/ Demo_Ext_Actor_ct,
    /**/ Demo_Ext_Actor_dt,
    /**/ Demo_Ext_main,
    /**/ Demo_Ext_draw,
};
