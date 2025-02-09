#include "global.h"
#include "terminal.h"
#include "assets/textures/parameter_static/parameter_static.h"
#if PLATFORM_N64
#include "n64dd.h"
#endif

typedef struct MapMarkInfo {
    /* 0x00 */ void* texture;
    /* 0x04 */ u32 imageFormat;
    /* 0x08 */ u32 imageSize;
    /* 0x0C */ u32 textureWidth;
    /* 0x10 */ u32 textureHeight;
    /* 0x14 */ u32 rectWidth;
    /* 0x18 */ u32 rectHeight;
    /* 0x1C */ u32 dsdx;
    /* 0x20 */ u32 dtdy;
} MapMarkInfo; // size = 0x24

typedef struct MapMarkDataOverlay {
    /* 0x00 */ void* loadedRamAddr; // original name: "allocp"
    /* 0x04 */ RomFile file;
    /* 0x0C */ void* vramStart;
    /* 0x10 */ void* vramEnd;
    /* 0x14 */ void* vramTable;
} MapMarkDataOverlay; // size = 0x18

#define GDP_LOADTEXTUREBLOCK_RUNTIME_QUALIFIERS
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

MapMarkInfo MarkData[] = {
    { gMapChestIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 8, 32, 32, 1 << 10, 1 << 10 }, // Chest Icon
    { gMapBossIconTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 32, 32, 1 << 10, 1 << 10 },     // Boss Skull Icon
};

static MapMarkDataOverlay my_dlftbl = {
    NULL, ROM_FILE(ovl_map_mark_data), _ovl_map_mark_dataSegmentStart, _ovl_map_mark_dataSegmentEnd, MarkPos,
};

static MapMarkData** markpos_p;

void MapMarkInit(PlayState* play) {
    MapMarkDataOverlay* overlay = &my_dlftbl;
    u32 overlaySize = (uintptr_t)overlay->vramEnd - (uintptr_t)overlay->vramStart;

    overlay->loadedRamAddr = GAME_STATE_ALLOC(&play->state, overlaySize, "../z_map_mark.c", 235);
    LOG_UTILS_CHECK_NULL_POINTER("dlftbl->allocp", overlay->loadedRamAddr, "../z_map_mark.c", 236);

    LoadFragmentFix2(overlay->file.vromStart, overlay->file.vromEnd, overlay->vramStart, overlay->vramEnd,
                 overlay->loadedRamAddr);

    markpos_p = MarkPos;
    markpos_p =
        (void*)(uintptr_t)((overlay->vramTable != NULL)
                               ? (void*)((uintptr_t)overlay->vramTable -
                                         (intptr_t)((uintptr_t)overlay->vramStart - (uintptr_t)overlay->loadedRamAddr))
                               : NULL);

#if PLATFORM_N64
    if ((B_80121220 != NULL) && (B_80121220->unk_2C != NULL)) {
        B_80121220->unk_2C(&markpos_p);
    }
#endif
}

void MapMarkCleanup(PlayState* play) {
#if PLATFORM_N64
    if ((B_80121220 != NULL) && (B_80121220->unk_30 != NULL)) {
        B_80121220->unk_30(&markpos_p);
    }
#endif

    my_dlftbl.loadedRamAddr = NULL;
    markpos_p = NULL;
}

void MapMarkDraw(PlayState* play) {
    InterfaceContext* interfaceCtx;
    MapMarkIconData* mapMarkIconData;
    MapMarkPoint* markPoint;
    MapMarkInfo* markInfo;
    u16 dungeon = z_common_data.mapIndex;
    s32 i;
    s32 rectLeft;
    s32 rectTop;

    interfaceCtx = &play->interfaceCtx;

    if ((map_exp_data_tbl_p != NULL) && (play->interfaceCtx.mapRoomNum >= map_exp_data_tbl_p->dgnMinimapCount[dungeon])) {
        PRINTF(VT_COL(RED, WHITE) T("部屋番号がオーバーしてるで,ヤバイで %d/%d  \nMapMarkDraw の処理を中断します\n",
                                    "Room number exceeded, yikes %d/%d  \nMapMarkDraw processing interrupted\n"),
               VT_RST, play->interfaceCtx.mapRoomNum, map_exp_data_tbl_p->dgnMinimapCount[dungeon]);
        return;
    }

    mapMarkIconData = &markpos_p[dungeon][interfaceCtx->mapRoomNum][0];

    OPEN_DISPS(play->state.gfxCtx, "../z_map_mark.c", 303);

    while (true) {
        if (mapMarkIconData->markType == MAP_MARK_NONE) {
            break;
        }

        gDPPipeSync(OVERLAY_DISP++);
        gDPSetTextureLUT(OVERLAY_DISP++, G_TT_NONE);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->minimapAlpha);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, interfaceCtx->minimapAlpha);

        markPoint = &mapMarkIconData->points[0];
        for (i = 0; i < mapMarkIconData->count; i++) {
            if ((mapMarkIconData->markType != MAP_MARK_CHEST) || !Actor_Environment_Tbox_Check(play, markPoint->chestFlag)) {
                markInfo = &MarkData[mapMarkIconData->markType];

                gDPPipeSync(OVERLAY_DISP++);
                gDPLoadTextureBlock_Runtime(OVERLAY_DISP++, markInfo->texture, markInfo->imageFormat,
                                            markInfo->imageSize, markInfo->textureWidth, markInfo->textureHeight, 0,
                                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                rectLeft = ((DEBUG_FEATURES ? GREG(94) : 0) + markPoint->x + 204) << 2;
                rectTop = ((DEBUG_FEATURES ? GREG(95) : 0) + markPoint->y + 140) << 2;
                gSPTextureRectangle(OVERLAY_DISP++, rectLeft, rectTop, markInfo->rectWidth + rectLeft,
                                    rectTop + markInfo->rectHeight, G_TX_RENDERTILE, 0, 0, markInfo->dsdx,
                                    markInfo->dtdy);
            }
            markPoint++;
        }
        mapMarkIconData++;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_map_mark.c", 339);
}

void MapMarkDisplay(PlayState* play) {
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
        case SCENE_DEKU_TREE_BOSS:
        case SCENE_DODONGOS_CAVERN_BOSS:
        case SCENE_JABU_JABU_BOSS:
        case SCENE_FOREST_TEMPLE_BOSS:
        case SCENE_FIRE_TEMPLE_BOSS:
            MapMarkDraw(play);
            break;
    }
}
