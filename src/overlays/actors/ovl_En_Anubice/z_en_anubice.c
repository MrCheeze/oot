/*
 * File: z_en_anubice.c
 * Overlay: ovl_En_Anubice
 * Description: Anubis Body
 */

#include "z_en_anubice.h"
#include "assets/objects/object_anubice/object_anubice.h"
#include "overlays/actors/ovl_En_Anubice_Tag/z_en_anubice_tag.h"
#include "overlays/actors/ovl_Bg_Hidan_Curtain/z_bg_hidan_curtain.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Anubice_actor_ct(Actor* thisx, PlayState* play);
void En_Anubice_actor_dt(Actor* thisx, PlayState* play);
void En_Anubice_actor_move(Actor* thisx, PlayState* play);
void En_Anubice_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(EnAnubice* this, PlayState* play);
static void mode_stop_init(EnAnubice* this, PlayState* play);
static void mode_stop(EnAnubice* this, PlayState* play);
static void mode_return(EnAnubice* this, PlayState* play);
static void mode_fire_init(EnAnubice* this, PlayState* play);
static void mode_fire(EnAnubice* this, PlayState* play);
static void mode_dead(EnAnubice* this, PlayState* play);

ActorProfile En_Anubice_Profile = {
    /**/ ACTOR_EN_ANUBICE,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_ANUBICE,
    /**/ sizeof(EnAnubice),
    /**/ En_Anubice_actor_ct,
    /**/ En_Anubice_actor_dt,
    /**/ En_Anubice_actor_move,
    /**/ En_Anubice_actor_draw,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 29, 103, 0, { 0, 0, 0 } },
};

typedef enum AnubiceDamageEffect {
    /* 0x0 */ ANUBICE_DMGEFF_NONE,
    /* 0x2 */ ANUBICE_DMGEFF_FIRE = 2,
    /* 0xF */ ANUBICE_DMGEFF_0xF = 0xF // Treated the same as ANUBICE_DMGEFF_NONE in code
} AnubiceDamageEffect;

static DamageTable btl_data[] = {
    /* Deku nut      */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Deku stick    */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Slingshot     */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Explosive     */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Boomerang     */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Normal arrow  */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Hammer swing  */ DMG_ENTRY(1, ANUBICE_DMGEFF_0xF),
    /* Hookshot      */ DMG_ENTRY(2, ANUBICE_DMGEFF_0xF),
    /* Kokiri sword  */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Master sword  */ DMG_ENTRY(2, ANUBICE_DMGEFF_0xF),
    /* Giant's Knife */ DMG_ENTRY(6, ANUBICE_DMGEFF_0xF),
    /* Fire arrow    */ DMG_ENTRY(2, ANUBICE_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Light arrow   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Unk arrow 1   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Unk arrow 2   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Unk arrow 3   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Fire magic    */ DMG_ENTRY(3, ANUBICE_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Light magic   */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Giant spin    */ DMG_ENTRY(6, ANUBICE_DMGEFF_0xF),
    /* Master spin   */ DMG_ENTRY(2, ANUBICE_DMGEFF_0xF),
    /* Kokiri jump   */ DMG_ENTRY(0, ANUBICE_DMGEFF_0xF),
    /* Giant jump    */ DMG_ENTRY(12, ANUBICE_DMGEFF_0xF),
    /* Master jump   */ DMG_ENTRY(4, ANUBICE_DMGEFF_0xF),
    /* Unknown 1     */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, ANUBICE_DMGEFF_NONE),
};

void huwahuwa_set(EnAnubice* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->hoverVelocityTimer += 1500.0f;
    this->targetHeight = player->actor.world.pos.y + this->playerHeightOffset;
    add_calc2(&this->actor.world.pos.y, this->targetHeight, 0.1f, 10.0f);
    add_calc2(&this->playerHeightOffset, 10.0f, 0.1f, 0.5f);
    this->actor.velocity.y = sin_s(this->hoverVelocityTimer);
}

void player_head_angle_srch(EnAnubice* this, PlayState* play) {
    f32 xzDist;
    f32 x;
    f32 y;
    f32 z;
    Player* player = GET_PLAYER(play);

    x = player->actor.world.pos.x - this->headPos.x;
    y = player->actor.world.pos.y + 10.0f - this->headPos.y;
    z = player->actor.world.pos.z - this->headPos.z;
    xzDist = sqrtf(SQ(x) + SQ(z));

    this->fireballRot.x = -RAD_TO_BINANG(fatan2(y, xzDist));
    this->fireballRot.y = RAD_TO_BINANG(fatan2(x, z));
}

void En_Anubice_actor_ct(Actor* thisx, PlayState* play) {
    EnAnubice* this = (EnAnubice*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 20.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gAnubiceSkel, &gAnubiceIdleAnim, this->jointTable, this->morphTable,
                   ANUBICE_LIMB_MAX);

    PRINTF("\n\n");
    // "☆☆☆☆☆ Anubis occurence ☆☆☆☆☆"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ アヌビス発生 ☆☆☆☆☆ \n" VT_RST);

    this->actor.naviEnemyId = NAVI_ENEMY_ANUBIS;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);

    Actor_set_scale(&this->actor, 0.015f);

    this->actor.colChkInfo.damageTable = btl_data;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.shape.yOffset = -4230.0f;
    this->focusHeightOffset = 0.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->home = this->actor.world.pos;
    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    this->actionFunc = mode_wait;
}

void En_Anubice_actor_dt(Actor* thisx, PlayState* play) {
    EnAnubice* this = (EnAnubice*)thisx;
    EnAnubiceTag* tag;

    ClObjPipe_dt(play, &this->collider);

    if (this->actor.params != 0) {
        if (this->actor.parent) {}

        tag = (EnAnubiceTag*)this->actor.parent;
        if (tag != NULL && tag->actor.update != NULL) {
            tag->anubis = NULL;
        }
    }
}

static void mode_wait(EnAnubice* this, PlayState* play) {
    Actor* currentProp;
    s32 flameCirclesFound;

    if (this->isMirroringPlayer) {
        if (!this->hasSearchedForFlameCircles) {
            flameCirclesFound = 0;
            currentProp = play->actorCtx.actorLists[ACTORCAT_PROP].head;
            while (currentProp != NULL) {
                if (currentProp->id != ACTOR_BG_HIDAN_CURTAIN) {
                    currentProp = currentProp->next;
                } else {
                    this->flameCircles[flameCirclesFound] = (BgHidanCurtain*)currentProp;
                    // "☆☆☆☆☆ How many fires? ☆☆☆☆☆"
                    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 火は幾つ？ ☆☆☆☆☆ %d\n" VT_RST, flameCirclesFound);
                    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 火は幾つ？ ☆☆☆☆☆ %x\n" VT_RST,
                           this->flameCircles[flameCirclesFound]);
                    if (flameCirclesFound < ARRAY_COUNT(this->flameCircles) - 1) {
                        flameCirclesFound++;
                    }
                    currentProp = currentProp->next;
                }
            }
            this->hasSearchedForFlameCircles = true;
        }
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionFunc = mode_stop_init;
    }
}

static void mode_stop_init(EnAnubice* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(&gAnubiceIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gAnubiceIdleAnim, 1.0f, 0.0f, (s16)lastFrame, ANIMMODE_LOOP, -10.0f);

    this->actionFunc = mode_stop;
    this->actor.velocity.x = this->actor.velocity.z = this->actor.gravity = 0.0f;
}

static void mode_stop(EnAnubice* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc0(&this->actor.shape.yOffset, 0.5f, 300.0f);
    add_calc2(&this->focusHeightOffset, 70.0f, 0.5f, 5.0f);

    if (!this->isKnockedback) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 3000, 0);
    }

    if (this->actor.shape.yOffset > -2.0f) {
        this->actor.shape.yOffset = 0.0f;

        if (player->meleeWeaponState != 0) {
            this->actionFunc = mode_fire_init;
        } else if (this->isPlayerOutOfRange) {
            this->actor.velocity.y = 0.0f;
            this->actor.gravity = -1.0f;
            this->actionFunc = mode_return;
        }
    }
}

static void mode_return(EnAnubice* this, PlayState* play) {
    f32 xzDist;
    f32 normalizedX;
    f32 normalizedY;
    f32 x;
    f32 z;

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc2(&this->actor.shape.yOffset, -4230.0f, 0.5f, 300.0f);
    add_calc0(&this->focusHeightOffset, 0.5f, 5.0f);

    if (!this->isKnockedback) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 3000, 0);
    }

    if ((fabsf(this->home.x - this->actor.world.pos.x) > 3.0f) &&
        (fabsf(this->home.z - this->actor.world.pos.z) > 3.0f)) {
        x = this->home.x - this->actor.world.pos.x;
        z = this->home.z - this->actor.world.pos.z;
        xzDist = sqrtf(SQ(x) + SQ(z));
        normalizedX = x / xzDist;
        normalizedY = z / xzDist;
        this->actor.world.pos.x += normalizedX * 8;
        this->actor.world.pos.z += normalizedY * 8.0f;
    } else if (this->actor.shape.yOffset < -4220.0f) {
        this->actor.shape.yOffset = -4230.0f;
        this->isMirroringPlayer = this->isPlayerOutOfRange = false;
        this->actionFunc = mode_wait;
        this->actor.gravity = 0.0f;
    }
}

static void mode_fire_init(EnAnubice* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(&gAnubiceAttackingAnim);

    this->animLastFrame = lastFrame;
    Skeleton_Info2_init(&this->skelAnime, &gAnubiceAttackingAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = mode_fire;
    this->actor.velocity.x = this->actor.velocity.z = 0.0f;
}

static void mode_fire(EnAnubice* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (!this->isKnockedback) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 3000, 0);
    }

    player_head_angle_srch(this, play);

    if (curFrame == 12.0f) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ANUBICE_FIRE, this->headPos.x, this->headPos.y + 15.0f,
                    this->headPos.z, this->fireballRot.x, this->fireballRot.y, 0, 0);
    }

    if (this->animLastFrame <= curFrame) {
        this->actionFunc = mode_stop_init;
    }
}

static void mode_dead_init(EnAnubice* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(&gAnubiceFallDownAnim);

    this->animLastFrame = lastFrame;
    Skeleton_Info2_init(&this->skelAnime, &gAnubiceFallDownAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -20.0f);

    this->isNearWall = false;
    this->fallTargetYaw = 0;
    this->deathTimer = 20;
    this->actor.velocity.x = this->actor.velocity.z = 0.0f;
    this->actor.gravity = -1.0f;

    if (T_BGCheck_SimpleCheck(&play->colCtx, &this->headPos, 70.0f)) {
        this->isNearWall = true;
        this->fallTargetYaw = this->actor.shape.rot.x - 0x7F00;
    }

    this->actionFunc = mode_dead;
}

static void mode_dead(EnAnubice* this, PlayState* play) {
    f32 curFrame;
    f32 rotX;
    Vec3f baseFireEffectPos = { 0.0f, 0.0f, 0.0f };
    Vec3f rotatedFireEffectPos = { 0.0f, 0.0f, 0.0f };
    s32 pad;

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc0(&this->actor.shape.shadowScale, 0.4f, 0.25f);

    // If near a wall, turn away from it while dying to avoid going through it.
    // The implementation of this is bugged in the sense that the target angle is hardcoded in practice. If the poly
    // already has an angle of -0x7F00, the expected behavior won't occur.
    if (this->isNearWall) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->fallTargetYaw, 1, 10000, 0);
        if (fabsf(this->actor.shape.rot.y - this->fallTargetYaw) < 100.0f) {
            this->isNearWall = false;
        }
    }

    curFrame = this->skelAnime.curFrame;
    rotX = curFrame * -3000.0f;
    rotX = CLAMP_MIN(rotX, -11000.0f);

    Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y), MTXMODE_NEW);
    Matrix_rotateX(BINANG_TO_RAD_ALT(rotX), MTXMODE_APPLY);
    baseFireEffectPos.y = rnd_fx(10.0f) + 30.0f;
    Matrix_Position(&baseFireEffectPos, &rotatedFireEffectPos);
    rotatedFireEffectPos.x += this->actor.world.pos.x + rnd_fx(40.0f);
    rotatedFireEffectPos.y += this->actor.world.pos.y + rnd_fx(40.0f);
    rotatedFireEffectPos.z += this->actor.world.pos.z + rnd_fx(30.0f);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 128, COLORFILTER_BUFFLAG_OPA, 8);
    Effect_En_Fire_ct(play, &this->actor, &rotatedFireEffectPos, 100, 0, 0, -1);

    if ((this->animLastFrame <= curFrame) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        add_calc2(&this->actor.shape.yOffset, -4230.0f, 0.5f, 300.0f);
        if (this->actor.shape.yOffset < -2000.0f) {
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xC0);
            Actor_delete(&this->actor);
        }
    }
}

void En_Anubice_actor_move(Actor* thisx, PlayState* play) {
    f32 zero;
    BgHidanCurtain* flameCircle;
    s32 i;
    Vec3f baseKnockbackVelocity;
    Vec3f rotatedKnockbackVelocity;
    EnAnubice* this = (EnAnubice*)thisx;

    if ((this->actionFunc != mode_dead_init) && (this->actionFunc != mode_dead) &&
        (this->actor.shape.yOffset == 0.0f)) {
        huwahuwa_set(this, play);
        for (i = 0; i < ARRAY_COUNT(this->flameCircles); i++) {
            flameCircle = this->flameCircles[i];

            if ((flameCircle != NULL) && (fabsf(flameCircle->actor.world.pos.x - this->actor.world.pos.x) < 60.0f) &&
                (fabsf(this->flameCircles[i]->actor.world.pos.z - this->actor.world.pos.z) < 60.0f) &&
                (flameCircle->timer != 0)) {
                Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                Actor_info_finish(play, &this->actor);
                Actor_SE_set(&this->actor, NA_SE_EN_ANUBIS_DEAD);
                this->actionFunc = mode_dead_init;
                return;
            }
        }

        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            if (this->actor.colChkInfo.damageEffect == ANUBICE_DMGEFF_FIRE) {
                Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                Actor_info_finish(play, &this->actor);
                Actor_SE_set(&this->actor, NA_SE_EN_ANUBIS_DEAD);
                this->actionFunc = mode_dead_init;
                return;
            }

            if (!this->isKnockedback) {
                this->knockbackTimer = 10;
                this->isKnockedback = true;

                baseKnockbackVelocity.x = 0.0f;
                baseKnockbackVelocity.y = 0.0f;
                baseKnockbackVelocity.z = -10.0f;
                rotatedKnockbackVelocity.x = 0.0f;
                rotatedKnockbackVelocity.y = 0.0f;
                rotatedKnockbackVelocity.z = 0.0f;

                Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y), MTXMODE_NEW);
                Matrix_Position(&baseKnockbackVelocity, &rotatedKnockbackVelocity);

                this->actor.velocity.x = rotatedKnockbackVelocity.x;
                this->actor.velocity.z = rotatedKnockbackVelocity.z;
                this->knockbackRecoveryVelocity.x = -rotatedKnockbackVelocity.x;
                this->knockbackRecoveryVelocity.z = -rotatedKnockbackVelocity.z;

                Actor_SE_set(&this->actor, NA_SE_EN_NUTS_CUTBODY);
            }
        }

        if (this->isKnockedback) {
            this->actor.shape.rot.y += 6500;
            add_calc2(&this->actor.velocity.x, this->knockbackRecoveryVelocity.x, 0.3f, 1.0f);
            add_calc2(&this->actor.velocity.z, this->knockbackRecoveryVelocity.z, 0.3f, 1.0f);

            zero = 0.0f;
            if (zero) {}

            if (this->knockbackTimer == 0) {
                this->actor.velocity.x = this->actor.velocity.z = 0.0f;
                this->knockbackRecoveryVelocity.x = this->knockbackRecoveryVelocity.z = 0.0f;
                this->isKnockedback = false;
            }
        }
    }

    this->timeAlive++;

    if (this->knockbackTimer != 0) {
        this->knockbackTimer--;
    }

    if (this->deathTimer != 0) {
        this->deathTimer--;
    }

    this->actionFunc(this, play);

    this->actor.velocity.y += this->actor.gravity;
    Actor_position_move(&this->actor);

    if (!this->isPlayerOutOfRange) {
        Actor_BGcheck2(play, &this->actor, 5.0f, 5.0f, 10.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                    UPDBGCHECKINFO_FLAG_4);
    } else {
        Actor_BGcheck2(play, &this->actor, 5.0f, 5.0f, 10.0f,
                                UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    }

    if ((this->actionFunc != mode_dead_init) && (this->actionFunc != mode_dead)) {
        Actor_world_to_eye(&this->actor, this->focusHeightOffset);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

        if (!this->isKnockedback && (this->actor.shape.yOffset == 0.0f)) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

s32 En_Anubice_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnAnubice* this = (EnAnubice*)thisx;

    if (limbIndex == ANUBICE_LIMB_HEAD) {
        rot->z += this->headPitch;
    }

    return false;
}

void En_Anubice_draw_sub2(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnAnubice* this = (EnAnubice*)thisx;
    Vec3f pos = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == ANUBICE_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_anubice.c", 853);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_anubice.c", 856);
        gSPDisplayList(POLY_XLU_DISP++, gAnubiceEyesDL);
        Matrix_Position(&pos, &this->headPos);

        CLOSE_DISPS(play->state.gfxCtx, "../z_en_anubice.c", 868);
    }
}

void En_Anubice_actor_draw(Actor* thisx, PlayState* play) {
    EnAnubice* this = (EnAnubice*)thisx;

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, En_Anubice_draw_sub,
                      En_Anubice_draw_sub2, this);
}
