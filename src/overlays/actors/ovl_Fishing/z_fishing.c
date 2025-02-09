/*
 * File: z_fishing.c
 * Overlay: ovl_Fishing
 * Description: Fishing Pond Elements (Owner, Fish, Props, Effects...)
 */

#include "z_fishing.h"

#include "overlays/actors/ovl_En_Kanban/z_en_kanban.h"
#include "assets/objects/object_fish/object_fish.h"
#include "libc64/math64.h"
#include "attributes.h"
#include "controller.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "letterbox.h"
#include "rand.h"
#include "regs.h"
#include "rumble.h"
#include "segmented_address.h"
#include "seqcmd.h"
#include "sequence.h"
#include "sfx.h"
#include "sys_math.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "versions.h"
#include "z64audio.h"
#include "z64play.h"
#include "z64player.h"
#include "z64skin_matrix.h"
#include "z_lib.h"
#if PLATFORM_N64
#include "cic6105.h"
#endif

#pragma increment_block_number "gc-eu:170 gc-eu-mq:170 gc-jp:170 gc-jp-ce:170 gc-jp-mq:170 gc-us:170 gc-us-mq:170" \
                               "ntsc-1.0:121 ntsc-1.1:121 ntsc-1.2:121 pal-1.0:121 pal-1.1:121 hiratsu3:170"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

#define WATER_SURFACE_Y(play) play->colCtx.colHeader->waterBoxes->ySurface

#if DEBUG_FEATURES
#define KREG_DEBUG(i) KREG(i)
#else
#define KREG_DEBUG(i) 0
#endif

void Fishing_actor_ct(Actor* thisx, PlayState* play2);
void Fishing_actor_dt(Actor* thisx, PlayState* play2);
void Fishing_actor_move(Actor* thisx, PlayState* play2);
void bm_move(Actor* thisx, PlayState* play2);
void Fishing_actor_draw(Actor* thisx, PlayState* play);
void bm_draw(Actor* thisx, PlayState* play);

typedef struct FishingFishInit {
    /* 0x00 */ u8 isLoach;
    /* 0x02 */ Vec3s pos;
    /* 0x08 */ u8 baseLength;
    /* 0x0C */ f32 perception;
} FishingFishInit; // size = 0x10

typedef enum FishingEffectType {
    /* 0x00 */ FS_EFF_NONE,
    /* 0x01 */ FS_EFF_RIPPLE,
    /* 0x02 */ FS_EFF_DUST_SPLASH,
    /* 0x03 */ FS_EFF_WATER_DUST,
    /* 0x04 */ FS_EFF_BUBBLE,
    /* 0x05 */ FS_EFF_RAIN_DROP,
    /* 0x06 */ FS_EFF_OWNER_HAT,
    /* 0x07 */ FS_EFF_RAIN_RIPPLE,
    /* 0x08 */ FS_EFF_RAIN_SPLASH
} FishingEffectType;

#define FISHING_EFFECT_COUNT 130

typedef struct FishingEffect {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f vel;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ u8 type;
    /* 0x25 */ u8 timer;
    /* 0x26 */ char unk_26[0x04];
    /* 0x2A */ s16 alpha;
    /* 0x2C */ s16 state;
    /* 0x2E */ s16 alphaMax;
    /* 0x30 */ f32 scale;
    /* 0x34 */ Vec3f rot;
} FishingEffect; // size = 0x40

#define POND_PROP_COUNT 140

typedef enum FishingPropType {
    /* 0x00 */ FS_PROP_NONE,
    /* 0x01 */ FS_PROP_REED,
    /* 0x02 */ FS_PROP_LILY_PAD,
    /* 0x03 */ FS_PROP_ROCK,
    /* 0x04 */ FS_PROP_WOOD_POST,
    /* 0x23 */ FS_PROP_INIT_STOP = 0x23
} FishingPropType;

typedef struct FishingPropInit {
    /* 0x00 */ u8 type;
    /* 0x02 */ Vec3s pos;
} FishingPropInit; // size = 0x08

typedef struct FishingProp {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ f32 rotX;
    /* 0x10 */ f32 rotY;
    /* 0x14 */ f32 reedAngle;
    /* 0x18 */ Vec3f projectedPos;
    /* 0x24 */ f32 scale;
    /* 0x28 */ s16 lilyPadAngle;
    /* 0x2C */ f32 lilyPadOffset;
    /* 0x30 */ u8 type;
    /* 0x32 */ s16 timer;
    /* 0x34 */ u8 shouldDraw;
    /* 0x38 */ f32 drawDistance;
} FishingProp; // size = 0x3C

typedef enum FishingGroupFishType {
    /* 0x00 */ FS_GROUP_FISH_NONE,
    /* 0x01 */ FS_GROUP_FISH_NORMAL
} FishingGroupFishType;

#define GROUP_FISH_COUNT 60

typedef struct FishingGroupFish {
    /* 0x00 */ u8 type;
    /* 0x02 */ s16 timer;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f homePos;
    /* 0x1C */ Vec3f projectedPos;
    /* 0x28 */ f32 velY;
    /* 0x2C */ f32 scaleX;
    /* 0x30 */ f32 unk_30;
    /* 0x34 */ f32 unk_34;
    /* 0x38 */ f32 unk_38;
    /* 0x3C */ s16 unk_3C;
    /* 0x3E */ s16 unk_3E;
    /* 0x40 */ s16 unk_40;
    /* 0x42 */ s16 unk_42;
    /* 0x44 */ u8 shouldDraw;
} FishingGroupFish; // size = 0x48

typedef enum FishingLureTypes {
    /* 0x00 */ FS_LURE_STOCK,
    /* 0x01 */ FS_LURE_UNK, // hinted at with an "== 1"
    /* 0x02 */ FS_LURE_SINKING
} FishingLureTypes;

#define LINE_SEG_COUNT 200
#define SINKING_LURE_SEG_COUNT 20

ActorProfile Fishing_Profile = {
    /**/ ACTOR_FISHING,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_FISH,
    /**/ sizeof(Fishing),
    /**/ Fishing_actor_ct,
    /**/ Fishing_actor_dt,
    /**/ Fishing_actor_move,
    /**/ Fishing_actor_draw,
};

static f32 rain_S = 0.0f;

static u8 rain = 0;

static f32 add_light = 0.0f;

static Vec3f rain_se_pos = { 0.0f, 0.0f, 0.0f };

static f32 rain_se_p = 0.0f;

static u8 worm_set_P = 0;

static f32 my_record = 0.0f;

static u8 rod_disp = true;

static u16 FishRangeForMessage = 0;

static u8 fs_message_time = 0;

static s32 fishing_time = 0;

static s16 head_check = 0;

typedef enum FishingOwnerHair {
    /* 0x00 */ FS_OWNER_BALD,
    /* 0x01 */ FS_OWNER_CAPPED,
    /* 0x02 */ FS_OWNER_HAIR
} FishingOwnerHair;

static u8 bm_head_no = FS_OWNER_BALD;
static u8 lure_with_head = false; // hat is on fishing hook
static u8 lure_with_head_off = false;   // hat is sinking into pond.

static s16 lure_mode = 0;

static Vec3f fish_mouse_p = { 500.0f, 500.0f, 0.0f };

static u8 line_tensyon = 0;

static f32 rod_curvY = 0.0f;
static f32 rod_curvX = 0.0f;
static f32 rod_curvY_spd = 0.0f;
static f32 rod_curvX_spd = 0.0f;
static f32 rod_curvX2 = 0.0f;
static f32 rod_curvX3 = 0.0f;
static f32 rod_base_curvX = 0.0f;

static s16 old_stick_x = 0;
static s16 old_stick_y = 0;

static u8 demo_mode = 0;
static u8 door_check_time = 0;
static u8 lure_hirotta = false;

static ColliderJntSphElementInit FsIshiOcInfoJntSphElemData[12] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
};

static ColliderJntSphInit FsIshiOcInfoJntSphData = {
    {
        COL_MATERIAL_NONE,
        AT_TYPE_ENEMY,
        AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    12,
    FsIshiOcInfoJntSphElemData,
};

static f32 minnow_panic = 0.0f;

static Vec3f zero = { 0.0f, 0.0f, 0.0f };
static Vec3f zero3 = { 0.0f, 0.0f, 2000.0f };

static Fishing* master;
static u8 CIC_ERROR;
static u8 AGE;
static u8 mad_water;
static u8 wether;
static f32 fish_record;
static u8 my_fish_kind;
static u8 play_count; // increments for each purchased play. effects weather
static u8 use_lure;
static s16 fish_fight_time;
static u8 seken_mes_ct;
static u16 fish_catch_count;
static u16 fs_message_no;
static s8 camera_zoom_switch;
static Vec3f bm_head_pos;
static Vec3s cap_ang;
static u8 lure_reel_ready; // a small delay between the lure hitting the water, and being able to reel.
static s16 vib_time;
static s16 lure_bgm_time;
static Fishing* hit_fish_actor;
static s16 fishing_game_mode;
static s16 lure_count; // AND'd for various effects/checks
static s16 lure_time;
static s16 lure_cast_time;
static s16 lure_cast_wait; // used for the initial line casting
static u8 lure_type;
static Vec3f lure_pos;
static Vec3f lure_head_pos;
static Vec3f lure_ang;
static Vec3f lure_spd;
static Vec3f lure_acc;
static f32 lure_vib_ya; // lure type 1 is programmed to change this.
static f32 lure_offset_z;
static f32 lure_dog_swing_X;
static f32 lure_ang_x_ad;
static s8 lure_in_water;
static s16 lure_hook_power; // holding A+Down to keep the fish on line
static u8 lure_search_p;
static f32 hit_power;
static u8 lure_action;
static s16 lure_hook_time;
static u8 lure_hook_up;
static Vec3f lure_act_vec;
static f32 lure_act_ang_Y;
static f32 lure_act_speed;
static s16 lure_ang_Y_s;
static f32 lure_dog_walk_way; // +/-1.0f
static f32 line_cap; // 200 represents the full spool.
static f32 line_G;
static f32 line_scale;
static s16 lure_dog_wait_time;
static f32 lure_hosei_s;
static Vec3f line_base_pos;
static Vec3f line_pos[LINE_SEG_COUNT];
static Vec3f line_angle[LINE_SEG_COUNT];
static Vec3f line_spd[LINE_SEG_COUNT];
static Vec3f hook_pos[2];
static f32 hook_angle_Y[2];
static u8 rod_hooking_impact; // brief timer for bending rod when line is snapped
static Vec3f worm_shape_pos[SINKING_LURE_SEG_COUNT];
static s16 worm_eat;
static f32 float_dammy;
static Vec3f demo_eye;
static Vec3f demo_way;
static s16 demo_camera_no;
static f32 demo_xa;
static f32 demo_morf;
static f32 demo_eye_xp;
static Vec3f lure_hirotta_pos;
static f32 lure_hirotta_pos_y2; // the lure going to its mark when being held up.
static s32 rnd20;
static s32 rnd21;
static s32 rnd22;
static FishingProp fishing_obj[POND_PROP_COUNT];
static FishingGroupFish minnow[GROUP_FISH_COUNT];
static f32 minnow_home_angle_Y1;
static f32 minnow_home_angle_Y2;
static f32 minnow_home_angle_Y3;
static FishingEffect fishing_eff[FISHING_EFFECT_COUNT];

static void CollisionCheck_pos_set(s32 index, ColliderJntSph* collider, Vec3f* pos, f32 scale) {
    collider->elements[index].dim.worldSphere.center.x = pos->x;
    collider->elements[index].dim.worldSphere.center.y = pos->y;
    collider->elements[index].dim.worldSphere.center.z = pos->z;
    collider->elements[index].dim.worldSphere.radius =
        collider->elements[index].dim.modelSphere.radius * collider->elements[index].dim.scale * scale * 1.6f;
}

static void init_stage_rnd(s32 seed0, s32 seed1, s32 seed2) {
    rnd20 = seed0;
    rnd21 = seed1;
    rnd22 = seed2;
}

static f32 stage_rnd(void) {
    f32 rand;

    // Wichmann-Hill algorithm
    rnd20 = (rnd20 * 171) % 30269;
    rnd21 = (rnd21 * 172) % 30307;
    rnd22 = (rnd22 * 170) % 30323;

    rand = (rnd20 / 30269.0f) + (rnd21 / 30307.0f) + (rnd22 / 30323.0f);
    while (rand >= 1.0f) {
        rand -= 1.0f;
    }

    return fabsf(rand);
}

s16 adds2(s16* pValue, s16 target, s16 scale, s16 step) {
    s16 stepSize;
    s16 diff;

    diff = target - *pValue;
    stepSize = diff / scale;

    if (stepSize > step) {
        stepSize = step;
    }

    if (stepSize < -step) {
        stepSize = -step;
    }

    *pValue += stepSize;

    return stepSize;
}

void fs_hamon_ct(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, f32 scale, f32 rotX, s16 alpha,
                         s16 countLimit) {
    s16 i;

    if ((projectedPos != NULL) && ((projectedPos->z > 500.0f) || (projectedPos->z < 0.0f))) {
        return;
    }

    for (i = 0; i < countLimit; i++, effect++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_RIPPLE;
            effect->pos = *pos;
            effect->vel = zero;
            effect->accel = zero;
            effect->scale = scale * 0.0025f;
            effect->rot.x = rotX * 0.0025f;

            if (scale > 300.0f) {
                effect->alpha = 0;
                effect->alphaMax = alpha;
                effect->state = 0;
                effect->rot.y = (effect->rot.x - effect->scale) * 0.05f;
            } else {
                effect->alpha = alpha;
                effect->state = 1;
                effect->rot.y = (effect->rot.x - effect->scale) * 0.1f;
            }
            break;
        }
    }
}

void fs_mizu_ct(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, Vec3f* vel, f32 scale) {
    s16 i;
    Vec3f accel = { 0.0f, -1.0f, 0.0f };

    if ((projectedPos != NULL) && ((projectedPos->z > 500.0f) || (projectedPos->z < 0.0f))) {
        return;
    }

    for (i = 0; i < 100; i++, effect++) {
        if ((effect->type == FS_EFF_NONE) || (effect->type == FS_EFF_RAIN_DROP) ||
            (effect->type == FS_EFF_RAIN_RIPPLE) || (effect->type == FS_EFF_RAIN_SPLASH)) {
            effect->type = FS_EFF_DUST_SPLASH;
            effect->pos = *pos;
            effect->vel = *vel;
            effect->accel = accel;
            effect->alpha = 100 + (s16)rnd_f(100.0f);
            effect->scale = scale;
            break;
        }
    }
}

void fs_smoke_ct(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, f32 scale) {
    s16 i;
    Vec3f accel = { 0.0f, 0.05f, 0.0f };

    if ((projectedPos != NULL) && ((projectedPos->z > 500.0f) || (projectedPos->z < 0.0f))) {
        return;
    }

    for (i = 0; i < 90; i++, effect++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_WATER_DUST;
            effect->pos = *pos;
            effect->vel = zero;
            effect->accel = accel;
            effect->alpha = 255;
            effect->timer = (s16)rnd_f(100.0f);
            effect->scale = scale;
            effect->rot.x = 2.0f * scale;
            break;
        }
    }
}

void fs_bubble_ct(Vec3f* projectedPos, FishingEffect* effect, Vec3f* pos, f32 scale, u8 state) {
    s16 i;
    Vec3f vel = { 0.0f, 1.0f, 0.0f };

    if ((projectedPos != NULL) && ((projectedPos->z > 500.0f) || (projectedPos->z < 0.0f))) {
        return;
    }

    for (i = 0; i < 90; i++, effect++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_BUBBLE;
            effect->pos = *pos;
            effect->vel = vel;
            effect->accel = zero;
            effect->timer = (s16)rnd_f(100.0f);
            effect->scale = scale;
            effect->state = state;
            break;
        }
    }
}

void fs_rain_ct(FishingEffect* effect, Vec3f* pos, Vec3f* rot) {
    s16 i;
    Vec3f velSrc;

    velSrc.x = 0.0f;
    velSrc.y = 0.0f;
    velSrc.z = 300.0f;

    effect += 30;

    for (i = 30; i < FISHING_EFFECT_COUNT; i++, effect++) {
        if (effect->type == FS_EFF_NONE) {
            effect->type = FS_EFF_RAIN_DROP;
            effect->pos = *pos;
            effect->accel = zero;
            effect->rot.x = rot->x;
            effect->rot.y = rot->y;
            effect->rot.z = rot->z;
            Matrix_rotateY(rot->y, MTXMODE_NEW);
            Matrix_rotateX(rot->x, MTXMODE_APPLY);
            Matrix_Position(&velSrc, &effect->vel);
            break;
        }
    }
}

static FishingPropInit fishing_obj_data[POND_PROP_COUNT + 1] = {
    { FS_PROP_ROCK, { 529, -53, -498 } },
    { FS_PROP_ROCK, { 461, -66, -480 } },
    { FS_PROP_ROCK, { 398, -73, -474 } },
    { FS_PROP_ROCK, { -226, -52, -691 } },
    { FS_PROP_ROCK, { -300, -41, -710 } },
    { FS_PROP_ROCK, { -333, -50, -643 } },
    { FS_PROP_ROCK, { -387, -46, -632 } },
    { FS_PROP_ROCK, { -484, -43, -596 } },
    { FS_PROP_ROCK, { -409, -57, -560 } },
    { FS_PROP_WOOD_POST, { 444, -87, -322 } },
    { FS_PROP_WOOD_POST, { 447, -91, -274 } },
    { FS_PROP_WOOD_POST, { 395, -109, -189 } },
    { FS_PROP_REED, { 617, -29, 646 } },
    { FS_PROP_REED, { 698, -26, 584 } },
    { FS_PROP_REED, { 711, -29, 501 } },
    { FS_PROP_REED, { 757, -28, 457 } },
    { FS_PROP_REED, { 812, -29, 341 } },
    { FS_PROP_REED, { 856, -30, 235 } },
    { FS_PROP_REED, { 847, -31, 83 } },
    { FS_PROP_REED, { 900, -26, 119 } },
    { FS_PROP_LILY_PAD, { 861, -22, 137 } },
    { FS_PROP_LILY_PAD, { 836, -22, 150 } },
    { FS_PROP_LILY_PAD, { 829, -22, 200 } },
    { FS_PROP_LILY_PAD, { 788, -22, 232 } },
    { FS_PROP_LILY_PAD, { 803, -22, 319 } },
    { FS_PROP_LILY_PAD, { 756, -22, 348 } },
    { FS_PROP_LILY_PAD, { 731, -22, 377 } },
    { FS_PROP_LILY_PAD, { 700, -22, 392 } },
    { FS_PROP_LILY_PAD, { 706, -22, 351 } },
    { FS_PROP_LILY_PAD, { 677, -22, 286 } },
    { FS_PROP_LILY_PAD, { 691, -22, 250 } },
    { FS_PROP_LILY_PAD, { 744, -22, 290 } },
    { FS_PROP_LILY_PAD, { 766, -22, 201 } },
    { FS_PROP_LILY_PAD, { 781, -22, 128 } },
    { FS_PROP_LILY_PAD, { 817, -22, 46 } },
    { FS_PROP_LILY_PAD, { 857, -22, -50 } },
    { FS_PROP_LILY_PAD, { 724, -22, 110 } },
    { FS_PROP_LILY_PAD, { 723, -22, 145 } },
    { FS_PROP_LILY_PAD, { 728, -22, 202 } },
    { FS_PROP_LILY_PAD, { 721, -22, 237 } },
    { FS_PROP_LILY_PAD, { 698, -22, 312 } },
    { FS_PROP_LILY_PAD, { 660, -22, 349 } },
    { FS_PROP_LILY_PAD, { 662, -22, 388 } },
    { FS_PROP_LILY_PAD, { 667, -22, 432 } },
    { FS_PROP_LILY_PAD, { 732, -22, 429 } },
    { FS_PROP_LILY_PAD, { 606, -22, 366 } },
    { FS_PROP_LILY_PAD, { 604, -22, 286 } },
    { FS_PROP_LILY_PAD, { 620, -22, 217 } },
    { FS_PROP_LILY_PAD, { 663, -22, 159 } },
    { FS_PROP_LILY_PAD, { 682, -22, 73 } },
    { FS_PROP_LILY_PAD, { 777, -22, 83 } },
    { FS_PROP_LILY_PAD, { 766, -22, 158 } },
    { FS_PROP_REED, { 1073, 0, -876 } },
    { FS_PROP_REED, { 970, 0, -853 } },
    { FS_PROP_REED, { 896, 0, -886 } },
    { FS_PROP_REED, { 646, -27, -651 } },
    { FS_PROP_REED, { 597, -29, -657 } },
    { FS_PROP_REED, { 547, -32, -651 } },
    { FS_PROP_REED, { 690, -29, -546 } },
    { FS_PROP_REED, { 720, -29, -490 } },
    { FS_PROP_REED, { -756, -30, -409 } },
    { FS_PROP_REED, { -688, -34, -458 } },
    { FS_PROP_REED, { -613, -34, -581 } },
    { FS_PROP_LILY_PAD, { -593, -22, -479 } },
    { FS_PROP_LILY_PAD, { -602, -22, -421 } },
    { FS_PROP_LILY_PAD, { -664, -22, -371 } },
    { FS_PROP_LILY_PAD, { -708, -22, -316 } },
    { FS_PROP_LILY_PAD, { -718, -22, -237 } },
    { FS_PROP_REED, { -807, -36, -183 } },
    { FS_PROP_REED, { -856, -29, -259 } },
    { FS_PROP_LILY_PAD, { -814, -22, -317 } },
    { FS_PROP_LILY_PAD, { -759, -22, -384 } },
    { FS_PROP_LILY_PAD, { -718, -22, -441 } },
    { FS_PROP_LILY_PAD, { -474, -22, -567 } },
    { FS_PROP_LILY_PAD, { -519, -22, -517 } },
    { FS_PROP_LILY_PAD, { -539, -22, -487 } },
    { FS_PROP_LILY_PAD, { -575, -22, -442 } },
    { FS_PROP_LILY_PAD, { -594, -22, -525 } },
    { FS_PROP_LILY_PAD, { -669, -22, -514 } },
    { FS_PROP_LILY_PAD, { -653, -22, -456 } },
    { FS_PROP_REED, { -663, -28, -606 } },
    { FS_PROP_REED, { -708, -26, -567 } },
    { FS_PROP_REED, { -739, -27, -506 } },
    { FS_PROP_REED, { -752, -28, -464 } },
    { FS_PROP_REED, { -709, -29, -513 } },
    { FS_PROP_LILY_PAD, { -544, -22, -436 } },
    { FS_PROP_LILY_PAD, { -559, -22, -397 } },
    { FS_PROP_LILY_PAD, { -616, -22, -353 } },
    { FS_PROP_LILY_PAD, { -712, -22, -368 } },
    { FS_PROP_LILY_PAD, { -678, -22, -403 } },
    { FS_PROP_LILY_PAD, { -664, -22, -273 } },
    { FS_PROP_LILY_PAD, { -630, -22, -276 } },
    { FS_PROP_LILY_PAD, { -579, -22, -311 } },
    { FS_PROP_LILY_PAD, { -588, -22, -351 } },
    { FS_PROP_LILY_PAD, { -555, -22, -534 } },
    { FS_PROP_LILY_PAD, { -547, -22, -567 } },
    { FS_PROP_LILY_PAD, { -592, -22, -571 } },
    { FS_PROP_LILY_PAD, { -541, -22, -610 } },
    { FS_PROP_LILY_PAD, { -476, -22, -629 } },
    { FS_PROP_LILY_PAD, { -439, -22, -598 } },
    { FS_PROP_LILY_PAD, { -412, -22, -550 } },
    { FS_PROP_LILY_PAD, { -411, -22, -606 } },
    { FS_PROP_LILY_PAD, { -370, -22, -634 } },
    { FS_PROP_LILY_PAD, { -352, -22, -662 } },
    { FS_PROP_LILY_PAD, { -413, -22, -641 } },
    { FS_PROP_LILY_PAD, { -488, -22, -666 } },
    { FS_PROP_LILY_PAD, { -578, -22, -656 } },
    { FS_PROP_LILY_PAD, { -560, -22, -640 } },
    { FS_PROP_LILY_PAD, { -531, -22, -654 } },
    { FS_PROP_LILY_PAD, { -451, -22, -669 } },
    { FS_PROP_LILY_PAD, { -439, -22, -699 } },
    { FS_PROP_LILY_PAD, { -482, -22, -719 } },
    { FS_PROP_LILY_PAD, { -524, -22, -720 } },
    { FS_PROP_LILY_PAD, { -569, -22, -714 } },
    { FS_PROP_REED, { -520, -27, -727 } },
    { FS_PROP_REED, { -572, -28, -686 } },
    { FS_PROP_REED, { -588, -32, -631 } },
    { FS_PROP_REED, { -622, -34, -571 } },
    { FS_PROP_REED, { -628, -36, -510 } },
    { FS_PROP_REED, { -655, -36, -466 } },
    { FS_PROP_REED, { -655, -41, -393 } },
    { FS_PROP_REED, { -661, -47, -328 } },
    { FS_PROP_REED, { -723, -40, -287 } },
    { FS_PROP_REED, { -756, -33, -349 } },
    { FS_PROP_REED, { -755, -43, -210 } },
    { FS_PROP_LILY_PAD, { -770, -22, -281 } },
    { FS_PROP_LILY_PAD, { -750, -22, -313 } },
    { FS_PROP_LILY_PAD, { -736, -22, -341 } },
    { FS_PROP_LILY_PAD, { -620, -22, -418 } },
    { FS_PROP_LILY_PAD, { -601, -22, -371 } },
    { FS_PROP_LILY_PAD, { -635, -22, -383 } },
    { FS_PROP_LILY_PAD, { -627, -22, -311 } },
    { FS_PROP_LILY_PAD, { -665, -22, -327 } },
    { FS_PROP_LILY_PAD, { -524, -22, -537 } },
    { FS_PROP_LILY_PAD, { -514, -22, -579 } },
    { FS_PROP_LILY_PAD, { -512, -22, -623 } },
    { FS_PROP_LILY_PAD, { -576, -22, -582 } },
    { FS_PROP_LILY_PAD, { -600, -22, -608 } },
    { FS_PROP_LILY_PAD, { -657, -22, -531 } },
    { FS_PROP_LILY_PAD, { -641, -22, -547 } },
    { FS_PROP_INIT_STOP, { 0 } },
};

void fishing_obj_init(Fishing* this, PlayState* play) {
    FishingProp* prop = &fishing_obj[0];
    Vec3f colliderPos;
    s16 i;

    init_stage_rnd(1, 29100, 9786);

    for (i = 0; i < POND_PROP_COUNT; i++, prop++) {
        if (fishing_obj_data[i].type == FS_PROP_INIT_STOP) {
            break;
        }

        prop->type = fishing_obj_data[i].type;
        prop->pos.x = fishing_obj_data[i].pos.x;
        prop->pos.y = fishing_obj_data[i].pos.y;
        prop->pos.z = fishing_obj_data[i].pos.z;
        prop->rotX = prop->reedAngle = 0.0f;

        prop->timer = rnd_f(100.0f);
        prop->drawDistance = 800.0f;

        if (prop->type == FS_PROP_REED) {
            prop->scale = (stage_rnd() * 0.25f) + 0.75f;
            prop->reedAngle = rnd_f(2 * M_PI);
            if (AGE == LINK_AGE_CHILD) {
                prop->scale *= 0.6f;
            }
            prop->drawDistance = 1200.0f;
        } else if (prop->type == FS_PROP_WOOD_POST) {
            prop->scale = 0.08f;
            prop->drawDistance = 1200.0f;
            colliderPos = prop->pos;
            colliderPos.y += 50.0f;
            CollisionCheck_pos_set(i, &master->collider, &colliderPos, prop->scale * 3.5f);
        } else if (prop->type == FS_PROP_LILY_PAD) {
            prop->scale = (stage_rnd() * 0.3f) + 0.5f;
            prop->rotY = rnd_f(2 * M_PI);
            if (AGE == LINK_AGE_CHILD) {
                if ((i % 4) != 0) {
                    prop->scale *= 0.6f;
                } else {
                    prop->type = FS_PROP_NONE;
                }
            }
        } else {
            prop->scale = (stage_rnd() * 0.1f) + 0.3f;
            prop->rotY = rnd_f(2 * M_PI);
            prop->drawDistance = 1000.0f;
            CollisionCheck_pos_set(i, &master->collider, &prop->pos, prop->scale);
        }
    }
}

static FishingFishInit fish_set[] = {
    { 0, { 666, -45, 354 }, 38, 0.1f },    { 0, { 681, -45, 240 }, 36, 0.1f },   { 0, { 670, -45, 90 }, 41, 0.05f },
    { 0, { 615, -45, -450 }, 35, 0.2f },   { 0, { 500, -45, -420 }, 39, 0.1f },  { 0, { 420, -45, -550 }, 44, 0.05f },
    { 0, { -264, -45, -640 }, 40, 0.1f },  { 0, { -470, -45, -540 }, 34, 0.2f }, { 0, { -557, -45, -430 }, 54, 0.01f },
    { 0, { -260, -60, -330 }, 47, 0.05f }, { 0, { -500, -60, 330 }, 42, 0.06f }, { 0, { 428, -40, -283 }, 33, 0.2f },
    { 0, { 409, -70, -230 }, 57, 0.0f },   { 0, { 450, -67, -300 }, 63, 0.0f },  { 0, { -136, -65, -196 }, 71, 0.0f },
    { 1, { -561, -35, -547 }, 45, 0.0f },  { 1, { 667, -35, 317 }, 43, 0.0f },
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 0, ICHAIN_STOP),
};

void Fishing_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Fishing* this = (Fishing*)thisx;
    u16 fishCount;
    s16 i;

    ValueSet_process(thisx, value_init);
    Shape_Info_init(&thisx->shape, 0.0f, NULL, 0.0f);

#if DEBUG_FEATURES
    if (KREG(5) != 0) {
        AGE = LINK_AGE_CHILD;
    } else {
        AGE = z_common_data.save.linkAge;
    }
#else
    AGE = z_common_data.save.linkAge;
#endif

    if (thisx->params < EN_FISH_PARAM) {
        s32 pad;

#if PLATFORM_N64
        // Anti-piracy check, if the check fails the line can't be reeled in if
        // a fish is caught and the fish will always let go after 50 frames.
        CIC_ERROR = !(B_80008EE0 == 0xAD090010);
#else
        CIC_ERROR = 0;
#endif

        master = this;
        ClObjJntSph_ct(play, &master->collider);
        ClObjJntSph_set5_nzm(play, &master->collider, thisx, &FsIshiOcInfoJntSphData, master->colliderElements);

        thisx->params = EN_FISH_OWNER;

        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gFishingOwnerSkel, &gFishingOwnerAnim, NULL, NULL, 0);
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gFishingOwnerAnim, 0.0f);

        thisx->update = bm_move;
        thisx->draw = bm_draw;

        thisx->shape.rot.y = -0x6000;
        thisx->world.pos.x = 160.0f;
        thisx->world.pos.y = -2.0f;
        thisx->world.pos.z = 1208.0f;

        Actor_set_scale(thisx, 0.011f);

        thisx->focus.pos = thisx->world.pos;
        thisx->focus.pos.y += 75.0f;
        thisx->flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;

        if (AGE != LINK_AGE_CHILD) {
            if (HIGH_SCORE(HS_FISHING) & HS_FISH_STOLE_HAT) {
                bm_head_no = FS_OWNER_BALD;
            } else {
                bm_head_no = FS_OWNER_CAPPED;
            }
        } else {
            bm_head_no = FS_OWNER_HAIR;
        }

        head_check = 20;
        play->specialEffects = fishing_eff;
        E_day_time_plus = 1;
        fishing_game_mode = 0;
        lure_bgm_time = 10;

        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);

        if (AGE == LINK_AGE_CHILD) {
            if ((HIGH_SCORE(HS_FISHING) & HS_FISH_LENGTH_CHILD) != 0) {
                fish_record = HIGH_SCORE(HS_FISHING) & HS_FISH_LENGTH_CHILD;
            } else {
                fish_record = 40.0f; // 6 lbs
            }
        } else {
            if ((HIGH_SCORE(HS_FISHING) & HS_FISH_LENGTH_ADULT) != 0) {
                fish_record = (HIGH_SCORE(HS_FISHING) & HS_FISH_LENGTH_ADULT) >> 0x18;
            } else {
                fish_record = 45.0f; // 7 lbs
            }
        }

        play_count = (HIGH_SCORE(HS_FISHING) & (HS_FISH_PLAYED * 255)) >> 0x10;
        if ((play_count & 7) == 7) {
            play->roomCtx.drawParams[0] = 90;
            mad_water = 1;
        } else {
            play->roomCtx.drawParams[0] = 40;
            mad_water = 0;
        }

#if DEBUG_FEATURES
        if (((play_count & 7) == 6) || (KREG(3) != 0))
#else
        if ((play_count & 7) == 6)
#endif
        {
            wether = 100;
#if DEBUG_FEATURES
            if (KREG(3) != 0) {
                KREG(3) = 0;
                HIGH_SCORE(HS_FISHING) &= ~(HS_FISH_PLAYED * 255);
                HIGH_SCORE(HS_FISHING) |= (HS_FISH_PLAYED * 6);
            }
#endif
        } else {
            wether = 0;
        }

        for (i = 0; i < FISHING_EFFECT_COUNT; i++) {
            fishing_eff[i].type = FS_EFF_NONE;
        }

        for (i = 0; i < POND_PROP_COUNT; i++) {
            fishing_obj[i].type = FS_PROP_NONE;
        }

        minnow_home_angle_Y1 = 0.7f;
        minnow_home_angle_Y2 = 2.3f;
        minnow_home_angle_Y3 = 4.6f;

        for (i = 0; i < GROUP_FISH_COUNT; i++) {
            minnow[i].type = FS_GROUP_FISH_NORMAL;

            if (i <= 20) {
                minnow[i].homePos.x = minnow[i].pos.x = sinf(minnow_home_angle_Y1) * 720.0f;
                minnow[i].homePos.z = minnow[i].pos.z = cosf(minnow_home_angle_Y1) * 720.0f;
            } else if (i <= 40) {
                minnow[i].homePos.x = minnow[i].pos.x = sinf(minnow_home_angle_Y2) * 720.0f;
                minnow[i].homePos.z = minnow[i].pos.z = cosf(minnow_home_angle_Y2) * 720.0f;
            } else {
                minnow[i].homePos.x = minnow[i].pos.x = sinf(minnow_home_angle_Y3) * 720.0f;
                minnow[i].homePos.z = minnow[i].pos.z = cosf(minnow_home_angle_Y3) * 720.0f;
            }

            minnow[i].homePos.y = minnow[i].pos.y = -35.0f;

            minnow[i].timer = rnd_f(100.0f);

            minnow[i].unk_3C = 0;
            minnow[i].unk_3E = 0;
            minnow[i].unk_40 = 0;

            if (AGE != LINK_AGE_CHILD) {
                if (((i >= 15) && (i < 20)) || ((i >= 35) && (i < 40)) || ((i >= 55) && (i < 60))) {
                    minnow[i].type = FS_GROUP_FISH_NONE;
                }
            }
        }

        fishing_obj_init(this, play);
        Actor_info_make_child_actor(&play->actorCtx, thisx, play, ACTOR_EN_KANBAN, 53.0f, -17.0f, 982.0f, 0, 0, 0,
                           ENKANBAN_FISHING);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_FISHING, 0.0f, 0.0f, 0.0f, 0, 0, 0, 200);

#if DEBUG_FEATURES
        if ((KREG(1) == 1) || ((play_count & 3) == 3))
#else
        if ((play_count & 3) == 3)
#endif
        {
            if (AGE != LINK_AGE_CHILD) {
                fishCount = 16;
            } else {
                fishCount = 17;
            }
        } else {
            fishCount = 15;
        }

        for (i = 0; i < fishCount; i++) {
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_FISHING, fish_set[i].pos.x, fish_set[i].pos.y,
                        fish_set[i].pos.z, 0, rnd_f(0x10000), 0, 100 + i);
        }

        return;
    }

    if ((thisx->params < (EN_FISH_PARAM + 15)) || (thisx->params == EN_FISH_AQUARIUM)) {
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gFishingFishSkel, &gFishingFishAnim, NULL, NULL, 0);
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gFishingFishAnim, 0.0f);
    } else {
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gFishingLoachSkel, &gFishingLoachAnim, NULL, NULL, 0);
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gFishingLoachAnim, 0.0f);
    }

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (thisx->params == EN_FISH_AQUARIUM) {
        this->fishState = 100;
        Actor_info_part_chg(play, &play->actorCtx, thisx, ACTORCAT_PROP);
        thisx->attentionRangeType = ATTENTION_RANGE_0;
        thisx->flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        this->lightNode = Global_light_list_new(play2, &play->lightCtx, &this->lightInfo);
    } else {
        this->fishState = 10;
        this->fishStateNext = 10;

        this->isLoach = fish_set[thisx->params - EN_FISH_PARAM].isLoach;
        this->perception = fish_set[thisx->params - EN_FISH_PARAM].perception;
        this->fishLength = fish_set[thisx->params - EN_FISH_PARAM].baseLength;

        this->fishLength += rnd_f(4.99999f);

        // small chance to make big fish even bigger.
        if ((this->fishLength >= 65.0f) && (fqrand() < 0.05f)) {
            this->fishLength += rnd_f(7.99999f);
        }

#if DEBUG_FEATURES
        if (KREG(6) != 0) {
            this->fishLength = KREG(6) + 80.0f;
        }
#endif

        // "Come back when you get older! The fish will be bigger, too!"
        if (AGE == LINK_AGE_CHILD) {
            this->fishLength *= 0.73f;
        }
    }
}

void Fishing_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Fishing* this = (Fishing*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);

    if (thisx->params == EN_FISH_AQUARIUM) {
        Global_light_list_delete(play, &play->lightCtx, this->lightNode);
    } else if (thisx->params == EN_FISH_OWNER) {
        ClObjJntSph_dt_nzf(play, &this->collider);
    }
}

void Fishing_Eff_move(FishingEffect* effect, PlayState* play) {
    f32 rippleY;
    s16 i;

    for (i = 0; i < FISHING_EFFECT_COUNT; i++, effect++) {
        if (effect->type) {
            effect->timer++;
            effect->pos.x += effect->vel.x;
            effect->pos.y += effect->vel.y;
            effect->pos.z += effect->vel.z;
            effect->vel.y += effect->accel.y;

            if (effect->type == FS_EFF_RIPPLE) {
                add_calc2(&effect->scale, effect->rot.x, 0.2f, effect->rot.y);

                if (effect->state == 0) {
                    effect->alpha += 20;

                    if (effect->alpha >= effect->alphaMax) {
                        effect->alpha = effect->alphaMax;
                        effect->state++;
                    }
                } else {
                    effect->alpha -= 8;

                    if (effect->alpha <= 0) {
                        effect->type = FS_EFF_NONE;
                    }
                }
            } else if (effect->type == FS_EFF_WATER_DUST) {
                add_calc2(&effect->scale, effect->rot.x, 0.1f, 0.1f);
                effect->alpha -= 10;

                if (effect->pos.y > (WATER_SURFACE_Y(play) - 5.0f)) {
                    effect->vel.y = effect->accel.y = 0.0f;
                    effect->alpha -= 5;
                }

                if (effect->alpha <= 0) {
                    effect->type = FS_EFF_NONE;
                }
            } else if (effect->type == FS_EFF_BUBBLE) {
                if (effect->state == 0) {
                    rippleY = WATER_SURFACE_Y(play);
                } else {
                    rippleY = 69.0f;
                }

                if (effect->pos.y >= rippleY) {
                    effect->type = FS_EFF_NONE;

                    if (fqrand() < 0.3f) {
                        Vec3f pos = effect->pos;
                        pos.y = rippleY;
                        fs_hamon_ct(NULL, play->specialEffects, &pos, 20.0f, 60.0f, 150, 90);
                    }
                }
            } else if (effect->type == FS_EFF_DUST_SPLASH) {
                if (effect->vel.y < -20.0f) {
                    effect->vel.y = -20.0f;
                    effect->accel.y = 0.0f;
                }

                if (effect->pos.y <= WATER_SURFACE_Y(play)) {
                    effect->type = FS_EFF_NONE;
                    if (fqrand() < 0.5f) {
                        Vec3f pos = effect->pos;
                        pos.y = WATER_SURFACE_Y(play);
                        fs_hamon_ct(NULL, play->specialEffects, &pos, 40.0f, 110.0f, 150, 90);
                    }
                }
            } else if (effect->type == FS_EFF_RAIN_DROP) {
                if (effect->pos.y < WATER_SURFACE_Y(play)) {
                    f32 sqDistXZ = SQ(effect->pos.x) + SQ(effect->pos.z);

                    if (sqDistXZ > SQ(920.0f)) {
                        effect->pos.y = WATER_SURFACE_Y(play) + ((sqrtf(sqDistXZ) - 920.0f) * 0.11f);
                        effect->timer = KREG(17) + 2;
                        effect->type = FS_EFF_RAIN_SPLASH;
                        effect->scale = (KREG(18) + 30) * 0.001f;
                    } else {
                        effect->pos.y = WATER_SURFACE_Y(play) + 3.0f;
                        effect->timer = 0;
                        if (fqrand() < 0.75f) {
                            effect->type = FS_EFF_RAIN_RIPPLE;
                            effect->vel = zero;
                            effect->scale = (KREG(18) + 30) * 0.001f;
                        } else {
                            effect->type = FS_EFF_NONE;
                        }
                    }

                    effect->vel = zero;
                }
            } else if (effect->type >= FS_EFF_RAIN_RIPPLE) {
                effect->scale += (KREG(18) + 30) * 0.001f;

                if (effect->timer >= 6) {
                    effect->type = FS_EFF_NONE;
                }
            } else if (effect->type == FS_EFF_OWNER_HAT) {
                f32 sqDistXZ;
                f32 bottomY;

                effect->scale = 10 * .001f;

                adds(&cap_ang.y, 0, 20, 100);
                adds(&cap_ang.x, 0, 20, 100);
                adds(&cap_ang.z, -0x4000, 20, 100);

                sqDistXZ = SQ(effect->pos.x) + SQ(effect->pos.z);
                bottomY = WATER_SURFACE_Y(play) + ((sqrtf(sqDistXZ) - 920.0f) * 0.147f);

                if (effect->pos.y > (bottomY - 10.0f)) {
                    effect->pos.y -= 0.1f;
                }

                if ((effect->timer % 16) == 0) {
                    Vec3f pos = effect->pos;
                    pos.y = WATER_SURFACE_Y(play);
                    fs_hamon_ct(NULL, play->specialEffects, &pos, 30.0f, 300.0f, 150, 90);
                }

                if (effect->state >= 0) {
                    effect->state++;
                }

                if (effect->state == 30) {
                    message_set(play, 0x40B3, NULL);
                }

                if ((effect->state >= 100) && (message_check(&play->msgCtx) == TEXT_STATE_EVENT)) {
                    if (pad_on_check(play) || (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                        message_close(play);
                        lupy_increase(-50);
                        effect->state = -1;
                    }
                }
            }
        }
    }
}

void Fishing_Eff_disp(FishingEffect* effect, PlayState* play2) {
    u8 materialFlag = 0;
    f32 rotY;
    s16 i;
    PlayState* play = (PlayState*)play2;
    FishingEffect* firstEffect = effect;

    OPEN_DISPS(play->state.gfxCtx, "../z_fishing.c", 2271);

    Matrix_push();

    gDPPipeSync(POLY_XLU_DISP++);

    for (i = 0; i < 100; i++, effect++) {
        if (effect->type == FS_EFF_RIPPLE) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingRippleMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, effect->alpha);

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_scale(effect->scale, 1.0f, effect->scale, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 2305);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRippleModelDL);
        }
    }

    materialFlag = 0;
    effect = firstEffect;
    for (i = 0; i < 100; i++, effect++) {
        if (effect->type == FS_EFF_DUST_SPLASH) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingDustSplashMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, 0);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 180, 180, 180, effect->alpha);

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play2->billboardMtxF);
            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 2346);

            gSPDisplayList(POLY_XLU_DISP++, gFishingDustSplashModelDL);
        }
    }

    materialFlag = 0;
    effect = firstEffect;
    for (i = 0; i < 100; i++, effect++) {
        if (effect->type == FS_EFF_WATER_DUST) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_OPA_DISP++, gFishingWaterDustMaterialDL);
                gDPSetEnvColor(POLY_OPA_DISP++, 40, 90, 80, 128);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 40, 90, 80, effect->alpha);

            gSPSegment(POLY_OPA_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, effect->timer + (i * 3),
                                        (effect->timer + (i * 3)) * 5, 32, 64, 1, 0, 0, 32, 32));

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play2->billboardMtxF);
            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 2394);

            gSPDisplayList(POLY_OPA_DISP++, gFishingWaterDustModelDL);
        }
    }

    materialFlag = 0;
    effect = firstEffect;
    for (i = 0; i < 100; i++, effect++) {
        if (effect->type == FS_EFF_BUBBLE) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingBubbleMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 150, 150, 150, 0);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
                materialFlag++;
            }

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play2->billboardMtxF);
            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 2423);

            gSPDisplayList(POLY_XLU_DISP++, gFishingBubbleModelDL);
        }
    }

    materialFlag = 0;
    effect = firstEffect;
    effect += 30;
    for (i = 30; i < FISHING_EFFECT_COUNT; i++, effect++) {
        if (effect->type == FS_EFF_RAIN_DROP) {
            if (materialFlag == 0) {
                POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_20);
                gDPSetCombineMode(POLY_XLU_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 150, 255, 255, 30);
                materialFlag++;
            }

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotateY(effect->rot.y, MTXMODE_APPLY);
            Matrix_rotateX(effect->rot.x, MTXMODE_APPLY);
            Matrix_rotateZ(effect->rot.z, MTXMODE_APPLY);
            Matrix_scale(0.002f, 1.0f, 0.1f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 2467);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRainDropModelDL);
        }
    }

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    materialFlag = 0;
    effect = firstEffect;
    effect += 30;
    for (i = 30; i < FISHING_EFFECT_COUNT; i++, effect++) {
        if (effect->type == FS_EFF_RAIN_RIPPLE) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingRippleMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 155, 155, 155, 0);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 130);
                materialFlag++;
            }

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_scale(effect->scale, 1.0f, effect->scale, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 2504);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRippleModelDL);
        }
    }

    materialFlag = 0;
    effect = firstEffect;
    effect += 30;
    for (i = 30; i < FISHING_EFFECT_COUNT; i++, effect++) {
        if (effect->type == FS_EFF_RAIN_SPLASH) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingRainSplashMaterialDL);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, KREG(19) + 80);
                materialFlag++;
            }

            if (fqrand() < 0.5f) {
                rotY = 0.0f;
            } else {
                rotY = M_PI;
            }

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play2->billboardMtxF);
            Matrix_rotateY(rotY, MTXMODE_APPLY);
            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 2541);

            gSPDisplayList(POLY_XLU_DISP++, gFishingRainSplashModelDL);
        }
    }

    effect = firstEffect;
    if (effect->type == FS_EFF_OWNER_HAT) {
        Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
        Matrix_rotateY(BINANG_TO_RAD_ALT2(cap_ang.y), MTXMODE_APPLY);
        Matrix_rotateX(BINANG_TO_RAD_ALT2(cap_ang.x), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT2(cap_ang.z), MTXMODE_APPLY);
        Matrix_scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
        Matrix_translate(-1250.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_rotateX(M_PI / 2, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 2560);

        gSPDisplayList(POLY_OPA_DISP++, gFishingOwnerHatDL);
    }

    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_fishing.c", 2565);
}

void backwater_disp(PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_fishing.c", 2572);

    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, play->gameplayFrames * 1, play->gameplayFrames * 8,
                                32, 64, 1, -(play->gameplayFrames * 2), 0, 16, 16));

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, 50);

    Matrix_translate(670.0f, -24.0f, -600.0f, MTXMODE_NEW);
    Matrix_scale(0.02f, 1.0f, 0.02f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 2598);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gFishingStreamSplashDL));

    CLOSE_DISPS(play->state.gfxCtx, "../z_fishing.c", 2613);
}

// Checks if postion is above Fishing Pond owner's countertop.
s32 oyaji_floor_check(Vec3f* vec) {
    if (((vec->x >= 110.0f) && (vec->x <= 150.0f) && (vec->z <= 1400.0f) && (vec->z >= 1160.0f)) ||
        ((vec->x >= 110.0f) && (vec->x <= 210.0f) && (vec->z <= 1200.0f) && (vec->z >= 1160.0f))) {
        if (vec->y <= 42.0f) {
            return true;
        }
    }

    return false;
}

void line_control(PlayState* play, Vec3f* basePos, Vec3f* pos, Vec3f* rot, Vec3f* unk) {
    s16 i;
    s16 k;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 rx;
    f32 ry;
    f32 dist;
    f32 spD8;
    s16 spooled;
    f32 segDist;
    f32 temp_f20;
    Vec3f posSrc = { 0.0f, 0.0f, 0.0f };
    Vec3f posStep;
    f32 phi_f12;
    Vec3f tempPos;
    Vec3f segPos;
    f32 sp94;
    f32 sp90;
    f32 sp8C;
    f32 sqDistXZ;
    f32 temp_f18;

    if (line_tensyon != 0) {
        tempPos = *basePos;
        segPos = pos[LINE_SEG_COUNT - 1];

        sp94 = segPos.x - tempPos.x;
        sp90 = segPos.y - tempPos.y;
        sp8C = segPos.z - tempPos.z;

        segDist = sqrtf(SQ(sp94) + SQ(sp90) + SQ(sp8C)) * 0.97f;
        if (segDist > 1000.0f) {
            segDist = 1000.0f;
        }

        line_cap = 200.0f - (segDist * 200.0f * 0.001f);
    }

    spooled = line_cap;
    posSrc.z = 5.0f;

    for (i = 0; i < LINE_SEG_COUNT; i++) {
        if (i <= spooled) {
            pos[i] = *basePos;
        } else if (line_tensyon != 0) {
            temp_f20 = (f32)(i - spooled) / (f32)(LINE_SEG_COUNT - spooled + 1);
            add_calc2(&pos[i].x, tempPos.x + (sp94 * temp_f20), 1.0f, 20.0f);
            add_calc2(&pos[i].y, tempPos.y + (sp90 * temp_f20), 1.0f, 20.0f);
            add_calc2(&pos[i].z, tempPos.z + (sp8C * temp_f20), 1.0f, 20.0f);
        }
    }

    for (i = spooled + 1, k = 0; i < LINE_SEG_COUNT; i++, k++) {
        dx = (pos + i)->x - (pos + i - 1)->x;
        spD8 = (pos + i)->y;

        temp_f18 = 2.0f * line_G;

        sqDistXZ = SQ((pos + i)->x) + SQ((pos + i)->z);

        if (sqDistXZ > SQ(920.0f)) {
            phi_f12 = WATER_SURFACE_Y(play) + ((sqrtf(sqDistXZ) - 920.0f) * 0.11f);
        } else {
            phi_f12 = WATER_SURFACE_Y(play);
        }

        if (lure_type == FS_LURE_SINKING) {
            s32 pad;

            if (spD8 < phi_f12) {
                phi_f12 = WATER_SURFACE_Y(play) + ((sqrtf(sqDistXZ) - 920.0f) * 0.147f);
                if (spD8 > phi_f12) {
                    f32 phi_f2 = (spD8 - phi_f12) * 0.05f;

                    if (phi_f2 > 0.29999998f) {
                        phi_f2 = 0.29999998f;
                    }
                    if (i >= 100) {
                        phi_f2 *= (i - 100) * 0.02f;
                        spD8 -= phi_f2;
                    }
                }
            } else {
                spD8 -= temp_f18;
            }
        } else if (i > LINE_SEG_COUNT - 10) {
            if (spD8 > phi_f12) {
                f32 phi_f2 = (spD8 - phi_f12) * 0.2f;

                if (phi_f2 > temp_f18) {
                    phi_f2 = temp_f18;
                }
                spD8 -= phi_f2;
            }
        } else {
            if (spD8 > phi_f12) {
                spD8 -= temp_f18;
            }
        }

        if (oyaji_floor_check(&pos[i])) {
            spD8 = 42.0f;
        }

        dy = spD8 - (pos + i - 1)->y;
        dz = (pos + i)->z - (pos + i - 1)->z;

        ry = atanf_table(dz, dx);
        dist = sqrtf(SQ(dx) + SQ(dz));
        rx = -atanf_table(dist, dy);

        (rot + i - 1)->y = ry;
        (rot + i - 1)->x = rx;

        Matrix_rotateY(ry, MTXMODE_NEW);
        Matrix_rotateX(rx, MTXMODE_APPLY);
        Matrix_Position(&posSrc, &posStep);

        (pos + i)->x = (pos + i - 1)->x + posStep.x;
        (pos + i)->y = (pos + i - 1)->y + posStep.y;
        (pos + i)->z = (pos + i - 1)->z + posStep.z;
    }
}

void line_control2(Vec3f* pos) {
    s16 i;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 rx;
    f32 ry;
    f32 dist;
    Vec3f posSrc = { 0.0f, 0.0f, 0.0f };
    Vec3f posStep;
    s16 spooled = line_cap;

    posSrc.z = 5.0f;

    for (i = LINE_SEG_COUNT - 2; i > spooled; i--) {
        dx = (pos + i)->x - (pos + i + 1)->x;
        dy = (pos + i)->y - (pos + i + 1)->y;
        dz = (pos + i)->z - (pos + i + 1)->z;

        ry = atanf_table(dz, dx);
        dist = sqrtf(SQ(dx) + SQ(dz));
        rx = -atanf_table(dist, dy);

        Matrix_rotateY(ry, MTXMODE_NEW);
        Matrix_rotateX(rx, MTXMODE_APPLY);
        Matrix_Position(&posSrc, &posStep);

        (pos + i)->x = (pos + i + 1)->x + posStep.x;
        (pos + i)->y = (pos + i + 1)->y + posStep.y;
        (pos + i)->z = (pos + i + 1)->z + posStep.z;
    }
}

void hook_draw(PlayState* play, Vec3f* pos, Vec3f* refPos, u8 hookIndex) {
    f32 dx;
    f32 dy;
    f32 dz;
    f32 rx;
    f32 ry;
    f32 dist;
    f32 offsetY;
    Vec3f posSrc = { 0.0f, 0.0f, 1.0f };
    Vec3f posStep;
    Player* player = GET_PLAYER(play);

    OPEN_DISPS(play->state.gfxCtx, "../z_fishing.c", 2963);

    Matrix_push();

    if ((lure_mode == 3) && ((pos->y > WATER_SURFACE_Y(play)) || (lure_with_head && hookIndex))) {
        offsetY = 0.0f;
    } else if (pos->y < WATER_SURFACE_Y(play)) {
        offsetY = -1.0f;
    } else {
        offsetY = -3.0f;
    }

    dx = refPos->x - pos->x;
    dy = refPos->y - pos->y + offsetY;
    dz = refPos->z - pos->z;

    ry = atanf_table(dz, dx);
    dist = sqrtf(SQ(dx) + SQ(dz));
    rx = -atanf_table(dist, dy);

    Matrix_rotateY(ry, MTXMODE_NEW);
    Matrix_rotateX(rx, MTXMODE_APPLY);
    Matrix_Position(&posSrc, &posStep);

    refPos->x = pos->x + posStep.x;
    refPos->y = pos->y + posStep.y;
    refPos->z = pos->z + posStep.z;

    Matrix_translate(pos->x, pos->y, pos->z, MTXMODE_NEW);

    if ((player->actor.speed == 0.0f) && (lure_act_speed == 0.0f)) {
        add_calc2(&hook_angle_Y[hookIndex], ry, 0.1f, 0.3f);
    } else {
        hook_angle_Y[hookIndex] = ry;
    }

    Matrix_rotateY(hook_angle_Y[hookIndex], MTXMODE_APPLY);
    Matrix_rotateX(rx, MTXMODE_APPLY);
    Matrix_scale(0.0039999997f, 0.0039999997f, 0.005f, MTXMODE_APPLY);
    Matrix_rotateY(M_PI, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 3029);
    gSPDisplayList(POLY_OPA_DISP++, gFishingLureHookDL);

    Matrix_rotateZ(M_PI / 2, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 3034);
    gSPDisplayList(POLY_OPA_DISP++, gFishingLureHookDL);

    if ((hookIndex == 1) && (lure_with_head)) {
        Matrix_scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);
        Matrix_translate(250.0f, 0.0f, -1400.0f, MTXMODE_APPLY);
        Matrix_push();

        if (lure_with_head_off) {
            FishingEffect* effect = play->specialEffects;
            MtxF mf;

            Matrix_Position(&zero, &effect->pos);
            Matrix_get(&mf);
            Matrix_to_rotate_new(&mf, &cap_ang, 0);

            lure_with_head_off = false;
            lure_with_head = false;

            effect->type = FS_EFF_OWNER_HAT;
            effect->state = 0;
            effect->vel = zero;
            effect->accel = zero;
        }

        Matrix_pull();
        Matrix_translate(-1250.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_rotateX(M_PI / 2, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 3085);
        gSPDisplayList(POLY_OPA_DISP++, gFishingOwnerHatDL);
    }

    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_fishing.c", 3098);
}

void worm_control(PlayState* play) {
    s16 i;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 rx;
    f32 ry;
    f32 dist;
    f32 offsetY;
    Vec3f posSrc = { 0.0f, 0.0f, 0.0f };
    Vec3f posStep;
    Vec3f sp94;
    Vec3f sp88;
    Vec3f offset;
    Player* player = GET_PLAYER(play);
    Vec3f* pos;

    posSrc.z = 0.85f;

    worm_shape_pos[0] = lure_pos;

    if (lure_hirotta) {
        offsetY = -1.0f;
    } else if (lure_pos.y < WATER_SURFACE_Y(play)) {
        offsetY = 0.5f;
    } else {
        offsetY = -5.0f;
    }

    if (lure_mode == 5) {
        Matrix_rotateY(BINANG_TO_RAD(player->actor.shape.rot.y), MTXMODE_NEW);
        sp94.x = 5.0f;
        sp94.y = 0.0f;
        sp94.z = 3.0f;
        Matrix_Position(&sp94, &sp88);
    }

    for (i = 1; i < SINKING_LURE_SEG_COUNT; i++) {
        pos = worm_shape_pos;

        if ((i < 10) && (lure_mode == 5)) {
            offset.x = sp88.x * (10 - i) * 0.1f;
            offset.z = sp88.z * (10 - i) * 0.1f;
        } else {
            offset.x = offset.z = 0.0f;
        }

        dx = (pos + i)->x - (pos + i - 1)->x + offset.x;
        dy = (pos + i)->y - (pos + i - 1)->y + offsetY;
        dz = (pos + i)->z - (pos + i - 1)->z + offset.z;

        ry = atanf_table(dz, dx);
        dist = sqrtf(SQ(dx) + SQ(dz));
        rx = -atanf_table(dist, dy);

        Matrix_rotateY(ry, MTXMODE_NEW);
        Matrix_rotateX(rx, MTXMODE_APPLY);
        Matrix_Position(&posSrc, &posStep);

        (pos + i)->x = (pos + i - 1)->x + posStep.x;
        (pos + i)->y = (pos + i - 1)->y + posStep.y;
        (pos + i)->z = (pos + i - 1)->z + posStep.z;
    }
}

static f32 worm_scale[] = {
    1.0f, 1.5f,  1.8f, 2.0f, 1.8f, 1.6f, 1.4f, 1.2f, 1.0f, 1.0f,
    0.9f, 0.85f, 0.8f, 0.7f, 0.8f, 1.0f, 1.2f, 1.1f, 1.0f, 0.8f,
};

void worm_disp(PlayState* play2) {
    PlayState* play = (PlayState*)play2;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_fishing.c", 3209);

    worm_control(play2);

    if (lure_pos.y < WATER_SURFACE_Y(play2)) {
        _texture_z_light_fog_prim(play->state.gfxCtx);

        gSPDisplayList(POLY_OPA_DISP++, gFishingSinkingLureSegmentMaterialDL);

        for (i = SINKING_LURE_SEG_COUNT - 1; i >= 0; i--) {
            if ((i + worm_eat) < SINKING_LURE_SEG_COUNT) {
                Matrix_translate(worm_shape_pos[i].x, worm_shape_pos[i].y, worm_shape_pos[i].z, MTXMODE_NEW);
                Matrix_scale(worm_scale[i + worm_eat] * 0.04f,
                             worm_scale[i + worm_eat] * 0.04f,
                             worm_scale[i + worm_eat] * 0.04f, MTXMODE_APPLY);
                Matrix_rotate_scale_exchange(&play2->billboardMtxF);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 3239);
                gSPDisplayList(POLY_OPA_DISP++, gFishingSinkingLureSegmentModelDL);
            }
        }
    } else {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

        gSPDisplayList(POLY_XLU_DISP++, gFishingSinkingLureSegmentMaterialDL);

        for (i = SINKING_LURE_SEG_COUNT - 1; i >= 0; i--) {
            if ((i + worm_eat) < SINKING_LURE_SEG_COUNT) {
                Matrix_translate(worm_shape_pos[i].x, worm_shape_pos[i].y, worm_shape_pos[i].z, MTXMODE_NEW);
                Matrix_scale(worm_scale[i + worm_eat] * 0.04f,
                             worm_scale[i + worm_eat] * 0.04f,
                             worm_scale[i + worm_eat] * 0.04f, MTXMODE_APPLY);
                Matrix_rotate_scale_exchange(&play2->billboardMtxF);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 3265);
                gSPDisplayList(POLY_XLU_DISP++, gFishingSinkingLureSegmentModelDL);
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_fishing.c", 3271);
}

void fishing_draw(PlayState* play, Vec3f* linePos, Vec3f* lineRot) {
    Vec3f posSrc;
    Vec3f posStep;
    Vec3f hookPos[2];
    s16 i;
    s16 spooled = line_cap;
    s32 pad;
    Player* player = GET_PLAYER(play);

    OPEN_DISPS(play->state.gfxCtx, "../z_fishing.c", 3287);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Matrix_push();

    if (lure_hirotta) {
        Vec3f posTemp = lure_pos;
        lure_pos = lure_hirotta_pos;
        worm_disp(play);
        lure_pos = posTemp;
    }

    if ((lure_mode == 4) || (lure_mode == 5)) {
        lure_pos = hit_fish_actor->fishMouthPos;

        if ((lure_mode == 5) && (lure_type == FS_LURE_SINKING)) {
            Matrix_rotateY(BINANG_TO_RAD(player->actor.shape.rot.y), MTXMODE_NEW);
            posSrc.x = 2.0f;
            posSrc.y = 0.0f;
            posSrc.z = 0.0f;
            Matrix_Position(&posSrc, &posStep);
            lure_pos.x += posStep.x;
            lure_pos.z += posStep.z;
        }
    } else if (lure_mode == 0) {
        lure_pos = line_pos[LINE_SEG_COUNT - 1];
        lure_ang.x = line_angle[LINE_SEG_COUNT - 2].x + M_PI;

        if ((player->actor.speed == 0.0f) && (lure_time == 0)) {
            add_calc2(&lure_ang.y, line_angle[LINE_SEG_COUNT - 2].y, 0.1f, 0.2f);
        } else {
            lure_ang.y = line_angle[LINE_SEG_COUNT - 2].y;
        }
    }

    if (lure_type != FS_LURE_SINKING) {
        Matrix_translate(lure_pos.x, lure_pos.y, lure_pos.z, MTXMODE_NEW);
        Matrix_rotateY(lure_ang.y + lure_vib_ya, MTXMODE_APPLY);
        Matrix_rotateX(lure_ang.x, MTXMODE_APPLY);
        Matrix_scale(0.0039999997f, 0.0039999997f, 0.0039999997f, MTXMODE_APPLY);
        Matrix_translate(0.0f, 0.0f, lure_offset_z, MTXMODE_APPLY);
        Matrix_rotateZ(M_PI / 2, MTXMODE_APPLY);
        Matrix_rotateY(M_PI / 2, MTXMODE_APPLY);

        _texture_z_light_fog_prim(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 3369);
        gSPDisplayList(POLY_OPA_DISP++, gFishingLureFloatDL);

        posSrc.x = -850.0f;
        posSrc.y = 0.0f;
        posSrc.z = 0.0f;
        Matrix_Position(&posSrc, &lure_head_pos);

        posSrc.x = 500.0f;
        posSrc.z = -300.0f;
        Matrix_Position(&posSrc, &hookPos[0]);
        hook_draw(play, &hookPos[0], &hook_pos[0], 0);

        posSrc.x = 2100.0f;
        posSrc.z = -50.0f;
        Matrix_Position(&posSrc, &hookPos[1]);
        hook_draw(play, &hookPos[1], &hook_pos[1], 1);
    }

    POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_20);

    gDPSetCombineMode(POLY_XLU_DISP++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 55);

    if ((lure_mode == 4) && ((lure_hook_up != 0) || (lure_type != FS_LURE_SINKING))) {
        f32 rx;
        f32 ry;
        f32 dist;
        f32 dx;
        f32 dy;
        f32 dz;

        dx = lure_pos.x - line_base_pos.x;
        dy = lure_pos.y - line_base_pos.y;
        dz = lure_pos.z - line_base_pos.z;

        ry = fatan2(dx, dz);
        dist = sqrtf(SQ(dx) + SQ(dz));
        rx = -fatan2(dy, dist);

        dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) * 0.001f;

        Matrix_translate(line_base_pos.x, line_base_pos.y, line_base_pos.z, MTXMODE_NEW);
        Matrix_rotateY(ry, MTXMODE_APPLY);
        Matrix_rotateX(rx, MTXMODE_APPLY);
        Matrix_scale(line_scale, 1.0f, dist, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 3444);
        gSPDisplayList(POLY_XLU_DISP++, gFishingLineModelDL);
    } else {
        for (i = spooled; i < LINE_SEG_COUNT - 1; i++) {
            if ((i == LINE_SEG_COUNT - 3) && (lure_type == FS_LURE_STOCK) && (lure_mode == 3)) {
                f32 rx;
                f32 ry;
                f32 dist;
                f32 dx;
                f32 dy;
                f32 dz;

                dx = lure_head_pos.x - (linePos + i)->x;
                dy = lure_head_pos.y - (linePos + i)->y;
                dz = lure_head_pos.z - (linePos + i)->z;

                ry = fatan2(dx, dz);
                dist = sqrtf(SQ(dx) + SQ(dz));
                rx = -fatan2(dy, dist);

                dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) * 0.001f;

                Matrix_translate((linePos + i)->x, (linePos + i)->y, (linePos + i)->z, MTXMODE_NEW);
                Matrix_rotateY(ry, MTXMODE_APPLY);
                Matrix_rotateX(rx, MTXMODE_APPLY);
                Matrix_scale(line_scale, 1.0f, dist, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 3475);
                gSPDisplayList(POLY_XLU_DISP++, gFishingLineModelDL);
                break;
            }

            Matrix_translate((linePos + i)->x, (linePos + i)->y, (linePos + i)->z, MTXMODE_NEW);
            Matrix_rotateY((lineRot + i)->y, MTXMODE_APPLY);
            Matrix_rotateX((lineRot + i)->x, MTXMODE_APPLY);
            Matrix_scale(line_scale, 1.0f, 0.005f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 3492);
            gSPDisplayList(POLY_XLU_DISP++, gFishingLineModelDL);
        }
    }

    Matrix_pull();
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    CLOSE_DISPS(play->state.gfxCtx, "../z_fishing.c", 3500);
}

static f32 rod_scale[22] = {
    1.0f,        1.0f,        1.0f,        0.9625f,     0.925f, 0.8875f,     0.85f,       0.8125f,
    0.775f,      0.73749995f, 0.7f,        0.6625f,     0.625f, 0.5875f,     0.54999995f, 0.5125f,
    0.47499996f, 0.4375f,     0.39999998f, 0.36249995f, 0.325f, 0.28749996f,
};

static f32 rod_teper[22] = {
    0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.06f,   0.12f,   0.18f,   0.24f,   0.30f,   0.36f,
    0.42f, 0.48f, 0.54f, 0.60f, 0.60f, 0.5142f, 0.4285f, 0.3428f, 0.2571f, 0.1714f, 0.0857f,
};

static Vec3f mae = { 0.0f, 0.0f, 0.0f };

void rod_draw(PlayState* play2) {
    s16 i;
    f32 lureXZLen;
    f32 spC4;
    f32 spC0;
    PlayState* play = (PlayState*)play2;
    Player* player = GET_PLAYER(play);
    Input* input = &play->state.input[0];

    OPEN_DISPS(play->state.gfxCtx, "../z_fishing.c", 3600);

    if (rod_hooking_impact != 0) {
        rod_hooking_impact--;

        add_calc2(&rod_base_curvX, 35.0f, 1.0f, 100.0f);
        add_calc2(&rod_curvX3, -0.8f, 1.0f, 0.4f);
        adds(&player->actor.shape.rot.x, -4000, 2, 15000);
    } else {
        s16 target = 0;

        if ((lure_mode == 4) && lure_hook_up) {
            target = sin_s(lure_count * 25600) * 1500.0f;
        } else {
            add_calc0(&rod_base_curvX, 0.1f, 10.0f);
            add_calc0(&rod_curvX3, 1.0f, 0.05f);
        }

        adds(&player->actor.shape.rot.x, target, 5, 1000);
    }

    if ((lure_mode == 3) || (lure_mode == 4)) {
        if ((input->rel.stick_x == 0) && (old_stick_x != 0)) {
            rod_curvY_spd = 0.0f;
        }
        if ((input->rel.stick_y == 0) && (old_stick_y != 0)) {
            rod_curvX_spd = 0.0f;
        }

        lureXZLen = player->unk_85C;
        add_calc(&player->unk_85C, input->rel.stick_y * 0.02f, 0.3f, 5.0f, 0.0f);
        lureXZLen = player->unk_85C - lureXZLen;

        spC4 = player->unk_858;
        add_calc(&player->unk_858, input->rel.stick_x * 0.02f, 0.3f, 5.0f, 0.0f);
        spC4 = player->unk_858 - spC4;

        if (player->unk_858 > 1.0f) {
            player->unk_858 = 1.0f;
        }
        if (player->unk_85C > 1.0f) {
            player->unk_85C = 1.0f;
        }
        if (player->unk_858 < -1.0f) {
            player->unk_858 = -1.0f;
        }
        if (player->unk_85C < -1.0f) {
            player->unk_85C = -1.0f;
        }

        add_calc2(&rod_curvY, spC4 * 70.0f * -0.01f, 1.0f, rod_curvY_spd);
        add_calc2(&rod_curvY_spd, 1.0f, 1.0f, 0.1f);
        add_calc2(&rod_curvX, lureXZLen * 70.0f * 0.01f, 1.0f, rod_curvX_spd);
        add_calc2(&rod_curvX_spd, 1.0f, 1.0f, 0.1f);
        add_calc0(&rod_curvX2, 1.0f, 0.05f);
    } else {
        add_calc0(&player->unk_85C, 1.0f, 0.1f);
        add_calc0(&player->unk_858, 1.0f, 0.1f);
        add_calc2(&rod_curvX, (sin_s(lure_count * 3000) * 0.025f) + -0.03f, 1.0f, 0.05f);
        add_calc0(&rod_curvY, 1.0f, 0.05f);

        if ((lure_cast_wait > 18) && (lure_cast_wait < 25)) {
            add_calc2(&rod_curvX2, 0.8f, 1.0f, 0.2f);
        } else {
            add_calc2(&rod_curvX2, 0.0f, 1.0f, 0.4f);
        }
    }

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPDisplayList(POLY_OPA_DISP++, gFishingRodMaterialDL);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 155, 0, 255);

    Matrix_mult(&player->mf_9E0, MTXMODE_NEW);

    if (AGE != LINK_AGE_CHILD) {
        Matrix_translate(0.0f, 400.0f, 0.0f, MTXMODE_APPLY);
    } else {
        Matrix_translate(0.0f, 230.0f, 0.0f, MTXMODE_APPLY);
    }

    if (lure_mode == 5) {
        Matrix_rotateY(0.56f * M_PI, MTXMODE_APPLY);
    } else {
        Matrix_rotateY(0.41f * M_PI, MTXMODE_APPLY);
    }

    Matrix_rotateX(-M_PI / 5.0000003f, MTXMODE_APPLY);
    Matrix_rotateZ((player->unk_858 * 0.5f) + 3.0f * M_PI / 20.0f, MTXMODE_APPLY);
    Matrix_rotateX((rod_base_curvX + 20.0f) * 0.01f * M_PI, MTXMODE_APPLY);
    Matrix_scale(0.70000005f, 0.70000005f, 0.70000005f, MTXMODE_APPLY);

    spC0 = rod_curvX + rod_curvX2 + (rod_curvX3 * ((((*player).unk_85C - 1.0f) * -0.25f) + 0.5f));

    Matrix_translate(0.0f, 0.0f, -1300.0f, MTXMODE_APPLY);

    for (i = 0; i < 22; i++) {
        Matrix_rotateY(rod_curvY * rod_teper[i] * 0.5f, MTXMODE_APPLY);
        Matrix_rotateX(spC0 * rod_teper[i] * 0.5f, MTXMODE_APPLY);

        Matrix_push();
        Matrix_scale(rod_scale[i], rod_scale[i], 0.52f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 3809);

        if (i < 5) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFishingRodSegmentBlackTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 8, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
        } else if ((i < 8) || ((i % 2) == 0)) {
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFishingRodSegmentWhiteTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 8, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
        } else {
            gDPLoadTextureBlock(POLY_OPA_DISP++, gFishingRodSegmentStripTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 8, 0,
                                G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, 3, G_TX_NOLOD, G_TX_NOLOD);
        }

        gSPDisplayList(POLY_OPA_DISP++, gFishingRodSegmentDL);

        Matrix_pull();
        Matrix_translate(0.0f, 0.0f, 500.0f, MTXMODE_APPLY);

        if (i == 21) {
            Matrix_Position(&mae, &line_base_pos);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_fishing.c", 3838);
}

static Vec3f lure_se_pos = { 0.0f, 0.0f, 0.0f };

void lure_move(Fishing* this, PlayState* play) {
    f32 spE4;
    f32 spE0;
    s16 i;
    s16 spDC;
    f32 spD8;
    f32 spD4;
    f32 spD0;
    f32 lengthCasted;
    f32 lureXZLen;
    f32 phi_f0;
    Player* player = GET_PLAYER(play);
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f spA8;
    Vec3f sp9C;
    Vec3f sp90;
    Input* input = &play->state.input[0];

    lure_count++;

    if (lure_time != 0) {
        lure_time--;
    }

    if (lure_cast_time != 0) {
        lure_cast_time--;
    }

    if (lure_cast_wait != 0) {
        lure_cast_wait--;
    }

    if (lure_hook_time != 0) {
        lure_hook_time--;
    }

    if (lure_dog_wait_time != 0) {
        lure_dog_wait_time--;
    }

    if (line_tensyon != 0) {
        line_tensyon--;
    }

    if (vib_time != 0) {
        vib_time--;
    }

    if (lure_in_water != 0) {
        lure_in_water--;
    }

    if (fishing_game_mode == 1) {
        fishing_game_mode = 2;
        fish_catch_count = 0;
        seken_mes_ct = 0;
        lure_type = FS_LURE_STOCK;

        // if prize item won as child or adult, set the sinking lure location.
        if (((AGE == LINK_AGE_CHILD) && (HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_CHILD)) ||
            ((AGE != LINK_AGE_CHILD) && (HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_ADULT))) {
            worm_set_P = (u8)rnd_f(3.999f) + 1;
        }

        line_G = 520.0f;
        line_cap = 195.0f;

        lure_mode = lure_type = lure_count = lure_time = lure_cast_time = lure_cast_wait = lure_action =
            lure_in_water = lure_dog_wait_time = 0;
        lure_vib_ya = lure_hosei_s = lure_offset_z = 0.0f;

        lure_act_vec = zeroVec;

        for (i = 0; i < LINE_SEG_COUNT; i++) {
            line_pos[i] = zeroVec;
            line_angle[i] = zeroVec;
            line_spd[i] = zeroVec;
        }
    }

    Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &lure_pos, &lure_se_pos, &float_dammy);

    if (lure_mode == 0) {
        add_calc2(&lure_offset_z, -800.0f, 1.0f, 20.0f);
    } else {
        add_calc2(&lure_offset_z, 300.0f, 1.0f, 20.0f);
    }

    switch (lure_mode) {
        case 0:
            worm_eat = 0;

#if DEBUG_FEATURES
            if (KREG(14) != 0) {
                KREG(14) = 0;
                lure_type = FS_LURE_SINKING - lure_type;
                if (lure_type != FS_LURE_STOCK) {
                    seken_mes_ct = 0;
                }
            }
#endif

            add_calc2(&line_cap, 195.0f, 1.0f, 1.0f);

            if (player->stateFlags1 & PLAYER_STATE1_27) {
                lure_cast_wait = 0;
                player->unk_860 = 0;
            }

            if (lure_cast_wait == 0) {
                if ((lure_time == 0) && (player->unk_860 == 1)) {
                    lure_cast_wait = 37;
                    message_close(play);
                }
            } else {
                lure_ang.x = line_angle[LINE_SEG_COUNT - 2].x + M_PI;
                lure_ang.y = line_angle[LINE_SEG_COUNT - 2].y;

                if (lure_cast_wait == 18) {
                    lure_mode = 1;
                    lure_pos = line_base_pos;
                    Matrix_rotateY(BINANG_TO_RAD_ALT(player->actor.shape.rot.y), MTXMODE_NEW);
                    sp90.x = 0.0f;
                    sp90.y = 0.0f;
                    sp90.z = 25.0f;
                    Matrix_Position(&sp90, &lure_spd);
                    lure_spd.y = 15.0f;
                    lure_acc.x = lure_acc.z = 0.0f;
                    lure_acc.y = -1.0f;
                    line_G = 0.0f;
                    lure_cast_time = 5;
                    hit_power = 0.5f;
                    lure_search_p = rnd_f(1.9f);
                    fish_mouse_p.y = 500.0f;
                    Na_StartObjectSe_F(&lure_se_pos, NA_SE_IT_SWORD_SWING_HARD);
                }
            }
            break;

        case 1: // casting the line
            spE0 = lure_pos.y;

            lure_pos.x += lure_spd.x;
            lure_pos.y += lure_spd.y;
            lure_pos.z += lure_spd.z;

            lure_spd.x += lure_acc.x;
            lure_spd.y += lure_acc.y;
            lure_spd.z += lure_acc.z;
            // air drag by hat or reeling during cast.
            if (CHECK_BTN_ALL(input->cur.button, BTN_A) || lure_with_head) {
                lure_spd.x *= 0.9f;
                lure_spd.z *= 0.9f;
                if (!lure_with_head) {
                    Na_StartSystemSe_F(NA_SE_IT_FISHING_REEL_HIGH - SFX_FLAG);
                }
            }

            spD8 = lure_pos.x - line_base_pos.x;
            spD4 = lure_pos.y - line_base_pos.y;
            spD0 = lure_pos.z - line_base_pos.z;

            if (lure_cast_time != 0) {
                lure_ang.x = line_angle[LINE_SEG_COUNT - 2].x + M_PI;
                lure_ang.y = line_angle[LINE_SEG_COUNT - 2].y;
            } else {
                lure_ang.x = 0.0f;
                lure_ang.y = atanf_table(spD0, spD8) + M_PI;
            }

            lengthCasted = sqrtf(SQ(spD8) + SQ(spD4) + SQ(spD0));
            if (lengthCasted > 1000.0f) {
                lengthCasted = 1000.0f;
            }
            line_cap = 200.0f - (lengthCasted * 200.0f * 0.001f);

            lureXZLen = SQ(lure_pos.x) + SQ(lure_pos.z);
            if (lureXZLen > SQ(920.0f)) {
                if ((KREG_DEBUG(56) != 0) || (lure_pos.y > 160.0f) || (lure_pos.x < 80.0f) || (lure_pos.x > 180.0f) ||
                    (lure_pos.z > 1350.0f) || (lure_pos.z < 1100.0f) || (lure_pos.y < 45.0f)) {
                    Vec3f sp80 = this->actor.world.pos;

                    this->actor.prevPos = this->actor.world.pos = lure_pos;
                    Actor_BGcheck2(play, &this->actor, 15.0f, 30.0f, 30.0f,
                                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_6);
                    this->actor.world.pos = sp80;

                    if (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) {
                        lure_spd.y = -0.5f;
                    }
                    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                        if (lure_spd.y > 0.0f) {
                            lure_spd.y = 0.0f;
                        }
                        lure_spd.x = lure_spd.z = 0.0f;
                    }
                } else {
                    if (oyaji_floor_check(&lure_pos)) {
                        lure_mode = 3;
                        lure_hosei_s = 0.0f;
                    }
                }

                spE4 = WATER_SURFACE_Y(play) + ((sqrtf(lureXZLen) - 920.0f) * 0.11f);
                if (lure_pos.y <= spE4) {
                    lure_pos.y = spE4;
                    lure_spd.x = lure_spd.y = lure_spd.z = 0.0f;
                    lure_mode = 3;
                    lure_hosei_s = 0.0;
                } else {
                    add_calc2(&line_G, 0.0f, 1.0f, 0.05f);
                    Na_StartObjectSe_F(&lure_se_pos, NA_SE_EN_FANTOM_FLOAT - SFX_FLAG);
                }
            } else {
                spE4 = WATER_SURFACE_Y(play);

                if (lure_pos.y <= spE4) {
                    lure_mode = 2;
                    lure_hosei_s = 0.0f;
                    lure_spd.x = lure_spd.z = 0.0f;

                    if (lure_type == FS_LURE_SINKING) {
                        lure_reel_ready = 0;
                    } else {
                        lure_reel_ready = 10;
                    }

                    if ((lure_pos.y <= spE4) && (spE4 < spE0) && (spE4 == WATER_SURFACE_Y(play))) {
                        lure_in_water = 10;
                        Na_StartObjectSe_F(&lure_se_pos, NA_SE_EV_BOMB_DROP_WATER);
                        lure_acc.y = 0.0f;
                        lure_spd.y *= 0.2f;

                        for (i = 0; i < 50; i++) {
                            f32 sp7C = rnd_f(1.5f) + 0.5f;
                            f32 sp78 = rnd_f(6.28f);

                            sp9C.x = sinf(sp78) * sp7C;
                            sp9C.z = cosf(sp78) * sp7C;
                            sp9C.y = rnd_f(3.0f) + 3.0f;

                            spA8 = lure_pos;
                            spA8.x += (sp9C.x * 3.0f);
                            spA8.y = WATER_SURFACE_Y(play);
                            spA8.z += (sp9C.z * 3.0f);
                            fs_mizu_ct(NULL, play->specialEffects, &spA8, &sp9C,
                                                    rnd_f(0.02f) + 0.025f);
                        }

                        spA8 = lure_pos;
                        spA8.y = WATER_SURFACE_Y(play);
                        fs_hamon_ct(NULL, play->specialEffects, &spA8, 100.0f, 800.0f, 150, 90);
                    }
                } else {
                    add_calc0(&line_G, 1.0f, 0.05f);
                    Na_StartObjectSe_F(&lure_se_pos, NA_SE_EN_FANTOM_FLOAT - SFX_FLAG);
                }
            }

            line_pos[LINE_SEG_COUNT - 1].x = lure_pos.x;
            line_pos[LINE_SEG_COUNT - 1].y = lure_pos.y;
            line_pos[LINE_SEG_COUNT - 1].z = lure_pos.z;

            lure_dog_walk_way = 1.0f;
            lure_dog_swing_X = 0.5f;
            break;

        case 2:
            if (lure_pos.y <= WATER_SURFACE_Y(play)) {
                lure_pos.y += lure_spd.y;

                add_calc0(&lure_spd.y, 1.0f, 1.0f);

                if (lure_type != FS_LURE_SINKING) {
                    add_calc2(&lure_pos.y, WATER_SURFACE_Y(play), 0.5f, 1.0f);
                }
            }

            add_calc2(&line_G, 2.0f, 1.0f, 0.1f);

            if (lure_reel_ready == 0) {
                lure_mode = 3;
            } else {
                lure_reel_ready--;
            }
            break;

        case 3:
            worm_eat = 0;

            if (lure_with_head && ((SQ(lure_pos.x) + SQ(lure_pos.z)) < SQ(500.0f))) {
                lure_with_head_off = true;
            }

            player->unk_860 = 2;

            if (lure_act_speed < 3.0f) {
                lureXZLen = sin_s(lure_count * 0x1060) * lure_dog_swing_X;
                add_calc2(&lure_ang.x, -M_PI / 6.0f + lureXZLen, 0.3f, lure_ang_x_ad);
                add_calc2(&lure_ang_x_ad, 0.5f, 1.0f, 0.02f);
                add_calc0(&lure_dog_swing_X, 1.0f, 0.02f);
            } else {
                lure_ang_x_ad = 0.0f;
            }

            spDC = 0x4000;
            spE4 = WATER_SURFACE_Y(play);

            lureXZLen = SQ(lure_pos.x) + SQ(lure_pos.z);
            if (lureXZLen < SQ(920.0f)) {
                if (lure_pos.y <= spE4 + 4) {
                    Input* input2 = &play->state.input[0];
                    f32 wiggle = 0.0f;

                    if (lure_dog_wait_time == 0) {
                        if (fabsf(input2->rel.stick_x) > 30.0f) {
                            wiggle = fabsf((input2->rel.stick_x - old_stick_x) * (1.0f / 60.0f));
                        } else if (fabsf(input2->rel.stick_y) > 30.0f) {
                            wiggle = fabsf((input2->rel.stick_y - old_stick_y) * (1.0f / 60.0f));
                        }
                    }

                    if (wiggle > 1.0f) {
                        wiggle = 1.0f;
                    }
                    if (CHECK_BTN_ALL(input2->press.button, BTN_B)) {
                        wiggle = 0.5f;
                    }

                    if (lure_with_head) {
                        if (wiggle > 0.3f) {
                            wiggle = 0.3f;
                        }
                    }

                    if ((wiggle > 0.2f) && (lure_act_speed < 4.0f)) {
                        lure_dog_wait_time = 5;

                        if (wiggle > 0.8f) {
                            lure_action = 2;
                        } else {
                            lure_action = 1;
                        }

                        sp90.x = player->actor.world.pos.x - lure_pos.x;
                        sp90.z = player->actor.world.pos.z - lure_pos.z;
                        sp90.y = atanf_table(sp90.z, sp90.x);

                        lure_act_ang_Y = sp90.y + (wiggle * lure_dog_walk_way);
                        lure_dog_walk_way *= -1.0f;
                        lure_act_speed = fabsf(wiggle) * 6.0f;
                        lure_ang.x = 0.0f;
                        lure_dog_swing_X = 0.5f;
                        line_cap += (fabsf(wiggle) * (7.5f + (KREG_DEBUG(25) * 0.1f)));

                        Na_SetMotorSe(&lure_se_pos, NA_SE_EV_LURE_MOVE_W, (wiggle * 1.999f * 0.25f) + 0.75f);

                        if (lure_type == FS_LURE_SINKING) {
                            lure_act_vec.y = 5.0f * wiggle;
                            line_pos[LINE_SEG_COUNT - 1].y += lure_act_vec.y;
                            lure_pos.y += lure_act_vec.y;
                        }
                    } else if (CHECK_BTN_ALL(input2->cur.button, BTN_A)) {
                        spDC = 0x500;
                        lure_act_ang_Y = line_angle[LINE_SEG_COUNT - 2].y + M_PI;
                        lure_ang.x = 0.0f;
                        lure_dog_swing_X = 0.5f;
                        if (lure_type == FS_LURE_SINKING) {
                            lure_act_vec.y = 0.2f;
                            line_pos[LINE_SEG_COUNT - 1].y += lure_act_vec.y;
                            lure_pos.y += lure_act_vec.y;
                        }
                    }
                } else {
                    if (line_cap > 150.0f) {
                        lure_ang.x = line_angle[LINE_SEG_COUNT - 2].x + M_PI;
                        lure_act_ang_Y = line_angle[LINE_SEG_COUNT - 2].y + M_PI;
                        line_cap += 2.0f;
                    }
                }
            } else {
                spE4 = WATER_SURFACE_Y(play) + ((sqrtf(lureXZLen) - 920.0f) * 0.11f);
                if (lure_pos.y <= spE4) {
                    lure_pos.y = spE4;
                    spDC = 0x500;
                    lure_act_ang_Y = line_angle[LINE_SEG_COUNT - 2].y + M_PI;
                    lure_ang.x = 0.0f;
                    // lure hopping on land
                    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                        line_cap += 6.0f;
                        Na_StartObjectSe_F(&lure_se_pos, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_SAND);
                    }
                } else {
                    if (line_cap > 150.0f) {
                        lure_ang.x = line_angle[LINE_SEG_COUNT - 2].x + M_PI;
                        lure_act_ang_Y = line_angle[LINE_SEG_COUNT - 2].y + M_PI;
                        line_cap += 2.0f;
                    }
                }
            }

            add_calc0(&lure_act_speed, 1.0f, 0.3f);
            adds(&lure_ang_Y_s, (lure_act_ang_Y * 32768.0f) / M_PI, 3, spDC);

            lure_ang.y = BINANG_TO_RAD_ALT(lure_ang_Y_s);

            sp90.x = 0.0f;
            sp90.y = 0.0f;
            sp90.z = lure_act_speed;

            Matrix_rotateY(lure_ang.y, MTXMODE_NEW);

            if (lure_type == FS_LURE_SINKING) {
                Vec3f sp64;

                Matrix_Position(&sp90, &sp64);
                lure_act_vec.x = sp64.x;
                lure_act_vec.z = sp64.z;
                phi_f0 = 10.0f;
            } else {
                Matrix_Position(&sp90, &lure_act_vec);
                phi_f0 = 0.0f;
            }

            lure_vib_ya = 0.0f;

            if ((lure_type == FS_LURE_UNK) && CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                lure_act_vec.y = -2.0f;

                if ((lure_count & 1) != 0) {
                    lure_vib_ya = 0.5f;
                } else {
                    lure_vib_ya = -0.5f;
                }
            } else if (line_pos[LINE_SEG_COUNT - 1].y < (WATER_SURFACE_Y(play) + phi_f0)) {
                if (lure_type == FS_LURE_SINKING) {
                    Vec3f sp58 = this->actor.world.pos;

                    this->actor.prevPos = this->actor.world.pos = lure_pos;
                    Actor_BGcheck2(play, &this->actor, 15.0f, 30.0f, 30.0f,
                                            UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_6);
                    this->actor.world.pos = sp58;

                    lure_act_vec.y += -0.5f;
                    if (lure_act_vec.y < -1.0f) {
                        lure_act_vec.y = -1.0f;
                    }

                    if (lure_pos.y < (this->actor.floorHeight + 5.0f)) {
                        line_pos[LINE_SEG_COUNT - 1].y = lure_pos.y = this->actor.floorHeight + 5.0f;
                        lure_act_vec.y = 0.0f;
                    } else {
                        lure_action = 1;
                    }
                } else {
                    lure_act_vec.y = fabsf(line_pos[LINE_SEG_COUNT - 1].y - WATER_SURFACE_Y(play)) * 0.2f;
                    if (lure_act_vec.y > 1.5f) {
                        lure_act_vec.y = 1.5f;
                    }
                }
            }

            line_pos[LINE_SEG_COUNT - 1].x += lure_act_vec.x;
            line_pos[LINE_SEG_COUNT - 1].y += lure_act_vec.y;
            line_pos[LINE_SEG_COUNT - 1].z += lure_act_vec.z;

            if (line_pos[LINE_SEG_COUNT - 1].y > (spE4 + 6.0f)) {
                line_pos[LINE_SEG_COUNT - 1].y -= 5.0f;
            }

            lure_spd.x = lure_spd.y = lure_spd.z = lure_acc.y = 0.0f;

            if (CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                if (CHECK_BTN_ALL(input->cur.button, BTN_R)) {
                    line_cap += 1.5f;
                    Na_StartSystemSe_F(NA_SE_IT_FISHING_REEL_HIGH - SFX_FLAG);
                    add_calc2(&lure_hosei_s, 1000.0f, 1.0f, 2.0f);
                } else {
                    line_cap += hit_power;
                    Na_StartSystemSe_F(NA_SE_IT_FISHING_REEL_SLOW - SFX_FLAG);
                    add_calc2(&lure_hosei_s, 1000.0f, 1.0f, 0.2f);
                }

                if (line_pos[LINE_SEG_COUNT - 1].y > (WATER_SURFACE_Y(play) + 4.0f)) {
                    add_calc2(&line_G, 3.0f, 1.0f, 0.2f);
                } else {
                    add_calc2(&line_G, 1.0f, 1.0f, 0.2f);
                }
            } else {
                add_calc2(&line_G, 2.0f, 1.0f, 0.2f);
            }

            add_calc2(&lure_pos.x, line_pos[LINE_SEG_COUNT - 1].x, 1.0f, lure_hosei_s);
            add_calc2(&lure_pos.y, line_pos[LINE_SEG_COUNT - 1].y, 1.0f, lure_hosei_s);
            add_calc2(&lure_pos.z, line_pos[LINE_SEG_COUNT - 1].z, 1.0f, lure_hosei_s);

            if (lure_act_speed > 1.0f) {
                add_calc2(&lure_hosei_s, 1000.0f, 1.0f, 1.0f);
            }

            add_calc2(&lure_hosei_s, 1000.0f, 1.0f, 0.1f);

            if (line_cap >= 195.0f) {
                line_cap = 195.0f;
                lure_mode = 0;
                line_G = 520.0f;
                demo_mode = 3;
            }

            if ((lure_pos.y <= (WATER_SURFACE_Y(play) + 4.0f)) && (lure_pos.y >= (WATER_SURFACE_Y(play) - 4.0f))) {
                s16 timer = 63;

                if (CHECK_BTN_ALL(input->cur.button, BTN_A) || (lure_act_speed > 1.0f)) {
                    timer = 1;
                }

                if ((lure_count & timer) == 0) {
                    spA8 = lure_pos;
                    spA8.y = WATER_SURFACE_Y(play);
                    fs_hamon_ct(NULL, play->specialEffects, &spA8, 30.0f, 300.0f, 150, 90);
                }
            }
            break;

        case 4:
            if (this->unk_157 != 0) {
                this->unk_157--;
                line_cap += hit_power;
            }

            if (CHECK_BTN_ALL(input->cur.button, BTN_A)) {
                if ((SQ(lure_pos.x) + SQ(lure_pos.z)) > SQ(920.0f)) {
                    line_cap += (1.0f + (KREG_DEBUG(65) * 0.1f));
                } else {
                    line_cap += hit_power;
                }
                Na_StartSystemSe_F(NA_SE_IT_FISHING_REEL_SLOW - SFX_FLAG);
            }

            if ((lure_count & 0x1F) == 0) {
                if (lure_hook_up || (lure_type != FS_LURE_SINKING)) {
                    line_tensyon = 5;
                }
            }

            add_calc2(&line_G, 0.0f, 1.0f, 0.2f);
            break;

        case 5:
            line_scale = 0.0005000001f;
            line_pos[LINE_SEG_COUNT - 1].x = lure_pos.x;
            line_pos[LINE_SEG_COUNT - 1].y = lure_pos.y;
            line_pos[LINE_SEG_COUNT - 1].z = lure_pos.z;
            line_G = 2.0f;
            break;
    }
}

s32 fish_sibuki_set(Fishing* this, PlayState* play, u8 ignorePosCheck) {
    s16 i;
    s16 count;
    f32 scale;
    Vec3f pos;
    Vec3f vel;
    f32 speedXZ;
    f32 angle;

    if ((this->actor.world.pos.y < (WATER_SURFACE_Y(play) - 10.0f)) && !ignorePosCheck) {
        return false;
    }

    if (this->fishLength >= 60.0f) {
        // Unknown optimized-out code
        count = 0;
        scale = 0.0f;
    }

    //! @bug Probably intended to be an else-if.
    if (this->fishLength >= 50.0f) {
        // Unknown optimized-out code
        count = 0;
        scale = 0.0f;
    }

    //! @bug Probably intended to be an else-if.
    if (this->fishLength >= 40.0f) {
        count = 40;
        scale = 1.2f;
    } else {
        count = 30;
        scale = 1.0f;
    }

    for (i = 0; i < count; i++) {
        speedXZ = (rnd_f(1.5f) + 0.5f) * scale;
        angle = rnd_f(6.28f);

        vel.x = sinf(angle) * speedXZ;
        vel.z = cosf(angle) * speedXZ;
        vel.y = (rnd_f(3.0f) + 3.0f) * scale;

        pos = this->actor.world.pos;
        pos.x += vel.x * 3.0f;
        pos.y = WATER_SURFACE_Y(play);
        pos.z += vel.z * 3.0f;

        fs_mizu_ct(&this->actor.projectedPos, play->specialEffects, &pos, &vel,
                                (rnd_f(0.02f) + 0.025f) * scale);
    }

    pos = this->actor.world.pos;
    pos.y = WATER_SURFACE_Y(play);

    fs_hamon_ct(&this->actor.projectedPos, play->specialEffects, &pos, 100.0f, 800.0f, 150, 90);

    this->lilyTimer = 30;

    return true;
}

void fish_sibuki_set_S(Fishing* this, PlayState* play) {
    s16 count;
    s16 i;
    f32 scale;
    Vec3f pos;
    Vec3f vel;
    f32 speedXZ;
    f32 angle;

    if (this->fishLength >= 60.0f) {
        // Unknown optimized-out code
        count = 0;
        scale = 0.0f;
    }

    //! @bug Probably intended to be an else-if.
    if (this->fishLength >= 45.0f) {
        count = 30;
        scale = 0.5f;
    } else {
        count = 20;
        scale = 0.3f;
    }

    for (i = 0; i < count; i++) {
        speedXZ = (rnd_f(1.5f) + 0.5f) * scale;
        angle = rnd_f(6.28f);

        vel.x = sinf(angle) * speedXZ;
        vel.z = cosf(angle) * speedXZ;
        vel.y = rnd_f(2.0f) + 2.0f;

        pos = this->actor.world.pos;
        pos.x += (vel.x * 3.0f);
        pos.y += (vel.y * 3.0f);
        pos.z += (vel.z * 3.0f);

        fs_mizu_ct(&this->actor.projectedPos, play->specialEffects, &pos, &vel,
                                (rnd_f(0.02f) + 0.025f) * scale);
    }
}

void lure_find_check(Fishing* this, Input* input) {
    Vec3f lineVec;
    Vec3f sp28;
    f32 lineLengthSQ;

    lineVec.x = lure_pos.x - this->actor.world.pos.x;
    lineVec.y = lure_pos.y - this->actor.world.pos.y;
    lineVec.z = lure_pos.z - this->actor.world.pos.z;

    lineLengthSQ = SQ(lineVec.x) + SQ(lineVec.y) + SQ(lineVec.z);

    if ((lure_mode == 3) && (this->unk_1A2 == 0) && !lure_with_head) {
        Matrix_rotateY(BINANG_TO_RAD_ALT(-this->actor.shape.rot.y), MTXMODE_NEW);
        Matrix_Position(&lineVec, &sp28);

        if ((sp28.z > 0.0f) || (this->fishLength < 40.0f)) {
            if ((this->fishState == 7) && (lineLengthSQ < SQ(200.0f))) {
                this->fishState = 4;
                this->fishTargetPos = lure_pos;
                this->rotationStep = 28672.0f;
                this->speedTarget = 5.0f;
            } else {
                if ((CHECK_BTN_ALL(input->cur.button, BTN_A) || (lure_act_speed > 1.0f)) &&
                    (lineLengthSQ < SQ(120.0f))) {
                    this->fishState = 2;
                    this->unk_15E = 0;
                    this->timerArray[0] = 0;
                    this->timerArray[2] = (s16)rnd_f(100.0f) + 100;
                    this->perception = fish_set[this->actor.params - EN_FISH_PARAM].perception;
                    this->rotationStep = 0.0f;
                }

                if ((this->timerArray[1] == 0) && (lineLengthSQ < SQ(70.0f))) {
                    this->fishState = 2;
                    this->unk_15E = 0;
                    this->timerArray[0] = 0;
                    this->timerArray[2] = (s16)rnd_f(100.0f) + 100;
                    this->perception = fish_set[this->actor.params - EN_FISH_PARAM].perception;
                    this->rotationStep = 0.0f;
                }
            }
        }
    } else if ((lure_mode == 4) && lure_hook_up && (lineLengthSQ < SQ(100.0f)) && (this->fishState >= 10)) {
        this->fishStateNext = 0;
        this->fishState = 1;
        this->unk_1A4 = 1000;
        this->unk_1A2 = 100;
        this->timerArray[1] = 50;
    }

    if ((lure_type != FS_LURE_SINKING) && (lure_in_water != 0) && (this->fishLength > 60.0f) &&
        (lineLengthSQ < SQ(30.0f)) && (this->fishState >= 10)) {
        this->fishStateNext = 0;
        this->fishState = 1;
        this->unk_1A4 = 1000;
        this->unk_1A2 = 100;
        this->timerArray[1] = 50;
    }
}

void fish_water_se_set(Fishing* this, u8 outOfWater) {
    s16 sfxId;
    u8 length;

    if (this->isLoach == 0) {
        length = this->fishLength;
    } else {
        length = 2.0f * this->fishLength;
    }

    if (outOfWater == false) {
        if (length >= 50) {
            sfxId = NA_SE_EV_DIVE_INTO_WATER;
        } else if (length >= 40) {
            sfxId = NA_SE_EV_BOMB_DROP_WATER;
        } else {
            sfxId = NA_SE_EV_BOMB_DROP_WATER;
        }
    } else {
        if (length >= 50) {
            sfxId = NA_SE_EV_JUMP_OUT_WATER;
        } else if (length >= 40) {
            sfxId = NA_SE_EV_OUT_OF_WATER;
        } else {
            sfxId = NA_SE_EV_OUT_OF_WATER;
        }
    }

    Actor_SE_set(&this->actor, sfxId);
}

void fish_message_check(Fishing* this, PlayState* play) {
    if (AGE == LINK_AGE_CHILD) {
        if ((HIGH_SCORE(HS_FISHING) & HS_FISH_LENGTH_CHILD) != 0) {
            if (HIGH_SCORE(HS_FISHING) & HS_FISH_CHEAT_CHILD) {
                this->actor.textId = 0x40B1;
            } else {
                this->actor.textId = 0x4089;
            }
        } else {
            this->actor.textId = 0x40AE;
        }
    } else {
        if ((HIGH_SCORE(HS_FISHING) & HS_FISH_LENGTH_ADULT) != 0) {
            if (HIGH_SCORE(HS_FISHING) & HS_FISH_CHEAT_ADULT) {
                this->actor.textId = 0x40B1;
            } else {
                this->actor.textId = 0x4089;
            }
        } else {
            this->actor.textId = 0x40AE;
        }
    }

    if (!this->isAquariumMessage) {
        if (this->aquariumWaitTimer == 0) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;

            if (Actor_talk_check(&this->actor, play)) {
                FishRangeForMessage = fish_record;
                this->isAquariumMessage = true;
            } else {
                Actor_talk_request(&this->actor, play);
            }
        } else {
            this->aquariumWaitTimer--;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        }
    } else if (Actor_talk_end_check(&this->actor, play)) {
        this->isAquariumMessage = false;
        this->aquariumWaitTimer = 20;
    }
}

void Fishing_actor_move(Actor* thisx, PlayState* play2) {
    s16 i;
    s16 rotXYScale = 10;
    f32 distX;
    f32 distY;
    f32 distZ;
    f32 distToTarget;
    f32 multiplier;
    f32 chance;
    f32 playerSpeedMod;
    Vec3f multiVecSrc;
    Vec3f targetPosOffset;
    s16 rotXTarget;
    s16 rotYtarget;
    s16 rotZScale;
    u8 phi_v0_2;
    s16 spF6;
    s16 rotXScale;
    s16 rotXStep;
    s16 rotYScale;
    s16 rotYStep;
    Fishing* this = (Fishing*)thisx;
    PlayState* play = play2;
    Player* player = GET_PLAYER(play);
    Input* input = &play->state.input[0];
    f32 spD8;
    f32 phi_f0;
    f32 phi_f2;
    Vec3f bubblePos;

    this->actor.cullingVolumeDistance = 700.0f;
    this->actor.cullingVolumeScale = 50.0f;

    if (this->isLoach == 0) {
        playerSpeedMod = (player->actor.speed * 0.15f) + 0.25f;
    } else {
        playerSpeedMod = (player->actor.speed * 0.3f) + 0.25f;
    }

    if ((lure_time != 0) || (demo_camera_no != SUB_CAM_ID_DONE) ||
        ((player->actor.world.pos.z > 1150.0f) && (this->fishState != 100))) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        if (lure_mode != 0) {
            if (lure_cast_time == 0) {
                this->actor.focus.pos = lure_pos;
            } else if (lure_cast_time == 1) {
                demo_mode = 1;
                demo_eye_xp = 0.0f;
                camera_zoom_switch = 2;
            }
        }
        this->actor.focus.pos = this->actor.world.pos;
    }

    this->stateAndTimer++;

    for (i = 0; i < 4; i++) {
        if (this->timerArray[i] != 0) {
            this->timerArray[i]--;
        }
    }

    if (this->unk_1A4 != 0) {
        this->unk_1A4--;
    }

    if (this->unk_1A2 != 0) {
        this->unk_1A2--;
    }

    if (this->bumpTimer != 0) {
        this->bumpTimer--;
    }

    if (this->lilyTimer != 0) {
        this->lilyTimer--;
    }

    add_calc2(&this->fishLimbRotPhaseStep, this->unk_190, 1.0f, 0.2f);

    if (this->fishState == 6) {
        add_calc2(&this->fishLimbRotPhaseMag, this->unk_194, 0.2f, 200.0f);
    } else {
        phi_f0 = 1.0f;
        phi_f2 = 1.0f;
        if (this->actor.world.pos.y > WATER_SURFACE_Y(play)) {
            phi_f0 = (KREG_DEBUG(64) * 0.1f) + 1.5f;
            phi_f2 = 3.0f;
        }
        add_calc2(&this->fishLimbRotPhaseMag, this->unk_194 * phi_f0, 1.0f, 500.0f * phi_f2);
    }

    adds(&this->fishLimbDRotZDelta, 0, 5, 500);

    if (this->isLoach == 0) {
        Actor_set_scale(&this->actor, this->fishLength * 15.0f * 0.00001f);

        this->fishLimbRotPhase += this->fishLimbRotPhaseStep;

        this->fishLimb23RotYDelta = (s16)(cosf(this->fishLimbRotPhase) * this->fishLimbRotPhaseMag) + (*this).unk_16E;

        this->fishLimb4RotYDelta =
            (s16)(cosf(this->fishLimbRotPhase + -1.2f) * this->fishLimbRotPhaseMag * 1.6f) + (*this).unk_16E;
    } else {
        Actor_set_scale(&this->actor, this->fishLength * 65.0f * 0.000001f);

        this->actor.scale.x = this->actor.scale.z * 1.1f;
        this->actor.scale.y = this->actor.scale.z * 1.1f;

        this->fishLimbRotPhase += this->fishLimbRotPhaseStep * 0.8f;

        for (i = 0; i < 3; i++) {
            this->loachRotYDelta[i] =
                (s16)(cosf(this->fishLimbRotPhase + (i * 2.1f)) * this->fishLimbRotPhaseMag * 2.0f) + (*this).unk_16E;
        }

        this->fishLimb23RotYDelta = (cosf(this->fishLimbRotPhase + 0.4f) * (*this).fishLimbRotPhaseMag * 2.0f) * 0.6f;
    }

    distX = this->fishTargetPos.x - this->actor.world.pos.x;
    distY = this->fishTargetPos.y - this->actor.world.pos.y;
    distZ = this->fishTargetPos.z - this->actor.world.pos.z;

    rotYtarget = atans_table(distZ, distX);
    distToTarget = sqrtf(SQ(distX) + SQ(distZ));

    rotXTarget = atans_table(distToTarget, distY);
    distToTarget = sqrtf(SQ(distX) + SQ(distZ) + SQ(distY));

    if ((this->bumpTimer != 0) && (this->fishState != 2) && (this->fishState != 3) && (this->fishState != 4)) {
        if ((this->stateAndTimer & 0x40) != 0) {
            rotYtarget += 0x4000;
        } else {
            rotYtarget -= 0x4000;
        }
        if (((this->stateAndTimer + 0x20) & 0x40) != 0) {
            rotXTarget += 0x2000;
        } else {
            rotXTarget -= 0x2000;
        }
    }

    switch (this->fishState) {
        case 100:
            fish_message_check(this, play);

            this->actor.cullingVolumeDistance = 500.0f;
            this->actor.cullingVolumeScale = 300.0f;

            Light_point_ct(&this->lightInfo, (s16)this->actor.world.pos.x,
                                      (s16)this->actor.world.pos.y + 20.0f, (s16)this->actor.world.pos.z - 50.0f, 255,
                                      255, 255, 255);

            this->fishLength = fish_record;
            targetPosOffset.y = (f32)sin_s(play->gameplayFrames * 300) * 1;
            targetPosOffset.z = (f32)sin_s(play->gameplayFrames * 230) * 2;
            this->actor.world.pos.x = 130.0f;
            this->actor.world.pos.y = 55.0f + targetPosOffset.y;
            this->actor.world.pos.z = 1300.0f + targetPosOffset.z;
            this->actor.shape.rot.y = -0x8000;

            if ((this->actor.projectedPos.z < 200.0f) && (this->actor.projectedPos.z > 0.0f)) {
                bubblePos.x = rnd_fx(5.0f) + 130.0f;
                bubblePos.y = 40.0f;
                bubblePos.z = rnd_fx(5.0f) + 1280.0f;
                fs_bubble_ct(NULL, play->specialEffects, &bubblePos, rnd_f(0.02f) + 0.03f, 1);
            }

            adds(&this->fishLimbEFRotYDelta, (sin_s(this->stateAndTimer * 0x800) * 2500.0f) + 2500.0f, 2,
                           0x7D0);
            adds(&this->fishLimb89RotYDelta, sin_s(this->stateAndTimer * 0xA00) * 1500.0f, 2, 0x7D0);

            this->unk_190 = 0.3f;
            this->unk_194 = 1000.0f / 3.0f;
            return;

        case 10:
            this->fishTargetPos = this->actor.home.pos;

            add_calc2(&this->actor.speed, 2.0f, 1.0f, 0.5f);
            add_calc2(&this->rotationStep, 4096.0f, 1.0f, 256.0f);

            if (distToTarget < 40.0f) {
                this->fishState = 11;
                this->unk_190 = 0.4f;
                this->unk_194 = 500.0f;
            }

            lure_find_check(this, input);

            if (this->actor.xzDistToPlayer < (250.0f * playerSpeedMod)) {
                this->fishStateNext = this->fishState = 0;
                this->unk_1A4 = 1000;
                this->unk_1A2 = 200;
                this->timerArray[1] = 50;
            }
            break;

        case 11:
            this->fishTargetPos = this->actor.home.pos;

            add_calc2(&this->actor.speed, 0.0f, 1.0f, 0.05f);
            add_calc2(&this->rotationStep, 0.0f, 1.0f, 256.0f);

            if (distToTarget >= 40.0f) {
                this->fishState = 10;
                this->unk_190 = 1.0f;
                this->unk_194 = 2000.0f;
            }
            lure_find_check(this, input);

            if (this->actor.xzDistToPlayer < (250.0f * playerSpeedMod)) {
                this->fishStateNext = this->fishState = 0;
                this->unk_1A4 = 1000;
                this->unk_1A2 = 200;
                this->timerArray[1] = 50;
            }

            if (message_check(&play->msgCtx) == TEXT_STATE_NONE) {
                if ((z_common_data.save.dayTime >= CLOCK_TIME(18, 0)) &&
                    (z_common_data.save.dayTime <= CLOCK_TIME(18, 0) + 27)) {
                    this->fishState = 7;
                    this->timerArray[3] = (s16)rnd_f(150.0f) + 200;
                }
                if ((z_common_data.save.dayTime >= CLOCK_TIME(5, 30) - 1) &&
                    (z_common_data.save.dayTime < CLOCK_TIME(5, 30) + 27)) {
                    this->fishState = 7;
                    this->timerArray[3] = (s16)rnd_f(150.0f) + 200;
                }
            }

#if DEBUG_FEATURES
            if (KREG(15) != 0) {
                KREG(15) = 0;
                this->fishState = 7;
                this->timerArray[3] = (s16)rnd_f(150.0f) + 2000;
            }
#endif
            break;

        case 0:
            add_calc2(&this->actor.speed, 1.0f, 1.0f, 0.05f);
            add_calc2(&this->rotationStep, 0.0f, 1.0f, 256.0f);

            if (this->timerArray[0] == 0) {
                if (this->unk_1A4 == 0) {
                    this->fishState = this->fishStateNext = 10;
                } else {
                    this->fishState = 1;
                    this->timerArray[0] = (s16)rnd_f(30.0f) + 10;
                    this->fishTargetPos.x = rnd_fx(300.0f);
                    this->fishTargetPos.y = (WATER_SURFACE_Y(play) - 50.0f) - rnd_f(50.0f);
                    this->fishTargetPos.z = rnd_fx(300.0f);
                    this->unk_190 = 1.0f;
                    this->unk_194 = 2000.0f;
                }
            }

            if (lure_type == FS_LURE_SINKING) {
                lure_find_check(this, input);
            } else {
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            }
            break;

        case 1:
            if (this->isLoach == 1) {
                this->fishState = -1;
                this->unk_1A4 = 20000;
                this->unk_1A2 = 20000;
                this->fishTargetPos.x = 0.0f;
                this->fishTargetPos.y = -140.0f;
                this->fishTargetPos.z = 0.0f;
            } else {
                add_calc2(&this->rotationStep, 4096.0f, 1.0f, 256.0f);

                if ((this->actor.xzDistToPlayer < (250.0f * playerSpeedMod)) || (this->timerArray[1] != 0)) {
                    add_calc2(&this->rotationStep, 8192.0f, 1.0f, 768.0f);
                    add_calc2(&this->actor.speed, 4.2f, 1.0f, 0.75);
                    this->unk_190 = 1.2f;
                    this->unk_194 = 4000.0f;
                    this->timerArray[0] = 20;
                } else {
                    this->unk_190 = 1.0f;
                    this->unk_194 = 2000.0f;
                    add_calc2(&this->actor.speed, 1.5f, 1.0f, 0.1f);
                }

                if ((this->timerArray[0] == 0) || (distToTarget < 50.0f)) {
                    this->fishState = 0;
                    this->timerArray[0] = (s16)rnd_f(30.0f) + 3;
                    this->unk_190 = 1.0f;
                    this->unk_194 = 500.0f;
                }

                if (lure_type == FS_LURE_SINKING) {
                    lure_find_check(this, input);
                } else {
                    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                }
            }
            break;

        case -1: // loach state 1
            adds(&this->rotationTarget.x, 0, 0x14, 0x20);

            if ((this->actor.xzDistToPlayer < (250.0f * playerSpeedMod)) || (this->timerArray[1] != 0)) {
                add_calc2(&this->actor.speed, 3.0f, 1.0f, 0.75);
                this->unk_190 = 1.0f;
                this->timerArray[0] = 20;
                this->unk_194 = 4000.0f;
                add_calc2(&this->rotationStep, 4096.0f, 1.0f, 256.0f);

                if ((play->gameplayFrames % 32) == 0) {
                    this->fishTargetPos.x = rnd_fx(600.0f);
                    this->fishTargetPos.z = rnd_fx(600.0f);
                    this->fishTargetPos.y = -120.0f;
                }
            } else if (distToTarget > 50.0f) {
                this->unk_190 = 0.8f;
                this->unk_194 = 1500.0f;
                add_calc2(&this->actor.speed, 1.0f, 1.0f, 0.1f);
                add_calc2(&this->rotationStep, 2048.0f, 1.0f, 128.0f);
            } else {
                this->unk_190 = 0.4f;
                this->unk_194 = 500.0f;
                add_calc0(&this->actor.speed, 1.0f, 0.02f);
                add_calc2(&this->rotationStep, 0.0f, 1.0f, 256.0f);
            }

            if (this->unk_1A4 == 0) {
                this->fishState = 10;
                this->fishStateNext = 10;
            } else if ((KREG_DEBUG(2) != 0) || (((this->unk_1A4 & 0x7FF) == 0) && (this->unk_1A4 < 15000))) {
#if DEBUG_FEATURES
                KREG(2) = 0;
#endif
                this->fishState = -2;
                this->actor.world.rot.x = this->actor.shape.rot.x = 0;
                this->fishTargetPos.y = WATER_SURFACE_Y(play) + 10.0f;
                this->fishTargetPos.x = rnd_f(50.0f);
                this->fishTargetPos.z = rnd_f(50.0f);
            }

            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            break;

        case -2:
            if ((this->actor.xzDistToPlayer < (250.0f * playerSpeedMod)) || (this->timerArray[1] != 0)) {
                this->fishState = -1;
                this->fishTargetPos.y = -120.0f;
            } else {
                this->unk_190 = 0.6f;
                this->unk_194 = 1000.0f;

                adds(&this->rotationTarget.x, -0x1000, 0x14, 0x100);

                if (this->actor.world.pos.y < (WATER_SURFACE_Y(play) - 20.0f)) {
                    add_calc2(&this->actor.speed, 0.5f, 1.0f, 0.1f);
                } else {
                    add_calc0(&this->actor.speed, 1.0f, 0.01f);

                    if ((this->actor.speed == 0.0f) || (this->actor.world.pos.y > (WATER_SURFACE_Y(play) - 5.0f))) {
                        Vec3f spB8;

                        this->fishTargetPos.x = rnd_f(300.0f);
                        this->fishTargetPos.z = rnd_f(300.0f);
                        this->fishTargetPos.y = this->actor.floorHeight + 10.0f;
                        this->fishState = -25;
                        this->rotationStep = 0.0f;

                        spB8 = this->fishMouthPos;
                        spB8.y = WATER_SURFACE_Y(play);
                        fs_hamon_ct(&this->actor.projectedPos, play->specialEffects, &spB8, 10.0f, 300.0f, 150,
                                            90);
                        fs_hamon_ct(&this->actor.projectedPos, play->specialEffects, &spB8, 30.0f, 400.0f, 150,
                                            90);

                        Actor_SE_set(&this->actor, NA_SE_PL_CATCH_BOOMERANG);
                        break;
                    }
                }

                add_calc2(&this->rotationStep, 2048.0f, 1.0f, 128.0f);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            }
            break;

        case -25:
            if ((this->actor.xzDistToPlayer < (250.0f * playerSpeedMod)) || (this->timerArray[1] != 0)) {
                this->fishState = -1;
                this->fishTargetPos.y = -120.0f;
            } else {
                adds(&this->rotationTarget.x, 0x1000, 0x14, 0x6A);

                if (distToTarget > 40.0f) {
                    this->unk_190 = 0.7f;
                    this->unk_194 = 1200.0f;
                    add_calc2(&this->actor.speed, 0.5f, 1.0f, 0.01f);
                    add_calc2(&this->rotationStep, 2048.0f, 1.0f, 128.0f);
                } else {
                    this->fishState = -1;
                }
            }
            break;

        case 2:
            if (((this->actor.params + lure_search_p) & 1) != 0) {
                multiVecSrc.x = 10.0f;
            } else {
                multiVecSrc.x = -10.0f;
            }
            multiVecSrc.y = 0.0f;
            multiVecSrc.z = 0.0f;
            Matrix_rotateY(lure_ang.y, MTXMODE_NEW);
            Matrix_Position(&multiVecSrc, &targetPosOffset);

            this->fishTargetPos.x = lure_pos.x + targetPosOffset.x;
            this->fishTargetPos.z = lure_pos.z + targetPosOffset.z;

            if (lure_type == FS_LURE_SINKING) {
                this->fishTargetPos.y = lure_pos.y + 0.0f;
            } else if (this->isLoach == 0) {
                this->fishTargetPos.y = lure_pos.y - 15.0f;
            } else {
                this->fishTargetPos.y = lure_pos.y - 5.0f;
            }

            if (this->fishTargetPos.y <= this->actor.floorHeight) {
                this->fishTargetPos.y = this->actor.floorHeight + 3.0f;
            }

            if ((lure_type != FS_LURE_SINKING) && (this->actor.world.pos.y > this->fishTargetPos.y)) {
                add_calc2(&this->actor.world.pos.y, this->fishTargetPos.y, 0.1f,
                               (this->actor.world.pos.y - this->fishTargetPos.y) * 0.1f);
            }

            add_calc2(&this->rotationStep, 8192.0f, 1.0f, (KREG_DEBUG(16) * 128) + 384.0f);
            if (CHECK_BTN_ALL(input->press.button, BTN_A)) {
                this->perception += 0.005f;
            }

            if (lure_action != 0) {
                if (lure_action == 1) {
                    this->perception += 0.01f;
                } else {
                    this->perception += 0.05f;
                }
                lure_action = 0;
            }

            if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
                this->perception += 0.008f;
            }

            if (distToTarget < ((this->fishLength * 0.5f) + 20.0f)) {
                if (this->unk_15E == 0) {
                    this->unk_190 = 1.0f;
                    this->unk_194 = 500.0f;
                    this->timerArray[0] = (s16)rnd_f(10.0f) + 2;
                }
                add_calc2(&this->actor.speed, -0.2f, 1.0f, 0.1f);
                this->unk_15E = 1;
            } else {
                if (this->unk_15E != 0) {
                    this->unk_190 = 1.0f;
                    this->rotationStep = 0.0f;
                    this->unk_194 = 3000.0f;
                }
                add_calc2(&this->actor.speed, 3.0f, 1.0f, 0.15f);
                this->unk_15E = 0;
            }

            if (this->fishLength >= 60.0f) {
                multiplier = 0.3f;
            } else if (this->fishLength >= 45.0f) {
                multiplier = 0.6f;
            } else {
                multiplier = 1.0f;
            }

            if ((z_common_data.save.dayTime >= CLOCK_TIME(17, 0)) && (z_common_data.save.dayTime < CLOCK_TIME(19, 0))) {
                multiplier *= 1.75f;
            } else if ((z_common_data.save.dayTime >= CLOCK_TIME(5, 0)) &&
                       (z_common_data.save.dayTime < CLOCK_TIME(7, 0))) {
                multiplier *= 1.5f;
            } else if (mad_water != 0) {
                multiplier *= 1.5f;
            } else if ((u8)rain_S != 0) {
                multiplier *= 3.0f;
            }

            chance = 0.03f * multiplier;
            if (lure_type == FS_LURE_SINKING) {
                chance *= 5.0f;
            }

#if DEBUG_FEATURES
            if (((this->timerArray[0] == 1) || (fqrand() < chance)) &&
                ((fqrand() < (this->perception * multiplier)) || ((this->isLoach + 1) == KREG(69))))
#else
            if (((this->timerArray[0] == 1) || (fqrand() < chance)) &&
                (fqrand() < (this->perception * multiplier)))
#endif
            {
                if (this->isLoach == 0) {
                    this->fishState = 3;
                    this->unk_190 = 1.2f;
                    this->unk_194 = 5000.0f;
                    this->timerArray[0] = rnd_f(10.0f);
                } else {
                    this->fishState = -3;
                    this->unk_190 = 1.0f;
                    this->unk_194 = 3000.0f;
                    this->timerArray[0] = 40;
                }
                if (lure_type == FS_LURE_SINKING) {
                    this->speedTarget = rnd_f(1.5f) + 3.0f;
                } else {
                    this->speedTarget = rnd_f(1.5f) + 4.5f;
                }
            }

            if ((lure_mode != 3) || (this->timerArray[2] == 0) ||
                (sqrtf(SQ(this->actor.world.pos.x) + SQ(this->actor.world.pos.z)) > 800.0f)) {
                this->fishState = this->fishStateNext;
                this->timerArray[1] = (s16)rnd_f(30.0f) + 50;
                this->timerArray[0] = (s16)rnd_f(10.0f) + 5;
                this->unk_190 = 1.0f;
                this->rotationStep = 0.0f;
                this->unk_194 = 2000.0f;
            }

            if (this->actor.xzDistToPlayer < (100.0f * playerSpeedMod)) {
                this->fishStateNext = this->fishState = 0;
                this->unk_1A4 = 1000;
                this->unk_1A2 = 200;
                this->timerArray[1] = 50;
            }
            break;

        case 3:
            this->lilyTimer = 6;
            rotXYScale = 2;

            if ((((s16)player->actor.world.pos.x + lure_search_p) & 1) != 0) {
                multiVecSrc.x = 30.0f;
            } else {
                multiVecSrc.x = -30.0f;
            }
            multiVecSrc.y = 0.0f;
            multiVecSrc.z = 30.0f;

            Matrix_rotateY(lure_ang.y, MTXMODE_NEW);
            Matrix_Position(&multiVecSrc, &targetPosOffset);

            this->fishTargetPos.x = lure_pos.x + targetPosOffset.x;
            this->fishTargetPos.z = lure_pos.z + targetPosOffset.z;
            this->fishTargetPos.y = lure_pos.y - 10.0f;
            this->rotationStep = 4096.0f;
            add_calc2(&this->actor.speed, this->speedTarget * 0.8f, 1.0f, 1.0f);

            if ((lure_mode != 3) || (lure_pos.y > (WATER_SURFACE_Y(play) + 5.0f)) ||
                (sqrtf(SQ(lure_pos.x) + SQ(lure_pos.z)) > 800.0f)) {
                this->fishState = this->fishStateNext;
                this->timerArray[0] = 0;
                this->unk_190 = 1.0f;
                this->unk_194 = 2000.0f;
            } else if ((this->timerArray[0] == 0) || (distToTarget < 30.0f)) {
                this->fishState = 4;
                this->fishTargetPos = lure_pos;
                this->rotationStep = 16384.0f;
                this->unk_190 = 1.2f;
                this->unk_194 = 5000.0f;
                this->timerArray[0] = 20;
            }
            break;

        case 4:
            add_calc2(&this->rotationStep, 16384.0f, 1.0f, 4096.0f);
            adds(&this->fishLimbDRotZDelta, 0x4E20, 4, 0x1388);

            this->lilyTimer = 50;
            rotXYScale = 2;
            this->fishTargetPos = lure_pos;
            add_calc2(&this->actor.speed, this->speedTarget, 1.0f, 1.0f);

            if ((lure_mode != 3) || (this->timerArray[0] == 0) || (lure_pos.y > (WATER_SURFACE_Y(play) + 5.0f)) ||
                (sqrtf(SQ(lure_pos.x) + SQ(lure_pos.z)) > 800.0f)) {

                this->timerArray[0] = 0;
                this->fishState = this->fishStateNext;
                this->unk_190 = 1.0f;
                this->unk_194 = 2000.0f;
            } else if (distToTarget < 10.0f) {
                s16 timer;

                if (fish_sibuki_set(this, play, false)) {
                    fish_water_se_set(this, false);
                }

                this->fishState = 5;
                this->unk_190 = 1.2f;
                this->unk_194 = 5000.0f;
                this->timerArray[1] = 150;
                this->timerArray[0] = 0;
                this->timerArray[2] = 0;
                this->timerArray[3] = 120;

                lure_mode = 4;
                hit_fish_actor = this;
                fish_mouse_p.y = 500.0f - rnd_f(400.0f);

                // get rumble time based on size and lure used.
                if (lure_type == FS_LURE_SINKING) {
                    if (this->fishLength > 70.0f) {
                        timer = (s16)rnd_f(20.0f) + 10;
                    } else if (this->fishLength > 60.0f) {
                        timer = (s16)rnd_f(30.0f) + 20;
                    } else if (this->fishLength > 50.0f) {
                        timer = (s16)rnd_f(30.0f) + 30;
                    } else {
                        timer = (s16)rnd_f(40.0f) + 40;
                    }
                    lure_hook_time = timer;
                    vib_time = timer;
                    z_vibctl2_vib_force_set(0.0f, 60, timer * 3, 10);
                } else {
                    if (this->fishLength > 70.0f) {
                        timer = (s16)rnd_f(5.0f) + 10;
                    } else if (this->fishLength > 60.0f) {
                        timer = (s16)rnd_f(5.0f) + 15;
                    } else if (this->fishLength > 50.0f) {
                        timer = (s16)rnd_f(5.0f) + 17;
                    } else {
                        timer = (s16)rnd_f(5.0f) + 25;
                    }
                    lure_hook_time = timer;
                    vib_time = timer;
                    z_vibctl2_vib_force_set(0.0f, 180, timer * 3, 10);
                }

                lure_hook_up = 0;
                lure_hook_power = 100;
                fish_fight_time = 0;
            }
            break;

        case -3:
            this->lilyTimer = 50;
            this->fishTargetPos = lure_pos;
            add_calc2(&this->actor.speed, 2.0f, 1.0f, 1.0f);

            if ((lure_mode != 3) || (this->timerArray[0] == 0) || (lure_pos.y > (WATER_SURFACE_Y(play) + 5.0f)) ||
                (sqrtf(SQ(lure_pos.x) + SQ(lure_pos.z)) > 800.0f)) {

                this->timerArray[0] = 0;
                this->fishState = this->fishStateNext;
                this->unk_190 = 1.0f;
                this->unk_194 = 2000.0f;
            } else if (distToTarget < 10.0f) {
                if (lure_pos.y > (WATER_SURFACE_Y(play) - 10.0f)) {
                    Actor_SE_set(&this->actor, NA_SE_EV_JUMP_OUT_WATER);
                    Na_StartSystemSe_F(NA_SE_PL_CATCH_BOOMERANG);
                }

                fish_sibuki_set(this, play, false);
                this->fishState = 5;
                this->unk_190 = 1.2f;
                this->unk_194 = 5000.0f;
                this->timerArray[1] = 150;
                this->timerArray[0] = 0;
                this->timerArray[2] = 0;
                this->timerArray[3] = 120;

                lure_mode = 4;
                hit_fish_actor = this;

                if (lure_type == FS_LURE_SINKING) {
                    lure_hook_time = 30;
                    vib_time = 100;
                    z_vibctl2_vib_force_set(0.0f, 60, 90, 10);
                } else {
                    lure_hook_time = 30;
                    vib_time = 40;
                    z_vibctl2_vib_force_set(0.0f, 180, 90, 10);
                }

                lure_hook_up = false;
                lure_hook_power = 100;
                fish_fight_time = 0;
            }
            break;

        case 5:
            this->actor.cullingVolumeDistance = 1200.0f;
            this->actor.cullingVolumeScale = 200.0f;

            fish_fight_time++;
            PRINTF("HIT FISH %dcm\n", (u8)this->fishLength);

            adds(&this->fishLimbDRotZDelta, 0x2AF8, 4, 0xBB8);
            hit_fish_actor = this;
            adds(&player->actor.shape.rot.y, this->actor.yawTowardsPlayer + 0x8000, 5, 0x500);

            if (lure_hook_up == 0) {
                if ((worm_eat < 20) && ((lure_count & 3) == 0)) {
                    worm_eat++;
                }
            }

            if ((lure_hook_time != 0) && (lure_hook_up == 0)) { // pull the line to hook it
                if (((input->rel.stick_y < -50) && (old_stick_y > -40)) ||
                    CHECK_BTN_ALL(input->press.button, BTN_A)) {
                    f32 temp_f0;
                    s32 pad;
                    f32 rumbleStrength;

                    if (input->rel.stick_y < -50) {
                        temp_f0 = 40.0f - ((this->fishLength - 30.0f) * 1.333333f);
                        if (temp_f0 > 0.0f) {
                            this->unk_152 = temp_f0;
                            this->unk_154 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                            this->unk_156 = 1;
                        }
                    }

                    this->fishLimbRotPhaseStep = 1.7f;
                    this->fishLimbRotPhaseMag = 7000.0f;
                    lure_hook_up = 1;
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 8, NA_BGM_ENEMY);
                    lure_bgm_time = 0;

                    if (this->isLoach == 1) {
                        rumbleStrength = (this->fishLength * 3.0f) + 120.0f;
                    } else {
                        rumbleStrength = (2.0f * this->fishLength) + 120.0f;
                    }
                    if (rumbleStrength > 255.0f) {
                        rumbleStrength = 255.0f;
                    }

                    z_vibctl2_vib_force_set(0.0f, rumbleStrength, 120, 5);
                    vib_time = 40;
                    rod_hooking_impact = 10;
                    Na_StartSystemSe_F(NA_SE_IT_FISHING_HIT);
                }
            }

            if (this->actor.world.pos.y < WATER_SURFACE_Y(play)) {
                f32 rumbleStrength;

                if (this->timerArray[1] > 30) {
                    phi_v0_2 = 7;
                } else {
                    phi_v0_2 = 0xF;
                }

                if (((this->stateAndTimer & phi_v0_2) == 0) && (fqrand() < 0.75f) && (vib_time == 0)) {

                    if (this->fishLength >= 70.0f) {
                        rumbleStrength = 255.0f;
                    } else if (this->fishLength >= 60.0f) {
                        rumbleStrength = 230.0f;
                    } else if (this->fishLength >= 50.0f) {
                        rumbleStrength = 200.0f;
                    } else if (this->fishLength >= 40.0f) {
                        rumbleStrength = 170.0f;
                    } else {
                        rumbleStrength = 140.0f;
                    }

                    if (phi_v0_2 == 0xF) {
                        rumbleStrength *= 3.0f / 4.0f;
                    }

                    z_vibctl2_vib_force_set(0.0f, rumbleStrength, (s16)rnd_f(5.0f) + 10, 5);
                }

                if (this->timerArray[1] > 30) {
                    if (this->timerArray[0] == 0) {
                        u16 attempts;

                        multiVecSrc.x = 0.0f;
                        multiVecSrc.y = 0.0f;
                        multiVecSrc.z = 200.0f;

                        for (attempts = 0; attempts < 100; attempts++) {
                            Matrix_rotateY(rnd_fx(3.0f * M_PI / 4.0f) +
                                               BINANG_TO_RAD_ALT(this->actor.yawTowardsPlayer + 0x8000),
                                           MTXMODE_NEW);
                            Matrix_Position(&multiVecSrc, &targetPosOffset);

                            this->fishTargetPos.x = this->actor.world.pos.x + targetPosOffset.x;
                            this->fishTargetPos.z = this->actor.world.pos.z + targetPosOffset.z;

                            if ((SQ(this->fishTargetPos.x) + SQ(this->fishTargetPos.z)) < SQ(750.0f)) {
                                break;
                            }
                        }

                        if ((fqrand() < 0.1f) && (this->timerArray[3] == 0)) {
                            u8 rumbleStrength8;

                            if (this->fishLength >= 60.0f) {
                                rumbleStrength8 = 255;
                            } else if (this->fishLength >= 50.0f) {
                                rumbleStrength8 = 200;
                            } else {
                                rumbleStrength8 = 180;
                            }
                            z_vibctl2_vib_force_set(0.0f, rumbleStrength8, 90, 2);
                            this->timerArray[0] = 20;
                            this->timerArray[1] = 100;
                            this->timerArray[2] = 20;
                            this->timerArray[3] = 100;
                            this->fishTargetPos.y = 300.0f;
                            vib_time = 40;
                            lure_hook_power = (s16)rnd_f(30.0f) + 20;
                        } else {
                            this->timerArray[0] = (s16)rnd_f(10.0f) + 3;
                            this->timerArray[2] = 0;
                            this->fishTargetPos.y = -70.0f - rnd_f(150.0f);
                        }
                    }

                    if (this->timerArray[2] != 0) {
                        hit_power = 0.0f;
                        this->unk_190 = 1.6f;
                        this->unk_194 = 6000.0f;
                        add_calc2(&this->actor.speed, 7.5f, 1.0f, 1.0f);
                        adds(&this->fishLimbDRotZDelta, 20000, 2, 4000);
                    } else {
                        if ((lure_hook_up == 0) && (lure_type == FS_LURE_SINKING)) {
                            this->unk_190 = 1.0f;
                            this->unk_194 = 2000.0f;
                            add_calc2(&this->actor.speed, 3.0f, 1.0f, 0.2f);
                        } else {
                            this->unk_190 = 1.4f;
                            this->unk_194 = 5000.0f;
                            add_calc2(&this->actor.speed, 5.0f, 1.0f, 0.5f);
                        }

                        if (this->isLoach == 0) {
                            hit_power = 1.0f - (this->fishLength * 0.00899f);
                        } else {
                            hit_power = 1.0f - (this->fishLength * 0.00899f * 1.4f);
                        }
                    }
                } else {
                    if (((this->timerArray[1] & 0xF) == 0) && CHECK_BTN_ALL(input->cur.button, BTN_A) &&
                        (!(this->fishLength >= 60.0f) || (fish_fight_time >= 2000))) {
                        this->unk_152 = (s16)rnd_f(30.0f) + 15;
                        this->unk_154 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                    }

                    this->unk_190 = 1.0f;
                    this->unk_194 = 4500.0f;

                    if (this->isLoach == 0) {
                        hit_power = 1.3f - (this->fishLength * 0.00899f);
                    } else {
                        hit_power = 1.3f - (this->fishLength * 0.00899f * 1.4f);
                    }

                    add_calc2(&this->actor.speed, 2.0f, 1.0f, 0.5f);

                    if (this->timerArray[1] == 0) {
                        this->unk_152 = 0;

                        if (fish_fight_time < 2000) {
                            this->timerArray[1] = (s16)rnd_f(50.0f) + 50;
                        } else if (fish_fight_time < 3000) {
                            this->timerArray[1] = (s16)rnd_f(20.0f) + 30;
                        } else {
                            this->timerArray[1] = (s16)rnd_f(10.0f) + 25;
                        }
                    }
                }
            }

            if (CIC_ERROR != 0) {
                hit_power = 0.0f;
            }

            if (lure_hook_up || (lure_type != FS_LURE_SINKING)) {
                if (this->actor.speed < 3.0f) {
                    if ((lure_count & 8) != 0) {
                        targetPosOffset.x = -0.8f;
                    } else {
                        targetPosOffset.x = -0.75f;
                    }
                } else {
                    if ((lure_count & 4) != 0) {
                        targetPosOffset.x = -0.9f;
                    } else {
                        targetPosOffset.x = -0.85f;
                    }
                }

                add_calc2(&rod_base_curvX, 35.0f, 0.1f, 3.5f);
                add_calc2(&rod_curvX3, targetPosOffset.x, 0.3f, 0.1f);
            }

            line_pos[LINE_SEG_COUNT - 1] = this->fishMouthPos;
            multiVecSrc.x = line_pos[LINE_SEG_COUNT - 1].x - line_pos[LINE_SEG_COUNT - 2].x;
            multiVecSrc.y = line_pos[LINE_SEG_COUNT - 1].y - line_pos[LINE_SEG_COUNT - 2].y;
            multiVecSrc.z = line_pos[LINE_SEG_COUNT - 1].z - line_pos[LINE_SEG_COUNT - 2].z;

            if ((SQ(multiVecSrc.x) + SQ(multiVecSrc.y) + SQ(multiVecSrc.z)) > SQ(20.0f)) {
                add_calc2(&this->actor.world.pos.x, line_pos[LINE_SEG_COUNT - 2].x, 0.2f,
                               2.0f * (this->actor.speed * 1.5f));
                add_calc2(&this->actor.world.pos.y, line_pos[LINE_SEG_COUNT - 2].y, 0.2f,
                               2.0f * (this->actor.speed * 1.5f) * 5.0f * 0.1f);
                add_calc2(&this->actor.world.pos.z, line_pos[LINE_SEG_COUNT - 2].z, 0.2f,
                               2.0f * (this->actor.speed * 1.5f));
            }

            if (CHECK_BTN_ALL(input->cur.button, BTN_A) || (input->rel.stick_y < -30)) {
                if (lure_hook_power < 100) {
                    lure_hook_power++;
                }
            } else {
                if (lure_hook_power != 0) {
                    lure_hook_power--;
                }
            }

            if ((lure_mode < 3) || ((CIC_ERROR != 0) && (fish_fight_time > 50)) || (fish_fight_time >= 6000) ||
                ((lure_hook_time == 0) && (lure_hook_up == 0)) || (lure_hook_power == 0) ||
                (((lure_count & 0x7F) == 0) && (fqrand() < 0.05f) && (lure_type != FS_LURE_SINKING) &&
                 (KREG_DEBUG(69) == 0))) {
                fs_message_time = 20;

                if ((lure_hook_time == 0) && (lure_hook_up == 0)) {
                    fs_message_no = 0x4081;
                    if (((AGE == LINK_AGE_CHILD) && (HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_CHILD)) ||
                        ((AGE != LINK_AGE_CHILD) && (HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_ADULT))) {
                        fs_message_time = 0;
                    }
                } else {
                    fs_message_no = 0x4082;
                    z_vibctl2_vib_force_set(0.0f, 1, 3, 1);
                    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 10);
                }

                this->fishState = this->fishStateNext = 0;
                this->unk_1A4 = 10000;
                this->unk_1A2 = 500;
                this->timerArray[1] = 50;
                this->timerArray[0] = 0;
                this->unk_190 = 1.0f;
                this->unk_194 = 3000.0f;

                if (lure_mode == 4) {
                    lure_mode = 3;
                }

                lure_bgm_time = 50;
                hit_power = 0.5f;
                this->unk_152 = 0;
            } else if (this->actor.xzDistToPlayer < (KREG_DEBUG(59) + 50.0f)) {
                this->fishState = 6;
                this->timerArray[0] = 100;
                player->unk_860 = 3;
                z_vibctl2_vib_force_set(0.0f, 1, 3, 1);
                fish_catch_count++;
                Demo_play_start(play, &play->csCtx);
                demo_mode = 100;
                demo_xa = 45.0f;
                lure_mode = 5;
                this->unk_190 = 1.0f;
                this->unk_194 = 500.0f;
                this->fishLimbRotPhaseMag = 5000.0f;

                if (this->actor.world.pos.y <= WATER_SURFACE_Y(play)) {
                    fish_water_se_set(this, true);
                    fish_sibuki_set(this, play, true);
                }
                goto hoistCatch;
            }
            break;

        hoistCatch:
        case 6:
            adds(&this->fishLimbDRotZDelta, 11000, 2, 4000);
            add_calc2(&demo_xa, 15.0f, 0.05f, 0.75f);

            multiVecSrc.x = demo_xa + 0.0f;
            if (AGE != LINK_AGE_CHILD) {
                multiVecSrc.y = 30.0f;
                multiVecSrc.z = 55.0f;
            } else {
                multiVecSrc.y = 10.0f;
                multiVecSrc.z = 50.0f;
            }
            Matrix_rotateY(BINANG_TO_RAD_ALT(player->actor.shape.rot.y), MTXMODE_NEW);
            Matrix_Position(&multiVecSrc, &demo_eye);

            demo_eye.x += player->actor.world.pos.x;
            demo_eye.y += player->actor.world.pos.y;
            demo_eye.z += player->actor.world.pos.z;

            demo_way = player->actor.world.pos;
            if (AGE != LINK_AGE_CHILD) {
                demo_way.y += 40.0f;
            } else {
                demo_way.y += 25.0f;
            }

            if (this->timerArray[0] == 90) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 9, NA_BGM_HEART_GET);
                fs_message_time = 40;

                if (this->isLoach == 0) {
                    FishRangeForMessage = this->fishLength;

                    if (FishRangeForMessage >= 75) {
                        fs_message_no = 0x409F;
                    } else if (FishRangeForMessage >= 50) {
                        fs_message_no = 0x4091;
                    } else {
                        fs_message_no = 0x4083;
                    }
                } else {
                    FishRangeForMessage = 2.0f * this->fishLength;
                    fs_message_no = 0x4099;
                }

                this->keepState = 0;
            }

            this->unk_160 = -0x4000;
            this->actor.shape.rot.y = player->actor.shape.rot.y + 0x5000;
            this->actor.shape.rot.x = this->actor.shape.rot.z = this->unk_162 = this->unk_164 = this->unk_16E = 0;

            multiVecSrc.x = 4.0f;
            multiVecSrc.y = -10.0f;
            multiVecSrc.z = 5.0f;
            Matrix_Position(&multiVecSrc, &targetPosOffset);
            add_calc2(&this->actor.world.pos.x, player->bodyPartsPos[PLAYER_BODYPART_R_HAND].x + targetPosOffset.x,
                           1.0f, 6.0f);
            add_calc2(&this->actor.world.pos.y, player->bodyPartsPos[PLAYER_BODYPART_R_HAND].y + targetPosOffset.y,
                           1.0f, 6.0f);
            add_calc2(&this->actor.world.pos.z, player->bodyPartsPos[PLAYER_BODYPART_R_HAND].z + targetPosOffset.z,
                           1.0f, 6.0f);

            line_cap = 188.0f;

            if (this->timerArray[0] <= 50) {
                switch (this->keepState) {
                    case 0:
                        if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) ||
                            (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                            if (pad_on_check(play)) {
                                message_close(play);
                                if (play->msgCtx.choiceIndex == 0) {
                                    if (my_record == 0.0f) {
                                        my_record = this->fishLength;
                                        my_fish_kind = this->isLoach;
                                        use_lure = lure_type;
                                        Actor_delete(&this->actor);
                                    } else if ((this->isLoach == 0) && (my_fish_kind == 0) &&
                                               ((s16)my_record > (s16)this->fishLength)) {
                                        this->keepState = 1;
                                        this->timerArray[0] = 0x3C;

                                        message_set(play, 0x4098, NULL);
                                    } else {
                                        f32 lengthTemp = my_record;
                                        s16 loachTemp = my_fish_kind;
                                        my_record = this->fishLength;
                                        my_fish_kind = this->isLoach;
                                        use_lure = lure_type;
                                        this->fishLength = lengthTemp;
                                        this->isLoach = loachTemp;
                                    }
                                }
                                if (this->keepState == 0) {
                                    lure_mode = 0;
                                }
                            }
                        }
                        break;
                    case 1:
                        if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) ||
                            (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                            if (pad_on_check(play)) {
                                message_close(play);
                                if (play->msgCtx.choiceIndex != 0) {
                                    f32 temp1 = my_record;
                                    s16 temp2 = my_fish_kind;
                                    my_record = this->fishLength;
                                    use_lure = lure_type;
                                    this->fishLength = temp1;
                                    this->isLoach = temp2;
                                }
                                lure_mode = 0;
                            }
                        }
                        break;
                }
            }

            if (lure_mode == 0) {
                if (this->actor.update != NULL) {
                    this->fishState = this->fishStateNext = 0;
                    this->unk_1A4 = 10000;
                    this->unk_1A2 = 500;
                    this->timerArray[1] = 50;
                    this->timerArray[0] = 0;
                    this->unk_190 = 1.0f;
                    this->unk_194 = 2000.0f;
                    Skeleton_Info_dt(&this->skelAnime, play);

                    if (this->isLoach == 0) {
                        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gFishingFishSkel, &gFishingFishAnim, NULL, NULL, 0);
                        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gFishingFishAnim, 0.0f);
                    } else {
                        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gFishingLoachSkel, &gFishingLoachAnim, NULL, NULL,
                                           0);
                        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gFishingLoachAnim, 0.0f);
                    }
                }

                line_G = 520.0f;
                line_cap = 195.0f;
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 10);
                lure_bgm_time = 20;
                demo_mode = 3;
            }
            break;

        case 7:
            this->lilyTimer = 50;
            rotXYScale = 5;
            this->rotationStep = 12288.0f;

            if (this->actor.params < (EN_FISH_PARAM + 4)) {
                this->fishTargetPos = minnow[this->actor.params - EN_FISH_PARAM].pos;
                minnow_panic = 1;
            } else if (this->actor.params < (EN_FISH_PARAM + 8)) {
                this->fishTargetPos = minnow[this->actor.params - EN_FISH_PARAM + 16].pos;
                minnow_panic = 2;
            } else {
                this->fishTargetPos = minnow[this->actor.params - EN_FISH_PARAM + 32].pos;
                minnow_panic = 3;
            }

            add_calc2(&this->actor.speed, 5.0f, 1.0f, 1.0f);

            if (distToTarget < 20.0f) {
                adds(&this->fishLimbDRotZDelta, 20000, 2, 4000);

                if ((this->timerArray[2] == 0) && fish_sibuki_set(this, play, false)) {
                    fish_water_se_set(this, rnd_f(1.99f));
                    this->timerArray[2] = (s16)rnd_f(20.0f) + 20;
                }
            }

            if (this->timerArray[3] == 0) {
                this->fishState = 10;
                this->fishStateNext = 10;
            } else {
                lure_find_check(this, input);
                if (this->actor.xzDistToPlayer < (100.0f * playerSpeedMod)) {
                    this->fishStateNext = this->fishState = 0;
                    this->unk_1A4 = 500;
                    this->unk_1A2 = 200;
                    this->timerArray[1] = 50;
                }
            }
            break;
    }

    adds(&this->fishLimbEFRotYDelta, (sin_s(this->stateAndTimer * 0x1000) * 5000.0f) + 5000.0f, 2, 0x7D0);

    if (this->fishState != 6) {
        if (this->actor.world.pos.y > WATER_SURFACE_Y(play)) {
            this->unk_190 = 1.5f;
            this->unk_194 = 5000.0f;

            adds(&this->unk_16E, 0, 5, 0x7D0);

            rotXScale = rotYScale = rotZScale = 3;
            rotXStep = rotYStep = 0x2000;

            this->timerArray[2] = 0;
            this->unk_184 -= 1.0f;
        } else {
            add_calc0(&this->unk_184, 1.0f, 2.0f);
            // check for Loach states
            if ((this->fishState != -1) && (this->fishState != -2) && (this->fishState != -25)) {
                this->rotationTarget.x = 0;
            }

            this->rotationTarget.y = this->rotationTarget.z = 0;
            rotXScale = rotYScale = rotZScale = 4;
            rotXStep = rotYStep = 0x2000;

            spF6 = adds2(&this->actor.world.rot.y, rotYtarget, rotXYScale, this->rotationStep) * 3.0f;
            adds(&this->actor.world.rot.x, rotXTarget, rotXYScale, this->rotationStep * 0.5f);

            if (spF6 > 8000) {
                spF6 = 8000;
            } else if (spF6 < -8000) {
                spF6 = -8000;
            }

            if (this->actor.speed >= 3.2f) {
                adds(&this->unk_16E, spF6, 2, 20000);
            } else {
                adds(&this->unk_16E, spF6, 3, 3000);
            }

            Actor_position_speed_set_XY(&this->actor);
        }

        Actor_position_move(&this->actor);

        this->actor.world.pos.y += (this->unk_184 * 1.5f);

        if (this->unk_152 != 0) {
            this->rotationTarget.y = this->unk_154;
            this->unk_152--;
            if (this->unk_156 != 0) {
                rotYScale = 5;
                rotYStep = 0x4000;
            } else {
                rotYScale = 10;
                rotYStep = 0x800;
            }
            this->rotationTarget.x = -0x500 - this->actor.shape.rot.x;
            rotXScale = 5;
            rotXStep = 0x4000;
        } else {
            this->unk_156 = 0;
        }

        adds(&this->unk_160, this->rotationTarget.x, rotXScale, rotXStep);
        adds(&this->unk_162, this->rotationTarget.y, rotYScale, rotYStep);
        adds(&this->unk_164, this->rotationTarget.z, rotZScale, 0x2000);

        if (this->actor.speed <= 0.5f) {
            adds(&this->actor.shape.rot.x, 0, 10, this->unk_178);
            adds(&this->unk_178, 0x500, 1, 0x20);
        } else {
            adds(&this->actor.shape.rot.x, -this->actor.world.rot.x, 10, 0x1000);
            this->unk_178 = 0;
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;

        if ((this->fishState != -1) && (this->fishState != -2) && (this->fishState != -25)) {
            if ((this->actor.world.pos.y > WATER_SURFACE_Y(play)) && (this->actor.prevPos.y <= WATER_SURFACE_Y(play))) {
                fish_sibuki_set(this, play, true);
                fish_water_se_set(this, true);
                this->unk_184 = this->actor.velocity.y;
                this->actor.velocity.y = 0.0f;
                this->rotationTarget.z = rnd_fx(32768.0f);
            } else if ((this->actor.world.pos.y < WATER_SURFACE_Y(play)) &&
                       (this->actor.prevPos.y >= WATER_SURFACE_Y(play))) {
                if (this->unk_184 < -5.0f) {
                    this->unk_184 = -5.0f;
                }
                this->actor.world.rot.x = -0xFA0;
                fish_sibuki_set(this, play, true);
                this->bubbleTime = 20;
                fish_water_se_set(this, 0);
            }
        }

        if ((this->actor.world.pos.y < WATER_SURFACE_Y(play)) &&
            (this->actor.world.pos.y > (WATER_SURFACE_Y(play) - 10.0f))) {
            if (((this->stateAndTimer & 1) == 0) && (this->actor.speed > 0.0f)) {
                Vec3f pos = this->actor.world.pos;

                pos.y = WATER_SURFACE_Y(play);
                fs_hamon_ct(&this->actor.projectedPos, play->specialEffects, &pos, 80.0f, 500.0f, 150, 90);
            }
        }

        if ((this->actor.speed > 0.0f) || (this->fishState == 5)) {
            f32 velocityY = this->actor.velocity.y;

            spD8 = this->fishLength * 0.1f;

            this->actor.world.pos.y -= spD8;
            this->actor.prevPos.y -= spD8;
            this->actor.velocity.y = -1.0f;
            Actor_BGcheck2(play, &this->actor, 30.0f, 30.0f, 100.0f,
                                    UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_6);
            this->actor.world.pos.y += spD8;
            this->actor.prevPos.y += spD8;

            this->actor.velocity.y = velocityY;

            if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                this->bumpTimer = 20;
            }

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                if (this->actor.world.pos.y > WATER_SURFACE_Y(play)) {
                    this->unk_184 = rnd_f(3.0f) + 3.0f;
                    this->actor.velocity.x = this->actor.world.pos.x * -0.003f;
                    this->actor.velocity.z = this->actor.world.pos.z * -0.003f;

                    Actor_SE_set(&this->actor, NA_SE_EV_FISH_LEAP);
                    fish_sibuki_set_S(this, play);

                    if (fqrand() < 0.5f) {
                        this->rotationTarget.z = 0x4000;
                    } else {
                        this->rotationTarget.z = -0x4000;
                    }

                    if (fqrand() < 0.5f) {
                        this->rotationTarget.x = 0;
                    } else {
                        this->rotationTarget.x = (s16)rnd_fx(32.0f) + 0x8000;
                    }

                    this->rotationTarget.y = (s16)rnd_fx(16384.0f);
                    this->unk_190 = 1.0f;
                    this->unk_194 = 5000.0f;
                    this->fishLimbRotPhaseMag = 5000.0f;
                } else {
                    this->unk_184 = 0.0f;

                    if ((this->fishState == 5) && ((this->stateAndTimer & 1) == 0)) {
                        Vec3f pos;

                        pos.x = this->actor.world.pos.x + rnd_fx(10.0f);
                        pos.z = this->actor.world.pos.z + rnd_fx(10.0f);
                        pos.y = this->actor.floorHeight + 5.0f;
                        fs_smoke_ct(&this->actor.projectedPos, play->specialEffects, &pos,
                                               (this->fishLength * 0.005f) + 0.15f);
                    }
                }
            }
        }
    }

    if (this->bubbleTime != 0) {
        s16 i;
        Vec3f pos;
        f32 range = (this->fishLength * 0.075f) + 10.0f;

        this->bubbleTime--;

        for (i = 0; i < 2; i++) {
            pos.x = this->actor.world.pos.x + rnd_fx(range);
            pos.y = this->actor.world.pos.y + rnd_fx(range);
            pos.z = this->actor.world.pos.z + rnd_fx(range);
            fs_bubble_ct(&this->actor.projectedPos, play->specialEffects, &pos, rnd_f(0.035f) + 0.04f,
                                0);
        }
    }
}

s32 fish_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Fishing* this = (Fishing*)thisx;

    if (limbIndex == 0xD) {
        rot->z -= this->fishLimbDRotZDelta - 11000;
    } else if ((limbIndex == 2) || (limbIndex == 3)) {
        rot->y += this->fishLimb23RotYDelta;
    } else if (limbIndex == 4) {
        rot->y += this->fishLimb4RotYDelta;
    } else if (limbIndex == 0xE) {
        rot->y -= this->fishLimbEFRotYDelta;
    } else if (limbIndex == 0xF) {
        rot->y += this->fishLimbEFRotYDelta;
    } else if (limbIndex == 8) {
        rot->y += this->fishLimb89RotYDelta;
    } else if (limbIndex == 9) {
        rot->y -= this->fishLimb89RotYDelta;
    }

    return 0;
}

void fish_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    Fishing* this = (Fishing*)thisx;

    if (limbIndex == 0xD) {
        Matrix_Position(&fish_mouse_p, &this->fishMouthPos);
    }
}

s32 snake_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Fishing* this = (Fishing*)thisx;

    if (limbIndex == 3) {
        rot->y += this->loachRotYDelta[0];
    } else if (limbIndex == 4) {
        rot->y += this->loachRotYDelta[1];
    } else if (limbIndex == 5) {
        rot->y += this->loachRotYDelta[2];
    }

    return 0;
}

void snake_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f mouse_p = { 500.0f, 500.0f, 0.0f };
    Fishing* this = (Fishing*)thisx;

    if (limbIndex == 0xB) {
        Matrix_Position(&mouse_p, &this->fishMouthPos);
    }
}

void Fishing_actor_draw(Actor* thisx, PlayState* play) {
    Fishing* this = (Fishing*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);

    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y + (*this).unk_162), MTXMODE_APPLY);
    Matrix_rotateX(BINANG_TO_RAD_ALT(this->actor.shape.rot.x + (*this).unk_160), MTXMODE_APPLY);
    Matrix_rotateZ(BINANG_TO_RAD_ALT(this->actor.shape.rot.z + (*this).unk_164), MTXMODE_APPLY);
    Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);

    if (this->isLoach == 0) {
        Matrix_rotateY(BINANG_TO_RAD(this->fishLimb23RotYDelta) - (M_PI / 2), MTXMODE_APPLY);
        Matrix_translate(0.0f, 0.0f, this->fishLimb23RotYDelta * 10.0f * 0.01f, MTXMODE_APPLY);

        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              fish_draw_sub, fish_draw_sub2, this);
    } else {
        Matrix_translate(0.0f, 0.0f, 3000.0f, MTXMODE_APPLY);
        Matrix_rotateY(BINANG_TO_RAD(this->fishLimb23RotYDelta), MTXMODE_APPLY);
        Matrix_translate(0.0f, 0.0f, -3000.0f, MTXMODE_APPLY);
        Matrix_rotateY(-(M_PI / 2), MTXMODE_APPLY);

        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              snake_draw_sub, snake_draw_sub2, this);
    }
}

void ashi_obj_check(FishingProp* prop, Vec3f* entityPos) {
    f32 dx = prop->pos.x - entityPos->x;
    f32 dz = prop->pos.z - entityPos->z;
    f32 distXZ = sqrtf(SQ(dx) + SQ(dz));

    if (distXZ <= 20.0f) {
        prop->rotY = atanf_table(dz, dx);

        add_calc2(&prop->rotX, (20.0f - distXZ) * 0.03f, 0.2f, 0.2f);
    }
}

void lily_obj_check(FishingProp* prop, Vec3f* entityPos, u8 fishTimer) {
    f32 dx = prop->pos.x - entityPos->x;
    f32 dz = prop->pos.z - entityPos->z;
    f32 distXZ = sqrtf(SQ(dx) + SQ(dz));

    if (distXZ <= 40.0f) {
        adds(&prop->lilyPadAngle, atans_table(dz, dx), 10, 0x300);
    }

    if (fishTimer && (distXZ <= 60.0f)) {
        f32 heightTarget = 1.0f;

        if (fishTimer >= 21) {
            heightTarget = 1.5f;
        }

        add_calc2(&prop->lilyPadOffset, heightTarget, 0.1f, 0.2f);
    }
}

void Fishing_Obj_move(PlayState* play) {
    FishingProp* prop = &fishing_obj[0];
    Player* player = GET_PLAYER(play);
    Actor* actor;
    s16 i;

    for (i = 0; i < POND_PROP_COUNT; i++, prop++) {
        if (prop->type != FS_PROP_NONE) {
            prop->shouldDraw = false;
            prop->timer++;

            Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &prop->pos, &prop->projectedPos, &float_dammy);

            if ((prop->projectedPos.z < prop->drawDistance) &&
                (fabsf(prop->projectedPos.x) < (100.0f + prop->projectedPos.z))) {
                prop->shouldDraw = true;
            }

            if ((prop->projectedPos.z < 500.0f) && (fabsf(prop->projectedPos.x) < (100.0f + prop->projectedPos.z))) {
                if (prop->type == FS_PROP_REED) {
                    ashi_obj_check(prop, &player->actor.world.pos);

                    actor = play->actorCtx.actorLists[ACTORCAT_NPC].head;
                    while (actor != NULL) {
                        if (!((actor->id == ACTOR_FISHING) && (actor->params >= EN_FISH_PARAM))) {
                            actor = actor->next;
                        } else {
                            ashi_obj_check(prop, &actor->world.pos);
                            actor = actor->next;
                        }
                    }

                    add_calc0(&prop->rotX, 0.05f, 0.05f);
                } else if (prop->type == FS_PROP_LILY_PAD) {
                    lily_obj_check(prop, &player->actor.world.pos, 0);

                    actor = play->actorCtx.actorLists[ACTORCAT_NPC].head;
                    while (actor != NULL) {
                        if (!((actor->id == ACTOR_FISHING) && (actor->params >= EN_FISH_PARAM))) {
                            actor = actor->next;
                        } else {
                            lily_obj_check(prop, &actor->world.pos, ((Fishing*)actor)->lilyTimer);
                            actor = actor->next;
                        }
                    }

                    adds(&prop->lilyPadAngle, 0, 20, 80);
                    prop->pos.y =
                        WATER_SURFACE_Y(play) + 2.0f + (sin_s(prop->timer * 0x1000) * prop->lilyPadOffset);
                    add_calc0(&prop->lilyPadOffset, 0.1f, 0.02f);
                }
            }
        }
    }

    if (demo_camera_no == SUB_CAM_ID_DONE) {
        CollisionCheck_setOC(play, &play->colChkCtx, &master->collider.base);
    }
}

void Fishing_Obj_disp(PlayState* play) {
    u8 materialFlag = 0;
    FishingProp* prop = &fishing_obj[0];
    s16 i;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_fishing.c", 7704);

    Matrix_push();

    for (i = 0; i < POND_PROP_COUNT; i++, prop++) {
        if (prop->type == FS_PROP_REED) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingReedMaterialDL);
                materialFlag++;
            }

            if (prop->shouldDraw) {
                Matrix_translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_scale(prop->scale, prop->scale, prop->scale, MTXMODE_APPLY);
                Matrix_rotateY(prop->rotY, MTXMODE_APPLY);
                Matrix_rotateX(prop->rotX, MTXMODE_APPLY);
                Matrix_rotateY(prop->reedAngle, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 7726);
                gSPDisplayList(POLY_XLU_DISP++, gFishingReedModelDL);
            }
        }
    }

    materialFlag = 0;
    prop = &fishing_obj[0];
    for (i = 0; i < POND_PROP_COUNT; i++, prop++) {
        if (prop->type == FS_PROP_WOOD_POST) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_OPA_DISP++, gFishingWoodPostMaterialDL);
                materialFlag++;
            }

            if (prop->shouldDraw) {
                Matrix_translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_scale(prop->scale, prop->scale, prop->scale, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 7748);
                gSPDisplayList(POLY_OPA_DISP++, gFishingWoodPostModelDL);
            }
        }
    }

    materialFlag = 0;
    prop = &fishing_obj[0];
    for (i = 0; i < POND_PROP_COUNT; i++, prop++) {
        if (prop->type == FS_PROP_LILY_PAD) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gFishingLilyPadMaterialDL);
                materialFlag++;
            }

            if (prop->shouldDraw) {
                Matrix_translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_scale(prop->scale, 1.0f, prop->scale, MTXMODE_APPLY);
                Matrix_rotateY(BINANG_TO_RAD(prop->lilyPadAngle), MTXMODE_APPLY);
                Matrix_translate(0.0f, 0.0f, 20.0f, MTXMODE_APPLY);
                Matrix_rotateY(prop->rotY, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 7774);
                gSPDisplayList(POLY_XLU_DISP++, gFishingLilyPadModelDL);
            }
        }
    }

    materialFlag = 0;
    prop = &fishing_obj[0];
    for (i = 0; i < POND_PROP_COUNT; i++, prop++) {
        if (prop->type == FS_PROP_ROCK) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_OPA_DISP++, gFishingRockMaterialDL);
                materialFlag++;
            }

            if (prop->shouldDraw) {
                Matrix_translate(prop->pos.x, prop->pos.y, prop->pos.z, MTXMODE_NEW);
                Matrix_scale(prop->scale, prop->scale, prop->scale, MTXMODE_APPLY);
                Matrix_rotateY(prop->rotY, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 7798);
                gSPDisplayList(POLY_OPA_DISP++, gFishingRockModelDL);
            }
        }
    }

    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_fishing.c", 7805);
}

void Minnow_move(PlayState* play) {
    s16 groupContactFlags = 0;
    Player* player = GET_PLAYER(play);
    FishingGroupFish* fish = &minnow[0];
    f32 dy;
    f32 dx;
    f32 dist;
    f32 dz;
    f32 offset;
    s16 groupIndex;
    s16 groupFlag;
    f32 spD8;
    s16 spD6;
    s16 spD4;
    s16 target;
    s16 i;
    Vec3f basePos[3];
    Vec3f ripplePos;
    Vec3f* refPos;
    f32 temp1;
    f32 temp2;

    if ((lure_in_water != 0) || (lure_mode == 4)) {
        refPos = &lure_pos;
    } else {
        refPos = &player->actor.world.pos;
    }

    basePos[0].x = sinf(minnow_home_angle_Y1) * 720.0f;
    basePos[0].y = -35.0f;
    basePos[0].z = cosf(minnow_home_angle_Y1) * 720.0f;

    dx = refPos->x - basePos[0].x;
    dz = refPos->z - basePos[0].z;

    if ((SQ(dx) + SQ(dz)) < SQ(50.0f)) {
        minnow_home_angle_Y1 += 0.3f;
        groupContactFlags |= 1;
    } else if (minnow_panic != 0.0f) {
        minnow_home_angle_Y1 += 0.05f;
        basePos[0].y = WATER_SURFACE_Y(play) - 5.0f;
    } else {
        add_calc2(&minnow_home_angle_Y1, 0.7f, 1.0f, 0.001f);
    }

    basePos[1].x = sinf(minnow_home_angle_Y2) * 720.0f;
    basePos[1].y = -35.0f;
    basePos[1].z = cosf(minnow_home_angle_Y2) * 720.0f;

    dx = refPos->x - basePos[1].x;
    dz = refPos->z - basePos[1].z;

    if ((SQ(dx) + SQ(dz)) < SQ(50.0f)) {
        minnow_home_angle_Y2 -= 0.3f;
        groupContactFlags |= 2;
    } else if (minnow_panic != 0.0f) {
        minnow_home_angle_Y2 -= 0.05f;
        basePos[1].y = WATER_SURFACE_Y(play) - 5.0f;
    } else {
        add_calc2(&minnow_home_angle_Y2, 2.3f, 1.0f, 0.001f);
    }

    basePos[2].x = sinf(minnow_home_angle_Y3) * 720.0f;
    basePos[2].y = -35.0f;
    basePos[2].z = cosf(minnow_home_angle_Y3) * 720.0f;

    dx = refPos->x - basePos[2].x;
    dz = refPos->z - basePos[2].z;

    if ((SQ(dx) + SQ(dz)) < SQ(50.0f)) {
        minnow_home_angle_Y3 -= 0.3f;
        groupContactFlags |= 4;
    } else if (minnow_panic != 0.0f) {
        minnow_home_angle_Y3 -= 0.05f;
        basePos[2].y = WATER_SURFACE_Y(play) - 5.0f;
    } else {
        add_calc2(&minnow_home_angle_Y3, 4.6f, 1.0f, 0.001f);
    }

    if (AGE == LINK_AGE_CHILD) {
        spD8 = 0.8f;
    } else {
        spD8 = 1.0f;
    }

    for (i = 0; i < GROUP_FISH_COUNT; i++, fish++) {
        if (fish->type != FS_GROUP_FISH_NONE) {
            fish->timer++;

            Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &fish->pos, &fish->projectedPos, &float_dammy);

            if ((fish->projectedPos.z < 400.0f) && (fabsf(fish->projectedPos.x) < (100.0f + fish->projectedPos.z))) {
                fish->shouldDraw = true;
            } else {
                fish->shouldDraw = false;
            }

            if (i <= 20) {
                groupIndex = 0;
                groupFlag = 1;
            } else if (i <= 40) {
                groupIndex = 1;
                groupFlag = 2;
            } else {
                groupIndex = 2;
                groupFlag = 4;
            }

            dx = fish->homePos.x - fish->pos.x;
            dy = fish->homePos.y - fish->pos.y;
            dz = fish->homePos.z - fish->pos.z;
            spD4 = atans_table(dz, dx);
            dist = sqrtf(SQ(dx) + SQ(dz));
            spD6 = atans_table(dist, dy);

            if ((dist < 10.0f) || (((fish->timer % 32) == 0) && (fqrand() > 0.5f))) {
                fish->homePos.y = basePos[groupIndex].y + rnd_fx(10.0f);

                if (minnow_panic != 0.0f) {
                    fish->homePos.x = basePos[groupIndex].x + rnd_fx(200.0f);
                    fish->homePos.z = basePos[groupIndex].z + rnd_fx(200.0f);
                } else {
                    fish->homePos.x = basePos[groupIndex].x + rnd_fx(100.0f);
                    fish->homePos.z = basePos[groupIndex].z + rnd_fx(100.0f);
                }

                ripplePos = fish->pos;
                ripplePos.y = WATER_SURFACE_Y(play);
                fs_hamon_ct(&fish->projectedPos, play->specialEffects, &ripplePos, 20.0f,
                                    rnd_f(50.0f) + 100.0f, 150, 90);

                if (fish->velY < 1.5f) {
                    fish->velY = 1.5f;
                }

                fish->unk_34 = 1.5f;
                fish->unk_38 = 1.0f;
            }

            target = adds2(&fish->unk_3E, spD4, 5, 0x4000) * 3.0f;
            if (target > 8000) {
                target = 8000;
            } else if (target < -8000) {
                target = -8000;
            }

            adds(&fish->unk_42, target, 3, 5000);

            offset = fish->unk_42 * -0.0001f;
            adds(&fish->unk_3C, spD6, 5, 0x4000);

            if (groupContactFlags & groupFlag) {
                fish->unk_38 = 1.0f;
                fish->velY = 6.0f;
                fish->unk_34 = 2.0f;
            }

            if (minnow_panic != 0.0f) {
                fish->unk_38 = 1.0f;
                fish->velY = 4.0f;
                fish->unk_34 = 2.0f;
            }

            add_calc2(&fish->velY, 0.75f, 1.0f, 0.05f);

            temp1 = fish->velY * spD8;
            temp2 = temp1 * cos_s(fish->unk_3C);

            fish->pos.x += temp2 * sin_s(fish->unk_3E);
            fish->pos.y += temp1 * sin_s(fish->unk_3C);
            fish->pos.z += temp2 * cos_s(fish->unk_3E);

            if (fish->shouldDraw) {
                add_calc2(&fish->unk_34, 1.0f, 1.0f, 0.1f);
                add_calc2(&fish->unk_38, 0.4f, 1.0f, 0.04f);
                fish->unk_30 += fish->unk_34;
                fish->scaleX = (cosf(fish->unk_30) * fish->unk_38) + offset;
            }
        }
    }

    minnow_panic = 0.0f;
}

void Minnow_disp(PlayState* play) {
    u8 materialFlag = 0;
    FishingGroupFish* fish = &minnow[0];
    f32 scale;
    s16 i;
    s32 pad;

    if (AGE == LINK_AGE_CHILD) {
        scale = 0.003325f;
    } else {
        scale = 0.00475f;
    }

    OPEN_DISPS(play->state.gfxCtx, "../z_fishing.c", 8048);

    for (i = 0; i < GROUP_FISH_COUNT; i++, fish++) {
        if (fish->type != FS_GROUP_FISH_NONE) {
            if (!materialFlag) {
                gSPDisplayList(POLY_OPA_DISP++, gFishingGroupFishMaterialDL);
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 155, 155, 155, 255);
                materialFlag++;
            }

            if (fish->shouldDraw) {
                Matrix_translate(fish->pos.x, fish->pos.y, fish->pos.z, MTXMODE_NEW);
                Matrix_rotateY(BINANG_TO_RAD_ALT2((f32)fish->unk_3E), MTXMODE_APPLY);
                Matrix_rotateX(BINANG_TO_RAD_ALT2(-(f32)fish->unk_3C), MTXMODE_APPLY);
                Matrix_scale(scale * fish->scaleX, scale, scale, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 8093);
                gSPDisplayList(POLY_OPA_DISP++, gFishingGroupFishModelDL);
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_fishing.c", 8099);
}

#include "z_fishing_oyaji.inc.c"

static s16 bm_eye_pt[] = { 0, 1, 2, 2, 1 };

static Vec3f wall_pos = { 670.0f, 0.0f, -600.0f };

static Vec3s worm_set_pos[] = {
    { -364, -30, -269 }, // in the pond, log past the lilies.
    { 1129, 3, -855 },   // rock next to stream
    { -480, 0, -1055 },  // wall opposite of entrance
    { 553, -48, -508 },  // tip of log beside 3 posts
};

void bm_move(Actor* thisx, PlayState* play2) {
    static Vec3f static_wall_pos;
    PlayState* play = play2;
    Fishing* this = (Fishing*)thisx;
    Vec3f multiVecSrc;
    Vec3f eyeTarget;
    Vec3f lureDist;
    s16 headRotTarget;
    s16 playerShadowAlpha;
    f32 target;
    f32 subCamAtMaxVelFrac;
    f32 lureDistXZ;
    Camera* mainCam;
    Player* player = GET_PLAYER(play);
    Input* input = &play->state.input[0];

#if DEBUG_FEATURES
    if (0) {
        // Strings existing only in rodata
        PRINTF_COLOR_GREEN();
        PRINTF_COLOR_YELLOW();
        PRINTF("plays %x\n");
        PRINTF("ys %x\n");
        PRINTF_RST();
    }
#endif

    playerShadowAlpha = player->actor.shape.shadowAlpha;

    if ((SQ(player->actor.world.pos.x) + SQ(player->actor.world.pos.z)) < SQ(920.0f)) {
        adds(&playerShadowAlpha, 0, 1, 40);
    } else {
        adds(&playerShadowAlpha, 200, 1, 40);
    }

    player->actor.shape.shadowAlpha = playerShadowAlpha;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((head_check != 0) || (message_check(&play->msgCtx) != TEXT_STATE_NONE)) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
    }

    if ((this->actor.xzDistToPlayer < 120.0f) || (message_check(&play->msgCtx) != TEXT_STATE_NONE)) {
        headRotTarget = this->actor.shape.rot.y - this->actor.yawTowardsPlayer;
    } else {
        headRotTarget = 0;
    }

    if (headRotTarget > 10000) {
        headRotTarget = 10000;
    } else if (headRotTarget < -10000) {
        headRotTarget = -10000;
    }

    adds(&this->unk_164, headRotTarget, 3, 5000);

    if (((play->gameplayFrames % 32) == 0) && (fqrand() < 0.3f)) {
        this->unk_162 = 4;
    }

    this->unk_160 = bm_eye_pt[this->unk_162];

    if (this->unk_162 != 0) {
        this->unk_162--;
    }

    if (head_check != 0) {
        head_check--;
    }

    // steal the owner's hat
    if (!lure_with_head && (lure_type != FS_LURE_SINKING) && (lure_mode > 0) &&
        (bm_head_no == FS_OWNER_CAPPED) && (head_check == 0)) {
        f32 dx = bm_head_pos.x - lure_pos.x;
        f32 dy = bm_head_pos.y - lure_pos.y;
        f32 dz = bm_head_pos.z - lure_pos.z;

        if ((sqrtf(SQ(dx) + SQ(dy) + SQ(dz)) < 25.0f) || (KREG_DEBUG(77) > 0)) {
#if DEBUG_FEATURES
            KREG(77) = 0;
#endif
            bm_head_no = FS_OWNER_BALD;
            lure_with_head = true;
            message_set(play, 0x4087, NULL);
        }
    }

    // update hat flag.
    if (bm_head_no == FS_OWNER_BALD) {
        HIGH_SCORE(HS_FISHING) |= HS_FISH_STOLE_HAT;
    } else if (bm_head_no == FS_OWNER_CAPPED) {
        HIGH_SCORE(HS_FISHING) &= ~HS_FISH_STOLE_HAT;
    }

#if DEBUG_FEATURES
    if (KREG(77) < 0) {
        KREG(77) = 0;
        lure_with_head_off = true;
    }
#endif

    if (fs_message_time != 0) {
        fs_message_time--;
        if (fs_message_time == 0) {
            message_set(play, fs_message_no, NULL);
        }
    }

    bm_message_check(this, play);

    line_scale = 0.0015f;
    fishing_time++;

    if ((fishing_game_mode != 0) && rod_disp) {
        lure_move(this, play);
    }

    Fishing_Eff_move(play->specialEffects, play);
    Fishing_Obj_move(play);
    Minnow_move(play);
    // can't leave with the rod
    if ((fishing_game_mode != 0) && (demo_mode == 0) && (player->actor.world.pos.z > 1360.0f) &&
        (fabsf(player->actor.world.pos.x) < 25.0f)) {
        player->actor.world.pos.z = 1360.0f;
        player->actor.speed = 0.0f;

        if (door_check_time == 0) {
            demo_mode = 10;
        }
    }

    // check if can/have found sinking lure.
    if ((worm_set_P != 0) &&
        (fabsf(player->actor.world.pos.x - worm_set_pos[worm_set_P - 1].x) < 25.0f) &&
        (fabsf(player->actor.world.pos.y - worm_set_pos[worm_set_P - 1].y) < 10.0f) &&
        (fabsf(player->actor.world.pos.z - worm_set_pos[worm_set_P - 1].z) < 25.0f)) {
        worm_set_P = 0;
        demo_mode = 20;
        z_vibctl2_vib_force_set(0.0f, 150, 10, 10);
        Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 20);
    }

#if DEBUG_FEATURES
    if (KREG(0) != 0) {
        s32 pad[3];

        KREG(0) = 0;
        lure_type = FS_LURE_STOCK;
        demo_mode = 20;
        z_vibctl2_vib_force_set(0.0f, 150, 10, 10);
        Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 20);
    }
#endif

    if (door_check_time != 0) {
        door_check_time--;
    }

    // handle cinematics (i.e.: catching fish, finding sinking lure)
    switch (demo_mode) {
        case 0:
            break;

        case 1:
            demo_camera_no = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, demo_camera_no, CAM_STAT_ACTIVE);
            mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
            demo_eye.x = mainCam->eye.x;
            demo_eye.y = mainCam->eye.y;
            demo_eye.z = mainCam->eye.z;
            demo_way.x = mainCam->at.x;
            demo_way.y = mainCam->at.y;
            demo_way.z = mainCam->at.z;
            demo_mode = 2;
            alpha_change(HUD_VISIBILITY_A_B_MINIMAP);
            demo_morf = 0.0f;
            FALLTHROUGH;

        case 2:
            shrink_window_setval(27);

            lureDist.x = lure_pos.x - player->actor.world.pos.x;
            lureDist.z = lure_pos.z - player->actor.world.pos.z;
            lureDistXZ = sqrtf(SQXZ(lureDist));
            Matrix_rotateY(atanf_table(lureDist.z, lureDist.x), MTXMODE_NEW);

            multiVecSrc.x = 0.0f;
            multiVecSrc.y = 0.0f;
            multiVecSrc.z = 100.0f;
            Matrix_Position(&multiVecSrc, &lureDist);

            if (lure_mode == 1) {
                subCamAtMaxVelFrac = 0.2f;
            } else {
                subCamAtMaxVelFrac = 0.1f;
            }

            add_calc2(&demo_way.x, lure_pos.x, subCamAtMaxVelFrac, fabsf(lureDist.x) * demo_morf);
            add_calc2(&demo_way.y, lure_pos.y, subCamAtMaxVelFrac, 50.0f * demo_morf);
            add_calc2(&demo_way.z, lure_pos.z, subCamAtMaxVelFrac, fabsf(lureDist.z) * demo_morf);

            multiVecSrc.x = -30.0f;
            multiVecSrc.x = 0.0f - demo_eye_xp;
            if (AGE != LINK_AGE_CHILD) {
                multiVecSrc.y = 80.0f;
            } else {
                multiVecSrc.y = 55.0f;
            }
            multiVecSrc.z = -80.0f;

            Matrix_Position(&multiVecSrc, &eyeTarget);
            eyeTarget.x += player->actor.world.pos.x;
            eyeTarget.y += player->actor.world.pos.y;
            eyeTarget.z += player->actor.world.pos.z;

            add_calc2(&demo_eye_xp, 30.0f, 0.1f, 0.4f);

            if (CHECK_BTN_ALL(input->press.button, BTN_Z)) { // zoom in/out from the lure
                if ((camera_zoom_switch >= 0) && (lure_hook_time == 0)) {
                    camera_zoom_switch++;

                    if (camera_zoom_switch >= 4) {
                        camera_zoom_switch = 0;
                    }

                    if ((camera_zoom_switch == 0) || (camera_zoom_switch == 3)) {
                        Na_StartSystemSe_F(NA_SE_SY_CAMERA_ZOOM_DOWN);
                    } else {
                        Na_StartSystemSe_F(NA_SE_SY_CAMERA_ZOOM_UP);
                    }
                }
            }

            if (lure_mode >= 3) {
                if (lureDistXZ < 110.0f) {
                    camera_zoom_switch = -1;
                } else if ((lureDistXZ > 300.0f) && (camera_zoom_switch < 0)) {
                    camera_zoom_switch = 0;
                }
            }

            if (camera_zoom_switch > 0) {
                f32 dist;
                f32 offset;
                f32 factor;

                dist = sqrtf(SQ(lureDist.x) + SQ(lureDist.z)) * 0.001f;
                if (dist > 1.0f) {
                    dist = 1.0f;
                }
                if (camera_zoom_switch == 2) {
                    offset = 0.3f;
                } else {
                    offset = 0.1f;
                }
                factor = 0.4f + offset + (dist * 0.4f);

                eyeTarget.x += (lure_pos.x - eyeTarget.x) * factor;
                eyeTarget.y += ((lure_pos.y - eyeTarget.y) * factor) + 20.0f;
                eyeTarget.z += (lure_pos.z - eyeTarget.z) * factor;
                line_scale = 0.0005000001f;
            }

            multiVecSrc.x = 0.0f;
            multiVecSrc.y = 0.0f;
            multiVecSrc.z = 100.0f;
            Matrix_Position(&multiVecSrc, &lureDist);

            add_calc2(&demo_eye.x, eyeTarget.x, 0.3f, fabsf(lureDist.x) * demo_morf);
            add_calc2(&demo_eye.y, eyeTarget.y, 0.3f, 20.0f * demo_morf);
            add_calc2(&demo_eye.z, eyeTarget.z, 0.3f, fabsf(lureDist.z) * demo_morf);
            break;

        case 3: {
            Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

            mainCam->eye = demo_eye;
            mainCam->eyeNext = demo_eye;
            mainCam->at = demo_way;
        }
            Gama_play_shift2main_camera(play, demo_camera_no, 0);
            Demo_play_end(play, &play->csCtx);
            demo_mode = 0;
            demo_camera_no = SUB_CAM_ID_DONE;
            water_in_kankyo(play, 0);
            play->envCtx.adjFogNear = 0;
            player->unk_860 = -5;
            lure_time = 5;
            break;

        case 10: // owner tells you to return the rod.
            Demo_play_start(play, &play->csCtx);
            demo_camera_no = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, demo_camera_no, CAM_STAT_ACTIVE);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_5);
            mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
            demo_eye.x = mainCam->eye.x;
            demo_eye.y = mainCam->eye.y;
            demo_eye.z = mainCam->eye.z;
            demo_way.x = mainCam->at.x;
            demo_way.y = mainCam->at.y;
            demo_way.z = mainCam->at.z;
            message_set(play, 0x409E, NULL);
            demo_mode = 11;
            z_vibctl2_vib_force_set(0.0f, 150, 10, 10);
            FALLTHROUGH;

        case 11:
            player->actor.world.pos.z = 1360.0f;
            player->actor.speed = 0.0f;

            if (message_check(&play->msgCtx) == TEXT_STATE_NONE) {
                Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

                mainCam->eye = demo_eye;
                mainCam->eyeNext = demo_eye;
                mainCam->at = demo_way;
                Gama_play_shift2main_camera(play, demo_camera_no, 0);
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                demo_mode = 0;

                demo_camera_no = SUB_CAM_ID_DONE;
                door_check_time = 30;
                water_in_kankyo(play, 0);
                play->envCtx.adjFogNear = 0;
            }
            break;

        case 20: // found the sinking lure
            Demo_play_start(play, &play->csCtx);
            demo_camera_no = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, demo_camera_no, CAM_STAT_ACTIVE);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_5);
            mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
            demo_eye.x = mainCam->eye.x;
            demo_eye.y = mainCam->eye.y;
            demo_eye.z = mainCam->eye.z;
            demo_way.x = mainCam->at.x;
            demo_way.y = mainCam->at.y;
            demo_way.z = mainCam->at.z;
            message_set(play, 0x409A, NULL);
            demo_mode = 21;
            demo_xa = 45.0f;
            door_check_time = 10;
            FALLTHROUGH;

        case 21:
            if ((door_check_time == 0) && pad_on_check(play)) {
                demo_mode = 22;
                door_check_time = 40;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_28);
                lure_hirotta_pos_y2 = 0.0f;
            }
            break;

        case 22:
            if (door_check_time == 30) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 9, NA_BGM_ITEM_GET);
            }

            lure_hirotta = true;

            add_calc2(&lure_hirotta_pos_y2, 71.0f, 0.5f, 3.0f);
            Matrix_rotateY(BINANG_TO_RAD_ALT(player->actor.shape.rot.y), MTXMODE_NEW);

            multiVecSrc.x = sin_s(play->gameplayFrames * 0x1000);
            multiVecSrc.y = lure_hirotta_pos_y2;
            multiVecSrc.z = -5.0f;
            if (AGE == LINK_AGE_CHILD) {
                multiVecSrc.y -= 20.0f;
            }

            Matrix_Position(&multiVecSrc, &eyeTarget);

            lure_hirotta_pos.x = player->actor.world.pos.x + eyeTarget.x;
            lure_hirotta_pos.y = player->actor.world.pos.y + eyeTarget.y;
            lure_hirotta_pos.z = player->actor.world.pos.z + eyeTarget.z;

            add_calc2(&demo_xa, 15.0f, 0.1f, 0.75f);

            multiVecSrc.x = demo_xa + 0.0f - 15.0f;

            if (AGE != LINK_AGE_CHILD) {
                multiVecSrc.y = 60.0f;
                multiVecSrc.z = -30.0f;
            } else {
                multiVecSrc.y = 40.0f;
                multiVecSrc.z = -35.0f;
            }

            Matrix_Position(&multiVecSrc, &demo_eye);
            demo_eye.x += player->actor.world.pos.x;
            demo_eye.y += player->actor.world.pos.y;
            demo_eye.z += player->actor.world.pos.z;

            demo_way = player->actor.world.pos;
            if (AGE != LINK_AGE_CHILD) {
                demo_way.y += 62.0f;
            } else {
                demo_way.y += 40.0f;
            }

            if (door_check_time == 0) {
                if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) ||
                    (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                    if (pad_on_check(play)) {
                        Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

                        message_close(play);
                        if (play->msgCtx.choiceIndex == 0) {
                            lure_type = FS_LURE_SINKING;
                            seken_mes_ct = 0;
                        }

                        mainCam->eye = demo_eye;
                        mainCam->eyeNext = demo_eye;
                        mainCam->at = demo_way;
                        Gama_play_shift2main_camera(play, demo_camera_no, 0);
                        Demo_play_end(play, &play->csCtx);
                        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                        demo_mode = 0;

                        demo_camera_no = SUB_CAM_ID_DONE;
                        player->unk_860 = -5;
                        lure_time = 5;
                        lure_hirotta = false;
                        lure_bgm_time = 20;
                        water_in_kankyo(play, 0);
                        play->envCtx.adjFogNear = 0;
                    }
                }
            }
            break;

        case 100:
            break;
    }

    if (demo_camera_no != SUB_CAM_ID_DONE) {
        Gama_play_camera_setting(play, demo_camera_no, &demo_way, &demo_eye);
        add_calc2(&demo_morf, 1.0f, 1.0f, 0.02f);

        if (demo_eye.y <= (WATER_SURFACE_Y(play) + 1.0f)) {
            water_in_kankyo(play, 1);
            if (mad_water != 0) {
                play->envCtx.adjFogNear = -178;
            } else {
                play->envCtx.adjFogNear = -46;
            }
        } else {
            water_in_kankyo(play, 0);
            play->envCtx.adjFogNear = 0;
        }
    }

    if ((player->actor.floorHeight < (WATER_SURFACE_Y(play) - 3.0f)) &&
        (player->actor.world.pos.y < (player->actor.floorHeight + 3.0f)) && (player->actor.speed > 1.0f) &&
        ((play->gameplayFrames % 2) == 0)) {
        Vec3f pos;

        pos.x = player->actor.world.pos.x + rnd_fx(20.0f);
        pos.z = player->actor.world.pos.z + rnd_fx(20.0f);
        pos.y = player->actor.floorHeight + 5.0f;
        fs_smoke_ct(NULL, play->specialEffects, &pos, 0.5f);
    }

    if ((player->actor.floorHeight < WATER_SURFACE_Y(play)) &&
        (player->actor.floorHeight > (WATER_SURFACE_Y(play) - 10.0f)) && (player->actor.speed >= 4.0f) &&
        ((play->gameplayFrames % 4) == 0)) {
        s16 i;

        for (i = 0; i < 10; i++) {
            Vec3f pos;
            Vec3f vel;
            f32 speedXZ;
            f32 angle;

            speedXZ = rnd_f(1.5f) + 1.5f;
            angle = rnd_f(6.28f);

            vel.x = sinf(angle) * speedXZ;
            vel.z = cosf(angle) * speedXZ;
            vel.y = rnd_f(3.0f) + 2.0f;

            pos = player->actor.world.pos;
            pos.x += 2.0f * vel.x;
            pos.y = WATER_SURFACE_Y(play);
            pos.z += 2.0f * vel.z;
            fs_mizu_ct(NULL, play->specialEffects, &pos, &vel, rnd_f(0.01f) + 0.020000001f);
        }
    }

#if DEBUG_FEATURES
    if (sREG(15) != 0) {
        if (rain != (sREG(15) - 1)) {
            if (rain == 0) {
                play->envCtx.stormRequest = STORM_REQUEST_START;
            } else {
                play->envCtx.stormRequest = STORM_REQUEST_STOP;
            }
        }

        rain = sREG(15) - 1;
    }

    if (sREG(14) == 1) {
        play->envCtx.stormRequest = STORM_REQUEST_START;
    }
    if (sREG(14) == -1) {
        play->envCtx.stormRequest = STORM_REQUEST_STOP;
    }

    sREG(14) = 0;

    PRINTF_COLOR_GREEN();
    PRINTF("zelda_time %x\n", ((void)0, z_common_data.save.dayTime));
    PRINTF_RST();
#endif

    if (wether >= 2) {
        wether--;
    }

    if ((wether == 1) && (message_check(&play->msgCtx) == TEXT_STATE_NONE) &&
        ((fishing_time & 0xFFF) == 0xFFF)) {
        wether = 200;

        if (fqrand() < 0.5f) {
            rain = (u8)rnd_f(10.0f) + 5;
            play->envCtx.stormRequest = STORM_REQUEST_START;
        } else {
            rain = 0;
            play->envCtx.stormRequest = STORM_REQUEST_STOP;
        }
    }

    add_calc2(&rain_S, rain, 1.0f, 0.05f);

    if (rain_S > 0.0f) {
        target = (rain_S * 0.03f) + 0.8f;
        if (target > 1.2f) {
            target = 1.2f;
        }
        add_calc2(&rain_se_p, target, 1.0f, 0.01f);
    }

    target = (10.0f - rain_S) * 150.1f;
    if (target < 0.0f) {
        target = 0.0f;
    }
    if (1) {}
    if (1) {}
    add_calc2(&rain_se_pos.z, target, 1.0f, 5.0f);

    if (rain_se_pos.z < 1500.0f) {
        Na_SetMotorSe(&rain_se_pos, NA_SE_EV_RAIN - SFX_FLAG, rain_se_p);
    }

    if (rain != 0) {
        add_calc2(&add_light, -200.0f, 1.0f, 2.0f);
    } else {
        add_calc0(&add_light, 1.0f, 2.0f);
    }

    play->envCtx.adjLight1Color[0] = play->envCtx.adjLight1Color[1] = play->envCtx.adjLight1Color[2] =
        add_light;

    if ((u8)rain_S > 0) {
        s32 pad;
        Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
        s16 i;
        s32 pad1;
        Vec3f pos;
        Vec3f rot;
        Vec3f projectedPos;
        s32 pad2;

        rot.x = M_PI / 2.0f + 0.1f;
        rot.y = 1.0f;
        rot.z = (getCameraAngleY(mainCam) * -(M_PI / 0x8000)) + rot.y;

        for (i = 0; i < (u8)rain_S; i++) {
            pos.x = play->view.eye.x + rnd_fx(700.0f);
            pos.y = (rnd_f(100.0f) + 150.0f) - 170.0f;
            pos.z = play->view.eye.z + rnd_fx(700.0f);

            if (pos.z < 1160.0f) {
                Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &pos, &projectedPos, &float_dammy);

                if (projectedPos.z < 0.0f) {
                    i--;
                } else {
                    fs_rain_ct(play->specialEffects, &pos, &rot);
                }
            }
        }
    }

    Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &wall_pos, &static_wall_pos, &float_dammy);

    Na_StartObjectSe_F(&static_wall_pos, NA_SE_EV_WATER_WALL - SFX_FLAG);

#if OOT_NTSC
    if (z_common_data.language == LANGUAGE_JPN) {
        z_common_data.minigameScore = FishRangeForMessage;
    } else {
        // Convert length to weight. Theoretical max of 59 lbs (127^2*.0036+.5)
        z_common_data.minigameScore = (SQ((f32)FishRangeForMessage) * 0.0036f) + 0.5f;
    }
#else
    // Same as above, but for PAL
    z_common_data.minigameScore = (SQ((f32)FishRangeForMessage) * 0.0036f) + 0.5f;
#endif

#if DEBUG_FEATURES
    if (BREG(26) != 0) {
        BREG(26) = 0;
        message_set(play, 0x407B + BREG(27), NULL);
    }

    PRINTF("HI_SCORE = %x\n", HIGH_SCORE(HS_FISHING));
#endif
}

s32 bm_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Fishing* this = (Fishing*)thisx;

    if (limbIndex == 8) { // Head
        rot->x -= this->unk_164;
    }

    return 0;
}

void bm_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    if (limbIndex == 8) { // Head
        OPEN_DISPS(play->state.gfxCtx, "../z_fishing.c", 9134);
        Matrix_Position(&zero, &bm_head_pos);

        if (bm_head_no == FS_OWNER_CAPPED) {
            gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gFishingOwnerHatDL));
        } else if (bm_head_no == FS_OWNER_HAIR) {
            gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gFishingOwnerHairDL));
        }

        CLOSE_DISPS(play->state.gfxCtx, "../z_fishing.c", 9142);
    }
}

static void* bm_eye_tex_no[] = {
    gFishingOwnerEyeOpenTex,
    gFishingOwnerEyeHalfTex,
    gFishingOwnerEyeClosedTex,
};

void bm_draw(Actor* thisx, PlayState* play) {
    PlayState* play2 = (PlayState*)play;
    Fishing* this = (Fishing*)thisx;
    Input* input = &play->state.input[0];

    OPEN_DISPS(play->state.gfxCtx, "../z_fishing.c", 9156);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    if ((thisx->projectedPos.z < 1500.0f) && (fabsf(thisx->projectedPos.x) < (100.0f + thisx->projectedPos.z))) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(bm_eye_tex_no[this->unk_160]));

        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              bm_draw_sub, bm_draw_sub2, this);
    }

    Fishing_Obj_disp(play2);
    Fishing_Eff_disp(play2->specialEffects, play2);
    Minnow_disp(play2);
    backwater_disp(play2);

    if (lure_bgm_time != 0) {
        lure_bgm_time--;

        if (lure_bgm_time == 0) {
            if (AGE != LINK_AGE_CHILD) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_KAKARIKO_ADULT);
            } else {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_KAKARIKO_KID);
            }

            if (AGE != LINK_AGE_CHILD) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_KAKARIKO_ADULT);
            } else {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_KAKARIKO_KID);
            }
        }
    }

    if ((fishing_game_mode != 0) && rod_disp) {
        rod_draw(play2);
        line_control2(line_pos);
        line_control(play2, &line_base_pos, line_pos, line_angle, line_spd);
        fishing_draw(play2, line_pos, line_angle);

        old_stick_x = input->rel.stick_x;
        old_stick_y = input->rel.stick_y;
    }

    rod_disp = true;

    Matrix_translate(130.0f, 40.0f, 1300.0f, MTXMODE_NEW);
    Matrix_scale(0.08f, 0.12f, 0.14f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_fishing.c", 9297);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_fishing.c", 9298);

    gSPDisplayList(POLY_OPA_DISP++, gFishingAquariumBottomDL);
    gSPDisplayList(POLY_XLU_DISP++, gFishingAquariumContainerDL);

    if ((fishing_game_mode != 0) && (lure_type == FS_LURE_SINKING)) {
        worm_disp(play2);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_fishing.c", 9305);
}
