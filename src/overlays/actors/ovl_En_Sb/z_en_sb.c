/*
 * File: z_en_sb.c
 * Overlay: ovl_En_Sb
 * Description: Shellblade
 */

#include "z_en_sb.h"
#include "terminal.h"
#include "assets/objects/object_sb/object_sb.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void En_Sb_actor_ct(Actor* thisx, PlayState* play);
void En_Sb_actor_dt(Actor* thisx, PlayState* play);
void En_Sb_actor_move(Actor* thisx, PlayState* play);
void En_Sb_actor_draw(Actor* thisx, PlayState* play);

void CHG_Sb_Wait(EnSb* this);

void Sb_Wait(EnSb* this, PlayState* play);
void Sb_Open(EnSb* this, PlayState* play);
void Sb_AttackWait(EnSb* this, PlayState* play);
void Sb_Attack(EnSb* this, PlayState* play);
void Sb_Attack01(EnSb* this, PlayState* play);
void Sb_Attack02(EnSb* this, PlayState* play);
void Sb_Attack_End(EnSb* this, PlayState* play);

ActorProfile En_Sb_Profile = {
    /**/ ACTOR_EN_SB,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_SB,
    /**/ sizeof(EnSb),
    /**/ En_Sb_actor_ct,
    /**/ En_Sb_actor_dt,
    /**/ En_Sb_actor_move,
    /**/ En_Sb_actor_draw,
};

static ColliderCylinderInitType1 EnSbOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0xFFCFFFFF, 0x04, 0x08 }, { 0xFFCFFFFF, 0x00, 0x00 }, 0x01, 0x01, 0x01 },
    { 30, 40, 0, { 0, 0, 0 } },
};

static DamageTable btl_data[] = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(2, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xD),
    /* Master sword  */ DMG_ENTRY(2, 0xD),
    /* Giant's Knife */ DMG_ENTRY(4, 0xD),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0xE),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0xF),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xD),
    /* Giant spin    */ DMG_ENTRY(4, 0xD),
    /* Master spin   */ DMG_ENTRY(2, 0xD),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xD),
    /* Giant jump    */ DMG_ENTRY(8, 0xD),
    /* Master jump   */ DMG_ENTRY(4, 0xD),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_SHELL_BLADE, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

static Vec3f fire_pos[] = {
    { 5.0f, 0.0f, 0.0f },
    { -5.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 5.0f },
    { 0.0f, 0.0f, -5.0f },
};

typedef enum ShellbladeBehavior {
    /* 0x00 */ SHELLBLADE_OPEN,
    /* 0x01 */ SHELLBLADE_WAIT_CLOSED,
    /* 0x02 */ SHELLBLADE_WAIT_OPEN,
    /* 0x03 */ SHELLBLADE_LUNGE,
    /* 0x04 */ SHELLBLADE_BOUNCE
} ShellbladeBehavior;

void En_Sb_actor_ct(Actor* thisx, PlayState* play) {
    EnSb* this = (EnSb*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actor.colChkInfo.damageTable = btl_data;
    this->actor.colChkInfo.health = 2;
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_sb_Skel_002BF0, &object_sb_Anim_000194, NULL, NULL, 0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set3(play, &this->collider, &this->actor, &EnSbOcInfoData);
    this->isDead = false;
    this->actor.colChkInfo.mass = 0;
    Actor_set_scale(&this->actor, 0.006f);
    this->actor.shape.rot.y = 0;
    this->actor.speed = 0.0f;
    this->actor.gravity = -0.35f;
    this->fire = 0;
    this->hitByWindArrow = false;
    this->actor.velocity.y = -1.0f;
    CHG_Sb_Wait(this);
}

void En_Sb_actor_dt(Actor* thisx, PlayState* play) {
    EnSb* this = (EnSb*)thisx;
    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjPipe_dt(play, &this->collider);
}

void En_Sb_Bubble_Set(PlayState* play, EnSb* this) {
    s32 i;

    if (this->actor.depthInWater > 0) {
        for (i = 0; i < 10; i++) {
            Effect_SS_Bubble_ct(play, &this->actor.world.pos, 10.0f, 10.0f, 30.0f, 0.25f);
        }
    }
}

void CHG_Sb_Wait(EnSb* this) {
    Skeleton_Info2_init(&this->skelAnime, &object_sb_Anim_00004C, 1.0f, 0, Si2_anime_end_frame(&object_sb_Anim_00004C),
                     ANIMMODE_ONCE, 0.0f);
    this->behavior = SHELLBLADE_WAIT_CLOSED;
    this->actionFunc = Sb_Wait;
}

void CHG_Sb_Open(EnSb* this) {
    Skeleton_Info2_init(&this->skelAnime, &object_sb_Anim_000194, 1.0f, 0, Si2_anime_end_frame(&object_sb_Anim_000194),
                     ANIMMODE_ONCE, 0.0f);
    this->behavior = SHELLBLADE_OPEN;
    this->actionFunc = Sb_Open;
    Actor_SE_set(&this->actor, NA_SE_EN_SHELL_MOUTH);
}

void CHG_Sb_AttackWait(EnSb* this) {
    Skeleton_Info2_init(&this->skelAnime, &object_sb_Anim_002C8C, 1.0f, 0, Si2_anime_end_frame(&object_sb_Anim_002C8C),
                     ANIMMODE_LOOP, 0.0f);
    this->behavior = SHELLBLADE_WAIT_OPEN;
    this->actionFunc = Sb_AttackWait;
}

void CHG_Sb_Attack01(EnSb* this) {
    f32 frameCount = Si2_anime_end_frame(&object_sb_Anim_000124);
    f32 playbackSpeed = this->actor.depthInWater > 0.0f ? 1.0f : 0.0f;

    Skeleton_Info2_init(&this->skelAnime, &object_sb_Anim_000124, playbackSpeed, 0.0f, frameCount, ANIMMODE_ONCE, 0);
    this->behavior = SHELLBLADE_LUNGE;
    this->actionFunc = Sb_Attack01;
    Actor_SE_set(&this->actor, NA_SE_EN_SHELL_MOUTH);
}

void CHG_Sb_Attack02(EnSb* this) {
    Skeleton_Info2_init(&this->skelAnime, &object_sb_Anim_0000B4, 1.0f, 0, Si2_anime_end_frame(&object_sb_Anim_0000B4),
                     ANIMMODE_ONCE, 0.0f);
    this->behavior = SHELLBLADE_BOUNCE;
    this->actionFunc = Sb_Attack02;
}

void CHG_Sb_Attack_End(EnSb* this, s32 changeSpeed) {
    f32 frameCount = Si2_anime_end_frame(&object_sb_Anim_00004C);

    if (this->behavior != SHELLBLADE_WAIT_CLOSED) {
        Skeleton_Info2_init(&this->skelAnime, &object_sb_Anim_00004C, 1.0f, 0, frameCount, ANIMMODE_ONCE, 0.0f);
    }
    this->behavior = SHELLBLADE_WAIT_CLOSED;
    if (changeSpeed) {
        if (this->actor.depthInWater > 0.0f) {
            this->actor.speed = -5.0f;
            if (this->actor.velocity.y < 0.0f) {
                this->actor.velocity.y = 2.1f;
            }
        } else {
            this->actor.speed = -6.0f;
            if (this->actor.velocity.y < 0.0f) {
                this->actor.velocity.y = 1.4f;
            }
        }
    }
    this->timer = 60;
    this->actionFunc = Sb_Attack_End;
}

void Sb_Wait(EnSb* this, PlayState* play) {
    // always face toward link
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x7D0, 0x0);

    if ((this->actor.xzDistToPlayer <= 160.0f) && (this->actor.xzDistToPlayer > 40.0f)) {
        CHG_Sb_Open(this);
    }
}

void Sb_Open(EnSb* this, PlayState* play) {
    f32 currentFrame = this->skelAnime.curFrame;

    if (Si2_anime_end_frame(&object_sb_Anim_000194) <= currentFrame) {
        this->timer = 15;
        CHG_Sb_AttackWait(this);
    } else {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x7D0, 0x0);
        if ((this->actor.xzDistToPlayer > 160.0f) || (this->actor.xzDistToPlayer <= 40.0f)) {
            CHG_Sb_Wait(this);
        }
    }
}

void Sb_AttackWait(EnSb* this, PlayState* play) {
    s16 timer = this->timer;

    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x7D0, 0x0);

    if ((this->actor.xzDistToPlayer > 160.0f) || (this->actor.xzDistToPlayer <= 40.0f)) {
        CHG_Sb_Wait(this);
    }

    if (timer != 0) {
        this->timer = timer - 1;
    } else {
        this->timer = 0;
        this->attackYaw = this->actor.yawTowardsPlayer;
        this->actionFunc = Sb_Attack;
    }
}

void Sb_Attack(EnSb* this, PlayState* play) {
    s16 invertedYaw;

    invertedYaw = this->attackYaw + 0x8000;
    add_calc_short_angle2(&this->actor.shape.rot.y, invertedYaw, 0x1, 0x1F40, 0xA);

    if (this->actor.shape.rot.y == invertedYaw) {
        this->actor.world.rot.y = this->attackYaw;
        if (this->actor.depthInWater > 0.0f) {
            this->actor.velocity.y = 3.0f;
            this->actor.speed = 5.0f;
            this->actor.gravity = -0.35f;
        } else {
            this->actor.velocity.y = 2.0f;
            this->actor.speed = 6.0f;
            this->actor.gravity = -2.0f;
        }
        En_Sb_Bubble_Set(play, this);
        this->bouncesLeft = 3;
        CHG_Sb_Attack01(this);
        // "Attack!!"
        PRINTF("アタァ〜ック！！\n");
    }
}

void Sb_Attack01(EnSb* this, PlayState* play) {
    chase_f(&this->actor.speed, 0.0f, 0.2f);
    if ((this->actor.velocity.y <= -0.1f) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH)) {
        if (!(this->actor.depthInWater > 0.0f)) {
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        }
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND_TOUCH;
        CHG_Sb_Attack02(this);
    }
}

void Sb_Attack02(EnSb* this, PlayState* play) {
    s32 pad;
    f32 currentFrame;
    f32 frameCount;

    currentFrame = this->skelAnime.curFrame;
    frameCount = Si2_anime_end_frame(&object_sb_Anim_0000B4);
    chase_f(&this->actor.speed, 0.0f, 0.2f);

    if (currentFrame == frameCount) {
        if (this->bouncesLeft != 0) {
            this->bouncesLeft--;
            this->timer = 1;
            if (this->actor.depthInWater > 0.0f) {
                this->actor.velocity.y = 3.0f;
                this->actor.speed = 5.0f;
                this->actor.gravity = -0.35f;
            } else {
                this->actor.velocity.y = 2.0f;
                this->actor.speed = 6.0f;
                this->actor.gravity = -2.0f;
            }
            En_Sb_Bubble_Set(play, this);
            CHG_Sb_Attack01(this);
        } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND_TOUCH;
            this->actor.speed = 0.0f;
            this->timer = 1;
            CHG_Sb_Wait(this);
            PRINTF(VT_FGCOL(RED) "攻撃終了！！" VT_RST "\n"); // "Attack Complete!"
        }
    }
}

void Sb_Attack_End(EnSb* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
            this->actor.speed = 0.0f;
        }
    } else {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
            this->actionFunc = Sb_Wait;
            this->actor.speed = 0.0f;
        }
    }
}

s32 En_Sb_DamageCrossCheck(EnSb* this) {
    switch (this->behavior) {
        case SHELLBLADE_OPEN:
            if ((this->skelAnime.curFrame >= 2.0f) && (this->skelAnime.curFrame <= 5.0f)) {
                return true;
            }
            break;
        case SHELLBLADE_WAIT_CLOSED:
            if ((this->skelAnime.curFrame >= 0.0f) && (this->skelAnime.curFrame <= 1.0f)) {
                return true;
            }
            break;
        case SHELLBLADE_WAIT_OPEN:
            if ((this->skelAnime.curFrame >= 0.0f) && (this->skelAnime.curFrame <= 19.0f)) {
                return true;
            }
            break;
        case SHELLBLADE_LUNGE:
            if (this->skelAnime.curFrame == 0.0f) {
                return true;
            }
            break;
        case SHELLBLADE_BOUNCE:
            if ((this->skelAnime.curFrame >= 3.0f) && (this->skelAnime.curFrame <= 5.0f)) {
                return true;
            }
            break;
    }
    return false;
}

s32 En_Sb_Cross(EnSb* this, PlayState* play) {
    Vec3f hitPoint;
    f32 hitY;
    s16 yawDiff;
    s32 tookDamage;
    u8 hitByWindArrow;

    // hit box collided, switch to cool down
    if (this->collider.base.atFlags & AT_HIT) {
        CHG_Sb_Attack_End(this, 1);
        return 1;
    }

    // hurt box collided, take damage if appropriate
    if (this->collider.base.acFlags & AC_HIT) {
        hitByWindArrow = false;
        tookDamage = false;
        this->collider.base.acFlags &= ~AC_HIT;

        switch (this->actor.colChkInfo.damageEffect) {
            case 14: // wind arrow
                hitByWindArrow = true;
                FALLTHROUGH;
            case 15: // explosions, arrow, hammer, ice arrow, light arrow, spirit arrow, shadow arrow
                if (En_Sb_DamageCrossCheck(this)) {
                    hitY = this->collider.elem.acDmgInfo.hitPos.y - this->actor.world.pos.y;
                    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                    if ((hitY < 30.0f) && (hitY > 10.0f) && (yawDiff >= -0x1FFF) && (yawDiff < 0x2000)) {
                        hp_down(&this->actor);
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 80);
                        tookDamage = true;
                    }
                }
                break;
            case 2: // fire arrow, dins fire
                this->fire = 4;
                hp_down(&this->actor);
                Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 80);
                tookDamage = true;
                break;
            case 1:  // hookshot/longshot
            case 13: // all sword damage
                if (En_Sb_DamageCrossCheck(this)) {
                    hitY = this->collider.elem.acDmgInfo.hitPos.y - this->actor.world.pos.y;
                    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
                    if ((hitY < 30.0f) && (hitY > 10.0f) && (yawDiff >= -0x1FFF) && (yawDiff < 0x2000)) {
                        hp_down(&this->actor);
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 80);
                        tookDamage = true;
                        CHG_Sb_Attack_End(this, 0);
                    }
                }
                break;
            default:
                break;
        }
        if (this->actor.colChkInfo.health == 0) {
            this->hitByWindArrow = hitByWindArrow;
            Part_Break_init(&this->bodyBreak, 8, play);
            this->isDead = true;
            Actor_info_finish(play, &this->actor);
            Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EN_SHELL_DEAD);
            return 1;
        }

        // if player attack didn't do damage, play recoil sound effect and spawn sparks
        if (!tookDamage) {
            hitPoint.x = this->collider.elem.acDmgInfo.hitPos.x;
            hitPoint.y = this->collider.elem.acDmgInfo.hitPos.y;
            hitPoint.z = this->collider.elem.acDmgInfo.hitPos.z;
            CollisionCheckSetSparkFlash(play, &hitPoint);
        }
    }

    return 0;
}

void En_Sb_actor_move(Actor* thisx, PlayState* play) {
    EnSb* this = (EnSb*)thisx;
    s32 pad;

    if (this->isDead) {
        if (this->actor.depthInWater > 0.0f) {
            this->actor.params = 4;
        } else {
            this->actor.params = 1;
        }
        if (Part_break(&this->actor, &this->bodyBreak, play, this->actor.params)) {
            if (!this->hitByWindArrow) {
                Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x80);
            } else {
                Item_set0(play, &this->actor.world.pos, ITEM00_ARROWS_SMALL);
            }
            Actor_delete(&this->actor);
        }
    } else {
        Actor_world_to_eye(&this->actor, 20.0f);
        Actor_set_scale(&this->actor, 0.006f);
        Actor_position_moveF(&this->actor);
        this->actionFunc(this, play);
        Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 20.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        En_Sb_Cross(this, play);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        Skeleton_Info2_anime_play(&this->skelAnime);
    }
}

void En_Sb_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnSb* this = (EnSb*)thisx;

    Part_Break_Get(&this->bodyBreak, limbIndex, 0, 6, 8, dList, BODYBREAK_OBJECT_SLOT_DEFAULT);
}

void En_Sb_actor_draw(Actor* thisx, PlayState* play) {
    EnSb* this = (EnSb*)thisx;
    Vec3f flamePos;
    Vec3f* offset;
    s16 fireDecr;

    Actor_HiliteReflect_set_init(&this->actor, play, 1);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          En_Sb_display2, this);
    if (this->fire != 0) {
        this->actor.colorFilterTimer++;
        fireDecr = this->fire - 1;
        // this is intended to draw flames after being burned, but the condition is never met to run this code
        // fire gets set to 4 when burned, decrements to 3 and fails the "& 1" check and never stores the decrement
        if ((fireDecr & 1) == 0) {
            offset = &fire_pos[(fireDecr & 3)];
            flamePos.x = rnd_fx(5.0f) + (this->actor.world.pos.x + offset->x);
            flamePos.y = rnd_fx(5.0f) + (this->actor.world.pos.y + offset->y);
            flamePos.z = rnd_fx(5.0f) + (this->actor.world.pos.z + offset->z);
            Effect_En_Fire_ct(play, &this->actor, &flamePos, 100, 0, 0, -1);
        }
    }
}
