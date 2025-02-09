#include "ultra64.h"
#include "global.h"
#include "quake.h"
#include "terminal.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ique-cn:128 ntsc-1.0:128 ntsc-1.1:128 ntsc-1.2:128 pal-1.0:128 pal-1.1:128 hiratsu3:128"

s16 change_camera_set(Camera* camera, s16 requestedSetting, s16 flags);
s32 change_camera_mode(Camera* camera, s16 requestedMode, u8 forceModeChange);
static s32 water_check(Camera* camera);

#if PLATFORM_N64
#define CAMERA_CHECK_BTN(input, btn) pad_on_trigger((input), (btn))
#else
#define CAMERA_CHECK_BTN(input, btn) CHECK_BTN_ALL((input)->press.button, (btn))
#endif

#if DEBUG_FEATURES
s32 Camera_QRegInit(void);
#endif

#if DEBUG_FEATURES
#define CAM_DEBUG_RELOAD_PREG(camera)        \
    if (R_RELOAD_CAM_PARAMS) {               \
        copyCameraPosPREGToModeValues(camera); \
    }                                        \
    (void)0
#else
#define CAM_DEBUG_RELOAD_PREG(camera) (void)0
#endif

// Camera will reload its paramData. Usually that means setting the read-only data from what is stored in
// CameraModeValue arrays. Although sometimes some read-write data is reset as well
#define RELOAD_PARAMS(camera) (camera->animState == 0 || camera->animState == 10 || camera->animState == 20)

#if DEBUG_FEATURES
#define CAM_DEBUG_RELOAD_PARAMS R_RELOAD_CAM_PARAMS
#else
#define CAM_DEBUG_RELOAD_PARAMS true
#endif

/**
 * Camera data is stored in both read-only data and OREG as s16, and then converted to the appropriate type during
 * runtime. If a small f32 is being stored as an s16, it is common to store that value 100 times larger than the
 * original value. This is then scaled back down during runtime with the CAM_DATA_SCALED macro.
 */
#define CAM_DATA_SCALED(x) ((x)*0.01f)

// Load the next value from camera read-only data stored in CameraModeValue
#define GET_NEXT_RO_DATA(values) ((values++)->val)
// Load the next value and scale down from camera read-only data stored in CameraModeValue
#define GET_NEXT_SCALED_RO_DATA(values) CAM_DATA_SCALED(GET_NEXT_RO_DATA(values))

#if DEBUG_FEATURES

#define CAM_GLOBAL_0 OREG(0)
#define CAM_GLOBAL_1 OREG(1)
#define CAM_XZ_OFFSET_UPDATE_RATE CAM_DATA_SCALED(R_CAM_XZ_OFFSET_UPDATE_RATE)
#define CAM_Y_OFFSET_UPDATE_RATE CAM_DATA_SCALED(R_CAM_Y_OFFSET_UPDATE_RATE)
#define CAM_FOV_UPDATE_RATE CAM_DATA_SCALED(R_CAM_FOV_UPDATE_RATE)
#define CAM_MAX_PITCH R_CAM_MAX_PITCH
#define CAM_R_UPDATE_RATE_INV R_CAM_R_UPDATE_RATE_INV
#define CAM_PITCH_UPDATE_RATE_INV R_CAM_PITCH_UPDATE_RATE_INV
#define CAM_GLOBAL_8 CAM_DATA_SCALED(OREG(8))
#define CAM_SLOPE_Y_ADJ_AMOUNT R_CAM_SLOPE_Y_ADJ_AMOUNT
#define CAM_GLOBAL_10 CAM_DATA_SCALED(OREG(10))
#define CAM_GLOBAL_11 CAM_DATA_SCALED(OREG(11))
#define CAM_GLOBAL_12 CAM_DATA_SCALED(OREG(12))
#define CAM_GLOBAL_13 OREG(13)
#define CAM_GLOBAL_14 OREG(14)
#define CAM_GLOBAL_15 OREG(15)
#define CAM_GLOBAL_16 OREG(16)
#define CAM_PITCH_FLOOR_CHECK_NEAR_DIST_FAC CAM_DATA_SCALED(R_CAM_PITCH_FLOOR_CHECK_NEAR_DIST_FAC)
#define CAM_PITCH_FLOOR_CHECK_FAR_DIST_FAC CAM_DATA_SCALED(R_CAM_PITCH_FLOOR_CHECK_FAR_DIST_FAC)
#define CAM_PITCH_FLOOR_CHECK_OFFSET_Y_FAC CAM_DATA_SCALED(R_CAM_PITCH_FLOOR_CHECK_OFFSET_Y_FAC)
#define CAM_PITCH_FLOOR_CHECK_NEAR_WEIGHT CAM_DATA_SCALED(R_CAM_PITCH_FLOOR_CHECK_NEAR_WEIGHT)
#define CAM_GLOBAL_21 OREG(21)
#define CAM_GLOBAL_22 CAM_DATA_SCALED(OREG(22))
#define CAM_DEFAULT_ANIM_TIME R_CAM_DEFAULT_ANIM_TIME
#define CAM_GLOBAL_24 OREG(24)
#define CAM_UPDATE_RATE_STEP_SCALE_XZ CAM_DATA_SCALED(R_CAM_UPDATE_RATE_STEP_SCALE_XZ)
#define CAM_UPDATE_RATE_STEP_SCALE_Y CAM_DATA_SCALED(R_CAM_UPDATE_RATE_STEP_SCALE_Y)
#define CAM_GLOBAL_27 OREG(27)
#define CAM_GLOBAL_28 CAM_DATA_SCALED(OREG(28))
#define CAM_GLOBAL_29 CAM_DATA_SCALED(OREG(29))
#define CAM_GLOBAL_30 CAM_DATA_SCALED(OREG(30))
#define CAM_JUMP1_EYE_Y_STEP_SCALE CAM_DATA_SCALED(R_CAM_JUMP1_EYE_Y_STEP_SCALE)
#define CAM_GLOBAL_32 OREG(32)
#define CAM_GLOBAL_33 OREG(33)
#define CAM_MIN_PITCH_1 R_CAM_MIN_PITCH_1
#define CAM_MIN_PITCH_2 R_CAM_MIN_PITCH_2
#define CAM_BATTLE1_ROLL_TARGET_BASE R_CAM_BATTLE1_ROLL_TARGET_BASE
#define CAM_BATTLE1_ROLL_STEP_SCALE CAM_DATA_SCALED(R_CAM_BATTLE1_ROLL_STEP_SCALE)
#define CAM_GLOBAL_38 CAM_DATA_SCALED(OREG(38))
#define CAM_GLOBAL_39 CAM_DATA_SCALED(OREG(39))
#define CAM_BATTLE1_XYZ_OFFSET_UPDATE_RATE_TARGET CAM_DATA_SCALED(R_CAM_BATTLE1_XYZ_OFFSET_UPDATE_RATE_TARGET)
#define CAM_AT_LERP_STEP_SCALE_MIN CAM_DATA_SCALED(R_CAM_AT_LERP_STEP_SCALE_MIN)
#define CAM_AT_LERP_STEP_SCALE_FAC CAM_DATA_SCALED(R_CAM_AT_LERP_STEP_SCALE_FAC)
#define CAM_GLOBAL_43 CAM_DATA_SCALED(OREG(43))
#define CAM_GLOBAL_44 OREG(44)
#define CAM_GLOBAL_45 OREG(45)
#define CAM_YOFFSET_NORM CAM_DATA_SCALED(R_CAM_YOFFSET_NORM)
#define CAM_GLOBAL_47 OREG(47)
#define CAM_GLOBAL_48 CAM_DATA_SCALED(OREG(48))
#define CAM_GLOBAL_49 CAM_DATA_SCALED(OREG(49))
#define CAM_GLOBAL_50 OREG(50)
#define CAM_GLOBAL_51 OREG(51)
#define CAM_GLOBAL_52 OREG(52)

#else

#define CAM_GLOBAL_0 0
#define CAM_GLOBAL_1 1
#define CAM_XZ_OFFSET_UPDATE_RATE .05f
#define CAM_Y_OFFSET_UPDATE_RATE .05f
#define CAM_FOV_UPDATE_RATE .05f
#define CAM_MAX_PITCH 14500
#define CAM_R_UPDATE_RATE_INV 20
#define CAM_PITCH_UPDATE_RATE_INV 16
#define CAM_GLOBAL_8 1.5f
#define CAM_SLOPE_Y_ADJ_AMOUNT 25
#define CAM_GLOBAL_10 1.5f
#define CAM_GLOBAL_11 0.06f
#define CAM_GLOBAL_12 .10f
#define CAM_GLOBAL_13 10
#define CAM_GLOBAL_14 0
#define CAM_GLOBAL_15 0
#define CAM_GLOBAL_16 1
#define CAM_PITCH_FLOOR_CHECK_NEAR_DIST_FAC 1.0f
#define CAM_PITCH_FLOOR_CHECK_FAR_DIST_FAC 2.5f
#define CAM_PITCH_FLOOR_CHECK_OFFSET_Y_FAC 1.2f
#define CAM_PITCH_FLOOR_CHECK_NEAR_WEIGHT 0.8f
#define CAM_GLOBAL_21 30
#define CAM_GLOBAL_22 1.2f
#define CAM_DEFAULT_ANIM_TIME 4
#define CAM_GLOBAL_24 1
#define CAM_UPDATE_RATE_STEP_SCALE_XZ .50f
#define CAM_UPDATE_RATE_STEP_SCALE_Y .20f
#define CAM_GLOBAL_27 1800
#define CAM_GLOBAL_28 0.5f
#define CAM_GLOBAL_29 0.5f
#define CAM_GLOBAL_30 0.5f
#define CAM_JUMP1_EYE_Y_STEP_SCALE 0.2f
#define CAM_GLOBAL_32 20
#define CAM_GLOBAL_33 -10
#define CAM_MIN_PITCH_1 -5460
#define CAM_MIN_PITCH_2 -9100
#define CAM_BATTLE1_ROLL_TARGET_BASE -6
#define CAM_BATTLE1_ROLL_STEP_SCALE 0.08f
#define CAM_GLOBAL_38 0.15f
#define CAM_GLOBAL_39 0.75f
#define CAM_BATTLE1_XYZ_OFFSET_UPDATE_RATE_TARGET 0.6f
#define CAM_AT_LERP_STEP_SCALE_MIN 0.12f
#define CAM_AT_LERP_STEP_SCALE_FAC 1.1f
#define CAM_GLOBAL_43 0.4f
#define CAM_GLOBAL_44 50
#define CAM_GLOBAL_45 250
#define CAM_YOFFSET_NORM -0.1f
#define CAM_GLOBAL_47 30
#define CAM_GLOBAL_48 0.3f
#define CAM_GLOBAL_49 0.7f
#define CAM_GLOBAL_50 20
#define CAM_GLOBAL_51 20
#define CAM_GLOBAL_52 20

#endif

#define DISTORTION_HOT_ROOM (1 << 0)
#define DISTORTION_UNDERWATER_WEAK (1 << 1)
#define DISTORTION_UNDERWATER_MEDIUM (1 << 2)
#define DISTORTION_UNDERWATER_STRONG (1 << 3)
#define DISTORTION_UNDERWATER_FISHING (1 << 4)

#define CAM_REQUEST_SETTING_FORCE_CHANGE (1 << 0)
// If set, then any other setting requests on the same frame will skip a priority check
// and overwrite the request
#define CAM_REQUEST_SETTING_IGNORE_PRIORITY (1 << 1)
#define CAM_REQUEST_SETTING_PRESERVE_BG_CAM_INDEX (1 << 2)
#define CAM_REQUEST_SETTING_RESTORE_PREV_BG_CAM_INDEX (1 << 3)

#include "ultra64.h"
#include "global.h"

typedef struct CameraModeValue {
    s16 val;
    s16 dataType;
} CameraModeValue;

typedef struct CameraMode {
    s16 funcIdx;
    s16 valueCnt;
    CameraModeValue* values;
} CameraMode;

typedef struct CameraSetting {
    union {
        u32 unk_00;
        struct {
            u32 unk_bit0 : 1;
            u32 unk_bit1 : 1;
            u32 validModes : 30;
        };
    };
    CameraMode* cameraModes;
} CameraSetting;

/*==================================================================*/
// Data

#if DEBUG_FEATURES
s16 sOREGInit[] = {
    0,     // OREG(0)
    1,     // OREG(1)
    5,     // R_CAM_XZ_OFFSET_UPDATE_RATE
    5,     // R_CAM_Y_OFFSET_UPDATE_RATE
    5,     // R_CAM_FOV_UPDATE_RATE
    14500, // R_CAM_MAX_PITCH
    20,    // R_CAM_R_UPDATE_RATE_INV
    16,    // R_CAM_PITCH_UPDATE_RATE_INV
    150,   // OREG(8)
    25,    // R_CAM_SLOPE_Y_ADJ_AMOUNT
    150,   // OREG(10)
    6,     // OREG(11)
    10,    // OREG(12)
    10,    // OREG(13)
    0,     // OREG(14) (unused)
    0,     // OREG(15) (unused)
    1,     // OREG(16) (unused)
    100,   // R_CAM_PITCH_FLOOR_CHECK_NEAR_DIST_FAC
    250,   // R_CAM_PITCH_FLOOR_CHECK_FAR_DIST_FAC
    120,   // R_CAM_PITCH_FLOOR_CHECK_OFFSET_Y_FAC
    80,    // R_CAM_PITCH_FLOOR_CHECK_NEAR_WEIGHT
    30,    // OREG(21)
    120,   // OREG(22)
    4,     // R_CAM_DEFAULT_ANIM_TIME
    1,     // OREG(24)
    50,    // R_CAM_UPDATE_RATE_STEP_SCALE_XZ
    20,    // R_CAM_UPDATE_RATE_STEP_SCALE_Y
    1800,  // OREG(27)
    50,    // OREG(28)
    50,    // OREG(29)
    50,    // OREG(30)
    20,    // R_CAM_JUMP1_EYE_Y_STEP_SCALE
    20,    // OREG(32)
    -10,   // OREG(33)
    -5460, // R_CAM_MIN_PITCH_1
    -9100, // R_CAM_MIN_PITCH_2
    -6,    // R_CAM_BATTLE1_ROLL_TARGET_BASE
    8,     // R_CAM_BATTLE1_ROLL_STEP_SCALE
    15,    // OREG(38)
    75,    // OREG(39)
    60,    // R_CAM_BATTLE1_XYZ_OFFSET_UPDATE_RATE_TARGET
    12,    // R_CAM_AT_LERP_STEP_SCALE_MIN
    110,   // R_CAM_AT_LERP_STEP_SCALE_FAC
    40,    // OREG(43)
    50,    // OREG(44)
    250,   // OREG(45)
    -10,   // R_CAM_YOFFSET_NORM
    30,    // OREG(47) (unused)
    30,    // OREG(48)
    70,    // OREG(49)
    20,    // OREG(50)
    20,    // OREG(51)
    20,    // OREG(52)
};

s16 sOREGInitCnt = ARRAY_COUNT(sOREGInit);
#endif

s16 p_reg_init_data[CAM_DATA_MAX] = {
    -20, // CAM_DATA_Y_OFFSET
    200, // CAM_DATA_EYE_DIST
    300, // CAM_DATA_EYE_DIST_NEXT
    10,  // CAM_DATA_PITCH_TARGET
    12,  // CAM_DATA_YAW_UPDATE_RATE_TARGET
    10,  // CAM_DATA_XZ_UPDATE_RATE_TARGET
    35,  // CAM_DATA_MAX_YAW_UPDATE
    60,  // CAM_DATA_FOV
    60,  // CAM_DATA_AT_LERP_STEP_SCALE
    3,   // CAM_DATA_INTERFACE_FIELD
    0,   // CAM_DATA_YAW_TARGET
    -40, // CAM_DATA_GROUND_Y_OFFSET
    20,  // CAM_DATA_GROUND_AT_LERP_STEP_SCALE
    25,  // CAM_DATA_SWING_YAW_INIT
    45,  // CAM_DATA_SWING_YAW_FINAL
    -5,  // CAM_DATA_SWING_PITCH_INIT
    15,  // CAM_DATA_SWING_PITCH_FINAL
    15,  // CAM_DATA_SWING_PITCH_ADJ
    20,  // CAM_DATA_MIN_MAX_DIST_FACTOR
    0,   // CAM_DATA_AT_OFFSET_X
    0,   // CAM_DATA_AT_OFFSET_Y
    0,   // CAM_DATA_AT_OFFSET_Z
    6,   // CAM_DATA_UNK_22
    60,  // CAM_DATA_UNK_23
    30,  // CAM_DATA_FOV_SCALE
    0,   // CAM_DATA_YAW_SCALE
    5,   // CAM_DATA_UNK_26
};

s16 n_p_reg_init_data = ARRAY_COUNT(p_reg_init_data);

#if DEBUG_FEATURES
char sCameraSettingNames[][12] = {
    "NONE      ",  // CAM_SET_NONE
    "NORMAL0    ", // CAM_SET_NORMAL0
    "NORMAL1    ", // CAM_SET_NORMAL1
    "DUNGEON0   ", // CAM_SET_DUNGEON0
    "DUNGEON1   ", // CAM_SET_DUNGEON1
    "NORMAL3    ", // CAM_SET_NORMAL3
    "HORSE0     ", // CAM_SET_HORSE
    "BOSS_GOMA  ", // CAM_SET_BOSS_GOHMA
    "BOSS_DODO  ", // CAM_SET_BOSS_DODONGO
    "BOSS_BARI  ", // CAM_SET_BOSS_BARINADE
    "BOSS_FGANON", // CAM_SET_BOSS_PHANTOM_GANON
    "BOSS_BAL   ", // CAM_SET_BOSS_VOLVAGIA
    "BOSS_SHADES", // CAM_SET_BOSS_BONGO
    "BOSS_MOFA  ", // CAM_SET_BOSS_MORPHA
    "BOSS_TWIN0 ", // CAM_SET_BOSS_TWINROVA_PLATFORM
    "BOSS_TWIN1 ", // CAM_SET_BOSS_TWINROVA_FLOOR
    "BOSS_GANON1", // CAM_SET_BOSS_GANONDORF
    "BOSS_GANON2", // CAM_SET_BOSS_GANON
    "TOWER0     ", // CAM_SET_TOWER_CLIMB
    "TOWER1     ", // CAM_SET_TOWER_UNUSED
    "FIXED0     ", // CAM_SET_MARKET_BALCONY
    "FIXED1     ", // CAM_SET_CHU_BOWLING
    "CIRCLE0    ", // CAM_SET_PIVOT_CRAWLSPACE
    "CIRCLE2    ", // CAM_SET_PIVOT_SHOP_BROWSING
    "CIRCLE3    ", // CAM_SET_PIVOT_IN_FRONT
    "PREREND0   ", // CAM_SET_PREREND_FIXED
    "PREREND1   ", // CAM_SET_PREREND_PIVOT
    "PREREND3   ", // CAM_SET_PREREND_SIDE_SCROLL
    "DOOR0      ", // CAM_SET_DOOR0
    "DOORC      ", // CAM_SET_DOORC
    "RAIL3      ", // CAM_SET_CRAWLSPACE
    "START0     ", // CAM_SET_START0
    "START1     ", // CAM_SET_START1
    "FREE0      ", // CAM_SET_FREE0
    "FREE2      ", // CAM_SET_FREE2
    "CIRCLE4    ", // CAM_SET_PIVOT_CORNER
    "CIRCLE5    ", // CAM_SET_PIVOT_WATER_SURFACE
    "DEMO0      ", // CAM_SET_CS_0
    "DEMO1      ", // CAM_SET_CS_TWISTED_HALLWAY
    "MORI1      ", // CAM_SET_FOREST_BIRDS_EYE
    "ITEM0      ", // CAM_SET_SLOW_CHEST_CS
    "ITEM1      ", // CAM_SET_ITEM_UNUSED
    "DEMO3      ", // CAM_SET_CS_3
    "DEMO4      ", // CAM_SET_CS_ATTENTION
    "UFOBEAN    ", // CAM_SET_BEAN_GENERIC
    "LIFTBEAN   ", // CAM_SET_BEAN_LOST_WOODS
    "SCENE0     ", // CAM_SET_SCENE_UNUSED
    "SCENE1     ", // CAM_SET_SCENE_TRANSITION
    "HIDAN1     ", // CAM_SET_ELEVATOR_PLATFORM
    "HIDAN2     ", // CAM_SET_FIRE_STAIRCASE
    "MORI2      ", // CAM_SET_FOREST_UNUSED
    "MORI3      ", // CAM_SET_FOREST_DEFEAT_POE
    "TAKO       ", // CAM_SET_BIG_OCTO
    "SPOT05A    ", // CAM_SET_MEADOW_BIRDS_EYE
    "SPOT05B    ", // CAM_SET_MEADOW_UNUSED
    "HIDAN3     ", // CAM_SET_FIRE_BIRDS_EYE
    "ITEM2      ", // CAM_SET_TURN_AROUND
    "CIRCLE6    ", // CAM_SET_PIVOT_VERTICAL
    "NORMAL2    ", // CAM_SET_NORMAL2
    "FISHING    ", // CAM_SET_FISHING
    "DEMOC      ", // CAM_SET_CS_C
    "UO_FIBER   ", // CAM_SET_JABU_TENTACLE
    "DUNGEON2   ", // CAM_SET_DUNGEON2
    "TEPPEN     ", // CAM_SET_DIRECTED_YAW
    "CIRCLE7    ", // CAM_SET_PIVOT_FROM_SIDE
    "NORMAL4    ", // CAM_SET_NORMAL4
};

char sCameraModeNames[][12] = {
    "NORMAL     ", // CAM_MODE_NORMAL
    "PARALLEL   ", // CAM_MODE_Z_PARALLEL
    "KEEPON     ", // CAM_MODE_Z_TARGET_FRIENDLY
    "TALK       ", // CAM_MODE_TALK
    "BATTLE     ", // CAM_MODE_Z_TARGET_UNFRIENDLY
    "CLIMB      ", // CAM_MODE_WALL_CLIMB
    "SUBJECT    ", // CAM_MODE_FIRST_PERSON
    "BOWARROW   ", // CAM_MODE_AIM_ADULT
    "BOWARROWZ  ", // CAM_MODE_Z_AIM
    "FOOKSHOT   ", // CAM_MODE_HOOKSHOT_FLY
    "BOOMERANG  ", // CAM_MODE_AIM_BOOMERANG
    "PACHINCO   ", // CAM_MODE_AIM_CHILD
    "CLIMBZ     ", // CAM_MODE_Z_WALL_CLIMB
    "JUMP       ", // CAM_MODE_JUMP
    "HANG       ", // CAM_MODE_LEDGE_HANG
    "HANGZ      ", // CAM_MODE_Z_LEDGE_HANG
    "FREEFALL   ", // CAM_MODE_FREE_FALL
    "CHARGE     ", // CAM_MODE_CHARGE
    "STILL      ", // CAM_MODE_STILL
    "PUSHPULL   ", // CAM_MODE_PUSH_PULL
    "BOOKEEPON  ", // CAM_MODE_FOLLOW_BOOMERANG
};
#endif

/**
 *=====================================================================
 *                   General Data: NORMAL0 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list0[] = {
    CAM_FUNCDATA_NORM1(
        -20, 200, 300, 10, 12, 10, 35, 60, 60,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list1[] = {
    CAM_FUNCDATA_PARA1(
        -20, 250, 0, 0, 5, 5, 45, 50,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1), -40,
        20),
};

CameraModeValue cam_param_list2[] = {
    CAM_FUNCDATA_KEEP1(-20, 120, 140, 25, 45, -5, 15, 15, 45, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, KEEPON1_FLAG_0), -50, 30),
};

CameraModeValue cam_param_list3[] = {
    CAM_FUNCDATA_KEEP3(-30, 70, 200, 40, 10, 0, 5, 70, 45, 50, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE, 0)),
};

CameraModeValue cam_param_list4[] = {
    CAM_FUNCDATA_BATT1(-20, 180, 10, 80, 0, 10, 25, 50, 80,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -40, 25),
};

CameraModeValue cam_param_list5[] = {
    CAM_FUNCDATA_JUMP2(-20, 200, 300, 20, 5, 5, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list6[] = {
    CAM_FUNCDATA_SUBJ3(0, 5, 50, 10, 0, 0, 0, 45, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list7[] = {
    CAM_FUNCDATA_SUBJ3(-7, 14, 50, 10, 0, -30, -5, 45,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list8[] = {
    CAM_FUNCDATA_SUBJ3(20, 70, 70, 10, -120, 20, 0, 45,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list9[] = {
    CAM_FUNCDATA_SPEC5_ALT(-20, 80, 250, 45, 60, 40, 6,
                           CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list10[] = {
    CAM_FUNCDATA_SUBJ3(5, 50, 50, 10, 0, 0, 0, 45,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list11[] = {
    CAM_FUNCDATA_SUBJ3(-7, 14, 50, 10, -9, -63, -30, 45,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list12[] = {
    CAM_FUNCDATA_JUMP2(-20, 200, 300, 20, 999, 5, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, JUMP2_FLAG_2 | JUMP2_FLAG_1)),
};

CameraModeValue cam_param_list13[] = {
    CAM_FUNCDATA_JUMP1(-20, 200, 300, 12, 35, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list14[] = {
    CAM_FUNCDATA_UNIQ1(-80, 200, 300, 40, 60, 10, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list15[] = {
    CAM_FUNCDATA_UNIQ1(-120, 300, 300, 70, 45, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list16[] = {
    CAM_FUNCDATA_JUMP1(-20, 200, 300, 15, 80, 60, 20,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list17[] = {
    CAM_FUNCDATA_BATT4(-20, 300, 50, 2, 80, 20, CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list18[] = {
    CAM_FUNCDATA_NORM1(
        -20, 200, 300, 10, 100, 10, 100, 60, 5,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list19[] = {
    CAM_FUNCDATA_PARA1(0, 250, 25, 0, 5, 5, 70, 30,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL,
                                           PARALLEL1_FLAG_6 | PARALLEL1_FLAG_5 | PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1),
                       -20, 30),
};

CameraModeValue cam_param_list20[] = {
    CAM_FUNCDATA_KEEP1(-5, 120, 140, 5, 85, 10, 5, 25, 45, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, KEEPON1_FLAG_0), -15, 30),
};

/**
 *=====================================================================
 *                   Custom Data: NORMAL1 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list21[] = {
    CAM_FUNCDATA_NORM1(
        0, 200, 400, 10, 12, 20, 40, 60, 60,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list22[] = {
    CAM_FUNCDATA_PARA1(0, 250, 0, 0, 5, 5, 45, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, PARALLEL1_FLAG_1), -40, 20),
};

CameraModeValue cam_param_list23[] = {
    CAM_FUNCDATA_KEEP1(-20, 120, 140, 25, 45, -5, 15, 15, 45, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, KEEPON1_FLAG_0), -50, 20),
};

CameraModeValue cam_param_list24[] = {
    CAM_FUNCDATA_BATT1(-20, 250, 10, 80, 0, 10, 25, 50, 65,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -40, 25),
};

CameraModeValue cam_param_list25[] = {
    CAM_FUNCDATA_SPEC5(-20, 80, 250, 6, 45, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list26[] = {
    CAM_FUNCDATA_JUMP1(0, 250, 400, 15, 50, 60, 30, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list27[] = {
    CAM_FUNCDATA_JUMP1(0, 200, 400, 30, 80, 60, 20, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list28[] = {
    CAM_FUNCDATA_JUMP2(-20, 200, 400, 20, 5, 5, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list29[] = {
    CAM_FUNCDATA_JUMP2(-20, 250, 400, 20, 999, 5, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, JUMP2_FLAG_2 | JUMP2_FLAG_1)),
};

CameraModeValue cam_param_list30[] = {
    CAM_FUNCDATA_BATT4(0, 300, 50, 2, 80, 20, CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list31[] = {
    CAM_FUNCDATA_UNIQ1(-80, 200, 400, 40, 60, 10, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list32[] = {
    CAM_FUNCDATA_UNIQ1(-120, 400, 400, 70, 45, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list33[] = {
    CAM_FUNCDATA_NORM1(
        0, 200, 400, 10, 100, 20, 100, 60, 5,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

/**
 *=====================================================================
 *                   Custom Data: DUNGEON0 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list34[] = {
    CAM_FUNCDATA_NORM1(
        -10, 150, 250, 5, 10, 5, 30, 60, 60,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list35[] = {
    CAM_FUNCDATA_PARA1(
        -20, 150, 0, 0, 5, 5, 45, 50,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1), -40,
        20),
};

CameraModeValue cam_param_list36[] = {
    CAM_FUNCDATA_KEEP1(-20, 120, 140, 25, 45, -5, 15, 15, 45, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, KEEPON1_FLAG_0), -40, 20),
};

CameraModeValue cam_param_list37[] = {
    CAM_FUNCDATA_BATT1(-20, 180, 10, 80, 0, 10, 25, 45, 80,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -40, 25),
};

CameraModeValue cam_param_list38[] = {
    CAM_FUNCDATA_JUMP1(-10, 150, 250, 10, 50, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list39[] = {
    CAM_FUNCDATA_JUMP1(-10, 150, 250, 10, 80, 60, 20,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list40[] = {
    CAM_FUNCDATA_JUMP2(-40, 150, 250, 20, 5, 5, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list41[] = {
    CAM_FUNCDATA_JUMP2(-40, 250, 250, 20, 999, 5, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, JUMP2_FLAG_2 | JUMP2_FLAG_1)),
};

CameraModeValue cam_param_list42[] = {
    CAM_FUNCDATA_BATT4(-10, 300, 50, 2, 80, 20, CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list43[] = {
    CAM_FUNCDATA_UNIQ1(-80, 150, 250, 40, 60, 10, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list44[] = {
    CAM_FUNCDATA_UNIQ1(-120, 250, 250, 70, 45, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list45[] = {
    CAM_FUNCDATA_NORM1(
        -10, 150, 250, 5, 100, 5, 100, 60, 5,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

/**
 *=====================================================================
 *                   Custom Data: DUNGEON1 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list46[] = {
    CAM_FUNCDATA_NORM1(
        -40, 150, 150, 0, 10, 5, 30, 60, 60,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list47[] = {
    CAM_FUNCDATA_KEEP3(-20, 70, 200, 40, 10, 0, 5, 70, 45, 50, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE, 0)),
};

CameraModeValue cam_param_list48[] = {
    CAM_FUNCDATA_JUMP1(-40, 150, 150, 10, 50, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list49[] = {
    CAM_FUNCDATA_JUMP1(-40, 150, 180, 12, 80, 60, 20,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list50[] = {
    CAM_FUNCDATA_JUMP2(-40, 150, 150, 20, 5, 5, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list51[] = {
    CAM_FUNCDATA_JUMP2(-40, 150, 150, 20, 999, 5, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, JUMP2_FLAG_2 | JUMP2_FLAG_1)),
};

CameraModeValue cam_param_list52[] = {
    CAM_FUNCDATA_BATT4(-40, 200, 50, 2, 80, 20, CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list53[] = {
    CAM_FUNCDATA_UNIQ1(-80, 150, 150, 40, 60, 10, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list54[] = {
    CAM_FUNCDATA_UNIQ1(-120, 150, 150, 70, 45, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list55[] = {
    CAM_FUNCDATA_NORM1(
        -40, 150, 150, 0, 100, 5, 100, 60, 5,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list56[] = {
    CAM_FUNCDATA_PARA1(-40, 180, 25, 0, 5, 5, 60, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL,
                                           PARALLEL1_FLAG_6 | PARALLEL1_FLAG_5 | PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1),
                       -20, 30),
};

/**
 *=====================================================================
 *                   Custom Data: NORMAL3 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list57[] = {
    CAM_FUNCDATA_JUMP3(-20, 280, 300, 20, 15, 5, 40, 60, 100,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, JUMP3_FLAG_2)),
};

CameraModeValue cam_param_list58[] = {
    CAM_FUNCDATA_PARA1(
        -50, 250, 70, 0, 15, 5, 60, 100,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1), -50,
        20),
};

CameraModeValue cam_param_list59[] = {
    CAM_FUNCDATA_KEEP3(-30, 70, 200, 40, 10, 10, 20, 70, 45, 10, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE, 0)),
};

/*
 * These values are for when the eye is >= OREG(45) units below the surface of the water.
 */
CameraModeValue cam_param_list60[] = {
    CAM_FUNCDATA_JUMP3(-40, 150, 250, -5, 18, 5, 60, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, JUMP3_FLAG_2 | JUMP3_FLAG_0)),
};

/**
 *=====================================================================
 *                   Custom Data: HORSE Setting
 *=====================================================================
 */

CameraModeValue cam_param_list61[] = {
    CAM_FUNCDATA_NORM3(-50, 220, 250, 10, 16, 20, 60, 100,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP_FORCE, 0)),
};

CameraModeValue cam_param_list62[] = {
    CAM_FUNCDATA_NORM3(-40, 180, 220, -2, 12, 100, 45, 100,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP_FORCE, 0)),
};

CameraModeValue cam_param_list63[] = {
    CAM_FUNCDATA_SUBJ3(-7, 14, 100, 10, 0, -30, -5, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP_FORCE, 0)),
};

CameraModeValue cam_param_list64[] = {
    CAM_FUNCDATA_KEEP1(
        -60, 180, 220, 25, 45, -5, 15, 15, 45, 50,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP_FORCE, KEEPON1_FLAG_0), -60,
        20),
};

CameraModeValue cam_param_list65[] = {
    CAM_FUNCDATA_KEEP3(-60, 140, 200, 40, 10, 0, 5, 70, 45, 50, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE, 0)),
};

/**
 *=====================================================================
 *                   Custom Data: BOSS_GOHMA Setting
 *=====================================================================
 */

CameraModeValue cam_param_list66[] = {
    CAM_FUNCDATA_NORM1(-20, 150, 250, 0, 15, 5, 40, 60, 60,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list67[] = {
    CAM_FUNCDATA_BATT1(-30, 150, 10, 40, -10, 0, 25, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -50, 20),
};

/**
 *=====================================================================
 *                 Custom Data: BOSS_DODONGO Setting
 *=====================================================================
 */

CameraModeValue cam_param_list68[] = {
    CAM_FUNCDATA_NORM1(
        0, 150, 300, 0, 12, 5, 70, 70, 40,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list69[] = {
    CAM_FUNCDATA_BATT1(-20, 160, 10, 60, -5, 0, 25, 70, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -40, 20),
};

/**
 *=====================================================================
 *                Custom Data: BOSS_BARINADE Setting
 *=====================================================================
 */

CameraModeValue cam_param_list70[] = {
    CAM_FUNCDATA_NORM1(
        -20, 150, 300, -5, 15, 5, 40, 70, 70,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list71[] = {
    CAM_FUNCDATA_BATT1(-30, 125, 10, 10, 0, 0, 50, 60, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -50, 20),
};

/**
 *=====================================================================
 *              Custom Data: BOSS_PHANTOM_GANON Setting
 *=====================================================================
 */

CameraModeValue cam_param_list72[] = {
    CAM_FUNCDATA_NORM1(
        10, 150, 250, 0, 15, 15, 40, 60, 100,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list73[] = {
    CAM_FUNCDATA_BATT1(-20, 200, 45, 40, 5, -5, 35, 60, 100,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -40, 60),
};

/**
 *=====================================================================
 *                Custom Data: BOSS_VOLVAGIA Setting
 *=====================================================================
 */

CameraModeValue cam_param_list74[] = {
    CAM_FUNCDATA_NORM1(
        -20, 500, 500, 10, 16, 10, 40, 60, 80,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list75[] = {
    CAM_FUNCDATA_BATT1(-20, 200, 20, 60, 0, 10, 15, 45, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -40, 20),
};

/**
 *=====================================================================
 *                   Custom Data: BOSS_BONGO Setting
 *=====================================================================
 */

CameraModeValue cam_param_list76[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 10, 20, 10, 40, 60, 80,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL,
                                           NORMAL1_FLAG_7 | NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list77[] = {
    CAM_FUNCDATA_BATT1(
        -20, 200, 20, 60, 0, 10, 15, 45, 50,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_7 | BATTLE1_FLAG_1), -40, 20),
};

CameraModeValue cam_param_list78[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 10, 20, 10, 80, 60, 80,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL,
                                           NORMAL1_FLAG_7 | NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

/**
 *=====================================================================
 *                 Custom Data: BOSS_MORPHA Setting
 *=====================================================================
 */

CameraModeValue cam_param_list79[] = {
    CAM_FUNCDATA_NORM1(
        -20, 100, 150, -10, 15, 10, 40, 80, 60,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list80[] = {
    CAM_FUNCDATA_BATT1(-20, 200, 10, 80, -10, 10, 25, 70, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -40, 20),
};

/**
 *=====================================================================
 *                  Custom Data: TWINROVA Setting
 *=====================================================================
 */

CameraModeValue cam_param_list81[] = {
    CAM_FUNCDATA_NORM1(
        -20, 150, 300, 0, 20, 10, 40, 60, 80,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list82[] = {
    CAM_FUNCDATA_BATT1(0, 400, 0, 60, -10, 5, 25, 45, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -20, 20),
};

CameraModeValue cam_param_list83[] = {
    CAM_FUNCDATA_NORM1(
        -10, 150, 200, -10, 12, 10, 40, 60, 50,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

/**
 *=====================================================================
 *                Custom Data: BOSS_GANONDORF Setting
 *=====================================================================
 */

CameraModeValue cam_param_list84[] = {
    CAM_FUNCDATA_NORM1(40, 330, 330, -5, 15, 15, 40, 60, 100,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list85[] = {
    CAM_FUNCDATA_BATT4(-40, 250, 0, 2, 80, 20, CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, 0)),
};

/**
 *=====================================================================
 *                 Custom Data: BOSS_GANON Setting
 *=====================================================================
 */

CameraModeValue cam_param_list86[] = {
    CAM_FUNCDATA_NORM1(
        -20, 500, 500, 10, 20, 10, 40, 60, 80,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list87[] = {
    CAM_FUNCDATA_BATT1(-20, 180, 20, 60, 0, 10, 25, 45, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -40, 20),
};

/**
 *=====================================================================
 *                 Custom Data: TOWER_CLIMB Setting
 *=====================================================================
 */

CameraModeValue cam_param_list88[] = {
    CAM_FUNCDATA_NORM2(0, 120, 280, 60, 8, 40, 60, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list89[] = {
    CAM_FUNCDATA_NORM2(0, 120, 280, 60, 8, 40, 60, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL2_FLAG_7)),
};

/**
 *=====================================================================
 *                Custom Data: TOWER_UNUSED Setting
 *=====================================================================
 */

CameraModeValue cam_param_list90[] = {
    CAM_FUNCDATA_NORM2(0, 270, 300, 120, 8, 60, 60, 100,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list91[] = {
    CAM_FUNCDATA_NORM2(0, 270, 300, 120, 6, 60, 60, 100,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

/**
 *=====================================================================
 *                Custom Data: MARKET_BALCONY Setting
 *=====================================================================
 */

CameraModeValue cam_param_list92[] = {
    CAM_FUNCDATA_FIXD1(-40, 100, 60, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list93[] = {
    CAM_FUNCDATA_FIXD1(-40, 100, 60, CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list94[] = {
    CAM_FUNCDATA_FIXD1(-40, 100, 60,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE, 0)),
};

/**
 *=====================================================================
 *                 Custom Data: CHU_BOWLING Setting
 *=====================================================================
 */

CameraModeValue cam_param_list95[] = {
    CAM_FUNCDATA_FIXD1(-40, 25, 60, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

/**
 *=====================================================================
 *               Custom Data: PIVOT_CRAWLSPACE Setting
 *=====================================================================
 */

CameraModeValue cam_param_list96[] = {
    CAM_FUNCDATA_FIXD2(-40, 50, 80, 60, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, FIXED2_FLAG_0)),
};

/**
 *=====================================================================
 *              Custom Data: PIVOT_SHOP_BROWSING Setting
 *=====================================================================
 */

CameraModeValue cam_param_list97[] = {
    CAM_FUNCDATA_DATA4(-40, 60, CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_IGNORE, 0)),
};

/**
 *=====================================================================
 *          Custom Data: PIVOT_IN_FRONT and PIVOT_FROM_SIDE
 *=====================================================================
 */

CameraModeValue cam_param_list98[] = {
    CAM_FUNCDATA_FIXD4(-40, 50, 80, 60, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, FIXED4_FLAG_2)),
};

/**
 *=====================================================================
 *                 Custom Data: No data, all flags off
 *=====================================================================
 */

CameraModeValue cam_param_list99[] = {
    CAM_FUNCDATA_INTERFACE_FIELD(CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

/**
 *=====================================================================
 *                 Custom Data: PREREND_FIXED Setting
 *=====================================================================
 */

CameraModeValue cam_param_list100[] = {
    CAM_FUNCDATA_INTERFACE_FIELD(CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

/**
 *=====================================================================
 *                Custom Data: PREREND_PIVOT Setting
 *=====================================================================
 */

CameraModeValue cam_param_list101[] = {
    CAM_FUNCDATA_UNIQ7(60, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list102[] = {
    CAM_FUNCDATA_UNIQ7(60, CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list103[] = {
    CAM_FUNCDATA_KEEP0(30, 0, 4, CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE, 0)),
};

/**
 *=====================================================================
 *                   Custom Data: DOOR0 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list104[] = {
    CAM_FUNCDATA_UNIQ3(-40, 60, CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, 0)),
};

/**
 *=====================================================================
 *                   Custom Data: DOORC Setting
 *=====================================================================
 */

CameraModeValue cam_param_list105[] = {
    CAM_FUNCDATA_SPEC9(-5, 60,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, SPECIAL9_FLAG_1)),
};

CameraModeValue cam_param_list106[] = {
    CAM_FUNCDATA_SPEC9(
        -5, 60,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, SPECIAL9_FLAG_3 | SPECIAL9_FLAG_1)),
};

/**
 *=====================================================================
 *                   Custom Data: CRAWLSPACE Setting
 *=====================================================================
 */

// subject_camerawork_04 only reads one setting which is used for flags
CameraModeValue cam_param_list107[] = {
    CAM_FUNCDATA_SUBJ4(CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0), 2, 30, 10, 45,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, 0)),
};

/**
 *=====================================================================
 *                   Custom Data: START1 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list108[] = {
    CAM_FUNCDATA_INTERFACE_FIELD(CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, UNIQUE0_FLAG_0)),
};

/**
 *=====================================================================
 *                   Custom Data: FREE0 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list109[] = {
    CAM_FUNCDATA_INTERFACE_FIELD(CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_IGNORE, 0)),
};

/**
 *=====================================================================
 *                   Custom Data: FREE2 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list110[] = {
    CAM_FUNCDATA_INTERFACE_FIELD(CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_IGNORE, UNIQUE6_FLAG_0)),
};

/**
 *=====================================================================
 *                 Custom Data: PIVOT_CORNER Setting
 *=====================================================================
 */

CameraModeValue cam_param_list111[] = {
    CAM_FUNCDATA_FIXD2(-40, 100, 80, 60, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

/**
 *=====================================================================
 *              Custom Data: PIVOT_WATER_SURFACE Setting
 *=====================================================================
 */

CameraModeValue cam_param_list112[] = {
    CAM_FUNCDATA_UNIQ2(-40, 60, 60, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, UNIQUE2_FLAG_1)),
};

CameraModeValue cam_param_list113[] = {
    CAM_FUNCDATA_UNIQ2(-30, 45, 100, CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, UNIQUE2_FLAG_0)),
};

/**
 *=====================================================================
 *               Custom Data: Various cutscene settings
 *=====================================================================
 */

CameraModeValue cam_param_list114[] = {
    CAM_FUNCDATA_INTERFACE_FIELD(CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, 0)),
};

/**
 *=====================================================================
 *                Custom Data: FOREST_BIRDS_EYE Setting
 *=====================================================================
 */

CameraModeValue cam_param_list115[] = {
    CAM_FUNCDATA_PARA1(
        -50, 450, 40, 180, 5, 5, 70, 30,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, PARALLEL1_FLAG_3 | PARALLEL1_FLAG_2), -50, 20),
};

CameraModeValue cam_param_list116[] = {
    CAM_FUNCDATA_INTERFACE_FIELD(
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE, PARALLEL3_FLAG_0)),
};

/**
 *=====================================================================
 *                 Custom Data: SLOW_CHEST_CS Setting
 *=====================================================================
 */

// Also set to but unused by function Demo4
CameraModeValue cam_param_list117[] = {
    CAM_FUNCDATA_DEMO3(60, 30, CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, 0)),
};

/**
 *=====================================================================
 *                     Custom Data: CS_3 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list118[] = {
    CAM_FUNCDATA_INTERFACE_FIELD(
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, DEMO9_FLAG_4 | DEMO9_FLAG_1)),
};

/**
 *=====================================================================
 *                 Custom Data: BEAN_GENERIC Setting
 *=====================================================================
 */

CameraModeValue cam_param_list119[] = {
    CAM_FUNCDATA_NORM1(-50, 300, 300, 50, 20, 10, 50, 70, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1)),
};

CameraModeValue cam_param_list120[] = {
    CAM_FUNCDATA_PARA1(
        -50, 300, 10, 0, 5, 5, 45, 50,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1), -40,
        20),
};

CameraModeValue cam_param_list121[] = {
    CAM_FUNCDATA_JUMP1(-50, 300, 300, 12, 35, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list122[] = {
    CAM_FUNCDATA_UNIQ1(-80, 300, 300, 60, 70, 30, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list123[] = {
    CAM_FUNCDATA_UNIQ1(-120, 300, 300, 70, 50, 30,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list124[] = {
    CAM_FUNCDATA_NORM1(-20, 300, 350, 50, 100, 10, 100, 70, 30,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1)),
};

/**
 *=====================================================================
 *                Custom Data: BEAN_LOST_WOODS Setting
 *=====================================================================
 */

CameraModeValue cam_param_list125[] = {
    CAM_FUNCDATA_NORM1(-50, 200, 200, 20, 16, 10, 50, 60, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1)),
};

CameraModeValue cam_param_list126[] = {
    CAM_FUNCDATA_PARA1(
        -50, 200, 40, 0, 5, 5, 45, 50,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1), -40,
        20),
};

CameraModeValue cam_param_list127[] = {
    CAM_FUNCDATA_JUMP1(-50, 150, 250, 12, 35, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list128[] = {
    CAM_FUNCDATA_UNIQ1(-80, 200, 200, 40, 60, 30, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list129[] = {
    CAM_FUNCDATA_UNIQ1(-120, 200, 200, 60, 50, 30,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list130[] = {
    CAM_FUNCDATA_NORM1(-20, 200, 250, 20, 100, 10, 100, 60, 30,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1)),
};

/**
 *=====================================================================
 *                 Custom Data: SCENE_UNUSED Setting
 *=====================================================================
 */

CameraModeValue cam_param_list131[] = {
    CAM_FUNCDATA_SPEC9(
        -30, 60,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_NOTHING, SPECIAL9_FLAG_3 | SPECIAL9_FLAG_1)),
};

/**
 *=====================================================================
 *                Custom Data: SCENE_TRANSITION Setting
 *=====================================================================
 */

CameraModeValue cam_param_list132[] = {
    CAM_FUNCDATA_UNIQ2(-20, 150, 60,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_NOTHING_ALT, UNIQUE2_FLAG_4)),
};

/**
 *=====================================================================
 *                   Custom Data: BIG_OCTO Setting
 *=====================================================================
 */

CameraModeValue cam_param_list133[] = {
    CAM_FUNCDATA_NORM1(
        0, 400, 500, 35, 14, 5, 20, 60, 40,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_4 | NORMAL1_FLAG_1)),
};

CameraModeValue cam_param_list134[] = {
    CAM_FUNCDATA_BATT1(-20, 250, 5, 10, 30, 20, 25, 45, 60,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -40, 25),
};

CameraModeValue cam_param_list135[] = {
    CAM_FUNCDATA_NORM1(
        0, 300, 500, 60, 8, 5, 60, 60, 30,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_4 | NORMAL1_FLAG_1)),
};

/**
 *=====================================================================
 *               Custom Data: MEADOW_BIRDS_EYE Setting
 *=====================================================================
 */

CameraModeValue cam_param_list136[] = {
    CAM_FUNCDATA_NORM1(
        -20, 500, 500, 80, 20, 10, 70, 70, 80,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_4 | NORMAL1_FLAG_1)),
};

CameraModeValue cam_param_list137[] = {
    CAM_FUNCDATA_PARA1(-20, 500, 80, 0, 5, 5, 70, 80,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL,
                                           PARALLEL1_FLAG_4 | PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1),
                       -40, 40),
};

CameraModeValue cam_param_list138[] = {
    CAM_FUNCDATA_PARA1(-20, 500, 80, 0, 5, 5, 60, 80,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL,
                                           PARALLEL1_FLAG_4 | PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1),
                       -40, 40),
};

CameraModeValue cam_param_list139[] = {
    CAM_FUNCDATA_NORM1(
        -20, 500, 500, 80, 20, 10, 80, 60, 20,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_4 | NORMAL1_FLAG_1)),
};

/**
 *=====================================================================
 *                 Custom Data: MEADOW_UNUSED Setting
 *=====================================================================
 */

CameraModeValue cam_param_list140[] = {
    CAM_FUNCDATA_NORM1(
        -20, 750, 750, 80, 20, 10, 70, 70, 80,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_4 | NORMAL1_FLAG_1)),
};

CameraModeValue cam_param_list141[] = {
    CAM_FUNCDATA_PARA1(-20, 750, 80, 0, 5, 5, 70, 80,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL,
                                           PARALLEL1_FLAG_4 | PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1),
                       -40, 40),
};

CameraModeValue cam_param_list142[] = {
    CAM_FUNCDATA_PARA1(
        -20, 750, 80, 0, 5, 5, 70, 80,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1), -40,
        40),
};

CameraModeValue cam_param_list143[] = {
    CAM_FUNCDATA_NORM1(
        -20, 750, 750, 80, 20, 10, 80, 70, 20,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_4 | NORMAL1_FLAG_1)),
};

/**
 *=====================================================================
 *                Custom Data: FIRE_BIRDS_EYE Setting
 *=====================================================================
 */

CameraModeValue cam_param_list144[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 80, 20, 10, 70, 70, 80,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1)),
};

CameraModeValue cam_param_list145[] = {
    CAM_FUNCDATA_PARA1(
        -20, 500, 80, 0, 5, 5, 70, 80,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1), -40,
        40),
};

CameraModeValue cam_param_list146[] = {
    CAM_FUNCDATA_PARA1(
        -20, 500, 80, 0, 5, 5, 60, 80,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1), -40,
        40),
};

CameraModeValue cam_param_list147[] = {
    CAM_FUNCDATA_NORM1(-20, 500, 500, 80, 20, 10, 80, 60, 20,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1)),
};

/**
 *=====================================================================
 *                Custom Data: TURN_AROUND Setting
 *=====================================================================
 */

CameraModeValue cam_param_list148[] = {
    CAM_FUNCDATA_KEEP4(
        -30, 120, -10, 170, 0, 60,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE, KEEPON4_FLAG_1), 25, 6),
};

/**
 *=====================================================================
 *                Custom Data: PIVOT_VERTICAL Setting
 *=====================================================================
 */

CameraModeValue cam_param_list149[] = {
    CAM_FUNCDATA_SPEC0(20, CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, 0)),
};

/**
 *=====================================================================
 *                 Custom Data: NORMAL2/4 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list150[] = {
    CAM_FUNCDATA_NORM1(-20, 200, 300, 10, 12, 10, 35, 60, 60,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1)),
};

/**
 *=====================================================================
 *                   Custom Data: FISHING Setting
 *=====================================================================
 */

CameraModeValue cam_param_list151[] = {
    CAM_FUNCDATA_NORM1(0, 200, 300, 20, 12, 10, 35, 55, 60,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_IGNORE, NORMAL1_FLAG_1)),
};

CameraModeValue cam_param_list152[] = {
    CAM_FUNCDATA_PARA1(
        -20, 250, 0, 0, 5, 5, 45, 50,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_IGNORE, PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1), -40,
        20),
};

CameraModeValue cam_param_list153[] = {
    CAM_FUNCDATA_BATT1(-20, 250, 0, 80, 0, 0, 25, 55, 80,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_IGNORE, BATTLE1_FLAG_1), -40, 25),
};

CameraModeValue cam_param_list154[] = {
    CAM_FUNCDATA_KEEP3(-30, 70, 200, 40, 10, 0, 5, 70, 45, 50, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_IGNORE, KEEPON3_FLAG_5)),
};

CameraModeValue cam_param_list155[] = {
    CAM_FUNCDATA_SUBJ3(0, 5, 50, 10, 0, 0, 0, 45,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_IGNORE, 0)),
};

CameraModeValue cam_param_list156[] = {
    CAM_FUNCDATA_JUMP1(-20, 200, 300, 12, 35, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_IGNORE, 0)),
};

CameraModeValue cam_param_list157[] = {
    CAM_FUNCDATA_JUMP1(-20, 200, 300, 15, 80, 60, 20,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_IGNORE, 0)),
};

CameraModeValue cam_param_list158[] = {
    CAM_FUNCDATA_UNIQ1(-80, 200, 300, 40, 60, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_IGNORE, 0)),
};

CameraModeValue cam_param_list159[] = {
    CAM_FUNCDATA_UNIQ1(-120, 300, 300, 70, 45, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_IGNORE, 0)),
};

/**
 *=====================================================================
 *                   Custom Data: CS_C Setting
 *=====================================================================
 */

CameraModeValue cam_param_list160[] = {
    CAM_FUNCDATA_INTERFACE_FIELD(CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_IGNORE, 0)),
};

/**
 *=====================================================================
 *                   Custom Data: JABU_TENTACLE Setting
 *=====================================================================
 */

CameraModeValue cam_param_list161[] = {
    CAM_FUNCDATA_NORM1_ALT(
        30, 200, 300, -20, 15, 5, 50, 70, 70,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list162[] = {
    CAM_FUNCDATA_BATT1(-30, 160, 10, 10, 0, 0, 70, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -50, 20),
};

/**
 *=====================================================================
 *                   Custom Data: DUNGEON2 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list163[] = {
    CAM_FUNCDATA_NORM1(
        -20, 350, 350, 20, 15, 5, 30, 60, 60,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list164[] = {
    CAM_FUNCDATA_PARA1(
        -20, 200, 0, 0, 5, 5, 45, 50,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1), -40,
        20),
};

CameraModeValue cam_param_list165[] = {
    CAM_FUNCDATA_BATT1(-20, 180, 10, 80, -10, 10, 25, 45, 80,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, BATTLE1_FLAG_1), -40, 25),
};

CameraModeValue cam_param_list166[] = {
    CAM_FUNCDATA_JUMP1(-20, 350, 350, 10, 50, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list167[] = {
    CAM_FUNCDATA_JUMP1(-20, 350, 350, 15, 80, 60, 20,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list168[] = {
    CAM_FUNCDATA_JUMP2(-40, 350, 350, 20, 5, 5, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list169[] = {
    CAM_FUNCDATA_JUMP2(-40, 350, 350, 20, 999, 5, 60, 40,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, JUMP2_FLAG_2 | JUMP2_FLAG_1)),
};

CameraModeValue cam_param_list170[] = {
    CAM_FUNCDATA_UNIQ1(-50, 350, 350, 40, 60, 10, CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list171[] = {
    CAM_FUNCDATA_UNIQ1(-100, 350, 350, 70, 45, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, 0)),
};

CameraModeValue cam_param_list172[] = {
    CAM_FUNCDATA_NORM1(
        -10, 350, 350, 20, 100, 5, 100, 60, 5,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1 | NORMAL1_FLAG_0)),
};

CameraModeValue cam_param_list173[] = {
    CAM_FUNCDATA_PARA1(0, 280, 25, 0, 5, 5, 70, 30,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL,
                                           PARALLEL1_FLAG_6 | PARALLEL1_FLAG_5 | PARALLEL1_FLAG_3 | PARALLEL1_FLAG_1),
                       -20, 30),
};

/**
 *=====================================================================
 *                 Custom Data: DIRECTED_YAW Setting
 *=====================================================================
 */

CameraModeValue cam_param_list174[] = {
    CAM_FUNCDATA_NORM1(-10, 280, 320, -8, 20, 10, 80, 60, 80,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, NORMAL1_FLAG_1)),
};

CameraModeValue cam_param_list175[] = {
    CAM_FUNCDATA_KEEP1(-20, 180, 200, 35, 45, 0, -5, 20, 50, 50,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, KEEPON1_FLAG_0), -50, 30),
};

CameraModeValue cam_param_list176[] = {
    CAM_FUNCDATA_KEEP3(
        -80, 200, 250, 30, 10, -8, -8, 30, 50, 50, 10,
        CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE, KEEPON3_FLAG_5)),
};

/**
 *=====================================================================
 *                   Custom Data: NORMAL4 Setting
 *=====================================================================
 */

CameraModeValue cam_param_list177[] = {
    CAM_FUNCDATA_KEEP3(-30, 70, 200, 40, 10, 0, 5, 70, 45, 50, 10,
                       CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE,
                                           KEEPON3_FLAG_7 | KEEPON3_FLAG_5)),
};

/**
 * =====================================================================
 *    CAMERA SETTINGS: USAGE OF FUNCTIONS AND DATA FOR SPECIFIC MODES
 * =====================================================================
 */

#define CAM_SETTING_MODE_ENTRY(func, data) \
    { func, ARRAY_COUNT(data), data }

CameraMode cam_mode_tbl1[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list0),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),         // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),   // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),              // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list4), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),       // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),          // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),              // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),       // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),      // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),          // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),              // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),         // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),        // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list16),          // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),            // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),             // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),          // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),   // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl2[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list21),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list22),         // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list23),   // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),              // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list24), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list28),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),       // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),          // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),              // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list25),       // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),      // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),          // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list29),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list26),              // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list31),         // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list32),        // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list27),          // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list30),            // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list33),             // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),          // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),   // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl3[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list34),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list35),         // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list36),   // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),               // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list37), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list40),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),        // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),           // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),               // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list25),        // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),       // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),           // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list41),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list38),              // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list43),         // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list44),        // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list39),          // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list42),            // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list45),             // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),           // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),    // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl4[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list46),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list35),         // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list23),    // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list47),              // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list37), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list50),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),        // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),           // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),               // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list25),        // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),       // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),           // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list51),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list48),              // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list53),         // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list54),        // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list49),          // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list52),            // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list55),             // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list56),          // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),    // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl5[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP3, cam_param_list57),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list58),         // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),   // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list59),              // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list4), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),       // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),          // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),              // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),       // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP3, cam_param_list60),      // CAM_MODE_AIM_BOOMERANG
    { CAM_FUNC_NONE, 0, NULL },                                                   // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),              // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),         // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),        // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),              // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),            // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),             // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),          // CAM_MODE_PUSH_PULL
};

CameraMode cam_mode_tbl6[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM3, cam_param_list61),          // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM3, cam_param_list62),       // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list64), // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list65),            // CAM_MODE_TALK
    { CAM_FUNC_NONE, 0, NULL },                                               // CAM_MODE_Z_TARGET_UNFRIENDLY
    { CAM_FUNC_NONE, 0, NULL },                                               // CAM_MODE_WALL_CLIMB
    { CAM_FUNC_NONE, 0, NULL },                                               // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list63),        // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),          // CAM_MODE_Z_AIM
};

CameraMode cam_mode_tbl7[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list66),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),           // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),     // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list67), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),           // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),         // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),            // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),         // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),        // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),            // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),          // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),           // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),          // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list42),             // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),               // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),            // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),     // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl8[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list68),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),             // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),       // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                  // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list69), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),             // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),           // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),              // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                  // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),           // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),          // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),              // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),            // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                  // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),             // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),            // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                  // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),                // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),                 // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),              // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),       // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl9[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list70),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),              // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),        // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                   // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list71), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),              // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),            // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),               // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                   // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),            // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),           // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),               // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),             // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                   // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),              // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),             // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                   // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),                 // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),                  // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),               // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),        // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl10[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list72), // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),       // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2), // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),            // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1,
                           cam_param_list73),      // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),       // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),     // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),        // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),            // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),     // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),    // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),        // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),      // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),            // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),       // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),      // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),            // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),          // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),           // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),        // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20), // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl11[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list74),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),              // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),        // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                   // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list75), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),              // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),            // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),               // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                   // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),            // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),           // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),               // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),             // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                   // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),              // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),             // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                   // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),                 // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),                  // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),               // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),        // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl12[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list76),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),           // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),     // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list77), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),           // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),         // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),            // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),         // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),        // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),            // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),          // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list78),              // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),           // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),          // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list78),              // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),              // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),               // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),            // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),     // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl13[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list79),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),            // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),      // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                 // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list80), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),            // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),          // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),             // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                 // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),          // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),         // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),             // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),           // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                 // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),            // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),           // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                 // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),               // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),                // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),             // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),      // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl14[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list81),    // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),              // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),        // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                   // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list82), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),              // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),            // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),               // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                   // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),            // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),           // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),               // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),             // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                   // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),              // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),             // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                   // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),                 // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),                  // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),               // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),        // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl15[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list83),       // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),              // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),        // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                   // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list82), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),              // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),            // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),               // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                   // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),            // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),           // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),               // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),             // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                   // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),              // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),             // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                   // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),                 // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),                  // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),               // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),        // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl16[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list84),    // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),       // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2), // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),            // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1,
                           cam_param_list73),      // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),       // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),     // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),        // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),            // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),     // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),    // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),        // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),      // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),            // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),       // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),      // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),            // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list85),    // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),           // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),        // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20), // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl17[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list86),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),           // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),     // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list87), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),           // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),         // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),            // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),         // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),        // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),            // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),          // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),           // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),          // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),              // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),               // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),            // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),     // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl18[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM2, cam_param_list88),         // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),         // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),   // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),              // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list4), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),       // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),          // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),              // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),       // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),      // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),          // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM2, cam_param_list89),           // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),         // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),        // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),              // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),            // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),             // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),          // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),   // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl19[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM2, cam_param_list90),        // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),         // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),   // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),              // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list4), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),       // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),          // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),              // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),       // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),      // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),          // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM2, cam_param_list91),          // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),         // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),        // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),              // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),            // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),             // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),          // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),   // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl20[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD1, cam_param_list92),          // CAM_MODE_NORMAL
    { CAM_FUNC_NONE, 0, NULL },                                                       // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD1, cam_param_list93), // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD1, cam_param_list94),            // CAM_MODE_TALK
};

CameraMode cam_mode_tbl21[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD1, cam_param_list95), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl22[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD2, cam_param_list96), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl23[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DATA4, cam_param_list97), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl24[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD4, cam_param_list98), // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),              // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),        // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                   // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list24),      // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),              // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),            // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),               // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                   // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),            // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),           // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),               // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),             // CAM_MODE_Z_WALL_CLIMB
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_JUMP
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_LEDGE_HANG
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_Z_LEDGE_HANG
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),                 // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),                  // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),               // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),        // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl25[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD3, cam_param_list99),                      // CAM_MODE_NORMAL
    { CAM_FUNC_NONE, 0, NULL },                                                      // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD3, cam_param_list100), // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD3, cam_param_list100), // CAM_MODE_TALK
};

CameraMode cam_mode_tbl26[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ7, cam_param_list101),          // CAM_MODE_NORMAL
    { CAM_FUNC_NONE, 0, NULL },                                                      // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ7, cam_param_list102), // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP0, cam_param_list103),            // CAM_MODE_TALK
};

CameraMode cam_mode_tbl27[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC6, cam_param_list99), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl28[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ3, cam_param_list104), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl29[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC9, cam_param_list105),    // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC9, cam_param_list106), // CAM_MODE_Z_PARALLEL
};

CameraMode cam_mode_tbl30[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ4, cam_param_list107), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl31[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ0, cam_param_list99), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl32[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ0, cam_param_list108), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl33[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ6, cam_param_list109), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl34[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ6, cam_param_list110), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl35[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD2, cam_param_list111), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl36[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ2, cam_param_list112),    // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ2, cam_param_list113), // CAM_MODE_Z_PARALLEL
};

CameraMode cam_mode_tbl37[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO1, cam_param_list114), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl38[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO2, cam_param_list114), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl39[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list115), // CAM_MODE_NORMAL
    { CAM_FUNC_NONE, 0, NULL },                                               // CAM_MODE_Z_PARALLEL
    { CAM_FUNC_NONE, 0, NULL },                                               // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA3, cam_param_list116),   // CAM_MODE_TALK
};

CameraMode cam_mode_tbl40[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO3, cam_param_list117), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl41[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO4, cam_param_list117), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl42[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO9, cam_param_list118), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl43[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO5, cam_param_list114), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl44[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list119),        // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list120),     // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),   // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),              // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list24), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),       // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),          // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),              // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),       // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),      // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),          // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list121),          // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list122),     // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list123),    // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),              // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),            // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list124),         // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),          // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),   // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl45[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list125),      // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list126),   // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),   // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),              // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list24), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),       // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),          // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),              // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),       // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),      // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),          // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list127),        // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list128),   // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list129),  // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),              // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),            // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list130),       // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),          // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),   // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl46[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC9, cam_param_list131), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl47[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ2, cam_param_list132), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl48[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC7, cam_param_list99), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl49[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC4, cam_param_list114), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl50[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ5, cam_param_list114), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl51[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_DEMO6, cam_param_list114), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl52[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list133),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list35),        // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list36),  // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),              // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list134), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list40),        // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),       // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),          // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),              // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list25),       // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),      // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),          // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list41),       // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list38),             // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list43),        // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list44),       // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list39),         // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list42),           // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list135),             // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),          // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),   // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl53[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list136),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list137),         // CAM_MODE_Z_PARALLEL
    { CAM_FUNC_NONE, 0, NULL },                                                          // CAM_MODE_Z_TARGET_FRIENDLY
    { CAM_FUNC_NONE, 0, NULL },                                                          // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list138), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list139),         // CAM_MODE_WALL_CLIMB
};

CameraMode cam_mode_tbl54[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list140),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list141),         // CAM_MODE_Z_PARALLEL
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_Z_TARGET_FRIENDLY
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list142), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list143),         // CAM_MODE_WALL_CLIMB
};

CameraMode cam_mode_tbl55[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list144),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list145),         // CAM_MODE_Z_PARALLEL
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_Z_TARGET_FRIENDLY
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list146), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list147),         // CAM_MODE_WALL_CLIMB
};

CameraMode cam_mode_tbl56[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP4, cam_param_list148), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl57[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC0, cam_param_list149), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl58[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list150),        // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),         // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),   // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),              // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list24), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),       // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),          // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),              // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),       // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),      // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),          // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),              // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),         // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),        // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),              // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),            // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),             // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),          // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),   // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl59[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list151),          // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list152),       // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list153), // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list154),            // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list153), // CAM_MODE_Z_TARGET_UNFRIENDLY
    { CAM_FUNC_NONE, 0, NULL },                                                 // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list155),     // CAM_MODE_FIRST_PERSON
    { CAM_FUNC_NONE, 0, NULL },                                                 // CAM_MODE_AIM_ADULT
    { CAM_FUNC_NONE, 0, NULL },                                                 // CAM_MODE_Z_AIM
    { CAM_FUNC_NONE, 0, NULL },                                                 // CAM_MODE_HOOKSHOT_FLY
    { CAM_FUNC_NONE, 0, NULL },                                                 // CAM_MODE_AIM_BOOMERANG
    { CAM_FUNC_NONE, 0, NULL },                                                 // CAM_MODE_AIM_CHILD
    { CAM_FUNC_NONE, 0, NULL },                                                 // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list156),            // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list158),       // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list159),      // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list157),        // CAM_MODE_FREE_FALL
};

CameraMode cam_mode_tbl60[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ9, cam_param_list160), // CAM_MODE_NORMAL
};

CameraMode cam_mode_tbl61[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list161),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),              // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),        // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                   // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list162), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),              // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),            // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),               // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                   // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),            // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),           // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),               // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),             // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                   // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),              // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),             // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                   // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),                 // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),                  // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),               // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),        // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl62[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list163),            // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list164),         // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list36),   // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),               // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list165), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list168),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),        // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),           // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),               // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list25),        // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),       // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),           // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list169),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list166),              // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list170),         // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list171),        // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list167),          // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),             // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list172),             // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list173),          // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),    // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl63[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list174),          // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),           // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list175), // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list176),            // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list4),   // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),           // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),         // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),            // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),                // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),         // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),        // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),            // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),          // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),           // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),          // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),                // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),              // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),               // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),            // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),     // CAM_MODE_FOLLOW_BOOMERANG
};

CameraMode cam_mode_tbl64[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD4, cam_param_list98), // CAM_MODE_NORMAL
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_FIXD4, cam_param_list98), // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list3),                   // CAM_MODE_TALK
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_Z_TARGET_UNFRIENDLY
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),            // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),               // CAM_MODE_AIM_ADULT
    { CAM_FUNC_NONE, 0, NULL },                                                        // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),            // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),           // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),               // CAM_MODE_AIM_CHILD
};

CameraMode cam_mode_tbl65[] = {
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list150),        // CAM_MODE_NORMAL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list1),         // CAM_MODE_Z_PARALLEL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list2),   // CAM_MODE_Z_TARGET_FRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP3, cam_param_list177),              // CAM_MODE_TALK
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT1, cam_param_list24), // CAM_MODE_Z_TARGET_UNFRIENDLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list5),         // CAM_MODE_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list6),       // CAM_MODE_FIRST_PERSON
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list7),          // CAM_MODE_AIM_ADULT
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list8),              // CAM_MODE_Z_AIM
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SPEC5, cam_param_list9),       // CAM_MODE_HOOKSHOT_FLY
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list10),      // CAM_MODE_AIM_BOOMERANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_SUBJ3, cam_param_list11),          // CAM_MODE_AIM_CHILD
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP2, cam_param_list12),        // CAM_MODE_Z_WALL_CLIMB
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),              // CAM_MODE_JUMP
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list14),         // CAM_MODE_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_UNIQ1, cam_param_list15),        // CAM_MODE_Z_LEDGE_HANG
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_JUMP1, cam_param_list13),              // CAM_MODE_FREE_FALL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_BATT4, cam_param_list17),            // CAM_MODE_CHARGE
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_NORM1, cam_param_list18),             // CAM_MODE_STILL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_PARA1, cam_param_list19),          // CAM_MODE_PUSH_PULL
    CAM_SETTING_MODE_ENTRY(CAM_FUNC_KEEP1, cam_param_list20),   // CAM_MODE_FOLLOW_BOOMERANG
};

CameraSetting cam_ctl_tbl_0[] = {
    { { 0x00000000 }, NULL },                             // CAM_SET_NONE
    { { 0x051FFFFF }, cam_mode_tbl1 },              // CAM_SET_NORMAL0
    { { 0x051FFFFF }, cam_mode_tbl2 },              // CAM_SET_NORMAL1
    { { 0x051FFFFF }, cam_mode_tbl3 },             // CAM_SET_DUNGEON0
    { { 0x051FFFFF }, cam_mode_tbl4 },             // CAM_SET_DUNGEON1
    { { 0x050FF7FF }, cam_mode_tbl5 },              // CAM_SET_NORMAL3
    { { 0x8500018F }, cam_mode_tbl6 },                // CAM_SET_HORSE
    { { 0x051FFFFF }, cam_mode_tbl7 },            // CAM_SET_BOSS_GOHMA
    { { 0x051FFFFF }, cam_mode_tbl8 },          // CAM_SET_BOSS_DODONGO
    { { 0x051FFFFF }, cam_mode_tbl9 },         // CAM_SET_BOSS_BARINADE
    { { 0x051FFFFF }, cam_mode_tbl10 },     // CAM_SET_BOSS_PHANTOM_GANON
    { { 0x051FFFFF }, cam_mode_tbl11 },         // CAM_SET_BOSS_VOLVAGIA
    { { 0x051FFFFF }, cam_mode_tbl12 },            // CAM_SET_BOSS_BONGO
    { { 0x051FFFFF }, cam_mode_tbl13 },           // CAM_SET_BOSS_MORPHA
    { { 0x051FFFFF }, cam_mode_tbl14 }, // CAM_SET_BOSS_TWINROVA_PLATFORM
    { { 0x051FFFFF }, cam_mode_tbl15 },    // CAM_SET_BOSS_TWINROVA_FLOOR
    { { 0x051FFFFF }, cam_mode_tbl16 },        // CAM_SET_BOSS_GANONDORF
    { { 0x051FFFFF }, cam_mode_tbl17 },            // CAM_SET_BOSS_GANON
    { { 0x851FFFFF }, cam_mode_tbl18 },           // CAM_SET_TOWER_CLIMB
    { { 0x851FFFFF }, cam_mode_tbl19 },          // CAM_SET_TOWER_UNUSED
    { { 0x8500000D }, cam_mode_tbl20 },        // CAM_SET_MARKET_BALCONY
    { { 0x85000001 }, cam_mode_tbl21 },           // CAM_SET_CHU_BOWLING
    { { 0x85000001 }, cam_mode_tbl22 },      // CAM_SET_PIVOT_CRAWLSPACE
    { { 0x85000001 }, cam_mode_tbl23 },    // CAM_SET_PIVOT_SHOP_BROWSING
    { { 0x851E1FFF }, cam_mode_tbl24 },         // CAM_SET_PIVOT_IN_FRONT
    { { 0x8C00000D }, cam_mode_tbl25 },         // CAM_SET_PREREND_FIXED
    { { 0x8C00000D }, cam_mode_tbl26 },         // CAM_SET_PREREND_PIVOT
    { { 0x8C000001 }, cam_mode_tbl27 },    // CAM_SET_PREREND_SIDE_SCROLL
    { { 0xC5000001 }, cam_mode_tbl28 },                // CAM_SET_DOOR0
    { { 0xC5000003 }, cam_mode_tbl29 },                // CAM_SET_DOORC
    { { 0xC5000001 }, cam_mode_tbl30 },           // CAM_SET_CRAWLSPACE
    { { 0xC5000001 }, cam_mode_tbl31 },               // CAM_SET_START0
    { { 0xC5000001 }, cam_mode_tbl32 },               // CAM_SET_START1
    { { 0x05000001 }, cam_mode_tbl33 },                // CAM_SET_FREE0
    { { 0x05000001 }, cam_mode_tbl34 },                // CAM_SET_FREE2
    { { 0x85000001 }, cam_mode_tbl35 },          // CAM_SET_PIVOT_CORNER
    { { 0x05000003 }, cam_mode_tbl36 },    // CAM_SET_PIVOT_WATER_SURFACE
    { { 0xCE000001 }, cam_mode_tbl37 },                  // CAM_SET_CS_0
    { { 0x4E000001 }, cam_mode_tbl38 },     // CAM_SET_CS_TWISTED_HALLWAY
    { { 0x05000009 }, cam_mode_tbl39 },       // CAM_SET_FOREST_BIRDS_EYE
    { { 0x45000001 }, cam_mode_tbl40 },          // CAM_SET_SLOW_CHEST_CS
    { { 0x45000001 }, cam_mode_tbl41 },           // CAM_SET_ITEM_UNUSED
    { { 0x45000001 }, cam_mode_tbl42 },                  // CAM_SET_CS_3
    { { 0x45000001 }, cam_mode_tbl43 },          // CAM_SET_CS_ATTENTION
    { { 0x451FFFFF }, cam_mode_tbl44 },          // CAM_SET_BEAN_GENERIC
    { { 0x451FFFFF }, cam_mode_tbl45 },        // CAM_SET_BEAN_LOST_WOODS
    { { 0xC5000001 }, cam_mode_tbl46 },          // CAM_SET_SCENE_UNUSED
    { { 0x45000001 }, cam_mode_tbl47 },      // CAM_SET_SCENE_TRANSITION
    { { 0x05000001 }, cam_mode_tbl48 },     // CAM_SET_ELEVATOR_PLATFORM
    { { 0x45000001 }, cam_mode_tbl49 },        // CAM_SET_FIRE_STAIRCASE
    { { 0x45000001 }, cam_mode_tbl50 },         // CAM_SET_FOREST_UNUSED
    { { 0x45000001 }, cam_mode_tbl51 },      // CAM_SET_FOREST_DEFEAT_POE
    { { 0x451FFFFF }, cam_mode_tbl52 },              // CAM_SET_BIG_OCTO
    { { 0x05000033 }, cam_mode_tbl53 },       // CAM_SET_MEADOW_BIRDS_EYE
    { { 0x05000033 }, cam_mode_tbl54 },         // CAM_SET_MEADOW_UNUSED
    { { 0x05000033 }, cam_mode_tbl55 },         // CAM_SET_FIRE_BIRDS_EYE
    { { 0x4A000001 }, cam_mode_tbl56 },           // CAM_SET_TURN_AROUND
    { { 0x05000001 }, cam_mode_tbl57 },        // CAM_SET_PIVOT_VERTICAL
    { { 0x051FFFFF }, cam_mode_tbl58 },                // CAM_SET_NORMAL2
    { { 0x0501E05F }, cam_mode_tbl59 },              // CAM_SET_FISHING
    { { 0x45000001 }, cam_mode_tbl60 },                  // CAM_SET_CS_C
    { { 0x051FFFFF }, cam_mode_tbl61 },         // CAM_SET_JABU_TENTACLE
    { { 0x051FFFFF }, cam_mode_tbl62 },             // CAM_SET_DUNGEON2
    { { 0x051FFFFF }, cam_mode_tbl63 },          // CAM_SET_DIRECTED_YAW
    { { 0xC5000ECD }, cam_mode_tbl64 },        // CAM_SET_PIVOT_FROM_SIDE
    { { 0x051FFFFF }, cam_mode_tbl65 },              // CAM_SET_NORMAL4
};

s32 normal_camerawork_00(Camera* camera);
s32 normal_camerawork_01(Camera* camera);
s32 normal_camerawork_02(Camera* camera);
s32 normal_camerawork_03(Camera* camera);
s32 normal_camerawork_04(Camera* camera);
s32 parallel_camerawork_00(Camera* camera);
s32 parallel_camerawork_01(Camera* camera);
s32 parallel_camerawork_02(Camera* camera);
s32 parallel_camerawork_03(Camera* camera);
s32 parallel_camerawork_04(Camera* camera);
s32 keepon_camerawork_00(Camera* camera);
s32 keepon_camerawork_01(Camera* camera);
s32 keepon_camerawork_02(Camera* camera);
s32 keepon_camerawork_03(Camera* camera);
s32 keepon_camerawork_04(Camera* camera);
s32 subject_camerawork_00(Camera* camera);
s32 subject_camerawork_01(Camera* camera);
s32 subject_camerawork_02(Camera* camera);
s32 subject_camerawork_03(Camera* camera);
s32 subject_camerawork_04(Camera* camera);
s32 jump_camerawork_00(Camera* camera);
s32 jump_camerawork_01(Camera* camera);
s32 jump_camerawork_02(Camera* camera);
s32 jump_camerawork_03(Camera* camera);
s32 jump_camerawork_04(Camera* camera);
s32 battle_camerawork_00(Camera* camera);
s32 battle_camerawork_01(Camera* camera);
s32 battle_camerawork_02(Camera* camera);
s32 battle_camerawork_03(Camera* camera);
s32 battle_camerawork_04(Camera* camera);
s32 fixed_camerawork_00(Camera* camera);
s32 fixed_camerawork_01(Camera* camera);
s32 fixed_camerawork_02(Camera* camera);
s32 fixed_camerawork_03(Camera* camera);
s32 fixed_camerawork_04(Camera* camera);
s32 data_camerawork_00(Camera* camera);
s32 data_camerawork_01(Camera* camera);
s32 data_camerawork_02(Camera* camera);
s32 data_camerawork_03(Camera* camera);
s32 data_camerawork_04(Camera* camera);
s32 unique_camerawork_00(Camera* camera);
s32 unique_camerawork_01(Camera* camera);
s32 unique_camerawork_02(Camera* camera);
s32 unique_camerawork_03(Camera* camera);
s32 unique_camerawork_04(Camera* camera);
s32 unique_camerawork_05(Camera* camera);
s32 unique_camerawork_06(Camera* camera);
s32 unique_camerawork_07(Camera* camera);
s32 unique_camerawork_08(Camera* camera);
s32 unique_camerawork_09(Camera* camera);
s32 demo_camerawork_00(Camera* camera);
s32 demo_camerawork_01(Camera* camera);
s32 demo_camerawork_02(Camera* camera);
s32 demo_camerawork_03(Camera* camera);
s32 demo_camerawork_04(Camera* camera);
s32 demo_camerawork_05(Camera* camera);
s32 demo_camerawork_06(Camera* camera);
s32 demo_camerawork_07(Camera* camera);
s32 demo_camerawork_08(Camera* camera);
s32 demo_camerawork_09(Camera* camera);
s32 special_camerawork_00(Camera* camera);
s32 special_camerawork_01(Camera* camera);
s32 special_camerawork_02(Camera* camera);
s32 special_camerawork_03(Camera* camera);
s32 special_camerawork_04(Camera* camera);
s32 special_camerawork_05(Camera* camera);
s32 special_camerawork_06(Camera* camera);
s32 special_camerawork_07(Camera* camera);
s32 special_camerawork_08(Camera* camera);
s32 special_camerawork_09(Camera* camera);

s32 (*cam_func_tbl_0[])(Camera*) = {
    NULL,             // CAM_FUNC_NONE
    normal_camerawork_00,   // CAM_FUNC_NORM0
    normal_camerawork_01,   // CAM_FUNC_NORM1
    normal_camerawork_02,   // CAM_FUNC_NORM2
    normal_camerawork_03,   // CAM_FUNC_NORM3
    normal_camerawork_04,   // CAM_FUNC_NORM4
    parallel_camerawork_00, // CAM_FUNC_PARA0
    parallel_camerawork_01, // CAM_FUNC_PARA1
    parallel_camerawork_02, // CAM_FUNC_PARA2
    parallel_camerawork_03, // CAM_FUNC_PARA3
    parallel_camerawork_04, // CAM_FUNC_PARA4
    keepon_camerawork_00,   // CAM_FUNC_KEEP0
    keepon_camerawork_01,   // CAM_FUNC_KEEP1
    keepon_camerawork_02,   // CAM_FUNC_KEEP2
    keepon_camerawork_03,   // CAM_FUNC_KEEP3
    keepon_camerawork_04,   // CAM_FUNC_KEEP4
    subject_camerawork_00,     // CAM_FUNC_SUBJ0
    subject_camerawork_01,     // CAM_FUNC_SUBJ1
    subject_camerawork_02,     // CAM_FUNC_SUBJ2
    subject_camerawork_03,     // CAM_FUNC_SUBJ3
    subject_camerawork_04,     // CAM_FUNC_SUBJ4
    jump_camerawork_00,     // CAM_FUNC_JUMP0
    jump_camerawork_01,     // CAM_FUNC_JUMP1
    jump_camerawork_02,     // CAM_FUNC_JUMP2
    jump_camerawork_03,     // CAM_FUNC_JUMP3
    jump_camerawork_04,     // CAM_FUNC_JUMP4
    battle_camerawork_00,   // CAM_FUNC_BATT0
    battle_camerawork_01,   // CAM_FUNC_BATT1
    battle_camerawork_02,   // CAM_FUNC_BATT2
    battle_camerawork_03,   // CAM_FUNC_BATT3
    battle_camerawork_04,   // CAM_FUNC_BATT4
    fixed_camerawork_00,    // CAM_FUNC_FIXD0
    fixed_camerawork_01,    // CAM_FUNC_FIXD1
    fixed_camerawork_02,    // CAM_FUNC_FIXD2
    fixed_camerawork_03,    // CAM_FUNC_FIXD3
    fixed_camerawork_04,    // CAM_FUNC_FIXD4
    data_camerawork_00,     // CAM_FUNC_DATA0
    data_camerawork_01,     // CAM_FUNC_DATA1
    data_camerawork_02,     // CAM_FUNC_DATA2
    data_camerawork_03,     // CAM_FUNC_DATA3
    data_camerawork_04,     // CAM_FUNC_DATA4
    unique_camerawork_00,   // CAM_FUNC_UNIQ0
    unique_camerawork_01,   // CAM_FUNC_UNIQ1
    unique_camerawork_02,   // CAM_FUNC_UNIQ2
    unique_camerawork_03,   // CAM_FUNC_UNIQ3
    unique_camerawork_04,   // CAM_FUNC_UNIQ4
    unique_camerawork_05,   // CAM_FUNC_UNIQ5
    unique_camerawork_06,   // CAM_FUNC_UNIQ6
    unique_camerawork_07,   // CAM_FUNC_UNIQ7
    unique_camerawork_08,   // CAM_FUNC_UNIQ8
    unique_camerawork_09,   // CAM_FUNC_UNIQ9
    demo_camerawork_00,     // CAM_FUNC_DEMO0
    demo_camerawork_01,     // CAM_FUNC_DEMO1
    demo_camerawork_02,     // CAM_FUNC_DEMO2
    demo_camerawork_03,     // CAM_FUNC_DEMO3
    demo_camerawork_04,     // CAM_FUNC_DEMO4
    demo_camerawork_05,     // CAM_FUNC_DEMO5
    demo_camerawork_06,     // CAM_FUNC_DEMO6
    demo_camerawork_07,     // CAM_FUNC_DEMO7
    demo_camerawork_08,     // CAM_FUNC_DEMO8
    demo_camerawork_09,     // CAM_FUNC_DEMO9
    special_camerawork_00,  // CAM_FUNC_SPEC0
    special_camerawork_01,  // CAM_FUNC_SPEC1
    special_camerawork_02,  // CAM_FUNC_SPEC2
    special_camerawork_03,  // CAM_FUNC_SPEC3
    special_camerawork_04,  // CAM_FUNC_SPEC4
    special_camerawork_05,  // CAM_FUNC_SPEC5
    special_camerawork_06,  // CAM_FUNC_SPEC6
    special_camerawork_07,  // CAM_FUNC_SPEC7
    special_camerawork_08,  // CAM_FUNC_SPEC8
    special_camerawork_09,  // CAM_FUNC_SPEC9
};

s32 first_time_flag = 1;

s32 debug_camera_sw = false;

#if DEBUG_FEATURES
s32 sDbgModeIdx = -1;
#endif

s16 camera_uid_number = 0;

s32 shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 1);
s32 bitem_no = HUD_VISIBILITY_NOTHING_ALT;
s32 shrink = 32;

s32 attention_part = -1;

s16 colli_add_angle_h[] = {
    0x0AAA, 0xF556, 0x1555, 0xEAAB, 0x2AAA, 0xD556, 0x3FFF, 0xC001, 0x5555, 0xAAAB, 0x6AAA, 0x9556, 0x7FFF, 0x0000,
};

s16 colli_add_angle_v[] = {
    0x0000, 0x02C6, 0x058C, 0x0000, 0x0000, 0xFD3A, 0x0000, 0x0852, 0x0000, 0x0000, 0x0B18, 0x02C6, 0xFA74, 0x0000,
};

s32 cutflag = 0;
s32 up_ctl_type = 0;
s32 scene_first = 0;

s32 attent_frame = -16;

#if DEBUG_FEATURES
char sCameraFunctionNames[][8] = {
    "NONE   ", // CAM_FUNC_NONE
    "NORM0()", // CAM_FUNC_NORM0
    "NORM1()", // CAM_FUNC_NORM1
    "NORM2()", // CAM_FUNC_NORM2
    "NORM3()", // CAM_FUNC_NORM3
    "NORM4()", // CAM_FUNC_NORM4
    "PARA0()", // CAM_FUNC_PARA0
    "PARA1()", // CAM_FUNC_PARA1
    "PARA2()", // CAM_FUNC_PARA2
    "PARA3()", // CAM_FUNC_PARA3
    "PARA4()", // CAM_FUNC_PARA4
    "KEEP0()", // CAM_FUNC_KEEP0
    "KEEP1()", // CAM_FUNC_KEEP1
    "KEEP2()", // CAM_FUNC_KEEP2
    "KEEP3()", // CAM_FUNC_KEEP3
    "KEEP4()", // CAM_FUNC_KEEP4
    "SUBJ0()", // CAM_FUNC_SUBJ0
    "SUBJ1()", // CAM_FUNC_SUBJ1
    "SUBJ2()", // CAM_FUNC_SUBJ2
    "SUBJ3()", // CAM_FUNC_SUBJ3
    "SUBJ4()", // CAM_FUNC_SUBJ4
    "JUMP0()", // CAM_FUNC_JUMP0
    "JUMP1()", // CAM_FUNC_JUMP1
    "JUMP2()", // CAM_FUNC_JUMP2
    "JUMP3()", // CAM_FUNC_JUMP3
    "JUMP4()", // CAM_FUNC_JUMP4
    "BATT0()", // CAM_FUNC_BATT0
    "BATT1()", // CAM_FUNC_BATT1
    "BATT2()", // CAM_FUNC_BATT2
    "BATT3()", // CAM_FUNC_BATT3
    "BATT4()", // CAM_FUNC_BATT4
    "FIXD0()", // CAM_FUNC_FIXD0
    "FIXD1()", // CAM_FUNC_FIXD1
    "FIXD2()", // CAM_FUNC_FIXD2
    "FIXD3()", // CAM_FUNC_FIXD3
    "FIXD4()", // CAM_FUNC_FIXD4
    "DATA0()", // CAM_FUNC_DATA0
    "DATA1()", // CAM_FUNC_DATA1
    "DATA2()", // CAM_FUNC_DATA2
    "DATA3()", // CAM_FUNC_DATA3
    "DATA4()", // CAM_FUNC_DATA4
    "UNIQ0()", // CAM_FUNC_UNIQ0
    "UNIQ1()", // CAM_FUNC_UNIQ1
    "UNIQ2()", // CAM_FUNC_UNIQ2
    "UNIQ3()", // CAM_FUNC_UNIQ3
    "UNIQ4()", // CAM_FUNC_UNIQ4
    "UNIQ5()", // CAM_FUNC_UNIQ5
    "UNIQ6()", // CAM_FUNC_UNIQ6
    "UNIQ7()", // CAM_FUNC_UNIQ7
    "UNIQ8()", // CAM_FUNC_UNIQ8
    "UNIQ9()", // CAM_FUNC_UNIQ9
    "DEMO0()", // CAM_FUNC_DEMO0
    "DEMO1()", // CAM_FUNC_DEMO1
    "DEMO2()", // CAM_FUNC_DEMO2
    "DEMO3()", // CAM_FUNC_DEMO3
    "DEMO4()", // CAM_FUNC_DEMO4
    "DEMO5()", // CAM_FUNC_DEMO5
    "DEMO6()", // CAM_FUNC_DEMO6
    "DEMO7()", // CAM_FUNC_DEMO7
    "DEMO8()", // CAM_FUNC_DEMO8
    "DEMO9()", // CAM_FUNC_DEMO9
    "SPEC0()", // CAM_FUNC_SPEC0
    "SPEC1()", // CAM_FUNC_SPEC1
    "SPEC2()", // CAM_FUNC_SPEC2
    "SPEC3()", // CAM_FUNC_SPEC3
    "SPEC4()", // CAM_FUNC_SPEC4
    "SPEC5()", // CAM_FUNC_SPEC5
    "SPEC6()", // CAM_FUNC_SPEC6
    "SPEC7()", // CAM_FUNC_SPEC7
    "SPEC8()", // CAM_FUNC_SPEC8
    "SPEC9()", // CAM_FUNC_SPEC9
    "",        "", "", "", "",
};
#endif

VecSph keygz[] = {
    { 50.0f, 0xEE3A, 0xD558 },
    { 75.0f, 0x0000, 0x8008 },
    { 80.0f, 0xEE3A, 0x8008 },
    { 15.0f, 0xEE3A, 0x8008 },
};

Vec3f keyadjz[] = {
    { 0.0f, 40.0f, 20.0f },
    { 0.0f, 40.0f, 0.0f },
    { 0.0f, 3.0f, -3.0f },
    { 0.0f, 3.0f, -3.0 },
};

PlayState* __game;

#if DEBUG_FEATURES
DebugCam D_8015BD80;
#endif

CollisionPoly* floor_poly;

/*===============================================================*/

/**
 * Interpolates along a curve between 0 and 1 with a period of
 * -a <= p <= a at time `b`
 */
f32 curve_xx(f32 a, f32 b) {
    f32 ret;
    f32 absB;
    f32 t = 0.4f;
    f32 t2;
    f32 t3;
    f32 t4;

    absB = fabsf(b);
    if (a < absB) {
        ret = 1.0f;
    } else {
        t2 = 1.0f - t;
        if ((a * t2) > absB) {
            t3 = SQ(b) * (1.0f - t);
            t4 = SQ(a * t2);
            ret = t3 / t4;
        } else {
            t3 = SQ(a - absB) * t;
            t4 = SQ(0.4f * a);
            ret = 1.0f - (t3 / t4);
        }
    }
    return ret;
}

/*
 * Performs linear interpolation between `cur` and `target`.  If `cur` is within
 * `minDiff` units, the result is rounded up to `target`
 */
f32 f_cush(f32 target, f32 cur, f32 stepScale, f32 minDiff) {
    f32 diff = target - cur;
    f32 step;
    f32 ret;

    if (fabsf(diff) >= minDiff) {
        step = diff * stepScale;
        ret = cur + step;
    } else {
        ret = target;
    }

    return ret;
}

/*
 * Performs linear interpolation between `cur` and `target`.  If `cur` is within
 * `minDiff` units, the result is rounded down to `cur`
 */
f32 f_cush2(f32 target, f32 cur, f32 stepScale, f32 minDiff) {
    f32 diff = target - cur;
    f32 step;
    f32 ret;

    if (fabsf(diff) >= minDiff) {
        step = diff * stepScale;
        ret = cur + step;
    } else {
        ret = cur;
    }

    return ret;
}

/*
 * Performs linear interpolation between `cur` and `target`.  If `cur` is within
 * `minDiff` units, the result is rounded up to `target`
 */
s16 s_cush(s16 target, s16 cur, f32 stepScale, s16 minDiff) {
    s16 diff = target - cur;
    s16 step;
    s32 ret;

    if (ABS(diff) >= minDiff) {
        step = diff * stepScale + 0.5f;
        ret = cur + step;
    } else {
        ret = target;
    }

    return ret;
}

/*
 * Performs linear interpolation between `cur` and `target`.  If `cur` is within
 * `minDiff` units, the result is rounded down to `cur`
 */
s16 s_cush2(s16 target, s16 cur, f32 stepScale, s16 minDiff) {
    s16 diff = target - cur;
    s16 step;
    s32 ret;

    if (ABS(diff) >= minDiff) {
        step = diff * stepScale + 0.5f;
        ret = cur + step;
    } else {
        ret = cur;
    }

    return ret;
}

/*
 * Performs linear interpolation between `cur` and `target`.  If `cur` is within
 * `minDiff` units, the result is rounded up to `target`
 */
void ez_xyz_cush(Vec3f* target, Vec3f* cur, f32 yStepScale, f32 xzStepScale, f32 minDiff) {
    cur->x = f_cush(target->x, cur->x, xzStepScale, minDiff);
    cur->y = f_cush(target->y, cur->y, yStepScale, minDiff);
    cur->z = f_cush(target->z, cur->z, xzStepScale, minDiff);
}

void default_cushon_set(Camera* camera) {
    camera->yawUpdateRateInv = 100.0f;
    camera->pitchUpdateRateInv = CAM_PITCH_UPDATE_RATE_INV;
    camera->rUpdateRateInv = CAM_R_UPDATE_RATE_INV;
    camera->xzOffsetUpdateRate = CAM_XZ_OFFSET_UPDATE_RATE;
    camera->yOffsetUpdateRate = CAM_Y_OFFSET_UPDATE_RATE;
    camera->fovUpdateRate = CAM_FOV_UPDATE_RATE;
}

void slowly_cushon_set(Camera* camera) {
    camera->rUpdateRateInv = CAM_GLOBAL_27;
    camera->yawUpdateRateInv = CAM_GLOBAL_27;
    camera->pitchUpdateRateInv = CAM_GLOBAL_27;
    camera->xzOffsetUpdateRate = 0.001f;
    camera->yOffsetUpdateRate = 0.001f;
    camera->fovUpdateRate = 0.001f;
}

Vec3f s_xyz2xyz_t(Vec3s* src) {
    Vec3f dest;

    dest.x = src->x;
    dest.y = src->y;
    dest.z = src->z;

    return dest;
}

static Vec3f translate_by_sglobe(Vec3f* a, VecGeo* geo) {
    Vec3f sum;
    Vec3f b = sglobe2world(geo);

    sum.x = a->x + b.x;
    sum.y = a->y + b.y;
    sum.z = a->z + b.z;

    return sum;
}

Vec3f translate_by_unitvector(Vec3f* src, Vec3f* unitVector, f32 uvScale) {
    Vec3f dest;

    dest.x = src->x + (unitVector->x * uvScale);
    dest.y = src->y + (unitVector->y * uvScale);
    dest.z = src->z + (unitVector->z * uvScale);

    return dest;
}

/**
 * Detects the collision poly between `from` and `to`, places collision info in `to`
 */
static s32 check_wall(Camera* camera, Vec3f* from, CamColChk* to) {
    CollisionContext* colCtx = &camera->play->colCtx;
    Vec3f toNewPos;
    Vec3f toPoint;
    Vec3f fromToNorm;
    f32 floorPolyY;
    CollisionPoly* floorPoly;
    s32 floorBgId;
    VecGeo fromToOffset;

    fromToOffset = sglobe_by_2pos(from, &to->pos);
    fromToOffset.r += 8.0f;
    toPoint = translate_by_sglobe(from, &fromToOffset);

    if (!T_BGCheck_CameraLineCheck_poly_chgrp_ai(colCtx, from, &toPoint, &toNewPos, &to->poly, 1, 1, 1, -1, &to->bgId)) {
        // no poly in path.
        fromToNorm = unitvector_by_2pos(from, &to->pos);

        to->norm.x = -fromToNorm.x;
        to->norm.y = -fromToNorm.y;
        to->norm.z = -fromToNorm.z;

        toNewPos = to->pos;
        toNewPos.y += 5.0f;
        floorPolyY = T_BGCheck_CameraGroundCheck_grpWG_ai(colCtx, &floorPoly, &floorBgId, &toNewPos);

        if ((to->pos.y - floorPolyY) > 5.0f) {
            // if the y distance from the check point to the floor is more than 5 units
            // the point is not colliding with any collision.
            to->pos.x += to->norm.x;
            to->pos.y += to->norm.y;
            to->pos.z += to->norm.z;
            return 0;
        }

        to->poly = floorPoly;
        toNewPos.y = floorPolyY + 1.0f;
        to->bgId = floorBgId;
    }

    to->norm.x = COLPOLY_GET_NORMAL(to->poly->normal.x);
    to->norm.y = COLPOLY_GET_NORMAL(to->poly->normal.y);
    to->norm.z = COLPOLY_GET_NORMAL(to->poly->normal.z);
    to->pos.x = to->norm.x + toNewPos.x;
    to->pos.y = to->norm.y + toNewPos.y;
    to->pos.z = to->norm.z + toNewPos.z;

    //! @bug floorBgId is uninitialized if T_BGCheck_CameraLineCheck_poly_chgrp_ai returned true above
    return floorBgId + 1;
}

/**
 * Detects if there is collision between `from` and `to`
 */
s32 check_wall_light(Camera* camera, Vec3f* from, Vec3f* to) {
    CamColChk toCol;
    s32 bgId;

    toCol.pos = *to;
    bgId = check_wall(camera, from, &toCol);
    *to = toCol.pos;
    return bgId;
}

s32 check_not_wall_bgc(Camera* camera, Vec3f* from, CamColChk* to) {
    CollisionContext* colCtx = &camera->play->colCtx;
    Vec3f toNewPos;
    Vec3f toPos;
    Vec3f fromToNorm;
    Vec3f playerFloorNormF;
    f32 floorY;
    CollisionPoly* floorPoly;
    s32 bgId;
    VecGeo fromToGeo;

    fromToGeo = sglobe_by_2pos(from, &to->pos);
    fromToGeo.r += 8.0f;
    toPos = translate_by_sglobe(from, &fromToGeo);
    if (!T_BGCheck_CameraLineCheck_poly_chgrp_ai(colCtx, from, &toPos, &toNewPos, &to->poly, 1, 1, 1, -1, &to->bgId)) {
        fromToNorm = unitvector_by_2pos(from, &to->pos);
        to->norm.x = -fromToNorm.x;
        to->norm.y = -fromToNorm.y;
        to->norm.z = -fromToNorm.z;
        toNewPos = to->pos;
        toNewPos.y += 5.0f;
        floorY = T_BGCheck_CameraGroundCheck_grpWG_ai(colCtx, &floorPoly, &bgId, &toNewPos);
        if ((to->pos.y - floorY) > 5.0f) {
            // to is not on the ground or below it.
            to->pos.x += to->norm.x;
            to->pos.y += to->norm.y;
            to->pos.z += to->norm.z;
            return 0;
        }
        // to is touching the ground, move it up 1 unit.
        to->poly = floorPoly;
        toNewPos.y = floorY + 1.0f;
        to->bgId = bgId;
    }
    to->norm.x = COLPOLY_GET_NORMAL(to->poly->normal.x);
    to->norm.y = COLPOLY_GET_NORMAL(to->poly->normal.y);
    to->norm.z = COLPOLY_GET_NORMAL(to->poly->normal.z);
    if ((to->norm.y > 0.5f) || (to->norm.y < -0.8f)) {
        to->pos.x = to->norm.x + toNewPos.x;
        to->pos.y = to->norm.y + toNewPos.y;
        to->pos.z = to->norm.z + toNewPos.z;
    } else if (floor_poly != NULL) {
        playerFloorNormF.x = COLPOLY_GET_NORMAL(floor_poly->normal.x);
        playerFloorNormF.y = COLPOLY_GET_NORMAL(floor_poly->normal.y);
        playerFloorNormF.z = COLPOLY_GET_NORMAL(floor_poly->normal.z);
        if (Math3DCheckPlaneCrossLine_xyz_t_cp(playerFloorNormF.x, playerFloorNormF.y, playerFloorNormF.z, floor_poly->dist,
                                  from, &toPos, &toNewPos, 1)) {
            // line is from->to is touching the poly the player is on.
            to->norm = playerFloorNormF;
            to->poly = floor_poly;
            to->bgId = camera->bgId;
            to->pos.x = to->norm.x + toNewPos.x;
            to->pos.y = to->norm.y + toNewPos.y;
            to->pos.z = to->norm.z + toNewPos.z;
        } else {
            fromToNorm = unitvector_by_2pos(from, &to->pos);
            to->norm.x = -fromToNorm.x;
            to->norm.y = -fromToNorm.y;
            to->norm.z = -fromToNorm.z;
            to->pos.x += to->norm.x;
            to->pos.y += to->norm.y;
            to->pos.z += to->norm.z;
            return 0;
        }
    }
    return 1;
}

void check_not_wall_light(Camera* camera, Vec3f* arg1, Vec3f* arg2) {
    CamColChk sp20;
    Vec3s unused;

    sp20.pos = *arg2;
    check_not_wall_bgc(camera, arg1, &sp20);
    *arg2 = sp20.pos;
}

/**
 * Checks if `from` to `to` is looking from the outside of a poly towards the front
 */
s32 out_of_game_area(Camera* camera, Vec3f* from, Vec3f* to) {
    s32 pad;
    Vec3f intersect;
    s32 pad2;
    s32 bgId;
    CollisionPoly* poly;
    CollisionContext* colCtx = &camera->play->colCtx;

    poly = NULL;
    if (T_BGCheck_CameraLineCheck_poly_chgrp_ai(colCtx, from, to, &intersect, &poly, 1, 1, 1, 0, &bgId) &&
        (T_Polygon_PlaneFunc(poly, from) < 0.0f)) {
        // if there is a poly between `from` and `to` and the `from` is behind the poly.
        return true;
    }

    return false;
}

/**
 * Gets the floor position underneath `chkPos`, and returns the normal of the floor to `floorNorm`,
 * and bgId to `bgId`.  If no floor is found, then the normal is a flat surface pointing upwards.
 */
static f32 floor_at(Camera* camera, Vec3f* floorNorm, Vec3f* chkPos, s32* bgId) {
    s32 pad;
    CollisionPoly* floorPoly;
    f32 floorY = T_BGCheck_ObjGroundCheck_ai(&camera->play->colCtx, &floorPoly, bgId, chkPos);

    if (floorY == BGCHECK_Y_MIN) {
        // no floor
        floorNorm->x = 0.0f;
        floorNorm->y = 1.0f;
        floorNorm->z = 0.0f;
    } else {
        floorNorm->x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
        floorNorm->y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
        floorNorm->z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
    }

    return floorY;
}

/**
 * Gets the position of the floor from `pos`
 */
f32 floor_at_light(Camera* camera, Vec3f* pos) {
    Vec3f posCheck;
    Vec3f floorNorm;
    s32 bgId;

    posCheck = *pos;
    posCheck.y += 80.0f;

    return floor_at(camera, &floorNorm, &posCheck, &bgId);
}

/**
 * Gets the position of the floor from `pos`, and if the floor is considered not solid,
 * it checks the next floor below that up to 3 times.  Returns the normal of the floor into `norm`
 */
f32 floor_at4fwdchk(Camera* camera, Vec3f* norm, Vec3f* pos, s32* bgId) {
    CollisionPoly* floorPoly;
    CollisionContext* colCtx = &camera->play->colCtx;
    f32 floorY;
    s32 i;

    for (i = 3; i > 0; i--) {
        floorY = T_BGCheck_CameraGroundCheck_grpWG_ai(colCtx, &floorPoly, bgId, pos);
        if (floorY == BGCHECK_Y_MIN ||
            (camera->playerGroundY < floorY && !(COLPOLY_GET_NORMAL(floorPoly->normal.y) > 0.5f))) {
            // no floor, or player is below the floor and floor is not considered steep
            norm->x = 0.0f;
            norm->y = 1.0f;
            norm->z = 0.0f;
            floorY = BGCHECK_Y_MIN;
            break;
        } else if (T_BGCheck_getAttributeCode_ai(colCtx, floorPoly, *bgId) == FLOOR_TYPE_1) {
            // floor is not solid, check below that floor.
            pos->y = floorY - 10.0f;
            continue;
        } else {
            norm->x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
            norm->y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
            norm->z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
            break;
        }
    }

#if DEBUG_FEATURES
    if (i == 0) {
        PRINTF(VT_COL(YELLOW, BLACK) "camera: foward check: too many layer!\n" VT_RST);
    }
#endif

    return floorY;
}

/**
 * Returns the CameraSettingType of the camera at index `bgCamIndex`
 */
s16 get_camera_set_by_cameraID(Camera* camera, s32 bgCamIndex) {
    return T_BGCheck_getCameraType_index_ai(&camera->play->colCtx, bgCamIndex, BGCHECK_SCENE);
}

/**
 * Returns the bgCamFuncData using the current bgCam index
 */
Vec3s* get_camera_point_by_cameraID(Camera* camera) {
    return T_BGCheck_getCameraPos_index_ai(&camera->play->colCtx, camera->bgCamIndex, BGCHECK_SCENE);
}

/**
 * Gets the bgCam index for the poly `poly`, returns -1 if
 * there is no camera data for that poly.
 */
s32 get_cameraID_from_polygon(Camera* camera, s32* bgId, CollisionPoly* poly) {
    s32 bgCamIndex;
    UNUSED PosRot playerPosRot;
    s32 ret;

    playerPosRot = Actor_get_shape(&camera->player->actor);
    bgCamIndex = T_BGCheck_getCameraSet_ai(&camera->play->colCtx, poly, *bgId);

    if (T_BGCheck_getCameraType_index_ai(&camera->play->colCtx, bgCamIndex, *bgId) == CAM_SET_NONE) {
        ret = -1;
    } else {
        ret = bgCamIndex;
    }
    return ret;
}

/**
 * Returns the bgCamFuncData for the floor under the player.
 * Also returns the number of pieces of data there are in `bgCamCount`.
 * If there is no floor, then return NULL
 */
Vec3s* get_camera_point_from_polygon(Camera* camera, u16* bgCamCount) {
    CollisionPoly* floorPoly;
    s32 pad;
    s32 bgId;
    PosRot playerPosRot;

    playerPosRot = Actor_get_shape(&camera->player->actor);
    playerPosRot.pos.y += player_get_tall(camera->player);

    if (T_BGCheck_ObjGroundCheck_ai(&camera->play->colCtx, &floorPoly, &bgId, &playerPosRot.pos) == BGCHECK_Y_MIN) {
        // no floor
        return NULL;
    }

    *bgCamCount = T_BGCheck_getCameraDataNum_ai(&camera->play->colCtx, floorPoly, bgId);
    return T_BGCheck_getCameraPos_ai(&camera->play->colCtx, floorPoly, bgId);
}

/**
 * Gets the Camera information for the water box the player is in.
 * Returns -1 if the player is not in a water box, or does not have a swimming state.
 * Returns -2 if there is no camera index for the water box.
 * Returns the camera data index otherwise.
 */
s32 get_cameraID_from_watersurface(Camera* camera, f32* waterY) {
    PosRot playerPosRot;
    WaterBox* waterBox;
    s32 bgCamIndex;

    playerPosRot = Actor_get_shape(&camera->player->actor);
    *waterY = playerPosRot.pos.y;

    if (!T_BGCheck_WaterSurfaceCheck3(camera->play, &camera->play->colCtx, playerPosRot.pos.x, playerPosRot.pos.z, waterY,
                              &waterBox)) {
        // player's position is not within the x/z boundaries of a water box.
        *waterY = BGCHECK_Y_MIN;
        return -1;
    }

    if (!(camera->player->stateFlags1 & PLAYER_STATE1_27)) {
        // player is not swimming
        *waterY = BGCHECK_Y_MIN;
        return -1;
    }

    bgCamIndex = T_BGCheck_WaterGetCameraSet(&camera->play->colCtx, waterBox);

    //! @bug bgCamIndex = 0 is a valid index, should be (bgCamIndex < 0)
    if ((bgCamIndex <= 0) || (T_BGCheck_WaterGetCameraType(&camera->play->colCtx, waterBox) <= CAM_SET_NONE)) {
        // no camera data index, or no CameraSettingType
        return -2;
    }

    return bgCamIndex;
}

/**
 * Checks if `chkPos` is inside a waterbox.
 * If there is no water box below `chkPos` or if `chkPos` is above the water surface, return BGCHECK_Y_MIN.
 * If `chkPos` is inside the waterbox, output light index to `lightIndex`.
 */
f32 watersurface_at(Camera* camera, Vec3f* chkPos, s32* lightIndex) {
    PosRot playerPosRot;
    f32 waterY;
    WaterBox* waterBox;

    playerPosRot = Actor_get_shape(&camera->player->actor);
    waterY = playerPosRot.pos.y;

    if (!T_BGCheck_WaterSurfaceCheck3(camera->play, &camera->play->colCtx, chkPos->x, chkPos->z, &waterY, &waterBox)) {
        // chkPos is not within the x/z boundaries of a water box.
        return BGCHECK_Y_MIN;
    }

    if (chkPos->y > waterY) {
        // the check position is above the water's y position
        // meaning the position is NOT in the water.
        return BGCHECK_Y_MIN;
    }

    *lightIndex = T_BGCheck_WaterGetKankyo(&camera->play->colCtx, waterBox);
    return waterY;
}

/**
 * Calculates the angle between points `from` and `to`
 */
static s16 get_y_angle_by_2pos(Vec3f* to, Vec3f* from) {
    return CAM_DEG_TO_BINANG(RAD_TO_DEG(fatan2(from->x - to->x, from->z - to->z)));
}

s16 check_forward(Camera* camera, s16 viewYaw, s16 initAndReturnZero) {
    static f32 f1;
    static f32 f2;
    static CamColChk bgc0;
    Vec3f playerPos;
    Vec3f nearPos;
    Vec3f floorNorm;
    f32 checkOffsetY;
    s16 pitchNear;
    s16 pitchFar;
    f32 floorYDiffFar;
    f32 viewForwardsUnitX;
    f32 viewForwardsUnitZ;
    s32 bgId;
    f32 nearDist;
    f32 farDist;
    f32 floorYDiffNear;
    f32 playerHeight;

    viewForwardsUnitX = sin_s(viewYaw);
    viewForwardsUnitZ = cos_s(viewYaw);

    playerHeight = player_get_tall(camera->player);
    checkOffsetY = playerHeight * CAM_PITCH_FLOOR_CHECK_OFFSET_Y_FAC;
    nearDist = playerHeight * CAM_PITCH_FLOOR_CHECK_NEAR_DIST_FAC;
    farDist = playerHeight * CAM_PITCH_FLOOR_CHECK_FAR_DIST_FAC;

    playerPos.x = camera->playerPosRot.pos.x;
    playerPos.y = camera->playerGroundY + checkOffsetY;
    playerPos.z = camera->playerPosRot.pos.z;

    nearPos.x = playerPos.x + (nearDist * viewForwardsUnitX);
    nearPos.y = playerPos.y;
    nearPos.z = playerPos.z + (nearDist * viewForwardsUnitZ);

    if (initAndReturnZero || (camera->play->state.frames % 2) == 0) {
        bgc0.pos.x = playerPos.x + (farDist * viewForwardsUnitX);
        bgc0.pos.y = playerPos.y;
        bgc0.pos.z = playerPos.z + (farDist * viewForwardsUnitZ);

        check_wall(camera, &playerPos, &bgc0);

        if (initAndReturnZero) {
            f1 = f2 = camera->playerGroundY;
        }
    } else {
        farDist = distance_2d(&playerPos, &bgc0.pos);

        bgc0.pos.x += bgc0.norm.x * 5.0f;
        bgc0.pos.y += bgc0.norm.y * 5.0f;
        bgc0.pos.z += bgc0.norm.z * 5.0f;

        if (nearDist > farDist) {
            nearDist = farDist;
            f1 = f2 = floor_at4fwdchk(camera, &floorNorm, &bgc0.pos, &bgId);
        } else {
            f1 = floor_at4fwdchk(camera, &floorNorm, &nearPos, &bgId);
            f2 = floor_at4fwdchk(camera, &floorNorm, &bgc0.pos, &bgId);
        }

        if (f1 == BGCHECK_Y_MIN) {
            f1 = camera->playerGroundY;
        }

        if (f2 == BGCHECK_Y_MIN) {
            f2 = f1;
        }
    }

    floorYDiffNear = CAM_PITCH_FLOOR_CHECK_NEAR_WEIGHT * (f1 - camera->playerGroundY);
    floorYDiffFar = (1.0f - CAM_PITCH_FLOOR_CHECK_NEAR_WEIGHT) * (f2 - camera->playerGroundY);

    pitchNear = CAM_DEG_TO_BINANG(RAD_TO_DEG(fatan2(floorYDiffNear, nearDist)));
    pitchFar = CAM_DEG_TO_BINANG(RAD_TO_DEG(fatan2(floorYDiffFar, farDist)));

    return pitchNear + pitchFar;
}

/**
 * Calculates a new Up vector from the pitch, yaw, roll
 */
Vec3f calcup(s16 pitch, s16 yaw, s16 roll) {
    f32 sinP = sin_s(pitch);
    f32 cosP = cos_s(pitch);
    f32 sinY = sin_s(yaw);
    f32 cosY = cos_s(yaw);
    f32 sinR = sin_s(-roll);
    f32 cosR = cos_s(-roll);
    Vec3f up;
    Vec3f baseUp;
    Vec3f u;
    Vec3f rollMtxRow1;
    Vec3f rollMtxRow2;
    Vec3f rollMtxRow3;
    f32 pad;

    // Axis to roll around
    u.x = cosP * sinY;
    u.y = sinP;
    u.z = cosP * cosY;

    // Matrix to apply the roll to the Up vector without roll
    rollMtxRow1.x = ((1.0f - SQ(u.x)) * cosR) + SQ(u.x);
    rollMtxRow1.y = ((u.x * u.y) * (1.0f - cosR)) - (u.z * sinR);
    rollMtxRow1.z = ((u.z * u.x) * (1.0f - cosR)) + (u.y * sinR);

    rollMtxRow2.x = ((u.x * u.y) * (1.0f - cosR)) + (u.z * sinR);
    rollMtxRow2.y = ((1.0f - SQ(u.y)) * cosR) + SQ(u.y);
    rollMtxRow2.z = ((u.y * u.z) * (1.0f - cosR)) - (u.x * sinR);

    rollMtxRow3.x = ((u.z * u.x) * (1.0f - cosR)) - (u.y * sinR);
    rollMtxRow3.y = ((u.y * u.z) * (1.0f - cosR)) + (u.x * sinR);
    rollMtxRow3.z = ((1.0f - SQ(u.z)) * cosR) + SQ(u.z);

    // Up without roll
    baseUp.x = -sinP * sinY;
    baseUp.y = cosP;
    baseUp.z = -sinP * cosY;

    // rollMtx * baseUp
    up.x = DOTXYZ(baseUp, rollMtxRow1);
    up.y = DOTXYZ(baseUp, rollMtxRow2);
    up.z = DOTXYZ(baseUp, rollMtxRow3);

    return up;
}

f32 recover_shift(Camera* camera, f32 maxLERPScale) {
    f32 ret;

    if (camera->atLERPStepScale < CAM_AT_LERP_STEP_SCALE_MIN) {
        ret = CAM_AT_LERP_STEP_SCALE_MIN;
    } else if (camera->atLERPStepScale >= maxLERPScale) {
        ret = maxLERPScale;
    } else {
        ret = CAM_AT_LERP_STEP_SCALE_FAC * camera->atLERPStepScale;
    }

    return ret;
}

void reset_local_camera(Camera* camera, s16 mode) {
    CameraModeValue* values;
    CameraModeValue* valueP;
    s32 i;

#if DEBUG_FEATURES
    if (PREG(82)) {
        PRINTF("camera: res: stat (%d/%d/%d)\n", camera->camId, camera->setting, mode);
    }

    values = cam_ctl_tbl_0[camera->setting].cameraModes[mode].values;

    for (i = 0; i < cam_ctl_tbl_0[camera->setting].cameraModes[mode].valueCnt; i++) {
        valueP = &values[i];
        PREG(valueP->dataType) = valueP->val;
        if (PREG(82)) {
            PRINTF("camera: res: PREG(%02d) = %d\n", valueP->dataType, valueP->val);
        }
    }
#endif

    camera->animState = 0;
}

#if DEBUG_FEATURES
s32 copyCameraPosPREGToModeValues(Camera* camera) {
    CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
    CameraModeValue* valueP;
    s32 i;

    for (i = 0; i < cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].valueCnt; i++) {
        valueP = &values[i];
        valueP->val = R_CAM_DATA(valueP->dataType);
        if (PREG(82)) {
            PRINTF("camera: res: %d = PREG(%02d)\n", valueP->val, valueP->dataType);
        }
    }
    return true;
}
#endif

void camera_set_shrink_and_bitem(s16 interfaceField) {
    s16 hudVisibilityMode;

    if ((interfaceField & CAM_LETTERBOX_MASK) != CAM_LETTERBOX_IGNORE) {
        switch (interfaceField & CAM_LETTERBOX_SIZE_MASK) {
            case CAM_LETTERBOX_SMALL:
                shrink = 26;
                break;

            case CAM_LETTERBOX_MEDIUM:
                shrink = 27;
                break;

            case CAM_LETTERBOX_LARGE:
                shrink = 32;
                break;

            default:
                shrink = 0;
                break;
        }

        if (interfaceField & CAM_LETTERBOX_INSTANT) {
            shrink_window_setnowval(shrink);
        } else {
            shrink_window_setval(shrink);
        }
    }

    if ((interfaceField & CAM_HUD_VISIBILITY_MASK) != CAM_HUD_VISIBILITY(CAM_HUD_VISIBILITY_IGNORE)) {
        hudVisibilityMode = (interfaceField & CAM_HUD_VISIBILITY_MASK) >> CAM_HUD_VISIBILITY_SHIFT;
        if (hudVisibilityMode == CAM_HUD_VISIBILITY_ALL) {
            hudVisibilityMode = HUD_VISIBILITY_ALL;
        }
        if (bitem_no != hudVisibilityMode) {
            bitem_no = hudVisibilityMode;
            alpha_change(bitem_no);
        }
    }
}

Vec3f search_corner(Vec3f* linePointA, Vec3f* linePointB, CamColChk* pointAColChk, CamColChk* pointBColChk) {
    Vec3f closestPoint;
    UNUSED_NDEBUG bool result;

    result = T_BGCheck_2PlaneLine_LineNearPos(pointAColChk->poly, pointBColChk->poly, linePointA, linePointB, &closestPoint);
#if DEBUG_FEATURES
    if (!result) {
        PRINTF(VT_COL(YELLOW, BLACK) "camera: corner check no cross point %x %x\n" VT_RST, pointAColChk, pointBColChk);
        return pointAColChk->pos;
    }
#endif

    return closestPoint;
}

/**
 * Checks collision between at and eyeNext, if `checkEye` is set, if there is no collision between
 * eyeNext->at, then eye->at is also checked.
 * Returns:
 * 0 if no collision is found between at->eyeNext
 * 2 if the angle between the polys is between 60 degrees and 120 degrees
 * 3 ?
 * 6 if the angle between the polys is greater than 120 degrees
 */
s32 camera_bgcheck(Camera* camera, VecGeo* diffGeo, CamColChk* eyeChk, CamColChk* atChk, s16 checkEye) {
    Vec3f* at = &camera->at;
    Vec3f* eye = &camera->eye;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f eyePos;
    s32 atEyeBgId;
    s32 eyeAtBgId;
    s32 ret;
    f32 cosEyeAt;

    eyeChk->pos = camera->eyeNext;

    ret = 0;

    atEyeBgId = check_wall(camera, at, eyeChk);
    if (atEyeBgId != 0) {
        // collision found between at->eye
        atChk->pos = camera->at;

        eyeChk->geoNorm = world2sglobe(&eyeChk->norm);

        if (eyeChk->geoNorm.pitch >= 0x2EE1) {
            eyeChk->geoNorm.yaw = diffGeo->yaw;
        }

        eyeAtBgId = check_wall(camera, eyeNext, atChk);

        if (eyeAtBgId == 0) {
            // no collision from eyeNext->at
            if (checkEye & 1) {

                atChk->pos = *at;
                eyePos = *eye;

                if (check_wall(camera, &eyePos, atChk) == 0) {
                    // no collision from eye->at
                    return 3;
                } else if (eyeChk->poly == atChk->poly) {
                    // at->eye and eye->at is the same poly
                    return 3;
                }
            } else {
                return 3;
            }
        } else if (eyeChk->poly == atChk->poly) {
            // at->eyeNext and eyeNext->at is the same poly
            return 3;
        }

        atChk->geoNorm = world2sglobe(&atChk->norm);

        if (atChk->geoNorm.pitch >= 0x2EE1) {
            atChk->geoNorm.yaw = diffGeo->yaw - 0x7FFF;
        }

        if (atEyeBgId != eyeAtBgId) {
            // different bgIds for at->eye[Next] and eye[Next]->at
            ret = 3;
        } else {
            cosEyeAt = M3D_getCos(&eyeChk->norm, &atChk->norm);
            if (cosEyeAt < -0.5f) {
                ret = 6;
            } else if (cosEyeAt > 0.5f) {
                ret = 3;
            } else {
                ret = 2;
            }
        }
    }
    return ret;
}

/**
 * Calculates how much to adjust the camera at's y value when on a slope.
 */
f32 calc_slope_adjust(Vec3f* floorNorm, s16 playerYRot, s16 eyeAtYaw, f32 adjAmt) {
    f32 tmp;
    VecGeo floorNormGeo;

    floorNormGeo = world2sglobe(floorNorm);

    tmp = cos_s(floorNormGeo.pitch) * cos_s(playerYRot - floorNormGeo.yaw);
    return (fabsf(tmp) * adjAmt) * cos_s(playerYRot - eyeAtYaw);
}

/**
 * Calculates new at vector for the camera pointing in `eyeAtDir`
 */
s32 center4normal(Camera* camera, VecGeo* eyeAtDir, f32 yOffset, s16 calcSlopeYAdj) {
    Vec3f* at = &camera->at;
    Vec3f playerToAtOffsetTarget;
    Vec3f atTarget;
    s32 pad2;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 playerHeight = player_get_tall(camera->player);

    playerToAtOffsetTarget.x = 0.f;
    playerToAtOffsetTarget.y = playerHeight + yOffset;
    playerToAtOffsetTarget.z = 0.f;

    if (calcSlopeYAdj) {
        playerToAtOffsetTarget.y -= limiter(
            calc_slope_adjust(&camera->floorNorm, playerPosRot->rot.y, eyeAtDir->yaw, CAM_SLOPE_Y_ADJ_AMOUNT),
            playerHeight);
    }

    ez_xyz_cush(&playerToAtOffsetTarget, &camera->playerToAtOffset, camera->yOffsetUpdateRate,
                         camera->xzOffsetUpdateRate, 0.1f);

    atTarget.x = playerPosRot->pos.x + camera->playerToAtOffset.x;
    atTarget.y = playerPosRot->pos.y + camera->playerToAtOffset.y;
    atTarget.z = playerPosRot->pos.z + camera->playerToAtOffset.z;

    ez_xyz_cush(&atTarget, at, camera->atLERPStepScale, camera->atLERPStepScale, 0.2f);

    return true;
}

s32 center4jump(Camera* camera, VecGeo* eyeAtDir, f32 yOffset, f32* arg3, s16 calcSlopeYAdj) {
    f32 phi_f2;
    Vec3f playerToAtOffsetTarget;
    Vec3f atTarget;
    f32 eyeAtAngle;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 deltaY;
    Vec3f* at = &camera->at;
    f32 temp;
    s32 pad;

    playerToAtOffsetTarget.y = player_get_tall(camera->player) + yOffset;
    playerToAtOffsetTarget.x = 0.0f;
    playerToAtOffsetTarget.z = 0.0f;

    if (calcSlopeYAdj) {
        playerToAtOffsetTarget.y -=
            calc_slope_adjust(&camera->floorNorm, playerPosRot->rot.y, eyeAtDir->yaw, CAM_SLOPE_Y_ADJ_AMOUNT);
    }

    deltaY = playerPosRot->pos.y - *arg3;
    eyeAtAngle = fatan2(deltaY, distance_2d(at, &camera->eye));

    if (eyeAtAngle > DEG_TO_RAD(CAM_GLOBAL_32)) {
        if (1) {}
        phi_f2 = 1.0f - sinf(eyeAtAngle - DEG_TO_RAD(CAM_GLOBAL_32));
    } else if (eyeAtAngle < DEG_TO_RAD(CAM_GLOBAL_33)) {
        phi_f2 = 1.0f - sinf(DEG_TO_RAD(CAM_GLOBAL_33) - eyeAtAngle);
    } else {
        phi_f2 = 1.0f;
    }

    playerToAtOffsetTarget.y -= deltaY * phi_f2;
    ez_xyz_cush(&playerToAtOffsetTarget, &camera->playerToAtOffset, CAM_GLOBAL_29, CAM_GLOBAL_30, 0.1f);

    atTarget.x = playerPosRot->pos.x + camera->playerToAtOffset.x;
    atTarget.y = playerPosRot->pos.y + camera->playerToAtOffset.y;
    atTarget.z = playerPosRot->pos.z + camera->playerToAtOffset.z;

    temp = camera->atLERPStepScale;
    ez_xyz_cush(&atTarget, at, temp, camera->atLERPStepScale, 0.2f);

    return 1;
}

s32 center4stairs(Camera* camera, VecGeo* eyeAtDir, f32 yOffset, s16 arg3) {
    f32 phi_f2;
    Vec3f playerToAtOffsetTarget;
    Vec3f atTarget;
    f32 pad;
    f32 temp_ret;
    PosRot* playerPosRot = &camera->playerPosRot;

    playerToAtOffsetTarget.y = player_get_tall(camera->player) + yOffset;
    playerToAtOffsetTarget.x = 0.0f;
    playerToAtOffsetTarget.z = 0.0f;

    temp_ret = sin_s(arg3);

    if (temp_ret < 0.0f) {
        phi_f2 = cos_s(playerPosRot->rot.y - eyeAtDir->yaw);
    } else {
        phi_f2 = -cos_s(playerPosRot->rot.y - eyeAtDir->yaw);
    }

    playerToAtOffsetTarget.y -= temp_ret * phi_f2 * CAM_SLOPE_Y_ADJ_AMOUNT;
    ez_xyz_cush(&playerToAtOffsetTarget, &camera->playerToAtOffset, camera->yOffsetUpdateRate,
                         camera->xzOffsetUpdateRate, 0.1f);

    atTarget.x = playerPosRot->pos.x + camera->playerToAtOffset.x;
    atTarget.y = playerPosRot->pos.y + camera->playerToAtOffset.y;
    atTarget.z = playerPosRot->pos.z + camera->playerToAtOffset.z;
    ez_xyz_cush(&atTarget, &camera->at, camera->atLERPStepScale, camera->atLERPStepScale, 0.2f);

    return 1;
}

/**
 * Adjusts the camera's at position for parallel_camerawork_01
 */
s32 center4parallel(Camera* camera, VecGeo* arg1, f32 yOffset, f32* arg3, s16 calcSlopeYAdj) {
    Vec3f* at = &camera->at;
    Vec3f playerToAtOffsetTarget;
    Vec3f atTarget;
    Vec3f* eye = &camera->eye;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 temp_f2;
    f32 phi_f16;
    f32 eyeAtDistXZ;
    f32 phi_f20;
    f32 playerHeight = player_get_tall(camera->player);

    playerToAtOffsetTarget.x = 0.0f;
    playerToAtOffsetTarget.y = playerHeight + yOffset;
    playerToAtOffsetTarget.z = 0.0f;

    if (R_CAM_PARALLEL_LOCKON_CALC_SLOPE_Y_ADJ && calcSlopeYAdj) {
        playerToAtOffsetTarget.y -=
            calc_slope_adjust(&camera->floorNorm, playerPosRot->rot.y, arg1->yaw, CAM_SLOPE_Y_ADJ_AMOUNT);
    }

    if (camera->playerGroundY == camera->playerPosRot.pos.y || camera->player->actor.gravity > -0.1f ||
        camera->player->stateFlags1 & PLAYER_STATE1_21) {
        *arg3 = f_cush(playerPosRot->pos.y, *arg3, CAM_GLOBAL_43, 0.1f);
        phi_f20 = playerPosRot->pos.y - *arg3;
        playerToAtOffsetTarget.y -= phi_f20;
        ez_xyz_cush(&playerToAtOffsetTarget, &camera->playerToAtOffset, camera->yOffsetUpdateRate,
                             camera->xzOffsetUpdateRate, 0.1f);
    } else {
        if (!PREG(75)) {
            phi_f20 = playerPosRot->pos.y - *arg3;
            eyeAtDistXZ = distance_2d(at, &camera->eye);
            phi_f16 = eyeAtDistXZ;
            fatan2(phi_f20, eyeAtDistXZ);
            temp_f2 = ftan(DEG_TO_RAD(camera->fov * 0.4f)) * phi_f16;
            if (temp_f2 < phi_f20) {
                *arg3 += phi_f20 - temp_f2;
                phi_f20 = temp_f2;
            } else if (phi_f20 < -temp_f2) {
                *arg3 += phi_f20 + temp_f2;
                phi_f20 = -temp_f2;
            }
            playerToAtOffsetTarget.y -= phi_f20;
        } else {
            phi_f20 = playerPosRot->pos.y - *arg3;
            temp_f2 = fatan2(phi_f20, distance_2d(at, eye));
            if (DEG_TO_RAD(CAM_GLOBAL_32) < temp_f2) {
                phi_f16 = 1 - sinf(temp_f2 - DEG_TO_RAD(CAM_GLOBAL_32));
            } else if (temp_f2 < DEG_TO_RAD(CAM_GLOBAL_33)) {
                phi_f16 = 1 - sinf(DEG_TO_RAD(CAM_GLOBAL_33) - temp_f2);
            } else {
                phi_f16 = 1;
            }
            playerToAtOffsetTarget.y -= phi_f20 * phi_f16;
        }
        ez_xyz_cush(&playerToAtOffsetTarget, &camera->playerToAtOffset, CAM_GLOBAL_29, CAM_GLOBAL_30, 0.1f);
        camera->yOffsetUpdateRate = CAM_GLOBAL_29;
        camera->xzOffsetUpdateRate = CAM_GLOBAL_30;
    }
    atTarget.x = playerPosRot->pos.x + camera->playerToAtOffset.x;
    atTarget.y = playerPosRot->pos.y + camera->playerToAtOffset.y;
    atTarget.z = playerPosRot->pos.z + camera->playerToAtOffset.z;
    ez_xyz_cush(&atTarget, at, camera->atLERPStepScale, camera->atLERPStepScale, 0.2f);
    return 1;
}

#define CAM_LOCKON_AT_FLAG_CALC_SLOPE_Y_ADJ (1 << 0)
#define CAM_LOCKON_AT_FLAG_OFF_GROUND (1 << 7)

/**
 * Adjusts at position for battle_camerawork_01 and keepon_camerawork_01
 */
s32 center4keepon(Camera* camera, VecGeo* eyeAtDir, Vec3f* targetPos, f32 yOffset, f32 distance,
                           f32* yPosOffset, VecGeo* outPlayerToTargetDir, s16 flags) {
    Vec3f* at = &camera->at;
    Vec3f playerToAtOffsetTarget;
    Vec3f tmpPos1;
    Vec3f lookFromOffset;
    Vec3f* floorNorm = &camera->floorNorm;
    VecGeo playerToTargetDir;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 yPosDelta;
    f32 phi_f16;
    f32 eyeAtDistXZ;
    f32 temp_f0_2;
    f32 playerHeight = player_get_tall(camera->player);

    playerToAtOffsetTarget.x = 0.0f;
    playerToAtOffsetTarget.y = playerHeight + yOffset;
    playerToAtOffsetTarget.z = 0.0f;
    if (R_CAM_PARALLEL_LOCKON_CALC_SLOPE_Y_ADJ && (flags & CAM_LOCKON_AT_FLAG_CALC_SLOPE_Y_ADJ)) {
        playerToAtOffsetTarget.y -=
            calc_slope_adjust(floorNorm, playerPosRot->rot.y, eyeAtDir->yaw, CAM_SLOPE_Y_ADJ_AMOUNT);
    }

    // tmpPos1 is player's head
    tmpPos1 = playerPosRot->pos;
    tmpPos1.y += playerHeight;
    *outPlayerToTargetDir = sglobe_by_2pos(&tmpPos1, targetPos);
    playerToTargetDir = *outPlayerToTargetDir;
    if (distance < playerToTargetDir.r) {
        playerToTargetDir.r = playerToTargetDir.r * CAM_GLOBAL_38;
    } else {
        // ratio of player's height off ground to player's height.
        temp_f0_2 = limiter((playerPosRot->pos.y - camera->playerGroundY) / playerHeight, 1.0f);
        playerToTargetDir.r =
            (playerToTargetDir.r * CAM_GLOBAL_39) -
            ((playerToTargetDir.r * (CAM_GLOBAL_39 - CAM_GLOBAL_38)) * (playerToTargetDir.r / distance));
        playerToTargetDir.r -= (playerToTargetDir.r * temp_f0_2) * temp_f0_2;
    }

    if (flags & CAM_LOCKON_AT_FLAG_OFF_GROUND) {
        playerToTargetDir.r *= 0.2f;
        camera->xzOffsetUpdateRate = camera->yOffsetUpdateRate = .01f;
    }

    lookFromOffset = sglobe2world(&playerToTargetDir);

    if (PREG(89)) {
        PRINTF("%f (%f %f %f) %f\n", playerToTargetDir.r / distance, lookFromOffset.x, lookFromOffset.y,
               lookFromOffset.z, camera->atLERPStepScale);
    }

    playerToAtOffsetTarget.x += lookFromOffset.x;
    playerToAtOffsetTarget.y += lookFromOffset.y;
    playerToAtOffsetTarget.z += lookFromOffset.z;

    if (camera->playerGroundY == camera->playerPosRot.pos.y || camera->player->actor.gravity > -0.1f ||
        camera->player->stateFlags1 & PLAYER_STATE1_21) {
        *yPosOffset = f_cush(playerPosRot->pos.y, *yPosOffset, CAM_GLOBAL_43, 0.1f);
        yPosDelta = playerPosRot->pos.y - *yPosOffset;
        playerToAtOffsetTarget.y -= yPosDelta;
        ez_xyz_cush(&playerToAtOffsetTarget, &camera->playerToAtOffset, camera->yOffsetUpdateRate,
                             camera->xzOffsetUpdateRate, 0.1f);
    } else {
        if (!(flags & CAM_LOCKON_AT_FLAG_OFF_GROUND)) {
            yPosDelta = playerPosRot->pos.y - *yPosOffset;
            eyeAtDistXZ = distance_2d(at, &camera->eye);
            phi_f16 = eyeAtDistXZ;
            fatan2(yPosDelta, eyeAtDistXZ);
            temp_f0_2 = ftan(DEG_TO_RAD(camera->fov * 0.4f)) * phi_f16;
            if (temp_f0_2 < yPosDelta) {
                *yPosOffset = *yPosOffset + (yPosDelta - temp_f0_2);
                yPosDelta = temp_f0_2;
            } else if (yPosDelta < -temp_f0_2) {
                *yPosOffset = *yPosOffset + (yPosDelta + temp_f0_2);
                yPosDelta = -temp_f0_2;
            }
            playerToAtOffsetTarget.y -= yPosDelta;
        } else {
            yPosDelta = playerPosRot->pos.y - *yPosOffset;
            temp_f0_2 = fatan2(yPosDelta, distance_2d(at, &camera->eye));

            if (temp_f0_2 > DEG_TO_RAD(CAM_GLOBAL_32)) {
                phi_f16 = 1.0f - sinf(temp_f0_2 - DEG_TO_RAD(CAM_GLOBAL_32));
            } else if (temp_f0_2 < DEG_TO_RAD(CAM_GLOBAL_33)) {
                phi_f16 = 1.0f - sinf(DEG_TO_RAD(CAM_GLOBAL_33) - temp_f0_2);
            } else {
                phi_f16 = 1.0f;
            }
            playerToAtOffsetTarget.y -= (yPosDelta * phi_f16);
        }

        ez_xyz_cush(&playerToAtOffsetTarget, &camera->playerToAtOffset, CAM_GLOBAL_29, CAM_GLOBAL_30, 0.1f);
        camera->yOffsetUpdateRate = CAM_GLOBAL_29;
        camera->xzOffsetUpdateRate = CAM_GLOBAL_30;
    }

    tmpPos1.x = playerPosRot->pos.x + camera->playerToAtOffset.x;
    tmpPos1.y = playerPosRot->pos.y + camera->playerToAtOffset.y;
    tmpPos1.z = playerPosRot->pos.z + camera->playerToAtOffset.z;
    ez_xyz_cush(&tmpPos1, at, camera->atLERPStepScale, camera->atLERPStepScale, 0.2f);
    return 1;
}

s32 center4horse(Camera* camera, VecGeo* eyeAtDir, f32 yOffset, f32* yPosOffset, s16 calcSlopeYAdj) {
    Vec3f* at = &camera->at;
    Vec3f playerToAtOffsetTarget;
    Vec3f atTarget;
    s32 pad;
    s32 pad2;
    f32 playerHeight = player_get_tall(camera->player);
    Player* player = camera->player;
    PosRot horsePosRot;

    horsePosRot = Actor_get_shape(player->rideActor);

    if (EN_HORSE_CHECK_JUMPING((EnHorse*)player->rideActor)) {
        horsePosRot.pos.y -= 49.f;
        *yPosOffset = f_cush(horsePosRot.pos.y, *yPosOffset, 0.1f, 0.2f);
        camera->atLERPStepScale = f_cush(0.4f, camera->atLERPStepScale, 0.2f, 0.02f);
    } else {
        *yPosOffset = f_cush(horsePosRot.pos.y, *yPosOffset, 0.5f, 0.2f);
    }

    playerToAtOffsetTarget.x = 0.0f;
    playerToAtOffsetTarget.y = playerHeight + yOffset;
    playerToAtOffsetTarget.z = 0.0f;

    if (calcSlopeYAdj) {
        playerToAtOffsetTarget.y -=
            calc_slope_adjust(&camera->floorNorm, camera->playerPosRot.rot.y, eyeAtDir->yaw, CAM_SLOPE_Y_ADJ_AMOUNT);
    }

    ez_xyz_cush(&playerToAtOffsetTarget, &camera->playerToAtOffset, camera->yOffsetUpdateRate,
                         camera->xzOffsetUpdateRate, 0.1f);

    atTarget.x = camera->playerToAtOffset.x + horsePosRot.pos.x;
    atTarget.y = camera->playerToAtOffset.y + horsePosRot.pos.y;
    atTarget.z = camera->playerToAtOffset.z + horsePosRot.pos.z;
    ez_xyz_cush(&atTarget, at, camera->atLERPStepScale, camera->atLERPStepScale, 0.2f);

    return 1;
}

f32 calc_default_radius(Camera* camera, f32 dist, f32 min, f32 max) {
    f32 distTarget;
    f32 rUpdateRateInvTarget;

    if (dist < min) {
        distTarget = min;
        rUpdateRateInvTarget = CAM_R_UPDATE_RATE_INV;
    } else if (dist > max) {
        distTarget = max;
        rUpdateRateInvTarget = CAM_R_UPDATE_RATE_INV;
    } else {
        distTarget = dist;
        rUpdateRateInvTarget = 1.0f;
    }

    camera->rUpdateRateInv =
        f_cush(rUpdateRateInvTarget, camera->rUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
    return f_cush(distTarget, camera->dist, 1.0f / camera->rUpdateRateInv, 0.2f);
}

f32 calc_default_radius2(Camera* camera, f32 dist, f32 minDist, f32 maxDist, s16 timer) {
    f32 distTarget;
    f32 rUpdateRateInvTarget;

    if (dist < minDist) {
        distTarget = minDist;

        rUpdateRateInvTarget = timer != 0 ? CAM_R_UPDATE_RATE_INV * 0.5f : CAM_R_UPDATE_RATE_INV;
    } else if (maxDist < dist) {
        distTarget = maxDist;

        rUpdateRateInvTarget = timer != 0 ? CAM_R_UPDATE_RATE_INV * 0.5f : CAM_R_UPDATE_RATE_INV;
    } else {
        distTarget = dist;

        rUpdateRateInvTarget = timer != 0 ? CAM_R_UPDATE_RATE_INV : 1.0f;
    }

    camera->rUpdateRateInv =
        f_cush(rUpdateRateInvTarget, camera->rUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
    return f_cush(distTarget, camera->dist, 1.0f / camera->rUpdateRateInv, 0.2f);
}

s16 calc_default_latitude(Camera* camera, s16 arg1, s16 arg2, s16 arg3) {
    f32 pad;
    f32 stepScale;
    f32 t;
    s16 phi_v0;
    s16 absCur;
    s16 target;

    absCur = ABS(arg1);
    phi_v0 = arg3 > 0 ? (s16)(cos_s(arg3) * arg3) : arg3;
    target = arg2 - phi_v0;

    if (ABS(target) < absCur) {
        stepScale = (1.0f / camera->pitchUpdateRateInv) * 3.0f;
    } else {
        t = absCur * (1.0f / CAM_MAX_PITCH);
        pad = curve_xx(0.8f, 1.0f - t);
        stepScale = (1.0f / camera->pitchUpdateRateInv) * pad;
    }
    return s_cush(target, arg1, stepScale, 0xA);
}

s16 calc_default_longitude(Camera* camera, s16 cur, s16 target, f32 arg3, f32 accel) {
    f32 velocity;
    s16 angDelta;
    f32 updSpeed;
    f32 speedT;
    f32 velFactor;
    f32 yawUpdRate;

    if (camera->xzSpeed > 0.001f) {
        angDelta = target - (s16)(cur - 0x7FFF);
        speedT = COLPOLY_GET_NORMAL((s16)(angDelta - 0x7FFF));
    } else {
        angDelta = target - (s16)(cur - 0x7FFF);
        speedT = CAM_GLOBAL_48;
    }

    updSpeed = curve_xx(arg3, speedT);

    velocity = updSpeed + (1.0f - updSpeed) * accel;

    if (velocity < 0.0f) {
        velocity = 0.0f;
    }

    velFactor = curve_xx(0.5f, camera->speedRatio);
    yawUpdRate = 1.0f / camera->yawUpdateRateInv;
    return cur + (s16)(angDelta * velocity * velFactor * yawUpdRate);
}

void default_bgcheck_proc(Camera* camera, VecGeo* eyeAdjustment, f32 minDist, f32 arg3, f32* arg4, SwingAnimation* anim) {
    static CamColChk bgc0;
    static CamColChk bgc1;
    static CamColChk bgc2;
    Vec3f* eye = &camera->eye;
    s32 temp_v0;
    Vec3f* at = &camera->at;
    Vec3f peekAroundPoint;
    Vec3f* eyeNext = &camera->eyeNext;
    f32 temp_f0;
    VecGeo newEyeAdjustment;
    VecGeo sp40;

    temp_v0 = camera_bgcheck(camera, eyeAdjustment, &bgc0, &bgc1, !anim->unk_18);

    switch (temp_v0) {
        case 1:
        case 2:
            // angle between polys is between 60 and 120 degrees.
            anim->collisionClosePoint = search_corner(at, eyeNext, &bgc0, &bgc1);
            peekAroundPoint.x = anim->collisionClosePoint.x + (bgc0.norm.x + bgc1.norm.x);
            peekAroundPoint.y = anim->collisionClosePoint.y + (bgc0.norm.y + bgc1.norm.y);
            peekAroundPoint.z = anim->collisionClosePoint.z + (bgc0.norm.z + bgc1.norm.z);

            temp_f0 = distance_between(at, &bgc0.pos);
            *arg4 = temp_f0 > minDist ? 1.0f : temp_f0 / minDist;

            anim->swingUpdateRate = CAM_GLOBAL_10;
            anim->unk_18 = 1;
            anim->atEyePoly = bgc1.poly;
            newEyeAdjustment = sglobe_by_2pos(at, &peekAroundPoint);

            newEyeAdjustment.r = eyeAdjustment->r;
            *eye = translate_by_sglobe(at, &newEyeAdjustment);
            bgc2.pos = *eye;
            if (check_wall(camera, at, &bgc2) == 0) {
                // no collision found between at->newEyePos
                newEyeAdjustment.yaw += (s16)(eyeAdjustment->yaw - newEyeAdjustment.yaw) >> 1;
                newEyeAdjustment.pitch += (s16)(eyeAdjustment->pitch - newEyeAdjustment.pitch) >> 1;
                *eye = translate_by_sglobe(at, &newEyeAdjustment);
                if (bgc0.geoNorm.pitch < 0x2AA8) {
                    // ~ 60 degrees
                    anim->unk_16 = newEyeAdjustment.yaw;
                    anim->unk_14 = newEyeAdjustment.pitch;
                } else {
                    anim->unk_16 = eyeAdjustment->yaw;
                    anim->unk_14 = eyeAdjustment->pitch;
                }
                peekAroundPoint.x = anim->collisionClosePoint.x - (bgc0.norm.x + bgc1.norm.x);
                peekAroundPoint.y = anim->collisionClosePoint.y - (bgc0.norm.y + bgc1.norm.y);
                peekAroundPoint.z = anim->collisionClosePoint.z - (bgc0.norm.z + bgc1.norm.z);
                newEyeAdjustment = sglobe_by_2pos(at, &peekAroundPoint);
                newEyeAdjustment.r = eyeAdjustment->r;
                *eyeNext = translate_by_sglobe(at, &newEyeAdjustment);
                break;
            }

            camera->eye = bgc2.pos;
            bgc0 = bgc2;
            FALLTHROUGH;
        case 3:
        case 6:
            if (anim->unk_18 != 0) {
                anim->swingUpdateRateTimer = CAM_GLOBAL_52;
                anim->unk_18 = 0;
                *eyeNext = *eye;
            }

            temp_f0 = distance_between(at, &bgc0.pos);
            *arg4 = temp_f0 > minDist ? 1.0f : temp_f0 / minDist;

            anim->swingUpdateRate = *arg4 * arg3;

            *eye = translate_by_unitvector(&bgc0.pos, &bgc0.norm, 1.0f);
            anim->atEyePoly = NULL;
            if (temp_f0 < CAM_GLOBAL_21) {
                sp40.yaw = eyeAdjustment->yaw;
                sp40.pitch = sin_s(bgc0.geoNorm.pitch + 0x3FFF) * 16380.0f;
                sp40.r = (CAM_GLOBAL_21 - temp_f0) * CAM_GLOBAL_22;
                *eye = translate_by_sglobe(eye, &sp40);
            }
            break;
        default:
            if (anim->unk_18 != 0) {
                anim->swingUpdateRateTimer = CAM_GLOBAL_52;
                *eyeNext = *eye;
                anim->unk_18 = 0;
            }
            anim->swingUpdateRate = arg3;
            anim->atEyePoly = NULL;
            eye->x = bgc0.pos.x + bgc0.norm.x;
            eye->y = bgc0.pos.y + bgc0.norm.y;
            eye->z = bgc0.pos.z + bgc0.norm.z;
            break;
    }
}

s32 NOTUSED(Camera* camera) {
    return true;
}

s32 normal_camerawork_01(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    f32 spA0;
    f32 sp9C;
    f32 sp98;
    f32 sp94;
    Vec3f sp88;
    s16 wiggleAdj;
    s16 t;
    VecGeo eyeAdjustment;
    VecGeo atEyeGeo;
    VecGeo atEyeNextGeo;
    PosRot* playerPosRot = &camera->playerPosRot;
    Normal1ReadOnlyData* roData = &camera->paramData.norm1.roData;
    Normal1ReadWriteData* rwData = &camera->paramData.norm1.rwData;
    f32 playerHeight;
    f32 rate = 0.1f;

    playerHeight = player_get_tall(camera->player);
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = (1.0f + CAM_YOFFSET_NORM - CAM_YOFFSET_NORM * (68.0f / playerHeight));

        sp94 = yNormal * CAM_DATA_SCALED(playerHeight);

        roData->yOffset = GET_NEXT_RO_DATA(values) * sp94;
        roData->distMin = GET_NEXT_RO_DATA(values) * sp94;
        roData->distMax = GET_NEXT_RO_DATA(values) * sp94;
        roData->pitchTarget = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_SCALED_RO_DATA(values);
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->atLERPScaleMax = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    shrink_and_bitem = roData->interfaceField;

    atEyeGeo = sglobe_by_2pos(at, eye);
    atEyeNextGeo = sglobe_by_2pos(at, eyeNext);

    switch (camera->animState) {
        case 20:
            camera->yawUpdateRateInv = CAM_GLOBAL_27;
            camera->pitchUpdateRateInv = CAM_GLOBAL_27;
            FALLTHROUGH;
        case 0:
        case 10:
        case 25:
            rwData->swing.atEyePoly = NULL;
            rwData->slopePitchAdj = 0;
            rwData->unk_28 = 0xA;
            rwData->swing.unk_16 = rwData->swing.unk_14 = rwData->swing.unk_18 = 0;
            rwData->swing.swingUpdateRate = roData->unk_0C;
            rwData->yOffset = camera->playerPosRot.pos.y;
            rwData->unk_20 = camera->xzSpeed;
            rwData->swing.swingUpdateRateTimer = 0;
            rwData->swingYawTarget = atEyeGeo.yaw;
            cutflag = 0;
            rwData->startSwingTimer = CAM_GLOBAL_50 + CAM_GLOBAL_51;
            break;
        default:
            break;
    }

    camera->animState = 1;
    cutflag = 1;

    if (rwData->unk_28 != 0) {
        rwData->unk_28--;
    }

    if (camera->xzSpeed > 0.001f) {
        rwData->startSwingTimer = CAM_GLOBAL_50 + CAM_GLOBAL_51;
    } else if (rwData->startSwingTimer > 0) {
        if (rwData->startSwingTimer > CAM_GLOBAL_50) {
            rwData->swingYawTarget = atEyeGeo.yaw + ((s16)((s16)(camera->playerPosRot.rot.y - 0x7FFF) - atEyeGeo.yaw) /
                                                     rwData->startSwingTimer);
        }
        rwData->startSwingTimer--;
    }

    spA0 = camera->speedRatio * CAM_UPDATE_RATE_STEP_SCALE_XZ;
    sp9C = camera->speedRatio * CAM_UPDATE_RATE_STEP_SCALE_Y;
    sp98 = rwData->swing.unk_18 != 0 ? CAM_UPDATE_RATE_STEP_SCALE_XZ : spA0;

    sp94 = (camera->xzSpeed - rwData->unk_20) * (0.333333f);
    if (sp94 > 1.0f) {
        sp94 = 1.0f;
    }
    if (sp94 > -1.0f) {
        sp94 = -1.0f;
    }

    rwData->unk_20 = camera->xzSpeed;

    if (rwData->swing.swingUpdateRateTimer != 0) {
        camera->yawUpdateRateInv =
            f_cush(rwData->swing.swingUpdateRate + (f32)(rwData->swing.swingUpdateRateTimer * 2),
                             camera->yawUpdateRateInv, sp98, rate);
        camera->pitchUpdateRateInv =
            f_cush((f32)CAM_PITCH_UPDATE_RATE_INV + (f32)(rwData->swing.swingUpdateRateTimer * 2),
                             camera->pitchUpdateRateInv, sp9C, rate);
        rwData->swing.swingUpdateRateTimer--;
    } else {
        camera->yawUpdateRateInv =
            f_cush(rwData->swing.swingUpdateRate - (rwData->swing.swingUpdateRate * CAM_GLOBAL_49 * sp94),
                             camera->yawUpdateRateInv, sp98, rate);
        camera->pitchUpdateRateInv =
            f_cush(CAM_PITCH_UPDATE_RATE_INV, camera->pitchUpdateRateInv, sp9C, rate);
    }

    camera->pitchUpdateRateInv = f_cush(CAM_PITCH_UPDATE_RATE_INV, camera->pitchUpdateRateInv, sp9C, rate);
    camera->xzOffsetUpdateRate = f_cush(CAM_XZ_OFFSET_UPDATE_RATE, camera->xzOffsetUpdateRate, spA0, rate);
    camera->yOffsetUpdateRate = f_cush(CAM_Y_OFFSET_UPDATE_RATE, camera->yOffsetUpdateRate, sp9C, rate);
    camera->fovUpdateRate =
        f_cush(CAM_FOV_UPDATE_RATE, camera->yOffsetUpdateRate, camera->speedRatio * 0.05f, rate);

    if (roData->interfaceField & NORMAL1_FLAG_0) {
        t = check_forward(camera, atEyeGeo.yaw - 0x7FFF, false);
        sp9C = ((1.0f / roData->unk_10) * 0.5f) * (1.0f - camera->speedRatio);
        rwData->slopePitchAdj =
            s_cush(t, rwData->slopePitchAdj, ((1.0f / roData->unk_10) * 0.5f) + sp9C, 0xF);
    } else {
        rwData->slopePitchAdj = 0;
        if (camera->playerGroundY == camera->playerPosRot.pos.y) {
            rwData->yOffset = camera->playerPosRot.pos.y;
        }
    }

    spA0 = ((rwData->swing.unk_18 != 0) && (roData->yOffset > -40.0f))
               ? (sp9C = sin_s(rwData->swing.unk_14), ((-40.0f * sp9C) + (roData->yOffset * (1.0f - sp9C))))
               : roData->yOffset;

    if (roData->interfaceField & NORMAL1_FLAG_7) {
        center4jump(camera, &atEyeNextGeo, spA0, &rwData->yOffset, roData->interfaceField & NORMAL1_FLAG_0);
    } else if (roData->interfaceField & NORMAL1_FLAG_5) {
        center4stairs(camera, &atEyeNextGeo, spA0, rwData->slopePitchAdj);
    } else {
        center4normal(camera, &atEyeNextGeo, spA0, roData->interfaceField & NORMAL1_FLAG_0);
    }

    eyeAdjustment = sglobe_by_2pos(at, eyeNext);

    camera->dist = eyeAdjustment.r =
        calc_default_radius2(camera, eyeAdjustment.r, roData->distMin, roData->distMax, rwData->unk_28);

    if (rwData->startSwingTimer <= 0) {
        eyeAdjustment.pitch = atEyeNextGeo.pitch;
        eyeAdjustment.yaw =
            s_cush(rwData->swingYawTarget, atEyeNextGeo.yaw, 1.0f / camera->yawUpdateRateInv, 0xA);
    } else if (rwData->swing.unk_18 != 0) {
        eyeAdjustment.yaw =
            s_cush(rwData->swing.unk_16, atEyeNextGeo.yaw, 1.0f / camera->yawUpdateRateInv, 0xA);
        eyeAdjustment.pitch =
            s_cush(rwData->swing.unk_14, atEyeNextGeo.pitch, 1.0f / camera->yawUpdateRateInv, 0xA);
    } else {
        // rotate yaw to follow player.
        eyeAdjustment.yaw =
            calc_default_longitude(camera, atEyeNextGeo.yaw, camera->playerPosRot.rot.y, roData->unk_14, sp94);
        eyeAdjustment.pitch =
            calc_default_latitude(camera, atEyeNextGeo.pitch, roData->pitchTarget, rwData->slopePitchAdj);
    }

    // set eyeAdjustment pitch from 79.65 degrees to -85 degrees
    if (eyeAdjustment.pitch > 0x38A4) {
        eyeAdjustment.pitch = 0x38A4;
    }
    if (eyeAdjustment.pitch < -0x3C8C) {
        eyeAdjustment.pitch = -0x3C8C;
    }

    *eyeNext = translate_by_sglobe(at, &eyeAdjustment);
    if ((camera->status == CAM_STAT_ACTIVE) && !(roData->interfaceField & NORMAL1_FLAG_4)) {
        rwData->swingYawTarget = camera->playerPosRot.rot.y - 0x7FFF;
        if (rwData->startSwingTimer > 0) {
            default_bgcheck_proc(camera, &eyeAdjustment, roData->distMin, roData->unk_0C, &sp98, &rwData->swing);
        } else {
            sp88 = *eyeNext;
            rwData->swing.swingUpdateRate = camera->yawUpdateRateInv = roData->unk_0C * 2.0f;
            if (check_wall_light(camera, at, &sp88)) {
                rwData->swingYawTarget = atEyeNextGeo.yaw;
                rwData->startSwingTimer = -1;
            } else {
                *eye = *eyeNext;
            }
            rwData->swing.unk_18 = 0;
        }

        if (rwData->swing.unk_18 != 0) {
            camera->inputDir.y =
                s_cush(camera->inputDir.y + (s16)((s16)(rwData->swing.unk_16 - 0x7FFF) - camera->inputDir.y),
                                 camera->inputDir.y, 1.0f - (0.99f * sp98), 0xA);
        }

        if (roData->interfaceField & NORMAL1_FLAG_2) {
            camera->inputDir.x = -atEyeGeo.pitch;
            camera->inputDir.y = atEyeGeo.yaw - 0x7FFF;
            camera->inputDir.z = 0;
        } else {
            eyeAdjustment = sglobe_by_2pos(eye, at);
            camera->inputDir.x = eyeAdjustment.pitch;
            camera->inputDir.y = eyeAdjustment.yaw;
            camera->inputDir.z = 0;
        }

        // crit wiggle
        if (z_common_data.save.info.playerData.health <= 16 && ((camera->play->state.frames % 256) == 0)) {
            wiggleAdj = fqrand() * 10000.0f;
            camera->inputDir.y = wiggleAdj + camera->inputDir.y;
        }
    } else {
        rwData->swing.swingUpdateRate = roData->unk_0C;
        rwData->swing.unk_18 = 0;
        cutflag = 0;
        *eye = *eyeNext;
    }

    spA0 = (z_common_data.save.info.playerData.health <= 16 ? 0.8f : 1.0f);
    camera->fov = f_cush(roData->fovTarget * spA0, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = s_cush(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = recover_shift(camera, roData->atLERPScaleMax);
    return 1;
}

s32 normal_camerawork_02(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    CamColChk bgChk;
    s16 phi_a0;
    s16 phi_a1;
    f32 spA4;
    f32 spA0;
    VecGeo adjGeo;
    VecGeo sp90;
    VecGeo sp88;
    VecGeo atToEyeDir;
    VecGeo atToEyeNextDir;
    PosRot* playerPosRot = &camera->playerPosRot;
    Normal2ReadOnlyData* roData = &camera->paramData.norm2.roData;
    Normal2ReadWriteData* rwData = &camera->paramData.norm2.rwData;
    s32 pad;
    BgCamFuncData* bgCamFuncData;
    f32 playerHeight;
    f32 yNormal;

    playerHeight = player_get_tall(camera->player);
    yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

    CAM_DEBUG_RELOAD_PREG(camera);

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->unk_00 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_04 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_08 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_1C = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_14 = GET_NEXT_RO_DATA(values);
        roData->unk_18 = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    shrink_and_bitem = roData->interfaceField;

    switch (camera->animState) {
        case 0:
        case 10:
        case 20:
        case 25:
            bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);
            rwData->unk_00 = s_xyz2xyz_t(&bgCamFuncData->pos);
            rwData->unk_20 = bgCamFuncData->rot.x;
            rwData->unk_22 = bgCamFuncData->rot.y;
            rwData->unk_24 = playerPosRot->pos.y;
            rwData->unk_1C = bgCamFuncData->fov == -1   ? roData->unk_14
                             : bgCamFuncData->fov > 360 ? CAM_DATA_SCALED(bgCamFuncData->fov)
                                                        : bgCamFuncData->fov;

            rwData->unk_28 = bgCamFuncData->flags == -1 ? 0 : bgCamFuncData->flags;

            rwData->unk_18 = 0.0f;

            if (roData->interfaceField & NORMAL2_FLAG_2) {
                sp88.pitch = rwData->unk_20;
                sp88.yaw = rwData->unk_22 + 0x3FFF;
                sp88.r = 100.0f;
                rwData->unk_0C = sglobe2world(&sp88);
            }

            camera->animState = 1;
            camera->yawUpdateRateInv = 50.0f;
            break;
        default:
            if (camera->playerGroundY == playerPosRot->pos.y) {
                rwData->unk_24 = playerPosRot->pos.y;
            }
            break;
    }

    atToEyeDir = sglobe_by_2pos(at, eye);
    atToEyeNextDir = sglobe_by_2pos(at, eyeNext);

    camera->speedRatio *= 0.5f;
    spA4 = CAM_UPDATE_RATE_STEP_SCALE_XZ * camera->speedRatio;
    spA0 = CAM_UPDATE_RATE_STEP_SCALE_Y * camera->speedRatio;

    camera->yawUpdateRateInv = f_cush(roData->unk_0C, camera->yawUpdateRateInv * camera->speedRatio,
                                                CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
    camera->pitchUpdateRateInv = f_cush(CAM_PITCH_UPDATE_RATE_INV, camera->pitchUpdateRateInv, spA0, 0.1f);
    camera->xzOffsetUpdateRate = f_cush(CAM_XZ_OFFSET_UPDATE_RATE, camera->xzOffsetUpdateRate, spA4, 0.1f);
    camera->yOffsetUpdateRate = f_cush(CAM_Y_OFFSET_UPDATE_RATE, camera->yOffsetUpdateRate, spA0, 0.1f);
    camera->fovUpdateRate =
        f_cush(CAM_FOV_UPDATE_RATE, camera->yOffsetUpdateRate, camera->speedRatio * 0.05f, 0.1f);

    if (!(roData->interfaceField & NORMAL2_FLAG_7)) {
        center4normal(camera, &atToEyeNextDir, roData->unk_00, roData->interfaceField & NORMAL2_FLAG_0);
    } else {
        center4jump(camera, &atToEyeNextDir, roData->unk_00, &rwData->unk_24,
                      roData->interfaceField & NORMAL2_FLAG_0);
    }

    if (roData->interfaceField & NORMAL2_FLAG_2) {
        rwData->unk_00.x = playerPosRot->pos.x + rwData->unk_0C.x;
        rwData->unk_00.z = playerPosRot->pos.z + rwData->unk_0C.z;
    }

    rwData->unk_00.y = playerPosRot->pos.y;

    sp88 = sglobe_by_2pos(&rwData->unk_00, at);
    sp90 = sglobe_by_2pos(at, eyeNext);

    phi_a1 = (rwData->unk_28 & 2 ? rwData->unk_22 : roData->unk_1C);
    phi_a0 = sp90.yaw - sp88.yaw;
    if ((phi_a1 < 0x4000 && ABS(phi_a0) > phi_a1) || (phi_a1 >= 0x4000 && ABS(phi_a0) < phi_a1)) {

        phi_a0 = (phi_a0 < 0 ? -phi_a1 : phi_a1);
        phi_a0 += sp88.yaw;
        adjGeo.yaw =
            s_cush(phi_a0, atToEyeDir.yaw, (1.0f / camera->yawUpdateRateInv) * camera->speedRatio, 0xA);
        if (rwData->unk_28 & 1) {
            adjGeo.pitch = calc_default_latitude(camera, atToEyeNextDir.pitch, rwData->unk_20, 0);
        } else {
            adjGeo.pitch = atToEyeDir.pitch;
        }
    } else {
        adjGeo = sp90;
    }

    camera->dist = adjGeo.r = calc_default_radius2(camera, sp90.r, roData->unk_04, roData->unk_08, 0);

    if (!(rwData->unk_28 & 1)) {
        if (adjGeo.pitch >= 0xE39) {
            adjGeo.pitch += ((s16)(0xE38 - adjGeo.pitch) >> 2);
        }

        if (adjGeo.pitch < 0) {
            adjGeo.pitch += ((s16)(-0x38E - adjGeo.pitch) >> 2);
        }
    }

    *eyeNext = translate_by_sglobe(at, &adjGeo);

    if (camera->status == CAM_STAT_ACTIVE) {
        bgChk.pos = *eyeNext;
        if (!camera->play->envCtx.skyboxDisabled || roData->interfaceField & NORMAL2_FLAG_4) {
            check_wall(camera, at, &bgChk);
            *eye = bgChk.pos;
        } else {
            check_not_wall_bgc(camera, at, &bgChk);
            *eye = bgChk.pos;
            adjGeo = sglobe_by_2pos(eye, at);
            camera->inputDir.x = adjGeo.pitch;
            camera->inputDir.y = adjGeo.yaw;
            camera->inputDir.z = 0;
        }
    }

    camera->fov = f_cush(rwData->unk_1C, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = s_cush(0, camera->roll, .5f, 0xA);
    camera->atLERPStepScale = recover_shift(camera, roData->unk_18);
    return 1;
}

// riding epona
s32 normal_camerawork_03(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    f32 sp98;
    f32 sp94;
    f32 sp90;
    f32 sp8C;
    VecGeo sp84;
    VecGeo sp7C;
    VecGeo sp74;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 temp_f0;
    f32 temp_f6;
    s16 phi_a0;
    s16 t2;
    Normal3ReadOnlyData* roData = &camera->paramData.norm3.roData;
    Normal3ReadWriteData* rwData = &camera->paramData.norm3.rwData;
    f32 playerHeight;

    playerHeight = player_get_tall(camera->player);
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->yOffset = GET_NEXT_RO_DATA(values) * CAM_DATA_SCALED(playerHeight);
        roData->distMin = GET_NEXT_RO_DATA(values) * CAM_DATA_SCALED(playerHeight);
        roData->distMax = GET_NEXT_RO_DATA(values) * CAM_DATA_SCALED(playerHeight);
        roData->pitchTarget = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->yawUpdateSpeed = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->maxAtLERPScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    sp7C = sglobe_by_2pos(at, eye);
    sp74 = sglobe_by_2pos(at, eyeNext);

    cutflag = true;
    shrink_and_bitem = roData->interfaceField;
    switch (camera->animState) {
        case 0:
        case 10:
        case 20:
        case 25:
            rwData->swing.atEyePoly = NULL;
            rwData->curPitch = 0;
            rwData->unk_1C = 0.0f;
            rwData->unk_20 = camera->playerGroundY;
            rwData->swing.unk_16 = rwData->swing.unk_14 = rwData->swing.unk_18 = 0;
            rwData->swing.swingUpdateRate = roData->yawUpdateSpeed;
            rwData->yawUpdAmt = (s16)((s16)(playerPosRot->rot.y - 0x7FFF) - sp7C.yaw) * (1.0f / CAM_DEFAULT_ANIM_TIME);
            rwData->distTimer = 10;
            rwData->yawTimer = CAM_DEFAULT_ANIM_TIME;
            camera->animState = 1;
            rwData->swing.swingUpdateRateTimer = 0;
    }

    if (rwData->distTimer != 0) {
        rwData->distTimer--;
    }

    sp98 = CAM_UPDATE_RATE_STEP_SCALE_XZ * camera->speedRatio;
    sp94 = CAM_UPDATE_RATE_STEP_SCALE_Y * camera->speedRatio;

    if (rwData->swing.swingUpdateRateTimer != 0) {
        camera->yawUpdateRateInv = f_cush(roData->yawUpdateSpeed + (rwData->swing.swingUpdateRateTimer * 2),
                                                    camera->yawUpdateRateInv, sp98, 0.1f);
        camera->pitchUpdateRateInv =
            f_cush((f32)CAM_PITCH_UPDATE_RATE_INV + (rwData->swing.swingUpdateRateTimer * 2),
                             camera->pitchUpdateRateInv, sp94, 0.1f);
        rwData->swing.swingUpdateRateTimer--;
    } else {
        camera->yawUpdateRateInv = f_cush(roData->yawUpdateSpeed, camera->yawUpdateRateInv, sp98, 0.1f);
        camera->pitchUpdateRateInv =
            f_cush(CAM_PITCH_UPDATE_RATE_INV, camera->pitchUpdateRateInv, sp94, 0.1f);
    }

    camera->xzOffsetUpdateRate = f_cush(CAM_XZ_OFFSET_UPDATE_RATE, camera->xzOffsetUpdateRate, sp98, 0.1f);
    camera->yOffsetUpdateRate = f_cush(CAM_Y_OFFSET_UPDATE_RATE, camera->yOffsetUpdateRate, sp94, 0.1f);
    camera->fovUpdateRate = f_cush(CAM_FOV_UPDATE_RATE, camera->fovUpdateRate, sp94, 0.1f);

    t2 = check_forward(camera, sp7C.yaw - 0x7FFF, true);
    sp94 = ((1.0f / roData->unk_10) * 0.5f);
    temp_f0 = (((1.0f / roData->unk_10) * 0.5f) * (1.0f - camera->speedRatio));
    rwData->curPitch = s_cush(t2, rwData->curPitch, sp94 + temp_f0, 0xF);

    center4horse(camera, &sp74, roData->yOffset, &rwData->unk_20, true);
    sp90 = (roData->distMax + roData->distMin) * 0.5f;
    sp84 = sglobe_by_2pos(at, eyeNext);
    camera->dist = sp84.r = calc_default_radius2(camera, sp84.r, roData->distMin, roData->distMax, rwData->distTimer);
    if (camera->xzSpeed > 0.001f) {
        sp84.r += (sp90 - sp84.r) * 0.002f;
    }
    phi_a0 = roData->pitchTarget - rwData->curPitch;
    sp84.pitch = s_cush(phi_a0, sp74.pitch, 1.0f / camera->pitchUpdateRateInv, 0xA);

    if (sp84.pitch > CAM_MAX_PITCH) {
        sp84.pitch = CAM_MAX_PITCH;
    }
    if (sp84.pitch < CAM_MIN_PITCH_1) {
        sp84.pitch = CAM_MIN_PITCH_1;
    }

    phi_a0 = playerPosRot->rot.y - (s16)(sp74.yaw - 0x7FFF);
    if (ABS(phi_a0) > 0x2AF8) {
        if (phi_a0 > 0) {
            phi_a0 = 0x2AF8;
        } else {
            phi_a0 = -0x2AF8;
        }
    }

    sp90 = 1.0f;
    sp98 = 0.5;
    sp94 = camera->speedRatio;
    sp90 -= sp98;
    sp98 = sp98 + (sp94 * sp90);
    sp98 = (sp98 * phi_a0) / camera->yawUpdateRateInv;

    if (1) {}
    sp84.yaw = fabsf(sp98) > (150.0f * (1.0f - camera->speedRatio)) ? (s16)(sp74.yaw + sp98) : sp74.yaw;

    if (rwData->yawTimer > 0) {
        sp84.yaw += rwData->yawUpdAmt;
        rwData->yawTimer--;
    }

    *eyeNext = translate_by_sglobe(at, &sp84);

    if (camera->status == CAM_STAT_ACTIVE) {
        default_bgcheck_proc(camera, &sp84, roData->distMin, roData->yawUpdateSpeed, &sp8C, &rwData->swing);
    } else {
        *eye = *eyeNext;
    }

    camera->fov = f_cush(roData->fovTarget, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = s_cush(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = recover_shift(camera, roData->maxAtLERPScale);
    return 1;
}

s32 normal_camerawork_04(Camera* camera) {
    return NOTUSED(camera);
}

s32 normal_camerawork_00(Camera* camera) {
    return NOTUSED(camera);
}

s32 parallel_camerawork_01(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    f32 spB8;
    f32 spB4;
    s16 tangle;
    VecGeo spA8;
    VecGeo atToEyeDir;
    VecGeo atToEyeNextDir;
    PosRot* playerPosRot = &camera->playerPosRot;
    CamColChk sp6C;
    s16 sp6A;
    s16 phi_a0;
    Parallel1ReadOnlyData* roData = &camera->paramData.para1.roData;
    Parallel1ReadWriteData* rwData = &camera->paramData.para1.rwData;
    f32 pad2;
    f32 playerHeight;
    s32 pad3;

    playerHeight = player_get_tall(camera->player);
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->distTarget = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->pitchTarget = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->yawTarget = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->unk_08 = GET_NEXT_RO_DATA(values);
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
        roData->unk_18 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_1C = GET_NEXT_SCALED_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    atToEyeDir = sglobe_by_2pos(at, eye);
    atToEyeNextDir = sglobe_by_2pos(at, eyeNext);

    switch (camera->animState) {
        case 0:
        case 10:
        case 20:
        case 25:
            rwData->unk_16 = 0;
            rwData->unk_10 = 0;
            if (roData->interfaceField & PARALLEL1_FLAG_2) {
                rwData->animTimer = 20;
            } else {
                rwData->animTimer = CAM_DEFAULT_ANIM_TIME;
            }
            rwData->unk_00.x = 0.0f;
            rwData->yTarget = playerPosRot->pos.y - camera->playerPosDelta.y;
            camera->animState++;
            break;
    }

    if (rwData->animTimer != 0) {
        if (roData->interfaceField & PARALLEL1_FLAG_1) {
            // Rotate roData->yawTarget degrees from behind the player.
            rwData->yawTarget = (s16)(playerPosRot->rot.y - 0x7FFF) + roData->yawTarget;
        } else if (roData->interfaceField & PARALLEL1_FLAG_2) {
            // rotate to roData->yawTarget
            rwData->yawTarget = roData->yawTarget;
        } else {
            // leave the rotation alone.
            rwData->yawTarget = atToEyeNextDir.yaw;
        }
    } else {
        if (roData->interfaceField & PARALLEL1_FLAG_5) {
            rwData->yawTarget = (s16)(playerPosRot->rot.y - 0x7FFF) + roData->yawTarget;
        }
        shrink_and_bitem = roData->interfaceField;
    }

    rwData->pitchTarget = roData->pitchTarget;

    if (camera->animState == 21) {
        rwData->unk_16 = 1;
        camera->animState = 1;
    } else if (camera->animState == 11) {
        camera->animState = 1;
    }

    spB8 = CAM_UPDATE_RATE_STEP_SCALE_XZ * camera->speedRatio;
    spB4 = CAM_UPDATE_RATE_STEP_SCALE_Y * camera->speedRatio;

    camera->rUpdateRateInv = f_cush(CAM_R_UPDATE_RATE_INV, camera->rUpdateRateInv, spB8, 0.1f);
    camera->yawUpdateRateInv = f_cush(roData->unk_08, camera->yawUpdateRateInv, spB8, 0.1f);
    camera->pitchUpdateRateInv = f_cush(2.0f, camera->pitchUpdateRateInv, spB4, 0.1f);
    camera->xzOffsetUpdateRate = f_cush(CAM_XZ_OFFSET_UPDATE_RATE, camera->xzOffsetUpdateRate, spB8, 0.1f);
    camera->yOffsetUpdateRate = f_cush(CAM_Y_OFFSET_UPDATE_RATE, camera->yOffsetUpdateRate, spB4, 0.1f);
    camera->fovUpdateRate =
        f_cush(CAM_FOV_UPDATE_RATE, camera->fovUpdateRate, camera->speedRatio * 0.05f, 0.1f);

    if (roData->interfaceField & PARALLEL1_FLAG_0) {
        tangle = check_forward(camera, atToEyeDir.yaw - 0x7FFF, true);

        spB8 = ((1.0f / roData->unk_0C) * 0.3f);
        pad2 = (((1.0f / roData->unk_0C) * 0.7f) * (1.0f - camera->speedRatio));
        rwData->unk_10 = s_cush(tangle, rwData->unk_10, spB8 + pad2, 0xF);
    } else {
        rwData->unk_10 = 0;
    }

    if (camera->playerGroundY == camera->playerPosRot.pos.y || camera->player->actor.gravity > -0.1f ||
        camera->player->stateFlags1 & PLAYER_STATE1_21) {
        rwData->yTarget = playerPosRot->pos.y;
        sp6A = 0;
    } else {
        sp6A = 1;
    }

    if (!(roData->interfaceField & PARALLEL1_FLAG_7) && !sp6A) {
        center4parallel(camera, &atToEyeNextDir, roData->yOffset, &rwData->yTarget,
                                 roData->interfaceField & PARALLEL1_FLAG_0);
    } else {
        center4jump(camera, &atToEyeNextDir, roData->unk_18, &rwData->yTarget,
                      roData->interfaceField & PARALLEL1_FLAG_0);
    }

    if (rwData->animTimer != 0) {
        camera->stateFlags |= CAM_STATE_LOCK_MODE;
        tangle = (((rwData->animTimer + 1) * rwData->animTimer) >> 1);
        spA8.yaw = atToEyeDir.yaw + (((s16)(rwData->yawTarget - atToEyeDir.yaw) / tangle) * rwData->animTimer);
        spA8.pitch = atToEyeDir.pitch;
        spA8.r = atToEyeDir.r;
        rwData->animTimer--;
    } else {
        rwData->unk_16 = 0;
        camera->dist = f_cush(roData->distTarget, camera->dist, 1.0f / camera->rUpdateRateInv, 2.0f);
        spA8 = sglobe_by_2pos(at, eyeNext);
        spA8.r = camera->dist;

        if (roData->interfaceField & PARALLEL1_FLAG_6) {
            spA8.yaw = s_cush(rwData->yawTarget, atToEyeNextDir.yaw, 0.6f, 0xA);
        } else {
            spA8.yaw = s_cush(rwData->yawTarget, atToEyeNextDir.yaw, 0.8f, 0xA);
        }

        if (roData->interfaceField & PARALLEL1_FLAG_0) {
            phi_a0 = rwData->pitchTarget - rwData->unk_10;
        } else {
            phi_a0 = rwData->pitchTarget;
        }

        spA8.pitch = s_cush(phi_a0, atToEyeNextDir.pitch, 1.0f / camera->pitchUpdateRateInv, 4);

        if (spA8.pitch > CAM_MAX_PITCH) {
            spA8.pitch = CAM_MAX_PITCH;
        }

        if (spA8.pitch < CAM_MIN_PITCH_1) {
            spA8.pitch = CAM_MIN_PITCH_1;
        }
    }
    *eyeNext = translate_by_sglobe(at, &spA8);
    if (camera->status == CAM_STAT_ACTIVE) {
        sp6C.pos = *eyeNext;
        if (!camera->play->envCtx.skyboxDisabled || roData->interfaceField & PARALLEL1_FLAG_4) {
            check_wall(camera, at, &sp6C);
            *eye = sp6C.pos;
        } else {
            check_not_wall_bgc(camera, at, &sp6C);
            *eye = sp6C.pos;
            spA8 = sglobe_by_2pos(eye, at);
            camera->inputDir.x = spA8.pitch;
            camera->inputDir.y = spA8.yaw;
            camera->inputDir.z = 0;
        }
    }
    camera->fov = f_cush(roData->fovTarget, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = s_cush(0, camera->roll, 0.5, 0xA);
    camera->atLERPStepScale = recover_shift(camera, sp6A ? roData->unk_1C : roData->unk_14);
    //! @bug Missing return, but the return value is not used.
}

s32 parallel_camerawork_02(Camera* camera) {
    return NOTUSED(camera);
}

s32 parallel_camerawork_03(Camera* camera) {
    CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
    s16 interfaceField = GET_NEXT_RO_DATA(values);

    shrink_and_bitem = interfaceField;

    if (interfaceField & PARALLEL3_FLAG_0) {
        camera->stateFlags |= CAM_STATE_BLOCK_BG;
    }
    if (interfaceField & PARALLEL3_FLAG_1) {
        camera->stateFlags |= CAM_STATE_CAM_FUNC_FINISH;
    }
    //! @bug Missing return, but the return value is not used.
}

s32 parallel_camerawork_04(Camera* camera) {
    return NOTUSED(camera);
}

s32 parallel_camerawork_00(Camera* camera) {
    return NOTUSED(camera);
}

/**
 * Generic jump, jumping off ledges
 */
s32 jump_camerawork_01(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    s32 pad2;
    f32 spA4;
    Vec3f newEye;
    VecGeo eyeAtOffset;
    VecGeo eyeNextAtOffset;
    VecGeo eyeDiffGeo;
    VecGeo eyeDiffTarget;
    PosRot* playerPosRot = &camera->playerPosRot;
    UNUSED PosRot playerhead;
    s16 tangle;
    Jump1ReadOnlyData* roData = &camera->paramData.jump1.roData;
    Jump1ReadWriteData* rwData = &camera->paramData.jump1.rwData;
    s32 pad;
    f32 playerHeight;

    playerHeight = player_get_tall(camera->player);
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

        roData->atYOffset = CAM_DATA_SCALED(GET_NEXT_RO_DATA(values)) * playerHeight * yNormal;
        roData->distMin = CAM_DATA_SCALED(GET_NEXT_RO_DATA(values)) * playerHeight * yNormal;
        roData->distMax = CAM_DATA_SCALED(GET_NEXT_RO_DATA(values)) * playerHeight * yNormal;
        roData->yawUpateRateTarget = GET_NEXT_RO_DATA(values);
        roData->maxYawUpdate = CAM_DATA_SCALED(GET_NEXT_RO_DATA(values));
        roData->unk_14 = GET_NEXT_RO_DATA(values);
        roData->atLERPScaleMax = CAM_DATA_SCALED(GET_NEXT_RO_DATA(values));
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    playerhead = Actor_get_eye(&camera->player->actor);

    eyeAtOffset = sglobe_by_2pos(at, eye);
    eyeNextAtOffset = sglobe_by_2pos(at, eyeNext);

    shrink_and_bitem = roData->interfaceField;

    if (RELOAD_PARAMS(camera)) {
        rwData->swing.unk_16 = rwData->swing.unk_18 = 0;
        rwData->swing.atEyePoly = NULL;
        rwData->unk_24 = 0;
        rwData->unk_26 = 200;
        rwData->swing.swingUpdateRateTimer = 0;
        rwData->swing.swingUpdateRate = roData->yawUpateRateTarget;
        rwData->unk_1C = playerPosRot->pos.y - camera->playerPosDelta.y;
        rwData->unk_20 = eyeAtOffset.r;
        camera->playerToAtOffset.y -= camera->playerPosDelta.y;
        camera->xzOffsetUpdateRate = (1.0f / 10000.0f);
        camera->animState++;
    }

    if (rwData->swing.swingUpdateRateTimer != 0) {
        camera->yawUpdateRateInv = f_cush(roData->yawUpateRateTarget + rwData->swing.swingUpdateRateTimer,
                                                    camera->yawUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_Y, 0.1f);
        camera->pitchUpdateRateInv =
            f_cush((f32)CAM_PITCH_UPDATE_RATE_INV + rwData->swing.swingUpdateRateTimer,
                             camera->pitchUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_Y, 0.1f);
        rwData->swing.swingUpdateRateTimer--;
    } else {
        camera->yawUpdateRateInv =
            f_cush(roData->yawUpateRateTarget, camera->yawUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_Y, 0.1f);
        camera->pitchUpdateRateInv = f_cush((f32)CAM_PITCH_UPDATE_RATE_INV, camera->pitchUpdateRateInv,
                                                      CAM_UPDATE_RATE_STEP_SCALE_Y, 0.1f);
    }

    camera->xzOffsetUpdateRate =
        f_cush(CAM_XZ_OFFSET_UPDATE_RATE, camera->xzOffsetUpdateRate, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
    camera->yOffsetUpdateRate =
        f_cush(CAM_Y_OFFSET_UPDATE_RATE, camera->yOffsetUpdateRate, CAM_UPDATE_RATE_STEP_SCALE_Y, 0.1f);
    camera->fovUpdateRate = f_cush(CAM_FOV_UPDATE_RATE, camera->yOffsetUpdateRate, 0.05f, 0.1f);

    center4jump(camera, &eyeNextAtOffset, roData->atYOffset, &rwData->unk_1C, false);

    eyeDiffGeo = eyeAtOffset;

    eyeDiffTarget = sglobe_by_2pos(at, eye);

    eyeDiffGeo.r = f_cush(eyeDiffTarget.r, eyeAtOffset.r, CAM_GLOBAL_29, 1.0f);
    eyeDiffGeo.pitch = s_cush(eyeDiffTarget.pitch, eyeAtOffset.pitch, CAM_GLOBAL_29, 0xA);

    if (rwData->swing.unk_18) {
        eyeDiffGeo.yaw =
            s_cush(rwData->swing.unk_16, eyeNextAtOffset.yaw, 1.0f / camera->yawUpdateRateInv, 0xA);
        eyeDiffGeo.pitch =
            s_cush(rwData->swing.unk_14, eyeNextAtOffset.pitch, 1.0f / camera->yawUpdateRateInv, 0xA);
    } else {
        eyeDiffGeo.yaw =
            calc_default_longitude(camera, eyeNextAtOffset.yaw, camera->playerPosRot.rot.y, roData->maxYawUpdate, 0.0f);
    }

    // Clamp the eye->at distance to roData->distMin < eyeDiffGeo.r < roData->distMax
    if (eyeDiffGeo.r < roData->distMin) {
        eyeDiffGeo.r = roData->distMin;
    } else if (eyeDiffGeo.r > roData->distMax) {
        eyeDiffGeo.r = roData->distMax;
    }

    // Clamp the phi rotation at CAM_MAX_PITCH AND CAM_MIN_PITCH_2
    if (eyeDiffGeo.pitch > CAM_MAX_PITCH) {
        eyeDiffGeo.pitch = CAM_MAX_PITCH;
    } else if (eyeDiffGeo.pitch < CAM_MIN_PITCH_2) {
        eyeDiffGeo.pitch = CAM_MIN_PITCH_2;
    }

    newEye = translate_by_sglobe(at, &eyeDiffGeo);
    eyeNext->x = newEye.x;
    eyeNext->z = newEye.z;
    eyeNext->y += (newEye.y - eyeNext->y) * CAM_JUMP1_EYE_Y_STEP_SCALE;
    if ((camera->status == CAM_STAT_ACTIVE) && !(roData->interfaceField & JUMP1_FLAG_4)) {
        default_bgcheck_proc(camera, &eyeDiffGeo, roData->distMin, roData->yawUpateRateTarget, &spA4, &rwData->swing);
        if (roData->interfaceField & JUMP1_FLAG_2) {
            camera->inputDir.x = -eyeAtOffset.pitch;
            camera->inputDir.y = eyeAtOffset.yaw - 0x7FFF;
            camera->inputDir.z = 0;
        } else {
            eyeDiffGeo = sglobe_by_2pos(eye, at);
            camera->inputDir.x = eyeDiffGeo.pitch;
            camera->inputDir.y = eyeDiffGeo.yaw;
            camera->inputDir.z = 0;
        }
        if (rwData->swing.unk_18) {
            camera->inputDir.y =
                s_cush(camera->inputDir.y + (s16)((s16)(rwData->swing.unk_16 - 0x7FFF) - camera->inputDir.y),
                                 camera->inputDir.y, 1.0f - (0.99f * spA4), 0xA);
        }
    } else {
        rwData->swing.swingUpdateRate = roData->yawUpateRateTarget;
        rwData->swing.unk_18 = 0;
        cutflag = 0;
        *eye = *eyeNext;
    }

    camera->dist = distance_between(at, eye);
    camera->roll = s_cush(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = recover_shift(camera, roData->atLERPScaleMax);
    return true;
}

// Climbing ladders/vines
s32 jump_camerawork_02(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f bgChkPos;
    Vec3f floorNorm;
    VecGeo adjAtToEyeDir;
    VecGeo bgChkPara;
    VecGeo atToEyeNextDir;
    UNUSED VecGeo atToEyeDir;
    f32 temp_f14;
    f32 temp_f16;
    f32 sp90;
    f32 sp8C;
    s32 bgId;
    CamColChk camBgChk;
    PosRot* playerPosRot = &camera->playerPosRot;
    s16 yawDiff;
    s16 playerYawRot180;
    Jump2ReadOnlyData* roData = &camera->paramData.jump2.roData;
    Jump2ReadWriteData* rwData = &camera->paramData.jump2.rwData;
    s32 pad;
    f32 playerHeight;

    playerHeight = player_get_tall(camera->player);

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

        sp90 = (camera->playerPosDelta.y > 0.0f) ? -10.0f : 10.0f;
        roData->atYOffset = CAM_DATA_SCALED(sp90 + GET_NEXT_RO_DATA(values)) * playerHeight * yNormal;
        roData->minDist = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->maxDist = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->minMaxDistFactor = GET_NEXT_SCALED_RO_DATA(values);
        roData->yawUpdRateTarget = GET_NEXT_RO_DATA(values);
        roData->xzUpdRateTarget = GET_NEXT_SCALED_RO_DATA(values);
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->atLERPStepScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    atToEyeDir = sglobe_by_2pos(at, eye);
    atToEyeNextDir = sglobe_by_2pos(at, eyeNext);

    shrink_and_bitem = roData->interfaceField;

    if (RELOAD_PARAMS(camera)) {
        bgChkPos = playerPosRot->pos;
        rwData->floorY = floor_at_light(camera, &bgChkPos);
        rwData->yawTarget = atToEyeNextDir.yaw;
        rwData->initYawDiff = 0;
        if (rwData->floorY == BGCHECK_Y_MIN) {
            PRINTF(VT_COL(YELLOW, BLACK) "camera: climb: no floor \n" VT_RST);
            rwData->onFloor = -1;
            rwData->floorY = playerPosRot->pos.y - 1000.0f;
        } else if (playerPosRot->pos.y - rwData->floorY < playerHeight) {
            // player's model is within the height of the floor.
            rwData->onFloor = 1;
        } else {
            rwData->onFloor = -1;
        }

        yawDiff = (s16)(playerPosRot->rot.y - 0x7FFF) - atToEyeNextDir.yaw;
        rwData->initYawDiff = ((yawDiff / CAM_DEFAULT_ANIM_TIME) / 4) * 3;
        if (roData->interfaceField & JUMP2_FLAG_1) {
            rwData->yawAdj = 0xA;
        } else {
            rwData->yawAdj = 0x2710;
        }

        playerPosRot->pos.x -= camera->playerPosDelta.x;
        playerPosRot->pos.y -= camera->playerPosDelta.y;
        playerPosRot->pos.z -= camera->playerPosDelta.z;
        rwData->animTimer = CAM_DEFAULT_ANIM_TIME;
        camera->animState++;
        camera->atLERPStepScale = roData->atLERPStepScale;
    }

    sp90 = CAM_UPDATE_RATE_STEP_SCALE_XZ * camera->speedRatio;
    sp8C = CAM_UPDATE_RATE_STEP_SCALE_Y * camera->speedRatio;
    camera->yawUpdateRateInv = f_cush(roData->yawUpdRateTarget, camera->yawUpdateRateInv, sp90, 0.1f);
    camera->xzOffsetUpdateRate = f_cush(roData->xzUpdRateTarget, camera->xzOffsetUpdateRate, sp90, 0.1f);
    camera->yOffsetUpdateRate = f_cush(CAM_Y_OFFSET_UPDATE_RATE, camera->yOffsetUpdateRate, sp8C, 0.1f);

    camera->fovUpdateRate =
        f_cush(CAM_FOV_UPDATE_RATE, camera->yOffsetUpdateRate, camera->speedRatio * 0.05f, 0.1f);
    camera->rUpdateRateInv = CAM_GLOBAL_27;

    center4normal(camera, &atToEyeNextDir, roData->atYOffset, false);
    adjAtToEyeDir = sglobe_by_2pos(at, eye);

    temp_f16 = roData->minDist;
    sp90 = roData->maxDist + (roData->maxDist * roData->minMaxDistFactor);
    temp_f14 = temp_f16 - (roData->minDist * roData->minMaxDistFactor);

    if (adjAtToEyeDir.r > sp90) {
        adjAtToEyeDir.r = sp90;
    } else if (adjAtToEyeDir.r < temp_f14) {
        adjAtToEyeDir.r = temp_f14;
    }

    yawDiff = (s16)(playerPosRot->rot.y - 0x7FFF) - adjAtToEyeDir.yaw;
    if (rwData->animTimer != 0) {
        rwData->yawTarget = playerPosRot->rot.y - 0x7FFF;
        rwData->animTimer--;
        adjAtToEyeDir.yaw = s_cush(rwData->yawTarget, atToEyeNextDir.yaw, 0.5f, 0xA);
    } else if (rwData->yawAdj < ABS(yawDiff)) {
        playerYawRot180 = playerPosRot->rot.y - 0x7FFF;
        adjAtToEyeDir.yaw = s_cush2(
            ((yawDiff < 0) ? (s16)(playerYawRot180 + rwData->yawAdj) : (s16)(playerYawRot180 - rwData->yawAdj)),
            atToEyeNextDir.yaw, 0.1f, 0xA);
    } else {
        adjAtToEyeDir.yaw = s_cush(adjAtToEyeDir.yaw, atToEyeNextDir.yaw, 0.25f, 0xA);
    }

    // Check the floor at the top of the climb
    bgChkPos.x = playerPosRot->pos.x + (sin_s(playerPosRot->rot.y) * 25.0f);
    bgChkPos.y = playerPosRot->pos.y + (playerHeight * 2.2f);
    bgChkPos.z = playerPosRot->pos.z + (cos_s(playerPosRot->rot.y) * 25.0f);

    sp90 = floor_at(camera, &floorNorm, &bgChkPos, &bgId);
    if ((sp90 != BGCHECK_Y_MIN) && (playerPosRot->pos.y < sp90)) {
        // top of the climb is within 2.2x of the player's height.
        camera->pitchUpdateRateInv =
            f_cush(20.0f, camera->pitchUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_Y, 0.1f);
        camera->rUpdateRateInv = f_cush(20.0f, camera->rUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_Y, 0.1f);
        adjAtToEyeDir.pitch = s_cush(0x1F4, atToEyeNextDir.pitch, 1.0f / camera->pitchUpdateRateInv, 0xA);
    } else if ((playerPosRot->pos.y - rwData->floorY) < playerHeight) {
        // player is within his height of the ground.
        camera->pitchUpdateRateInv =
            f_cush(20.0f, camera->pitchUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_Y, 0.1f);
        camera->rUpdateRateInv = f_cush(20.0f, camera->rUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_Y, 0.1f);
        adjAtToEyeDir.pitch = s_cush(0x1F4, atToEyeNextDir.pitch, 1.0f / camera->pitchUpdateRateInv, 0xA);
    } else {
        camera->pitchUpdateRateInv = 100.0f;
        camera->rUpdateRateInv = 100.0f;
    }

    // max pitch to +/- ~ 60 degrees
    if (adjAtToEyeDir.pitch > 0x2AF8) {
        adjAtToEyeDir.pitch = 0x2AF8;
    }

    if (adjAtToEyeDir.pitch < -0x2AF8) {
        adjAtToEyeDir.pitch = -0x2AF8;
    }

    *eyeNext = translate_by_sglobe(at, &adjAtToEyeDir);
    camBgChk.pos = *eyeNext;
    if (check_wall(camera, at, &camBgChk)) {
        // Collision detected between at->eyeNext, Check if collision between
        // at->eyeNext, but parallel to at (pitch = 0).
        bgChkPos = camBgChk.pos;
        bgChkPara.r = adjAtToEyeDir.r;
        bgChkPara.pitch = 0;
        bgChkPara.yaw = adjAtToEyeDir.yaw;
        camBgChk.pos = translate_by_sglobe(at, &bgChkPara);
        if (check_wall(camera, at, &camBgChk)) {
            // Collision found between parallel at->eyeNext, set eye position to
            // first collision point.
            *eye = bgChkPos;
        } else {
            // no collision found with the parallel at->eye, animate to be parallel
            adjAtToEyeDir.pitch = s_cush(0, adjAtToEyeDir.pitch, 0.2f, 0xA);
            *eye = translate_by_sglobe(at, &adjAtToEyeDir);
            // useless?
            check_wall_light(camera, at, eye);
        }
    } else {
        // no collision detected.
        *eye = *eyeNext;
    }

    camera->dist = adjAtToEyeDir.r;
    camera->fov = f_cush(roData->fovTarget, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = s_cush(0, camera->roll, 0.5f, 0xA);
    return true;
}

// swimming
s32 jump_camerawork_03(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    s32 prevMode;
    f32 spC4;
    f32 spC0;
    f32 spBC;
    UNUSED Vec3f spB0;
    VecGeo eyeDiffGeo;
    PosRot* playerPosRot = &camera->playerPosRot;
    Jump3ReadOnlyData* roData = &camera->paramData.jump3.roData;
    VecGeo eyeAtOffset;
    VecGeo eyeNextAtOffset;
    s32 pad;
    s32 pad2;
    CameraModeValue* values;
    f32 t2;
    f32 phi_f0;
    f32 phi_f2;
    f32 playerHeight;
    PosRot playerhead;
    f32 yNormal;
    f32 temp_f18;
    s32 modeSwitch;
    f32 temp_f2_2;
    Jump3ReadWriteData* rwData = &camera->paramData.jump3.rwData;

    playerHeight = player_get_tall(camera->player);
    playerhead = Actor_get_eye(&camera->player->actor);

    modeSwitch = false;
    if (((camera->waterYPos - eye->y) < CAM_GLOBAL_44 || (camera->animState == 0))) {
        if (rwData->mode != CAM_MODE_NORMAL) {
            rwData->mode = CAM_MODE_NORMAL;
            modeSwitch = true;
        }
    } else if (((camera->waterYPos - eye->y) > CAM_GLOBAL_45) && (rwData->mode != CAM_MODE_AIM_BOOMERANG)) {
        rwData->mode = CAM_MODE_AIM_BOOMERANG;
        modeSwitch = true;
    }

    eyeAtOffset = sglobe_by_2pos(at, eye);
    eyeNextAtOffset = sglobe_by_2pos(at, eyeNext);

    if (RELOAD_PARAMS(camera) || modeSwitch || CAM_DEBUG_RELOAD_PARAMS) {
        values = cam_ctl_tbl_0[camera->setting].cameraModes[rwData->mode].values;
        yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));
        t2 = CAM_DATA_SCALED(playerHeight) * yNormal;
        roData->yOffset = GET_NEXT_RO_DATA(values) * t2;
        roData->distMin = GET_NEXT_RO_DATA(values) * t2;
        roData->distMax = GET_NEXT_RO_DATA(values) * t2;
        roData->pitchTarget = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->swingUpdateRate = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_SCALED_RO_DATA(values);
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->unk_1C = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

#if DEBUG_FEATURES
    if (R_RELOAD_CAM_PARAMS) {
        prevMode = camera->mode;
        camera->mode = rwData->mode;
        copyCameraPosPREGToModeValues(camera);
        camera->mode = prevMode;
    }
#endif

    shrink_and_bitem = roData->interfaceField;

    switch (camera->animState) {
        case 0:
        case 10:
        case 20:
        case 25:
            rwData->swing.atEyePoly = NULL;
            rwData->unk_1C = camera->playerGroundY;
            rwData->swing.unk_16 = rwData->swing.unk_14 = rwData->swing.unk_18 = 0;
            rwData->animTimer = 10;
            rwData->swing.swingUpdateRate = roData->swingUpdateRate;
            camera->animState++;
            rwData->swing.swingUpdateRateTimer = 0;
            break;
        default:
            if (rwData->animTimer != 0) {
                rwData->animTimer--;
            }
            break;
    }

    spB0 = *eye;

    spC4 = CAM_UPDATE_RATE_STEP_SCALE_XZ * camera->speedRatio;
    spC0 = camera->speedRatio * CAM_UPDATE_RATE_STEP_SCALE_Y;
    spBC = rwData->swing.unk_18 != 0 ? CAM_UPDATE_RATE_STEP_SCALE_XZ : spC4;

    if (rwData->swing.swingUpdateRateTimer != 0) {
        camera->yawUpdateRateInv =
            f_cush(rwData->swing.swingUpdateRate + (rwData->swing.swingUpdateRateTimer * 2),
                             camera->yawUpdateRateInv, spC4, 0.1f);
        camera->pitchUpdateRateInv =
            f_cush((rwData->swing.swingUpdateRateTimer * 2) + 40.0f, camera->pitchUpdateRateInv, spC0, 0.1f);
        rwData->swing.swingUpdateRateTimer--;
    } else {
        camera->yawUpdateRateInv =
            f_cush(rwData->swing.swingUpdateRate, camera->yawUpdateRateInv, spBC, 0.1f);
        camera->pitchUpdateRateInv = f_cush(40.0f, camera->pitchUpdateRateInv, spC0, 0.1f);
    }

    camera->xzOffsetUpdateRate = f_cush(CAM_XZ_OFFSET_UPDATE_RATE, camera->xzOffsetUpdateRate, spC4, 0.1f);
    camera->yOffsetUpdateRate = f_cush(CAM_Y_OFFSET_UPDATE_RATE, camera->yOffsetUpdateRate, spC0, 0.1f);
    camera->fovUpdateRate =
        f_cush(CAM_FOV_UPDATE_RATE, camera->yOffsetUpdateRate, camera->speedRatio * 0.05f, 0.1f);

    center4normal(camera, &eyeNextAtOffset, roData->yOffset, roData->interfaceField);
    eyeDiffGeo = sglobe_by_2pos(at, eyeNext);

    camera->dist = eyeDiffGeo.r =
        calc_default_radius2(camera, eyeDiffGeo.r, roData->distMin, roData->distMax, rwData->animTimer);

    if (camera->playerGroundY <= playerPosRot->pos.y) {
        phi_f0 = playerPosRot->pos.y - camera->playerGroundY;
    } else {
        phi_f0 = -(playerPosRot->pos.y - camera->playerGroundY);
    }

    if (!(phi_f0 < 10.0f)) {
        if (camera->waterYPos <= playerhead.pos.y) {
            phi_f2 = playerhead.pos.y - camera->waterYPos;
        } else {
            phi_f2 = -(playerhead.pos.y - camera->waterYPos);
        }
        if (!(phi_f2 < 50.0f)) {
            camera->pitchUpdateRateInv = 100.0f;
        }
    }
    if (rwData->swing.unk_18 != 0) {
        eyeDiffGeo.yaw =
            s_cush(rwData->swing.unk_16, eyeNextAtOffset.yaw, 1.0f / camera->yawUpdateRateInv, 0xA);
        eyeDiffGeo.pitch =
            s_cush(rwData->swing.unk_14, eyeNextAtOffset.pitch, 1.0f / camera->yawUpdateRateInv, 0xA);
    } else {
        eyeDiffGeo.yaw = calc_default_longitude(camera, eyeNextAtOffset.yaw, playerPosRot->rot.y, roData->unk_14, 0.0f);
        eyeDiffGeo.pitch = calc_default_latitude(camera, eyeNextAtOffset.pitch, roData->pitchTarget, 0);
    }

    if (eyeDiffGeo.pitch > CAM_MAX_PITCH) {
        eyeDiffGeo.pitch = CAM_MAX_PITCH;
    }

    if (eyeDiffGeo.pitch < CAM_MIN_PITCH_1) {
        eyeDiffGeo.pitch = CAM_MIN_PITCH_1;
    }

    *eyeNext = translate_by_sglobe(at, &eyeDiffGeo);
    if ((camera->status == CAM_STAT_ACTIVE) && !(roData->interfaceField & JUMP3_FLAG_4)) {
        default_bgcheck_proc(camera, &eyeDiffGeo, roData->distMin, roData->swingUpdateRate, &spBC, &rwData->swing);
        if (roData->interfaceField & JUMP3_FLAG_2) {
            camera->inputDir.x = -eyeAtOffset.pitch;
            camera->inputDir.y = eyeAtOffset.yaw - 0x7FFF;
            camera->inputDir.z = 0;
        } else {
            eyeDiffGeo = sglobe_by_2pos(eye, at);
            camera->inputDir.x = eyeDiffGeo.pitch;
            camera->inputDir.y = eyeDiffGeo.yaw;
            camera->inputDir.z = 0;
        }

        if (rwData->swing.unk_18 != 0) {
            camera->inputDir.y =
                s_cush(camera->inputDir.y + (s16)((s16)(rwData->swing.unk_16 - 0x7FFF) - camera->inputDir.y),
                                 camera->inputDir.y, 1.0f - (0.99f * spBC), 0xA);
        }
    } else {
        rwData->swing.swingUpdateRate = roData->swingUpdateRate;
        rwData->swing.unk_18 = 0;
        cutflag = 0;
        *eye = *eyeNext;
    }
    camera->fov = f_cush(roData->fovTarget, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = s_cush(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = recover_shift(camera, roData->unk_1C);
    return true;
}

s32 jump_camerawork_04(Camera* camera) {
    return NOTUSED(camera);
}

s32 jump_camerawork_00(Camera* camera) {
    return NOTUSED(camera);
}

s32 battle_camerawork_01(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f sp128;
    Vec3f playerHead;
    Vec3f targetPos;
    f32 var3;
    f32 var2;
    f32 temp_f0_2;
    f32 temp_f12_2;
    f32 spFC;
    f32 spF8;
    f32 swingAngle;
    f32 temp_f2_2;
    f32 temp_f14;
    s32 skipEyeAtCalc;
    f32 distRatio;
    CamColChk spBC;
    VecGeo spB4;
    VecGeo atToTargetDir;
    VecGeo playerToTargetDir;
    VecGeo atToEyeDir;
    VecGeo atToEyeNextDir;
    PosRot* playerPosRot = &camera->playerPosRot;
    s16 tmpAng1;
    s16 tmpAng2;
    Player* player;
    s16 sp86;
    s16 isOffGround;
    f32 distance;
    f32 sp7C;
    f32 sp78;
    f32 fov;
    Battle1ReadOnlyData* roData = &camera->paramData.batt1.roData;
    Battle1ReadWriteData* rwData = &camera->paramData.batt1.rwData;
    s32 pad;
    f32 playerHeight;

    skipEyeAtCalc = false;
    player = camera->player;
    playerHeight = player_get_tall(camera->player);
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->distance = GET_NEXT_RO_DATA(values);
        roData->swingYawInitial = GET_NEXT_RO_DATA(values);
        roData->swingYawFinal = GET_NEXT_RO_DATA(values);
        roData->swingPitchInitial = GET_NEXT_RO_DATA(values);
        roData->swingPitchFinal = GET_NEXT_RO_DATA(values);
        roData->swingPitchAdj = GET_NEXT_SCALED_RO_DATA(values);
        roData->fov = GET_NEXT_RO_DATA(values);
        roData->atLERPScaleOnGround = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
        roData->yOffsetOffGround = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->atLERPScaleOffGround = GET_NEXT_SCALED_RO_DATA(values);
        rwData->chargeTimer = 40;
        rwData->unk_10 = CAM_GLOBAL_12;
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    distance = roData->distance;
    sp7C = roData->swingPitchInitial;
    sp78 = roData->swingPitchFinal;
    fov = roData->fov;

    if (camera->player->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) {
        // charging sword.
        rwData->unk_10 =
            f_cush(CAM_GLOBAL_12 * (1.0f - 0.5f), rwData->unk_10, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
        camera->xzOffsetUpdateRate =
            f_cush(0.2f, camera->xzOffsetUpdateRate, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
        camera->yOffsetUpdateRate =
            f_cush(0.2f, camera->yOffsetUpdateRate, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
        if (rwData->chargeTimer > -20) {
            rwData->chargeTimer--;
        } else {
            distance = 250.0f;
            sp7C = 50.0f;
            sp78 = 40.0f;
            fov = 60.0f;
        }
    } else if (rwData->chargeTimer < 0) {
        distance = 250.0f;
        sp7C = 50.0f;
        sp78 = 40.0f;
        fov = 60.0f;
        rwData->chargeTimer++;
    } else {
        rwData->chargeTimer = 40;
        rwData->unk_10 = f_cush(CAM_GLOBAL_12, rwData->unk_10, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
        camera->xzOffsetUpdateRate =
            f_cush(CAM_BATTLE1_XYZ_OFFSET_UPDATE_RATE_TARGET, camera->xzOffsetUpdateRate,
                             CAM_UPDATE_RATE_STEP_SCALE_XZ * camera->speedRatio, 0.1f);
        camera->yOffsetUpdateRate =
            f_cush(CAM_BATTLE1_XYZ_OFFSET_UPDATE_RATE_TARGET, camera->yOffsetUpdateRate,
                             CAM_UPDATE_RATE_STEP_SCALE_Y * camera->speedRatio, 0.1f);
    }
    camera->fovUpdateRate =
        f_cush(CAM_FOV_UPDATE_RATE, camera->fovUpdateRate, camera->speedRatio * 0.05f, 0.1f);
    playerHeight += roData->yOffset;
    atToEyeDir = sglobe_by_2pos(at, eye);
    atToEyeNextDir = sglobe_by_2pos(at, eyeNext);
    if (camera->target == NULL || camera->target->update == NULL) {
        if (camera->target == NULL) {
            PRINTF(VT_COL(YELLOW, BLACK) "camera: warning: battle: target is not valid, change parallel\n" VT_RST);
        }
        camera->target = NULL;
        changeCameraMode(camera, CAM_MODE_Z_PARALLEL);
        return true;
    }

    shrink_and_bitem = roData->interfaceField;

    if (RELOAD_PARAMS(camera)) {
        rwData->unk_14 = 0;
        rwData->roll = 0.0f;
        rwData->target = camera->target;
        camera->animState++;

#if DEBUG_FEATURES
        if (rwData->target->id > 0) {
            PRINTF("camera: battle: target actor name " VT_FGCOL(BLUE) "%d" VT_RST "\n", rwData->target->id);
        } else {
            PRINTF("camera: battle: target actor name " VT_COL(RED, WHITE) "%d" VT_RST "\n", rwData->target->id);
            camera->target = NULL;
            changeCameraMode(camera, CAM_MODE_Z_PARALLEL);
            return true;
        }
#endif

        rwData->animTimer = CAM_DEFAULT_ANIM_TIME + CAM_GLOBAL_24;
        rwData->initialEyeToAtYaw = atToEyeDir.yaw;
        rwData->initialEyeToAtPitch = atToEyeDir.pitch;
        rwData->initialEyeToAtDist = atToEyeDir.r;
        rwData->yPosOffset = playerPosRot->pos.y - camera->playerPosDelta.y;
    }

    if (camera->status == CAM_STAT_ACTIVE) {
        cutflag = 1;
        camera->inputDir.x = -atToEyeDir.pitch;
        camera->inputDir.y = atToEyeDir.yaw - 0x7FFF;
        camera->inputDir.z = 0;
    }

    if (camera->playerGroundY == camera->playerPosRot.pos.y || camera->player->actor.gravity > -0.1f ||
        camera->player->stateFlags1 & PLAYER_STATE1_21) {
        isOffGround = false;
        rwData->yPosOffset = playerPosRot->pos.y;
    } else {
        isOffGround = true;
    }

    if (rwData->animTimer == 0) {
        camera->atLERPStepScale =
            recover_shift(camera, isOffGround ? roData->atLERPScaleOffGround : roData->atLERPScaleOnGround);
    }
    camera->targetPosRot = Actor_get_eye(camera->target);
    if (rwData->target != camera->target) {
        PRINTF("camera: battle: change target %d -> " VT_FGCOL(BLUE) "%d" VT_RST "\n", rwData->target->id,
               camera->target->id);
        camera->animState = 0;
        return true;
    }

    center4keepon(camera, &atToEyeNextDir, &camera->targetPosRot.pos,
                           isOffGround ? roData->yOffsetOffGround : roData->yOffset, distance, &rwData->yPosOffset,
                           &playerToTargetDir,
                           (isOffGround ? (CAM_LOCKON_AT_FLAG_OFF_GROUND | CAM_LOCKON_AT_FLAG_CALC_SLOPE_Y_ADJ)
                                        : CAM_LOCKON_AT_FLAG_CALC_SLOPE_Y_ADJ) |
                               roData->interfaceField);
    tmpAng2 = playerToTargetDir.yaw;
    playerHead = playerPosRot->pos;
    playerHead.y += playerHeight;
    playerToTargetDir = sglobe_by_2pos(&playerHead, &camera->targetPosRot.pos);
    distRatio = playerToTargetDir.r > distance ? 1.0f : playerToTargetDir.r / distance;
    targetPos = camera->targetPosRot.pos;
    atToTargetDir = sglobe_by_2pos(at, &targetPos);
    atToTargetDir.r = distance - ((atToTargetDir.r <= distance ? atToTargetDir.r : distance) * 0.5f);
    swingAngle = roData->swingYawInitial + ((roData->swingYawFinal - roData->swingYawInitial) * (1.1f - distRatio));
    spF8 = swingAngle + CAM_GLOBAL_13;

    spB4.r = camera->dist = f_cush(distance, camera->dist, CAM_GLOBAL_11, 2.0f);
    spB4.yaw = atToEyeNextDir.yaw;
    tmpAng1 = (s16)(atToTargetDir.yaw - (s16)(atToEyeNextDir.yaw - 0x7FFF));
    if (rwData->animTimer != 0) {
        if (rwData->animTimer >= CAM_GLOBAL_24) {
            sp86 = rwData->animTimer - CAM_GLOBAL_24;
            playerToTargetDir = sglobe_by_2pos(at, eye);
            playerToTargetDir.yaw = tmpAng2 - 0x7FFF;

            var2 = 1.0f / CAM_DEFAULT_ANIM_TIME;
            var3 = (rwData->initialEyeToAtDist - playerToTargetDir.r) * var2;
            tmpAng1 = (s16)(rwData->initialEyeToAtYaw - playerToTargetDir.yaw) * var2;
            tmpAng2 = (s16)(rwData->initialEyeToAtPitch - playerToTargetDir.pitch) * var2;

            spB4.r = f_cush(playerToTargetDir.r + (var3 * sp86), atToEyeDir.r, CAM_GLOBAL_28, 1.0f);
            spB4.yaw = s_cush(playerToTargetDir.yaw + (tmpAng1 * sp86), atToEyeDir.yaw, CAM_GLOBAL_28, 0xA);
            spB4.pitch =
                s_cush(playerToTargetDir.pitch + (tmpAng2 * sp86), atToEyeDir.pitch, CAM_GLOBAL_28, 0xA);
        } else {
            skipEyeAtCalc = true;
        }
        rwData->animTimer--;
    } else if (ABS(tmpAng1) > CAM_DEG_TO_BINANG(swingAngle)) {
        spFC = CAM_BINANG_TO_DEG(tmpAng1);
        temp_f2_2 = swingAngle + (spF8 - swingAngle) * (limiter(atToTargetDir.r, spB4.r) / spB4.r);
        temp_f12_2 = ((temp_f2_2 * temp_f2_2) - 2.0f) / (temp_f2_2 - 360.0f);
        var2 = ((temp_f12_2 * spFC) + (2.0f - (360.0f * temp_f12_2)));
        temp_f14 = SQ(spFC) / var2;
        tmpAng2 = tmpAng1 >= 0 ? CAM_DEG_TO_BINANG(temp_f14) : (-CAM_DEG_TO_BINANG(temp_f14));
        spB4.yaw = (s16)((s16)(atToEyeNextDir.yaw - 0x7FFF) + tmpAng2) - 0x7FFF;
    } else {
        spFC = 0.05f;
        spFC = (1.0f - camera->speedRatio) * spFC;
        tmpAng2 = tmpAng1 >= 0 ? CAM_DEG_TO_BINANG(swingAngle) : -CAM_DEG_TO_BINANG(swingAngle);
        spB4.yaw = atToEyeNextDir.yaw - (s16)((tmpAng2 - tmpAng1) * spFC);
    }

    if (!skipEyeAtCalc) {
        var3 = atToTargetDir.pitch * roData->swingPitchAdj;
        var2 = F32_LERPIMP(sp7C, sp78, distRatio);
        tmpAng1 =
            CAM_DEG_TO_BINANG(var2) - (s16)(playerToTargetDir.pitch * ((1.0f - 0.5f) + distRatio * (1.0f - 0.5f)));
        tmpAng1 += (s16)(var3);

        if (tmpAng1 < -0x2AA8) {
            tmpAng1 = -0x2AA8;
        } else if (tmpAng1 > 0x2AA8) {
            tmpAng1 = 0x2AA8;
        }

        spB4.pitch = s_cush(tmpAng1, atToEyeNextDir.pitch, rwData->unk_10, 0xA);
        *eyeNext = translate_by_sglobe(at, &spB4);
        spBC.pos = *eyeNext;
        if (camera->status == CAM_STAT_ACTIVE) {
            if (!camera->play->envCtx.skyboxDisabled || roData->interfaceField & BATTLE1_FLAG_0) {
                check_wall(camera, at, &spBC);
            } else if (roData->interfaceField & BATTLE1_FLAG_1) {
                check_not_wall_bgc(camera, at, &spBC);
            } else {
                sp128 = unitvector_by_2pos(at, &spBC.pos);
                spBC.pos.x -= sp128.x;
                spBC.pos.y -= sp128.y;
                spBC.pos.z -= sp128.z;
            }
            *eye = spBC.pos;
        } else {
            *eye = *eyeNext;
        }
    }
    rwData->roll += ((CAM_BATTLE1_ROLL_TARGET_BASE * camera->speedRatio * (1.0f - distRatio)) - rwData->roll) *
                    CAM_BATTLE1_ROLL_STEP_SCALE;
    camera->roll = CAM_DEG_TO_BINANG(rwData->roll);
    camera->fov = f_cush((player->meleeWeaponState != 0                      ? 0.8f
                                    : z_common_data.save.info.playerData.health <= 0x10 ? 0.8f
                                                                                       : 1.0f) *
                                       (fov - ((fov * 0.05f) * distRatio)),
                                   camera->fov, camera->fovUpdateRate, 1.0f);
    //! @bug Missing return, but the return value is not used.
}

s32 battle_camerawork_02(Camera* camera) {
    return NOTUSED(camera);
}

s32 battle_camerawork_03(Camera* camera) {
    return NOTUSED(camera);
}

/**
 * Charging spin attack
 * Camera zooms out slowly for 50 frames, then tilts up to a specified
 * setting value.
 */
s32 battle_camerawork_04(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    VecGeo eyeNextOffset;
    VecGeo eyeAtOffset;
    VecGeo eyeNextAtOffset;
    Battle4ReadOnlyData* roData = &camera->paramData.batt4.roData;
    Battle4ReadWriteData* rwData = &camera->paramData.batt4.rwData;
    s32 pad;
    f32 playerHeight;

    playerHeight = player_get_tall(camera->player);
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->rTarget = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->pitchTarget = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->lerpUpdateRate = GET_NEXT_SCALED_RO_DATA(values);
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->atLERPTarget = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    eyeAtOffset = sglobe_by_2pos(at, eye);
    eyeNextAtOffset = sglobe_by_2pos(at, eyeNext);

    shrink_and_bitem = roData->interfaceField;

    switch (camera->animState) {
        case 0:
        case 10:
        case 20:
            rwData->animTimer = 50;
            camera->animState++;
            break;
    }

    camera->yawUpdateRateInv = f_cush(roData->lerpUpdateRate, camera->yawUpdateRateInv,
                                                CAM_UPDATE_RATE_STEP_SCALE_XZ * camera->speedRatio, 0.1f);
    camera->rUpdateRateInv = 1000.0f;
    camera->pitchUpdateRateInv = 1000.0f;
    camera->xzOffsetUpdateRate =
        f_cush(0.025f, camera->xzOffsetUpdateRate, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
    camera->yOffsetUpdateRate = f_cush(CAM_Y_OFFSET_UPDATE_RATE, camera->yOffsetUpdateRate,
                                                 CAM_UPDATE_RATE_STEP_SCALE_Y * camera->speedRatio, 0.1f);
    camera->fovUpdateRate = 0.0001f;
    center4normal(camera, &eyeNextAtOffset, roData->yOffset, true);
    if (rwData->animTimer != 0) {
        eyeNextOffset.yaw = eyeAtOffset.yaw;
        eyeNextOffset.pitch = eyeAtOffset.pitch;
        eyeNextOffset.r = eyeAtOffset.r;
        rwData->animTimer--;
    } else {
        eyeNextOffset.yaw = eyeAtOffset.yaw;
        eyeNextOffset.pitch = s_cush(roData->pitchTarget, eyeAtOffset.pitch, roData->lerpUpdateRate, 2);
        eyeNextOffset.r = f_cush(roData->rTarget, eyeAtOffset.r, roData->lerpUpdateRate, 0.001f);
    }
    *eyeNext = translate_by_sglobe(at, &eyeNextOffset);
    *eye = *eyeNext;
    camera->dist = eyeNextOffset.r;
    camera->fov = f_cush(roData->fovTarget, camera->fov, roData->lerpUpdateRate, 1.0f);
    camera->roll = 0;
    camera->atLERPStepScale = recover_shift(camera, roData->atLERPTarget);
    return true;
}

s32 battle_camerawork_00(Camera* camera) {
    return NOTUSED(camera);
}

// Targeting non-enemy
s32 keepon_camerawork_01(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f sp120;
    Vec3f sp114;
    Vec3f sp108;
    f32 sp104;
    f32 temp_f12_2;
    f32 temp_f14;
    f32 t1;
    f32 spF4;
    f32 spF0;
    f32 spEC;
    f32 spE8;
    f32 t2;
    s16 spE2;
    s16 spE0;
    VecGeo spD8;
    VecGeo spD0;
    VecGeo spC8;
    VecGeo spC0;
    VecGeo spB8;
    PosRot* playerPosRot = &camera->playerPosRot;
    CamColChk sp8C;
    s32 sp88;
    f32 sp84;
    s16 sp82;
    s16 isOffGround;
    KeepOn1ReadOnlyData* roData = &camera->paramData.keep1.roData;
    KeepOn1ReadWriteData* rwData = &camera->paramData.keep1.rwData;
    s16 t3;
    f32 playerHeight;

    sp88 = 0;
    playerHeight = player_get_tall(camera->player);
    if ((camera->target == NULL) || (camera->target->update == NULL)) {
        if (camera->target == NULL) {
            PRINTF(VT_COL(YELLOW, BLACK) "camera: warning: keepon: target is not valid, change parallel\n" VT_RST);
        }
        camera->target = NULL;
        changeCameraMode(camera, CAM_MODE_Z_PARALLEL);
        return 1;
    }

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

        roData->unk_00 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_04 = GET_NEXT_RO_DATA(values);
        roData->unk_08 = GET_NEXT_RO_DATA(values);
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_RO_DATA(values);
        roData->unk_18 = GET_NEXT_RO_DATA(values);
        roData->unk_1C = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_20 = GET_NEXT_RO_DATA(values);
        roData->unk_24 = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
        roData->unk_28 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_2C = GET_NEXT_SCALED_RO_DATA(values);
    }
    CAM_DEBUG_RELOAD_PREG(camera);

    playerHeight += roData->unk_00;
    spC0 = sglobe_by_2pos(at, eye);
    spB8 = sglobe_by_2pos(at, eyeNext);
    shrink_and_bitem = roData->interfaceField;
    if (RELOAD_PARAMS(camera)) {
        camera->animState++;
        rwData->unk_10 = 0;
        rwData->unk_04 = 0.0f;
        rwData->unk_0C = camera->target;
        rwData->unk_16 = CAM_DEFAULT_ANIM_TIME + CAM_GLOBAL_24;
        rwData->unk_12 = spC0.yaw;
        rwData->unk_14 = spC0.pitch;
        rwData->unk_00 = spC0.r;
        rwData->unk_08 = playerPosRot->pos.y - camera->playerPosDelta.y;
    }
    if (camera->status == CAM_STAT_ACTIVE) {
        cutflag = 1;
        camera->inputDir.x = -spC0.pitch;
        camera->inputDir.y = spC0.yaw - 0x7FFF;
        camera->inputDir.z = 0;
    }

    sp104 = roData->unk_04;
    sp84 = 1;

    switch (camera->viewFlags & (CAM_VIEW_TARGET | CAM_VIEW_TARGET_POS)) {
        case CAM_VIEW_TARGET:
            if ((camera->player->actor.category == 2) && (camera->player->interactRangeActor == camera->target)) {
                PosRot sp54;

                sp54 = Actor_get_eye(&camera->player->actor);
                spC8.r = 60.0f;
                spC8.yaw = camera->playerPosRot.rot.y;
                spC8.pitch = 0x2EE0;
                camera->targetPosRot.pos = translate_by_sglobe(&sp54.pos, &spC8);
            } else {
                camera->targetPosRot = Actor_get_eye(camera->target);
            }
            camera->targetPosRot = Actor_get_eye(camera->target);
            if (rwData->unk_0C != camera->target) {
                rwData->unk_0C = camera->target;
                camera->atLERPStepScale = 0.0f;
            }
            camera->xzOffsetUpdateRate = f_cush(1.0f, camera->xzOffsetUpdateRate,
                                                          CAM_UPDATE_RATE_STEP_SCALE_XZ * camera->speedRatio, 0.1f);
            camera->yOffsetUpdateRate = f_cush(1.0f, camera->yOffsetUpdateRate,
                                                         CAM_UPDATE_RATE_STEP_SCALE_Y * camera->speedRatio, 0.1f);
            camera->fovUpdateRate =
                f_cush(CAM_FOV_UPDATE_RATE, camera->fovUpdateRate, camera->speedRatio * 0.05f, 0.1f);
            goto cont;
        case CAM_VIEW_TARGET_POS:
            rwData->unk_0C = NULL;
        cont:
            if (camera->playerGroundY == camera->playerPosRot.pos.y || camera->player->actor.gravity > -0.1f ||
                camera->player->stateFlags1 & PLAYER_STATE1_21) {
                rwData->unk_08 = playerPosRot->pos.y;
                isOffGround = false;
            } else {
                isOffGround = true;
            }

            center4keepon(camera, &spB8, &camera->targetPosRot.pos,
                                   isOffGround ? roData->unk_28 : roData->unk_00, sp104, &rwData->unk_08, &spC8,
                                   (isOffGround ? CAM_LOCKON_AT_FLAG_OFF_GROUND : 0) | roData->interfaceField);
            sp114 = playerPosRot->pos;
            sp114.y += playerHeight;
            spC8 = sglobe_by_2pos(&sp114, &camera->targetPosRot.pos);
            sp84 = spC8.r > sp104 ? 1.0f : spC8.r / sp104;
            break;
        default:
            *at = playerPosRot->pos;
            at->y += playerHeight;
            rwData->unk_0C = NULL;
            break;
    }
    spD8 = sglobe_by_2pos(at, eyeNext);
    if (spD8.r < roData->unk_04) {
        sp104 = roData->unk_04;
        spE8 = CAM_R_UPDATE_RATE_INV;
    } else if (roData->unk_08 < spD8.r) {
        sp104 = roData->unk_08;
        spE8 = CAM_R_UPDATE_RATE_INV;
    } else {
        sp104 = spD8.r;
        spE8 = 1.0f;
    }

    camera->rUpdateRateInv = f_cush(spE8, camera->rUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
    spD8.r = spE8 = camera->dist = f_cush(sp104, camera->dist, 1.0f / camera->rUpdateRateInv, 0.2f);
    sp108 = camera->targetPosRot.pos;
    spD0 = sglobe_by_2pos(at, &sp108);
    spD0.r = spE8 - ((spD0.r <= spE8 ? spD0.r : spE8) * 0.5f);
    spEC = roData->unk_0C + ((roData->unk_10 - roData->unk_0C) * (1.1f - sp84));
    spF0 = spEC + CAM_GLOBAL_13;
    spD8.r = camera->dist = f_cush(spE8, camera->dist, CAM_GLOBAL_11, 2.0f);
    spD8.yaw = spB8.yaw;
    spE2 = spD0.yaw - (s16)(spB8.yaw - 0x7FFF);
    if (rwData->unk_16 != 0) {
        if (rwData->unk_16 >= CAM_GLOBAL_24) {
            sp82 = rwData->unk_16 - CAM_GLOBAL_24;
            spE2 = spC8.yaw;
            spC8 = sglobe_by_2pos(at, eye);
            spC8.yaw = spE2 - 0x7FFF;

            t2 = 1.0f / CAM_DEFAULT_ANIM_TIME;
            spE8 = (rwData->unk_00 - spC8.r) * t2;
            spE2 = (s16)(rwData->unk_12 - spC8.yaw) * t2;
            spE0 = (s16)(rwData->unk_14 - spC8.pitch) * t2;

            spD8.r = f_cush(spC8.r + (spE8 * sp82), spC0.r, CAM_GLOBAL_28, 1.0f);
            spD8.yaw = s_cush(spC8.yaw + (spE2 * sp82), spC0.yaw, CAM_GLOBAL_28, 0xA);
            spD8.pitch = s_cush(spC8.pitch + (spE0 * sp82), spC0.pitch, CAM_GLOBAL_28, 0xA);
        } else {
            sp88 = 1;
        }
        rwData->unk_16--;
    } else if (ABS(spE2) > CAM_DEG_TO_BINANG(spEC)) {
        spF4 = CAM_BINANG_TO_DEG(spE2);
        t2 = spEC + (spF0 - spEC) * (limiter(spD0.r, spD8.r) / spD8.r);
        temp_f12_2 = ((SQ(t2) - 2.0f) / (t2 - 360.0f));
        t1 = (temp_f12_2 * spF4) + (2.0f - (360.0f * temp_f12_2));
        temp_f14 = SQ(spF4) / t1;
        spE0 = spE2 >= 0 ? (CAM_DEG_TO_BINANG(temp_f14)) : (-CAM_DEG_TO_BINANG(temp_f14));
        spD8.yaw = (s16)((s16)(spB8.yaw - 0x7FFF) + spE0) - 0x7FFF;
    } else {
        spF4 = 0.02f;
        spF4 = (1.0f - camera->speedRatio) * spF4;
        spE0 = spE2 >= 0 ? CAM_DEG_TO_BINANG(spEC) : -CAM_DEG_TO_BINANG(spEC);
        spD8.yaw = spB8.yaw - (s16)((spE0 - spE2) * spF4);
    }

    if (sp88 == 0) {
        spE2 = CAM_DEG_TO_BINANG((f32)(roData->unk_14 + ((roData->unk_18 - roData->unk_14) * sp84)));
        spE2 -= (s16)(spC8.pitch * (0.5f + (sp84 * 0.5f)));

        spE8 = spD0.pitch * roData->unk_1C;
        spE2 += (s16)spE8;
        if (spE2 < -0x3200) {
            spE2 = -0x3200;
        } else if (spE2 > 0x3200) {
            spE2 = 0x3200;
        }

        spD8.pitch = s_cush(spE2, spB8.pitch, CAM_GLOBAL_12, 0xA);
        *eyeNext = translate_by_sglobe(at, &spD8);
        sp8C.pos = *eyeNext;
        if (camera->status == CAM_STAT_ACTIVE) {
            if (!camera->play->envCtx.skyboxDisabled || roData->interfaceField & KEEPON1_FLAG_0) {
                check_wall(camera, at, &sp8C);
            } else if (roData->interfaceField & KEEPON1_FLAG_1) {
                check_not_wall_bgc(camera, at, &sp8C);
            } else {
                sp120 = unitvector_by_2pos(at, &sp8C.pos);
                sp8C.pos.x -= sp120.x;
                sp8C.pos.y -= sp120.y;
                sp8C.pos.z -= sp120.z;
            }
            *eye = sp8C.pos;
        } else {
            *eye = *eyeNext;
        }
        sp120 = unitvector_by_2pos(eye, at);
        *eye = translate_by_unitvector(eye, &sp120, CAM_GLOBAL_1);
    }
    camera->fov = f_cush(roData->unk_20, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = s_cush(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = recover_shift(camera, isOffGround ? roData->unk_2C : roData->unk_24);
    return 1;
}

s32 keepon_camerawork_02(Camera* camera) {
    return NOTUSED(camera);
}

/**
 * Talking to an NPC
 */
s32 keepon_camerawork_03(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f playerHeadPos;
    Vec3f lineChkPointB;
    f32 temp_f0;
    f32 spBC;
    f32 prevTargetPlayerDist;
    f32 swingAngle;
    Actor* colChkActors[2];
    VecGeo targetToPlayerDir;
    VecGeo atToEyeAdj;
    UNUSED VecGeo atToEyeDir;
    VecGeo atToEyeNextDir;
    s32 i;
    s32 angleCnt;
    s16 sp82;
    s16 sp80;
    UNUSED PosRot playerPosRot;
    PosRot* camPlayerPosRot = &camera->playerPosRot;
    KeepOn3ReadOnlyData* roData = &camera->paramData.keep3.roData;
    KeepOn3ReadWriteData* rwData = &camera->paramData.keep3.rwData;
    s32 pad;
    f32 playerHeight;

    playerHeight = player_get_tall(camera->player);
    if (camera->target == NULL || camera->target->update == NULL) {
        if (camera->target == NULL) {
            PRINTF(VT_COL(YELLOW, BLACK) "camera: warning: talk: target is not valid, change parallel\n" VT_RST);
        }
        camera->target = NULL;
        changeCameraMode(camera, CAM_MODE_Z_PARALLEL);
        return 1;
    }
    if (RELOAD_PARAMS(camera)) {
        if (camera->play->view.unk_124 == 0) {
            camera->stateFlags |= CAM_STATE_LOCK_MODE;
            camera->play->view.unk_124 = camera->camId | 0x50;
            return 1;
        }
        camera->stateFlags &= ~CAM_STATE_LOCK_MODE;
    }
    camera->stateFlags &= ~CAM_STATE_CAM_FUNC_FINISH;
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->minDist = GET_NEXT_RO_DATA(values);
        roData->maxDist = GET_NEXT_RO_DATA(values);
        roData->swingYawInitial = GET_NEXT_RO_DATA(values);
        roData->swingYawFinal = GET_NEXT_RO_DATA(values);
        roData->swingPitchInitial = GET_NEXT_RO_DATA(values);
        roData->swingPitchFinal = GET_NEXT_RO_DATA(values);
        roData->swingPitchAdj = GET_NEXT_SCALED_RO_DATA(values);
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->atLERPScaleMax = GET_NEXT_SCALED_RO_DATA(values);
        roData->initTimer = GET_NEXT_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    playerHeight += roData->yOffset;
    atToEyeDir = sglobe_by_2pos(at, eye);
    atToEyeNextDir = sglobe_by_2pos(at, eyeNext);
    camera->targetPosRot = Actor_get_eye(camera->target);
    playerPosRot = Actor_get_eye(&camera->player->actor);
    playerHeadPos = camPlayerPosRot->pos;
    playerHeadPos.y += playerHeight;
    targetToPlayerDir = sglobe_by_2pos(&playerHeadPos, &camera->targetPosRot.pos);
    shrink_and_bitem = roData->interfaceField;
    if (RELOAD_PARAMS(camera)) {
        colChkActors[0] = camera->target;
        colChkActors[1] = &camera->player->actor;
        camera->animState++;
        rwData->target = camera->target;
        temp_f0 = (roData->maxDist < targetToPlayerDir.r ? 1.0f : targetToPlayerDir.r / roData->maxDist);
        rwData->animTimer = roData->initTimer;
        spBC = ((1.0f - temp_f0) * targetToPlayerDir.r) / rwData->animTimer;
        swingAngle = F32_LERPIMP(roData->swingPitchInitial, roData->swingPitchFinal, temp_f0);
        atToEyeAdj.pitch = CAM_DEG_TO_BINANG(swingAngle) + ((s16)(-(targetToPlayerDir.pitch * roData->swingPitchAdj)));
        swingAngle = F32_LERPIMP(roData->swingYawInitial, roData->swingYawFinal, temp_f0);
        if (roData->interfaceField & KEEPON3_FLAG_4) {
            if ((s16)(targetToPlayerDir.yaw - atToEyeNextDir.yaw) < 0) {
                atToEyeAdj.yaw = targetToPlayerDir.yaw + CAM_DEG_TO_BINANG(swingAngle);
            } else {
                atToEyeAdj.yaw = targetToPlayerDir.yaw - CAM_DEG_TO_BINANG(swingAngle);
            }
        } else if (roData->interfaceField & KEEPON3_FLAG_5) {
            if ((s16)(targetToPlayerDir.yaw - atToEyeNextDir.yaw) < 0) {
                atToEyeAdj.yaw = (s16)(targetToPlayerDir.yaw - 0x7FFF) - CAM_DEG_TO_BINANG(swingAngle);
            } else {
                atToEyeAdj.yaw = (s16)(targetToPlayerDir.yaw - 0x7FFF) + CAM_DEG_TO_BINANG(swingAngle);
            }
        } else if (ABS((s16)(targetToPlayerDir.yaw - atToEyeNextDir.yaw)) < 0x3FFF) {
            if ((s16)(targetToPlayerDir.yaw - atToEyeNextDir.yaw) < 0) {
                atToEyeAdj.yaw = targetToPlayerDir.yaw + CAM_DEG_TO_BINANG(swingAngle);
            } else {
                atToEyeAdj.yaw = targetToPlayerDir.yaw - CAM_DEG_TO_BINANG(swingAngle);
            }
        } else {
            if ((s16)(targetToPlayerDir.yaw - atToEyeNextDir.yaw) < 0) {
                atToEyeAdj.yaw = (s16)(targetToPlayerDir.yaw - 0x7FFF) - CAM_DEG_TO_BINANG(swingAngle);
            } else {
                atToEyeAdj.yaw = (s16)(targetToPlayerDir.yaw - 0x7FFF) + CAM_DEG_TO_BINANG(swingAngle);
            }
        }
        prevTargetPlayerDist = targetToPlayerDir.r;
        temp_f0 = 0.6f;
        targetToPlayerDir.r = (spBC * 0.6f) + (prevTargetPlayerDist * (1.0f - temp_f0));
        sp80 = atToEyeAdj.yaw;
        sp82 = atToEyeAdj.pitch;
        playerHeadPos = camPlayerPosRot->pos;
        playerHeadPos.y += playerHeight;
        rwData->atTarget = translate_by_sglobe(&playerHeadPos, &targetToPlayerDir);
        angleCnt = ARRAY_COUNT(colli_add_angle_h);
        i = 0;
        targetToPlayerDir.r = prevTargetPlayerDist;
        atToEyeAdj.r = roData->minDist + (targetToPlayerDir.r * (1 - 0.5f)) - atToEyeNextDir.r + atToEyeNextDir.r;
        lineChkPointB = translate_by_sglobe(&rwData->atTarget, &atToEyeAdj);
        if (!(roData->interfaceField & KEEPON3_FLAG_7)) {
            while (i < angleCnt) {
                if (!CollisionCheck_lineOcCheck2(camera->play, &camera->play->colChkCtx, &rwData->atTarget,
                                                &lineChkPointB, colChkActors, 2) &&
                    !check_wall_light(camera, &rwData->atTarget, &lineChkPointB)) {
                    break;
                }
                atToEyeAdj.yaw = sp80 + colli_add_angle_h[i];
                atToEyeAdj.pitch = sp82 + colli_add_angle_v[i];
                lineChkPointB = translate_by_sglobe(&rwData->atTarget, &atToEyeAdj);
                i++;
            }
        }
        PRINTF("camera: talk: BG&collision check %d time(s)\n", i);
        camera->stateFlags &= ~(CAM_STATE_CHECK_BG | CAM_STATE_EXTERNAL_FINISHED);
        pad = ((rwData->animTimer + 1) * rwData->animTimer) >> 1;
        rwData->eyeToAtTargetYaw = (f32)(s16)(atToEyeAdj.yaw - atToEyeNextDir.yaw) / pad;
        rwData->eyeToAtTargetPitch = (f32)(s16)(atToEyeAdj.pitch - atToEyeNextDir.pitch) / pad;
        rwData->eyeToAtTargetR = (atToEyeAdj.r - atToEyeNextDir.r) / pad;
        return 1;
    }

    if (rwData->animTimer != 0) {
        at->x += (rwData->atTarget.x - at->x) / rwData->animTimer;
        at->y += (rwData->atTarget.y - at->y) / rwData->animTimer;
        at->z += (rwData->atTarget.z - at->z) / rwData->animTimer;
        // needed to match
        if (!prevTargetPlayerDist) {}
        atToEyeAdj.r = ((rwData->eyeToAtTargetR * rwData->animTimer) + atToEyeNextDir.r) + 1.0f;
        atToEyeAdj.yaw = atToEyeNextDir.yaw + (s16)(rwData->eyeToAtTargetYaw * rwData->animTimer);
        atToEyeAdj.pitch = atToEyeNextDir.pitch + (s16)(rwData->eyeToAtTargetPitch * rwData->animTimer);
        *eyeNext = translate_by_sglobe(at, &atToEyeAdj);
        *eye = *eyeNext;
        camera->fov = f_cush(roData->fovTarget, camera->fov, 0.5, 1.0f);
        camera->roll = s_cush(0, camera->roll, 0.5, 0xA);
        camera->atLERPStepScale = recover_shift(camera, roData->atLERPScaleMax);
        check_wall_light(camera, at, eye);
        rwData->animTimer--;
    } else {
        camera->stateFlags |= (CAM_STATE_CAM_FUNC_FINISH | CAM_STATE_BLOCK_BG);
    }

    if (camera->stateFlags & CAM_STATE_EXTERNAL_FINISHED) {
        shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);
        slowly_cushon_set(camera);
        camera->atLERPStepScale = 0.0f;

        if (camera->xzSpeed > 0.001f || CAMERA_CHECK_BTN(&__game->state.input[0], BTN_A) ||
            CAMERA_CHECK_BTN(&__game->state.input[0], BTN_B) ||
            CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CLEFT) ||
            CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CDOWN) ||
            CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CUP) ||
            CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CRIGHT) ||
            CAMERA_CHECK_BTN(&__game->state.input[0], BTN_R) ||
            CAMERA_CHECK_BTN(&__game->state.input[0], BTN_Z)) {
            camera->stateFlags |= CAM_STATE_CHECK_BG;
            camera->stateFlags &= ~CAM_STATE_EXTERNAL_FINISHED;
        }
    }
    return 1;
}

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ique-cn:128 ntsc-1.0:124 ntsc-1.1:124 ntsc-1.2:124 pal-1.0:122 pal-1.1:122 hiratsu3:128"

s32 keepon_camerawork_04(Camera* camera) {
    static Vec3f ctr;
    static Vec3f pos0;
    static Vec3f pos1;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Actor* spCC[2];
    f32 t = -0.5f;
    f32 temp_f0_2;
    CollisionPoly* spC0;
    VecGeo spB8;
    UNUSED VecGeo spB0;
    VecGeo spA8;
    s16* temp_s0 = &camera->data2;
    s16 spA2;
    s16 spA0;
    s16 sp9E;
    s16 sp9C;
    PosRot* playerPosRot = &camera->playerPosRot;
    KeepOn4ReadOnlyData* roData = &camera->paramData.keep4.roData;
    KeepOn4ReadWriteData* rwData = &camera->paramData.keep4.rwData;
    s32 pad;
    f32 playerHeight;
    Player* player = GET_PLAYER(camera->play);
    s16 angleCnt;
    s32 i;

    if (RELOAD_PARAMS(camera)) {
        if (camera->play->view.unk_124 == 0) {
            camera->stateFlags |= CAM_STATE_LOCK_MODE;
            camera->stateFlags &= ~(CAM_STATE_CHECK_WATER | CAM_STATE_CHECK_BG);
            camera->play->view.unk_124 = camera->camId | 0x50;
            return 1;
        }
        rwData->unk_14 = *temp_s0;
        camera->stateFlags &= ~CAM_STATE_LOCK_MODE;
    }

    if (rwData->unk_14 != *temp_s0) {
        PRINTF(VT_COL(YELLOW, BLACK) "camera: item: item type changed %d -> %d\n" VT_RST, rwData->unk_14, *temp_s0);
        camera->animState = 20;
        camera->stateFlags |= CAM_STATE_LOCK_MODE;
        camera->stateFlags &= ~(CAM_STATE_CHECK_WATER | CAM_STATE_CHECK_BG);
        camera->play->view.unk_124 = camera->camId | 0x50;
        return 1;
    }

    playerHeight = player_get_tall(camera->player);
    camera->stateFlags &= ~CAM_STATE_CAM_FUNC_FINISH;
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + t - (68.0f / playerHeight * t);

        roData->unk_00 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_04 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->unk_08 = GET_NEXT_RO_DATA(values);
        roData->unk_0C = GET_NEXT_RO_DATA(values);
        roData->unk_10 = GET_NEXT_RO_DATA(values);
        roData->unk_18 = GET_NEXT_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
        roData->unk_14 = GET_NEXT_SCALED_RO_DATA(values);
        roData->unk_1E = GET_NEXT_RO_DATA(values);
        PRINTF("camera: item: type %d\n", *temp_s0);
        switch (*temp_s0) {
            case 1:
                roData->unk_00 = playerHeight * -0.6f * yNormal;
                roData->unk_04 = playerHeight * 2.0f * yNormal;
                roData->unk_08 = 10.0f;
                break;

            case 2:
            case 3:
                roData->unk_08 = -20.0f;
                roData->unk_18 = 80.0f;
                break;

            case 4:
                roData->unk_00 = playerHeight * -0.2f * yNormal;
                roData->unk_08 = 25.0f;
                break;

            case 8:
                roData->unk_00 = playerHeight * -0.2f * yNormal;
                roData->unk_04 = playerHeight * 0.8f * yNormal;
                roData->unk_08 = 50.0f;
                roData->unk_18 = 70.0f;
                break;

            case 9:
                roData->unk_00 = playerHeight * 0.1f * yNormal;
                roData->unk_04 = playerHeight * 0.5f * yNormal;
                roData->unk_08 = -20.0f;
                roData->unk_0C = 0.0f;
                roData->interfaceField =
                    CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE, KEEPON4_FLAG_6);
                break;

            case 5:
                roData->unk_00 = playerHeight * -0.4f * yNormal;
                roData->unk_08 = -10.0f;
                roData->unk_0C = 45.0f;
                roData->interfaceField =
                    CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_ALL, KEEPON4_FLAG_1);
                break;

            case 10:
                roData->unk_00 = playerHeight * -0.5f * yNormal;
                roData->unk_04 = playerHeight * 1.5f * yNormal;
                roData->unk_08 = -15.0f;
                roData->unk_0C = 175.0f;
                roData->unk_18 = 70.0f;
                roData->interfaceField =
                    CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_NOTHING_ALT, KEEPON4_FLAG_1);
                roData->unk_1E = 0x3C;
                break;

            case 12:
                roData->unk_00 = playerHeight * -0.6f * yNormal;
                roData->unk_04 = playerHeight * 1.6f * yNormal;
                roData->unk_08 = -2.0f;
                roData->unk_0C = 120.0f;
                roData->unk_10 = player->stateFlags1 & PLAYER_STATE1_27 ? 0.0f : 20.0f;
                roData->interfaceField = CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT,
                                                             KEEPON4_FLAG_4 | KEEPON4_FLAG_1);
                roData->unk_1E = 0x1E;
                roData->unk_18 = 50.0f;
                break;

            case 0x5A:
                roData->unk_00 = playerHeight * -0.3f * yNormal;
                roData->unk_18 = 45.0f;
                roData->interfaceField =
                    CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_IGNORE, KEEPON4_FLAG_1);
                break;

            case 0x5B:
                roData->unk_00 = playerHeight * -0.1f * yNormal;
                roData->unk_04 = playerHeight * 1.5f * yNormal;
                roData->unk_08 = -3.0f;
                roData->unk_0C = 10.0f;
                roData->unk_18 = 55.0f;
                roData->interfaceField =
                    CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_IGNORE, KEEPON4_FLAG_3);
                break;

            case 0x51:
                roData->unk_00 = playerHeight * -0.3f * yNormal;
                roData->unk_04 = playerHeight * 1.5f * yNormal;
                roData->unk_08 = 2.0f;
                roData->unk_18 = 45.0f;
                roData->unk_0C = 20.0f;
                roData->unk_10 = 20.0f;
                roData->interfaceField =
                    CAM_INTERFACE_FIELD(CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_NOTHING_ALT, KEEPON4_FLAG_7);
                roData->unk_1E = 0x1E;
                break;

            case 11:
                roData->unk_00 = playerHeight * -0.19f * yNormal;
                roData->unk_04 = playerHeight * 0.7f * yNormal;
                roData->unk_0C = 130.0f;
                roData->unk_10 = 10.0f;
                roData->interfaceField = CAM_INTERFACE_FIELD(
                    CAM_LETTERBOX_MEDIUM, CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_FORCE, KEEPON4_FLAG_5 | KEEPON4_FLAG_1);
                break;

            default:
                break;
        }
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    cutflag = 1;
    shrink_and_bitem = roData->interfaceField;
    spB0 = sglobe_by_2pos(at, eye);
    spA8 = sglobe_by_2pos(at, eyeNext);
    ctr = playerPosRot->pos;
    ctr.y += playerHeight;
    temp_f0_2 = T_BGCheck_CameraGroundCheck_grpWG_ai(&camera->play->colCtx, &spC0, &i, &ctr);
    if (temp_f0_2 > (roData->unk_00 + ctr.y)) {
        ctr.y = temp_f0_2 + 10.0f;
    } else {
        ctr.y += roData->unk_00;
    }

    sp9C = 0;
    switch (camera->animState) {
        case 0:
        case 20:
            spCC[sp9C] = &camera->player->actor;
            sp9C++;
            default_cushon_set(camera);
            camera->stateFlags &= ~(CAM_STATE_CHECK_WATER | CAM_STATE_CHECK_BG);
            rwData->unk_10 = roData->unk_1E;
            rwData->unk_08 = playerPosRot->pos.y - camera->playerPosDelta.y;
            if (roData->interfaceField & KEEPON4_FLAG_1) {
                spA2 = CAM_DEG_TO_BINANG(roData->unk_08);
                spA0 = (s16)((s16)(playerPosRot->rot.y - 0x7FFF) - spA8.yaw) > 0
                           ? (s16)(playerPosRot->rot.y - 0x7FFF) + CAM_DEG_TO_BINANG(roData->unk_0C)
                           : (s16)(playerPosRot->rot.y - 0x7FFF) - CAM_DEG_TO_BINANG(roData->unk_0C);
            } else if (roData->interfaceField & KEEPON4_FLAG_2) {
                spA2 = CAM_DEG_TO_BINANG(roData->unk_08);
                spA0 = CAM_DEG_TO_BINANG(roData->unk_0C);
            } else if ((roData->interfaceField & KEEPON4_FLAG_3) && camera->target != NULL) {
                PosRot sp60;

                sp60 = Actor_get_shape(camera->target);
                spA2 = CAM_DEG_TO_BINANG(roData->unk_08) - sp60.rot.x;
                spA0 = (s16)((s16)(sp60.rot.y - 0x7FFF) - spA8.yaw) > 0
                           ? (s16)(sp60.rot.y - 0x7FFF) + CAM_DEG_TO_BINANG(roData->unk_0C)
                           : (s16)(sp60.rot.y - 0x7FFF) - CAM_DEG_TO_BINANG(roData->unk_0C);
                spCC[1] = camera->target;
                sp9C++;
            } else if ((roData->interfaceField & KEEPON4_FLAG_7) && camera->target != NULL) {
                PosRot sp4C;

                sp4C = Actor_get_world(camera->target);
                spA2 = CAM_DEG_TO_BINANG(roData->unk_08);
                sp9E = get_y_angle_by_2pos(&sp4C.pos, &playerPosRot->pos);
                spA0 = ((s16)(sp9E - spA8.yaw) > 0) ? sp9E + CAM_DEG_TO_BINANG(roData->unk_0C)
                                                    : sp9E - CAM_DEG_TO_BINANG(roData->unk_0C);
                spCC[1] = camera->target;
                sp9C++;
            } else if (roData->interfaceField & KEEPON4_FLAG_6) {
                spA2 = CAM_DEG_TO_BINANG(roData->unk_08);
                spA0 = spA8.yaw;
            } else {
                spA2 = spA8.pitch;
                spA0 = spA8.yaw;
            }

            spB8.pitch = spA2;
            spB8.yaw = spA0;
            spB8.r = roData->unk_04;
            pos1 = translate_by_sglobe(&ctr, &spB8);
            if (!(roData->interfaceField & KEEPON4_FLAG_0)) {
                angleCnt = ARRAY_COUNT(colli_add_angle_h);
                for (i = 0; i < angleCnt; i++) {
                    if (!CollisionCheck_lineOcCheck2(camera->play, &camera->play->colChkCtx, &ctr, &pos1,
                                                    spCC, sp9C) &&
                        !check_wall_light(camera, &ctr, &pos1)) {
                        break;
                    }
                    spB8.yaw = colli_add_angle_h[i] + spA0;
                    spB8.pitch = colli_add_angle_v[i] + spA2;
                    pos1 = translate_by_sglobe(&ctr, &spB8);
                }
#if DEBUG_FEATURES
                PRINTF("camera: item: BG&collision check %d time(s)\n", i);
#endif
            }
            rwData->unk_04 = (s16)(spB8.pitch - spA8.pitch) / (f32)rwData->unk_10;
            rwData->unk_00 = (s16)(spB8.yaw - spA8.yaw) / (f32)rwData->unk_10;
            rwData->unk_0C = spA8.yaw;
            rwData->unk_0E = spA8.pitch;
            camera->animState++;
            rwData->unk_12 = 1;
            break;
        case 10:
            rwData->unk_08 = playerPosRot->pos.y - camera->playerPosDelta.y;
        default:
            break;
    }
    camera->xzOffsetUpdateRate = 0.25f;
    camera->yOffsetUpdateRate = 0.25f;
    camera->atLERPStepScale = 0.75f;
    ez_xyz_cush(&ctr, at, 0.5f, 0.5f, 0.2f);
    if (roData->unk_10 != 0.0f) {
        spB8.r = roData->unk_10;
        spB8.pitch = 0;
        spB8.yaw = playerPosRot->rot.y;
        *at = translate_by_sglobe(at, &spB8);
    }
    camera->atLERPStepScale = 0.0f;
    camera->dist = f_cush(roData->unk_04, camera->dist, 0.25f, 2.0f);
    spB8.r = camera->dist;
    if (rwData->unk_10 != 0) {
        camera->stateFlags |= CAM_STATE_LOCK_MODE;
        rwData->unk_0C += (s16)rwData->unk_00;
        rwData->unk_0E += (s16)rwData->unk_04;
        rwData->unk_10--;
    } else if (roData->interfaceField & KEEPON4_FLAG_4) {
        camera->stateFlags |= (CAM_STATE_CAM_FUNC_FINISH | CAM_STATE_BLOCK_BG);
        camera->stateFlags |= (CAM_STATE_CHECK_WATER | CAM_STATE_CHECK_BG);
        camera->stateFlags &= ~CAM_STATE_EXTERNAL_FINISHED;
        if (camera->timer > 0) {
            camera->timer--;
        }
    } else {
        camera->stateFlags |= (CAM_STATE_CAM_FUNC_FINISH | CAM_STATE_BLOCK_BG);
        if ((camera->stateFlags & CAM_STATE_EXTERNAL_FINISHED) || (roData->interfaceField & KEEPON4_FLAG_7)) {
            shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);
            camera->stateFlags |= (CAM_STATE_CHECK_WATER | CAM_STATE_CHECK_BG);
            camera->stateFlags &= ~CAM_STATE_EXTERNAL_FINISHED;
            if (camera->prevBgCamIndex < 0) {
                change_camera_set(camera, camera->prevSetting, CAM_REQUEST_SETTING_IGNORE_PRIORITY);
            } else {
                changeCameraID(camera, camera->prevBgCamIndex);
                camera->prevBgCamIndex = -1;
            }
        }
    }
    spB8.yaw = s_cush(rwData->unk_0C, spA8.yaw, roData->unk_14, 4);
    spB8.pitch = s_cush(rwData->unk_0E, spA8.pitch, roData->unk_14, 4);
    *eyeNext = translate_by_sglobe(at, &spB8);
    *eye = *eyeNext;
    check_wall_light(camera, at, eye);
    camera->fov = f_cush(roData->unk_18, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = s_cush(0, camera->roll, 0.5f, 0xA);
    //! @bug Missing return, but the return value is not used.
}

/**
 * Talking in a pre-rendered room
 */
s32 keepon_camerawork_00(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f* at = &camera->at;
    VecGeo eyeTargetPosOffset;
    VecGeo eyeAtOffset;
    KeepOn0ReadOnlyData* roData = &camera->paramData.keep0.roData;
    KeepOn0ReadWriteData* rwData = &camera->paramData.keep0.rwData;
    s32 pad;
    BgCamFuncData* bgCamFuncData;
    UNUSED Vec3s bgCamRot;
    s16 fov;

    camera->stateFlags &= ~CAM_STATE_CAM_FUNC_FINISH;

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->fovScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->yawScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->timerInit = GET_NEXT_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);
    *eyeNext = s_xyz2xyz_t(&bgCamFuncData->pos);
    *eye = *eyeNext;

    bgCamRot = bgCamFuncData->rot;

    fov = bgCamFuncData->fov;
    if (fov == -1) {
        fov = 6000;
    }

    if (camera->target == NULL || camera->target->update == NULL) {
        if (camera->target == NULL) {
            PRINTF(VT_COL(YELLOW, BLACK) "camera: warning: talk: target is not valid, change normal camera\n" VT_RST);
        }
        camera->target = NULL;
        changeCameraMode(camera, CAM_MODE_NORMAL);
        return true;
    }

    camera->targetPosRot = Actor_get_eye(camera->target);

    eyeAtOffset = sglobe_by_2pos(eye, at);
    eyeTargetPosOffset = sglobe_by_2pos(eye, &camera->targetPosRot.pos);

    shrink_and_bitem = roData->interfaceField;

    if (camera->animState == 0) {
        camera->animState++;
        camera->fov = CAM_DATA_SCALED(fov);
        camera->roll = 0;
        camera->atLERPStepScale = 0.0f;
        rwData->animTimer = roData->timerInit;
        rwData->fovTarget = camera->fov - (camera->fov * roData->fovScale);
    }

    if (rwData->animTimer != 0) {
        eyeAtOffset.yaw += ((s16)(eyeTargetPosOffset.yaw - eyeAtOffset.yaw) / rwData->animTimer) * roData->yawScale;
        *at = translate_by_sglobe(eye, &eyeAtOffset);
        rwData->animTimer--;
    } else {
        camera->stateFlags |= (CAM_STATE_CAM_FUNC_FINISH | CAM_STATE_BLOCK_BG);
    }
    camera->fov = f_cush(rwData->fovTarget, camera->fov, 0.5f, 10.0f);
    return true;
}

s32 fixed_camerawork_01(Camera* camera) {
    Fixed1ReadOnlyData* roData = &camera->paramData.fixd1.roData;
    Fixed1ReadWriteData* rwData = &camera->paramData.fixd1.rwData;
    s32 pad;
    VecGeo eyeOffset;
    VecGeo eyeAtOffset;
    s32 pad2;
    Vec3f adjustedPos;
    BgCamFuncData* bgCamFuncData;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 playerHeight;

    playerHeight = player_get_tall(camera->player);
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);
        rwData->eyePosRotTarget.pos = s_xyz2xyz_t(&bgCamFuncData->pos);
        rwData->eyePosRotTarget.rot = bgCamFuncData->rot;
        rwData->fov = bgCamFuncData->fov;

        roData->unk_00 = GET_NEXT_SCALED_RO_DATA(values) * playerHeight;
        roData->lerpStep = GET_NEXT_SCALED_RO_DATA(values);
        roData->fov = GET_NEXT_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);
    if (rwData->fov == -1) {
        rwData->fov = roData->fov * 100.0f;
    } else if (rwData->fov <= 360) {
        rwData->fov *= 100;
    }

    shrink_and_bitem = roData->interfaceField;

    if (camera->animState == 0) {
        camera->animState++;
        slowly_cushon_set(camera);
        if (rwData->fov != -1) {
            roData->fov = CAM_DATA_SCALED(rwData->fov);
        }
    }

    eyeAtOffset = sglobe_by_2pos(eye, at);

    ez_xyz_cush(&rwData->eyePosRotTarget.pos, eye, 0.1f, 0.1f, 0.2f);
    adjustedPos = playerPosRot->pos;
    adjustedPos.y += playerHeight;
    camera->dist = distance_between(&adjustedPos, eye);

    eyeOffset.r = camera->dist;
    eyeOffset.pitch = s_cush(-rwData->eyePosRotTarget.rot.x, eyeAtOffset.pitch, roData->lerpStep, 5);
    eyeOffset.yaw = s_cush(rwData->eyePosRotTarget.rot.y, eyeAtOffset.yaw, roData->lerpStep, 5);

    *at = translate_by_sglobe(eye, &eyeOffset);

    camera->eyeNext = *eye;

    camera->fov = f_cush(roData->fov, camera->fov, roData->lerpStep, 0.01f);
    camera->roll = 0;
    camera->atLERPStepScale = 0.0f;

    camera->playerToAtOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->playerToAtOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->playerToAtOffset.z = camera->at.z - playerPosRot->pos.z;

    return true;
}

s32 fixed_camerawork_02(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f atTarget;
    Vec3f playerToAtOffsetTarget;
    PosRot* playerPosRot = &camera->playerPosRot;
    BgCamFuncData* bgCamFuncData;
    Fixed2ReadOnlyData* roData = &camera->paramData.fixd2.roData;
    Fixed2ReadWriteData* rwData = &camera->paramData.fixd2.rwData;
    s32 pad;
    f32 playerHeight;

    playerHeight = player_get_tall(camera->player);

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

        roData->yOffset = (GET_NEXT_SCALED_RO_DATA(values) * playerHeight) * yNormal;
        roData->eyeStepScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->posStepScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->fov = GET_NEXT_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
        rwData->fov = roData->fov * 100.0f;

        bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);
        if (bgCamFuncData != NULL) {
            rwData->eye = s_xyz2xyz_t(&bgCamFuncData->pos);
            if (bgCamFuncData->fov != -1) {
                rwData->fov = bgCamFuncData->fov;
            }
        } else {
            rwData->eye = *eye;
        }
        if (rwData->fov <= 360) {
            rwData->fov *= 100;
        }
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    shrink_and_bitem = roData->interfaceField;

    playerToAtOffsetTarget.x = 0.0f;
    playerToAtOffsetTarget.y = roData->yOffset + playerHeight;
    playerToAtOffsetTarget.z = 0.0f;

    ez_xyz_cush(&playerToAtOffsetTarget, &camera->playerToAtOffset, roData->posStepScale, roData->posStepScale,
                         0.1f);
    atTarget.x = playerPosRot->pos.x + camera->playerToAtOffset.x;
    atTarget.y = playerPosRot->pos.y + camera->playerToAtOffset.y;
    atTarget.z = playerPosRot->pos.z + camera->playerToAtOffset.z;
    if (camera->animState == 0) {
        camera->animState++;
        slowly_cushon_set(camera);
        if (!(roData->interfaceField & FIXED2_FLAG_0)) {
            *eye = *eyeNext = rwData->eye;
            camera->at = atTarget;
        }
    }

    ez_xyz_cush(&atTarget, &camera->at, roData->posStepScale, roData->posStepScale, 10.0f);
    ez_xyz_cush(&rwData->eye, eyeNext, roData->eyeStepScale, roData->eyeStepScale, 0.1f);

    *eye = *eyeNext;
    camera->dist = distance_between(at, eye);
    camera->roll = 0;
    camera->xzSpeed = 0.0f;
    camera->fov = CAM_DATA_SCALED(rwData->fov);
    camera->atLERPStepScale = recover_shift(camera, 1.0f);
    camera->playerToAtOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->playerToAtOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->playerToAtOffset.z = camera->at.z - playerPosRot->pos.z;
    return true;
}

/**
 * Camera's position is fixed, does not move, or rotate
 */
s32 fixed_camerawork_03(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    VecGeo atGeo;
    BgCamFuncData* bgCamFuncData;
    UNUSED VecGeo eyeAtOffset;
    Fixed3ReadOnlyData* roData = &camera->paramData.fixd3.roData;
    Fixed3ReadWriteData* rwData = &camera->paramData.fixd3.rwData;
    s32 pad;

    bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);

    eyeAtOffset = sglobe_by_2pos(eye, at);

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceField = GET_NEXT_RO_DATA(values);
        *eyeNext = s_xyz2xyz_t(&bgCamFuncData->pos);
        *eye = *eyeNext;
        rwData->rot = bgCamFuncData->rot;
        rwData->fov = bgCamFuncData->fov;
        rwData->roomImageOverrideBgCamIndex = bgCamFuncData->roomImageOverrideBgCamIndex;
        if (rwData->fov == -1) {
            rwData->fov = 6000;
        }
        if (rwData->fov <= 360) {
            rwData->fov *= 100;
        }
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    if (camera->animState == 0) {
        rwData->updDirTimer = 5;
        R_CAM_DATA(CAM_DATA_FOV) = rwData->fov;
        camera->animState++;
    }

    if (bgCamFuncData->roomImageOverrideBgCamIndex != rwData->roomImageOverrideBgCamIndex) {
        PRINTF("camera: position change %d \n", rwData->roomImageOverrideBgCamIndex);
        rwData->roomImageOverrideBgCamIndex = bgCamFuncData->roomImageOverrideBgCamIndex;
        rwData->updDirTimer = 5;
    }

    if (rwData->updDirTimer > 0) {
        rwData->updDirTimer--;
        cutflag = true;
    } else {
        cutflag = false;
    }

    atGeo.r = 150.0f;
    atGeo.yaw = rwData->rot.y;
    atGeo.pitch = -rwData->rot.x;

    *at = translate_by_sglobe(eye, &atGeo);
    shrink_and_bitem = roData->interfaceField;
    rwData->fov = R_CAM_DATA(CAM_DATA_FOV);
    camera->roll = 0;
    camera->fov = rwData->fov * 0.01f;
    camera->atLERPStepScale = 0.0f;
    return true;
}

/**
 * camera follow player, eye is in a fixed offset of the previous eye, and a value
 * specified in the scene.
 */
s32 fixed_camerawork_04(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f playerPosWithCamOffset;
    Vec3f atTarget;
    Vec3f playerToAtOffsetTarget;
    VecGeo atEyeNextOffset;
    VecGeo atTargetEyeNextOffset;
    PosRot* playerPosRot = &camera->playerPosRot;
    BgCamFuncData* bgCamFuncData;
    Vec3f* posOffset = &camera->playerToAtOffset;
    Fixed4ReadOnlyData* roData = &camera->paramData.fixd4.roData;
    Fixed4ReadWriteData* rwData = &camera->paramData.fixd4.rwData;
    f32 playerYOffset;

    playerYOffset = player_get_tall(camera->player);

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerYOffset));

        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * playerYOffset * yNormal;
        roData->speedToEyePos = GET_NEXT_SCALED_RO_DATA(values);
        roData->followSpeed = GET_NEXT_SCALED_RO_DATA(values);
        roData->fov = GET_NEXT_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);

        bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);
        if (bgCamFuncData != NULL) {
            rwData->eyeTarget = s_xyz2xyz_t(&bgCamFuncData->pos);
        } else {
            rwData->eyeTarget = *eye;
        }
    }
    CAM_DEBUG_RELOAD_PREG(camera);
    shrink_and_bitem = roData->interfaceField;
    if (camera->animState == 0) {
        camera->animState++;
        if (!(roData->interfaceField & FIXED4_FLAG_2)) {
            slowly_cushon_set(camera);
        }
        rwData->followSpeed = roData->followSpeed;
    }

    VEC3F_LERPIMPDST(eyeNext, eyeNext, &rwData->eyeTarget, roData->speedToEyePos);
    *eye = *eyeNext;

    playerToAtOffsetTarget.x = 0.0f;
    playerToAtOffsetTarget.y = roData->yOffset + playerYOffset;
    playerToAtOffsetTarget.z = 0.0f;
    ez_xyz_cush(&playerToAtOffsetTarget, &camera->playerToAtOffset, 0.1f, 0.1f, 0.1f);

    playerPosWithCamOffset.x = playerPosRot->pos.x + camera->playerToAtOffset.x;
    playerPosWithCamOffset.y = playerPosRot->pos.y + camera->playerToAtOffset.y;
    playerPosWithCamOffset.z = playerPosRot->pos.z + camera->playerToAtOffset.z;
    VEC3F_LERPIMPDST(&atTarget, at, &playerPosWithCamOffset, 0.5f);

    atEyeNextOffset = sglobe_by_2pos(eyeNext, at);
    atTargetEyeNextOffset = sglobe_by_2pos(eyeNext, &atTarget);

    atEyeNextOffset.r += (atTargetEyeNextOffset.r - atEyeNextOffset.r) * rwData->followSpeed;
    atEyeNextOffset.pitch = s_cush(atTargetEyeNextOffset.pitch, atEyeNextOffset.pitch,
                                             rwData->followSpeed * camera->speedRatio, 0xA);
    atEyeNextOffset.yaw =
        s_cush(atTargetEyeNextOffset.yaw, atEyeNextOffset.yaw, rwData->followSpeed * camera->speedRatio, 0xA);
    *at = translate_by_sglobe(eyeNext, &atEyeNextOffset);
    camera->dist = distance_between(at, eye);
    camera->roll = 0;
    camera->fov = roData->fov;
    camera->atLERPStepScale = recover_shift(camera, 1.0f);
    return true;
}

s32 fixed_camerawork_00(Camera* camera) {
    return NOTUSED(camera);
}

s32 subject_camerawork_01(Camera* camera) {
    return NOTUSED(camera);
}

s32 subject_camerawork_02(Camera* camera) {
    return NOTUSED(camera);
}

/**
 * First person view
 */
s32 subject_camerawork_03(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f sp98;
    UNUSED Vec3f sp8C;
    VecGeo sp84;
    VecGeo sp7C;
    VecGeo tGeo;
    PosRot sp60;
    PosRot* playerPosRot = &camera->playerPosRot;
    f32 sp58;
    f32 temp_f0_3;
    s16 sp52;
    s16 sp50;
    Subj3ReadOnlyData* roData = &camera->paramData.subj3.roData;
    Subj3ReadWriteData* rwData = &camera->paramData.subj3.rwData;
    CameraModeValue* values;
    Vec3f* pad2;
    f32 playerHeight;

    sp60 = Actor_get_eye(&camera->player->actor);
    playerHeight = player_get_tall(camera->player);

    if (camera->play->view.unk_124 == 0) {
        camera->play->view.unk_124 = camera->camId | 0x50;
        return true;
    }

    default_cushon_set(camera);

#if DEBUG_FEATURES
    copyCameraPosPREGToModeValues(camera);
#endif

    values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
    roData->eyeNextYOffset = GET_NEXT_SCALED_RO_DATA(values) * playerHeight;
    roData->eyeDist = GET_NEXT_RO_DATA(values);
    roData->eyeNextDist = GET_NEXT_RO_DATA(values);
    roData->unk_0C = GET_NEXT_RO_DATA(values);
    roData->atOffset.x = GET_NEXT_RO_DATA(values) * 0.1f;
    roData->atOffset.y = GET_NEXT_RO_DATA(values) * 0.1f;
    roData->atOffset.z = GET_NEXT_RO_DATA(values) * 0.1f;
    roData->fovTarget = GET_NEXT_RO_DATA(values);
    roData->interfaceField = GET_NEXT_RO_DATA(values);
    sp84.r = roData->eyeNextDist;
    sp84.yaw = sp60.rot.y - 0x7FFF;
    sp84.pitch = sp60.rot.x;
    sp98 = sp60.pos;
    sp98.y += roData->eyeNextYOffset;

    sp8C = translate_by_sglobe(&sp98, &sp84);
    sp7C = sglobe_by_2pos(at, eye);

    shrink_and_bitem = roData->interfaceField;
    if (RELOAD_PARAMS(camera)) {
        rwData->r = sp7C.r;
        rwData->yaw = sp7C.yaw;
        rwData->pitch = sp7C.pitch;
        rwData->animTimer = CAM_DEFAULT_ANIM_TIME;
        camera->dist = roData->eyeNextDist;
        camera->animState++;
        camera->rUpdateRateInv = 1.0f;
        camera->dist = roData->eyeNextDist;
    }

    tGeo.r = rwData->r;
    tGeo.yaw = rwData->yaw;
    tGeo.pitch = rwData->pitch;
    if (rwData->animTimer != 0) {
        temp_f0_3 = (1.0f / rwData->animTimer);
        at->x = F32_LERPIMP(at->x, sp98.x, temp_f0_3);
        at->y = F32_LERPIMP(at->y, sp98.y, temp_f0_3);
        at->z = F32_LERPIMP(at->z, sp98.z, temp_f0_3);

        temp_f0_3 = (1.0f / CAM_DEFAULT_ANIM_TIME);
        sp58 = (tGeo.r - sp84.r) * temp_f0_3;
        sp52 = (s16)(tGeo.yaw - sp84.yaw) * temp_f0_3;
        sp50 = (s16)(tGeo.pitch - sp84.pitch) * temp_f0_3;

        sp7C.r = f_cush(sp84.r + (sp58 * rwData->animTimer), sp7C.r, CAM_GLOBAL_28, 1.0f);
        sp7C.yaw = s_cush(sp84.yaw + (sp52 * rwData->animTimer), sp7C.yaw, CAM_GLOBAL_28, 0xA);
        sp7C.pitch = s_cush(sp84.pitch + (sp50 * rwData->animTimer), sp7C.pitch, CAM_GLOBAL_28, 0xA);
        *eyeNext = translate_by_sglobe(at, &sp7C);

        *eye = *eyeNext;
        rwData->animTimer--;

        if (!camera->play->envCtx.skyboxDisabled) {
            check_wall_light(camera, at, eye);
        } else {
            check_not_wall_light(camera, at, eye);
        }
    } else {
        sp58 = sin_s(-sp60.rot.x);
        temp_f0_3 = cos_s(-sp60.rot.x);
        sp98.x = roData->atOffset.x;
        sp98.y = (roData->atOffset.y * temp_f0_3) - (roData->atOffset.z * sp58);
        sp98.z = (roData->atOffset.y * sp58) + (roData->atOffset.z * temp_f0_3);
        sp58 = sin_s(sp60.rot.y - 0x7FFF);
        temp_f0_3 = cos_s(sp60.rot.y - 0x7FFF);
        roData->atOffset.x = (sp98.z * sp58) + (sp98.x * temp_f0_3);
        roData->atOffset.y = sp98.y;
        roData->atOffset.z = (sp98.z * temp_f0_3) - (sp98.x * sp58);
        at->x = roData->atOffset.x + sp60.pos.x;
        at->y = roData->atOffset.y + sp60.pos.y;
        at->z = roData->atOffset.z + sp60.pos.z;
        sp7C.r = roData->eyeNextDist;
        sp7C.yaw = sp60.rot.y - 0x7FFF;
        sp7C.pitch = sp60.rot.x;
        *eyeNext = translate_by_sglobe(at, &sp7C);
        sp7C.r = roData->eyeDist;
        *eye = translate_by_sglobe(at, &sp7C);
    }

    camera->playerToAtOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->playerToAtOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->playerToAtOffset.z = camera->at.z - playerPosRot->pos.z;
    camera->fov = f_cush(roData->fovTarget, camera->fov, 0.25f, 1.0f);
    camera->roll = 0;
    camera->atLERPStepScale = 0.0f;
    return 1;
}

/**
 * Crawlspaces
 * Moves the camera from third person to first person when entering a crawlspace
 * While in the crawlspace, link remains fixed in a single direction
 * The camera is what swings up and down while crawling forward or backwards
 *
 * Note:
 * Subject 4 uses bgCamFuncData.data differently than other functions:
 * All Vec3s data are points along the crawlspace
 * The second point represents the entrance, and the second to last point represents the exit
 * All other points are unused
 * All instances of crawlspaces have 6 points, except for the Testroom scene which has 9 points
 */
s32 subject_camerawork_04(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f* at = &camera->at;
    u16 crawlspaceNumPoints;
    Vec3s* crawlspacePoints;
    Vec3f temp1;
    Vec3f zoomAtTarget;
    f32 temp2;
    s32 pad1;
    f32 eyeLerp;
    PosRot playerPosRot;
    VecGeo targetOffset;
    VecGeo atEyeOffset;
    s16 eyeToAtYaw;
    s32 pad2;
    f32 temp;
    Subj4ReadOnlyData* roData = &camera->paramData.subj4.roData;
    Subj4ReadWriteData* rwData = &camera->paramData.subj4.rwData;

#define vCrawlSpaceBackPos temp1
#define vEyeTarget temp1
#define vPlayerDistToFront temp2
#define vZoomTimer temp2

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    if (camera->play->view.unk_124 == 0) {
        camera->play->view.unk_124 = camera->camId | 0x50;
        rwData->xzSpeed = camera->xzSpeed;
        return true;
    }

    playerPosRot = Actor_get_shape(&camera->player->actor);
    atEyeOffset = sglobe_by_2pos(at, eye);

    shrink_and_bitem = roData->interfaceField;

    // Crawlspace setup (runs for only 1 frame)
    if (camera->animState == 0) {
        crawlspacePoints = (Vec3s*)get_camera_point_from_polygon(camera, &crawlspaceNumPoints);
        // Second entry of crawlspacePoints contains the front position
        rwData->crawlspaceLine.point = s_xyz2xyz_t(&crawlspacePoints[1]);
        // Second last entry of crawlspacePoints contains the back position
        vCrawlSpaceBackPos = s_xyz2xyz_t(&crawlspacePoints[crawlspaceNumPoints - 2]);

        targetOffset.r = 10.0f;
        targetOffset.pitch = 0x238C; // ~50 degrees
        targetOffset.yaw = get_y_angle_by_2pos(&vCrawlSpaceBackPos, &rwData->crawlspaceLine.point);

        vPlayerDistToFront = distance_between(&camera->playerPosRot.pos, &rwData->crawlspaceLine.point);
        if (distance_between(&camera->playerPosRot.pos, &vCrawlSpaceBackPos) < vPlayerDistToFront) {
            // Player is entering the crawlspace from the back
            rwData->crawlspaceLine.dir.x = rwData->crawlspaceLine.point.x - vCrawlSpaceBackPos.x;
            rwData->crawlspaceLine.dir.y = rwData->crawlspaceLine.point.y - vCrawlSpaceBackPos.y;
            rwData->crawlspaceLine.dir.z = rwData->crawlspaceLine.point.z - vCrawlSpaceBackPos.z;
            rwData->crawlspaceLine.point = vCrawlSpaceBackPos;
        } else {
            // Player is entering the crawlspace from the front
            rwData->crawlspaceLine.dir.x = vCrawlSpaceBackPos.x - rwData->crawlspaceLine.point.x;
            rwData->crawlspaceLine.dir.y = vCrawlSpaceBackPos.y - rwData->crawlspaceLine.point.y;
            rwData->crawlspaceLine.dir.z = vCrawlSpaceBackPos.z - rwData->crawlspaceLine.point.z;
            targetOffset.yaw -= 0x7FFF;
        }

        rwData->forwardYaw = targetOffset.yaw;
        rwData->zoomTimer = 10;
        rwData->eyeLerpPhase = 0;
        rwData->isSfxOff = false;
        rwData->eyeLerp = 0.0f;
        camera->animState++;
    }

    // Camera zooms in from third person to first person over 10 frames
    if (rwData->zoomTimer != 0) {
        targetOffset.r = 10.0f;
        targetOffset.pitch = 0x238C; // ~50 degrees
        targetOffset.yaw = rwData->forwardYaw;
        zoomAtTarget = translate_by_sglobe(&playerPosRot.pos, &targetOffset);

        vZoomTimer = rwData->zoomTimer + 1.0f;
        at->x = F32_LERPIMPINV(at->x, zoomAtTarget.x, vZoomTimer);
        at->y = F32_LERPIMPINV(at->y, zoomAtTarget.y, vZoomTimer);
        at->z = F32_LERPIMPINV(at->z, zoomAtTarget.z, vZoomTimer);

        atEyeOffset.r -= (atEyeOffset.r / vZoomTimer);
        atEyeOffset.yaw = BINANG_LERPIMPINV(atEyeOffset.yaw, (s16)(playerPosRot.rot.y - 0x7FFF), rwData->zoomTimer);
        atEyeOffset.pitch = BINANG_LERPIMPINV(atEyeOffset.pitch, playerPosRot.rot.x, rwData->zoomTimer);
        *eyeNext = translate_by_sglobe(at, &atEyeOffset);
        *eye = *eyeNext;
        rwData->zoomTimer--;
        return false;
    }

    if (rwData->xzSpeed < 0.5f) {
        return false;
    }

    playerPosRot = Actor_get_shape(&camera->player->actor);
    Math3D_lineVsPosSuisenCross(&rwData->crawlspaceLine, &playerPosRot.pos, eyeNext);

    // *at is unused before getting overwritten later this function
    at->x = eyeNext->x + rwData->crawlspaceLine.dir.x;
    at->y = eyeNext->y + rwData->crawlspaceLine.dir.y;
    at->z = eyeNext->z + rwData->crawlspaceLine.dir.z;

    *eye = *eyeNext;

    targetOffset.yaw = rwData->forwardYaw;
    targetOffset.r = 5.0f;
    targetOffset.pitch = 0x238C; // ~50 degrees

    vEyeTarget = translate_by_sglobe(eyeNext, &targetOffset);

    rwData->eyeLerpPhase += 0xBB8;
    eyeLerp = cos_s(rwData->eyeLerpPhase);

    // VEC3F_LERPIMPDST(eye, eye, &vEyeTarget, fabsf(eyeLerp))
    eye->x += (vEyeTarget.x - eye->x) * fabsf(eyeLerp);
    eye->y += (vEyeTarget.y - eye->y) * fabsf(eyeLerp);
    eye->z += (vEyeTarget.z - eye->z) * fabsf(eyeLerp);

    // When camera reaches the peak of offset and starts to move down
    // && alternating cycles (sfx plays only every 2nd cycle)
    if ((eyeLerp > rwData->eyeLerp) && !rwData->isSfxOff) {
        Player* player = camera->player;

        rwData->isSfxOff = true;
        Na_StartLinkWalkSe(&player->actor.projectedPos, NA_SE_PL_CRAWL + player->floorSfxOffset, 4.0f);
    } else if (eyeLerp < rwData->eyeLerp) {
        rwData->isSfxOff = false;
    }

    rwData->eyeLerp = eyeLerp;

    camera->player->actor.world.pos = *eyeNext;
    camera->player->actor.world.pos.y = camera->playerGroundY;
    camera->player->actor.shape.rot.y = targetOffset.yaw;

    eyeLerp = (240.0f * eyeLerp) * (rwData->xzSpeed * 0.416667f);
    eyeToAtYaw = rwData->forwardYaw + eyeLerp;

    at->x = eye->x + (sin_s(eyeToAtYaw) * 10.0f);
    at->y = eye->y;
    at->z = eye->z + (cos_s(eyeToAtYaw) * 10.0f);

    camera->roll = s_cush(0, camera->roll, 0.5f, 0xA);

    return true;
}

s32 subject_camerawork_00(Camera* camera) {
    return NOTUSED(camera);
}

s32 data_camerawork_00(Camera* camera) {
    return NOTUSED(camera);
}

s32 data_camerawork_01(Camera* camera) {
    PRINTF("chau!chau!\n");
    return normal_camerawork_01(camera);
}

s32 data_camerawork_02(Camera* camera) {
    return NOTUSED(camera);
}

s32 data_camerawork_03(Camera* camera) {
    return NOTUSED(camera);
}

s32 data_camerawork_04(Camera* camera) {
    s32 pad2[2];
    Data4ReadOnlyData* roData = &camera->paramData.data4.roData;
    VecGeo eyeAtOffset;
    VecGeo atOffset;
    VecGeo eyeNextAtOffset;
    f32 yNormal;
    s16 fov;
    Vec3f* eyeNext = &camera->eyeNext;
    BgCamFuncData* bgCamFuncData;
    Vec3f lookAt;
    s32 pad;
    Data4ReadWriteData* rwData = &camera->paramData.data4.rwData;
    Vec3f* eye = &camera->eye;
    f32 playerHeight;
    Vec3f* at = &camera->at;

    playerHeight = player_get_tall(camera->player);

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));
        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->fov = GET_NEXT_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);

        bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);
        rwData->eyePosRot.pos = s_xyz2xyz_t(&bgCamFuncData->pos);
        rwData->eyePosRot.rot = bgCamFuncData->rot;
        fov = bgCamFuncData->fov;
        rwData->fov = fov;
        if (fov != -1) {
            roData->fov = rwData->fov <= 360 ? rwData->fov : CAM_DATA_SCALED(rwData->fov);
        }

        rwData->flags = bgCamFuncData->flags;
        *eye = rwData->eyePosRot.pos;
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    shrink_and_bitem = roData->interfaceField;

    if (camera->animState == 0) {
        camera->animState++;
        slowly_cushon_set(camera);
    }

    eyeNextAtOffset = sglobe_by_2pos(at, eyeNext);
    center4normal(camera, &eyeNextAtOffset, roData->yOffset, false);
    eyeAtOffset = sglobe_by_2pos(eye, at);

    atOffset.r = eyeAtOffset.r;
    atOffset.yaw = (rwData->flags & 1) ? (CAM_DEG_TO_BINANG(camera->data2) + rwData->eyePosRot.rot.y) : eyeAtOffset.yaw;
    atOffset.pitch =
        (rwData->flags & 2) ? (CAM_DEG_TO_BINANG(camera->data3) + rwData->eyePosRot.rot.x) : eyeAtOffset.pitch;

    *at = translate_by_sglobe(eye, &atOffset);

    lookAt = camera->playerPosRot.pos;
    lookAt.y += playerHeight;

    camera->dist = distance_between(&lookAt, eye);
    camera->roll = 0;
    camera->xzSpeed = 0.0f;
    camera->fov = roData->fov;
    camera->atLERPStepScale = 0;
    return true;
}

/**
 * Hanging off of a ledge
 */
s32 unique_camerawork_01(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f playerWaistPos;
    s16 phiTarget;
    VecGeo sp8C;
    VecGeo unk908PlayerPosOffset;
    VecGeo eyeAtOffset;
    VecGeo eyeNextAtOffset;
    PosRot* playerPosRot = &camera->playerPosRot;
    UNUSED PosRot playerhead;
    Unique1ReadOnlyData* roData = &camera->paramData.uniq1.roData;
    Unique1ReadWriteData* rwData = &camera->paramData.uniq1.rwData;
    s32 pad;
    f32 playerHeight;
    s32 pad2;

    playerHeight = player_get_tall(camera->player);
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->distMin = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->distMax = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->pitchTarget = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->atLERPScaleMax = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    cutflag = 1;

    eyeAtOffset = sglobe_by_2pos(at, eye);
    eyeNextAtOffset = sglobe_by_2pos(at, eyeNext);

    shrink_and_bitem = roData->interfaceField;

    if (camera->animState == 0) {
        camera->playerToAtOffset.y -= camera->playerPosDelta.y;
        rwData->yawTarget = eyeNextAtOffset.yaw;
        rwData->unk_00 = 0.0f;
        playerWaistPos = camera->player->bodyPartsPos[PLAYER_BODYPART_WAIST];
        unk908PlayerPosOffset = sglobe_by_2pos(&playerPosRot->pos, &playerWaistPos);
        rwData->timer = CAM_DEFAULT_ANIM_TIME;
        rwData->yawTargetAdj = ABS((s16)(unk908PlayerPosOffset.yaw - eyeAtOffset.yaw)) < 0x3A98
                                   ? 0
                                   : (((s16)(unk908PlayerPosOffset.yaw - eyeAtOffset.yaw) / rwData->timer) / 4) * 3;
        camera->animState++;
    }

    playerhead = Actor_get_eye(&camera->player->actor);

    camera->yawUpdateRateInv = f_cush(100.0f, camera->yawUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
    camera->pitchUpdateRateInv =
        f_cush(100.0f, camera->pitchUpdateRateInv, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.1f);
    camera->xzOffsetUpdateRate =
        f_cush(0.005f, camera->xzOffsetUpdateRate, CAM_UPDATE_RATE_STEP_SCALE_XZ, 0.01f);
    camera->yOffsetUpdateRate = f_cush(0.01f, camera->yOffsetUpdateRate, CAM_UPDATE_RATE_STEP_SCALE_Y, 0.01f);
    camera->fovUpdateRate = f_cush(CAM_FOV_UPDATE_RATE, camera->fovUpdateRate, 0.05f, 0.1f);

    center4normal(camera, &eyeNextAtOffset, roData->yOffset, true);
    sp8C = sglobe_by_2pos(at, eyeNext);

    camera->dist = calc_default_radius(camera, sp8C.r, roData->distMin, roData->distMax);

    phiTarget = roData->pitchTarget;
    sp8C.pitch = s_cush(phiTarget, eyeNextAtOffset.pitch, 1.0f / camera->pitchUpdateRateInv, 0xA);

    if (sp8C.pitch > CAM_MAX_PITCH) {
        sp8C.pitch = CAM_MAX_PITCH;
    }
    if (sp8C.pitch < -CAM_MAX_PITCH) {
        sp8C.pitch = -CAM_MAX_PITCH;
    }

    if (rwData->timer != 0) {
        rwData->yawTarget += rwData->yawTargetAdj;
        rwData->timer--;
    }

    sp8C.yaw = s_cush2(rwData->yawTarget, eyeNextAtOffset.yaw, 0.5f, 0x2710);
    *eyeNext = translate_by_sglobe(at, &sp8C);
    *eye = *eyeNext;
    check_wall_light(camera, at, eye);
    camera->fov = f_cush(roData->fovTarget, camera->fov, camera->fovUpdateRate, 1.0f);
    camera->roll = 0;
    camera->atLERPStepScale = recover_shift(camera, roData->atLERPScaleMax);
    return true;
}

s32 unique_camerawork_02(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f playerPos;
    VecGeo eyeOffset;
    VecGeo eyeAtOffset;
    s32 pad;
    f32 lerpRateFactor;
    Unique2ReadOnlyData* roData = &camera->paramData.uniq2.roData;
    Unique2ReadWriteData* rwData = &camera->paramData.uniq2.rwData;
    s32 pad2;
    f32 playerHeight;

    playerHeight = player_get_tall(camera->player);

    eyeAtOffset = sglobe_by_2pos(at, eye);

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->distTarget = GET_NEXT_RO_DATA(values);
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    shrink_and_bitem = roData->interfaceField;

    if ((camera->animState == 0) || (rwData->unk_04 != roData->interfaceField)) {
        rwData->unk_04 = roData->interfaceField;
    }

    if (camera->animState == 0) {
        camera->animState = 1;
        slowly_cushon_set(camera);
        rwData->unk_00 = 200.0f;
        if (roData->interfaceField & UNIQUE2_FLAG_4) {
            camera->stateFlags &= ~CAM_STATE_CHECK_BG;
        }
    }

    playerPos = camera->playerPosRot.pos;
    lerpRateFactor = (roData->interfaceField & UNIQUE2_FLAG_0 ? 1.0f : camera->speedRatio);
    at->x = F32_LERPIMP(at->x, playerPos.x, lerpRateFactor * 0.6f);
    at->y = F32_LERPIMP(at->y, playerPos.y + playerHeight + roData->yOffset, 0.4f);
    at->z = F32_LERPIMP(at->z, playerPos.z, lerpRateFactor * 0.6f);
    rwData->unk_00 = F32_LERPIMP(rwData->unk_00, 2.0f, 0.05f); // unused.

    if (roData->interfaceField & UNIQUE2_FLAG_0) {
        eyeOffset = sglobe_by_2pos(at, eyeNext);
        eyeOffset.r = roData->distTarget;
        playerPos = translate_by_sglobe(at, &eyeOffset);
        ez_xyz_cush(&playerPos, eye, 0.25f, 0.25f, 0.2f);
    } else if (roData->interfaceField & UNIQUE2_FLAG_1) {
        if (distance_2d(at, eyeNext) < roData->distTarget) {
            eyeOffset = sglobe_by_2pos(at, eyeNext);
            eyeOffset.yaw = s_cush(eyeOffset.yaw, eyeAtOffset.yaw, 0.1f, 0xA);
            eyeOffset.r = roData->distTarget;
            eyeOffset.pitch = 0;
            *eye = translate_by_sglobe(at, &eyeOffset);
            eye->y = eyeNext->y;
        } else {
            ez_xyz_cush(eyeNext, eye, 0.25f, 0.25f, 0.2f);
        }
    }

    check_wall_light(camera, at, eye);
    camera->dist = distance_between(at, eye);
    camera->roll = 0;
    camera->fov = f_cush(roData->fovTarget, camera->fov, 0.2f, 0.1f);
    camera->atLERPStepScale = recover_shift(camera, 1.0f);
    return true;
}

s32 unique_camerawork_03(Camera* camera) {
    VecGeo sp60;
    f32 playerHeight;
    DoorParams* doorParams = &camera->paramData.doorParams;
    BgCamFuncData* bgCamFuncData;
    Vec3s bgCamRot;
    Unique3ReadWriteData* rwData = &camera->paramData.uniq3.rwData;
    Unique3ReadOnlyData* roData = &camera->paramData.uniq3.roData;
    Vec3f* at = &camera->at;
    PosRot* cameraPlayerPosRot = &camera->playerPosRot;

    playerHeight = player_get_tall(camera->player);
    camera->stateFlags &= ~CAM_STATE_CAM_FUNC_FINISH;

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerHeight));

        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * playerHeight * yNormal;
        roData->fov = GET_NEXT_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    shrink_and_bitem = roData->interfaceField;

    switch (camera->animState) {
        case 0:
            slowly_cushon_set(camera);
            camera->stateFlags &= ~(CAM_STATE_CHECK_BG | CAM_STATE_EXTERNAL_FINISHED);
            rwData->initialFov = camera->fov;
            rwData->initialDist = distance_between(at, &camera->eye);
            camera->animState++;
            FALLTHROUGH;
        case 1:
            if (doorParams->timer1-- > 0) {
                break;
            }

            bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);
            camera->eyeNext = s_xyz2xyz_t(&bgCamFuncData->pos);
            camera->eye = camera->eyeNext;
            bgCamRot = bgCamFuncData->rot;

            sp60.r = 100.0f;
            sp60.yaw = bgCamRot.y;
            sp60.pitch = -bgCamRot.x;

            *at = translate_by_sglobe(&camera->eye, &sp60);
            camera->animState++;
            FALLTHROUGH;
        case 2:
            if (roData->interfaceField & UNIQUE3_FLAG_2) {
                camera->at = cameraPlayerPosRot->pos;
                camera->at.y += playerHeight + roData->yOffset;
            }
            if (doorParams->timer2-- > 0) {
                break;
            }
            camera->animState++;
            FALLTHROUGH;
        case 3:
            camera->stateFlags |= (CAM_STATE_CAM_FUNC_FINISH | CAM_STATE_BLOCK_BG);
            if (camera->stateFlags & CAM_STATE_EXTERNAL_FINISHED) {
                camera->animState++;
            } else {
                break;
            }
            FALLTHROUGH;
        case 4:
            if (roData->interfaceField & UNIQUE3_FLAG_1) {
                camera->stateFlags |= CAM_STATE_CHECK_BG;
                camera->stateFlags &= ~CAM_STATE_EXTERNAL_FINISHED;
                change_camera_set(camera, CAM_SET_PIVOT_IN_FRONT, CAM_REQUEST_SETTING_IGNORE_PRIORITY);
                break;
            }
            doorParams->timer3 = 5;
            if (camera->xzSpeed > 0.001f || CAMERA_CHECK_BTN(&__game->state.input[0], BTN_A) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_B) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CLEFT) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CDOWN) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CUP) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CRIGHT) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_R) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_Z)) {
                camera->animState++;
            } else {
                break;
            }
            FALLTHROUGH;
        case 5:
            camera->fov = f_cush(rwData->initialFov, camera->fov, 0.4f, 0.1f);
            sp60 = sglobe_by_2pos(at, &camera->eye);
            sp60.r = f_cush(100.0f, sp60.r, 0.4f, 0.1f);
            camera->eyeNext = translate_by_sglobe(at, &sp60);
            camera->eye = camera->eyeNext;
            if (doorParams->timer3-- > 0) {
                break;
            }
            camera->animState++;
            FALLTHROUGH;
        default:
            camera->stateFlags |= CAM_STATE_CHECK_BG;
            camera->stateFlags &= ~CAM_STATE_EXTERNAL_FINISHED;
            camera->fov = roData->fov;
            change_camera_set(camera, camera->prevSetting, CAM_REQUEST_SETTING_IGNORE_PRIORITY);
            camera->atLERPStepScale = 0.0f;
            camera->playerToAtOffset.x = camera->at.x - cameraPlayerPosRot->pos.x;
            camera->playerToAtOffset.y = camera->at.y - cameraPlayerPosRot->pos.y;
            camera->playerToAtOffset.z = camera->at.z - cameraPlayerPosRot->pos.z;
            break;
    }

    return true;
}

/**
 * Camera's eye is specified by scene camera data, at point is generated at the intersection
 * of the eye to the player
 */
s32 unique_camerawork_00(Camera* camera) {
    f32 yOffset;
    s16 fov;
    Player* player;
    Vec3f playerPosWithOffset;
    VecGeo atPlayerOffset;
    BgCamFuncData* bgCamFuncData;
    Vec3s bgCamRot;
    PosRot* playerPosRot = &camera->playerPosRot;
    DoorParams* doorParams = &camera->paramData.doorParams;
    Unique0ReadOnlyData* roData = &camera->paramData.uniq0.roData;
    Unique0ReadWriteData* rwData = &camera->paramData.uniq0.rwData;
    Vec3f* eye = &camera->eye;

    yOffset = player_get_tall(camera->player);
    player = camera->player;

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    playerPosWithOffset = playerPosRot->pos;
    playerPosWithOffset.y += yOffset;

    shrink_and_bitem = roData->interfaceField;

    if (camera->animState == 0) {
        slowly_cushon_set(camera);
        camera->stateFlags &= ~CAM_STATE_CHECK_BG;

        bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);
        rwData->eyeAndDirection.point = s_xyz2xyz_t(&bgCamFuncData->pos);

        *eye = camera->eyeNext = rwData->eyeAndDirection.point;
        bgCamRot = bgCamFuncData->rot;
        fov = bgCamFuncData->fov;
        if (fov != -1) {
            camera->fov = fov <= 360 ? fov : CAM_DATA_SCALED(fov);
        }
        rwData->animTimer = bgCamFuncData->timer;
        if (rwData->animTimer == -1) {
            rwData->animTimer = doorParams->timer1 + doorParams->timer2;
        }
        atPlayerOffset.r = distance_between(&playerPosWithOffset, eye);
        atPlayerOffset.yaw = bgCamRot.y;
        atPlayerOffset.pitch = -bgCamRot.x;
        rwData->eyeAndDirection.dir = sglobe2world(&atPlayerOffset);
        Math3D_lineVsPosSuisenCross(&rwData->eyeAndDirection, &playerPosRot->pos, &camera->at);
        rwData->initialPos = playerPosRot->pos;
        camera->animState++;
    }

    if (player->stateFlags1 & PLAYER_STATE1_29) {
        rwData->initialPos = playerPosRot->pos;
    }

    if (roData->interfaceField & UNIQUE0_FLAG_0) {
        if (rwData->animTimer > 0) {
            rwData->animTimer--;
            rwData->initialPos = playerPosRot->pos;
        } else if (!(player->stateFlags1 & PLAYER_STATE1_29) &&
                   ((distance_2d(&playerPosRot->pos, &rwData->initialPos) >= 10.0f) ||
                    CAMERA_CHECK_BTN(&__game->state.input[0], BTN_A) ||
                    CAMERA_CHECK_BTN(&__game->state.input[0], BTN_B) ||
                    CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CLEFT) ||
                    CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CDOWN) ||
                    CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CUP) ||
                    CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CRIGHT) ||
                    CAMERA_CHECK_BTN(&__game->state.input[0], BTN_R) ||
                    CAMERA_CHECK_BTN(&__game->state.input[0], BTN_Z))) {
            camera->dist = distance_between(&camera->at, eye);
            camera->playerToAtOffset.x = camera->at.x - playerPosRot->pos.x;
            camera->playerToAtOffset.y = camera->at.y - playerPosRot->pos.y;
            camera->playerToAtOffset.z = camera->at.z - playerPosRot->pos.z;
            camera->atLERPStepScale = 0.0f;
            camera->stateFlags |= CAM_STATE_CHECK_BG;
            change_camera_set(camera, camera->prevSetting, CAM_REQUEST_SETTING_IGNORE_PRIORITY);
        }
    } else {
        if (rwData->animTimer > 0) {
            rwData->animTimer--;
            if (rwData->animTimer == 0) {
                shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);
            }
        } else {
            rwData->initialPos = playerPosRot->pos;
        }

        if (!(player->stateFlags1 & PLAYER_STATE1_29) &&
            ((0.001f < camera->xzSpeed) || CAMERA_CHECK_BTN(&__game->state.input[0], BTN_A) ||
             CAMERA_CHECK_BTN(&__game->state.input[0], BTN_B) ||
             CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CLEFT) ||
             CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CDOWN) ||
             CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CUP) ||
             CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CRIGHT) ||
             CAMERA_CHECK_BTN(&__game->state.input[0], BTN_R) ||
             CAMERA_CHECK_BTN(&__game->state.input[0], BTN_Z))) {
            camera->dist = distance_between(&camera->at, &camera->eye);
            camera->playerToAtOffset.x = camera->at.x - playerPosRot->pos.x;
            camera->playerToAtOffset.y = camera->at.y - playerPosRot->pos.y;
            camera->playerToAtOffset.z = camera->at.z - playerPosRot->pos.z;
            camera->atLERPStepScale = 0.0f;
            change_camera_set(camera, camera->prevSetting, CAM_REQUEST_SETTING_IGNORE_PRIORITY);
            camera->stateFlags |= CAM_STATE_CHECK_BG;
        }
    }
    return true;
}

s32 unique_camerawork_04(Camera* camera) {
    return NOTUSED(camera);
}

/**
 * Was setup to be used by the camera setting "FOREST_UNUSED"
 */
s32 unique_camerawork_05(Camera* camera) {
    return NOTUSED(camera);
}

/**
 * This function doesn't really update much.
 * Eye/at positions are updated via lockCamera
 */
s32 unique_camerawork_06(Camera* camera) {
    Unique6ReadOnlyData* roData = &camera->paramData.uniq6.roData;
    f32 offset;
    Vec3f sp2C;
    PosRot* playerPosRot = &camera->playerPosRot;

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    shrink_and_bitem = roData->interfaceField;

    if (camera->animState == 0) {
        camera->animState++;
        default_cushon_set(camera);
    }

    if (camera->player != NULL) {
        offset = player_get_tall(camera->player);
        sp2C = playerPosRot->pos;
        sp2C.y += offset;
        camera->dist = distance_between(&sp2C, &camera->eye);
        camera->playerToAtOffset.x = camera->at.x - playerPosRot->pos.x;
        camera->playerToAtOffset.y = camera->at.y - playerPosRot->pos.y;
        camera->playerToAtOffset.z = camera->at.z - playerPosRot->pos.z;
    } else {
        camera->dist = distance_between(&camera->at, &camera->eye);
    }

    if ((roData->interfaceField & UNIQUE6_FLAG_0) && (camera->timer > 0)) {
        camera->timer--;
    }

    return true;
}

/**
 * Camera is at a fixed point specified by the scene's camera data,
 * camera rotates to follow player
 */
s32 unique_camerawork_07(Camera* camera) {
    s32 pad;
    Unique7ReadOnlyData* roData = &camera->paramData.uniq7.roData;
    PosRot* playerPosRot = &camera->playerPosRot;
    VecGeo playerPosEyeOffset;
    s16 fov;
    BgCamFuncData* bgCamFuncData;
    UNUSED Vec3s bgCamRot;
    Vec3f* at = &camera->at;
    Vec3f* eye = &camera->eye;
    Vec3f* eyeNext = &camera->eyeNext;
    Unique7ReadWriteData* rwData = &camera->paramData.uniq7.rwData;

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->fov = GET_NEXT_RO_DATA(values);
        roData->interfaceField = (s16)GET_NEXT_RO_DATA(values);
    }
    CAM_DEBUG_RELOAD_PREG(camera);

    bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);

    *eyeNext = s_xyz2xyz_t(&bgCamFuncData->pos);
    *eye = *eyeNext;
    bgCamRot = bgCamFuncData->rot;

    playerPosEyeOffset = sglobe_by_2pos(eye, &playerPosRot->pos);

    // fov actually goes unused since it's hard set later on.
    fov = bgCamFuncData->fov;
    if (fov == -1) {
        fov = roData->fov * 100.0f;
    }

    if (fov <= 360) {
        fov *= 100;
    }

    shrink_and_bitem = roData->interfaceField;

    if (camera->animState == 0) {
        camera->animState++;
        camera->fov = CAM_DATA_SCALED(fov);
        camera->atLERPStepScale = 0.0f;
        camera->roll = 0;
        rwData->unk_00.x = playerPosEyeOffset.yaw;
    }

    camera->fov = 60.0f;

    // 0x7D0 ~ 10.98 degres.
    rwData->unk_00.x = s_cush2(playerPosEyeOffset.yaw, rwData->unk_00.x, 0.4f, 0x7D0);
    playerPosEyeOffset.pitch = -bgCamFuncData->rot.x * cos_s(playerPosEyeOffset.yaw - bgCamFuncData->rot.y);
    *at = translate_by_sglobe(eye, &playerPosEyeOffset);
    camera->stateFlags |= CAM_STATE_BLOCK_BG;
    return true;
}

s32 unique_camerawork_08(Camera* camera) {
    return NOTUSED(camera);
}

/**
 * OnePoint Cutscene
 */
s32 unique_camerawork_09(Camera* camera) {
    Vec3f atTarget;
    Vec3f eyeTarget;
    Unique9ReadOnlyData* roData = &camera->paramData.uniq9.roData;
    Unique9ReadWriteData* rwData = &camera->paramData.uniq9.rwData;
    f32 invKeyFrameTimer;
    VecGeo eyeNextAtOffset;
    VecGeo scratchGeo;
    VecGeo playerTargetOffset;
    s16 action;
    s16 atFlags;
    s16 eyeFlags;
    s16 pad2;
    PosRot targethead;
    PosRot playerhead;
    PosRot playerPosRot;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f* at = &camera->at;
    Vec3f* eye = &camera->eye;
    Player* player = camera->player;
    Actor* focusActor;
    f32 spB4;
    PosRot atFocusPosRot;
    Vec3f eyeLookAtPos;

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    shrink_and_bitem = roData->interfaceField;

    playerPosRot = Actor_get_world(&camera->player->actor);

    if (camera->animState == 0) {
        camera->animState++;
        rwData->curKeyFrameIdx = -1;
        rwData->keyFrameTimer = 1;
        rwData->unk_38 = 0;
        rwData->playerPos.x = playerPosRot.pos.x;
        rwData->playerPos.y = playerPosRot.pos.y;
        rwData->playerPos.z = playerPosRot.pos.z;
        camera->atLERPStepScale = 0.0f;
        slowly_cushon_set(camera);
    }

    if (rwData->unk_38 == 0 && rwData->keyFrameTimer > 0) {
        rwData->keyFrameTimer--;
    }

    if (rwData->keyFrameTimer == 0) {
        rwData->isNewKeyFrame = true;
        rwData->curKeyFrameIdx++;
        if (rwData->curKeyFrameIdx < ONEPOINT_CS_INFO(camera)->keyFrameCount) {
            rwData->curKeyFrame = &ONEPOINT_CS_INFO(camera)->keyFrames[rwData->curKeyFrameIdx];
            rwData->keyFrameTimer = rwData->curKeyFrame->timerInit;

            if (rwData->curKeyFrame->initField != ONEPOINT_CS_INIT_FIELD_NONE) {
                if (ONEPOINT_CS_INIT_FIELD_IS_TYPE_ACTORCAT(rwData->curKeyFrame->initField)) {
                    attention_part = rwData->curKeyFrame->initField & 0xF;
                } else if (ONEPOINT_CS_INIT_FIELD_IS_TYPE_HUD_VISIBILITY(rwData->curKeyFrame->initField)) {
                    camera_set_shrink_and_bitem(
                        CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, rwData->curKeyFrame->initField, 0));
                } else { // initField is a PlayerCsAction
                    if ((camera->player->stateFlags1 & PLAYER_STATE1_27) &&
                        (player->currentBoots != PLAYER_BOOTS_IRON)) {
                        player_demo_mode_set2(camera->play, camera->target, PLAYER_CSACTION_8);
                        PRINTF("camera: demo: player demo set WAIT\n");
                    } else {
                        PRINTF("camera: demo: player demo set %d\n", rwData->curKeyFrame->initField);
                        player_demo_mode_set2(camera->play, camera->target, rwData->curKeyFrame->initField & 0xFF);
                    }
                }
            }
        } else {
            // We've gone through all the keyframes.
            if (camera->camId != CAM_ID_MAIN) {
                camera->timer = 0;
            }
            return true;
        }
    } else {
        rwData->isNewKeyFrame = false;
    }

    atFlags = rwData->curKeyFrame->viewFlags & 0xFF;
    if (atFlags == 1) {
        rwData->atTarget = rwData->curKeyFrame->atTargetInit;
    } else if (atFlags == 2) {
        if (rwData->isNewKeyFrame) {
            rwData->atTarget.x = camera->play->view.at.x + rwData->curKeyFrame->atTargetInit.x;
            rwData->atTarget.y = camera->play->view.at.y + rwData->curKeyFrame->atTargetInit.y;
            rwData->atTarget.z = camera->play->view.at.z + rwData->curKeyFrame->atTargetInit.z;
        }
    } else if (atFlags == 3) {
        if (rwData->isNewKeyFrame) {
            rwData->atTarget.x = camera->at.x + rwData->curKeyFrame->atTargetInit.x;
            rwData->atTarget.y = camera->at.y + rwData->curKeyFrame->atTargetInit.y;
            rwData->atTarget.z = camera->at.z + rwData->curKeyFrame->atTargetInit.z;
        }
    } else if (atFlags == 4 || atFlags == 0x84) {
        if (camera->target != NULL && camera->target->update != NULL) {
            targethead = Actor_get_eye(camera->target);
            playerhead = Actor_get_eye(&camera->player->actor);
            playerhead.pos.x = playerPosRot.pos.x;
            playerhead.pos.z = playerPosRot.pos.z;
            playerTargetOffset = sglobe_by_2pos(&targethead.pos, &playerhead.pos);
            if (atFlags & (s16)0x8080) {
                scratchGeo.pitch = CAM_DEG_TO_BINANG(rwData->curKeyFrame->atTargetInit.x);
                scratchGeo.yaw = CAM_DEG_TO_BINANG(rwData->curKeyFrame->atTargetInit.y);
                scratchGeo.r = rwData->curKeyFrame->atTargetInit.z;
            } else {
                scratchGeo = world2sglobe(&rwData->curKeyFrame->atTargetInit);
            }
            scratchGeo.yaw += playerTargetOffset.yaw;
            scratchGeo.pitch += playerTargetOffset.pitch;
            rwData->atTarget = translate_by_sglobe(&targethead.pos, &scratchGeo);
        } else {
            if (camera->target == NULL) {
                PRINTF(VT_COL(YELLOW, BLACK) "camera: warning: demo C: actor is not valid\n" VT_RST);
            }

            camera->target = NULL;
            rwData->atTarget = camera->at;
        }
    } else if (atFlags & 0x6060) {
        if (!(atFlags & 4) || rwData->isNewKeyFrame) {
            if (atFlags & 0x2020) {
                focusActor = &camera->player->actor;
            } else if (camera->target != NULL && camera->target->update != NULL) {
                focusActor = camera->target;
            } else {
                camera->target = NULL;
                focusActor = NULL;
            }

            if (focusActor != NULL) {
                if ((atFlags & 0xF) == 1) {
                    atFocusPosRot = Actor_get_eye(focusActor);
                } else if ((atFlags & 0xF) == 2) {
                    atFocusPosRot = Actor_get_world(focusActor);
                } else {
                    atFocusPosRot = Actor_get_shape(focusActor);
                }

                if (atFlags & (s16)0x8080) {
                    scratchGeo.pitch = CAM_DEG_TO_BINANG(rwData->curKeyFrame->atTargetInit.x);
                    scratchGeo.yaw = CAM_DEG_TO_BINANG(rwData->curKeyFrame->atTargetInit.y);
                    scratchGeo.r = rwData->curKeyFrame->atTargetInit.z;
                } else {
                    scratchGeo = world2sglobe(&rwData->curKeyFrame->atTargetInit);
                }

                scratchGeo.yaw += atFocusPosRot.rot.y;
                scratchGeo.pitch -= atFocusPosRot.rot.x;
                rwData->atTarget = translate_by_sglobe(&atFocusPosRot.pos, &scratchGeo);
            } else {
                if (camera->target == NULL) {
                    PRINTF(VT_COL(YELLOW, BLACK) "camera: warning: demo C: actor is not valid\n" VT_RST);
                }
                rwData->atTarget = *at;
            }
        }
    } else {
        rwData->atTarget = *at;
    }

    eyeFlags = rwData->curKeyFrame->viewFlags & 0xFF00;
    if (eyeFlags == 0x100) {
        rwData->eyeTarget = rwData->curKeyFrame->eyeTargetInit;
    } else if (eyeFlags == 0x200) {
        if (rwData->isNewKeyFrame) {
            rwData->eyeTarget.x = camera->play->view.eye.x + rwData->curKeyFrame->eyeTargetInit.x;
            rwData->eyeTarget.y = camera->play->view.eye.y + rwData->curKeyFrame->eyeTargetInit.y;
            rwData->eyeTarget.z = camera->play->view.eye.z + rwData->curKeyFrame->eyeTargetInit.z;
        }
    } else if (eyeFlags == 0x300) {
        if (rwData->isNewKeyFrame) {
            rwData->eyeTarget.x = camera->eyeNext.x + rwData->curKeyFrame->eyeTargetInit.x;
            rwData->eyeTarget.y = camera->eyeNext.y + rwData->curKeyFrame->eyeTargetInit.y;
            rwData->eyeTarget.z = camera->eyeNext.z + rwData->curKeyFrame->eyeTargetInit.z;
        }
    } else if (eyeFlags == 0x400 || eyeFlags == (s16)0x8400 || eyeFlags == 0x500 || eyeFlags == (s16)0x8500) {
        if (camera->target != NULL && camera->target->update != NULL) {
            targethead = Actor_get_eye(camera->target);
            playerhead = Actor_get_eye(&camera->player->actor);
            playerhead.pos.x = playerPosRot.pos.x;
            playerhead.pos.z = playerPosRot.pos.z;
            playerTargetOffset = sglobe_by_2pos(&targethead.pos, &playerhead.pos);
            if (eyeFlags == 0x400 || eyeFlags == (s16)0x8400) {
                eyeLookAtPos = targethead.pos;
            } else {
                eyeLookAtPos = rwData->atTarget;
            }

            if (eyeFlags & (s16)0x8080) {
                scratchGeo.pitch = CAM_DEG_TO_BINANG(rwData->curKeyFrame->eyeTargetInit.x);
                scratchGeo.yaw = CAM_DEG_TO_BINANG(rwData->curKeyFrame->eyeTargetInit.y);
                scratchGeo.r = rwData->curKeyFrame->eyeTargetInit.z;
            } else {
                scratchGeo = world2sglobe(&rwData->curKeyFrame->eyeTargetInit);
            }

            scratchGeo.yaw += playerTargetOffset.yaw;
            scratchGeo.pitch += playerTargetOffset.pitch;
            rwData->eyeTarget = translate_by_sglobe(&eyeLookAtPos, &scratchGeo);
        } else {
            if (camera->target == NULL) {
                PRINTF(VT_COL(YELLOW, BLACK) "camera: warning: demo C: actor is not valid\n" VT_RST);
            }
            camera->target = NULL;
            rwData->eyeTarget = *eyeNext;
        }
    } else if (eyeFlags & 0x6060) {
        if (!(eyeFlags & 0x400) || rwData->isNewKeyFrame) {
            if (eyeFlags & 0x2020) {
                focusActor = &camera->player->actor;
            } else if (camera->target != NULL && camera->target->update != NULL) {
                focusActor = camera->target;
            } else {
                camera->target = NULL;
                focusActor = NULL;
            }

            if (focusActor != NULL) {
                PosRot eyeFocusPosRot;

                if ((eyeFlags & 0xF00) == 0x100) {
                    // head
                    eyeFocusPosRot = Actor_get_eye(focusActor);
                } else if ((eyeFlags & 0xF00) == 0x200) {
                    // world
                    eyeFocusPosRot = Actor_get_world(focusActor);
                } else {
                    // world, shapeRot
                    eyeFocusPosRot = Actor_get_shape(focusActor);
                }

                if (eyeFlags & (s16)0x8080) {
                    scratchGeo.pitch = CAM_DEG_TO_BINANG(rwData->curKeyFrame->eyeTargetInit.x);
                    scratchGeo.yaw = CAM_DEG_TO_BINANG(rwData->curKeyFrame->eyeTargetInit.y);
                    scratchGeo.r = rwData->curKeyFrame->eyeTargetInit.z;
                } else {
                    scratchGeo = world2sglobe(&rwData->curKeyFrame->eyeTargetInit);
                }

                scratchGeo.yaw += eyeFocusPosRot.rot.y;
                scratchGeo.pitch -= eyeFocusPosRot.rot.x;
                rwData->eyeTarget = translate_by_sglobe(&eyeFocusPosRot.pos, &scratchGeo);
            } else {
                if (camera->target == NULL) {
                    PRINTF(VT_COL(YELLOW, BLACK) "camera: warning: demo C: actor is not valid\n" VT_RST);
                }
                camera->target = NULL;
                rwData->eyeTarget = *eyeNext;
            }
        }
    } else {
        rwData->eyeTarget = *eyeNext;
    }

    if (rwData->curKeyFrame->viewFlags == 2) {
        rwData->fovTarget = camera->play->view.fovy;
        rwData->rollTarget = 0;
    } else if (rwData->curKeyFrame->viewFlags == 0) {
        rwData->fovTarget = camera->fov;
        rwData->rollTarget = camera->roll;
    } else {
        rwData->fovTarget = rwData->curKeyFrame->fovTargetInit;
        rwData->rollTarget = CAM_DEG_TO_BINANG(rwData->curKeyFrame->rollTargetInit);
    }

    action = ONEPOINT_CS_GET_ACTION(rwData->curKeyFrame);
    switch (action) {
        case ONEPOINT_CS_ACTION_ID_15:
            // static copy to at/eye/fov/roll
            *at = rwData->atTarget;
            *eyeNext = rwData->eyeTarget;
            camera->fov = rwData->fovTarget;
            camera->roll = rwData->rollTarget;
            camera->stateFlags |= CAM_STATE_BLOCK_BG;
            break;

        case ONEPOINT_CS_ACTION_ID_21:
            // same as 15, but with unk_38 ?
            if (rwData->unk_38 == 0) {
                rwData->unk_38 = 1;
            } else if (camera->stateFlags & CAM_STATE_EXTERNAL_FINISHED) {
                rwData->unk_38 = 0;
                camera->stateFlags &= ~CAM_STATE_EXTERNAL_FINISHED;
            }
            *at = rwData->atTarget;
            *eyeNext = rwData->eyeTarget;
            camera->fov = rwData->fovTarget;
            camera->roll = rwData->rollTarget;
            break;

        case ONEPOINT_CS_ACTION_ID_16:
            // same as 21, but don't unset CAM_STATE_EXTERNAL_FINISHED on stateFlags
            if (rwData->unk_38 == 0) {
                rwData->unk_38 = 1;
            } else if (camera->stateFlags & CAM_STATE_EXTERNAL_FINISHED) {
                rwData->unk_38 = 0;
            }

            *at = rwData->atTarget;
            *eyeNext = rwData->eyeTarget;
            camera->fov = rwData->fovTarget;
            camera->roll = rwData->rollTarget;
            break;

        case ONEPOINT_CS_ACTION_ID_1:
            // linear interpolation of eye/at using the geographic coordinates
            eyeNextAtOffset = sglobe_by_2pos(at, eyeNext);
            rwData->atEyeOffsetTarget = sglobe_by_2pos(&rwData->atTarget, &rwData->eyeTarget);
            invKeyFrameTimer = 1.0f / rwData->keyFrameTimer;
            scratchGeo.r = F32_LERPIMP(eyeNextAtOffset.r, rwData->atEyeOffsetTarget.r, invKeyFrameTimer);
            scratchGeo.pitch = eyeNextAtOffset.pitch +
                               ((s16)(rwData->atEyeOffsetTarget.pitch - eyeNextAtOffset.pitch) * invKeyFrameTimer);
            scratchGeo.yaw =
                eyeNextAtOffset.yaw + ((s16)(rwData->atEyeOffsetTarget.yaw - eyeNextAtOffset.yaw) * invKeyFrameTimer);
            eyeTarget = translate_by_sglobe(at, &scratchGeo);
            goto setEyeNext;

        case ONEPOINT_CS_ACTION_ID_2:
            // linear interpolation of eye/at using the eyeTarget
            invKeyFrameTimer = 1.0f / rwData->keyFrameTimer;
            eyeTarget.x = F32_LERPIMP(camera->eyeNext.x, rwData->eyeTarget.x, invKeyFrameTimer);
            eyeTarget.y = F32_LERPIMP(camera->eyeNext.y, rwData->eyeTarget.y, invKeyFrameTimer);
            eyeTarget.z = F32_LERPIMP(camera->eyeNext.z, rwData->eyeTarget.z, invKeyFrameTimer);

        setEyeNext:
            camera->eyeNext.x =
                f_cush2(eyeTarget.x, camera->eyeNext.x, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->eyeNext.y =
                f_cush2(eyeTarget.y, camera->eyeNext.y, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->eyeNext.z =
                f_cush2(eyeTarget.z, camera->eyeNext.z, rwData->curKeyFrame->lerpStepScale, 1.0f);
            FALLTHROUGH;
        case ONEPOINT_CS_ACTION_ID_9:
        case ONEPOINT_CS_ACTION_ID_10:
            // linear interpolation of at/fov/roll
            invKeyFrameTimer = 1.0f / rwData->keyFrameTimer;
            atTarget.x = F32_LERPIMP(camera->at.x, rwData->atTarget.x, invKeyFrameTimer);
            atTarget.y = F32_LERPIMP(camera->at.y, rwData->atTarget.y, invKeyFrameTimer);
            atTarget.z = F32_LERPIMP(camera->at.z, rwData->atTarget.z, invKeyFrameTimer);
            camera->at.x = f_cush2(atTarget.x, camera->at.x, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->at.y = f_cush2(atTarget.y, camera->at.y, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->at.z = f_cush2(atTarget.z, camera->at.z, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->fov = f_cush2(F32_LERPIMP(camera->fov, rwData->fovTarget, invKeyFrameTimer), camera->fov,
                                            rwData->curKeyFrame->lerpStepScale, 0.01f);
            camera->roll = s_cush2(BINANG_LERPIMPINV(camera->roll, rwData->rollTarget, rwData->keyFrameTimer),
                                             camera->roll, rwData->curKeyFrame->lerpStepScale, 0xA);
            break;

        case ONEPOINT_CS_ACTION_ID_4:
            // linear interpolation of eye/at/fov/roll using the step scale, and spherical coordinates
            eyeNextAtOffset = sglobe_by_2pos(at, eyeNext);
            rwData->atEyeOffsetTarget = sglobe_by_2pos(&rwData->atTarget, &rwData->eyeTarget);
            scratchGeo.r = f_cush(rwData->atEyeOffsetTarget.r, eyeNextAtOffset.r,
                                            rwData->curKeyFrame->lerpStepScale, 0.1f);
            scratchGeo.pitch = s_cush(rwData->atEyeOffsetTarget.pitch, eyeNextAtOffset.pitch,
                                                rwData->curKeyFrame->lerpStepScale, 1);
            scratchGeo.yaw = s_cush(rwData->atEyeOffsetTarget.yaw, eyeNextAtOffset.yaw,
                                              rwData->curKeyFrame->lerpStepScale, 1);
            *eyeNext = translate_by_sglobe(at, &scratchGeo);
            goto setAtFovRoll;

        case ONEPOINT_CS_ACTION_ID_3:
            // linear interplation of eye/at/fov/roll using the step scale using eyeTarget
            camera->eyeNext.x =
                f_cush(rwData->eyeTarget.x, camera->eyeNext.x, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->eyeNext.y =
                f_cush(rwData->eyeTarget.y, camera->eyeNext.y, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->eyeNext.z =
                f_cush(rwData->eyeTarget.z, camera->eyeNext.z, rwData->curKeyFrame->lerpStepScale, 1.0f);
            FALLTHROUGH;
        case ONEPOINT_CS_ACTION_ID_11:
        case ONEPOINT_CS_ACTION_ID_12:
        setAtFovRoll:
            // linear interpolation of at/fov/roll using the step scale.
            camera->at.x = f_cush(rwData->atTarget.x, camera->at.x, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->at.y = f_cush(rwData->atTarget.y, camera->at.y, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->at.z = f_cush(rwData->atTarget.z, camera->at.z, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->fov = f_cush(rwData->fovTarget, camera->fov, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->roll = s_cush(rwData->rollTarget, camera->roll, rwData->curKeyFrame->lerpStepScale, 1);
            break;

        case ONEPOINT_CS_ACTION_ID_13:
            // linear interpolation of at, with rotation around eyeTargetInit.y
            camera->at.x = f_cush(rwData->atTarget.x, camera->at.x, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->at.y += camera->playerPosDelta.y * rwData->curKeyFrame->lerpStepScale;
            camera->at.z = f_cush(rwData->atTarget.z, camera->at.z, rwData->curKeyFrame->lerpStepScale, 1.0f);
            scratchGeo = sglobe_by_2pos(at, eyeNext);
            scratchGeo.yaw += CAM_DEG_TO_BINANG(rwData->curKeyFrame->eyeTargetInit.y);

            // 3A98 ~ 82.40 degrees
            if (scratchGeo.pitch >= 0x3A99) {
                scratchGeo.pitch = 0x3A98;
            }

            if (scratchGeo.pitch < -0x3A98) {
                scratchGeo.pitch = -0x3A98;
            }

            spB4 = scratchGeo.r;
            if (1) {}
            scratchGeo.r = !(spB4 < rwData->curKeyFrame->eyeTargetInit.z)
                               ? f_cush(rwData->curKeyFrame->eyeTargetInit.z, spB4,
                                                  rwData->curKeyFrame->lerpStepScale, 1.0f)
                               : scratchGeo.r;

            *eyeNext = translate_by_sglobe(at, &scratchGeo);
            camera->fov =
                f_cush(F32_LERPIMPINV(camera->fov, rwData->curKeyFrame->fovTargetInit, rwData->keyFrameTimer),
                                 camera->fov, rwData->curKeyFrame->lerpStepScale, 1.0f);
            camera->roll = s_cush(rwData->rollTarget, camera->roll, rwData->curKeyFrame->lerpStepScale, 1);
            break;

        case ONEPOINT_CS_ACTION_ID_24:
            // Set current keyframe to the roll target?
            rwData->curKeyFrameIdx = rwData->rollTarget;
            break;

        case ONEPOINT_CS_ACTION_ID_19: {
            // Change the parent camera (or default)'s mode to normal
            s32 camId = camera->parentCamId <= CAM_ID_NONE ? CAM_ID_MAIN : camera->parentCamId;

            change_camera_mode(camera->play->cameraPtrs[camId], CAM_MODE_NORMAL, true);
        }
            FALLTHROUGH;
        case ONEPOINT_CS_ACTION_ID_18: {
            // copy the current camera to the parent (or default)'s camera.
            s32 camId = camera->parentCamId <= CAM_ID_NONE ? CAM_ID_MAIN : camera->parentCamId;
            Camera* cam = camera->play->cameraPtrs[camId];

            *eye = *eyeNext;
            copyCameraPos(cam, camera);
        }
            FALLTHROUGH;
        default:
            if (camera->camId != CAM_ID_MAIN) {
                camera->timer = 0;
            }
            break;
    }

    *eye = *eyeNext;

    if (rwData->curKeyFrame->actionFlags & ONEPOINT_CS_ACTION_FLAG_BGCHECK) {
        check_wall_light(camera, at, eye);
    }

    if (rwData->curKeyFrame->actionFlags & ONEPOINT_CS_ACTION_FLAG_40) {
        // Set the player's position
        camera->player->actor.world.pos.x = rwData->playerPos.x;
        camera->player->actor.world.pos.z = rwData->playerPos.z;
        if (camera->player->stateFlags1 & PLAYER_STATE1_27 && player->currentBoots != PLAYER_BOOTS_IRON) {
            camera->player->actor.world.pos.y = rwData->playerPos.y;
        }
    } else {
        rwData->playerPos.x = playerPosRot.pos.x;
        rwData->playerPos.y = playerPosRot.pos.y;
        rwData->playerPos.z = playerPosRot.pos.z;
    }

    if (rwData->unk_38 == 0 && camera->timer > 0) {
        camera->timer--;
    }

    if (camera->player != NULL) {
        camera->playerToAtOffset.x = camera->at.x - camera->playerPosRot.pos.x;
        camera->playerToAtOffset.y = camera->at.y - camera->playerPosRot.pos.y;
        camera->playerToAtOffset.z = camera->at.z - camera->playerPosRot.pos.z;
    }

    camera->dist = distance_between(at, eye);
    return true;
}

#if DEBUG_FEATURES
void Camera_DebugPrintSplineArray(char* name, s16 length, CutsceneCameraPoint cameraPoints[]) {
    s32 i;

    PRINTF("static SplinedatZ  %s[] = {\n", name);
    for (i = 0; i < length; i++) {
        PRINTF("    /* key frame %2d */ {\n", i);
        PRINTF("    /*     code     */ %d,\n", cameraPoints[i].continueFlag);
        PRINTF("    /*     z        */ %d,\n", cameraPoints[i].cameraRoll);
        PRINTF("    /*     T        */ %d,\n", cameraPoints[i].nextPointFrame);
        PRINTF("    /*     zoom     */ %f,\n", cameraPoints[i].viewAngle);
        PRINTF("    /*     pos      */ { %d, %d, %d }\n", cameraPoints[i].pos.x, cameraPoints[i].pos.y,
               cameraPoints[i].pos.z);
        PRINTF("    },\n");
    }
    PRINTF("};\n\n");
}
#endif

/**
 * Copies `src` to `dst`, used in demo_camerawork_01
 * Name from AC map: Camera2_SetPos_Demo
 */
static void fv2xyz(Vec3f* src, Vec3f* dst) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

/**
 * Calculates new position from `at` to `pos`, outputs to `dst
 * Name from AC map: Camera2_CalcPos_Demo
 */
static void fvrel2xyz(PosRot* at, Vec3f* pos, Vec3f* dst) {
    VecGeo posGeo;
    Vec3f posCopy;

    fv2xyz(pos, &posCopy);
    posGeo = world2sglobe(&posCopy);
    posGeo.yaw += at->rot.y;
    *dst = translate_by_sglobe(&at->pos, &posGeo);
}

/**
 * Camera follows points specified at pointers to CutsceneCameraPoints,
 * camera->data0 for camera at positions, and camera->data1 for camera eye positions
 * until all keyFrames have been exhausted.
 */
s32 demo_camerawork_01(Camera* camera) {
    s32 pad;
    Demo1ReadOnlyData* roData = &camera->paramData.demo1.roData;
    f32* cameraFOV = &camera->fov;
    Vec3f* at = &camera->at;
    CutsceneCameraPoint* csAtPoints = (CutsceneCameraPoint*)camera->data0;
    CutsceneCameraPoint* csEyePoints = (CutsceneCameraPoint*)camera->data1;
    Vec3f* eye = &camera->eye;
    PosRot curPlayerPosRot;
    Vec3f csEyeUpdate;
    Vec3f csAtUpdate;
    f32 newRoll;
    Vec3f* eyeNext = &camera->eyeNext;
    s16* relativeToPlayer = &camera->data2;
    Demo1ReadWriteData* rwData = &camera->paramData.demo1.rwData;

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    shrink_and_bitem = roData->interfaceField;

    switch (camera->animState) {
        case 0:
            // initialize camera state
            rwData->keyframe = 0;
            rwData->curFrame = 0.0f;
            camera->animState++;
            PRINTF(VT_SGR("1") "%06u:" VT_RST " camera: spline demo: start %s \n", camera->play->state.frames,
                   *relativeToPlayer == 0 ? T("絶対", "absolute") : T("相対", "relative"));

#if DEBUG_FEATURES
            if (PREG(93)) {
                Camera_DebugPrintSplineArray("CENTER", 5, csAtPoints);
                Camera_DebugPrintSplineArray("   EYE", 5, csEyePoints);
            }
#endif
            FALLTHROUGH;
        case 1:
            // follow CutsceneCameraPoints.  function returns 1 if at the end.
            if (Grou_Dospline(&csEyeUpdate, &newRoll, cameraFOV, csEyePoints, &rwData->keyframe, &rwData->curFrame) ||
                Grou_Dospline(&csAtUpdate, &newRoll, cameraFOV, csAtPoints, &rwData->keyframe, &rwData->curFrame)) {
                camera->animState++;
            }
            if (*relativeToPlayer) {
                // if the camera is set to be relative to the player, move the interpolated points
                // relative to the player's position
                if (camera->player != NULL && camera->player->actor.update != NULL) {
                    curPlayerPosRot = Actor_get_world(&camera->player->actor);
                    fvrel2xyz(&curPlayerPosRot, &csEyeUpdate, eyeNext);
                    fvrel2xyz(&curPlayerPosRot, &csAtUpdate, at);
                }
#if DEBUG_FEATURES
                else {
                    PRINTF(VT_COL(RED, WHITE) "camera: spline demo: owner dead\n" VT_RST);
                }
#endif
            } else {
                // simply copy the interpolated values to the eye and at
                fv2xyz(&csEyeUpdate, eyeNext);
                fv2xyz(&csAtUpdate, at);
            }
            *eye = *eyeNext;
            camera->roll = newRoll * 256.0f;
            camera->dist = distance_between(at, eye);
            break;
    }
    return true;
}

s32 demo_camerawork_02(Camera* camera) {
    return NOTUSED(camera);
}

/**
 * Opening large chests.
 * The camera position will be at a fixed point, and rotate around at different intervals.
 * The direction, and initial position is dependent on when the camera was started.
 */
s32 demo_camerawork_03(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    PosRot* camPlayerPosRot = &camera->playerPosRot;
    VecGeo eyeAtOffset;
    VecGeo eyeOffset;
    VecGeo atOffset;
    Vec3f sp74;
    Vec3f sp68;
    Vec3f sp5C;
    f32 temp_f0;
    s32 pad;
    u8 skipUpdateEye = false;
    f32 yOffset = player_get_tall(camera->player);
    s16 angle;
    Demo3ReadOnlyData* roData = &camera->paramData.demo3.roData;
    Demo3ReadWriteData* rwData = &camera->paramData.demo3.rwData;
    s32 pad2;

    camera->stateFlags &= ~CAM_STATE_CAM_FUNC_FINISH;

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->fov = GET_NEXT_RO_DATA(values);
        roData->unk_04 = GET_NEXT_RO_DATA(values); // unused.
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    eyeAtOffset = sglobe_by_2pos(at, eye);

    shrink_and_bitem = roData->interfaceField;

    switch (camera->animState) {
        case 0:
            camera->stateFlags &= ~(CAM_STATE_CHECK_BG | CAM_STATE_EXTERNAL_FINISHED);
            slowly_cushon_set(camera);
            camera->fov = roData->fov;
            camera->roll = rwData->animFrame = 0;
            rwData->initialAt = camPlayerPosRot->pos;
            if (camera->playerGroundY != BGCHECK_Y_MIN) {
                rwData->initialAt.y = camera->playerGroundY;
            }
            angle = camPlayerPosRot->rot.y;
            sp68.x = rwData->initialAt.x + (sin_s(angle) * 40.0f);
            sp68.y = rwData->initialAt.y + 40.0f;
            sp68.z = rwData->initialAt.z + (cos_s(angle) * 40.0f);
            if (camera->play->state.frames & 1) {
                angle -= 0x3FFF;
                rwData->yawDir = 1;
            } else {
                angle += 0x3FFF;
                rwData->yawDir = -1;
            }
            sp74.x = sp68.x + (keygz[1].r * sin_s(angle));
            sp74.y = rwData->initialAt.y + 5.0f;
            sp74.z = sp68.z + (keygz[1].r * cos_s(angle));
            if (check_wall_light(camera, &sp68, &sp74)) {
                rwData->yawDir = -rwData->yawDir;
            }
            atOffset = world2sglobe(&keyadjz[0]);
            atOffset.yaw += camPlayerPosRot->rot.y;
            *at = translate_by_sglobe(&rwData->initialAt, &atOffset);
            eyeOffset.r = keygz[0].r;
            eyeOffset.pitch = keygz[0].pitch;
            eyeOffset.yaw = (keygz[0].yaw * rwData->yawDir) + camPlayerPosRot->rot.y;
            rwData->unk_0C = 1.0f;
            break;
        case 1:
            temp_f0 = (rwData->animFrame - 2) * (1.0f / 146.0f);

            sp5C.x = F32_LERPIMP(keyadjz[0].x, keyadjz[1].x, temp_f0);
            sp5C.y = F32_LERPIMP(keyadjz[0].y, keyadjz[1].y, temp_f0);
            sp5C.z = F32_LERPIMP(keyadjz[0].z, keyadjz[1].z, temp_f0);

            atOffset = world2sglobe(&sp5C);
            atOffset.yaw = (atOffset.yaw * rwData->yawDir) + camPlayerPosRot->rot.y;
            *at = translate_by_sglobe(&rwData->initialAt, &atOffset);

            atOffset.r = F32_LERPIMP(keygz[0].r, keygz[1].r, temp_f0);
            atOffset.pitch = BINANG_LERPIMP(keygz[0].pitch, keygz[1].pitch, temp_f0);
            atOffset.yaw = BINANG_LERPIMP(keygz[0].yaw, keygz[1].yaw, temp_f0);

            eyeOffset.r = atOffset.r;
            eyeOffset.pitch = atOffset.pitch;
            eyeOffset.yaw = (atOffset.yaw * rwData->yawDir) + camPlayerPosRot->rot.y;

            rwData->unk_0C -= (1.0f / 365.0f);
            break;
        case 2:
            temp_f0 = (rwData->animFrame - 0x94) * 0.1f;

            sp5C.x = F32_LERPIMP(keyadjz[1].x, keyadjz[2].x, temp_f0);
            sp5C.y = F32_LERPIMP((keyadjz[1].y - yOffset), keyadjz[2].y, temp_f0);
            sp5C.y += yOffset;
            sp5C.z = F32_LERPIMP(keyadjz[1].z, keyadjz[2].z, temp_f0);

            atOffset = world2sglobe(&sp5C);
            atOffset.yaw = (atOffset.yaw * rwData->yawDir) + camPlayerPosRot->rot.y;
            *at = translate_by_sglobe(&rwData->initialAt, &atOffset);

            atOffset.r = F32_LERPIMP(keygz[1].r, keygz[2].r, temp_f0);
            atOffset.pitch = BINANG_LERPIMP(keygz[1].pitch, keygz[2].pitch, temp_f0);
            atOffset.yaw = BINANG_LERPIMP(keygz[1].yaw, keygz[2].yaw, temp_f0);

            eyeOffset.r = atOffset.r;
            eyeOffset.pitch = atOffset.pitch;
            eyeOffset.yaw = (atOffset.yaw * rwData->yawDir) + camPlayerPosRot->rot.y;
            rwData->unk_0C -= 0.04f;
            break;
        case 3:
            temp_f0 = (rwData->animFrame - 0x9F) * (1.0f / 9.0f);

            sp5C.x = F32_LERPIMP(keyadjz[2].x, keyadjz[3].x, temp_f0);
            sp5C.y = F32_LERPIMP(keyadjz[2].y, keyadjz[3].y, temp_f0);
            sp5C.y += yOffset;
            sp5C.z = F32_LERPIMP(keyadjz[2].z, keyadjz[3].z, temp_f0);

            atOffset = world2sglobe(&sp5C);
            atOffset.yaw = (atOffset.yaw * rwData->yawDir) + camPlayerPosRot->rot.y;
            *at = translate_by_sglobe(&rwData->initialAt, &atOffset);

            atOffset.r = F32_LERPIMP(keygz[2].r, keygz[3].r, temp_f0);
            atOffset.pitch = BINANG_LERPIMP(keygz[2].pitch, keygz[3].pitch, temp_f0);
            atOffset.yaw = BINANG_LERPIMP(keygz[2].yaw, keygz[3].yaw, temp_f0);

            eyeOffset.r = atOffset.r;
            eyeOffset.pitch = atOffset.pitch;
            eyeOffset.yaw = (atOffset.yaw * rwData->yawDir) + camPlayerPosRot->rot.y;
            rwData->unk_0C += (4.0f / 45.0f);
            break;
        case 30:
            camera->stateFlags |= CAM_STATE_BLOCK_BG;
            if (camera->stateFlags & CAM_STATE_EXTERNAL_FINISHED) {
                camera->animState = 4;
            }
            FALLTHROUGH;
        case 10:
        case 20:
            skipUpdateEye = true;
            break;
        case 4:
            eyeOffset.r = 80.0f;
            eyeOffset.pitch = 0;
            eyeOffset.yaw = eyeAtOffset.yaw;
            rwData->unk_0C = 0.1f;
            shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_A, 0);

            if ((rwData->animFrame < 0 || camera->xzSpeed > 0.001f ||
                 CAMERA_CHECK_BTN(&__game->state.input[0], BTN_A) ||
                 CAMERA_CHECK_BTN(&__game->state.input[0], BTN_B) ||
                 CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CLEFT) ||
                 CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CDOWN) ||
                 CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CUP) ||
                 CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CRIGHT) ||
                 CAMERA_CHECK_BTN(&__game->state.input[0], BTN_R) ||
                 CAMERA_CHECK_BTN(&__game->state.input[0], BTN_Z)) &&
                (camera->stateFlags & CAM_STATE_EXTERNAL_FINISHED)) {
                    // clang-format off
        // Note: default for switch inside if statement
        default:
                    // clang-format on
                    camera->stateFlags |= (CAM_STATE_CHECK_BG | CAM_STATE_CAM_FUNC_FINISH);
                    camera->stateFlags &= ~CAM_STATE_EXTERNAL_FINISHED;
                    if (camera->prevBgCamIndex < 0) {
                        change_camera_set(camera, camera->prevSetting, CAM_REQUEST_SETTING_IGNORE_PRIORITY);
                    } else {
                        changeCameraID(camera, camera->prevBgCamIndex);
                        camera->prevBgCamIndex = -1;
                    }
                    shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);
            }
            skipUpdateEye = true;
            break;
    }

    rwData->animFrame++;

    if (rwData->animFrame == 1) {
        camera->animState = 10;
    } else if (rwData->animFrame == 2) {
        camera->animState = 1;
    } else if (rwData->animFrame == 148) {
        camera->animState = 2;
    } else if (rwData->animFrame == 158) {
        camera->animState = 20;
    } else if (rwData->animFrame == 159) {
        camera->animState = 3;
    } else if (rwData->animFrame == 168) {
        camera->animState = 30;
    } else if (rwData->animFrame == 228) {
        camera->animState = 4;
    }

    if (!skipUpdateEye) {
        eyeOffset.r = f_cush(eyeOffset.r, eyeAtOffset.r, rwData->unk_0C, 2.0f);
        eyeOffset.pitch = s_cush(eyeOffset.pitch, eyeAtOffset.pitch, rwData->unk_0C, 0xA);
        eyeOffset.yaw = s_cush(eyeOffset.yaw, eyeAtOffset.yaw, rwData->unk_0C, 0xA);
        *eyeNext = translate_by_sglobe(at, &eyeOffset);
        *eye = *eyeNext;
    }

    camera->dist = distance_between(at, eye);
    camera->atLERPStepScale = 0.1f;
    camera->playerToAtOffset.x = camera->at.x - camPlayerPosRot->pos.x;
    camera->playerToAtOffset.y = camera->at.y - camPlayerPosRot->pos.y;
    camera->playerToAtOffset.z = camera->at.z - camPlayerPosRot->pos.z;
    return true;
}

s32 demo_camerawork_04(Camera* camera) {
    return NOTUSED(camera);
}

/**
 * Sets up a OnePoint attention cutscene
 */
s32 demo_camerawork_05(Camera* camera) {
    static s32 chime = -200;
    f32 eyeTargetDist;
    f32 sp90;
    VecGeo playerTargetGeo;
    VecGeo eyePlayerGeo;
    s16 targetScreenPosX;
    s16 targetScreenPosY;
    s32 pad1;
    PosRot playerhead;
    PosRot targethead;
    Player* player;
    s16 sp4A;
    s32 framesDiff;
    s32 temp_v0;
    s16 t;
    s32 pad2;

    playerhead = Actor_get_eye(&camera->player->actor);
    player = camera->player;
    shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, 0);

    if ((camera->target == NULL) || (camera->target->update == NULL)) {
        if (camera->target == NULL) {
            PRINTF(VT_COL(YELLOW, BLACK) "camera: warning: attention: target is not valid, stop!\n" VT_RST);
        }
        camera->target = NULL;
        return true;
    }

    camera->targetPosRot = Actor_get_eye(camera->target);
    playerTargetGeo = sglobe_by_2pos(&camera->targetPosRot.pos, &camera->playerPosRot.pos);
    attention_part = camera->target->category;
    Actor_display_position_set(camera->play, camera->target, &targetScreenPosX, &targetScreenPosY);
    eyeTargetDist = distance_between(&camera->targetPosRot.pos, &camera->eye);
    eyePlayerGeo = sglobe_by_2pos(&playerhead.pos, &camera->eyeNext);
    sp4A = eyePlayerGeo.yaw - playerTargetGeo.yaw;

    if (camera->target->category == ACTORCAT_PLAYER) {
        // camera is targeting a(the) player actor
        if (eyePlayerGeo.r > 30.0f) {
            // target is player, far from eye
            static OnePointCsFull cuts_of_this[] = {
                {
                    // viewFlags & 0x00FF (at): 2, atTarget is view lookAt + atInit
                    // viewFlags & 0xFF00 (eye): none
                    // action: 15, copy at, eye, roll, fov to camera
                    // result: eye remains in the same location, at is View's lookAt
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0002,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    // viewFlags & 0x00FF (at): 3, atTarget is camera's current at + atInit
                    // viewFlags & 0xFF00 (eye): 3, eyeTarget is the camera's current eye + eyeInit
                    // action: interpolate eye and at.
                    // result: eye and at's y interpolate to become +20 from their current location.
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0303,
                    19,
                    0,
                    45.0f,
                    1.0f,
                    { 0.0f, 20.0f, 0.0f },
                    { 0.0f, 20.0f, 0.0f },
                },
                {
                    // viewFlags & 0x00FF (at): 0 none
                    // viewFlags & 0xFF00 (eye): 0 none
                    // action: 18, copy this camera to default camera.
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[1].timerInit = camera->timer - 1;
            cuts_of_this[1].atTargetInit.z = fqrand() * 10.0f;
            cuts_of_this[1].eyeTargetInit.x = fqrand() * 10.0f;
            ONEPOINT_CS_INFO(camera)->keyFrames = cuts_of_this;
            ONEPOINT_CS_INFO(camera)->keyFrameCount = ARRAY_COUNT(cuts_of_this);
            if (camera->parentCamId != CAM_ID_MAIN) {
                ONEPOINT_CS_INFO(camera)->keyFrameCount--;
            } else {
                camera->timer += cuts_of_this[2].timerInit;
            }
        } else {
            // target is player close to current eye
            static OnePointCsFull cuts_of_this[] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2424,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 10.0f, -20.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    19,
                    0,
                    50.0f,
                    1.0f,
                    { 0.0f, -10.0f, 0.0f },
                    { 0.0f, 0.0f, 60.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[1].eyeTargetInit.x = fqrand() * 10.0f;
            cuts_of_this[1].timerInit = camera->timer - 1;
            ONEPOINT_CS_INFO(camera)->keyFrames = cuts_of_this;
            ONEPOINT_CS_INFO(camera)->keyFrameCount = ARRAY_COUNT(cuts_of_this);
            if (camera->parentCamId != CAM_ID_MAIN) {
                ONEPOINT_CS_INFO(camera)->keyFrameCount--;
            } else {
                camera->timer += cuts_of_this[2].timerInit;
            }
        }
    } else if (playerTargetGeo.r < 30.0f) {
        // distance between player and target is less than 30 units.
        static OnePointCsFull cuts_of_this[] = {
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, true, true),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x0002,
                1,
                0,
                60.0f,
                1.0f,
                { 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f },
            },
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, true, true),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x0303,
                19,
                0,
                45.0f,
                1.0f,
                { 0.0f, -20.0f, 0.0f },
                { 0.0f, -10.0f, 5.0f },
            },
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, true, true),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x0303,
                9,
                0,
                60.0f,
                1.0f,
                { 0.0f, 10.0f, 0.0f },
                { 0.0f, 10.0f, 0.0f },
            },
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x0000,
                1,
                0,
                60.0f,
                1.0f,
                { -1.0f, -1.0f, -1.0f },
                { -1.0f, -1.0f, -1.0f },
            },
        };

        ONEPOINT_CS_INFO(camera)->keyFrames = cuts_of_this;
        ONEPOINT_CS_INFO(camera)->keyFrameCount = ARRAY_COUNT(cuts_of_this);
        if ((targetScreenPosX <= 20) || (targetScreenPosX >= SCREEN_WIDTH - 20) || (targetScreenPosY <= 40) ||
            (targetScreenPosY >= SCREEN_HEIGHT - 40)) {
            cuts_of_this[0].actionFlags = ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, true, false);
            cuts_of_this[0].atTargetInit.y = -30.0f;
            cuts_of_this[0].atTargetInit.x = 0.0f;
            cuts_of_this[0].atTargetInit.z = 0.0f;
            cuts_of_this[0].eyeTargetInit.y = 0.0f;
            cuts_of_this[0].eyeTargetInit.x = 10.0f;
            cuts_of_this[0].eyeTargetInit.z = -50.0f;
        }

        cuts_of_this[1].timerInit = camera->timer - 1;

        if (camera->parentCamId != CAM_ID_MAIN) {
            ONEPOINT_CS_INFO(camera)->keyFrameCount -= 2;
        } else {
            camera->timer += cuts_of_this[2].timerInit + cuts_of_this[3].timerInit;
        }
    } else if (eyeTargetDist < 300.0f && eyePlayerGeo.r < 30.0f) {
        // distance from the camera's current positon and the target is less than 300 units
        // and the distance fromthe camera's current position to the player is less than 30 units
        static OnePointCsFull cuts_of_this[] = {
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, true),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x2141,
                20,
                0,
                45.0f,
                0.2f,
                { 0.0f, 0.0f, 10.0f },
                { 0.0f, 0.0f, 10.0f },
            },
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x0000,
                1,
                0,
                60.0f,
                1.0f,
                { -1.0f, -1.0f, -1.0f },
                { -1.0f, -1.0f, -1.0f },
            },
        };

        cuts_of_this[0].timerInit = camera->timer;
        ONEPOINT_CS_INFO(camera)->keyFrames = cuts_of_this;
        ONEPOINT_CS_INFO(camera)->keyFrameCount = ARRAY_COUNT(cuts_of_this);
        if (camera->parentCamId != CAM_ID_MAIN) {
            ONEPOINT_CS_INFO(camera)->keyFrameCount--;
        } else {
            camera->timer += cuts_of_this[1].timerInit;
        }
    } else if (eyeTargetDist < 700.0f && ABS(sp4A) < 0x36B0) {
        // The distance between the camera's current position and the target is less than 700 units
        // and the angle between the camera's position and the player, and the player to the target
        // is less than ~76.9 degrees
        if ((targetScreenPosX > 20) && (targetScreenPosX < SCREEN_WIDTH - 20) && (targetScreenPosY > 40) &&
            (targetScreenPosY < SCREEN_HEIGHT - 40) && (eyePlayerGeo.r > 30.0f)) {
            // The x/y coordinates of the target on screen is between (21, 41) and (300, 200),
            // and the player is farther than 30 units of the eye
            static OnePointCsFull cuts_of_this[] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0303,
                    20,
                    0,
                    45.0f,
                    1.0f,
                    { 0.0f, 0.0f, 0.0f },
                    { 0.0f, 0.0f, 0.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[0].timerInit = camera->timer;
            ONEPOINT_CS_INFO(camera)->keyFrames = cuts_of_this;
            ONEPOINT_CS_INFO(camera)->keyFrameCount = ARRAY_COUNT(cuts_of_this);
            if (camera->parentCamId != CAM_ID_MAIN) {
                ONEPOINT_CS_INFO(camera)->keyFrameCount--;
            } else {
                camera->timer += cuts_of_this[1].timerInit;
            }
        } else {
            // same as above, but the target is NOT within the screen area.
            static OnePointCsFull cuts_of_this[] = {
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0404,
                    20,
                    1,
                    50.0f,
                    1.0f,
                    { 0.0f, 5.0f, 10.0f },
                    { 0.0f, 10.0f, -80.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, true),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x2121,
                    5,
                    0,
                    60.0f,
                    1.0f,
                    { 0.0f, 5.0f, 0.0f },
                    { 5.0f, 5.0f, -200.0f },
                },
                {
                    ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                    ONEPOINT_CS_INIT_FIELD_NONE,
                    0x0000,
                    1,
                    0,
                    60.0f,
                    1.0f,
                    { -1.0f, -1.0f, -1.0f },
                    { -1.0f, -1.0f, -1.0f },
                },
            };

            cuts_of_this[0].atTargetInit.z = eyeTargetDist * 0.6f;
            cuts_of_this[0].eyeTargetInit.z = eyeTargetDist + 50.0f;
            cuts_of_this[0].eyeTargetInit.x = fqrand() * 10.0f;
            if ((s16)(eyePlayerGeo.yaw - playerTargetGeo.yaw) > 0) {
                cuts_of_this[0].atTargetInit.x = -cuts_of_this[0].atTargetInit.x;
                cuts_of_this[0].eyeTargetInit.x = -cuts_of_this[0].eyeTargetInit.x;
                cuts_of_this[0].rollTargetInit = -cuts_of_this[0].rollTargetInit;
            }
            cuts_of_this[0].timerInit = camera->timer;
            cuts_of_this[1].timerInit = (s16)(eyeTargetDist * 0.005f) + 8;
            ONEPOINT_CS_INFO(camera)->keyFrames = cuts_of_this;
            ONEPOINT_CS_INFO(camera)->keyFrameCount = ARRAY_COUNT(cuts_of_this);
            if (camera->parentCamId != CAM_ID_MAIN) {
                ONEPOINT_CS_INFO(camera)->keyFrameCount -= 2;
            } else {
                camera->timer += cuts_of_this[1].timerInit + cuts_of_this[2].timerInit;
            }
        }
    } else if (camera->target->category == ACTORCAT_DOOR) {
        // the target is a door.
        static OnePointCsFull cuts_of_this[] = {
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, false),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0xC1C1,
                20,
                0,
                60.0f,
                1.0f,
                { 0.0f, 0.0f, 50.0f },
                { 0.0f, 0.0f, 250.0f },
            },
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_3, false, true),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x05B1,
                5,
                0,
                60.0f,
                0.1f,
                { 0.0f, 10.0f, 50.0f },
                { 0.0f, 10.0f, 100.0f },
            },
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, true),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x2121,
                5,
                2,
                60.0f,
                1.0f,
                { 0.0f, 10.0f, 0.0f },
                { 0.0f, 20.0f, -150.0f },
            },
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x0000,
                1,
                0,
                60.0f,
                1.0f,
                { -1.0f, -1.0f, -1.0f },
                { -1.0f, -1.0f, -1.0f },
            },
        };

        cuts_of_this[0].timerInit = camera->timer - 5;
        sp4A = 0;
        if (!Game_play_getDoorAngle(camera->play, camera->target, &sp4A)) {
            PRINTF(VT_COL(YELLOW, BLACK) "camera: attention demo: this door is dummy door!\n" VT_RST);
            if (ABS(playerTargetGeo.yaw - camera->target->shape.rot.y) >= 0x4000) {
                sp4A = camera->target->shape.rot.y;
            } else {
                sp4A = camera->target->shape.rot.y - 0x7FFF;
            }
        }

        cuts_of_this[0].atTargetInit.y = cuts_of_this[0].eyeTargetInit.y = cuts_of_this[1].atTargetInit.y =
            camera->target->shape.rot.y == sp4A ? 180.0f : 0.0f;
        sp90 = ((s16)(playerTargetGeo.yaw - sp4A) < 0 ? 20.0f : -20.0f) * fqrand();
        cuts_of_this[0].eyeTargetInit.y = cuts_of_this->eyeTargetInit.y + sp90;
        temp_v0 = fqrand() * (sp90 * -0.2f);
        cuts_of_this[1].rollTargetInit = temp_v0;
        cuts_of_this[0].rollTargetInit = temp_v0;
        targethead = Actor_get_eye(camera->target);
        targethead.pos.x += 50.0f * sin_s(sp4A - 0x7FFF);
        targethead.pos.z += 50.0f * cos_s(sp4A - 0x7FFF);
        if (check_wall_light(camera, &playerhead.pos, &targethead.pos)) {
            cuts_of_this[1].actionFlags = ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_1, true, true);
            cuts_of_this[2].actionFlags = ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true);
        } else {
            cuts_of_this[2].timerInit = (s16)(eyeTargetDist * 0.004f) + 6;
        }
        ONEPOINT_CS_INFO(camera)->keyFrames = cuts_of_this;
        ONEPOINT_CS_INFO(camera)->keyFrameCount = ARRAY_COUNT(cuts_of_this);
        if (camera->parentCamId != CAM_ID_MAIN) {
            ONEPOINT_CS_INFO(camera)->keyFrameCount -= 2;
        } else {
            camera->timer += cuts_of_this[2].timerInit + cuts_of_this[3].timerInit;
        }
    } else {
        // otherwise
        static OnePointCsFull cuts_of_this[] = {
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x0504,
                20,
                2,
                60.0f,
                1.0f,
                { 0.0f, 5.0f, 50.0f },
                { 0.0f, 20.0f, 300.0f },
            },
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_2, false, true),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x2121,
                5,
                2,
                60.0f,
                1.0f,
                { 0.0f, 10.0f, 0.0f },
                { 0.0f, 20.0f, -150.0f },
            },
            {
                ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_18, false, false),
                ONEPOINT_CS_INIT_FIELD_NONE,
                0x0000,
                1,
                0,
                60.0f,
                1.0f,
                { -1.0f, -1.0f, -1.0f },
                { -1.0f, -1.0f, -1.0f },
            },
        };

        if (playerTargetGeo.r < 200.0f) {
            cuts_of_this[0].eyeTargetInit.z = playerTargetGeo.r;
            cuts_of_this[0].atTargetInit.z = playerTargetGeo.r * 0.25f;
        }
        if (playerTargetGeo.r < 400.0f) {
            cuts_of_this[0].eyeTargetInit.x = fqrand() * 25.0f;
        }
        player_get_tall(camera->player);
        cuts_of_this[0].timerInit = camera->timer;
        targethead = Actor_get_eye(camera->target);
        if (check_wall_light(camera, &playerhead.pos, &targethead.pos)) {
            cuts_of_this[1].timerInit = 4;
            cuts_of_this[1].actionFlags = ONEPOINT_CS_ACTION(ONEPOINT_CS_ACTION_ID_15, false, true);
        } else {
            t = eyeTargetDist * 0.005f;
            cuts_of_this[1].timerInit = t + 8;
        }
        ONEPOINT_CS_INFO(camera)->keyFrames = cuts_of_this;
        ONEPOINT_CS_INFO(camera)->keyFrameCount = ARRAY_COUNT(cuts_of_this);
        if (camera->parentCamId != CAM_ID_MAIN) {
            if (camera->play->state.frames & 1) {
                cuts_of_this[0].rollTargetInit = -cuts_of_this[0].rollTargetInit;
                cuts_of_this[1].rollTargetInit = -cuts_of_this[1].rollTargetInit;
            }
            ONEPOINT_CS_INFO(camera)->keyFrameCount -= 2;
        } else {
            camera->timer += cuts_of_this[1].timerInit + cuts_of_this[2].timerInit;
            cuts_of_this[0].rollTargetInit = cuts_of_this[1].rollTargetInit = 0;
        }
    }

    framesDiff = chime - camera->play->state.frames;
    if ((framesDiff > 50) || (framesDiff < -50)) {
        Na_StartSystemSe_F((u32)camera->data1);
    }

    chime = camera->play->state.frames;

    if (camera->player->stateFlags1 & PLAYER_STATE1_27 && (player->currentBoots != PLAYER_BOOTS_IRON)) {
        // swimming, and not iron boots
        player->stateFlags1 |= PLAYER_STATE1_29;
        // env frozen
        player->actor.freezeTimer = camera->timer;
    } else {
        sp4A = playerhead.rot.y - playerTargetGeo.yaw;
        if (camera->target->category == ACTORCAT_PLAYER) {
            framesDiff = camera->play->state.frames - attent_frame;
            if (player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
                // holding object over head.
                player_demo_mode_set(camera->play, camera->target, PLAYER_CSACTION_8);
            } else if (ABS(framesDiff) > 3000) {
                player_demo_mode_set(camera->play, camera->target, PLAYER_CSACTION_12);
            } else {
                player_demo_mode_set(camera->play, camera->target, PLAYER_CSACTION_69);
            }
        } else {
            player_demo_mode_set(camera->play, camera->target, PLAYER_CSACTION_1);
        }
    }

    attent_frame = camera->play->state.frames;
    change_camera_set(camera, CAM_SET_CS_C,
                              CAM_REQUEST_SETTING_PRESERVE_BG_CAM_INDEX | CAM_REQUEST_SETTING_FORCE_CHANGE);

    unique_camerawork_09(camera);

    return true;
}

/**
 * Used in Forest Temple when poes are defeated, follows the flames to the torches.
 * Fixed position, rotates to follow the target
 */
s32 demo_camerawork_06(Camera* camera) {
    Camera* mainCam;
    Demo6ReadOnlyData* roData = &camera->paramData.demo6.roData;
    Vec3f* eyeNext = &camera->eyeNext;
    Demo6ReadWriteData* rwData = &camera->paramData.demo6.rwData;
    VecGeo eyeOffset;
    Actor* camFocus;
    PosRot focusPosRot;
    s16 stateTimers[4];

    mainCam = Gama_play_get_camera(camera->play, CAM_ID_MAIN);
    camFocus = camera->target;
    stateTimers[1] = 55;
    stateTimers[2] = 70;
    stateTimers[3] = 90;

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    switch (camera->animState) {
        case 0:
            // initializes the camera state.
            rwData->animTimer = 0;
            camera->fov = 60.0f;
            focusPosRot = Actor_get_world(camFocus);
            camera->at.x = focusPosRot.pos.x;
            camera->at.y = focusPosRot.pos.y + 20.0f;
            camera->at.z = focusPosRot.pos.z;
            eyeOffset.r = 200.0f;
            // 0x7D0 ~10.99 degrees
            eyeOffset.yaw = get_y_angle_by_2pos(&focusPosRot.pos, &mainCam->playerPosRot.pos) + 0x7D0;
            // -0x3E8 ~5.49 degrees
            eyeOffset.pitch = -0x3E8;
            *eyeNext = translate_by_sglobe(&camera->at, &eyeOffset);
            camera->eye = *eyeNext;
            camera->animState++;
            FALLTHROUGH;
        case 1:
            if (stateTimers[camera->animState] < rwData->animTimer) {
                player_demo_mode_set(camera->play, &camera->player->actor, PLAYER_CSACTION_8);
                focusPosRot = Actor_get_world(camFocus);
                rwData->atTarget.x = focusPosRot.pos.x;
                rwData->atTarget.y = focusPosRot.pos.y - 20.0f;
                rwData->atTarget.z = focusPosRot.pos.z;
                camera->animState++;
            } else {
                break;
            }
            FALLTHROUGH;
        case 2:
            ez_xyz_cush(&rwData->atTarget, &camera->at, 0.1f, 0.1f, 8.0f);
            if (stateTimers[camera->animState] < rwData->animTimer) {
                camera->animState++;
            } else {
                break;
            }
            FALLTHROUGH;
        case 3:
            camera->fov = f_cush(50.0f, camera->fov, 0.2f, 0.01f);
            if (stateTimers[camera->animState] < rwData->animTimer) {
                camera->timer = 0;
                return true;
            }
            break;
    }

    rwData->animTimer++;
    focusPosRot = Actor_get_world(camFocus);

    return true;
}

s32 demo_camerawork_07(Camera* camera) {
    if (camera->animState == 0) {
        camera->stateFlags &= ~CAM_STATE_CHECK_BG;
        camera->stateFlags |= CAM_STATE_DEMO7;
        camera->animState++;
    }
    //! @bug Missing return, but the return value is not used.
}

s32 demo_camerawork_08(Camera* camera) {
    return NOTUSED(camera);
}

/**
 * Camera follows points specified by demo9.atPoints and demo9.eyePoints, allows finer control
 * over the final eye and at points than demo_camerawork_01, by allowing the interpolated at and eye points
 * to be relative to the main camera's player, the current camera's player, or the main camera's target
 */
s32 demo_camerawork_09(Camera* camera) {
    s32 pad;
    s32 finishAction;
    s16 onePointTimer;
    OnePointCamData* onePointCamData = &camera->paramData.demo9.onePointCamData;
    Vec3f csEyeUpdate;
    Vec3f csAtUpdate;
    Vec3f newEye;
    Vec3f newAt;
    f32 newRoll;
    s32 pad3;
    Camera* mainCam;
    Vec3f* eye = &camera->eye;
    PosRot* mainCamPlayerPosRot;
    PosRot focusPosRot;
    Vec3f* eyeNext = &camera->eyeNext;
    Demo9ReadOnlyData* roData = &camera->paramData.demo9.roData;
    Vec3f* at = &camera->at;
    f32* camFOV = &camera->fov;
    Demo9ReadWriteData* rwData = &camera->paramData.demo9.rwData;

    mainCam = Gama_play_get_camera(camera->play, CAM_ID_MAIN);
    mainCamPlayerPosRot = &mainCam->playerPosRot;
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    shrink_and_bitem = roData->interfaceField;

    switch (camera->animState) {
        case 0:
            // initialize the camera state
            rwData->keyframe = 0;
            rwData->finishAction = 0;
            rwData->curFrame = 0.0f;
            camera->animState++;
            rwData->doLERPAt = false;
            finishAction = onePointCamData->actionParameters & 0xF000;
            if (finishAction != 0) {
                rwData->finishAction = finishAction;

                // Clear finish parameters
                onePointCamData->actionParameters &= 0xFFF;
            }
            rwData->animTimer = onePointCamData->initTimer;
            FALLTHROUGH;
        case 1:
            // Run the camera state
            if (rwData->animTimer > 0) {
                // if the animation timer is still running, run the demo logic
                // if it is not, then the case will fallthrough to the finish logic.

                // Run the at and eye cs interpolation functions, if either of them returns 1 (that no more points
                // exist) change the animation state to 2 (standby)
                if (Grou_Dospline(&csEyeUpdate, &newRoll, camFOV, onePointCamData->eyePoints, &rwData->keyframe,
                                  &rwData->curFrame) != 0 ||
                    Grou_Dospline(&csAtUpdate, &newRoll, camFOV, onePointCamData->atPoints, &rwData->keyframe,
                                  &rwData->curFrame) != 0) {
                    camera->animState = 2;
                }

                if (onePointCamData->actionParameters == 1) {
                    // rotate around mainCam's player
                    fvrel2xyz(mainCamPlayerPosRot, &csEyeUpdate, &newEye);
                    fvrel2xyz(mainCamPlayerPosRot, &csAtUpdate, &newAt);
                } else if (onePointCamData->actionParameters == 4) {
                    // rotate around the current camera's player
                    focusPosRot = Actor_get_world(&camera->player->actor);
                    fvrel2xyz(&focusPosRot, &csEyeUpdate, &newEye);
                    fvrel2xyz(&focusPosRot, &csAtUpdate, &newAt);
                } else if (onePointCamData->actionParameters == 8) {
                    // rotate around the current camera's target
                    if (camera->target != NULL && camera->target->update != NULL) {
                        focusPosRot = Actor_get_world(camera->target);
                        fvrel2xyz(&focusPosRot, &csEyeUpdate, &newEye);
                        fvrel2xyz(&focusPosRot, &csAtUpdate, &newAt);
                    } else {
                        camera->target = NULL;
                        newEye = *eye;
                        newAt = *at;
                    }
                } else {
                    // simple copy
                    fv2xyz(&csEyeUpdate, &newEye);
                    fv2xyz(&csAtUpdate, &newAt);
                }

                *eyeNext = newEye;
                *eye = *eyeNext;
                if (rwData->doLERPAt) {
                    ez_xyz_cush(&newAt, at, 0.5f, 0.5f, 0.1f);
                } else {
                    *at = newAt;
                    rwData->doLERPAt = true;
                }
                camera->roll = newRoll * 256.0f;
                rwData->animTimer--;
                break;
            }
            FALLTHROUGH;
        case 3:
            // the cs is finished, decide the next action
            camera->timer = 0;
            if (rwData->finishAction != 0) {
                if (rwData->finishAction != 0x1000) {
                    if (rwData->finishAction == 0x2000) {
                        // finish action = 0x2000, run OnePointCs 0x3FC (Dramatic Return to Link)
                        onePointTimer = onePointCamData->initTimer < 50 ? 5 : onePointCamData->initTimer / 5;
                        makeOnepointDemo(camera->play, 1020, onePointTimer, NULL, camera->parentCamId);
                    }
                } else {
                    // finish action = 0x1000, copy the current camera's values to the
                    // default camera.
                    copyCameraPos(mainCam, camera);
                }
            }
            break;
        case 2:
            // standby while the timer finishes, change the animState to finish when
            // the timer runs out.
            rwData->animTimer--;
            if (rwData->animTimer < 0) {
                camera->animState++;
            }
            break;
        case 4:
            // do nothing.
            break;
    }

    return true;
}

s32 demo_camerawork_00(Camera* camera) {
    return NOTUSED(camera);
}

s32 special_camerawork_00(Camera* camera) {
    PosRot* playerPosRot = &camera->playerPosRot;
    Special0ReadOnlyData* roData = &camera->paramData.spec0.roData;

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->lerpAtScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    shrink_and_bitem = roData->interfaceField;

    if (camera->animState == 0) {
        camera->animState++;
    }

    if ((camera->target == NULL) || (camera->target->update == NULL)) {
        if (camera->target == NULL) {
            PRINTF(VT_COL(YELLOW, BLACK) "camera: warning: circle: target is not valid, stop!\n" VT_RST);
        }
        camera->target = NULL;
        return true;
    }

    camera->targetPosRot = Actor_get_eye(camera->target);
    ez_xyz_cush(&camera->targetPosRot.pos, &camera->at, roData->lerpAtScale, roData->lerpAtScale, 0.1f);

    camera->playerToAtOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->playerToAtOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->playerToAtOffset.z = camera->at.z - playerPosRot->pos.z;

    camera->dist = distance_between(&camera->at, &camera->eye);
    camera->xzSpeed = 0.0f;
    if (camera->timer > 0) {
        camera->timer--;
    }
    return true;
}

s32 special_camerawork_01(Camera* camera) {
    return NOTUSED(camera);
}

s32 special_camerawork_02(Camera* camera) {
    return unique_camerawork_02(camera);
}

s32 special_camerawork_03(Camera* camera) {
    return NOTUSED(camera);
}

s32 special_camerawork_04(Camera* camera) {
    PosRot curTargetPosRot;
    s16 sp3A;
    s16* timer = &camera->timer;
    Special4ReadWriteData* rwData = &camera->paramData.spec4.rwData;

    if (camera->animState == 0) {
        shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, 0);
        camera->fov = 40.0f;
        camera->animState++;
        rwData->initialTimer = camera->timer;
    }

    camera->fov = f_cush(80.0f, camera->fov, 1.0f / *timer, 0.1f);
    if ((rwData->initialTimer - *timer) < 0xF) {
        (*timer)--;
        return false;
    } else {
        camera->roll = -0x1F4;
        curTargetPosRot = Actor_get_world(camera->target);

        camera->at = curTargetPosRot.pos;
        camera->at.y -= 150.0f;

        // 0x3E8 ~ 5.49 degrees
        sp3A = (s16)(curTargetPosRot.rot.y - 0x7FFF) + 0x3E8;
        camera->eye.x = camera->eyeNext.x = (sin_s(sp3A) * 780.0f) + camera->at.x;
        camera->eyeNext.y = camera->at.y;
        camera->eye.z = camera->eyeNext.z = (cos_s(sp3A) * 780.0f) + camera->at.z;
        camera->eye.y = curTargetPosRot.pos.y;
        camera->eye.y = floor_at_light(camera, &camera->eye) + 20.0f;
        (*timer)--;
        return true;
    }
}

/**
 * Flying with hookshot
 */
s32 special_camerawork_05(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    PosRot spA8;
    s16 pad;
    s16 spA4;
    CamColChk sp7C;
    VecGeo sp74;
    VecGeo sp6C;
    UNUSED VecGeo sp64;
    VecGeo sp5C;
    PosRot* playerPosRot = &camera->playerPosRot;
    Special5ReadOnlyData* roData = &camera->paramData.spec5.roData;
    Special5ReadWriteData* rwData = &camera->paramData.spec5.rwData;
    f32 temp_f0_2;
    f32 yOffset;

    yOffset = player_get_tall(camera->player);
    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        f32 yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / yOffset));

        roData->yOffset = (GET_NEXT_SCALED_RO_DATA(values) * yOffset) * yNormal;
        roData->eyeDist = GET_NEXT_RO_DATA(values);
        roData->minDistForRot = GET_NEXT_RO_DATA(values);
        roData->timerInit = GET_NEXT_RO_DATA(values);
        roData->pitch = CAM_DEG_TO_BINANG(GET_NEXT_RO_DATA(values));
        roData->fovTarget = GET_NEXT_RO_DATA(values);
        roData->atMaxLERPScale = GET_NEXT_SCALED_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    sp64 = sglobe_by_2pos(at, eye);
    sp5C = sglobe_by_2pos(at, eyeNext);
    spA8 = Actor_get_world(camera->target);

    shrink_and_bitem = roData->interfaceField;

    if (camera->animState == 0) {
        camera->animState++;
        rwData->animTimer = roData->timerInit;
    }

    if (rwData->animTimer > 0) {
        rwData->animTimer--;
    } else if (rwData->animTimer == 0) {
        if (camera->target == NULL || camera->target->update == NULL) {
            camera->target = NULL;
            return true;
        }

        rwData->animTimer--;
        if (roData->minDistForRot < distance_between(&spA8.pos, &playerPosRot->pos)) {
            sp6C.yaw = playerPosRot->rot.y;
            sp6C.pitch = -playerPosRot->rot.x;
            sp6C.r = 20.0f;
            sp7C.pos = translate_by_sglobe(&spA8.pos, &sp6C);
            check_wall(camera, at, &sp7C);
            sp6C = world2sglobe(&sp7C.norm);
            spA4 = playerPosRot->rot.y - sp6C.yaw;
            sp74.r = roData->eyeDist;
            temp_f0_2 = fqrand();
            sp74.yaw =
                (s16)(playerPosRot->rot.y - 0x7FFF) + (s16)(spA4 < 0 ? -(s16)(0x1553 + (s16)(temp_f0_2 * 2730.0f))
                                                                     : (s16)(0x1553 + (s16)(temp_f0_2 * 2730.0f)));
            sp74.pitch = roData->pitch;
            *eyeNext = translate_by_sglobe(&spA8.pos, &sp74);
            *eye = *eyeNext;
            check_wall_light(camera, &spA8.pos, eye);
        }
    }

    center4normal(camera, &sp5C, roData->yOffset, false);
    camera->fov = f_cush(roData->fovTarget, camera->fov, camera->atLERPStepScale * CAM_FOV_UPDATE_RATE, 1.0f);
    camera->roll = s_cush(0, camera->roll, 0.5f, 0xA);
    camera->atLERPStepScale = recover_shift(camera, roData->atMaxLERPScale);
    return true;
}

typedef enum CamElevatorPlatform {
    /* 0 */ CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_LOWER_FLOOR, // ACTOR_BG_HIDAN_ROCK
    /* 1 */ CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_EAST_TOWER,  // ACTOR_BG_HIDAN_FSLIFT
    /* 2 */ CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_WEST_TOWER,  // ACTOR_BG_HIDAN_SYOKU
    /* 3 */ CAM_ELEVATOR_PLATFORM_SPIRIT_TEMPLE_ENTRANCE   // ACTOR_BG_JYA_1FLIFT
} CamElevatorPlatform;

Vec3f pos1[] = {
    { 3050.0f, 700.0f, 0.0f },     // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_LOWER_FLOOR
    { 1755.0f, 3415.0f, -380.0f }, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_EAST_TOWER
    { -3120.0f, 3160.0f, 245.0f }, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_WEST_TOWER
    { 0.0f, -10.0f, 240.0f },      // CAM_ELEVATOR_PLATFORM_SPIRIT_TEMPLE_ENTRANCE
};

Vec3f pos2[] = {
    { 3160.0f, 2150.0f, 0.0f },    // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_LOWER_FLOOR
    { 1515.0f, 4130.0f, -835.0f }, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_EAST_TOWER
    { -3040.0f, 4135.0f, 230.0f }, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_WEST_TOWER
    { -50.0f, 600.0f, -75.0f },    // CAM_ELEVATOR_PLATFORM_SPIRIT_TEMPLE_ENTRANCE
};

// Trigger player y position to swap eye points
f32 th[] = {
    1570.0f, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_LOWER_FLOOR
    3680.0f, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_EAST_TOWER
    3700.0f, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_WEST_TOWER
    395.0f,  // CAM_ELEVATOR_PLATFORM_SPIRIT_TEMPLE_ENTRANCE
};

f32 fh[] = {
    320.0f, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_LOWER_FLOOR
    320.0f, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_EAST_TOWER
    320.0f, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_WEST_TOWER
    0.0f,   // CAM_ELEVATOR_PLATFORM_SPIRIT_TEMPLE_ENTRANCE
};

s16 za[] = {
    -2000, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_LOWER_FLOOR
    -1000, // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_EAST_TOWER
    0,     // CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_WEST_TOWER
    0      // CAM_ELEVATOR_PLATFORM_SPIRIT_TEMPLE_ENTRANCE
};

// unused
s32 MainCamera = 0;
s32 SubCamera = 0;

/**
 * Camera's eye is fixed at points specified at lower or upper points depending on the player's position.
 * Designed around 4 specific elevator platforms, 1 in spirit temple and 3 in fire temple.
 * Used by `CAM_SET_ELEVATOR_PLATFORM`
 */
s32 special_camerawork_07(Camera* camera) {
    Special7ReadWriteData* rwData = &camera->paramData.spec7.rwData;
    PosRot* playerPosRot = &camera->playerPosRot;
    Vec3f atTarget;
    f32 yOffset = player_get_tall(camera->player);
    f32 fovRollParam;

    if (camera->animState == 0) {
        // Use sceneIds and hardcoded positions in the fire temple to identify the 4 platforms
        if (camera->play->sceneId == SCENE_SPIRIT_TEMPLE) {
            rwData->index = CAM_ELEVATOR_PLATFORM_SPIRIT_TEMPLE_ENTRANCE;
        } else {
            // Hardcoded positions in the fire temple
            if (playerPosRot->pos.x < 1500.0f) {
                rwData->index = CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_WEST_TOWER;
            } else if (playerPosRot->pos.y < 3000.0f) {
                rwData->index = CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_LOWER_FLOOR;
            } else {
                rwData->index = CAM_ELEVATOR_PLATFORM_FIRE_TEMPLE_EAST_TOWER;
            }
        }
        camera->animState++;
        camera->roll = 0;
    }

    if (camera->at.y < th[rwData->index]) {
        // Cam at lower position

        // look at player
        atTarget = playerPosRot->pos;
        atTarget.y -= 20.0f;
        ez_xyz_cush(&atTarget, &camera->at, 0.4f, 0.4f, 0.10f);

        // place camera based on hard-coded positions
        camera->eye = camera->eyeNext = pos1[rwData->index];

        fovRollParam =
            (playerPosRot->pos.y - fh[rwData->index]) /
            (th[rwData->index] - fh[rwData->index]);
        camera->roll = za[rwData->index] * fovRollParam;
        camera->fov = 60.0f + (20.0f * fovRollParam);
    } else {
        // Cam at upper position

        // look at player
        atTarget = playerPosRot->pos;
        atTarget.y += yOffset;
        ez_xyz_cush(&atTarget, &camera->at, 0.4f, 0.4f, 0.1f);

        camera->roll = 0;
        // place camera based on hard-coded positions
        camera->eye = camera->eyeNext = pos2[rwData->index];
        camera->fov = 70.0f;
    }

    camera->dist = distance_between(&camera->at, &camera->eye);
    camera->atLERPStepScale = 0.0f;
    camera->playerToAtOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->playerToAtOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->playerToAtOffset.z = camera->at.z - playerPosRot->pos.z;
    return true;
}

/**
 * Courtyard.
 * Camera's eye is fixed on the z plane, slides on the xy plane with link
 * When the camera's scene data changes the animation to the next "screen"
 * happens for 12 frames.  The camera's eyeNext is the scene's camera data's position
 */
s32 special_camerawork_06(Camera* camera) {
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    VecGeo atOffset;
    Vec3f bgCamPos;
    Vec3f eyePosCalc;
    Vec3f eyeAnim;
    Vec3f atAnim;
    UNUSED VecGeo eyeAtOffset;
    PosRot* playerPosRot = &camera->playerPosRot;
    BgCamFuncData* bgCamFuncData;
    Vec3s bgCamRot;
    s16 fov;
    f32 sp54;
    f32 timerF;
    f32 timerDivisor;
    Special6ReadOnlyData* roData = &camera->paramData.spec6.roData;
    Special6ReadWriteData* rwData = &camera->paramData.spec6.rwData;
    s32 pad;

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    eyeAtOffset = sglobe_by_2pos(eye, at);

    bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);
    bgCamPos = s_xyz2xyz_t(&bgCamFuncData->pos);
    bgCamRot = bgCamFuncData->rot;
    fov = bgCamFuncData->fov;
    if (fov == -1) {
        fov = 6000;
    }

    if (fov <= 360) {
        fov *= 100;
    }

    shrink_and_bitem = roData->interfaceField;

    if (eyeNext->x != bgCamPos.x || eyeNext->y != bgCamPos.y || eyeNext->z != bgCamPos.z || camera->animState == 0) {
        // A change in the current scene's camera positon has been detected,
        // Change "screens"
        camera->player->actor.freezeTimer = 12;
        // Overwrite hud visibility to CAM_HUD_VISIBILITY_HEARTS_FORCE
        shrink_and_bitem = (shrink_and_bitem & (u16)~CAM_HUD_VISIBILITY_MASK) |
                                CAM_HUD_VISIBILITY(CAM_HUD_VISIBILITY_HEARTS_FORCE);
        rwData->initialPlayerY = playerPosRot->pos.y;
        rwData->animTimer = 12;
        *eyeNext = bgCamPos;
        if (camera->animState == 0) {
            camera->animState++;
        }
    }

    if (rwData->animTimer > 0) {
        // In transition between "screens"
        timerF = rwData->animTimer;
        eyePosCalc = *eyeNext;
        eyePosCalc.x += (playerPosRot->pos.x - eyePosCalc.x) * 0.5f;
        eyePosCalc.y += (playerPosRot->pos.y - rwData->initialPlayerY) * 0.2f;
        eyeAnim = eyePosCalc;
        eyeAnim.y = f_cush(eyePosCalc.y, eye->y, 0.5f, 0.01f);

        // set the at point to be 100 units from the eye looking at the
        // direction specified in the scene's camera data.
        atOffset.r = 100.0f;
        atOffset.yaw = bgCamRot.y;
        atOffset.pitch = -bgCamRot.x;
        atAnim = translate_by_sglobe(&eyeAnim, &atOffset);
        timerDivisor = 1.0f / timerF;
        eye->x += (eyeAnim.x - eye->x) * timerDivisor;
        eye->y += (eyeAnim.y - eye->y) * timerDivisor;
        eye->z += (eyeAnim.z - eye->z) * timerDivisor;
        at->x += (atAnim.x - at->x) * timerDivisor;
        at->y += (atAnim.y - at->y) * timerDivisor;
        at->z += (atAnim.z - at->z) * timerDivisor;
        camera->fov += (CAM_DATA_SCALED(fov) - camera->fov) / rwData->animTimer;
        rwData->animTimer--;
    } else {
        // Camera following player on the x axis.
        // Overwrite hud visibility to CAM_HUD_VISIBILITY_ALL
        shrink_and_bitem =
            (shrink_and_bitem & (u16)~CAM_HUD_VISIBILITY_MASK) | CAM_HUD_VISIBILITY(CAM_HUD_VISIBILITY_ALL);
        eyePosCalc = *eyeNext;
        eyePosCalc.x += (playerPosRot->pos.x - eyePosCalc.x) * 0.5f;
        eyePosCalc.y += (playerPosRot->pos.y - rwData->initialPlayerY) * 0.2f;
        *eye = eyePosCalc;
        eye->y = f_cush(eyePosCalc.y, eye->y, 0.5f, 0.01f);

        // set the at point to be 100 units from the eye looking at the
        // direction specified in the scene's camera data.
        atOffset.r = 100.0f;
        atOffset.yaw = bgCamRot.y;
        atOffset.pitch = -bgCamRot.x;
        *at = translate_by_sglobe(eye, &atOffset);
    }
    return true;
}

s32 special_camerawork_08(Camera* camera) {
    return NOTUSED(camera);
}

s32 special_camerawork_09(Camera* camera) {
    s32 pad;
    Vec3f* eye = &camera->eye;
    Vec3f* at = &camera->at;
    Vec3f* eyeNext = &camera->eyeNext;
    Vec3f spAC;
    VecGeo eyeAdjustment;
    VecGeo atEyeOffsetGeo;
    f32 playerYOffset;
    s32 pad3;
    PosRot* playerPosRot = &camera->playerPosRot;
    PosRot referencePosRot;
    f32 yNormal;
    DoorParams* doorParams = &camera->paramData.doorParams;
    Special9ReadOnlyData* roData = &camera->paramData.spec9.roData;
    Special9ReadWriteData* rwData = &camera->paramData.spec9.rwData;
    s32 pad4;
    BgCamFuncData* bgCamFuncData;

    playerYOffset = player_get_tall(camera->player);
    camera->stateFlags &= ~CAM_STATE_CAM_FUNC_FINISH;
    yNormal = 1.0f + CAM_YOFFSET_NORM - (CAM_YOFFSET_NORM * (68.0f / playerYOffset));

    if (RELOAD_PARAMS(camera) || CAM_DEBUG_RELOAD_PARAMS) {
        CameraModeValue* values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;

        roData->yOffset = GET_NEXT_SCALED_RO_DATA(values) * playerYOffset * yNormal;
        roData->unk_04 = GET_NEXT_RO_DATA(values);
        roData->interfaceField = GET_NEXT_RO_DATA(values);
    }

    CAM_DEBUG_RELOAD_PREG(camera);

    if (doorParams->doorActor != NULL) {
        referencePosRot = Actor_get_shape(doorParams->doorActor);
    } else {
        referencePosRot = *playerPosRot;
        referencePosRot.pos.y += playerYOffset + roData->yOffset;
        referencePosRot.rot.x = 0;
    }

    atEyeOffsetGeo = sglobe_by_2pos(at, eye);

    shrink_and_bitem = roData->interfaceField;

    switch (camera->animState) {
        if (1) {}

        case 0:
            camera->stateFlags &= ~(CAM_STATE_CHECK_WATER | CAM_STATE_CHECK_BG);
            camera->animState++;

            //! @bug The angle passed to ABS should be cast to s16.
            //! The lack of a cast means that a door which has an angle of around 0x8000 will calculate an incorrect
            //! angle and use an incorrect `targetYaw`.
            //! In-game, this means if the player opens a susceptible door at a slanted angle, the camera will not
            //! appear correctly on the other side after the player has walked through the door.
            rwData->targetYaw = ABS(playerPosRot->rot.y - referencePosRot.rot.y) >= 0x4000
                                    ? referencePosRot.rot.y - 0x7FFF
                                    : referencePosRot.rot.y;
            FALLTHROUGH;
        case 1:
            doorParams->timer1--;
            if (doorParams->timer1 <= 0) {
                camera->animState++;
                if (roData->interfaceField & SPECIAL9_FLAG_0) {
                    bgCamFuncData = (BgCamFuncData*)get_camera_point_by_cameraID(camera);
                    *eyeNext = s_xyz2xyz_t(&bgCamFuncData->pos);
                    spAC = *eye = *eyeNext;
                } else {
                    s16 yaw;

                    // 0xE38 ~ 20 degrees
                    eyeAdjustment.pitch = 0xE38;
                    // 0xAAA ~ 15 degrees.
                    yaw = 0xAAA * ((camera->play->state.frames & 1) ? 1 : -1);
                    eyeAdjustment.yaw = rwData->targetYaw + yaw;
                    eyeAdjustment.r = 200.0f * yNormal;
                    *eyeNext = translate_by_sglobe(at, &eyeAdjustment);
                    spAC = *eye = *eyeNext;
                    if (out_of_game_area(camera, &spAC, &playerPosRot->pos)) {
                        yaw = -yaw;
                        eyeAdjustment.yaw = rwData->targetYaw + yaw;
                        *eyeNext = translate_by_sglobe(at, &eyeAdjustment);
                        *eye = *eyeNext;
                    }
                }
            } else {
                break;
            }
            FALLTHROUGH;
        case 2:
            spAC = playerPosRot->pos;
            spAC.y += playerYOffset + roData->yOffset;

            ez_xyz_cush(&spAC, at, 0.25f, 0.25f, 0.1f);
            doorParams->timer2--;
            if (doorParams->timer2 <= 0) {
                camera->animState++;
                rwData->targetYaw -= 0x7FFF;
            } else {
                break;
            }
            FALLTHROUGH;
        case 3:
            spAC = playerPosRot->pos;
            spAC.y += (playerYOffset + roData->yOffset);
            ez_xyz_cush(&spAC, at, 0.5f, 0.5f, 0.1f);
            eyeAdjustment.pitch = s_cush(0xAAA, atEyeOffsetGeo.pitch, 0.3f, 0xA);
            eyeAdjustment.yaw = s_cush(rwData->targetYaw, atEyeOffsetGeo.yaw, 0.3f, 0xA);
            eyeAdjustment.r = f_cush(60.0f, atEyeOffsetGeo.r, 0.3f, 1.0f);
            *eyeNext = translate_by_sglobe(at, &eyeAdjustment);
            *eye = *eyeNext;
            doorParams->timer3--;
            if (doorParams->timer3 <= 0) {
                camera->animState++;
            } else {
                break;
            }
            FALLTHROUGH;
        case 4:
            camera->animState++;
            FALLTHROUGH;
        default:
            camera->stateFlags |= (CAM_STATE_CAM_FUNC_FINISH | CAM_STATE_BLOCK_BG);
            shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);

            if (camera->xzSpeed > 0.001f || CAMERA_CHECK_BTN(&__game->state.input[0], BTN_A) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_B) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CLEFT) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CDOWN) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CUP) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_CRIGHT) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_R) ||
                CAMERA_CHECK_BTN(&__game->state.input[0], BTN_Z) || (roData->interfaceField & SPECIAL9_FLAG_3)) {

                change_camera_set(camera, camera->prevSetting, CAM_REQUEST_SETTING_IGNORE_PRIORITY);
                camera->stateFlags |= (CAM_STATE_CHECK_WATER | CAM_STATE_CHECK_BG);
            }
            break;
    }
    if (1) {}
    spAC = playerPosRot->pos;
    spAC.y += playerYOffset;
    camera->dist = distance_between(&spAC, eye);
    camera->playerToAtOffset.x = camera->at.x - playerPosRot->pos.x;
    camera->playerToAtOffset.y = camera->at.y - playerPosRot->pos.y;
    camera->playerToAtOffset.z = camera->at.z - playerPosRot->pos.z;
    return true;
}

Camera* createCamera(View* view, CollisionContext* colCtx, PlayState* play) {
    Camera* newCamera = ZELDA_ARENA_MALLOC(sizeof(*newCamera), "../z_camera.c", 9370);

    if (newCamera != NULL) {
        PRINTF(VT_FGCOL(BLUE) "camera: create --- allocate %d byte" VT_RST "\n", sizeof(*newCamera) * 4);
        initCamera(newCamera, view, colCtx, play);
    } else {
        PRINTF(VT_COL(RED, WHITE) "camera: create: not enough memory\n" VT_RST);
    }
    return newCamera;
}

void destroyCamera(Camera* camera) {
    if (camera != NULL) {
        PRINTF(VT_FGCOL(BLUE) "camera: destroy ---" VT_RST "\n");
        ZELDA_ARENA_FREE(camera, "../z_camera.c", 9391);
    } else {
        PRINTF(VT_COL(YELLOW, BLACK) "camera: destroy: already cleared\n" VT_RST);
    }
}

void initCamera(Camera* camera, View* view, CollisionContext* colCtx, PlayState* play) {
    Camera* camP;
    s16 curUID;
    s16 j;

    memset(camera, 0, sizeof(Camera));
    if (first_time_flag) {
        s32 i;

#if DEBUG_FEATURES
        for (i = 0; i < sOREGInitCnt; i++) {
            OREG(i) = sOREGInit[i];
        }

        for (i = 0; i < n_p_reg_init_data; i++) {
            R_CAM_DATA(i) = p_reg_init_data[i];
        }

        DebugCamera_Reset(camera, &D_8015BD80);
#endif
        first_time_flag = false;
        PREG(88) = -1;
    }
    camera->play = __game = play;
#if DEBUG_FEATURES
    DebugCamera_Init(&D_8015BD80, camera);
#endif
    curUID = camera_uid_number;
    camera_uid_number++;
    while (curUID != 0) {
        if (curUID == 0) {
            camera_uid_number++;
        }

        for (j = 0; j < NUM_CAMS; j++) {
            camP = camera->play->cameraPtrs[j];
            if (camP != NULL && curUID == camP->uid) {
                break;
            }
        }

        if (j == 4) {
            break;
        }

        curUID = camera_uid_number++;
    }

    // ~ 90 degrees
    camera->inputDir.y = 0x3FFF;
    camera->uid = curUID;
    camera->camDir = camera->inputDir;
    camera->rUpdateRateInv = 10.0f;
    camera->yawUpdateRateInv = 10.0f;
    camera->up.y = 1.0f;
    camera->up.z = camera->up.x = 0.0f;
    camera->fov = 60.0f;
    camera->pitchUpdateRateInv = CAM_PITCH_UPDATE_RATE_INV;
    camera->xzOffsetUpdateRate = CAM_XZ_OFFSET_UPDATE_RATE;
    camera->yOffsetUpdateRate = CAM_Y_OFFSET_UPDATE_RATE;
    camera->fovUpdateRate = CAM_FOV_UPDATE_RATE;
    shrink = 32;
    bitem_no = HUD_VISIBILITY_NO_CHANGE;
    camera->stateFlags = 0;
    camera->setting = camera->prevSetting = CAM_SET_FREE0;
    camera->bgCamIndex = camera->prevBgCamIndex = -1;
    camera->mode = 0;
    camera->bgId = BGCHECK_SCENE;
    camera->csId = 0x7FFF;
    camera->timer = -1;
    camera->stateFlags |= CAM_STATE_CAM_INIT;

    camera->up.y = 1.0f;
    camera->up.z = camera->up.x = 0.0f;
    camera->quakeOffset.x = 0;
    camera->quakeOffset.y = 0;
    camera->quakeOffset.z = 0;
    camera->atLERPStepScale = 1;
    shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_IGNORE, 0);
#if DEBUG_FEATURES
    sDbgModeIdx = -1;
#endif
    scene_first = 3;
    PRINTF(VT_FGCOL(BLUE) "camera: initialize --- " VT_RST " UID %d\n", camera->uid);
}

void set_default_cameraset(Camera* camera) {
    if (camera != &camera->play->mainCamera) {
        camera->prevSetting = camera->setting = CAM_SET_FREE0;
        camera->stateFlags &= ~CAM_STATE_CHECK_BG;
    } else if (camera->play->roomCtx.curRoom.roomShape->base.type != ROOM_SHAPE_TYPE_IMAGE) {
        switch (camera->play->roomCtx.curRoom.type) {
            case ROOM_TYPE_DUNGEON:
                setDoorCameraInfo(camera, NULL, -99, 0, 0, 18, 10);
                camera->prevSetting = camera->setting = CAM_SET_DUNGEON0;
                break;
            case ROOM_TYPE_NORMAL:
                PRINTF("camera: room type: default set field\n");
                setDoorCameraInfo(camera, NULL, -99, 0, 0, 18, 10);
                camera->prevSetting = camera->setting = CAM_SET_NORMAL0;
                break;
            default:
                PRINTF("camera: room type: default set etc (%d)\n", camera->play->roomCtx.curRoom.type);
                setDoorCameraInfo(camera, NULL, -99, 0, 0, 18, 10);
                camera->prevSetting = camera->setting = CAM_SET_NORMAL0;
                camera->stateFlags |= CAM_STATE_CHECK_BG;
                break;
        }
    } else {
        PRINTF("camera: room type: prerender\n");
        camera->prevSetting = camera->setting = CAM_SET_FREE0;
        camera->stateFlags &= ~CAM_STATE_CHECK_BG;
    }
}

void releaseCamera(Camera* camera) {
}

void leaveCamera(Camera* camera, Player* player) {
    PosRot playerPosRot;
    VecGeo eyeNextAtOffset;
    s32 bgId;
    Vec3f floorNorm;
    s32 upXZ;
    f32 playerToAtOffsetY;
    Vec3f* at = &camera->at;

    playerPosRot = Actor_get_shape(&player->actor);
    playerToAtOffsetY = player_get_tall(player);
    camera->player = player;
    camera->playerPosRot = playerPosRot;
    camera->dist = eyeNextAtOffset.r = 180.0f;
    camera->inputDir.y = playerPosRot.rot.y;
    eyeNextAtOffset.yaw = camera->inputDir.y - 0x7FFF;
    camera->inputDir.x = eyeNextAtOffset.pitch = 0x71C;
    camera->inputDir.z = 0;
    camera->camDir = camera->inputDir;
    camera->xzSpeed = 0.0f;
    camera->playerPosDelta.y = 0.0f;
    camera->at = playerPosRot.pos;
    camera->at.y += playerToAtOffsetY;

    camera->playerToAtOffset.x = 0;
    camera->playerToAtOffset.y = playerToAtOffsetY;
    camera->playerToAtOffset.z = 0;

    camera->eyeNext = translate_by_sglobe(at, &eyeNextAtOffset);
    camera->eye = camera->eyeNext;
    camera->roll = 0;

    upXZ = 0;
    camera->up.z = upXZ;
    camera->up.y = 1.0f;
    camera->up.x = upXZ;

    if (floor_at(camera, &floorNorm, at, &bgId) != BGCHECK_Y_MIN) {
        camera->bgId = bgId;
    }

    {
        s32 pad[2];

        camera->bgCamIndexBeforeUnderwater = -1;
        camera->waterCamSetting = -1;
    }

    camera->stateFlags |= CAM_STATE_CHECK_BG;

    if (camera == &camera->play->mainCamera) {
        shrink_and_bitem =
            CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE | CAM_LETTERBOX_INSTANT, CAM_HUD_VISIBILITY_NOTHING_ALT, 0);
    } else {
        shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);
    }

    set_default_cameraset(camera);
    camera->behaviorFlags = 0;
    camera->viewFlags = 0;
    camera->nextBgCamIndex = -1;
    camera->atLERPStepScale = 1.0f;
    reset_local_camera(camera, camera->mode);
#if DEBUG_FEATURES
    Camera_QRegInit();
#endif
    PRINTF(VT_FGCOL(BLUE) "camera: personalize ---" VT_RST "\n");

    if (camera->camId == CAM_ID_MAIN) {
        water_check(camera);
    }
}

s16 changeCameraStatus(Camera* camera, s16 status) {
    CameraModeValue* values;
    CameraModeValue* valueP;
    s32 i;

#if DEBUG_FEATURES
    if (PREG(82)) {
        PRINTF("camera: change camera status: cond %c%c\n", status == CAM_STAT_ACTIVE ? 'o' : 'x',
               camera->status != CAM_STAT_ACTIVE ? 'o' : 'x');
    }

    if (PREG(82)) {
        PRINTF("camera: res: stat (%d/%d/%d)\n", camera->camId, camera->setting, camera->mode);
    }

    if (status == CAM_STAT_ACTIVE && camera->status != CAM_STAT_ACTIVE) {
        values = cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].values;
        for (i = 0; i < cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].valueCnt; i++) {
            valueP = &values[i];
            R_CAM_DATA(valueP->dataType) = valueP->val;
            if (PREG(82)) {
                PRINTF("camera: change camera status: PREG(%02d) = %d\n", valueP->dataType, valueP->val);
            }
        }
    }
#endif

    camera->status = status;
    return camera->status;
}

#if DEBUG_FEATURES
void Camera_PrintSettings(Camera* camera) {
    char sp58[8];
    char sp50[8];
    char sp48[8];
    s32 i;

    if ((OREG(0) & 1) && (camera->play->activeCamId == camera->camId) && !debug_camera_sw) {
        for (i = 0; i < NUM_CAMS; i++) {
            if (camera->play->cameraPtrs[i] == NULL) {
                sp58[i] = '-';
                sp48[i] = ' ';
            } else {
                switch (camera->play->cameraPtrs[i]->status) {
                    case 0:
                        sp58[i] = 'c';
                        break;
                    case 1:
                        sp58[i] = 'w';
                        break;
                    case 3:
                        sp58[i] = 's';
                        break;
                    case 7:
                        sp58[i] = 'a';
                        break;
                    case 0x100:
                        sp58[i] = 'd';
                        break;
                    default:
                        sp58[i] = '*';
                        break;
                }
            }
            sp48[i] = ' ';
        }
        sp58[i] = '\0';
        sp48[i] = '\0';

        sp48[camera->play->activeCamId] = 'a';
        Debug_Print2_write(3, 22, DEBUG_CAM_TEXT_WHITE, sp58);
        Debug_Print2_write(3, 22, DEBUG_CAM_TEXT_PEACH, sp48);
        Debug_Print2_write(3, 23, DEBUG_CAM_TEXT_WHITE, "S:");
        Debug_Print2_write(5, 23, DEBUG_CAM_TEXT_GOLD, sCameraSettingNames[camera->setting]);
        Debug_Print2_write(3, 24, DEBUG_CAM_TEXT_WHITE, "M:");
        Debug_Print2_write(5, 24, DEBUG_CAM_TEXT_GOLD, sCameraModeNames[camera->mode]);
        Debug_Print2_write(3, 25, DEBUG_CAM_TEXT_WHITE, "F:");
        Debug_Print2_write(
            5, 25, DEBUG_CAM_TEXT_GOLD,
            sCameraFunctionNames[cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].funcIdx]);

        i = 0;
        if (camera->bgCamIndex < 0) {
            sp50[i++] = '-';
        }

        //! @bug: this code was clearly meaning to print `abs(camera->bgCamIndex)` as a
        //! one-or-two-digit number, instead of `i`.
        // "sp50[i++] = ..." matches here, but is undefined behavior due to conflicting
        // reads/writes between sequence points, triggering warnings. Work around by
        // putting i++ afterwards while on the same line.
        // clang-format off
        if (camera->bgCamIndex / 10 != 0) {
            sp50[i] = i / 10 + '0'; i++;
        }
        sp50[i] = i % 10 + '0'; i++;
        // clang-format on

        sp50[i++] = ' ';
        sp50[i++] = ' ';
        sp50[i++] = ' ';
        sp50[i++] = ' ';
        sp50[i] = '\0';
        Debug_Print2_write(3, 26, DEBUG_CAM_TEXT_WHITE, "I:");
        Debug_Print2_write(5, 26, DEBUG_CAM_TEXT_GOLD, sp50);
    }
}
#endif

static s32 water_check(Camera* camera) {
    f32 waterY;
    s16 quakeIndex;
    s32 waterLightsIndex;
    s32* waterCamSetting = &camera->waterCamSetting;
    s16 waterBgCamIndex;
    s16* waterQuakeIndex = (s16*)&camera->waterQuakeIndex;
    Player* player = camera->player;
    s16 prevBgId;

    if (!(camera->stateFlags & CAM_STATE_CHECK_WATER) || cam_ctl_tbl_0[camera->setting].unk_00 & 0x40000000) {
        return 0;
    }

    // Process player diving in water
    if (camera->stateFlags & CAM_STATE_PLAYER_IN_WATER) {
        if (player->stateFlags2 & PLAYER_STATE2_11) {
            change_camera_set(camera, CAM_SET_PIVOT_WATER_SURFACE,
                                      CAM_REQUEST_SETTING_PRESERVE_BG_CAM_INDEX | CAM_REQUEST_SETTING_IGNORE_PRIORITY);
            camera->stateFlags |= CAM_STATE_PLAYER_DIVING;
        } else if (camera->stateFlags & CAM_STATE_PLAYER_DIVING) {
            change_camera_set(camera, *waterCamSetting,
                                      CAM_REQUEST_SETTING_PRESERVE_BG_CAM_INDEX | CAM_REQUEST_SETTING_IGNORE_PRIORITY);
            camera->stateFlags &= ~CAM_STATE_PLAYER_DIVING;
        }
    }

    // Process player swimming in water
    if (!(camera->stateFlags & CAM_STATE_PLAYER_DIVING)) {
        if (waterBgCamIndex = get_cameraID_from_watersurface(camera, &waterY), waterBgCamIndex == -2) {
            // In a water box that has no bgCamIndex
            if (!(camera->stateFlags & CAM_STATE_PLAYER_IN_WATER)) {
                camera->stateFlags |= CAM_STATE_PLAYER_IN_WATER;
                camera->waterYPos = waterY;
                camera->bgCamIndexBeforeUnderwater = camera->bgCamIndex;
                *waterQuakeIndex = -1;
            }
            if (camera->playerGroundY != camera->playerPosRot.pos.y) {
                prevBgId = camera->bgId;
                camera->bgId = BGCHECK_SCENE;
                change_camera_set(camera, CAM_SET_NORMAL3, CAM_REQUEST_SETTING_IGNORE_PRIORITY);
                *waterCamSetting = camera->setting;
                camera->bgId = prevBgId;
                camera->bgCamIndex = -2;
            }
        } else if (waterBgCamIndex != -1) {
            // In a water box with a bgCamIndex
            if (!(camera->stateFlags & CAM_STATE_PLAYER_IN_WATER)) {
                camera->stateFlags |= CAM_STATE_PLAYER_IN_WATER;
                camera->waterYPos = waterY;
                camera->bgCamIndexBeforeUnderwater = camera->bgCamIndex;
                *waterQuakeIndex = -1;
            }
            if (camera->playerGroundY != camera->playerPosRot.pos.y) {
                prevBgId = camera->bgId;
                camera->bgId = BGCHECK_SCENE;
                changeCameraID(camera, waterBgCamIndex);
                *waterCamSetting = camera->setting;
                camera->bgId = prevBgId;
            }
        } else if (camera->stateFlags & CAM_STATE_PLAYER_IN_WATER) {
            // player is leaving a water box.
            PRINTF("camera: water: off\n");
            camera->stateFlags &= ~CAM_STATE_PLAYER_IN_WATER;
            prevBgId = camera->bgId;
            camera->bgId = BGCHECK_SCENE;
            if (camera->bgCamIndexBeforeUnderwater < 0) {
                set_default_cameraset(camera);
                camera->bgCamIndex = -1;
            } else {
                changeCameraID(camera, camera->bgCamIndexBeforeUnderwater);
            }
            camera->bgId = prevBgId;
        }
    }

    // Process camera in water
    if (waterY = watersurface_at(camera, &camera->eye, &waterLightsIndex), waterY != BGCHECK_Y_MIN) {
        camera->waterYPos = waterY;
        if (!(camera->stateFlags & CAM_STATE_CAMERA_IN_WATER)) {
            camera->stateFlags |= CAM_STATE_CAMERA_IN_WATER;
            PRINTF("kankyo changed water, sound on\n");
            water_in_kankyo(camera->play, waterLightsIndex);
            camera->waterDistortionTimer = 80;
        }

        Na_SetWaterSeModeFlag(0x20);

        if (PREG(81)) {
            stopQuake(*waterQuakeIndex);
            *waterQuakeIndex = -1;
            PREG(81) = 0;
        }

        if ((*waterQuakeIndex == -1) || (getTimerQuake(*waterQuakeIndex) == 10)) {
            quakeIndex = startQuake(camera, QUAKE_TYPE_5);

            *waterQuakeIndex = quakeIndex;
            if (quakeIndex != 0) {
                setSpeedQuake(*waterQuakeIndex, 550);
                setScaleQuake(*waterQuakeIndex, 1, 1, 180, 0);
                setTimerQuake(*waterQuakeIndex, 1000);
            }
        }

        if (camera->waterDistortionTimer > 0) {
            camera->waterDistortionTimer--;
            camera->distortionFlags |= DISTORTION_UNDERWATER_STRONG;
        } else if (camera->play->sceneId == SCENE_FISHING_POND) {
            camera->distortionFlags |= DISTORTION_UNDERWATER_FISHING;
        } else {
            camera->distortionFlags |= DISTORTION_UNDERWATER_WEAK;
        }
    } else {
        if (camera->stateFlags & CAM_STATE_CAMERA_IN_WATER) {
            camera->stateFlags &= ~CAM_STATE_CAMERA_IN_WATER;
            PRINTF("kankyo changed water off, sound off\n");
            water_out_kankyo(camera->play);
            if (*waterQuakeIndex != 0) {
                stopQuake(*waterQuakeIndex);
            }
            camera->waterDistortionTimer = 0;
            camera->distortionFlags = 0;
        }
        Na_SetWaterSeModeFlag(0);
    }
    //! @bug Missing return, but the return value is not used.
}

s32 weather_check(Camera* camera) {
    camera->distortionFlags &= ~DISTORTION_HOT_ROOM;
    if (camera->play->roomCtx.curRoom.environmentType == ROOM_ENV_HOT) {
        camera->distortionFlags |= DISTORTION_HOT_ROOM;
    }

    return 1;
}

#if DEBUG_FEATURES
s32 Camera_DbgChangeMode(Camera* camera) {
    static s16 D_8011DAFC[] = {
        CAM_SET_NORMAL0, CAM_SET_NORMAL1, CAM_SET_NORMAL2, CAM_SET_DUNGEON0, CAM_SET_DUNGEON1, CAM_SET_DUNGEON2,
    };
    s32 changeDir = 0;

    if (!debug_camera_sw && camera->play->activeCamId == CAM_ID_MAIN) {
        if (CAMERA_CHECK_BTN(&__game->state.input[2], BTN_CUP)) {
            PRINTF("attention sound URGENCY\n");
            Na_StartSystemSe_F(NA_SE_SY_ATTENTION_URGENCY);
        }
        if (CAMERA_CHECK_BTN(&__game->state.input[2], BTN_CDOWN)) {
            PRINTF("attention sound NORMAL\n");
            Na_StartSystemSe_F(NA_SE_SY_ATTENTION_ON);
        }

        if (CAMERA_CHECK_BTN(&__game->state.input[2], BTN_CRIGHT)) {
            changeDir = 1;
        }
        if (CAMERA_CHECK_BTN(&__game->state.input[2], BTN_CLEFT)) {
            changeDir = -1;
        }
        if (changeDir != 0) {
            sDbgModeIdx = (sDbgModeIdx + changeDir) % 6;
            if (changeCameraSet(camera, D_8011DAFC[sDbgModeIdx]) > 0) {
                PRINTF("camera: force change SET to %s!\n", sCameraSettingNames[D_8011DAFC[sDbgModeIdx]]);
            }
        }
    }
    return true;
}
#endif

void stretch_view(Camera* camera) {
    static s16 c1 = 0x3F0;
    static s16 c2 = 0x156;
    f32 scaleFactor;
    f32 speedFactor;
    f32 depthPhaseStep;
    f32 screenPlanePhaseStep;
    s32 pad[5];
    f32 xScale;
    f32 yScale;
    f32 zScale;
    f32 speed;

    if (camera->distortionFlags != 0) {
        if (camera->distortionFlags & DISTORTION_UNDERWATER_MEDIUM) {
            depthPhaseStep = 0.0f;
            screenPlanePhaseStep = 170.0f;

            xScale = -0.01f;
            yScale = 0.01f;
            zScale = 0.0f;

            speed = 0.6f;
            scaleFactor = camera->waterDistortionTimer / 60.0f;
            speedFactor = 1.0f;
        } else if (camera->distortionFlags & DISTORTION_UNDERWATER_STRONG) {
            depthPhaseStep = 248.0f;
            screenPlanePhaseStep = -90.0f;

            xScale = -0.3f;
            yScale = 0.3f;
            zScale = 0.2f;

            speed = 0.2f;
            scaleFactor = camera->waterDistortionTimer / 80.0f;
            speedFactor = 1.0f;
        } else if (camera->distortionFlags & DISTORTION_UNDERWATER_WEAK) {
            depthPhaseStep = 359.2f;
            screenPlanePhaseStep = -18.5f;

            xScale = 0.09f;
            yScale = 0.09f;
            zScale = 0.01f;

            speed = 0.08f;
            scaleFactor =
                (((camera->waterYPos - camera->eye.y) > 150.0f ? 1.0f : (camera->waterYPos - camera->eye.y) / 150.0f) *
                 0.45f) +
                (camera->speedRatio * 0.45f);
            speedFactor = scaleFactor;
        } else if (camera->distortionFlags & DISTORTION_HOT_ROOM) {
            // Gives the hot-room a small mirage-like appearance
            depthPhaseStep = 0.0f;
            screenPlanePhaseStep = 150.0f;

            xScale = -0.01f;
            yScale = 0.01f;
            zScale = 0.01f;

            speed = 0.6f;
            speedFactor = 1.0f;
            scaleFactor = 1.0f;
        } else {
            // DISTORTION_UNDERWATER_FISHING
            return;
        }

        c1 += CAM_DEG_TO_BINANG(depthPhaseStep);
        c2 += CAM_DEG_TO_BINANG(screenPlanePhaseStep);

        stretchViewRotate(&camera->play->view, cos_s(c1) * 0.0f, sin_s(c1) * 0.0f,
                                      sin_s(c2) * 0.0f);
        stretchViewScale(&camera->play->view, sin_s(c2) * (xScale * scaleFactor) + 1.0f,
                                cos_s(c2) * (yScale * scaleFactor) + 1.0f,
                                cos_s(c1) * (zScale * scaleFactor) + 1.0f);
        stretchViewSpeed(&camera->play->view, speed * speedFactor);

        camera->stateFlags |= CAM_STATE_DISTORTION;

    } else if (camera->stateFlags & CAM_STATE_DISTORTION) {
        stretchViewReset(&camera->play->view);
        camera->stateFlags &= ~CAM_STATE_DISTORTION;
    }
}

#if DEBUG_FEATURES
#define ENABLE_DEBUG_CAM_UPDATE R_DEBUG_CAM_UPDATE
#else
#define ENABLE_DEBUG_CAM_UPDATE false
#endif

Vec3s actionCameraWork(Camera* camera) {
    static s32 sokonasi = 0;
    Vec3f viewAt;
    Vec3f viewEye;
    Vec3f viewUp;
    f32 viewFov;
    Vec3f pos;
    s32 bgId;
    f32 playerGroundY;
    f32 playerXZSpeed;
    VecGeo eyeAtAngle;
    s16 bgCamIndex;
    s16 numQuakesApplied;
    PosRot curPlayerPosRot;
    ShakeInfo camShake;
    Player* player;

    player = camera->play->cameraPtrs[CAM_ID_MAIN]->player;

    if (ENABLE_DEBUG_CAM_UPDATE) {
        PRINTF("camera: in %x\n", camera);
    }

    if (camera->status == CAM_STAT_CUT) {
        if (ENABLE_DEBUG_CAM_UPDATE) {
            PRINTF("camera: cut out %x\n", camera);
        }
        return camera->inputDir;
    }

    cutflag = false;

    if (camera->player != NULL) {
        curPlayerPosRot = Actor_get_shape(&camera->player->actor);
        camera->xzSpeed = playerXZSpeed = distance_2d(&curPlayerPosRot.pos, &camera->playerPosRot.pos);

        camera->speedRatio = limiter(playerXZSpeed / (player_get_max_speed(camera->player) * CAM_GLOBAL_8), 1.0f);
        camera->playerPosDelta.x = curPlayerPosRot.pos.x - camera->playerPosRot.pos.x;
        camera->playerPosDelta.y = curPlayerPosRot.pos.y - camera->playerPosRot.pos.y;
        camera->playerPosDelta.z = curPlayerPosRot.pos.z - camera->playerPosRot.pos.z;
        pos = curPlayerPosRot.pos;
        pos.y += player_get_tall(camera->player);

        playerGroundY = T_BGCheck_ObjGroundCheck_aiac2(camera->play, &camera->play->colCtx, &floor_poly, &bgId,
                                                   &camera->player->actor, &pos);
        if (playerGroundY != BGCHECK_Y_MIN) {
            // player is above ground.
            camera->floorNorm.x = COLPOLY_GET_NORMAL(floor_poly->normal.x);
            camera->floorNorm.y = COLPOLY_GET_NORMAL(floor_poly->normal.y);
            camera->floorNorm.z = COLPOLY_GET_NORMAL(floor_poly->normal.z);
            camera->bgId = bgId;
            camera->playerGroundY = playerGroundY;
            sokonasi = 0;
        } else {
            // player is not above ground.
            camera->floorNorm.x = 0.0;
            camera->floorNorm.y = 1.0f;
            camera->floorNorm.z = 0.0;
            sokonasi++;
        }

        camera->playerPosRot = curPlayerPosRot;

        if (sokonasi < 200) {
            if (camera->status == CAM_STAT_ACTIVE) {
                water_check(camera);
                weather_check(camera);
            }

            if (!(camera->stateFlags & CAM_STATE_CHECK_BG)) {
                camera->nextBgCamIndex = -1;
            }

            if ((camera->stateFlags & CAM_STATE_CHECK_BG_ALT) && (camera->stateFlags & CAM_STATE_CHECK_BG) &&
                !(camera->stateFlags & CAM_STATE_BLOCK_BG) &&
                (!(camera->stateFlags & CAM_STATE_PLAYER_IN_WATER) || (player->currentBoots == PLAYER_BOOTS_IRON)) &&
                !(camera->stateFlags & CAM_STATE_PLAYER_DIVING) && (playerGroundY != BGCHECK_Y_MIN)) {
                bgCamIndex = get_cameraID_from_polygon(camera, &bgId, floor_poly);
                if (bgCamIndex != -1) {
                    camera->nextBgId = bgId;
                    if (bgId == BGCHECK_SCENE) {
                        camera->nextBgCamIndex = bgCamIndex;
                    }
                }
            }

            if ((camera->nextBgCamIndex != -1) && (fabsf(curPlayerPosRot.pos.y - playerGroundY) < 2.0f) &&
                (!(camera->stateFlags & CAM_STATE_PLAYER_IN_WATER) || (player->currentBoots == PLAYER_BOOTS_IRON))) {
                camera->bgId = camera->nextBgId;
                changeCameraID(camera, camera->nextBgCamIndex);
                camera->nextBgCamIndex = -1;
            }
        }
    }

#if DEBUG_FEATURES
    Camera_PrintSettings(camera);
    Camera_DbgChangeMode(camera);
#endif

    if (camera->status == CAM_STAT_WAIT) {
        if (ENABLE_DEBUG_CAM_UPDATE) {
            PRINTF("camera: wait out %x\n", camera);
        }
        return camera->inputDir;
    }

    camera->behaviorFlags = 0;
    camera->stateFlags &= ~(CAM_STATE_BLOCK_BG | CAM_STATE_LOCK_MODE);
    camera->stateFlags |= CAM_STATE_CAM_FUNC_FINISH;

    if (ENABLE_DEBUG_CAM_UPDATE) {
        PRINTF("camera: engine (%d %d %d) %04x \n", camera->setting, camera->mode,
               cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].funcIdx, camera->stateFlags);
    }

    if (sokonasi < 200) {
        cam_func_tbl_0[cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].funcIdx](camera);
    } else if (camera->player != NULL) {
        eyeAtAngle = sglobe_by_2pos(&camera->at, &camera->eye);
        center4normal(camera, &eyeAtAngle, 0.0f, false);
    }

    if (camera->status == CAM_STAT_ACTIVE) {
        if ((z_common_data.gameMode != GAMEMODE_NORMAL) && (z_common_data.gameMode != GAMEMODE_END_CREDITS)) {
            shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_NONE, CAM_HUD_VISIBILITY_ALL, 0);
            camera_set_shrink_and_bitem(shrink_and_bitem);
        } else if ((scene_first != 0) && (camera->camId == CAM_ID_MAIN)) {
            scene_first--;
            shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, 0);
            camera_set_shrink_and_bitem(shrink_and_bitem);
        } else if (camera->play->transitionMode != TRANS_MODE_OFF) {
            shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_IGNORE, CAM_HUD_VISIBILITY_NOTHING_ALT, 0);
            camera_set_shrink_and_bitem(shrink_and_bitem);
        } else if (camera->play->csCtx.state != CS_STATE_IDLE) {
            // clang-format off
            shrink_and_bitem = CAM_INTERFACE_FIELD(CAM_LETTERBOX_LARGE, CAM_HUD_VISIBILITY_NOTHING_ALT, 0); \
            camera_set_shrink_and_bitem(shrink_and_bitem);
            // clang-format on
        } else {
            camera_set_shrink_and_bitem(shrink_and_bitem);
        }
    }

    if (ENABLE_DEBUG_CAM_UPDATE) {
        PRINTF("camera: shrink_and_bitem %x(%d)\n", shrink_and_bitem, camera->play->transitionMode);
    }

    if (ENABLE_DEBUG_CAM_UPDATE) {
        PRINTF("camera: engine (%s(%d) %s(%d) %s(%d)) ok!\n", &sCameraSettingNames[camera->setting], camera->setting,
               &sCameraModeNames[camera->mode], camera->mode,
               &sCameraFunctionNames[cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].funcIdx],
               cam_ctl_tbl_0[camera->setting].cameraModes[camera->mode].funcIdx);
    }

#if DEBUG_FEATURES
    // enable/disable debug cam
    if (CAMERA_CHECK_BTN(&__game->state.input[2], BTN_START)) {
        debug_camera_sw ^= 1;
        if (debug_camera_sw) {
            DebugCamera_Enable(&D_8015BD80, camera);
        } else if (camera->play->csCtx.state != CS_STATE_IDLE) {
            Demo_play_end(camera->play, &camera->play->csCtx);
        }
    }

    // Debug cam update
    if (debug_camera_sw) {
        camera->play->view.fovy = D_8015BD80.fov;
        DebugactionCameraWork(&D_8015BD80, camera);
        setLookAtView(&camera->play->view, &D_8015BD80.eye, &D_8015BD80.at, &D_8015BD80.unk_1C);
        if (ENABLE_DEBUG_CAM_UPDATE) {
            PRINTF("camera: debug out\n");
        }
        return D_8015BD80.sub.unk_104A;
    }

    OREG(0) &= ~8;
#endif

    if (camera->status == CAM_STAT_UNK3) {
        return camera->inputDir;
    }

    numQuakesApplied = cameraQuakeControl(camera, &camShake);

    bgId = numQuakesApplied; // required to match

    if ((numQuakesApplied != 0) && (camera->setting != CAM_SET_TURN_AROUND)) {
        viewAt.x = camera->at.x + camShake.atOffset.x;
        viewAt.y = camera->at.y + camShake.atOffset.y;
        viewAt.z = camera->at.z + camShake.atOffset.z;

        viewEye.x = camera->eye.x + camShake.eyeOffset.x;
        viewEye.y = camera->eye.y + camShake.eyeOffset.y;
        viewEye.z = camera->eye.z + camShake.eyeOffset.z;

        eyeAtAngle = sglobe_by_2pos(&viewEye, &viewAt);
        viewUp = calcup(eyeAtAngle.pitch + camShake.upPitchOffset,
                                               eyeAtAngle.yaw + camShake.upYawOffset, camera->roll);
        viewFov = camera->fov + CAM_BINANG_TO_DEG(camShake.fovOffset);
    } else {
        viewAt = camera->at;
        viewEye = camera->eye;
        eyeAtAngle = sglobe_by_2pos(&viewEye, &viewAt);
        viewUp = calcup(eyeAtAngle.pitch, eyeAtAngle.yaw, camera->roll);
        viewFov = camera->fov;
    }

    if (camera->viewFlags & CAM_VIEW_UP) {
        camera->viewFlags &= ~CAM_VIEW_UP;
        viewUp = camera->up;
    } else {
        camera->up = viewUp;
    }

    camera->quakeOffset = camShake.eyeOffset;

    stretch_view(camera);

    if ((camera->play->sceneId == SCENE_HYRULE_FIELD) && (camera->fov < 59.0f)) {
        setScaleView(&camera->play->view, 0.79f);
    } else {
        setScaleView(&camera->play->view, 1.0f);
    }
    camera->play->view.fovy = viewFov;
    setLookAtView(&camera->play->view, &viewEye, &viewAt, &viewUp);
    camera->camDir.x = eyeAtAngle.pitch;
    camera->camDir.y = eyeAtAngle.yaw;
    camera->camDir.z = 0;

    if (cutflag == 0) {
        camera->inputDir.x = eyeAtAngle.pitch;
        camera->inputDir.y = eyeAtAngle.yaw;
        camera->inputDir.z = 0;
    }

    if (PREG(81)) {
        PRINTF("dir  (%d) %d(%f) %d(%f) 0(0) \n", cutflag, camera->inputDir.x,
               CAM_BINANG_TO_DEG(camera->inputDir.x), camera->inputDir.y, CAM_BINANG_TO_DEG(camera->inputDir.y));
        PRINTF("real (%d) %d(%f) %d(%f) 0(0) \n", cutflag, camera->camDir.x,
               CAM_BINANG_TO_DEG(camera->camDir.x), camera->camDir.y, CAM_BINANG_TO_DEG(camera->camDir.y));
    }

#if DEBUG_FEATURES
    if (camera->timer != -1 && CAMERA_CHECK_BTN(&__game->state.input[0], BTN_DRIGHT)) {
        camera->timer = 0;
    }
#endif

    if (ENABLE_DEBUG_CAM_UPDATE) {
        PRINTF("camera: out (%f %f %f) (%f %f %f)\n", camera->at.x, camera->at.y, camera->at.z, camera->eye.x,
               camera->eye.y, camera->eye.z);
        PRINTF("camera: dir (%f %d(%f) %d(%f)) (%f)\n", eyeAtAngle.r, eyeAtAngle.pitch,
               CAM_BINANG_TO_DEG(eyeAtAngle.pitch), eyeAtAngle.yaw, CAM_BINANG_TO_DEG(eyeAtAngle.yaw), camera->fov);
        if (camera->player != NULL) {
            PRINTF("camera: foot(%f %f %f) dist (%f)\n", curPlayerPosRot.pos.x, curPlayerPosRot.pos.y,
                   curPlayerPosRot.pos.z, camera->dist);
        }
    }

    return camera->inputDir;
}

/**
 * When the camera's timer is 0, change the camera to its parent
 */
void batontouchCamera(Camera* camera) {
    Camera* mainCam = camera->play->cameraPtrs[CAM_ID_MAIN];
    Player* player = GET_PLAYER(camera->play);

    if (camera->timer == 0) {
        Gama_play_set_camera_status(camera->play, camera->parentCamId, CAM_STAT_ACTIVE);

        if ((camera->parentCamId == CAM_ID_MAIN) && (camera->csId != 0)) {
            player->actor.freezeTimer = 0;
            player->stateFlags1 &= ~PLAYER_STATE1_29;

            if (player->csAction != PLAYER_CSACTION_NONE) {
                player_demo_mode_set(camera->play, &player->actor, PLAYER_CSACTION_7);
                PRINTF("camera: player demo end!!\n");
            }

            mainCam->stateFlags |= CAM_STATE_EXTERNAL_FINISHED;
        }

        if (CHILD_CAM(camera)->parentCamId == camera->camId) {
            CHILD_CAM(camera)->parentCamId = camera->parentCamId;
        }

        if (PARENT_CAM(camera)->childCamId == camera->camId) {
            PARENT_CAM(camera)->childCamId = camera->childCamId;
        }

        if (PARENT_CAM(camera)->camId == CAM_ID_MAIN) {
            PARENT_CAM(camera)->animState = 0;
        }

        camera->childCamId = camera->parentCamId = CAM_ID_MAIN;
        camera->timer = -1;
        camera->play->envCtx.fillScreen = false;

        Gama_play_clear_camera(camera->play, camera->camId);
    }
}

s32 clear_idflg(Camera* camera) {
    camera->stateFlags |= (CAM_STATE_CHECK_BG | CAM_STATE_EXTERNAL_FINISHED);
    camera->stateFlags &= ~(CAM_STATE_EXTERNAL_FINISHED | CAM_STATE_DEMO7);
    return true;
}

#define CAM_REQUEST_MODE_FORCE_NO_SFX (1 << 0)
#define CAM_REQUEST_MODE_SFX_ATTENTION (1 << 1)
#define CAM_REQUEST_MODE_Z_TARGET_UNFRIENDLY (1 << 2)
#define CAM_REQUEST_MODE_Z_TARGET_FRIENDLY (1 << 3)
#define CAM_REQUEST_MODE_SKIP_ANIM_NORMAL_PARALLEL (1 << 4) // never set to
#define CAM_REQUEST_MODE_SKIP_ANIM_FIRST_PERSON (1 << 5)

s32 change_camera_mode(Camera* camera, s16 requestedMode, u8 forceModeChange) {
    static s32 flg = 0;

    if (QREG(89)) {
        PRINTF("+=+(%d)+=+ recive request -> %s\n", camera->play->state.frames, sCameraModeNames[requestedMode]);
    }

    if ((camera->stateFlags & CAM_STATE_LOCK_MODE) && !forceModeChange) {
        camera->behaviorFlags |= CAM_BEHAVIOR_MODE_VALID;
        return -1;
    }

    if (!((cam_ctl_tbl_0[camera->setting].unk_00 & 0x3FFFFFFF) & (1 << requestedMode))) {
        if (requestedMode == CAM_MODE_FIRST_PERSON) {
            PRINTF("camera: error sound\n");
            Na_StartSystemSe_F(NA_SE_SY_ERROR);
        }

        if (camera->mode != CAM_MODE_NORMAL) {
            PRINTF(VT_COL(YELLOW, BLACK) "camera: change camera mode: force NORMAL: %s %s refused\n" VT_RST,
                   sCameraSettingNames[camera->setting], sCameraModeNames[requestedMode]);
            camera->mode = CAM_MODE_NORMAL;
            reset_local_camera(camera, camera->mode);
            clear_idflg(camera);
            return 0xC0000000 | requestedMode;
        }

        camera->behaviorFlags |= CAM_BEHAVIOR_MODE_VALID;
        camera->behaviorFlags |= CAM_BEHAVIOR_MODE_SUCCESS;
        return CAM_MODE_NORMAL;
    }

    if ((requestedMode == camera->mode) && !forceModeChange) {
        camera->behaviorFlags |= CAM_BEHAVIOR_MODE_VALID;
        camera->behaviorFlags |= CAM_BEHAVIOR_MODE_SUCCESS;
        return -1;
    }

    camera->behaviorFlags |= CAM_BEHAVIOR_MODE_VALID;
    camera->behaviorFlags |= CAM_BEHAVIOR_MODE_SUCCESS;

    reset_local_camera(camera, requestedMode);

    flg = 0;

    // requested camMode
    switch (requestedMode) {
        case CAM_MODE_FIRST_PERSON:
            flg = CAM_REQUEST_MODE_SKIP_ANIM_FIRST_PERSON;
            break;

        case CAM_MODE_Z_TARGET_UNFRIENDLY:
            flg = CAM_REQUEST_MODE_Z_TARGET_UNFRIENDLY;
            break;

        case CAM_MODE_Z_TARGET_FRIENDLY:
            if ((camera->target != NULL) && (camera->target->id != ACTOR_EN_BOOM)) {
                flg = CAM_REQUEST_MODE_Z_TARGET_FRIENDLY;
            }
            break;

        case CAM_MODE_Z_PARALLEL:
        case CAM_MODE_TALK:
        case CAM_MODE_Z_AIM:
        case CAM_MODE_Z_LEDGE_HANG:
        case CAM_MODE_PUSH_PULL:
            flg = CAM_REQUEST_MODE_SFX_ATTENTION;
            break;

        default:
            break;
    }

    // If the requested mode is already the same as the current mode,
    // then many modes will reset their animState to 10.
    switch (camera->mode) {
        case CAM_MODE_FIRST_PERSON:
            if (flg & CAM_REQUEST_MODE_SKIP_ANIM_FIRST_PERSON) {
                camera->animState = 10;
            }
            break;

        case CAM_MODE_Z_PARALLEL:
            if (flg & CAM_REQUEST_MODE_SKIP_ANIM_NORMAL_PARALLEL) {
                camera->animState = 10;
            }
            flg |= CAM_REQUEST_MODE_FORCE_NO_SFX;
            break;

        case CAM_MODE_CHARGE:
            flg |= CAM_REQUEST_MODE_FORCE_NO_SFX;
            break;

        case CAM_MODE_Z_TARGET_FRIENDLY:
            if (flg & CAM_REQUEST_MODE_Z_TARGET_FRIENDLY) {
                camera->animState = 10;
            }
            flg |= CAM_REQUEST_MODE_FORCE_NO_SFX;
            break;

        case CAM_MODE_Z_TARGET_UNFRIENDLY:
            if (flg & CAM_REQUEST_MODE_Z_TARGET_UNFRIENDLY) {
                camera->animState = 10;
            }
            flg |= CAM_REQUEST_MODE_FORCE_NO_SFX;
            break;

        case CAM_MODE_Z_AIM:
        case CAM_MODE_Z_LEDGE_HANG:
        case CAM_MODE_PUSH_PULL:
            flg |= CAM_REQUEST_MODE_FORCE_NO_SFX;
            break;

        case CAM_MODE_NORMAL:
            if (flg & CAM_REQUEST_MODE_SKIP_ANIM_NORMAL_PARALLEL) {
                camera->animState = 10;
            }
            break;

        default:
            break;
    }

    flg &= ~CAM_REQUEST_MODE_SKIP_ANIM_NORMAL_PARALLEL;

    // Choose a sound effect to play.
    // Having `CAM_REQUEST_MODE_FORCE_NO_SFX` set often means `default` is taken from two bits being set
    if (camera->status == CAM_STAT_ACTIVE) {
        switch (flg) {
            case CAM_REQUEST_MODE_FORCE_NO_SFX:
                Na_StartSystemSe_F(NA_SE_NONE);
                break;

            case CAM_REQUEST_MODE_SFX_ATTENTION:
                if (camera->play->roomCtx.curRoom.type == ROOM_TYPE_DUNGEON) {
                    Na_StartSystemSe_F(NA_SE_SY_ATTENTION_URGENCY);
                } else {
                    Na_StartSystemSe_F(NA_SE_SY_ATTENTION_ON);
                }
                break;

            case CAM_REQUEST_MODE_Z_TARGET_UNFRIENDLY:
                Na_StartSystemSe_F(NA_SE_SY_ATTENTION_URGENCY);
                break;

            case CAM_REQUEST_MODE_Z_TARGET_FRIENDLY:
                Na_StartSystemSe_F(NA_SE_SY_ATTENTION_ON);
                break;

            default:
                break;
        }
    }

    clear_idflg(camera);
    camera->mode = requestedMode;

    return requestedMode | 0x80000000;
}

s32 changeCameraMode(Camera* camera, s16 mode) {
    return change_camera_mode(camera, mode, false);
}

s32 useCameraModeOK(Camera* camera, s16 mode) {
    if (QREG(89) != 0) {
        PRINTF("+=+=+=+ recive asking -> %s (%s)\n", sCameraModeNames[mode], sCameraSettingNames[camera->setting]);
    }
    if (!(cam_ctl_tbl_0[camera->setting].validModes & (1 << mode))) {
        return 0;
    } else if (mode == camera->mode) {
        return -1;
    } else {
        return mode | 0x80000000;
    }
}

s16 change_camera_set(Camera* camera, s16 requestedSetting, s16 flags) {
    if (camera->behaviorFlags & CAM_BEHAVIOR_SETTING_CHECK_PRIORITY) {
        // If a second setting is requested this frame, determine if the setting overwrites the
        // current setting through priority
        if (((cam_ctl_tbl_0[camera->setting].unk_00 & 0xF000000) >> 0x18) >=
            ((cam_ctl_tbl_0[requestedSetting].unk_00 & 0xF000000) >> 0x18)) {
            camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_VALID;
            return -2;
        }
    }

    if (((requestedSetting == CAM_SET_MEADOW_BIRDS_EYE) || (requestedSetting == CAM_SET_MEADOW_UNUSED)) &&
        LINK_IS_ADULT && (camera->play->sceneId == SCENE_SACRED_FOREST_MEADOW)) {
        camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_VALID;
        return -5;
    }

    if ((requestedSetting == CAM_SET_NONE) || (requestedSetting >= CAM_SET_MAX)) {
        PRINTF(VT_COL(RED, WHITE) "camera: error: illegal camera set (%d) !!!!\n" VT_RST, requestedSetting);
        return -99;
    }

    if ((requestedSetting == camera->setting) && !(flags & CAM_REQUEST_SETTING_FORCE_CHANGE)) {
        camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_VALID;
        if (!(flags & CAM_REQUEST_SETTING_IGNORE_PRIORITY)) {
            camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_CHECK_PRIORITY;
        }
        return -1;
    }

    camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_VALID;

    if (!(flags & CAM_REQUEST_SETTING_IGNORE_PRIORITY)) {
        camera->behaviorFlags |= CAM_BEHAVIOR_SETTING_CHECK_PRIORITY;
    }

    camera->stateFlags |= (CAM_STATE_CHECK_BG | CAM_STATE_EXTERNAL_FINISHED);
    camera->stateFlags &= ~(CAM_STATE_EXTERNAL_FINISHED | CAM_STATE_DEMO7);

    if (!(cam_ctl_tbl_0[camera->setting].unk_00 & 0x40000000)) {
        camera->prevSetting = camera->setting;
    }

    if (flags & CAM_REQUEST_SETTING_RESTORE_PREV_BG_CAM_INDEX) {
        if (1) {}
        camera->bgCamIndex = camera->prevBgCamIndex;
        camera->prevBgCamIndex = -1;
    } else if (!(flags & CAM_REQUEST_SETTING_PRESERVE_BG_CAM_INDEX)) {
        if (!(cam_ctl_tbl_0[camera->setting].unk_00 & 0x40000000)) {
            camera->prevBgCamIndex = camera->bgCamIndex;
        }
        camera->bgCamIndex = -1;
    }

    camera->setting = requestedSetting;

    if (change_camera_mode(camera, camera->mode, true) >= 0) {
        reset_local_camera(camera, camera->mode);
    }

    PRINTF(VT_SGR("1") "%06u:" VT_RST " camera: change camera[%d] set %s\n", camera->play->state.frames, camera->camId,
           sCameraSettingNames[camera->setting]);

    return requestedSetting;
}

s32 changeCameraSet(Camera* camera, s16 setting) {
    return change_camera_set(camera, setting, 0);
}

s32 changeCameraID(Camera* camera, s32 requestedBgCamIndex) {
    s16 requestedCamSetting;
    s16 settingChangeSuccessful;

    if ((requestedBgCamIndex == -1) || (requestedBgCamIndex == camera->bgCamIndex)) {
        camera->behaviorFlags |= CAM_BEHAVIOR_BG_PROCESSED;
        return -1;
    }

    if (!(camera->behaviorFlags & CAM_BEHAVIOR_BG_PROCESSED)) {
        requestedCamSetting = get_camera_set_by_cameraID(camera, requestedBgCamIndex);
        camera->behaviorFlags |= CAM_BEHAVIOR_BG_PROCESSED;
#if DEBUG_FEATURES
        settingChangeSuccessful = change_camera_set(camera, requestedCamSetting,
                                                            CAM_REQUEST_SETTING_PRESERVE_BG_CAM_INDEX |
                                                                CAM_REQUEST_SETTING_FORCE_CHANGE) >= 0;
        if ((settingChangeSuccessful != CAM_SET_NONE) || (cam_ctl_tbl_0[camera->setting].unk_00 & 0x80000000)) {
            camera->bgCamIndex = requestedBgCamIndex;
            camera->behaviorFlags |= CAM_BEHAVIOR_BG_SUCCESS;
            reset_local_camera(camera, camera->mode);
        } else if (settingChangeSuccessful < -1) {
            //! @bug: `settingChangeSuccessful` is a bool and is likely checking the wrong value. This can never pass.
            // The actual return of change_camera_set or bgCamIndex would make more sense.
            PRINTF(VT_COL(RED, WHITE) "camera: error: illegal camera ID (%d) !! (%d|%d|%d)\n" VT_RST,
                   requestedBgCamIndex, camera->camId, BGCHECK_SCENE, requestedCamSetting);
        }
#else
        if ((change_camera_set(camera, requestedCamSetting,
                                       CAM_REQUEST_SETTING_PRESERVE_BG_CAM_INDEX | CAM_REQUEST_SETTING_FORCE_CHANGE) >=
             0) ||
            (cam_ctl_tbl_0[camera->setting].unk_00 & 0x80000000)) {
            camera->bgCamIndex = requestedBgCamIndex;
            camera->behaviorFlags |= CAM_BEHAVIOR_BG_SUCCESS;
            reset_local_camera(camera, camera->mode);
        }
#endif
        return 0x80000000 | requestedBgCamIndex;
    }
    //! @bug Missing return, but the return value is not used.
}

Vec3s getCameraAngle(Camera* camera) {
#if DEBUG_FEATURES
    if (debug_camera_sw) {
        return D_8015BD80.sub.unk_104A;
    }
#endif

    return camera->inputDir;
}

s16 getCameraAngleX(Camera* camera) {
    Vec3s dir = getCameraAngle(camera);

    return dir.x;
}

s16 getCameraAngleY(Camera* camera) {
    Vec3s dir = getCameraAngle(camera);

    return dir.y;
}

Vec3s getRealCameraAngle(Camera* camera) {
#if DEBUG_FEATURES
    if (debug_camera_sw) {
        return D_8015BD80.sub.unk_104A;
    }
#endif

    return camera->camDir;
}

s16 getRealCameraAngleX(Camera* camera) {
    Vec3s camDir = getRealCameraAngle(camera);

    return camDir.x;
}

s16 getRealCameraAngleY(Camera* camera) {
    Vec3s camDir = getRealCameraAngle(camera);

    return camDir.y;
}

s32 setDamageCamera(Camera* camera, s32 unused, s16 y, s32 duration) {
    s16 quakeIndex;

    quakeIndex = startQuake(camera, QUAKE_TYPE_3);
    if (quakeIndex == 0) {
        return false;
    }
    setSpeedQuake(quakeIndex, 0x61A8);
    setScaleQuake(quakeIndex, y, 0, 0, 0);
    setTimerQuake(quakeIndex, duration);
    return true;
}

s32 lockCamera(Camera* camera, s32 viewFlag, void* param) {
    s32 pad[3];

    if (param != NULL) {
        switch (viewFlag) {
            case CAM_VIEW_AT:
                camera->viewFlags &= ~(CAM_VIEW_AT | CAM_VIEW_TARGET | CAM_VIEW_TARGET_POS);
                camera->at = *(Vec3f*)param;
                break;

            case CAM_VIEW_TARGET_POS:
                camera->viewFlags &= ~(CAM_VIEW_AT | CAM_VIEW_TARGET | CAM_VIEW_TARGET_POS);
                camera->targetPosRot.pos = *(Vec3f*)param;
                break;

            case CAM_VIEW_TARGET:
                if (camera->setting != CAM_SET_CS_C && camera->setting != CAM_SET_CS_ATTENTION) {
                    camera->target = (Actor*)param;
                    camera->viewFlags &= ~(CAM_VIEW_AT | CAM_VIEW_TARGET | CAM_VIEW_TARGET_POS);
                }
                break;

            case CAM_VIEW_EYE:
                camera->eye = camera->eyeNext = *(Vec3f*)param;
                break;

            case CAM_VIEW_UP:
                camera->up = *(Vec3f*)param;
                break;

            case CAM_VIEW_ROLL:
                camera->roll = CAM_DEG_TO_BINANG(*(f32*)param);
                break;

            case CAM_VIEW_FOV:
                camera->fov = *(f32*)param;
                break;

            default:
                return false;
        }
        camera->viewFlags |= viewFlag;
    } else {
        return false;
    }
    return true;
}

s32 unlockCamera(Camera* camera, s16 viewFlag) {
    camera->viewFlags &= ~viewFlag;
    return true;
}

s32 changeCameraByPolygon(Camera* camera, s16 stateFlags) {
    camera->stateFlags = stateFlags;
    return true;
}

s32 setCameraResetSpline(Camera* camera) {
    camera->animState = 0;
    return 1;
}

s32 setCameraDemoSplineInfo(Camera* camera, CutsceneCameraPoint* atPoints, CutsceneCameraPoint* eyePoints, Player* player,
                       s16 relativeToPlayer) {
    PosRot playerPosRot;

    camera->data0 = atPoints;
    camera->data1 = eyePoints;
    camera->data2 = relativeToPlayer;

    if (camera->data2 != 0) {
        camera->player = player;
        playerPosRot = Actor_get_shape(&player->actor);
        camera->playerPosRot = playerPosRot;

        camera->nextBgCamIndex = -1;
        camera->xzSpeed = 0.0f;
        camera->speedRatio = 0.0f;
    }

    return 1;
}

s16 setCameraFlag(Camera* camera, s16 stateFlag) {
    camera->stateFlags |= stateFlag;
    return camera->stateFlags;
}

s16 clearCameraFlag(Camera* camera, s16 stateFlag) {
    camera->stateFlags &= ~stateFlag;
    return camera->stateFlags;
}

/**
 * A bgCamIndex of -99 will save the door params without changing the camera setting
 * A bgCamIndex of -1 uses the default door camera setting (CAM_SET_DOORC)
 * Otherwise, change the door camera setting by reading the bgCam indexed at bgCamIndex
 */
s32 setDoorCameraInfo(Camera* camera, Actor* doorActor, s16 bgCamIndex, f32 arg3, s16 timer1, s16 timer2,
                         s16 timer3) {
    DoorParams* doorParams = &camera->paramData.doorParams;

    if ((camera->setting == CAM_SET_CS_ATTENTION) || (camera->setting == CAM_SET_DOORC)) {
        return 0;
    }

    doorParams->doorActor = doorActor;
    doorParams->timer1 = timer1;
    doorParams->timer2 = timer2;
    doorParams->timer3 = timer3;
    doorParams->bgCamIndex = bgCamIndex;

    if (bgCamIndex == -99) {
        reset_local_camera(camera, camera->mode);
        return -99;
    }

    if (bgCamIndex == -1) {
        changeCameraSet(camera, CAM_SET_DOORC);
        PRINTF(".... change default door camera (set %d)\n", CAM_SET_DOORC);
    } else {
        s32 setting = get_camera_set_by_cameraID(camera, bgCamIndex);

        camera->behaviorFlags |= CAM_BEHAVIOR_BG_PROCESSED;

        if (changeCameraSet(camera, setting) >= 0) {
            camera->bgCamIndex = bgCamIndex;
            camera->behaviorFlags |= CAM_BEHAVIOR_BG_SUCCESS;
        }

        PRINTF("....change door camera ID %d (set %d)\n", camera->bgCamIndex, camera->setting);
    }

    reset_local_camera(camera, camera->mode);
    return -1;
}

s32 copyCameraPos(Camera* dstCamera, Camera* srcCamera) {
    dstCamera->playerToAtOffset.x = dstCamera->playerToAtOffset.y = dstCamera->playerToAtOffset.z = 0.0f;
    dstCamera->atLERPStepScale = 0.1f;
    dstCamera->at = srcCamera->at;

    dstCamera->eye = dstCamera->eyeNext = srcCamera->eye;

    dstCamera->dist = distance_between(&dstCamera->at, &dstCamera->eye);
    dstCamera->fov = srcCamera->fov;
    dstCamera->roll = srcCamera->roll;
    slowly_cushon_set(dstCamera);

    if (dstCamera->player != NULL) {
        dstCamera->playerPosRot = Actor_get_world(&dstCamera->player->actor);
        dstCamera->playerToAtOffset.x = dstCamera->at.x - dstCamera->playerPosRot.pos.x;
        dstCamera->playerToAtOffset.y = dstCamera->at.y - dstCamera->playerPosRot.pos.y;
        dstCamera->playerToAtOffset.z = dstCamera->at.z - dstCamera->playerPosRot.pos.z;
        dstCamera->dist = distance_between(&dstCamera->playerPosRot.pos, &dstCamera->eye);
        dstCamera->xzOffsetUpdateRate = 1.0f;
        dstCamera->yOffsetUpdateRate = 1.0f;
    }
    return true;
}

s32 onDebugCamera(void) {
#if DEBUG_FEATURES
    return debug_camera_sw;
#else
    return false;
#endif
}

Vec3f getCameraGap(Camera* camera) {
    return camera->quakeOffset;
}

void setCameraData(Camera* camera, s16 setDataFlags, void* data0, void* data1, s16 data2, s16 data3,
                          UNK_TYPE arg6) {
    if (setDataFlags & 0x1) {
        camera->data0 = data0;
    }

    if (setDataFlags & 0x2) {
        camera->data1 = data1;
    }

    if (setDataFlags & 0x4) {
        camera->data2 = data2;
    }

    if (setDataFlags & 0x8) {
        camera->data3 = data3;
    }

    if (setDataFlags & 0x10) {
        PRINTF(VT_COL(RED, WHITE) "camera: setCameraData: last argument not alive!\n" VT_RST);
    }
}

#if DEBUG_FEATURES
s32 Camera_QRegInit(void) {
    if (!R_RELOAD_CAM_PARAMS) {
        QREG(2) = 1;
        QREG(10) = -1;
        QREG(11) = 100;
        QREG(12) = 80;
        QREG(20) = 90;
        QREG(21) = 10;
        QREG(22) = 10;
        QREG(23) = 50;
        QREG(24) = 6000;
        QREG(25) = 240;
        QREG(26) = 40;
        QREG(27) = 85;
        QREG(28) = 55;
        QREG(29) = 87;
        QREG(30) = 23;
        QREG(31) = 20;
        QREG(32) = 4;
        QREG(33) = 5;
        QREG(50) = 1;
        QREG(51) = 20;
        QREG(52) = 200;
        QREG(53) = 1;
        QREG(54) = 15;
        QREG(55) = 60;
        QREG(56) = 15;
        QREG(57) = 30;
        QREG(58) = 0;
    }

    QREG(65) = 50;
    return true;
}
#endif

s32 getAttentionDemoPart(void) {
    return attention_part;
}

/**
 * Signal to the camera update function through stateFlags that something external has
 * finished and is ready for the next camera setting/function
 * Different camera update functions will respond differently to this flag being set.
 */
s16 restartCameraStoped(Camera* camera) {
    camera->stateFlags |= CAM_STATE_EXTERNAL_FINISHED;

    if ((camera->camId == CAM_ID_MAIN) && (camera->play->activeCamId != CAM_ID_MAIN)) {
        GET_ACTIVE_CAM(camera->play)->stateFlags |= CAM_STATE_EXTERNAL_FINISHED;
        return camera->play->activeCamId;
    }

    return camera->camId;
}
