#include "z_en_eiyer.h"
#include "assets/objects/object_ei/object_ei.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void En_Eiyer_actor_ct(Actor* thisx, PlayState* play);
void En_Eiyer_actor_dt(Actor* thisx, PlayState* play);
void En_Eiyer_actor_move(Actor* thisx, PlayState* play);
void En_Eiyer_actor_draw(Actor* thisx, PlayState* play);

void mode_ground_init(EnEiyer* this);
static void mode_move_init(EnEiyer* this);
void mode_dummy_down_init(EnEiyer* this);
static void mode_rotate_init(EnEiyer* this, PlayState* play);
static void mode_swim_init(EnEiyer* this);
static void mode_ready_init(EnEiyer* this);
static void mode_attack_init(EnEiyer* this, PlayState* play);
void mode_ground_down_init(EnEiyer* this);
static void mode_damage_init(EnEiyer* this);
static void mode_predead_init(EnEiyer* this);
static void mode_dead_init(EnEiyer* this);
static void mode_stop_init(EnEiyer* this);

void mode_ground(EnEiyer* this, PlayState* play);
void mode_single_move(EnEiyer* this, PlayState* play);
static void mode_move(EnEiyer* this, PlayState* play);
void mode_dummy_down(EnEiyer* this, PlayState* play);
static void mode_rotate(EnEiyer* this, PlayState* play);
static void mode_swim(EnEiyer* this, PlayState* play);
static void mode_ready(EnEiyer* this, PlayState* play);
static void mode_attack(EnEiyer* this, PlayState* play);
void mode_ground_down(EnEiyer* this, PlayState* play);
static void mode_damage(EnEiyer* this, PlayState* play);
static void mode_predead(EnEiyer* this, PlayState* play);
static void mode_dead(EnEiyer* this, PlayState* play);
static void mode_stop(EnEiyer* this, PlayState* play);

ActorProfile En_Eiyer_Profile = {
    /**/ ACTOR_EN_EIYER,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_EI,
    /**/ sizeof(EnEiyer),
    /**/ En_Eiyer_actor_ct,
    /**/ En_Eiyer_actor_dt,
    /**/ En_Eiyer_actor_move,
    /**/ En_Eiyer_actor_draw,
};

static ColliderCylinderInit EiyerAllPipeData = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x04, 0x08 },
        { 0x00000019, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 27, 17, -10, { 0, 0, 0 } },
};

static CollisionCheckInfoInit EiyerStatusData = { 2, 45, 15, 100 };

static DamageTable EiyerBtlData = {
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
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(4, 0x0),
    /* Light magic   */ DMG_ENTRY(4, 0x0),
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
    ICHAIN_VEC3F_DIV1000(scale, 5, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2500, ICHAIN_STOP),
};

/**
 * params    0: Spawn 3 clones and circle around spawn point
 * params  1-3: Clone, spawn another clone for the main Eiyer if params < 3
 * params   10: Normal Eiyer, wander around spawn point
 */
void En_Eiyer_actor_ct(Actor* thisx, PlayState* play) {
    EnEiyer* this = (EnEiyer*)thisx;
    s32 pad;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 600.0f, Actor_shadow_circle, 65.0f);
    Skeleton_Info2_M_ct(play, &this->skelanime, &gStingerSkel, &gStingerIdleAnim, this->jointTable, this->morphTable, 19);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EiyerAllPipeData);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, &EiyerBtlData, &EiyerStatusData);

    if (this->actor.params < 3) {
        // Each clone spawns another clone
        if (Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_EIYER, this->actor.home.pos.x,
                               this->actor.home.pos.y, this->actor.home.pos.z, 0, this->actor.shape.rot.y + 0x4000, 0,
                               this->actor.params + 1) == NULL) {
            Actor_delete(&this->actor);
            return;
        }

        if (this->actor.params == 0) {
            Actor* child = this->actor.child;
            s32 clonesSpawned;

            for (clonesSpawned = 0; clonesSpawned != 3; clonesSpawned++) {
                if (child == NULL) {
                    break;
                }
                child = child->child;
            }

            if (clonesSpawned != 3) {
                for (child = &this->actor; child != NULL; child = child->child) {
                    Actor_delete(child);
                }
                return;
            } else {
                this->actor.child->parent = &this->actor;
                this->actor.child->child->parent = &this->actor;
                this->actor.child->child->child->parent = &this->actor;
            }
        }
    }

    if (this->actor.params == 0 || this->actor.params == 10) {
        mode_ground_init(this);
    } else {
        mode_dummy_down_init(this);
    }
}

void En_Eiyer_actor_dt(Actor* thisx, PlayState* play) {
    EnEiyer* this = (EnEiyer*)thisx;
    ClObjPipe_dt(play, &this->collider);
}

void set_eiyer_position(EnEiyer* this) {
    this->actor.world.pos.x = sin_s(this->actor.world.rot.y) * 80.0f + this->actor.home.pos.x;
    this->actor.world.pos.z = cos_s(this->actor.world.rot.y) * 80.0f + this->actor.home.pos.z;
    this->actor.shape.rot.y = this->actor.world.rot.y + 0x4000;
}

void mode_ground_init(EnEiyer* this) {
    this->collider.elem.acDmgInfo.dmgFlags = DMG_BOOMERANG | DMG_EXPLOSIVE | DMG_DEKU_NUT;
    Skeleton_Info2_init_standard_repeat(&this->skelanime, &gStingerIdleAnim);

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.y = this->actor.home.pos.y - 40.0f;
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 0.0f;

    if (this->actor.params != 0xA) {
        if (this->actor.params == 0) {
            this->actor.world.rot.y = fqrand() * 0x10000;
        } else {
            this->actor.world.rot.y = this->actor.parent->world.rot.y + this->actor.params * 0x4000;
        }
        set_eiyer_position(this);
    } else {
        this->actor.world.rot.y = this->actor.shape.rot.y = fqrand() * 0x10000;
    }

    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_IGNORE_QUAKE);
    this->actor.shape.shadowScale = 0.0f;
    this->actor.shape.yOffset = 0.0f;
    this->actionFunc = mode_ground;
}

static void mode_move_init(EnEiyer* this) {
    if (this->actor.params == 0xA) {
        this->actor.speed = -0.5f;
        this->actionFunc = mode_single_move;
    } else {
        this->actionFunc = mode_move;
    }

    this->collider.base.acFlags |= AC_ON;
    this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
}

void mode_dummy_down_init(EnEiyer* this) {
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = mode_dummy_down;
}

static void mode_rotate_init(EnEiyer* this, PlayState* play) {
    this->actor.speed = 0.0f;
    Skeleton_Info2_init_standard_stop(&this->skelanime, &gStingerBackflipAnim);
    this->collider.elem.acDmgInfo.dmgFlags = DMG_DEFAULT;
    this->basePos = this->actor.world.pos;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.flags |= ACTOR_FLAG_IGNORE_QUAKE;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.shape.shadowScale = 65.0f;
    this->actor.shape.yOffset = 600.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_OCTAROCK_JUMP);
    Effect_SS_G_Splash_sc_cl_ct(play, &this->actor.world.pos, NULL, NULL, 1, 700);
    this->actionFunc = mode_rotate;
}

static void mode_swim_init(EnEiyer* this) {
    this->targetYaw = this->actor.shape.rot.y;
    this->basePos.y = (cosf(-M_PI / 8) * 5.0f) + this->actor.world.pos.y;
    Skeleton_Info2_init_standard_repeat_morf(&this->skelanime, &gStingerHitAnim, -5.0f);
    this->timer = 60;
    this->actionFunc = mode_swim;
}

static void mode_ready_init(EnEiyer* this) {
    this->actionFunc = mode_ready;
}

static void mode_attack_init(EnEiyer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.velocity.y = 0.0f;
    this->basePos.y = player->actor.world.pos.y + 15.0f;
    this->collider.base.atFlags |= AT_ON;
    this->collider.base.atFlags &= ~AT_HIT;
    this->actionFunc = mode_attack;
}

void mode_ground_down_init(EnEiyer* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelanime, &gStingerDiveAnim, -3.0f);
    this->collider.base.atFlags &= ~AT_ON;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;

    // Update BgCheck info, play sound, and spawn effect on the first frame of the land action
    this->timer = -1;
    this->actor.gravity = 0.0f;
    this->collider.dim.height = EiyerAllPipeData.dim.height;
    this->actionFunc = mode_ground_down;
}

static void mode_damage_init(EnEiyer* this) {
    this->basePos.y = this->actor.world.pos.y;
    Skeleton_Info2_init(&this->skelanime, &gStingerHitAnim, 2.0f, 0.0f, 0.0f, 0, -3.0f);
    this->timer = 40;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 5.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA, 40);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_damage;
}

static void mode_predead_init(EnEiyer* this) {
    this->timer = 20;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA, 40);

    if (this->collider.elem.acDmgInfo.dmgFlags != (DMG_BOOMERANG | DMG_EXPLOSIVE | DMG_DEKU_NUT)) {
        this->actor.speed = 6.0f;
        Skeleton_Info2_init_standard_repeat_morf(&this->skelanime, &gStingerHitAnim, -3.0f);
    } else {
        this->actor.speed -= 6.0f;
    }

    this->collider.elem.acDmgInfo.dmgFlags = DMG_DEFAULT;
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_predead;
}

static void mode_dead_init(EnEiyer* this) {
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.colorFilterParams |= 0x2000;
    this->actionFunc = mode_dead;
}

static void mode_stop_init(EnEiyer* this) {
    Skeleton_Info2_init(&this->skelanime, &gStingerPopOutAnim, 2.0f, 0.0f, 0.0f, 0, -8.0f);
    this->timer = 80;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -1.0f;
    this->collider.dim.height = EiyerAllPipeData.dim.height + 8;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 200, COLORFILTER_BUFFLAG_OPA, 80);
    this->collider.base.atFlags &= ~AT_ON;
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->actionFunc = mode_stop;
}

void mode_ground(EnEiyer* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (chase_f(&this->actor.world.pos.y, this->actor.home.pos.y, 0.75f)) {
        mode_move_init(this);
    }
}

void check_player_hit(EnEiyer* this, PlayState* play) {
    if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
        this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        mode_rotate_init(this, play);
    }
}

static void mode_move(EnEiyer* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);
    this->actor.world.rot.y += -0x60;
    set_eiyer_position(this);
    check_player_hit(this, play);

    // Clones disappear when the main Eiyer leaves the ground
    if (this->actor.params != 0 && ((EnEiyer*)this->actor.parent)->actionFunc != mode_move) {
        mode_dummy_down_init(this);
    }
}

void mode_single_move(EnEiyer* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (Actor_search_position_distanceXZ(&this->actor, &this->actor.home.pos) > 100.0f) {
        this->targetYaw = Actor_search_position_angleY(&this->actor, &this->actor.home.pos) + 0x8000;
    } else if (this->targetYaw == this->actor.world.rot.y && fqrand() > 0.99f) {
        this->targetYaw =
            this->actor.world.rot.y + (fqrand() < 0.5f ? -1 : 1) * (fqrand() * 0x2000 + 0x2000);
    }

    chase_angle(&this->actor.world.rot.y, this->targetYaw, 0xB6);
    check_player_hit(this, play);
}

void mode_dummy_down(EnEiyer* this, PlayState* play) {
    EnEiyer* parent;

    if (this->actor.home.pos.y - 50.0f < this->actor.world.pos.y) {
        this->actor.world.pos.y -= 0.5f;
    }

    parent = (EnEiyer*)this->actor.parent;
    if (parent->actionFunc == mode_dead) {
        Actor_delete(&this->actor);
    } else if (parent->actionFunc == mode_ground) {
        mode_ground_init(this);
    }
}

static void mode_rotate(EnEiyer* this, PlayState* play) {
    s32 animFinished;
    f32 curFrame;
    f32 xzOffset;
    s32 bgId;

    animFinished = Skeleton_Info2_anime_play(&this->skelanime);
    curFrame = this->skelanime.curFrame;

    if (this->skelanime.curFrame < 12.0f) {
        this->actor.world.pos.y = ((1.0f - cosf((0.996f * M_PI / 12.0f) * curFrame)) * 40.0f) + this->actor.home.pos.y;
        xzOffset = sinf((0.996f * M_PI / 12.0f) * curFrame) * -40.0f;
        this->actor.world.pos.x = (sin_s(this->actor.shape.rot.y) * xzOffset) + this->basePos.x;
        this->actor.world.pos.z = (cos_s(this->actor.shape.rot.y) * xzOffset) + this->basePos.z;
    } else {
        chase_f(&this->actor.world.pos.y, this->actor.home.pos.y + 80.0f, 0.5f);
        this->actor.speed = 0.8f;
    }

    if (animFinished) {
        this->collider.base.acFlags |= AC_ON;
        mode_swim_init(this);
    } else {
        this->actor.floorHeight = T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor,
                                                             &this->actor.world.pos);
    }
}

static void mode_swim(EnEiyer* this, PlayState* play) {
    f32 curFrame;
    s32 pad;
    s16 yawChange;

    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->timer != 0) {
        this->timer--;
    }

    curFrame = this->skelanime.curFrame;
    add_calc2(&this->basePos.y, this->actor.floorHeight + 80.0f + 5.0f, 0.3f, this->actor.speed);
    this->actor.world.pos.y = this->basePos.y - cosf((curFrame - 5.0f) * (M_PI / 40)) * 5.0f;

    if (curFrame <= 45.0f) {
        chase_f(&this->actor.speed, 1.0f, 0.03f);
    } else {
        chase_f(&this->actor.speed, 1.5f, 0.03f);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->targetYaw = this->actor.wallYaw;
    }

    if (chase_angle(&this->actor.world.rot.y, this->targetYaw, 0xB6)) {
        if (this->timer != 0 || fqrand() > 0.05f) {
            this->actor.world.rot.y += 0x100;
        } else {
            yawChange = get_random_timer(0x2000, 0x2000);
            this->targetYaw = (fqrand() < 0.5f ? -1 : 1) * yawChange + this->actor.world.rot.y;
        }
    }

    if (this->timer == 0 && this->actor.yDistToPlayer < 0.0f && this->actor.xzDistToPlayer < 120.0f) {
        mode_ready_init(this);
    }

    Actor_level_SE_set(&this->actor, NA_SE_EN_EIER_FLY - SFX_FLAG);
}

static void mode_ready(EnEiyer* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f focus;

    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->actor.shape.rot.x > 0 && this->actor.shape.rot.x < 0x8000) {
        focus.x = player->actor.world.pos.x;
        focus.y = player->actor.world.pos.y + 20.0f;
        focus.z = player->actor.world.pos.z;

        if (chase_angle(&this->actor.shape.rot.x, Actor_search_position_angleX(&this->actor, &focus), 0x1000)) {
            mode_attack_init(this, play);
        }
    } else {
        this->actor.shape.rot.x -= 0x1000;
    }

    this->actor.world.rot.x = -this->actor.shape.rot.x;
    chase_f(&this->actor.speed, 5.0f, 0.3f);
    adds(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 2, 0x71C);
    Actor_level_SE_set(&this->actor, NA_SE_EN_EIER_FLY - SFX_FLAG);
}

static void mode_attack(EnEiyer* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);
    this->actor.speed *= 1.1f;

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        mode_ground_down_init(this);
    }

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~(AT_ON | AT_HIT);
    }

    Actor_level_SE_set(&this->actor, NA_SE_EN_EIER_FLY - SFX_FLAG);
}

void mode_ground_down(EnEiyer* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);
    chase_angle(&this->actor.world.rot.x, -0x4000, 0x450);
    chase_f(&this->actor.speed, 7.0f, 1.0f);

    if (this->timer == -1) {
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            this->timer = 10;
            Effect_SE_Info_new(play, &this->actor.world.pos, 30, NA_SE_EN_OCTAROCK_SINK);

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                Effect_SS_G_Splash_sc_cl_ct(play, &this->actor.world.pos, NULL, NULL, 1, 700);
            }
        }
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            this->actor.shape.rot.x = 0;
            this->actor.world.rot.x = 0;
            mode_ground_init(this);
        }
    }
}

static void mode_damage(EnEiyer* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->timer != 0) {
        this->timer--;
    }

    add_calc2(&this->basePos.y, this->actor.floorHeight + 80.0f + 5.0f, 0.5f, this->actor.speed);
    this->actor.world.pos.y = this->basePos.y - 5.0f;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->targetYaw = this->actor.wallYaw;
    } else {
        this->targetYaw = this->actor.yawTowardsPlayer + 0x8000;
    }

    chase_angle(&this->actor.world.rot.y, this->targetYaw, 0x38E);
    chase_angle(&this->actor.shape.rot.x, 0, 0x200);
    this->actor.shape.rot.z = sinf(this->timer * (M_PI / 5)) * 5120.0f;

    if (this->timer == 0) {
        this->actor.shape.rot.x = 0;
        this->actor.shape.rot.z = 0;
        this->collider.base.acFlags |= AC_ON;
        mode_swim_init(this);
    }
    this->actor.world.rot.x = -this->actor.shape.rot.x;
}

static void mode_predead(EnEiyer* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->actor.speed > 0.0f) {
        chase_angle(&this->actor.shape.rot.x, -0x4000, 0x400);
    } else {
        chase_angle(&this->actor.shape.rot.x, 0x4000, 0x400);
    }

    this->actor.shape.rot.z += 0x1000;

    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.world.rot.x = -this->actor.shape.rot.x;

    if (this->timer == 0 || this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) {
        mode_dead_init(this);
    }
}

static void mode_dead(EnEiyer* this, PlayState* play) {
    this->actor.shape.shadowAlpha = CLAMP_MIN((s16)(this->actor.shape.shadowAlpha - 5), 0);
    this->actor.world.pos.y -= 2.0f;

    if (this->actor.shape.shadowAlpha == 0) {
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 80);
        Actor_delete(&this->actor);
    }
}

static void mode_stop(EnEiyer* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    chase_angle(&this->actor.shape.rot.x, 0, 0x200);
    Skeleton_Info2_anime_play(&this->skelanime);

    if (Skeleton_Info_frame_check(&this->skelanime, 0.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_EIER_FLUTTER);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
    }

    if (this->timer == 0) {
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->collider.dim.height = EiyerAllPipeData.dim.height;
        mode_swim_init(this);
    }
}

void En_Eiyer_damage_proc(EnEiyer* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elem, true);

        if (this->actor.colChkInfo.damageEffect != 0 || this->actor.colChkInfo.damage != 0) {
            if (hp_down(&this->actor) == 0) {
                Actor_info_finish(play, &this->actor);
                Actor_SE_set(&this->actor, NA_SE_EN_EIER_DEAD);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            }

            // If underground, one hit kill
            if (this->collider.elem.acDmgInfo.dmgFlags == (DMG_BOOMERANG | DMG_EXPLOSIVE | DMG_DEKU_NUT)) {
                if (this->actor.colChkInfo.damage == 0) {
                    mode_rotate_init(this, play);
                } else {
                    mode_predead_init(this);
                }
            } else if (this->actor.colChkInfo.damageEffect == 1) {
                if (this->actionFunc != mode_stop) {
                    mode_stop_init(this);
                }
            } else if (this->actor.colChkInfo.health != 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_EIER_DAMAGE);
                mode_damage_init(this);
            } else {
                this->collider.dim.height = EiyerAllPipeData.dim.height;
                mode_predead_init(this);
            }
        }
    }
}

void En_Eiyer_actor_move(Actor* thisx, PlayState* play) {
    EnEiyer* this = (EnEiyer*)thisx;
    s32 pad;

    En_Eiyer_damage_proc(this, play);
    this->actionFunc(this, play);

    if (this->actor.world.rot.x == 0 || this->actionFunc == mode_stop) {
        Actor_position_moveF(&this->actor);
    } else {
        Actor_position_moveF_XY(&this->actor);
    }

    if (this->actionFunc == mode_swim || this->actionFunc == mode_attack ||
        this->actionFunc == mode_stop || this->actionFunc == mode_predead || this->actionFunc == mode_damage ||
        (this->actionFunc == mode_ground_down && this->timer == -1)) {
        Actor_BGcheck2(play, &this->actor, 5.0f, 27.0f, 30.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    }

    if (this->actor.params == 0xA ||
        (this->actionFunc != mode_ground && this->actionFunc != mode_move)) {
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    // only the main Eiyer can ambush the player
    if (this->actor.params == 0 || this->actor.params == 0xA) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        if (this->collider.base.atFlags & AT_ON) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->actionFunc != mode_rotate) {
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        }
    }

    if (this->actor.flags & ACTOR_FLAG_ATTENTION_ENABLED) {
        this->actor.focus.pos.x = this->actor.world.pos.x + sin_s(this->actor.shape.rot.y) * 12.5f;
        this->actor.focus.pos.z = this->actor.world.pos.z + cos_s(this->actor.shape.rot.y) * 12.5f;
        this->actor.focus.pos.y = this->actor.world.pos.y;
    }
}

s32 en_eiyer_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                             Gfx** gfx) {
    EnEiyer* this = (EnEiyer*)thisx;

    if (limbIndex == 1) {
        pos->z += 2500.0f;
    }

    if (this->collider.elem.acDmgInfo.dmgFlags == (DMG_BOOMERANG | DMG_EXPLOSIVE | DMG_DEKU_NUT) && limbIndex != 9 &&
        limbIndex != 10) {
        *dList = NULL;
    }
    return 0;
}

void En_Eiyer_actor_draw(Actor* thisx, PlayState* play) {
    EnEiyer* this = (EnEiyer*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_eiyer.c", 1494);
    if (this->actionFunc != mode_dead) {
        _texture_z_light_fog_prim(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, &Actor_change_render_mode[2]);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);

        POLY_OPA_DISP = Si2_draw2(play, this->skelanime.skeleton, this->skelanime.jointTable,
                                       en_eiyer_display1, NULL, this, POLY_OPA_DISP);
    } else {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, Actor_change_render_mode);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->actor.shape.shadowAlpha);

        POLY_XLU_DISP = Si2_draw2(play, this->skelanime.skeleton, this->skelanime.jointTable,
                                       en_eiyer_display1, NULL, this, POLY_XLU_DISP);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_eiyer.c", 1541);
}
