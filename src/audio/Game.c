#include "ultra64.h"
#include "global.h"
#include "versions.h"

#define ABS_ALT(x) ((x) < 0 ? -(x) : (x))

#if !PLATFORM_N64
#define AUDIO_PRINTF osSyncPrintf
#elif IDO_PRINTF_WORKAROUND
#define AUDIO_PRINTF(args) (void)0
#else
#define AUDIO_PRINTF(format, ...) (void)0
#endif

typedef struct SfxPlayerState {
    /* 0x0 */ f32 vol;
    /* 0x4 */ f32 freqScale;
    /* 0x8 */ s8 reverb;
    /* 0x9 */ s8 pan;
    /* 0xA */ s8 stereoBits;
    /* 0xB */ u8 filter;
    /* 0xC */ u8 combFilterGain;
} SfxPlayerState;

typedef enum SfxChannelIndex {
    /* 0x0 */ SFX_CHANNEL_PLAYER0, // SfxPlayerBank
    /* 0x1 */ SFX_CHANNEL_PLAYER1,
    /* 0x2 */ SFX_CHANNEL_PLAYER2,
    /* 0x3 */ SFX_CHANNEL_ITEM0, // SfxItemBank
    /* 0x4 */ SFX_CHANNEL_ITEM1,
    /* 0x5 */ SFX_CHANNEL_ENV0, // SfxEnvironmentBank
    /* 0x6 */ SFX_CHANNEL_ENV1,
    /* 0x7 */ SFX_CHANNEL_ENV2,
    /* 0x8 */ SFX_CHANNEL_ENEMY0, // SfxEnemyBank
    /* 0x9 */ SFX_CHANNEL_ENEMY1,
    /* 0xA */ SFX_CHANNEL_ENEMY2,
    /* 0xB */ SFX_CHANNEL_SYSTEM0, // SfxSystemBank
    /* 0xC */ SFX_CHANNEL_SYSTEM1,
    /* 0xD */ SFX_CHANNEL_OCARINA, // SfxOcarinaBank
    /* 0xE */ SFX_CHANNEL_VOICE0,  // SfxVoiceBank
    /* 0xF */ SFX_CHANNEL_VOICE1
} SfxChannelIndex; // playerIdx = 2

typedef struct FreqLerp {
    /* 0x0 */ f32 value;
    /* 0x4 */ f32 target;
    /* 0x8 */ f32 step;
    /* 0xC */ s32 remainingFrames;
} FreqLerp;

typedef struct NatureAmbienceDataIO {
    /* 0x0 */ u16 playerIO;
    /* 0x2 */ u16 channelMask;
    /* 0x4 */ u8 channelIO[3 * 33 + 1];
} NatureAmbienceDataIO; // size = 0x68

typedef enum AudioDebugPage {
    /* 0x0 */ PAGE_NON,
    /* 0x1 */ PAGE_SOUND_CONTROL,
    /* 0x2 */ PAGE_SPEC_INFO, // unused
    /* 0x3 */ PAGE_HEAP_INFO,
    /* 0x4 */ PAGE_GROUP_TRACK_INFO, // unused
    /* 0x5 */ PAGE_SUB_TRACK_INFO,
    /* 0x6 */ PAGE_CHANNEL_INFO, // unused
    /* 0x7 */ PAGE_INTERFACE_INFO,
    /* 0x8 */ PAGE_SFX_SWAP,
    /* 0x9 */ PAGE_BLOCK_CHANGE_BGM,
    /* 0xA */ PAGE_NATURAL_SOUND_CONTROL, // unused
    /* 0xB */ PAGE_OCARINA_TEST,
    /* 0xC */ PAGE_SFX_PARAMETER_CHANGE,
    /* 0xD */ PAGE_SCROLL_PRINT,
    /* 0xE */ PAGE_FREE_AREA,
    /* 0xF */ PAGE_MAX
} AudioDebugPage;

#define SCROLL_PRINT_BUF_SIZE 25

typedef struct OcarinaStick {
    s8 x;
    s8 y;
} OcarinaStick;

#define DEFINE_SFX(_0, _1, _2, _3, _4, _5) 1 +
u8 _se_handle_extend_senum[7] = {
    (
#include "tables/sfx/playerbank_table.h"
        0) > UINT8_MAX,
    (
#include "tables/sfx/itembank_table.h"
        0) > UINT8_MAX,
    (
#include "tables/sfx/environmentbank_table.h"
        0) > UINT8_MAX,
    (
#include "tables/sfx/enemybank_table.h"
        0) > UINT8_MAX,
    (
#include "tables/sfx/systembank_table.h"
        0) > UINT8_MAX,
    (
#include "tables/sfx/ocarinabank_table.h"
        0) > UINT8_MAX,
    (
#include "tables/sfx/voicebank_table.h"
        0) > UINT8_MAX,
};
#undef DEFINE_SFX

// Only the first row of these is supported by sequence 0. (se_scene_mode is always 0.)
u8 _se_handle_sounds[4][7] = {
    { 3, 2, 3, 3, 2, 1, 2 },
    { 3, 2, 2, 2, 2, 2, 2 },
    { 3, 2, 2, 2, 2, 2, 2 },
    { 4, 1, 0, 0, 2, 2, 2 },
};
u8 _se_handle_objsounds[4][7] = {
    { 3, 2, 3, 2, 2, 1, 1 },
    { 3, 1, 1, 1, 2, 1, 1 },
    { 3, 1, 1, 1, 2, 1, 1 },
    { 2, 1, 0, 0, 1, 1, 1 },
};

f32 na_black_link_pitch = 0.7950898f;
s8 na_black_link_fxmix = 35;
s8 na_autotrig_intv_now = 20;
s8 na_autotrig_intv_min = 30;
s8 na_autotrig_intv_normal = 20;
f32 na_behind_posz[2] = { -15.0f, -65.0f };
u8 na_picth_up_se_count = 0;
u8 na_mofer_picth_up_table[16] = { 0, 0, 0, 1, 1, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8 };
u8 na_sword_charge_level = 0;
f32 na_sword_charge_pitch_table[4] = { 1.0f, 1.12246f, 1.33484f, 1.33484f }; // 2**({0, 2, 5, 5}/12)
f32 na_sword_charge_pitch_now = 1.0f;
u8 na_trigger_mute_vol_tbl[8] = { 127, 80, 75, 73, 70, 68, 65, 60 };
u8 na_dist_bgm_set = 0;
#if DEBUG_FEATURES
s8 na_sound_output = SOUNDMODE_SURROUND;
#else
s8 na_sound_output = SOUNDMODE_STEREO;
#endif
s8 na_pause_flag = 0;
s8 na_mes_flag = 0;
s8 default_fx = 0;
s8 enviroment_fx = 0;
s8 code_fx_ofs = 0;
u8 na_seq_switch_flag = 0;
f32 na_bgm_enm_dist = 0.0f;
s8 na_bgm_enm_vol = 127;
u16 na_now_bgm_flag = NA_BGM_DISABLED;

#define SEQ_RESUME_POINT_NONE 0xC0
u8 na_bgm_continue_block = 0;
u8 na_bgm_continue_old_num = NA_BGM_GENERAL_SFX;

u32 na_seq_switch_int_time = 0;
u32 na_seq_switch_normal_time = 0;
u8 na_forth_lowpass_mode_0 = 0;
u8 na_forth_lowpass_mode_1 = 0;
u8 na_forth_vibrate_mode = 0;
u8 na_forth_vibrate_mode2 = 0;
Vec3f* saria_oca_pos = NULL;
f32 saria_oca_lpos = 2000.0f;

#if DEBUG_FEATURES
u8 sSeqModeInput = 0;
#endif

#define SEQ_FLAG_ENEMY (1 << 0) // Allows enemy bgm
#define SEQ_FLAG_FANFARE (1 << 1)
#define SEQ_FLAG_FANFARE_GANON (1 << 2)
#define SEQ_FLAG_RESTORE (1 << 3) // required for Na_StopMiddleBossBgm to restore a sequence after Na_StartMiddleBossBgm

/**
 * These two sequence flags work together to implement a “resume playing from where you left off” system for scene
 * sequences when leaving and returning to a scene. For a scene to resume playing from the point where it left off, it
 * must have `SEQ_FLAG_RESUME` attached to it. Then, if the scene changes and the new scene sequence contain
 * `SEQ_FLAG_RESUME_PREV`, the point from the previous scene sequence will be stored. Then, when returning to the
 * scene with the sequence `SEQ_FLAG_RESUME`, then the sequence will resume playing from where it left off.
 *
 * There are only 5 sequences with `SEQ_FLAG_RESUME`, and all 5 of those sequences have special sequence
 * instructions in their .seq files to read io port 7 and branch to different starting points along the sequence
 * i.e. this system will only work for: kokiri forest, kakariko child, kakariko adult, zoras domain, gerudo valley
 */
#define SEQ_FLAG_RESUME (1 << 4)
#define SEQ_FLAG_RESUME_PREV (1 << 5)

/**
 * Will write a value of 1 to ioPort 7 when called through the scene. How it's used depends on the sequence:
 * NA_BGM_CHAMBER_OF_SAGES - ioPort 7 is never read from
 * NA_BGM_FILE_SELECT - ioPort 7 skips the harp intro when a value of 1 is written to it.
 * Note: NA_BGM_FILE_SELECT is not called through the scene. So this flag serves no purpose
 */
#define SEQ_FLAG_SKIP_HARP_INTRO (1 << 6)
#define SEQ_FLAG_NO_AMBIENCE (1 << 7)

#define DEFINE_SEQUENCE(name, seqId, storageMedium, cachePolicy, seqFlags) seqFlags,
#define DEFINE_SEQUENCE_PTR(seqIdReal, seqId, storageMediumReal, cachePolicyReal, seqFlags) seqFlags,
u8 na_bgm_info[] = {
#include "tables/sequence_table.h"
};
#undef DEFINE_SEQUENCE
#undef DEFINE_SEQUENCE_PTR

s8 na_spec_echo_ofs[20] = { 0, 0, 0, 0, 0, 0, 0, 40, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

NatureAmbienceDataIO na_nsc_init_flag[20] = {
    // NATURE_ID_GENERAL_NIGHT
    {
        0xC0FF, // PlayerIO Data
        0xC0FE, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CROWS_CAWS),
            NATURE_IO_CRITTER_0_BEND_PITCH(64),
            NATURE_IO_CRITTER_0_NUM_LAYERS(0),
            NATURE_IO_CRITTER_0_PORT5(32),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_1_BEND_PITCH(0),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(16),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_SMALL_BIRD_CHIRPS),
            NATURE_IO_CRITTER_2_BEND_PITCH(112),
            NATURE_IO_CRITTER_2_NUM_LAYERS(1),
            NATURE_IO_CRITTER_2_PORT5(48),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_HAWK_SCREECH),
            NATURE_IO_CRITTER_3_BEND_PITCH(127),
            NATURE_IO_CRITTER_3_NUM_LAYERS(0),
            NATURE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            NATURE_IO_CRITTER_4_TYPE(NATURE_CRITTER_BIRD_CHIRP_1),
            NATURE_IO_CRITTER_4_BEND_PITCH(127),
            NATURE_IO_CRITTER_4_NUM_LAYERS(1),
            NATURE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            NATURE_IO_CRITTER_5_TYPE(NATURE_CRITTER_TAP),
            NATURE_IO_CRITTER_5_BEND_PITCH(127),
            NATURE_IO_CRITTER_5_NUM_LAYERS(3),
            NATURE_IO_CRITTER_5_PORT5(16),

            // Channel 7
            NATURE_IO_CRITTER_6_TYPE(NATURE_CRITTER_CUCCO_CROWS),
            NATURE_IO_CRITTER_6_BEND_PITCH(127),
            NATURE_IO_CRITTER_6_NUM_LAYERS(1),
            NATURE_IO_CRITTER_6_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_MARKET_ENTRANCE
    {
        0xC0FB, // PlayerIO Data
        0xC0FA, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_2_BEND_PITCH(112),
            NATURE_IO_CRITTER_2_NUM_LAYERS(1),
            NATURE_IO_CRITTER_2_PORT5(48),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_HAWK_SCREECH),
            NATURE_IO_CRITTER_3_BEND_PITCH(127),
            NATURE_IO_CRITTER_3_NUM_LAYERS(0),
            NATURE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            NATURE_IO_CRITTER_4_TYPE(NATURE_CRITTER_BIRD_CHIRP_1),
            NATURE_IO_CRITTER_4_BEND_PITCH(127),
            NATURE_IO_CRITTER_4_NUM_LAYERS(1),
            NATURE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            NATURE_IO_CRITTER_5_TYPE(NATURE_CRITTER_TAP),
            NATURE_IO_CRITTER_5_BEND_PITCH(127),
            NATURE_IO_CRITTER_5_NUM_LAYERS(3),
            NATURE_IO_CRITTER_5_PORT5(16),

            // Channel 7
            NATURE_IO_CRITTER_6_TYPE(NATURE_CRITTER_CUCCO_CROWS),
            NATURE_IO_CRITTER_6_BEND_PITCH(127),
            NATURE_IO_CRITTER_6_NUM_LAYERS(1),
            NATURE_IO_CRITTER_6_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_KAKARIKO_REGION
    {
        0xC001, // PlayerIO Data
        0x4000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_1_BEND_PITCH(48),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(32),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_MARKET_RUINS
    {
        0xC005, // PlayerIO Data
        0x4000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_HOWLING_WIND),
            NATURE_IO_STREAM_0_PORT3(32),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_1_BEND_PITCH(48),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(32),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_KOKIRI_REGION
    {
        0xC01F, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(47),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_OWL_HOOT),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_CAWING_BIRD),
            NATURE_IO_CRITTER_1_BEND_PITCH(0),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(32),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_HAWK_SCREECH),
            NATURE_IO_CRITTER_2_BEND_PITCH(0),
            NATURE_IO_CRITTER_2_NUM_LAYERS(0),
            NATURE_IO_CRITTER_2_PORT5(44),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_3_BEND_PITCH(63),
            NATURE_IO_CRITTER_3_NUM_LAYERS(1),
            NATURE_IO_CRITTER_3_PORT5(44),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_MARKET_NIGHT
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_06
    {
        0xC0FB, // PlayerIO Data
        0xC0FA, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_2_BEND_PITCH(112),
            NATURE_IO_CRITTER_2_NUM_LAYERS(1),
            NATURE_IO_CRITTER_2_PORT5(48),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_HAWK_SCREECH),
            NATURE_IO_CRITTER_3_BEND_PITCH(127),
            NATURE_IO_CRITTER_3_NUM_LAYERS(0),
            NATURE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            NATURE_IO_CRITTER_4_TYPE(NATURE_CRITTER_BIRD_CHIRP_1),
            NATURE_IO_CRITTER_4_BEND_PITCH(127),
            NATURE_IO_CRITTER_4_NUM_LAYERS(1),
            NATURE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            NATURE_IO_CRITTER_5_TYPE(NATURE_CRITTER_TAP),
            NATURE_IO_CRITTER_5_BEND_PITCH(127),
            NATURE_IO_CRITTER_5_NUM_LAYERS(3),
            NATURE_IO_CRITTER_5_PORT5(16),

            // Channel 7
            NATURE_IO_CRITTER_6_TYPE(NATURE_CRITTER_CUCCO_CROWS),
            NATURE_IO_CRITTER_6_BEND_PITCH(127),
            NATURE_IO_CRITTER_6_NUM_LAYERS(1),
            NATURE_IO_CRITTER_6_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_GANONS_LAIR
    {
        0x8001, // PlayerIO Data
        0x0,    // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_HOWLING_WIND),
            NATURE_IO_STREAM_0_PORT3(32),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_08
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_09
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_WASTELAND
    {
        0xC001, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_SCREECHING_WIND),
            NATURE_IO_STREAM_0_PORT3(0),
            NATURE_IO_STREAM_0_PORT4(0),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_COLOSSUS
    {
        0xC02F, // PlayerIO Data
        0xC02E, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_SCREECHING_WIND),
            NATURE_IO_STREAM_0_PORT3(0),
            NATURE_IO_STREAM_0_PORT4(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_SMALL_BIRD_CHIRPS),
            NATURE_IO_CRITTER_0_BEND_PITCH(64),
            NATURE_IO_CRITTER_0_NUM_LAYERS(0),
            NATURE_IO_CRITTER_0_PORT5(32),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_BIRD_CALL),
            NATURE_IO_CRITTER_1_BEND_PITCH(112),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(48),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_HAWK_SCREECH),
            NATURE_IO_CRITTER_2_BEND_PITCH(127),
            NATURE_IO_CRITTER_2_NUM_LAYERS(0),
            NATURE_IO_CRITTER_2_PORT5(16),

            // Channel 5
            NATURE_IO_CRITTER_4_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_4_BEND_PITCH(127),
            NATURE_IO_CRITTER_4_NUM_LAYERS(0),
            NATURE_IO_CRITTER_4_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_DEATH_MOUNTAIN_TRAIL
    {
        0xC07F, // PlayerIO Data
        0xC07E, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),
            NATURE_IO_STREAM_0_PORT4(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_SMALL_BIRD_CHIRPS),
            NATURE_IO_CRITTER_0_BEND_PITCH(64),
            NATURE_IO_CRITTER_0_NUM_LAYERS(0),
            NATURE_IO_CRITTER_0_PORT5(32),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_1_BEND_PITCH(112),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(48),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_BIRD_SONG),
            NATURE_IO_CRITTER_2_BEND_PITCH(127),
            NATURE_IO_CRITTER_2_NUM_LAYERS(0),
            NATURE_IO_CRITTER_2_PORT5(16),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_LOUD_CHIRPING),
            NATURE_IO_CRITTER_3_BEND_PITCH(0),
            NATURE_IO_CRITTER_3_NUM_LAYERS(0),
            NATURE_IO_CRITTER_3_PORT5(16),

            // Channel 5
            NATURE_IO_CRITTER_4_TYPE(NATURE_CRITTER_BIRD_CHIRP_1),
            NATURE_IO_CRITTER_4_BEND_PITCH(0),
            NATURE_IO_CRITTER_4_NUM_LAYERS(0),
            NATURE_IO_CRITTER_4_PORT5(16),

            // Channel 6
            NATURE_IO_CRITTER_5_TYPE(NATURE_CRITTER_TAP),
            NATURE_IO_CRITTER_5_BEND_PITCH(0),
            NATURE_IO_CRITTER_5_NUM_LAYERS(0),
            NATURE_IO_CRITTER_5_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_0D
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_0E
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_0F
    {
        0xC01F, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_BIRD_CHIRP_1),
            NATURE_IO_CRITTER_0_BEND_PITCH(80),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(8),

            // Channel 2
            NATURE_IO_CRITTER_1_TYPE(NATURE_CRITTER_SMALL_BIRD_CHIRPS),
            NATURE_IO_CRITTER_1_BEND_PITCH(80),
            NATURE_IO_CRITTER_1_NUM_LAYERS(1),
            NATURE_IO_CRITTER_1_PORT5(48),

            // Channel 3
            NATURE_IO_CRITTER_2_TYPE(NATURE_CRITTER_LOUD_CHIRPING),
            NATURE_IO_CRITTER_2_BEND_PITCH(0),
            NATURE_IO_CRITTER_2_NUM_LAYERS(0),
            NATURE_IO_CRITTER_2_PORT5(0),

            // Channel 4
            NATURE_IO_CRITTER_3_TYPE(NATURE_CRITTER_BIRD_SCREECH),
            NATURE_IO_CRITTER_3_BEND_PITCH(96),
            NATURE_IO_CRITTER_3_NUM_LAYERS(0),
            NATURE_IO_CRITTER_3_PORT5(32),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_10
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_11
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_12
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },

    // NATURE_ID_NONE
    // While there is data for this natureAmbienceId, it is identical to previous entries
    // and the game treats it as no nature ambience
    {
        0xC003, // PlayerIO Data
        0xC000, // Channel Mask
        {
            // Channel 0
            NATURE_IO_STREAM_0_TYPE(NATURE_STREAM_RUSHING_WATER),
            NATURE_IO_STREAM_0_PORT3(0),

            // Channel 1
            NATURE_IO_CRITTER_0_TYPE(NATURE_CRITTER_CRICKETS),
            NATURE_IO_CRITTER_0_BEND_PITCH(0),
            NATURE_IO_CRITTER_0_NUM_LAYERS(1),
            NATURE_IO_CRITTER_0_PORT5(16),

            // End
            NATURE_IO_ENTRIES_END,
        },
    },
};

#if !PLATFORM_N64
u32 na_oca_key_mask = (BTN_A | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT);
s32 a_button = BTN_A;
s32 u_cbuttons = BTN_CUP;
s32 d_cbuttons = BTN_CDOWN;
#endif

u8 na_oca_key_check = false;
s8 na_oca_flag = OCARINA_INSTRUMENT_OFF;
u8 na_oca_key_now = OCARINA_PITCH_NONE;
u8 na_oca_key_old = OCARINA_PITCH_C4;
u8 na_oca_key_btn = OCARINA_BTN_A;
u8 na_oca_key_last = OCARINA_PITCH_C4;
f32 na_oca_total_picth = 1.0f;
f32 na_oca_total_volume = 87.0f / 127.0f;
s8 na_oca_bend_now = 0;
s8 na_oca_s8_volume = 87;
s8 na_oca_vib_now = 0;
u8 na_oca_play = 0;
u32 na_oca_check = 0;
u32 na_oca_note_step = 0;
u16 na_oca_seq_step = 0;
u16 na_oca_keyon_step = 0;
u16 na_oca_continue = 0;
u8 na_oca_note_key = OCARINA_PITCH_NONE; // Pitch + PitchFlags
u8 na_oca_note_vol = 0;
u8 na_oca_note_vib = 0;
s8 na_oca_note_bend = 0;
f32 na_oca_seq_picth = 1.0f;
f32 na_oca_seq_volume = 1.0f;
s32 na_oca_vframe = 0;

u8 na_oca_btn_key[5] = {
    OCARINA_PITCH_D4, // OCARINA_BTN_A
    OCARINA_PITCH_F4, // OCARINA_BTN_C_DOWN
    OCARINA_PITCH_A4, // OCARINA_BTN_C_RIGHT
    OCARINA_PITCH_B4, // OCARINA_BTN_C_LEFT
    OCARINA_PITCH_D5, // OCARINA_BTN_C_UP
};

u8 na_oca_game_count = 0;
u8 na_oca_game_max = 0;
u8 na_oca_game_level_max[] = { 5, 6, 8 };

OcarinaNote na_oca_patern[OCARINA_SONG_MAX][20] = {
    // OCARINA_SONG_MINUET
    {
        { OCARINA_PITCH_D4, FRAMERATE_CONST(18, 15), 86, 0, 0, 0 },
        { OCARINA_PITCH_D5, FRAMERATE_CONST(18, 15), 92, 0, 0, 0 },
        { OCARINA_PITCH_B4, FRAMERATE_CONST(72, 60), 86, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(18, 15), 80, 0, 0, 0 },
        { OCARINA_PITCH_B4, FRAMERATE_CONST(18, 15), 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(144, 120), 86, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 86, 0, 0, 0 },
    },

    // OCARINA_SONG_BOLERO
    {
        { OCARINA_PITCH_F4, FRAMERATE_CONST(15, 12), 80, 0, 0, 0 },
        { OCARINA_PITCH_D4, FRAMERATE_CONST(15, 13), 72, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(15, 12), 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, FRAMERATE_CONST(15, 13), 76, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(15, 12), 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(15, 13), 74, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(15, 12), 78, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(135, 113), 66, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 66, 0, 0, 0 },
    },

    // OCARINA_SONG_SERENADE
    {
        { OCARINA_PITCH_D4, FRAMERATE_CONST(36, 30), 60, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(36, 30), 78, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(33, 27), 82, 0, 0, 0 },
        { OCARINA_PITCH_NONE, FRAMERATE_CONST(3, 3), 82, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(36, 30), 84, 0, 0, 0 },
        { OCARINA_PITCH_B4, FRAMERATE_CONST(144, 120), 90, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_REQUIEM
    {
        { OCARINA_PITCH_D4, FRAMERATE_CONST(45, 37), 88, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(23, 19), 86, 0, 0, 0 },
        { OCARINA_PITCH_D4, FRAMERATE_CONST(22, 19), 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(45, 37), 86, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(45, 38), 94, 0, 0, 0 },
        { OCARINA_PITCH_D4, FRAMERATE_CONST(180, 150), 94, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 94, 0, 0, 0 },
    },

    // OCARINA_SONG_NOCTURNE
    {
        { OCARINA_PITCH_B4, FRAMERATE_CONST(36, 30), 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(33, 27), 84, 0, 0, 0 },
        { OCARINA_PITCH_NONE, FRAMERATE_CONST(3, 3), 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(18, 15), 82, 0, 0, 0 },
        { OCARINA_PITCH_D4, FRAMERATE_CONST(18, 15), 60, 0, 0, 0 },
        { OCARINA_PITCH_B4, FRAMERATE_CONST(18, 15), 90, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(18, 15), 88, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(144, 120), 96, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 96, 0, 0, 0 },
    },

    // OCARINA_SONG_PRELUDE
    {
        { OCARINA_PITCH_D5, FRAMERATE_CONST(15, 12), 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(45, 38), 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, FRAMERATE_CONST(15, 12), 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(15, 13), 82, 0, 0, 0 },
        { OCARINA_PITCH_B4, FRAMERATE_CONST(15, 12), 86, 0, 0, 0 },
        { OCARINA_PITCH_D5, FRAMERATE_CONST(60, 50), 90, 0, 0, 0 },
        { OCARINA_PITCH_NONE, FRAMERATE_CONST(75, 63), 90, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_SARIAS
    {
        { OCARINA_PITCH_F4, FRAMERATE_CONST(17, 14), 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(17, 14), 88, 0, 0, 0 },
        { OCARINA_PITCH_B4, FRAMERATE_CONST(34, 28), 80, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(17, 14), 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(17, 14), 88, 0, 0, 0 },
        { OCARINA_PITCH_B4, FRAMERATE_CONST(136, 113), 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_EPONAS
    {
        { OCARINA_PITCH_D5, FRAMERATE_CONST(18, 15), 84, 0, 0, 0 },
        { OCARINA_PITCH_B4, FRAMERATE_CONST(18, 15), 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(72, 60), 80, 0, 0, 0 },
        { OCARINA_PITCH_D5, FRAMERATE_CONST(18, 15), 84, 0, 0, 0 },
        { OCARINA_PITCH_B4, FRAMERATE_CONST(18, 15), 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(144, 120), 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_LULLABY
    {
        { OCARINA_PITCH_B4, FRAMERATE_CONST(51, 42), 84, 0, 0, 0 },
        { OCARINA_PITCH_D5, FRAMERATE_CONST(25, 21), 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(78, 65), 80, 0, 0, 0 },
        { OCARINA_PITCH_B4, FRAMERATE_CONST(51, 42), 84, 0, 0, 0 },
        { OCARINA_PITCH_D5, FRAMERATE_CONST(25, 21), 88, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(100, 83), 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_SUNS
    {
        { OCARINA_PITCH_A4, FRAMERATE_CONST(12, 10), 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(13, 10), 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, FRAMERATE_CONST(29, 25), 80, 2, 0, 0 },
        { OCARINA_PITCH_NONE, FRAMERATE_CONST(9, 9), 84, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(12, 10), 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(13, 10), 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, FRAMERATE_CONST(120, 100), 80, 3, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_TIME
    {
        { OCARINA_PITCH_A4, FRAMERATE_CONST(32, 26), 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, FRAMERATE_CONST(65, 54), 88, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(33, 28), 80, 0, 0, 0 },
        { OCARINA_PITCH_A4, FRAMERATE_CONST(32, 26), 84, 0, 0, 0 },
        { OCARINA_PITCH_D4, FRAMERATE_CONST(65, 54), 88, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(99, 83), 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_STORMS
    {
        { OCARINA_PITCH_D4, FRAMERATE_CONST(11, 9), 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(11, 9), 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, FRAMERATE_CONST(45, 37), 80, 0, 0, 0 },
        { OCARINA_PITCH_D4, FRAMERATE_CONST(11, 9), 84, 0, 0, 0 },
        { OCARINA_PITCH_F4, FRAMERATE_CONST(11, 9), 88, 0, 0, 0 },
        { OCARINA_PITCH_D5, FRAMERATE_CONST(90, 75), 80, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 90, 0, 0, 0 },
    },

    // OCARINA_SONG_SCARECROW_SPAWN
    {
        { OCARINA_PITCH_D4, FRAMERATE_CONST(3, 3), 0, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 255, 0, 0, 0 },
    },

    // OCARINA_SONG_MEMORY_GAME
    {
        { OCARINA_PITCH_D4, FRAMERATE_CONST(3, 3), 0, 0, 0, 0 },
        { OCARINA_PITCH_NONE, 0, 0, 0, 0, 0 },
    },
};

OcarinaNote* na_oca_now_seq = na_oca_patern[0];
u8 na_frog_tbl_data[14] = {
    OCARINA_BTN_A,       OCARINA_BTN_C_LEFT,  OCARINA_BTN_C_RIGHT, OCARINA_BTN_C_DOWN, OCARINA_BTN_C_LEFT,
    OCARINA_BTN_C_RIGHT, OCARINA_BTN_C_DOWN,  OCARINA_BTN_A,       OCARINA_BTN_C_DOWN, OCARINA_BTN_A,
    OCARINA_BTN_C_DOWN,  OCARINA_BTN_C_RIGHT, OCARINA_BTN_C_LEFT,  OCARINA_BTN_A,
};
u8* na_frog_tbl_ptr = na_frog_tbl_data;
u8 na_oca_record = OCARINA_RECORD_OFF;
u8 na_oca_rec_point = 0;
u32 na_oca_rec_step = 0;
u8 na_oca_rec_key = 0;
u8 na_oca_rec_vol = 0;
u8 na_oca_rec_vib = 0;
s8 na_oca_rec_bend = 0;
u8 na_oca_rec_btn = 0;
u8 na_oca_correct_num = 0;
u8 na_oca_chk_max = 0;
u8 na_oca_key_wait_count = 0;

OcarinaNote na_oca_rec_buf[108] = {
    { OCARINA_PITCH_NONE, 0, 0, 0, 0, 0 },
    { OCARINA_PITCH_NONE, 0, 0, 0, 0, 0 },
};
OcarinaNote* na_oca_rec_buf_ptr = na_oca_rec_buf;

u8* na_oca_rec_buf2_ptr = (u8*)&na_oca_patern[OCARINA_SONG_SCARECROW_SPAWN];
OcarinaNote* na_oca_rec_buf_ptr_temp = na_oca_patern[OCARINA_SONG_MEMORY_GAME];

u8 note_key_to_button[16] = {
    OCARINA_BTN_A,                 // OCARINA_PITCH_C4
    OCARINA_BTN_A,                 // OCARINA_PITCH_DFLAT4
    OCARINA_BTN_A,                 // OCARINA_PITCH_D4
    OCARINA_BTN_A,                 // OCARINA_PITCH_EFLAT4
    OCARINA_BTN_C_DOWN,            // OCARINA_PITCH_E4
    OCARINA_BTN_C_DOWN,            // OCARINA_PITCH_F4
    OCARINA_BTN_C_DOWN,            // OCARINA_PITCH_GFLAT4
    OCARINA_BTN_C_RIGHT,           // OCARINA_PITCH_G4
    OCARINA_BTN_C_RIGHT,           // OCARINA_PITCH_AFLAT4
    OCARINA_BTN_C_RIGHT,           // OCARINA_PITCH_A4
    OCARINA_BTN_C_RIGHT_OR_C_LEFT, // OCARINA_PITCH_BFLAT4: Interface/Overlap between C_RIGHT and C_LEFT
    OCARINA_BTN_C_LEFT,            // OCARINA_PITCH_B4
    OCARINA_BTN_C_LEFT,            // OCARINA_PITCH_C5
    OCARINA_BTN_C_UP,              // OCARINA_PITCH_DFLAT5
    OCARINA_BTN_C_UP,              // OCARINA_PITCH_D5
    OCARINA_BTN_C_UP,              // OCARINA_PITCH_EFLAT5
};

OcarinaSongButtons na_oca_note[OCARINA_SONG_MAX] = {
    // OCARINA_SONG_MINUET
    { 6,
      {
          OCARINA_BTN_A,
          OCARINA_BTN_C_UP,
          OCARINA_BTN_C_LEFT,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_LEFT,
          OCARINA_BTN_C_RIGHT,
      } },
    // OCARINA_SONG_BOLERO
    { 8,
      {
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_A,
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_A,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_DOWN,
      } },
    // OCARINA_SONG_SERENADE
    { 5,
      {
          OCARINA_BTN_A,
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_LEFT,
      } },
    // OCARINA_SONG_REQUIEM
    { 6,
      {
          OCARINA_BTN_A,
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_A,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_A,
      } },
    // OCARINA_SONG_NOCTURNE
    { 7,
      {
          OCARINA_BTN_C_LEFT,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_A,
          OCARINA_BTN_C_LEFT,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_DOWN,
      } },
    // OCARINA_SONG_PRELUDE
    { 6,
      {
          OCARINA_BTN_C_UP,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_UP,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_LEFT,
          OCARINA_BTN_C_UP,
      } },
    // OCARINA_SONG_SARIAS
    { 6,
      {
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_LEFT,
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_LEFT,
      } },
    // OCARINA_SONG_EPONAS
    { 6,
      {
          OCARINA_BTN_C_UP,
          OCARINA_BTN_C_LEFT,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_UP,
          OCARINA_BTN_C_LEFT,
          OCARINA_BTN_C_RIGHT,
      } },
    // OCARINA_SONG_LULLABY
    { 6,
      {
          OCARINA_BTN_C_LEFT,
          OCARINA_BTN_C_UP,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_LEFT,
          OCARINA_BTN_C_UP,
          OCARINA_BTN_C_RIGHT,
      } },
    // OCARINA_SONG_SUNS
    { 6,
      {
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_C_UP,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_C_UP,
      } },
    // OCARINA_SONG_TIME
    { 6,
      {
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_A,
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_C_RIGHT,
          OCARINA_BTN_A,
          OCARINA_BTN_C_DOWN,
      } },
    // OCARINA_SONG_STORMS
    { 6,
      {
          OCARINA_BTN_A,
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_C_UP,
          OCARINA_BTN_A,
          OCARINA_BTN_C_DOWN,
          OCARINA_BTN_C_UP,
      } },
    // OCARINA_SONG_SCARECROW_SPAWN
    { 8, { 0 } },
    // OCARINA_SONG_MEMORY_GAME
    { 0, { 0 } },
};

#if DEBUG_FEATURES
u32 sAudioUpdateStartTime;
u32 sAudioUpdateEndTime;
#endif
f32 na_link_walk_volume;
f32 na_link_walk_volume2;
f32 na_link_walk_pitch;
f32 D_8016B7B4;
FreqLerp na_river_pitch;
FreqLerp na_waterfall_pitch;
f32 na_motor_pitch;
s8 na_set_fx_param;
f32 na_set_volume_param;
#if DEBUG_FEATURES
u16 D_8016B7E4;
struct {
    char str[5];
    u16 num;
} sAudioScrPrtBuf[SCROLL_PRINT_BUF_SIZE];
#endif
u8 na_level_mute_vol_target;
u8 na_level_mute_vol_now;
u8 na_level_mute_flag_next;
u8 na_level_mute_flag_old;
u8 na_trigger_mute_vol_now;
SfxPlayerState track_update[0x10];
#if DEBUG_FEATURES
char sBinToStrBuf[0x20];
#endif
u8 na_ronron_bgm_mode;
#if DEBUG_FEATURES
u8 sAudioSpecPeakNumNotes[0x12];
#endif
u8 na_mute_bgm_flag;
u8 na_cross_bgm_delay;
u8 na_fanfare_delay;
u16 na_fanfare_flag;

#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
u16 sPrevAmbienceSeqId;
#endif

OcarinaStaff na_oca_play_info;
OcarinaStaff na_oca_seq_info;
OcarinaStaff na_oca_rec_info;
u32 na_oca_framecounter;
OcarinaStick na_stick;
u32 na_oca_pad_button;
u32 na_oca_pad_init_stat;
u32 na_oca_pad_before;
s32 na_oca_pad_last;
u8 na_oca_input_buffer[8];
u8 na_oca_input_ptr;
u8 na_oca_start_check;
u8 na_oca_chk_start;
u8 na_oca_chk_end;
u16 na_oca_melody;
u8 na_oca_chk_note_count;
u16 na_oca_chk_seq_step[OCARINA_SONG_MAX];
u16 na_oca_chk_key_step[OCARINA_SONG_MAX];
u16 na_oca_chk_org_step[OCARINA_SONG_MAX];
u8 na_oca_chk_org_key[OCARINA_SONG_MAX];
OcarinaNote na_oca_line0_buf;
#if DEBUG_FEATURES
u8 sIsMalonSinging;
f32 sMalonSingingDist;
u32 sDebugPadHold;
u32 sDebugPadBtnLast;
u32 sDebugPadPress;
s32 sAudioUpdateTaskStart;
s32 sAudioUpdateTaskEnd;
#endif

void padmgr_RequestPadData(PadMgr* padMgr, Input* inputs, s32 gameRequest);

void Na_CheckMovePitch(FreqLerp* lerp);
void Na_CheckContinueBgm(void);
void Na_NscInitStart(u8 natureAmbienceId);
s32 Na_SetTriggerMuteFlag(u8 targetVol);

// =========== Audio Ocarina ===========

#if PLATFORM_N64

#define OCARINA_ALLOWED_BUTTON_MASK (BTN_A | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT)
#define OCARINA_A_MAP BTN_A
#define OCARINA_CUP_MAP BTN_CUP
#define OCARINA_CDOWN_MAP BTN_CDOWN

#else

#define OCARINA_ALLOWED_BUTTON_MASK na_oca_key_mask
#define OCARINA_A_MAP a_button
#define OCARINA_CUP_MAP u_cbuttons
#define OCARINA_CDOWN_MAP d_cbuttons

void Na_ChangeOcarinaControl(u8 useCustom) {
    if (!useCustom) {
        AUDIO_PRINTF("AUDIO : Ocarina Control Assign Normal\n");
        OCARINA_ALLOWED_BUTTON_MASK = (BTN_A | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT);
        OCARINA_A_MAP = BTN_A;
        OCARINA_CUP_MAP = BTN_CUP;
        OCARINA_CDOWN_MAP = BTN_CDOWN;
    } else {
        AUDIO_PRINTF("AUDIO : Ocarina Control Assign Custom\n");
        OCARINA_ALLOWED_BUTTON_MASK = (BTN_A | BTN_B | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT);
        OCARINA_A_MAP = BTN_B;
        OCARINA_CUP_MAP = BTN_CDOWN;
        OCARINA_CDOWN_MAP = BTN_A;
    }
}

#endif

void Na_GetZeldaController(void) {
    Input inputs[MAXCONTROLLERS];
    Input* input = &inputs[0];
    u32 ocarinaInputButtonPrev = na_oca_pad_button;

    padmgr_RequestPadData(&padmgr, inputs, false);
    na_oca_pad_button = input->cur.button;
    na_oca_pad_before = ocarinaInputButtonPrev;
    na_stick.x = input->rel.stick_x;
    na_stick.y = input->rel.stick_y;
}

/**
 * Looks up the frequency to bend the pitch by.
 * The pitch will bend up to a maximum of 2 semitones
 * in each direction giving a total range of 4 semitones
 */
f32 Na_StickToBend(s8 bendIndex) {
    s8 adjBendIndex;
    f32 bendFreq;

    if (bendIndex > 64) {
        adjBendIndex = 127;
    } else if (bendIndex < -64) {
        adjBendIndex = -128;
    } else if (bendIndex >= 0) {
        adjBendIndex = (bendIndex * 127) / 64;
    } else {
        adjBendIndex = (bendIndex * 128) / 64;
    }

    /**
     * index 128 is in the middle of the table and
     * contains the value 1.0f i.e. no bend
     * absolute indices above 128 will bend the pitch 2 semitones upwards
     * absolute indices below 128 will bend the pitch 2 semitones downwards
     */
    bendFreq = PCENTTABLE2[adjBendIndex + 128];
    return bendFreq;
}

/**
 * If an available song has been played, then return that song index
 * If the ocarina is on, but no song has been played then return 0xFE
 * If the ocarina is off, return 0xFF
 */
u8 Na_GetOcarinaCheckStatus(void) {
    u8 playedOcarinaSongIndex;

    if (na_oca_correct_num != 0) {
        playedOcarinaSongIndex = na_oca_correct_num - 1;
        na_oca_correct_num = 0;
    } else if (na_oca_check != 0) {
        playedOcarinaSongIndex = 0xFE;
    } else {
        playedOcarinaSongIndex = 0xFF;
    }

    return playedOcarinaSongIndex;
}

u8 Na_GetNoteKeyToButton(u8 pitchAndBFlatFlag) {
    u8 buttonIndex = note_key_to_button[pitchAndBFlatFlag & 0x3F];

    /**
     * Special case for bFlat4:
     * CRIGHT and CLEFT are the only two pitches that are 2 semitones apart
     * which are pitches A4 and B4 respectively
     * bFlat4 is in the middle of those two and is the only pitches that can not
     * be resolved between the two buttons without external information.
     * That information is stored as flags in pitch with the mask:
     * (pitchAndBFlatFlag & 0xC0)
     */
    if (buttonIndex == OCARINA_BTN_C_RIGHT_OR_C_LEFT) {
        if (pitchAndBFlatFlag & 0x80) {
            return OCARINA_BTN_C_RIGHT;
        }
        return OCARINA_BTN_C_LEFT;
    }

    return buttonIndex;
}

void Na_MakeRec8Score(u8 noteSongIndex) {
    u8 buttonSongPos = 0;
    u8 noteSongPos = 0;
    u8 pitch;

    while (buttonSongPos < 8 && noteSongPos < 16) {
        pitch = na_oca_patern[noteSongIndex][noteSongPos++].pitch;

        if (pitch != OCARINA_PITCH_NONE) {
            na_oca_note[OCARINA_SONG_SCARECROW_SPAWN].buttonsIndex[buttonSongPos++] = note_key_to_button[pitch];
        }
    }
}

/**
 * Ocarina flags:
 * bitmask 0x3FFF:
 *      - Ocarina song id
 * bitmask 0xC000:
 *      - 0x0000: Limits the notes to 8 notes at a time. Not playing a correct song after 8 notes will cause an ocarina
 * error
 *      - 0x4000: (Identical to 0xC000)
 *      - 0x8000: Limits the notes to 1 note at a time. A single incorrect note will cause an ocarina error
 *      - 0xC000: Free-play, no limitations to the number of notes to play
 * bitmask 0x7FFF0000:
 *      - ocarina action (only used to make flags != 0)
 * bitmask 0x80000000:
 *      - unused (only used to make flags != 0)
 */
void Na_SetOcarinaPlayCheckFlag2(u16 ocarinaFlags) {
    u8 i;

    if ((na_oca_patern[OCARINA_SONG_SCARECROW_SPAWN][1].volume != 0xFF) && ((ocarinaFlags & 0xFFF) == 0xFFF)) {
        ocarinaFlags |= 0x1000;
    }

    if ((ocarinaFlags == 0xCFFF) && (na_oca_patern[OCARINA_SONG_SCARECROW_SPAWN][1].volume != 0xFF)) {
        ocarinaFlags = 0xDFFF;
    }

    if ((ocarinaFlags == 0xFFF) && (na_oca_patern[OCARINA_SONG_SCARECROW_SPAWN][1].volume != 0xFF)) {
        ocarinaFlags = 0x1FFF;
    }

    if (ocarinaFlags != 0xFFFF) {
        na_oca_check = 0x80000000 + (u32)ocarinaFlags;
        na_oca_chk_start = 0;
        na_oca_chk_end = OCARINA_SONG_MAX;
        if (ocarinaFlags != 0xA000) {
            na_oca_chk_end--;
        }
        na_oca_melody = ocarinaFlags & 0x3FFF;
        na_oca_chk_max = 8; // Ocarina Check
        na_oca_start_check = false;
        na_oca_correct_num = 0;
        na_oca_chk_note_count = 0;
        na_oca_play_info.state = Na_GetOcarinaCheckStatus();
        na_oca_key_check = true;
        na_oca_continue = 0;

        // Reset music staff song check
        for (i = 0; i < OCARINA_SONG_MAX; i++) {
            na_oca_chk_seq_step[i] = 0;
            na_oca_chk_key_step[i] = 0;
            na_oca_chk_org_step[i] = 0;
            na_oca_chk_org_key[i] = 0;
        }

        if (ocarinaFlags & 0x8000) {
            na_oca_chk_max = 0; // Ocarina Playback
        }

        if (ocarinaFlags & 0x4000) {
            na_oca_input_ptr = 0;
        }

        if (ocarinaFlags & 0xD000) {
            Na_MakeRec8Score(OCARINA_SONG_SCARECROW_SPAWN);
        }
    } else {
        na_oca_check = 0;
        na_oca_key_check = false;
    }
}

void Na_CheckOcarinaInputStart(void) {
    if (na_oca_key_now != OCARINA_PITCH_NONE && !na_oca_start_check) {
        na_oca_start_check = true;
        na_oca_key_last = OCARINA_PITCH_NONE;
    }
}

/**
 * Checks for ocarina songs from user input with a music staff prompt
 * Type 1) Playback: tests note-by-note (ocarinaFlag & 0xC000 == 0x8000) eg:
 *      - learning a new song
 *      - playing the ocarina memory game
 * Type 2) Check: tests in 8-note chunks (ocarinaFlag & 0xC000 == 0x0000) eg:
 *      - validating scarecrow spawn song as adult
 *      - ocarina prompt for zelda's lullaby, saria's song, Storms, Song of Time, etc...
 */
void Na_CheckOcarinaInputLearn(void) {
    u16 curOcarinaSongFlag;
    u16 pad;
    u8 noNewValidInput = false;
    u16 pad2;
    s8 staffOcarinaPlayingPosOffset = 0;
    u8 songIndex;
    OcarinaNote* curNote;
    OcarinaNote* nextNote;

    Na_CheckOcarinaInputStart();

    if (!na_oca_start_check) {
        return;
    }

    if (ABS_ALT(na_oca_bend_now) > 20) {
        na_oca_check = 0;
        return;
    }

    // clang-format off
    if (na_oca_key_old == na_oca_key_now || na_oca_key_now == OCARINA_PITCH_NONE) { noNewValidInput = true; }
    // clang-format on

    for (songIndex = na_oca_chk_start; songIndex < na_oca_chk_end; songIndex++) {
        curOcarinaSongFlag = 1 << songIndex;

        if (na_oca_melody & curOcarinaSongFlag) {
            na_oca_chk_key_step[songIndex] = na_oca_chk_org_step[songIndex] + 18;

            if (noNewValidInput) {
                if ((na_oca_chk_key_step[songIndex] >= na_oca_chk_org_step[songIndex] - 18) &&
                    (na_oca_chk_key_step[songIndex] >= na_oca_chk_org_step[songIndex] + 18) &&
                    (na_oca_patern[songIndex][na_oca_chk_seq_step[songIndex]].length == 0) &&
                    (na_oca_key_last == na_oca_chk_org_key[songIndex])) {
                    // This case is taken if the song is finished and successfully played
                    // (i.e. .length == 0 indicates that the song is at the end)
                    na_oca_correct_num = songIndex + 1;
                    na_oca_key_check = false;
                    na_oca_check = 0;
                }
            } else if (na_oca_chk_key_step[songIndex] >= (na_oca_chk_org_step[songIndex] - 18)) {
                // This else-if statement always holds true, taken if a new note is played
                if (na_oca_key_last != OCARINA_PITCH_NONE) {
                    // New note is played
                    if (na_oca_key_last == na_oca_chk_org_key[songIndex]) {
                        // Note is part of expected song
                        if (songIndex == OCARINA_SONG_SCARECROW_SPAWN) {
                            na_oca_chk_key_step[songIndex] = 0;
                        }
                    } else {
                        // Note is not part of expected song, so this song is no longer available as an option in this
                        // playback
                        na_oca_melody ^= curOcarinaSongFlag;
                    }
                }

                curNote = &na_oca_patern[songIndex][na_oca_chk_seq_step[songIndex]];
                nextNote = &na_oca_patern[songIndex][++na_oca_chk_seq_step[songIndex]];
                na_oca_chk_org_step[songIndex] = curNote->length;
                na_oca_chk_org_key[songIndex] = curNote->pitch;

                // The current note is not the expected note.
                if (na_oca_key_now != na_oca_chk_org_key[songIndex]) {
                    na_oca_melody ^= curOcarinaSongFlag;
                }

                while (curNote->pitch == nextNote->pitch ||
                       (nextNote->pitch == OCARINA_BTN_INVALID && nextNote->length != 0)) {
                    na_oca_chk_org_step[songIndex] += nextNote->length;
                    curNote = &na_oca_patern[songIndex][na_oca_chk_seq_step[songIndex]];
                    nextNote = &na_oca_patern[songIndex][na_oca_chk_seq_step[songIndex] + 1];
                    na_oca_chk_seq_step[songIndex]++;
                }
            } else if (na_oca_chk_key_step[songIndex] < 10) {
                // case never taken
                staffOcarinaPlayingPosOffset = -1;
                na_oca_chk_key_step[songIndex] = 0;
                na_oca_key_last = na_oca_key_now;
            } else {
                // case never taken
                na_oca_melody ^= curOcarinaSongFlag;
            }
        }

        // if a note is played that doesn't match a song, the song bit in na_oca_melody is turned off
        // if there are no more songs remaining that it could be and the maximum position has been exceeded, then
        if (na_oca_melody == 0 && na_oca_chk_note_count >= na_oca_chk_max) {
            na_oca_key_check = false;
            if ((na_oca_check & 0x4000) && na_oca_key_now == na_oca_patern[songIndex][0].pitch) {
                // case never taken, this function is not called if (na_oca_check & 0x4000) is set
                na_oca_continue = na_oca_check;
            }
            na_oca_check = 0;
            return;
        }
    }

    if (!noNewValidInput) {
        na_oca_key_last = na_oca_key_now;
        na_oca_chk_note_count += staffOcarinaPlayingPosOffset + 1;
    }
}

/**
 * Checks for ocarina songs from user input with no music staff prompt.
 * Includes ocarina actions such as free play, no warp
 */
void Na_CheckOcarinaInputFree(void) {
    u32 pitch;
    u8 i;
    u8 j;
    u8 k;

    if (CHECK_BTN_ANY(na_oca_pad_button, BTN_L) &&
        CHECK_BTN_ANY(na_oca_pad_button, OCARINA_ALLOWED_BUTTON_MASK)) {
        Na_SetOcarinaPlayCheckFlag2((u16)na_oca_check);
        return;
    }

    Na_CheckOcarinaInputStart();

    if (!na_oca_start_check) {
        return;
    }

    if ((na_oca_key_old != na_oca_key_now) && (na_oca_key_now != OCARINA_PITCH_NONE)) {
        na_oca_chk_note_count++;
        if (na_oca_chk_note_count > ARRAY_COUNT(na_oca_input_buffer)) {
            na_oca_chk_note_count = 1;
        }

        if (na_oca_input_ptr == 8) {
            for (i = 0; i < 7; i++) {
                na_oca_input_buffer[i] = na_oca_input_buffer[i + 1];
            }
        } else {
            na_oca_input_ptr++;
        }

        if (ABS_ALT(na_oca_bend_now) > 20) {
            na_oca_input_buffer[na_oca_input_ptr - 1] = OCARINA_PITCH_NONE;
        } else {
            na_oca_input_buffer[na_oca_input_ptr - 1] = na_oca_key_now;
        }

        // This nested for-loop tests to see if the notes from the ocarina are identical
        // to any of the songIndex from na_oca_chk_start to na_oca_chk_end

        // Loop through each of the songs
        for (i = na_oca_chk_start; i < na_oca_chk_end; i++) {
            // Checks to see if the song is available to be played
            if (na_oca_melody & (u16)(1 << i)) {
                for (j = 0, k = 0; j < na_oca_note[i].numButtons && k == 0 &&
                                   na_oca_input_ptr >= na_oca_note[i].numButtons;) {
                    pitch = na_oca_input_buffer[na_oca_input_ptr -
                                                             na_oca_note[i].numButtons + j];
                    if (pitch == na_oca_btn_key[na_oca_note[i].buttonsIndex[j]]) {
                        j++;
                    } else {
                        k++;
                    }
                }

                // This conditional is true if songIndex = i is detected
                if (j == na_oca_note[i].numButtons) {
                    na_oca_correct_num = i + 1;
                    na_oca_key_check = false;
                    na_oca_check = 0;
                }
            }
        }
    }
}

// This unused argument is used in Majora's Mask as a u8
void Na_CheckOcarinaControl(u8 unused) {
    u32 ocarinaBtnsHeld;

    // Prevents two different ocarina notes from being played on two consecutive frames
    if ((na_oca_check != 0) && (na_oca_key_wait_count != 0)) {
        na_oca_key_wait_count--;
        return;
    }

    // Ensures the button pressed to start the ocarina does not also play an ocarina note
    if ((na_oca_pad_init_stat == 0) || ((na_oca_pad_init_stat & OCARINA_ALLOWED_BUTTON_MASK) !=
                                            (na_oca_pad_button & OCARINA_ALLOWED_BUTTON_MASK))) {
        na_oca_pad_init_stat = 0;
        if (1) {}
        na_oca_key_now = OCARINA_PITCH_NONE;
        na_oca_key_btn = OCARINA_BTN_INVALID;
        ocarinaBtnsHeld = (na_oca_pad_button & OCARINA_ALLOWED_BUTTON_MASK) &
                          (na_oca_pad_before & OCARINA_ALLOWED_BUTTON_MASK);
        if (!(na_oca_pad_last & ocarinaBtnsHeld) && (na_oca_pad_button != 0)) {
            na_oca_pad_last = na_oca_pad_button;
        } else {
            na_oca_pad_last &= ocarinaBtnsHeld;
        }

        // Interprets and transforms controller input into ocarina buttons and notes
        if (CHECK_BTN_ANY(na_oca_pad_last, OCARINA_A_MAP)) {
            AUDIO_PRINTF("Presss NA_KEY_D4 %08x\n", OCARINA_A_MAP);
            na_oca_key_now = OCARINA_PITCH_D4;
            na_oca_key_btn = OCARINA_BTN_A;

        } else if (CHECK_BTN_ANY(na_oca_pad_last, OCARINA_CDOWN_MAP)) {
            AUDIO_PRINTF("Presss NA_KEY_F4 %08x\n", OCARINA_CDOWN_MAP);
            na_oca_key_now = OCARINA_PITCH_F4;
            na_oca_key_btn = OCARINA_BTN_C_DOWN;

        } else if (CHECK_BTN_ANY(na_oca_pad_last, BTN_CRIGHT)) {
            AUDIO_PRINTF("Presss NA_KEY_A4 %08x\n", BTN_CRIGHT);
            na_oca_key_now = OCARINA_PITCH_A4;
            na_oca_key_btn = OCARINA_BTN_C_RIGHT;

        } else if (CHECK_BTN_ANY(na_oca_pad_last, BTN_CLEFT)) {
            AUDIO_PRINTF("Presss NA_KEY_B4 %08x\n", BTN_CLEFT);
            na_oca_key_now = OCARINA_PITCH_B4;
            na_oca_key_btn = OCARINA_BTN_C_LEFT;

        } else if (CHECK_BTN_ANY(na_oca_pad_last, OCARINA_CUP_MAP)) {
            AUDIO_PRINTF("Presss NA_KEY_D5 %08x\n", OCARINA_CUP_MAP);
            na_oca_key_now = OCARINA_PITCH_D5;
            na_oca_key_btn = OCARINA_BTN_C_UP;
        }

#if PLATFORM_N64
        if (na_oca_pad_button) {}
#endif

        // Pressing the R Button will raise the pitch by 1 semitone
        if ((na_oca_key_now != OCARINA_PITCH_NONE) && CHECK_BTN_ANY(na_oca_pad_button, BTN_R) &&
            (na_oca_record != OCARINA_RECORD_SCARECROW_SPAWN)) {
            na_oca_key_btn += 0x80; // Flag to resolve B Flat 4
            na_oca_key_now++;             // Raise the pitch by 1 semitone
        }

        // Pressing the Z Button will lower the pitch by 1 semitone
        if ((na_oca_key_now != OCARINA_PITCH_NONE) && CHECK_BTN_ANY(na_oca_pad_button, BTN_Z) &&
            (na_oca_record != OCARINA_RECORD_SCARECROW_SPAWN)) {
            na_oca_key_btn += 0x40; // Flag to resolve B Flat 4
            na_oca_key_now--;             // Lower the pitch by 1 semitone
        }

        if (na_oca_record != OCARINA_RECORD_SCARECROW_SPAWN) {
            // Bend the pitch of the note based on y control stick
            na_oca_bend_now = na_stick.y;
            na_oca_total_picth = Na_StickToBend(na_oca_bend_now);

            // Add vibrato of the ocarina note based on the x control stick
            na_oca_vib_now = ABS_ALT(na_stick.x) >> 2;
            // Sets vibrato to io port 6
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 6, na_oca_vib_now);
        } else {
            // no bending or vibrato for recording state OCARINA_RECORD_SCARECROW_SPAWN
            na_oca_bend_now = 0;
#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
            na_oca_vib_now = 0;
#endif
            na_oca_total_picth = 1.0f; // No bend
        }

        // Processes new and valid notes
        if ((na_oca_key_now != OCARINA_PITCH_NONE) && (na_oca_key_old != na_oca_key_now)) {
            // Sets ocarina instrument Id to channelIndex io port 7, which is used
            // as an index in seq 0 to get the true instrument Id
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 7, na_oca_flag - 1);
            // Sets pitch to io port 5
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 5, na_oca_key_now);
            Nai_FxFlagEntry(NA_SE_OC_OCARINA, &_dummy_zero_f, 4, &na_oca_total_picth, &na_oca_total_volume,
                                 &_dummy_zero_s8);
        } else if ((na_oca_key_old != OCARINA_PITCH_NONE) && (na_oca_key_now == OCARINA_PITCH_NONE)) {
            // Stops ocarina sound when transitioning from playing to not playing a note
            Nai_StopFx(NA_SE_OC_OCARINA);
        }
    }
}

/**
 * Directly enable the ocarina to receive input without
 * properly resetting it based on an ocarina instrument id
 * Unused.
 */
void Na_SetOcarinaKeyInputFlag(u8 inputEnabled) {
    na_oca_key_check = inputEnabled;
}

/**
 * Resets ocarina properties based on the ocarina instrument id
 * If ocarina instrument id is "OCARINA_INSTRUMENT_OFF", turn off the ocarina
 * For all ocarina instrument ids, turn the ocarina on with the instrument id
 */
void Na_SetOcarinaModeFlag(u8 ocarinaInstrumentId) {
    if (na_oca_flag == ocarinaInstrumentId) {
        return;
    }

    SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 1, ocarinaInstrumentId);
    na_oca_flag = ocarinaInstrumentId;
    if (ocarinaInstrumentId == OCARINA_INSTRUMENT_OFF) {
        na_oca_pad_button = 0;
        na_oca_pad_before = 0;
        na_oca_pad_last = 0;

        na_oca_pad_init_stat = 0xFFFF;

        Na_CheckOcarinaControl(false);
        Nai_StopFx(NA_SE_OC_OCARINA);
        Nai_FxSetLockFlag(0);
        na_oca_play = 0;
        na_oca_keyon_step = 0;
        na_oca_key_check = false;
        na_oca_check = 0;
        // return to full volume for players 0 and 3 (background bgm) after ocarina is finished
        Nai_FxClearBgmMuteFlag(SFX_CHANNEL_OCARINA);
    } else {
        na_oca_pad_button = 0;
        Na_GetZeldaController();
        // Store button used to turn on ocarina
        na_oca_pad_init_stat = na_oca_pad_button;
        // lowers volumes of players 0 and 3 (background bgm) while playing ocarina
        Nai_FxSetBgmMuteFlag(SFX_CHANNEL_OCARINA);
    }
}

void Na_SetOcarinaSeq(s8 songIndexPlusOne, s8 playbackState) {
    if (songIndexPlusOne == 0) {
        na_oca_play = 0;
        Nai_StopFx(NA_SE_OC_OCARINA);
        return;
    }

    if (songIndexPlusOne < (OCARINA_SONG_SCARECROW_LONG + 1)) {
        na_oca_now_seq = na_oca_patern[songIndexPlusOne - 1];
    } else {
        na_oca_now_seq = na_oca_rec_buf;
    }

    na_oca_play = playbackState;
    na_oca_note_step = 0;
    na_oca_note_key = OCARINA_PITCH_NONE;
    na_oca_seq_step = 0;
    na_oca_keyon_step = 0;

    while (na_oca_now_seq[na_oca_seq_step].pitch == OCARINA_PITCH_NONE) {
        na_oca_seq_step++;
    }
}

/**
 * Play a song with the ocarina to the user that is
 * based on OcarinaNote data and not user input
 */
void Na_PlayOcarinaSeq(void) {
    u32 noteTimerStep;
    u32 nextNoteTimerStep;

    if (na_oca_play == 0) {
        return;
    }

    if (na_oca_keyon_step == 0) {
        noteTimerStep = 3;
    } else {
        noteTimerStep = na_oca_framecounter - na_oca_vframe;
    }

    if (noteTimerStep < na_oca_note_step) {
        na_oca_note_step -= noteTimerStep;
    } else {
        nextNoteTimerStep = noteTimerStep - na_oca_note_step;
        na_oca_note_step = 0;
    }

    if (na_oca_note_step == 0) {

        na_oca_note_step = na_oca_now_seq[na_oca_seq_step].length;

        if (na_oca_seq_step == 1) {
            na_oca_note_step++;
        }

        if (na_oca_note_step == 0) {
            na_oca_play--;
            if (na_oca_play != 0) {
                na_oca_seq_step = 0;
                na_oca_keyon_step = 0;
                na_oca_note_key = OCARINA_PITCH_NONE;
            } else {
                Nai_StopFx(NA_SE_OC_OCARINA);
            }
            return;
        } else {
            na_oca_note_step -= nextNoteTimerStep;
        }

        // Update volume
        if (na_oca_note_vol != na_oca_now_seq[na_oca_seq_step].volume) {
            na_oca_note_vol = na_oca_now_seq[na_oca_seq_step].volume;
            na_oca_seq_volume = na_oca_note_vol / 127.0f;
        }

        // Update vibrato
#if OOT_VERSION < PAL_1_0 || !PLATFORM_N64
        if (na_oca_note_vib != na_oca_now_seq[na_oca_seq_step].vibrato) {
            na_oca_note_vib = na_oca_now_seq[na_oca_seq_step].vibrato;
            // Sets vibrato to io port 6
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 6, na_oca_note_vib);
        }
#else
        na_oca_note_vib = na_oca_now_seq[na_oca_seq_step].vibrato;
        // Sets vibrato to io port 6
        AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 6, na_oca_note_vib);
#endif

        // Update bend
        if (na_oca_note_bend != na_oca_now_seq[na_oca_seq_step].bend) {
            na_oca_note_bend = na_oca_now_seq[na_oca_seq_step].bend;
            na_oca_seq_picth = Na_StickToBend(na_oca_note_bend);
        }

        // No changes in volume, vibrato, or bend between notes
        if ((na_oca_now_seq[na_oca_seq_step].volume == na_oca_now_seq[na_oca_seq_step - 1].volume &&
             (na_oca_now_seq[na_oca_seq_step].vibrato == na_oca_now_seq[na_oca_seq_step - 1].vibrato) &&
             (na_oca_now_seq[na_oca_seq_step].bend == na_oca_now_seq[na_oca_seq_step - 1].bend))) {
            na_oca_note_key = 0xFE;
        }

        if (na_oca_note_key != na_oca_now_seq[na_oca_seq_step].pitch) {
            u8 pitch = na_oca_now_seq[na_oca_seq_step].pitch;

            // As bFlat4 is exactly in the middle of notes B & A, a flag is
            // added to the pitch to resolve which button to map Bflat4 to
            if (pitch == OCARINA_PITCH_BFLAT4) {
                na_oca_note_key = pitch + na_oca_now_seq[na_oca_seq_step].bFlat4Flag;
            } else {
                na_oca_note_key = pitch;
            }

            if (na_oca_note_key != OCARINA_PITCH_NONE) {
                na_oca_keyon_step++;
                // Sets ocarina instrument Id to channelIndex io port 7, which is used
                // as an index in seq 0 to get the true instrument Id
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 7, na_oca_flag - 1);
                // Sets na_oca_note_key to channelIndex io port 5
                AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, SFX_CHANNEL_OCARINA, 5, na_oca_note_key & 0x3F);
                Nai_FxFlagEntry(NA_SE_OC_OCARINA, &_dummy_zero_f, 4, &na_oca_seq_picth,
                                     &na_oca_seq_volume, &_dummy_zero_s8);
            } else {
                Nai_StopFx(NA_SE_OC_OCARINA);
            }
        }
        na_oca_seq_step++;
    }
}

void Na_WriteOcarinaRecBuffer(u8 isRecordingComplete) {
    u16 i;
    u16 i2;
    u16 pad;
    u8 pitch;
    OcarinaNote* note;
    u8 j;
    u8 k;
    s32 t;
    OcarinaNote* recordedSong;

    if (na_oca_record == OCARINA_RECORD_SCARECROW_LONG) {
        recordedSong = na_oca_rec_buf_ptr;
    } else {
        /**
         * OCARINA_RECORD_SCARECROW_SPAWN
         *
         * The notes for scarecrows spawn song are first recorded into the ocarina memory
         * game address to act as a buffer. That way, if a new scarecrow spawn song is
         * rejected, the previous scarecrow spawn song is not overwritten. If the scarecrow
         * spawn song is accepted, then the notes are copied over to the scarecrow spawn
         * song address
         */
        recordedSong = na_oca_rec_buf_ptr_temp;
    }

    recordedSong[na_oca_rec_point].pitch = na_oca_rec_key;
    recordedSong[na_oca_rec_point].length = na_oca_framecounter - na_oca_rec_step;
    recordedSong[na_oca_rec_point].volume = na_oca_rec_vol;
    recordedSong[na_oca_rec_point].vibrato = na_oca_rec_vib;
    recordedSong[na_oca_rec_point].bend = na_oca_rec_bend;
    recordedSong[na_oca_rec_point].bFlat4Flag = na_oca_rec_btn & 0xC0;

    na_oca_rec_key = na_oca_key_now;
    na_oca_rec_vol = na_oca_s8_volume;
    na_oca_rec_vib = na_oca_vib_now;
    na_oca_rec_bend = na_oca_bend_now;
    na_oca_rec_btn = na_oca_key_btn;

    na_oca_rec_point++;

    if ((na_oca_rec_point != (ARRAY_COUNT(na_oca_rec_buf) - 1)) && !isRecordingComplete) {
        // Continue recording
        return;
    }

    // Recording is complete

    i = na_oca_rec_point;
    pitch = OCARINA_PITCH_NONE;
    while (i != 0 && pitch == OCARINA_PITCH_NONE) {
        i--;
        pitch = recordedSong[i].pitch;
    }

    if (1) {}

    if (na_oca_rec_point != (i + 1)) {
        na_oca_rec_point = i + 2;
        recordedSong[na_oca_rec_point - 1].length = 0;
    }

    recordedSong[na_oca_rec_point].length = 0;

    if (na_oca_record == OCARINA_RECORD_SCARECROW_SPAWN) {
        if (na_oca_chk_note_count >= 8) {
            for (i = 0; i < na_oca_rec_point; i++) {
                recordedSong[i] = recordedSong[i + 1];
            }

            // Copies Notes from buffer into scarecrows spawn buttons to be tested for acceptance or rejection
            Na_MakeRec8Score(OCARINA_SONG_MEMORY_GAME);

            // Loop through each of the songs
            for (i = 0; i < OCARINA_SONG_SCARECROW_SPAWN; i++) {
                // Loops through all possible starting indices
                for (j = 0; j < 9 - na_oca_note[i].numButtons; j++) {
                    // Loops through the notes of song i
                    for (k = 0; k < na_oca_note[i].numButtons && k + j < 8 &&
                                na_oca_note[i].buttonsIndex[k] ==
                                    na_oca_note[OCARINA_SONG_SCARECROW_SPAWN].buttonsIndex[k + j];
                         k++) {
                        ;
                    }

                    // This conditional is true if the recorded song contains a reserved song
                    if (k == na_oca_note[i].numButtons) {
                        na_oca_record = OCARINA_RECORD_REJECTED;
                        na_oca_patern[OCARINA_SONG_SCARECROW_SPAWN][1].volume = 0xFF;
                        return;
                    }
                }
            }

            // Counts how many times a note is repeated
            i = 1;
            while (i < 8) {
                if (na_oca_note[OCARINA_SONG_SCARECROW_SPAWN].buttonsIndex[0] !=
                    na_oca_note[OCARINA_SONG_SCARECROW_SPAWN].buttonsIndex[i]) {
                    i = 9; // break
                } else {
                    i++;
                }
            }

            // This condition is true if all 8 notes are the same pitch
            if (i == 8) {
                na_oca_record = OCARINA_RECORD_REJECTED;
                na_oca_patern[OCARINA_SONG_SCARECROW_SPAWN][1].volume = 0xFF;
                return;
            }

            // The scarecrow spawn song is accepted and copied from the buffer to the scarecrow spawn notes
            for (i = 0; i < na_oca_rec_point; i++) {
                na_oca_patern[OCARINA_SONG_SCARECROW_SPAWN][i] = na_oca_patern[OCARINA_SONG_MEMORY_GAME][i];
            }

            na_oca_key_check = false;
        } else {
            na_oca_patern[OCARINA_SONG_SCARECROW_SPAWN][1].volume = 0xFF;
        }
    }

    na_oca_record = OCARINA_RECORD_OFF;
}

/**
 * recordingState = OCARINA_RECORD_OFF, end
 * recordingState = OCARINA_RECORD_SCARECROW_LONG, start long scarecrows song
 * recordingState = OCARINA_RECORD_SCARECROW_SPAWN, start spawn scarecrows song
 */
void Na_SetOcarinaRecModeFlag(u8 recordingState) {
    if ((u32)recordingState == na_oca_record) {
        return;
    }

    if (recordingState != OCARINA_RECORD_OFF) {
        na_oca_rec_step = na_oca_framecounter;
        na_oca_rec_key = OCARINA_PITCH_NONE;
        na_oca_rec_vol = 0x57;
        na_oca_rec_vib = 0;
        na_oca_rec_bend = 0;
        na_oca_rec_btn = 0;
        na_oca_rec_point = 0;
        na_oca_key_check = true;
        na_oca_chk_note_count = 0;
        na_oca_line0_buf = na_oca_rec_buf[1];
    } else {
        if (na_oca_rec_point == 0) {
            na_oca_rec_buf[1] = na_oca_line0_buf;
        } else {
            if (na_oca_record == OCARINA_RECORD_SCARECROW_SPAWN) {
                na_oca_chk_note_count = 1;
            }

            Na_WriteOcarinaRecBuffer(true);
        }

        na_oca_key_check = false;
        na_oca_chk_note_count = 0;
    }

    na_oca_record = recordingState;
}

void Na_SetOcarinaRecInfoWork(void) {
    na_oca_rec_info.state = na_oca_record;
    na_oca_rec_info.pos = na_oca_chk_note_count;
    if (na_oca_record == OCARINA_RECORD_REJECTED) {
        na_oca_record = OCARINA_RECORD_OFF;
    }
}

void Na_SetOcarinaInfoWork(void) {
    na_oca_play_info.buttonIndex = na_oca_key_btn & 0x3F;
    na_oca_play_info.state = Na_GetOcarinaCheckStatus();
    na_oca_play_info.pos = na_oca_chk_note_count;
}

void Na_SetOcarinaSeqInfoWork(void) {
    if ((na_oca_note_key & 0x3F) <= OCARINA_PITCH_EFLAT5) {
        na_oca_seq_info.buttonIndex = Na_GetNoteKeyToButton(na_oca_note_key);
    }

    na_oca_seq_info.state = na_oca_play;

    if (na_oca_now_seq != na_oca_rec_buf) {
        na_oca_seq_info.pos = na_oca_keyon_step;
    } else if (na_oca_keyon_step == 0) {
        na_oca_seq_info.pos = 0;
    } else {
        na_oca_seq_info.pos = ((na_oca_keyon_step - 1) % 8) + 1;
    }
}

OcarinaStaff* Na_GetOcarinaRecInfoWork(void) {
    return &na_oca_rec_info;
}

OcarinaStaff* Na_GetOcarinaInfoWork(void) {
    if (na_oca_play_info.state < 0xFE) {
        na_oca_check = 0;
    }

    return &na_oca_play_info;
}

OcarinaStaff* Na_GetOcarinaSeqInfoWork(void) {
    return &na_oca_seq_info;
}

void Na_RecordOcarina(void) {
    s32 noteChanged;

    if ((na_oca_record != OCARINA_RECORD_OFF) && ((na_oca_framecounter - na_oca_rec_step) >= 3)) {
        noteChanged = false;
        if (na_oca_rec_key != na_oca_key_now) {
            if (na_oca_key_now != OCARINA_PITCH_NONE) {
                na_oca_rec_info.buttonIndex = na_oca_key_btn & 0x3F;
                na_oca_chk_note_count++;
            } else if ((na_oca_record == OCARINA_RECORD_SCARECROW_SPAWN) && (na_oca_chk_note_count == 8)) {
                Na_WriteOcarinaRecBuffer(true);
                return;
            }

            if (na_oca_chk_note_count > 8) {
                if (na_oca_record == OCARINA_RECORD_SCARECROW_SPAWN) {
                    // notes played are over 8 and in recording mode.
                    Na_WriteOcarinaRecBuffer(true);
                    return;
                }
                na_oca_chk_note_count = 1;
            }

            noteChanged = true;
        } else if (na_oca_rec_vol != na_oca_s8_volume) {
            noteChanged = true;
        } else if (na_oca_rec_vib != na_oca_vib_now) {
#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
            if (na_oca_record != OCARINA_RECORD_SCARECROW_SPAWN) {
                noteChanged = true;
            }
#else
            noteChanged = true;
#endif
        } else if (na_oca_rec_bend != na_oca_bend_now) {
#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
            if (na_oca_record != OCARINA_RECORD_SCARECROW_SPAWN) {
                noteChanged = true;
            }
#else
            noteChanged = true;
#endif
        }

        if (noteChanged) {
            Na_WriteOcarinaRecBuffer(false);
            na_oca_rec_step = na_oca_framecounter;
        }
    }
}

void Na_InitOcarinaGameWork(u8 minigameRound) {
    u8 i;

    if (minigameRound > 2) {
        minigameRound = 2;
    }

    na_oca_game_count = 0;
    na_oca_game_max = na_oca_game_level_max[minigameRound];

    for (i = 0; i < 3; i++) {
        Na_MakeOcarinaGameData();
    }
}

s32 Na_MakeOcarinaGameData(void) {
    u32 randomButtonIndex;
    u8 randomPitch;

    if (na_oca_game_count == na_oca_game_max) {
        return 1;
    }

    randomButtonIndex = Nap_GetRandom();
    randomPitch = na_oca_btn_key[randomButtonIndex % 5];

    if (na_oca_patern[OCARINA_SONG_MEMORY_GAME][na_oca_game_count - 1].pitch == randomPitch) {
        randomPitch = na_oca_btn_key[(randomButtonIndex + 1) % 5];
    }

    na_oca_patern[OCARINA_SONG_MEMORY_GAME][na_oca_game_count].pitch = randomPitch;
    na_oca_patern[OCARINA_SONG_MEMORY_GAME][na_oca_game_count].length = FRAMERATE_CONST(45, 38);
    na_oca_patern[OCARINA_SONG_MEMORY_GAME][na_oca_game_count].volume = 0x50;
    na_oca_patern[OCARINA_SONG_MEMORY_GAME][na_oca_game_count].vibrato = 0;
    na_oca_patern[OCARINA_SONG_MEMORY_GAME][na_oca_game_count].bend = 0;

    na_oca_game_count++;

    na_oca_patern[OCARINA_SONG_MEMORY_GAME][na_oca_game_count].pitch = OCARINA_PITCH_NONE;
    na_oca_patern[OCARINA_SONG_MEMORY_GAME][na_oca_game_count].length = 0;
    na_oca_patern[OCARINA_SONG_MEMORY_GAME][na_oca_game_count + 1].pitch = OCARINA_PITCH_NONE;
    na_oca_patern[OCARINA_SONG_MEMORY_GAME][na_oca_game_count + 1].length = 0;
    if (1) {}
    return 0;
}

void Na_CheckOcarinaMode(void) {
    na_oca_framecounter = AG.totalTaskCount;
    if (na_oca_flag != OCARINA_INSTRUMENT_OFF) {
        if (na_oca_key_check == true) {
            Na_GetZeldaController();
        }

        if ((na_oca_play == 0) && (na_oca_key_check == true)) {
            Na_CheckOcarinaControl(false);
        }

        if (na_oca_check != 0) {
            if (na_oca_check & 0x4000) {
                Na_CheckOcarinaInputFree();
            } else {
                Na_CheckOcarinaInputLearn();
            }
        }

        Na_PlayOcarinaSeq();
        na_oca_vframe = na_oca_framecounter;

        if (na_oca_play == 0) {
            Na_RecordOcarina();
        }

        if ((na_oca_check != 0) && (na_oca_key_old != na_oca_key_now)) {
            na_oca_key_wait_count = 1; // Drops ocarina input for 1 frame
        }

        na_oca_key_old = na_oca_key_now;
    }

    Na_SetOcarinaInfoWork();
    Na_SetOcarinaSeqInfoWork();
    Na_SetOcarinaRecInfoWork();
}

void Na_AutoPlayUserOcarinaMelody(void) {
    static u8 phase = 0;
    static u8 inst = OCARINA_INSTRUMENT_DEFAULT;
    static u16 wait = 1200;

    switch (phase) {
        case 0:
            if (wait-- == 0) {
                if (inst < OCARINA_INSTRUMENT_MAX) {
                    // set next ocarina instrument and restart
                    phase++;
                } else {
                    // finished
                    phase = 3;
                    Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_OFF);
                }
                wait = 1200;
            }
            break;
        case 1:
            Nai_FxSetLockFlag(0);
            Na_SetOcarinaModeFlag(inst);
            Na_SetOcarinaSeq(OCARINA_SONG_SCARECROW_LONG + 1, 1);
            inst++;
            phase++;
            break;
        case 2:
            if (Na_GetOcarinaSeqInfoWork()->state == 0) {
                phase = 0;
            }
            break;
    }
}

void Na_InitOcarinaWork(void) {
    na_oca_play_info.buttonIndex = OCARINA_BTN_INVALID;
    na_oca_play_info.state = 0xFF;
    na_oca_play_info.pos = 0;
    na_oca_seq_info.buttonIndex = OCARINA_BTN_INVALID;
    na_oca_seq_info.state = 0;
    na_oca_seq_info.pos = 0;
    na_oca_rec_info.buttonIndex = OCARINA_BTN_INVALID;
    na_oca_rec_info.state = OCARINA_RECORD_REJECTED;
    na_oca_rec_info.pos = 0;
    na_oca_key_wait_count = 0;
}

#if DEBUG_FEATURES
#include "debug.inc.c"
#else
void Na_AudioPrint(GfxPrint* printer) {
}

void Nad_GameScrollPrint(const char* str, u16 num) {
}
#endif

void Na_CheckLevelMuteFlag(void);
void Na_CheckFanfareFlag(void);

/**
 * This is Nas_AudioInput for the graph thread
 */
void Nas_AudioInput(void) {
    if (Nai_SpecBusyCheck() == 0) {
#if DEBUG_FEATURES
        sAudioUpdateTaskStart = AG.totalTaskCount;
        sAudioUpdateStartTime = osGetTime();
#endif

        Na_CheckOcarinaMode();
        Na_CheckMovePitch(&na_river_pitch);
        Na_CheckMovePitch(&na_waterfall_pitch);
        Na_CheckLevelMuteFlag();
        Na_CheckContinueBgm();
        Na_CheckFanfareFlag();
        if (SndSpecNum == 7) {
            Na_CheckSariaMelodyTag();
        }
        Nai_FxBufFlagCheck();
        Nai_SeqBufFlagCheck();
        Nai_FxPlayCheck();
        Nai_SeqGameFrameWork();

#if DEBUG_FEATURES
        AudioDebug_SetInput();
        AudioDebug_ProcessInput();
#endif

        Nap_SendStart();

#if DEBUG_FEATURES
        sAudioUpdateTaskEnd = AG.totalTaskCount;
        sAudioUpdateEndTime = osGetTime();
#endif
    }
}

void Na_EntryMesVoice(UNK_TYPE arg0) {
}

void Na_SetVolumeBallnce(UNK_TYPE arg0, UNK_TYPE arg1) {
}

void Na_MusicStart(s8 seqId) {
    AUDIOCMD_GLOBAL_INIT_SEQPLAYER(SEQ_PLAYER_BGM_MAIN, (u8)seqId, 1);
}

f32 Naa_VolumeSet(u8 bankId, u8 entryIdx) {
    SfxBankEntry* bankEntry = &EntrySeStatus[bankId][entryIdx];
    f32 minDist;
    f32 baseDist;
    f32 ret;

    if (bankEntry->sfxParams & SFX_FLAG_13) {
        return 1.0f;
    }

    if (bankEntry->dist > 10000.0f) {
        ret = 0.0f;
    } else {
        switch ((bankEntry->sfxParams & SFX_PARAM_01_MASK) >> SFX_PARAM_01_SHIFT) {
            case 1:
                baseDist = 10000.0f / 15.0f;
                break;
            case 2:
                baseDist = 10000.0f / 10.5f;
                break;
            case 3:
                baseDist = 10000.0f / 2.6f;
                break;
            default:
                baseDist = 10000.0f / 20.0f;
                break;
        }

        minDist = baseDist / 5.0f;

        // Volume grows as inverse square of distance. Linearly approximate
        // the inverse part, then square.
        if (bankEntry->dist < minDist) {
            ret = 1.0f;
        } else if (bankEntry->dist < baseDist) {
            ret = ((((baseDist - minDist) - (bankEntry->dist - minDist)) / (baseDist - minDist)) * 0.19f) + 0.81f;
        } else {
            ret = (1.0f - ((bankEntry->dist - baseDist) / (10000.0f - baseDist))) * 0.81f;
        }
        ret = SQ(ret);
    }

    return ret;
}

s8 Naa_FxmixSet(u8 bankId, u8 entryIdx, u8 channelIdx) {
    s8 distAdd = 0;
    s32 scriptAdd = 0;
    SfxBankEntry* entry = &EntrySeStatus[bankId][entryIdx];
    s32 reverb;

    if (!(entry->sfxParams & SFX_FLAG_12)) {
        if (entry->dist < 2500.0f) {
            distAdd = *entry->posZ > 0.0f ? (entry->dist / 2500.0f) * 70.0f : (entry->dist / 2500.0f) * 91.0f;
        } else {
            distAdd = 70;
        }
    }

    if (IS_SEQUENCE_CHANNEL_VALID(AG.seqPlayers[SEQ_PLAYER_SFX].channels[channelIdx])) {
        scriptAdd = AG.seqPlayers[SEQ_PLAYER_SFX].channels[channelIdx]->seqScriptIO[1];
        if (AG.seqPlayers[SEQ_PLAYER_SFX].channels[channelIdx]->seqScriptIO[1] <= SEQ_IO_VAL_NONE) {
            scriptAdd = 0;
        }
    }

    reverb = *entry->reverbAdd + distAdd + scriptAdd;
    if ((bankId != BANK_OCARINA) || !((entry->sfxId & 0x1FF) < 2)) {
        reverb += enviroment_fx + code_fx_ofs + default_fx;
    }

    if (reverb > 0x7F) {
        reverb = 0x7F;
    }

    return reverb;
}

s8 Naa_PanSet(f32 x, f32 z, u8 token) {
    f32 absX;
    f32 absZ;
    f32 pan;

    if (x < 0) {
        absX = -x;
    } else {
        absX = x;
    }
    if (z < 0) {
        absZ = -z;
    } else {
        absZ = z;
    }

    if (absX > 8000.0f) {
        absX = 8000.0f;
    }

    if (absZ > 8000.0f) {
        absZ = 8000.0f;
    }

    if ((x == 0.0f) && (z == 0.0f)) {
        pan = 0.5f;
    } else if (absZ <= absX) {
        pan = (16000.0f - absX) / (3.3f * (16000.0f - absZ));
        if (x >= 0.0f) {
            pan = 1.0f - pan;
        }
    } else {
        pan = (x / (5.0769234f * absZ)) + 0.5f; // about 66 / 13
    }

    if (absZ < 50.0f) {
        if (absX < 50.0f) {
            pan = ((pan - 0.5f) * SQ(absX / 50.0f)) + 0.5f;
        }
    }
    return (s8)((pan * 127.0f) + 0.5f);
}

f32 Naa_TuneSet(u8 bankId, u8 entryIdx) {
    s32 phi_v0 = 0;
    SfxBankEntry* entry = &EntrySeStatus[bankId][entryIdx];
    f32 unk1C;
    f32 freq = 1.0f;

    if (entry->sfxParams & SFX_FLAG_14) {
        freq = 1.0f - ((AG.audioRandom & 0xF) / 192.0f);
    }

    switch (bankId) {
        case BANK_VOICE:
#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
            if (((entry->sfxId & 0xFF) < 0x40) && (na_forth_vibrate_mode != 0)) {
                phi_v0 = true;
            } else if (((entry->sfxId & 0xFF) >= 0x40) && (na_forth_vibrate_mode2 != 0)) {
                phi_v0 = true;
            }
            break;
#endif
        case BANK_PLAYER:
        case BANK_ITEM:
            if (na_forth_vibrate_mode != 0) {
                phi_v0 = 1;
            }
            break;
        case BANK_ENV:
        case BANK_ENEMY:
            if (na_forth_vibrate_mode2 != 0) {
                phi_v0 = 1;
            }
            break;
        case BANK_SYSTEM:
        case BANK_OCARINA:
            break;
    }

    if (phi_v0 == 1) {
        if (!(entry->sfxParams & SFX_FLAG_11)) {
            freq *= (1.0293 - ((AG.audioRandom & 0xF) / 144.0f));
        }
    }

    unk1C = entry->dist;
    if (!(entry->sfxParams & SFX_FLAG_13)) {
        if (!(entry->sfxParams & SFX_FLAG_15)) {
            if (unk1C >= 10000.0f) {
                freq += 0.2f;
            } else {
                freq += (0.2f * (unk1C / 10000.0f));
            }
        }
    }

    if ((entry->sfxParams & SFX_PARAM_67_MASK) != (0 << SFX_PARAM_67_SHIFT)) {
        freq += (entry->unk_2F / 192.0f);
    }

    return freq;
}

u8 Naa_FrtSet(f32 behindScreenZ, SfxBankEntry* arg1, s8 arg2) {
    s8 phi_v0;
    u8 phi_v1;
    f32 phi_f0;
    f32 phi_f12;

    if (*arg1->posZ < behindScreenZ) {
        phi_v0 = arg2 < 65 ? arg2 : 0x7F - arg2;

        if (phi_v0 < 30) {
            phi_v1 = 0;
        } else {
            phi_v1 = (((phi_v0 & 0xFFFF) * 10) - 300) / 34;
            if (phi_v1 != 0) {
                phi_v1 = 0x10 - phi_v1;
            }
        }
    } else {
        phi_v1 = 0;
    }

    if (phi_v1 == 0) {
        if (arg1->sfxParams & SFX_FLAG_9) {
            phi_v1 = 0xF;
        }
    }

    switch ((arg1->sfxParams & SFX_PARAM_01_MASK) >> SFX_PARAM_01_SHIFT) {
        case 1:
            phi_f0 = 12.0f;
            break;
        case 2:
            phi_f0 = 9.0f;
            break;
        case 3:
            phi_f0 = 6.0f;
            break;
        default:
            phi_f0 = 15.0f;
            break;
    }

    phi_f12 = CLAMP_MAX(arg1->dist, 10000.0f / 5.2f);

    return (phi_v1 * 0x10) + (u8)((phi_f0 * phi_f12) / (10000.0f / 5.2f));
}

s8 Naa_CombSet(f32 posY, u16 sfxParams) {
    s8 combFilterGain = 0;

    if (posY >= 0.0f) {
        if (posY > 625.0f) {
            combFilterGain = 127;
        } else {
            combFilterGain = (posY / 625.0f) * 126.0f;
        }
    }
    return combFilterGain | 1;
}

void Naa_PlaySeParaSet(u8 bankId, u8 entryIdx, u8 channelIndex) {
    f32 vol = 1.0f;
    s8 volS8;
    s8 reverb = 0;
    f32 freqScale = 1.0f;
    s8 pan = 0x40;
    u8 stereoBits = 0;
    u8 filter = 0;
    s8 combFilterGain = 0;
    f32 behindScreenZ;
    u8 baseFilter = 0;
    SfxBankEntry* entry = &EntrySeStatus[bankId][entryIdx];
#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
    s32 pad;
#endif

    switch (bankId) {
        case BANK_PLAYER:
        case BANK_ITEM:
        case BANK_ENV:
        case BANK_ENEMY:
        case BANK_VOICE:
            if (na_sound_output == SOUNDMODE_SURROUND) {
                combFilterGain = Naa_CombSet(*entry->posY, entry->sfxParams);
            }
            FALLTHROUGH;
        case BANK_OCARINA:
            entry->dist = sqrtf(entry->dist * SFX_DIST_SCALING);

            vol = Naa_VolumeSet(bankId, entryIdx) * *entry->vol;
            reverb = Naa_FxmixSet(bankId, entryIdx, channelIndex);
            pan = Naa_PanSet(*entry->posX, *entry->posZ, entry->token);
            freqScale = Naa_TuneSet(bankId, entryIdx) * *entry->freqScale;

            if (na_sound_output == SOUNDMODE_SURROUND) {
                behindScreenZ = na_behind_posz[(entry->sfxParams & SFX_FLAG_10) >> SFX_FLAG_10_SHIFT];
                if (!(entry->sfxParams & SFX_FLAG_11)) {
                    if (*entry->posZ < behindScreenZ) {
                        stereoBits = 0x10;
                    }

                    if ((track_update[channelIndex].stereoBits ^ stereoBits) & 0x10) {
                        if (pan < 0x40) {
                            stereoBits = track_update[channelIndex].stereoBits ^ 0x14;
                        } else {
                            stereoBits = track_update[channelIndex].stereoBits ^ 0x18;
                        }
                    } else {
                        stereoBits = track_update[channelIndex].stereoBits;
                    }
                }
            }
            if (na_forth_lowpass_mode_0 != 0) {
#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
                if ((bankId == BANK_PLAYER) || (bankId == BANK_ITEM) ||
                    ((bankId == BANK_VOICE) && ((entry->sfxId & 0xFF) < 0x40)))
#else
                if ((bankId == BANK_ITEM) || (bankId == BANK_PLAYER) || (bankId == BANK_VOICE))
#endif
                {
                    baseFilter = na_forth_lowpass_mode_0;
                }
            }

            if ((baseFilter | na_forth_lowpass_mode_1) != 0) {
                filter = (baseFilter | na_forth_lowpass_mode_1);
            } else if ((na_sound_output == SOUNDMODE_SURROUND) && !(entry->sfxParams & SFX_FLAG_13)) {
                filter = Naa_FrtSet(behindScreenZ, entry, pan);
            }
            break;
        case BANK_SYSTEM:
            break;
    }

    if (track_update[channelIndex].vol != vol) {
        volS8 = (u8)(vol * 127.0f);
        track_update[channelIndex].vol = vol;
    } else {
        volS8 = -1;
    }

    // CHAN_UPD_SCRIPT_IO (slot 2, sets volume)
    AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, channelIndex, 2, volS8);

    if (reverb != track_update[channelIndex].reverb) {
        AUDIOCMD_CHANNEL_SET_REVERB_VOLUME(SEQ_PLAYER_SFX, channelIndex, reverb);
        track_update[channelIndex].reverb = reverb;
    }

    if (freqScale != track_update[channelIndex].freqScale) {
        AUDIOCMD_CHANNEL_SET_FREQ_SCALE(SEQ_PLAYER_SFX, channelIndex, freqScale);
        track_update[channelIndex].freqScale = freqScale;
    }

    //! @bug: comparing a `u8` to an `s8`. if the most significant bit is set,
    //! it'll always pass because the s8 value will be <0 and the u8 value is always >=0
    if (stereoBits != track_update[channelIndex].stereoBits) {
        AUDIOCMD_CHANNEL_SET_STEREO(SEQ_PLAYER_SFX, channelIndex, stereoBits | 0x10);
        track_update[channelIndex].stereoBits = stereoBits;
    }

    if (filter != track_update[channelIndex].filter) {
        AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, channelIndex, 3, filter);
        track_update[channelIndex].filter = filter;
    }

    if (combFilterGain != track_update[channelIndex].combFilterGain) {
        AUDIOCMD_CHANNEL_SET_COMB_FILTER_SIZE(SEQ_PLAYER_SFX, channelIndex, 0x10);
        AUDIOCMD_CHANNEL_SET_COMB_FILTER_GAIN(SEQ_PLAYER_SFX, channelIndex, ((u16)(combFilterGain) << 8) + 0xFF);
        track_update[channelIndex].combFilterGain = combFilterGain;
    }

    if (pan != track_update[channelIndex].pan) {
        AUDIOCMD_CHANNEL_SET_PAN(SEQ_PLAYER_SFX, channelIndex, pan);
        track_update[channelIndex].pan = pan;
    }
}

void Naa_InitApliWork(void) {
    u8 i;
    SfxPlayerState* state;

    for (i = 0; i < 16; i++) {
        state = &track_update[i];
        state->vol = 1.0f;
        state->freqScale = 1.0f;
        state->reverb = 0;
        state->pan = 0x40;
        state->stereoBits = 0;
        state->filter = 0xFF;
        state->combFilterGain = 0xFF;
    }

    track_update[SFX_CHANNEL_OCARINA].combFilterGain = 0;
    na_seq_switch_flag = 0;
    code_fx_ofs = 0;
}

void Na_StartDemoSe(u8 csEffectType) {
    if (se_handle_lock_flag[0] != 1) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_SUB, 0, 0, NA_BGM_CUTSCENE_EFFECTS);
        SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_BGM_SUB, 0, 0, csEffectType);
    }
}

f32 Na_SetLinkSpeedPara(f32 arg0) {
    f32 ret = 1.0f;

    if (arg0 > 6.0f) {
        na_link_walk_volume = 1.0f;
        na_link_walk_pitch = 1.1f;
    } else {
        ret = arg0 / 6.0f;
        na_link_walk_volume = (ret * 0.22500002f) + 0.775f;
        na_link_walk_pitch = (ret * 0.2f) + 0.9f;
    }
    return ret;
}

void Na_StartLinkWalkSe(Vec3f* pos, u16 sfxId, f32 arg2) {
    f32 sp24;
    f32 phi_f0;
    u8 phi_v0;
    u16 sfxId2;

#if DEBUG_FEATURES
    D_80131C8C = arg2;
#endif

    sp24 = Na_SetLinkSpeedPara(arg2);
    Nai_FxFlagEntry(sfxId, pos, 4, &na_link_walk_pitch, &na_link_walk_volume, &_dummy_zero_s8);

    if ((sfxId & 0xF0) == 0xB0) {
        phi_f0 = 0.3f;
        phi_v0 = 1;
        sp24 = 1.0f;
    } else {
        phi_f0 = 1.1f;
        phi_v0 = AG.audioRandom % 2;
    }

    if ((phi_f0 < arg2) && (phi_v0 != 0)) {
        if ((sfxId & 0x80) != 0) {
            sfxId2 = NA_SE_PL_METALEFFECT_ADULT;
        } else {
            sfxId2 = NA_SE_PL_METALEFFECT_KID;
        }
        na_link_walk_volume2 = (sp24 * 0.7) + 0.3;
        Nai_FxFlagEntry(sfxId2, pos, 4, &na_link_walk_pitch, &na_link_walk_volume2, &_dummy_zero_s8);
    }
}

void Na_SetLinkSliplevelSe(Vec3f* pos, u16 sfxId, f32 arg2) {
    Na_SetLinkSpeedPara(arg2);
    Nai_FxFlagEntry(sfxId, pos, 4, &na_link_walk_pitch, &na_link_walk_volume, &_dummy_zero_s8);
}

void Na_SetBlackLinkVoice(Vec3f* pos, u16 sfxId) {
    Nai_FxFlagEntry(sfxId, pos, 4, &na_black_link_pitch, &_dummy_one, &na_black_link_fxmix);
}
void Na_SetRandomSeFlag(Vec3f* pos, u16 baseSfxId, u8 randLim) {
    u8 offset = Nap_GetRandom() % randLim;

    Nai_FxFlagEntry(baseSfxId + offset, pos, 4, &_dummy_one, &_dummy_one,
                         &_dummy_zero_s8);
}

void Na_SetSwordChargeSe(Vec3f* pos, u8 level) {
    level &= 3;
    if (level != na_sword_charge_level) {
        na_sword_charge_pitch_now = na_sword_charge_pitch_table[level];
        switch (level) {
            case 1:
                Nai_FxFlagEntry(NA_SE_PL_SWORD_CHARGE, pos, 4, &na_sword_charge_pitch_now, &_dummy_one,
                                     &_dummy_zero_s8);
                break;
            case 2:
                Nai_FxFlagEntry(NA_SE_PL_SWORD_CHARGE, pos, 4, &na_sword_charge_pitch_now, &_dummy_one,
                                     &_dummy_zero_s8);
                break;
        }

        na_sword_charge_level = level;
    }

    if (level != 0) {
        Nai_FxFlagEntry(NA_SE_IT_SWORD_CHARGE - SFX_FLAG, pos, 4, &na_sword_charge_pitch_now, &_dummy_one,
                             &_dummy_zero_s8);
    }
}

void Na_SetMotorSe(Vec3f* pos, u16 sfxId, f32 arg2) {
    if (arg2 < 0.75f) {
        na_motor_pitch = ((arg2 / 0.75f) * 0.25f) + 0.5f;
    } else {
        na_motor_pitch = arg2;
    }

    if (na_motor_pitch > 0.5f) {
        Nai_FxFlagEntry(sfxId, pos, 4, &na_motor_pitch, &_dummy_one, &_dummy_zero_s8);
    }
}

void Na_SetAutoTriggerSe(Vec3f* pos, u16 sfxId, f32 arg2) {
    na_autotrig_intv_now--;
    if (na_autotrig_intv_now == 0) {
        Nai_FxFlagEntry(sfxId, pos, 4, &na_motor_pitch, &_dummy_one, &_dummy_zero_s8);

        if (arg2 > 2.0f) {
            arg2 = 2.0f;
        }
        na_autotrig_intv_now = (s8)((na_autotrig_intv_normal - na_autotrig_intv_min) * (1.0f - arg2)) + na_autotrig_intv_normal;
    }
}

void Na_InitAutoTriggerSe(s8 arg0, s8 arg1) {
    na_autotrig_intv_now = 1;
    na_autotrig_intv_min = arg1;
    na_autotrig_intv_normal = arg0;
}

void Na_StartFxmixSe(Vec3f* pos, u16 sfxId, s8 arg2) {
    na_set_fx_param = arg2;
    Nai_FxFlagEntry(sfxId, pos, 4, &_dummy_one, &_dummy_one, &na_set_fx_param);
}

void Na_StartVolumeSetSe(Vec3f* pos, u16 sfxId, f32 arg2) {
    na_set_volume_param = arg2;
    Nai_FxFlagEntry(sfxId, pos, 4, &_dummy_one, &na_set_volume_param, &_dummy_zero_s8);
}

void Na_SetFishingRealSe(f32 arg0) {
    Na_SetAutoTriggerSe(&_dummy_zero_f, NA_SE_IT_FISHING_REEL_SLOW - SFX_FLAG, arg0);
    Na_SetMotorSe(&_dummy_zero_f, 0, (0.15f * arg0) + 1.4f);
}

void Na_SetRiverSe(Vec3f* pos, f32 freqScale) {
    if (!Nai_FxFlagCheck(NA_SE_EV_RIVER_STREAM - SFX_FLAG)) {
        na_river_pitch.value = freqScale;
    } else if (freqScale != na_river_pitch.value) {
        na_river_pitch.target = freqScale;
        na_river_pitch.remainingFrames = 40;
        na_river_pitch.step = (na_river_pitch.target - na_river_pitch.value) / 40;
    }
    Nai_FxFlagEntry(NA_SE_EV_RIVER_STREAM - SFX_FLAG, pos, 4, &na_river_pitch.value,
                         &_dummy_one, &_dummy_zero_s8);
}

void Na_SetWaterfallSe(Vec3f* pos, f32 freqScale) {
    if (!Nai_FxFlagCheck(NA_SE_EV_WATER_WALL_BIG - SFX_FLAG)) {
        na_waterfall_pitch.value = freqScale;
    } else if (freqScale != na_waterfall_pitch.value) {
        na_waterfall_pitch.target = freqScale;
        na_waterfall_pitch.remainingFrames = 40;
        na_waterfall_pitch.step = (na_waterfall_pitch.target - na_waterfall_pitch.value) / 40;
    }
    Nai_FxFlagEntry(NA_SE_EV_WATER_WALL_BIG - SFX_FLAG, pos, 4, &na_waterfall_pitch.value,
                         &na_waterfall_pitch.value, &_dummy_zero_s8);
}

void Na_CheckMovePitch(FreqLerp* lerp) {
    if (lerp->remainingFrames != 0) {
        lerp->remainingFrames--;
        if (lerp->remainingFrames != 0) {
            lerp->value += lerp->step;
        } else {
            lerp->value = lerp->target;
        }
    }
}

void Na_SetLinkDownSound(void) {
    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_FANFARE, 0, 10);
    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_FANFARE, 0, 10);
}

void Na_ClearLinkDownSound(void) {
    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_FANFARE, 0x7F, 3);
    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_FANFARE, 0x7F, 3);
}

void Na_SetForceBgmVol(u8 targetVol, u8 volFadeTimer) {
    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_MAIN, targetVol, volFadeTimer);
}

/**
 * Incrementally increase volume of NA_BGM_GANON_TOWER for each new room during the climb of Ganon's Tower
 */
void Na_SetVolumeDistanceBgm(u8 ganonsTowerLevel) {
    u8 channelIndex;
    s8 panChannelWeight = 0; // Pan comes entirely from the SequenceLayer

    // Ganondorf's Lair
    if (ganonsTowerLevel == 0) {
        // Pan comes entirely from the SequenceChannel
        panChannelWeight = 0x7F;
    }

    for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
        AUDIOCMD_CHANNEL_SET_PAN_WEIGHT(SEQ_PLAYER_BGM_MAIN, (u32)channelIndex, panChannelWeight);
    }

    // Lowest room in Ganon's Tower (Entrance Room)
    if (ganonsTowerLevel == 7) {
        // Adds a delay to setting the volume in the first room
        na_dist_bgm_set = 2;
    } else {
        Na_SetTriggerMuteFlag(na_trigger_mute_vol_tbl[ganonsTowerLevel % ARRAY_COUNTU(na_trigger_mute_vol_tbl)]);
    }
}

/**
 * If a new volume is requested for ganon's tower, update the volume and
 * calculate a new low-pass filter cutoff and reverb based on the new volume
 */
s32 Na_SetTriggerMuteFlag(u8 targetVol) {
    u8 lowPassFilterCutoff;
    u16 reverb;
    u8 channelIndex;

    if (na_trigger_mute_vol_now != targetVol) {
        // Sets the volume
        Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_MAIN, targetVol, 2);

        // Sets the filter cutoff of the form (lowPassFilterCutoff << 4) | (highPassFilter & 0xF). highPassFilter is
        // always set to 0
        if (targetVol < 0x40) {
            // Only the first room
            lowPassFilterCutoff = 1 << 4;
        } else {
            // Higher volume leads to a higher cut-off frequency in the low-pass filtering
            lowPassFilterCutoff = (((targetVol - 0x40) >> 2) + 1) << 4;
        }
        // Set lowPassFilterCutoff to io port 4 from channel 15
        SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_BGM_MAIN, 15, 4, lowPassFilterCutoff);

        // Sets the reverb
        for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
            if (AG.seqPlayers[SEQ_PLAYER_BGM_MAIN].channels[channelIndex] != &AG.sequenceChannelNone) {
                // seqScriptIO[5] is set to 0x40 in channels 0, 1, and 4
                if ((u8)AG.seqPlayers[SEQ_PLAYER_BGM_MAIN].channels[channelIndex]->seqScriptIO[5] !=
                    (u8)SEQ_IO_VAL_NONE) {
                    // Higher volume leads to lower reverb
                    reverb = ((u16)AG.seqPlayers[SEQ_PLAYER_BGM_MAIN].channels[channelIndex]->seqScriptIO[5] -
                              targetVol) +
                             0x7F;
                    if (reverb > 0x7F) {
                        reverb = 0x7F;
                    }
                    AUDIOCMD_CHANNEL_SET_REVERB_VOLUME(SEQ_PLAYER_BGM_MAIN, (u32)channelIndex, (u8)reverb);
                }
            }
        }
        na_trigger_mute_vol_now = targetVol;
    }
    return -1;
}

/**
 * Responsible for lowering market bgm in Child Market Entrance and Child Market Back Alley
 * Only lowers volume for 1 frame, so must be called every frame to maintain lower volume
 */
void Na_SetLevelMuteFlag(u8 volume) {
    na_level_mute_vol_target = volume;
    na_level_mute_flag_next = true;
}

void Na_CheckLevelMuteFlag(void) {
    // Updates Main Bgm Volume (RiverSound of type RS_LOWER_MAIN_BGM_VOLUME)
    if (na_level_mute_flag_next == true) {
        if (na_level_mute_vol_now != na_level_mute_vol_target) {
            // lowers the volume for 1 frame
            Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_MAIN, na_level_mute_vol_target, 10);
            na_level_mute_vol_now = na_level_mute_vol_target;
            na_level_mute_flag_old = true;
        }
        na_level_mute_flag_next = false;
    } else if (na_level_mute_flag_old == true && na_pause_flag == 0) {
        // restores the volume every frame
        Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_MAIN, 0x7F, 10);
        na_level_mute_vol_now = 0x7F;
        na_level_mute_flag_old = false;
    }

    // Update Ganon's Tower Volume (RiverSound of type RS_GANON_TOWER_7)
    if (na_dist_bgm_set != 0) {
        na_dist_bgm_set--;
        if (na_dist_bgm_set == 0) {
            Na_SetTriggerMuteFlag(na_trigger_mute_vol_tbl[7]);
        }
    }
}

void Na_StartPicthUpSe(Vec3f* pos, s16 sfxId, u8* semitones) {
    Nai_FxFlagEntry(sfxId, pos, 4, &PITCHTABLE[semitones[na_picth_up_se_count] + 39],
                         &_dummy_one, &_dummy_zero_s8);

    if (na_picth_up_se_count < 15) {
        na_picth_up_se_count++;
    }
}

void Na_InitPicthUpSe(void) {
    na_picth_up_se_count = 0;
}

void Na_StartTransposeSe(Vec3f* pos, u16 sfxId, s8 semitone) {
    Nai_FxFlagEntry(sfxId, pos, 4, &PITCHTABLE[semitone + 39], &_dummy_one,
                         &_dummy_zero_s8);
}

void Na_StartInputPortSe(Vec3f* pos, u16 sfxId, u8 ioData) {
    u8 channelIndex = 0;
    u8 i;
    u8 bankId;

    bankId = SFX_BANK_SHIFT(sfxId);
    for (i = 0; i < bankId; i++) {
        channelIndex += _se_handle_sounds[se_scene_mode][i];
    }

    for (i = 0; i < _se_handle_sounds[se_scene_mode][bankId]; i++) {
        if ((play_next[bankId][i].entryIndex != 0xFF) &&
            (sfxId == EntrySeStatus[bankId][play_next[bankId][i].entryIndex].sfxId)) {
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_SFX, channelIndex, 6, ioData);
        }
        channelIndex++;
    }
    Nai_FxFlagEntry(sfxId, pos, 4, &_dummy_one, &_dummy_one, &_dummy_zero_s8);
}

void Na_SetSariaMelodyTag2(Vec3f* pos, f32 arg1) {
    f32 phi_f22;
    s8 pan;
    u8 channelIndex;

    if (saria_oca_pos == NULL) {
        saria_oca_pos = pos;
        saria_oca_lpos = arg1;
    } else if (pos != saria_oca_pos) {
        if (arg1 < saria_oca_lpos) {
            saria_oca_pos = pos;
            saria_oca_lpos = arg1;
        }
    } else {
        saria_oca_lpos = arg1;
    }

    if (saria_oca_pos->x > 100.0f) {
        pan = 0x7F;
    } else if (saria_oca_pos->x < -100.0f) {
        pan = 0;
    } else {
        pan = ((saria_oca_pos->x / 100.0f) * 64.0f) + 64.0f;
    }

    if (saria_oca_lpos > 400.0f) {
        phi_f22 = 0.1f;
    } else if (saria_oca_lpos < 120.0f) {
        phi_f22 = 1.0f;
    } else {
        phi_f22 = ((1.0f - ((saria_oca_lpos - 120.0f) / 280.0f)) * 0.9f) + 0.1f;
    }

    for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
        if (channelIndex != 9) {
            SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_MAIN, channelIndex, 2, (127.0f * phi_f22));
            AUDIOCMD_CHANNEL_SET_PAN(SEQ_PLAYER_BGM_MAIN, (u32)channelIndex, pan);
        }
    }
}

void Na_CheckSariaMelodyTag(void) {
    if (saria_oca_pos != NULL) {
        saria_oca_pos = NULL;
    }
}

void Na_ClearSariaMelodyTag2(Vec3f* pos) {
    if (saria_oca_pos == pos) {
        saria_oca_pos = NULL;
    }
}

/**
 * Turns on and off channels from both bgm players in a way that splits
 * equally between the two bgm channels. Split based on note priority
 */
void Na_SetBgmPartMute(s8 volSplit) {
    u8 volume;
    u8 notePriority;
    u16 channelBits;
    u8 bgmPlayers[2] = { SEQ_PLAYER_BGM_MAIN, SEQ_PLAYER_BGM_SUB };
    u8 channelIdx;
    u8 i;

    if ((Nai_GetPlayingSeqFlag(SEQ_PLAYER_FANFARE) == NA_BGM_DISABLED) &&
        (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_SUB) != NA_BGM_LONLON)) {
        for (i = 0; i < ARRAY_COUNT(bgmPlayers); i++) {
            if (i == 0) {
                // Main Bgm SeqPlayer
                volume = volSplit;
            } else {
                // Sub Bgm SeqPlayer
                volume = 0x7F - volSplit;
            }

            if (volume > 100) {
                notePriority = 11;
            } else if (volume < 20) {
                notePriority = 2;
            } else {
                notePriority = ((volume - 20) / 10) + 2;
            }

            channelBits = 0;
            for (channelIdx = 0; channelIdx < 16; channelIdx++) {
                if (notePriority > AG.seqPlayers[bgmPlayers[i]].channels[channelIdx]->notePriority) {
                    // If the note currently playing in the channel is a high enough priority,
                    // then keep the channel on by setting a channelBit
                    // If this condition fails, then the channel will be shut off
                    channelBits += (1 << channelIdx);
                }
            }

            SEQCMD_SET_CHANNEL_DISABLE_MASK(bgmPlayers[i], channelBits);
        }
    }
}

void Na_SetCrossBgmTag(Vec3f* pos, u16 seqId, u16 distMax) {
    f32 absY;
    f32 dist;
    u8 vol;
    f32 prevDist;

    if (na_cross_bgm_delay != 0) {
        na_cross_bgm_delay--;
        return;
    }

    dist = sqrtf(SQ(pos->z) + SQ(pos->x));
    if (saria_oca_pos == NULL) {
        saria_oca_pos = pos;
        Na_StartBgmWithPortWrite(SEQ_PLAYER_BGM_SUB, seqId, 0, 7, 2);
    } else {
        prevDist = sqrtf(SQ(saria_oca_pos->z) + SQ(saria_oca_pos->x));
        if (dist < prevDist) {
            saria_oca_pos = pos;
        } else {
            dist = prevDist;
        }
    }

    if (pos->y < 0.0f) {
        absY = -pos->y;
    } else {
        absY = pos->y;
    }

    if ((distMax / 15.0f) < absY) {
        vol = 0;
    } else if (dist < distMax) {
        vol = (1.0f - (dist / distMax)) * 127.0f;
    } else {
        vol = 0;
    }

    if (seqId != NA_BGM_GREAT_FAIRY) {
        Na_SetBgmPartMute(vol);
    }

    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_BGM_SUB, vol, 0);
    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F - vol, 0);
}

void Na_ClearCrossBgmTag(void) {
    saria_oca_pos = NULL;
}

void Na_StartMorinigBgm(u16 seqId) {
    Na_StartContinueBgm(seqId);
    // Writing a value of 1 to ioPort 0 will be used by
    // `NA_BGM_FIELD_LOGIC` to play `NA_BGM_FIELD_MORNING` first
    Na_StartBgmWithPortWrite(SEQ_PLAYER_BGM_MAIN, seqId, 0, 0, 1);
}

void Na_StartContinueBgm(u16 seqId) {
    u8 fadeInDuration = 0;
    u8 skipHarpIntro;

    if (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) != NA_BGM_WINDMILL) {
        if (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_SUB) == NA_BGM_LONLON) {
            Nai_StopSeqSet(SEQ_PLAYER_BGM_SUB, 0);
            AUDIOCMD_GLOBAL_STOP_AUDIOCMDS();
        }

#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
        if (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) != NA_BGM_DISABLED) {
            Nai_StopSeqSet(SEQ_PLAYER_BGM_MAIN, 0);
            AUDIOCMD_GLOBAL_STOP_AUDIOCMDS();
        }
#endif

        if ((na_bgm_info[na_bgm_continue_old_num] & SEQ_FLAG_RESUME_PREV) && (na_bgm_info[seqId & 0xFF & 0xFF] & SEQ_FLAG_RESUME)) {
            // Resume the sequence from the point where it left off last time it was played in the scene
            if ((na_bgm_continue_block & 0x3F) != 0) {
                fadeInDuration = 30;
            }

            // Write the sequence resumePoint to start from into ioPort 7
            Na_StartBgmWithPortWrite(SEQ_PLAYER_BGM_MAIN, seqId, fadeInDuration, 7, na_bgm_continue_block);

            na_bgm_continue_block = 0;
        } else {
            // Start the sequence from the beginning

            // Writes to ioPort 7. See `SEQ_FLAG_SKIP_HARP_INTRO` for writing a value of 1 to ioPort 7.
            skipHarpIntro = (na_bgm_info[seqId & 0xFF & 0xFF] & SEQ_FLAG_SKIP_HARP_INTRO) ? 1 : (u8)SEQ_IO_VAL_NONE;
            Na_StartBgmWithPortWrite(SEQ_PLAYER_BGM_MAIN, seqId, 0, 7, skipHarpIntro);

            if (!(na_bgm_info[seqId] & SEQ_FLAG_RESUME_PREV)) {
                // Reset the sequence resumePoint
                na_bgm_continue_block = SEQ_RESUME_POINT_NONE;
            }
        }
        na_bgm_continue_old_num = seqId & 0xFF;
    }
}

void Na_CheckContinueBgm(void) {
    u16 seqId = Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN);

    if ((seqId != NA_BGM_DISABLED) && (na_bgm_info[seqId & 0xFF & 0xFF] & SEQ_FLAG_RESUME)) {
        if (na_bgm_continue_block != SEQ_RESUME_POINT_NONE) {
            // Get the current point to resume from
            na_bgm_continue_block = AG.seqPlayers[SEQ_PLAYER_BGM_MAIN].seqScriptIO[3];
        } else {
            // Initialize the point to resume from to the start of the sequence.
            na_bgm_continue_block = 0;
        }
    }
}

void Na_StartWindmillBgm(void) {
    if (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) != NA_BGM_WINDMILL) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_WINDMILL);
    }
}

void Na_SetBgmPitch(f32 scaleTempoAndFreq, u8 duration) {
    if (scaleTempoAndFreq == 1.0f) {
        // Should instead use `SEQCMD_SETUP_RESET_TEMPO` to wait until the fanfare is finished
        SEQCMD_RESET_TEMPO(SEQ_PLAYER_BGM_MAIN, duration);
    } else {
        SEQCMD_SETUP_SCALE_TEMPO(SEQ_PLAYER_FANFARE, SEQ_PLAYER_BGM_MAIN, duration, scaleTempoAndFreq * 100.0f);
    }

    SEQCMD_SETUP_SET_SEQPLAYER_FREQ(SEQ_PLAYER_FANFARE, SEQ_PLAYER_BGM_MAIN, duration, scaleTempoAndFreq * 100.0f);
}

/**
 * Set the tempo for the timed minigame sequence to 210 bpm,
 * which is faster than the default tempo
 */
void Na_SetMiniGameBgmTempoUp(void) {
    if ((Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) == NA_BGM_TIMED_MINI_GAME) &&
        Nai_CheckSeqFlagBuffer2(SEQCMD_OP_PLAY_SEQUENCE << 28, SEQCMD_OP_MASK)) {
        SEQCMD_SET_TEMPO(SEQ_PLAYER_BGM_MAIN, 5, 210);
    }
}

void Na_StartDemoBgm(u16 seqId) {
    if (na_bgm_info[seqId & 0xFF & 0xFF] & SEQ_FLAG_FANFARE) {
        Na_StartFanfare(seqId);
    } else if (na_bgm_info[seqId & 0xFF & 0xFF] & SEQ_FLAG_FANFARE_GANON) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_FANFARE, 0, 0, seqId);
    } else {
        Na_StartBgmWithPortWrite(SEQ_PLAYER_BGM_MAIN, seqId, 0, 7, SEQ_IO_VAL_NONE);
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
    }
}

void Na_StopDemoBgm(u16 seqId) {
    if (na_bgm_info[seqId & 0xFF & 0xFF] & SEQ_FLAG_FANFARE) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
    } else if (na_bgm_info[seqId & 0xFF & 0xFF] & SEQ_FLAG_FANFARE_GANON) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
    } else {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
    }
}

s32 Na_CheckPlayingBgm(u16 seqId) {
    u8 seqPlayerIndex = SEQ_PLAYER_BGM_MAIN;

    if (na_bgm_info[seqId & 0xFF & 0xFF] & SEQ_FLAG_FANFARE) {
        seqPlayerIndex = SEQ_PLAYER_FANFARE;
    } else if (na_bgm_info[seqId & 0xFF & 0xFF] & SEQ_FLAG_FANFARE_GANON) {
        seqPlayerIndex = SEQ_PLAYER_FANFARE;
    }

    if ((seqId & 0xFF) == (Nai_GetPlayingSeqFlag(seqPlayerIndex) & 0xFF)) {
        return true;
    } else {
        return false;
    }
}

/**
 * Plays a sequence on the main bgm player, but stores the previous sequence to return to later
 * Designed for the mini-boss sequence, but also used by mini-game 2 sequence
 */
void Na_StartMiddleBossBgm(u16 seqId) {
    u16 curSeqId = Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN);

#if !DEBUG_FEATURES
    if (1) {}
#endif

    if ((curSeqId & 0xFF) != NA_BGM_GANON_TOWER && (curSeqId & 0xFF) != NA_BGM_ESCAPE && curSeqId != seqId) {
        Na_SetBgmPattern(SEQ_MODE_IGNORE);
        if (curSeqId != NA_BGM_DISABLED) {
            na_now_bgm_flag = curSeqId & 0xFFFF;
        } else {
            PRINTF("Middle Boss BGM Start not stack \n");
        }

        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, seqId);
    }
}

/**
 * Restores the previous sequence to the main bgm player before Na_StartMiddleBossBgm was called
 */
void Na_StopMiddleBossBgm(void) {
    if ((Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) != NA_BGM_DISABLED) && (na_now_bgm_flag != NA_BGM_DISABLED) &&
        (na_bgm_info[Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) & 0xFF] & SEQ_FLAG_RESTORE)) {
        if (na_now_bgm_flag == NA_BGM_DISABLED) {
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
        } else {
#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
            if (na_now_bgm_flag == NA_BGM_NATURE_AMBIENCE) {
                na_now_bgm_flag = sPrevAmbienceSeqId;
            }
#endif
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, na_now_bgm_flag);
        }

        na_now_bgm_flag = NA_BGM_DISABLED;
    }
}

/**
 * Plays the nature ambience sequence on the main bgm player, but stores the previous sequence to return to later
 */
void Na_StartForceNatureSound(u8 natureAmbienceId) {
    u16 curSeqId = Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN);

    if (curSeqId != NA_BGM_NATURE_AMBIENCE) {
        na_now_bgm_flag = curSeqId;
    }

    Na_NscInitStart(natureAmbienceId);
}

/**
 * Restores the previous sequence to the main bgm player before Na_StartForceNatureSound was called
 */
void Na_StopForceNatureSound(void) {
    if (na_now_bgm_flag != NA_BGM_DISABLED) {
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, na_now_bgm_flag);
    }
    na_now_bgm_flag = NA_BGM_DISABLED;
}

void Na_StartFanfare(u16 seqId) {
    u16 curSeqId;
    u32 outNumFonts;
    u8* curFontId;
    u8* requestedFontId;

    curSeqId = Nai_GetPlayingSeqFlag(SEQ_PLAYER_FANFARE);

    curFontId = Nap_SeqToBank(curSeqId & 0xFF, &outNumFonts);
    requestedFontId = Nap_SeqToBank(seqId & 0xFF, &outNumFonts);

    if ((curSeqId == NA_BGM_DISABLED) || (*curFontId == *requestedFontId)) {
        na_fanfare_delay = 1;
    } else {
        // Give extra time to start the fanfare if both another fanfare needs to be stopped
        // and a new fontId needs to be loaded in
        na_fanfare_delay = 5;
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
    }
    na_fanfare_flag = seqId;
}

void Na_CheckFanfareFlag(void) {
    u16 seqIdFanfare;
    u16 seqIdBgmMain;
    u16 seqIdBgmSub;

    if (na_fanfare_delay != 0) {
        na_fanfare_delay--;
        if (na_fanfare_delay == 0) {
            AUDIOCMD_GLOBAL_POP_PERSISTENT_CACHE(SEQUENCE_TABLE);
            AUDIOCMD_GLOBAL_POP_PERSISTENT_CACHE(FONT_TABLE);

            seqIdBgmMain = Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN);
            seqIdFanfare = Nai_GetPlayingSeqFlag(SEQ_PLAYER_FANFARE);
            seqIdBgmSub = Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_SUB);

            (void)seqIdBgmMain; // suppresses set but unused warning
            if (seqIdFanfare == NA_BGM_DISABLED) {
                Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_FANFARE, 0, 5);
                Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_FANFARE, 0, 5);
                SEQCMD_SETUP_RESTORE_SEQPLAYER_VOLUME_WITH_SCALE_INDEX(SEQ_PLAYER_FANFARE, SEQ_PLAYER_BGM_MAIN,
                                                                       VOL_SCALE_INDEX_FANFARE, 10);
                SEQCMD_SETUP_RESTORE_SEQPLAYER_VOLUME_WITH_SCALE_INDEX(SEQ_PLAYER_FANFARE, SEQ_PLAYER_BGM_SUB,
                                                                       VOL_SCALE_INDEX_FANFARE, 10);
                SEQCMD_SETUP_SET_CHANNEL_DISABLE_MASK(SEQ_PLAYER_FANFARE, SEQ_PLAYER_BGM_MAIN, 0);
                if (seqIdBgmSub != NA_BGM_LONLON) {
                    SEQCMD_SETUP_SET_CHANNEL_DISABLE_MASK(SEQ_PLAYER_FANFARE, SEQ_PLAYER_BGM_SUB, 0);
                }
            }
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_FANFARE, 1, 0, na_fanfare_flag);
            SEQCMD_SET_CHANNEL_DISABLE_MASK(SEQ_PLAYER_BGM_MAIN, 0xFFFF);
            if (seqIdBgmSub != NA_BGM_LONLON) {
                SEQCMD_SET_CHANNEL_DISABLE_MASK(SEQ_PLAYER_BGM_SUB, 0xFFFF);
            }
        }
    }
}

void Na_StartBgmWithPortWrite(u8 seqPlayerIndex, u16 seqId, u8 fadeInDuration, s8 ioPort, s8 ioData) {
    SEQCMD_SET_SEQPLAYER_IO(seqPlayerIndex, ioPort, ioData);
    SEQCMD_PLAY_SEQUENCE(seqPlayerIndex, fadeInDuration, 0, seqId);
}

void Na_SetBgmPattern(u8 seqMode) {
    s32 volumeFadeInTimer;
    u16 seqId;
    u8 volumeFadeOutTimer;

#if DEBUG_FEATURES
    sSeqModeInput = seqMode;
#endif

    if (na_now_bgm_flag == NA_BGM_DISABLED) {
        if (na_mes_flag) {
            seqMode = SEQ_MODE_IGNORE;
        }

        seqId = SeqInterWork[SEQ_PLAYER_BGM_MAIN].seqId;

        if (seqId == NA_BGM_FIELD_LOGIC && Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_SUB) == (NA_BGM_ENEMY | 0x800)) {
            seqMode = SEQ_MODE_IGNORE;
        }

        if ((seqId == NA_BGM_DISABLED) || (na_bgm_info[seqId & 0xFF & 0xFF] & SEQ_FLAG_ENEMY) ||
            ((na_seq_switch_flag & 0x7F) == SEQ_MODE_ENEMY)) {
            if (seqMode != (na_seq_switch_flag & 0x7F)) {
                if (seqMode == SEQ_MODE_ENEMY) {
                    // Start playing enemy bgm
                    if (SeqInterWork[SEQ_PLAYER_BGM_SUB].volScales[VOL_SCALE_INDEX_FANFARE] - na_bgm_enm_vol < 0) {
                        volumeFadeInTimer =
                            -(SeqInterWork[SEQ_PLAYER_BGM_SUB].volScales[VOL_SCALE_INDEX_FANFARE] - na_bgm_enm_vol);
                    } else {
                        volumeFadeInTimer =
                            SeqInterWork[SEQ_PLAYER_BGM_SUB].volScales[VOL_SCALE_INDEX_FANFARE] - na_bgm_enm_vol;
                    }

                    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_BGM_SUB, na_bgm_enm_vol,
                                         volumeFadeInTimer);
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_SUB, 10, 8, NA_BGM_ENEMY);

#if OOT_VERSION < PAL_1_0 || !PLATFORM_N64
                    if (seqId != NA_BGM_NATURE_AMBIENCE)
#else
                    if (seqId > NA_BGM_NATURE_AMBIENCE)
#endif
                    {
                        Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB,
                                             (0x7F - na_bgm_enm_vol) & 0xFF, 0xA);
                        Na_SetBgmPartMute(na_bgm_enm_vol);
                    }
                } else if ((na_seq_switch_flag & 0x7F) == SEQ_MODE_ENEMY) {
                    // Stop playing enemy bgm
                    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_SUB, 10);
                    if (seqMode == SEQ_MODE_IGNORE) {
                        volumeFadeOutTimer = 0;
                    } else {
                        volumeFadeOutTimer = 10;
                    }

                    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F, volumeFadeOutTimer);
                    Na_SetBgmPartMute(0);
                }

                na_seq_switch_flag = seqMode + 0x80;
            } else {
#if OOT_VERSION < NTSC_1_1 || !PLATFORM_N64
                // Empty
#elif OOT_VERSION < PAL_1_0
                if ((seqMode == SEQ_MODE_ENEMY) && (seqId != NA_BGM_FIELD_LOGIC) &&
                    (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_SUB) == NA_BGM_DISABLED)) {
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_SUB, 10, 8, NA_BGM_ENEMY);
                    na_seq_switch_flag = seqMode + 0x80;
                }
#else
                // If both seqMode = na_seq_switch_flag = SEQ_MODE_ENEMY
                if ((seqMode == SEQ_MODE_ENEMY) && (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_SUB) == NA_BGM_DISABLED) &&
                    (seqId != NA_BGM_DISABLED) && (na_bgm_info[seqId & 0xFF & 0xFF] & SEQ_FLAG_ENEMY)) {
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_SUB, 10, 8, NA_BGM_ENEMY);
                    na_seq_switch_flag = seqMode + 0x80;
                }
#endif
            }
        } else {
            // Hyrule Field will play slightly different background music depending on whether player is standing
            // still or moving. This is the logic to determine the transition between those two states
            if (seqMode == SEQ_MODE_DEFAULT) {
                if (na_seq_switch_flag == SEQ_MODE_STILL) {
                    na_seq_switch_normal_time = 0;
                }
                na_seq_switch_int_time = 0;
                na_seq_switch_normal_time++;
            } else {
                na_seq_switch_int_time++;
            }

            if (seqMode == SEQ_MODE_STILL && na_seq_switch_int_time < 30 && na_seq_switch_normal_time > 20) {
                seqMode = SEQ_MODE_DEFAULT;
            }

            na_seq_switch_flag = seqMode;
            SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_BGM_MAIN, 2, seqMode);
        }
    }
}

void Na_SetEnemyBgmDistance(f32 dist) {
    f32 adjDist;

    if (na_seq_switch_flag == (0x80 | SEQ_MODE_ENEMY)) {
        if (dist != na_bgm_enm_dist) {
            if (dist < 150.0f) {
                adjDist = 0.0f;
            } else if (dist > 500.0f) {
                adjDist = 350.0f;
            } else {
                adjDist = dist - 150.0f;
            }

            na_bgm_enm_vol = ((350.0f - adjDist) * 127.0f) / 350.0f;
            Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_SUB, VOL_SCALE_INDEX_BGM_SUB, na_bgm_enm_vol, 10);

#if OOT_VERSION < PAL_1_0 || !PLATFORM_N64
            if (SeqInterWork[SEQ_PLAYER_BGM_MAIN].seqId != NA_BGM_NATURE_AMBIENCE)
#else
            if (SeqInterWork[SEQ_PLAYER_BGM_MAIN].seqId > NA_BGM_NATURE_AMBIENCE)
#endif
            {
                Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, (0x7F - na_bgm_enm_vol), 10);
            }
        }

#if OOT_VERSION < PAL_1_0 || !PLATFORM_N64
        if (SeqInterWork[SEQ_PLAYER_BGM_MAIN].seqId != NA_BGM_NATURE_AMBIENCE)
#else
        if (SeqInterWork[SEQ_PLAYER_BGM_MAIN].seqId > NA_BGM_NATURE_AMBIENCE)
#endif
        {
            Na_SetBgmPartMute(na_bgm_enm_vol);
        }
    }
    na_bgm_enm_dist = dist;
}

void Na_SetObjectBgmDistance(f32 dist, u16 seqId) {
    s8 pad;
    s8 melodyVolume;
    s16 curSeqId;

#if DEBUG_FEATURES
    sIsMalonSinging = true;
    sMalonSingingDist = dist;
#endif

    if (na_mute_bgm_flag) {
        return;
    }

    curSeqId = (s8)(Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) & 0xFF);

    if (curSeqId == (seqId & 0xFF)) {
        if ((seqId & 0xFF) == NA_BGM_LONLON) {
            // Malon is singing along with the Lon Lon Sequence

            if (dist > 2000.0f) {
                melodyVolume = 127;
            } else if (dist < 200.0f) {
                melodyVolume = 0;
            } else {
                melodyVolume = (s8)(((dist - 200.0f) * 127.0f) / 1800.0f);
            }

            // Update volume for channels 0 & 1, which contain Malon's singing
            SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_MAIN, 0, 3, 127 - melodyVolume);
            SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_MAIN, 1, 3, 127 - melodyVolume);

            // Update volume for channel 13, which contains the melody line for Lon Lon's Sequence
            SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_MAIN, 13, 3, melodyVolume);
            if (na_ronron_bgm_mode == 0) {
                na_ronron_bgm_mode++;
            }
        }
    } else if ((curSeqId == NA_BGM_NATURE_AMBIENCE) && ((seqId & 0xFF) == NA_BGM_LONLON)) {
        // Malon is singing along with ambience
        curSeqId = (s8)(Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_SUB) & 0xFF);

        if ((curSeqId != (seqId & 0xFF)) && (na_ronron_bgm_mode < 10)) {
            Na_StartBgmWithPortWrite(SEQ_PLAYER_BGM_SUB, NA_BGM_LONLON, 0, 0, 0);
            // Disable all channels between 2-15.
            // Only allow the two channels with Malon's singing to play, and suppress the full lon lon sequence.
            SEQCMD_SET_CHANNEL_DISABLE_MASK(SEQ_PLAYER_BGM_SUB, 0xFFFC);
            na_ronron_bgm_mode = 10;
        }

        if (dist > 2000.0f) {
            melodyVolume = 127;
        } else if (dist < 200.0f) {
            melodyVolume = 0;
        } else {
            melodyVolume = (s8)(((dist - 200.0f) * 127.0f) / 1800.0f);
        }

        // Update volume for channels 0 & 1, which contain Malon's singing
        SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_SUB, 0, 3, 127 - melodyVolume);
        SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_BGM_SUB, 1, 3, 127 - melodyVolume);
    }

    if (na_ronron_bgm_mode < 10) {
        na_ronron_bgm_mode++;
    }
}

void Na_SetWindowSound(u8 arg0) {
    na_pause_flag = arg0;
    if (arg0 != 0) {
        Nai_FxFlagEntry(NA_SE_SY_WIN_OPEN, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        AUDIOCMD_GLOBAL_MUTE();
    } else {
        Nai_FxFlagEntry(NA_SE_SY_WIN_CLOSE, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        AUDIOCMD_GLOBAL_UNMUTE(0);
    }
}

/**
 * Enable or disable Malon's singing
 *
 * @param malonSingingDisabled true to disable, false to enable
 */
void Na_SetMuteBgm(u8 malonSingingDisabled) {
    u8 seqPlayerIndex;
    u16 channelMaskDisable;

    na_mute_bgm_flag = malonSingingDisabled;

    if ((Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) & 0xFF) == NA_BGM_LONLON) {
        // Malon is singing along with the Lon Lon Sequence
        seqPlayerIndex = SEQ_PLAYER_BGM_MAIN;
        // Do not disable any channel.
        // Allow the full lon lon sequence to play in addition to Malon's singing.
        channelMaskDisable = 0;
    } else if ((u8)Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_SUB) == NA_BGM_LONLON) {
        // Malon is singing along with ambience
        seqPlayerIndex = SEQ_PLAYER_BGM_SUB;
        // Disable all channels between 2-15.
        // Only allow the two channels with Malon's singing to play, and suppress the full lon lon sequence.
        channelMaskDisable = 0xFFFC;
    } else {
        return;
    }

    if (malonSingingDisabled) {
        // Turn volume off for channels 0 & 1, which contain Malon's singing
        SEQCMD_SET_CHANNEL_VOLUME(seqPlayerIndex, 0, 1, 0);
        SEQCMD_SET_CHANNEL_VOLUME(seqPlayerIndex, 1, 1, 0);

        if (seqPlayerIndex == SEQ_PLAYER_BGM_SUB) {
            // When singing along with ambience, disable all 16 channels
            SEQCMD_SET_CHANNEL_DISABLE_MASK(seqPlayerIndex, channelMaskDisable | 3);
        }
    } else {
        if (seqPlayerIndex == SEQ_PLAYER_BGM_SUB) {
            // When singing along with ambience, start the sequence
            Na_StartBgmWithPortWrite(SEQ_PLAYER_BGM_SUB, NA_BGM_LONLON, 0, 0, 0);
        }

        // Turn volume on for only channels 0 & 1, which contain Malon's singing
        SEQCMD_SET_CHANNEL_VOLUME(seqPlayerIndex, 0, 1, 0x7F);
        SEQCMD_SET_CHANNEL_VOLUME(seqPlayerIndex, 1, 1, 0x7F);

        if (seqPlayerIndex == SEQ_PLAYER_BGM_SUB) {
            // When singing along with ambience, disable channels 2-15
            SEQCMD_SET_CHANNEL_DISABLE_MASK(seqPlayerIndex, channelMaskDisable);
        }
    }
}

void Na_SetEnvEcho(s8 reverb) {
    enviroment_fx = reverb & 0x7F;
}

void Na_SetAddEcho(s8 reverb) {
    if (reverb != 0) {
        code_fx_ofs = reverb & 0x7F;
    }
}

void Na_SetSoundOutputMode(s8 audioSetting) {
    s8 soundModeIndex;

    switch (audioSetting) {
        case 0:
            soundModeIndex = SOUNDMODE_STEREO;
            na_sound_output = SOUNDMODE_STEREO;
            break;

        case 1:
            soundModeIndex = SOUNDMODE_MONO;
            na_sound_output = SOUNDMODE_MONO;
            break;

        case 2:
            soundModeIndex = SOUNDMODE_HEADSET;
            na_sound_output = SOUNDMODE_HEADSET;
            break;

        case 3:
            soundModeIndex = SOUNDMODE_STEREO;
            na_sound_output = SOUNDMODE_SURROUND;
            break;
    }

    SEQCMD_SET_SOUND_MODE(soundModeIndex);
}

void Na_SetDirectFir(u8 filter) {
    if (na_forth_lowpass_mode_0 != filter) {
        if (filter == 0) {
            Nai_StopFx(NA_SE_PL_IN_BUBBLE);
        } else if (na_forth_lowpass_mode_0 == 0) {
            Nai_FxFlagEntry(NA_SE_PL_IN_BUBBLE, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }
    na_forth_lowpass_mode_0 = filter;
    na_forth_vibrate_mode = filter;
}

void Na_SetWaterSeModeFlag(u8 filter) {
    u8 channelIndex;

    na_forth_vibrate_mode2 = filter;
    na_forth_lowpass_mode_1 = filter;
    if (SeqInterWork[SEQ_PLAYER_BGM_MAIN].seqId == NA_BGM_NATURE_AMBIENCE) {
        for (channelIndex = 0; channelIndex < SEQ_NUM_CHANNELS; channelIndex++) {
            AUDIOCMD_CHANNEL_SET_IO(SEQ_PLAYER_BGM_MAIN, (u32)channelIndex, 6, filter);
        }
    }
}

void Na_SetMessageStatusFlag(s8 flag) {
    na_mes_flag = flag;
}

void Na_DemoOffFxFlagEntry(u16 sfxId, Vec3f* pos, u8 arg2, f32* freqScale, f32* arg4, s8* reverbAdd) {
    if (!na_mes_flag) {
        Nai_FxFlagEntry(sfxId, pos, arg2, freqScale, arg4, reverbAdd);
    }
}

void Na_SwapSeFlagOnDemo(u16 sfxId) {
    Na_DemoOffFxFlagEntry(sfxId, &_dummy_zero_f, 4, &_dummy_one,
                                        &_dummy_one, &_dummy_zero_s8);
}

void Na_StopAllSound(u16 arg0) {
    s32 skip;
    u8 channelIdx;

    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, (arg0 * 3) / 2);
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, (arg0 * 3) / 2);
    for (channelIdx = 0; channelIdx < 16; channelIdx++) {
        skip = false;
        switch (channelIdx) {
            case SFX_CHANNEL_SYSTEM0:
            case SFX_CHANNEL_SYSTEM1:
                if (SndSpecNum == 10) {
                    skip = true;
                }
                break;
            case SFX_CHANNEL_OCARINA:
                skip = true;
                break;
        }

        if (!skip) {
            SEQCMD_SET_CHANNEL_VOLUME(SEQ_PLAYER_SFX, channelIdx, arg0 >> 1, 0);
        }
    }

    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_SUB, (arg0 * 3) / 2);
}

void Na_StopAllBgm(u16 fadeOutDuration) {
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, fadeOutDuration);
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, fadeOutDuration);
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_SUB, fadeOutDuration);
    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F, 0);
    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_FANFARE, 0x7F, 0);
}

void Na_RecoverSe(void) {
    Nai_StartSeqSet(SEQ_PLAYER_SFX, 0, 0xFF, 5);
}

void Na_StopAllSound2(void) {
    AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(SEQ_PLAYER_BGM_MAIN, 0);
    AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(SEQ_PLAYER_FANFARE, 0);
    AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(SEQ_PLAYER_SFX, 0);
    AUDIOCMD_GLOBAL_DISABLE_SEQPLAYER(SEQ_PLAYER_BGM_SUB, 0);
    Nap_SendStart();
}

s8 Na_CheckAllChannels(void) {
    return Nap_SilenceCheck();
}

void Na_ForceStopSound(void) {
    Na_StopAllSound2();
    Nap_SendStart();
    while (true) {
        if (!Na_CheckAllChannels()) {
            return;
        }
    }
}

void Na_ResetAudio(void) {
    Nap_StartReset();
}

void Nag_InitGameSoundWork(void) {
    na_seq_switch_flag = 0;
    na_link_walk_volume = 1.0f;
    na_link_walk_pitch = 1.0f;
    na_forth_lowpass_mode_0 = 0;
    na_forth_lowpass_mode_1 = 0;
    na_forth_vibrate_mode = 0;
    na_forth_vibrate_mode2 = 0;
    Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_OFF);
    na_river_pitch.remainingFrames = 0;
    na_waterfall_pitch.remainingFrames = 0;
    na_river_pitch.value = 1.0f;
    na_waterfall_pitch.value = 1.0f;
    na_motor_pitch = 1.0f;
    na_level_mute_vol_target = 0x7F;
    na_level_mute_vol_now = 0x7F;
    na_level_mute_flag_next = false;
    na_level_mute_flag_old = false;
    na_trigger_mute_vol_now = 0xFF;
    na_ronron_bgm_mode = 0;
    default_fx = na_spec_echo_ofs[SndSpecNum];
    na_pause_flag = 0;
    na_now_bgm_flag = NA_BGM_DISABLED;
    AUDIOCMD_SEQPLAYER_SET_IO(SEQ_PLAYER_BGM_MAIN, 0, SEQ_IO_VAL_NONE);
    saria_oca_pos = NULL;
    na_fanfare_delay = 0;
    na_cross_bgm_delay = 1;
    na_mute_bgm_flag = false;
#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
    sPrevAmbienceSeqId = NA_BGM_DISABLED;
#endif
}

void Na_ControlNaturalSound(u8 channelIdxRange, u8 ioPort, u8 ioData) {
    u8 firstChannelIdx;
    u8 lastChannelIdx;
    u8 channelIdx;

    if ((SeqInterWork[SEQ_PLAYER_BGM_MAIN].seqId != NA_BGM_NATURE_AMBIENCE) &&
        Nai_CheckSeqFlagBuffer2(SEQCMD_OP_PLAY_SEQUENCE << 28 | NA_BGM_NATURE_AMBIENCE, SEQCMD_OP_MASK | 0xFF)) {

#if DEBUG_FEATURES
        sAudioNatureFailed = true;
#endif

        return;
    }

    // channelIdxRange = 01 on ioPort 1
    if (((channelIdxRange << 8) + ioPort) == ((NATURE_CHANNEL_CRITTER_0 << 8) + CHANNEL_IO_PORT_1)) {
        if (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_SUB) != NA_BGM_LONLON) {
            na_ronron_bgm_mode = 0;
        }
    }

    firstChannelIdx = channelIdxRange >> 4;
    lastChannelIdx = channelIdxRange & 0xF;

    if (firstChannelIdx == 0) {
        firstChannelIdx = channelIdxRange & 0xF;
    }

    for (channelIdx = firstChannelIdx; channelIdx <= lastChannelIdx; channelIdx++) {
        SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_BGM_MAIN, channelIdx, ioPort, ioData);
    }
}

void Na_StartNaturalSound(u16 playerIO, u16 channelMask) {
    u8 channelIdx;

    if (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) == NA_BGM_WINDMILL) {
        Na_StartDemoSe(SEQ_CS_EFFECTS_RAINFALL);
        return;
    }

    SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_BGM_MAIN, 0, 1);
    SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_BGM_MAIN, 4, playerIO >> 8);
    SEQCMD_SET_SEQPLAYER_IO(SEQ_PLAYER_BGM_MAIN, 5, playerIO & 0xFF);
    Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_MAIN, 0x7F, 1);

    channelIdx = false;

#if DEBUG_FEATURES
    if (SeqStartCancel) {
        channelIdx = true;
        SEQCMD_DISABLE_PLAY_SEQUENCES(false);
    }
#endif

#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
    if ((Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) != NA_BGM_DISABLED) &&
        (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_MAIN) != NA_BGM_NATURE_AMBIENCE)) {
        Nai_StopSeqSet(SEQ_PLAYER_BGM_MAIN, 0);
        AUDIOCMD_GLOBAL_STOP_AUDIOCMDS();
    }

    if (Nai_GetPlayingSeqFlag(SEQ_PLAYER_BGM_SUB) == (NA_BGM_ENEMY | 0x800)) {
        Nai_SetSeqLineVolMove(SEQ_PLAYER_BGM_MAIN, VOL_SCALE_INDEX_BGM_SUB, 0x7F, 1);
    }
#endif

    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_NATURE_AMBIENCE);

    if (channelIdx) {
        SEQCMD_DISABLE_PLAY_SEQUENCES(true);
    }

    for (channelIdx = 0; channelIdx < 16; channelIdx++) {
        if (!(channelMask & (1 << channelIdx)) && (playerIO & (1 << channelIdx))) {
            SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_BGM_MAIN, channelIdx, CHANNEL_IO_PORT_1, 1);
        }
    }
}

void Na_NscInitStart(u8 natureAmbienceId) {
    u8 i = 0;
    u8 channelIdx;
    u8 ioPort;
    u8 ioData;

    if ((SeqInterWork[SEQ_PLAYER_BGM_MAIN].seqId == NA_BGM_DISABLED) ||
        !(na_bgm_info[SeqInterWork[SEQ_PLAYER_BGM_MAIN].seqId & 0xFF & 0xFF] & SEQ_FLAG_NO_AMBIENCE)) {

#if !(OOT_VERSION < NTSC_1_1 || !PLATFORM_N64)
        if (SeqInterWork[SEQ_PLAYER_BGM_MAIN].seqId != NA_BGM_NATURE_AMBIENCE) {
            sPrevAmbienceSeqId = SeqInterWork[SEQ_PLAYER_BGM_MAIN].seqId;
        }
#endif

        Na_StartNaturalSound(na_nsc_init_flag[natureAmbienceId].playerIO,
                                          na_nsc_init_flag[natureAmbienceId].channelMask);

        while ((na_nsc_init_flag[natureAmbienceId].channelIO[i] != 0xFF) && (i < 100)) {
            channelIdx = na_nsc_init_flag[natureAmbienceId].channelIO[i++];
            ioPort = na_nsc_init_flag[natureAmbienceId].channelIO[i++];
            ioData = na_nsc_init_flag[natureAmbienceId].channelIO[i++];
            SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_BGM_MAIN, channelIdx, ioPort, ioData);
        }

        SEQCMD_SET_CHANNEL_IO(SEQ_PLAYER_BGM_MAIN, NATURE_CHANNEL_UNK, CHANNEL_IO_PORT_7, na_sound_output);
    }
}

void Na_AudioInit(void) {
    Nas_InitAudio(NULL, 0);
}

void Nai_InitInterface(void) {
    Nag_InitGameSoundWork();
    Na_InitOcarinaWork();
    Naa_InitApliWork();
    Nai_seq_inter_init();
    Nai_fx_inter_init();
    Nai_StartSeqSet(SEQ_PLAYER_SFX, 0, 0x70, 10);
}

void Nai_InitInterface_spec(void) {
    Nai_StartSeqSet(SEQ_PLAYER_SFX, 0, 0x70, 1);
    AUDIOCMD_GLOBAL_UNMUTE(1);
    Nap_SendStart();
    AUDIOCMD_GLOBAL_STOP_AUDIOCMDS();
}

void Nai_StartSpecMode(s32 arg0) {
    SpecBusyFlag = 1;
    Nag_InitGameSoundWork();
    Na_InitOcarinaWork();
    Naa_InitApliWork();
    Nai_seq_inter_init_spec();
    Nai_fx_inter_init();
}

void Nai_InitNoSpec(void) {
    Nai_seq_inter_init_spec();
    AUDIOCMD_GLOBAL_UNMUTE(1);
    Nag_InitGameSoundWork();
    Naa_InitApliWork();
    Nai_StartSeqSet(SEQ_PLAYER_SFX, 0, 0x70, 1);
}
