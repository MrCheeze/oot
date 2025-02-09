#include "global.h"

void sample_move(SampleState* this) {
    if (CHECK_BTN_ALL(this->state.input[0].press.button, BTN_START)) {
        SET_NEXT_GAMESTATE(&this->state, play_init, PlayState);
        this->state.running = false;
    }
}

void sample_draw(SampleState* this) {
    GraphicsContext* gfxCtx = this->state.gfxCtx;
    View* view = &this->view;

    OPEN_DISPS(gfxCtx, "../z_sample.c", 62);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    gSPSegment(POLY_OPA_DISP++, 0x01, this->staticSegment);

    DisplayList_initialize(gfxCtx, 0, 0, 0);

    view->flags = VIEW_VIEWING | VIEW_VIEWPORT | VIEW_PROJECTION_PERSPECTIVE;
    showView(view, VIEW_ALL);

    {
        Mtx* mtx = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));

        guPosition(mtx, SREG(37), SREG(38), SREG(39), 1.0f, SREG(40), SREG(41), SREG(42));
        gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    }

    POLY_OPA_DISP = set_fog(POLY_OPA_DISP, 255, 255, 255, 0, 0, 0);
    _texture_z_light_fog_prim(gfxCtx);

    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_1CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 0);

    CLOSE_DISPS(gfxCtx, "../z_sample.c", 111);
}

void sample_main(GameState* thisx) {
    SampleState* this = (SampleState*)thisx;

    sample_draw(this);
    sample_move(this);
}

void sample_cleanup(GameState* thisx) {
}

void sample_view_init(SampleState* this) {
    View* view = &this->view;
    GraphicsContext* gfxCtx = this->state.gfxCtx;

    initView(view, gfxCtx);
    SET_FULLSCREEN_VIEWPORT(view);
    setPerspectiveView(view, 60.0f, 10.0f, 12800.0f);

    {
        Vec3f eye;
        Vec3f lookAt;
        Vec3f up;

        eye.x = 0.0f;
        eye.y = 0.0f;
        eye.z = 3000.0f;
        lookAt.x = 0.0f;
        lookAt.y = 0.0f;
        lookAt.z = 0.0f;
        up.x = 0.0f;
        up.z = 0.0f;
        up.y = 1.0f;

        setLookAtView(view, &eye, &lookAt, &up);
    }
}

void sample_data_init(SampleState* this) {
    u32 size = _title_staticSegmentRomEnd - _title_staticSegmentRomStart;

    this->staticSegment = GAME_STATE_ALLOC(&this->state, size, "../z_sample.c", 163);
    DMA_REQUEST_SYNC(this->staticSegment, (uintptr_t)_title_staticSegmentRomStart, size, "../z_sample.c", 164);
}

void sample_init(GameState* thisx) {
    SampleState* this = (SampleState*)thisx;

    this->state.main = sample_main;
    this->state.destroy = sample_cleanup;
    R_UPDATE_RATE = 1;
    sample_view_init(this);
    sample_data_init(this);
    SREG(37) = 0;
    SREG(38) = 0;
    SREG(39) = 0;
    SREG(40) = 0;
    SREG(41) = 0;
    SREG(42) = 0;
}
