#if PLATFORM_N64
#include "n64dd.h"
#endif
#include "z_kaleido_scope.h"
#include "assets/textures/parameter_static/parameter_static.h"

typedef struct PauseMapMarkInfo {
    /* 0x00 */ void* texture;
    /* 0x04 */ u32 imageFormat;
    /* 0x08 */ u32 imageSize;
    /* 0x0C */ u32 textureWidth;
    /* 0x10 */ u32 textureHeight;
    /* 0x14 */ u32 rectWidth;
    /* 0x18 */ u32 rectHeight;
    /* 0x1C */ u32 dsdx;
    /* 0x20 */ u32 dtdy;
} PauseMapMarkInfo; // size = 0x24

#define GDP_LOADTEXTUREBLOCK_RUNTIME_QUALIFIERS const
#ifndef GDP_LOADTEXTUREBLOCK_RUNTIME_INC_C
#define GDP_LOADTEXTUREBLOCK_RUNTIME_INC_C

#include "gfx.h"

#ifndef GDP_LOADTEXTUREBLOCK_RUNTIME_QUALIFIERS
#define GDP_LOADTEXTUREBLOCK_RUNTIME_QUALIFIERS
#endif

// The Following arrays must be defined as const in z_lmap_mark.c to appear in rodata

static GDP_LOADTEXTUREBLOCK_RUNTIME_QUALIFIERS u32 custom_SIZ[] = {
    G_IM_SIZ_4b,
    G_IM_SIZ_8b,
    G_IM_SIZ_16b,
    G_IM_SIZ_32b,
};
static GDP_LOADTEXTUREBLOCK_RUNTIME_QUALIFIERS u32 custom_LOAD_BLOCK[] = {
    G_IM_SIZ_4b_LOAD_BLOCK,
    G_IM_SIZ_8b_LOAD_BLOCK,
    G_IM_SIZ_16b_LOAD_BLOCK,
    G_IM_SIZ_32b_LOAD_BLOCK,
};
static GDP_LOADTEXTUREBLOCK_RUNTIME_QUALIFIERS u32 custom_INCR[] = {
    G_IM_SIZ_4b_INCR,
    G_IM_SIZ_8b_INCR,
    G_IM_SIZ_16b_INCR,
    G_IM_SIZ_32b_INCR,
};
static GDP_LOADTEXTUREBLOCK_RUNTIME_QUALIFIERS u32 custom_SHIFT[] = {
    G_IM_SIZ_4b_SHIFT,
    G_IM_SIZ_8b_SHIFT,
    G_IM_SIZ_16b_SHIFT,
    G_IM_SIZ_32b_SHIFT,
};
static GDP_LOADTEXTUREBLOCK_RUNTIME_QUALIFIERS u32 custom_BYTES[] = {
    G_IM_SIZ_4b_BYTES,
    G_IM_SIZ_8b_BYTES,
    G_IM_SIZ_16b_BYTES,
    G_IM_SIZ_32b_BYTES,
};
static GDP_LOADTEXTUREBLOCK_RUNTIME_QUALIFIERS u32 custom_LINE_BYTES[] = {
    G_IM_SIZ_4b_LINE_BYTES,
    G_IM_SIZ_8b_LINE_BYTES,
    G_IM_SIZ_16b_LINE_BYTES,
    G_IM_SIZ_32b_LINE_BYTES,
};

/**
 * Implements a version of gDPLoadTextureBlock using table lookups instead of token pasting, to allow values to be
 * passed into `siz` during runtime.
 */
#define gDPLoadTextureBlock_Runtime(pkt, timg, fmt, siz, width, height, pal, cms, cmt, masks, maskt, shifts, shiftt)   \
    _DW({                                                                                                              \
        gDPSetTextureImage(pkt, fmt, custom_LOAD_BLOCK[siz], 1, timg);                                  \
        gDPSetTile(pkt, fmt, custom_LOAD_BLOCK[siz], 0, 0, G_TX_LOADTILE, 0, cmt, maskt, shiftt, cms,   \
                   masks, shifts);                                                                                     \
        gDPLoadSync(pkt);                                                                                              \
        gDPLoadBlock(pkt, G_TX_LOADTILE, 0, 0,                                                                         \
                     (((width) * (height) + custom_INCR[siz]) >> custom_SHIFT[siz]) - 1, \
                     CALC_DXT(width, custom_BYTES[siz]));                                               \
        gDPPipeSync(pkt);                                                                                              \
        gDPSetTile(pkt, fmt, custom_SIZ[siz], (((width)*custom_LINE_BYTES[siz]) + 7) >> 3,   \
                   0, G_TX_RENDERTILE, pal, cmt, maskt, shiftt, cms, masks, shifts);                                   \
        gDPSetTileSize(pkt, G_TX_RENDERTILE, 0, 0, ((width)-1) << G_TEXTURE_IMAGE_FRAC,                                \
                       ((height)-1) << G_TEXTURE_IMAGE_FRAC);                                                          \
    })
#endif

static PauseMapMarkInfo MarkData[] = {
    { gMapChestIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 8, 32, 32, 1 << 10, 1 << 10 },
    { gMapBossIconTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 32, 32, 1 << 10, 1 << 10 },
};

extern PauseMapMarksData LargeMarkPos[];

void LargeMapMarkInit(PlayState* play) {
    LargeMapMark_MarkScaleMode = 0;
    LargeMapMark_MarkScale = 1.0f;
    LargeMapMark_lmarkpos_p = LargeMarkPos;
#if PLATFORM_N64
    if ((B_80121220 != NULL) && (B_80121220->unk_34 != NULL)) {
        B_80121220->unk_34(&LargeMapMark_lmarkpos_p);
    }
#endif
}

void LargeMapMarkCleanup(PlayState* play) {
#if PLATFORM_N64
    if ((B_80121220 != NULL) && (B_80121220->unk_38 != NULL)) {
        B_80121220->unk_38(&LargeMapMark_lmarkpos_p);
    }
#endif
    LargeMapMark_lmarkpos_p = NULL;
}

void LargeMapMarkDraw(PlayState* play) {
    PauseMapMarkData* mapMarkData;
    PauseMapMarkPoint* markPoint;
    PauseMapMarkInfo* markInfo;
    f32 scale;
    s32 i = 0;

    mapMarkData = &LargeMapMark_lmarkpos_p[R_MAP_TEX_INDEX >> 1][i];

    OPEN_DISPS(play->state.gfxCtx, "../z_lmap_mark.c", 182);

    while (true) {
        if (mapMarkData->markType == PAUSE_MAP_MARK_NONE) {
            break;
        }

        if ((mapMarkData->markType == PAUSE_MAP_MARK_BOSS) && (play->sceneId >= SCENE_DEKU_TREE_BOSS) &&
            (play->sceneId <= SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR)) {
            if (LargeMapMark_MarkScaleMode == 0) {
                add_calc2(&LargeMapMark_MarkScale, 1.5f, 1.0f, 0.041f);
                if (LargeMapMark_MarkScale == 1.5f) {
                    LargeMapMark_MarkScaleMode = 1;
                }
            } else {
                add_calc2(&LargeMapMark_MarkScale, 1.0f, 1.0f, 0.041f);
                if (LargeMapMark_MarkScale == 1.0f) {
                    LargeMapMark_MarkScaleMode = 0;
                }
            }
            scale = LargeMapMark_MarkScale;
        } else {
            scale = 1.0f;
        }

        Matrix_push();

        if ((play->pauseCtx.state == PAUSE_STATE_OPENING_1) || (play->pauseCtx.state >= PAUSE_STATE_CLOSING)) {
            Matrix_translate(-36.0f, 101.0f, 0.0f, MTXMODE_APPLY);
        } else {
            Matrix_translate(-36.0f, 21.0f, 0.0f, MTXMODE_APPLY);
        }

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

        markPoint = &mapMarkData->points[0];
        for (i = 0; i < mapMarkData->count; i++) {
            s32 display;

            if (mapMarkData->markType == PAUSE_MAP_MARK_CHEST) {
                if (Actor_Environment_Tbox_Check(play, markPoint->chestFlag)) {
                    display = false;
                } else {
                    switch (play->sceneId) {
                        case SCENE_DEKU_TREE_BOSS:
                        case SCENE_DODONGOS_CAVERN_BOSS:
                        case SCENE_JABU_JABU_BOSS:
                        case SCENE_FOREST_TEMPLE_BOSS:
                        case SCENE_FIRE_TEMPLE_BOSS:
                        case SCENE_WATER_TEMPLE_BOSS:
                        case SCENE_SPIRIT_TEMPLE_BOSS:
                        case SCENE_SHADOW_TEMPLE_BOSS:
                            display = false;
                            break;
                        default:
                            display = true;
                            break;
                    }
                }
            } else {
                display = true;
            }

            if (display) {
                markInfo = &MarkData[mapMarkData->markType];

                gDPPipeSync(POLY_OPA_DISP++);
                gDPLoadTextureBlock_Runtime(POLY_OPA_DISP++, markInfo->texture, markInfo->imageFormat,
                                            markInfo->imageSize, markInfo->textureWidth, markInfo->textureHeight, 0,
                                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                Matrix_push();

#if DEBUG_FEATURES
                Matrix_translate(markPoint->x + GREG(92), markPoint->y + GREG(93), 0.0f, MTXMODE_APPLY);
#else
                Matrix_translate(markPoint->x, markPoint->y, 0.0f, MTXMODE_APPLY);
#endif

                Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_lmap_mark.c", 272);
                Matrix_pull();

                gSPVertex(POLY_OPA_DISP++, mapMarkData->vtx, mapMarkData->vtxCount, 0);
                gSP1Quadrangle(POLY_OPA_DISP++, 1, 3, 2, 0, 0);
            }

            markPoint++;
        }

        mapMarkData++;
        Matrix_pull();
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_lmap_mark.c", 286);
}

void LargeMapMarkDisplay(PlayState* play) {
    LargeMapMarkInit(play);

    switch (play->sceneId) {
        case SCENE_DEKU_TREE:
        case SCENE_DODONGOS_CAVERN:
        case SCENE_JABU_JABU:
        case SCENE_FOREST_TEMPLE:
        case SCENE_FIRE_TEMPLE:
        case SCENE_WATER_TEMPLE:
        case SCENE_SPIRIT_TEMPLE:
        case SCENE_SHADOW_TEMPLE:
        case SCENE_BOTTOM_OF_THE_WELL:
        case SCENE_ICE_CAVERN:
            LargeMapMarkDraw(play);
            break;
    }

    LargeMapMarkCleanup(play);
}
