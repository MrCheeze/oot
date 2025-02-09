#include "z_kaleido_scope.h"
#include "versions.h"
#include "assets/textures/icon_item_24_static/icon_item_24_static.h"
#if OOT_NTSC
#include "assets/textures/icon_item_jpn_static/icon_item_jpn_static.h"
#include "assets/textures/icon_item_nes_static/icon_item_nes_static.h"
#else
#include "assets/textures/icon_item_nes_static/icon_item_nes_static.h"
#include "assets/textures/icon_item_ger_static/icon_item_ger_static.h"
#include "assets/textures/icon_item_fra_static/icon_item_fra_static.h"
#endif
#include "assets/textures/icon_item_field_static/icon_item_field_static.h"
#include "assets/textures/icon_item_dungeon_static/icon_item_dungeon_static.h"
#include "assets/textures/icon_item_nes_static/icon_item_nes_static.h"

void dungeon_map_disp(PlayState* play, GraphicsContext* gfxCtx) {
    static void* dungeon_keep_data[] = {
        gQuestIconDungeonBossKeyTex,
        gQuestIconDungeonCompassTex,
        gQuestIconDungeonMapTex,
    };
    static void* dungeon_name_data[] = {
        gPauseDekuTitleENGTex, gPauseDodongoTitleENGTex,   gPauseJabuTitleENGTex,   gPauseForestTitleENGTex,
        gPauseFireTitleENGTex, gPauseWaterTitleENGTex,     gPauseSpiritTitleENGTex, gPauseShadowTitleENGTex,
        gPauseBotWTitleENGTex, gPauseIceCavernTitleENGTex,
    };
    static void* floor_data[] = {
        gDungeonMapBlankFloorButtonTex, gDungeonMap8FButtonTex, gDungeonMap7FButtonTex, gDungeonMap6FButtonTex,
        gDungeonMap5FButtonTex,         gDungeonMap4FButtonTex, gDungeonMap3FButtonTex, gDungeonMap2FButtonTex,
        gDungeonMap1FButtonTex,         gDungeonMapB1ButtonTex, gDungeonMapB2ButtonTex, gDungeonMapB3ButtonTex,
        gDungeonMapB4ButtonTex,         gDungeonMapB5ButtonTex, gDungeonMapB6ButtonTex, gDungeonMapB7ButtonTex,
        gDungeonMapB8ButtonTex,
    };
    static u16 map_color[][3] = {
        { 0 / 8, 80 / 8, 255 / 8 },
        { 0 / 8, 200 / 8, 140 / 8 },
    };
    static s16 rc = 0 / 8;
    static s16 gc = 200 / 8;
    static s16 bc = 140 / 8;
    static u16 cnt = 20;
    static u16 pt = 0;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 i;
    s16 j;
    s16 oldCursorPoint;
    s16 stepR;
    s16 stepG;
    s16 stepB;
    u16 rgba16;

    OPEN_DISPS(gfxCtx, "../z_kaleido_map_PAL.c", 123);

    if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
        (pauseCtx->pageIndex == PAUSE_MAP)) {
        pauseCtx->cursorColorSet = 0;
        oldCursorPoint = pauseCtx->cursorPoint[PAUSE_MAP];

        if (pauseCtx->cursorSpecialPos == 0) {
            if (pauseCtx->stickAdjX > 30) {
                if (pauseCtx->cursorX[PAUSE_MAP] != 0) {
                    key_arrangement(play, PAUSE_CURSOR_PAGE_RIGHT);
                } else {
                    pauseCtx->cursorX[PAUSE_MAP] = 1;
                    pauseCtx->cursorPoint[PAUSE_MAP] = 0;
                    if (!CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, z_common_data.mapIndex)) {
                        pauseCtx->cursorPoint[PAUSE_MAP]++;
                        if (!CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, z_common_data.mapIndex)) {
                            pauseCtx->cursorPoint[PAUSE_MAP]++;
                            if (!CHECK_DUNGEON_ITEM(DUNGEON_MAP, z_common_data.mapIndex)) {
                                key_arrangement(play, PAUSE_CURSOR_PAGE_RIGHT);
                            }
                        }
                    }
                }
            } else if (pauseCtx->stickAdjX < -30) {
                if (pauseCtx->cursorX[PAUSE_MAP] == 0) {
                    key_arrangement(play, PAUSE_CURSOR_PAGE_LEFT);
                } else {
                    pauseCtx->cursorX[PAUSE_MAP] = 0;
                    pauseCtx->cursorPoint[PAUSE_MAP] = pauseCtx->dungeonMapSlot;
                    PRINTF("kscope->cursor_point=%d\n", pauseCtx->cursorPoint[PAUSE_MAP]);
                    R_MAP_TEX_INDEX =
                        R_MAP_TEX_INDEX_BASE +
                        map_exp_data_tbl_p->floorTexIndexOffset[z_common_data.mapIndex][pauseCtx->cursorPoint[PAUSE_MAP] - 3];
                    map_dma(play);
                }
            }

            if (pauseCtx->cursorPoint[PAUSE_MAP] < 3) {
                if (pauseCtx->stickAdjY > 30) {
                    if (pauseCtx->cursorPoint[PAUSE_MAP] != 0) {
                        for (i = pauseCtx->cursorPoint[PAUSE_MAP] - 1; i >= 0; i--) {
                            if (CHECK_DUNGEON_ITEM(i, z_common_data.mapIndex)) {
                                pauseCtx->cursorPoint[PAUSE_MAP] = i;
                                break;
                            }
                        }
                    }
                } else {
                    if (pauseCtx->stickAdjY < -30) {
                        if (pauseCtx->cursorPoint[PAUSE_MAP] != 2) {
                            for (i = pauseCtx->cursorPoint[PAUSE_MAP] + 1; i < 3; i++) {
                                if (CHECK_DUNGEON_ITEM(i, z_common_data.mapIndex)) {
                                    pauseCtx->cursorPoint[PAUSE_MAP] = i;
                                    break;
                                }
                            }
                        }
                    }
                }
            } else {
                if (pauseCtx->stickAdjY > 30) {
                    if (pauseCtx->cursorPoint[PAUSE_MAP] >= 4) {
                        for (i = pauseCtx->cursorPoint[PAUSE_MAP] - 3 - 1; i >= 0; i--) {
                            if ((z_common_data.save.info.sceneFlags[z_common_data.mapIndex].floors & check_bit[i]) ||
                                (CHECK_DUNGEON_ITEM(DUNGEON_MAP, z_common_data.mapIndex) &&
                                 (map_exp_data_tbl_p->floorID[interfaceCtx->unk_25A][i] != 0))) {
                                pauseCtx->cursorPoint[PAUSE_MAP] = i + 3;
                                break;
                            }
                        }
                    }
                } else if (pauseCtx->stickAdjY < -30) {
                    if (pauseCtx->cursorPoint[PAUSE_MAP] != 10) {
                        for (i = pauseCtx->cursorPoint[PAUSE_MAP] - 3 + 1; i < 11; i++) {
                            if ((z_common_data.save.info.sceneFlags[z_common_data.mapIndex].floors & check_bit[i]) ||
                                (CHECK_DUNGEON_ITEM(DUNGEON_MAP, z_common_data.mapIndex) &&
                                 (map_exp_data_tbl_p->floorID[interfaceCtx->unk_25A][i] != 0))) {
                                pauseCtx->cursorPoint[PAUSE_MAP] = i + 3;
                                break;
                            }
                        }
                    }
                }

                i = R_MAP_TEX_INDEX;
                R_MAP_TEX_INDEX =
                    R_MAP_TEX_INDEX_BASE +
                    map_exp_data_tbl_p->floorTexIndexOffset[z_common_data.mapIndex][pauseCtx->cursorPoint[PAUSE_MAP] - 3];
                pauseCtx->dungeonMapSlot = pauseCtx->cursorPoint[PAUSE_MAP];
                if (i != R_MAP_TEX_INDEX) {
                    map_dma(play);
                }
            }
        } else if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
            if (pauseCtx->stickAdjX > 30) {
                pauseCtx->nameDisplayTimer = 0;
                pauseCtx->cursorSpecialPos = 0;
                pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP] = pauseCtx->dungeonMapSlot;
                pauseCtx->cursorX[PAUSE_MAP] = 0;
                j = (pauseCtx->cursorSlot[PAUSE_MAP] + 18) * 4;
                cursol_defalute(pauseCtx, j, pauseCtx->mapPageVtx);
                Nai_FxFlagEntry(NA_SE_SY_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
        } else {
            if (pauseCtx->stickAdjX < -30) {
                pauseCtx->nameDisplayTimer = 0;
                pauseCtx->cursorSpecialPos = 0;
                pauseCtx->cursorX[PAUSE_MAP] = 1;
                pauseCtx->cursorPoint[PAUSE_MAP] = 0;
                if (!CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, z_common_data.mapIndex)) {
                    pauseCtx->cursorPoint[PAUSE_MAP]++;
                    if (!CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, z_common_data.mapIndex)) {
                        pauseCtx->cursorPoint[PAUSE_MAP]++;
                        if (!CHECK_DUNGEON_ITEM(DUNGEON_MAP, z_common_data.mapIndex)) {
                            pauseCtx->cursorX[PAUSE_MAP] = 0;
                            pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP] =
                                pauseCtx->dungeonMapSlot;
                            R_MAP_TEX_INDEX =
                                R_MAP_TEX_INDEX_BASE +
                                map_exp_data_tbl_p
                                    ->floorTexIndexOffset[z_common_data.mapIndex][pauseCtx->cursorPoint[PAUSE_MAP] - 3];
                            map_dma(play);
                        }
                    }
                } else {
                    pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP];
                }

                PRINTF("kscope->cursor_point====%d\n", pauseCtx->cursorPoint[PAUSE_MAP]);
                j = (pauseCtx->cursorSlot[PAUSE_MAP] + 18) * 4;
                cursol_defalute(pauseCtx, j, pauseCtx->mapPageVtx);
                Nai_FxFlagEntry(NA_SE_SY_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
        }

        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_MAP]) {
            Nai_FxFlagEntry(NA_SE_SY_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }

    if (pauseCtx->cursorSpecialPos == 0) {
        if (pauseCtx->cursorPoint[PAUSE_MAP] < 3) {
            pauseCtx->cursorItem[PAUSE_MAP] = ITEM_DUNGEON_BOSS_KEY + pauseCtx->cursorPoint[PAUSE_MAP];
        } else {
            pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
        }

        pauseCtx->cursorSlot[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_MAP];

        j = (pauseCtx->cursorSlot[PAUSE_MAP] + 18) * 4;
        cursol_defalute(pauseCtx, j, pauseCtx->mapPageVtx);

        if (pauseCtx->cursorX[PAUSE_MAP] == 0) {
            pauseCtx->mapPageVtx[j + 0].v.ob[0] = pauseCtx->mapPageVtx[j + 2].v.ob[0] =
                pauseCtx->mapPageVtx[j + 0].v.ob[0] - 2;
            pauseCtx->mapPageVtx[j + 1].v.ob[0] = pauseCtx->mapPageVtx[j + 3].v.ob[0] =
                pauseCtx->mapPageVtx[j + 1].v.ob[0] + 4;
            pauseCtx->mapPageVtx[j + 0].v.ob[1] = pauseCtx->mapPageVtx[j + 1].v.ob[1] =
                pauseCtx->mapPageVtx[j + 0].v.ob[1] + 2;
            pauseCtx->mapPageVtx[j + 2].v.ob[1] = pauseCtx->mapPageVtx[j + 3].v.ob[1] =
                pauseCtx->mapPageVtx[j + 2].v.ob[1] - 4;
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA, G_CC_MODULATEIA);

    gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[68], 16, 0);

    gDPLoadTextureBlock(POLY_OPA_DISP++, dungeon_name_data[z_common_data.mapIndex], G_IM_FMT_IA, G_IM_SIZ_8b, 96, 16, 0,
                        G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    for (i = 0, j = 4; i < 3; i++, j += 4) {
        if (CHECK_DUNGEON_ITEM(i, z_common_data.mapIndex)) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, dungeon_keep_data[i], G_IM_FMT_RGBA, G_IM_SIZ_32b, 24, 24, 0,
                                G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 200, pauseCtx->alpha);

    gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[84], 32, 0);

    for (i = j = 0; i < 8; i++, j += 4) {
        if ((z_common_data.save.info.sceneFlags[z_common_data.mapIndex].floors & check_bit[i]) ||
            CHECK_DUNGEON_ITEM(DUNGEON_MAP, z_common_data.mapIndex)) {
            if (i != (pauseCtx->dungeonMapSlot - 3)) {
                gDPLoadTextureBlock(POLY_OPA_DISP++, floor_data[map_exp_data_tbl_p->floorID[interfaceCtx->unk_25A][i]],
                                    G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0, G_TX_WRAP | G_TX_NOMIRROR,
                                    G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
            }
        }
    }

    j = (pauseCtx->dungeonMapSlot - 3) * 4;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 150, 255, pauseCtx->alpha);

    gDPLoadTextureBlock(POLY_OPA_DISP++,
                        floor_data[map_exp_data_tbl_p->floorID[interfaceCtx->unk_25A][pauseCtx->dungeonMapSlot - 3]],
                        G_IM_FMT_IA, G_IM_SIZ_8b, 24, 16, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR,
                        G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);

    pauseCtx->mapPageVtx[124].v.ob[0] = pauseCtx->mapPageVtx[126].v.ob[0] = pauseCtx->mapPageVtx[124].v.ob[0] + 2;
    pauseCtx->mapPageVtx[125].v.ob[0] = pauseCtx->mapPageVtx[127].v.ob[0] = pauseCtx->mapPageVtx[124].v.ob[0] + 19;
    pauseCtx->mapPageVtx[124].v.ob[1] = pauseCtx->mapPageVtx[125].v.ob[1] = pauseCtx->mapPageVtx[124].v.ob[1] - 2;
    pauseCtx->mapPageVtx[126].v.ob[1] = pauseCtx->mapPageVtx[127].v.ob[1] = pauseCtx->mapPageVtx[124].v.ob[1] - 19;

    gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[116], 12, 0);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

    pauseCtx->mapPageVtx[116].v.ob[1] = pauseCtx->mapPageVtx[117].v.ob[1] =
        pauseCtx->pagesYOrigin1 + 50 - (VREG(30) * 14) - 1;
    pauseCtx->mapPageVtx[118].v.ob[1] = pauseCtx->mapPageVtx[119].v.ob[1] = pauseCtx->mapPageVtx[116].v.ob[1] - 16;

    gDPLoadTextureBlock(POLY_OPA_DISP++, gDungeonMapLinkHeadTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0,
                        G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    if (CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, z_common_data.mapIndex) &&
        (map_exp_data_tbl_p->skullFloorIconY[z_common_data.mapIndex] != -99)) {
        pauseCtx->mapPageVtx[120].v.ob[1] = pauseCtx->mapPageVtx[121].v.ob[1] =
            map_exp_data_tbl_p->skullFloorIconY[z_common_data.mapIndex] + pauseCtx->pagesYOrigin1;
        pauseCtx->mapPageVtx[122].v.ob[1] = pauseCtx->mapPageVtx[123].v.ob[1] = pauseCtx->mapPageVtx[120].v.ob[1] - 16;

        gDPLoadTextureBlock(POLY_OPA_DISP++, gDungeonMapSkullTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0,
                            G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);
    }

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

    if (GET_GS_FLAGS(z_common_data.mapIndex) == kin_sta_suu[z_common_data.mapIndex]) {
        kaleido_scope_item_set(gfxCtx, gQuestIconGoldSkulltulaTex, QUEST_ICON_WIDTH, QUEST_ICON_HEIGHT, 8);
    }

    if ((play->sceneId >= SCENE_DEKU_TREE) && (play->sceneId <= SCENE_TREASURE_BOX_SHOP)) {
        stepR = (rc - map_color[pt][0]) / cnt;
        stepG = (gc - map_color[pt][1]) / cnt;
        stepB = (bc - map_color[pt][2]) / cnt;
        rc -= stepR;
        gc -= stepG;
        bc -= stepB;

        rgba16 = ((rc & 0x1F) << 11) | ((gc & 0x1F) << 6) | ((bc & 0x1F) << 1) | 1;
        interfaceCtx->mapPalette[28] = (rgba16 & 0xFF00) >> 8;
        interfaceCtx->mapPalette[29] = rgba16 & 0xFF;

        cnt--;
        if (cnt == 0) {
            pt ^= 1;
            cnt = 20;
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_POINT);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);

    gDPLoadTLUT_pal16(POLY_OPA_DISP++, 0, interfaceCtx->mapPalette);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_RGBA16);

    gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[60], 8, 0);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, interfaceCtx->mapSegment, G_IM_FMT_CI, 48, 85, 0, G_TX_WRAP | G_TX_NOMIRROR,
                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, interfaceCtx->mapSegment + 0x800, G_IM_FMT_CI, 48, 85, 0,
                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

    CLOSE_DISPS(gfxCtx, "../z_kaleido_map_PAL.c", 388);
}

void field_map_disp(PlayState* play, GraphicsContext* gfxCtx) {
    static void* cloud_txt_dt[] = {
        gWorldMapCloudSacredForestMeadowTex, // WORLD_MAP_QUAD_CLOUDS_SACRED_FOREST_MEADOW
        gWorldMapCloudHyruleFieldTex,        // WORLD_MAP_QUAD_CLOUDS_HYRULE_FIELD
        gWorldMapCloudLonLonRanchTex,        // WORLD_MAP_QUAD_CLOUDS_LON_LON_RANCH
        gWorldMapCloudMarketTex,             // WORLD_MAP_QUAD_CLOUDS_MARKET
        gWorldMapCloudHyruleCastleTex,       // WORLD_MAP_QUAD_CLOUDS_HYRULE_CASTLE
        gWorldMapCloudKakarikoVillageTex,    // WORLD_MAP_QUAD_CLOUDS_KAKARIKO_VILLAGE
        gWorldMapCloudGraveyardTex,          // WORLD_MAP_QUAD_CLOUDS_GRAVEYARD
        gWorldMapCloudDeathMountainTrailTex, // WORLD_MAP_QUAD_CLOUDS_DEATH_MOUNTAIN_TRAIL
        gWorldMapCloudGoronCityTex,          // WORLD_MAP_QUAD_CLOUDS_GORON_CITY
        gWorldMapCloudZorasRiverTex,         // WORLD_MAP_QUAD_CLOUDS_ZORAS_RIVER
        gWorldMapCloudZorasDomainTex,        // WORLD_MAP_QUAD_CLOUDS_ZORAS_DOMAIN
        gWorldMapCloudZorasFountainTex,      // WORLD_MAP_QUAD_CLOUDS_ZORAS_FOUNTAIN
        gWorldMapCloudGerudoValleyTex,       // WORLD_MAP_QUAD_CLOUDS_GERUDO_VALLEY
        gWorldMapCloudGerudosFortressTex,    // WORLD_MAP_QUAD_CLOUDS_GERUDOS_FORTRESS
        gWorldMapCloudDesertColossusTex,     // WORLD_MAP_QUAD_CLOUDS_DESERT_COLOSSUS
        gWorldMapCloudLakeHyliaTex,          // WORLD_MAP_QUAD_CLOUDS_LAKE_HYLIA
    };
    static u16 cloud_number[] = {
        WORLD_MAP_AREA_SACRED_FOREST_MEADOW, // WORLD_MAP_QUAD_CLOUDS_SACRED_FOREST_MEADOW
        WORLD_MAP_AREA_HYRULE_FIELD,         // WORLD_MAP_QUAD_CLOUDS_HYRULE_FIELD
        WORLD_MAP_AREA_LON_LON_RANCH,        // WORLD_MAP_QUAD_CLOUDS_LON_LON_RANCH
        WORLD_MAP_AREA_MARKET,               // WORLD_MAP_QUAD_CLOUDS_MARKET
        WORLD_MAP_AREA_HYRULE_CASTLE,        // WORLD_MAP_QUAD_CLOUDS_HYRULE_CASTLE
        WORLD_MAP_AREA_KAKARIKO_VILLAGE,     // WORLD_MAP_QUAD_CLOUDS_KAKARIKO_VILLAGE
        WORLD_MAP_AREA_GRAVEYARD,            // WORLD_MAP_QUAD_CLOUDS_GRAVEYARD
        WORLD_MAP_AREA_DEATH_MOUNTAIN_TRAIL, // WORLD_MAP_QUAD_CLOUDS_DEATH_MOUNTAIN_TRAIL
        WORLD_MAP_AREA_GORON_CITY,           // WORLD_MAP_QUAD_CLOUDS_GORON_CITY
        WORLD_MAP_AREA_ZORAS_RIVER,          // WORLD_MAP_QUAD_CLOUDS_ZORAS_RIVER
        WORLD_MAP_AREA_ZORAS_DOMAIN,         // WORLD_MAP_QUAD_CLOUDS_ZORAS_DOMAIN
        WORLD_MAP_AREA_ZORAS_FOUNTAIN,       // WORLD_MAP_QUAD_CLOUDS_ZORAS_FOUNTAIN
        WORLD_MAP_AREA_GERUDO_VALLEY,        // WORLD_MAP_QUAD_CLOUDS_GERUDO_VALLEY
        WORLD_MAP_AREA_GERUDOS_FORTRESS,     // WORLD_MAP_QUAD_CLOUDS_GERUDOS_FORTRESS
        WORLD_MAP_AREA_DESERT_COLOSSUS,      // WORLD_MAP_QUAD_CLOUDS_DESERT_COLOSSUS
        WORLD_MAP_AREA_LAKE_HYLIA,           // WORLD_MAP_QUAD_CLOUDS_LAKE_HYLIA
    };
    static s16 prim_rgb[] = { 0, 0, 255 };
    static s16 prim_color[][3] = {
        { 0, 0, 255 },
        { 255, 255, 0 },
    };
    static s16 env_rgb[] = { 255, 255, 0 };
    static s16 env_color[][3] = {
        { 255, 255, 0 },
        { 0, 0, 255 },
    };
    static s16 mark_color_pt = 1;
    static s16 mark_color_ct = 20;
    static s16 xsize[] = { 64, 64, 64, 28 };
    static s16 area_xdt[] = {
        -41,  // WORLD_MAP_AREA_HYRULE_FIELD
        19,   // WORLD_MAP_AREA_KAKARIKO_VILLAGE
        44,   // WORLD_MAP_AREA_GRAVEYARD
        40,   // WORLD_MAP_AREA_ZORAS_RIVER
        49,   // WORLD_MAP_AREA_KOKIRI_FOREST
        51,   // WORLD_MAP_AREA_SACRED_FOREST_MEADOW
        -49,  // WORLD_MAP_AREA_LAKE_HYLIA
        83,   // WORLD_MAP_AREA_ZORAS_DOMAIN
        80,   // WORLD_MAP_AREA_ZORAS_FOUNTAIN
        -67,  // WORLD_MAP_AREA_GERUDO_VALLEY
        50,   // WORLD_MAP_AREA_LOST_WOODS
        -109, // WORLD_MAP_AREA_DESERT_COLOSSUS
        -76,  // WORLD_MAP_AREA_GERUDOS_FORTRESS
        -86,  // WORLD_MAP_AREA_HAUNTED_WASTELAND
        -10,  // WORLD_MAP_AREA_MARKET
        -6,   // WORLD_MAP_AREA_HYRULE_CASTLE
        19,   // WORLD_MAP_AREA_DEATH_MOUNTAIN_TRAIL
        24,   // WORLD_MAP_AREA_DEATH_MOUNTAIN_CRATER
        11,   // WORLD_MAP_AREA_GORON_CITY
        -17,  // WORLD_MAP_AREA_LON_LON_RANCH
        37,   // WORLD_MAP_AREA_QUESTION_MARK
        -6,   // WORLD_MAP_AREA_GANONS_CASTLE
    };
    static s16 area_xsz[] = {
        96, // WORLD_MAP_AREA_HYRULE_FIELD
        32, // WORLD_MAP_AREA_KAKARIKO_VILLAGE
        32, // WORLD_MAP_AREA_GRAVEYARD
        48, // WORLD_MAP_AREA_ZORAS_RIVER
        48, // WORLD_MAP_AREA_KOKIRI_FOREST
        32, // WORLD_MAP_AREA_SACRED_FOREST_MEADOW
        48, // WORLD_MAP_AREA_LAKE_HYLIA
        32, // WORLD_MAP_AREA_ZORAS_DOMAIN
        32, // WORLD_MAP_AREA_ZORAS_FOUNTAIN
        32, // WORLD_MAP_AREA_GERUDO_VALLEY
        16, // WORLD_MAP_AREA_LOST_WOODS
        32, // WORLD_MAP_AREA_DESERT_COLOSSUS
        32, // WORLD_MAP_AREA_GERUDOS_FORTRESS
        16, // WORLD_MAP_AREA_HAUNTED_WASTELAND
        32, // WORLD_MAP_AREA_MARKET
        32, // WORLD_MAP_AREA_HYRULE_CASTLE
        32, // WORLD_MAP_AREA_DEATH_MOUNTAIN_TRAIL
        32, // WORLD_MAP_AREA_DEATH_MOUNTAIN_CRATER
        32, // WORLD_MAP_AREA_GORON_CITY
        32, // WORLD_MAP_AREA_LON_LON_RANCH
        16, // WORLD_MAP_AREA_QUESTION_MARK
        32, // WORLD_MAP_AREA_GANONS_CASTLE
    };
    static s16 area_ydt[] = {
        30,  // WORLD_MAP_AREA_HYRULE_FIELD
        36,  // WORLD_MAP_AREA_KAKARIKO_VILLAGE
        35,  // WORLD_MAP_AREA_GRAVEYARD
        26,  // WORLD_MAP_AREA_ZORAS_RIVER
        7,   // WORLD_MAP_AREA_KOKIRI_FOREST
        11,  // WORLD_MAP_AREA_SACRED_FOREST_MEADOW
        -31, // WORLD_MAP_AREA_LAKE_HYLIA
        30,  // WORLD_MAP_AREA_ZORAS_DOMAIN
        38,  // WORLD_MAP_AREA_ZORAS_FOUNTAIN
        23,  // WORLD_MAP_AREA_GERUDO_VALLEY
        2,   // WORLD_MAP_AREA_LOST_WOODS
        42,  // WORLD_MAP_AREA_DESERT_COLOSSUS
        40,  // WORLD_MAP_AREA_GERUDOS_FORTRESS
        32,  // WORLD_MAP_AREA_HAUNTED_WASTELAND
        38,  // WORLD_MAP_AREA_MARKET
        50,  // WORLD_MAP_AREA_HYRULE_CASTLE
        57,  // WORLD_MAP_AREA_DEATH_MOUNTAIN_TRAIL
        58,  // WORLD_MAP_AREA_DEATH_MOUNTAIN_CRATER
        56,  // WORLD_MAP_AREA_GORON_CITY
        12,  // WORLD_MAP_AREA_LON_LON_RANCH
        36,  // WORLD_MAP_AREA_QUESTION_MARK
        50,  // WORLD_MAP_AREA_GANONS_CASTLE
    };
    static s16 area_ysz[] = {
        59, // WORLD_MAP_AREA_HYRULE_FIELD
        19, // WORLD_MAP_AREA_KAKARIKO_VILLAGE
        13, // WORLD_MAP_AREA_GRAVEYARD
        19, // WORLD_MAP_AREA_ZORAS_RIVER
        38, // WORLD_MAP_AREA_KOKIRI_FOREST
        17, // WORLD_MAP_AREA_SACRED_FOREST_MEADOW
        38, // WORLD_MAP_AREA_LAKE_HYLIA
        17, // WORLD_MAP_AREA_ZORAS_DOMAIN
        13, // WORLD_MAP_AREA_ZORAS_FOUNTAIN
        26, // WORLD_MAP_AREA_GERUDO_VALLEY
        16, // WORLD_MAP_AREA_LOST_WOODS
        26, // WORLD_MAP_AREA_DESERT_COLOSSUS
        26, // WORLD_MAP_AREA_GERUDOS_FORTRESS
        16, // WORLD_MAP_AREA_HAUNTED_WASTELAND
        19, // WORLD_MAP_AREA_MARKET
        17, // WORLD_MAP_AREA_HYRULE_CASTLE
        26, // WORLD_MAP_AREA_DEATH_MOUNTAIN_TRAIL
        13, // WORLD_MAP_AREA_DEATH_MOUNTAIN_CRATER
        17, // WORLD_MAP_AREA_GORON_CITY
        17, // WORLD_MAP_AREA_LON_LON_RANCH
        16, // WORLD_MAP_AREA_QUESTION_MARK
        17, // WORLD_MAP_AREA_GANONS_CASTLE
    };
    static void* area_data_txt[] = {
        gWorldMapAreaBox7Tex, // WORLD_MAP_AREA_HYRULE_FIELD
        gWorldMapAreaBox1Tex, // WORLD_MAP_AREA_KAKARIKO_VILLAGE
        gWorldMapAreaBox4Tex, // WORLD_MAP_AREA_GRAVEYARD
        gWorldMapAreaBox6Tex, // WORLD_MAP_AREA_ZORAS_RIVER
        gWorldMapAreaBox2Tex, // WORLD_MAP_AREA_KOKIRI_FOREST
        gWorldMapAreaBox3Tex, // WORLD_MAP_AREA_SACRED_FOREST_MEADOW
        gWorldMapAreaBox2Tex, // WORLD_MAP_AREA_LAKE_HYLIA
        gWorldMapAreaBox3Tex, // WORLD_MAP_AREA_ZORAS_DOMAIN
        gWorldMapAreaBox4Tex, // WORLD_MAP_AREA_ZORAS_FOUNTAIN
        gWorldMapAreaBox5Tex, // WORLD_MAP_AREA_GERUDO_VALLEY
        gWorldMapAreaBox8Tex, // WORLD_MAP_AREA_LOST_WOODS
        gWorldMapAreaBox5Tex, // WORLD_MAP_AREA_DESERT_COLOSSUS
        gWorldMapAreaBox5Tex, // WORLD_MAP_AREA_GERUDOS_FORTRESS
        gWorldMapAreaBox8Tex, // WORLD_MAP_AREA_HAUNTED_WASTELAND
        gWorldMapAreaBox1Tex, // WORLD_MAP_AREA_MARKET
        gWorldMapAreaBox3Tex, // WORLD_MAP_AREA_HYRULE_CASTLE
        gWorldMapAreaBox5Tex, // WORLD_MAP_AREA_DEATH_MOUNTAIN_TRAIL
        gWorldMapAreaBox4Tex, // WORLD_MAP_AREA_DEATH_MOUNTAIN_CRATER
        gWorldMapAreaBox3Tex, // WORLD_MAP_AREA_GORON_CITY
        gWorldMapAreaBox3Tex, // WORLD_MAP_AREA_LON_LON_RANCH
        gWorldMapAreaBox8Tex, // WORLD_MAP_AREA_QUESTION_MARK
        gWorldMapAreaBox3Tex, // WORLD_MAP_AREA_GANONS_CASTLE
    };
    static void* here_txt[] = LANGUAGE_ARRAY(gPauseCurrentPositionJPNTex, gPauseCurrentPositionENGTex,
                                                        gPauseCurrentPositionGERTex, gPauseCurrentPositionFRATex);
    static u16 spot_timer = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 i;
    s16 j;
    s16 t;
    s16 k;
    s16 oldCursorPoint;
    s16 stepR;
    s16 stepG;
    s16 stepB;

    OPEN_DISPS(gfxCtx, "../z_kaleido_map_PAL.c", 556);

    if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_IDLE) &&
        (pauseCtx->pageIndex == PAUSE_MAP)) {
        pauseCtx->cursorColorSet = 0;
        oldCursorPoint = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];

        if (pauseCtx->cursorSpecialPos == 0) {
            if (pauseCtx->stickAdjX > 30) {
                do {
                    spot_timer = 0;
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP]++;
                    if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] >= WORLD_MAP_POINT_MAX) {
                        pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = WORLD_MAP_POINT_MAX - 1;
                        key_arrangement(play, PAUSE_CURSOR_PAGE_RIGHT);
                        break;
                    }
                } while (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] ==
                         WORLD_MAP_POINT_STATE_HIDE);
            } else if (pauseCtx->stickAdjX < -30) {
                do {
                    spot_timer = 0;
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP]--;
                    if (pauseCtx->cursorPoint[PAUSE_WORLD_MAP] < 0) {
                        pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = 0;
                        key_arrangement(play, PAUSE_CURSOR_PAGE_LEFT);
                        break;
                    }
                } while (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] ==
                         WORLD_MAP_POINT_STATE_HIDE);
            } else {
                spot_timer++;
            }

            pauseCtx->cursorItem[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
            pauseCtx->cursorSlot[PAUSE_MAP] =
                PAGE_BG_QUADS + WORLD_MAP_QUAD_POINT_FIRST + pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
            cursol_defalute(pauseCtx, pauseCtx->cursorSlot[PAUSE_MAP] * 4, pauseCtx->mapPageVtx);
        } else {
            pauseCtx->cursorItem[PAUSE_MAP] = z_common_data.worldMapArea + 0x18;
            if (pauseCtx->cursorSpecialPos == PAUSE_CURSOR_PAGE_LEFT) {
                if (pauseCtx->stickAdjX > 30) {
                    pauseCtx->cursorSpecialPos = 0;
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = 0;

                    while (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] ==
                           WORLD_MAP_POINT_STATE_HIDE) {
                        pauseCtx->cursorPoint[PAUSE_WORLD_MAP]++;
                    }

                    pauseCtx->cursorItem[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
                    pauseCtx->cursorSlot[PAUSE_MAP] =
                        PAGE_BG_QUADS + WORLD_MAP_QUAD_POINT_FIRST + pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
                    cursol_defalute(pauseCtx, pauseCtx->cursorSlot[PAUSE_MAP] * 4, pauseCtx->mapPageVtx);
                    Nai_FxFlagEntry(NA_SE_SY_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    spot_timer = 0;
                }
            } else {
                if (pauseCtx->stickAdjX < -30) {
                    pauseCtx->cursorSpecialPos = 0;
                    pauseCtx->cursorPoint[PAUSE_WORLD_MAP] = WORLD_MAP_POINT_MAX - 1;

                    while (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] ==
                           WORLD_MAP_POINT_STATE_HIDE) {
                        pauseCtx->cursorPoint[PAUSE_WORLD_MAP]--;
                    }

                    pauseCtx->cursorItem[PAUSE_MAP] = pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
                    pauseCtx->cursorSlot[PAUSE_MAP] =
                        PAGE_BG_QUADS + WORLD_MAP_QUAD_POINT_FIRST + pauseCtx->cursorPoint[PAUSE_WORLD_MAP];
                    cursol_defalute(pauseCtx, pauseCtx->cursorSlot[PAUSE_MAP] * 4, pauseCtx->mapPageVtx);
                    Nai_FxFlagEntry(NA_SE_SY_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    spot_timer = 0;
                }
            }
        }

        if (pauseCtx->worldMapPoints[pauseCtx->cursorPoint[PAUSE_WORLD_MAP]] == WORLD_MAP_POINT_STATE_HIDE) {
            pauseCtx->cursorItem[PAUSE_MAP] = PAUSE_ITEM_NONE;
        }

        if (oldCursorPoint != pauseCtx->cursorPoint[PAUSE_WORLD_MAP]) {
            Nai_FxFlagEntry(NA_SE_SY_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);

#if PLATFORM_N64 && OOT_VERSION != NTSC_1_2
    if (0) {
        s32 pad[3];
    }
#endif

#if DEBUG_FEATURES
    if (HREG(15) == 0) {
        gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_POINT);

        gDPLoadTLUT_pal256(POLY_OPA_DISP++, gWorldMapImageTLUT);
        gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_RGBA16);

        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
        gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_IMAGE_FIRST) * 4], 32, 0);

        for (j = t = i = 0; i < 8; i++, t++, j += 4) {
            gDPLoadTextureBlock(
                POLY_OPA_DISP++, (u8*)gWorldMapImageTex + t * WORLD_MAP_IMAGE_WIDTH * WORLD_MAP_IMAGE_FRAG_HEIGHT,
                G_IM_FMT_CI, G_IM_SIZ_8b, WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_FRAG_HEIGHT, 0,
                G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
        }

        gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_IMAGE_FIRST + 8) * 4], 28, 0);

        for (j = i = 0; i < 6; i++, t++, j += 4) {
            gDPLoadTextureBlock(
                POLY_OPA_DISP++, (u8*)gWorldMapImageTex + t * WORLD_MAP_IMAGE_WIDTH * WORLD_MAP_IMAGE_FRAG_HEIGHT,
                G_IM_FMT_CI, G_IM_SIZ_8b, WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_FRAG_HEIGHT, 0,
                G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
        }

        gDPLoadTextureBlock(
            POLY_OPA_DISP++, (u8*)gWorldMapImageTex + t * WORLD_MAP_IMAGE_WIDTH * WORLD_MAP_IMAGE_FRAG_HEIGHT,
            G_IM_FMT_CI, G_IM_SIZ_8b, WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_HEIGHT % WORLD_MAP_IMAGE_FRAG_HEIGHT, 0,
            G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
    } else if (HREG(15) == 1) {
        Gfx* gfx = POLY_OPA_DISP;

        gSPLoadUcodeL(gfx++, gspS2DEX2d_fifo);

        prerender_draw2(&gfx, gWorldMapImageTex, gWorldMapImageTLUT, WORLD_MAP_IMAGE_WIDTH,
                              WORLD_MAP_IMAGE_HEIGHT, G_IM_FMT_CI, G_IM_SIZ_8b, G_TT_RGBA16, 256, HREG(13) / 100.0f,
                              HREG(14) / 100.0f);

        gSPLoadUcode(gfx++, ucode_GetPolyTextStart(), ucode_GetPolyDataStart());

        POLY_OPA_DISP = gfx;
    }

    if (HREG(15) == 2) {
        HREG(15) = 1;
        HREG(14) = 6100;
        HREG(13) = 5300;
    }
#else
    // Same as `HREG(15) == 0` case above
    gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_POINT);

    gDPLoadTLUT_pal256(POLY_OPA_DISP++, gWorldMapImageTLUT);
    gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_RGBA16);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, pauseCtx->alpha);
    gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[188], 32, 0);

    for (j = t = i = 0; i < 8; i++, t++, j += 4) {
        gDPLoadTextureBlock(
            POLY_OPA_DISP++, (u8*)gWorldMapImageTex + t * WORLD_MAP_IMAGE_WIDTH * WORLD_MAP_IMAGE_FRAG_HEIGHT,
            G_IM_FMT_CI, G_IM_SIZ_8b, WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_FRAG_HEIGHT, 0, G_TX_WRAP | G_TX_NOMIRROR,
            G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
    }

    gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[220], 28, 0);

    for (j = i = 0; i < 6; i++, t++, j += 4) {
        gDPLoadTextureBlock(
            POLY_OPA_DISP++, (u8*)gWorldMapImageTex + t * WORLD_MAP_IMAGE_WIDTH * WORLD_MAP_IMAGE_FRAG_HEIGHT,
            G_IM_FMT_CI, G_IM_SIZ_8b, WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_FRAG_HEIGHT, 0, G_TX_WRAP | G_TX_NOMIRROR,
            G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
    }

    gDPLoadTextureBlock(
        POLY_OPA_DISP++, (u8*)gWorldMapImageTex + t * WORLD_MAP_IMAGE_WIDTH * WORLD_MAP_IMAGE_FRAG_HEIGHT, G_IM_FMT_CI,
        G_IM_SIZ_8b, WORLD_MAP_IMAGE_WIDTH, WORLD_MAP_IMAGE_HEIGHT % WORLD_MAP_IMAGE_FRAG_HEIGHT, 0,
        G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
#endif

    if (ZREG(38) == 0) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);

        kaleido_scope_prim(gfxCtx);

        gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                          PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 235, 235, 235, pauseCtx->alpha);

        for (k = 0; k < 15; k += 8) {
            gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[(PAGE_BG_QUADS + k) * 4], 32, 0);

            for (j = i = 0; i < 8; i++, j += 4) {
                if (!(z_common_data.save.info.worldMapAreaData & check_bit[cloud_number[k + i]])) {
                    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, cloud_txt_dt[k + i], G_IM_FMT_I,
                                           partvtx_mapg_xsz[k + i], partvtx_mapg_ysz[k + i], 0,
                                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK,
                                           G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSP1Quadrangle(POLY_OPA_DISP++, j, j + 2, j + 3, j + 1, 0);
                }
            }
        }
    }

    if (z_common_data.worldMapArea < WORLD_MAP_AREA_MAX) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_POINT);

        // Quad PAGE_BG_QUADS + WORLD_MAP_QUAD_28

        pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 0].v.ob[0] =
            pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 2].v.ob[0] =
                area_xdt[((void)0, z_common_data.worldMapArea)];

        pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 1].v.ob[0] =
            pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 3].v.ob[0] =
                pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 0].v.ob[0] +
                area_xsz[((void)0, z_common_data.worldMapArea)];

        pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 0].v.ob[1] =
            pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 1].v.ob[1] =
                area_ydt[((void)0, z_common_data.worldMapArea)] + pauseCtx->pagesYOrigin1;

        pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 2].v.ob[1] =
            pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 3].v.ob[1] =
                pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 0].v.ob[1] -
                area_ysz[((void)0, z_common_data.worldMapArea)];

        pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 1].v.tc[0] =
            pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 3].v.tc[0] =
                area_xsz[((void)0, z_common_data.worldMapArea)] << 5;

        pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 2].v.tc[1] =
            pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4 + 3].v.tc[1] =
                area_ysz[((void)0, z_common_data.worldMapArea)] << 5;

        gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_28) * 4], 4, 0);

        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 100, 255, 255, pauseCtx->alpha);

        gDPLoadTextureBlock_4b(POLY_OPA_DISP++, area_data_txt[((void)0, (z_common_data.worldMapArea))], G_IM_FMT_IA,
                               area_xsz[((void)0, (z_common_data.worldMapArea))],
                               area_ysz[((void)0, (z_common_data.worldMapArea))], 0, G_TX_WRAP | G_TX_NOMIRROR,
                               G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);
    }

    stepR = ABS(prim_rgb[0] - prim_color[mark_color_pt][0]) / mark_color_ct;
    stepG = ABS(prim_rgb[1] - prim_color[mark_color_pt][1]) / mark_color_ct;
    stepB = ABS(prim_rgb[2] - prim_color[mark_color_pt][2]) / mark_color_ct;
    if (prim_rgb[0] >= prim_color[mark_color_pt][0]) {
        prim_rgb[0] -= stepR;
    } else {
        prim_rgb[0] += stepR;
    }
    if (prim_rgb[1] >= prim_color[mark_color_pt][1]) {
        prim_rgb[1] -= stepG;
    } else {
        prim_rgb[1] += stepG;
    }
    if (prim_rgb[2] >= prim_color[mark_color_pt][2]) {
        prim_rgb[2] -= stepB;
    } else {
        prim_rgb[2] += stepB;
    }

    stepR = ABS(env_rgb[0] - env_color[mark_color_pt][0]) / mark_color_ct;
    stepG = ABS(env_rgb[1] - env_color[mark_color_pt][1]) / mark_color_ct;
    stepB = ABS(env_rgb[2] - env_color[mark_color_pt][2]) / mark_color_ct;
    if (env_rgb[0] >= env_color[mark_color_pt][0]) {
        env_rgb[0] -= stepR;
    } else {
        env_rgb[0] += stepR;
    }
    if (env_rgb[1] >= env_color[mark_color_pt][1]) {
        env_rgb[1] -= stepG;
    } else {
        env_rgb[1] += stepG;
    }
    if (env_rgb[2] >= env_color[mark_color_pt][2]) {
        env_rgb[2] -= stepB;
    } else {
        env_rgb[2] += stepB;
    }

    mark_color_ct--;
    if (mark_color_ct == 0) {
        prim_rgb[0] = prim_color[mark_color_pt][0];
        prim_rgb[1] = prim_color[mark_color_pt][1];
        prim_rgb[2] = prim_color[mark_color_pt][2];
        env_rgb[0] = env_color[mark_color_pt][0];
        env_rgb[1] = env_color[mark_color_pt][1];
        env_rgb[2] = env_color[mark_color_pt][2];
        mark_color_ct = 20;
        mark_color_pt ^= 1;
    }

    kaleido_scope_prim(gfxCtx);

    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    gDPLoadTextureBlock(POLY_OPA_DISP++, gWorldMapDotTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0, G_TX_WRAP | G_TX_NOMIRROR,
                        G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    for (j = i = 0; i < WORLD_MAP_POINT_MAX; i++, t++, j += 4) {
        if (pauseCtx->worldMapPoints[i] != WORLD_MAP_POINT_STATE_HIDE) {
            gDPPipeSync(POLY_OPA_DISP++);

            if (pauseCtx->worldMapPoints[i] == WORLD_MAP_POINT_STATE_SHOW) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, prim_color[0][0], prim_color[0][1],
                                prim_color[0][2], pauseCtx->alpha);
                gDPSetEnvColor(POLY_OPA_DISP++, env_color[0][0], env_color[0][1], env_color[0][2], 0);
            } else { // WORLD_MAP_POINT_STATE_HIGHLIGHT
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, prim_rgb[0], prim_rgb[1],
                                prim_rgb[2], pauseCtx->alpha);
                gDPSetEnvColor(POLY_OPA_DISP++, env_rgb[0], env_rgb[1], env_rgb[2], 0);
            }

            gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_POINT_FIRST + i) * 4], 4,
                      0);

            gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
        }
    }

    if (pauseCtx->cursorSpecialPos == 0) {
        cursor_draw(play, PAUSE_MAP);
    }

    gSPVertex(POLY_OPA_DISP++, &pauseCtx->mapPageVtx[(PAGE_BG_QUADS + WORLD_MAP_QUAD_TRADE_QUEST_MARKER) * 4], 16, 0);

    if (pauseCtx->tradeQuestMarker != TRADE_QUEST_MARKER_NONE) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, prim_rgb[0], 0, pauseCtx->alpha);

        gDPLoadTextureBlock(POLY_OPA_DISP++, gWorldMapArrowTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 8, 0,
                            G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

        gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    }

    if (z_common_data.worldMapArea < WORLD_MAP_AREA_MAX) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 150, 255, 255, pauseCtx->alpha);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        POLY_OPA_DISP = texture_QuadrangleIA8(POLY_OPA_DISP,
                                                    pauseCtx->nameSegment + MAX(MAP_NAME_TEX1_SIZE, ITEM_NAME_TEX_SIZE),
                                                    MAP_NAME_TEX2_WIDTH, MAP_NAME_TEX2_HEIGHT, 4);
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, pauseCtx->alpha);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, here_txt[z_common_data.language], G_IM_FMT_I, 64, 8, 0,
                           G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    gSP1Quadrangle(POLY_OPA_DISP++, 8, 10, 11, 9, 0);

    gDPPipeSync(POLY_OPA_DISP++);

    CLOSE_DISPS(gfxCtx, "../z_kaleido_map_PAL.c", 874);
}
