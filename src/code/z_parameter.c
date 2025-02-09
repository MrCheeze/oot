#include "global.h"
#include "terminal.h"
#include "versions.h"
#include "z64horse.h"
#include "assets/textures/parameter_static/parameter_static.h"
#include "assets/textures/do_action_static/do_action_static.h"
#include "assets/textures/icon_item_static/icon_item_static.h"

#include "yama.inc.c"

static s16 yabu_sound = 0;
static u16 yabu_total[] = { 0, 0, 0, 0 };

static u16 navi_pt = 0;
static u16 navi_ct = 0;

s16 Warashibe_item[] = { ITEM_ODD_MUSHROOM, ITEM_EYEBALL_FROG, ITEM_EYE_DROPS };
s16 Warashibe_before_item[] = { ITEM_COJIRO, ITEM_PRESCRIPTION, ITEM_PRESCRIPTION };

static s16 mgr = 255;
static s16 mgg = 255;
static s16 mgb = 255;

static s16 shift_item_no[] = {
    ITEM_DEKU_STICK, // ITEM_DEKU_STICKS_5
    ITEM_DEKU_STICK, // ITEM_DEKU_STICKS_10
    ITEM_DEKU_NUT,   // ITEM_DEKU_NUTS_5
    ITEM_DEKU_NUT,   // ITEM_DEKU_NUTS_10
    ITEM_BOMB,       // ITEM_BOMBS_5
    ITEM_BOMB,       // ITEM_BOMBS_10
    ITEM_BOMB,       // ITEM_BOMBS_20
    ITEM_BOMB,       // ITEM_BOMBS_30
    ITEM_BOW,        // ITEM_ARROWS_5
    ITEM_BOW,        // ITEM_ARROWS_10
    ITEM_BOW,        // ITEM_ARROWS_30
    ITEM_DEKU_SEEDS, // ITEM_DEKU_SEEDS_30
    ITEM_BOMBCHU,    // ITEM_BOMBCHUS_5
    ITEM_BOMBCHU,    // ITEM_BOMBCHUS_20
    ITEM_DEKU_STICK, // ITEM_DEKU_STICK_UPGRADE_20
    ITEM_DEKU_STICK, // ITEM_DEKU_STICK_UPGRADE_30
    ITEM_DEKU_NUT,   // ITEM_DEKU_NUT_UPGRADE_30
    ITEM_DEKU_NUT,   // ITEM_DEKU_NUT_UPGRADE_40
};

static s16 player_condition = PLAYER_ENV_HAZARD_NONE;
static s16 player_diving = false;

static Gfx fbdemo_fade_gfx_init[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_SHADING_SMOOTH | G_LOD),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
    gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
    gsSPEndDisplayList(),
};

// original name: "alpha_change"
void alpha_change(u16 hudVisibilityMode) {
    if (hudVisibilityMode != z_common_data.hudVisibilityMode) {
        PRINTF("ＡＬＰＨＡーＴＹＰＥ＝%d  LAST_TIME_TYPE=%d\n", hudVisibilityMode, z_common_data.prevHudVisibilityMode);
        z_common_data.hudVisibilityMode = z_common_data.nextHudVisibilityMode = hudVisibilityMode;
        z_common_data.hudVisibilityModeTimer = 1;
    }
}

/**
 * Raise button alphas on the HUD to the requested value
 * Apply a set value of 70 to disabled buttons
 */
void c_alpha_set_1(PlayState* play, s16 risingAlpha) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (z_common_data.buttonStatus[0] == BTN_DISABLED) {
        if (interfaceCtx->bAlpha != 70) {
            interfaceCtx->bAlpha = 70;
        }
    } else {
        if (interfaceCtx->bAlpha != 255) {
            interfaceCtx->bAlpha = risingAlpha;
        }
    }

    if (z_common_data.buttonStatus[1] == BTN_DISABLED) {
        if (interfaceCtx->cLeftAlpha != 70) {
            interfaceCtx->cLeftAlpha = 70;
        }
    } else {
        if (interfaceCtx->cLeftAlpha != 255) {
            interfaceCtx->cLeftAlpha = risingAlpha;
        }
    }

    if (z_common_data.buttonStatus[2] == BTN_DISABLED) {
        if (interfaceCtx->cDownAlpha != 70) {
            interfaceCtx->cDownAlpha = 70;
        }
    } else {
        if (interfaceCtx->cDownAlpha != 255) {
            interfaceCtx->cDownAlpha = risingAlpha;
        }
    }

    if (z_common_data.buttonStatus[3] == BTN_DISABLED) {
        if (interfaceCtx->cRightAlpha != 70) {
            interfaceCtx->cRightAlpha = 70;
        }
    } else {
        if (interfaceCtx->cRightAlpha != 255) {
            interfaceCtx->cRightAlpha = risingAlpha;
        }
    }

    if (z_common_data.buttonStatus[4] == BTN_DISABLED) {
        if (interfaceCtx->aAlpha != 70) {
            interfaceCtx->aAlpha = 70;
        }
    } else {
        if (interfaceCtx->aAlpha != 255) {
            interfaceCtx->aAlpha = risingAlpha;
        }
    }
}

/**
 * Lower button alphas on the HUD to the requested value
 * if forceRisingButtonAlphas is set, then instead raise button alphas
 */
void c_alpha_set_0(PlayState* play, s16 dimmingAlpha, s16 risingAlpha) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (z_common_data.forceRisingButtonAlphas) {
        c_alpha_set_1(play, risingAlpha);
        return;
    }

    if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
        interfaceCtx->bAlpha = dimmingAlpha;
    }

    if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
        interfaceCtx->aAlpha = dimmingAlpha;
    }

    if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
        interfaceCtx->cLeftAlpha = dimmingAlpha;
    }

    if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
        interfaceCtx->cDownAlpha = dimmingAlpha;
    }

    if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
        interfaceCtx->cRightAlpha = dimmingAlpha;
    }
}

void alpha_setting0(PlayState* play, s16 dimmingAlpha) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 risingAlpha = 255 - dimmingAlpha;

    switch (z_common_data.nextHudVisibilityMode) {
        case HUD_VISIBILITY_NOTHING:
        case HUD_VISIBILITY_NOTHING_ALT:
        case HUD_VISIBILITY_B:
            PRINTF("a_alpha=%d, c_alpha=%d   →   ", interfaceCtx->aAlpha, interfaceCtx->cLeftAlpha);

            if (z_common_data.nextHudVisibilityMode == HUD_VISIBILITY_B) {
                if (interfaceCtx->bAlpha != 255) {
                    interfaceCtx->bAlpha = risingAlpha;
                }
            } else {
                if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
                    interfaceCtx->bAlpha = dimmingAlpha;
                }
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            PRINTF("a_alpha=%d, c_alpha=%d\n", interfaceCtx->aAlpha, interfaceCtx->cLeftAlpha);

            break;

        case HUD_VISIBILITY_HEARTS_FORCE:
            // aAlpha is immediately overwritten in c_alpha_set_0
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            c_alpha_set_0(play, dimmingAlpha, risingAlpha);

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_A:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
                interfaceCtx->bAlpha = dimmingAlpha;
            }

            // aAlpha is immediately overwritten below
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE:
            c_alpha_set_0(play, dimmingAlpha, risingAlpha);

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            // aAlpha overwrites the value set in c_alpha_set_0
            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP_FORCE:
            c_alpha_set_0(play, dimmingAlpha, risingAlpha);

            // aAlpha overwrites the value set in c_alpha_set_0
            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

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
                    if (interfaceCtx->minimapAlpha < 170) {
                        interfaceCtx->minimapAlpha = risingAlpha;
                    } else {
                        interfaceCtx->minimapAlpha = 170;
                    }
                    break;

                default:
                    if (interfaceCtx->minimapAlpha != 255) {
                        interfaceCtx->minimapAlpha = risingAlpha;
                    }
                    break;
            }
            break;

        case HUD_VISIBILITY_ALL_NO_MINIMAP_BY_BTN_STATUS:
            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            c_alpha_set_1(play, risingAlpha);

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_HEARTS_MAGIC:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
                interfaceCtx->bAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_B_ALT:
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if (interfaceCtx->bAlpha != 255) {
                interfaceCtx->bAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_HEARTS:
            if ((interfaceCtx->bAlpha != 0) && (interfaceCtx->bAlpha > dimmingAlpha)) {
                interfaceCtx->bAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            break;

        case HUD_VISIBILITY_A_B_MINIMAP:
            if (interfaceCtx->aAlpha != 255) {
                interfaceCtx->aAlpha = risingAlpha;
            }

            if (interfaceCtx->bAlpha != 255) {
                interfaceCtx->bAlpha = risingAlpha;
            }

            if (interfaceCtx->minimapAlpha != 255) {
                interfaceCtx->minimapAlpha = risingAlpha;
            }

            if ((interfaceCtx->cLeftAlpha != 0) && (interfaceCtx->cLeftAlpha > dimmingAlpha)) {
                interfaceCtx->cLeftAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cDownAlpha != 0) && (interfaceCtx->cDownAlpha > dimmingAlpha)) {
                interfaceCtx->cDownAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->cRightAlpha != 0) && (interfaceCtx->cRightAlpha > dimmingAlpha)) {
                interfaceCtx->cRightAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->magicAlpha != 0) && (interfaceCtx->magicAlpha > dimmingAlpha)) {
                interfaceCtx->magicAlpha = dimmingAlpha;
            }

            if ((interfaceCtx->healthAlpha != 0) && (interfaceCtx->healthAlpha > dimmingAlpha)) {
                interfaceCtx->healthAlpha = dimmingAlpha;
            }

            break;

        case HUD_VISIBILITY_HEARTS_MAGIC_FORCE:
            c_alpha_set_0(play, dimmingAlpha, risingAlpha);

            if ((interfaceCtx->minimapAlpha != 0) && (interfaceCtx->minimapAlpha > dimmingAlpha)) {
                interfaceCtx->minimapAlpha = dimmingAlpha;
            }

            // aAlpha overwrites the value set in c_alpha_set_0
            if ((interfaceCtx->aAlpha != 0) && (interfaceCtx->aAlpha > dimmingAlpha)) {
                interfaceCtx->aAlpha = dimmingAlpha;
            }

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

            break;
    }

    if ((play->roomCtx.curRoom.type == ROOM_TYPE_DUNGEON) && (interfaceCtx->minimapAlpha >= 255)) {
        interfaceCtx->minimapAlpha = 255;
    }
}

void parameter_check(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Player* player = GET_PLAYER(play);
    MessageContext* msgCtx = &play->msgCtx;
    s16 i;
    s16 sp28 = false;

    if ((z_common_data.save.cutsceneIndex < 0xFFF0) ||
        ((play->sceneId == SCENE_LON_LON_RANCH) && (z_common_data.save.cutsceneIndex == 0xFFF0))) {
        z_common_data.forceRisingButtonAlphas = false;

        if ((player->stateFlags1 & PLAYER_STATE1_23) || (play->shootingGalleryStatus > 1) ||
            ((play->sceneId == SCENE_BOMBCHU_BOWLING_ALLEY) && Actor_Environment_sw_Check(play, 0x38))) {
            if (z_common_data.save.info.equips.buttonItems[0] != ITEM_NONE) {
                z_common_data.forceRisingButtonAlphas = true;

                if (z_common_data.buttonStatus[0] == BTN_DISABLED) {
                    z_common_data.buttonStatus[0] = z_common_data.buttonStatus[1] = z_common_data.buttonStatus[2] =
                        z_common_data.buttonStatus[3] = BTN_ENABLED;
                }

                if ((z_common_data.save.info.equips.buttonItems[0] != ITEM_SLINGSHOT) &&
                    (z_common_data.save.info.equips.buttonItems[0] != ITEM_BOW) &&
                    (z_common_data.save.info.equips.buttonItems[0] != ITEM_BOMBCHU) &&
                    (z_common_data.save.info.equips.buttonItems[0] != ITEM_NONE)) {
                    z_common_data.buttonStatus[0] = z_common_data.save.info.equips.buttonItems[0];

                    if ((play->sceneId == SCENE_BOMBCHU_BOWLING_ALLEY) && Actor_Environment_sw_Check(play, 0x38)) {
                        z_common_data.save.info.equips.buttonItems[0] = ITEM_BOMBCHU;
                        item_textuer_dma(play, 0);
                    } else {
                        z_common_data.save.info.equips.buttonItems[0] = ITEM_BOW;
                        if (play->shootingGalleryStatus > 1) {
                            if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
                                z_common_data.save.info.equips.buttonItems[0] = ITEM_SLINGSHOT;
                            }

                            item_textuer_dma(play, 0);
                        } else {
                            if (z_common_data.save.info.inventory.items[SLOT_BOW] == ITEM_NONE) {
                                z_common_data.save.info.equips.buttonItems[0] = ITEM_NONE;
                            } else {
                                item_textuer_dma(play, 0);
                            }
                        }
                    }

                    z_common_data.buttonStatus[1] = z_common_data.buttonStatus[2] = z_common_data.buttonStatus[3] =
                        BTN_DISABLED;
                    alpha_change(HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP_FORCE);
                }

                if (play->transitionMode != TRANS_MODE_OFF) {
                    alpha_change(HUD_VISIBILITY_NOTHING);
                } else if (z_common_data.minigameState == 1) {
                    alpha_change(HUD_VISIBILITY_B);
                } else if (play->shootingGalleryStatus > 1) {
                    alpha_change(HUD_VISIBILITY_B);
                } else if ((play->sceneId == SCENE_BOMBCHU_BOWLING_ALLEY) && Actor_Environment_sw_Check(play, 0x38)) {
                    alpha_change(HUD_VISIBILITY_B);
                } else if (player->stateFlags1 & PLAYER_STATE1_23) {
                    alpha_change(HUD_VISIBILITY_A_B_MINIMAP);
                }
            } else {
                if (player->stateFlags1 & PLAYER_STATE1_23) {
                    alpha_change(HUD_VISIBILITY_A_B_MINIMAP);
                }
            }
        } else if (play->sceneId == SCENE_CHAMBER_OF_THE_SAGES) {
            alpha_change(HUD_VISIBILITY_NOTHING);
        } else if (play->sceneId == SCENE_FISHING_POND) {
            // should likely be set to true
            z_common_data.forceRisingButtonAlphas = 2;
            if (play->interfaceCtx.unk_260 != 0) {
                if (z_common_data.save.info.equips.buttonItems[0] != ITEM_FISHING_POLE) {
                    z_common_data.buttonStatus[0] = z_common_data.save.info.equips.buttonItems[0];
                    z_common_data.save.info.equips.buttonItems[0] = ITEM_FISHING_POLE;
                    z_common_data.hudVisibilityMode = HUD_VISIBILITY_NO_CHANGE;
                    item_textuer_dma(play, 0);
                    alpha_change(HUD_VISIBILITY_A_B_MINIMAP);
                }

                if (z_common_data.hudVisibilityMode != HUD_VISIBILITY_A_B_MINIMAP) {
                    alpha_change(HUD_VISIBILITY_A_B_MINIMAP);
                }
            } else if (z_common_data.save.info.equips.buttonItems[0] == ITEM_FISHING_POLE) {
                z_common_data.save.info.equips.buttonItems[0] = z_common_data.buttonStatus[0];
                z_common_data.hudVisibilityMode = HUD_VISIBILITY_NO_CHANGE;

                if (z_common_data.save.info.equips.buttonItems[0] != ITEM_NONE) {
                    item_textuer_dma(play, 0);
                }

                z_common_data.buttonStatus[0] = z_common_data.buttonStatus[1] = z_common_data.buttonStatus[2] =
                    z_common_data.buttonStatus[3] = BTN_DISABLED;
                alpha_change(HUD_VISIBILITY_ALL);
            } else {
                if (z_common_data.buttonStatus[0] == BTN_ENABLED) {
                    z_common_data.hudVisibilityMode = HUD_VISIBILITY_NO_CHANGE;
                }

                z_common_data.buttonStatus[0] = z_common_data.buttonStatus[1] = z_common_data.buttonStatus[2] =
                    z_common_data.buttonStatus[3] = BTN_DISABLED;
                alpha_change(HUD_VISIBILITY_ALL);
            }
        } else if (msgCtx->msgMode == MSGMODE_NONE) {
            if ((player_condition_check(play) >= PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) &&
                (player_condition_check(play) <= PLAYER_ENV_HAZARD_UNDERWATER_FREE)) {
                if (z_common_data.buttonStatus[0] != BTN_DISABLED) {
                    sp28 = true;
                }

                z_common_data.buttonStatus[0] = BTN_DISABLED;

                for (i = 1; i < 4; i++) {
                    if (player_condition_check(play) == PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) {
                        if ((z_common_data.save.info.equips.buttonItems[i] != ITEM_HOOKSHOT) &&
                            (z_common_data.save.info.equips.buttonItems[i] != ITEM_LONGSHOT)) {
                            if (z_common_data.buttonStatus[i] == BTN_ENABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_DISABLED;
                        } else {
                            if (z_common_data.buttonStatus[i] == BTN_DISABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_ENABLED;
                        }
                    } else {
                        if (z_common_data.buttonStatus[i] == BTN_ENABLED) {
                            sp28 = true;
                        }

                        z_common_data.buttonStatus[i] = BTN_DISABLED;
                    }
                }

                if (sp28) {
                    z_common_data.hudVisibilityMode = HUD_VISIBILITY_NO_CHANGE;
                }

                alpha_change(HUD_VISIBILITY_ALL);
            } else if ((player->stateFlags1 & PLAYER_STATE1_21) || (player->stateFlags2 & PLAYER_STATE2_CRAWLING)) {
                if (z_common_data.buttonStatus[0] != BTN_DISABLED) {
                    z_common_data.buttonStatus[0] = BTN_DISABLED;
                    z_common_data.buttonStatus[1] = BTN_DISABLED;
                    z_common_data.buttonStatus[2] = BTN_DISABLED;
                    z_common_data.buttonStatus[3] = BTN_DISABLED;
                    z_common_data.hudVisibilityMode = HUD_VISIBILITY_NO_CHANGE;
                    alpha_change(HUD_VISIBILITY_ALL);
                }
            } else if (GET_EVENTINF_INGO_RACE_STATE() == INGO_RACE_STATE_HORSE_RENTAL_PERIOD) {
                if (player->stateFlags1 & PLAYER_STATE1_23) {
                    if ((z_common_data.save.info.equips.buttonItems[0] != ITEM_NONE) &&
                        (z_common_data.save.info.equips.buttonItems[0] != ITEM_BOW)) {
                        if (z_common_data.save.info.inventory.items[SLOT_BOW] == ITEM_NONE) {
                            z_common_data.save.info.equips.buttonItems[0] = ITEM_NONE;
                        } else {
                            z_common_data.save.info.equips.buttonItems[0] = ITEM_BOW;
                            sp28 = true;
                        }
                    }
                } else {
                    if ((z_common_data.save.info.equips.buttonItems[0] == ITEM_NONE) ||
                        (z_common_data.save.info.equips.buttonItems[0] == ITEM_BOW)) {

                        if ((z_common_data.save.info.equips.buttonItems[0] != ITEM_SWORD_KOKIRI) &&
                            (z_common_data.save.info.equips.buttonItems[0] != ITEM_SWORD_MASTER) &&
                            (z_common_data.save.info.equips.buttonItems[0] != ITEM_SWORD_BIGGORON) &&
                            (z_common_data.save.info.equips.buttonItems[0] != ITEM_GIANTS_KNIFE)) {
                            z_common_data.save.info.equips.buttonItems[0] = z_common_data.buttonStatus[0];
                        } else {
                            z_common_data.buttonStatus[0] = z_common_data.save.info.equips.buttonItems[0];
                        }
                    }
                    sp28 = true;
                }

                if (sp28) {
                    item_textuer_dma(play, 0);
                    sp28 = false;
                }

                for (i = 1; i < 4; i++) {
                    if ((z_common_data.save.info.equips.buttonItems[i] != ITEM_OCARINA_FAIRY) &&
                        (z_common_data.save.info.equips.buttonItems[i] != ITEM_OCARINA_OF_TIME)) {
                        if (z_common_data.buttonStatus[i] == BTN_ENABLED) {
                            sp28 = true;
                        }

                        z_common_data.buttonStatus[i] = BTN_DISABLED;
                    } else {
                        if (z_common_data.buttonStatus[i] == BTN_DISABLED) {
                            sp28 = true;
                        }

                        z_common_data.buttonStatus[i] = BTN_ENABLED;
                    }
                }

                if (sp28) {
                    z_common_data.hudVisibilityMode = HUD_VISIBILITY_NO_CHANGE;
                }

                alpha_change(HUD_VISIBILITY_ALL);
            } else {
                if (interfaceCtx->restrictions.bButton == 0) {
                    if ((z_common_data.save.info.equips.buttonItems[0] == ITEM_SLINGSHOT) ||
                        (z_common_data.save.info.equips.buttonItems[0] == ITEM_BOW) ||
                        (z_common_data.save.info.equips.buttonItems[0] == ITEM_BOMBCHU) ||
                        (z_common_data.save.info.equips.buttonItems[0] == ITEM_NONE)) {
                        if ((z_common_data.save.info.equips.buttonItems[0] != ITEM_NONE) ||
                            (z_common_data.save.info.infTable[INFTABLE_INDEX_1DX] == 0)) {
                            z_common_data.save.info.equips.buttonItems[0] = z_common_data.buttonStatus[0];
                            sp28 = true;

                            if (z_common_data.save.info.equips.buttonItems[0] != ITEM_NONE) {
                                item_textuer_dma(play, 0);
                            }
                        }
                    } else if ((z_common_data.buttonStatus[0] & 0xFF) == BTN_DISABLED) {
                        sp28 = true;

                        if (((z_common_data.buttonStatus[0] & 0xFF) == BTN_DISABLED) ||
                            ((z_common_data.buttonStatus[0] & 0xFF) == BTN_ENABLED)) {
                            z_common_data.buttonStatus[0] = BTN_ENABLED;
                        } else {
                            z_common_data.save.info.equips.buttonItems[0] = z_common_data.buttonStatus[0] & 0xFF;
                        }
                    }
                } else if (interfaceCtx->restrictions.bButton == 1) {
                    if ((z_common_data.save.info.equips.buttonItems[0] == ITEM_SLINGSHOT) ||
                        (z_common_data.save.info.equips.buttonItems[0] == ITEM_BOW) ||
                        (z_common_data.save.info.equips.buttonItems[0] == ITEM_BOMBCHU) ||
                        (z_common_data.save.info.equips.buttonItems[0] == ITEM_NONE)) {
                        if ((z_common_data.save.info.equips.buttonItems[0] != ITEM_NONE) ||
                            (z_common_data.save.info.infTable[INFTABLE_INDEX_1DX] == 0)) {
                            z_common_data.save.info.equips.buttonItems[0] = z_common_data.buttonStatus[0];
                            sp28 = true;

                            if (z_common_data.save.info.equips.buttonItems[0] != ITEM_NONE) {
                                item_textuer_dma(play, 0);
                            }
                        }
                    } else {
                        if (z_common_data.buttonStatus[0] == BTN_ENABLED) {
                            sp28 = true;
                        }

                        z_common_data.buttonStatus[0] = BTN_DISABLED;
                    }
                }

                if (interfaceCtx->restrictions.bottles != 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] >= ITEM_BOTTLE_EMPTY) &&
                            (z_common_data.save.info.equips.buttonItems[i] <= ITEM_BOTTLE_POE)) {
                            if (z_common_data.buttonStatus[i] == BTN_ENABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_DISABLED;
                        }
                    }
                } else if (interfaceCtx->restrictions.bottles == 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] >= ITEM_BOTTLE_EMPTY) &&
                            (z_common_data.save.info.equips.buttonItems[i] <= ITEM_BOTTLE_POE)) {
                            if (z_common_data.buttonStatus[i] == BTN_DISABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.tradeItems != 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                            (z_common_data.save.info.equips.buttonItems[i] <= ITEM_CLAIM_CHECK)) {
                            if (z_common_data.buttonStatus[i] == BTN_ENABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_DISABLED;
                        }
                    }
                } else if (interfaceCtx->restrictions.tradeItems == 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                            (z_common_data.save.info.equips.buttonItems[i] <= ITEM_CLAIM_CHECK)) {
                            if (z_common_data.buttonStatus[i] == BTN_DISABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.hookshot != 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] == ITEM_HOOKSHOT) ||
                            (z_common_data.save.info.equips.buttonItems[i] == ITEM_LONGSHOT)) {
                            if (z_common_data.buttonStatus[i] == BTN_ENABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_DISABLED;
                        }
                    }
                } else if (interfaceCtx->restrictions.hookshot == 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] == ITEM_HOOKSHOT) ||
                            (z_common_data.save.info.equips.buttonItems[i] == ITEM_LONGSHOT)) {
                            if (z_common_data.buttonStatus[i] == BTN_DISABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.ocarina != 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] == ITEM_OCARINA_FAIRY) ||
                            (z_common_data.save.info.equips.buttonItems[i] == ITEM_OCARINA_OF_TIME)) {
                            if (z_common_data.buttonStatus[i] == BTN_ENABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_DISABLED;
                        }
                    }
                } else if (interfaceCtx->restrictions.ocarina == 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] == ITEM_OCARINA_FAIRY) ||
                            (z_common_data.save.info.equips.buttonItems[i] == ITEM_OCARINA_OF_TIME)) {
                            if (z_common_data.buttonStatus[i] == BTN_DISABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.farores != 0) {
                    for (i = 1; i < 4; i++) {
                        if (z_common_data.save.info.equips.buttonItems[i] == ITEM_FARORES_WIND) {
                            if (z_common_data.buttonStatus[i] == BTN_ENABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_DISABLED;
                            PRINTF("***(i=%d)***  ", i);
                        }
                    }
                } else if (interfaceCtx->restrictions.farores == 0) {
                    for (i = 1; i < 4; i++) {
                        if (z_common_data.save.info.equips.buttonItems[i] == ITEM_FARORES_WIND) {
                            if (z_common_data.buttonStatus[i] == BTN_DISABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.dinsNayrus != 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] == ITEM_DINS_FIRE) ||
                            (z_common_data.save.info.equips.buttonItems[i] == ITEM_NAYRUS_LOVE)) {
                            if (z_common_data.buttonStatus[i] == BTN_ENABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_DISABLED;
                        }
                    }
                } else if (interfaceCtx->restrictions.dinsNayrus == 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] == ITEM_DINS_FIRE) ||
                            (z_common_data.save.info.equips.buttonItems[i] == ITEM_NAYRUS_LOVE)) {
                            if (z_common_data.buttonStatus[i] == BTN_DISABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_ENABLED;
                        }
                    }
                }

                if (interfaceCtx->restrictions.all != 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] != ITEM_OCARINA_FAIRY) &&
                            (z_common_data.save.info.equips.buttonItems[i] != ITEM_OCARINA_OF_TIME) &&
                            !((z_common_data.save.info.equips.buttonItems[i] >= ITEM_BOTTLE_EMPTY) &&
                              (z_common_data.save.info.equips.buttonItems[i] <= ITEM_BOTTLE_POE)) &&
                            !((z_common_data.save.info.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                              (z_common_data.save.info.equips.buttonItems[i] <= ITEM_CLAIM_CHECK))) {
                            if ((play->sceneId != SCENE_TREASURE_BOX_SHOP) ||
                                (z_common_data.save.info.equips.buttonItems[i] != ITEM_LENS_OF_TRUTH)) {
                                if (z_common_data.buttonStatus[i] == BTN_ENABLED) {
                                    sp28 = true;
                                }

                                z_common_data.buttonStatus[i] = BTN_DISABLED;
                            } else {
                                if (z_common_data.buttonStatus[i] == BTN_DISABLED) {
                                    sp28 = true;
                                }

                                z_common_data.buttonStatus[i] = BTN_ENABLED;
                            }
                        }
                    }
                } else if (interfaceCtx->restrictions.all == 0) {
                    for (i = 1; i < 4; i++) {
                        if ((z_common_data.save.info.equips.buttonItems[i] != ITEM_DINS_FIRE) &&
                            (z_common_data.save.info.equips.buttonItems[i] != ITEM_HOOKSHOT) &&
                            (z_common_data.save.info.equips.buttonItems[i] != ITEM_LONGSHOT) &&
                            (z_common_data.save.info.equips.buttonItems[i] != ITEM_FARORES_WIND) &&
                            (z_common_data.save.info.equips.buttonItems[i] != ITEM_NAYRUS_LOVE) &&
                            (z_common_data.save.info.equips.buttonItems[i] != ITEM_OCARINA_FAIRY) &&
                            (z_common_data.save.info.equips.buttonItems[i] != ITEM_OCARINA_OF_TIME) &&
                            !((z_common_data.save.info.equips.buttonItems[i] >= ITEM_BOTTLE_EMPTY) &&
                              (z_common_data.save.info.equips.buttonItems[i] <= ITEM_BOTTLE_POE)) &&
                            !((z_common_data.save.info.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                              (z_common_data.save.info.equips.buttonItems[i] <= ITEM_CLAIM_CHECK))) {
                            if (z_common_data.buttonStatus[i] == BTN_DISABLED) {
                                sp28 = true;
                            }

                            z_common_data.buttonStatus[i] = BTN_ENABLED;
                        }
                    }
                }
            }
        }
    }

    if (sp28) {
        z_common_data.hudVisibilityMode = HUD_VISIBILITY_NO_CHANGE;
        if ((play->transitionTrigger == TRANS_TRIGGER_OFF) && (play->transitionMode == TRANS_MODE_OFF)) {
            alpha_change(HUD_VISIBILITY_ALL);
            PRINTF("????????  alpha_change( 50 );  ?????\n");
        } else {
            PRINTF("game_play->fade_direction || game_play->fbdemo_wipe_modem");
        }
    }
}

void button_status_set(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 i = 0;
    u8 sceneId;
    s32 pad1;
    s32 pad2;
    s32 pad3;

    interfaceCtx->restrictions.all = 0;
    interfaceCtx->restrictions.dinsNayrus = 0;
    interfaceCtx->restrictions.farores = 0;
    interfaceCtx->restrictions.sunsSong = 0;
    interfaceCtx->restrictions.warpSongs = 0;
    interfaceCtx->restrictions.ocarina = 0;
    interfaceCtx->restrictions.hookshot = 0;
    interfaceCtx->restrictions.tradeItems = 0;
    interfaceCtx->restrictions.bottles = 0;
    interfaceCtx->restrictions.aButton = 0;
    interfaceCtx->restrictions.bButton = 0;
    interfaceCtx->restrictions.hGauge = 0;

    PRINTF(
        T("ボタン表示関係データ設定 scene_data_ID=%d\n", "Data settings related to button display scene_data_ID=%d\n"),
        play->sceneId);

    do {
        sceneId = (u8)play->sceneId;
        if (button_data[i].sceneId == sceneId) {
            interfaceCtx->restrictions.hGauge = (button_data[i].flags1 & 0xC0) >> 6;
            interfaceCtx->restrictions.bButton = (button_data[i].flags1 & 0x30) >> 4;
            interfaceCtx->restrictions.aButton = (button_data[i].flags1 & 0x0C) >> 2;
            interfaceCtx->restrictions.bottles = (button_data[i].flags1 & 0x03) >> 0;
            interfaceCtx->restrictions.tradeItems = (button_data[i].flags2 & 0xC0) >> 6;
            interfaceCtx->restrictions.hookshot = (button_data[i].flags2 & 0x30) >> 4;
            interfaceCtx->restrictions.ocarina = (button_data[i].flags2 & 0x0C) >> 2;
            interfaceCtx->restrictions.warpSongs = (button_data[i].flags2 & 0x03) >> 0;
            interfaceCtx->restrictions.sunsSong = (button_data[i].flags3 & 0xC0) >> 6;
            interfaceCtx->restrictions.farores = (button_data[i].flags3 & 0x30) >> 4;
            interfaceCtx->restrictions.dinsNayrus = (button_data[i].flags3 & 0x0C) >> 2;
            interfaceCtx->restrictions.all = (button_data[i].flags3 & 0x03) >> 0;

            PRINTF_COLOR_YELLOW();
            PRINTF("parameter->button_status = %x,%x,%x\n", button_data[i].flags1, button_data[i].flags2,
                   button_data[i].flags3);
            PRINTF("h_gage=%d, b_button=%d, a_button=%d, c_bottle=%d\n", interfaceCtx->restrictions.hGauge,
                   interfaceCtx->restrictions.bButton, interfaceCtx->restrictions.aButton,
                   interfaceCtx->restrictions.bottles);
            PRINTF("c_warasibe=%d, c_hook=%d, c_ocarina=%d, c_warp=%d\n", interfaceCtx->restrictions.tradeItems,
                   interfaceCtx->restrictions.hookshot, interfaceCtx->restrictions.ocarina,
                   interfaceCtx->restrictions.warpSongs);
            PRINTF("c_sunmoon=%d, m_wind=%d, m_magic=%d, another=%d\n", interfaceCtx->restrictions.sunsSong,
                   interfaceCtx->restrictions.farores, interfaceCtx->restrictions.dinsNayrus,
                   interfaceCtx->restrictions.all);
            PRINTF_RST();
            break;
        }
        i++;
    } while (button_data[i].sceneId != 0xFF);
}

Gfx* texture_rectangleIA8(Gfx* displayListHead, void* texture, s16 textureWidth, s16 textureHeight, s16 rectLeft, s16 rectTop,
                    s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy) {
    gDPLoadTextureBlock(displayListHead++, texture, G_IM_FMT_IA, G_IM_SIZ_8b, textureWidth, textureHeight, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSPTextureRectangle(displayListHead++, rectLeft << 2, rectTop << 2, (rectLeft + rectWidth) << 2,
                        (rectTop + rectHeight) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);

    return displayListHead;
}

static Gfx* texture_rectangleI8(Gfx* displayListHead, void* texture, s16 textureWidth, s16 textureHeight, s16 rectLeft, s16 rectTop,
                   s16 rectWidth, s16 rectHeight, u16 dsdx, u16 dtdy) {
    gDPLoadTextureBlock(displayListHead++, texture, G_IM_FMT_I, G_IM_SIZ_8b, textureWidth, textureHeight, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSPTextureRectangle(displayListHead++, rectLeft << 2, rectTop << 2, (rectLeft + rectWidth) << 2,
                        (rectTop + rectHeight) << 2, G_TX_RENDERTILE, 0, 0, dsdx, dtdy);

    return displayListHead;
}

void equip_default_set(void) {
    s16 i;
    u16 shieldEquipValue;

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        for (i = 0; i < 4; i++) {
            if (i != 0) {
                z_common_data.save.info.playerData.childEquips.buttonItems[i] =
                    z_common_data.save.info.equips.buttonItems[i];
            } else {
                z_common_data.save.info.playerData.childEquips.buttonItems[i] = ITEM_SWORD_KOKIRI;
            }

            if (i != 0) {
                z_common_data.save.info.playerData.childEquips.cButtonSlots[i - 1] =
                    z_common_data.save.info.equips.cButtonSlots[i - 1];
            }
        }

        z_common_data.save.info.playerData.childEquips.equipment = z_common_data.save.info.equips.equipment;

        if (z_common_data.save.info.playerData.adultEquips.buttonItems[0] == ITEM_NONE) {
            z_common_data.save.info.equips.buttonItems[0] = ITEM_SWORD_MASTER;

            if (z_common_data.save.info.inventory.items[SLOT_DEKU_NUT] != ITEM_NONE) {
                z_common_data.save.info.equips.buttonItems[1] = ITEM_DEKU_NUT;
                z_common_data.save.info.equips.cButtonSlots[0] = SLOT_DEKU_NUT;
            } else {
                z_common_data.save.info.equips.buttonItems[1] = z_common_data.save.info.equips.cButtonSlots[0] =
                    ITEM_NONE;
            }

            z_common_data.save.info.equips.buttonItems[2] = ITEM_BOMB;
            z_common_data.save.info.equips.buttonItems[3] = z_common_data.save.info.inventory.items[SLOT_OCARINA];
            z_common_data.save.info.equips.cButtonSlots[1] = SLOT_BOMB;
            z_common_data.save.info.equips.cButtonSlots[2] = SLOT_OCARINA;
            z_common_data.save.info.equips.equipment = (EQUIP_VALUE_SWORD_MASTER << (EQUIP_TYPE_SWORD * 4)) |
                                                      (EQUIP_VALUE_SHIELD_HYLIAN << (EQUIP_TYPE_SHIELD * 4)) |
                                                      (EQUIP_VALUE_TUNIC_KOKIRI << (EQUIP_TYPE_TUNIC * 4)) |
                                                      (EQUIP_VALUE_BOOTS_KOKIRI << (EQUIP_TYPE_BOOTS * 4));
        } else {
            for (i = 0; i < 4; i++) {
                z_common_data.save.info.equips.buttonItems[i] =
                    z_common_data.save.info.playerData.adultEquips.buttonItems[i];

                if (i != 0) {
                    z_common_data.save.info.equips.cButtonSlots[i - 1] =
                        z_common_data.save.info.playerData.adultEquips.cButtonSlots[i - 1];
                }

                if (((z_common_data.save.info.equips.buttonItems[i] >= ITEM_BOTTLE_EMPTY) &&
                     (z_common_data.save.info.equips.buttonItems[i] <= ITEM_BOTTLE_POE)) ||
                    ((z_common_data.save.info.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                     (z_common_data.save.info.equips.buttonItems[i] <= ITEM_CLAIM_CHECK))) {
                    PRINTF("Register_Item_Pt(%d)=%d\n", i, z_common_data.save.info.equips.cButtonSlots[i - 1]);
                    z_common_data.save.info.equips.buttonItems[i] =
                        z_common_data.save.info.inventory.items[z_common_data.save.info.equips.cButtonSlots[i - 1]];
                }
            }

            z_common_data.save.info.equips.equipment = z_common_data.save.info.playerData.adultEquips.equipment;
        }
    } else {
        for (i = 0; i < 4; i++) {
            z_common_data.save.info.playerData.adultEquips.buttonItems[i] = z_common_data.save.info.equips.buttonItems[i];

            if (i != 0) {
                z_common_data.save.info.playerData.adultEquips.cButtonSlots[i - 1] =
                    z_common_data.save.info.equips.cButtonSlots[i - 1];
            }
        }

        z_common_data.save.info.playerData.adultEquips.equipment = z_common_data.save.info.equips.equipment;

        if (z_common_data.save.info.playerData.childEquips.buttonItems[0] != ITEM_NONE) {
            for (i = 0; i < 4; i++) {
                z_common_data.save.info.equips.buttonItems[i] =
                    z_common_data.save.info.playerData.childEquips.buttonItems[i];

                if (i != 0) {
                    z_common_data.save.info.equips.cButtonSlots[i - 1] =
                        z_common_data.save.info.playerData.childEquips.cButtonSlots[i - 1];
                }

                if (((z_common_data.save.info.equips.buttonItems[i] >= ITEM_BOTTLE_EMPTY) &&
                     (z_common_data.save.info.equips.buttonItems[i] <= ITEM_BOTTLE_POE)) ||
                    ((z_common_data.save.info.equips.buttonItems[i] >= ITEM_WEIRD_EGG) &&
                     (z_common_data.save.info.equips.buttonItems[i] <= ITEM_CLAIM_CHECK))) {
                    PRINTF("Register_Item_Pt(%d)=%d\n", i, z_common_data.save.info.equips.cButtonSlots[i - 1]);
                    z_common_data.save.info.equips.buttonItems[i] =
                        z_common_data.save.info.inventory.items[z_common_data.save.info.equips.cButtonSlots[i - 1]];
                }
            }

            z_common_data.save.info.equips.equipment = z_common_data.save.info.playerData.childEquips.equipment;
            z_common_data.save.info.equips.equipment &= (u16) ~(0xF << (EQUIP_TYPE_SWORD * 4));
            z_common_data.save.info.equips.equipment |= EQUIP_VALUE_SWORD_KOKIRI << (EQUIP_TYPE_SWORD * 4);
        }
    }

    shieldEquipValue = z_common_data.save.info.equips.equipment & bit_check_data[EQUIP_TYPE_SHIELD];
    if (shieldEquipValue) {
        shieldEquipValue >>= bit_shift_data[EQUIP_TYPE_SHIELD];
        if (!CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SHIELD, shieldEquipValue - 1)) {
            z_common_data.save.info.equips.equipment &= bit_and_data[EQUIP_TYPE_SHIELD];
        }
    }
}

void yabusame_game_start(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    z_common_data.minigameState = 1;
    interfaceCtx->unk_23C = interfaceCtx->unk_240 = interfaceCtx->unk_242 = 0;
    z_common_data.minigameScore = yabu_sound = 0;
    interfaceCtx->hbaAmmo = 20;
}

void sword_system_dma(PlayState* play) {
    z_common_data.save.info.inventory.equipment |= OWNED_EQUIP_FLAG(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BIGGORON);
    z_common_data.save.info.inventory.equipment ^=
        OWNED_EQUIP_FLAG_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE);

    if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE)) {
        z_common_data.save.info.equips.buttonItems[0] = ITEM_GIANTS_KNIFE;
    } else {
        z_common_data.save.info.equips.buttonItems[0] = ITEM_SWORD_BIGGORON;
    }

    item_textuer_dma(play, 0);
}

void item_textuer_dma(PlayState* play, u16 button) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    osCreateMesgQueue(&interfaceCtx->loadQueue, &interfaceCtx->loadMsg, 1);
    DMA_REQUEST_ASYNC(&interfaceCtx->dmaRequest_160, interfaceCtx->iconItemSegment + (button * ITEM_ICON_SIZE),
                      GET_ITEM_ICON_VROM(z_common_data.save.info.equips.buttonItems[button]), ITEM_ICON_SIZE, 0,
                      &interfaceCtx->loadQueue, NULL, "../z_parameter.c", 1171);
    osRecvMesg(&interfaceCtx->loadQueue, NULL, OS_MESG_BLOCK);
}

void item_textuer_dma1(PlayState* play, u16 button) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    osCreateMesgQueue(&interfaceCtx->loadQueue, &interfaceCtx->loadMsg, 1);
    DMA_REQUEST_ASYNC(&interfaceCtx->dmaRequest_180, interfaceCtx->iconItemSegment + (button * ITEM_ICON_SIZE),
                      GET_ITEM_ICON_VROM(z_common_data.save.info.equips.buttonItems[button]), ITEM_ICON_SIZE, 0,
                      &interfaceCtx->loadQueue, NULL, "../z_parameter.c", 1193);
    osRecvMesg(&interfaceCtx->loadQueue, NULL, OS_MESG_BLOCK);
}

void a_item_dma_change(PlayState* play, u16 flag) {
    if (flag) {
        if ((z_common_data.save.info.equips.buttonItems[0] == ITEM_SLINGSHOT) ||
            (z_common_data.save.info.equips.buttonItems[0] == ITEM_BOW) ||
            (z_common_data.save.info.equips.buttonItems[0] == ITEM_BOMBCHU) ||
            (z_common_data.save.info.equips.buttonItems[0] == ITEM_FISHING_POLE) ||
            (z_common_data.buttonStatus[0] == BTN_DISABLED)) {
            if ((z_common_data.save.info.equips.buttonItems[0] == ITEM_SLINGSHOT) ||
                (z_common_data.save.info.equips.buttonItems[0] == ITEM_BOW) ||
                (z_common_data.save.info.equips.buttonItems[0] == ITEM_BOMBCHU) ||
                (z_common_data.save.info.equips.buttonItems[0] == ITEM_FISHING_POLE)) {
                z_common_data.save.info.equips.buttonItems[0] = z_common_data.buttonStatus[0];
                item_textuer_dma(play, 0);
            }
        } else if (z_common_data.save.info.equips.buttonItems[0] == ITEM_NONE) {
            if ((z_common_data.save.info.equips.buttonItems[0] != ITEM_NONE) ||
                (z_common_data.save.info.infTable[INFTABLE_INDEX_1DX] == 0)) {
                z_common_data.save.info.equips.buttonItems[0] = z_common_data.buttonStatus[0];
                item_textuer_dma(play, 0);
            }
        }

        z_common_data.buttonStatus[0] = z_common_data.buttonStatus[1] = z_common_data.buttonStatus[2] =
            z_common_data.buttonStatus[3] = BTN_ENABLED;
        alpha_change(HUD_VISIBILITY_ALL_NO_MINIMAP_BY_BTN_STATUS);
    } else {
        z_common_data.buttonStatus[0] = z_common_data.buttonStatus[1] = z_common_data.buttonStatus[2] =
            z_common_data.buttonStatus[3] = BTN_ENABLED;
        parameter_check(play);
    }
}

u8 item_get_setting(PlayState* play, u8 item) {
    static s16 BS_count[] = { 5, 10, 20, 30 }; // Sticks, nuts, bombs
    static s16 arrow_count[] = { 5, 10, 30 };
    static s16 bomchuw_count[] = { 5, 20 };
    static s16 rupy_ctdt[] = { 1, 5, 20, 50, 200, 10 };
    s16 i;
    s16 slot;
    s16 temp;

    slot = SLOT(item);
    if (item >= ITEM_DEKU_STICKS_5) {
        slot = SLOT(shift_item_no[item - ITEM_DEKU_STICKS_5]);
    }

    PRINTF_COLOR_YELLOW();
    PRINTF("item_get_setting=%d  pt=%d  z=%x\n", item, slot, z_common_data.save.info.inventory.items[slot]);
    PRINTF_RST();

    if ((item >= ITEM_MEDALLION_FOREST) && (item <= ITEM_MEDALLION_LIGHT)) {
        z_common_data.save.info.inventory.questItems |= check_bit[item - ITEM_MEDALLION_FOREST + QUEST_MEDALLION_FOREST];

        PRINTF_COLOR_YELLOW();
        PRINTF(T("封印 = %x\n", "Seals = %x\n"), z_common_data.save.info.inventory.questItems);
        PRINTF_RST();

        if (item == ITEM_MEDALLION_WATER) {
            Horse_escape_Spot06Water(play);
        }

        return ITEM_NONE;
    } else if ((item >= ITEM_SONG_MINUET) && (item <= ITEM_SONG_STORMS)) {
        z_common_data.save.info.inventory.questItems |= check_bit[item - ITEM_SONG_MINUET + QUEST_SONG_MINUET];

        PRINTF_COLOR_YELLOW();
        PRINTF(T("楽譜 = %x\n", "Musical scores = %x\n"), z_common_data.save.info.inventory.questItems);
        PRINTF(T("楽譜 = %x (%x) (%x)\n", "Musical scores = %x (%x) (%x)\n"),
               z_common_data.save.info.inventory.questItems, check_bit[item - ITEM_SONG_MINUET + QUEST_SONG_MINUET],
               check_bit[item - ITEM_SONG_MINUET]);
        PRINTF_RST();

        return ITEM_NONE;
    } else if ((item >= ITEM_KOKIRI_EMERALD) && (item <= ITEM_ZORA_SAPPHIRE)) {
        z_common_data.save.info.inventory.questItems |= check_bit[item - ITEM_KOKIRI_EMERALD + QUEST_KOKIRI_EMERALD];

        PRINTF_COLOR_YELLOW();
        PRINTF(T("精霊石 = %x\n", "Spiritual Stones = %x\n"), z_common_data.save.info.inventory.questItems);
        PRINTF_RST();

        return ITEM_NONE;
    } else if ((item == ITEM_STONE_OF_AGONY) || (item == ITEM_GERUDOS_CARD)) {
        z_common_data.save.info.inventory.questItems |= check_bit[item - ITEM_STONE_OF_AGONY + QUEST_STONE_OF_AGONY];

        PRINTF_COLOR_YELLOW();
        PRINTF(T("アイテム = %x\n", "Items = %x\n"), z_common_data.save.info.inventory.questItems);
        PRINTF_RST();

        return ITEM_NONE;
    } else if (item == ITEM_SKULL_TOKEN) {
        z_common_data.save.info.inventory.questItems |= check_bit[item - ITEM_SKULL_TOKEN + QUEST_SKULL_TOKEN];
        z_common_data.save.info.inventory.gsTokens++;

        PRINTF_COLOR_YELLOW();
        PRINTF(T("Ｎコイン = %x(%d)\n", "N Coins = %x(%d)\n"), z_common_data.save.info.inventory.questItems,
               z_common_data.save.info.inventory.gsTokens);
        PRINTF_RST();

        return ITEM_NONE;
    } else if ((item >= ITEM_SWORD_KOKIRI) && (item <= ITEM_SWORD_BIGGORON)) {
        z_common_data.save.info.inventory.equipment |=
            OWNED_EQUIP_FLAG(EQUIP_TYPE_SWORD, item - ITEM_SWORD_KOKIRI + EQUIP_INV_SWORD_KOKIRI);

        if (item == ITEM_SWORD_BIGGORON) {
            z_common_data.save.info.playerData.swordHealth = 8;

            if (ALL_EQUIP_VALUE(EQUIP_TYPE_SWORD) ==
                ((1 << EQUIP_INV_SWORD_KOKIRI) | (1 << EQUIP_INV_SWORD_MASTER) | (1 << EQUIP_INV_SWORD_BIGGORON) |
                 (1 << EQUIP_INV_SWORD_BROKENGIANTKNIFE))) {
                z_common_data.save.info.inventory.equipment ^=
                    OWNED_EQUIP_FLAG_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE);
                if (z_common_data.save.info.equips.buttonItems[0] == ITEM_GIANTS_KNIFE) {
                    z_common_data.save.info.equips.buttonItems[0] = ITEM_SWORD_BIGGORON;
                    item_textuer_dma(play, 0);
                }
            }
        } else if (item == ITEM_SWORD_MASTER) {
            z_common_data.save.info.equips.buttonItems[0] = ITEM_SWORD_MASTER;
            z_common_data.save.info.equips.equipment &= (u16) ~(0xF << (EQUIP_TYPE_SWORD * 4));
            z_common_data.save.info.equips.equipment |= EQUIP_VALUE_SWORD_MASTER << (EQUIP_TYPE_SWORD * 4);
            item_textuer_dma(play, 0);
        }

        return ITEM_NONE;
    } else if ((item >= ITEM_SHIELD_DEKU) && (item <= ITEM_SHIELD_MIRROR)) {
        z_common_data.save.info.inventory.equipment |= OWNED_EQUIP_FLAG(EQUIP_TYPE_SHIELD, item - ITEM_SHIELD_DEKU);
        return ITEM_NONE;
    } else if ((item >= ITEM_TUNIC_KOKIRI) && (item <= ITEM_TUNIC_ZORA)) {
        z_common_data.save.info.inventory.equipment |= OWNED_EQUIP_FLAG(EQUIP_TYPE_TUNIC, item - ITEM_TUNIC_KOKIRI);
        return ITEM_NONE;
    } else if ((item >= ITEM_BOOTS_KOKIRI) && (item <= ITEM_BOOTS_HOVER)) {
        z_common_data.save.info.inventory.equipment |= OWNED_EQUIP_FLAG(EQUIP_TYPE_BOOTS, item - ITEM_BOOTS_KOKIRI);
        return ITEM_NONE;
    } else if ((item == ITEM_DUNGEON_BOSS_KEY) || (item == ITEM_DUNGEON_COMPASS) || (item == ITEM_DUNGEON_MAP)) {
        z_common_data.save.info.inventory.dungeonItems[z_common_data.mapIndex] |= check_bit[item - ITEM_DUNGEON_BOSS_KEY];
        return ITEM_NONE;
    } else if (item == ITEM_SMALL_KEY) {
        if (z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex] < 0) {
            z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex] = 1;
            return ITEM_NONE;
        } else {
            z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex]++;
            return ITEM_NONE;
        }
    } else if ((item == ITEM_QUIVER_30) || (item == ITEM_BOW)) {
        if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
            Set_Non_Equip_Register(UPG_QUIVER, 1);
            INV_CONTENT(ITEM_BOW) = ITEM_BOW;
            AMMO(ITEM_BOW) = CAPACITY(UPG_QUIVER, 1);
            return ITEM_NONE;
        } else {
            AMMO(ITEM_BOW)++;
            if (AMMO(ITEM_BOW) > CUR_CAPACITY(UPG_QUIVER)) {
                AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
            }
        }
    } else if (item == ITEM_QUIVER_40) {
        Set_Non_Equip_Register(UPG_QUIVER, 2);
        AMMO(ITEM_BOW) = CAPACITY(UPG_QUIVER, 2);
        return ITEM_NONE;
    } else if (item == ITEM_QUIVER_50) {
        Set_Non_Equip_Register(UPG_QUIVER, 3);
        AMMO(ITEM_BOW) = CAPACITY(UPG_QUIVER, 3);
        return ITEM_NONE;
    } else if (item == ITEM_BULLET_BAG_40) {
        Set_Non_Equip_Register(UPG_BULLET_BAG, 2);
        AMMO(ITEM_SLINGSHOT) = CAPACITY(UPG_BULLET_BAG, 2);
        return ITEM_NONE;
    } else if (item == ITEM_BULLET_BAG_50) {
        Set_Non_Equip_Register(UPG_BULLET_BAG, 3);
        AMMO(ITEM_SLINGSHOT) = CAPACITY(UPG_BULLET_BAG, 3);
        return ITEM_NONE;
    } else if (item == ITEM_BOMB_BAG_20) {
        if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
            Set_Non_Equip_Register(UPG_BOMB_BAG, 1);
            INV_CONTENT(ITEM_BOMB) = ITEM_BOMB;
            AMMO(ITEM_BOMB) = CAPACITY(UPG_BOMB_BAG, 1);
            return ITEM_NONE;
        } else {
            AMMO(ITEM_BOMB)++;
            if (AMMO(ITEM_BOMB) > CUR_CAPACITY(UPG_BOMB_BAG)) {
                AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
            }
        }
    } else if (item == ITEM_BOMB_BAG_30) {
        Set_Non_Equip_Register(UPG_BOMB_BAG, 2);
        AMMO(ITEM_BOMB) = CAPACITY(UPG_BOMB_BAG, 2);
        return ITEM_NONE;
    } else if (item == ITEM_BOMB_BAG_40) {
        Set_Non_Equip_Register(UPG_BOMB_BAG, 3);
        AMMO(ITEM_BOMB) = CAPACITY(UPG_BOMB_BAG, 3);
        return ITEM_NONE;
    } else if (item == ITEM_STRENGTH_GORONS_BRACELET) {
        Set_Non_Equip_Register(UPG_STRENGTH, 1);
        return ITEM_NONE;
    } else if (item == ITEM_STRENGTH_SILVER_GAUNTLETS) {
        Set_Non_Equip_Register(UPG_STRENGTH, 2);
        return ITEM_NONE;
    } else if (item == ITEM_STRENGTH_GOLD_GAUNTLETS) {
        Set_Non_Equip_Register(UPG_STRENGTH, 3);
        return ITEM_NONE;
    } else if (item == ITEM_SCALE_SILVER) {
        Set_Non_Equip_Register(UPG_SCALE, 1);
        return ITEM_NONE;
    } else if (item == ITEM_SCALE_GOLDEN) {
        Set_Non_Equip_Register(UPG_SCALE, 2);
        return ITEM_NONE;
    } else if (item == ITEM_ADULTS_WALLET) {
        Set_Non_Equip_Register(UPG_WALLET, 1);
        return ITEM_NONE;
    } else if (item == ITEM_GIANTS_WALLET) {
        Set_Non_Equip_Register(UPG_WALLET, 2);
        return ITEM_NONE;
    } else if (item == ITEM_DEKU_STICK_UPGRADE_20) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_DEKU_STICK) = ITEM_DEKU_STICK;
        }
        Set_Non_Equip_Register(UPG_DEKU_STICKS, 2);
        AMMO(ITEM_DEKU_STICK) = CAPACITY(UPG_DEKU_STICKS, 2);
        return ITEM_NONE;
    } else if (item == ITEM_DEKU_STICK_UPGRADE_30) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_DEKU_STICK) = ITEM_DEKU_STICK;
        }
        Set_Non_Equip_Register(UPG_DEKU_STICKS, 3);
        AMMO(ITEM_DEKU_STICK) = CAPACITY(UPG_DEKU_STICKS, 3);
        return ITEM_NONE;
    } else if (item == ITEM_DEKU_NUT_UPGRADE_30) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_DEKU_NUT) = ITEM_DEKU_NUT;
        }
        Set_Non_Equip_Register(UPG_DEKU_NUTS, 2);
        AMMO(ITEM_DEKU_NUT) = CAPACITY(UPG_DEKU_NUTS, 2);
        return ITEM_NONE;
    } else if (item == ITEM_DEKU_NUT_UPGRADE_40) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_DEKU_NUT) = ITEM_DEKU_NUT;
        }
        Set_Non_Equip_Register(UPG_DEKU_NUTS, 3);
        AMMO(ITEM_DEKU_NUT) = CAPACITY(UPG_DEKU_NUTS, 3);
        return ITEM_NONE;
    } else if (item == ITEM_LONGSHOT) {
        INV_CONTENT(item) = item;
        for (i = 1; i < 4; i++) {
            if (z_common_data.save.info.equips.buttonItems[i] == ITEM_HOOKSHOT) {
                z_common_data.save.info.equips.buttonItems[i] = ITEM_LONGSHOT;
                item_textuer_dma(play, i);
            }
        }
        return ITEM_NONE;
    } else if (item == ITEM_DEKU_STICK) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_NONE) {
            Set_Non_Equip_Register(UPG_DEKU_STICKS, 1);
            AMMO(ITEM_DEKU_STICK) = 1;
        } else {
            AMMO(ITEM_DEKU_STICK)++;
            if (AMMO(ITEM_DEKU_STICK) > CUR_CAPACITY(UPG_DEKU_STICKS)) {
                AMMO(ITEM_DEKU_STICK) = CUR_CAPACITY(UPG_DEKU_STICKS);
            }
        }
    } else if ((item == ITEM_DEKU_STICKS_5) || (item == ITEM_DEKU_STICKS_10)) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_NONE) {
            Set_Non_Equip_Register(UPG_DEKU_STICKS, 1);
            AMMO(ITEM_DEKU_STICK) = BS_count[item - ITEM_DEKU_STICKS_5];
        } else {
            AMMO(ITEM_DEKU_STICK) += BS_count[item - ITEM_DEKU_STICKS_5];
            if (AMMO(ITEM_DEKU_STICK) > CUR_CAPACITY(UPG_DEKU_STICKS)) {
                AMMO(ITEM_DEKU_STICK) = CUR_CAPACITY(UPG_DEKU_STICKS);
            }
        }
        item = ITEM_DEKU_STICK;
    } else if (item == ITEM_DEKU_NUT) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_NONE) {
            Set_Non_Equip_Register(UPG_DEKU_NUTS, 1);
            AMMO(ITEM_DEKU_NUT) = ITEM_DEKU_NUT;
        } else {
            AMMO(ITEM_DEKU_NUT)++;
            if (AMMO(ITEM_DEKU_NUT) > CUR_CAPACITY(UPG_DEKU_NUTS)) {
                AMMO(ITEM_DEKU_NUT) = CUR_CAPACITY(UPG_DEKU_NUTS);
            }
        }
    } else if ((item == ITEM_DEKU_NUTS_5) || (item == ITEM_DEKU_NUTS_10)) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_NONE) {
            Set_Non_Equip_Register(UPG_DEKU_NUTS, 1);
            AMMO(ITEM_DEKU_NUT) += BS_count[item - ITEM_DEKU_NUTS_5];
            PRINTF(T("デクの実 %d(%d)=%d  BS_count=%d\n", "Deku Nuts %d(%d)=%d  BS_count=%d\n"), item, ITEM_DEKU_NUTS_5,
                   item - ITEM_DEKU_NUTS_5, BS_count[item - ITEM_DEKU_NUTS_5]);
        } else {
            AMMO(ITEM_DEKU_NUT) += BS_count[item - ITEM_DEKU_NUTS_5];
            if (AMMO(ITEM_DEKU_NUT) > CUR_CAPACITY(UPG_DEKU_NUTS)) {
                AMMO(ITEM_DEKU_NUT) = CUR_CAPACITY(UPG_DEKU_NUTS);
            }
        }
        item = ITEM_DEKU_NUT;
    } else if (item == ITEM_BOMB) {
        PRINTF(T(" 爆弾  爆弾  爆弾  爆弾 爆弾   爆弾 爆弾 \n", "Bomb  Bomb  Bomb  Bomb Bomb   Bomb Bomb\n"));
        if ((AMMO(ITEM_BOMB) += 1) > CUR_CAPACITY(UPG_BOMB_BAG)) {
            AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
        }
        return ITEM_NONE;
    } else if ((item >= ITEM_BOMBS_5) && (item <= ITEM_BOMBS_30)) {
        if ((AMMO(ITEM_BOMB) += BS_count[item - ITEM_BOMBS_5]) > CUR_CAPACITY(UPG_BOMB_BAG)) {
            AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
        }
        return ITEM_NONE;
    } else if (item == ITEM_BOMBCHU) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_BOMBCHU) = ITEM_BOMBCHU;
            AMMO(ITEM_BOMBCHU) = 10;
            return ITEM_NONE;
        } else {
            if ((AMMO(ITEM_BOMBCHU) += 10) > 50) {
                AMMO(ITEM_BOMBCHU) = 50;
            }
            return ITEM_NONE;
        }
    } else if ((item == ITEM_BOMBCHUS_5) || (item == ITEM_BOMBCHUS_20)) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(ITEM_BOMBCHU) = ITEM_BOMBCHU;
            AMMO(ITEM_BOMBCHU) += bomchuw_count[item - ITEM_BOMBCHUS_5];
            return ITEM_NONE;
        } else {
            if ((AMMO(ITEM_BOMBCHU) += bomchuw_count[item - ITEM_BOMBCHUS_5]) > 50) {
                AMMO(ITEM_BOMBCHU) = 50;
            }
            return ITEM_NONE;
        }
    } else if ((item >= ITEM_ARROWS_5) && (item <= ITEM_ARROWS_30)) {
        AMMO(ITEM_BOW) += arrow_count[item - ITEM_ARROWS_5];

        if ((AMMO(ITEM_BOW) >= CUR_CAPACITY(UPG_QUIVER)) || (AMMO(ITEM_BOW) < 0)) {
            AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
        }

        PRINTF(T("%d本  Item_MaxGet=%d\n", "%d books  Item_MaxGet=%d\n"), AMMO(ITEM_BOW), CUR_CAPACITY(UPG_QUIVER));

        return ITEM_BOW;
    } else if (item == ITEM_SLINGSHOT) {
        Set_Non_Equip_Register(UPG_BULLET_BAG, 1);
        INV_CONTENT(ITEM_SLINGSHOT) = ITEM_SLINGSHOT;
        AMMO(ITEM_SLINGSHOT) = 30;
        return ITEM_NONE;
    } else if (item == ITEM_DEKU_SEEDS) {
        AMMO(ITEM_SLINGSHOT) += 5;

        if (AMMO(ITEM_SLINGSHOT) >= CUR_CAPACITY(UPG_BULLET_BAG)) {
            AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
        }

        if (!GET_ITEMGETINF(ITEMGETINF_13)) {
            SET_ITEMGETINF(ITEMGETINF_13);
            return ITEM_NONE;
        }

        return ITEM_DEKU_SEEDS;
    } else if (item == ITEM_DEKU_SEEDS_30) {
        AMMO(ITEM_SLINGSHOT) += 30;

        if (AMMO(ITEM_SLINGSHOT) >= CUR_CAPACITY(UPG_BULLET_BAG)) {
            AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
        }

        if (!GET_ITEMGETINF(ITEMGETINF_13)) {
            SET_ITEMGETINF(ITEMGETINF_13);
            return ITEM_NONE;
        }

        return ITEM_DEKU_SEEDS;
    } else if (item == ITEM_OCARINA_FAIRY) {
        INV_CONTENT(ITEM_OCARINA_FAIRY) = ITEM_OCARINA_FAIRY;
        return ITEM_NONE;
    } else if (item == ITEM_OCARINA_OF_TIME) {
        INV_CONTENT(ITEM_OCARINA_OF_TIME) = ITEM_OCARINA_OF_TIME;
        for (i = 1; i < 4; i++) {
            if (z_common_data.save.info.equips.buttonItems[i] == ITEM_OCARINA_FAIRY) {
                z_common_data.save.info.equips.buttonItems[i] = ITEM_OCARINA_OF_TIME;
                item_textuer_dma(play, i);
            }
        }
        return ITEM_NONE;
    } else if (item == ITEM_MAGIC_BEAN) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_NONE) {
            INV_CONTENT(item) = item;
            AMMO(ITEM_MAGIC_BEAN) = 1;
            BEANS_BOUGHT = 1;
        } else {
            AMMO(ITEM_MAGIC_BEAN)++;
            BEANS_BOUGHT++;
        }
        return ITEM_NONE;
    } else if ((item == ITEM_HEART_PIECE_2) || (item == ITEM_HEART_PIECE)) {
        z_common_data.save.info.inventory.questItems += 1 << QUEST_HEART_PIECE_COUNT;
        return ITEM_NONE;
    } else if (item == ITEM_HEART_CONTAINER) {
        z_common_data.save.info.playerData.healthCapacity += 0x10;
        z_common_data.save.info.playerData.health += 0x10;
        return ITEM_NONE;
    } else if (item == ITEM_RECOVERY_HEART) {
        PRINTF(T("回復ハート回復ハート回復ハート\n", "Recovery Heart Recovery Heart Recovery Heart\n"));
        life_meter_play(play, 0x10);
        return item;
    } else if (item == ITEM_MAGIC_JAR_SMALL) {
        // magic_mode_check is only used to store the magicState.
        // Setting the state to FILL gets immediately overwritten in magic_meter_check.
        // I.e. magic is added not filled.
#if OOT_VERSION < PAL_1_0
        magic_mode_check(play);
#else
        if (z_common_data.magicState != MAGIC_STATE_ADD) {
            magic_mode_check(play);
        }
#endif

        magic_meter_check(play, 12, MAGIC_ADD);

        if (!GET_INFTABLE(INFTABLE_198)) {
            SET_INFTABLE(INFTABLE_198);
            return ITEM_NONE;
        }

        return item;
    } else if (item == ITEM_MAGIC_JAR_BIG) {
        // magic_mode_check is only used to store the magicState.
        // Setting the state to FILL gets immediately overwritten in magic_meter_check.
        // I.e. magic is added not filled.
#if OOT_VERSION < PAL_1_0
        magic_mode_check(play);
#else
        if (z_common_data.magicState != MAGIC_STATE_ADD) {
            magic_mode_check(play);
        }
#endif

        magic_meter_check(play, 24, MAGIC_ADD);

        if (!GET_INFTABLE(INFTABLE_198)) {
            SET_INFTABLE(INFTABLE_198);
            return ITEM_NONE;
        }

        return item;
    } else if ((item >= ITEM_RUPEE_GREEN) && (item <= ITEM_INVALID_8)) {
        lupy_increase(rupy_ctdt[item - ITEM_RUPEE_GREEN]);
        return ITEM_NONE;
    } else if (item == ITEM_BOTTLE_EMPTY) {
        temp = SLOT(item);

        for (i = 0; i < 4; i++) {
            if (z_common_data.save.info.inventory.items[temp + i] == ITEM_NONE) {
                z_common_data.save.info.inventory.items[temp + i] = item;
                return ITEM_NONE;
            }
        }
    } else if (((item >= ITEM_BOTTLE_POTION_RED) && (item <= ITEM_BOTTLE_POE)) || (item == ITEM_MILK)) {
        temp = SLOT(item);

        if ((item != ITEM_BOTTLE_MILK_FULL) && (item != ITEM_BOTTLE_RUTOS_LETTER)) {
            if (item == ITEM_MILK) {
                item = ITEM_BOTTLE_MILK_FULL;
                temp = SLOT(item);
            }

            for (i = 0; i < 4; i++) {
                if (z_common_data.save.info.inventory.items[temp + i] == ITEM_BOTTLE_EMPTY) {
                    PRINTF(T("Item_Pt(1)=%d Item_Pt(2)=%d Item_Pt(3)=%d   空瓶=%d   中味=%d\n",
                             "Item_Pt(1)=%d Item_Pt(2)=%d Item_Pt(3)=%d   Empty Bottle=%d   Content=%d\n"),
                           z_common_data.save.info.equips.cButtonSlots[0], z_common_data.save.info.equips.cButtonSlots[1],
                           z_common_data.save.info.equips.cButtonSlots[2], temp + i, item);

                    if (z_common_data.save.info.equips.cButtonSlots[0] == temp + i) {
                        z_common_data.save.info.equips.buttonItems[1] = item;
                        item_textuer_dma1(play, 1);
                        z_common_data.buttonStatus[1] = BTN_ENABLED;
                    } else if (z_common_data.save.info.equips.cButtonSlots[1] == temp + i) {
                        z_common_data.save.info.equips.buttonItems[2] = item;
                        item_textuer_dma1(play, 2);
                        z_common_data.buttonStatus[2] = BTN_ENABLED;
                    } else if (z_common_data.save.info.equips.cButtonSlots[2] == temp + i) {
                        z_common_data.save.info.equips.buttonItems[3] = item;
                        item_textuer_dma(play, 3);
                        z_common_data.buttonStatus[3] = BTN_ENABLED;
                    }

                    z_common_data.save.info.inventory.items[temp + i] = item;
                    return ITEM_NONE;
                }
            }
        } else {
            for (i = 0; i < 4; i++) {
                if (z_common_data.save.info.inventory.items[temp + i] == ITEM_NONE) {
                    z_common_data.save.info.inventory.items[temp + i] = item;
                    return ITEM_NONE;
                }
            }
        }
    } else if ((item >= ITEM_WEIRD_EGG) && (item <= ITEM_CLAIM_CHECK)) {
        if (item == ITEM_POACHERS_SAW) {
            //! @bug Setting this shared flag makes getting the Deku Nut upgrade impossible
            SET_ITEMGETINF(ITEMGETINF_FOREST_STAGE_NUT_UPGRADE);
        }

        temp = INV_CONTENT(item);
        INV_CONTENT(item) = item;

        if (temp != ITEM_NONE) {
            for (i = 1; i < 4; i++) {
                if (z_common_data.save.info.equips.buttonItems[i] == temp) {
                    if (item != ITEM_SOLD_OUT) {
                        z_common_data.save.info.equips.buttonItems[i] = item;
                        item_textuer_dma(play, i);
                    } else {
                        z_common_data.save.info.equips.buttonItems[i] = ITEM_NONE;
                    }
                    return ITEM_NONE;
                }
            }
        }

        return ITEM_NONE;
    }

    temp = z_common_data.save.info.inventory.items[slot];
    PRINTF("Item_Register(%d)=%d  %d\n", slot, item, temp);
    INV_CONTENT(item) = item;

    return temp;
}

u8 item_get_non_setting(u8 item) {
    s16 i;
    s16 slot = SLOT(item);
    s16 temp;

    if (item >= ITEM_DEKU_STICKS_5) {
        slot = SLOT(shift_item_no[item - ITEM_DEKU_STICKS_5]);
    }

    PRINTF_COLOR_GREEN();
    PRINTF("item_get_non_setting=%d  pt=%d  z=%x\n", item, slot, z_common_data.save.info.inventory.items[slot]);
    PRINTF_RST();

    if ((item >= ITEM_MEDALLION_FOREST) && (item <= ITEM_MEDALLION_LIGHT)) {
        return ITEM_NONE;
    } else if ((item >= ITEM_KOKIRI_EMERALD) && (item <= ITEM_SKULL_TOKEN)) {
        return ITEM_NONE;
    } else if ((item >= ITEM_SWORD_KOKIRI) && (item <= ITEM_SWORD_BIGGORON)) {
        if (item == ITEM_SWORD_BIGGORON) {
            return ITEM_NONE;
        } else if (CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, item - ITEM_SWORD_KOKIRI + EQUIP_INV_SWORD_KOKIRI)) {
            return item;
        } else {
            return ITEM_NONE;
        }
    } else if ((item >= ITEM_SHIELD_DEKU) && (item <= ITEM_SHIELD_MIRROR)) {
        if (CHECK_OWNED_EQUIP(EQUIP_TYPE_SHIELD, item - ITEM_SHIELD_DEKU + EQUIP_INV_SHIELD_DEKU)) {
            return item;
        } else {
            return ITEM_NONE;
        }
    } else if ((item >= ITEM_TUNIC_KOKIRI) && (item <= ITEM_TUNIC_ZORA)) {
        if (CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, item - ITEM_TUNIC_KOKIRI + EQUIP_INV_TUNIC_KOKIRI)) {
            return item;
        } else {
            return ITEM_NONE;
        }
    } else if ((item >= ITEM_BOOTS_KOKIRI) && (item <= ITEM_BOOTS_HOVER)) {
        if (CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, item - ITEM_BOOTS_KOKIRI + EQUIP_INV_BOOTS_KOKIRI)) {
            return item;
        } else {
            return ITEM_NONE;
        }
    } else if ((item == ITEM_DUNGEON_BOSS_KEY) || (item == ITEM_DUNGEON_COMPASS) || (item == ITEM_DUNGEON_MAP)) {
        return ITEM_NONE;
    } else if (item == ITEM_SMALL_KEY) {
        return ITEM_NONE;
    } else if ((item >= ITEM_SLINGSHOT) && (item <= ITEM_BOMBCHU)) {
        return ITEM_NONE;
    } else if ((item == ITEM_BOMBCHUS_5) || (item == ITEM_BOMBCHUS_20)) {
        return ITEM_NONE;
    } else if ((item == ITEM_QUIVER_30) || (item == ITEM_BOW)) {
        if (CUR_UPG_VALUE(UPG_QUIVER) == 0) {
            return ITEM_NONE;
        } else {
            return 0;
        }
    } else if ((item == ITEM_QUIVER_40) || (item == ITEM_QUIVER_50)) {
        return ITEM_NONE;
    } else if ((item == ITEM_BULLET_BAG_40) || (item == ITEM_BULLET_BAG_50)) {
        return ITEM_NONE;
    } else if ((item == ITEM_BOMB_BAG_20) || (item == ITEM_BOMB)) {
        if (CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
            return ITEM_NONE;
        } else {
            return 0;
        }
    } else if ((item >= ITEM_DEKU_STICK_UPGRADE_20) && (item <= ITEM_DEKU_NUT_UPGRADE_40)) {
        return ITEM_NONE;
    } else if ((item >= ITEM_BOMB_BAG_30) && (item <= ITEM_GIANTS_WALLET)) {
        return ITEM_NONE;
    } else if (item == ITEM_LONGSHOT) {
        return ITEM_NONE;
    } else if ((item == ITEM_DEKU_SEEDS) || (item == ITEM_DEKU_SEEDS_30)) {
        if (!GET_ITEMGETINF(ITEMGETINF_13)) {
            return ITEM_NONE;
        } else {
            return ITEM_DEKU_SEEDS;
        }
    } else if (item == ITEM_MAGIC_BEAN) {
        return ITEM_NONE;
    } else if ((item == ITEM_HEART_PIECE_2) || (item == ITEM_HEART_PIECE)) {
        return ITEM_NONE;
    } else if (item == ITEM_HEART_CONTAINER) {
        return ITEM_NONE;
    } else if (item == ITEM_RECOVERY_HEART) {
        return ITEM_RECOVERY_HEART;
    } else if ((item == ITEM_MAGIC_JAR_SMALL) || (item == ITEM_MAGIC_JAR_BIG)) {
        PRINTF(T("魔法の壷 Get_Inf_Table( 25, 0x0100)=%d\n", "Magic Pot Get_Inf_Table( 25, 0x0100)=%d\n"),
               GET_INFTABLE(INFTABLE_198));
        if (!GET_INFTABLE(INFTABLE_198)) {
            return ITEM_NONE;
        } else {
            return item;
        }
    } else if ((item >= ITEM_RUPEE_GREEN) && (item <= ITEM_INVALID_8)) {
        return ITEM_NONE;
    } else if (item == ITEM_BOTTLE_EMPTY) {
        return ITEM_NONE;
    } else if (((item >= ITEM_BOTTLE_POTION_RED) && (item <= ITEM_BOTTLE_POE)) || (item == ITEM_MILK)) {
        temp = SLOT(item);

        if ((item != ITEM_BOTTLE_MILK_FULL) && (item != ITEM_BOTTLE_RUTOS_LETTER)) {
            if (item == ITEM_MILK) {
                item = ITEM_BOTTLE_MILK_FULL;
                temp = SLOT(item);
            }

            for (i = 0; i < 4; i++) {
                if (z_common_data.save.info.inventory.items[temp + i] == ITEM_BOTTLE_EMPTY) {
                    return ITEM_NONE;
                }
            }
        } else {
            for (i = 0; i < 4; i++) {
                if (z_common_data.save.info.inventory.items[temp + i] == ITEM_NONE) {
                    return ITEM_NONE;
                }
            }
        }
    } else if ((item >= ITEM_WEIRD_EGG) && (item <= ITEM_CLAIM_CHECK)) {
        return ITEM_NONE;
    }

    return z_common_data.save.info.inventory.items[slot];
}

void item_clear_setting(u16 item, u16 invSlot) {
    s16 i;

    if (item == ITEM_MAGIC_BEAN) {
        BEANS_BOUGHT = 0;
    }

    z_common_data.save.info.inventory.items[invSlot] = ITEM_NONE;

    PRINTF("\nItem_Register(%d)\n", invSlot, z_common_data.save.info.inventory.items[invSlot]);

    for (i = 1; i < 4; i++) {
        if (z_common_data.save.info.equips.buttonItems[i] == item) {
            z_common_data.save.info.equips.buttonItems[i] = ITEM_NONE;
            z_common_data.save.info.equips.cButtonSlots[i - 1] = SLOT_NONE;
        }
    }
}

s32 item_change_setting(PlayState* play, u16 oldItem, u16 newItem) {
    s16 i;

    for (i = 0; i < ARRAY_COUNT(z_common_data.save.info.inventory.items); i++) {
        if (z_common_data.save.info.inventory.items[i] == oldItem) {
            z_common_data.save.info.inventory.items[i] = newItem;
            PRINTF(T("アイテム消去(%d)\n", "Item Purge (%d)\n"), i);
            for (i = 1; i < 4; i++) {
                if (z_common_data.save.info.equips.buttonItems[i] == oldItem) {
                    z_common_data.save.info.equips.buttonItems[i] = newItem;
                    item_textuer_dma(play, i);
                    return true;
                }
            }
            return true;
        }
    }

    return false;
}

s32 findEmptyBottle(void) {
    s32 slot;

    for (slot = SLOT_BOTTLE_1; slot <= SLOT_BOTTLE_4; slot++) {
        if (z_common_data.save.info.inventory.items[slot] == ITEM_BOTTLE_EMPTY) {
            return true;
        }
    }

    return false;
}

s32 bottle_interior_check(u8 bottleItem) {
    s32 slot;

    for (slot = SLOT_BOTTLE_1; slot <= SLOT_BOTTLE_4; slot++) {
        if (z_common_data.save.info.inventory.items[slot] == bottleItem) {
            return true;
        }
    }

    return false;
}

void bottle_getting(PlayState* play, u8 item, u8 button) {
    PRINTF("item_no=%x,  c_no=%x,  Pt=%x  Item_Register=%x\n", item, button,
           z_common_data.save.info.equips.cButtonSlots[button - 1],
           z_common_data.save.info.inventory.items[z_common_data.save.info.equips.cButtonSlots[button - 1]]);

    // Special case to only empty half of a Lon Lon Milk Bottle
    if ((z_common_data.save.info.inventory.items[z_common_data.save.info.equips.cButtonSlots[button - 1]] ==
         ITEM_BOTTLE_MILK_FULL) &&
        (item == ITEM_BOTTLE_EMPTY)) {
        item = ITEM_BOTTLE_MILK_HALF;
    }

    z_common_data.save.info.inventory.items[z_common_data.save.info.equips.cButtonSlots[button - 1]] = item;
    z_common_data.save.info.equips.buttonItems[button] = item;

    item_textuer_dma(play, button);

    play->pauseCtx.cursorItem[PAUSE_ITEM] = item;
    z_common_data.buttonStatus[button] = BTN_ENABLED;
}

s32 bottle_fairy_use(PlayState* play) {
    s32 bottleSlot = SLOT(ITEM_BOTTLE_FAIRY);
    s16 i;
    s16 j;

    for (i = 0; i < 4; i++) {
        if (z_common_data.save.info.inventory.items[bottleSlot + i] == ITEM_BOTTLE_FAIRY) {
            for (j = 1; j < 4; j++) {
                if (z_common_data.save.info.equips.buttonItems[j] == ITEM_BOTTLE_FAIRY) {
                    z_common_data.save.info.equips.buttonItems[j] = ITEM_BOTTLE_EMPTY;
                    item_textuer_dma(play, j);
                    i = 0;
                    bottleSlot = z_common_data.save.info.equips.cButtonSlots[j - 1];
                    break;
                }
            }
            PRINTF(T("妖精使用＝%d\n", "Fairy Usage=%d\n"), bottleSlot);
            z_common_data.save.info.inventory.items[bottleSlot + i] = ITEM_BOTTLE_EMPTY;
            return true;
        }
    }

    return false;
}

void nuki_set(s32* buf, u16 size) {
    u16 i;

    for (i = 0; i < size; i++) {
        buf[i] = 0;
    }
}

void next_do_action_textuer_dma(InterfaceContext* interfaceCtx, u16 action, s16 loadOffset) {
#if OOT_NTSC
    static void* do_index[] = { gAttackDoActionJPNTex, gCheckDoActionJPNTex };
#else
    static void* do_index[] = { gAttackDoActionENGTex, gCheckDoActionENGTex };
#endif

    if (action >= DO_ACTION_MAX) {
        action = DO_ACTION_NONE;
    }

    if (z_common_data.language != 0) { // LANGUAGE_JPN for NTSC versions, LANGUAGE_ENG for PAL versions
        action += DO_ACTION_MAX;
    }

#if OOT_VERSION >= PAL_1_0
    if (z_common_data.language == 2) { // LANGUAGE_FRA for PAL versions
        action += DO_ACTION_MAX;
    }
#endif

#if OOT_VERSION < PAL_1_0
    if ((action != 0 * DO_ACTION_MAX + DO_ACTION_NONE) && (action != 1 * DO_ACTION_MAX + DO_ACTION_NONE))
#else
    if ((action != 0 * DO_ACTION_MAX + DO_ACTION_NONE) && (action != 1 * DO_ACTION_MAX + DO_ACTION_NONE) &&
        (action != 2 * DO_ACTION_MAX + DO_ACTION_NONE))
#endif
    {
        osCreateMesgQueue(&interfaceCtx->loadQueue, &interfaceCtx->loadMsg, 1);
        DMA_REQUEST_ASYNC(&interfaceCtx->dmaRequest_160,
                          interfaceCtx->doActionSegment + (loadOffset * DO_ACTION_TEX_SIZE),
                          (uintptr_t)_do_action_staticSegmentRomStart + (action * DO_ACTION_TEX_SIZE),
                          DO_ACTION_TEX_SIZE, 0, &interfaceCtx->loadQueue, NULL, "../z_parameter.c", 2145);
        osRecvMesg(&interfaceCtx->loadQueue, NULL, OS_MESG_BLOCK);
    } else {
        SegmentBaseAddress[7] = VIRTUAL_TO_PHYSICAL(interfaceCtx->doActionSegment);
        nuki_set(SEGMENTED_TO_VIRTUAL(do_index[loadOffset]), DO_ACTION_TEX_SIZE / 4);
    }
}

void do_action_point_set(PlayState* play, u16 action) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    PauseContext* pauseCtx = &play->pauseCtx;

    if (interfaceCtx->unk_1F0 != action) {
        interfaceCtx->unk_1F0 = action;
        interfaceCtx->unk_1EC = 1;
        interfaceCtx->unk_1F4 = 0.0f;
        next_do_action_textuer_dma(interfaceCtx, action, 1);
        if (pauseCtx->state != PAUSE_STATE_OFF) {
            interfaceCtx->unk_1EC = 3;
        }
    }
}

void do_action_navi_set(PlayState* play, u16 naviCallState) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (((naviCallState == 0x1D) || (naviCallState == 0x1E)) && !interfaceCtx->naviCalling &&
        (play->csCtx.state == CS_STATE_IDLE)) {
        // clang-format off
        if (naviCallState == 0x1E) { Nai_FxFlagEntry(NA_SE_VO_NAVY_CALL, &_dummy_zero_f, 4,
                                                            &_dummy_one, &_dummy_one,
                                                            &_dummy_zero_s8);
        }
        // clang-format on

        if (naviCallState == 0x1D) {
            Na_StartFxmixSe(&_dummy_zero_f, NA_SE_VO_NA_HELLO_2, 32);
        }

        interfaceCtx->naviCalling = true;
        navi_pt = 0;
        navi_ct = 10;
    } else if ((naviCallState == 0x1F) && interfaceCtx->naviCalling) {
        interfaceCtx->naviCalling = false;
    }
}

void sp_action_set(PlayState* play, u16 action) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (z_common_data.language != 0) { // LANGUAGE_JPN for NTSC versions, LANGUAGE_ENG for PAL versions
        action += DO_ACTION_MAX;
    }

#if OOT_VERSION >= PAL_1_0
    if (z_common_data.language == 2) { // LANGUAGE_FRA for PAL versions
        action += DO_ACTION_MAX;
    }
#endif

    interfaceCtx->unk_1FC = action;

    osCreateMesgQueue(&interfaceCtx->loadQueue, &interfaceCtx->loadMsg, 1);
    DMA_REQUEST_ASYNC(&interfaceCtx->dmaRequest_160, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE,
                      (uintptr_t)_do_action_staticSegmentRomStart + (action * DO_ACTION_TEX_SIZE), DO_ACTION_TEX_SIZE,
                      0, &interfaceCtx->loadQueue, NULL, "../z_parameter.c", 2228);
    osRecvMesg(&interfaceCtx->loadQueue, NULL, OS_MESG_BLOCK);

    interfaceCtx->unk_1FA = true;
}

/**
 * @return false if player is out of health
 */
s32 life_meter_play(PlayState* play, s16 amount) {
    u16 heartCount;
    UNUSED_NDEBUG u16 healthLevel;

    PRINTF(T("＊＊＊＊＊  増減=%d (now=%d, max=%d)  ＊＊＊", "*****  Fluctuation=%d (now=%d, max=%d)  ***"), amount,
           z_common_data.save.info.playerData.health, z_common_data.save.info.playerData.healthCapacity);

    // clang-format off
    if (amount > 0) { Nai_FxFlagEntry(NA_SE_SY_HP_RECOVER, &_dummy_zero_f, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
    } else if (z_common_data.save.info.playerData.isDoubleDefenseAcquired && (amount < 0)) {
        amount >>= 1;
        PRINTF(T("ハート減少半分！！＝%d\n", "Heart decrease halved!! = %d\n"),  amount);
    }
    // clang-format on

    z_common_data.save.info.playerData.health += amount;

    if (z_common_data.save.info.playerData.health > z_common_data.save.info.playerData.healthCapacity) {
        z_common_data.save.info.playerData.health = z_common_data.save.info.playerData.healthCapacity;
    }

    heartCount = z_common_data.save.info.playerData.health % 0x10;

    healthLevel = heartCount;
    if (heartCount != 0) {
        if (heartCount > 10) {
            healthLevel = 3;
        } else if (heartCount > 5) {
            healthLevel = 2;
        } else {
            healthLevel = 1;
        }
    }

    PRINTF(T("  ライフ=%d  ＊＊＊  %d  ＊＊＊＊＊＊\n", "  Life=%d  ***  %d  ******\n"),
           z_common_data.save.info.playerData.health, healthLevel);

    if (z_common_data.save.info.playerData.health <= 0) {
        z_common_data.save.info.playerData.health = 0;
        return false;
    } else {
        return true;
    }
}

void life_meter_max(s16 hearts) {
    z_common_data.save.info.playerData.healthCapacity += hearts * 0x10;
}

void lupy_increase(s16 rupeeChange) {
    z_common_data.rupeeAccumulator += rupeeChange;
}

void item_count_vary(s16 item, s16 ammoChange) {
    PRINTF(T("アイテム = (%d)    数 = (%d + %d)  ", "Item = (%d)    Amount = (%d + %d)  "), item, AMMO(item),
           ammoChange);

    if (item == ITEM_DEKU_STICK) {
        AMMO(ITEM_DEKU_STICK) += ammoChange;

        if (AMMO(ITEM_DEKU_STICK) >= CUR_CAPACITY(UPG_DEKU_STICKS)) {
            AMMO(ITEM_DEKU_STICK) = CUR_CAPACITY(UPG_DEKU_STICKS);
        } else if (AMMO(ITEM_DEKU_STICK) < 0) {
            AMMO(ITEM_DEKU_STICK) = 0;
        }
    } else if (item == ITEM_DEKU_NUT) {
        AMMO(ITEM_DEKU_NUT) += ammoChange;

        if (AMMO(ITEM_DEKU_NUT) >= CUR_CAPACITY(UPG_DEKU_NUTS)) {
            AMMO(ITEM_DEKU_NUT) = CUR_CAPACITY(UPG_DEKU_NUTS);
        } else if (AMMO(ITEM_DEKU_NUT) < 0) {
            AMMO(ITEM_DEKU_NUT) = 0;
        }
    } else if (item == ITEM_BOMBCHU) {
        AMMO(ITEM_BOMBCHU) += ammoChange;

        if (AMMO(ITEM_BOMBCHU) >= 50) {
            AMMO(ITEM_BOMBCHU) = 50;
        } else if (AMMO(ITEM_BOMBCHU) < 0) {
            AMMO(ITEM_BOMBCHU) = 0;
        }
    } else if (item == ITEM_BOW) {
        AMMO(ITEM_BOW) += ammoChange;

        if (AMMO(ITEM_BOW) >= CUR_CAPACITY(UPG_QUIVER)) {
            AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
        } else if (AMMO(ITEM_BOW) < 0) {
            AMMO(ITEM_BOW) = 0;
        }
    } else if ((item == ITEM_SLINGSHOT) || (item == ITEM_DEKU_SEEDS)) {
        AMMO(ITEM_SLINGSHOT) += ammoChange;

        if (AMMO(ITEM_SLINGSHOT) >= CUR_CAPACITY(UPG_BULLET_BAG)) {
            AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
        } else if (AMMO(ITEM_SLINGSHOT) < 0) {
            AMMO(ITEM_SLINGSHOT) = 0;
        }
    } else if (item == ITEM_BOMB) {
        AMMO(ITEM_BOMB) += ammoChange;

        if (AMMO(ITEM_BOMB) >= CUR_CAPACITY(UPG_BOMB_BAG)) {
            AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
        } else if (AMMO(ITEM_BOMB) < 0) {
            AMMO(ITEM_BOMB) = 0;
        }
    } else if (item == ITEM_MAGIC_BEAN) {
        AMMO(ITEM_MAGIC_BEAN) += ammoChange;
    }

    PRINTF(T("合計 = (%d)\n", "Total = (%d)\n"), AMMO(item));
}

void magic_mode_check(PlayState* play) {
    if (z_common_data.save.info.playerData.isMagicAcquired) {
        z_common_data.prevMagicState = z_common_data.magicState;
        z_common_data.magicFillTarget =
            (z_common_data.save.info.playerData.isDoubleMagicAcquired + 1) * MAGIC_NORMAL_METER;
        z_common_data.magicState = MAGIC_STATE_FILL;
    }
}

void magic_cancel_check(PlayState* play) {
    if ((z_common_data.magicState != MAGIC_STATE_STEP_CAPACITY) && (z_common_data.magicState != MAGIC_STATE_FILL)) {
        if (z_common_data.magicState == MAGIC_STATE_ADD) {
            z_common_data.prevMagicState = z_common_data.magicState;
        }
        z_common_data.magicState = MAGIC_STATE_RESET;
    }
}

/**
 * Request to either increase or consume magic.
 * @param amount the positive-valued amount to either increase or decrease magic by
 * @param type how the magic is increased or consumed.
 * @return false if the request failed
 */
s32 magic_meter_check(PlayState* play, s16 amount, s16 type) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (!z_common_data.save.info.playerData.isMagicAcquired) {
        return false;
    }

    if ((type != MAGIC_ADD) && (z_common_data.save.info.playerData.magic - amount) < 0) {
        if (z_common_data.magicCapacity != 0) {
            Nai_FxFlagEntry(NA_SE_SY_ERROR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
        return false;
    }

    switch (type) {
        case MAGIC_CONSUME_NOW:
        case MAGIC_CONSUME_NOW_ALT:
            // Consume magic immediately
            if ((z_common_data.magicState == MAGIC_STATE_IDLE) ||
                (z_common_data.magicState == MAGIC_STATE_CONSUME_LENS)) {
                if (z_common_data.magicState == MAGIC_STATE_CONSUME_LENS) {
                    play->actorCtx.lensActive = false;
                }
                z_common_data.magicTarget = z_common_data.save.info.playerData.magic - amount;
                z_common_data.magicState = MAGIC_STATE_CONSUME_SETUP;
                return true;
            } else {
                Nai_FxFlagEntry(NA_SE_SY_ERROR, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
                return false;
            }

        case MAGIC_CONSUME_WAIT_NO_PREVIEW:
            // Sets consume target but waits to consume.
            // No yellow magic to preview target consumption.
            // Unused
            if ((z_common_data.magicState == MAGIC_STATE_IDLE) ||
                (z_common_data.magicState == MAGIC_STATE_CONSUME_LENS)) {
                if (z_common_data.magicState == MAGIC_STATE_CONSUME_LENS) {
                    play->actorCtx.lensActive = false;
                }
                z_common_data.magicTarget = z_common_data.save.info.playerData.magic - amount;
                z_common_data.magicState = MAGIC_STATE_METER_FLASH_3;
                return true;
            } else {
                Nai_FxFlagEntry(NA_SE_SY_ERROR, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
                return false;
            }

        case MAGIC_CONSUME_LENS:
            if (z_common_data.magicState == MAGIC_STATE_IDLE) {
                if (z_common_data.save.info.playerData.magic != 0) {
                    interfaceCtx->lensMagicConsumptionTimer = 80;
                    z_common_data.magicState = MAGIC_STATE_CONSUME_LENS;
                    return true;
                } else {
                    return false;
                }
            } else if (z_common_data.magicState == MAGIC_STATE_CONSUME_LENS) {
                return true;
            } else {
                return false;
            }

        case MAGIC_CONSUME_WAIT_PREVIEW:
            // Sets consume target but waits to consume.
            // Preview consumption with a yellow bar
            if ((z_common_data.magicState == MAGIC_STATE_IDLE) ||
                (z_common_data.magicState == MAGIC_STATE_CONSUME_LENS)) {
                if (z_common_data.magicState == MAGIC_STATE_CONSUME_LENS) {
                    play->actorCtx.lensActive = false;
                }
                z_common_data.magicTarget = z_common_data.save.info.playerData.magic - amount;
                z_common_data.magicState = MAGIC_STATE_METER_FLASH_2;
                return true;
            } else {
                Nai_FxFlagEntry(NA_SE_SY_ERROR, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
                return false;
            }

        case MAGIC_ADD:
            // Sets target for magic to increase to
            if (z_common_data.save.info.playerData.magic <= z_common_data.magicCapacity) {
                z_common_data.magicTarget = z_common_data.save.info.playerData.magic + amount;

                if (z_common_data.magicTarget >= z_common_data.magicCapacity) {
                    z_common_data.magicTarget = z_common_data.magicCapacity;
                }

                z_common_data.magicState = MAGIC_STATE_ADD;
                return true;
            }
            break;
    }

    return false;
}

void magic_meter_move(PlayState* play) {
    static s16 m_gage_color[][3] = {
        { 255, 255, 255 },
        { 150, 150, 150 },
        { 255, 255, 150 }, // unused
        { 255, 255, 50 },  // unused
    };
    static s16 magic_pt[] = { 0, 1, 1, 0 };
    static s16 mc = 2;
    static s16 mcpt = 1;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    MessageContext* msgCtx = &play->msgCtx;
    s16 borderChangeR;
    s16 borderChangeG;
    s16 borderChangeB;
    s16 temp; // target for magicCapacity, or magicBorderIndex

    switch (z_common_data.magicState) {
        case MAGIC_STATE_STEP_CAPACITY:
            // Step magicCapacity to the capacity determined by magicLevel
            // This changes the width of the magic meter drawn
            temp = z_common_data.save.info.playerData.magicLevel * MAGIC_NORMAL_METER;
            if (temp != z_common_data.magicCapacity) {
                if (z_common_data.magicCapacity < temp) {
                    z_common_data.magicCapacity += 8;
                    if (z_common_data.magicCapacity > temp) {
                        z_common_data.magicCapacity = temp;
                    }
                } else {
                    z_common_data.magicCapacity -= 8;
                    if (z_common_data.magicCapacity <= temp) {
                        z_common_data.magicCapacity = temp;
                    }
                }
            } else {
                // Once the capacity has reached its target,
                // follow up by filling magic to magicFillTarget
                z_common_data.magicState = MAGIC_STATE_FILL;
            }
            break;

        case MAGIC_STATE_FILL:
            // Add magic until magicFillTarget is reached
            z_common_data.save.info.playerData.magic += 4;

            if (z_common_data.gameMode == GAMEMODE_NORMAL && !IS_CUTSCENE_LAYER) {
                Nai_FxFlagEntry(NA_SE_SY_GAUGE_UP - SFX_FLAG, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }

            PRINTF(T("蓄電  MAGIC_NOW=%d (%d)\n", "Storage  MAGIC_NOW=%d (%d)\n"),
                   z_common_data.save.info.playerData.magic, z_common_data.magicFillTarget);

            if (z_common_data.save.info.playerData.magic >= z_common_data.magicFillTarget) {
                z_common_data.save.info.playerData.magic = z_common_data.magicFillTarget;
                z_common_data.magicState = z_common_data.prevMagicState;
                z_common_data.prevMagicState = MAGIC_STATE_IDLE;
            }
            break;

        case MAGIC_STATE_CONSUME_SETUP:
            // Sets the speed at which magic border flashes
            mc = 2;
            z_common_data.magicState = MAGIC_STATE_CONSUME;
            break;

        case MAGIC_STATE_CONSUME:
            // Consume magic until target is reached or no more magic is available
            z_common_data.save.info.playerData.magic -= 2;
            if (z_common_data.save.info.playerData.magic <= 0) {
                z_common_data.save.info.playerData.magic = 0;
                z_common_data.magicState = MAGIC_STATE_METER_FLASH_1;
                mgr = mgg = mgb = 255;
            } else if (z_common_data.save.info.playerData.magic == z_common_data.magicTarget) {
                z_common_data.magicState = MAGIC_STATE_METER_FLASH_1;
                mgr = mgg = mgb = 255;
            }
            FALLTHROUGH; // Flash border while magic is being consumed
        case MAGIC_STATE_METER_FLASH_1:
        case MAGIC_STATE_METER_FLASH_2:
        case MAGIC_STATE_METER_FLASH_3:
            temp = magic_pt[mcpt];
            borderChangeR = ABS(mgr - m_gage_color[temp][0]) / mc;
            borderChangeG = ABS(mgg - m_gage_color[temp][1]) / mc;
            borderChangeB = ABS(mgb - m_gage_color[temp][2]) / mc;

            if (mgr >= m_gage_color[temp][0]) {
                mgr -= borderChangeR;
            } else {
                mgr += borderChangeR;
            }

            if (mgg >= m_gage_color[temp][1]) {
                mgg -= borderChangeG;
            } else {
                mgg += borderChangeG;
            }

            if (mgb >= m_gage_color[temp][2]) {
                mgb -= borderChangeB;
            } else {
                mgb += borderChangeB;
            }

            mc--;
            if (mc == 0) {
                mgr = m_gage_color[temp][0];
                mgg = m_gage_color[temp][1];
                mgb = m_gage_color[temp][2];
                mc = YREG(40 + mcpt);
                mcpt++;
                if (mcpt >= 4) {
                    mcpt = 0;
                }
            }
            break;

        case MAGIC_STATE_RESET:
            mgr = mgg = mgb = 255;
            z_common_data.magicState = MAGIC_STATE_IDLE;
            break;

        case MAGIC_STATE_CONSUME_LENS:
            // Slowly consume magic while lens is on
            if (!IS_PAUSED(&play->pauseCtx) && (msgCtx->msgMode == MSGMODE_NONE) &&
                (play->gameOverCtx.state == GAMEOVER_INACTIVE) && (play->transitionTrigger == TRANS_TRIGGER_OFF) &&
                (play->transitionMode == TRANS_MODE_OFF) && !Game_play_demo_mode_check(play)) {
                if ((z_common_data.save.info.playerData.magic == 0) ||

                    ((player_condition_check(play) >= PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) &&
                     (player_condition_check(play) <= PLAYER_ENV_HAZARD_UNDERWATER_FREE)) ||
                    ((z_common_data.save.info.equips.buttonItems[1] != ITEM_LENS_OF_TRUTH) &&
                     (z_common_data.save.info.equips.buttonItems[2] != ITEM_LENS_OF_TRUTH) &&
                     (z_common_data.save.info.equips.buttonItems[3] != ITEM_LENS_OF_TRUTH)) ||
                    !play->actorCtx.lensActive) {
                    // Force lens off and set magic meter state to idle
                    play->actorCtx.lensActive = false;
                    Nai_FxFlagEntry(NA_SE_SY_GLASSMODE_OFF, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    z_common_data.magicState = MAGIC_STATE_IDLE;
                    mgr = mgg = mgb = 255;
                    break;
                }

                interfaceCtx->lensMagicConsumptionTimer--;
                if (interfaceCtx->lensMagicConsumptionTimer == 0) {
                    z_common_data.save.info.playerData.magic--;
                    interfaceCtx->lensMagicConsumptionTimer = 80;
                }
            }

            temp = magic_pt[mcpt];
            borderChangeR = ABS(mgr - m_gage_color[temp][0]) / mc;
            borderChangeG = ABS(mgg - m_gage_color[temp][1]) / mc;
            borderChangeB = ABS(mgb - m_gage_color[temp][2]) / mc;

            if (mgr >= m_gage_color[temp][0]) {
                mgr -= borderChangeR;
            } else {
                mgr += borderChangeR;
            }

            if (mgg >= m_gage_color[temp][1]) {
                mgg -= borderChangeG;
            } else {
                mgg += borderChangeG;
            }

            if (mgb >= m_gage_color[temp][2]) {
                mgb -= borderChangeB;
            } else {
                mgb += borderChangeB;
            }

            mc--;
            if (mc == 0) {
                mgr = m_gage_color[temp][0];
                mgg = m_gage_color[temp][1];
                mgb = m_gage_color[temp][2];
                mc = YREG(40 + mcpt);
                mcpt++;
                if (mcpt >= 4) {
                    mcpt = 0;
                }
            }
            break;

        case MAGIC_STATE_ADD:
            // Add magic until target is reached
            z_common_data.save.info.playerData.magic += 4;
            Nai_FxFlagEntry(NA_SE_SY_GAUGE_UP - SFX_FLAG, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            if (z_common_data.save.info.playerData.magic >= z_common_data.magicTarget) {
                z_common_data.save.info.playerData.magic = z_common_data.magicTarget;
                z_common_data.magicState = z_common_data.prevMagicState;
                z_common_data.prevMagicState = MAGIC_STATE_IDLE;
            }
            break;

        default:
            z_common_data.magicState = MAGIC_STATE_IDLE;
            break;
    }
}

void magic_meter_draw(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 magicMeterY;

    OPEN_DISPS(play->state.gfxCtx, "../z_parameter.c", 2650);

    if (z_common_data.save.info.playerData.magicLevel != 0) {
        // NOLINTBEGIN
        if (z_common_data.save.info.playerData.healthCapacity > 0xA0)
            magicMeterY = R_MAGIC_METER_Y_LOWER; // two rows of hearts
        else
            magicMeterY = R_MAGIC_METER_Y_HIGHER; // one row of hearts
        // NOLINTEND

        overlay_rectangle_a_prim(play->state.gfxCtx);

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, mgr, mgg, mgb, interfaceCtx->magicAlpha);
        gDPSetEnvColor(OVERLAY_DISP++, 100, 50, 50, 255);

        OVERLAY_DISP = texture_rectangleIA8(OVERLAY_DISP, gMagicMeterEndTex, 8, 16, R_MAGIC_METER_X, magicMeterY, 8, 16,
                                      1 << 10, 1 << 10);

        OVERLAY_DISP = texture_rectangleIA8(OVERLAY_DISP, gMagicMeterMidTex, 24, 16, R_MAGIC_METER_X + 8, magicMeterY,
                                      z_common_data.magicCapacity, 16, 1 << 10, 1 << 10);

        gDPLoadTextureBlock(OVERLAY_DISP++, gMagicMeterEndTex, G_IM_FMT_IA, G_IM_SIZ_8b, 8, 16, 0,
                            G_TX_MIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 3, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSPTextureRectangle(OVERLAY_DISP++, (R_MAGIC_METER_X + 8 + z_common_data.magicCapacity) << 2, magicMeterY << 2,
                            (R_MAGIC_METER_X + 8 + z_common_data.magicCapacity + 8) << 2, (magicMeterY + 16) << 2,
                            G_TX_RENDERTILE, 256, 0, 1 << 10, 1 << 10);

        gDPPipeSync(OVERLAY_DISP++);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE, PRIMITIVE,
                          ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);

        if (z_common_data.magicState == MAGIC_STATE_METER_FLASH_2) {
            // Yellow part of the meter indicating the amount of magic to be subtracted
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 250, 250, 0, interfaceCtx->magicAlpha);

            gDPLoadMultiBlock_4b(OVERLAY_DISP++, gMagicMeterFillTex, 0x0000, G_TX_RENDERTILE, G_IM_FMT_I, 16, 16, 0,
                                 G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                 G_TX_NOLOD, G_TX_NOLOD);

            gSPTextureRectangle(OVERLAY_DISP++, R_MAGIC_FILL_X << 2, (magicMeterY + 3) << 2,
                                (R_MAGIC_FILL_X + z_common_data.save.info.playerData.magic) << 2,
                                (magicMeterY + 10) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

            // Fill the rest of the meter with the normal magic color
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_MAGIC_FILL_COLOR(0), R_MAGIC_FILL_COLOR(1), R_MAGIC_FILL_COLOR(2),
                            interfaceCtx->magicAlpha);

            gSPTextureRectangle(OVERLAY_DISP++, R_MAGIC_FILL_X << 2, (magicMeterY + 3) << 2,
                                (R_MAGIC_FILL_X + z_common_data.magicTarget) << 2, (magicMeterY + 10) << 2,
                                G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
        } else {
            // Fill the whole meter with the normal magic color
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_MAGIC_FILL_COLOR(0), R_MAGIC_FILL_COLOR(1), R_MAGIC_FILL_COLOR(2),
                            interfaceCtx->magicAlpha);

            gDPLoadMultiBlock_4b(OVERLAY_DISP++, gMagicMeterFillTex, 0x0000, G_TX_RENDERTILE, G_IM_FMT_I, 16, 16, 0,
                                 G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                 G_TX_NOLOD, G_TX_NOLOD);

            gSPTextureRectangle(OVERLAY_DISP++, R_MAGIC_FILL_X << 2, (magicMeterY + 3) << 2,
                                (R_MAGIC_FILL_X + z_common_data.save.info.playerData.magic) << 2,
                                (magicMeterY + 10) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_parameter.c", 2731);
}

void total_event_timer_set(s16 seconds) {
    z_common_data.timerX[TIMER_ID_SUB] = 140;
    z_common_data.timerY[TIMER_ID_SUB] = 80;
    player_diving = false;
    z_common_data.subTimerSeconds = seconds;

    if (seconds != 0) {
        // count down
        z_common_data.subTimerState = SUBTIMER_STATE_DOWN_INIT;
    } else {
        // count up
        z_common_data.subTimerState = SUBTIMER_STATE_UP_INIT;
    }
}

/**
 * Set the subTimer to 1 second left
 */
void total_event_timer_end(PlayState* play) {
    if (z_common_data.subTimerState != SUBTIMER_STATE_OFF) {
        if (GET_EVENTINF(EVENTINF_MARATHON_ACTIVE)) {
            // The running-man race counts up and finished at MARATHON_TIME_LIMIT
            z_common_data.subTimerSeconds = MARATHON_TIME_LIMIT - 1;
        } else {
            z_common_data.subTimerSeconds = 1;
        }
    }
}

void event_timer_set(s16 seconds) {
    z_common_data.timerX[TIMER_ID_MAIN] = 140;
    z_common_data.timerY[TIMER_ID_MAIN] = 80;
    player_diving = false;
    z_common_data.timerSeconds = seconds;

    if (seconds != 0) {
        // count down
        z_common_data.timerState = TIMER_STATE_DOWN_INIT;
    } else {
        // count up
        z_common_data.timerState = TIMER_STATE_UP_INIT;
    }
}

void do_message_display(GraphicsContext* gfxCtx, void* texture) {
    OPEN_DISPS(gfxCtx, "../z_parameter.c", 2820);

    gDPLoadTextureBlock_4b(OVERLAY_DISP++, texture, G_IM_FMT_IA, DO_ACTION_TEX_WIDTH, DO_ACTION_TEX_HEIGHT, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

    CLOSE_DISPS(gfxCtx, "../z_parameter.c", 2829);
}

void button_background_rectangle_all(PlayState* play) {
    static void* navi_txt[] = LANGUAGE_ARRAY(gNaviCUpJPNTex, gNaviCUpENGTex, gNaviCUpENGTex, gNaviCUpENGTex);
#if OOT_VERSION >= PAL_1_0
    static s16 start_xpos[] = { 132, 130, 130 };
#endif
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Player* player = GET_PLAYER(play);
    PauseContext* pauseCtx = &play->pauseCtx;
    s16 temp; // Used as both an alpha value and a button index
#if OOT_PAL
    s16 texCoordScale;
    s16 width;
    s16 height;
#endif

    OPEN_DISPS(play->state.gfxCtx, "../z_parameter.c", 2900);

    // B Button Color & Texture
    // Also loads the Item Button Texture reused by other buttons afterwards
    gDPPipeSync(OVERLAY_DISP++);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_B_BTN_COLOR(0), R_B_BTN_COLOR(1), R_B_BTN_COLOR(2), interfaceCtx->bAlpha);
    gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
    OVERLAY_DISP =
        texture_rectangleIA8(OVERLAY_DISP, gButtonBackgroundTex, 32, 32, R_ITEM_BTN_X(0), R_ITEM_BTN_Y(0),
                       R_ITEM_BTN_WIDTH(0), R_ITEM_BTN_WIDTH(0), R_ITEM_BTN_DD(0) << 1, R_ITEM_BTN_DD(0) << 1);

    // C-Left Button Color & Texture
    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_C_BTN_COLOR(0), R_C_BTN_COLOR(1), R_C_BTN_COLOR(2),
                    interfaceCtx->cLeftAlpha);
    gSPTextureRectangle(OVERLAY_DISP++, R_ITEM_BTN_X(1) << 2, R_ITEM_BTN_Y(1) << 2,
                        (R_ITEM_BTN_X(1) + R_ITEM_BTN_WIDTH(1)) << 2, (R_ITEM_BTN_Y(1) + R_ITEM_BTN_WIDTH(1)) << 2,
                        G_TX_RENDERTILE, 0, 0, R_ITEM_BTN_DD(1) << 1, R_ITEM_BTN_DD(1) << 1);

    // C-Down Button Color & Texture
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_C_BTN_COLOR(0), R_C_BTN_COLOR(1), R_C_BTN_COLOR(2),
                    interfaceCtx->cDownAlpha);
    gSPTextureRectangle(OVERLAY_DISP++, R_ITEM_BTN_X(2) << 2, R_ITEM_BTN_Y(2) << 2,
                        (R_ITEM_BTN_X(2) + R_ITEM_BTN_WIDTH(2)) << 2, (R_ITEM_BTN_Y(2) + R_ITEM_BTN_WIDTH(2)) << 2,
                        G_TX_RENDERTILE, 0, 0, R_ITEM_BTN_DD(2) << 1, R_ITEM_BTN_DD(2) << 1);

    // C-Right Button Color & Texture
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_C_BTN_COLOR(0), R_C_BTN_COLOR(1), R_C_BTN_COLOR(2),
                    interfaceCtx->cRightAlpha);
    gSPTextureRectangle(OVERLAY_DISP++, R_ITEM_BTN_X(3) << 2, R_ITEM_BTN_Y(3) << 2,
                        (R_ITEM_BTN_X(3) + R_ITEM_BTN_WIDTH(3)) << 2, (R_ITEM_BTN_Y(3) + R_ITEM_BTN_WIDTH(3)) << 2,
                        G_TX_RENDERTILE, 0, 0, R_ITEM_BTN_DD(3) << 1, R_ITEM_BTN_DD(3) << 1);

    if (!IS_PAUSE_STATE_GAMEOVER(pauseCtx)) {
        if (IS_PAUSED(&play->pauseCtx)) {
            // Start Button Texture, Color & Label
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, START_BUTTON_R, START_BUTTON_G, START_BUTTON_B,
                            interfaceCtx->startAlpha);

#if OOT_VERSION < PAL_1_0
            gSPTextureRectangle(OVERLAY_DISP++, R_START_BTN_X << 2, R_START_BTN_Y << 2, (R_START_BTN_X + 22) << 2,
                                (R_START_BTN_Y + 22) << 2, G_TX_RENDERTILE, 0, 0, (s32)(1.4277344 * (1 << 10)),
                                (s32)(1.4277344 * (1 << 10)));
#elif OOT_NTSC
            gSPTextureRectangle(OVERLAY_DISP++, 132 << 2, 17 << 2, (132 + 22) << 2, (17 + 22) << 2, G_TX_RENDERTILE, 0,
                                0, (s32)(1.4277344 * (1 << 10)), (s32)(1.4277344 * (1 << 10)));
#else
            gSPTextureRectangle(OVERLAY_DISP++, start_xpos[z_common_data.language] << 2, 17 << 2,
                                (start_xpos[z_common_data.language] + 22) << 2, (17 + 22) << 2, G_TX_RENDERTILE,
                                0, 0, (s32)(1.4277344 * (1 << 10)), (s32)(1.4277344 * (1 << 10)));
#endif

            gDPPipeSync(OVERLAY_DISP++);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->startAlpha);
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

            gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE * 2, G_IM_FMT_IA,
                                   DO_ACTION_TEX_WIDTH, DO_ACTION_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

#if OOT_NTSC
            R_START_LABEL_SCALE = (1 << 10) / (R_START_LABEL_DD(z_common_data.language) / 100.0f);
            R_START_LABEL_WIDTH = DO_ACTION_TEX_WIDTH / (R_START_LABEL_DD(z_common_data.language) / 100.0f);
            R_START_LABEL_HEIGHT = DO_ACTION_TEX_HEIGHT / (R_START_LABEL_DD(z_common_data.language) / 100.0f);
            gSPTextureRectangle(OVERLAY_DISP++, R_START_LABEL_X(z_common_data.language) << 2,
                                R_START_LABEL_Y(z_common_data.language) << 2,
                                (R_START_LABEL_X(z_common_data.language) + R_START_LABEL_WIDTH) << 2,
                                (R_START_LABEL_Y(z_common_data.language) + R_START_LABEL_HEIGHT) << 2, G_TX_RENDERTILE,
                                0, 0, R_START_LABEL_SCALE, R_START_LABEL_SCALE);
#else
            texCoordScale = (1 << 10) / (R_START_LABEL_DD(z_common_data.language) / 100.0f);
            width = DO_ACTION_TEX_WIDTH / (R_START_LABEL_DD(z_common_data.language) / 100.0f);
            height = DO_ACTION_TEX_HEIGHT / (R_START_LABEL_DD(z_common_data.language) / 100.0f);
            gSPTextureRectangle(OVERLAY_DISP++, R_START_LABEL_X(z_common_data.language) << 2,
                                R_START_LABEL_Y(z_common_data.language) << 2,
                                (R_START_LABEL_X(z_common_data.language) + width) << 2,
                                (R_START_LABEL_Y(z_common_data.language) + height) << 2, G_TX_RENDERTILE, 0, 0,
                                texCoordScale, texCoordScale);
#endif
        }
    }

    if (interfaceCtx->naviCalling && !IS_PAUSED(&play->pauseCtx) && (play->csCtx.state == CS_STATE_IDLE)) {
        if (!navi_pt) {
            // C-Up Button Texture, Color & Label (Navi Text)
            gDPPipeSync(OVERLAY_DISP++);

            if ((z_common_data.hudVisibilityMode == HUD_VISIBILITY_NOTHING) ||
                (z_common_data.hudVisibilityMode == HUD_VISIBILITY_NOTHING_ALT) ||
                (z_common_data.hudVisibilityMode == HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE)) {
                temp = 0;
            } else if ((player->stateFlags1 & PLAYER_STATE1_21) ||
                       (player_condition_check(play) == PLAYER_ENV_HAZARD_UNDERWATER_FREE) ||
                       (player->stateFlags2 & PLAYER_STATE2_CRAWLING)) {
                temp = 70;
            } else {
                temp = interfaceCtx->healthAlpha;
            }

            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_C_BTN_COLOR(0), R_C_BTN_COLOR(1), R_C_BTN_COLOR(2), temp);
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gSPTextureRectangle(OVERLAY_DISP++, R_C_UP_BTN_X << 2, R_C_UP_BTN_Y << 2, (R_C_UP_BTN_X + 16) << 2,
                                (R_C_UP_BTN_Y + 16) << 2, G_TX_RENDERTILE, 0, 0, 2 << 10, 2 << 10);
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, temp);
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

#if !PLATFORM_IQUE
            gDPLoadTextureBlock_4b(OVERLAY_DISP++, navi_txt[z_common_data.language], G_IM_FMT_IA, 32, 8, 0,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                   G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(OVERLAY_DISP++, R_C_UP_ICON_X << 2, R_C_UP_ICON_Y << 2, (R_C_UP_ICON_X + 32) << 2,
                                (R_C_UP_ICON_Y + 8) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
#else
            gDPLoadTextureBlock_4b(OVERLAY_DISP++, navi_txt[z_common_data.language], G_IM_FMT_IA, 48, 16, 0,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                   G_TX_NOLOD, G_TX_NOLOD);
            gSPTextureRectangle(OVERLAY_DISP++, (R_C_UP_ICON_X - 8) << 2, (R_C_UP_ICON_Y - 4) << 2,
                                (R_C_UP_ICON_X + 40) << 2, (R_C_UP_ICON_Y + 12) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10,
                                1 << 10);
#endif
        }

        navi_ct--;
        if (navi_ct == 0) {
            navi_pt ^= 1;
            navi_ct = 10;
        }
    }

    gDPPipeSync(OVERLAY_DISP++);

    // Empty C Button Arrows
    for (temp = 1; temp < 4; temp++) {
        if (z_common_data.save.info.equips.buttonItems[temp] > 0xF0) {
            if (temp == 1) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_C_BTN_COLOR(0), R_C_BTN_COLOR(1), R_C_BTN_COLOR(2),
                                interfaceCtx->cLeftAlpha);
            } else if (temp == 2) {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_C_BTN_COLOR(0), R_C_BTN_COLOR(1), R_C_BTN_COLOR(2),
                                interfaceCtx->cDownAlpha);
            } else {
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_C_BTN_COLOR(0), R_C_BTN_COLOR(1), R_C_BTN_COLOR(2),
                                interfaceCtx->cRightAlpha);
            }

            OVERLAY_DISP = texture_rectangleIA8(OVERLAY_DISP, ((u8*)gButtonBackgroundTex + ((32 * 32) * (temp + 1))), 32, 32,
                                          R_ITEM_BTN_X(temp), R_ITEM_BTN_Y(temp), R_ITEM_BTN_WIDTH(temp),
                                          R_ITEM_BTN_WIDTH(temp), R_ITEM_BTN_DD(temp) << 1, R_ITEM_BTN_DD(temp) << 1);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_parameter.c", 3071);
}

void button_item_rectangle(PlayState* play, void* texture, s16 button) {
    OPEN_DISPS(play->state.gfxCtx, "../z_parameter.c", 3079);

    gDPLoadTextureBlock(OVERLAY_DISP++, texture, G_IM_FMT_RGBA, G_IM_SIZ_32b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

    gSPTextureRectangle(OVERLAY_DISP++, R_ITEM_ICON_X(button) << 2, R_ITEM_ICON_Y(button) << 2,
                        (R_ITEM_ICON_X(button) + R_ITEM_ICON_WIDTH(button)) << 2,
                        (R_ITEM_ICON_Y(button) + R_ITEM_ICON_WIDTH(button)) << 2, G_TX_RENDERTILE, 0, 0,
                        R_ITEM_ICON_DD(button) << 1, R_ITEM_ICON_DD(button) << 1);

    CLOSE_DISPS(play->state.gfxCtx, "../z_parameter.c", 3094);
}

void count_display_rectangle(PlayState* play, s16 button, s16 alpha) {
    s16 i;
    s16 ammo;

    OPEN_DISPS(play->state.gfxCtx, "../z_parameter.c", 3105);

    i = z_common_data.save.info.equips.buttonItems[button];

    if ((i == ITEM_DEKU_STICK) || (i == ITEM_DEKU_NUT) || (i == ITEM_BOMB) || (i == ITEM_BOW) ||
        ((i >= ITEM_BOW_FIRE) && (i <= ITEM_BOW_LIGHT)) || (i == ITEM_SLINGSHOT) || (i == ITEM_BOMBCHU) ||
        (i == ITEM_MAGIC_BEAN)) {

        if ((i >= ITEM_BOW_FIRE) && (i <= ITEM_BOW_LIGHT)) {
            i = ITEM_BOW;
        }

        ammo = AMMO(i);

        gDPPipeSync(OVERLAY_DISP++);

        if ((button == 0) && (z_common_data.minigameState == 1)) {
            ammo = play->interfaceCtx.hbaAmmo;
        } else if ((button == 0) && (play->shootingGalleryStatus > 1)) {
            ammo = play->shootingGalleryStatus - 1;
        } else if ((button == 0) && (play->sceneId == SCENE_BOMBCHU_BOWLING_ALLEY) && Actor_Environment_sw_Check(play, 0x38)) {
            ammo = play->bombchuBowlingStatus;
            if (ammo < 0) {
                ammo = 0;
            }
        } else if (((i == ITEM_BOW) && (AMMO(i) == CUR_CAPACITY(UPG_QUIVER))) ||
                   ((i == ITEM_BOMB) && (AMMO(i) == CUR_CAPACITY(UPG_BOMB_BAG))) ||
                   ((i == ITEM_SLINGSHOT) && (AMMO(i) == CUR_CAPACITY(UPG_BULLET_BAG))) ||
                   ((i == ITEM_DEKU_STICK) && (AMMO(i) == CUR_CAPACITY(UPG_DEKU_STICKS))) ||
                   ((i == ITEM_DEKU_NUT) && (AMMO(i) == CUR_CAPACITY(UPG_DEKU_NUTS))) ||
                   ((i == ITEM_BOMBCHU) && (ammo == 50)) || ((i == ITEM_MAGIC_BEAN) && (ammo == 15))) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 120, 255, 0, alpha);
        }

        if (ammo == 0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 100, 100, alpha);
        }

        i = 0;
        while (ammo >= 10) {
            i++;
            ammo -= 10;
        }

        if (i != 0) {
            OVERLAY_DISP = texture_rectangleIA8(OVERLAY_DISP, ((u8*)gAmmoDigit0Tex + ((8 * 8) * i)), 8, 8,
                                          R_ITEM_AMMO_X(button), R_ITEM_AMMO_Y(button), 8, 8, 1 << 10, 1 << 10);
        }

        OVERLAY_DISP = texture_rectangleIA8(OVERLAY_DISP, ((u8*)gAmmoDigit0Tex + ((8 * 8) * ammo)), 8, 8,
                                      R_ITEM_AMMO_X(button) + 6, R_ITEM_AMMO_Y(button), 8, 8, 1 << 10, 1 << 10);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_parameter.c", 3158);
}

void button_background_set(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    OPEN_DISPS(play->state.gfxCtx, "../z_parameter.c", 3172);

    Matrix_translate(0.0f, 0.0f, XREG(18) / 10.0f, MTXMODE_NEW);
    Matrix_scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_rotateX(interfaceCtx->unk_1F4 / 10000.0f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(OVERLAY_DISP++, play->state.gfxCtx, "../z_parameter.c", 3177);
    gSPVertex(OVERLAY_DISP++, &interfaceCtx->actionVtx[0], 4, 0);

    gDPLoadTextureBlock(OVERLAY_DISP++, gButtonBackgroundTex, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);

    gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

    CLOSE_DISPS(play->state.gfxCtx, "../z_parameter.c", 3187);
}

void vtx_data_initial(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    s16 i;

    interfaceCtx->actionVtx = GRAPH_ALLOC(play->state.gfxCtx, 8 * sizeof(Vtx));

    interfaceCtx->actionVtx[0].v.ob[0] = interfaceCtx->actionVtx[2].v.ob[0] = -14;
    interfaceCtx->actionVtx[1].v.ob[0] = interfaceCtx->actionVtx[3].v.ob[0] = interfaceCtx->actionVtx[0].v.ob[0] + 28;

    interfaceCtx->actionVtx[0].v.ob[1] = interfaceCtx->actionVtx[1].v.ob[1] = 14;
    interfaceCtx->actionVtx[2].v.ob[1] = interfaceCtx->actionVtx[3].v.ob[1] = interfaceCtx->actionVtx[0].v.ob[1] - 28;

    interfaceCtx->actionVtx[4].v.ob[0] = interfaceCtx->actionVtx[6].v.ob[0] = -(XREG(21) / 2);
    interfaceCtx->actionVtx[5].v.ob[0] = interfaceCtx->actionVtx[7].v.ob[0] =
        interfaceCtx->actionVtx[4].v.ob[0] + XREG(21);

    interfaceCtx->actionVtx[4].v.ob[1] = interfaceCtx->actionVtx[5].v.ob[1] = XREG(28) / 2;
    interfaceCtx->actionVtx[6].v.ob[1] = interfaceCtx->actionVtx[7].v.ob[1] =
        interfaceCtx->actionVtx[4].v.ob[1] - XREG(28);

    for (i = 0; i < 8; i += 4) {
        interfaceCtx->actionVtx[i].v.ob[2] = interfaceCtx->actionVtx[i + 1].v.ob[2] =
            interfaceCtx->actionVtx[i + 2].v.ob[2] = interfaceCtx->actionVtx[i + 3].v.ob[2] = 0;

        interfaceCtx->actionVtx[i].v.flag = interfaceCtx->actionVtx[i + 1].v.flag =
            interfaceCtx->actionVtx[i + 2].v.flag = interfaceCtx->actionVtx[i + 3].v.flag = 0;

        interfaceCtx->actionVtx[i].v.tc[0] = interfaceCtx->actionVtx[i].v.tc[1] =
            interfaceCtx->actionVtx[i + 1].v.tc[1] = interfaceCtx->actionVtx[i + 2].v.tc[0] = 0;
        interfaceCtx->actionVtx[i + 1].v.tc[0] = interfaceCtx->actionVtx[i + 2].v.tc[1] =
            interfaceCtx->actionVtx[i + 3].v.tc[0] = interfaceCtx->actionVtx[i + 3].v.tc[1] = 1024;

        interfaceCtx->actionVtx[i].v.cn[0] = interfaceCtx->actionVtx[i + 1].v.cn[0] =
            interfaceCtx->actionVtx[i + 2].v.cn[0] = interfaceCtx->actionVtx[i + 3].v.cn[0] =
                interfaceCtx->actionVtx[i].v.cn[1] = interfaceCtx->actionVtx[i + 1].v.cn[1] =
                    interfaceCtx->actionVtx[i + 2].v.cn[1] = interfaceCtx->actionVtx[i + 3].v.cn[1] =
                        interfaceCtx->actionVtx[i].v.cn[2] = interfaceCtx->actionVtx[i + 1].v.cn[2] =
                            interfaceCtx->actionVtx[i + 2].v.cn[2] = interfaceCtx->actionVtx[i + 3].v.cn[2] = 255;

        interfaceCtx->actionVtx[i].v.cn[3] = interfaceCtx->actionVtx[i + 1].v.cn[3] =
            interfaceCtx->actionVtx[i + 2].v.cn[3] = interfaceCtx->actionVtx[i + 3].v.cn[3] = 255;
    }

    interfaceCtx->actionVtx[5].v.tc[0] = interfaceCtx->actionVtx[7].v.tc[0] = 1536;
    interfaceCtx->actionVtx[6].v.tc[1] = interfaceCtx->actionVtx[7].v.tc[1] = 512;

    interfaceCtx->beatingHeartVtx = GRAPH_ALLOC(play->state.gfxCtx, 4 * sizeof(Vtx));

    interfaceCtx->beatingHeartVtx[0].v.ob[0] = interfaceCtx->beatingHeartVtx[2].v.ob[0] = -8;
    interfaceCtx->beatingHeartVtx[1].v.ob[0] = interfaceCtx->beatingHeartVtx[3].v.ob[0] = 8;
    interfaceCtx->beatingHeartVtx[0].v.ob[1] = interfaceCtx->beatingHeartVtx[1].v.ob[1] = 8;
    interfaceCtx->beatingHeartVtx[2].v.ob[1] = interfaceCtx->beatingHeartVtx[3].v.ob[1] = -8;

    interfaceCtx->beatingHeartVtx[0].v.ob[2] = interfaceCtx->beatingHeartVtx[1].v.ob[2] =
        interfaceCtx->beatingHeartVtx[2].v.ob[2] = interfaceCtx->beatingHeartVtx[3].v.ob[2] = 0;

    interfaceCtx->beatingHeartVtx[0].v.flag = interfaceCtx->beatingHeartVtx[1].v.flag =
        interfaceCtx->beatingHeartVtx[2].v.flag = interfaceCtx->beatingHeartVtx[3].v.flag = 0;

    interfaceCtx->beatingHeartVtx[0].v.tc[0] = interfaceCtx->beatingHeartVtx[0].v.tc[1] =
        interfaceCtx->beatingHeartVtx[1].v.tc[1] = interfaceCtx->beatingHeartVtx[2].v.tc[0] = 0;
    interfaceCtx->beatingHeartVtx[1].v.tc[0] = interfaceCtx->beatingHeartVtx[2].v.tc[1] =
        interfaceCtx->beatingHeartVtx[3].v.tc[0] = interfaceCtx->beatingHeartVtx[3].v.tc[1] = 512;

    interfaceCtx->beatingHeartVtx[0].v.cn[0] = interfaceCtx->beatingHeartVtx[1].v.cn[0] =
        interfaceCtx->beatingHeartVtx[2].v.cn[0] = interfaceCtx->beatingHeartVtx[3].v.cn[0] =
            interfaceCtx->beatingHeartVtx[0].v.cn[1] = interfaceCtx->beatingHeartVtx[1].v.cn[1] =
                interfaceCtx->beatingHeartVtx[2].v.cn[1] = interfaceCtx->beatingHeartVtx[3].v.cn[1] =
                    interfaceCtx->beatingHeartVtx[0].v.cn[2] = interfaceCtx->beatingHeartVtx[1].v.cn[2] =
                        interfaceCtx->beatingHeartVtx[2].v.cn[2] = interfaceCtx->beatingHeartVtx[3].v.cn[2] =
                            interfaceCtx->beatingHeartVtx[0].v.cn[3] = interfaceCtx->beatingHeartVtx[1].v.cn[3] =
                                interfaceCtx->beatingHeartVtx[2].v.cn[3] = interfaceCtx->beatingHeartVtx[3].v.cn[3] =
                                    255;
}

void parameter_perspective_view(PlayState* play, s32 topY, s32 bottomY, s32 leftX, s32 rightX) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    Vec3f eye;
    Vec3f lookAt;
    Vec3f up;

    eye.x = eye.y = eye.z = 0.0f;
    lookAt.x = lookAt.y = 0.0f;
    lookAt.z = -1.0f;
    up.x = up.z = 0.0f;
    up.y = 1.0f;

    setLookAtView(&interfaceCtx->view, &eye, &lookAt, &up);

    interfaceCtx->viewport.topY = topY;
    interfaceCtx->viewport.bottomY = bottomY;
    interfaceCtx->viewport.leftX = leftX;
    interfaceCtx->viewport.rightX = rightX;
    setScissorView(&interfaceCtx->view, &interfaceCtx->viewport);

    setPerspectiveView(&interfaceCtx->view, 60.0f, 10.0f, 60.0f);
    showPerspectiveOverLayView(&interfaceCtx->view);
}

void parameter_view(InterfaceContext* interfaceCtx) {
    SET_FULLSCREEN_VIEWPORT(&interfaceCtx->view);
    showOverLayView(&interfaceCtx->view);
}

void parameter_draw(PlayState* play) {
    static s16 power_r[] = { 255, 100, 255 };
    static s16 power_g[] = { 0, 100, 255 };
    static s16 power_b[] = { 0, 255, 100 };
    static s16 tm_xp[] = { 16, 25, 34, 42, 51 };
    static s16 tm_xsz[] = { 9, 9, 8, 9, 9 };
    // unused, most likely colors
    static s16 navi_flush_color[][3] = {
        { 0, 150, 0 }, { 100, 255, 0 }, { 255, 255, 255 }, { 0, 0, 0 }, { 255, 255, 255 },
    };
    static s16 purse_pt0[] = { 1, 0, 0 };
    static s16 purse_pt1[] = { 2, 3, 3 };
    static s16 Next_scene[] = { ENTR_LOST_WOODS_2, ENTR_ZORAS_DOMAIN_3, ENTR_ZORAS_DOMAIN_3 };
    static f32 a_scale[] = { -40.0f, -35.0f }; // unused
    static s16 b_scale[] = { 91, 91 };         // unused
    static s16 display_tm;
    static s16 frame_tm;
    static s16 total_display_tm;
    static s16 total_frame_tm;
    static s16 etm[5];
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    PauseContext* pauseCtx = &play->pauseCtx;
    MessageContext* msgCtx = &play->msgCtx;
    Player* player = GET_PLAYER(play);
    s16 svar1;
    s16 svar2;
    s16 svar3;
    s16 svar4;
    s16 svar5;
    s16 timerId;

    OPEN_DISPS(play->state.gfxCtx, "../z_parameter.c", 3405);

    gSPSegment(OVERLAY_DISP++, 0x02, interfaceCtx->parameterSegment);
    gSPSegment(OVERLAY_DISP++, 0x07, interfaceCtx->doActionSegment);
    gSPSegment(OVERLAY_DISP++, 0x08, interfaceCtx->iconItemSegment);
    gSPSegment(OVERLAY_DISP++, 0x0B, interfaceCtx->mapSegment);

    if (pauseCtx->debugState == 0) {
        vtx_data_initial(play);
        parameter_view(interfaceCtx);
        DrawLifeMeter(play);

        overlay_rectangle_a_prim(play->state.gfxCtx);

        // Rupee Icon
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 200, 255, 100, interfaceCtx->magicAlpha);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 80, 0, 255);
        OVERLAY_DISP = texture_rectangleIA8(OVERLAY_DISP, gRupeeCounterIconTex, 16, 16, 26, 206, 16, 16, 1 << 10, 1 << 10);

        switch (play->sceneId) {
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
                if (z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex] >= 0) {
                    // Small Key Icon
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 200, 230, 255, interfaceCtx->magicAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 20, 255);
                    OVERLAY_DISP = texture_rectangleIA8(OVERLAY_DISP, gSmallKeyCounterIconTex, 16, 16, 26, 190, 16, 16,
                                                  1 << 10, 1 << 10);

                    // Small Key Counter
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->magicAlpha);
                    gDPSetCombineLERP(OVERLAY_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE,
                                      TEXEL0, 0, PRIMITIVE, 0);

                    interfaceCtx->counterDigits[2] = 0;
                    interfaceCtx->counterDigits[3] =
                        z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex];

                    while (interfaceCtx->counterDigits[3] >= 10) {
                        interfaceCtx->counterDigits[2]++;
                        interfaceCtx->counterDigits[3] -= 10;
                    }

                    svar3 = 42;

                    if (interfaceCtx->counterDigits[2] != 0) {
                        OVERLAY_DISP = texture_rectangleI8(
                            OVERLAY_DISP, ((u8*)gCounterDigit0Tex + (8 * 16 * interfaceCtx->counterDigits[2])), 8, 16,
                            svar3, 190, 8, 16, 1 << 10, 1 << 10);
                        svar3 += 8;
                    }

                    OVERLAY_DISP = texture_rectangleI8(OVERLAY_DISP,
                                                 ((u8*)gCounterDigit0Tex + (8 * 16 * interfaceCtx->counterDigits[3])),
                                                 8, 16, svar3, 190, 8, 16, 1 << 10, 1 << 10);
                }
                break;
            default:
                break;
        }

        // Rupee Counter
        gDPPipeSync(OVERLAY_DISP++);

        if (z_common_data.save.info.playerData.rupees == CUR_CAPACITY(UPG_WALLET)) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 120, 255, 0, interfaceCtx->magicAlpha);
        } else if (z_common_data.save.info.playerData.rupees != 0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->magicAlpha);
        } else {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 100, 100, interfaceCtx->magicAlpha);
        }

        gDPSetCombineLERP(OVERLAY_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                          PRIMITIVE, 0);

        interfaceCtx->counterDigits[0] = interfaceCtx->counterDigits[1] = 0;
        interfaceCtx->counterDigits[2] = z_common_data.save.info.playerData.rupees;

        if ((interfaceCtx->counterDigits[2] > 9999) || (interfaceCtx->counterDigits[2] < 0)) {
            interfaceCtx->counterDigits[2] &= 0xDDD;
        }

        while (interfaceCtx->counterDigits[2] >= 100) {
            interfaceCtx->counterDigits[0]++;
            interfaceCtx->counterDigits[2] -= 100;
        }

        while (interfaceCtx->counterDigits[2] >= 10) {
            interfaceCtx->counterDigits[1]++;
            interfaceCtx->counterDigits[2] -= 10;
        }

        svar2 = purse_pt0[CUR_UPG_VALUE(UPG_WALLET)];
        svar4 = purse_pt1[CUR_UPG_VALUE(UPG_WALLET)];

        for (svar1 = 0, svar3 = 42; svar1 < svar4; svar1++, svar2++, svar3 += 8) {
            OVERLAY_DISP =
                texture_rectangleI8(OVERLAY_DISP, ((u8*)gCounterDigit0Tex + (8 * 16 * interfaceCtx->counterDigits[svar2])), 8,
                              16, svar3, 206, 8, 16, 1 << 10, 1 << 10);
        }

        magic_meter_draw(play);
        map_exp_draw(play);

        if ((R_PAUSE_BG_PRERENDER_STATE != PAUSE_BG_PRERENDER_PROCESS) &&
            (R_PAUSE_BG_PRERENDER_STATE != PAUSE_BG_PRERENDER_READY)) {
            Anchor_Marker_target_draw(&play->actorCtx.attention, play);
        }

        overlay_rectangle_a_prim(play->state.gfxCtx);

        button_background_rectangle_all(play);

        gDPPipeSync(OVERLAY_DISP++);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->bAlpha);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);

        if (!(interfaceCtx->unk_1FA)) {
            // B Button Icon & Ammo Count
            if (z_common_data.save.info.equips.buttonItems[0] != ITEM_NONE) {
                button_item_rectangle(play, interfaceCtx->iconItemSegment, 0);

                if ((player->stateFlags1 & PLAYER_STATE1_23) || (play->shootingGalleryStatus > 1) ||
                    ((play->sceneId == SCENE_BOMBCHU_BOWLING_ALLEY) && Actor_Environment_sw_Check(play, 0x38))) {
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE,
                                      0, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                    count_display_rectangle(play, 0, interfaceCtx->bAlpha);
                }
            }
        } else {
            // B Button Do Action Label
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->bAlpha);

            gDPLoadTextureBlock_4b(OVERLAY_DISP++, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE, G_IM_FMT_IA,
                                   DO_ACTION_TEX_WIDTH, DO_ACTION_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                   G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            R_B_LABEL_DD = (1 << 10) / (R_B_LABEL_SCALE(z_common_data.language) / 100.0f);
            gSPTextureRectangle(OVERLAY_DISP++, R_B_LABEL_X(z_common_data.language) << 2,
                                R_B_LABEL_Y(z_common_data.language) << 2,
                                (R_B_LABEL_X(z_common_data.language) + DO_ACTION_TEX_WIDTH) << 2,
                                (R_B_LABEL_Y(z_common_data.language) + DO_ACTION_TEX_HEIGHT) << 2, G_TX_RENDERTILE, 0, 0,
                                R_B_LABEL_DD, R_B_LABEL_DD);
        }

        gDPPipeSync(OVERLAY_DISP++);

        // C-Left Button Icon & Ammo Count
        if (z_common_data.save.info.equips.buttonItems[1] < 0xF0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->cLeftAlpha);
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            button_item_rectangle(play, interfaceCtx->iconItemSegment + 0x1000, 1);
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            count_display_rectangle(play, 1, interfaceCtx->cLeftAlpha);
        }

        gDPPipeSync(OVERLAY_DISP++);

        // C-Down Button Icon & Ammo Count
        if (z_common_data.save.info.equips.buttonItems[2] < 0xF0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->cDownAlpha);
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            button_item_rectangle(play, interfaceCtx->iconItemSegment + 0x2000, 2);
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            count_display_rectangle(play, 2, interfaceCtx->cDownAlpha);
        }

        gDPPipeSync(OVERLAY_DISP++);

        // C-Right Button Icon & Ammo Count
        if (z_common_data.save.info.equips.buttonItems[3] < 0xF0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->cRightAlpha);
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            button_item_rectangle(play, interfaceCtx->iconItemSegment + 0x3000, 3);
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            count_display_rectangle(play, 3, interfaceCtx->cRightAlpha);
        }

        // A Button
        overlay_kaleido_scope_prim(play->state.gfxCtx);
        parameter_perspective_view(play, R_A_BTN_Y, R_A_BTN_Y + 45, R_A_BTN_X, R_A_BTN_X + 45);
        gSPClearGeometryMode(OVERLAY_DISP++, G_CULL_BOTH);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, R_A_BTN_COLOR(0), R_A_BTN_COLOR(1), R_A_BTN_COLOR(2),
                        interfaceCtx->aAlpha);
        button_background_set(play);
        gDPPipeSync(OVERLAY_DISP++);
        parameter_perspective_view(play, R_A_ICON_Y, R_A_ICON_Y + 45, R_A_ICON_X, R_A_ICON_X + 45);
        gSPSetGeometryMode(OVERLAY_DISP++, G_CULL_BACK);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->aAlpha);
        gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
        Matrix_translate(0.0f, 0.0f, R_A_LABEL_Z(z_common_data.language) / 10.0f, MTXMODE_NEW);
        Matrix_scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_rotateX(interfaceCtx->unk_1F4 / 10000.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(OVERLAY_DISP++, play->state.gfxCtx, "../z_parameter.c", 3701);
        gSPVertex(OVERLAY_DISP++, &interfaceCtx->actionVtx[4], 4, 0);

        if ((interfaceCtx->unk_1EC < 2) || (interfaceCtx->unk_1EC == 3)) {
            do_message_display(play->state.gfxCtx, interfaceCtx->doActionSegment);
        } else {
            do_message_display(play->state.gfxCtx, interfaceCtx->doActionSegment + DO_ACTION_TEX_SIZE);
        }

        gDPPipeSync(OVERLAY_DISP++);

        parameter_view(interfaceCtx);

        if ((pauseCtx->state == PAUSE_STATE_MAIN) && (pauseCtx->mainState == PAUSE_MAIN_STATE_3)) {
            // Inventory Equip Effects
            gSPSegment(OVERLAY_DISP++, 0x08, pauseCtx->iconItemSegment);
            overlay_kaleido_scope_prim(play->state.gfxCtx);
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            gSPMatrix(OVERLAY_DISP++, &Mtx_clear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            // PAUSE_CURSOR_QUAD_4
            pauseCtx->cursorVtx[16].v.ob[0] = pauseCtx->cursorVtx[18].v.ob[0] = pauseCtx->equipAnimX / 10;
            pauseCtx->cursorVtx[17].v.ob[0] = pauseCtx->cursorVtx[19].v.ob[0] =
                pauseCtx->cursorVtx[16].v.ob[0] + WREG(90) / 10;
            pauseCtx->cursorVtx[16].v.ob[1] = pauseCtx->cursorVtx[17].v.ob[1] = pauseCtx->equipAnimY / 10;
            pauseCtx->cursorVtx[18].v.ob[1] = pauseCtx->cursorVtx[19].v.ob[1] =
                pauseCtx->cursorVtx[16].v.ob[1] - WREG(90) / 10;

            if (pauseCtx->equipTargetItem < 0xBF) {
                // Normal Equip (icon goes from the inventory slot to the C button when equipping it)
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, pauseCtx->equipAnimAlpha);
                gSPVertex(OVERLAY_DISP++, &pauseCtx->cursorVtx[PAUSE_CURSOR_QUAD_4 * 4], 4, 0);

                gDPLoadTextureBlock(OVERLAY_DISP++, item_data[pauseCtx->equipTargetItem], G_IM_FMT_RGBA, G_IM_SIZ_32b,
                                    ITEM_ICON_WIDTH, ITEM_ICON_HEIGHT, 0, G_TX_NOMIRROR | G_TX_WRAP,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            } else {
                // Magic Arrow Equip Effect
                svar1 = pauseCtx->equipTargetItem - 0xBF;
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, power_r[svar1], power_g[svar1],
                                power_b[svar1], pauseCtx->equipAnimAlpha);

                if ((pauseCtx->equipAnimAlpha > 0) && (pauseCtx->equipAnimAlpha < 255)) {
                    svar1 = (pauseCtx->equipAnimAlpha / 8) / 2;
                    // PAUSE_CURSOR_QUAD_4
                    pauseCtx->cursorVtx[16].v.ob[0] = pauseCtx->cursorVtx[18].v.ob[0] =
                        pauseCtx->cursorVtx[16].v.ob[0] - svar1;
                    pauseCtx->cursorVtx[17].v.ob[0] = pauseCtx->cursorVtx[19].v.ob[0] =
                        pauseCtx->cursorVtx[16].v.ob[0] + 32 + svar1 * 2;
                    pauseCtx->cursorVtx[16].v.ob[1] = pauseCtx->cursorVtx[17].v.ob[1] =
                        pauseCtx->cursorVtx[16].v.ob[1] + svar1;
                    pauseCtx->cursorVtx[18].v.ob[1] = pauseCtx->cursorVtx[19].v.ob[1] =
                        pauseCtx->cursorVtx[16].v.ob[1] - 32 - svar1 * 2;
                }

                gSPVertex(OVERLAY_DISP++, &pauseCtx->cursorVtx[PAUSE_CURSOR_QUAD_4 * 4], 4, 0);
                gDPLoadTextureBlock(OVERLAY_DISP++, gMagicArrowEquipEffectTex, G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                    G_TX_NOLOD, G_TX_NOLOD);
            }

            gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);
        }

        overlay_rectangle_a_prim(play->state.gfxCtx);

        if (!IS_PAUSED(&play->pauseCtx)) {
            if (z_common_data.minigameState != 1) {
                // Carrots rendering if the action corresponds to riding a horse
                if (interfaceCtx->unk_1EE == 8) {
                    // Load Carrot Icon
                    gDPLoadTextureBlock(OVERLAY_DISP++, gCarrotIconTex, G_IM_FMT_RGBA, G_IM_SIZ_32b, 16, 16, 0,
                                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                        G_TX_NOLOD, G_TX_NOLOD);

                    // Draw 6 carrots
                    for (svar1 = 1, svar5 = ZREG(14); svar1 < 7; svar1++, svar5 += 16) {
                        // Carrot Color (based on availability)
                        if ((interfaceCtx->numHorseBoosts == 0) || (interfaceCtx->numHorseBoosts < svar1)) {
                            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 150, 255, interfaceCtx->aAlpha);
                        } else {
                            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->aAlpha);
                        }

                        gSPTextureRectangle(OVERLAY_DISP++, svar5 << 2, ZREG(15) << 2, (svar5 + 16) << 2,
                                            (ZREG(15) + 16) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
                    }
                }
            } else {
                // Score for the Horseback Archery
                svar5 = WREG(32);
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->bAlpha);

                // Target Icon
                gDPLoadTextureBlock(OVERLAY_DISP++, gArcheryScoreIconTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 24, 16, 0,
                                    G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                    G_TX_NOLOD, G_TX_NOLOD);

                gSPTextureRectangle(OVERLAY_DISP++, (svar5 + 28) << 2, ZREG(15) << 2, (svar5 + 52) << 2,
                                    (ZREG(15) + 16) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);

                // Score Counter
                gDPPipeSync(OVERLAY_DISP++);
                gDPSetCombineLERP(OVERLAY_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE,
                                  TEXEL0, 0, PRIMITIVE, 0);

                svar5 = WREG(32) + 6 * 9;

                for (svar1 = svar2 = 0; svar1 < 4; svar1++) {
                    if (yabu_total[svar1] != 0 || (svar2 != 0) || (svar1 >= 3)) {
                        OVERLAY_DISP = texture_rectangleI8(
                            OVERLAY_DISP, ((u8*)gCounterDigit0Tex + (8 * 16 * yabu_total[svar1])), 8, 16, svar5,
                            (ZREG(15) - 2), tm_xsz[0], VREG(42), VREG(43) << 1, VREG(43) << 1);
                        svar5 += 9;
                        svar2++;
                    }
                }

                gDPPipeSync(OVERLAY_DISP++);
                gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            }
        }

        if ((z_common_data.subTimerState == SUBTIMER_STATE_RESPAWN) &&
            (message_check(&play->msgCtx) == TEXT_STATE_EVENT)) {
            // Trade quest timer reached 0
            total_frame_tm = 40;
            z_common_data.save.cutsceneIndex = 0;
            play->transitionTrigger = TRANS_TRIGGER_START;
            play->transitionType = TRANS_TYPE_FADE_WHITE;
            z_common_data.subTimerState = SUBTIMER_STATE_OFF;

            if ((z_common_data.save.info.equips.buttonItems[0] != ITEM_SWORD_KOKIRI) &&
                (z_common_data.save.info.equips.buttonItems[0] != ITEM_SWORD_MASTER) &&
                (z_common_data.save.info.equips.buttonItems[0] != ITEM_SWORD_BIGGORON) &&
                (z_common_data.save.info.equips.buttonItems[0] != ITEM_GIANTS_KNIFE)) {
                if (z_common_data.buttonStatus[0] != BTN_ENABLED) {
                    z_common_data.save.info.equips.buttonItems[0] = z_common_data.buttonStatus[0];
                } else {
                    z_common_data.save.info.equips.buttonItems[0] = ITEM_NONE;
                }
            }

            // Revert any spoiling trade quest items
            for (svar1 = 0; svar1 < ARRAY_COUNT(Warashibe_item); svar1++) {
                if (INV_CONTENT(ITEM_TRADE_ADULT) == Warashibe_item[svar1]) {
#if OOT_VERSION >= NTSC_1_1
                    z_common_data.eventInf[EVENTINF_INDEX_HORSES] &=
                        (u16) ~(EVENTINF_INGO_RACE_STATE_MASK | EVENTINF_MASK(EVENTINF_INGO_RACE_HORSETYPE) |
                                EVENTINF_MASK(EVENTINF_INGO_RACE_LOST_ONCE) |
                                EVENTINF_MASK(EVENTINF_INGO_RACE_SECOND_RACE) | EVENTINF_MASK(EVENTINF_INGO_RACE_0F));
                    PRINTF("EVENT_INF=%x\n", z_common_data.eventInf[EVENTINF_INDEX_HORSES]);
#endif
                    play->nextEntranceIndex = Next_scene[svar1];
                    INV_CONTENT(Warashibe_before_item[svar1]) = Warashibe_before_item[svar1];

                    for (svar2 = 1; svar2 < 4; svar2++) {
                        if (z_common_data.save.info.equips.buttonItems[svar2] == Warashibe_item[svar1]) {
                            z_common_data.save.info.equips.buttonItems[svar2] = Warashibe_before_item[svar1];
                            item_textuer_dma(play, svar2);
                        }
                    }
                }
            }
        }

        if (!IS_PAUSED(&play->pauseCtx) && (play->gameOverCtx.state == GAMEOVER_INACTIVE) &&
            (msgCtx->msgMode == MSGMODE_NONE) && !(player->stateFlags2 & PLAYER_STATE2_24) &&
            (play->transitionTrigger == TRANS_TRIGGER_OFF) && (play->transitionMode == TRANS_MODE_OFF) &&
            !Game_play_demo_mode_check(play) && (z_common_data.minigameState != 1) && (play->shootingGalleryStatus <= 1) &&
            !((play->sceneId == SCENE_BOMBCHU_BOWLING_ALLEY) && Actor_Environment_sw_Check(play, 0x38))) {

            timerId = TIMER_ID_MAIN;

            switch (z_common_data.timerState) {
                case TIMER_STATE_ENV_HAZARD_INIT:
                    frame_tm = 20;
                    display_tm = 20;
                    z_common_data.timerSeconds = z_common_data.save.info.playerData.health >> 1;
                    z_common_data.timerState = TIMER_STATE_ENV_HAZARD_PREVIEW;
                    break;

                case TIMER_STATE_ENV_HAZARD_PREVIEW:
                    frame_tm--;
                    if (frame_tm == 0) {
                        frame_tm = 20;
                        z_common_data.timerState = TIMER_STATE_ENV_HAZARD_MOVE;
                    }
                    break;

                case TIMER_STATE_DOWN_INIT:
                case TIMER_STATE_UP_INIT:
                    frame_tm = 20;
                    display_tm = 20;
                    if (z_common_data.timerState == TIMER_STATE_DOWN_INIT) {
                        z_common_data.timerState = TIMER_STATE_DOWN_PREVIEW;
                    } else {
                        z_common_data.timerState = TIMER_STATE_UP_PREVIEW;
                    }
                    break;

                case TIMER_STATE_DOWN_PREVIEW:
                case TIMER_STATE_UP_PREVIEW:
                    frame_tm--;
                    if (frame_tm == 0) {
                        frame_tm = 20;
                        if (z_common_data.timerState == TIMER_STATE_DOWN_PREVIEW) {
                            z_common_data.timerState = TIMER_STATE_DOWN_MOVE;
                        } else {
                            z_common_data.timerState = TIMER_STATE_UP_MOVE;
                        }
                    }
                    break;

                case TIMER_STATE_ENV_HAZARD_MOVE:
                case TIMER_STATE_DOWN_MOVE:
                    svar1 = (z_common_data.timerX[TIMER_ID_MAIN] - 26) / frame_tm;
                    z_common_data.timerX[TIMER_ID_MAIN] -= svar1;

                    if (z_common_data.save.info.playerData.healthCapacity > 0xA0) {
                        svar1 = (z_common_data.timerY[TIMER_ID_MAIN] - 54) / frame_tm; // two rows of hearts
                    } else {
                        svar1 = (z_common_data.timerY[TIMER_ID_MAIN] - 46) / frame_tm; // one row of hearts
                    }
                    z_common_data.timerY[TIMER_ID_MAIN] -= svar1;

                    frame_tm--;
                    if (frame_tm == 0) {
                        frame_tm = 20;
                        z_common_data.timerX[TIMER_ID_MAIN] = 26;

                        if (z_common_data.save.info.playerData.healthCapacity > 0xA0) {
                            z_common_data.timerY[TIMER_ID_MAIN] = 54; // two rows of hearts
                        } else {
                            z_common_data.timerY[TIMER_ID_MAIN] = 46; // one row of hearts
                        }

                        if (z_common_data.timerState == TIMER_STATE_ENV_HAZARD_MOVE) {
                            z_common_data.timerState = TIMER_STATE_ENV_HAZARD_TICK;
                        } else {
                            z_common_data.timerState = TIMER_STATE_DOWN_TICK;
                        }
                    }
                    FALLTHROUGH;
                case TIMER_STATE_ENV_HAZARD_TICK:
                case TIMER_STATE_DOWN_TICK:
                    if ((z_common_data.timerState == TIMER_STATE_ENV_HAZARD_TICK) ||
                        (z_common_data.timerState == TIMER_STATE_DOWN_TICK)) {
                        if (z_common_data.save.info.playerData.healthCapacity > 0xA0) {
                            z_common_data.timerY[TIMER_ID_MAIN] = 54; // two rows of hearts
                        } else {
                            z_common_data.timerY[TIMER_ID_MAIN] = 46; // one row of hearts
                        }
                    }

                    if ((z_common_data.timerState >= TIMER_STATE_ENV_HAZARD_MOVE) && (msgCtx->msgLength == 0)) {
                        if (--display_tm == 0) {
                            if (z_common_data.timerSeconds != 0) {
                                z_common_data.timerSeconds--;
                            }

                            display_tm = 20;

                            if (z_common_data.timerSeconds == 0) {
                                // Out of time
                                z_common_data.timerState = TIMER_STATE_STOP;
                                if (player_diving) {
                                    z_common_data.save.info.playerData.health = 0;
                                    play->damagePlayer(play, -(z_common_data.save.info.playerData.health + 2));
                                }
                                player_diving = false;
                            } else if (z_common_data.timerSeconds > 60) {
                                // Beep at "xx:x1" (every 10 seconds)
                                if (etm[4] == 1) {
                                    Nai_FxFlagEntry(NA_SE_SY_MESSAGE_WOMAN, &_dummy_zero_f, 4,
                                                         &_dummy_one, &_dummy_one,
                                                         &_dummy_zero_s8);
                                }
                            } else if (z_common_data.timerSeconds > 10) {
                                // Beep on alternating seconds
                                if ((etm[4] % 2) != 0) {
                                    Nai_FxFlagEntry(NA_SE_SY_WARNING_COUNT_N, &_dummy_zero_f, 4,
                                                         &_dummy_one, &_dummy_one,
                                                         &_dummy_zero_s8);
                                }
                            } else {
                                // Beep every second
                                Nai_FxFlagEntry(NA_SE_SY_WARNING_COUNT_E, &_dummy_zero_f, 4,
                                                     &_dummy_one, &_dummy_one,
                                                     &_dummy_zero_s8);
                            }
                        }
                    }
                    break;

                case TIMER_STATE_UP_MOVE:
                    svar1 = (z_common_data.timerX[TIMER_ID_MAIN] - 26) / frame_tm;
                    z_common_data.timerX[TIMER_ID_MAIN] -= svar1;

                    if (z_common_data.save.info.playerData.healthCapacity > 0xA0) {
                        svar1 = (z_common_data.timerY[TIMER_ID_MAIN] - 54) / frame_tm; // two rows of hearts
                    } else {
                        svar1 = (z_common_data.timerY[TIMER_ID_MAIN] - 46) / frame_tm; // one row of hearts
                    }
                    z_common_data.timerY[TIMER_ID_MAIN] -= svar1;

                    frame_tm--;
                    if (frame_tm == 0) {
                        frame_tm = 20;
                        z_common_data.timerX[TIMER_ID_MAIN] = 26;
                        if (z_common_data.save.info.playerData.healthCapacity > 0xA0) {
                            z_common_data.timerY[TIMER_ID_MAIN] = 54; // two rows of hearts
                        } else {
                            z_common_data.timerY[TIMER_ID_MAIN] = 46; // one row of hearts
                        }

                        z_common_data.timerState = TIMER_STATE_UP_TICK;
                    }
                    FALLTHROUGH;
                case TIMER_STATE_UP_TICK:
                    if (z_common_data.timerState == TIMER_STATE_UP_TICK) {
                        if (z_common_data.save.info.playerData.healthCapacity > 0xA0) {
                            z_common_data.timerY[TIMER_ID_MAIN] = 54; // two rows of hearts
                        } else {
                            z_common_data.timerY[TIMER_ID_MAIN] = 46; // one row of hearts
                        }
                    }

                    if (z_common_data.timerState >= TIMER_STATE_ENV_HAZARD_MOVE) {
                        display_tm--;
                        if (display_tm == 0) {
                            z_common_data.timerSeconds++;
                            display_tm = 20;

                            if (z_common_data.timerSeconds == 3599) { // 59 minutes, 59 seconds
                                frame_tm = 40;
                                z_common_data.timerState = TIMER_STATE_UP_FREEZE;
                            } else {
                                Nai_FxFlagEntry(NA_SE_SY_WARNING_COUNT_N, &_dummy_zero_f, 4,
                                                     &_dummy_one, &_dummy_one,
                                                     &_dummy_zero_s8);
                            }
                        }
                    }
                    break;

                case TIMER_STATE_STOP:
                    if (z_common_data.subTimerState != SUBTIMER_STATE_OFF) {
                        total_frame_tm = 20;
                        total_display_tm = 20;
                        z_common_data.timerX[TIMER_ID_SUB] = 140;
                        z_common_data.timerY[TIMER_ID_SUB] = 80;

                        if (z_common_data.subTimerState <= SUBTIMER_STATE_STOP) {
                            z_common_data.subTimerState = SUBTIMER_STATE_DOWN_PREVIEW;
                        } else {
                            z_common_data.subTimerState = SUBTIMER_STATE_UP_PREVIEW;
                        }

                        z_common_data.timerState = TIMER_STATE_OFF;
                    } else {
                        z_common_data.timerState = TIMER_STATE_OFF;
                    }
                    FALLTHROUGH;
                case TIMER_STATE_UP_FREEZE:
                    break;

                default: // TIMER_STATE_OFF
                    // Process the subTimer only if the main timer is off
                    timerId = TIMER_ID_SUB;

                    switch (z_common_data.subTimerState) {
                        case SUBTIMER_STATE_DOWN_INIT:
                        case SUBTIMER_STATE_UP_INIT:
                            total_frame_tm = 20;
                            total_display_tm = 20;
                            z_common_data.timerX[TIMER_ID_SUB] = 140;
                            z_common_data.timerY[TIMER_ID_SUB] = 80;
                            if (z_common_data.subTimerState == SUBTIMER_STATE_DOWN_INIT) {
                                z_common_data.subTimerState = SUBTIMER_STATE_DOWN_PREVIEW;
                            } else {
                                z_common_data.subTimerState = SUBTIMER_STATE_UP_PREVIEW;
                            }
                            break;

                        case SUBTIMER_STATE_DOWN_PREVIEW:
                        case SUBTIMER_STATE_UP_PREVIEW:
                            total_frame_tm--;
                            if (total_frame_tm == 0) {
                                total_frame_tm = 20;
                                if (z_common_data.subTimerState == SUBTIMER_STATE_DOWN_PREVIEW) {
                                    z_common_data.subTimerState = SUBTIMER_STATE_DOWN_MOVE;
                                } else {
                                    z_common_data.subTimerState = SUBTIMER_STATE_UP_MOVE;
                                }
                            }
                            break;

                        case SUBTIMER_STATE_DOWN_MOVE:
                        case SUBTIMER_STATE_UP_MOVE:
                            PRINTF("event_xp[1]=%d,  event_yp[1]=%d  TOTAL_EVENT_TM=%d\n",
                                   ((void)0, z_common_data.timerX[TIMER_ID_SUB]),
                                   ((void)0, z_common_data.timerY[TIMER_ID_SUB]), z_common_data.subTimerSeconds);
                            svar1 = (z_common_data.timerX[TIMER_ID_SUB] - 26) / total_frame_tm;
                            z_common_data.timerX[TIMER_ID_SUB] -= svar1;
                            if (z_common_data.save.info.playerData.healthCapacity > 0xA0) {
                                // two rows of hearts
                                svar1 = (z_common_data.timerY[TIMER_ID_SUB] - 54) / total_frame_tm;
                            } else {
                                // one row of hearts
                                svar1 = (z_common_data.timerY[TIMER_ID_SUB] - 46) / total_frame_tm;
                            }
                            z_common_data.timerY[TIMER_ID_SUB] -= svar1;

                            total_frame_tm--;
                            if (total_frame_tm == 0) {
                                total_frame_tm = 20;
                                z_common_data.timerX[TIMER_ID_SUB] = 26;

                                if (z_common_data.save.info.playerData.healthCapacity > 0xA0) {
                                    z_common_data.timerY[TIMER_ID_SUB] = 54; // two rows of hearts
                                } else {
                                    z_common_data.timerY[TIMER_ID_SUB] = 46; // one row of hearts
                                }

                                if (z_common_data.subTimerState == SUBTIMER_STATE_DOWN_MOVE) {
                                    z_common_data.subTimerState = SUBTIMER_STATE_DOWN_TICK;
                                } else {
                                    z_common_data.subTimerState = SUBTIMER_STATE_UP_TICK;
                                }
                            }
                            FALLTHROUGH;
                        case SUBTIMER_STATE_DOWN_TICK:
                        case SUBTIMER_STATE_UP_TICK:
                            if ((z_common_data.subTimerState == SUBTIMER_STATE_DOWN_TICK) ||
                                (z_common_data.subTimerState == SUBTIMER_STATE_UP_TICK)) {
                                if (z_common_data.save.info.playerData.healthCapacity > 0xA0) {
                                    z_common_data.timerY[TIMER_ID_SUB] = 54; // two rows of hearts
                                } else {
                                    z_common_data.timerY[TIMER_ID_SUB] = 46; // one row of hearts
                                }
                            }

                            if (z_common_data.subTimerState >= SUBTIMER_STATE_DOWN_MOVE) {
                                total_display_tm--;
                                if (total_display_tm == 0) {
                                    total_display_tm = 20;
                                    if (z_common_data.subTimerState == SUBTIMER_STATE_DOWN_TICK) {
                                        z_common_data.subTimerSeconds--;
                                        PRINTF("TOTAL_EVENT_TM=%d\n", z_common_data.subTimerSeconds);

#if OOT_VERSION < PAL_1_0
                                        if (z_common_data.subTimerSeconds == 0)
#else
                                        if (z_common_data.subTimerSeconds <= 0)
#endif
                                        {
                                            // Out of time
                                            if (!Actor_Environment_sw_Check(play, 0x37) ||
                                                ((play->sceneId != SCENE_GANON_BOSS) &&
                                                 (play->sceneId != SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) &&
                                                 (play->sceneId != SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) &&
                                                 (play->sceneId != SCENE_INSIDE_GANONS_CASTLE_COLLAPSE))) {
                                                total_frame_tm = 40;
                                                z_common_data.subTimerState = SUBTIMER_STATE_RESPAWN;
                                                z_common_data.save.cutsceneIndex = 0;
                                                message_set(play, 0x71B0, NULL);
                                                player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
                                            } else {
                                                total_frame_tm = 40;
                                                z_common_data.subTimerState = SUBTIMER_STATE_STOP;
                                            }
                                        } else if (z_common_data.subTimerSeconds > 60) {
                                            // Beep at "xx:x1" (every 10 seconds)
                                            if (etm[4] == 1) {
                                                Nai_FxFlagEntry(NA_SE_SY_MESSAGE_WOMAN, &_dummy_zero_f, 4,
                                                                     &_dummy_one,
                                                                     &_dummy_one, &_dummy_zero_s8);
                                            }
                                        } else if (z_common_data.subTimerSeconds > 10) {
                                            // Beep on alternating seconds
                                            if ((etm[4] % 2) != 0) {
                                                Nai_FxFlagEntry(NA_SE_SY_WARNING_COUNT_N, &_dummy_zero_f, 4,
                                                                     &_dummy_one,
                                                                     &_dummy_one, &_dummy_zero_s8);
                                            }
                                        } else {
                                            // Beep every second
                                            Nai_FxFlagEntry(NA_SE_SY_WARNING_COUNT_E, &_dummy_zero_f, 4,
                                                                 &_dummy_one,
                                                                 &_dummy_one, &_dummy_zero_s8);
                                        }
                                    } else { // SUBTIMER_STATE_UP_TICK
                                        z_common_data.subTimerSeconds++;

                                        // Special case for the running-man race
                                        if (GET_EVENTINF(EVENTINF_MARATHON_ACTIVE) &&
                                            (z_common_data.subTimerSeconds == MARATHON_TIME_LIMIT)) {
                                            // After 4 minutes, cancel the timer
                                            message_set(play, 0x6083, NULL);
                                            CLEAR_EVENTINF(EVENTINF_MARATHON_ACTIVE);
                                            z_common_data.subTimerState = SUBTIMER_STATE_OFF;
                                        }
                                    }

                                    // Beep at the minute mark
                                    if ((z_common_data.subTimerSeconds % 60) == 0) {
                                        Nai_FxFlagEntry(NA_SE_SY_WARNING_COUNT_N, &_dummy_zero_f, 4,
                                                             &_dummy_one, &_dummy_one,
                                                             &_dummy_zero_s8);
                                    }
                                }
                            }
                            break;

                        case SUBTIMER_STATE_STOP:
                            total_frame_tm--;
                            if (total_frame_tm == 0) {
                                z_common_data.subTimerState = SUBTIMER_STATE_OFF;
                            }
                            break;
                    }
                    break;
            }

            if (((z_common_data.timerState != TIMER_STATE_OFF) && (z_common_data.timerState != TIMER_STATE_STOP)) ||
                (z_common_data.subTimerState != SUBTIMER_STATE_OFF)) {
                etm[0] = etm[1] = etm[3] = 0;
                etm[2] = 10; // digit 10 is used as ':' (colon)

                if (z_common_data.timerState != TIMER_STATE_OFF) {
                    etm[4] = z_common_data.timerSeconds;
                } else {
                    etm[4] = z_common_data.subTimerSeconds;
                }

                while (etm[4] >= 60) {
                    etm[1]++;
                    if (etm[1] >= 10) {
                        etm[0]++;
                        etm[1] -= 10;
                    }
                    etm[4] -= 60;
                }

                while (etm[4] >= 10) {
                    etm[3]++;
                    etm[4] -= 10;
                }

                // Clock Icon
                gDPPipeSync(OVERLAY_DISP++);
                gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, 255);
                gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
                OVERLAY_DISP =
                    texture_rectangleIA8(OVERLAY_DISP, gClockIconTex, 16, 16, ((void)0, z_common_data.timerX[timerId]),
                                   ((void)0, z_common_data.timerY[timerId]) + 2, 16, 16, 1 << 10, 1 << 10);

                // Timer Counter
                gDPPipeSync(OVERLAY_DISP++);
                gDPSetCombineLERP(OVERLAY_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE,
                                  TEXEL0, 0, PRIMITIVE, 0);

                if (z_common_data.timerState != TIMER_STATE_OFF) {
                    // TIMER_ID_MAIN
                    if ((z_common_data.timerSeconds < 10) && (z_common_data.timerState <= TIMER_STATE_STOP)) {
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 50, 0, 255);
                    } else {
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, 255);
                    }
                } else {
                    // TIMER_ID_SUB
                    if ((z_common_data.subTimerSeconds < 10) && (z_common_data.subTimerState <= SUBTIMER_STATE_RESPAWN)) {
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 50, 0, 255);
                    } else {
                        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 0, 255);
                    }
                }

                for (svar1 = 0; svar1 < ARRAY_COUNT(etm); svar1++) {
                    OVERLAY_DISP =
                        texture_rectangleI8(OVERLAY_DISP, ((u8*)gCounterDigit0Tex + (8 * 16 * etm[svar1])), 8, 16,
                                      ((void)0, z_common_data.timerX[timerId]) + tm_xp[svar1],
                                      ((void)0, z_common_data.timerY[timerId]), tm_xsz[svar1], VREG(42),
                                      VREG(43) << 1, VREG(43) << 1);
                }
            }
        }
    }

#if DEBUG_FEATURES
    if (pauseCtx->debugState == 3) {
        flg_set(play);
    }
#endif

    if (interfaceCtx->unk_244 != 0) {
        gDPPipeSync(OVERLAY_DISP++);
        gSPDisplayList(OVERLAY_DISP++, fbdemo_fade_gfx_init);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, interfaceCtx->unk_244);
        gDPFillRectangle(OVERLAY_DISP++, 0, 0, ScreenWidth - 1, ScreenHeight - 1);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_parameter.c", 4269);
}

void parameter_move(PlayState* play) {
    static u8 zelda_time = false;
    static s16 keep_zelda_time = 0;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    MessageContext* msgCtx = &play->msgCtx;
    Player* player = GET_PLAYER(play);
    s16 dimmingAlpha;
    s16 risingAlpha;
    u16 action;

#if OOT_PAL && DEBUG_FEATURES
    {
        Input* debugInput = &play->state.input[2];

        if (CHECK_BTN_ALL(debugInput->press.button, BTN_DLEFT)) {
            z_common_data.language = LANGUAGE_ENG;
            PRINTF("J_N=%x J_N=%x\n", z_common_data.language, &z_common_data.language);
        } else if (CHECK_BTN_ALL(debugInput->press.button, BTN_DUP)) {
            z_common_data.language = LANGUAGE_GER;
            PRINTF("J_N=%x J_N=%x\n", z_common_data.language, &z_common_data.language);
        } else if (CHECK_BTN_ALL(debugInput->press.button, BTN_DRIGHT)) {
            z_common_data.language = LANGUAGE_FRA;
            PRINTF("J_N=%x J_N=%x\n", z_common_data.language, &z_common_data.language);
        }
    }
#endif

    if (!IS_PAUSED(&play->pauseCtx)) {
        if ((z_common_data.minigameState == 1) || !IS_CUTSCENE_LAYER ||
            ((play->sceneId == SCENE_LON_LON_RANCH) && (z_common_data.sceneLayer == 4))) {
            if ((msgCtx->msgMode == MSGMODE_NONE) ||
                ((msgCtx->msgMode != MSGMODE_NONE) && (play->sceneId == SCENE_BOMBCHU_BOWLING_ALLEY))) {
                if (play->gameOverCtx.state == GAMEOVER_INACTIVE) {
                    parameter_check(play);
                }
            }
        }
    }

    // Update hud visibility mode
    switch (z_common_data.nextHudVisibilityMode) {
        case HUD_VISIBILITY_NOTHING:
        case HUD_VISIBILITY_NOTHING_ALT:
        case HUD_VISIBILITY_HEARTS_FORCE:
        case HUD_VISIBILITY_A:
        case HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE:
        case HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP_FORCE:
        case HUD_VISIBILITY_ALL_NO_MINIMAP_BY_BTN_STATUS:
        case HUD_VISIBILITY_B:
        case HUD_VISIBILITY_HEARTS_MAGIC:
        case HUD_VISIBILITY_B_ALT:
        case HUD_VISIBILITY_HEARTS:
        case HUD_VISIBILITY_A_B_MINIMAP:
        case HUD_VISIBILITY_HEARTS_MAGIC_FORCE:
            dimmingAlpha = 255 - (32 * z_common_data.hudVisibilityModeTimer);
            if (dimmingAlpha < 0) {
                dimmingAlpha = 0;
            }

            alpha_setting0(play, dimmingAlpha);
            z_common_data.hudVisibilityModeTimer++;

            if (dimmingAlpha == 0) {
                z_common_data.nextHudVisibilityMode = HUD_VISIBILITY_NO_CHANGE;
            }
            break;

        case HUD_VISIBILITY_ALL:
            dimmingAlpha = 255 - (32 * z_common_data.hudVisibilityModeTimer);
            if (dimmingAlpha < 0) {
                dimmingAlpha = 0;
            }

            risingAlpha = 255 - dimmingAlpha;
            if (risingAlpha >= 255) {
                risingAlpha = 255;
            }

            PRINTF("case 50 : alpha=%d  alpha1=%d\n", dimmingAlpha, risingAlpha);

            c_alpha_set_1(play, risingAlpha);

            if (interfaceCtx->healthAlpha != 255) {
                interfaceCtx->healthAlpha = risingAlpha;
            }

            if (interfaceCtx->magicAlpha != 255) {
                interfaceCtx->magicAlpha = risingAlpha;
            }

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
                    if (interfaceCtx->minimapAlpha < 170) {
                        interfaceCtx->minimapAlpha = risingAlpha;
                    } else {
                        interfaceCtx->minimapAlpha = 170;
                    }
                    break;

                default:
                    if (interfaceCtx->minimapAlpha != 255) {
                        interfaceCtx->minimapAlpha = risingAlpha;
                    }
                    break;
            }

            z_common_data.hudVisibilityModeTimer++;
            if (risingAlpha == 255) {
                z_common_data.nextHudVisibilityMode = HUD_VISIBILITY_NO_CHANGE;
            }

            break;

        case HUD_VISIBILITY_NOTHING_INSTANT:
            // Hide the HUD immediately
            z_common_data.nextHudVisibilityMode = HUD_VISIBILITY_NOTHING;
            alpha_setting0(play, 0);
            z_common_data.nextHudVisibilityMode = HUD_VISIBILITY_NO_CHANGE;
            FALLTHROUGH;
        default:
            break;
    }

    map_exp_move(play);

    if (z_common_data.healthAccumulator != 0) {
        z_common_data.healthAccumulator -= 4;
        z_common_data.save.info.playerData.health += 4;

        if ((z_common_data.save.info.playerData.health & 0xF) < 4) {
            Nai_FxFlagEntry(NA_SE_SY_HP_RECOVER, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }

        PRINTF("now_life=%d  max_life=%d\n", z_common_data.save.info.playerData.health,
               z_common_data.save.info.playerData.healthCapacity);

        if (z_common_data.save.info.playerData.health >= z_common_data.save.info.playerData.healthCapacity) {
            z_common_data.save.info.playerData.health = z_common_data.save.info.playerData.healthCapacity;
            PRINTF("S_Private.now_life=%d  S_Private.max_life=%d\n", z_common_data.save.info.playerData.health,
                   z_common_data.save.info.playerData.healthCapacity);
            z_common_data.healthAccumulator = 0;
        }
    }

    LifeMeterZoom(play);
    player_condition = player_condition_check(play);

    if (player_condition == PLAYER_ENV_HAZARD_HOTROOM) {
        if (CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC) == EQUIP_VALUE_TUNIC_GORON) {
            player_condition = PLAYER_ENV_HAZARD_NONE;
        }
    } else if ((player_condition_check(play) >= PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) &&
               (player_condition_check(play) <= PLAYER_ENV_HAZARD_UNDERWATER_FREE)) {
        if (CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC) == EQUIP_VALUE_TUNIC_ZORA) {
            player_condition = PLAYER_ENV_HAZARD_NONE;
        }
    }

    LifeMeterColorAnimation(play);

    if ((z_common_data.timerState >= TIMER_STATE_ENV_HAZARD_MOVE) && !IS_PAUSED(&play->pauseCtx) &&
        (msgCtx->msgMode == MSGMODE_NONE) && !(player->stateFlags2 & PLAYER_STATE2_24) &&
        (play->transitionTrigger == TRANS_TRIGGER_OFF) && (play->transitionMode == TRANS_MODE_OFF) &&
        !Game_play_demo_mode_check(play)) {}

    if (z_common_data.rupeeAccumulator != 0) {
        if (z_common_data.rupeeAccumulator > 0) {
            if (z_common_data.save.info.playerData.rupees < CUR_CAPACITY(UPG_WALLET)) {
                z_common_data.rupeeAccumulator--;
                z_common_data.save.info.playerData.rupees++;
                Nai_FxFlagEntry(NA_SE_SY_RUPY_COUNT, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            } else {
                PRINTF(T("ルピー数ＭＡＸ = %d\n", "Rupee Amount MAX = %d\n"), CUR_CAPACITY(UPG_WALLET));
                z_common_data.save.info.playerData.rupees = CUR_CAPACITY(UPG_WALLET);
                z_common_data.rupeeAccumulator = 0;
            }
        } else if (z_common_data.save.info.playerData.rupees != 0) {
            if (z_common_data.rupeeAccumulator <= -50) {
                z_common_data.rupeeAccumulator += 10;
                z_common_data.save.info.playerData.rupees -= 10;

                if (z_common_data.save.info.playerData.rupees < 0) {
                    z_common_data.save.info.playerData.rupees = 0;
                }

                Nai_FxFlagEntry(NA_SE_SY_RUPY_COUNT, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            } else {
                z_common_data.rupeeAccumulator++;
                z_common_data.save.info.playerData.rupees--;
                Nai_FxFlagEntry(NA_SE_SY_RUPY_COUNT, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
        } else {
            z_common_data.rupeeAccumulator = 0;
        }
    }

    switch (interfaceCtx->unk_1EC) {
        case 1:
            interfaceCtx->unk_1F4 += 31400.0f / WREG(5);
            if (interfaceCtx->unk_1F4 >= 15700.0f) {
                interfaceCtx->unk_1F4 = -15700.0f;
                interfaceCtx->unk_1EC = 2;
            }
            break;
        case 2:
            interfaceCtx->unk_1F4 += 31400.0f / WREG(5);
            if (interfaceCtx->unk_1F4 >= 0.0f) {
                interfaceCtx->unk_1F4 = 0.0f;
                interfaceCtx->unk_1EC = 0;
                interfaceCtx->unk_1EE = interfaceCtx->unk_1F0;
                action = interfaceCtx->unk_1EE;
                if ((action == DO_ACTION_MAX) || (action == DO_ACTION_MAX + 1)) {
                    action = DO_ACTION_NONE;
                }
                next_do_action_textuer_dma(interfaceCtx, action, 0);
            }
            break;
        case 3:
            interfaceCtx->unk_1F4 += 31400.0f / WREG(5);
            if (interfaceCtx->unk_1F4 >= 15700.0f) {
                interfaceCtx->unk_1F4 = -15700.0f;
                interfaceCtx->unk_1EC = 2;
            }
            break;
        case 4:
            interfaceCtx->unk_1F4 += 31400.0f / WREG(5);
            if (interfaceCtx->unk_1F4 >= 0.0f) {
                interfaceCtx->unk_1F4 = 0.0f;
                interfaceCtx->unk_1EC = 0;
                interfaceCtx->unk_1EE = interfaceCtx->unk_1F0;
                action = interfaceCtx->unk_1EE;
                if ((action == DO_ACTION_MAX) || (action == DO_ACTION_MAX + 1)) {
                    action = DO_ACTION_NONE;
                }
                next_do_action_textuer_dma(interfaceCtx, action, 0);
            }
            break;
    }

    WREG(7) = interfaceCtx->unk_1F4;

    // Update Magic
    if (!IS_PAUSED(&play->pauseCtx) && (msgCtx->msgMode == MSGMODE_NONE) &&
        (play->transitionTrigger == TRANS_TRIGGER_OFF) && (play->gameOverCtx.state == GAMEOVER_INACTIVE) &&
        (play->transitionMode == TRANS_MODE_OFF) && ((play->csCtx.state == CS_STATE_IDLE) || !player_demo_check(play))) {

        if (z_common_data.save.info.playerData.isMagicAcquired && (z_common_data.save.info.playerData.magicLevel == 0)) {
            z_common_data.save.info.playerData.magicLevel = z_common_data.save.info.playerData.isDoubleMagicAcquired + 1;
            z_common_data.magicState = MAGIC_STATE_STEP_CAPACITY;
            PRINTF_COLOR_YELLOW();
            PRINTF(T("魔法スター─────ト！！！！！！！！！\n", "Magic Start!!!!!!!!!\n"));
            PRINTF("MAGIC_MAX=%d\n", z_common_data.save.info.playerData.magicLevel);
            PRINTF("MAGIC_NOW=%d\n", z_common_data.save.info.playerData.magic);
            PRINTF("Z_MAGIC_NOW_NOW=%d\n", z_common_data.magicFillTarget);
            PRINTF("Z_MAGIC_NOW_MAX=%d\n", z_common_data.magicCapacity);
            PRINTF_RST();
        }

        magic_meter_move(play);
    }

    if (z_common_data.timerState == TIMER_STATE_OFF) {
        if (((player_condition == PLAYER_ENV_HAZARD_HOTROOM) || (player_condition == PLAYER_ENV_HAZARD_UNDERWATER_FLOOR) ||
             (player_condition == PLAYER_ENV_HAZARD_UNDERWATER_FREE)) &&

            ((z_common_data.save.info.playerData.health >> 1) != 0)) {
            z_common_data.timerState = TIMER_STATE_ENV_HAZARD_INIT;
            z_common_data.timerX[TIMER_ID_MAIN] = 140;
            z_common_data.timerY[TIMER_ID_MAIN] = 80;
            player_diving = true;
        }
    } else {
        if (((player_condition == PLAYER_ENV_HAZARD_NONE) || (player_condition == PLAYER_ENV_HAZARD_SWIMMING)) &&
            (z_common_data.timerState <= TIMER_STATE_ENV_HAZARD_TICK)) {
            z_common_data.timerState = TIMER_STATE_OFF;
        }
    }

    if (z_common_data.minigameState == 1) {
        z_common_data.minigameScore += interfaceCtx->unk_23C;
        interfaceCtx->unk_23C = 0;

        if (yabu_sound == 0) {
            if (z_common_data.minigameScore >= 1000) {
                yabu_sound++;
            }
        } else if (yabu_sound == 1) {
            if (z_common_data.minigameScore >= 1500) {
                yabu_sound++;
            }
        }

        yabu_total[0] = yabu_total[1] = 0;
        yabu_total[2] = 0;
        yabu_total[3] = z_common_data.minigameScore;

        while (yabu_total[3] >= 1000) {
            yabu_total[0]++;
            yabu_total[3] -= 1000;
        }

        while (yabu_total[3] >= 100) {
            yabu_total[1]++;
            yabu_total[3] -= 100;
        }

        while (yabu_total[3] >= 10) {
            yabu_total[2]++;
            yabu_total[3] -= 10;
        }
    }

    if (z_common_data.sunsSongState != SUNSSONG_INACTIVE) {
        // exit out of ocarina mode after suns song finishes playing
        if ((msgCtx->ocarinaAction != OCARINA_ACTION_CHECK_NOWARP_DONE) &&
            (z_common_data.sunsSongState == SUNSSONG_START)) {
            play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        }

        // handle suns song in areas where time moves
        if (play->envCtx.sceneTimeSpeed != 0) {
            if (z_common_data.sunsSongState != SUNSSONG_SPEED_TIME) {
                zelda_time = false;
                if ((z_common_data.save.dayTime >= CLOCK_TIME(6, 30)) &&
                    (z_common_data.save.dayTime <= CLOCK_TIME(18, 0) + 1)) {
                    zelda_time = true;
                }

                z_common_data.sunsSongState = SUNSSONG_SPEED_TIME;
                keep_zelda_time = E_day_time_plus;
                E_day_time_plus = 400;
            } else if (!zelda_time) {
                if ((z_common_data.save.dayTime >= CLOCK_TIME(6, 30)) &&
                    (z_common_data.save.dayTime <= CLOCK_TIME(18, 0) + 1)) {
                    z_common_data.sunsSongState = SUNSSONG_INACTIVE;
                    E_day_time_plus = keep_zelda_time;
                    play->msgCtx.ocarinaMode = OCARINA_MODE_04;
                }
            } else if (z_common_data.save.dayTime > CLOCK_TIME(18, 0) + 1) {
                z_common_data.sunsSongState = SUNSSONG_INACTIVE;
                E_day_time_plus = keep_zelda_time;
                play->msgCtx.ocarinaMode = OCARINA_MODE_04;
            }
        } else if ((play->roomCtx.curRoom.type != ROOM_TYPE_DUNGEON) && (interfaceCtx->restrictions.sunsSong != 3)) {
            if ((z_common_data.save.dayTime >= CLOCK_TIME(6, 30)) &&
                (z_common_data.save.dayTime < CLOCK_TIME(18, 0) + 1)) {
                z_common_data.nextDayTime = NEXT_TIME_NIGHT;
                play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
                z_common_data.nextTransitionType = TRANS_TYPE_FADE_BLACK;
                play->haltAllActors = true;
            } else {
                z_common_data.nextDayTime = NEXT_TIME_DAY;
                play->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
                z_common_data.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                play->haltAllActors = true;
            }

            if (play->sceneId == SCENE_HAUNTED_WASTELAND) {
                play->transitionType = TRANS_TYPE_SANDSTORM_PERSIST;
                z_common_data.nextTransitionType = TRANS_TYPE_SANDSTORM_PERSIST;
            }

            z_common_data.respawnFlag = -2;
            play->nextEntranceIndex = z_common_data.save.entranceIndex;
            play->transitionTrigger = TRANS_TRIGGER_START;
            z_common_data.sunsSongState = SUNSSONG_INACTIVE;
#if OOT_VERSION >= PAL_1_0
            Na_StopAllSound(30);
            z_common_data.seqId = (u8)NA_BGM_DISABLED;
            z_common_data.natureAmbienceId = NATURE_ID_DISABLED;
#endif
        } else {
            z_common_data.sunsSongState = SUNSSONG_SPECIAL;
        }
    }
}
