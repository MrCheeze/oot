/*
 * File: z_boss_goma.c
 * Overlay: ovl_Boss_Goma
 * Description: Gohma
 */

#include "z_boss_goma.h"
#include "assets/objects/object_goma/object_goma.h"
#include "overlays/actors/ovl_En_Goma/z_en_goma.h"
#include "overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

// IRIS_FOLLOW: gohma looks towards the player (iris rotation)
// BONUS_IFRAMES: gain invincibility frames when the player does something (throwing things?), or
// randomly (see Boss_Goma_Eye_Control)
typedef enum GohmaEyeState {
    EYESTATE_IRIS_FOLLOW_BONUS_IFRAMES, // default, allows not drawing lens and iris when eye is closed
    EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES,
    EYESTATE_IRIS_FOLLOW_NO_IFRAMES
} GohmaEyeState;

typedef enum GohmaVisualState {
    VISUALSTATE_RED,         // main/eye: red
    VISUALSTATE_DEFAULT,     // main: greenish cyan, blinks with dark gray every 16 frames; eye: white
    VISUALSTATE_DEFEATED,    // main/eye: dark gray
    VISUALSTATE_STUNNED = 4, // main: greenish cyan, alternates with blue; eye: greenish cyan
    VISUALSTATE_HIT          // main: greenish cyan, alternates with red; eye: greenish cyan
} GohmaVisualState;

void Boss_Goma_actor_ct(Actor* thisx, PlayState* play);
void Boss_Goma_actor_dt(Actor* thisx, PlayState* play);
void Boss_Goma_actor_move(Actor* thisx, PlayState* play);
void Boss_Goma_actor_draw(Actor* thisx, PlayState* play);

static void mode_start_demo_init(BossGoma* this, PlayState* play);
static void mode_start_demo(BossGoma* this, PlayState* play);
static void mode_down(BossGoma* this, PlayState* play);
static void mode_pre_atack(BossGoma* this, PlayState* play);
void mode_pre_atack2(BossGoma* this, PlayState* play);
static void mode_atack(BossGoma* this, PlayState* play);
static void mode_damage(BossGoma* this, PlayState* play);
void mode_chakuchi(BossGoma* this, PlayState* play);
void mode_chakuchi02(BossGoma* this, PlayState* play);
void mode_stan(BossGoma* this, PlayState* play);
void mode_fall02(BossGoma* this, PlayState* play);
void mode_fall(BossGoma* this, PlayState* play);
void mode_umu(BossGoma* this, PlayState* play);
void mode_standby_umu(BossGoma* this, PlayState* play);
static void mode_wait(BossGoma* this, PlayState* play);
void mode_U_wait(BossGoma* this, PlayState* play);
static void mode_walk(BossGoma* this, PlayState* play);
static void mode_up(BossGoma* this, PlayState* play);
void mode_U_walk(BossGoma* this, PlayState* play);
void set_kogoma(BossGoma* this, PlayState* play, s16 i);

ActorProfile Boss_Goma_Profile = {
    /**/ ACTOR_BOSS_GOMA,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_GOMA,
    /**/ sizeof(BossGoma),
    /**/ Boss_Goma_actor_ct,
    /**/ Boss_Goma_actor_dt,
    /**/ Boss_Goma_actor_move,
    /**/ Boss_Goma_actor_draw,
};

static ColliderJntSphElementInit GoAcOcInfoJntSphElemData[13] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_EYE, { { 0, 0, 1200 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_TAIL4, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_TAIL3, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_TAIL2, { { 0, 0, 0 }, 12 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_TAIL1, { { 0, 0, 0 }, 25 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_R_FEET, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_R_SHIN, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_R_THIGH_SHELL, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_L_ANTENNA_CLAW, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_R_ANTENNA_CLAW, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_L_FEET, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_L_SHIN, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { BOSSGOMA_LIMB_L_THIGH_SHELL, { { 0, 0, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit GoAcOcInfoJntSphData = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    13,
    GoAcOcInfoJntSphElemData,
};

static u8 mesh_data[16 * 16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
    0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00
};

static u8 mesh_data_cl[16 * 16] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

// indexed by limb (where the root limb is 1)
static u8 goma_buhin_timer[] = {
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    30, // tail end/last part
    40, // tail 2nd to last part
    0,  0, 0, 0, 0, 0, 0, 0,
    10, // back of right claw/hand
    15, // front of right claw/hand
    21, // part of right arm (inner)
    0,  0,
    25, // part of right arm (shell)
    0,  0,
    31, // part of right arm (shell on shoulder)
    35, // part of right arm (shoulder)
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    43, // end of left antenna
    48, // middle of left antenna
    53, // start of left antenna
    0,  0, 0, 0,
    42, // end of right antenna
    45, // middle of right antenna
    53, // start of right antenna
    0,  0, 0, 0, 0, 0,
    11, // back of left claw/hand
    15, // front of left claw/hand
    21, // part of left arm (inner)
    0,  0,
    25, // part of left arm (shell)
    0,  0,
    30, // part of left arm (shell on shoulder)
    35, // part of left arm (shoulder)
    0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/**
 * Sets the `i`th pixel of a 16x16 RGBA16 image to 0 (transparent black)
 * according to the `clearPixelTable`
 */
void texcel_mesh_1616(s16* rgba16image, u8* clearPixelTable, s16 i) {
    if (clearPixelTable[i]) {
        rgba16image[i] = 0;
    }
}

/**
 * Sets the `i`th 2x2 pixels block of a 32x32 RGBA16 image to 0 (transparent black)
 * according to the `clearPixelTable`
 */
void texcel_mesh_3232(s16* rgba16image, u8* clearPixelTable, s16 i) {
    s16* targetPixel;

    if (clearPixelTable[i]) {
        // address of the top left pixel in a 2x2 pixels block located at
        // (i & 0xF, i >> 4) in a 16x16 grid of 2x2 pixels
        targetPixel = rgba16image;
        targetPixel += (s16)((i & 0xF) * 2 + (i & 0xF0) * 4);

        // set the 2x2 block of pixels to 0
        targetPixel[0] = 0;
        targetPixel[1] = 0;
        targetPixel[32 + 0] = 0;
        targetPixel[32 + 1] = 0;
    }
}

/**
 * Clear pixels from Gohma's textures
 */
void tex_mesh(u8* clearPixelTable, s16 i) {
    texcel_mesh_1616(SEGMENTED_TO_VIRTUAL(gGohmaBodyTex), clearPixelTable, i);
    texcel_mesh_1616(SEGMENTED_TO_VIRTUAL(gGohmaShellUndersideTex), clearPixelTable, i);
    texcel_mesh_1616(SEGMENTED_TO_VIRTUAL(gGohmaDarkShellTex), clearPixelTable, i);
    texcel_mesh_1616(SEGMENTED_TO_VIRTUAL(gGohmaEyeTex), clearPixelTable, i);

    texcel_mesh_3232(SEGMENTED_TO_VIRTUAL(gGohmaShellTex), clearPixelTable, i);
    texcel_mesh_3232(SEGMENTED_TO_VIRTUAL(gGohmaIrisTex), clearPixelTable, i);
}

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_GOHMA, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
};

void Boss_Goma_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BossGoma* this = (BossGoma*)thisx;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 4000.0f, Actor_shadow_circle, 150.0f);
    Skeleton_Info2_M_ct(play, &this->skelanime, &gGohmaSkel, &gGohmaIdleCrouchedAnim, NULL, NULL, 0);
    Skeleton_Info2_init_standard_repeat(&this->skelanime, &gGohmaIdleCrouchedAnim);
    this->actor.shape.rot.x = -0x8000; // upside-down
    this->eyeIrisScaleX = 1.0f;
    this->eyeIrisScaleY = 1.0f;
    this->unusedInitX = this->actor.world.pos.x;
    this->unusedInitZ = this->actor.world.pos.z;
    this->actor.world.pos.y = -300.0f; // ceiling
    this->actor.gravity = 0.0f;
    mode_start_demo_init(this, play);
    this->actor.colChkInfo.health = 10;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &GoAcOcInfoJntSphData, this->colliderItems);

    if (Actor_Environment_room_clear_Check(play, play->roomCtx.curRoom.num)) {
        Actor_delete(&this->actor);
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, 0.0f, -640.0f, 0.0f, 0, 0, 0,
                           WARP_DUNGEON_CHILD);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, 141.0f, -640.0f, -84.0f, 0, 0, 0, 0);
    }
}

void goma_chakuchi_dust_se_set(BossGoma* this, PlayState* play, s16 arg2, s16 amountMinus1) {
    if (arg2 == 0 || arg2 == 1 || arg2 == 3) {
        _dust_ground_set(play, &this->actor, &this->rightHandBackLimbWorldPos, 25.0f, amountMinus1, 8.0f, 500,
                                 10, true);
    }

    if (arg2 == 0 || arg2 == 2 || arg2 == 3) {
        _dust_ground_set(play, &this->actor, &this->leftHandBackLimbWorldPos, 25.0f, amountMinus1, 8.0f, 500,
                                 10, true);
    }

    if (arg2 == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_DOWN);
    } else {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_WALK);
    }
}

void Boss_Goma_actor_dt(Actor* thisx, PlayState* play) {
    BossGoma* this = (BossGoma*)thisx;

    Skeleton_Info_dt(&this->skelanime, play);
    ClObjJntSph_dt_nzf(play, &this->collider);
}

/**
 * When Gohma is hit and its health drops to 0
 */
static void mode_down_init(BossGoma* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaDeathAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaDeathAnim),
                     ANIMMODE_ONCE, -2.0f);
    this->actionFunc = mode_down;
    this->disableGameplayLogic = true;
    this->decayingProgress = 0;
    this->noBackfaceCulling = false;
    this->framesUntilNextAction = 1200;
    this->actionState = 0;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    this->actor.speed = 0.0f;
    this->actor.shape.shadowScale = 0.0f;
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_DEAD);
}

/**
 * Initial action setup, with Gohma waiting on the ceiling for the fight to start.
 */
static void mode_start_demo_init(BossGoma* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(&gGohmaWalkAnim);

    Skeleton_Info2_init(&this->skelanime, &gGohmaWalkAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, -15.0f);
    this->actionFunc = mode_start_demo;
    this->actionState = 0;
    this->disableGameplayLogic = true;
    play->envCtx.lightSettingOverride = 4;
    play->envCtx.lightBlendRateOverride = 255;
}

/**
 * On the floor and not doing anything for 20-30 frames, before going back to mode_walk
 */
static void mode_wait_init(BossGoma* this) {
    f32 lastFrame = Si2_anime_end_frame(&gGohmaIdleCrouchedAnim);

    this->framesUntilNextAction = get_random_timer(20, 30);
    Skeleton_Info2_init(&this->skelanime, &gGohmaIdleCrouchedAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP, -5.0f);
    this->actionFunc = mode_wait;
}

/**
 * On the ceiling and not doing anything for 20-30 frames, leads to spawning children gohmas
 */
void mode_U_wait_init(BossGoma* this) {
    this->framesUntilNextAction = get_random_timer(20, 30);
    Skeleton_Info2_init(&this->skelanime, &gGohmaHangAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaHangAnim),
                     ANIMMODE_LOOP, -5.0f);
    this->actionFunc = mode_U_wait;
}

/**
 * When the player killed all children gohmas
 */
void mode_fall02_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaLandAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -5.0f);
    this->actionFunc = mode_fall02;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -2.0f;
}

/**
 * When the player successfully hits Gohma on the ceiling
 */
void mode_fall_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaCrashAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -5.0f);
    this->actionFunc = mode_fall;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -2.0f;
}

void mode_umu_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaLayEggsAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaLayEggsAnim),
                     ANIMMODE_LOOP, -15.0f);
    this->actionFunc = mode_umu;
    this->spawnGohmasActionTimer = 0;
}

void mode_standby_umu_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaPrepareEggsAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gGohmaPrepareEggsAnim), ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_standby_umu;
    this->framesUntilNextAction = 70;
}

static void mode_up_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaClimbAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaClimbAnim),
                     ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_up;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;
}

/**
 * Gohma either reached the ceiling after climbing a wall, or is waiting for the player to kill the (children) Gohmas.
 */
void mode_U_walk_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaWalkAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaWalkAnim),
                     ANIMMODE_LOOP, -5.0f);
    this->actionFunc = mode_U_walk;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;
    this->framesUntilNextAction = get_random_timer(30, 60);
}

/**
 * Root action when on the floor, leads to attacking or climbing.
 */
static void mode_walk_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaWalkCrouchedAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gGohmaWalkCrouchedAnim), ANIMMODE_LOOP, -5.0f);
    this->actionFunc = mode_walk;
    this->framesUntilNextAction = get_random_timer(70, 110);
}

/**
 * Gohma jumped to the floor on its own, after the player has killed its children Gohmas.
 */
void mode_chakuchi02_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaLandAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaLandAnim),
                     ANIMMODE_ONCE, -2.0f);
    this->actionFunc = mode_chakuchi02;
    this->currentAnimFrameCount = Si2_anime_end_frame(&gGohmaLandAnim);
}

/**
 * Gohma was shot by the player down from the ceiling.
 */
void mode_chakuchi_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaCrashAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaCrashAnim),
                     ANIMMODE_ONCE, -2.0f);
    this->currentAnimFrameCount = Si2_anime_end_frame(&gGohmaCrashAnim);
    this->actionFunc = mode_chakuchi;
    this->currentAnimFrameCount = Si2_anime_end_frame(&gGohmaCrashAnim);
}

/**
 * Gohma is vulnerable, from being struck down from the ceiling or on the ground.
 */
void mode_stan_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaStunnedAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaStunnedAnim),
                     ANIMMODE_LOOP, -2.0f);
    this->actionFunc = mode_stan;
}

/**
 * Take an attack posture, when the player is close enough.
 */
static void mode_pre_atack_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaPrepareAttackAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gGohmaPrepareAttackAnim), ANIMMODE_ONCE, -10.0f);
    this->actionFunc = mode_pre_atack;
}

/**
 * Leads to mode_atack after 1 frame
 */
void mode_pre_atack2_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaStandAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaStandAnim),
                     ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_pre_atack2;
    this->framesUntilNextAction = 0;
}

static void mode_atack_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaAttackAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaAttackAnim),
                     ANIMMODE_ONCE, -10.0f);
    this->actionFunc = mode_atack;
    this->actionState = 0;
    this->framesUntilNextAction = 0;
}

/**
 * Plays an animation for Gohma being hit (while stunned)
 * The setup and the action preserve timers apart from the patience one, notably `framesUntilNextAction` which is used
 * as the stun duration
 */
static void mode_damage_init(BossGoma* this) {
    Skeleton_Info2_init(&this->skelanime, &gGohmaDamageAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaDamageAnim),
                     ANIMMODE_ONCE, -2.0f);
    this->actionFunc = mode_damage;
}

void goma_roof_walk(BossGoma* this, PlayState* play, f32 dz, f32 targetSpeedXZ,
                                    s16 rotateTowardsCenter) {
    static Vec3f velInit = { 0.0f, 0.0f, 0.0f };
    static Vec3f accelInit = { 0.0f, -0.5f, 0.0f };
    static Vec3f check_P = { -150.0f, 0.0f, -350.0f };
    Vec3f* basePos = NULL;
    s16 i;
    Vec3f vel;
    Vec3f accel;
    Vec3f pos;

    check_P.z += dz; // dz is always 0
    Skeleton_Info2_anime_play(&this->skelanime);
    add_calc2(&this->actor.speed, targetSpeedXZ, 0.5f, 2.0f);

    if (rotateTowardsCenter) {
        adds(&this->actor.world.rot.y, search_position_angleY(&this->actor.world.pos, &check_P) + 0x8000, 3,
                       0x3E8);
    }

    if (Skeleton_Info_frame_check(&this->skelanime, 9.0f)) {
        basePos = &this->rightHandBackLimbWorldPos;
    } else if (Skeleton_Info_frame_check(&this->skelanime, 1.0f)) {
        basePos = &this->leftHandBackLimbWorldPos;
    }

    if (basePos != NULL) {
        for (i = 0; i < 5; i++) {
            vel = velInit;
            accel = accelInit;
            pos.x = rnd_fx(70.0f) + basePos->x;
            pos.y = rnd_f(30.0f) + basePos->y;
            pos.z = rnd_fx(70.0f) + basePos->z;
            Effect_Hahen_ct3(play, &pos, &vel, &accel, 0, (s16)(fqrand() * 5.0f) + 10, -1, 10, NULL);
        }
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_HIGH);
    }
}

void set_demo_S_03(BossGoma* this, PlayState* play) {
    Player* player;
    Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

    player = GET_PLAYER(play);
    this->actionState = 4;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Demo_play_start(play, &play->csCtx);
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
    this->subCamId = Gama_play_make_camera(play);
    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_UNK3);
    Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
    Skeleton_Info2_init(&this->skelanime, &gGohmaEyeRollAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaEyeRollAnim),
                     ANIMMODE_ONCE, 0.0f);
    this->currentAnimFrameCount = Si2_anime_end_frame(&gGohmaEyeRollAnim);

    // room center (todo: defines for hardcoded positions relative to room center)
    this->actor.world.pos.x = -150.0f;
    this->actor.world.pos.z = -350.0f;

    // room entrance, towards center
    player->actor.world.pos.x = 150.0f;
    player->actor.world.pos.z = 300.0f;

    player->actor.world.rot.y = player->actor.shape.rot.y = -0x705C;
    this->actor.world.rot.y = Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor) + 0x8000;

    // room entrance, closer to room center
    this->subCamEye.x = 90.0f;
    this->subCamEye.z = 170.0f;
    this->subCamEye.y = mainCam->eye.y + 20.0f;

    this->framesUntilNextAction = 50;

    this->subCamAt.x = this->actor.world.pos.x;
    this->subCamAt.y = this->actor.world.pos.y;
    this->subCamAt.z = this->actor.world.pos.z;

    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
}

/**
 * Spawns the door once the player entered
 * Wait for the player to look at Gohma on the ceiling
 * Handles the "meeting Gohma" cutscene, including boss card
 *
 * Skips the door and look-at-Gohma puzzle if the player already reached the boss card part before
 */
static void mode_start_demo(BossGoma* this, PlayState* play) {
    Camera* mainCam;
    Player* player = GET_PLAYER(play);
    f32 s;
    s32 pad;

    add_calc0(&this->actor.speed, 0.5f, 2.0f);

    switch (this->actionState) {
        case 0: // wait for the player to enter the room
            // entrance of the boss room
            if (fabsf(player->actor.world.pos.x - 150.0f) < 60.0f &&
                fabsf(player->actor.world.pos.z - 350.0f) < 60.0f) {
                if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_GOHMA_BATTLE)) {
                    set_demo_S_03(this, play);
                    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_SHUTTER, 164.72f, -480.0f,
                                       397.68002f, 0, -0x705C, 0, DOORSHUTTER_PARAMS(SHUTTER_GOHMA_BLOCK, 0));
                } else {
                    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
                    this->actionState = 1;
                }
            }
            break;

        case 1: // player entered the room
            Demo_play_start(play, &play->csCtx);
            this->subCamId = Gama_play_make_camera(play);
            PRINTF("MAKE CAMERA !!!   1   !!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            this->actionState = 2;
            // ceiling center
            this->actor.world.pos.x = -150.0f;
            this->actor.world.pos.y = -320.0f;
            this->actor.world.pos.z = -350.0f;
            // room entrance
            player->actor.world.pos.x = 150.0f;
            player->actor.world.pos.z = 300.0f;
            // near ceiling center
            this->subCamEye.x = -350.0f;
            this->subCamEye.y = -310.0f;
            this->subCamEye.z = -350.0f;
            // below room entrance
            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = player->actor.world.pos.y - 200.0f + 25.0f;
            this->subCamAt.z = player->actor.world.pos.z;
            this->framesUntilNextAction = 50;
            this->timer = 80;
            this->frameCount = 0;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            FALLTHROUGH;
        case 2: // zoom on player from room center
            // room entrance, towards center
            player->actor.shape.rot.y = -0x705C;
            player->actor.world.pos.x = 150.0f;
            player->actor.world.pos.z = 300.0f;
            player->actor.world.rot.y = player->actor.shape.rot.y;
            player->actor.speed = 0.0f;

            if (this->framesUntilNextAction == 0) {
                // (-20, 25, -65) is towards room center
                add_calc2(&this->subCamEye.x, player->actor.world.pos.x - 20.0f, 0.049999997f,
                               this->subCamFollowSpeed * 50.0f);
                add_calc2(&this->subCamEye.y, player->actor.world.pos.y + 25.0f, 0.099999994f,
                               this->subCamFollowSpeed * 130.0f);
                add_calc2(&this->subCamEye.z, player->actor.world.pos.z - 65.0f, 0.049999997f,
                               this->subCamFollowSpeed * 30.0f);
                add_calc2(&this->subCamFollowSpeed, 0.29999998f, 1.0f, 0.0050000004f);
                if (this->timer == 0) {
                    add_calc2(&this->subCamAt.y, player->actor.world.pos.y + 35.0f, 0.099999994f,
                                   this->subCamFollowSpeed * 30.0f);
                }
                this->subCamAt.x = player->actor.world.pos.x;
                this->subCamAt.z = player->actor.world.pos.z;
            }

            Gama_play_camera_setting(play, CAM_ID_MAIN, &this->subCamAt, &this->subCamEye);

            if (this->frameCount == 176) {
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_SHUTTER, 164.72f, -480.0f,
                                   397.68002f, 0, -0x705C, 0, DOORSHUTTER_PARAMS(SHUTTER_GOHMA_BLOCK, 0));
            }

            if (this->frameCount == 176) {
                play->envCtx.lightSettingOverride = 3;
                play->envCtx.lightBlendRateOverride = LIGHT_BLENDRATE_OVERRIDE_NONE;
            }

            if (this->frameCount == 190) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_2);
            }

            if (this->frameCount >= 228) {
                Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                this->actionState = 3;
            }
            break;

        case 3: // wait for the player to look at Gohma
            if (fabsf(this->actor.projectedPos.x) < 150.0f && fabsf(this->actor.projectedPos.y) < 250.0f &&
                this->actor.projectedPos.z < 800.0f && this->actor.projectedPos.z > 0.0f) {
                this->lookedAtFrames++;
                add_calc0(&this->actor.speed, 0.5f, 2.0f);
                adds(&this->actor.world.rot.y,
                               Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor) + 0x8000, 2, 0xBB8);
                this->eyeLidBottomRotX = this->eyeLidTopRotX = this->eyeIrisRotX = this->eyeIrisRotY = 0;
            } else {
                this->lookedAtFrames = 0;
                goma_roof_walk(this, play, 0.0f, -5.0f, true);
            }

            if (this->lookedAtFrames > 15) {
                set_demo_S_03(this, play);
            }
            break;

        case 4: // focus Gohma on the ceiling
            if (Skeleton_Info_frame_check(&this->skelanime, 15.0f)) {
                Actor_SE_set(&this->actor, NA_SE_EN_GOMA_DEMO_EYE);
            }

            if (this->framesUntilNextAction <= 40) {
                // (22, -25, 45) is towards room entrance
                add_calc2(&this->subCamEye.x, this->actor.world.pos.x + 22.0f, 0.2f, 100.0f);
                add_calc2(&this->subCamEye.y, this->actor.world.pos.y - 25.0f, 0.2f, 100.0f);
                add_calc2(&this->subCamEye.z, this->actor.world.pos.z + 45.0f, 0.2f, 100.0f);
                add_calc2(&this->subCamAt.x, this->actor.world.pos.x, 0.2f, 100.0f);
                add_calc2(&this->subCamAt.y, this->actor.world.pos.y + 5.0f, 0.2f, 100.0f);
                add_calc2(&this->subCamAt.z, this->actor.world.pos.z, 0.2f, 100.0f);

                if (this->framesUntilNextAction == 30) {
                    play->envCtx.lightSettingOverride = 4;
                }

                if (this->framesUntilNextAction < 20) {
                    Skeleton_Info2_anime_play(&this->skelanime);
                    add_calc2(&this->eyeIrisScaleX, 1.0f, 0.8f, 0.4f);
                    add_calc2(&this->eyeIrisScaleY, 1.0f, 0.8f, 0.4f);

                    if (Skeleton_Info_frame_check(&this->skelanime, 36.0f)) {
                        this->eyeIrisScaleX = 1.8f;
                        this->eyeIrisScaleY = 1.8f;
                    }

                    if (Skeleton_Info_frame_check(&this->skelanime, this->currentAnimFrameCount)) {
                        this->actionState = 5;
                        Skeleton_Info2_init(&this->skelanime, &gGohmaWalkAnim, 2.0f, 0.0f,
                                         Si2_anime_end_frame(&gGohmaWalkAnim), ANIMMODE_LOOP, -5.0f);
                        this->framesUntilNextAction = 30;
                        this->subCamFollowSpeed = 0.0f;
                    }
                }
            }
            break;

        case 5: // running on the ceiling
            // (98, 0, 85) is towards room entrance
            add_calc2(&this->subCamEye.x, this->actor.world.pos.x + 8.0f + 90.0f, 0.1f,
                           this->subCamFollowSpeed * 30.0f);
            add_calc2(&this->subCamEye.y, player->actor.world.pos.y, 0.1f, this->subCamFollowSpeed * 30.0f);
            add_calc2(&this->subCamEye.z, this->actor.world.pos.z + 45.0f + 40.0f, 0.1f,
                           this->subCamFollowSpeed * 30.0f);
            add_calc2(&this->subCamFollowSpeed, 1.0f, 1.0f, 0.05f);
            this->subCamAt.x = this->actor.world.pos.x;
            this->subCamAt.y = this->actor.world.pos.y;
            this->subCamAt.z = this->actor.world.pos.z;

            if (this->framesUntilNextAction < 0) {
                //! @bug ? unreachable, timer is >= 0
                Skeleton_Info2_anime_play(&this->skelanime);
                add_calc0(&this->actor.speed, 1.0f, 2.0f);
            } else {
                goma_roof_walk(this, play, 0.0f, -7.5f, false);
            }

            if (this->framesUntilNextAction == 0) {
                Skeleton_Info2_init(&this->skelanime, &gGohmaHangAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGohmaHangAnim),
                                 ANIMMODE_LOOP, -5.0f);
            }

            if (this->framesUntilNextAction == 0) {
                this->actionState = 9;
                this->actor.speed = 0.0f;
                this->actor.velocity.y = 0.0f;
                this->actor.gravity = -2.0f;
                Skeleton_Info2_init(&this->skelanime, &gGohmaInitialLandingAnim, 1.0f, 0.0f,
                                 Si2_anime_end_frame(&gGohmaInitialLandingAnim), ANIMMODE_ONCE, -5.0f);
                player->actor.world.pos.x = 0.0f;
                player->actor.world.pos.z = -30.0f;
            }
            break;

        case 9: // falling from the ceiling
            add_calc2(&this->subCamEye.x, this->actor.world.pos.x + 8.0f + 90.0f, 0.1f,
                           this->subCamFollowSpeed * 30.0f);
            add_calc2(&this->subCamEye.y, player->actor.world.pos.y + 10.0f, 0.1f,
                           this->subCamFollowSpeed * 30.0f);
            add_calc2(&this->subCamEye.z, this->actor.world.pos.z + 45.0f + 40.0f, 0.1f,
                           this->subCamFollowSpeed * 30.0f);
            this->subCamAt.x = this->actor.world.pos.x;
            this->subCamAt.y = this->actor.world.pos.y;
            this->subCamAt.z = this->actor.world.pos.z;
            Skeleton_Info2_anime_play(&this->skelanime);
            adds(&this->actor.shape.rot.x, 0, 2, 0xBB8);
            adds(&this->actor.world.rot.y, Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor),
                           2, 0x7D0);

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                this->actionState = 130;
                this->actor.velocity.y = 0.0f;
                Skeleton_Info2_init(&this->skelanime, &gGohmaInitialLandingAnim, 1.0f, 0.0f,
                                 Si2_anime_end_frame(&gGohmaInitialLandingAnim), ANIMMODE_ONCE, -2.0f);
                this->currentAnimFrameCount = Si2_anime_end_frame(&gGohmaInitialLandingAnim);
                goma_chakuchi_dust_se_set(this, play, 0, 5);
                this->framesUntilNextAction = 15;
                z_vibctl2_vib_force_set(0.0f, 200, 20, 20);
            }
            break;

        case 130: // focus Gohma on the ground
            add_calc2(&this->subCamEye.x, this->actor.world.pos.x + 8.0f + 90.0f, 0.1f,
                           this->subCamFollowSpeed * 30.0f);
            add_calc2(&this->subCamEye.y, player->actor.world.pos.y + 10.0f, 0.1f,
                           this->subCamFollowSpeed * 30.0f);
            add_calc2(&this->subCamEye.z, this->actor.world.pos.z + 45.0f + 40.0f, 0.1f,
                           this->subCamFollowSpeed * 30.0f);
            adds(&this->actor.shape.rot.x, 0, 2, 0xBB8);
            adds(&this->actor.world.rot.y, Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor),
                           2, 0x7D0);
            Skeleton_Info2_anime_play(&this->skelanime);
            this->subCamAt.x = this->actor.world.pos.x;
            this->subCamAt.z = this->actor.world.pos.z;

            if (this->framesUntilNextAction != 0) {
                s = sinf(this->framesUntilNextAction * 3.1415f * 0.5f);
                this->subCamAt.y = this->framesUntilNextAction * s * 0.7f + this->actor.world.pos.y;
            } else {
                add_calc2(&this->subCamAt.y, this->actor.focus.pos.y, 0.1f, 10.0f);
            }

            if (Skeleton_Info_frame_check(&this->skelanime, 40.0f)) {
                Actor_SE_set(&this->actor, NA_SE_EN_GOMA_CRY1);

                if (!GET_EVENTCHKINF(EVENTCHKINF_BEGAN_GOHMA_BATTLE)) {
                    Actor_Name_Disp_Set(play, &play->actorCtx.titleCtx, SEGMENTED_TO_VIRTUAL(gGohmaTitleCardTex),
                                           160, 180, 128, 40);
                }

                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS);
                SET_EVENTCHKINF(EVENTCHKINF_BEGAN_GOHMA_BATTLE);
            }

            if (Skeleton_Info_frame_check(&this->skelanime, this->currentAnimFrameCount)) {
                this->actionState = 140;
                Skeleton_Info2_init(&this->skelanime, &gGohmaStandAnim, 1.0f, 0.0f,
                                 Si2_anime_end_frame(&gGohmaStandAnim), ANIMMODE_LOOP, -10.0f);
                this->framesUntilNextAction = 20;
            }
            break;

        case 140:
            Skeleton_Info2_anime_play(&this->skelanime);
            add_calc2(&this->subCamAt.y, this->actor.focus.pos.y, 0.1f, 10.0f);

            if (this->framesUntilNextAction == 0) {
                this->framesUntilNextAction = 30;
                this->actionState = 150;
                Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_UNK3);
            }
            break;

        case 150:
            Skeleton_Info2_anime_play(&this->skelanime);
            add_calc(&this->subCamEye.x, this->actor.world.pos.x + 150.0f, 0.2f, 100.0f, 0.1f);
            add_calc(&this->subCamEye.y, this->actor.world.pos.y + 20.0f, 0.2f, 100.0f, 0.1f);
            add_calc(&this->subCamEye.z, this->actor.world.pos.z + 220.0f, 0.2f, 100.0f, 0.1f);

            if (this->framesUntilNextAction == 0) {
                mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                mode_walk_init(this);
                this->disableGameplayLogic = false;
                this->patienceTimer = 200;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
            }
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

/**
 * Handles the "Gohma defeated" cutscene and effects
 * Spawns the heart container and blue warp actors
 */
static void mode_down(BossGoma* this, PlayState* play) {
    static Vec3f check_P = { -150.0f, 0.0f, -350.0f };
    f32 dx;
    f32 dz;
    s16 j;
    Vec3f vel1 = { 0.0f, 0.0f, 0.0f };
    Vec3f accel1 = { 0.0f, 1.0f, 0.0f };
    Color_RGBA8 color1 = { 255, 255, 255, 255 };
    Color_RGBA8 color2 = { 0, 100, 255, 255 };
    Vec3f vel2 = { 0.0f, 0.0f, 0.0f };
    Vec3f accel2 = { 0.0f, -0.5f, 0.0f };
    Vec3f pos;
    Camera* mainCam;
    Player* player = GET_PLAYER(play);
    Vec3f childPos;
    s16 i;

    Skeleton_Info2_anime_play(&this->skelanime);
    adds(&this->actor.shape.rot.x, 0, 2, 0xBB8);

    if (Skeleton_Info_frame_check(&this->skelanime, 107.0f)) {
        goma_chakuchi_dust_se_set(this, play, 0, 8);
        z_vibctl2_vib_force_set(0.0f, 150, 20, 20);
    }

    this->visualState = VISUALSTATE_DEFEATED;
    this->eyeState = EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES;

    if (this->framesUntilNextAction == 1001) {
        for (i = 0; i < 90; i++) {
            if (goma_buhin_timer[i] != 0) {
                this->deadLimbsState[i] = 1;
            }
        }
    }

    if (this->framesUntilNextAction < 1200 && this->framesUntilNextAction > 1100 &&
        this->framesUntilNextAction % 8 == 0) {
        DamageEffectSibukiSet(play, &this->actor.focus.pos);
    }

    if (this->framesUntilNextAction < 1080 && this->actionState < 3) {
        if (this->framesUntilNextAction < 1070) {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_LAST - SFX_FLAG);
        }

        for (i = 0; i < 4; i++) {
            //! @bug this 0-indexes into this->defeatedLimbPositions which is initialized with
            //! this->defeatedLimbPositions[limb], but limb is 1-indexed in skelanime callbacks, this means effects
            //! should spawn at this->defeatedLimbPositions[0] too, which is uninitialized, so map origin?
            j = (s16)(fqrand() * (BOSSGOMA_LIMB_MAX - 1));
            if (this->defeatedLimbPositions[j].y < 10000.0f) {
                pos.x = rnd_fx(20.0f) + this->defeatedLimbPositions[j].x;
                pos.y = rnd_fx(10.0f) + this->defeatedLimbPositions[j].y;
                pos.z = rnd_fx(20.0f) + this->defeatedLimbPositions[j].z;
                Effect_SS_Dust_sc_cl_co_ct(play, &pos, &vel1, &accel1, &color1, &color2, 500, 10, 10);
            }
        }

        for (i = 0; i < 15; i++) {
            //! @bug same as above
            j = (s16)(fqrand() * (BOSSGOMA_LIMB_MAX - 1));
            if (this->defeatedLimbPositions[j].y < 10000.0f) {
                pos.x = rnd_fx(20.0f) + this->defeatedLimbPositions[j].x;
                pos.y = rnd_fx(10.0f) + this->defeatedLimbPositions[j].y;
                pos.z = rnd_fx(20.0f) + this->defeatedLimbPositions[j].z;
                Effect_Hahen_ct3(play, &pos, &vel2, &accel2, 0, (s16)(fqrand() * 5.0f) + 10, -1, 10, NULL);
            }
        }
    }

    switch (this->actionState) {
        case 0:
            this->actionState = 1;
            Demo_play_start(play, &play->csCtx);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
            this->subCamId = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_UNK3);
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
            this->subCamEye.x = mainCam->eye.x;
            this->subCamEye.y = mainCam->eye.y;
            this->subCamEye.z = mainCam->eye.z;
            this->subCamAt.x = mainCam->at.x;
            this->subCamAt.y = mainCam->at.y;
            this->subCamAt.z = mainCam->at.z;
            dx = this->subCamEye.x - this->actor.world.pos.x;
            dz = this->subCamEye.z - this->actor.world.pos.z;
            this->defeatedCameraEyeDist = sqrtf(SQ(dx) + SQ(dz));
            this->defeatedCameraEyeAngle = fatan2(dx, dz);
            this->timer = 270;
            break;

        case 1:
            dx = sin_s(this->actor.shape.rot.y) * 100.0f;
            dz = cos_s(this->actor.shape.rot.y) * 100.0f;
            add_calc2(&player->actor.world.pos.x, this->actor.world.pos.x + dx, 0.5f, 5.0f);
            add_calc2(&player->actor.world.pos.z, this->actor.world.pos.z + dz, 0.5f, 5.0f);

            if (this->framesUntilNextAction < 1080) {
                this->noBackfaceCulling = true;

                for (i = 0; i < 4; i++) {
                    tex_mesh(mesh_data, this->decayingProgress);
                    //! @bug this allows this->decayingProgress = 0x100 = 256 which
                    //! is out of bounds when accessing mesh_data
                    if (this->decayingProgress < 0x100) {
                        this->decayingProgress++;
                    }
                }
            }

            if (this->framesUntilNextAction < 1070 && this->frameCount % 4 == 0 && fqrand() < 0.5f) {
                this->blinkTimer = 3;
            }

            this->defeatedCameraEyeAngle += 0.022f;
            add_calc2(&this->defeatedCameraEyeDist, 150.0f, 0.1f, 5.0f);
            dx = sinf(this->defeatedCameraEyeAngle);
            dx = dx * this->defeatedCameraEyeDist;
            dz = cosf(this->defeatedCameraEyeAngle);
            dz = dz * this->defeatedCameraEyeDist;
            add_calc(&this->subCamEye.x, this->actor.world.pos.x + dx, 0.2f, 50.0f, 0.1f);
            add_calc(&this->subCamEye.y, this->actor.world.pos.y + 20.0f, 0.2f, 50.0f, 0.1f);
            add_calc(&this->subCamEye.z, this->actor.world.pos.z + dz, 0.2f, 50.0f, 0.1f);
            add_calc(&this->subCamAt.x, this->firstTailLimbWorldPos.x, 0.2f, 50.0f, 0.1f);
            add_calc(&this->subCamAt.y, this->actor.focus.pos.y, 0.5f, 100.0f, 0.1f);
            add_calc(&this->subCamAt.z, this->firstTailLimbWorldPos.z, 0.2f, 50.0f, 0.1f);

            if (this->timer == 80) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS_CLEAR);
            }

            if (this->timer == 0) {
                this->actionState = 2;
                Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_UNK3);
                this->timer = 70;
                this->decayingProgress = 0;
                this->subCamFollowSpeed = 0.0f;
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, 0, 0, 0);
            }
            break;

        case 2:
            mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
            add_calc(&this->subCamEye.x, mainCam->eye.x, 0.2f, this->subCamFollowSpeed * 50.0f, 0.1f);
            add_calc(&this->subCamEye.y, mainCam->eye.y, 0.2f, this->subCamFollowSpeed * 50.0f, 0.1f);
            add_calc(&this->subCamEye.z, mainCam->eye.z, 0.2f, this->subCamFollowSpeed * 50.0f, 0.1f);
            add_calc(&this->subCamAt.x, mainCam->at.x, 0.2f, this->subCamFollowSpeed * 50.0f, 0.1f);
            add_calc(&this->subCamAt.y, mainCam->at.y, 0.2f, this->subCamFollowSpeed * 50.0f, 0.1f);
            add_calc(&this->subCamAt.z, mainCam->at.z, 0.2f, this->subCamFollowSpeed * 50.0f, 0.1f);
            add_calc(&this->subCamFollowSpeed, 1.0f, 1.0f, 0.02f, 0.0f);

            if (this->timer == 0) {
                childPos = check_P;
                this->timer = 30;
                this->actionState = 3;

                for (i = 0; i < 10000; i++) {
                    if ((fabsf(childPos.x - player->actor.world.pos.x) < 100.0f &&
                         fabsf(childPos.z - player->actor.world.pos.z) < 100.0f) ||
                        (fabsf(childPos.x - this->actor.world.pos.x) < 150.0f &&
                         fabsf(childPos.z - this->actor.world.pos.z) < 150.0f)) {
                        childPos.x = rnd_fx(400.0f) + -150.0f;
                        childPos.z = rnd_fx(400.0f) + -350.0f;
                    } else {
                        break;
                    }
                }

                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, childPos.x,
                                   this->actor.world.pos.y, childPos.z, 0, 0, 0, WARP_DUNGEON_CHILD);
                Actor_Environment_room_clear_On(play, play->roomCtx.curRoom.num);
            }

            for (i = 0; i < 4; i++) {
                tex_mesh(mesh_data_cl, this->decayingProgress);
                //! @bug same as mesh_data
                if (this->decayingProgress < 0x100) {
                    this->decayingProgress++;
                }
            }
            break;

        case 3:
            for (i = 0; i < 4; i++) {
                tex_mesh(mesh_data_cl, this->decayingProgress);
                //! @bug same as mesh_data
                if (this->decayingProgress < 0x100) {
                    this->decayingProgress++;
                }
            }

            if (this->timer == 0) {
                if (add_calc(&this->actor.scale.y, 0, 1.0f, 0.00075f, 0.0f) <= 0.001f) {
                    mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
                    mainCam->eye = this->subCamEye;
                    mainCam->eyeNext = this->subCamEye;
                    mainCam->at = this->subCamAt;
                    Gama_play_shift2main_camera(play, this->subCamId, 0);
                    this->subCamId = SUB_CAM_ID_DONE;
                    Demo_play_end(play, &play->csCtx);
                    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                    Actor_delete(&this->actor);
                }

                this->actor.scale.x = this->actor.scale.z = this->actor.scale.y;
            }
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }

    if (this->blinkTimer != 0) {
        this->blinkTimer--;
        play->envCtx.adjAmbientColor[0] += 40;
        play->envCtx.adjAmbientColor[1] += 40;
        play->envCtx.adjAmbientColor[2] += 80;
        play->envCtx.adjFogColor[0] += 10;
        play->envCtx.adjFogColor[1] += 10;
        play->envCtx.adjFogColor[2] += 20;
    } else {
        play->envCtx.adjAmbientColor[0] -= 20;
        play->envCtx.adjAmbientColor[1] -= 20;
        play->envCtx.adjAmbientColor[2] -= 40;
        play->envCtx.adjFogColor[0] -= 5;
        play->envCtx.adjFogColor[1] -= 5;
        play->envCtx.adjFogColor[2] -= 10;
    }

    if (play->envCtx.adjAmbientColor[0] > 200) {
        play->envCtx.adjAmbientColor[0] = 200;
    }
    if (play->envCtx.adjAmbientColor[1] > 200) {
        play->envCtx.adjAmbientColor[1] = 200;
    }
    if (play->envCtx.adjAmbientColor[2] > 200) {
        play->envCtx.adjAmbientColor[2] = 200;
    }
    if (play->envCtx.adjFogColor[0] > 70) {
        play->envCtx.adjFogColor[0] = 70;
    }
    if (play->envCtx.adjFogColor[1] > 70) {
        play->envCtx.adjFogColor[1] = 70;
    }
    if (play->envCtx.adjFogColor[2] > 140) {
        play->envCtx.adjFogColor[2] = 140;
    }

    if (play->envCtx.adjAmbientColor[0] < 0) {
        play->envCtx.adjAmbientColor[0] = 0;
    }
    if (play->envCtx.adjAmbientColor[1] < 0) {
        play->envCtx.adjAmbientColor[1] = 0;
    }
    if (play->envCtx.adjAmbientColor[2] < 0) {
        play->envCtx.adjAmbientColor[2] = 0;
    }
    if (play->envCtx.adjFogColor[0] < 0) {
        play->envCtx.adjFogColor[0] = 0;
    }
    if (play->envCtx.adjFogColor[1] < 0) {
        play->envCtx.adjFogColor[1] = 0;
    }
    if (play->envCtx.adjFogColor[2] < 0) {
        play->envCtx.adjFogColor[2] = 0;
    }
}

/**
 * If the player backs off, cancel the attack, or attack.
 */
static void mode_pre_atack(BossGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);
    add_calc0(&this->actor.speed, 0.5f, 2.0f);

    if (this->skelanime.curFrame >= (19.0f + 1.0f / 3.0f) && this->skelanime.curFrame <= 30.0f) {
        adds(&this->actor.world.rot.y, Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor), 3,
                       0xBB8);
    }

    if (Skeleton_Info_frame_check(&this->skelanime, Si2_anime_end_frame(&gGohmaPrepareAttackAnim))) {
        if (this->actor.xzDistToPlayer < 250.0f) {
            mode_pre_atack2_init(this);
        } else {
            mode_walk_init(this);
        }
    }

    this->eyeState = EYESTATE_IRIS_FOLLOW_NO_IFRAMES;
    this->visualState = VISUALSTATE_RED;
}

/**
 * Only lasts 1 frame. Plays a sound effect.
 */
void mode_pre_atack2(BossGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->framesUntilNextAction == 0) {
        mode_atack_init(this);
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_CRY1);
    }

    this->eyeState = EYESTATE_IRIS_FOLLOW_NO_IFRAMES;
    this->visualState = VISUALSTATE_RED;
}

/**
 * Gohma attacks, then the action eventually goes back to mode_walk
 */
static void mode_atack(BossGoma* this, PlayState* play) {
    s16 i;

    this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
    Skeleton_Info2_anime_play(&this->skelanime);

    switch (this->actionState) {
        case 0:
            for (i = 0; i < this->collider.count; i++) {
                if (this->collider.elements[i].base.atElemFlags & ATELEM_HIT) {
                    this->framesUntilNextAction = 10;
                    break;
                }
            }

            if (Skeleton_Info_frame_check(&this->skelanime, 10.0f)) {
                goma_chakuchi_dust_se_set(this, play, 3, 5);
                ActorQuakeRumbleSet(&this->actor, play, 5, 15);
            }

            if (Skeleton_Info_frame_check(&this->skelanime, Si2_anime_end_frame(&gGohmaAttackAnim))) {
                this->actionState = 1;
                Skeleton_Info2_init(&this->skelanime, &gGohmaRestAfterAttackAnim, 1.0f, 0.0f,
                                 Si2_anime_end_frame(&gGohmaRestAfterAttackAnim), ANIMMODE_LOOP, -1.0f);

                if (this->framesUntilNextAction == 0) {
                    this->timer = (s16)(fqrand() * 30.0f) + 30;
                }
            }
            break;

        case 1:
            if (Skeleton_Info_frame_check(&this->skelanime, 3.0f)) {
                Actor_SE_set(&this->actor, NA_SE_EN_GOMA_UNARI2);
            }

            if (this->timer == 0) {
                this->actionState = 2;
                Skeleton_Info2_init(&this->skelanime, &gGohmaRecoverAfterAttackAnim, 1.0f, 0.0f,
                                 Si2_anime_end_frame(&gGohmaRecoverAfterAttackAnim), ANIMMODE_ONCE, -5.0f);
            }
            break;

        case 2:
            if (Skeleton_Info_frame_check(&this->skelanime, Si2_anime_end_frame(&gGohmaRecoverAfterAttackAnim))) {
                mode_wait_init(this);
            }
            break;
    }

    this->eyeState = EYESTATE_IRIS_FOLLOW_NO_IFRAMES;
    this->visualState = VISUALSTATE_RED;
}

/**
 * Plays the animation to its end, then goes back to mode_stan
 */
static void mode_damage(BossGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (Skeleton_Info_frame_check(&this->skelanime, Si2_anime_end_frame(&gGohmaDamageAnim))) {
        mode_stan_init(this);
        this->patienceTimer = 0;
    }

    this->eyeState = EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES;
    add_calc2(&this->eyeIrisScaleX, 0.4f, 0.5f, 0.2f);
    this->visualState = VISUALSTATE_HIT;
}

/**
 * Gohma is back on the floor after the player struck it down from the ceiling.
 * Sets patience to 0
 * Gohma is then stunned (mode_stan)
 */
void mode_chakuchi(BossGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (Skeleton_Info_frame_check(&this->skelanime, this->currentAnimFrameCount)) {
        mode_stan_init(this);
        this->sfxFaintTimer = 92;
        this->patienceTimer = 0;
        this->framesUntilNextAction = 150;
    }

    _dust_ground_set(play, &this->actor, &this->actor.world.pos, 55.0f, 4, 8.0f, 500, 10, true);
}

/**
 * Gohma is back on the floor after the player has killed its children Gohmas.
 * Plays an animation then goes to usual floor behavior, with refilled patience.
 */
void mode_chakuchi02(BossGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (Skeleton_Info_frame_check(&this->skelanime, this->currentAnimFrameCount)) {
        mode_wait_init(this);
        this->patienceTimer = 200;
    }
}

/**
 * Gohma is stunned and vulnerable. It can only be damaged during this action.
 */
void mode_stan(BossGoma* this, PlayState* play) {
    if (this->sfxFaintTimer <= 90) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_FAINT - 0x800);
    }
    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->timer == 1) {
        _dust_ground_set(play, &this->actor, &this->actor.world.pos, 55.0f, 4, 8.0f, 500, 10, true);
    }

    add_calc0(&this->actor.speed, 0.5f, 1.0f);

    if (this->framesUntilNextAction == 0) {
        mode_walk_init(this);
        if (this->patienceTimer == 0 && this->actor.xzDistToPlayer < 130.0f) {
            this->timer = 20;
        }
    }

    adds(&this->actor.shape.rot.x, 0, 2, 0xBB8);
    this->eyeState = EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES;
    add_calc2(&this->eyeIrisScaleX, 0.4f, 0.5f, 0.2f);
    this->visualState = VISUALSTATE_STUNNED;
}

/**
 * Gohma goes back to the floor after the player killed the three gohmas it spawned
 */
void mode_fall02(BossGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);
    adds(&this->actor.shape.rot.x, 0, 2, 0xBB8);
    adds(&this->actor.world.rot.y, Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor), 2,
                   0x7D0);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        mode_chakuchi02_init(this);
        this->actor.velocity.y = 0.0f;
        goma_chakuchi_dust_se_set(this, play, 0, 8);
        ActorQuakeRumbleSet(&this->actor, play, 5, 15);
    }
}

/**
 * Gohma falls to the floor after the player hit it
 */
void mode_fall(BossGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);
    adds(&this->actor.shape.rot.x, 0, 2, 0xBB8);
    adds(&this->actor.world.rot.y, Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor), 3,
                   0x7D0);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        mode_chakuchi_init(this);
        this->actor.velocity.y = 0.0f;
        goma_chakuchi_dust_se_set(this, play, 0, 8);
        ActorQuakeRumbleSet(&this->actor, play, 10, 15);
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_DAM1);
    }
}

/**
 * Spawn three gohmas, one after the other. Cannot be interrupted
 */
void mode_umu(BossGoma* this, PlayState* play) {
    s16 i;

    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->frameCount % 16 == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_UNARI);
    }

    add_calc0(&this->actor.speed, 0.5f, 2.0f);
    this->spawnGohmasActionTimer++;

    switch (this->spawnGohmasActionTimer) {
        case 24:
            // BOSSGOMA_LIMB_TAIL1, the tail limb closest to the body
            this->tailLimbsScaleTimers[3] = 10;
            break;
        case 32:
            // BOSSGOMA_LIMB_TAIL2
            this->tailLimbsScaleTimers[2] = 10;
            break;
        case 40:
            // BOSSGOMA_LIMB_TAIL3
            this->tailLimbsScaleTimers[1] = 10;
            break;
        case 48:
            // BOSSGOMA_LIMB_TAIL4, the furthest from the body
            this->tailLimbsScaleTimers[0] = 10;
            break;
    }

    if (this->tailLimbsScaleTimers[0] == 2) {
        for (i = 0; i < ARRAY_COUNT(this->childrenGohmaState); i++) {
            if (this->childrenGohmaState[i] == 0) {
                set_kogoma(this, play, i);
                break;
            }
        }

        if (this->childrenGohmaState[0] == 0 || this->childrenGohmaState[1] == 0 || this->childrenGohmaState[2] == 0) {
            this->spawnGohmasActionTimer = 23;
        }
    }

    if (this->spawnGohmasActionTimer >= 64) {
        mode_U_wait_init(this);
    }

    this->eyeState = EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES;
}

/**
 * Prepare to spawn children gohmas, red eye for 70 frames
 * During this time, the player can interrupt by hitting Gohma and make it fall from the ceiling
 */
void mode_standby_umu(BossGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->framesUntilNextAction == 0) {
        mode_umu_init(this);
    }

    this->eyeState = EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES;
    this->visualState = VISUALSTATE_RED;
}

/**
 * On the floor, not doing anything special.
 */
static void mode_wait(BossGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);
    add_calc0(&this->actor.speed, 0.5f, 2.0f);
    adds(&this->actor.shape.rot.x, 0, 2, 0xBB8);

    if (this->framesUntilNextAction == 0) {
        mode_walk_init(this);
    }
}

/**
 * On the ceiling, not doing anything special.
 * Eventually spawns children gohmas, jumping down to the floor when they are killed, or staying on the ceiling as long
 * as any is still alive.
 */
void mode_U_wait(BossGoma* this, PlayState* play) {
    s16 i;

    Skeleton_Info2_anime_play(&this->skelanime);
    add_calc0(&this->actor.speed, 0.5f, 2.0f);

    if (this->framesUntilNextAction == 0) {
        if (this->childrenGohmaState[0] == 0 && this->childrenGohmaState[1] == 0 && this->childrenGohmaState[2] == 0) {
            // if no child gohma has been spawned
            mode_standby_umu_init(this);
        } else if (this->childrenGohmaState[0] < 0 && this->childrenGohmaState[1] < 0 &&
                   this->childrenGohmaState[2] < 0) {
            // if all children gohmas are dead
            mode_fall02_init(this);
        } else {
            for (i = 0; i < ARRAY_COUNT(this->childrenGohmaState); i++) {
                if (this->childrenGohmaState[i] == 0) {
                    // if any child gohma hasn't been spawned
                    // this seems unreachable since mode_umu spawns all three and can't be
                    // interrupted
                    mode_umu_init(this);
                    return;
                }
            }
            // if all children gohmas have been spawned
            mode_U_walk_init(this);
        }
    }
}

/**
 * Gohma approaches the player as long as it has patience (see patienceTimer), then moves away from the player
 * Gohma climbs any wall it collides with
 * Uses the "walk cautiously" animation
 */
static void mode_walk(BossGoma* this, PlayState* play) {
    s16 rot;

    Skeleton_Info2_anime_play(&this->skelanime);

    if (Skeleton_Info_frame_check(&this->skelanime, 1.0f)) {
        this->doNotMoveThisFrame = true;
    } else if (Skeleton_Info_frame_check(&this->skelanime, 30.0f)) {
        this->doNotMoveThisFrame = true;
    } else if (Skeleton_Info_frame_check(&this->skelanime, 15.0f)) {
        this->doNotMoveThisFrame = true;
    } else if (Skeleton_Info_frame_check(&this->skelanime, 16.0f)) {
        this->doNotMoveThisFrame = true;
    }

    if (Skeleton_Info_frame_check(&this->skelanime, 15.0f)) {
        goma_chakuchi_dust_se_set(this, play, 1, 3);
    } else if (Skeleton_Info_frame_check(&this->skelanime, 30.0f)) {
        goma_chakuchi_dust_se_set(this, play, 2, 3);
    }

    if (this->frameCount % 64 == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_CRY2);
    }

    if (!this->doNotMoveThisFrame) {
        rot = Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor);

        if (this->patienceTimer != 0) {
            this->patienceTimer--;

            if (this->actor.xzDistToPlayer < 150.0f) {
                mode_pre_atack_init(this);
            }

            add_calc2(&this->actor.speed, 10.0f / 3.0f, 0.5f, 2.0f);
            adds(&this->actor.world.rot.y, rot, 5, 0x3E8);
        } else {
            if (this->timer != 0) {
                // move away from the player, walking backwards
                add_calc2(&this->actor.speed, -10.0f, 0.5f, 2.0f);
                this->skelanime.playSpeed = -3.0f;
                if (this->timer == 1) {
                    this->actor.speed = 0.0f;
                }
            } else {
                // move away from the player, walking forwards
                add_calc2(&this->actor.speed, 20.0f / 3.0f, 0.5f, 2.0f);
                this->skelanime.playSpeed = 2.0f;
                rot += 0x8000;
            }

            adds(&this->actor.world.rot.y, rot, 3, 0x9C4);
        }
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.velocity.y = 0.0f;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        mode_up_init(this);
    }

    if (this->framesUntilNextAction == 0 && this->patienceTimer != 0) {
        mode_wait_init(this);
    }
}

/**
 * Gohma moves up until it reaches the ceiling
 */
static void mode_up(BossGoma* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->frameCount % 8 == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_CLIM);
    }

    add_calc2(&this->actor.velocity.y, 5.0f, 0.5f, 2.0f);
    adds(&this->actor.shape.rot.x, -0x4000, 2, 0x7D0);
    adds(&this->actor.world.rot.y, this->actor.wallYaw + 0x8000, 2, 0x5DC);

    // -320 is a bit below boss room ceiling
    if (this->actor.world.pos.y > -320.0f) {
        mode_U_walk_init(this);
        // allow new spawns
        this->childrenGohmaState[0] = this->childrenGohmaState[1] = this->childrenGohmaState[2] = 0;
    }
}

/**
 * Goes to mode_U_wait after enough time and after being close enough to the center of the ceiling.
 */
void mode_U_walk(BossGoma* this, PlayState* play) {
    s16 angle;
    s16 absDiff;

    goma_roof_walk(this, play, 0.0f, -5.0f, true);

    if (this->frameCount % 64 == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_CRY2);
    }

    adds(&this->actor.shape.rot.x, -0x8000, 3, 0x3E8);

    // avoid walking into a wall?
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        angle = this->actor.shape.rot.y + 0x8000;

        if (angle < this->actor.wallYaw) {
            absDiff = this->actor.wallYaw - angle;
            angle = angle + absDiff / 2;
        } else {
            absDiff = angle - this->actor.wallYaw;
            angle = this->actor.wallYaw + absDiff / 2;
        }

        this->actor.world.pos.z += cos_s(angle) * (5.0f + fqrand() * 5.0f) + rnd_fx(2.0f);
        this->actor.world.pos.x += sin_s(angle) * (5.0f + fqrand() * 5.0f) + rnd_fx(2.0f);
    }

    // timer setup to 30-60
    if (this->framesUntilNextAction == 0 && fabsf(-150.0f - this->actor.world.pos.x) < 100.0f &&
        fabsf(-350.0f - this->actor.world.pos.z) < 100.0f) {
        mode_U_wait_init(this);
    }
}

/**
 * Update eye-related properties
 *  - open/close (eye lid rotation)
 *  - look at the player (iris rotation)
 *  - iris scale, when menacing or damaged
 */
void Boss_Goma_Eye_Control(BossGoma* this, PlayState* play) {
    s16 targetEyeIrisRotX;
    s16 targetEyeIrisRotY;

    if (!this->disableGameplayLogic) {
        Player* player = GET_PLAYER(play);

        if (this->eyeState == EYESTATE_IRIS_FOLLOW_BONUS_IFRAMES) {
            // player + 0xA73 seems to be related to "throwing something"
            if (player->unk_A73 != 0) {
                player->unk_A73 = 0;
                this->eyeClosedTimer = 12;
            }

            if (this->frameCount % 16 == 0 && fqrand() < 0.3f) {
                this->eyeClosedTimer = 7;
            }
        }

        if (this->childrenGohmaState[0] > 0 || this->childrenGohmaState[1] > 0 || this->childrenGohmaState[2] > 0) {
            this->eyeClosedTimer = 7;
        }

        if (this->eyeClosedTimer != 0) {
            this->eyeClosedTimer--;
            // close eye
            adds(&this->eyeLidBottomRotX, -0xA98, 1, 0x7D0);
            adds(&this->eyeLidTopRotX, 0x1600, 1, 0x7D0);
        } else {
            // open eye
            adds(&this->eyeLidBottomRotX, 0, 1, 0x7D0);
            adds(&this->eyeLidTopRotX, 0, 1, 0x7D0);
        }

        if (this->eyeState != EYESTATE_IRIS_NO_FOLLOW_NO_IFRAMES) {
            targetEyeIrisRotY =
                Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor) - this->actor.shape.rot.y;
            targetEyeIrisRotX =
                Actor_search_actor_angleX(&this->actor, &GET_PLAYER(play)->actor) - this->actor.shape.rot.x;

            if (this->actor.shape.rot.x > 0x4000 || this->actor.shape.rot.x < -0x4000) {
                targetEyeIrisRotY = -(s16)(targetEyeIrisRotY + 0x8000);
                targetEyeIrisRotX = -0xBB8;
            }

            if (targetEyeIrisRotY > 0x1770) {
                targetEyeIrisRotY = 0x1770;
            }

            if (targetEyeIrisRotY < -0x1770) {
                targetEyeIrisRotY = -0x1770;
            }

            adds(&this->eyeIrisRotY, targetEyeIrisRotY, 3, 0x7D0);
            adds(&this->eyeIrisRotX, targetEyeIrisRotX, 3, 0x7D0);
        } else {
            adds(&this->eyeIrisRotY, 0, 3, 0x3E8);
            adds(&this->eyeIrisRotX, 0, 3, 0x3E8);
        }

        add_calc2(&this->eyeIrisScaleX, 1.0f, 0.2f, 0.07f);
        add_calc2(&this->eyeIrisScaleY, 1.0f, 0.2f, 0.07f);
    }
}

/**
 * Part of achieving visual effects when spawning children gohmas,
 * inflating each tail limb one after the other.
 */
void Boss_Goma_Child_Ct(BossGoma* this) {
    s16 i;

    if (this->frameCount % 128 == 0) {
        this->unusedTimer++;
        if (this->unusedTimer >= 3) {
            this->unusedTimer = 0;
        }
    }

    // See mode_umu for `tailLimbsScaleTimers` usage
    for (i = 0; i < ARRAY_COUNT(this->tailLimbsScaleTimers); i++) {
        if (this->tailLimbsScaleTimers[i] != 0) {
            this->tailLimbsScaleTimers[i]--;
            add_calc2(&this->tailLimbsScale[i], 1.5f, 0.2f, 0.1f);
        } else {
            add_calc2(&this->tailLimbsScale[i], 1.0f, 0.2f, 0.1f);
        }
    }
}

void Boss_Goma_Damage_check(BossGoma* this, PlayState* play) {
    if (this->invincibilityFrames != 0) {
        this->invincibilityFrames--;
    } else {
        ColliderElement* acHitElem = this->collider.elements[0].base.acHitElem;
        s32 damage;

        if (this->eyeClosedTimer == 0 && this->actionFunc != mode_umu &&
            (this->collider.elements[0].base.acElemFlags & ACELEM_HIT)) {
            this->collider.elements[0].base.acElemFlags &= ~ACELEM_HIT;

            if (this->actionFunc == mode_U_walk || this->actionFunc == mode_U_wait ||
                this->actionFunc == mode_standby_umu) {
                mode_fall_init(this);
                Actor_SE_set(&this->actor, NA_SE_EN_GOMA_DAM2);
            } else if (this->actionFunc == mode_stan &&
                       (damage = GetSwordAP(acHitElem->atDmgInfo.dmgFlags)) != 0) {
                this->actor.colChkInfo.health -= damage;

                if ((s8)this->actor.colChkInfo.health > 0) {
                    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_DAM1);
                    mode_damage_init(this);
                    DamageEffectSibukiSet(play, &this->actor.focus.pos);
                } else {
                    mode_down_init(this, play);
                    Actor_info_finish(play, &this->actor);
                }

                this->invincibilityFrames = 10;
            } else if (this->actionFunc != mode_stan && this->patienceTimer != 0 &&
                       (acHitElem->atDmgInfo.dmgFlags & (DMG_SLINGSHOT | DMG_DEKU_NUT))) {
                Actor_SE_set(&this->actor, NA_SE_EN_GOMA_DAM2);
                Nai_StopFx(NA_SE_EN_GOMA_CRY1);
                this->invincibilityFrames = 10;
                mode_stan_init(this);
                this->sfxFaintTimer = 100;

                if (acHitElem->atDmgInfo.dmgFlags & DMG_DEKU_NUT) {
                    this->framesUntilNextAction = 40;
                } else {
                    this->framesUntilNextAction = 90;
                }

                this->timer = 4;
                ActorQuakeRumbleSet(&this->actor, play, 4, 12);
            }
        }
    }
}

void Boss_Goma_Col_Control(BossGoma* this) {
    static f32 go_col[][3] = {
        { 255.0f, 17.0f, 0.0f },  { 0.0f, 255.0f, 170.0f }, { 50.0f, 50.0f, 50.0f },
        { 0.0f, 255.0f, 170.0f }, { 0.0f, 255.0f, 170.0f }, { 0.0f, 255.0f, 170.0f },
    };
    static f32 go_col_2[][3] = {
        { 255.0f, 17.0f, 0.0f },  { 0.0f, 255.0f, 170.0f }, { 50.0f, 50.0f, 50.0f },
        { 0.0f, 255.0f, 170.0f }, { 0.0f, 0.0f, 255.0f },   { 255.0f, 17.0f, 0.0f },
    };

    if (this->visualState == VISUALSTATE_DEFAULT && this->frameCount & 0x10) {
        add_calc2(&this->mainEnvColor[0], 50.0f, 0.5f, 20.0f);
        add_calc2(&this->mainEnvColor[1], 50.0f, 0.5f, 20.0f);
        add_calc2(&this->mainEnvColor[2], 50.0f, 0.5f, 20.0f);
    } else if (this->invincibilityFrames != 0) {
        if (this->invincibilityFrames & 2) {
            this->mainEnvColor[0] = go_col_2[this->visualState][0];
            this->mainEnvColor[1] = go_col_2[this->visualState][1];
            this->mainEnvColor[2] = go_col_2[this->visualState][2];
        } else {
            this->mainEnvColor[0] = go_col[this->visualState][0];
            this->mainEnvColor[1] = go_col[this->visualState][1];
            this->mainEnvColor[2] = go_col[this->visualState][2];
        }
    } else {
        add_calc2(&this->mainEnvColor[0], go_col[this->visualState][0], 0.5f, 20.0f);
        add_calc2(&this->mainEnvColor[1], go_col[this->visualState][1], 0.5f, 20.0f);
        add_calc2(&this->mainEnvColor[2], go_col[this->visualState][2], 0.5f, 20.0f);
    }
}

void Boss_Goma_Eye_Col_Control(BossGoma* this) {
    static f32 go_e_col[][3] = {
        { 255.0f, 17.0f, 0.0f },  { 255.0f, 255.0f, 255.0f }, { 50.0f, 50.0f, 50.0f },
        { 0.0f, 255.0f, 170.0f }, { 0.0f, 255.0f, 170.0f },   { 0.0f, 255.0f, 170.0f },
    };

    add_calc2(&this->eyeEnvColor[0], go_e_col[this->visualState][0], 0.5f, 20.0f);
    add_calc2(&this->eyeEnvColor[1], go_e_col[this->visualState][1], 0.5f, 20.0f);
    add_calc2(&this->eyeEnvColor[2], go_e_col[this->visualState][2], 0.5f, 20.0f);
}

void Boss_Goma_actor_move(Actor* thisx, PlayState* play) {
    BossGoma* this = (BossGoma*)thisx;
    s32 pad;

    this->visualState = VISUALSTATE_DEFAULT;
    this->frameCount++;

    if (this->framesUntilNextAction != 0) {
        this->framesUntilNextAction--;
    }

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->sfxFaintTimer != 0) {
        this->sfxFaintTimer--;
    }

    this->eyeState = EYESTATE_IRIS_FOLLOW_BONUS_IFRAMES;
    this->actionFunc(this, play);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (!this->doNotMoveThisFrame) {
        Actor_position_moveF(&this->actor);
    } else {
        this->doNotMoveThisFrame = false;
    }

    if (this->actor.world.pos.y < -400.0f) {
        Actor_BGcheck2(play, &this->actor, 30.0f, 30.0f, 80.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    } else {
        Actor_BGcheck2(play, &this->actor, 0.0f, 30.0f, 80.0f, UPDBGCHECKINFO_FLAG_0);
    }

    Boss_Goma_Eye_Control(this, play);
    Boss_Goma_Col_Control(this);
    Boss_Goma_Eye_Col_Control(this);
    Boss_Goma_Child_Ct(this);

    if (this->disableGameplayLogic) {
        return;
    }

    Boss_Goma_Damage_check(this, play);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

    if (this->actionFunc != mode_stan && this->actionFunc != mode_damage &&
        (this->actionFunc != mode_walk || this->timer == 0)) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 Boss_Goma_draw_sub1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossGoma* this = (BossGoma*)thisx;
    s32 doNotDrawLimb = false;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_goma.c", 4685);

    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, (s16)this->mainEnvColor[0], (s16)this->mainEnvColor[1], (s16)this->mainEnvColor[2],
                   255);

    if (this->deadLimbsState[limbIndex] >= 2) {
        *dList = NULL;
    }

    switch (limbIndex) {
        case BOSSGOMA_LIMB_EYE:
            if (this->eyeState == EYESTATE_IRIS_FOLLOW_BONUS_IFRAMES && this->eyeLidBottomRotX < -0xA8C) {
                *dList = NULL;
            } else if (this->invincibilityFrames != 0) {
                gDPSetEnvColor(POLY_OPA_DISP++, (s16)(fqrand() * 255.0f), (s16)(fqrand() * 255.0f),
                               (s16)(fqrand() * 255.0f), 63);
            } else {
                gDPSetEnvColor(POLY_OPA_DISP++, (s16)this->eyeEnvColor[0], (s16)this->eyeEnvColor[1],
                               (s16)this->eyeEnvColor[2], 63);
            }
            break;

        case BOSSGOMA_LIMB_EYE_LID_BOTTOM_ROOT2:
            rot->x += this->eyeLidBottomRotX;
            break;

        case BOSSGOMA_LIMB_EYE_LID_TOP_ROOT2:
            rot->x += this->eyeLidTopRotX;
            break;

        case BOSSGOMA_LIMB_IRIS_ROOT2:
            rot->x += this->eyeIrisRotX;
            rot->y += this->eyeIrisRotY;
            break;

        case BOSSGOMA_LIMB_IRIS:
            if (this->eyeState == EYESTATE_IRIS_FOLLOW_BONUS_IFRAMES && this->eyeLidBottomRotX < -0xA8C) {
                *dList = NULL;
            } else {
                if (this->visualState == VISUALSTATE_DEFEATED) {
                    gDPSetEnvColor(POLY_OPA_DISP++, 50, 50, 50, 255);
                } else {
                    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
                }

                Matrix_softcv3_mult(pos, rot);

                if (*dList != NULL) {
                    Matrix_push();
                    Matrix_scale(this->eyeIrisScaleX, this->eyeIrisScaleY, 1.0f, MTXMODE_APPLY);
                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_boss_goma.c", 4815);
                    gSPDisplayList(POLY_OPA_DISP++, *dList);
                    Matrix_pull();
                }

                doNotDrawLimb = true;
            }
            break;

        case BOSSGOMA_LIMB_TAIL4:
        case BOSSGOMA_LIMB_TAIL3:
        case BOSSGOMA_LIMB_TAIL2:
        case BOSSGOMA_LIMB_TAIL1:
            Matrix_softcv3_mult(pos, rot);

            if (*dList != NULL) {
                Matrix_push();
                Matrix_scale(this->tailLimbsScale[limbIndex - BOSSGOMA_LIMB_TAIL4],
                             this->tailLimbsScale[limbIndex - BOSSGOMA_LIMB_TAIL4],
                             this->tailLimbsScale[limbIndex - BOSSGOMA_LIMB_TAIL4], MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_boss_goma.c", 4836);
                gSPDisplayList(POLY_OPA_DISP++, *dList);
                Matrix_pull();
            }

            doNotDrawLimb = true;
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_goma.c", 4858);

    return doNotDrawLimb;
}

void Boss_Goma_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f tail_p = { 0.0f, 0.0f, 0.0f };
    static Vec3f foot_p = { 0.0f, 0.0f, 0.0f };
    static Vec3f eye_p = { 0.0f, 300.0f, 2650.0f }; // in the center of the surface of the lens
    static Vec3f buhin_p1 = { 0.0f, 0.0f, 0.0f };
    Vec3f childPos;
    Vec3s childRot;
    BossGoma* this = (BossGoma*)thisx;
    s32 pad[2];
    MtxF mtx;

    if (limbIndex == BOSSGOMA_LIMB_TAIL4) { // tail end/last part
        Matrix_Position(&tail_p, &this->lastTailLimbWorldPos);
    } else if (limbIndex == BOSSGOMA_LIMB_TAIL1) { // tail start/first part
        Matrix_Position(&tail_p, &this->firstTailLimbWorldPos);
    } else if (limbIndex == BOSSGOMA_LIMB_EYE) {
        Matrix_Position(&eye_p, &this->actor.focus.pos);
    } else if (limbIndex == BOSSGOMA_LIMB_R_FEET_BACK) {
        Matrix_Position(&foot_p, &this->rightHandBackLimbWorldPos);
    } else if (limbIndex == BOSSGOMA_LIMB_L_FEET_BACK) {
        Matrix_Position(&foot_p, &this->leftHandBackLimbWorldPos);
    }

    if (this->visualState == VISUALSTATE_DEFEATED) {
        if (*dList != NULL) {
            Matrix_Position(&foot_p, &this->defeatedLimbPositions[limbIndex]);
        } else {
            this->defeatedLimbPositions[limbIndex].y = 10000.0f;
        }
    }

    if (this->deadLimbsState[limbIndex] == 1) {
        EnGoma* babyGohma;

        this->deadLimbsState[limbIndex] = 2;
        Matrix_Position(&buhin_p1, &childPos);
        Matrix_get(&mtx);
        Matrix_to_rotate_new(&mtx, &childRot, 0);
        // These are the pieces of Gohma as it falls apart. It appears to use the same actor as the baby gohmas.
        babyGohma = (EnGoma*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_GOMA, childPos.x,
                                                childPos.y, childPos.z, childRot.x, childRot.y, childRot.z,
                                                goma_buhin_timer[limbIndex] + 100);
        if (babyGohma != NULL) {
            babyGohma->bossLimbDL = *dList;
            babyGohma->actor.objectSlot = this->actor.objectSlot;
        }
    }

    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->collider);
}

Gfx* goma_mode_1(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dList = dListHead = GRAPH_ALLOC(gfxCtx, sizeof(Gfx) * 1);

    gSPEndDisplayList(dListHead++);

    return dList;
}

Gfx* goma_mode_2(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dList = dListHead = GRAPH_ALLOC(gfxCtx, sizeof(Gfx) * 4);

    gDPPipeSync(dListHead++);
    gDPSetRenderMode(dListHead++, G_RM_PASS, G_RM_AA_ZB_TEX_EDGE2);
    gSPClearGeometryMode(dListHead++, G_CULL_BACK);
    gSPEndDisplayList(dListHead++);

    return dList;
}

void Boss_Goma_actor_draw(Actor* thisx, PlayState* play) {
    BossGoma* this = (BossGoma*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_goma.c", 4991);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Matrix_translate(0.0f, -4000.0f, 0.0f, MTXMODE_APPLY);

    if (this->noBackfaceCulling) {
        gSPSegment(POLY_OPA_DISP++, 0x08, goma_mode_2(play->state.gfxCtx));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, goma_mode_1(play->state.gfxCtx));
    }

    Si2_draw(play, this->skelanime.skeleton, this->skelanime.jointTable, Boss_Goma_draw_sub1,
                      Boss_Goma_draw_sub2, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_goma.c", 5012);
}

void set_kogoma(BossGoma* this, PlayState* play, s16 i) {
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_GOMA, this->lastTailLimbWorldPos.x,
                       this->lastTailLimbWorldPos.y - 50.0f, this->lastTailLimbWorldPos.z, 0, i * (0x10000 / 3), 0, i);

    this->childrenGohmaState[i] = 1;
}
