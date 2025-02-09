/*
 * File: z_en_hy.c
 * Overlay: ovl_En_Hy
 * Description: Hylian NPCs
 */

#include "z_en_hy.h"
#include "versions.h"
#include "assets/objects/object_aob/object_aob.h"
#include "assets/objects/object_ahg/object_ahg.h"
#include "assets/objects/object_bob/object_bob.h"
#include "assets/objects/object_boj/object_boj.h"
#include "assets/objects/object_bba/object_bba.h"
#include "assets/objects/object_bji/object_bji.h"
#include "assets/objects/object_cne/object_cne.h"
#include "assets/objects/object_cob/object_cob.h"
#include "assets/objects/object_os_anime/object_os_anime.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Hy_Actor_ct(Actor* thisx, PlayState* play);
void En_Hy_Actor_dt(Actor* thisx, PlayState* play);
void En_Hy_Actor_move(Actor* thisx, PlayState* play);
void En_Hy_Actor_draw(Actor* thisx, PlayState* play);

static void init(EnHy* this, PlayState* play);
void bji_path_walk_pw(EnHy* this, PlayState* play);
static void carry_end(EnHy* this, PlayState* play);
void boj_path_walk_nw(EnHy* this, PlayState* play);
void bji_wait_pw(EnHy* this, PlayState* play);
void aob_wait(EnHy* this, PlayState* play);
void wait_pw(EnHy* this, PlayState* play);
static void wait_nw(EnHy* this, PlayState* play);
void carry_request(EnHy* this, PlayState* play);

ActorProfile En_Hy_Profile = {
    /**/ ACTOR_EN_HY,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnHy),
    /**/ En_Hy_Actor_ct,
    /**/ En_Hy_Actor_dt,
    /**/ En_Hy_Actor_move,
    /**/ En_Hy_Actor_draw,
};

static ColliderCylinderInit EnHyAtInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 20, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 HyStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

// NULL-terminated arrays of eye textures
static void* Aob_eye[] = {
    gHylianWoman1EyeOpenTex,
    gHylianWoman1EyeHalfTex,
    gHylianWoman1EyeClosedTex,
    NULL,
};
static void* Ahg_eye[] = {
    gHylianMan1BeardedEyeOpenTex,
    gHylianMan1BeardedEyeHalfTex,
    gHylianMan1BeardedEyeClosedTex,
    NULL,
};
static void* Bba_eye[] = {
    gHylianOldWomanEyeTex,
    NULL,
};
static void* Bji_eye[] = {
    gHylianOldManEyeOpenTex,
    gHylianOldManEyeHalfTex,
    gHylianOldManEyeClosedTex,
    NULL,
};
static void* Boj_eye[] = {
    gHylianMan2MustachedEyeOpenTex,
    gHylianMan2MustachedEyeHalfTex,
    gHylianMan2MustachedEyeClosedTex,
    NULL,
};
static void* Bob_eye[] = {
    gHylianWoman2EyeOpenTex,
    gHylianWoman2EyeHalfTex,
    gHylianWoman2EyeClosedTex,
    NULL,
};

typedef struct EnHyHeadInfo {
    /* 0x0 */ s16 objectId;
    /* 0x4 */ Gfx* headDList;
    /* 0x8 */ void** eyeTextures;
} EnHyHeadInfo; // size = 0xC

typedef enum {
    /*  0 */ ENHY_HEAD_WOMAN_1,
    /*  1 */ ENHY_HEAD_WOMAN_2,
    /*  2 */ ENHY_HEAD_MAN_2_MUSTACHE,
    /*  3 */ ENHY_HEAD_MAN_2_BALD,
    /*  4 */ ENHY_HEAD_MAN_2_LONG_HAIR,
    /*  5 */ ENHY_HEAD_MAN_2_ALT_MUSTACHE,
    /*  6 */ ENHY_HEAD_MAN_2_BEARD,
    /*  7 */ ENHY_HEAD_MAN_1_BEARD,
    /*  8 */ ENHY_HEAD_MAN_1_SHAVED,
    /*  9 */ ENHY_HEAD_MAN_1_BOWL_CUT,
    /* 10 */ ENHY_HEAD_OLD_WOMAN,
    /* 11 */ ENHY_HEAD_YOUNG_WOMAN_BROWN_HAIR,
    /* 12 */ ENHY_HEAD_YOUNG_WOMAN_ORANGE_HAIR,
    /* 13 */ ENHY_HEAD_OLD_MAN,
    /* 14 */ ENHY_HEAD_OLD_MAN_BALD,
    /* 15 */ ENHY_HEAD_WOMAN_3
} EnHyHeadIndex;

static EnHyHeadInfo head[] = {
    /* ENHY_HEAD_WOMAN_1 */ { OBJECT_AOB, gHylianWoman1HeadDL, Aob_eye },
    /* ENHY_HEAD_WOMAN_2 */ { OBJECT_BOB, gHylianWoman2HeadDL, Bob_eye },
    /* ENHY_HEAD_MAN_2_MUSTACHE */ { OBJECT_BOJ, gHylianMan2MustachedHeadDL, Boj_eye },
    /* ENHY_HEAD_MAN_2_BALD */ { OBJECT_BOJ, gHylianMan2BaldHeadDL, NULL },
    /* ENHY_HEAD_MAN_2_LONG_HAIR */ { OBJECT_BOJ, gHylianMan2LongHairHeadDL, NULL },
    /* ENHY_HEAD_MAN_2_ALT_MUSTACHE */ { OBJECT_BOJ, gHylianMan2AltMustachedHeadDL, NULL },
    /* ENHY_HEAD_MAN_2_BEARD */ { OBJECT_BOJ, gHylianMan2BeardedHeadDL, NULL },
    /* ENHY_HEAD_MAN_1_BEARD */ { OBJECT_AHG, gHylianMan1BeardedHeadDL, Ahg_eye },
    /* ENHY_HEAD_MAN_1_SHAVED */ { OBJECT_AHG, gHylianMan1ShavedHeadDL, NULL },
    /* ENHY_HEAD_MAN_1_BOWL_CUT */ { OBJECT_AHG, gHylianMan1BowlCutHeadDL, NULL },
    /* ENHY_HEAD_OLD_WOMAN */ { OBJECT_BBA, gHylianOldWomanHeadDL, Bba_eye },
    /* ENHY_HEAD_YOUNG_WOMAN_BROWN_HAIR */ { OBJECT_CNE, gHylianYoungWomanBrownHairHeadDL, NULL },
    /* ENHY_HEAD_YOUNG_WOMAN_ORANGE_HAIR */ { OBJECT_CNE, gHylianYoungWomanOrangeHairHeadDL, NULL },
    /* ENHY_HEAD_OLD_MAN */ { OBJECT_BJI, gHylianOldManHeadDL, Bji_eye },
    /* ENHY_HEAD_OLD_MAN_BALD */ { OBJECT_BJI, gHylianOldManBaldHeadDL, NULL },
    /* ENHY_HEAD_WOMAN_3 */ { OBJECT_COB, gHylianWoman3HeadDL, NULL },
};

typedef struct EnHySkeletonInfo {
    /* 0x0 */ s16 objectId;
    /* 0x4 */ FlexSkeletonHeader* skeleton;
} EnHySkeletonInfo; // size = 0x8

typedef enum {
    /* 0 */ ENHY_SKEL_WOMAN_1,
    /* 1 */ ENHY_SKEL_WOMAN_2,
    /* 2 */ ENHY_SKEL_MAN_2,
    /* 3 */ ENHY_SKEL_MAN_1,
    /* 4 */ ENHY_SKEL_OLD_WOMAN,
    /* 5 */ ENHY_SKEL_YOUNG_WOMAN,
    /* 6 */ ENHY_SKEL_OLD_MAN,
    /* 7 */ ENHY_SKEL_WOMAN_3
} EnHySkeletonIndex;

static EnHySkeletonInfo body[] = {
    /* ENHY_SKEL_WOMAN_1 */ { OBJECT_AOB, &gHylianWoman1Skel },
    /* ENHY_SKEL_WOMAN_2 */ { OBJECT_BOB, &gHylianWoman2Skel },
    /* ENHY_SKEL_MAN_2 */ { OBJECT_BOJ, &gHylianMan2Skel },
    /* ENHY_SKEL_MAN_1 */ { OBJECT_AHG, &gHylianMan1Skel },
    /* ENHY_SKEL_OLD_WOMAN */ { OBJECT_BBA, &gHylianOldWomanSkel },
    /* ENHY_SKEL_YOUNG_WOMAN */ { OBJECT_CNE, &gHylianYoungWomanSkel },
    /* ENHY_SKEL_OLD_MAN */ { OBJECT_BJI, &gHylianOldManSkel },
    /* ENHY_SKEL_WOMAN_3 */ { OBJECT_COB, &gHylianWoman3Skel },
};

typedef enum EnHyAnimationIndex {
    /*  0 */ ENHY_ANIM_0,
    /*  1 */ ENHY_ANIM_1,
    /*  2 */ ENHY_ANIM_2,
    /*  3 */ ENHY_ANIM_3,
    /*  4 */ ENHY_ANIM_4,
    /*  5 */ ENHY_ANIM_5,
    /*  6 */ ENHY_ANIM_6,
    /*  7 */ ENHY_ANIM_7,
    /*  8 */ ENHY_ANIM_8,
    /*  9 */ ENHY_ANIM_9,
    /* 10 */ ENHY_ANIM_10,
    /* 11 */ ENHY_ANIM_11,
    /* 12 */ ENHY_ANIM_12,
    /* 13 */ ENHY_ANIM_13,
    /* 14 */ ENHY_ANIM_14,
    /* 15 */ ENHY_ANIM_15,
    /* 16 */ ENHY_ANIM_16,
    /* 17 */ ENHY_ANIM_17,
    /* 18 */ ENHY_ANIM_18,
    /* 19 */ ENHY_ANIM_19,
    /* 20 */ ENHY_ANIM_20,
    /* 21 */ ENHY_ANIM_21,
    /* 22 */ ENHY_ANIM_22,
    /* 23 */ ENHY_ANIM_23,
    /* 24 */ ENHY_ANIM_24,
    /* 25 */ ENHY_ANIM_25,
    /* 26 */ ENHY_ANIM_26
} EnHyAnimationIndex;

static AnimationInfo animetbl[] = {
    /* ENHY_ANIM_0 */ { &gObjOsAnim_092C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_1 */ { &gObjOsAnim_0228, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_2 */ { &gObjOsAnim_4CF4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_3 */ { &gObjOsAnim_16EC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_4 */ { &gObjOsAnim_265C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_5 */ { &gObjOsAnim_42AC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_6 */ { &gObjOsAnim_28DC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_7 */ { &gObjOsAnim_2160, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    /* ENHY_ANIM_8 */ { &gObjOsAnim_265C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -10.0f },
    /* ENHY_ANIM_9 */ { &gObjOsAnim_4E90, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_10 */ { &gObjOsAnim_1E7C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_11 */ { &gObjOsAnim_0170, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_12 */ { &gObjOsAnim_00B4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_13 */ { &gObjOsAnim_3D84, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_14 */ { &gObjOsAnim_41F8, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_15 */ { &gObjOsAnim_300C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_16 */ { &gObjOsAnim_31B0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_17 */ { &gObjOsAnim_31B0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    /* ENHY_ANIM_18 */ { &gObjOsAnim_2D0C, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_19 */ { &gObjOsAnim_2DC0, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_20 */ { &gObjOsAnim_4408, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_21 */ { &gObjOsAnim_1F18, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_22 */ { &gObjOsAnim_4F28, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    /* ENHY_ANIM_23 */ { &gObjOsAnim_33B4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    /* ENHY_ANIM_24 */ { &gObjOsAnim_12E8, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    /* ENHY_ANIM_25 */ { &gObjOsAnim_0FE4, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
    /* ENHY_ANIM_26 */ { &gObjOsAnim_0BFC, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
};

typedef struct EnHyModelInfo {
    /* 0x0 */ u8 headInfoIndex;      // EnHyHeadIndex
    /* 0x1 */ u8 upperSkelInfoIndex; // EnHySkeletonIndex, see EnHy.objectSlotUpperSkel
    /* 0x2 */ Color_RGBA8 envColorSeg8;
    /* 0x6 */ u8 lowerSkelInfoIndex; // EnHySkeletonIndex, see EnHy.objectSlotLowerSkel
    /* 0x7 */ Color_RGBA8 envColorSeg9;
    /* 0xB */ u8 animInfoIndex; // EnHyAnimationIndex
} EnHyModelInfo;                // size = 0xC

static EnHyModelInfo npcdata[] = {
    /* ENHY_TYPE_DOG_LADY */
    {
        ENHY_HEAD_WOMAN_1,
        ENHY_SKEL_WOMAN_1,
        { 255, 255, 255, 255 },
        ENHY_SKEL_WOMAN_1,
        { 255, 255, 255, 255 },
        ENHY_ANIM_0,
    },
    /* ENHY_TYPE_WOMAN_3 */
    {
        ENHY_HEAD_WOMAN_3,
        ENHY_SKEL_WOMAN_3,
        { 255, 255, 255, 255 },
        ENHY_SKEL_WOMAN_3,
        { 255, 255, 255, 255 },
        ENHY_ANIM_22,
    },
    /* ENHY_TYPE_MAN_1_BEARD */
    {
        ENHY_HEAD_MAN_1_BEARD,
        ENHY_SKEL_MAN_1,
        { 255, 255, 255, 255 },
        ENHY_SKEL_MAN_1,
        { 255, 255, 255, 255 },
        ENHY_ANIM_1,
    },
    /* ENHY_TYPE_MAN_2_BALD */
    {
        ENHY_HEAD_MAN_2_BALD,
        ENHY_SKEL_MAN_2,
        { 255, 255, 255, 0 },
        ENHY_SKEL_MAN_2,
        { 55, 55, 255, 0 },
        ENHY_ANIM_15,
    },
    /* ENHY_TYPE_MAN_1_SHAVED_BLACK_SHIRT */
    {
        ENHY_HEAD_MAN_1_SHAVED,
        ENHY_SKEL_MAN_1,
        { 0, 0, 0, 0 },
        ENHY_SKEL_MAN_1,
        { 255, 0, 0, 0 },
        ENHY_ANIM_11,
    },
    /* ENHY_TYPE_BEGGAR */
    {
        ENHY_HEAD_MAN_2_LONG_HAIR,
        ENHY_SKEL_MAN_2,
        { 50, 80, 0, 0 },
        ENHY_SKEL_MAN_2,
        { 50, 80, 0, 0 },
        ENHY_ANIM_16,
    },
    /* ENHY_TYPE_OLD_WOMAN */
    {
        ENHY_HEAD_OLD_WOMAN,
        ENHY_SKEL_OLD_WOMAN,
        { 255, 255, 255, 255 },
        ENHY_SKEL_OLD_WOMAN,
        { 255, 255, 255, 255 },
        ENHY_ANIM_10,
    },
    /* ENHY_TYPE_OLD_MAN */
    {
        ENHY_HEAD_OLD_MAN,
        ENHY_SKEL_OLD_MAN,
        { 0, 50, 160, 0 },
        ENHY_SKEL_OLD_MAN,
        { 255, 255, 255, 0 },
        ENHY_ANIM_4,
    },
    /* ENHY_TYPE_YOUNG_WOMAN_BROWN_HAIR */
    {
        ENHY_HEAD_YOUNG_WOMAN_BROWN_HAIR,
        ENHY_SKEL_YOUNG_WOMAN,
        { 160, 180, 255, 0 },
        ENHY_SKEL_YOUNG_WOMAN,
        { 160, 180, 255, 0 },
        ENHY_ANIM_9,
    },
    /* ENHY_TYPE_MAN_2_MUSTACHE_RED_SHIRT */
    {
        ENHY_HEAD_MAN_2_MUSTACHE,
        ENHY_SKEL_MAN_2,
        { 220, 0, 80, 0 },
        ENHY_SKEL_MAN_2,
        { 255, 255, 255, 0 },
        ENHY_ANIM_13,
    },
    /* ENHY_TYPE_MAN_2_MUSTACHE_BLUE_SHIRT */
    {
        ENHY_HEAD_MAN_2_MUSTACHE,
        ENHY_SKEL_MAN_2,
        { 0, 130, 220, 0 },
        ENHY_SKEL_MAN_2,
        { 255, 255, 255, 0 },
        ENHY_ANIM_14,
    },
    /* ENHY_TYPE_YOUNG_WOMAN_ORANGE_HAIR */
    {
        ENHY_HEAD_YOUNG_WOMAN_ORANGE_HAIR,
        ENHY_SKEL_YOUNG_WOMAN,
        { 70, 160, 230, 0 },
        ENHY_SKEL_YOUNG_WOMAN,
        { 255, 255, 100, 0 },
        ENHY_ANIM_20,
    },
    /* ENHY_TYPE_MAN_2_ALT_MUSTACHE */
    {
        ENHY_HEAD_MAN_2_ALT_MUSTACHE,
        ENHY_SKEL_MAN_2,
        { 150, 60, 90, 0 },
        ENHY_SKEL_MAN_2,
        { 255, 240, 150, 0 },
        ENHY_ANIM_18,
    },
    /* ENHY_TYPE_MAN_1_BOWL_CUT_PURPLE_SHIRT */
    {
        ENHY_HEAD_MAN_1_BOWL_CUT,
        ENHY_SKEL_MAN_1,
        { 200, 180, 255, 0 },
        ENHY_SKEL_MAN_1,
        { 200, 180, 255, 0 },
        ENHY_ANIM_12,
    },
    /* ENHY_TYPE_MAN_2_BEARD */
    {
        ENHY_HEAD_MAN_2_BEARD,
        ENHY_SKEL_MAN_2,
        { 140, 255, 110, 0 },
        ENHY_SKEL_MAN_2,
        { 255, 255, 255, 0 },
        ENHY_ANIM_19,
    },
    /* ENHY_TYPE_OLD_MAN_BALD_BROWN_ROBE */
    {
        ENHY_HEAD_OLD_MAN_BALD,
        ENHY_SKEL_OLD_MAN,
        { 130, 70, 20, 0 },
        ENHY_SKEL_OLD_MAN,
        { 130, 180, 255, 0 },
        ENHY_ANIM_21,
    },
    /* ENHY_TYPE_MAN_2_MUSTACHE_WHITE_SHIRT */
    {
        ENHY_HEAD_MAN_2_MUSTACHE,
        ENHY_SKEL_MAN_2,
        { 255, 255, 255, 255 },
        ENHY_SKEL_MAN_2,
        { 255, 255, 255, 255 },
        ENHY_ANIM_5,
    },
    /* ENHY_TYPE_MAN_1_SHAVED_GREEN_SHIRT */
    {
        ENHY_HEAD_MAN_1_SHAVED,
        ENHY_SKEL_MAN_1,
        { 90, 100, 20, 255 },
        ENHY_SKEL_MAN_1,
        { 100, 140, 50, 255 },
        ENHY_ANIM_11,
    },
    /* ENHY_TYPE_WOMAN_2 */
    {
        ENHY_HEAD_WOMAN_2,
        ENHY_SKEL_WOMAN_2,
        { 255, 255, 255, 255 },
        ENHY_SKEL_WOMAN_2,
        { 255, 255, 255, 255 },
        ENHY_ANIM_6,
    },
    /* ENHY_TYPE_OLD_MAN_BALD_PURPLE_ROBE */
    {
        ENHY_HEAD_OLD_MAN_BALD,
        ENHY_SKEL_OLD_MAN,
        { 160, 0, 100, 0 },
        ENHY_SKEL_OLD_MAN,
        { 70, 130, 210, 0 },
        ENHY_ANIM_21,
    },
    /* ENHY_TYPE_MAN_1_BOWL_CUT_GREEN_SHIRT */
    {
        ENHY_HEAD_MAN_1_BOWL_CUT,
        ENHY_SKEL_MAN_1,
        { 160, 230, 0, 0 },
        ENHY_SKEL_MAN_1,
        { 0, 150, 110, 0 },
        ENHY_ANIM_12,
    },
};

typedef struct EnHyColliderInfo {
    /* 0x0 */ Vec3s offset;
    /* 0x6 */ s16 radius;
    /* 0x8 */ s16 height;
} EnHyColliderInfo; // size 0xA

static EnHyColliderInfo collision_ct_data[] = {
    /* ENHY_TYPE_DOG_LADY */ { { 0, 0, 4 }, 24, 70 },
    /* ENHY_TYPE_WOMAN_3 */ { { 0, 0, 8 }, 28, 62 },
    /* ENHY_TYPE_MAN_1_BEARD */ { { 0, 0, 4 }, 20, 60 },
    /* ENHY_TYPE_MAN_2_BALD */ { { 0, 0, 2 }, 20, 60 },
    /* ENHY_TYPE_MAN_1_SHAVED_BLACK_SHIRT */ { { 0, 0, -2 }, 20, 60 },
    /* ENHY_TYPE_BEGGAR */ { { 0, 0, 8 }, 24, 40 },
    /* ENHY_TYPE_OLD_WOMAN */ { { 0, 0, 10 }, 26, 40 },
    /* ENHY_TYPE_OLD_MAN */ { { 0, 0, 12 }, 26, 58 },
    /* ENHY_TYPE_YOUNG_WOMAN_BROWN_HAIR */ { { 0, 0, 2 }, 18, 68 },
    /* ENHY_TYPE_MAN_2_MUSTACHE_RED_SHIRT */ { { 0, 0, 4 }, 20, 60 },
    /* ENHY_TYPE_MAN_2_MUSTACHE_BLUE_SHIRT */ { { 0, 0, 4 }, 20, 60 },
    /* ENHY_TYPE_YOUNG_WOMAN_ORANGE_HAIR */ { { 0, 0, 6 }, 20, 64 },
    /* ENHY_TYPE_MAN_2_ALT_MUSTACHE */ { { 0, 0, 0 }, 18, 60 },
    /* ENHY_TYPE_MAN_1_BOWL_CUT_PURPLE_SHIRT */ { { 0, 0, 0 }, 16, 60 },
    /* ENHY_TYPE_MAN_2_BEARD */ { { 0, 0, 0 }, 16, 64 },
    /* ENHY_TYPE_OLD_MAN_BALD_BROWN_ROBE */ { { 0, 0, 8 }, 20, 58 },
    /* ENHY_TYPE_MAN_2_MUSTACHE_WHITE_SHIRT */ { { 4, 0, 0 }, 18, 62 },
    /* ENHY_TYPE_MAN_1_SHAVED_GREEN_SHIRT */ { { 4, 0, 0 }, 18, 62 },
    /* ENHY_TYPE_WOMAN_2 */ { { 0, 0, 8 }, 28, 62 },
    /* ENHY_TYPE_OLD_MAN_BALD_PURPLE_ROBE */ { { 0, 0, 0 }, 16, 60 },
    /* ENHY_TYPE_MAN_1_BOWL_CUT_GREEN_SHIRT */ { { 0, 0, 8 }, 20, 58 },
};

typedef struct EnHyPlayerTrackingInfo {
    /* 0x00 */ u8 presetIndex;
    /* 0x04 */ f32 childYOffset;
    /* 0x08 */ f32 adultYOffset;
} EnHyPlayerTrackingInfo; // size = 0xC

static EnHyPlayerTrackingInfo eye_move_data[] = {
    /* ENHY_TYPE_DOG_LADY */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_WOMAN_3 */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_MAN_1_BEARD */ { 7, 40.0f, 20.0f },
    /* ENHY_TYPE_MAN_2_BALD */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_MAN_1_SHAVED_BLACK_SHIRT */ { 7, 40.0f, 20.0f },
    /* ENHY_TYPE_BEGGAR */ { 8, 0.0f, -20.0f },
    /* ENHY_TYPE_OLD_WOMAN */ { 9, 20.0f, 0.0f },
    /* ENHY_TYPE_OLD_MAN */ { 9, 20.0f, 0.0f },
    /* ENHY_TYPE_YOUNG_WOMAN_BROWN_HAIR */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_MAN_2_MUSTACHE_RED_SHIRT */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_MAN_2_MUSTACHE_BLUE_SHIRT */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_YOUNG_WOMAN_ORANGE_HAIR */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_MAN_2_ALT_MUSTACHE */ { 0, 0.0f, 0.0f },
    /* ENHY_TYPE_MAN_1_BOWL_CUT_PURPLE_SHIRT */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_MAN_2_BEARD */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_OLD_MAN_BALD_BROWN_ROBE */ { 10, 20.0f, 0.0f },
    /* ENHY_TYPE_MAN_2_MUSTACHE_WHITE_SHIRT */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_MAN_1_SHAVED_GREEN_SHIRT */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_WOMAN_2 */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_OLD_MAN_BALD_PURPLE_ROBE */ { 6, 20.0f, 10.0f },
    /* ENHY_TYPE_MAN_1_BOWL_CUT_GREEN_SHIRT */ { 10, 20.0f, 0.0f },
};

typedef struct EnHyInit2Info {
    /* 0x00 */ f32 shadowScale;
    /* 0x04 */ Vec3f modelOffset;
    /* 0x10 */ f32 scale;
    /* 0x14 */ s8 attentionRangeType;
    /* 0x18 */ f32 interactRange;
} EnHyInit2Info; // size = 0x1C

static EnHyInit2Info actor_ct_data[] = {
    /* ENHY_TYPE_DOG_LADY */ { 36.0f, { 0.0f, 0.0f, 600.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_WOMAN_3 */ { 40.0f, { -100.0f, 0.0f, 400.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_MAN_1_BEARD */ { 22.0f, { 0.0f, 0.0f, -200.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_MAN_2_BALD */ { 20.0f, { -100.0f, 0.0f, 0.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_MAN_1_SHAVED_BLACK_SHIRT */ { 22.0f, { 0.0f, 0.0f, 0.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_BEGGAR */ { 21.0f, { 0.0f, 0.0f, 0.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_OLD_WOMAN */ { 25.0f, { -100.0f, 0.0f, 600.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_OLD_MAN */ { 28.0f, { -100.0f, 0.0f, 800.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_YOUNG_WOMAN_BROWN_HAIR */ { 17.0f, { 0.0f, 0.0f, 700.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_MAN_2_MUSTACHE_RED_SHIRT */ { 18.0f, { 0.0f, 0.0f, 100.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_MAN_2_MUSTACHE_BLUE_SHIRT */ { 18.0f, { 0.0f, 0.0f, -200.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_YOUNG_WOMAN_ORANGE_HAIR */ { 17.0f, { 0.0f, 0.0f, 700.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_MAN_2_ALT_MUSTACHE */ { 21.0f, { 0.0f, 0.0f, -300.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_MAN_1_BOWL_CUT_PURPLE_SHIRT */ { 20.0f, { 0.0f, 0.0f, -200.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_MAN_2_BEARD */ { 18.0f, { -200.0f, 0.0f, -200.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_OLD_MAN_BALD_BROWN_ROBE */ { 27.0f, { -100.0f, 0.0f, 800.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_MAN_2_MUSTACHE_WHITE_SHIRT */ { 19.0f, { 400.0f, 0.0f, 0.0f }, 0.01f, 4, 30.0f },
    /* ENHY_TYPE_MAN_1_SHAVED_GREEN_SHIRT */ { 19.0f, { 400.0f, 0.0f, 0.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_WOMAN_2 */ { 40.0f, { -100.0f, 0.0f, 400.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_OLD_MAN_BALD_PURPLE_ROBE */ { 17.0f, { 0.0f, 0.0f, 700.0f }, 0.01f, 6, 30.0f },
    /* ENHY_TYPE_MAN_1_BOWL_CUT_GREEN_SHIRT */ { 20.0f, { 0.0f, 0.0f, -200.0f }, 0.01f, 6, 30.0f },
};

static s32 get_bank_id(EnHy* this, PlayState* play) {
    u8 headInfoIndex = npcdata[ENHY_GET_TYPE(&this->actor)].headInfoIndex;
    u8 upperSkelInfoIndex = npcdata[ENHY_GET_TYPE(&this->actor)].upperSkelInfoIndex;
    u8 lowerSkelInfoIndex = npcdata[ENHY_GET_TYPE(&this->actor)].lowerSkelInfoIndex;

    this->objectSlotLowerSkel = Object_Exchange_bank_check(&play->objectCtx, body[lowerSkelInfoIndex].objectId);
    if (this->objectSlotLowerSkel < 0) {
        return false;
    }

    this->objectSlotUpperSkel = Object_Exchange_bank_check(&play->objectCtx, body[upperSkelInfoIndex].objectId);
    if (this->objectSlotUpperSkel < 0) {
        return false;
    }

    this->objectSlotHead = Object_Exchange_bank_check(&play->objectCtx, head[headInfoIndex].objectId);
    if (this->objectSlotHead < 0) {
        return false;
    }

    return true;
}

static s32 bank_trans_check(EnHy* this, PlayState* play) {
    if (!Object_Exchange_bank_dma_check(&play->objectCtx, this->objectSlotLowerSkel)) {
        return false;
    }

    if (!Object_Exchange_bank_dma_check(&play->objectCtx, this->objectSlotUpperSkel)) {
        return false;
    }

    if (!Object_Exchange_bank_dma_check(&play->objectCtx, this->objectSlotHead)) {
        return false;
    }

    return true;
}

static s32 get_animation_bank_id(EnHy* this, PlayState* play) {
    this->objectSlotOsAnime = Object_Exchange_bank_check(&play->objectCtx, OBJECT_OS_ANIME);

    if (this->objectSlotOsAnime < 0) {
        return false;
    }

    return true;
}

static s32 animation_bank_trans_check(EnHy* this, PlayState* play) {
    if (!Object_Exchange_bank_dma_check(&play->objectCtx, this->objectSlotOsAnime)) {
        return false;
    }

    return true;
}

static void carry_request_set(EnHy* this, PlayState* play, s32 getItemId) {
    this->getItemId = getItemId;
    Actor_carry_request_set2(&this->actor, play, getItemId, this->actor.xzDistToPlayer + 1.0f,
                       fabsf(this->actor.yDistToPlayer) + 1.0f);
}

u16 hy_set_message(PlayState* play, Actor* thisx) {
    Player* player = GET_PLAYER(play);
    EnHy* this = (EnHy*)thisx;
    u16 textId = get_mask_message(play, ENHY_GET_TYPE(&this->actor) + MASK_REACTION_SET_DOG_LADY);

    if (textId != 0) {
        if (ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_BEGGAR) {
            player->exchangeItemId = EXCH_ITEM_BOTTLE_BLUE_FIRE;
        }
        return textId;
    }

    switch (ENHY_GET_TYPE(&this->actor)) {
        case ENHY_TYPE_DOG_LADY:
            if (play->sceneId == SCENE_KAKARIKO_CENTER_GUEST_HOUSE) {
                return (this->talonEventChkInf & EVENTCHKINF_MASK(EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO))
                           ? 0x508D
                           : (GET_INFTABLE(INFTABLE_CB) ? 0x508C : 0x508B);
            } else if (play->sceneId == SCENE_MARKET_DAY) {
                return GET_EVENTINF(EVENTINF_30) ? 0x709B : 0x709C;
            } else if (z_common_data.dogIsLost) {
                s16 followingDog = (z_common_data.dogParams & 0xF00) >> 8;

                if (followingDog != 0) {
                    this->playedSfx = false;
                    return (followingDog == 1) ? 0x709F : 0x709E;
                } else {
                    return 0x709D;
                }
            } else {
                return 0x70A0;
            }

        case ENHY_TYPE_WOMAN_3:
            if (GET_EVENTCHKINF(EVENTCHKINF_80)) {
                return GET_INFTABLE(INFTABLE_C1) ? 0x7017 : 0x7045;
            } else {
                return GET_INFTABLE(INFTABLE_C0) ? 0x7017 : 0x7016;
            }

        case ENHY_TYPE_MAN_1_BEARD:
            if (play->sceneId == SCENE_KAKARIKO_CENTER_GUEST_HOUSE) {
                return 0x5086;
            } else if (play->sceneId == SCENE_KAKARIKO_VILLAGE) {
                return 0x5085;
            } else if (GET_EVENTCHKINF(EVENTCHKINF_80)) {
                return GET_INFTABLE(INFTABLE_C3) ? 0x701A : 0x7047;
            } else if (GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                return 0x701A;
            } else if (GET_EVENTCHKINF(EVENTCHKINF_TALKED_TO_MALON_FIRST_TIME)) {
                return 0x701B;
            } else if (GET_INFTABLE(INFTABLE_C2)) {
                return 0x701C;
            } else {
                return 0x701A;
            }

        case ENHY_TYPE_MAN_2_BALD:
            return GET_EVENTCHKINF(EVENTCHKINF_80) ? (GET_INFTABLE(INFTABLE_C4) ? 0x7001 : 0x70EB) : 0x7001;

        case ENHY_TYPE_MAN_1_SHAVED_BLACK_SHIRT:
            return GET_EVENTCHKINF(EVENTCHKINF_80) ? 0x704B : (GET_INFTABLE(INFTABLE_C5) ? 0x7024 : 0x7023);

        case ENHY_TYPE_BEGGAR:
            player->exchangeItemId = EXCH_ITEM_BOTTLE_BLUE_FIRE;
            return 0x700C;

        case ENHY_TYPE_OLD_WOMAN:
            return GET_EVENTCHKINF(EVENTCHKINF_80) ? 0x704A : (GET_INFTABLE(INFTABLE_C6) ? 0x7022 : 0x7021);

        case ENHY_TYPE_OLD_MAN:
            if (play->sceneId == SCENE_KAKARIKO_CENTER_GUEST_HOUSE) {
                return 0x5088;
            } else if (play->sceneId == SCENE_KAKARIKO_VILLAGE) {
                return 0x5087;
            } else {
                return GET_EVENTCHKINF(EVENTCHKINF_80) ? 0x704D : (GET_INFTABLE(INFTABLE_C7) ? 0x7028 : 0x7027);
            }

        case ENHY_TYPE_YOUNG_WOMAN_BROWN_HAIR:
            if (GET_EVENTCHKINF(EVENTCHKINF_80)) {
                return GET_INFTABLE(INFTABLE_C9) ? 0x701E : 0x7048;
            } else {
                return GET_INFTABLE(INFTABLE_C8) ? 0x701E : 0x701D;
            }

        case ENHY_TYPE_MAN_2_MUSTACHE_RED_SHIRT:
            if (play->sceneId == SCENE_KAKARIKO_CENTER_GUEST_HOUSE) {
                return GET_EVENTCHKINF(EVENTCHKINF_AA) ? 0x5082 : 0x5081;
            } else if (play->sceneId == SCENE_KAKARIKO_VILLAGE) {
                return CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) ? 0x5080 : 0x507F;
            } else {
                return GET_EVENTCHKINF(EVENTCHKINF_80) ? 0x7049 : (GET_INFTABLE(INFTABLE_CA) ? 0x7020 : 0x701F);
            }

        case ENHY_TYPE_MAN_2_MUSTACHE_BLUE_SHIRT:
            if (play->sceneId == SCENE_IMPAS_HOUSE) {
                return GET_EVENTCHKINF(EVENTCHKINF_AA) ? 0x507E : 0x507D;
            } else if (play->sceneId == SCENE_KAKARIKO_VILLAGE) {
                return CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) ? 0x507C : 0x507B;
            } else {
#if OOT_VERSION < NTSC_1_1
                return GET_EVENTCHKINF(EVENTCHKINF_80) ? 0x7046 : (GET_INFTABLE(INFTABLE_CB) ? 0x7019 : 0x7018);
#else
                return GET_EVENTCHKINF(EVENTCHKINF_80) ? 0x7046 : (GET_INFTABLE(INFTABLE_CD) ? 0x7019 : 0x7018);
#endif
            }

        case ENHY_TYPE_YOUNG_WOMAN_ORANGE_HAIR:
            return GET_INFTABLE(INFTABLE_MALON_SPAWNED_AT_HYRULE_CASTLE) ? (GET_INFTABLE(INFTABLE_CC) ? 0x7014 : 0x70A4)
                                                                         : 0x7014;

        case ENHY_TYPE_MAN_2_ALT_MUSTACHE:
            if (play->sceneId == SCENE_KAKARIKO_VILLAGE) {
                return !IS_DAY ? 0x5084 : 0x5083;
            } else {
                return GET_EVENTCHKINF(EVENTCHKINF_80) ? 0x7044 : 0x7015;
            }

        case ENHY_TYPE_MAN_1_BOWL_CUT_PURPLE_SHIRT:
            return 0x7055;

        case ENHY_TYPE_MAN_2_BEARD:
            return 0x7089;

        case ENHY_TYPE_OLD_MAN_BALD_BROWN_ROBE:
            return 0x708A;

        case ENHY_TYPE_MAN_2_MUSTACHE_WHITE_SHIRT:
            return 0x700E;

        case ENHY_TYPE_MAN_1_SHAVED_GREEN_SHIRT:
            if (!LINK_IS_ADULT) {
                if (IS_DAY) {
                    return GET_INFTABLE(INFTABLE_160) ? 0x5058 : 0x5057;
                } else {
                    return GET_INFTABLE(INFTABLE_161) ? 0x505A : 0x5059;
                }
            } else if (IS_DAY) {
                return GET_INFTABLE(INFTABLE_162) ? 0x505C : 0x505B;
            } else {
                return 0x5058;
            }

        case ENHY_TYPE_WOMAN_2:
            if (!LINK_IS_ADULT) {
                return GET_EVENTCHKINF(EVENTCHKINF_80) ? 0x505F : (GET_INFTABLE(INFTABLE_163) ? 0x505E : 0x505D);
            } else {
                return (this->talonEventChkInf & EVENTCHKINF_MASK(EVENTCHKINF_TALON_RETURNED_FROM_KAKARIKO))
                           ? 0x5062
                           : (GET_INFTABLE(INFTABLE_164) ? 0x5061 : 0x5060);
            }

        case ENHY_TYPE_OLD_MAN_BALD_PURPLE_ROBE:
            return 0x7120;

        case ENHY_TYPE_MAN_1_BOWL_CUT_GREEN_SHIRT:
            return 0x7121;

        default:
            return 0;
    }
}

s16 hy_end_message(PlayState* play, Actor* thisx) {
    EnHy* this = (EnHy*)thisx;
    s16 beggarItems[] = { ITEM_BOTTLE_BLUE_FIRE, ITEM_BOTTLE_FISH, ITEM_BOTTLE_BUG, ITEM_BOTTLE_FAIRY };
    s16 beggarRewards[] = { 150, 100, 50, 25 };

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            return NPC_TALK_STATE_TALKING;

        case TEXT_STATE_DONE_FADING:
            switch (this->actor.textId) {
                case 0x709E:
                case 0x709F:
                    if (!this->playedSfx) {
                        Nai_FxFlagEntry(this->actor.textId == 0x709F ? NA_SE_SY_CORRECT_CHIME : NA_SE_SY_ERROR,
                                             &_dummy_zero_f, 4, &_dummy_one,
                                             &_dummy_one, &_dummy_zero_s8);
                        this->playedSfx = true;
                    }
                    break;

                case 0x70F0:
                case 0x70F1:
                case 0x70F2:
                case 0x70F3:
                    if (this->skelAnime.animation != &gObjOsAnim_33B4) {
                        npc_anime_ct(&this->skelAnime, animetbl, ENHY_ANIM_23);
                        Na_StartFanfare(NA_BGM_ITEM_GET | 0x900);
                    }
                    break;
            }
            return NPC_TALK_STATE_TALKING;

        case TEXT_STATE_CLOSING:
            switch (this->actor.textId) {
                case 0x70F0:
                case 0x70F1:
                case 0x70F2:
                case 0x70F3:
                    lupy_increase(beggarRewards[this->actor.textId - 0x70F0]);
                    npc_anime_ct(&this->skelAnime, animetbl, ENHY_ANIM_17);
                    bottle_item_change(play, GET_PLAYER(play), ITEM_BOTTLE_EMPTY, PLAYER_IA_BOTTLE);
                    break;

                case 0x7016:
                    SET_INFTABLE(INFTABLE_C0);
                    break;

                case 0x7045:
                    SET_INFTABLE(INFTABLE_C1);
                    break;

                case 0x701B:
                    SET_INFTABLE(INFTABLE_C2);
                    break;

                case 0x7047:
                    SET_INFTABLE(INFTABLE_C3);
                    break;

                case 0x70EB:
                    SET_INFTABLE(INFTABLE_C4);
                    break;

                case 0x7023:
                    SET_INFTABLE(INFTABLE_C5);
                    break;

                case 0x7021:
                    SET_INFTABLE(INFTABLE_C6);
                    break;

                case 0x7027:
                    SET_INFTABLE(INFTABLE_C7);
                    break;

                case 0x701D:
                    SET_INFTABLE(INFTABLE_C8);
                    break;

                case 0x7048:
                    SET_INFTABLE(INFTABLE_C9);
                    break;

                case 0x701F:
                    SET_INFTABLE(INFTABLE_CA);
                    break;

                case 0x7018:
#if OOT_VERSION < NTSC_1_1
                    SET_INFTABLE(INFTABLE_CB);
#else
                    SET_INFTABLE(INFTABLE_CD);
#endif
                    break;

                case 0x70A4:
                    SET_INFTABLE(INFTABLE_CC);
                    break;

                case 0x5057:
                    SET_INFTABLE(INFTABLE_160);
                    break;

                case 0x5059:
                    SET_INFTABLE(INFTABLE_161);
                    break;

                case 0x505B:
                    SET_INFTABLE(INFTABLE_162);
                    break;

                case 0x505D:
                    SET_INFTABLE(INFTABLE_163);
                    break;

                case 0x5060:
                    SET_INFTABLE(INFTABLE_164);
                    break;

                case 0x508B:
                    SET_INFTABLE(INFTABLE_CB);
                    break;

                case 0x709E:
                    z_common_data.dogParams = 0;
                    break;

                case 0x709F:
                    carry_request_set(this, play, GET_INFTABLE(INFTABLE_191) ? GI_RUPEE_BLUE : GI_HEART_PIECE);
                    this->actionFunc = carry_request;
                    break;
            }
            return NPC_TALK_STATE_IDLE;

        case TEXT_STATE_EVENT:
            if (!pad_on_check(play)) {
                return NPC_TALK_STATE_TALKING;
            } else {
                return NPC_TALK_STATE_ACTION;
            }

        default:
            return NPC_TALK_STATE_TALKING;
    }
}

void hy_eye_paci2(EnHy* this) {
    if (DECR(this->nextEyeIndexTimer) == 0) {
        u8 headInfoIndex = npcdata[ENHY_GET_TYPE(&this->actor)].headInfoIndex;

        this->curEyeIndex++;
        if ((head[headInfoIndex].eyeTextures != NULL) &&
            (head[headInfoIndex].eyeTextures[this->curEyeIndex] == NULL)) {
            this->nextEyeIndexTimer = get_random_timer(30, 30);
            this->curEyeIndex = 0;
        }
    }
}

static void separate_collision_data_set(EnHy* this) {
    u8 type = ENHY_GET_TYPE(&this->actor);

    this->collider.dim.radius = collision_ct_data[type].radius;
    this->collider.dim.height = collision_ct_data[type].height;
}

static void separate_actor_data_set(EnHy* this) {
    u8 type = ENHY_GET_TYPE(&this->actor);

    this->actor.shape.shadowScale = actor_ct_data[type].shadowScale;
    Actor_set_scale(&this->actor, actor_ct_data[type].scale);
    this->actor.attentionRangeType = actor_ct_data[type].attentionRangeType;
    this->modelOffset = actor_ct_data[type].modelOffset;
    this->interactRange = actor_ct_data[type].interactRange;
    this->interactRange += this->collider.dim.radius;
}

static void set_collision(EnHy* this, PlayState* play) {
    Vec3s pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y;
    pos.z = this->actor.world.pos.z;
    pos.x += collision_ct_data[ENHY_GET_TYPE(&this->actor)].offset.x;
    pos.y += collision_ct_data[ENHY_GET_TYPE(&this->actor)].offset.y;
    pos.z += collision_ct_data[ENHY_GET_TYPE(&this->actor)].offset.z;
    this->collider.dim.pos = pos;
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void NPC_009_special(EnHy* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_BEGGAR) {
        if (!bottle_interior_check(ITEM_BOTTLE_BLUE_FIRE) && !bottle_interior_check(ITEM_BOTTLE_BUG) &&
            !bottle_interior_check(ITEM_BOTTLE_FISH)) {
            switch (Actor_get_item_check(play)) {
                case EXCH_ITEM_BOTTLE_POE:
                case EXCH_ITEM_BOTTLE_BIG_POE:
                case EXCH_ITEM_BOTTLE_RUTOS_LETTER:
                    this->actor.textId = 0x70EF;
                    break;

                default:
                    if (mask_check(play) == PLAYER_MASK_NONE) {
                        this->actor.textId = 0x70ED;
                    }
                    break;
            }
        } else {
            switch (Actor_get_item_check(play)) {
                case EXCH_ITEM_BOTTLE_BLUE_FIRE:
                    this->actor.textId = 0x70F0;
                    break;

                case EXCH_ITEM_BOTTLE_FISH:
                    this->actor.textId = 0x70F1;
                    break;

                case EXCH_ITEM_BOTTLE_BUG:
                    this->actor.textId = 0x70F2;
                    break;

                default:
                    if (mask_check(play) == PLAYER_MASK_NONE) {
                        this->actor.textId = 0x700C;
                    }
                    break;
            }
        }

        player->actor.textId = this->actor.textId;
    }
}

void hy_speak(EnHy* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode;

    switch (ENHY_GET_TYPE(&this->actor)) {
        case ENHY_TYPE_MAN_2_BALD:
        case ENHY_TYPE_OLD_MAN:
        case ENHY_TYPE_MAN_2_MUSTACHE_RED_SHIRT:
        case ENHY_TYPE_MAN_2_MUSTACHE_BLUE_SHIRT:
            trackingMode =
                (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) ? NPC_TRACKING_NONE : NPC_TRACKING_HEAD_AND_TORSO;
            break;

        case ENHY_TYPE_MAN_2_ALT_MUSTACHE:
            trackingMode = NPC_TRACKING_NONE;
            break;

        case ENHY_TYPE_MAN_1_BEARD:
        case ENHY_TYPE_MAN_1_SHAVED_GREEN_SHIRT:
            trackingMode = NPC_TRACKING_FULL_BODY;
            break;

        case ENHY_TYPE_DOG_LADY:
        case ENHY_TYPE_WOMAN_2:
            trackingMode = (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) ? NPC_TRACKING_HEAD_AND_TORSO
                                                                                 : NPC_TRACKING_FULL_BODY;
            break;

        default:
            trackingMode = NPC_TRACKING_HEAD_AND_TORSO;
            break;
    }

    this->interactInfo.trackPos = player->actor.world.pos;

    if (LINK_IS_ADULT) {
        this->interactInfo.yOffset = eye_move_data[ENHY_GET_TYPE(&this->actor)].adultYOffset;
    } else {
        this->interactInfo.yOffset = eye_move_data[ENHY_GET_TYPE(&this->actor)].childYOffset;
    }

    eye_moveM(&this->actor, &this->interactInfo, eye_move_data[ENHY_GET_TYPE(&this->actor)].presetIndex,
                   trackingMode);

    if (npc_talk(play, &this->actor, &this->interactInfo.talkState, this->interactRange, hy_set_message,
                          hy_end_message)) {
        NPC_009_special(this, play);
    }
}

static s32 appearance_check(EnHy* this, PlayState* play) {
    switch (play->sceneId) {
        case SCENE_KAKARIKO_VILLAGE:
            if (!(ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_MAN_2_MUSTACHE_RED_SHIRT ||
                  ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_MAN_2_MUSTACHE_BLUE_SHIRT ||
                  ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_MAN_2_ALT_MUSTACHE ||
                  ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_MAN_1_BEARD ||
                  ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_OLD_MAN)) {
                return true;
            } else if (!LINK_IS_ADULT) {
                return true;
            } else if (ENHY_GET_TYPE(&this->actor) != ENHY_TYPE_MAN_2_ALT_MUSTACHE && IS_NIGHT) {
                return false;
            } else {
                return true;
            }

        case SCENE_IMPAS_HOUSE:
            if (ENHY_GET_TYPE(&this->actor) != ENHY_TYPE_MAN_2_MUSTACHE_BLUE_SHIRT) {
                return true;
            } else if (LINK_IS_CHILD) {
                return false;
            } else if (ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_MAN_2_MUSTACHE_BLUE_SHIRT && IS_DAY) {
                return false;
            } else {
                return true;
            }

        case SCENE_DOG_LADY_HOUSE:
            if (ENHY_GET_TYPE(&this->actor) != ENHY_TYPE_DOG_LADY) {
                return true;
            } else if (IS_DAY) {
                return false;
            } else {
                return true;
            }
        case SCENE_KAKARIKO_CENTER_GUEST_HOUSE:
            if (ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_DOG_LADY) {
                return !LINK_IS_ADULT ? false : true;
            } else if (!(ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_MAN_2_MUSTACHE_RED_SHIRT ||
                         ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_MAN_1_BEARD ||
                         ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_OLD_MAN)) {
                return true;
            } else if (IS_DAY) {
                return false;
            } else if (LINK_IS_CHILD) {
                return false;
            } else {
                return true;
            }

        case SCENE_BACK_ALLEY_DAY:
        case SCENE_BACK_ALLEY_NIGHT:
            if (ENHY_GET_TYPE(&this->actor) != ENHY_TYPE_MAN_2_BEARD) {
                return true;
            } else if (IS_NIGHT) {
                return false;
            } else if (GET_EVENTCHKINF(EVENTCHKINF_80) && !GET_EVENTCHKINF(EVENTCHKINF_45)) {
                return false;
            } else {
                return true;
            }

        default:
            switch (ENHY_GET_TYPE(&this->actor)) {
                case ENHY_TYPE_OLD_MAN_BALD_PURPLE_ROBE:
                case ENHY_TYPE_MAN_1_BOWL_CUT_GREEN_SHIRT:
                    if (LINK_IS_ADULT) {
                        return false;
                    }
                    FALLTHROUGH;
                default:
                    return true;
            }
    }
}

void En_Hy_Actor_ct(Actor* thisx, PlayState* play) {
    EnHy* this = (EnHy*)thisx;

    if ((ENHY_GET_TYPE(&this->actor) >= ENHY_TYPE_MAX) || !get_animation_bank_id(this, play) ||
        !get_bank_id(this, play)) {
        Actor_delete(&this->actor);
    }

    if (!appearance_check(this, play)) {
        Actor_delete(&this->actor);
    }

    this->actionFunc = init;
}

void En_Hy_Actor_dt(Actor* thisx, PlayState* play) {
    EnHy* this = (EnHy*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void init(EnHy* this, PlayState* play) {
    if (animation_bank_trans_check(this, play) && bank_trans_check(this, play)) {
        this->actor.objectSlot = this->objectSlotLowerSkel;
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->actor.objectSlot].segment);
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime,
                           body[npcdata[ENHY_GET_TYPE(&this->actor)].lowerSkelInfoIndex].skeleton, NULL,
                           this->jointTable, this->morphTable, ENHY_LIMB_MAX);
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 0.0f);
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->objectSlotOsAnime].segment);
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &EnHyAtInfoData);
        separate_collision_data_set(this);
        CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &HyStatusData);
        npc_anime_ct(&this->skelAnime, animetbl, npcdata[ENHY_GET_TYPE(&this->actor)].animInfoIndex);

        if ((play->sceneId == SCENE_BACK_ALLEY_DAY) || (play->sceneId == SCENE_MARKET_DAY)) {
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->actor.cullingVolumeScale = 0.0f;
        }

        if (play->sceneId == SCENE_KAKARIKO_CENTER_GUEST_HOUSE) {
            this->talonEventChkInf = z_common_data.save.info.eventChkInf[EVENTCHKINF_INDEX_TALON_RETURNED_FROM_KAKARIKO];
        }

        separate_actor_data_set(this);
        this->path = get_path_data(play, ENHY_GET_PATH_INDEX(&this->actor), 15);

        switch (ENHY_GET_TYPE(&this->actor)) {
            case ENHY_TYPE_MAN_2_BALD:
                if (this->path != NULL) {
                    this->actor.speed = 3.0f;
                }
                this->actionFunc = boj_path_walk_nw;
                break;

            case ENHY_TYPE_OLD_MAN:
                this->pathReverse = false;
                this->actionFunc = bji_wait_pw;
                break;

            case ENHY_TYPE_DOG_LADY:
                if (play->sceneId == SCENE_MARKET_DAY) {
                    this->actionFunc = aob_wait;
                    break;
                }
                FALLTHROUGH;
            case ENHY_TYPE_WOMAN_3:
            case ENHY_TYPE_MAN_1_BEARD:
            case ENHY_TYPE_MAN_1_SHAVED_BLACK_SHIRT:
            case ENHY_TYPE_OLD_WOMAN:
            case ENHY_TYPE_YOUNG_WOMAN_BROWN_HAIR:
            case ENHY_TYPE_MAN_1_BOWL_CUT_PURPLE_SHIRT:
            case ENHY_TYPE_MAN_2_BEARD:
            case ENHY_TYPE_OLD_MAN_BALD_BROWN_ROBE:
            case ENHY_TYPE_MAN_2_MUSTACHE_WHITE_SHIRT:
            case ENHY_TYPE_MAN_1_SHAVED_GREEN_SHIRT:
            case ENHY_TYPE_WOMAN_2:
            case ENHY_TYPE_OLD_MAN_BALD_PURPLE_ROBE:
            case ENHY_TYPE_MAN_1_BOWL_CUT_GREEN_SHIRT:
                this->actionFunc = wait_pw;
                break;

            case ENHY_TYPE_BEGGAR:
            case ENHY_TYPE_MAN_2_MUSTACHE_RED_SHIRT:
            case ENHY_TYPE_MAN_2_MUSTACHE_BLUE_SHIRT:
            case ENHY_TYPE_YOUNG_WOMAN_ORANGE_HAIR:
            case ENHY_TYPE_MAN_2_ALT_MUSTACHE:
                this->actionFunc = wait_nw;
                break;

            default:
                Actor_delete(&this->actor);
                break;
        }
    }
}

void aob_wait(EnHy* this, PlayState* play) {
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        if (this->skelAnime.animation != &gObjOsAnim_0BFC) {
            npc_anime_ct(&this->skelAnime, animetbl, ENHY_ANIM_26);
        }
    } else if (GET_EVENTINF(EVENTINF_30)) {
        if (this->skelAnime.animation != &gObjOsAnim_0FE4) {
            npc_anime_ct(&this->skelAnime, animetbl, ENHY_ANIM_25);
        }
    } else if (this->skelAnime.animation != &gObjOsAnim_12E8) {
        npc_anime_ct(&this->skelAnime, animetbl, ENHY_ANIM_24);
    }
}

void boj_path_walk_nw(EnHy* this, PlayState* play) {
    s16 yaw;
    f32 distSq;

    distSq = path_move(&this->actor, this->path, this->waypoint, &yaw);
    add_calc_short_angle2(&this->actor.world.rot.y, yaw, 10, 1000, 1);
    this->actor.shape.rot = this->actor.world.rot;

    if ((distSq > 0.0f) && (distSq < 1000.0f)) {
        this->waypoint++;
        if (this->waypoint > (this->path->count - 1)) {
            this->waypoint = 0;
        }
    }
}

void wait_pw(EnHy* this, PlayState* play) {
    program_wait(play, this->fidgetTableY, this->fidgetTableZ, ENHY_LIMB_MAX);
}

static void wait_nw(EnHy* this, PlayState* play) {
}

void bji_wait_pw(EnHy* this, PlayState* play) {
    if ((this->actor.xzDistToPlayer <= 100.0f) && (this->path != NULL)) {
        npc_anime_ct(&this->skelAnime, animetbl, ENHY_ANIM_7);
        this->actor.speed = 0.4f;
        this->actionFunc = bji_path_walk_pw;
    }

    program_wait(play, this->fidgetTableY, this->fidgetTableZ, ENHY_LIMB_MAX);
}

void bji_path_walk_pw(EnHy* this, PlayState* play) {
    s16 yaw;
    f32 distSq;

    if ((this->skelAnime.animation == &gObjOsAnim_2160) && (this->interactInfo.talkState != NPC_TALK_STATE_IDLE)) {
        npc_anime_ct(&this->skelAnime, animetbl, ENHY_ANIM_8);
    }

    if ((this->skelAnime.animation == &gObjOsAnim_265C) && (this->interactInfo.talkState == NPC_TALK_STATE_IDLE)) {
        npc_anime_ct(&this->skelAnime, animetbl, ENHY_ANIM_7);
    }

    this->actor.speed = 0.4f;
    distSq = path_move(&this->actor, this->path, this->waypoint, &yaw);
    add_calc_short_angle2(&this->actor.world.rot.y, yaw, 10, 1000, 1);
    this->actor.shape.rot = this->actor.world.rot;

    if (!(distSq <= 0.0f) && !(distSq >= 1000.0f)) {
        if (!this->pathReverse) {
            this->waypoint++;
            if (this->waypoint > (this->path->count - 1)) {
                this->pathReverse = true;
                this->waypoint = this->path->count - 2;
            }
        } else {
            this->waypoint--;
            if (this->waypoint < 0) {
                this->pathReverse = false;
                this->waypoint = 1;
            }
        }
    }
}

void carry_request(EnHy* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actionFunc = carry_end;
    } else {
        Actor_carry_request_set2(&this->actor, play, this->getItemId, this->actor.xzDistToPlayer + 1.0f,
                           fabsf(this->actor.yDistToPlayer) + 1.0f);
    }
}

static void carry_end(EnHy* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        switch (this->getItemId) {
            case GI_HEART_PIECE:
                z_common_data.dogParams = 0;
                z_common_data.dogIsLost = false;
                SET_INFTABLE(INFTABLE_191);
                break;

            case GI_RUPEE_BLUE:
                lupy_increase(5);
                z_common_data.dogParams = 0;
                z_common_data.dogIsLost = false;
                break;
        }

        this->actionFunc = wait_pw;
    }
}

void En_Hy_Actor_move(Actor* thisx, PlayState* play) {
    EnHy* this = (EnHy*)thisx;

    if (this->actionFunc != init) {
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->objectSlotOsAnime].segment);
        Skeleton_Info2_anime_play(&this->skelAnime);
        hy_eye_paci2(this);

        if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
            Actor_position_moveF(&this->actor);
        }

        Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    }

    this->actionFunc(this, play);
    hy_speak(this, play);
    set_collision(this, play);
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHy* this = (EnHy*)thisx;
    s32 pad;
    Vec3s limbRot;
    u8 headInfoIndex;
    void* eyeTex;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_hy.c", 2170);

    if (limbIndex == ENHY_LIMB_HEAD) {
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->objectSlotHead].segment);
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->objectSlotHead].segment);
        headInfoIndex = npcdata[ENHY_GET_TYPE(&this->actor)].headInfoIndex;
        *dList = head[headInfoIndex].headDList;

        if (head[headInfoIndex].eyeTextures != NULL) {
            eyeTex = head[headInfoIndex].eyeTextures[this->curEyeIndex];
            gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(eyeTex));
        }

        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->objectSlotLowerSkel].segment);
    }

    if (limbIndex == ENHY_LIMB_HEAD) {
        Matrix_translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        limbRot = this->interactInfo.headRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == ENHY_LIMB_TORSO) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(-limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
    }

    if ((limbIndex == ENHY_LIMB_TORSO) || (limbIndex == ENHY_LIMB_LEFT_UPPER_ARM) ||
        (limbIndex == ENHY_LIMB_RIGHT_UPPER_ARM)) {
        rot->y += sin_s(this->fidgetTableY[limbIndex]) * FIDGET_AMPLITUDE;
        rot->z += cos_s(this->fidgetTableZ[limbIndex]) * FIDGET_AMPLITUDE;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_hy.c", 2228);

    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnHy* this = (EnHy*)thisx;
    s32 pad;
    Vec3f focusOffset = { 400.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_hy.c", 2255);

    if (limbIndex == ENHY_LIMB_RIGHT_FOOT) {
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->objectSlotUpperSkel].segment);
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->objectSlotUpperSkel].segment);
    }

    if (ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_MAN_2_BALD && limbIndex == ENHY_LIMB_TORSO) {
        gSPDisplayList(POLY_OPA_DISP++, gHylianMan2BagDL);
    }

    if (limbIndex == ENHY_LIMB_HEAD) {
        Matrix_Position(&focusOffset, &this->actor.focus.pos);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_hy.c", 2281);
}

static Gfx* pa(GraphicsContext* gfxCtx, u8 envR, u8 envG, u8 envB, u8 envA) {
    Gfx* gfx = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));

    gDPSetEnvColor(&gfx[0], envR, envG, envB, envA);
    gSPEndDisplayList(&gfx[1]);

    return gfx;
}

void En_Hy_Actor_draw(Actor* thisx, PlayState* play) {
    EnHy* this = (EnHy*)thisx;
    Color_RGBA8 envColorSeg8;
    Color_RGBA8 envColorSeg9;
    Color_RGBA8 envColorSeg10;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_hy.c", 2318);

    if (this->actionFunc != init) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        Matrix_translate(this->modelOffset.x, this->modelOffset.y, this->modelOffset.z, MTXMODE_APPLY);
        envColorSeg8 = npcdata[ENHY_GET_TYPE(&this->actor)].envColorSeg8;
        envColorSeg9 = npcdata[ENHY_GET_TYPE(&this->actor)].envColorSeg9;

        switch (ENHY_GET_TYPE(&this->actor)) {
            case ENHY_TYPE_MAN_1_BEARD:
            case ENHY_TYPE_MAN_2_BALD:
            case ENHY_TYPE_MAN_1_SHAVED_BLACK_SHIRT:
            case ENHY_TYPE_BEGGAR:
            case ENHY_TYPE_OLD_MAN:
            case ENHY_TYPE_YOUNG_WOMAN_BROWN_HAIR:
            case ENHY_TYPE_MAN_2_MUSTACHE_RED_SHIRT:
            case ENHY_TYPE_MAN_2_MUSTACHE_BLUE_SHIRT:
            case ENHY_TYPE_YOUNG_WOMAN_ORANGE_HAIR:
            case ENHY_TYPE_MAN_2_ALT_MUSTACHE:
            case ENHY_TYPE_MAN_1_BOWL_CUT_PURPLE_SHIRT:
            case ENHY_TYPE_MAN_2_BEARD:
            case ENHY_TYPE_OLD_MAN_BALD_BROWN_ROBE:
            case ENHY_TYPE_MAN_2_MUSTACHE_WHITE_SHIRT:
            case ENHY_TYPE_MAN_1_SHAVED_GREEN_SHIRT:
            case ENHY_TYPE_OLD_MAN_BALD_PURPLE_ROBE:
            case ENHY_TYPE_MAN_1_BOWL_CUT_GREEN_SHIRT:
                gSPSegment(POLY_OPA_DISP++, 0x08,
                           pa(play->state.gfxCtx, envColorSeg8.r, envColorSeg8.g, envColorSeg8.b,
                                            envColorSeg8.a));
                gSPSegment(POLY_OPA_DISP++, 0x09,
                           pa(play->state.gfxCtx, envColorSeg9.r, envColorSeg9.g, envColorSeg9.b,
                                            envColorSeg9.a));

                if (ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_YOUNG_WOMAN_BROWN_HAIR ||
                    ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_YOUNG_WOMAN_ORANGE_HAIR) {
                    if (ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_YOUNG_WOMAN_BROWN_HAIR) {
                        envColorSeg10 = envColorSeg8;
                    }
                    if (ENHY_GET_TYPE(&this->actor) == ENHY_TYPE_YOUNG_WOMAN_ORANGE_HAIR) {
                        envColorSeg10.r = envColorSeg10.g = envColorSeg10.b = 255;
                        envColorSeg10.a = 0;
                    }
                    gSPSegment(POLY_OPA_DISP++, 0x0A,
                               pa(play->state.gfxCtx, envColorSeg10.r, envColorSeg10.g, envColorSeg10.b,
                                                envColorSeg10.a));
                }
                break;

            // ENHY_TYPE_DOG_LADY
            // ENHY_TYPE_WOMAN_3
            // ENHY_TYPE_OLD_WOMAN
            // ENHY_TYPE_WOMAN_2
            default:
                break;
        }

        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              before_display, after_display, &this->actor);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_hy.c", 2388);
}
