#include "z_en_ssh.h"
#include "assets/objects/object_ssh/object_ssh.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define SSH_STATE_STUNNED (1 << 0)
#define SSH_STATE_GROUND_START (1 << 2)
#define SSH_STATE_ATTACKED (1 << 3)
#define SSH_STATE_SPIN (1 << 4)

typedef enum EnSshAnimation {
    SSH_ANIM_UNK0, // Unused animation. Possibly being knocked back?
    SSH_ANIM_UP,
    SSH_ANIM_WAIT,
    SSH_ANIM_LAND,
    SSH_ANIM_DROP,
    SSH_ANIM_UNK5, // Slower version of ANIM_DROP
    SSH_ANIM_UNK6  // Faster repeating version of ANIM_UNK0
} EnSshAnimation;

void En_Ssh_Actor_ct(Actor* thisx, PlayState* play);
void En_Ssh_Actor_dt(Actor* thisx, PlayState* play);
void En_Ssh_Actor_move(Actor* thisx, PlayState* play);
void En_Ssh_Actor_draw(Actor* thisx, PlayState* play);

void ssh_wait_ground(EnSsh* this, PlayState* play);
void ssh_attack(EnSsh* this, PlayState* play);
void ssh_climb(EnSsh* this, PlayState* play);
void ssh_first_proc(EnSsh* this, PlayState* play);

#include "assets/overlays/ovl_En_St/z_en_st_shape.c"

ActorProfile En_Ssh_Profile = {
    /**/ ACTOR_EN_SSH,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SSH,
    /**/ sizeof(EnSsh),
    /**/ En_Ssh_Actor_ct,
    /**/ En_Ssh_Actor_dt,
    /**/ En_Ssh_Actor_move,
    /**/ En_Ssh_Actor_draw,
};

static ColliderCylinderInit EnSshAcInfoData = {
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

static CollisionCheckInfoInit2 SshStatusData = { 1, 0, 0, 0, MASS_IMMOVABLE };

static ColliderCylinderInit EnSshAtInfoData = {
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

static ColliderJntSphElementInit SshJntSphElemData[1] = {
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

static ColliderJntSphInit SshAllJntSphData = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(SshJntSphElemData),
    SshJntSphElemData,
};

void En_Ssh_Actor_set_process(EnSsh* this, EnSshActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static void set_blast_effect(EnSsh* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.floorHeight;
    pos.z = this->actor.world.pos.z;
    Effect_SS_Blast_sc_ct(play, &pos, &zeroVec, &zeroVec, 100, 220, 8);
}

s32 ssh_blure_ct(PlayState* play) {
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

s32 ssh_string_ct(EnSsh* this, PlayState* play) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f posB;

    posB.x = this->actor.world.pos.x;
    posB.y = this->actor.world.pos.y + 1000.0f;
    posB.z = this->actor.world.pos.z;
    if (!T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &posB, &this->ceilingPos, &poly, false, false,
                                 true, true, &bgId)) {
        return false;
    } else {
        return true;
    }
}

void ssh_blure_set(EnSsh* this) {
    Vec3f p1base = { 834.0f, 834.0f, 0.0f };
    Vec3f p2base = { 834.0f, -584.0f, 0.0f };
    Vec3f p1;
    Vec3f p2;

    p1base.x *= this->colliderScale;
    p1base.y *= this->colliderScale;
    p1base.z *= this->colliderScale;
    p2base.x *= this->colliderScale;
    p2base.y *= this->colliderScale;
    p2base.z *= this->colliderScale;
    Matrix_push();
    Matrix_Position(&p1base, &p1);
    Matrix_Position(&p2base, &p2);
    Matrix_pull();
    EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->blureIdx), &p1, &p2);
}

void ssh_blure_del(EnSsh* this) {
    EffectBlure_space_add(Effect_GetEffectMemoryPointer(this->blureIdx));
}

void ssh_collision_ct(EnSsh* this, PlayState* play) {
    ColliderCylinderInit* cylinders[6] = {
        &EnSshAcInfoData, &EnSshAcInfoData, &EnSshAcInfoData, &EnSshAtInfoData, &EnSshAtInfoData, &EnSshAtInfoData,
    };
    s32 i;
    s32 pad;

    for (i = 0; i < ARRAY_COUNT(cylinders); i++) {
        ClObjPipe_ct(play, &this->colCylinder[i]);
        ClObjPipe_set5(play, &this->colCylinder[i], &this->actor, cylinders[i]);
    }

    this->colCylinder[0].elem.acDmgInfo.dmgFlags =
        DMG_ARROW | DMG_MAGIC_FIRE | DMG_HOOKSHOT | DMG_HAMMER_SWING | DMG_EXPLOSIVE | DMG_DEKU_NUT;
    this->colCylinder[1].elem.acDmgInfo.dmgFlags =
        DMG_DEFAULT & ~(DMG_ARROW | DMG_MAGIC_FIRE | DMG_HOOKSHOT | DMG_HAMMER_SWING | DMG_EXPLOSIVE | DMG_DEKU_NUT) &
        ~(DMG_MAGIC_LIGHT | DMG_MAGIC_ICE);
    this->colCylinder[2].base.colMaterial = COL_MATERIAL_METAL;
    this->colCylinder[2].elem.acElemFlags = ACELEM_ON | ACELEM_HOOKABLE | ACELEM_NO_AT_INFO;
    this->colCylinder[2].elem.elemMaterial = ELEM_MATERIAL_UNK2;
    this->colCylinder[2].elem.acDmgInfo.dmgFlags =
        DMG_DEFAULT & ~(DMG_ARROW | DMG_MAGIC_FIRE | DMG_HOOKSHOT | DMG_HAMMER_SWING | DMG_EXPLOSIVE | DMG_DEKU_NUT);

    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(2), &SshStatusData);

    ClObjJntSph_ct(play, &this->colSph);
    ClObjJntSph_set5_nzm(play, &this->colSph, &this->actor, &SshAllJntSphData, this->colSphElements);
}

f32 ssh_anime_select(EnSsh* this, s32 animIndex) {
    AnimationHeader* animation[] = {
        &object_ssh_Anim_005BE8, &object_ssh_Anim_000304, &object_ssh_Anim_000304, &object_ssh_Anim_0055F8,
        &object_ssh_Anim_000304, &object_ssh_Anim_000304, &object_ssh_Anim_005BE8,
    };
    f32 playbackSpeed[] = { 1.0f, 4.0f, 1.0f, 1.0f, 8.0f, 6.0f, 2.0f };
    u8 mode[] = { 3, 3, 1, 3, 1, 1, 1 };
    f32 frameCount = Si2_anime_end_frame(animation[animIndex]);
    s32 pad;

    Skeleton_Info2_init(&this->skelAnime, animation[animIndex], playbackSpeed[animIndex], 0.0f, frameCount,
                     mode[animIndex], -6.0f);

    return frameCount;
}

void ssh_wait_ct(EnSsh* this) {
    ssh_anime_select(this, SSH_ANIM_WAIT);
}

void ssh_climb_ct(EnSsh* this) {
    Actor_SE_set(&this->actor, NA_SE_EN_STALTU_UP);
    ssh_anime_select(this, SSH_ANIM_UP);
}

void ssh_fall_end_ct(EnSsh* this) {
    this->actor.world.pos.y = this->floorHeightOffset + this->actor.floorHeight;
    this->animTimer = ssh_anime_select(this, SSH_ANIM_LAND);
}

void ssh_attack_ct(EnSsh* this) {
    if (this->unkTimer == 0) {
        this->animTimer = ssh_anime_select(this, SSH_ANIM_DROP);
    }
    this->actor.velocity.y = -10.0f;
}

void ssh_stop_ct(EnSsh* this) {
    if (this->stunTimer == 0) {
        this->stateFlags |= SSH_STATE_ATTACKED;
        this->stunTimer = 120;
        this->actor.colorFilterTimer = 0;
    }
}

void set_ssh_scale(EnSsh* this, f32 scale, f32 radiusMod) {
    f32 radius;
    f32 height;
    f32 yShift;
    s32 i;

    radius = this->colSph.elements[0].dim.modelSphere.radius;
    radius *= scale;
    this->colSph.elements[0].dim.modelSphere.radius = radius;

    for (i = 0; i < 6; i++) {
        yShift = this->colCylinder[i].dim.yShift;
        radius = this->colCylinder[i].dim.radius;
        height = this->colCylinder[i].dim.height;
        yShift *= scale;
        radius *= scale * radiusMod;
        height *= scale;

        this->colCylinder[i].dim.yShift = yShift;
        this->colCylinder[i].dim.radius = radius;
        this->colCylinder[i].dim.height = height;
    }
    Actor_set_scale(&this->actor, 0.04f * scale);
    this->floorHeightOffset = 40.0f * scale;
    this->colliderScale = scale * 1.5f;
}

s32 ssh_stop_sub(EnSsh* this) {
    if ((this->stunTimer == 120) && (this->stateFlags & SSH_STATE_STUNNED)) {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 200, COLORFILTER_BUFFLAG_OPA, this->stunTimer);
    }
    if (DECR(this->stunTimer) != 0) {
        add_calc_short_angle2(&this->maxTurnRate, 0x2710, 0xA, 0x3E8, 1);
        return false;
    } else {
        this->stunTimer = 0;
        this->stateFlags &= ~SSH_STATE_STUNNED;
        this->spinTimer = 0;
        if (this->swayTimer == 0) {
            this->spinTimer = 30;
        }
        Actor_SE_set(&this->actor, NA_SE_EN_STALTU_ROLL);
        Actor_SE_set(&this->actor, NA_SE_VO_ST_ATTACK);
        return true;
    }
}

static void set_default_direction(EnSsh* this, PlayState* play) {
    if (this->hitTimer != 0) {
        this->hitTimer--;
    }
    if (DECR(this->spinTimer) != 0) {
        this->actor.world.rot.y += 10000.0f * (this->spinTimer / 30.0f);
    } else if ((this->swayTimer == 0) && (this->stunTimer == 0)) {
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 4, 0x2710, 1);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void set_stop_direction(EnSsh* this, PlayState* play) {
    if ((this->swayTimer == 0) && (this->stunTimer == 0)) {
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer ^ 0x8000, 4, this->maxTurnRate, 1);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (this->stunTimer < 30) {
        if (this->stunTimer & 1) {
            this->actor.shape.rot.y += 0x7D0;
        } else {
            this->actor.shape.rot.y -= 0x7D0;
        }
    }
}

void ssh_direction_set(EnSsh* this, PlayState* play) {
    if (this->stunTimer != 0) {
        set_stop_direction(this, play);
    } else {
        set_default_direction(this, play);
    }
}

void ssh_up_down_move(EnSsh* this, PlayState* play) {
    f32 bobVel = 0.5f;

    if ((play->state.frames & 8) != 0) {
        bobVel *= -1.0f;
    }
    add_calc(&this->actor.velocity.y, bobVel, 0.4f, 1000.0f, 0.0f);
}

s32 ssh_player_search(EnSsh* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 yDist;

    if (this->stateFlags & SSH_STATE_GROUND_START) {
        return true;
    }
    if (this->unkTimer != 0) {
        return true;
    }
    if (this->swayTimer != 0) {
        return true;
    }
    if (this->animTimer != 0) {
        return true;
    }

    if (this->actor.xzDistToPlayer > 160.0f) {
        return false;
    }

    yDist = this->actor.world.pos.y - player->actor.world.pos.y;
    if (yDist < 0.0f || yDist > 400.0f) {
        return false;
    }

    if (player->actor.world.pos.y < this->actor.floorHeight) {
        return false;
    }
    return true;
}

s32 ssh_climb_end_check(EnSsh* this) {
    f32 vel = this->actor.velocity.y;
    f32 nextY = this->actor.world.pos.y + 2.0f * this->actor.velocity.y;

    if (nextY >= this->actor.home.pos.y) {
        return 1;
    }
    return 0;
}

s32 ssh_fall_end_check(EnSsh* this) {
    f32 vel = this->actor.velocity.y;
    f32 nextY = this->actor.world.pos.y + 2.0f * this->actor.velocity.y;

    if ((nextY - this->actor.floorHeight) <= this->floorHeightOffset) {
        return 1;
    }
    return 0;
}

void ssh_swing(EnSsh* this) {
    Vec3f swayVecBase;
    Vec3f swayVec;
    f32 temp;
    s16 swayAngle;

    if (this->swayTimer != 0) {
        this->swayAngle += 0x640;
        this->swayTimer--;
        if (this->swayTimer == 0) {
            this->swayAngle = 0;
        }
        temp = this->swayTimer * (1.0f / 6);
        swayAngle = temp * (0x10000 / 360.0f) * sin_s(this->swayAngle);
        temp = this->actor.world.pos.y - this->ceilingPos.y;
        swayVecBase.x = sin_s(swayAngle) * temp;
        swayVecBase.y = cos_s(swayAngle) * temp;
        swayVecBase.z = 0.0f;
        Matrix_push();
        Matrix_translate(this->ceilingPos.x, this->ceilingPos.y, this->ceilingPos.z, MTXMODE_NEW);
        Matrix_rotateY(BINANG_TO_RAD(this->actor.world.rot.y), MTXMODE_APPLY);
        Matrix_Position(&swayVecBase, &swayVec);
        Matrix_pull();
        this->actor.shape.rot.z = -(swayAngle * 2);
        this->actor.world.pos.x = swayVec.x;
        this->actor.world.pos.z = swayVec.z;
    }
}

void ssh_collision_info_change(EnSsh* this, PlayState* play) {
    ColliderElement* elem = &this->colCylinder[0].elem;
    Player* player = GET_PLAYER(play);

    if (player->unk_860 != 0) {
        elem->acDmgInfo.dmgFlags |= DMG_DEKU_STICK;
        this->colCylinder[1].elem.acDmgInfo.dmgFlags &= ~DMG_DEKU_STICK;
        this->colCylinder[2].elem.acDmgInfo.dmgFlags &= ~DMG_DEKU_STICK;
    } else {
        elem->acDmgInfo.dmgFlags &= ~DMG_DEKU_STICK;
        this->colCylinder[1].elem.acDmgInfo.dmgFlags |= DMG_DEKU_STICK;
        this->colCylinder[2].elem.acDmgInfo.dmgFlags |= DMG_DEKU_STICK;
    }
}

s32 ssh_hit_to_player(EnSsh* this, PlayState* play) {
    s32 i;
    s32 hit = false;

    if ((this->hitCount == 0) && (this->spinTimer == 0)) {
        return false;
    }
    for (i = 0; i < 3; i++) {
        if (this->colCylinder[i + 3].base.ocFlags2 & OC2_HIT_PLAYER) {
            this->colCylinder[i + 3].base.ocFlags2 &= ~OC2_HIT_PLAYER;
            hit = true;
        }
    }
    if (!hit) {
        return false;
    }
    this->hitTimer = 30;
    if (this->swayTimer == 0) {
        this->spinTimer = this->hitTimer;
    }
    Actor_SE_set(&this->actor, NA_SE_EN_STALTU_ROLL);
    Actor_SE_set(&this->actor, NA_SE_VO_ST_ATTACK);
    play->damagePlayer(play, -8);
    Actor_player_power_damage_set(play, &this->actor, 4.0f, this->actor.yawTowardsPlayer, 6.0f);
    this->hitCount--;
    return true;
}

s32 ssh_hit_to_shield(EnSsh* this) {
    u32 acFlags;

    if (this->colCylinder[2].base.acFlags) {} // Needed for matching
    acFlags = this->colCylinder[2].base.acFlags;

    if (!!(acFlags & AC_HIT) == 0) {
        return 0;
    } else {
        this->colCylinder[2].base.acFlags &= ~AC_HIT;
        this->invincibilityTimer = 8;
        if ((this->swayTimer == 0) && (this->hitTimer == 0) && (this->stunTimer == 0)) {
            this->swayTimer = 60;
        }
        return 1;
    }
}

s32 ssh_hit_to_weapon(EnSsh* this, PlayState* play) {
    ColliderCylinder* cyl = &this->colCylinder[0];
    s32 hit = false;

    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
    }
    cyl = &this->colCylinder[1];
    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
    }
    if (!hit) {
        return false;
    }
    this->invincibilityTimer = 8;
    if (this->hitCount <= 0) {
        this->hitCount++;
    }
    if (this->stunTimer == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
        Actor_SE_set(&this->actor, NA_SE_VO_ST_DAMAGE);
    }
    ssh_stop_ct(this);
    this->stateFlags |= SSH_STATE_STUNNED;
    return false;
}

static s32 hitcheck(EnSsh* this, PlayState* play) {
    if (this->stunTimer == 0) {
        ssh_hit_to_player(this, play);
    }
    if (ssh_hit_to_shield(this)) {
        return false;
    } else if (play->actorCtx.unk_02 != 0) {
        this->invincibilityTimer = 8;
        if (this->stunTimer == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
            Actor_SE_set(&this->actor, NA_SE_VO_ST_DAMAGE);
        }
        ssh_stop_ct(this);
        this->stateFlags |= SSH_STATE_STUNNED;
        return false;
    } else {
        return ssh_hit_to_weapon(this, play);
        // Always returns false
    }
}

void ssh_pipe_AC_collision_set1(EnSsh* this, PlayState* play) {
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colCylinder[0]);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->colCylinder[0].base);
}

void ssh_pipe_AC_collision_set2(EnSsh* this, PlayState* play) {
    s16 angleTowardsLink = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

    if (angleTowardsLink < 90 * (0x10000 / 360)) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colCylinder[2]);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colCylinder[2].base);
    } else {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colCylinder[1]);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colCylinder[1].base);
    }
}

s32 ssh_pipe_OC_collision_set(EnSsh* this, PlayState* play) {
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
    return 1;
}

void ssh_collision_set(EnSsh* this, PlayState* play) {
    if (this->actor.colChkInfo.health == 0) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colSph.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colSph.base);
    } else {
        if (this->hitTimer == 0) {
            ssh_pipe_OC_collision_set(this, play);
        }
        if (DECR(this->invincibilityTimer) == 0) {
            ssh_pipe_AC_collision_set1(this, play);
            ssh_pipe_AC_collision_set2(this, play);
        }
    }
}

void En_Ssh_Actor_ct(Actor* thisx, PlayState* play) {
    f32 frameCount;
    s32 pad;
    EnSsh* this = (EnSsh*)thisx;

    frameCount = Si2_anime_end_frame(&object_ssh_Anim_000304);
    if (this->actor.params == ENSSH_FATHER) {
        if (z_common_data.save.info.inventory.gsTokens >= 100) {
            Actor_delete(&this->actor);
            return;
        }
    } else if (z_common_data.save.info.inventory.gsTokens >= (this->actor.params * 10)) {
        Actor_delete(&this->actor);
        return;
    }
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &object_ssh_Skel_0052E0, NULL, this->jointTable, this->morphTable, 30);
    Skeleton_Info2_init(&this->skelAnime, &object_ssh_Anim_000304, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP_INTERP, 0.0f);
    this->blureIdx = ssh_blure_ct(play);
    ssh_collision_ct(this, play);
    this->stateFlags = 0;
    this->hitCount = 0;
    ssh_string_ct(this, play);
    if (this->actor.params != ENSSH_FATHER) {
        set_ssh_scale(this, 0.5f, 1.0f);
    } else {
        set_ssh_scale(this, 0.75f, 1.0f);
    }
    this->actor.gravity = 0.0f;
    this->initialYaw = this->actor.world.rot.y;
    En_Ssh_Actor_set_process(this, ssh_first_proc);
}

void En_Ssh_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSsh* this = (EnSsh*)thisx;
    s32 i;

    EffectFreeIndex(play, this->blureIdx);
    for (i = 0; i < 6; i++) {
        ClObjPipe_dt(play, &this->colCylinder[i]);
    }
    ClObjJntSph_dt_nzf(play, &this->colSph);
}

void ssh_wait_roof(EnSsh* this, PlayState* play) {
    if (ssh_player_search(this, play)) {
        ssh_attack_ct(this);
        En_Ssh_Actor_set_process(this, ssh_attack);
    } else {
        ssh_up_down_move(this, play);
    }
}

void ssh_talk_ground(EnSsh* this, PlayState* play) {
    ssh_up_down_move(this, play);
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = ssh_wait_ground;
    }
}

void ssh_wait_ground(EnSsh* this, PlayState* play) {
    if (1) {}
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = ssh_talk_ground;
        if (this->actor.params == ENSSH_FATHER) {
            SET_EVENTCHKINF(EVENTCHKINF_96);
        }
        if ((this->actor.textId == 0x26) || (this->actor.textId == 0x27)) {
            SET_INFTABLE(INFTABLE_196);
        }
        if ((this->actor.textId == 0x24) || (this->actor.textId == 0x25)) {
            SET_INFTABLE(INFTABLE_197);
        }
    } else {
        if ((this->unkTimer != 0) && (DECR(this->unkTimer) == 0)) {
            ssh_anime_select(this, SSH_ANIM_WAIT);
        }
        if ((this->animTimer != 0) && (DECR(this->animTimer) == 0)) {
            ssh_anime_select(this, SSH_ANIM_WAIT);
        }
        if (!ssh_player_search(this, play)) {
            ssh_climb_ct(this);
            En_Ssh_Actor_set_process(this, ssh_climb);
        } else {
            if (DECR(this->sfxTimer) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_STALTU_LAUGH);
                this->sfxTimer = 64;
            }
            ssh_up_down_move(this, play);
            if ((this->unkTimer == 0) && (this->animTimer == 0)) {
                this->actor.textId = get_mask_message(play, MASK_REACTION_SET_CURSED_SKULLTULA_MAN);
                if (this->actor.textId == 0) {
                    if (this->actor.params == ENSSH_FATHER) {
                        if (z_common_data.save.info.inventory.gsTokens >= 50) {
                            this->actor.textId = 0x29;
                        } else if (z_common_data.save.info.inventory.gsTokens >= 10) {
                            if (GET_INFTABLE(INFTABLE_197)) {
                                this->actor.textId = 0x24;
                            } else {
                                this->actor.textId = 0x25;
                            }
                        } else {
                            if (GET_INFTABLE(INFTABLE_196)) {
                                this->actor.textId = 0x27;
                            } else {
                                this->actor.textId = 0x26;
                            }
                        }
                    } else {
                        this->actor.textId = 0x22;
                    }
                }
                Actor_talk_request2(&this->actor, play, 100.0f);
            }
        }
    }
}

void ssh_fall_end(EnSsh* this, PlayState* play) {
    if ((this->unkTimer != 0) && (DECR(this->unkTimer) == 0)) {
        ssh_anime_select(this, SSH_ANIM_WAIT);
    }
    if ((this->animTimer != 0) && (DECR(this->animTimer) == 0)) {
        ssh_anime_select(this, SSH_ANIM_WAIT);
    }
    if ((this->actor.floorHeight + this->floorHeightOffset) <= this->actor.world.pos.y) {
        En_Ssh_Actor_set_process(this, ssh_wait_ground);
    } else {
        add_calc(&this->actor.velocity.y, 2.0f, 0.6f, 1000.0f, 0.0f);
    }
}

void ssh_attack(EnSsh* this, PlayState* play) {
    if ((this->unkTimer != 0) && (DECR(this->unkTimer) == 0)) {
        ssh_anime_select(this, SSH_ANIM_DROP);
    }
    if (!ssh_player_search(this, play)) {
        ssh_climb_ct(this);
        En_Ssh_Actor_set_process(this, ssh_climb);
    } else if (ssh_fall_end_check(this)) {
        set_blast_effect(this, play);
        ssh_fall_end_ct(this);
        En_Ssh_Actor_set_process(this, ssh_fall_end);
    } else if (DECR(this->sfxTimer) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_STALTU_DOWN);
        this->sfxTimer = 3;
    }
}

void ssh_climb(EnSsh* this, PlayState* play) {
    f32 frameRatio = this->skelAnime.curFrame / (this->skelAnime.animLength - 1.0f);

    if (frameRatio == 1.0f) {
        ssh_climb_ct(this);
    }
    if (ssh_player_search(this, play)) {
        ssh_attack_ct(this);
        En_Ssh_Actor_set_process(this, ssh_attack);
    } else if (ssh_climb_end_check(this)) {
        ssh_wait_ct(this);
        En_Ssh_Actor_set_process(this, ssh_wait_roof);
    } else {
        this->actor.velocity.y = 4.0f * frameRatio;
    }
}

void local_set_attack_area(EnSsh* this) {
    if (this->stateFlags & SSH_STATE_SPIN) {
        if (this->spinTimer == 0) {
            this->stateFlags &= ~SSH_STATE_SPIN;
            if (this->actor.params != ENSSH_FATHER) {
                set_ssh_scale(this, 0.5f, 1.0f);
            } else {
                set_ssh_scale(this, 0.75f, 1.0f);
            }
        }
    } else {
        if (this->spinTimer != 0) {
            this->stateFlags |= SSH_STATE_SPIN;
            if (this->actor.params != ENSSH_FATHER) {
                set_ssh_scale(this, 0.5f, 2.0f);
            } else {
                set_ssh_scale(this, 0.75f, 2.0f);
            }
        }
    }
}

void ssh_first_proc(EnSsh* this, PlayState* play) {
    if (!ssh_fall_end_check(this)) {
        En_Ssh_Actor_set_process(this, ssh_wait_roof);
        ssh_wait_roof(this, play);
    } else {
        ssh_fall_end_ct(this);
        this->stateFlags |= 4;
        En_Ssh_Actor_set_process(this, ssh_fall_end);
        ssh_fall_end(this, play);
    }
}

void En_Ssh_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSsh* this = (EnSsh*)thisx;

    local_set_attack_area(this);
    if (hitcheck(this, play)) {
        return; // hitcheck always returns false, so this never happens
    }
    if (this->stunTimer != 0) {
        ssh_stop_sub(this);
    } else {
        Skeleton_Info2_anime_play(&this->skelAnime);
        Actor_position_move(&this->actor);
        Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
        this->actionFunc(this, play);
    }
    ssh_direction_set(this, play);
    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = get_random_timer(60, 60);
    }
    this->blinkState = this->blinkTimer;
    if (this->blinkState >= 3) {
        this->blinkState = 0;
    }
    ssh_collision_set(this, play);
    Actor_world_to_eye(&this->actor, 0.0f);
}

s32 en_ssh_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSsh* this = (EnSsh*)thisx;

    switch (limbIndex) {
        case 1:
            if ((this->spinTimer != 0) && (this->swayTimer == 0)) {
                if (this->spinTimer >= 2) {
                    ssh_blure_set(this);
                } else {
                    ssh_blure_del(this);
                }
            }
            break;
        case 4:
            if (this->actor.params == ENSSH_FATHER) {
                *dList = object_ssh_DL_0046C0;
            }
            break;
        case 5:
            if (this->actor.params == ENSSH_FATHER) {
                *dList = object_ssh_DL_004080;
            }
            break;
        case 8:
            if (this->actor.params == ENSSH_FATHER) {
                *dList = object_ssh_DL_004DE8;
            }
            break;
    }
    return false;
}

void en_ssh_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnSsh* this = (EnSsh*)thisx;

    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->colSph);
}

void En_Ssh_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = {
        object_ssh_Tex_0007E0,
        object_ssh_Tex_000C60,
        object_ssh_Tex_001060,
    };
    s32 pad;
    EnSsh* this = (EnSsh*)thisx;

    ssh_collision_info_change(this, play);
    ssh_swing(this);
    OPEN_DISPS(play->state.gfxCtx, "../z_en_ssh.c", 2333);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->blinkState]));
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ssh.c", 2336);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_ssh_display1,
                      en_ssh_display2, &this->actor);
}
