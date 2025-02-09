#include "global.h"
#include "assets/textures/parameter_static/parameter_static.h"

static s16 base_prim[3][3] = {
    { HEARTS_PRIM_R, HEARTS_PRIM_G, HEARTS_PRIM_B },
    { HEARTS_BURN_PRIM_R, HEARTS_BURN_PRIM_G, HEARTS_BURN_PRIM_B },    // unused
    { HEARTS_DROWN_PRIM_R, HEARTS_DROWN_PRIM_G, HEARTS_DROWN_PRIM_B }, // unused
};

static s16 base_env[3][3] = {
    { HEARTS_ENV_R, HEARTS_ENV_G, HEARTS_ENV_B },
    { HEARTS_BURN_ENV_R, HEARTS_BURN_ENV_G },                       // unused
    { HEARTS_DROWN_ENV_R, HEARTS_DROWN_ENV_G, HEARTS_DROWN_ENV_B }, // unused
};

static s16 prim[3][3] = {
    {
        HEARTS_PRIM_R - HEARTS_PRIM_R,
        HEARTS_PRIM_G - HEARTS_PRIM_G,
        HEARTS_PRIM_B - HEARTS_PRIM_B,
    },
    // unused
    {
        HEARTS_BURN_PRIM_R - HEARTS_PRIM_R,
        HEARTS_BURN_PRIM_G - HEARTS_PRIM_G,
        HEARTS_BURN_PRIM_B - HEARTS_PRIM_B,
    },
    // unused
    {
        HEARTS_DROWN_PRIM_R - HEARTS_PRIM_R,
        HEARTS_DROWN_PRIM_G - HEARTS_PRIM_G,
        HEARTS_DROWN_PRIM_B - HEARTS_PRIM_B,
    },
};

static s16 env[3][3] = {
    {
        HEARTS_ENV_R - HEARTS_ENV_R,
        HEARTS_ENV_G - HEARTS_ENV_G,
        HEARTS_ENV_B - HEARTS_ENV_B,
    },
    // unused
    {
        HEARTS_BURN_ENV_R - HEARTS_ENV_R,
        HEARTS_BURN_ENV_G - HEARTS_ENV_G,
        HEARTS_BURN_ENV_B - HEARTS_ENV_B,
    },
    // unused
    {
        HEARTS_DROWN_ENV_R - HEARTS_ENV_R,
        HEARTS_DROWN_ENV_G - HEARTS_ENV_G,
        HEARTS_DROWN_ENV_B - HEARTS_ENV_B,
    },
};

static s16 base_prim02[3][3] = {
    { HEARTS_DD_PRIM_R, HEARTS_DD_PRIM_G, HEARTS_DD_PRIM_B },
    { HEARTS_BURN_PRIM_R, HEARTS_BURN_PRIM_G, HEARTS_BURN_PRIM_B },    // unused
    { HEARTS_DROWN_PRIM_R, HEARTS_DROWN_PRIM_G, HEARTS_DROWN_PRIM_B }, // unused
};

static s16 base_env02[3][3] = {
    { HEARTS_DD_ENV_R, HEARTS_DD_ENV_G, HEARTS_DD_ENV_B },
    { HEARTS_BURN_ENV_R, HEARTS_BURN_ENV_G, HEARTS_BURN_ENV_B },    // unused
    { HEARTS_DROWN_ENV_R, HEARTS_DROWN_ENV_G, HEARTS_DROWN_ENV_B }, // unused
};

static s16 prim02[3][3] = {
    {
        HEARTS_DD_PRIM_R - HEARTS_DD_PRIM_R,
        HEARTS_DD_PRIM_G - HEARTS_DD_PRIM_G,
        HEARTS_DD_PRIM_B - HEARTS_DD_PRIM_B,
    },
    // unused
    {
        HEARTS_BURN_PRIM_R - HEARTS_DD_PRIM_R,
        HEARTS_BURN_PRIM_G - HEARTS_DD_PRIM_G,
        HEARTS_BURN_PRIM_B - HEARTS_DD_PRIM_B,
    },
    // unused
    {
        HEARTS_DROWN_PRIM_R - HEARTS_DD_PRIM_R,
        HEARTS_DROWN_PRIM_G - HEARTS_DD_PRIM_G,
        HEARTS_DROWN_PRIM_B - HEARTS_DD_PRIM_B,
    },
};

static s16 env02[3][3] = {
    {
        HEARTS_DD_ENV_R - HEARTS_DD_ENV_R,
        HEARTS_DD_ENV_G - HEARTS_DD_ENV_G,
        HEARTS_DD_ENV_B - HEARTS_DD_ENV_B,
    },
    // unused
    {
        HEARTS_BURN_ENV_R - HEARTS_DD_ENV_R,
        HEARTS_BURN_ENV_G - HEARTS_DD_ENV_G,
        HEARTS_BURN_ENV_B - HEARTS_DD_ENV_B,
    },
    // unused
    {
        HEARTS_DROWN_ENV_R - HEARTS_DD_ENV_R,
        HEARTS_DROWN_ENV_G - HEARTS_DD_ENV_G,
        HEARTS_DROWN_ENV_B - HEARTS_DD_ENV_B,
    },
};

// Current colors for the double defense hearts
s16 life2_prim[3];
s16 life2_env[3];
s16 life2_org_prim[2][3];
s16 life2_org_env[2][3];

void initial_LifeMeterColorAnimation(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    interfaceCtx->unk_228 = 0x140;
    interfaceCtx->unk_226 = z_common_data.save.info.playerData.health;
    interfaceCtx->beatingHeartOscillator = interfaceCtx->heartColorOscillator = 0;
    interfaceCtx->beatingHeartOscillatorDirection = interfaceCtx->heartColorOscillatorDirection = 0;

    interfaceCtx->heartsPrimR[0] = HEARTS_PRIM_R;
    interfaceCtx->heartsPrimG[0] = HEARTS_PRIM_G;
    interfaceCtx->heartsPrimB[0] = HEARTS_PRIM_B;

    interfaceCtx->heartsEnvR[0] = HEARTS_ENV_R;
    interfaceCtx->heartsEnvG[0] = HEARTS_ENV_G;
    interfaceCtx->heartsEnvB[0] = HEARTS_ENV_B;

    interfaceCtx->heartsPrimR[1] = HEARTS_PRIM_R;
    interfaceCtx->heartsPrimG[1] = HEARTS_PRIM_G;
    interfaceCtx->heartsPrimB[1] = HEARTS_PRIM_B;

    interfaceCtx->heartsEnvR[1] = HEARTS_ENV_R;
    interfaceCtx->heartsEnvG[1] = HEARTS_ENV_G;
    interfaceCtx->heartsEnvB[1] = HEARTS_ENV_B;

    life2_org_prim[0][0] = life2_org_prim[1][0] = HEARTS_DD_PRIM_R;
    life2_org_prim[0][1] = life2_org_prim[1][1] = HEARTS_DD_PRIM_G;
    life2_org_prim[0][2] = life2_org_prim[1][2] = HEARTS_DD_PRIM_B;

    life2_org_env[0][0] = life2_org_env[1][0] = HEARTS_DD_ENV_R;
    life2_org_env[0][1] = life2_org_env[1][1] = HEARTS_DD_ENV_G;
    life2_org_env[0][2] = life2_org_env[1][2] = HEARTS_DD_ENV_B;
}

void LifeMeterColorAnimation(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    f32 factor = interfaceCtx->heartColorOscillator * 0.1f;
    f32 ddFactor;
    s32 type = 0;
    s32 ddType;
    s16 rFactor;
    s16 gFactor;
    s16 bFactor;

    if (interfaceCtx) {}

    if (interfaceCtx->heartColorOscillatorDirection != 0) {
        interfaceCtx->heartColorOscillator--;
        if (interfaceCtx->heartColorOscillator <= 0) {
            interfaceCtx->heartColorOscillator = 0;
            interfaceCtx->heartColorOscillatorDirection = 0;
        }
    } else {
        interfaceCtx->heartColorOscillator++;
        if (interfaceCtx->heartColorOscillator >= 10) {
            interfaceCtx->heartColorOscillator = 10;
            interfaceCtx->heartColorOscillatorDirection = 1;
        }
    }

    ddFactor = factor;

    interfaceCtx->heartsPrimR[0] = HEARTS_PRIM_R;
    interfaceCtx->heartsPrimG[0] = HEARTS_PRIM_G;
    interfaceCtx->heartsPrimB[0] = HEARTS_PRIM_B;

    interfaceCtx->heartsEnvR[0] = HEARTS_ENV_R;
    interfaceCtx->heartsEnvG[0] = HEARTS_ENV_G;
    interfaceCtx->heartsEnvB[0] = HEARTS_ENV_B;

    interfaceCtx->heartsPrimR[1] = base_prim[type][0];
    interfaceCtx->heartsPrimG[1] = base_prim[type][1];
    interfaceCtx->heartsPrimB[1] = base_prim[type][2];

    interfaceCtx->heartsEnvR[1] = base_env[type][0];
    interfaceCtx->heartsEnvG[1] = base_env[type][1];
    interfaceCtx->heartsEnvB[1] = base_env[type][2];

    rFactor = prim[0][0] * factor;
    gFactor = prim[0][1] * factor;
    bFactor = prim[0][2] * factor;

    interfaceCtx->beatingHeartPrim[0] = (u8)(rFactor + HEARTS_PRIM_R) & 0xFF;
    interfaceCtx->beatingHeartPrim[1] = (u8)(gFactor + HEARTS_PRIM_G) & 0xFF;
    interfaceCtx->beatingHeartPrim[2] = (u8)(bFactor + HEARTS_PRIM_B) & 0xFF;

    rFactor = env[0][0] * factor;
    gFactor = env[0][1] * factor;
    bFactor = env[0][2] * factor;

    if (1) {}
    ddType = type;

    interfaceCtx->beatingHeartEnv[0] = (u8)(rFactor + HEARTS_ENV_R) & 0xFF;
    interfaceCtx->beatingHeartEnv[1] = (u8)(gFactor + HEARTS_ENV_G) & 0xFF;
    interfaceCtx->beatingHeartEnv[2] = (u8)(bFactor + HEARTS_ENV_B) & 0xFF;

    life2_org_prim[0][0] = HEARTS_DD_PRIM_R;
    life2_org_prim[0][1] = HEARTS_DD_PRIM_G;
    life2_org_prim[0][2] = HEARTS_DD_PRIM_B;

    life2_org_env[0][0] = HEARTS_DD_ENV_R;
    life2_org_env[0][1] = HEARTS_DD_ENV_G;
    life2_org_env[0][2] = HEARTS_DD_ENV_B;

    life2_org_prim[1][0] = base_prim02[ddType][0];
    life2_org_prim[1][1] = base_prim02[ddType][1];
    life2_org_prim[1][2] = base_prim02[ddType][2];

    life2_org_env[1][0] = base_env02[ddType][0];
    life2_org_env[1][1] = base_env02[ddType][1];
    life2_org_env[1][2] = base_env02[ddType][2];

    rFactor = prim02[ddType][0] * ddFactor;
    gFactor = prim02[ddType][1] * ddFactor;
    bFactor = prim02[ddType][2] * ddFactor;

    life2_prim[0] = (u8)(rFactor + HEARTS_DD_PRIM_R) & 0xFF;
    life2_prim[1] = (u8)(gFactor + HEARTS_DD_PRIM_G) & 0xFF;
    life2_prim[2] = (u8)(bFactor + HEARTS_DD_PRIM_B) & 0xFF;

    rFactor = env02[ddType][0] * ddFactor;
    gFactor = env02[ddType][1] * ddFactor;
    bFactor = env02[ddType][2] * ddFactor;

    life2_env[0] = (u8)(rFactor + HEARTS_DD_ENV_R) & 0xFF;
    life2_env[1] = (u8)(gFactor + HEARTS_DD_ENV_G) & 0xFF;
    life2_env[2] = (u8)(bFactor + HEARTS_DD_ENV_B) & 0xFF;
}

// Unused
s32 initial_LifeRestChange_Damage(PlayState* play) {
    z_common_data.save.info.playerData.health = play->interfaceCtx.unk_226;
    return 1;
}

// Unused
s32 initial_LifeRestChange(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    interfaceCtx->unk_228 = 0x140;
    interfaceCtx->unk_226 += 0x10;

    if (interfaceCtx->unk_226 >= z_common_data.save.info.playerData.health) {
        interfaceCtx->unk_226 = z_common_data.save.info.playerData.health;
        return 1;
    }

    return 0;
}

// Unused
s32 LifeRestChange(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (interfaceCtx->unk_228 != 0) {
        interfaceCtx->unk_228--;
    } else {
        interfaceCtx->unk_228 = 0x140;
        interfaceCtx->unk_226 -= 0x10;
        if (interfaceCtx->unk_226 <= 0) {
            interfaceCtx->unk_226 = 0;
            play->damagePlayer(play, -(z_common_data.save.info.playerData.health + 1));
            return 1;
        }
    }
    return 0;
}

static void* Heart[] = {
    gHeartFullTex,         gHeartQuarterTex,      gHeartQuarterTex,      gHeartQuarterTex,
    gHeartQuarterTex,      gHeartQuarterTex,      gHeartHalfTex,         gHeartHalfTex,
    gHeartHalfTex,         gHeartHalfTex,         gHeartHalfTex,         gHeartThreeQuarterTex,
    gHeartThreeQuarterTex, gHeartThreeQuarterTex, gHeartThreeQuarterTex, gHeartThreeQuarterTex,
};

static void* Heart2[] = {
    gDefenseHeartFullTex,         gDefenseHeartQuarterTex,      gDefenseHeartQuarterTex,
    gDefenseHeartQuarterTex,      gDefenseHeartQuarterTex,      gDefenseHeartQuarterTex,
    gDefenseHeartHalfTex,         gDefenseHeartHalfTex,         gDefenseHeartHalfTex,
    gDefenseHeartHalfTex,         gDefenseHeartHalfTex,         gDefenseHeartThreeQuarterTex,
    gDefenseHeartThreeQuarterTex, gDefenseHeartThreeQuarterTex, gDefenseHeartThreeQuarterTex,
    gDefenseHeartThreeQuarterTex,
};

void DrawLifeMeter(PlayState* play) {
    s32 pad[5];
    void* heartBgImg;
    u32 curColorSet;
    f32 offsetX;
    f32 offsetY;
    s32 heartIndex;
    f32 halfHeartLength;
    f32 heartCenterX;
    f32 heartCenterY;
    f32 heartTexCoordPerPixel;
    InterfaceContext* interfaceCtx = &play->interfaceCtx;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    Vtx* beatingHeartVtx = interfaceCtx->beatingHeartVtx;
    s32 curHeartFraction = z_common_data.save.info.playerData.health % 0x10;
    s16 totalHeartCount = z_common_data.save.info.playerData.healthCapacity / 0x10;
    s16 fullHeartCount = z_common_data.save.info.playerData.health / 0x10;
    s32 pad2;
    f32 beatingHeartPulsingSize = interfaceCtx->beatingHeartOscillator * 0.1f;
    s32 curCombineModeSet = 0;
    u8* curBgImgLoaded = NULL;
    s32 ddHeartCountMinusOne = z_common_data.save.info.inventory.defenseHearts - 1;

    OPEN_DISPS(gfxCtx, "../z_lifemeter.c", 353);

    if (!(z_common_data.save.info.playerData.health % 0x10)) {
        fullHeartCount--;
    }

    curColorSet = -1;
    offsetY = 0.0f;
    offsetX = 0.0f;

    for (heartIndex = 0; heartIndex < totalHeartCount; heartIndex++) {
        if ((ddHeartCountMinusOne < 0) || (heartIndex > ddHeartCountMinusOne)) {
            if (heartIndex < fullHeartCount) {
                if (curColorSet != 0) {
                    curColorSet = 0;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, interfaceCtx->heartsPrimR[0], interfaceCtx->heartsPrimG[0],
                                    interfaceCtx->heartsPrimB[0], interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, interfaceCtx->heartsEnvR[0], interfaceCtx->heartsEnvG[0],
                                   interfaceCtx->heartsEnvB[0], 255);
                }
            } else if (heartIndex == fullHeartCount) {
                if (curColorSet != 1) {
                    curColorSet = 1;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, interfaceCtx->beatingHeartPrim[0],
                                    interfaceCtx->beatingHeartPrim[1], interfaceCtx->beatingHeartPrim[2],
                                    interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, interfaceCtx->beatingHeartEnv[0], interfaceCtx->beatingHeartEnv[1],
                                   interfaceCtx->beatingHeartEnv[2], 255);
                }
            } else if (heartIndex > fullHeartCount) {
                if (curColorSet != 2) {
                    curColorSet = 2;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, interfaceCtx->heartsPrimR[0], interfaceCtx->heartsPrimG[0],
                                    interfaceCtx->heartsPrimB[0], interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, interfaceCtx->heartsEnvR[0], interfaceCtx->heartsEnvG[0],
                                   interfaceCtx->heartsEnvB[0], 255);
                }
            } else {
                if (curColorSet != 3) {
                    curColorSet = 3;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, interfaceCtx->heartsPrimR[1], interfaceCtx->heartsPrimG[1],
                                    interfaceCtx->heartsPrimB[1], interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, interfaceCtx->heartsEnvR[1], interfaceCtx->heartsEnvG[1],
                                   interfaceCtx->heartsEnvB[1], 255);
                }
            }

            if (heartIndex < fullHeartCount) {
                heartBgImg = gHeartFullTex;
            } else if (heartIndex == fullHeartCount) {
                heartBgImg = Heart[curHeartFraction];
            } else {
                heartBgImg = gHeartEmptyTex;
            }
        } else {
            if (heartIndex < fullHeartCount) {
                if (curColorSet != 4) {
                    curColorSet = 4;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, life2_org_prim[0][0], life2_org_prim[0][1], life2_org_prim[0][2],
                                    interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, life2_org_env[0][0], life2_org_env[0][1], life2_org_env[0][2], 255);
                }
            } else if (heartIndex == fullHeartCount) {
                if (curColorSet != 5) {
                    curColorSet = 5;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, life2_prim[0], life2_prim[1],
                                    life2_prim[2], interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, life2_env[0], life2_env[1],
                                   life2_env[2], 255);
                }
            } else if (heartIndex > fullHeartCount) {
                if (curColorSet != 6) {
                    curColorSet = 6;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, life2_org_prim[0][0], life2_org_prim[0][1], life2_org_prim[0][2],
                                    interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, life2_org_env[0][0], life2_org_env[0][1], life2_org_env[0][2], 255);
                }
            } else {
                if (curColorSet != 7) {
                    curColorSet = 7;
                    gDPPipeSync(OVERLAY_DISP++);
                    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, life2_org_prim[1][0], life2_org_prim[1][1], life2_org_prim[1][2],
                                    interfaceCtx->healthAlpha);
                    gDPSetEnvColor(OVERLAY_DISP++, life2_org_env[1][0], life2_org_env[1][1], life2_org_env[1][2], 255);
                }
            }

            if (heartIndex < fullHeartCount) {
                heartBgImg = gDefenseHeartFullTex;
            } else if (heartIndex == fullHeartCount) {
                heartBgImg = Heart2[curHeartFraction];
            } else {
                heartBgImg = gDefenseHeartEmptyTex;
            }
        }

        if (curBgImgLoaded != heartBgImg) {
            curBgImgLoaded = heartBgImg;
            gDPLoadTextureBlock(OVERLAY_DISP++, heartBgImg, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
        }

        if (heartIndex != fullHeartCount) {
            if ((ddHeartCountMinusOne < 0) || (heartIndex > ddHeartCountMinusOne)) {
                if (curCombineModeSet != 1) {
                    curCombineModeSet = 1;
                    overlay_rectangle_a_prim(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE,
                                      0, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                }
            } else {
                if (curCombineModeSet != 3) {
                    curCombineModeSet = 3;
                    overlay_rectangle_a_prim(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                                      0, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);
                }
            }

            heartCenterY = 26.0f + offsetY;
            heartCenterX = 30.0f + offsetX;
            heartTexCoordPerPixel = 1.0f;
            heartTexCoordPerPixel /= 0.68f;
            heartTexCoordPerPixel *= 1 << 10;
            halfHeartLength = 8.0f;
            halfHeartLength *= 0.68f;
            gSPTextureRectangle(OVERLAY_DISP++, (s32)((heartCenterX - halfHeartLength) * 4),
                                (s32)((heartCenterY - halfHeartLength) * 4),
                                (s32)((heartCenterX + halfHeartLength) * 4),
                                (s32)((heartCenterY + halfHeartLength) * 4), G_TX_RENDERTILE, 0, 0,
                                (s32)heartTexCoordPerPixel, (s32)heartTexCoordPerPixel);
        } else {
            if ((ddHeartCountMinusOne < 0) || (heartIndex > ddHeartCountMinusOne)) {
                if (curCombineModeSet != 2) {
                    curCombineModeSet = 2;
                    overlay_kaleido_scope_prim(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE,
                                      0, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
                }
            } else {
                if (curCombineModeSet != 4) {
                    curCombineModeSet = 4;
                    overlay_kaleido_scope_prim(gfxCtx);
                    gDPSetCombineLERP(OVERLAY_DISP++, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                                      0, ENVIRONMENT, PRIMITIVE, TEXEL0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0);
                }
            }

            {
                Mtx* matrix = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
                suMtxMakeTS(
                    matrix, 1.0f - (0.32f * beatingHeartPulsingSize), 1.0f - (0.32f * beatingHeartPulsingSize),
                    1.0f - (0.32f * beatingHeartPulsingSize), -130.0f + offsetX, 94.5f - offsetY, 0.0f);
                gSPMatrix(OVERLAY_DISP++, matrix, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                gSPVertex(OVERLAY_DISP++, beatingHeartVtx, 4, 0);
                gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);
            }
        }

        // Move offset to next heart
        offsetX += 10.0f;

        // Go down one line after 10 hearts
        if (heartIndex == 9) {
            offsetY += 10.0f;
            offsetX = 0.0f;
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_lifemeter.c", 606);
}

void LifeMeterZoom(PlayState* play) {
    InterfaceContext* interfaceCtx = &play->interfaceCtx;

    if (interfaceCtx->beatingHeartOscillatorDirection != 0) {
        interfaceCtx->beatingHeartOscillator--;
        if (interfaceCtx->beatingHeartOscillator <= 0) {
            interfaceCtx->beatingHeartOscillator = 0;
            interfaceCtx->beatingHeartOscillatorDirection = 0;
            if (!player_demo_check(play) && !IS_PAUSED(&play->pauseCtx) && Life_Caution_Check() && !Game_play_demo_mode_check(play)) {
                Na_StartSystemSe_F(NA_SE_SY_HITPOINT_ALARM);
            }
        }
    } else {
        interfaceCtx->beatingHeartOscillator++;
        if (interfaceCtx->beatingHeartOscillator >= 10) {
            interfaceCtx->beatingHeartOscillator = 10;
            interfaceCtx->beatingHeartOscillatorDirection = 1;
        }
    }
}

u32 Life_Caution_Check(void) {
    s32 criticalHealth;

    if (z_common_data.save.info.playerData.healthCapacity <= 0x50) {
        criticalHealth = 0x10;
    } else if (z_common_data.save.info.playerData.healthCapacity <= 0xA0) {
        criticalHealth = 0x18;
    } else if (z_common_data.save.info.playerData.healthCapacity <= 0xF0) {
        criticalHealth = 0x20;
    } else {
        criticalHealth = 0x2C;
    }

    if ((criticalHealth >= z_common_data.save.info.playerData.health) &&
        (z_common_data.save.info.playerData.health > 0)) {
        return true;
    } else {
        return false;
    }
}
