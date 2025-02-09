/*
 * File: z_en_floormas
 * Overlay: En_Floormas
 * Description: Floormaster
 */

#include "z_en_floormas.h"
#include "assets/objects/object_wallmaster/object_wallmaster.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

#define SPAWN_INVISIBLE 0x8000
#define SPAWN_SMALL 0x10

#define MERGE_LEADER 0x40
#define MERGE_FOLLOWER 0x20

void En_Floormas_actor_ct(Actor* thisx, PlayState* play2);
void En_Floormas_actor_dt(Actor* thisx, PlayState* play);
void En_Floormas_actor_move(Actor* thisx, PlayState* play);
void En_Floormas_actor_draw(Actor* thisx, PlayState* play);

static void mode_catch(EnFloormas* this, PlayState* play);
static void mode_break(EnFloormas* this, PlayState* play);
static void mode_damage_after(EnFloormas* this, PlayState* play);
void En_Floormas_actor_draw_xlu(Actor* thisx, PlayState* play);
static void mode_dummy(EnFloormas* this, PlayState* play);
static void mode_wait_init(EnFloormas* this);
static void mode_stop(EnFloormas* this, PlayState* play);
static void mode_damage(EnFloormas* this, PlayState* play);
void mode_hontai_wait(EnFloormas* this, PlayState* play);
static void mode_jump(EnFloormas* this, PlayState* play);
static void mode_return(EnFloormas* this, PlayState* play);
static void mode_down(EnFloormas* this, PlayState* play);
void mode_buntai_move(EnFloormas* this, PlayState* play);
static void mode_escape(EnFloormas* this, PlayState* play);
static void mode_after(EnFloormas* this, PlayState* play);
static void mode_ready(EnFloormas* this, PlayState* play);
static void mode_turn(EnFloormas* this, PlayState* play);
static void mode_chase(EnFloormas* this, PlayState* play);
void mode_move_end(EnFloormas* this, PlayState* play);
static void mode_move(EnFloormas* this, PlayState* play);
void mode_move_start(EnFloormas* this, PlayState* play);
static void mode_wait(EnFloormas* this, PlayState* play);
static void mode_attack(EnFloormas* this, PlayState* play);

ActorProfile En_Floormas_Profile = {
    /**/ ACTOR_EN_FLOORMAS,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_WALLMASTER,
    /**/ sizeof(EnFloormas),
    /**/ En_Floormas_actor_ct,
    /**/ En_Floormas_actor_dt,
    /**/ En_Floormas_actor_move,
    /**/ En_Floormas_actor_draw,
};

static ColliderCylinderInit FloormasAllPipeData = {
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
        { 0xFFCFFFFF, 0x04, 0x10 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 25, 40, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit FloormasStatusData = { 4, 30, 40, 150 };

static DamageTable FloormasBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(4, 0x2),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(4, 0x4),
    /* Unk arrow 1   */ DMG_ENTRY(4, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(4, 0x2),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(4, 0x4),
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
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_FLOORMASTER, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 5500, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_STOP),
};

void En_Floormas_actor_ct(Actor* thisx, PlayState* play2) {
    EnFloormas* this = (EnFloormas*)thisx;
    PlayState* play = play2;
    s32 invisble;
    s32 pad;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 50.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gWallmasterSkel, &gWallmasterWaitAnim, this->jointTable,
                       this->morphTable, 25);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &FloormasAllPipeData);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, &FloormasBtlData, &FloormasStatusData);
    this->zOffset = -1600;
    invisble = PARAMS_GET_NOSHIFT(this->actor.params, 15, 1); // SPAWN_INVISIBLE

    // s16 cast needed
    this->actor.params &= (s16) ~(SPAWN_INVISIBLE);
    if (invisble) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
        this->actor.draw = En_Floormas_actor_draw_xlu;
    }

    if (this->actor.params == SPAWN_SMALL) {
        this->actor.draw = NULL;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionFunc = mode_dummy;
    } else {
        // spawn first small floormaster
        this->actor.parent =
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_FLOORMAS, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, invisble + SPAWN_SMALL);
        if (this->actor.parent == NULL) {
            Actor_delete(&this->actor);
            return;
        }
        // spawn 2nd small floormaster
        this->actor.child =
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_FLOORMAS, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, invisble + SPAWN_SMALL);
        if (this->actor.child == NULL) {
            Actor_delete(this->actor.parent);
            Actor_delete(&this->actor);
            return;
        }

        // link floormasters together
        this->actor.parent->child = &this->actor;
        this->actor.parent->parent = this->actor.child;
        this->actor.child->parent = &this->actor;
        this->actor.child->child = this->actor.parent;
        mode_wait_init(this);
    }
}

void En_Floormas_actor_dt(Actor* thisx, PlayState* play) {
    EnFloormas* this = (EnFloormas*)thisx;
    ColliderCylinder* col = &this->collider;
    ClObjPipe_dt(play, col);
}

void floormas_set_ac_shield(EnFloormas* this) {
    this->collider.base.colMaterial = COL_MATERIAL_HARD;
    this->collider.base.acFlags |= AC_HARD;
    this->actionTarget = 0x28;
}

void floormas_set_ac_normal(EnFloormas* this) {
    this->collider.base.colMaterial = COL_MATERIAL_HIT0;
    this->actionTarget = 0;
    this->collider.base.acFlags &= ~AC_HARD;
}

static void mode_wait_init(EnFloormas* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gWallmasterWaitAnim);
    this->actionFunc = mode_wait;
    this->actor.speed = 0.0f;
}

void mode_move_start_init(EnFloormas* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gWallmasterStandUpAnim, -3.0f);
    this->actionFunc = mode_move_start;
}

static void mode_move_init(EnFloormas* this) {
    if (this->actionFunc != mode_chase) {
        Skeleton_Info2_init_standard_speedset_repeat(&this->skelAnime, &gWallmasterWalkAnim, 1.5f);
    } else {
        this->skelAnime.playSpeed = 1.5f;
    }

    this->actionTimer = get_random_timer(2, 4);
    this->actionFunc = mode_move;
    this->actor.speed = 1.5f;
}

void mode_move_end_init(EnFloormas* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gWallmasterStopWalkAnim);
    this->actionFunc = mode_move_end;
    this->actor.speed = 0.0f;
}

static void mode_chase_init(EnFloormas* this) {
    this->actionTimer = 0;
    this->actionFunc = mode_chase;
    this->actor.speed = 5.0f;
    this->skelAnime.playSpeed = 3.0f;
}

static void mode_turn_init(EnFloormas* this) {
    s16 rotDelta = this->actionTarget - this->actor.shape.rot.y;

    this->actor.speed = 0.0f;
    if (rotDelta > 0) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gFloormasterTurnAnim, -3.0f);
    } else {
        Skeleton_Info2_init(&this->skelAnime, &gFloormasterTurnAnim, -1.0f, Si2_anime_end_frame(&gFloormasterTurnAnim),
                         0.0f, ANIMMODE_ONCE, -3.0f);
    }

    if (this->actor.scale.x > 0.004f) {
        this->actionTarget = (rotDelta * (2.0f / 30.0f));
    } else {
        this->skelAnime.playSpeed *= 2.0f;
        this->actionTarget = rotDelta * (2.0f / 15.0f);
    }
    this->actionFunc = mode_turn;
}

static void mode_ready_init(EnFloormas* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gWallmasterHoverAnim, 3.0f, 0, Si2_anime_end_frame(&gWallmasterHoverAnim),
                     ANIMMODE_ONCE, -3.0f);
    this->actor.speed = 0.0f;
    this->actor.gravity = 0.0f;
    floormas_set_ac_shield(this);
    _dust_ground_set(play, &this->actor, &this->actor.world.pos, 15.0f, 6, 20.0f, 300, 100, true);
    Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_ATTACK);
    this->actionFunc = mode_ready;
}

static void mode_attack_init(EnFloormas* this) {
    this->actionTimer = 25;
    this->actor.gravity = -0.15f;
    this->actionFunc = mode_attack;
    this->actor.speed = 0.5f;
}

static void mode_after_init(EnFloormas* this) {
    Skeleton_Info2_init(&this->skelAnime, &gWallmasterJumpAnim, 1.0f, 41.0f, 42.0f, ANIMMODE_ONCE, 5.0f);
    if ((this->actor.speed < 0.0f) || (this->actionFunc != mode_attack)) {
        this->actionTimer = 30;
    } else {
        this->actionTimer = 45;
    }
    this->actor.gravity = -1.0f;
    this->actionFunc = mode_after;
}

static void mode_break_init(EnFloormas* this) {
    Actor_set_scale(&this->actor, 0.004f);
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_REACT_TO_LENS)) {
        this->actor.draw = En_Floormas_actor_draw_xlu;
    } else {
        this->actor.draw = En_Floormas_actor_draw;
    }
    this->actor.shape.rot.y = this->actor.parent->shape.rot.y + 0x5555;
    this->actor.world.pos = this->actor.parent->world.pos;
    this->actor.params = 0x10;
    Skeleton_Info2_init(&this->skelAnime, &gWallmasterJumpAnim, 1.0f, 41.0f, Si2_anime_end_frame(&gWallmasterJumpAnim),
                     ANIMMODE_ONCE, 0.0f);
    this->collider.dim.radius = FloormasAllPipeData.dim.radius * 0.6f;
    this->collider.dim.height = FloormasAllPipeData.dim.height * 0.6f;
    this->collider.elem.acElemFlags &= ~ACELEM_HOOKABLE;
    this->actor.speed = 4.0f;
    this->actor.velocity.y = 7.0f;
    // using div creates a signed check.
    this->actor.colChkInfo.health = FloormasStatusData.health >> 1;
    this->actionFunc = mode_break;
}

static void mode_escape_init(EnFloormas* this) {
    Skeleton_Info2_init_standard_speedset_repeat(&this->skelAnime, &gWallmasterWalkAnim, 4.5f);
    this->actionFunc = mode_escape;
    this->actor.speed = 5.0f;
}

void mode_buntai_move_init(EnFloormas* this) {
    if (this->actionFunc != mode_escape) {
        Skeleton_Info2_init_standard_speedset_repeat(&this->skelAnime, &gWallmasterWalkAnim, 4.5f);
    }
    this->actionFunc = mode_buntai_move;
    this->actor.speed = 5.0f;
}

static void mode_down_init(EnFloormas* this, PlayState* play) {
    static Vec3f pvec = { 0.0f, 0.0f, 0.0f };
    static Vec3f pacc = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;

    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + 15.0f;
    pos.z = this->actor.world.pos.z;
    _Effect_SS_Db_ct(play, &pos, &pvec, &pacc, 150, -10, 255, 255, 255, 255, 0, 0, 255, 1, 9, true);
    this->actionFunc = mode_down;
}

static void mode_return_init(EnFloormas* this) {
    Skeleton_Info2_init(&this->skelAnime, &gWallmasterJumpAnim, 2.0f, 0.0f, 41.0f, ANIMMODE_ONCE, 0.0f);
    this->actionFunc = mode_return;
    this->actor.speed = 0.0f;
}

static void mode_jump_init(EnFloormas* this) {
    Skeleton_Info2_init(&this->skelAnime, &gWallmasterJumpAnim, 2.0f, 0.0f, 41.0f, ANIMMODE_ONCE, 0.0f);
    this->actionFunc = mode_jump;
    this->actor.speed = 0.0f;
}

static void mode_catch_init(EnFloormas* this, Player* player) {
    f32 yDelta;
    f32 xzDelta;

    Skeleton_Info2_init(&this->skelAnime, &gWallmasterJumpAnim, 1.0f, 36.0f, 45.0f, ANIMMODE_ONCE, -3.0f);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    floormas_set_ac_shield(this);
    if (!LINK_IS_ADULT) {
        yDelta = CLAMP(-this->actor.yDistToPlayer, 20.0f, 30.0f);
        xzDelta = -10.0f;
    } else {
        yDelta = CLAMP(-this->actor.yDistToPlayer, 25.0f, 45.0f);
        xzDelta = -70.0f;
    }
    this->actor.world.pos.y = player->actor.world.pos.y + yDelta;
    this->actor.world.pos.x = sin_s(this->actor.shape.rot.y) * (xzDelta * 0.1f) + player->actor.world.pos.x;
    this->actor.world.pos.z = cos_s(this->actor.shape.rot.y) * (xzDelta * 0.1f) + player->actor.world.pos.z;
    this->actor.shape.rot.x = -0x4CE0;
    this->actionFunc = mode_catch;
}

void mode_hontai_wait_init(EnFloormas* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gWallmasterWaitAnim);
    this->actionTimer = 0;
    this->smallActionTimer += 1500;
    floormas_set_ac_shield(this);
    this->actionFunc = mode_hontai_wait;
}

static void mode_dummy_init(EnFloormas* this) {
    EnFloormas* parent = (EnFloormas*)this->actor.parent;
    EnFloormas* child = (EnFloormas*)this->actor.child;

    // if this is the last remaining small floor master, kill all.
    if ((parent->actionFunc == mode_dummy) && (child->actionFunc == mode_dummy)) {
        Actor_delete(&parent->actor);
        Actor_delete(&child->actor);
        Actor_delete(&this->actor);
        return;
    }
    this->actor.draw = NULL;
    this->actionFunc = mode_dummy;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED);
}

static void mode_damage_init(EnFloormas* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gWallmasterDamageAnim, -3.0f);
    if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & (DMG_ARROW | DMG_SLINGSHOT)) {
        this->actor.world.rot.y = this->collider.base.ac->world.rot.y;
    } else {
        this->actor.world.rot.y = Actor_search_actor_angleY(&this->actor, this->collider.base.ac) + 0x8000;
    }
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 20);
    this->actionFunc = mode_damage;
    this->actor.speed = 5.0f;
    this->actor.velocity.y = 5.5f;
}

static void mode_damage_after_init(EnFloormas* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gWallmasterRecoverFromDamageAnim);
    this->actor.velocity.y = this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = mode_damage_after;
}

static void mode_stop_init(EnFloormas* this) {
    Skeleton_Info2_init(&this->skelAnime, &gWallmasterJumpAnim, 1.5f, 0, 20.0f, ANIMMODE_ONCE, -3.0f);
    this->actor.speed = 0.0f;
    if (this->actor.colChkInfo.damageEffect == 4) {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_GRAY, 255, COLORFILTER_BUFFLAG_OPA, 80);
    } else {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 80);
        if (this->actor.scale.x > 0.004f) {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
        }
    }
    this->actionTimer = 80;
    this->actionFunc = mode_stop;
}

void floormas_check_down_or_break(EnFloormas* this, PlayState* play) {
    if (this->actor.scale.x > 0.004f) {
        // split
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer + 0x8000;
        mode_break_init((EnFloormas*)this->actor.child);
        mode_break_init((EnFloormas*)this->actor.parent);
        mode_break_init(this);
        Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_SPLIT);
    } else {
        // Die
        Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x90);
        mode_down_init(this, play);
    }
}

static void mode_wait(EnFloormas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        // within 400 units and within 90 degrees rotation of player
        if (this->actor.xzDistToPlayer < 400.0f && !Actor_player_direction_check(&this->actor, 0x4000)) {
            this->actionTarget = this->actor.yawTowardsPlayer;
            mode_turn_init(this);
            // within 280 units and within 45 degrees rotation of player
        } else if (this->actor.xzDistToPlayer < 280.0f && Actor_player_direction_check(&this->actor, 0x2000)) {
            mode_ready_init(this, play);
        } else {
            mode_move_start_init(this);
        }
    }
}

void mode_move_start(EnFloormas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.scale.x > 0.004f) {
            mode_move_init(this);
        } else if (this->actor.params == MERGE_FOLLOWER) {
            mode_buntai_move_init(this);
        } else {
            mode_escape_init(this);
        }
    }
}

static void mode_move(EnFloormas* this, PlayState* play) {
    s32 animPastFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    animPastFrame = Skeleton_Info_frame_check(&this->skelAnime, 0.0f);
    if (animPastFrame) {
        if (this->actionTimer != 0) {
            this->actionTimer--;
        }
    }
    if (((animPastFrame || (Skeleton_Info_frame_check(&this->skelAnime, 12.0f))) ||
         (Skeleton_Info_frame_check(&this->skelAnime, 24.0f) != 0)) ||
        (Skeleton_Info_frame_check(&this->skelAnime, 36.0f) != 0)) {
        Actor_SE_set(&this->actor, NA_SE_EN_FALL_WALK);
    }

    if ((this->actor.xzDistToPlayer < 320.0f) && (Actor_player_direction_check(&this->actor, 0x4000))) {
        mode_chase_init(this);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        // set target rotation to the colliding wall's rotation
        this->actionTarget = this->actor.wallYaw;
        mode_turn_init(this);
    } else if ((this->actor.xzDistToPlayer < 400.0f) && !Actor_player_direction_check(&this->actor, 0x4000)) {
        // set target rotation to player.
        this->actionTarget = this->actor.yawTowardsPlayer;
        mode_turn_init(this);
    } else if (this->actionTimer == 0) {
        mode_move_end_init(this);
    }
}

void mode_move_end(EnFloormas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_wait_init(this);
    }
}

static void mode_chase(EnFloormas* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 12.0f) ||
        Skeleton_Info_frame_check(&this->skelAnime, 24.0f) || Skeleton_Info_frame_check(&this->skelAnime, 36.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_FALL_WALK);
    }

    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x71C);

    if ((this->actor.xzDistToPlayer < 280.0f) && Actor_player_direction_check(&this->actor, 0x2000) &&
        !(this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        mode_ready_init(this, play);
    } else if (this->actor.xzDistToPlayer > 400.0f) {
        mode_move_init(this);
    }
}

static void mode_turn(EnFloormas* this, PlayState* play) {
    char pad[4];
    f32 sp30;
    f32 sp2C;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_move_start_init(this);
    }

    if (((this->skelAnime.playSpeed > 0.0f) && Skeleton_Info_frame_check(&this->skelAnime, 21.0f)) ||
        ((this->skelAnime.playSpeed < 0.0f) && Skeleton_Info_frame_check(&this->skelAnime, 6.0f))) {
        if (this->actor.scale.x > 0.004f) {
            Actor_SE_set(&this->actor, NA_SE_EN_FALL_WALK);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_SM_WALK);
        }
    }
    // Needed to match
    if (!this->skelAnime.curFrame) {}
    if (this->skelAnime.curFrame >= 7.0f && this->skelAnime.curFrame < 22.0f) {
        sp30 = sin_s(this->actor.shape.rot.y + 0x4268);
        sp2C = cos_s(this->actor.shape.rot.y + 0x4268);
        this->actor.shape.rot.y += this->actionTarget;
        this->actor.world.pos.x -=
            (this->actor.scale.x * 2700.0f) * (sin_s(this->actor.shape.rot.y + 0x4268) - sp30);
        this->actor.world.pos.z -=
            (this->actor.scale.x * 2700.0f) * (cos_s(this->actor.shape.rot.y + 0x4268) - sp2C);
    }
}

static void mode_ready(EnFloormas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_attack_init(this);
    }
    this->actor.shape.rot.x += 0x140;
    this->actor.world.pos.y += 10.0f;
    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 2730);
    chase_s(&this->zOffset, 1200, 100);
}

void set_attack_dust(EnFloormas* this, PlayState* play) {
    static Vec3f acc = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    Vec3f pvec;

    pos.x = this->actor.world.pos.x;
    pos.z = this->actor.world.pos.z;
    pos.y = this->actor.floorHeight;

    pvec.y = 2.0f;
    pvec.x = sin_s(this->actor.shape.rot.y + 0x6000) * 7.0f;
    pvec.z = cos_s(this->actor.shape.rot.y + 0x6000) * 7.0f;

    Effect_SS_Dust_sc_li_ct(play, &pos, &pvec, &acc, 450, 100);

    pvec.x = sin_s(this->actor.shape.rot.y - 0x6000) * 7.0f;
    pvec.z = cos_s(this->actor.shape.rot.y - 0x6000) * 7.0f;

    Effect_SS_Dust_sc_li_ct(play, &pos, &pvec, &acc, 450, 100);

    Actor_level_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_SLIDING - SFX_FLAG);
}

static void mode_attack(EnFloormas* this, PlayState* play) {
    f32 distFromGround;

    if (this->actionTimer != 0) {
        this->actionTimer--;
    }

    chase_f(&this->actor.speed, 15.0f, SQ(this->actor.speed) * (1.0f / 3.0f));
    chase_angle(&this->actor.shape.rot.x, -0x1680, 0x140);

    distFromGround = this->actor.world.pos.y - this->actor.floorHeight;
    if (distFromGround < 10.0f) {
        this->actor.world.pos.y = this->actor.floorHeight + 10.0f;
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = 0.0f;
    }

    if (distFromGround < 12.0f) {
        set_attack_dust(this, play);
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->actionTimer == 0)) {
        mode_after_init(this);
    }
}

static void mode_after(EnFloormas* this, PlayState* play) {
    s32 isOnGround;

    isOnGround = this->actor.bgCheckFlags & BGCHECKFLAG_GROUND;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        if (this->actor.params != MERGE_LEADER) {
            floormas_set_ac_normal(this);
        }

        if (this->actor.velocity.y < -4.0f) {
            if (this->actor.scale.x > 0.004f) {
                Actor_SE_set(&this->actor, NA_SE_EN_FALL_LAND);
            } else {
                Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
            }
        }
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.speed = 0.0f;
    }

    if (isOnGround) {
        chase_f(&this->actor.speed, 0.0f, 2.0f);
    }

    if ((this->actor.speed > 0.0f) && ((this->actor.world.pos.y - this->actor.floorHeight) < 12.0f)) {
        set_attack_dust(this, play);
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actionTimer != 0) {
            this->actionTimer--;
        }

        if (this->actionTimer == 0 && isOnGround) {
            if (this->skelAnime.endFrame < 45.0f) {
                this->skelAnime.endFrame = Si2_anime_end_frame(&gWallmasterJumpAnim);
            } else if (this->actor.params == MERGE_LEADER) {
                mode_hontai_wait_init(this);
            } else {
                mode_move_start_init(this);
                this->smallActionTimer = 50;
            }
        }
    }

    chase_angle(&this->actor.shape.rot.x, 0, 0x140);
    chase_s(&this->zOffset, -1600, 100);
}

static void mode_break(EnFloormas* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (Skeleton_Info2_anime_play(&this->skelAnime)) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->smallActionTimer = 50;
            mode_move_start_init(this);
        }
        chase_f(&this->actor.speed, 0.0f, 1.0f);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
    }
}

static void mode_escape(EnFloormas* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    DECR(this->smallActionTimer);

    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 18.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_SM_WALK);
    }

    if (this->smallActionTimer == 0) {
        mode_buntai_move_init(this);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actionTarget = this->actor.wallYaw;
        mode_turn_init(this);
    } else if (this->actor.xzDistToPlayer < 120.0f) {
        chase_angle(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer + 0x8000, 0x38E);
    }
}

void mode_buntai_move(EnFloormas* this, PlayState* play) {
    Actor* primaryFloormas;
    s32 isAgainstWall;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 18.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_SM_WALK);
    }
    isAgainstWall = this->actor.bgCheckFlags & BGCHECKFLAG_WALL;
    if (isAgainstWall) {
        this->actionTarget = this->actor.wallYaw;
        mode_turn_init(this);
        return;
    }

    if (this->actor.params == MERGE_FOLLOWER) {
        if (this->actor.parent->params == MERGE_LEADER) {
            primaryFloormas = this->actor.parent;
        } else if (this->actor.child->params == MERGE_LEADER) {
            primaryFloormas = this->actor.child;
        } else {
            this->actor.params = 0x10;
            return;
        }

        chase_angle(&this->actor.shape.rot.y, Actor_search_actor_angleY(&this->actor, primaryFloormas), 0x38E);
        if (Actor_search_actor_distanceXZ(&this->actor, primaryFloormas) < 80.0f) {
            mode_return_init(this);
        }
    } else {
        adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x71C);
        if (this->actor.xzDistToPlayer < 80.0f) {
            mode_jump_init(this);
        }
    }
}

static void mode_down(EnFloormas* this, PlayState* play) {
    if (chase_f(&this->actor.scale.x, 0.0f, 0.0015f)) {
        mode_dummy_init(this);
    }
    this->actor.scale.z = this->actor.scale.x;
    this->actor.scale.y = this->actor.scale.x;
}

static void mode_jump(EnFloormas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->skelAnime.curFrame < 20.0f) {
        adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 20.0f)) {
        this->actor.speed = 5.0f;
        this->actor.velocity.y = 7.0f;
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actionTimer = 0x32;
        this->actor.speed = 0.0f;
        Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
        mode_after_init(this);
    } else if ((this->actor.yDistToPlayer < -10.0f) && (this->collider.base.ocFlags1 & OC1_HIT) &&
               (&player->actor == this->collider.base.oc)) {
        play->grabPlayer(play, player);
        mode_catch_init(this, player);
    }
}

static void mode_catch(EnFloormas* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnFloormas* parent;
    EnFloormas* child;
    f32 yDelta;
    f32 xzDelta;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->skelAnime.playSpeed > 0.0f) {
            this->skelAnime.playSpeed = -1.0f;
            this->skelAnime.endFrame = 36.0f;
            this->skelAnime.startFrame = 45.0f;
        } else {
            this->skelAnime.playSpeed = 1.0f;
            this->skelAnime.endFrame = 45.0f;
            this->skelAnime.startFrame = 36.0f;
        }
    }

    if (!LINK_IS_ADULT) {
        yDelta = CLAMP(-this->actor.yDistToPlayer, 20.0f, 30.0f);
        xzDelta = -10.0f;
    } else {
        yDelta = CLAMP(-this->actor.yDistToPlayer, 25.0f, 45.0f);
        xzDelta = -30.0f;
    }

    this->actor.world.pos.y = player->actor.world.pos.y + yDelta;
    this->actor.world.pos.x = sin_s(this->actor.shape.rot.y) * (xzDelta * 0.1f) + player->actor.world.pos.x;
    this->actor.world.pos.z = cos_s(this->actor.shape.rot.y) * (xzDelta * 0.1f) + player->actor.world.pos.z;

    // let go
    if (!(player->stateFlags2 & PLAYER_STATE2_7) || (player->invincibilityTimer < 0)) {
        parent = (EnFloormas*)this->actor.parent;
        child = (EnFloormas*)this->actor.child;

        if (((parent->actionFunc == mode_catch) || parent->actionFunc == mode_dummy) &&
            (child->actionFunc == mode_catch || child->actionFunc == mode_dummy)) {

            parent->actor.params = MERGE_FOLLOWER;
            child->actor.params = MERGE_FOLLOWER;
            this->actor.params = MERGE_LEADER;
        }

        this->actor.shape.rot.x = 0;
        this->actor.velocity.y = 6.0f;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.speed = -3.0f;
        mode_after_init(this);
    } else {
        // Damage link every 20 frames
        if ((this->actionTarget % 20) == 0) {
            if (!LINK_IS_ADULT) {
                player_SE_set(player, NA_SE_VO_LI_DAMAGE_S_KID);
            } else {
                player_SE_set(player, NA_SE_VO_LI_DAMAGE_S);
            }
            play->damagePlayer(play, -8);
        }
    }

    Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_SM_STICK - SFX_FLAG);
}

static void mode_return(EnFloormas* this, PlayState* play) {
    Actor* primFloormas;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->actor.parent->params == MERGE_LEADER) {
        primFloormas = this->actor.parent;
    } else if (this->actor.child->params == MERGE_LEADER) {
        primFloormas = this->actor.child;
    } else {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            this->actor.params = 0x10;
            mode_after_init(this);
        }
        return;
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 20.0f)) {
        this->actor.speed = 5.0f;
        this->actor.velocity.y = 7.0f;
    } else if (this->skelAnime.curFrame < 20.0f) {
        adds(&this->actor.shape.rot.y, Actor_search_actor_angleY(&this->actor, primFloormas), 2, 0xE38);
    } else if ((((primFloormas->world.pos.y - this->actor.world.pos.y) < -10.0f) &&
                (fabsf(this->actor.world.pos.x - primFloormas->world.pos.x) < 10.0f)) &&
               (fabsf(this->actor.world.pos.z - primFloormas->world.pos.z) < 10.0f)) {
        mode_dummy_init(this);
        this->collider.base.ocFlags1 |= OC1_ON;
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        this->actor.speed = 0.0f;
        Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_SM_LAND);
        mode_after_init(this);
    }

    if (fabsf(this->actor.world.pos.x - primFloormas->world.pos.x) < 5.0f &&
        fabsf(this->actor.world.pos.z - primFloormas->world.pos.z) < 5.0f) {
        chase_f(&this->actor.speed, 0, 2.0f);
    }
}

void mode_hontai_wait(EnFloormas* this, PlayState* play) {
    EnFloormas* parent;
    EnFloormas* child;
    s32 mergeCnt;
    f32 prevScale;
    f32 curScale;

    mergeCnt = 0;

    DECR(this->smallActionTimer);

    parent = (EnFloormas*)this->actor.parent;
    child = (EnFloormas*)this->actor.child;

    if (this->smallActionTimer == 0) {
        if (parent->actionFunc != mode_dummy) {
            mode_down_init(parent, play);
        }

        if (child->actionFunc != mode_dummy) {
            mode_down_init(child, play);
        }
    } else {
        if ((parent->actionFunc != mode_dummy) && (parent->actionFunc != mode_down)) {
            mergeCnt++;
        }

        if ((child->actionFunc != mode_dummy) && (child->actionFunc != mode_down)) {
            mergeCnt++;
        }
    }

    prevScale = this->actor.scale.x;

    if (mergeCnt == 1) {
        chase_f(&this->actor.scale.x, 0.007f, 0.0005f);
    } else if (mergeCnt == 0) {
        chase_f(&this->actor.scale.x, 0.01f, 0.0005f);
    }

    curScale = this->actor.scale.x;
    this->actor.scale.y = this->actor.scale.z = curScale;

    if (((prevScale == 0.007f) || (prevScale == 0.004f)) && (prevScale != this->actor.scale.x)) {
        Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_EXPAND);
    }

    this->collider.dim.radius = (FloormasAllPipeData.dim.radius * 100.0f) * this->actor.scale.x;
    this->collider.dim.height = (FloormasAllPipeData.dim.height * 100.0f) * this->actor.scale.x;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.scale.x >= 0.01f) {
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            floormas_set_ac_normal(this);
            this->actor.params = 0;
            this->collider.elem.acElemFlags |= ACELEM_HOOKABLE;
            this->actor.colChkInfo.health = FloormasStatusData.health;
            mode_move_start_init(this);
        } else {
            if (this->actionTimer == 0) {
                Skeleton_Info2_init_standard_stop(&this->skelAnime, &gFloormasterTapFingerAnim);
                this->actionTimer = 1;
            } else {
                Skeleton_Info2_init_standard_stop(&this->skelAnime, &gWallmasterWaitAnim);
                this->actionTimer = 0;
            }
        }
    }
    Actor_level_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_RESTORE - SFX_FLAG);
}

static void mode_dummy(EnFloormas* this, PlayState* play) {
}

static void mode_damage(EnFloormas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.colChkInfo.health == 0) {
            floormas_check_down_or_break(this, play);
        } else {
            mode_damage_after_init(this);
        }
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, 13.0f)) {
        if (this->actor.scale.x > 0.004f) {
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        }
    }
    chase_f(&this->actor.speed, 0.0f, 0.2f);
}

static void mode_damage_after(EnFloormas* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_move_start_init(this);
    }
}

static void mode_stop(EnFloormas* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (this->actionTimer == 0) {
        if (this->actor.colChkInfo.health == 0) {
            floormas_check_down_or_break(this, play);
            return;
        }
        mode_move_start_init(this);
    }
}

void En_Floormas_damage_proc(EnFloormas* this, PlayState* play) {
    s32 pad;
    s32 isSmall;

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elem, true);
        if ((this->actor.colChkInfo.damageEffect != 0) || (this->actor.colChkInfo.damage != 0)) {
            if (this->collider.base.colMaterial != COL_MATERIAL_HARD) {
                isSmall = false;
                if (this->actor.scale.x < 0.01f) {
                    isSmall = true;
                }
                if (isSmall && this->collider.elem.acHitElem->atDmgInfo.dmgFlags & DMG_HOOKSHOT) {
                    this->actor.colChkInfo.damage = 2;
                    this->actor.colChkInfo.damageEffect = 0;
                }
                if (hp_down(&this->actor) == 0) {
                    if (isSmall) {
                        Actor_SE_set(&this->actor, NA_SE_EN_FLOORMASTER_SM_DEAD);
                    } else {
                        Actor_SE_set(&this->actor, NA_SE_EN_FALL_DEAD);
                    }
                    Actor_info_finish(play, &this->actor);
                    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                } else if (this->actor.colChkInfo.damage != 0) {
                    Actor_SE_set(&this->actor, NA_SE_EN_FALL_DAMAGE);
                }

                if ((this->actor.colChkInfo.damageEffect == 4) || (this->actor.colChkInfo.damageEffect == 1)) {
                    if (this->actionFunc != mode_stop) {
                        mode_stop_init(this);
                    }
                } else {
                    if (this->actor.colChkInfo.damageEffect == 2) {
                        Effect_Fcircle_ct(play, &this->actor, &this->actor.world.pos, this->actor.scale.x * 4000.0f,
                                              this->actor.scale.x * 4000.0f);
                    }
                    mode_damage_init(this);
                }
            }
        }
    }
}

void En_Floormas_actor_move(Actor* thisx, PlayState* play) {
    EnFloormas* this = (EnFloormas*)thisx;
    s32 pad;

    if (this->actionFunc != mode_dummy) {
        if (this->collider.base.atFlags & AT_HIT) {
            this->collider.base.atFlags &= ~AT_HIT;
            this->actor.speed *= -0.5f;

            this->actor.speed = CLAMP_MAX(this->actor.speed, -5.0f);
            this->actor.velocity.y = 5.0f;

            mode_after_init(this);
        }
        En_Floormas_damage_proc(this, play);
        this->actionFunc(this, play);

        if (this->actionFunc != mode_damage) {
            this->actor.world.rot.y = this->actor.shape.rot.y;
        }

        if (this->actionFunc != mode_catch) {
            Actor_position_moveF(&this->actor);
        }

        Actor_BGcheck2(play, &this->actor, 20.0f, this->actor.scale.x * 3000.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                    UPDBGCHECKINFO_FLAG_4);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        if (this->actionFunc == mode_attack) {
            this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->actionFunc != mode_catch) {
            if (this->actionFunc != mode_break && this->actionFunc != mode_damage &&
                this->actor.freezeTimer == 0) {
                CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
            }

            if ((this->actionFunc != mode_return) || (this->skelAnime.curFrame < 20.0f)) {
                CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
            }
        }

        Actor_world_to_eye(&this->actor, this->actor.scale.x * 2500.0f);

        if (this->collider.base.colMaterial == COL_MATERIAL_HARD) {
            if (this->actionTarget != 0) {
                this->actionTarget--;
            }

            if (this->actionTarget == 0) {
                this->actionTarget = 0x28;
            }
        }
    }
}

s32 en_floormas_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                                Gfx** gfx) {
    EnFloormas* this = (EnFloormas*)thisx;

    if (limbIndex == 1) {
        pos->z += this->zOffset;
    }
    return false;
}

void en_floormas_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    if (limbIndex == 2) {
        Matrix_push();
        Matrix_translate(1600.0f, -700.0f, -1700.0f, MTXMODE_APPLY);
        Matrix_rotateY(DEG_TO_RAD(60), MTXMODE_APPLY);
        Matrix_rotateZ(DEG_TO_RAD(15), MTXMODE_APPLY);
        Matrix_scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD((*gfx)++, play->state.gfxCtx, "../z_en_floormas.c", 2299);
        gSPDisplayList((*gfx)++, gWallmasterFingerDL);
        Matrix_pull();
    }
}

static Color_RGBA8 fog_color = { 0, 255, 0, 0 };

void En_Floormas_actor_draw(Actor* thisx, PlayState* play) {
    EnFloormas* this = (EnFloormas*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_floormas.c", 2318);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    if (this->collider.base.colMaterial == COL_MATERIAL_HARD) {
        Eff_Set_Fog2(play, &fog_color, this->actionTarget % 0x28, 0x28);
    }

    POLY_OPA_DISP =
        Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                           en_floormas_display1, en_floormas_display2, this, POLY_OPA_DISP);
    if (this->collider.base.colMaterial == COL_MATERIAL_HARD) {
        Eff_Off_Fog(play);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_floormas.c", 2340);
}

void En_Floormas_actor_draw_xlu(Actor* thisx, PlayState* play) {
    EnFloormas* this = (EnFloormas*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_floormas.c", 2352);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    if (this->collider.base.colMaterial == COL_MATERIAL_HARD) {
        Eff_Set_Fog2_xlu(play, &fog_color, this->actionTarget % 0x28, 0x28);
    }
    POLY_XLU_DISP =
        Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                           en_floormas_display1, en_floormas_display2, this, POLY_XLU_DISP);
    if (this->collider.base.colMaterial == COL_MATERIAL_HARD) {
        Eff_Off_Fog_xlu(play);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_floormas.c", 2374);
}
