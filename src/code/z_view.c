#include "global.h"
#include "terminal.h"

vu32 First = true;

s32 showPerspectiveView(View*);
s32 showOrthoView(View*);

void set_viewport(Vp* dest, Viewport* src) {
    s32 width = src->rightX - src->leftX;
    s32 height = src->bottomY - src->topY;

    dest->vp.vscale[0] = width * 2;
    dest->vp.vscale[1] = height * 2;
    dest->vp.vscale[2] = G_MAXZ / 2;
    dest->vp.vscale[3] = 0;
    dest->vp.vtrans[0] = ((src->leftX * 2) + width) * 2;
    dest->vp.vtrans[1] = ((src->topY * 2) + height) * 2;
    dest->vp.vtrans[2] = G_MAXZ / 2;
    dest->vp.vtrans[3] = 0;
}

View* creteView(GraphicsContext* gfxCtx) {
    View* view = SYSTEM_ARENA_MALLOC(sizeof(View), "../z_view.c", 285);

    if (view != NULL) {
        memset(view, 0, sizeof(View));
        initView(view, gfxCtx);
    }

    return view;
}

void destroyView(View* view) {
    SYSTEM_ARENA_FREE(view, "../z_view.c", 297);
}

void initView(View* view, GraphicsContext* gfxCtx) {
    view->gfxCtx = gfxCtx;

    view->viewport.topY = 0;
    view->viewport.bottomY = SCREEN_HEIGHT;
    view->viewport.leftX = 0;
    view->viewport.rightX = SCREEN_WIDTH;

    view->magic = 0x56494557; // "VIEW"

    if (First == false) {}

    view->scale = 1.0f;
    view->fovy = 60.0f;
    view->zNear = 10.0f;
    view->zFar = 12800.0f;

    view->eye.x = 0.0f;
    view->eye.y = 0.0f;
    view->eye.z = -1.0f;
    view->at.x = 0.0f;
    view->up.x = 0.0f;
    view->up.y = 1.0f;
    view->up.z = 0.0f;

    if (First) {
        PRINTF("\nview: initialize ---\n");
        First = false;
    }

    view->unk_124 = 0;
    view->flags = VIEW_VIEWING | VIEW_VIEWPORT | VIEW_PROJECTION_PERSPECTIVE;
    stretchViewInit(view);
}

void setLookAtView(View* view, Vec3f* eye, Vec3f* at, Vec3f* up) {
    if (eye->x == at->x && eye->z == at->z) {
        eye->x += 0.1f;
    }

    view->eye = *eye;
    view->at = *at;
    view->up = *up;
    view->flags |= VIEW_VIEWING;
}

/*
 * Unused. setLookAtView is always used instead. This version is similar but
 * is missing the input sanitization and the update to the flags.
 */
void getLookAtView(View* view, Vec3f* eye, Vec3f* at, Vec3f* up) {
    view->eye = *eye;
    view->at = *at;
    view->up = *up;
}

void setScaleView(View* view, f32 scale) {
    view->flags |= VIEW_PROJECTION_PERSPECTIVE;
    view->scale = scale;
}

void getScaleView(View* view, f32* scale) {
    *scale = view->scale;
}

void setPerspectiveView(View* view, f32 fovy, f32 zNear, f32 zFar) {
    view->fovy = fovy;
    view->zNear = zNear;
    view->zFar = zFar;
    view->flags |= VIEW_PROJECTION_PERSPECTIVE;
}

void getPerspectiveView(View* view, f32* fovy, f32* zNear, f32* zFar) {
    *fovy = view->fovy;
    *zNear = view->zNear;
    *zFar = view->zFar;
}

void setOrthoView(View* view, f32 fovy, f32 zNear, f32 zFar) {
    view->fovy = fovy;
    view->zNear = zNear;
    view->zFar = zFar;
    view->flags |= VIEW_PROJECTION_ORTHO;
    view->scale = 1.0f;
}

/*
 * Identical to getPerspectiveView, and never called.
 * Named as it seems to fit the "set, get" pattern.
 */
void getOrthoView(View* view, f32* fovy, f32* zNear, f32* zFar) {
    *fovy = view->fovy;
    *zNear = view->zNear;
    *zFar = view->zFar;
}

void setScissorView(View* view, Viewport* viewport) {
    view->viewport = *viewport;
    view->flags |= VIEW_VIEWPORT;
}

void getScissorView(View* view, Viewport* viewport) {
    *viewport = view->viewport;
}

void setScissor(View* view) {
    GraphicsContext* gfxCtx = view->gfxCtx;
    s32 pillarboxSize;
    s32 letterboxSize;
    s32 ulx;
    s32 uly;
    s32 lrx;
    s32 lry;
    s32 pad;

    letterboxSize = shrink_window_getnowval();

    // The following is optimized to pillarboxSize = 0 but affects codegen
    pillarboxSize = -1;

    if (pillarboxSize < 0) {
        pillarboxSize = 0;
    }
    if (pillarboxSize > SCREEN_WIDTH / 2) {
        pillarboxSize = SCREEN_WIDTH / 2;
    }

    if (letterboxSize < 0) {
        letterboxSize = 0;
    }
    if (letterboxSize > SCREEN_HEIGHT / 2) {
        letterboxSize = SCREEN_HEIGHT / 2;
    }

    ulx = view->viewport.leftX + pillarboxSize;
    uly = view->viewport.topY + letterboxSize;
    lrx = view->viewport.rightX - pillarboxSize;
    lry = view->viewport.bottomY - letterboxSize;

    ASSERT(ulx >= 0, "ulx >= 0", "../z_view.c", 454);
    ASSERT(uly >= 0, "uly >= 0", "../z_view.c", 455);
    ASSERT(lrx <= SCREEN_WIDTH, "lrx <= SCREEN_WD", "../z_view.c", 456);
    ASSERT(lry <= SCREEN_HEIGHT, "lry <= SCREEN_HT", "../z_view.c", 457);

    OPEN_DISPS(gfxCtx, "../z_view.c", 459);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetScissor(POLY_OPA_DISP++, G_SC_NON_INTERLACE, ulx, uly, lrx, lry);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetScissor(POLY_XLU_DISP++, G_SC_NON_INTERLACE, ulx, uly, lrx, lry);

    CLOSE_DISPS(gfxCtx, "../z_view.c", 472);
}

void stretchViewRotate(View* view, f32 rotX, f32 rotY, f32 rotZ) {
    view->distortionOrientation.x = rotX;
    view->distortionOrientation.y = rotY;
    view->distortionOrientation.z = rotZ;
}

void stretchViewScale(View* view, f32 scaleX, f32 scaleY, f32 scaleZ) {
    view->distortionScale.x = scaleX;
    view->distortionScale.y = scaleY;
    view->distortionScale.z = scaleZ;
}

BAD_RETURN(s32) stretchViewSpeed(View* view, f32 speed) {
    view->distortionSpeed = speed;
}

void stretchViewInit(View* view) {
    view->distortionOrientation.x = 0.0f;
    view->distortionOrientation.y = 0.0f;
    view->distortionOrientation.z = 0.0f;
    view->distortionScale.x = 1.0f;
    view->distortionScale.y = 1.0f;
    view->distortionScale.z = 1.0f;
    view->curDistortionOrientation = view->distortionOrientation;
    view->curDistortionScale = view->distortionScale;
    view->distortionSpeed = 0.0f;
}

void stretchViewReset(View* view) {
    view->distortionOrientation.x = 0.0f;
    view->distortionOrientation.y = 0.0f;
    view->distortionOrientation.z = 0.0f;
    view->distortionScale.x = 1.0f;
    view->distortionScale.y = 1.0f;
    view->distortionScale.z = 1.0f;
    view->distortionSpeed = 1.0f;
}

void stretchViewSet(View* view, Vec3f orientation, Vec3f scale, f32 speed) {
    view->distortionOrientation = orientation;
    view->distortionScale = scale;
    view->distortionSpeed = speed;
}

s32 do_stretch_view(View* view, Mtx* projectionMtx) {
    MtxF projectionMtxF;

    if (view->distortionSpeed == 0.0f) {
        return false;
    } else if (view->distortionSpeed == 1.0f) {
        view->curDistortionOrientation = view->distortionOrientation;
        view->curDistortionScale = view->distortionScale;
        view->distortionSpeed = 0.0f;
    } else {
        view->curDistortionOrientation.x =
            F32_LERPIMP(view->curDistortionOrientation.x, view->distortionOrientation.x, view->distortionSpeed);
        view->curDistortionOrientation.y =
            F32_LERPIMP(view->curDistortionOrientation.y, view->distortionOrientation.y, view->distortionSpeed);
        view->curDistortionOrientation.z =
            F32_LERPIMP(view->curDistortionOrientation.z, view->distortionOrientation.z, view->distortionSpeed);

        view->curDistortionScale.x =
            F32_LERPIMP(view->curDistortionScale.x, view->distortionScale.x, view->distortionSpeed);
        view->curDistortionScale.y =
            F32_LERPIMP(view->curDistortionScale.y, view->distortionScale.y, view->distortionSpeed);
        view->curDistortionScale.z =
            F32_LERPIMP(view->curDistortionScale.z, view->distortionScale.z, view->distortionSpeed);
    }

    Matrix_MtxtoMtxF(projectionMtx, &projectionMtxF);
    Matrix_put(&projectionMtxF);
    Matrix_rotateX(view->curDistortionOrientation.x, MTXMODE_APPLY);
    Matrix_rotateY(view->curDistortionOrientation.y, MTXMODE_APPLY);
    Matrix_rotateZ(view->curDistortionOrientation.z, MTXMODE_APPLY);
    Matrix_scale(view->curDistortionScale.x, view->curDistortionScale.y, view->curDistortionScale.z, MTXMODE_APPLY);
    Matrix_rotateZ(-view->curDistortionOrientation.z, MTXMODE_APPLY);
    Matrix_rotateY(-view->curDistortionOrientation.y, MTXMODE_APPLY);
    Matrix_rotateX(-view->curDistortionOrientation.x, MTXMODE_APPLY);
    MATRIX_TO_MTX(projectionMtx, "../z_view.c", 566);

    return true;
}

/**
 * Apply view to POLY_OPA_DISP, POLY_XLU_DISP (and OVERLAY_DISP if ortho)
 */
s32 showView(View* view, s32 mask) {
    mask = (view->flags & mask) | (mask >> 4);

    if (mask & VIEW_PROJECTION_ORTHO) {
        return showOrthoView(view);
    } else {
        return showPerspectiveView(view);
    }
}

s32 showPerspectiveView(View* view) {
    GraphicsContext* gfxCtx = view->gfxCtx;
    s32 width;
    s32 height;
    Vp* vp;
    Mtx* projection;
    Mtx* viewing;
    f32 aspect;

    OPEN_DISPS(gfxCtx, "../z_view.c", 596);

    // Viewport
    vp = GRAPH_ALLOC(gfxCtx, sizeof(Vp));
    LOG_UTILS_CHECK_NULL_POINTER("vp", vp, "../z_view.c", 601);
    set_viewport(vp, &view->viewport);
    view->vp = *vp;

    setScissor(view);

    gSPViewport(POLY_OPA_DISP++, vp);
    gSPViewport(POLY_XLU_DISP++, vp);

    // Perspective projection
    projection = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    LOG_UTILS_CHECK_NULL_POINTER("projection", projection, "../z_view.c", 616);
    view->projectionPtr = projection;

    width = view->viewport.rightX - view->viewport.leftX;
    height = view->viewport.bottomY - view->viewport.topY;
    aspect = (f32)width / (f32)height;

    if (DEBUG_FEATURES && R_HREG_MODE == HREG_MODE_PERSPECTIVE) {
        if (R_PERSPECTIVE_INIT != HREG_MODE_PERSPECTIVE) {
            R_PERSPECTIVE_INIT = HREG_MODE_PERSPECTIVE;
            R_PERSPECTIVE_FOVY = 60;
            R_PERSPECTIVE_ASPECT = (10000 * 4) / 3;
            R_PERSPECTIVE_NEAR = 10;
            R_PERSPECTIVE_FAR = 12800;
            R_PERSPECTIVE_SCALE = 100;
        }
        guPerspective(projection, &view->normal, R_PERSPECTIVE_FOVY, R_PERSPECTIVE_ASPECT / 10000.0f,
                      R_PERSPECTIVE_NEAR, R_PERSPECTIVE_FAR, R_PERSPECTIVE_SCALE / 100.0f);
    } else {
        guPerspective(projection, &view->normal, view->fovy, aspect, view->zNear, view->zFar, view->scale);
    }

#if DEBUG_FEATURES
    if (QREG(88) & 1) {
        s32 i;
        MtxF mf;

        PRINTF("fovy %f near %f far %f scale %f aspect %f normal %08x\n", view->fovy, view->zNear, view->zFar,
               view->scale, aspect, view->normal);

        Matrix_MtxtoMtxF(projection, &mf);
        PRINTF("projection\n");
        for (i = 0; i < 4; i++) {
            PRINTF("\t%f\t%f\t%f\t%f\n", mf.mf[i][0], mf.mf[i][1], mf.mf[i][2], mf.mf[i][3]);
        }
        PRINTF("\n");
    }
#endif

    view->projection = *projection;

    do_stretch_view(view, projection);

    gSPPerspNormalize(POLY_OPA_DISP++, view->normal);
    gSPMatrix(POLY_OPA_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPPerspNormalize(POLY_XLU_DISP++, view->normal);
    gSPMatrix(POLY_XLU_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    // View matrix (look-at)
    viewing = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    LOG_UTILS_CHECK_NULL_POINTER("viewing", viewing, "../z_view.c", 667);
    view->viewingPtr = viewing;

    if (view->eye.x == view->at.x && view->eye.y == view->at.y && view->eye.z == view->at.z) {
        view->eye.x += 1.0f;
        view->eye.y += 1.0f;
        view->eye.z += 1.0f;
    }

    VIEW_ERROR_CHECK_EYE_POS(view->eye.x, view->eye.y, view->eye.z);

    guLookAt(viewing, view->eye.x, view->eye.y, view->eye.z, view->at.x, view->at.y, view->at.z, view->up.x, view->up.y,
             view->up.z);

    view->viewing = *viewing;

#if DEBUG_FEATURES
    // Debug print view matrix
    if (QREG(88) & 2) {
        s32 i;
        MtxF mf;

        Matrix_MtxtoMtxF(view->viewingPtr, &mf);

        PRINTF("viewing\n");
        for (i = 0; i < 4; i++) {
            PRINTF("\t%f\t%f\t%f\t%f\n", mf.mf[i][0], mf.mf[i][1], mf.mf[i][2], mf.mf[i][3]);
        }
        PRINTF("\n");
    }
#endif

    gSPMatrix(POLY_OPA_DISP++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
    gSPMatrix(POLY_XLU_DISP++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    CLOSE_DISPS(gfxCtx, "../z_view.c", 711);

    return 1;
}

s32 showOrthoView(View* view) {
    GraphicsContext* gfxCtx = view->gfxCtx;
    Vp* vp;
    Mtx* projection;

    OPEN_DISPS(gfxCtx, "../z_view.c", 726);

    vp = GRAPH_ALLOC(gfxCtx, sizeof(Vp));
    LOG_UTILS_CHECK_NULL_POINTER("vp", vp, "../z_view.c", 730);
    set_viewport(vp, &view->viewport);
    view->vp = *vp;

    setScissor(view);

    gSPViewport(POLY_OPA_DISP++, vp);
    gSPViewport(POLY_XLU_DISP++, vp);
    gSPViewport(OVERLAY_DISP++, vp);

    projection = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    LOG_UTILS_CHECK_NULL_POINTER("projection", projection, "../z_view.c", 744);
    view->projectionPtr = projection;

    guOrtho(projection, -(f32)ScreenWidth * 0.5f, (f32)ScreenWidth * 0.5f, -(f32)ScreenHeight * 0.5f,
            (f32)ScreenHeight * 0.5f, view->zNear, view->zFar, view->scale);

    view->projection = *projection;

    gSPMatrix(POLY_OPA_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    gSPMatrix(POLY_XLU_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    CLOSE_DISPS(gfxCtx, "../z_view.c", 762);

    return 1;
}

/**
 * Apply scissor, viewport and projection (ortho) to OVERLAY_DISP.
 */
s32 showOverLayView(View* view) {
    GraphicsContext* gfxCtx = view->gfxCtx;
    Vp* vp;
    Mtx* projection;

    OPEN_DISPS(gfxCtx, "../z_view.c", 777);

    vp = GRAPH_ALLOC(gfxCtx, sizeof(Vp));
    LOG_UTILS_CHECK_NULL_POINTER("vp", vp, "../z_view.c", 781);
    set_viewport(vp, &view->viewport);
    view->vp = *vp;

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetScissor(OVERLAY_DISP++, G_SC_NON_INTERLACE, view->viewport.leftX, view->viewport.topY, view->viewport.rightX,
                  view->viewport.bottomY);
    gSPViewport(OVERLAY_DISP++, vp);

    projection = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    LOG_UTILS_CHECK_NULL_POINTER("projection", projection, "../z_view.c", 791);
    view->projectionPtr = projection;

    guOrtho(projection, -(f32)ScreenWidth * 0.5f, (f32)ScreenWidth * 0.5f, -(f32)ScreenHeight * 0.5f,
            (f32)ScreenHeight * 0.5f, view->zNear, view->zFar, view->scale);

    view->projection = *projection;

    gSPMatrix(OVERLAY_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    CLOSE_DISPS(gfxCtx, "../z_view.c", 801);

    return 1;
}

/**
 * Apply scissor, viewport, view and projection (perspective) to OVERLAY_DISP.
 */
s32 showPerspectiveOverLayView(View* view) {
    GraphicsContext* gfxCtx = view->gfxCtx;
    s32 pad;
    f32 aspect;
    s32 width;
    s32 height;
    Vp* vp;
    Mtx* projection;
    Mtx* viewing;
    s32 pad1;

    OPEN_DISPS(gfxCtx, "../z_view.c", 816);

    vp = GRAPH_ALLOC(gfxCtx, sizeof(Vp));
    LOG_UTILS_CHECK_NULL_POINTER("vp", vp, "../z_view.c", 821);
    set_viewport(vp, &view->viewport);
    view->vp = *vp;

    gDPPipeSync(OVERLAY_DISP++);
    gDPSetScissor(OVERLAY_DISP++, G_SC_NON_INTERLACE, view->viewport.leftX, view->viewport.topY, view->viewport.rightX,
                  view->viewport.bottomY);
    gSPViewport(OVERLAY_DISP++, vp);

    projection = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    LOG_UTILS_CHECK_NULL_POINTER("projection", projection, "../z_view.c", 833);
    view->projectionPtr = projection;

    width = view->viewport.rightX - view->viewport.leftX;
    height = view->viewport.bottomY - view->viewport.topY;

    aspect = (f32)width / (f32)height;
    guPerspective(projection, &view->normal, view->fovy, aspect, view->zNear, view->zFar, view->scale);

    view->projection = *projection;

    gSPPerspNormalize(OVERLAY_DISP++, view->normal);
    gSPMatrix(OVERLAY_DISP++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    viewing = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
    LOG_UTILS_CHECK_NULL_POINTER("viewing", viewing, "../z_view.c", 848);
    view->viewingPtr = viewing;

    // This check avoids a divide-by-zero in guLookAt if eye == at
    if (view->eye.x == view->at.x && view->eye.y == view->at.y && view->eye.z == view->at.z) {
        view->eye.x += 1.0f;
        view->eye.y += 1.0f;
        view->eye.z += 1.0f;
    }

    VIEW_ERROR_CHECK_EYE_POS(view->eye.x, view->eye.y, view->eye.z);

    guLookAt(viewing, view->eye.x, view->eye.y, view->eye.z, view->at.x, view->at.y, view->at.z, view->up.x, view->up.y,
             view->up.z);

    view->viewing = *viewing;

    gSPMatrix(OVERLAY_DISP++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    CLOSE_DISPS(gfxCtx, "../z_view.c", 871);

    return 1;
}

/**
 * Just updates view's view matrix from its eye/at/up vectors. Opens disps but doesn't use them.
 */
s32 showViewAgain(View* view) {
    OPEN_DISPS(view->gfxCtx, "../z_view.c", 878);

    VIEW_ERROR_CHECK_EYE_POS(view->eye.x, view->eye.y, view->eye.z);

    guLookAt(view->viewingPtr, view->eye.x, view->eye.y, view->eye.z, view->at.x, view->at.y, view->at.z, view->up.x,
             view->up.y, view->up.z);

    CLOSE_DISPS(view->gfxCtx, "../z_view.c", 886);

    return 1;
}

s32 showView1(View* view, s32 mask, Gfx** gfxP) {
    Gfx* gfx = *gfxP;
    GraphicsContext* gfxCtx = view->gfxCtx;
    s32 width;
    s32 height;
    Vp* vp;
    Mtx* projection;
    Mtx* viewing;

    mask = (view->flags & mask) | (mask >> 4);

    if (mask & VIEW_VIEWPORT) {
        vp = GRAPH_ALLOC(gfxCtx, sizeof(Vp));
        LOG_UTILS_CHECK_NULL_POINTER("vp", vp, "../z_view.c", 910);
        set_viewport(vp, &view->viewport);

        view->vp = *vp;

        gDPPipeSync(gfx++);
        gDPSetScissor(gfx++, G_SC_NON_INTERLACE, view->viewport.leftX, view->viewport.topY, view->viewport.rightX,
                      view->viewport.bottomY);
        gSPViewport(gfx++, vp);
    }

    if (mask & VIEW_PROJECTION_ORTHO) {
        projection = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
        LOG_UTILS_CHECK_NULL_POINTER("projection", projection, "../z_view.c", 921);
        view->projectionPtr = projection;

        guOrtho(projection, -(f32)ScreenWidth * 0.5f, (f32)ScreenWidth * 0.5f, -(f32)ScreenHeight * 0.5f,
                (f32)ScreenHeight * 0.5f, view->zNear, view->zFar, view->scale);

        view->projection = *projection;

        gSPMatrix(gfx++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    } else if (mask & (VIEW_PROJECTION_PERSPECTIVE | VIEW_VIEWPORT)) {
        projection = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
        LOG_UTILS_CHECK_NULL_POINTER("projection", projection, "../z_view.c", 932);
        view->projectionPtr = projection;

        width = view->viewport.rightX - view->viewport.leftX;
        height = view->viewport.bottomY - view->viewport.topY;

        guPerspective(projection, &view->normal, view->fovy, (f32)width / (f32)height, view->zNear, view->zFar,
                      view->scale);

        view->projection = *projection;

        gSPPerspNormalize(gfx++, view->normal);
        gSPMatrix(gfx++, projection, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);
    }

    if (mask & VIEW_VIEWING) {
        viewing = GRAPH_ALLOC(gfxCtx, sizeof(Mtx));
        LOG_UTILS_CHECK_NULL_POINTER("viewing", viewing, "../z_view.c", 948);
        view->viewingPtr = viewing;

        VIEW_ERROR_CHECK_EYE_POS(view->eye.x, view->eye.y, view->eye.z);

        guLookAt(viewing, view->eye.x, view->eye.y, view->eye.z, view->at.x, view->at.y, view->at.z, view->up.x,
                 view->up.y, view->up.z);

        view->viewing = *viewing;

        gSPMatrix(gfx++, viewing, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);
    }

    view->flags = 0;
    *gfxP = gfx;

    return 1;
}

#if DEBUG_FEATURES
/**
 * Logs an error and returns nonzero if camera is too far from the origin.
 */
s32 View_ErrorCheckEyePosition(f32 eyeX, f32 eyeY, f32 eyeZ) {
    s32 error = 0;

    if (SQ(eyeX) + SQ(eyeY) + SQ(eyeZ) > SQ(32767.0f)) {
        error = 3;
    } else {
        f32 absEyeX = ABS(eyeX);
        f32 absEyeY = ABS(eyeY);
        f32 absEyeZ = ABS(eyeZ);

        if (((18900.0f < absEyeX) || (18900.0f < absEyeY)) || (18900.0f < absEyeZ)) {
            error = 2;
        } else if (((16000.0f < absEyeX) || (16000.0f < absEyeY)) || (16000.0f < absEyeZ)) {
            error = 1;
        }
    }

    if (error != 0) {
        PRINTF_COLOR_RED();
        PRINTF(T("eye が大きすぎます eye=[%8.3f %8.3f %8.3f] error=%d\n",
                 "eye is too large eye=[%8.3f %8.3f %8.3f] error=%d\n"),
               eyeX, eyeY, eyeZ, error);
        PRINTF_RST();
    }

    return error;
}
#endif
