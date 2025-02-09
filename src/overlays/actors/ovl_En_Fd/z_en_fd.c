/*
 * File: z_en_fd.c
 * Overlay: ovl_En_Fd
 * Description: Flare Dancer (enflamed form)
 */

#include "z_en_fd.h"
#include "versions.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_fw/object_fw.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR)

void En_Fd_Actor_ct(Actor* thisx, PlayState* play);
void En_Fd_Actor_dt(Actor* thisx, PlayState* play);
void En_Fd_Actor_move(Actor* thisx, PlayState* play);
void En_Fd_Actor_draw(Actor* thisx, PlayState* play);
static void run(EnFd* this, PlayState* play);
static void turn(EnFd* this, PlayState* play);
void greeting_ct(EnFd* this, PlayState* play);
static void greeting(EnFd* this, PlayState* play);
static void jump(EnFd* this, PlayState* play);
void die(EnFd* this, PlayState* play);
void fd_eff_dust_mv(EnFd* this);
void fd_eff_hinoko_mv(EnFd* this);
void fd_eff_ct(EnFd*, u8, Vec3f*, Vec3f*, Vec3f*, u8, f32, f32);
void fd_eff_hinoko_dr(EnFd* this, PlayState* play);
void fd_eff_dust_dr(EnFd* this, PlayState* play);
void landing(EnFd* this, PlayState* play);

ActorProfile En_Fd_Profile = {
    /**/ ACTOR_EN_FD,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_FW,
    /**/ sizeof(EnFd),
    /**/ En_Fd_Actor_ct,
    /**/ En_Fd_Actor_dt,
    /**/ En_Fd_Actor_move,
    /**/ En_Fd_Actor_draw,
};

#if OOT_VERSION < NTSC_1_1
#define FLARE_DANCER_BODY_DMG 0x00
#else
#define FLARE_DANCER_BODY_DMG 0x04
#endif

static ColliderJntSphElementInit FdAllJntSphElemData[12] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040088, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 21, { { 1600, 0, 0 }, 5 }, 300 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040008, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 12, { { 1600, 0, 0 }, 5 }, 400 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040008, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 14, { { 800, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040008, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 15, { { 1600, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040008, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 16, { { 2000, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040008, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 17, { { 800, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040008, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 18, { { 1600, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040008, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 19, { { 2000, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040008, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 4, { { 2200, 0, 0 }, 4 }, 400 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040008, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 4, { { 5000, 0, 0 }, 4 }, 300 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040008, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 8, { { 2200, 0, 0 }, 4 }, 400 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x01, FLARE_DANCER_BODY_DMG },
            { 0x00040008, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 8, { { 5000, 0, 0 }, 4 }, 300 },
    },
};

static ColliderJntSphInit FdAllJntSphData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    12,
    FdAllJntSphElemData,
};

static CollisionCheckInfoInit2 FdStatusData = { 24, 2, 25, 25, MASS_IMMOVABLE };

typedef enum EnFdAnimation {
    /* 0 */ ENFD_ANIM_0,
    /* 1 */ ENFD_ANIM_1,
    /* 2 */ ENFD_ANIM_2,
    /* 3 */ ENFD_ANIM_3,
    /* 4 */ ENFD_ANIM_4
} EnFdAnimation;

static AnimationInfo anime_ct_data[] = {
    { &gFlareDancerCastingFireAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, 0.0f },
    { &gFlareDancerBackflipAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -10.0f },
    { &gFlareDancerGettingUpAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -10.0f },
    { &gFlareDancerChasingAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -10.0f },
    { &gFlareDancerTwirlAnim, 0.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -10.0f },
};

s32 set_en_fw(EnFd* this, PlayState* play) {
    if (this->invincibilityTimer != 0) {
        return false;
    }

    if (Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FW, this->corePos.x, this->corePos.y,
                           this->corePos.z, 0, this->actor.shape.rot.y, 0, this->runDir) == NULL) {
        return false;
    }

    this->actor.child->colChkInfo.health = this->actor.colChkInfo.health % 8;

    if (this->actor.child->colChkInfo.health == 0) {
        this->actor.child->colChkInfo.health = 8;
    }

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        player_hook_carry_change(play, &this->actor, this->actor.child);
    }

    this->coreActive = true;

    return true;
}

void set_en_fd_fire(EnFd* this, PlayState* play, s16 fireCnt, s16 color) {
    s32 i;

    for (i = 0; i < fireCnt; i++) {
        s16 angle = DEG_TO_BINANG((i * 360.0f) / fireCnt) + this->actor.yawTowardsPlayer;
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FD_FIRE, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, angle, 0, (color << 0xF) | i);
    }
}

void foot_eff_set(EnFd* this, PlayState* play) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };

    if (this->actionFunc == run || this->actionFunc == turn) {
        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.floorHeight + 4.0f;
        pos.z = this->actor.world.pos.z;
        accel.x = (fqrand() - 0.5f) * 2.0f;
        accel.y = ((fqrand() - 0.5f) * 0.2f) + 0.3f;
        accel.z = (fqrand() - 0.5f) * 2.0f;
        fd_eff_ct(this, FD_EFFECT_FLAME, &pos, &velocity, &accel, 8, 0.6f, 0.2f);
    }
}

/**
 * Checks to see if the hammer effect is active, and if it should be applied
 */
s32 hammer_check(EnFd* this, PlayState* play) {
    if (this->actionFunc == greeting_ct || this->actionFunc == greeting ||
        this->actionFunc == jump || this->actionFunc == die) {
        return false;
    } else if (play->actorCtx.unk_02 != 0 && this->actor.xzDistToPlayer < 300.0f && this->actor.yDistToPlayer < 60.0f) {
        return true;
    } else {
        return false;
    }
}

static s32 hitcheck(EnFd* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    ColliderElement* elem;

    if (this->collider.base.acFlags & AC_HIT || hammer_check(this, play)) {
        this->collider.base.acFlags &= ~AC_HIT;
        if (this->invincibilityTimer != 0) {
            return false;
        }
        elem = &this->collider.elements[0].base;
        if (elem->acHitElem != NULL && (elem->acHitElem->atDmgInfo.dmgFlags & DMG_HOOKSHOT)) {
            return false;
        }

        if (!set_en_fw(this, play)) {
            return false;
        }
        this->invincibilityTimer = 30;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        Actor_SE_set(&this->actor, NA_SE_EN_FLAME_DAMAGE);
        Actor_info_finish(play, &this->actor);
        return true;
    } else if (DECR(this->attackTimer) == 0 && this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        if (this->invincibilityTimer != 0) {
            return false;
        }

        if (this->collider.base.atFlags & AT_BOUNCED) {
            return false;
        }
        this->attackTimer = 30;
        Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
        Actor_player_power_damage_set(play, &this->actor, this->actor.speed + 2.0f,
                                              this->actor.yawTowardsPlayer, 6.0f);
    }
    return false;
}

/**
 * Determines if `actor` is within an acceptable range for `this` to be able to "see" `actor`
 * `actor` must be within 400 units of `this`, `actor` must be within +/- 40 degrees facing angle
 * towards `actor`, and there must not be a collision poly between `this` and `actor`
 */
s32 search_sub(EnFd* this, Actor* actor, PlayState* play) {
    CollisionPoly* colPoly;
    s32 bgId;
    Vec3f colPoint;
    s16 angle;
    s32 pad;

    // Check to see if `actor` is within 400 units of `this`
    if (search_position_distance(&this->actor.world.pos, &actor->world.pos) > 400.0f) {
        return false;
    }

    // Check to see if the angle between this facing angle and `actor` is within ~40 degrees
    angle = (f32)search_position_angleY(&this->actor.world.pos, &actor->world.pos) - this->actor.shape.rot.y;
    if (ABS(angle) > 0x1C70) {
        return false;
    }

    // check to see if the line between `this` and `actor` does not intersect a collision poly
    if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &actor->world.pos, &colPoint, &colPoly, true,
                                false, false, true, &bgId)) {
        return false;
    }

    return true;
}

Actor* bom_search(EnFd* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

    while (actor != NULL) {
        if (actor->params != 0 || actor->parent != NULL) {
            actor = actor->next;
            continue;
        }

        if (actor->id != ACTOR_EN_BOM) {
            actor = actor->next;
            continue;
        }

        if (search_sub(this, actor, play) != 1) {
            actor = actor->next;
            continue;
        }

        return actor;
    }
    return NULL;
}

Actor* fd_search(EnFd* this, PlayState* play) {
    Player* player;
    Actor* bomb = bom_search(this, play);

    if (bomb != NULL) {
        return bomb;
    }

    if (this->attackTimer != 0) {
        return NULL;
    }

    player = GET_PLAYER(play);
    if (!search_sub(this, &player->actor, play)) {
        return NULL;
    }

    return &player->actor;
}

/**
 * Creates a delta in `dst` for the position from `this`'s current position to the next
 * position in a circle formed by `radius` with center at `this`'s initial position.
 */
static Vec3f* go_to_next(Vec3f* dst, EnFd* this, f32 radius, s16 dir) {
    s16 angle;
    Vec3f newPos;

    angle = search_position_angleY(&this->actor.home.pos, &this->actor.world.pos) + (dir * 0x1554); // ~30 degrees
    newPos.x = (sin_s(angle) * radius) + this->actor.home.pos.x;
    newPos.z = (cos_s(angle) * radius) + this->actor.home.pos.z;
    newPos.x -= this->actor.world.pos.x;
    newPos.z -= this->actor.world.pos.z;
    *dst = newPos;
    return dst;
}

s32 turn_check(EnFd* this, PlayState* play, f32 radius, s16* runDir) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f colPoint;
    Vec3f pos;

    // Check to see if the next position on the rotation around the circle
    // will result in a background collision
    go_to_next(&pos, this, radius, *runDir);

    pos.x += this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y;
    pos.z += this->actor.world.pos.z;

    if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &pos, &colPoint, &poly, true, false, false, true,
                                &bgId)) {
        *runDir = -*runDir;
        return true;
    }

    if (this->circlesToComplete != 0 || DECR(this->spinTimer) != 0) {
        return false;
    }

    if (fqrand() > 0.5f) {
        *runDir = -*runDir;
    }
    return true;
}

void alpha_control(EnFd* this, PlayState* play) {
    if (this->invincibilityTimer != 0) {
        add_calc(&this->fadeAlpha, 0.0f, 0.3f, 10.0f, 0.0f);
        this->actor.shape.shadowAlpha = this->fadeAlpha;
        if (!(this->fadeAlpha >= 0.9f)) {
            this->invincibilityTimer = 0;
            this->spinTimer = 0;
            this->actionFunc = die;
            this->actor.speed = 0.0f;
        }
    }
}

void En_Fd_Actor_ct(Actor* thisx, PlayState* play) {
    EnFd* this = (EnFd*)thisx;

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gFlareDancerSkel, NULL, this->jointTable, this->morphTable, 27);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 32.0f);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &FdAllJntSphData, this->colSphs);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(0xF), &FdStatusData);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
    Actor_set_scale(&this->actor, 0.01f);
    this->firstUpdateFlag = true;
    this->actor.gravity = -1.0f;
    this->runDir = fqrand() < 0.5f ? -1 : 1;
    this->actor.naviEnemyId = NAVI_ENEMY_FLARE_DANCER;
    this->actionFunc = greeting_ct;
}

void En_Fd_Actor_dt(Actor* thisx, PlayState* play) {
    EnFd* this = (EnFd*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

void greeting_ct(EnFd* this, PlayState* play) {
    this->actor.world.pos = this->actor.home.pos;
    this->actor.params = 0;
    this->actor.shape.shadowAlpha = 0xFF;
    this->coreActive = false;
    this->actor.scale.y = 0.0f;
    this->fadeAlpha = 255.0f;
    npc_anime_ct(&this->skelAnime, anime_ct_data, ENFD_ANIM_0);
    Actor_SE_set(&this->actor, NA_SE_EN_FLAME_LAUGH);
    this->actionFunc = greeting;
}

static void greeting(EnFd* this, PlayState* play) {
    if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
        this->actor.velocity.y = 6.0f;
        this->actor.scale.y = 0.01f;
        this->actor.world.rot.y ^= 0x8000;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.speed = 8.0f;
        npc_anime_ct(&this->skelAnime, anime_ct_data, ENFD_ANIM_1);
        this->actionFunc = jump;
    } else {
        this->actor.scale.y = this->skelAnime.curFrame * (0.01f / this->skelAnime.animLength);
        this->actor.shape.rot.y += 0x2000;
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

static void jump(EnFd* this, PlayState* play) {
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && !(this->actor.velocity.y > 0.0f)) {
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        npc_anime_ct(&this->skelAnime, anime_ct_data, ENFD_ANIM_2);
        this->actionFunc = landing;
    }
}

void landing(EnFd* this, PlayState* play) {
    Vec3f adjPos;

    add_calc(&this->skelAnime.playSpeed, 1.0f, 0.1f, 1.0f, 0.0f);
    if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
        this->spinTimer = get_random_timer(60, 90);
        this->runRadius = search_position_distance(&this->actor.world.pos, &this->actor.home.pos);
        go_to_next(&adjPos, this, this->runRadius, this->runDir);
        this->actor.world.rot.y = RAD_TO_BINANG(fatan2(adjPos.x, adjPos.z));
        npc_anime_ct(&this->skelAnime, anime_ct_data, ENFD_ANIM_4);
        this->actionFunc = turn;
    }
}

static void turn(EnFd* this, PlayState* play) {
    f32 deceleration;
    f32 tgtSpeed;
    f32 rotSpeed;

    if ((this->spinTimer <= 30) && (this->invincibilityTimer == 0)) {
        Actor_level_SE_set(&this->actor, NA_SE_EN_FLAME_FIRE_ATTACK - SFX_FLAG);
    } else {
        Actor_level_SE_set(&this->actor, NA_SE_EN_FLAME_ROLL - SFX_FLAG);
    }

    if (DECR(this->spinTimer) != 0) {
        this->actor.shape.rot.y += (this->runDir * 0x2000);
        if (this->spinTimer == 30 && this->invincibilityTimer == 0) {
            if (this->actor.xzDistToPlayer > 160.0f) {
                // orange flames
                set_en_fd_fire(this, play, 8, 0);
            } else {
                // blue flames
                set_en_fd_fire(this, play, 8, 1);
            }
        }
    } else {
        // slow shape rotation down to meet `this` rotation within ~1.66 degrees
        deceleration = this->actor.world.rot.y;
        deceleration -= this->actor.shape.rot.y;
        rotSpeed = 0.0f;
        tgtSpeed = fabsf(deceleration);
        deceleration /= tgtSpeed;
        add_calc2(&rotSpeed, tgtSpeed, 0.6f, 0x2000);
        rotSpeed *= deceleration;
        this->actor.shape.rot.y += (s16)rotSpeed;
        rotSpeed = fabsf(rotSpeed);
        if ((s32)rotSpeed <= 300) {
            // ~1.6 degrees
            this->actor.shape.rot.y = this->actor.world.rot.y;
        }

        if (this->actor.shape.rot.y == this->actor.world.rot.y) {
            this->initYawToInitPos = search_position_angleY(&this->actor.home.pos, &this->actor.world.pos);
            this->curYawToInitPos = this->runDir < 0 ? 0xFFFF : 0;
            this->circlesToComplete = (play->state.frames & 7) + 2;
            this->spinTimer = get_random_timer(30, 120);
            npc_anime_ct(&this->skelAnime, anime_ct_data, ENFD_ANIM_3);
            this->actionFunc = run;
        }
    }
}

/**
 * Run around in a circle with the center being the initial position, and
 * the radius being the distance from the initial position to the nearest
 * threat (bomb or player).
 */
static void run(EnFd* this, PlayState* play) {
    Actor* potentialThreat;
    s16 yawToYawTarget;
    f32 runRadiusTarget;
    Vec3f adjPos;

    if (turn_check(this, play, this->runRadius, &this->runDir)) {
        if (this->invincibilityTimer == 0) {
            this->actor.world.rot.y ^= 0x8000;
            this->actor.velocity.y = 6.0f;
            this->actor.speed = 0.0f;
            npc_anime_ct(&this->skelAnime, anime_ct_data, ENFD_ANIM_1);
            this->actionFunc = jump;
            return;
        }
    }

    yawToYawTarget = search_position_angleY(&this->actor.home.pos, &this->actor.world.pos) - this->initYawToInitPos;
    if (this->runDir > 0) {
        if ((u16)this->curYawToInitPos > (u16)(yawToYawTarget)) {
            this->circlesToComplete--;
        }
    } else if ((u16)this->curYawToInitPos < (u16)(yawToYawTarget)) {
        this->circlesToComplete--;
    }

    if (this->circlesToComplete < 0) {
        this->circlesToComplete = 0;
    }
    this->curYawToInitPos = yawToYawTarget;

    // If there is a bomb out, or if the player exists, set radius to
    // the distance to that threat, otherwise default to 200.
    potentialThreat = fd_search(this, play);
    if ((potentialThreat != NULL) && (this->invincibilityTimer == 0)) {
        runRadiusTarget = search_position_distance(&this->actor.home.pos, &potentialThreat->world.pos);
    } else {
        runRadiusTarget = 200.0f;
    }
    add_calc(&this->runRadius, runRadiusTarget, 0.3f, 100.0f, 0.0f);
    go_to_next(&adjPos, this, this->runRadius, this->runDir);
    add_calc_short_angle2(&this->actor.shape.rot.y, RAD_TO_BINANG(fatan2(adjPos.x, adjPos.z)), 4, 0xFA0, 1);
    this->actor.world.rot = this->actor.shape.rot;
    Actor_level_SE_set(&this->actor, NA_SE_EN_FLAME_RUN - SFX_FLAG);
    if (this->skelAnime.curFrame == 6.0f || this->skelAnime.curFrame == 13.0f || this->skelAnime.curFrame == 28.0f) {
        Actor_SE_set(&this->actor, NA_SE_EN_FLAME_KICK);
    }
    add_calc(&this->actor.speed, 8.0f, 0.1f, 1.0f, 0.0f);
}

#define FLG_COREDEAD (0x4000)
#define FLG_COREDONE (0x8000)

/**
 * En_Fw will set `this` params when it is done with its action.
 * It will set FLG_COREDONE when the core has returned to `this`'s initial
 * position, and FLG_COREDEAD when there is no health left
 */
void die(EnFd* this, PlayState* play) {
    if (this->spinTimer != 0) {
        this->spinTimer--;
        if (this->spinTimer == 0) {
            Actor_delete(&this->actor);
        }
    } else if (PARAMS_GET_NOSHIFT(this->actor.params, 15, 1)) { // FLG_COREDONE
        this->actionFunc = greeting_ct;
    } else if (PARAMS_GET_NOSHIFT(this->actor.params, 14, 1)) { // FLG_COREDEAD
        this->actor.params = 0;
        this->spinTimer = 30;
    }
}

void En_Fd_Actor_move(Actor* thisx, PlayState* play) {
    EnFd* this = (EnFd*)thisx;
    s32 pad;

    if (this->firstUpdateFlag) {
        Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
        this->firstUpdateFlag = false;
    }

    if (this->actionFunc != greeting_ct) {
        Skeleton_Info2_anime_play(&this->skelAnime);
        foot_eff_set(this, play);
    }

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_HOOKSHOT_ATTACHED)) {
        if (set_en_fw(this, play)) {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->invincibilityTimer = 30;
            Actor_SE_set(&this->actor, NA_SE_EN_FLAME_DAMAGE);
            Actor_info_finish(play, &this->actor);
        } else {
            this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_ATTACHED;
        }
    } else if (this->actionFunc != die) {
        hitcheck(this, play);
    }
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    alpha_control(this, play);
    this->actionFunc(this, play);
    fd_eff_hinoko_mv(this);
    fd_eff_dust_mv(this);
    if (this->actionFunc != greeting_ct && this->actionFunc != greeting &&
        this->actionFunc != die) {
        if (this->attackTimer == 0 && this->invincibilityTimer == 0) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }

        if ((this->actionFunc == run) || (this->actionFunc == turn)) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                          Gfx** gfxP) {
    EnFd* this = (EnFd*)thisx;

    if (this->invincibilityTimer != 0) {
        switch (limbIndex) {
            case 13:
            case 21:
                *dList = NULL;
                break;
        }
    }

    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfxP) {
    EnFd* this = (EnFd*)thisx;
    Vec3f unused0 = { 6800.0f, 0.0f, 0.0f };
    Vec3f unused1 = { 6800.0f, 0.0f, 0.0f };
    Vec3f initialPos = { 0.0f, 0.0f, 0.0f };
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    s32 i;

    if (limbIndex == 21) {
        Matrix_Position(&initialPos, &this->corePos);
    }

    if (limbIndex == 13) {
        Matrix_Position(&initialPos, &this->actor.focus.pos);
    }

    if (limbIndex == 3 || limbIndex == 6 || limbIndex == 7 || limbIndex == 10 || limbIndex == 14 || limbIndex == 15 ||
        limbIndex == 17 || limbIndex == 18 || limbIndex == 20 || limbIndex == 22 || limbIndex == 23 ||
        limbIndex == 24 || limbIndex == 25 || limbIndex == 26) {
        if ((play->state.frames % 2) != 0) {
            for (i = 0; i < 1; i++) {
                Matrix_Position(&initialPos, &pos);
                pos.x += (fqrand() - 0.5f) * 20.0f;
                pos.y += (fqrand() - 0.5f) * 40.0f;
                pos.z += (fqrand() - 0.5f) * 20.0f;
                accel.x = (fqrand() - 0.5f) * 0.4f;
                accel.y = ((fqrand() - 0.5f) * 0.2f) + 0.6f;
                accel.z = (fqrand() - 0.5f) * 0.4f;
                fd_eff_ct(this, FD_EFFECT_DOT, &pos, &velocity, &accel, 0, 0.006f, 0.0f);
            }
        }
    }

    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->collider);
}

void En_Fd_Actor_draw(Actor* thisx, PlayState* play) {
    EnFd* this = (EnFd*)thisx;
    s32 clampedHealth;
    Color_RGBA8 primColors[] = {
        { 255, 255, 200, 255 },
        { 200, 200, 200, 255 },
        { 255, 255, 0, 255 },
    };
    Color_RGBA8 envColors[] = {
        { 0, 255, 0, 255 },
        { 0, 0, 255, 255 },
        { 255, 0, 0, 255 },
    };
    u32 frames;
    s32 pad;

    frames = play->state.frames;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_fd.c", 1751);

    Matrix_push();
    fd_eff_hinoko_dr(this, play);
    fd_eff_dust_dr(this, play);
    Matrix_pull();
    if (this->actionFunc != greeting_ct && !(this->fadeAlpha < 0.9f)) {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        clampedHealth = CLAMP(thisx->colChkInfo.health - 1, 0, 23) / 8;
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 128, primColors[clampedHealth].r, primColors[clampedHealth].g,
                        primColors[clampedHealth].b, (u8)this->fadeAlpha);
        gDPSetEnvColor(POLY_XLU_DISP++, envColors[clampedHealth].r, envColors[clampedHealth].g,
                       envColors[clampedHealth].b, (u8)this->fadeAlpha);
        gSPSegment(POLY_XLU_DISP++, 0x8,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1, 0,
                                    0xFF - (u8)(frames * 6), 8, 0x40));
        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x9, Actor_change_render_mode);

        POLY_XLU_DISP =
            Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               before_display, after_display, this, POLY_XLU_DISP);
    }

    if (this->fadeAlpha) {}

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_fd.c", 1822);
}

void fd_eff_ct(EnFd* this, u8 type, Vec3f* pos, Vec3f* velocity, Vec3f* accel, u8 timer, f32 scale,
                      f32 scaleStep) {
    EnFdEffect* eff = this->effects;
    s16 i;

    for (i = 0; i < EN_FD_EFFECT_COUNT; i++, eff++) {
        if (eff->type != FD_EFFECT_NONE) {
            continue;
        }
        eff->scale = scale;
        eff->scaleStep = scaleStep;
        eff->initialTimer = eff->timer = timer;
        eff->type = type;
        eff->pos = *pos;
        eff->accel = *accel;
        eff->velocity = *velocity;
        if (eff->type == FD_EFFECT_DOT) {
            eff->color.a = 255;
            eff->timer = (s16)(fqrand() * 10.0f);
        }
        return;
    }
}

void fd_eff_dust_mv(EnFd* this) {
    s16 i;
    EnFdEffect* eff = this->effects;

    for (i = 0; i < EN_FD_EFFECT_COUNT; i++, eff++) {
        if (eff->type == FD_EFFECT_FLAME) {
            eff->timer--;
            if (eff->timer == 0) {
                eff->type = FD_EFFECT_NONE;
            }
            eff->accel.x = (fqrand() * 0.4f) - 0.2f;
            eff->accel.z = (fqrand() * 0.4f) - 0.2f;
            eff->pos.x += eff->velocity.x;
            eff->pos.y += eff->velocity.y;
            eff->pos.z += eff->velocity.z;
            eff->velocity.x += eff->accel.x;
            eff->velocity.y += eff->accel.y;
            eff->velocity.z += eff->accel.z;
            eff->scale += eff->scaleStep;
        }
    }
}

void fd_eff_hinoko_mv(EnFd* this) {
    EnFdEffect* eff = this->effects;
    s16 i;
    Color_RGBA8 dotColors[] = {
        { 255, 128, 0, 0 },
        { 255, 0, 0, 0 },
        { 255, 255, 0, 0 },
        { 255, 0, 0, 0 },
    };

    for (i = 0; i < EN_FD_EFFECT_COUNT; i++, eff++) {
        if (eff->type == FD_EFFECT_DOT) {
            eff->pos.x += eff->velocity.x;
            eff->pos.y += eff->velocity.y;
            eff->pos.z += eff->velocity.z;
            eff->timer++;
            eff->velocity.x += eff->accel.x;
            eff->velocity.y += eff->accel.y;
            eff->velocity.z += eff->accel.z;

            eff->color.r = dotColors[eff->timer % 4].r;
            eff->color.g = dotColors[eff->timer % 4].g;
            eff->color.b = dotColors[eff->timer % 4].b;
            if (eff->color.a > 30) {
                eff->color.a -= 30;
            } else {
                eff->color.a = 0;
                eff->type = FD_EFFECT_NONE;
            }
        }
    }
}

void fd_eff_dust_dr(EnFd* this, PlayState* play) {
    static void* smoke_txt[] = {
        gDust8Tex, gDust7Tex, gDust6Tex, gDust5Tex, gDust4Tex, gDust3Tex, gDust2Tex, gDust1Tex,
    };
    s16 i;
    s16 idx;
    s16 pad;
    s16 materialFlag;
    EnFdEffect* eff = this->effects;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_fd.c", 1969);

    materialFlag = false;
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < EN_FD_EFFECT_COUNT; i++, eff++) {
        if (eff->type != FD_EFFECT_FLAME) {
            continue;
        }

        if (!materialFlag) {
            POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_0);
            gSPDisplayList(POLY_XLU_DISP++, gFlareDancerDL_7928);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 10, 0, (u8)((this->fadeAlpha / 255.0f) * 255));
            materialFlag = true;
        }

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, (u8)((this->fadeAlpha / 255.0f) * 255));
        gDPPipeSync(POLY_XLU_DISP++);
        Matrix_translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_fd.c", 2006);
        idx = eff->timer * (8.0f / eff->initialTimer);
        gSPSegment(POLY_XLU_DISP++, 0x8, SEGMENTED_TO_VIRTUAL(smoke_txt[idx]));
        gSPDisplayList(POLY_XLU_DISP++, gFlareDancerSquareParticleDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_fd.c", 2020);
}

void fd_eff_hinoko_dr(EnFd* this, PlayState* play) {
    s16 i;
    s16 materialFlag;
    EnFdEffect* eff = this->effects;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_fd.c", 2034);

    materialFlag = false;
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < EN_FD_EFFECT_COUNT; i++, eff++) {
        if (eff->type != FD_EFFECT_DOT) {
            continue;
        }

        if (!materialFlag) {
            _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gFlareDancerDL_79F8);
            materialFlag = true;
        }

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, eff->color.r, eff->color.g, eff->color.b,
                        (u8)(eff->color.a * (this->fadeAlpha / 255.0f)));
        gDPPipeSync(POLY_XLU_DISP++);
        Matrix_translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_fd.c", 2064);
        gSPDisplayList(POLY_XLU_DISP++, gFlareDancerTriangleParticleDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_fd.c", 2071);
}
