/*
 * File: z_player.c
 * Overlay: ovl_player_actor
 * Description: Link
 */

#include "ultra64.h"
#include "global.h"
#include "quake.h"
#include "versions.h"

#include "overlays/actors/ovl_Bg_Heavy_Block/z_bg_heavy_block.h"
#include "overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo.h"
#include "overlays/actors/ovl_En_Boom/z_en_boom.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_En_Box/z_en_box.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "overlays/actors/ovl_En_Fish/z_en_fish.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"
#include "overlays/actors/ovl_En_Insect/z_en_insect.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_link_child/object_link_child.h"

// Some player animations are played at this reduced speed, for reasons yet unclear.
// This is called "adjusted" for now.
#define PLAYER_ANIM_ADJUSTED_SPEED (2.0f / 3.0f)

typedef struct GetItemEntry {
    /* 0x00 */ u8 itemId;
    /* 0x01 */ u8 field; // various bit-packed data
    /* 0x02 */ s8 gi;    // defines the draw id and chest opening animation
    /* 0x03 */ u8 textId;
    /* 0x04 */ u16 objectId;
} GetItemEntry; // size = 0x06

#define GET_ITEM(itemId, objectId, drawId, textId, field, chestAnim) \
    { itemId, field, (chestAnim != CHEST_ANIM_SHORT ? 1 : -1) * (drawId + 1), textId, objectId }

#define CHEST_ANIM_SHORT 0
#define CHEST_ANIM_LONG 1

#define GET_ITEM_NONE \
    { ITEM_NONE, 0, 0, 0, OBJECT_INVALID }

typedef struct ExplosiveInfo {
    /* 0x00 */ u8 itemId;
    /* 0x02 */ s16 actorId;
} ExplosiveInfo; // size = 0x04

typedef struct BottleDropInfo {
    /* 0x00 */ s16 actorId;
    /* 0x02 */ s16 actorParams;
} BottleDropInfo; // size = 0x04

typedef struct FallImpactInfo {
    /* 0x00 */ s8 damage;
    /* 0x01 */ u8 rumbleStrength;
    /* 0x02 */ u8 rumbleDuration;
    /* 0x03 */ u8 rumbleDecreaseRate;
    /* 0x04 */ u16 sfxId;
} FallImpactInfo; // size = 0x06

typedef struct SpecialRespawnInfo {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yaw;
} SpecialRespawnInfo; // size = 0x10

typedef enum AnimSfxType {
    /* 1 */ ANIMSFX_TYPE_GENERAL = 1,
    /* 2 */ ANIMSFX_TYPE_FLOOR,
    /* 3 */ ANIMSFX_TYPE_FLOOR_BY_AGE,
    /* 4 */ ANIMSFX_TYPE_VOICE,
    /* 5 */ ANIMSFX_TYPE_LANDING, // `AnimSfxEntry.sfxId` is ignored. Adjusted for Iron Boots if needed.
    /* 6 */ ANIMSFX_TYPE_RUNNING, // `AnimSfxEntry.sfxId` is ignored. Adjusted for Iron Boots if needed.
    /* 7 */ ANIMSFX_TYPE_JUMPING, // `AnimSfxEntry.sfxId` is ignored. Adjusted for Iron Boots if needed.
    /* 8 */ ANIMSFX_TYPE_WALKING, // `AnimSfxEntry.sfxId` is ignored. Adjusted for Iron Boots if needed.
    /* 9 */ ANIMSFX_TYPE_UNKNOWN  // `AnimSfxEntry.sfxId` is ignored. Only used in the intro cutscene.
} AnimSfxType;

#define ANIMSFX_SHIFT_TYPE(type) ((type) << 11)

#define ANIMSFX_DATA(type, frame) ((ANIMSFX_SHIFT_TYPE(type) | ((frame)&0x7FF)))

#define ANIMSFX_GET_TYPE(data) ((data)&0x7800)
#define ANIMSFX_GET_FRAME(data) ((data)&0x7FF)

typedef struct AnimSfxEntry {
    /* 0x00 */ u16 sfxId;
    /* 0x02 */ s16 data;
} AnimSfxEntry; // size = 0x04

typedef struct struct_808551A4 {
    /* 0x00 */ u16 unk_00;
    /* 0x02 */ s16 unk_02;
} struct_808551A4; // size = 0x04

typedef struct ItemChangeInfo {
    /* 0x00 */ LinkAnimationHeader* anim;
    /* 0x04 */ u8 changeFrame;
} ItemChangeInfo; // size = 0x08

typedef struct struct_80854190 {
    /* 0x00 */ LinkAnimationHeader* unk_00;
    /* 0x04 */ LinkAnimationHeader* unk_04;
    /* 0x08 */ LinkAnimationHeader* unk_08;
    /* 0x0C */ u8 unk_0C;
    /* 0x0D */ u8 unk_0D;
} struct_80854190; // size = 0x10

typedef struct struct_80854578 {
    /* 0x00 */ LinkAnimationHeader* anim;
    /* 0x04 */ f32 unk_04;
    /* 0x04 */ f32 unk_08;
} struct_80854578; // size = 0x0C

typedef struct struct_80854B18 {
    /* 0x00 */ s8 type;
    /* 0x04 */ union {
        void* ptr;
        void (*func)(PlayState*, Player*, CsCmdActorCue*);
    };
} struct_80854B18; // size = 0x08

void ability_A_item_change(PlayState* play, Player* this, s8 itemAction);

void ability_A_item_non_init(PlayState* play, Player* this);
void ability_A_item_hammer_init(PlayState* play, Player* this);
void ability_A_item_bow_init(PlayState* play, Player* this);
void ability_A_item_stick_init(PlayState* play, Player* this);
void ability_A_item_bomb_init(PlayState* play, Player* this);
void ability_A_item_hook_init(PlayState* play, Player* this);
void ability_A_item_boom_init(PlayState* play, Player* this);

s32 uperbody_nom2figh_play(Player* this, PlayState* play);
s32 uperbody_non(Player* this, PlayState* play);
s32 uperbody_nom2figh_wait(Player* this, PlayState* play);
s32 uperbody_defense(Player* this, PlayState* play);
s32 uperbody_defense_end(Player* this, PlayState* play);
s32 uperbody_bow_wait(Player* this, PlayState* play);
s32 uperbody_bow_ready(Player* this, PlayState* play);
s32 uperbody_bow_shoot(Player* this, PlayState* play);
s32 uperbody_bow_shoot_end(Player* this, PlayState* play);
s32 uperbody_carry(Player* this, PlayState* play);
s32 uperbody_boom_wait(Player* this, PlayState* play);
s32 uperbody_boom_ready(Player* this, PlayState* play);
s32 uperbody_boom_ready_wait(Player* this, PlayState* play);
s32 uperbody_boom_throw(Player* this, PlayState* play);
s32 uperbody_boom_throw_wait(Player* this, PlayState* play);
s32 uperbody_boom_catch(Player* this, PlayState* play);

void player_item_on(PlayState* play, Player* this, s32 item);
void to_wait_set_check(Player* this, PlayState* play);
s32 to_light_bom_check(PlayState* play, Player* this);
void Ext_Player_actor_proc(Player* this, PlayState* play, Input* input);
void bow_gen_action(Player* this);
void rabit_mask_action(Player* this);
void demo_play_non_init(PlayState* play, Player* this, void* anim);
void demo_play_stop_anime_init(PlayState* play, Player* this, void* anim);
void demo_play_0_stop_anime_init(PlayState* play, Player* this, void* anim);
void demo_play_stop_anime2_init(PlayState* play, Player* this, void* anim);
void demo_play_repeat_anime2_init(PlayState* play, Player* this, void* anim);
void demo_play_stop_anime_move_init(PlayState* play, Player* this, void* anim);
void demo_play_stop_anime_move2_init(PlayState* play, Player* this, void* anim);
void demo_play_repeat_anime_move_init(PlayState* play, Player* this, void* anim);
void demo_play_repeat_anime_move3_init(PlayState* play, Player* this, void* anim);
void demo_play_standard_stop_init(PlayState* play, Player* this, void* anim);
void demo_play_standard_repeat_init(PlayState* play, Player* this, void* anim);
void demo_play_standard_stop2_init(PlayState* play, Player* this, void* anim);
void demo_play_standard_repeat2_init(PlayState* play, Player* this, void* anim);
void demo_play_anime_play(PlayState* play, Player* this, void* anim);
void demo_play_anime_play_repeat(PlayState* play, Player* this, void* anim);
void demo_play_anime_play_standard_repeat2(PlayState* play, Player* this, void* anim);
void demo_play_anime_move_play_repeat(PlayState* play, Player* this, void* anim);
void demo_play_anime_play_SE_set(PlayState* play, Player* this, void* arg2);
void demo_play_swim_wait_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_swim_wait(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_anchor_stop(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_goma_furimuki(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_wait_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_wait(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_furimuki(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_warp_out_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_warp_out(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_fighter_wait_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_fighter_wait(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_run_start(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_run_end(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_m_sword_start_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_m_sword_start(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_kenjya_start_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_kenjya_start(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_big_down(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_okarina_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_sword_standby(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_eye_close(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_eye_open(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_water_get_item_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_op3_negaeri_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_op3_negaeri(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_op3_wait_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_op3_okiagari(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_op3_tatiagari(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_futtobi_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_futtobi(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_warp_up(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_wait2_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_wait2(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_clear_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_display_init(PlayState* play, Player* this, CsCmdActorCue* cue);
#if OOT_VERSION >= PAL_1_0
void demo_play_nozokikomi(PlayState* play, Player* this, CsCmdActorCue* cue);
#endif
void demo_play_tewatashi(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_koutai_kennuki(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_furimuki2(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_mimawasi(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_koutai(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_power_kiru_wait_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_power_kiru_wait(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_power_kiru_shoot(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_sword_check_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_sword_check(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_down_to_stand_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_down_to_stand(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_m_sword_end_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_hajikareru(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_okarina_look(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_m_sword_catch(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_last_hit(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_end(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_position_init(PlayState* play, Player* this, CsCmdActorCue* cue);
void demo_play_tool_play(PlayState* play, Player* this, CsCmdActorCue* cue);
int Ext_player_fish_out_check(PlayState* play);
s32 Ext_to_fishing_set(PlayState* play);
s32 Ext_to_re_dead_attack_set(PlayState* play, Player* this);
s32 Ext_player_demo_set(PlayState* play, Actor* actor, s32 csAction);
void Ext_to_wait_set(Player* this, PlayState* play);
s32 Ext_to_damage_set(PlayState* play, s32 damage);
void Ext_player_to_talk_set(PlayState* play, Actor* actor);

void move_anchor_wait(Player* this, PlayState* play);
void move_parallel_wait(Player* this, PlayState* play);
static void move_wait(Player* this, PlayState* play);
void move_parallel_side_walk(Player* this, PlayState* play);
void move_parallel_back_run(Player* this, PlayState* play);
void move_parallel_back_brake(Player* this, PlayState* play);
void move_parallel_back_brake_end(Player* this, PlayState* play);
void move_anchor_side_walk(Player* this, PlayState* play);
static void move_turn(Player* this, PlayState* play);
void move_run(Player* this, PlayState* play);
void move_anchor_run(Player* this, PlayState* play);
void move_bean_set(Player* this, PlayState* play);
void move_anchor_back_walk(Player* this, PlayState* play);
void move_anchor_back_brake(Player* this, PlayState* play);
void move_defense(Player* this, PlayState* play);
void move_defense_hit(Player* this, PlayState* play);
void move_damage(Player* this, PlayState* play);
void move_big_damage(Player* this, PlayState* play);
void move_big_damage_slip(Player* this, PlayState* play);
void move_big_damage_wake(Player* this, PlayState* play);
static void move_down(Player* this, PlayState* play);
void move_landing(Player* this, PlayState* play);
void move_landing_roll(Player* this, PlayState* play);
void move_run_jump_water(Player* this, PlayState* play);
void move_jump_kiru(Player* this, PlayState* play);
void move_power_kiru_wait(Player* this, PlayState* play);
void move_power_kiru_walk(Player* this, PlayState* play);
void move_power_kiru_side_walk(Player* this, PlayState* play);
void move_stepup100(Player* this, PlayState* play);
void move_demo_init(Player* this, PlayState* play);
void move_shutter_open(Player* this, PlayState* play);
void move_door_open(Player* this, PlayState* play);
static void move_carry(Player* this, PlayState* play);
void move_heavy_carry(Player* this, PlayState* play);
void move_silver_carry(Player* this, PlayState* play);
void move_silver_throw(Player* this, PlayState* play);
void move_no_carry(Player* this, PlayState* play);
void move_put(Player* this, PlayState* play);
static void move_throw(Player* this, PlayState* play);
void move_look(Player* this, PlayState* play);
static void move_talk(Player* this, PlayState* play);
void move_push_wait(Player* this, PlayState* play);
void move_pushing(Player* this, PlayState* play);
void move_pulling(Player* this, PlayState* play);
void move_fall_wait(Player* this, PlayState* play);
void move_fall_up(Player* this, PlayState* play);
void move_climb(Player* this, PlayState* play);
void move_climb_end(Player* this, PlayState* play);
void move_tunnel(Player* this, PlayState* play);
void move_tunnel_end(Player* this, PlayState* play);
void move_ride_horse(Player* this, PlayState* play);
void move_ride_horse_end(Player* this, PlayState* play);
void move_swim_wait(Player* this, PlayState* play);
void move_swim_demo(Player* this, PlayState* play);
void move_swim(Player* this, PlayState* play);
void move_anchor_swim(Player* this, PlayState* play);
void move_swim_deep(Player* this, PlayState* play);
void move_swim_deep_end(Player* this, PlayState* play);
void move_swim_damage(Player* this, PlayState* play);
void move_swim_down(Player* this, PlayState* play);
void move_okarina(Player* this, PlayState* play);
void move_light_bom(Player* this, PlayState* play);
void move_get_item(Player* this, PlayState* play);
void move_m_sword_end(Player* this, PlayState* play);
void move_bottle_drink(Player* this, PlayState* play);
void move_bottle_attack(Player* this, PlayState* play);
void move_bottle_bug_out(Player* this, PlayState* play);
void move_bottle_fish_out(Player* this, PlayState* play);
void move_put_item(Player* this, PlayState* play);
void move_slope_slip(Player* this, PlayState* play);
void move_tool_demo_wait(Player* this, PlayState* play);
void move_from_okarina_warp(Player* this, PlayState* play);
void move_from_warp(Player* this, PlayState* play);
void move_warp_fall(Player* this, PlayState* play);
void move_door_start(Player* this, PlayState* play);
void move_jump_start(Player* this, PlayState* play);
void move_bow_game(Player* this, PlayState* play);
void move_ice_down(Player* this, PlayState* play);
void move_electric_shock(Player* this, PlayState* play);
void move_kiru(Player* this, PlayState* play);
void move_kiru_rebound(Player* this, PlayState* play);
void move_twistar_magic_select(Player* this, PlayState* play);
void move_from_magic_window(Player* this, PlayState* play);
void move_magic_wait(Player* this, PlayState* play);
void move_hook_fly(Player* this, PlayState* play);
void move_fishing_throw(Player* this, PlayState* play);
void move_fishing_catch(Player* this, PlayState* play);
void move_demo_play(Player* this, PlayState* play);

// .bss part 1

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ique-cn:128 ntsc-1.0:64 ntsc-1.1:64 ntsc-1.2:64 pal-1.0:64 pal-1.1:64 hiratsu3:128"

static s32 old_anime_move_flag;

// TODO: There's probably a way to match BSS ordering with less padding by spreading the variables out and moving
// data around. It would be easier if we had more options for controlling BSS ordering in debug.
#pragma increment_block_number "gc-eu:192 gc-eu-mq:192 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ique-cn:128 ntsc-1.0:192 ntsc-1.1:192 ntsc-1.2:192 pal-1.0:192 pal-1.1:192 hiratsu3:128"

static s32 old_mask;
static Vec3f wall_hit_pos;
static Input* pad;

#pragma increment_block_number "gc-eu:160 gc-eu-mq:160 gc-jp:192 gc-jp-ce:192 gc-jp-mq:192 gc-us:192 gc-us-mq:192" \
                               "ique-cn:192 ntsc-1.0:128 ntsc-1.1:128 ntsc-1.2:128 pal-1.0:128 pal-1.1:128 hiratsu3:192"

// .data

static u8 link_joint_status[PLAYER_LIMB_MAX] = {
    false, // PLAYER_LIMB_NONE
    false, // PLAYER_LIMB_ROOT
    false, // PLAYER_LIMB_WAIST
    false, // PLAYER_LIMB_LOWER
    false, // PLAYER_LIMB_R_THIGH
    false, // PLAYER_LIMB_R_SHIN
    false, // PLAYER_LIMB_R_FOOT
    false, // PLAYER_LIMB_L_THIGH
    false, // PLAYER_LIMB_L_SHIN
    false, // PLAYER_LIMB_L_FOOT
    true,  // PLAYER_LIMB_UPPER
    true,  // PLAYER_LIMB_HEAD
    true,  // PLAYER_LIMB_HAT
    true,  // PLAYER_LIMB_COLLAR
    true,  // PLAYER_LIMB_L_SHOULDER
    true,  // PLAYER_LIMB_L_FOREARM
    true,  // PLAYER_LIMB_L_HAND
    true,  // PLAYER_LIMB_R_SHOULDER
    true,  // PLAYER_LIMB_R_FOREARM
    true,  // PLAYER_LIMB_R_HAND
    true,  // PLAYER_LIMB_SHEATH
    true   // PLAYER_LIMB_TORSO
};

static PlayerAgeProperties link_age_status_data[] = {
    {
        56.0f,            // ceilingCheckHeight
        90.0f,            // unk_04
        1.0f,             // unk_08
        111.0f,           // unk_0C
        70.0f,            // unk_10
        79.4f,            // unk_14
        59.0f,            // unk_18
        41.0f,            // unk_1C
        19.0f,            // unk_20
        36.0f,            // unk_24
        44.8f,            // unk_28
        56.0f,            // unk_2C
        68.0f,            // unk_30
        70.0f,            // unk_34
        18.0f,            // wallCheckRadius
        15.0f,            // unk_3C
        70.0f,            // unk_40
        { 9, 4671, 359 }, // unk_44
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // unk_4A
        {
            { 9, 6122, 359 },
            { 9, 7693, 380 },
            { 9, 6122, 359 },
            { 9, 7693, 380 },
        }, // unk_62
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // unk_7A
        {
            { -1592, 4694, 380 },
            { -1591, 6122, 359 },
        },                                     // unk_86
        0,                                     // unk_92
        0x80,                                  // unk_94
        &gPlayerAnim_link_demo_Tbox_open,      // unk_98
        &gPlayerAnim_link_demo_back_to_past,   // timeTravelStartAnim
        &gPlayerAnim_link_demo_return_to_past, // timeTravelEndAnim
        &gPlayerAnim_link_normal_climb_startA, // unk_A4
        &gPlayerAnim_link_normal_climb_startB, // unk_A8
        { &gPlayerAnim_link_normal_climb_upL, &gPlayerAnim_link_normal_climb_upR, &gPlayerAnim_link_normal_Fclimb_upL,
          &gPlayerAnim_link_normal_Fclimb_upR },                                          // unk_AC
        { &gPlayerAnim_link_normal_Fclimb_sideL, &gPlayerAnim_link_normal_Fclimb_sideR }, // unk_BC
        { &gPlayerAnim_link_normal_climb_endAL, &gPlayerAnim_link_normal_climb_endAR },   // unk_C4
        { &gPlayerAnim_link_normal_climb_endBR, &gPlayerAnim_link_normal_climb_endBL },   // unk_CC
    },
    {
        40.0f,                   // ceilingCheckHeight
        60.0f,                   // unk_04
        11.0f / 17.0f,           // unk_08
        71.0f,                   // unk_0C
        50.0f,                   // unk_10
        47.0f,                   // unk_14
        39.0f,                   // unk_18
        27.0f,                   // unk_1C
        19.0f,                   // unk_20
        22.0f,                   // unk_24
        29.6f,                   // unk_28
        32.0f,                   // unk_2C
        48.0f,                   // unk_30
        70.0f * (11.0f / 17.0f), // unk_34
        14.0f,                   // wallCheckRadius
        12.0f,                   // unk_3C
        55.0f,                   // unk_40
        { -24, 3565, 876 },      // unk_44
        {
            { -24, 3474, 862 },
            { -24, 4977, 937 },
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // unk_4A
        {
            { -24, 4977, 937 },
            { -24, 6495, 937 },
            { 9, 6122, 359 },
            { 9, 7693, 380 },
        }, // unk_62
        {
            { 8, 4694, 380 },
            { 9, 6122, 359 },
        }, // unk_7A
        {
            { -1592, 4694, 380 },
            { -1591, 6122, 359 },
        },                                        // unk_86
        0x20,                                     // unk_92
        0,                                        // unk_94
        &gPlayerAnim_clink_demo_Tbox_open,        // unk_98
        &gPlayerAnim_clink_demo_goto_future,      // timeTravelStartAnim
        &gPlayerAnim_clink_demo_return_to_future, // timeTravelEndAnim
        &gPlayerAnim_clink_normal_climb_startA,   // unk_A4
        &gPlayerAnim_clink_normal_climb_startB,   // unk_A8
        { &gPlayerAnim_clink_normal_climb_upL, &gPlayerAnim_clink_normal_climb_upR, &gPlayerAnim_link_normal_Fclimb_upL,
          &gPlayerAnim_link_normal_Fclimb_upR },                                          // unk_AC
        { &gPlayerAnim_link_normal_Fclimb_sideL, &gPlayerAnim_link_normal_Fclimb_sideR }, // unk_BC
        { &gPlayerAnim_clink_normal_climb_endAL, &gPlayerAnim_clink_normal_climb_endAR }, // unk_C4
        { &gPlayerAnim_clink_normal_climb_endBR, &gPlayerAnim_clink_normal_climb_endBL }, // unk_CC
    },
};

static u32 player_debug_mode = false;
static f32 stick_length = 0.0f;
static s16 stick_angle = 0;
static s16 stick_camera_angle = 0;
static s32 uperbody_action_flag = false; // see `uperbody_action_check`
static s32 ground_attribute = FLOOR_TYPE_0;
static f32 environment_speed_ratio = 1.0f;    // Set to 0.5f in water, 1.0f otherwise. Influences different speed values.
static f32 environment_p_speed_ratio = 1.0f; // Inverse of `environment_speed_ratio` (1.0f / environment_speed_ratio)
static u32 wall_code = 0;
static u32 slide_power_flag = CONVEYOR_SPEED_DISABLED;
static s16 slide_power_type = false;
static s16 slide_power_angle = 0;
static f32 to_ground_y = 0.0f;
static s32 ground_auto_jump_type = FLOOR_PROPERTY_0; // floor property from the previous frame
static s32 wall_distance_angle_y = 0;
static s32 action_wall_distance_angle_y = 0;
static s16 ground_shape_angle_x = 0;
static s32 item_key = false; // When true, the current held item is used. Is reset to false every frame.
static s32 item_also_key = false; // Indicates if the button for the current held item is held down.

static u16 stop_voice_SE_flag[] = {
    NA_SE_VO_LI_SWEAT,
    NA_SE_VO_LI_SNEEZE,
    NA_SE_VO_LI_RELAX,
    NA_SE_VO_LI_FALL_L,
};

#include "z_player_item.inc.c"

#define GET_PLAYER_ANIM(group, type) link_anime_table[group * PLAYER_ANIMTYPE_MAX + type]

static LinkAnimationHeader* link_anime_table[PLAYER_ANIMGROUP_MAX * PLAYER_ANIMTYPE_MAX] = {
    /* PLAYER_ANIMGROUP_wait */
    &gPlayerAnim_link_normal_wait_free,
    &gPlayerAnim_link_normal_wait,
    &gPlayerAnim_link_normal_wait,
    &gPlayerAnim_link_fighter_wait_long,
    &gPlayerAnim_link_normal_wait_free,
    &gPlayerAnim_link_normal_wait_free,
    /* PLAYER_ANIMGROUP_walk */
    &gPlayerAnim_link_normal_walk_free,
    &gPlayerAnim_link_normal_walk,
    &gPlayerAnim_link_normal_walk,
    &gPlayerAnim_link_fighter_walk_long,
    &gPlayerAnim_link_normal_walk_free,
    &gPlayerAnim_link_normal_walk_free,
    /* PLAYER_ANIMGROUP_run */
    &gPlayerAnim_link_normal_run_free,
    &gPlayerAnim_link_fighter_run,
    &gPlayerAnim_link_normal_run,
    &gPlayerAnim_link_fighter_run_long,
    &gPlayerAnim_link_normal_run_free,
    &gPlayerAnim_link_normal_run_free,
    /* PLAYER_ANIMGROUP_damage_run */
    &gPlayerAnim_link_normal_damage_run_free,
    &gPlayerAnim_link_fighter_damage_run,
    &gPlayerAnim_link_normal_damage_run_free,
    &gPlayerAnim_link_fighter_damage_run_long,
    &gPlayerAnim_link_normal_damage_run_free,
    &gPlayerAnim_link_normal_damage_run_free,
    /* PLAYER_ANIMGROUP_heavy_run */
    &gPlayerAnim_link_normal_heavy_run_free,
    &gPlayerAnim_link_normal_heavy_run,
    &gPlayerAnim_link_normal_heavy_run_free,
    &gPlayerAnim_link_fighter_heavy_run_long,
    &gPlayerAnim_link_normal_heavy_run_free,
    &gPlayerAnim_link_normal_heavy_run_free,
    /* PLAYER_ANIMGROUP_waitL */
    &gPlayerAnim_link_normal_waitL_free,
    &gPlayerAnim_link_anchor_waitL,
    &gPlayerAnim_link_anchor_waitL,
    &gPlayerAnim_link_fighter_waitL_long,
    &gPlayerAnim_link_normal_waitL_free,
    &gPlayerAnim_link_normal_waitL_free,
    /* PLAYER_ANIMGROUP_waitR */
    &gPlayerAnim_link_normal_waitR_free,
    &gPlayerAnim_link_anchor_waitR,
    &gPlayerAnim_link_anchor_waitR,
    &gPlayerAnim_link_fighter_waitR_long,
    &gPlayerAnim_link_normal_waitR_free,
    &gPlayerAnim_link_normal_waitR_free,
    /* PLAYER_ANIMGROUP_wait2waitR */
    &gPlayerAnim_link_fighter_wait2waitR_long,
    &gPlayerAnim_link_normal_wait2waitR,
    &gPlayerAnim_link_normal_wait2waitR,
    &gPlayerAnim_link_fighter_wait2waitR_long,
    &gPlayerAnim_link_fighter_wait2waitR_long,
    &gPlayerAnim_link_fighter_wait2waitR_long,
    /* PLAYER_ANIMGROUP_normal2fighter */
    &gPlayerAnim_link_normal_normal2fighter_free,
    &gPlayerAnim_link_fighter_normal2fighter,
    &gPlayerAnim_link_fighter_normal2fighter,
    &gPlayerAnim_link_normal_normal2fighter_free,
    &gPlayerAnim_link_normal_normal2fighter_free,
    &gPlayerAnim_link_normal_normal2fighter_free,
    /* PLAYER_ANIMGROUP_doorA_free */
    &gPlayerAnim_link_demo_doorA_link_free,
    &gPlayerAnim_link_demo_doorA_link,
    &gPlayerAnim_link_demo_doorA_link,
    &gPlayerAnim_link_demo_doorA_link_free,
    &gPlayerAnim_link_demo_doorA_link_free,
    &gPlayerAnim_link_demo_doorA_link_free,
    /* PLAYER_ANIMGROUP_doorA */
    &gPlayerAnim_clink_demo_doorA_link,
    &gPlayerAnim_clink_demo_doorA_link,
    &gPlayerAnim_clink_demo_doorA_link,
    &gPlayerAnim_clink_demo_doorA_link,
    &gPlayerAnim_clink_demo_doorA_link,
    &gPlayerAnim_clink_demo_doorA_link,
    /* PLAYER_ANIMGROUP_doorB_free */
    &gPlayerAnim_link_demo_doorB_link_free,
    &gPlayerAnim_link_demo_doorB_link,
    &gPlayerAnim_link_demo_doorB_link,
    &gPlayerAnim_link_demo_doorB_link_free,
    &gPlayerAnim_link_demo_doorB_link_free,
    &gPlayerAnim_link_demo_doorB_link_free,
    /* PLAYER_ANIMGROUP_doorB */
    &gPlayerAnim_clink_demo_doorB_link,
    &gPlayerAnim_clink_demo_doorB_link,
    &gPlayerAnim_clink_demo_doorB_link,
    &gPlayerAnim_clink_demo_doorB_link,
    &gPlayerAnim_clink_demo_doorB_link,
    &gPlayerAnim_clink_demo_doorB_link,
    /* PLAYER_ANIMGROUP_carryB */
    &gPlayerAnim_link_normal_carryB_free,
    &gPlayerAnim_link_normal_carryB,
    &gPlayerAnim_link_normal_carryB,
    &gPlayerAnim_link_normal_carryB_free,
    &gPlayerAnim_link_normal_carryB_free,
    &gPlayerAnim_link_normal_carryB_free,
    /* PLAYER_ANIMGROUP_landing */
    &gPlayerAnim_link_normal_landing_free,
    &gPlayerAnim_link_normal_landing,
    &gPlayerAnim_link_normal_landing,
    &gPlayerAnim_link_normal_landing_free,
    &gPlayerAnim_link_normal_landing_free,
    &gPlayerAnim_link_normal_landing_free,
    /* PLAYER_ANIMGROUP_short_landing */
    &gPlayerAnim_link_normal_short_landing_free,
    &gPlayerAnim_link_normal_short_landing,
    &gPlayerAnim_link_normal_short_landing,
    &gPlayerAnim_link_normal_short_landing_free,
    &gPlayerAnim_link_normal_short_landing_free,
    &gPlayerAnim_link_normal_short_landing_free,
    /* PLAYER_ANIMGROUP_landing_roll */
    &gPlayerAnim_link_normal_landing_roll_free,
    &gPlayerAnim_link_normal_landing_roll,
    &gPlayerAnim_link_normal_landing_roll,
    &gPlayerAnim_link_fighter_landing_roll_long,
    &gPlayerAnim_link_normal_landing_roll_free,
    &gPlayerAnim_link_normal_landing_roll_free,
    /* PLAYER_ANIMGROUP_hip_down */
    &gPlayerAnim_link_normal_hip_down_free,
    &gPlayerAnim_link_normal_hip_down,
    &gPlayerAnim_link_normal_hip_down,
    &gPlayerAnim_link_normal_hip_down_long,
    &gPlayerAnim_link_normal_hip_down_free,
    &gPlayerAnim_link_normal_hip_down_free,
    /* PLAYER_ANIMGROUP_walk_endL */
    &gPlayerAnim_link_normal_walk_endL_free,
    &gPlayerAnim_link_normal_walk_endL,
    &gPlayerAnim_link_normal_walk_endL,
    &gPlayerAnim_link_fighter_walk_endL_long,
    &gPlayerAnim_link_normal_walk_endL_free,
    &gPlayerAnim_link_normal_walk_endL_free,
    /* PLAYER_ANIMGROUP_walk_endR */
    &gPlayerAnim_link_normal_walk_endR_free,
    &gPlayerAnim_link_normal_walk_endR,
    &gPlayerAnim_link_normal_walk_endR,
    &gPlayerAnim_link_fighter_walk_endR_long,
    &gPlayerAnim_link_normal_walk_endR_free,
    &gPlayerAnim_link_normal_walk_endR_free,
    /* PLAYER_ANIMGROUP_defense */
    &gPlayerAnim_link_normal_defense_free,
    &gPlayerAnim_link_normal_defense,
    &gPlayerAnim_link_normal_defense,
    &gPlayerAnim_link_normal_defense_free,
    &gPlayerAnim_link_bow_defense,
    &gPlayerAnim_link_normal_defense_free,
    /* PLAYER_ANIMGROUP_defense_wait */
    &gPlayerAnim_link_normal_defense_wait_free,
    &gPlayerAnim_link_normal_defense_wait,
    &gPlayerAnim_link_normal_defense_wait,
    &gPlayerAnim_link_normal_defense_wait_free,
    &gPlayerAnim_link_bow_defense_wait,
    &gPlayerAnim_link_normal_defense_wait_free,
    /* PLAYER_ANIMGROUP_defense_end */
    &gPlayerAnim_link_normal_defense_end_free,
    &gPlayerAnim_link_normal_defense_end,
    &gPlayerAnim_link_normal_defense_end,
    &gPlayerAnim_link_normal_defense_end_free,
    &gPlayerAnim_link_normal_defense_end_free,
    &gPlayerAnim_link_normal_defense_end_free,
    /* PLAYER_ANIMGROUP_side_walk */
    &gPlayerAnim_link_normal_side_walk_free,
    &gPlayerAnim_link_normal_side_walk,
    &gPlayerAnim_link_normal_side_walk,
    &gPlayerAnim_link_fighter_side_walk_long,
    &gPlayerAnim_link_normal_side_walk_free,
    &gPlayerAnim_link_normal_side_walk_free,
    /* PLAYER_ANIMGROUP_side_walkL */
    &gPlayerAnim_link_normal_side_walkL_free,
    &gPlayerAnim_link_anchor_side_walkL,
    &gPlayerAnim_link_anchor_side_walkL,
    &gPlayerAnim_link_fighter_side_walkL_long,
    &gPlayerAnim_link_normal_side_walkL_free,
    &gPlayerAnim_link_normal_side_walkL_free,
    /* PLAYER_ANIMGROUP_side_walkR */
    &gPlayerAnim_link_normal_side_walkR_free,
    &gPlayerAnim_link_anchor_side_walkR,
    &gPlayerAnim_link_anchor_side_walkR,
    &gPlayerAnim_link_fighter_side_walkR_long,
    &gPlayerAnim_link_normal_side_walkR_free,
    &gPlayerAnim_link_normal_side_walkR_free,
    /* PLAYER_ANIMGROUP_45_turn */
    &gPlayerAnim_link_normal_45_turn_free,
    &gPlayerAnim_link_normal_45_turn,
    &gPlayerAnim_link_normal_45_turn,
    &gPlayerAnim_link_normal_45_turn_free,
    &gPlayerAnim_link_normal_45_turn_free,
    &gPlayerAnim_link_normal_45_turn_free,
    /* PLAYER_ANIMGROUP_waitL2wait */
    &gPlayerAnim_link_fighter_waitL2wait_long,
    &gPlayerAnim_link_normal_waitL2wait,
    &gPlayerAnim_link_normal_waitL2wait,
    &gPlayerAnim_link_fighter_waitL2wait_long,
    &gPlayerAnim_link_fighter_waitL2wait_long,
    &gPlayerAnim_link_fighter_waitL2wait_long,
    /* PLAYER_ANIMGROUP_waitR2wait */
    &gPlayerAnim_link_fighter_waitR2wait_long,
    &gPlayerAnim_link_normal_waitR2wait,
    &gPlayerAnim_link_normal_waitR2wait,
    &gPlayerAnim_link_fighter_waitR2wait_long,
    &gPlayerAnim_link_fighter_waitR2wait_long,
    &gPlayerAnim_link_fighter_waitR2wait_long,
    /* PLAYER_ANIMGROUP_throw */
    &gPlayerAnim_link_normal_throw_free,
    &gPlayerAnim_link_normal_throw,
    &gPlayerAnim_link_normal_throw,
    &gPlayerAnim_link_normal_throw_free,
    &gPlayerAnim_link_normal_throw_free,
    &gPlayerAnim_link_normal_throw_free,
    /* PLAYER_ANIMGROUP_put */
    &gPlayerAnim_link_normal_put_free,
    &gPlayerAnim_link_normal_put,
    &gPlayerAnim_link_normal_put,
    &gPlayerAnim_link_normal_put_free,
    &gPlayerAnim_link_normal_put_free,
    &gPlayerAnim_link_normal_put_free,
    /* PLAYER_ANIMGROUP_back_walk */
    &gPlayerAnim_link_normal_back_walk,
    &gPlayerAnim_link_normal_back_walk,
    &gPlayerAnim_link_normal_back_walk,
    &gPlayerAnim_link_normal_back_walk,
    &gPlayerAnim_link_normal_back_walk,
    &gPlayerAnim_link_normal_back_walk,
    /* PLAYER_ANIMGROUP_check */
    &gPlayerAnim_link_normal_check_free,
    &gPlayerAnim_link_normal_check,
    &gPlayerAnim_link_normal_check,
    &gPlayerAnim_link_normal_check_free,
    &gPlayerAnim_link_normal_check_free,
    &gPlayerAnim_link_normal_check_free,
    /* PLAYER_ANIMGROUP_check_wait */
    &gPlayerAnim_link_normal_check_wait_free,
    &gPlayerAnim_link_normal_check_wait,
    &gPlayerAnim_link_normal_check_wait,
    &gPlayerAnim_link_normal_check_wait_free,
    &gPlayerAnim_link_normal_check_wait_free,
    &gPlayerAnim_link_normal_check_wait_free,
    /* PLAYER_ANIMGROUP_check_end */
    &gPlayerAnim_link_normal_check_end_free,
    &gPlayerAnim_link_normal_check_end,
    &gPlayerAnim_link_normal_check_end,
    &gPlayerAnim_link_normal_check_end_free,
    &gPlayerAnim_link_normal_check_end_free,
    &gPlayerAnim_link_normal_check_end_free,
    /* PLAYER_ANIMGROUP_pull_start */
    &gPlayerAnim_link_normal_pull_start_free,
    &gPlayerAnim_link_normal_pull_start,
    &gPlayerAnim_link_normal_pull_start,
    &gPlayerAnim_link_normal_pull_start_free,
    &gPlayerAnim_link_normal_pull_start_free,
    &gPlayerAnim_link_normal_pull_start_free,
    /* PLAYER_ANIMGROUP_pulling */
    &gPlayerAnim_link_normal_pulling_free,
    &gPlayerAnim_link_normal_pulling,
    &gPlayerAnim_link_normal_pulling,
    &gPlayerAnim_link_normal_pulling_free,
    &gPlayerAnim_link_normal_pulling_free,
    &gPlayerAnim_link_normal_pulling_free,
    /* PLAYER_ANIMGROUP_pull_end */
    &gPlayerAnim_link_normal_pull_end_free,
    &gPlayerAnim_link_normal_pull_end,
    &gPlayerAnim_link_normal_pull_end,
    &gPlayerAnim_link_normal_pull_end_free,
    &gPlayerAnim_link_normal_pull_end_free,
    &gPlayerAnim_link_normal_pull_end_free,
    /* PLAYER_ANIMGROUP_fall_up */
    &gPlayerAnim_link_normal_fall_up_free,
    &gPlayerAnim_link_normal_fall_up,
    &gPlayerAnim_link_normal_fall_up,
    &gPlayerAnim_link_normal_fall_up_free,
    &gPlayerAnim_link_normal_fall_up_free,
    &gPlayerAnim_link_normal_fall_up_free,
    /* PLAYER_ANIMGROUP_jump_climb_hold */
    &gPlayerAnim_link_normal_jump_climb_hold_free,
    &gPlayerAnim_link_normal_jump_climb_hold,
    &gPlayerAnim_link_normal_jump_climb_hold,
    &gPlayerAnim_link_normal_jump_climb_hold_free,
    &gPlayerAnim_link_normal_jump_climb_hold_free,
    &gPlayerAnim_link_normal_jump_climb_hold_free,
    /* PLAYER_ANIMGROUP_jump_climb_wait */
    &gPlayerAnim_link_normal_jump_climb_wait_free,
    &gPlayerAnim_link_normal_jump_climb_wait,
    &gPlayerAnim_link_normal_jump_climb_wait,
    &gPlayerAnim_link_normal_jump_climb_wait_free,
    &gPlayerAnim_link_normal_jump_climb_wait_free,
    &gPlayerAnim_link_normal_jump_climb_wait_free,
    /* PLAYER_ANIMGROUP_jump_climb_up */
    &gPlayerAnim_link_normal_jump_climb_up_free,
    &gPlayerAnim_link_normal_jump_climb_up,
    &gPlayerAnim_link_normal_jump_climb_up,
    &gPlayerAnim_link_normal_jump_climb_up_free,
    &gPlayerAnim_link_normal_jump_climb_up_free,
    &gPlayerAnim_link_normal_jump_climb_up_free,
    /* PLAYER_ANIMGROUP_down_slope_slip_end */
    &gPlayerAnim_link_normal_down_slope_slip_end_free,
    &gPlayerAnim_link_normal_down_slope_slip_end,
    &gPlayerAnim_link_normal_down_slope_slip_end,
    &gPlayerAnim_link_normal_down_slope_slip_end_long,
    &gPlayerAnim_link_normal_down_slope_slip_end_free,
    &gPlayerAnim_link_normal_down_slope_slip_end_free,
    /* PLAYER_ANIMGROUP_up_slope_slip_end */
    &gPlayerAnim_link_normal_up_slope_slip_end_free,
    &gPlayerAnim_link_normal_up_slope_slip_end,
    &gPlayerAnim_link_normal_up_slope_slip_end,
    &gPlayerAnim_link_normal_up_slope_slip_end_long,
    &gPlayerAnim_link_normal_up_slope_slip_end_free,
    &gPlayerAnim_link_normal_up_slope_slip_end_free,
    /* PLAYER_ANIMGROUP_nwait */
    &gPlayerAnim_sude_nwait,
    &gPlayerAnim_lkt_nwait,
    &gPlayerAnim_lkt_nwait,
    &gPlayerAnim_sude_nwait,
    &gPlayerAnim_sude_nwait,
    &gPlayerAnim_sude_nwait,
};

static LinkAnimationHeader* anchor_jump_anm_data[][3] = {
    { &gPlayerAnim_link_fighter_front_jump, &gPlayerAnim_link_fighter_front_jump_end,
      &gPlayerAnim_link_fighter_front_jump_endR },
    { &gPlayerAnim_link_fighter_Lside_jump, &gPlayerAnim_link_fighter_Lside_jump_end,
      &gPlayerAnim_link_fighter_Lside_jump_endL },
    { &gPlayerAnim_link_fighter_backturn_jump, &gPlayerAnim_link_fighter_backturn_jump_end,
      &gPlayerAnim_link_fighter_backturn_jump_endR },
    { &gPlayerAnim_link_fighter_Rside_jump, &gPlayerAnim_link_fighter_Rside_jump_end,
      &gPlayerAnim_link_fighter_Rside_jump_endR },
};

typedef enum FidgetType {
    /* 0x00 */ FIDGET_LOOK_AROUND, // ROOM_ENV_DEFAULT
    /* 0x01 */ FIDGET_COLD,        // ROOM_ENV_COLD
    /* 0x02 */ FIDGET_WARM,        // ROOM_ENV_WARM
    /* 0x03 */ FIDGET_HOT,         // ROOM_ENV_HOT (same animations as FIDGET_WARM)
    /* 0x04 */ FIDGET_STRETCH_1,   // ROOM_ENV_UNK_STRETCH_1
    /* 0x05 */ FIDGET_STRETCH_2,   // ROOM_ENV_UNK_STRETCH_1 (same animations as FIDGET_STRETCH_1)
    /* 0x06 */ FIDGET_STRETCH_3,   // ROOM_ENV_UNK_STRETCH_1 (same animations as FIDGET_STRETCH_1)
    /* 0x07 */ FIDGET_CRIT_HEALTH_START,
    /* 0x08 */ FIDGET_CRIT_HEALTH_LOOP,
    /* 0x09 */ FIDGET_SWORD_SWING,
    /* 0x0A */ FIDGET_ADJUST_TUNIC,
    /* 0x0B */ FIDGET_TAP_FEET,
    /* 0x0C */ FIDGET_ADJUST_SHIELD,
    /* 0x0D */ FIDGET_SWORD_SWING_TWO_HAND
} FidgetType;

static LinkAnimationHeader* weather_wait_anm_data[][2] = {
    // FIDGET_LOOK_AROUND
    { &gPlayerAnim_link_normal_wait_typeA_20f, &gPlayerAnim_link_normal_waitF_typeA_20f },

    // FIDGET_COLD
    { &gPlayerAnim_link_normal_wait_typeC_20f, &gPlayerAnim_link_normal_waitF_typeC_20f },

    // FIDGET_WARM
    { &gPlayerAnim_link_normal_wait_typeB_20f, &gPlayerAnim_link_normal_waitF_typeB_20f },

    // FIDGET_HOT
    { &gPlayerAnim_link_normal_wait_typeB_20f, &gPlayerAnim_link_normal_waitF_typeB_20f },

    // FIDGET_STRETCH_1
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },

    // FIDGET_STRETCH_2
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },

    // FIDGET_STRETCH_3
    { &gPlayerAnim_link_wait_typeD_20f, &gPlayerAnim_link_waitF_typeD_20f },

    // FIDGET_CRIT_HEALTH_START
    { &gPlayerAnim_link_wait_heat1_20f, &gPlayerAnim_link_waitF_heat1_20f },

    // FIDGET_CRIT_HEALTH_LOOP
    { &gPlayerAnim_link_wait_heat2_20f, &gPlayerAnim_link_waitF_heat2_20f },

    // FIDGET_SWORD_SWING
    { &gPlayerAnim_link_wait_itemD1_20f, &gPlayerAnim_link_wait_itemD1_20f },

    // FIDGET_ADJUST_TUNIC
    { &gPlayerAnim_link_wait_itemA_20f, &gPlayerAnim_link_waitF_itemA_20f },

    // FIDGET_TAP_FEET
    { &gPlayerAnim_link_wait_itemB_20f, &gPlayerAnim_link_waitF_itemB_20f },

    // FIDGET_ADJUST_SHIELD
    { &gPlayerAnim_link_wait_itemC_20f, &gPlayerAnim_link_wait_itemC_20f },

    // FIDGET_SWORD_SWING_TWO_HAND
    { &gPlayerAnim_link_wait_itemD2_20f, &gPlayerAnim_link_wait_itemD2_20f }
};

static AnimSfxEntry cold_SE_set_status[] = {
    { NA_SE_VO_LI_SNEEZE, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 8) },
};

static AnimSfxEntry hot_SE_set_status[] = {
    { NA_SE_VO_LI_SWEAT, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 18) },
};

static AnimSfxEntry heat1_SE_set_status[] = {
    { NA_SE_VO_LI_BREATH_REST, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 13) },
};

static AnimSfxEntry heat2_SE_set_status[] = {
    { NA_SE_VO_LI_BREATH_REST, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 10) },
};

static AnimSfxEntry itemA_SE_set_status[] = {
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 44) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 48) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 52) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 56) },
    { NA_SE_PL_CALM_HIT, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 60) },
};

static AnimSfxEntry itemB_SE_set_status[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 25) }, { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 30) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 44) }, { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 48) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 52) }, { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 56) },
};

static AnimSfxEntry itemC_SE_set_status[] = {
    { NA_SE_IT_SHIELD_POSTURE, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 16) },
    { NA_SE_IT_SHIELD_POSTURE, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 20) },
    { NA_SE_IT_SHIELD_POSTURE, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 70) },
};

static AnimSfxEntry itemD1_SE_set_status[] = {
    { NA_SE_IT_HAMMER_SWING, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 10) },
    { NA_SE_VO_LI_AUTO_JUMP, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 10) },
    { NA_SE_IT_SWORD_SWING, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 22) },
    { NA_SE_VO_LI_SWORD_N, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 22) },
};

static AnimSfxEntry itemD2_SE_set_status[] = {
    { NA_SE_IT_SWORD_SWING, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 39) },
    { NA_SE_VO_LI_SWORD_N, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 39) },
};

static AnimSfxEntry typeD_SE_set_status[] = {
    { NA_SE_VO_LI_RELAX, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 20) },
};

typedef enum FidgetAnimSfxType {
    /* 0x0 */ FIDGET_ANIMSFX_NONE,
    /* 0x1 */ FIDGET_ANIMSFX_SNEEZE,
    /* 0x2 */ FIDGET_ANIMSFX_SWEAT,
    /* 0x3 */ FIDGET_ANIMSFX_CRIT_HEALTH_START,
    /* 0x4 */ FIDGET_ANIMSFX_CRIT_HEALTH_LOOP,
    /* 0x5 */ FIDGET_ANIMSFX_TUNIC,
    /* 0x6 */ FIDGET_ANIMSFX_TAP_FEET,
    /* 0x7 */ FIDGET_ANIMSFX_SHIELD,
    /* 0x8 */ FIDGET_ANIMSFX_SWORD,
    /* 0x9 */ FIDGET_ANIMSFX_SWORD_TWO_HAND,
    /* 0xA */ FIDGET_ANIMSFX_STRETCH
} FidgetAnimSfxType;

static AnimSfxEntry* SE_set_status_data[] = {
    cold_SE_set_status,          // FIDGET_ANIMSFX_SNEEZE
    hot_SE_set_status,           // FIDGET_ANIMSFX_SWEAT
    heat1_SE_set_status, // FIDGET_ANIMSFX_CRIT_HEALTH_START
    heat2_SE_set_status,  // FIDGET_ANIMSFX_CRIT_HEALTH_LOOP
    itemA_SE_set_status,           // FIDGET_ANIMSFX_TUNIC
    itemB_SE_set_status,         // FIDGET_ANIMSFX_TAP_FEET
    itemC_SE_set_status,          // FIDGET_ANIMSFX_SHIELD
    itemD1_SE_set_status,           // FIDGET_ANIMSFX_SWORD
    itemD2_SE_set_status,    // FIDGET_ANIMSFX_SWORD_TWO_HAND
    typeD_SE_set_status,         // FIDGET_ANIMSFX_STRETCH
    NULL,                          // unused entry
};

/**
 * The indices in this array correspond 1 to 1 with the entries of weather_wait_anm_data.
 * There is also an extra FIDGET_ANIMSFX_NONE at the end that doesn't correspond to any animation.
 */
static u8 SE_set_status_table[] = {
    FIDGET_ANIMSFX_NONE,              // FIDGET_LOOK_AROUND
    FIDGET_ANIMSFX_NONE,              // FIDGET_LOOK_AROUND (sword/shield in hand)
    FIDGET_ANIMSFX_SNEEZE,            // FIDGET_COLD
    FIDGET_ANIMSFX_SNEEZE,            // FIDGET_COLD (sword/shield in hand)
    FIDGET_ANIMSFX_SWEAT,             // FIDGET_WARM
    FIDGET_ANIMSFX_SWEAT,             // FIDGET_WARM (sword/shield in hand)
    FIDGET_ANIMSFX_SWEAT,             // FIDGET_HOT
    FIDGET_ANIMSFX_SWEAT,             // FIDGET_HOT (sword/shield in hand)
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_1
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_1 (sword/shield in hand)
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_2
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_2 (sword/shield in hand)
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_3
    FIDGET_ANIMSFX_STRETCH,           // FIDGET_STRETCH_3 (sword/shield in hand)
    FIDGET_ANIMSFX_CRIT_HEALTH_START, // FIDGET_CRIT_HEALTH_START
    FIDGET_ANIMSFX_CRIT_HEALTH_START, // FIDGET_CRIT_HEALTH_START (sword/shield in hand)
    FIDGET_ANIMSFX_CRIT_HEALTH_LOOP,  // FIDGET_CRIT_HEALTH_LOOP
    FIDGET_ANIMSFX_CRIT_HEALTH_LOOP,  // FIDGET_CRIT_HEALTH_LOOP (sword/shield in hand)
    FIDGET_ANIMSFX_SWORD,             // FIDGET_SWORD_SWING
    FIDGET_ANIMSFX_SWORD,             // FIDGET_SWORD_SWING (sword/shield in hand)
    FIDGET_ANIMSFX_TUNIC,             // FIDGET_ADJUST_TUNIC
    FIDGET_ANIMSFX_TUNIC,             // FIDGET_ADJUST_TUNIC (sword/shield in hand)
    FIDGET_ANIMSFX_TAP_FEET,          // FIDGET_TAP_FEET
    FIDGET_ANIMSFX_TAP_FEET,          // FIDGET_TAP_FEET (sword/shield in hand)
    FIDGET_ANIMSFX_SHIELD,            // FIDGET_ADJUST_SHIELD
    FIDGET_ANIMSFX_SHIELD,            // FIDGET_ADJUST_SHIELD (sword/shield in hand)
    FIDGET_ANIMSFX_SWORD_TWO_HAND,    // FIDGET_SWORD_SWING_TWO_HAND
    FIDGET_ANIMSFX_SWORD_TWO_HAND,    // FIDGET_SWORD_SWING_TWO_HAND (sword/shield in hand)
    FIDGET_ANIMSFX_NONE,              // unused, doesnt correspond to any animation
};

#include "z_player_ability.inc.c"

static s32 (*uperbody_process[])(Player* this, PlayState* play) = {
    uperbody_non,                 // PLAYER_IA_NONE
    uperbody_non,                 // PLAYER_IA_SWORD_CS
    uperbody_non,                 // PLAYER_IA_FISHING_POLE
    uperbody_nom2figh_wait,      // PLAYER_IA_SWORD_MASTER
    uperbody_nom2figh_wait,      // PLAYER_IA_SWORD_KOKIRI
    uperbody_nom2figh_wait,      // PLAYER_IA_SWORD_BIGGORON
    uperbody_non,                 // PLAYER_IA_DEKU_STICK
    uperbody_non,                 // PLAYER_IA_HAMMER
    uperbody_bow_wait,                 // PLAYER_IA_BOW
    uperbody_bow_wait,                 // PLAYER_IA_BOW_FIRE
    uperbody_bow_wait,                 // PLAYER_IA_BOW_ICE
    uperbody_bow_wait,                 // PLAYER_IA_BOW_LIGHT
    uperbody_bow_wait,                 // PLAYER_IA_BOW_0C
    uperbody_bow_wait,                 // PLAYER_IA_BOW_0D
    uperbody_bow_wait,                 // PLAYER_IA_BOW_0E
    uperbody_bow_wait,                 // PLAYER_IA_SLINGSHOT
    uperbody_bow_wait,                 // PLAYER_IA_HOOKSHOT
    uperbody_bow_wait,                 // PLAYER_IA_LONGSHOT
    uperbody_carry, // PLAYER_IA_BOMB
    uperbody_carry, // PLAYER_IA_BOMBCHU
    uperbody_boom_wait,                 // PLAYER_IA_BOOMERANG
    uperbody_non,                 // PLAYER_IA_MAGIC_SPELL_15
    uperbody_non,                 // PLAYER_IA_MAGIC_SPELL_16
    uperbody_non,                 // PLAYER_IA_MAGIC_SPELL_17
    uperbody_non,                 // PLAYER_IA_FARORES_WIND
    uperbody_non,                 // PLAYER_IA_NAYRUS_LOVE
    uperbody_non,                 // PLAYER_IA_DINS_FIRE
    uperbody_non,                 // PLAYER_IA_DEKU_NUT
    uperbody_non,                 // PLAYER_IA_OCARINA_FAIRY
    uperbody_non,                 // PLAYER_IA_OCARINA_OF_TIME
    uperbody_non,                 // PLAYER_IA_BOTTLE
    uperbody_non,                 // PLAYER_IA_BOTTLE_FISH
    uperbody_non,                 // PLAYER_IA_BOTTLE_FIRE
    uperbody_non,                 // PLAYER_IA_BOTTLE_BUG
    uperbody_non,                 // PLAYER_IA_BOTTLE_POE
    uperbody_non,                 // PLAYER_IA_BOTTLE_BIG_POE
    uperbody_non,                 // PLAYER_IA_BOTTLE_RUTOS_LETTER
    uperbody_non,                 // PLAYER_IA_BOTTLE_POTION_RED
    uperbody_non,                 // PLAYER_IA_BOTTLE_POTION_BLUE
    uperbody_non,                 // PLAYER_IA_BOTTLE_POTION_GREEN
    uperbody_non,                 // PLAYER_IA_BOTTLE_MILK_FULL
    uperbody_non,                 // PLAYER_IA_BOTTLE_MILK_HALF
    uperbody_non,                 // PLAYER_IA_BOTTLE_FAIRY
    uperbody_non,                 // PLAYER_IA_ZELDAS_LETTER
    uperbody_non,                 // PLAYER_IA_WEIRD_EGG
    uperbody_non,                 // PLAYER_IA_CHICKEN
    uperbody_non,                 // PLAYER_IA_MAGIC_BEAN
    uperbody_non,                 // PLAYER_IA_POCKET_EGG
    uperbody_non,                 // PLAYER_IA_POCKET_CUCCO
    uperbody_non,                 // PLAYER_IA_COJIRO
    uperbody_non,                 // PLAYER_IA_ODD_MUSHROOM
    uperbody_non,                 // PLAYER_IA_ODD_POTION
    uperbody_non,                 // PLAYER_IA_POACHERS_SAW
    uperbody_non,                 // PLAYER_IA_BROKEN_GORONS_SWORD
    uperbody_non,                 // PLAYER_IA_PRESCRIPTION
    uperbody_non,                 // PLAYER_IA_FROG
    uperbody_non,                 // PLAYER_IA_EYEDROPS
    uperbody_non,                 // PLAYER_IA_CLAIM_CHECK
    uperbody_non,                 // PLAYER_IA_MASK_KEATON
    uperbody_non,                 // PLAYER_IA_MASK_SKULL
    uperbody_non,                 // PLAYER_IA_MASK_SPOOKY
    uperbody_non,                 // PLAYER_IA_MASK_BUNNY_HOOD
    uperbody_non,                 // PLAYER_IA_MASK_GORON
    uperbody_non,                 // PLAYER_IA_MASK_ZORA
    uperbody_non,                 // PLAYER_IA_MASK_GERUDO
    uperbody_non,                 // PLAYER_IA_MASK_TRUTH
    uperbody_non,                 // PLAYER_IA_LENS_OF_TRUTH
};

static void (*ability_A_item_init[])(PlayState* play, Player* this) = {
    ability_A_item_non_init,        // PLAYER_IA_NONE
    ability_A_item_non_init,        // PLAYER_IA_SWORD_CS
    ability_A_item_non_init,        // PLAYER_IA_FISHING_POLE
    ability_A_item_non_init,        // PLAYER_IA_SWORD_MASTER
    ability_A_item_non_init,        // PLAYER_IA_SWORD_KOKIRI
    ability_A_item_non_init,        // PLAYER_IA_SWORD_BIGGORON
    ability_A_item_stick_init,      // PLAYER_IA_DEKU_STICK
    ability_A_item_hammer_init,         // PLAYER_IA_HAMMER
    ability_A_item_bow_init, // PLAYER_IA_BOW
    ability_A_item_bow_init, // PLAYER_IA_BOW_FIRE
    ability_A_item_bow_init, // PLAYER_IA_BOW_ICE
    ability_A_item_bow_init, // PLAYER_IA_BOW_LIGHT
    ability_A_item_bow_init, // PLAYER_IA_BOW_0C
    ability_A_item_bow_init, // PLAYER_IA_BOW_0D
    ability_A_item_bow_init, // PLAYER_IA_BOW_0E
    ability_A_item_bow_init, // PLAYER_IA_SLINGSHOT
    ability_A_item_hook_init,       // PLAYER_IA_HOOKSHOT
    ability_A_item_hook_init,       // PLAYER_IA_LONGSHOT
    ability_A_item_bomb_init,      // PLAYER_IA_BOMB
    ability_A_item_bomb_init,      // PLAYER_IA_BOMBCHU
    ability_A_item_boom_init,      // PLAYER_IA_BOOMERANG
    ability_A_item_non_init,        // PLAYER_IA_MAGIC_SPELL_15
    ability_A_item_non_init,        // PLAYER_IA_MAGIC_SPELL_16
    ability_A_item_non_init,        // PLAYER_IA_MAGIC_SPELL_17
    ability_A_item_non_init,        // PLAYER_IA_FARORES_WIND
    ability_A_item_non_init,        // PLAYER_IA_NAYRUS_LOVE
    ability_A_item_non_init,        // PLAYER_IA_DINS_FIRE
    ability_A_item_non_init,        // PLAYER_IA_DEKU_NUT
    ability_A_item_non_init,        // PLAYER_IA_OCARINA_FAIRY
    ability_A_item_non_init,        // PLAYER_IA_OCARINA_OF_TIME
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_FISH
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_FIRE
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_BUG
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_POE
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_BIG_POE
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_RUTOS_LETTER
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_POTION_RED
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_POTION_BLUE
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_POTION_GREEN
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_MILK_FULL
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_MILK_HALF
    ability_A_item_non_init,        // PLAYER_IA_BOTTLE_FAIRY
    ability_A_item_non_init,        // PLAYER_IA_ZELDAS_LETTER
    ability_A_item_non_init,        // PLAYER_IA_WEIRD_EGG
    ability_A_item_non_init,        // PLAYER_IA_CHICKEN
    ability_A_item_non_init,        // PLAYER_IA_MAGIC_BEAN
    ability_A_item_non_init,        // PLAYER_IA_POCKET_EGG
    ability_A_item_non_init,        // PLAYER_IA_POCKET_CUCCO
    ability_A_item_non_init,        // PLAYER_IA_COJIRO
    ability_A_item_non_init,        // PLAYER_IA_ODD_MUSHROOM
    ability_A_item_non_init,        // PLAYER_IA_ODD_POTION
    ability_A_item_non_init,        // PLAYER_IA_POACHERS_SAW
    ability_A_item_non_init,        // PLAYER_IA_BROKEN_GORONS_SWORD
    ability_A_item_non_init,        // PLAYER_IA_PRESCRIPTION
    ability_A_item_non_init,        // PLAYER_IA_FROG
    ability_A_item_non_init,        // PLAYER_IA_EYEDROPS
    ability_A_item_non_init,        // PLAYER_IA_CLAIM_CHECK
    ability_A_item_non_init,        // PLAYER_IA_MASK_KEATON
    ability_A_item_non_init,        // PLAYER_IA_MASK_SKULL
    ability_A_item_non_init,        // PLAYER_IA_MASK_SPOOKY
    ability_A_item_non_init,        // PLAYER_IA_MASK_BUNNY_HOOD
    ability_A_item_non_init,        // PLAYER_IA_MASK_GORON
    ability_A_item_non_init,        // PLAYER_IA_MASK_ZORA
    ability_A_item_non_init,        // PLAYER_IA_MASK_GERUDO
    ability_A_item_non_init,        // PLAYER_IA_MASK_TRUTH
    ability_A_item_non_init,        // PLAYER_IA_LENS_OF_TRUTH
};

typedef enum ItemChangeType {
    /*  0 */ PLAYER_ITEM_CHG_0,
    /*  1 */ PLAYER_ITEM_CHG_1,
    /*  2 */ PLAYER_ITEM_CHG_2,
    /*  3 */ PLAYER_ITEM_CHG_3,
    /*  4 */ PLAYER_ITEM_CHG_4,
    /*  5 */ PLAYER_ITEM_CHG_5,
    /*  6 */ PLAYER_ITEM_CHG_6,
    /*  7 */ PLAYER_ITEM_CHG_7,
    /*  8 */ PLAYER_ITEM_CHG_8,
    /*  9 */ PLAYER_ITEM_CHG_9,
    /* 10 */ PLAYER_ITEM_CHG_10,
    /* 11 */ PLAYER_ITEM_CHG_11,
    /* 12 */ PLAYER_ITEM_CHG_12,
    /* 13 */ PLAYER_ITEM_CHG_13,
    /* 14 */ PLAYER_ITEM_CHG_MAX
} ItemChangeType;

static ItemChangeInfo item_change_data[PLAYER_ITEM_CHG_MAX] = {
    /* PLAYER_ITEM_CHG_0 */ { &gPlayerAnim_link_normal_free2free, 12 },
    /* PLAYER_ITEM_CHG_1 */ { &gPlayerAnim_link_normal_normal2fighter, 6 },
    /* PLAYER_ITEM_CHG_2 */ { &gPlayerAnim_link_hammer_normal2long, 8 },
    /* PLAYER_ITEM_CHG_3 */ { &gPlayerAnim_link_normal_normal2free, 8 },
    /* PLAYER_ITEM_CHG_4 */ { &gPlayerAnim_link_fighter_fighter2long, 8 },
    /* PLAYER_ITEM_CHG_5 */ { &gPlayerAnim_link_normal_fighter2free, 10 },
    /* PLAYER_ITEM_CHG_6 */ { &gPlayerAnim_link_hammer_long2free, 7 },
    /* PLAYER_ITEM_CHG_7 */ { &gPlayerAnim_link_hammer_long2long, 11 },
    /* PLAYER_ITEM_CHG_8 */ { &gPlayerAnim_link_normal_free2free, 12 },
    /* PLAYER_ITEM_CHG_9 */ { &gPlayerAnim_link_normal_normal2bom, 4 },
    /* PLAYER_ITEM_CHG_10 */ { &gPlayerAnim_link_normal_long2bom, 4 },
    /* PLAYER_ITEM_CHG_11 */ { &gPlayerAnim_link_normal_free2bom, 4 },
    /* PLAYER_ITEM_CHG_12 */ { &gPlayerAnim_link_anchor_anchor2fighter, 5 },
    /* PLAYER_ITEM_CHG_13 */ { &gPlayerAnim_link_normal_free2freeB, 13 },
};

// Maps the appropriate ItemChangeType based on current and next animtype.
// A negative type value means the corresponding animation should be played in reverse.
static s8 item_change_table[PLAYER_ANIMTYPE_MAX][PLAYER_ANIMTYPE_MAX] = {
    { PLAYER_ITEM_CHG_8, -PLAYER_ITEM_CHG_5, -PLAYER_ITEM_CHG_3, -PLAYER_ITEM_CHG_6, PLAYER_ITEM_CHG_8,
      PLAYER_ITEM_CHG_11 },
    { PLAYER_ITEM_CHG_5, PLAYER_ITEM_CHG_0, -PLAYER_ITEM_CHG_1, PLAYER_ITEM_CHG_4, PLAYER_ITEM_CHG_5,
      PLAYER_ITEM_CHG_9 },
    { PLAYER_ITEM_CHG_3, PLAYER_ITEM_CHG_1, PLAYER_ITEM_CHG_0, PLAYER_ITEM_CHG_2, PLAYER_ITEM_CHG_3,
      PLAYER_ITEM_CHG_9 },
    { PLAYER_ITEM_CHG_6, -PLAYER_ITEM_CHG_4, -PLAYER_ITEM_CHG_2, PLAYER_ITEM_CHG_7, PLAYER_ITEM_CHG_6,
      PLAYER_ITEM_CHG_10 },
    { PLAYER_ITEM_CHG_8, -PLAYER_ITEM_CHG_5, -PLAYER_ITEM_CHG_3, -PLAYER_ITEM_CHG_6, PLAYER_ITEM_CHG_8,
      PLAYER_ITEM_CHG_11 },
    { PLAYER_ITEM_CHG_8, -PLAYER_ITEM_CHG_5, -PLAYER_ITEM_CHG_3, -PLAYER_ITEM_CHG_6, PLAYER_ITEM_CHG_8,
      PLAYER_ITEM_CHG_11 },
};

static ExplosiveInfo bom_status[] = {
    { ITEM_BOMB, ACTOR_EN_BOM },
    { ITEM_BOMBCHU, ACTOR_EN_BOM_CHU },
};

static struct_80854190 kiru_anime_status[PLAYER_MWA_MAX] = {
    /* PLAYER_MWA_FORWARD_SLASH_1H */
    { &gPlayerAnim_link_fighter_normal_kiru, &gPlayerAnim_link_fighter_normal_kiru_end,
      &gPlayerAnim_link_fighter_normal_kiru_endR, 1, 4 },
    /* PLAYER_MWA_FORWARD_SLASH_2H */
    { &gPlayerAnim_link_fighter_Lnormal_kiru, &gPlayerAnim_link_fighter_Lnormal_kiru_end,
      &gPlayerAnim_link_anchor_Lnormal_kiru_endR, 1, 4 },
    /* PLAYER_MWA_FORWARD_COMBO_1H */
    { &gPlayerAnim_link_fighter_normal_kiru_finsh, &gPlayerAnim_link_fighter_normal_kiru_finsh_end,
      &gPlayerAnim_link_anchor_normal_kiru_finsh_endR, 0, 5 },
    /* PLAYER_MWA_FORWARD_COMBO_2H */
    { &gPlayerAnim_link_fighter_Lnormal_kiru_finsh, &gPlayerAnim_link_fighter_Lnormal_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lnormal_kiru_finsh_endR, 1, 7 },
    /* PLAYER_MWA_RIGHT_SLASH_1H */
    { &gPlayerAnim_link_fighter_Lside_kiru, &gPlayerAnim_link_fighter_Lside_kiru_end,
      &gPlayerAnim_link_anchor_Lside_kiru_endR, 1, 4 },
    /* PLAYER_MWA_RIGHT_SLASH_2H */
    { &gPlayerAnim_link_fighter_LLside_kiru, &gPlayerAnim_link_fighter_LLside_kiru_end,
      &gPlayerAnim_link_anchor_LLside_kiru_endL, 0, 5 },
    /* PLAYER_MWA_RIGHT_COMBO_1H */
    { &gPlayerAnim_link_fighter_Lside_kiru_finsh, &gPlayerAnim_link_fighter_Lside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lside_kiru_finsh_endR, 2, 8 },
    /* PLAYER_MWA_RIGHT_COMBO_2H */
    { &gPlayerAnim_link_fighter_LLside_kiru_finsh, &gPlayerAnim_link_fighter_LLside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_LLside_kiru_finsh_endR, 3, 8 },
    /* PLAYER_MWA_LEFT_SLASH_1H */
    { &gPlayerAnim_link_fighter_Rside_kiru, &gPlayerAnim_link_fighter_Rside_kiru_end,
      &gPlayerAnim_link_anchor_Rside_kiru_endR, 0, 4 },
    /* PLAYER_MWA_LEFT_SLASH_2H */
    { &gPlayerAnim_link_fighter_LRside_kiru, &gPlayerAnim_link_fighter_LRside_kiru_end,
      &gPlayerAnim_link_anchor_LRside_kiru_endR, 0, 5 },
    /* PLAYER_MWA_LEFT_COMBO_1H */
    { &gPlayerAnim_link_fighter_Rside_kiru_finsh, &gPlayerAnim_link_fighter_Rside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Rside_kiru_finsh_endR, 0, 6 },
    /* PLAYER_MWA_LEFT_COMBO_2H */
    { &gPlayerAnim_link_fighter_LRside_kiru_finsh, &gPlayerAnim_link_fighter_LRside_kiru_finsh_end,
      &gPlayerAnim_link_anchor_LRside_kiru_finsh_endL, 1, 5 },
    /* PLAYER_MWA_STAB_1H */
    { &gPlayerAnim_link_fighter_pierce_kiru, &gPlayerAnim_link_fighter_pierce_kiru_end,
      &gPlayerAnim_link_anchor_pierce_kiru_endR, 0, 3 },
    /* PLAYER_MWA_STAB_2H */
    { &gPlayerAnim_link_fighter_Lpierce_kiru, &gPlayerAnim_link_fighter_Lpierce_kiru_end,
      &gPlayerAnim_link_anchor_Lpierce_kiru_endL, 0, 3 },
    /* PLAYER_MWA_STAB_COMBO_1H */
    { &gPlayerAnim_link_fighter_pierce_kiru_finsh, &gPlayerAnim_link_fighter_pierce_kiru_finsh_end,
      &gPlayerAnim_link_anchor_pierce_kiru_finsh_endR, 1, 9 },
    /* PLAYER_MWA_STAB_COMBO_2H */
    { &gPlayerAnim_link_fighter_Lpierce_kiru_finsh, &gPlayerAnim_link_fighter_Lpierce_kiru_finsh_end,
      &gPlayerAnim_link_anchor_Lpierce_kiru_finsh_endR, 1, 8 },
    /* PLAYER_MWA_FLIPSLASH_START */
    { &gPlayerAnim_link_fighter_jump_rollkiru, &gPlayerAnim_link_fighter_jump_kiru_finsh,
      &gPlayerAnim_link_fighter_jump_kiru_finsh, 1, 10 },
    /* PLAYER_MWA_JUMPSLASH_START */
    { &gPlayerAnim_link_fighter_Lpower_jump_kiru, &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit,
      &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit, 1, 11 },
    /* PLAYER_MWA_FLIPSLASH_FINISH */
    { &gPlayerAnim_link_fighter_jump_kiru_finsh, &gPlayerAnim_link_fighter_jump_kiru_finsh_end,
      &gPlayerAnim_link_fighter_jump_kiru_finsh_end, 1, 2 },
    /* PLAYER_MWA_JUMPSLASH_FINISH */
    { &gPlayerAnim_link_fighter_Lpower_jump_kiru_hit, &gPlayerAnim_link_fighter_Lpower_jump_kiru_end,
      &gPlayerAnim_link_fighter_Lpower_jump_kiru_end, 1, 2 },
    /* PLAYER_MWA_BACKSLASH_RIGHT */
    { &gPlayerAnim_link_fighter_turn_kiruR, &gPlayerAnim_link_fighter_turn_kiruR_end,
      &gPlayerAnim_link_fighter_turn_kiruR_end, 1, 5 },
    /* PLAYER_MWA_BACKSLASH_LEFT */
    { &gPlayerAnim_link_fighter_turn_kiruL, &gPlayerAnim_link_fighter_turn_kiruL_end,
      &gPlayerAnim_link_fighter_turn_kiruL_end, 1, 4 },
    /* PLAYER_MWA_HAMMER_FORWARD */
    { &gPlayerAnim_link_hammer_hit, &gPlayerAnim_link_hammer_hit_end, &gPlayerAnim_link_hammer_hit_endR, 3, 10 },
    /* PLAYER_MWA_HAMMER_SIDE */
    { &gPlayerAnim_link_hammer_side_hit, &gPlayerAnim_link_hammer_side_hit_end, &gPlayerAnim_link_hammer_side_hit_endR,
      2, 11 },
    /* PLAYER_MWA_SPIN_ATTACK_1H */
    { &gPlayerAnim_link_fighter_rolling_kiru, &gPlayerAnim_link_fighter_rolling_kiru_end,
      &gPlayerAnim_link_anchor_rolling_kiru_endR, 0, 12 },
    /* PLAYER_MWA_SPIN_ATTACK_2H */
    { &gPlayerAnim_link_fighter_Lrolling_kiru, &gPlayerAnim_link_fighter_Lrolling_kiru_end,
      &gPlayerAnim_link_anchor_Lrolling_kiru_endR, 0, 15 },
    /* PLAYER_MWA_BIG_SPIN_1H */
    { &gPlayerAnim_link_fighter_Wrolling_kiru, &gPlayerAnim_link_fighter_Wrolling_kiru_end,
      &gPlayerAnim_link_anchor_rolling_kiru_endR, 0, 16 },
    /* PLAYER_MWA_BIG_SPIN_2H */
    { &gPlayerAnim_link_fighter_Wrolling_kiru, &gPlayerAnim_link_fighter_Wrolling_kiru_end,
      &gPlayerAnim_link_anchor_Lrolling_kiru_endR, 0, 16 },
};

static LinkAnimationHeader* link_power_kiru_start_anm[] = {
    &gPlayerAnim_link_fighter_power_kiru_start,
    &gPlayerAnim_link_fighter_Lpower_kiru_start,
};

static LinkAnimationHeader* link_power_kiru_startL_anm[] = {
    &gPlayerAnim_link_fighter_power_kiru_startL,
    &gPlayerAnim_link_fighter_Lpower_kiru_start,
};

static LinkAnimationHeader* link_power_kiru_wait_anm[] = {
    &gPlayerAnim_link_fighter_power_kiru_wait,
    &gPlayerAnim_link_fighter_Lpower_kiru_wait,
};

static LinkAnimationHeader* link_power_kiru_wait_end_anm[] = {
    &gPlayerAnim_link_fighter_power_kiru_wait_end,
    &gPlayerAnim_link_fighter_Lpower_kiru_wait_end,
};

static LinkAnimationHeader* link_power_kiru_walk_anm[] = {
    &gPlayerAnim_link_fighter_power_kiru_walk,
    &gPlayerAnim_link_fighter_Lpower_kiru_walk,
};

static LinkAnimationHeader* link_power_kiru_side_walk_anm[] = {
    &gPlayerAnim_link_fighter_power_kiru_side_walk,
    &gPlayerAnim_link_fighter_Lpower_kiru_side_walk,
};

static u8 rolling_kiru[2] = { PLAYER_MWA_SPIN_ATTACK_1H, PLAYER_MWA_SPIN_ATTACK_2H };
static u8 Wrolling_kiru[2] = { PLAYER_MWA_BIG_SPIN_1H, PLAYER_MWA_BIG_SPIN_2H };

static u16 item_key_data[] = { BTN_B, BTN_CLEFT, BTN_CDOWN, BTN_CRIGHT };

static u8 magic_power[] = { 12, 24, 24, 12, 24, 12 };

static u16 draw_SE[] = { NA_SE_IT_BOW_DRAW, NA_SE_IT_SLING_DRAW, NA_SE_IT_HOOKSHOT_READY };

static u8 magic_arrow_power[] = { 4, 4, 8 };

static LinkAnimationHeader* link_uperbody_waitR2defense_anm[] = {
    &gPlayerAnim_link_anchor_waitR2defense,
    &gPlayerAnim_link_anchor_waitR2defense_long,
};

static LinkAnimationHeader* link_uperbody_waitL2defense_anm[] = {
    &gPlayerAnim_link_anchor_waitL2defense,
    &gPlayerAnim_link_anchor_waitL2defense_long,
};

static LinkAnimationHeader* link_anchor_defense_hitL_anm[] = {
    &gPlayerAnim_link_anchor_defense_hit,
    &gPlayerAnim_link_anchor_defense_long_hitL,
};

static LinkAnimationHeader* link_anchor_defense_hitR_anm[] = {
    &gPlayerAnim_link_anchor_defense_hit,
    &gPlayerAnim_link_anchor_defense_long_hitR,
};

static LinkAnimationHeader* link_normal_defense_hit_anm[] = {
    &gPlayerAnim_link_normal_defense_hit,
    &gPlayerAnim_link_fighter_defense_long_hit,
};

static LinkAnimationHeader* walk2ready_anm[] = {
    &gPlayerAnim_link_bow_walk2ready,
    &gPlayerAnim_link_hook_walk2ready,
};

static LinkAnimationHeader* wait_anm[] = {
    &gPlayerAnim_link_bow_bow_wait,
    &gPlayerAnim_link_hook_wait,
};

BAD_RETURN(s32) speedF_clear(Player* this) {
    this->actor.speed = 0.0f;
    this->speedXZ = 0.0f;
}

BAD_RETURN(s32) speedF_look_mode_clear(Player* this) {
    speedF_clear(this);
    this->unk_6AD = 0;
}

s32 player_talking_now(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_TALK);
}

void anime_init_standard_stop(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Skeleton_Info_Rom_init_standard_stop(play, &this->skelAnime, anim);
}

void anime_init_standard_repeat(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Skeleton_Info_Rom_init_standard_repeat(play, &this->skelAnime, anim);
}

void anime_init_standard_repeat_3f(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Skeleton_Info_Rom_init_standard_speedset_repeat(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED);
}

void anime_init_standard_stop_3f(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Skeleton_Info_Rom_init_standard_speedset_stop(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED);
}

void anime_morf_end_set(Player* this) {
    this->actor.shape.rot.y += this->skelAnime.jointTable[1].y;
    this->skelAnime.jointTable[1].y = 0;
}

void sword_attack_reset(Player* this) {
    this->stateFlags2 &= ~PLAYER_STATE2_17;
    this->meleeWeaponState = 0;
    this->meleeWeaponInfo[0].active = this->meleeWeaponInfo[1].active = this->meleeWeaponInfo[2].active = 0;
}

void sub_camera_mode_reset(PlayState* play, Player* this) {
    Camera* subCam;

    if (this->subCamId != CAM_ID_NONE) {
        subCam = play->cameraPtrs[this->subCamId];
        if ((subCam != NULL) && (subCam->csId == 1100)) {
            deleteOnepointDemo(play, this->subCamId);
            this->subCamId = CAM_ID_NONE;
        }
    }

    this->stateFlags2 &= ~(PLAYER_STATE2_10 | PLAYER_STATE2_11);
}

void l_hand_child_cancel(PlayState* play, Player* this) {
    Actor* heldActor = this->heldActor;

    if ((heldActor != NULL) && !hook_check(this)) {
        this->actor.child = NULL;
        this->heldActor = NULL;
        this->interactRangeActor = NULL;
        heldActor->parent = NULL;
        this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
#if OOT_VERSION < NTSC_1_1
        if (bom_check(this) >= 0) {
            ability_A_item_change(play, this, PLAYER_IA_NONE);
            this->heldItemId = ITEM_NONE_FE;
        }
#endif
    }

#if OOT_VERSION >= NTSC_1_1
    if (bom_check(this) >= 0) {
        ability_A_item_change(play, this, PLAYER_IA_NONE);
        this->heldItemId = ITEM_NONE_FE;
    }
#endif
}

void action_reset(PlayState* play, Player* this) {
    if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->heldActor == NULL)) {
        if (this->interactRangeActor != NULL) {
            if (this->getItemId == GI_NONE) {
                this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
                this->interactRangeActor = NULL;
            }
        } else {
            this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
        }
    }

    sword_attack_reset(this);
    this->unk_6AD = 0;

    sub_camera_mode_reset(play, this);
    restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));

    this->stateFlags1 &= ~(PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_20 | PLAYER_STATE1_21);
    this->stateFlags2 &= ~(PLAYER_STATE2_4 | PLAYER_STATE2_7 | PLAYER_STATE2_CRAWLING);

    this->actor.shape.rot.x = 0;
    this->actor.shape.yOffset = 0.0f;

    this->unk_845 = this->unk_844 = 0;
}

/**
 * Puts away item currently in hand, if holding any.
 * @return  true if an item needs to be put away, false if not.
 */
s32 to_no_item_set(PlayState* play, Player* this) {
    if (this->heldItemAction >= PLAYER_IA_FISHING_POLE) {
        player_item_on(play, this, ITEM_NONE);
        return true;
    } else {
        return false;
    }
}

void action_reset_all(PlayState* play, Player* this) {
    action_reset(play, this);
    l_hand_child_cancel(play, this);
}

s32 escape_key_input_check(Player* this, s32 arg1, s32 arg2) {
    s16 controlStickAngleDiff = this->prevControlStickAngle - stick_angle;

    this->av2.actionVar2 +=
        arg1 + (s16)(ABS(controlStickAngleDiff) * fabsf(stick_length) * 2.5415802156203426e-06f);

    if (CHECK_BTN_ANY(pad->press.button, BTN_A | BTN_B)) {
        this->av2.actionVar2 += 5;
    }

    return this->av2.actionVar2 > arg2;
}

void sword_hit_stop_set(PlayState* play) {
    if (play->actorCtx.freezeFlashTimer == 0) {
        play->actorCtx.freezeFlashTimer = 1;
    }
}

void player_rumble_entry(Player* this, s32 sourceStrength, s32 duration, s32 decreaseRate, s32 distSq) {
    if (this->actor.category == ACTORCAT_PLAYER) {
        z_vibctl2_vib_setQ(distSq, sourceStrength, duration, decreaseRate);
    }
}

void player_voice_SE_set(Player* this, u16 sfxId) {
    if (this->actor.category == ACTORCAT_PLAYER) {
        player_SE_set(this, sfxId + this->ageProperties->unk_92);
    } else {
        Na_SetBlackLinkVoice(&this->actor.projectedPos, sfxId);
    }
}

void player_voice_SE_stop(Player* this) {
    u16* entry = &stop_voice_SE_flag[0];
    s32 i;

    for (i = 0; i < 4; i++) {
        Nai_StopFx((u16)(*entry + this->ageProperties->unk_92));
        entry++;
    }
}

u16 player_ground_SE_no(Player* this, u16 sfxId) {
    return sfxId + this->floorSfxOffset;
}

void player_ground_SE_set(Player* this, u16 sfxId) {
    player_SE_set(this, player_ground_SE_no(this, sfxId));
}

u16 player_ground_age_SE_no(Player* this, u16 sfxId) {
    return sfxId + this->floorSfxOffset + this->ageProperties->unk_94;
}

void player_ground_age_SE_set(Player* this, u16 sfxId) {
    player_SE_set(this, player_ground_age_SE_no(this, sfxId));
}

void player_walk_SE_set(Player* this, f32 pitchAdjustment) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_IRON_BOOTS;
    } else {
        sfxId = player_ground_age_SE_no(this, NA_SE_PL_WALK_GROUND);
    }

    Na_StartLinkWalkSe(&this->actor.projectedPos, sfxId, pitchAdjustment);
}

void player_jump_SE_set(Player* this) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_JUMP + SURFACE_SFX_OFFSET_IRON_BOOTS;
    } else {
        sfxId = player_ground_age_SE_no(this, NA_SE_PL_JUMP);
    }

    player_SE_set(this, sfxId);
}

void player_land_SE_set(Player* this) {
    s32 sfxId;

    if (this->currentBoots == PLAYER_BOOTS_IRON) {
        sfxId = NA_SE_PL_LAND + SURFACE_SFX_OFFSET_IRON_BOOTS;
    } else {
        sfxId = player_ground_age_SE_no(this, NA_SE_PL_LAND);
    }

    player_SE_set(this, sfxId);
}

void player_big_SE_set(Player* this, u16 sfxId) {
    player_SE_set(this, sfxId);
    this->stateFlags2 |= PLAYER_STATE2_3;
}

/**
 * Process a list of `AnimSfx` entries.
 * An `AnimSfx` entry contains a sound effect to play, a frame number that indicates
 * when during an animation it should play, and a type value that indicates how it should be played back.
 *
 * The list will stop being processed after an entry that has a negative value for the `data` field.
 *
 * Some types do not make use of `sfxId`, the SFX function called will pick a sound on its own.
 * The `sfxId` field is not used in this case and can be any value, but 0 is typically used.
 *
 * @param entry  A pointer to the first entry of an `AnimSfx` list.
 */
void player_anime_check_SE_set(Player* this, AnimSfxEntry* entry) {
    s32 cont;
    s32 padding;

    do {
        s32 absData = ABS(entry->data);
        s32 type = ANIMSFX_GET_TYPE(absData);

        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, fabsf(ANIMSFX_GET_FRAME(absData)))) {
            if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_GENERAL)) {
                player_SE_set(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR)) {
                player_ground_SE_set(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_FLOOR_BY_AGE)) {
                player_ground_age_SE_set(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_VOICE)) {
                player_voice_SE_set(this, entry->sfxId);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_LANDING)) {
                player_land_SE_set(this);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_RUNNING)) {
                player_walk_SE_set(this, 6.0f);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_JUMPING)) {
                player_jump_SE_set(this);
            } else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_WALKING)) {
                player_walk_SE_set(this, 0.0f);
            }
#if OOT_VERSION >= PAL_1_0
            else if (type == ANIMSFX_SHIFT_TYPE(ANIMSFX_TYPE_UNKNOWN)) {
                Na_StartLinkWalkSe(&this->actor.projectedPos,
                              NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_WOOD + this->ageProperties->unk_94, 0.0f);
            }
#endif
        }

        cont = (entry->data >= 0); // stop processing if `data` is negative
        entry++;
    } while (cont);
}

void anime_init_stop_basic(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Skeleton_Info_Rom_init(play, &this->skelAnime, anim, 1.0f, 0.0f, Si2_anime_end_frame(anim), ANIMMODE_ONCE, -6.0f);
}

void anime_init_stop_basic_3f(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Skeleton_Info_Rom_init(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, Si2_anime_end_frame(anim),
                         ANIMMODE_ONCE, -6.0f);
}

void anime_init_repeat_basic(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Skeleton_Info_Rom_init(play, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void anime_init_0_stop_basic(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Skeleton_Info_Rom_init(play, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
}

void anime_init_0_repeat_basic(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    Skeleton_Info_Rom_init(play, &this->skelAnime, anim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -16.0f);
}

s32 anime_play_repeat(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_init_standard_repeat(play, this, anim);
        return 1;
    } else {
        return 0;
    }
}

void anime_move_base_read(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;
    this->skelAnime.prevRot = this->actor.shape.rot.y;
}

void anime_move_base_read_scale(Player* this) {
    anime_move_base_read(this);

    this->skelAnime.prevTransl.x *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.y *= this->ageProperties->unk_08;
    this->skelAnime.prevTransl.z *= this->ageProperties->unk_08;
}

void anime_morf_reset(Player* this) {
    this->skelAnime.jointTable[1].y = 0;
}

/**
 * Finishes "AnimMovement" by resetting various aspects of Player's SkelAnime structure.
 *
 * This function is called in Player_actor_set_process so it will run on every action change, but
 * it can also be called within action functions to change animations in the middle of an action.
 */
void anime_move_reset(Player* this) {
    if (this->skelAnime.movementFlags != 0) {
        anime_morf_end_set(this);

        this->skelAnime.jointTable[0].x = this->skelAnime.baseTransl.x;
        this->skelAnime.jointTable[0].z = this->skelAnime.baseTransl.z;

        if (this->skelAnime.movementFlags & ANIM_FLAG_ENABLE_MOVEMENT) {
            if (this->skelAnime.movementFlags & ANIM_FLAG_UPDATE_Y) {
                this->skelAnime.jointTable[0].y = this->skelAnime.prevTransl.y;
            }
        } else {
            this->skelAnime.jointTable[0].y = this->skelAnime.baseTransl.y;
        }

        anime_move_base_read(this);

        this->skelAnime.movementFlags = 0;
    }
}

/**
 * This is a reimplementation of `Skeleton_Proc_Anime_Move`.
 *
 * This achieves the same goal as `Skeleton_Proc_Anime_Move`but it adds
 * the ability to scale the resulting movement according to age.
 *
 * When using the AnimTask variant, age specific scaling can only be applied visually
 * to the root bone position and does not affect world position.
 */
void anime_move_calc(Player* this, s32 movementFlags) {
    Vec3f diff;

    this->skelAnime.movementFlags = movementFlags;
    this->skelAnime.prevTransl = this->skelAnime.baseTransl;

    Skeleton_Info_translate_set(&this->skelAnime, &diff, this->actor.shape.rot.y);

    if (movementFlags & ANIM_FLAG_UPDATE_XZ) {
        if (!LINK_IS_ADULT) {
            diff.x *= 0.64f;
            diff.z *= 0.64f;
        }

        this->actor.world.pos.x += diff.x * this->actor.scale.x;
        this->actor.world.pos.z += diff.z * this->actor.scale.z;
    }

    if (movementFlags & ANIM_FLAG_UPDATE_Y) {
        if (!(movementFlags & ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT)) {
            diff.y *= this->ageProperties->unk_08;
        }

        this->actor.world.pos.y += diff.y * this->actor.scale.y;
    }

    anime_morf_end_set(this);
}

#define PLAYER_ANIM_MOVEMENT_RESET (1 << 8)
#define PLAYER_ANIM_MOVEMENT_RESET_BY_AGE (1 << 9)

/**
 * Starts "AnimMovement" so that Player will move according to the translation and rotation specified
 * by the animation that is playing.
 *
 * The `flags` field can be any of the SkelAnime system's `ANIM_FLAG_` flags, as well as Player-specific
 * `PLAYER_ANIM_MOVEMENT_` flags.
 *
 * For AnimMovement features to be enabled, it is usually required to pass `ANIM_FLAG_ENABLE_MOVEMENT`
 * as one of the flags, but there are a few niche cases where it can be desirable to omit it
 * (for example to use `ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT` without any actual AnimMovement).
 *
 * Note: AnimMovement is always disabled during every action change.
 *       This means the order that functions are called matters.
 *       `anime_move_init` must be called *after* a call to `Player_actor_set_process`.
 */
void anime_move_init(PlayState* play, Player* this, s32 flags) {
    if (flags & PLAYER_ANIM_MOVEMENT_RESET_BY_AGE) {
        anime_move_base_read_scale(this);
    } else if ((flags & PLAYER_ANIM_MOVEMENT_RESET) || (this->skelAnime.movementFlags != 0)) {
        // If AnimMovement is already in use when this function is called and
        // `PLAYER_ANIM_MOVEMENT_RESET_BY_AGE` is not set, then this case will be used.
        anime_move_base_read(this);
    } else {
        // Default case used when AnimMovement was not enabled previously.
        // This sets prevTransl and prevRot to Players current translation and yaw.
        this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
        this->skelAnime.prevRot = this->actor.shape.rot.y;
    }

    // Remove Player specific flags by masking the lower byte before setting to `skelAnime.movementFlags`
    this->skelAnime.movementFlags = flags & 0xFF;

    speedF_clear(this);
    Skeleton_Proc_Anime_Change_Stop_On(play);
}

// TODO: Change all of these wrapper functions below to use "AnimMovement" instead of "AnimReplace"
void demo_stop_anime_move_init_set(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags,
                                        f32 playbackSpeed) {
    Skeleton_Info_Rom_init_standard_speedset_stop(play, &this->skelAnime, anim, playbackSpeed);
    anime_move_init(play, this, flags);
}

void demo_stop_anime_move_init3(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags) {
    demo_stop_anime_move_init_set(play, this, anim, flags, 1.0f);
}

void demo_stop_anime_move_init2(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags) {
    demo_stop_anime_move_init_set(play, this, anim, flags, PLAYER_ANIM_ADJUSTED_SPEED);
}

void demo_stop_anime_move_init(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    demo_stop_anime_move_init2(play, this, anim,
                                       ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                           ANIM_FLAG_ADJUST_STARTING_POS);
}

void demo_repeat_anime_move_init_set(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags,
                                        f32 playbackSpeed) {
    Skeleton_Info_Rom_init_standard_speedset_repeat(play, &this->skelAnime, anim, playbackSpeed);
    anime_move_init(play, this, flags);
}

void demo_repeat_anime_move_init3(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags) {
    demo_repeat_anime_move_init_set(play, this, anim, flags, 1.0f);
}

void demo_repeat_anime_move_init2(PlayState* play, Player* this, LinkAnimationHeader* anim, s32 flags) {
    demo_repeat_anime_move_init_set(play, this, anim, flags, PLAYER_ANIM_ADJUSTED_SPEED);
}

void demo_repeat_anime_move_init(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    demo_repeat_anime_move_init2(play, this, anim,
                                       ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                           ANIM_FLAG_ADJUST_STARTING_POS);
}

void stick_read(PlayState* play, Player* this) {
    s8 spinAngle;
    s8 direction;

    this->prevControlStickMagnitude = stick_length;
    this->prevControlStickAngle = stick_angle;

    stick_ratio_set(&stick_length, &stick_angle, pad);

    stick_camera_angle = getCameraAngleY(GET_ACTIVE_CAM(play)) + stick_angle;

    this->controlStickDataIndex = (this->controlStickDataIndex + 1) % 4;

    if (stick_length < 55.0f) {
        direction = PLAYER_STICK_DIR_NONE;
        spinAngle = -1;
    } else {
        spinAngle = (u16)(stick_angle + 0x2000) >> 9;
        direction = (u16)((s16)(stick_camera_angle - this->actor.shape.rot.y) + 0x2000) >> 14;
    }

    this->controlStickSpinAngles[this->controlStickDataIndex] = spinAngle;
    this->controlStickDirections[this->controlStickDataIndex] = direction;
}

void swim_check_anime_init_standard_stop(PlayState* play, Player* this, LinkAnimationHeader* linkAnim) {
    Skeleton_Info_Rom_init_standard_speedset_stop(play, &this->skelAnime, linkAnim, environment_speed_ratio);
}

int swim_check(Player* this) {
    return (this->stateFlags1 & PLAYER_STATE1_27) && (this->currentBoots != PLAYER_BOOTS_IRON);
}

s32 boom_check(Player* this) {
    return (this->stateFlags1 & PLAYER_STATE1_USING_BOOMERANG);
}

void get_item_shape_set(Player* this, PlayState* play) {
    GetItemEntry* giEntry = &get_item_status[this->getItemId - 1];

    this->unk_862 = ABS(giEntry->gi);
}

/**
 * Get the appropriate Idle animation based on current `modelAnimType`.
 * This is the default idle animation.
 *
 * For fidget idle animations (which can for example, change based on environment)
 * see `weather_wait_anm_data`.
 */
LinkAnimationHeader* wait_anime_select(Player* this) {
    return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType);
}

/**
 * Return values for `wait_anime_check`
 */
#define IDLE_ANIM_DEFAULT -1
#define IDLE_ANIM_NONE 0
// Fidget idle anims are returned by index. See `weather_wait_anm_data` and `FidgetType`.

/**
 * Checks if the current animation is an idle animation.
 * If the current animation is a fidget animation, the index into
 * `weather_wait_anm_data` is returned (plus one).
 * If the current animation is a default idle animation, -1 is returned.
 * Lastly if the current animation is neither of these, 0 is returned.
 */
s32 wait_anime_check(Player* this) {
    if (wait_anime_select(this) != this->skelAnime.animation) {
        LinkAnimationHeader** fidgetAnim;
        s32 i;

        for (i = 0, fidgetAnim = &weather_wait_anm_data[0][0]; i < ARRAY_COUNT_2D(weather_wait_anm_data); i++, fidgetAnim++) {
            if (this->skelAnime.animation == *fidgetAnim) {
                return i + 1;
            }
        }

        return IDLE_ANIM_NONE;
    }

    return IDLE_ANIM_DEFAULT;
}

void wait_anime_SE_set(Player* this, s32 fidgetAnimIndex) {
    if (SE_set_status_table[fidgetAnimIndex] != FIDGET_ANIMSFX_NONE) {
        player_anime_check_SE_set(this, SE_set_status_data[SE_set_status_table[fidgetAnimIndex] - 1]);
    }
}

LinkAnimationHeader* run_anime_select(Player* this) {
    if (this->unk_890 != 0) {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_damage_run, this->modelAnimType);
    } else if (!(this->stateFlags1 & (PLAYER_STATE1_27 | PLAYER_STATE1_29)) &&
               (this->currentBoots == PLAYER_BOOTS_IRON)) {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_heavy_run, this->modelAnimType);
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_run, this->modelAnimType);
    }
}

int boom_ready_check(Player* this) {
    return boom_check(this) && (this->unk_834 != 0);
}

LinkAnimationHeader* waitR_anime_select(Player* this) {
    if (boom_ready_check(this)) {
        return &gPlayerAnim_link_boom_throw_waitR;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_waitR, this->modelAnimType);
    }
}

LinkAnimationHeader* waitL_anime_select(Player* this) {
    if (boom_ready_check(this)) {
        return &gPlayerAnim_link_boom_throw_waitL;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_waitL, this->modelAnimType);
    }
}

LinkAnimationHeader* side_walk_anime_select(Player* this) {
    if (player_bow_ready_check(this)) {
        return &gPlayerAnim_link_bow_side_walk;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walk, this->modelAnimType);
    }
}

LinkAnimationHeader* side_walkR_anime_select(Player* this) {
    if (boom_ready_check(this)) {
        return &gPlayerAnim_link_boom_throw_side_walkR;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkR, this->modelAnimType);
    }
}

LinkAnimationHeader* side_walkL_anime_select(Player* this) {
    if (boom_ready_check(this)) {
        return &gPlayerAnim_link_boom_throw_side_walkL;
    } else {
        return GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkL, this->modelAnimType);
    }
}

void Player_actor_set_uperbody_process(Player* this, UpperActionFunc upperActionFunc) {
    this->upperActionFunc = upperActionFunc;
    this->unk_836 = 0;
    this->upperAnimInterpWeight = 0.0f;
    player_voice_SE_stop(this);
}

#if OOT_VERSION < NTSC_1_1
s32 sword_check2(Player* this) {
    return sword_check(this);
}
#else
#define sword_check2 sword_check
#endif

void item_change_anime_check(PlayState* play, Player* this, s8 itemAction) {
    LinkAnimationHeader* current = this->skelAnime.animation;
    LinkAnimationHeader** iter = link_anime_table + this->modelAnimType;
    u32 animGroup;

    // This is redundant, the same two flags get unset in
    // `ability_A_item_change` called below.
    this->stateFlags1 &= ~(PLAYER_STATE1_3 | PLAYER_STATE1_USING_BOOMERANG);

    for (animGroup = 0; animGroup < PLAYER_ANIMGROUP_MAX; animGroup++) {
        if (current == *iter) {
            break;
        }
        iter += PLAYER_ANIMTYPE_MAX;
    }

    ability_A_item_change(play, this, itemAction);

    if (animGroup < PLAYER_ANIMGROUP_MAX) {
        this->skelAnime.animation = GET_PLAYER_ANIM(animGroup, this->modelAnimType);
    }
}

s8 Player_item_type_check(s32 item) {
    if (item >= ITEM_NONE_FE) {
        return PLAYER_IA_NONE;
    } else if (item == ITEM_SWORD_CS) {
        return PLAYER_IA_SWORD_CS;
    } else if (item == ITEM_FISHING_POLE) {
        return PLAYER_IA_FISHING_POLE;
    } else {
        return item_type_data[item];
    }
}

void ability_A_item_non_init(PlayState* play, Player* this) {
}

#if OOT_VERSION < NTSC_1_1
s32 longsword_check2(Player* this) {
    return longsword_check(this);
}
#else
#define longsword_check2 longsword_check
#endif

void ability_A_item_stick_init(PlayState* play, Player* this) {
    this->unk_85C = 1.0f;
}

void ability_A_item_hammer_init(PlayState* play, Player* this) {
}

void ability_A_item_bow_init(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_3;

    if (this->heldItemAction != PLAYER_IA_SLINGSHOT) {
        this->unk_860 = -1;
    } else {
        this->unk_860 = -2;
    }
}

void ability_A_item_bomb_init(PlayState* play, Player* this) {
    s32 explosiveType;
    ExplosiveInfo* explosiveInfo;
    Actor* spawnedActor;

    if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        to_no_item_set(play, this);
        return;
    }

    explosiveType = bom_check(this);
    explosiveInfo = &bom_status[explosiveType];

    spawnedActor =
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, explosiveInfo->actorId, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 0);
    if (spawnedActor != NULL) {
        if ((explosiveType != 0) && (play->bombchuBowlingStatus != 0)) {
            play->bombchuBowlingStatus--;
            if (play->bombchuBowlingStatus == 0) {
                play->bombchuBowlingStatus = -1;
            }
        } else {
            item_count_vary(explosiveInfo->itemId, -1);
        }

        this->interactRangeActor = spawnedActor;
        this->heldActor = spawnedActor;
        this->getItemId = GI_NONE;
        this->unk_3BC.y = spawnedActor->shape.rot.y - this->actor.shape.rot.y;
        this->stateFlags1 |= PLAYER_STATE1_CARRYING_ACTOR;
    }
}

void ability_A_item_hook_init(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_3;
    this->unk_860 = -3;

    this->heldActor =
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_ARMS_HOOK, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, 0);
}

void ability_A_item_boom_init(PlayState* play, Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_USING_BOOMERANG;
}

void ability_A_item_change(PlayState* play, Player* this, s8 itemAction) {
    this->unk_85C = 0.0f;
    this->unk_858 = 0.0f;
    this->unk_860 = 0;

    this->heldItemAction = this->itemAction = itemAction;
    this->modelGroup = this->nextModelGroup;

#if OOT_VERSION >= NTSC_1_1
    this->stateFlags1 &= ~(PLAYER_STATE1_3 | PLAYER_STATE1_USING_BOOMERANG);
#endif

    ability_A_item_init[itemAction](play, this);

    shape_type_set(this, this->modelGroup);
}

void sword_attack_set(Player* this, s32 newMeleeWeaponState) {
    u16 itemSfx;
    u16 voiceSfx;

    if (this->meleeWeaponState == 0) {
        if ((this->heldItemAction == PLAYER_IA_SWORD_BIGGORON) &&
            (z_common_data.save.info.playerData.swordHealth > 0.0f)) {
            itemSfx = NA_SE_IT_HAMMER_SWING;
        } else {
            itemSfx = NA_SE_IT_SWORD_SWING;
        }

        voiceSfx = NA_SE_VO_LI_SWORD_N;
        if (this->heldItemAction == PLAYER_IA_HAMMER) {
            itemSfx = NA_SE_IT_HAMMER_SWING;
        } else if (this->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) {
            itemSfx = 0;
            voiceSfx = NA_SE_VO_LI_SWORD_L;
        } else if (this->unk_845 >= 3) {
            itemSfx = NA_SE_IT_SWORD_SWING_HARD;
            voiceSfx = NA_SE_VO_LI_SWORD_L;
        }

        if (itemSfx != 0) {
            player_big_SE_set(this, itemSfx);
        }

        if (!((this->meleeWeaponAnimation >= PLAYER_MWA_FLIPSLASH_START) &&
              (this->meleeWeaponAnimation <= PLAYER_MWA_JUMPSLASH_FINISH))) {
            player_voice_SE_set(this, voiceSfx);
        }
    }

    this->meleeWeaponState = newMeleeWeaponState;
}

/**
 * This function checks for friendly (non-hostile) Z-Target related states.
 * For hostile related lock-on states, see `anchor_mode_check_set` and `anchor_mode_check`.
 *
 * Note that `PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS` will include all `focusActor` use cases that relate to
 * friendly actors. This function can return true when talking to an actor, for example.
 * Despite that, this function is only relevant in the context of actor lock-on, which is a subset of actor focus.
 * This is why the function name states `FriendlyLockOn` instead of `FriendlyActorFocus`.
 *
 * There is a special case that allows hostile actors to be treated as "friendly" if Player is carrying another actor
 * See relevant code in `anchor_check` for more details.
 *
 * Additionally, `PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE` will be set very briefly in some conditions when
 * a lock-on is forced to release. In these niche cases, this function will apply to both friendly and hostile actors.
 * Overall, it is safe to assume that this specific state flag is not very relevant for this function's use cases.
 */
s32 parallel_mode_check(Player* this) {
    if (this->stateFlags1 &
        (PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS | PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE)) {
        return true;
    } else {
        return false;
    }
}

/**
 * Checks the current state of `focusActor` and if it is a hostile actor (if applicable).
 * If so, sets `PLAYER_STATE1_HOSTILE_LOCK_ON` which will control Player's "battle" response to
 * hostile actors. This includes affecting how movement is handled, and enabling a "fighting" set
 * of animations.
 *
 * Note that `anchor_mode_check` also exists to check if there is currently a hostile lock-on actor.
 * This function differs in that it first updates the flag if appropriate, then returns the same information.
 *
 * @return  true if there is currently a hostile lock-on actor, false otherwise
 */
s32 anchor_mode_check_set(Player* this) {
    if ((this->focusActor != NULL) &&
        CHECK_FLAG_ALL(this->focusActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)) {
        this->stateFlags1 |= PLAYER_STATE1_HOSTILE_LOCK_ON;

        return true;
    } else {
        if (this->stateFlags1 & PLAYER_STATE1_HOSTILE_LOCK_ON) {
            this->stateFlags1 &= ~PLAYER_STATE1_HOSTILE_LOCK_ON;

            // sync world and shape yaw when not moving
            if (this->speedXZ == 0.0f) {
                this->yaw = this->actor.shape.rot.y;
            }
        }

        return false;
    }
}

/**
 * Returns true if currently Z-Targeting, false if not.
 * Z-Targeting here is a blanket term that covers both the "actor lock-on" and "parallel" states.
 *
 * This variant of the function calls `anchor_mode_check`, which does not update the hostile
 * lock-on actor state.
 */
int anchor_parallel_mode_check(Player* this) {
    return anchor_mode_check(this) || parallel_mode_check(this);
}

/**
 * Returns true if currently Z-Targeting, false if not.
 * Z-Targeting here is a blanket term that covers both the "actor lock-on" and "parallel" states.
 *
 * This variant of the function calls `anchor_mode_check_set`, which updates the hostile
 * lock-on actor state before checking its state.
 */
int anchor_parallel_mode_check_set(Player* this) {
    return anchor_mode_check_set(this) || parallel_mode_check(this);
}

void anchor_LR_ratio_reset(Player* this) {
    this->unk_870 = this->unk_874 = 0.0f;
}

s32 item_type_check(Player* this, s32 item) {
    if ((item < ITEM_NONE_FE) && (Player_item_type_check(item) == this->itemAction)) {
        return true;
    } else {
        return false;
    }
}

s32 mask_type_check(s32 item1, s32 itemAction) {
    if ((item1 < ITEM_NONE_FE) && (Player_item_type_check(item1) == itemAction)) {
        return true;
    } else {
        return false;
    }
}

s32 item_read(PlayState* play, s32 index) {
    if (index >= 4) {
        return ITEM_NONE;
    } else if (play->bombchuBowlingStatus != 0) {
        return (play->bombchuBowlingStatus > 0) ? ITEM_BOMBCHU : ITEM_NONE;
    } else if (index == 0) {
        return B_BTN_ITEM;
    } else if (index == 1) {
        return C_BTN_ITEM(0);
    } else if (index == 2) {
        return C_BTN_ITEM(1);
    } else {
        return C_BTN_ITEM(2);
    }
}

/**
 * Handles the high level item usage and changing process based on the B and C buttons.
 *
 * Tasks include:
 *    - Put away a mask if it is not present on any C button
 *    - Put away an item if it is not present on the B button or any C button
 *    - Use an item on the B button or any C button if the corresponding button is pressed
 *    - Keep track of the current item button being held down
 */
void arms_change_check(Player* this, PlayState* play) {
    s32 maskItemAction;
    s32 item;
    s32 i;

    if (this->currentMask != PLAYER_MASK_NONE) {
        maskItemAction = this->currentMask - 1 + PLAYER_IA_MASK_KEATON;

        if (!mask_type_check(C_BTN_ITEM(0), maskItemAction) &&
            !mask_type_check(C_BTN_ITEM(1), maskItemAction) &&
            !mask_type_check(C_BTN_ITEM(2), maskItemAction)) {
            this->currentMask = PLAYER_MASK_NONE;
        }
    }

    if (!(this->stateFlags1 & (PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_29)) && !hook_shoot_check(this)) {
        if (this->itemAction >= PLAYER_IA_FISHING_POLE) {
            if (!item_type_check(this, B_BTN_ITEM) && !item_type_check(this, C_BTN_ITEM(0)) &&
                !item_type_check(this, C_BTN_ITEM(1)) && !item_type_check(this, C_BTN_ITEM(2))) {
                player_item_on(play, this, ITEM_NONE);
                return;
            }
        }

        for (i = 0; i < ARRAY_COUNT(item_key_data); i++) {
            if (CHECK_BTN_ALL(pad->press.button, item_key_data[i])) {
                break;
            }
        }

        item = item_read(play, i);

        if (item >= ITEM_NONE_FE) {
            for (i = 0; i < ARRAY_COUNT(item_key_data); i++) {
                if (CHECK_BTN_ALL(pad->cur.button, item_key_data[i])) {
                    break;
                }
            }

            item = item_read(play, i);

            if ((item < ITEM_NONE_FE) && (Player_item_type_check(item) == this->heldItemAction)) {
                item_also_key = true;
            }
        } else {
            this->heldItemButton = i;
            player_item_on(play, this, item);
        }
    }
}

void uperbody_nom2figh_init(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 endFrameTemp;
    f32 startFrame;
    f32 endFrame;
    f32 playSpeed;
    s32 itemChangeType;
    s8 heldItemAction;
    s32 nextAnimType;

    heldItemAction = Player_item_type_check(this->heldItemId);

    Player_actor_set_uperbody_process(this, uperbody_nom2figh_play);

    nextAnimType = player_shape_type[this->nextModelGroup][PLAYER_MODELGROUPENTRY_ANIM];
    itemChangeType = item_change_table[player_shape_type[this->modelGroup][PLAYER_MODELGROUPENTRY_ANIM]][nextAnimType];

    if ((heldItemAction == PLAYER_IA_BOTTLE) || (heldItemAction == PLAYER_IA_BOOMERANG) ||
        ((heldItemAction == PLAYER_IA_NONE) &&
         ((this->heldItemAction == PLAYER_IA_BOTTLE) || (this->heldItemAction == PLAYER_IA_BOOMERANG)))) {
        itemChangeType = (heldItemAction == PLAYER_IA_NONE) ? -PLAYER_ITEM_CHG_13 : PLAYER_ITEM_CHG_13;
    }

    this->itemChangeType = ABS(itemChangeType);
    anim = item_change_data[this->itemChangeType].anim;

    if ((anim == &gPlayerAnim_link_normal_fighter2free) && (this->currentShield == PLAYER_SHIELD_NONE)) {
        anim = &gPlayerAnim_link_normal_free2fighter_free;
    }

    endFrameTemp = Si2_anime_end_frame(anim);
    endFrame = endFrameTemp;

    if (itemChangeType >= 0) {
        playSpeed = 1.2f;
        startFrame = 0.0f;
    } else {
        endFrame = 0.0f;
        playSpeed = -1.2f;
        startFrame = endFrameTemp;
    }

    if (heldItemAction != PLAYER_IA_NONE) {
        playSpeed *= 2.0f;
    }

    Skeleton_Info_Rom_init(play, &this->upperSkelAnime, anim, playSpeed, startFrame, endFrame, ANIMMODE_ONCE, 0.0f);

    this->stateFlags1 &= ~PLAYER_STATE1_START_CHANGING_HELD_ITEM;
}

void to_uperbody_item_change_check(Player* this, PlayState* play) {
    if ((this->actor.category == ACTORCAT_PLAYER) &&
#if OOT_VERSION >= NTSC_1_1
        !(this->stateFlags1 & PLAYER_STATE1_START_CHANGING_HELD_ITEM) &&
#endif
        ((this->heldItemAction == this->itemAction) || (this->stateFlags1 & PLAYER_STATE1_SHIELDING)) &&
        (z_common_data.save.info.playerData.health != 0) && (play->csCtx.state == CS_STATE_IDLE) &&
        (this->csAction == PLAYER_CSACTION_NONE) && (play->shootingGalleryStatus == 0) &&
        (play->activeCamId == CAM_ID_MAIN) && (play->transitionTrigger != TRANS_TRIGGER_START) &&
        (z_common_data.timerState != TIMER_STATE_STOP)) {
        arms_change_check(this, play);
    }

    if (this->stateFlags1 & PLAYER_STATE1_START_CHANGING_HELD_ITEM) {
        uperbody_nom2figh_init(this, play);
    }
}

s32 bow_count_check(PlayState* play, Player* this, s32* itemPtr, s32* typePtr) {
    if (LINK_IS_ADULT) {
        *itemPtr = ITEM_BOW;
        if (this->stateFlags1 & PLAYER_STATE1_23) {
            *typePtr = ARROW_NORMAL_HORSE;
        } else {
            *typePtr = ARROW_NORMAL + (this->heldItemAction - PLAYER_IA_BOW);
        }
    } else {
        *itemPtr = ITEM_SLINGSHOT;
        *typePtr = ARROW_SEED;
    }

    if (z_common_data.minigameState == 1) {
        return play->interfaceCtx.hbaAmmo;
    } else if (play->shootingGalleryStatus != 0) {
        return play->shootingGalleryStatus;
    } else {
        return AMMO(*itemPtr);
    }
}

s32 bow_ready_init_check(Player* this, PlayState* play) {
    s32 item;
    s32 arrowType;
    s32 magicArrowType;

    if ((this->heldItemAction >= PLAYER_IA_BOW_FIRE) && (this->heldItemAction <= PLAYER_IA_BOW_0E) &&
        (z_common_data.magicState != MAGIC_STATE_IDLE)) {
        Na_StartSystemSe_F(NA_SE_SY_ERROR);
    } else {
        Player_actor_set_uperbody_process(this, uperbody_bow_ready);

        this->stateFlags1 |= PLAYER_STATE1_9;
        this->unk_834 = 14;

        if (this->unk_860 >= 0) {
            player_SE_set(this, draw_SE[ABS(this->unk_860) - 1]);

            if (!hook_check(this) && (bow_count_check(play, this, &item, &arrowType) > 0)) {
                magicArrowType = arrowType - ARROW_FIRE;

                if (this->unk_860 >= 0) {
                    if ((magicArrowType >= 0) && (magicArrowType <= 2) &&
                        !magic_meter_check(play, magic_arrow_power[magicArrowType], MAGIC_CONSUME_NOW)) {
                        arrowType = ARROW_NORMAL;
                    }

                    this->heldActor = Actor_info_make_child_actor(
                        &play->actorCtx, &this->actor, play, ACTOR_EN_ARROW, this->actor.world.pos.x,
                        this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.shape.rot.y, 0, arrowType);
                }
            }
        }

        return 1;
    }

    return 0;
}

void item_change_set(PlayState* play, Player* this) {
    if (this->heldItemAction != PLAYER_IA_NONE) {
        if (nom2figh_check(this, this->heldItemAction) >= 0) {
            player_big_SE_set(this, NA_SE_IT_SWORD_PUTAWAY);
        } else {
            player_big_SE_set(this, NA_SE_PL_CHANGE_ARMS);
        }
    }

    player_item_on(play, this, this->heldItemId);

    if (nom2figh_check(this, this->heldItemAction) >= 0) {
        player_big_SE_set(this, NA_SE_IT_SWORD_PICKOUT);
    } else if (this->heldItemAction != PLAYER_IA_NONE) {
        player_big_SE_set(this, NA_SE_PL_CHANGE_ARMS);
    }
}

void Player_actor_uperbody_process_init(PlayState* play, Player* this) {
    if (uperbody_nom2figh_play == this->upperActionFunc) {
        item_change_set(play, this);
    }

    Player_actor_set_uperbody_process(this, uperbody_process[this->heldItemAction]);
    this->unk_834 = 0;
    this->idleType = PLAYER_IDLE_DEFAULT;
    l_hand_child_cancel(play, this);
    this->stateFlags1 &= ~PLAYER_STATE1_START_CHANGING_HELD_ITEM;
}

LinkAnimationHeader* uperbody_defense_set(PlayState* play, Player* this) {
    Player_actor_set_uperbody_process(this, uperbody_defense);
    l_hand_child_cancel(play, this);

    if (this->unk_870 < 0.5f) {
        return link_uperbody_waitR2defense_anm[longsword_check(this)];
    } else {
        return link_uperbody_waitL2defense_anm[longsword_check(this)];
    }
}

s32 to_uperbody_defense_check(PlayState* play, Player* this) {
    LinkAnimationHeader* anim;
    f32 frame;

    if (!(this->stateFlags1 & (PLAYER_STATE1_SHIELDING | PLAYER_STATE1_23 | PLAYER_STATE1_29)) &&
        (play->shootingGalleryStatus == 0) && (this->heldItemAction == this->itemAction) &&
        (this->currentShield != PLAYER_SHIELD_NONE) && !child_hyral_shield_check(this) &&
        anchor_parallel_mode_check(this) && CHECK_BTN_ALL(pad->cur.button, BTN_R)) {

        anim = uperbody_defense_set(play, this);
        frame = Si2_anime_end_frame(anim);
        Skeleton_Info_Rom_init(play, &this->upperSkelAnime, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);
        player_SE_set(this, NA_SE_IT_SHIELD_POSTURE);

        return 1;
    } else {
        return 0;
    }
}

s32 uperbody_non(Player* this, PlayState* play) {
    if (to_uperbody_defense_check(play, this)) {
        return true;
    } else {
        return false;
    }
}

void uperbody_defense_end_init(Player* this) {
    Player_actor_set_uperbody_process(this, uperbody_defense_end);

    if (this->itemAction < 0) {
        now_item_cancel(this);
    }

    Skeleton_Info_reverse(&this->upperSkelAnime);
    player_SE_set(this, NA_SE_IT_SHIELD_REMOVE);
}

void normal_fighter_shape_set(PlayState* play, Player* this) {
    ItemChangeInfo* itemChangeEntry = &item_change_data[this->itemChangeType];
    f32 changeFrame;

    changeFrame = itemChangeEntry->changeFrame;
    changeFrame = (this->upperSkelAnime.playSpeed < 0.0f) ? changeFrame - 1.0f : changeFrame;

    if (Skeleton_Info_Rom_frame_check(&this->upperSkelAnime, changeFrame)) {
        item_change_set(play, this);
    }

    anchor_mode_check_set(this);
}

s32 to_uperbody_nom2figh_check(Player* this, PlayState* play) {
    if (this->stateFlags1 & PLAYER_STATE1_START_CHANGING_HELD_ITEM) {
        uperbody_nom2figh_init(this, play);
    } else {
        return 0;
    }

    return 1;
}

/**
 * The actual sword weapon is not handled here. See `to_kiru_check` for melee weapon usage.
 * This upper body action allows for shielding or changing held items while a sword is in hand.
 */
s32 uperbody_nom2figh_wait(Player* this, PlayState* play) {
    if (to_uperbody_defense_check(play, this) || to_uperbody_nom2figh_check(this, play)) {
        return true;
    } else {
        return false;
    }
}

s32 uperbody_nom2figh_play(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime) ||
        ((Player_item_type_check(this->heldItemId) == this->heldItemAction) &&
         (item_key =
              (item_key || ((this->modelAnimType != PLAYER_ANIMTYPE_3) && (play->shootingGalleryStatus == 0)))))) {
        Player_actor_set_uperbody_process(this, uperbody_process[this->heldItemAction]);
        this->unk_834 = 0;
        this->idleType = PLAYER_IDLE_DEFAULT;
        item_also_key = item_key;

        return this->upperActionFunc(this, play);
    }

    if (wait_anime_check(this) != IDLE_ANIM_NONE) {
        normal_fighter_shape_set(play, this);
        anime_init_standard_stop(play, this, wait_anime_select(this));
        this->idleType = PLAYER_IDLE_DEFAULT;
    } else {
        normal_fighter_shape_set(play, this);
    }

    return true;
}

s32 uperbody_defense(Player* this, PlayState* play) {
    Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime);

    if (!CHECK_BTN_ALL(pad->cur.button, BTN_R)) {
        uperbody_defense_end_init(this);
        return true;
    } else {
        this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
        defense_shape_type_set(this);
        return true;
    }
}

s32 uperbody_defense_hit(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 frame;

    if (Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime)) {
        anim = uperbody_defense_set(play, this);
        frame = Si2_anime_end_frame(anim);
        Skeleton_Info_Rom_init(play, &this->upperSkelAnime, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);
    }

    this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
    defense_shape_type_set(this);

    return true;
}

s32 uperbody_defense_end(Player* this, PlayState* play) {
    item_key = item_also_key;

    if (item_key || Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime)) {
        Player_actor_set_uperbody_process(this, uperbody_process[this->heldItemAction]);
        Skeleton_Info_Rom_init_standard_repeat(play, &this->upperSkelAnime,
                               GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType));
        this->idleType = PLAYER_IDLE_DEFAULT;
        this->upperActionFunc(this, play);

        return false;
    }

    return true;
}

s32 to_bow_ready_check(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;

    if (this->heldItemAction != PLAYER_IA_BOOMERANG) {
        if (!bow_ready_init_check(this, play)) {
            return 0;
        }

        if (!hook_check(this)) {
            anim = &gPlayerAnim_link_bow_bow_ready;
        } else {
            anim = &gPlayerAnim_link_hook_shot_ready;
        }
        Skeleton_Info_Rom_init_standard_stop(play, &this->upperSkelAnime, anim);
    } else {
        Player_actor_set_uperbody_process(this, uperbody_boom_ready);
        this->unk_834 = 10;
        Skeleton_Info_Rom_init_standard_stop(play, &this->upperSkelAnime, &gPlayerAnim_link_boom_throw_wait2waitR);
    }

    if (this->stateFlags1 & PLAYER_STATE1_23) {
        anime_init_standard_repeat(play, this, &gPlayerAnim_link_uma_anim_walk);
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && !anchor_mode_check_set(this)) {
        anime_init_standard_repeat(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType));
    }

    return 1;
}

int bow_game_trigger_key_check(PlayState* play) {
    return (play->shootingGalleryStatus > 0) && CHECK_BTN_ALL(pad->press.button, BTN_B);
}

int bow_game_also_key_check(PlayState* play) {
    return (play->shootingGalleryStatus != 0) &&
           ((play->shootingGalleryStatus < 0) ||
            CHECK_BTN_ANY(pad->cur.button, BTN_A | BTN_B | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT));
}

s32 to_bow_view_set_check(Player* this, PlayState* play) {
    if ((this->unk_6AD == 0) || (this->unk_6AD == 2)) {
        if (anchor_parallel_mode_check(this) ||
            (useCameraModeOK(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_MODE_AIM_ADULT) == 0)) {
            return 1;
        }
        this->unk_6AD = 2;
    }

    return 0;
}

s32 to_uperbody_bow_check(Player* this, PlayState* play) {
    if ((this->doorType == PLAYER_DOORTYPE_NONE) && !(this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN)) {
        if (item_key || bow_game_trigger_key_check(play)) {
            if (to_bow_ready_check(this, play)) {
                return to_bow_view_set_check(this, play);
            }
        }
    }

    return 0;
}

s32 hook_shoot_end_check(Player* this) {
    if (this->actor.child != NULL) {
        if (this->heldActor == NULL) {
            this->heldActor = this->actor.child;
            player_rumble_entry(this, 255, 10, 250, 0);
            player_SE_set(this, NA_SE_IT_HOOKSHOT_RECEIVE);
        }

        return 1;
    }

    return 0;
}

s32 uperbody_bow_wait(Player* this, PlayState* play) {
    if (this->unk_860 >= 0) {
        this->unk_860 = -this->unk_860;
    }

    if ((!hook_check(this) || hook_shoot_end_check(this)) && !to_uperbody_defense_check(play, this) &&
        !to_uperbody_bow_check(this, play)) {
        return false;
    }

    return true;
}

s32 bow_shoot_set(PlayState* play, Player* this) {
    s32 item;
    s32 arrowType;

    if (this->heldActor != NULL) {
        if (!hook_check(this)) {
            bow_count_check(play, this, &item, &arrowType);

            if (z_common_data.minigameState == 1) {
                play->interfaceCtx.hbaAmmo--;
            } else if (play->shootingGalleryStatus != 0) {
                play->shootingGalleryStatus--;
            } else {
                item_count_vary(item, -1);
            }

            if (play->shootingGalleryStatus == 1) {
                play->shootingGalleryStatus = -10;
            }

            player_rumble_entry(this, 150, 10, 150, 0);
        } else {
            player_rumble_entry(this, 255, 20, 150, 0);
        }

        this->unk_A73 = 4;
        this->heldActor->parent = NULL;
        this->actor.child = NULL;
        this->heldActor = NULL;

        return 1;
    }

    return 0;
}

static u16 shot_SE_data[] = { NA_SE_IT_BOW_FLICK, NA_SE_IT_SLING_FLICK };

s32 uperbody_bow_ready(Player* this, PlayState* play) {
    s32 sp2C;

    if (!hook_check(this)) {
        sp2C = 0;
    } else {
        sp2C = 1;
    }

    chase_angle(&this->upperLimbRot.z, 1200, 400);
    this->unk_6AE_rotFlags |= UNK6AE_ROT_UPPER_Z;

    if ((this->unk_836 == 0) && (wait_anime_check(this) == IDLE_ANIM_NONE) &&
        (this->skelAnime.animation == &gPlayerAnim_link_bow_side_walk)) {
        Skeleton_Info_Rom_init_standard_stop(play, &this->upperSkelAnime, walk2ready_anm[sp2C]);
        this->unk_836 = -1;
    } else if (Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime)) {
        Skeleton_Info_Rom_init_standard_repeat(play, &this->upperSkelAnime, wait_anm[sp2C]);
        this->unk_836 = 1;
    } else if (this->unk_836 == 1) {
        this->unk_836 = 2;
    }

    if (this->unk_834 > 10) {
        this->unk_834--;
    }

    to_bow_view_set_check(this, play);

    if ((this->unk_836 > 0) && ((this->unk_860 < 0) || (!item_also_key && !bow_game_also_key_check(play)))) {
        Player_actor_set_uperbody_process(this, uperbody_bow_shoot);
        if (this->unk_860 >= 0) {
            if (sp2C == 0) {
                if (!bow_shoot_set(play, this)) {
                    player_SE_set(this, shot_SE_data[ABS(this->unk_860) - 1]);
                }
            } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                bow_shoot_set(play, this);
            }
        }
        this->unk_834 = 10;
        speedF_clear(this);
    } else {
        this->stateFlags1 |= PLAYER_STATE1_9;
    }

    return true;
}

s32 uperbody_bow_shoot(Player* this, PlayState* play) {
    Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime);

    if (hook_check(this) && !hook_shoot_end_check(this)) {
        return true;
    }

    if (!to_uperbody_defense_check(play, this) &&
        (item_key || ((this->unk_860 < 0) && item_also_key) || bow_game_trigger_key_check(play))) {
        this->unk_860 = ABS(this->unk_860);

        if (bow_ready_init_check(this, play)) {
            if (hook_check(this)) {
                this->unk_836 = 1;
            } else {
                Skeleton_Info_Rom_init_standard_stop(play, &this->upperSkelAnime, &gPlayerAnim_link_bow_bow_shoot_next);
            }
        }
    } else {
        if (this->unk_834 != 0) {
            this->unk_834--;
        }

        if (anchor_parallel_mode_check(this) || (this->unk_6AD != 0) || (this->stateFlags1 & PLAYER_STATE1_20)) {
            if (this->unk_834 == 0) {
                this->unk_834++;
            }

            return true;
        }

        if (hook_check(this)) {
            Player_actor_set_uperbody_process(this, uperbody_bow_wait);
        } else {
            Player_actor_set_uperbody_process(this, uperbody_bow_shoot_end);
            Skeleton_Info_Rom_init_standard_stop(play, &this->upperSkelAnime, &gPlayerAnim_link_bow_bow_shoot_end);
        }

        this->unk_834 = 0;
    }

    return true;
}

s32 uperbody_bow_shoot_end(Player* this, PlayState* play) {
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime)) {
        Player_actor_set_uperbody_process(this, uperbody_bow_wait);
    }

    return true;
}

void parallel_camera_init(Player* this) {
    this->stateFlags1 |= PLAYER_STATE1_PARALLEL;

    if (!(this->skelAnime.movementFlags & ANIM_FLAG_OVERRIDE_MOVEMENT) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) && (wall_distance_angle_y < 0x2000)) {
        // snap to the wall
        this->yaw = this->actor.shape.rot.y = this->actor.wallYaw + 0x8000;
    }

    this->parallelYaw = this->actor.shape.rot.y;
}

s32 carry_cancel_check(PlayState* play, Player* this, Actor* arg2) {
    if (arg2 == NULL) {
        action_reset_all(play, this);
        to_wait_set_check(this, play);
        return 1;
    }

    return 0;
}

void to_uperbody_carry_init(Player* this, PlayState* play) {
    if (!carry_cancel_check(play, this, this->heldActor)) {
        Player_actor_set_uperbody_process(this, uperbody_carry);
        Skeleton_Info_Rom_init_standard_repeat(play, &this->upperSkelAnime, &gPlayerAnim_link_normal_carryB_wait);
    }
}

s32 uperbody_carry(Player* this, PlayState* play) {
#if OOT_VERSION >= NTSC_1_1
    Actor* heldActor = this->heldActor;

    if (heldActor == NULL) {
        Player_actor_uperbody_process_init(play, this);
    }
#endif

    if (to_uperbody_defense_check(play, this)) {
        return true;
    }

    if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
#if OOT_VERSION < NTSC_1_1
        Actor* heldActor;
#endif

        if (Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime)) {
            Skeleton_Info_Rom_init_standard_repeat(play, &this->upperSkelAnime, &gPlayerAnim_link_normal_carryB_wait);
        }

#if OOT_VERSION < NTSC_1_1
        heldActor = this->heldActor;
        if (heldActor == NULL) {
            Player_actor_uperbody_process_init(play, this);
            return true;
        }
#endif

        if ((heldActor->id == ACTOR_EN_NIW) && (this->actor.velocity.y <= 0.0f)) {
            this->actor.minVelocityY = -2.0f;
            this->actor.gravity = -0.5f;
            this->fallStartHeight = this->actor.world.pos.y;
        }

        return true;
    }

    return uperbody_non(this, play);
}

void L_hand_shape_set(Player* this, Gfx** dLists) {
    this->leftHandDLists = dLists + z_common_data.save.linkAge;
}

s32 uperbody_boom_wait(Player* this, PlayState* play) {
    if (to_uperbody_defense_check(play, this)) {
        return true;
    }

    if (this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN) {
        Player_actor_set_uperbody_process(this, uperbody_boom_throw_wait);
    } else if (to_uperbody_bow_check(this, play)) {
        return true;
    }

    return false;
}

s32 uperbody_boom_ready(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime)) {
        Player_actor_set_uperbody_process(this, uperbody_boom_ready_wait);
        Skeleton_Info_Rom_init_standard_repeat(play, &this->upperSkelAnime, &gPlayerAnim_link_boom_throw_waitR);
    }

    to_bow_view_set_check(this, play);

    return true;
}

s32 uperbody_boom_ready_wait(Player* this, PlayState* play) {
    LinkAnimationHeader* animSeg = this->skelAnime.animation;

    if ((waitR_anime_select(this) == animSeg) || (waitL_anime_select(this) == animSeg) || (side_walkR_anime_select(this) == animSeg) ||
        (side_walkL_anime_select(this) == animSeg)) {
        Skeleton_Proc_Copy_init(play, this->skelAnime.limbCount, this->upperSkelAnime.jointTable,
                              this->skelAnime.jointTable);
    } else {
        Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime);
    }

    to_bow_view_set_check(this, play);

    if (!item_also_key) {
        Player_actor_set_uperbody_process(this, uperbody_boom_throw);
        Skeleton_Info_Rom_init_standard_stop(play, &this->upperSkelAnime,
                               (this->unk_870 < 0.5f) ? &gPlayerAnim_link_boom_throwR : &gPlayerAnim_link_boom_throwL);
    }

    return true;
}

s32 uperbody_boom_throw(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime)) {
        Player_actor_set_uperbody_process(this, uperbody_boom_throw_wait);
        this->unk_834 = 0;
    } else if (Skeleton_Info_Rom_frame_check(&this->upperSkelAnime, 6.0f)) {
        f32 posX = (sin_s(this->actor.shape.rot.y) * 10.0f) + this->actor.world.pos.x;
        f32 posZ = (cos_s(this->actor.shape.rot.y) * 10.0f) + this->actor.world.pos.z;
        s32 yaw = (this->focusActor != NULL) ? this->actor.shape.rot.y + 14000 : this->actor.shape.rot.y;
        EnBoom* boomerang =
            (EnBoom*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOOM, posX, this->actor.world.pos.y + 30.0f, posZ,
                                 this->actor.focus.rot.x, yaw, 0, 0);

        this->boomerangActor = &boomerang->actor;

        if (boomerang != NULL) {
            boomerang->moveTo = this->focusActor;
            boomerang->returnTimer = 20;
            this->stateFlags1 |= PLAYER_STATE1_BOOMERANG_THROWN;

            if (!anchor_mode_check(this)) {
                parallel_camera_init(this);
            }

            this->unk_A73 = 4;
            player_SE_set(this, NA_SE_IT_BOOMERANG_THROW);
            player_voice_SE_set(this, NA_SE_VO_LI_SWORD_N);
        }
    }

    return true;
}

s32 uperbody_boom_throw_wait(Player* this, PlayState* play) {
    if (to_uperbody_defense_check(play, this)) {
        return true;
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN)) {
        Player_actor_set_uperbody_process(this, uperbody_boom_catch);
        Skeleton_Info_Rom_init_standard_stop(play, &this->upperSkelAnime, &gPlayerAnim_link_boom_catch);
        L_hand_shape_set(this, l_boomhand_model_data);
        player_SE_set(this, NA_SE_PL_CATCH_BOOMERANG);
        player_voice_SE_set(this, NA_SE_VO_LI_SWORD_N);

        return true;
    }

    return false;
}

s32 uperbody_boom_catch(Player* this, PlayState* play) {
    if (!uperbody_boom_wait(this, play) && Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime)) {
        Player_actor_set_uperbody_process(this, uperbody_boom_wait);
    }

    return true;
}

s32 Player_actor_set_process(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 flags) {
    if (actionFunc == this->actionFunc) {
        return 0;
    }

    if (move_okarina == this->actionFunc) {
        Na_SetOcarinaModeFlag(OCARINA_INSTRUMENT_OFF);
        this->stateFlags2 &= ~(PLAYER_STATE2_24 | PLAYER_STATE2_25);
    } else if (move_magic_wait == this->actionFunc) {
        sub_camera_mode_reset(play, this);
    }

    this->actionFunc = actionFunc;

    if ((this->itemAction != this->heldItemAction) &&
        (!(flags & 1) || !(this->stateFlags1 & PLAYER_STATE1_SHIELDING))) {
        now_item_cancel(this);
    }

    if (!(flags & 1) && !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
        Player_actor_uperbody_process_init(play, this);
        this->stateFlags1 &= ~PLAYER_STATE1_SHIELDING;
    }

    anime_move_reset(this);

    this->stateFlags1 &= ~(PLAYER_STATE1_2 | PLAYER_STATE1_TALKING | PLAYER_STATE1_26 | PLAYER_STATE1_28 |
                           PLAYER_STATE1_29 | PLAYER_STATE1_31);
    this->stateFlags2 &= ~(PLAYER_STATE2_19 | PLAYER_STATE2_USING_OCARINA | PLAYER_STATE2_IDLE_FIDGET);
    this->stateFlags3 &= ~(PLAYER_STATE3_1 | PLAYER_STATE3_3 | PLAYER_STATE3_FLYING_WITH_HOOKSHOT);

    this->av1.actionVar1 = 0;
    this->av2.actionVar2 = 0;

    this->idleType = PLAYER_IDLE_DEFAULT;

    player_voice_SE_stop(this);

    return 1;
}

/**
 * Calls `Player_actor_set_process` to setup a new action, but takes extra measures to
 * preserve AnimMovement while doing so.
 */
void Player_actor_anime_move_save_set_process(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 flags) {
    s32 savedMovementFlags;

    savedMovementFlags = this->skelAnime.movementFlags;

    // Setting `skelAnime.movementFlags` to 0 will prevent `anime_move_reset` from ending
    // AnimMovement when `Player_actor_set_process` is called.
    this->skelAnime.movementFlags = 0;

    Player_actor_set_process(play, this, actionFunc, flags);
    this->skelAnime.movementFlags = savedMovementFlags;
}

/**
 * Calls `Player_actor_set_process` to setup a new action, but takes extra measures to
 * preserve the current itemAction while doing so.
 *
 * Note that `itemAction` must be PLAYER_IA_NONE or higher for the action change to take place.
 */
void Player_actor_now_A_item_save_set_process(PlayState* play, Player* this, PlayerActionFunc actionFunc, s32 flags) {
    s32 savedItemAction;

    if (this->itemAction >= PLAYER_IA_NONE) {
        savedItemAction = this->itemAction;

        // Setting `itemAction` to `heldItemAction` will prevent `now_item_cancel` from running when
        // `Player_actor_set_process` is called.
        this->itemAction = this->heldItemAction;

        Player_actor_set_process(play, this, actionFunc, flags);
        this->itemAction = savedItemAction;
        shape_set(this, item_shape_type_set(this, this->itemAction));
    }
}

void change_camera_check_set(PlayState* play, s16 camSetting) {
    if (!Game_play_change_camera_check(play)) {
        if (camSetting == CAM_SET_SCENE_TRANSITION) {
            alpha_change(HUD_VISIBILITY_NOTHING_ALT);
        }
    } else {
        changeCameraSet(Gama_play_get_camera(play, CAM_ID_MAIN), camSetting);
    }
}

void change_item_camera_check_set(PlayState* play, s32 arg1) {
    change_camera_check_set(play, CAM_SET_TURN_AROUND);
    setCameraData(Gama_play_get_camera(play, CAM_ID_MAIN), 4, NULL, NULL, arg1, 0, 0);
}

void hook_cancel_check(Player* this) {
    if (hook_check(this)) {
        Actor* heldActor = this->heldActor;

        if (heldActor != NULL) {
            Actor_delete(heldActor);
            this->actor.child = NULL;
            this->heldActor = NULL;
        }
    }
}

void player_item_on(PlayState* play, Player* this, s32 item) {
    s8 itemAction;
    s32 temp;
    s32 nextAnimType;

    itemAction = Player_item_type_check(item);

    if (((this->heldItemAction == this->itemAction) &&
         (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) || (sword_item_check(itemAction) != 0) ||
          (itemAction == PLAYER_IA_NONE))) ||
        ((this->itemAction < 0) && ((sword_item_check(itemAction) != 0) || (itemAction == PLAYER_IA_NONE)))) {

        if ((itemAction == PLAYER_IA_NONE) || !(this->stateFlags1 & PLAYER_STATE1_27) ||
            ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
             ((itemAction == PLAYER_IA_HOOKSHOT) || (itemAction == PLAYER_IA_LONGSHOT)))) {

            if ((play->bombchuBowlingStatus == 0) &&
                (((itemAction == PLAYER_IA_DEKU_STICK) && (AMMO(ITEM_DEKU_STICK) == 0)) ||
                 ((itemAction == PLAYER_IA_MAGIC_BEAN) && (AMMO(ITEM_MAGIC_BEAN) == 0)) ||
                 (temp = bom_item_check(this, itemAction),
                  ((temp >= 0) && ((AMMO(bom_status[temp].itemId) == 0) ||
                                   (play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].length >= 3)))))) {
                // Prevent some items from being used if player is out of ammo.
                // Also prevent explosives from being used if there are 3 or more active (outside of bombchu bowling)
                Na_StartSystemSe_F(NA_SE_SY_ERROR);
            } else if (itemAction == PLAYER_IA_LENS_OF_TRUTH) {
                // Handle Lens of Truth
                if (magic_meter_check(play, 0, MAGIC_CONSUME_LENS)) {
                    if (play->actorCtx.lensActive) {
                        magic_grass_cancel(play);
                    } else {
                        play->actorCtx.lensActive = true;
                    }

                    Na_StartSystemSe_F((play->actorCtx.lensActive) ? NA_SE_SY_GLASSMODE_ON : NA_SE_SY_GLASSMODE_OFF);
                } else {
                    Na_StartSystemSe_F(NA_SE_SY_ERROR);
                }
            } else if (itemAction == PLAYER_IA_DEKU_NUT) {
                // Handle Deku Nuts
                if (AMMO(ITEM_DEKU_NUT) != 0) {
                    to_light_bom_check(play, this);
                } else {
                    Na_StartSystemSe_F(NA_SE_SY_ERROR);
                }
            } else if ((temp = magic_item_check(this, itemAction)) >= 0) {
                // Handle magic spells
                if (((itemAction == PLAYER_IA_FARORES_WIND) && (z_common_data.respawn[RESPAWN_MODE_TOP].data > 0)) ||
                    ((z_common_data.magicCapacity != 0) && (z_common_data.magicState == MAGIC_STATE_IDLE) &&
                     (z_common_data.save.info.playerData.magic >= magic_power[temp]))) {
                    this->itemAction = itemAction;
                    this->unk_6AD = 4;
                } else {
                    Na_StartSystemSe_F(NA_SE_SY_ERROR);
                }
            } else if (itemAction >= PLAYER_IA_MASK_KEATON) {
                // Handle wearable masks
                if (this->currentMask != PLAYER_MASK_NONE) {
                    this->currentMask = PLAYER_MASK_NONE;
                } else {
                    this->currentMask = itemAction - PLAYER_IA_MASK_KEATON + 1;
                }

                player_big_SE_set(this, NA_SE_PL_CHANGE_ARMS);
            } else if (((itemAction >= PLAYER_IA_OCARINA_FAIRY) && (itemAction <= PLAYER_IA_OCARINA_OF_TIME)) ||
                       (itemAction >= PLAYER_IA_BOTTLE_FISH)) {
                // Handle "cutscene items"
                if (!anchor_mode_check(this) ||
                    ((itemAction >= PLAYER_IA_BOTTLE_POTION_RED) && (itemAction <= PLAYER_IA_BOTTLE_FAIRY))) {
                    Actor_Name_Disp_Cancel(play, &play->actorCtx.titleCtx);
                    this->unk_6AD = 4;
                    this->itemAction = itemAction;
                }
            } else if ((itemAction != this->heldItemAction) ||
                       ((this->heldActor == NULL) && (bom_item_check(this, itemAction) >= 0))) {
                // Handle using a new held item
                this->nextModelGroup = item_shape_type_set(this, itemAction);
                nextAnimType = player_shape_type[this->nextModelGroup][PLAYER_MODELGROUPENTRY_ANIM];

                if ((this->heldItemAction >= 0) && (magic_item_check(this, itemAction) < 0) &&
                    (item != this->heldItemId) &&
                    (item_change_table[player_shape_type[this->modelGroup][PLAYER_MODELGROUPENTRY_ANIM]][nextAnimType] !=
                     PLAYER_ITEM_CHG_0)) {
                    // Start the held item change process
                    this->heldItemId = item;
                    this->stateFlags1 |= PLAYER_STATE1_START_CHANGING_HELD_ITEM;
                } else {
                    // Init new held item for use
                    hook_cancel_check(this);
                    l_hand_child_cancel(play, this);
                    item_change_anime_check(play, this, itemAction);
                }
            } else {
                // Handle using the held item already in hand
                item_key = item_also_key = true;
            }
        }
    }
}

void to_down_set(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    s32 cond = swim_check(this);

    action_reset_all(play, this);

    Player_actor_set_process(play, this, cond ? move_swim_down : move_down, 0);

    this->stateFlags1 |= PLAYER_STATE1_DEAD;

    anime_init_standard_stop(play, this, anim);
    if (anim == &gPlayerAnim_link_derth_rebirth) {
        this->skelAnime.endFrame = 84.0f;
    }

    speedF_look_mode_clear(this);
    player_voice_SE_set(this, NA_SE_VO_LI_DOWN);

    if (this->actor.category == ACTORCAT_PLAYER) {
        Na_SetLinkDownSound();

        if (bottle_fairy_use(play)) {
            play->gameOverCtx.state = GAMEOVER_REVIVE_START;
            this->av1.actionVar1 = 1;
        } else {
            play->gameOverCtx.state = GAMEOVER_DEATH_START;
            Na_StopAllBgm(0);
            Na_StartFanfare(NA_BGM_GAME_OVER);
            z_common_data.seqId = (u8)NA_BGM_DISABLED;
            z_common_data.natureAmbienceId = NATURE_ID_DISABLED;
        }

        makeOnepointDemo(play, 9806, cond ? 120 : 60, &this->actor, CAM_ID_MAIN);
        shrink_window_setval(32);
    }
}

int item_action_ok_check(Player* this) {
    return (!(move_demo_init == this->actionFunc) ||
            ((this->stateFlags1 & PLAYER_STATE1_START_CHANGING_HELD_ITEM) &&
             ((this->heldItemId == ITEM_SWORD_CS) || (this->heldItemId == ITEM_NONE)))) &&
           (!(uperbody_nom2figh_play == this->upperActionFunc) ||
            (Player_item_type_check(this->heldItemId) == this->heldItemAction));
}

/**
 * Updates the Upper Body system.
 * The Upper Body system is composed of an upper action function and
 * a separate skelanime that can play an animation which is different
 * from the main skelanime.
 *
 * @return true if the upper body is "busy", false otherwise.
 *
 * The upper body being "busy" can mean a few things:
 * - Hookshot has just connected with something that Player can fly to
 * - A deku nut is currently being thrown
 * - The current upper action function has indicated that it is busy
 *
 * If an upper action indicates being busy by returning true, the
 * animation playing in the upper body skeleton will be used.
 * This animation may be used for all limbs or only the upper body limbs
 * depending on some conditions. See details below.
 */
s32 uperbody_action_check(Player* this, PlayState* play) {
    if (!(this->stateFlags1 & PLAYER_STATE1_23) && (this->actor.parent != NULL) && hook_check(this)) {
        Player_actor_set_process(play, this, move_hook_fly, 1);
        this->stateFlags3 |= PLAYER_STATE3_FLYING_WITH_HOOKSHOT;
        anime_init_standard_stop(play, this, &gPlayerAnim_link_hook_fly_start);
        anime_move_init(play, this,
                                 ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_ENABLE_MOVEMENT |
                                     ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);
        speedF_look_mode_clear(this);
        this->yaw = this->actor.shape.rot.y;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        this->hoverBootsTimer = 0;
        this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_FOCUS_Y | UNK6AE_ROT_UPPER_X;
        player_voice_SE_set(this, NA_SE_VO_LI_LASH);
        return true;
    }

    if (item_action_ok_check(this)) {
        to_uperbody_item_change_check(this, play);
        if (move_light_bom == this->actionFunc) {
            return true;
        }
    }

    if (!this->upperActionFunc(this, play)) {
        return false;
    }

    if (this->upperAnimInterpWeight != 0.0f) {
        // The functionality contained within this block of code is never used in practice
        // because `upperAnimInterpWeight` is always 0.
        if ((wait_anime_check(this) == IDLE_ANIM_NONE) || (this->speedXZ != 0.0f)) {
            Skeleton_Proc_Add_reverse_init(play, this->skelAnime.limbCount, this->upperSkelAnime.jointTable,
                                                  this->skelAnime.jointTable, link_joint_status);
        }
        chase_f(&this->upperAnimInterpWeight, 0.0f, 0.25f);
        Skeleton_Proc_Morf_init(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                this->upperSkelAnime.jointTable, 1.0f - this->upperAnimInterpWeight);
    } else if ((wait_anime_check(this) == IDLE_ANIM_NONE) || (this->speedXZ != 0.0f)) {
        // Only copy the upper body animation to the upper body limbs in the main skeleton.
        // Doing so allows the main skeleton to play its own animation for the lower body limbs.
        Skeleton_Proc_Add_init(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                      this->upperSkelAnime.jointTable, link_joint_status);
    } else {
        // Copy all of the upper body animation into the whole main skeleton.
        // The upper body has full control of all limbs.
        Skeleton_Proc_Copy_init(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                              this->upperSkelAnime.jointTable);
    }

    return true;
}

/**
 * Sets up `move_demo_init`, which will allow the held item put away process
 * to complete before moving on to a new action.
 *
 * The function provided by the `afterPutAwayFunc` argument will run after the put away is complete.
 * This function is expected to set a new action and move execution away from `move_demo_init`.
 *
 * @return  From `to_no_item_set`: true if an item needs to be put away, false if not.
 */
s32 Player_actor_set_demo_init_proc(PlayState* play, Player* this, AfterPutAwayFunc afterPutAwayFunc) {
    this->afterPutAwayFunc = afterPutAwayFunc;
    Player_actor_set_process(play, this, move_demo_init, 0);
    this->stateFlags2 |= PLAYER_STATE2_6;

    return to_no_item_set(play, this);
}

/**
 * Updates Shape Yaw (`shape.rot.y`). In other words, the Y rotation of Player's model.
 * This does not affect the direction Player will move in.
 *
 * There are 3 modes shape yaw can be updated with, based on player state:
 *     - Lock on:  Rotates Player to face the current lock on target.
 *     - Parallel: Rotates Player to face the current Parallel angle, set when Z-Targeting without an actor lock-on
 *     - Normal:   Rotates Player to face `this->yaw`, the direction he is currently moving
 */
void anime_angle_control(Player* this, PlayState* play) {
    s16 previousYaw = this->actor.shape.rot.y;

    if (!(this->stateFlags2 & (PLAYER_STATE2_5 | PLAYER_STATE2_6))) {
        Actor* focusActor = this->focusActor;

        if ((focusActor != NULL) &&
            ((play->actorCtx.attention.reticleSpinCounter != 0) || (this->actor.category != ACTORCAT_PLAYER))) {
            chase_angle(&this->actor.shape.rot.y, search_position_angleY(&this->actor.world.pos, &focusActor->focus.pos),
                               4000);
        } else if ((this->stateFlags1 & PLAYER_STATE1_PARALLEL) &&
                   !(this->stateFlags2 & (PLAYER_STATE2_5 | PLAYER_STATE2_6))) {
            chase_angle(&this->actor.shape.rot.y, this->parallelYaw, 4000);
        }
    } else if (!(this->stateFlags2 & PLAYER_STATE2_6)) {
        chase_angle(&this->actor.shape.rot.y, this->yaw, 2000);
    }

    this->unk_87C = this->actor.shape.rot.y - previousYaw;
}

/**
 * Step a value by `step` to a `target` value.
 * Constrains the value to be no further than `constraintRange` from `constraintMid` (accounting for wrapping).
 * Constrains the value to be no further than `overflowRange` from 0.
 * If this second constraint is enforced, return how much the value was past by the range, or return 0.
 *
 * @return The amount by which the value overflowed the absolute range defined by `overflowRange`
 */
s32 chain_angle_set(s16* pValue, s16 target, s16 step, s16 overflowRange, s16 constraintMid,
                                   s16 constraintRange) {
    s16 diff;
    s16 clampedDiff;
    s16 valueBeforeOverflowClamp;

    // Clamp value to [constraintMid - constraintRange , constraintMid + constraintRange]
    // This is more involved than a simple `CLAMP`, to account for binang wrapping
    diff = clampedDiff = constraintMid - *pValue;
    clampedDiff = CLAMP(clampedDiff, -constraintRange, constraintRange);
    *pValue += (s16)(diff - clampedDiff);

    chase_angle(pValue, target, step);

    valueBeforeOverflowClamp = *pValue;
    if (*pValue < -overflowRange) {
        *pValue = -overflowRange;
    } else if (*pValue > overflowRange) {
        *pValue = overflowRange;
    }
    return valueBeforeOverflowClamp - *pValue;
}

s32 eye_move_anime_set(Player* this, s32 arg1) {
    s16 targetUpperBodyYaw;
    s16 yaw;

    yaw = this->actor.shape.rot.y;
    if (arg1) {
        yaw = this->actor.focus.rot.y;
        this->upperLimbRot.x = this->actor.focus.rot.x;
        this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_UPPER_X;
    } else {
        // Step the head pitch to the focus pitch.
        // If the head cannot be pitched enough, pitch the upper body.
        chain_angle_set(&this->upperLimbRot.x,
                                       chain_angle_set(&this->headLimbRot.x, this->actor.focus.rot.x,
                                                                      600, 10000, this->actor.focus.rot.x, 0),
                                       200, 4000, this->headLimbRot.x, 10000);

        // Step the upper body and head yaw to the focus yaw.
        // Eventually prefers turning the upper body rather than the head.
        targetUpperBodyYaw = this->actor.focus.rot.y - yaw;
        chain_angle_set(&targetUpperBodyYaw, 0, 200, 24000, this->upperLimbRot.y, 8000);
        yaw = this->actor.focus.rot.y - targetUpperBodyYaw;
        chain_angle_set(&this->headLimbRot.y, targetUpperBodyYaw - this->upperLimbRot.y, 200, 8000,
                                       targetUpperBodyYaw, 8000);
        chain_angle_set(&this->upperLimbRot.y, targetUpperBodyYaw, 200, 8000, this->headLimbRot.y, 8000);

        this->unk_6AE_rotFlags |=
            UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_HEAD_X | UNK6AE_ROT_HEAD_Y | UNK6AE_ROT_UPPER_X | UNK6AE_ROT_UPPER_Y;
    }

    return yaw;
}

/**
 * Updates state related to Z-Targeting.
 *
 * Z-Targeting is an umbrella term for two main states:
 * - Actor Lock-on: Player has locked onto an actor, a reticle appears, both Player and the camera focus on the actor.
 * - Parallel: Player and the camera keep facing the same angle from when Z was pressed. Can snap to walls.
 *             This state occurs when there are no actors available to lock onto.
 *
 * First this function updates `zTargetActiveTimer`. For most Z-Target related states to update, this
 * timer has to have a non-zero value. Additionally, the timer must have a value of 5 or greater
 * for the Attention system to recognize that an actor lock-on is active.
 *
 * Following this, a next lock-on actor is chosen. If there is currently no actor lock-on active, the actor
 * Navi is hovering over will be chosen. If there is an active lock-on, the next available
 * lock-on will be the actor with an arrow hovering above it.
 *
 * If the above regarding actor lock-on does not occur, then Z-Parallel can begin.
 *
 * Lastly, the function handles updating general "actor focus" state. This applies to non Z-Target states
 * like talking to an actor. If the current focus actor is not considered "hostile", then
 * `PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS` can be set. This flag being set will trigger `camera_mode_check`
 * to make the camera focus on the current focus actor.
 */
void anchor_check(Player* this, PlayState* play) {
    s32 ignoreLeash = false;
    s32 zButtonHeld = CHECK_BTN_ALL(pad->cur.button, BTN_Z);
    Actor* nextLockOnActor;
    s32 padding;
    s32 usingHoldTargeting;
    s32 isTalking;

    if (!zButtonHeld) {
        this->stateFlags1 &= ~PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE;
    }

    if ((play->csCtx.state != CS_STATE_IDLE) || (this->csAction != PLAYER_CSACTION_NONE) ||
        (this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_29)) ||
        (this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT)) {
        // Don't allow Z-Targeting in various states
        this->zTargetActiveTimer = 0;
    } else if (zButtonHeld || (this->stateFlags2 & PLAYER_STATE2_LOCK_ON_WITH_SWITCH) ||
               (this->autoLockOnActor != NULL)) {
        // While a lock-on is active, decrement the timer and hold it at 5.
        // Values under 5 indicate a lock-on has ended and will make the reticle release.
        // See usage toward the end of `Actor_info_call_actor`.
        //
        // `zButtonHeld` will also be true for Parallel. This is necessary because the timer
        // needs to be non-zero for `parallel_camera_init` to be able to run below.
        if (this->zTargetActiveTimer <= 5) {
            this->zTargetActiveTimer = 5;
        } else {
            this->zTargetActiveTimer--;
        }
    } else if (this->stateFlags1 & PLAYER_STATE1_PARALLEL) {
        // If the above code block which checks `zButtonHeld` is not taken, that means Z has been released.
        // In that case, setting `zTargetActiveTimer` to 0 will stop Parallel if it is currently active.
        this->zTargetActiveTimer = 0;
    } else if (this->zTargetActiveTimer != 0) {
        this->zTargetActiveTimer--;
    }

    if (this->zTargetActiveTimer >= 6) {
        // When a lock-on is started, `zTargetActiveTimer` will be set to 15 and then immediately start decrementing
        // down to 5. During this 10 frame period, set `ignoreLeash` so that the lock-on will temporarily
        // have an infinite leash distance.
        // This gives time for the reticle to settle while it locks on, even if the player leaves the leash range.
        ignoreLeash = true;
    }

    isTalking = player_talking_now(play);

    if (isTalking || (this->zTargetActiveTimer != 0) ||
        (this->stateFlags1 & (PLAYER_STATE1_CHARGING_SPIN_ATTACK | PLAYER_STATE1_BOOMERANG_THROWN))) {
        if (!isTalking) {
            if (!(this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN) &&
                ((this->heldItemAction != PLAYER_IA_FISHING_POLE) || (this->unk_860 == 0)) &&
                CHECK_BTN_ALL(pad->press.button, BTN_Z)) {
                if (this->actor.category == ACTORCAT_PLAYER) {
                    // The next lock-on actor defaults to the actor Navi is hovering over.
                    // This may change to the arrow hover actor below.
                    nextLockOnActor = play->actorCtx.attention.naviHoverActor;
                } else {
                    // Dark Link will always lock onto the player.
                    nextLockOnActor = &GET_PLAYER(play)->actor;
                }

                // Get saved Z Target setting.
                // Dark Link uses Hold Targeting.
                usingHoldTargeting = (z_common_data.zTargetSetting != 0) || (this->actor.category != ACTORCAT_PLAYER);

                this->stateFlags1 |= PLAYER_STATE1_Z_TARGETING;

                if ((nextLockOnActor != NULL) && !(nextLockOnActor->flags & ACTOR_FLAG_LOCK_ON_DISABLED)) {

                    // Navi hovers over the current lock-on actor, so `nextLockOnActor` and `focusActor`
                    // will be the same if already locked on.
                    // In this case, `nextLockOnActor` will be the arrow hover actor instead.
                    if ((nextLockOnActor == this->focusActor) && (this->actor.category == ACTORCAT_PLAYER)) {
                        nextLockOnActor = play->actorCtx.attention.arrowHoverActor;
                    }

                    if (nextLockOnActor != this->focusActor) {
                        // Set new lock-on

                        if (!usingHoldTargeting) {
                            this->stateFlags2 |= PLAYER_STATE2_LOCK_ON_WITH_SWITCH;
                        }

                        this->focusActor = nextLockOnActor;
                        this->zTargetActiveTimer = 15;
                        this->stateFlags2 &= ~(PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER | PLAYER_STATE2_21);
                    } else {
                        if (!usingHoldTargeting) {
                            anchor_cancel(this);
                        }
                    }

                    this->stateFlags1 &= ~PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE;
                } else {
                    // Lock-on was not started above. Set Parallel Mode.
                    if (!(this->stateFlags1 & (PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE))) {
                        parallel_camera_init(this);
                    }
                }
            }

            if (this->focusActor != NULL) {
                if ((this->actor.category == ACTORCAT_PLAYER) && (this->focusActor != this->autoLockOnActor) &&
                    Actor_anchor_check(this->focusActor, this, ignoreLeash)) {
                    anchor_cancel(this);
                    this->stateFlags1 |= PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE;
                } else if (this->focusActor != NULL) {
                    this->focusActor->attentionPriority = 40;
                }
            } else if (this->autoLockOnActor != NULL) {
                // Because of the previous if condition above, `autoLockOnActor` does not take precedence
                // over `focusActor` if it already exists.
                // However, `autoLockOnActor` is expected to be set with `to_anchor_set`
                // which will release any existing lock-on before setting the new one.
                this->focusActor = this->autoLockOnActor;
            }
        }

        if (this->focusActor != NULL) {
            this->stateFlags1 &= ~(PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS | PLAYER_STATE1_PARALLEL);

            // Check if an actor is not hostile, aka "friendly", to set `PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS`.
            //
            // When carrying another actor, `PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS` will be set even if the actor
            // is hostile. This is a special case to allow Player to have more freedom of movement and be able
            // to throw a carried actor at the lock-on actor, even if it is hostile.
            if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) ||
                !CHECK_FLAG_ALL(this->focusActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)) {
                this->stateFlags1 |= PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS;
            }
        } else {
            if (this->stateFlags1 & PLAYER_STATE1_PARALLEL) {
                this->stateFlags2 &= ~PLAYER_STATE2_LOCK_ON_WITH_SWITCH;
            } else {
                anchor_reset(this);
            }
        }
    } else {
        anchor_reset(this);
    }
}

/**
 * These defines exist to simplify the variable used to toggle the different speed modes.
 * While the `speedMode` variable is a float and can contain a non-boolean value,
 * `stick_control_set` never actually uses the value for anything.
 * It simply checks if the value is non-zero to toggle the "curved" mode.
 * In practice, 0.0f or 0.018f are the only values passed to this function.
 *
 * It's clear that this value was intended to mean something in the curved mode calculation at
 * some point in development, but was either never implemented or removed.
 *
 * To see the difference between linear and curved mode, with interactive toggles for
 * speed cap and floor pitch, see the following desmos graph: https://www.desmos.com/calculator/hri7dcws4c
 */

// Linear mode is a straight line, increasing target speed at a steady rate relative to the control stick magnitude
#define SPEED_MODE_LINEAR 0.0f

// Curved mode drops any input below 20 units of magnitude, resulting in zero for target speed.
// Beyond 20 units, a gradual curve slowly moves up until around the 40 unit mark
// when target speed ramps up very quickly.
#define SPEED_MODE_CURVED 0.018f

/**
 * Calculates target speed and yaw based on input from the control stick.
 * See `stick_control_check_anchor` for detailed argument descriptions.
 *
 * @return true if the control stick has any magnitude, false otherwise.
 */
s32 stick_control_set(PlayState* play, Player* this, f32* outSpeedTarget, s16* outYawTarget,
                                           f32 speedMode) {
    f32 temp;
    f32 sinFloorPitch;
    f32 floorPitchInfluence;
    f32 speedCap;

    if ((this->unk_6AD != 0) || (play->transitionTrigger == TRANS_TRIGGER_START) ||
        (this->stateFlags1 & PLAYER_STATE1_0)) {
        *outSpeedTarget = 0.0f;
        *outYawTarget = this->actor.shape.rot.y;
    } else {
        *outSpeedTarget = stick_length;
        *outYawTarget = stick_angle;

        // The value of `speedMode` is never actually used. It only toggles this condition.
        // See the definition of `SPEED_MODE_LINEAR` and `SPEED_MODE_CURVED` for more information.
        if (speedMode != SPEED_MODE_LINEAR) {
            *outSpeedTarget -= 20.0f;

            if (*outSpeedTarget < 0.0f) {
                // If control stick magnitude is below 20, return zero speed.
                *outSpeedTarget = 0.0f;
            } else {
                // Cosine of the control stick magnitude isn't exactly meaningful, but
                // it happens to give a desirable curve for grounded movement speed relative
                // to control stick magnitude.
                temp = 1.0f - cos_s(*outSpeedTarget * 450.0f);
                *outSpeedTarget = (SQ(temp) * 30.0f) + 7.0f;
            }
        } else {
            // Speed increases linearly relative to control stick magnitude
            *outSpeedTarget *= 0.8f;
        }

        if (stick_length != 0.0f) {
            sinFloorPitch = sin_s(this->floorPitch);
            speedCap = this->unk_880;
            floorPitchInfluence = CLAMP(sinFloorPitch, 0.0f, 0.6f);

            if (this->unk_6C4 != 0.0f) {
                speedCap -= this->unk_6C4 * 0.008f;
                speedCap = CLAMP_MIN(speedCap, 2.0f);
            }

            *outSpeedTarget = (*outSpeedTarget * 0.14f) - (8.0f * floorPitchInfluence * floorPitchInfluence);
            *outSpeedTarget = CLAMP(*outSpeedTarget, 0.0f, speedCap);

            return true;
        }
    }

    return false;
}

/**
 * Steps speed toward zero to at a rate defined by current boot data.
 * After zero is reached, speed will be held at zero.
 *
 * @return true if speed is 0, false otherwise
 */
s32 ground_breaking_speedF(Player* this) {
    return chase_f(&this->speedXZ, 0.0f, R_DECELERATE_RATE / 100.0f);
}

/**
 * Gets target speed and yaw values for movement based on control stick input.
 * Control stick magnitude and angle are processed in `stick_control_set` to get target values.
 * Additionally, this function does extra processing on the target yaw value if the control stick is neutral.
 *
 * @param outSpeedTarget  a pointer to the variable that will hold the resulting target speed value
 * @param outYawTarget    a pointer to the variable that will hold the resulting target yaw value
 * @param speedMode       toggles between a linear and curved mode for the speed value
 *
 * @see stick_control_set for more information on the linear vs curved speed mode.
 *
 * @return true if the control stick has any magnitude, false otherwise.
 */
s32 stick_control_check_anchor(Player* this, f32* outSpeedTarget, s16* outYawTarget, f32 speedMode,
                                  PlayState* play) {
    if (!stick_control_set(play, this, outSpeedTarget, outYawTarget, speedMode)) {
        *outYawTarget = this->actor.shape.rot.y;

        if (this->focusActor != NULL) {
            if ((play->actorCtx.attention.reticleSpinCounter != 0) && !(this->stateFlags2 & PLAYER_STATE2_6)) {
                *outYawTarget = search_position_angleY(&this->actor.world.pos, &this->focusActor->focus.pos);
                return false;
            }
        } else if (parallel_mode_check(this)) {
            *outYawTarget = this->parallelYaw;
        }

        return false;
    } else {
        *outYawTarget += getCameraAngleY(GET_ACTIVE_CAM(play));
        return true;
    }
}

typedef enum ActionHandlerIndex {
    /*  0 */ PLAYER_ACTION_HANDLER_0,
    /*  1 */ PLAYER_ACTION_HANDLER_1,
    /*  2 */ PLAYER_ACTION_HANDLER_2,
    /*  3 */ PLAYER_ACTION_HANDLER_3,
    /*  4 */ PLAYER_ACTION_HANDLER_TALK,
    /*  5 */ PLAYER_ACTION_HANDLER_5,
    /*  6 */ PLAYER_ACTION_HANDLER_ROLL,
    /*  7 */ PLAYER_ACTION_HANDLER_7,
    /*  8 */ PLAYER_ACTION_HANDLER_8,
    /*  9 */ PLAYER_ACTION_HANDLER_9,
    /* 10 */ PLAYER_ACTION_HANDLER_10,
    /* 11 */ PLAYER_ACTION_HANDLER_11,
    /* 12 */ PLAYER_ACTION_HANDLER_12,
    /* 13 */ PLAYER_ACTION_HANDLER_13
} ActionHandlerIndex;

static s8 anchor_wait_action_check[] = {
    PLAYER_ACTION_HANDLER_13, PLAYER_ACTION_HANDLER_2,  PLAYER_ACTION_HANDLER_TALK, PLAYER_ACTION_HANDLER_9,
    PLAYER_ACTION_HANDLER_10, PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_8,    -PLAYER_ACTION_HANDLER_7,
};

static s8 parallel_wait_action_check[] = {
    PLAYER_ACTION_HANDLER_13, PLAYER_ACTION_HANDLER_1,    PLAYER_ACTION_HANDLER_2, PLAYER_ACTION_HANDLER_5,
    PLAYER_ACTION_HANDLER_3,  PLAYER_ACTION_HANDLER_TALK, PLAYER_ACTION_HANDLER_9, PLAYER_ACTION_HANDLER_10,
    PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_7,    PLAYER_ACTION_HANDLER_8, -PLAYER_ACTION_HANDLER_ROLL,
};

static s8 parallel_side_walk_action_check[] = {
    PLAYER_ACTION_HANDLER_13,   PLAYER_ACTION_HANDLER_1, PLAYER_ACTION_HANDLER_2,     PLAYER_ACTION_HANDLER_3,
    PLAYER_ACTION_HANDLER_TALK, PLAYER_ACTION_HANDLER_9, PLAYER_ACTION_HANDLER_10,    PLAYER_ACTION_HANDLER_11,
    PLAYER_ACTION_HANDLER_8,    PLAYER_ACTION_HANDLER_7, -PLAYER_ACTION_HANDLER_ROLL,
};

static s8 parallel_back_run_action_check[] = {
    PLAYER_ACTION_HANDLER_13, PLAYER_ACTION_HANDLER_2,  PLAYER_ACTION_HANDLER_TALK, PLAYER_ACTION_HANDLER_9,
    PLAYER_ACTION_HANDLER_10, PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_8,    -PLAYER_ACTION_HANDLER_7,
};

static s8 anchor_side_walk_action_check[] = {
    PLAYER_ACTION_HANDLER_13, PLAYER_ACTION_HANDLER_2,  PLAYER_ACTION_HANDLER_TALK,
    PLAYER_ACTION_HANDLER_9,  PLAYER_ACTION_HANDLER_10, PLAYER_ACTION_HANDLER_11,
    PLAYER_ACTION_HANDLER_12, PLAYER_ACTION_HANDLER_8,  -PLAYER_ACTION_HANDLER_7,
};

static s8 turn_action_check[] = {
    -PLAYER_ACTION_HANDLER_7,
};

static s8 wait_action_check[] = {
    PLAYER_ACTION_HANDLER_0, PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_1,     PLAYER_ACTION_HANDLER_2,
    PLAYER_ACTION_HANDLER_3, PLAYER_ACTION_HANDLER_5,  PLAYER_ACTION_HANDLER_TALK,  PLAYER_ACTION_HANDLER_9,
    PLAYER_ACTION_HANDLER_8, PLAYER_ACTION_HANDLER_7,  -PLAYER_ACTION_HANDLER_ROLL,
};

static s8 run_action_check[] = {
    PLAYER_ACTION_HANDLER_0, PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_1, PLAYER_ACTION_HANDLER_2,
    PLAYER_ACTION_HANDLER_3, PLAYER_ACTION_HANDLER_12, PLAYER_ACTION_HANDLER_5, PLAYER_ACTION_HANDLER_TALK,
    PLAYER_ACTION_HANDLER_9, PLAYER_ACTION_HANDLER_8,  PLAYER_ACTION_HANDLER_7, -PLAYER_ACTION_HANDLER_ROLL,
};

static s8 anchor_run_action_check[] = {
    PLAYER_ACTION_HANDLER_13,    PLAYER_ACTION_HANDLER_1,  PLAYER_ACTION_HANDLER_2,    PLAYER_ACTION_HANDLER_3,
    PLAYER_ACTION_HANDLER_12,    PLAYER_ACTION_HANDLER_5,  PLAYER_ACTION_HANDLER_TALK, PLAYER_ACTION_HANDLER_9,
    PLAYER_ACTION_HANDLER_10,    PLAYER_ACTION_HANDLER_11, PLAYER_ACTION_HANDLER_8,    PLAYER_ACTION_HANDLER_7,
    -PLAYER_ACTION_HANDLER_ROLL,
};

static s8 anchor_back_brake_action_check[] = {
    PLAYER_ACTION_HANDLER_10,
    PLAYER_ACTION_HANDLER_8,
    -PLAYER_ACTION_HANDLER_7,
};

static s8 swim_action_check[] = {
    PLAYER_ACTION_HANDLER_0,
    PLAYER_ACTION_HANDLER_12,
    PLAYER_ACTION_HANDLER_5,
    -PLAYER_ACTION_HANDLER_TALK,
};

s32 to_look_check(Player* this, PlayState* play);
s32 to_door_open_check(Player* this, PlayState* play);
s32 to_carry_check(Player* this, PlayState* play);
s32 to_ride_check(Player* this, PlayState* play);
s32 to_talk_check(Player* this, PlayState* play);
s32 to_push_check(Player* this, PlayState* play);
s32 to_check_check(Player* this, PlayState* play);
s32 to_kiru_check(Player* this, PlayState* play);
s32 to_power_kiru_charge_check(Player* this, PlayState* play);
s32 to_throw_check(Player* this, PlayState* play);
s32 to_anchor_jump_check(Player* this, PlayState* play);
s32 to_defense_check(Player* this, PlayState* play);
s32 to_wall_action_check(Player* this, PlayState* play);
s32 to_look_set_check(Player* this, PlayState* play);

static s32 (*action_check_proc[])(Player* this, PlayState* play) = {
    to_look_check,    // PLAYER_ACTION_HANDLER_0
    to_door_open_check,    // PLAYER_ACTION_HANDLER_1
    to_carry_check,    // PLAYER_ACTION_HANDLER_2
    to_ride_check,    // PLAYER_ACTION_HANDLER_3
    to_talk_check, // PLAYER_ACTION_HANDLER_TALK
    to_push_check,    // PLAYER_ACTION_HANDLER_5
    to_check_check, // PLAYER_ACTION_HANDLER_ROLL
    to_kiru_check,    // PLAYER_ACTION_HANDLER_7
    to_power_kiru_charge_check,    // PLAYER_ACTION_HANDLER_8
    to_throw_check,    // PLAYER_ACTION_HANDLER_9
    to_anchor_jump_check,   // PLAYER_ACTION_HANDLER_10
    to_defense_check,   // PLAYER_ACTION_HANDLER_11
    to_wall_action_check,   // PLAYER_ACTION_HANDLER_12
    to_look_set_check,   // PLAYER_ACTION_HANDLER_13
};

/**
 * This function processes "Action Handler Lists".
 *
 * An Action Handler is a function that "listens" for certain conditions or the right time
 * to change to a certain action. These can include actions triggered manually by the player
 * or actions that happen automatically, given some other condition(s).
 *
 * Action Handler Lists are a list of indices for the `action_check_proc` array.
 * The Action Handlers are ran in order until one of them returns true, or the end of the list is reached.
 * An Action Handler index having a negative value indicates that it is the last member in the list.
 *
 * Because these lists are processed sequentially, the order of the indices in the list
 * determines an Action Handler's priority.
 *
 * If the `updateUpperBody` argument is true, Player's upper body will update before the Action Handler List
 * is processed. This allows for Item Action functions to run, for example.
 *
 * @return true if a new action has been chosen
 *
 */
s32 action_select_check(PlayState* play, Player* this, s8* actionHandlerList, s32 updateUpperBody) {
    s32 i;

    if (!(this->stateFlags1 & (PLAYER_STATE1_0 | PLAYER_STATE1_DEAD | PLAYER_STATE1_29))) {
        if (updateUpperBody) {
            uperbody_action_flag = uperbody_action_check(this, play);

            if (move_light_bom == this->actionFunc) {
                return true;
            }
        }

        if (hook_shoot_check(this)) {
            this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_UPPER_X;
            return true;
        }

        if (!(this->stateFlags1 & PLAYER_STATE1_START_CHANGING_HELD_ITEM) &&
            (uperbody_nom2figh_play != this->upperActionFunc)) {
            // Process all entries in the Action Handler List with a positive index
            while (*actionHandlerList >= 0) {
                if (action_check_proc[*actionHandlerList](this, play)) {
                    return true;
                }
                actionHandlerList++;
            }

            // Try the last entry in the list. Negate the index to make it positive again.
            if (action_check_proc[-(*actionHandlerList)](this, play)) {
                return true;
            }
        }
    }

    return false;
}

typedef enum PlayerActionInterruptResult {
    /* -1 */ PLAYER_INTERRUPT_NONE = -1,
    /*  0 */ PLAYER_INTERRUPT_NEW_ACTION,
    /*  1 */ PLAYER_INTERRUPT_MOVE
} PlayerActionInterruptResult;

/**
 * An Action Interrupt allows for ending an action early, toward the end of an animation.
 *
 * First, `wait_action_check` will be checked to see if any of those actions should be used.
 * It should be noted that the `updateUpperBody` argument passed to `action_select_check`
 * is `true`. This means that an item can be used during the interrupt window.
 *
 * If no actions from the Action Handler List are used, then the control stick is checked to see if
 * any movement should occur.
 *
 * Note that while this function can set up a new action with `wait_action_check`, this function
 * will not set up an appropriate action for moving.
 * It is the callers responsibility to react accordingly to `PLAYER_INTERRUPT_MOVE`.
 *
 * @param frameRange  The number of frames, from the end of the current animation, where an interrupt can occur.
 * @return The interrupt result. See `PlayerActionInterruptResult`.
 */
s32 to_next_action_cancel_check(PlayState* play, Player* this, SkelAnime* skelAnime, f32 frameRange) {
    f32 speedTarget;
    s16 yawTarget;

    if ((skelAnime->endFrame - frameRange) <= skelAnime->curFrame) {
        if (action_select_check(play, this, wait_action_check, true)) {
            return PLAYER_INTERRUPT_NEW_ACTION;
        }

        if (stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play)) {
            return PLAYER_INTERRUPT_MOVE;
        }
    }

    return PLAYER_INTERRUPT_NONE;
}

void power_kiru_effect_set(PlayState* play, Player* this, s32 arg2) {
    if (arg2 != 0) {
        this->unk_858 = 0.0f;
    } else {
        this->unk_858 = 0.5f;
    }

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (this->actor.category == ACTORCAT_PLAYER) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_M_THUNDER, this->bodyPartsPos[PLAYER_BODYPART_WAIST].x,
                    this->bodyPartsPos[PLAYER_BODYPART_WAIST].y, this->bodyPartsPos[PLAYER_BODYPART_WAIST].z, 0, 0, 0,
                    sword_check(this) | arg2);
    }
}

s32 rolling_kiru_check(Player* this) {
    s8 sp3C[4];
    s8* iter;
    s8* iter2;
    s8 temp1;
    s8 temp2;
    s32 i;

    if ((this->heldItemAction == PLAYER_IA_DEKU_STICK) || break_longsword_check(this)) {
        return false;
    }

    iter = &this->controlStickSpinAngles[0];
    iter2 = &sp3C[0];

    for (i = 0; i < 4; i++, iter++, iter2++) {
        if ((*iter2 = *iter) < 0) {
            return false;
        }

        *iter2 *= 2;
    }

    temp1 = sp3C[0] - sp3C[1];

    if (ABS(temp1) < 10) {
        return false;
    }

    iter2 = &sp3C[1];

    for (i = 1; i < 3; i++, iter2++) {
        temp2 = *iter2 - *(iter2 + 1);

        if ((ABS(temp2) < 10) || (temp2 * temp1 < 0)) {
            return false;
        }
    }

    return true;
}

void to_power_kiru_start_init(PlayState* play, Player* this) {
    LinkAnimationHeader* anim;

    if ((this->meleeWeaponAnimation >= PLAYER_MWA_RIGHT_SLASH_1H) &&
        (this->meleeWeaponAnimation <= PLAYER_MWA_RIGHT_COMBO_2H)) {
        anim = link_power_kiru_startL_anm[longsword_check(this)];
    } else {
        anim = link_power_kiru_start_anm[longsword_check(this)];
    }

    sword_attack_reset(this);
    Skeleton_Info_Rom_init(play, &this->skelAnime, anim, 1.0f, 8.0f, Si2_anime_end_frame(anim), ANIMMODE_ONCE, -9.0f);
    power_kiru_effect_set(play, this, 0x200);
}

void to_power_kiru_start_set(PlayState* play, Player* this) {
    Player_actor_set_process(play, this, move_power_kiru_wait, 1);
    to_power_kiru_start_init(play, this);
}

static s8 basic_kiru_type[] = {
    PLAYER_MWA_STAB_1H,
    PLAYER_MWA_RIGHT_SLASH_1H,
    PLAYER_MWA_RIGHT_SLASH_1H,
    PLAYER_MWA_LEFT_SLASH_1H,
};

static s8 hammer_type[] = {
    PLAYER_MWA_HAMMER_FORWARD,
    PLAYER_MWA_HAMMER_SIDE,
    PLAYER_MWA_HAMMER_FORWARD,
    PLAYER_MWA_HAMMER_SIDE,
};

s32 kiru_type_check(Player* this) {
    s32 controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];
    s32 sp18;

    if (this->heldItemAction == PLAYER_IA_HAMMER) {
        if (controlStickDirection <= PLAYER_STICK_DIR_NONE) {
            controlStickDirection = PLAYER_STICK_DIR_FORWARD;
        }

        sp18 = hammer_type[controlStickDirection];
        this->unk_845 = 0;
    } else {
        if (rolling_kiru_check(this)) {
            sp18 = PLAYER_MWA_SPIN_ATTACK_1H;
        } else {
            if (controlStickDirection <= PLAYER_STICK_DIR_NONE) {
                if (anchor_parallel_mode_check(this)) {
                    sp18 = PLAYER_MWA_FORWARD_SLASH_1H;
                } else {
                    sp18 = PLAYER_MWA_RIGHT_SLASH_1H;
                }
            } else {
                sp18 = basic_kiru_type[controlStickDirection];

                if (sp18 == PLAYER_MWA_STAB_1H) {
                    this->stateFlags2 |= PLAYER_STATE2_30;

                    if (!anchor_parallel_mode_check(this)) {
                        sp18 = PLAYER_MWA_FORWARD_SLASH_1H;
                    }
                }
            }

            if (this->heldItemAction == PLAYER_IA_DEKU_STICK) {
                sp18 = PLAYER_MWA_FORWARD_SLASH_1H;
            }
        }

        if (longsword_check(this)) {
            sp18++;
        }
    }

    return sp18;
}

void sword_AT_type_set(Player* this, s32 quadIndex, u32 dmgFlags) {
    this->meleeWeaponQuads[quadIndex].elem.atDmgInfo.dmgFlags = dmgFlags;

    if (dmgFlags == DMG_DEKU_STICK) {
        this->meleeWeaponQuads[quadIndex].elem.atElemFlags = ATELEM_ON | ATELEM_NEAREST | ATELEM_SFX_WOOD;
    } else {
        this->meleeWeaponQuads[quadIndex].elem.atElemFlags = ATELEM_ON | ATELEM_NEAREST;
    }
}

static u32 ATtype_status[][2] = {
    { DMG_SLASH_MASTER, DMG_JUMP_MASTER }, { DMG_SLASH_KOKIRI, DMG_JUMP_KOKIRI }, { DMG_SLASH_GIANT, DMG_JUMP_GIANT },
    { DMG_DEKU_STICK, DMG_JUMP_MASTER },   { DMG_HAMMER_SWING, DMG_HAMMER_JUMP },
};

void to_kiru_set(PlayState* play, Player* this, s32 arg2) {
    s32 padding;
    u32 dmgFlags;
    s32 temp;

    Player_actor_set_process(play, this, move_kiru, 0);
    this->unk_844 = 8;
    if (!((arg2 >= PLAYER_MWA_FLIPSLASH_FINISH) && (arg2 <= PLAYER_MWA_JUMPSLASH_FINISH))) {
        sword_attack_reset(this);
    }

    if ((arg2 != this->meleeWeaponAnimation) || !(this->unk_845 < 3)) {
        this->unk_845 = 0;
    }

    this->unk_845++;
    if (this->unk_845 >= 3) {
        arg2 += 2;
    }

    this->meleeWeaponAnimation = arg2;

    anime_init_standard_stop_3f(play, this, kiru_anime_status[arg2].unk_00);
    if ((arg2 != PLAYER_MWA_FLIPSLASH_START) && (arg2 != PLAYER_MWA_JUMPSLASH_START)) {
        anime_move_init(play, this,
                                 PLAYER_ANIM_MOVEMENT_RESET_BY_AGE | ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_ENABLE_MOVEMENT);
    }

    this->yaw = this->actor.shape.rot.y;

    if (break_longsword_check(this)) {
        temp = 1;
    } else {
        temp = sword_check(this) - 1;
    }

    if ((arg2 >= PLAYER_MWA_FLIPSLASH_START) && (arg2 <= PLAYER_MWA_JUMPSLASH_FINISH)) {
        dmgFlags = ATtype_status[temp][1];
    } else {
        dmgFlags = ATtype_status[temp][0];
    }

    sword_AT_type_set(this, 0, dmgFlags);
    sword_AT_type_set(this, 1, dmgFlags);
}

/**
 * Gives the player intangibility frames. Used for when the player takes damage.
 *
 * If the player is already intangible, it will be overridden by the new intangibility duration.
 * If the player is already invunerable, no intangibility will be applied.
 *
 * @param timer must be a positive value representing the number of intangibility frames.
 * @note Intangibility prevents taking damage and responses to damage like knockback, while invulnerability only
 * prevents taking damage.
 */
void no_damage_timer_set(Player* this, s32 timer) {
    if (this->invincibilityTimer >= 0) {
        this->invincibilityTimer = timer;
        this->damageFlickerAnimCounter = 0;
    }
}

/**
 * Gives the player invulnerability frames. Used for when the player performs a dodging maneuver like a roll.
 *
 * If the player is already intangible, they will become invulnerable instead.
 * If the player is already invulnerable, the longer of the two invulnerability periods is kept.
 *
 * @param timer must be a negative value representing the number of invulnerability frames.
 * @note Intangibility prevents taking damage and responses to damage like knockback, while invulnerability only
 * prevents taking damage.
 */
void shield_no_damage_timer_check(Player* this, s32 timer) {
    if (this->invincibilityTimer > timer) {
        this->invincibilityTimer = timer;
    }
    this->damageFlickerAnimCounter = 0;
}

/**
 * @return false if player is out of health
 */
s32 damage_point_set(PlayState* play, Player* this, s32 damage) {
    if ((this->invincibilityTimer != 0) || (this->actor.category != ACTORCAT_PLAYER)) {
        return true;
    }

    return life_meter_play(play, damage);
}

void fall_wait_cancel_pos_set(Player* this) {
    this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
    anime_move_calc(this, ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y);
}

void to_landing_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_landing, 0);
    anime_init_standard_repeat(play, this, &gPlayerAnim_link_normal_landing_wait);
    this->av2.actionVar2 = 1;
    if (this->unk_6AD != 3) {
        this->unk_6AD = 0;
    }
}

static LinkAnimationHeader* hit_anm_data[] = {
    &gPlayerAnim_link_normal_front_shit, &gPlayerAnim_link_normal_front_shitR, &gPlayerAnim_link_normal_back_shit,
    &gPlayerAnim_link_normal_back_shitR, &gPlayerAnim_link_normal_front_hit,   &gPlayerAnim_link_anchor_front_hitR,
    &gPlayerAnim_link_normal_back_hit,   &gPlayerAnim_link_anchor_back_hitR,
};

void damage_set(PlayState* play, Player* this, s32 damageResponseType, f32 speed, f32 yVelocity, s16 yRot,
                   s32 invincibilityTimer) {
    LinkAnimationHeader* anim = NULL;
    LinkAnimationHeader** sp28;

    if (this->stateFlags1 & PLAYER_STATE1_13) {
        fall_wait_cancel_pos_set(this);
    }

    this->unk_890 = 0;

    player_SE_set(this, NA_SE_PL_DAMAGE);

    if (!damage_point_set(play, this, 0 - this->actor.colChkInfo.damage)) {
        this->stateFlags2 &= ~PLAYER_STATE2_7;
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && !(this->stateFlags1 & PLAYER_STATE1_27)) {
            to_landing_set(this, play);
        }
        return;
    }

    no_damage_timer_set(this, invincibilityTimer);

    if (damageResponseType == PLAYER_HIT_RESPONSE_ICE_TRAP) {
        Player_actor_set_process(play, this, move_ice_down, 0);

        anim = &gPlayerAnim_link_normal_ice_down;

        speedF_look_mode_clear(this);
        player_rumble_entry(this, 255, 10, 40, 0);

        player_SE_set(this, NA_SE_PL_FREEZE_S);
        player_voice_SE_set(this, NA_SE_VO_LI_FREEZE);
    } else if (damageResponseType == PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK) {
        Player_actor_set_process(play, this, move_electric_shock, 0);

        player_rumble_entry(this, 255, 80, 150, 0);

        anime_init_standard_repeat_3f(play, this, &gPlayerAnim_link_normal_electric_shock);
        speedF_look_mode_clear(this);

        this->av2.actionVar2 = 20;
    } else {
        yRot -= this->actor.shape.rot.y;
        if (this->stateFlags1 & PLAYER_STATE1_27) {
            Player_actor_set_process(play, this, move_swim_damage, 0);
            player_rumble_entry(this, 180, 20, 50, 0);

            this->speedXZ = 4.0f;
            this->actor.velocity.y = 0.0f;

            anim = &gPlayerAnim_link_swimer_swim_hit;

            player_voice_SE_set(this, NA_SE_VO_LI_DAMAGE_S);
        } else if ((damageResponseType == PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE) ||
                   (damageResponseType == PLAYER_HIT_RESPONSE_KNOCKBACK_SMALL) ||
                   !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
                   (this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_21))) {
            Player_actor_set_process(play, this, move_big_damage, 0);

            this->stateFlags3 |= PLAYER_STATE3_1;

            player_rumble_entry(this, 255, 20, 150, 0);
            speedF_look_mode_clear(this);

            if (damageResponseType == PLAYER_HIT_RESPONSE_KNOCKBACK_SMALL) {
                this->av2.actionVar2 = 4;

                this->actor.speed = 3.0f;
                this->speedXZ = 3.0f;
                this->actor.velocity.y = 6.0f;

                anime_init_0_stop_basic(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_damage_run, this->modelAnimType));
                player_voice_SE_set(this, NA_SE_VO_LI_DAMAGE_S);
            } else {
                this->actor.speed = speed;
                this->speedXZ = speed;
                this->actor.velocity.y = yVelocity;

                if (ABS(yRot) > 0x4000) {
                    anim = &gPlayerAnim_link_normal_front_downA;
                } else {
                    anim = &gPlayerAnim_link_normal_back_downA;
                }

                if ((this->actor.category != ACTORCAT_PLAYER) && (this->actor.colChkInfo.health == 0)) {
                    player_voice_SE_set(this, NA_SE_VO_BL_DOWN);
                } else {
                    player_voice_SE_set(this, NA_SE_VO_LI_FALL_L);
                }
            }

            this->hoverBootsTimer = 0;
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        } else {
            if ((this->speedXZ > 4.0f) && !anchor_mode_check(this)) {
                this->unk_890 = 20;
                player_rumble_entry(this, 120, 20, 10, 0);
                player_voice_SE_set(this, NA_SE_VO_LI_DAMAGE_S);
                return;
            }

            sp28 = hit_anm_data;

            Player_actor_set_process(play, this, move_damage, 0);
            anchor_LR_ratio_reset(this);

            if (this->actor.colChkInfo.damage < 5) {
                player_rumble_entry(this, 120, 20, 10, 0);
            } else {
                player_rumble_entry(this, 180, 20, 100, 0);
                this->speedXZ = 23.0f;
                sp28 += 4;
            }

            if (ABS(yRot) <= 0x4000) {
                sp28 += 2;
            }

            if (anchor_mode_check(this)) {
                sp28 += 1;
            }

            anim = *sp28;

            player_voice_SE_set(this, NA_SE_VO_LI_DAMAGE_S);
        }

        this->actor.shape.rot.y += yRot;
        this->yaw = this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (ABS(yRot) > 0x4000) {
            this->actor.shape.rot.y += 0x8000;
        }
    }

    action_reset_all(play, this);

    this->stateFlags1 |= PLAYER_STATE1_26;

    if (anim != NULL) {
        anime_init_standard_stop_3f(play, this, anim);
    }
}

s32 damage_ground_attribute_check(s32 arg0) {
    s32 temp = arg0 - FLOOR_TYPE_2;

    if ((temp >= 0) && (temp <= (FLOOR_TYPE_3 - FLOOR_TYPE_2))) {
        return temp;
    } else {
        return -1;
    }
}

int desert_ground_attribute_check(s32 arg0) {
    return (arg0 == FLOOR_TYPE_4) || (arg0 == FLOOR_TYPE_7) || (arg0 == FLOOR_TYPE_12);
}

void wood_shield_fire_check(Player* this, PlayState* play) {
    if (this->currentShield == PLAYER_SHIELD_DEKU) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_SHIELD, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, 1);
        ClearEquip_Item(play, EQUIP_TYPE_SHIELD);
        message_set(play, 0x305F, NULL);
    }
}

void fire_set_init(Player* this) {
    s32 i;

    // clang-format off
    for (i = 0; i < PLAYER_BODYPART_MAX; i++) { this->bodyFlameTimers[i] = get_random_timer(0, 200); }
    // clang-format on

    this->bodyIsBurning = true;
}

void fire_hit_check(Player* this) {
    if (this->actor.colChkInfo.acHitEffect == 1) {
        fire_set_init(this);
    }
    player_voice_SE_set(this, NA_SE_VO_LI_FALL_L);
}

void no_damage_timer_check(Player* this) {
    if ((this->invincibilityTimer >= 0) && (this->invincibilityTimer < 20)) {
        this->invincibilityTimer = 20;
    }
}

s32 damage_check(Player* this, PlayState* play) {
    s32 padding;
    s32 sp68 = false;
    s32 sp64;

    if (this->unk_A86 != 0) {
        if (!player_action_check(play, this)) {
            Ext_to_damage_set(play, -16);
            this->unk_A86 = 0;
        }
    } else {
        sp68 = ((player_get_tall(this) - 8.0f) < (this->unk_6C4 * this->actor.scale.y));

        if (sp68 || (this->actor.bgCheckFlags & BGCHECKFLAG_CRUSHED) || (ground_attribute == FLOOR_TYPE_9) ||
            (this->stateFlags2 & PLAYER_STATE2_31)) {
            player_voice_SE_set(this, NA_SE_VO_LI_DAMAGE_S);

            if (sp68) {
                Game_play_down_restart_top(play);
                Exit_Fade_Set(play);
            } else {
                // Special case for getting crushed in Forest Temple's Checkboard Ceiling Hall or Shadow Temple's
                // Falling Spike Trap Room, to respawn the player in a specific place
                if (((play->sceneId == SCENE_FOREST_TEMPLE) && (play->roomCtx.curRoom.num == 15)) ||
                    ((play->sceneId == SCENE_SHADOW_TEMPLE) && (play->roomCtx.curRoom.num == 10))) {
                    static SpecialRespawnInfo down_restart_sp_data = { { 1992.0f, 403.0f, -3432.0f }, 0 };
                    static SpecialRespawnInfo fallingSpikeTrapRespawn = { { 1200.0f, -1343.0f, 3850.0f }, 0 };
                    SpecialRespawnInfo* respawnInfo;

                    if (play->sceneId == SCENE_FOREST_TEMPLE) {
                        respawnInfo = &down_restart_sp_data;
                    } else {
                        respawnInfo = &fallingSpikeTrapRespawn;
                    }

                    Game_play_restart_set(play, RESPAWN_MODE_DOWN,
                                           PLAYER_PARAMS(PLAYER_START_MODE_IDLE, PLAYER_START_BG_CAM_DEFAULT));
                    z_common_data.respawn[RESPAWN_MODE_DOWN].pos = respawnInfo->pos;
                    z_common_data.respawn[RESPAWN_MODE_DOWN].yaw = respawnInfo->yaw;
                }

                Game_play_down_restart(play);
            }

            player_voice_SE_set(this, NA_SE_VO_LI_TAKEN_AWAY);
            play->haltAllActors = true;
            Na_StartSystemSe_F(NA_SE_OC_ABYSS);
        } else if ((this->knockbackType != PLAYER_KNOCKBACK_NONE) &&
                   ((this->knockbackType >= PLAYER_KNOCKBACK_LARGE) || (this->invincibilityTimer == 0))) {
            u8 knockbackResponse[] = {
                PLAYER_HIT_RESPONSE_KNOCKBACK_SMALL,
                PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE,
                PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE,
            };

            fire_hit_check(this);

            if (this->knockbackType == PLAYER_KNOCKBACK_LARGE_SHOCK) {
                this->bodyShockTimer = 40;
            }

            this->actor.colChkInfo.damage += this->knockbackDamage;
            damage_set(play, this, knockbackResponse[this->knockbackType - 1], this->knockbackSpeed,
                          this->knockbackYVelocity, this->knockbackRot, 20);
        } else {
            sp64 = (this->shieldQuad.base.acFlags & AC_BOUNCED) != 0;

            //! @bug The second set of conditions here seems intended as a way for Link to "block" hits by rolling.
            // However, `Collider.atFlags` is a byte so the flag check at the end is incorrect and cannot work.
            // Additionally, `Collider.atHit` can never be set while already colliding as AC, so it's also bugged.
            // This behavior was later fixed in MM, most likely by removing both the `atHit` and `atFlags` checks.
            if (sp64 || ((this->invincibilityTimer < 0) && (this->cylinder.base.acFlags & AC_HIT) &&
                         (this->cylinder.elem.atHit != NULL) && (this->cylinder.elem.atHit->atFlags & 0x20000000))) {

                player_rumble_entry(this, 180, 20, 100, 0);

                if (!child_hyral_shield_check(this)) {
                    if (this->invincibilityTimer >= 0) {
                        LinkAnimationHeader* anim;
                        s32 sp54 = move_defense == this->actionFunc;

                        if (!swim_check(this)) {
                            Player_actor_set_process(play, this, move_defense_hit, 0);
                        }

                        if (!(this->av1.actionVar1 = sp54)) {
                            Player_actor_set_uperbody_process(this, uperbody_defense_hit);

                            if (this->unk_870 < 0.5f) {
                                anim = link_anchor_defense_hitR_anm[longsword_check(this)];
                            } else {
                                anim = link_anchor_defense_hitL_anm[longsword_check(this)];
                            }
                            Skeleton_Info_Rom_init_standard_stop(play, &this->upperSkelAnime, anim);
                        } else {
                            anime_init_standard_stop(play, this, link_normal_defense_hit_anm[longsword_check(this)]);
                        }
                    }

                    if (!(this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_21))) {
                        this->speedXZ = -18.0f;
                        this->yaw = this->actor.shape.rot.y;
                    }
                }

                if (sp64 && (this->shieldQuad.elem.acHitElem->atDmgInfo.effect == 1)) {
                    wood_shield_fire_check(this, play);
                }

                return 0;
            }

            if ((this->unk_A87 != 0) || (this->invincibilityTimer > 0) || (this->stateFlags1 & PLAYER_STATE1_26) ||
                (this->csAction != PLAYER_CSACTION_NONE) || (this->meleeWeaponQuads[0].base.atFlags & AT_HIT) ||
                (this->meleeWeaponQuads[1].base.atFlags & AT_HIT)) {
                return 0;
            }

            if (this->cylinder.base.acFlags & AC_HIT) {
                Actor* ac = this->cylinder.base.ac;
                s32 sp4C;

                if (ac->flags & ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT) {
                    player_SE_set(this, NA_SE_PL_BODY_HIT);
                }

                if (this->stateFlags1 & PLAYER_STATE1_27) {
                    sp4C = PLAYER_HIT_RESPONSE_NONE;
                } else if (this->actor.colChkInfo.acHitEffect == 2) {
                    sp4C = PLAYER_HIT_RESPONSE_ICE_TRAP;
                } else if (this->actor.colChkInfo.acHitEffect == 3) {
                    sp4C = PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK;
                } else if (this->actor.colChkInfo.acHitEffect == 4) {
                    sp4C = PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE;
                } else {
                    fire_hit_check(this);
                    sp4C = PLAYER_HIT_RESPONSE_NONE;
                }

                damage_set(play, this, sp4C, 4.0f, 5.0f, Actor_search_actor_angleY(ac, &this->actor), 20);
            } else if (this->invincibilityTimer != 0) {
                return 0;
            } else {
                static u8 ground_damage_timer_data[] = { 120, 60 };
                s32 sp48 = damage_ground_attribute_check(ground_attribute);

                if (((this->actor.wallPoly != NULL) &&
                     T_BGCheck_CheckDamagePoly(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId)) ||
                    ((sp48 >= 0) && T_BGCheck_CheckDamagePoly(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) &&
                     (this->floorTypeTimer >= ground_damage_timer_data[sp48])) ||
                    ((sp48 >= 0) &&
                     ((this->currentTunic != PLAYER_TUNIC_GORON) || (this->floorTypeTimer >= ground_damage_timer_data[sp48])))) {
                    this->floorTypeTimer = 0;
                    this->actor.colChkInfo.damage = 4;
                    damage_set(play, this, PLAYER_HIT_RESPONSE_NONE, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
                } else {
                    return 0;
                }
            }
        }
    }

    return 1;
}

void to_jump_init(Player* this, LinkAnimationHeader* anim, f32 arg2, PlayState* play, u16 sfxId) {
    Player_actor_set_process(play, this, move_landing, 1);

    if (anim != NULL) {
        anime_init_standard_stop_3f(play, this, anim);
    }

    this->actor.velocity.y = arg2 * environment_speed_ratio;
    this->hoverBootsTimer = 0;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;

    player_jump_SE_set(this);
    player_voice_SE_set(this, sfxId);

    this->stateFlags1 |= PLAYER_STATE1_18;
}

void to_jump_set(Player* this, LinkAnimationHeader* anim, f32 arg2, PlayState* play) {
    to_jump_init(this, anim, arg2, play, NA_SE_VO_LI_SWORD_N);
}

s32 to_wall_action_check(Player* this, PlayState* play) {
    s32 sp3C;
    LinkAnimationHeader* anim;
    f32 sp34;
    f32 temp;

    if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->ledgeClimbType >= PLAYER_LEDGE_CLIMB_2) &&
        (!(this->stateFlags1 & PLAYER_STATE1_27) || (this->ageProperties->unk_14 > this->yDistToLedge))) {
        sp3C = 0;

        if (swim_check(this)) {
            if (this->actor.depthInWater < 50.0f) {
                if ((this->ledgeClimbType < PLAYER_LEDGE_CLIMB_2) ||
                    (this->yDistToLedge > this->ageProperties->unk_10)) {
                    return 0;
                }
            } else if ((this->currentBoots != PLAYER_BOOTS_IRON) || (this->ledgeClimbType > PLAYER_LEDGE_CLIMB_2)) {
                return 0;
            }
        } else if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
                   ((this->ageProperties->unk_14 <= this->yDistToLedge) && (this->stateFlags1 & PLAYER_STATE1_27))) {
            return 0;
        }

        if ((this->actor.wallBgId != BGCHECK_SCENE) && (wall_code & WALL_FLAG_6)) {
            if (this->ledgeClimbDelayTimer >= 6) {
                this->stateFlags2 |= PLAYER_STATE2_2;
                if (CHECK_BTN_ALL(pad->press.button, BTN_A)) {
                    sp3C = 1;
                }
            }
        } else if ((this->ledgeClimbDelayTimer >= 6) || CHECK_BTN_ALL(pad->press.button, BTN_A)) {
            sp3C = 1;
        }

        if (sp3C != 0) {
            Player_actor_set_process(play, this, move_stepup100, 0);

            this->stateFlags1 |= PLAYER_STATE1_18;

            sp34 = this->yDistToLedge;

            if (this->ageProperties->unk_14 <= sp34) {
                anim = &gPlayerAnim_link_normal_250jump_start;
                this->speedXZ = 1.0f;
            } else {
                f32 wallPolyNormalX = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.x);
                f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(this->actor.wallPoly->normal.z);
                f32 sp24 = this->distToInteractWall + 0.5f;

                this->stateFlags1 |= PLAYER_STATE1_14;

                if (swim_check(this)) {
                    anim = &gPlayerAnim_link_swimer_swim_15step_up;
                    sp34 -= (60.0f * this->ageProperties->unk_08);
                    this->stateFlags1 &= ~PLAYER_STATE1_27;
                } else if (this->ageProperties->unk_18 <= sp34) {
                    anim = &gPlayerAnim_link_normal_150step_up;
                    sp34 -= (59.0f * this->ageProperties->unk_08);
                } else {
                    anim = &gPlayerAnim_link_normal_100step_up;
                    sp34 -= (41.0f * this->ageProperties->unk_08);
                }

                this->actor.shape.yOffset -= sp34 * 100.0f;

                this->actor.world.pos.x -= sp24 * wallPolyNormalX;
                this->actor.world.pos.y += this->yDistToLedge;
                this->actor.world.pos.z -= sp24 * wallPolyNormalZ;

                speedF_look_mode_clear(this);
            }

            this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;

            Skeleton_Info_Rom_init_standard_speedset_stop(play, &this->skelAnime, anim, 1.3f);
            Skeleton_Proc_Anime_Change_Stop_On(play);

            this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;

            return 1;
        }
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->ledgeClimbType == PLAYER_LEDGE_CLIMB_1) &&
               (this->ledgeClimbDelayTimer >= 3)) {
        temp = (this->yDistToLedge * 0.08f) + 5.5f;
        to_jump_set(this, &gPlayerAnim_link_normal_jump, temp, play);
        this->speedXZ = 2.5f;

        return 1;
    }

    return 0;
}

void to_demo_run_init(PlayState* play, Player* this, f32 arg2, s16 arg3) {
    Player_actor_set_process(play, this, move_shutter_open, 0);
    action_reset(play, this);

    this->av1.actionVar1 = 1;
    this->av2.actionVar2 = 1;

    this->unk_450.x = (sin_s(arg3) * arg2) + this->actor.world.pos.x;
    this->unk_450.z = (cos_s(arg3) * arg2) + this->actor.world.pos.z;

    anime_init_standard_stop(play, this, wait_anime_select(this));
}

void to_swim_wait_set(PlayState* play, Player* this) {
    Player_actor_set_process(play, this, move_swim_wait, 0);
    anime_init_0_repeat_basic(play, this, &gPlayerAnim_link_swimer_swim_wait);
}

void to_warp_fall_set(PlayState* play, Player* this) {
    Player_actor_set_process(play, this, move_warp_fall, 0);

    this->stateFlags1 |= PLAYER_STATE1_29 | PLAYER_STATE1_31;

    changeCameraSet(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_SET_FREE0);
}

s32 to_warp_fall_check(PlayState* play, Player* this) {
    if ((play->transitionTrigger == TRANS_TRIGGER_OFF) && (this->stateFlags1 & PLAYER_STATE1_31)) {
        to_warp_fall_set(play, this);
        anime_init_standard_repeat(play, this, &gPlayerAnim_link_normal_landing_wait);
        player_voice_SE_set(this, NA_SE_VO_LI_FALL_S);
        Na_StartFixSe_F(NA_SE_OC_SECRET_WARP_IN);
        return 1;
    }

    return 0;
}

/**
 * The actual entrances each "return entrance" value can map to.
 * This is used by scenes that are shared between locations, like child/adult Shooting Gallery or Great Fairy Fountains.
 *
 * This 1D array is split into groups of entrances.
 * The start of each group is indexed by `return_scene_index` values.
 * The resulting groups are then indexed by the spawn value.
 *
 * The spawn value (`PlayState.spawn`) is set to a different value depending on the entrance used to enter the
 * scene, which allows these dynamic "return entrances" to link back to the previous scene.
 *
 * Note: grottos and normal fairy fountains use `ENTR_RETURN_GROTTO`
 */
static s16 return_scene_no[] = {
    // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_MAGIC
    /*  0 */ ENTR_DEATH_MOUNTAIN_TRAIL_4,  // from Magic Fairy Fountain
    /*  1 */ ENTR_DEATH_MOUNTAIN_CRATER_3, // from Double Magic Fairy Fountain
    /*  2 */ ENTR_HYRULE_CASTLE_2,         // from Double Defense Fairy Fountain (as adult)

    // ENTR_RETURN_2
    /*  3 */ ENTR_KAKARIKO_VILLAGE_9, // from Potion Shop in Kakariko
    /*  4 */ ENTR_MARKET_DAY_5,       // from Potion Shop in Market

    // ENTR_RETURN_BAZAAR
    /*  5 */ ENTR_KAKARIKO_VILLAGE_3,
    /*  6 */ ENTR_MARKET_DAY_6,

    // ENTR_RETURN_4
    /*  7 */ ENTR_KAKARIKO_VILLAGE_11, // from House of Skulltulas
    /*  8 */ ENTR_BACK_ALLEY_DAY_2,    // from Bombchu Shop

    // ENTR_RETURN_SHOOTING_GALLERY
    /*  9 */ ENTR_KAKARIKO_VILLAGE_10,
    /* 10 */ ENTR_MARKET_DAY_8,

    // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_SPELLS
    /* 11 */ ENTR_ZORAS_FOUNTAIN_5,  // from Farores Wind Fairy Fountain
    /* 12 */ ENTR_HYRULE_CASTLE_2,   // from Dins Fire Fairy Fountain (as child)
    /* 13 */ ENTR_DESERT_COLOSSUS_7, // from Nayrus Love Fairy Fountain
};

/**
 * The values are indices into `return_scene_no` marking the start of each group
 */
static u8 return_scene_index[] = {
    11, // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_SPELLS
    9,  // ENTR_RETURN_SHOOTING_GALLERY
    3,  // ENTR_RETURN_2
    5,  // ENTR_RETURN_BAZAAR
    7,  // ENTR_RETURN_4
    0,  // ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_MAGIC
};

s32 scene_change_check(PlayState* play, Player* this, CollisionPoly* poly, u32 bgId) {
    s32 exitIndex;
    s32 temp;

    if (this->actor.category == ACTORCAT_PLAYER) {
        exitIndex = 0;

        if (!(this->stateFlags1 & PLAYER_STATE1_DEAD) && (play->transitionTrigger == TRANS_TRIGGER_OFF) &&
            (this->csAction == PLAYER_CSACTION_NONE) && !(this->stateFlags1 & PLAYER_STATE1_0) &&
            (((poly != NULL) && (exitIndex = T_BGCheck_getSceneChangeCode_ai(&play->colCtx, poly, bgId), exitIndex != 0)) ||
             (desert_ground_attribute_check(ground_attribute) && (this->floorProperty == FLOOR_PROPERTY_12)))) {
            s32 sp34 = this->unk_A84 - (s32)this->actor.world.pos.y;

            if (!(this->stateFlags1 & (PLAYER_STATE1_23 | PLAYER_STATE1_27 | PLAYER_STATE1_29)) &&
                !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (sp34 < 100) && (to_ground_y > 100.0f)) {
                return 0;
            }

            if (exitIndex == 0) {
                Game_play_down_restart(play);
                Exit_Fade_Set(play);
            } else {
                play->nextEntranceIndex = play->exitList[exitIndex - 1];

                if (play->nextEntranceIndex == ENTR_RETURN_GROTTO) {
                    z_common_data.respawnFlag = 2;
                    play->nextEntranceIndex = z_common_data.respawn[RESPAWN_MODE_RETURN].entranceIndex;
                    play->transitionType = TRANS_TYPE_FADE_WHITE;
                    z_common_data.nextTransitionType = TRANS_TYPE_FADE_WHITE;
                } else if (play->nextEntranceIndex >= ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_SPELLS) {
                    play->nextEntranceIndex =
                        return_scene_no[return_scene_index[play->nextEntranceIndex -
                                                                             ENTR_RETURN_GREAT_FAIRYS_FOUNTAIN_SPELLS] +
                                                 play->spawn];
                    Exit_Fade_Set(play);
                } else {
                    if (T_BGCheck_getFriction_ai(&play->colCtx, poly, bgId) == FLOOR_EFFECT_2) {
                        z_common_data.respawn[RESPAWN_MODE_DOWN].entranceIndex = play->nextEntranceIndex;
                        Game_play_down_restart(play);
                        z_common_data.respawnFlag = -2;
                    }

                    z_common_data.retainWeatherMode = true;
                    Exit_Fade_Set(play);
                }

                play->transitionTrigger = TRANS_TRIGGER_START;
            }

            if (!(this->stateFlags1 & (PLAYER_STATE1_23 | PLAYER_STATE1_29)) &&
                !(this->stateFlags2 & PLAYER_STATE2_CRAWLING) && !swim_check(this) &&
                (temp = T_BGCheck_getAttributeCode_ai(&play->colCtx, poly, bgId), (temp != FLOOR_TYPE_10)) &&
                ((sp34 < 100) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {

                if (temp == FLOOR_TYPE_11) {
                    Na_StartFixSe_F(NA_SE_OC_SECRET_HOLE_OUT);
                    Na_StopAllSound(5);
                    z_common_data.seqId = (u8)NA_BGM_DISABLED;
                    z_common_data.natureAmbienceId = NATURE_ID_DISABLED;
                } else {
                    f32 speedXZ = this->speedXZ;
                    s32 yaw;

                    if (speedXZ < 0.0f) {
                        this->actor.world.rot.y += 0x8000;
                        speedXZ = -speedXZ;
                    }

                    if (speedXZ > R_RUN_SPEED_LIMIT / 100.0f) {
                        z_common_data.entranceSpeed = R_RUN_SPEED_LIMIT / 100.0f;
                    } else {
                        z_common_data.entranceSpeed = speedXZ;
                    }

                    if (slide_power_flag != CONVEYOR_SPEED_DISABLED) {
                        yaw = slide_power_angle;
                    } else {
                        yaw = this->actor.world.rot.y;
                    }
                    to_demo_run_init(play, this, 400.0f, yaw);
                }
            } else {
                if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                    speedF_clear(this);
                }
            }

            this->stateFlags1 |= PLAYER_STATE1_0 | PLAYER_STATE1_29;

            change_camera_check_set(play, CAM_SET_SCENE_TRANSITION);

            return 1;
        } else {
            if (play->transitionTrigger == TRANS_TRIGGER_OFF) {

                if ((this->actor.world.pos.y < -4000.0f) ||
                    (((this->floorProperty == FLOOR_PROPERTY_5) || (this->floorProperty == FLOOR_PROPERTY_12)) &&
                     ((to_ground_y < 100.0f) || (this->fallDistance > 400.0f) ||
                      ((play->sceneId != SCENE_SHADOW_TEMPLE) && (this->fallDistance > 200.0f)))) ||
                    ((play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) && (this->fallDistance > 320.0f))) {

                    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                        if (this->floorProperty == FLOOR_PROPERTY_5) {
                            Game_play_down_restart_top(play);
                        } else {
                            Game_play_down_restart(play);
                        }
                        play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
                        Na_StartSystemSe_F(NA_SE_OC_ABYSS);
                    } else {
                        to_warp_fall_set(play, this);
                        this->av2.actionVar2 = 9999;
                        if (this->floorProperty == FLOOR_PROPERTY_5) {
                            this->av1.actionVar1 = -1;
                        } else {
                            this->av1.actionVar1 = 1;
                        }
                    }
                }

                this->unk_A84 = this->actor.world.pos.y;
            }
        }
    }

    return 0;
}

/**
 * Gets a position relative to player's yaw.
 * An offset is applied to the provided base position in the direction of shape y rotation.
 * The resulting position is stored in `dest`
 */
void offset_pos_calc(Player* this, Vec3f* base, Vec3f* offset, Vec3f* dest) {
    f32 cos = cos_s(this->actor.shape.rot.y);
    f32 sin = sin_s(this->actor.shape.rot.y);

    dest->x = base->x + ((offset->x * cos) + (offset->z * sin));
    dest->y = base->y + offset->y;
    dest->z = base->z + ((offset->z * cos) - (offset->x * sin));
}

Actor* make_elf(PlayState* play, Player* this, Vec3f* arg2, Vec3f* arg3, s32 type) {
    Vec3f pos;

    offset_pos_calc(this, arg2, arg3, &pos);

    return Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ELF, pos.x, pos.y, pos.z, 0, 0, 0, type);
}

f32 player_ground_BGcheck2(PlayState* play, Player* this, Vec3f* arg2, Vec3f* arg3, CollisionPoly** arg4, s32* arg5) {
    offset_pos_calc(this, &this->actor.world.pos, arg2, arg3);

    return T_BGCheck_ObjGroundCheck_ai(&play->colCtx, arg4, arg5, arg3);
}

f32 player_ground_BGcheck(PlayState* play, Player* this, Vec3f* arg2, Vec3f* arg3) {
    CollisionPoly* sp24;
    s32 sp20;

    return player_ground_BGcheck2(play, this, arg2, arg3, &sp24, &sp20);
}

/**
 * Checks if a line between the player's position and the provided `offset` intersect a wall.
 *
 * Point A of the line is at player's world position offset by the height provided in `offset`.
 * Point B of the line is at player's world position offset by the entire `offset` vector.
 * Point A and B are always at the same height, meaning this is a horizontal line test.
 */
s32 player_wall_BGcheck(PlayState* play, Player* this, Vec3f* offset, CollisionPoly** wallPoly, s32* bgId,
                             Vec3f* posResult) {
    Vec3f posA;
    Vec3f posB;

    posA.x = this->actor.world.pos.x;
    posA.y = this->actor.world.pos.y + offset->y;
    posA.z = this->actor.world.pos.z;

    offset_pos_calc(this, &this->actor.world.pos, offset, &posB);

    return T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &posA, &posB, posResult, wallPoly, true, false, false, true, bgId);
}

s32 to_door_open_check(Player* this, PlayState* play) {
    Actor* attachedActor;
    s32 pad3;
    s32 doorDirection;
    f32 sp78;
    f32 sp74;
    Actor* doorActor;
    f32 sp6C;
    s32 frontRoom;

    if ((this->doorType != PLAYER_DOORTYPE_NONE) &&
        (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) ||
         ((this->heldActor != NULL) && (this->heldActor->id == ACTOR_EN_RU1)))) {
        if (CHECK_BTN_ALL(pad->press.button, BTN_A) || (move_door_start == this->actionFunc)) {
            doorActor = this->doorActor;

            if (this->doorType <= PLAYER_DOORTYPE_AJAR) {
                doorActor->textId = 0xD0;
                Ext_player_to_talk_set(play, doorActor);
                return 0;
            }

            doorDirection = this->doorDirection;
            sp78 = cos_s(doorActor->shape.rot.y);
            sp74 = sin_s(doorActor->shape.rot.y);

            if (this->doorType == PLAYER_DOORTYPE_SLIDING) {
                SlidingDoorActorBase* slidingDoor = (SlidingDoorActorBase*)doorActor;

                this->yaw = slidingDoor->dyna.actor.home.rot.y;
                if (doorDirection > 0) {
                    this->yaw -= 0x8000;
                }
                this->actor.shape.rot.y = this->yaw;

                if (this->speedXZ <= 0.0f) {
                    this->speedXZ = 0.1f;
                }

                to_demo_run_init(play, this, 50.0f, this->actor.shape.rot.y);

                this->av1.actionVar1 = 0;
                this->unk_447 = this->doorType;
                this->stateFlags1 |= PLAYER_STATE1_29;

                this->unk_450.x = this->actor.world.pos.x + ((doorDirection * 20.0f) * sp74);
                this->unk_450.z = this->actor.world.pos.z + ((doorDirection * 20.0f) * sp78);
                this->unk_45C.x = this->actor.world.pos.x + ((doorDirection * -120.0f) * sp74);
                this->unk_45C.z = this->actor.world.pos.z + ((doorDirection * -120.0f) * sp78);

                slidingDoor->isActive = true;
                speedF_look_mode_clear(this);

                if (this->doorTimer != 0) {
                    this->av2.actionVar2 = 0;
                    anime_init_stop_basic(play, this, wait_anime_select(this));
                    this->skelAnime.endFrame = 0.0f;
                } else {
                    this->speedXZ = 0.1f;
                }

                if (slidingDoor->dyna.actor.category == ACTORCAT_DOOR) {
                    this->cv.slidingDoorBgCamIndex =
                        play->transitionActors.list[GET_TRANSITION_ACTOR_INDEX(&slidingDoor->dyna.actor)]
                            .sides[(doorDirection > 0) ? 0 : 1]
                            .bgCamIndex;

                    magic_grass_cancel(play);
                }
            } else {
                // The door actor can be either EnDoor or DoorKiller.
                DoorActorBase* door = (DoorActorBase*)doorActor;
                LinkAnimationHeader* sp5C;

                door->openAnim = (doorDirection < 0.0f)
                                     ? (LINK_IS_ADULT ? DOOR_OPEN_ANIM_ADULT_L : DOOR_OPEN_ANIM_CHILD_L)
                                     : (LINK_IS_ADULT ? DOOR_OPEN_ANIM_ADULT_R : DOOR_OPEN_ANIM_CHILD_R);

                if (door->openAnim == DOOR_OPEN_ANIM_ADULT_L) {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorA_free, this->modelAnimType);
                } else if (door->openAnim == DOOR_OPEN_ANIM_CHILD_L) {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorA, this->modelAnimType);
                } else if (door->openAnim == DOOR_OPEN_ANIM_ADULT_R) {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorB_free, this->modelAnimType);
                } else {
                    sp5C = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_doorB, this->modelAnimType);
                }

                Player_actor_set_process(play, this, move_door_open, 0);
                to_no_item_set(play, this);

                if (doorDirection < 0) {
                    this->actor.shape.rot.y = doorActor->shape.rot.y;
                } else {
                    this->actor.shape.rot.y = doorActor->shape.rot.y - 0x8000;
                }

                this->yaw = this->actor.shape.rot.y;

                sp6C = (doorDirection * 22.0f);
                this->actor.world.pos.x = doorActor->world.pos.x + sp6C * sp74;
                this->actor.world.pos.z = doorActor->world.pos.z + sp6C * sp78;

                swim_check_anime_init_standard_stop(play, this, sp5C);

                if (this->doorTimer != 0) {
                    this->skelAnime.endFrame = 0.0f;
                }

                speedF_look_mode_clear(this);
                anime_move_init(play, this,
                                         PLAYER_ANIM_MOVEMENT_RESET_BY_AGE | ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y |
                                             ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                             ANIM_FLAG_OVERRIDE_MOVEMENT);

                // If this door is the second half of a double door (spawned as child)
                if (doorActor->parent != NULL) {
                    doorDirection = -doorDirection;
                }

                door->playerIsOpening = true;

                // If the door actor is not DoorKiller
                if (this->doorType != PLAYER_DOORTYPE_FAKE) {
                    // The door actor is EnDoor

                    this->stateFlags1 |= PLAYER_STATE1_29;
                    magic_grass_cancel(play);

                    if (ENDOOR_GET_TYPE(doorActor) == DOOR_SCENEEXIT) {
                        CollisionPoly* groundPoly;
                        Vec3f checkPos;

                        checkPos.x = doorActor->world.pos.x - (sp6C * sp74);
                        checkPos.y = doorActor->world.pos.y + 10.0f;
                        checkPos.z = doorActor->world.pos.z - (sp6C * sp78);

                        T_BGCheck_ObjGroundCheck(&play->colCtx, &groundPoly, &checkPos);

                        //! @bug groundPoly's bgId is not guaranteed to be BGCHECK_SCENE
                        if (scene_change_check(play, this, groundPoly, BGCHECK_SCENE)) {
                            z_common_data.entranceSpeed = 2.0f;
                            z_common_data.entranceSound = NA_SE_OC_DOOR_OPEN;
                        }
                    } else {
                        setDoorCameraInfo(Gama_play_get_camera(play, CAM_ID_MAIN), doorActor,
                                             play->transitionActors.list[GET_TRANSITION_ACTOR_INDEX(doorActor)]
                                                 .sides[(doorDirection > 0) ? 0 : 1]
                                                 .bgCamIndex,
                                             0, 38.0f * environment_p_speed_ratio, 26.0f * environment_p_speed_ratio,
                                             10.0f * environment_p_speed_ratio);
                    }
                }
            }

            if ((this->doorType != PLAYER_DOORTYPE_FAKE) && (doorActor->category == ACTORCAT_DOOR)) {
                frontRoom = play->transitionActors.list[GET_TRANSITION_ACTOR_INDEX(doorActor)]
                                .sides[(doorDirection > 0) ? 0 : 1]
                                .room;

                if ((frontRoom >= 0) && (frontRoom != play->roomCtx.curRoom.num)) {
                    Room_Info_exchange_start(play, &play->roomCtx, frontRoom);
                }
            }

            doorActor->room = play->roomCtx.curRoom.num;

            if (((attachedActor = doorActor->child) != NULL) || ((attachedActor = doorActor->parent) != NULL)) {
                attachedActor->room = play->roomCtx.curRoom.num;
            }

            return 1;
        }
    }

    return 0;
}

void to_anchor_wait_set(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;

    Player_actor_set_process(play, this, move_anchor_wait, 1);

    if (this->unk_870 < 0.5f) {
        anim = waitR_anime_select(this);
        this->unk_870 = 0.0f;
    } else {
        anim = waitL_anime_select(this);
        this->unk_870 = 1.0f;
    }

    this->unk_874 = this->unk_870;
    anime_init_standard_repeat(play, this, anim);
    this->yaw = this->actor.shape.rot.y;
}

void to_parallel_wait_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_parallel_wait, 1);
    anime_init_stop_basic(play, this, wait_anime_select(this));
    this->yaw = this->actor.shape.rot.y;
}

void to_wait_set_check(Player* this, PlayState* play) {
    if (anchor_mode_check(this)) {
        to_anchor_wait_set(this, play);
    } else if (parallel_mode_check(this)) {
        to_parallel_wait_set(this, play);
    } else {
        Ext_to_wait_set(this, play);
    }
}

void to_wait_proc_set(Player* this, PlayState* play) {
    PlayerActionFunc actionFunc;

    if (anchor_mode_check(this)) {
        actionFunc = move_anchor_wait;
    } else if (parallel_mode_check(this)) {
        actionFunc = move_parallel_wait;
    } else {
        actionFunc = move_wait;
    }

    Player_actor_set_process(play, this, actionFunc, 1);
}

void to_wait_proc_set_init(Player* this, PlayState* play) {
    to_wait_proc_set(this, play);

    if (anchor_mode_check(this)) {
        this->av2.actionVar2 = 1;
    }
}

void to_wait_set_check_anim(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    to_wait_proc_set_init(this, play);
    swim_check_anime_init_standard_stop(play, this, anim);
}

int carry_ok_check(Player* this) {
    return (this->interactRangeActor != NULL) && (this->heldActor == NULL);
}

void demo_init_carry(PlayState* play, Player* this) {
    if (carry_ok_check(this)) {
        Actor* interactRangeActor = this->interactRangeActor;
        s32 interactActorId = interactRangeActor->id;

        if (interactActorId == ACTOR_BG_TOKI_SWD) {
            this->interactRangeActor->parent = &this->actor;
            Player_actor_set_process(play, this, move_tool_demo_wait, 0);
            this->stateFlags1 |= PLAYER_STATE1_29;
        } else {
            LinkAnimationHeader* anim;

            if (interactActorId == ACTOR_BG_HEAVY_BLOCK) {
                Player_actor_set_process(play, this, move_heavy_carry, 0);
                this->stateFlags1 |= PLAYER_STATE1_29;
                anim = &gPlayerAnim_link_normal_heavy_carry;
            } else if ((interactActorId == ACTOR_EN_ISHI) && (PARAMS_GET_U(interactRangeActor->params, 0, 4) == 1)) {
                Player_actor_set_process(play, this, move_silver_carry, 0);
                anim = &gPlayerAnim_link_silver_carry;
            } else if (((interactActorId == ACTOR_EN_BOMBF) || (interactActorId == ACTOR_EN_KUSA)) &&
                       (player_grove_check() <= PLAYER_STR_NONE)) {
                Player_actor_set_process(play, this, move_no_carry, 0);
                this->actor.world.pos.x =
                    (sin_s(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.x;
                this->actor.world.pos.z =
                    (cos_s(interactRangeActor->yawTowardsPlayer) * 20.0f) + interactRangeActor->world.pos.z;
                this->yaw = this->actor.shape.rot.y = interactRangeActor->yawTowardsPlayer + 0x8000;
                anim = &gPlayerAnim_link_normal_nocarry_free;
            } else {
                Player_actor_set_process(play, this, move_carry, 0);
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_carryB, this->modelAnimType);
            }

            anime_init_standard_stop(play, this, anim);
        }
    } else {
        to_wait_set_check(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
    }
}

void demo_init_talk(PlayState* play, Player* this) {
    Player_actor_anime_move_save_set_process(play, this, move_talk, 0);

    this->stateFlags1 |= PLAYER_STATE1_TALKING | PLAYER_STATE1_29;

    if (this->actor.textId != 0) {
        message_set(play, this->actor.textId, this->talkActor);
        this->focusActor = this->talkActor;
    }
}

void demo_init_ride_horse(PlayState* play, Player* this) {
    Player_actor_anime_move_save_set_process(play, this, move_ride_horse, 0);
}

void demo_init_push(PlayState* play, Player* this) {
    Player_actor_set_process(play, this, move_push_wait, 0);
}

void demo_init_climb(PlayState* play, Player* this) {
    s32 sp1C = this->av2.actionVar2;
    s32 sp18 = this->av1.actionVar1;

    Player_actor_anime_move_save_set_process(play, this, move_climb, 0);
    this->actor.velocity.y = 0.0f;

    this->av2.actionVar2 = sp1C;
    this->av1.actionVar1 = sp18;
}

void demo_init_tunnel(PlayState* play, Player* this) {
    Player_actor_anime_move_save_set_process(play, this, move_tunnel, 0);
}

void demo_init_get_item(PlayState* play, Player* this) {
    Player_actor_anime_move_save_set_process(play, this, move_get_item, 0);

    this->stateFlags1 |= PLAYER_STATE1_10 | PLAYER_STATE1_29;

    if (this->getItemId == GI_HEART_CONTAINER_2) {
        this->av2.actionVar2 = 20;
    } else if (this->getItemId >= 0) {
        this->av2.actionVar2 = 1;
    } else {
        this->getItemId = -this->getItemId;
    }
}

s32 to_jump_check_set(Player* this, PlayState* play) {
    s16 yawDiff;
    LinkAnimationHeader* anim;
    f32 temp;

    yawDiff = this->yaw - this->actor.shape.rot.y;

    if ((ABS(yawDiff) < 0x1000) && (this->speedXZ > 4.0f)) {
        anim = &gPlayerAnim_link_normal_run_jump;
    } else {
        anim = &gPlayerAnim_link_normal_jump;
    }

    if (this->speedXZ > (IREG(66) / 100.0f)) {
        temp = IREG(67) / 100.0f;
    } else {
        temp = (IREG(68) / 100.0f) + ((IREG(69) * this->speedXZ) / 1000.0f);
    }

    to_jump_init(this, anim, temp, play, NA_SE_VO_LI_AUTO_JUMP);
    this->av2.actionVar2 = 1;

    return 1;
}

void to_fall_set(PlayState* play, Player* this, CollisionPoly* arg2, f32 arg3, LinkAnimationHeader* anim) {
    f32 nx = COLPOLY_GET_NORMAL(arg2->normal.x);
    f32 nz = COLPOLY_GET_NORMAL(arg2->normal.z);

    Player_actor_set_process(play, this, move_fall_wait, 0);
    action_reset_all(play, this);
    anime_init_standard_stop(play, this, anim);

    this->actor.world.pos.x -= (arg3 + 1.0f) * nx;
    this->actor.world.pos.z -= (arg3 + 1.0f) * nz;
    this->actor.shape.rot.y = this->yaw = atans_table(nz, nx);

    speedF_look_mode_clear(this);
    anime_move_base_read(this);
}

s32 to_fall_check(Player* this, PlayState* play) {
    //! @bug `floorPitch` and `floorPitchAlt` are cleared to 0 before this function is called, because the player
    //! left the ground. The angles will always be zero and therefore will always pass these checks.
    //! The intention seems to be to prevent ledge hanging or vine grabbing when walking off of a steep enough slope.
    if ((this->actor.depthInWater < -80.0f) && (ABS(this->floorPitch) < 0xAAA) && (ABS(this->floorPitchAlt) < 0xAAA)) {
        CollisionPoly* sp84;
        s32 sp80;
        Vec3f sp74;
        Vec3f sp68;
        f32 temp1;

        sp74.x = this->actor.prevPos.x - this->actor.world.pos.x;
        sp74.z = this->actor.prevPos.z - this->actor.world.pos.z;

        temp1 = sqrtf(SQ(sp74.x) + SQ(sp74.z));
        if (temp1 != 0.0f) {
            temp1 = 5.0f / temp1;
        } else {
            temp1 = 0.0f;
        }

        sp74.x = this->actor.prevPos.x + (sp74.x * temp1);
        sp74.y = this->actor.world.pos.y;
        sp74.z = this->actor.prevPos.z + (sp74.z * temp1);

        if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &sp74, &sp68, &sp84, true, false, false,
                                    true, &sp80) &&
            (ABS(sp84->normal.y) < 600)) {
            f32 nx = COLPOLY_GET_NORMAL(sp84->normal.x);
            f32 ny = COLPOLY_GET_NORMAL(sp84->normal.y);
            f32 nz = COLPOLY_GET_NORMAL(sp84->normal.z);
            f32 sp54;
            s32 sp50;

            sp54 = Math3DLengthPlaneAndPos(nx, ny, nz, sp84->dist, &this->actor.world.pos);

            sp50 = (ground_auto_jump_type == FLOOR_PROPERTY_6);
            if (!sp50 && (T_BGCheck_getWallCodeBit_ai(&play->colCtx, sp84, sp80) & WALL_FLAG_3)) {
                sp50 = 1;
            }

            to_fall_set(play, this, sp84, sp54,
                          sp50 ? &gPlayerAnim_link_normal_Fclimb_startB : &gPlayerAnim_link_normal_fall);

            if (sp50) {
                Player_actor_set_demo_init_proc(play, this, demo_init_climb);

                this->yaw += 0x8000;
                this->actor.shape.rot.y = this->yaw;

                this->stateFlags1 |= PLAYER_STATE1_21;
                anime_move_init(play, this,
                                         ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y |
                                             ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                             ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);

                this->av2.actionVar2 = -1;
                this->av1.actionVar1 = sp50;
            } else {
                this->stateFlags1 |= PLAYER_STATE1_13;
                this->stateFlags1 &= ~PLAYER_STATE1_PARALLEL;
            }

            player_SE_set(this, NA_SE_PL_SLIPDOWN);
            player_voice_SE_set(this, NA_SE_VO_LI_HANG);
            return 1;
        }
    }

    return 0;
}

void to_fall_up_set(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    Player_actor_set_process(play, this, move_fall_up, 0);
    Skeleton_Info_Rom_init_standard_speedset_stop(play, &this->skelAnime, anim, 1.3f);
}

static Vec3f run_jump_water_check_offset = { 0.0f, 0.0f, 100.0f };

void to_landing_check(Player* this, PlayState* play) {
    s32 sp5C;
    CollisionPoly* sp58;
    s32 sp54;
    WaterBox* sp50;
    Vec3f sp44;
    f32 sp40;
    f32 sp3C;

    this->fallDistance = this->fallStartHeight - (s32)this->actor.world.pos.y;

    if (!(this->stateFlags1 & (PLAYER_STATE1_27 | PLAYER_STATE1_29)) &&
        !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (!to_warp_fall_check(play, this)) {
            if (ground_auto_jump_type == FLOOR_PROPERTY_8) {
                this->actor.world.pos.x = this->actor.prevPos.x;
                this->actor.world.pos.z = this->actor.prevPos.z;
                return;
            }

            if (!(this->stateFlags3 & PLAYER_STATE3_1) &&
                !(this->skelAnime.movementFlags & ANIM_FLAG_OVERRIDE_MOVEMENT) &&
                (move_landing != this->actionFunc) && (move_run_jump_water != this->actionFunc)) {

                if ((ground_auto_jump_type == FLOOR_PROPERTY_7) || (this->meleeWeaponState != 0)) {
                    xyz_t_move(&this->actor.world.pos, &this->actor.prevPos);
                    speedF_clear(this);
                    return;
                }

                if (this->hoverBootsTimer != 0) {
                    this->actor.velocity.y = 1.0f;
                    ground_auto_jump_type = FLOOR_PROPERTY_9;
                    return;
                }

                sp5C = (s16)(this->yaw - this->actor.shape.rot.y);

                Player_actor_set_process(play, this, move_landing, 1);
                action_reset(play, this);

                this->floorSfxOffset = this->prevFloorSfxOffset;

                if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_LEAVE) && !(this->stateFlags1 & PLAYER_STATE1_27) &&
                    (ground_auto_jump_type != FLOOR_PROPERTY_6) && (ground_auto_jump_type != FLOOR_PROPERTY_9) &&
                    (to_ground_y > 20.0f) && (this->meleeWeaponState == 0) && (ABS(sp5C) < 0x2000) &&
                    (this->speedXZ > 3.0f)) {

                    if ((ground_auto_jump_type == FLOOR_PROPERTY_11) &&
                        !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {

                        sp40 = player_ground_BGcheck2(play, this, &run_jump_water_check_offset, &sp44, &sp58, &sp54);
                        sp3C = this->actor.world.pos.y;

                        if (T_BGCheck_WaterSurfaceCheck3(play, &play->colCtx, sp44.x, sp44.z, &sp3C, &sp50) &&
                            ((sp3C - sp40) > 50.0f)) {
                            to_jump_set(this, &gPlayerAnim_link_normal_run_jump_water_fall, 6.0f, play);
                            Player_actor_set_process(play, this, move_run_jump_water, 0);
                            return;
                        }
                    }

                    to_jump_check_set(this, play);
                    return;
                }
                if ((ground_auto_jump_type == FLOOR_PROPERTY_9) || (to_ground_y <= this->ageProperties->unk_34) ||
                    !to_fall_check(this, play)) {
                    anime_init_standard_repeat(play, this, &gPlayerAnim_link_normal_landing_wait);
                    return;
                }
            }
        }
    } else {
        this->fallStartHeight = this->actor.world.pos.y;
    }
}

s32 subject_camera_mode_check(PlayState* play, Player* this) {
    s32 camMode;

    if (this->unk_6AD == 2) {
        if (player_bow_check(this)) {
            if (LINK_IS_ADULT) {
                camMode = CAM_MODE_AIM_ADULT;
            } else {
                camMode = CAM_MODE_AIM_CHILD;
            }
        } else {
            camMode = CAM_MODE_AIM_BOOMERANG;
        }
    } else {
        camMode = CAM_MODE_FIRST_PERSON;
    }

    return changeCameraMode(Gama_play_get_camera(play, CAM_ID_MAIN), camMode);
}

/**
 * If appropriate, setup action for performing a `csAction`
 *
 * @return  true if a `csAction` is started, false if not
 */
s32 to_tool_demo_check(PlayState* play, Player* this) {
    // unk_6AD will get set to 3 in `Ext_Player_actor_proc` if `this->csAction` is non-zero
    // (with a special case for `PLAYER_CSACTION_7`)
    if (this->unk_6AD == 3) {
        Player_actor_set_process(play, this, move_demo_play, 0);

        if (this->cv.haltActorsDuringCsAction) {
            this->stateFlags1 |= PLAYER_STATE1_29;
        }

        sword_attack_reset(this);
        return true;
    } else {
        return false;
    }
}

void to_get_item(Player* this, s16 objectId) {
    s32 padding;
    u32 size;

    if (objectId != OBJECT_INVALID) {
        this->giObjectLoading = true;
        osCreateMesgQueue(&this->giObjectLoadQueue, &this->giObjectLoadMsg, 1);

        size = object_exchange_rom_address[objectId].vromEnd - object_exchange_rom_address[objectId].vromStart;

        LOG_HEX("size", size, "../z_player.c", 9090);
        ASSERT(size <= 1024 * 8, "size <= 1024 * 8", "../z_player.c", 9091);

        DMA_REQUEST_ASYNC(&this->giObjectDmaRequest, this->giObjectSegment, object_exchange_rom_address[objectId].vromStart, size, 0,
                          &this->giObjectLoadQueue, NULL, "../z_player.c", 9099);
    }
}

void to_magic_wait_set(PlayState* play, Player* this, s32 magicSpell) {
    Player_actor_now_A_item_save_set_process(play, this, move_magic_wait, 0);

    this->av1.actionVar1 = magicSpell - 3;

    //! @bug `MAGIC_CONSUME_WAIT_PREVIEW` is not guaranteed to succeed.
    //! Ideally, the return value of `magic_meter_check` should be checked before allowing the process of
    //! using a spell to continue. If the magic state change request fails, `z_common_data.magicTarget` will
    //! never be set correctly.
    //! When `MAGIC_STATE_CONSUME_SETUP` is set in `move_magic_wait`, magic will eventually be
    //! consumed to a stale target value. If that stale target value is higher than the current
    //! magic value, it will be consumed to zero.
    magic_meter_check(play, magic_power[magicSpell], MAGIC_CONSUME_WAIT_PREVIEW);

    Skeleton_Info_Rom_init_standard_speedset_stop(play, &this->skelAnime, &gPlayerAnim_link_magic_tame, 0.83f);

    if (magicSpell == 5) {
        this->subCamId = makeOnepointDemo(play, 1100, -101, NULL, CAM_ID_MAIN);
    } else {
        change_item_camera_check_set(play, 10);
    }
}

void look_angle_reset(Player* this) {
    this->actor.focus.rot.x = this->actor.focus.rot.z = this->headLimbRot.x = this->headLimbRot.y =
        this->headLimbRot.z = this->upperLimbRot.x = this->upperLimbRot.y = this->upperLimbRot.z = 0;

    this->actor.focus.rot.y = this->actor.shape.rot.y;
}

static u8 put_item_status[] = {
    GI_ZELDAS_LETTER,       // EXCH_ITEM_ZELDAS_LETTER
    GI_WEIRD_EGG,           // EXCH_ITEM_WEIRD_EGG
    GI_CHICKEN,             // EXCH_ITEM_CHICKEN
    GI_MAGIC_BEAN,          // EXCH_ITEM_MAGIC_BEAN
    GI_POCKET_EGG,          // EXCH_ITEM_POCKET_EGG
    GI_POCKET_CUCCO,        // EXCH_ITEM_POCKET_CUCCO
    GI_COJIRO,              // EXCH_ITEM_COJIRO
    GI_ODD_MUSHROOM,        // EXCH_ITEM_ODD_MUSHROOM
    GI_ODD_POTION,          // EXCH_ITEM_ODD_POTION
    GI_POACHERS_SAW,        // EXCH_ITEM_POACHERS_SAW
    GI_BROKEN_GORONS_SWORD, // EXCH_ITEM_BROKEN_GORONS_SWORD
    GI_PRESCRIPTION,        // EXCH_ITEM_PRESCRIPTION
    GI_EYEBALL_FROG,        // EXCH_ITEM_EYEBALL_FROG
    GI_EYE_DROPS,           // EXCH_ITEM_EYE_DROPS
    GI_CLAIM_CHECK,         // EXCH_ITEM_CLAIM_CHECK
    GI_MASK_SKULL,          // EXCH_ITEM_MASK_SKULL
    GI_MASK_SPOOKY,         // EXCH_ITEM_MASK_SPOOKY
    GI_MASK_KEATON,         // EXCH_ITEM_MASK_KEATON
    GI_MASK_BUNNY_HOOD,     // EXCH_ITEM_MASK_BUNNY_HOOD
    GI_MASK_TRUTH,          // EXCH_ITEM_MASK_TRUTH
    GI_MASK_GORON,          // EXCH_ITEM_MASK_GORON
    GI_MASK_ZORA,           // EXCH_ITEM_MASK_ZORA
    GI_MASK_GERUDO,         // EXCH_ITEM_MASK_GERUDO
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_FISH
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_BLUE_FIRE
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_BUG
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_POE
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_BIG_POE
    GI_BOTTLE_RUTOS_LETTER, // EXCH_ITEM_BOTTLE_RUTOS_LETTER
};

static LinkAnimationHeader* put_item_anm[] = {
    &gPlayerAnim_link_normal_give_other,
    &gPlayerAnim_link_bottle_read,
    &gPlayerAnim_link_normal_take_out,
};

s32 to_look_set_check(Player* this, PlayState* play) {
    s32 sp2C;
    s32 sp28;
    GetItemEntry* giEntry;
    Actor* talkActor;

    if ((this->unk_6AD != 0) && (swim_check(this) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
                                 (this->stateFlags1 & PLAYER_STATE1_23))) {

        if (!to_tool_demo_check(play, this)) {
            if (this->unk_6AD == 4) {
                sp2C = magic_item_check(this, this->itemAction);
                if (sp2C >= 0) {
                    if ((sp2C != 3) || (z_common_data.respawn[RESPAWN_MODE_TOP].data <= 0)) {
                        to_magic_wait_set(play, this, sp2C);
                    } else {
                        Player_actor_set_process(play, this, move_twistar_magic_select, 1);
                        this->stateFlags1 |= PLAYER_STATE1_28 | PLAYER_STATE1_29;
                        anime_init_standard_stop(play, this, wait_anime_select(this));
                        change_item_camera_check_set(play, 4);
                    }

                    speedF_look_mode_clear(this);
                    return 1;
                }

                sp2C = this->itemAction - PLAYER_IA_ZELDAS_LETTER;
                if ((sp2C >= 0) ||
                    (sp28 = bottle_item_check(this, this->itemAction) - 1,
                     ((sp28 >= 0) && (sp28 < 6) &&
                      ((this->itemAction > PLAYER_IA_BOTTLE_POE) ||
                       ((this->talkActor != NULL) && (((this->itemAction == PLAYER_IA_BOTTLE_POE) &&
                                                       (this->exchangeItemId == EXCH_ITEM_BOTTLE_POE)) ||
                                                      (this->exchangeItemId == EXCH_ITEM_BOTTLE_BLUE_FIRE))))))) {

                    if ((play->actorCtx.titleCtx.delayTimer == 0) && (play->actorCtx.titleCtx.alpha == 0)) {
                        Player_actor_now_A_item_save_set_process(play, this, move_put_item, 0);

                        if (sp2C >= 0) {
                            giEntry = &get_item_status[put_item_status[sp2C] - 1];
                            to_get_item(this, giEntry->objectId);
                        }

                        this->stateFlags1 |= PLAYER_STATE1_TALKING | PLAYER_STATE1_28 | PLAYER_STATE1_29;

                        if (sp2C >= 0) {
                            sp2C = sp2C + 1;
                        } else {
                            sp2C = sp28 + 0x18;
                        }

                        talkActor = this->talkActor;

                        if ((talkActor != NULL) &&
                            ((this->exchangeItemId == sp2C) || (this->exchangeItemId == EXCH_ITEM_BOTTLE_BLUE_FIRE) ||
                             ((this->exchangeItemId == EXCH_ITEM_BOTTLE_POE) &&
                              (this->itemAction == PLAYER_IA_BOTTLE_BIG_POE)) ||
                             ((this->exchangeItemId == EXCH_ITEM_MAGIC_BEAN) &&
                              (this->itemAction == PLAYER_IA_BOTTLE_BUG))) &&
                            ((this->exchangeItemId != EXCH_ITEM_MAGIC_BEAN) ||
                             (this->itemAction == PLAYER_IA_MAGIC_BEAN))) {
                            if (this->exchangeItemId == EXCH_ITEM_MAGIC_BEAN) {
                                item_count_vary(ITEM_MAGIC_BEAN, -1);
                                Player_actor_now_A_item_save_set_process(play, this, move_bean_set, 0);
                                this->stateFlags1 |= PLAYER_STATE1_29;
                                this->av2.actionVar2 = 0x50;
                                this->av1.actionVar1 = -1;
                            }
                            talkActor->flags |= ACTOR_FLAG_TALK;
                            this->focusActor = this->talkActor;
                        } else if (sp2C == EXCH_ITEM_BOTTLE_RUTOS_LETTER) {
                            this->av1.actionVar1 = 1;
                            this->actor.textId = 0x4005;
                            change_item_camera_check_set(play, 1);
                        } else {
                            this->av1.actionVar1 = 2;
                            this->actor.textId = 0xCF;
                            change_item_camera_check_set(play, 4);
                        }

                        this->actor.flags |= ACTOR_FLAG_TALK;
                        this->exchangeItemId = sp2C;

                        if (this->av1.actionVar1 < 0) {
                            anime_init_stop_basic(play, this,
                                                       GET_PLAYER_ANIM(PLAYER_ANIMGROUP_check, this->modelAnimType));
                        } else {
                            anime_init_standard_stop(play, this, put_item_anm[this->av1.actionVar1]);
                        }

                        speedF_look_mode_clear(this);
                    }
                    return 1;
                }

                sp2C = bottle_item_check(this, this->itemAction);
                if (sp2C >= 0) {
                    if (sp2C == 0xC) {
                        Player_actor_now_A_item_save_set_process(play, this, move_bottle_bug_out, 0);
                        anime_init_standard_stop_3f(play, this, &gPlayerAnim_link_bottle_bug_out);
                        change_item_camera_check_set(play, 3);
                    } else if ((sp2C > 0) && (sp2C < 4)) {
                        Player_actor_now_A_item_save_set_process(play, this, move_bottle_fish_out, 0);
                        anime_init_standard_stop_3f(play, this, &gPlayerAnim_link_bottle_fish_out);
                        change_item_camera_check_set(play, (sp2C == 1) ? 1 : 5);
                    } else {
                        Player_actor_now_A_item_save_set_process(play, this, move_bottle_drink, 0);
                        anime_init_stop_basic_3f(play, this, &gPlayerAnim_link_bottle_drink_demo_start);
                        change_item_camera_check_set(play, 2);
                    }
                } else {
                    Player_actor_now_A_item_save_set_process(play, this, move_okarina, 0);
                    anime_init_standard_stop_3f(play, this, &gPlayerAnim_link_normal_okarina_start);
                    this->stateFlags2 |= PLAYER_STATE2_USING_OCARINA;
                    change_item_camera_check_set(play, (this->unk_6A8 != NULL) ? 0x5B : 0x5A);
                    if (this->unk_6A8 != NULL) {
                        this->stateFlags2 |= PLAYER_STATE2_25;
                        lockCamera(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_VIEW_TARGET, this->unk_6A8);
                    }
                }
            } else if (subject_camera_mode_check(play, this) != CAM_MODE_NORMAL) {
                if (!(this->stateFlags1 & PLAYER_STATE1_23)) {
                    Player_actor_set_process(play, this, move_look, 1);
                    this->av2.actionVar2 = 13;
                    look_angle_reset(this);
                }
                this->stateFlags1 |= PLAYER_STATE1_20;
                Na_StartSystemSe_F(NA_SE_SY_CAMERA_ZOOM_UP);
                speedF_clear(this);
                return 1;
            } else {
                this->unk_6AD = 0;
                Na_StartSystemSe_F(NA_SE_SY_ERROR);
                return 0;
            }

            this->stateFlags1 |= PLAYER_STATE1_28 | PLAYER_STATE1_29;
        }

        speedF_look_mode_clear(this);
        return 1;
    }

    return 0;
}

s32 to_talk_check(Player* this, PlayState* play) {
    Actor* talkOfferActor = this->talkActor;
    Actor* lockOnActor = this->focusActor;
    Actor* cUpTalkActor = NULL;
    s32 forceTalkToNavi = false;
    s32 canTalkToLockOnWithCUp;

    canTalkToLockOnWithCUp =
        (lockOnActor != NULL) &&
        (CHECK_FLAG_ALL(lockOnActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_TALK_WITH_C_UP) ||
         (lockOnActor->naviEnemyId != NAVI_ENEMY_NONE));

    if (canTalkToLockOnWithCUp || (this->naviTextId != 0)) {
        // If `naviTextId` is negative and outside the 0x2XX range, talk to Navi instantly
        forceTalkToNavi = (this->naviTextId < 0) && ((ABS(this->naviTextId) & 0xFF00) != 0x200);

        if (forceTalkToNavi || !canTalkToLockOnWithCUp) {
            // If `lockOnActor` can't be talked to with c-up, the only option left is Navi
            cUpTalkActor = this->naviActor;

            if (forceTalkToNavi) {
                // Clearing these pointers guarantees that `cUpTalkActor` will take priority
                lockOnActor = NULL;
                talkOfferActor = NULL;
            }
        } else {
            // Navi is not the talk actor, so the only option left for talking with c-up is `lockOnActor`
            // (though, `lockOnActor` may be NULL at this point).
            cUpTalkActor = lockOnActor;
        }
    }

    if ((talkOfferActor != NULL) || (cUpTalkActor != NULL)) {
        if ((lockOnActor != NULL) && (lockOnActor != talkOfferActor) && (lockOnActor != cUpTalkActor)) {
            goto dont_talk;
        }

        if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
            if ((this->heldActor == NULL) ||
                (!forceTalkToNavi && (talkOfferActor != this->heldActor) && (cUpTalkActor != this->heldActor) &&
                 ((talkOfferActor == NULL) || !(talkOfferActor->flags & ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED)))) {
                goto dont_talk;
            }
        }

        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            if (!(this->stateFlags1 & PLAYER_STATE1_23) &&
                !(swim_check(this) && !(this->stateFlags2 & PLAYER_STATE2_10))) {
                goto dont_talk;
            }
        }

        if (talkOfferActor != NULL) {
            // At this point the talk offer can be accepted.
            // "Speak" or "Check" will appear on the A button in the HUD.
            this->stateFlags2 |= PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER;

            if (CHECK_BTN_ALL(pad->press.button, BTN_A) ||
                (talkOfferActor->flags & ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED)) {
                // Talk Offer has been accepted.
                // Clearing `cUpTalkActor` guarantees that `talkOfferActor` is the actor that will be spoken to
                cUpTalkActor = NULL;
            } else if (cUpTalkActor == NULL) {
                return false;
            }
        }

        if (cUpTalkActor != NULL) {
            if (!forceTalkToNavi) {
                this->stateFlags2 |= PLAYER_STATE2_21;
            }

            if (!CHECK_BTN_ALL(pad->press.button, BTN_CUP) && !forceTalkToNavi) {
                return false;
            }

            talkOfferActor = cUpTalkActor;
            this->talkActor = NULL;

            if (forceTalkToNavi || !canTalkToLockOnWithCUp) {
                cUpTalkActor->textId = ABS(this->naviTextId);
            } else if (cUpTalkActor->naviEnemyId != NAVI_ENEMY_NONE) {
                cUpTalkActor->textId = cUpTalkActor->naviEnemyId + 0x600;
            }
        }

        // `old_mask` saves the current mask just before the current action runs on this frame.
        // This saved mask value is then restored just before starting a conversation.
        //
        // This handles an edge case where a conversation is started on the same frame that a mask was taken on or off.
        // Because Player updates early before most actors, the text ID being offered comes from the previous frame.
        // If a mask was taken on or off the same frame this function runs, the wrong text will be used.
        // This is especially important to prevent unwanted behavior with regards to mask trading.
        this->currentMask = old_mask;

        Ext_player_to_talk_set(play, talkOfferActor);

        return true;
    }

dont_talk:
    return false;
}

s32 to_subject_check(Player* this, PlayState* play) {
    if (!(this->stateFlags1 & (PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_23)) &&
        useCameraModeOK(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_MODE_FIRST_PERSON)) {
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
            (swim_check(this) && (this->actor.depthInWater < this->ageProperties->unk_2C))) {
            this->unk_6AD = 1;
            return 1;
        }
    }

    return 0;
}

s32 to_look_check(Player* this, PlayState* play) {
    if (this->unk_6AD != 0) {
        to_look_set_check(this, play);
        return 1;
    }

    if ((this->focusActor != NULL) &&
        (CHECK_FLAG_ALL(this->focusActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_TALK_WITH_C_UP) ||
         (this->focusActor->naviEnemyId != NAVI_ENEMY_NONE))) {
        this->stateFlags2 |= PLAYER_STATE2_21;
    } else if ((this->naviTextId == 0) && !anchor_mode_check(this) &&
               CHECK_BTN_ALL(pad->press.button, BTN_CUP) &&
               (R_SCENE_CAM_TYPE != SCENE_CAM_TYPE_FIXED_SHOP_VIEWPOINT) &&
               (R_SCENE_CAM_TYPE != SCENE_CAM_TYPE_FIXED_TOGGLE_VIEWPOINT) && !to_subject_check(this, play)) {
        Na_StartSystemSe_F(NA_SE_SY_ERROR);
    }

    return 0;
}

void to_jump_kiru_set(PlayState* play, Player* this, s32 arg2, f32 xzSpeed, f32 yVelocity) {
    to_kiru_set(play, this, arg2);
    Player_actor_set_process(play, this, move_jump_kiru, 0);

    this->stateFlags3 |= PLAYER_STATE3_1;

    this->yaw = this->actor.shape.rot.y;
    this->speedXZ = xzSpeed;
    this->actor.velocity.y = yVelocity;

    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->hoverBootsTimer = 0;

    player_jump_SE_set(this);
    player_voice_SE_set(this, NA_SE_VO_LI_SWORD_L);
}

s32 kiru_check(Player* this) {
    if (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) && (sword_check(this) != 0)) {
        if (item_key ||
            ((this->actor.category != ACTORCAT_PLAYER) && CHECK_BTN_ALL(pad->press.button, BTN_B))) {
            return 1;
        }
    }

    return 0;
}

s32 to_jump_kiru_check(Player* this, PlayState* play) {
    if (kiru_check(this) && (ground_attribute != FLOOR_TYPE_7)) {
        to_jump_kiru_set(play, this, PLAYER_MWA_JUMPSLASH_START, 3.0f, 4.5f);
        return 1;
    }

    return 0;
}

void to_landing_roll_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_landing_roll, 0);
    Skeleton_Info_Rom_init_standard_speedset_stop(play, &this->skelAnime,
                                   GET_PLAYER_ANIM(PLAYER_ANIMGROUP_landing_roll, this->modelAnimType),
                                   FRAMERATE_CONST(1.25f, 1.5f) * environment_speed_ratio);
}

s32 to_attack_check(Player* this, PlayState* play) {
    if ((this->controlStickDirections[this->controlStickDataIndex] == PLAYER_STICK_DIR_FORWARD) &&
        (ground_attribute != FLOOR_TYPE_7)) {
        to_landing_roll_set(this, play);

        return true;
    }

    return false;
}

void anchor_jump_set(Player* this, PlayState* play, s32 controlStickDirection) {
    to_jump_init(this, anchor_jump_anm_data[controlStickDirection][0], !(controlStickDirection & 1) ? 5.8f : 3.5f, play,
                  NA_SE_VO_LI_SWORD_N);

    if (controlStickDirection) {}

    this->av2.actionVar2 = 1;
    this->av1.actionVar1 = controlStickDirection;

    this->yaw = this->actor.shape.rot.y + (controlStickDirection << 0xE);
    this->speedXZ = !(controlStickDirection & 1) ? 6.0f : 8.5f;

    this->stateFlags2 |= PLAYER_STATE2_19;

    player_SE_set(this, ((controlStickDirection << 0xE) == 0x8000) ? NA_SE_PL_ROLL : NA_SE_PL_SKIP);
}

s32 to_anchor_jump_check(Player* this, PlayState* play) {
    s32 controlStickDirection;

    if (CHECK_BTN_ALL(pad->press.button, BTN_A) && (play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) &&
        (ground_attribute != FLOOR_TYPE_7) &&
        (T_BGCheck_getFriction_ai(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) != FLOOR_EFFECT_1)) {
        controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];

        if (controlStickDirection <= PLAYER_STICK_DIR_FORWARD) {
            if (anchor_parallel_mode_check(this)) {
                if (this->actor.category != ACTORCAT_PLAYER) {
                    if (controlStickDirection <= PLAYER_STICK_DIR_NONE) {
                        to_jump_set(this, &gPlayerAnim_link_normal_jump, REG(69) / 100.0f, play);
                    } else {
                        to_landing_roll_set(this, play);
                    }
                } else {
                    if ((sword_check(this) != 0) && item_action_ok_check(this)) {
                        to_jump_kiru_set(play, this, PLAYER_MWA_JUMPSLASH_START, 5.0f, 5.0f);
                    } else {
                        to_landing_roll_set(this, play);
                    }
                }

                return 1;
            }
        } else {
            anchor_jump_set(this, play, controlStickDirection);

            return 1;
        }
    }

    return 0;
}

void to_wait_from_run_anime_set(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 sp30;

    sp30 = this->unk_868 - 3.0f;
    if (sp30 < 0.0f) {
        sp30 += 29.0f;
    }

    if (sp30 < 14.0f) {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk_endL, this->modelAnimType);
        sp30 = 11.0f - sp30;
        if (sp30 < 0.0f) {
            sp30 = 1.375f * -sp30;
        }
        sp30 /= 11.0f;
    } else {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk_endR, this->modelAnimType);
        sp30 = 26.0f - sp30;
        if (sp30 < 0.0f) {
            sp30 = 2 * -sp30;
        }
        sp30 /= 12.0f;
    }

    Skeleton_Info_Rom_init(play, &this->skelAnime, anim, 1.0f, 0.0f, Si2_anime_end_frame(anim), ANIMMODE_ONCE,
                         4.0f * sp30);
    this->yaw = this->actor.shape.rot.y;
}

void to_wait_from_run_set(Player* this, PlayState* play) {
    to_wait_proc_set(this, play);
    to_wait_from_run_anime_set(this, play);
}

void to_wait_no_morf_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_wait, 1);
    anime_init_standard_stop(play, this, wait_anime_select(this));
    this->yaw = this->actor.shape.rot.y;
}

void to_move_set(Player* this, PlayState* play) {
    if (!(this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT)) {
        look_angle_reset(this);
        if (this->stateFlags1 & PLAYER_STATE1_27) {
            to_swim_wait_set(play, this);
        } else {
            to_wait_set_check(this, play);
        }
        if (this->unk_6AD < 4) {
            this->unk_6AD = 0;
        }
    }

    this->stateFlags1 &= ~(PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_20);
}

/**
 * Handles setting up a roll if it is appropriate.
 *
 * If a roll is not applicable, there are two extra behaviors that could occur:
 * - If an item is currently held in hand, it can be put away.
 * - Navi can be toggled on and off.
 *   She will either appear and fly around Link's head, or disappear by flying back into his hat
 *
 * These extra behaviors are not new actions themselves, so they will result in `false` being returned
 * even if they occur.
 */
s32 to_check_check(Player* this, PlayState* play) {
    if (!anchor_mode_check_set(this) && !uperbody_action_flag && !(this->stateFlags1 & PLAYER_STATE1_23) &&
        CHECK_BTN_ALL(pad->press.button, BTN_A)) {
        if (to_attack_check(this, play)) {
            return true;
        } else if ((this->putAwayCooldownTimer == 0) && (this->heldItemAction >= PLAYER_IA_SWORD_MASTER)) {
            player_item_on(play, this, ITEM_NONE);
        } else {
            this->stateFlags2 ^= PLAYER_STATE2_NAVI_ACTIVE;
        }
    }

    return false;
}

s32 to_defense_check(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;
    f32 frame;

    if ((play->shootingGalleryStatus == 0) && (this->currentShield != PLAYER_SHIELD_NONE) &&
        CHECK_BTN_ALL(pad->cur.button, BTN_R) &&
        (child_hyral_shield_check(this) ||
         (!parallel_mode_check(this) && (this->focusActor == NULL)))) {

        sword_attack_reset(this);
        l_hand_child_cancel(play, this);

        if (Player_actor_set_process(play, this, move_defense, 0)) {
            this->stateFlags1 |= PLAYER_STATE1_SHIELDING;

            if (!child_hyral_shield_check(this)) {
                defense_shape_type_set(this);
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense, this->modelAnimType);
            } else {
                anim = &gPlayerAnim_clink_normal_defense_ALL;
            }

            if (anim != this->skelAnime.animation) {
                if (anchor_mode_check(this)) {
                    this->unk_86C = 1.0f;
                } else {
                    this->unk_86C = 0.0f;
                    anchor_LR_ratio_reset(this);
                }
                this->upperLimbRot.x = this->upperLimbRot.y = this->upperLimbRot.z = 0;
            }

            frame = Si2_anime_end_frame(anim);
            Skeleton_Info_Rom_init(play, &this->skelAnime, anim, 1.0f, frame, frame, ANIMMODE_ONCE, 0.0f);

            if (child_hyral_shield_check(this)) {
                anime_move_init(play, this, ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT);
            }

            player_SE_set(this, NA_SE_IT_SHIELD_POSTURE);
        }

        return 1;
    }

    return 0;
}

s32 to_brake_check(Player* this, f32* arg1, s16* arg2) {
    s16 yaw = this->yaw - *arg2;

    if (ABS(yaw) > 0x6000) {
        if (ground_breaking_speedF(this)) {
            *arg1 = 0.0f;
            *arg2 = this->yaw;
        } else {
            return 1;
        }
    }

    return 0;
}

void combo_key_check(Player* this) {
    if ((this->unk_844 > 0) && !CHECK_BTN_ALL(pad->cur.button, BTN_B)) {
        this->unk_844 = -this->unk_844;
    }
}

s32 to_power_kiru_charge_check(Player* this, PlayState* play) {
    if (CHECK_BTN_ALL(pad->cur.button, BTN_B)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) && (sword_check2(this) != 0) &&
            (this->unk_844 == 1) && (this->heldItemAction != PLAYER_IA_DEKU_STICK)) {
            if ((this->heldItemAction != PLAYER_IA_SWORD_BIGGORON) ||
                (z_common_data.save.info.playerData.swordHealth > 0.0f)) {
                to_power_kiru_start_set(play, this);
                return 1;
            }
        }
    } else {
        combo_key_check(this);
    }

    return 0;
}

s32 to_light_bom_check(PlayState* play, Player* this) {
    if ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        (AMMO(ITEM_DEKU_NUT) != 0)) {
        Player_actor_set_process(play, this, move_light_bom, 0);
        anime_init_standard_stop(play, this, &gPlayerAnim_link_normal_light_bom);
        this->unk_6AD = 0;
        return 1;
    }

    return 0;
}

typedef struct BottleSwingInfo {
    /* 0x00 */ LinkAnimationHeader* missAnimation;
    /* 0x04 */ LinkAnimationHeader* catchAnimation;
    /* 0x08 */ u8 firstActiveFrame;
    /* 0x09 */ u8 numActiveFrames;
} BottleSwingInfo; // size = 0x0C

static BottleSwingInfo bottle_attack_status[] = {
    { &gPlayerAnim_link_bottle_bug_miss, &gPlayerAnim_link_bottle_bug_in, 2, 3 },   // on land
    { &gPlayerAnim_link_bottle_fish_miss, &gPlayerAnim_link_bottle_fish_in, 5, 3 }, // in water
};

s32 to_arms_attack_check(PlayState* play, Player* this) {
    if (item_key) {
        if (bottle_check(this) >= 0) {
            Player_actor_set_process(play, this, move_bottle_attack, 0);

            if (this->actor.depthInWater > 12.0f) {
                this->av2.inWater = true;
            }

            anime_init_standard_stop_3f(play, this, bottle_attack_status[this->av2.inWater].missAnimation);

            player_SE_set(this, NA_SE_IT_SWORD_SWING);
            player_voice_SE_set(this, NA_SE_VO_LI_AUTO_JUMP);
            return 1;
        }

        if (this->heldItemAction == PLAYER_IA_FISHING_POLE) {
            Vec3f rodCheckPos = this->actor.world.pos;

            rodCheckPos.y += 50.0f;

#if OOT_VERSION < NTSC_1_1
            if (T_BGCheck_SimpleCheck(&play->colCtx, &rodCheckPos, 20.0f))
#else
            if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.world.pos.z > 1300.0f) ||
                T_BGCheck_SimpleCheck(&play->colCtx, &rodCheckPos, 20.0f))
#endif
            {
                Na_StartSystemSe_F(NA_SE_SY_ERROR);
                return 0;
            }

#if OOT_VERSION < NTSC_1_1
            this->unk_860 = 1;
            Player_actor_set_process(play, this, move_fishing_throw, 0);
#else
            Player_actor_set_process(play, this, move_fishing_throw, 0);
            this->unk_860 = 1;
            speedF_clear(this);
#endif

            anime_init_standard_stop(play, this, &gPlayerAnim_link_fishing_throw);
            return 1;
        } else {
            return 0;
        }
    }

    return 0;
}

void to_run_set(Player* this, PlayState* play) {
    PlayerActionFunc actionFunc;

    if (anchor_parallel_mode_check(this)) {
        actionFunc = move_anchor_run;
    } else {
        actionFunc = move_run;
    }

    Player_actor_set_process(play, this, actionFunc, 1);
    anime_init_repeat_basic(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_run, this->modelAnimType));

    this->unk_89C = 0;
    this->unk_864 = this->unk_868 = 0.0f;
}

void to_run_set2(Player* this, PlayState* play, s16 arg2) {
    this->actor.shape.rot.y = this->yaw = arg2;
    to_run_set(this, play);
}

s32 to_demo_run_start_init(PlayState* play, Player* this, f32 arg2) {
    WaterBox* sp2C;
    f32 sp28;

    sp28 = this->actor.world.pos.y;
    if (T_BGCheck_WaterSurfaceCheck3(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp28, &sp2C) !=
        0) {
        sp28 -= this->actor.world.pos.y;
        if (this->ageProperties->unk_24 <= sp28) {
            Player_actor_set_process(play, this, move_swim_demo, 0);
            anime_init_0_repeat_basic(play, this, &gPlayerAnim_link_swimer_swim);
            this->stateFlags1 |= PLAYER_STATE1_27 | PLAYER_STATE1_29;
            this->av2.actionVar2 = 20;
            this->speedXZ = 2.0f;
            player_performance_init(play, this);
            return 0;
        }
    }

    to_demo_run_init(play, this, arg2, this->actor.shape.rot.y);
    this->stateFlags1 |= PLAYER_STATE1_29;
    return 1;
}

void to_demo_wait_start_set(PlayState* play, Player* this) {
    if (to_demo_run_start_init(play, this, 180.0f)) {
        this->av2.actionVar2 = -20;
    }
}

void to_demo_run_start_set(PlayState* play, Player* this) {
    this->speedXZ = 2.0f;
    z_common_data.entranceSpeed = 2.0f;

    if (to_demo_run_start_init(play, this, 120.0f)) {
        this->av2.actionVar2 = -15;
    }
}

void to_run_start_set(PlayState* play, Player* this) {
    if (z_common_data.entranceSpeed < 0.1f) {
        z_common_data.entranceSpeed = 0.1f;
    }

    this->speedXZ = z_common_data.entranceSpeed;

    if (to_demo_run_start_init(play, this, 800.0f)) {
        this->av2.actionVar2 = -80 / this->speedXZ;

        if (this->av2.actionVar2 < -20) {
            this->av2.actionVar2 = -20;
        }
    }
}

void to_parallel_back_run_set(Player* this, s16 yaw, PlayState* play) {
    Player_actor_set_process(play, this, move_parallel_back_run, 1);
    Skeleton_Info_Rom_anime_now_to_morf(play, &this->skelAnime);
    this->unk_864 = this->unk_868 = 0.0f;
    this->yaw = yaw;
}

void to_parallel_side_walk_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_parallel_side_walk, 1);
    anime_init_repeat_basic(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType));
}

void to_anchor_back_walk_set(Player* this, s16 yaw, PlayState* play) {
    Player_actor_set_process(play, this, move_anchor_back_walk, 1);
    Skeleton_Info_Rom_init(play, &this->skelAnime, &gPlayerAnim_link_anchor_back_walk, 2.2f, 0.0f,
                         Si2_anime_end_frame(&gPlayerAnim_link_anchor_back_walk), ANIMMODE_ONCE, -6.0f);
    this->speedXZ = 8.0f;
    this->yaw = yaw;
}

void to_anchor_side_walk_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_anchor_side_walk, 1);
    anime_init_repeat_basic(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkR, this->modelAnimType));
    this->unk_868 = 0.0f;
}

void to_anchor_back_brake_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_anchor_back_brake, 1);
    Skeleton_Info_Rom_init_standard_speedset_stop(play, &this->skelAnime, &gPlayerAnim_link_anchor_back_brake, 2.0f);
}

void to_turn_set(PlayState* play, Player* this, s16 yaw) {
    this->yaw = yaw;

    Player_actor_set_process(play, this, move_turn, 1);

    this->turnRate = 1200;
    this->turnRate *= environment_speed_ratio; // slow turn rate by half when in water

    Skeleton_Info_Rom_init(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_45_turn, this->modelAnimType), 1.0f,
                         0.0f, 0.0f, ANIMMODE_LOOP, -6.0f);
}

void to_wait_from_anchor_wait_set(Player* this, PlayState* play) {
    LinkAnimationHeader* anim;

    Player_actor_set_process(play, this, move_wait, 1);

    if (this->unk_870 < 0.5f) {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_waitR2wait, this->modelAnimType);
    } else {
        anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_waitL2wait, this->modelAnimType);
    }
    anime_init_standard_stop(play, this, anim);

    this->yaw = this->actor.shape.rot.y;
}

void to_anchor_wait_from_wait_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_anchor_wait, 1);
    anime_init_stop_basic(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait2waitR, this->modelAnimType));
    this->av2.actionVar2 = 1;
}

void to_wait_run_set(Player* this, PlayState* play) {
    if (this->speedXZ != 0.0f) {
        to_run_set(this, play);
    } else {
        to_wait_from_anchor_wait_set(this, play);
    }
}

void to_wait_run_check_set(Player* this, PlayState* play) {
    if (this->speedXZ != 0.0f) {
        to_run_set(this, play);
    } else {
        to_wait_set_check(this, play);
    }
}

s32 effect_splash2_set(PlayState* play, Player* this, f32 arg2, s32 splashScale) {
    f32 sp3C = fabsf(arg2);
    WaterBox* sp38;
    f32 sp34;
    Vec3f splashPos;
    s32 splashType;

    if (sp3C > 2.0f) {
        splashPos.x = this->bodyPartsPos[PLAYER_BODYPART_WAIST].x;
        splashPos.z = this->bodyPartsPos[PLAYER_BODYPART_WAIST].z;
        sp34 = this->actor.world.pos.y;
        if (T_BGCheck_WaterSurfaceCheck3(play, &play->colCtx, splashPos.x, splashPos.z, &sp34, &sp38)) {
#if OOT_VERSION < PAL_1_0
            if ((sp34 - this->actor.world.pos.y) < 80.0f)
#else
            if ((sp34 - this->actor.world.pos.y) < 100.0f)
#endif
            {
                splashType = (sp3C <= 10.0f) ? 0 : 1;
                splashPos.y = sp34;
                Effect_SS_G_Splash_sc_cl_ct(play, &splashPos, NULL, NULL, splashType, splashScale);
                return 1;
            }
        }
    }

    return 0;
}

void water_out_set(PlayState* play, Player* this, f32 arg2) {
    this->stateFlags1 |= PLAYER_STATE1_18;
    this->stateFlags1 &= ~PLAYER_STATE1_27;

    sub_camera_mode_reset(play, this);
    if (effect_splash2_set(play, this, arg2, 500)) {
        player_SE_set(this, NA_SE_EV_JUMP_OUT_WATER);
    }

    player_performance_init(play, this);
}

s32 to_swim_deep_check(PlayState* play, Player* this, Input* arg2) {
    if (!(this->stateFlags1 & PLAYER_STATE1_10) && !(this->stateFlags2 & PLAYER_STATE2_10)) {
        if ((arg2 == NULL) || (CHECK_BTN_ALL(arg2->press.button, BTN_A) && (ABS(this->unk_6C2) < 12000) &&
                               (this->currentBoots != PLAYER_BOOTS_IRON))) {

            Player_actor_set_process(play, this, move_swim_deep, 0);
            anime_init_standard_stop(play, this, &gPlayerAnim_link_swimer_swim_deep_start);

            this->unk_6C2 = 0;
            this->stateFlags2 |= PLAYER_STATE2_10;
            this->actor.velocity.y = 0.0f;

            if (arg2 != NULL) {
                this->stateFlags2 |= PLAYER_STATE2_11;
                player_SE_set(this, NA_SE_PL_DIVE_BUBBLE);
            }

            return 1;
        }
    }

    if ((this->stateFlags1 & PLAYER_STATE1_10) || (this->stateFlags2 & PLAYER_STATE2_10)) {
        if (this->actor.velocity.y > 0.0f) {
            if (this->actor.depthInWater < this->ageProperties->unk_30) {

                this->stateFlags2 &= ~PLAYER_STATE2_10;

                if (arg2 != NULL) {
                    Player_actor_set_process(play, this, move_swim_deep_end, 1);

                    if (this->stateFlags1 & PLAYER_STATE1_10) {
                        this->stateFlags1 |= PLAYER_STATE1_10 | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_29;
                    }

                    this->av2.actionVar2 = 2;
                }

                sub_camera_mode_reset(play, this);
                anime_init_stop_basic(play, this,
                                           (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)
                                               ? &gPlayerAnim_link_swimer_swim_get
                                               : &gPlayerAnim_link_swimer_swim_deep_end);

                if (effect_splash2_set(play, this, this->actor.velocity.y, 500)) {
                    player_SE_set(this, NA_SE_PL_FACE_UP);
                }

                return 1;
            }
        }
    }

    return 0;
}

void deep_swim_up_init(PlayState* play, Player* this) {
    anime_init_standard_repeat(play, this, &gPlayerAnim_link_swimer_swim);
    this->unk_6C2 = 16000;
    this->av2.actionVar2 = 1;
}

void water_in_set(PlayState* play, Player* this) {
    if ((this->currentBoots != PLAYER_BOOTS_IRON) || !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        action_reset_all(play, this);

        if ((this->currentBoots != PLAYER_BOOTS_IRON) && (this->stateFlags2 & PLAYER_STATE2_10)) {
            this->stateFlags2 &= ~PLAYER_STATE2_10;
            to_swim_deep_check(play, this, NULL);
            this->av1.actionVar1 = 1;
        } else if (move_run_jump_water == this->actionFunc) {
            Player_actor_set_process(play, this, move_swim_deep, 0);
            deep_swim_up_init(play, this);
        } else {
            Player_actor_set_process(play, this, move_swim_wait, 1);
            anime_init_stop_basic(play, this,
                                       (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)
                                           ? &gPlayerAnim_link_swimer_wait2swim_wait
                                           : &gPlayerAnim_link_swimer_land2swim_wait);
        }
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_27) || (this->actor.depthInWater < this->ageProperties->unk_2C)) {
        if (effect_splash2_set(play, this, this->actor.velocity.y, 500)) {
            player_SE_set(this, NA_SE_EV_DIVE_INTO_WATER);

            if (this->fallDistance > 800.0f) {
                player_voice_SE_set(this, NA_SE_VO_LI_CLIMB_END);
            }
        }
    }

    this->stateFlags1 |= PLAYER_STATE1_27;
    this->stateFlags2 |= PLAYER_STATE2_10;
    this->stateFlags1 &= ~(PLAYER_STATE1_18 | PLAYER_STATE1_19);
    this->unk_854 = 0.0f;

    player_performance_init(play, this);
}

static void water_check(PlayState* play, Player* this) {
    if (this->actor.depthInWater < this->ageProperties->unk_2C) {
        Na_SetDirectFir(0);
        this->underwaterTimer = 0;
    } else {
        Na_SetDirectFir(0x20);
        if (this->underwaterTimer < 300) {
            this->underwaterTimer++;
        }
    }

    if ((move_stepup100 != this->actionFunc) && (move_fall_up != this->actionFunc)) {
        if (this->ageProperties->unk_2C < this->actor.depthInWater) {
            if (!(this->stateFlags1 & PLAYER_STATE1_27) ||
                (!((this->currentBoots == PLAYER_BOOTS_IRON) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) &&
                 (move_swim_damage != this->actionFunc) && (move_swim_down != this->actionFunc) &&
                 (move_swim_wait != this->actionFunc) && (move_swim != this->actionFunc) &&
                 (move_anchor_swim != this->actionFunc) && (move_swim_deep != this->actionFunc) &&
                 (move_swim_deep_end != this->actionFunc) && (move_swim_demo != this->actionFunc))) {
                water_in_set(play, this);
                return;
            }
        } else if ((this->stateFlags1 & PLAYER_STATE1_27) && (this->actor.depthInWater < this->ageProperties->unk_24)) {
            if ((this->skelAnime.movementFlags == 0) && (this->currentBoots != PLAYER_BOOTS_IRON)) {
                to_turn_set(play, this, this->actor.shape.rot.y);
            }
            water_out_set(play, this, this->actor.velocity.y);
        }
    }
}

void ground_condition_check(PlayState* play, Player* this) {
    Vec3f ripplePos;

    this->actor.minVelocityY = FRAMERATE_CONST(-20.0f, -24.0f);
    this->actor.gravity = REG(68) / 100.0f;

    if (desert_ground_attribute_check(ground_attribute)) {
        f32 temp1;
        f32 temp2;
        f32 temp3;

        temp1 = fabsf(this->speedXZ) * 20.0f;
        temp3 = 0.0f;

        if (ground_attribute == FLOOR_TYPE_4) {
            if (this->unk_6C4 > 1300.0f) {
                temp2 = this->unk_6C4;
            } else {
                temp2 = 1300.0f;
            }
            if (this->currentBoots == PLAYER_BOOTS_HOVER) {
                temp1 += temp1;
            } else if (this->currentBoots == PLAYER_BOOTS_IRON) {
                temp1 *= 0.3f;
            }
        } else {
            temp2 = 20000.0f;
            if (this->currentBoots != PLAYER_BOOTS_HOVER) {
                temp1 += temp1;
            } else if ((ground_attribute == FLOOR_TYPE_7) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
                temp1 = 0;
            }
        }

        if (this->currentBoots != PLAYER_BOOTS_HOVER) {
            temp3 = (temp2 - this->unk_6C4) * 0.02f;
            temp3 = CLAMP(temp3, 0.0f, 300.0f);
            if (this->currentBoots == PLAYER_BOOTS_IRON) {
                temp3 += temp3;
            }
        }

        this->unk_6C4 += temp3 - temp1;
        this->unk_6C4 = CLAMP(this->unk_6C4, 0.0f, temp2);

        this->actor.gravity -= this->unk_6C4 * 0.004f;
    } else {
        this->unk_6C4 = 0.0f;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
        if (this->actor.depthInWater < 50.0f) {
            f32 temp4;

            temp4 = fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].x - this->unk_A88.x) +
                    fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].y - this->unk_A88.y) +
                    fabsf(this->bodyPartsPos[PLAYER_BODYPART_WAIST].z - this->unk_A88.z);
            if (temp4 > 4.0f) {
                temp4 = 4.0f;
            }
            this->unk_854 += temp4;

            if (this->unk_854 > 15.0f) {
                this->unk_854 = 0.0f;

                ripplePos.x = (fqrand() * 10.0f) + this->actor.world.pos.x;
                ripplePos.y = this->actor.world.pos.y + this->actor.depthInWater;
                ripplePos.z = (fqrand() * 10.0f) + this->actor.world.pos.z;
                Effect_SS_G_Ripple_ct2(play, &ripplePos, 100, 500, 0);

                if ((this->speedXZ > 4.0f) && !swim_check(this) &&
                    ((this->actor.world.pos.y + this->actor.depthInWater) <
                     this->bodyPartsPos[PLAYER_BODYPART_WAIST].y)) {
                    effect_splash2_set(play, this, 20.0f,
                                  (fabsf(this->speedXZ) * 50.0f) + (this->actor.depthInWater * 5.0f));
                }
            }
        }

        if (this->actor.depthInWater > 40.0f) {
            s32 numBubbles = 0;
            s32 i;

            if ((this->actor.velocity.y > -1.0f) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                if (fqrand() < 0.2f) {
                    numBubbles = 1;
                }
            } else {
                numBubbles = this->actor.velocity.y * -2.0f;
            }

            for (i = 0; i < numBubbles; i++) {
                Effect_SS_Bubble_ct(play, &this->actor.world.pos, 20.0f, 10.0f, 20.0f, 0.13f);
            }
        }
    }
}

s32 anchor_eye_move(Player* this, s32 arg1) {
    Actor* focusActor = this->focusActor;
    Vec3f playerHeadPos;
    s16 targetFocusRotX;
    s16 targetFocusRotY;

    playerHeadPos.x = this->actor.world.pos.x;
    playerHeadPos.y = this->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
    playerHeadPos.z = this->actor.world.pos.z;

    targetFocusRotX = search_position_angleX(&playerHeadPos, &focusActor->focus.pos);
    targetFocusRotY = search_position_angleY(&playerHeadPos, &focusActor->focus.pos);

    add_calc_short_angle2(&this->actor.focus.rot.y, targetFocusRotY, 4, 10000, 0);
    add_calc_short_angle2(&this->actor.focus.rot.x, targetFocusRotX, 4, 10000, 0);

    this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_Y;

    return eye_move_anime_set(this, arg1);
}

static Vec3f look_pos_offset = { 0.0f, 100.0f, 40.0f };

void BGcheck_look_angle_move(Player* this, PlayState* play) {
    s16 sp46;
    s16 temp2;
    f32 temp1;
    Vec3f sp34;

    if (this->focusActor != NULL) {
        if (player_bow_ready_check(this) || boom_ready_check(this)) {
            anchor_eye_move(this, true);
        } else {
            anchor_eye_move(this, false);
        }
        return;
    }

    if (ground_attribute == FLOOR_TYPE_11) {
        add_calc_short_angle2(&this->actor.focus.rot.x, -20000, 10, 4000, 800);
    } else {
        sp46 = 0;
        temp1 = player_ground_BGcheck(play, this, &look_pos_offset, &sp34);
        if (temp1 > BGCHECK_Y_MIN) {
            temp2 = atans_table(40.0f, this->actor.world.pos.y - temp1);
            sp46 = CLAMP(temp2, -4000, 4000);
        }
        this->actor.focus.rot.y = this->actor.shape.rot.y;
        add_calc_short_angle2(&this->actor.focus.rot.x, sp46, 14, 4000, 30);
    }

    eye_move_anime_set(this, player_bow_ready_check(this) || boom_ready_check(this));
}

void run_joint_control(Player* this, PlayState* play) {
    if (!player_bow_ready_check(this) && !boom_ready_check(this) && (this->speedXZ > 5.0f)) {
        s16 targetPitch;
        s16 targetRoll;

        targetPitch = this->speedXZ * 200.0f;
        targetRoll = (s16)(this->yaw - this->actor.shape.rot.y) * this->speedXZ * 0.1f;

        targetPitch = CLAMP(targetPitch, -4000, 4000);
        targetRoll = CLAMP(-targetRoll, -4000, 4000);

        chase_angle(&this->upperLimbRot.x, targetPitch, 900);
        this->headLimbRot.x = -(f32)this->upperLimbRot.x * 0.5f;

        chase_angle(&this->headLimbRot.z, targetRoll, 300);
        chase_angle(&this->upperLimbRot.z, targetRoll, 200);

        this->unk_6AE_rotFlags |= UNK6AE_ROT_HEAD_X | UNK6AE_ROT_HEAD_Z | UNK6AE_ROT_UPPER_X | UNK6AE_ROT_UPPER_Z;
    } else {
        BGcheck_look_angle_move(this, play);
    }
}

void run_control(Player* this, f32 arg1, s16 arg2) {
    chase_f3(&this->speedXZ, arg1, REG(19) / 100.0f, 1.5f);
    chase_angle(&this->yaw, arg2, REG(27));
}

void sky_control(Player* this, f32* arg1, s16* arg2) {
    s16 yawDiff = this->yaw - *arg2;

    if (this->meleeWeaponState == 0) {
        this->speedXZ = CLAMP(this->speedXZ, -(R_RUN_SPEED_LIMIT / 100.0f), (R_RUN_SPEED_LIMIT / 100.0f));
    }

    if (ABS(yawDiff) > 0x6000) {
        if (chase_f(&this->speedXZ, 0.0f, 1.0f)) {
            this->yaw = *arg2;
        }
    } else {
        chase_f3(&this->speedXZ, *arg1, 0.05f, 0.1f);
        chase_angle(&this->yaw, *arg2, 200);
    }
}

static struct_80854578 ride_horse_status[] = {
    { &gPlayerAnim_link_uma_left_up, 35.17f, 6.6099997f },
    { &gPlayerAnim_link_uma_right_up, -34.16f, 7.91f },
};

s32 to_ride_check(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;

    if ((rideActor != NULL) && CHECK_BTN_ALL(pad->press.button, BTN_A)) {
        f32 unk_04;
        f32 unk_08;
        f32 sp38;
        f32 sp34;
        s32 temp;

        sp38 = cos_s(rideActor->actor.shape.rot.y);
        sp34 = sin_s(rideActor->actor.shape.rot.y);

        Player_actor_set_demo_init_proc(play, this, demo_init_ride_horse);

        this->stateFlags1 |= PLAYER_STATE1_23;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER;

        if (this->mountSide < 0) {
            temp = 0;
        } else {
            temp = 1;
        }

        unk_04 = ride_horse_status[temp].unk_04;
        unk_08 = ride_horse_status[temp].unk_08;

        this->actor.world.pos.x =
            rideActor->actor.world.pos.x + rideActor->riderPos.x + ((unk_04 * sp38) + (unk_08 * sp34));
        this->actor.world.pos.z =
            rideActor->actor.world.pos.z + rideActor->riderPos.z + ((unk_08 * sp38) - (unk_04 * sp34));

        this->unk_878 = rideActor->actor.world.pos.y - this->actor.world.pos.y;
        this->yaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

        player_ride_horse_init(play, this, &rideActor->actor);
        anime_init_standard_stop(play, this, ride_horse_status[temp].anim);
        anime_move_init(play, this,
                                 ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_ENABLE_MOVEMENT |
                                     ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);

        this->actor.parent = this->rideActor;
        speedF_look_mode_clear(this);
        magic_grass_cancel(play);

        return 1;
    }

    return 0;
}

void ground_status_check(CollisionPoly* floorPoly, Vec3f* slopeNormal, s16* downwardSlopeYaw) {
    slopeNormal->x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
    slopeNormal->y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
    slopeNormal->z = COLPOLY_GET_NORMAL(floorPoly->normal.z);

    *downwardSlopeYaw = atans_table(slopeNormal->z, slopeNormal->x);
}

s32 to_slope_slip_check(PlayState* play, Player* this, CollisionPoly* floorPoly) {
    static LinkAnimationHeader* slope_slip_anm[] = {
        &gPlayerAnim_link_normal_down_slope_slip,
        &gPlayerAnim_link_normal_up_slope_slip,
    };
    s32 padding;
    s16 playerVelYaw;
    Vec3f slopeNormal;
    s16 downwardSlopeYaw;
    f32 slopeSlowdownSpeed;
    f32 slopeSlowdownSpeedStep;
    s16 velYawToDownwardSlope;

    if (!player_action_check(play, this) && (move_slope_slip != this->actionFunc) &&
        (T_BGCheck_getFriction_ai(&play->colCtx, floorPoly, this->actor.floorBgId) == FLOOR_EFFECT_1)) {
        // Get direction of movement relative to the downward direction of the slope
        playerVelYaw = atans_table(this->actor.velocity.z, this->actor.velocity.x);
        ground_status_check(floorPoly, &slopeNormal, &downwardSlopeYaw);
        velYawToDownwardSlope = downwardSlopeYaw - playerVelYaw;

        if (ABS(velYawToDownwardSlope) > 0x3E80) { // 87.9 degrees
            // moving parallel or upwards on the slope, player does not slip but does slow down
            slopeSlowdownSpeed = (1.0f - slopeNormal.y) * 40.0f;
            slopeSlowdownSpeedStep = SQ(slopeSlowdownSpeed) * 0.015f;

            if (slopeSlowdownSpeedStep < 1.2f) {
                slopeSlowdownSpeedStep = 1.2f;
            }

            // slows down speed as player is climbing a slope
            this->pushedYaw = downwardSlopeYaw;
            chase_f(&this->pushedSpeed, slopeSlowdownSpeed, slopeSlowdownSpeedStep);
        } else {
            // moving downward on the slope, causing player to slip and then slide down
            Player_actor_set_process(play, this, move_slope_slip, 0);
            action_reset_all(play, this);

            if (ground_shape_angle_x >= 0) {
                this->av1.facingUpSlope = true;
            }
            anime_init_repeat_basic(play, this, slope_slip_anm[this->av1.facingUpSlope]);
            this->speedXZ = sqrtf(SQ(this->actor.velocity.x) + SQ(this->actor.velocity.z));
            this->yaw = playerVelYaw;
            return true;
        }
    }

    return false;
}

// unknown data (unused)
static s32 tunnel_start_base_translate[] = {
    0xFFDB0871, 0xF8310000, 0x00940470, 0xF3980000, 0xFFB504A9, 0x0C9F0000, 0x08010402,
};

void get_item_set(PlayState* play, Player* this, GetItemEntry* giEntry) {
    s32 dropType = giEntry->field & 0x1F;

    if (!(giEntry->field & 0x80)) {
        Item_set0(play, &this->actor.world.pos, dropType | 0x8000);
        if ((dropType != ITEM00_BOMBS_A) && (dropType != ITEM00_ARROWS_SMALL) && (dropType != ITEM00_ARROWS_MEDIUM) &&
            (dropType != ITEM00_ARROWS_LARGE) && (dropType != ITEM00_RUPEE_GREEN) && (dropType != ITEM00_RUPEE_BLUE) &&
            (dropType != ITEM00_RUPEE_RED) && (dropType != ITEM00_RUPEE_PURPLE) && (dropType != ITEM00_RUPEE_ORANGE)) {
            item_get_setting(play, giEntry->itemId);
        }
    } else {
        item_get_setting(play, giEntry->itemId);
    }

    Na_StartSystemSe_F((this->getItemId < 0) ? NA_SE_SY_GET_BOXITEM : NA_SE_SY_GET_ITEM);
}

#if DEBUG_FEATURES
#define DEBUG_iREG_67 iREG(67)
#else
#define DEBUG_iREG_67 0
#endif

s32 to_carry_check(Player* this, PlayState* play) {
    Actor* interactedActor;

    if (DEBUG_iREG_67 ||
        (((interactedActor = this->interactRangeActor) != NULL) && Actor_Name_Disp_Cancel(play, &play->actorCtx.titleCtx))) {
        if (DEBUG_iREG_67 || (this->getItemId > GI_NONE)) {
            if (DEBUG_iREG_67) {
                this->getItemId = iREG(68);
            }

            if (this->getItemId < GI_MAX) {
                GetItemEntry* giEntry = &get_item_status[this->getItemId - 1];

#if DEBUG_FEATURES
                if ((interactedActor != &this->actor) && !iREG(67)) {
                    interactedActor->parent = &this->actor;
                }
                iREG(67) = false;
#else
                interactedActor->parent = &this->actor;
#endif

                if ((item_get_non_setting(giEntry->itemId) == ITEM_NONE) ||
                    (play->sceneId == SCENE_BOMBCHU_BOWLING_ALLEY)) {
                    l_hand_child_cancel(play, this);
                    to_get_item(this, giEntry->objectId);

                    if (!(this->stateFlags2 & PLAYER_STATE2_10) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
                        Player_actor_set_demo_init_proc(play, this, demo_init_get_item);
                        anime_init_standard_stop_3f(play, this, &gPlayerAnim_link_demo_get_itemB);
                        change_item_camera_check_set(play, 9);
                    }

                    this->stateFlags1 |= PLAYER_STATE1_10 | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_29;
                    speedF_look_mode_clear(this);
                    return 1;
                }

                get_item_set(play, this, giEntry);
                this->getItemId = GI_NONE;
            }
        } else if (CHECK_BTN_ALL(pad->press.button, BTN_A) &&
                   !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && !(this->stateFlags2 & PLAYER_STATE2_10)) {
            if (this->getItemId != GI_NONE) {
                GetItemEntry* giEntry = &get_item_status[-this->getItemId - 1];
                EnBox* chest = (EnBox*)interactedActor;

                if (giEntry->itemId != ITEM_NONE) {
                    if (((item_get_non_setting(giEntry->itemId) == ITEM_NONE) && (giEntry->field & 0x40)) ||
                        ((item_get_non_setting(giEntry->itemId) != ITEM_NONE) && (giEntry->field & 0x20))) {
                        this->getItemId = -GI_RUPEE_BLUE;
                        giEntry = &get_item_status[GI_RUPEE_BLUE - 1];
                    }
                }

                Player_actor_set_demo_init_proc(play, this, demo_init_get_item);
                this->stateFlags1 |= PLAYER_STATE1_10 | PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_29;
                to_get_item(this, giEntry->objectId);
                this->actor.world.pos.x =
                    chest->dyna.actor.world.pos.x - (sin_s(chest->dyna.actor.shape.rot.y) * 29.4343f);
                this->actor.world.pos.z =
                    chest->dyna.actor.world.pos.z - (cos_s(chest->dyna.actor.shape.rot.y) * 29.4343f);
                this->yaw = this->actor.shape.rot.y = chest->dyna.actor.shape.rot.y;
                speedF_look_mode_clear(this);

                if ((giEntry->itemId != ITEM_NONE) && (giEntry->gi >= 0) &&
                    (item_get_non_setting(giEntry->itemId) == ITEM_NONE)) {
                    anime_init_standard_stop_3f(play, this, this->ageProperties->unk_98);
                    anime_move_init(play, this,
                                             PLAYER_ANIM_MOVEMENT_RESET_BY_AGE | ANIM_FLAG_UPDATE_XZ |
                                                 ANIM_FLAG_UPDATE_Y | ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT |
                                                 ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_OVERRIDE_MOVEMENT);
                    chest->unk_1F4 = 1;
                    changeCameraSet(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_SET_SLOW_CHEST_CS);
                } else {
                    anime_init_standard_stop(play, this, &gPlayerAnim_link_normal_box_kick);
                    chest->unk_1F4 = -1;
                }

                return 1;
            }

            if ((this->heldActor == NULL) || hook_check(this)) {
                if ((interactedActor->id == ACTOR_BG_TOKI_SWD) && LINK_IS_ADULT) {
                    s32 sp24 = this->itemAction;

                    this->itemAction = PLAYER_IA_NONE;
                    this->modelAnimType = PLAYER_ANIMTYPE_0;
                    this->heldItemAction = this->itemAction;
                    Player_actor_set_demo_init_proc(play, this, demo_init_carry);

                    if (sp24 == PLAYER_IA_SWORD_MASTER) {
                        this->nextModelGroup = item_shape_type_set(this, PLAYER_IA_SWORD_CS);
                        ability_A_item_change(play, this, PLAYER_IA_SWORD_CS);
                    } else {
                        player_item_on(play, this, ITEM_SWORD_CS);
                    }
                } else {
                    s32 strength = player_grove_check();

                    if ((interactedActor->id == ACTOR_EN_ISHI) && (PARAMS_GET_U(interactedActor->params, 0, 4) == 1) &&
                        (strength < PLAYER_STR_SILVER_G)) {
                        return 0;
                    }

                    Player_actor_set_demo_init_proc(play, this, demo_init_carry);
                }

                speedF_look_mode_clear(this);
                this->stateFlags1 |= PLAYER_STATE1_CARRYING_ACTOR;
                return 1;
            }
        }
    }

    return 0;
}

void to_throw_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_throw, 1);
    anime_init_standard_stop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_throw, this->modelAnimType));
}

/**
 * Checks if an actor can be thrown or dropped.
 * It is assumed that the `actor` argument is the actor currently being carried.
 *
 * @return true if it can be thrown, false if it can be dropped.
 */
s32 to_put_or_throw_check(Player* this, Actor* actor) {
    // If the actor arg is null, true will be returned.
    // It doesn't make sense for a non-existent actor to be thrown or dropped, so
    // the safety check should happen before this function is even called.
    if ((actor != NULL) && !(actor->flags & ACTOR_FLAG_THROW_ONLY) &&
        ((this->speedXZ < 1.1f) || (actor->id == ACTOR_EN_BOM_CHU))) {
        return false;
    }

    return true;
}

s32 to_throw_check(Player* this, PlayState* play) {
    if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->heldActor != NULL) &&
        CHECK_BTN_ANY(pad->press.button, BTN_A | BTN_B | BTN_CLEFT | BTN_CDOWN | BTN_CRIGHT)) {
        if (!carry_cancel_check(play, this, this->heldActor)) {
            if (!to_put_or_throw_check(this, this->heldActor)) {
                Player_actor_set_process(play, this, move_put, 1);
                anime_init_standard_stop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_put, this->modelAnimType));
            } else {
                to_throw_set(this, play);
            }
        }
        return 1;
    }

    return 0;
}

s32 to_climb_check(Player* this, PlayState* play, u32 wallFlags) {
    if (this->yDistToLedge >= 79.0f) {
        if (!(this->stateFlags1 & PLAYER_STATE1_27) || (this->currentBoots == PLAYER_BOOTS_IRON) ||
            (this->actor.depthInWater < this->ageProperties->unk_2C)) {
            s32 sp8C = (wallFlags & WALL_FLAG_3) ? 2 : 0;

            if ((sp8C != 0) || (wallFlags & WALL_FLAG_1) ||
                T_BGCheck_WallLadderUp_ai(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId)) {
                f32 phi_f20;
                CollisionPoly* wallPoly = this->actor.wallPoly;
                f32 sp80;
                f32 sp7C;
                f32 phi_f12;
                f32 phi_f14;

                phi_f20 = phi_f12 = 0.0f;

                if (sp8C != 0) {
                    sp80 = this->actor.world.pos.x;
                    sp7C = this->actor.world.pos.z;
                } else {
                    Vec3f sp50[3];
                    s32 i;
                    f32 sp48;
                    Vec3f* sp44 = &sp50[0];
                    s32 padding;

                    T_Polygon_GetVertex_bg_ai(wallPoly, this->actor.wallBgId, &play->colCtx, sp50);

                    sp80 = phi_f12 = sp44->x;
                    sp7C = phi_f14 = sp44->z;
                    phi_f20 = sp44->y;
                    for (i = 1; i < 3; i++) {
                        sp44++;
                        if (sp80 > sp44->x) {
                            sp80 = sp44->x;
                        } else if (phi_f12 < sp44->x) {
                            phi_f12 = sp44->x;
                        }

                        if (sp7C > sp44->z) {
                            sp7C = sp44->z;
                        } else if (phi_f14 < sp44->z) {
                            phi_f14 = sp44->z;
                        }

                        if (phi_f20 > sp44->y) {
                            phi_f20 = sp44->y;
                        }
                    }

                    sp80 = (sp80 + phi_f12) * 0.5f;
                    sp7C = (sp7C + phi_f14) * 0.5f;

                    phi_f12 = ((this->actor.world.pos.x - sp80) * COLPOLY_GET_NORMAL(wallPoly->normal.z)) -
                              ((this->actor.world.pos.z - sp7C) * COLPOLY_GET_NORMAL(wallPoly->normal.x));
                    sp48 = this->actor.world.pos.y - phi_f20;

                    phi_f20 = ((f32)(s32)((sp48 / 15.000000223517418) + 0.5) * 15.000000223517418) - sp48;
                    phi_f12 = fabsf(phi_f12);
                }

                if (phi_f12 < 8.0f) {
                    f32 wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                    f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                    f32 sp34 = this->distToInteractWall;
                    LinkAnimationHeader* anim;

                    Player_actor_set_demo_init_proc(play, this, demo_init_climb);
                    this->stateFlags1 |= PLAYER_STATE1_21;
                    this->stateFlags1 &= ~PLAYER_STATE1_27;

                    if ((sp8C != 0) || (wallFlags & WALL_FLAG_1)) {
                        if ((this->av1.actionVar1 = sp8C) != 0) {
                            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                                anim = &gPlayerAnim_link_normal_Fclimb_startA;
                            } else {
                                anim = &gPlayerAnim_link_normal_Fclimb_hold2upL;
                            }
                            sp34 = (this->ageProperties->wallCheckRadius - 1.0f) - sp34;
                        } else {
                            anim = this->ageProperties->unk_A4;
                            sp34 = sp34 - 1.0f;
                        }
                        this->av2.actionVar2 = -2;
                        this->actor.world.pos.y += phi_f20;
                        this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
                    } else {
                        anim = this->ageProperties->unk_A8;
                        this->av2.actionVar2 = -4;
                        this->actor.shape.rot.y = this->yaw = this->actor.wallYaw;
                    }

                    this->actor.world.pos.x = (sp34 * wallPolyNormalX) + sp80;
                    this->actor.world.pos.z = (sp34 * wallPolyNormalZ) + sp7C;
                    speedF_look_mode_clear(this);
                    xyz_t_move(&this->actor.prevPos, &this->actor.world.pos);
                    anime_init_standard_stop(play, this, anim);
                    anime_move_init(play, this,
                                             ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y |
                                                 ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                                 ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);

                    return true;
                }
            }
        }
    }

    return false;
}

void to_climb_end_set(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    Player_actor_anime_move_save_set_process(play, this, move_climb_end, 0);
    Skeleton_Info_Rom_init_standard_speedset_stop(play, &this->skelAnime, anim, (4.0f / 3.0f));
}

/**
 * @return true if Player chooses to enter crawlspace
 */
s32 to_tunnel_check(Player* this, PlayState* play, u32 interactWallFlags) {
    if (!LINK_IS_ADULT && !(this->stateFlags1 & PLAYER_STATE1_27) && (interactWallFlags & WALL_FLAG_CRAWLSPACE)) {
        CollisionPoly* wallPoly;
        Vec3f wallVertices[3];
        f32 xVertex1;
        f32 xVertex2;
        f32 zVertex1;
        f32 zVertex2;
        s32 i;

        wallPoly = this->actor.wallPoly;
        T_Polygon_GetVertex_bg_ai(wallPoly, this->actor.wallBgId, &play->colCtx, wallVertices);

        // Determines min and max vertices for x & z (edges of the crawlspace hole)
        xVertex1 = xVertex2 = wallVertices[0].x;
        zVertex1 = zVertex2 = wallVertices[0].z;
        for (i = 1; i < 3; i++) {
            if (xVertex1 > wallVertices[i].x) {
                // Update x min
                xVertex1 = wallVertices[i].x;
            } else if (xVertex2 < wallVertices[i].x) {
                // Update x max
                xVertex2 = wallVertices[i].x;
            }
            if (zVertex1 > wallVertices[i].z) {
                // Update z min
                zVertex1 = wallVertices[i].z;
            } else if (zVertex2 < wallVertices[i].z) {
                // Update z max
                zVertex2 = wallVertices[i].z;
            }
        }

        // XZ Center of the crawlspace hole
        xVertex1 = (xVertex1 + xVertex2) * 0.5f;
        zVertex1 = (zVertex1 + zVertex2) * 0.5f;

        // Perpendicular (sideways) XZ-Distance from player pos to crawlspace line
        // Uses y-component of crossproduct formula for the distance from a point to a line
        xVertex2 = ((this->actor.world.pos.x - xVertex1) * COLPOLY_GET_NORMAL(wallPoly->normal.z)) -
                   ((this->actor.world.pos.z - zVertex1) * COLPOLY_GET_NORMAL(wallPoly->normal.x));

        if (fabsf(xVertex2) < 8.0f) {
            // Give do-action prompt to "Enter on A" for the crawlspace
            this->stateFlags2 |= PLAYER_STATE2_DO_ACTION_ENTER;

            if (CHECK_BTN_ALL(pad->press.button, BTN_A)) {
                // Enter Crawlspace
                f32 wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 distToInteractWall = this->distToInteractWall;

                Player_actor_set_demo_init_proc(play, this, demo_init_tunnel);
                this->stateFlags2 |= PLAYER_STATE2_CRAWLING;
                this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
                this->actor.world.pos.x = xVertex1 + (distToInteractWall * wallPolyNormalX);
                this->actor.world.pos.z = zVertex1 + (distToInteractWall * wallPolyNormalZ);
                speedF_look_mode_clear(this);
                this->actor.prevPos = this->actor.world.pos;
                anime_init_standard_stop(play, this, &gPlayerAnim_link_child_tunnel_start);
                anime_move_init(play, this,
                                         ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT |
                                             ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_ADJUST_STARTING_POS |
                                             ANIM_FLAG_OVERRIDE_MOVEMENT);

                return true;
            }
        }
    }

    return false;
}

s32 wall_angle_check2(PlayState* play, Player* this, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    CollisionPoly* wallPoly;
    s32 wallBgId;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;
    f32 yawCos;
    f32 yawSin;
    s32 temp;
    f32 wallPolyNormalX;
    f32 wallPolyNormalZ;

    yawCos = cos_s(this->actor.shape.rot.y);
    yawSin = sin_s(this->actor.shape.rot.y);

    sp6C.x = this->actor.world.pos.x + (arg4 * yawSin);
    sp6C.z = this->actor.world.pos.z + (arg4 * yawCos);
    sp60.x = this->actor.world.pos.x + (arg3 * yawSin);
    sp60.z = this->actor.world.pos.z + (arg3 * yawCos);
    sp60.y = sp6C.y = this->actor.world.pos.y + arg1;

    if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &sp6C, &sp60, &sp54, &this->actor.wallPoly, true, false, false, true,
                                &wallBgId)) {
        wallPoly = this->actor.wallPoly;

        this->actor.bgCheckFlags |= BGCHECKFLAG_PLAYER_WALL_INTERACT;
        this->actor.wallBgId = wallBgId;

        wall_code = T_BGCheck_getWallCodeBit_ai(&play->colCtx, wallPoly, wallBgId);

        wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
        wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
        temp = atans_table(-wallPolyNormalZ, -wallPolyNormalX);
        chase_angle(&this->actor.shape.rot.y, temp, 800);

        this->yaw = this->actor.shape.rot.y;
        this->actor.world.pos.x = sp54.x - (sin_s(this->actor.shape.rot.y) * arg2);
        this->actor.world.pos.z = sp54.z - (cos_s(this->actor.shape.rot.y) * arg2);

        return 1;
    }

    this->actor.bgCheckFlags &= ~BGCHECKFLAG_PLAYER_WALL_INTERACT;

    return 0;
}

s32 wall_angle_check(PlayState* play, Player* this) {
    return wall_angle_check2(play, this, 26.0f, this->ageProperties->wallCheckRadius + 5.0f, 30.0f, 0.0f);
}

/**
 * Two exit walls are placed at each end of the crawlspace, separate to the two entrance walls used to enter the
 * crawlspace. These front and back exit walls are further into the crawlspace than the front and
 * back entrance walls. When player interacts with either of these two interior exit walls, start the leaving-crawlspace
 * cutscene and return true. Else, return false
 */
s32 to_tunnel_end_check(Player* this, PlayState* play) {
    s16 yawToWall;

    if ((this->speedXZ != 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
        (wall_code & WALL_FLAG_CRAWLSPACE)) {

        // The exit wallYaws will always point inward on the crawlline
        // Interacting with the exit wall in front will have a yaw diff of 0x8000
        // Interacting with the exit wall behind will have a yaw diff of 0
        yawToWall = this->actor.shape.rot.y - this->actor.wallYaw;
        if (this->speedXZ < 0.0f) {
            yawToWall += 0x8000;
        }

        if (ABS(yawToWall) > 0x4000) {
            Player_actor_set_process(play, this, move_tunnel_end, 0);

            if (this->speedXZ > 0.0f) {
                // Leaving a crawlspace forwards
                this->actor.shape.rot.y = this->actor.wallYaw + 0x8000;
                anime_init_standard_stop(play, this, &gPlayerAnim_link_child_tunnel_end);
                anime_move_init(play, this,
                                         ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT |
                                             ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_ADJUST_STARTING_POS |
                                             ANIM_FLAG_OVERRIDE_MOVEMENT);
                makeOnepointDemo(play, 9601, 999, NULL, CAM_ID_MAIN);
            } else {
                // Leaving a crawlspace backwards
                this->actor.shape.rot.y = this->actor.wallYaw;
                Skeleton_Info_Rom_init(play, &this->skelAnime, &gPlayerAnim_link_child_tunnel_start, -1.0f,
                                     Si2_anime_end_frame(&gPlayerAnim_link_child_tunnel_start), 0.0f, ANIMMODE_ONCE,
                                     0.0f);
                anime_move_init(play, this,
                                         ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT |
                                             ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_ADJUST_STARTING_POS |
                                             ANIM_FLAG_OVERRIDE_MOVEMENT);
                makeOnepointDemo(play, 9602, 999, NULL, CAM_ID_MAIN);
            }

            this->yaw = this->actor.shape.rot.y;
            speedF_clear(this);

            return true;
        }
    }

    return false;
}

void to_push_wait_set(Player* this, LinkAnimationHeader* anim, PlayState* play) {
    if (!Player_actor_set_demo_init_proc(play, this, demo_init_push)) {
        Player_actor_set_process(play, this, move_push_wait, 0);
    }

    anime_init_standard_stop(play, this, anim);
    speedF_look_mode_clear(this);

    this->actor.shape.rot.y = this->yaw = this->actor.wallYaw + 0x8000;
}

s32 to_push_check(Player* this, PlayState* play) {
    DynaPolyActor* wallPolyActor;

    if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) && (wall_distance_angle_y < 0x3000)) {

        if (((this->speedXZ > 0.0f) && to_climb_check(this, play, wall_code)) ||
            to_tunnel_check(this, play, wall_code)) {
            return 1;
        }

        if (!swim_check(this) && ((this->speedXZ == 0.0f) || !(this->stateFlags2 & PLAYER_STATE2_2)) &&
            (wall_code & WALL_FLAG_6) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
            (this->yDistToLedge >= 39.0f)) {

            this->stateFlags2 |= PLAYER_STATE2_0;

            if (CHECK_BTN_ALL(pad->cur.button, BTN_A)) {

                if ((this->actor.wallBgId != BGCHECK_SCENE) &&
                    ((wallPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, this->actor.wallBgId)) != NULL)) {

                    if (wallPolyActor->actor.id == ACTOR_BG_HEAVY_BLOCK) {
                        if (player_grove_check() < PLAYER_STR_GOLD_G) {
                            return 0;
                        }

                        Player_actor_set_demo_init_proc(play, this, demo_init_carry);
                        this->stateFlags1 |= PLAYER_STATE1_CARRYING_ACTOR;
                        this->interactRangeActor = &wallPolyActor->actor;
                        this->getItemId = GI_NONE;
                        this->yaw = this->actor.wallYaw + 0x8000;
                        speedF_look_mode_clear(this);

                        return 1;
                    }

                    this->unk_3C4 = &wallPolyActor->actor;
                } else {
                    this->unk_3C4 = NULL;
                }

                to_push_wait_set(this, &gPlayerAnim_link_normal_push_wait, play);

                return 1;
            }
        }
    }

    return 0;
}

s32 to_push_cancel_check(PlayState* play, Player* this) {
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) &&
        ((this->stateFlags2 & PLAYER_STATE2_4) || CHECK_BTN_ALL(pad->cur.button, BTN_A))) {
        DynaPolyActor* wallPolyActor = NULL;

        if (this->actor.wallBgId != BGCHECK_SCENE) {
            wallPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, this->actor.wallBgId);
        }

        if (&wallPolyActor->actor == this->unk_3C4) {
            if (this->stateFlags2 & PLAYER_STATE2_4) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    to_wait_proc_set(this, play);
    anime_init_standard_stop(play, this, &gPlayerAnim_link_normal_push_wait_end);
    this->stateFlags2 &= ~PLAYER_STATE2_4;
    return 1;
}

void to_pushing_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_pushing, 0);
    this->stateFlags2 |= PLAYER_STATE2_4;
    anime_init_standard_stop(play, this, &gPlayerAnim_link_normal_push_start);
}

void to_pulling_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_pulling, 0);
    this->stateFlags2 |= PLAYER_STATE2_4;
    anime_init_standard_stop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_pull_start, this->modelAnimType));
}

void to_climb_cancel_set(Player* this, PlayState* play) {
    this->stateFlags1 &= ~(PLAYER_STATE1_21 | PLAYER_STATE1_27);
    to_landing_set(this, play);
    this->speedXZ = -0.4f;
}

s32 to_climb_cancel_check(Player* this, PlayState* play) {
    if (!CHECK_BTN_ALL(pad->press.button, BTN_A) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) &&
        ((wall_code & WALL_FLAG_3) || (wall_code & WALL_FLAG_1) ||
         T_BGCheck_WallLadderUp_ai(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId))) {
        return false;
    }

    to_climb_cancel_set(this, play);
    player_voice_SE_set(this, NA_SE_VO_LI_AUTO_JUMP);
    return true;
}

s32 anchor_mode_key_check(Player* this, f32 arg1, s16 arg2) {
    f32 sp1C = (s16)(arg2 - this->actor.shape.rot.y);
    f32 temp;

    if (this->focusActor != NULL) {
        anchor_eye_move(this, player_bow_ready_check(this) || boom_ready_check(this));
    }

    temp = fabsf(sp1C) / 32768.0f;

    if (arg1 > (((temp * temp) * 50.0f) + 6.0f)) {
        return 1;
    } else if (arg1 > (((1.0f - temp) * 10.0f) + 6.8f)) {
        return -1;
    }

    return 0;
}

s32 parallel_mode_key_check(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    s16 sp2E = *arg2 - this->parallelYaw;
    u16 sp2C = ABS(sp2E);

    if ((player_bow_ready_check(this) || boom_ready_check(this)) && (this->focusActor == NULL)) {
        *arg1 *= sin_s(sp2C);

        if (*arg1 != 0.0f) {
            *arg2 = (((sp2E >= 0) ? 1 : -1) << 0xE) + this->actor.shape.rot.y;
        } else {
            *arg2 = this->actor.shape.rot.y;
        }

        if (this->focusActor != NULL) {
            anchor_eye_move(this, true);
        } else {
            add_calc_short_angle2(&this->actor.focus.rot.x, pad->rel.stick_y * 240.0f, 14, 4000, 30);
            eye_move_anime_set(this, true);
        }
    } else {
        if (this->focusActor != NULL) {
            return anchor_mode_key_check(this, *arg1, *arg2);
        } else {
            BGcheck_look_angle_move(this, play);
            if ((*arg1 != 0.0f) && (sp2C < 6000)) {
                return 1;
            } else if (*arg1 > sin_s((0x4000 - (sp2C >> 1))) * 200.0f) {
                return -1;
            }
        }
    }

    return 0;
}

s32 push_mode_key_check(Player* this, f32* arg1, s16* arg2) {
    s16 temp1 = *arg2 - this->actor.shape.rot.y;
    u16 temp2 = ABS(temp1);
    f32 temp3 = cos_s(temp2);

    *arg1 *= temp3;

    if (*arg1 != 0.0f) {
        if (temp3 > 0) {
            return 1;
        } else {
            return -1;
        }
    }

    return 0;
}

s32 power_kiru_mode_key_check(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    BGcheck_look_angle_move(this, play);

    if ((*arg1 != 0.0f) || (ABS(this->unk_87C) > 400)) {
        s16 temp1 = *arg2 - (u16)getCameraAngleY(GET_ACTIVE_CAM(play));
        u16 temp2 = (ABS(temp1) - 0x2000);

        if ((temp2 < 0x4000) || (this->unk_87C != 0)) {
            return -1;
        }

        return 1;
    }

    return 0;
}

void anchor_LR_ratio_check(Player* this, f32 arg1, s16 arg2) {
    s16 temp = arg2 - this->actor.shape.rot.y;

    if (arg1 > 0.0f) {
        if (temp < 0) {
            this->unk_874 = 0.0f;
        } else {
            this->unk_874 = 1.0f;
        }
    }

    chase_f(&this->unk_870, this->unk_874, 0.3f);
}

void anchor_wait_anime_set(PlayState* play, Player* this) {
    Skeleton_Info_Rom_morf_anime_to_now(play, &this->skelAnime, waitR_anime_select(this), this->unk_868, waitL_anime_select(this),
                               this->unk_868, this->unk_870, this->blendTable);
}

static s32 anime_frame_check(f32 arg0, f32 arg1, f32 arg2, f32 arg3) {
    f32 temp;

    if ((arg3 == 0.0f) && (arg1 > 0.0f)) {
        arg3 = arg2;
    }

    temp = (arg0 + arg1) - arg3;

    if (((temp * arg1) >= 0.0f) && (((temp - arg1) * arg1) < 0.0f)) {
        return 1;
    }

    return 0;
}

void run_anime_frame_set(Player* this, f32 arg1) {
    f32 updateScale = R_UPDATE_RATE * 0.5f;

    arg1 *= updateScale;
    if (arg1 < -7.25) {
        arg1 = -7.25;
    } else if (arg1 > 7.25f) {
        arg1 = 7.25f;
    }

    if (1) {}

    if ((this->currentBoots == PLAYER_BOOTS_HOVER) && !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        (this->hoverBootsTimer != 0)) {
        Actor_player_level_SE_set(&this->actor, NA_SE_PL_HOBBERBOOTS_LV - SFX_FLAG);
    } else if (anime_frame_check(this->unk_868, arg1, 29.0f, 10.0f) || anime_frame_check(this->unk_868, arg1, 29.0f, 24.0f)) {
        player_walk_SE_set(this, this->speedXZ);
        if (this->speedXZ > 4.0f) {
            this->stateFlags2 |= PLAYER_STATE2_3;
        }
    }

    this->unk_868 += arg1;

    if (this->unk_868 < 0.0f) {
        this->unk_868 += 29.0f;
    } else if (this->unk_868 >= 29.0f) {
        this->unk_868 -= 29.0f;
    }
}

void move_anchor_wait(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp1;
    u32 temp2;
    s16 temp3;
    s32 temp4;

    if (this->stateFlags3 & PLAYER_STATE3_3) {
        if (sword_check(this) != 0) {
            this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;
        } else {
            this->stateFlags3 &= ~PLAYER_STATE3_3;
        }
    }

    if (this->av2.actionVar2 != 0) {
        if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
            anime_move_reset(this);
            anime_init_standard_repeat(play, this, waitR_anime_select(this));
            this->av2.actionVar2 = 0;
            this->stateFlags3 &= ~PLAYER_STATE3_3;
        }
        anchor_LR_ratio_reset(this);
    } else {
        anchor_wait_anime_set(play, this);
    }

    ground_breaking_speedF(this);

    if (!action_select_check(play, this, anchor_wait_action_check, true)) {
        if (!anchor_mode_check_set(this) &&
            (!parallel_mode_check(this) || (uperbody_defense != this->upperActionFunc))) {
            to_wait_run_set(this, play);
            return;
        }

        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        temp1 = anchor_mode_key_check(this, speedTarget, yawTarget);

        if (temp1 > 0) {
            to_run_set2(this, play, yawTarget);
            return;
        }

        if (temp1 < 0) {
            to_anchor_back_walk_set(this, yawTarget, play);
            return;
        }

        if (speedTarget > 4.0f) {
            to_anchor_side_walk_set(this, play);
            return;
        }

        run_anime_frame_set(this, (this->speedXZ * 0.3f) + 1.0f);
        anchor_LR_ratio_check(this, speedTarget, yawTarget);

        temp2 = this->unk_868;
        if ((temp2 < 6) || ((temp2 - 0xE) < 6)) {
            chase_f(&this->speedXZ, 0.0f, 1.5f);
            return;
        }

        temp3 = yawTarget - this->yaw;
        temp4 = ABS(temp3);

        if (temp4 > 0x4000) {
            if (chase_f(&this->speedXZ, 0.0f, 1.5f)) {
                this->yaw = yawTarget;
            }
            return;
        }

        chase_f3(&this->speedXZ, speedTarget * 0.3f, 2.0f, 1.5f);

        if (!(this->stateFlags3 & PLAYER_STATE3_3)) {
            chase_angle(&this->yaw, yawTarget, temp4 * 0.1f);
        }
    }
}

void move_parallel_wait(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp1;
    s16 temp2;
    s32 temp3;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_move_reset(this);
        anime_init_standard_stop(play, this, wait_anime_select(this));
    }

    ground_breaking_speedF(this);

    if (!action_select_check(play, this, parallel_wait_action_check, true)) {
        if (anchor_mode_check_set(this)) {
            to_anchor_wait_from_wait_set(this, play);
            return;
        }

        if (!parallel_mode_check(this)) {
            Player_actor_anime_move_save_set_process(play, this, move_wait, 1);
            this->yaw = this->actor.shape.rot.y;
            return;
        }

        if (uperbody_defense == this->upperActionFunc) {
            to_anchor_wait_from_wait_set(this, play);
            return;
        }

        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        temp1 = parallel_mode_key_check(this, &speedTarget, &yawTarget, play);

        if (temp1 > 0) {
            to_run_set2(this, play, yawTarget);
            return;
        }

        if (temp1 < 0) {
            to_parallel_back_run_set(this, yawTarget, play);
            return;
        }

        if (speedTarget > 4.9f) {
            to_anchor_side_walk_set(this, play);
            anchor_LR_ratio_reset(this);
            return;
        }
        if (speedTarget != 0.0f) {
            to_parallel_side_walk_set(this, play);
            return;
        }

        temp2 = yawTarget - this->actor.shape.rot.y;
        temp3 = ABS(temp2);

        if (temp3 > 800) {
            to_turn_set(play, this, yawTarget);
        }
    }
}

void wait_anime_check_set(PlayState* play, Player* this) {
    LinkAnimationHeader* anim;
    LinkAnimationHeader** fidgetAnimPtr;
    s32 heathIsCritical;
    s32 fidgetType;
    s32 commonType;

    if ((this->focusActor != NULL) ||
        (!(heathIsCritical = Life_Caution_Check()) && ((this->idleType = (this->idleType + 1) & 1) != 0))) {
        this->stateFlags2 &= ~PLAYER_STATE2_IDLE_FIDGET;
        anim = wait_anime_select(this);
    } else {
        this->stateFlags2 |= PLAYER_STATE2_IDLE_FIDGET;

        if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
            // Default idle animation will play if carrying an actor.
            // Note that in this case, `PLAYER_STATE2_IDLE_FIDGET` is still set even though the
            // animation that plays isn't a fidget animation.
            anim = wait_anime_select(this);
        } else {
            // Pick fidget type based on room behavior.
            // This may be changed below.
            fidgetType = play->roomCtx.curRoom.environmentType;

            if (heathIsCritical) {
                if (this->idleType >= PLAYER_IDLE_DEFAULT) {
                    fidgetType = FIDGET_CRIT_HEALTH_START;

                    // When health is critical, `idleType` will not be updated.
                    // It will stay as `PLAYER_IDLE_CRIT_HEALTH` until health is no longer critical.
                    this->idleType = PLAYER_IDLE_CRIT_HEALTH;
                } else {
                    // Keep looping the critical health animation until critical health ends
                    fidgetType = FIDGET_CRIT_HEALTH_LOOP;
                }
            } else {
                commonType = fqrand() * 5;

                // There is a 4/5 chance that a common fidget type will be considered.
                // However it may get rejected by the conditions below.
                // The type determined by `curRoom.environmentType` will be used if a common type is
                // rejected.
                if (commonType < 4) {
                    // `FIDGET_ADJUST_TUNIC` and `FIDGET_TAP_FEET` are accepted unconditionally.
                    // The sword and shield related common types have extra restrictions.
                    //
                    // Note that `FIDGET_SWORD_SWING` is the first common fidget type, which is why
                    // all operations are done relative to this type.
                    if (((commonType + FIDGET_SWORD_SWING != FIDGET_SWORD_SWING) &&
                         (commonType + FIDGET_SWORD_SWING != FIDGET_ADJUST_SHIELD)) ||
                        ((this->rightHandType == PLAYER_MODELTYPE_RH_SHIELD) &&
                         ((commonType + FIDGET_SWORD_SWING == FIDGET_ADJUST_SHIELD) ||
                          (sword_check2(this) != 0)))) {
                        //! @bug It is possible for `FIDGET_ADJUST_SHIELD` to be used even if
                        //! a shield is not currently equipped. This is because of how being shieldless
                        //! is implemented. There is no sword-only model type, only
                        //! `PLAYER_MODELGROUP_SWORD_AND_SHIELD` exists. Therefore, the right hand type will be
                        //! `PLAYER_MODELTYPE_RH_SHIELD` if sword is in hand, even if no shield is equipped.
                        if ((commonType + FIDGET_SWORD_SWING == FIDGET_SWORD_SWING) &&
                            longsword_check(this)) {
                            //! @bug This code is unreachable.
                            //! The check above groups the `sword_check2` check and
                            //! `PLAYER_MODELTYPE_RH_SHIELD` conditions together, meaning sword and shield must be
                            //! in hand. However shield is not in hand when using a two handed melee weapon.
                            commonType = FIDGET_SWORD_SWING_TWO_HAND - FIDGET_SWORD_SWING;
                        }

                        fidgetType = FIDGET_SWORD_SWING + commonType;
                    }
                }
            }

            fidgetAnimPtr = &weather_wait_anm_data[fidgetType][0];

            if (this->modelAnimType != PLAYER_ANIMTYPE_1) {
                fidgetAnimPtr = &weather_wait_anm_data[fidgetType][1];
            }

            anim = *fidgetAnimPtr;
        }
    }

    Skeleton_Info_Rom_init(play, &this->skelAnime, anim, (2.0f / 3.0f) * environment_speed_ratio, 0.0f,
                         Si2_anime_end_frame(anim), ANIMMODE_ONCE, -6.0f);
}

static void move_wait(Player* this, PlayState* play) {
    s32 idleAnimResult = wait_anime_check(this);
    s32 animDone = Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    f32 speedTarget;
    s16 yawTarget;
    s16 yawDiff;

    if (idleAnimResult > IDLE_ANIM_NONE) {
        wait_anime_SE_set(this, idleAnimResult - 1);
    }

    if (animDone) {
        if (this->av2.fallDamageStunTimer != 0) {
            if (DECR(this->av2.fallDamageStunTimer) == 0) {
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }

            // Offset model y position.
            // Depending on if the timer is even or odd, the offset will be 40 or -40 model space units.
            this->skelAnime.jointTable[0].y =
                (this->skelAnime.jointTable[0].y + ((this->av2.fallDamageStunTimer & 1) * 80)) - 40;
        } else {
            anime_move_reset(this);
            wait_anime_check_set(play, this);
        }
    }

    ground_breaking_speedF(this);

    if (this->av2.fallDamageStunTimer == 0) {
        if (!action_select_check(play, this, wait_action_check, true)) {
            if (anchor_mode_check_set(this)) {
                to_anchor_wait_from_wait_set(this, play);
                return;
            }

            if (parallel_mode_check(this)) {
                to_parallel_wait_set(this, play);
                return;
            }

            stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);

            if (speedTarget != 0.0f) {
                to_run_set2(this, play, yawTarget);
                return;
            }

            yawDiff = yawTarget - this->actor.shape.rot.y;

            if (ABS(yawDiff) > 800) {
                to_turn_set(play, this, yawTarget);
                return;
            }

            chase_angle(&this->actor.shape.rot.y, yawTarget, 1200);
            this->yaw = this->actor.shape.rot.y;

            if (wait_anime_select(this) == this->skelAnime.animation) {
                BGcheck_look_angle_move(this, play);
            }
        }
    }
}

void move_parallel_side_walk(Player* this, PlayState* play) {
    f32 frames;
    f32 coeff;
    f32 speedTarget;
    s16 yawTarget;
    s32 temp1;
    s16 temp2;
    s32 temp3;
    s32 direction;

    this->skelAnime.mode = 0;
    Skeleton_Info_Rom_anime_play_proc_set(&this->skelAnime);

    this->skelAnime.animation = side_walk_anime_select(this);

    if (this->skelAnime.animation == &gPlayerAnim_link_bow_side_walk) {
        frames = 24.0f;
        coeff = -(MREG(95) / 100.0f);
    } else {
        frames = 29.0f;
        coeff = MREG(95) / 100.0f;
    }

    this->skelAnime.animLength = frames;
    this->skelAnime.endFrame = frames - 1.0f;

    if ((s16)(this->yaw - this->actor.shape.rot.y) >= 0) {
        direction = 1;
    } else {
        direction = -1;
    }

    this->skelAnime.playSpeed = direction * (this->speedXZ * coeff);

    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_Rom_frame_check(&this->skelAnime, frames * 0.5f)) {
        player_walk_SE_set(this, this->speedXZ);
    }

    if (!action_select_check(play, this, parallel_side_walk_action_check, true)) {
        if (anchor_mode_check_set(this)) {
            to_anchor_wait_from_wait_set(this, play);
            return;
        }

        if (!parallel_mode_check(this)) {
            Ext_to_wait_set(this, play);
            return;
        }

        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        temp1 = parallel_mode_key_check(this, &speedTarget, &yawTarget, play);

        if (temp1 > 0) {
            to_run_set2(this, play, yawTarget);
            return;
        }

        if (temp1 < 0) {
            to_parallel_back_run_set(this, yawTarget, play);
            return;
        }

        if (speedTarget > 4.9f) {
            to_anchor_side_walk_set(this, play);
            anchor_LR_ratio_reset(this);
            return;
        }

        if ((speedTarget == 0.0f) && (this->speedXZ == 0.0f)) {
            to_parallel_wait_set(this, play);
            return;
        }

        temp2 = yawTarget - this->yaw;
        temp3 = ABS(temp2);

        if (temp3 > 0x4000) {
            if (chase_f(&this->speedXZ, 0.0f, 1.5f)) {
                this->yaw = yawTarget;
            }
            return;
        }

        chase_f3(&this->speedXZ, speedTarget * 0.4f, 1.5f, 1.5f);
        chase_angle(&this->yaw, yawTarget, temp3 * 0.1f);
    }
}

void parallel_back_run_anime_set(Player* this, PlayState* play) {
    f32 temp1;
    f32 temp2;

    if (this->unk_864 < 1.0f) {
        s32 padding;

        temp1 = R_UPDATE_RATE * 0.5f;
        run_anime_frame_set(this, REG(35) / 1000.0f);
        Skeleton_Info_Rom_anime_to_now(play, &this->skelAnime,
                                  GET_PLAYER_ANIM(PLAYER_ANIMGROUP_back_walk, this->modelAnimType), this->unk_868);
        this->unk_864 += 1 * temp1;
        if (this->unk_864 >= 1.0f) {
            this->unk_864 = 1.0f;
        }
        temp1 = this->unk_864;
    } else {
        temp2 = this->speedXZ - (REG(48) / 100.0f);
        if (temp2 < 0.0f) {
            temp1 = 1.0f;
            run_anime_frame_set(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->speedXZ));
            Skeleton_Info_Rom_anime_to_now(play, &this->skelAnime,
                                      GET_PLAYER_ANIM(PLAYER_ANIMGROUP_back_walk, this->modelAnimType), this->unk_868);
        } else {
            temp1 = (REG(37) / 1000.0f) * temp2;
            if (temp1 < 1.0f) {
                run_anime_frame_set(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->speedXZ));
            } else {
                temp1 = 1.0f;
                run_anime_frame_set(this, 1.2f + ((REG(38) / 1000.0f) * temp2));
            }
            Skeleton_Info_Rom_anime_to_morf(play, &this->skelAnime,
                                      GET_PLAYER_ANIM(PLAYER_ANIMGROUP_back_walk, this->modelAnimType), this->unk_868);
            Skeleton_Info_Rom_anime_to_now(play, &this->skelAnime, &gPlayerAnim_link_normal_back_run,
                                      this->unk_868 * (16.0f / 29.0f));
        }
    }

    if (temp1 < 1.0f) {
        Skeleton_Info_Rom_anime_morf(play, &this->skelAnime, 1.0f - temp1);
    }
}

void to_parallel_back_brake_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_parallel_back_brake, 1);
    anime_init_stop_basic(play, this, &gPlayerAnim_link_normal_back_brake);
}

s32 to_parallel_back_brake_check(Player* this, f32* arg1, s16* arg2, PlayState* play) {
    if (this->speedXZ > 6.0f) {
        to_parallel_back_brake_set(this, play);
        return 1;
    }

    if (*arg1 != 0.0f) {
        if (ground_breaking_speedF(this)) {
            *arg1 = 0.0f;
            *arg2 = this->yaw;
        } else {
            return 1;
        }
    }

    return 0;
}

void move_parallel_back_run(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 sp2C;

    parallel_back_run_anime_set(this, play);

    if (!action_select_check(play, this, parallel_back_run_action_check, true)) {
        if (!anchor_parallel_mode_check_set(this)) {
            to_run_set2(this, play, this->yaw);
            return;
        }

        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        sp2C = parallel_mode_key_check(this, &speedTarget, &yawTarget, play);

        if (sp2C >= 0) {
            if (!to_parallel_back_brake_check(this, &speedTarget, &yawTarget, play)) {
                if (sp2C != 0) {
                    to_run_set(this, play);
                } else if (speedTarget > 4.9f) {
                    to_anchor_side_walk_set(this, play);
                } else {
                    to_parallel_side_walk_set(this, play);
                }
            }
        } else {
            s16 sp2A = yawTarget - this->yaw;

            chase_f3(&this->speedXZ, speedTarget * 1.5f, 1.5f, 2.0f);
            chase_angle(&this->yaw, yawTarget, sp2A * 0.1f);

            if ((speedTarget == 0.0f) && (this->speedXZ == 0.0f)) {
                to_parallel_wait_set(this, play);
            }
        }
    }
}

void to_parallel_back_brake_end_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_parallel_back_brake_end, 1);
    anime_init_standard_stop(play, this, &gPlayerAnim_link_normal_back_brake_end);
}

void move_parallel_back_brake(Player* this, PlayState* play) {
    s32 sp34;
    f32 speedTarget;
    s16 yawTarget;

    sp34 = Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    ground_breaking_speedF(this);

    if (!action_select_check(play, this, parallel_back_run_action_check, true)) {
        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (this->speedXZ == 0.0f) {
            this->yaw = this->actor.shape.rot.y;

            if (parallel_mode_key_check(this, &speedTarget, &yawTarget, play) > 0) {
                to_run_set(this, play);
            } else if ((speedTarget != 0.0f) || (sp34 != 0)) {
                to_parallel_back_brake_end_set(this, play);
            }
        }
    }
}

void move_parallel_back_brake_end(Player* this, PlayState* play) {
    s32 sp1C;

    sp1C = Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if (!action_select_check(play, this, parallel_back_run_action_check, true)) {
        if (sp1C != 0) {
            to_parallel_wait_set(this, play);
        }
    }
}

void side_walk_anime_set(PlayState* play, Player* this) {
    f32 frame;
    LinkAnimationHeader* sp38 = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkL, this->modelAnimType);
    LinkAnimationHeader* sp34 = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_side_walkR, this->modelAnimType);

    this->skelAnime.animation = sp38;

    run_anime_frame_set(this, (REG(30) / 1000.0f) + ((REG(32) / 1000.0f) * this->speedXZ));

    frame = this->unk_868 * (16.0f / 29.0f);
    Skeleton_Info_Rom_morf_anime_to_now(play, &this->skelAnime, sp34, frame, sp38, frame, this->unk_870, this->blendTable);
}

void move_anchor_side_walk(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp1;

    side_walk_anime_set(play, this);

    if (!action_select_check(play, this, anchor_side_walk_action_check, true)) {
        if (!anchor_parallel_mode_check_set(this)) {
            to_run_set(this, play);
            return;
        }

        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (parallel_mode_check(this)) {
            temp1 = parallel_mode_key_check(this, &speedTarget, &yawTarget, play);
        } else {
            temp1 = anchor_mode_key_check(this, speedTarget, yawTarget);
        }

        if (temp1 > 0) {
            to_run_set(this, play);
        } else if (temp1 < 0) {
            if (parallel_mode_check(this)) {
                to_parallel_back_run_set(this, yawTarget, play);
            } else {
                to_anchor_back_walk_set(this, yawTarget, play);
            }
        } else if ((this->speedXZ < 3.6f) && (speedTarget < 4.0f)) {
            if (!anchor_mode_check(this) && parallel_mode_check(this)) {
                to_parallel_side_walk_set(this, play);
            } else {
                to_wait_set_check(this, play);
            }
        } else {
            s16 temp2;
            s32 temp3;

            anchor_LR_ratio_check(this, speedTarget, yawTarget);

            temp2 = yawTarget - this->yaw;
            temp3 = ABS(temp2);

            if (temp3 > 0x4000) {
                if (chase_f(&this->speedXZ, 0.0f, 3.0f) != 0) {
                    this->yaw = yawTarget;
                }
            } else {
                speedTarget *= 0.9f;
                chase_f3(&this->speedXZ, speedTarget, 2.0f, 3.0f);
                chase_angle(&this->yaw, yawTarget, temp3 * 0.1f);
            }
        }
    }
}

/**
 * Turn in place until the angle pointed to by the control stick is reached.
 *
 * This is the state that the speedrunning community refers to as "ESS" or "ESS Position".
 * See the bug comment below and https://www.zeldaspeedruns.com/oot/tech/extended-superslide
 * for more information.
 */
static void move_turn(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if (longsword_check2(this)) {
        Skeleton_Proc_Get_init(play, wait_anime_select(this), 0, this->skelAnime.limbCount,
                                         this->skelAnime.morphTable);
        Skeleton_Proc_Add_init(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                      this->skelAnime.morphTable, link_joint_status);
    }

    stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);

    //! @bug This action does not handle xzSpeed in any capacity.
    //! Player's current speed value will be maintained the entire time this action is running.
    //! This is the core bug that allows many different glitches to manifest.
    //!
    //! One possible fix is to kill all speed instantly in `to_turn_set`.
    //! Another possible fix is to gradually kill speed by calling `ground_breaking_speedF`
    //! here, which plenty of other "standing" actions do.

    if (!action_select_check(play, this, turn_action_check, true)) {
        if (speedTarget != 0.0f) {
            this->actor.shape.rot.y = yawTarget;
            to_run_set(this, play);
        } else if (chase_angle(&this->actor.shape.rot.y, yawTarget, this->turnRate)) {
            to_wait_no_morf_set(this, play);
        }

        this->yaw = this->actor.shape.rot.y;
    }
}

void walk_anime_motion_set(Player* this, s32 arg1, PlayState* play) {
    LinkAnimationHeader* anim;
    s16 target;
    f32 rate;

    if (ABS(ground_shape_angle_x) < 3640) {
        target = 0;
    } else {
        target = CLAMP(ground_shape_angle_x, -10922, 10922);
    }

    chase_angle(&this->unk_89C, target, 400);

    if ((this->modelAnimType == PLAYER_ANIMTYPE_3) || ((this->unk_89C == 0) && (this->unk_6C4 <= 0.0f))) {
        if (arg1 == 0) {
            Skeleton_Info_Rom_anime_to_now(play, &this->skelAnime,
                                      GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType), this->unk_868);
        } else {
            Skeleton_Info_Rom_anime_to_morf(play, &this->skelAnime,
                                      GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType), this->unk_868);
        }
        return;
    }

    if (this->unk_89C != 0) {
        rate = this->unk_89C / 10922.0f;
    } else {
        rate = this->unk_6C4 * 0.0006f;
    }

    rate *= fabsf(this->speedXZ) * 0.5f;

    if (rate > 1.0f) {
        rate = 1.0f;
    }

    if (rate < 0.0f) {
        anim = &gPlayerAnim_link_normal_climb_down;
        rate = -rate;
    } else {
        anim = &gPlayerAnim_link_normal_climb_up;
    }

    if (arg1 == 0) {
        Skeleton_Info_Rom_morf_anime_to_now(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType),
                                   this->unk_868, anim, this->unk_868, rate, this->blendTable);
    } else {
        Skeleton_Info_Rom_morf_anime_to_morf(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType),
                                   this->unk_868, anim, this->unk_868, rate, this->blendTable);
    }
}

void run_anime_set(Player* this, PlayState* play) {
    f32 temp1;
    f32 temp2;

    if (this->unk_864 < 1.0f) {
        s32 padding;

        temp1 = R_UPDATE_RATE * 0.5f;

        run_anime_frame_set(this, REG(35) / 1000.0f);
        Skeleton_Info_Rom_anime_to_now(play, &this->skelAnime, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_walk, this->modelAnimType),
                                  this->unk_868);

        this->unk_864 += 1 * temp1;
        if (this->unk_864 >= 1.0f) {
            this->unk_864 = 1.0f;
        }

        temp1 = this->unk_864;
    } else {
        temp2 = this->speedXZ - (REG(48) / 100.0f);

        if (temp2 < 0.0f) {
            temp1 = 1.0f;
            run_anime_frame_set(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->speedXZ));

            walk_anime_motion_set(this, 0, play);
        } else {
            temp1 = (REG(37) / 1000.0f) * temp2;
            if (temp1 < 1.0f) {
                run_anime_frame_set(this, (REG(35) / 1000.0f) + ((REG(36) / 1000.0f) * this->speedXZ));
            } else {
                temp1 = 1.0f;
                run_anime_frame_set(this, 1.2f + ((REG(38) / 1000.0f) * temp2));
            }

            walk_anime_motion_set(this, 1, play);

            Skeleton_Info_Rom_anime_to_now(play, &this->skelAnime, run_anime_select(this), this->unk_868 * (20.0f / 29.0f));
        }
    }

    if (temp1 < 1.0f) {
        Skeleton_Info_Rom_anime_morf(play, &this->skelAnime, 1.0f - temp1);
    }
}

void move_run(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_5;
    run_anime_set(this, play);

    if (!action_select_check(play, this, run_action_check, true)) {
        if (anchor_parallel_mode_check_set(this)) {
            to_run_set(this, play);
            return;
        }

        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);

        if (!to_brake_check(this, &speedTarget, &yawTarget)) {
            run_control(this, speedTarget, yawTarget);
            run_joint_control(this, play);

            if ((this->speedXZ == 0.0f) && (speedTarget == 0.0f)) {
                to_wait_from_run_set(this, play);
            }
        }
    }
}

void move_anchor_run(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_5;
    run_anime_set(this, play);

    if (!action_select_check(play, this, anchor_run_action_check, true)) {
        if (!anchor_parallel_mode_check_set(this)) {
            to_run_set(this, play);
            return;
        }

        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (!to_brake_check(this, &speedTarget, &yawTarget)) {
            if ((parallel_mode_check(this) && (speedTarget != 0.0f) &&
                 (parallel_mode_key_check(this, &speedTarget, &yawTarget, play) <= 0)) ||
                (!parallel_mode_check(this) && (anchor_mode_key_check(this, speedTarget, yawTarget) <= 0))) {
                to_wait_set_check(this, play);
                return;
            }

            run_control(this, speedTarget, yawTarget);
            run_joint_control(this, play);

            if ((this->speedXZ == 0) && (speedTarget == 0)) {
                to_wait_set_check(this, play);
            }
        }
    }
}

void move_anchor_back_walk(Player* this, PlayState* play) {
    s32 sp34;
    f32 speedTarget;
    s16 yawTarget;

    sp34 = Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if (!action_select_check(play, this, anchor_side_walk_action_check, true)) {
        if (!anchor_parallel_mode_check_set(this)) {
            to_run_set(this, play);
            return;
        }

        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if ((this->skelAnime.morphWeight == 0.0f) && (this->skelAnime.curFrame > 5.0f)) {
            ground_breaking_speedF(this);

            if ((this->skelAnime.curFrame > 10.0f) && (anchor_mode_key_check(this, speedTarget, yawTarget) < 0)) {
                to_anchor_back_walk_set(this, yawTarget, play);
                return;
            }

            if (sp34 != 0) {
                to_anchor_back_brake_set(this, play);
            }
        }
    }
}

void move_anchor_back_brake(Player* this, PlayState* play) {
    s32 sp34;
    f32 speedTarget;
    s16 yawTarget;

    sp34 = Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    ground_breaking_speedF(this);

    if (!action_select_check(play, this, anchor_back_brake_action_check, true)) {
        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (this->speedXZ == 0.0f) {
            this->yaw = this->actor.shape.rot.y;

            if (anchor_mode_key_check(this, speedTarget, yawTarget) > 0) {
                to_run_set(this, play);
                return;
            }

            if ((speedTarget != 0.0f) || (sp34 != 0)) {
                to_wait_set_check(this, play);
            }
        }
    }
}

void random_position(Vec3f* src, Vec3f* dest, f32 arg2, f32 arg3, f32 arg4) {
    dest->x = (fqrand() * arg3) + src->x;
    dest->y = (fqrand() * arg4) + (src->y + arg2);
    dest->z = (fqrand() * arg3) + src->z;
}

static Vec3f dust_vec = { 0.0f, 0.0f, 0.0f };
static Vec3f dust_acc = { 0.0f, 0.0f, 0.0f };

s32 slip_effect_set(PlayState* play, Player* this) {
    Vec3f sp2C;

    if ((this->floorSfxOffset == SURFACE_SFX_OFFSET_DIRT) || (this->floorSfxOffset == SURFACE_SFX_OFFSET_SAND)) {
        random_position(&this->actor.shape.feetPos[FOOT_LEFT], &sp2C,
                      this->actor.floorHeight - this->actor.shape.feetPos[FOOT_LEFT].y, 7.0f, 5.0f);
        Effect_SS_Dust_sc_li_ct(play, &sp2C, &dust_vec, &dust_acc, 50, 30);
        random_position(&this->actor.shape.feetPos[FOOT_RIGHT], &sp2C,
                      this->actor.floorHeight - this->actor.shape.feetPos[FOOT_RIGHT].y, 7.0f, 5.0f);
        Effect_SS_Dust_sc_li_ct(play, &this->actor.shape.feetPos[FOOT_RIGHT], &dust_vec, &dust_acc, 50, 30);
        return 1;
    }

    return 0;
}

void move_bean_set(Player* this, PlayState* play) {
    anime_play_repeat(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_check_wait, this->modelAnimType));

    if (DECR(this->av2.actionVar2) == 0) {
        if (!to_look_set_check(this, play)) {
            to_wait_set_check_anim(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_check_end, this->modelAnimType), play);
        }

        this->actor.flags &= ~ACTOR_FLAG_TALK;
        restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));
    }
}

s32 sword_attack_check(Player* this, f32 arg1, f32 arg2, f32 arg3) {
    if ((arg1 <= this->skelAnime.curFrame) && (this->skelAnime.curFrame <= arg3)) {
        sword_attack_set(this, (arg2 <= this->skelAnime.curFrame) ? 1 : -1);
        return 1;
    }

    sword_attack_reset(this);
    return 0;
}

s32 to_defense_kiru_check(Player* this, PlayState* play) {
    if (!child_hyral_shield_check(this) && (sword_check2(this) != 0) && item_key) {
        anime_init_standard_stop(play, this, &gPlayerAnim_link_normal_defense_kiru);
        this->av1.actionVar1 = 1;
        this->meleeWeaponAnimation = PLAYER_MWA_STAB_1H;
        this->yaw = this->actor.shape.rot.y + this->upperLimbRot.y;
        return 1;
    }

    return 0;
}

int to_demo_talk_check(Player* this, PlayState* play) {
    return to_look_set_check(this, play) || to_talk_check(this, play) ||
           to_carry_check(this, play);
}

void quake_set(PlayState* play, s32 speed, s32 y, s32 duration) {
    s32 quakeIndex = startQuake(Gama_play_get_camera(play, CAM_ID_MAIN), QUAKE_TYPE_3);

    setSpeedQuake(quakeIndex, speed);
    setScaleQuake(quakeIndex, y, 0, 0, 0);
    setTimerQuake(quakeIndex, duration);
}

void hammer_impact_set(PlayState* play, Player* this) {
    quake_set(play, 27767, 7, 20);
    play->actorCtx.unk_02 = 4;
    player_rumble_entry(this, 255, 20, 150, 0);
    player_SE_set(this, NA_SE_IT_HAMMER_HIT);
}

void stick_dec_cancel(PlayState* play, Player* this) {
    item_count_vary(ITEM_DEKU_STICK, -1);
    player_item_on(play, this, ITEM_NONE);
}

s32 stick_length_check(PlayState* play, Player* this) {
    if ((this->heldItemAction == PLAYER_IA_DEKU_STICK) && (this->unk_85C > 0.5f)) {
        if (AMMO(ITEM_DEKU_STICK) != 0) {
            Effect_SS_Stick_ct(play, &this->bodyPartsPos[PLAYER_BODYPART_R_HAND], this->actor.shape.rot.y + 0x8000);
            this->unk_85C = 0.5f;
            stick_dec_cancel(play, this);
            player_SE_set(this, NA_SE_IT_WOODSTICK_BROKEN);
        }

        return 1;
    }

    return 0;
}

s32 long_sword_HP_check(PlayState* play, Player* this) {
    if (this->heldItemAction == PLAYER_IA_SWORD_BIGGORON) {
        if (!z_common_data.save.info.playerData.bgsFlag && (z_common_data.save.info.playerData.swordHealth > 0.0f)) {
            if ((z_common_data.save.info.playerData.swordHealth -= 1.0f) <= 0.0f) {
                Effect_SS_Stick_ct(play, &this->bodyPartsPos[PLAYER_BODYPART_R_HAND],
                                    this->actor.shape.rot.y + 0x8000);
                sword_system_dma(play);
                player_SE_set(this, NA_SE_IT_MAJIN_SWORD_BROKEN);
            }
        }

        return 1;
    }

    return 0;
}

void sword_life_check(PlayState* play, Player* this) {
    stick_length_check(play, this);
    long_sword_HP_check(play, this);
}

static LinkAnimationHeader* kiru_rebound_anm[] = {
    &gPlayerAnim_link_fighter_rebound,
    &gPlayerAnim_link_fighter_rebound_long,
    &gPlayerAnim_link_fighter_reboundR,
    &gPlayerAnim_link_fighter_rebound_longR,
};

void to_kiru_rebound_set(PlayState* play, Player* this) {
    s32 padding;
    s32 sp28;

    if (move_defense != this->actionFunc) {
        action_reset(play, this);
        Player_actor_set_process(play, this, move_kiru_rebound, 0);

        if (anchor_mode_check(this)) {
            sp28 = 2;
        } else {
            sp28 = 0;
        }

        anime_init_standard_stop_3f(play, this, kiru_rebound_anm[longsword_check(this) + sp28]);
    }

    player_rumble_entry(this, 180, 20, 100, 0);
    this->speedXZ = -18.0f;
    sword_life_check(play, this);
}

s32 to_kiru_rebound_check(PlayState* play, Player* this) {
    f32 phi_f2;
    CollisionPoly* groundPoly;
    s32 bgId;
    Vec3f sp68;
    Vec3f sp5C;
    Vec3f sp50;
    s32 temp1;
    s32 surfaceMaterial;

    if (this->meleeWeaponState > 0) {
        if (this->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H) {
            if (!(this->meleeWeaponQuads[0].base.atFlags & AT_BOUNCED) &&
                !(this->meleeWeaponQuads[1].base.atFlags & AT_BOUNCED)) {
                if (this->skelAnime.curFrame >= 2.0f) {

                    phi_f2 = search_position_distance2(&this->meleeWeaponInfo[0].tip,
                                                            &this->meleeWeaponInfo[0].base, &sp50);
                    if (phi_f2 != 0.0f) {
                        phi_f2 = (phi_f2 + 10.0f) / phi_f2;
                    }

                    sp68.x = this->meleeWeaponInfo[0].tip.x + (sp50.x * phi_f2);
                    sp68.y = this->meleeWeaponInfo[0].tip.y + (sp50.y * phi_f2);
                    sp68.z = this->meleeWeaponInfo[0].tip.z + (sp50.z * phi_f2);

                    if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &sp68, &this->meleeWeaponInfo[0].tip, &sp5C, &groundPoly,
                                                true, false, false, true, &bgId) &&
                        !T_BGCheck_CheckObjNoHit(&play->colCtx, groundPoly, bgId) &&
                        (T_BGCheck_getAttributeCode_ai(&play->colCtx, groundPoly, bgId) != FLOOR_TYPE_6) &&
                        (jyabujyabu_kiru_check(play, &this->actor, groundPoly, bgId, &sp5C) == 0)) {

                        if (this->heldItemAction == PLAYER_IA_HAMMER) {
                            sword_hit_stop_set(play);
                            hammer_impact_set(play, this);
                            to_kiru_rebound_set(play, this);
                            return 1;
                        }

                        if (this->speedXZ >= 0.0f) {
                            surfaceMaterial = T_BGCheck_getTypeSoundCode_ai(&play->colCtx, groundPoly, bgId);

                            if (surfaceMaterial == SURFACE_MATERIAL_WOOD) {
                                CollisionCheckSetWoodParticle(play, &sp5C, &this->actor.projectedPos);
                            } else {
                                CollisionCheckSetSparkFlashBlue_NoSE(play, &sp5C);
                                if (surfaceMaterial == SURFACE_MATERIAL_DIRT_SOFT) {
                                    player_SE_set(this, NA_SE_IT_WALL_HIT_SOFT);
                                } else {
                                    player_SE_set(this, NA_SE_IT_WALL_HIT_HARD);
                                }
                            }

                            sword_life_check(play, this);
                            this->speedXZ = -14.0f;
                            player_rumble_entry(this, 180, 20, 100, 0);
                        }
                    }
                }
            } else {
                to_kiru_rebound_set(play, this);
                sword_hit_stop_set(play);
                return 1;
            }
        }

        temp1 = (this->meleeWeaponQuads[0].base.atFlags & AT_HIT) || (this->meleeWeaponQuads[1].base.atFlags & AT_HIT);

        if (temp1) {
            if (this->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H) {
                Actor* at = this->meleeWeaponQuads[temp1 ? 1 : 0].base.at;

                if ((at != NULL) && (at->id != ACTOR_EN_KANBAN)) {
                    sword_hit_stop_set(play);
                }
            }

            if ((stick_length_check(play, this) == 0) && (this->heldItemAction != PLAYER_IA_HAMMER)) {
                long_sword_HP_check(play, this);

                if (this->actor.colChkInfo.atHitEffect == 1) {
                    this->actor.colChkInfo.damage = 8;
                    damage_set(play, this, PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK, 0.0f, 0.0f, this->actor.shape.rot.y,
                                  20);
                    return 1;
                }
            }
        }
    }

    return 0;
}

void move_defense(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (!child_hyral_shield_check(this)) {
            anime_init_standard_repeat(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense_wait, this->modelAnimType));
        }
        this->av2.actionVar2 = 1;
        this->av1.actionVar1 = 0;
    }

    if (!child_hyral_shield_check(this)) {
        this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
        uperbody_action_check(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_SHIELDING;
    }

    ground_breaking_speedF(this);

    if (this->av2.actionVar2 != 0) {
        f32 sp54;
        f32 sp50;
        s16 sp4E;
        s16 sp4C;
        s16 sp4A;
        s16 sp48;
        s16 sp46;
        f32 sp40;

        sp54 = pad->rel.stick_y * 100;
        sp50 = pad->rel.stick_x * -120;
        sp4E = this->actor.shape.rot.y - getCameraAngleY(GET_ACTIVE_CAM(play));

        sp40 = cos_s(sp4E);
        sp4C = (sin_s(sp4E) * sp50) + (sp54 * sp40);
        sp40 = cos_s(sp4E);
        sp4A = (sp50 * sp40) - (sin_s(sp4E) * sp54);

        if (sp4C > 3500) {
            sp4C = 3500;
        }

        sp48 = ABS(sp4C - this->actor.focus.rot.x) * 0.25f;
        if (sp48 < 100) {
            sp48 = 100;
        }

        sp46 = ABS(sp4A - this->upperLimbRot.y) * 0.25f;
        if (sp46 < 50) {
            sp46 = 50;
        }

        chase_angle(&this->actor.focus.rot.x, sp4C, sp48);
        this->upperLimbRot.x = this->actor.focus.rot.x;
        chase_angle(&this->upperLimbRot.y, sp4A, sp46);

        if (this->av1.actionVar1 != 0) {
            if (!to_kiru_rebound_check(play, this)) {
                if (this->skelAnime.curFrame < 2.0f) {
                    sword_attack_set(this, 1);
                }
            } else {
                this->av2.actionVar2 = 1;
                this->av1.actionVar1 = 0;
            }
        } else if (!to_demo_talk_check(this, play)) {
            if (to_defense_check(this, play)) {
                to_defense_kiru_check(this, play);
            } else {
                this->stateFlags1 &= ~PLAYER_STATE1_SHIELDING;
                sword_attack_reset(this);

                if (child_hyral_shield_check(this)) {
                    to_wait_proc_set_init(this, play);
                    Skeleton_Info_Rom_init(play, &this->skelAnime, &gPlayerAnim_clink_normal_defense_ALL, 1.0f,
                                         Si2_anime_end_frame(&gPlayerAnim_clink_normal_defense_ALL), 0.0f,
                                         ANIMMODE_ONCE, 0.0f);
                    anime_move_init(play, this, ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT);
                } else {
                    if (this->itemAction < 0) {
                        now_item_cancel(this);
                    }
                    to_wait_set_check_anim(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense_end, this->modelAnimType), play);
                }

                player_SE_set(this, NA_SE_IT_SHIELD_REMOVE);
                return;
            }
        } else {
            return;
        }
    }

    this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
    defense_shape_type_set(this);

    this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_UPPER_X | UNK6AE_ROT_UPPER_Y;
}

void move_defense_hit(Player* this, PlayState* play) {
    s32 interruptResult;
    LinkAnimationHeader* anim;
    f32 frames;

    ground_breaking_speedF(this);

    if (this->av1.actionVar1 == 0) {
        uperbody_action_flag = uperbody_action_check(this, play);

        if ((uperbody_defense == this->upperActionFunc) ||
            (to_next_action_cancel_check(play, this, &this->upperSkelAnime, 4.0f) >= PLAYER_INTERRUPT_MOVE)) {
            Player_actor_set_process(play, this, move_anchor_wait, 1);
        }
    } else {
        interruptResult = to_next_action_cancel_check(play, this, &this->skelAnime, 4.0f);

        if ((interruptResult != PLAYER_INTERRUPT_NEW_ACTION) &&
            ((interruptResult >= PLAYER_INTERRUPT_MOVE) || Skeleton_Info_Rom_anime_play(play, &this->skelAnime))) {
            Player_actor_set_process(play, this, move_defense, 1);
            this->stateFlags1 |= PLAYER_STATE1_SHIELDING;
            defense_shape_type_set(this);
            anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_defense, this->modelAnimType);
            frames = Si2_anime_end_frame(anim);
            Skeleton_Info_Rom_init(play, &this->skelAnime, anim, 1.0f, frames, frames, ANIMMODE_ONCE, 0.0f);
        }
    }
}

void move_damage(Player* this, PlayState* play) {
    s32 interruptResult;

    ground_breaking_speedF(this);

    interruptResult = to_next_action_cancel_check(play, this, &this->skelAnime, 16.0f);

    if ((interruptResult != PLAYER_INTERRUPT_NEW_ACTION) &&
        (Skeleton_Info_Rom_anime_play(play, &this->skelAnime) || (interruptResult >= PLAYER_INTERRUPT_MOVE))) {
        to_wait_set_check(this, play);
    }
}

void move_big_damage(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;

    no_damage_timer_check(this);

    if (!(this->stateFlags1 & PLAYER_STATE1_29) && (this->av2.actionVar2 == 0) &&
        (this->knockbackType != PLAYER_KNOCKBACK_NONE)) {
        s16 temp = this->actor.shape.rot.y - this->knockbackRot;

        this->yaw = this->actor.shape.rot.y = this->knockbackRot;
        this->speedXZ = this->knockbackSpeed;

        if (ABS(temp) > 0x4000) {
            this->actor.shape.rot.y = this->knockbackRot + 0x8000;
        }

        if (this->actor.velocity.y < 0.0f) {
            this->actor.gravity = 0.0f;
            this->actor.velocity.y = 0.0f;
        }
    }

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->av2.actionVar2 != 0) {
            this->av2.actionVar2--;
            if (this->av2.actionVar2 == 0) {
                Ext_to_wait_set(this, play);
            }
        } else if ((this->stateFlags1 & PLAYER_STATE1_29) ||
                   (!(this->cylinder.base.acFlags & AC_HIT) && (this->knockbackType == PLAYER_KNOCKBACK_NONE))) {
            if (this->stateFlags1 & PLAYER_STATE1_29) {
                this->av2.actionVar2++;
            } else {
                Player_actor_set_process(play, this, move_big_damage_slip, 0);
                this->stateFlags1 |= PLAYER_STATE1_26;
            }

            anime_init_standard_stop(play, this,
                                (this->yaw != this->actor.shape.rot.y) ? &gPlayerAnim_link_normal_front_downB
                                                                       : &gPlayerAnim_link_normal_back_downB);
            player_voice_SE_set(this, NA_SE_VO_LI_FREEZE);
        }
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        player_ground_SE_set(this, NA_SE_PL_BOUND);
    }
}

void move_big_damage_slip(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;
    no_damage_timer_check(this);

    ground_breaking_speedF(this);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime) && (this->speedXZ == 0.0f)) {
        if (this->stateFlags1 & PLAYER_STATE1_29) {
            this->av2.actionVar2++;
        } else {
            Player_actor_set_process(play, this, move_big_damage_wake, 0);
            this->stateFlags1 |= PLAYER_STATE1_26;
        }

        anime_init_standard_stop_3f(play, this,
                                    (this->yaw != this->actor.shape.rot.y) ? &gPlayerAnim_link_normal_front_down_wake
                                                                           : &gPlayerAnim_link_normal_back_down_wake);
        this->yaw = this->actor.shape.rot.y;
    }
}

void move_big_damage_wake(Player* this, PlayState* play) {
#if OOT_VERSION >= PAL_1_0
    static AnimSfxEntry SE_set_status[] = {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 20) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 30) },
    };
#endif

    s32 interruptResult;

    this->stateFlags2 |= PLAYER_STATE2_5;
    no_damage_timer_check(this);

    if (this->stateFlags1 & PLAYER_STATE1_29) {
        Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    } else {
        interruptResult = to_next_action_cancel_check(play, this, &this->skelAnime, 16.0f);

        if ((interruptResult != PLAYER_INTERRUPT_NEW_ACTION) &&
            (Skeleton_Info_Rom_anime_play(play, &this->skelAnime) || (interruptResult >= PLAYER_INTERRUPT_MOVE))) {
            to_wait_set_check(this, play);
        }
    }

#if OOT_VERSION >= PAL_1_0
    player_anime_check_SE_set(this, SE_set_status);
#endif
}

static Vec3f wake_up_elf_set_offset = { 0.0f, 0.0f, 5.0f };

void down_result_check(PlayState* play, Player* this) {
    if (this->av2.actionVar2 != 0) {
        if (this->av2.actionVar2 > 0) {
            this->av2.actionVar2--;
            if (this->av2.actionVar2 == 0) {
                if (this->stateFlags1 & PLAYER_STATE1_27) {
                    Skeleton_Info_Rom_init(play, &this->skelAnime, &gPlayerAnim_link_swimer_swim_wait, 1.0f, 0.0f,
                                         Si2_anime_end_frame(&gPlayerAnim_link_swimer_swim_wait), ANIMMODE_ONCE,
                                         -16.0f);
                } else {
                    Skeleton_Info_Rom_init(play, &this->skelAnime, &gPlayerAnim_link_derth_rebirth, 1.0f, 99.0f,
                                         Si2_anime_end_frame(&gPlayerAnim_link_derth_rebirth), ANIMMODE_ONCE, 0.0f);
                }
                z_common_data.healthAccumulator = 0x140;
                this->av2.actionVar2 = -1;
            }
        } else if (z_common_data.healthAccumulator == 0) {
            this->stateFlags1 &= ~PLAYER_STATE1_DEAD;
            if (this->stateFlags1 & PLAYER_STATE1_27) {
                to_swim_wait_set(play, this);
            } else {
                Ext_to_wait_set(this, play);
            }
            this->unk_A87 = 20;
            shield_no_damage_timer_check(this, -20);
            Na_ClearLinkDownSound();
        }
    } else if (this->av1.actionVar1 != 0) {
        this->av2.actionVar2 = 60;
        make_elf(play, this, &this->actor.world.pos, &wake_up_elf_set_offset, FAIRY_REVIVE_DEATH);
        player_SE_set(this, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
        makeOnepointDemo(play, 9908, 125, &this->actor, CAM_ID_MAIN);
    } else if (play->gameOverCtx.state == GAMEOVER_DEATH_WAIT_GROUND) {
        play->gameOverCtx.state = GAMEOVER_DEATH_DELAY_MENU;
    }
}

static void move_down(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_PL_BOUND, ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 60) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 140) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 164) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 170) },
    };

    if (this->currentTunic != PLAYER_TUNIC_GORON) {
        if ((play->roomCtx.curRoom.environmentType == ROOM_ENV_HOT) || (ground_attribute == FLOOR_TYPE_9) ||
            ((damage_ground_attribute_check(ground_attribute) >= 0) &&
             !T_BGCheck_CheckDamagePoly(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId))) {
            fire_set_init(this);
        }
    }

    ground_breaking_speedF(this);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (this->actor.category == ACTORCAT_PLAYER) {
            down_result_check(play, this);
        }
        return;
    }

    if (this->skelAnime.animation == &gPlayerAnim_link_derth_rebirth) {
        player_anime_check_SE_set(this, SE_set_status);
    }
#if OOT_VERSION >= PAL_1_0
    else if (this->skelAnime.animation == &gPlayerAnim_link_normal_electric_shock_end) {
        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 88.0f)) {
            player_ground_SE_set(this, NA_SE_PL_BOUND);
        }
    }
#endif
}

void fall_voice_set(Player* this, u16 sfxId) {
    player_voice_SE_set(this, sfxId);

    if ((this->heldActor != NULL) && (this->heldActor->id == ACTOR_EN_RU1)) {
        Actor_SE_set(this->heldActor, NA_SE_VO_RT_FALL);
    }
}

static FallImpactInfo landing_damage_status[] = {
    { -8, 180, 40, 100, NA_SE_VO_LI_LAND_DAMAGE_S },
    { -16, 255, 140, 150, NA_SE_VO_LI_LAND_DAMAGE_S },
};

s32 landing_just_check(PlayState* play, Player* this) {
    s32 fallDistance;

    if ((ground_attribute == FLOOR_TYPE_6) || (ground_attribute == FLOOR_TYPE_9)) {
        fallDistance = 0;
    } else {
        fallDistance = this->fallDistance;
    }

    chase_f(&this->speedXZ, 0.0f, 1.0f);

    this->stateFlags1 &= ~(PLAYER_STATE1_18 | PLAYER_STATE1_19);

    if (fallDistance >= 400) {
        s32 impactIndex;
        FallImpactInfo* impactInfo;

        if (this->fallDistance < 800) {
            impactIndex = 0;
        } else {
            impactIndex = 1;
        }

        impactInfo = &landing_damage_status[impactIndex];

        if (Ext_to_damage_set(play, impactInfo->damage)) {
            return -1;
        }

        no_damage_timer_set(this, 40);
        quake_set(play, 32967, 2, 30);
        player_rumble_entry(this, impactInfo->rumbleStrength, impactInfo->rumbleDuration,
                             impactInfo->rumbleDecreaseRate, 0);
        player_SE_set(this, NA_SE_PL_BODY_HIT);
        player_voice_SE_set(this, impactInfo->sfxId);

        return impactIndex + 1;
    }

    if (fallDistance > 200) {
        fallDistance *= 2;

        if (fallDistance > 255) {
            fallDistance = 255;
        }

        player_rumble_entry(this, (u8)fallDistance, (u8)(fallDistance * 0.1f), (u8)fallDistance, 0);

        if (ground_attribute == FLOOR_TYPE_6) {
            player_voice_SE_set(this, NA_SE_VO_LI_CLIMB_END);
        }
    }

    player_land_SE_set(this);

    return 0;
}

void carry_throw_set(PlayState* play, Player* this, f32 speedXZ, f32 velocityY) {
    Actor* heldActor = this->heldActor;

    if (!carry_cancel_check(play, this, heldActor)) {
        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speed = speedXZ;
        heldActor->velocity.y = velocityY;
        Player_actor_uperbody_process_init(play, this);
        player_SE_set(this, NA_SE_PL_THROW);
        player_voice_SE_set(this, NA_SE_VO_LI_SWORD_N);
    }
}

void move_landing(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    if (z_common_data.respawn[RESPAWN_MODE_TOP].data > 40) {
        this->actor.gravity = 0.0f;
    } else if (anchor_mode_check(this)) {
        this->actor.gravity = -1.2f;
    }

    stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Actor* heldActor;

        if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
            heldActor = this->heldActor;

            if (!carry_cancel_check(play, this, heldActor) && (heldActor->id == ACTOR_EN_NIW) &&
                CHECK_BTN_ANY(pad->press.button, BTN_A | BTN_B | BTN_CLEFT | BTN_CDOWN | BTN_CRIGHT)) {
                carry_throw_set(play, this, this->speedXZ + 2.0f, this->actor.velocity.y + 2.0f);
            }
        }

        Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

        if (!(this->stateFlags2 & PLAYER_STATE2_19)) {
            sky_control(this, &speedTarget, &yawTarget);
        }

        uperbody_action_check(this, play);

        if (((this->stateFlags2 & PLAYER_STATE2_19) && (this->av1.actionVar1 == 2)) || !to_jump_kiru_check(this, play)) {
            if (this->actor.velocity.y < 0.0f) {
                if (this->av2.actionVar2 >= 0) {
                    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->av2.actionVar2 == 0) ||
                        (this->fallDistance > 0)) {
                        if ((to_ground_y > 800.0f) || (this->stateFlags1 & PLAYER_STATE1_2)) {
                            fall_voice_set(this, NA_SE_VO_LI_FALL_S);
                            this->stateFlags1 &= ~PLAYER_STATE1_2;
                        }

                        Skeleton_Info_Rom_init(play, &this->skelAnime, &gPlayerAnim_link_normal_landing, 1.0f, 0.0f, 0.0f,
                                             ANIMMODE_ONCE, 8.0f);
                        this->av2.actionVar2 = -1;
                    }
                } else {
                    if ((this->av2.actionVar2 == -1) && (this->fallDistance > 120.0f) && (to_ground_y > 280.0f)) {
                        this->av2.actionVar2 = -2;
                        fall_voice_set(this, NA_SE_VO_LI_FALL_L);
                    }

                    if ((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) &&
                        !(this->stateFlags2 & PLAYER_STATE2_19) &&
                        !(this->stateFlags1 & (PLAYER_STATE1_CARRYING_ACTOR | PLAYER_STATE1_27)) &&
                        (this->speedXZ > 0.0f)) {
                        if ((this->yDistToLedge >= 150.0f) &&
                            (this->controlStickDirections[this->controlStickDataIndex] == PLAYER_STICK_DIR_FORWARD)) {
                            to_climb_check(this, play, wall_code);
                        } else if ((this->ledgeClimbType >= PLAYER_LEDGE_CLIMB_2) && (this->yDistToLedge < 150.0f) &&
                                   (((this->actor.world.pos.y - this->actor.floorHeight) + this->yDistToLedge) >
                                    (70.0f * this->ageProperties->unk_08))) {
                            Skeleton_Proc_Anime_Change_Stop_On(play);
                            if (this->stateFlags1 & PLAYER_STATE1_2) {
                                player_voice_SE_set(this, NA_SE_VO_LI_HOOKSHOT_HANG);
                            } else {
                                player_voice_SE_set(this, NA_SE_VO_LI_HANG);
                            }
                            this->actor.world.pos.y += this->yDistToLedge;
                            to_fall_set(play, this, this->actor.wallPoly, this->distToInteractWall,
                                          GET_PLAYER_ANIM(PLAYER_ANIMGROUP_jump_climb_hold, this->modelAnimType));
                            this->actor.shape.rot.y = this->yaw += 0x8000;
                            this->stateFlags1 |= PLAYER_STATE1_13;
                        }
                    }
                }
            }
        }
    } else {
        LinkAnimationHeader* anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_landing, this->modelAnimType);
        s32 sp3C;

        if (this->stateFlags2 & PLAYER_STATE2_19) {
            if (anchor_mode_check(this)) {
                anim = anchor_jump_anm_data[this->av1.actionVar1][2];
            } else {
                anim = anchor_jump_anm_data[this->av1.actionVar1][1];
            }
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_run_jump) {
            anim = &gPlayerAnim_link_normal_run_jump_end;
        } else if (anchor_mode_check(this)) {
            anim = &gPlayerAnim_link_anchor_landingR;
            anchor_LR_ratio_reset(this);
        } else if (this->fallDistance <= 80) {
            anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_short_landing, this->modelAnimType);
        } else if ((this->fallDistance < 800) &&
                   (this->controlStickDirections[this->controlStickDataIndex] == PLAYER_STICK_DIR_FORWARD) &&
                   !(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            to_landing_roll_set(this, play);
            return;
        }

        sp3C = landing_just_check(play, this);

        if (sp3C > 0) {
            to_wait_set_check_anim(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_landing, this->modelAnimType), play);
            this->skelAnime.endFrame = 8.0f;

            // `to_wait_set_check_anim` above can choose from a few different "idle" action variants.
            // However `fallDamageStunTimer` is only processed by `move_wait`.
            // This means it is possible for the stun to not take effect
            // (for example, by holding Z when landing).
            if (sp3C == 1) {
                this->av2.fallDamageStunTimer = 10;
            } else {
                this->av2.fallDamageStunTimer = 20;
            }
        } else if (sp3C == 0) {
            to_wait_set_check_anim(this, anim, play);
        }
    }
}

void move_landing_roll(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_VO_LI_SWORD_N, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 1) },
        { NA_SE_PL_WALK_GROUND, ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR_BY_AGE, 6) },
        { NA_SE_PL_ROLL, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 6) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 18) },
    };

    Actor* ocCollidedActor;
    s32 interruptResult;
    s32 animDone;
    DynaPolyActor* wallPolyActor;
    s32 padding;
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_5;

    ocCollidedActor = NULL;
    animDone = Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 8.0f)) {
        shield_no_damage_timer_check(this, FRAMERATE_CONST(-10, -8));
    }

    if (!to_demo_talk_check(this, play)) {
        if (this->av2.bonked) {
            chase_f(&this->speedXZ, 0.0f, 2.0f);

            interruptResult = to_next_action_cancel_check(play, this, &this->skelAnime, 5.0f);

            if ((interruptResult != PLAYER_INTERRUPT_NEW_ACTION) &&
                ((interruptResult >= PLAYER_INTERRUPT_MOVE) || animDone)) {
                to_wait_proc_set_init(this, play);
            }
        } else {
            // Must have a speed of 7 or above to be able to bonk into something
            if (this->speedXZ >= 7.0f) {
                if (((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) &&
                     (action_wall_distance_angle_y < 0x2000)) ||
                    ((this->cylinder.base.ocFlags1 & OC1_HIT) &&
                     (ocCollidedActor = this->cylinder.base.oc,
                      ((ocCollidedActor->id == ACTOR_EN_WOOD02) &&
                       (ABS((s16)(this->actor.world.rot.y - ocCollidedActor->yawTowardsPlayer)) > 0x6000))))) {
                    if (ocCollidedActor != NULL) {
                        // The EN_WOOD02 actor uses home y rotation as a flag to signal that it has been
                        // bonked into and should try to spawn a drop.
                        ocCollidedActor->home.rot.y = 1;
                    } else if (this->actor.wallBgId != BGCHECK_SCENE) {
                        wallPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, this->actor.wallBgId);

                        if ((wallPolyActor != NULL) && (wallPolyActor->actor.id == ACTOR_OBJ_KIBAKO2)) {
                            // The OBJ_KIBAKO2 actor uses home z rotation as a flag to signal that it has been
                            // bonked into and should break.
                            wallPolyActor->actor.home.rot.z = 1;
                        }
                    }

                    anime_init_standard_stop(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_hip_down, this->modelAnimType));
                    this->speedXZ = -this->speedXZ;
                    quake_set(play, 33267, 3, 12);
                    player_rumble_entry(this, 255, 20, 150, 0);
                    player_SE_set(this, NA_SE_PL_BODY_HIT);
                    player_voice_SE_set(this, NA_SE_VO_LI_CLIMB_END);
                    this->av2.bonked = true;

                    return;
                }
            }

            if ((this->skelAnime.curFrame < 15.0f) || !to_kiru_check(this, play)) {
                if (this->skelAnime.curFrame >= 20.0f) {
                    to_wait_proc_set_init(this, play);

                    return;
                }

                stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play);

                // `speedTarget` at this point is the speed that would be used for regular walking.
                // Rolling speed is 1.5 times faster than what the walking speed would be for the current control stick
                // input.
                speedTarget *= 1.5f;

                if ((speedTarget < 3.0f) ||
                    (this->controlStickDirections[this->controlStickDataIndex] != PLAYER_STICK_DIR_FORWARD)) {
                    speedTarget = 3.0f;
                }

                run_control(this, speedTarget, this->actor.shape.rot.y);

                if (slip_effect_set(play, this)) {
                    Actor_player_level_SE_set(&this->actor, NA_SE_PL_ROLL_DUST - SFX_FLAG);
                }

                player_anime_check_SE_set(this, SE_set_status);
            }
        }
    }
}

void move_run_jump_water(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_5;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_init_standard_repeat(play, this, &gPlayerAnim_link_normal_run_jump_water_fall_wait);
    }

    chase_f(&this->speedXZ, 0.0f, 0.05f);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.colChkInfo.damage = 0x10;
        damage_set(play, this, PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE, 4.0f, 5.0f, this->actor.shape.rot.y, 20);
    }
}

void move_jump_kiru(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    this->stateFlags2 |= PLAYER_STATE2_5;

    this->actor.gravity = -1.2f;
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if (!to_kiru_rebound_check(play, this)) {
        sword_attack_check(this, 6.0f, 7.0f, 99.0f);

        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
            sky_control(this, &speedTarget, &this->yaw);
            return;
        }

        if (landing_just_check(play, this) >= 0) {
            this->meleeWeaponAnimation += 2;
            to_kiru_set(play, this, this->meleeWeaponAnimation);
            this->unk_845 = 3;
            player_land_SE_set(this);
        }
    }
}

s32 to_power_kiru_check(Player* this, PlayState* play) {
    if (to_tool_demo_check(play, this)) {
        this->stateFlags2 |= PLAYER_STATE2_17;
    } else {
        if (!CHECK_BTN_ALL(pad->cur.button, BTN_B)) {
            s32 temp;

            if ((this->unk_858 >= 0.85f) || rolling_kiru_check(this)) {
                temp = Wrolling_kiru[longsword_check(this)];
            } else {
                temp = rolling_kiru[longsword_check(this)];
            }

            to_kiru_set(play, this, temp);
            shield_no_damage_timer_check(this, -8);

            this->stateFlags2 |= PLAYER_STATE2_17;
            if (this->controlStickDirections[this->controlStickDataIndex] == PLAYER_STICK_DIR_FORWARD) {
                this->stateFlags2 |= PLAYER_STATE2_30;
            }
        } else {
            return 0;
        }
    }

    return 1;
}

void to_power_kiru_walk_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_power_kiru_walk, 1);
}

void to_power_kiru_side_walk_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_power_kiru_side_walk, 1);
}

void to_wait_from_power_kiru_set(Player* this, PlayState* play) {
    to_wait_proc_set(this, play);
    sword_attack_reset(this);
    anime_init_stop_basic(play, this, link_power_kiru_wait_end_anm[longsword_check(this)]);
    this->yaw = this->actor.shape.rot.y;
}

void to_power_kiru_wait_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_power_kiru_wait, 1);
    this->unk_868 = 0.0f;
    anime_init_standard_repeat(play, this, link_power_kiru_wait_anm[longsword_check(this)]);
    this->av2.actionVar2 = 1;
}

void power_kiru_timer_check(Player* this) {
    chase_f(&this->unk_858, 1.0f, 0.02f);
}

void move_power_kiru_wait(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp;

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_move_reset(this);
        parallel_camera_init(this);
        this->stateFlags1 &= ~PLAYER_STATE1_PARALLEL;
        anime_init_standard_repeat(play, this, link_power_kiru_wait_anm[longsword_check(this)]);
        this->av2.actionVar2 = -1;
    }

    ground_breaking_speedF(this);

    if (!to_demo_talk_check(this, play) && (this->av2.actionVar2 != 0)) {
        power_kiru_timer_check(this);

        if (this->av2.actionVar2 < 0) {
            if (this->unk_858 >= 0.1f) {
                this->unk_845 = 0;
                this->av2.actionVar2 = 1;
            } else if (!CHECK_BTN_ALL(pad->cur.button, BTN_B)) {
                to_wait_from_power_kiru_set(this, play);
            }
        } else if (!to_power_kiru_check(this, play)) {
            stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

            temp = power_kiru_mode_key_check(this, &speedTarget, &yawTarget, play);
            if (temp > 0) {
                to_power_kiru_walk_set(this, play);
            } else if (temp < 0) {
                to_power_kiru_side_walk_set(this, play);
            }
        }
    }
}

void move_power_kiru_walk(Player* this, PlayState* play) {
    s16 temp1;
    s32 temp2;
    f32 sp5C;
    f32 sp58;
    f32 speedTarget;
    s16 yawTarget;
    s32 temp4;
    s16 temp5;
    s32 sp44;

    temp1 = this->yaw - this->actor.shape.rot.y;
    temp2 = ABS(temp1);

    sp5C = fabsf(this->speedXZ);
    sp58 = sp5C * 1.5f;

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (sp58 < 1.5f) {
        sp58 = 1.5f;
    }

    sp58 = ((temp2 < 0x4000) ? -1.0f : 1.0f) * sp58;

    run_anime_frame_set(this, sp58);

    sp58 = CLAMP(sp5C * 0.5f, 0.5f, 1.0f);

    Skeleton_Info_Rom_morf_anime_to_now(play, &this->skelAnime, link_power_kiru_wait_anm[longsword_check(this)], 0.0f,
                               link_power_kiru_walk_anm[longsword_check(this)], this->unk_868 * (21.0f / 29.0f), sp58,
                               this->blendTable);

    if (!to_demo_talk_check(this, play) && !to_power_kiru_check(this, play)) {
        power_kiru_timer_check(this);
        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        temp4 = power_kiru_mode_key_check(this, &speedTarget, &yawTarget, play);

        if (temp4 < 0) {
            to_power_kiru_side_walk_set(this, play);
            return;
        }

        if (temp4 == 0) {
            speedTarget = 0.0f;
            yawTarget = this->yaw;
        }

        temp5 = yawTarget - this->yaw;
        sp44 = ABS(temp5);

        if (sp44 > 0x4000) {
            if (chase_f(&this->speedXZ, 0.0f, 1.0f)) {
                this->yaw = yawTarget;
            }
            return;
        }

        chase_f3(&this->speedXZ, speedTarget * 0.2f, 1.0f, 0.5f);
        chase_angle(&this->yaw, yawTarget, sp44 * 0.1f);

        if ((speedTarget == 0.0f) && (this->speedXZ == 0.0f)) {
            to_power_kiru_wait_set(this, play);
        }
    }
}

void move_power_kiru_side_walk(Player* this, PlayState* play) {
    f32 sp5C;
    f32 sp58;
    f32 speedTarget;
    s16 yawTarget;
    s32 temp4;
    s16 temp5;
    s32 sp44;

    sp5C = fabsf(this->speedXZ);

    this->stateFlags1 |= PLAYER_STATE1_CHARGING_SPIN_ATTACK;

    if (sp5C == 0.0f) {
        sp5C = ABS(this->unk_87C) * 0.0015f;
        if (sp5C < 400.0f) {
            sp5C = 0.0f;
        }
        run_anime_frame_set(this, ((this->unk_87C >= 0) ? 1 : -1) * sp5C);
    } else {
        sp58 = sp5C * 1.5f;
        if (sp58 < 1.5f) {
            sp58 = 1.5f;
        }
        run_anime_frame_set(this, sp58);
    }

    sp58 = CLAMP(sp5C * 0.5f, 0.5f, 1.0f);

    Skeleton_Info_Rom_morf_anime_to_now(play, &this->skelAnime, link_power_kiru_wait_anm[longsword_check(this)], 0.0f,
                               link_power_kiru_side_walk_anm[longsword_check(this)], this->unk_868 * (21.0f / 29.0f), sp58,
                               this->blendTable);

    if (!to_demo_talk_check(this, play) && !to_power_kiru_check(this, play)) {
        power_kiru_timer_check(this);
        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        temp4 = power_kiru_mode_key_check(this, &speedTarget, &yawTarget, play);

        if (temp4 > 0) {
            to_power_kiru_walk_set(this, play);
            return;
        }

        if (temp4 == 0) {
            speedTarget = 0.0f;
            yawTarget = this->yaw;
        }

        temp5 = yawTarget - this->yaw;
        sp44 = ABS(temp5);

        if (sp44 > 0x4000) {
            if (chase_f(&this->speedXZ, 0.0f, 1.0f)) {
                this->yaw = yawTarget;
            }
            return;
        }

        chase_f3(&this->speedXZ, speedTarget * 0.2f, 1.0f, 0.5f);
        chase_angle(&this->yaw, yawTarget, sp44 * 0.1f);

        if ((speedTarget == 0.0f) && (this->speedXZ == 0.0f) && (sp5C == 0.0f)) {
            to_power_kiru_wait_set(this, play);
        }
    }
}

void move_stepup100(Player* this, PlayState* play) {
    s32 sp3C;
    s32 interruptResult;
    f32 temp3;

    this->stateFlags2 |= PLAYER_STATE2_5;
    sp3C = Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if (this->skelAnime.animation == &gPlayerAnim_link_normal_250jump_start) {
        this->speedXZ = 1.0f;

        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 8.0f)) {
            f32 temp1 = this->yDistToLedge;

            if (temp1 > this->ageProperties->unk_0C) {
                temp1 = this->ageProperties->unk_0C;
            }

            if (this->stateFlags1 & PLAYER_STATE1_27) {
                temp1 *= 0.085f;
            } else {
                temp1 *= 0.072f;
            }

            if (!LINK_IS_ADULT) {
                temp1 += 1.0f;
            }

            to_jump_init(this, NULL, temp1, play, NA_SE_VO_LI_AUTO_JUMP);
            this->av2.actionVar2 = -1;
        }
    } else {
        interruptResult = to_next_action_cancel_check(play, this, &this->skelAnime, 4.0f);

        if (interruptResult == PLAYER_INTERRUPT_NEW_ACTION) {
            this->stateFlags1 &= ~(PLAYER_STATE1_14 | PLAYER_STATE1_18);
            return;
        }

        if ((sp3C != 0) || (interruptResult >= PLAYER_INTERRUPT_MOVE)) {
            to_wait_no_morf_set(this, play);
            this->stateFlags1 &= ~(PLAYER_STATE1_14 | PLAYER_STATE1_18);
            return;
        }

        temp3 = 0.0f;

        if (this->skelAnime.animation == &gPlayerAnim_link_swimer_swim_15step_up) {
            if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 30.0f)) {
                water_out_set(play, this, 10.0f);
            }
#if OOT_VERSION < PAL_1_0
            temp3 = 68.0f;
#else
            temp3 = 50.0f;
#endif
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_150step_up) {
            temp3 = 30.0f;
        } else if (this->skelAnime.animation == &gPlayerAnim_link_normal_100step_up) {
            temp3 = 16.0f;
        }

        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, temp3)) {
            player_land_SE_set(this);
            player_voice_SE_set(this, NA_SE_VO_LI_CLIMB_END);
        }

        if ((this->skelAnime.animation == &gPlayerAnim_link_normal_100step_up) || (this->skelAnime.curFrame > 5.0f)) {
            if (this->av2.actionVar2 == 0) {
                player_jump_SE_set(this);
                this->av2.actionVar2 = 1;
            }
            chase_f(&this->actor.shape.yOffset, 0.0f, 150.0f);
        }
    }
}

/**
 * Allow the held item put away process to complete before running `afterPutAwayFunc`
 */
void move_demo_init(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    // Wait for the held item put away process to complete.
    // Determining if the put away process is complete is a bit complicated:
    // `uperbody_action_check` will only return false if the current UpperAction returns false.
    // The UpperAction responsible for putting away items, `uperbody_nom2figh_play`, constantly
    // returns true until the item change is done. False won't be returned until the item change is done, and a new
    // UpperAction is running and can return false itself.
    // Note that this implementation allows for delaying indefinitely by, for example, holding shield
    // during the item put away. The shield UpperAction will return true while shielding and targeting.
    // Meaning, `afterPutAwayFunc` will be delayed until the player decides to let go of shield.
    // This quirk can contribute to the possibility of other bugs manifesting.
    //
    // The other conditions listed will force the put away delay function to run instantly if carrying an actor.
    // This is necessary because the UpperAction for carrying actors will always return true while holding
    // the actor, so `!uperbody_action_check` could never pass.
    if (((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->heldActor != NULL) &&
         (this->getItemId == GI_NONE)) ||
        !uperbody_action_check(this, play)) {
        this->afterPutAwayFunc(play, this);
    }
}

s32 demo_run_control(PlayState* play, Player* this, CsCmdActorCue* cue, f32 arg3, s16 arg4, s32 arg5) {
    if ((arg5 != 0) && (this->speedXZ == 0.0f)) {
        return Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    }

    if (arg5 != 2) {
        f32 sp34 = R_UPDATE_RATE * 0.5f;
        f32 selfDistX = cue->endPos.x - this->actor.world.pos.x;
        f32 selfDistZ = cue->endPos.z - this->actor.world.pos.z;
        f32 sp28 = sqrtf(SQ(selfDistX) + SQ(selfDistZ)) / sp34;
        s32 sp24 = (cue->endFrame - play->csCtx.curFrame) + 1;

        arg4 = atans_table(selfDistZ, selfDistX);

        if (arg5 == 1) {
            f32 distX = cue->endPos.x - cue->startPos.x;
            f32 distZ = cue->endPos.z - cue->startPos.z;
            s32 temp = (((sqrtf(SQ(distX) + SQ(distZ)) / sp34) / (cue->endFrame - cue->startFrame)) / 1.5f) * 4.0f;

            if (temp >= sp24) {
                arg4 = this->actor.shape.rot.y;
                arg3 = 0.0f;
            } else {
                arg3 = sp28 / ((sp24 - temp) + 1);
            }
        } else {
            arg3 = sp28 / sp24;
        }
    }

    this->stateFlags2 |= PLAYER_STATE2_5;
    run_anime_set(this, play);
    run_control(this, arg3, arg4);

    if ((arg3 == 0.0f) && (this->speedXZ == 0.0f)) {
        to_wait_from_run_anime_set(this, play);
    }

    return 0;
}

s32 aim_demo_run_control(PlayState* play, Player* this, f32* arg2, s32 arg3) {
    f32 dx = this->unk_450.x - this->actor.world.pos.x;
    f32 dz = this->unk_450.z - this->actor.world.pos.z;
    s32 sp2C = sqrtf(SQ(dx) + SQ(dz));
    s16 yaw = search_position_angleY(&this->actor.world.pos, &this->unk_450);

    if (sp2C < arg3) {
        *arg2 = 0.0f;
        yaw = this->actor.shape.rot.y;
    }

    if (demo_run_control(play, this, NULL, *arg2, yaw, 2)) {
        return 0;
    }

    return sp2C;
}

s32 restart_data_save(PlayState* play, s32 arg1) {
    if (arg1 == 0) {
        Game_play_restart_set(play, RESPAWN_MODE_DOWN,
                               PLAYER_PARAMS(PLAYER_START_MODE_IDLE, PLAYER_START_BG_CAM_DEFAULT));
    }
    z_common_data.respawn[RESPAWN_MODE_DOWN].data = 0;
    return arg1;
}

void move_shutter_open(Player* this, PlayState* play) {
    if (!to_look_set_check(this, play)) {
        if (this->av2.actionVar2 == 0) {
            Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

            if (DECR(this->doorTimer) == 0) {
                this->speedXZ = 0.1f;
                this->av2.actionVar2 = 1;
            }
        } else if (this->av1.actionVar1 == 0) {
            f32 sp3C = 5.0f * environment_speed_ratio;
            s32 temp = aim_demo_run_control(play, this, &sp3C, -1);

            if (temp < 30) {
                this->av1.actionVar1 = 1;
                this->stateFlags1 |= PLAYER_STATE1_29;

                this->unk_450.x = this->unk_45C.x;
                this->unk_450.z = this->unk_45C.z;
            }
        } else {
            f32 sp34 = 5.0f;
            s32 sp30 = 20;
            s32 sp2C;

            if (this->stateFlags1 & PLAYER_STATE1_0) {
                sp34 = z_common_data.entranceSpeed;

                if (slide_power_flag != CONVEYOR_SPEED_DISABLED) {
                    this->unk_450.x = (sin_s(slide_power_angle) * 400.0f) + this->actor.world.pos.x;
                    this->unk_450.z = (cos_s(slide_power_angle) * 400.0f) + this->actor.world.pos.z;
                }
            } else if (this->av2.actionVar2 < 0) {
                this->av2.actionVar2++;

                sp34 = z_common_data.entranceSpeed;
                sp30 = -1;
            }

            sp2C = aim_demo_run_control(play, this, &sp34, sp30);

            if ((this->av2.actionVar2 == 0) ||
                ((sp2C == 0) && (this->speedXZ == 0.0f) &&
                 (Gama_play_get_camera(play, CAM_ID_MAIN)->stateFlags & CAM_STATE_CAM_FUNC_FINISH))) {

                restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));
                restart_data_save(play, z_common_data.respawn[RESPAWN_MODE_DOWN].data);

                if (!to_talk_check(this, play)) {
                    to_wait_run_check_set(this, play);
                }
            }
        }
    }

    if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        uperbody_action_check(this, play);
    }
}

void move_door_open(Player* this, PlayState* play) {
    s32 sp2C;

    this->stateFlags2 |= PLAYER_STATE2_5;
    sp2C = Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    uperbody_action_check(this, play);

    if (sp2C) {
        if (this->av2.actionVar2 == 0) {
            if (DECR(this->doorTimer) == 0) {
                this->av2.actionVar2 = 1;
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        } else {
            to_wait_no_morf_set(this, play);
            if (play->roomCtx.prevRoom.num >= 0) {
                Room_Info_old_room_clear(play, &play->roomCtx);
            }
            restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));
            Game_play_restart_set(play, RESPAWN_MODE_DOWN,
                                   PLAYER_PARAMS(PLAYER_START_MODE_IDLE, PLAYER_START_BG_CAM_DEFAULT));
        }
        return;
    }

    if (!(this->stateFlags1 & PLAYER_STATE1_29) && Skeleton_Info_Rom_frame_check(&this->skelAnime, 15.0f)) {
        play->func_11D54(this, play);
    }
}

static void move_carry(Player* this, PlayState* play) {
    ground_breaking_speedF(this);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        to_wait_set_check(this, play);
        to_uperbody_carry_init(this, play);
    } else if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 4.0f)) {
        Actor* interactRangeActor = this->interactRangeActor;

        if (!carry_cancel_check(play, this, interactRangeActor)) {
            this->heldActor = interactRangeActor;
            this->actor.child = interactRangeActor;
            interactRangeActor->parent = &this->actor;
            interactRangeActor->bgCheckFlags &=
                ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_GROUND_LEAVE | BGCHECKFLAG_WALL |
                  BGCHECKFLAG_CEILING | BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH | BGCHECKFLAG_GROUND_STRICT);
            this->unk_3BC.y = interactRangeActor->shape.rot.y - this->actor.shape.rot.y;
        }
    } else {
        chase_angle(&this->unk_3BC.y, 0, 4000);
    }
}

void move_heavy_carry(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_VO_LI_SWORD_L, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 49) },
        { NA_SE_VO_LI_SWORD_N, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 230) },
    };

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime) && (this->av2.actionVar2++ > 20)) {
        if (!to_look_set_check(this, play)) {
            to_wait_set_check_anim(this, &gPlayerAnim_link_normal_heavy_carry_end, play);
        }
    } else if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 41.0f)) {
        BgHeavyBlock* heavyBlock = (BgHeavyBlock*)this->interactRangeActor;

        this->heldActor = &heavyBlock->dyna.actor;
        this->actor.child = &heavyBlock->dyna.actor;
        heavyBlock->dyna.actor.parent = &this->actor;
        Actor_search_position_project_distanceXZ(&heavyBlock->dyna.actor, &heavyBlock->unk_164, &this->leftHandPos);
    } else if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 229.0f)) {
        Actor* heldActor = this->heldActor;

        heldActor->speed = sin_s(heldActor->shape.rot.x) * 40.0f;
        heldActor->velocity.y = cos_s(heldActor->shape.rot.x) * 40.0f;
        heldActor->gravity = -2.0f;
        heldActor->minVelocityY = -30.0f;
        l_hand_child_cancel(play, this);
    } else {
        player_anime_check_SE_set(this, SE_set_status);
    }
}

void move_silver_carry(Player* this, PlayState* play) {
    ground_breaking_speedF(this);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_init_standard_repeat(play, this, &gPlayerAnim_link_silver_wait);
        this->av2.actionVar2 = 1;
    } else if (this->av2.actionVar2 == 0) {
        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 27.0f)) {
            Actor* interactRangeActor = this->interactRangeActor;

            this->heldActor = interactRangeActor;
            this->actor.child = interactRangeActor;
            interactRangeActor->parent = &this->actor;
        } else if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 25.0f)) {
            player_voice_SE_set(this, NA_SE_VO_LI_SWORD_L);
        }
    } else if (CHECK_BTN_ANY(pad->press.button, BTN_A | BTN_B | BTN_CLEFT | BTN_CDOWN | BTN_CRIGHT)) {
        Player_actor_set_process(play, this, move_silver_throw, 1);
        anime_init_standard_stop(play, this, &gPlayerAnim_link_silver_throw);
    }
}

void move_silver_throw(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        to_wait_set_check(this, play);
        return;
    }

    if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 6.0f)) {
        Actor* heldActor = this->heldActor;

        heldActor->world.rot.y = this->actor.shape.rot.y;
        heldActor->speed = 10.0f;
        heldActor->velocity.y = 20.0f;
        Player_actor_uperbody_process_init(play, this);
        player_SE_set(this, NA_SE_PL_THROW);
        player_voice_SE_set(this, NA_SE_VO_LI_SWORD_N);
    }
}

void move_no_carry(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_init_standard_repeat(play, this, &gPlayerAnim_link_normal_nocarry_free_wait);
        this->av2.actionVar2 = 15;
        return;
    }

    if (this->av2.actionVar2 != 0) {
        this->av2.actionVar2--;
        if (this->av2.actionVar2 == 0) {
            to_wait_set_check_anim(this, &gPlayerAnim_link_normal_nocarry_free_end, play);
            this->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
            player_voice_SE_set(this, NA_SE_VO_LI_DAMAGE_S);
        }
    }
}

void move_put(Player* this, PlayState* play) {
    ground_breaking_speedF(this);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        to_wait_set_check(this, play);
        return;
    }

    if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 4.0f)) {
        Actor* heldActor = this->heldActor;

        if (!carry_cancel_check(play, this, heldActor)) {
            heldActor->velocity.y = 0.0f;
            heldActor->speed = 0.0f;
            Player_actor_uperbody_process_init(play, this);
            if (heldActor->id == ACTOR_EN_BOM_CHU) {
                to_subject_check(this, play);
            }
        }
    }
}

static void move_throw(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    ground_breaking_speedF(this);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime) ||
        ((this->skelAnime.curFrame >= 8.0f) &&
         stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_CURVED, play))) {
        to_wait_set_check(this, play);
        return;
    }

    if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 3.0f)) {
        carry_throw_set(play, this, this->speedXZ + 8.0f, 12.0f);
    }
}

static ColliderCylinderInit PlayerOcInfoData = {
    {
        COL_MATERIAL_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 12, 60, 0, { 0, 0, 0 } },
};

static ColliderQuadInit PlayerATSwordInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000100, 0x00, 0x01 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderQuadInit PlayerACShieldInfoData = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_HARD | AC_TYPE_ENEMY,
        OC1_NONE,
        OC2_TYPE_PLAYER,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0xDFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

void player_actor_move_non(Actor* thisx, PlayState* play) {
}

void to_non_set(PlayState* play, Player* this) {
    this->actor.update = player_actor_move_non;
    this->actor.draw = NULL;
}

void to_from_warp_set(PlayState* play, Player* this) {
    Player_actor_set_process(play, this, move_from_warp, 0);

    if ((play->sceneId == SCENE_LAKE_HYLIA) && IS_CUTSCENE_LAYER) {
        this->av1.isLakeHyliaCs = true;
    }

    this->stateFlags1 |= PLAYER_STATE1_29;
    Skeleton_Info_Rom_init(play, &this->skelAnime, &gPlayerAnim_link_okarina_warp_goal, 2.0f / 3.0f, 0.0f, 24.0f,
                         ANIMMODE_ONCE, 0.0f);

    // Start high up in the air
    this->actor.world.pos.y += 800.0f;
}

/**
 * Put the sword item in hand. If `playSfx` is true, the sword unsheathing sound will play.
 * Kokiri Sword or Master Sword is used depending on age.
 *
 * Note: This will not play an animation, the sword instantly appears in hand.
 *       It is expected that this function is called while an appropriate animation
 *       is already playing, for example in a cutscene.
 */
void to_sword_item_set(PlayState* play, Player* this, s32 playSfx) {
    static u8 sword_item_data[] = { ITEM_SWORD_MASTER, ITEM_SWORD_KOKIRI };
    s32 swordItemId = sword_item_data[(void)0, z_common_data.save.linkAge];
    s32 swordItemAction = item_type_data[swordItemId];

    hook_cancel_check(this);
    l_hand_child_cancel(play, this);

    this->heldItemId = swordItemId;
    this->nextModelGroup = item_shape_type_set(this, swordItemAction);

    ability_A_item_change(play, this, swordItemAction);
    Player_actor_uperbody_process_init(play, this);

    if (playSfx) {
        player_SE_set(this, NA_SE_IT_SWORD_PICKOUT);
    }
}

void to_m_sword_end_set(PlayState* play, Player* this) {
    static Vec3f m_sword_end_init_pos = { -1.0f, 69.0f, 20.0f };

    Player_actor_set_process(play, this, move_m_sword_end, 0);
    this->stateFlags1 |= PLAYER_STATE1_29;

    xyz_t_move(&this->actor.world.pos, &m_sword_end_init_pos);
    this->yaw = this->actor.shape.rot.y = -0x8000;

    // The start frame and end frame are both set to 0 so that that the animation is frozen.
    // `move_m_sword_end` will play the animation after `animDelayTimer` completes.
    Skeleton_Info_Rom_init(play, &this->skelAnime, this->ageProperties->timeTravelEndAnim, 2.0f / 3.0f, 0.0f, 0.0f,
                         ANIMMODE_ONCE, 0.0f);
    anime_move_init(play, this,
                             PLAYER_ANIM_MOVEMENT_RESET_BY_AGE | ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y |
                                 ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                 ANIM_FLAG_OVERRIDE_MOVEMENT);

    if (LINK_IS_ADULT) {
        to_sword_item_set(play, this, false);
    }

    this->av2.animDelayTimer = 20;
}

void to_door_start_set(PlayState* play, Player* this) {
    Player_actor_set_process(play, this, move_door_start, 0);
    anime_move_init(play, this,
                             ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_ENABLE_MOVEMENT |
                                 ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);
}

void to_jump_start_set(PlayState* play, Player* this) {
    to_jump_set(this, &gPlayerAnim_link_normal_jump, 12.0f, play);
    Player_actor_set_process(play, this, move_jump_start, 0);
    this->stateFlags1 |= PLAYER_STATE1_29;
    this->fallStartHeight = this->actor.world.pos.y;
    makeOnepointDemo(play, 5110, 40, &this->actor, CAM_ID_MAIN);
}

void to_damage_start_set(PlayState* play, Player* this) {
    damage_set(play, this, PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE, 2.0f, 2.0f, this->actor.shape.rot.y + 0x8000, 0);
}

void to_from_okarina_warp_set(PlayState* play, Player* this) {
    Player_actor_set_process(play, this, move_from_okarina_warp, 0);
    this->actor.draw = NULL; // Start invisible
    this->stateFlags1 |= PLAYER_STATE1_29;
}

Actor* magic_effect_set(PlayState* play, Player* this, s32 spell) {
    static s16 magic_profile[] = { ACTOR_MAGIC_WIND, ACTOR_MAGIC_DARK, ACTOR_MAGIC_FIRE };

    return Actor_info_make_actor(&play->actorCtx, play, magic_profile[spell], this->actor.world.pos.x,
                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
}

void to_from_magic_warp_set(PlayState* play, Player* this) {
    this->actor.draw = NULL; // Start invisible
    Player_actor_set_process(play, this, move_from_magic_window, 0);
    this->stateFlags1 |= PLAYER_STATE1_29;
}

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 500, ICHAIN_STOP),
};

static EffectBlureInit2 blure_info = {
    0, 8, 0, { 255, 255, 255, 255 }, { 255, 255, 255, 64 }, { 255, 255, 255, 0 }, { 255, 255, 255, 0 }, 4,
    0, 2, 0, { 0, 0, 0, 0 },         { 0, 0, 0, 0 },
};

static Vec3s anime_move_base_translate = { -57, 3377, 0 };

void Ext_Player_actor_init(Player* this, PlayState* play, FlexSkeletonHeader* skelHeader) {
    this->ageProperties = &link_age_status_data[z_common_data.save.linkAge];
    ValueSet_process(&this->actor, value_init);
    this->meleeWeaponEffectIndex = TOTAL_EFFECT_COUNT;
    this->yaw = this->actor.world.rot.y;
    Player_actor_uperbody_process_init(play, this);

    Skeleton_Info_Rom_SV_ct(play, &this->skelAnime, skelHeader, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_wait, this->modelAnimType),
                       9, this->jointTable, this->morphTable, PLAYER_LIMB_MAX);
    this->skelAnime.baseTransl = anime_move_base_translate;
    Skeleton_Info_Rom_SV_ct(play, &this->upperSkelAnime, skelHeader, wait_anime_select(this), 9, this->upperJointTable,
                       this->upperMorphTable, PLAYER_LIMB_MAX);
    this->upperSkelAnime.baseTransl = anime_move_base_translate;

    EffectAdd(play, &this->meleeWeaponEffectIndex, EFFECT_BLURE2, 0, 0, &blure_info);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_foot, this->ageProperties->unk_04);
    this->subCamId = CAM_ID_NONE;

    ClObjPipe_ct(play, &this->cylinder);
    ClObjPipe_set5(play, &this->cylinder, &this->actor, &PlayerOcInfoData);
    ClObjSwrd_ct(play, &this->meleeWeaponQuads[0]);
    ClObjSwrd_set5(play, &this->meleeWeaponQuads[0], &this->actor, &PlayerATSwordInfoData);
    ClObjSwrd_ct(play, &this->meleeWeaponQuads[1]);
    ClObjSwrd_set5(play, &this->meleeWeaponQuads[1], &this->actor, &PlayerATSwordInfoData);
    ClObjSwrd_ct(play, &this->shieldQuad);
    ClObjSwrd_set5(play, &this->shieldQuad, &this->actor, &PlayerACShieldInfoData);
}

static void (*start_mode_set[PLAYER_START_MODE_MAX])(PlayState* play, Player* this) = {
    to_non_set,         // PLAYER_START_MODE_NOTHING
    to_m_sword_end_set,      // PLAYER_START_MODE_TIME_TRAVEL
    to_from_warp_set,        // PLAYER_START_MODE_BLUE_WARP
    to_door_start_set,            // PLAYER_START_MODE_DOOR
    to_jump_start_set,          // PLAYER_START_MODE_GROTTO
    to_from_okarina_warp_set,        // PLAYER_START_MODE_WARP_SONG
    to_from_magic_warp_set,     // PLAYER_START_MODE_FARORES_WIND
    to_damage_start_set,     // PLAYER_START_MODE_KNOCKED_OVER
    to_demo_run_start_set, // PLAYER_START_MODE_UNUSED_8
    to_demo_run_start_set, // PLAYER_START_MODE_UNUSED_9
    to_demo_run_start_set, // PLAYER_START_MODE_UNUSED_10
    to_demo_run_start_set, // PLAYER_START_MODE_UNUSED_11
    to_demo_run_start_set, // PLAYER_START_MODE_UNUSED_12
    to_demo_wait_start_set,            // PLAYER_START_MODE_IDLE
    to_demo_run_start_set, // PLAYER_START_MODE_MOVE_FORWARD_SLOW
    to_run_start_set,     // PLAYER_START_MODE_MOVE_FORWARD
};

void Player_actor_ct(Actor* thisx, PlayState* play2) {
    Player* this = (Player*)thisx;
    PlayState* play = play2;
    SceneTableEntry* scene = play->loadedScene;
    u32 titleFileSize;
    s32 startMode;
    s32 respawnFlag;
    s32 respawnMode;

    play->shootingGalleryStatus = play->bombchuBowlingStatus = 0;

    play->playerInit = Ext_Player_actor_init;
    play->playerUpdate = Ext_Player_actor_proc;
    play->isPlayerDroppingFish = Ext_player_fish_out_check;
    play->startPlayerFishing = Ext_to_fishing_set;
    play->grabPlayer = Ext_to_re_dead_attack_set;
    play->tryPlayerCsAction = Ext_player_demo_set;
    play->func_11D54 = Ext_to_wait_set;
    play->damagePlayer = Ext_to_damage_set;
    play->talkWithPlayer = Ext_player_to_talk_set;

    thisx->room = -1;
    this->ageProperties = &link_age_status_data[z_common_data.save.linkAge];
    this->itemAction = this->heldItemAction = -1;
    this->heldItemId = ITEM_NONE;

    player_item_on(play, this, ITEM_NONE);

    player_ability_set(play, this);
    this->prevBoots = this->currentBoots;

    Ext_Player_actor_init(this, play, skeleton_list[((void)0, z_common_data.save.linkAge)]);

    // `giObjectSegment` is used for both "get item" objects and title cards. The maximum size for
    // get item objects is 0x2000 (see the assert in to_get_item), and the maximum size for
    // title cards is 0x1000 * LANGUAGE_MAX since each title card image includes all languages.
    this->giObjectSegment =
        (void*)(((uintptr_t)ZELDA_ARENA_MALLOC(0x1000 * LANGUAGE_MAX + 8, "../z_player.c", 17175) + 8) & ~0xF);

    respawnFlag = z_common_data.respawnFlag;

    if (respawnFlag != 0) {
        if (respawnFlag == -3) {
            thisx->params = z_common_data.respawn[RESPAWN_MODE_RETURN].playerParams;
        } else {
            if ((respawnFlag == 1) || (respawnFlag == -1)) {
                this->unk_A86 = -2;
            }

            if (respawnFlag < 0) {
                respawnMode = RESPAWN_MODE_DOWN;
            } else {
                respawnMode = respawnFlag - 1;
                xyz_t_move(&thisx->world.pos, &z_common_data.respawn[respawnMode].pos);
                xyz_t_move(&thisx->home.pos, &thisx->world.pos);
                xyz_t_move(&thisx->prevPos, &thisx->world.pos);
                this->fallStartHeight = thisx->world.pos.y;
                this->yaw = thisx->shape.rot.y = z_common_data.respawn[respawnMode].yaw;
                thisx->params = z_common_data.respawn[respawnMode].playerParams;
            }

            play->actorCtx.flags.tempSwch = z_common_data.respawn[respawnMode].tempSwchFlags & 0xFFFFFF;
            play->actorCtx.flags.tempCollect = z_common_data.respawn[respawnMode].tempCollectFlags;
        }
    }

    if ((respawnFlag == 0) || (respawnFlag < -1)) {
        titleFileSize = scene->titleFile.vromEnd - scene->titleFile.vromStart;

        if ((titleFileSize != 0) && z_common_data.showTitleCard) {
            if (!IS_CUTSCENE_LAYER &&
                (scene_status_data[((void)0, z_common_data.save.entranceIndex) + ((void)0, z_common_data.sceneLayer)].field &
                 ENTRANCE_INFO_DISPLAY_TITLE_CARD_FLAG) &&
                ((play->sceneId != SCENE_DODONGOS_CAVERN) || GET_EVENTCHKINF(EVENTCHKINF_B0)) &&
#if OOT_VERSION < PAL_1_0
                ((play->sceneId != SCENE_BOMBCHU_BOWLING_ALLEY) || GET_EVENTCHKINF(EVENTCHKINF_25))
#else
                ((play->sceneId != SCENE_BOMBCHU_SHOP) || GET_EVENTCHKINF(EVENTCHKINF_25))
#endif
            ) {
                Actor_Name_Disp_Set2(play, &play->actorCtx.titleCtx, this->giObjectSegment, 160, 120, 144, 24, 20);
            }
        }

        z_common_data.showTitleCard = true;
    }

    if (restart_data_save(play, (respawnFlag == 2) ? 1 : 0) == 0) {
        z_common_data.respawn[RESPAWN_MODE_DOWN].playerParams =
            PLAYER_PARAMS(PLAYER_START_MODE_IDLE, PLAYER_GET_START_BG_CAM_INDEX(thisx));
    }

    z_common_data.respawn[RESPAWN_MODE_DOWN].data = 1;

    if (play->sceneId <= SCENE_INSIDE_GANONS_CASTLE_COLLAPSE) {
        z_common_data.save.info.infTable[INFTABLE_INDEX_1AX] |= check_bit[play->sceneId];
    }

    startMode = PLAYER_GET_START_MODE(thisx);

    if ((startMode == PLAYER_START_MODE_WARP_SONG) || (startMode == PLAYER_START_MODE_FARORES_WIND)) {
        if (z_common_data.save.cutsceneIndex >= 0xFFF0) {
            startMode = PLAYER_START_MODE_IDLE;
        }
    }

    start_mode_set[startMode](play, this);

    if (startMode != PLAYER_START_MODE_NOTHING) {
        if ((z_common_data.gameMode == GAMEMODE_NORMAL) || (z_common_data.gameMode == GAMEMODE_END_CREDITS)) {
            static Vec3f elf_init_set_offset = { 0.0f, 50.0f, 0.0f };

            this->naviActor = make_elf(play, this, &thisx->world.pos, &elf_init_set_offset, FAIRY_NAVI);

            if (z_common_data.dogParams != 0) {
                z_common_data.dogParams |= 0x8000;
            }
        }
    }

    if (z_common_data.nayrusLoveTimer != 0) {
        z_common_data.magicState = MAGIC_STATE_METER_FLASH_1;
        magic_effect_set(play, this, 1);
        this->stateFlags3 &= ~PLAYER_STATE3_RESTORE_NAYRUS_LOVE;
    }

    if (z_common_data.entranceSound != 0) {
        Actor_SE_set(&this->actor, ((void)0, z_common_data.entranceSound));
        z_common_data.entranceSound = 0;
    }

    player_position_hold(play);
    MREG(64) = 0;
}

void look_anime_angle_reset(s16* pValue) {
    s16 step;

    step = ABS(*pValue) * 100.0f / 1000.0f;
    step = CLAMP(step, 400, 4000);

    chase_angle(pValue, 0, step);
}

void look_anime_angle_check(Player* this) {
    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_FOCUS_Y)) {
        s16 diff = this->actor.focus.rot.y - this->actor.shape.rot.y;

        look_anime_angle_reset(&diff);
        this->actor.focus.rot.y = this->actor.shape.rot.y + diff;
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_FOCUS_X)) {
        look_anime_angle_reset(&this->actor.focus.rot.x);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_HEAD_X)) {
        look_anime_angle_reset(&this->headLimbRot.x);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_UPPER_X)) {
        look_anime_angle_reset(&this->upperLimbRot.x);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_FOCUS_Z)) {
        look_anime_angle_reset(&this->actor.focus.rot.z);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_HEAD_Y)) {
        look_anime_angle_reset(&this->headLimbRot.y);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_HEAD_Z)) {
        look_anime_angle_reset(&this->headLimbRot.z);
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_UPPER_Y)) {
        if (this->upperLimbYawSecondary != 0) {
            look_anime_angle_reset(&this->upperLimbYawSecondary);
        } else {
            look_anime_angle_reset(&this->upperLimbRot.y);
        }
    }

    if (!(this->unk_6AE_rotFlags & UNK6AE_ROT_UPPER_Z)) {
        look_anime_angle_reset(&this->upperLimbRot.z);
    }

    this->unk_6AE_rotFlags = 0;
}

static f32 swim_deep_y[] = { 120.0f, 240.0f, 360.0f };

/**
 * Updates the two main interface elements that player is responsible for:
 *     - Do Action label on the A button
 *     - Navi C-up icon for hints
 */
void do_check(PlayState* play, Player* this) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_NONE) && (this->actor.category == ACTORCAT_PLAYER)) {
        Actor* heldActor = this->heldActor;
        Actor* interactRangeActor = this->interactRangeActor;
        s32 sp24;
        s32 controlStickDirection = this->controlStickDirections[this->controlStickDataIndex];
        s32 sp1C = swim_check(this);
        s32 doAction = DO_ACTION_NONE;

        if (!player_action_check(play, this)) {
            if (this->stateFlags1 & PLAYER_STATE1_20) {
                doAction = DO_ACTION_RETURN;
            } else if ((this->heldItemAction == PLAYER_IA_FISHING_POLE) && (this->unk_860 != 0)) {
                if (this->unk_860 == 2) {
                    doAction = DO_ACTION_REEL;
                }
            } else if ((move_okarina != this->actionFunc) && !(this->stateFlags2 & PLAYER_STATE2_CRAWLING)) {
                if ((this->doorType != PLAYER_DOORTYPE_NONE) &&
                    (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) ||
                     ((heldActor != NULL) && (heldActor->id == ACTOR_EN_RU1)))) {
                    doAction = DO_ACTION_OPEN;
                } else if ((!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) || (heldActor == NULL)) &&
                           (interactRangeActor != NULL) &&
                           ((!sp1C && (this->getItemId == GI_NONE)) ||
                            ((this->getItemId < 0) && !(this->stateFlags1 & PLAYER_STATE1_27)))) {
                    if (this->getItemId < 0) {
                        doAction = DO_ACTION_OPEN;
                    } else if ((interactRangeActor->id == ACTOR_BG_TOKI_SWD) && LINK_IS_ADULT) {
                        doAction = DO_ACTION_DROP;
                    } else {
                        doAction = DO_ACTION_GRAB;
                    }
                } else if (!sp1C && (this->stateFlags2 & PLAYER_STATE2_0)) {
                    doAction = DO_ACTION_GRAB;
                } else if ((this->stateFlags2 & PLAYER_STATE2_2) ||
                           (!(this->stateFlags1 & PLAYER_STATE1_23) && (this->rideActor != NULL))) {
                    doAction = DO_ACTION_CLIMB;
                } else if ((this->stateFlags1 & PLAYER_STATE1_23) && !EN_HORSE_CHECK_4((EnHorse*)this->rideActor) &&
                           (move_ride_horse_end != this->actionFunc)) {
                    if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
                        if (this->talkActor->category == ACTORCAT_NPC) {
                            doAction = DO_ACTION_SPEAK;
                        } else {
                            doAction = DO_ACTION_CHECK;
                        }
                    } else if (!player_bow_ready_check(this) && !(this->stateFlags1 & PLAYER_STATE1_20)) {
                        doAction = DO_ACTION_FASTER;
                    }
                } else if ((this->stateFlags2 & PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER) && (this->talkActor != NULL)) {
                    if (this->talkActor->category == ACTORCAT_NPC) {
                        doAction = DO_ACTION_SPEAK;
                    } else {
                        doAction = DO_ACTION_CHECK;
                    }
                } else if ((this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_21)) ||
                           ((this->stateFlags1 & PLAYER_STATE1_23) && (this->stateFlags2 & PLAYER_STATE2_22))) {
                    doAction = DO_ACTION_DOWN;
                } else if (this->stateFlags2 & PLAYER_STATE2_DO_ACTION_ENTER) {
                    doAction = DO_ACTION_ENTER;
                } else if ((this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (this->getItemId == GI_NONE) &&
                           (heldActor != NULL)) {
                    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (heldActor->id == ACTOR_EN_NIW)) {
                        if (!to_put_or_throw_check(this, heldActor)) {
                            doAction = DO_ACTION_DROP;
                        } else {
                            doAction = DO_ACTION_THROW;
                        }
                    }
                } else if (!(this->stateFlags1 & PLAYER_STATE1_27) && carry_ok_check(this) &&
                           (this->getItemId < GI_MAX)) {
                    doAction = DO_ACTION_GRAB;
                } else if (this->stateFlags2 & PLAYER_STATE2_11) {
                    static u8 do_swim_deep_no[] = { DO_ACTION_1, DO_ACTION_2, DO_ACTION_3, DO_ACTION_4,
                                                         DO_ACTION_5, DO_ACTION_6, DO_ACTION_7, DO_ACTION_8 };

                    sp24 = (swim_deep_y[CUR_UPG_VALUE(UPG_SCALE)] - this->actor.depthInWater) / 40.0f;
                    sp24 = CLAMP(sp24, 0, 7);
                    doAction = do_swim_deep_no[sp24];
                } else if (sp1C && !(this->stateFlags2 & PLAYER_STATE2_10)) {
                    doAction = DO_ACTION_DIVE;
                } else if (!sp1C && (!(this->stateFlags1 & PLAYER_STATE1_SHIELDING) || anchor_parallel_mode_check(this) ||
                                     !child_hyral_shield_check(this))) {
                    if ((!(this->stateFlags1 & PLAYER_STATE1_14) &&
                         (controlStickDirection <= PLAYER_STICK_DIR_FORWARD) &&
                         (anchor_mode_check(this) ||
                          ((ground_attribute != FLOOR_TYPE_7) && (parallel_mode_check(this) ||
                                                            ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) &&
                                                             !(this->stateFlags1 & PLAYER_STATE1_SHIELDING) &&
                                                             (controlStickDirection == PLAYER_STICK_DIR_FORWARD))))))) {
                        doAction = DO_ACTION_ATTACK;
                    } else if ((play->roomCtx.curRoom.type != ROOM_TYPE_INDOORS) && anchor_parallel_mode_check(this) &&
                               (controlStickDirection >= PLAYER_STICK_DIR_LEFT)) {
                        doAction = DO_ACTION_JUMP;
                    } else if ((this->heldItemAction >= PLAYER_IA_SWORD_MASTER) ||
                               ((this->stateFlags2 & PLAYER_STATE2_NAVI_ACTIVE) &&
                                (play->actorCtx.attention.naviHoverActor == NULL))) {
                        doAction = DO_ACTION_PUTAWAY;
                    }
                }
            }
        }

        if (doAction != DO_ACTION_PUTAWAY) {
            this->putAwayCooldownTimer = 20;
        } else if (this->putAwayCooldownTimer != 0) {
            // Replace the "Put Away" Do Action label with a blank label while
            // the cooldown timer is counting down
            doAction = DO_ACTION_NONE;
            this->putAwayCooldownTimer--;
        }

        do_action_point_set(play, doAction);

        if (this->stateFlags2 & PLAYER_STATE2_21) {
            if (this->focusActor != NULL) {
                do_action_navi_set(play, 0x1E);
            } else {
                do_action_navi_set(play, 0x1D);
            }
            do_action_navi_set(play, 0x1E);
        } else {
            do_action_navi_set(play, 0x1F);
        }
    }
}

/**
 * Updates state related to the Hover Boots.
 * Handles a special case where the Hover Boots are able to activate when standing on certain floor types even if the
 * player is standing on the ground.
 *
 * If the player is not on the ground, regardless of the usage of the Hover Boots, various floor related variables are
 * reset.
 *
 * @return true if not on the ground, false otherwise. Note this is independent of the Hover Boots state.
 */
s32 no_ground_hit_check(Player* this) {
    s32 canHoverOnGround;

    if ((this->currentBoots == PLAYER_BOOTS_HOVER) && (this->hoverBootsTimer != 0)) {
        this->hoverBootsTimer--;
    } else {
        this->hoverBootsTimer = 0;
    }

    canHoverOnGround =
        (this->currentBoots == PLAYER_BOOTS_HOVER) &&
        ((this->actor.depthInWater >= 0.0f) || (damage_ground_attribute_check(ground_attribute) >= 0) || desert_ground_attribute_check(ground_attribute));

    if (canHoverOnGround && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->hoverBootsTimer != 0)) {
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (!canHoverOnGround) {
            this->hoverBootsTimer = 19;
        }

        return false;
    } else {
        ground_attribute = FLOOR_TYPE_0;
        this->floorPitch = this->floorPitchAlt = ground_shape_angle_x = 0;

        return true;
    }
}

/**
 * Performs various tasks related to scene collision.
 *
 * This includes:
 * - Update BgCheckInfo, parameters adjusted due to various state flags
 * - Update floor type, floor property and floor sfx offset
 * - Update conveyor, reverb and light settings according to the current floor poly
 * - Handle exits and voids
 * - Update information relating to the "interact wall"
 * - Update information for ledge climbing
 * - Update hover boots
 * - Calculate floor poly angles
 *
 */
void BGcheck(PlayState* play, Player* this) {
    static Vec3f wall_recheck_offset = { 0.0f, 18.0f, 0.0f };
    u8 nextLedgeClimbType = PLAYER_LEDGE_CLIMB_NONE;
    CollisionPoly* floorPoly;
    Vec3f unusedWorldPos;
    f32 float0; // multi-purpose variable, see define names (fake match?)
    f32 float1; // multi-purpose variable, see define names (fake match?)
    f32 ceilingCheckHeight;
    u32 flags;

    ground_auto_jump_type = this->floorProperty;

#define vWallCheckRadius float0
#define vWallCheckHeight float1

    if (this->stateFlags2 & PLAYER_STATE2_CRAWLING) {
        vWallCheckRadius = 10.0f;
        vWallCheckHeight = 15.0f;
        ceilingCheckHeight = 30.0f;
    } else {
        vWallCheckRadius = this->ageProperties->wallCheckRadius;
        vWallCheckHeight = 26.0f;
        ceilingCheckHeight = this->ageProperties->ceilingCheckHeight;
    }

    if (this->stateFlags1 & (PLAYER_STATE1_29 | PLAYER_STATE1_31)) {
        if (this->stateFlags1 & PLAYER_STATE1_31) {
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
            flags = UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_5;
        } else if ((this->stateFlags1 & PLAYER_STATE1_0) && ((this->unk_A84 - (s32)this->actor.world.pos.y) >= 100)) {
            flags = UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_5;
        } else if (!(this->stateFlags1 & PLAYER_STATE1_0) &&
                   ((move_door_open == this->actionFunc) || (move_shutter_open == this->actionFunc))) {
            this->actor.bgCheckFlags &= ~(BGCHECKFLAG_WALL | BGCHECKFLAG_PLAYER_WALL_INTERACT);
            flags = UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_5;
        } else {
            flags = UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                    UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_5;
        }
    } else {
        flags = UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_5;
    }

    if (this->stateFlags3 & PLAYER_STATE3_0) {
        flags &= ~(UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    }

    if (flags & UPDBGCHECKINFO_FLAG_2) {
        this->stateFlags3 |= PLAYER_STATE3_4;
    }

    xyz_t_move(&unusedWorldPos, &this->actor.world.pos);

    Actor_BGcheck2(play, &this->actor, vWallCheckHeight, vWallCheckRadius, ceilingCheckHeight, flags);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) {
        this->actor.velocity.y = 0.0f;
    }

    to_ground_y = this->actor.world.pos.y - this->actor.floorHeight;
    slide_power_flag = CONVEYOR_SPEED_DISABLED;
    floorPoly = this->actor.floorPoly;

    if (floorPoly != NULL) {
        this->floorProperty = T_BGCheck_GroundAutoJumpType_ai(&play->colCtx, floorPoly, this->actor.floorBgId);
        this->prevFloorSfxOffset = this->floorSfxOffset;

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
            if (this->actor.depthInWater < 20.0f) {
                this->floorSfxOffset = SURFACE_SFX_OFFSET_WATER_SHALLOW;
            } else {
                this->floorSfxOffset = SURFACE_SFX_OFFSET_WATER_DEEP;
            }
        } else {
            if (this->stateFlags2 & PLAYER_STATE2_FORCE_SAND_FLOOR_SOUND) {
                this->floorSfxOffset = SURFACE_SFX_OFFSET_SAND;
            } else {
                this->floorSfxOffset = T_BGCheck_getSoundGroundLabel(&play->colCtx, floorPoly, this->actor.floorBgId);
            }
        }

        if (this->actor.category == ACTORCAT_PLAYER) {
            Na_SetAddEcho(T_BGCheck_getEcho_ai(&play->colCtx, floorPoly, this->actor.floorBgId));

            if (this->actor.floorBgId == BGCHECK_SCENE) {
                get_pol_color(
                    play, T_BGCheck_getKankyo_ai(&play->colCtx, floorPoly, this->actor.floorBgId));
            } else {
                MoveBG_setOverPlayerStatus_ai(&play->colCtx, this->actor.floorBgId);
            }
        }

        slide_power_flag = T_BGCheck_getSlidePowerIndex(&play->colCtx, floorPoly, this->actor.floorBgId);

        if (slide_power_flag != CONVEYOR_SPEED_DISABLED) {
            slide_power_type = T_BGCheck_checkBelt(&play->colCtx, floorPoly, this->actor.floorBgId);

            if ((!slide_power_type && (this->actor.depthInWater > 20.0f) &&
                 (this->currentBoots != PLAYER_BOOTS_IRON)) ||
                (slide_power_type && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND))) {
                slide_power_angle = CONVEYOR_DIRECTION_TO_BINANG(
                    T_BGCheck_getSlideAngle(&play->colCtx, floorPoly, this->actor.floorBgId));
            } else {
                slide_power_flag = CONVEYOR_SPEED_DISABLED;
            }
        }
    }

    scene_change_check(play, this, floorPoly, this->actor.floorBgId);

    this->actor.bgCheckFlags &= ~BGCHECKFLAG_PLAYER_WALL_INTERACT;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        CollisionPoly* wallPoly;
        s32 wallBgId;
        s16 yawDiff;
        s32 padding;

        wall_recheck_offset.y = 18.0f;
        wall_recheck_offset.z = this->ageProperties->wallCheckRadius + 10.0f;

        if (!(this->stateFlags2 & PLAYER_STATE2_CRAWLING) &&
            player_wall_BGcheck(play, this, &wall_recheck_offset, &wallPoly, &wallBgId,
                                     &wall_hit_pos)) {
            this->actor.bgCheckFlags |= BGCHECKFLAG_PLAYER_WALL_INTERACT;

            if (this->actor.wallPoly != wallPoly) {
                this->actor.wallPoly = wallPoly;
                this->actor.wallBgId = wallBgId;
                this->actor.wallYaw = atans_table(wallPoly->normal.z, wallPoly->normal.x);
            }
        }

        yawDiff = this->actor.shape.rot.y - (s16)(this->actor.wallYaw + 0x8000);
        wall_code = T_BGCheck_getWallCodeBit_ai(&play->colCtx, this->actor.wallPoly, this->actor.wallBgId);
        wall_distance_angle_y = ABS(yawDiff);

        yawDiff = this->yaw - (s16)(this->actor.wallYaw + 0x8000);
        action_wall_distance_angle_y = ABS(yawDiff);

#define vSpeedScale float0
#define vSpeedLimit float1

        vSpeedScale = action_wall_distance_angle_y * 0.00008f;

        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || vSpeedScale >= 1.0f) {
            this->unk_880 = R_RUN_SPEED_LIMIT / 100.0f;
        } else {
            vSpeedLimit = (R_RUN_SPEED_LIMIT / 100.0f * vSpeedScale);
            this->unk_880 = vSpeedLimit;

            if (vSpeedLimit < 0.1f) {
                this->unk_880 = 0.1f;
            }
        }

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_PLAYER_WALL_INTERACT) && (wall_distance_angle_y < 0x3000)) {
            CollisionPoly* wallPoly = this->actor.wallPoly;

            if (ABS(wallPoly->normal.y) < 600) {
                f32 wallPolyNormalX = COLPOLY_GET_NORMAL(wallPoly->normal.x);
                f32 wallPolyNormalY = COLPOLY_GET_NORMAL(wallPoly->normal.y);
                f32 wallPolyNormalZ = COLPOLY_GET_NORMAL(wallPoly->normal.z);
                f32 yDistToLedge;
                CollisionPoly* ledgeFloorPoly;
                CollisionPoly* poly;
                s32 bgId;
                Vec3f ledgeCheckPos;
                f32 ledgePosY;
                f32 ceillingPosY;
                s32 wallYawDiff;

                this->distToInteractWall = Math3DLengthPlaneAndPos(wallPolyNormalX, wallPolyNormalY, wallPolyNormalZ,
                                                                  wallPoly->dist, &this->actor.world.pos);

#define vLedgeCheckOffsetXZ float0

                vLedgeCheckOffsetXZ = this->distToInteractWall + 10.0f;

                ledgeCheckPos.x = this->actor.world.pos.x - (vLedgeCheckOffsetXZ * wallPolyNormalX);
                ledgeCheckPos.z = this->actor.world.pos.z - (vLedgeCheckOffsetXZ * wallPolyNormalZ);
                ledgeCheckPos.y = this->actor.world.pos.y + this->ageProperties->unk_0C;

                ledgePosY = T_BGCheck_ObjGroundCheck(&play->colCtx, &ledgeFloorPoly, &ledgeCheckPos);
                yDistToLedge = ledgePosY - this->actor.world.pos.y;
                this->yDistToLedge = yDistToLedge;

                if ((this->yDistToLedge < 18.0f) ||
                    T_BGCheck_ObjRoofCheck_aiac(&play->colCtx, &ceillingPosY, &this->actor.world.pos,
                                               (ledgePosY - this->actor.world.pos.y) + 20.0f, &poly, &bgId,
                                               &this->actor)) {
                    this->yDistToLedge = LEDGE_DIST_MAX;
                } else {
                    wall_recheck_offset.y = (ledgePosY + 5.0f) - this->actor.world.pos.y;

                    if (player_wall_BGcheck(play, this, &wall_recheck_offset, &poly, &bgId,
                                                 &wall_hit_pos) &&
                        (wallYawDiff = this->actor.wallYaw - atans_table(poly->normal.z, poly->normal.x),
                         ABS(wallYawDiff) < 0x4000) &&
                        !T_BGCheck_WallLadder_ai(&play->colCtx, poly, bgId)) {
                        this->yDistToLedge = LEDGE_DIST_MAX;
                    } else if (T_BGCheck_WallNotClib_ai(&play->colCtx, wallPoly, this->actor.wallBgId) == 0) {
                        if (this->ageProperties->unk_1C <= this->yDistToLedge) {
                            if (ABS(ledgeFloorPoly->normal.y) > 0x6D60) {
                                if (this->ageProperties->unk_14 <= this->yDistToLedge) {
                                    nextLedgeClimbType = PLAYER_LEDGE_CLIMB_4;
                                } else if (this->ageProperties->unk_18 <= this->yDistToLedge) {
                                    nextLedgeClimbType = PLAYER_LEDGE_CLIMB_3;
                                } else {
                                    nextLedgeClimbType = PLAYER_LEDGE_CLIMB_2;
                                }
                            }
                        } else {
                            nextLedgeClimbType = PLAYER_LEDGE_CLIMB_1;
                        }
                    }
                }
            }
        }
    } else {
        this->unk_880 = R_RUN_SPEED_LIMIT / 100.0f;
        this->ledgeClimbDelayTimer = 0;
        this->yDistToLedge = 0.0f;
    }

    if (nextLedgeClimbType == this->ledgeClimbType) {
        if ((this->speedXZ != 0.0f) && (this->ledgeClimbDelayTimer < 100)) {
            this->ledgeClimbDelayTimer++;
        }
    } else {
        this->ledgeClimbType = nextLedgeClimbType;
        this->ledgeClimbDelayTimer = 0;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        ground_attribute = T_BGCheck_getAttributeCode_ai(&play->colCtx, floorPoly, this->actor.floorBgId);

        if (!no_ground_hit_check(this)) {
            f32 floorPolyNormalX;
            f32 invFloorPolyNormalY;
            f32 floorPolyNormalZ;
            f32 sin;
            s32 pad2;
            f32 cos;
            s32 pad3;

            if (this->actor.floorBgId != BGCHECK_SCENE) {
                MoveBG_setRidePlayerStatus_ai(&play->colCtx, this->actor.floorBgId);
            }

            floorPolyNormalX = COLPOLY_GET_NORMAL(floorPoly->normal.x);
            invFloorPolyNormalY = 1.0f / COLPOLY_GET_NORMAL(floorPoly->normal.y);
            floorPolyNormalZ = COLPOLY_GET_NORMAL(floorPoly->normal.z);

            sin = sin_s(this->yaw);
            cos = cos_s(this->yaw);

            this->floorPitch =
                atans_table(1.0f, (-(floorPolyNormalX * sin) - (floorPolyNormalZ * cos)) * invFloorPolyNormalY);
            this->floorPitchAlt =
                atans_table(1.0f, (-(floorPolyNormalX * cos) - (floorPolyNormalZ * sin)) * invFloorPolyNormalY);

            sin = sin_s(this->actor.shape.rot.y);
            cos = cos_s(this->actor.shape.rot.y);

            ground_shape_angle_x =
                atans_table(1.0f, (-(floorPolyNormalX * sin) - (floorPolyNormalZ * cos)) * invFloorPolyNormalY);

            to_slope_slip_check(play, this, floorPoly);
        }
    } else {
        no_ground_hit_check(this);
    }

    if (this->prevFloorType == ground_attribute) {
        this->floorTypeTimer++;
    } else {
        this->prevFloorType = ground_attribute;
        this->floorTypeTimer = 0;
    }
}

void camera_mode_check(PlayState* play, Player* this) {
    u8 seqMode;
    s32 padding;
    Actor* focusActor;
    s32 camMode;

    if (this->actor.category == ACTORCAT_PLAYER) {
        seqMode = SEQ_MODE_DEFAULT;

        if (this->csAction != PLAYER_CSACTION_NONE) {
            changeCameraMode(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_MODE_NORMAL);
        } else if (!(this->stateFlags1 & PLAYER_STATE1_20)) {
            if ((this->actor.parent != NULL) && (this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT)) {
                camMode = CAM_MODE_HOOKSHOT_FLY;
                lockCamera(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_VIEW_TARGET, this->actor.parent);
            } else if (move_big_damage == this->actionFunc) {
                camMode = CAM_MODE_STILL;
            } else if (this->stateFlags2 & PLAYER_STATE2_8) {
                camMode = CAM_MODE_PUSH_PULL;
            } else if ((focusActor = this->focusActor) != NULL) {
                if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_TALK)) {
                    camMode = CAM_MODE_TALK;
                } else if (this->stateFlags1 & PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS) {
                    if (this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN) {
                        camMode = CAM_MODE_FOLLOW_BOOMERANG;
                    } else {
                        camMode = CAM_MODE_Z_TARGET_FRIENDLY;
                    }
                } else {
                    camMode = CAM_MODE_Z_TARGET_UNFRIENDLY;
                }
                lockCamera(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_VIEW_TARGET, focusActor);
            } else if (this->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK) {
                camMode = CAM_MODE_CHARGE;
            } else if (this->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN) {
                camMode = CAM_MODE_FOLLOW_BOOMERANG;
                lockCamera(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_VIEW_TARGET, this->boomerangActor);
            } else if (this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14)) {
                if (parallel_mode_check(this)) {
                    camMode = CAM_MODE_Z_LEDGE_HANG;
                } else {
                    camMode = CAM_MODE_LEDGE_HANG;
                }
            } else if (this->stateFlags1 & (PLAYER_STATE1_PARALLEL | PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE)) {
                if (player_bow_ready_check(this) || boom_ready_check(this)) {
                    camMode = CAM_MODE_Z_AIM;
                } else if (this->stateFlags1 & PLAYER_STATE1_21) {
                    camMode = CAM_MODE_Z_WALL_CLIMB;
                } else {
                    camMode = CAM_MODE_Z_PARALLEL;
                }
            } else if (this->stateFlags1 & (PLAYER_STATE1_18 | PLAYER_STATE1_21)) {
                if ((move_stepup100 == this->actionFunc) || (this->stateFlags1 & PLAYER_STATE1_21)) {
                    camMode = CAM_MODE_WALL_CLIMB;
                } else {
                    camMode = CAM_MODE_JUMP;
                }
            } else if (this->stateFlags1 & PLAYER_STATE1_19) {
                camMode = CAM_MODE_FREE_FALL;
            } else if ((this->meleeWeaponState != 0) && (this->meleeWeaponAnimation >= PLAYER_MWA_FORWARD_SLASH_1H) &&
                       (this->meleeWeaponAnimation < PLAYER_MWA_SPIN_ATTACK_1H)) {
                camMode = CAM_MODE_STILL;
            } else {
                camMode = CAM_MODE_NORMAL;
                if ((this->speedXZ == 0.0f) &&
                    (!(this->stateFlags1 & PLAYER_STATE1_23) || (this->rideActor->speed == 0.0f))) {
                    // not moving
                    seqMode = SEQ_MODE_STILL;
                }
            }

            changeCameraMode(Gama_play_get_camera(play, CAM_ID_MAIN), camMode);
        } else {
            // First person mode
            seqMode = SEQ_MODE_STILL;
        }

        if (play->actorCtx.attention.bgmEnemy != NULL) {
            seqMode = SEQ_MODE_ENEMY;
            Na_SetEnemyBgmDistance(sqrtf(play->actorCtx.attention.bgmEnemy->xyzDistToPlayerSq));
        }

        if (play->sceneId != SCENE_FISHING_POND) {
            Na_SetBgmPattern(seqMode);
        }
    }
}

static Vec3f fire_vec = { 0.0f, 0.5f, 0.0f };
static Vec3f fire_acc = { 0.0f, 0.5f, 0.0f };

static Color_RGBA8 fire_prim = { 255, 255, 100, 255 };
static Color_RGBA8 fire_env = { 255, 50, 0, 0 };

void stick_check(PlayState* play, Player* this) {
    f32 temp;

    if (this->unk_85C == 0.0f) {
        player_item_on(play, this, ITEM_NONE);
        return;
    }

    temp = 1.0f;
    if (DECR(this->unk_860) == 0) {
        item_count_vary(ITEM_DEKU_STICK, -1);
        this->unk_860 = 1;
        temp = 0.0f;
        this->unk_85C = temp;
    } else if (this->unk_860 > 200) {
        temp = (210 - this->unk_860) / 10.0f;
    } else if (this->unk_860 < 20) {
        temp = this->unk_860 / 20.0f;
        this->unk_85C = temp;
    }

    Effect_SS_Dust_sc_cl_co_ct(play, &this->meleeWeaponInfo[0].tip, &fire_vec, &fire_acc, &fire_prim, &fire_env,
                  temp * 200.0f, 0, 8);
}

void electric_effect_set(PlayState* play, Player* this) {
    Vec3f shockPos;
    Vec3f* randBodyPart;
    s32 shockScale;

    this->bodyShockTimer--;
    this->unk_892 += this->bodyShockTimer;

    if (this->unk_892 > 20) {
        shockScale = this->bodyShockTimer * 2;
        this->unk_892 -= 20;

        if (shockScale > 40) {
            shockScale = 40;
        }

        randBodyPart = this->bodyPartsPos + (s32)rnd_f(PLAYER_BODYPART_MAX - 0.1f);
        shockPos.x = (rnd_fx(5.0f) + randBodyPart->x) - this->actor.world.pos.x;
        shockPos.y = (rnd_fx(5.0f) + randBodyPart->y) - this->actor.world.pos.y;
        shockPos.z = (rnd_fx(5.0f) + randBodyPart->z) - this->actor.world.pos.z;

        Effect_fhg_flash_mini_ct(play, &this->actor, &shockPos, shockScale, FHGFLASH_SHOCK_PLAYER);
        Actor_player_level_SE_set(&this->actor, NA_SE_PL_SPARK - SFX_FLAG);
    }
}

void fire_effect_set(PlayState* play, Player* this) {
    s32 spawnedFlame;
    u8* timerPtr;
    s32 timerStep;
    f32 flameScale;
    f32 flameIntensity;
    s32 dmgCooldown;
    s32 i;
    s32 sp58;
    s32 sp54;

    if (this->currentTunic == PLAYER_TUNIC_GORON) {
        sp54 = 20;
    } else {
        sp54 = (s32)(this->speedXZ * 0.4f) + 1;
    }

    if (this->stateFlags2 & PLAYER_STATE2_3) {
        sp58 = 100;
    } else {
        sp58 = 0;
    }

    spawnedFlame = false;
    timerPtr = this->bodyFlameTimers;

    wood_shield_fire_check(this, play);

    for (i = 0; i < PLAYER_BODYPART_MAX; i++, timerPtr++) {
        timerStep = sp58 + sp54;

        if (*timerPtr <= timerStep) {
            *timerPtr = 0;
        } else {
            spawnedFlame = true;
            *timerPtr -= timerStep;

            if (*timerPtr > 20.0f) {
                flameIntensity = (*timerPtr - 20.0f) * 0.01f;
                flameScale = CLAMP(flameIntensity, 0.19999999f, 0.2f);
            } else {
                flameScale = *timerPtr * 0.01f;
            }

            flameIntensity = (*timerPtr - 25.0f) * 0.02f;
            flameIntensity = CLAMP(flameIntensity, 0.0f, 1.0f);
            fire_tail_effect_set2(play, flameScale, i, flameIntensity);
        }
    }

    if (spawnedFlame) {
        player_SE_set(this, NA_SE_EV_TORCH - SFX_FLAG);

        if (play->sceneId == SCENE_SPIRIT_TEMPLE_BOSS) {
            dmgCooldown = 0;
        } else {
            dmgCooldown = 7;
        }

        if ((dmgCooldown & play->gameplayFrames) == 0) {
            Ext_to_damage_set(play, -1);
        }
    } else {
        this->bodyIsBurning = false;
    }
}

void dowsing_check(Player* this) {
    if (CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY)) {
        f32 temp = 200000.0f - (this->closestSecretDistSq * 5.0f);

        if (temp < 0.0f) {
            temp = 0.0f;
        }

        this->unk_6A0 += temp;
        if (this->unk_6A0 > 4000000.0f) {
            this->unk_6A0 = 0.0f;
            player_rumble_entry(this, 120, 20, 10, 0);
        }
    }
}

static s8 tool_demo_mode_data[PLAYER_CUEID_MAX] = {
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_NONE
    PLAYER_CSACTION_3,    // PLAYER_CUEID_1
    PLAYER_CSACTION_3,    // PLAYER_CUEID_2
    PLAYER_CSACTION_5,    // PLAYER_CUEID_3
    PLAYER_CSACTION_4,    // PLAYER_CUEID_4
    PLAYER_CSACTION_8,    // PLAYER_CUEID_5
    PLAYER_CSACTION_9,    // PLAYER_CUEID_6
    PLAYER_CSACTION_13,   // PLAYER_CUEID_7
    PLAYER_CSACTION_14,   // PLAYER_CUEID_8
    PLAYER_CSACTION_15,   // PLAYER_CUEID_9
    PLAYER_CSACTION_16,   // PLAYER_CUEID_10
    PLAYER_CSACTION_17,   // PLAYER_CUEID_11
    PLAYER_CSACTION_18,   // PLAYER_CUEID_12
    -PLAYER_CSACTION_22,  // PLAYER_CUEID_13
    PLAYER_CSACTION_23,   // PLAYER_CUEID_14
    PLAYER_CSACTION_24,   // PLAYER_CUEID_15
    PLAYER_CSACTION_25,   // PLAYER_CUEID_16
    PLAYER_CSACTION_26,   // PLAYER_CUEID_17
    PLAYER_CSACTION_27,   // PLAYER_CUEID_18
    PLAYER_CSACTION_28,   // PLAYER_CUEID_19
    PLAYER_CSACTION_29,   // PLAYER_CUEID_20
    PLAYER_CSACTION_31,   // PLAYER_CUEID_21
    PLAYER_CSACTION_32,   // PLAYER_CUEID_22
    PLAYER_CSACTION_33,   // PLAYER_CUEID_23
    PLAYER_CSACTION_34,   // PLAYER_CUEID_24
    -PLAYER_CSACTION_35,  // PLAYER_CUEID_25
    PLAYER_CSACTION_30,   // PLAYER_CUEID_26
    PLAYER_CSACTION_36,   // PLAYER_CUEID_27
    PLAYER_CSACTION_38,   // PLAYER_CUEID_28
    -PLAYER_CSACTION_39,  // PLAYER_CUEID_29
    -PLAYER_CSACTION_40,  // PLAYER_CUEID_30
    -PLAYER_CSACTION_41,  // PLAYER_CUEID_31
    PLAYER_CSACTION_42,   // PLAYER_CUEID_32
    PLAYER_CSACTION_43,   // PLAYER_CUEID_33
    PLAYER_CSACTION_45,   // PLAYER_CUEID_34
    PLAYER_CSACTION_46,   // PLAYER_CUEID_35
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_36
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_37
    PLAYER_CSACTION_NONE, // PLAYER_CUEID_38
    PLAYER_CSACTION_67,   // PLAYER_CUEID_39
    PLAYER_CSACTION_48,   // PLAYER_CUEID_40
    PLAYER_CSACTION_47,   // PLAYER_CUEID_41
    -PLAYER_CSACTION_50,  // PLAYER_CUEID_42
    PLAYER_CSACTION_51,   // PLAYER_CUEID_43
    -PLAYER_CSACTION_52,  // PLAYER_CUEID_44
    -PLAYER_CSACTION_53,  // PLAYER_CUEID_45
    PLAYER_CSACTION_54,   // PLAYER_CUEID_46
    PLAYER_CSACTION_55,   // PLAYER_CUEID_47
    PLAYER_CSACTION_56,   // PLAYER_CUEID_48
    PLAYER_CSACTION_57,   // PLAYER_CUEID_49
    PLAYER_CSACTION_58,   // PLAYER_CUEID_50
    PLAYER_CSACTION_59,   // PLAYER_CUEID_51
    PLAYER_CSACTION_60,   // PLAYER_CUEID_52
    PLAYER_CSACTION_61,   // PLAYER_CUEID_53
    PLAYER_CSACTION_62,   // PLAYER_CUEID_54
    PLAYER_CSACTION_63,   // PLAYER_CUEID_55
    PLAYER_CSACTION_64,   // PLAYER_CUEID_56
    -PLAYER_CSACTION_65,  // PLAYER_CUEID_57
    -PLAYER_CSACTION_66,  // PLAYER_CUEID_58
    PLAYER_CSACTION_68,   // PLAYER_CUEID_59
    PLAYER_CSACTION_11,   // PLAYER_CUEID_60
    PLAYER_CSACTION_69,   // PLAYER_CUEID_61
    PLAYER_CSACTION_70,   // PLAYER_CUEID_62
    PLAYER_CSACTION_71,   // PLAYER_CUEID_63
    PLAYER_CSACTION_8,    // PLAYER_CUEID_64
    PLAYER_CSACTION_8,    // PLAYER_CUEID_65
    PLAYER_CSACTION_72,   // PLAYER_CUEID_66
    PLAYER_CSACTION_73,   // PLAYER_CUEID_67
    PLAYER_CSACTION_78,   // PLAYER_CUEID_68
    PLAYER_CSACTION_79,   // PLAYER_CUEID_69
    PLAYER_CSACTION_80,   // PLAYER_CUEID_70
    PLAYER_CSACTION_89,   // PLAYER_CUEID_71
    PLAYER_CSACTION_90,   // PLAYER_CUEID_72
    PLAYER_CSACTION_91,   // PLAYER_CUEID_73
    PLAYER_CSACTION_92,   // PLAYER_CUEID_74
    PLAYER_CSACTION_77,   // PLAYER_CUEID_75
    PLAYER_CSACTION_19,   // PLAYER_CUEID_76
    PLAYER_CSACTION_94,   // PLAYER_CUEID_77
};

static Vec3f scene_change_check_offset = { 0.0f, 0.0f, 200.0f };

static f32 water_slide_power_speedF[CONVEYOR_SPEED_MAX - 1] = {
    2.0f, // CONVEYOR_SPEED_SLOW
    4.0f, // CONVEYOR_SPEED_MEDIUM
    7.0f, // CONVEYOR_SPEED_FAST
};
static f32 belt_slide_power_speedF[CONVEYOR_SPEED_MAX - 1] = {
    0.5f, // CONVEYOR_SPEED_SLOW
    1.0f, // CONVEYOR_SPEED_MEDIUM
    3.0f, // CONVEYOR_SPEED_FAST
};

void Ext_Player_actor_proc(Player* this, PlayState* play, Input* input) {
    s32 padding;

    pad = input;

    if (this->unk_A86 < 0) {
        this->unk_A86++;
        if (this->unk_A86 == 0) {
            this->unk_A86 = 1;
            Na_StartSystemSe_F(NA_SE_OC_REVENGE);
        }
    }

    xyz_t_move(&this->actor.prevPos, &this->actor.home.pos);

    if (this->unk_A73 != 0) {
        this->unk_A73--;
    }

    if (this->textboxBtnCooldownTimer != 0) {
        this->textboxBtnCooldownTimer--;
    }

    if (this->unk_A87 != 0) {
        this->unk_A87--;
    }

    if (this->invincibilityTimer < 0) {
        this->invincibilityTimer++;
    } else if (this->invincibilityTimer > 0) {
        this->invincibilityTimer--;
    }

    if (this->unk_890 != 0) {
        this->unk_890--;
    }

    do_check(play, this);

    anchor_check(this, play);

    if ((this->heldItemAction == PLAYER_IA_DEKU_STICK) && (this->unk_860 != 0)) {
        stick_check(play, this);
    } else if ((this->heldItemAction == PLAYER_IA_FISHING_POLE) && (this->unk_860 < 0)) {
        this->unk_860++;
    }

    if (this->bodyShockTimer != 0) {
        electric_effect_set(play, this);
    }

    if (this->bodyIsBurning) {
        fire_effect_set(play, this);
    }

    if ((this->stateFlags3 & PLAYER_STATE3_RESTORE_NAYRUS_LOVE) && (z_common_data.nayrusLoveTimer != 0) &&
        (z_common_data.magicState == MAGIC_STATE_IDLE)) {
        z_common_data.magicState = MAGIC_STATE_METER_FLASH_1;
        magic_effect_set(play, this, 1);
        this->stateFlags3 &= ~PLAYER_STATE3_RESTORE_NAYRUS_LOVE;
    }

    if (this->stateFlags2 & PLAYER_STATE2_15) {
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            speedF_clear(this);
            Actor_position_moveF(&this->actor);
        }

        BGcheck(play, this);
    } else {
        f32 temp_f0;
        f32 phi_f12;

        if (this->currentBoots != this->prevBoots) {
            if (this->currentBoots == PLAYER_BOOTS_IRON) {
                if (this->stateFlags1 & PLAYER_STATE1_27) {
                    sub_camera_mode_reset(play, this);
                    if (this->ageProperties->unk_2C < this->actor.depthInWater) {
                        this->stateFlags2 |= PLAYER_STATE2_10;
                    }
                }
            } else {
                if (this->stateFlags1 & PLAYER_STATE1_27) {
                    if ((this->prevBoots == PLAYER_BOOTS_IRON) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                        water_in_set(play, this);
                        this->stateFlags2 &= ~PLAYER_STATE2_10;
                    }
                }
            }

            this->prevBoots = this->currentBoots;
        }

        if ((this->actor.parent == NULL) && (this->stateFlags1 & PLAYER_STATE1_23)) {
            this->actor.parent = this->rideActor;
            demo_init_ride_horse(play, this);
            this->stateFlags1 |= PLAYER_STATE1_23;
            anime_init_standard_stop(play, this, &gPlayerAnim_link_uma_wait_1);
            anime_move_init(play, this,
                                     ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y | ANIM_FLAG_ENABLE_MOVEMENT |
                                         ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);
            this->av2.actionVar2 = 99;
        }

        if (this->unk_844 == 0) {
            this->unk_845 = 0;
        } else if (this->unk_844 < 0) {
            this->unk_844++;
        } else {
            this->unk_844--;
        }

        chase_angle(&this->unk_6C2, 0, 400);

        Eye_Anime_pattern_set(&this->faceChange, 20, 80, 6);
        this->actor.shape.face = this->faceChange.face + ((play->gameplayFrames & 32) ? 0 : 3);

        if (this->currentMask == PLAYER_MASK_BUNNY) {
            rabit_mask_action(this);
        }

        if (player_bow_check(this) != 0) {
            bow_gen_action(this);
        }

        if (!(this->skelAnime.movementFlags & ANIM_FLAG_OVERRIDE_MOVEMENT)) {
            if (((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (ground_attribute == FLOOR_TYPE_5) &&
                 (this->currentBoots != PLAYER_BOOTS_IRON)) ||
                ((this->currentBoots == PLAYER_BOOTS_HOVER) &&
                 !(this->stateFlags1 & (PLAYER_STATE1_27 | PLAYER_STATE1_29)))) {
                f32 speedXZTarget = this->speedXZ;
                s16 yawTarget = this->yaw;
                s16 yawDiff = this->actor.world.rot.y - yawTarget;
                s32 padding;

                if ((ABS(yawDiff) > 0x6000) && (this->actor.speed != 0.0f)) {
                    speedXZTarget = 0.0f;
                    yawTarget += 0x8000;
                }

                if (chase_f(&this->actor.speed, speedXZTarget, 0.35f) && (speedXZTarget == 0.0f)) {
                    this->actor.world.rot.y = this->yaw;
                }

                if (this->speedXZ != 0.0f) {
                    s32 step;

                    step = (fabsf(this->speedXZ) * 700.0f) - (fabsf(this->actor.speed) * 100.0f);
                    step = CLAMP(step, 0, 1350);

                    chase_angle(&this->actor.world.rot.y, yawTarget, step);
                }

                if ((this->speedXZ == 0.0f) && (this->actor.speed != 0.0f)) {
                    Na_SetLinkSliplevelSe(&this->actor.projectedPos, NA_SE_PL_SLIP_LEVEL - SFX_FLAG, this->actor.speed);
                }
            } else {
                this->actor.speed = this->speedXZ;
                this->actor.world.rot.y = this->yaw;
            }

            Actor_position_speed_set(&this->actor);

            if ((this->pushedSpeed != 0.0f) && !player_demo_check(play) &&
                !(this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_21)) &&
                (move_stepup100 != this->actionFunc) && (move_magic_wait != this->actionFunc)) {
                this->actor.velocity.x += this->pushedSpeed * sin_s(this->pushedYaw);
                this->actor.velocity.z += this->pushedSpeed * cos_s(this->pushedYaw);
            }

            Actor_position_move(&this->actor);
            BGcheck(play, this);
        } else {
            ground_attribute = FLOOR_TYPE_0;
            this->floorProperty = FLOOR_PROPERTY_0;

            if (!(this->stateFlags1 & PLAYER_STATE1_0) && (this->stateFlags1 & PLAYER_STATE1_23)) {
                EnHorse* rideActor = (EnHorse*)this->rideActor;
                CollisionPoly* sp5C;
                s32 sp58;
                Vec3f sp4C;

                if (!(rideActor->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                    player_ground_BGcheck2(play, this, &scene_change_check_offset, &sp4C, &sp5C, &sp58);
                } else {
                    sp5C = rideActor->actor.floorPoly;
                    sp58 = rideActor->actor.floorBgId;
                }

                if ((sp5C != NULL) && scene_change_check(play, this, sp5C, sp58)) {
                    if (DREG(25) != 0) {
                        DREG(25) = 0;
                    } else {
                        R_EXITED_SCENE_RIDING_HORSE = true;
                    }
                }
            }

            slide_power_flag = CONVEYOR_SPEED_DISABLED;
            this->pushedSpeed = 0.0f;
        }

        // This block applies the bg conveyor to pushedSpeed
        if ((slide_power_flag != CONVEYOR_SPEED_DISABLED) && (this->currentBoots != PLAYER_BOOTS_IRON)) {
            f32 conveyorSpeed;

            // converts 1-index to 0-index
            slide_power_flag--;

            if (!slide_power_type) {
                conveyorSpeed = water_slide_power_speedF[slide_power_flag];

                if (!(this->stateFlags1 & PLAYER_STATE1_27)) {
                    conveyorSpeed *= 0.25f;
                }
            } else {
                conveyorSpeed = belt_slide_power_speedF[slide_power_flag];
            }

            chase_f(&this->pushedSpeed, conveyorSpeed, conveyorSpeed * 0.1f);

            chase_angle(&this->pushedYaw, slide_power_angle,
                               ((this->stateFlags1 & PLAYER_STATE1_27) ? 400.0f : 800.0f) * conveyorSpeed);
        } else if (this->pushedSpeed != 0.0f) {
            chase_f(&this->pushedSpeed, 0.0f, (this->stateFlags1 & PLAYER_STATE1_27) ? 0.5f : 1.0f);
        }

        if (!player_action_check(play, this) && !(this->stateFlags2 & PLAYER_STATE2_CRAWLING)) {
            water_check(play, this);

            if ((this->actor.category == ACTORCAT_PLAYER) && (z_common_data.save.info.playerData.health == 0)) {
                if (this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_21)) {
                    action_reset(play, this);
                    to_landing_set(this, play);
                } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->stateFlags1 & PLAYER_STATE1_27)) {
                    to_down_set(play, this,
                                  swim_check(this)           ? &gPlayerAnim_link_swimer_swim_down
                                  : (this->bodyShockTimer != 0) ? &gPlayerAnim_link_normal_electric_shock_end
                                                                : &gPlayerAnim_link_derth_rebirth);
                }
            } else {
                if ((this->actor.parent == NULL) && ((play->transitionTrigger == TRANS_TRIGGER_START) ||
                                                     (this->unk_A87 != 0) || !damage_check(this, play))) {
                    to_landing_check(this, play);
                } else {
                    this->fallStartHeight = this->actor.world.pos.y;
                }

                dowsing_check(this);
            }
        }

        if ((play->csCtx.state != CS_STATE_IDLE) && (this->csAction != PLAYER_CSACTION_6) &&
            !(this->stateFlags1 & PLAYER_STATE1_23) && !(this->stateFlags2 & PLAYER_STATE2_7) &&
            (this->actor.category == ACTORCAT_PLAYER)) {
            CsCmdActorCue* cue = play->csCtx.playerCue;

            if ((cue != NULL) && (tool_demo_mode_data[cue->id] != PLAYER_CSACTION_NONE)) {
                player_demo_mode_set(play, NULL, PLAYER_CSACTION_6);
                speedF_clear(this);
            } else if ((this->csAction == PLAYER_CSACTION_NONE) && !(this->stateFlags2 & PLAYER_STATE2_10) &&
                       (play->csCtx.state != CS_STATE_STOP)) {
                player_demo_mode_set(play, NULL, PLAYER_CSACTION_49);
                speedF_clear(this);
            }
        }

        if (this->csAction != PLAYER_CSACTION_NONE) {
            if ((this->csAction != PLAYER_CSACTION_7) ||
                !(this->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_21 | PLAYER_STATE1_26))) {
                this->unk_6AD = 3;
            } else if (move_demo_play != this->actionFunc) {
                demo_play_end(play, this, NULL);
            }
        } else {
            this->prevCsAction = PLAYER_CSACTION_NONE;
        }

        ground_condition_check(play, this);

        if ((this->focusActor == NULL) && (this->naviTextId == 0)) {
            this->stateFlags2 &= ~(PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER | PLAYER_STATE2_21);
        }

        this->stateFlags1 &= ~(PLAYER_STATE1_SWINGING_BOTTLE | PLAYER_STATE1_9 | PLAYER_STATE1_CHARGING_SPIN_ATTACK |
                               PLAYER_STATE1_SHIELDING);
        this->stateFlags2 &= ~(PLAYER_STATE2_0 | PLAYER_STATE2_2 | PLAYER_STATE2_3 | PLAYER_STATE2_5 | PLAYER_STATE2_6 |
                               PLAYER_STATE2_8 | PLAYER_STATE2_FORCE_SAND_FLOOR_SOUND | PLAYER_STATE2_12 |
                               PLAYER_STATE2_14 | PLAYER_STATE2_DO_ACTION_ENTER | PLAYER_STATE2_22 | PLAYER_STATE2_26);
        this->stateFlags3 &= ~PLAYER_STATE3_4;

        look_anime_angle_check(this);
        stick_read(play, this);

        if (this->stateFlags1 & PLAYER_STATE1_27) {
            environment_speed_ratio = 0.5f;
        } else {
            environment_speed_ratio = 1.0f;
        }

        environment_p_speed_ratio = 1.0f / environment_speed_ratio;

        item_key = item_also_key = false;
        old_mask = this->currentMask;

        if (!(this->stateFlags3 & PLAYER_STATE3_2)) {
            this->actionFunc(this, play);
        }

        camera_mode_check(play, this);

        if (this->skelAnime.movementFlags & ANIM_FLAG_ENABLE_MOVEMENT) {
            Skeleton_Proc_Anime_Move_init(play, &this->actor, &this->skelAnime,
                                           (this->skelAnime.movementFlags & ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT)
                                               ? 1.0f
                                               : this->ageProperties->unk_08);
        }

        anime_angle_control(this, play);

        if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_TALK)) {
            this->talkActorDistance = 0.0f;
        } else {
            this->talkActor = NULL;
            this->talkActorDistance = MAXFLOAT;
            this->exchangeItemId = EXCH_ITEM_NONE;
        }

        if (!(this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            this->interactRangeActor = NULL;
            this->getItemDirection = 0x6000;
        }

        if (this->actor.parent == NULL) {
            this->rideActor = NULL;
        }

        this->naviTextId = 0;

        if (!(this->stateFlags2 & PLAYER_STATE2_25)) {
            this->unk_6A8 = NULL;
        }

        this->stateFlags2 &= ~PLAYER_STATE2_23;
        this->closestSecretDistSq = MAXFLOAT;

        temp_f0 = this->actor.world.pos.y - this->actor.prevPos.y;

        this->doorType = PLAYER_DOORTYPE_NONE;
        this->knockbackType = PLAYER_KNOCKBACK_NONE;
        this->autoLockOnActor = NULL;

        phi_f12 =
            ((this->bodyPartsPos[PLAYER_BODYPART_L_FOOT].y + this->bodyPartsPos[PLAYER_BODYPART_R_FOOT].y) * 0.5f) +
            temp_f0;
        temp_f0 += this->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 10.0f;

        this->cylinder.dim.height = temp_f0 - phi_f12;

        if (this->cylinder.dim.height < 0) {
            phi_f12 = temp_f0;
            this->cylinder.dim.height = -this->cylinder.dim.height;
        }

        this->cylinder.dim.yShift = phi_f12 - this->actor.world.pos.y;

        if (this->stateFlags1 & PLAYER_STATE1_SHIELDING) {
            this->cylinder.dim.height *= 0.8f;
        }

        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->cylinder);

        if (!(this->stateFlags2 & PLAYER_STATE2_14)) {
            if (!(this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_13 | PLAYER_STATE1_14 | PLAYER_STATE1_23))) {
                CollisionCheck_setOC(play, &play->colChkCtx, &this->cylinder.base);
            }

            if (!(this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_26)) && (this->invincibilityTimer <= 0)) {
                CollisionCheck_setAC(play, &play->colChkCtx, &this->cylinder.base);

                if (this->invincibilityTimer < 0) {
                    CollisionCheck_setAT(play, &play->colChkCtx, &this->cylinder.base);
                }
            }
        }
        // Because Player updates early relative to most actors, his animation tasks will belong to group 0.
        // Task group 1 is set here at the end of Player's update, meaning tasks that get added by other
        // actors will be in a different group.
        Skeleton_Proc_Anime_Change_Channel_Up(play);
    }

    xyz_t_move(&this->actor.home.pos, &this->actor.world.pos);
    xyz_t_move(&this->unk_A88, &this->bodyPartsPos[PLAYER_BODYPART_WAIST]);

    if (this->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_28 | PLAYER_STATE1_29)) {
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    } else {
        this->actor.colChkInfo.mass = 50;
    }

    this->stateFlags3 &= ~PLAYER_STATE3_2;

    ClObjPipe_ACClear(play, &this->cylinder.base);

    ClObjSwrd_ATClear(play, &this->meleeWeaponQuads[0].base);
    ClObjSwrd_ATClear(play, &this->meleeWeaponQuads[1].base);

    ClObjSwrd_ACClear(play, &this->shieldQuad.base);
    ClObjSwrd_ATClear(play, &this->shieldQuad.base);
}

#if DEBUG_FEATURES
s32 Player_actor_moveNoclip(Player* this, PlayState* play);
#endif

void Player_actor_move(Actor* thisx, PlayState* play) {
    Player* this = (Player*)thisx;
    s32 dogParams;
    s32 padding;
    Input input;

#if DEBUG_FEATURES
    if (!Player_actor_moveNoclip(this, play)) {
        goto skip_update;
    }
#endif

    if (z_common_data.dogParams < 0) {
        static Vec3f dog_offset = { 0.0f, 0.0f, -30.0f };
        static Vec3f dog_set_pos;

        if (Object_Exchange_bank_check(&play->objectCtx, OBJECT_DOG) < 0) {
            z_common_data.dogParams = 0;
        } else {
            Actor* dog;

            z_common_data.dogParams &= 0x7FFF;
            offset_pos_calc(this, &this->actor.world.pos, &dog_offset, &dog_set_pos);
            dogParams = z_common_data.dogParams;

            dog = Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_DOG, dog_set_pos.x, dog_set_pos.y, dog_set_pos.z, 0,
                              this->actor.shape.rot.y, 0, dogParams | 0x8000);
            if (dog != NULL) {
                dog->room = 0;
            }
        }
    }

    if ((this->interactRangeActor != NULL) && (this->interactRangeActor->update == NULL)) {
        this->interactRangeActor = NULL;
    }

    if ((this->heldActor != NULL) && (this->heldActor->update == NULL)) {
        l_hand_child_cancel(play, this);
    }

    if (this->stateFlags1 & (PLAYER_STATE1_5 | PLAYER_STATE1_29)) {
        bzero(&input, sizeof(input));
    } else {
        input = play->state.input[0];

        if (this->textboxBtnCooldownTimer != 0) {
            // Prevent the usage of A/B/C-up.
            // Helps avoid accidental inputs when mashing to close the final textbox.
            input.cur.button &= ~(BTN_A | BTN_B | BTN_CUP);
            input.press.button &= ~(BTN_A | BTN_B | BTN_CUP);
        }
    }

    Ext_Player_actor_proc(this, play, &input);

#if DEBUG_FEATURES
skip_update:;
#endif
    {
        s32 padding;

        MREG(52) = this->actor.world.pos.x;
        MREG(53) = this->actor.world.pos.y;
        MREG(54) = this->actor.world.pos.z;

        MREG(55) = this->actor.world.rot.y;
    }
}

typedef struct BunnyEarKinematics {
    /* 0x0 */ Vec3s rot;
    /* 0x6 */ Vec3s angVel;
} BunnyEarKinematics; // size = 0xC

static BunnyEarKinematics rabit_info;

static Gfx* mask_shape[PLAYER_MASK_MAX - 1] = {
    gLinkChildKeatonMaskDL, gLinkChildSkullMaskDL, gLinkChildSpookyMaskDL, gLinkChildBunnyHoodDL,
    gLinkChildGoronMaskDL,  gLinkChildZoraMaskDL,  gLinkChildGerudoMaskDL, gLinkChildMaskOfTruthDL,
};

static Vec3s hover_angle = { 0, 0, 0 };

void player_shape_draw_set(PlayState* play, Player* this, s32 lod, Gfx* cullDList, OverrideLimbDrawOpa overrideLimbDraw) {
    OPEN_DISPS(play->state.gfxCtx, "../z_player.c", 19228);

    gSPSegment(POLY_OPA_DISP++, 0x0C, cullDList);
    gSPSegment(POLY_XLU_DISP++, 0x0C, cullDList);

    Player_shape_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, lod,
                    this->currentTunic, this->currentBoots, this->actor.shape.face, overrideLimbDraw,
                    player_after_draw, this);

    if ((overrideLimbDraw == player_before_draw) && (this->currentMask != PLAYER_MASK_NONE)) {
        Mtx* bunnyEarMtx = GRAPH_ALLOC(play->state.gfxCtx, 2 * sizeof(Mtx));

        if (this->currentMask == PLAYER_MASK_BUNNY) {
            Vec3s earRot;

            gSPSegment(POLY_OPA_DISP++, 0x0B, bunnyEarMtx);

            // Right ear
            earRot.x = rabit_info.rot.y + 0x3E2;
            earRot.y = rabit_info.rot.z + 0xDBE;
            earRot.z = rabit_info.rot.x - 0x348A;
            Matrix_softcv3_load(97.0f, -1203.0f, -240.0f, &earRot);
            MATRIX_TO_MTX(bunnyEarMtx++, "../z_player.c", 19273);

            // Left ear
            earRot.x = rabit_info.rot.y - 0x3E2;
            earRot.y = -rabit_info.rot.z - 0xDBE;
            earRot.z = rabit_info.rot.x - 0x348A;
            Matrix_softcv3_load(97.0f, -1203.0f, 240.0f, &earRot);
            MATRIX_TO_MTX(bunnyEarMtx, "../z_player.c", 19279);
        }

        gSPDisplayList(POLY_OPA_DISP++, mask_shape[this->currentMask - 1]);
    }

    if ((this->currentBoots == PLAYER_BOOTS_HOVER) && !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        !(this->stateFlags1 & PLAYER_STATE1_23) && ((u32)this->hoverBootsTimer != 0)) {
        static s32 alpha = 255;

        if (this->hoverBootsTimer < 19) {
            if (this->hoverBootsTimer >= 15) {
                alpha = (19 - this->hoverBootsTimer) * 51.0f;
            } else if (this->hoverBootsTimer < 19) {
                s32 sp5C = this->hoverBootsTimer;

                if (sp5C > 9) {
                    sp5C = 9;
                }

                alpha = (-sp5C * 4) + 36;
                alpha = SQ(alpha);
                alpha = (s32)((cos_s(alpha) * 100.0f) + 100.0f) + 55.0f;
                alpha *= sp5C * (1.0f / 9.0f);
            }

            Matrix_softcv3_load(this->actor.world.pos.x, this->actor.world.pos.y + 2.0f,
                                         this->actor.world.pos.z, &hover_angle);
            Matrix_scale(4.0f, 4.0f, 4.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_player.c", 19317);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 16, 32, 1, 0,
                                        (play->gameplayFrames * -15) % 128, 16, 32));
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 255, alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 120, 90, 30, 128);
            gSPDisplayList(POLY_XLU_DISP++, gHoverBootsCircleDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_player.c", 19328);
}

void Player_actor_draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    Player* this = (Player*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_player.c", 19346);

    if (!(this->stateFlags2 & PLAYER_STATE2_29)) {
        OverrideLimbDrawOpa overrideLimbDraw = player_before_draw;
        s32 lod;
        s32 padding;

        if ((this->csAction != PLAYER_CSACTION_NONE) || (anchor_mode_check(this) && 0) ||
            (this->actor.projectedPos.z < 160.0f)) {
            lod = 0;
        } else {
            lod = 1;
        }

        _texture_z_light_fog_prim2(play);
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

        if (this->invincibilityTimer > 0) {
            this->damageFlickerAnimCounter += CLAMP(50 - this->invincibilityTimer, 8, 40);
            POLY_OPA_DISP = set_fog(POLY_OPA_DISP, 255, 0, 0, 0, 0,
                                        4000 - (s32)(cos_s(this->damageFlickerAnimCounter * 256) * 2000.0f));
        }

        Actor_HiliteReflect_set_init(&this->actor, play, 0);
        Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);

        if (this->unk_6AD != 0) {
            Vec3f projectedHeadPos;

            Skin_Matrix_MulVector(&play->viewProjectionMtxF, &this->actor.focus.pos, &projectedHeadPos);
            if (projectedHeadPos.z < -4.0f) {
                overrideLimbDraw = before_high_draw;
            }
        } else if (this->stateFlags2 & PLAYER_STATE2_CRAWLING) {
            if (this->actor.projectedPos.z < 0.0f) {
                // Player is behind the camera
                overrideLimbDraw = before_no_draw;
            }
        }

        if (this->stateFlags2 & PLAYER_STATE2_26) {
            f32 sp78 = BINANG_TO_RAD_ALT2((u16)(play->gameplayFrames * 600));
            f32 sp74 = BINANG_TO_RAD_ALT2((u16)(play->gameplayFrames * 1000));

            Matrix_push();
            this->actor.scale.y = -this->actor.scale.y;
            Matrix_softcv3_load(
                this->actor.world.pos.x,
                (this->actor.floorHeight + (this->actor.floorHeight - this->actor.world.pos.y)) +
                    (this->actor.shape.yOffset * this->actor.scale.y),
                this->actor.world.pos.z, &this->actor.shape.rot);
            Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Matrix_rotateX(sp78, MTXMODE_APPLY);
            Matrix_rotateY(sp74, MTXMODE_APPLY);
            Matrix_scale(1.1f, 0.95f, 1.05f, MTXMODE_APPLY);
            Matrix_rotateY(-sp74, MTXMODE_APPLY);
            Matrix_rotateX(-sp78, MTXMODE_APPLY);
            player_shape_draw_set(play, this, lod, set_cull_front_model, overrideLimbDraw);
            this->actor.scale.y = -this->actor.scale.y;
            Matrix_pull();
        }

        gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BOTH);
        gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BOTH);

        player_shape_draw_set(play, this, lod, set_cull_back_model, overrideLimbDraw);

        if (this->invincibilityTimer > 0) {
            POLY_OPA_DISP = game_play_set_fog(play, POLY_OPA_DISP);
        }

        if (this->stateFlags2 & PLAYER_STATE2_14) {
            f32 scale = (this->av1.actionVar1 >> 1) * 22.0f;

            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (0 - play->gameplayFrames) % 128, 32,
                                        32, 1, 0, (play->gameplayFrames * -2) % 128, 32, 32));

            Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_player.c", 19459);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, 255);
            gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);
        }

        if (this->unk_862 > 0) {
            get_item_draw(play, this);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_player.c", 19473);
}

void Player_actor_dt(Actor* thisx, PlayState* play) {
    Player* this = (Player*)thisx;

    EffectFreeIndex(play, this->meleeWeaponEffectIndex);

    ClObjPipe_dt(play, &this->cylinder);
    ClObjSwrd_dt(play, &this->meleeWeaponQuads[0]);
    ClObjSwrd_dt(play, &this->meleeWeaponQuads[1]);
    ClObjSwrd_dt(play, &this->shieldQuad);

    magic_cancel_check(play);

    z_common_data.save.linkAge = play->linkAgeOnLoad;
}

s16 look_eye_move(PlayState* play, Player* this, s32 arg2, s16 arg3) {
    s32 temp1;
    s16 temp2;
    s16 temp3;

    if (!player_bow_ready_check(this) && !boom_ready_check(this) && !arg2) {
        temp2 = pad->rel.stick_y * 240.0f;
        add_calc_short_angle2(&this->actor.focus.rot.x, temp2, 14, 4000, 30);

        temp2 = pad->rel.stick_x * -16.0f;
        temp2 = CLAMP(temp2, -3000, 3000);
        this->actor.focus.rot.y += temp2;
    } else {
        temp1 = (this->stateFlags1 & PLAYER_STATE1_23) ? 3500 : 14000;
        temp3 = ((pad->rel.stick_y >= 0) ? 1 : -1) *
                (s32)((1.0f - cos_s(pad->rel.stick_y * 200)) * 1500.0f);
        this->actor.focus.rot.x += temp3;
        this->actor.focus.rot.x = CLAMP(this->actor.focus.rot.x, -temp1, temp1);

        temp1 = 19114;
        temp2 = this->actor.focus.rot.y - this->actor.shape.rot.y;
        temp3 = ((pad->rel.stick_x >= 0) ? 1 : -1) *
                (s32)((1.0f - cos_s(pad->rel.stick_x * 200)) * -1500.0f);
        temp2 += temp3;
        this->actor.focus.rot.y = CLAMP(temp2, -temp1, temp1) + this->actor.shape.rot.y;
    }

    this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_Y;
    return eye_move_anime_set(this, (play->shootingGalleryStatus != 0) || player_bow_ready_check(this) || boom_ready_check(this)) - arg3;
}

void swim_control(Player* this, f32* arg1, f32 arg2, s16 arg3) {
    f32 temp1;
    f32 temp2;

    temp1 = this->skelAnime.curFrame - 10.0f;

    temp2 = (R_RUN_SPEED_LIMIT / 100.0f) * 0.8f;
    if (*arg1 > temp2) {
        *arg1 = temp2;
    }

    if ((0.0f < temp1) && (temp1 < 10.0f)) {
        temp1 *= 6.0f;
    } else {
        temp1 = 0.0f;
        arg2 = 0.0f;
    }

    chase_f3(arg1, arg2 * 0.8f, temp1, (fabsf(*arg1) * 0.02f) + 0.05f);
    chase_angle(&this->yaw, arg3, 1600);
}

void water_pressure_set(Player* this) {
    f32 phi_f18;
    f32 phi_f16;
    f32 phi_f14;
    f32 depthInWater;

    phi_f14 = FRAMERATE_CONST(-5.0f, -6.0f);

    phi_f16 = this->ageProperties->unk_28;
    if (this->actor.velocity.y < 0.0f) {
        phi_f16 += 1.0f;
    }

    if (this->actor.depthInWater < phi_f16) {
        if (this->actor.velocity.y <= 0.0f) {
            phi_f16 = 0.0f;
        } else {
            phi_f16 = this->actor.velocity.y * 0.5f;
        }
        phi_f18 = -0.1f - phi_f16;
    } else {
        if (!(this->stateFlags1 & PLAYER_STATE1_DEAD) && (this->currentBoots == PLAYER_BOOTS_IRON) &&
            (this->actor.velocity.y >= FRAMERATE_CONST(-3.0f, -3.6f))) {
            phi_f18 = -0.2f;
        } else {
            phi_f14 = FRAMERATE_CONST(2.0f, 2.4f);
            if (this->actor.velocity.y >= 0.0f) {
                phi_f16 = 0.0f;
            } else {
                phi_f16 = this->actor.velocity.y * -0.3f;
            }
            phi_f18 = phi_f16 + 0.1f;
        }

        depthInWater = this->actor.depthInWater;
        if (depthInWater > 100.0f) {
            this->stateFlags2 |= PLAYER_STATE2_10;
        }
    }

    this->actor.velocity.y += phi_f18;

    if (((this->actor.velocity.y - phi_f14) * phi_f18) > 0) {
        this->actor.velocity.y = phi_f14;
    }

    this->actor.gravity = 0.0f;
}

void swim_anime_play(PlayState* play, Player* this, Input* input, f32 arg3) {
    f32 temp;
    f32 limit;

    if ((input != NULL) && CHECK_BTN_ANY(input->press.button, BTN_A | BTN_B)) {
        temp = limit = FRAMERATE_CONST(1.0f, 1.2f);
    } else {
        temp = FRAMERATE_CONST(0.5f, 0.6f);
        limit = FRAMERATE_CONST(1.0f, 1.2f);
    }

    temp *= arg3;

    if (temp < limit) {
        temp = limit;
    }

    this->skelAnime.playSpeed = temp;
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
}

void move_look(Player* this, PlayState* play) {
    if (this->stateFlags1 & PLAYER_STATE1_27) {
        water_pressure_set(this);
        swim_control(this, &this->speedXZ, 0, this->actor.shape.rot.y);
    } else {
        ground_breaking_speedF(this);
    }

    if ((this->unk_6AD == 2) && (player_bow_check(this) || boom_check(this))) {
        uperbody_action_check(this, play);
    }

    if ((this->csAction != PLAYER_CSACTION_NONE) || (this->unk_6AD == 0) || (this->unk_6AD >= 4) ||
        anchor_mode_check_set(this) || (this->focusActor != NULL) ||
        (subject_camera_mode_check(play, this) == CAM_MODE_NORMAL) ||
        (((this->unk_6AD == 2) &&
          (CHECK_BTN_ANY(pad->press.button, BTN_A | BTN_B | BTN_R) || parallel_mode_check(this) ||
           (!player_bow_ready_check(this) && !boom_ready_check(this)))) ||
         ((this->unk_6AD == 1) &&
          CHECK_BTN_ANY(pad->press.button,
                        BTN_A | BTN_B | BTN_R | BTN_CUP | BTN_CDOWN | BTN_CLEFT | BTN_CRIGHT)))) {
        to_move_set(this, play);
        Na_StartSystemSe_F(NA_SE_SY_CAMERA_ZOOM_UP);
    } else if ((DECR(this->av2.actionVar2) == 0) || (this->unk_6AD != 2)) {
        if (hook_shoot_check(this)) {
            this->unk_6AE_rotFlags |= UNK6AE_ROT_FOCUS_X | UNK6AE_ROT_FOCUS_Y | UNK6AE_ROT_UPPER_X;
        } else {
            this->actor.shape.rot.y = look_eye_move(play, this, false, 0);
        }
    }

    this->yaw = this->actor.shape.rot.y;
}

s32 to_bow_game_check(PlayState* play, Player* this) {
    if (play->shootingGalleryStatus != 0) {
        action_reset_all(play, this);
        Player_actor_set_process(play, this, move_bow_game, 0);

        if (!player_bow_check(this) || hook_check(this)) {
            player_item_on(play, this, 3);
        }

        this->stateFlags1 |= PLAYER_STATE1_20;
        anime_init_standard_stop(play, this, wait_anime_select(this));
        speedF_clear(this);
        look_angle_reset(this);
        return 1;
    }

    return 0;
}

void okarina_now_item_check_set(Player* this) {
    this->itemAction =
        (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY) ? PLAYER_IA_OCARINA_FAIRY : PLAYER_IA_OCARINA_OF_TIME;
}

s32 to_okarina_set_check(PlayState* play, Player* this) {
    if (this->stateFlags3 & PLAYER_STATE3_5) {
        this->stateFlags3 &= ~PLAYER_STATE3_5;
        okarina_now_item_check_set(this);
        this->unk_6AD = 4;
        to_look_set_check(this, play);
        return 1;
    }

    return 0;
}

static void move_talk(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_5;

    uperbody_action_check(this, play);

    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->actor.flags &= ~ACTOR_FLAG_TALK;

        if (!CHECK_FLAG_ALL(this->talkActor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)) {
            this->stateFlags2 &= ~PLAYER_STATE2_LOCK_ON_WITH_SWITCH;
        }

        restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));

        if (!to_okarina_set_check(play, this) && !to_bow_game_check(play, this) && !to_tool_demo_check(play, this)) {
            if ((this->talkActor != this->interactRangeActor) || !to_carry_check(this, play)) {
                if (this->stateFlags1 & PLAYER_STATE1_23) {
                    s32 sp24 = this->av2.actionVar2;
                    demo_init_ride_horse(play, this);
                    this->av2.actionVar2 = sp24;
                } else if (swim_check(this)) {
                    to_swim_wait_set(play, this);
                } else {
                    Ext_to_wait_set(this, play);
                }
            }
        }

        this->textboxBtnCooldownTimer = 10;
        return;
    }

    if (this->stateFlags1 & PLAYER_STATE1_23) {
        move_ride_horse(this, play);
    } else if (swim_check(this)) {
        move_swim_wait(this, play);
    } else if (!anchor_mode_check(this) && Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (this->skelAnime.movementFlags != 0) {
            anime_move_reset(this);

            if ((this->talkActor->category == ACTORCAT_NPC) && (this->heldItemAction != PLAYER_IA_FISHING_POLE)) {
                anime_init_standard_stop_3f(play, this, &gPlayerAnim_link_normal_talk_free);
            } else {
                anime_init_standard_repeat(play, this, wait_anime_select(this));
            }
        } else {
            anime_init_standard_repeat_3f(play, this, &gPlayerAnim_link_normal_talk_free_wait);
        }
    }

    if (this->focusActor != NULL) {
        this->yaw = this->actor.shape.rot.y = anchor_eye_move(this, false);
    }
}

void move_push_wait(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s32 temp;

    this->stateFlags2 |= PLAYER_STATE2_0 | PLAYER_STATE2_6 | PLAYER_STATE2_8;
    wall_angle_check(play, this);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (!to_push_cancel_check(play, this)) {
            stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
            temp = push_mode_key_check(this, &speedTarget, &yawTarget);
            if (temp > 0) {
                to_pushing_set(this, play);
            } else if (temp < 0) {
                to_pulling_set(this, play);
            }
        }
    }
}

void moveBG_wall_powerF_set_check(PlayState* play, Player* this, f32 arg2) {
    if (this->actor.wallBgId != BGCHECK_SCENE) {
        DynaPolyActor* dynaPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, this->actor.wallBgId);

        if (dynaPolyActor != NULL) {
            MoveBG_Actor_powerF_set(dynaPolyActor, arg2, this->actor.world.rot.y);
        }
    }
}

void move_pushing(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_PL_SLIP, ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 3) },
        { NA_SE_PL_SLIP, -ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 21) },
    };

    this->stateFlags2 |= PLAYER_STATE2_0 | PLAYER_STATE2_6 | PLAYER_STATE2_8;

    if (anime_play_repeat(play, this, &gPlayerAnim_link_normal_pushing)) {
        this->av2.actionVar2 = 1;
    } else if (this->av2.actionVar2 == 0) {
        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 11.0f)) {
            player_voice_SE_set(this, NA_SE_VO_LI_PUSH);
        }
    }

    player_anime_check_SE_set(this, SE_set_status);
    wall_angle_check(play, this);

    if (!to_push_cancel_check(play, this)) {
        f32 speedTarget;
        s16 yawTarget;
        s32 temp;

        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        temp = push_mode_key_check(this, &speedTarget, &yawTarget);
        if (temp < 0) {
            to_pulling_set(this, play);
        } else if (temp == 0) {
            to_push_wait_set(this, &gPlayerAnim_link_normal_push_end, play);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_4;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_4) {
        moveBG_wall_powerF_set_check(play, this, 2.0f);
        this->speedXZ = 2.0f;
    }
}

void move_pulling(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_PL_SLIP, ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 4) },
        { NA_SE_PL_SLIP, -ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 24) },
    };

    static Vec3f pull_check_offset = { 0.0f, 26.0f, -40.0f };

    LinkAnimationHeader* anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_pulling, this->modelAnimType);

    this->stateFlags2 |= PLAYER_STATE2_0 | PLAYER_STATE2_6 | PLAYER_STATE2_8;

    if (anime_play_repeat(play, this, anim)) {
        this->av2.actionVar2 = 1;
    } else {
        if (this->av2.actionVar2 == 0) {
            if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 11.0f)) {
                player_voice_SE_set(this, NA_SE_VO_LI_PUSH);
            }
            if (!DEBUG_FEATURES) {}
        } else {
            player_anime_check_SE_set(this, SE_set_status);
        }
    }

    wall_angle_check(play, this);

    if (!to_push_cancel_check(play, this)) {
        f32 speedTarget;
        s16 yawTarget;
        s32 temp1;

        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        temp1 = push_mode_key_check(this, &speedTarget, &yawTarget);
        if (temp1 > 0) {
            to_pushing_set(this, play);
        } else if (temp1 == 0) {
            to_push_wait_set(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_pull_end, this->modelAnimType), play);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_4;
        }
    }

    if (this->stateFlags2 & PLAYER_STATE2_4) {
        Vec3f sp5C;
        f32 temp2;
        CollisionPoly* sp54;
        s32 sp50;
        Vec3f sp44;
        Vec3f sp38;

        temp2 = player_ground_BGcheck(play, this, &pull_check_offset, &sp5C) - this->actor.world.pos.y;
        if (fabsf(temp2) < 20.0f) {
            sp44.x = this->actor.world.pos.x;
            sp44.z = this->actor.world.pos.z;
            sp44.y = sp5C.y;
            if (!T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &sp44, &sp5C, &sp38, &sp54, true, false, false, true, &sp50)) {
                moveBG_wall_powerF_set_check(play, this, -2.0f);
                return;
            }
        }
        this->stateFlags2 &= ~PLAYER_STATE2_4;
    }
}

void move_fall_wait(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    LinkAnimationHeader* anim;
    f32 temp;

    this->stateFlags2 |= PLAYER_STATE2_6;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        // clang-format off
        anim = (this->av1.actionVar1 > 0) ? &gPlayerAnim_link_normal_fall_wait : GET_PLAYER_ANIM(PLAYER_ANIMGROUP_jump_climb_wait, this->modelAnimType); anime_init_standard_repeat(play, this, anim);
        // clang-format on
    } else if (this->av1.actionVar1 == 0) {
        if (this->skelAnime.animation == &gPlayerAnim_link_normal_fall) {
            temp = 11.0f;
        } else {
            temp = 1.0f;
        }

        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, temp)) {
            player_ground_SE_set(this, NA_SE_PL_WALK_GROUND);
            if (this->skelAnime.animation == &gPlayerAnim_link_normal_fall) {
                this->av1.actionVar1 = 1;
            } else {
                this->av1.actionVar1 = -1;
            }
        }
    }

    chase_angle(&this->actor.shape.rot.y, this->yaw, 0x800);

    if (this->av1.actionVar1 != 0) {
        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
        if (this->controlStickSpinAngles[this->controlStickDataIndex] >= 0) {
            if (this->av1.actionVar1 > 0) {
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_fall_up, this->modelAnimType);
            } else {
                anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_jump_climb_up, this->modelAnimType);
            }
            to_fall_up_set(this, anim, play);
            return;
        }

        if (CHECK_BTN_ALL(pad->cur.button, BTN_A) || (this->actor.shape.feetFloorFlag != 0)) {
            fall_wait_cancel_pos_set(this);
            if (this->av1.actionVar1 < 0) {
                this->speedXZ = -0.8f;
            } else {
                this->speedXZ = 0.8f;
            }
            to_landing_set(this, play);
            this->stateFlags1 &= ~(PLAYER_STATE1_13 | PLAYER_STATE1_14);
        }
    }
}

void move_fall_up(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_6;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_move_calc(this, ANIM_FLAG_UPDATE_XZ);
        to_wait_no_morf_set(this, play);
        return;
    }

    if (Skeleton_Info_Rom_frame_check(&this->skelAnime, this->skelAnime.endFrame - 6.0f)) {
        player_land_SE_set(this);
    } else if (Skeleton_Info_Rom_frame_check(&this->skelAnime, this->skelAnime.endFrame - 34.0f)) {
        this->stateFlags1 &= ~(PLAYER_STATE1_13 | PLAYER_STATE1_14);
        player_SE_set(this, NA_SE_PL_CLIMB_CLIFF);
        player_voice_SE_set(this, NA_SE_VO_LI_CLIMB_END);
    }
}

void climb_SE_set(Player* this) {
    player_SE_set(this, (this->av1.actionVar1 != 0) ? NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_VINE
                                                     : NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_WOOD);
}

void move_climb(Player* this, PlayState* play) {
    static Vec3f climb_end_check_offset = { 0.0f, 0.0f, 26.0f };
    s32 sp84;
    s32 sp80;
    f32 phi_f0;
    f32 phi_f2;
    Vec3f sp6C;
    s32 sp68;
    Vec3f sp5C;
    DynaPolyActor* wallPolyActor;
    LinkAnimationHeader* anim1;
    LinkAnimationHeader* anim2;

    sp84 = pad->rel.stick_y;
    sp80 = pad->rel.stick_x;

    this->fallStartHeight = this->actor.world.pos.y;
    this->stateFlags2 |= PLAYER_STATE2_6;

    if ((this->av1.actionVar1 != 0) && (ABS(sp84) < ABS(sp80))) {
        phi_f0 = ABS(sp80) * 0.0325f;
        sp84 = 0;
    } else {
        phi_f0 = ABS(sp84) * 0.05f;
        sp80 = 0;
    }

    if (phi_f0 < 1.0f) {
        phi_f0 = 1.0f;
    } else if (phi_f0 > 3.35f) {
        phi_f0 = 3.35f;
    }

    if (this->skelAnime.playSpeed >= 0.0f) {
        phi_f2 = 1.0f;
    } else {
        phi_f2 = -1.0f;
    }

    this->skelAnime.playSpeed = phi_f2 * phi_f0;

    if (this->av2.actionVar2 >= 0) {
        if ((this->actor.wallPoly != NULL) && (this->actor.wallBgId != BGCHECK_SCENE)) {
            wallPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, this->actor.wallBgId);
            if (wallPolyActor != NULL) {
                xyz_t_sub(&wallPolyActor->actor.world.pos, &wallPolyActor->actor.prevPos, &sp6C);
                xyz_t_add(&this->actor.world.pos, &sp6C, &this->actor.world.pos);
            }
        }

        Actor_BGcheck2(play, &this->actor, 26.0f, 6.0f, this->ageProperties->ceilingCheckHeight,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
        wall_angle_check2(play, this, 26.0f, this->ageProperties->unk_3C, 50.0f, -20.0f);
    }

    if ((this->av2.actionVar2 < 0) || !to_climb_cancel_check(this, play)) {
        if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime) != 0) {
            if (this->av2.actionVar2 < 0) {
                this->av2.actionVar2 = ABS(this->av2.actionVar2) & 1;
                return;
            }

            if (sp84 != 0) {
                f32 temp_f0;

                sp68 = this->av1.actionVar1 + this->av2.actionVar2;

                if (sp84 > 0) {
                    climb_end_check_offset.y = this->ageProperties->unk_40;
                    temp_f0 = player_ground_BGcheck(play, this, &climb_end_check_offset, &sp5C);

                    if (this->actor.world.pos.y < temp_f0) {
                        if (this->av1.actionVar1 != 0) {
                            this->actor.world.pos.y = temp_f0;
                            this->stateFlags1 &= ~PLAYER_STATE1_21;
                            to_fall_set(play, this, this->actor.wallPoly, this->ageProperties->unk_3C,
                                          &gPlayerAnim_link_normal_jump_climb_up_free);
                            this->yaw += 0x8000;
                            this->actor.shape.rot.y = this->yaw;
                            to_fall_up_set(this, &gPlayerAnim_link_normal_jump_climb_up_free, play);
                            this->stateFlags1 |= PLAYER_STATE1_14;
                        } else {
                            to_climb_end_set(this, this->ageProperties->unk_CC[this->av2.actionVar2], play);
                        }
                    } else {
                        this->skelAnime.prevTransl = this->ageProperties->unk_4A[sp68];
                        anime_init_standard_stop(play, this, this->ageProperties->unk_AC[sp68]);
                    }
                } else {
                    if ((this->actor.world.pos.y - this->actor.floorHeight) < 15.0f) {
                        if (this->av1.actionVar1 != 0) {
                            to_climb_cancel_set(this, play);
                        } else {
                            if (this->av2.actionVar2 != 0) {
                                this->skelAnime.prevTransl = this->ageProperties->unk_44;
                            }
                            to_climb_end_set(this, this->ageProperties->unk_C4[this->av2.actionVar2], play);
                            this->av2.actionVar2 = 1;
                        }
                    } else {
                        sp68 ^= 1;
                        this->skelAnime.prevTransl = this->ageProperties->unk_62[sp68];
                        anim1 = this->ageProperties->unk_AC[sp68];
                        Skeleton_Info_Rom_init(play, &this->skelAnime, anim1, -1.0f, Si2_anime_end_frame(anim1), 0.0f,
                                             ANIMMODE_ONCE, 0.0f);
                    }
                }
                this->av2.actionVar2 ^= 1;
            } else {
                if ((this->av1.actionVar1 != 0) && (sp80 != 0)) {
                    anim2 = this->ageProperties->unk_BC[this->av2.actionVar2];

                    if (sp80 > 0) {
                        this->skelAnime.prevTransl = this->ageProperties->unk_7A[this->av2.actionVar2];
                        anime_init_standard_stop(play, this, anim2);
                    } else {
                        this->skelAnime.prevTransl = this->ageProperties->unk_86[this->av2.actionVar2];
                        Skeleton_Info_Rom_init(play, &this->skelAnime, anim2, -1.0f, Si2_anime_end_frame(anim2), 0.0f,
                                             ANIMMODE_ONCE, 0.0f);
                    }
                } else {
                    this->stateFlags2 |= PLAYER_STATE2_12;
                }
            }

            return;
        }
    }

    if (this->av2.actionVar2 < 0) {
        if (((this->av2.actionVar2 == -2) &&
             (Skeleton_Info_Rom_frame_check(&this->skelAnime, 14.0f) || Skeleton_Info_Rom_frame_check(&this->skelAnime, 29.0f))) ||
            ((this->av2.actionVar2 == -4) &&
             (Skeleton_Info_Rom_frame_check(&this->skelAnime, 22.0f) || Skeleton_Info_Rom_frame_check(&this->skelAnime, 35.0f) ||
              Skeleton_Info_Rom_frame_check(&this->skelAnime, 49.0f) || Skeleton_Info_Rom_frame_check(&this->skelAnime, 55.0f)))) {
            climb_SE_set(this);
        }
        return;
    }

    if (Skeleton_Info_Rom_frame_check(&this->skelAnime, (this->skelAnime.playSpeed > 0.0f) ? 20.0f : 0.0f)) {
        climb_SE_set(this);
    }
}

void move_climb_end(Player* this, PlayState* play) {
    static f32 land_SE_set_status_data[] = { 10.0f, 20.0f };
    static f32 D_808548A0[] = { 40.0f, 50.0f };

    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_WOOD, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 10) },
        { NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_WOOD, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 20) },
        { NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_WOOD, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 30) },
    };

    s32 interruptResult;
    f32* sp38;
    CollisionPoly* groundPoly;
    s32 bgId;
    Vec3f sp24;

    this->stateFlags2 |= PLAYER_STATE2_6;

    interruptResult = to_next_action_cancel_check(play, this, &this->skelAnime, 4.0f);

    if (interruptResult == PLAYER_INTERRUPT_NEW_ACTION) {
        this->stateFlags1 &= ~PLAYER_STATE1_21;
        return;
    }

    if ((interruptResult >= PLAYER_INTERRUPT_MOVE) || Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        to_wait_no_morf_set(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_21;
        return;
    }

    sp38 = land_SE_set_status_data;

    if (this->av2.actionVar2 != 0) {
        player_anime_check_SE_set(this, SE_set_status);
        sp38 = D_808548A0;
    }

    if (Skeleton_Info_Rom_frame_check(&this->skelAnime, sp38[0]) || Skeleton_Info_Rom_frame_check(&this->skelAnime, sp38[1])) {
        sp24.x = this->actor.world.pos.x;
        sp24.y = this->actor.world.pos.y + 20.0f;
        sp24.z = this->actor.world.pos.z;
        if (T_BGCheck_ObjGroundCheck_ai(&play->colCtx, &groundPoly, &bgId, &sp24) != 0.0f) {
            //! @bug should use `T_BGCheck_getSoundGroundLabel` instead of `T_BGCheck_getTypeSoundCode_ai`.
            // Most material and sfxOffsets share identical enum values,
            // so this will mostly result in the correct sfx played, but not in all cases, such as carpet and ice.
            this->floorSfxOffset = T_BGCheck_getTypeSoundCode_ai(&play->colCtx, groundPoly, bgId);
            player_land_SE_set(this);
        }
    }
}

/**
 * Update player's animation while entering the crawlspace.
 * Once inside, stop all player animations and update player's movement.
 */
void move_tunnel(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 40) },   { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 48) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 56) },   { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 64) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 72) },   { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 80) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 88) },   { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 96) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 104) },
    };

    this->stateFlags2 |= PLAYER_STATE2_6;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_0)) {
            // While inside a crawlspace, player's skeleton does not move
            if (this->skelAnime.movementFlags != 0) {
                this->skelAnime.movementFlags = 0;
                return;
            }

            if (!to_tunnel_end_check(this, play)) {
                // Move forward and back while inside the crawlspace
                this->speedXZ = pad->rel.stick_y * 0.03f;
            }
        }
        return;
    }

    // Still entering crawlspace
    player_anime_check_SE_set(this, SE_set_status);
}

/**
 * Update player's animation while leaving the crawlspace.
 */
void move_tunnel_end(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 10) },  { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 18) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 26) },  { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 34) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 52) },  { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 60) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 68) },  { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 76) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 84) },
    };

    this->stateFlags2 |= PLAYER_STATE2_6;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        // Player is finished exiting the crawlspace and control is returned
        to_wait_no_morf_set(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_CRAWLING;
        return;
    }

    // Continue animation of leaving crawlspace
    player_anime_check_SE_set(this, SE_set_status);
}

static Vec3f ground_check_pos[] = {
    { 40.0f, 0.0f, 0.0f },
    { -40.0f, 0.0f, 0.0f },
};

static Vec3f hi_wall_check_pos[] = {
    { 60.0f, 20.0f, 0.0f },
    { -60.0f, 20.0f, 0.0f },
};

static Vec3f lo_wall_check_pos[] = {
    { 60.0f, -20.0f, 0.0f },
    { -60.0f, -20.0f, 0.0f },
};

int ride_horse_end_condition_check(PlayState* play, Player* this, s32 arg2, f32* arg3) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    f32 sp50;
    f32 sp4C;
    Vec3f sp40;
    Vec3f sp34;
    CollisionPoly* sp30;
    s32 sp2C;

    sp50 = rideActor->actor.world.pos.y + 20.0f;
    sp4C = rideActor->actor.world.pos.y - 20.0f;

    *arg3 = player_ground_BGcheck(play, this, &ground_check_pos[arg2], &sp40);

    return (sp4C < *arg3) && (*arg3 < sp50) &&
           !player_wall_BGcheck(play, this, &hi_wall_check_pos[arg2], &sp30, &sp2C, &sp34) &&
           !player_wall_BGcheck(play, this, &lo_wall_check_pos[arg2], &sp30, &sp2C, &sp34);
}

s32 to_ride_horse_end_check(Player* this, PlayState* play) {
    EnHorse* rideActor = (EnHorse*)this->rideActor;
    s32 sp38;
    f32 sp34;

    if (this->av2.actionVar2 < 0) {
        this->av2.actionVar2 = 99;
    } else {
        sp38 = (this->mountSide < 0) ? 0 : 1;
        if (!ride_horse_end_condition_check(play, this, sp38, &sp34)) {
            sp38 ^= 1;
            if (!ride_horse_end_condition_check(play, this, sp38, &sp34)) {
                return 0;
            } else {
                this->mountSide = -this->mountSide;
            }
        }

        if ((play->csCtx.state == CS_STATE_IDLE) && (play->transitionMode == TRANS_MODE_OFF) &&
            (EN_HORSE_CHECK_1(rideActor) || EN_HORSE_CHECK_4(rideActor))) {
            this->stateFlags2 |= PLAYER_STATE2_22;

            if (EN_HORSE_CHECK_1(rideActor) ||
                (EN_HORSE_CHECK_4(rideActor) && CHECK_BTN_ALL(pad->press.button, BTN_A))) {
                rideActor->actor.child = NULL;
                Player_actor_anime_move_save_set_process(play, this, move_ride_horse_end, 0);
                this->unk_878 = sp34 - rideActor->actor.world.pos.y;
                anime_init_standard_stop(play, this,
                                    (this->mountSide < 0) ? &gPlayerAnim_link_uma_left_down
                                                          : &gPlayerAnim_link_uma_right_down);
                return 1;
            }
        }
    }

    return 0;
}

void horse_offset_calc(Player* this, f32 arg1, f32 arg2) {
    f32 temp;
    f32 dir;

    if ((this->unk_878 != 0.0f) && (arg2 <= this->skelAnime.curFrame)) {
        if (arg1 < fabsf(this->unk_878)) {
            if (this->unk_878 >= 0.0f) {
                dir = 1.0f;
            } else {
                dir = -1.0f;
            }
            temp = dir * arg1;
        } else {
            temp = this->unk_878;
        }
        this->actor.world.pos.y += temp;
        this->unk_878 -= temp;
    }
}

static LinkAnimationHeader* ride_horse_anm[] = {
    &gPlayerAnim_link_uma_anim_stop,
    &gPlayerAnim_link_uma_anim_stand,
    &gPlayerAnim_link_uma_anim_walk,
    &gPlayerAnim_link_uma_anim_slowrun,
    &gPlayerAnim_link_uma_anim_fastrun,
    &gPlayerAnim_link_uma_anim_jump100,
    &gPlayerAnim_link_uma_anim_jump200,
    NULL,
    NULL,
};

static LinkAnimationHeader* ride_horse_muti_anm[] = {
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_walk_muti,
    &gPlayerAnim_link_uma_anim_slowrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    &gPlayerAnim_link_uma_anim_fastrun_muti,
    NULL,
    NULL,
};

static LinkAnimationHeader* ride_horse_wait_anm[] = {
    &gPlayerAnim_link_uma_wait_3,
    &gPlayerAnim_link_uma_wait_1,
    &gPlayerAnim_link_uma_wait_2,
};

static u8 ride_start_SE_set_data[2][2] = {
    { 32, 58 },
    { 25, 42 },
};

static Vec3s ride_horse_base_trans = { -69, 7146, -266 };

void move_ride_horse(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 48) },
        { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 58) },
        { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 68) },
        { NA_SE_PL_CALM_PAT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 92) },
        { NA_SE_PL_CALM_PAT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 110) },
        { NA_SE_PL_CALM_PAT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 126) },
        { NA_SE_PL_CALM_PAT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 132) },
        { NA_SE_PL_CALM_PAT, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 136) },
    };

    EnHorse* rideActor = (EnHorse*)this->rideActor;
    u8* arr;

    this->stateFlags2 |= PLAYER_STATE2_6;

    horse_offset_calc(this, 1.0f, 10.0f);

    if (this->av2.actionVar2 == 0) {
        if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
            this->skelAnime.animation = &gPlayerAnim_link_uma_wait_1;
            this->av2.actionVar2 = 99;
            return;
        }

        arr = ride_start_SE_set_data[(this->mountSide < 0) ? 0 : 1];

        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, arr[0])) {
            player_SE_set(this, NA_SE_PL_CLIMB_CLIFF);
            return;
        }

        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, arr[1])) {
            player_make_horse_camera(play, this);
            player_SE_set(this, NA_SE_PL_SIT_ON_HORSE);
            return;
        }

        return;
    }

    player_make_horse_camera(play, this);
    this->skelAnime.prevTransl = ride_horse_base_trans;

    if ((rideActor->animationIdx != this->av2.actionVar2) &&
        ((rideActor->animationIdx >= 2) || (this->av2.actionVar2 >= 2))) {
        if ((this->av2.actionVar2 = rideActor->animationIdx) < 2) {
            f32 rand = fqrand();
            s32 temp = 0;

            this->av2.actionVar2 = 1;

            if (rand < 0.1f) {
                temp = 2;
            } else if (rand < 0.2f) {
                temp = 1;
            }
            anime_init_standard_stop(play, this, ride_horse_wait_anm[temp]);
        } else {
            this->skelAnime.animation = ride_horse_anm[this->av2.actionVar2 - 2];
            Skeleton_Info_Rom_morf_ratio_set(play, &this->skelAnime, 8.0f);
            if (this->av2.actionVar2 < 4) {
                Player_actor_uperbody_process_init(play, this);
                this->av1.actionVar1 = 0;
            }
        }
    }

    if (this->av2.actionVar2 == 1) {
        if (uperbody_action_flag || player_talking_now(play)) {
            anime_init_standard_stop(play, this, &gPlayerAnim_link_uma_wait_3);
        } else if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
            this->av2.actionVar2 = 99;
        } else if (this->skelAnime.animation == &gPlayerAnim_link_uma_wait_1) {
            player_anime_check_SE_set(this, SE_set_status);
        }
    } else {
        this->skelAnime.curFrame = rideActor->curFrame;
        Skeleton_Info_Rom_anime_read(play, &this->skelAnime);
    }

    Skeleton_Proc_Copy_init(play, this->skelAnime.limbCount, this->skelAnime.morphTable, this->skelAnime.jointTable);

    if ((play->csCtx.state != CS_STATE_IDLE) || (this->csAction != PLAYER_CSACTION_NONE)) {
        if (this->csAction == PLAYER_CSACTION_7) {
            this->csAction = PLAYER_CSACTION_NONE;
        }
        this->unk_6AD = 0;
        this->av1.actionVar1 = 0;
    } else if ((this->av2.actionVar2 < 2) || (this->av2.actionVar2 >= 4)) {
        uperbody_action_flag = uperbody_action_check(this, play);

        if (uperbody_action_flag) {
            this->av1.actionVar1 = 0;
        }
    }

    this->actor.world.pos.x = rideActor->actor.world.pos.x + rideActor->riderPos.x;
    this->actor.world.pos.y = (rideActor->actor.world.pos.y + rideActor->riderPos.y) - 27.0f;
    this->actor.world.pos.z = rideActor->actor.world.pos.z + rideActor->riderPos.z;

    this->yaw = this->actor.shape.rot.y = rideActor->actor.shape.rot.y;

    if ((this->csAction != PLAYER_CSACTION_NONE) ||
        (!player_talking_now(play) && ((rideActor->actor.speed != 0.0f) || !to_talk_check(this, play)) &&
         !to_check_check(this, play))) {
        if (!uperbody_action_flag) {
            if (this->av1.actionVar1 != 0) {
                if (Skeleton_Info_Rom_anime_play(play, &this->upperSkelAnime)) {
                    rideActor->stateFlags &= ~ENHORSE_FLAG_8;
                    this->av1.actionVar1 = 0;
                }

                if (this->upperSkelAnime.animation == &gPlayerAnim_link_uma_stop_muti) {
                    if (Skeleton_Info_Rom_frame_check(&this->upperSkelAnime, 23.0f)) {
                        player_SE_set(this, NA_SE_IT_LASH);
                        player_voice_SE_set(this, NA_SE_VO_LI_LASH);
                    }

                    Skeleton_Proc_Copy_init(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                          this->upperSkelAnime.jointTable);
                } else {
                    if (Skeleton_Info_Rom_frame_check(&this->upperSkelAnime, 10.0f)) {
                        player_SE_set(this, NA_SE_IT_LASH);
                        player_voice_SE_set(this, NA_SE_VO_LI_LASH);
                    }

                    Skeleton_Proc_Add_init(play, this->skelAnime.limbCount, this->skelAnime.jointTable,
                                                  this->upperSkelAnime.jointTable, link_joint_status);
                }
            } else {
                LinkAnimationHeader* anim = NULL;

                if (EN_HORSE_CHECK_3(rideActor)) {
                    anim = &gPlayerAnim_link_uma_stop_muti;
                } else if (EN_HORSE_CHECK_2(rideActor)) {
                    if ((this->av2.actionVar2 >= 2) && (this->av2.actionVar2 != 99)) {
                        anim = ride_horse_muti_anm[this->av2.actionVar2 - 2];
                    }
                }

                if (anim != NULL) {
                    Skeleton_Info_Rom_init_standard_stop(play, &this->upperSkelAnime, anim);
                    this->av1.actionVar1 = 1;
                }
            }
        }

        if (this->stateFlags1 & PLAYER_STATE1_20) {
            if ((subject_camera_mode_check(play, this) == CAM_MODE_NORMAL) || CHECK_BTN_ANY(pad->press.button, BTN_A) ||
                anchor_parallel_mode_check(this)) {
                this->unk_6AD = 0;
                this->stateFlags1 &= ~PLAYER_STATE1_20;
            } else {
                this->upperLimbRot.y = look_eye_move(play, this, true, -5000) - this->actor.shape.rot.y;
                this->upperLimbRot.y += 5000;
                this->upperLimbYawSecondary = -5000;
            }
            return;
        }

        if ((this->csAction != PLAYER_CSACTION_NONE) ||
            (!to_ride_horse_end_check(this, play) && !to_look_set_check(this, play))) {
            if (this->focusActor != NULL) {
                if (player_bow_ready_check(this)) {
                    this->upperLimbRot.y = anchor_eye_move(this, true) - this->actor.shape.rot.y;
                    this->upperLimbRot.y = CLAMP(this->upperLimbRot.y, -0x4AAA, 0x4AAA);
                    this->actor.focus.rot.y = this->actor.shape.rot.y + this->upperLimbRot.y;
                    this->upperLimbRot.y += 5000;
                    this->unk_6AE_rotFlags |= UNK6AE_ROT_UPPER_Y;
                } else {
                    anchor_eye_move(this, false);
                }
            } else {
                if (player_bow_ready_check(this)) {
                    this->upperLimbRot.y = look_eye_move(play, this, true, -5000) - this->actor.shape.rot.y;
                    this->upperLimbRot.y += 5000;
                    this->upperLimbYawSecondary = -5000;
                }
            }
        }
    }
}

void move_ride_horse_end(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 0) },
        { NA_SE_PL_GET_OFF_HORSE, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 10) },
        { NA_SE_PL_SLIPDOWN, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 25) },
    };

    this->stateFlags2 |= PLAYER_STATE2_6;
    horse_offset_calc(this, 1.0f, 10.0f);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        EnHorse* rideActor = (EnHorse*)this->rideActor;

        to_wait_no_morf_set(this, play);
        this->stateFlags1 &= ~PLAYER_STATE1_23;
        this->actor.parent = NULL;
        R_EXITED_SCENE_RIDING_HORSE = false;

        if (event_check(EVENTCHKINF_EPONA_OBTAINED) || R_DEBUG_FORCE_EPONA_OBTAINED) {
            z_common_data.save.info.horseData.pos.x = rideActor->actor.world.pos.x;
            z_common_data.save.info.horseData.pos.y = rideActor->actor.world.pos.y;
            z_common_data.save.info.horseData.pos.z = rideActor->actor.world.pos.z;
            z_common_data.save.info.horseData.angle = rideActor->actor.shape.rot.y;
        }
    } else {
        changeCameraSet(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_SET_NORMAL0);

        if (this->mountSide < 0) {
            SE_set_status[0].data = ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 40);
        } else {
            SE_set_status[0].data = ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 29);
        }
        player_anime_check_SE_set(this, SE_set_status);
    }
}

void swim_control_SE_set(Player* this, f32* arg1, f32 arg2, s16 arg3) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_PL_SWIM, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 0) },
    };

    swim_control(this, arg1, arg2, arg3);
    player_anime_check_SE_set(this, SE_set_status);
}

void to_swim_set(PlayState* play, Player* this, s16 arg2) {
    Player_actor_set_process(play, this, move_swim, 0);
    this->actor.shape.rot.y = this->yaw = arg2;
    anime_init_0_repeat_basic(play, this, &gPlayerAnim_link_swimer_swim);
}

void to_anchor_swim_set(PlayState* play, Player* this) {
    Player_actor_set_process(play, this, move_anchor_swim, 0);
    anime_init_0_repeat_basic(play, this, &gPlayerAnim_link_swimer_swim);
}

void move_swim_wait(Player* this, PlayState* play) {
    anime_play_repeat(play, this, &gPlayerAnim_link_swimer_swim_wait);
    water_pressure_set(this);

    if (!player_talking_now(play) && !action_select_check(play, this, swim_action_check, true) &&
        !to_swim_deep_check(play, this, pad)) {
        f32 speedTarget;
        s16 yawTarget;

        if (this->unk_6AD != 1) {
            this->unk_6AD = 0;
        }

        if (this->currentBoots == PLAYER_BOOTS_IRON) {
            speedTarget = 0.0f;
            yawTarget = this->actor.shape.rot.y;

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                to_wait_set_check_anim(this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_short_landing, this->modelAnimType), play);
                player_land_SE_set(this);
            }
        } else {
            stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

            if (speedTarget != 0.0f) {
                s16 temp = this->actor.shape.rot.y - yawTarget;

                if ((ABS(temp) > 0x6000) && !chase_f(&this->speedXZ, 0.0f, 1.0f)) {
                    return;
                }

                if (anchor_parallel_mode_check_set(this)) {
                    to_anchor_swim_set(play, this);
                } else {
                    to_swim_set(play, this, yawTarget);
                }
            }
        }

        swim_control(this, &this->speedXZ, speedTarget, yawTarget);
    }
}

void move_swim_demo(Player* this, PlayState* play) {
    if (!to_look_set_check(this, play)) {
        this->stateFlags2 |= PLAYER_STATE2_5;

        swim_anime_play(play, this, NULL, this->speedXZ);
        water_pressure_set(this);

        if (DECR(this->av2.actionVar2) == 0) {
            to_swim_wait_set(play, this);
        }
    }
}

void move_swim(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;
    s16 temp;

    this->stateFlags2 |= PLAYER_STATE2_5;

    swim_anime_play(play, this, pad, this->speedXZ);
    water_pressure_set(this);

    if (!action_select_check(play, this, swim_action_check, true) &&
        !to_swim_deep_check(play, this, pad)) {
        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        temp = this->actor.shape.rot.y - yawTarget;
        if ((speedTarget == 0.0f) || (ABS(temp) > 0x6000) || (this->currentBoots == PLAYER_BOOTS_IRON)) {
            to_swim_wait_set(play, this);
        } else if (anchor_parallel_mode_check_set(this)) {
            to_anchor_swim_set(play, this);
        }

        swim_control_SE_set(this, &this->speedXZ, speedTarget, yawTarget);
    }
}

s32 anchor_swim_anime_check(PlayState* play, Player* this, f32* arg2, s16* arg3) {
    LinkAnimationHeader* anim;
    s16 temp1;
    s32 temp2;

    temp1 = this->yaw - *arg3;

    if (ABS(temp1) > 0x6000) {
        anim = &gPlayerAnim_link_swimer_swim_wait;

        if (chase_f(&this->speedXZ, 0.0f, 1.0f)) {
            this->yaw = *arg3;
        } else {
            *arg2 = 0.0f;
            *arg3 = this->yaw;
        }
    } else {
        temp2 = parallel_mode_key_check(this, arg2, arg3, play);

        if (temp2 > 0) {
            anim = &gPlayerAnim_link_swimer_swim;
        } else if (temp2 < 0) {
            anim = &gPlayerAnim_link_swimer_back_swim;
        } else if ((temp1 = this->actor.shape.rot.y - *arg3) > 0) {
            anim = &gPlayerAnim_link_swimer_Rside_swim;
        } else {
            anim = &gPlayerAnim_link_swimer_Lside_swim;
        }
    }

    if (anim != this->skelAnime.animation) {
        anime_init_0_repeat_basic(play, this, anim);
        return 1;
    }

    return 0;
}

void move_anchor_swim(Player* this, PlayState* play) {
    f32 speedTarget;
    s16 yawTarget;

    swim_anime_play(play, this, pad, this->speedXZ);
    water_pressure_set(this);

    if (!action_select_check(play, this, swim_action_check, true) &&
        !to_swim_deep_check(play, this, pad)) {
        stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);

        if (speedTarget == 0.0f) {
            to_swim_wait_set(play, this);
        } else if (!anchor_parallel_mode_check_set(this)) {
            to_swim_set(play, this, yawTarget);
        } else {
            anchor_swim_anime_check(play, this, &speedTarget, &yawTarget);
        }

        swim_control_SE_set(this, &this->speedXZ, speedTarget, yawTarget);
    }
}

void deep_swim_control(PlayState* play, Player* this, f32 arg2) {
    f32 speedTarget;
    s16 yawTarget;

    stick_control_check_anchor(this, &speedTarget, &yawTarget, SPEED_MODE_LINEAR, play);
    swim_control(this, &this->speedXZ, speedTarget * 0.5f, yawTarget);
    swim_control(this, &this->actor.velocity.y, arg2, this->yaw);
}

void move_swim_deep(Player* this, PlayState* play) {
    f32 sp2C;

    this->stateFlags2 |= PLAYER_STATE2_5;
    this->actor.gravity = 0.0f;
    uperbody_action_check(this, play);

    if (!to_look_set_check(this, play)) {
        if (this->currentBoots == PLAYER_BOOTS_IRON) {
            to_swim_wait_set(play, this);
            return;
        }

        if (this->av1.actionVar1 == 0) {
            if (this->av2.actionVar2 == 0) {
                if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime) ||
                    ((this->skelAnime.curFrame >= 22.0f) && !CHECK_BTN_ALL(pad->cur.button, BTN_A))) {
                    deep_swim_up_init(play, this);
                } else if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 20.0f) != 0) {
                    this->actor.velocity.y = -2.0f;
                }

                ground_breaking_speedF(this);
                return;
            }

            swim_anime_play(play, this, pad, this->actor.velocity.y);
            this->unk_6C2 = 16000;

            if (CHECK_BTN_ALL(pad->cur.button, BTN_A) && !to_carry_check(this, play) &&
                !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
                (this->actor.depthInWater < swim_deep_y[CUR_UPG_VALUE(UPG_SCALE)])) {
                deep_swim_control(play, this, -2.0f);
            } else {
                this->av1.actionVar1++;
                anime_init_0_repeat_basic(play, this, &gPlayerAnim_link_swimer_swim_wait);
            }
        } else if (this->av1.actionVar1 == 1) {
            Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
            water_pressure_set(this);

            if (this->unk_6C2 < 10000) {
                this->av1.actionVar1++;
                this->av2.actionVar2 = this->actor.depthInWater;
                anime_init_0_repeat_basic(play, this, &gPlayerAnim_link_swimer_swim);
            }
        } else if (!to_swim_deep_check(play, this, pad)) {
            sp2C = (this->av2.actionVar2 * 0.018f) + 4.0f;

            if (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
                pad = NULL;
            }

            swim_anime_play(play, this, pad, fabsf(this->actor.velocity.y));
            chase_angle(&this->unk_6C2, -10000, 800);

            if (sp2C > 8.0f) {
                sp2C = 8.0f;
            }

            deep_swim_control(play, this, sp2C);
        }
    }
}

void get_item_end_set(PlayState* play, Player* this) {
    this->unk_862 = 0;
    this->stateFlags1 &= ~(PLAYER_STATE1_10 | PLAYER_STATE1_CARRYING_ACTOR);
    this->getItemId = GI_NONE;
    restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));
}

void to_get_item_end_set(PlayState* play, Player* this) {
    get_item_end_set(play, this);
    anime_morf_end_set(this);
    to_wait_no_morf_set(this, play);
    this->yaw = this->actor.shape.rot.y;
}

s32 get_item_message_check(PlayState* play, Player* this) {
    GetItemEntry* giEntry;
    s32 temp1;
    s32 temp2;

    if (this->getItemId == GI_NONE) {
        return 1;
    }

    if (this->av1.actionVar1 == 0) {
        giEntry = &get_item_status[this->getItemId - 1];
        this->av1.actionVar1 = 1;

        message_set(play, giEntry->textId, &this->actor);
        item_get_setting(play, giEntry->itemId);

        if (((this->getItemId >= GI_RUPEE_GREEN) && (this->getItemId <= GI_RUPEE_RED)) ||
            ((this->getItemId >= GI_RUPEE_PURPLE) && (this->getItemId <= GI_RUPEE_GOLD)) ||
            ((this->getItemId >= GI_RUPEE_GREEN_LOSE) && (this->getItemId <= GI_RUPEE_PURPLE_LOSE)) ||
            (this->getItemId == GI_RECOVERY_HEART)) {
            Nai_FxFlagEntry(NA_SE_SY_GET_BOXITEM, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            if ((this->getItemId == GI_HEART_CONTAINER_2) || (this->getItemId == GI_HEART_CONTAINER) ||
                ((this->getItemId == GI_HEART_PIECE) &&
                 ((z_common_data.save.info.inventory.questItems & 0xF0000000) == (4 << QUEST_HEART_PIECE_COUNT)))) {
                temp1 = NA_BGM_HEART_GET | 0x900;
            } else {
                temp1 = temp2 = (this->getItemId == GI_HEART_PIECE) ? NA_BGM_SMALL_ITEM_GET : NA_BGM_ITEM_GET | 0x900;
            }
            Na_StartFanfare(temp1);
        }
    } else {
        if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
            if (this->getItemId == GI_SILVER_GAUNTLETS) {
                play->nextEntranceIndex = ENTR_DESERT_COLOSSUS_0;
                play->transitionTrigger = TRANS_TRIGGER_START;
                z_common_data.nextCutsceneIndex = 0xFFF1;
                play->transitionType = TRANS_TYPE_SANDSTORM_END;
                this->stateFlags1 &= ~PLAYER_STATE1_29;
                Ext_player_demo_set(play, NULL, PLAYER_CSACTION_8);
            }
            this->getItemId = GI_NONE;
        }
    }

    return 0;
}

void move_swim_deep_end(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_5;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (!(this->stateFlags1 & PLAYER_STATE1_10) || get_item_message_check(play, this)) {
            get_item_end_set(play, this);
            to_swim_wait_set(play, this);
            sub_camera_mode_reset(play, this);
        }
    } else {
        if ((this->stateFlags1 & PLAYER_STATE1_10) && Skeleton_Info_Rom_frame_check(&this->skelAnime, 10.0f)) {
            get_item_shape_set(this, play);
            sub_camera_mode_reset(play, this);
            change_item_camera_check_set(play, 8);
        } else if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 5.0f)) {
            player_voice_SE_set(this, NA_SE_VO_LI_BREATH_DRINK);
        }
    }

    water_pressure_set(this);
    swim_control(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
}

void move_swim_damage(Player* this, PlayState* play) {
    water_pressure_set(this);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        to_swim_wait_set(play, this);
    }

    swim_control(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
}

void move_swim_down(Player* this, PlayState* play) {
    water_pressure_set(this);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        down_result_check(play, this);
    }

    swim_control(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
}

static s16 okarina_warp_data[] = {
    ENTR_SACRED_FOREST_MEADOW_2,
    ENTR_DEATH_MOUNTAIN_CRATER_4,
    ENTR_LAKE_HYLIA_8,
    ENTR_DESERT_COLOSSUS_5,
    ENTR_GRAVEYARD_7,
    ENTR_TEMPLE_OF_TIME_7,
};

void move_okarina(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_init_standard_repeat_3f(play, this, &gPlayerAnim_link_normal_okarina_swing);
        this->av2.actionVar2 = 1;
        if (this->stateFlags2 & (PLAYER_STATE2_23 | PLAYER_STATE2_25)) {
            this->stateFlags2 |= PLAYER_STATE2_24;
        } else {
            ocarina_set(play, OCARINA_ACTION_FREE_PLAY);
        }
        return;
    }

    if (this->av2.actionVar2 == 0) {
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));

        if ((this->talkActor != NULL) && (this->talkActor == this->unk_6A8)) {
            Ext_player_to_talk_set(play, this->talkActor);
        } else if (this->naviTextId < 0) {
            this->talkActor = this->naviActor;
            this->naviActor->textId = -this->naviTextId;
            Ext_player_to_talk_set(play, this->talkActor);
        } else if (!to_look_set_check(this, play)) {
            to_wait_set_check_anim(this, &gPlayerAnim_link_normal_okarina_end, play);
        }

        this->stateFlags2 &= ~(PLAYER_STATE2_23 | PLAYER_STATE2_24 | PLAYER_STATE2_25);
        this->unk_6A8 = NULL;
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_02) {
        s32 padding;

        z_common_data.respawn[RESPAWN_MODE_RETURN].entranceIndex = okarina_warp_data[play->msgCtx.lastPlayedSong];
        z_common_data.respawn[RESPAWN_MODE_RETURN].playerParams =
            PLAYER_PARAMS(PLAYER_START_MODE_WARP_SONG, PLAYER_START_BG_CAM_DEFAULT);
        z_common_data.respawn[RESPAWN_MODE_RETURN].data = play->msgCtx.lastPlayedSong;

        this->csAction = PLAYER_CSACTION_NONE;
        this->stateFlags1 &= ~PLAYER_STATE1_29;

        Ext_player_demo_set(play, NULL, PLAYER_CSACTION_8);
        play->mainCamera.stateFlags &= ~CAM_STATE_EXTERNAL_FINISHED;

        // Setting these flags again is necessary because `Ext_player_demo_set` calls
        // `Player_actor_set_process` which unsets the flags.
        this->stateFlags1 |= PLAYER_STATE1_28 | PLAYER_STATE1_29;
        this->stateFlags2 |= PLAYER_STATE2_USING_OCARINA;

        if (Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, DEMOKANKYO_WARP_OUT) ==
            NULL) {
            OcaWarpJumpProc(play);
        }

        z_common_data.seqId = (u8)NA_BGM_DISABLED;
        z_common_data.natureAmbienceId = NATURE_ID_DISABLED;
    }
}

void move_light_bom(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        to_wait_set_check_anim(this, &gPlayerAnim_link_normal_light_bom_end, play);
    } else if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 3.0f)) {
        item_count_vary(ITEM_DEKU_NUT, -1);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ARROW, this->bodyPartsPos[PLAYER_BODYPART_R_HAND].x,
                    this->bodyPartsPos[PLAYER_BODYPART_R_HAND].y, this->bodyPartsPos[PLAYER_BODYPART_R_HAND].z, 4000,
                    this->actor.shape.rot.y, 0, ARROW_NUT);
        player_voice_SE_set(this, NA_SE_VO_LI_SWORD_N);
    }

    ground_breaking_speedF(this);
}

void move_get_item(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_JUMPING, 87) },
        { NA_SE_VO_LI_CLIMB_END, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 87) },
        { NA_SE_VO_LI_AUTO_JUMP, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 69) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 123) },
    };

    s32 cond;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (this->av2.actionVar2 != 0) {
            if (this->av2.actionVar2 >= 2) {
                this->av2.actionVar2--;
            }

            if (get_item_message_check(play, this) && (this->av2.actionVar2 == 1)) {
                cond =
                    ((this->talkActor != NULL) && (this->exchangeItemId < 0)) || (this->stateFlags3 & PLAYER_STATE3_5);

                if (cond || (z_common_data.healthAccumulator == 0)) {
                    if (cond) {
                        get_item_end_set(play, this);
                        this->exchangeItemId = EXCH_ITEM_NONE;

                        if (to_okarina_set_check(play, this) == 0) {
                            Ext_player_to_talk_set(play, this->talkActor);
                        }
                    } else {
                        to_get_item_end_set(play, this);
                    }
                }
            }
        } else {
            anime_move_reset(this);

            if (this->getItemId == GI_ICE_TRAP) {
                this->stateFlags1 &= ~(PLAYER_STATE1_10 | PLAYER_STATE1_CARRYING_ACTOR);

                if (this->getItemId != GI_ICE_TRAP) {
                    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x,
                                this->actor.world.pos.y + 100.0f, this->actor.world.pos.z, 0, 0, 0, 0);
                    to_wait_no_morf_set(this, play);
                } else {
                    this->actor.colChkInfo.damage = 0;
                    damage_set(play, this, PLAYER_HIT_RESPONSE_ICE_TRAP, 0.0f, 0.0f, 0, 20);
                }
                return;
            }

            if (this->skelAnime.animation == &gPlayerAnim_link_normal_box_kick) {
                anime_init_standard_stop_3f(play, this, &gPlayerAnim_link_demo_get_itemB);
            } else {
                anime_init_standard_stop_3f(play, this, &gPlayerAnim_link_demo_get_itemA);
            }

            this->av2.actionVar2 = 2;
            change_item_camera_check_set(play, 9);
        }
    } else {
        if (this->av2.actionVar2 == 0) {
            if (!LINK_IS_ADULT) {
                player_anime_check_SE_set(this, SE_set_status);
            }
            return;
        }

        if (this->skelAnime.animation == &gPlayerAnim_link_demo_get_itemB) {
            chase_angle(&this->actor.shape.rot.y, getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000, 4000);
        }

        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 21.0f)) {
            get_item_shape_set(this, play);
        }
    }
}

void m_sword_end_SE_set(Player* this) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_IT_MASTER_SWORD_SWING, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 60) },
    };

    player_anime_check_SE_set(this, SE_set_status);
}

void move_m_sword_end(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (!this->av1.startedAnim) {
            if (DECR(this->av2.animDelayTimer) == 0) {
                this->av1.startedAnim = true;

                // endFrame was previously set to 0 to freeze the animation.
                // Set it properly to allow the animation to play.
                this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
            }
        } else {
            to_wait_no_morf_set(this, play);
        }
    } else {
        if (LINK_IS_ADULT && Skeleton_Info_Rom_frame_check(&this->skelAnime, 158.0f)) {
            player_voice_SE_set(this, NA_SE_VO_LI_SWORD_N);
            return;
        }

#if OOT_VERSION < PAL_1_0
        if (!LINK_IS_ADULT && Skeleton_Info_Rom_frame_check(&this->skelAnime, 5.0f)) {
            // There is a jump sound when leaving the pedestal, but no landing sound when hitting the floor.
            // This is fixed in PAL 1.0 and above.
            player_voice_SE_set(this, NA_SE_VO_LI_AUTO_JUMP);
        } else if (LINK_IS_ADULT) {
            m_sword_end_SE_set(this);
        }
#else
        if (!LINK_IS_ADULT) {
            static AnimSfxEntry SE_set_status[] = {
                { NA_SE_VO_LI_AUTO_JUMP, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 5) },
                { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 15) },
            };

            player_anime_check_SE_set(this, SE_set_status);
        } else {
            m_sword_end_SE_set(this);
        }
#endif
    }
}

void move_bottle_drink(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (this->av2.actionVar2 == 0) {
            static u8 bottle_info_data[] = {
                0x01, 0x03, 0x02, 0x04, 0x04,
            };

            if (this->itemAction == PLAYER_IA_BOTTLE_POE) {
                s32 rand = get_random_timer(-1, 3);

                if (rand == 0) {
                    rand = 3;
                }

                if ((rand < 0) && (z_common_data.save.info.playerData.health <= 0x10)) {
                    rand = 3;
                }

                if (rand < 0) {
                    life_meter_play(play, -0x10);
                } else {
                    z_common_data.healthAccumulator = rand * 0x10;
                }
            } else {
                s32 sp28 = bottle_info_data[this->itemAction - PLAYER_IA_BOTTLE_POTION_RED];

                if (sp28 & 1) {
                    z_common_data.healthAccumulator = 0x140;
                }

                if (sp28 & 2) {
                    magic_mode_check(play);
                }

                if (sp28 & 4) {
                    z_common_data.healthAccumulator = 0x50;
                }
            }

            anime_init_standard_repeat_3f(play, this, &gPlayerAnim_link_bottle_drink_demo_wait);
            this->av2.actionVar2 = 1;
        } else {
            to_wait_no_morf_set(this, play);
            restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));
        }
    } else if (this->av2.actionVar2 == 1) {
        if ((z_common_data.healthAccumulator == 0) && (z_common_data.magicState != MAGIC_STATE_FILL)) {
            anime_init_stop_basic_3f(play, this, &gPlayerAnim_link_bottle_drink_demo_end);
            this->av2.actionVar2 = 2;
            bottle_item_change(play, this, ITEM_BOTTLE_EMPTY, PLAYER_IA_BOTTLE);
        }
        player_voice_SE_set(this, NA_SE_VO_LI_DRINK - SFX_FLAG);
    } else if ((this->av2.actionVar2 == 2) && Skeleton_Info_Rom_frame_check(&this->skelAnime, 29.0f)) {
        player_voice_SE_set(this, NA_SE_VO_LI_BREATH_DRINK);
    }
}

typedef enum BottleCatchType {
    BOTTLE_CATCH_NONE, // This type does not have an associated entry in `bottle_catch_status`
    BOTTLE_CATCH_FAIRY,
    BOTTLE_CATCH_FISH,
    BOTTLE_CATCH_BLUE_FIRE,
    BOTTLE_CATCH_BUGS
} BottleCatchType;

typedef struct BottleCatchInfo {
    /* 0x00 */ s16 actorId;
    /* 0x02 */ u8 itemId;
    /* 0x03 */ u8 itemAction;
    /* 0x04 */ u8 textId;
} BottleCatchInfo; // size = 0x06

static BottleCatchInfo bottle_catch_status[] = {
    { ACTOR_EN_ELF, ITEM_BOTTLE_FAIRY, PLAYER_IA_BOTTLE_FAIRY, 0x46 },         // BOTTLE_CATCH_FAIRY
    { ACTOR_EN_FISH, ITEM_BOTTLE_FISH, PLAYER_IA_BOTTLE_FISH, 0x47 },          // BOTTLE_CATCH_FISH
    { ACTOR_EN_ICE_HONO, ITEM_BOTTLE_BLUE_FIRE, PLAYER_IA_BOTTLE_FIRE, 0x5D }, // BOTTLE_CATCH_BLUE_FIRE
    { ACTOR_EN_INSECT, ITEM_BOTTLE_BUG, PLAYER_IA_BOTTLE_BUG, 0x7A },          // BOTTLE_CATCH_BUGS
};

void move_bottle_attack(Player* this, PlayState* play) {
    // Action Variable 2 has two separate uses within the same action.
    // After it is used as `inWater` here, it will be used for `startedTextbox` below.
    // The two usages will never overlap, so this won't cause any issues.
    BottleSwingInfo* swingEntry = &bottle_attack_status[this->av2.inWater];

    ground_breaking_speedF(this);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (this->av1.bottleCatchType != BOTTLE_CATCH_NONE) {
            if (!this->av2.startedTextbox) {
                // 1 is subtracted because `bottle_catch_status` does not have an entry for `BOTTLE_CATCH_NONE`
                message_set(play, bottle_catch_status[this->av1.bottleCatchType - 1].textId, &this->actor);
                Na_StartFanfare(NA_BGM_ITEM_GET | 0x900);
                this->av2.startedTextbox = true;
            } else if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
                this->av1.bottleCatchType = BOTTLE_CATCH_NONE;
                restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));
            }
        } else {
            to_wait_no_morf_set(this, play);
        }
    } else if (this->av1.bottleCatchType == BOTTLE_CATCH_NONE) {
        s32 activeFrame = this->skelAnime.curFrame - swingEntry->firstActiveFrame;

        if (activeFrame >= 0 && activeFrame <= swingEntry->numActiveFrames) {
            if (this->av2.inWater && activeFrame == 0) {
                // Play water scoop sound on the first active frame, if applicable
                player_SE_set(this, NA_SE_IT_SCOOP_UP_WATER);
            }

            // `interactRangeActor` will be set by the Get Item system. See `Actor_carry_request_set2`.
            if (this->interactRangeActor != NULL) {
                BottleCatchInfo* catchInfo = &bottle_catch_status[0];
                s32 i;

                // Try to find an `interactRangeActor` with the same ID as an entry in `bottle_catch_status`
                for (i = 0; i < ARRAY_COUNT(bottle_catch_status); i++, catchInfo++) {
                    if (this->interactRangeActor->id == catchInfo->actorId) {
                        break;
                    }
                }

                if (i < ARRAY_COUNT(bottle_catch_status)) {
                    // 1 is added because `bottle_catch_status` does not have an entry for `BOTTLE_CATCH_NONE`
                    this->av1.bottleCatchType = i + 1;

                    this->av2.startedTextbox = false;
                    this->stateFlags1 |= PLAYER_STATE1_28 | PLAYER_STATE1_29;
                    this->interactRangeActor->parent = &this->actor;

                    bottle_item_change(play, this, catchInfo->itemId, ABS(catchInfo->itemAction));
                    anime_init_standard_stop_3f(play, this, swingEntry->catchAnimation);
                    change_item_camera_check_set(play, 4);
                }
            }
        }
    }

    //! @bug If the animation is changed at any point above (such as by to_wait_no_morf_set() or
    //! anime_init_standard_stop_3f()), it will change the curFrame to 0. This causes this flag to be set for one frame,
    //! at a time when it does not look like Player is swinging the bottle.
    if (this->skelAnime.curFrame <= 7.0f) {
        this->stateFlags1 |= PLAYER_STATE1_SWINGING_BOTTLE;
    }
}

static Vec3f life_up_elf_set_offset = { 0.0f, 0.0f, 5.0f };

void move_bottle_bug_out(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        to_wait_no_morf_set(this, play);
        restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));
        return;
    }

    if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 37.0f)) {
        make_elf(play, this, &this->leftHandPos, &life_up_elf_set_offset, FAIRY_REVIVE_BOTTLE);
        bottle_item_change(play, this, ITEM_BOTTLE_EMPTY, PLAYER_IA_BOTTLE);
        player_SE_set(this, NA_SE_EV_BOTTLE_CAP_OPEN);
        player_SE_set(this, NA_SE_EV_FIATY_HEAL - SFX_FLAG);
    } else if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 47.0f)) {
        z_common_data.healthAccumulator = 0x140;
    }
}

static BottleDropInfo bottle_out_status[] = {
    { ACTOR_EN_FISH, FISH_DROPPED },
    { ACTOR_EN_ICE_HONO, 0 },
    { ACTOR_EN_INSECT, INSECT_TYPE_FIRST_DROPPED },
};

void move_bottle_fish_out(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_VO_LI_AUTO_JUMP, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 38) },
        { NA_SE_EV_BOTTLE_CAP_OPEN, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 40) },
    };

    ground_breaking_speedF(this);

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        to_wait_no_morf_set(this, play);
        restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));
    } else if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 76.0f)) {
        BottleDropInfo* dropInfo = &bottle_out_status[this->itemAction - PLAYER_IA_BOTTLE_FISH];

        Actor_info_make_actor(&play->actorCtx, play, dropInfo->actorId,
                    (sin_s(this->actor.shape.rot.y) * 5.0f) + this->leftHandPos.x, this->leftHandPos.y,
                    (cos_s(this->actor.shape.rot.y) * 5.0f) + this->leftHandPos.z, 0x4000, this->actor.shape.rot.y,
                    0, dropInfo->actorParams);

        bottle_item_change(play, this, ITEM_BOTTLE_EMPTY, PLAYER_IA_BOTTLE);
    } else {
        player_anime_check_SE_set(this, SE_set_status);
    }
}

void move_put_item(Player* this, PlayState* play) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_PL_PUT_OUT_ITEM, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 30) },
    };

    this->stateFlags2 |= PLAYER_STATE2_5;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (this->av2.actionVar2 < 0) {
            to_wait_no_morf_set(this, play);
        } else if (this->exchangeItemId == EXCH_ITEM_NONE) {
            Actor* talkActor = this->talkActor;

            this->unk_862 = 0;
            if (talkActor->textId != 0xFFFF) {
                this->actor.flags |= ACTOR_FLAG_TALK;
            }

            Ext_player_to_talk_set(play, talkActor);
        } else {
            GetItemEntry* giEntry = &get_item_status[put_item_status[this->exchangeItemId - 1] - 1];

            if (this->itemAction >= PLAYER_IA_ZELDAS_LETTER) {
                this->unk_862 = ABS(giEntry->gi);
            }

            if (this->av2.actionVar2 == 0) {
                s32 padding;

                message_set(play, this->actor.textId, &this->actor);

                if ((this->itemAction == PLAYER_IA_CHICKEN) || (this->itemAction == PLAYER_IA_POCKET_CUCCO)) {
                    player_SE_set(this, NA_SE_EV_CHICKEN_CRY_M);
                }

                this->av2.actionVar2 = 1;
            } else if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
                this->actor.flags &= ~ACTOR_FLAG_TALK;
                this->unk_862 = 0;

                if (this->av1.actionVar1 == 1) {
                    anime_init_standard_stop(play, this, &gPlayerAnim_link_bottle_read_end);
                    this->av2.actionVar2 = -1;
                } else {
                    to_wait_no_morf_set(this, play);
                }

                restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));
            }
        }
    } else if (this->av2.actionVar2 >= 0) {
        player_anime_check_SE_set(this, SE_set_status);
    }

    if ((this->av1.actionVar1 == 0) && (this->focusActor != NULL)) {
        this->yaw = this->actor.shape.rot.y = anchor_eye_move(this, false);
    }
}

void move_re_dead_attack(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_init_standard_repeat(play, this, &gPlayerAnim_link_normal_re_dead_attack_wait);
    }

    if (escape_key_input_check(this, 0, 100)) {
        to_wait_set_check(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_7;
    }
}

void move_slope_slip(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_5 | PLAYER_STATE2_6;
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    slip_effect_set(play, this);
    Na_SetLinkSliplevelSe(&this->actor.projectedPos, NA_SE_PL_SLIP_LEVEL - SFX_FLAG, this->actor.speed);

    if (to_look_set_check(this, play) == 0) {
        CollisionPoly* floorPoly = this->actor.floorPoly;
        f32 xzSpeedTarget;
        f32 xzSpeedIncrStep;
        f32 xzSpeedDecrStep;
        s16 downwardSlopeYaw;
        s16 shapeYawTarget;
        Vec3f slopeNormal;

        if (floorPoly == NULL) {
            to_landing_set(this, play);
            return;
        }

        ground_status_check(floorPoly, &slopeNormal, &downwardSlopeYaw);

        shapeYawTarget = downwardSlopeYaw;
        if (this->av1.facingUpSlope) {
            shapeYawTarget = downwardSlopeYaw + 0x8000;
        }

        if (this->speedXZ < 0.0f) {
            downwardSlopeYaw += 0x8000;
        }

        xzSpeedTarget = (1.0f - slopeNormal.y) * 40.0f;
        xzSpeedTarget = CLAMP(xzSpeedTarget, 0.0f, 10.0f);
        xzSpeedIncrStep = SQ(xzSpeedTarget) * 0.015f;
        xzSpeedDecrStep = slopeNormal.y * 0.01f;

        if (T_BGCheck_getFriction_ai(&play->colCtx, floorPoly, this->actor.floorBgId) != FLOOR_EFFECT_1) {
            xzSpeedTarget = 0.0f;
            xzSpeedDecrStep = slopeNormal.y * 10.0f;
        }

        if (xzSpeedIncrStep < 1.0f) {
            xzSpeedIncrStep = 1.0f;
        }

        if (chase_f3(&this->speedXZ, xzSpeedTarget, xzSpeedIncrStep, xzSpeedDecrStep) &&
            (xzSpeedTarget == 0.0f)) {
            LinkAnimationHeader* slideAnimation;

            if (!this->av1.facingUpSlope) {
                slideAnimation = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_down_slope_slip_end, this->modelAnimType);
            } else {
                slideAnimation = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_up_slope_slip_end, this->modelAnimType);
            }
            to_wait_set_check_anim(this, slideAnimation, play);
        }

        add_calc_short_angle2(&this->yaw, downwardSlopeYaw, 10, 4000, 800);
        chase_angle(&this->actor.shape.rot.y, shapeYawTarget, 2000);
    }
}

/**
 * Waits to start processing a Cutscene Action.
 * First, the timer `csDelayTimer` much reach 0.
 * Then, there must be a CS action available to start processing.
 *
 * When starting the cutscene action, `draw` will be set to make
 * Player appear, if he was invisible.
 */
void move_tool_demo_wait(Player* this, PlayState* play) {
    if ((DECR(this->av2.csDelayTimer) == 0) && to_tool_demo_check(play, this)) {
        demo_play_display_init(play, this, NULL);
        Player_actor_set_process(play, this, move_demo_play, 0);
        move_demo_play(this, play);
    }
}

void move_from_okarina_warp(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_tool_demo_wait, 0);
    this->av2.csDelayTimer = 40;

    // Note: The warp song sparkles actor is responsible for starting the warp-in cutscene script
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_KANKYO, 0.0f, 0.0f, 0.0f, 0, 0, 0, DEMOKANKYO_WARP_IN);
}

void move_from_warp(Player* this, PlayState* play) {
    s32 padding;

    if ((this->av1.isLakeHyliaCs) && (play->csCtx.curFrame < 305)) {
        // Delay falling down until frame 306 of the Lake Hylia cutscene after completing Water Temple
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
    } else if (to_ground_y < 150.0f) {
        if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
            if (!this->av2.playedLandingSfx) {
                if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                    this->skelAnime.endFrame = this->skelAnime.animLength - 1.0f;
                    player_land_SE_set(this);
                    this->av2.playedLandingSfx = true;
                }
            } else {
                if ((play->sceneId == SCENE_KOKIRI_FOREST) && to_tool_demo_check(play, this)) {
                    return;
                }

                Ext_to_wait_set(this, play);
            }
        }

        add_calc(&this->actor.velocity.y, 2.0f, 0.3f, 8.0f, 0.5f);
    }

    if ((play->sceneId == SCENE_CHAMBER_OF_THE_SAGES) && to_tool_demo_check(play, this)) {
        return;
    }

    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.playerCue != NULL)) {
        f32 savedYPos = this->actor.world.pos.y;

        demo_play_position_init(play, this, play->csCtx.playerCue);
        this->actor.world.pos.y = savedYPos;
    }
}

void move_warp_fall(Player* this, PlayState* play) {
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if ((this->av2.actionVar2++ > 8) && (play->transitionTrigger == TRANS_TRIGGER_OFF)) {

        if (this->av1.actionVar1 != 0) {
            if (play->sceneId == SCENE_ICE_CAVERN) {
                Game_play_down_restart_top(play);
                play->nextEntranceIndex = ENTR_ICE_CAVERN_0;
            } else if (this->av1.actionVar1 < 0) {
                Game_play_down_restart_top(play);
            } else {
                Game_play_down_restart(play);
            }

            play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
            Na_StartSystemSe_F(NA_SE_OC_ABYSS);
        } else {
            play->transitionType = TRANS_TYPE_FADE_BLACK;
            z_common_data.nextTransitionType = TRANS_TYPE_FADE_BLACK;
            z_common_data.seqId = (u8)NA_BGM_DISABLED;
            z_common_data.natureAmbienceId = 0xFF;
        }

        play->transitionTrigger = TRANS_TRIGGER_START;
    }
}

/**
 * Automatically open a door (no need for the A button).
 * Note: If no door is in useable range, a softlock will occur.
 */
void move_door_start(Player* this, PlayState* play) {
    to_door_open_check(this, play);
}

void move_jump_start(Player* this, PlayState* play) {
    this->actor.gravity = -1.0f;

    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if (this->actor.velocity.y < 0.0f) {
        to_landing_set(this, play);
    } else if (this->actor.velocity.y < 6.0f) {
        chase_f(&this->speedXZ, 3.0f, 0.5f);
    }
}

void move_bow_game(Player* this, PlayState* play) {
    this->unk_6AD = 2;

    subject_camera_mode_check(play, this);
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    uperbody_action_check(this, play);

    this->upperLimbRot.y = look_eye_move(play, this, true, 0) - this->actor.shape.rot.y;
    this->unk_6AE_rotFlags |= UNK6AE_ROT_UPPER_Y;

    if (play->shootingGalleryStatus < 0) {
        play->shootingGalleryStatus++;
        if (play->shootingGalleryStatus == 0) {
            to_move_set(this, play);
        }
    }
}

void move_ice_down(Player* this, PlayState* play) {
    if (this->av1.actionVar1 >= 0) {
        if (this->av1.actionVar1 < 6) {
            this->av1.actionVar1++;
        }

        if (escape_key_input_check(this, 1, 100)) {
            this->av1.actionVar1 = -1;
            ice_piece_effect_set(play, &this->actor.world.pos, this->actor.scale.x);
            player_SE_set(this, NA_SE_PL_ICE_BROKEN);
        } else {
            this->stateFlags2 |= PLAYER_STATE2_14;
        }

        if ((play->gameplayFrames % 4) == 0) {
            Ext_to_damage_set(play, -1);
        }
    } else {
        if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
            to_wait_set_check(this, play);
            shield_no_damage_timer_check(this, -20);
        }
    }
}

void move_electric_shock(Player* this, PlayState* play) {
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    no_damage_timer_check(this);

    if (((this->av2.actionVar2 % 25) != 0) || damage_point_set(play, this, -1)) {
        if (DECR(this->av2.actionVar2) == 0) {
            to_wait_set_check(this, play);
        }
    }

    this->bodyShockTimer = 40;
    Actor_player_level_SE_set(&this->actor, NA_SE_VO_LI_TAKEN_AWAY - SFX_FLAG + this->ageProperties->unk_92);
}

#if DEBUG_FEATURES
/**
 * Updates the "Noclip" debug feature, which allows the player to fly around anywhere
 * in the world and clip through any collision.
 *
 * Noclip can be toggled on and off with two different button combos:
 * Hold L + R + A and press B
 * or
 * Hold L and press D-pad right
 *
 * To control Noclip mode:
 * - Move horizontally with the 4 D-pad directions
 * - Move up with B
 * - Move down with A
 * - Hold R to move faster
 *
 * With Noclip enabled, another button combination can be pressed to set all "temp clear" flags
 * in the current room. To do so hold L and press D-pad left.
 *
 * @return  true if Noclip is disabled, false if enabled
 */
s32 Player_actor_moveNoclip(Player* this, PlayState* play) {
    pad = &play->state.input[0];

    if ((CHECK_BTN_ALL(pad->cur.button, BTN_A | BTN_L | BTN_R) &&
         CHECK_BTN_ALL(pad->press.button, BTN_B)) ||
        (CHECK_BTN_ALL(pad->cur.button, BTN_L) && CHECK_BTN_ALL(pad->press.button, BTN_DRIGHT))) {

        player_debug_mode ^= 1;

        if (player_debug_mode) {
            changeCameraMode(Gama_play_get_camera(play, CAM_ID_MAIN), CAM_MODE_Z_AIM);
        }
    }

    if (player_debug_mode) {
        f32 speed;

        if (CHECK_BTN_ALL(pad->cur.button, BTN_R)) {
            speed = 100.0f;
        } else {
            speed = 20.0f;
        }

        Debug_Print_write(3, 2, "DEBUG MODE");

        if (!CHECK_BTN_ALL(pad->cur.button, BTN_L)) {
            if (CHECK_BTN_ALL(pad->cur.button, BTN_B)) {
                this->actor.world.pos.y += speed;
            } else if (CHECK_BTN_ALL(pad->cur.button, BTN_A)) {
                this->actor.world.pos.y -= speed;
            }

            if (CHECK_BTN_ANY(pad->cur.button, BTN_DUP | BTN_DDOWN | BTN_DLEFT | BTN_DRIGHT)) {
                s16 angle;
                s16 temp;

                angle = temp = getCameraAngleY(GET_ACTIVE_CAM(play));

                if (CHECK_BTN_ALL(pad->cur.button, BTN_DDOWN)) {
                    angle = temp + 0x8000;
                } else if (CHECK_BTN_ALL(pad->cur.button, BTN_DLEFT)) {
                    angle = temp + 0x4000;
                } else if (CHECK_BTN_ALL(pad->cur.button, BTN_DRIGHT)) {
                    angle = temp - 0x4000;
                }

                this->actor.world.pos.x += speed * sin_s(angle);
                this->actor.world.pos.z += speed * cos_s(angle);
            }
        }

        speedF_clear(this);

        this->actor.gravity = 0.0f;
        this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;

        if (CHECK_BTN_ALL(pad->cur.button, BTN_L) && CHECK_BTN_ALL(pad->press.button, BTN_DLEFT)) {
            Actor_Environment_no_enemy_On(play, play->roomCtx.curRoom.num);
        }

        xyz_t_move(&this->actor.home.pos, &this->actor.world.pos);

        return false;
    }

    return true;
}
#endif

void bow_gen_action(Player* this) {
    this->unk_858 += this->unk_85C;
    this->unk_85C -= this->unk_858 * 5.0f;
    this->unk_85C *= 0.3f;

    if (ABS(this->unk_85C) < 0.00001f) {
        this->unk_85C = 0.0f;
        if (ABS(this->unk_858) < 0.00001f) {
            this->unk_858 = 0.0f;
        }
    }
}

/**
 * Updates the Bunny Hood's floppy ears' rotation and velocity.
 */
void rabit_mask_action(Player* this) {
    Vec3s force;
    s16 angle;

    // Damping: decay by 1/8 the previous value each frame
    rabit_info.angVel.x -= rabit_info.angVel.x >> 3;
    rabit_info.angVel.y -= rabit_info.angVel.y >> 3;

    // Elastic restorative force
    rabit_info.angVel.x += -rabit_info.rot.x >> 2;
    rabit_info.angVel.y += -rabit_info.rot.y >> 2;

    // Forcing from motion relative to shape frame
    angle = this->actor.world.rot.y - this->actor.shape.rot.y;
    force.x = (s32)(this->actor.speed * -200.0f * cos_s(angle) * (rnd_fx(2.0f) + 10.0f)) & 0xFFFF;
    force.y = (s32)(this->actor.speed * 100.0f * sin_s(angle) * (rnd_fx(2.0f) + 10.0f)) & 0xFFFF;

    rabit_info.angVel.x += force.x >> 2;
    rabit_info.angVel.y += force.y >> 2;

    // Clamp both angular velocities to [-6000, 6000]
    if (rabit_info.angVel.x > 6000) {
        rabit_info.angVel.x = 6000;
    } else if (rabit_info.angVel.x < -6000) {
        rabit_info.angVel.x = -6000;
    }
    if (rabit_info.angVel.y > 6000) {
        rabit_info.angVel.y = 6000;
    } else if (rabit_info.angVel.y < -6000) {
        rabit_info.angVel.y = -6000;
    }

    // Add angular velocity to rotations
    rabit_info.rot.x += rabit_info.angVel.x;
    rabit_info.rot.y += rabit_info.angVel.y;

    // swivel ears outwards if bending backwards
    if (rabit_info.rot.x < 0) {
        rabit_info.rot.z = rabit_info.rot.x >> 1;
    } else {
        rabit_info.rot.z = 0;
    }
}

s32 to_kiru_check(Player* this, PlayState* play) {
    if (to_arms_attack_check(play, this) == 0) {
        if (kiru_check(this) != 0) {
            s32 sp24 = kiru_type_check(this);

            to_kiru_set(play, this, sp24);

            if (sp24 >= PLAYER_MWA_SPIN_ATTACK_1H) {
                this->stateFlags2 |= PLAYER_STATE2_17;
                power_kiru_effect_set(play, this, 0);
                return 1;
            }
        } else {
            return 0;
        }
    }

    return 1;
}

static Vec3f hammer_BGcheck_offset = { 0.0f, 40.0f, 45.0f };

void move_kiru(Player* this, PlayState* play) {
    struct_80854190* sp44 = &kiru_anime_status[this->meleeWeaponAnimation];

    this->stateFlags2 |= PLAYER_STATE2_5;

    if (!to_kiru_rebound_check(play, this)) {
        sword_attack_check(this, 0.0f, sp44->unk_0C, sp44->unk_0D);

        if ((this->stateFlags2 & PLAYER_STATE2_30) && (this->heldItemAction != PLAYER_IA_HAMMER) &&
            Skeleton_Info_Rom_frame_check(&this->skelAnime, 0.0f)) {
            this->speedXZ = 15.0f;
            this->stateFlags2 &= ~PLAYER_STATE2_30;
        }

        if (this->speedXZ > 12.0f) {
            slip_effect_set(play, this);
        }

        chase_f(&this->speedXZ, 0.0f, 5.0f);
        combo_key_check(this);

        if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
            if (!to_kiru_check(this, play)) {
                u8 sp43 = this->skelAnime.movementFlags;
                LinkAnimationHeader* sp3C;

                if (anchor_mode_check(this)) {
                    sp3C = sp44->unk_08;
                } else {
                    sp3C = sp44->unk_04;
                }

                sword_attack_reset(this);
                this->skelAnime.movementFlags = 0;

                if ((sp3C == &gPlayerAnim_link_fighter_Lpower_jump_kiru_end) &&
                    (this->modelAnimType != PLAYER_ANIMTYPE_3)) {
                    sp3C = &gPlayerAnim_link_fighter_power_jump_kiru_end;
                }

                to_wait_set_check_anim(this, sp3C, play);

                this->skelAnime.movementFlags = sp43;
                this->stateFlags3 |= PLAYER_STATE3_3;
            }
        } else if (this->heldItemAction == PLAYER_IA_HAMMER) {
            if ((this->meleeWeaponAnimation == PLAYER_MWA_HAMMER_FORWARD) ||
                (this->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_FINISH)) {
                static Vec3f blast_vec_acc = { 0.0f, 0.0f, 0.0f };
                Vec3f shockwavePos;
                f32 sp2C;

                shockwavePos.y = player_ground_BGcheck(play, this, &hammer_BGcheck_offset, &shockwavePos);
                sp2C = this->actor.world.pos.y - shockwavePos.y;

                chase_angle(&this->actor.focus.rot.x, atans_table(45.0f, sp2C), 800);
                eye_move_anime_set(this, true);

                if ((((this->meleeWeaponAnimation == PLAYER_MWA_HAMMER_FORWARD) &&
                      Skeleton_Info_Rom_frame_check(&this->skelAnime, 7.0f)) ||
                     ((this->meleeWeaponAnimation == PLAYER_MWA_JUMPSLASH_FINISH) &&
                      Skeleton_Info_Rom_frame_check(&this->skelAnime, 2.0f))) &&
                    (sp2C > -40.0f) && (sp2C < 40.0f)) {
                    hammer_impact_set(play, this);
                    Effect_SS_Blast_ct_direct(play, &shockwavePos, &blast_vec_acc, &blast_vec_acc);
                }
            }
        }
    }
}

void move_kiru_rebound(Player* this, PlayState* play) {
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    ground_breaking_speedF(this);

    if (this->skelAnime.curFrame >= 6.0f) {
        to_wait_proc_set(this, play);
    }
}

void move_twistar_magic_select(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_5;

    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    uperbody_action_check(this, play);

    if (this->av2.actionVar2 == 0) {
        message_set(play, 0x3B, &this->actor);
        this->av2.actionVar2 = 1;
        return;
    }

    if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
        s32 respawnData = z_common_data.respawn[RESPAWN_MODE_TOP].data;

        if (play->msgCtx.choiceIndex == 0) {
            z_common_data.respawnFlag = 3;
            play->transitionTrigger = TRANS_TRIGGER_START;
            play->nextEntranceIndex = z_common_data.respawn[RESPAWN_MODE_TOP].entranceIndex;
            play->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
            total_event_timer_end(play);
            return;
        }

        if (play->msgCtx.choiceIndex == 1) {
            z_common_data.respawn[RESPAWN_MODE_TOP].data = -respawnData;
            z_common_data.save.info.fw.set = 0;
            Na_StartObjectSe_F(&z_common_data.respawn[RESPAWN_MODE_TOP].pos, NA_SE_PL_MAGIC_WIND_VANISH);
        }

        Ext_to_wait_set(this, play);
        restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));
    }
}

void move_from_magic_window(Player* this, PlayState* play) {
    s32 respawnData = z_common_data.respawn[RESPAWN_MODE_TOP].data;

    if (this->av2.appearTimer > 20) {
        this->actor.draw = Player_actor_draw;
        this->actor.world.pos.y += 60.0f;
        to_landing_set(this, play);
        return;
    }

    if (this->av2.appearTimer++ == 20) {
        z_common_data.respawn[RESPAWN_MODE_TOP].data = respawnData + 1;
        Na_StartObjectSe_F(&z_common_data.respawn[RESPAWN_MODE_TOP].pos, NA_SE_PL_MAGIC_WIND_WARP);
    }
}

static LinkAnimationHeader* magic_shoot1_anm[] = {
    &gPlayerAnim_link_magic_kaze1,
    &gPlayerAnim_link_magic_honoo1,
    &gPlayerAnim_link_magic_tamashii1,
};

static LinkAnimationHeader* magic_shoot2_anm[] = {
    &gPlayerAnim_link_magic_kaze2,
    &gPlayerAnim_link_magic_honoo2,
    &gPlayerAnim_link_magic_tamashii2,
};

static LinkAnimationHeader* magic_shoot_end_anm[] = {
    &gPlayerAnim_link_magic_kaze3,
    &gPlayerAnim_link_magic_honoo3,
    &gPlayerAnim_link_magic_tamashii3,
};

static u8 magic_end_timer[] = { 70, 10, 10 };

void move_magic_wait(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (this->av1.actionVar1 < 0) {
            if ((this->itemAction == PLAYER_IA_NAYRUS_LOVE) || (z_common_data.magicState == MAGIC_STATE_IDLE)) {
                to_wait_proc_set(this, play);
                restartCameraStoped(Gama_play_get_camera(play, CAM_ID_MAIN));
            }
        } else {
            if (this->av2.actionVar2 == 0) {
                Skeleton_Info_Rom_init_standard_speedset_stop(play, &this->skelAnime, magic_shoot1_anm[this->av1.actionVar1], 0.83f);

                if (magic_effect_set(play, this, this->av1.actionVar1) != NULL) {
                    this->stateFlags1 |= PLAYER_STATE1_28 | PLAYER_STATE1_29;
                    if ((this->av1.actionVar1 != 0) || (z_common_data.respawn[RESPAWN_MODE_TOP].data <= 0)) {
                        z_common_data.magicState = MAGIC_STATE_CONSUME_SETUP;
                    }
                } else {
                    magic_cancel_check(play);
                }
            } else {
                Skeleton_Info_Rom_init_standard_speedset_repeat(play, &this->skelAnime, magic_shoot2_anm[this->av1.actionVar1], 0.83f);

                if (this->av1.actionVar1 == 0) {
                    this->av2.actionVar2 = -10;
                }
            }

            this->av2.actionVar2++;
        }
    } else {
        if (this->av2.actionVar2 < 0) {
            this->av2.actionVar2++;

            if (this->av2.actionVar2 == 0) {
                z_common_data.respawn[RESPAWN_MODE_TOP].data = 1;
                Game_play_restart_set(play, RESPAWN_MODE_TOP,
                                       PLAYER_PARAMS(PLAYER_START_MODE_FARORES_WIND, PLAYER_START_BG_CAM_DEFAULT));
                z_common_data.save.info.fw.set = 1;
                z_common_data.save.info.fw.pos.x = z_common_data.respawn[RESPAWN_MODE_DOWN].pos.x;
                z_common_data.save.info.fw.pos.y = z_common_data.respawn[RESPAWN_MODE_DOWN].pos.y;
                z_common_data.save.info.fw.pos.z = z_common_data.respawn[RESPAWN_MODE_DOWN].pos.z;
                z_common_data.save.info.fw.yaw = z_common_data.respawn[RESPAWN_MODE_DOWN].yaw;
                z_common_data.save.info.fw.playerParams =
                    PLAYER_PARAMS(PLAYER_START_MODE_FARORES_WIND, PLAYER_START_BG_CAM_DEFAULT);
                z_common_data.save.info.fw.entranceIndex = z_common_data.respawn[RESPAWN_MODE_DOWN].entranceIndex;
                z_common_data.save.info.fw.roomIndex = z_common_data.respawn[RESPAWN_MODE_DOWN].roomIndex;
                z_common_data.save.info.fw.tempSwchFlags = z_common_data.respawn[RESPAWN_MODE_DOWN].tempSwchFlags;
                z_common_data.save.info.fw.tempCollectFlags = z_common_data.respawn[RESPAWN_MODE_DOWN].tempCollectFlags;
                this->av2.actionVar2 = 2;
            }
        } else if (this->av1.actionVar1 >= 0) {
            if (this->av2.actionVar2 == 0) {
                static AnimSfxEntry SE_set_status[] = {
                    { NA_SE_PL_SKIP, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 20) },
                    { NA_SE_VO_LI_SWORD_N, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 20) },
                    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 26) },
                };

                player_anime_check_SE_set(this, SE_set_status);
            } else if (this->av2.actionVar2 == 1) {
                static AnimSfxEntry SE_set_status[][2] = {
                    {
                        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 20) },
                        { NA_SE_VO_LI_MAGIC_FROL, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 30) },
                    },
                    {
                        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 20) },
                        { NA_SE_VO_LI_MAGIC_NALE, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 44) },
                    },
                    {
                        { NA_SE_VO_LI_MAGIC_ATTACK, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 20) },
                        { NA_SE_IT_SWORD_SWING_HARD, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 20) },
                    },
                };

                player_anime_check_SE_set(this, SE_set_status[this->av1.actionVar1]);
                if ((this->av1.actionVar1 == 2) && Skeleton_Info_Rom_frame_check(&this->skelAnime, 30.0f)) {
                    this->stateFlags1 &= ~(PLAYER_STATE1_28 | PLAYER_STATE1_29);
                }
            } else if (magic_end_timer[this->av1.actionVar1] < this->av2.actionVar2++) {
                Skeleton_Info_Rom_init_standard_speedset_stop(play, &this->skelAnime, magic_shoot_end_anm[this->av1.actionVar1], 0.83f);
                this->yaw = this->actor.shape.rot.y;
                this->av1.actionVar1 = -1;
            }
        }
    }

    ground_breaking_speedF(this);
}

void move_hook_fly(Player* this, PlayState* play) {
    this->stateFlags2 |= PLAYER_STATE2_5;

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_init_standard_repeat(play, this, &gPlayerAnim_link_hook_fly_wait);
    }

    xyz_t_add(&this->actor.world.pos, &this->actor.velocity, &this->actor.world.pos);

    if (hook_shoot_end_check(this)) {
        f32 temp;

        xyz_t_move(&this->actor.prevPos, &this->actor.world.pos);
        BGcheck(play, this);

        temp = this->actor.world.pos.y - this->actor.floorHeight;
        temp = CLAMP_MAX(temp, 20.0f);

        this->actor.world.rot.x = this->actor.shape.rot.x = 0;
        this->actor.world.pos.y -= temp;
        this->speedXZ = 1.0f;
        this->actor.velocity.y = 0.0f;
        to_landing_set(this, play);
        this->stateFlags2 &= ~PLAYER_STATE2_10;
        this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;
        this->stateFlags1 |= PLAYER_STATE1_2;
    } else if ((this->skelAnime.animation != &gPlayerAnim_link_hook_fly_start) || (4.0f <= this->skelAnime.curFrame)) {
        this->actor.gravity = 0.0f;
        chase_angle(&this->actor.shape.rot.x, this->actor.world.rot.x, 0x800);
        player_rumble_entry(this, 100, 2, 100, 0);
    }
}

void move_fishing_throw(Player* this, PlayState* play) {
    if ((this->av2.actionVar2 != 0) && ((this->unk_858 != 0.0f) || (this->unk_85C != 0.0f))) {
        // 144-byte buffer, declared as a u64 array for 8-byte alignment. Skeleton_Info_Rom_morf_anime_to_morf will round up
        // the buffer address to the nearest 16-byte alignment before passing it to Skeleton_Proc_Get_init,
        // and Skeleton_Proc_Get_init requires space for `sizeof(Vec3s) * limbCount + 2` bytes. Link's
        // skeleton has 22 limbs (including the root limb) so we need 134 bytes of space, plus 8 bytes of margin for
        // the 16-byte alignment operation.
        static u64 work2_joint[18];
        f32 updateScale = R_UPDATE_RATE * 0.5f;

        this->skelAnime.curFrame += this->skelAnime.playSpeed * updateScale;
        if (this->skelAnime.curFrame >= this->skelAnime.animLength) {
            this->skelAnime.curFrame -= this->skelAnime.animLength;
        }

        Skeleton_Info_Rom_morf_anime_to_now(play, &this->skelAnime, &gPlayerAnim_link_fishing_wait, this->skelAnime.curFrame,
                                   (this->unk_858 < 0.0f) ? &gPlayerAnim_link_fishing_reel_left
                                                          : &gPlayerAnim_link_fishing_reel_right,
                                   5.0f, fabsf(this->unk_858), this->blendTable);
        Skeleton_Info_Rom_morf_anime_to_morf(play, &this->skelAnime, &gPlayerAnim_link_fishing_wait, this->skelAnime.curFrame,
                                   (this->unk_85C < 0.0f) ? &gPlayerAnim_link_fishing_reel_up
                                                          : &gPlayerAnim_link_fishing_reel_down,
                                   5.0f, fabsf(this->unk_85C), (Vec3s*)work2_joint);
        Skeleton_Info_Rom_anime_morf(play, &this->skelAnime, 0.5f);
    } else if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        this->unk_860 = 2;
        anime_init_standard_repeat(play, this, &gPlayerAnim_link_fishing_wait);
        this->av2.actionVar2 = 1;
    }

    ground_breaking_speedF(this);

    if (this->unk_860 == 0) {
        Ext_to_wait_set(this, play);
    } else if (this->unk_860 == 3) {
        Player_actor_set_process(play, this, move_fishing_catch, 0);
        anime_init_stop_basic(play, this, &gPlayerAnim_link_fishing_fish_catch);
    }
}

void move_fishing_catch(Player* this, PlayState* play) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime) && (this->unk_860 == 0)) {
        to_wait_set_check_anim(this, &gPlayerAnim_link_fishing_fish_catch_end, play);
    }
}

static void (*demo_play_proc[])(PlayState*, Player*, void*) = {
    NULL,          demo_play_non_init, demo_play_stop_anime_init, demo_play_stop_anime2_init, demo_play_repeat_anime2_init, demo_play_stop_anime_move_init, demo_play_stop_anime_move2_init,
    demo_play_repeat_anime_move_init, demo_play_repeat_anime_move3_init, demo_play_standard_stop_init, demo_play_standard_repeat_init, demo_play_anime_play, demo_play_anime_play_repeat, demo_play_anime_move_play_repeat,
    demo_play_0_stop_anime_init, demo_play_standard_stop2_init, demo_play_standard_repeat2_init, demo_play_anime_play_standard_repeat2, demo_play_anime_play_SE_set,
};

static AnimSfxEntry standup_SE_set_status[] = {
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 34) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 45) },
    { NA_SE_PL_CALM_HIT, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 51) },
    { NA_SE_PL_CALM_HIT, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 64) },
};

static AnimSfxEntry atozusari_SE_set_status[] = {
    { NA_SE_VO_LI_SURPRISE, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 3) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 15) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 24) },
    { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 30) },
    { NA_SE_VO_LI_FALL_L, -ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 31) },
};

static AnimSfxEntry syagamu_SE_set_status[] = {
    { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 10) },
};

static struct_80854B18 demo_play_init_status[PLAYER_CSACTION_MAX] = {
    { 0, NULL },                                         // PLAYER_CSACTION_NONE
    { -1, demo_play_wait_init },                               // PLAYER_CSACTION_1
    { 2, &gPlayerAnim_link_demo_goma_furimuki },         // PLAYER_CSACTION_2
    { 0, NULL },                                         // PLAYER_CSACTION_3
    { 0, NULL },                                         // PLAYER_CSACTION_4
    { 3, &gPlayerAnim_link_demo_bikkuri },               // PLAYER_CSACTION_5
    { 0, NULL },                                         // PLAYER_CSACTION_6
    { 0, NULL },                                         // PLAYER_CSACTION_7
    { -1, demo_play_wait_init },                               // PLAYER_CSACTION_8
    { 2, &gPlayerAnim_link_demo_furimuki },              // PLAYER_CSACTION_9
    { -1, demo_play_warp_out_init },                               // PLAYER_CSACTION_10
    { 3, &gPlayerAnim_link_demo_warp },                  // PLAYER_CSACTION_11
    { -1, demo_play_fighter_wait_init },                               // PLAYER_CSACTION_12
    { 7, &gPlayerAnim_clink_demo_get1 },                 // PLAYER_CSACTION_13
    { 5, &gPlayerAnim_clink_demo_get2 },                 // PLAYER_CSACTION_14
    { 5, &gPlayerAnim_clink_demo_get3 },                 // PLAYER_CSACTION_15
    { 5, &gPlayerAnim_clink_demo_standup },              // PLAYER_CSACTION_16
    { 7, &gPlayerAnim_clink_demo_standup_wait },         // PLAYER_CSACTION_17
    { -1, demo_play_m_sword_start_init },                               // PLAYER_CSACTION_18
    { 2, &gPlayerAnim_link_demo_baru_op1 },              // PLAYER_CSACTION_19
    { 2, &gPlayerAnim_link_demo_baru_op3 },              // PLAYER_CSACTION_20
    { 0, NULL },                                         // PLAYER_CSACTION_21
    { -1, demo_play_kenjya_start_init },                               // PLAYER_CSACTION_22
    { 3, &gPlayerAnim_link_demo_jibunmiru },             // PLAYER_CSACTION_23
    { 9, &gPlayerAnim_link_normal_back_downA },          // PLAYER_CSACTION_24
    { 2, &gPlayerAnim_link_normal_back_down_wake },      // PLAYER_CSACTION_25
    { -1, demo_play_okarina_init },                               // PLAYER_CSACTION_26
    { 2, &gPlayerAnim_link_normal_okarina_end },         // PLAYER_CSACTION_27
    { 3, &gPlayerAnim_link_demo_get_itemA },             // PLAYER_CSACTION_28
    { -1, demo_play_wait_init },                               // PLAYER_CSACTION_29
    { 2, &gPlayerAnim_link_normal_normal2fighter_free }, // PLAYER_CSACTION_30
    { 0, NULL },                                         // PLAYER_CSACTION_31
    { 0, NULL },                                         // PLAYER_CSACTION_32
    { 5, &gPlayerAnim_clink_demo_atozusari },            // PLAYER_CSACTION_33
    { -1, demo_play_swim_wait_init },                               // PLAYER_CSACTION_34
    { -1, demo_play_water_get_item_init },                               // PLAYER_CSACTION_35
    { 5, &gPlayerAnim_clink_demo_bashi },                // PLAYER_CSACTION_36
    { 16, &gPlayerAnim_link_normal_hang_up_down },       // PLAYER_CSACTION_37
    { -1, demo_play_op3_wait_init },                               // PLAYER_CSACTION_38
    { -1, demo_play_op3_negaeri_init },                               // PLAYER_CSACTION_39
    { 6, &gPlayerAnim_clink_op3_okiagari },              // PLAYER_CSACTION_40
    { 6, &gPlayerAnim_clink_op3_tatiagari },             // PLAYER_CSACTION_41
    { -1, demo_play_futtobi_init },                               // PLAYER_CSACTION_42
    { 5, &gPlayerAnim_clink_demo_miokuri },              // PLAYER_CSACTION_43
    { -1, demo_play_wait2_init },                               // PLAYER_CSACTION_44
    { -1, demo_play_clear_init },                               // PLAYER_CSACTION_45
    { -1, demo_play_display_init },                               // PLAYER_CSACTION_46
    { 5, &gPlayerAnim_clink_demo_nozoki },               // PLAYER_CSACTION_47
    { 5, &gPlayerAnim_clink_demo_koutai },               // PLAYER_CSACTION_48
    { -1, demo_play_wait_init },                               // PLAYER_CSACTION_49
    { 5, &gPlayerAnim_clink_demo_koutai_kennuki },       // PLAYER_CSACTION_50
    { 5, &gPlayerAnim_link_demo_kakeyori },              // PLAYER_CSACTION_51
    { 5, &gPlayerAnim_link_demo_kakeyori_mimawasi },     // PLAYER_CSACTION_52
    { 5, &gPlayerAnim_link_demo_kakeyori_miokuri },      // PLAYER_CSACTION_53
    { 3, &gPlayerAnim_link_demo_furimuki2 },             // PLAYER_CSACTION_54
    { 3, &gPlayerAnim_link_demo_kaoage },                // PLAYER_CSACTION_55
    { 4, &gPlayerAnim_link_demo_kaoage_wait },           // PLAYER_CSACTION_56
    { 3, &gPlayerAnim_clink_demo_mimawasi },             // PLAYER_CSACTION_57
    { 3, &gPlayerAnim_link_demo_nozokikomi },            // PLAYER_CSACTION_58
    { 6, &gPlayerAnim_kolink_odoroki_demo },             // PLAYER_CSACTION_59
    { 6, &gPlayerAnim_link_shagamu_demo },               // PLAYER_CSACTION_60
    { 14, &gPlayerAnim_link_okiru_demo },                // PLAYER_CSACTION_61
    { 3, &gPlayerAnim_link_okiru_demo },                 // PLAYER_CSACTION_62
    { 5, &gPlayerAnim_link_fighter_power_kiru_start },   // PLAYER_CSACTION_63
    { 16, &gPlayerAnim_demo_link_nwait },                // PLAYER_CSACTION_64
    { 15, &gPlayerAnim_demo_link_tewatashi },            // PLAYER_CSACTION_65
    { 15, &gPlayerAnim_demo_link_orosuu },               // PLAYER_CSACTION_66
    { 3, &gPlayerAnim_d_link_orooro },                   // PLAYER_CSACTION_67
    { 3, &gPlayerAnim_d_link_imanodare },                // PLAYER_CSACTION_68
    { 3, &gPlayerAnim_link_hatto_demo },                 // PLAYER_CSACTION_69
    { 6, &gPlayerAnim_o_get_mae },                       // PLAYER_CSACTION_70
    { 6, &gPlayerAnim_o_get_ato },                       // PLAYER_CSACTION_71
    { 6, &gPlayerAnim_om_get_mae },                      // PLAYER_CSACTION_72
    { 6, &gPlayerAnim_nw_modoru },                       // PLAYER_CSACTION_73
    { 3, &gPlayerAnim_link_demo_gurad },                 // PLAYER_CSACTION_74
    { 3, &gPlayerAnim_link_demo_look_hand },             // PLAYER_CSACTION_75
    { 4, &gPlayerAnim_link_demo_sita_wait },             // PLAYER_CSACTION_76
    { 3, &gPlayerAnim_link_demo_ue },                    // PLAYER_CSACTION_77
    { 3, &gPlayerAnim_Link_muku },                       // PLAYER_CSACTION_78
    { 3, &gPlayerAnim_Link_miageru },                    // PLAYER_CSACTION_79
    { 6, &gPlayerAnim_Link_ha },                         // PLAYER_CSACTION_80
    { 3, &gPlayerAnim_L_1kyoro },                        // PLAYER_CSACTION_81
    { 3, &gPlayerAnim_L_2kyoro },                        // PLAYER_CSACTION_82
    { 3, &gPlayerAnim_L_sagaru },                        // PLAYER_CSACTION_83
    { 3, &gPlayerAnim_L_bouzen },                        // PLAYER_CSACTION_84
    { 3, &gPlayerAnim_L_kamaeru },                       // PLAYER_CSACTION_85
    { 3, &gPlayerAnim_L_hajikareru },                    // PLAYER_CSACTION_86
    { 3, &gPlayerAnim_L_ken_miru },                      // PLAYER_CSACTION_87
    { 3, &gPlayerAnim_L_mukinaoru },                     // PLAYER_CSACTION_88
    { -1, demo_play_power_kiru_wait_init },                               // PLAYER_CSACTION_89
    { 3, &gPlayerAnim_link_wait_itemD1_20f },            // PLAYER_CSACTION_90
    { -1, demo_play_sword_check_init },                               // PLAYER_CSACTION_91
    { -1, demo_play_down_to_stand_init },                               // PLAYER_CSACTION_92
    { 3, &gPlayerAnim_link_normal_wait_typeB_20f },      // PLAYER_CSACTION_93
    { -1, demo_play_m_sword_end_init },                               // PLAYER_CSACTION_94
    { 3, &gPlayerAnim_link_demo_kousan },                // PLAYER_CSACTION_95
    { 3, &gPlayerAnim_link_demo_return_to_past },        // PLAYER_CSACTION_96
    { 3, &gPlayerAnim_link_last_hit_motion1 },           // PLAYER_CSACTION_97
    { 3, &gPlayerAnim_link_last_hit_motion2 },           // PLAYER_CSACTION_98
    { 3, &gPlayerAnim_link_demo_zeldamiru },             // PLAYER_CSACTION_99
    { 3, &gPlayerAnim_link_demo_kenmiru1 },              // PLAYER_CSACTION_100
    { 3, &gPlayerAnim_link_demo_kenmiru2 },              // PLAYER_CSACTION_101
    { 3, &gPlayerAnim_link_demo_kenmiru2_modori },       // PLAYER_CSACTION_102
};

static struct_80854B18 demo_play_move_status[PLAYER_CSACTION_MAX] = {
    { 0, NULL },                                          // PLAYER_CSACTION_NONE
    { -1, demo_play_anchor_stop },                                // PLAYER_CSACTION_1
    { -1, demo_play_goma_furimuki },                                // PLAYER_CSACTION_2
    { -1, demo_play_run_start },                                // PLAYER_CSACTION_3
    { -1, demo_play_run_end },                                // PLAYER_CSACTION_4
    { 11, NULL },                                         // PLAYER_CSACTION_5
    { -1, demo_play_tool_play },                                // PLAYER_CSACTION_6
    { -1, demo_play_end },                                // PLAYER_CSACTION_7
    { -1, demo_play_wait },                                // PLAYER_CSACTION_8
    { -1, demo_play_furimuki },                                // PLAYER_CSACTION_9
    { -1, demo_play_warp_out },                                // PLAYER_CSACTION_10
    { -1, demo_play_warp_up },                                // PLAYER_CSACTION_11
    { -1, demo_play_fighter_wait },                                // PLAYER_CSACTION_12
    { 11, NULL },                                         // PLAYER_CSACTION_13
    { 11, NULL },                                         // PLAYER_CSACTION_14
    { 11, NULL },                                         // PLAYER_CSACTION_15
    { 18, standup_SE_set_status },                                   // PLAYER_CSACTION_16
    { 11, NULL },                                         // PLAYER_CSACTION_17
    { -1, demo_play_m_sword_start },                                // PLAYER_CSACTION_18
    { 12, &gPlayerAnim_link_demo_baru_op2 },              // PLAYER_CSACTION_19
    { 11, NULL },                                         // PLAYER_CSACTION_20
    { 0, NULL },                                          // PLAYER_CSACTION_21
    { -1, demo_play_kenjya_start },                                // PLAYER_CSACTION_22
    { 11, NULL },                                         // PLAYER_CSACTION_23
    { -1, demo_play_big_down },                                // PLAYER_CSACTION_24
    { 11, NULL },                                         // PLAYER_CSACTION_25
    { 17, &gPlayerAnim_link_normal_okarina_swing },       // PLAYER_CSACTION_26
    { 11, NULL },                                         // PLAYER_CSACTION_27
    { 11, NULL },                                         // PLAYER_CSACTION_28
    { 11, NULL },                                         // PLAYER_CSACTION_29
    { -1, demo_play_sword_standby },                                // PLAYER_CSACTION_30
    { -1, demo_play_eye_close },                                // PLAYER_CSACTION_31
    { -1, demo_play_eye_open },                                // PLAYER_CSACTION_32
    { 18, atozusari_SE_set_status },                                   // PLAYER_CSACTION_33
    { -1, demo_play_swim_wait },                                // PLAYER_CSACTION_34
    { 11, NULL },                                         // PLAYER_CSACTION_35
    { 11, NULL },                                         // PLAYER_CSACTION_36
    { 11, NULL },                                         // PLAYER_CSACTION_37
    { 11, NULL },                                         // PLAYER_CSACTION_38
    { -1, demo_play_op3_negaeri },                                // PLAYER_CSACTION_39
    { -1, demo_play_op3_okiagari },                                // PLAYER_CSACTION_40
    { -1, demo_play_op3_tatiagari },                                // PLAYER_CSACTION_41
    { -1, demo_play_futtobi },                                // PLAYER_CSACTION_42
    { 13, &gPlayerAnim_clink_demo_miokuri_wait },         // PLAYER_CSACTION_43
    { -1, demo_play_wait2 },                                // PLAYER_CSACTION_44
    { 0, NULL },                                          // PLAYER_CSACTION_45
    { 0, NULL },                                          // PLAYER_CSACTION_46
    { 11, NULL },                                         // PLAYER_CSACTION_47
    { -1, demo_play_koutai },                                // PLAYER_CSACTION_48
    { -1, demo_play_wait },                                // PLAYER_CSACTION_49
    { -1, demo_play_koutai_kennuki },                                // PLAYER_CSACTION_50
    { 13, &gPlayerAnim_link_demo_kakeyori_wait },         // PLAYER_CSACTION_51
    { -1, demo_play_mimawasi },                                // PLAYER_CSACTION_52
    { 13, &gPlayerAnim_link_demo_kakeyori_miokuri_wait }, // PLAYER_CSACTION_53
    { -1, demo_play_furimuki2 },                                // PLAYER_CSACTION_54
    { 11, NULL },                                         // PLAYER_CSACTION_55
    { 11, NULL },                                         // PLAYER_CSACTION_56
    { 12, &gPlayerAnim_clink_demo_mimawasi_wait },        // PLAYER_CSACTION_57
#if OOT_VERSION < PAL_1_0
    { 12, &gPlayerAnim_link_demo_nozokikomi_wait }, // PLAYER_CSACTION_58
#else
    { -1, demo_play_nozokikomi }, // PLAYER_CSACTION_58
#endif
    { 11, NULL },                                  // PLAYER_CSACTION_59
    { 18, syagamu_SE_set_status },                            // PLAYER_CSACTION_60
    { 11, NULL },                                  // PLAYER_CSACTION_61
    { 11, NULL },                                  // PLAYER_CSACTION_62
    { 11, NULL },                                  // PLAYER_CSACTION_63
    { 11, NULL },                                  // PLAYER_CSACTION_64
    { -1, demo_play_tewatashi },                         // PLAYER_CSACTION_65
    { 17, &gPlayerAnim_demo_link_nwait },          // PLAYER_CSACTION_66
    { 12, &gPlayerAnim_d_link_orowait },           // PLAYER_CSACTION_67
    { 12, &gPlayerAnim_demo_link_nwait },          // PLAYER_CSACTION_68
    { 11, NULL },                                  // PLAYER_CSACTION_69
    { -1, demo_play_okarina_look },                         // PLAYER_CSACTION_70
    { 17, &gPlayerAnim_sude_nwait },               // PLAYER_CSACTION_71
    { -1, demo_play_okarina_look },                         // PLAYER_CSACTION_72
    { 17, &gPlayerAnim_sude_nwait },               // PLAYER_CSACTION_73
    { 12, &gPlayerAnim_link_demo_gurad_wait },     // PLAYER_CSACTION_74
    { 12, &gPlayerAnim_link_demo_look_hand_wait }, // PLAYER_CSACTION_75
    { 11, NULL },                                  // PLAYER_CSACTION_76
    { 12, &gPlayerAnim_link_demo_ue_wait },        // PLAYER_CSACTION_77
    { 12, &gPlayerAnim_Link_m_wait },              // PLAYER_CSACTION_78
    { 13, &gPlayerAnim_Link_ue_wait },             // PLAYER_CSACTION_79
    { 12, &gPlayerAnim_Link_otituku_w },           // PLAYER_CSACTION_80
    { 12, &gPlayerAnim_L_kw },                     // PLAYER_CSACTION_81
    { 11, NULL },                                  // PLAYER_CSACTION_82
    { 11, NULL },                                  // PLAYER_CSACTION_83
    { 11, NULL },                                  // PLAYER_CSACTION_84
    { 11, NULL },                                  // PLAYER_CSACTION_85
    { -1, demo_play_hajikareru },                         // PLAYER_CSACTION_86
    { 11, NULL },                                  // PLAYER_CSACTION_87
    { 12, &gPlayerAnim_L_kennasi_w },              // PLAYER_CSACTION_88
    { -1, demo_play_power_kiru_wait },                         // PLAYER_CSACTION_89
    { -1, demo_play_power_kiru_shoot },                         // PLAYER_CSACTION_90
    { -1, demo_play_sword_check },                         // PLAYER_CSACTION_91
    { -1, demo_play_down_to_stand },                         // PLAYER_CSACTION_92
    { 11, NULL },                                  // PLAYER_CSACTION_93
    { 11, NULL },                                  // PLAYER_CSACTION_94
    { 11, NULL },                                  // PLAYER_CSACTION_95
    { -1, demo_play_m_sword_catch },                         // PLAYER_CSACTION_96
    { -1, demo_play_last_hit },                         // PLAYER_CSACTION_97
    { -1, demo_play_last_hit },                         // PLAYER_CSACTION_98
    { 12, &gPlayerAnim_link_demo_zeldamiru_wait }, // PLAYER_CSACTION_99
    { 12, &gPlayerAnim_link_demo_kenmiru1_wait },  // PLAYER_CSACTION_100
    { 12, &gPlayerAnim_link_demo_kenmiru2_wait },  // PLAYER_CSACTION_101
    { 12, &gPlayerAnim_demo_link_nwait },          // PLAYER_CSACTION_102
};

void demo_stop_anime_init(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    anime_morf_reset(this);
    anime_init_stop_basic(play, this, anim);
    speedF_clear(this);
}

void demo_stop_anime_init2(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    anime_morf_reset(this);
    Skeleton_Info_Rom_init(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, Si2_anime_end_frame(anim),
                         ANIMMODE_ONCE, -8.0f);
    speedF_clear(this);
}

void demo_repeat_anime_init2(PlayState* play, Player* this, LinkAnimationHeader* anim) {
    anime_morf_reset(this);
    Skeleton_Info_Rom_init(play, &this->skelAnime, anim, PLAYER_ANIM_ADJUSTED_SPEED, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f);
    speedF_clear(this);
}

void demo_play_non_init(PlayState* play, Player* this, void* anim) {
    speedF_clear(this);
}

void demo_play_stop_anime_init(PlayState* play, Player* this, void* anim) {
    demo_stop_anime_init(play, this, anim);
}

void demo_play_0_stop_anime_init(PlayState* play, Player* this, void* anim) {
    anime_morf_reset(this);
    anime_init_0_stop_basic(play, this, anim);
    speedF_clear(this);
}

void demo_play_stop_anime2_init(PlayState* play, Player* this, void* anim) {
    demo_stop_anime_init2(play, this, anim);
}

void demo_play_repeat_anime2_init(PlayState* play, Player* this, void* anim) {
    demo_repeat_anime_init2(play, this, anim);
}

void demo_play_stop_anime_move_init(PlayState* play, Player* this, void* anim) {
    demo_stop_anime_move_init(play, this, anim);
}

void demo_play_stop_anime_move2_init(PlayState* play, Player* this, void* anim) {
    demo_stop_anime_move_init3(play, this, anim,
                               ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                   ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);
}

void demo_play_repeat_anime_move_init(PlayState* play, Player* this, void* anim) {
    demo_repeat_anime_move_init(play, this, anim);
}

void demo_play_repeat_anime_move3_init(PlayState* play, Player* this, void* anim) {
    demo_repeat_anime_move_init3(play, this, anim,
                               ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                   ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);
}

void demo_play_standard_stop_init(PlayState* play, Player* this, void* anim) {
    anime_init_standard_stop(play, this, anim);
}

void demo_play_standard_repeat_init(PlayState* play, Player* this, void* anim) {
    anime_init_standard_repeat(play, this, anim);
}

void demo_play_standard_stop2_init(PlayState* play, Player* this, void* anim) {
    anime_init_standard_stop_3f(play, this, anim);
}

void demo_play_standard_repeat2_init(PlayState* play, Player* this, void* anim) {
    anime_init_standard_repeat_3f(play, this, anim);
}

void demo_play_anime_play(PlayState* play, Player* this, void* anim) {
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
}

void demo_play_anime_play_repeat(PlayState* play, Player* this, void* anim) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        demo_repeat_anime_init2(play, this, anim);
        this->av2.actionVar2 = 1;
    }
}

void demo_play_anime_play_standard_repeat2(PlayState* play, Player* this, void* anim) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_move_reset(this);
        anime_init_standard_repeat_3f(play, this, anim);
    }
}

void demo_play_anime_move_play_repeat(PlayState* play, Player* this, void* anim) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        demo_repeat_anime_move_init(play, this, anim);
        this->av2.actionVar2 = 1;
    }
}

void demo_play_anime_play_SE_set(PlayState* play, Player* this, void* arg2) {
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    player_anime_check_SE_set(this, arg2);
}

void demo_play_anchor_set(Player* this) {
    if ((this->csActor == NULL) || (this->csActor->update == NULL)) {
        this->csActor = NULL;
    }

    this->focusActor = this->csActor;

    if (this->focusActor != NULL) {
        this->actor.shape.rot.y = anchor_eye_move(this, false);
    }
}

void demo_play_swim_wait_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->stateFlags1 |= PLAYER_STATE1_27;
    this->stateFlags2 |= PLAYER_STATE2_10;
    this->stateFlags1 &= ~(PLAYER_STATE1_18 | PLAYER_STATE1_19);

    anime_init_standard_repeat(play, this, &gPlayerAnim_link_swimer_swim);
}

void demo_play_swim_wait(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.gravity = 0.0f;

    if (this->av1.actionVar1 == 0) {
        if (to_swim_deep_check(play, this, NULL)) {
            this->av1.actionVar1 = 1;
        } else {
            swim_anime_play(play, this, NULL, fabsf(this->actor.velocity.y));
            chase_angle(&this->unk_6C2, -10000, 800);
            swim_control(this, &this->actor.velocity.y, 4.0f, this->yaw);
        }
        return;
    }

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        if (this->av1.actionVar1 == 1) {
            anime_init_0_repeat_basic(play, this, &gPlayerAnim_link_swimer_swim_wait);
        } else {
            anime_init_standard_repeat(play, this, &gPlayerAnim_link_swimer_swim_wait);
        }
    }

    water_pressure_set(this);
    swim_control(this, &this->speedXZ, 0.0f, this->actor.shape.rot.y);
}

void demo_play_anchor_stop(PlayState* play, Player* this, CsCmdActorCue* cue) {
    demo_play_anchor_set(this);

    if (swim_check(this)) {
        demo_play_swim_wait(play, this, NULL);
        return;
    }

    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if (hook_shoot_check(this) || (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
        uperbody_action_check(this, play);
        return;
    }

    if ((this->interactRangeActor != NULL) && (this->interactRangeActor->textId == 0xFFFF)) {
        to_carry_check(this, play);
    }
}

void demo_play_goma_furimuki(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
}

void demo_play_wait_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    LinkAnimationHeader* anim;

    if (swim_check(this)) {
        demo_play_swim_wait_init(play, this, NULL);
        return;
    }

    anim = GET_PLAYER_ANIM(PLAYER_ANIMGROUP_nwait, this->modelAnimType);

    if ((this->cueId == PLAYER_CUEID_6) || (this->cueId == PLAYER_CUEID_46)) {
        anime_init_standard_stop(play, this, anim);
    } else {
        anime_morf_reset(this);
        Skeleton_Info_Rom_init(play, &this->skelAnime, anim, (2.0f / 3.0f), 0.0f, Si2_anime_end_frame(anim),
                             ANIMMODE_LOOP, -4.0f);
    }

    speedF_clear(this);
}

void demo_play_wait(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (to_bow_game_check(play, this) == 0) {
        if ((this->csAction == PLAYER_CSACTION_49) && (play->csCtx.state == CS_STATE_IDLE)) {
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
            return;
        }

        if (swim_check(this) != 0) {
            demo_play_swim_wait(play, this, NULL);
            return;
        }

        Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

        if (hook_shoot_check(this) || (this->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
            uperbody_action_check(this, play);
        }
    }
}

void demo_play_furimuki(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static AnimSfxEntry SE_set_status[] = {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 42) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 48) },
    };

    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    player_anime_check_SE_set(this, SE_set_status);
}

void demo_play_warp_out_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->stateFlags1 &= ~PLAYER_STATE1_BOOMERANG_THROWN;

    this->yaw = this->actor.shape.rot.y = this->actor.world.rot.y =
        search_position_angleY(&this->actor.world.pos, &this->unk_450);

    if (this->speedXZ <= 0.0f) {
        this->speedXZ = 0.1f;
    } else if (this->speedXZ > 2.5f) {
        this->speedXZ = 2.5f;
    }
}

void demo_play_warp_out(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 sp1C = 2.5f;

    aim_demo_run_control(play, this, &sp1C, 10);

    if (play->sceneId == SCENE_JABU_JABU_BOSS) {
        if (this->av2.actionVar2 == 0) {
            if (message_check(&play->msgCtx) == TEXT_STATE_NONE) {
                return;
            }
        } else {
            if (message_check(&play->msgCtx) != TEXT_STATE_NONE) {
                return;
            }
        }
    }

    this->av2.actionVar2++;
    if (this->av2.actionVar2 > 20) {
        this->csAction = PLAYER_CSACTION_11;
    }
}

void demo_play_fighter_wait_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    to_anchor_wait_from_wait_set(this, play);
}

void demo_play_fighter_wait(PlayState* play, Player* this, CsCmdActorCue* cue) {
    demo_play_anchor_set(this);

    if (this->av2.actionVar2 != 0) {
        if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
            anime_init_standard_repeat(play, this, waitR_anime_select(this));
            this->av2.actionVar2 = 0;
        }

        anchor_LR_ratio_reset(this);
    } else {
        anchor_wait_anime_set(play, this);
    }
}

void demo_play_run_start(PlayState* play, Player* this, CsCmdActorCue* cue) {
    demo_run_control(play, this, cue, 0.0f, 0, 0);
}

void demo_play_run_end(PlayState* play, Player* this, CsCmdActorCue* cue) {
    demo_run_control(play, this, cue, 0.0f, 0, 1);
}

// unused
static LinkAnimationHeader* m_sword_start_anm[] = {
    &gPlayerAnim_link_demo_back_to_past,
    &gPlayerAnim_clink_demo_goto_future,
};

void demo_play_m_sword_start_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static Vec3f demo_start_pos = { -1.0f, 70.0f, 20.0f };

    xyz_t_move(&this->actor.world.pos, &demo_start_pos);
    this->actor.shape.rot.y = -0x8000;
    anime_init_standard_stop_3f(play, this, this->ageProperties->timeTravelStartAnim);
    anime_move_init(play, this,
                             PLAYER_ANIM_MOVEMENT_RESET_BY_AGE | ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y |
                                 ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                 ANIM_FLAG_OVERRIDE_MOVEMENT);
}

static struct_808551A4 m_sword_demo_SE_data[] = {
    { NA_SE_IT_SWORD_PUTAWAY_STN, 0 },
    { NA_SE_IT_SWORD_STICK_STN, NA_SE_VO_LI_SWORD_N },
};

void demo_play_m_sword_start(PlayState* play, Player* this, CsCmdActorCue* cue) {
#if OOT_VERSION >= PAL_1_0
    static AnimSfxEntry SE_set_status[] = {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 29) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 39) },
    };
#endif

    struct_808551A4* sp2C;
    Gfx** dLists;

    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if ((LINK_IS_ADULT && Skeleton_Info_Rom_frame_check(&this->skelAnime, 70.0f)) ||
        (!LINK_IS_ADULT && Skeleton_Info_Rom_frame_check(&this->skelAnime, 87.0f))) {
        sp2C = &m_sword_demo_SE_data[z_common_data.save.linkAge];
        this->interactRangeActor->parent = &this->actor;

        if (!LINK_IS_ADULT) {
            dLists = l_longsword_model_data;
        } else {
            dLists = l_LhandG_model_data;
        }
        this->leftHandDLists = dLists + z_common_data.save.linkAge;

        player_SE_set(this, sp2C->unk_00);
        if (!LINK_IS_ADULT) {
            player_voice_SE_set(this, sp2C->unk_02);
        }
    } else if (LINK_IS_ADULT) {
        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 66.0f)) {
            player_voice_SE_set(this, NA_SE_VO_LI_SWORD_L);
        }
    } else {
#if OOT_VERSION >= PAL_1_0
        player_anime_check_SE_set(this, SE_set_status);
#endif
    }
}

void demo_play_kenjya_start_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Skeleton_Info_Rom_init(play, &this->skelAnime, &gPlayerAnim_link_demo_warp, -(2.0f / 3.0f), 12.0f, 12.0f,
                         ANIMMODE_ONCE, 0.0f);
}

void demo_play_kenjya_start(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static AnimSfxEntry SE_set_status[] = {
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_LANDING, 30) },
    };

    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    this->av2.actionVar2++;

    if (this->av2.actionVar2 >= 180) {
        if (this->av2.actionVar2 == 180) {
            Skeleton_Info_Rom_init(play, &this->skelAnime, &gPlayerAnim_link_okarina_warp_goal, (2.0f / 3.0f), 10.0f,
                                 Si2_anime_end_frame(&gPlayerAnim_link_okarina_warp_goal), ANIMMODE_ONCE, -8.0f);
        }
        player_anime_check_SE_set(this, SE_set_status);
    }
}

void demo_play_big_down(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime) && (this->av2.actionVar2 == 0) &&
        (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        anime_init_standard_stop(play, this, &gPlayerAnim_link_normal_back_downB);
        this->av2.actionVar2 = 1;
    }

    if (this->av2.actionVar2 != 0) {
        ground_breaking_speedF(this);
    }
}

void demo_play_okarina_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    demo_stop_anime_init2(play, this, &gPlayerAnim_link_normal_okarina_start);
    okarina_now_item_check_set(this);
    shape_set(this, item_shape_type_set(this, this->itemAction));
}

void demo_play_sword_standby(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_IT_SWORD_PICKOUT, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 12) },
    };

    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 6.0f)) {
        to_sword_item_set(play, this, false);
    } else {
        player_anime_check_SE_set(this, SE_set_status);
    }
}

void demo_play_eye_close(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    chase_s(&this->actor.shape.face, 0, 1);
}

void demo_play_eye_open(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    chase_s(&this->actor.shape.face, 2, 1);
}

void demo_play_water_get_item_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    demo_stop_anime_move_init2(play, this, &gPlayerAnim_link_swimer_swim_get,
                                       ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_ADJUST_STARTING_POS |
                                           ANIM_FLAG_OVERRIDE_MOVEMENT);
}

void demo_play_op3_negaeri_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    demo_stop_anime_move_init3(play, this, &gPlayerAnim_clink_op3_negaeri,
                               ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                   ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);
    player_voice_SE_set(this, NA_SE_VO_LI_GROAN);
}

void demo_play_op3_negaeri(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        demo_repeat_anime_move_init3(play, this, &gPlayerAnim_clink_op3_wait2,
                                   ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                       ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);
    }
}

void demo_play_repeat_anime_SE_set(PlayState* play, Player* this, LinkAnimationHeader* anim, AnimSfxEntry* arg3) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_init_standard_repeat_3f(play, this, anim);
        this->av2.actionVar2 = 1;
    } else if (this->av2.actionVar2 == 0) {
        player_anime_check_SE_set(this, arg3);
    }
}

void demo_play_op3_wait_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.shape.shadowDraw = NULL;
    demo_play_repeat_anime_move3_init(play, this, &gPlayerAnim_clink_op3_wait1);
}

void demo_play_op3_okiagari(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_VO_LI_RELAX, ANIMSFX_DATA(ANIMSFX_TYPE_VOICE, 35) },
        { NA_SE_PL_SLIPDOWN, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 236) },
        { NA_SE_PL_SLIPDOWN, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 256) },
    };

    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        demo_repeat_anime_move_init3(play, this, &gPlayerAnim_clink_op3_wait3,
                                   ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT | ANIM_FLAG_ENABLE_MOVEMENT |
                                       ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);
        this->av2.actionVar2 = 1;
    } else if (this->av2.actionVar2 == 0) {
        player_anime_check_SE_set(this, SE_set_status);
        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 240.0f)) {
            this->actor.shape.shadowDraw = Actor_shadow_foot;
        }
    }
}

void demo_play_op3_tatiagari(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static AnimSfxEntry SE_set_status[] = {
#if OOT_VERSION < PAL_1_0
        { NA_SE_PL_LAND, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 67) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 84) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 90) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 96) },
#else
        { NA_SE_PL_LAND + SURFACE_SFX_OFFSET_WOOD, ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 67) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_UNKNOWN, 84) },
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_UNKNOWN, 90) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_UNKNOWN, 96) },
#endif
    };

    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    player_anime_check_SE_set(this, SE_set_status);
}

void demo_play_futtobi_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    demo_stop_anime_move_init2(play, this, &gPlayerAnim_clink_demo_futtobi,
                                       ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT |
                                           ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_ADJUST_STARTING_POS |
                                           ANIM_FLAG_OVERRIDE_MOVEMENT);
    player_voice_SE_set(this, NA_SE_VO_LI_FALL_L);
}

void demo_play_position_calc(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 startX = cue->startPos.x;
    f32 startY = cue->startPos.y;
    f32 startZ = cue->startPos.z;

    f32 distX = cue->endPos.x - startX;
    f32 distY = cue->endPos.y - startY;
    f32 distZ = cue->endPos.z - startZ;

    f32 sp4 = (f32)(play->csCtx.curFrame - cue->startFrame) / (f32)(cue->endFrame - cue->startFrame);

    this->actor.world.pos.x = distX * sp4 + startX;
    this->actor.world.pos.y = distY * sp4 + startY;
    this->actor.world.pos.z = distZ * sp4 + startZ;
}

void demo_play_futtobi(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_PL_BOUND, ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 20) },
        { NA_SE_PL_BOUND, -ANIMSFX_DATA(ANIMSFX_TYPE_FLOOR, 30) },
    };

    demo_play_position_calc(play, this, cue);
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
    player_anime_check_SE_set(this, SE_set_status);
}

void demo_play_warp_up(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (cue != NULL) {
        demo_play_position_calc(play, this, cue);
    }
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
}

void demo_play_wait2_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    anime_init_stop_basic(play, this, GET_PLAYER_ANIM(PLAYER_ANIMGROUP_nwait, this->modelAnimType));
    speedF_clear(this);
}

void demo_play_wait2(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);
}

void demo_play_clear_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    anime_move_init(play, this,
                             ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_ADJUST_STARTING_POS | ANIM_FLAG_OVERRIDE_MOVEMENT);
}

void demo_play_display_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.draw = Player_actor_draw;
}

void demo_play_koutai_kennuki(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        demo_repeat_anime_move_init(play, this, &gPlayerAnim_clink_demo_koutai_wait);
        this->av2.actionVar2 = 1;
    } else if (this->av2.actionVar2 == 0) {
        if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 10.0f)) {
            to_sword_item_set(play, this, true);
        }
    }
}

void demo_play_furimuki2(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static AnimSfxEntry SE_set_status[] = {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 10) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 24) },
    };

    demo_play_repeat_anime_SE_set(play, this, &gPlayerAnim_link_demo_furimuki2_wait, SE_set_status);
}

#if OOT_VERSION >= PAL_1_0
void demo_play_nozokikomi(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static AnimSfxEntry SE_set_status[] = {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 15) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_WALKING, 35) },
    };

    demo_play_repeat_anime_SE_set(play, this, &gPlayerAnim_link_demo_nozokikomi_wait, SE_set_status);
}
#endif

void demo_play_tewatashi(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        anime_init_standard_repeat_3f(play, this, &gPlayerAnim_demo_link_twait);
        this->av2.actionVar2 = 1;
    }

    if ((this->av2.actionVar2 != 0) && (play->csCtx.curFrame >= 900)) {
        this->rightHandType = PLAYER_MODELTYPE_LH_OPEN;
    } else {
        this->rightHandType = PLAYER_MODELTYPE_RH_FF;
    }
}

void demo_play_anime_move_play_repeat_SE_set(PlayState* play, Player* this, LinkAnimationHeader* anim, AnimSfxEntry* arg3) {
    demo_play_anime_move_play_repeat(play, this, anim);
    if (this->av2.actionVar2 == 0) {
        player_anime_check_SE_set(this, arg3);
    }
}

void demo_play_koutai(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static AnimSfxEntry SE_set_status[] = {
        { 0, ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 15) },
        { 0, -ANIMSFX_DATA(ANIMSFX_TYPE_RUNNING, 33) },
    };

    demo_play_anime_move_play_repeat_SE_set(play, this, &gPlayerAnim_clink_demo_koutai_wait, SE_set_status);
}

void demo_play_mimawasi(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static AnimSfxEntry SE_set_status[] = {
        { NA_SE_PL_KNOCK, -ANIMSFX_DATA(ANIMSFX_TYPE_GENERAL, 78) },
    };

    demo_play_anime_move_play_repeat_SE_set(play, this, &gPlayerAnim_link_demo_kakeyori_wait, SE_set_status);
}

void demo_play_power_kiru_wait_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    to_power_kiru_start_init(play, this);
}

void demo_play_power_kiru_wait(PlayState* play, Player* this, CsCmdActorCue* cue) {
    pad->press.button |= BTN_B;

    move_power_kiru_wait(this, play);
}

void demo_play_power_kiru_shoot(PlayState* play, Player* this, CsCmdActorCue* cue) {
    move_power_kiru_wait(this, play);
}

void demo_play_sword_check_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
}

void demo_play_sword_check(PlayState* play, Player* this, CsCmdActorCue* cue) {
}

void demo_play_down_to_stand_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->stateFlags3 |= PLAYER_STATE3_1;
    this->speedXZ = 2.0f;
    this->actor.velocity.y = -1.0f;

    anime_init_standard_stop(play, this, &gPlayerAnim_link_normal_back_downA);
    player_voice_SE_set(this, NA_SE_VO_LI_FALL_L);
}

static void (*down_to_stand_process[])(Player* this, PlayState* play) = {
    move_big_damage,
    move_big_damage_slip,
    move_big_damage_wake,
};

void demo_play_down_to_stand(PlayState* play, Player* this, CsCmdActorCue* cue) {
    down_to_stand_process[this->av2.actionVar2](this, play);
}

void demo_play_m_sword_end_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    to_sword_item_set(play, this, false);
    anime_init_standard_stop_3f(play, this, &gPlayerAnim_link_demo_return_to_past);
}

void demo_play_hajikareru(PlayState* play, Player* this, CsCmdActorCue* cue) {
    Skeleton_Info_Rom_anime_play(play, &this->skelAnime);

    if (Skeleton_Info_Rom_frame_check(&this->skelAnime, 10.0f)) {
        this->heldItemAction = this->itemAction = PLAYER_IA_NONE;
        this->heldItemId = ITEM_NONE;
        this->modelGroup = this->nextModelGroup = item_shape_type_set(this, PLAYER_IA_NONE);
        this->leftHandDLists = l_Lhand_model_data;
        SetEquip_Item(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_MASTER);
        z_common_data.save.info.equips.buttonItems[0] = ITEM_SWORD_MASTER;
        ClearEquip_Item(play, EQUIP_TYPE_SWORD);
    }
}

static LinkAnimationHeader* okarina_look_repeat_anm[] = {
    &gPlayerAnim_L_okarina_get,
    &gPlayerAnim_om_get,
};

static Vec3s okarina_kirakira_status[2][2] = {
    { { -200, 700, 100 }, { 800, 600, 800 } },
    { { -200, 500, 0 }, { 600, 400, 600 } },
};

void demo_play_okarina_look(PlayState* play, Player* this, CsCmdActorCue* cue) {
    static Vec3f kirakira_vec = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 kirakira_prim = { 255, 255, 255, 0 };
    static Color_RGBA8 kirakira_env = { 0, 128, 128, 0 };
    s32 linkAge = z_common_data.save.linkAge;
    Vec3f sparklePos;
    Vec3f sp34;
    Vec3s* ptr;

    demo_play_anime_move_play_repeat(play, this, okarina_look_repeat_anm[linkAge]);

    if (this->rightHandType != PLAYER_MODELTYPE_RH_FF) {
        this->rightHandType = PLAYER_MODELTYPE_RH_FF;
        return;
    }

    ptr = okarina_kirakira_status[z_common_data.save.linkAge];

    sp34.x = ptr[0].x + rnd_fx(ptr[1].x);
    sp34.y = ptr[0].y + rnd_fx(ptr[1].y);
    sp34.z = ptr[0].z + rnd_fx(ptr[1].z);

    Skin_Matrix_MulVector(&this->shieldMf, &sp34, &sparklePos);

    Effect_SS_KiraKira_sc_ct_ct(play, &sparklePos, &kirakira_vec, &kirakira_vec, &kirakira_prim, &kirakira_env, 600, -10);
}

void demo_play_m_sword_catch(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        demo_play_end(play, this, cue);
    } else if (this->av2.actionVar2 == 0) {
        item_get_setting(play, ITEM_SWORD_MASTER);
        to_sword_item_set(play, this, false);
    } else {
        m_sword_end_SE_set(this);
    }
}

void demo_play_last_hit(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (Skeleton_Info_Rom_anime_play(play, &this->skelAnime)) {
        sword_attack_check(this, 0.0f, 99.0f, this->skelAnime.endFrame - 8.0f);
    }

    if (this->heldItemAction != PLAYER_IA_SWORD_MASTER) {
        to_sword_item_set(play, this, true);
    }
}

void demo_play_end(PlayState* play, Player* this, CsCmdActorCue* cue) {
    if (swim_check(this)) {
        to_swim_wait_set(play, this);
        sub_camera_mode_reset(play, this);
    } else {
        to_move_set(this, play);
        if (!to_talk_check(this, play)) {
            to_carry_check(this, play);
        }
    }

    this->csAction = PLAYER_CSACTION_NONE;
    this->unk_6AD = 0;
}

void demo_play_position_init(PlayState* play, Player* this, CsCmdActorCue* cue) {
    this->actor.world.pos.x = cue->startPos.x;
    this->actor.world.pos.y = cue->startPos.y;

    if ((play->sceneId == SCENE_KOKIRI_FOREST) && !LINK_IS_ADULT) {
        this->actor.world.pos.y -= 1.0f;
    }

    this->actor.world.pos.z = cue->startPos.z;
    this->yaw = this->actor.shape.rot.y = cue->rot.y;
}

void demo_play_position_init_check(PlayState* play, Player* this, CsCmdActorCue* cue) {
    f32 dx = cue->startPos.x - (s32)this->actor.world.pos.x;
    f32 dy = cue->startPos.y - (s32)this->actor.world.pos.y;
    f32 dz = cue->startPos.z - (s32)this->actor.world.pos.z;
    f32 dist = sqrtf(SQ(dx) + SQ(dy) + SQ(dz));
    s16 yawDiff = (s16)cue->rot.y - this->actor.shape.rot.y;

    if ((this->speedXZ == 0.0f) && ((dist > 50.0f) || (ABS(yawDiff) > 0x4000))) {
        demo_play_position_init(play, this, cue);
    }

    this->skelAnime.movementFlags = 0;
    anime_morf_reset(this);
}

void call_demo_play_proc(PlayState* play, Player* this, CsCmdActorCue* cue, struct_80854B18* arg3) {
    if (arg3->type > 0) {
        demo_play_proc[arg3->type](play, this, arg3->ptr);
    } else if (arg3->type < 0) {
        arg3->func(play, this, cue);
    }

    if ((old_anime_move_flag & ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT) &&
        !(this->skelAnime.movementFlags & ANIM_FLAG_DISABLE_CHILD_ROOT_ADJUSTMENT)) {
        this->skelAnime.morphTable[0].y /= this->ageProperties->unk_08;
        old_anime_move_flag = 0;
    }
}

void demo_play_carry_cancel_check(PlayState* play, Player* this, s32 csAction) {
    if ((csAction != PLAYER_CSACTION_1) && (csAction != PLAYER_CSACTION_8) && (csAction != PLAYER_CSACTION_49) &&
        (csAction != PLAYER_CSACTION_7)) {
        l_hand_child_cancel(play, this);
    }
}

void demo_play_tool_play(PlayState* play, Player* this, CsCmdActorCue* cueUnused) {
    CsCmdActorCue* cue = play->csCtx.playerCue;
    s32 padding;

    if (play->csCtx.state == CS_STATE_STOP) {
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        this->cueId = PLAYER_CUEID_NONE;
        speedF_clear(this);
        return;
    }

    if (cue == NULL) {
        this->actor.flags &= ~ACTOR_FLAG_INSIDE_CULLING_VOLUME;
    } else {
        s32 csAction;

        if (this->cueId != cue->id) {
            csAction = tool_demo_mode_data[cue->id];

            if (csAction >= PLAYER_CSACTION_NONE) {
                if ((csAction == PLAYER_CSACTION_3) || (csAction == PLAYER_CSACTION_4)) {
                    demo_play_position_init_check(play, this, cue);
                } else {
                    demo_play_position_init(play, this, cue);
                }
            }

            old_anime_move_flag = this->skelAnime.movementFlags;

            anime_move_reset(this);
            PRINTF("TOOL MODE=%d\n", csAction);
            demo_play_carry_cancel_check(play, this, ABS(csAction));
            call_demo_play_proc(play, this, cue, &demo_play_init_status[ABS(csAction)]);

            this->av2.actionVar2 = 0;
            this->av1.actionVar1 = 0;
            this->cueId = cue->id;
        }

        csAction = tool_demo_mode_data[this->cueId];
        call_demo_play_proc(play, this, cue, &demo_play_move_status[ABS(csAction)]);
    }
}

void move_demo_play(Player* this, PlayState* play) {
    if (this->csAction != this->prevCsAction) {
        old_anime_move_flag = this->skelAnime.movementFlags;

        anime_move_reset(this);
        this->prevCsAction = this->csAction;
        PRINTF("DEMO MODE=%d\n", this->csAction);
        demo_play_carry_cancel_check(play, this, this->csAction);
        call_demo_play_proc(play, this, NULL, &demo_play_init_status[this->csAction]);
    }

    call_demo_play_proc(play, this, NULL, &demo_play_move_status[this->csAction]);
}

int Ext_player_fish_out_check(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return (move_bottle_fish_out == this->actionFunc) && (this->itemAction == PLAYER_IA_BOTTLE_FISH);
}

s32 Ext_to_fishing_set(PlayState* play) {
    Player* this = GET_PLAYER(play);

    action_reset_all(play, this);
    player_item_on(play, this, ITEM_FISHING_POLE);
    return 1;
}

s32 Ext_to_re_dead_attack_set(PlayState* play, Player* this) {
    if (!player_action_check(play, this) && (this->invincibilityTimer >= 0) && !hook_shoot_check(this) &&
        !(this->stateFlags3 & PLAYER_STATE3_FLYING_WITH_HOOKSHOT)) {
        action_reset_all(play, this);
        Player_actor_set_process(play, this, move_re_dead_attack, 0);
        anime_init_standard_stop(play, this, &gPlayerAnim_link_normal_re_dead_attack);
        this->stateFlags2 |= PLAYER_STATE2_7;
        speedF_look_mode_clear(this);
        player_voice_SE_set(this, NA_SE_VO_LI_HELD);
        return true;
    }

    return false;
}

/**
 * Tries to starts a cutscene action specified by `csAction`.
 * A cutscene action will only start if player is not already in another form of cutscene.
 *
 * No actors will be halted over the duration of the cutscene action.
 *
 * @return  true if successful starting a `csAction`, false if not
 */
s32 Ext_player_demo_set(PlayState* play, Actor* actor, s32 csAction) {
    Player* this = GET_PLAYER(play);

    if (!player_action_check(play, this)) {
        action_reset_all(play, this);
        Player_actor_set_process(play, this, move_demo_play, 0);
        this->csAction = csAction;
        this->csActor = actor;
        speedF_look_mode_clear(this);
        return true;
    }

    return false;
}

void Ext_to_wait_set(Player* this, PlayState* play) {
    Player_actor_set_process(play, this, move_wait, 1);
    anime_init_stop_basic(play, this, wait_anime_select(this));
    this->yaw = this->actor.shape.rot.y;
}

s32 Ext_to_damage_set(PlayState* play, s32 damage) {
    Player* this = GET_PLAYER(play);

    if (!player_action_check(play, this) && !damage_point_set(play, this, damage)) {
        this->stateFlags2 &= ~PLAYER_STATE2_7;
        return 1;
    }

    return 0;
}

/**
 * Start talking to the specified actor.
 *
 * This function does not concern trading exchange items.
 * For item exchanges see relevant code in `Player_ActionChange_13` and `move_put_item`.
 */
void Ext_player_to_talk_set(PlayState* play, Actor* actor) {
    Player* this = GET_PLAYER(play);
    s32 padding;

    if ((this->talkActor != NULL) || (actor == this->naviActor) ||
        CHECK_FLAG_ALL(actor->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_TALK_WITH_C_UP)) {
        actor->flags |= ACTOR_FLAG_TALK;
    }

    this->talkActor = actor;
    this->exchangeItemId = EXCH_ITEM_NONE;

    if (actor->textId == 0xFFFF) {
        // Player will stand and look at the actor with no text appearing.
        // This can be used to delay text from appearing, for example.
        player_demo_mode_set(play, actor, PLAYER_CSACTION_1);
        actor->flags |= ACTOR_FLAG_TALK;
        to_no_item_set(play, this);
    } else {
        if (this->actor.flags & ACTOR_FLAG_TALK) {
            this->actor.textId = 0;
        } else {
            this->actor.flags |= ACTOR_FLAG_TALK;
            this->actor.textId = actor->textId;
        }

        if (this->stateFlags1 & PLAYER_STATE1_23) {
            s32 sp24 = this->av2.actionVar2;

            to_no_item_set(play, this);
            demo_init_talk(play, this);

            this->av2.actionVar2 = sp24;
        } else {
            if (swim_check(this)) {
                Player_actor_set_demo_init_proc(play, this, demo_init_talk);
                anime_init_0_repeat_basic(play, this, &gPlayerAnim_link_swimer_swim_wait);
            } else if ((actor->category != ACTORCAT_NPC) || (this->heldItemAction == PLAYER_IA_FISHING_POLE)) {
                demo_init_talk(play, this);

                if (!anchor_mode_check(this)) {
                    if ((actor != this->naviActor) && (actor->xzDistToPlayer < 40.0f)) {
                        anime_init_standard_stop_3f(play, this, &gPlayerAnim_link_normal_backspace);
                    } else {
                        anime_init_standard_repeat(play, this, wait_anime_select(this));
                    }
                }
            } else {
                Player_actor_set_demo_init_proc(play, this, demo_init_talk);
                anime_init_standard_stop_3f(play, this,
                                            (actor->xzDistToPlayer < 40.0f) ? &gPlayerAnim_link_normal_backspace
                                                                            : &gPlayerAnim_link_normal_talk_free);
            }

            if (this->skelAnime.animation == &gPlayerAnim_link_normal_backspace) {
                anime_move_init(
                    play, this, ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_ENABLE_MOVEMENT | ANIM_FLAG_ADJUST_STARTING_POS);
            }

            speedF_look_mode_clear(this);
        }

        this->stateFlags1 |= PLAYER_STATE1_TALKING | PLAYER_STATE1_29;
    }

    if ((this->naviActor == this->talkActor) && ((this->talkActor->textId & 0xFF00) != 0x200)) {
        this->naviActor->flags |= ACTOR_FLAG_TALK;
        change_item_camera_check_set(play, 0xB);
    }
}
