/*
 * File: z_title.c
 * Overlay: ovl_title
 * Description: Displays the Nintendo Logo
 */

#include "global.h"
#include "alloca.h"
#include "versions.h"

#if PLATFORM_N64
#include "cic6105.h"
#include "n64dd.h"
#endif

#include "assets/textures/nintendo_rogo_static/nintendo_rogo_static.h"

#if DEBUG_FEATURES
void ConsoleLogo_PrintBuildInfo(Gfx** gfxP) {
    Gfx* gfx;
    GfxPrint* printer;

    gfx = *gfxP;
    gfx = gfx_rect_moji(gfx);
    printer = alloca(sizeof(GfxPrint));
    gfxprint_init(printer);
    gfxprint_open(printer, gfx);
    gfxprint_color(printer, 255, 155, 255, 255);
    gfxprint_locate8x8(printer, 9, 21);
    gfxprint_printf(printer, "NOT MARIO CLUB VERSION");
    gfxprint_color(printer, 255, 255, 255, 255);
    gfxprint_locate8x8(printer, 7, 23);
    gfxprint_printf(printer, "[Creator:%s]", __Creator__);
    gfxprint_locate8x8(printer, 7, 24);
    gfxprint_printf(printer, "[Date:%s]", __DateTime__);
    gfx = gfxprint_close(printer);
    gfxprint_cleanup(printer);
    *gfxP = gfx;
}
#endif

void nintendo_rogo_move(ConsoleLogoState* this) {
#if !PLATFORM_GC || OOT_VERSION == HIRATSU3
    if ((this->coverAlpha == 0) && (this->visibleDuration != 0)) {
        this->unk_1D4--;
        this->visibleDuration--;
        if (this->unk_1D4 == 0) {
            this->unk_1D4 = 400;
        }
    } else {
        this->coverAlpha += this->addAlpha;
        if (this->coverAlpha <= 0) {
            this->coverAlpha = 0;
            this->addAlpha = 3;
        } else if (this->coverAlpha >= 255) {
            this->coverAlpha = 255;
            this->exit = true;
        }
    }
    this->uls = this->ult & 0x7F;
    this->ult++;
#else
    this->exit = true;
#endif
}

static void view_setting(ConsoleLogoState* this, f32 x, f32 y, f32 z) {
    View* view = &this->view;
    Vec3f eye;
    Vec3f lookAt;
    Vec3f up;

    eye.x = x;
    eye.y = y;
    eye.z = z;
    up.x = up.z = 0.0f;
    up.y = 1.0f;
    lookAt.x = lookAt.y = lookAt.z = 0.0f;

    setPerspectiveView(view, 30.0f, 10.0f, 12800.0f);
    setLookAtView(view, &eye, &lookAt, &up);
    showView(view, VIEW_ALL);
}

void nintendo_rogo_draw(ConsoleLogoState* this) {
    static s16 yang = 0;
    static Lights1 light_data = gdSPDefLights1(100, 100, 100, 255, 255, 255, 69, 69, 69);

    u16 y;
    u16 idx;
    s32 pad1;
    Vec3f v3;
    Vec3f v1;
    Vec3f v2;
    s32 pad2;
    s32 pad3;

    OPEN_DISPS(this->state.gfxCtx, "../z_title.c", 395);

    v3.x = 69;
    v3.y = 69;
    v3.z = 69;
    v2.x = -4949.148;
    v2.y = 4002.5417;
    v1.x = 0;
    v1.y = 0;
    v1.z = 0;
    v2.z = 1119.0837;

    HiliteReflect_init(&v1, &v2, &v3, this->state.gfxCtx);
    gSPSetLights1(POLY_OPA_DISP++, light_data);
    view_setting(this, 0, 150.0, 300.0);
    _texture_z_light_fog_prim(this->state.gfxCtx);
    Matrix_translate(-53.0, -5.0, 0, MTXMODE_NEW);
    Matrix_scale(1.0, 1.0, 1.0, MTXMODE_APPLY);
    Matrix_rotateXYZ(0, yang, 0, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx, "../z_title.c", 424);
    gSPDisplayList(POLY_OPA_DISP++, gNintendo64LogoDL);
    rectangle_a_prim(this->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_PASS, G_RM_CLD_SURF2);
    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, PRIMITIVE, ENV_ALPHA, TEXEL0, 0, 0, 0, TEXEL0, PRIMITIVE, ENVIRONMENT,
                      COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 170, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 255, 128);

    gDPLoadMultiBlock(POLY_OPA_DISP++, nintendo_rogo_static_Tex_001800, 0x100, 1, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
                      G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 2, 11);

    for (idx = 0, y = 94; idx < 16; idx++, y += 2) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, &((u8*)nintendo_rogo_static_Tex_000000)[0x180 * idx], G_IM_FMT_I,
                            G_IM_SIZ_8b, 192, 2, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gDPSetTileSize(POLY_OPA_DISP++, 1, this->uls, (this->ult & 0x7F) - (idx << 2), 0, 0);
        gSPTextureRectangle(POLY_OPA_DISP++, 97 << 2, y << 2, 289 << 2, (y + 2) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10,
                            1 << 10);
    }

    Display_Fillter(this->state.gfxCtx, 0, 0, 0, (s16)this->coverAlpha, FILL_SCREEN_XLU);

    yang += 300;

    CLOSE_DISPS(this->state.gfxCtx, "../z_title.c", 483);
}

void title_main(GameState* thisx) {
    ConsoleLogoState* this = (ConsoleLogoState*)thisx;

    OPEN_DISPS(this->state.gfxCtx, "../z_title.c", 494);

    gSPSegment(POLY_OPA_DISP++, 0, NULL);
    gSPSegment(POLY_OPA_DISP++, 1, this->staticSegment);
    DisplayList_initialize(this->state.gfxCtx, 0, 0, 0);
    nintendo_rogo_move(this);
    nintendo_rogo_draw(this);

#if DEBUG_FEATURES
    if (gIsCtrlr2Valid) {
        Gfx* gfx = POLY_OPA_DISP;

        ConsoleLogo_PrintBuildInfo(&gfx);
        POLY_OPA_DISP = gfx;
    }
#endif

#if PLATFORM_IQUE
    this->exit = true;
#endif

    if (this->exit) {
        z_common_data.seqId = (u8)NA_BGM_DISABLED;
        z_common_data.natureAmbienceId = 0xFF;
        z_common_data.gameMode = GAMEMODE_TITLE_SCREEN;
        this->state.running = false;
        SET_NEXT_GAMESTATE(&this->state, opening_init, TitleSetupState);
    }

    CLOSE_DISPS(this->state.gfxCtx, "../z_title.c", 541);
}

void title_cleanup(GameState* thisx) {
    ConsoleLogoState* this = (ConsoleLogoState*)thisx;

#if PLATFORM_N64
    if (this->unk_1E0) {
        if (func_801C7818() != 0) {
            Freeze();
        }
        func_801C7268();
    }
#endif

    sram_initialize(&this->state, &this->sramCtx);

#if PLATFORM_N64
    func_800014E8();
#endif
}

void title_init(GameState* thisx) {
    u32 size = (uintptr_t)_nintendo_rogo_staticSegmentRomEnd - (uintptr_t)_nintendo_rogo_staticSegmentRomStart;
    ConsoleLogoState* this = (ConsoleLogoState*)thisx;

#if PLATFORM_N64
    if ((D_80121210 != 0) && (D_80121211 != 0) && (D_80121212 == 0)) {
        if (func_801C7658() != 0) {
            Freeze();
        }
        this->unk_1E0 = true;
    } else {
        this->unk_1E0 = false;
    }
#endif

    this->staticSegment = GAME_STATE_ALLOC(&this->state, size, "../z_title.c", 611);
    PRINTF("z_title.c\n");
    ASSERT(this->staticSegment != NULL, "this->staticSegment != NULL", "../z_title.c", 614);
    DMA_REQUEST_SYNC(this->staticSegment, (uintptr_t)_nintendo_rogo_staticSegmentRomStart, size, "../z_title.c", 615);
    R_UPDATE_RATE = 1;
    new_Matrix(&this->state);
    initView(&this->view, this->state.gfxCtx);
    this->state.main = title_main;
    this->state.destroy = title_cleanup;
    this->exit = false;

#if OOT_VERSION < GC_US || PLATFORM_IQUE || OOT_VERSION == HIRATSU3
    if (!(padmgr.validCtrlrsMask & 1)) {
        z_common_data.fileNum = 0xFEDC;
    } else {
        z_common_data.fileNum = 0xFF;
    }
#else
    z_common_data.fileNum = 0xFF;
#endif

    sram_title_ct(&this->state, &this->sramCtx);
    this->ult = 0;
    this->unk_1D4 = 0x14;
    this->coverAlpha = 255;
    this->addAlpha = -3;
    this->visibleDuration = 0x3C;
}
