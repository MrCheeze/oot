#include "ultra64.h"
#include "z64.h"
#include "macros.h"
#include "z_fbdemo_wipe1_data.h"
#include "assets/misc/link_animetion/link_animetion.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

static Vtx wipe1_v[] = {
#include "assets/code/fbdemo_wipe1/wipe1_v.vtx.inc"
};

static u64 g_wipe1_txt[] = {
#include "assets/code/fbdemo_wipe1/g_wipe1_txt.i4.inc.c"
};

static Gfx wipe1_modelT[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH),
    gsSPSetGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_1PRIMITIVE,
                     G_AC_NONE | G_ZS_PRIM | G_RM_PASS | G_RM_AA_ZB_TEX_EDGE2),
    gsDPSetCombineLERP(TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, PRIM_LOD_FRAC, TEXEL0, COMBINED, 0,
                       PRIMITIVE, 0, COMBINED, 0, PRIMITIVE, 0),
    gsDPSetPrimDepth(0, 0),
    gsDPLoadTextureBlock_4b(g_wipe1_txt, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 6,
                            6, 11, G_TX_NOLOD),
    gsDPLoadMultiBlock_4b(g_wipe1_txt, 0x0100, 1, G_IM_FMT_I, 64, 64, 0, G_TX_NOMIRROR | G_TX_WRAP,
                          G_TX_MIRROR | G_TX_WRAP, 6, 6, 11, 1),
    gsDPSetTextureLUT(G_TT_NONE),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPDisplayList(0x08000000),
    gsSPVertex(wipe1_v, 25, 0),
    gsSP2Triangles(0, 1, 2, 0, 1, 3, 4, 0),
    gsSP2Triangles(5, 6, 7, 0, 6, 8, 9, 0),
    gsSP2Triangles(8, 10, 11, 0, 10, 12, 13, 0),
    gsSP2Triangles(12, 14, 15, 0, 14, 16, 17, 0),
    gsSP2Triangles(16, 18, 19, 0, 18, 20, 21, 0),
    gsSP2Triangles(20, 22, 23, 0, 22, 0, 24, 0),
    gsSPEndDisplayList(),
};

