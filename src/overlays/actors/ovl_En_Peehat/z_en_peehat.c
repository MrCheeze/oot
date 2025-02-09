#include "z_en_peehat.h"
#include "assets/objects/object_peehat/object_peehat.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT)

#define GROUND_HOVER_HEIGHT 75.0f
#define MAX_LARVA 3

void En_Peehat_Actor_ct(Actor* thisx, PlayState* play);
void En_Peehat_Actor_dt(Actor* thisx, PlayState* play);
void En_Peehat_move(Actor* thisx, PlayState* play);
void En_Peehat_display(Actor* thisx, PlayState* play);

void En_Peehat_Actor_mode_wait_init(EnPeehat* this);
void En_Peehat_Actor_mode_wait2_init(EnPeehat* this);
void En_Peehat_Actor_mode_forward2_init(EnPeehat* this);
void En_Peehat_Actor_mode_wait(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_wake_init(EnPeehat* this);
void En_Peehat_Actor_mode_wait2(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_wake2_init(EnPeehat* this);
void En_Peehat_Actor_mode_wait3(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_down2_init(EnPeehat* this);
void En_Peehat_Actor_mode_wake(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_rnd_move_init(EnPeehat* this);
void En_Peehat_Actor_mode_wake2(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_forward(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_home_init(EnPeehat* this);
void En_Peehat_Actor_mode_down_init(EnPeehat* this);
void En_Peehat_Actor_mode_forward2(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_shock_init(EnPeehat* this);
void En_Peehat_Actor_mode_down(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_down2(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_rnd_move(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_home(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_shock(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_paralyze(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_damage(EnPeehat* this, PlayState* play);
void En_Peehat_Actor_mode_dead_init(EnPeehat* this);
void En_Peehat_Actor_mode_dead(EnPeehat* this, PlayState* play);

ActorProfile En_Peehat_Profile = {
    /**/ ACTOR_EN_PEEHAT,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_PEEHAT,
    /**/ sizeof(EnPeehat),
    /**/ En_Peehat_Actor_ct,
    /**/ En_Peehat_Actor_dt,
    /**/ En_Peehat_move,
    /**/ En_Peehat_display,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_WOOD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 50, 160, -70, { 0, 0, 0 } },
};

static ColliderJntSphElementInit JntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit JntSphData = {
    {
        COL_MATERIAL_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    JntSphElemData,
};

static ColliderQuadInit AtInfoData = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum DamageEffect {
    /* 00 */ PEAHAT_DMG_EFF_ATTACK = 0,
    /* 06 */ PEAHAT_DMG_EFF_LIGHT_ICE_ARROW = 6,
    /* 12 */ PEAHAT_DMG_EFF_FIRE = 12,
    /* 13 */ PEAHAT_DMG_EFF_HOOKSHOT = 13,
    /* 14 */ PEAHAT_DMG_EFF_BOOMERANG = 14,
    /* 15 */ PEAHAT_DMG_EFF_NUT = 15
} DamageEffect;

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, PEAHAT_DMG_EFF_NUT),
    /* Deku stick    */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Slingshot     */ DMG_ENTRY(1, PEAHAT_DMG_EFF_ATTACK),
    /* Explosive     */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Boomerang     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_BOOMERANG),
    /* Normal arrow  */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Hammer swing  */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Hookshot      */ DMG_ENTRY(2, PEAHAT_DMG_EFF_HOOKSHOT),
    /* Kokiri sword  */ DMG_ENTRY(1, PEAHAT_DMG_EFF_ATTACK),
    /* Master sword  */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Giant's Knife */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Fire arrow    */ DMG_ENTRY(4, PEAHAT_DMG_EFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Light arrow   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Unk arrow 1   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Unk arrow 2   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Unk arrow 3   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Fire magic    */ DMG_ENTRY(3, PEAHAT_DMG_EFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_LIGHT_ICE_ARROW),
    /* Light magic   */ DMG_ENTRY(0, PEAHAT_DMG_EFF_LIGHT_ICE_ARROW),
    /* Shield        */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Mirror Ray    */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Kokiri spin   */ DMG_ENTRY(1, PEAHAT_DMG_EFF_ATTACK),
    /* Giant spin    */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Master spin   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Kokiri jump   */ DMG_ENTRY(2, PEAHAT_DMG_EFF_ATTACK),
    /* Giant jump    */ DMG_ENTRY(8, PEAHAT_DMG_EFF_ATTACK),
    /* Master jump   */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Unknown 1     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Unblockable   */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
    /* Hammer jump   */ DMG_ENTRY(4, PEAHAT_DMG_EFF_ATTACK),
    /* Unknown 2     */ DMG_ENTRY(0, PEAHAT_DMG_EFF_ATTACK),
};

typedef enum PeahatState {
    /* 00 */ PEAHAT_STATE_DYING,
    /* 01 */ PEAHAT_STATE_EXPLODE,
    /* 03 */ PEAHAT_STATE_3 = 3,
    /* 04 */ PEAHAT_STATE_4,
    /* 05 */ PEAHAT_STATE_FLY,
    /* 07 */ PEAHAT_STATE_ATTACK_RECOIL = 7,
    /* 08 */ PEAHAT_STATE_8,
    /* 09 */ PEAHAT_STATE_9,
    /* 10 */ PEAHAT_STATE_LANDING,
    /* 12 */ PEAHAT_STATE_RETURN_HOME = 12,
    /* 13 */ PEAHAT_STATE_STUNNED,
    /* 14 */ PEAHAT_STATE_SEEK_PLAYER,
    /* 15 */ PEAHAT_STATE_15
} PeahatState;

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 700, ICHAIN_STOP),
};

void En_Peehat_actor_set_process(EnPeehat* this, EnPeehatActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Peehat_Actor_ct(Actor* thisx, PlayState* play) {
    EnPeehat* this = (EnPeehat*)thisx;

    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, 36.0f * 0.001f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gPeehatSkel, &gPeehatRisingAnim, this->jointTable, this->morphTable, 24);
    Shape_Info_init(&this->actor.shape, 100.0f, Actor_shadow_circle, 27.0f);
    this->actor.focus.pos = this->actor.world.pos;
    this->unk_2D4 = 0;
    this->actor.world.rot.y = 0;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.health = 6;
    this->actor.colChkInfo.damageTable = &btl_data;
    this->actor.floorHeight = this->actor.world.pos.y;
    ClObjPipe_ct(play, &this->colCylinder);
    ClObjPipe_set5(play, &this->colCylinder, &this->actor, &OcInfoData);
    ClObjSwrd_ct(play, &this->colQuad);
    ClObjSwrd_set5(play, &this->colQuad, &this->actor, &AtInfoData);
    ClObjJntSph_ct(play, &this->colJntSph);
    ClObjJntSph_set5_nzm(play, &this->colJntSph, &this->actor, &JntSphData, this->colJntSphItemList);

    this->actor.naviEnemyId = NAVI_ENEMY_PEAHAT;
    this->xzDistToRise = 740.0f;
    this->xzDistMax = 1200.0f;
    this->actor.cullingVolumeDistance = 4000.0f;
    this->actor.cullingVolumeScale = 800.0f;
    this->actor.cullingVolumeDownward = 1800.0f;
    switch (this->actor.params) {
        case PEAHAT_TYPE_GROUNDED:
            En_Peehat_Actor_mode_wait_init(this);
            break;
        case PEAHAT_TYPE_FLYING:
            this->actor.cullingVolumeDistance = 4200.0f;
            this->xzDistToRise = 2800.0f;
            this->xzDistMax = 1400.0f;
            En_Peehat_Actor_mode_wait2_init(this);
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            break;
        case PEAHAT_TYPE_LARVA:
            this->actor.scale.x = this->actor.scale.z = 0.006f;
            this->actor.scale.y = 0.003f;
            this->colCylinder.dim.radius = 25;
            this->colCylinder.dim.height = 15;
            this->colCylinder.dim.yShift = -5;
            this->colCylinder.elem.acDmgInfo.dmgFlags = DMG_ARROW | DMG_SLINGSHOT;
            this->colQuad.base.atFlags = AT_ON | AT_TYPE_ENEMY;
            this->colQuad.base.acFlags = AC_ON | AC_TYPE_PLAYER;
            this->actor.naviEnemyId = NAVI_ENEMY_PEAHAT_LARVA;
            En_Peehat_Actor_mode_forward2_init(this);
            break;
    }
}

void En_Peehat_Actor_dt(Actor* thisx, PlayState* play) {
    EnPeehat* this = (EnPeehat*)thisx;
    EnPeehat* parent;

    ClObjPipe_dt(play, &this->colCylinder);
    ClObjJntSph_dt_nzf(play, &this->colJntSph);

    // If PEAHAT_TYPE_LARVA, decrement total larva spawned
    if (this->actor.params > 0) {
        parent = (EnPeehat*)this->actor.parent;
        if (parent != NULL && parent->actor.update != NULL) {
            parent->unk_2FA--;
        }
    }
}

static void dust_ground_setx(PlayState* play, EnPeehat* this, Vec3f* pos, f32 arg3, s32 arg4, f32 arg5, f32 arg6) {
    Vec3f dustPos;
    Vec3f dustVel = { 0.0f, 8.0f, 0.0f };
    Vec3f dustAccel = { 0.0f, -1.5f, 0.0f };
    f32 rot; // radians
    s32 pScale;

    rot = (fqrand() - 0.5f) * 6.28f;
    dustPos.y = this->actor.floorHeight;
    dustPos.x = sinf_table(rot) * arg3 + pos->x;
    dustPos.z = cosf_table(rot) * arg3 + pos->z;
    dustAccel.x = (fqrand() - 0.5f) * arg5;
    dustAccel.z = (fqrand() - 0.5f) * arg5;
    dustVel.y += (fqrand() - 0.5f) * 4.0f;
    pScale = (fqrand() * 5 + 12) * arg6;
    Effect_Hahen_ct3(play, &dustPos, &dustVel, &dustAccel, arg4, pScale, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

/**
 * Handles being hit when on the ground
 */
void r_set(EnPeehat* this, PlayState* play) {
    this->colCylinder.base.acFlags &= ~AC_HIT;
    if ((play->gameplayFrames & 0xF) == 0) {
        Vec3f itemDropPos = this->actor.world.pos;

        itemDropPos.y += 70.0f;
        Item_Set_Std(play, &this->actor, &itemDropPos, 0x40);
        Item_Set_Std(play, &this->actor, &itemDropPos, 0x40);
        Item_Set_Std(play, &this->actor, &itemDropPos, 0x40);
        this->unk_2D4 = 240;
    } else {
        s32 i;

        this->colCylinder.base.acFlags &= ~AC_HIT;
        for (i = MAX_LARVA - this->unk_2FA; i > 0; i--) {
            Actor* larva =
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_PEEHAT,
                                   rnd_fx(25.0f) + this->actor.world.pos.x,
                                   rnd_fx(25.0f) + (this->actor.world.pos.y + 50.0f),
                                   rnd_fx(25.0f) + this->actor.world.pos.z, 0, 0, 0, PEAHAT_TYPE_LARVA);

            if (larva != NULL) {
                larva->velocity.y = 6.0f;
                larva->shape.rot.y = larva->world.rot.y = rnd_fx(0xFFFF);
                this->unk_2FA++;
            }
        }
        this->unk_2D4 = 8;
    }
    Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_DAMAGE);
}

void En_Peehat_Actor_mode_wait_init(EnPeehat* this) {
    Skeleton_Info2_init(&this->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, Si2_anime_end_frame(&gPeehatRisingAnim),
                     ANIMMODE_ONCE, 0.0f);
    this->seekPlayerTimer = 600;
    this->unk_2D4 = 0;
    this->unk_2FA = 0;
    this->state = PEAHAT_STATE_3;
    this->colCylinder.base.acFlags &= ~AC_HIT;
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_wait);
}

void En_Peehat_Actor_mode_wait(EnPeehat* this, PlayState* play) {
    if (IS_DAY) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        if (this->riseDelayTimer == 0) {
            if (this->actor.xzDistToPlayer < this->xzDistToRise) {
                En_Peehat_Actor_mode_wake_init(this);
            }
        } else {
            add_calc(&this->actor.shape.yOffset, -1000.0f, 1.0f, 10.0f, 0.0f);
            this->riseDelayTimer--;
        }
    } else {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        add_calc(&this->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
        if (this->unk_2D4 != 0) {
            this->unk_2D4--;
            if (this->unk_2D4 & 4) {
                add_calc(&this->scaleShift, 0.205f, 1.0f, 0.235f, 0.0f);
            } else {
                add_calc(&this->scaleShift, 0.0f, 1.0f, 0.005f, 0.0f);
            }
        } else if (this->colCylinder.base.acFlags & AC_HIT) {
            r_set(this, play);
        }
    }
}

void En_Peehat_Actor_mode_wait2_init(EnPeehat* this) {
    Skeleton_Info2_init(&this->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, Si2_anime_end_frame(&gPeehatRisingAnim),
                     ANIMMODE_ONCE, 0.0f);
    this->seekPlayerTimer = 400;
    this->unk_2D4 = 0;
    this->unk_2FA = 0; //! @bug: overwrites number of child larva spawned, allowing for more than MAX_LARVA spawns
    this->state = PEAHAT_STATE_4;
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_wait2);
}

void En_Peehat_Actor_mode_wait2(EnPeehat* this, PlayState* play) {
    if (IS_DAY) {
        if (this->actor.xzDistToPlayer < this->xzDistToRise) {
            En_Peehat_Actor_mode_wake2_init(this);
        }
    } else {
        add_calc(&this->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
        if (this->unk_2D4 != 0) {
            this->unk_2D4--;
            if (this->unk_2D4 & 4) {
                add_calc(&this->scaleShift, 0.205f, 1.0f, 0.235f, 0.0f);
            } else {
                add_calc(&this->scaleShift, 0.0f, 1.0f, 0.005f, 0.0f);
            }
        } else if (this->colCylinder.base.acFlags & AC_HIT) {
            r_set(this, play);
        }
    }
}

void En_Peehat_Actor_mode_wait3_init(EnPeehat* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gPeehatFlyingAnim);
    this->state = PEAHAT_STATE_FLY;
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_wait3);
}

void En_Peehat_Actor_mode_wait3(EnPeehat* this, PlayState* play) {
    Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (!IS_DAY || this->xzDistToRise < this->actor.xzDistToPlayer) {
        En_Peehat_Actor_mode_down2_init(this);
    } else if (this->actor.xzDistToPlayer < this->xzDistMax) {
        if (this->unk_2FA < MAX_LARVA && (play->gameplayFrames & 7) == 0) {
            Actor* larva = Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_PEEHAT,
                                              rnd_fx(25.0f) + this->actor.world.pos.x,
                                              rnd_fx(5.0f) + this->actor.world.pos.y,
                                              rnd_fx(25.0f) + this->actor.world.pos.z, 0, 0, 0, 1);
            if (larva != NULL) {
                larva->shape.rot.y = larva->world.rot.y = rnd_fx(0xFFFF);
                this->unk_2FA++;
            }
        }
    }
    this->bladeRot += this->bladeRotVel;
}

void En_Peehat_Actor_mode_wake_init(EnPeehat* this) {
    f32 lastFrame = Si2_anime_end_frame(&gPeehatRisingAnim);

    if (this->state != PEAHAT_STATE_STUNNED) {
        Skeleton_Info2_init(&this->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    }
    this->state = PEAHAT_STATE_8;
    this->animTimer = lastFrame;
    Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_UP);
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_wake);
}

void En_Peehat_Actor_mode_wake(EnPeehat* this, PlayState* play) {
    Vec3f pos;

    add_calc(&this->actor.shape.yOffset, 0.0f, 1.0f, 50.0f, 0.0f);
    if (add_calc_short_angle2(&this->bladeRotVel, 4000, 1, 800, 0) == 0) {
        if (this->animTimer != 0) {
            this->animTimer--;
            if (this->skelAnime.playSpeed == 0.0f) {
                if (this->animTimer == 0) {
                    this->animTimer = 40;
                    this->skelAnime.playSpeed = 1.0f;
                }
            }
        }
        if (Skeleton_Info2_anime_play(&this->skelAnime) || this->animTimer == 0) {
            En_Peehat_Actor_mode_rnd_move_init(this);
        } else {
            this->actor.world.pos.y += 6.5f;
        }
        if (this->actor.world.pos.y - this->actor.floorHeight < 80.0f) {
            pos = this->actor.world.pos;
            pos.y = this->actor.floorHeight;
            dust_fly_set2(play, &pos, 90.0f, 1, 0x96, 100, 1);
        }
    }
    dust_ground_setx(play, this, &this->actor.world.pos, 75.0f, 2, 1.05f, 2.0f);
    add_calc(&this->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    this->bladeRot += this->bladeRotVel;
}

void En_Peehat_Actor_mode_wake2_init(EnPeehat* this) {
    f32 lastFrame;

    lastFrame = Si2_anime_end_frame(&gPeehatRisingAnim);
    if (this->state != PEAHAT_STATE_STUNNED) {
        Skeleton_Info2_init(&this->skelAnime, &gPeehatRisingAnim, 0.0f, 3.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    }
    this->state = PEAHAT_STATE_9;
    this->animTimer = lastFrame;
    Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_UP);
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_wake2);
}

void En_Peehat_Actor_mode_wake2(EnPeehat* this, PlayState* play) {
    Vec3f pos;

    add_calc(&this->actor.shape.yOffset, 0.0f, 1.0f, 50.0f, 0.0f);
    if (add_calc_short_angle2(&this->bladeRotVel, 4000, 1, 800, 0) == 0) {
        if (this->animTimer != 0) {
            this->animTimer--;
            if (this->skelAnime.playSpeed == 0.0f) {
                if (this->animTimer == 0) {
                    this->animTimer = 40;
                    this->skelAnime.playSpeed = 1.0f;
                }
            }
        }
        if (Skeleton_Info2_anime_play(&this->skelAnime) || this->animTimer == 0) {
            //! @bug: overwrites number of child larva spawned, allowing for more than MAX_LARVA spawns
            this->unk_2FA = 0;
            En_Peehat_Actor_mode_wait3_init(this);
        } else {
            this->actor.world.pos.y += 18.0f;
        }
        if (this->actor.world.pos.y - this->actor.floorHeight < 80.0f) {
            pos = this->actor.world.pos;
            pos.y = this->actor.floorHeight;
            dust_fly_set2(play, &pos, 90.0f, 1, 0x96, 100, 1);
        }
    }
    dust_ground_setx(play, this, &this->actor.world.pos, 75.0f, 2, 1.05f, 2.0f);
    add_calc(&this->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    this->bladeRot += this->bladeRotVel;
}

void En_Peehat_Actor_mode_forward_init(EnPeehat* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gPeehatFlyingAnim);
    this->state = PEAHAT_STATE_SEEK_PLAYER;
    this->unk_2E0 = 0.0f;
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_forward);
}

void En_Peehat_Actor_mode_forward(EnPeehat* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    add_calc(&this->actor.speed, 3.0f, 1.0f, 0.25f, 0.0f);
    add_calc(&this->actor.world.pos.y, this->actor.floorHeight + 80.0f, 1.0f, 3.0f, 0.0f);
    if (this->seekPlayerTimer <= 0) {
        En_Peehat_Actor_mode_down_init(this);
        this->riseDelayTimer = 40;
    } else {
        this->seekPlayerTimer--;
    }
    if (IS_DAY && (search_position_distanceXZ(&this->actor.home.pos, &player->actor.world.pos) < this->xzDistMax)) {
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 1000, 0);
        if (this->unk_2FA != 0) {
            this->actor.shape.rot.y += 0x1C2;
        } else {
            this->actor.shape.rot.y -= 0x1C2;
        }
    } else {
        En_Peehat_Actor_mode_home_init(this);
    }
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc_short_angle2(&this->bladeRotVel, 4000, 1, 500, 0);
    this->bladeRot += this->bladeRotVel;
    add_calc(&this->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void En_Peehat_Actor_mode_forward2_init(EnPeehat* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gPeehatFlyingAnim);
    this->state = PEAHAT_STATE_SEEK_PLAYER;
    this->unk_2D4 = 0;
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_forward2);
}

void En_Peehat_Actor_mode_forward2(EnPeehat* this, PlayState* play) {
    f32 speedXZ = 5.3f;

    if (this->actor.xzDistToPlayer <= 5.3f) {
        speedXZ = this->actor.xzDistToPlayer + 0.0005f;
    }
    if (this->actor.parent != NULL && this->actor.parent->update == NULL) {
        this->actor.parent = NULL;
    }
    this->actor.speed = speedXZ;
    if (this->actor.world.pos.y - this->actor.floorHeight >= 70.0f) {
        add_calc(&this->actor.velocity.y, -1.3f, 1.0f, 0.5f, 0.0f);
    } else {
        add_calc(&this->actor.velocity.y, -0.135f, 1.0f, 0.05f, 0.0f);
    }
    if (this->unk_2D4 == 0) {
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 830, 0);
    } else {
        this->unk_2D4--;
    }
    this->actor.shape.rot.y += 0x15E;
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc_short_angle2(&this->bladeRotVel, 4000, 1, 500, 0);
    this->bladeRot += this->bladeRotVel;
    add_calc(&this->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_SM_FLY - SFX_FLAG);
    if (this->colQuad.base.atFlags & AT_BOUNCED) {
        this->actor.colChkInfo.health = 0;
        this->colQuad.base.acFlags &= ~AC_BOUNCED;
        En_Peehat_Actor_mode_shock_init(this);
    } else if ((this->colQuad.base.atFlags & AT_HIT) || (this->colCylinder.base.acFlags & AC_HIT) ||
               (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        Player* player = GET_PLAYER(play);
        this->colQuad.base.atFlags &= ~AT_HIT;
        if (!(this->colCylinder.base.acFlags & AC_HIT) && &player->actor == this->colQuad.base.at) {
            if (fqrand() > 0.5f) {
                this->actor.world.rot.y += 0x2000;
            } else {
                this->actor.world.rot.y -= 0x2000;
            }
            this->unk_2D4 = 40;
        } else if (this->colCylinder.base.acFlags & AC_HIT || this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            Vec3f zeroVec = { 0, 0, 0 };
            s32 i;
            for (i = 4; i >= 0; i--) {
                Vec3f pos;
                pos.x = rnd_fx(20.0f) + this->actor.world.pos.x;
                pos.y = rnd_fx(10.0f) + this->actor.world.pos.y;
                pos.z = rnd_fx(20.0f) + this->actor.world.pos.z;
                _Effect_SS_Db_ct(play, &pos, &zeroVec, &zeroVec, 40, 7, 255, 255, 255, 255, 255, 0, 0, 1, 9, 1);
            }
        }
        if (&player->actor != this->colQuad.base.at || this->colCylinder.base.acFlags & AC_HIT) {
            if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
                Effect_sound_ct(play, &this->actor.projectedPos, NA_SE_EN_PIHAT_SM_DEAD, 1, 1, 40);
            }
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x20);
            Actor_delete(&this->actor);
        }
    }
}

void En_Peehat_Actor_mode_down_init(EnPeehat* this) {
    this->state = PEAHAT_STATE_LANDING;
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gPeehatLandingAnim);
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_down);
}

void En_Peehat_Actor_mode_down(EnPeehat* this, PlayState* play) {
    add_calc(&this->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
    add_calc(&this->actor.speed, 0.0f, 1.0f, 1.0f, 0.0f);
    add_calc_short_angle2(&this->actor.shape.rot.x, 0, 1, 50, 0);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        En_Peehat_Actor_mode_wait_init(this);
        this->actor.world.pos.y = this->actor.floorHeight;
        Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_LAND);
    } else if (this->actor.floorHeight < this->actor.world.pos.y) {
        add_calc(&this->actor.world.pos.y, this->actor.floorHeight, 0.3f, 3.5f, 0.25f);
        if (this->actor.world.pos.y - this->actor.floorHeight < 60.0f) {
            Vec3f pos = this->actor.world.pos;
            pos.y = this->actor.floorHeight;
            dust_fly_set2(play, &pos, 80.0f, 1, 150, 100, 1);
            dust_ground_setx(play, this, &pos, 75.0f, 2, 1.05f, 2.0f);
        }
    }
    add_calc_short_angle2(&this->bladeRotVel, 0, 1, 100, 0);
    this->bladeRot += this->bladeRotVel;
}

void En_Peehat_Actor_mode_down2_init(EnPeehat* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gPeehatLandingAnim);
    this->state = PEAHAT_STATE_LANDING;
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_down2);
}

void En_Peehat_Actor_mode_down2(EnPeehat* this, PlayState* play) {
    add_calc(&this->actor.shape.yOffset, -1000.0f, 1.0f, 50.0f, 0.0f);
    add_calc(&this->actor.speed, 0.0f, 1.0f, 1.0f, 0.0f);
    add_calc_short_angle2(&this->actor.shape.rot.x, 0, 1, 50, 0);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        En_Peehat_Actor_mode_wait2_init(this);
        Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_LAND);
        this->actor.world.pos.y = this->actor.floorHeight;
    } else if (this->actor.floorHeight < this->actor.world.pos.y) {
        add_calc(&this->actor.world.pos.y, this->actor.floorHeight, 0.3f, 13.5f, 0.25f);
        if (this->actor.world.pos.y - this->actor.floorHeight < 60.0f) {
            Vec3f pos = this->actor.world.pos;
            pos.y = this->actor.floorHeight;
            dust_fly_set2(play, &pos, 80.0f, 1, 150, 100, 1);
            dust_ground_setx(play, this, &pos, 75.0f, 2, 1.05f, 2.0f);
        }
    }
    add_calc_short_angle2(&this->bladeRotVel, 0, 1, 100, 0);
    this->bladeRot += this->bladeRotVel;
}

void En_Peehat_Actor_mode_rnd_move_init(EnPeehat* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gPeehatFlyingAnim);
    this->actor.speed = fqrand() * 0.5f + 2.5f;
    this->unk_2D4 = fqrand() * 10 + 10;
    this->state = PEAHAT_STATE_15;
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_rnd_move);
}

void En_Peehat_Actor_mode_rnd_move(EnPeehat* this, PlayState* play) {
    f32 cos;
    Player* player = GET_PLAYER(play);

    // hover but don't gain altitude
    if (this->actor.world.pos.y - this->actor.floorHeight > 75.0f) {
        this->actor.world.pos.y -= 1.0f;
    }
    this->actor.world.pos.y += cosf_table(this->unk_2E0) * 1.4f;
    cos = cosf_table(this->unk_2E0) * 0.18f;
    this->unk_2E0 += ((0.0f <= cos) ? cos : -cos) + 0.07f;
    this->unk_2D4--;
    if (this->unk_2D4 <= 0) {
        this->actor.speed = fqrand() * 0.5f + 2.5f;
        this->unk_2D4 = fqrand() * 10.0f + 10.0f;
        this->unk_2F4 = (fqrand() - 0.5f) * 1000.0f;
    }
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actor.world.rot.y += this->unk_2F4;
    if (this->seekPlayerTimer <= 0) {
        En_Peehat_Actor_mode_down_init(this);
        this->riseDelayTimer = 40;
    } else {
        this->seekPlayerTimer--;
    }
    this->actor.shape.rot.y += 0x15E;
    // if daytime, and the player is close to the initial spawn position
    if (IS_DAY && search_position_distanceXZ(&this->actor.home.pos, &player->actor.world.pos) < this->xzDistMax) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        En_Peehat_Actor_mode_forward_init(this);
        this->unk_2FA = play->gameplayFrames & 1;
    } else {
        En_Peehat_Actor_mode_home_init(this);
    }
    add_calc_short_angle2(&this->bladeRotVel, 4000, 1, 500, 0);
    this->bladeRot += this->bladeRotVel;
    add_calc(&this->scaleShift, 0.075f, 1.0f, 0.005f, 0.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void En_Peehat_Actor_mode_home_init(EnPeehat* this) {
    this->state = PEAHAT_STATE_RETURN_HOME;
    this->actor.speed = 2.5f;
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_home);
}

void En_Peehat_Actor_mode_home(EnPeehat* this, PlayState* play) {
    f32 cos;
    s16 yRot;
    Player* player;

    player = GET_PLAYER(play);
    if (this->actor.world.pos.y - this->actor.floorHeight > 75.0f) {
        this->actor.world.pos.y -= 1.0f;
    } else {
        this->actor.world.pos.y += 1.0f;
    }
    this->actor.world.pos.y += cosf_table(this->unk_2E0) * 1.4f;
    cos = cosf_table(this->unk_2E0) * 0.18f;
    this->unk_2E0 += ((0.0f <= cos) ? cos : -cos) + 0.07f;
    yRot = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);
    add_calc_short_angle2(&this->actor.world.rot.y, yRot, 1, 600, 0);
    add_calc_short_angle2(&this->actor.shape.rot.x, 4500, 1, 600, 0);
    this->actor.shape.rot.y += 0x15E;
    this->bladeRot += this->bladeRotVel;
    if (search_position_distanceXZ(&this->actor.world.pos, &this->actor.home.pos) < 2.0f) {
        En_Peehat_Actor_mode_down_init(this);
        this->riseDelayTimer = 60;
    }
    if (IS_DAY && search_position_distanceXZ(&this->actor.home.pos, &player->actor.world.pos) < this->xzDistMax) {
        this->seekPlayerTimer = 400;
        En_Peehat_Actor_mode_forward_init(this);
        this->unk_2FA = (play->gameplayFrames & 1);
    }
    Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void En_Peehat_Actor_mode_shock_init(EnPeehat* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPeehatRecoilAnim, -4.0f);
    this->state = PEAHAT_STATE_ATTACK_RECOIL;
    this->actor.speed = -9.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_shock);
}

void En_Peehat_Actor_mode_shock(EnPeehat* this, PlayState* play) {
    this->bladeRot += this->bladeRotVel;
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actor.speed += 0.5f;
    if (this->actor.speed == 0.0f) {
        // Is PEAHAT_TYPE_LARVA
        if (this->actor.params > 0) {
            Vec3f zeroVec = { 0, 0, 0 };
            s32 i;
            for (i = 4; i >= 0; i--) {
                Vec3f pos;
                pos.x = rnd_fx(20.0f) + this->actor.world.pos.x;
                pos.y = rnd_fx(10.0f) + this->actor.world.pos.y;
                pos.z = rnd_fx(20.0f) + this->actor.world.pos.z;
                _Effect_SS_Db_ct(play, &pos, &zeroVec, &zeroVec, 40, 7, 255, 255, 255, 255, 255, 0, 0, 1, 9, 1);
            }
            Actor_delete(&this->actor);
        } else {
            En_Peehat_Actor_mode_forward_init(this);
            // Is PEAHAT_TYPE_GROUNDED
            if (this->actor.params < 0) {
                this->unk_2FA = (this->unk_2FA != 0) ? 0 : 1;
            }
        }
    }
    Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_FLY - SFX_FLAG);
}

void En_Peehat_Actor_mode_paralyze_init(EnPeehat* this) {
    this->state = PEAHAT_STATE_STUNNED;
    if (this->actor.floorHeight < this->actor.world.pos.y) {
        this->actor.speed = -9.0f;
    }
    this->bladeRotVel = 0;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 200, COLORFILTER_BUFFLAG_OPA, 80);
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_paralyze);
}

void En_Peehat_Actor_mode_paralyze(EnPeehat* this, PlayState* play) {
    add_calc(&this->actor.speed, 0.0f, 1.0f, 1.0f, 0.0f);
    add_calc(&this->actor.world.pos.y, this->actor.floorHeight, 1.0f, 8.0f, 0.0f);
    if (this->actor.colorFilterTimer == 0) {
        En_Peehat_Actor_mode_wake_init(this);
    }
}

void En_Peehat_Actor_mode_damage_init(EnPeehat* this) {
    this->bladeRotVel = 0;
    this->isStateDieFirstUpdate = 1;
    this->actor.speed = 0.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
    this->state = PEAHAT_STATE_DYING;
    this->scaleShift = 0.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_damage);
}

void En_Peehat_Actor_mode_damage(EnPeehat* this, PlayState* play) {
    if (this->isStateDieFirstUpdate) {
        this->unk_2D4--;
        if (this->unk_2D4 <= 0 || this->actor.colChkInfo.health == 0) {
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPeehatRecoilAnim, -4.0f);
            this->bladeRotVel = 4000;
            this->unk_2D4 = 14;
            this->actor.speed = 0;
            this->actor.velocity.y = 6;
            this->isStateDieFirstUpdate = 0;
            this->actor.shape.rot.z = this->actor.shape.rot.x = 0;
        } else if (this->actor.colorFilterTimer & 4) {
            add_calc(&this->scaleShift, 0.205f, 1.0f, 0.235f, 0);
        } else {
            add_calc(&this->scaleShift, 0, 1.0f, 0.005f, 0);
        }
    } else {
        Skeleton_Info2_anime_play(&this->skelAnime);
        this->bladeRot += this->bladeRotVel;
        add_calc_short_angle2(&this->bladeRotVel, 4000, 1, 250, 0);
        if (this->actor.colChkInfo.health == 0) {
            this->actor.scale.x -= 0.0015f;
            Actor_set_scale(&this->actor, this->actor.scale.x);
        }
        if (add_calc(&this->actor.world.pos.y, this->actor.floorHeight + 88.5f, 1.0f, 3.0f, 0.0f) == 0.0f &&
            this->actor.world.pos.y - this->actor.floorHeight < 59.0f) {
            Vec3f pos = this->actor.world.pos;
            pos.y = this->actor.floorHeight;
            dust_fly_set2(play, &pos, 80.0f, 1, 150, 100, 1);
            dust_ground_setx(play, this, &pos, 75.0f, 2, 1.05f, 2.0f);
        }
        if (this->actor.speed < 0) {
            this->actor.speed += 0.25f;
        }
        this->unk_2D4--;
        if (this->unk_2D4 <= 0) {
            if (this->actor.colChkInfo.health == 0) {
                En_Peehat_Actor_mode_dead_init(this);
                // if PEAHAT_TYPE_GROUNDED
            } else if (this->actor.params < 0) {
                En_Peehat_Actor_mode_rnd_move_init(this);
                this->riseDelayTimer = 60;
            } else {
                En_Peehat_Actor_mode_wait3_init(this);
            }
        }
    }
}

void En_Peehat_Actor_mode_dead_init(EnPeehat* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gPeehatFlyingAnim);
    this->state = PEAHAT_STATE_EXPLODE;
    this->animTimer = 5;
    this->unk_2E0 = 0.0f;
    En_Peehat_actor_set_process(this, En_Peehat_Actor_mode_dead);
}

void En_Peehat_Actor_mode_dead(EnPeehat* this, PlayState* play) {
    s32 pad[2];

    if (this->animTimer == 5) {
        EnBom* bomb = (EnBom*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                          this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0x602, 0);
        if (bomb != NULL) {
            bomb->timer = 0;
        }
    }
    this->animTimer--;
    if (this->animTimer == 0) {
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x40);
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x40);
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x40);
        Actor_delete(&this->actor);
    }
}

void En_Peehat_damage_proc(EnPeehat* this, PlayState* play) {
    if ((this->colCylinder.base.acFlags & AC_BOUNCED) || (this->colQuad.base.acFlags & AC_BOUNCED)) {
        this->colQuad.base.acFlags &= ~AC_BOUNCED;
        this->colCylinder.base.acFlags &= ~AC_BOUNCED;
        this->colJntSph.base.acFlags &= ~AC_HIT;
    } else if (this->colJntSph.base.acFlags & AC_HIT) {
        this->colJntSph.base.acFlags &= ~AC_HIT;
        Hit_bit_set_sph(&this->actor, &this->colJntSph, true);
        if (this->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_NUT ||
            this->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_LIGHT_ICE_ARROW) {
            return;
        }
        if (this->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_HOOKSHOT) {
            this->actor.colChkInfo.health = 0;
        } else if (this->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_BOOMERANG) {
            if (this->state != PEAHAT_STATE_STUNNED) {
                En_Peehat_Actor_mode_paralyze_init(this);
            }
            return;
        } else {
            hp_down(&this->actor);
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
            Actor_SE_set(&this->actor, NA_SE_EN_PIHAT_DAMAGE);
        }

        if (this->actor.colChkInfo.damageEffect == PEAHAT_DMG_EFF_FIRE) {
            Vec3f pos;
            s32 i;
            for (i = 4; i >= 0; i--) {
                pos.x = rnd_fx(20.0f) + this->actor.world.pos.x;
                pos.y = fqrand() * 25.0f + this->actor.world.pos.y;
                pos.z = rnd_fx(20.0f) + this->actor.world.pos.z;
                Effect_En_Fire_ct(play, &this->actor, &pos, 70, 0, 0, -1);
            }
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA, 100);
        }
        if (this->actor.colChkInfo.health == 0) {
            En_Peehat_Actor_mode_damage_init(this);
        }
    }
}

void En_Peehat_move(Actor* thisx, PlayState* play) {
    EnPeehat* this = (EnPeehat*)thisx;
    s32 i;
    Player* player = GET_PLAYER(play);
    Vec3f posResult;
    CollisionPoly* poly;
    s32 bgId;
    Vec3f* posB;

    // If Adult Peahat
    if (thisx->params <= 0) {
        En_Peehat_damage_proc(this, play);
    }
    if (thisx->colChkInfo.damageEffect != PEAHAT_DMG_EFF_LIGHT_ICE_ARROW) {
        if (thisx->speed != 0.0f || thisx->velocity.y != 0.0f) {
            Actor_position_moveF(thisx);
            Actor_BGcheck2(play, thisx, 25.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        }

        this->actionFunc(this, play);
        if ((play->gameplayFrames & 0x7F) == 0) {
            this->jiggleRotInc = (fqrand() * 0.25f) + 0.5f;
        }
        this->jiggleRot += this->jiggleRotInc;
    }
    // if PEAHAT_TYPE_GROUNDED
    if (thisx->params < 0) {
        // Set the Z-Target point on the Peahat's weak point
        thisx->focus.pos.x = this->colJntSph.elements[0].dim.worldSphere.center.x;
        thisx->focus.pos.y = this->colJntSph.elements[0].dim.worldSphere.center.y;
        thisx->focus.pos.z = this->colJntSph.elements[0].dim.worldSphere.center.z;
        if (this->state == PEAHAT_STATE_SEEK_PLAYER) {
            add_calc_short_angle2(&thisx->shape.rot.x, 6000, 1, 300, 0);
        } else {
            add_calc_short_angle2(&thisx->shape.rot.x, 0, 1, 300, 0);
        }
    } else {
        thisx->focus.pos = thisx->world.pos;
    }
    CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->colCylinder);
    if (thisx->colChkInfo.health > 0) {
        // If Adult Peahat
        if (thisx->params <= 0) {
            CollisionCheck_setOC(play, &play->colChkCtx, &this->colCylinder.base);
            CollisionCheck_setOC(play, &play->colChkCtx, &this->colJntSph.base);
            if (thisx->colorFilterTimer == 0 || !(thisx->colorFilterParams & 0x4000)) {
                if (this->state != PEAHAT_STATE_EXPLODE) {
                    CollisionCheck_setAC(play, &play->colChkCtx, &this->colJntSph.base);
                }
            }
        }
        if (thisx->params != PEAHAT_TYPE_FLYING && this->colQuad.base.atFlags & AT_HIT) {
            this->colQuad.base.atFlags &= ~AT_HIT;
            if (&player->actor == this->colQuad.base.at) {
                En_Peehat_Actor_mode_shock_init(this);
            }
        }
    }
    if (this->state == PEAHAT_STATE_15 || this->state == PEAHAT_STATE_SEEK_PLAYER || this->state == PEAHAT_STATE_FLY ||
        this->state == PEAHAT_STATE_RETURN_HOME || this->state == PEAHAT_STATE_EXPLODE) {
        if (thisx->params != PEAHAT_TYPE_FLYING) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->colQuad.base);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->colQuad.base);
        }
        // if PEAHAT_TYPE_GROUNDED
        if (thisx->params < 0 && (thisx->flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME)) {
            for (i = 1; i >= 0; i--) {
                poly = NULL;
                posB = &this->bladeTip[i];

                if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &thisx->world.pos, posB, &posResult, &poly, true, true,
                                            false, true, &bgId) == true) {
                    dust_fly_set2(play, &posResult, 0.0f, 1, 300, 150, 1);
                    dust_ground_setx(play, this, &posResult, 0.0f, 3, 1.05f, 1.5f);
                }
            }
        } else if (thisx->params != PEAHAT_TYPE_FLYING) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->colCylinder.base);
        }
    } else {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colCylinder.base);
    }
    add_calc(&this->scaleShift, 0.0f, 1.0f, 0.001f, 0.0f);
}

s32 en_peehat_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnPeehat* this = (EnPeehat*)thisx;

    if (limbIndex == 4) {
        rot->x = -this->bladeRot;
    }
    if (limbIndex == 3 || (limbIndex == 23 && (this->state == PEAHAT_STATE_DYING || this->state == PEAHAT_STATE_3 ||
                                               this->state == PEAHAT_STATE_4))) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_peehat.c", 1946);
        Matrix_push();
        Matrix_scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_rotateX(this->jiggleRot * 0.115f, MTXMODE_APPLY);
        Matrix_rotateY(this->jiggleRot * 0.13f, MTXMODE_APPLY);
        Matrix_rotateZ(this->jiggleRot * 0.1f, MTXMODE_APPLY);
        Matrix_scale(1.0f - this->scaleShift, this->scaleShift + 1.0f, 1.0f - this->scaleShift, MTXMODE_APPLY);
        Matrix_rotateZ(-(this->jiggleRot * 0.1f), MTXMODE_APPLY);
        Matrix_rotateY(-(this->jiggleRot * 0.13f), MTXMODE_APPLY);
        Matrix_rotateX(-(this->jiggleRot * 0.115f), MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_peehat.c", 1959);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_pull();
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_peehat.c", 1963);
        return true;
    }
    return false;
}

void en_ph_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f local_pos[] = { { 0.0f, 0.0f, 5500.0f }, { 0.0f, 0.0f, -5500.0f } };

    EnPeehat* this = (EnPeehat*)thisx;
    f32 damageYRot;

    if (limbIndex == 4) {
        Matrix_Position(&local_pos[0], &this->bladeTip[0]);
        Matrix_Position(&local_pos[1], &this->bladeTip[1]);
        return;
    }
    // is Adult Peahat
    if (limbIndex == 3 && this->actor.params <= 0) {
        damageYRot = 0.0f;
        OPEN_DISPS(play->state.gfxCtx, "../z_en_peehat.c", 1981);
        Matrix_push();
        Matrix_translate(-1000.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        CollisionCheck_Uty_convJntSphL2G(0, &this->colJntSph);
        Matrix_translate(500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        if (this->actor.colorFilterTimer != 0 && (this->actor.colorFilterParams & 0x4000)) {
            damageYRot = sin_s(this->actor.colorFilterTimer * 0x4E20) * 0.35f;
        }
        Matrix_rotateY(3.2f + damageYRot, MTXMODE_APPLY);
        Matrix_scale(0.3f, 0.2f, 0.2f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_peehat.c", 1990);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_pull();
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_peehat.c", 1994);
    }
}

void En_Peehat_display(Actor* thisx, PlayState* play) {
    static Vec3f sword_top[] = {
        { 0.0f, 0.0f, -4500.0f }, { -4500.0f, 0.0f, 0.0f }, { 4500.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 4500.0f }
    };
    EnPeehat* this = (EnPeehat*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_peehat_display1,
                      en_ph_display2, this);
    if (this->actor.speed != 0.0f || this->actor.velocity.y != 0.0f) {
        Matrix_Position(&sword_top[0], &this->colQuad.dim.quad[1]);
        Matrix_Position(&sword_top[1], &this->colQuad.dim.quad[0]);
        Matrix_Position(&sword_top[2], &this->colQuad.dim.quad[3]);
        Matrix_Position(&sword_top[3], &this->colQuad.dim.quad[2]);
        CollisionCheck_Uty_setSword4Pos(&this->colQuad, &this->colQuad.dim.quad[0], &this->colQuad.dim.quad[1],
                                 &this->colQuad.dim.quad[2], &this->colQuad.dim.quad[3]);
    }
}
