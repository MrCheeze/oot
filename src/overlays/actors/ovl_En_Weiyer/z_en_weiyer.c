/*
 * File: z_en_weiyer.c
 * Overlay: ovl_En_Weiyer
 * Description: Stinger (Water)
 */

#include "z_en_weiyer.h"
#include "assets/objects/object_ei/object_ei.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void En_Weiyer_actor_ct(Actor* thisx, PlayState* play);
void En_Weiyer_actor_dt(Actor* thisx, PlayState* play);
void En_Weiyer_actor_move(Actor* thisx, PlayState* play);
void En_Weiyer_actor_draw(Actor* thisx, PlayState* play);

static void mode_water_wait(EnWeiyer* this, PlayState* play);
static void mode_swim(EnWeiyer* this, PlayState* play);
static void mode_return(EnWeiyer* this, PlayState* play);
void mode_land(EnWeiyer* this, PlayState* play);
void mode_water_attack(EnWeiyer* this, PlayState* play);
static void mode_escape(EnWeiyer* this, PlayState* play);
static void mode_damage(EnWeiyer* this, PlayState* play);
static void mode_stop(EnWeiyer* this, PlayState* play);
static void mode_predead(EnWeiyer* this, PlayState* play);
static void mode_dead(EnWeiyer* this, PlayState* play);
void mode_fly_attack(EnWeiyer* this, PlayState* play);

ActorProfile En_Weiyer_Profile = {
    /**/ ACTOR_EN_WEIYER,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_EI,
    /**/ sizeof(EnWeiyer),
    /**/ En_Weiyer_actor_ct,
    /**/ En_Weiyer_actor_dt,
    /**/ En_Weiyer_actor_move,
    /**/ En_Weiyer_actor_draw,
};

static ColliderCylinderInit WeiyerAllPipeData = {
    {
        COL_MATERIAL_HIT0,
        AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 16, 10, -6, { 0, 0, 0 } },
};

static CollisionCheckInfoInit WeiyerStatusData = { 2, 45, 15, 100 };

static DamageTable WeiyerBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_STINGER, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 3, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2500, ICHAIN_STOP),
};

void En_Weiyer_actor_ct(Actor* thisx, PlayState* play) {
    EnWeiyer* this = (EnWeiyer*)thisx;

    ValueSet_process(thisx, value_init);
    Shape_Info_init(&this->actor.shape, 1000.0f, Actor_shadow_circle, 65.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gStingerSkel, &gStingerIdleAnim, this->jointTable, this->morphTable, 19);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &WeiyerAllPipeData);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, &WeiyerBtlData, &WeiyerStatusData);
    this->actionFunc = mode_water_wait;
}

void En_Weiyer_actor_dt(Actor* thisx, PlayState* play) {
    EnWeiyer* this = (EnWeiyer*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void mode_swim_init(EnWeiyer* this) {
    this->unk_196 = this->actor.shape.rot.y;
    this->unk_27C = (cosf(-M_PI / 8) * 3.0f) + this->actor.world.pos.y;
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gStingerHitAnim, -5.0f);
    this->unk_194 = 30;
    this->actor.speed = CLAMP_MAX(this->actor.speed, 2.5f);
    this->collider.base.atFlags &= ~AT_ON;
    this->unk_280 = this->actor.floorHeight;
    this->actionFunc = mode_swim;
}

static void mode_return_init(EnWeiyer* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gStingerHitAnim, -5.0f);
    this->collider.base.atFlags |= AT_ON;
    this->unk_194 = 0;
    this->actor.speed = 5.0f;
    this->actionFunc = mode_return;
}

void mode_land_init(EnWeiyer* this) {
    Skeleton_Info2_init(&this->skelAnime, &gStingerPopOutAnim, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f);
    this->unk_194 = 40;
    this->collider.base.atFlags |= AT_ON;
    this->actionFunc = mode_land;
}

void mode_water_attack_init(EnWeiyer* this) {
    this->unk_194 = 200;
    this->collider.base.atFlags |= AT_ON;
    this->skelAnime.playSpeed = 3.0f;
    this->actionFunc = mode_water_attack;
}

static void mode_escape_init(EnWeiyer* this) {
    this->unk_194 = 200;
    this->unk_196 = this->actor.yawTowardsPlayer + 0x8000;
    this->unk_27C = this->actor.world.pos.y;
    this->actor.speed = CLAMP_MAX(this->actor.speed, 4.0f);
    this->collider.base.atFlags &= ~AT_ON;
    this->skelAnime.playSpeed = 1.0f;
    this->actionFunc = mode_escape;
}

static void mode_damage_init(EnWeiyer* this) {
    Skeleton_Info2_init(&this->skelAnime, &gStingerHitAnim, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -3.0f);
    this->unk_194 = 40;
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 3.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA, 40);
    this->collider.dim.height = WeiyerAllPipeData.dim.height;
    this->actionFunc = mode_damage;
}

static void mode_stop_init(EnWeiyer* this) {
    Skeleton_Info2_init(&this->skelAnime, &gStingerPopOutAnim, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f);
    this->unk_194 = 80;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -1.0f;
    this->collider.dim.height = WeiyerAllPipeData.dim.height + 15;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 200, COLORFILTER_BUFFLAG_OPA, 80);
    this->collider.base.atFlags &= ~AT_ON;
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->actionFunc = mode_stop;
}

static void mode_predead_init(EnWeiyer* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gStingerHitAnim, -5.0f);
    this->unk_194 = 20;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA, 40);
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.speed = 3.0f;
    this->actionFunc = mode_predead;
}

static void mode_dead_init(EnWeiyer* this) {
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.colorFilterParams |= 0x2000;
    this->actionFunc = mode_dead;
}

void mode_fly_attack_init(EnWeiyer* this) {
    this->actor.shape.rot.x = -0x2000;
    this->unk_194 = -1;
    this->actor.speed = 5.0f;
    this->actionFunc = mode_fly_attack;
}

static void mode_water_wait(EnWeiyer* this, PlayState* play) {
    WaterBox* waterBox;
    s32 bgId;

    this->actor.world.pos.y += 0.5f;
    this->actor.floorHeight =
        T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &this->actor.world.pos);

    if (!T_BGCheck_WaterSurfaceCheck2(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z,
                                 &this->actor.home.pos.y, &waterBox) ||
        ((this->actor.home.pos.y - 5.0f) <= this->actor.floorHeight)) {
        Actor_delete(&this->actor);
    } else {
        this->actor.home.pos.y -= 5.0f;
        this->actor.world.pos.y = (this->actor.home.pos.y + this->actor.floorHeight) / 2.0f;
        mode_swim_init(this);
    }
}

static void mode_swim(EnWeiyer* this, PlayState* play) {
    s32 sp34;
    f32 curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_angle(&this->actor.shape.rot.x, 0, 0x800);
    sp34 = Skeleton_Info_frame_check(&this->skelAnime, 0.0f);
    curFrame = this->skelAnime.curFrame;
    chase_f(&this->unk_27C, this->unk_280, 0.5f);
    this->actor.world.pos.y = this->unk_27C - cosf((curFrame - 5.0f) * (M_PI / 40)) * 3.0f;

    if (curFrame <= 45.0f) {
        chase_f(&this->actor.speed, 1.0f, 0.03f);
    } else {
        chase_f(&this->actor.speed, 1.3f, 0.03f);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->unk_196 = this->actor.wallYaw;
        this->unk_194 = 30;
    }

    if (chase_angle(&this->actor.shape.rot.y, this->unk_196, 182)) {
        if (this->unk_194 != 0) {
            this->unk_194--;
        }

        if (this->unk_194 == 0) {
            this->unk_196 =
                get_random_timer(0x2000, 0x2000) * ((fqrand() < 0.5f) ? -1 : 1) + this->actor.shape.rot.y;
            this->unk_194 = 30;

            if (fqrand() < 0.3333f) {
                Actor_SE_set(&this->actor, NA_SE_EN_EIER_CRY);
            }
        }
    }

    if (this->actor.home.pos.y < this->actor.world.pos.y) {
        if (this->actor.home.pos.y < this->actor.floorHeight) {
            mode_return_init(this);
        } else {
            this->actor.world.pos.y = this->actor.home.pos.y;
            this->unk_280 =
                fqrand() * ((this->actor.home.pos.y - this->actor.floorHeight) / 2.0f) + this->actor.floorHeight;
        }
    } else {
        Player* player = GET_PLAYER(play);

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->unk_280 =
                this->actor.home.pos.y - fqrand() * ((this->actor.home.pos.y - this->actor.floorHeight) / 2.0f);
        } else if (sp34 && (fqrand() < 0.1f)) {
            this->unk_280 =
                fqrand() * (this->actor.home.pos.y - this->actor.floorHeight) + this->actor.floorHeight;
        }

        if ((this->actor.xzDistToPlayer < 400.0f) && (fabsf(this->actor.yDistToPlayer) < 250.0f) &&
            (player->actor.world.pos.y < (this->actor.home.pos.y + 20.0f))) {
            mode_water_attack_init(this);
        }
    }
}

static void mode_return(EnWeiyer* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->unk_194 == 0) {
        if (chase_angle(&this->actor.shape.rot.x, -0x4000, 0x800)) {
            this->actor.shape.rot.z = 0;
            this->actor.shape.rot.y += 0x8000;
            this->unk_194 = 1;
        } else {
            this->actor.shape.rot.z = this->actor.shape.rot.x * 2;
        }
    } else {
        chase_angle(&this->actor.shape.rot.x, 0x1800, 0x800);

        if (this->actor.world.pos.y < this->actor.home.pos.y) {
            if (this->actor.shape.rot.x > 0) {
                Effect_SS_G_Splash_sc_cl_ct(play, &this->actor.world.pos, NULL, NULL, 1, 400);
                Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_SINK);
            }

            mode_escape_init(this);
        } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            mode_land_init(this);
        }
    }
}

void mode_land(EnWeiyer* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_EIER_FLUTTER);
    }

    chase_angle(&this->actor.shape.rot.x, 0, 0x800);
    chase_f(&this->actor.speed, 0.0f, 1.0f);

    if (this->unk_194 != 0) {
        this->unk_194--;
    }

    if (this->unk_194 == 0) {
        mode_return_init(this);
    } else if (this->actor.world.pos.y < this->actor.home.pos.y) {
        mode_swim_init(this);
    }
}

s16 weiyer_aim_angle_x(EnWeiyer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f vec;

    vec.x = player->actor.world.pos.x;
    vec.y = player->actor.world.pos.y + 20.0f;
    vec.z = player->actor.world.pos.z;

    return Actor_search_position_angleX(&this->actor, &vec);
}

void mode_water_attack(EnWeiyer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->unk_194 != 0) {
        this->unk_194--;
    }

    if ((this->unk_194 == 0) || ((this->actor.home.pos.y + 20.0f) <= player->actor.world.pos.y) ||
        (this->collider.base.atFlags & AT_HIT)) {
        mode_escape_init(this);
    } else {
        if (Actor_player_direction_check(&this->actor, 0x2800)) {
            chase_f(&this->actor.speed, 4.0f, 0.2f);
        } else {
            chase_f(&this->actor.speed, 1.3f, 0.2f);
        }

        if (this->actor.home.pos.y < this->actor.world.pos.y) {
            if (this->actor.home.pos.y < this->actor.floorHeight) {
                this->actor.shape.rot.x = 0;
                mode_return_init(this);
                return;
            }

            this->actor.world.pos.y = this->actor.home.pos.y;
            add_calc_short_angle2(&this->actor.shape.rot.x, 0x1000, 2, 0x100, 0x40);
        } else {
            add_calc_short_angle2(&this->actor.shape.rot.x, weiyer_aim_angle_x(this, play), 2, 0x100, 0x40);
        }

        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x200, 0x80);

        if ((player->actor.depthInWater < 50.0f) && (this->actor.depthInWater < 20.0f) &&
            Actor_player_direction_check(&this->actor, 0x2000)) {
            mode_fly_attack_init(this);
        }
    }
}

static void mode_escape(EnWeiyer* this, PlayState* play) {
    f32 curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_angle(&this->actor.shape.rot.x, 0, 0x800);
    curFrame = this->skelAnime.curFrame;
    chase_f(&this->unk_27C, (this->actor.home.pos.y - this->actor.floorHeight) / 4.0f + this->actor.floorHeight,
                 1.0f);
    this->actor.world.pos.y = this->unk_27C - cosf((curFrame - 5.0f) * (M_PI / 40)) * 3.0f;

    if (curFrame <= 45.0f) {
        chase_f(&this->actor.speed, 1.0f, 0.03f);
    } else {
        chase_f(&this->actor.speed, 1.3f, 0.03f);
    }

    if (this->unk_194 != 0) {
        this->unk_194--;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->unk_196 = this->actor.wallYaw;
    }

    if (add_calc_short_angle2(&this->actor.shape.rot.y, this->unk_196, 2, 0x200, 0x80) == 0) {
        this->unk_196 = this->actor.yawTowardsPlayer + 0x8000;
    }

    if (this->actor.home.pos.y < this->actor.world.pos.y) {
        if (this->actor.home.pos.y < this->actor.floorHeight) {
            mode_return_init(this);
        } else {
            this->actor.world.pos.y = this->actor.home.pos.y;
        }
    }

    if (this->unk_194 == 0) {
        mode_swim_init(this);
    }
}

static void mode_damage(EnWeiyer* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->unk_194 != 0) {
        this->unk_194--;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->unk_196 = this->actor.wallYaw;
    } else {
        this->unk_196 = this->actor.yawTowardsPlayer + 0x8000;
    }

    chase_angle(&this->actor.world.rot.y, this->unk_196, 0x38E);
    chase_angle(&this->actor.shape.rot.x, 0, 0x200);
    this->actor.shape.rot.z = sinf(this->unk_194 * (M_PI / 5)) * 5120.0f;

    if (this->unk_194 == 0) {
        this->actor.shape.rot.z = 0;
        this->collider.base.acFlags |= AC_ON;
        mode_swim_init(this);
    }
}

static void mode_predead(EnWeiyer* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_angle(&this->actor.shape.rot.x, -0x4000, 0x400);
    this->actor.shape.rot.z += 0x1000;

    if (this->unk_194 != 0) {
        this->unk_194--;
    }

    if ((this->unk_194 == 0) || (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING)) {
        mode_dead_init(this);
    }
}

static void mode_dead(EnWeiyer* this, PlayState* play) {
    this->actor.shape.shadowAlpha = CLAMP_MIN((s16)(this->actor.shape.shadowAlpha - 5), 0);
    this->actor.world.pos.y -= 2.0f;

    if (this->actor.shape.shadowAlpha == 0) {
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0xE0);
        Actor_delete(&this->actor);
    }
}

static void mode_stop(EnWeiyer* this, PlayState* play) {
    if (this->unk_194 != 0) {
        this->unk_194--;
    }

    chase_angle(&this->actor.shape.rot.x, 0, 0x200);
    chase_angle(&this->actor.shape.rot.z, 0, 0x200);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actor.home.pos.y < this->actor.floorHeight) {
        if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f)) {
            Actor_SE_set(&this->actor, NA_SE_EN_EIER_FLUTTER);
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        }
    }

    if (this->unk_194 == 0) {
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->collider.dim.height = WeiyerAllPipeData.dim.height;
        mode_swim_init(this);
    }
}

void mode_fly_attack(EnWeiyer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 phi_a1;
    s32 phi_a0;

    Skeleton_Info2_anime_play(&this->skelAnime);

    phi_a0 = ((this->actor.home.pos.y + 20.0f) <= player->actor.world.pos.y);

    if (this->unk_194 == -1) {
        if (phi_a0 || (this->collider.base.atFlags & AT_HIT)) {
            mode_escape_init(this);
        } else if (this->actor.depthInWater < 0.0f) {
            this->unk_194 = 10;
            Effect_SS_G_Splash_sc_cl_ct(play, &this->actor.world.pos, NULL, NULL, 1, 400);
            Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_JUMP);
        }
    } else {
        if (phi_a0 || (this->collider.base.atFlags & AT_HIT)) {
            this->unk_194 = 0;
        } else if (this->unk_194 != 0) {
            this->unk_194--;
        }

        if (this->unk_194 == 0) {
            phi_a1 = 0x1800;
        } else {
            phi_a1 = weiyer_aim_angle_x(this, play);
            phi_a1 = CLAMP_MIN(phi_a1, 0);
        }

        if (this->actor.shape.rot.x < phi_a1) {
            chase_angle(&this->actor.shape.rot.x, phi_a1, 0x400);
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            mode_return_init(this);
        } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->actor.shape.rot.x > 0)) {
            Effect_SS_G_Splash_sc_cl_ct(play, &this->actor.world.pos, NULL, NULL, 1, 400);
            Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_SINK);
            mode_escape_init(this);
        } else {
            add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 8, 0x100, 0x80);
        }
    }
}

void En_Weiyer_damage_proc(EnWeiyer* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elem, true);

        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (this->actor.colChkInfo.damageEffect == 1) {
                if (this->actionFunc != mode_stop) {
                    mode_stop_init(this);
                }
            } else if (hp_down(&this->actor) == 0) {
                Actor_info_finish(play, &this->actor);
                Actor_SE_set(&this->actor, NA_SE_EN_EIER_DEAD);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                mode_predead_init(this);
            } else {
                mode_damage_init(this);
            }
        }
    }
}

void En_Weiyer_actor_move(Actor* thisx, PlayState* play) {
    EnWeiyer* this = (EnWeiyer*)thisx;
    s32 pad;

    this->actor.home.pos.y = this->actor.depthInWater + this->actor.world.pos.y - 5.0f;
    En_Weiyer_damage_proc(this, play);
    this->actionFunc(this, play);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.world.rot.x = -this->actor.shape.rot.x;

    if ((this->actor.world.rot.x == 0) || (this->actionFunc == mode_stop)) {
        Actor_position_moveF(&this->actor);
    } else {
        Actor_position_moveF_XY(&this->actor);
    }

    Actor_BGcheck2(play, &this->actor, 10.0f, 30.0f, 45.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    Actor_world_to_eye(&this->actor, 0.0f);

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~(AT_ON | AT_HIT);
        Actor_SE_set(&this->actor, NA_SE_EN_EIER_ATTACK);
    }

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

    if (this->collider.base.atFlags & AT_ON) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->collider.base.acFlags & AT_ON) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }

    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

s32 en_weiyer_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                              Gfx** gfx) {
    if (limbIndex == 1) {
        pos->z += 2000.0f;
    }

    return 0;
}

void En_Weiyer_actor_draw(Actor* thisx, PlayState* play) {
    EnWeiyer* this = (EnWeiyer*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_weiyer.c", 1193);

    if (this->actionFunc != mode_dead) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, &Actor_change_render_mode[2]);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        POLY_OPA_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       en_weiyer_display1, NULL, &this->actor, POLY_OPA_DISP);
    } else {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, &Actor_change_render_mode[0]);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->actor.shape.shadowAlpha);
        POLY_XLU_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       en_weiyer_display1, NULL, &this->actor, POLY_XLU_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_weiyer.c", 1240);
}
