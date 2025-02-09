#include "global.h"
#include "terminal.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/textures/parameter_static/parameter_static.h"
#if PLATFORM_N64
#include "n64dd.h"
#endif

MapData* map_exp_data_tbl_p;

s16 player_xpos = 0;
s16 player_zpos = 0;
s16 player_angley = 0;
s16 real_scene_data_ID = 0;

void player_position_hold(PlayState* play) {
    Player* player = GET_PLAYER(play);

    player_xpos = player->actor.world.pos.x;
    player_zpos = player->actor.world.pos.z;
    player_angley = (s16)((0x7FFF - player->actor.shape.rot.y) / 0x400);
}

void map_palete_set(PlayState* play, s16 room) {
    s32 mapIndex = z_common_data.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 paletteIndex = map_exp_data_tbl_p->roomPalette[mapIndex][room];

    if (interfaceCtx->mapRoomNum == room) {
        interfaceCtx->mapPaletteIndex = paletteIndex;
    }

    PRINTF_COLOR_YELLOW();
    PRINTF(T("ＰＡＬＥＴＥセット 【 i=%x : room=%x 】Room_Inf[%d][4]=%x  ( map_palete_no = %d )\n",
             "PALETE Set 【 i=%x : room=%x 】Room_Inf[%d][4]=%x  ( map_palete_no = %d )\n"),
           paletteIndex, room, mapIndex, z_common_data.save.info.sceneFlags[mapIndex].rooms,
           interfaceCtx->mapPaletteIndex);
    PRINTF_RST();

    interfaceCtx->mapPalette[paletteIndex * 2] = 2;
    interfaceCtx->mapPalette[paletteIndex * 2 + 1] = 0xBF;
}

void map_floor_check(PlayState* play, s16 floor) {
    s32 mapIndex = z_common_data.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 room;
    s16 i;

    for (i = 0; i < 16; i++) {
        interfaceCtx->mapPalette[i] = 0;
        interfaceCtx->mapPalette[i + 16] = 0;
    }

    if (CHECK_DUNGEON_ITEM(DUNGEON_MAP, mapIndex)) {
        interfaceCtx->mapPalette[30] = 0;
        interfaceCtx->mapPalette[31] = 1;
    }

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
        case SCENE_WATER_TEMPLE_BOSS:
        case SCENE_SPIRIT_TEMPLE_BOSS:
        case SCENE_SHADOW_TEMPLE_BOSS:
            for (i = 0; i < map_exp_data_tbl_p->maxPaletteCount[mapIndex]; i++) {
                room = map_exp_data_tbl_p->paletteRoom[mapIndex][floor][i];
                if ((room != 0xFF) && (z_common_data.save.info.sceneFlags[mapIndex].rooms & check_bit[room])) {
                    map_palete_set(play, room);
                }
            }
            break;
    }
}

void map_textuer_dma(PlayState* play, s16 room) {
    s32 mapIndex = z_common_data.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 extendedMapIndex;

    switch (play->sceneId) {
        case SCENE_HYRULE_FIELD:
        case SCENE_KAKARIKO_VILLAGE:
        case SCENE_GRAVEYARD:
        case SCENE_ZORAS_RIVER:
        case SCENE_KOKIRI_FOREST:
        case SCENE_SACRED_FOREST_MEADOW:
        case SCENE_LAKE_HYLIA:
        case SCENE_ZORAS_DOMAIN:
        case SCENE_ZORAS_FOUNTAIN:
        case SCENE_GERUDO_VALLEY:
        case SCENE_LOST_WOODS:
        case SCENE_DESERT_COLOSSUS:
        case SCENE_GERUDOS_FORTRESS:
        case SCENE_HAUNTED_WASTELAND:
        case SCENE_HYRULE_CASTLE:
        case SCENE_DEATH_MOUNTAIN_TRAIL:
        case SCENE_DEATH_MOUNTAIN_CRATER:
        case SCENE_GORON_CITY:
        case SCENE_LON_LON_RANCH:
        case SCENE_OUTSIDE_GANONS_CASTLE:
            extendedMapIndex = mapIndex;
            if (play->sceneId == SCENE_GRAVEYARD) {
                if (CHECK_QUEST_ITEM(QUEST_SONG_NOCTURNE)) {
                    extendedMapIndex = 0x14;
                }
            } else if (play->sceneId == SCENE_LAKE_HYLIA) {
                if ((LINK_AGE_IN_YEARS == YEARS_ADULT) && !CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER)) {
                    extendedMapIndex = 0x15;
                }
            } else if (play->sceneId == SCENE_GERUDO_VALLEY) {
                if ((LINK_AGE_IN_YEARS == YEARS_ADULT) && !GET_EVENTCHKINF_CARPENTERS_ALL_RESCUED()) {
                    extendedMapIndex = 0x16;
                }
            } else if (play->sceneId == SCENE_GERUDOS_FORTRESS) {
                if (GET_EVENTCHKINF_CARPENTERS_ALL_RESCUED()) {
                    extendedMapIndex = 0x17;
                }
            }
            PRINTF_COLOR_BLUE();
            PRINTF("ＫＫＫ＝%d\n", extendedMapIndex);
            PRINTF_RST();
            real_scene_data_ID = extendedMapIndex;
            DMA_REQUEST_SYNC(interfaceCtx->mapSegment,
                             (uintptr_t)_map_grand_staticSegmentRomStart +
                                 map_exp_data_tbl_p->owMinimapTexOffset[extendedMapIndex],
                             map_exp_data_tbl_p->owMinimapTexSize[mapIndex], "../z_map_exp.c", 309);
            interfaceCtx->unk_258 = mapIndex;
            break;
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
        case SCENE_WATER_TEMPLE_BOSS:
        case SCENE_SPIRIT_TEMPLE_BOSS:
        case SCENE_SHADOW_TEMPLE_BOSS:
            PRINTF_COLOR_YELLOW();
            PRINTF(T("デクの樹ダンジョンＭＡＰ テクスチャＤＭＡ(%x) scene_id_offset=%d  VREG(30)=%d\n",
                     "Deku Tree Dungeon MAP Texture DMA(%x) scene_id_offset=%d  VREG(30)=%d\n"),
                   room, mapIndex, VREG(30));
            PRINTF_RST();

#if PLATFORM_N64
            if ((B_80121220 != NULL) && (B_80121220->unk_28 != NULL) && B_80121220->unk_28(play)) {
            } else {
                DMA_REQUEST_SYNC(play->interfaceCtx.mapSegment,
                                 (uintptr_t)_map_i_staticSegmentRomStart +
                                     ((map_exp_data_tbl_p->dgnMinimapTexIndexOffset[mapIndex] + room) * MAP_I_TEX_SIZE),
                                 MAP_I_TEX_SIZE, "../z_map_exp.c", UNK_LINE);
            }
#else
            DMA_REQUEST_SYNC(play->interfaceCtx.mapSegment,
                             (uintptr_t)_map_i_staticSegmentRomStart +
                                 ((map_exp_data_tbl_p->dgnMinimapTexIndexOffset[mapIndex] + room) * MAP_I_TEX_SIZE),
                             MAP_I_TEX_SIZE, "../z_map_exp.c", 346);
#endif

            R_COMPASS_OFFSET_X = map_exp_data_tbl_p->roomCompassOffsetX[mapIndex][room];
            R_COMPASS_OFFSET_Y = map_exp_data_tbl_p->roomCompassOffsetY[mapIndex][room];
            map_floor_check(play, VREG(30));
            PRINTF(T("ＭＡＰ 各階ＯＮチェック\n", "MAP Individual Floor ON Check\n"));
            break;
    }
}

void map_enter_set(PlayState* play, s16 room) {
    s32 mapIndex = z_common_data.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    PRINTF("＊＊＊＊＊＊＊\n＊＊＊＊＊＊＊\nroom_no=%d (%d)(%d)\n＊＊＊＊＊＊＊\n＊＊＊＊＊＊＊\n", room, mapIndex,
           play->sceneId);

    if (room >= 0) {
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
            case SCENE_WATER_TEMPLE_BOSS:
            case SCENE_SPIRIT_TEMPLE_BOSS:
            case SCENE_SHADOW_TEMPLE_BOSS:
                z_common_data.save.info.sceneFlags[mapIndex].rooms |= check_bit[room];
                PRINTF("ＲＯＯＭ＿ＩＮＦ＝%d\n", z_common_data.save.info.sceneFlags[mapIndex].rooms);
                interfaceCtx->mapRoomNum = room;
                interfaceCtx->unk_25A = mapIndex;
                map_palete_set(play, room);
                PRINTF_COLOR_YELLOW();
                PRINTF(T("部屋部屋＝%d\n", "Room Room = %d\n"), room);
                PRINTF_RST();
                map_textuer_dma(play, room);
                break;
        }
    } else {
        interfaceCtx->mapRoomNum = 0;
    }

    if (z_common_data.sunsSongState != SUNSSONG_SPEED_TIME) {
        z_common_data.sunsSongState = SUNSSONG_INACTIVE;
    }
}

void map_exp_dt(PlayState* play) {
    MapMarkCleanup(play);

#if PLATFORM_N64
    if ((B_80121220 != NULL) && (B_80121220->unk_24 != NULL)) {
        B_80121220->unk_24();
    }
    if ((B_80121220 != NULL) && (B_80121220->unk_1C != NULL)) {
        B_80121220->unk_1C(&map_exp_data_tbl_p);
    }
#endif

    map_exp_data_tbl_p = NULL;
}

void map_exp_ct(PlayState* play) {
    s32 mapIndex = z_common_data.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    map_exp_data_tbl_p = &rom_map_exp_data_tbl;

#if PLATFORM_N64
    if ((B_80121220 != NULL) && (B_80121220->unk_18 != NULL)) {
        B_80121220->unk_18(&map_exp_data_tbl_p);
    }
#endif

    interfaceCtx->unk_258 = -1;
    interfaceCtx->unk_25A = -1;

    interfaceCtx->mapSegment = GAME_STATE_ALLOC(&play->state, 0x1000, "../z_map_exp.c", 457);
    PRINTF(T("\n\n\nＭＡＰ テクスチャ初期化   scene_data_ID=%d\nmapSegment=%x\n\n",
             "\n\n\nMAP texture initialization   scene_data_ID=%d\nmapSegment=%x\n\n"),
           play->sceneId, interfaceCtx->mapSegment);
    ASSERT(interfaceCtx->mapSegment != NULL, "parameter->mapSegment != NULL", "../z_map_exp.c", 459);

    switch (play->sceneId) {
        case SCENE_HYRULE_FIELD:
        case SCENE_KAKARIKO_VILLAGE:
        case SCENE_GRAVEYARD:
        case SCENE_ZORAS_RIVER:
        case SCENE_KOKIRI_FOREST:
        case SCENE_SACRED_FOREST_MEADOW:
        case SCENE_LAKE_HYLIA:
        case SCENE_ZORAS_DOMAIN:
        case SCENE_ZORAS_FOUNTAIN:
        case SCENE_GERUDO_VALLEY:
        case SCENE_LOST_WOODS:
        case SCENE_DESERT_COLOSSUS:
        case SCENE_GERUDOS_FORTRESS:
        case SCENE_HAUNTED_WASTELAND:
        case SCENE_HYRULE_CASTLE:
        case SCENE_DEATH_MOUNTAIN_TRAIL:
        case SCENE_DEATH_MOUNTAIN_CRATER:
        case SCENE_GORON_CITY:
        case SCENE_LON_LON_RANCH:
        case SCENE_OUTSIDE_GANONS_CASTLE:
            mapIndex = play->sceneId - SCENE_HYRULE_FIELD;
            R_MAP_INDEX = z_common_data.mapIndex = mapIndex;
            R_COMPASS_SCALE_X = map_exp_data_tbl_p->owCompassInfo[mapIndex][0];
            R_COMPASS_SCALE_Y = map_exp_data_tbl_p->owCompassInfo[mapIndex][1];
            R_COMPASS_OFFSET_X = map_exp_data_tbl_p->owCompassInfo[mapIndex][2];
            R_COMPASS_OFFSET_Y = map_exp_data_tbl_p->owCompassInfo[mapIndex][3];
            map_textuer_dma(play, mapIndex);
            R_OW_MINIMAP_X = map_exp_data_tbl_p->owMinimapPosX[mapIndex];
            R_OW_MINIMAP_Y = map_exp_data_tbl_p->owMinimapPosY[mapIndex];
            break;
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
        case SCENE_GANONS_TOWER:
        case SCENE_GERUDO_TRAINING_GROUND:
        case SCENE_THIEVES_HIDEOUT:
        case SCENE_INSIDE_GANONS_CASTLE:
        case SCENE_GANONS_TOWER_COLLAPSE_INTERIOR:
        case SCENE_INSIDE_GANONS_CASTLE_COLLAPSE:
        case SCENE_TREASURE_BOX_SHOP:
        case SCENE_DEKU_TREE_BOSS:
        case SCENE_DODONGOS_CAVERN_BOSS:
        case SCENE_JABU_JABU_BOSS:
        case SCENE_FOREST_TEMPLE_BOSS:
        case SCENE_FIRE_TEMPLE_BOSS:
        case SCENE_WATER_TEMPLE_BOSS:
        case SCENE_SPIRIT_TEMPLE_BOSS:
        case SCENE_SHADOW_TEMPLE_BOSS:
            mapIndex = (play->sceneId >= SCENE_DEKU_TREE_BOSS) ? play->sceneId - SCENE_DEKU_TREE_BOSS : play->sceneId;
            R_MAP_INDEX = z_common_data.mapIndex = mapIndex;
            if ((play->sceneId <= SCENE_ICE_CAVERN) || (play->sceneId >= SCENE_DEKU_TREE_BOSS)) {
                R_COMPASS_SCALE_X = map_exp_data_tbl_p->dgnCompassInfo[mapIndex][0];
                R_COMPASS_SCALE_Y = map_exp_data_tbl_p->dgnCompassInfo[mapIndex][1];
                R_COMPASS_OFFSET_X = map_exp_data_tbl_p->dgnCompassInfo[mapIndex][2];
                R_COMPASS_OFFSET_Y = map_exp_data_tbl_p->dgnCompassInfo[mapIndex][3];
                R_MAP_TEX_INDEX = R_MAP_TEX_INDEX_BASE = map_exp_data_tbl_p->dgnTexIndexBase[mapIndex];
#if PLATFORM_N64
                if ((B_80121220 != NULL) && (B_80121220->unk_20 != NULL)) {
                    B_80121220->unk_20(map_exp_data_tbl_p);
                }
#endif
                map_enter_set(play, play->roomCtx.curRoom.num);
                MapMarkInit(play);
            }
            break;
    }
}

void compass_draw(PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s16 tempX, tempZ;

    OPEN_DISPS(play->state.gfxCtx, "../z_map_exp.c", 565);

    if (play->interfaceCtx.minimapAlpha >= 0xAA) {
        overlay_kaleido_scope_prim(play->state.gfxCtx);

        gSPMatrix(OVERLAY_DISP++, &Mtx_clear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);

        tempX = player->actor.world.pos.x;
        tempZ = player->actor.world.pos.z;
        tempX /= R_COMPASS_SCALE_X;
        tempZ /= R_COMPASS_SCALE_Y;
        Matrix_translate((R_COMPASS_OFFSET_X + tempX) / 10.0f, (R_COMPASS_OFFSET_Y - tempZ) / 10.0f, 0.0f, MTXMODE_NEW);
        Matrix_scale(0.4f, 0.4f, 0.4f, MTXMODE_APPLY);
        Matrix_rotateX(-1.6f, MTXMODE_APPLY);
        tempX = (0x7FFF - player->actor.shape.rot.y) / 0x400;
        Matrix_rotateY(tempX / 10.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(OVERLAY_DISP++, play->state.gfxCtx, "../z_map_exp.c", 585);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 200, 255, 0, 255);
        gSPDisplayList(OVERLAY_DISP++, gCompassArrowDL);

        tempX = player_xpos;
        tempZ = player_zpos;
        tempX /= R_COMPASS_SCALE_X;
        tempZ /= R_COMPASS_SCALE_Y;
        Matrix_translate((R_COMPASS_OFFSET_X + tempX) / 10.0f, (R_COMPASS_OFFSET_Y - tempZ) / 10.0f, 0.0f, MTXMODE_NEW);
        Matrix_scale(VREG(9) / 100.0f, VREG(9) / 100.0f, VREG(9) / 100.0f, MTXMODE_APPLY);
        Matrix_rotateX(VREG(52) / 10.0f, MTXMODE_APPLY);
        Matrix_rotateY(player_angley / 10.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(OVERLAY_DISP++, play->state.gfxCtx, "../z_map_exp.c", 603);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0xFF, 200, 0, 0, 255);
        gSPDisplayList(OVERLAY_DISP++, gCompassArrowDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_map_exp.c", 607);
}

void map_exp_draw(PlayState* play) {
    s32 pad[2];
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s32 mapIndex = z_common_data.mapIndex;

    OPEN_DISPS(play->state.gfxCtx, "../z_map_exp.c", 626);

    if (play->pauseCtx.state <= PAUSE_STATE_INIT) {
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
                if (!R_MINIMAP_DISABLED) {
                    overlay_rectangle_a_prim(play->state.gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0,
                                      TEXEL0, 0, PRIMITIVE, 0);

                    if (CHECK_DUNGEON_ITEM(DUNGEON_MAP, mapIndex)) {
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 255, 255, interfaceCtx->minimapAlpha);

                        gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->mapSegment, G_IM_FMT_I, MAP_I_TEX_WIDTH,
                                               MAP_I_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                               G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                                               G_TX_NOLOD);

                        gSPTextureRectangle(OVERLAY_DISP++, R_DGN_MINIMAP_X << 2, R_DGN_MINIMAP_Y << 2,
                                            (R_DGN_MINIMAP_X + MAP_I_TEX_WIDTH) << 2,
                                            (R_DGN_MINIMAP_Y + MAP_I_TEX_HEIGHT) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10,
                                            1 << 10);
                    }

                    if (CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, mapIndex)) {
                        compass_draw(play); // Draw icons for the player spawn and current position
                        overlay_rectangle_a_prim(play->state.gfxCtx);
                        MapMarkDisplay(play);
                    }
                }

                if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_L) && !Game_play_demo_mode_check(play)) {
                    PRINTF("Game_play_demo_mode_check=%d\n", Game_play_demo_mode_check(play));
                    // clang-format off
                    if (!R_MINIMAP_DISABLED) { Nai_FxFlagEntry(NA_SE_SY_CAMERA_ZOOM_UP, &_dummy_zero_f, 4,
                                                                      &_dummy_one, &_dummy_one,
                                                                      &_dummy_zero_s8);
                    } else {
                        Nai_FxFlagEntry(NA_SE_SY_CAMERA_ZOOM_DOWN, &_dummy_zero_f, 4,
                                               &_dummy_one, &_dummy_one,
                                               &_dummy_zero_s8);
                    }
                    // clang-format on
                    R_MINIMAP_DISABLED ^= 1;
                }

                break;
            case SCENE_HYRULE_FIELD:
            case SCENE_KAKARIKO_VILLAGE:
            case SCENE_GRAVEYARD:
            case SCENE_ZORAS_RIVER:
            case SCENE_KOKIRI_FOREST:
            case SCENE_SACRED_FOREST_MEADOW:
            case SCENE_LAKE_HYLIA:
            case SCENE_ZORAS_DOMAIN:
            case SCENE_ZORAS_FOUNTAIN:
            case SCENE_GERUDO_VALLEY:
            case SCENE_LOST_WOODS:
            case SCENE_DESERT_COLOSSUS:
            case SCENE_GERUDOS_FORTRESS:
            case SCENE_HAUNTED_WASTELAND:
            case SCENE_HYRULE_CASTLE:
            case SCENE_DEATH_MOUNTAIN_TRAIL:
            case SCENE_DEATH_MOUNTAIN_CRATER:
            case SCENE_GORON_CITY:
            case SCENE_LON_LON_RANCH:
            case SCENE_OUTSIDE_GANONS_CASTLE:
                if (!R_MINIMAP_DISABLED) {
                    overlay_rectangle_a_prim(play->state.gfxCtx);

                    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_MINIMAP_COLOR(0), R_MINIMAP_COLOR(1), R_MINIMAP_COLOR(2),
                                    interfaceCtx->minimapAlpha);

                    gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->mapSegment, G_IM_FMT_IA,
                                           map_exp_data_tbl_p->owMinimapWidth[mapIndex], map_exp_data_tbl_p->owMinimapHeight[mapIndex], 0,
                                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                           G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                    gSPTextureRectangle(OVERLAY_DISP++, R_OW_MINIMAP_X << 2, R_OW_MINIMAP_Y << 2,
                                        (R_OW_MINIMAP_X + map_exp_data_tbl_p->owMinimapWidth[mapIndex]) << 2,
                                        (R_OW_MINIMAP_Y + map_exp_data_tbl_p->owMinimapHeight[mapIndex]) << 2, G_TX_RENDERTILE, 0,
                                        0, 1 << 10, 1 << 10);

                    if (((play->sceneId != SCENE_KAKARIKO_VILLAGE) && (play->sceneId != SCENE_KOKIRI_FOREST) &&
                         (play->sceneId != SCENE_ZORAS_FOUNTAIN)) ||
                        (LINK_AGE_IN_YEARS != YEARS_ADULT)) {
                        if ((map_exp_data_tbl_p->owEntranceFlag[real_scene_data_ID] == 0xFFFF) ||
                            ((map_exp_data_tbl_p->owEntranceFlag[real_scene_data_ID] != 0xFFFF) &&
                             (z_common_data.save.info.infTable[INFTABLE_INDEX_1AX] &
                              check_bit[map_exp_data_tbl_p->owEntranceFlag[mapIndex]]))) {

                            gDPLoadTextureBlock(OVERLAY_DISP++, gMapDungeonEntranceIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b,
                                                8, 8, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                            gSPTextureRectangle(OVERLAY_DISP++,
                                                map_exp_data_tbl_p->owEntranceIconPosX[real_scene_data_ID] << 2,
                                                map_exp_data_tbl_p->owEntranceIconPosY[real_scene_data_ID] << 2,
                                                (map_exp_data_tbl_p->owEntranceIconPosX[real_scene_data_ID] + 8) << 2,
                                                (map_exp_data_tbl_p->owEntranceIconPosY[real_scene_data_ID] + 8) << 2,
                                                G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                        }
                    }

                    if ((play->sceneId == SCENE_ZORAS_FOUNTAIN) &&
                        (z_common_data.save.info.infTable[INFTABLE_INDEX_1AX] & check_bit[INFTABLE_1A9_SHIFT])) {
                        gDPLoadTextureBlock(OVERLAY_DISP++, gMapDungeonEntranceIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 8,
                                            8, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

                        gSPTextureRectangle(OVERLAY_DISP++, 270 << 2, 154 << 2, 278 << 2, 162 << 2, G_TX_RENDERTILE, 0,
                                            0, 1 << 10, 1 << 10);
                    }

                    compass_draw(play); // Draw icons for the player spawn and current position
                }

                if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_L) && !Game_play_demo_mode_check(play)) {
                    // clang-format off
                    if (!R_MINIMAP_DISABLED) { Nai_FxFlagEntry(NA_SE_SY_CAMERA_ZOOM_UP, &_dummy_zero_f, 4,
                                                                      &_dummy_one, &_dummy_one,
                                                                      &_dummy_zero_s8);
                    } else {
                        Nai_FxFlagEntry(NA_SE_SY_CAMERA_ZOOM_DOWN, &_dummy_zero_f, 4,
                                               &_dummy_one, &_dummy_one,
                                               &_dummy_zero_s8);
                    }
                    // clang-format on
                    R_MINIMAP_DISABLED ^= 1;
                }

                break;
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_map_exp.c", 782);
}

s16 map_exp_get_map_no(s32 mapIndex, s32 floor) {
    return map_exp_data_tbl_p->floorTexIndexOffset[mapIndex][floor];
}

void map_exp_move(PlayState* play) {
    static s16 ckroom = 99;
    Player* player = GET_PLAYER(play);
    s32 mapIndex = z_common_data.mapIndex;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 floor;
    s16 i;

    if (!IS_PAUSED(&play->pauseCtx)) {
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
                interfaceCtx->mapPalette[30] = 0;
                if (CHECK_DUNGEON_ITEM(DUNGEON_MAP, mapIndex)) {
                    interfaceCtx->mapPalette[31] = 1;
                } else {
                    interfaceCtx->mapPalette[31] = 0;
                }

                for (floor = 0; floor < 8; floor++) {
                    if (player->actor.world.pos.y > map_exp_data_tbl_p->floorCoordY[mapIndex][floor]) {
                        break;
                    }
                }

                z_common_data.save.info.sceneFlags[mapIndex].floors |= check_bit[floor];
                VREG(30) = floor;
                if (R_MAP_TEX_INDEX != (R_MAP_TEX_INDEX_BASE + map_exp_get_map_no(mapIndex, floor))) {
                    R_MAP_TEX_INDEX = R_MAP_TEX_INDEX_BASE + map_exp_get_map_no(mapIndex, floor);
                }

                if (interfaceCtx->mapRoomNum != ckroom) {
                    PRINTF(T("現在階＝%d  現在部屋＝%x  部屋数＝%d\n",
                             "Current floor = %d  Current room = %x  Number of rooms = %d\n"),
                           floor, interfaceCtx->mapRoomNum, map_exp_data_tbl_p->switchEntryCount[mapIndex]);
                    ckroom = interfaceCtx->mapRoomNum;
                }

                for (i = 0; i < map_exp_data_tbl_p->switchEntryCount[mapIndex]; i++) {
                    if ((interfaceCtx->mapRoomNum == map_exp_data_tbl_p->switchFromRoom[mapIndex][i]) &&
                        (floor == map_exp_data_tbl_p->switchFromFloor[mapIndex][i])) {
                        interfaceCtx->mapRoomNum = map_exp_data_tbl_p->switchToRoom[mapIndex][i];
                        PRINTF_COLOR_YELLOW();
                        PRINTF(T("階層切替＝%x\n", "Layer switching = %x\n"), interfaceCtx->mapRoomNum);
                        PRINTF_RST();
                        map_textuer_dma(play, interfaceCtx->mapRoomNum);
                        z_common_data.sunsSongState = SUNSSONG_INACTIVE;
                        player_position_hold(play);
                    }
                }

                VREG(10) = interfaceCtx->mapRoomNum;
                break;
            case SCENE_DEKU_TREE_BOSS:
            case SCENE_DODONGOS_CAVERN_BOSS:
            case SCENE_JABU_JABU_BOSS:
            case SCENE_FOREST_TEMPLE_BOSS:
            case SCENE_FIRE_TEMPLE_BOSS:
            case SCENE_WATER_TEMPLE_BOSS:
            case SCENE_SPIRIT_TEMPLE_BOSS:
            case SCENE_SHADOW_TEMPLE_BOSS:
                VREG(30) = map_exp_data_tbl_p->bossFloor[play->sceneId - SCENE_DEKU_TREE_BOSS];
                R_MAP_TEX_INDEX = R_MAP_TEX_INDEX_BASE +
                                  map_exp_data_tbl_p->floorTexIndexOffset[play->sceneId - SCENE_DEKU_TREE_BOSS][VREG(30)];
                break;
        }
    }
}
