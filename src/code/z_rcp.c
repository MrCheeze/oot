#include "global.h"

Gfx z_gsCPModeSet_Data[SETUPDL_MAX][6] = {
    {
        /* SETUPDL_0 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0, 0, 0, 0, COMBINED, 0,
                           0, 0, COMBINED),
        gsDPSetOtherMode(G_AD_NOISE | G_CD_NOISE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_ZB_CLD_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_1 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_2 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_OPA_SURF | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_3 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_4 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineLERP(PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE, PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_SURF | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_5 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_SURF | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_6 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_TEX_EDGE2),
        gsSPLoadGeometryMode(G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_7 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_XLU_SURF | G_RM_AA_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_8 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_OPA_SURF | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_9 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_10 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_OPA_SURF | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_11 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_12 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_OPA_SURF | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_13 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_14 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_TEX_EDGE | G_RM_AA_ZB_TEX_EDGE2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_15 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_TEX_EDGE2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_16 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_17 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_TEX_EDGE | G_RM_AA_ZB_TEX_EDGE2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_18 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_TEX_EDGE2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_19 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | AA_EN | Z_CMP | IM_RD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             FORCE_BL | GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) |
                             GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_20 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_ZB_CLD_SURF | G_RM_ZB_CLD_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_21 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_SURF | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_22 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_23 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_OPA_SURF | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_24 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIA2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_25 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_26 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_OPA_SURF | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_27 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_SURF | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_28 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_RGBA16 | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_AA_TEX_EDGE | G_RM_AA_TEX_EDGE2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_29 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, TEXEL0, 0, SHADE, 0, NOISE, 0, COMBINED, 0, 0, 0, 0, COMBINED),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_PASS | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_30 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_LINE | G_RM_AA_ZB_XLU_LINE2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_31 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_HILITERGBA, G_CC_HILITERGBA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_OPA_SURF | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_32 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_HILITERGBA, G_CC_HILITERGBA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_OPA_SURF | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR |
                             G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_33 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, SHADE, 1, 0, TEXEL0, SHADE, ENVIRONMENT, COMBINED, TEXEL0, COMBINED,
                           ENVIRONMENT, COMBINED, TEXEL0, COMBINED),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_PASS | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR |
                             G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_34 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGB, G_CC_DECALRGB),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_OPA_SURF | G_RM_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_35 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_DECAL | G_RM_AA_ZB_XLU_DECAL2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_36 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_FILL | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_37 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineLERP(PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE, 0, 0, 0, COMBINED, 0, 0, 0, COMBINED),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_38 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_ZB_XLU_SURF | G_RM_AA_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_39 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_40 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL1, TEXEL0, PRIMITIVE_ALPHA, TEXEL0, TEXEL1, TEXEL0, PRIMITIVE, TEXEL0, 0, 0, 0,
                           COMBINED, 0, 0, 0, COMBINED),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_OPA_SURF | G_RM_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_FRONT | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_41 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_ZB_XLU_SURF | G_RM_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_42 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_43 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_RGBA16 | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_44 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_ZB_OVL_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_45 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0, 0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_46 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0, 0, 0, 0, PRIMITIVE, 0, 0, 0, TEXEL0),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_47 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_ZB_XLU_SURF | G_RM_ZB_XLU_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_48 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_XLU_SURF | G_RM_AA_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_49 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineLERP(PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE, PRIMITIVE, 0, SHADE, 0, 0, 0, 0, PRIMITIVE),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_AA_XLU_SURF | G_RM_AA_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_50 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_OPA_SURF | G_RM_OPA_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_51 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_52 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_53 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL1, TEXEL0, ENV_ALPHA, TEXEL0, TEXEL1, TEXEL0, ENVIRONMENT, TEXEL0, PRIMITIVE,
                           ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_OPA_SURF | G_RM_OPA_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_54 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL1, TEXEL0, ENV_ALPHA, TEXEL0, TEXEL1, TEXEL0, ENVIRONMENT, TEXEL0, PRIMITIVE,
                           ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_PASS | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_55 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL1, TEXEL0, ENV_ALPHA, TEXEL0, TEXEL1, TEXEL0, ENVIRONMENT, TEXEL0, PRIMITIVE,
                           ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_PASS | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_CULL_BACK),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_56 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_XLU_SURF | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_57 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_PRIMITIVE, G_CC_PRIMITIVE),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
        gsSPLoadGeometryMode(G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_58 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineLERP(PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE, 0, PRIMITIVE, 0, SHADE,
                           0),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_59 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(TEXEL0, 0, PRIMITIVE, SHADE, 1, 0, TEXEL0, SHADE, ENVIRONMENT, COMBINED, TEXEL0, COMBINED,
                           ENVIRONMENT, COMBINED, TEXEL0, COMBINED),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_PASS | G_RM_AA_OPA_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR |
                             G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_60 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, ENVIRONMENT,
                           TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_ZB_CLD_SURF | G_RM_ZB_CLD_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_61 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineLERP(PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0, PRIMITIVE, ENVIRONMENT,
                           TEXEL0, ENVIRONMENT, PRIMITIVE, 0, TEXEL0, 0),
        gsDPSetOtherMode(G_AD_NOISE | G_CD_NOISE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_ZB_CLD_SURF | G_RM_ZB_CLD_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_62 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIDECALA, G_CC_MODULATEIA_PRIM2),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_NONE | G_ZS_PIXEL | G_RM_FOG_SHADE_A | G_RM_AA_ZB_OPA_SURF2),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_63 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_UPD | IM_RD | CVG_DST_SAVE | ZMODE_OPA | FORCE_BL |
                             GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) |
                             GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_64 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_PASS2),
        gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_NONE | G_CYC_2CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_PASS | G_RM_XLU_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_65 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_CLD_SURF | G_RM_CLD_SURF2),
        gsSPLoadGeometryMode(G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_66 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_CMP | Z_UPD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             ALPHA_CVG_SEL | G_RM_PASS | GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_67 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_CMP | Z_UPD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             ALPHA_CVG_SEL | G_RM_PASS | GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_68 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_CMP | Z_UPD | IM_RD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             ALPHA_CVG_SEL | GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) |
                             GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_69 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_CMP | Z_UPD | IM_RD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             ALPHA_CVG_SEL | GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) |
                             GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
    {
        /* SETUPDL_70 */
        gsDPPipeSync(),
        gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
        gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
        gsDPSetOtherMode(G_AD_NOTPATTERN | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                             G_TD_CLAMP | G_TP_PERSP | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                         G_AC_THRESHOLD | G_ZS_PIXEL | Z_CMP | Z_UPD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA |
                             ALPHA_CVG_SEL | G_RM_PASS | GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)),
        gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_FOG | G_SHADING_SMOOTH),
        gsSPEndDisplayList(),
    },
};

static Gfx RSP_RDP_clear_data[] = {
    gsDPPipeSync(),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_PERSP | G_CYC_FILL | G_PM_NPRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
    gsDPSetBlendColor(0, 0, 0, 8),
    gsSPClipRatio(FRUSTRATIO_2),
    gsSPEndDisplayList(),
};

// unused?
Gfx Zbuffer_clear_data[] = {
    gsDPPipeSync(),
    gsDPSetFillColor((GPACK_RGBA5551(255, 255, 240, 0) << 16) | GPACK_RGBA5551(255, 255, 240, 0)),
    gsDPFillRectangle(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1),
    gsDPSetDepthSource(G_ZS_PIXEL),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

// unused?
Gfx FrameBuffer_clear_data[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_FILL),
    gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP2),
    gsDPSetFillColor((GPACK_RGBA5551(0, 0, 0, 1) << 16) | GPACK_RGBA5551(0, 0, 0, 1)),
    gsDPFillRectangle(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

Gfx dl_noop[] = {
    gsSPEndDisplayList(),
};

Gfx* gfx_set_fog_nosync(Gfx* gfx, s32 r, s32 g, s32 b, s32 a, s32 near, s32 far) {
    if (far == near) {
        far++;
    }

    ASSERT(near != far, "n != f", "../z_rcp.c", 1155);

    gDPSetFogColor(gfx++, r, g, b, a);

    if (near >= 1000) {
        gSPFogFactor(gfx++, 0, 0);
    } else if (near >= 997) {
        gSPFogFactor(gfx++, 0x7FFF, 0x8100);
    } else if (near < 0) {
        gSPFogFactor(gfx++, 0, 255);
    } else {
        gSPFogPosition(gfx++, near, far);
    }

    return gfx;
}

Gfx* gfx_set_fog_sync(Gfx* gfx, s32 r, s32 g, s32 b, s32 a, s32 near, s32 far) {
    if (far == near) {
        far++;
    }
    ASSERT(near != far, "n != f", "../z_rcp.c", 1187);

    gDPPipeSync(gfx++);
    gDPSetFogColor(gfx++, r, g, b, a);

    if (near >= 1000) {
        gSPFogFactor(gfx++, 0, 0);
    } else if (near >= 997) {
        gSPFogFactor(gfx++, 0x7FFF, 0x8100);
    } else if (near < 0) {
        gSPFogFactor(gfx++, 0, 255);
    } else {
        gSPFogPosition(gfx++, near, far);
    }

    return gfx;
}

Gfx* set_fog(Gfx* gfx, s32 r, s32 g, s32 b, s32 a, s32 near, s32 far) {
    return gfx_set_fog_nosync(gfx, r, g, b, a, near, far);
}

Gfx* gfx_rcp_mode_set(Gfx* gfx, u32 i) {
    s32 dListIndex = 6 * i;

    gSPDisplayList(gfx++, &((Gfx*)z_gsCPModeSet_Data)[dListIndex]);
    // Equivalent to gSPDisplayList(gfx++, z_gsCPModeSet_Data[i])
    return gfx;
}

Gfx* rcp_mode_set(Gfx* gfx, u32 i) {
    return gfx_rcp_mode_set(gfx, i);
}

void gp_overlay_rcp_mode_set(Gfx** gfxP, u32 i) {
    *gfxP = rcp_mode_set(*gfxP, i);
}

Gfx* fill_rectangle_prim_free(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_57]);
    return gfx;
}

Gfx* gfx_fill_rectangle_prim(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_57]);
    return gfx;
}

Gfx* gfx_texture_cullback_xlu(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_52]);
    gDPSetColorDither(gfx++, G_CD_DISABLE);
    return gfx;
}

void polygon_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1293);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_58]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1297);
}

void fill_rectangle_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1309);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_57]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1313);
}

void texture_cullback(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1325);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_50]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1329);
}

void xlu_texture_cullback(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1341);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_51]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1345);
}

void texture_cullback_xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1357);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_52]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1361);
}

void morf_texture_cullback(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1373);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_53]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1377);
}

void xlu_morf_texture_cullback(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1389);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_54]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1393);
}

void morf_texture_cullback_xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1405);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_55]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1409);
}

void texture_z_light_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1421);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_26]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1425);
}

void texture_z_light_prim_xlu_disp(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1439);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_25]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1443);
}

void _texture_z_light_fog_prim2(PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    _texture_z_light_fog_prim(gfxCtx);

    if (play->roomCtx.curRoom.type == ROOM_TYPE_3) {
        OPEN_DISPS(gfxCtx, "../z_rcp.c", 1460);

        gDPSetColorDither(POLY_OPA_DISP++, G_CD_DISABLE);

        CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1462);
    }
}

void _texture_z_light_fog_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1475);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_25]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1479);
}

void _texture_z_light_fog_prim_xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1491);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_25]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1495);
}

void texture_z_hilight(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1507);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_31]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1511);
}

void texture_z_reflect(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1523);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_32]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1527);
}

void texture_z_hilight_reflect(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1539);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_33]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1543);
}

Gfx* gfx_xlu_rectangle_2c(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_64]);
    return gfx;
}

Gfx* gfx_rectangle(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_34]);
    return gfx;
}

void rectangle(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1569);

    POLY_OPA_DISP = gfx_rectangle(POLY_OPA_DISP);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1573);
}

void texture_decal(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1585);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_35]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1589);
}

void _texture_decal_shadow(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1601);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_44]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1605);
}

void fill_rectangle(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1617);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_36]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1621);
}

Gfx* gfx_rect_moji(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_28]);
    return gfx;
}

void rect_moji(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1640);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_28]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1644);
}

void rect_moji2(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1651);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_43]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1655);
}

void message_moji(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1670);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_45]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1674);
}

void message_moji_polygon(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1681);

    gSPDisplayList(OVERLAY_DISP++, z_gsCPModeSet_Data[SETUPDL_46]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1685);
}

void vertex_color_xlu_polygon(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1700);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_38]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1704);
}

void polygon_z_light_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1722);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_4]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1726);
}

void _polygon_z_light_fog_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1758);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_37]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1762);
}

void polygon_z_light(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1775);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_2]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1779);
}

Gfx* gfx_rectangle_a_prim(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_39]);
    return gfx;
}

void rectangle_a_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1799);

    POLY_OPA_DISP = gfx_rectangle_a_prim(POLY_OPA_DISP);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1801);
}

void overlay_rectangle_a_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1809);

    OVERLAY_DISP = gfx_rectangle_a_prim(OVERLAY_DISP);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1811);
}

void gp_overlay_rectangle_a_prim(Gfx** gfxP) {
    Gfx* gfx = *gfxP;

    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_39]);
    *gfxP = gfx;
}

void vr_box_polygon(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1837);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_40]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1841);
}

void softsprite(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1853);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_41]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1857);
}

void softsprite_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1869);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_47]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1873);
}

Gfx* gfx_softsprite_z(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_66]);
    return gfx;
}

Gfx* gfx_softsprite_z_prim(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_67]);
    return gfx;
}

Gfx* gfx_softsprite_z_xlu(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_68]);
    gDPSetColorDither(gfx++, G_CD_DISABLE);
    return gfx;
}

Gfx* gfx_softsprite_z_prim_xlu(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_69]);
    gDPSetColorDither(gfx++, G_CD_DISABLE);
    return gfx;
}

#if DEBUG_FEATURES
#define HREG_21 HREG(21)
#define HREG_22 HREG(22)
#else
#define HREG_21 0
#define HREG_22 0
#endif

Gfx* gfx_softsprite_prim_xlu(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_65]);
    gDPSetColorDither(gfx++, G_CD_DISABLE);

    // clang-format off
    switch (HREG_21) {
        case 1: gDPSetAlphaDither(gfx++, G_AD_DISABLE); break;
        case 2: gDPSetAlphaDither(gfx++, G_AD_PATTERN); break;
        case 3: gDPSetAlphaDither(gfx++, G_AD_NOTPATTERN); break;
        case 4: gDPSetAlphaDither(gfx++, G_AD_NOISE); break;
        default: break;
    }

    switch (HREG_22) {
        case 1: gDPSetColorDither(gfx++, G_CD_DISABLE); break;
        case 2: gDPSetColorDither(gfx++, G_CD_MAGICSQ); break;
        case 3: gDPSetColorDither(gfx++, G_CD_BAYER); break;
        case 4: gDPSetColorDither(gfx++, G_CD_NOISE); break;
        default: break;
    }
    // clang-format on

    return gfx;
}

Gfx* gfx_softsprite_z_fog(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_70]);
    return gfx;
}

Gfx* gfx_softsprite_z_prim_cld(Gfx* gfx) {
    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_20]);
    gDPSetColorDither(gfx++, G_CD_DISABLE);
    return gfx;
}

void kaleido_scope_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1953);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_42]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1957);
}

void overlay_kaleido_scope_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1964);

    gSPDisplayList(OVERLAY_DISP++, z_gsCPModeSet_Data[SETUPDL_42]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1968);
}

void fukidasi_polygon(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 1992);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_48]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 1996);
}

void polygon_light_prim(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 2008);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_49]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 2012);
}

void texture_z(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 2024);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_27]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 2028);
}

void texture_z_cld_poly_xlu(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 2040);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_60]);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 2043);
}

void texture_z_cld_poly_xlu_nd(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 2056);

    gSPDisplayList(POLY_XLU_DISP++, z_gsCPModeSet_Data[SETUPDL_61]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 2058);
}

void rgba32texture(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 2086);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_56]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 2090);
}

void gp_overlay_rgba32texture(Gfx** gfxP) {
    Gfx* gfx = *gfxP;

    gSPDisplayList(gfx++, z_gsCPModeSet_Data[SETUPDL_56]);

    *gfxP = gfx;
}

void texture_hilight_reflect(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 2112);

    gSPDisplayList(POLY_OPA_DISP++, z_gsCPModeSet_Data[SETUPDL_59]);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 2116);
}

Gfx* gfx_tex_scroll2(Gfx** gfxP, u32 x, u32 y, s32 width, s32 height) {
    Gfx* displayList = gfxalloc(gfxP, 3 * sizeof(Gfx));

    gDPTileSync(displayList);
    gDPSetTileSize(displayList + 1, G_TX_RENDERTILE, x, y, x + ((width - 1) << 2), y + ((height - 1) << 2));
    gSPEndDisplayList(displayList + 2);

    return displayList;
}

Gfx* gfx_tex_scroll(Gfx** gfxP, u32 x, u32 y) {
    return gfx_tex_scroll2(gfxP, x, y, 0, 0);
}

Gfx* tex_scroll(GraphicsContext* gfxCtx, u32 x, u32 y) {
    return tex_scroll2(gfxCtx, x, y, 0, 0);
}

Gfx* tex_scroll2(GraphicsContext* gfxCtx, u32 x, u32 y, s32 width, s32 height) {
    Gfx* displayList = GRAPH_ALLOC(gfxCtx, 3 * sizeof(Gfx));

    x %= 512 << 2;
    y %= 512 << 2;

    gDPTileSync(displayList);
    gDPSetTileSize(displayList + 1, G_TX_RENDERTILE, x, y, x + ((width - 1) << 2), y + ((height - 1) << 2));
    gSPEndDisplayList(displayList + 2);

    return displayList;
}

Gfx* two_tex_scroll(GraphicsContext* gfxCtx, s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2, u32 x2,
                      u32 y2, s32 width2, s32 height2) {
    Gfx* displayList = GRAPH_ALLOC(gfxCtx, 5 * sizeof(Gfx));

    x1 %= 512 << 2;
    y1 %= 512 << 2;
    x2 %= 512 << 2;
    y2 %= 512 << 2;

    gDPTileSync(displayList);
    gDPSetTileSize(displayList + 1, tile1, x1, y1, x1 + ((width1 - 1) << 2), y1 + ((height1 - 1) << 2));
    gDPTileSync(displayList + 2);
    gDPSetTileSize(displayList + 3, tile2, x2, y2, x2 + ((width2 - 1) << 2), y2 + ((height2 - 1) << 2));
    gSPEndDisplayList(displayList + 4);

    return displayList;
}

Gfx* two_tex_scroll_env(GraphicsContext* gfxCtx, s32 tile1, u32 x1, u32 y1, s32 width1, s32 height1, s32 tile2,
                              u32 x2, u32 y2, s32 width2, s32 height2, s32 r, s32 g, s32 b, s32 a) {
    Gfx* displayList = GRAPH_ALLOC(gfxCtx, 6 * sizeof(Gfx));

    x1 %= 512 << 2;
    y1 %= 512 << 2;
    x2 %= 512 << 2;
    y2 %= 512 << 2;

    gDPTileSync(displayList);
    gDPSetTileSize(displayList + 1, tile1, x1, y1, x1 + ((width1 - 1) << 2), y1 + ((height1 - 1) << 2));
    gDPTileSync(displayList + 2);
    gDPSetTileSize(displayList + 3, tile2, x2, y2, x2 + ((width2 - 1) << 2), y2 + ((height2 - 1) << 2));
    gDPSetEnvColor(displayList + 4, r, g, b, a);
    gSPEndDisplayList(displayList + 5);

    return displayList;
}

Gfx* anime_envcolor(GraphicsContext* gfxCtx, s32 r, s32 g, s32 b, s32 a) {
    Gfx* displayList = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));

    gDPSetEnvColor(displayList, r, g, b, a);
    gSPEndDisplayList(displayList + 1);

    return displayList;
}

/**
 * Sets up the frame for drawing.
 * Initializes the scissor region to full screen.
 * Set up the framebuffer and z-buffer.
 * The whole screen is filled with the color supplied as arguments.
 * Letterbox is also applied here, and will share the color of the screen base.
 */
void DisplayList_initialize(GraphicsContext* gfxCtx, u8 r, u8 g, u8 b) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 2386);

    // Set up the RDP render state for rectangles in FILL mode
    gSPDisplayList(POLY_OPA_DISP++, RSP_RDP_clear_data);
    gSPDisplayList(POLY_XLU_DISP++, RSP_RDP_clear_data);
    gSPDisplayList(OVERLAY_DISP++, RSP_RDP_clear_data);

    // Set the scissor region to the full screen
    gDPSetScissor(POLY_OPA_DISP++, G_SC_NON_INTERLACE, 0, 0, ScreenWidth, ScreenHeight);
    gDPSetScissor(POLY_XLU_DISP++, G_SC_NON_INTERLACE, 0, 0, ScreenWidth, ScreenHeight);
    gDPSetScissor(OVERLAY_DISP++, G_SC_NON_INTERLACE, 0, 0, ScreenWidth, ScreenHeight);

    // Set up the framebuffer, primitives will be drawn here
    gDPSetColorImage(POLY_OPA_DISP++, G_IM_FMT_RGBA, G_IM_SIZ_16b, ScreenWidth, gfxCtx->curFrameBuffer);
    gDPSetColorImage(POLY_OPA_DISP++, G_IM_FMT_RGBA, G_IM_SIZ_16b, ScreenWidth, gfxCtx->curFrameBuffer);
    gDPSetColorImage(POLY_XLU_DISP++, G_IM_FMT_RGBA, G_IM_SIZ_16b, ScreenWidth, gfxCtx->curFrameBuffer);
    gDPSetColorImage(OVERLAY_DISP++, G_IM_FMT_RGBA, G_IM_SIZ_16b, ScreenWidth, gfxCtx->curFrameBuffer);

    // Set up the z-buffer
    gDPSetDepthImage(POLY_OPA_DISP++, sys_zb);
    gDPSetDepthImage(POLY_XLU_DISP++, sys_zb);
    gDPSetDepthImage(OVERLAY_DISP++, sys_zb);

    if ((R_PAUSE_BG_PRERENDER_STATE <= PAUSE_BG_PRERENDER_SETUP) && (fbdemo_mode <= TRANS_TILE_SETUP)) {
        s32 letterboxSize = shrink_window_getnowval();

#if DEBUG_FEATURES
        if (R_HREG_MODE == HREG_MODE_SETUP_FRAME) {
            if (R_SETUP_FRAME_INIT != HREG_MODE_SETUP_FRAME) {
                R_SETUP_FRAME_GET = (SETUP_FRAME_LETTERBOX_SIZE_FLAG | SETUP_FRAME_BASE_COLOR_FLAG);
                R_SETUP_FRAME_SET = (SETUP_FRAME_LETTERBOX_SIZE_FLAG | SETUP_FRAME_BASE_COLOR_FLAG);
                R_SETUP_FRAME_LETTERBOX_SIZE = 0;
                R_SETUP_FRAME_BASE_COLOR_R = 0;
                R_SETUP_FRAME_BASE_COLOR_G = 0;
                R_SETUP_FRAME_BASE_COLOR_B = 0;

                // these regs are not used in this mode
                HREG(87) = 0;
                HREG(88) = 0;
                HREG(89) = 0;
                HREG(90) = 0;
                HREG(91) = 0;
                HREG(92) = 0;
                HREG(93) = 0;
                HREG(94) = 0;

                R_SETUP_FRAME_INIT = HREG_MODE_SETUP_FRAME;
            }

            if (R_SETUP_FRAME_GET & SETUP_FRAME_LETTERBOX_SIZE_FLAG) {
                R_SETUP_FRAME_LETTERBOX_SIZE = letterboxSize;
            }

            if (R_SETUP_FRAME_GET & SETUP_FRAME_BASE_COLOR_FLAG) {
                R_SETUP_FRAME_BASE_COLOR_R = r;
                R_SETUP_FRAME_BASE_COLOR_G = g;
                R_SETUP_FRAME_BASE_COLOR_B = b;
            }

            if (R_SETUP_FRAME_SET & SETUP_FRAME_LETTERBOX_SIZE_FLAG) {
                letterboxSize = R_SETUP_FRAME_LETTERBOX_SIZE;
            }

            if (R_SETUP_FRAME_SET & SETUP_FRAME_BASE_COLOR_FLAG) {
                r = R_SETUP_FRAME_BASE_COLOR_R;
                g = R_SETUP_FRAME_BASE_COLOR_G;
                b = R_SETUP_FRAME_BASE_COLOR_B;
            }
        }
#endif

        // Set the whole z buffer to maximum depth
        // Don't bother with pixels that are being covered by the letterbox
        gDPSetColorImage(POLY_OPA_DISP++, G_IM_FMT_RGBA, G_IM_SIZ_16b, ScreenWidth, sys_zb);
        gDPSetCycleType(POLY_OPA_DISP++, G_CYC_FILL);
        gDPSetRenderMode(POLY_OPA_DISP++, G_RM_NOOP, G_RM_NOOP2);
        gDPSetFillColor(POLY_OPA_DISP++, (GPACK_ZDZ(G_MAXFBZ, 0) << 16) | GPACK_ZDZ(G_MAXFBZ, 0));
        gDPFillRectangle(POLY_OPA_DISP++, 0, letterboxSize, ScreenWidth - 1, ScreenHeight - letterboxSize - 1);
        gDPPipeSync(POLY_OPA_DISP++);

        // Fill the whole screen with the base color
        // Don't bother with pixels that are being covered by the letterbox
        gDPSetColorImage(POLY_OPA_DISP++, G_IM_FMT_RGBA, G_IM_SIZ_16b, ScreenWidth, gfxCtx->curFrameBuffer);
        gDPSetCycleType(POLY_OPA_DISP++, G_CYC_FILL);
        gDPSetRenderMode(POLY_OPA_DISP++, G_RM_NOOP, G_RM_NOOP2);
        gDPSetFillColor(POLY_OPA_DISP++, (GPACK_RGBA5551(r, g, b, 1) << 16) | GPACK_RGBA5551(r, g, b, 1));
        gDPFillRectangle(POLY_OPA_DISP++, 0, letterboxSize, ScreenWidth - 1, ScreenHeight - letterboxSize - 1);
        gDPPipeSync(POLY_OPA_DISP++);

        // Draw the letterbox if applicable (uses the same color as the screen base)
        if (letterboxSize > 0) {
            gDPPipeSync(OVERLAY_DISP++);
            gDPSetCycleType(OVERLAY_DISP++, G_CYC_FILL);
            gDPSetRenderMode(OVERLAY_DISP++, G_RM_NOOP, G_RM_NOOP2);
            gDPSetFillColor(OVERLAY_DISP++, (GPACK_RGBA5551(r, g, b, 1) << 16) | GPACK_RGBA5551(r, g, b, 1));
            gDPFillRectangle(OVERLAY_DISP++, 0, 0, ScreenWidth - 1, letterboxSize - 1);
            gDPFillRectangle(OVERLAY_DISP++, 0, ScreenHeight - letterboxSize, ScreenWidth - 1, ScreenHeight - 1);
            gDPPipeSync(OVERLAY_DISP++);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 2497);
}

void static_DisplayList_initialize(GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_rcp.c", 2503);

    gSPDisplayList(POLY_OPA_DISP++, RSP_RDP_clear_data);
    gDPSetScissor(POLY_OPA_DISP++, G_SC_NON_INTERLACE, 0, 0, ScreenWidth, ScreenHeight);
    gDPSetDepthImage(POLY_OPA_DISP++, sys_zb);
    gDPSetColorImage(POLY_OPA_DISP++, G_IM_FMT_RGBA, G_IM_SIZ_16b, ScreenWidth, gfxCtx->curFrameBuffer);

    CLOSE_DISPS(gfxCtx, "../z_rcp.c", 2513);
}
