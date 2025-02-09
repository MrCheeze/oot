#include "file_select.h"
#include "terminal.h"
#include "versions.h"
#include "assets/textures/title_static/title_static.h"
#include "assets/overlays/ovl_file_choose/ovl_file_choose.h"

void file_choose_moji_set(GraphicsContext* gfxCtx, void* texture, s16 vtx) {
    OPEN_DISPS(gfxCtx, "../z_file_nameset_PAL.c", 110);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, texture, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                           G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    CLOSE_DISPS(gfxCtx, "../z_file_nameset_PAL.c", 119);
}

#if OOT_NTSC
void morph_moji_set(GraphicsContext* gfxCtx, void* texture1, void* texture2, s16 vtx) {
    OPEN_DISPS(gfxCtx, "../z_file_nameset_PAL.c", UNK_LINE);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, texture1, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gDPLoadMultiBlock_4b(POLY_OPA_DISP++, texture2, 0x0080, 1, G_IM_FMT_I, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                         G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    CLOSE_DISPS(gfxCtx, "../z_file_nameset_PAL.c", UNK_LINE);
}
#endif

#if OOT_PAL

#if OOT_VERSION >= PAL_1_1
s16 D_808124C0[] = {
    2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 0, 1, 1, 2, 1, 1, 4, 2, 2, 2, 1, 1, 0, 2, 0, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 2, 2, 2, 2, 2, 3, 2, 2, 4, 3, 2, 4, 1, 2, 2, 1, 1, 2, 2, 3, 2, 2, 0, 2, 2, 2, 0, 3, 1, 0,
};
#endif

s16 D_80812544[] = {
#if OOT_VERSION < PAL_1_1
    1, 2, 0, 1, 1, 2, 1, 2, 4, 2, 2, 2, 1, 1, 0, 2, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 2, 2, 2, 2, 2, 3,
    2, 2, 4, 3, 2, 4, 1, 2, 2, 1, 1, 2, 2, 3, 2, 2, 0, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3,
#else
    // Differs only at index 7
    1, 2, 0, 1, 1, 2, 1, 1, 4, 2, 2, 2, 1, 1, 0, 2, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 2, 2, 2, 2, 2, 3,
    2, 2, 4, 3, 2, 4, 1, 2, 2, 1, 1, 2, 2, 3, 2, 2, 0, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 3,
#endif
};

#endif

void nameset_vtx_set(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 phi_t2;
    s16 phi_t0;
    s16 phi_t3;
    s16 phi_s1;
    s16 phi_t1;
    s16 phi_s2;

    this->keyboardVtx = GRAPH_ALLOC(this->state.gfxCtx, sizeof(Vtx) * 4 * 5 * 13);

    phi_s1 = 0x26;

#if OOT_NTSC
    for (phi_s2 = 0, phi_t3 = 0; phi_s2 < 5; phi_s2++) {
        for (phi_t0 = -0x60, phi_t1 = 0; phi_t1 < 13; phi_t1++, phi_t3 += 4) {
            this->keyboardVtx[phi_t3].v.ob[0] = this->keyboardVtx[phi_t3 + 2].v.ob[0] = phi_t0;
            this->keyboardVtx[phi_t3 + 1].v.ob[0] = this->keyboardVtx[phi_t3 + 3].v.ob[0] = phi_t0 + 12;

            this->keyboardVtx[phi_t3].v.ob[1] = this->keyboardVtx[phi_t3 + 1].v.ob[1] = phi_s1;

            this->keyboardVtx[phi_t3 + 2].v.ob[1] = this->keyboardVtx[phi_t3 + 3].v.ob[1] = phi_s1 - 12;

            this->keyboardVtx[phi_t3].v.ob[2] = this->keyboardVtx[phi_t3 + 1].v.ob[2] =
                this->keyboardVtx[phi_t3 + 2].v.ob[2] = this->keyboardVtx[phi_t3 + 3].v.ob[2] = 0;

            this->keyboardVtx[phi_t3].v.flag = this->keyboardVtx[phi_t3 + 1].v.flag =
                this->keyboardVtx[phi_t3 + 2].v.flag = this->keyboardVtx[phi_t3 + 3].v.flag = 0;

            this->keyboardVtx[phi_t3].v.tc[0] = this->keyboardVtx[phi_t3].v.tc[1] =
                this->keyboardVtx[phi_t3 + 1].v.tc[1] = this->keyboardVtx[phi_t3 + 2].v.tc[0] = 0;

            this->keyboardVtx[phi_t3 + 1].v.tc[0] = this->keyboardVtx[phi_t3 + 2].v.tc[1] =
                this->keyboardVtx[phi_t3 + 3].v.tc[0] = this->keyboardVtx[phi_t3 + 3].v.tc[1] = 0x200;

            this->keyboardVtx[phi_t3].v.cn[0] = this->keyboardVtx[phi_t3 + 1].v.cn[0] =
                this->keyboardVtx[phi_t3 + 2].v.cn[0] = this->keyboardVtx[phi_t3 + 3].v.cn[0] =
                    this->keyboardVtx[phi_t3].v.cn[1] = this->keyboardVtx[phi_t3 + 1].v.cn[1] =
                        this->keyboardVtx[phi_t3 + 2].v.cn[1] = this->keyboardVtx[phi_t3 + 3].v.cn[1] =
                            this->keyboardVtx[phi_t3].v.cn[2] = this->keyboardVtx[phi_t3 + 1].v.cn[2] =
                                this->keyboardVtx[phi_t3 + 2].v.cn[2] = this->keyboardVtx[phi_t3 + 3].v.cn[2] =
                                    this->keyboardVtx[phi_t3].v.cn[3] = this->keyboardVtx[phi_t3 + 1].v.cn[3] =
                                        this->keyboardVtx[phi_t3 + 2].v.cn[3] = this->keyboardVtx[phi_t3 + 3].v.cn[3] =
                                            255;

            phi_t0 += 0x10;
        }

        phi_s1 -= 0x10;
    }
#else
    for (phi_t2 = 0, phi_s2 = 0, phi_t3 = 0; phi_s2 < 5; phi_s2++) {
        for (phi_t0 = -0x60, phi_t1 = 0; phi_t1 < 13; phi_t1++, phi_t3 += 4, phi_t2++) {
            //! @bug D_80812544 is accessed out of bounds when drawing the empty space character (value
            //! of 64). Under normal circumstances it reads a halfword from h_namae_txt_data.
            this->keyboardVtx[phi_t3].v.ob[0] = this->keyboardVtx[phi_t3 + 2].v.ob[0] = D_80812544[phi_t2] + phi_t0;
            this->keyboardVtx[phi_t3 + 1].v.ob[0] = this->keyboardVtx[phi_t3 + 3].v.ob[0] =
                D_80812544[phi_t2] + phi_t0 + 12;

            this->keyboardVtx[phi_t3].v.ob[1] = this->keyboardVtx[phi_t3 + 1].v.ob[1] = phi_s1;

            this->keyboardVtx[phi_t3 + 2].v.ob[1] = this->keyboardVtx[phi_t3 + 3].v.ob[1] = phi_s1 - 12;

            this->keyboardVtx[phi_t3].v.ob[2] = this->keyboardVtx[phi_t3 + 1].v.ob[2] =
                this->keyboardVtx[phi_t3 + 2].v.ob[2] = this->keyboardVtx[phi_t3 + 3].v.ob[2] = 0;

            this->keyboardVtx[phi_t3].v.flag = this->keyboardVtx[phi_t3 + 1].v.flag =
                this->keyboardVtx[phi_t3 + 2].v.flag = this->keyboardVtx[phi_t3 + 3].v.flag = 0;

            this->keyboardVtx[phi_t3].v.tc[0] = this->keyboardVtx[phi_t3].v.tc[1] =
                this->keyboardVtx[phi_t3 + 1].v.tc[1] = this->keyboardVtx[phi_t3 + 2].v.tc[0] = 0;

            this->keyboardVtx[phi_t3 + 1].v.tc[0] = this->keyboardVtx[phi_t3 + 2].v.tc[1] =
                this->keyboardVtx[phi_t3 + 3].v.tc[0] = this->keyboardVtx[phi_t3 + 3].v.tc[1] = 0x200;

            this->keyboardVtx[phi_t3].v.cn[0] = this->keyboardVtx[phi_t3 + 1].v.cn[0] =
                this->keyboardVtx[phi_t3 + 2].v.cn[0] = this->keyboardVtx[phi_t3 + 3].v.cn[0] =
                    this->keyboardVtx[phi_t3].v.cn[1] = this->keyboardVtx[phi_t3 + 1].v.cn[1] =
                        this->keyboardVtx[phi_t3 + 2].v.cn[1] = this->keyboardVtx[phi_t3 + 3].v.cn[1] =
                            this->keyboardVtx[phi_t3].v.cn[2] = this->keyboardVtx[phi_t3 + 1].v.cn[2] =
                                this->keyboardVtx[phi_t3 + 2].v.cn[2] = this->keyboardVtx[phi_t3 + 3].v.cn[2] =
                                    this->keyboardVtx[phi_t3].v.cn[3] = this->keyboardVtx[phi_t3 + 1].v.cn[3] =
                                        this->keyboardVtx[phi_t3 + 2].v.cn[3] = this->keyboardVtx[phi_t3 + 3].v.cn[3] =
                                            255;

            phi_t0 += 0x10;
        }

        phi_s1 -= 0x10;
    }
#endif
}

static void* h_namae_txt_data[] =
    LANGUAGE_ARRAY(gFileSelNameJPNTex, gFileSelNameENGTex, gFileSelNameENGTex, gFileSelNameFRATex);

#if OOT_NTSC
static void* plate_no_data[] = {
    gFileSelHiraganaButtonTex,  gFileSelKatakanaButtonTex, gFileSelKanjiButtonTex,
    gFileSelBackspaceButtonTex, gFileSelENDButtonENGTex,
};

static u16 plate_xsize[] = { 44, 44, 28, 28, 44 };
#else
static void* plate_no_data[][2] = {
    { gFileSelBackspaceButtonTex, gFileSelENDButtonENGTex },
    { gFileSelBackspaceButtonTex, gFileSelENDButtonGERTex },
    { gFileSelBackspaceButtonTex, gFileSelENDButtonFRATex },
};

static u16 plate_xsize[] = { 28, 44 };
#endif

static s16 cursorXps[] = {
    0xFFE2, 0xFFF0, 0xFFFA, 0x0004, 0x000E, 0x0018, 0x0022, 0x002C, 0x0036, 0xFFF0, 0xFFF0,
};

static s16 cursorYps[] = {
    0x0048, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045, 0x0045,
};

/**
 * Set vertices used by all elements of the name entry screen that are NOT the keyboard.
 * This includes the cursor highlight, the name entry plate and characters, and the buttons.
 */
void nameset_basic_display(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Font* font = &this->font;
    s16 phi_s0;
    s16 phi_t1;
#if OOT_PAL
    u8 temp;
#endif
    s16 phi_v0;

    OPEN_DISPS(this->state.gfxCtx, "../z_file_nameset_PAL.c", 205);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    gSPVertex(POLY_OPA_DISP++, menu_select_titlevtx, 24, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, h_namae_txt_data[z_common_data.language], G_IM_FMT_IA, G_IM_SIZ_8b, 56, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    gDPPipeSync(POLY_OPA_DISP++);

#if OOT_NTSC
    for (phi_t1 = 0, phi_s0 = 4; phi_t1 < 5; phi_t1++, phi_s0 += 4) {
        if (z_common_data.language == LANGUAGE_JPN) {
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            255);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

            gDPLoadTextureBlock(POLY_OPA_DISP++, plate_no_data[phi_t1], G_IM_FMT_IA, G_IM_SIZ_16b,
                                plate_xsize[phi_t1], 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(POLY_OPA_DISP++, phi_s0, phi_s0 + 2, phi_s0 + 3, phi_s0 + 1, 0);
        } else if (phi_t1 >= 3) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                            255);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

            gDPLoadTextureBlock(POLY_OPA_DISP++, plate_no_data[phi_t1], G_IM_FMT_IA, G_IM_SIZ_16b,
                                plate_xsize[phi_t1], 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

            gSP1Quadrangle(POLY_OPA_DISP++, phi_s0, phi_s0 + 2, phi_s0 + 3, phi_s0 + 1, 0);
        }
    }
#else
    phi_s0 = 0x10;
    for (phi_t1 = 0; phi_t1 < 2; phi_t1++, phi_s0 += 4) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2], 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

        gDPLoadTextureBlock(POLY_OPA_DISP++, plate_no_data[z_common_data.language][phi_t1], G_IM_FMT_IA, G_IM_SIZ_16b,
                            plate_xsize[phi_t1], 16, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gSP1Quadrangle(POLY_OPA_DISP++, phi_s0, phi_s0 + 2, phi_s0 + 3, phi_s0 + 1, 0);
    }
#endif

    this->nameEntryVtx = GRAPH_ALLOC(this->state.gfxCtx, 44 * sizeof(Vtx));

    for (phi_s0 = 0, phi_t1 = 0; phi_t1 < 44; phi_t1 += 4, phi_s0++) {
#if OOT_NTSC
        this->nameEntryVtx[phi_t1].v.ob[0] = this->nameEntryVtx[phi_t1 + 2].v.ob[0] =
            cursorXps[phi_s0] + this->nameEntryBoxPosX;

        this->nameEntryVtx[phi_t1 + 1].v.ob[0] = this->nameEntryVtx[phi_t1 + 3].v.ob[0] =
            this->nameEntryVtx[phi_t1].v.ob[0] + 0xA;
#else
        if ((phi_s0 > 0) && (phi_s0 < 9)) {
#if OOT_VERSION >= PAL_1_1
            temp = this->fileNames[this->buttonIndex][phi_s0 - 1];

            this->nameEntryVtx[phi_t1].v.ob[0] = this->nameEntryVtx[phi_t1 + 2].v.ob[0] =
                cursorXps[phi_s0] + this->nameEntryBoxPosX + D_808124C0[temp];
#else
            this->nameEntryVtx[phi_t1].v.ob[0] = this->nameEntryVtx[phi_t1 + 2].v.ob[0] =
                cursorXps[phi_s0] + this->nameEntryBoxPosX;
#endif

            this->nameEntryVtx[phi_t1 + 1].v.ob[0] = this->nameEntryVtx[phi_t1 + 3].v.ob[0] =
                this->nameEntryVtx[phi_t1].v.ob[0] + 0xA;
        } else {
            this->nameEntryVtx[phi_t1].v.ob[0] = this->nameEntryVtx[phi_t1 + 2].v.ob[0] =
                cursorXps[phi_s0] + this->nameEntryBoxPosX;

            this->nameEntryVtx[phi_t1 + 1].v.ob[0] = this->nameEntryVtx[phi_t1 + 3].v.ob[0] =
                this->nameEntryVtx[phi_t1].v.ob[0] + 0xA;
        }
#endif

        this->nameEntryVtx[phi_t1].v.ob[1] = this->nameEntryVtx[phi_t1 + 1].v.ob[1] = cursorYps[phi_s0];

        this->nameEntryVtx[phi_t1 + 2].v.ob[1] = this->nameEntryVtx[phi_t1 + 3].v.ob[1] =
            this->nameEntryVtx[phi_t1].v.ob[1] - 0xA;

        this->nameEntryVtx[phi_t1].v.ob[2] = this->nameEntryVtx[phi_t1 + 1].v.ob[2] =
            this->nameEntryVtx[phi_t1 + 2].v.ob[2] = this->nameEntryVtx[phi_t1 + 3].v.ob[2] = 0;

        this->nameEntryVtx[phi_t1].v.flag = this->nameEntryVtx[phi_t1 + 1].v.flag =
            this->nameEntryVtx[phi_t1 + 2].v.flag = this->nameEntryVtx[phi_t1 + 3].v.flag = 0;

        this->nameEntryVtx[phi_t1].v.tc[0] = this->nameEntryVtx[phi_t1].v.tc[1] =
            this->nameEntryVtx[phi_t1 + 1].v.tc[1] = this->nameEntryVtx[phi_t1 + 2].v.tc[0] = 0;

        this->nameEntryVtx[phi_t1 + 1].v.tc[0] = this->nameEntryVtx[phi_t1 + 2].v.tc[1] =
            this->nameEntryVtx[phi_t1 + 3].v.tc[0] = this->nameEntryVtx[phi_t1 + 3].v.tc[1] = 0x200;

        this->nameEntryVtx[phi_t1].v.cn[0] = this->nameEntryVtx[phi_t1 + 1].v.cn[0] =
            this->nameEntryVtx[phi_t1 + 2].v.cn[0] = this->nameEntryVtx[phi_t1 + 3].v.cn[0] =
                this->nameEntryVtx[phi_t1].v.cn[1] = this->nameEntryVtx[phi_t1 + 1].v.cn[1] =
                    this->nameEntryVtx[phi_t1 + 2].v.cn[1] = this->nameEntryVtx[phi_t1 + 3].v.cn[1] =
                        this->nameEntryVtx[phi_t1].v.cn[2] = this->nameEntryVtx[phi_t1 + 1].v.cn[2] =
                            this->nameEntryVtx[phi_t1 + 2].v.cn[2] = this->nameEntryVtx[phi_t1 + 3].v.cn[2] =
                                this->nameEntryVtx[phi_t1].v.cn[3] = this->nameEntryVtx[phi_t1 + 1].v.cn[3] =
                                    this->nameEntryVtx[phi_t1 + 2].v.cn[3] = this->nameEntryVtx[phi_t1 + 3].v.cn[3] =
                                        0xFF;
    }

    this->nameEntryVtx[1].v.ob[0] = this->nameEntryVtx[3].v.ob[0] = this->nameEntryVtx[0].v.ob[0] + 0x6C;
    this->nameEntryVtx[2].v.ob[1] = this->nameEntryVtx[3].v.ob[1] = this->nameEntryVtx[0].v.ob[1] - 0x10;
    this->nameEntryVtx[1].v.tc[0] = this->nameEntryVtx[3].v.tc[0] = 0xD80;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->windowColor[0], this->windowColor[1], this->windowColor[2],
                    this->nameEntryBoxAlpha);
    gSPVertex(POLY_OPA_DISP++, this->nameEntryVtx, 4, 0);
    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelNameBoxTex, G_IM_FMT_IA, G_IM_SIZ_16b, 108, 16, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);
    gSPVertex(POLY_OPA_DISP++, this->nameEntryVtx + 4, 32, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->nameEntryBoxAlpha);

    for (phi_s0 = 0, phi_v0 = 0; phi_s0 < 0x20; phi_s0 += 4, phi_v0++) {
        file_choose_moji_set(this->state.gfxCtx,
                                 font->fontBuf + this->fileNames[this->buttonIndex][phi_v0] * FONT_CHAR_TEX_SIZE,
                                 phi_s0);
    }

    this->nameEntryVtx[0x25].v.tc[0] = this->nameEntryVtx[0x26].v.tc[1] = this->nameEntryVtx[0x27].v.tc[0] =
        this->nameEntryVtx[0x27].v.tc[1] = this->nameEntryVtx[0x29].v.tc[0] = this->nameEntryVtx[0x2A].v.tc[1] =
            this->nameEntryVtx[0x2B].v.tc[0] = this->nameEntryVtx[0x2B].v.tc[1] = 0x300;

    if (this->kbdButton == FS_KBD_BTN_HIRA || this->kbdButton == FS_KBD_BTN_KATA || this->kbdButton == FS_KBD_BTN_END) {
        this->nameEntryVtx[0x29].v.tc[0] = this->nameEntryVtx[0x2B].v.tc[0] = 0x700;
    } else if (this->kbdButton == FS_KBD_BTN_ENG || this->kbdButton == FS_KBD_BTN_BACKSPACE) {
        this->nameEntryVtx[0x29].v.tc[0] = this->nameEntryVtx[0x2B].v.tc[0] = 0x500;
    }

    CLOSE_DISPS(this->state.gfxCtx, "../z_file_nameset_PAL.c", 307);
}

void nameset_display_set(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Font* font = &this->font;
    s16 i;
    s16 tmp;
    s16 vtx;

    i = vtx = 0;

    OPEN_DISPS(this->state.gfxCtx, "../z_file_nameset_PAL.c", 324);

    kaleido_scope_prim(this->state.gfxCtx);
    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_PASS, G_RM_XLU_SURF2);
    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, 0, 0, 0, COMBINED, 0,
                      0, 0, COMBINED);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, this->charBgAlpha, 255, 255, 255, 255);

#if OOT_NTSC
    if (this->charPage == FS_CHAR_PAGE_HIRA || this->charPage == FS_CHAR_PAGE_HIRA_TO_KATA ||
        this->charPage == FS_CHAR_PAGE_HIRA_TO_ENG) {
        if (this->charPage != FS_CHAR_PAGE_HIRA_TO_ENG) {
            for (; vtx < 0x100; vtx += 32) {
                gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

                for (tmp = 0; tmp < 32; i++, tmp += 4) {
                    morph_moji_set(this->state.gfxCtx,
                                                       font->fontBuf + title_name_hira[i] * FONT_CHAR_TEX_SIZE,
                                                       font->fontBuf + title_name_kata[i] * FONT_CHAR_TEX_SIZE, tmp);
                }
            }
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
            morph_moji_set(this->state.gfxCtx,
                                               font->fontBuf + title_name_hira[i] * FONT_CHAR_TEX_SIZE,
                                               font->fontBuf + title_name_kata[i] * FONT_CHAR_TEX_SIZE, 0);
        } else {
            for (; vtx < 0x100; vtx += 32) {
                gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

                for (tmp = 0; tmp < 32; i++, tmp += 4) {
                    morph_moji_set(this->state.gfxCtx,
                                                       font->fontBuf + title_name_hira[i] * FONT_CHAR_TEX_SIZE,
                                                       font->fontBuf + title_name_eisu[i] * FONT_CHAR_TEX_SIZE, tmp);
                }
            }
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
            morph_moji_set(this->state.gfxCtx,
                                               font->fontBuf + title_name_hira[i] * FONT_CHAR_TEX_SIZE,
                                               font->fontBuf + title_name_eisu[i] * FONT_CHAR_TEX_SIZE, 0);
        }
    } else if (this->charPage == FS_CHAR_PAGE_KATA || this->charPage == FS_CHAR_PAGE_KATA_TO_HIRA ||
               this->charPage == FS_CHAR_PAGE_KATA_TO_ENG) {
        if (this->charPage != FS_CHAR_PAGE_KATA_TO_ENG) {
            for (; vtx < 0x100; vtx += 32) {
                gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

                for (tmp = 0; tmp < 32; i++, tmp += 4) {
                    morph_moji_set(this->state.gfxCtx,
                                                       font->fontBuf + title_name_kata[i] * FONT_CHAR_TEX_SIZE,
                                                       font->fontBuf + title_name_hira[i] * FONT_CHAR_TEX_SIZE, tmp);
                }
            }
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
            morph_moji_set(this->state.gfxCtx,
                                               font->fontBuf + title_name_kata[i] * FONT_CHAR_TEX_SIZE,
                                               font->fontBuf + title_name_hira[i] * FONT_CHAR_TEX_SIZE, 0);
        } else {
            for (; vtx < 0x100; vtx += 32) {
                gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

                for (tmp = 0; tmp < 32; i++, tmp += 4) {
                    morph_moji_set(this->state.gfxCtx,
                                                       font->fontBuf + title_name_kata[i] * FONT_CHAR_TEX_SIZE,
                                                       font->fontBuf + title_name_eisu[i] * FONT_CHAR_TEX_SIZE, tmp);
                }
            }
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
            morph_moji_set(this->state.gfxCtx,
                                               font->fontBuf + title_name_kata[i] * FONT_CHAR_TEX_SIZE,
                                               font->fontBuf + title_name_eisu[i] * FONT_CHAR_TEX_SIZE, 0);
        }
    } else {
        if (this->charPage != FS_CHAR_PAGE_ENG_TO_KATA) {
            for (; vtx < 0x100; vtx += 32) {
                gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

                for (tmp = 0; tmp < 32; i++, tmp += 4) {
                    morph_moji_set(this->state.gfxCtx,
                                                       font->fontBuf + title_name_eisu[i] * FONT_CHAR_TEX_SIZE,
                                                       font->fontBuf + title_name_hira[i] * FONT_CHAR_TEX_SIZE, tmp);
                }
            }
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
            morph_moji_set(this->state.gfxCtx, font->fontBuf + title_name_eisu[i] * FONT_CHAR_TEX_SIZE,
                                               font->fontBuf + title_name_hira[i] * FONT_CHAR_TEX_SIZE, 0);
        } else {
            for (; vtx < 0x100; vtx += 32) {
                gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

                for (tmp = 0; tmp < 32; i++, tmp += 4) {
                    morph_moji_set(this->state.gfxCtx,
                                                       font->fontBuf + title_name_eisu[i] * FONT_CHAR_TEX_SIZE,
                                                       font->fontBuf + title_name_kata[i] * FONT_CHAR_TEX_SIZE, tmp);
                }
            }
            gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
            morph_moji_set(this->state.gfxCtx, font->fontBuf + title_name_eisu[i] * FONT_CHAR_TEX_SIZE,
                                               font->fontBuf + title_name_kata[i] * FONT_CHAR_TEX_SIZE, 0);
        }
    }
#else
    for (; vtx < 0x100; vtx += 32) {
        gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[vtx], 32, 0);

        for (tmp = 0; tmp < 32; i++, tmp += 4) {
            file_choose_moji_set(this->state.gfxCtx, font->fontBuf + title_name_eisu[i] * FONT_CHAR_TEX_SIZE, tmp);
        }
    }

    gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[0x100], 4, 0);
    file_choose_moji_set(this->state.gfxCtx, font->fontBuf + title_name_eisu[i] * FONT_CHAR_TEX_SIZE, 0);
#endif

    CLOSE_DISPS(this->state.gfxCtx, "../z_file_nameset_PAL.c", 347);
}

#if OOT_NTSC
// Tables for applying or removing Japanese diacritics (dakuten and handakuten) to filename characters.
// For the range of characters between RangeMin[i] and RangeMax[i], the modified character can be found
// by adding RangeOffset[i].
static s16 check_data00[12] = { 0x0F, 0x23, 0x41, 0x50, 0x55, 0x5F, 0x73, 0x91, 0xA0, 0xA5, 0x5C, 0xAA };
static s16 check_data01[12] = { 0x1D, 0x27, 0x4F, 0x54, 0x59, 0x6D, 0x77, 0x9F, 0xA4, 0xA9, 0x5C, 0xAA };
static s16 change_data0[12] = { 0x32, 0x2D,  -0x32, 0x05,  -0x32, 0x32,
                                               0x2D, -0x32, 0x05,  -0x32, 0x4E,  -0x4E };
static s16 check_data10[8] = { 0x0F, 0x23, 0x55, 0x5F, 0x73, 0xA5, 0x5C, 0xAA };
static s16 check_data11[8] = { 0x1D, 0x27, 0x59, 0x6D, 0x77, 0xA9, 0x5C, 0xAA };
static s16 change_data1[8] = { 0x32, 0x2D, -0x05, 0x32, 0x2D, -0x05, 0x4E, -0x4E };
static s16 check_data20[4] = { 0x23, 0x50, 0x73, 0xA0 };
static s16 check_data21[4] = { 0x27, 0x54, 0x77, 0xA4 };
static s16 change_data2[4] = { 0x32, 0x05, 0x32, 0x05 };

/**
 * Apply a Japanese diacritic to a character in the filename. The diacritic can be
 * FILENAME_DAKUTEN, FILENAME_HANDAKUTEN, or FILENAME_SPACE (which removes any
 * diacritic). Returns true if the diacritic was successfully applied (or removed).
 */
s32 dakuonn_change(GameState* thisx, s16 diacritic, s16 charIndex) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 i;

    if (diacritic == FILENAME_SPACE) {
        for (i = 0; i < ARRAY_COUNTU(change_data0); i++) {
            if (check_data00[i] <= this->fileNames[this->buttonIndex][charIndex] &&
                this->fileNames[this->buttonIndex][charIndex] <= check_data01[i]) {
                this->fileNames[this->buttonIndex][charIndex] += change_data0[i];
                return true;
            }
        }
    } else if (diacritic == FILENAME_DAKUTEN) {
        for (i = 0; i < ARRAY_COUNTU(change_data1); i++) {
            if (check_data10[i] <= this->fileNames[this->buttonIndex][charIndex] &&
                this->fileNames[this->buttonIndex][charIndex] <= check_data11[i]) {
                this->fileNames[this->buttonIndex][charIndex] += change_data1[i];
                return true;
            }
        }
    } else if (diacritic == FILENAME_HANDAKUTEN) {
        for (i = 0; i < ARRAY_COUNTU(change_data2); i++) {
            if (check_data20[i] <= this->fileNames[this->buttonIndex][charIndex] &&
                this->fileNames[this->buttonIndex][charIndex] <= check_data21[i]) {
                this->fileNames[this->buttonIndex][charIndex] += change_data2[i];
                return true;
            }
        }
    }
    return false;
}

/**
 * Apply a Japanese diacritic to the filename, either replacing the current
 * character or the previous character in the filename. The diacritic can be
 * FILENAME_DAKUTEN, FILENAME_HANDAKUTEN, or FILENAME_SPACE (which removes any
 * diacritic). Returns true if the diacritic was successfully applied (or removed).
 */
s32 dakuonn_change_check(GameState* thisx, s16 diacritic) {
    FileSelectState* this = (FileSelectState*)thisx;
    s32 pad;

    if (!dakuonn_change(&this->state, diacritic, this->newFileNameCharCount)) {
        if (this->newFileNameCharCount != 0) {
            if (!dakuonn_change(&this->state, diacritic, this->newFileNameCharCount - 1)) {
                return false;
            }
        } else {
            return false;
        }
    }
    return true;
}
#endif

void menu_nameset_display(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    Font* font = &this->font;
    Input* input = &this->state.input[0];
    s16 i;
    s16 tmp;
    u16 dayTime;
    s16 validName;

    OPEN_DISPS(this->state.gfxCtx, "../z_file_nameset_PAL.c", 368);

    nameset_vtx_set(&this->state);
    nameset_basic_display(&this->state);
    cursor_color_change(&this->state);

    tmp = (this->newFileNameCharCount * 4) + 4;
    this->nameEntryVtx[36].v.ob[0] = this->nameEntryVtx[38].v.ob[0] = this->nameEntryVtx[tmp].v.ob[0] - 6;
    this->nameEntryVtx[37].v.ob[0] = this->nameEntryVtx[39].v.ob[0] = this->nameEntryVtx[36].v.ob[0] + 24;
    this->nameEntryVtx[36].v.ob[1] = this->nameEntryVtx[37].v.ob[1] = this->nameEntryVtx[tmp].v.ob[1] + 7;
    this->nameEntryVtx[38].v.ob[1] = this->nameEntryVtx[39].v.ob[1] = this->nameEntryVtx[36].v.ob[1] - 24;

    if ((this->kbdButton == FS_KBD_BTN_HIRA) || (this->kbdButton == FS_KBD_BTN_KATA) ||
        (this->kbdButton == FS_KBD_BTN_END)) {
        if (this->kbdX != this->kbdButton) {
            PRINTF("014 xpos=%d  contents=%d\n", this->kbdX, this->kbdButton);
        }
        this->nameEntryVtx[40].v.ob[0] = this->nameEntryVtx[42].v.ob[0] =
            menu_select_titlevtx[(this->kbdX + 1) * 4].v.ob[0] - 4;
        this->nameEntryVtx[41].v.ob[0] = this->nameEntryVtx[43].v.ob[0] = this->nameEntryVtx[40].v.ob[0] + 52;
        this->nameEntryVtx[40].v.ob[1] = this->nameEntryVtx[41].v.ob[1] =
            menu_select_titlevtx[(this->kbdX + 1) * 4].v.ob[1] + 4;

    } else if ((this->kbdButton == FS_KBD_BTN_ENG) || (this->kbdButton == FS_KBD_BTN_BACKSPACE)) {
        if (this->kbdX != this->kbdButton) {
            PRINTF("23 xpos=%d  contents=%d\n", this->kbdX, this->kbdButton);
        }
        this->nameEntryVtx[40].v.ob[0] = this->nameEntryVtx[42].v.ob[0] =
            menu_select_titlevtx[(this->kbdX + 1) * 4].v.ob[0] - 4;
        this->nameEntryVtx[41].v.ob[0] = this->nameEntryVtx[43].v.ob[0] = this->nameEntryVtx[40].v.ob[0] + 40;
        this->nameEntryVtx[40].v.ob[1] = this->nameEntryVtx[41].v.ob[1] =
            menu_select_titlevtx[(this->kbdX + 1) * 4].v.ob[1] + 4;
    } else {
        if (this->charIndex >= 65) {
            PRINTF("mjp=%d  xpos=%d  ypos=%d  name_contents=%d\n", this->charIndex, this->kbdX, this->kbdY,
                   this->kbdButton);
        }

#if OOT_NTSC
        this->nameEntryVtx[40].v.ob[0] = this->nameEntryVtx[42].v.ob[0] =
            this->keyboardVtx[this->charIndex * 4].v.ob[0] - 6;
#else
        this->nameEntryVtx[40].v.ob[0] = this->nameEntryVtx[42].v.ob[0] =
            this->keyboardVtx[this->charIndex * 4].v.ob[0] - D_80812544[this->charIndex] - 6;
#endif
        this->nameEntryVtx[41].v.ob[0] = this->nameEntryVtx[43].v.ob[0] = this->nameEntryVtx[40].v.ob[0] + 24;
        this->nameEntryVtx[40].v.ob[1] = this->nameEntryVtx[41].v.ob[1] =
            this->keyboardVtx[this->charIndex * 4].v.ob[1] + 6;
    }

    this->nameEntryVtx[42].v.ob[1] = this->nameEntryVtx[43].v.ob[1] = this->nameEntryVtx[40].v.ob[1] - 24;

    gSPVertex(POLY_OPA_DISP++, &this->nameEntryVtx[36], 8, 0);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, 1, 0, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, 0, 1, 0, PRIMITIVE, 0, TEXEL0, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->highlightColor[0], this->highlightColor[1], this->highlightColor[2],
                    this->highlightColor[3]);
    gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelCharHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 24, 24, 0,
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                        G_TX_NOLOD);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);

    if ((this->kbdButton == FS_KBD_BTN_HIRA) || (this->kbdButton == FS_KBD_BTN_KATA) ||
        (this->kbdButton == FS_KBD_BTN_END)) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelMediumButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 56, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);

    } else if ((this->kbdButton == FS_KBD_BTN_ENG) || (this->kbdButton == FS_KBD_BTN_BACKSPACE)) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, gFileSelSmallButtonHighlightTex, G_IM_FMT_I, G_IM_SIZ_8b, 40, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
    }

    gSP1Quadrangle(POLY_OPA_DISP++, 4, 6, 7, 5, 0);

    nameset_display_set(&this->state);
    gDPPipeSync(POLY_OPA_DISP++);
    kaleido_scope_prim(this->state.gfxCtx);

    gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);

    if (this->configMode == CM_NAME_ENTRY) {
        if (CHECK_BTN_ALL(input->press.button, BTN_START)) {
            Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_L, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            // place cursor on END button
            this->kbdY = 5;
            this->kbdX = 4;
        } else if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
            if ((this->newFileNameCharCount == 7) && (this->fileNames[this->buttonIndex][7] != FILENAME_SPACE)) {
                for (i = this->newFileNameCharCount; i < 7; i++) {
                    this->fileNames[this->buttonIndex][i] = this->fileNames[this->buttonIndex][i + 1];
                }

                this->fileNames[this->buttonIndex][i] = FILENAME_SPACE;
                Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            } else {
                this->newFileNameCharCount--;

                if (this->newFileNameCharCount < 0) {
                    this->newFileNameCharCount = 0;
                    this->configMode = CM_NAME_ENTRY_TO_MAIN;
                } else {
                    for (i = this->newFileNameCharCount; i < 7; i++) {
                        this->fileNames[this->buttonIndex][i] = this->fileNames[this->buttonIndex][i + 1];
                    }

                    this->fileNames[this->buttonIndex][i] = FILENAME_SPACE;
                    Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                }
            }
        } else {
            if (this->charPage <= FS_CHAR_PAGE_ENG) {
#if OOT_NTSC
                if (this->kbdY != 5) {
                    // draw the character the cursor is hovering over in yellow
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 255);

                    if (this->charPage == FS_CHAR_PAGE_HIRA) {
                        gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[(this->charIndex * 4)], 4, 0);
                        file_choose_moji_set(
                            this->state.gfxCtx, font->fontBuf + title_name_hira[this->charIndex] * FONT_CHAR_TEX_SIZE, 0);
                        if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                            if ((title_name_hira[this->charIndex] == FILENAME_DAKUTEN) ||
                                (title_name_hira[this->charIndex] == FILENAME_HANDAKUTEN)) {
                                if (!dakuonn_change_check(&this->state,
                                                                         title_name_hira[this->charIndex])) {
                                    Nai_FxFlagEntry(NA_SE_SY_FSEL_ERROR, &_dummy_zero_f, 4,
                                                         &_dummy_one, &_dummy_one,
                                                         &_dummy_zero_s8);
                                } else {
                                    Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4,
                                                         &_dummy_one, &_dummy_one,
                                                         &_dummy_zero_s8);
                                }
                            } else {
                                Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4,
                                                     &_dummy_one, &_dummy_one,
                                                     &_dummy_zero_s8);
                                this->fileNames[this->buttonIndex][this->newFileNameCharCount] =
                                    title_name_hira[this->charIndex];
                                this->newFileNameCharCount++;
                                if (this->newFileNameCharCount >= 8) {
                                    this->newFileNameCharCount = 7;
                                }
                            }
                        }
                    } else if (this->charPage == FS_CHAR_PAGE_KATA) {
                        gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[(this->charIndex * 4)], 4, 0);
                        file_choose_moji_set(
                            this->state.gfxCtx, font->fontBuf + title_name_kata[this->charIndex] * FONT_CHAR_TEX_SIZE, 0);
                        if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                            if ((title_name_hira[this->charIndex] == FILENAME_DAKUTEN) ||
                                (title_name_hira[this->charIndex] == FILENAME_HANDAKUTEN)) {
                                if (!dakuonn_change_check(&this->state,
                                                                         title_name_hira[this->charIndex])) {
                                    Nai_FxFlagEntry(NA_SE_SY_FSEL_ERROR, &_dummy_zero_f, 4,
                                                         &_dummy_one, &_dummy_one,
                                                         &_dummy_zero_s8);
                                } else {
                                    Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4,
                                                         &_dummy_one, &_dummy_one,
                                                         &_dummy_zero_s8);
                                }
                            } else {
                                Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4,
                                                     &_dummy_one, &_dummy_one,
                                                     &_dummy_zero_s8);
                                this->fileNames[this->buttonIndex][this->newFileNameCharCount] =
                                    title_name_kata[this->charIndex];
                                this->newFileNameCharCount++;
                                if (this->newFileNameCharCount >= 8) {
                                    this->newFileNameCharCount = 7;
                                }
                            }
                        }
                    } else {
                        gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[this->charIndex * 4], 4, 0);

                        file_choose_moji_set(this->state.gfxCtx,
                                                 font->fontBuf + title_name_eisu[this->charIndex] * FONT_CHAR_TEX_SIZE, 0);

                        if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                            Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4,
                                                 &_dummy_one, &_dummy_one,
                                                 &_dummy_zero_s8);
                            this->fileNames[this->buttonIndex][this->newFileNameCharCount] =
                                title_name_eisu[this->charIndex];
                            this->newFileNameCharCount++;

                            if (this->newFileNameCharCount > 7) {
                                this->newFileNameCharCount = 7;
                            }
                        }
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_A) && (this->charPage != this->kbdButton)) {
                    if (this->kbdButton == FS_KBD_BTN_HIRA) {
                        if (this->charPage == FS_CHAR_PAGE_KATA) {
                            this->charPage = FS_CHAR_PAGE_KATA_TO_HIRA;
                        } else {
                            this->charPage = FS_CHAR_PAGE_ENG_TO_HIRA;
                        }
                        Nai_FxFlagEntry(NA_SE_SY_WIN_OPEN, &_dummy_zero_f, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
                    } else if (this->kbdButton == FS_KBD_BTN_KATA) {
                        if (this->charPage == FS_CHAR_PAGE_HIRA) {
                            this->charPage = FS_CHAR_PAGE_HIRA_TO_KATA;
                        } else {
                            this->charPage = FS_CHAR_PAGE_ENG_TO_KATA;
                        }
                        Nai_FxFlagEntry(NA_SE_SY_WIN_OPEN, &_dummy_zero_f, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
                    } else if (this->kbdButton == FS_KBD_BTN_ENG) {
                        if (this->charPage == FS_CHAR_PAGE_HIRA) {
                            this->charPage = FS_CHAR_PAGE_HIRA_TO_ENG;
                        } else {
                            this->charPage = FS_CHAR_PAGE_KATA_TO_ENG;
                        }
                        Nai_FxFlagEntry(NA_SE_SY_WIN_OPEN, &_dummy_zero_f, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
                    } else {
                        if (this->kbdButton == FS_KBD_BTN_BACKSPACE) {
                            if ((this->newFileNameCharCount == 7) &&
                                (this->fileNames[this->buttonIndex][7] != FILENAME_SPACE)) {
                                for (i = this->newFileNameCharCount; i < 7; i++) {
                                    this->fileNames[this->buttonIndex][i] = this->fileNames[this->buttonIndex][i + 1];
                                }

                                this->fileNames[this->buttonIndex][i] = FILENAME_SPACE;
                                Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4,
                                                     &_dummy_one, &_dummy_one,
                                                     &_dummy_zero_s8);
                            } else {
                                this->newFileNameCharCount--;

                                if (this->newFileNameCharCount < 0) {
                                    this->newFileNameCharCount = 0;
                                }

                                for (i = this->newFileNameCharCount; i < 7; i++) {
                                    this->fileNames[this->buttonIndex][i] = this->fileNames[this->buttonIndex][i + 1];
                                }

                                this->fileNames[this->buttonIndex][i] = FILENAME_SPACE;
                                Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4,
                                                     &_dummy_one, &_dummy_one,
                                                     &_dummy_zero_s8);
                            }
                        } else if (this->kbdButton == FS_KBD_BTN_END) {
                            validName = false;

                            for (i = 0; i < 8; i++) {
                                if (this->fileNames[this->buttonIndex][i] != FILENAME_SPACE) {
                                    validName = true;
                                    break;
                                }
                            }

                            if (validName) {
                                Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_L, &_dummy_zero_f, 4,
                                                     &_dummy_one, &_dummy_one,
                                                     &_dummy_zero_s8);
                                z_common_data.fileNum = this->buttonIndex;
                                dayTime = ((void)0, z_common_data.save.dayTime);
                                sram_start_save(this, &this->sramCtx);
                                z_common_data.save.dayTime = dayTime;
                                this->configMode = CM_NAME_ENTRY_TO_MAIN;
                                this->nameBoxAlpha[this->buttonIndex] = this->nameAlpha[this->buttonIndex] = 200;
                                this->connectorAlpha[this->buttonIndex] = 255;
                                z_vibctl2_vib_setQ(300.0f, 180, 20, 100);
                            } else {
                                Nai_FxFlagEntry(NA_SE_SY_FSEL_ERROR, &_dummy_zero_f, 4,
                                                     &_dummy_one, &_dummy_one,
                                                     &_dummy_zero_s8);
                            }
                        }
                    }
                }

                if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    this->newFileNameCharCount++;

                    if (this->newFileNameCharCount > 7) {
                        this->newFileNameCharCount = 7;
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    this->newFileNameCharCount--;

                    if (this->newFileNameCharCount < 0) {
                        this->newFileNameCharCount = 0;
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_Z)) {
                    if (dakuonn_change_check(&this->state, FILENAME_SPACE)) {
                        Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
                    }
                }
#else
                if (this->kbdY != 5) {
                    // draw the character the cursor is hovering over in yellow
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 0, 255);

                    gSPVertex(POLY_OPA_DISP++, &this->keyboardVtx[this->charIndex * 4], 4, 0);

                    file_choose_moji_set(this->state.gfxCtx,
                                             font->fontBuf + title_name_eisu[this->charIndex] * FONT_CHAR_TEX_SIZE, 0);

                    if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                        Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
                        this->fileNames[this->buttonIndex][this->newFileNameCharCount] = title_name_eisu[this->charIndex];
                        this->newFileNameCharCount++;

                        if (this->newFileNameCharCount > 7) {
                            this->newFileNameCharCount = 7;
                        }
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_A) && (this->charPage != this->kbdButton)) {
                    if (this->kbdButton == FS_KBD_BTN_BACKSPACE) {
                        if ((this->newFileNameCharCount == 7) &&
                            (this->fileNames[this->buttonIndex][7] != FILENAME_SPACE)) {
                            for (i = this->newFileNameCharCount; i < 7; i++) {
                                this->fileNames[this->buttonIndex][i] = this->fileNames[this->buttonIndex][i + 1];
                            }

                            this->fileNames[this->buttonIndex][i] = FILENAME_SPACE;
                            Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4,
                                                 &_dummy_one, &_dummy_one,
                                                 &_dummy_zero_s8);
                        } else {
                            this->newFileNameCharCount--;

                            if (this->newFileNameCharCount < 0) {
                                this->newFileNameCharCount = 0;
                            }

                            for (i = this->newFileNameCharCount; i < 7; i++) {
                                this->fileNames[this->buttonIndex][i] = this->fileNames[this->buttonIndex][i + 1];
                            }

                            this->fileNames[this->buttonIndex][i] = FILENAME_SPACE;
                            Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_S, &_dummy_zero_f, 4,
                                                 &_dummy_one, &_dummy_one,
                                                 &_dummy_zero_s8);
                        }
                    } else if (this->kbdButton == FS_KBD_BTN_END) {
                        validName = false;

                        for (i = 0; i < 8; i++) {
                            if (this->fileNames[this->buttonIndex][i] != FILENAME_SPACE) {
                                validName = true;
                                break;
                            }
                        }

                        if (validName) {
                            Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_L, &_dummy_zero_f, 4,
                                                 &_dummy_one, &_dummy_one,
                                                 &_dummy_zero_s8);
                            z_common_data.fileNum = this->buttonIndex;
                            dayTime = ((void)0, z_common_data.save.dayTime);
                            sram_start_save(this, &this->sramCtx);
                            z_common_data.save.dayTime = dayTime;
                            this->configMode = CM_NAME_ENTRY_TO_MAIN;
                            this->nameBoxAlpha[this->buttonIndex] = this->nameAlpha[this->buttonIndex] = 200;
                            this->connectorAlpha[this->buttonIndex] = 255;
                            z_vibctl2_vib_setQ(300.0f, 180, 20, 100);
                        } else {
                            Nai_FxFlagEntry(NA_SE_SY_FSEL_ERROR, &_dummy_zero_f, 4, &_dummy_one,
                                                 &_dummy_one, &_dummy_zero_s8);
                        }
                    }
                }

                if (CHECK_BTN_ALL(input->press.button, BTN_CRIGHT)) {
                    Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    this->newFileNameCharCount++;

                    if (this->newFileNameCharCount > 7) {
                        this->newFileNameCharCount = 7;
                    }
                } else if (CHECK_BTN_ALL(input->press.button, BTN_CLEFT)) {
                    Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    this->newFileNameCharCount--;

                    if (this->newFileNameCharCount < 0) {
                        this->newFileNameCharCount = 0;
                    }
                }
#endif
            }
        }
    }

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);

    CLOSE_DISPS(this->state.gfxCtx, "../z_file_nameset_PAL.c", 550);
}

/**
 * Fade in the name entry box and slide it to the center of the screen from the right side.
 * After the name entry box is in place, init the keyboard/cursor and change modes.
 * Update function for `CM_START_NAME_ENTRY`
 */
void menu_nameset_fdin(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->nameEntryBoxAlpha += 25;

    if (this->nameEntryBoxAlpha >= 255) {
        this->nameEntryBoxAlpha = 255;
    }

    this->nameEntryBoxPosX -= 30;

    if (this->nameEntryBoxPosX <= 0) {
        this->nameEntryBoxPosX = 0;
        this->nameEntryBoxAlpha = 255;
        this->kbdX = 0;
        this->kbdY = 0;
        this->kbdButton = FS_KBD_BTN_NONE;
        this->configMode = CM_NAME_ENTRY;
    }
}

/**
 * Update the keyboard cursor and play sound effects at the appropriate times.
 * There are many special cases for warping the cursor depending on where
 * the cursor currently is.
 * Update function for `CM_NAME_ENTRY`
 */
void menu_nameset_play(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    s16 prevKbdX;
#if !(PLATFORM_GC && OOT_PAL)
    Input* input = &this->state.input[0];
    s32 pad;
#endif

#if OOT_NTSC
    if (this->charPage <= FS_CHAR_PAGE_ENG) {
        if (CHECK_BTN_ALL(input->press.button, BTN_R)) {
            if (z_common_data.language == LANGUAGE_JPN) {
                if (this->charPage == FS_CHAR_PAGE_HIRA) {
                    this->charPage = FS_CHAR_PAGE_HIRA_TO_KATA;
                    Nai_FxFlagEntry(NA_SE_SY_WIN_OPEN, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                } else if (this->charPage == FS_CHAR_PAGE_KATA) {
                    this->charPage = FS_CHAR_PAGE_KATA_TO_ENG;
                    Nai_FxFlagEntry(NA_SE_SY_WIN_OPEN, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                } else if (this->charPage == FS_CHAR_PAGE_ENG) {
                    this->charPage = FS_CHAR_PAGE_ENG_TO_HIRA;
                    Nai_FxFlagEntry(NA_SE_SY_WIN_OPEN, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                }
            }
        } else {
            this->kbdButton = FS_KBD_BTN_NONE;

            if (this->kbdY != 5) {
                if (this->stickAdjX < -30) {
                    Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    this->charIndex--;
                    this->kbdX--;
                    if (this->kbdX < 0) {
                        this->kbdX = 12;
                        this->charIndex = this->kbdY * 13 + this->kbdX;
                    }
                } else if (this->stickAdjX > 30) {
                    Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    this->charIndex++;
                    this->kbdX++;
                    if (this->kbdX >= 13) {
                        this->kbdX = 0;
                        this->charIndex = this->kbdY * 13 + this->kbdX;
                    }
                }
            } else if (z_common_data.language == LANGUAGE_JPN) {
                if (this->stickAdjX < -30) {
                    Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    this->kbdX--;
                    if (this->kbdX < 0) {
                        this->kbdX = 4;
                    }
                } else if (this->stickAdjX > 30) {
                    Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    this->kbdX++;
                    if (this->kbdX >= 5) {
                        this->kbdX = 0;
                    }
                }
            } else {
                if (this->stickAdjX < -30) {
                    Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    this->kbdX--;
                    if (this->kbdX < 3) {
                        this->kbdX = 4;
                    }
                } else if (this->stickAdjX > 30) {
                    Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    this->kbdX++;
                    if (this->kbdX >= 5) {
                        this->kbdX = 3;
                    }
                }
            }

            if (this->stickAdjY > 30) {
                Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
                this->kbdY--;
                if (this->kbdY < 0) {
                    if (z_common_data.language == LANGUAGE_JPN) {
                        this->kbdY = 5;
                        this->charIndex += 52;
                        prevKbdX = this->kbdX;
                        if (this->kbdX < 3) {
                            this->kbdX = 0;
                        } else if (this->kbdX < 6) {
                            this->kbdX = 1;
                        } else if (this->kbdX < 8) {
                            this->kbdX = 2;
                        } else if (this->kbdX < 10) {
                            this->kbdX = 3;
                        } else if (this->kbdX < 13) {
                            this->kbdX = 4;
                        }

                        this->unk_1CAD6[this->kbdX] = prevKbdX;
                    } else {
                        // don't go to bottom row
                        if (this->kbdX < 8) {
                            this->kbdY = 4;
                            this->charIndex = this->kbdX + 52;
                        } else {
                            this->kbdY = 5;
                            this->charIndex += 52;
                            prevKbdX = this->kbdX;
                            if (this->kbdX < 10) {
                                this->kbdX = 3;
                            } else if (this->kbdX < 13) {
                                this->kbdX = 4;
                            }

                            this->unk_1CAD6[this->kbdX] = prevKbdX;
                        }
                    }
                } else {
                    this->charIndex -= 13;
                    if (this->kbdY == 4) {
                        this->charIndex = 52;
                        this->kbdX = this->unk_1CAD6[this->kbdX];
                        this->charIndex += this->kbdX;
                    }
                }
            } else if (this->stickAdjY < -30) {
                Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
                this->kbdY++;

                if (this->kbdY >= 6) {
                    this->kbdY = 0;
                    this->kbdX = this->unk_1CAD6[this->kbdX];
                    this->charIndex = this->kbdX;
                } else {
                    this->charIndex += 13;

                    if (this->kbdY == 5) {
                        if (z_common_data.language != LANGUAGE_JPN) {
                            if (this->kbdX < 8) {
                                this->kbdY = 0;
                                this->charIndex = this->kbdX;
                            } else {
                                prevKbdX = this->kbdX;

                                if (this->kbdX < 3) {
                                    this->kbdX = 0;
                                } else if (this->kbdX < 6) {
                                    this->kbdX = 1;
                                } else if (this->kbdX < 8) {
                                    this->kbdX = 2;
                                } else if (this->kbdX < 10) {
                                    this->kbdX = 3;
                                } else if (this->kbdX < 13) {
                                    this->kbdX = 4;
                                }

                                this->unk_1CAD6[this->kbdX] = prevKbdX;
                            }
                        } else {
                            prevKbdX = this->kbdX;

                            if (this->kbdX < 3) {
                                this->kbdX = 0;
                            } else if (this->kbdX < 6) {
                                this->kbdX = 1;
                            } else if (this->kbdX < 8) {
                                this->kbdX = 2;
                            } else if (this->kbdX < 10) {
                                this->kbdX = 3;
                            } else if (this->kbdX < 13) {
                                this->kbdX = 4;
                            }

                            this->unk_1CAD6[this->kbdX] = prevKbdX;
                        }
                    }
                }
            }
            if (this->kbdY == 5) {
                this->kbdButton = this->kbdX;
            }
        }
    } else {
        this->charBgAlpha += ZREG(9);
        if (this->charBgAlpha >= 255) {
            this->charBgAlpha = 0;
            this->charPage = alpha_index[this->charPage];
        }
    }
#else
    this->kbdButton = FS_KBD_BTN_NONE;

    if (this->kbdY != 5) {
        if (this->stickAdjX < -30) {
            Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            this->charIndex--;
            this->kbdX--;
            if (this->kbdX < 0) {
                this->kbdX = 12;
                this->charIndex = this->kbdY * 13 + this->kbdX;
            }
        } else if (this->stickAdjX > 30) {
            Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            this->charIndex++;
            this->kbdX++;
            if (this->kbdX >= 13) {
                this->kbdX = 0;
                this->charIndex = this->kbdY * 13 + this->kbdX;
            }
        }
    } else {
        if (this->stickAdjX < -30) {
            Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            this->kbdX--;
            if (this->kbdX < 3) {
                this->kbdX = 4;
            }
        } else if (this->stickAdjX > 30) {
            Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            this->kbdX++;
            if (this->kbdX >= 5) {
                this->kbdX = 3;
            }
        }
    }

    if (this->stickAdjY > 30) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->kbdY--;
        if (this->kbdY < 0) {
            // don't go to bottom row
            if (this->kbdX < 8) {
                this->kbdY = 4;
                this->charIndex = this->kbdX + 52;
            } else {
                this->kbdY = 5;
                this->charIndex += 52;
                prevKbdX = this->kbdX;
                if (this->kbdX < 10) {
                    this->kbdX = 3;
                } else if (this->kbdX < 13) {
                    this->kbdX = 4;
                }

                this->unk_1CAD6[this->kbdX] = prevKbdX;
            }
        } else {
            this->charIndex -= 13;
            if (this->kbdY == 4) {
                this->charIndex = 52;
                this->kbdX = this->unk_1CAD6[this->kbdX];
                this->charIndex += this->kbdX;
            }
        }
    } else if (this->stickAdjY < -30) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->kbdY++;

        if (this->kbdY >= 6) {
            this->kbdY = 0;
            this->kbdX = this->unk_1CAD6[this->kbdX];
            this->charIndex = this->kbdX;
        } else {
            this->charIndex += 13;

            if (this->kbdY == 5) {
                if (this->kbdX < 8) {
                    this->kbdY = 0;
                    this->charIndex = this->kbdX;
                } else {
                    prevKbdX = this->kbdX;

                    if (this->kbdX < 3) {
                        this->kbdX = 0;
                    } else if (this->kbdX < 6) {
                        this->kbdX = 1;
                    } else if (this->kbdX < 8) {
                        this->kbdX = 2;
                    } else if (this->kbdX < 10) {
                        this->kbdX = 3;
                    } else if (this->kbdX < 13) {
                        this->kbdX = 4;
                    }

                    this->unk_1CAD6[this->kbdX] = prevKbdX;
                }
            }
        }
    }
    if (this->kbdY == 5) {
        this->kbdButton = this->kbdX;
    }
#endif
}

/**
 * This function is mostly a copy paste of `menu_nameset_fdin`.
 * The name entry box fades and slides in even though it is not visible.
 * After this is complete, change to the options config mode.
 * Update function for `CM_START_OPTIONS`
 */
void menu_option_fdin(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;

    this->nameEntryBoxAlpha += 25;

    if (this->nameEntryBoxAlpha >= 255) {
        this->nameEntryBoxAlpha = 255;
    }

    this->nameEntryBoxPosX -= 30;

    if (this->nameEntryBoxPosX <= 0) {
        this->nameEntryBoxPosX = 0;
        this->nameEntryBoxAlpha = 255;
        this->configMode = CM_OPTIONS_MENU;
    }
}

static u8 H_Move_index;

/**
 * Update the cursor and appropriate settings for the options menu.
 * If the player presses B, write the selected options to the SRAM header
 * and set config mode to rotate back to the main menu.
 * Update function for `CM_OPTIONS_MENU`
 */
void menu_option_play(GameState* thisx) {
    FileSelectState* this = (FileSelectState*)thisx;
    SramContext* sramCtx = &this->sramCtx;
    Input* input = &this->state.input[0];

    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_L, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->configMode = CM_OPTIONS_TO_MAIN;
        sramCtx->readBuff[0] = z_common_data.audioSetting;
        sramCtx->readBuff[1] = z_common_data.zTargetSetting;
#if OOT_PAL_N64
        sramCtx->readBuff[2] = z_common_data.language;
#endif
        PRINTF("ＳＡＶＥ");
        sram_sound_save(sramCtx);
        PRINTF_COLOR_YELLOW();
        PRINTF("sram->read_buff[2] = J_N = %x\n", sramCtx->readBuff[2]);
        PRINTF("sram->read_buff[2] = J_N = %x\n", &sramCtx->readBuff[2]);
        PRINTF("Na_SetSoundOutputMode = %d\n", z_common_data.audioSetting);
        PRINTF("Na_SetSoundOutputMode = %d\n", z_common_data.audioSetting);
        PRINTF("Na_SetSoundOutputMode = %d\n", z_common_data.audioSetting);
        PRINTF_RST();
        Na_SetSoundOutputMode(z_common_data.audioSetting);
        PRINTF("終了\n");
        return;
    }

    if (this->stickAdjX < -30) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);

        if (H_Move_index == FS_SETTING_AUDIO) {
            z_common_data.audioSetting--;

            // because audio setting is unsigned, can't check for < 0
            if (z_common_data.audioSetting > 0xF0) {
                z_common_data.audioSetting = FS_AUDIO_SURROUND;
            }
        } else {
#if !OOT_PAL_N64
            z_common_data.zTargetSetting ^= 1;
#else
            if (H_Move_index == FS_SETTING_TARGET) {
                z_common_data.zTargetSetting ^= 1;
            } else {
                z_common_data.language--;
                if (z_common_data.language >= LANGUAGE_MAX) {
                    z_common_data.language = LANGUAGE_MAX - 1;
                }
            }
#endif
        }
    } else if (this->stickAdjX > 30) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);

        if (H_Move_index == FS_SETTING_AUDIO) {
            z_common_data.audioSetting++;

            if (z_common_data.audioSetting > FS_AUDIO_SURROUND) {
                z_common_data.audioSetting = FS_AUDIO_STEREO;
            }
        } else {
#if !OOT_PAL_N64
            z_common_data.zTargetSetting ^= 1;
#else
            if (H_Move_index == FS_SETTING_TARGET) {
                z_common_data.zTargetSetting ^= 1;
            } else {
                z_common_data.language++;
                if (z_common_data.language >= LANGUAGE_MAX) {
                    z_common_data.language = 0;
                }
            }
#endif
        }
    }

#if !OOT_PAL_N64
    if ((this->stickAdjY < -30) || (this->stickAdjY > 30)) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        H_Move_index ^= 1;
    } else if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_L, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        H_Move_index ^= 1;
    }
#else
    if (this->stickAdjY < -30) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        H_Move_index++;
        if (H_Move_index >= FS_SETTING_MAX) {
            H_Move_index = 0;
        }
    } else if (this->stickAdjY > 30) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_CURSOR, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        H_Move_index--;
        if (H_Move_index >= FS_SETTING_MAX) {
            H_Move_index = FS_SETTING_MAX - 1;
        }
    } else if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
        Nai_FxFlagEntry(NA_SE_SY_FSEL_DECIDE_L, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        H_Move_index++;
        if (H_Move_index >= FS_SETTING_MAX) {
            H_Move_index = 0;
        }
    }
#endif
}

#if OOT_NTSC

typedef struct OptionsMenuTextureInfo {
    /* 0x00 */ void* texture[2];
    /* 0x08 */ u16 width;
    /* 0x0A */ u16 height;
} OptionsMenuTextureInfo; // size = 0x0C

#define OPTIONS_MENU_TEXTURE_WIDTH(info) info.width
#define OPTIONS_MENU_TEXTURE_HEIGHT(info) info.height

static OptionsMenuTextureInfo h_op_data[] = {
    { { gFileSelOptionsJPNTex, gFileSelOptionsENGTex }, 128, 16 },
    { { gFileSelSOUNDENGTex, gFileSelSOUNDENGTex }, 64, 16 },
    { { gFileSelZTargetingJPNTex, gFileSelZTargetingENGTex }, 64, 16 },
    { { gFileSelCheckBrightnessJPNTex, gFileSelCheckBrightnessENGTex }, 96, 16 },
};

static OptionsMenuTextureInfo h_op_data2[] = {
    { { gFileSelStereoJPNTex, gFileSelStereoENGTex }, 48, 16 },
    { { gFileSelMonoJPNTex, gFileSelMonoENGTex }, 48, 16 },
    { { gFileSelHeadsetJPNTex, gFileSelHeadsetENGTex }, 48, 16 },
    { { gFileSelSurroundJPNTex, gFileSelSurroundENGTex }, 48, 16 },
    { { gFileSelSwitchJPNTex, gFileSelSwitchENGTex }, 48, 16 },
    { { gFileSelHoldJPNTex, gFileSelHoldENGTex }, 48, 16 },
};

#elif OOT_PAL_N64

static u16 sZTargetSettingWidths[2][3] = {
    { 48, 80, 48 },
    { 48, 80, 48 },
};

typedef struct LanguageChoiceTextureInfo {
    /* 0x00 */ void* texture;
    /* 0x04 */ u16 width;
    /* 0x06 */ u16 height;
} LanguageChoiceTextureInfo; // size = 0x10

static LanguageChoiceTextureInfo sLanguageChoices[] = {
    { gFileSelLanguageChoiceENGTex, 48, 16 },
    { gFileSelLanguageChoiceGERTex, 48, 16 },
    { gFileSelLanguageChoiceFRATex, 48, 16 },
};

typedef struct OptionsMenuTextureInfo {
    /* 0x00 */ void* texture[3];
    /* 0x0C */ u16 width;
    /* 0x0E */ u16 height;
} OptionsMenuTextureInfo; // size = 0x10

#define OPTIONS_MENU_TEXTURE_WIDTH(info) info.width
#define OPTIONS_MENU_TEXTURE_HEIGHT(info) info.height

static OptionsMenuTextureInfo h_op_data[] = {
    { { gFileSelOptionsENGTex, gFileSelOptionsGERTex, gFileSelOptionsENGTex }, 128, 16 },
    { { gFileSelSOUNDENGTex, gFileSelSOUNDENGTex, gFileSelSOUNDFRATex }, 64, 16 },
    { { gFileSelZTargetingENGTex, gFileSelZTargetingGERTex, gFileSelZTargetingFRATex }, 64, 16 },
    { { gFileSelCheckBrightnessENGTex, gFileSelCheckBrightnessGERTex, gFileSelCheckBrightnessFRATex }, 128, 16 },
    { { gFileSelLanguageENGTex, gFileSelLanguageGERTex, gFileSelLanguageFRATex }, 64, 16 },
};

static OptionsMenuTextureInfo h_op_data2[] = {
    { { gFileSelStereoENGTex, gFileSelStereoENGTex, gFileSelStereoFRATex }, 48, 16 },
    { { gFileSelMonoENGTex, gFileSelMonoENGTex, gFileSelMonoENGTex }, 48, 16 },
    { { gFileSelHeadsetENGTex, gFileSelHeadsetGERTex, gFileSelHeadsetFRATex }, 48, 16 },
    { { gFileSelSurroundENGTex, gFileSelSurroundENGTex, gFileSelSurroundENGTex }, 48, 16 },
    { { gFileSelSwitchENGTex, gFileSelSwitchGERTex, gFileSelSwitchFRATex }, 48, 16 },
    { { gFileSelHoldENGTex, gFileSelHoldGERTex, gFileSelHoldFRATex }, 48, 16 },
    { { gFileSelSwitchENGTex, gFileSelSwitchGERTex, gFileSelSwitchFRATex }, 48, 16 },
    { { gFileSelHoldENGTex, gFileSelHoldGERTex, gFileSelHoldFRATex }, 48, 16 },
};

#else

typedef struct OptionsMenuTextureInfo {
    /* 0x00 */ void* texture[3];
    /* 0x0C */ u16 width[3];
    /* 0x12 */ u16 height;
} OptionsMenuTextureInfo; // size = 0x14

#define OPTIONS_MENU_TEXTURE_WIDTHS(jpn, eng, ger, fra) \
    { eng, ger, fra }
#define OPTIONS_MENU_TEXTURE_WIDTH(info) info.width[z_common_data.language]
#define OPTIONS_MENU_TEXTURE_HEIGHT(info) info.height

static OptionsMenuTextureInfo h_op_data[] = {
    { { gFileSelOptionsENGTex, gFileSelOptionsGERTex, gFileSelOptionsENGTex }, { 128, 128, 128 }, 16 },
    { { gFileSelSOUNDENGTex, gFileSelSOUNDENGTex, gFileSelSOUNDFRATex }, { 64, 64, 64 }, 16 },
    { { gFileSelZTargetingENGTex, gFileSelZTargetingGERTex, gFileSelZTargetingFRATex }, { 64, 144, 64 }, 16 },
    { { gFileSelCheckBrightnessENGTex, gFileSelCheckBrightnessGERTex, gFileSelCheckBrightnessFRATex },
      { 128, 128, 128 },
      16 },
};

static OptionsMenuTextureInfo h_op_data2[] = {
    { { gFileSelStereoENGTex, gFileSelStereoENGTex, gFileSelStereoFRATex }, { 48, 48, 48 }, 16 },
    { { gFileSelMonoENGTex, gFileSelMonoENGTex, gFileSelMonoENGTex }, { 48, 48, 48 }, 16 },
    { { gFileSelHeadsetENGTex, gFileSelHeadsetGERTex, gFileSelHeadsetFRATex }, { 48, 48, 48 }, 16 },
    { { gFileSelSurroundENGTex, gFileSelSurroundENGTex, gFileSelSurroundENGTex }, { 48, 48, 48 }, 16 },
    { { gFileSelSwitchENGTex, gFileSelSwitchGERTex, gFileSelSwitchFRATex }, { 48, 80, 48 }, 16 },
    { { gFileSelHoldENGTex, gFileSelHoldGERTex, gFileSelHoldFRATex }, { 48, 80, 48 }, 16 },
};

#endif

void option_basic_display(GameState* thisx) {
    static s16 p_red = 255;
    static s16 p_green = 255;
    static s16 p_blue = 255;
    static s16 e_red = 0;
    static s16 e_green = 0;
    static s16 e_blue = 0;
    static s16 mp = 1;
    static s16 mc = 20;
    static s16 prim_col[][3] = {
        { 255, 255, 255 },
        { 0, 255, 255 },
    };
    static s16 env_col[][3] = {
        { 0, 0, 0 },
        { 0, 150, 150 },
    };
    FileSelectState* this = (FileSelectState*)thisx;
    s16 cursorRed;
    s16 cursorGreen;
    s16 cursorBlue;
#if !OOT_PAL_N64
    s16 i;
    s16 j;
    s16 vtx;
#else
    s16 startIndex;
    s32 endIndex;
    s32 i;
    s32 vtx;
    s32 pad;
#endif

    OPEN_DISPS(this->state.gfxCtx, "../z_file_nameset_PAL.c", 848);

    cursorRed = ABS(p_red - prim_col[mp][0]) / mc;
    cursorGreen = ABS(p_green - prim_col[mp][1]) / mc;
    cursorBlue = ABS(p_blue - prim_col[mp][2]) / mc;

    if (p_red >= prim_col[mp][0]) {
        p_red -= cursorRed;
    } else {
        p_red += cursorRed;
    }

    if (p_green >= prim_col[mp][1]) {
        p_green -= cursorGreen;
    } else {
        p_green += cursorGreen;
    }

    if (p_blue >= prim_col[mp][2]) {
        p_blue -= cursorBlue;
    } else {
        p_blue += cursorBlue;
    }

    cursorRed = ABS(e_red - env_col[mp][0]) / mc;
    cursorGreen = ABS(e_green - env_col[mp][1]) / mc;
    cursorBlue = ABS(e_blue - env_col[mp][2]) / mc;

    if (e_red >= env_col[mp][0]) {
        e_red -= cursorRed;
    } else {
        e_red += cursorRed;
    }

    if (e_green >= env_col[mp][1]) {
        e_green -= cursorGreen;
    } else {
        e_green += cursorGreen;
    }

    if (e_blue >= env_col[mp][2]) {
        e_blue -= cursorBlue;
    } else {
        e_blue += cursorBlue;
    }

    if (--mc == 0) {
        p_red = prim_col[mp][0];
        p_green = prim_col[mp][1];
        p_blue = prim_col[mp][2];

        e_red = env_col[mp][0];
        e_green = env_col[mp][1];
        e_blue = env_col[mp][2];

        mc = 20;

        if (++mp > 1) {
            mp = 0;
        }
    }

#if OOT_NTSC
    gSPVertex(POLY_OPA_DISP++, menu_option_titlevtx, 32, 0);
#elif OOT_PAL_N64
    gSPVertex(POLY_OPA_DISP++, menu_option_titlevtx, 24, 0);
#else
    if (z_common_data.language == LANGUAGE_GER) {
        gSPVertex(POLY_OPA_DISP++, gOptionsMenuHeadersGERVtx, 32, 0);
    } else {
        gSPVertex(POLY_OPA_DISP++, menu_option_titlevtx, 32, 0);
    }
#endif

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCombineLERP(POLY_OPA_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

#if !OOT_PAL_N64
    for (i = 0, vtx = 0; i < 4; i++, vtx += 4) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, h_op_data[i].texture[z_common_data.language], G_IM_FMT_IA,
                            G_IM_SIZ_8b, OPTIONS_MENU_TEXTURE_WIDTH(h_op_data[i]),
                            OPTIONS_MENU_TEXTURE_HEIGHT(h_op_data[i]), 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }
#else
    for (i = 0, vtx = 0; i < 5; i++, vtx += 4) {
        if (i == 2 && z_common_data.language == LANGUAGE_GER) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, h_op_data[i].texture[z_common_data.language], G_IM_FMT_IA,
                                G_IM_SIZ_8b, 144, OPTIONS_MENU_TEXTURE_HEIGHT(h_op_data[i]), 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK,
                                G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, vtx + 12, vtx + 14, vtx + 15, vtx + 13, 0);
        } else {
            gDPLoadTextureBlock(POLY_OPA_DISP++, h_op_data[i].texture[z_common_data.language], G_IM_FMT_IA,
                                G_IM_SIZ_8b, OPTIONS_MENU_TEXTURE_WIDTH(h_op_data[i]),
                                OPTIONS_MENU_TEXTURE_HEIGHT(h_op_data[i]), 0, G_TX_NOMIRROR | G_TX_WRAP,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
            gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
        }
    }
#endif

#if OOT_NTSC
    gSPVertex(POLY_OPA_DISP++, menu_option_plate, 32, 0);
#elif OOT_PAL_N64
    gSPVertex(POLY_OPA_DISP++, menu_option_plate, 32, 0);
#else
    if (z_common_data.language == LANGUAGE_GER) {
        gSPVertex(POLY_OPA_DISP++, gOptionsMenuSettingsGERVtx, 32, 0);
    } else {
        gSPVertex(POLY_OPA_DISP++, menu_option_plate, 32, 0);
    }
#endif

    for (i = 0, vtx = 0; i < 4; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);
        if (i == z_common_data.audioSetting) {
            if (H_Move_index == FS_SETTING_AUDIO) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, p_red, p_green, p_blue,
                                this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, e_red, e_green, e_blue, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[0]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        //! @bug Mistakenly using h_op_data instead of h_op_data2 for the height.
        //! This works out anyway because all heights are 16.
        gDPLoadTextureBlock(POLY_OPA_DISP++, h_op_data2[i].texture[z_common_data.language], G_IM_FMT_IA,
                            G_IM_SIZ_8b, OPTIONS_MENU_TEXTURE_WIDTH(h_op_data2[i]),
                            OPTIONS_MENU_TEXTURE_HEIGHT(h_op_data[i]), 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

#if !OOT_PAL_N64
    for (; i < 6; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);

        if (i == (z_common_data.zTargetSetting + 4)) {
            if (H_Move_index != FS_SETTING_AUDIO) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, p_red, p_green, p_blue,
                                this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, e_red, e_green, e_blue, 0xFF);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[0]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

#ifndef AVOID_UB
        //! @bug Mistakenly using h_op_data instead of h_op_data2 for the height.
        //! This is also an OOB read that happens to access the height of the first two elements in
        //! h_op_data2, and since all heights are 16, it works out anyway.
#define sOptionsMenuSettingsBug h_op_data
#else
        // Avoid UB: Use the correct array for the heights to avoid reading out of bounds memory that may not
        // happen to work out nicely.
#define sOptionsMenuSettingsBug h_op_data2
#endif
        gDPLoadTextureBlock(POLY_OPA_DISP++, h_op_data2[i].texture[z_common_data.language], G_IM_FMT_IA,
                            G_IM_SIZ_8b, OPTIONS_MENU_TEXTURE_WIDTH(h_op_data2[i]),
                            OPTIONS_MENU_TEXTURE_HEIGHT(sOptionsMenuSettingsBug[i]), 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
#undef sOptionsMenuSettingsBug
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }
#else
    if (z_common_data.language == LANGUAGE_GER) {
        endIndex = 8;
        startIndex = 6;
        i += 2;
        vtx += 8;
    } else {
        endIndex = 6;
        startIndex = 4;
    }

    for (; i < endIndex; i++, vtx += 4) {
        s32 j = i - startIndex;

        gDPPipeSync(POLY_OPA_DISP++);

        if (j == z_common_data.zTargetSetting) {
            if (H_Move_index == FS_SETTING_TARGET) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, p_red, p_green, p_blue,
                                this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, e_red, e_green, e_blue, 0xFF);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[0]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

#ifndef AVOID_UB
        //! @bug Mistakenly using h_op_data instead of h_op_data2 for the height.
        //! This is also an OOB read that happens to access the height of up to the first three elements
        //! in h_op_data2, and since all heights are 16, it works out anyway.
#define sOptionsMenuSettingsBug h_op_data
#else
        // Avoid UB: Use the correct array for the heights to avoid reading out of bounds memory that may not
        // happen to work out nicely.
#define sOptionsMenuSettingsBug h_op_data2
#endif
        gDPLoadTextureBlock(POLY_OPA_DISP++, h_op_data2[i].texture[z_common_data.language], G_IM_FMT_IA,
                            G_IM_SIZ_8b, sZTargetSettingWidths[j][z_common_data.language],
                            OPTIONS_MENU_TEXTURE_HEIGHT(sOptionsMenuSettingsBug[i]), 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
#undef sOptionsMenuSettingsBug
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }

    gSPVertex(POLY_OPA_DISP++, gOptionsMenuBrightnessVtx, 8, 0);
    vtx = 0;
#endif

    gDPPipeSync(POLY_OPA_DISP++);

    // check brightness bars
    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelBrightnessCheckTex, G_IM_FMT_IA, 96, 16, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 55, 55, 55, this->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 40, 40, 40, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    vtx += 4;

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 30, 30, 30, this->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);

    vtx += 4;

#if OOT_PAL_N64
    gSPVertex(POLY_OPA_DISP++, gOptionsMenuLanguageVtx, 12, 0);

    for (i = 0, vtx = 0; i != LANGUAGE_MAX; i++, vtx += 4) {
        gDPPipeSync(POLY_OPA_DISP++);

        if (i == z_common_data.language) {
            if (H_Move_index == FS_SETTING_LANGUAGE) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, p_red, p_green, p_blue,
                                this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, e_red, e_green, e_blue, 0xFF);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, this->titleAlpha[0]);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            }
        } else {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 120, 120, 120, this->titleAlpha[0]);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        }

        gDPLoadTextureBlock(POLY_OPA_DISP++, sLanguageChoices[i].texture, G_IM_FMT_IA, G_IM_SIZ_8b,
                            sLanguageChoices[i].width, sLanguageChoices[i].height, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSP1Quadrangle(POLY_OPA_DISP++, vtx, vtx + 2, vtx + 3, vtx + 1, 0);
    }
#endif

    // blue divider lines
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 255, 255, this->titleAlpha[0]);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    gDPLoadTextureBlock_4b(POLY_OPA_DISP++, gFileSelOptionsDividerTex, G_IM_FMT_IA, 256, 2, 0,
                           G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                           G_TX_NOLOD);

    Matrix_push();
    Matrix_translate(0.0f, 0.1f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx, "../z_file_nameset_PAL.c", 1009);
    gSPVertex(POLY_OPA_DISP++, menu_option_line, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_pull();

    Matrix_push();
    Matrix_translate(0.0f, 0.2f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx, "../z_file_nameset_PAL.c", 1021);

    gSPVertex(POLY_OPA_DISP++, gOptionsDividerZTargetVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_pull();

    Matrix_push();
    Matrix_translate(0.0f, 0.4f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx, "../z_file_nameset_PAL.c", 1033);
    gSPVertex(POLY_OPA_DISP++, gOptionsDividerBrightnessVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_pull();

#if OOT_PAL_N64
    Matrix_push();
    Matrix_translate(0.0f, 0.8f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, this->state.gfxCtx, "../z_file_nameset_PAL.c", UNK_LINE);
    gSPVertex(POLY_OPA_DISP++, gOptionsDividerLanguageVtx, 4, 0);
    gSP1Quadrangle(POLY_OPA_DISP++, 0, 2, 3, 1, 0);
    Matrix_pull();
#endif

    CLOSE_DISPS(this->state.gfxCtx, "../z_file_nameset_PAL.c", 1040);
}

void menu_option_display(GameState* thisx) {
    option_basic_display(thisx);
}
