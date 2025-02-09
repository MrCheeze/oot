/*
 * File: z_en_skb.c
 * Overlay: ovl_En_Skb
 * Description: Stalchild
 */

#include "z_en_skb.h"
#include "overlays/actors/ovl_En_Encount1/z_en_encount1.h"
#include "assets/objects/object_skb/object_skb.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

typedef enum StalchildBehavior {
    SKB_BEHAVIOR_BURIED,
    SKB_BEHAVIOR_DYING,
    SKB_BEHAVIOR_DAMAGED,
    SKB_BEHAVIOR_ATTACKING,
    SKB_BEHAVIOR_WALKING,
    SKB_BEHAVIOR_RECOILING,
    SKB_BEHAVIOR_STUNNED
} StalchildBehavior;

void En_Skb_actor_ct(Actor* thisx, PlayState* play);
void En_Skb_actor_dt(Actor* thisx, PlayState* play);
void En_Skb_actor_move(Actor* thisx, PlayState* play);
void En_Skb_actor_draw(Actor* thisx, PlayState* play);

void mode_before_move_init(EnSkb* this);
void mode_before_move(EnSkb* this, PlayState* play);
void mode_ret_init(EnSkb* this);
void mode_ret(EnSkb* this, PlayState* play);
static void mode_forward_init(EnSkb* this);
static void mode_forward(EnSkb* this, PlayState* play);
static void mode_attack_init(EnSkb* this);
static void mode_attack(EnSkb* this, PlayState* play);
static void mode_hirumu_init(EnSkb* this);
static void mode_hirumu(EnSkb* this, PlayState* play);
static void mode_paralyze(EnSkb* this, PlayState* play);
static void mode_damage(EnSkb* this, PlayState* play);
static void mode_down_init(EnSkb* this, PlayState* play);
static void mode_down(EnSkb* this, PlayState* play);

static ColliderJntSphElementInit JntSphElemData[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 15, { { 0, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit JntSphData = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    2,
    JntSphElemData,
};

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0xF),
    /* Slingshot     */ DMG_ENTRY(1, 0xF),
    /* Explosive     */ DMG_ENTRY(2, 0xF),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0xF),
    /* Hammer swing  */ DMG_ENTRY(2, 0xF),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xE),
    /* Master sword  */ DMG_ENTRY(2, 0xF),
    /* Giant's Knife */ DMG_ENTRY(4, 0xF),
    /* Fire arrow    */ DMG_ENTRY(4, 0x7),
    /* Ice arrow     */ DMG_ENTRY(2, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0xF),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x7),
    /* Ice magic     */ DMG_ENTRY(0, 0x6),
    /* Light magic   */ DMG_ENTRY(3, 0xD),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xD),
    /* Giant spin    */ DMG_ENTRY(4, 0xF),
    /* Master spin   */ DMG_ENTRY(2, 0xF),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xF),
    /* Giant jump    */ DMG_ENTRY(8, 0xF),
    /* Master jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0xF),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

ActorProfile En_Skb_Profile = {
    /**/ ACTOR_EN_SKB,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_SKB,
    /**/ sizeof(EnSkb),
    /**/ En_Skb_actor_ct,
    /**/ En_Skb_actor_dt,
    /**/ En_Skb_actor_move,
    /**/ En_Skb_actor_draw,
};

void En_Skb_actor_set_process(EnSkb* this, EnSkbActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static void dust_ground_setx(PlayState* play, EnSkb* this, Vec3f* spawnPos) {
    Vec3f pos;
    Vec3f vel = { 0.0f, 8.0f, 0.0f };
    Vec3f accel = { 0.0f, -1.5f, 0.0f };
    f32 spreadAngle;
    f32 scale;

    spreadAngle = (fqrand() - 0.5f) * 6.28f;
    pos.y = this->actor.floorHeight;
    pos.x = (sinf_table(spreadAngle) * 15.0f) + spawnPos->x;
    pos.z = (cosf_table(spreadAngle) * 15.0f) + spawnPos->z;
    accel.x = rnd_fx(1.0f);
    accel.z = rnd_fx(1.0f);
    vel.y += (fqrand() - 0.5f) * 4.0f;
    scale = (fqrand() * 5.0f) + 12.0f;
    Effect_Hahen_ct3(play, &pos, &vel, &accel, 2, scale * 0.8f, -1, 10, NULL);
    dust_fly_set2(play, &pos, 10.0f, 1, 150, 0, 1);
}

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
};

void En_Skb_actor_ct(Actor* thisx, PlayState* play) {
    EnSkb* this = (EnSkb*)thisx;

    ValueSet_process(&this->actor, value_init);
    this->actor.colChkInfo.damageTable = &btl_data;
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 0.0f);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = 2;
    this->actor.shape.yOffset = -8000.0f;
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gStalchildSkel, &gStalchildUncurlingAnim, this->jointTable,
                   this->morphTable, 20);
    this->actor.naviEnemyId = NAVI_ENEMY_STALCHILD;

    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &JntSphData, this->colliderItem);
    Actor_set_scale(&this->actor, ((this->actor.params * 0.1f) + 1.0f) * 0.01f);

    this->collider.elements[0].dim.modelSphere.radius = this->collider.elements[0].dim.worldSphere.radius =
        10 + this->actor.params;
    this->collider.elements[1].dim.modelSphere.radius = this->collider.elements[1].dim.worldSphere.radius =
        20 + (this->actor.params * 2);

    this->actor.home.pos = this->actor.world.pos;
    this->actor.floorHeight = this->actor.world.pos.y;
    mode_before_move_init(this);
}

void En_Skb_actor_dt(Actor* thisx, PlayState* play) {
    EnSkb* this = (EnSkb*)thisx;

    if (this->actor.parent != NULL) {
        EnEncount1* spawner = (EnEncount1*)this->actor.parent;

        if (spawner->actor.update != NULL) {
            if (spawner->curNumSpawn > 0) {
                spawner->curNumSpawn--;
            }
        }
    }
    ClObjJntSph_dt_nzf(play, &this->collider);
}

void attack_or_forward(EnSkb* this) {
    if (IS_DAY) {
        mode_ret_init(this);
    } else if (Actor_player_direction_check(&this->actor, 0x11C7) &&
               (this->actor.xzDistToPlayer < (60.0f + (this->actor.params * 6.0f)))) {
        mode_attack_init(this);
    } else {
        mode_forward_init(this);
    }
}

void mode_before_move_init(EnSkb* this) {
    Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gStalchildUncurlingAnim, 1.0f);
    this->actionState = SKB_BEHAVIOR_BURIED;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_SE_set(&this->actor, NA_SE_EN_RIVA_APPEAR);
    En_Skb_actor_set_process(this, mode_before_move);
}

void mode_before_move(EnSkb* this, PlayState* play) {
    if (this->skelAnime.curFrame < 4.0f) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
    add_calc(&this->actor.shape.yOffset, 0.0f, 1.0f, 800.0f, 0.0f);
    add_calc(&this->actor.shape.shadowScale, 25.0f, 1.0f, 2.5f, 0.0f);
    if ((play->gameplayFrames & 1) != 0) {
        dust_ground_setx(play, this, &this->actor.world.pos);
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime) && (0.0f == this->actor.shape.yOffset)) {
        attack_or_forward(this);
    }
}

void mode_ret_init(EnSkb* this) {
    Skeleton_Info2_init(&this->skelAnime, &gStalchildUncurlingAnim, -1.0f,
                     Si2_anime_end_frame(&gStalchildUncurlingAnim), 0.0f, ANIMMODE_ONCE, -4.0f);
    this->actionState = SKB_BEHAVIOR_BURIED;
    this->setColliderAT = false;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
    En_Skb_actor_set_process(this, mode_ret);
}

void mode_ret(EnSkb* this, PlayState* play) {
    if ((add_calc(&this->actor.shape.yOffset, -8000.0f, 1.0f, 500.0f, 0.0f) != 0.0f) &&
        (play->gameplayFrames & 1)) {
        dust_ground_setx(play, this, &this->actor.world.pos);
    }
    add_calc(&this->actor.shape.shadowScale, 0.0f, 1.0f, 2.5f, 0.0f);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        Actor_delete(&this->actor);
    }
}

static void mode_forward_init(EnSkb* this) {
    Skeleton_Info2_init(&this->skelAnime, &gStalchildWalkingAnim, 0.96000004f, 0.0f,
                     Si2_anime_end_frame(&gStalchildWalkingAnim), ANIMMODE_LOOP, -4.0f);
    this->actionState = SKB_BEHAVIOR_WALKING;
    this->headlessYawOffset = 0;
    this->actor.speed = this->actor.scale.y * 160.0f;
    En_Skb_actor_set_process(this, mode_forward);
}

static void mode_forward(EnSkb* this, PlayState* play) {
    s32 thisKeyFrame;
    s32 prevKeyFrame;
    f32 playSpeed;
    Player* player = GET_PLAYER(play);

    if ((this->breakFlags != 0) && ((play->gameplayFrames & 0xF) == 0)) {
        this->headlessYawOffset = rnd_fx(50000.0f);
    }
    add_calc_short_angle2(&this->actor.shape.rot.y, (this->actor.yawTowardsPlayer + this->headlessYawOffset), 1, 0x2EE, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    thisKeyFrame = this->skelAnime.curFrame;
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->skelAnime.playSpeed >= 0.0f) {
        playSpeed = this->skelAnime.playSpeed;
    } else {
        playSpeed = -this->skelAnime.playSpeed;
    }
    prevKeyFrame = (this->skelAnime.curFrame - playSpeed);
    if (this->skelAnime.playSpeed >= 0.0f) {
        playSpeed = this->skelAnime.playSpeed;
    } else {
        playSpeed = -this->skelAnime.playSpeed;
    }
    if (thisKeyFrame != (s32)this->skelAnime.curFrame) {
        if (((prevKeyFrame < 9) && (((s32)playSpeed + thisKeyFrame) >= 8)) ||
            !((prevKeyFrame >= 16) || (((s32)playSpeed + thisKeyFrame) < 15))) {

            Actor_SE_set(&this->actor, NA_SE_EN_STALKID_WALK);
        }
    }
    if (search_position_distanceXZ(&this->actor.home.pos, &player->actor.world.pos) > 800.0f || IS_DAY) {
        mode_ret_init(this);
    } else if (Actor_player_direction_check(&this->actor, 0x11C7) &&
               (this->actor.xzDistToPlayer < (60.0f + (this->actor.params * 6.0f)))) {
        mode_attack_init(this);
    }
}

static void mode_attack_init(EnSkb* this) {
    Skeleton_Info2_init(&this->skelAnime, &gStalchildAttackingAnim, 0.6f, 0.0f,
                     Si2_anime_end_frame(&gStalchildAttackingAnim), ANIMMODE_ONCE_INTERP, 4.0f);
    this->collider.base.atFlags &= ~AT_BOUNCED;
    this->actionState = SKB_BEHAVIOR_ATTACKING;
    this->actor.speed = 0.0f;
    En_Skb_actor_set_process(this, mode_attack);
}

static void mode_attack(EnSkb* this, PlayState* play) {
    s32 frameData;

    frameData = this->skelAnime.curFrame;
    if (frameData == 3) {
        Actor_SE_set(&this->actor, NA_SE_EN_STALKID_ATTACK);
        this->setColliderAT = true;
    } else if (frameData == 6) {
        this->setColliderAT = false;
    }
    if (this->collider.base.atFlags & AT_BOUNCED) {
        this->collider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        mode_hirumu_init(this);
    } else if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        attack_or_forward(this);
    }
}

static void mode_hirumu_init(EnSkb* this) {
    Skeleton_Info2_init(&this->skelAnime, &gStalchildAttackingAnim, -0.4f, this->skelAnime.curFrame - 1.0f, 0.0f,
                     ANIMMODE_ONCE_INTERP, 0.0f);
    this->collider.base.atFlags &= ~AT_BOUNCED;
    this->actionState = SKB_BEHAVIOR_RECOILING;
    this->setColliderAT = false;
    En_Skb_actor_set_process(this, mode_hirumu);
}

static void mode_hirumu(EnSkb* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        attack_or_forward(this);
    }
}

static void mode_paralyze_init(EnSkb* this) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = 0.0f;
    }
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->setColliderAT = false;
    this->actionState = SKB_BEHAVIOR_STUNNED;
    En_Skb_actor_set_process(this, mode_paralyze);
}

static void mode_paralyze(EnSkb* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.speed < 0.0f) {
            this->actor.speed += 0.05f;
        }
    }
    if ((this->actor.colorFilterTimer == 0) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (this->actor.colChkInfo.health == 0) {
            mode_down_init(this, play);
        } else {
            attack_or_forward(this);
        }
    }
}

static void mode_damage_init(EnSkb* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gStalchildDamagedAnim, -4.0f);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = -4.0f;
    }
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Actor_SE_set(&this->actor, NA_SE_EN_STALKID_DAMAGE);
    this->actionState = SKB_BEHAVIOR_DAMAGED;
    En_Skb_actor_set_process(this, mode_damage);
}

static void mode_damage(EnSkb* this, PlayState* play) {
    // this cast is likely not real, but allows for a match
    u8* new_var;

    new_var = &this->breakFlags;
    if ((this->breakFlags != 1) || Part_break(&this->actor, &this->bodyBreak, play, 1)) {
        if ((*new_var) != 0) {
            this->breakFlags = (*new_var) | 2;
        }
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            this->actor.speed = 0;
        }
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            if (this->actor.speed < 0.0f) {
                this->actor.speed += 0.05f;
            }
        }

        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 1, 0x1194, 0);
        if (Skeleton_Info2_anime_play(&this->skelAnime) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            attack_or_forward(this);
        }
    }
}

static void mode_down_init(EnSkb* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gStalchildDyingAnim, -4.0f);
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = -6.0f;
    }
    this->actionState = SKB_BEHAVIOR_DYING;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Part_Break_init(&this->bodyBreak, 18, play);
    this->breakFlags |= 4;
    Effect_sound_ct(play, &this->actor.projectedPos, NA_SE_EN_STALKID_DEAD, 1, 1, 0x28);
    En_Skb_actor_set_process(this, mode_down);
}

static void mode_down(EnSkb* this, PlayState* play) {
    if (Part_break(&this->actor, &this->bodyBreak, play, 1)) {
        if (this->actor.scale.x == 0.01f) {
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x10);
        } else if (this->actor.scale.x <= 0.015f) {
            Item_set0(play, &this->actor.world.pos, ITEM00_RUPEE_BLUE);
        } else {
            Item_set0(play, &this->actor.world.pos, ITEM00_RUPEE_RED);
            Item_set0(play, &this->actor.world.pos, ITEM00_RUPEE_RED);
            Item_set0(play, &this->actor.world.pos, ITEM00_RUPEE_RED);
        }

        this->breakFlags |= 8;
        Actor_delete(&this->actor);
    }
}

static void damage_proc(EnSkb* this, PlayState* play) {
    s16 pad;
    s32 i;
    Vec3f flamePos;
    s16 scale;
    s16 colorFilterDuration;
    Player* player;

    if ((this->actionState != SKB_BEHAVIOR_DYING) &&
        (this->actor.bgCheckFlags & (BGCHECKFLAG_WATER | BGCHECKFLAG_WATER_TOUCH)) &&
        (this->actor.depthInWater >= 40.0f)) {
        this->actor.colChkInfo.health = 0;
        this->setColliderAT = false;
        mode_down_init(this, play);
    } else if (this->actionState >= SKB_BEHAVIOR_ATTACKING) {
        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            if (this->actor.colChkInfo.damageEffect != 6) {
                this->lastDamageEffect = this->actor.colChkInfo.damageEffect;
                Hit_bit_set(&this->actor, &this->collider.elements[1].base, true);
                this->setColliderAT = false;
                if (this->actor.colChkInfo.damageEffect == 1) {
                    if (this->actionState != SKB_BEHAVIOR_STUNNED) {
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA,
                                             80);
                        hp_down(&this->actor);
                        mode_paralyze_init(this);
                    }
                } else {
                    colorFilterDuration = 8;
                    if (this->actor.colChkInfo.damageEffect == 7) {
                        scale = this->actor.scale.y * 7500.0f;
                        for (i = 4; i >= 0; i--) {
                            flamePos = this->actor.world.pos;
                            flamePos.x += rnd_fx(20.0f);
                            flamePos.z += rnd_fx(20.0f);
                            flamePos.y += (fqrand() * 25.0f);
                            Effect_En_Fire_ct(play, &this->actor, &flamePos, scale, 0, 0, -1);
                        }
                        colorFilterDuration = 25;
                    }
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA,
                                         colorFilterDuration);
                    if (!hp_down(&this->actor)) {
                        mode_down_init(this, play);
                        return;
                    }
                    player = GET_PLAYER(play);
                    if (this->breakFlags == 0) {
                        if ((this->actor.colChkInfo.damageEffect == 0xD) ||
                            ((this->actor.colChkInfo.damageEffect == 0xE) &&
                             ((player->meleeWeaponAnimation >= PLAYER_MWA_RIGHT_SLASH_1H &&
                               player->meleeWeaponAnimation <= PLAYER_MWA_LEFT_COMBO_2H) ||
                              (player->meleeWeaponAnimation == PLAYER_MWA_BACKSLASH_RIGHT ||
                               player->meleeWeaponAnimation == PLAYER_MWA_BACKSLASH_LEFT)))) {
                            Part_Break_init(&this->bodyBreak, 2, play);
                            this->breakFlags = 1; // setup for head bodybreak
                        }
                    }
                    mode_damage_init(this);
                }
            }
        }
    }
}

void En_Skb_actor_move(Actor* thisx, PlayState* play) {
    EnSkb* this = (EnSkb*)thisx;
    s32 pad;

    damage_proc(this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 15.0f, 30.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    this->actionFunc(this, play);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += (3000.0f * this->actor.scale.y);
    if (this->setColliderAT) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->actionState >= SKB_BEHAVIOR_ATTACKING) {
        if ((this->actor.colorFilterTimer == 0) || ((this->actor.colorFilterParams & 0x4000) == 0)) {

            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

s32 skb_display1(PlayState* play2, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSkb* this = (EnSkb*)thisx;
    PlayState* play = (PlayState*)play2;
    s16 color;

    if (limbIndex == 11) {
        if ((this->breakFlags & 2) == 0) { // head limb, head is still attached
            OPEN_DISPS(play->state.gfxCtx, "../z_en_skb.c", 972);
            color = ABS((s16)(sin_s(play->gameplayFrames * 0x1770) * 95.0f)) + 160;
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, color, color, color, 255);
            CLOSE_DISPS(play->state.gfxCtx, "../z_en_skb.c", 978);
        } else {
            *dList = NULL;
        }
    } else if ((limbIndex == 12) && ((this->breakFlags & 2) != 0)) { // jaw limb, don't draw if headless
        *dList = NULL;
    }
    return 0;
}

void skb_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnSkb* this = (EnSkb*)thisx;

    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->collider);

    if ((this->breakFlags ^ 1) == 0) {
        Part_Break_Get(&this->bodyBreak, limbIndex, 11, 12, 18, dList, BODYBREAK_OBJECT_SLOT_DEFAULT);
    } else if ((this->breakFlags ^ (this->breakFlags | 4)) == 0) {
        Part_Break_Get(&this->bodyBreak, limbIndex, 0, 18, 18, dList, BODYBREAK_OBJECT_SLOT_DEFAULT);
    }
}

void En_Skb_actor_draw(Actor* thisx, PlayState* play) {
    EnSkb* this = (EnSkb*)thisx;
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, skb_display1,
                      skb_display2, &this->actor);
}
