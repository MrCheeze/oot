/*
 * File: z_en_st.c
 * Overlay: ovl_En_St
 * Description: Skulltula (normal, big, invisible)
 */

#include "z_en_st.h"
#include "assets/objects/object_st/object_st.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_St_Actor_ct(Actor* thisx, PlayState* play);
void En_St_Actor_dt(Actor* thisx, PlayState* play);
void En_St_Actor_move(Actor* thisx, PlayState* play);
void En_St_Actor_draw(Actor* thisx, PlayState* play);
void st_climb(EnSt* this, PlayState* play);
void st_attack(EnSt* this, PlayState* play);
void st_first_proc(EnSt* this, PlayState* play);
void st_wait_ground(EnSt* this, PlayState* play);
void st_burn(EnSt* this, PlayState* play);
void st_dead(EnSt* this, PlayState* play);
void st_struggle(EnSt* this, PlayState* play);

#include "assets/overlays/ovl_En_St/z_en_st_shape.c"

ActorProfile En_St_Profile = {
    /**/ ACTOR_EN_ST,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_ST,
    /**/ sizeof(EnSt),
    /**/ En_St_Actor_ct,
    /**/ En_St_Actor_dt,
    /**/ En_St_Actor_move,
    /**/ En_St_Actor_draw,
};

static ColliderCylinderInit EnStAcInfoData = {
    {
        COL_MATERIAL_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 32, 50, -24, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 StStatusData = { 2, 0, 0, 0, MASS_IMMOVABLE };

static ColliderCylinderInit EnStAtInfoData = {
    {
        COL_MATERIAL_HIT6,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
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
    { 20, 60, -30, { 0, 0, 0 } },
};

static ColliderJntSphElementInit StJntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 1, { { 0, -240, 0 }, 28 }, 100 },
    },
};

static ColliderJntSphInit StAllJntSphData = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    StJntSphElemData,
};

typedef enum EnStAnimation {
    /* 0 */ ENST_ANIM_0,
    /* 1 */ ENST_ANIM_1,
    /* 2 */ ENST_ANIM_2,
    /* 3 */ ENST_ANIM_3,
    /* 4 */ ENST_ANIM_4,
    /* 5 */ ENST_ANIM_5,
    /* 6 */ ENST_ANIM_6,
    /* 7 */ ENST_ANIM_7
} EnStAnimation;

static AnimationInfo anime_ct_data[] = {
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, 0.0f },
    { &object_st_Anim_005B98, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -8.0f },
    { &object_st_Anim_000304, 4.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -8.0f },
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
    { &object_st_Anim_0055A8, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -8.0f },
    { &object_st_Anim_000304, 8.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
    { &object_st_Anim_000304, 6.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
    { &object_st_Anim_005B98, 2.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
};

void En_St_actor_set_process(EnSt* this, EnStActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

/**
 * Spawns `dustCnt` dust particles in a random pattern around the Skulltula
 */
static void set_dust_effect(EnSt* this, PlayState* play, s32 dustCnt) {
    Color_RGBA8 primColor = { 170, 130, 90, 255 };
    Color_RGBA8 envColor = { 100, 60, 20, 0 };
    Vec3f dustVel = { 0.0f, 0.0f, 0.0f };
    Vec3f dustAccel = { 0.0f, 0.3f, 0.0f };
    Vec3f dustPos;
    s16 yAngle;
    s32 i;

    yAngle = (fqrand() - 0.5f) * 65536.0f;
    dustPos.y = this->actor.floorHeight;
    for (i = dustCnt; i >= 0; i--, yAngle += (s16)(0x10000 / dustCnt)) {
        dustAccel.x = (fqrand() - 0.5f) * 4.0f;
        dustAccel.z = (fqrand() - 0.5f) * 4.0f;
        dustPos.x = this->actor.world.pos.x + (sin_s(yAngle) * 22.0f);
        dustPos.z = this->actor.world.pos.z + (cos_s(yAngle) * 22.0f);
        Effect_SS_Dust_sc_cl_co_ct(play, &dustPos, &dustVel, &dustAccel, &primColor, &envColor, 120, 40, 10);
    }
}

static void set_blast_effect(EnSt* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f blastPos;

    blastPos.x = this->actor.world.pos.x;
    blastPos.y = this->actor.floorHeight;
    blastPos.z = this->actor.world.pos.z;

    Effect_SS_Blast_sc_ct(play, &blastPos, &zeroVec, &zeroVec, 100, 220, 8);
}

void set_burn_effect(EnSt* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f firePos;

    firePos.x = this->actor.world.pos.x + ((fqrand() - 0.5f) * 60.0f);
    firePos.y = (this->actor.world.pos.y + 10.0f) + ((fqrand() - 0.5f) * 45.0f);
    firePos.z = this->actor.world.pos.z + ((fqrand() - 0.5f) * 60.0f);
    _Effect_SS_Db_ct(play, &firePos, &zeroVec, &zeroVec, 100, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, true);
}

s32 st_blure_ct(PlayState* play) {
    EffectBlureInit1 blureInit;
    u8 p1StartColor[] = { 255, 255, 255, 75 };
    u8 p2StartColor[] = { 255, 255, 255, 75 };
    u8 p1EndColor[] = { 255, 255, 255, 0 };
    u8 p2EndColor[] = { 255, 255, 255, 0 };
    s32 i;
    s32 blureIdx;

    for (i = 0; i < 4; i++) {
        blureInit.p1StartColor[i] = p1StartColor[i];
        blureInit.p2StartColor[i] = p2StartColor[i];
        blureInit.p1EndColor[i] = p1EndColor[i];
        blureInit.p2EndColor[i] = p2EndColor[i];
    }

    blureInit.elemDuration = 6;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 3;

    EffectAdd(play, &blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
    return blureIdx;
}

/**
 * Checks for the position of the ceiling above the Skulltula.
 * If no ceiling is found it is set to 1000 units above the Skulltula
 */
s32 st_string_ct(EnSt* this, PlayState* play) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f checkPos;

    checkPos.x = this->actor.world.pos.x;
    checkPos.y = this->actor.world.pos.y + 1000.0f;
    checkPos.z = this->actor.world.pos.z;
    if (!T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &checkPos, &this->ceilingPos, &poly, false,
                                 false, true, true, &bgId)) {
        return false;
    }
    this->unusedPos = this->actor.world.pos;
    this->unusedPos.y -= 100.0f;
    return true;
}

void st_blure_set(EnSt* this) {
    Vec3f v1 = { 834.0f, 834.0f, 0.0f };
    Vec3f v2 = { 834.0f, -584.0f, 0.0f };
    Vec3f v1Pos;
    Vec3f v2Pos;

    v1.x *= this->colliderScale;
    v1.y *= this->colliderScale;
    v1.z *= this->colliderScale;

    v2.x *= this->colliderScale;
    v2.y *= this->colliderScale;
    v2.z *= this->colliderScale;

    Matrix_push();
    Matrix_Position(&v1, &v1Pos);
    Matrix_Position(&v2, &v2Pos);
    Matrix_pull();
    EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->blureIdx), &v1Pos, &v2Pos);
}

void st_blure_del(EnSt* this) {
    EffectBlure_space_add(Effect_GetEffectMemoryPointer(this->blureIdx));
}

void st_wait_ct(EnSt* this) {
    npc_anime_ct(&this->skelAnime, anime_ct_data, ENST_ANIM_3);
}

void st_climb_ct(EnSt* this) {
    Actor_SE_set(&this->actor, NA_SE_EN_STALTU_UP);
    npc_anime_ct(&this->skelAnime, anime_ct_data, ENST_ANIM_2);
}

void st_fall_end_ct(EnSt* this) {
    this->actor.world.pos.y = this->actor.floorHeight + this->floorHeightOffset;
    npc_anime_ct(&this->skelAnime, anime_ct_data, ENST_ANIM_4);
    this->sfxTimer = 0;
    this->animFrames = this->skelAnime.animLength;
}

void st_attack_ct(EnSt* this) {
    if (this->takeDamageSpinTimer == 0) {
        npc_anime_ct(&this->skelAnime, anime_ct_data, ENST_ANIM_4);
        this->animFrames = this->skelAnime.animLength;
    }
    this->sfxTimer = 0;
    this->actor.velocity.y = -10.0f;
}

/**
 * Initializes the Skulltula's 6 cylinders, and sphere collider.
 */
void st_collision_ct(EnSt* this, PlayState* play) {
    ColliderCylinderInit* cylinders[6] = {
        &EnStAcInfoData, &EnStAcInfoData, &EnStAcInfoData, &EnStAtInfoData, &EnStAtInfoData, &EnStAtInfoData,
    };

    s32 i;
    s32 pad;

    for (i = 0; i < ARRAY_COUNT(cylinders); i++) {
        ClObjPipe_ct(play, &this->colCylinder[i]);
        ClObjPipe_set5(play, &this->colCylinder[i], &this->actor, cylinders[i]);
    }

    this->colCylinder[0].elem.acDmgInfo.dmgFlags =
        DMG_MAGIC_FIRE | DMG_ARROW | DMG_HOOKSHOT | DMG_HAMMER_SWING | DMG_BOOMERANG | DMG_EXPLOSIVE | DMG_DEKU_NUT;
    this->colCylinder[1].elem.acDmgInfo.dmgFlags =
        DMG_DEFAULT &
        ~(DMG_MAGIC_FIRE | DMG_ARROW | DMG_HOOKSHOT | DMG_HAMMER_SWING | DMG_BOOMERANG | DMG_EXPLOSIVE | DMG_DEKU_NUT) &
        ~(DMG_MAGIC_LIGHT | DMG_MAGIC_ICE);
    this->colCylinder[2].base.colMaterial = COL_MATERIAL_METAL;
    this->colCylinder[2].elem.acElemFlags = ACELEM_ON | ACELEM_HOOKABLE | ACELEM_NO_AT_INFO;
    this->colCylinder[2].elem.elemMaterial = ELEM_MATERIAL_UNK2;
    this->colCylinder[2].elem.acDmgInfo.dmgFlags =
        DMG_DEFAULT &
        ~(DMG_MAGIC_FIRE | DMG_ARROW | DMG_HOOKSHOT | DMG_HAMMER_SWING | DMG_BOOMERANG | DMG_EXPLOSIVE | DMG_DEKU_NUT);

    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(2), &StStatusData);

    ClObjJntSph_ct(play, &this->colSph);
    ClObjJntSph_set5_nzm(play, &this->colSph, &this->actor, &StAllJntSphData, this->colSphItems);
}

void st_collision_info_change(EnSt* this, PlayState* play) {
    ColliderElement* bodyElem = &this->colCylinder[0].elem;
    Player* player = GET_PLAYER(play);

    if (player->unk_860 != 0) {
        bodyElem->acDmgInfo.dmgFlags |= DMG_DEKU_STICK;
        this->colCylinder[1].elem.acDmgInfo.dmgFlags &= ~DMG_DEKU_STICK;
        this->colCylinder[2].elem.acDmgInfo.dmgFlags &= ~DMG_DEKU_STICK;
    } else {
        bodyElem->acDmgInfo.dmgFlags &= ~DMG_DEKU_STICK;
        this->colCylinder[1].elem.acDmgInfo.dmgFlags |= DMG_DEKU_STICK;
        this->colCylinder[2].elem.acDmgInfo.dmgFlags |= DMG_DEKU_STICK;
    }
}

void st_pipe_AC_collision_set1(EnSt* this, PlayState* play) {
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colCylinder[0]);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->colCylinder[0].base);
}

void st_pipe_AC_collision_set2(EnSt* this, PlayState* play) {
    s16 angleTowardsLink = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

    if (angleTowardsLink < 0x3FFC) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colCylinder[2]);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colCylinder[2].base);
    } else {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colCylinder[1]);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colCylinder[1].base);
    }
}

s32 st_pipe_OC_collision_set(EnSt* this, PlayState* play) {
    Vec3f cyloffsets[] = {
        { 40.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { -40.0f, 0.0f, 0.0f },
    };
    Vec3f cylPos;
    s32 i;

    for (i = 0; i < 3; i++) {
        cylPos = this->actor.world.pos;
        cyloffsets[i].x *= this->colliderScale;
        cyloffsets[i].y *= this->colliderScale;
        cyloffsets[i].z *= this->colliderScale;
        Matrix_push();
        Matrix_translate(cylPos.x, cylPos.y, cylPos.z, MTXMODE_NEW);
        Matrix_rotateY(BINANG_TO_RAD_ALT(this->initialYaw), MTXMODE_APPLY);
        Matrix_Position(&cyloffsets[i], &cylPos);
        Matrix_pull();
        this->colCylinder[i + 3].dim.pos.x = cylPos.x;
        this->colCylinder[i + 3].dim.pos.y = cylPos.y;
        this->colCylinder[i + 3].dim.pos.z = cylPos.z;
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colCylinder[i + 3].base);
    }

    return true;
}

void st_collision_set(EnSt* this, PlayState* play) {
    if ((this->actor.colChkInfo.health != 0) || (this->actionFunc == st_struggle)) {
        if (DECR(this->gaveDamageSpinTimer) == 0) {
            st_pipe_OC_collision_set(this, play);
        }

        DECR(this->invulnerableTimer);
        DECR(this->takeDamageSpinTimer);

        if (this->invulnerableTimer == 0 && this->takeDamageSpinTimer == 0) {
            st_pipe_AC_collision_set1(this, play);
            st_pipe_AC_collision_set2(this, play);
        }
    }
}

s32 st_hit_to_player(EnSt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 hit;
    s32 i;

    for (i = 0, hit = 0; i < 3; i++) {
        if (((this->colCylinder[i + 3].base.ocFlags2 & OC2_HIT_PLAYER) != 0) == 0) {
            continue;
        }
        this->colCylinder[i + 3].base.ocFlags2 &= ~OC2_HIT_PLAYER;
        hit = true;
    }

    if (!hit) {
        return false;
    }

    if (this->swayTimer == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_STALTU_ROLL);
    }

    this->gaveDamageSpinTimer = 30;
    play->damagePlayer(play, -8);
    Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
    Actor_player_power_damage_set(play, &this->actor, 4.0f, this->actor.yawTowardsPlayer, 6.0f);
    return true;
}

s32 st_hit_to_shield(EnSt* this) {
    u8 acFlags = this->colCylinder[2].base.acFlags;

    if (!!(acFlags & AC_HIT) == 0) {
        // not hit
        return false;
    } else {
        this->colCylinder[2].base.acFlags &= ~AC_HIT;
        this->invulnerableTimer = 8;
        this->playSwayFlag = 0;
        this->swayTimer = 60;
        return true;
    }
}

s32 st_hit_to_weapon(EnSt* this, PlayState* play) {
    ColliderCylinder* cyl = &this->colCylinder[0];
    s32 flags = 0; // damage flags from colliders 0 and 1
    s32 hit = false;

    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
        flags |= cyl->elem.acHitElem->atDmgInfo.dmgFlags;
    }

    cyl = &this->colCylinder[1];
    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
        flags |= cyl->elem.acHitElem->atDmgInfo.dmgFlags;
    }

    if (!hit) {
        return false;
    }

    this->invulnerableTimer = 8;
    if (this->actor.colChkInfo.damageEffect == 1) {
        if (this->stunTimer == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
            this->stunTimer = 120;
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 200, COLORFILTER_BUFFLAG_OPA,
                                 this->stunTimer);
        }
        return false;
    }

    this->swayTimer = this->stunTimer = 0;
    this->gaveDamageSpinTimer = 1;
    npc_anime_ct(&this->skelAnime, anime_ct_data, ENST_ANIM_3);
    this->takeDamageSpinTimer = this->skelAnime.animLength;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA,
                         this->takeDamageSpinTimer);
    if (hp_down(&this->actor)) {
        Actor_SE_set(&this->actor, NA_SE_EN_STALTU_DAMAGE);
        return false;
    }
    Actor_info_finish(play, &this->actor);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->groundBounces = 3;
    this->deathTimer = 20;
    this->actor.gravity = -1.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_STALWALL_DEAD);

    if (flags & DMG_ARROW) {
        En_St_actor_set_process(this, st_burn);
        this->finishDeathTimer = 8;
    } else {
        En_St_actor_set_process(this, st_dead);
    }

    return true;
}

/**
 * Checks if the Skulltula's colliders have been hit, returns true if the hit has dealt damage to the Skulltula
 */
static s32 hitcheck(EnSt* this, PlayState* play) {
    if (st_hit_to_shield(this)) {
        // player has hit the front shield area of the Skulltula
        return false;
    }

    if (play->actorCtx.unk_02 != 0) {
        return true;
    }

    if (st_hit_to_weapon(this, play)) {
        // player has hit the backside of the Skulltula
        return true;
    }

    if (this->stunTimer == 0 && this->takeDamageSpinTimer == 0) {
        // check if the Skulltula has hit the player.
        st_hit_to_player(this, play);
    }
    return false;
}

void set_st_scale(EnSt* this) {
    f32 scaleAmount = 1.0f;
    f32 radius;
    f32 height;
    f32 yShift;
    s32 i;

    if (this->actor.params == 1) {
        scaleAmount = 1.4f;
    }

    radius = this->colSph.elements[0].dim.modelSphere.radius;
    radius *= scaleAmount;
    this->colSph.elements[0].dim.modelSphere.radius = radius;

    for (i = 0; i < 6; i++) {
        yShift = this->colCylinder[i].dim.yShift;
        radius = this->colCylinder[i].dim.radius;
        height = this->colCylinder[i].dim.height;
        yShift *= scaleAmount;
        radius *= scaleAmount;
        height *= scaleAmount;

        this->colCylinder[i].dim.yShift = yShift;
        this->colCylinder[i].dim.radius = radius;
        this->colCylinder[i].dim.height = height;
    }
    Actor_set_scale(&this->actor, 0.04f * scaleAmount);
    this->colliderScale = scaleAmount;
    this->floorHeightOffset = 32.0f * scaleAmount;
}

s32 set_eye_color(EnSt* this, s16 redTarget, s16 greenTarget, s16 blueTarget, s16 minMaxStep) {
    s16 red = this->teethR;
    s16 green = this->teethG;
    s16 blue = this->teethB;

    minMaxStep = 255 / (s16)(0.6f * minMaxStep);
    if (minMaxStep <= 0) {
        minMaxStep = 1;
    }

    add_calc_short_angle2(&red, redTarget, 1, minMaxStep, minMaxStep);
    add_calc_short_angle2(&green, greenTarget, 1, minMaxStep, minMaxStep);
    add_calc_short_angle2(&blue, blueTarget, 1, minMaxStep, minMaxStep);
    this->teethR = red;
    this->teethG = green;
    this->teethB = blue;
    return 1;
}

s32 st_stop_sub(EnSt* this) {
    if (this->stunTimer == 0) {
        return 0;
    }
    this->stunTimer--;
    //! @bug No return, v0 ends up being stunTimer before decrement.
    //! The return value is not used so it doesn't matter.
}

/**
 * Updates the yaw of the Skulltula, used for the shaking animation right before
 * turning, and the actual turning to face away from the player, and then back to
 * face the player
 */
static void set_default_direction(EnSt* this, PlayState* play) {
    u16 yawDir = 0;
    Vec3s rot;
    s16 yawDiff;
    s16 timer;
    s16 yawTarget;

    // Shake towards the end of the stun.
    if (this->stunTimer != 0) {
        if (this->stunTimer < 30) {
            if ((this->stunTimer % 2) != 0) {
                this->actor.shape.rot.y += 0x800;
            } else {
                this->actor.shape.rot.y -= 0x800;
            }
        }
        return;
    }

    if (this->swayTimer == 0 && this->deathTimer == 0 && this->finishDeathTimer == 0) {
        // not swaying or dying
        if (this->takeDamageSpinTimer != 0 || this->gaveDamageSpinTimer != 0) {
            // Skulltula is doing a spinning animation
            this->actor.shape.rot.y += 0x2000;
            return;
        }

        if (this->actionFunc != st_wait_ground) {
            // set the timers to turn away or turn towards the player
            this->rotAwayTimer = 30;
            this->rotTowardsTimer = 0;
        }

        if (this->rotAwayTimer != 0) {
            // turn away from the player
            this->rotAwayTimer--;
            if (this->rotAwayTimer == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_STALTU_ROLL);
                this->rotTowardsTimer = 30;
            }
        } else if (this->rotTowardsTimer != 0) {
            // turn towards the player
            this->rotTowardsTimer--;
            if (this->rotTowardsTimer == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_STALTU_ROLL);
                this->rotAwayTimer = 30;
            }
            yawDir = 0x8000;
        }

        // calculate the new yaw to or away from the player.
        rot = this->actor.shape.rot;
        yawTarget = (this->actionFunc == st_wait_ground ? this->actor.yawTowardsPlayer : this->initialYaw);
        yawDiff = rot.y - (yawTarget ^ yawDir);
        if (ABS(yawDiff) <= 0x4000) {
            add_calc_short_angle2(&rot.y, yawTarget ^ yawDir, 4, 0x2000, 1);
        } else {
            rot.y += 0x2000;
        }

        this->actor.shape.rot = this->actor.world.rot = rot;

        // Do the shaking animation.
        if (yawDir == 0 && this->rotAwayTimer < 0xA) {
            timer = this->rotAwayTimer;
        } else if (yawDir == 0x8000 && this->rotTowardsTimer < 0xA) {
            timer = this->rotTowardsTimer;
        } else {
            return;
        }

        if ((timer % 2) != 0) {
            this->actor.shape.rot.y += 0x800;
        } else {
            this->actor.shape.rot.y -= 0x800;
        }
    }
}

/**
 * Checks to see if the Skulltula is done bouncing on the ground,
 * spawns dust particles as the Skulltula hits the ground
 */
static s32 bound(EnSt* this, PlayState* play) {
    if (this->actor.velocity.y > 0.0f || this->groundBounces == 0) {
        // the Skulltula is moving upwards or the groundBounces is 0
        return false;
    }

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        // the Skulltula is not on the ground.
        return false;
    }

    Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
    set_dust_effect(this, play, 10);
    // creates an elastic bouncing effect, boucing up less for each hit on the ground.
    this->actor.velocity.y = 6.0f / (4 - this->groundBounces);
    this->groundBounces--;
    if (this->groundBounces != 0) {
        return false;
    } else {
        // make sure the Skulltula stays on the ground.
        this->actor.velocity.y = 0.0f;
    }
    return true;
}

void st_up_down_move(EnSt* this, PlayState* play) {
    f32 ySpeedTarget = 0.5f;

    if ((play->state.frames & 8) != 0) {
        ySpeedTarget *= -1.0f;
    }
    add_calc(&this->actor.velocity.y, ySpeedTarget, 0.4f, 1000.0f, 0.0f);
}

s32 st_player_search(EnSt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 yDist;

    if (this->takeDamageSpinTimer != 0) {
        // skull is spinning from damage.
        return false;
    } else if (this->actor.xzDistToPlayer > 160.0f) {
        // player is more than 160 xz units from the Skulltula
        return false;
    }

    yDist = this->actor.world.pos.y - player->actor.world.pos.y;
    if (yDist < 0.0f || yDist > 400.0f) {
        // player is above the Skulltula or more than 400 units below
        // the Skulltula
        return false;
    }

    if (player->actor.world.pos.y < this->actor.floorHeight) {
        // player is below the Skulltula's ground position
        return false;
    }
    return true;
}

s32 st_climb_end_check(EnSt* this) {
    f32 velY = this->actor.velocity.y;
    f32 checkY = this->actor.world.pos.y + (velY * 2.0f);

    if (checkY >= this->actor.home.pos.y) {
        return true;
    }
    return false;
}

s32 st_fall_end_check(EnSt* this) {
    f32 velY = this->actor.velocity.y;
    f32 checkY = this->actor.world.pos.y + (velY * 2.0f);

    if (checkY - this->actor.floorHeight <= this->floorHeightOffset) {
        return true;
    }
    return false;
}

/**
 * Does the animation of the Skulltula swaying back and forth after the Skulltula
 * has been hit in the front by a sword
 */
void st_swing(EnSt* this) {
    Vec3f amtToTranslate;
    Vec3f translatedPos;
    f32 swayAmt;
    s16 rotAngle;

    if (this->swayTimer != 0) {

        this->swayAngle += 0xA28;
        this->swayTimer--;

        if (this->swayTimer == 0) {
            this->swayAngle = 0;
        }

        swayAmt = this->swayTimer * (7.0f / 15.0f);
        rotAngle = sin_s(this->swayAngle) * (swayAmt * (65536.0f / 360.0f));

        if (this->absPrevSwayAngle >= ABS(rotAngle) && this->playSwayFlag == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_STALTU_WAVE);
            this->playSwayFlag = 1;
        }

        if (this->absPrevSwayAngle < ABS(rotAngle)) {
            this->playSwayFlag = 0;
        }

        this->absPrevSwayAngle = ABS(rotAngle);
        amtToTranslate.x = sin_s(rotAngle) * -200.0f;
        amtToTranslate.y = cos_s(rotAngle) * -200.0f;
        amtToTranslate.z = 0.0f;
        Matrix_push();
        Matrix_translate(this->ceilingPos.x, this->ceilingPos.y, this->ceilingPos.z, MTXMODE_NEW);
        Matrix_rotateY(BINANG_TO_RAD(this->actor.world.rot.y), MTXMODE_APPLY);
        Matrix_Position(&amtToTranslate, &translatedPos);
        Matrix_pull();
        this->actor.shape.rot.z = -(rotAngle * 2);
        this->actor.world.pos.x = translatedPos.x;
        this->actor.world.pos.z = translatedPos.z;
    }
}

void En_St_Actor_ct(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 14.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &object_st_Skel_005298, NULL, this->jointTable, this->morphTable, 30);
    npc_anime_ct(&this->skelAnime, anime_ct_data, ENST_ANIM_0);
    this->blureIdx = st_blure_ct(play);
    st_collision_ct(this, play);
    if (thisx->params == 2) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
    }
    if (this->actor.params == 1) {
        this->actor.naviEnemyId = NAVI_ENEMY_BIG_SKULLTULA;
    } else {
        this->actor.naviEnemyId = NAVI_ENEMY_SKULLTULA;
    }
    st_string_ct(this, play);
    this->actor.flags |= ACTOR_FLAG_CAN_ATTACH_TO_ARROW;
    this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
    set_st_scale(this);
    this->actor.gravity = 0.0f;
    this->initialYaw = this->actor.world.rot.y;
    En_St_actor_set_process(this, st_first_proc);
}

void En_St_Actor_dt(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;
    s32 i;

    EffectFreeIndex(play, this->blureIdx);
    for (i = 0; i < 6; i++) {
        ClObjPipe_dt(play, &this->colCylinder[i]);
    }
    ClObjJntSph_dt_nzf(play, &this->colSph);
}

void st_wait_roof(EnSt* this, PlayState* play) {
    if (st_player_search(this, play)) {
        st_attack_ct(this);
        En_St_actor_set_process(this, st_attack);
    } else {
        st_up_down_move(this, play);
    }
}

/**
 * Skulltula is waiting on the ground for the player to move away, or for
 * a collider to have contact
 */
void st_wait_ground(EnSt* this, PlayState* play) {
    if (this->takeDamageSpinTimer != 0) {
        this->takeDamageSpinTimer--;
        if (this->takeDamageSpinTimer == 0) {
            npc_anime_ct(&this->skelAnime, anime_ct_data, ENST_ANIM_3);
        }
    }

    if (this->animFrames != 0) {
        this->animFrames--;
        if (this->animFrames == 0) {
            npc_anime_ct(&this->skelAnime, anime_ct_data, ENST_ANIM_3);
        }
    }

    if (!st_player_search(this, play)) {
        // Player is no longer within range, return to ceiling.
        st_climb_ct(this);
        En_St_actor_set_process(this, st_climb);
        return;
    }

    if (DECR(this->sfxTimer) == 0) {
        // play the "laugh" sfx every 64 frames.
        Actor_SE_set(&this->actor, NA_SE_EN_STALTU_LAUGH);
        this->sfxTimer = 64;
    }

    // simply bob up and down.
    st_up_down_move(this, play);
}

void st_fall_end(EnSt* this, PlayState* play) {
    if (this->animFrames != 0) {
        this->animFrames--;
        if (this->animFrames == 0) {
            npc_anime_ct(&this->skelAnime, anime_ct_data, ENST_ANIM_3);
        }
    }

    if (this->takeDamageSpinTimer != 0) {
        this->takeDamageSpinTimer--;
        if (this->takeDamageSpinTimer == 0) {
            npc_anime_ct(&this->skelAnime, anime_ct_data, ENST_ANIM_3);
        }
    }

    this->sfxTimer++;
    if (this->sfxTimer == 14) {
        // play the sound effect of the Skulltula hitting the ground.
        Actor_SE_set(&this->actor, NA_SE_EN_STALTU_DOWN_SET);
    }

    if ((this->actor.floorHeight + this->floorHeightOffset) < this->actor.world.pos.y) {
        // the Skulltula has hit the ground.
        this->sfxTimer = 0;
        En_St_actor_set_process(this, st_wait_ground);
    } else {
        add_calc(&this->actor.velocity.y, 2.0f, 0.3f, 1.0f, 0.0f);
    }
}

void st_attack(EnSt* this, PlayState* play) {
    if (this->takeDamageSpinTimer != 0) {
        this->takeDamageSpinTimer--;
        if (this->takeDamageSpinTimer == 0) {
            npc_anime_ct(&this->skelAnime, anime_ct_data, ENST_ANIM_5);
        }
    }

    if (!st_player_search(this, play)) {
        // the player moved out of range, return to the ceiling.
        st_climb_ct(this);
        En_St_actor_set_process(this, st_climb);
    } else if (st_fall_end_check(this)) {
        // The Skulltula has become close to the ground.
        set_blast_effect(this, play);
        st_fall_end_ct(this);
        En_St_actor_set_process(this, st_fall_end);
    } else if (DECR(this->sfxTimer) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_STALTU_DOWN);
        this->sfxTimer = 3;
    }
}

void st_climb(EnSt* this, PlayState* play) {
    f32 animPctDone = this->skelAnime.curFrame / (this->skelAnime.animLength - 1.0f);

    if (animPctDone == 1.0f) {
        st_climb_ct(this);
    }

    if (st_player_search(this, play)) {
        // player came back into range
        st_attack_ct(this);
        En_St_actor_set_process(this, st_attack);
    } else if (st_climb_end_check(this)) {
        // the Skulltula is close to the initial postion.
        st_wait_ct(this);
        En_St_actor_set_process(this, st_wait_roof);
    } else {
        // accelerate based on the current animation frame.
        this->actor.velocity.y = 4.0f * animPctDone;
    }
}

/**
 * The Skulltula has been killed, bounce around
 */
void st_dead(EnSt* this, PlayState* play) {
    this->actor.colorFilterTimer = this->deathTimer;
    Actor_position_speed_set(&this->actor);
    this->actor.world.rot.x += 0x800;
    this->actor.world.rot.z -= 0x800;
    this->actor.shape.rot = this->actor.world.rot;
    if (bound(this, play)) {
        this->actor.shape.yOffset = 400.0f;
        this->actor.speed = 1.0f;
        this->actor.gravity = -2.0f;
        En_St_actor_set_process(this, st_struggle);
    } else {
        add_calc(&this->actor.shape.yOffset, 400.0f, 0.4f, 10000.0f, 0.0f);
    }
}

/**
 * Finish up the bouncing animation, and rotate towards the final position
 */
void st_struggle(EnSt* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (DECR(this->deathTimer) == 0) {
        this->actor.velocity = zeroVec;
        this->finishDeathTimer = 8;
        En_St_actor_set_process(this, st_burn);
        return;
    }

    if (DECR(this->setTargetYawTimer) == 0) {
        this->deathYawTarget = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);
        this->setTargetYawTimer = 8;
    }

    add_calc_short_angle2(&this->actor.world.rot.x, 0x3FFC, 4, 0x2710, 1);
    add_calc_short_angle2(&this->actor.world.rot.z, 0, 4, 0x2710, 1);
    add_calc_short_angle2(&this->actor.world.rot.y, this->deathYawTarget, 0xA, 0x2710, 1);

    this->actor.shape.rot = this->actor.world.rot;

    Actor_position_speed_set(&this->actor);
    this->groundBounces = 2;
    bound(this, play);
}

/**
 * Spawn the enemy dying effects, and drop a random item
 */
void st_burn(EnSt* this, PlayState* play) {
    if (DECR(this->finishDeathTimer) != 0) {
        set_burn_effect(this, play);
    } else {
        Item_Set_Std(play, NULL, &this->actor.world.pos, 0xE0);
        Actor_delete(&this->actor);
    }
}

void st_first_proc(EnSt* this, PlayState* play) {
    if (!st_fall_end_check(this)) {
        this->rotAwayTimer = 60;
        En_St_actor_set_process(this, st_wait_roof);
        st_wait_roof(this, play);
    } else {
        st_fall_end_ct(this);
        En_St_actor_set_process(this, st_fall_end);
        st_fall_end(this, play);
    }
}

void En_St_Actor_move(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;
    s32 pad;
    Color_RGBA8 color = { 0, 0, 0, 0 };

    if (this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW) {
        Skeleton_Info2_anime_play(&this->skelAnime);
    } else if (!hitcheck(this, play)) {
        // no collision has been detected.

        if (this->stunTimer == 0) {
            Skeleton_Info2_anime_play(&this->skelAnime);
        }

        if (this->swayTimer == 0 && this->stunTimer == 0) {
            Actor_position_move(&this->actor);
        }

        Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

        if ((this->stunTimer == 0) && (this->swayTimer == 0)) {
            // run the current action if the Skulltula isn't stunned
            // or swaying.
            this->actionFunc(this, play);
        } else if (this->stunTimer != 0) {
            // decrement the stun timer.
            st_stop_sub(this);
        } else {
            // sway the Skulltula.
            st_swing(this);
        }

        set_default_direction(this, play);

        if (this->actionFunc == st_wait_ground) {
            if ((play->state.frames & 0x10) != 0) {
                color.r = 255;
            }
        }

        set_eye_color(this, color.r, color.g, color.b, 8);
        st_collision_set(this, play);
        Actor_world_to_eye(&this->actor, 0.0f);
    }
}

s32 en_st_display1(PlayState* play, s32 limbIndex, Gfx** dListP, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSt* this = (EnSt*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_st.c", 2260);
    switch (limbIndex) {
        case 1:
            if (this->gaveDamageSpinTimer != 0 && this->swayTimer == 0) {
                if (this->gaveDamageSpinTimer >= 2) {
                    st_blure_set(this);
                } else {
                    st_blure_del(this);
                }
            }
            break;
        case 4:
            // teeth
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, this->teethR, this->teethG, this->teethB, 0);
            break;
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_st.c", 2295);
    return false;
}

void en_st_display2(PlayState* play, s32 limbIndex, Gfx** dListP, Vec3s* rot, void* thisx) {
    EnSt* this = (EnSt*)thisx;

    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->colSph);
}

void En_St_Actor_draw(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;

    st_collision_info_change(this, play);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_st_display1,
                      en_st_display2, this);
}
