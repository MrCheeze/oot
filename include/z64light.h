#ifndef Z64LIGHT_H
#define Z64LIGHT_H

#include "ultra64.h"
#include "ultra64/gbi.h"
#include "z64math.h"
#include "color.h"

struct GraphicsContext;
struct PlayState;

typedef struct LightPoint {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
    /* 0x6 */ u8 color[3];
    /* 0x9 */ u8 drawGlow;
    /* 0xA */ s16 radius;
} LightPoint; // size = 0xC

typedef struct LightDirectional {
    /* 0x0 */ s8 x;
    /* 0x1 */ s8 y;
    /* 0x2 */ s8 z;
    /* 0x3 */ u8 color[3];
} LightDirectional; // size = 0x6

typedef union LightParams {
    LightPoint point;
    LightDirectional dir;
} LightParams; // size = 0xC

typedef struct LightInfo {
    /* 0x0 */ u8 type;
    /* 0x2 */ LightParams params;
} LightInfo; // size = 0xE

typedef struct Lights {
    /* 0x00 */ u8 numLights;
    /* 0x08 */ Lightsn l;
} Lights; // size = 0x80

typedef struct LightNode {
    /* 0x0 */ LightInfo* info;
    /* 0x4 */ struct LightNode* prev;
    /* 0x8 */ struct LightNode* next;
} LightNode; // size = 0xC

#define ENV_FOGNEAR_MAX 996
#define ENV_ZFAR_MAX 12800

typedef struct LightContext {
    /* 0x0 */ LightNode* listHead;
    /* 0x4 */ u8 ambientColor[3];
    /* 0x7 */ u8 fogColor[3];
    /* 0xA */ s16 fogNear; // how close until fog starts taking effect. range 0 - ENV_FOGNEAR_MAX
    /* 0xC */ s16 zFar; // draw distance. range 0 - ENV_ZFAR_MAX
} LightContext; // size = 0x10

typedef enum LightType {
    /* 0x00 */ LIGHT_POINT_NOGLOW,
    /* 0x01 */ LIGHT_DIRECTIONAL,
    /* 0x02 */ LIGHT_POINT_GLOW
} LightType;

typedef void (*LightsBindFunc)(Lights* lights, LightParams* params, Vec3f* vec);

void point_data_set(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius, s32 type);
void Light_point_ct(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius);
void Light_point2_ct(LightInfo* info, s16 x, s16 y, s16 z, u8 r, u8 g, u8 b, s16 radius);
void Light_point_color_set(LightInfo* info, u8 r, u8 g, u8 b, s16 radius);
void Light_diffuse_ct(LightInfo* info, s8 x, s8 y, s8 z, u8 r, u8 g, u8 b);
void LightsN_ct(Lights* lights, u8 ambentR, u8 ambentG, u8 ambentB);
void LightsN_disp(Lights* lights, struct GraphicsContext* gfxCtx);
void LightsN_list_check(Lights* lights, LightNode* listHead, Vec3f* vec);
void Global_light_ct(struct PlayState* play, LightContext* lightCtx);
void Global_light_ambient_set(LightContext* lightCtx, u8 r, u8 g, u8 b);
void Global_light_fog_set(LightContext* lightCtx, u8 r, u8 g, u8 b, s16 fogNear, s16 zFar);
Lights* Global_light_read(LightContext* lightCtx, struct GraphicsContext* gfxCtx);
void Global_light_list_ct(struct PlayState* play, LightContext* lightCtx);
void Global_light_list_dt(struct PlayState* play, LightContext* lightCtx);
LightNode* Global_light_list_new(struct PlayState* play, LightContext* lightCtx, LightInfo* info);
void Global_light_list_delete(struct PlayState* play, LightContext* lightCtx, LightNode* node);
Lights* new_Lights(struct GraphicsContext* gfxCtx, u8 ambientR, u8 ambientG, u8 ambientB, u8 numLights, u8 r, u8 g,
                          u8 b, s8 x, s8 y, s8 z);
Lights* new_LightsN(struct GraphicsContext* gfxCtx, u8 ambientR, u8 ambientG, u8 ambientB);
void Light_list_point_draw_check(struct PlayState* play);
void Light_list_point_draw(struct PlayState* play);

#endif
