#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ntsc-1.2:128 pal-1.0:128 pal-1.1:128 hiratsu3:128"

#include "global.h"
#include "terminal.h"

/**
 * How much time the audio update on the audio thread (`Nas_AudioMain`) took in total, between scheduling the last
 * two graphics tasks.
 */
volatile OSTime audio_cpu_time;

/**
 * How much time elapsed between scheduling the previous graphics task and the current one being ready (the previous
 * task not necessarily being finished yet), without the amount of time spent on the audio update in the audio thread.
 */
volatile OSTime graph_cpu_time;

/**
 * How much time the RSP ran audio tasks for over the course of `frame_time`.
 */
volatile OSTime audio_rsp_time;

/**
 * How much time the RSP ran graphics tasks for over the course of `frame_time`.
 * Typically the RSP runs 1 graphics task per `graph_main` cycle, but may run 0 (see `graph_main`).
 */
volatile OSTime graph_rsp_time;

/**
 * How much time the RDP ran for over the course of `frame_time`.
 */
volatile OSTime rdp_time;

/**
 * How much time elapsed between the last two `graph_main` ending.
 * This is expected to be at least the duration of a single frame, since it includes the time spent waiting on the
 * graphics task to be done.
 */
volatile OSTime frame_time;

/**
 * The time at which the audio thread audio update started.
 */
volatile OSTime audio_cpu_start_time;

// Accumulator for `audio_cpu_start_time`
volatile OSTime audio_cpu_sum_time;

// Accumulator for `audio_rsp_time`
volatile OSTime audio_rsp_sum_time;

// Accumulator for `graph_rsp_time`.
volatile OSTime graph_rsp_sum_time;

volatile OSTime other_rsp_sum_time;
volatile OSTime other_rsp_time;

// Accumulator for `rdp_time`
volatile OSTime rdp_sum_time;

typedef struct SpeedMeterTimeEntry {
    /* 0x00 */ volatile OSTime* time;
    /* 0x04 */ u8 x;
    /* 0x05 */ u8 y;
    /* 0x06 */ u16 color;
} SpeedMeterTimeEntry; // size = 0x08

SpeedMeterTimeEntry* vvvp;

SpeedMeterTimeEntry vvv[] = {
    { &audio_cpu_time, 0, 0, GPACK_RGBA5551(255, 0, 0, 1) },
    { &graph_cpu_time, 0, 2, GPACK_RGBA5551(255, 255, 0, 1) },
    { &audio_rsp_time, 0, 4, GPACK_RGBA5551(0, 0, 255, 1) },
    { &graph_rsp_time, 0, 6, GPACK_RGBA5551(255, 128, 128, 1) },
    { &rdp_time, 0, 8, GPACK_RGBA5551(0, 255, 0, 1) },
    { &frame_time, 0, 10, GPACK_RGBA5551(255, 0, 255, 1) },
};

typedef struct SpeedMeterAllocEntry {
    /* 0x00 */ s32 maxval;
    /* 0x04 */ s32 val;
    /* 0x08 */ u16 backColor;
    /* 0x0A */ u16 foreColor;
    /* 0x0C */ s32 ulx;
    /* 0x10 */ s32 lrx;
    /* 0x14 */ s32 uly;
    /* 0x18 */ s32 lry;
} SpeedMeterAllocEntry; // size = 0x1C

#define gDrawRect(gfx, color, ulx, uly, lrx, lry)      \
    gDPPipeSync(gfx);                                  \
    gDPSetFillColor(gfx, ((color) << 16) | (color));   \
    gDPFillRectangle(gfx, (ulx), (uly), (lrx), (lry)); \
    gDPPipeSync(gfx)

void speed_meter_setpos(SpeedMeter* this, u32 x, u32 y) {
    LOG_UTILS_CHECK_NULL_POINTER("this", this, "../speed_meter.c", 181);
    this->x = x;
    this->y = y;
}

void speed_meter_init(SpeedMeter* this) {
    speed_meter_setpos(this, 32, 22);
}

void speed_meter_cleanup(SpeedMeter* this) {
}

void speed_meter_draw(SpeedMeter* this, GraphicsContext* gfxCtx) {
    s32 pad[2];
    u32 baseX = 32;
    s32 width;
    s32 i;
    s32 uly;
    s32 lry;
    View view;
    u32 pad2[3];
    Gfx* gfx;

    uly = this->y;
    lry = this->y + 2;

    OPEN_DISPS(gfxCtx, "../speed_meter.c", 225);

    /*! @bug if RetraceTime is 0, CLOSE_DISPS will never be reached */
    if (RetraceTime == 0) {
        return;
    }

    vvvp = &vvv[0];
    for (i = 0; i < ARRAY_COUNT(vvv); i++) {
        width = ((f64)*vvvp->time / RetraceTime) * 64.0;
        vvvp->x = baseX + width;
        vvvp++;
    }

    initView(&view, gfxCtx);
    view.flags = VIEW_VIEWPORT | VIEW_PROJECTION_ORTHO;

    SET_FULLSCREEN_VIEWPORT(&view);

    gfx = OVERLAY_DISP;
    showView1(&view, VIEW_ALL, &gfx);

    gDPPipeSync(gfx++);
    gDPSetOtherMode(gfx++,
                    G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_NONE | G_CYC_FILL | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);

    gDrawRect(gfx++, GPACK_RGBA5551(0, 0, 255, 1), baseX + 64 * 0, uly, baseX + 64 * 1, lry);
    gDrawRect(gfx++, GPACK_RGBA5551(0, 255, 0, 1), baseX + 64 * 1, uly, baseX + 64 * 2, lry);
    gDrawRect(gfx++, GPACK_RGBA5551(255, 0, 0, 1), baseX + 64 * 2, uly, baseX + 64 * 3, lry);
    gDrawRect(gfx++, GPACK_RGBA5551(255, 0, 255, 1), baseX + 64 * 3, uly, baseX + 64 * 4, lry);

    vvvp = &vvv[0];
    for (i = 0; i < ARRAY_COUNT(vvv); i++) {
        gDrawRect(gfx++, vvvp->color, baseX, lry + vvvp->y,
                  vvvp->x, lry + vvvp->y + 1);
        vvvp++;
    }
    gDPPipeSync(gfx++);

    OVERLAY_DISP = gfx;

    CLOSE_DISPS(gfxCtx, "../speed_meter.c", 276);
}

void drawbox_init(SpeedMeterAllocEntry* this, u32 maxval, u32 val, u16 backColor, u16 foreColor, u32 ulx,
                               u32 lrx, u32 uly, u32 lry) {
    this->maxval = maxval;
    this->val = val;
    this->backColor = backColor;
    this->foreColor = foreColor;
    this->ulx = ulx;
    this->lrx = lrx;
    this->uly = uly;
    this->lry = lry;
}

void drawbox_draw(SpeedMeterAllocEntry* this, GraphicsContext* gfxCtx) {
    s32 usedOff;
    View view;
    Gfx* gfx;

    if (this->maxval == 0) {
        PRINTF_COLOR_RED();
        LOG_NUM("this->maxval", this->maxval, "../speed_meter.c", 313);
        PRINTF_RST();
    } else {
        OPEN_DISPS(gfxCtx, "../speed_meter.c", 318);

        initView(&view, gfxCtx);
        view.flags = VIEW_VIEWPORT | VIEW_PROJECTION_ORTHO;

        SET_FULLSCREEN_VIEWPORT(&view);

        gfx = OVERLAY_DISP;
        showView1(&view, VIEW_ALL, &gfx);

        gDPPipeSync(gfx++);
        gDPSetOtherMode(gfx++,
                        G_AD_PATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_CONV | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                            G_TD_CLAMP | G_TP_NONE | G_CYC_FILL | G_PM_NPRIMITIVE,
                        G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);

        usedOff = ((this->lrx - this->ulx) * this->val) / this->maxval + this->ulx;
        gDrawRect(gfx++, this->backColor, usedOff, this->uly, this->lrx, this->lry);
        gDrawRect(gfx++, this->foreColor, this->ulx, this->uly, usedOff, this->lry);

        gDPPipeSync(gfx++);

        OVERLAY_DISP = gfx;
        CLOSE_DISPS(gfxCtx, "../speed_meter.c", 339);
    }
}

void speed_meter_draw_memory(SpeedMeter* meter, GraphicsContext* gfxCtx, GameState* state) {
    s32 pad1[2];
    u32 ulx = 30;
    u32 lrx = 290;
    SpeedMeterAllocEntry entry;
    TwoHeadArena* tha;
    s32 y;
    TwoHeadGfxArena* thga;
    u32 zeldaFreeMax;
    u32 zeldaFree;
    u32 zeldaAlloc;
    s32 sysFreeMax;
    s32 sysFree;
    s32 sysAlloc;

    y = 212;
    if (R_ENABLE_ARENA_DBG > 2) {
        if (zelda_MallocIsInitalized()) {
            zelda_GetFreeArena(&zeldaFreeMax, &zeldaFree, &zeldaAlloc);
            drawbox_init(&entry, zeldaFree + zeldaAlloc, zeldaAlloc, GPACK_RGBA5551(0, 0, 255, 1),
                                      GPACK_RGBA5551(255, 255, 255, 1), ulx, lrx, y, y + 1);
            drawbox_draw(&entry, gfxCtx);
            y++;
            y++;
        }
    }

    if (R_ENABLE_ARENA_DBG > 1) {
        GetFreeArena((u32*)&sysFreeMax, (u32*)&sysFree, (u32*)&sysAlloc);
        drawbox_init(&entry, sysFree + sysAlloc - state->tha.size, sysAlloc - state->tha.size,
                                  GPACK_RGBA5551(0, 0, 255, 1), GPACK_RGBA5551(255, 128, 128, 1), ulx, lrx, y, y);
        drawbox_draw(&entry, gfxCtx);
        y++;
    }

    tha = &state->tha;
    drawbox_init(&entry, tha->size, tha->size - THA_getFreeBytes(tha), GPACK_RGBA5551(0, 0, 255, 1),
                              GPACK_RGBA5551(0, 255, 0, 1), ulx, lrx, y, y);
    drawbox_draw(&entry, gfxCtx);
    y++;

    thga = &gfxCtx->polyOpa;
    drawbox_init(&entry, thga->size, thga->size - THA_GA_getFreeBytes(thga), GPACK_RGBA5551(0, 0, 255, 1),
                              GPACK_RGBA5551(255, 0, 255, 1), ulx, lrx, y, y);
    drawbox_draw(&entry, gfxCtx);
    y++;

    thga = &gfxCtx->polyXlu;
    drawbox_init(&entry, thga->size, thga->size - THA_GA_getFreeBytes(thga), GPACK_RGBA5551(0, 0, 255, 1),
                              GPACK_RGBA5551(255, 255, 0, 1), ulx, lrx, y, y);
    drawbox_draw(&entry, gfxCtx);
    y++;

    thga = &gfxCtx->overlay;
    drawbox_init(&entry, thga->size, thga->size - THA_GA_getFreeBytes(thga), GPACK_RGBA5551(0, 0, 255, 1),
                              GPACK_RGBA5551(255, 0, 0, 1), ulx, lrx, y, y);
    drawbox_draw(&entry, gfxCtx);
    y++;
}
