#include "z_en_bubble.h"
#include "assets/objects/object_bubble/object_bubble.h"

#define FLAGS ACTOR_FLAG_ATTENTION_ENABLED

void En_Bubble_Actor_ct(Actor* thisx, PlayState* play);
void En_Bubble_Actor_dt(Actor* thisx, PlayState* play);
void En_Bubble_Actor_move(Actor* thisx, PlayState* play);
void En_Bubble_Actor_draw(Actor* thisx, PlayState* play);

void bb_move(EnBubble* this, PlayState* play);
void bb_explosion(EnBubble* this, PlayState* play);
void bb_restore(EnBubble* this, PlayState* play);

ActorProfile En_Bubble_Profile = {
    /**/ ACTOR_EN_BUBBLE,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BUBBLE,
    /**/ sizeof(EnBubble),
    /**/ En_Bubble_Actor_ct,
    /**/ En_Bubble_Actor_dt,
    /**/ En_Bubble_Actor_move,
    /**/ En_Bubble_Actor_draw,
};

static ColliderJntSphElementInit BbJntSphElemData[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x04 },
            { 0xFFCFD753, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 16 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00002824, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_NO_AT_INFO | ACELEM_NO_DAMAGE | ACELEM_NO_SWORD_SFX | ACELEM_NO_HITMARK,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 16 }, 100 },
    },
};

static ColliderJntSphInit BbAllJntSphData = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    2,
    BbJntSphElemData,
};

static CollisionCheckInfoInit2 BbStatusData = { 1, 2, 25, 25, MASS_IMMOVABLE };

static Vec3f sEffectAccel = { 0.0f, -0.5f, 0.0f };

static Color_RGBA8 prm = { 255, 255, 255, 255 };

static Color_RGBA8 env = { 150, 150, 150, 0 };

void bb_status_ct(EnBubble* this, f32 dim) {
    f32 a;
    f32 b;
    f32 c;
    f32 d;

    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_set_scale(&this->actor, 1.0f);
    this->actor.shape.yOffset = 16.0f;
    this->graphicRotSpeed = 16.0f;
    this->graphicEccentricity = 0.08f;
    this->expansionWidth = dim;
    this->expansionHeight = dim;
    a = fqrand();
    b = fqrand();
    c = fqrand();
    this->unk_218 = 1.0f;
    this->unk_21C = 1.0f;
    d = (a * a) + (b * b) + (c * c);
    this->unk_1FC.x = a / d;
    this->unk_1FC.y = b / d;
    this->unk_1FC.z = c / d;
}

u32 bb_explosion_ct(EnBubble* this) {
    ColliderElement* elem = &this->colliderSphere.elements[0].base;

    elem->atDmgInfo.dmgFlags = DMG_EXPLOSIVE;
    elem->atDmgInfo.effect = 0;
    elem->atDmgInfo.damage = 4;
    elem->atElemFlags = ATELEM_ON;
    this->actor.velocity.y = 0.0f;
    return 6;
}

// only called in an unused actionFunc
u32 bb_restore_ct(EnBubble* this) {
    bb_status_ct(this, -1.0f);
    return 12;
}

void bb_expansion_ct(EnBubble* this, PlayState* play) {
    s32 damage = -this->colliderSphere.elements[0].base.atDmgInfo.damage;

    play->damagePlayer(play, damage);
    Actor_player_power_damage_set2(play, &this->actor, 6.0f, this->actor.yawTowardsPlayer, 6.0f);
}

s32 bb_explosion_sub(EnBubble* this, PlayState* play) {
    u32 i;
    Vec3f effectAccel;
    Vec3f effectVel;
    Vec3f effectPos;

    effectAccel = sEffectAccel;
    add_calc(&this->expansionWidth, 4.0f, 0.1f, 1000.0f, 0.0f);
    add_calc(&this->expansionHeight, 4.0f, 0.1f, 1000.0f, 0.0f);
    add_calc(&this->graphicRotSpeed, 54.0f, 0.1f, 1000.0f, 0.0f);
    add_calc(&this->graphicEccentricity, 0.2f, 0.1f, 1000.0f, 0.0f);
    this->actor.shape.yOffset = ((this->expansionHeight + 1.0f) * 16.0f);

    if (DECR(this->explosionCountdown) != 0) {
        return -1;
    }
    effectPos.x = this->actor.world.pos.x;
    effectPos.y = this->actor.world.pos.y + this->actor.shape.yOffset;
    effectPos.z = this->actor.world.pos.z;
    for (i = 0; i < 20; i++) {
        effectVel.x = (fqrand() - 0.5f) * 7.0f;
        effectVel.y = fqrand() * 7.0f;
        effectVel.z = (fqrand() - 0.5f) * 7.0f;
        Effect_SS_Dt_Bubble_sc_cl_co_ct(play, &effectPos, &effectVel, &effectAccel, &prm,
                                          &env, get_random_timer(100, 50), 0x19, 0);
    }
    Item_Set_Std(play, NULL, &this->actor.world.pos, 0x50);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    return get_random_timer(90, 60);
}

// only called in an unused actionFunc
u32 bb_wait_sub(EnBubble* this) {
    if (DECR(this->explosionCountdown) != 0) {
        return -1;
    }
    return bb_restore_ct(this);
}

// only called in an unused actionFunc
s32 bb_restore_sub(EnBubble* this) {
    this->expansionWidth += 1.0f / 12.0f;
    this->expansionHeight += 1.0f / 12.0f;

    if (DECR(this->explosionCountdown) != 0) {
        return false;
    }
    return true;
}

void bb_refvec_calc(Vec3f* vec1, Vec3f* vec2, Vec3f* ret) {
    f32 norm;

    M3D_getRefVec(vec1, vec2, ret);
    norm = sqrtf((ret->x * ret->x) + (ret->y * ret->y) + (ret->z * ret->z));
    if (norm != 0.0f) {
        ret->x /= norm;
        ret->y /= norm;
        ret->z /= norm;
    } else {
        ret->x = ret->y = ret->z = 0.0f;
    }
}

void seikika(Vec3f* vec) {
    f32 norm = sqrt((vec->x * vec->x) + (vec->y * vec->y) + (vec->z * vec->z));

    if (norm != 0.0f) {
        vec->x /= norm;
        vec->y /= norm;
        vec->z /= norm;
    } else {
        vec->x = vec->y = vec->z = 0.0f;
    }
}

void bb_set_speed(EnBubble* this, PlayState* play) {
    CollisionPoly* poly;
    Actor* attackerActor;
    Vec3f sp84;
    Vec3f sp78;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;
    f32 bounceSpeed;
    s32 bgId;
    u8 bounceCount;

    if (this->colliderSphere.elements[1].base.acElemFlags & ACELEM_HIT) {
        attackerActor = this->colliderSphere.base.ac;
        this->normalizedAttackerVelocity = attackerActor->velocity;
        seikika(&this->normalizedAttackerVelocity);
        this->velocityFromAttack.x += (this->normalizedAttackerVelocity.x * 3.0f);
        this->velocityFromAttack.y += (this->normalizedAttackerVelocity.y * 3.0f);
        this->velocityFromAttack.z += (this->normalizedAttackerVelocity.z * 3.0f);
    }
    this->sinkSpeed -= 0.1f;
    if (this->sinkSpeed < this->actor.minVelocityY) {
        this->sinkSpeed = this->actor.minVelocityY;
    }
    sp54.x = this->velocityFromBounce.x + this->velocityFromAttack.x;
    sp54.y = this->velocityFromBounce.y + this->velocityFromAttack.y + this->sinkSpeed;
    sp54.z = this->velocityFromBounce.z + this->velocityFromAttack.z;
    seikika(&sp54);

    sp78.x = this->actor.world.pos.x;
    sp78.y = this->actor.world.pos.y + this->actor.shape.yOffset;
    sp78.z = this->actor.world.pos.z;
    sp6C = sp78;

    sp6C.x += (sp54.x * 24.0f);
    sp6C.y += (sp54.y * 24.0f);
    sp6C.z += (sp54.z * 24.0f);
    if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &sp78, &sp6C, &sp84, &poly, true, true, true, false, &bgId)) {
        sp60.x = COLPOLY_GET_NORMAL(poly->normal.x);
        sp60.y = COLPOLY_GET_NORMAL(poly->normal.y);
        sp60.z = COLPOLY_GET_NORMAL(poly->normal.z);
        bb_refvec_calc(&sp54, &sp60, &sp54);
        this->bounceDirection = sp54;
        bounceCount = this->bounceCount;
        this->bounceCount = ++bounceCount;
        if (bounceCount > (s16)(fqrand() * 10.0f)) {
            this->bounceCount = 0;
        }
        bounceSpeed = (this->bounceCount == 0) ? 3.6000001f : 3.0f;
        this->velocityFromAttack.x = this->velocityFromAttack.y = this->velocityFromAttack.z = 0.0f;
        this->velocityFromBounce.x = (this->bounceDirection.x * bounceSpeed);
        this->velocityFromBounce.y = (this->bounceDirection.y * bounceSpeed);
        this->velocityFromBounce.z = (this->bounceDirection.z * bounceSpeed);
        this->sinkSpeed = 0.0f;
        Actor_SE_set(&this->actor, NA_SE_EN_AWA_BOUND);
        this->graphicRotSpeed = 128.0f;
        this->graphicEccentricity = 0.48f;
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && sp54.y < 0.0f) {
        sp60.x = sp60.z = 0.0f;
        sp60.y = 1.0f;
        bb_refvec_calc(&sp54, &sp60, &sp54);
        this->bounceDirection = sp54;
        bounceCount = this->bounceCount;
        this->bounceCount = ++bounceCount;
        if (bounceCount > (s16)(fqrand() * 10.0f)) {
            this->bounceCount = 0;
        }
        bounceSpeed = (this->bounceCount == 0) ? 3.6000001f : 3.0f;
        this->velocityFromAttack.x = this->velocityFromAttack.y = this->velocityFromAttack.z = 0.0f;
        this->velocityFromBounce.x = (this->bounceDirection.x * bounceSpeed);
        this->velocityFromBounce.y = (this->bounceDirection.y * bounceSpeed);
        this->velocityFromBounce.z = (this->bounceDirection.z * bounceSpeed);
        this->sinkSpeed = 0.0f;
        Actor_SE_set(&this->actor, NA_SE_EN_AWA_BOUND);
        this->graphicRotSpeed = 128.0f;
        this->graphicEccentricity = 0.48f;
    }
    this->actor.velocity.x = this->velocityFromBounce.x + this->velocityFromAttack.x;
    this->actor.velocity.y = this->velocityFromBounce.y + this->velocityFromAttack.y + this->sinkSpeed;
    this->actor.velocity.z = this->velocityFromBounce.z + this->velocityFromAttack.z;
    add_calc2(&this->velocityFromAttack.x, 0.0f, 0.3f, 0.1f);
    add_calc2(&this->velocityFromAttack.y, 0.0f, 0.3f, 0.1f);
    add_calc2(&this->velocityFromAttack.z, 0.0f, 0.3f, 0.1f);
}

u32 bb_hit_to_weapon(EnBubble* this) {
    if (((this->colliderSphere.base.acFlags & AC_HIT) != 0) == false) {
        return false;
    }
    this->colliderSphere.base.acFlags &= ~AC_HIT;
    if (this->colliderSphere.elements[1].base.acElemFlags & ACELEM_HIT) {
        this->unk_1F0.x = this->colliderSphere.base.ac->velocity.x / 10.0f;
        this->unk_1F0.y = this->colliderSphere.base.ac->velocity.y / 10.0f;
        this->unk_1F0.z = this->colliderSphere.base.ac->velocity.z / 10.0f;
        this->graphicRotSpeed = 128.0f;
        this->graphicEccentricity = 0.48f;
        return false;
    }
    this->unk_208 = 8;
    return true;
}

static u32 hitcheck(EnBubble* this, PlayState* play) {
    if (DECR(this->unk_208) != 0 || this->actionFunc == bb_explosion) {
        return false;
    }
    if (this->colliderSphere.base.ocFlags2 & OC2_HIT_PLAYER) {
        this->colliderSphere.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        bb_expansion_ct(this, play);
        this->unk_208 = 8;
        return true;
    }
    return bb_hit_to_weapon(this);
}

void collision_calc(EnBubble* this) {
    ColliderJntSphElementDim* dim;
    Vec3f src;
    Vec3f dest;

    dim = &this->colliderSphere.elements[0].dim;
    src.x = dim->modelSphere.center.x;
    src.y = dim->modelSphere.center.y;
    src.z = dim->modelSphere.center.z;

    Matrix_Position(&src, &dest);
    dim->worldSphere.center.x = dest.x;
    dim->worldSphere.center.y = dest.y;
    dim->worldSphere.center.z = dest.z;
    dim->worldSphere.radius = dim->modelSphere.radius * (1.0f + this->expansionWidth);
    this->colliderSphere.elements[1].dim = *dim;
}

void En_Bubble_Actor_ct(Actor* thisx, PlayState* play) {
    EnBubble* this = (EnBubble*)thisx;
    u32 pad;

    Shape_Info_init(&this->actor.shape, 16.0f, Actor_shadow_circle, 0.2f);
    ClObjJntSph_ct(play, &this->colliderSphere);
    ClObjJntSph_set5_nzm(play, &this->colliderSphere, &this->actor, &BbAllJntSphData, this->colliderSphereItems);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(9), &BbStatusData);
    this->actor.naviEnemyId = NAVI_ENEMY_SHABOM;
    this->bounceDirection.x = fqrand();
    this->bounceDirection.y = fqrand();
    this->bounceDirection.z = fqrand();
    seikika(&this->bounceDirection);
    this->velocityFromBounce.x = this->bounceDirection.x * 3.0f;
    this->velocityFromBounce.y = this->bounceDirection.y * 3.0f;
    this->velocityFromBounce.z = this->bounceDirection.z * 3.0f;
    bb_status_ct(this, 0.0f);
    this->actionFunc = bb_move;
}

void En_Bubble_Actor_dt(Actor* thisx, PlayState* play) {
    EnBubble* this = (EnBubble*)thisx;

    ClObjJntSph_dt_nzf(play, &this->colliderSphere);
}

void bb_move(EnBubble* this, PlayState* play) {
    if (hitcheck(this, play)) {
        this->explosionCountdown = bb_explosion_ct(this);
        this->actionFunc = bb_explosion;
    } else {
        bb_set_speed(this, play);
        this->actor.shape.yOffset = ((this->expansionHeight + 1.0f) * 16.0f);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderSphere.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderSphere.base);
    }
}

void bb_explosion(EnBubble* this, PlayState* play) {
    if (bb_explosion_sub(this, play) >= 0) {
        Effect_SE_Info_new(play, &this->actor.world.pos, 60, NA_SE_EN_AWA_BREAK);
        Actor_delete(&this->actor);
    }
}

// unused
void bb_wait(EnBubble* this, PlayState* play) {
    s32 temp_v0;

    temp_v0 = bb_wait_sub(this);
    if (temp_v0 >= 0) {
        this->actor.shape.shadowDraw = Actor_shadow_circle;
        this->explosionCountdown = temp_v0;
        this->actionFunc = bb_restore;
    }
}

// unused
void bb_restore(EnBubble* this, PlayState* play) {
    if (bb_restore_sub(this)) {
        this->actionFunc = bb_move;
    }
    CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderSphere.base);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderSphere.base);
}

void En_Bubble_Actor_move(Actor* thisx, PlayState* play) {
    EnBubble* this = (EnBubble*)thisx;

    Actor_position_move(&this->actor);
    Actor_BGcheck2(play, &this->actor, 16.0f, 16.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    this->actionFunc(this, play);
    Actor_world_to_eye(&this->actor, this->actor.shape.yOffset);
}

void En_Bubble_Actor_draw(Actor* thisx, PlayState* play) {
    EnBubble* this = (EnBubble*)thisx;
    PlayState* play2 = (PlayState*)play;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_bubble.c", 1175);

    if (this->actionFunc != bb_wait) {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        add_calc(&this->graphicRotSpeed, 16.0f, 0.2f, 1000.0f, 0.0f);
        add_calc(&this->graphicEccentricity, 0.08f, 0.2f, 1000.0f, 0.0f);
        Matrix_rotate_scale_exchange(&play2->billboardMtxF);

        Matrix_scale(this->expansionWidth + 1.0f, this->expansionHeight + 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_rotateZ(DEG_TO_RAD((f32)play->state.frames) * this->graphicRotSpeed, MTXMODE_APPLY);
        Matrix_scale(this->graphicEccentricity + 1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_rotateZ(DEG_TO_RAD(-(f32)play->state.frames) * this->graphicRotSpeed, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_bubble.c", 1220);
        gSPDisplayList(POLY_XLU_DISP++, gBubbleDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_bubble.c", 1226);

    if (this->actionFunc != bb_wait) {
        this->actor.shape.shadowScale = (f32)((this->expansionWidth + 1.0f) * 0.2f);
        collision_calc(this);
    }
}
