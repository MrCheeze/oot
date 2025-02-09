#include "global.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

void EffectBlure_edge_add(EffectBlure* this, Vec3f* p1, Vec3f* p2) {
    EffectBlureElement* elem;
    s32 numElements;

    // Necessary to match
    if (this) {}
    if (this) {}

    if (this != NULL) {
        numElements = this->numElements;
        if (numElements >= 16) {
            PRINTF(T("ブラ─頂点追加処理:テーブルオーバー %d\n", "Blur - vertex addition processing: Table over %d\n"),
                   numElements);
            return;
        }

        elem = &this->elements[numElements];
        elem->state = 1;

        if (!(this->flags & 2)) {
            elem->p1.x = p1->x;
            elem->p1.y = p1->y;
            elem->p1.z = p1->z;
            elem->p2.x = p2->x;
            elem->p2.y = p2->y;
            elem->p2.z = p2->z;
        } else {
            Vec3f sp16C;
            Vec3f sp160;
            Vec3f sp154;
            f32 scale;
            MtxF sp110;
            MtxF spD0;
            MtxF sp90;
            MtxF sp50;
            Vec3f sp44;
            Vec3f sp38;

            sp16C.x = ((f32)(elem - 1)->p2.x + (f32)(elem - 1)->p1.x) * 0.5f;
            sp16C.y = ((f32)(elem - 1)->p2.y + (f32)(elem - 1)->p1.y) * 0.5f;
            sp16C.z = ((f32)(elem - 1)->p2.z + (f32)(elem - 1)->p1.z) * 0.5f;
            sp160.x = (p1->x + p2->x) * 0.5f;
            sp160.y = (p1->y + p2->y) * 0.5f;
            sp160.z = (p1->z + p2->z) * 0.5f;

            xyz_t_sub(&sp160, &sp16C, &sp154);
            scale = Math3DVecLength(&sp154);
            if (!(fabsf(scale) < 0.008f)) {
                scale = 1.0f / scale;
                xyz_t_mult_v(&sp154, scale);

                Skin_Matrix_SetTranslate(&sp110, sp160.x, sp160.y, sp160.z);
                Skin_Matrix_SetFreeVecRotMatrix(&spD0, this->addAngle, sp154.x, sp154.y, sp154.z);
                Skin_Matrix_MulMatrix(&sp110, &spD0, &sp90);
                Skin_Matrix_SetTranslate(&sp110, -sp160.x, -sp160.y, -sp160.z);
                Skin_Matrix_MulMatrix(&sp90, &sp110, &sp50);
                Skin_Matrix_MulVector(&sp50, p1, &sp38);
                Skin_Matrix_MulVector(&sp50, p2, &sp44);

                elem->p1.x = sp38.x;
                elem->p1.y = sp38.y;
                elem->p1.z = sp38.z;
                elem->p2.x = sp44.x;
                elem->p2.y = sp44.y;
                elem->p2.z = sp44.z;
            }
        }

        elem->timer = 1;
        this->numElements++;
    }
}

void EffectBlure_space_add(EffectBlure* this) {
    EffectBlureElement* elem;
    s32 numElements;

    if (this != NULL) {
        numElements = this->numElements;
        if (numElements >= 16) {
            PRINTF(T("ブラ─空白追加処理:テーブルオーバー %d\n", "Blur - space addition processing: Table over %d\n"),
                   numElements);
            return;
        }

        elem = &this->elements[numElements];
        elem->state = 0;
        elem->timer = 1;

        this->numElements++;
    }
}

void EffectBlureInfo_commonClear(EffectBlure* this) {
    EffectBlureElement* elem;
    s32 i;

    this->numElements = 0;

    for (i = 0; i < 16; i++) {
        elem = &this->elements[i];

        elem->state = 2;
        elem->p1.x = 0;
        elem->p1.y = 0;
        elem->p1.z = 0;
        elem->p2.x = 0;
        elem->p2.y = 0;
        elem->p2.z = 0;
        elem->timer = 0;
        elem->flags = 0;
    }
}

void EffectBlureInfo_ct(void* thisx, void* initParamsx) {
    EffectBlure* this = (EffectBlure*)thisx;
    EffectBlureInit1* initParams = (EffectBlureInit1*)initParamsx;

    if ((this != NULL) && (initParams != NULL)) {
        s32 i;

        EffectBlureInfo_commonClear(this);

        for (i = 0; i < 4; i++) {
            this->p1StartColor[i] = initParams->p1StartColor[i];
            this->p2StartColor[i] = initParams->p2StartColor[i];
            this->p1EndColor[i] = initParams->p1EndColor[i];
            this->p2EndColor[i] = initParams->p2EndColor[i];
        }

        this->elemDuration = initParams->elemDuration;
        this->unkFlag = initParams->unkFlag;
        this->calcMode = initParams->calcMode;
        this->flags = 0;
        this->addAngleChange = 0;
        this->addAngle = 0;
        this->drawMode = 0;
        this->altPrimColor.r = 0;
        this->altPrimColor.g = 0;
        this->altPrimColor.b = 0;
        this->altPrimColor.a = 0;
        this->altEnvColor.r = 0;
        this->altEnvColor.g = 0;
        this->altEnvColor.b = 0;
        this->altEnvColor.a = 0;
        this->mode4Param = 1.0f;
    }
}

void EffectBlureInfo_v1_ct(void* thisx, void* initParamsx) {
    EffectBlure* this = (EffectBlure*)thisx;
    EffectBlureInit2* initParams = (EffectBlureInit2*)initParamsx;

    if ((this != NULL) && (initParams != NULL)) {
        s32 i;

        EffectBlureInfo_commonClear(this);

        for (i = 0; i < 4; i++) {
            this->p1StartColor[i] = initParams->p1StartColor[i];
            this->p2StartColor[i] = initParams->p2StartColor[i];
            this->p1EndColor[i] = initParams->p1EndColor[i];
            this->p2EndColor[i] = initParams->p2EndColor[i];
        }

        this->elemDuration = initParams->elemDuration;
        this->unkFlag = initParams->unkFlag;
        this->calcMode = initParams->calcMode;
        this->flags = initParams->flags;
        this->drawMode = initParams->drawMode;
        this->addAngleChange = initParams->addAngleChange;
        this->addAngle = 0;
        this->mode4Param = initParams->mode4Param;
        this->altPrimColor = initParams->altPrimColor;
        this->altEnvColor = initParams->altEnvColor;
    }
}

void EffectBlureInfo_dt(void* thisx) {
}

s32 EffectBlureInfo_proc(void* thisx) {
    EffectBlure* this = (EffectBlure*)thisx;
    s32 i;

    if (this == NULL) {
        return 0;
    }

    if (this->numElements == 0) {
        return 0;
    }

    while (true) {
        if (this->elements[0].state == 0) {
            for (i = 0; i < 15; i++) {
                this->elements[i] = this->elements[i + 1];
            }

            this->elements[i].state = 2;
            this->elements[i].p1.x = 0;
            this->elements[i].p1.y = 0;
            this->elements[i].p1.z = 0;
            this->elements[i].p2.x = 0;
            this->elements[i].p2.y = 0;
            this->elements[i].p2.z = 0;
            this->elements[i].flags = 0;
            this->elements[i].timer = 0;

            this->numElements--;
            if (this->numElements <= 0) {
                this->numElements = 0;
                return 0;
            }
        } else {
            break;
        }
    }

    if (this->elements[0].state == 2) {
        return 0;
    }

    for (i = 0; i < this->numElements; i++) {
        this->elements[i].timer++;
    }

    if (this->elemDuration < this->elements[0].timer) {
        for (i = 0; i < 15; i++) {
            this->elements[i] = this->elements[i + 1];
        }

        this->elements[i].state = 2;
        this->elements[i].p1.x = 0;
        this->elements[i].p1.y = 0;
        this->elements[i].p1.z = 0;
        this->elements[i].p2.x = 0;
        this->elements[i].p2.y = 0;
        this->elements[i].p2.z = 0;
        this->elements[i].flags = 0;
        this->elements[i].timer = 0;

        this->numElements--;
        if (this->numElements <= 0) {
            this->numElements = 0;
            return 0;
        }
        return 0;
    }

    this->addAngle += this->addAngleChange;
    return 0;
}

void HermiteInterpolateCheck(EffectBlureElement* elem) {
    Vec3f sp64;
    Vec3f sp58;
    Vec3f sp4C;
    Vec3f sp40;
    EffectBlureElement* prev = elem - 1;
    EffectBlureElement* next = elem + 1;
    f32 sp34;
    f32 sp30;
    f32 sp2C;

    if (((elem - 1)->state == 0) || ((elem + 1)->state == 0)) {
        elem->flags &= ~3;
        elem->flags |= 2;
    } else {
        xyz_t_sub_ss(&sp64, &elem->p1, &prev->p1);
        xyz_t_sub_ss(&sp58, &elem->p2, &prev->p2);
        xyz_t_sub_ss(&sp4C, &next->p1, &elem->p1);
        xyz_t_sub_ss(&sp40, &next->p2, &elem->p2);

        if (M3D_getCos2(&sp64, &sp4C, &sp34) || M3D_getCos2(&sp58, &sp40, &sp30) ||
            M3D_getCos2(&sp4C, &sp40, &sp2C)) {
            elem->flags &= ~3;
            elem->flags |= 0;
        } else if ((sp34 <= -0.5f) || (sp30 <= -0.5f) || (sp2C <= 0.7071f)) { // cos(45 degrees)
            elem->flags &= ~3;
            elem->flags |= 0;
        } else {
            elem->flags &= ~3;
            elem->flags |= 1;
        }
    }
}

void EffectBlureInfo2_disp_calc(EffectBlure* this, s32 index, f32 ratio, Vec3s* vec1, Vec3s* vec2,
                                   Color_RGBA8* color1, Color_RGBA8* color2) {
    Vec3s sp30;
    f32 mode4Param;
    EffectBlureElement* elem = &this->elements[index];

    switch (this->calcMode) {
        case 1:
            vec1->x = Effect_SS_Uty_short_interpolation_t01(elem->p1.x, elem->p2.x, ratio);
            vec1->y = Effect_SS_Uty_short_interpolation_t01(elem->p1.y, elem->p2.y, ratio);
            vec1->z = Effect_SS_Uty_short_interpolation_t01(elem->p1.z, elem->p2.z, ratio);
            vec2->x = elem->p2.x;
            vec2->y = elem->p2.y;
            vec2->z = elem->p2.z;
            break;

        case 2:
            vec1->x = elem->p1.x;
            vec1->y = elem->p1.y;
            vec1->z = elem->p1.z;
            vec2->x = Effect_SS_Uty_short_interpolation_t01(elem->p2.x, elem->p1.x, ratio);
            vec2->y = Effect_SS_Uty_short_interpolation_t01(elem->p2.y, elem->p1.y, ratio);
            vec2->z = Effect_SS_Uty_short_interpolation_t01(elem->p2.z, elem->p1.z, ratio);
            break;

        case 3:
            ratio *= 0.5f;
            vec1->x = Effect_SS_Uty_short_interpolation_t01(elem->p1.x, elem->p2.x, ratio);
            vec1->y = Effect_SS_Uty_short_interpolation_t01(elem->p1.y, elem->p2.y, ratio);
            vec1->z = Effect_SS_Uty_short_interpolation_t01(elem->p1.z, elem->p2.z, ratio);
            vec2->x = Effect_SS_Uty_short_interpolation_t01(elem->p2.x, elem->p1.x, ratio);
            vec2->y = Effect_SS_Uty_short_interpolation_t01(elem->p2.y, elem->p1.y, ratio);
            vec2->z = Effect_SS_Uty_short_interpolation_t01(elem->p2.z, elem->p1.z, ratio);
            ratio *= 2.0f;
            break;

        case 4:
            sp30.x = elem->p1.x - elem->p2.x;
            sp30.y = elem->p1.y - elem->p2.y;
            sp30.z = elem->p1.z - elem->p2.z;
            mode4Param = this->mode4Param - 1.0f;

            vec1->x = (sp30.x * 0.5f * mode4Param * ratio) + elem->p1.x;
            vec1->y = (sp30.y * 0.5f * mode4Param * ratio) + elem->p1.y;
            if (1) {} // Necessary to match
            vec1->z = (sp30.z * 0.5f * mode4Param * ratio) + elem->p1.z;

            vec2->x = -(sp30.x * 0.5f * mode4Param * ratio) + elem->p2.x;
            vec2->y = -(sp30.y * 0.5f * mode4Param * ratio) + elem->p2.y;
            vec2->z = -(sp30.z * 0.5f * mode4Param * ratio) + elem->p2.z;
            break;

        case 0:
        default:
            vec1->x = elem->p1.x;
            vec1->y = elem->p1.y;
            vec1->z = elem->p1.z;
            vec2->x = elem->p2.x;
            vec2->y = elem->p2.y;
            vec2->z = elem->p2.z;
            break;
    }

    sp30 = sp30; // Optimized out but seems necessary to match stack usage

    if (this->flags & 0x10) {
        color1->r = color1->g = color1->b = color1->a = 255;
        color2->r = color2->g = color2->b = color2->a = 255;
    } else {
        color1->r = Effect_SS_Uty_uc_interpolation_t01(this->p1StartColor[0], this->p1EndColor[0], ratio);
        color1->g = Effect_SS_Uty_uc_interpolation_t01(this->p1StartColor[1], this->p1EndColor[1], ratio);
        color1->b = Effect_SS_Uty_uc_interpolation_t01(this->p1StartColor[2], this->p1EndColor[2], ratio);
        color1->a = Effect_SS_Uty_uc_interpolation_t01(this->p1StartColor[3], this->p1EndColor[3], ratio);
        color2->r = Effect_SS_Uty_uc_interpolation_t01(this->p2StartColor[0], this->p2EndColor[0], ratio);
        color2->g = Effect_SS_Uty_uc_interpolation_t01(this->p2StartColor[1], this->p2EndColor[1], ratio);
        color2->b = Effect_SS_Uty_uc_interpolation_t01(this->p2StartColor[2], this->p2EndColor[2], ratio);
        color2->a = Effect_SS_Uty_uc_interpolation_t01(this->p2StartColor[3], this->p2EndColor[3], ratio);
    }
}

void EffectBlureInfo2_hermite_disp_init(EffectBlure* this, GraphicsContext* gfxCtx) {
    OPEN_DISPS(gfxCtx, "../z_eff_blure.c", 809);

    POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_38);

    CLOSE_DISPS(gfxCtx, "../z_eff_blure.c", 813);
}

// original name: "SQ_NoInterpolate_disp"
void SQ_NoInterpolate_disp(EffectBlure* this, EffectBlureElement* elem, s32 index,
                                         GraphicsContext* gfxCtx) {
    static Vtx_t clear_vtx = VTX_T(0, 0, 0, 0, 0, 255, 255, 255, 255);
    Vtx* vtx;
    Vec3s sp8C;
    Vec3s sp84;
    f32 ratio;
    Color_RGBA8 sp7C;
    Color_RGBA8 sp78;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;

    OPEN_DISPS(gfxCtx, "../z_eff_blure.c", 838);

    xyz_t_move_s_xyz(&sp6C, &this->elements[0].p2);

    vtx = GRAPH_ALLOC(gfxCtx, sizeof(Vtx[4]));
    if (vtx == NULL) {
        PRINTF(T("z_eff_blure.c::SQ_NoInterpolate_disp() 頂点確保できず。\n",
                 "z_eff_blure.c::SQ_NoInterpolate_disp() Vertices cannot be secured.\n"));
        goto close_disps;
    }

    vtx[0].v = clear_vtx;
    vtx[1].v = clear_vtx;
    vtx[2].v = clear_vtx;
    vtx[3].v = clear_vtx;

    ratio = (f32)elem->timer / (f32)this->elemDuration;
    EffectBlureInfo2_disp_calc(this, index, ratio, &sp8C, &sp84, &sp7C, &sp78);

    sp60.x = sp84.x;
    sp60.y = sp84.y;
    sp60.z = sp84.z;
    xyz_t_sub(&sp60, &sp6C, &sp54);
    xyz_t_mult_v(&sp54, 10.0f);
    vtx[0].v.ob[0] = sp54.x;
    vtx[0].v.ob[1] = sp54.y;
    vtx[0].v.ob[2] = sp54.z;
    vtx[0].v.cn[0] = sp78.r;
    vtx[0].v.cn[1] = sp78.g;
    vtx[0].v.cn[2] = sp78.b;
    vtx[0].v.cn[3] = sp78.a;

    sp60.x = sp8C.x;
    sp60.y = sp8C.y;
    sp60.z = sp8C.z;
    xyz_t_sub(&sp60, &sp6C, &sp54);
    xyz_t_mult_v(&sp54, 10.0f);
    vtx[1].v.ob[0] = sp54.x;
    vtx[1].v.ob[1] = sp54.y;
    vtx[1].v.ob[2] = sp54.z;
    vtx[1].v.cn[0] = sp7C.r;
    vtx[1].v.cn[1] = sp7C.g;
    vtx[1].v.cn[2] = sp7C.b;
    vtx[1].v.cn[3] = sp7C.a;

    ratio = (f32)(elem + 1)->timer / (f32)this->elemDuration;
    EffectBlureInfo2_disp_calc(this, index + 1, ratio, &sp8C, &sp84, &sp7C, &sp78);

    sp60.x = sp8C.x;
    sp60.y = sp8C.y;
    sp60.z = sp8C.z;
    xyz_t_sub(&sp60, &sp6C, &sp54);
    xyz_t_mult_v(&sp54, 10.0f);
    vtx[2].v.ob[0] = sp54.x;
    vtx[2].v.ob[1] = sp54.y;
    vtx[2].v.ob[2] = sp54.z;
    vtx[2].v.cn[0] = sp7C.r;
    vtx[2].v.cn[1] = sp7C.g;
    vtx[2].v.cn[2] = sp7C.b;
    vtx[2].v.cn[3] = sp7C.a;

    sp60.x = sp84.x;
    sp60.y = sp84.y;
    sp60.z = sp84.z;
    xyz_t_sub(&sp60, &sp6C, &sp54);
    xyz_t_mult_v(&sp54, 10.0f);
    vtx[3].v.ob[0] = sp54.x;
    vtx[3].v.ob[1] = sp54.y;
    vtx[3].v.ob[2] = sp54.z;
    vtx[3].v.cn[0] = sp78.r;
    vtx[3].v.cn[1] = sp78.g;
    vtx[3].v.cn[2] = sp78.b;
    vtx[3].v.cn[3] = sp78.a;

    if (1) {}

    gSPVertex(POLY_XLU_DISP++, vtx, 4, 0);
    gSP2Triangles(POLY_XLU_DISP++, 0, 1, 2, 0, 0, 2, 3, 0);

close_disps:
    CLOSE_DISPS(gfxCtx, "../z_eff_blure.c", 932);
}

void SQ_HermiteInterpolate_disp(EffectBlure* this, EffectBlureElement* elem, s32 index,
                                              GraphicsContext* gfxCtx) {
    static Vtx_t clear_vtx = VTX_T(0, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    Vtx* vtx;
    Vec3s sp1EC;
    Vec3s sp1E4;
    f32 ratio;
    Color_RGBA8 sp1DC;
    Color_RGBA8 sp1D8;
    Vec3f sp1CC;
    Vec3f sp1C0;
    Vec3f sp1B4;
    Vec3f sp1A8;
    Color_RGBA8 sp1A4;
    Color_RGBA8 sp1A0;
    Color_RGBA8 sp19C;
    Color_RGBA8 sp198;
    Vec3f sp18C;
    Vec3f sp180;
    Vec3f sp174;
    Vec3f sp168;
    s32 i;
    Vec3f sp158;
    Vec3f sp14C;
    Color_RGBA8 sp148;
    Color_RGBA8 sp144;
    Vec3f sp138;

    OPEN_DISPS(gfxCtx, "../z_eff_blure.c", 971);

    xyz_t_move_s_xyz(&sp138, &this->elements[0].p2);

    ratio = (f32)elem->timer / (f32)this->elemDuration;
    EffectBlureInfo2_disp_calc(this, index, ratio, &sp1EC, &sp1E4, &sp1A4, &sp1A0);
    xyz_t_move_s_xyz(&sp1CC, &sp1EC);
    xyz_t_move_s_xyz(&sp1C0, &sp1E4);

    ratio = (f32)(elem + 1)->timer / (f32)this->elemDuration;
    EffectBlureInfo2_disp_calc(this, index + 1, ratio, &sp1EC, &sp1E4, &sp19C, &sp198);
    xyz_t_move_s_xyz(&sp18C, &sp1EC);
    xyz_t_move_s_xyz(&sp180, &sp1E4);

    if ((elem->flags & 3) == 2) {
        xyz_t_sub(&sp18C, &sp1CC, &sp1B4);
        xyz_t_sub(&sp180, &sp1C0, &sp1A8);
    } else {
        Vec3f sp118;
        Vec3f sp10C;

        ASSERT(index - 1 >= 0, "index - 1 >= 0", "../z_eff_blure.c", 1005);

        ratio = (f32)(elem - 1)->timer / (f32)this->elemDuration;
        EffectBlureInfo2_disp_calc(this, index - 1, ratio, &sp1EC, &sp1E4, &sp1DC, &sp1D8);
        xyz_t_move_s_xyz(&sp118, &sp1EC);
        xyz_t_move_s_xyz(&sp10C, &sp1E4);
        xyz_t_sub(&sp18C, &sp118, &sp1B4);
        xyz_t_sub(&sp180, &sp10C, &sp1A8);
    }

    xyz_t_mult_v(&sp1B4, 0.5f);
    xyz_t_mult_v(&sp1A8, 0.5f);

    if (((elem + 1)->flags & 3) == 2) {
        xyz_t_sub(&sp18C, &sp1CC, &sp174);
        xyz_t_sub(&sp180, &sp1C0, &sp168);
    } else {
        Vec3f sp100;
        Vec3f spF4;

        ASSERT(index + 2 < this->numElements, "index + 2 < this2->now_edge_num", "../z_eff_blure.c", 1032);

        ratio = (f32)(elem + 2)->timer / (f32)this->elemDuration;
        EffectBlureInfo2_disp_calc(this, index + 2, ratio, &sp1EC, &sp1E4, &sp1DC, &sp1D8);
        xyz_t_move_s_xyz(&sp100, &sp1EC);
        xyz_t_move_s_xyz(&spF4, &sp1E4);
        xyz_t_sub(&sp100, &sp1CC, &sp174);
        xyz_t_sub(&spF4, &sp1C0, &sp168);
    }

    xyz_t_mult_v(&sp174, 0.5f);
    xyz_t_mult_v(&sp168, 0.5f);

    vtx = GRAPH_ALLOC(gfxCtx, sizeof(Vtx[16]));
    if (vtx == NULL) {
        PRINTF(T("z_eff_blure.c::SQ_HermiteInterpolate_disp() 頂点確保できず。\n",
                 "z_eff_blure.c::SQ_HermiteInterpolate_disp() Vertices cannot be secured.\n"));
    } else {
        xyz_t_sub(&sp1CC, &sp138, &sp158);
        xyz_t_mult_v(&sp158, 10.0f);
        xyz_t_sub(&sp1C0, &sp138, &sp14C);
        xyz_t_mult_v(&sp14C, 10.0f);

        rgba_t_move(&sp148, &sp1A4);
        rgba_t_move(&sp144, &sp1A0);

        vtx[0].v = clear_vtx;
        vtx[1].v = clear_vtx;

        vtx[0].v.ob[0] = fround(sp158.x);
        vtx[0].v.ob[1] = fround(sp158.y);
        vtx[0].v.ob[2] = fround(sp158.z);
        vtx[0].v.cn[0] = sp148.r;
        vtx[0].v.cn[1] = sp148.g;
        vtx[0].v.cn[2] = sp148.b;
        vtx[0].v.cn[3] = sp148.a;
        vtx[1].v.ob[0] = fround(sp14C.x);
        vtx[1].v.ob[1] = fround(sp14C.y);
        vtx[1].v.ob[2] = fround(sp14C.z);
        vtx[1].v.cn[0] = sp144.r;
        vtx[1].v.cn[1] = sp144.g;
        vtx[1].v.cn[2] = sp144.b;
        vtx[1].v.cn[3] = sp144.a;

        for (i = 1; i < 8; i++) {
            s32 j1 = 2 * i;
            s32 j2 = 2 * i + 1;
            Vec3f spE0;
            f32 temp_f28 = i / 7.0f;                               // t
            f32 temp_f0 = temp_f28 * temp_f28;                     // t^2
            f32 temp_f2 = temp_f0 * temp_f28;                      // t^3
            f32 temp_f20 = temp_f2 - temp_f0;                      // t^3 - t^2
            f32 temp_f22 = temp_f2 - 2.0f * temp_f0 + temp_f28;    // t^3 - 2t^2 + t
            f32 temp_f24 = 2.0f * temp_f2 - temp_f0 * 3.0f + 1.0f; // 2t^3 - 3t^2 + 1
            f32 temp_f26 = temp_f0 * 3.0f - 2.0f * temp_f2;        // 3t^2 - 2t^3
            s32 pad1;
            s32 pad2;

            // p = (2t^3 - 3t^2 + 1)p0 + (3t^2 - 2t^3)p1 + (t^3 - 2t^2 + t)m0 + (t^3 - t^2)m1
            spE0.x = (temp_f24 * sp1CC.x) + (temp_f26 * sp18C.x) + (temp_f22 * sp1B4.x) + (temp_f20 * sp174.x);
            spE0.y = (temp_f24 * sp1CC.y) + (temp_f26 * sp18C.y) + (temp_f22 * sp1B4.y) + (temp_f20 * sp174.y);
            spE0.z = (temp_f24 * sp1CC.z) + (temp_f26 * sp18C.z) + (temp_f22 * sp1B4.z) + (temp_f20 * sp174.z);
            xyz_t_sub(&spE0, &sp138, &sp158);
            xyz_t_mult_v(&sp158, 10.0f);

            spE0.x = (temp_f24 * sp1C0.x) + (temp_f26 * sp180.x) + (temp_f22 * sp1A8.x) + (temp_f20 * sp168.x);
            spE0.y = (temp_f24 * sp1C0.y) + (temp_f26 * sp180.y) + (temp_f22 * sp1A8.y) + (temp_f20 * sp168.y);
            spE0.z = (temp_f24 * sp1C0.z) + (temp_f26 * sp180.z) + (temp_f22 * sp1A8.z) + (temp_f20 * sp168.z);
            xyz_t_sub(&spE0, &sp138, &sp14C);
            xyz_t_mult_v(&sp14C, 10.0f);

            vtx[j1].v = clear_vtx;
            vtx[j2].v = clear_vtx;

            vtx[j1].v.ob[0] = fround(sp158.x);
            vtx[j1].v.ob[1] = fround(sp158.y);
            vtx[j1].v.ob[2] = fround(sp158.z);
            vtx[j1].v.cn[0] = Effect_SS_Uty_uc_interpolation_t01(sp1A4.r, sp19C.r, temp_f28);
            vtx[j1].v.cn[1] = Effect_SS_Uty_uc_interpolation_t01(sp1A4.g, sp19C.g, temp_f28);
            vtx[j1].v.cn[2] = Effect_SS_Uty_uc_interpolation_t01(sp1A4.b, sp19C.b, temp_f28);
            vtx[j1].v.cn[3] = Effect_SS_Uty_uc_interpolation_t01(sp1A4.a, sp19C.a, temp_f28);

            vtx[j2].v.ob[0] = fround(sp14C.x);
            vtx[j2].v.ob[1] = fround(sp14C.y);
            vtx[j2].v.ob[2] = fround(sp14C.z);
            vtx[j2].v.cn[0] = Effect_SS_Uty_uc_interpolation_t01(sp1A0.r, sp198.r, temp_f28);
            vtx[j2].v.cn[1] = Effect_SS_Uty_uc_interpolation_t01(sp1A0.g, sp198.g, temp_f28);
            vtx[j2].v.cn[2] = Effect_SS_Uty_uc_interpolation_t01(sp1A0.b, sp198.b, temp_f28);
            vtx[j2].v.cn[3] = Effect_SS_Uty_uc_interpolation_t01(sp1A0.a, sp198.a, temp_f28);
        }

        gSPVertex(POLY_XLU_DISP++, vtx, 16, 0);
        gSP2Triangles(POLY_XLU_DISP++, 0, 1, 3, 0, 0, 3, 2, 0);
        gSP2Triangles(POLY_XLU_DISP++, 2, 3, 5, 0, 2, 5, 4, 0);
        gSP2Triangles(POLY_XLU_DISP++, 4, 5, 7, 0, 4, 7, 6, 0);
        gSP2Triangles(POLY_XLU_DISP++, 6, 7, 9, 0, 6, 9, 8, 0);
        gSP2Triangles(POLY_XLU_DISP++, 8, 9, 11, 0, 8, 11, 10, 0);
        gSP2Triangles(POLY_XLU_DISP++, 10, 11, 13, 0, 10, 13, 12, 0);
        gSP2Triangles(POLY_XLU_DISP++, 12, 13, 15, 0, 12, 15, 14, 0);
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_blure.c", 1184);
}

void EffectBlureInfo2_hermite_disp(EffectBlure* this2, GraphicsContext* gfxCtx) {
    EffectBlure* this = this2;
    EffectBlureElement* elem;
    s32 i;
    MtxF spDC;
    MtxF sp9C;
    MtxF sp5C;
    Mtx* mtx;

    OPEN_DISPS(gfxCtx, "../z_eff_blure.c", 1201);

    if (this->numElements < 2) {
        return;
    }

    this->elements[0].flags &= ~3;
    this->elements[0].flags |= 2;

    for (elem = &this->elements[1]; elem < this->elements + this->numElements - 1; elem++) {
        HermiteInterpolateCheck(elem);
    }

    this->elements[this->numElements - 1].flags &= ~3;
    this->elements[this->numElements - 1].flags |= 2;

    EffectBlureInfo2_hermite_disp_init(this, gfxCtx);
    Skin_Matrix_SetTranslate(&spDC, this->elements[0].p2.x, this->elements[0].p2.y, this->elements[0].p2.z);
    Skin_Matrix_SetScale(&sp9C, 0.1f, 0.1f, 0.1f);
    Skin_Matrix_MulMatrix(&spDC, &sp9C, &sp5C);

    mtx = Skin_Matrix_to_Mtx_new(gfxCtx, &sp5C);
    if (mtx == NULL) {
        return;
    }

    gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    for (i = 0, elem = &this->elements[0]; elem < this->elements + this->numElements - 1; i++, elem++) {
        if ((elem->state == 0) || ((elem + 1)->state == 0)) {
            continue;
        }
        if ((((elem->flags & 3) == 0) && (((elem + 1)->flags & 3) == 0)) ||
            (((elem->flags & 3) == 2) && (((elem + 1)->flags & 3) == 0)) ||
            (((elem->flags & 3) == 0) && (((elem + 1)->flags & 3) == 2)) ||
            (((elem->flags & 3) == 2) && (((elem + 1)->flags & 3) == 2))) {
            SQ_NoInterpolate_disp(this, elem, i, gfxCtx);
        } else {
            SQ_HermiteInterpolate_disp(this, elem, i, gfxCtx);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_blure.c", 1263);
}

void EffectBlureInfo2_disp_makeDisplayListInit0(GraphicsContext* gfxCtx, EffectBlure* this, Vtx* vtx) {
    OPEN_DISPS(gfxCtx, "../z_eff_blure.c", 1280);

    POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_38);

    CLOSE_DISPS(gfxCtx, "../z_eff_blure.c", 1285);
}

void EffectBlureInfo2_disp_makeDisplayListInit1(GraphicsContext* gfxCtx, EffectBlure* this, Vtx* vtx) {
    OPEN_DISPS(gfxCtx, "../z_eff_blure.c", 1294);

    gDPPipeSync(POLY_XLU_DISP++);
    POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_38);

    gDPSetCycleType(POLY_XLU_DISP++, G_CYC_2CYCLE);
    gDPSetTextureLUT(POLY_XLU_DISP++, G_TT_NONE);
    gSPTexture(POLY_XLU_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);

    gDPLoadTextureBlock(POLY_XLU_DISP++, gUnknownEffBlureTex, G_IM_FMT_I, G_IM_SIZ_8b, 64, 32, 0,
                        G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_WRAP, 6, 5, G_TX_NOLOD, G_TX_NOLOD);

    gDPSetCombineLERP(POLY_XLU_DISP++, TEXEL0, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, COMBINED, ENVIRONMENT, 0, 0, 0, COMBINED);
    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
    gSPClearGeometryMode(POLY_XLU_DISP++, G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);
    gSPSetGeometryMode(POLY_XLU_DISP++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH);
    gDPPipeSync(POLY_XLU_DISP++);

    gDPSetEnvColor(POLY_XLU_DISP++, this->altEnvColor.r, this->altEnvColor.g, this->altEnvColor.b, this->altEnvColor.a);

    CLOSE_DISPS(gfxCtx, "../z_eff_blure.c", 1329);
}

void (*init_func_tbl[])(GraphicsContext* gfxCtx, EffectBlure* this, Vtx* vtx) = {
    EffectBlureInfo2_disp_makeDisplayListInit0,
    EffectBlureInfo2_disp_makeDisplayListInit1,
};

s32 D_80115788 = 0; // unused

// original name: "EffectBlureInfo2_disp_makeDisplayList"
void EffectBlureInfo2_disp_makeDisplayList(GraphicsContext* gfxCtx, EffectBlure* this, Vtx* vtx) {
    Mtx* mtx;

    OPEN_DISPS(gfxCtx, "../z_eff_blure.c", 1356);

    init_func_tbl[this->drawMode](gfxCtx, this, vtx);
    gDPPipeSync(POLY_XLU_DISP++);

    {
        Vec3f sp1B0;
        Vec3f sp1A4;
        Vec3f sp198;
        f32 alphaRatio;
        MtxF sp154;
        MtxF sp114;
        MtxF spD4;
        MtxF sp94;
        f32 scale;
        s32 i;

        for (i = 0; i < this->numElements - 1; i++) {
            if (this->drawMode == 1) {
                alphaRatio = (f32)this->elements[i].timer / (f32)this->elemDuration;
                gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, this->altPrimColor.r, this->altPrimColor.g,
                                this->altPrimColor.b, this->altPrimColor.a * (1.0f - alphaRatio));
                gDPPipeSync(POLY_XLU_DISP++);
            }

            gSPVertex(POLY_XLU_DISP++, &vtx[4 * i], 4, 0);
            gSP2Triangles(POLY_XLU_DISP++, 0, 1, 3, 0, 0, 3, 2, 0);

            if (this->flags & 4) {
                sp1B0.x = ((f32)vtx[4 * i + 0].v.ob[0] + (f32)vtx[4 * i + 1].v.ob[0]) * 0.5f;
                sp1B0.y = ((f32)vtx[4 * i + 0].v.ob[1] + (f32)vtx[4 * i + 1].v.ob[1]) * 0.5f;
                sp1B0.z = ((f32)vtx[4 * i + 0].v.ob[2] + (f32)vtx[4 * i + 1].v.ob[2]) * 0.5f;
                sp1A4.x = ((f32)vtx[4 * i + 2].v.ob[0] + (f32)vtx[4 * i + 3].v.ob[0]) * 0.5f;
                sp1A4.y = ((f32)vtx[4 * i + 2].v.ob[1] + (f32)vtx[4 * i + 3].v.ob[1]) * 0.5f;
                sp1A4.z = ((f32)vtx[4 * i + 2].v.ob[2] + (f32)vtx[4 * i + 3].v.ob[2]) * 0.5f;

                xyz_t_sub(&sp1A4, &sp1B0, &sp198);
                scale = sqrtf(SQ(sp198.x) + SQ(sp198.y) + SQ(sp198.z));

                if (fabsf(scale) > 0.0005f) {
                    scale = 1.0f / scale;
                    xyz_t_mult_v(&sp198, scale);

                    Skin_Matrix_SetTranslate(&sp154, sp1B0.x, sp1B0.y, sp1B0.z);
                    Skin_Matrix_SetFreeVecRotMatrix(&sp114, 0x3FFF, sp198.x, sp198.y, sp198.z);
                    Skin_Matrix_MulMatrix(&sp154, &sp114, &spD4);
                    Skin_Matrix_SetTranslate(&sp154, -sp1B0.x, -sp1B0.y, -sp1B0.z);
                    Skin_Matrix_MulMatrix(&spD4, &sp154, &sp94);

                    mtx = Skin_Matrix_to_Mtx_new(gfxCtx, &sp94);
                    if (mtx == NULL) {
                        PRINTF(T("EffectBlureInfo2_disp_makeDisplayList()マトリックス取れないので,強制終了\n",
                                 "EffectBlureInfo2_disp_makeDisplayList() Forced termination because a matrix cannot "
                                 "be taken\n"));
                        break;
                    }

                    gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                    gSPVertex(POLY_XLU_DISP++, &vtx[4 * i], 4, 0);
                    gSP2Triangles(POLY_XLU_DISP++, 0, 1, 3, 0, 0, 3, 2, 0);
                    gSPMatrix(POLY_XLU_DISP++, &Mtx_clear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
                }
            }
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_blure.c", 1452);
}

Vtx_t tail_dv[] = {
    VTX_T(0, 0, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX_T(0, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX_T(0, 0, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX_T(0, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};

Vtx_t body_dv[] = {
    VTX_T(0, 0, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX_T(0, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX_T(0, 0, 0, 2048, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    VTX_T(0, 0, 0, 2048, 0, 0xFF, 0xFF, 0xFF, 0xFF),
};

void EffectBlureInfo2_disp(EffectBlure* this2, GraphicsContext* gfxCtx) {
    EffectBlure* this = this2;
    Vtx* vtx;
    Vtx* vtxIter;
    EffectBlureElement* elem;
    s32 vtxCount;
    s32 i;
    s32 j;
    Vec3s sp74;
    Vec3s sp6C;
    f32 ratio;
    Color_RGBA8 sp64;
    Color_RGBA8 sp60;

    if (this->numElements >= 2) {
        vtxCount = this->numElements * 4;

        vtx = GRAPH_ALLOC(gfxCtx, vtxCount * sizeof(Vtx));
        if (vtx == NULL) {
            PRINTF(T("ブラ─表示:頂点確保できず。強制終了\n",
                     "Blur - display: vertices cannot be secured. Forced termination\n"));
            return;
        }

        vtxIter = vtx;
        for (i = 0; i < 4; i++) {
            vtxIter->v = tail_dv[i];
            vtxIter++;
        }

        if (this->numElements >= 2) {
            for (elem = this->elements; elem < this->elements + this->numElements - 2; elem++) {
                for (i = 0; i < 4; i++) {
                    vtxIter->v = body_dv[i];
                    vtxIter++;
                }
            }
        }

        for (i = 0; i < this->numElements; i++) {
            elem = &this->elements[i];

            ratio = (f32)elem->timer / (f32)this->elemDuration;
            EffectBlureInfo2_disp_calc(this, i, ratio, &sp74, &sp6C, &sp64, &sp60);

            j = i * 4 - 2;
            if (j >= 0) {
                vtx[j].v.ob[0] = sp74.x;
                vtx[j].v.ob[1] = sp74.y;
                vtx[j].v.ob[2] = sp74.z;
                vtx[j].v.cn[0] = sp64.r;
                vtx[j].v.cn[1] = sp64.g;
                vtx[j].v.cn[2] = sp64.b;
                vtx[j].v.cn[3] = sp64.a;
            }

            j++;
            if (j >= 0) {
                vtx[j].v.ob[0] = sp6C.x;
                vtx[j].v.ob[1] = sp6C.y;
                vtx[j].v.ob[2] = sp6C.z;
                vtx[j].v.cn[0] = sp60.r;
                vtx[j].v.cn[1] = sp60.g;
                vtx[j].v.cn[2] = sp60.b;
                vtx[j].v.cn[3] = sp60.a;
            }

            j++;
            if (vtxCount >= j) {
                vtx[j].v.ob[0] = sp74.x;
                vtx[j].v.ob[1] = sp74.y;
                vtx[j].v.ob[2] = sp74.z;
                vtx[j].v.cn[0] = sp64.r;
                vtx[j].v.cn[1] = sp64.g;
                vtx[j].v.cn[2] = sp64.b;
                vtx[j].v.cn[3] = sp64.a;
            }

            j++;
            if (vtxCount >= j) {
                vtx[j].v.ob[0] = sp6C.x;
                vtx[j].v.ob[1] = sp6C.y;
                vtx[j].v.ob[2] = sp6C.z;
                vtx[j].v.cn[0] = sp60.r;
                vtx[j].v.cn[1] = sp60.g;
                vtx[j].v.cn[2] = sp60.b;
                vtx[j].v.cn[3] = sp60.a;
            }
        }

        EffectBlureInfo2_disp_makeDisplayList(gfxCtx, this, vtx);
    }
}

void EffectBlureInfo_disp(void* thisx, GraphicsContext* gfxCtx) {
    EffectBlure* this = (EffectBlure*)thisx;
    Vtx* vtx;
    EffectBlureElement* elem;
    s32 i;
    s32 j;
    s32 flag;

    OPEN_DISPS(gfxCtx, "../z_eff_blure.c", 1596);

    gSPMatrix(POLY_XLU_DISP++, &Mtx_clear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (this->numElements != 0) {
        if (this->flags == 0) {
            vertex_color_xlu_polygon(gfxCtx);
            gDPPipeSync(POLY_XLU_DISP++);

            vtx = GRAPH_ALLOC(gfxCtx, sizeof(Vtx[32]));
            if (vtx == NULL) {
                PRINTF(T("ブラ─表示:頂点テーブル確保できず\n", "Blur - display: vertex table could not be secured\n"));
            } else {
                j = 0;
                for (i = 0; i < this->numElements; i++) {
                    elem = &this->elements[i];

                    if (elem->state == 1) {
                        f32 ratio = (f32)elem->timer / (f32)this->elemDuration;

                        switch (this->calcMode) {
                            case 1:
                                vtx[j].v.ob[0] = Effect_SS_Uty_short_interpolation_t01(elem->p1.x, elem->p2.x, ratio);
                                vtx[j].v.ob[1] = Effect_SS_Uty_short_interpolation_t01(elem->p1.y, elem->p2.y, ratio);
                                vtx[j].v.ob[2] = Effect_SS_Uty_short_interpolation_t01(elem->p1.z, elem->p2.z, ratio);
                                vtx[j + 1].v.ob[0] = elem->p2.x;
                                vtx[j + 1].v.ob[1] = elem->p2.y;
                                vtx[j + 1].v.ob[2] = elem->p2.z;
                                break;
                            case 2:
                                vtx[j].v.ob[0] = elem->p1.x;
                                vtx[j].v.ob[1] = elem->p1.y;
                                vtx[j].v.ob[2] = elem->p1.z;
                                vtx[j + 1].v.ob[0] = Effect_SS_Uty_short_interpolation_t01(elem->p2.x, elem->p1.x, ratio);
                                vtx[j + 1].v.ob[1] = Effect_SS_Uty_short_interpolation_t01(elem->p2.y, elem->p1.y, ratio);
                                vtx[j + 1].v.ob[2] = Effect_SS_Uty_short_interpolation_t01(elem->p2.z, elem->p1.z, ratio);
                                break;
                            case 3:
                                ratio *= 0.5f;
                                vtx[j].v.ob[0] = Effect_SS_Uty_short_interpolation_t01(elem->p1.x, elem->p2.x, ratio);
                                vtx[j].v.ob[1] = Effect_SS_Uty_short_interpolation_t01(elem->p1.y, elem->p2.y, ratio);
                                vtx[j].v.ob[2] = Effect_SS_Uty_short_interpolation_t01(elem->p1.z, elem->p2.z, ratio);
                                vtx[j + 1].v.ob[0] = Effect_SS_Uty_short_interpolation_t01(elem->p2.x, elem->p1.x, ratio);
                                vtx[j + 1].v.ob[1] = Effect_SS_Uty_short_interpolation_t01(elem->p2.y, elem->p1.y, ratio);
                                vtx[j + 1].v.ob[2] = Effect_SS_Uty_short_interpolation_t01(elem->p2.z, elem->p1.z, ratio);
                                ratio *= 2.0f;
                                break;
                            case 0:
                            default:
                                vtx[j].v.ob[0] = elem->p1.x;
                                vtx[j].v.ob[1] = elem->p1.y;
                                vtx[j].v.ob[2] = elem->p1.z;
                                vtx[j + 1].v.ob[0] = elem->p2.x;
                                vtx[j + 1].v.ob[1] = elem->p2.y;
                                vtx[j + 1].v.ob[2] = elem->p2.z;
                                break;
                        }

                        vtx[j].v.flag = 0;
                        vtx[j].v.tc[0] = 0;
                        vtx[j].v.tc[1] = 0;
                        vtx[j].v.cn[0] = Effect_SS_Uty_uc_interpolation_t01(this->p1StartColor[0], this->p1EndColor[0], ratio);
                        vtx[j].v.cn[1] = Effect_SS_Uty_uc_interpolation_t01(this->p1StartColor[1], this->p1EndColor[1], ratio);
                        vtx[j].v.cn[2] = Effect_SS_Uty_uc_interpolation_t01(this->p1StartColor[2], this->p1EndColor[2], ratio);
                        vtx[j].v.cn[3] = Effect_SS_Uty_uc_interpolation_t01(this->p1StartColor[3], this->p1EndColor[3], ratio);
                        j++;

                        vtx[j].v.flag = 0;
                        vtx[j].v.tc[0] = 0;
                        vtx[j].v.tc[1] = 0;
                        vtx[j].v.cn[0] = Effect_SS_Uty_uc_interpolation_t01(this->p2StartColor[0], this->p2EndColor[0], ratio);
                        vtx[j].v.cn[1] = Effect_SS_Uty_uc_interpolation_t01(this->p2StartColor[1], this->p2EndColor[1], ratio);
                        vtx[j].v.cn[2] = Effect_SS_Uty_uc_interpolation_t01(this->p2StartColor[2], this->p2EndColor[2], ratio);
                        vtx[j].v.cn[3] = Effect_SS_Uty_uc_interpolation_t01(this->p2StartColor[3], this->p2EndColor[3], ratio);
                        j++;
                    }
                }

                flag = 0;
                j = 0;

                gSPVertex(POLY_XLU_DISP++, vtx, 32, 0);

                for (i = 0; i < this->numElements; i++) {
                    elem = &this->elements[i];

                    if (elem->state == 0) {
                        flag = 0;
                    } else {
                        if (flag == 0) {
                            flag = 1;
                        } else {
                            gSP1Quadrangle(POLY_XLU_DISP++, j - 2, j - 1, j + 1, j, 0);

                            if (1) {} // Necessary to match

                            if (this->unkFlag == 1) {
                                flag = 0;
                            }
                        }
                        j += 2;
                    }
                }
            }
        } else if (this->drawMode < 2) {
            EffectBlureInfo2_disp(this, gfxCtx);
        } else {
            EffectBlureInfo2_hermite_disp(this, gfxCtx);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_blure.c", 1823);
}
