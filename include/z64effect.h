#ifndef Z64EFFECT_H
#define Z64EFFECT_H

#include "color.h"
#include "romfile.h"
#include "z64light.h"
#include "z64math.h"

struct Actor;
struct GraphicsContext;
struct PlayState;

/* Effects */

#define SPARK_COUNT 3
#define BLURE_COUNT 25
#define SHIELD_PARTICLE_COUNT 3

#define TOTAL_EFFECT_COUNT SPARK_COUNT + BLURE_COUNT + SHIELD_PARTICLE_COUNT

typedef struct EffectStatus {
    /* 0x00 */ u8 active;
    /* 0x01 */ u8 unk_01;
    /* 0x02 */ u8 unk_02;
} EffectStatus; // size = 0x03

typedef struct EffectSparkElement {
    /* 0x00 */ Vec3f velocity;
    /* 0x0C */ Vec3f position;
    /* 0x18 */ Vec3s unkVelocity;
    /* 0x1E */ Vec3s unkPosition;
} EffectSparkElement; // size = 0x24

typedef struct EffectSparkInit {
    /* 0x000 */ Vec3s position;
    /* 0x008 */ s32 numElements; // "table_size"; calculated as uDiv * vDiv + 2
    /* 0x00C */ EffectSparkElement elements[32];
    /* 0x48C */ f32 speed;
    /* 0x490 */ f32 gravity;
    /* 0x494 */ u32 uDiv; // "u_div"
    /* 0x498 */ u32 vDiv; // "v_div"
    /* 0x49C */ Color_RGBA8 colorStart[4];
    /* 0x4AC */ Color_RGBA8 colorEnd[4];
    /* 0x4BC */ s32 timer;
    /* 0x4C0 */ s32 duration;
} EffectSparkInit; // size = 0x4C4

typedef struct EffectSpark {
    /* 0x000 */ Vec3s position;
    /* 0x008 */ s32 numElements; // "table_size"; calculated as uDiv * vDiv + 2
    /* 0x00C */ EffectSparkElement elements[32];
    /* 0x48C */ f32 speed;
    /* 0x490 */ f32 gravity;
    /* 0x494 */ u32 uDiv; // "u_div"
    /* 0x498 */ u32 vDiv; // "v_div"
    /* 0x49C */ Color_RGBA8 colorStart[4];
    /* 0x4AC */ Color_RGBA8 colorEnd[4];
    /* 0x4BC */ s32 timer;
    /* 0x4C0 */ s32 duration;
} EffectSpark; // size = 0x4C4

typedef struct EffectBlureElement {
    /* 0x00 */ s32 state;
    /* 0x04 */ s32 timer;
    /* 0x08 */ Vec3s p1;
    /* 0x0E */ Vec3s p2;
    /* 0x14 */ u16 flags;
} EffectBlureElement; // size = 0x18

typedef struct EffectBlureInit1 {
    /* 0x000 */ char unk_00[0x184];
    /* 0x184 */ u8 p1StartColor[4];
    /* 0x188 */ u8 p2StartColor[4];
    /* 0x18C */ u8 p1EndColor[4];
    /* 0x190 */ u8 p2EndColor[4];
    /* 0x194 */ s32 elemDuration;
    /* 0x198 */ s32 unkFlag;
    /* 0x19C */ s32 calcMode;
} EffectBlureInit1; // size = 0x1A0

typedef struct EffectBlureInit2 {
    /* 0x00 */ s32 calcMode;
    /* 0x04 */ u16 flags;
    /* 0x06 */ s16 addAngleChange;
    /* 0x08 */ u8 p1StartColor[4];
    /* 0x0C */ u8 p2StartColor[4];
    /* 0x10 */ u8 p1EndColor[4];
    /* 0x14 */ u8 p2EndColor[4];
    /* 0x18 */ u8 elemDuration;
    /* 0x19 */ u8 unkFlag;
    /* 0x1A */ u8 drawMode; // 0: simple; 1: simple with alt colors; 2+: smooth
    /* 0x1B */ u8 mode4Param;
    /* 0x1C */ Color_RGBA8 altPrimColor; // used with drawMode 1
    /* 0x20 */ Color_RGBA8 altEnvColor; // used with drawMode 1
} EffectBlureInit2; // size = 0x24

typedef struct EffectBlure {
    /* 0x000 */ EffectBlureElement elements[16];
    /* 0x180 */ s32 calcMode;
    /* 0x184 */ f32 mode4Param;
    /* 0x188 */ u16 flags;
    /* 0x18A */ s16 addAngleChange;
    /* 0x18C */ s16 addAngle;
    /* 0x18E */ u8 p1StartColor[4];
    /* 0x192 */ u8 p2StartColor[4];
    /* 0x196 */ u8 p1EndColor[4];
    /* 0x19A */ u8 p2EndColor[4];
    /* 0x19E */ u8 numElements; // "now_edge_num"
    /* 0x19F */ u8 elemDuration;
    /* 0x1A0 */ u8 unkFlag;
    /* 0x1A1 */ u8 drawMode; // 0: simple; 1: simple with alt colors; 2+: smooth
    /* 0x1A2 */ Color_RGBA8 altPrimColor; // used with drawMode 1
    /* 0x1A6 */ Color_RGBA8 altEnvColor; // used with drawMode 1
} EffectBlure; // size = 0x1AC

typedef struct EffectShieldParticleElement {
    /* 0x00 */ f32 initialSpeed;
    /* 0x04 */ f32 endXChange;
    /* 0x08 */ f32 endX;
    /* 0x0C */ f32 startXChange;
    /* 0x10 */ f32 startX;
    /* 0x14 */ s16 yaw;
    /* 0x16 */ s16 pitch;
} EffectShieldParticleElement; // size = 0x18

typedef struct EffectShieldParticleInit {
    /* 0x00 */ u8 numElements;
    /* 0x02 */ Vec3s position;
    /* 0x08 */ Color_RGBA8 primColorStart;
    /* 0x0C */ Color_RGBA8 envColorStart;
    /* 0x10 */ Color_RGBA8 primColorMid;
    /* 0x14 */ Color_RGBA8 envColorMid;
    /* 0x18 */ Color_RGBA8 primColorEnd;
    /* 0x1C */ Color_RGBA8 envColorEnd;
    /* 0x20 */ f32 deceleration;
    /* 0x24 */ f32 maxInitialSpeed;
    /* 0x28 */ f32 lengthCutoff;
    /* 0x2C */ u8 duration;
    /* 0x2E */ LightPoint lightPoint;
    /* 0x3C */ s32 lightDecay; // halves light radius every frame when set to 1
} EffectShieldParticleInit; // size = 0x40

typedef struct EffectShieldParticle {
    /* 0x000 */ EffectShieldParticleElement elements[16];
    /* 0x180 */ u8 numElements;
    /* 0x182 */ Vec3s position;
    /* 0x188 */ Color_RGBA8 primColorStart;
    /* 0x18C */ Color_RGBA8 envColorStart;
    /* 0x190 */ Color_RGBA8 primColorMid;
    /* 0x194 */ Color_RGBA8 envColorMid;
    /* 0x198 */ Color_RGBA8 primColorEnd;
    /* 0x19C */ Color_RGBA8 envColorEnd;
    /* 0x1A0 */ f32 deceleration;
    /* 0x1A4 */ char unk_1A4[0x04];
    /* 0x1A8 */ f32 maxInitialSpeed;
    /* 0x1AC */ f32 lengthCutoff;
    /* 0x1B0 */ u8 duration;
    /* 0x1B1 */ u8 timer;
    /* 0x1B2 */ LightInfo lightInfo;
    /* 0x1C0 */ LightNode* lightNode;
    /* 0x1C4 */ s32 lightDecay; // halves light radius every frame when set to 1
} EffectShieldParticle; // size = 0x1C8

typedef struct EffectContext {
    /* 0x0000 */ struct PlayState* play;
    struct {
        EffectStatus status;
        EffectSpark effect;
    } /* 0x0004 */ sparks[SPARK_COUNT];
    struct {
        EffectStatus status;
        EffectBlure effect;
    } /* 0x0E5C */ blures[BLURE_COUNT];
    struct {
        EffectStatus status;
        EffectShieldParticle effect;
    } /* 0x388C */ shieldParticles[SHIELD_PARTICLE_COUNT];
} EffectContext; // size = 0x3DF0

typedef struct EffectInfo {
    /* 0x00 */ u32 size;
    /* 0x04 */ void (*init)(void* effect, void* initParams);
    /* 0x08 */ void (*destroy)(void* effect);
    /* 0x0C */ s32 (*update)(void* effect);
    /* 0x10 */ void (*draw)(void* effect, struct GraphicsContext* gfxCtx);
} EffectInfo; // size = 0x14

typedef enum EffectType {
    /* 0x00 */ EFFECT_SPARK,
    /* 0x01 */ EFFECT_BLURE1,
    /* 0x02 */ EFFECT_BLURE2,
    /* 0x03 */ EFFECT_SHIELD_PARTICLE
} EffectType;

/* Effect Soft Sprites */

struct EffectSs;

typedef u32 (*EffectSsInitFunc)(struct PlayState* play, u32 index, struct EffectSs* effectSs, void* initParams);
typedef void (*EffectSsUpdateFunc)(struct PlayState* play, u32 index, struct EffectSs* effectSs);
typedef void (*EffectSsDrawFunc)(struct PlayState* play, u32 index, struct EffectSs* effectSs);

typedef struct EffectSsProfile {
    /* 0x00 */ u32 type;
    /* 0x04 */ EffectSsInitFunc init;
} EffectSsProfile; // size = 0x08

typedef struct EffectSsOverlay {
    /* 0x00 */ RomFile file;
    /* 0x08 */ void* vramStart;
    /* 0x0C */ void* vramEnd;
    /* 0x10 */ void* loadedRamAddr;
    /* 0x14 */ EffectSsProfile* profile;
    /* 0x18 */ u8 unk_18;
} EffectSsOverlay; // size = 0x1C

typedef struct EffectSs {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ EffectSsUpdateFunc update;
    /* 0x28 */ EffectSsDrawFunc draw;
    /* 0x2C */ Vec3f vec; // usage specific per effect
    /* 0x38 */ void* gfx; // mostly used for display lists, sometimes textures
    /* 0x3C */ struct Actor* actor; // interfacing actor, usually the actor that spawned the effect
    /* 0x40 */ s16 regs[13]; // specific per effect
    /* 0x5A */ u16 flags;
    /* 0x5C */ s16 life; // -1 means this entry is free
    /* 0x5E */ u8 priority; // Lower value means higher priority
    /* 0x5F */ u8 type;
} EffectSs; // size = 0x60

typedef struct EffectSsInfo {
    /* 0x00 */ EffectSs* table; // "data_table"
    /* 0x04 */ s32 searchStartIndex;
    /* 0x08 */ s32 tableSize;
} EffectSsInfo; // size = 0x0C

/* G Effect Regs */

#define rgTexIdx regs[0]
#define rgScale regs[1]
#define rgTexIdxStep regs[2]
#define rgPrimColorR regs[3]
#define rgPrimColorG regs[4]
#define rgPrimColorB regs[5]
#define rgPrimColorA regs[6]
#define rgEnvColorR regs[7]
#define rgEnvColorG regs[8]
#define rgEnvColorB regs[9]
#define rgEnvColorA regs[10]
#define rgObjectSlot regs[11]

#define DEFINE_EFFECT_SS(_0, enum) enum,
#define DEFINE_EFFECT_SS_UNSET(enum) enum,

typedef enum EffectSsType {
    #include "tables/effect_ss_table.h"
    /* 0x25 */ EFFECT_SS_TYPE_MAX // originally "EFFECT_SS2_TYPE_LAST_LABEL"
} EffectSsType;

#undef DEFINE_EFFECT_SS
#undef DEFINE_EFFECT_SS_UNSET

void EffectBlure_edge_add(EffectBlure* this, Vec3f* p1, Vec3f* p2);
void EffectBlure_space_add(EffectBlure* this);
void EffectBlureInfo_ct(void* thisx, void* initParamsx);
void EffectBlureInfo_v1_ct(void* thisx, void* initParamsx);
void EffectBlureInfo_dt(void* thisx);
s32 EffectBlureInfo_proc(void* thisx);
void EffectBlureInfo_disp(void* thisx, struct GraphicsContext* gfxCtx);
void EffectShieldParticle_ct(void* thisx, void* initParamsx);
void EffectShieldParticle_dt(void* thisx);
s32 EffectShieldParticle_proc(void* thisx);
void EffectShieldParticle_disp(void* thisx, struct GraphicsContext* gfxCtx);
void EffectSparkInfo_ct(void* thisx, void* initParamsx);
void EffectSparkInfo_dt(void* thisx);
s32 EffectSparkInfo_proc(void* thisx);
void EffectSparkInfo_disp(void* thisx, struct GraphicsContext* gfxCtx);
void Eff_Set_Fog2(struct PlayState* play, Color_RGBA8* color, s16 arg2, s16 arg3);
void Eff_Set_Fog3(struct PlayState* play, Color_RGBA8* color, s16 arg2, s16 arg3);
void Eff_Off_Fog(struct PlayState* play);
void Eff_Set_Fog2_xlu(struct PlayState* play, Color_RGBA8* color, s16 arg2, s16 arg3);
void Eff_Set_Fog3_xlu(struct PlayState* play, Color_RGBA8* color, s16 arg2, s16 arg3);
void Eff_Off_Fog_xlu(struct PlayState* play);
struct PlayState* Effect_GetGamePointer(void);
void* Effect_GetEffectMemoryPointer(s32 index);
void EffectInit(struct PlayState* play);
void EffectAdd(struct PlayState* play, s32* pIndex, s32 type, u8 arg3, u8 arg4, void* initParams);
void EffectDisp(struct GraphicsContext* gfxCtx);
void EffectProc(struct PlayState* play);
void EffectFreeIndex(struct PlayState* play, s32 index);
void EffectFreeAllIndex(struct PlayState* play);
void EffectSoftSprite_ct(struct PlayState* play, s32 tableSize);
void EffectSoftSprite_dt(struct PlayState* play);
void EffectSoftSprite2_ElementDestructClear(EffectSs* effectSs);
void EffectSoftSprite2_ElementClear(EffectSs* effectSs);
void EffectSoftSprite2_add(struct PlayState* play, EffectSs* effectSs);
void EffectSoftSprite2_makeEffect(struct PlayState* play, s32 type, s32 priority, void* initParams);
void EffectSoftSprite_proc(struct PlayState* play);
void EffectSoftSprite_disp(struct PlayState* play);
s16 Effect_SS_Uty_short_interpolation(s16 a, s16 b, s32 weightInv);
s16 Effect_SS_Uty_short_interpolation_t01(s16 a, s16 b, f32 weight);
u8 Effect_SS_Uty_uc_interpolation_t01(u8 a, u8 b, f32 weight);
void effect_disp_mode_sub(struct PlayState* play, EffectSs* this, void* texture);
void Effect_SS_Dust_add_general(struct PlayState* play, u16 drawFlags, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                        Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life,
                        u8 updateMode);
void Effect_SS_Dust_sc_cl_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void Effect_SS_Dust_sc_cl_li_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void Effect_SS_Dust_sc_cl_co_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life);
void Effect_SS_Dust_sc_cl_co_li_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life);
void Effect_SS_Dust_sc_cl_co_nofog_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep, s16 life);
void Effect_SS_Dust_sc_cl_ct2(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void Effect_SS_Dust_sc_cl_li_ct2(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void Effect_SS_Dust_ct_direct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void Effect_SS_Dust_ct_li_direct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void Effect_SS_Dust_sc_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep);
void Effect_SS_Dust_sc_li_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep);
void Effect_SS_Dust_sc_co_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep,
                   s16 life);
void Effect_SS_Dust_sc_co_li_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep,
                   s16 life);
void Effect_SS_Dust_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor);
void Effect_SS_Dust_li_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* primColor,
                   Color_RGBA8* envColor);
void Effect_SS_Dust_spread20(struct PlayState* play, f32 randScale, Vec3f* srcPos);
void Effect_SS_Dust_li_spread20(struct PlayState* play, f32 randScale, Vec3f* srcPos);
void Effect_SS_KiraKira_ct_direct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void Effect_SS_KiraKira_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                                 Color_RGBA8* primColor, Color_RGBA8* envColor);
void Effect_SS_KiraKira_sc_ct_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                                     Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s32 life);
void Effect_SS_KiraKira_soul_sc_ct_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                                   Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s32 life);
void Effect_SS_Bomb_ct_direct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void Effect_SS_Bomb2_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void Effect_SS_Bomb2_2_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale,
                                s16 scaleStep);
void Effect_SS_Blast_sc_cl_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, Color_RGBA8* innerColor,
                         Color_RGBA8* outerColor, s16 scale, s16 scaleStep, s16 scaleStepDecay, s16 life);
void Effect_SS_Blast_sc_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                                               s16 scale, s16 scaleStep, s16 life);
void Effect_SS_Blast_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                                          Color_RGBA8* innerColor, Color_RGBA8* outerColor, s16 life);
void Effect_SS_Blast_ct_direct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void Effect_SS_Spk_sc_cl_ct(struct PlayState* play, struct Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                             Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void Effect_SS_Spk_sc_cl_ct2(struct PlayState* play, struct Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                               Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 scaleStep);
void Effect_SS_Spk_ct_direct(struct PlayState* play, struct Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel);
void Effect_SS_Spk_sc_ct(struct PlayState* play, struct Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                                 s16 scale, s16 scaleStep);
void Effect_SS_Spk_ct(struct PlayState* play, struct Actor* actor, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                             Color_RGBA8* primColor, Color_RGBA8* envColor);
void Effect_SS_Dfire_ct_s(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep,
                         s16 alpha, s16 fadeDelay, s32 life);
void Effect_SS_Dfire_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 alpha,
                                   s16 fadeDelay);
void Effect_SS_Bubble_ct(struct PlayState* play, Vec3f* pos, f32 yPosOffset, f32 yPosRandScale, f32 xzPosRandScale,
                          f32 scale);
void Effect_SS_G_Ripple_ct2(struct PlayState* play, Vec3f* pos, s16 radius, s16 radiusMax, s16 life);
void Effect_SS_G_Splash_sc_cl_ct(struct PlayState* play, Vec3f* pos, Color_RGBA8* primColor, Color_RGBA8* envColor, s16 type,
                           s16 scale);
void Effect_SS_G_Magma_ct(struct PlayState* play, Vec3f* pos);
void Effect_SS_G_Fire_ct(struct PlayState* play, Vec3f* pos);
void Effect_SS_Lightning_sc_cl_co_ct(struct PlayState* play, Vec3f* pos, Color_RGBA8* primColor, Color_RGBA8* envColor,
                             s16 scale, s16 yaw, s16 life, s16 numBolts);
void Effect_SS_Dt_Bubble_sc_co_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale,
                                        s16 life, s16 colorProfile, s16 randXZ);
void Effect_SS_Dt_Bubble_sc_cl_co_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel,
                                       Color_RGBA8* primColor, Color_RGBA8* envColor, s16 scale, s16 life, s16 randXZ);
void Effect_Hahen_ct3(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 unused, s16 scale,
                         s16 objId, s16 life, Gfx* dList);
void Effect_Hahen_Kakusan_ct3(struct PlayState* play, Vec3f* pos, f32 burstScale, s16 unused, s16 scale,
                              s16 randScaleRange, s16 count, s16 objId, s16 life, Gfx* dList);
void Effect_SS_Stick_ct(struct PlayState* play, Vec3f* pos, s16 yaw);
void Effect_SS_Sibuki_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 moveDelay,
                          s16 direction, s16 scale);
void DamageEffectSibukiSet(struct PlayState* play, Vec3f* pos);
void Effect_SS_Sibuki2_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale);
void Effect_SS_Magma2_ct(struct PlayState* play, Vec3f* pos, Color_RGBA8* primColor, Color_RGBA8* envColor,
                           s16 updateRate, s16 drawMode, s16 scale);
void Effect_SS_Stone1_ct(struct PlayState* play, Vec3f* pos, s32 arg2);
void Effect_SS_HitMark_ct_general(struct PlayState* play, s32 type, s16 scale, Vec3f* pos);
void Effect_SS_HitMark_ct(struct PlayState* play, s32 type, Vec3f* pos);
void Effect_SS_HitMark_scl_ct(struct PlayState* play, s32 type, s16 scale, Vec3f* pos);
void Effect_fhg_flash_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale,
                                     u8 param);
void Effect_fhg_flash_mini_ct(struct PlayState* play, struct Actor* actor, Vec3f* pos, s16 scale, u8 param);
void Effect_k_fire_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scaleMax, u8 type);
void Effect_Ss_Solder_Srch_Ball_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 unused,
                                  s16* linkDetected);
void Effect_Kakera_ct2(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* arg3, s16 gravity, s16 arg5,
                          s16 arg6, s16 arg7, s16 arg8, s16 scale, s16 arg10, s16 arg11, s32 life, s16 colorIdx,
                          s16 objId, Gfx* dList);
void Effect_SS_Ice_Piece_ct(struct PlayState* play, Vec3f* pos, f32 scale, Vec3f* velocity, Vec3f* accel, s32 life);
void ice_piece_effect_set(struct PlayState* play, Vec3f* refPos, f32 scale);
void Effect_En_Ice_ct0(struct PlayState* play, struct Actor* actor, Vec3f* pos, s16 primR, s16 primG,
                                    s16 primB, s16 primA, s16 envR, s16 envG, s16 envB, f32 scale);
void Effect_En_Ice_ct1(struct PlayState* play, struct Actor* actor, Vec3s* pos, s16 primR, s16 primG,
                                    s16 primB, s16 primA, s16 envR, s16 envG, s16 envB, f32 scale);
void Effect_SS_Ice_Piece2_ct(struct PlayState* play, Vec3f* pos, f32 scale, Vec3f* velocity, Vec3f* accel,
                         Color_RGBA8* primColor, Color_RGBA8* envColor, s32 life);
void Effect_SS_Fire_Tail_ct(struct PlayState* play, struct Actor* actor, Vec3f* pos, f32 scale, Vec3f* arg4, s16 arg5,
                            Color_RGBA8* primColor, Color_RGBA8* envColor, s16 type, s16 bodyPart, s32 life);
void fire_tail_effect_set3(struct PlayState* play, struct Actor* actor, Vec3f* pos, f32 arg3, s16 bodyPart,
                                 f32 colorIntensity);
void fire_tail_effect_set2(struct PlayState* play, f32 scale, s16 bodyPart, f32 colorIntensity);
void Effect_En_Fire_ct(struct PlayState* play, struct Actor* actor, Vec3f* pos, s16 scale, s16 arg4, s16 flags,
                               s16 bodyPart);
void Effect_En_Fire_ct_s(struct PlayState* play, struct Actor* actor, Vec3s* pos, s16 scale, s16 arg4, s16 flags,
                               s16 bodyPart);
void Effect_SS_Extra_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scoreIdx);
void Effect_Fcircle_ct(struct PlayState* play, struct Actor* actor, Vec3f* pos, s16 radius, s16 height);
void _Effect_SS_Db_ct(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep,
                          s16 primR, s16 primG, s16 primB, s16 primA, s16 envR, s16 envG, s16 envB, s16 unused,
                          s32 arg14, s16 playSfx);
void Effect_SS_Dd_ct0(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep,
                          s16 primR, s16 primG, s16 primB, s16 alpha, s16 envR, s16 envG, s16 envB, s16 alphaStep,
                          s32 life);
void Effect_SS_Dd_ct1(struct PlayState* play, Vec3f* pos, s16 scale, s16 scaleStep, f32 randPosScale,
                                    s32 randIter, s32 life);
void Effect_SS_Ds_ct0(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep,
                          s16 alpha, s32 life);
void Effect_SS_Ds_ct1(struct PlayState* play, Vec3f* pos, s16 scale, s16 scaleStep, s16 alpha, s32 life);
void Effect_sound_ct0(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u16 sfxId,
                             s16 lowerPriority, s16 repeatMode, s32 life);
void Effect_sound_ct(struct PlayState* play, Vec3f* pos, u16 sfxId, s16 lowerPriority, s16 repeatMode,
                                       s32 life);
void Effect_SS_Ice_Smoke_make(struct PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale);

#endif
