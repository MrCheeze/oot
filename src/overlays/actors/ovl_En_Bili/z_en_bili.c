/*
 * File: z_en_bili.c
 * Overlay: ovl_En_Bili
 * Description: Biri (small jellyfish-like enemy)
 */

#include "z_en_bili.h"
#include "versions.h"
#include "assets/objects/object_bl/object_bl.h"

#define FLAGS \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

void En_Bili_actor_ct(Actor* thisx, PlayState* play);
void En_Bili_actor_dt(Actor* thisx, PlayState* play);
void En_Bili_actor_move(Actor* thisx, PlayState* play2);
void En_Bili_actor_draw(Actor* thisx, PlayState* play);

static void mode_move_init(EnBili* this);
static void mode_appear_init(EnBili* this);
static void mode_move(EnBili* this, PlayState* play);
static void mode_appear(EnBili* this, PlayState* play);
static void mode_elect(EnBili* this, PlayState* play);
static void mode_jump(EnBili* this, PlayState* play);
static void mode_chase(EnBili* this, PlayState* play);
static void mode_escape(EnBili* this, PlayState* play);
static void mode_reverse(EnBili* this, PlayState* play);
static void mode_damage(EnBili* this, PlayState* play);
static void mode_down(EnBili* this, PlayState* play);
static void mode_stop(EnBili* this, PlayState* play);
static void mode_drop(EnBili* this, PlayState* play);

ActorProfile En_Bili_Profile = {
    /**/ ACTOR_EN_BILI,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BL,
    /**/ sizeof(EnBili),
    /**/ En_Bili_actor_ct,
    /**/ En_Bili_actor_dt,
    /**/ En_Bili_actor_move,
    /**/ En_Bili_actor_draw,
};

static ColliderCylinderInit BiliAllPipeData = {
    {
        COL_MATERIAL_HIT8,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x03, 0x08 },
        { 0xFFCFFFFF, 0x01, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 9, 28, -20, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 BiliStatusData = { 1, 9, 28, -20, 30 };

typedef enum BiriDamageEffect {
    /* 0x0 */ BIRI_DMGEFF_NONE,
    /* 0x1 */ BIRI_DMGEFF_DEKUNUT,
    /* 0x2 */ BIRI_DMGEFF_FIRE,
    /* 0x3 */ BIRI_DMGEFF_ICE,
    /* 0xE */ BIRI_DMGEFF_SLINGSHOT = 0xE,
    /* 0xF */ BIRI_DMGEFF_SWORD
} BiriDamageEffect;

static DamageTable BiliBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, BIRI_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(0, BIRI_DMGEFF_SLINGSHOT),
    /* Explosive     */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(1, BIRI_DMGEFF_NONE),
    /* Normal arrow  */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Kokiri sword  */ DMG_ENTRY(1, BIRI_DMGEFF_SWORD),
    /* Master sword  */ DMG_ENTRY(2, BIRI_DMGEFF_SWORD),
    /* Giant's Knife */ DMG_ENTRY(4, BIRI_DMGEFF_SWORD),
    /* Fire arrow    */ DMG_ENTRY(4, BIRI_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(4, BIRI_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, BIRI_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(4, BIRI_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, BIRI_DMGEFF_NONE),
    /* Giant spin    */ DMG_ENTRY(4, BIRI_DMGEFF_NONE),
    /* Master spin   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Kokiri jump   */ DMG_ENTRY(2, BIRI_DMGEFF_NONE),
    /* Giant jump    */ DMG_ENTRY(8, BIRI_DMGEFF_NONE),
    /* Master jump   */ DMG_ENTRY(4, BIRI_DMGEFF_NONE),
    /* Unknown 1     */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, BIRI_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, BIRI_DMGEFF_NONE),
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_BIRI, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_STOP),
};

void En_Bili_actor_ct(Actor* thisx, PlayState* play) {
    EnBili* this = (EnBili*)thisx;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 17.0f);
    this->actor.shape.shadowAlpha = 155;
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gBiriSkel, &gBiriDefaultAnim, this->jointTable, this->morphTable,
                   EN_BILI_LIMB_MAX);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &BiliAllPipeData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, &BiliBtlData, &BiliStatusData);
    this->playFlySfx = false;

    if (this->actor.params == EN_BILI_TYPE_NORMAL) {
        mode_move_init(this);
    } else {
        mode_appear_init(this);
    }
}

void En_Bili_actor_dt(Actor* thisx, PlayState* play) {
    EnBili* this = (EnBili*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

// Setup Action Functions

static void mode_move_init(EnBili* this) {
    this->actor.speed = 0.7f;
    this->collider.elem.acDmgInfo.effect = 1; // Shock?
    this->timer = 32;
    this->actor.home.pos.y = this->actor.world.pos.y;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->collider.base.atFlags |= AT_ON;
    this->collider.base.acFlags |= AC_ON;
    this->actionFunc = mode_move;
}

/**
 * Separates the Biri spawned by a dying EnVali.
 */
static void mode_appear_init(EnBili* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gBiriDefaultAnim);
    this->timer = 25;
    this->actor.velocity.y = 6.0f;
    this->actor.gravity = -0.3f;
    this->collider.base.atFlags &= ~AT_ON;
    this->actionFunc = mode_appear;
    this->actor.speed = 3.0f;
}

/**
 * Used for both touching player/player's shield and being hit with sword. What to do next is determined by params.
 */
static void mode_elect_init(EnBili* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gBiriDischargeLightningAnim);
    this->timer = 10;
    this->actionFunc = mode_elect;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = -1.0f;
}

static void mode_jump_init(EnBili* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gBiriClimbAnim);
    this->collider.base.atFlags &= ~AT_ON;
    this->actionFunc = mode_jump;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
}

static void mode_chase_init(EnBili* this) {
    this->actor.speed = 1.2f;
    this->actionFunc = mode_chase;
}

static void mode_escape_init(EnBili* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gBiriDefaultAnim);
    this->timer = 96;
    this->actor.speed = 0.9f;
    this->collider.base.atFlags |= AT_ON;
    this->actionFunc = mode_escape;
    this->actor.home.pos.y = this->actor.world.pos.y;
}

static void mode_reverse_init(EnBili* this) {
    if (this->skelAnime.animation != &gBiriDefaultAnim) {
        Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gBiriDefaultAnim);
    }

    this->actor.world.rot.y = Actor_search_position_angleY(&this->actor, &this->collider.base.ac->prevPos) + 0x8000;
    this->actor.world.rot.x = Actor_search_position_angleX(&this->actor, &this->collider.base.ac->prevPos);
    this->actionFunc = mode_reverse;
    this->actor.speed = 5.0f;
}

/**
 * Used for both fire damage and generic damage
 */
static void mode_damage_init(EnBili* this) {
    if (this->actionFunc == mode_jump) {
        Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gBiriDefaultAnim);
    }

    this->timer = 20;
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.speed = 0.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_XLU, 20);
    this->actionFunc = mode_damage;
}

static void mode_down_init(EnBili* this) {
    this->timer = 18;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = mode_down;
    this->actor.speed = 0.0f;
}

/**
 * Falls to ground
 */
static void mode_stop_init(EnBili* this) {
    this->timer = 80;
    this->collider.elem.acDmgInfo.effect = 0;
    this->actor.gravity = -1.0f;
    this->actor.speed = 0.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 150, COLORFILTER_BUFFLAG_XLU, 80);
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->collider.base.atFlags &= ~AT_ON;
    this->actionFunc = mode_stop;
}

static void mode_drop_init(EnBili* this, PlayState* play) {
    s32 i;
    Vec3f effectPos;

    if (!(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
        this->actor.gravity = -1.0f;
    }

    this->actor.velocity.y = 0.0f;
    effectPos.y = this->actor.world.pos.y - 15.0f;

    for (i = 0; i < 8; i++) {

        effectPos.x = this->actor.world.pos.x + ((i & 1) ? 7.0f : -7.0f);
        effectPos.y += 2.5f;
        effectPos.z = this->actor.world.pos.z + ((i & 4) ? 7.0f : -7.0f);

        Effect_En_Ice_ct0(play, &this->actor, &effectPos, 150, 150, 150, 250, 235, 245, 255,
                                       (fqrand() * 0.2f) + 0.7f);
    }

    this->actor.speed = 0.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 150, COLORFILTER_BUFFLAG_XLU, 10);
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->timer = 300;
    this->actionFunc = mode_drop;
}

// Miscellaneous

/**
 * Changes the texture displayed on the oral arms limb using the current frame.
 */
void set_bili_txt_num(EnBili* this) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 temp; // Not strictly necessary, but avoids a few s16 casts

    if (this->actionFunc == mode_elect) {
        temp = 3 - curFrame;
        this->tentaclesTexIndex = (ABS(temp) + 5) % 8;
    } else if (this->actionFunc == mode_jump) {
        if (curFrame <= 9) {
            temp = curFrame >> 1;
            this->tentaclesTexIndex = CLAMP_MAX(temp, 3);
        } else if (curFrame <= 18) {
            temp = 17 - curFrame;
            this->tentaclesTexIndex = CLAMP_MIN(temp, 0) >> 1;
        } else if (curFrame <= 36) {
            this->tentaclesTexIndex = ((36 - curFrame) / 3) + 2;
        } else {
            this->tentaclesTexIndex = (40 - curFrame) >> 1;
        }
    } else {
        this->tentaclesTexIndex = curFrame >> 1;
    }
}

/**
 * Tracks Player height, with oscillation, and moves away from walls
 */
void bili_check_move_pos_angle(EnBili* this) {
    f32 playerHeight = this->actor.world.pos.y + this->actor.yDistToPlayer;
    f32 heightOffset = ((this->actionFunc == mode_escape) ? 100.0f : 40.0f);
    f32 baseHeight = CLAMP_MIN(this->actor.floorHeight, playerHeight);

    chase_f(&this->actor.home.pos.y, baseHeight + heightOffset, 1.0f);
    this->actor.world.pos.y = this->actor.home.pos.y + (sinf(this->timer * (M_PI / 16)) * 3.0f);

    // Turn around if touching wall
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.world.rot.y = this->actor.wallYaw;
    }
}

// Action functions

static void mode_move(EnBili* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    if (!(this->timer % 4)) {
        this->actor.world.rot.y += rnd_fx(1820.0f);
    }

    bili_check_move_pos_angle(this);

    if (this->timer == 0) {
        this->timer = 32;
    }

    if ((this->actor.xzDistToPlayer < 160.0f) && (fabsf(this->actor.yDistToPlayer) < 45.0f)) {
        mode_chase_init(this);
    }
}

static void mode_appear(EnBili* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        mode_move_init(this);
    }
}

static void mode_elect(EnBili* this, PlayState* play) {
    static Color_RGBA8 lightning_prim = { 255, 255, 255, 255 };
    static Color_RGBA8 lightning_env = { 200, 255, 255, 255 };
    s32 i;
    Vec3f effectPos;
    s16 effectYaw;

    for (i = 0; i < 4; i++) {
        if (!((this->timer + (i << 1)) % 4)) {
            effectYaw = (s16)rnd_fx(12288.0f) + (i * 0x4000) + 0x2000;
            effectPos.x = rnd_fx(5.0f) + this->actor.world.pos.x;
            effectPos.y = (fqrand() * 5.0f) + this->actor.world.pos.y + 2.5f;
            effectPos.z = rnd_fx(5.0f) + this->actor.world.pos.z;
            Effect_SS_Lightning_sc_cl_co_ct(play, &effectPos, &lightning_prim, &lightning_env, 15, effectYaw, 6, 2);
        }
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    Actor_level_SE_set(&this->actor, NA_SE_EN_BIRI_SPARK - SFX_FLAG);

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.velocity.y *= -1.0f;

    if ((this->timer == 0) && Skeleton_Info_frame_check(&this->skelAnime, 0.0f)) {
        if (this->actor.params == EN_BILI_TYPE_DYING) {
            mode_down_init(this);
        } else {
            mode_jump_init(this);
        }
    }
}

static void mode_jump(EnBili* this, PlayState* play) {
    s32 skelAnimeUpdate = Skeleton_Info2_anime_play(&this->skelAnime);
    f32 curFrame = this->skelAnime.curFrame;

    if (Skeleton_Info_frame_check(&this->skelAnime, 9.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_BIRI_JUMP);
    }

    if (curFrame > 9.0f) {
        add_calc2(&this->actor.world.pos.y, this->actor.world.pos.y + this->actor.yDistToPlayer + 100.0f, 0.5f,
                       5.0f);
    }

    if (skelAnimeUpdate) {
        mode_escape_init(this);
    }
}

static void mode_chase(EnBili* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    adds(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 2, 1820);

    if (this->timer != 0) {
        this->timer--;
    }

    bili_check_move_pos_angle(this);

    if (this->timer == 0) {
        this->timer = 32;
    }

    if (this->actor.xzDistToPlayer > 200.0f) {
        mode_move_init(this);
    }
}

static void mode_escape(EnBili* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    chase_angle(&this->actor.world.rot.y, (s16)(this->actor.yawTowardsPlayer + 0x8000), 910);
    bili_check_move_pos_angle(this);

    if (this->timer == 0) {
        mode_move_init(this);
    }
}

static void mode_reverse(EnBili* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (chase_f(&this->actor.speed, 0.0f, 0.3f)) {
        this->actor.world.rot.y += 0x8000;
        mode_move_init(this);
    }
}

static void mode_damage(EnBili* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW) {
        this->actor.colorFilterTimer = 20;
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            mode_down_init(this);
        }
    }
}

static void mode_down(EnBili* this, PlayState* play) {
    static Vec3f pvec = { 0.0f, 0.0f, 0.0f };
    static Vec3f pacc = { 0.0f, 0.0f, 0.0f };
    s16 effectScale;
    Vec3f effectPos;
    s32 i;

    if (this->actor.draw != NULL) {
        if (this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW) {
            return;
        }
        this->actor.draw = NULL;
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x50);
    }

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer != 0) {
        for (i = 0; i < 2; i++) {
            effectPos.x = ((fqrand() * 10.0f) + this->actor.world.pos.x) - 5.0f;
            effectPos.y = ((fqrand() * 5.0f) + this->actor.world.pos.y) - 2.5f;
            effectPos.z = ((fqrand() * 10.0f) + this->actor.world.pos.z) - 5.0f;

            pvec.y = fqrand() + 1.0f;
            effectScale = get_random_timer(40, 40);

            if (fqrand() < 0.7f) {
                Effect_SS_Dt_Bubble_sc_co_ct(play, &effectPos, &pvec, &pacc, effectScale, 25, 2,
                                                   1);
            } else {
                Effect_SS_Dt_Bubble_sc_co_ct(play, &effectPos, &pvec, &pacc, effectScale, 25, 0,
                                                   1);
            }
        }
    } else {
        Actor_delete(&this->actor);
    }

    if (this->timer == 14) {
        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EN_BIRI_BUBLE);
    }
}

static void mode_stop(EnBili* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
    }

    if (this->timer == 0) {
        mode_move_init(this);
    }
}

static void mode_drop(EnBili* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (!(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
        this->actor.gravity = -1.0f;
    }

#if OOT_VERSION < NTSC_1_1
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) || (this->actor.floorHeight == BGCHECK_Y_MIN))
#else
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.floorHeight == BGCHECK_Y_MIN))
#endif
    {
        this->actor.colorFilterTimer = 0;
        mode_down_init(this);
    } else {
        this->actor.colorFilterTimer = 10;
    }
}

void En_Bili_damage_proc(EnBili* this, PlayState* play) {
    u8 damageEffect;

    if ((this->actor.colChkInfo.health != 0) && (this->collider.base.acFlags & AC_HIT)) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elem, true);

        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (hp_down(&this->actor) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_BIRI_DEAD);
                Actor_info_finish(play, &this->actor);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            }

            damageEffect = this->actor.colChkInfo.damageEffect;

            if (damageEffect == BIRI_DMGEFF_DEKUNUT) {
                if (this->actionFunc != mode_stop) {
                    mode_stop_init(this);
                }
            } else if (damageEffect == BIRI_DMGEFF_SWORD) {
                if (this->actionFunc != mode_stop) {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_XLU, 10);

                    if (this->actor.colChkInfo.health == 0) {
                        this->actor.params = EN_BILI_TYPE_DYING;
                    }
                    mode_elect_init(this);
                } else {
                    mode_damage_init(this);
                }
            } else if (damageEffect == BIRI_DMGEFF_FIRE) {
                mode_damage_init(this);
                this->timer = 2;
            } else if (damageEffect == BIRI_DMGEFF_ICE) {
                mode_drop_init(this, play);
            } else if (damageEffect == BIRI_DMGEFF_SLINGSHOT) {
                mode_reverse_init(this);
            } else {
                mode_damage_init(this);
            }

            if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & DMG_ARROW) {
                this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            }
        }
    }
}

void En_Bili_actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBili* this = (EnBili*)thisx;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        mode_elect_init(this);
    }

    En_Bili_damage_proc(this, play);
    this->actionFunc(this, play);

    if (this->actionFunc != mode_down) {
        set_bili_txt_num(this);
        if (Skeleton_Info_frame_check(&this->skelAnime, 9.0f)) {
            if ((this->actionFunc == mode_move) || (this->actionFunc == mode_escape) ||
                (this->actionFunc == mode_chase) || (this->actionFunc == mode_reverse)) {
                if (this->playFlySfx) {
                    Actor_SE_set(&this->actor, NA_SE_EN_BIRI_FLY);
                    this->playFlySfx = false;
                } else {
                    this->playFlySfx = true;
                }
            }
        }
        if (this->actionFunc == mode_reverse) {
            Actor_position_moveF_XY(&this->actor);
        } else {
            Actor_position_moveF(&this->actor);
        }

        Actor_BGcheck2(play, &this->actor, 5.0f, this->collider.dim.radius, this->collider.dim.height,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

        if (this->collider.base.atFlags & AT_ON) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }

        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        Actor_world_to_eye(&this->actor, 0.0f);
    }
}

// Draw and associated functions

void bili_outside_scale_anm(EnBili* this, f32 frame, Vec3f* arg2) {
    f32 cos;
    f32 sin;

    if (this->actionFunc == mode_elect) {
        arg2->y = 1.0f - (sinf((M_PI * 0.16667f) * frame) * 0.26f);
    } else if (this->actionFunc == mode_jump) {
        if (frame <= 8.0f) {
            arg2->y = (cosf((M_PI * 0.125f) * frame) * 0.15f) + 0.85f;
        } else if (frame <= 18.0f) {
            cos = cosf((frame - 8.0f) * (M_PI * 0.1f));
            arg2->y = 1.0f - (0.3f * cos);
            arg2->x = (0.2f * cos) + 0.8f;
        } else {
            cos = cosf((frame - 18.0f) * (M_PI * 0.0227f));
            arg2->y = (0.31f * cos) + 1.0f;
            arg2->x = 1.0f - (0.4f * cos);
        }

        arg2->z = arg2->x;
    } else if (this->actionFunc == mode_stop) {
        sin = sinf((M_PI * 0.1f) * this->timer) * 0.08f;
        arg2->x -= sin;
        arg2->y += sin;
        arg2->z -= sin;
    } else {
        arg2->y = (cosf((M_PI * 0.125f) * frame) * 0.13f) + 0.87f;
    }
}

void bili_inside_scale_anm(EnBili* this, f32 frame, Vec3f* arg2) {
    f32 cos;
    f32 sin;

    if (this->actionFunc == mode_elect) {
        arg2->y = (sinf((M_PI * 0.16667f) * frame) * 0.2f) + 1.0f;
    } else if (this->actionFunc == mode_jump) {
        if (frame <= 8.0f) {
            arg2->x = 1.125f - (cosf((M_PI * 0.125f) * frame) * 0.125f);
        } else if (frame <= 18.0f) {
            cos = cosf((frame - 8.0f) * (M_PI * 0.1f));
            arg2->x = (0.275f * cos) + 0.975f;
            arg2->y = 1.25f - (0.25f * cos);
        } else {
            cos = cosf((frame - 18.0f) * (M_PI * 0.0227f));
            arg2->x = 1.0f - (0.3f * cos);
            arg2->y = (0.48f * cos) + 1.0f;
        }
        arg2->z = arg2->x;
    } else if (this->actionFunc == mode_stop) {
        sin = sinf((M_PI * 0.1f) * this->timer) * 0.08f;
        arg2->x += sin;
        arg2->y -= sin;
        arg2->z += sin;
    } else {
        arg2->y = 1.1f - (cosf((M_PI * 0.125f) * frame) * 0.1f);
    }
}

void bili_hair_scale_anm(EnBili* this, f32 frame, Vec3f* arg2) {
    f32 cos;

    if (this->actionFunc == mode_jump) {
        if (frame <= 8.0f) {
            cos = cosf((M_PI * 0.125f) * frame);
            arg2->x = 1.125f - (0.125f * cos);
            arg2->y = (0.3f * cos) + 0.7f;
        } else if (frame <= 18.0f) {
            cos = cosf((frame - 8.0f) * (M_PI * 0.1f));
            arg2->x = (0.325f * cos) + 0.925f;
            arg2->y = 0.95f - (0.55f * cos);
        } else {
            cos = cosf((frame - 18.0f) * (M_PI * 0.0227f));
            arg2->x = 1.0f - (0.4f * cos);
            arg2->y = (0.52f * cos) + 1.0f;
        }
        arg2->z = arg2->x;
    }
}

s32 en_bili_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                            Gfx** gfx) {
    EnBili* this = (EnBili*)thisx;
    Vec3f limbScale = { 1.0f, 1.0f, 1.0f };
    f32 curFrame = this->skelAnime.curFrame;

    if (limbIndex == EN_BILI_LIMB_OUTER_HOOD) {
        bili_outside_scale_anm(this, curFrame, &limbScale);
    } else if (limbIndex == EN_BILI_LIMB_INNER_HOOD) {
        bili_inside_scale_anm(this, curFrame, &limbScale);
    } else if (limbIndex == EN_BILI_LIMB_TENTACLES) {
        bili_hair_scale_anm(this, curFrame, &limbScale);
        rot->y = (getRealCameraAngleY(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y) + 0x8000;
    }

    Matrix_scale(limbScale.x, limbScale.y, limbScale.z, MTXMODE_APPLY);
    return false;
}

static void* bili_txt[] = {
    gBiriTentacles0Tex, gBiriTentacles1Tex, gBiriTentacles2Tex, gBiriTentacles3Tex,
    gBiriTentacles4Tex, gBiriTentacles5Tex, gBiriTentacles6Tex, gBiriTentacles7Tex,
};

#include "assets/overlays/ovl_En_Bili/z_en_bili.c"

void En_Bili_actor_draw(Actor* thisx, PlayState* play) {
    EnBili* this = (EnBili*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_bili.c", 1521);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    this->tentaclesTexIndex = CLAMP_MAX(this->tentaclesTexIndex, 7);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(bili_txt[this->tentaclesTexIndex]));

    if ((this->actionFunc == mode_elect) && ((this->timer & 1) != 0)) {
        gSPSegment(POLY_XLU_DISP++, 0x09, reverse_combine_mode);
    } else {
        gSPSegment(POLY_XLU_DISP++, 0x09, normal_combine_mode);
    }

    POLY_XLU_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_bili_display1,
                                   NULL, this, POLY_XLU_DISP);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_bili.c", 1552);
}
