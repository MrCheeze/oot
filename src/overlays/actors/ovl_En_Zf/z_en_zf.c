/*
 * File: z_en_zf.c
 * Overlay: ovl_En_Zf
 * Description: Lizalfos and Dinolfos
 */

#include "z_en_zf.h"
#include "assets/objects/object_zf/object_zf.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Zf_Actor_ct(Actor* thisx, PlayState* play);
void En_Zf_Actor_dt(Actor* thisx, PlayState* play);
void En_Zf_move(Actor* thisx, PlayState* play);
void En_Zf_display(Actor* thisx, PlayState* play);

s16 tbl_p_get(Vec3f* pos, s16 preferredIndex);
void En_Zf_Actor_mode_before_drop_init(EnZf* this);
void En_Zf_Actor_mode_before_drop(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_wait_init(EnZf* this);
void En_Zf_Actor_mode_wait(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_forward_init(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_forward(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_oi_jump_init(EnZf* this);
void En_Zf_Actor_mode_oi_jump(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_search_rot_init(EnZf* this);
void En_Zf_Actor_mode_search_rot(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_move_to_player_front_init(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_move_to_player_front(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_kiru_init(EnZf* this);
void En_Zf_Actor_mode_kiru(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_hirumu(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_back_jump_init(EnZf* this);
void En_Zf_Actor_mode_back_jump(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_paralyze(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_nige_start_init(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_nige_start(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_nige_wait(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_nige_move_init(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_nige_move(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_nige_end(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_damage(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_jump_move_init(EnZf* this);
void En_Zf_Actor_mode_jump_move(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_side_step_init(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_side_step(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_down_init(EnZf* this);
void En_Zf_Actor_mode_down(EnZf* this, PlayState* play);
void En_Zf_Actor_mode_side_step_init2(EnZf* this, f32 speedXZ);
static s32 Shot_def(PlayState* play, EnZf* this);
s32 Shot_def2(PlayState* play, EnZf* this);

#define PLATFORM_INDEX_DOWNSTAIRS_MIN 0
#define PLATFORM_INDEX_DOWNSTAIRS_INNER_MAX 5
#define PLATFORM_INDEX_DOWNSTAIRS_MAX 7
#define PLATFORM_INDEX_UPSTAIRS_MIN (PLATFORM_INDEX_DOWNSTAIRS_MAX + 1)
#define PLATFORM_INDEX_UPSTAIRS_INNER_MAX 15

/**
 * Array of platform positions in Dodongo's Cavern miniboss room.
 *  0 -  7 : Downstairs
 *  8 - 15 : Upstairs inner platforms
 * 16 - 23 : Upstairs outer platforms (including several points on the long thin one)
 */
static Vec3f g_tbl[] = {
    // Downstairs
    { 3560.0f, 100.0f, -1517.0f },
    { 3170.0f, 100.0f, -1767.0f },
    { 3165.0f, 100.0f, -2217.0f },
    { 3563.0f, 100.0f, -2437.0f },
    { 3946.0f, 100.0f, -2217.0f },
    { 3942.0f, 100.0f, -1765.0f },
    // Westmost downstairs two
    { 2861.0f, 100.0f, -2394.0f },
    { 2776.0f, 100.0f, -1987.0f },

    // Upstairs inner
    { 4527.0f, 531.0f, -1146.0f },
    { 4442.0f, 531.0f, -1405.0f },
    { 4170.0f, 531.0f, -1395.0f },
    { 4030.0f, 531.0f, -1162.0f },
    { 4010.0f, 531.0f, -883.0f },
    { 4270.0f, 531.0f, -810.0f },
    { 4520.0f, 531.0f, -880.0f },
    { 4260.0f, 531.0f, -1035.0f },

    // Upstairs outer
    { 4757.0f, 531.0f, -1146.0f },
    { 3850.0f, 531.0f, -883.0f },
    { 4380.0f, 531.0f, -690.0f },
    { 4197.0f, 531.0f, -646.0f },
    { 4070.0f, 531.0f, -1575.0f },
    { 3930.0f, 531.0f, -1705.0f },
    { 3780.0f, 531.0f, -1835.0f },
    { 3560.0f, 531.0f, -1985.0f },
};

// These seem to relate to the tagging in/out the minibosses do
static s16 move_flg_trg = 0;
static s16 move_flg = 1;

ActorProfile En_Zf_Profile = {
    /**/ ACTOR_EN_ZF,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_ZF,
    /**/ sizeof(EnZf),
    /**/ En_Zf_Actor_ct,
    /**/ En_Zf_Actor_dt,
    /**/ En_Zf_move,
    /**/ En_Zf_display,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
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
    { 20, 70, 0, { 0, 0, 0 } },
};

static ColliderQuadInit AtInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum EnZfDamageEffect {
    /* 0x0 */ ENZF_DMGEFF_NONE,
    /* 0x1 */ ENZF_DMGEFF_STUN,
    /* 0x6 */ ENZF_DMGEFF_IMMUNE = 6,       // Skips damage code, but also skips the top half of Update
    /* 0xD */ ENZF_DMGEFF_PROJECTILE = 0xD, // Projectiles that don't have another damageeffect
    /* 0xF */ ENZF_DMGEFF_ICE = 0xF
} EnZfDamageEffect;

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, ENZF_DMGEFF_STUN),
    /* Deku stick    */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, ENZF_DMGEFF_PROJECTILE),
    /* Explosive     */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(0, ENZF_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(0, ENZF_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, ENZF_DMGEFF_NONE),
    /* Master sword  */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Giant's Knife */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Fire arrow    */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Ice arrow     */ DMG_ENTRY(4, ENZF_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Unk arrow 1   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Unk arrow 2   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Unk arrow 3   */ DMG_ENTRY(2, ENZF_DMGEFF_PROJECTILE),
    /* Fire magic    */ DMG_ENTRY(0, ENZF_DMGEFF_IMMUNE),
    /* Ice magic     */ DMG_ENTRY(3, ENZF_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(0, ENZF_DMGEFF_IMMUNE),
    /* Shield        */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, ENZF_DMGEFF_NONE),
    /* Giant spin    */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Master spin   */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Kokiri jump   */ DMG_ENTRY(2, ENZF_DMGEFF_NONE),
    /* Giant jump    */ DMG_ENTRY(8, ENZF_DMGEFF_NONE),
    /* Master jump   */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Unknown 1     */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, ENZF_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, ENZF_DMGEFF_NONE),
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 15, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -3500, ICHAIN_STOP),
};

static AnimationHeader* j_tbl[] = { &gZfHopCrouchingAnim, &gZfHopLeapingAnim, &gZfHopLandingAnim };

static s32 tim; // Set to 0 and incremented in En_Zf_Actor_mode_nige_move, but not actually used

void En_Zf_actor_set_process(EnZf* this, EnZfActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

/**
 * Tests if it will still be on a floor after moving forwards a distance determined by dist, in the shape forward
 * direction. If `dist` is 0, it defaults to a dist depending on speed direction, and params.
 */
s32 hasi_chk(EnZf* this, PlayState* play, f32 dist) {
    s16 ret;
    s16 curBgCheckFlags;
    f32 sin;
    f32 cos;
    Vec3f curPos;

    if (dist == 0.0f) {
        dist = ((this->actor.speed >= 0.0f) ? 1.0f : -1.0f);
        dist = ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) ? dist * 45.0f : dist * 30.0f);
    }

    // Save currents to restore later
    curPos = this->actor.world.pos;
    curBgCheckFlags = this->actor.bgCheckFlags;

    sin = sin_s(this->actor.world.rot.y) * dist;
    cos = cos_s(this->actor.world.rot.y) * dist;

    this->actor.world.pos.x += sin;
    this->actor.world.pos.z += cos;

    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    this->actor.world.pos = curPos;
    ret = !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND);
    this->actor.bgCheckFlags = curBgCheckFlags;
    return ret;
}

/**
 * Supplementary floor test.
 */
s16 hasi_chk2(EnZf* this, PlayState* play, f32 dist) {
    s16 ret;
    s16 curBgCheckFlags;
    f32 sin;
    f32 cos;
    Vec3f curPos;

    if ((this->actor.speed != 0.0f) && hasi_chk(this, play, this->actor.speed)) {
        return true;
    }

    // Save currents to restore later
    curPos = this->actor.world.pos;
    curBgCheckFlags = this->actor.bgCheckFlags;

    sin = sin_s(this->actor.shape.rot.y) * dist;
    cos = cos_s(this->actor.shape.rot.y) * dist;

    this->actor.world.pos.x += sin;
    this->actor.world.pos.z += cos;

    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    this->actor.world.pos = curPos;
    ret = !(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND);
    this->actor.bgCheckFlags = curBgCheckFlags;
    return ret;
}

void En_Zf_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZf* this = (EnZf*)thisx;
    Player* player = GET_PLAYER(play);
    EffectBlureInit1 blureInit;
    f32 posDiff;

    ValueSet_process(thisx, value_init);
    thisx->attentionRangeType = ATTENTION_RANGE_3;
    this->clearFlag = PARAMS_GET_S(thisx->params, 8, 8);
    /* Strip the top byte of params */
    thisx->params &= 0xFF;

    /* Return the params to their original value if they were originally negative, i.e. 0xFFFF or 0xFFFE */
    if (PARAMS_GET_NOSHIFT(thisx->params, 7, 1)) {
        thisx->params |= 0xFF00;
    }

    Shape_Info_init(&thisx->shape, 0.0f, Actor_shadow_foot, 90.0f);
    this->unk_3E0 = 0;
    thisx->colChkInfo.mass = MASS_HEAVY;
    thisx->colChkInfo.damageTable = &btl_data;

    blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] = blureInit.p1StartColor[3] =
        blureInit.p2StartColor[0] = blureInit.p2StartColor[1] = blureInit.p2StartColor[2] = blureInit.p1EndColor[0] =
            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] = blureInit.p2EndColor[1] =
                blureInit.p2EndColor[2] = 255;
    blureInit.p2StartColor[3] = 64;
    blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;
    blureInit.elemDuration = 8;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 2;

    EffectAdd(play, &this->blureIndex, EFFECT_BLURE1, 0, 0, &blureInit);

    Actor_BGcheck2(play, thisx, 75.0f, 45.0f, 45.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);

    this->alpha = 255;
    thisx->colChkInfo.cylRadius = 40;
    thisx->colChkInfo.cylHeight = 100;
    ClObjPipe_ct(play, &this->bodyCollider);
    ClObjPipe_set5(play, &this->bodyCollider, thisx, &OcInfoData);
    ClObjSwrd_ct(play, &this->swordCollider);
    ClObjSwrd_set5(play, &this->swordCollider, thisx, &AtInfoData);

    if (thisx->params == ENZF_TYPE_DINOLFOS) {
        thisx->colChkInfo.health = 12;
        thisx->naviEnemyId = NAVI_ENEMY_DINOLFOS;
        Skeleton_Info2_M_ct(play, &this->skelAnime, &gZfDinolfosSkel, &gZfCryingAnim, this->jointTable, this->morphTable,
                       ENZF_LIMB_MAX);
    } else { // Lizalfos
        thisx->colChkInfo.health = 6;
        thisx->naviEnemyId = NAVI_ENEMY_LIZALFOS;
        Skeleton_Info2_M_ct(play, &this->skelAnime, &gZfLizalfosSkel, &gZfCryingAnim, this->jointTable, this->morphTable,
                       ENZF_LIMB_MAX);
    }

    if (thisx->params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // Not minibosses
        this->homePlatform = this->curPlatform = -1;
        move_flg = -1;
        this->hopAnimIndex = 1;
        if (thisx->params == ENZF_TYPE_LIZALFOS_LONE) {
            En_Zf_Actor_mode_before_drop_init(this);
        } else { // Dinolfos
            En_Zf_Actor_mode_wait_init(this);
        }
    } else { // Minibosses
        posDiff = player->actor.world.pos.y - thisx->world.pos.y;

        if ((ABS(posDiff) <= 100.0f) && !Actor_Environment_sw_Check(play, this->clearFlag)) {
            this->homePlatform = this->curPlatform = tbl_p_get(&thisx->world.pos, 0);
            En_Zf_Actor_mode_before_drop_init(this);
            move_flg = 1;
        } else {
            Actor_delete(thisx);
        }
    }
}

void En_Zf_Actor_dt(Actor* thisx, PlayState* play) {
    EnZf* this = (EnZf*)thisx;

    if ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ &&
        (ActorSearch(play, &this->actor, ACTOR_EN_ZF, ACTORCAT_ENEMY, 10000.0f) == NULL)) {
        Na_StopMiddleBossBgm();
    }

    EffectFreeIndex(play, this->blureIndex);
    ClObjPipe_dt(play, &this->bodyCollider);
    ClObjSwrd_dt(play, &this->swordCollider);
}

/**
 * Finds the index of the platform position in `g_tbl` that is sufficiently close to `pos`.
 * Returns `preferedIndex` if the associated position is close enough.
 */
s16 tbl_p_get(Vec3f* pos, s16 preferredIndex) {
    f32 rangeXZ;
    s16 i;

    rangeXZ = 210.0f;

    // Upstairs has a smaller range
    if (pos->y >= 420.0f) {
        rangeXZ = 110.0f;
    }

    if (preferredIndex != -1) {
        i = preferredIndex;
        if (((g_tbl[i].y - 150.0f) <= pos->y) && (pos->y <= (g_tbl[i].y + 150.0f)) &&
            ((g_tbl[i].x - rangeXZ) <= pos->x) && (pos->x <= (g_tbl[i].x + rangeXZ)) &&
            ((g_tbl[i].z - rangeXZ) <= pos->z) && (pos->z <= (g_tbl[i].z + rangeXZ))) {
            return preferredIndex;
        }
    }

    for (i = ARRAY_COUNT(g_tbl) - 1; i > -1; i--) {
        if (((g_tbl[i].y - 150.0f) <= pos->y) && (pos->y <= (g_tbl[i].y + 150.0f)) &&
            ((g_tbl[i].x - rangeXZ) <= pos->x) && (pos->x <= (g_tbl[i].x + rangeXZ)) &&
            ((g_tbl[i].z - rangeXZ) <= pos->z) && (pos->z <= (g_tbl[i].z + rangeXZ))) {
            break;
        }
    }
    return i;
}

s16 tbl_p_get2(Vec3f* pos, s16 curPlatform, s16 arg2, PlayState* play) {
    f32 distToCurLoopPlatform;
    f32 platformMinDist = 585.0f;
    Player* player = GET_PLAYER(play);
    s16 initialPlatform = curPlatform;
    f32 playerMaxDist = 400.0f;
    f32 smallMaxRange = 99998.0f;
    s16 curLoopPlatform = PLATFORM_INDEX_DOWNSTAIRS_INNER_MAX; // Will never retreat to the last two
    s16 minIndex = PLATFORM_INDEX_DOWNSTAIRS_MIN;
    f32 largeMaxRange = 99999.0f;
    s16 nextPlatform;
    s16 altNextPlatform;
    s16 playerPlatform;

    altNextPlatform = nextPlatform = -1;
    playerPlatform = tbl_p_get(&player->actor.world.pos, initialPlatform);

    // Set up search constraints
    // Upstairs
    if (pos->y > 420.0f) {
        minIndex = PLATFORM_INDEX_UPSTAIRS_MIN;
        playerMaxDist = 50.0f;

        // Upstairs outer
        if (initialPlatform >= PLATFORM_INDEX_UPSTAIRS_INNER_MAX) {
            curLoopPlatform = ARRAY_COUNT(g_tbl) - 1;
            platformMinDist = 400.0f;
        } else { // upstairs inner
            curLoopPlatform = PLATFORM_INDEX_UPSTAIRS_INNER_MAX - 1;
            platformMinDist = 380.0f;
        }
    }

    for (; curLoopPlatform >= minIndex; curLoopPlatform--) {
        if ((curLoopPlatform == initialPlatform) || (curLoopPlatform == playerPlatform)) {
            continue;
        }
        if (playerPlatform == -1) {
            s16 pad;

            if (search_position_distance(&player->actor.world.pos, &g_tbl[curLoopPlatform]) < playerMaxDist) {
                continue;
            }
        }
        distToCurLoopPlatform = search_position_distance(pos, &g_tbl[curLoopPlatform]);

        if (platformMinDist < distToCurLoopPlatform) {
            continue;
        }
        if (distToCurLoopPlatform < smallMaxRange) {
            largeMaxRange = smallMaxRange;
            altNextPlatform = nextPlatform;
            smallMaxRange = distToCurLoopPlatform;
            nextPlatform = curLoopPlatform;
        } else if (distToCurLoopPlatform < largeMaxRange) {
            largeMaxRange = distToCurLoopPlatform;
            altNextPlatform = curLoopPlatform;
        }
    }

    //! @bug `altNextPlatform` can be -1 in certain conditions and cause an out of bounds access.
    //! Under normal conditions, this doesn't cause problems because the data before `g_tbl`
    //! is section padding between .text and .data, so 0 gets read as a float.
    // These two function calls do nothing. Their return values aren't used and they have no side effects.
#ifndef AVOID_UB
    search_position_distance(&player->actor.world.pos, &g_tbl[nextPlatform]);
    search_position_distance(&player->actor.world.pos, &g_tbl[altNextPlatform]);
#endif

    if (altNextPlatform > 0) {
        s16 nextPlatformToPlayerYaw =
            search_position_angleY(pos, &g_tbl[nextPlatform]) - search_position_angleY(pos, &player->actor.world.pos);

        if (ABS(nextPlatformToPlayerYaw) < 0x36B0) {
            nextPlatform = altNextPlatform;
        }
    }

    if (nextPlatform < 0) {
        nextPlatform = arg2;
    }
    return nextPlatform;
}

s16 tbl_p_get3(Vec3f* pos, s16 curPlatform, s16 arg2, PlayState* play) {
    s16 curLoopPlatform = PLATFORM_INDEX_DOWNSTAIRS_MAX;
    s16 minIndex = PLATFORM_INDEX_DOWNSTAIRS_MIN;
    Player* player = GET_PLAYER(play);
    s16 nextPlatform = tbl_p_get(&player->actor.world.pos, -1);
    f32 minRange = 500.0f;
    f32 smallMaxRange = 99998.0f;
    f32 largeMaxRange = 99999.0f;
    s16 phi_s2 = curPlatform;
    s16 phi_s3 = arg2;
    f32 curPlatformDistToPlayer;

    // Upstairs
    if (pos->y > 200.0f) {
        curLoopPlatform = ARRAY_COUNT(g_tbl) - 1;
        minIndex = PLATFORM_INDEX_UPSTAIRS_MIN;
        minRange = 290.0f;
    }

    for (; curLoopPlatform >= minIndex; curLoopPlatform--) {
        if (minRange < search_position_distance(pos, &g_tbl[curLoopPlatform])) {
            continue;
        }
        if (curLoopPlatform != nextPlatform) {
            curPlatformDistToPlayer =
                search_position_distance(&player->actor.world.pos, &g_tbl[curLoopPlatform]);

            if (curPlatformDistToPlayer < smallMaxRange) {
                largeMaxRange = smallMaxRange;
                phi_s3 = phi_s2;
                smallMaxRange = curPlatformDistToPlayer;
                phi_s2 = curLoopPlatform;
            } else if (curPlatformDistToPlayer < largeMaxRange) {
                largeMaxRange = curPlatformDistToPlayer;
                phi_s3 = curLoopPlatform;
            }
        } else {
            phi_s2 = nextPlatform;
            break;
        }
    }

    if (phi_s3 != nextPlatform) {
        nextPlatform = phi_s2;
    } else {
        nextPlatform = phi_s3;
    }

    return nextPlatform;
}

// Player not targeting this or another EnZf?
s32 zf_ham_chk(PlayState* play, EnZf* this) {
    Actor* playerFocusActor;
    Player* player = GET_PLAYER(play);

    if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) {             // miniboss
        if (player->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14)) { // Hanging or climbing
            return false;
        } else {
            return true;
        }
    } else {
        if (!Anc_Fight_ham_Check(play, &this->actor)) {
            return true;
        }
        if (this->actor.params == ENZF_TYPE_DINOLFOS) {
            playerFocusActor = player->focusActor;
            if (playerFocusActor == NULL) {
                return false;
            } else {
                if (playerFocusActor->category != ACTORCAT_ENEMY) {
                    return true;
                }
                if (playerFocusActor->id != ACTOR_EN_ZF) {
                    return false;
                } else if (playerFocusActor->colorFilterTimer != 0) {
                    return true;
                }
            }
        }
    }
    return false;
}

void zf_branch(EnZf* this, PlayState* play) {
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (angleDiff < 0) {
        angleDiff = -angleDiff;
    }

    if (angleDiff >= 0x1B58) {
        En_Zf_Actor_mode_side_step_init(this, play);
    } else if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 8) != 0) &&
               zf_ham_chk(play, this)) {
        En_Zf_Actor_mode_kiru_init(this);
    } else {
        En_Zf_Actor_mode_wait_init(this);
    }
}

static s32 zf_br2(PlayState* play, EnZf* this) {
    s16 angleToWall;
    Actor* explosive;

    angleToWall = this->actor.wallYaw - this->actor.shape.rot.y;
    angleToWall = ABS(angleToWall);

    if (PlayerSwingCheck(play, &this->actor, 100.0f, 0x5DC0, 0x2AA8, this->actor.shape.rot.y)) {
        this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (ABS(angleToWall) < 0x2EE0) &&
            (this->actor.xzDistToPlayer < 80.0f)) {
            En_Zf_Actor_mode_jump_move_init(this);
            return true;
        } else if ((this->actor.xzDistToPlayer < 90.0f) && ((play->gameplayFrames % 2) != 0)) {
            En_Zf_Actor_mode_jump_move_init(this);
            return true;
        } else {
            En_Zf_Actor_mode_back_jump_init(this);
            return true;
        }
    }

    explosive = ActorSearch(play, &this->actor, -1, ACTORCAT_EXPLOSIVE, 80.0f);

    if (explosive != NULL) {
        this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        if (((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (angleToWall < 0x2EE0)) ||
            (explosive->id == ACTOR_EN_BOM_CHU)) {
            if ((explosive->id == ACTOR_EN_BOM_CHU) && (Actor_search_actor_distance(&this->actor, explosive) < 80.0f) &&
                ((s16)((this->actor.shape.rot.y - explosive->world.rot.y) + 0x8000) < 0x3E80)) {
                En_Zf_Actor_mode_jump_move_init(this);
                return true;
            } else {
                En_Zf_Actor_mode_side_step_init2(this, 4.0f);
                return true;
            }
        } else {
            En_Zf_Actor_mode_back_jump_init(this);
            return true;
        }
    }
    return false;
}

// Setup functions and action functions

/**
 * Set position 300 units above ground and invisible, fade in and drop to ground, fully solid when on ground
 */
void En_Zf_Actor_mode_before_drop_init(EnZf* this) {
    Skeleton_Info2_init(&this->skelAnime, &gZfJumpingAnim, 0.0f, 9.0f, Si2_anime_end_frame(&gZfJumpingAnim),
                     ANIMMODE_LOOP, 0.0f);

    this->actor.world.pos.y = this->actor.floorHeight + 300.0f;
    this->alpha = this->actor.shape.shadowAlpha = 0;
    this->unk_3F0 = 10;
    this->hopAnimIndex = 1;
    this->action = ENZF_ACTION_DROP_IN;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND_TOUCH;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_before_drop);
}

void En_Zf_Actor_mode_before_drop(EnZf* this, PlayState* play) {
    if (this->unk_3F0 == 1) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;

        if (this->actor.params == ENZF_TYPE_LIZALFOS_MINIBOSS_A) {
            Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
        }
    }

    if (this->unk_3F0 != 0) {
        if (this->actor.params != ENZF_TYPE_LIZALFOS_LONE) {
            this->unk_3F0--;
        } else if (this->actor.xzDistToPlayer <= 160.0f) {
            this->unk_3F0 = 0;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
        }

        this->actor.world.pos.y = this->actor.floorHeight + 300.0f;
    } else if (this->alpha < 255) {
        this->alpha += 255 / 5;
    }

    if ((this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) && (this->hopAnimIndex != 0)) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_ONGND);
        Skeleton_Info2_init(&this->skelAnime, &gZfLandingAnim, 1.0f, 0.0f, 17.0f, ANIMMODE_ONCE, 0.0f);
        this->hopAnimIndex = 0;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND_TOUCH;
        this->actor.world.pos.y = this->actor.floorHeight;
        this->actor.velocity.y = 0.0f;
        _dust_ground_set(play, &this->actor, &this->leftFootPos, 3.0f, 2, 2.0f, 0, 0, false);
        _dust_ground_set(play, &this->actor, &this->rightFootPos, 3.0f, 2, 2.0f, 0, 0, false);
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->alpha = 255;
        if (this->actor.params > ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // Only miniboss B
            En_Zf_Actor_mode_nige_start_init(this, play);
        } else {
            En_Zf_Actor_mode_wait_init(this);
        }
    }
    this->actor.shape.shadowAlpha = this->alpha;
}

// stop? and choose an action
void En_Zf_Actor_mode_wait_init(EnZf* this) {
    Skeleton_Info2_init(&this->skelAnime, &gZfCryingAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gZfCryingAnim),
                     ANIMMODE_LOOP_INTERP, -4.0f);
    this->action = ENZF_ACTION_3;
    this->unk_3F0 = fqrand() * 10.0f + 5.0f;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_wait);
}

void En_Zf_Actor_mode_wait(EnZf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 angleToPlayer = (this->actor.yawTowardsPlayer - this->headRot) - this->actor.shape.rot.y;

    angleToPlayer = ABS(angleToPlayer);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (!Shot_def(play, this)) {
        if (this->actor.params == ENZF_TYPE_DINOLFOS) {
            if (this->unk_3F4 != 0) {
                this->unk_3F4--;
                if (angleToPlayer >= 0x1FFE) {
                    return;
                }
                this->unk_3F4 = 0;

            } else if (zf_br2(play, this)) {
                return;
            }
        }
        angleToPlayer = player->actor.shape.rot.y - this->actor.shape.rot.y;
        angleToPlayer = ABS(angleToPlayer);

        if ((this->actor.xzDistToPlayer < 100.0f) && (player->meleeWeaponState != 0) && (angleToPlayer >= 0x1F40)) {
            this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            En_Zf_Actor_mode_side_step_init(this, play);
        } else if (this->unk_3F0 != 0) {
            this->unk_3F0--;
        } else {
            if (Actor_player_direction_check(&this->actor, 30 * 0x10000 / 360)) {
                if ((this->actor.xzDistToPlayer < 200.0f) && (this->actor.xzDistToPlayer > 100.0f) &&
                    (fqrand() < 0.3f)) {
                    if (this->actor.params == ENZF_TYPE_DINOLFOS) {
                        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
                        En_Zf_Actor_mode_oi_jump_init(this);
                    } else {
                        En_Zf_Actor_mode_side_step_init(this, play);
                    }
                } else if (fqrand() > 0.3f) {
                    En_Zf_Actor_mode_forward_init(this, play);
                } else {
                    En_Zf_Actor_mode_side_step_init(this, play);
                }
            } else {
                En_Zf_Actor_mode_search_rot_init(this);
            }

            if ((play->gameplayFrames & 0x5F) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
            }
        }
    }
}

void En_Zf_Actor_mode_forward_init(EnZf* this, PlayState* play) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gZfWalkingAnim, -4.0f);
    this->action = ENZF_ACTION_APPROACH_PLAYER;

    if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        this->curPlatform = tbl_p_get(&this->actor.world.pos, this->curPlatform);
        this->nextPlatform =
            tbl_p_get3(&this->actor.world.pos, this->curPlatform, this->homePlatform, play);
        this->hopAnimIndex = 0;
    }
    this->actor.speed = 0.0f;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_forward);
}

void En_Zf_Actor_mode_forward(EnZf* this, PlayState* play) {
    s32 prevFrame;
    s32 beforeCurFrame;
    s32 afterPrevFrame;
    s16 temp_v1;
    s16 sp48 = -1;
    f32 sp44 = 350.0f;
    f32 sp40 = 0.0f;
    Player* player = GET_PLAYER(play);
    s32 absPlaySpeed;

    if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        sp48 = tbl_p_get(&player->actor.world.pos, sp48);
        this->curPlatform = tbl_p_get(&this->actor.world.pos, sp48);

        if (this->actor.world.pos.y >= 420.0f) {
            sp44 = 270.0f;
        }
    }

    if (!Shot_def(play, this)) {
        if (sp48 != this->curPlatform) {
            this->nextPlatform =
                tbl_p_get3(&this->actor.world.pos, this->curPlatform, this->homePlatform, play);

            if ((sp48 < 0) && (this->nextPlatform == this->curPlatform)) {
                sp48 = this->curPlatform;
                this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            } else {
                this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer =
                    Actor_search_position_angleY(&this->actor, &g_tbl[this->nextPlatform]);

                temp_v1 = this->actor.wallYaw - this->actor.shape.rot.y;
                temp_v1 = ABS(temp_v1);

                if ((this->unk_3F8 && (this->actor.speed > 0.0f)) ||
                    ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (temp_v1 >= 0x5C19))) {
                    if ((Actor_search_position_distanceXZ(&this->actor, &g_tbl[this->nextPlatform]) < sp44) &&
                        !hasi_chk(this, play, 191.9956f)) {
                        En_Zf_Actor_mode_oi_jump_init(this);

                        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                            this->actor.velocity.y = 20.0f;
                        }

                        return;
                    } else {
                        this->actor.world.rot.y =
                            Actor_search_position_angleY(&this->actor, &g_tbl[this->curPlatform]);
                    }
                } else {
                    this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    add_calc(&this->actor.speed, 8.0f, 1.0f, 1.5f, 0.0f);
                }
            }
        }

        if (Anc_Fight_ham_Check(play, &this->actor)) {
            sp40 = 100.0f;
        }

        if (this->actor.xzDistToPlayer <= (70.0f + sp40)) {
            add_calc(&this->actor.speed, -8.0f, 1.0f, 0.5f, 0.0f);
        } else {
            add_calc(&this->actor.speed, 8.0f, 1.0f, 0.5f, 0.0f);
        }

        this->skelAnime.playSpeed = this->actor.speed * 1.2f;

        temp_v1 = player->actor.shape.rot.y - this->actor.shape.rot.y;
        temp_v1 = ABS(temp_v1);

        if ((sp48 == this->curPlatform) && (this->actor.xzDistToPlayer < 150.0f) && (player->meleeWeaponState != 0) &&
            (temp_v1 >= 0x1F40)) {
            this->actor.shape.rot.y = this->actor.world.rot.y = this->actor.yawTowardsPlayer;

            if (fqrand() > 0.7f) {
                En_Zf_Actor_mode_side_step_init(this, play);
                return;
            }
        }

        prevFrame = (s32)this->skelAnime.curFrame;
        Skeleton_Info2_anime_play(&this->skelAnime);
        beforeCurFrame = (s32)(this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed));
        absPlaySpeed = (s32)(f32)ABS(this->skelAnime.playSpeed);

        if (sp48 == this->curPlatform) {
            if (!Actor_player_direction_check(&this->actor, 0x11C7)) {
                if (fqrand() > 0.5f) {
                    En_Zf_Actor_mode_move_to_player_front_init(this, play);
                } else {
                    En_Zf_Actor_mode_wait_init(this);
                }
            } else if (this->actor.xzDistToPlayer < 100.0f) {
                if ((fqrand() > 0.05f) && zf_ham_chk(play, this)) {
                    En_Zf_Actor_mode_kiru_init(this);
                } else if (fqrand() > 0.5f) {
                    En_Zf_Actor_mode_side_step_init(this, play);
                } else {
                    En_Zf_Actor_mode_wait_init(this);
                }
            } else {
                if (this->unk_3F8) {
                    En_Zf_Actor_mode_move_to_player_front_init(this, play);
                } else if (fqrand() < 0.1f) {
                    En_Zf_Actor_mode_wait_init(this);
                }
            }
        }

        if (this->actor.params == ENZF_TYPE_DINOLFOS) {
            if (zf_br2(play, this)) {
                return;
            }

            if ((this->actor.xzDistToPlayer < 180.0f) && (this->actor.xzDistToPlayer > 160.0f) &&
                Actor_player_direction_check(&this->actor, 0x71C)) {
                if (Anc_Fight_My_Check(play, &this->actor)) {
                    if (fqrand() < 0.1f) {
                        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
                        En_Zf_Actor_mode_oi_jump_init(this);
                        return;
                    }
                } else {
                    En_Zf_Actor_mode_side_step_init(this, play);
                    return;
                }
            }
        }

        if ((play->gameplayFrames & 0x5F) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
        }

        if (prevFrame != (s32)this->skelAnime.curFrame) {
            afterPrevFrame = absPlaySpeed + prevFrame;

            if (((beforeCurFrame < 2) && (afterPrevFrame >= 4)) || ((beforeCurFrame < 32) && (afterPrevFrame >= 34))) {
                Actor_SE_set(&this->actor, NA_SE_EN_RIZA_WALK);
            }
        }
    }
}

void En_Zf_Actor_mode_oi_jump_init(EnZf* this) {
    Skeleton_Info2_init(&this->skelAnime, &gZfJumpingAnim, 1.0f, 0.0f, 3.0f, ANIMMODE_ONCE, -3.0f);
    this->unk_3F0 = 0;
    this->hopAnimIndex = 1;
    this->actor.velocity.y = 15.0f;

    if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        this->actor.speed = 16.0f;
    } else {
        this->actor.speed = 10.0f;
    }

    this->action = ENZF_ACTION_JUMP_FORWARD;
    Actor_SE_set(&this->actor, NA_SE_EN_RIZA_JUMP);
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_oi_jump);
}

void En_Zf_Actor_mode_oi_jump(EnZf* this, PlayState* play) {
    if ((this->unk_3F0 != 0) && (this->actor.world.pos.y <= this->actor.floorHeight)) {
        this->actor.world.pos.y = this->actor.floorHeight;
        this->hopAnimIndex = 0;
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->unk_3F0 == 0) {
            Skeleton_Info2_init(&this->skelAnime, &gZfLandingAnim, 3.0f, 0.0f, 17.0f, ANIMMODE_ONCE, -3.0f);
            this->unk_3F0 = 10;
            Actor_SE_set(&this->actor, NA_SE_EN_RIZA_JUMP);
        } else {
            this->actor.speed = 0.0f;
            this->hopAnimIndex = 0;
            En_Zf_Actor_mode_forward_init(this, play);
        }
    }
    if ((play->gameplayFrames & 0x5F) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
    }

    if ((this->actor.params == ENZF_TYPE_DINOLFOS) &&
        (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH))) {
        if (zf_ham_chk(play, this)) {
            En_Zf_Actor_mode_kiru_init(this);
        } else {
            En_Zf_Actor_mode_side_step_init(this, play);
        }
    }
}

void En_Zf_Actor_mode_search_rot_init(EnZf* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gZfWalkingAnim, -4.0f);
    this->action = ENZF_ACTION_6;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_search_rot);
}

void En_Zf_Actor_mode_search_rot(EnZf* this, PlayState* play) {
    s32 pad;
    f32 phi_f2;
    Player* player = GET_PLAYER(play);
    s16 temp_v0;
    s16 phi_v1;

    if (!Shot_def(play, this)) {
        if ((this->actor.params != ENZF_TYPE_DINOLFOS) || !zf_br2(play, this)) {
            temp_v0 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

            if (temp_v0 > 0) {
                phi_v1 = temp_v0 * 0.25f + 2000.0f;
            } else {
                phi_v1 = temp_v0 * 0.25f - 2000.0f;
            }

            this->actor.shape.rot.y += phi_v1;
            this->actor.world.rot.y = this->actor.shape.rot.y;

            if (temp_v0 > 0) {
                phi_f2 = phi_v1 * 1.5f;
                if (phi_f2 > 2.0f) {
                    phi_f2 = 2.0f;
                }
            } else {
                phi_f2 = phi_v1 * 1.5f;
                if (phi_f2 < -2.0f) {
                    phi_f2 = -2.0f;
                }
            }

            this->skelAnime.playSpeed = -phi_f2;
            Skeleton_Info2_anime_play(&this->skelAnime);

            if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
                this->curPlatform = tbl_p_get(&this->actor.world.pos, this->curPlatform);
                if (this->curPlatform != tbl_p_get(&player->actor.world.pos, -1)) {
                    En_Zf_Actor_mode_forward_init(this, play);
                    return;
                }
            }

            if (Actor_player_direction_check(&this->actor, 30 * 0x10000 / 360)) {
                if (fqrand() > 0.8f) {
                    En_Zf_Actor_mode_move_to_player_front_init(this, play);
                } else {
                    En_Zf_Actor_mode_forward_init(this, play);
                }
            }
            if ((play->gameplayFrames & 0x5F) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
            }
        }
    }
}

// Conditional setup function
void En_Zf_Actor_mode_move_to_player_front_init(EnZf* this, PlayState* play) {
    if ((this->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ ||
        BG_point_check(&this->actor, play, 40.0f, (s16)(this->actor.shape.rot.y + 0x3FFF)) ||
        BG_point_check(&this->actor, play, -40.0f, (s16)(this->actor.shape.rot.y + 0x3FFF))) {
        Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gZfSidesteppingAnim);
        this->actor.speed = rnd_fx(12.0f);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->unk_3F0 = fqrand() * 10.0f + 20.0f;
        this->hopAnimIndex = 0;
        this->action = ENZF_ACTION_7;
        this->unk_408 = 0.0f;
        En_Zf_actor_set_process(this, En_Zf_Actor_mode_move_to_player_front);
    } else {
        En_Zf_Actor_mode_forward_init(this, play);
    }
}

void En_Zf_Actor_mode_move_to_player_front(EnZf* this, PlayState* play) {
    s16 angleBehindPlayer;
    s16 phi_v0_3;
    s32 pad;
    s32 prevFrame;
    s32 beforeCurFrame;
    s32 absPlaySpeed;
    Player* player = GET_PLAYER(play);
    f32 baseRange = 0.0f;

    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 4000, 1);

    if (!Shot_def(play, this) &&
        ((this->actor.params != ENZF_TYPE_DINOLFOS) || !zf_br2(play, this))) {
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3A98;
        angleBehindPlayer = player->actor.shape.rot.y + 0x8000;

        if (sin_s(angleBehindPlayer - this->actor.shape.rot.y) >= 0.0f) {
            this->actor.speed -= 0.25f;
            if (this->actor.speed < -8.0f) {
                this->actor.speed = -8.0f;
            }
        } else if (sin_s(angleBehindPlayer - this->actor.shape.rot.y) < 0.0f) { // Superfluous check
            this->actor.speed += 0.25f;
            if (this->actor.speed > 8.0f) {
                this->actor.speed = 8.0f;
            }
        }

        if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
            if (this->unk_3F8) {
                this->actor.speed = -this->actor.speed;
            }
        } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
                   !BG_point_check(&this->actor, play, this->actor.speed,
                                               this->actor.shape.rot.y + 0x3FFF)) {
            if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                if (this->actor.speed >= 0.0f) {
                    phi_v0_3 = this->actor.shape.rot.y + 0x3FFF;
                } else {
                    phi_v0_3 = this->actor.shape.rot.y - 0x3FFF;
                }
                phi_v0_3 = this->actor.wallYaw - phi_v0_3;
            } else {
                this->actor.speed *= -0.8f;
                phi_v0_3 = 0;
            }

            if (ABS(phi_v0_3) > 0x4000) {
                this->actor.speed *= -0.8f;
                if (this->actor.speed < 0.0f) {
                    this->actor.speed -= 0.5f;
                } else {
                    this->actor.speed += 0.5f;
                }
            }
        }

        if (Anc_Fight_ham_Check(play, &this->actor)) {
            baseRange = 100.0f;
        }

        if (this->actor.xzDistToPlayer <= (70.0f + baseRange)) {
            add_calc(&this->unk_408, -4.0f, 1.0f, 1.5f, 0.0f);
        } else if ((90.0f + baseRange) < this->actor.xzDistToPlayer) {
            add_calc(&this->unk_408, 4.0f, 1.0f, 1.5f, 0.0f);
        } else {
            add_calc(&this->unk_408, 0.0f, 1.0f, 5.65f, 0.0f);
        }

        if ((this->unk_408 != 0.0f) && !hasi_chk2(this, play, this->unk_408)) {
            this->actor.world.pos.x += sin_s(this->actor.shape.rot.y) * this->unk_408;
            this->actor.world.pos.z += cos_s(this->actor.shape.rot.y) * this->unk_408;
        }

        if (ABS(this->actor.speed) >= ABS(this->unk_408)) {
            this->skelAnime.playSpeed = this->actor.speed * 0.75f;
        } else if (this->skelAnime.playSpeed < 0.0f) {
            this->skelAnime.playSpeed = this->unk_408 * -0.75f;
        } else {
            this->skelAnime.playSpeed = this->unk_408 * 0.75f;
        }

        prevFrame = (s32)this->skelAnime.curFrame;
        Skeleton_Info2_anime_play(&this->skelAnime);
        beforeCurFrame = (s32)(this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed));
        absPlaySpeed = (s32)(f32)ABS(this->skelAnime.playSpeed);

        if (prevFrame != (s32)this->skelAnime.curFrame) {
            s32 afterPrevFrame = absPlaySpeed + prevFrame;

            if (((beforeCurFrame < 14) && (afterPrevFrame >= 16)) ||
                ((beforeCurFrame < 27) && (afterPrevFrame >= 29))) {
                Actor_SE_set(&this->actor, NA_SE_EN_RIZA_WALK);
            }
        }

        if ((play->gameplayFrames & 0x5F) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
        }

        if ((cos_s(angleBehindPlayer - this->actor.shape.rot.y) < -0.85f) || (this->unk_3F0 == 0)) {
            this->actor.world.rot.y = this->actor.shape.rot.y;

            if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) == 0) &&
                zf_ham_chk(play, this)) {
                En_Zf_Actor_mode_kiru_init(this);
            } else {
                En_Zf_Actor_mode_wait_init(this);
            }
        } else if (this->unk_3F0 != 0) {
            this->unk_3F0--;
        }
    }
}

void En_Zf_Actor_mode_kiru_init(EnZf* this) {
    Skeleton_Info2_init(&this->skelAnime, &gZfSlashAnim, 1.25f, 0.0f, Si2_anime_end_frame(&gZfSlashAnim), ANIMMODE_ONCE,
                     -4.0f);

    if (this->actor.params == ENZF_TYPE_DINOLFOS) {
        this->skelAnime.playSpeed = 1.75f;
    }

    this->swordCollider.base.atFlags &= ~AT_BOUNCED;
    this->action = ENZF_ACTION_SLASH;
    Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
    this->actor.speed = 0.0f;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_kiru);
}

void En_Zf_Actor_mode_kiru(EnZf* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 rotDiff;
    s16 yawDiff;

    this->actor.speed = 0.0f;

    if ((s32)this->skelAnime.curFrame == 10) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_ATTACK);
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        EffectBlure_space_add(Effect_GetEffectMemoryPointer(this->blureIndex));

        if ((this->actor.params == ENZF_TYPE_DINOLFOS) && !Actor_player_direction_check(&this->actor, 5460)) {
            En_Zf_Actor_mode_wait_init(this);
            this->unk_3F0 = fqrand() * 5.0f + 5.0f;
            this->unk_3F4 = fqrand() * 20.0f + 100.0f;
        } else if ((fqrand() > 0.7f) || (this->actor.xzDistToPlayer >= 120.0f)) {
            En_Zf_Actor_mode_wait_init(this);
            this->unk_3F0 = fqrand() * 5.0f + 5.0f;
        } else {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;

            if (fqrand() > 0.7f) {
                En_Zf_Actor_mode_side_step_init(this, play);
            } else {
                rotDiff = player->actor.shape.rot.y - this->actor.shape.rot.y;
                rotDiff = ABS(rotDiff);

                if (rotDiff <= 10000) {
                    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                    yawDiff = ABS(yawDiff);

                    if (yawDiff > 16000) {
                        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                        En_Zf_Actor_mode_side_step_init(this, play);
                    } else if (player->stateFlags1 &
                               (PLAYER_STATE1_HOSTILE_LOCK_ON | PLAYER_STATE1_13 | PLAYER_STATE1_14)) {
                        if (this->actor.isLockedOn) {
                            En_Zf_Actor_mode_kiru_init(this);
                        } else {
                            En_Zf_Actor_mode_side_step_init(this, play);
                        }
                    } else {
                        En_Zf_Actor_mode_kiru_init(this);
                    }
                } else {
                    En_Zf_Actor_mode_side_step_init(this, play);
                }
            }
        }
    }
}

void En_Zf_Actor_mode_hirumu_init(EnZf* this) {
    f32 frame = this->skelAnime.curFrame - 3.0f;

    Skeleton_Info2_init(&this->skelAnime, &gZfSlashAnim, -1.0f, frame, 0.0f, ANIMMODE_ONCE, 0.0f);
    this->action = ENZF_ACTION_RECOIL_FROM_BLOCKED_SLASH;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_hirumu);
}

void En_Zf_Actor_mode_hirumu(EnZf* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (fqrand() > 0.7f) {
            En_Zf_Actor_mode_wait_init(this);
        } else if ((fqrand() > 0.2f) && zf_ham_chk(play, this)) {
            En_Zf_Actor_mode_kiru_init(this);
        } else {
            En_Zf_Actor_mode_side_step_init(this, play);
        }
    }
}

void En_Zf_Actor_mode_back_jump_init(EnZf* this) {
    Skeleton_Info2_init(&this->skelAnime, &gZfJumpingAnim, -1.0f, 3.0f, 0.0f, ANIMMODE_ONCE, -3.0f);
    this->unk_3F0 = 0;
    this->hopAnimIndex = 1;
    this->action = ENZF_ACTION_JUMP_BACK;
    this->actor.velocity.y = 15.0f;
    this->actor.speed = -15.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_RIZA_JUMP);
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_back_jump);
}

void En_Zf_Actor_mode_back_jump(EnZf* this, PlayState* play) {
    if ((this->unk_3F0 != 0) && (this->actor.world.pos.y <= this->actor.floorHeight)) {
        this->actor.world.pos.y = this->actor.floorHeight;
        this->hopAnimIndex = 0;
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->unk_3F0 == 0) {
            Skeleton_Info2_init(&this->skelAnime, &gZfLandingAnim, 3.0f, 0.0f, 17.0f, ANIMMODE_ONCE, -3.0f);
            this->unk_3F0 = 10;
            Actor_SE_set(&this->actor, NA_SE_EN_RIZA_JUMP);
        } else if ((play->gameplayFrames % 2) != 0) {
            En_Zf_Actor_mode_side_step_init(this, play);
        } else {
            En_Zf_Actor_mode_wait_init(this);
        }
    }

    if ((play->state.frames & 0x5F) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
    }
}

void En_Zf_Actor_mode_paralyze_init(EnZf* this) {
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        ((this->actor.velocity.y == 0.0f) || (this->actor.velocity.y == -4.0f))) {
        this->actor.speed = 0.0f;
        this->hopAnimIndex = 0;
    } else {
        this->hopAnimIndex = 1;
    }

    if (this->damageEffect == ENZF_DMGEFF_ICE) {
        this->iceTimer = 36;
    } else {
        Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gZfKnockedBackAnim, 0.0f);
    }

    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->action = ENZF_ACTION_STUNNED;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_paralyze);
}

void En_Zf_Actor_mode_paralyze(EnZf* this, PlayState* play) {
    s16 angleToWall;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.speed < 0.0f) {
            this->actor.speed += 0.05f;
        }
        this->hopAnimIndex = 0;
    }

    if ((this->actor.colorFilterTimer == 0) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->actor.colChkInfo.health == 0) {
            En_Zf_Actor_mode_down_init(this);
        } else if ((this->actor.params != ENZF_TYPE_DINOLFOS) || !zf_br2(play, this)) {
            if (move_flg != -1) {
                zf_branch(this, play);
            } else {
                angleToWall = this->actor.wallYaw - this->actor.shape.rot.y;
                angleToWall = ABS(angleToWall);

                if ((this->actor.params == ENZF_TYPE_DINOLFOS) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
                    (ABS(angleToWall) < 0x2EE0) && (this->actor.xzDistToPlayer < 90.0f)) {
                    this->actor.world.rot.y = this->actor.shape.rot.y;
                    En_Zf_Actor_mode_jump_move_init(this);
                } else if (!Shot_def(play, this)) {
                    if (this->actor.params != ENZF_TYPE_DINOLFOS) {
                        zf_branch(this, play);
                    } else if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) != 0) &&
                               zf_ham_chk(play, this)) {
                        En_Zf_Actor_mode_kiru_init(this);
                    } else {
                        zf_branch(this, play);
                    }
                }
            }
        }
    }
}

void En_Zf_Actor_mode_nige_start_init(EnZf* this, PlayState* play) {
    f32 morphFrames = 0.0f;
    f32 lastFrame = Si2_anime_end_frame(&gZfSheathingSwordAnim);

    if (this->action <= ENZF_ACTION_DAMAGED) {
        morphFrames = -4.0f;
    }

    Skeleton_Info2_init(&this->skelAnime, &gZfSheathingSwordAnim, 2.0f, 0.0f, lastFrame, ANIMMODE_ONCE, morphFrames);
    this->action = ENZF_ACTION_SHEATHE_SWORD;
    this->actor.speed = 0.0f;
    this->curPlatform = tbl_p_get(&this->actor.world.pos, this->curPlatform);
    this->nextPlatform =
        tbl_p_get2(&this->actor.world.pos, this->curPlatform, this->homePlatform, play);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_nige_start);
}

void En_Zf_Actor_mode_nige_start(EnZf* this, PlayState* play) {
    s16 yaw = Actor_search_position_angleY(&this->actor, &g_tbl[this->nextPlatform]) + 0x8000;

    add_calc_short_angle2(&this->actor.world.rot.y, yaw, 1, 1000, 0);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->actor.world.rot.y = yaw - 0x8000;
        En_Zf_Actor_mode_nige_move_init(this, play);
        this->swordSheathed = true;
    }
}

void En_Zf_Actor_mode_nige_wait_init(EnZf* this) {
    this->hopAnimIndex = 0;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, j_tbl[0], -4.0f);
    this->action = ENZF_ACTION_HOP_AND_TAUNT;
    this->actor.speed = 0.0f;
    this->unk_40C = 0.0f;
    this->unk_408 = 0.0f;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_nige_wait);
}

void En_Zf_Actor_mode_nige_wait(EnZf* this, PlayState* play) {
    f32 lastFrame;
    f32 maxDist = 400.0f;

    add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer + 0x8000, 1, 4000, 0);

    // Upstairs
    if (this->actor.world.pos.y >= 420.0f) {
        maxDist = 250.0f;
    }

    // If player gets too close, run away
    if ((this->actor.xzDistToPlayer < maxDist) && (this->hopAnimIndex != 1)) {
        this->actor.shape.rot.y = this->actor.world.rot.y;
        En_Zf_Actor_mode_nige_start_init(this, play);
    } else {
        if (this->hopAnimIndex != 1) {
            Shot_def2(play, this);
        }

        if (Skeleton_Info2_anime_play(&this->skelAnime)) {
            this->hopAnimIndex++; // move on to next animation

            // Loop back to beginning
            if (this->hopAnimIndex >= ARRAY_COUNT(j_tbl)) {
                this->hopAnimIndex = 0;
            }

            if ((this->unk_408 != 0.0f) || (this->unk_40C != 0.0f)) {
                this->hopAnimIndex = 1;
            }

            lastFrame = Si2_anime_end_frame(j_tbl[this->hopAnimIndex]);

            switch (this->hopAnimIndex) {
                case 0:
                    this->actor.velocity.y = 0.0f;
                    this->actor.world.pos.y = this->actor.floorHeight;
                    break;

                case 1:
                    this->actor.velocity.y = this->unk_40C + 10.0f;
                    this->actor.speed = this->unk_408;
                    this->unk_408 = 0.0f;
                    this->unk_40C = 0.0f;
                    break;

                case 2:
                    this->actor.world.pos.y = this->actor.floorHeight;
                    lastFrame = 3.0f;
                    break;

                default:
                    break;
            }

            Skeleton_Info2_init(&this->skelAnime, j_tbl[this->hopAnimIndex], 1.5f, 0.0f, lastFrame, ANIMMODE_ONCE,
                             0.0f);
        }

        if ((play->gameplayFrames & 0x5F) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
        }
    }
}

void En_Zf_Actor_mode_nige_move_init(EnZf* this, PlayState* play) {
    this->hopAnimIndex = 0;
    Skeleton_Info2_init_standard_stop(&this->skelAnime, j_tbl[0]);
    this->action = ENZF_ACTION_HOP_AWAY;
    this->curPlatform = tbl_p_get(&this->actor.world.pos, this->curPlatform);
    this->nextPlatform =
        tbl_p_get2(&this->actor.world.pos, this->curPlatform, this->homePlatform, play);
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_nige_move);
}

void En_Zf_Actor_mode_nige_move(EnZf* this, PlayState* play) {
    f32 sp74;
    f32 sp70 = 1.0f;
    f32 phi_f20 = 550.0f;
    s32 pad;
    f32 phi_f20_2;
    f32 phi_f0;
    s32 pad2;
    s16 sp5A;
    s32 sp54;
    s32 temp_v1_2;
    s32 phi_v1;

    sp74 = Actor_search_position_distanceXZ(&this->actor, &g_tbl[this->nextPlatform]);
    sp54 = this->hopAnimIndex;

    if ((play->gameplayFrames & 0x5F) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
    }

    // Upstairs
    if (this->actor.world.pos.y >= 420.0f) {
        phi_f20 = 280.0f;
    }

    sp5A = Actor_search_position_angleY(&this->actor, &g_tbl[this->nextPlatform]);

    switch (this->hopAnimIndex) {
        case 0:
            this->actor.world.rot.y = sp5A;
            this->actor.shape.rot.y = sp5A + 0x8000;
            tim = 0;
            this->homePlatform = this->curPlatform;
            temp_v1_2 = !hasi_chk(this, play, 107.0f);
            temp_v1_2 |= !hasi_chk(this, play, 220.0f) << 1;
            this->hopAnimIndex++;

            switch (temp_v1_2) {
                case 1:
                case 1 | 2:
                    this->actor.velocity.y = 12.0f;
                    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                        this->actor.velocity.y += 8.0f;
                    }

                    this->actor.speed = 8.0f;
                    break;

                case 2:
                    this->actor.velocity.y = 15.0f;
                    this->actor.speed = 20.0f;
                    break;

                default: // 0
                    phi_f20_2 = 107.0f;
                    phi_f20_2 += 10.0f;
                    phi_f0 = 8.0f;
                    phi_f0 += 1.2f;

                    for (phi_v1 = 20; phi_v1 >= 0; phi_v1--, phi_f20_2 += 10.0f, phi_f0 += 1.2f) {

                        if (!hasi_chk(this, play, phi_f20_2)) {
                            this->actor.speed = phi_f0;
                            this->actor.velocity.y = 12.0f;
                            break;
                        }
                    }
                    if (this->actor.speed == 0.0f) {
                        En_Zf_Actor_mode_nige_wait_init(this);
                    }
            }
            break;

        case 1:
            if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) ||
                (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                Actor_SE_set(&this->actor, NA_SE_EN_RIZA_ONGND);
                this->actor.velocity.y = 0.0f;
                this->actor.world.pos.y = this->actor.floorHeight;
                this->actor.speed = 0.0f;
                _dust_ground_set(play, &this->actor, &this->leftFootPos, 3.0f, 2, 2.0f, 0, 0, false);
                _dust_ground_set(play, &this->actor, &this->rightFootPos, 3.0f, 2, 2.0f, 0, 0, false);

                if (phi_f20 <= this->actor.xzDistToPlayer) {
                    En_Zf_Actor_mode_nige_wait_init(this);
                } else if (sp74 < 80.0f) {
                    this->curPlatform = tbl_p_get(&this->actor.world.pos, this->curPlatform);
                    this->nextPlatform = tbl_p_get2(&this->actor.world.pos, this->curPlatform,
                                                                             this->homePlatform, play);
                }

                this->hopAnimIndex = 0;
                sp70 = 2.0f;
            } else {
                add_calc_short_angle2(&this->actor.world.rot.y, sp5A, 1, 0xFA0, 0);
                this->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
                tim++;
            }
            break;

        case 2:
            if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
                this->hopAnimIndex = 0;
            }
            break;
    }

    if (sp54 != this->hopAnimIndex) {
        Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, j_tbl[this->hopAnimIndex], sp70);
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
}

void En_Zf_Actor_mode_nige_end_init(EnZf* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gZfDrawingSwordAnim);
    this->actor.world.rot.y += 0x8000;
    this->action = ENZF_ACTION_DRAW_SWORD;
    this->actor.speed = 0.0f;
    this->curPlatform = tbl_p_get(&this->actor.world.pos, this->curPlatform);
    this->nextPlatform =
        tbl_p_get2(&this->actor.world.pos, this->curPlatform, this->homePlatform, play);
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_nige_end);
}

void En_Zf_Actor_mode_nige_end(EnZf* this, PlayState* play) {
    s16 yawTowardsPlayer = this->actor.yawTowardsPlayer;

    if (this->skelAnime.curFrame >= 26.0f) {
        add_calc_short_angle2(&this->actor.shape.rot.y, yawTowardsPlayer, 1, 6000, 0);
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
        this->actor.world.rot.y = yawTowardsPlayer;
        this->hopAnimIndex = -1;
        En_Zf_Actor_mode_wait_init(this);
    }

    if (this->skelAnime.curFrame == 22.0f) {
        this->swordSheathed = false;
    }
}

void En_Zf_Actor_mode_damage_init(EnZf* this) {
    Skeleton_Info2_init(&this->skelAnime, &gZfKnockedBackAnim, 1.5f, 0.0f, Si2_anime_end_frame(&gZfKnockedBackAnim),
                     ANIMMODE_ONCE, -4.0f);

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        ((this->actor.velocity.y == 0.0f) || (this->actor.velocity.y == -4.0f))) {
        this->actor.speed = -4.0f;
        this->hopAnimIndex = 0;
    } else {
        this->hopAnimIndex = 1;
    }

    if (this->actor.params == ENZF_TYPE_DINOLFOS) {
        this->skelAnime.playSpeed = 4.5f;
    }

    if (this->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // not miniboss
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    }

    Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DAMAGE);
    this->action = ENZF_ACTION_DAMAGED;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_damage);
}

void En_Zf_Actor_mode_damage(EnZf* this, PlayState* play) {
    s16 wallYawDiff;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.speed < 0.0f) {
            this->actor.speed += 0.05f;
        }
        this->hopAnimIndex = 0;
    }

    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 4500, 0);

    if (((this->actor.params != ENZF_TYPE_DINOLFOS) || !zf_br2(play, this)) &&
        Skeleton_Info2_anime_play(&this->skelAnime) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {

        if (move_flg != -1) {
            if (this->damageEffect == ENZF_DMGEFF_PROJECTILE) {
                move_flg_trg++;
            } else {
                this->actor.world.rot.y = this->actor.shape.rot.y;

                if (!hasi_chk(this, play, 135.0f) && (this->actor.xzDistToPlayer < 90.0f)) {
                    En_Zf_Actor_mode_jump_move_init(this);
                } else if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) == 0)) {
                    En_Zf_Actor_mode_kiru_init(this);
                } else {
                    zf_branch(this, play);
                }
            }
        } else {

            wallYawDiff = this->actor.wallYaw - this->actor.shape.rot.y;
            wallYawDiff = ABS(wallYawDiff);

            if ((this->actor.params == ENZF_TYPE_DINOLFOS) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) &&
                (ABS(wallYawDiff) < 12000) && (this->actor.xzDistToPlayer < 90.0f)) {
                En_Zf_Actor_mode_jump_move_init(this);
            } else if (!Shot_def(play, this)) {
                if (this->actor.params != ENZF_TYPE_DINOLFOS) {
                    this->actor.world.rot.y = this->actor.shape.rot.y;

                    if (!hasi_chk(this, play, 135.0f) && (this->actor.xzDistToPlayer < 90.0f)) {
                        En_Zf_Actor_mode_jump_move_init(this);
                    } else if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) == 0)) {
                        En_Zf_Actor_mode_kiru_init(this);
                    } else {
                        zf_branch(this, play);
                    }
                } else if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) == 0) &&
                           zf_ham_chk(play, this)) {
                    En_Zf_Actor_mode_kiru_init(this);
                } else {
                    zf_branch(this, play);
                }
            }
        }
    }
}

void En_Zf_Actor_mode_jump_move_init(EnZf* this) {
    Skeleton_Info2_init(&this->skelAnime, &gZfJumpingAnim, 1.0f, 0.0f, 3.0f, ANIMMODE_ONCE, 0.0f);
    this->unk_3F0 = 0;
    this->hopAnimIndex = 1;
    this->action = ENZF_ACTION_JUMP_UP;
    this->actor.velocity.y = 22.0f;
    this->actor.speed = 7.5f;
    Actor_SE_set(&this->actor, NA_SE_EN_RIZA_JUMP);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_jump_move);
}

void En_Zf_Actor_mode_jump_move(EnZf* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 4000, 1);
    if (this->actor.velocity.y >= 5.0f) {
        suna_set(play, &this->leftFootPos);
        suna_set(play, &this->rightFootPos);
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->unk_3F0 == 0) {
            Skeleton_Info2_init(&this->skelAnime, &gZfSlashAnim, 3.0f, 0.0f, 13.0f, ANIMMODE_ONCE, -4.0f);
            this->unk_3F0 = 10;
        } else if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) {
            this->actor.velocity.y = 0.0f;
            this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
            this->actor.speed = 0.0f;
            this->actor.world.pos.y = this->actor.floorHeight;
            En_Zf_Actor_mode_kiru_init(this);
            Actor_SE_set(&this->actor, NA_SE_EN_RIZA_ATTACK);
            this->skelAnime.curFrame = 13.0f;
        }
    }
}

// Conditional setup function
void En_Zf_Actor_mode_side_step_init(EnZf* this, PlayState* play) {
    s16 playerRotY;
    Player* player;

    if ((this->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* not miniboss */ ||
        BG_point_check(&this->actor, play, 40.0f, (s16)(this->actor.shape.rot.y + 0x3FFF)) ||
        BG_point_check(&this->actor, play, -40.0f, (s16)(this->actor.shape.rot.y + 0x3FFF))) {
        Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gZfSidesteppingAnim);
        player = GET_PLAYER(play);
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 4000, 1);
        playerRotY = player->actor.shape.rot.y;

        if (sin_s(playerRotY - this->actor.shape.rot.y) >= 0.0f) {
            this->actor.speed = -6.0f;
        } else if (sin_s(playerRotY - this->actor.shape.rot.y) < 0.0f) { // Superfluous check
            this->actor.speed = 6.0f;
        }

        this->unk_408 = 0.0f;
        this->hopAnimIndex = 0;
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;
        this->unk_3F0 = fqrand() * 10.0f + 5.0f;
        this->action = ENZF_ACTION_CIRCLE_AROUND_PLAYER;
        En_Zf_actor_set_process(this, En_Zf_Actor_mode_side_step);
    } else {
        En_Zf_Actor_mode_forward_init(this, play);
    }
}

void En_Zf_Actor_mode_side_step(EnZf* this, PlayState* play) {
    s16 playerRot;
    s16 phi_v0_4;
    Player* player = GET_PLAYER(play);
    s32 prevFrame;
    s32 beforeCurFrame;
    s32 absPlaySpeed;
    f32 baseRange = 0.0f;

    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 1);
    playerRot = player->actor.shape.rot.y;

    if (this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) { // miniboss
        if (this->unk_3F8) {
            this->actor.speed = -this->actor.speed;
        }
    } else {
        s16 pad;

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
            !BG_point_check(&this->actor, play, this->actor.speed, this->actor.shape.rot.y + 0x3FFF)) {
            if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
                if (this->actor.speed >= 0.0f) {
                    phi_v0_4 = this->actor.shape.rot.y + 0x3FFF;
                } else {
                    phi_v0_4 = this->actor.shape.rot.y - 0x3FFF;
                }

                phi_v0_4 = this->actor.wallYaw - phi_v0_4;
            } else {
                this->actor.speed *= -0.8f;
                phi_v0_4 = 0;
            }

            if (ABS(phi_v0_4) > 0x4000) {
                this->actor.speed *= -0.8f;
                if (this->actor.speed < 0.0f) {
                    this->actor.speed -= 0.5f;
                } else {
                    this->actor.speed += 0.5f;
                }
            }
        }
    }

    if (sin_s(playerRot - this->actor.shape.rot.y) >= 0.0f) {
        this->actor.speed += 0.125f;
    } else {
        this->actor.speed -= 0.125f;
    }

    this->actor.world.rot.y = this->actor.shape.rot.y + 0x4000;

    if (Anc_Fight_ham_Check(play, &this->actor)) {
        baseRange = 100.0f;
    }

    if (this->actor.xzDistToPlayer <= (70.0f + baseRange)) {
        add_calc(&this->unk_408, -4.0f, 1.0f, 1.5f, 0.0f);
    } else if ((90.0f + baseRange) < this->actor.xzDistToPlayer) {
        add_calc(&this->unk_408, 4.0f, 1.0f, 1.5f, 0.0f);
    } else {
        add_calc(&this->unk_408, 0.0f, 1.0f, 5.65f, 0.0f);
    }

    if ((this->unk_408 != 0.0f) && !hasi_chk2(this, play, this->unk_408)) {
        this->actor.world.pos.x += sin_s(this->actor.shape.rot.y) * this->unk_408;
        this->actor.world.pos.z += cos_s(this->actor.shape.rot.y) * this->unk_408;
    }

    if (ABS(this->actor.speed) >= ABS(this->unk_408)) {
        this->skelAnime.playSpeed = -this->actor.speed * 0.75f;
    } else if (this->skelAnime.playSpeed < 0.0f) {
        this->skelAnime.playSpeed = this->unk_408 * -0.75f;
    } else {
        this->skelAnime.playSpeed = this->unk_408 * 0.75f;
    }

    prevFrame = (s32)this->skelAnime.curFrame;
    Skeleton_Info2_anime_play(&this->skelAnime);
    beforeCurFrame = (s32)(this->skelAnime.curFrame - ABS(this->skelAnime.playSpeed));
    absPlaySpeed = (s32)(f32)ABS(this->skelAnime.playSpeed);

    this->curPlatform = tbl_p_get(&this->actor.world.pos, this->curPlatform);

    if (tbl_p_get(&player->actor.world.pos, -1) != this->curPlatform) {
        this->actor.speed = 0.0f;

        if ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ &&
            (move_flg == this->actor.params)) {
            En_Zf_Actor_mode_nige_wait_init(this);
        } else {
            En_Zf_Actor_mode_forward_init(this, play);
        }
    } else if ((this->actor.params != ENZF_TYPE_DINOLFOS) || !zf_br2(play, this)) {
        if (this->unk_3F0 == 0) {
            phi_v0_4 = player->actor.shape.rot.y - this->actor.shape.rot.y;

            phi_v0_4 = ABS(phi_v0_4);

            if (phi_v0_4 >= 0x3A98) {
                if ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) && (move_flg == this->actor.params)) {
                    En_Zf_Actor_mode_nige_wait_init(this);
                } else {
                    En_Zf_Actor_mode_wait_init(this);
                    this->unk_3F0 = fqrand() * 5.0f + 1.0f;
                }
            } else if ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) && (move_flg == this->actor.params)) {
                En_Zf_Actor_mode_nige_wait_init(this);
            } else {
                s16 pad;

                this->actor.world.rot.y = this->actor.shape.rot.y;

                if ((this->actor.xzDistToPlayer <= 100.0f) && ((play->gameplayFrames % 4) == 0) &&
                    zf_ham_chk(play, this)) {
                    En_Zf_Actor_mode_kiru_init(this);
                } else if ((this->actor.xzDistToPlayer < 280.0f) && (this->actor.xzDistToPlayer > 240.0f) &&
                           !hasi_chk(this, play, 191.9956f) && ((play->gameplayFrames % 2) == 0)) {
                    En_Zf_Actor_mode_oi_jump_init(this);
                } else {
                    En_Zf_Actor_mode_forward_init(this, play);
                }
            }
        } else {
            this->unk_3F0--;
        }
        if (prevFrame != (s32)this->skelAnime.curFrame) {
            if (((beforeCurFrame < 14) && (absPlaySpeed + prevFrame >= 16)) ||
                ((beforeCurFrame < 27) && (absPlaySpeed + prevFrame >= 29))) {
                Actor_SE_set(&this->actor, NA_SE_EN_RIZA_WALK);
            }
        }
        if ((play->gameplayFrames & 0x5F) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
        }
    }
}

void En_Zf_Actor_mode_down_init(EnZf* this) {
    Skeleton_Info2_init(&this->skelAnime, &gZfDyingAnim, 1.5f, 0.0f, Si2_anime_end_frame(&gZfDyingAnim), ANIMMODE_ONCE,
                     -4.0f);

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        ((this->actor.velocity.y == 0.0f) || (this->actor.velocity.y == -4.0f))) {
        this->actor.speed = 0.0f;
        this->hopAnimIndex = 0;
    } else {
        this->hopAnimIndex = 1;
    }

    this->action = ENZF_ACTION_DIE;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    if (move_flg != -1) {
        if (this->actor.prev != NULL) {
            ((EnZf*)this->actor.prev)->unk_3F4 = 90;

            if (this->actor.prev->colChkInfo.health < 3) {
                this->actor.prev->colChkInfo.health = 3;
            }
        } else {
            ((EnZf*)this->actor.next)->unk_3F4 = 90;

            if (this->actor.next->colChkInfo.health < 3) {
                this->actor.next->colChkInfo.health = 3;
            }
        }
    }

    move_flg_trg = 0;
    Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DEAD);
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_down);
}

void En_Zf_Actor_mode_down(EnZf* this, PlayState* play) {

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        add_calc(&this->actor.speed, 0.0f, 1.0f, 0.15f, 0.0f);
        this->hopAnimIndex = 0;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.category != ACTORCAT_PROP) {
            if ((this->actor.params >= ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* miniboss */ && (move_flg == -1)) {
                Actor_Environment_sw_On(play, this->clearFlag);
                Na_StopMiddleBossBgm();
            } else {
                move_flg = -1;
            }
            Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
        }

        if (this->alpha != 0) {
            this->actor.shape.shadowAlpha = this->alpha -= 5;

        } else {
            Actor_delete(&this->actor);
        }
    } else {
        s32 curFrame = this->skelAnime.curFrame;

        if ((curFrame == 10) || (curFrame == 18)) {
            Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DOWN);
        }
    }
}

void En_Zf_Face_To_Player(EnZf* this, PlayState* play) {
    s16 angleTemp;

    if ((this->actor.params == ENZF_TYPE_DINOLFOS) && (this->action == ENZF_ACTION_3) && (this->unk_3F4 != 0)) {
        this->headRot = sin_s(this->unk_3F4 * 1400) * 0x2AA8;
    } else {
        angleTemp = this->actor.yawTowardsPlayer;
        angleTemp -= (s16)(this->headRot + this->actor.shape.rot.y);
        this->headRotTemp = CLAMP(angleTemp, -0x7D0, 0x7D0);
        this->headRot += this->headRotTemp;
        this->headRot = CLAMP(this->headRot, -0x1CD7, 0x1CD7);
    }
}

void En_Zf_damage_proc(EnZf* this, PlayState* play) {
    s32 pad;
    s16 dropParams;

    if ((this->bodyCollider.base.acFlags & AC_HIT) && (this->action <= ENZF_ACTION_STUNNED)) {
        this->bodyCollider.base.acFlags &= ~AC_HIT;

        if (((this->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* not miniboss */ ||
             (move_flg != this->actor.params)) &&
            (this->actor.colChkInfo.damageEffect != ENZF_DMGEFF_IMMUNE)) {
            this->damageEffect = this->actor.colChkInfo.damageEffect;
            Hit_bit_set(&this->actor, &this->bodyCollider.elem, false);

            if ((this->actor.colChkInfo.damageEffect == ENZF_DMGEFF_STUN) ||
                (this->actor.colChkInfo.damageEffect == ENZF_DMGEFF_ICE)) {
                if (this->action != ENZF_ACTION_STUNNED) {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 80);
                    hp_down(&this->actor);
                    En_Zf_Actor_mode_paralyze_init(this);
                }
            } else {
                Actor_SE_set(&this->actor, NA_SE_EN_RIZA_CRY);
                Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);

                if (hp_down(&this->actor) == 0) {
                    dropParams = 0x40;
                    En_Zf_Actor_mode_down_init(this);

                    if (this->actor.params == ENZF_TYPE_DINOLFOS) {
                        dropParams = 0xE0;
                    }

                    Item_Set_Std(play, &this->actor, &this->actor.world.pos, dropParams);
                    Actor_info_finish(play, &this->actor);
                } else {
                    if ((move_flg != -1) && ((this->actor.colChkInfo.health + this->actor.colChkInfo.damage) >= 4) &&
                        (this->actor.colChkInfo.health < 4)) {
                        this->damageEffect = ENZF_DMGEFF_PROJECTILE;
                    }

                    En_Zf_Actor_mode_damage_init(this);
                }
            }
        }
    }
}

void En_Zf_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZf* this = (EnZf*)thisx;
    s32 pad2;

    En_Zf_damage_proc(this, play);
    if (this->actor.colChkInfo.damageEffect != ENZF_DMGEFF_IMMUNE) {
        this->unk_3F8 = false;
        if ((this->hopAnimIndex != 1) && (this->action != ENZF_ACTION_HOP_AWAY)) {
            if (this->actor.speed != 0.0f) {
                this->unk_3F8 = hasi_chk(this, play, this->actor.speed * 1.5f);
            }
            if (!this->unk_3F8) {
                this->unk_3F8 = hasi_chk(this, play, 0.0f);
            }
        }

        if (!this->unk_3F8) {
            Actor_position_moveF(&this->actor);
        }

        Actor_BGcheck2(play, &this->actor, 25.0f, 30.0f, 60.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                    UPDBGCHECKINFO_FLAG_4);

        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            this->hopAnimIndex = 1;
        }

        this->actionFunc(this, play);
    }

    if (this->actor.colChkInfo.health > 0) {
        if ((this->action != ENZF_ACTION_SLASH) && (this->action != ENZF_ACTION_STUNNED)) {
            En_Zf_Face_To_Player(this, play);
        }

        if ((move_flg_trg != 0) && (move_flg != this->actor.params)) {
            En_Zf_Actor_mode_nige_start_init(this, play);
            move_flg = this->actor.params;
            move_flg_trg = 0;

            if (this->actor.prev != NULL) {
                ((EnZf*)this->actor.prev)->unk_3F4 = 90;
            } else {
                ((EnZf*)this->actor.next)->unk_3F4 = 90;
            }
        }
    }

    if (this->action >= ENZF_ACTION_DIE) {
        add_calc_short_angle2(&this->headRot, 0, 1, 2000, 0);

        if (this->action <= ENZF_ACTION_HOP_AND_TAUNT) {
            if ((this->unk_3F4 == 1) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                if (this->actor.colChkInfo.health > 0) {
                    En_Zf_Actor_mode_nige_end_init(this, play);
                }
                this->unk_3F4--;
            }
        }

        if (this->unk_3F4 >= 2) {
            this->unk_3F4--;
        }
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 40.0f;

    if ((this->actor.colChkInfo.health > 0) && (this->alpha == 255)) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->bodyCollider);

        if ((this->actor.world.pos.y == this->actor.floorHeight) && (this->action <= ENZF_ACTION_DAMAGED)) {
            CollisionCheck_setOC(play, &play->colChkCtx, &this->bodyCollider.base);
        }

        if ((this->actor.params < ENZF_TYPE_LIZALFOS_MINIBOSS_A) /* not miniboss */ ||
            (move_flg != this->actor.params)) {
            if ((this->actor.colorFilterTimer == 0) || !(this->actor.colorFilterParams & 0x4000)) {
                CollisionCheck_setAC(play, &play->colChkCtx, &this->bodyCollider.base);
            }
        }
    }

    if ((this->action == ENZF_ACTION_SLASH) && (this->skelAnime.curFrame >= 14.0f) &&
        (this->skelAnime.curFrame <= 20.0f)) {
        if (!(this->swordCollider.base.atFlags & AT_BOUNCED) && !(this->swordCollider.base.acFlags & AC_HIT)) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->swordCollider.base);
        } else {
            this->swordCollider.base.atFlags &= ~AT_BOUNCED;
            this->swordCollider.base.acFlags &= ~AC_HIT;
            En_Zf_Actor_mode_hirumu_init(this);
        }
    }
}

s32 en_zf_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnZf* this = (EnZf*)thisx;

    switch (limbIndex) {
        case ENZF_LIMB_HEAD_ROOT:
            rot->y -= this->headRot;
            break;
        case ENZF_LIMB_SWORD:
            if (this->swordSheathed) {
                *dList = gZfEmptyHandDL;
            }
            break;
        case ENZF_LIMB_SCABBARD:
            if (this->swordSheathed) {
                *dList = gZfSheathedSwordDL;
            }
            break;
        default:
            break;
    }

    return false;
}

void en_zf_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    static Vec3f local_eye = { 1100.0f, -700.0f, 0.0f };
    static Vec3f local_foot = { 300.0f, 0.0f, 0.0f };
    static Vec3f sword_top = { 300.0f, -1700.0f, 0.0f }; // Sword tip?
    static Vec3f sword_root = { -600.0f, 300.0f, 0.0f };  // Sword hilt?
    static Vec3f sword_top2 = { 0.0f, 1500.0f, 0.0f };
    static Vec3f sword_root2 = { -600.0f, -3000.0f, 1000.0f };
    static Vec3f sword_top3 = { -600.0f, -3000.0f, -1000.0f };
    static Vec3f sword_root3 = { 1500.0f, -3000.0f, 0.0f };
    static Vec3f dam_root = { 0.0f, 0.0f, 0.0f };
    Vec3f sp54;
    Vec3f sp48;
    EnZf* this = (EnZf*)thisx;
    s32 bodyPart = -1;

    if (limbIndex == ENZF_LIMB_SWORD) {
        Matrix_Position(&sword_top2, &this->swordCollider.dim.quad[1]);
        Matrix_Position(&sword_root2, &this->swordCollider.dim.quad[0]);
        Matrix_Position(&sword_top3, &this->swordCollider.dim.quad[3]);
        Matrix_Position(&sword_root3, &this->swordCollider.dim.quad[2]);
        CollisionCheck_Uty_setSword4Pos(&this->swordCollider, &this->swordCollider.dim.quad[0],
                                 &this->swordCollider.dim.quad[1], &this->swordCollider.dim.quad[2],
                                 &this->swordCollider.dim.quad[3]);
        Matrix_Position(&sword_top, &sp54);
        Matrix_Position(&sword_root, &sp48);

        if (this->action == ENZF_ACTION_SLASH) {
            if (this->skelAnime.curFrame < 14.0f) {
                EffectBlure_space_add(Effect_GetEffectMemoryPointer(this->blureIndex));
            } else if (this->skelAnime.curFrame < 20.0f) {
                EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->blureIndex), &sp54, &sp48);
            }
        }
    } else {
        Actor_foot_shadow_pos_set(&this->actor, limbIndex, ENZF_LIMB_LEFT_FOOT, &local_foot, ENZF_LIMB_RIGHT_FOOT, &local_foot);
    }

    switch (limbIndex) {
        case ENZF_LIMB_LEFT_FOOT:
            Matrix_Position(&local_foot, &this->leftFootPos);
            break;
        case ENZF_LIMB_RIGHT_FOOT:
            Matrix_Position(&local_foot, &this->rightFootPos);
            break;
    }

    if (this->iceTimer != 0) {
        switch (limbIndex) {
            case ENZF_LIMB_HEAD:
                bodyPart = 0;
                break;
            case ENZF_LIMB_NECK:
                bodyPart = 1;
                break;
            case ENZF_LIMB_CHEST_ARMOR:
                bodyPart = 2;
                break;
            case ENZF_LIMB_RIGHT_FOREARM:
                bodyPart = 3;
                break;
            case ENZF_LIMB_LEFT_FOREARM:
                bodyPart = 4;
                break;
            case ENZF_LIMB_TRUNK:
                bodyPart = 5;
                break;
            case ENZF_LIMB_SWORD_ROOT:
                bodyPart = 6;
                break;
            case ENZF_LIMB_RIGHT_SHIN:
                bodyPart = 7;
                break;
            case ENZF_LIMB_LEFT_SHIN_ROOT:
                bodyPart = 8;
                break;
            default:
                break;
        }
        if (bodyPart >= 0) {
            Matrix_Position(&dam_root, &this->bodyPartsPos[bodyPart]);
        }
    }
}

static Gfx reflect_test[] = {
    gsSPTexture(0x0A00, 0x0A00, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

void En_Zf_display(Actor* thisx, PlayState* play) {
    s32 pad;
    EnZf* this = (EnZf*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_zf.c", 3533);

    Actor_HiliteReflect_set_init(thisx, play, 1);

    gSPTexture(reflect_test, IREG(0), IREG(1), 0, G_TX_RENDERTILE, G_ON);

    gSPSegment(POLY_OPA_DISP++, 0x08, reflect_test);

    if (this->alpha == 255) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->alpha);
        gSPSegment(POLY_OPA_DISP++, 0x09, &Actor_change_render_mode[2]);

        POLY_OPA_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       en_zf_display1, en_zf_display2, this, POLY_OPA_DISP);

        if (this->iceTimer != 0) {
            thisx->colorFilterTimer++;
            this->iceTimer--;

            if ((this->iceTimer % 4) == 0) {
                s32 icePosIndex = this->iceTimer >> 2;

                Effect_En_Ice_ct0(play, thisx, &this->bodyPartsPos[icePosIndex], 150, 150, 150, 250, 235,
                                               245, 255, 1.4f);
            }
            if (1) {}
        }
    } else { // fades out when dead
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
        gSPSegment(POLY_XLU_DISP++, 0x09, &Actor_change_render_mode[0]);
        POLY_XLU_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       en_zf_display1, en_zf_display2, this, POLY_XLU_DISP);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_zf.c", 3601);
}

void En_Zf_Actor_mode_side_step_init2(EnZf* this, f32 speedXZ) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gZfSidesteppingAnim, -1.0f);
    this->unk_3F0 = fqrand() * 10.0f + 8.0f;

    if (this->actor.params == ENZF_TYPE_DINOLFOS) {
        this->actor.speed = 2.0f * speedXZ;
        this->unk_3F0 /= 2;
    } else {
        this->actor.speed = speedXZ;
    }

    this->hopAnimIndex = 0;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;
    this->action = ENZF_ACTION_CIRCLE_AROUND_PLAYER;
    En_Zf_actor_set_process(this, En_Zf_Actor_mode_side_step);
}

static s32 Shot_def(PlayState* play, EnZf* this) {
    Actor* projectileActor;
    s16 yawToProjectile;
    s16 phi_t0;
    s16 phi_v1;

    projectileActor = ShotVsMyCheck(play, &this->actor, 600.0f);

    if (projectileActor != NULL) {
        yawToProjectile =
            Actor_search_actor_angleY(&this->actor, projectileActor) - (s16)(u16)(this->actor.shape.rot.y);
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;

        phi_t0 = 0;

        if (hasi_chk(this, play, -8.0f)) {
            phi_t0 = 1;
        }

        if (hasi_chk(this, play, 8.0f)) {
            phi_t0 |= 2;
        }

        this->actor.world.rot.y = this->actor.shape.rot.y;

        if ((((this->actor.xzDistToPlayer < 90.0f) || (phi_t0 == 3)) && !hasi_chk(this, play, 135.0f)) ||
            (projectileActor->id == ACTOR_ARMS_HOOK)) {
            En_Zf_Actor_mode_jump_move_init(this);
            return true;
        }

        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF;

        if (phi_t0 == 0) {
            phi_v1 = play->gameplayFrames % 2;
        } else {
            phi_v1 = phi_t0;
        }

        if ((ABS(yawToProjectile) < 0x2000) || (ABS(yawToProjectile) >= 0x6000)) {
            if (phi_v1 & 1) {
                En_Zf_Actor_mode_side_step_init2(this, 8.0f);
                return true;
            }
            En_Zf_Actor_mode_side_step_init2(this, -8.0f);
            return true;
        }
        if (ABS(yawToProjectile) < 0x5FFF) {
            if (phi_v1 & 1) {
                En_Zf_Actor_mode_side_step_init2(this, 4.0f);
                return true;
            }
            En_Zf_Actor_mode_side_step_init2(this, -4.0f);
        }
        return true;
    }
    return false;
}

s32 Shot_def2(PlayState* play, EnZf* this) {
    Actor* projectileActor;
    s16 yawToProjectile;
    s16 phi_t0;
    s16 sp1E;
    s16 sp1C = 0;

    projectileActor = ShotVsMyCheck(play, &this->actor, 600.0f);
    if (projectileActor != NULL) {
        yawToProjectile = Actor_search_actor_angleY(&this->actor, projectileActor) - (s16)(u16)this->actor.shape.rot.y;
        this->actor.world.rot.y = this->actor.shape.rot.y + 0x3FFF; // Set to move sideways

        phi_t0 = 0;

        if (hasi_chk(this, play, -70.0f)) {
            phi_t0 = 1;
        }

        if (hasi_chk(this, play, 70.0f)) {
            phi_t0 |= 2;
        }

        this->actor.speed = 0.0f;

        if ((ABS(yawToProjectile) < 0x2000) || (ABS(yawToProjectile) >= 0x6000)) {
            if (phi_t0 == 0) {
                if ((play->gameplayFrames % 2) != 0) {
                    sp1E = 6;
                } else {
                    sp1E = -6;
                }
            } else {
                switch (phi_t0) {
                    case 1:
                        sp1E = 6;
                        break;
                    case 2:
                        sp1E = -6;
                        break;
                    case 1 | 2:
                        sp1C = 5;
                        sp1E = 0;
                        break;
                }
            }
        } else if (ABS(yawToProjectile) < 0x5FFF) {
            if (phi_t0 == 0) {
                if ((play->gameplayFrames % 2) != 0) {
                    sp1E = 6;
                } else {
                    sp1E = -6;
                }
            } else {
                switch (phi_t0) {
                    case 1:
                        sp1E = 6;
                        break;
                    case 2:
                        sp1E = -6;
                        break;
                    case 1 | 2:
                        sp1C = 10;
                        sp1E = 0;
                        break;
                }
            }
        }

        this->unk_408 = sp1E;
        this->unk_40C = sp1C;
        return true;
    }
    return false;
}
