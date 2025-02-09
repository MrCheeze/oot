#include "z_kaleido_scope.h"
#include "assets/textures/parameter_static/parameter_static.h"

// Positions of each input section in the editor
static u16 cursol_pos_data[][2] = {
    { 64, 15 },   { 144, 15 },  { 170, 15 },  { 78, 35 },   { 104, 35 },  { 130, 35 },  { 156, 35 },  { 182, 35 },
    { 208, 35 },  { 78, 50 },   { 104, 50 },  { 130, 50 },  { 156, 50 },  { 182, 50 },  { 208, 50 },  { 78, 65 },
    { 104, 65 },  { 130, 65 },  { 156, 65 },  { 182, 65 },  { 208, 65 },  { 78, 80 },   { 104, 80 },  { 130, 80 },
    { 156, 80 },  { 182, 80 },  { 208, 80 },  { 78, 98 },   { 88, 98 },   { 98, 98 },   { 108, 98 },  { 118, 98 },
    { 128, 98 },  { 138, 98 },  { 148, 98 },  { 158, 98 },  { 168, 98 },  { 178, 98 },  { 188, 98 },  { 198, 98 },
    { 208, 98 },  { 218, 98 },  { 228, 98 },  { 238, 98 },  { 78, 115 },  { 90, 115 },  { 102, 115 }, { 114, 115 },
    { 126, 115 }, { 138, 115 }, { 150, 115 }, { 162, 115 }, { 202, 115 }, { 214, 115 }, { 226, 115 }, { 238, 115 },
    { 78, 132 },  { 90, 132 },  { 102, 132 }, { 114, 132 }, { 126, 132 }, { 138, 132 }, { 150, 132 }, { 162, 132 },
    { 174, 132 }, { 186, 132 }, { 198, 132 }, { 210, 132 }, { 78, 149 },  { 90, 149 },  { 102, 149 }, { 114, 149 },
    { 126, 149 }, { 138, 149 }, { 78, 166 },  { 90, 166 },  { 102, 166 }, { 114, 166 }, { 126, 166 }, { 138, 166 },
    { 150, 166 }, { 162, 166 }, { 174, 166 }, { 186, 166 }, { 198, 166 }, { 210, 166 }, { 210, 149 }, { 222, 149 },
    { 234, 149 }, { 78, 185 },  { 90, 185 },  { 145, 185 }, { 210, 185 },
};

// First section of each row in the editor (starting from the top)
static u16 point_head[] = {
    0x00, 0x03, 0x1B, 0x2C, 0x34, 0x38, 0x44, 0x4A, 0x56, 0x59, 0x5C,
};

// Maximum value of each upgrade type
static u8 no_equip_maxdt[] = {
    3, 3, 3, 2, 2, 3, 3, 3,
};

// Item ID corresponding to each slot, aside from bottles and trade items
static s16 item_no_table[] = {
    ITEM_DEKU_STICK, ITEM_DEKU_NUT,      ITEM_BOMB,       ITEM_BOW,      ITEM_ARROW_FIRE,  ITEM_DINS_FIRE,
    ITEM_SLINGSHOT,  ITEM_OCARINA_FAIRY, ITEM_BOMBCHU,    ITEM_HOOKSHOT, ITEM_ARROW_ICE,   ITEM_FARORES_WIND,
    ITEM_BOOMERANG,  ITEM_LENS_OF_TRUTH, ITEM_MAGIC_BEAN, ITEM_HAMMER,   ITEM_ARROW_LIGHT, ITEM_NAYRUS_LOVE,
};

void menu_draw(Gfx** gfxP) {
    GfxPrint printer;
    s32 pad[2];

    gfxprint_init(&printer);
    gfxprint_open(&printer, *gfxP);

    gfxprint_locate8x8(&printer, 4, 2);
    gfxprint_color(&printer, 255, 60, 0, 255);
    gfxprint_printf(&printer, "%s", GFXP_KATAKANA "ﾙﾋﾟｰ"); // "Rupee"
    gfxprint_locate8x8(&printer, 15, 2);
    gfxprint_printf(&printer, "%s", GFXP_KATAKANA "ﾊｰﾄ"); // "Heart"
    gfxprint_locate8x8(&printer, 26, 3);
    gfxprint_printf(&printer, "%s", "/4");
    gfxprint_locate8x8(&printer, 4, 5);
    gfxprint_printf(&printer, "%s", GFXP_KATAKANA "ｱｲﾃﾑ"); // "Item"
    gfxprint_locate8x8(&printer, 4, 13);
    gfxprint_printf(&printer, "%s", "KEY");
    gfxprint_locate8x8(&printer, 4, 15);
    gfxprint_printf(&printer, "%s", GFXP_HIRAGANA "ｿｳﾋﾞ"); // "Equipment"
    gfxprint_locate8x8(&printer, 23, 14);
    gfxprint_printf(&printer, "%s", GFXP_KATAKANA "ｹﾝ"); // "Sword"
    gfxprint_locate8x8(&printer, 23, 15);
    gfxprint_printf(&printer, "%s", GFXP_KATAKANA "ﾀﾃ"); // "Shield"
    gfxprint_locate8x8(&printer, 4, 17);
    gfxprint_printf(&printer, "%s", "MAP");
    gfxprint_locate8x8(&printer, 4, 19);
    gfxprint_printf(&printer, "%s", GFXP_HIRAGANA "ﾌｳｲﾝ"); // "Seal"
    gfxprint_locate8x8(&printer, 20, 19);
    gfxprint_printf(&printer, "%s", GFXP_HIRAGANA "ｾｲﾚｲｾｷ"); // "Spiritual Stone"
    gfxprint_locate8x8(&printer, 4, 21);
    gfxprint_printf(&printer, "%s", GFXP_KATAKANA "ｵｶﾘﾅ"); // "Ocarina"
    gfxprint_locate8x8(&printer, 4, 24);
    gfxprint_printf(&printer, "%s", GFXP_KATAKANA "ｺﾚｸﾄ"); // "Collect"
    gfxprint_locate8x8(&printer, 14, 24);
    gfxprint_printf(&printer, "%s", GFXP_KATAKANA "ｷﾝｽﾀ"); // "Skulltula"
    gfxprint_locate8x8(&printer, 23, 24);
    gfxprint_printf(&printer, "%s", GFXP_KATAKANA "ｶｹﾗ"); // "Gold Token"
    gfxprint_locate8x8(&printer, 28, 24);
    gfxprint_printf(&printer, "%s", "/4");

    *gfxP = gfxprint_close(&printer);
    gfxprint_cleanup(&printer);
}

void suuji_display(PlayState* play, s32 digit, s32 rectLeft, s32 rectTop) {
    OPEN_DISPS(play->state.gfxCtx, "../z_kaleido_debug.c", 208);

    gDPLoadTextureBlock(POLY_OPA_DISP++, ((u8*)gCounterDigit0Tex + (8 * 16 * digit)), G_IM_FMT_I, G_IM_SIZ_8b, 8, 16, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSPTextureRectangle(POLY_OPA_DISP++, rectLeft << 2, rectTop << 2, (rectLeft + 8) << 2, (rectTop + 16) << 2,
                        G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

    CLOSE_DISPS(play->state.gfxCtx, "../z_kaleido_debug.c", 220);
}

void kaleido_scope_debug_draw(PlayState* play) {
    static s16 c_pos = 0;
    static s16 t_pos = 0;
    static s32 old_key = 0;
    static s32 key_wait = 0;
    PauseContext* pauseCtx = &play->pauseCtx;
    Input* input = &play->state.input[0];
    Gfx* gfx;
    Gfx* gfxRef;
    s16 spD8[4];
    s16 slot;
    s16 i;
    s16 j;
    s16 x;
    s16 y;
    s32 dBtnInput = input->cur.button & (BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT);

    OPEN_DISPS(play->state.gfxCtx, "../z_kaleido_debug.c", 402);

    pauseCtx->stickAdjX = input->rel.stick_x;
    pauseCtx->stickAdjY = input->rel.stick_y;

    rectangle_a_prim(play->state.gfxCtx);

    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, 220);
    gDPFillRectangle(POLY_OPA_DISP++, 24, 12, 298, 228);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);

    gfxRef = POLY_OPA_DISP;
    gfx = gfxopen(gfxRef);
    gSPDisplayList(OVERLAY_DISP++, gfx);

    menu_draw(&gfx);

    gSPEndDisplayList(gfx++);
    gfxclose(gfxRef, gfx);
    POLY_OPA_DISP = gfx;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 0, 0, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    // Current Health Quarter (X / 4)
    suuji_display(play, (z_common_data.save.info.playerData.health % 0x10) / 4, 194, 15);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

    // Rupees
    spD8[0] = spD8[1] = spD8[2] = 0;
    spD8[3] = z_common_data.save.info.playerData.rupees;
    while (spD8[3] >= 1000) {
        spD8[0]++;
        spD8[3] -= 1000;
    }

    while (spD8[3] >= 100) {
        spD8[1]++;
        spD8[3] -= 100;
    }

    while (spD8[3] >= 10) {
        spD8[2]++;
        spD8[3] -= 10;
    }

    for (i = 0, x = 68; i < 4; i++, x += 10) {
        suuji_display(play, spD8[i], x, 15);
    }

    // Health capacity
    spD8[2] = 0;
    spD8[3] = z_common_data.save.info.playerData.healthCapacity / 0x10;
    while (spD8[3] >= 10) {
        spD8[2]++;
        spD8[3] -= 10;
    }

    suuji_display(play, spD8[2], 146, 15);
    suuji_display(play, spD8[3], 156, 15);

    // Health
    spD8[2] = 0;
    spD8[3] = z_common_data.save.info.playerData.health / 0x10;
    while (spD8[3] >= 10) {
        spD8[2]++;
        spD8[3] -= 10;
    }

    suuji_display(play, spD8[2], 172, 15);
    suuji_display(play, spD8[3], 182, 15);

    // Inventory
    for (slot = 0, i = 0, y = 35; i < 4; i++, y += 15) {
        for (j = 0, x = 78; j < 6; j++, slot++, x += 26) {
            spD8[2] = 0;

            if ((slot <= SLOT_BOW) || (slot == SLOT_SLINGSHOT) || (slot == SLOT_BOMBCHU) || (slot == SLOT_MAGIC_BEAN)) {
                spD8[3] = AMMO(number_item[slot]);
            } else if (slot == SLOT_OCARINA) {
                spD8[3] = z_common_data.save.info.inventory.items[slot];
            } else {
                spD8[3] = z_common_data.save.info.inventory.items[slot];
            }

            if (spD8[3] != ITEM_NONE) {
                while (spD8[3] >= 10) {
                    spD8[2]++;
                    spD8[3] -= 10;
                }
            } else {
                spD8[2] = spD8[3] = 0;
            }

            suuji_display(play, spD8[2], x, y);
            suuji_display(play, spD8[3], x + 10, y);
        }
    }

    // Keys
    for (spD8[1] = 78, i = 0; i < 17; i++) {
        spD8[2] = 0;

        if ((spD8[3] = z_common_data.save.info.inventory.dungeonKeys[i]) >= 0) {
            while (spD8[3] >= 10) {
                spD8[2]++;
                spD8[3] -= 10;
            }
        } else {
            spD8[2] = spD8[3] = 0;
        }

        suuji_display(play, spD8[3], spD8[1], 98);
        spD8[1] += 10;
    }

    // Upgrades
    for (spD8[1] = 78, i = 0; i < 8; i++, spD8[1] += 12) {
        suuji_display(play, CUR_UPG_VALUE(i), spD8[1], 115);
    }

    // Equipment
    for (spD8[1] = 202, i = 0; i < 4; i++, spD8[1] += 12) {
        suuji_display(play, ALL_EQUIP_VALUE(i), spD8[1], 115);
    }

    // Dungeon Items
    for (spD8[1] = 78, i = 0; i < 12; i++, spD8[1] += 12) {
        spD8[2] = z_common_data.save.info.inventory.dungeonItems[i] & bit_check_data[0];
        suuji_display(play, spD8[2], spD8[1], 132);
    }

    // Medallions
    for (spD8[1] = 78, i = 0; i < 6; i++, spD8[1] += 12) {
        spD8[2] = 0;
        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST + i)) {
            spD8[2] = 1;
        }
        suuji_display(play, spD8[2], spD8[1], 149);
    }

    // Spiritual Stones
    for (spD8[1] = 210, i = 0; i < 3; i++, spD8[1] += 12) {
        spD8[2] = 0;
        if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD + i)) {
            spD8[2] = 1;
        }
        suuji_display(play, spD8[2], spD8[1], 149);
    }

    // Songs
    for (spD8[1] = 78, i = 0; i < 12; i++, spD8[1] += 12) {
        spD8[2] = 0;
        if (CHECK_QUEST_ITEM(QUEST_SONG_MINUET + i)) {
            spD8[2] = 1;
        }
        suuji_display(play, spD8[2], spD8[1], 166);
    }

    // Other Quest Items
    for (spD8[1] = 78, i = 0; i < 2; i++, spD8[1] += 12) {
        spD8[2] = 0;
        if (CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY + i)) {
            spD8[2] = 1;
        }
        suuji_display(play, spD8[2], spD8[1], 185);
    }

    // GS Tokens
    spD8[3] = z_common_data.save.info.inventory.gsTokens;
    spD8[1] = 0;
    spD8[2] = 0;
    while (spD8[3] >= 100) {
        spD8[1]++;
        spD8[3] -= 100;
    }

    while (spD8[3] >= 10) {
        spD8[2]++;
        spD8[3] -= 10;
    }

    suuji_display(play, spD8[1], 145, 185);
    suuji_display(play, spD8[2], 155, 185);
    suuji_display(play, spD8[3], 165, 185);

    // Heart Pieces (X / 4)
    suuji_display(
        play, ((z_common_data.save.info.inventory.questItems & 0xF0000000) & 0xF0000000) >> QUEST_HEART_PIECE_COUNT, 210,
        185);

    // Handles navigating the menu to different sections with the D-Pad
    // When the same direction is held, registers the input periodically based on a timer
    if (dBtnInput == old_key) {
        key_wait--;
        if (key_wait < 0) {
            key_wait = 1;
        } else {
            dBtnInput ^= old_key;
        }
    } else {
        old_key = dBtnInput;
        key_wait = 16;
    }

    if (CHECK_BTN_ANY(dBtnInput, BTN_DDOWN)) {
        if ((u32)++t_pos > 10) {
            t_pos = 0;
        }
        c_pos = point_head[t_pos];
    } else if (CHECK_BTN_ANY(dBtnInput, BTN_DUP)) {
        if (--t_pos < 0) {
            t_pos = 22;
        }
        c_pos = point_head[t_pos];
    } else if (CHECK_BTN_ANY(dBtnInput, BTN_DLEFT)) {
        if (--c_pos < 0) {
            c_pos = 0x5C;
        }
    } else if (CHECK_BTN_ANY(dBtnInput, BTN_DRIGHT)) {
        if (++c_pos > 0x5C) {
            c_pos = 0;
        }
    }

    // Handles the logic to change values based on the selected section
    switch (c_pos) {
        case 0:
            if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                z_common_data.save.info.playerData.rupees -= 100;
                if (z_common_data.save.info.playerData.rupees < 0) {
                    z_common_data.save.info.playerData.rupees = 0;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                z_common_data.save.info.playerData.rupees += 100;
                if (z_common_data.save.info.playerData.rupees >= 9999) {
                    z_common_data.save.info.playerData.rupees = 9999;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                z_common_data.save.info.playerData.rupees--;
                if (z_common_data.save.info.playerData.rupees < 0) {
                    z_common_data.save.info.playerData.rupees = 0;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                z_common_data.save.info.playerData.rupees++;
                if (z_common_data.save.info.playerData.rupees >= 9999) {
                    z_common_data.save.info.playerData.rupees = 9999;
                }
            }
            break;

        case 1:
            if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                z_common_data.save.info.playerData.healthCapacity -= 0x10;
                if (z_common_data.save.info.playerData.healthCapacity < 0x30) {
                    z_common_data.save.info.playerData.healthCapacity = 0x30;
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                       CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                z_common_data.save.info.playerData.healthCapacity += 0x10;
                if (z_common_data.save.info.playerData.healthCapacity >= 0x140) {
                    z_common_data.save.info.playerData.healthCapacity = 0x140;
                }
            }
            break;

        case 2:
            if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                life_meter_play(play, -4);
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                life_meter_play(play, 4);
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                life_meter_play(play, -0x10);
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                life_meter_play(play, 0x10);
            }
            break;

        case 0x5C:
            if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                if ((((z_common_data.save.info.inventory.questItems & 0xF0000000) & 0xF0000000) >>
                     QUEST_HEART_PIECE_COUNT) != 0) {
                    z_common_data.save.info.inventory.questItems -= (1 << QUEST_HEART_PIECE_COUNT);
                }
            } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                       CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                if ((z_common_data.save.info.inventory.questItems & 0xF0000000) <= (4 << QUEST_HEART_PIECE_COUNT)) {
                    z_common_data.save.info.inventory.questItems += (1 << QUEST_HEART_PIECE_COUNT);
                }
            }
            break;

        default:
            if (c_pos < 0x1B) {
                i = c_pos - 3;
                if ((i <= SLOT_BOW) || (i == SLOT_SLINGSHOT) || (i == SLOT_BOMBCHU) || (i == SLOT_MAGIC_BEAN)) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        item_clear_setting(number_item[i], SLOT(number_item[i]));
                        AMMO(number_item[i]) = 0;
                    }

                    if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (i != INV_CONTENT(number_item[i])) {
                            INV_CONTENT(number_item[i]) = number_item[i];
                        }
                        AMMO(number_item[i])++;
                        if (AMMO(number_item[i]) > 99) {
                            AMMO(number_item[i]) = 99;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        AMMO(number_item[i])--;
                        if (AMMO(number_item[i]) < 0) {
                            AMMO(number_item[i]) = 0;
                        }
                    }
                } else if (i == SLOT_OCARINA) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        item_clear_setting(ITEM_OCARINA_FAIRY, SLOT(ITEM_OCARINA_FAIRY));
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                            z_common_data.save.info.inventory.items[i] = ITEM_OCARINA_FAIRY;
                        } else if ((z_common_data.save.info.inventory.items[i] >= ITEM_OCARINA_FAIRY) &&
                                   (z_common_data.save.info.inventory.items[i] < ITEM_OCARINA_OF_TIME)) {
                            z_common_data.save.info.inventory.items[i]++;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                            z_common_data.save.info.inventory.items[i] = ITEM_OCARINA_OF_TIME;
                        } else if ((z_common_data.save.info.inventory.items[i] > ITEM_OCARINA_FAIRY) &&
                                   (z_common_data.save.info.inventory.items[i] <= ITEM_OCARINA_OF_TIME)) {
                            z_common_data.save.info.inventory.items[i]--;
                        }
                    }
                } else if (i == SLOT_HOOKSHOT) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        item_clear_setting(ITEM_HOOKSHOT, SLOT(ITEM_HOOKSHOT));
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                            z_common_data.save.info.inventory.items[i] = ITEM_HOOKSHOT;
                        } else if ((z_common_data.save.info.inventory.items[i] >= ITEM_HOOKSHOT) &&
                                   (z_common_data.save.info.inventory.items[i] < ITEM_LONGSHOT)) {
                            z_common_data.save.info.inventory.items[i]++;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                            z_common_data.save.info.inventory.items[i] = ITEM_LONGSHOT;
                        } else if ((z_common_data.save.info.inventory.items[i] > ITEM_HOOKSHOT) &&
                                   (z_common_data.save.info.inventory.items[i] <= ITEM_LONGSHOT)) {
                            z_common_data.save.info.inventory.items[i]--;
                        }
                    }
                } else if (i == SLOT_TRADE_ADULT) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        item_clear_setting(ITEM_POCKET_EGG, SLOT(ITEM_POCKET_EGG));
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                            z_common_data.save.info.inventory.items[i] = ITEM_POCKET_EGG;
                        } else if ((z_common_data.save.info.inventory.items[i] >= ITEM_POCKET_EGG) &&
                                   (z_common_data.save.info.inventory.items[i] < ITEM_CLAIM_CHECK)) {
                            z_common_data.save.info.inventory.items[i]++;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                            z_common_data.save.info.inventory.items[i] = ITEM_CLAIM_CHECK;
                        } else if ((z_common_data.save.info.inventory.items[i] > ITEM_POCKET_EGG) &&
                                   (z_common_data.save.info.inventory.items[i] <= ITEM_CLAIM_CHECK)) {
                            z_common_data.save.info.inventory.items[i]--;
                        }
                    }
                } else if (i == SLOT_TRADE_CHILD) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        item_clear_setting(ITEM_WEIRD_EGG, SLOT(ITEM_WEIRD_EGG));
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                            z_common_data.save.info.inventory.items[i] = ITEM_WEIRD_EGG;
                        } else if ((z_common_data.save.info.inventory.items[i] >= ITEM_WEIRD_EGG) &&
                                   (z_common_data.save.info.inventory.items[i] < ITEM_SOLD_OUT)) {
                            z_common_data.save.info.inventory.items[i]++;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                            z_common_data.save.info.inventory.items[i] = ITEM_SOLD_OUT;
                        } else if ((z_common_data.save.info.inventory.items[i] > ITEM_WEIRD_EGG) &&
                                   (z_common_data.save.info.inventory.items[i] <= ITEM_SOLD_OUT)) {
                            z_common_data.save.info.inventory.items[i]--;
                        }
                    }
                } else if ((i >= SLOT_BOTTLE_1) && (i <= SLOT_BOTTLE_4)) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                        item_clear_setting(ITEM_BOTTLE_EMPTY + i - SLOT_BOTTLE_1,
                                             SLOT(ITEM_BOTTLE_EMPTY) + i - SLOT_BOTTLE_1);
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                            z_common_data.save.info.inventory.items[i] = ITEM_BOTTLE_EMPTY;
                        } else if ((z_common_data.save.info.inventory.items[i] >= ITEM_BOTTLE_EMPTY) &&
                                   (z_common_data.save.info.inventory.items[i] <= ITEM_BOTTLE_MILK_HALF)) {
                            z_common_data.save.info.inventory.items[i]++;
                        }
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                            z_common_data.save.info.inventory.items[i] = ITEM_BOTTLE_POE;
                        } else if ((z_common_data.save.info.inventory.items[i] >= ITEM_BOTTLE_POTION_RED) &&
                                   (z_common_data.save.info.inventory.items[i] <= ITEM_BOTTLE_POE)) {
                            z_common_data.save.info.inventory.items[i]--;
                        }
                    }
                } else if (i < 0x1B) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT) ||
                        CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                        CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        if (i == SLOT_TRADE_ADULT) {
                            if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                                z_common_data.save.info.inventory.items[i] = ITEM_MAGIC_BEAN;
                            } else {
                                item_clear_setting(ITEM_MAGIC_BEAN, SLOT(ITEM_MAGIC_BEAN));
                            }
                        } else {
                            j = item_no_table[i];
                            PRINTF("i=%d  j=%d\n", i, j);
                            if (z_common_data.save.info.inventory.items[i] == ITEM_NONE) {
                                z_common_data.save.info.inventory.items[i] = j;
                            } else {
                                item_clear_setting(j, i);
                            }
                        }
                    }
                }
            } else if (c_pos < 0x2C) {
                if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    i = c_pos - 0x1B;
                    z_common_data.save.info.inventory.dungeonKeys[i]--;
                    if (z_common_data.save.info.inventory.dungeonKeys[i] < 0) {
                        z_common_data.save.info.inventory.dungeonKeys[i] = -1;
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                           CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    i = c_pos - 0x1B;
                    if (z_common_data.save.info.inventory.dungeonKeys[i] < 0) {
                        z_common_data.save.info.inventory.dungeonKeys[i] = 1;
                    } else {
                        z_common_data.save.info.inventory.dungeonKeys[i]++;
                        if (z_common_data.save.info.inventory.dungeonKeys[i] >= 9) {
                            z_common_data.save.info.inventory.dungeonKeys[i] = 9;
                        }
                    }
                }
            } else {
                if (c_pos < 0x38) {
                    i = c_pos - 0x2C;
                    if ((c_pos >= 0x2C) && (c_pos < 0x34)) {
                        if (CHECK_BTN_ALL(input->press.button, BTN_CUP) ||
                            CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                            if (CUR_UPG_VALUE(i) != 0) {
                                Set_Non_Equip_Register(i, CUR_UPG_VALUE(i) - 1);
                            }
                        } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                                   CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                            if (CUR_UPG_VALUE(i) < no_equip_maxdt[i]) {
                                Set_Non_Equip_Register(i, CUR_UPG_VALUE(i) + 1);
                            }
                        }
                    } else {
                        i = c_pos - 0x34; // 0 <= i < 4
                        if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                            z_common_data.save.info.inventory.equipment ^= OWNED_EQUIP_FLAG_ALT(i, 0);
                        }
                        if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                            z_common_data.save.info.inventory.equipment ^= OWNED_EQUIP_FLAG_ALT(i, 1);
                        }
                        if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                            z_common_data.save.info.inventory.equipment ^= OWNED_EQUIP_FLAG_ALT(i, 2);
                        }
                        if (CHECK_BTN_ALL(input->press.button, BTN_CUP)) {
                            z_common_data.save.info.inventory.equipment ^= OWNED_EQUIP_FLAG_ALT(i, 3);
                        }
                    }
                } else if (c_pos < 0x44) {
                    i = c_pos - 0x38;
                    if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        z_common_data.save.info.inventory.dungeonItems[i] ^= 4;
                    }
                    if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN)) {
                        z_common_data.save.info.inventory.dungeonItems[i] ^= 2;
                    }
                    if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        z_common_data.save.info.inventory.dungeonItems[i] ^= 1;
                    }
                } else if (c_pos == 0x5B) {
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        z_common_data.save.info.inventory.gsTokens++;
#if PLATFORM_N64
                        if (z_common_data.save.info.inventory.gsTokens >= 100) {
                            z_common_data.save.info.inventory.gsTokens = 100;
                        }
#endif
                    } else if (CHECK_BTN_ALL(input->press.button, BTN_CDOWN) ||
                               CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                        z_common_data.save.info.inventory.gsTokens--;
                        if (z_common_data.save.info.inventory.gsTokens <= 0) {
                            z_common_data.save.info.inventory.gsTokens = 0;
                        }
                    }
                } else if (c_pos < 0x5C) {
                    i = c_pos - 0x44;
                    if (CHECK_BTN_ALL(input->press.button, BTN_CUP) || CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                        z_common_data.save.info.inventory.questItems ^= check_bit[i];
                    }
                }
            }
            break;
    }

    // Draws a highlight on the selected section
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 200, 120);

    if (c_pos == 0) {
        gDPFillRectangle(POLY_OPA_DISP++, cursol_pos_data[c_pos][0], cursol_pos_data[c_pos][1],
                         cursol_pos_data[c_pos][0] + 45, cursol_pos_data[c_pos][1] + 16);
    } else if ((c_pos >= 0x1B) || (c_pos == 0x5B)) {
        gDPFillRectangle(POLY_OPA_DISP++, cursol_pos_data[c_pos][0] - 2, cursol_pos_data[c_pos][1],
                         cursol_pos_data[c_pos][0] + 14, cursol_pos_data[c_pos][1] + 16);
    } else {
        gDPFillRectangle(POLY_OPA_DISP++, cursol_pos_data[c_pos][0] - 4, cursol_pos_data[c_pos][1],
                         cursol_pos_data[c_pos][0] + 24, cursol_pos_data[c_pos][1] + 16);
    }

    // Handles exiting the inventory editor with the L button
    // The editor is opened with `debugState` set to 1, and becomes closable after a frame once `debugState` is set to 2
    if (pauseCtx->debugState == 1) {
        pauseCtx->debugState = 2;
    } else if ((pauseCtx->debugState == 2) && CHECK_BTN_ALL(input->press.button, BTN_L)) {
        pauseCtx->debugState = 0;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_kaleido_debug.c", 861);
}
