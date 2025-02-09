#include "global.h"

#include "assets/code/fbdemo_triforce/z_fbdemo_triforce.c"

void fbdemo_triforce_startup(void* thisx) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    switch (this->state) {
        case 1:
        case 2:
            this->transPos = 1.0f;
            return;

        default:
            this->transPos = 0.03f;
            return;
    }
}

void* fbdemo_triforce_init(void* thisx) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    bzero(this, sizeof(TransitionTriforce));
    guOrtho(&this->projection, -160.0f, 160.0f, -120.0f, 120.0f, -1000.0f, 1000.0f, 1.0f);
    this->transPos = 1.0f;
    this->state = 2;
    this->step = 0.015f;
    this->type = TRANS_INSTANCE_TYPE_FILL_OUT;

    return this;
}

void fbdemo_triforce_cleanup(void* thisx) {
}

void fbdemo_triforce_move(void* thisx, s32 updateRate) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;
    s32 i;

    for (i = updateRate; i > 0; i--) {
        if (this->state == 1) {
            this->transPos = CLAMP_MIN(this->transPos * (1.0f - this->step), 0.03f);
        } else if (this->state == 2) {
            this->transPos = CLAMP_MIN(this->transPos - this->step, 0.03f);
        } else if (this->state == 3) {
            this->transPos = CLAMP_MAX(this->transPos / (1.0f - this->step), 1.0f);
        } else if (this->state == 4) {
            this->transPos = CLAMP_MAX(this->transPos + this->step, 1.0f);
        }
    }
}

void fbdemo_triforce_setcolor_rgba8888(void* thisx, u32 color) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    this->color.rgba = color;
}

void fbdemo_triforce_settype(void* thisx, s32 type) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    this->type = type;
}

// unused
void fbdemo_triforce_setscaletype(void* thisx, s32 state) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    this->state = state;
}

void fbdemo_triforce_draw(void* thisx, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    Mtx* modelView;
    f32 scale;
    TransitionTriforce* this = (TransitionTriforce*)thisx;
    s32 pad;
    f32 rotation = this->transPos * 360.0f;

    modelView = this->modelView[this->frame];
    scale = this->transPos * 0.625f;
    this->frame ^= 1;
    PRINTF("rate=%f tx=%f ty=%f rotate=%f\n", this->transPos, 0.0f, 0.0f, rotation);
    guScale(&modelView[0], scale, scale, 1.0f);
    guRotate(&modelView[1], rotation, 0.0f, 0.0f, 1.0f);
    guTranslate(&modelView[2], 0.0f, 0.0f, 0.0f);
    gDPPipeSync(gfx++);
    gSPDisplayList(gfx++, fbdemo_triforce_gfx_init);
    gDPSetColor(gfx++, G_SETPRIMCOLOR, this->color.rgba);
    gDPSetCombineMode(gfx++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPMatrix(gfx++, &this->projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(gfx++, &modelView[0], G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(gfx++, &modelView[1], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gSPMatrix(gfx++, &modelView[2], G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
    gSPVertex(gfx++, triangle_vtx, 10, 0);

    if (!fbdemo_triforce_is_finish(this)) {
        switch (this->type) {
            case TRANS_INSTANCE_TYPE_FILL_OUT:
                gSP2Triangles(gfx++, 0, 4, 5, 0, 4, 1, 3, 0);
                gSP1Triangle(gfx++, 5, 3, 2, 0);
                break;

            case TRANS_INSTANCE_TYPE_FILL_IN:
                gSP2Triangles(gfx++, 3, 4, 5, 0, 0, 2, 6, 0);
                gSP2Triangles(gfx++, 0, 6, 7, 0, 1, 0, 7, 0);
                gSP2Triangles(gfx++, 1, 7, 8, 0, 1, 8, 9, 0);
                gSP2Triangles(gfx++, 1, 9, 2, 0, 2, 9, 6, 0);
                break;
        }
    } else {
        switch (this->type) {
            case TRANS_INSTANCE_TYPE_FILL_OUT:
                break;

            case TRANS_INSTANCE_TYPE_FILL_IN:
                gSP1Quadrangle(gfx++, 6, 7, 8, 9, 0);
                break;
        }
    }
    gDPPipeSync(gfx++);
    *gfxP = gfx;
}

s32 fbdemo_triforce_is_finish(void* thisx) {
    TransitionTriforce* this = (TransitionTriforce*)thisx;

    if (this->state == 1 || this->state == 2) {
        return this->transPos <= 0.03f;
    } else if (this->state == 3 || this->state == 4) {
        return this->transPos >= 1.0f;
    } else {
        return false;
    }
}
