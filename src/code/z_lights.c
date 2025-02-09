#include "global.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define LIGHTS_BUFFER_SIZE 32

typedef struct LightsBuffer {
    /* 0x000 */ s32 numOccupied;
    /* 0x004 */ s32 searchIndex;
    /* 0x008 */ LightNode buf[LIGHTS_BUFFER_SIZE];
} LightsBuffer; // size = 0x188

LightsBuffer light_list_buf;

void point_data_set(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius, s32 type) {
    info->type = type;
    info->params.point.x = x;
    info->params.point.y = y;
    info->params.point.z = z;
    Light_point_color_set(info, r, g, b, radius);
}

void Light_point_ct(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius) {
    point_data_set(info, x, y, z, r, g, b, radius, LIGHT_POINT_NOGLOW);
}

void Light_point2_ct(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius) {
    point_data_set(info, x, y, z, r, g, b, radius, LIGHT_POINT_GLOW);
}

void Light_point_color_set(LightInfo* info, u8 r, u8 g, u8 b, s16 radius) {
    info->params.point.color[0] = r;
    info->params.point.color[1] = g;
    info->params.point.color[2] = b;
    info->params.point.radius = radius;
}

void Light_diffuse_ct(LightInfo* info, s8 x, s8 y, s8 z, u8 r, u8 g, u8 b) {
    info->type = LIGHT_DIRECTIONAL;
    info->params.dir.x = x;
    info->params.dir.y = y;
    info->params.dir.z = z;
    info->params.dir.color[0] = r;
    info->params.dir.color[1] = g;
    info->params.dir.color[2] = b;
}

// unused
void LightsN_ct(Lights* lights, u8 ambentR, u8 ambentG, u8 ambentB) {
    lights->l.a.l.col[0] = lights->l.a.l.colc[0] = ambentR;
    lights->l.a.l.col[1] = lights->l.a.l.colc[1] = ambentG;
    lights->l.a.l.col[2] = lights->l.a.l.colc[2] = ambentB;
    lights->numLights = 0;
}

/*
 * Draws every light in the provided Lights group
 */
void LightsN_disp(Lights* lights, GraphicsContext* gfxCtx) {
    Light* light;
    s32 i;

    OPEN_DISPS(gfxCtx, "../z_lights.c", 339);

    gSPNumLights(POLY_OPA_DISP++, (u32)lights->numLights);
    gSPNumLights(POLY_XLU_DISP++, (u32)lights->numLights);

    light = &lights->l.l[0];
    i = 0;

    while (i < lights->numLights) {
        gSPLight(POLY_OPA_DISP++, light, ++i);
        gSPLight(POLY_XLU_DISP++, light++, i);
    }

    // ambient light is total number of lights + 1
    gSPLight(POLY_OPA_DISP++, &lights->l.a, ++i);
    gSPLight(POLY_XLU_DISP++, &lights->l.a, i);

    CLOSE_DISPS(gfxCtx, "../z_lights.c", 352);
}

Light* LightsN_new_diffuse(Lights* lights) {
    if (lights->numLights >= 7) {
        return NULL;
    } else {
        return &lights->l.l[lights->numLights++];
    }
}

void LightsN__point_proc(Lights* lights, LightParams* params, Vec3f* vec) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 posDiff;
    f32 scale;
    Light* light;

    if (vec != NULL) {
        xDiff = params->point.x - vec->x;
        yDiff = params->point.y - vec->y;
        zDiff = params->point.z - vec->z;
        scale = params->point.radius;
        posDiff = SQ(xDiff) + SQ(yDiff) + SQ(zDiff);

        if (posDiff < SQ(scale)) {
            light = LightsN_new_diffuse(lights);

            if (light != NULL) {
                posDiff = sqrtf(posDiff);

                scale = posDiff / scale;
                scale = 1 - SQ(scale);

                light->l.col[0] = light->l.colc[0] = params->point.color[0] * scale;
                light->l.col[1] = light->l.colc[1] = params->point.color[1] * scale;
                light->l.col[2] = light->l.colc[2] = params->point.color[2] * scale;

                scale = (posDiff < 1.0f) ? 120.0f : 120.0f / posDiff;

                light->l.dir[0] = xDiff * scale;
                light->l.dir[1] = yDiff * scale;
                light->l.dir[2] = zDiff * scale;
            }
        }
    }
}

void LightsN__diffuse_proc(Lights* lights, LightParams* params, Vec3f* vec) {
    Light* light = LightsN_new_diffuse(lights);

    if (light != NULL) {
        light->l.col[0] = light->l.colc[0] = params->dir.color[0];
        light->l.col[1] = light->l.colc[1] = params->dir.color[1];
        light->l.col[2] = light->l.colc[2] = params->dir.color[2];
        light->l.dir[0] = params->dir.x;
        light->l.dir[1] = params->dir.y;
        light->l.dir[2] = params->dir.z;
    }
}

/**
 * For every light in a provided list, try to find a free slot in the provided Lights group and bind
 * a light to it. Then apply color and positional/directional info for each light
 * based on the parameters supplied by the node.
 *
 * Note: Lights in a given list can only be bound to however many free slots are
 * available in the Lights group. This is at most 7 slots for a new group, but could be less.
 */
void LightsN_list_check(Lights* lights, LightNode* listHead, Vec3f* vec) {
    LightsBindFunc bindFuncs[] = { LightsN__point_proc, LightsN__diffuse_proc, LightsN__point_proc };
    LightInfo* info;

    while (listHead != NULL) {
        info = listHead->info;
        bindFuncs[info->type](lights, &info->params, vec);
        listHead = listHead->next;
    }
}

LightNode* Light_list_buf_new(void) {
    LightNode* node;

    if (light_list_buf.numOccupied >= LIGHTS_BUFFER_SIZE) {
        return NULL;
    }

    node = &light_list_buf.buf[light_list_buf.searchIndex];

    while (node->info != NULL) {
        light_list_buf.searchIndex++;

        if (light_list_buf.searchIndex < LIGHTS_BUFFER_SIZE) {
            node++;
        } else {
            light_list_buf.searchIndex = 0;
            node = &light_list_buf.buf[0];
        }
    }

    light_list_buf.numOccupied++;

    return node;
}

BAD_RETURN(s32) Light_list_buf_delete(LightNode* light) {
    if (light != NULL) {
        light_list_buf.numOccupied--;
        light->info = NULL;
        light_list_buf.searchIndex = (light - light_list_buf.buf) / sizeof(LightNode);
    }
}

void Global_light_ct(PlayState* play, LightContext* lightCtx) {
    Global_light_list_ct(play, lightCtx);
    Global_light_ambient_set(lightCtx, 80, 80, 80);
    Global_light_fog_set(lightCtx, 0, 0, 0, ENV_FOGNEAR_MAX, ENV_ZFAR_MAX);
    bzero(&light_list_buf, sizeof(light_list_buf));
}

void Global_light_ambient_set(LightContext* lightCtx, u8 r, u8 g, u8 b) {
    lightCtx->ambientColor[0] = r;
    lightCtx->ambientColor[1] = g;
    lightCtx->ambientColor[2] = b;
}

void Global_light_fog_set(LightContext* lightCtx, u8 r, u8 g, u8 b, s16 fogNear, s16 zFar) {
    lightCtx->fogColor[0] = r;
    lightCtx->fogColor[1] = g;
    lightCtx->fogColor[2] = b;
    lightCtx->fogNear = fogNear;
    lightCtx->zFar = zFar;
}

/**
 * Allocate a new Lights group and initialize the ambient color with that provided by LightContext
 */
Lights* Global_light_read(LightContext* lightCtx, GraphicsContext* gfxCtx) {
    return new_LightsN(gfxCtx, lightCtx->ambientColor[0], lightCtx->ambientColor[1], lightCtx->ambientColor[2]);
}

void Global_light_list_ct(PlayState* play, LightContext* lightCtx) {
    lightCtx->listHead = NULL;
}

void Global_light_list_dt(PlayState* play, LightContext* lightCtx) {
    while (lightCtx->listHead != NULL) {
        Global_light_list_delete(play, lightCtx, lightCtx->listHead);
        lightCtx->listHead = lightCtx->listHead->next;
    }
}

/**
 * Insert a new light into the list pointed to by LightContext
 *
 * Note: Due to the limited number of slots in a Lights group, inserting too many lights in the
 * list may result in older entries not being bound to a Light when calling LightsN_list_check
 */
LightNode* Global_light_list_new(PlayState* play, LightContext* lightCtx, LightInfo* info) {
    LightNode* node;

    node = Light_list_buf_new();

    if (node != NULL) {
        node->info = info;
        node->prev = NULL;
        node->next = lightCtx->listHead;

        if (lightCtx->listHead != NULL) {
            lightCtx->listHead->prev = node;
        }

        lightCtx->listHead = node;
    }

    return node;
}

void Global_light_list_delete(PlayState* play, LightContext* lightCtx, LightNode* node) {
    if (node != NULL) {
        if (node->prev != NULL) {
            node->prev->next = node->next;
        } else {
            lightCtx->listHead = node->next;
        }

        if (node->next != NULL) {
            node->next->prev = node->prev;
        }

        Light_list_buf_delete(node);
    }
}

// unused
Lights* new_Lights(GraphicsContext* gfxCtx, u8 ambientR, u8 ambientG, u8 ambientB, u8 numLights, u8 r, u8 g,
                          u8 b, s8 x, s8 y, s8 z) {
    Lights* lights;
    s32 i;

    lights = GRAPH_ALLOC(gfxCtx, sizeof(Lights));

    lights->l.a.l.col[0] = lights->l.a.l.colc[0] = ambientR;
    lights->l.a.l.col[1] = lights->l.a.l.colc[1] = ambientG;
    lights->l.a.l.col[2] = lights->l.a.l.colc[2] = ambientB;
    lights->numLights = numLights;

    for (i = 0; i < numLights; i++) {
        lights->l.l[i].l.col[0] = lights->l.l[i].l.colc[0] = r;
        lights->l.l[i].l.col[1] = lights->l.l[i].l.colc[1] = g;
        lights->l.l[i].l.col[2] = lights->l.l[i].l.colc[2] = b;
        lights->l.l[i].l.dir[0] = x;
        lights->l.l[i].l.dir[1] = y;
        lights->l.l[i].l.dir[2] = z;
    }

    LightsN_disp(lights, gfxCtx);

    return lights;
}

Lights* new_LightsN(GraphicsContext* gfxCtx, u8 ambientR, u8 ambientG, u8 ambientB) {
    Lights* lights;

    lights = GRAPH_ALLOC(gfxCtx, sizeof(Lights));

    lights->l.a.l.col[0] = lights->l.a.l.colc[0] = ambientR;
    lights->l.a.l.col[1] = lights->l.a.l.colc[1] = ambientG;
    lights->l.a.l.col[2] = lights->l.a.l.colc[2] = ambientB;
    lights->numLights = 0;

    return lights;
}

void Light_list_point_draw_check(PlayState* play) {
    LightNode* node = play->lightCtx.listHead;

    while (node != NULL) {
        LightPoint* params = &node->info->params.point;

        if (node->info->type == LIGHT_POINT_GLOW) {
            Vec3f pos;
            Vec3f multDest;
            f32 cappedInvWDest;
            f32 wX;
            f32 wY;
            s32 wZ;
            s32 zBuf;

            pos.x = params->x;
            pos.y = params->y;
            pos.z = params->z;
            projection_pos_set(play, &pos, &multDest, &cappedInvWDest);
            params->drawGlow = false;
            wX = multDest.x * cappedInvWDest;
            wY = multDest.y * cappedInvWDest;

            if ((multDest.z > 1.0f) && (fabsf(wX) < 1.0f) && (fabsf(wY) < 1.0f)) {
                // Compute screen z value assuming the viewport scale and translation both have value G_MAXZ / 2
                // The multiplication by 32 follows from how the RSP microcode computes the screen z value.
                wZ = (s32)((multDest.z * cappedInvWDest) * ((G_MAXZ / 2) * 32)) + ((G_MAXZ / 2) * 32);
                // Obtain the z-buffer value for the screen pixel corresponding to the center of the glow.
                zBuf = sys_zb[(s32)((wY * -(SCREEN_HEIGHT / 2)) + (SCREEN_HEIGHT / 2))]
                               [(s32)((wX * (SCREEN_WIDTH / 2)) + (SCREEN_WIDTH / 2))]
                       << 2;
                if (1) {}
                if (1) {}

                // Compare the computed screen z value to the integer part of the z-buffer value in fixed point. If
                // it is less than the value from the z-buffer the depth test passes and the glow can draw.
                if (wZ < (zmem2z(zBuf) >> 3)) {
                    params->drawGlow = true;
                }
            }
        }
        node = node->next;
    }
}

void Light_list_point_draw(PlayState* play) {
    s32 pad;
    LightNode* node = play->lightCtx.listHead;

    OPEN_DISPS(play->state.gfxCtx, "../z_lights.c", 887);

    POLY_XLU_DISP = gfx_softsprite_prim_xlu(POLY_XLU_DISP++);
    gDPSetAlphaDither(POLY_XLU_DISP++, G_AD_NOISE);
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_MAGICSQ);
    gSPDisplayList(POLY_XLU_DISP++, gGlowCircleTextureLoadDL);

    while (node != NULL) {
        if ((node->info->type == LIGHT_POINT_GLOW)) {
            s32 pad[6];
            LightPoint* params = &node->info->params.point;

            if (params->drawGlow) {
                f32 scale = SQ(params->radius) * 0.0000026f;

                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, params->color[0], params->color[1], params->color[2], 50);
                Matrix_translate(params->x, params->y, params->z, MTXMODE_NEW);
                Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_lights.c", 918);
                gSPDisplayList(POLY_XLU_DISP++, gGlowCircleDL);
            }
        }

        node = node->next;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_lights.c", 927);
}
