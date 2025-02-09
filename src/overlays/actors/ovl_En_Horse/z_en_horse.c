/**
 * File: z_en_horse.c
 * Overlay: ovl_En_Horse
 * Description: Rideable horses
 */

#include "z_en_horse.h"
#include "z64horse.h"
#include "global.h"
#include "versions.h"
#include "overlays/actors/ovl_En_In/z_en_in.h"
#include "assets/objects/object_horse/object_horse.h"
#include "assets/objects/object_hni/object_hni.h"
#include "assets/scenes/overworld/spot09/spot09_scene.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef void (*EnHorseCsFunc)(EnHorse*, PlayState*, CsCmdActorCue*);
typedef void (*EnHorseActionFunc)(EnHorse*, PlayState*);

void En_Horse_Actor_ct(Actor* thisx, PlayState* play2);
void En_Horse_Actor_dt(Actor* thisx, PlayState* play);
void En_Horse_move(Actor* thisx, PlayState* play2);
void En_Horse_display(Actor* thisx, PlayState* play);

void En_Horse_Actor_TD_move_init(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_spot12_yabusame_init_start(EnHorse* this);
void En_Horse_Actor_mode_spot20_start_run_init(EnHorse* this);
void En_Horse_Actor_mode_base_wait_noclear_no_ride_request_init(EnHorse* this);
void En_Horse_Actor_mode_base_wait_init(EnHorse* this);
void En_Horse_mode_sleep_init(EnHorse* this);
void En_Horse_Actor_mode_race_run_start_init(EnHorse* this);

void En_Horse_Actor_mode_wait_lock(EnHorse* this, PlayState* play);
void En_Horse_mode_sleep(EnHorse* this, PlayState* play2);
void En_Horse_Actor_mode_base_wait(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_base_move(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_race_run(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_control_wait(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_control_wait02(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_control_rot_wait(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_control_walk(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_control_slow_run(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_control_fast_run(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_control_standing(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_control_stop(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_control_back_walk(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_demo_jump100(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_demo_jump200(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_demo_sport09_brideg_jump(EnHorse* this, PlayState* play);
void En_Horse_Actor_TD_move(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_spot12_yabusame(EnHorse* this, PlayState* play);
void En_Horse_Actor_mode_spot20_start_run(EnHorse* this, PlayState* play);

static AnimationHeader* skin_horse_anm_tbl2[] = {
    &gEponaIdleAnim,     &gEponaWhinnyAnim,    &gEponaRefuseAnim,  &gEponaRearingAnim,     &gEponaWalkingAnim,
    &gEponaTrottingAnim, &gEponaGallopingAnim, &gEponaJumpingAnim, &gEponaJumpingHighAnim,
};

static AnimationHeader* hni_anm_tbl[] = {
    &gHorseIngoIdleAnim,      &gHorseIngoWhinnyAnim,  &gHorseIngoRefuseAnim,
    &gHorseIngoRearingAnim,   &gHorseIngoWalkingAnim, &gHorseIngoTrottingAnim,
    &gHorseIngoGallopingAnim, &gHorseIngoJumpingAnim, &gHorseIngoJumpingHighAnim,
};

static AnimationHeader** anim_tbl_tbl[] = { skin_horse_anm_tbl2, hni_anm_tbl };

static f32 anim_speed[] = { 2.0f / 3.0f, 2.0f / 3.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f / 3.0f, 2.0f / 3.0f };

static SkeletonHeader* skel_info_tbl[] = { &gEponaSkel, &gHorseIngoSkel };

ActorProfile En_Horse_Profile = {
    /**/ ACTOR_EN_HORSE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HORSE,
    /**/ sizeof(EnHorse),
    /**/ En_Horse_Actor_ct,
    /**/ En_Horse_Actor_dt,
    /**/ En_Horse_move,
    /**/ En_Horse_display,
};

static ColliderCylinderInit HorseAtOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_TYPE_PLAYER,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000400, 0x00, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit HorseOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit HorseAcOcInfoJntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_NO_AT_INFO | ACELEM_NO_DAMAGE | ACELEM_NO_SWORD_SFX | ACELEM_NO_HITMARK,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit HorseAcOcInfoJntSphData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    1,
    HorseAcOcInfoJntSphElemData,
};

static CollisionCheckInfoInit HorseStatusData = { 10, 35, 100, MASS_HEAVY };

typedef struct EnHorseSpawnpoint {
    /* 0x0 */ s16 sceneId;
    /* 0x2 */ Vec3s pos;
    /* 0x8 */ s16 angle;
} EnHorseSpawnpoint; // size = 0xA

static EnHorseSpawnpoint kusabue_set_tbl[] = {
    { SCENE_HYRULE_FIELD, 16, 0, 1341, 0 },
    { SCENE_HYRULE_FIELD, -1297, 0, 1459, 0 },
    { SCENE_HYRULE_FIELD, -5416, -300, 1296, 0 },
    { SCENE_HYRULE_FIELD, -4667, -300, 3620, 0 },
    { SCENE_HYRULE_FIELD, -3837, 81, 5537, 0 },
    { SCENE_HYRULE_FIELD, -5093, -226, 6661, 0 },
    { SCENE_HYRULE_FIELD, -6588, -79, 5053, 0 },
    { SCENE_HYRULE_FIELD, -7072, -500, 7538, 0 },
    { SCENE_HYRULE_FIELD, -6139, -500, 8910, 0 },
    { SCENE_HYRULE_FIELD, -8497, -300, 7802, 0 },
    { SCENE_HYRULE_FIELD, -5481, -499, 12127, 0 },
    { SCENE_HYRULE_FIELD, -4808, -700, 13583, 0 },
    { SCENE_HYRULE_FIELD, -3416, -490, 12092, 0 },
    { SCENE_HYRULE_FIELD, -2915, 100, 8339, 0 },
    { SCENE_HYRULE_FIELD, -2277, -500, 13247, 0 },
    { SCENE_HYRULE_FIELD, -1026, -500, 12101, 0 },
    { SCENE_HYRULE_FIELD, 1427, -500, 13341, 0 },
    { SCENE_HYRULE_FIELD, -200, -486, 10205, 0 },
    { SCENE_HYRULE_FIELD, -1469, 100, 7496, 0 },
    { SCENE_HYRULE_FIELD, -995, 168, 5652, 0 },
    { SCENE_HYRULE_FIELD, 1938, 89, 6232, 0 },
    { SCENE_HYRULE_FIELD, 1387, -105, 9206, 0 },
    { SCENE_HYRULE_FIELD, 1571, -223, 7701, 0 },
    { SCENE_HYRULE_FIELD, 3893, -121, 7068, 0 },
    { SCENE_HYRULE_FIELD, 3179, 373, 5221, 0 },
    { SCENE_HYRULE_FIELD, 4678, -20, 3869, 0 },
    { SCENE_HYRULE_FIELD, 3460, 246, 4207, 0 },
    { SCENE_HYRULE_FIELD, 3686, 128, 2366, 0 },
    { SCENE_HYRULE_FIELD, 1791, 18, 152, 0 },
    { SCENE_HYRULE_FIELD, 3667, -16, 1399, 0 },
    { SCENE_HYRULE_FIELD, 1827, -15, 983, 0 },
    { SCENE_HYRULE_FIELD, 1574, 399, 4318, 0 },
    { SCENE_HYRULE_FIELD, 716, 95, 3391, 0 },
    { SCENE_HYRULE_FIELD, -1189, -41, 4739, 0 },
    { SCENE_HYRULE_FIELD, -1976, -171, 4172, 0 },
    { SCENE_HYRULE_FIELD, 1314, 391, 5665, 0 },
    { SCENE_HYRULE_FIELD, 112, 0, 1959, 0 },
    { SCENE_HYRULE_FIELD, -3011, -111, 9397, 0 },
    { SCENE_HYRULE_FIELD, -5674, -270, 8585, 0 },
    { SCENE_HYRULE_FIELD, -8861, -300, 7836, 0 },
    { SCENE_HYRULE_FIELD, -6056, -500, 7810, 0 },
    { SCENE_HYRULE_FIELD, -7306, -500, 5994, 0 },
    { SCENE_HYRULE_FIELD, -7305, -500, 7605, 0 },
    { SCENE_HYRULE_FIELD, -7439, -300, 7600, 0 },
    { SCENE_HYRULE_FIELD, -7464, -300, 6268, 0 },
    { SCENE_HYRULE_FIELD, -8080, -300, 7553, 0 },
    { SCENE_HYRULE_FIELD, -8091, -300, 7349, 0 },
    { SCENE_HYRULE_FIELD, -8785, -300, 7383, 0 },
    { SCENE_HYRULE_FIELD, -8745, -300, 7508, 0 },
    { SCENE_HYRULE_FIELD, -8777, -300, 7788, 0 },
    { SCENE_HYRULE_FIELD, -8048, -299, 7738, 0 },
    { SCENE_HYRULE_FIELD, -7341, -184, 7730, 0 },
    { SCENE_HYRULE_FIELD, -6410, -288, 7824, 0 },
    { SCENE_HYRULE_FIELD, -6326, -290, 8205, 0 },
    { SCENE_HYRULE_FIELD, -6546, -292, 8400, 0 },
    { SCENE_HYRULE_FIELD, -7533, -180, 8459, 0 },
    { SCENE_HYRULE_FIELD, -8024, -295, 7903, 0 },
    { SCENE_HYRULE_FIELD, -8078, -308, 7994, 0 },
    { SCENE_HYRULE_FIELD, -9425, -287, 7696, 0 },
    { SCENE_HYRULE_FIELD, -9322, -292, 7577, 0 },
    { SCENE_HYRULE_FIELD, -9602, -199, 7160, 0 },
    { SCENE_HYRULE_FIELD, -9307, -300, 7758, 0 },
    { SCENE_HYRULE_FIELD, -9230, -300, 7642, 0 },
    { SCENE_HYRULE_FIELD, -7556, -499, 8695, 0 },
    { SCENE_HYRULE_FIELD, -6438, -500, 8606, 0 },
    { SCENE_HYRULE_FIELD, -6078, -500, 8258, 0 },
    { SCENE_HYRULE_FIELD, -6233, -500, 7613, 0 },
    { SCENE_HYRULE_FIELD, -5035, -205, 7814, 0 },
    { SCENE_HYRULE_FIELD, -5971, -500, 8501, 0 },
    { SCENE_HYRULE_FIELD, -5724, -498, 10123, 0 },
    { SCENE_HYRULE_FIELD, -5094, -392, 11106, 0 },
    { SCENE_HYRULE_FIELD, -5105, -393, 11312, 0 },
    { SCENE_HYRULE_FIELD, -4477, -314, 11132, 0 },
    { SCENE_HYRULE_FIELD, -3867, -380, 11419, 0 },
    { SCENE_HYRULE_FIELD, -2952, -500, 11944, 0 },
    { SCENE_HYRULE_FIELD, -2871, -488, 11743, 0 },
    { SCENE_HYRULE_FIELD, -3829, -372, 11327, 0 },
    { SCENE_HYRULE_FIELD, -4439, -293, 10989, 0 },
    { SCENE_HYRULE_FIELD, -5014, -381, 11086, 0 },
    { SCENE_HYRULE_FIELD, -5113, -188, 10968, 0 },
    { SCENE_HYRULE_FIELD, -5269, -188, 11156, 0 },
    { SCENE_HYRULE_FIELD, -5596, -178, 9972, 0 },
    { SCENE_HYRULE_FIELD, -5801, -288, 8518, 0 },
    { SCENE_HYRULE_FIELD, -4910, -178, 7931, 0 },
    { SCENE_HYRULE_FIELD, -3586, 73, 8140, 0 },
    { SCENE_HYRULE_FIELD, -4487, -106, 9362, 0 },
    { SCENE_HYRULE_FIELD, -4339, -112, 6412, 0 },
    { SCENE_HYRULE_FIELD, -3417, 105, 8194, 0 },
    { SCENE_HYRULE_FIELD, -4505, -20, 6608, 0 },
    { SCENE_HYRULE_FIELD, -5038, -199, 6603, 0 },
    { SCENE_HYRULE_FIELD, -4481, 1, 6448, 0 },
    { SCENE_HYRULE_FIELD, -5032, -168, 6418, 0 },
    { SCENE_HYRULE_FIELD, -5256, -700, 14329, 0 },
    { SCENE_HYRULE_FIELD, -5749, -820, 15380, 0 },
    { SCENE_HYRULE_FIELD, -3122, -700, 13608, 0 },
    { SCENE_HYRULE_FIELD, -3758, -525, 13228, 0 },
    { SCENE_HYRULE_FIELD, -3670, -500, 13123, 0 },
    { SCENE_HYRULE_FIELD, -2924, -500, 13526, 0 },
    { SCENE_HYRULE_FIELD, 1389, -115, 9370, 0 },
    { SCENE_HYRULE_FIELD, 548, -116, 8889, 0 },
    { SCENE_HYRULE_FIELD, -106, -107, 8530, 0 },
    { SCENE_HYRULE_FIELD, -1608, 85, 7646, 0 },
    { SCENE_HYRULE_FIELD, -5300, -700, 13772, 0 },
    { SCENE_HYRULE_FIELD, -5114, -700, 13400, 0 },
    { SCENE_HYRULE_FIELD, -4561, -700, 13700, 0 },
    { SCENE_HYRULE_FIELD, -4762, -700, 14084, 0 },
    { SCENE_HYRULE_FIELD, -2954, 100, 8216, 0 },
    { SCENE_HYRULE_FIELD, 1460, -104, 9246, 0 },
    { SCENE_HYRULE_FIELD, 629, -105, 8791, 0 },
    { SCENE_HYRULE_FIELD, -10, -90, 8419, 0 },
    { SCENE_HYRULE_FIELD, -1462, 100, 7504, 0 },
    { SCENE_HYRULE_FIELD, -3018, -500, 12493, 0 },
    { SCENE_HYRULE_FIELD, -2994, -311, 10331, 0 },
    { SCENE_HYRULE_FIELD, -4006, -700, 14152, 0 },
    { SCENE_HYRULE_FIELD, -4341, -500, 12743, 0 },
    { SCENE_HYRULE_FIELD, -5879, -497, 6799, 0 },
    { SCENE_HYRULE_FIELD, 22, -473, 10103, 0 },
    { SCENE_HYRULE_FIELD, -1389, -192, 8874, 0 },
    { SCENE_HYRULE_FIELD, -4, 92, 6866, 0 },
    { SCENE_HYRULE_FIELD, 483, 104, 6637, 0 },
    { SCENE_HYRULE_FIELD, 1580, 183, 5987, 0 },
    { SCENE_HYRULE_FIELD, 1548, 308, 5077, 0 },
    { SCENE_HYRULE_FIELD, 1511, 399, 4267, 0 },
    { SCENE_HYRULE_FIELD, 1358, 385, 4271, 0 },
    { SCENE_HYRULE_FIELD, 1379, 395, 5063, 0 },
    { SCENE_HYRULE_FIELD, 1360, 394, 5870, 0 },
    { SCENE_HYRULE_FIELD, 813, 283, 6194, 0 },
    { SCENE_HYRULE_FIELD, -57, 101, 6743, 0 },
    { SCENE_HYRULE_FIELD, 91, 325, 5143, 0 },
    { SCENE_HYRULE_FIELD, 1425, -214, 7659, 0 },
    { SCENE_HYRULE_FIELD, 3487, -19, 880, 0 },
    { SCENE_HYRULE_FIELD, 2933, 152, 2094, 0 },
    { SCENE_HYRULE_FIELD, 2888, -145, 6862, 0 },
    { SCENE_HYRULE_FIELD, 1511, 67, 6471, 0 },
    { SCENE_HYRULE_FIELD, 4051, -47, 1722, 0 },
    { SCENE_HYRULE_FIELD, -7335, -500, 8627, 0 },
    { SCENE_HYRULE_FIELD, -7728, -462, 8498, 0 },
    { SCENE_HYRULE_FIELD, -7791, -446, 8832, 0 },
    { SCENE_HYRULE_FIELD, -2915, -435, 11334, 0 },
    { SCENE_HYRULE_FIELD, 165, -278, 3352, 0 },

    { SCENE_LAKE_HYLIA, -2109, -882, 1724, 0 },
    { SCENE_LAKE_HYLIA, -328, -1238, 2705, 0 },
    { SCENE_LAKE_HYLIA, -3092, -1033, 3527, 0 },

    { SCENE_GERUDO_VALLEY, 2687, -269, 753, 0 },
    { SCENE_GERUDO_VALLEY, 2066, -132, 317, 0 },
    { SCENE_GERUDO_VALLEY, 523, -8, 635, 0 },
    { SCENE_GERUDO_VALLEY, 558, 36, -323, 0 },
    { SCENE_GERUDO_VALLEY, 615, 51, -839, 0 },
    { SCENE_GERUDO_VALLEY, -614, 32, 29, 0 },
    { SCENE_GERUDO_VALLEY, -639, -3, 553, 0 },
    { SCENE_GERUDO_VALLEY, -540, 10, -889, 0 },
    { SCENE_GERUDO_VALLEY, -1666, 58, 378, 0 },
    { SCENE_GERUDO_VALLEY, -3044, 210, -648, 0 },

    { SCENE_GERUDOS_FORTRESS, -678, 21, -623, 0 },
    { SCENE_GERUDOS_FORTRESS, 149, 333, -2499, 0 },
    { SCENE_GERUDOS_FORTRESS, 499, 581, -547, 0 },
    { SCENE_GERUDOS_FORTRESS, 3187, 1413, -3775, 0 },
    { SCENE_GERUDOS_FORTRESS, 3198, 1413, 307, 0 },
    { SCENE_GERUDOS_FORTRESS, 3380, 1413, -1200, 0 },
    { SCENE_GERUDOS_FORTRESS, -966, 1, -56, 0 },
    { SCENE_GERUDOS_FORTRESS, -966, 24, -761, 0 },
    { SCENE_GERUDOS_FORTRESS, -694, 174, -2820, 0 },

    { SCENE_LON_LON_RANCH, 1039, 0, 2051, 0 },
    { SCENE_LON_LON_RANCH, -1443, 0, 1429, 0 },
    { SCENE_LON_LON_RANCH, 856, 0, -918, 0 }, // Hardcoded to always load in lon lon
    { SCENE_LON_LON_RANCH, 882, 0, -2256, 0 },
    { SCENE_LON_LON_RANCH, -1003, 0, -755, 0 }, // Hardcoded to always load in lon lon
    { SCENE_LON_LON_RANCH, -2254, 0, -629, 0 },
    { SCENE_LON_LON_RANCH, 907, 0, -2336, 0 },
};

typedef struct BridgeJumpPoint {
    /* 0x00 */ s16 zMin;
    /* 0x02 */ s16 zMax;
    /* 0x04 */ s16 xMin;
    /* 0x06 */ s16 xMax;
    /* 0x08 */ s16 xOffset;
    /* 0x0A */ s16 angle;
    /* 0x0C */ s16 angleRange;
    /* 0x0E */ Vec3s pos;
} BridgeJumpPoint; // size = 0x14

static BridgeJumpPoint spot09_bridge_jump_area[] = {
    { -195, -40, 225, 120, 360, -0x4000, 0x7D0, -270, -52, -117 },
    { -195, -40, -240, -120, -360, 0x4000, 0x7D0, 270, -52, -117 },
};

typedef struct RaceWaypoint {
    /* 0x0 */ s16 x;
    /* 0x2 */ s16 y;
    /* 0x4 */ s16 z;
    /* 0x6 */ s16 speedXZ;
    /* 0x8 */ s16 angle;
} RaceWaypoint; // size = 0xA

typedef struct RaceInfo {
    /* 0x0 */ s32 numWaypoints;
    /* 0x4 */ RaceWaypoint* waypoints;
} RaceInfo; // size = 0x8

static RaceWaypoint spot20_race_data[] = {
    { 1056, 1, -1540, 11, 0x2A8D },  { 1593, 1, -985, 10, 0xFC27 },   { 1645, 1, -221, 11, 0xE891 },
    { 985, 1, 403, 10, 0xBB9C },     { -1023, 1, 354, 11, 0xA37D },   { -1679, 1, -213, 10, 0x889C },
    { -1552, 1, -1008, 11, 0x638D }, { -947, -1, -1604, 10, 0x4002 },
};

static RaceInfo spot20_race_info = { ARRAY_COUNT(spot20_race_data), spot20_race_data };
static s32 sound_on_frames[] = { 0, 16 };

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 600, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_STOP),
};

static u8 argd_ride_flag[] = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0 };

static s32 next_anim[] = { 1, 3, 0, 3, 1, 0 };

static s16 h2i_tbl[] = { 7, 6, 2, 2, 1, 1, 0, 0, 0, 0 };

void Hl_Td_Run_init(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void Hl_Td_Jump_init(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void Hl_Td_Wait_init(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void Hl_Td_Change_Pos_Run_init(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void Hl_Td_Change_Pos_Wait_init(EnHorse* this, PlayState* play, CsCmdActorCue* cue);

static EnHorseCsFunc td_init_function[] = {
    NULL,
    Hl_Td_Run_init,
    Hl_Td_Jump_init,
    Hl_Td_Wait_init,
    Hl_Td_Change_Pos_Run_init,
    Hl_Td_Change_Pos_Wait_init,
};

void Hl_Td_Run(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void Hl_Td_Jump(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void Hl_Td_Wait(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void Hl_Td_Change_Pos_Run(EnHorse* this, PlayState* play, CsCmdActorCue* cue);
void Hl_Td_Change_Pos_Wait(EnHorse* this, PlayState* play, CsCmdActorCue* cue);

static EnHorseCsFunc td_function[] = {
    NULL, Hl_Td_Run, Hl_Td_Jump, Hl_Td_Wait, Hl_Td_Change_Pos_Run, Hl_Td_Change_Pos_Wait,
};

typedef struct CsActionEntry {
    /* 0x0 */ s32 cueId;
    /* 0x4 */ s32 csFuncIdx;
} CsActionEntry; // size = 0x8

static CsActionEntry P2hCnvTbl[] = {
    { PLAYER_CUEID_36, 1 }, { PLAYER_CUEID_37, 2 }, { PLAYER_CUEID_38, 3 },
    { PLAYER_CUEID_64, 4 }, { PLAYER_CUEID_65, 5 },
};

static RaceWaypoint spot12_yabu_data[] = {
    { 3600, 1413, -5055, 11, 0x8001 }, { 3360, 1413, -5220, 5, 0xC000 }, { 3100, 1413, -4900, 5, 0x0000 },
    { 3600, 1413, -4100, 11, 0x0000 }, { 3600, 1413, 360, 11, 0x0000 },
};

static RaceInfo spot12_yabu_info = { ARRAY_COUNT(spot12_yabu_data), spot12_yabu_data };

static EnHorseActionFunc mode_function[] = {
    En_Horse_Actor_mode_wait_lock,                 // ENHORSE_ACT_FROZEN
    En_Horse_mode_sleep,               // ENHORSE_ACT_INACTIVE
    En_Horse_Actor_mode_base_wait,                   // ENHORSE_ACT_IDLE
    En_Horse_Actor_mode_base_move,           // ENHORSE_ACT_FOLLOW_PLAYER
    En_Horse_Actor_mode_race_run,         // ENHORSE_ACT_INGO_RACE
    En_Horse_Actor_mode_control_wait,            // ENHORSE_ACT_MOUNTED_IDLE
    En_Horse_Actor_mode_control_wait02,  // ENHORSE_ACT_MOUNTED_IDLE_WHINNEYING
    En_Horse_Actor_mode_control_rot_wait,            // ENHORSE_ACT_MOUNTED_TURN
    En_Horse_Actor_mode_control_walk,            // ENHORSE_ACT_MOUNTED_WALK
    En_Horse_Actor_mode_control_slow_run,            // ENHORSE_ACT_MOUNTED_TROT
    En_Horse_Actor_mode_control_fast_run,          // ENHORSE_ACT_MOUNTED_GALLOP
    En_Horse_Actor_mode_control_standing,         // ENHORSE_ACT_MOUNTED_REARING
    En_Horse_Actor_mode_control_stop,               // ENHORSE_ACT_STOPPING
    En_Horse_Actor_mode_control_back_walk,                // ENHORSE_ACT_REVERSE
    En_Horse_Actor_mode_demo_jump100,                // ENHORSE_ACT_LOW_JUMP
    En_Horse_Actor_mode_demo_jump200,               // ENHORSE_ACT_HIGH_JUMP
    En_Horse_Actor_mode_demo_sport09_brideg_jump,             // ENHORSE_ACT_BRIDGE_JUMP
    En_Horse_Actor_TD_move,         // ENHORSE_ACT_CS_UPDATE
    En_Horse_Actor_mode_spot12_yabusame, // ENHORSE_ACT_HBA
    En_Horse_Actor_mode_spot20_start_run,             // ENHORSE_ACT_FLEE_PLAYER
};

s32 Spot09BridgeJumpRunAreaCheck(EnHorse* this, PlayState* play) {
    f32 xMin;
    f32 xMax;
    s32 i;

    if (play->sceneId != SCENE_GERUDO_VALLEY) {
        return false;
    }
    if (this->actor.speed < 12.8f) {
        return false;
    }
    if (GET_EVENTCHKINF_CARPENTERS_ALL_RESCUED()) {
        return false;
    }

    for (i = 0; i < ARRAY_COUNT(spot09_bridge_jump_area); i++) {
        xMin = spot09_bridge_jump_area[i].xMin;
        xMax = (xMin + spot09_bridge_jump_area[i].xMax) + spot09_bridge_jump_area[i].xOffset;
        if (xMax < xMin) {
            f32 temp = xMin;

            xMin = xMax;
            xMax = temp;
        }
        if (spot09_bridge_jump_area[i].zMin < this->actor.world.pos.z && this->actor.world.pos.z < spot09_bridge_jump_area[i].zMax) {
            if (xMin < this->actor.world.pos.x && this->actor.world.pos.x < xMax) {
                if (spot09_bridge_jump_area[i].angle - spot09_bridge_jump_area[i].angleRange < this->actor.world.rot.y &&
                    this->actor.world.rot.y < spot09_bridge_jump_area[i].angle + spot09_bridge_jump_area[i].angleRange) {
                    return true;
                }
            }
        }
    }
    return false;
}

void En_Horse_Actor_mode_demo_spot09_bridge_jump_init(EnHorse* this, PlayState* play);

s32 Spot09BridgeJumpCheckSet(EnHorse* this, PlayState* play) {
    f32 xMin;
    f32 xMax;
    s32 i;

    if (this->actor.speed < 12.8f) {
        return false;
    }

    for (i = 0; i < ARRAY_COUNT(spot09_bridge_jump_area); i++) {
        xMin = spot09_bridge_jump_area[i].xMin;
        xMax = spot09_bridge_jump_area[i].xMax + xMin;

        if (xMax < xMin) {
            f32 temp = xMin;

            xMin = xMax;
            xMax = temp;
        }

        if (spot09_bridge_jump_area[i].zMin < this->actor.world.pos.z && this->actor.world.pos.z < spot09_bridge_jump_area[i].zMax) {
            if (xMin < this->actor.world.pos.x && this->actor.world.pos.x < xMax) {
                if (spot09_bridge_jump_area[i].angle - spot09_bridge_jump_area[i].angleRange < this->actor.world.rot.y &&
                    this->actor.world.rot.y < spot09_bridge_jump_area[i].angle + spot09_bridge_jump_area[i].angleRange) {
                    this->bridgeJumpIdx = i;
                    En_Horse_Actor_mode_demo_spot09_bridge_jump_init(this, play);
                    return true;
                }
            }
        }
    }

    return false;
}

void CourseData2xyz_t(RaceWaypoint* waypoints, s32 index, Vec3f* pos) {
    pos->x = waypoints[index].x;
    pos->y = waypoints[index].y;
    pos->z = waypoints[index].z;
}

void En_HL_trace_pos(EnHorse* this, PlayState* play, Vec3f* pos, s16 turnAmount) {
    horse_rot_trace_pos(&this->actor, pos, turnAmount);
}

void En_HL_CourseRun(EnHorse* this, PlayState* play, RaceInfo* raceInfo) {
    Vec3f curWaypointPos;
    Vec3f prevWaypointPos;
    s32 prevWaypoint;
    f32 sp50;
    s16 relPlayerYaw;
    f32 px;
    f32 pz;
    f32 d;
    f32 distSq;

    CourseData2xyz_t(raceInfo->waypoints, this->curRaceWaypoint, &curWaypointPos);
    Math3DPlanePosAngleY(&curWaypointPos, raceInfo->waypoints[this->curRaceWaypoint].angle, &px, &pz, &d);
    if (((this->actor.world.pos.x * px) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        this->curRaceWaypoint++;
        if (this->curRaceWaypoint >= raceInfo->numWaypoints) {
            this->curRaceWaypoint = 0;
        }
    }

    CourseData2xyz_t(raceInfo->waypoints, this->curRaceWaypoint, &curWaypointPos);

    prevWaypoint = this->curRaceWaypoint - 1;
    if (prevWaypoint < 0) {
        prevWaypoint = raceInfo->numWaypoints - 1;
    }
    CourseData2xyz_t(raceInfo->waypoints, prevWaypoint, &prevWaypointPos);
    Math3D_pointVsLineSegmentLengthSquare2D(this->actor.world.pos.x, this->actor.world.pos.z, prevWaypointPos.x, prevWaypointPos.z,
                               curWaypointPos.x, curWaypointPos.z, &distSq);
    En_HL_trace_pos(this, play, &curWaypointPos, 400);

    if (distSq < SQ(300.0f)) {
        if (this->actor.xzDistToPlayer < 130.0f || this->jntSph.elements[0].base.ocElemFlags & OCELEM_HIT) {
            s32 pad;

            if (sin_s(this->actor.yawTowardsPlayer - this->actor.world.rot.y) > 0.0f) {
                this->actor.world.rot.y -= 280;
            } else {
                this->actor.world.rot.y += 280;
            }
        } else if (this->actor.xzDistToPlayer < 300.0f) {
            if (sin_s(this->actor.yawTowardsPlayer - this->actor.world.rot.y) > 0.0f) {
                this->actor.world.rot.y += 280;
            } else {
                this->actor.world.rot.y -= 280;
            }
        }
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    sp50 = Actor_search_actor_distanceXZ(&this->actor, &GET_PLAYER(play)->actor);
    relPlayerYaw = Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;
    if (sp50 <= 200.0f || (fabsf(sin_s(relPlayerYaw)) < 0.8f && cos_s(relPlayerYaw) > 0.0f)) {
        if (this->actor.speed < this->ingoHorseMaxSpeed) {
            this->actor.speed += 0.47f;
        } else {
            this->actor.speed -= 0.47f;
        }
        this->ingoRaceFlags |= 1;
        return;
    }

    if (this->actor.speed < raceInfo->waypoints[this->curRaceWaypoint].speedXZ) {
        this->actor.speed += 0.4f;
    } else {
        this->actor.speed -= 0.4f;
    }
    this->ingoRaceFlags &= ~0x1;
}

void En_Horse_setWalkSound(EnHorse* this) {
    if (sound_on_frames[this->soundTimer] < this->curFrame) {
        if (this->soundTimer == 0 && (sound_on_frames[1] < this->curFrame)) {
            return;
        }

        Nai_FxFlagEntry(NA_SE_EV_HORSE_WALK, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        if (++this->soundTimer >= ARRAY_COUNT(sound_on_frames)) {
            this->soundTimer = 0;
        }
    }
}

void En_Horse_setSlowRunSound(EnHorse* this) {
    Nai_FxFlagEntry(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
}

void En_Horse_setFastRunSound(EnHorse* this) {
    Nai_FxFlagEntry(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
}

f32 ground_speed_effect(EnHorse* this, PlayState* play) {
    f32 multiplier = 1.0f;

    if (cos_s(this->actor.shape.rot.x) < 0.939262f && sin_s(this->actor.shape.rot.x) < 0.0f) {
        multiplier = 0.7f;
    }
    return multiplier;
}

void calc_camera_pos(PlayState* play, Vec3f* vec, Vec3f* arg2, f32* arg3) {
    Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, vec, arg2, arg3);
}

int check_camera_view(PlayState* play, EnHorse* this, Vec3f* pos) {
    Vec3f sp24;
    f32 sp20;
    f32 eyeDist;

    calc_camera_pos(play, pos, &sp24, &sp20);
    if (fabsf(sp20) < 0.008f) {
        return false;
    }
    eyeDist = Math3DLength(pos, &play->view.eye);
    return Actor_draw_actor_no_culling_check2(play, &this->actor, &sp24, sp20) || eyeDist < 100.0f;
}

void En_Horse_Actor_setWaitSound(EnHorse* this, PlayState* play) {
    if (this->animationIdx == ENHORSE_ANIM_IDLE &&
        ((this->curFrame > 35.0f && this->type == HORSE_EPONA) ||
         (this->curFrame > 28.0f && this->type == HORSE_HNI)) &&
        !(this->stateFlags & ENHORSE_SANDDUST_SOUND)) {
        this->stateFlags |= ENHORSE_SANDDUST_SOUND;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_SANDDUST, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (this->animationIdx == ENHORSE_ANIM_REARING && this->curFrame > 25.0f &&
               !(this->stateFlags & ENHORSE_LAND2_SOUND)) {
        this->stateFlags |= ENHORSE_LAND2_SOUND;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

s32 En_Horse_Actor_setKusabuePos(EnHorse* this, PlayState* play) {
    f32 minDist = 1e38f;
    s32 spawn = false;
    f32 dist;
    s32 i;
    Player* player;
    Vec3f spawnPos;

    for (i = 0; i < ARRAY_COUNT(kusabue_set_tbl); i++) {
        if (kusabue_set_tbl[i].sceneId == play->sceneId) {
            player = GET_PLAYER(play);
            if (play->sceneId != SCENE_LON_LON_RANCH ||
                //! Same flag checked twice
                (event_check(EVENTCHKINF_EPONA_OBTAINED) &&
                 (GET_EVENTINF_INGO_RACE_STATE() != INGO_RACE_STATE_TRAPPED_WIN_EPONA ||
                  event_check(EVENTCHKINF_EPONA_OBTAINED))) ||
                // always load two spawns inside lon lon
                ((kusabue_set_tbl[i].pos.x == 856 && kusabue_set_tbl[i].pos.y == 0 && kusabue_set_tbl[i].pos.z == -918) ||
                 (kusabue_set_tbl[i].pos.x == -1003 && kusabue_set_tbl[i].pos.y == 0 && kusabue_set_tbl[i].pos.z == -755))) {

                spawnPos.x = kusabue_set_tbl[i].pos.x;
                spawnPos.y = kusabue_set_tbl[i].pos.y;
                spawnPos.z = kusabue_set_tbl[i].pos.z;
                dist = Math3DLength(&player->actor.world.pos, &spawnPos);

                if ((minDist < dist) || check_camera_view(play, this, &spawnPos)) {
                    continue;
                }

                minDist = dist;
                this->actor.world.pos.x = kusabue_set_tbl[i].pos.x;
                this->actor.world.pos.y = kusabue_set_tbl[i].pos.y;
                this->actor.world.pos.z = kusabue_set_tbl[i].pos.z;
                this->actor.prevPos = this->actor.world.pos;
                this->actor.world.rot.y = kusabue_set_tbl[i].angle;
                this->actor.shape.rot.y = Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor);
                spawn = true;
#if OOT_VERSION >= PAL_1_0
                Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &this->actor.world.pos,
                                             &this->actor.projectedPos, &this->actor.projectedW);
#endif
            }
        }
    }

    return spawn;
}

void En_Horse_Actor_Demo_init(EnHorse* this, PlayState* play) {
    this->cutsceneAction = -1;
    this->cutsceneFlags = 0;
}

void En_Horse_Actor_Race_init(EnHorse* this, PlayState* play) {
    this->inRace = false;
}

s32 En_HL_checkControl(EnHorse* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 & PLAYER_STATE1_0) || player_bow_ready_check(GET_PLAYER(play)) == true ||
        (player->stateFlags1 & PLAYER_STATE1_20) || ((this->stateFlags & ENHORSE_FLAG_19) && !this->inRace) ||
        this->action == ENHORSE_ACT_HBA || player->actor.flags & ACTOR_FLAG_TALK ||
        play->csCtx.state != CS_STATE_IDLE) {
        return false;
    }
    return true;
}

void En_Horse_Actor_Yabu_init(EnHorse* this, PlayState* play) {
    this->unk_39C = 0;
    this->hbaStarted = 0;
    this->hbaFlags = 0;
}

void Dust_ct(u16* dustFlags) {
    *dustFlags = 0;
}

void En_Horse_Actor_ct(Actor* thisx, PlayState* play2) {
    EnHorse* this = (EnHorse*)thisx;
    PlayState* play = play2;

    R_EXITED_SCENE_RIDING_HORSE = false;
    ValueSet_process(&this->actor, value_init);
    Dust_ct(&this->dustFlags);
    R_EPONAS_SONG_PLAYED = false;
    this->riderPos = this->actor.world.pos;
    this->noInputTimer = 0;
    this->noInputTimerMax = 0;
    this->riderPos.y += 70.0f;

    if (DREG(4) == 0) {
        DREG(4) = 70;
    }

    if (PARAMS_GET_NOSHIFT(this->actor.params, 15, 1)) {
        this->actor.params &= ~0x8000;
        this->type = HORSE_HNI;

        if ((this->hniObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_HNI)) < 0) {
            Actor_delete(&this->actor);
            return;
        }

        do {
        } while (!Object_Exchange_bank_dma_check(&play->objectCtx, this->hniObjectSlot));

        this->actor.objectSlot = this->hniObjectSlot;
        Actor_set_segment(play, &this->actor);
        this->boostSpeed = 12;
    } else {
        this->type = HORSE_EPONA;
        this->boostSpeed = 14;
    }

    // params was -1
    if (this->actor.params == 0x7FFF) {
        this->actor.params = HORSE_PTYPE_1;
    }

    if (play->sceneId == SCENE_LON_LON_BUILDINGS) {
        this->stateFlags = ENHORSE_UNRIDEABLE;
    } else if (play->sceneId == SCENE_GERUDOS_FORTRESS && this->type == HORSE_HNI) {
        this->stateFlags = ENHORSE_FLAG_18 | ENHORSE_UNRIDEABLE;
    } else {
        if (this->actor.params == HORSE_PTYPE_INGO_SPAWNED_RIDING) {
            this->stateFlags = ENHORSE_FLAG_19 | ENHORSE_CANT_JUMP | ENHORSE_UNRIDEABLE;
        } else if (this->actor.params == HORSE_PTYPE_6) {
            this->stateFlags = ENHORSE_FLAG_19 | ENHORSE_CANT_JUMP;
            if (event_check(EVENTCHKINF_EPONA_OBTAINED) || R_DEBUG_FORCE_EPONA_OBTAINED) {
                this->stateFlags &= ~ENHORSE_CANT_JUMP;
                this->stateFlags |= ENHORSE_FLAG_26;
            } else if (GET_EVENTINF(EVENTINF_INGO_RACE_SECOND_RACE) && this->type == HORSE_HNI) {
                this->stateFlags |= ENHORSE_FLAG_21 | ENHORSE_FLAG_20;
            }
        } else if (this->actor.params == HORSE_PTYPE_1) {
            this->stateFlags = ENHORSE_FLAG_7;
        } else {
            this->stateFlags = 0;
        }
    }

    if (play->sceneId == SCENE_LON_LON_RANCH && GET_EVENTINF_INGO_RACE_STATE() == INGO_RACE_STATE_TRAPPED_WIN_EPONA &&
        !(event_check(EVENTCHKINF_EPONA_OBTAINED) || R_DEBUG_FORCE_EPONA_OBTAINED)) {
        this->stateFlags |= ENHORSE_FLAG_25;
    }

    Actor_set_scale(&this->actor, 0.01f);
    this->actor.gravity = -3.5f;
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_horse, 20.0f);
    this->action = ENHORSE_ACT_IDLE;
    this->actor.speed = 0.0f;
    ClObjPipe_ct(play, &this->cyl1);
    ClObjPipe_set5(play, &this->cyl1, &this->actor, &HorseAtOcInfoData);
    ClObjPipe_ct(play, &this->cyl2);
    ClObjPipe_set5(play, &this->cyl2, &this->actor, &HorseOcInfoData);
    ClObjJntSph_ct(play, &this->jntSph);
    ClObjJntSph_set5_nzm(play, &this->jntSph, &this->actor, &HorseAcOcInfoJntSphData, &this->jntSphList);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, CollisionBtlTbl_get(0xB), &HorseStatusData);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    this->playerControlled = false;

    if ((play->sceneId == SCENE_LON_LON_RANCH) && !IS_CUTSCENE_LAYER) {
        if (this->type == HORSE_HNI) {
            if (this->actor.world.rot.z == 0 || !IS_DAY) {
                Actor_delete(&this->actor);
                return;
            }
            if (event_check(EVENTCHKINF_EPONA_OBTAINED)) {
                Actor_delete(&this->actor);
                return;
            }
            if (this->actor.world.rot.z != 5) {
                Actor_delete(&this->actor);
                return;
            }
        } else if (!(event_check(EVENTCHKINF_EPONA_OBTAINED) || R_DEBUG_FORCE_EPONA_OBTAINED) && !IS_DAY) {
            Actor_delete(&this->actor);
            return;
        }
    } else if (play->sceneId == SCENE_STABLE) {
        if (IS_DAY || event_check(EVENTCHKINF_EPONA_OBTAINED) || R_DEBUG_FORCE_EPONA_OBTAINED ||
            !LINK_IS_ADULT) {
            Actor_delete(&this->actor);
            return;
        }
        this->stateFlags |= ENHORSE_UNRIDEABLE;
    }

    Skin_AnimationWorkBuffer2_ct(play, &this->skin, skel_info_tbl[this->type], anim_tbl_tbl[this->type][ENHORSE_ANIM_IDLE]);
    this->animationIdx = ENHORSE_ANIM_IDLE;
    Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx]);
    this->numBoosts = 6;
    this->boostRegenTime = 0;
    this->postDrawFunc = NULL;
    this->blinkTimer = 0;
    En_Horse_Actor_Demo_init(this, play);
    En_Horse_Actor_Race_init(this, play);
    En_Horse_Actor_Yabu_init(this, play);

    if (this->actor.params == HORSE_PTYPE_INACTIVE) {
        En_Horse_mode_sleep_init(this);
    } else if (this->actor.params == HORSE_PTYPE_INGO_SPAWNED_RIDING) {
        En_Horse_Actor_mode_race_run_start_init(this);
        this->rider = Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_IN, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, this->actor.shape.rot.x, this->actor.shape.rot.y, 1, 1);
        ASSERT(this->rider != NULL, "this->race.rider != NULL", "../z_en_horse.c", 3077);
        if (!GET_EVENTINF(EVENTINF_INGO_RACE_SECOND_RACE)) {
            this->ingoHorseMaxSpeed = 12.07f;
        } else {
            this->ingoHorseMaxSpeed = 12.625f;
        }
    } else if (this->actor.params == HORSE_PTYPE_7) {
        En_Horse_Actor_TD_move_init(this, play);
    } else if (this->actor.params == HORSE_PTYPE_HORSEBACK_ARCHERY) {
        En_Horse_Actor_mode_spot12_yabusame_init_start(this);
        yabusame_game_start(play);
    } else if (play->sceneId == SCENE_LON_LON_RANCH && !event_check(EVENTCHKINF_EPONA_OBTAINED) &&
               !R_DEBUG_FORCE_EPONA_OBTAINED) {
        En_Horse_Actor_mode_spot20_start_run_init(this);
    } else {
        if (play->sceneId == SCENE_LON_LON_BUILDINGS) {
            En_Horse_Actor_mode_base_wait_noclear_no_ride_request_init(this);
        } else if (play->sceneId == SCENE_GERUDOS_FORTRESS && this->type == HORSE_HNI) {
            En_Horse_Actor_mode_base_wait_noclear_no_ride_request_init(this);
        } else {
            En_Horse_Actor_mode_base_wait_init(this);
        }
    }
    this->actor.home.rot.z = this->actor.world.rot.z = this->actor.shape.rot.z = 0;
}

void En_Horse_Actor_dt(Actor* thisx, PlayState* play) {
    EnHorse* this = (EnHorse*)thisx;

    if (this->stateFlags & ENHORSE_DRAW) {
        Nai_StopAllObjFx(&this->unk_21C);
    }
    Skin_AnimationWorkBuffer2_dt(play, &this->skin);
    ClObjPipe_dt(play, &this->cyl1);
    ClObjPipe_dt(play, &this->cyl2);
    ClObjJntSph_dt_nzf(play, &this->jntSph);
}

void En_Horse_Actor_mode_GPS_move_trace_player(EnHorse* this, PlayState* play) {
    En_HL_trace_pos(this, play, &GET_PLAYER(play)->actor.world.pos, 400);
    if (this->stateFlags & ENHORSE_OBSTACLE) {
        this->actor.world.rot.y += 800.0f;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void En_Horse_Actor_mode_wait_lock_init(EnHorse* this) {
    if (this->action != ENHORSE_ACT_CS_UPDATE && this->action != ENHORSE_ACT_HBA) {
        if (!(argd_ride_flag[this->actor.params] == 0 || this->actor.params == HORSE_PTYPE_4)) {
            this->noInputTimer = 0;
            this->noInputTimerMax = 0;
        }
        this->prevAction = this->action;
        this->action = ENHORSE_ACT_FROZEN;
        this->cyl1.base.ocFlags1 &= ~OC1_ON;
        this->cyl2.base.ocFlags1 &= ~OC1_ON;
        this->jntSph.base.ocFlags1 &= ~OC1_ON;
        this->animationIdx = ENHORSE_ANIM_IDLE;
    }
}

void En_Horse_Actor_mode_base_wait_init_hold_frame(EnHorse* this, s32 anim, f32 morphFrames);
void En_Horse_Actor_mode_control_wait_init(EnHorse* this);
void En_Horse_Actor_mode_control_wait_init_hold_frame(EnHorse* this);
void En_Horse_Actor_mode_control_fast_run_no_timer_clear_init(EnHorse* this);

void En_Horse_Actor_mode_wait_lock(EnHorse* this, PlayState* play) {
    this->actor.speed = 0.0f;
    this->noInputTimer--;
    if (this->noInputTimer < 0) {
        this->cyl1.base.ocFlags1 |= OC1_ON;
        this->cyl2.base.ocFlags1 |= OC1_ON;
        this->jntSph.base.ocFlags1 |= OC1_ON;
        if (this->playerControlled == true) {
            this->stateFlags &= ~ENHORSE_FLAG_7;
            if (this->actor.params == HORSE_PTYPE_4) {
                En_Horse_Actor_mode_control_wait_init(this);
            } else if (this->actor.params == HORSE_PTYPE_PLAYER_SPAWNED_RIDING) {
                this->actor.params = HORSE_PTYPE_5;
                if (play->csCtx.state != CS_STATE_IDLE) {
                    En_Horse_Actor_mode_control_wait_init_hold_frame(this);
                } else {
                    this->actor.speed = 8.0f;
                    En_Horse_Actor_mode_control_fast_run_no_timer_clear_init(this);
                }
            } else if (this->prevAction == 2) {
                En_Horse_Actor_mode_control_wait_init_hold_frame(this);
            } else {
                En_Horse_Actor_mode_control_wait_init(this);
            }
            if (this->actor.params != HORSE_PTYPE_0) {
                this->actor.params = HORSE_PTYPE_0;
                return;
            }
        } else {
            if (this->prevAction == 5) {
                En_Horse_Actor_mode_base_wait_init_hold_frame(this, 0, 0);
                return;
            }
            if (this->prevAction == 6) {
                En_Horse_Actor_mode_base_wait_init_hold_frame(this, 0, 0);
                return;
            }
            En_Horse_Actor_mode_base_wait_init_hold_frame(this, 0, 0);
        }
    }
}

void stick_data_convert(Vec2f* curStick, f32* stickMag, s16* angle);

void En_Horse_Actor_stick2pos(EnHorse* this, PlayState* play, f32 brakeDecel, f32 brakeAngle, f32 minStickMag, f32 decel,
                         f32 baseSpeed, s16 turnSpeed) {
    s16* stickAnglePtr; // probably fake
    f32 stickMag;
    s16 stickAngle;
    f32 temp_f12;
    f32 traction;
    s16 turn;

    if (!En_HL_checkControl(this, play)) {
        if (this->actor.speed > 8) {
            this->actor.speed -= decel;
        } else if (this->actor.speed < 0) {
            this->actor.speed = 0;
        }

        return;
    }

    stickAnglePtr = &stickAngle;

    baseSpeed *= ground_speed_effect(this, play);
    stick_data_convert(&this->curStick, &stickMag, &stickAngle);
    if (cos_s(stickAngle) <= brakeAngle) {
        this->actor.speed -= brakeDecel;
        this->actor.speed = CLAMP_MIN(this->actor.speed, 0.0f);
        return;
    }

    if (stickMag < minStickMag) {
        this->stateFlags &= ~ENHORSE_BOOST;
        this->stateFlags &= ~ENHORSE_BOOST_DECEL;
        this->actor.speed -= decel;
        if (this->actor.speed < 0.0f) {
            this->actor.speed = 0.0f;
        }

        return;
    }

    if (this->stateFlags & ENHORSE_BOOST) {
        if ((16 - this->boostTimer) > 0) {
            this->actor.speed = (ground_speed_effect(this, play) * this->boostSpeed - this->actor.speed) /
                                    (16 - this->boostTimer) +
                                this->actor.speed;
        } else {
            this->actor.speed = ground_speed_effect(this, play) * this->boostSpeed;
        }

        if ((ground_speed_effect(this, play) * this->boostSpeed) <= this->actor.speed) {
            this->stateFlags &= ~ENHORSE_BOOST;
            this->stateFlags |= ENHORSE_BOOST_DECEL;
        }

    } else if (this->stateFlags & ENHORSE_BOOST_DECEL) {
        if (baseSpeed < this->actor.speed) {
            temp_f12 = this->actor.speed;
            this->actor.speed = temp_f12 - 0.06f;
        } else if (this->actor.speed < baseSpeed) {
            this->actor.speed = baseSpeed;
            this->stateFlags &= ~ENHORSE_BOOST_DECEL;
        }
    } else {
        this->actor.speed +=
            (this->actor.speed <= baseSpeed * (1.0f / 54.0f) * stickMag ? 1.0f : -1.0f) * 50.0f * 0.01f;
        if (baseSpeed < this->actor.speed) {
            this->actor.speed -= decel;
            if (this->actor.speed < baseSpeed) {
                this->actor.speed = baseSpeed;
            }
        }
    }

    temp_f12 = *stickAnglePtr * (1 / 32236.f);
    traction = 2.2f - (this->actor.speed * (1.0f / this->boostSpeed));
    turn = *stickAnglePtr * temp_f12 * temp_f12 * traction;
    turn = CLAMP(turn, -turnSpeed * traction, turnSpeed * traction);
    this->actor.world.rot.y += turn;
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void En_Horse_Actor_mode_control_wait_init(EnHorse* this) {
    this->skin.skelAnime.curFrame = 0.0f;
    En_Horse_Actor_mode_control_wait_init_hold_frame(this);
    this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
}

void En_Horse_Actor_mode_control_wait_init_hold_frame(EnHorse* this) {
    f32 curFrame;

    this->action = ENHORSE_ACT_MOUNTED_IDLE;
    this->animationIdx = ENHORSE_ANIM_IDLE;
    if ((this->curFrame > 35.0f && this->type == HORSE_EPONA) || (this->curFrame > 28.0f && this->type == HORSE_HNI)) {
        if (!(this->stateFlags & ENHORSE_SANDDUST_SOUND)) {
            this->stateFlags |= ENHORSE_SANDDUST_SOUND;
            Nai_FxFlagEntry(NA_SE_EV_HORSE_SANDDUST, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }
    curFrame = this->skin.skelAnime.curFrame;
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, curFrame,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void En_Horse_Actor_mode_control_back_walk_init(EnHorse* this);
void En_Horse_Actor_mode_control_rot_wait_init(EnHorse* this);
void En_Horse_Actor_mode_control_start_walk_init(EnHorse* this);
void En_Horse_Actor_mode_control_wait02_init(EnHorse* this);
void En_Horse_Actor_mode_control_back_walk_no_timer_clear_init(EnHorse* this);
void En_Horse_Actor_mode_control_walk_init(EnHorse* this);
void En_Horse_Actor_mode_control_wait02_init_hold_frame(EnHorse* this);
void En_Horse_Actor_mode_control_walk_no_timer_clear_init(EnHorse* this);

void En_Horse_Actor_mode_control_wait(EnHorse* this, PlayState* play) {
    f32 mag;
    s16 angle = 0;

    this->actor.speed = 0;
    stick_data_convert(&this->curStick, &mag, &angle);
    if (mag > 10.0f && En_HL_checkControl(this, play) == true) {
        if (cos_s(angle) <= -0.5f) {
            En_Horse_Actor_mode_control_back_walk_init(this);
        } else if (cos_s(angle) <= 0.7071) { // cos(45 degrees)
            En_Horse_Actor_mode_control_rot_wait_init(this);
        } else {
            En_Horse_Actor_mode_control_start_walk_init(this);
        }
    }
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        En_Horse_Actor_mode_control_wait02_init(this);
    }
}

void En_Horse_Actor_mode_control_wait02_init(EnHorse* this) {
    this->skin.skelAnime.curFrame = 0.0f;
    En_Horse_Actor_mode_control_wait02_init_hold_frame(this);
}

void En_Horse_Actor_mode_control_wait02_init_hold_frame(EnHorse* this) {
    f32 curFrame;

    this->action = ENHORSE_ACT_MOUNTED_IDLE_WHINNEYING;
    this->animationIdx = ENHORSE_ANIM_WHINNEY;
    curFrame = this->skin.skelAnime.curFrame;
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, curFrame,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
    this->unk_21C = this->unk_228;
    if (this->stateFlags & ENHORSE_DRAW) {
        Nai_FxFlagEntry(NA_SE_EV_HORSE_GROAN, &this->unk_21C, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

void En_Horse_Actor_mode_control_wait02(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle = 0;

    this->actor.speed = 0;
    stick_data_convert(&this->curStick, &stickMag, &stickAngle);
    if (stickMag > 10.0f && En_HL_checkControl(this, play) == true) {
        if (cos_s(stickAngle) <= -0.5f) {
            En_Horse_Actor_mode_control_back_walk_init(this);
        } else if (cos_s(stickAngle) <= 0.7071) { // cos(45 degrees)
            En_Horse_Actor_mode_control_rot_wait_init(this);
        } else {
            En_Horse_Actor_mode_control_start_walk_init(this);
        }
    }
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        En_Horse_Actor_mode_control_wait_init(this);
    }
}

void En_Horse_Actor_mode_control_rot_wait_init(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_TURN;
    this->soundTimer = 0;
    this->animationIdx = ENHORSE_ANIM_WALK;
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void En_Horse_Actor_mode_control_rot_wait(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 clampedYaw;
    s16 stickAngle;

    this->actor.speed = 0;
    En_Horse_setWalkSound(this);
    stick_data_convert(&this->curStick, &stickMag, &stickAngle);
    if (stickMag > 10.0f) {
        if (!En_HL_checkControl(this, play)) {
            En_Horse_Actor_mode_control_wait_init(this);
        } else if (cos_s(stickAngle) <= -0.5f) {
            En_Horse_Actor_mode_control_back_walk_init(this);
        } else if (cos_s(stickAngle) <= 0.7071) { // cos(45 degrees)
            clampedYaw = CLAMP(stickAngle, -800.0f, 800.0f);
            this->actor.world.rot.y += clampedYaw;
            this->actor.shape.rot.y = this->actor.world.rot.y;
        } else {
            En_Horse_Actor_mode_control_walk_init(this);
        }
    }

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        if (cos_s(stickAngle) <= 0.7071) { // cos(45 degrees)
            En_Horse_Actor_mode_control_rot_wait_init(this);
        } else {
            En_Horse_Actor_mode_control_wait_init(this);
        }
    }
}

void En_Horse_Actor_mode_control_start_walk_init(EnHorse* this) {
    En_Horse_Actor_mode_control_walk_init(this);

    if (!(this->stateFlags & ENHORSE_FLAG_8) && !(this->stateFlags & ENHORSE_FLAG_9)) {
        this->stateFlags |= ENHORSE_FLAG_9;
        this->waitTimer = 8;
        return;
    }
    this->waitTimer = 0;
}

void En_Horse_Actor_mode_control_walk_init(EnHorse* this) {
    this->noInputTimer = 0;
    this->noInputTimerMax = 0;
    En_Horse_Actor_mode_control_walk_no_timer_clear_init(this);
}

void En_Horse_Actor_mode_control_walk_no_timer_clear_init(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_WALK;
    this->soundTimer = 0;
    this->animationIdx = ENHORSE_ANIM_WALK;
    this->waitTimer = 0;
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void En_Horse_Actor_mode_control_walk_direct_init(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_WALK;
    this->soundTimer = 0;
    this->animationIdx = ENHORSE_ANIM_WALK;
    this->waitTimer = 0;
    Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx]);
}

void En_Horse_Actor_mode_control_slow_run_init(EnHorse* this);

void En_Horse_Actor_mode_control_walk(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    En_Horse_setWalkSound(this);
    stick_data_convert(&this->curStick, &stickMag, &stickAngle);
    if (this->noInputTimerMax == 0.0f ||
        (this->noInputTimer > 0.0f && this->noInputTimer < this->noInputTimerMax - 20.0f)) {
        En_Horse_Actor_stick2pos(this, play, 0.3f, -0.5f, 10.0f, 0.06f, 3.0f, 400);
    } else {
        this->actor.speed = 3.0f;
    }

    if (this->actor.speed == 0.0f) {
        this->stateFlags &= ~ENHORSE_FLAG_9;
        En_Horse_Actor_mode_control_wait_init(this);
        this->noInputTimer = 0;
        this->noInputTimerMax = 0;
    } else if (this->actor.speed > 3.0f) {
        this->stateFlags &= ~ENHORSE_FLAG_9;
        En_Horse_Actor_mode_control_slow_run_init(this);
        this->noInputTimer = 0;
        this->noInputTimerMax = 0;
    }

    if (this->noInputTimer > 0.0f) {
        this->noInputTimer--;
        if (this->noInputTimer <= 0.0f) {
            this->noInputTimerMax = 0;
        }
    }

    if (this->waitTimer <= 0) {
        this->stateFlags &= ~ENHORSE_FLAG_9;
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.75f;
        if (Skeleton_Info2_anime_play(&this->skin.skelAnime) || this->actor.speed == 0.0f) {
            if (this->noInputTimer <= 0.0f) {
                if (this->actor.speed > 3.0f) {
                    En_Horse_Actor_mode_control_slow_run_init(this);
                    this->noInputTimer = 0;
                    this->noInputTimerMax = 0;
                } else if ((stickMag < 10.0f) || (cos_s(stickAngle) <= -0.5f)) {
                    En_Horse_Actor_mode_control_wait_init(this);
                    this->noInputTimer = 0;
                    this->noInputTimerMax = 0;
                } else {
                    En_Horse_Actor_mode_control_walk_direct_init(this);
                }
            }
        }
    } else {
        this->actor.speed = 0.0f;
        this->waitTimer--;
    }
}

void En_Horse_Actor_mode_control_slow_run_init(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_TROT;
    this->animationIdx = ENHORSE_ANIM_TROT;
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void En_Horse_Actor_mode_control_slow_run_direct_init(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_TROT;
    this->animationIdx = ENHORSE_ANIM_TROT;
    Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx]);
}

void En_Horse_Actor_mode_control_fast_run_init(EnHorse* this);

void En_Horse_Actor_mode_control_slow_run(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    En_Horse_Actor_stick2pos(this, play, 0.3f, -0.5f, 10.0f, 0.06f, 6.0f, 400);
    stick_data_convert(&this->curStick, &stickMag, &stickAngle);
    if (this->actor.speed < 3.0f) {
        En_Horse_Actor_mode_control_walk_init(this);
    }

    this->skin.skelAnime.playSpeed = this->actor.speed * 0.375f;
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        En_Horse_setSlowRunSound(this);
        z_vibctl2_vib_setQ(0.0f, 60, 8, 255);
        if (this->actor.speed >= 6.0f) {
            En_Horse_Actor_mode_control_fast_run_init(this);
        } else if (this->actor.speed < 3.0f) {
            En_Horse_Actor_mode_control_walk_init(this);
        } else {
            En_Horse_Actor_mode_control_slow_run_direct_init(this);
        }
    }
}

void En_Horse_Actor_mode_control_fast_run_init(EnHorse* this) {
    this->noInputTimerMax = this->noInputTimer = 0;
    En_Horse_Actor_mode_control_fast_run_no_timer_clear_init(this);
}

void En_Horse_Actor_mode_control_fast_run_no_timer_clear_init(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_GALLOP;
    this->animationIdx = ENHORSE_ANIM_GALLOP;
    this->unk_234 = 0;
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void En_Horse_Actor_mode_control_fast_run_direct_init(EnHorse* this) {
    this->noInputTimerMax = this->noInputTimer = 0;
    this->action = ENHORSE_ACT_MOUNTED_GALLOP;
    this->animationIdx = ENHORSE_ANIM_GALLOP;
    this->unk_234 = 0;
    Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx]);
}

void En_Horse_Actor_move_control_fast_run_after_jump_init(EnHorse* this, PlayState* play) {
    Vec3s* jointTable;
    f32 y;

    this->action = ENHORSE_ACT_MOUNTED_GALLOP;
    this->animationIdx = ENHORSE_ANIM_GALLOP;
    Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx]);
    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable->y;
    this->riderPos.y += y * 0.01f;
    this->postDrawFunc = NULL;
}

void En_Horse_Actor_mode_control_stop_init(EnHorse* this, PlayState* play);

void En_Horse_Actor_mode_control_fast_run(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    stick_data_convert(&this->curStick, &stickMag, &stickAngle);

    if (this->noInputTimer <= 0.0f) {
        En_Horse_Actor_stick2pos(this, play, 0.3f, -0.5f, 10.0f, 0.06f, 8.0f, 0x190);
    } else if (this->noInputTimer > 0.0f) {
        this->noInputTimer--;
        this->actor.speed = 8.0f;
    }
    if (this->actor.speed < 6.0f) {
        En_Horse_Actor_mode_control_slow_run_init(this);
    }

    this->skin.skelAnime.playSpeed = this->actor.speed * 0.3f;
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        En_Horse_setFastRunSound(this);
        z_vibctl2_vib_setQ(0, 120, 8, 255);
        if (En_HL_checkControl(this, play) == true) {
            if (stickMag >= 10.0f && cos_s(stickAngle) <= -0.5f) {
                En_Horse_Actor_mode_control_stop_init(this, play);
            } else if (this->actor.speed < 6.0f) {
                En_Horse_Actor_mode_control_slow_run_init(this);
            } else {
                En_Horse_Actor_mode_control_fast_run_direct_init(this);
            }
            return;
        }
        En_Horse_Actor_mode_control_fast_run_direct_init(this);
    }
}

void En_Horse_Actor_mode_control_standing_init(EnHorse* this) {
    this->action = ENHORSE_ACT_MOUNTED_REARING;
    this->animationIdx = ENHORSE_ANIM_REARING;
    this->stateFlags &= ~ENHORSE_LAND2_SOUND;
    this->unk_21C = this->unk_228;
    if (this->stateFlags & ENHORSE_DRAW) {
        Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
    z_vibctl2_vib_setQ(0.0f, 180, 20, 100);
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void En_Horse_Actor_mode_control_standing(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;

    this->actor.speed = 0.0f;
    if (this->curFrame > 25.0f) {
        if (!(this->stateFlags & ENHORSE_LAND2_SOUND)) {
            this->stateFlags |= ENHORSE_LAND2_SOUND;
            Nai_FxFlagEntry(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            z_vibctl2_vib_setQ(0.0f, 180, 20, 100);
        }
    }

    stick_data_convert(&this->curStick, &stickMag, &stickAngle);
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        if (En_HL_checkControl(this, play) == true) {
            if (this->stateFlags & ENHORSE_FORCE_REVERSING) {
                this->noInputTimer = 100;
                this->noInputTimerMax = 100;
                this->stateFlags &= ~ENHORSE_FORCE_REVERSING;
                En_Horse_Actor_mode_control_back_walk_no_timer_clear_init(this);
            } else if (this->stateFlags & ENHORSE_FORCE_WALKING) {
                this->noInputTimer = 100;
                this->noInputTimerMax = 100;
                this->stateFlags &= ~ENHORSE_FORCE_WALKING;
                En_Horse_Actor_mode_control_walk_no_timer_clear_init(this);
            } else if (cos_s(stickAngle) <= -0.5f) {
                En_Horse_Actor_mode_control_back_walk_init(this);
            } else {
                En_Horse_Actor_mode_control_wait_init(this);
            }
            return;
        }
        En_Horse_Actor_mode_control_wait_init(this);
    }
}

void En_Horse_Actor_mode_control_stop_init(EnHorse* this, PlayState* play) {
    this->action = ENHORSE_ACT_STOPPING;
    this->animationIdx = ENHORSE_ANIM_STOPPING;

    Nai_FxFlagEntry(NA_SE_EV_HORSE_SLIP, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.5f, 0.0f,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);

    this->stateFlags |= ENHORSE_STOPPING_NEIGH_SOUND;
    this->stateFlags &= ~ENHORSE_BOOST;
}

void En_Horse_Actor_mode_control_stop(EnHorse* this, PlayState* play) {
    if (this->actor.speed > 0.0f) {
        this->actor.speed -= 0.6f;
        if (this->actor.speed < 0.0f) {
            this->actor.speed = 0.0f;
        }
    }

    if (this->stateFlags & ENHORSE_STOPPING_NEIGH_SOUND && this->skin.skelAnime.curFrame > 29.0f) {
        this->actor.speed = 0.0f;
        if (fqrand() > 0.5) {
            this->unk_21C = this->unk_228;
            if (this->stateFlags & ENHORSE_DRAW) {
                Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
            z_vibctl2_vib_setQ(0.0f, 180, 20, 100);
            this->stateFlags &= ~ENHORSE_STOPPING_NEIGH_SOUND;
        } else {
            En_Horse_Actor_mode_control_wait_init(this);
        }
    }

    if (this->skin.skelAnime.curFrame > 29.0f) {
        this->actor.speed = 0.0f;
    } else if (this->actor.speed > 3.0f && this->stateFlags & ENHORSE_FORCE_REVERSING) {
        this->actor.speed = 3.0f;
    }

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        if (this->stateFlags & ENHORSE_FORCE_REVERSING) {
            this->noInputTimer = 100;
            this->noInputTimerMax = 100;
            En_Horse_Actor_mode_control_back_walk_no_timer_clear_init(this);
            this->stateFlags &= ~ENHORSE_FORCE_REVERSING;
        } else {
            En_Horse_Actor_mode_control_wait_init(this);
        }
    }
}

void En_Horse_Actor_mode_control_back_walk_init(EnHorse* this) {
    this->noInputTimerMax = this->noInputTimer = 0;
    En_Horse_Actor_mode_control_back_walk_no_timer_clear_init(this);
}

void En_Horse_Actor_mode_control_back_walk_no_timer_clear_init(EnHorse* this) {
    this->action = ENHORSE_ACT_REVERSE;
    this->animationIdx = ENHORSE_ANIM_WALK;
    this->soundTimer = 0;
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_LOOP, -3.0f);
}

void En_Horse_Actor_mode_control_back_walk(EnHorse* this, PlayState* play) {
    f32 stickMag;
    s16 stickAngle;
    s16 turnAmount;
    Player* player = GET_PLAYER(play);

    En_Horse_setWalkSound(this);
    stick_data_convert(&this->curStick, &stickMag, &stickAngle);
    if (En_HL_checkControl(this, play) == true) {
        if (this->noInputTimerMax == 0.0f ||
            (this->noInputTimer > 0.0f && this->noInputTimer < this->noInputTimerMax - 20.0f)) {
            if (stickMag < 10.0f && this->noInputTimer <= 0.0f) {
                En_Horse_Actor_mode_control_wait_init(this);
                this->actor.speed = 0.0f;
                return;
            }
            if (stickMag < 10.0f) {
                stickAngle = -0x7FFF;
            } else if (cos_s(stickAngle) > -0.5f) {
                this->noInputTimerMax = 0;
                En_Horse_Actor_mode_control_wait_init(this);
                this->actor.speed = 0.0f;
                return;
            }
        } else if (stickMag < 10.0f) {
            stickAngle = -0x7FFF;
        }
    } else if (player->actor.flags & ACTOR_FLAG_TALK) {
        En_Horse_Actor_mode_control_wait_init(this);
        this->actor.speed = 0.0f;
        return;
    } else {
        stickAngle = -0x7FFF;
    }

    this->actor.speed = -2.0f;
    turnAmount = 0x7FFF - stickAngle;
    turnAmount = CLAMP(turnAmount, -1200.0f, 1200.0f);
    this->actor.world.rot.y += turnAmount;
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->noInputTimer > 0.0f) {
        this->noInputTimer--;
        if (this->noInputTimer <= 0.0f) {
            this->noInputTimerMax = 0;
        }
    }
    this->skin.skelAnime.playSpeed = this->actor.speed * 0.5f * 1.5f;
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime) && (f32)this->noInputTimer <= 0.0f &&
        En_HL_checkControl(this, play) == true) {
        if (stickMag > 10.0f && cos_s(stickAngle) <= -0.5f) {
            this->noInputTimerMax = 0;
            En_Horse_Actor_mode_control_back_walk_init(this);
        } else if (stickMag < 10.0f) {
            this->noInputTimerMax = 0;
            En_Horse_Actor_mode_control_wait_init(this);
        } else {
            En_Horse_Actor_mode_control_back_walk_no_timer_clear_init(this);
        }
    }
}

void En_Horse_Actor_mode_demo_jump100_no_set_frame_da(EnHorse* this, PlayState* play);

void En_Horse_Actor_mode_demo_jump100_da(EnHorse* this, PlayState* play) {
    this->skin.skelAnime.curFrame = 0.0f;
    En_Horse_Actor_mode_demo_jump100_no_set_frame_da(this, play);
}

void En_Horse_Actor_mode_demo_jump100_no_set_frame_da(EnHorse* this, PlayState* play) {
    f32 curFrame;
    Vec3s* jointTable;
    f32 y;

    this->action = ENHORSE_ACT_LOW_JUMP;
    this->animationIdx = ENHORSE_ANIM_LOW_JUMP;
    curFrame = this->skin.skelAnime.curFrame;
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.5f, curFrame,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);

    this->postDrawFunc = NULL;
    this->jumpStartY = this->actor.world.pos.y;

    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0;

    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable->y;
    this->riderPos.y -= y * 0.01f;

    Nai_FxFlagEntry(NA_SE_EV_HORSE_JUMP, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
    z_vibctl2_vib_setQ(0.0f, 170, 10, 10);
}

void En_Horse_Actor_mode_demo_jump100_init(EnHorse* this) {
}

void En_Horse_Actor_mode_demo_jump100(EnHorse* this, PlayState* play) {
    Vec3f pad;
    Vec3s* jointTable;
    f32 curFrame;
    f32 y;

    curFrame = this->skin.skelAnime.curFrame;
    this->stateFlags |= ENHORSE_JUMPING;
    this->actor.speed = 12.0f;
    if (curFrame > 17.0f) {
        this->actor.gravity = -3.5f;
        if (this->actor.velocity.y == 0) {
            this->actor.velocity.y = -6.0f;
        }
        if (this->actor.world.pos.y < this->actor.floorHeight + 90.0f) {
            this->skin.skelAnime.playSpeed = 1.5f;
        } else {
            this->skin.skelAnime.playSpeed = 0;
        }
    } else {
        jointTable = this->skin.skelAnime.jointTable;
        y = jointTable->y;
        this->actor.world.pos.y = this->jumpStartY + y * 0.01f;
    }

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime) ||
        (curFrame > 17.0f && this->actor.world.pos.y < this->actor.floorHeight - this->actor.velocity.y + 80.0f)) {
        Nai_FxFlagEntry(NA_SE_EV_HORSE_LAND, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        z_vibctl2_vib_setQ(0.0f, 255, 10, 80);
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->actor.gravity = -3.5f;
        this->actor.world.pos.y = this->actor.floorHeight;
        Effect_SS_Dust_li_spread20(play, 25.0f, &this->actor.world.pos);
        En_Horse_Actor_move_control_fast_run_after_jump_init(this, play);
    }
}

void En_Horse_Actor_mode_demo_jump200_no_set_frame_da(EnHorse* this, PlayState* play);

void En_Horse_Actor_mode_demo_jump200_da(EnHorse* this, PlayState* play) {
    this->skin.skelAnime.curFrame = 0.0f;
    En_Horse_Actor_mode_demo_jump200_no_set_frame_da(this, play);
}

void En_Horse_Actor_mode_demo_jump200_no_set_frame_da(EnHorse* this, PlayState* play) {
    f32 curFrame;
    Vec3s* jointTable;
    f32 y;

    this->action = ENHORSE_ACT_HIGH_JUMP;
    this->animationIdx = ENHORSE_ANIM_HIGH_JUMP;
    curFrame = this->skin.skelAnime.curFrame;
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.5f, curFrame,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);

    this->jumpStartY = this->actor.world.pos.y;
    this->postDrawFunc = NULL;

    this->actor.gravity = 0;
    this->actor.velocity.y = 0.0f;

    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable->y;
    this->riderPos.y -= y * 0.01f;

    this->stateFlags |= ENHORSE_CALC_RIDER_POS;
    Nai_FxFlagEntry(NA_SE_EV_HORSE_JUMP, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
    z_vibctl2_vib_setQ(0.0f, 170, 10, 10);
}

void En_Horse_Actor_mode_demo_jump200_init(EnHorse* this) {
}

void En_Horse_Actor_mode_demo_jump200(EnHorse* this, PlayState* play) {
    Vec3f pad;
    Vec3s* jointTable;
    f32 curFrame;
    f32 y;

    curFrame = this->skin.skelAnime.curFrame;
    this->stateFlags |= ENHORSE_JUMPING;
    this->actor.speed = 13.0f;
    if (curFrame > 23.0f) {
        this->actor.gravity = -3.5f;
        if (this->actor.velocity.y == 0) {
            this->actor.velocity.y = -10.5f;
        }

        if (this->actor.world.pos.y < this->actor.floorHeight + 90.0f) {
            this->skin.skelAnime.playSpeed = 1.5f;
        } else {
            this->skin.skelAnime.playSpeed = 0;
        }
    } else {
        jointTable = this->skin.skelAnime.jointTable;
        y = jointTable->y;
        this->actor.world.pos.y = this->jumpStartY + y * 0.01f;
    }

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime) ||
        (curFrame > 23.0f && this->actor.world.pos.y < this->actor.floorHeight - this->actor.velocity.y + 80.0f)) {
        Nai_FxFlagEntry(NA_SE_EV_HORSE_LAND, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        z_vibctl2_vib_setQ(0.0f, 255, 10, 80);
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->actor.gravity = -3.5f;
        this->actor.world.pos.y = this->actor.floorHeight;
        Effect_SS_Dust_li_spread20(play, 25.0f, &this->actor.world.pos);
        En_Horse_Actor_move_control_fast_run_after_jump_init(this, play);
    }
}

void En_Horse_mode_sleep_init(EnHorse* this) {
    this->cyl1.base.ocFlags1 &= ~OC1_ON;
    this->cyl2.base.ocFlags1 &= ~OC1_ON;
    this->jntSph.base.ocFlags1 &= ~OC1_ON;
    this->action = ENHORSE_ACT_INACTIVE;
    this->animationIdx = ENHORSE_ANIM_WALK;
    this->stateFlags |= ENHORSE_INACTIVE;
    this->followTimer = 0;
}

void En_Horse_Actor_mode_base_move_init(EnHorse* this, PlayState* play);

void En_Horse_mode_sleep(EnHorse* this, PlayState* play2) {
    PlayState* play = play2;

    if (R_EPONAS_SONG_PLAYED && this->type == HORSE_EPONA) {
        R_EPONAS_SONG_PLAYED = false;
        if (En_Horse_Actor_setKusabuePos(this, play)) {
#if OOT_VERSION >= PAL_1_0
            Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
#endif
            this->stateFlags &= ~ENHORSE_INACTIVE;
            z_common_data.save.info.horseData.sceneId = play->sceneId;

            // Focus the camera on Epona
            lockCamera(play->cameraPtrs[CAM_ID_MAIN], CAM_VIEW_TARGET, &this->actor);
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_TURN_AROUND);
            setCameraData(play->cameraPtrs[CAM_ID_MAIN], 4, NULL, NULL, 0x51, 0, 0);
        }
    }
    if (!(this->stateFlags & ENHORSE_INACTIVE)) {
        this->followTimer = 0;
        En_Horse_Actor_mode_base_move_init(this, play);
        this->actor.params = HORSE_PTYPE_0;
        this->cyl1.base.ocFlags1 |= OC1_ON;
        this->cyl2.base.ocFlags1 |= OC1_ON;
        this->jntSph.base.ocFlags1 |= OC1_ON;
    }
}

void En_Horse_Actor_mode_base_wait_init_general(EnHorse* this, s32 anim, f32 morphFrames, f32 startFrame) {
    this->action = ENHORSE_ACT_IDLE;
    this->actor.speed = 0.0f;
    if (anim != ENHORSE_ANIM_IDLE && anim != ENHORSE_ANIM_WHINNEY && anim != ENHORSE_ANIM_REARING) {
        anim = ENHORSE_ANIM_IDLE;
    }
    if (anim != this->animationIdx) {
        this->animationIdx = anim;
        if (this->animationIdx == ENHORSE_ANIM_IDLE) {
            this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
        } else if (this->animationIdx == ENHORSE_ANIM_WHINNEY) {
            this->unk_21C = this->unk_228;
            if (this->stateFlags & ENHORSE_DRAW) {
                Nai_FxFlagEntry(NA_SE_EV_HORSE_GROAN, &this->unk_21C, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
        } else if (this->animationIdx == ENHORSE_ANIM_REARING) {
            this->unk_21C = this->unk_228;
            if (this->stateFlags & ENHORSE_DRAW) {
                Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
            this->stateFlags &= ~ENHORSE_LAND2_SOUND;
        }

        Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, startFrame,
                         Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE,
                         morphFrames);
    }
}

void En_Horse_Actor_mode_base_wait_init_hold_frame(EnHorse* this, s32 anim, f32 morphFrames) {
    En_Horse_Actor_mode_base_wait_init_general(this, anim, morphFrames, this->curFrame);
}

void En_Horse_Actor_mode_base_wait_noclear_no_ride_request_init(EnHorse* this) {
    this->animationIdx = ENHORSE_ANIM_WALK; // this forces anim 0 to play from the beginning
    En_Horse_Actor_mode_base_wait_init_general(this, this->animationIdx, 0, 0);
}

void En_Horse_Actor_mode_base_wait_init(EnHorse* this) {
    En_Horse_Actor_mode_base_wait_noclear_no_ride_request_init(this);
    this->stateFlags &= ~ENHORSE_UNRIDEABLE;
}

void En_Horse_Actor_mode_base_move_init_general(EnHorse* this, s32 animId, f32 morphFrames, f32 startFrame);

void En_Horse_Actor_mode_base_wait(EnHorse* this, PlayState* play) {
    this->actor.speed = 0.0f;
    En_Horse_Actor_setWaitSound(this, play);

    if (R_EPONAS_SONG_PLAYED && this->type == HORSE_EPONA) {
        R_EPONAS_SONG_PLAYED = false;
        if (!check_camera_view(play, this, &this->actor.world.pos)) {
            if (En_Horse_Actor_setKusabuePos(this, play)) {
#if OOT_VERSION >= PAL_1_0
                Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->actor.projectedPos, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
#endif
                this->followTimer = 0;
                En_Horse_Actor_mode_base_move_init(this, play);
                lockCamera(play->cameraPtrs[CAM_ID_MAIN], CAM_VIEW_TARGET, &this->actor);
                changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_TURN_AROUND);
                setCameraData(play->cameraPtrs[CAM_ID_MAIN], 4, NULL, NULL, 0x51, 0, 0);
            }
        } else {
            Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            this->followTimer = 0;
            En_Horse_Actor_mode_base_move_init_general(this, 6, -3.0f, 0.0f);
        }
    }

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        s32 idleAnimIdx = 0;

        if (this->animationIdx != ENHORSE_ANIM_IDLE) {
            if (this->animationIdx == ENHORSE_ANIM_WHINNEY) {
                idleAnimIdx = 1;
            } else if (this->animationIdx == ENHORSE_ANIM_REARING) {
                idleAnimIdx = 2;
            }
        }

        // Play one of the two other idle animations
        En_Horse_Actor_mode_base_wait_init_general(this, next_anim[(fqrand() > 0.5f ? 0 : 1) + idleAnimIdx * 2], 0.0f, 0.0f);
    }
}

void En_Horse_Actor_mode_base_move_init_general(EnHorse* this, s32 animId, f32 morphFrames, f32 startFrame) {
    this->action = ENHORSE_ACT_FOLLOW_PLAYER;
    this->stateFlags &= ~ENHORSE_TURNING_TO_PLAYER;
    if (animId != ENHORSE_ANIM_TROT && animId != ENHORSE_ANIM_GALLOP && animId != ENHORSE_ANIM_WALK) {
        animId = ENHORSE_ANIM_WALK;
    }
    if (this->animationIdx != animId) {
        this->animationIdx = animId;
        Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, startFrame,
                         Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE,
                         morphFrames);
    } else {
        Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, startFrame,
                         Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE,
                         0.0f);
    }
}

void En_Horse_Actor_mode_base_move_init(EnHorse* this, PlayState* play) {
    s32 animId = ENHORSE_ANIM_WALK;
    f32 distToPlayer;

    distToPlayer = Actor_search_actor_distanceXZ(&this->actor, &GET_PLAYER(play)->actor);
    if (distToPlayer > 400.0f) {
        animId = ENHORSE_ANIM_GALLOP;
    } else if (!(distToPlayer <= 300.0f)) {
        if (distToPlayer <= 400.0f) {
            animId = ENHORSE_ANIM_TROT;
        }
    }

    if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        if (distToPlayer > 400.0f) {
            animId = ENHORSE_ANIM_GALLOP;
        } else {
            animId = ENHORSE_ANIM_TROT;
        }
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        if (distToPlayer > 400.0f) {
            animId = ENHORSE_ANIM_GALLOP;
        } else if (distToPlayer < 300.0f) {
            animId = ENHORSE_ANIM_WALK;
        } else {
            animId = ENHORSE_ANIM_TROT;
        }
    } else if (this->animationIdx == ENHORSE_ANIM_WALK) {
        if (distToPlayer > 300.0f) {
            animId = ENHORSE_ANIM_TROT;
        } else {
            animId = ENHORSE_ANIM_WALK;
        }
    }
    En_Horse_Actor_mode_base_move_init_general(this, animId, -3.0f, 0.0f);
}

void En_Horse_Actor_mode_base_move(EnHorse* this, PlayState* play) {
    f32 distToPlayer;

    R_EPONAS_SONG_PLAYED = false;
    distToPlayer = Actor_search_actor_distanceXZ(&this->actor, &GET_PLAYER(play)->actor);

    // First rotate if the player is behind
    if ((this->playerDir == PLAYER_DIR_BACK_R || this->playerDir == PLAYER_DIR_BACK_L) &&
        (distToPlayer > 300.0f && !(this->stateFlags & ENHORSE_TURNING_TO_PLAYER))) {
        f32 angleDiff;

        this->animationIdx = ENHORSE_ANIM_REARING;
        this->stateFlags |= ENHORSE_TURNING_TO_PLAYER;
        this->angleToPlayer = Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor);
        angleDiff = (f32)this->angleToPlayer - (f32)this->actor.world.rot.y;
        if (angleDiff > 32767.f) {
            angleDiff -= 32767.0f;
        } else if (angleDiff < -32767) {
            angleDiff += 32767;
        }

        this->followPlayerTurnSpeed =
            angleDiff / Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]);
        Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx]);
        this->skin.skelAnime.playSpeed = 1.0f;
        this->stateFlags &= ~ENHORSE_LAND2_SOUND;
        this->unk_21C = this->unk_228;
#if OOT_VERSION < PAL_1_0
        if (this->stateFlags & ENHORSE_DRAW) {
            Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
#endif
    } else if (this->stateFlags & ENHORSE_TURNING_TO_PLAYER) {
        this->actor.world.rot.y += this->followPlayerTurnSpeed;
        this->actor.shape.rot.y = this->actor.world.rot.y;
        if (this->curFrame > 25.0f) {
            if (!(this->stateFlags & ENHORSE_LAND2_SOUND)) {
                this->stateFlags |= ENHORSE_LAND2_SOUND;
                Nai_FxFlagEntry(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
        }
    } else {
        En_Horse_Actor_mode_GPS_move_trace_player(this, play);
    }

    if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        this->actor.speed = 8;
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.3f;
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        this->actor.speed = 6;
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.375f;
    } else if (this->animationIdx == ENHORSE_ANIM_WALK) {
        this->actor.speed = 3;
        En_Horse_setWalkSound(this);
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.75f;
    } else {
        this->actor.speed = 0;
        this->skin.skelAnime.playSpeed = 1.0f;
    }

    if (!(this->stateFlags & ENHORSE_TURNING_TO_PLAYER) && ++this->followTimer > 300) {
        En_Horse_Actor_mode_base_wait_init(this);
        this->unk_21C = this->unk_228;

        if (this->stateFlags & ENHORSE_DRAW) {
            Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
            En_Horse_setFastRunSound(this);
        } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
            En_Horse_setSlowRunSound(this);
        }
        this->stateFlags &= ~ENHORSE_TURNING_TO_PLAYER;
        if (distToPlayer < 100.0f) {
            En_Horse_Actor_mode_base_wait_init(this);
        } else {
            En_Horse_Actor_mode_base_move_init(this, play);
        }
    }
}

void En_Horse_Actor_mode_race_run_init(EnHorse* this);

void En_Horse_Actor_mode_race_run_start_init(EnHorse* this) {
    this->curRaceWaypoint = 0;
    this->soundTimer = 0;
    this->actor.speed = 0.0f;
    En_Horse_Actor_mode_race_run_init(this);
    this->unk_21C = this->unk_228;
    if (this->stateFlags & ENHORSE_DRAW) {
        Nai_FxFlagEntry(NA_SE_IT_INGO_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

void Horse_Anim2In_anim_conv(s32 index, f32 curFrame, s32 arg2, s16* animIdxOut, f32* curFrameOut) {
    *animIdxOut = h2i_tbl[index];
    *curFrameOut = curFrame;
    if ((index == 3) || (index == 7) || (index == 8) || (index == 4)) {
        *curFrameOut = 0.0f;
    }
    if (arg2 == 1) {
        if (index == 5) {
            *animIdxOut = 4;
            *curFrameOut = curFrame;
        } else if (index == 6) {
            *animIdxOut = 3;
            *curFrameOut = curFrame;
        }
    }
}

void En_Horse_Actor_mode_race_run_init(EnHorse* this) {
    s32 animChanged = 0;
    f32 animSpeed;

    this->action = ENHORSE_ACT_INGO_RACE;
    this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
    if (this->actor.speed == 0.0f) {
        if (this->animationIdx != ENHORSE_ANIM_IDLE) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_IDLE;
    } else if (this->actor.speed <= 3.0f) {
        if (this->animationIdx != ENHORSE_ANIM_WALK) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_WALK;
    } else if (this->actor.speed <= 6.0f) {
        if (this->animationIdx != ENHORSE_ANIM_TROT) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_TROT;
    } else {
        if (this->animationIdx != ENHORSE_ANIM_GALLOP) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_GALLOP;
    }

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        animSpeed = this->actor.speed * 0.5f;
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        animSpeed = this->actor.speed * 0.25f;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        animSpeed = this->actor.speed * 0.2f;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else {
        animSpeed = 1.0f;
    }

    if (animChanged == true) {
        Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx],
                         anim_speed[this->animationIdx] * animSpeed * 1.5f, 0,
                         Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3);
    } else {
        Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx],
                         anim_speed[this->animationIdx] * animSpeed * 1.5f, 0,
                         Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, 0);
    }
}

void En_Horse_Actor_mode_race_run(EnHorse* this, PlayState* play) {
    f32 playSpeed;

    if (this->animationIdx == ENHORSE_ANIM_IDLE || this->animationIdx == ENHORSE_ANIM_WHINNEY) {
        En_Horse_Actor_setWaitSound(this, play);
    } else if (this->animationIdx == ENHORSE_ANIM_WALK) {
        En_Horse_setWalkSound(this);
    }

    En_HL_CourseRun(this, play, &spot20_race_info);
    if (!this->inRace) {
        this->actor.speed = 0.0f;
        this->rider->speed = 0.0f;
        if (this->animationIdx != ENHORSE_ANIM_IDLE) {
            En_Horse_Actor_mode_race_run_init(this);
        }
    }

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        playSpeed = this->actor.speed * 0.5f;
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        playSpeed = this->actor.speed * 0.25f;
    } else if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        playSpeed = this->actor.speed * 0.2f;
    } else {
        playSpeed = 1.0f;
    }
    this->skin.skelAnime.playSpeed = playSpeed;
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime) ||
        (this->animationIdx == ENHORSE_ANIM_IDLE && this->actor.speed != 0.0f)) {
        En_Horse_Actor_mode_race_run_init(this);
    }

    if (this->stateFlags & ENHORSE_INGO_WON) {
        ((EnIn*)this->rider)->animationIdx = 7;
        ((EnIn*)this->rider)->unk_1E0 = 0;
        return;
    }

    Horse_Anim2In_anim_conv(this->animationIdx, this->skin.skelAnime.curFrame, this->ingoRaceFlags & 1,
                             &((EnIn*)this->rider)->animationIdx, &((EnIn*)this->rider)->unk_1E0);
}

void Hl_Td_Run_init(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->animationIdx = ENHORSE_ANIM_GALLOP;
    this->cutsceneAction = 1;
    Skeleton_Info2_init_standard_speedset_stop(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx],
                               this->actor.speed * 0.3f);
}

void Hl_Td_Jump200_no_set_frame_da(EnHorse* this, PlayState* play);

void Hl_Td_Run(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    Vec3f endPos;
    f32 speedXZ = 8.0f;

    endPos.x = cue->endPos.x;
    endPos.y = cue->endPos.y;
    endPos.z = cue->endPos.z;

    if (Math3DLength(&endPos, &this->actor.world.pos) > speedXZ) {
        En_HL_trace_pos(this, play, &endPos, 400);
        this->actor.speed = speedXZ;
        this->skin.skelAnime.playSpeed = speedXZ * 0.3f;
    } else {
        this->actor.world.pos = endPos;
        this->actor.speed = 0.0f;
    }

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        En_Horse_setFastRunSound(this);
        z_vibctl2_vib_setQ(0.0f, 120, 8, 255);
        Skeleton_Info2_init_standard_speedset_stop(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx],
                                   this->actor.speed * 0.3f);
    }
}

void Hl_Td_Jump200_da(EnHorse* this, PlayState* play) {
    this->skin.skelAnime.curFrame = 0.0f;
    Hl_Td_Jump200_no_set_frame_da(this, play);
}

void Hl_Td_Jump200_no_set_frame_da(EnHorse* this, PlayState* play) {
    f32 curFrame;
    f32 y;
    Vec3s* jointTable;

    this->animationIdx = ENHORSE_ANIM_HIGH_JUMP;
    curFrame = this->skin.skelAnime.curFrame;
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.5f, curFrame,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
    this->postDrawFunc = NULL;
    this->jumpStartY = this->actor.world.pos.y;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0;

    jointTable = this->skin.skelAnime.jointTable;
    y = jointTable->y;
    this->riderPos.y -= y * 0.01f;

    this->stateFlags |= ENHORSE_CALC_RIDER_POS;
    Nai_FxFlagEntry(NA_SE_EV_HORSE_JUMP, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
    z_vibctl2_vib_setQ(0.0f, 170, 10, 10);
}

void Hl_Td_Jump_init(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    Hl_Td_Jump200_da(this, play);
    this->cutsceneAction = 2;
    this->cutsceneFlags &= ~1;
}

void Hl_Td_Jump(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    f32 temp_f2;
    Vec3s* jointTable;
    f32 y;

    if (this->cutsceneFlags & 1) {
        Hl_Td_Run(this, play, cue);
        return;
    }
    temp_f2 = this->skin.skelAnime.curFrame;
    this->stateFlags |= ENHORSE_JUMPING;
    this->actor.speed = 13.0f;
    if (temp_f2 > 19.0f) {
        this->actor.gravity = -3.5f;
        if (this->actor.velocity.y == 0.0f) {
            this->actor.velocity.y = -10.5f;
        }
        if (this->actor.world.pos.y < (this->actor.floorHeight + 90.0f)) {
            this->skin.skelAnime.playSpeed = 1.5f;
        } else {
            this->skin.skelAnime.playSpeed = 0.0f;
        }
    } else {
        Vec3s* jointTable;
        f32 y;

        jointTable = this->skin.skelAnime.jointTable;
        y = jointTable->y;
        this->actor.world.pos.y = this->jumpStartY + y * 0.01f;
    }
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime) ||
        (temp_f2 > 19.0f && this->actor.world.pos.y < (this->actor.floorHeight - this->actor.velocity.y) + 80.0f)) {
        this->cutsceneFlags |= 1;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_LAND, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        z_vibctl2_vib_setQ(0.0f, 255, 10, 80);
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->actor.gravity = -3.5f;
        this->actor.velocity.y = 0;
        this->actor.world.pos.y = this->actor.floorHeight;
        Effect_SS_Dust_li_spread20(play, 25.0f, &this->actor.world.pos);
        this->animationIdx = ENHORSE_ANIM_GALLOP;
        Skeleton_Info2_init_standard_speedset_stop(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx],
                                   anim_speed[6]);
        jointTable = this->skin.skelAnime.jointTable;
        y = jointTable->y;
        this->riderPos.y += y * 0.01f;
        this->postDrawFunc = NULL;
    }
}

void Hl_Td_Wait_init(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->animationIdx = ENHORSE_ANIM_REARING;
    this->cutsceneAction = 3;
    this->cutsceneFlags &= ~4;
    this->stateFlags &= ~ENHORSE_LAND2_SOUND;
    this->unk_21C = this->unk_228;
    if (this->stateFlags & ENHORSE_DRAW) {
        Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void Hl_Td_Wait(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->actor.speed = 0.0f;
    if (this->curFrame > 25.0f) {
        if (!(this->stateFlags & ENHORSE_LAND2_SOUND)) {
            this->stateFlags |= ENHORSE_LAND2_SOUND;
            Nai_FxFlagEntry(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        this->animationIdx = ENHORSE_ANIM_IDLE;
        if (!(this->cutsceneFlags & 4)) {
            this->cutsceneFlags |= 4;
            Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                             Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                             Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), 0, 0.0f);
        }
    }
}

void Hl_Td_Change_Pos_Run_init(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->actor.world.pos.x = cue->startPos.x;
    this->actor.world.pos.y = cue->startPos.y;
    this->actor.world.pos.z = cue->startPos.z;

    this->actor.prevPos = this->actor.world.pos;

    this->actor.world.rot.y = cue->rot.y;
    this->actor.shape.rot = this->actor.world.rot;

    this->animationIdx = ENHORSE_ANIM_GALLOP;
    this->cutsceneAction = 4;
    Skeleton_Info2_init_standard_speedset_stop(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx],
                               this->actor.speed * 0.3f);
}

void Hl_Td_Change_Pos_Run(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    Vec3f endPos;
    f32 speedXZ = 8.0f;

    endPos.x = cue->endPos.x;
    endPos.y = cue->endPos.y;
    endPos.z = cue->endPos.z;

    if (Math3DLength(&endPos, &this->actor.world.pos) > speedXZ) {
        En_HL_trace_pos(this, play, &endPos, 400);
        this->actor.speed = speedXZ;
        this->skin.skelAnime.playSpeed = speedXZ * 0.3f;
    } else {
        this->actor.world.pos = endPos;
        this->actor.speed = 0.0f;
    }

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        En_Horse_setFastRunSound(this);
        z_vibctl2_vib_setQ(0.0f, 120, 8, 255);
        Skeleton_Info2_init_standard_speedset_stop(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx],
                                   this->actor.speed * 0.3f);
    }
}

void Hl_Td_Change_Pos_Wait_init(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->actor.world.pos.x = cue->startPos.x;
    this->actor.world.pos.y = cue->startPos.y;
    this->actor.world.pos.z = cue->startPos.z;

    this->actor.prevPos = this->actor.world.pos;

    this->actor.world.rot.y = cue->rot.y;
    this->actor.shape.rot = this->actor.world.rot;

    this->animationIdx = ENHORSE_ANIM_REARING;
    this->cutsceneAction = 5;
    this->cutsceneFlags &= ~4;
    this->stateFlags &= ~ENHORSE_LAND2_SOUND;
    this->unk_21C = this->unk_228;

    if (this->stateFlags & ENHORSE_DRAW) {
        Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }

    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
}

void Hl_Td_Change_Pos_Wait(EnHorse* this, PlayState* play, CsCmdActorCue* cue) {
    this->actor.speed = 0.0f;
    if (this->curFrame > 25.0f) {
        if (!(this->stateFlags & ENHORSE_LAND2_SOUND)) {
            this->stateFlags |= ENHORSE_LAND2_SOUND;
            Nai_FxFlagEntry(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        this->animationIdx = ENHORSE_ANIM_IDLE;
        if (!(this->cutsceneFlags & 4)) {
            this->cutsceneFlags |= 4;
            Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                             Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                             Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), 0, 0.0f);
        }
    }
}

void En_Horse_Actor_TD_move_init(EnHorse* this, PlayState* play) {
    this->playerControlled = false;
    this->action = ENHORSE_ACT_CS_UPDATE;
    this->cutsceneAction = 0;
    this->actor.speed = 0.0f;
}

s32 En_Horse_Actor_TD_CnvP2HIndex(s32 cueId) {
    s32 numActions = ARRAY_COUNT(P2hCnvTbl); // prevents unrolling
    s32 i;

    for (i = 0; i < numActions; i++) {
        if (cueId == P2hCnvTbl[i].cueId) {
            return P2hCnvTbl[i].csFuncIdx;
        }
        if (cueId < P2hCnvTbl[i].cueId) {
            return 0;
        }
    }
    return 0;
}

void En_Horse_Actor_TD_move(EnHorse* this, PlayState* play) {
    s32 csFunctionIdx;
    CsCmdActorCue* playerCue = play->csCtx.playerCue;

    if (play->csCtx.state == 3) {
        this->playerControlled = 1;
        this->actor.params = HORSE_PTYPE_10;
        this->action = ENHORSE_ACT_IDLE;
        En_Horse_Actor_mode_wait_lock_init(this);
        return;
    }
    if (playerCue != NULL) {
        csFunctionIdx = En_Horse_Actor_TD_CnvP2HIndex(playerCue->id);

        if (csFunctionIdx != 0) {
            if (this->cutsceneAction != csFunctionIdx) {
                if (this->cutsceneAction == 0) {
                    this->actor.world.pos.x = playerCue->startPos.x;
                    this->actor.world.pos.y = playerCue->startPos.y;
                    this->actor.world.pos.z = playerCue->startPos.z;

                    this->actor.world.rot.y = playerCue->rot.y;
                    this->actor.shape.rot = this->actor.world.rot;

                    this->actor.prevPos = this->actor.world.pos;
                }
                this->cutsceneAction = csFunctionIdx;
                td_init_function[this->cutsceneAction](this, play, playerCue);
            }
            td_function[this->cutsceneAction](this, play, playerCue);
        }
    }
}

s32 En_HL_Spot12CourseRun(EnHorse* this, PlayState* play, RaceInfo* raceInfo) {
    Vec3f pos;
    f32 px;
    f32 pz;
    f32 d;

    CourseData2xyz_t(raceInfo->waypoints, this->curRaceWaypoint, &pos);
    Math3DPlanePosAngleY(&pos, raceInfo->waypoints[this->curRaceWaypoint].angle, &px, &pz, &d);

    if (this->curRaceWaypoint >= raceInfo->numWaypoints - 1 &&
        Math3DLength(&pos, &this->actor.world.pos) < DREG(8)) {
        this->hbaFlags |= 2;
    }

    if (((this->actor.world.pos.x * px) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        this->curRaceWaypoint++;
        if (this->curRaceWaypoint >= raceInfo->numWaypoints) {
            this->hbaFlags |= 1;
            return 1;
        }
    }

    if (!(this->hbaFlags & 1)) {
        En_HL_trace_pos(this, play, &pos, 800);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (this->actor.speed < raceInfo->waypoints[this->curRaceWaypoint].speedXZ && !(this->hbaFlags & 1)) {
        this->actor.speed += 0.4f;
    } else {
        this->actor.speed -= 0.4f;
        if (this->actor.speed < 0.0f) {
            this->actor.speed = 0.0f;
        }
    }
    return 0;
}

void En_Horse_Actor_mode_spot12_yabusame_init(EnHorse* this);

void En_Horse_Actor_mode_spot12_yabusame_init_start(EnHorse* this) {
    this->hbaStarted = 0;
    this->soundTimer = 0;
    this->curRaceWaypoint = 0;
    this->hbaTimer = 0;
    this->actor.speed = 0.0f;
    En_Horse_Actor_mode_spot12_yabusame_init(this);
}

void En_Horse_Actor_mode_spot12_yabusame_init(EnHorse* this) {
    s32 animChanged = 0;
    f32 animSpeed;

    this->action = ENHORSE_ACT_HBA;
    if (this->actor.speed == 0.0f) {
        if (this->animationIdx != ENHORSE_ANIM_IDLE) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_IDLE;
    } else if (this->actor.speed <= 3.0f) {
        if (this->animationIdx != ENHORSE_ANIM_WALK) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_WALK;
    } else if (this->actor.speed <= 6.0f) {
        if (this->animationIdx != ENHORSE_ANIM_TROT) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_TROT;
    } else {
        if (this->animationIdx != ENHORSE_ANIM_GALLOP) {
            animChanged = true;
        }
        this->animationIdx = ENHORSE_ANIM_GALLOP;
    }

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        animSpeed = this->actor.speed * 0.5f;
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        animSpeed = this->actor.speed * 0.25f;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        z_vibctl2_vib_setQ(0.0f, 60, 8, 255);
    } else if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        animSpeed = this->actor.speed * 0.2f;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        z_vibctl2_vib_setQ(0.0f, 120, 8, 255);
    } else {
        animSpeed = 1.0f;
    }

    if (animChanged == true) {
        Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx],
                         anim_speed[this->animationIdx] * animSpeed * 1.5f, 0,
                         Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE,
                         -3.0f);
    } else {
        Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx],
                         anim_speed[this->animationIdx] * animSpeed * 1.5f, 0,
                         Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, 0);
    }
}

void En_Horse_Actor_mode_spot12_yabusame(EnHorse* this, PlayState* play) {
    f32 playSpeed;
    s32 isFanfarePlaying;

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        En_Horse_setWalkSound(this);
    }
    if (play->interfaceCtx.hbaAmmo == 0) {
        this->hbaTimer++;
    }

    isFanfarePlaying = Na_CheckPlayingBgm(NA_BGM_HORSE_GOAL);
    En_HL_Spot12CourseRun(this, play, &spot12_yabu_info);
    if ((this->hbaFlags & 1) || (this->hbaTimer >= 46)) {
        if ((isFanfarePlaying != true) && (z_common_data.minigameState != 3)) {
            z_common_data.save.cutsceneIndex = 0;
            play->nextEntranceIndex = ENTR_GERUDOS_FORTRESS_16;
            play->transitionTrigger = TRANS_TRIGGER_START;
            play->transitionType = TRANS_TYPE_CIRCLE(TCA_NORMAL, TCC_BLACK, TCS_FAST);
        }
    }

    if (play->interfaceCtx.hbaAmmo != 0) {
        if (!(this->hbaFlags & 2)) {
            if (GET_INFTABLE(INFTABLE_190)) {
                if ((s32)z_common_data.minigameScore >= 1500) {
                    this->hbaFlags |= 4;
                }
            } else {
                if ((s32)z_common_data.minigameScore >= 1000) {
                    this->hbaFlags |= 4;
                }
            }
        }
    }

    if ((play->interfaceCtx.hbaAmmo == 0) || (this->hbaFlags & 2)) {
        if (this->hbaFlags & 4) {
            this->hbaFlags &= ~4;
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_HORSE_GOAL);
        }
    }

    if (!this->hbaStarted) {
        this->actor.speed = 0.0f;
        if (this->animationIdx != ENHORSE_ANIM_IDLE) {
            En_Horse_Actor_mode_spot12_yabusame_init(this);
        }
    }

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        playSpeed = this->actor.speed * 0.5f;
    } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
        playSpeed = this->actor.speed * 0.25f;
    } else if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
        playSpeed = this->actor.speed * 0.2f;
    } else {
        playSpeed = 1.0f;
    }

    this->skin.skelAnime.playSpeed = playSpeed;
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime) ||
        (this->animationIdx == ENHORSE_ANIM_IDLE && this->actor.speed != 0.0f)) {
        En_Horse_Actor_mode_spot12_yabusame_init(this);
    }
}

void En_Horse_Actor_mode_spot20_start_run_init(EnHorse* this) {
    this->action = ENHORSE_ACT_FLEE_PLAYER;
    this->stateFlags |= ENHORSE_UNRIDEABLE;
    this->actor.speed = 0.0f;
}

void En_Horse_Actor_mode_spot20_start_run(EnHorse* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 distToHome;
    f32 playerDistToHome;
    f32 distToPlayer;
    s32 nextAnim = this->animationIdx;
    s32 animFinished;
    s16 yaw;

    if (R_EPONAS_SONG_PLAYED || this->type == HORSE_HNI) {
        En_Horse_Actor_mode_base_wait_init(this);
        Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }

    distToHome = Math3DLength(&this->actor.home.pos, &this->actor.world.pos);
    playerDistToHome = Math3DLength(&player->actor.world.pos, &this->actor.home.pos);
    distToPlayer = Math3DLength(&player->actor.world.pos, &this->actor.world.pos);

    // Run home
    if (playerDistToHome > 300.0f) {
        if (distToHome > 150.0f) {
            this->actor.speed += 0.4f;
            if (this->actor.speed > 8.0f) {
                this->actor.speed = 8.0f;
            }
        } else {
            this->actor.speed -= 0.47f;
            if (this->actor.speed < 0.0f) {
                this->actor.speed = 0.0f;
            }
        }
    } else {
        // Run away from Link
        if (distToPlayer < 300.0f) {
            this->actor.speed += 0.4f;
            if (this->actor.speed > 8.0f) {
                this->actor.speed = 8.0f;
            }
        } else {
            this->actor.speed -= 0.47f;
            if (this->actor.speed < 0.0f) {
                this->actor.speed = 0.0f;
            }
        }
    }

    if (this->actor.speed >= 6.0f) { // hoof it
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.3f;
        nextAnim = ENHORSE_ANIM_GALLOP;
    } else if (this->actor.speed >= 3.0f) { // trot
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.375f;
        nextAnim = ENHORSE_ANIM_TROT;
    } else if (this->actor.speed > 0.1f) { // walk
        this->skin.skelAnime.playSpeed = this->actor.speed * 0.75f;
        nextAnim = ENHORSE_ANIM_WALK;
        En_Horse_setWalkSound(this);
    } else { // idle
        nextAnim = fqrand() > 0.5f ? 1 : 0;
        En_Horse_Actor_setWaitSound(this, play);
        this->skin.skelAnime.playSpeed = 1.0f;
    }

    // Turn away from Link, or towards home
    if (nextAnim == ENHORSE_ANIM_GALLOP || nextAnim == ENHORSE_ANIM_TROT || nextAnim == ENHORSE_ANIM_WALK) {
        if (playerDistToHome < 300.0f) {
            yaw = player->actor.shape.rot.y;
            yaw += (Actor_search_actor_angleY(&this->actor, &player->actor) > 0 ? 1 : -1) * 0x3FFF;
        } else {
            yaw = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos) - this->actor.world.rot.y;
        }

        if (yaw > 400) {
            this->actor.world.rot.y += 400;
        } else if (yaw < -400) {
            this->actor.world.rot.y -= 400;
        } else {
            this->actor.world.rot.y += yaw;
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    animFinished = Skeleton_Info2_anime_play(&this->skin.skelAnime);

    if (this->animationIdx == ENHORSE_ANIM_IDLE || this->animationIdx == ENHORSE_ANIM_WHINNEY) {
        if (nextAnim == ENHORSE_ANIM_GALLOP || nextAnim == ENHORSE_ANIM_TROT || nextAnim == ENHORSE_ANIM_WALK) {
            this->animationIdx = nextAnim;
            Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                             Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
            if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
                En_Horse_setFastRunSound(this);
            } else if (this->animationIdx == ENHORSE_ANIM_TROT) {
                En_Horse_setSlowRunSound(this);
            }
            return;
        }
    }

    if (animFinished) {
        if (nextAnim == ENHORSE_ANIM_GALLOP) {
            En_Horse_setFastRunSound(this);
        } else if (nextAnim == ENHORSE_ANIM_TROT) {
            En_Horse_setSlowRunSound(this);
        }

        if (this->animationIdx == ENHORSE_ANIM_IDLE || this->animationIdx == ENHORSE_ANIM_WHINNEY) {
            if (nextAnim != this->animationIdx) {
                this->animationIdx = nextAnim;
                Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                                 Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]),
                                 ANIMMODE_ONCE, -3.0f);
                return;
            } else {
                if (fqrand() > 0.5f) {
                    this->animationIdx = ENHORSE_ANIM_IDLE;
                    this->stateFlags &= ~ENHORSE_SANDDUST_SOUND;
                } else {
                    this->animationIdx = ENHORSE_ANIM_WHINNEY;
                    this->unk_21C = this->unk_228;
                    if (this->stateFlags & ENHORSE_DRAW) {
                        Nai_FxFlagEntry(NA_SE_EV_HORSE_GROAN, &this->unk_21C, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
                    }
                }
                Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                                 Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]),
                                 ANIMMODE_ONCE, -3.0f);
                return;
            }
        }

        if (nextAnim != this->animationIdx) {
            this->animationIdx = nextAnim;
            Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                             Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        } else {
            Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                             Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             0.0f);
        }
        return;
    }

    if (this->animationIdx == ENHORSE_ANIM_WALK) {
        if (nextAnim == ENHORSE_ANIM_IDLE || nextAnim == ENHORSE_ANIM_WHINNEY) {
            this->animationIdx = nextAnim;
            Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.0f, 0.0f,
                             Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE,
                             -3.0f);
        }
    }
}

void En_Horse_Actor_mode_demo_spot09_bridge_jump_da(EnHorse* this, PlayState* play) {
    f32 y;

    Effect_SS_Dust_li_spread20(play, 25.0f, &this->actor.world.pos);
    this->action = ENHORSE_ACT_BRIDGE_JUMP;
    this->stateFlags |= ENHORSE_JUMPING;
    this->animationIdx = ENHORSE_ANIM_HIGH_JUMP;
    y = this->skin.skelAnime.jointTable->y;
    y *= 0.01f;
    this->bridgeJumpStart = this->actor.world.pos;
    this->bridgeJumpStart.y += y;
    this->bridgeJumpYVel =
        (((spot09_bridge_jump_area[this->bridgeJumpIdx].pos.y + 48.7f) - this->bridgeJumpStart.y) - -360.0f) / 30.0f;
    this->riderPos.y -= y;
    this->stateFlags |= ENHORSE_CALC_RIDER_POS;
    this->bridgeJumpRelAngle = this->actor.world.rot.y - spot09_bridge_jump_area[this->bridgeJumpIdx].angle;
    this->bridgeJumpTimer = 0;
    this->actor.gravity = 0.0f;
    this->actor.speed = 0;
    Skeleton_Info2_init(&this->skin.skelAnime, anim_tbl_tbl[this->type][this->animationIdx], 1.5f, 0.0f,
                     Si2_anime_end_frame(anim_tbl_tbl[this->type][this->animationIdx]), ANIMMODE_ONCE, -3.0f);
    this->unk_21C = this->unk_228;
    if (this->stateFlags & ENHORSE_DRAW) {
        Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
    Nai_FxFlagEntry(NA_SE_EV_HORSE_JUMP, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
    z_vibctl2_vib_setQ(0.0f, 170, 10, 10);
    this->postDrawFunc = NULL;
}

void En_Horse_Actor_mode_demo_spot09_bridge_jump_init(EnHorse* this, PlayState* play) {
    this->postDrawFunc = En_Horse_Actor_mode_demo_spot09_bridge_jump_da;
    if (this->bridgeJumpIdx == 0) {
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gGerudoValleyBridgeJumpFieldFortressCs);
        z_common_data.cutsceneTrigger = 1;
    } else {
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gGerudoValleyBridgeJumpFortressToFieldCs);
        z_common_data.cutsceneTrigger = 1;
    }
}

void En_Horse_Actor_mode_demo_spot09_bridge_jump_ChakutiMade(EnHorse* this, PlayState* play) {
    f32 interp;
    f32 timeSq;

    interp = this->bridgeJumpTimer / 30.0f;
    timeSq = (this->bridgeJumpTimer * this->bridgeJumpTimer);

    this->actor.world.pos.x =
        ((spot09_bridge_jump_area[this->bridgeJumpIdx].pos.x - this->bridgeJumpStart.x) * interp) + this->bridgeJumpStart.x;
    this->actor.world.pos.z =
        ((spot09_bridge_jump_area[this->bridgeJumpIdx].pos.z - this->bridgeJumpStart.z) * interp) + this->bridgeJumpStart.z;

    this->actor.world.pos.y =
        (this->bridgeJumpStart.y + (this->bridgeJumpYVel * this->bridgeJumpTimer) + (-0.4f * timeSq));

    this->actor.world.rot.y = this->actor.shape.rot.y =
        (spot09_bridge_jump_area[this->bridgeJumpIdx].angle + ((1.0f - interp) * this->bridgeJumpRelAngle));
    this->skin.skelAnime.curFrame = 23.0f * interp;
    Skeleton_Info2_anime_play(&this->skin.skelAnime);
    if (this->bridgeJumpTimer < 30) {
        this->stateFlags |= ENHORSE_FLAG_24;
    }
}

void En_Horse_Actor_mode_demo_spot09_bridge_jump_run(EnHorse* this, PlayState* play) {
    this->actor.speed = 8.0f;
    this->skin.skelAnime.playSpeed = 1.5f;
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        this->stateFlags &= ~ENHORSE_JUMPING;
        this->actor.gravity = -3.5f;
        this->actor.world.pos.y = spot09_bridge_jump_area[this->bridgeJumpIdx].pos.y;
        Effect_SS_Dust_li_spread20(play, 25.0f, &this->actor.world.pos);
        En_Horse_Actor_move_control_fast_run_after_jump_init(this, play);
        Nai_FxFlagEntry(NA_SE_EV_HORSE_LAND, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        z_vibctl2_vib_setQ(0.0f, 255, 10, 80);
    }
}

void En_Horse_Actor_mode_demo_sport09_brideg_jump(EnHorse* this, PlayState* play) {
    this->bridgeJumpTimer++;
    if (this->bridgeJumpTimer < 30) {
        En_Horse_Actor_mode_demo_spot09_bridge_jump_ChakutiMade(this, play);
        return;
    }
    En_Horse_Actor_mode_demo_spot09_bridge_jump_run(this, play);
}

void foot_under(Vec3f* src, s16 yaw, f32 dist, f32 height, Vec3f* dst) {
    dst->x = src->x + sin_s(yaw) * dist;
    dst->y = src->y + height;
    dst->z = src->z + cos_s(yaw) * dist;
}

s32 En_Horse_groundCheck(EnHorse* this, PlayState* play, Vec3f* pos, CollisionPoly** floorPoly, f32* floorHeight) {
    s32 bgId;
    f32 waterY;
    WaterBox* waterBox;

    *floorPoly = NULL;
    *floorHeight = T_BGCheck_ObjGroundCheck_ai(&play->colCtx, floorPoly, &bgId, pos);

    if (*floorHeight == BGCHECK_Y_MIN) {
        return 1; // No floor
    }

    if (T_BGCheck_WaterSurfaceCheck2(play, &play->colCtx, pos->x, pos->z, &waterY, &waterBox) == 1 &&
        *floorHeight < waterY) {
        return 2; // Water
    }

    if (COLPOLY_GET_NORMAL((*floorPoly)->normal.y) < 0.81915206f || // cos(35 degrees)
        T_BGCheck_getHorseKeepOutBit_ai(&play->colCtx, *floorPoly, bgId) ||
        T_BGCheck_getAttributeCode_ai(&play->colCtx, *floorPoly, bgId) == FLOOR_TYPE_7) {
        return 3; // Horse blocked surface
    }
    return 0;
}

/**
 * obstacleType:
 *  1: Water in front
 *  2: Water behind?
 *  3: ?
 *  4: Obstructed in front
 *  5: Obstructed behind
 */
void En_Horse_KeepOutProc(EnHorse* this, PlayState* play, s32 obstacleType, s32 galloping) {
    if (this->action == ENHORSE_ACT_CS_UPDATE || Spot09BridgeJumpRunAreaCheck(this, play)) {
        return;
    }

    this->actor.world.pos = this->lastPos;
    this->actor.shape.rot.y = this->lastYaw;
    this->actor.world.rot.y = this->lastYaw;
    this->stateFlags |= ENHORSE_OBSTACLE;

    if (!this->playerControlled) {
        if (this->animationIdx != ENHORSE_ANIM_REARING) {
            return;
        }
    } else if (this->action != ENHORSE_ACT_MOUNTED_REARING) {
        if (this->stateFlags & ENHORSE_JUMPING) {
            this->stateFlags &= ~ENHORSE_JUMPING;
            this->actor.gravity = -3.5f;
            this->actor.world.pos.y = this->actor.floorHeight;
        }
        if (obstacleType == 1 || obstacleType == 4) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
        } else if (obstacleType == 2 || obstacleType == 5) {
            this->stateFlags |= ENHORSE_FORCE_WALKING;
        }
        if (galloping == true) {
            En_Horse_Actor_mode_control_standing_init(this);
        }
    }
}

void En_Horse_xrot_calc(EnHorse* this, PlayState* play) {
    s32 status;
    CollisionPoly* frontFloor;
    CollisionPoly* backFloor;
    s16 floorSlope;
    Vec3f frontPos;
    Vec3f backPos;
    Vec3f pos;
    f32 nx;
    f32 ny;
    f32 nz;
    s32 galloping = this->actor.speed > 8;
    f32 dist;
    f32 waterHeight;
    WaterBox* waterBox;
    s32 pad;

    if (T_BGCheck_WaterSurfaceCheck2(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterHeight,
                                &waterBox) == 1 &&
        this->actor.floorHeight < waterHeight) {
        En_Horse_KeepOutProc(this, play, 1, galloping);
        return;
    }

    foot_under(&this->actor.world.pos, this->actor.shape.rot.y, 30.0f, 60.0f, &frontPos);
    status = En_Horse_groundCheck(this, play, &frontPos, &frontFloor, &this->yFront);
    if (status == 1) {
        this->actor.shape.rot.x = 0;
        En_Horse_KeepOutProc(this, play, 4, galloping);
        return;
    }
    if (status == 2) {
        En_Horse_KeepOutProc(this, play, 4, galloping);
        return;
    }
    if (status == 3) {
        En_Horse_KeepOutProc(this, play, 4, galloping);
        return;
    }

    foot_under(&this->actor.world.pos, this->actor.shape.rot.y, -30.0f, 60.0f, &backPos);
    status = En_Horse_groundCheck(this, play, &backPos, &backFloor, &this->yBack);
    if (status == 1) {
        this->actor.shape.rot.x = 0;
        En_Horse_KeepOutProc(this, play, 5, galloping);
        return;
    }
    if (status == 2) {
        En_Horse_KeepOutProc(this, play, 5, galloping);
        return;
    }
    if (status == 3) {
        En_Horse_KeepOutProc(this, play, 5, galloping);
        return;
    }

    floorSlope = RAD_TO_BINANG(fatan2(this->yBack - this->yFront, 60.0f));
    if (this->actor.floorPoly != NULL) {
        nx = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.x);
        ny = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.y);
        nz = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.z);
        pos = frontPos;
        pos.y = this->yFront;
        dist = Math3DSignedLengthPlaneAndPos(nx, ny, nz, this->actor.floorPoly->dist, &pos);
        if ((frontFloor != this->actor.floorPoly) && (this->actor.speed >= 0.0f)) {
            if ((!(this->stateFlags & ENHORSE_JUMPING) && dist < -40.0f) ||
                (this->stateFlags & ENHORSE_JUMPING && dist < -200.0f)) {
                En_Horse_KeepOutProc(this, play, 4, galloping);
                return;
            }
        }

        pos = backPos;
        pos.y = this->yBack;
        dist = Math3DSignedLengthPlaneAndPos(nx, ny, nz, this->actor.floorPoly->dist, &pos);
        if (((backFloor != this->actor.floorPoly) && (this->actor.speed <= 0.0f) &&
             !(this->stateFlags & ENHORSE_JUMPING) && (dist < -40.0f)) ||
            (this->stateFlags & ENHORSE_JUMPING && dist < -200.0f)) {
            En_Horse_KeepOutProc(this, play, 5, galloping);
            return;
        }

        if (ny < 0.81915206f || // cos(35 degrees)
            T_BGCheck_getHorseKeepOutBit_ai(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) ||
            T_BGCheck_getAttributeCode_ai(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) == FLOOR_TYPE_7) {
            if (this->actor.speed >= 0.0f) {
                En_Horse_KeepOutProc(this, play, 4, galloping);
            } else {
                En_Horse_KeepOutProc(this, play, 5, galloping);
            }
            return;
        }

        if (this->stateFlags & ENHORSE_JUMPING) {
            this->actor.shape.rot.x = 0;
            return;
        }

        if (this->actor.floorHeight + 4.0f < this->actor.world.pos.y) {
            this->actor.shape.rot.x = 0;
            return;
        }

        if (fabsf(floorSlope) > 8191.0f) {
            return;
        }

        this->actor.shape.rot.x = floorSlope;
        this->actor.shape.yOffset =
            (this->yFront + (((this->yBack - this->yFront) * 20.0f) / 45.0f)) - this->actor.floorHeight;
    }
}

s32 En_Horse_player_checkRide(EnHorse* this, PlayState* play);

void En_Horse_before_move(EnHorse* this, PlayState* play) {
    s32 pad[2];
    s32 mountSide;
    Player* player = GET_PLAYER(play);

    mountSide = En_Horse_player_checkRide(this, play);
    if (mountSide != 0 && !(this->stateFlags & ENHORSE_UNRIDEABLE) && player->rideActor == NULL) {
        Actor_ride_request(play, &this->actor, mountSide);
    }

    if (!this->playerControlled && Actor_ride_check(play, &this->actor) == true) {
        this->noInputTimer = 55;
        this->noInputTimerMax = 55;
        this->playerControlled = 1;
        En_Horse_Actor_mode_wait_lock_init(this);
    } else if (this->playerControlled == true && Actor_ride_end_check(play, &this->actor) == true) {
        this->noInputTimer = 35;
        this->noInputTimerMax = 35;
        this->stateFlags &= ~ENHORSE_UNRIDEABLE;
        this->playerControlled = 0;
        En_Horse_Actor_mode_wait_lock_init(this);
    }
}

void stick_data_convert(Vec2f* curStick, f32* stickMag, s16* angle) {
    f32 dist;
    f32 y;
    f32 x;

    x = curStick->x;
    y = curStick->y;
    dist = sqrtf(SQ(x) + SQ(y));

    *stickMag = dist;
    if (dist > 60.0f) {
        *stickMag = 60.0f;
    } else {
        *stickMag = *stickMag;
    }

    *angle = RAD_TO_BINANG(fatan2(-curStick->x, curStick->y));
}

void En_Horse_get_pad_info(EnHorse* this, PlayState* play) {
    this->lastStick = this->curStick;
    this->curStick.x = play->state.input[0].rel.stick_x;
    this->curStick.y = play->state.input[0].rel.stick_y;
}

void En_Horse_polyPosCollection(EnHorse* this, PlayState* play, CollisionPoly* colPoly) {
    f32 dist;
    f32 nx;
    f32 ny;
    f32 nz;
    f32 offset;

    nx = COLPOLY_GET_NORMAL(colPoly->normal.x);
    ny = COLPOLY_GET_NORMAL(colPoly->normal.y);
    nz = COLPOLY_GET_NORMAL(colPoly->normal.z);
    if (!(cos_s(this->actor.world.rot.y - RAD_TO_BINANG(fatan2(colPoly->normal.x, colPoly->normal.z)) -
                    0x7FFF) < 0.7071f)) { // cos(45 degrees)
        dist = Math3DSignedLengthPlaneAndPos(nx, ny, nz, colPoly->dist, &this->actor.world.pos);
        offset = (1.0f / sqrtf(SQ(nx) + SQ(nz)));
        offset = (30.0f - dist) * offset;
        this->actor.world.pos.x += offset * nx;
        this->actor.world.pos.z += offset * nz;
    }
}

void En_Horse_lineCollection(EnHorse* this, PlayState* play) {
    f32 yOffset;
    Vec3f start;
    Vec3f end;
    Vec3f intersect;
    CollisionPoly* colPoly;
    s32 bgId;

    if (play->sceneId == SCENE_LON_LON_RANCH) {
        yOffset = 19.0f;
    } else {
        yOffset = 40.0f;
    }
    xyz_t_move(&start, &this->actor.world.pos);
    start.y += yOffset;

    xyz_t_move(&end, &start);
    end.x += 30.0f * sin_s(this->actor.world.rot.y);
    end.y += 30.0f * sin_s(-this->actor.shape.rot.x);
    end.z += 30.0f * cos_s(this->actor.world.rot.y);
    if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &start, &end, &intersect, &colPoly, 1, 0, 0, 1, &bgId) != 0) {
        En_Horse_polyPosCollection(this, play, colPoly);
    }
}

void En_Horse_BGcheck(EnHorse* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    Vec3f startPos;
    Vec3f endPos;
    Vec3f obstaclePos;
    f32 pad3;
    f32 intersectDist;
    CollisionPoly* wall = NULL;
    CollisionPoly* obstacleFloor = NULL;
    s32 bgId;
    f32 obstacleHeight;
    f32 behindObstacleHeight;
    f32 ny;
    s32 movingFast;
    s32 pad5;
    DynaPolyActor* dynaPoly;
    Vec3f intersect;
    Vec3f obstacleTop;

    Actor_BGcheck2(play, &this->actor, play->sceneId == SCENE_LON_LON_RANCH ? 19.0f : 40.0f, 35.0f, 100.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);

    if (Spot09BridgeJumpRunAreaCheck(this, play)) {
        return;
    }

    // void 0 trick required to match, but is surely not real. revisit at a later time
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
        cos_s(this->actor.wallYaw - ((void)0, this->actor.world).rot.y) < -0.3f) {
        if (this->actor.speed > 4.0f) {
            this->actor.speed -= 1.0f;
            Nai_FxFlagEntry(NA_SE_EV_HORSE_SANDDUST, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }

    if (this->stateFlags & ENHORSE_JUMPING || !this->playerControlled) {
        return;
    }

    if (this->actor.speed < 0.0f) {
        return;
    }

    // Braking or rearing from obstacle
    if (this->action == ENHORSE_ACT_STOPPING || this->action == ENHORSE_ACT_MOUNTED_REARING) {
        return;
    }

    if (this->actor.speed > 8.0f) {
        if (this->actor.speed < 12.8f) {
            intersectDist = 160.0f;
            movingFast = 0;
        } else {
            intersectDist = 230.0f;
            movingFast = 1;
        }
    } else {
        En_Horse_lineCollection(this, play);
        return;
    }

    startPos = this->actor.world.pos;
    startPos.y += 19.0f;
    endPos = startPos;
    endPos.x += (intersectDist * sin_s(this->actor.world.rot.y));
    endPos.y += (intersectDist * sin_s(-this->actor.shape.rot.x));
    endPos.z += (intersectDist * cos_s(this->actor.world.rot.y));
    intersect = endPos;
    wall = NULL;
    if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &startPos, &endPos, &intersect, &wall, 1, 0, 0, 1, &bgId) == 1) {
        intersectDist = sqrt(Math3DLengthSquare(&startPos, &intersect));
        this->stateFlags |= ENHORSE_OBSTACLE;
    }

    if (wall != NULL) {
        if (intersectDist < 30.0f) {
            En_Horse_polyPosCollection(this, play, wall);
        }
        if ((cos_s(this->actor.world.rot.y - RAD_TO_BINANG(fatan2(wall->normal.x, wall->normal.z)) - 0x7FFF) <
             0.5f) ||
            T_BGCheck_getHorseKeepOutBit_ai(&play->colCtx, wall, bgId) != 0) {
            return;
        }

        // too close to jump
        if ((!movingFast && intersectDist < 80.0f) || (movingFast == true && intersectDist < 150.0f)) {
            if (!movingFast) {
                this->stateFlags |= ENHORSE_FORCE_REVERSING;
            } else if (movingFast == true) {
                this->stateFlags |= ENHORSE_FORCE_REVERSING;
                En_Horse_Actor_mode_control_stop_init(this, play);
            }
            return;
        }

        dynaPoly = DynaPolyInfo_actor_index2pointer(&play->colCtx, bgId);
        if ((this->stateFlags & ENHORSE_FLAG_26) &&
            ((dynaPoly && dynaPoly->actor.id != ACTOR_BG_UMAJUMP) || dynaPoly == NULL)) {
            if (!movingFast) {
                this->stateFlags |= ENHORSE_FORCE_REVERSING;
            } else if (movingFast == true) {
                this->stateFlags |= ENHORSE_FORCE_REVERSING;
                En_Horse_Actor_mode_control_stop_init(this, play);
            }
            return;
        }
    }

    // Get obstacle's height
    intersectDist += 5.0f;
    obstaclePos = startPos;
    obstaclePos.x += intersectDist * sin_s(this->actor.world.rot.y);
    obstaclePos.y = this->actor.world.pos.y + 120.0f;
    obstaclePos.z += intersectDist * cos_s(this->actor.world.rot.y);
    obstacleTop = obstaclePos;
    obstacleTop.y = T_BGCheck_ObjGroundCheck_ai(&play->colCtx, &obstacleFloor, &bgId, &obstaclePos);
    if (obstacleTop.y == BGCHECK_Y_MIN) {
        return;
    }
    obstacleHeight = obstacleTop.y - this->actor.world.pos.y;
    if (this->actor.floorPoly == NULL || obstacleFloor == NULL) {
        return;
    }

    if (Math3DSignedLengthPlaneAndPos(
            COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.x), COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.y),
            COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.z), this->actor.floorPoly->dist, &obstacleTop) < -40.0f &&
        Math3DSignedLengthPlaneAndPos(COLPOLY_GET_NORMAL(obstacleFloor->normal.x), COLPOLY_GET_NORMAL(obstacleFloor->normal.y),
                              COLPOLY_GET_NORMAL(obstacleFloor->normal.z), obstacleFloor->dist,
                              &this->actor.world.pos) > 40.0f) {
        if (movingFast == true && this->action != ENHORSE_ACT_STOPPING) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            En_Horse_Actor_mode_control_stop_init(this, play);
        }
        this->stateFlags |= ENHORSE_OBSTACLE;
        return;
    }

    ny = COLPOLY_GET_NORMAL(obstacleFloor->normal.y);
    if (ny < 0.81915206f || // cos(35 degrees)
        (T_BGCheck_getHorseKeepOutBit_ai(&play->colCtx, obstacleFloor, bgId) != 0) ||
        (T_BGCheck_getAttributeCode_ai(&play->colCtx, obstacleFloor, bgId) == FLOOR_TYPE_7)) {
        if (movingFast == true && this->action != ENHORSE_ACT_STOPPING) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            En_Horse_Actor_mode_control_stop_init(this, play);
        }
        return;
    }

    if (wall == NULL || obstacleTop.y < intersect.y || (this->stateFlags & ENHORSE_CANT_JUMP)) {
        return;
    }

    obstaclePos = startPos;
    obstaclePos.y = this->actor.world.pos.y + 120.0f;
    if (!movingFast) {
        obstaclePos.x += (276.0f * sin_s(this->actor.world.rot.y));
        obstaclePos.z += (276.0f * cos_s(this->actor.world.rot.y));
    } else {
        obstaclePos.x += (390.0f * sin_s(this->actor.world.rot.y));
        obstaclePos.z += (390.0f * cos_s(this->actor.world.rot.y));
    }

    obstacleTop = obstaclePos;
    obstacleTop.y = T_BGCheck_ObjGroundCheck_ai(&play->colCtx, &obstacleFloor, &bgId, &obstaclePos);
    if (obstacleTop.y == BGCHECK_Y_MIN) {
        return;
    }

    behindObstacleHeight = obstacleTop.y - this->actor.world.pos.y;

    if (obstacleFloor == NULL) {
        return;
    }

    ny = COLPOLY_GET_NORMAL(obstacleFloor->normal.y);
    if (ny < 0.81915206f || // cos(35 degrees)
        T_BGCheck_getHorseKeepOutBit_ai(&play->colCtx, obstacleFloor, bgId) ||
        T_BGCheck_getAttributeCode_ai(&play->colCtx, obstacleFloor, bgId) == FLOOR_TYPE_7) {
        if (movingFast == true && this->action != ENHORSE_ACT_STOPPING) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            En_Horse_Actor_mode_control_stop_init(this, play);
        }
    } else if (behindObstacleHeight < -DREG(4)) { // -70
        if (movingFast == true && this->action != ENHORSE_ACT_STOPPING) {
            this->stateFlags |= ENHORSE_FORCE_REVERSING;
            En_Horse_Actor_mode_control_stop_init(this, play);
        }
    } else if (!movingFast && obstacleHeight > 19.0f && obstacleHeight <= 40.0f) {
        En_Horse_Actor_mode_demo_jump100_init(this);
        this->postDrawFunc = En_Horse_Actor_mode_demo_jump100_da;
    } else if ((movingFast == true && this->actor.speed < 13.8f && obstacleHeight > 19.0f && obstacleHeight <= 72.0f) ||
               (this->actor.speed > 13.8f && obstacleHeight <= 112.0f)) {

        En_Horse_Actor_mode_demo_jump200_init(this);
        this->postDrawFunc = En_Horse_Actor_mode_demo_jump200_da;
    }
}

void En_Horse_WhipProc(EnHorse* thisx, PlayState* play2) {
    EnHorse* this = (EnHorse*)thisx;
    PlayState* play = play2;
    s32 pad;

    if (this->action == ENHORSE_ACT_MOUNTED_WALK || this->action == ENHORSE_ACT_MOUNTED_TROT ||
        this->action == ENHORSE_ACT_MOUNTED_GALLOP) {
        if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A) && (play->interfaceCtx.unk_1EE == 8)) {
            if (!(this->stateFlags & ENHORSE_BOOST) && !(this->stateFlags & ENHORSE_FLAG_8) &&
                !(this->stateFlags & ENHORSE_FLAG_9)) {
                if (this->numBoosts > 0) {
                    z_vibctl2_vib_setQ(0.0f, 180, 20, 100);
                    this->stateFlags |= ENHORSE_BOOST;
                    this->stateFlags |= ENHORSE_FIRST_BOOST_REGEN;
                    this->stateFlags |= ENHORSE_FLAG_8;
                    this->numBoosts--;
                    this->boostTimer = 0;
                    if (this->numBoosts == 0) {
                        this->boostRegenTime = 140;
                        return;
                    }
                    if (this->type == HORSE_EPONA) {
                        if (this->stateFlags & ENHORSE_FIRST_BOOST_REGEN) {
                            this->boostRegenTime = 60;
                            this->stateFlags &= ~ENHORSE_FIRST_BOOST_REGEN;
                        } else {
                            this->boostRegenTime = 8; // Never happens
                        }
                    } else {
                        this->boostRegenTime = 70;
                    }
                    return;
                }
                this->unk_21C = this->unk_228;
                if (this->stateFlags & ENHORSE_DRAW) {
                    if (fqrand() < 0.1f) {
                        Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
                    }
                }
            }
        }
    }
}

void En_Horse_WhipRecover(EnHorse* this, PlayState* play) {
    if (this->numBoosts < 6 && this->numBoosts > 0) {
        this->boostRegenTime--;
        this->boostTimer++;

        if (this->boostRegenTime <= 0) {
            this->numBoosts++;

            if (!EN_HORSE_CHECK_4(this)) {
                Nai_FxFlagEntry(NA_SE_SY_CARROT_RECOVER, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }

            if (this->numBoosts < 6) {
                this->boostRegenTime = 11;
            }
        }
    } else if (this->numBoosts == 0) {
        this->boostRegenTime--;
        this->boostTimer++;

        if (this->boostRegenTime <= 0) {
            this->boostRegenTime = 0;
            this->numBoosts = 6;

            if (!EN_HORSE_CHECK_4(this)) {
                Nai_FxFlagEntry(NA_SE_SY_CARROT_RECOVER, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
        }
    }

    if (this->boostTimer == 8 && fqrand() < 0.25f) {
        this->unk_21C = this->unk_228;
        if (this->stateFlags & ENHORSE_DRAW) {
            Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }
    play->interfaceCtx.numHorseBoosts = this->numBoosts;
}

void En_Horse_getPlayerPosRelation(EnHorse* this, PlayState* play) {
    EnHorse* pad;
    s16 angle;
    f32 s;
    f32 c;
    Player* player = GET_PLAYER(play);

    angle = Actor_search_actor_angleY(&this->actor, &player->actor) - this->actor.world.rot.y;
    s = sin_s(angle);
    c = cos_s(angle);
    if (s > 0.8660254f) { // sin(60 degrees)
        this->playerDir = PLAYER_DIR_SIDE_L;
    } else if (s < -0.8660254f) { // -sin(60 degrees)
        this->playerDir = PLAYER_DIR_SIDE_R;
    } else {
        if (c > 0.0f) {
            if (s > 0) {
                this->playerDir = PLAYER_DIR_FRONT_L;
            } else {
                this->playerDir = PLAYER_DIR_FRONT_R;
            }
        } else {
            if (s > 0) {
                this->playerDir = PLAYER_DIR_BACK_L;
            } else {
                this->playerDir = PLAYER_DIR_BACK_R;
            }
        }
    }
}

void En_HL_BankProc(EnHorse* this, PlayState* play) {
    f32 speed;
    f32 rollDiff;
    s32 targetRoll;
    s16 turnVel;

    speed = this->actor.speed / this->boostSpeed;
    turnVel = this->actor.shape.rot.y - this->lastYaw;
    targetRoll = -((s16)((1820.0f * speed) * (turnVel / 480.00003f)));
    rollDiff = targetRoll - this->actor.world.rot.z;

    if (fabsf(targetRoll) < 100.0f) {
        this->actor.world.rot.z = 0;
    } else if (fabsf(rollDiff) < 100.0f) {
        this->actor.world.rot.z = targetRoll;
    } else if (rollDiff > 0.0f) {
        this->actor.world.rot.z += 100;
    } else {
        this->actor.world.rot.z -= 100;
    }

    this->actor.shape.rot.z = this->actor.world.rot.z;
}

s32 En_Horse_SceneChange(EnHorse* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 conveyorDir;

    if ((this->actor.floorPoly == NULL) && (this != (EnHorse*)player->rideActor)) {
        return 0;
    }
    conveyorDir = T_BGCheck_getSlideAngle(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);
    conveyorDir = CONVEYOR_DIRECTION_TO_BINANG(conveyorDir) - this->actor.world.rot.y;
    if (conveyorDir > 800.0f) {
        this->actor.world.rot.y += 800.0f;
    } else if (conveyorDir < -800.0f) {
        this->actor.world.rot.y -= 800.0f;
    } else {
        this->actor.world.rot.y += conveyorDir;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;

    return 1;
}

s32 rnd_i(f32 range) {
    return fqrand() * range;
}

void En_Horse_move(Actor* thisx, PlayState* play2) {
    EnHorse* this = (EnHorse*)thisx;
    PlayState* play = play2;
    Vec3f dustAcc = { 0.0f, 0.0f, 0.0f };
    Vec3f dustVel = { 0.0f, 1.0f, 0.0f };
    Player* player = GET_PLAYER(play);

    this->lastYaw = thisx->shape.rot.y;
    En_Horse_get_pad_info(this, play);
    En_Horse_getPlayerPosRelation(this, play);

    if (!(this->stateFlags & ENHORSE_INACTIVE)) {
        En_Horse_before_move(this, play);
    }

    if (this->stateFlags & ENHORSE_FLAG_19) {
        if (this->stateFlags & ENHORSE_FLAG_20 && this->inRace == true) {
            this->stateFlags &= ~ENHORSE_FLAG_20;
            En_Horse_Actor_mode_control_standing_init(this);
        } else if (!(this->stateFlags & ENHORSE_FLAG_20) && this->stateFlags & ENHORSE_FLAG_21 &&
                   this->action != ENHORSE_ACT_MOUNTED_REARING && this->inRace == true) {
            this->stateFlags &= ~ENHORSE_FLAG_21;
            En_Horse_Actor_mode_control_standing_init(this);
        }
    }

    mode_function[this->action](this, play);
    this->stateFlags &= ~ENHORSE_OBSTACLE;
    this->curFrame = this->skin.skelAnime.curFrame;
    this->lastPos = thisx->world.pos;
    if (!(this->stateFlags & ENHORSE_INACTIVE)) {
        if (this->action == ENHORSE_ACT_MOUNTED_GALLOP || this->action == ENHORSE_ACT_MOUNTED_TROT ||
            this->action == ENHORSE_ACT_MOUNTED_WALK) {
            En_Horse_WhipProc(this, play);
        }
        if (this->playerControlled == true) {
            En_Horse_WhipRecover(this, play);
        }
        Actor_position_moveF(thisx);
        if (this->action == ENHORSE_ACT_INGO_RACE) {
            if (this->rider != NULL) {
                this->rider->world.pos.x = thisx->world.pos.x;
                this->rider->world.pos.y = thisx->world.pos.y + 10.0f;
                this->rider->world.pos.z = thisx->world.pos.z;
                this->rider->shape.rot.x = thisx->shape.rot.x;
                this->rider->shape.rot.y = thisx->shape.rot.y;
            }
        }
        if (this->jntSph.elements[0].base.ocElemFlags & OCELEM_HIT) {
            if (thisx->speed > 6.0f) {
                thisx->speed -= 1.0f;
            }
        }
        if (this->jntSph.base.acFlags & AC_HIT) {
            this->unk_21C = this->unk_228;
            if (this->stateFlags & ENHORSE_DRAW) {
                Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_21C, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
        }
        if (this->action != ENHORSE_ACT_INGO_RACE) {
            En_HL_BankProc(this, play);
        }
        CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->cyl1);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->cyl2);

        // Required to match
        this->cyl1.dim.pos.x = this->cyl1.dim.pos.x + (s16)(sin_s(thisx->shape.rot.y) * 11.0f);
        this->cyl1.dim.pos.z = this->cyl1.dim.pos.z + (s16)(cos_s(thisx->shape.rot.y) * 11.0f);
        this->cyl2.dim.pos.x = this->cyl2.dim.pos.x + (s16)(sin_s(thisx->shape.rot.y) * -18.0f);
        this->cyl2.dim.pos.z = this->cyl2.dim.pos.z + (s16)(cos_s(thisx->shape.rot.y) * -18.0f);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->cyl1.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->cyl1.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->cyl2.base);
        if ((player->stateFlags1 & PLAYER_STATE1_0) && player->rideActor != NULL) {
            if (play->sceneId != SCENE_LON_LON_RANCH ||
                (play->sceneId == SCENE_LON_LON_RANCH && (thisx->world.pos.z < -2400.0f))) {
                En_Horse_SceneChange(this, play);
            }
        }
        if (!(this->stateFlags & ENHORSE_FLAG_24)) {
            En_Horse_BGcheck(this, play);
            En_Horse_xrot_calc(this, play);
            if (thisx->world.pos.y < this->yFront && thisx->world.pos.y < this->yBack) {
                if (this->yBack < this->yFront) {
                    thisx->world.pos.y = this->yBack;
                } else {
                    thisx->world.pos.y = this->yFront;
                }
            }

        } else {
            this->stateFlags &= ~ENHORSE_FLAG_24;
        }

        if (play->sceneId == SCENE_GERUDO_VALLEY && !GET_EVENTCHKINF_CARPENTERS_ALL_RESCUED()) {
            Spot09BridgeJumpCheckSet(this, play);
        }

        thisx->focus.pos = thisx->world.pos;
        thisx->focus.pos.y += 70.0f;
        if ((fqrand() < 0.025f) && this->blinkTimer == 0) {
            this->blinkTimer++;
        } else if (this->blinkTimer > 0) {
            this->blinkTimer++;
            if (this->blinkTimer >= 4) {
                this->blinkTimer = 0;
            }
        }

        if (thisx->speed == 0.0f && !(this->stateFlags & ENHORSE_FLAG_19)) {
            thisx->colChkInfo.mass = MASS_IMMOVABLE;
        } else {
            thisx->colChkInfo.mass = MASS_HEAVY;
        }

        if (thisx->speed >= 5.0f) {
            this->cyl1.base.atFlags |= AT_ON;
        } else {
            this->cyl1.base.atFlags &= ~AT_ON;
        }

        if (z_common_data.save.entranceIndex != ENTR_LON_LON_RANCH_0 || z_common_data.sceneLayer != 9) {
            if (this->dustFlags & 1) {
                this->dustFlags &= ~1;
                Effect_SS_Dust_sc_co_li_ct(play, &this->frontRightHoof, &dustVel, &dustAcc, rnd_i(100) + 200,
                              rnd_i(10) + 30, rnd_i(20) + 30);
            } else if (this->dustFlags & 2) {
                this->dustFlags &= ~2;
                Effect_SS_Dust_sc_co_li_ct(play, &this->frontLeftHoof, &dustVel, &dustAcc, rnd_i(100) + 200,
                              rnd_i(10) + 30, rnd_i(20) + 30);
            } else if (this->dustFlags & 4) {
                this->dustFlags &= ~4;
                Effect_SS_Dust_sc_co_li_ct(play, &this->backRightHoof, &dustVel, &dustAcc, rnd_i(100) + 200,
                              rnd_i(10) + 30, rnd_i(20) + 30);
            } else if (this->dustFlags & 8) {
                this->dustFlags &= ~8;
                Effect_SS_Dust_sc_co_li_ct(play, &this->backLeftHoof, &dustVel, &dustAcc, rnd_i(100) + 200,
                              rnd_i(10) + 30, rnd_i(20) + 30);
            }
        }
        this->stateFlags &= ~ENHORSE_DRAW;
    }
}

s32 En_Horse_actor_checkSide(EnHorse* this, PlayState* play, Player* player) {
    if (this->playerDir == PLAYER_DIR_SIDE_L) {
        return -1;
    }
    if (this->playerDir == PLAYER_DIR_SIDE_R) {
        return 1;
    }
    return 0;
}

s32 En_Horse_actor_checkRide(EnHorse* this, PlayState* play, Player* player) {
    s32 mountSide;

    if (Actor_search_actor_distanceXZ(&this->actor, &player->actor) > 75.0f) {
        return 0;
    } else if (fabsf(this->actor.world.pos.y - player->actor.world.pos.y) > 30.0f) {
        return 0;
    } else if (cos_s(Actor_search_actor_angleY(&player->actor, &this->actor) - player->actor.world.rot.y) <
               0.17364818f) { // cos(80 degrees)
        return 0;
    } else {
        mountSide = En_Horse_actor_checkSide(this, play, player);
        if (mountSide == -1) {
            return -1;
        }
        if (mountSide == 1) {
            return 1;
        }
    }
    return 0;
}

s32 En_Horse_player_checkRide(EnHorse* this, PlayState* play) {
    if (this->action != ENHORSE_ACT_IDLE) {
        return 0;
    }
    if ((this->animationIdx != ENHORSE_ANIM_IDLE) && (this->animationIdx != ENHORSE_ANIM_WHINNEY)) {
        return 0;
    }
    return En_Horse_actor_checkRide(this, play, GET_PLAYER(play));
}

void position_plus_rnd(Vec3f* src, f32 dist, Vec3f* dst) {
    dst->x = (fqrand() * (dist * 2.0f) + src->x) - dist;
    dst->y = (fqrand() * (dist * 2.0f) + src->y) - dist;
    dst->z = (fqrand() * (dist * 2.0f) + src->z) - dist;
}

void En_Horse_calc2(Actor* thisx, PlayState* play, Skin* skin) {
    EnHorse* this = (EnHorse*)thisx;
    s32 pad;
    Vec3f sp94 = { 0.0f, 0.0f, 0.0f };
    Vec3f hoofOffset = { 5.0f, -4.0f, 5.0f };
    Vec3f riderOffset = { 600.0f, -1670.0f, 0.0f };
    Vec3f sp70;
    Vec3f sp64 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp58 = { 0.0f, -1.0f, 0.0f };

    f32 frame = this->skin.skelAnime.curFrame;
    Vec3f center;
    Vec3f newCenter;
    s32 i;
    Vec3f sp2C;
    f32 sp28;

    if (!(this->stateFlags & ENHORSE_CALC_RIDER_POS)) {
        Skin_MatrixPosition2_gfx(skin, 30, &riderOffset, &this->riderPos);
        this->riderPos.x -= this->actor.world.pos.x;
        this->riderPos.y -= this->actor.world.pos.y;
        this->riderPos.z -= this->actor.world.pos.z;
    } else {
        this->stateFlags &= ~ENHORSE_CALC_RIDER_POS;
    }

    Skin_MatrixPosition2_gfx(skin, 13, &sp94, &sp2C);
    Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &sp2C, &this->unk_228, &sp28);
    if ((this->animationIdx == ENHORSE_ANIM_IDLE && this->action != ENHORSE_ACT_FROZEN) &&
        ((frame > 40.0f && frame < 45.0f && this->type == HORSE_EPONA) ||
         (frame > 28.0f && frame < 33.0f && this->type == HORSE_HNI))) {
        if (fqrand() < 0.6f) {
            this->dustFlags |= 1;
            Skin_MatrixPosition2_gfx(skin, 28, &hoofOffset, &this->frontRightHoof);
            this->frontRightHoof.y -= 5.0f;
        }
    } else {
        if (this->action == ENHORSE_ACT_STOPPING) {
            if ((frame > 10.0f && frame < 13.0f) || (frame > 25.0f && frame < 33.0f)) {
                if (fqrand() < 0.7f) {
                    this->dustFlags |= 2;
                    Skin_MatrixPosition2_gfx(skin, 20, &hoofOffset, &sp70);
                    position_plus_rnd(&sp70, 10.0f, &this->frontLeftHoof);
                }
                if (fqrand() < 0.7f) {
                    this->dustFlags |= 1;
                    Skin_MatrixPosition2_gfx(skin, 28, &hoofOffset, &sp70);
                    position_plus_rnd(&sp70, 10.0f, &this->frontRightHoof);
                }
            }

            if ((frame > 6.0f && frame < 10.0f) || (frame > 23.0f && frame < 29.0f)) {
                if (fqrand() < 0.7f) {
                    this->dustFlags |= 8;
                    Skin_MatrixPosition2_gfx(skin, 37, &hoofOffset, &sp70);
                    position_plus_rnd(&sp70, 10.0f, &this->backLeftHoof);
                }
            }

            if ((frame > 7.0f && frame < 14.0f) || (frame > 26.0f && frame < 30.0f)) {
                if (fqrand() < 0.7f) {
                    this->dustFlags |= 4;
                    Skin_MatrixPosition2_gfx(skin, 45, &hoofOffset, &sp70);
                    position_plus_rnd(&sp70, 10.0f, &this->backRightHoof);
                }
            }
        } else if (this->animationIdx == ENHORSE_ANIM_GALLOP) {
            if ((frame > 14.0f) && (frame < 16.0f)) {
                this->dustFlags |= 1;
                Skin_MatrixPosition2_gfx(skin, 28, &hoofOffset, &sp70);
                position_plus_rnd(&sp70, 5.0f, &this->frontRightHoof);
            } else if (frame > 8.0f && frame < 10.0f) {
                this->dustFlags |= 2;
                Skin_MatrixPosition2_gfx(skin, 20, &hoofOffset, &sp70);
                position_plus_rnd(&sp70, 10.0f, &this->frontLeftHoof);
            } else if (frame > 1.0f && frame < 3.0f) {
                this->dustFlags |= 4;
                Skin_MatrixPosition2_gfx(skin, 45, &hoofOffset, &sp70);
                position_plus_rnd(&sp70, 10.0f, &this->backRightHoof);
            } else if ((frame > 26.0f) && (frame < 28.0f)) {
                this->dustFlags |= 8;
                Skin_MatrixPosition2_gfx(skin, 37, &hoofOffset, &sp70);
                position_plus_rnd(&sp70, 10.0f, &this->backLeftHoof);
            }
        } else if (this->action == ENHORSE_ACT_LOW_JUMP && frame > 6.0f &&
                   fqrand() < 1.0f - (frame - 6.0f) * (1.0f / 17.0f)) {
            if (fqrand() < 0.5f) {
                this->dustFlags |= 8;
                Skin_MatrixPosition2_gfx(skin, 37, &hoofOffset, &sp70);
                position_plus_rnd(&sp70, 10.0f, &this->backLeftHoof);
            }
            if (fqrand() < 0.5f) {
                this->dustFlags |= 4;
                Skin_MatrixPosition2_gfx(skin, 45, &hoofOffset, &sp70);
                position_plus_rnd(&sp70, 10.0f, &this->backRightHoof);
            }
        } else if (this->action == ENHORSE_ACT_HIGH_JUMP && frame > 5.0f &&
                   fqrand() < 1.0f - (frame - 5.0f) * (1.0f / 25.0f)) {
            if (fqrand() < 0.5f) {
                this->dustFlags |= 8;
                Skin_MatrixPosition2_gfx(skin, 37, &hoofOffset, &sp70);
                position_plus_rnd(&sp70, 10.0f, &this->backLeftHoof);
            }
            if (fqrand() < 0.5f) {
                this->dustFlags |= 4;
                Skin_MatrixPosition2_gfx(skin, 45, &hoofOffset, &sp70);
                position_plus_rnd(&sp70, 10.0f, &this->backRightHoof);
            }
        } else if (this->action == ENHORSE_ACT_BRIDGE_JUMP && fqrand() < 0.5f) {
            if (fqrand() < 0.5f) {
                this->dustFlags |= 8;
                Skin_MatrixPosition2_gfx(skin, 37, &hoofOffset, &sp70);
                position_plus_rnd(&sp70, 10.0f, &this->backLeftHoof);
            } else {
                this->dustFlags |= 4;
                Skin_MatrixPosition2_gfx(skin, 45, &hoofOffset, &sp70);
                position_plus_rnd(&sp70, 10.0f, &this->backRightHoof);
            }
        }
    }

    for (i = 0; i < this->jntSph.count; i++) {
        center.x = this->jntSph.elements[i].dim.modelSphere.center.x;
        center.y = this->jntSph.elements[i].dim.modelSphere.center.y;
        center.z = this->jntSph.elements[i].dim.modelSphere.center.z;

        Skin_MatrixPosition2_gfx(skin, this->jntSph.elements[i].dim.limb, &center, &newCenter);
        this->jntSph.elements[i].dim.worldSphere.center.x = newCenter.x;
        this->jntSph.elements[i].dim.worldSphere.center.y = newCenter.y;
        this->jntSph.elements[i].dim.worldSphere.center.z = newCenter.z;
        this->jntSph.elements[i].dim.worldSphere.radius =
            this->jntSph.elements[i].dim.modelSphere.radius * this->jntSph.elements[i].dim.scale;
    }

    //! @bug Setting colliders in a draw function allows for duplicate entries to be added to their respective lists
    //! under certain conditions, like when pausing and unpausing the game.
    //! Actors will draw for a couple of frames between the pauses, but some important logic updates will not occur.
    //! In the case of OC, this can cause unwanted effects such as a very large amount of displacement being applied to
    //! a colliding actor.
    CollisionCheck_setOC(play, &play->colChkCtx, &this->jntSph.base);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->jntSph.base);
}

// unused
static s32 fast_run_on_frames[] = { 0, 3, 7, 14 };

s32 En_Horse_joint_proc(Actor* thisx, PlayState* play, s32 limbIndex, Skin* arg3) {
    static void* eye_txt[] = {
        gEponaEyeOpenTex,
        gEponaEyeHalfTex,
        gEponaEyeClosedTex,
    };
    static u8 mepachi_tbl[] = { 0, 1, 2, 1 };
    EnHorse* this = (EnHorse*)thisx;
    s32 drawOriginalLimb = true;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_horse.c", 8582);
    if (limbIndex == 13 && this->type == HORSE_EPONA) {
        u8 index = mepachi_tbl[this->blinkTimer];

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[index]));
    } else if (this->type == HORSE_HNI && this->stateFlags & ENHORSE_FLAG_18 && limbIndex == 30) {
        Skin_gfx_mesh2_disp(play->state.gfxCtx, &this->skin, limbIndex, gHorseIngoGerudoSaddleDL, 0);
        drawOriginalLimb = false;
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_horse.c", 8601);
    return drawOriginalLimb;
}

void En_Horse_display(Actor* thisx, PlayState* play) {
    EnHorse* this = (EnHorse*)thisx;

    if (!(this->stateFlags & ENHORSE_INACTIVE)) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        this->stateFlags |= ENHORSE_DRAW;
        if (this->stateFlags & ENHORSE_JUMPING) {
            Skin_disp3(thisx, play, &this->skin, En_Horse_calc2, En_Horse_joint_proc, false);
        } else {
            Skin_disp3(thisx, play, &this->skin, En_Horse_calc2, En_Horse_joint_proc, true);
        }
        if (this->postDrawFunc != NULL) {
            this->postDrawFunc(this, play);
        }
    }
}
