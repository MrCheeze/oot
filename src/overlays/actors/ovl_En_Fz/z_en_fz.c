#include "z_en_fz.h"
#include "assets/objects/object_fz/object_fz.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void En_Fz_actor_ct(Actor* thisx, PlayState* play);
void En_Fz_actor_dt(Actor* thisx, PlayState* play);
void En_Fz_actor_move(Actor* thisx, PlayState* play);
void En_Fz_actor_draw(Actor* thisx, PlayState* play);

void Fz_Beam_BG_Check(EnFz* this, PlayState* play);

// Stationary Freezard
void Change_Fz_search3(EnFz* this);
void Fz_search3(EnFz* this, PlayState* play);

// Moving Freezard that can vanish and reappear
void Fz_wait(EnFz* this, PlayState* play);
void Change_Fz_rise(EnFz* this);
void Fz_rise(EnFz* this, PlayState* play);
void Change_Fz_search(EnFz* this);
void Fz_search(EnFz* this, PlayState* play);
void Change_Fz_bodycrash(EnFz* this);
void Fz_bodycrash(EnFz* this, PlayState* play);
void Change_Fz_search2(EnFz* this);
void Fz_search2(EnFz* this, PlayState* play);
void Change_Fz_freeze(EnFz* this, PlayState* play);
void Fz_freeze(EnFz* this, PlayState* play);
void Change_Fz_dive(EnFz* this);
void Fz_dive(EnFz* this, PlayState* play);
void Change_Fz_wait(EnFz* this);

// Killed with fire source
void Change_Fz_melt(EnFz* this);
void Fz_melt(EnFz* this, PlayState* play);

// Death
void Change_Fz_death(EnFz* this, PlayState* play);
void Fz_death(EnFz* this, PlayState* play);

// Ice Smoke Effects
void fz_eff_smoke_ct(EnFz* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale);
void fz_eff_beam_ct(EnFz* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale, f32 xyScaleTarget,
                              s16 primAlpha, u8 isTimerMod8);
void Fz_Eff_move(EnFz* this, PlayState* play);
void Fz_Eff_disp(EnFz* this, PlayState* play);

ActorProfile En_Fz_Profile = {
    /**/ ACTOR_EN_FZ,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_FZ,
    /**/ sizeof(EnFz),
    /**/ En_Fz_actor_ct,
    /**/ En_Fz_actor_dt,
    /**/ En_Fz_actor_move,
    /**/ En_Fz_actor_draw,
};

static ColliderCylinderInitType1 EnFzOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCE0FDB, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 80, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 EnFzShieldOcInfoData = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0x0001F024, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 35, 80, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 EnFzBeamOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x02, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 20, 30, -15, { 0, 0, 0 } },
};

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0xF),
    /* Slingshot     */ DMG_ENTRY(0, 0xF),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0xF),
    /* Normal arrow  */ DMG_ENTRY(0, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(2, 0xF),
    /* Kokiri sword  */ DMG_ENTRY(0, 0xF),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(0, 0xF),
    /* Light arrow   */ DMG_ENTRY(0, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0xF),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0xF),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0xF),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0xF),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(0, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_FREEZARD, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

void En_Fz_actor_ct(Actor* thisx, PlayState* play) {
    EnFz* this = (EnFz*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actor.colChkInfo.damageTable = &btl_data;
    this->actor.colChkInfo.health = 6;

    ClObjPipe_ct(play, &this->collider1);
    ClObjPipe_set3(play, &this->collider1, &this->actor, &EnFzOcInfoData);

    ClObjPipe_ct(play, &this->collider2);
    ClObjPipe_set3(play, &this->collider2, &this->actor, &EnFzShieldOcInfoData);

    ClObjPipe_ct(play, &this->collider3);
    ClObjPipe_set3(play, &this->collider3, &this->actor, &EnFzBeamOcInfoData);

    Actor_set_scale(&this->actor, 0.008f);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unusedTimer1 = 0;
    this->unusedCounter = 0;
    this->updateBgInfo = true;
    this->isMoving = false;
    this->isFreezing = false;
    this->isActive = true;
    this->isDespawning = false;
    this->actor.speed = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->posOrigin.y = this->actor.world.pos.y;
    this->iceSmokeFreezingSpawnHeight = this->actor.world.pos.y;
    this->posOrigin.x = this->actor.world.pos.x;
    this->posOrigin.z = this->actor.world.pos.z;
    this->unusedFloat = 135.0f;

    if (this->actor.params < 0) {
        this->envAlpha = 0;
        this->actor.scale.y = 0.0f;
        Change_Fz_wait(this);
    } else {
        this->envAlpha = 255;
        Change_Fz_search3(this);
    }

    Fz_Beam_BG_Check(this, play);
}

void En_Fz_actor_dt(Actor* thisx, PlayState* play) {
    EnFz* this = (EnFz*)thisx;

    ClObjPipe_dt(play, &this->collider1);
    ClObjPipe_dt(play, &this->collider2);
    ClObjPipe_dt(play, &this->collider3);
}

void Fz_Beam_BG_Check(EnFz* this, PlayState* play) {
    Vec3f pos;
    Vec3f hitPos;
    Vec3f vec1;
    s32 bgId;
    CollisionPoly* hitPoly;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + 20.0f;
    pos.z = this->actor.world.pos.z;

    Matrix_translate(pos.x, pos.y, pos.z, MTXMODE_NEW);
    Matrix_rotateXYZ(this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z, MTXMODE_APPLY);
    vec1.x = vec1.y = 0.0f;
    vec1.z = 220.0f;
    Matrix_Position(&vec1, &this->wallHitPos);

    if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &pos, &this->wallHitPos, &hitPos, &hitPoly, true, false, false, true,
                                &bgId)) {
        xyz_t_move(&this->wallHitPos, &hitPos);
    }

    pos.x = this->actor.world.pos.x - this->wallHitPos.x;
    pos.z = this->actor.world.pos.z - this->wallHitPos.z;

    this->distToTargetSq = SQ(pos.x) + SQ(pos.z);
}

s32 Fz_Beam_BG_Check2(EnFz* this, Vec3f* vec) {
    if (this->distToTargetSq <= (SQ(this->actor.world.pos.x - vec->x) + SQ(this->actor.world.pos.z - vec->z))) {
        return true;
    } else {
        return false;
    }
}

void Fz_fragmentSet(EnFz* this, PlayState* play, Vec3f* vec, s32 numEffects, f32 unkFloat) {
    s32 i;
    Vec3f pos;
    Vec3f vel;
    Vec3f accel;
    Color_RGBA8 primColor;
    Color_RGBA8 envColor;
    f32 scale;
    s32 life;

    accel.x = accel.z = 0.0f;
    accel.y = -1.0f;
    primColor.r = 155;
    primColor.g = 255;
    primColor.b = 255;
    primColor.a = 255;
    envColor.r = 200;
    envColor.g = 200;
    envColor.b = 200;

    for (i = 0; i < numEffects; i++) {
        scale = rnd_fx(0.3f) + 0.6f;
        life = (s32)rnd_fx(5.0f) + 12;
        pos.x = rnd_fx(unkFloat) + vec->x;
        pos.y = rnd_f(unkFloat) + vec->y;
        pos.z = rnd_fx(unkFloat) + vec->z;
        vel.x = rnd_fx(10.0f);
        vel.y = rnd_f(10.0f) + 2.0f;
        vel.z = rnd_fx(10.0f);
        Effect_SS_Ice_Piece2_ct(play, &pos, scale, &vel, &accel, &primColor, &envColor, life);
    }

    CollisionCheckSetSparkFlashBlue_NoSE(play, vec);
}

void Fz_smokeSetnone(EnFz* this) {
}

// Fully grown
void Fz_smokeSetnormal(EnFz* this) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    if ((this->counter % 16) == 0) {
        pos.x = rnd_fx(40.0f) + this->actor.world.pos.x;
        pos.y = rnd_fx(40.0f) + this->actor.world.pos.y + 30.0f;
        pos.z = rnd_fx(40.0f) + this->actor.world.pos.z;
        accel.x = accel.z = 0.0f;
        accel.y = 0.1f;
        velocity.x = velocity.y = velocity.z = 0.0f;
        fz_eff_smoke_ct(this, &pos, &velocity, &accel, rnd_f(7.5f) + 15.0f);
    }
}

// (2) Growing or Shrinking to/from hiding or (3) melting from fire
void Fz_smokeSetdive(EnFz* this) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    if ((this->counter % 4) == 0) {
        pos.x = rnd_fx(40.0f) + this->actor.world.pos.x;
        pos.y = this->iceSmokeFreezingSpawnHeight;
        pos.z = rnd_fx(40.0f) + this->actor.world.pos.z;
        accel.x = accel.z = 0.0f;
        accel.y = 0.1f;
        velocity.x = velocity.y = velocity.z = 0.0f;
        fz_eff_smoke_ct(this, &pos, &velocity, &accel, rnd_f(7.5f) + 15.0f);
    }
}

void Fz_CrossCheck(EnFz* this, PlayState* play) {
    Vec3f vec;

    if (this->isMoving && ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
                           (BG_point_check(&this->actor, play, 60.0f, this->actor.world.rot.y) == 0))) {
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
        this->isMoving = false;
        this->speedXZ = 0.0f;
        this->actor.speed = 0.0f;
    }

    if (this->isFreezing) {
        if ((this->actor.params < 0) && (this->collider1.base.atFlags & AT_HIT)) {
            this->isMoving = false;
            this->collider1.base.acFlags &= ~AC_HIT;
            this->actor.speed = this->speedXZ = 0.0f;
            this->timer = 10;
            Change_Fz_dive(this);
        } else if (this->collider2.base.acFlags & AC_BOUNCED) {
            this->collider2.base.acFlags &= ~AC_BOUNCED;
            this->collider1.base.acFlags &= ~AC_HIT;
        } else if (this->collider1.base.acFlags & AC_HIT) {
            this->collider1.base.acFlags &= ~AC_HIT;
            switch (this->actor.colChkInfo.damageEffect) {
                case 0xF:
                    hp_down(&this->actor);
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 8);
                    if (this->actor.colChkInfo.health != 0) {
                        Actor_SE_set(&this->actor, NA_SE_EN_FREEZAD_DAMAGE);
                        vec.x = this->actor.world.pos.x;
                        vec.y = this->actor.world.pos.y;
                        vec.z = this->actor.world.pos.z;
                        Fz_fragmentSet(this, play, &vec, 10, 0.0f);
                        this->unusedCounter++;
                    } else {
                        Actor_SE_set(&this->actor, NA_SE_EN_FREEZAD_DEAD);
                        Actor_SE_set(&this->actor, NA_SE_EV_ICE_BROKEN);
                        vec.x = this->actor.world.pos.x;
                        vec.y = this->actor.world.pos.y;
                        vec.z = this->actor.world.pos.z;
                        Fz_fragmentSet(this, play, &vec, 30, 10.0f);
                        Change_Fz_death(this, play);
                    }
                    break;

                case 2:
                    hp_down(&this->actor);
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 8);
                    if (this->actor.colChkInfo.health == 0) {
                        Actor_SE_set(&this->actor, NA_SE_EN_FREEZAD_DEAD);
                        Change_Fz_melt(this);
                    } else {
                        Actor_SE_set(&this->actor, NA_SE_EN_FREEZAD_DAMAGE);
                    }
                    break;

                default:
                    break;
            }
        }
    }
}

void Fz_SearchPlayer(EnFz* this) {
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 10, 2000, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void Change_Fz_dive(EnFz* this) {
    this->state = 2;
    this->isFreezing = false;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = Fz_dive;
}

void Fz_dive(EnFz* this, PlayState* play) {
    this->envAlpha -= 16;

    if (this->envAlpha > 255) {
        this->envAlpha = 0;
    }

    if (add_calc(&this->actor.scale.y, 0.0f, 1.0f, 0.0005f, 0) == 0.0f) {
        Change_Fz_wait(this);
    }
}

void Change_Fz_wait(EnFz* this) {
    this->state = 0;
    this->unusedNum2 = 0;
    this->unusedNum1 = 0;
    this->timer = 100;
    this->actionFunc = Fz_wait;
    this->actor.world.pos.x = this->posOrigin.x;
    this->actor.world.pos.y = this->posOrigin.y;
    this->actor.world.pos.z = this->posOrigin.z;
}

void Fz_wait(EnFz* this, PlayState* play) {
    if ((this->timer == 0) && (this->actor.xzDistToPlayer < 400.0f)) {
        Change_Fz_rise(this);
    }
}

void Change_Fz_rise(EnFz* this) {
    this->state = 2;
    this->timer = 20;
    this->unusedNum2 = 4000;
    this->actionFunc = Fz_rise;
}

void Fz_rise(EnFz* this, PlayState* play) {
    if (this->timer == 0) {
        this->envAlpha += 8;
        if (this->envAlpha > 255) {
            this->envAlpha = 255;
        }

        if (add_calc(&this->actor.scale.y, 0.008f, 1.0f, 0.0005f, 0.0f) == 0.0f) {
            Change_Fz_search(this);
        }
    }
}

void Change_Fz_search(EnFz* this) {
    this->state = 1;
    this->timer = 40;
    this->updateBgInfo = true;
    this->isFreezing = true;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = Fz_search;
    this->actor.gravity = -1.0f;
}

void Fz_search(EnFz* this, PlayState* play) {
    Fz_SearchPlayer(this);

    if (this->timer == 0) {
        Change_Fz_bodycrash(this);
    }
}

void Change_Fz_bodycrash(EnFz* this) {
    this->state = 1;
    this->isMoving = true;
    this->timer = 100;
    this->actionFunc = Fz_bodycrash;
    this->speedXZ = 4.0f;
}

void Fz_bodycrash(EnFz* this, PlayState* play) {
    if ((this->timer == 0) || !this->isMoving) {
        Change_Fz_search2(this);
    }
}

void Change_Fz_search2(EnFz* this) {
    this->state = 1;
    this->speedXZ = 0.0f;
    this->actor.speed = 0.0f;
    this->timer = 40;
    this->actionFunc = Fz_search2;
}

void Fz_search2(EnFz* this, PlayState* play) {
    Fz_SearchPlayer(this);

    if (this->timer == 0) {
        Change_Fz_freeze(this, play);
    }
}

void Change_Fz_freeze(EnFz* this, PlayState* play) {
    this->state = 1;
    this->timer = 80;
    this->actionFunc = Fz_freeze;
    Fz_Beam_BG_Check(this, play);
}

void Fz_freeze(EnFz* this, PlayState* play) {
    Vec3f vec1;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    u8 isTimerMod8;
    s16 primAlpha;

    if (this->timer == 0) {
        Change_Fz_dive(this);
    } else if (this->timer >= 11) {
        isTimerMod8 = false;
        primAlpha = 150;
        Actor_level_SE_set(&this->actor, NA_SE_EN_FREEZAD_BREATH - SFX_FLAG);

        if ((this->timer - 10) < 16) { // t < 26
            primAlpha = (this->timer * 10) - 100;
        }

        accel.x = accel.z = 0.0f;
        accel.y = 0.6f;

        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 20.0f;
        pos.z = this->actor.world.pos.z;

        Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y), MTXMODE_NEW);

        vec1.x = 0.0f;
        vec1.y = -2.0f;
        vec1.z = 20.0f; // xz velocity

        Matrix_Position(&vec1, &velocity);

        if ((this->timer % 8) == 0) {
            isTimerMod8 = true;
        }

        fz_eff_beam_ct(this, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, isTimerMod8);

        pos.x += (velocity.x * 0.5f);
        pos.y += (velocity.y * 0.5f);
        pos.z += (velocity.z * 0.5f);

        fz_eff_beam_ct(this, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, false);
    }
}

void Change_Fz_death(EnFz* this, PlayState* play) {
    this->state = 0;
    this->speedXZ = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 0.0f;
    this->updateBgInfo = true;
    this->isFreezing = false;
    this->isDespawning = true;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->isActive = false;
    this->timer = 60;
    Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x60);
    this->actionFunc = Fz_death;
}

void Fz_death(EnFz* this, PlayState* play) {
    if (this->timer == 0) {
        Actor_delete(&this->actor);
    }
}

void Change_Fz_melt(EnFz* this) {
    this->state = 3;
    this->isFreezing = false;
    this->isDespawning = true;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    this->speedXZ = 0.0f;
    this->actionFunc = Fz_melt;
}

void Fz_melt(EnFz* this, PlayState* play) {
    chase_f(&this->actor.scale.y, 0.0006f, 0.0002f);

    if (this->actor.scale.y < 0.006f) {
        this->actor.scale.x += 0.0004f;
        this->actor.scale.z += 0.0004f;
    }

    if (this->actor.scale.y < 0.004f) {
        this->envAlpha -= 8;
        if (this->envAlpha > 255) {
            this->envAlpha = 0;
        }
    }

    if (this->envAlpha == 0) {
        Change_Fz_death(this, play);
    }
}

void Change_Fz_search3(EnFz* this) {
    this->state = 1;
    this->timer = 40;
    this->updateBgInfo = true;
    this->isFreezing = true;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = Fz_search3;
    this->actor.gravity = -1.0f;
}

void Fz_search3(EnFz* this, PlayState* play) {
    Vec3f vec1;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    u8 isTimerMod8;
    s16 primAlpha;

    if (this->counter & 0xC0) {
        Fz_SearchPlayer(this);
        Fz_Beam_BG_Check(this, play);
    } else {
        isTimerMod8 = false;
        primAlpha = 150;
        Actor_level_SE_set(&this->actor, NA_SE_EN_FREEZAD_BREATH - SFX_FLAG);

        if ((this->counter & 0x3F) >= 48) {
            primAlpha = 630 - ((this->counter & 0x3F) * 10);
        }

        accel.x = accel.z = 0.0f;
        accel.y = 0.6f;

        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 20.0f;
        pos.z = this->actor.world.pos.z;

        Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y), MTXMODE_NEW);

        vec1.x = 0.0f;
        vec1.y = -2.0f;
        vec1.z = 20.0f;

        Matrix_Position(&vec1, &velocity);

        if ((this->counter % 8) == 0) {
            isTimerMod8 = true;
        }

        fz_eff_beam_ct(this, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, isTimerMod8);

        pos.x += (velocity.x * 0.5f);
        pos.y += (velocity.y * 0.5f);
        pos.z += (velocity.z * 0.5f);

        fz_eff_beam_ct(this, &pos, &velocity, &accel, 2.0f, 25.0f, primAlpha, false);
    }
}

static EnFzSpawnIceSmokeFunc smokeProc[] = {
    Fz_smokeSetnone,
    Fz_smokeSetnormal,
    Fz_smokeSetdive,
    Fz_smokeSetdive,
};

void En_Fz_actor_move(Actor* thisx, PlayState* play) {
    EnFz* this = (EnFz*)thisx;
    s32 pad;

    this->counter++;

    if (this->unusedTimer1 != 0) {
        this->unusedTimer1--;
    }

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->unusedTimer2 != 0) {
        this->unusedTimer2--;
    }

    Actor_world_to_eye(&this->actor, 50.0f);
    Fz_CrossCheck(this, play);
    this->actionFunc(this, play);
    if (!this->isDespawning) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider1);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider2);
        if (this->isFreezing) {
            if (this->actor.colorFilterTimer == 0) {
                CollisionCheck_setAC(play, &play->colChkCtx, &this->collider1.base);
                CollisionCheck_setAC(play, &play->colChkCtx, &this->collider2.base);
            }
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider1.base);
        }
    }

    chase_f(&this->actor.speed, this->speedXZ, 0.2f);
    Actor_position_moveF(&this->actor);

    if (this->updateBgInfo) {
        Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 20.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    }

    smokeProc[this->state](this);
    Fz_Eff_move(this, play);
}

void En_Fz_actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* fz_model[] = {
        gFreezardIntactDL,              // Body fully intact           (5 or 6 health)
        gFreezardTopRightHornChippedDL, // Top right horn chipped off  (from Freezards perspective)   (3 or 4 health)
        gFreezardHeadChippedDL,         // Entire head chipped off     (1 or 2 health)
    };
    EnFz* this = (EnFz*)thisx;
    s32 pad;
    s32 index;

    index = (6 - this->actor.colChkInfo.health) >> 1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_fz.c", 1167);

    if (this->actor.colChkInfo.health == 0) {
        index = 2;
    }

    if (this->isActive) {
        Actor_HiliteReflect_xlu_set_init(&this->actor, play, 0);
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, play->state.frames & 0x7F, 32, 32, 1, 0,
                                    (2 * play->state.frames) & 0x7F, 32, 32));
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_fz.c", 1183);
        gDPSetCombineLERP(POLY_XLU_DISP++, TEXEL1, PRIMITIVE, PRIM_LOD_FRAC, TEXEL0, TEXEL1, TEXEL0, PRIMITIVE, TEXEL0,
                          PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, COMBINED, 0, ENVIRONMENT, 0);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 128, 155, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 200, this->envAlpha);
        gSPDisplayList(POLY_XLU_DISP++, fz_model[index]);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_fz.c", 1200);
    Fz_Eff_disp(this, play);
}

void fz_eff_smoke_ct(EnFz* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale) {
    EnFzEffect* effect = this->effects;
    s16 i;

    for (i = 0; i < EN_FZ_EFFECT_COUNT; i++) {
        if (effect->type == 0) {
            effect->type = 1;
            effect->pos = *pos;
            effect->velocity = *velocity;
            effect->accel = *accel;
            effect->primAlphaState = 0;
            effect->xyScale = xyScale / 1000.0f;
            effect->primAlpha = 0;
            effect->timer = 0;
            break;
        }

        effect++;
    }
}

void fz_eff_beam_ct(EnFz* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 xyScale, f32 xyScaleTarget,
                              s16 primAlpha, u8 isTimerMod8) {
    EnFzEffect* effect = this->effects;
    s16 i;

    for (i = 0; i < EN_FZ_EFFECT_COUNT; i++) {
        if (effect->type == 0) {
            effect->type = 2;
            effect->pos = *pos;
            effect->velocity = *velocity;
            effect->accel = *accel;
            effect->primAlphaState = 0;
            effect->xyScale = xyScale / 1000.0f;
            effect->xyScaleTarget = xyScaleTarget / 1000.0f;
            effect->primAlpha = primAlpha;
            effect->timer = 0;
            effect->isTimerMod8 = isTimerMod8;
            break;
        }

        effect++;
    }
}

void Fz_Eff_move(EnFz* this, PlayState* play) {
    EnFzEffect* effect = this->effects;
    s16 i;
    Vec3f pos;

    for (i = 0; i < EN_FZ_EFFECT_COUNT; i++) {
        if (effect->type) {
            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;
            effect->timer++;
            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;
            if (effect->type == 1) {
                if (effect->primAlphaState == 0) { // Becoming more opaque
                    effect->primAlpha += 10;
                    if (effect->primAlpha >= 100) {
                        effect->primAlphaState++;
                    }
                } else { // Becoming more transparent
                    effect->primAlpha -= 3;
                    if (effect->primAlpha <= 0) {
                        effect->primAlpha = 0;
                        effect->type = 0;
                    }
                }
            } else if (effect->type == 2) { // Freezing
                add_calc2(&effect->xyScale, effect->xyScaleTarget, 0.1f, effect->xyScaleTarget / 10.0f);
                if (effect->primAlphaState == 0) { // Becoming more opaque
                    if (effect->timer >= 7) {
                        effect->primAlphaState++;
                    }
                } else { // Becoming more transparent, slows down
                    effect->accel.y = 2.0f;
                    effect->primAlpha -= 17;
                    effect->velocity.x *= 0.75f;
                    effect->velocity.z *= 0.75f;
                    if (effect->primAlpha <= 0) {
                        effect->primAlpha = 0;
                        effect->type = 0;
                    }
                }

                if ((this->unusedTimer2 == 0) && (effect->primAlpha >= 101) && effect->isTimerMod8) {
                    this->collider3.dim.pos.x = (s16)effect->pos.x;
                    this->collider3.dim.pos.y = (s16)effect->pos.y;
                    this->collider3.dim.pos.z = (s16)effect->pos.z;
                    CollisionCheck_setAT(play, &play->colChkCtx, &this->collider3.base);
                }

                pos.x = effect->pos.x;
                pos.y = effect->pos.y + 10.0f;
                pos.z = effect->pos.z;

                if ((effect->primAlphaState != 2) && Fz_Beam_BG_Check2(this, &pos)) {
                    effect->primAlphaState = 2;
                    effect->velocity.x = 0.0f;
                    effect->velocity.z = 0.0f;
                }
            }
        }
        effect++;
    }
}

void Fz_Eff_disp(EnFz* this, PlayState* play) {
    s16 i;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    u8 materialFlag = 0;
    EnFzEffect* effect = this->effects;

    OPEN_DISPS(gfxCtx, "../z_en_fz.c", 1384);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < EN_FZ_EFFECT_COUNT; i++) {
        if (effect->type > 0) {
            gDPPipeSync(POLY_XLU_DISP++);

            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gFreezardSteamStartDL));
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, effect->primAlpha);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 3 * (effect->timer + (3 * i)),
                                        15 * (effect->timer + (3 * i)), 32, 64, 1, 0, 0, 32, 32));
            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(effect->xyScale, effect->xyScale, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_en_fz.c", 1424);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gFreezardSteamDL));
        }

        effect++;
    }

    CLOSE_DISPS(gfxCtx, "../z_en_fz.c", 1430);
}
