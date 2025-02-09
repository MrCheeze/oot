#include "global.h"
#include "terminal.h"

typedef enum TransitionFadeDirection {
    /* 0 */ TRANS_FADE_DIR_IN,
    /* 1 */ TRANS_FADE_DIR_OUT
} TransitionFadeDirection;

typedef enum TransitionFadeType {
    /* 0 */ TRANS_FADE_TYPE_NONE,
    /* 1 */ TRANS_FADE_TYPE_ONE_WAY,
    /* 2 */ TRANS_FADE_TYPE_FLASH
} TransitionFadeType;

static Gfx fbdemo_fade_gfx_init[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
    gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE),
    gsSPEndDisplayList(),
};

void fbdemo_fade_startup(void* thisx) {
    TransitionFade* this = (TransitionFade*)thisx;

    switch (this->type) {
        case TRANS_FADE_TYPE_NONE:
            break;

        case TRANS_FADE_TYPE_ONE_WAY:
            this->timer = 0;
            this->color.a = (this->direction != TRANS_FADE_DIR_IN) ? 255 : 0;
            break;

        case TRANS_FADE_TYPE_FLASH:
            this->color.a = 0;
            break;
    }
    this->isDone = false;
}

void* fbdemo_fade_init(void* thisx) {
    TransitionFade* this = (TransitionFade*)thisx;

    bzero(this, sizeof(TransitionFade));
    return this;
}

void fbdemo_fade_cleanup(void* thisx) {
}

void fbdemo_fade_move(void* thisx, s32 updateRate) {
    s32 alpha;
    s16 newAlpha;
    TransitionFade* this = (TransitionFade*)thisx;

    switch (this->type) {
        case TRANS_FADE_TYPE_NONE:
            break;

        case TRANS_FADE_TYPE_ONE_WAY:
            ((TransitionFade*)thisx)->timer += updateRate;
            if (this->timer >= z_common_data.transFadeDuration) {
                this->timer = z_common_data.transFadeDuration;
                this->isDone = true;
            }
            if ((u32)z_common_data.transFadeDuration == 0) {
                PRINTF(VT_COL(RED, WHITE) T("０除算! ZCommonGet fade_speed に０がはいってる",
                                            "Divide by 0! Zero is included in ZCommonGet fade_speed") VT_RST);
            }

            alpha = (255.0f * this->timer) / ((void)0, z_common_data.transFadeDuration);
            this->color.a = (this->direction != TRANS_FADE_DIR_IN) ? 255 - alpha : alpha;
            break;

        case TRANS_FADE_TYPE_FLASH:
            newAlpha = this->color.a;
            if (R_TRANS_FADE_FLASH_ALPHA_STEP != 0) {
                if (R_TRANS_FADE_FLASH_ALPHA_STEP < 0) {
                    if (chase_s(&newAlpha, 255, 255)) {
                        R_TRANS_FADE_FLASH_ALPHA_STEP = 150;
                    }
                } else {
                    chase_s(&R_TRANS_FADE_FLASH_ALPHA_STEP, 20, 60);
                    if (chase_s(&newAlpha, 0, R_TRANS_FADE_FLASH_ALPHA_STEP)) {
                        R_TRANS_FADE_FLASH_ALPHA_STEP = 0;
                        this->isDone = true;
                    }
                }
            }
            this->color.a = newAlpha;
            break;
    }
}

void fbdemo_fade_draw(void* thisx, Gfx** gfxP) {
    TransitionFade* this = (TransitionFade*)thisx;
    Gfx* gfx;
    Color_RGBA8_u32* color = &this->color;

#if PLATFORM_N64
    if (color->a != 0)
#else
    if (color->a > 0)
#endif
    {
        gfx = *gfxP;
        gSPDisplayList(gfx++, fbdemo_fade_gfx_init);
        gDPSetPrimColor(gfx++, 0, 0, color->r, color->g, color->b, color->a);
        gDPFillRectangle(gfx++, 0, 0, ScreenWidth - 1, ScreenHeight - 1);
        gDPPipeSync(gfx++);
        *gfxP = gfx;
    }
}

s32 fbdemo_fade_is_finish(void* thisx) {
    TransitionFade* this = (TransitionFade*)thisx;

    return this->isDone;
}

void fbdemo_fade_setcolor_rgba8888(void* thisx, u32 color) {
    TransitionFade* this = (TransitionFade*)thisx;

    this->color.rgba = color;
}

void fbdemo_fade_settype(void* thisx, s32 type) {
    TransitionFade* this = (TransitionFade*)thisx;

    if (type == TRANS_INSTANCE_TYPE_FILL_OUT) {
        this->type = TRANS_FADE_TYPE_ONE_WAY;
        this->direction = TRANS_FADE_DIR_OUT;
    } else if (type == TRANS_INSTANCE_TYPE_FILL_IN) {
        this->type = TRANS_FADE_TYPE_ONE_WAY;
        this->direction = TRANS_FADE_DIR_IN;
    } else if (type == TRANS_INSTANCE_TYPE_FADE_FLASH) {
        this->type = TRANS_FADE_TYPE_FLASH;
    } else {
        this->type = TRANS_FADE_TYPE_NONE;
    }
}
