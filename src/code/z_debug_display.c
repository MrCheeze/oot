#include "gfx.h"
#include "gfx_setupdl.h"
#include "sys_matrix.h"
#include "z64play.h"
#include "z64debug_display.h"

#include "assets/objects/gameplay_keep/gameplay_keep.h"

typedef struct DebugDispObjectInfo {
    /* 0x00 */ s16 drawType;  // indicates which draw function to use when displaying the object
    /* 0x04 */ void* drawArg; // segment address (display list or texture) passed to the draw function when called
} DebugDispObjectInfo;        // size = 0x8

typedef void (*DebugDispObject_DrawFunc)(DebugDispObject*, void*, PlayState*);

void debug_display_output_sprite_16x16_I8(DebugDispObject* dispObj, void* texture, PlayState* play);
void debug_display_output_polygon(DebugDispObject* dispObj, void* dlist, PlayState* play);

static DebugDispObject_DrawFunc debug_display_output_proc[] = {
    debug_display_output_sprite_16x16_I8,
    debug_display_output_polygon,
};

static DebugDispObjectInfo debug_display_shape_data[] = {
    { 0, gDebugCircleTex }, { 0, gDebugCrossTex }, { 0, gDebugBallTex },
    { 0, gDebugCursorTex }, { 1, gDebugArrowDL },  { 1, gDebugCameraDL },
};

static Lights1 material = gdSPDefLights1(128, 128, 128, 255, 255, 255, 73, 73, 73);

static DebugDispObject* debug_display;

void Debug_Display_init(void) {
    debug_display = NULL;
}

DebugDispObject* Debug_Display_new(f32 posX, f32 posY, f32 posZ, s16 rotX, s16 rotY, s16 rotZ, f32 scaleX,
                                        f32 scaleY, f32 scaleZ, u8 red, u8 green, u8 blue, u8 alpha, s16 type,
                                        GraphicsContext* gfxCtx) {
    DebugDispObject* prevHead = debug_display;

    debug_display = GRAPH_ALLOC(gfxCtx, sizeof(DebugDispObject));

    debug_display->pos.x = posX;
    debug_display->pos.y = posY;
    debug_display->pos.z = posZ;
    debug_display->rot.x = rotX;
    debug_display->rot.y = rotY;
    debug_display->rot.z = rotZ;
    debug_display->scale.x = scaleX;
    debug_display->scale.y = scaleY;
    debug_display->scale.z = scaleZ;
    debug_display->color.r = red;
    debug_display->color.g = green;
    debug_display->color.b = blue;
    debug_display->color.a = alpha;
    debug_display->type = type;
    debug_display->next = prevHead;

    return debug_display;
}

void Debug_Display_output(PlayState* play) {
    DebugDispObject* dispObj = debug_display;
    DebugDispObjectInfo* objInfo;

    while (dispObj != NULL) {
        objInfo = &debug_display_shape_data[dispObj->type];
        debug_display_output_proc[objInfo->drawType](dispObj, objInfo->drawArg, play);
        dispObj = dispObj->next;
    }
}

void debug_display_output_sprite_16x16_I8(DebugDispObject* dispObj, void* texture, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_debug_display.c", 169);

    softsprite_prim(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, dispObj->color.r, dispObj->color.g, dispObj->color.b, dispObj->color.a);

    Matrix_translate(dispObj->pos.x, dispObj->pos.y, dispObj->pos.z, MTXMODE_NEW);
    Matrix_scale(dispObj->scale.x, dispObj->scale.y, dispObj->scale.z, MTXMODE_APPLY);
    Matrix_mult(&play->billboardMtxF, MTXMODE_APPLY);
    Matrix_rotateXYZ(dispObj->rot.x, dispObj->rot.y, dispObj->rot.z, MTXMODE_APPLY);

    gDPLoadTextureBlock(POLY_XLU_DISP++, texture, G_IM_FMT_I, G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_debug_display.c", 189);
    gSPDisplayList(POLY_XLU_DISP++, gDebugSpriteDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_debug_display.c", 192);
}

void debug_display_output_polygon(DebugDispObject* dispObj, void* dlist, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_debug_display.c", 211);

    polygon_z_light_prim(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, dispObj->color.r, dispObj->color.g, dispObj->color.b, dispObj->color.a);

    gSPSetLights1(POLY_XLU_DISP++, material);

    Matrix_softcv3_load(dispObj->pos.x, dispObj->pos.y, dispObj->pos.z, &dispObj->rot);
    Matrix_scale(dispObj->scale.x, dispObj->scale.y, dispObj->scale.z, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_debug_display.c", 228);
    gSPDisplayList(POLY_XLU_DISP++, dlist);

    CLOSE_DISPS(play->state.gfxCtx, "../z_debug_display.c", 231);
}
