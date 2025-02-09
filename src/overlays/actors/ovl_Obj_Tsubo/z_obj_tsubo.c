/*
 * File: z_obj_tsubo.c
 * Overlay: ovl_Obj_Tsubo
 * Description: Breakable pot
 */

#include "z_obj_tsubo.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "assets/objects/object_tsubo/object_tsubo.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_THROW_ONLY)

void Obj_Tsubo_actor_ct(Actor* thisx, PlayState* play);
void Obj_Tsubo_actor_dt(Actor* thisx, PlayState* play2);
void Obj_Tsubo_actor_move(Actor* thisx, PlayState* play);
void Obj_Tsubo_actor_draw(Actor* thisx, PlayState* play);

static void setItem_tsubo(ObjTsubo* this, PlayState* play);
static void setSpd_gravity(ObjTsubo* this);
static s32 set_pos_groundY(ObjTsubo* this, PlayState* play);
void set_collision_tsubo(Actor* thisx, PlayState* play);
static void eff_obj_tsubo(ObjTsubo* this, PlayState* play);
static void eff_obj_tsubo_inWater(ObjTsubo* this, PlayState* play);
static void mv_dmaWait_init(ObjTsubo* this);
static void mv_dmaWait(ObjTsubo* this, PlayState* play);
static void mv_stop_init(ObjTsubo* this);
static void mv_stop(ObjTsubo* this, PlayState* play);
static void mv_carry_init(ObjTsubo* this);
static void mv_carry(ObjTsubo* this, PlayState* play);
static void mv_drop_init(ObjTsubo* this);
static void mv_drop(ObjTsubo* this, PlayState* play);

static s16 Tsubo_AngGoalSpdX = 0;
static s16 Tsubo_AngSpdX = 0;
static s16 Tsubo_AngGoalSpdY = 0;
static s16 Tsubo_AngSpdY = 0;

ActorProfile Obj_Tsubo_Profile = {
    /**/ ACTOR_OBJ_TSUBO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(ObjTsubo),
    /**/ Obj_Tsubo_actor_ct,
    /**/ Obj_Tsubo_actor_dt,
    /**/ Obj_Tsubo_actor_move,
    /**/ NULL,
};

static s16 TsuboBankTbl[] = { OBJECT_GAMEPLAY_DANGEON_KEEP, OBJECT_TSUBO };

static Gfx* TsuboModelTbl[] = { gPotDL, object_tsubo_DL_0017C0 };

static Gfx* TsuboHahenModelTbl[] = { gPotFragmentDL, object_tsubo_DL_001960 };

static ColliderCylinderInit ClPipeDt_tsubo = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_PLAYER,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000002, 0x00, 0x01 },
        { 0x4FC1FFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 9, 26, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit StatusDt_tsubo[] = { 0, 12, 60, MASS_IMMOVABLE };

static InitChainEntry value_init[] = {
    ICHAIN_F32_DIV1000(gravity, -1200, ICHAIN_CONTINUE),  ICHAIN_F32_DIV1000(minVelocityY, -20000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 150, ICHAIN_CONTINUE),    ICHAIN_F32(cullingVolumeDistance, 900, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE), ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
};

static void setItem_tsubo(ObjTsubo* this, PlayState* play) {
    s16 dropParams = PARAMS_GET_U(this->actor.params, 0, 5);

    if ((dropParams >= 0) && (dropParams < ITEM00_MAX)) {
        Item_set0(play, &this->actor.world.pos, dropParams | (PARAMS_GET_U(this->actor.params, 9, 6) << 8));
    }
}

static void setSpd_gravity(ObjTsubo* this) {
    this->actor.velocity.y += this->actor.gravity;
    if (this->actor.velocity.y < this->actor.minVelocityY) {
        this->actor.velocity.y = this->actor.minVelocityY;
    }
}

static s32 set_pos_groundY(ObjTsubo* this, PlayState* play) {
    CollisionPoly* groundPoly;
    Vec3f pos;
    s32 bgId;
    f32 groundY;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + 20.0f;
    pos.z = this->actor.world.pos.z;
    groundY = T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &groundPoly, &bgId, &this->actor, &pos);
    if (groundY > BGCHECK_Y_MIN) {
        this->actor.world.pos.y = groundY;
        xyz_t_move(&this->actor.home.pos, &this->actor.world.pos);
        return true;
    } else {
        PRINTF("地面に付着失敗\n");
        return false;
    }
}

void set_collision_tsubo(Actor* thisx, PlayState* play) {
    ObjTsubo* this = (ObjTsubo*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &ClPipeDt_tsubo);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
}

void Obj_Tsubo_actor_ct(Actor* thisx, PlayState* play) {
    ObjTsubo* this = (ObjTsubo*)thisx;

    ValueSet_process(&this->actor, value_init);
    set_collision_tsubo(&this->actor, play);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, StatusDt_tsubo);
    if (!set_pos_groundY(this, play)) {
        Actor_delete(&this->actor);
        return;
    }
    this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, TsuboBankTbl[PARAMS_GET_U(this->actor.params, 8, 1)]);
    if (this->requiredObjectSlot < 0) {
        PRINTF("Error : バンク危険！ (arg_data 0x%04x)(%s %d)\n", this->actor.params, "../z_obj_tsubo.c", 410);
        Actor_delete(&this->actor);
    } else {
        mv_dmaWait_init(this);
        PRINTF("(dungeon keep 壷)(arg_data 0x%04x)\n", this->actor.params);
    }
}

void Obj_Tsubo_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjTsubo* this = (ObjTsubo*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void eff_obj_tsubo(ObjTsubo* this, PlayState* play) {
    s32 pad;
    f32 rand;
    s16 angle;
    Vec3f pos;
    Vec3f velocity;
    f32 sins;
    f32 coss;
    s32 arg5;
    s32 i;

    for (i = 0, angle = 0; i < 15; i++, angle += 0x4E20) {
        sins = sin_s(angle);
        coss = cos_s(angle);
        pos.x = sins * 8.0f;
        pos.y = (fqrand() * 5.0f) + 2.0f;
        pos.z = coss * 8.0f;
        velocity.x = pos.x * 0.23f;
        velocity.y = (fqrand() * 5.0f) + 2.0f;
        velocity.z = pos.z * 0.23f;
        xyz_t_add(&pos, &this->actor.world.pos, &pos);
        rand = fqrand();
        if (rand < 0.2f) {
            arg5 = 96;
        } else if (rand < 0.6f) {
            arg5 = 64;
        } else {
            arg5 = 32;
        }
        Effect_Kakera_ct2(play, &pos, &velocity, &this->actor.world.pos, -240, arg5, 10, 10, 0,
                             (fqrand() * 95.0f) + 15.0f, 0, 32, 60, KAKERA_COLOR_NONE,
                             TsuboBankTbl[PARAMS_GET_U(this->actor.params, 8, 1)],
                             TsuboHahenModelTbl[PARAMS_GET_U(this->actor.params, 8, 1)]);
    }
    dust_fly_set2(play, &this->actor.world.pos, 30.0f, 4, 20, 50, 1);
}

static void eff_obj_tsubo_inWater(ObjTsubo* this, PlayState* play) {
    s32 pad[2];
    s16 angle;
    Vec3f pos = this->actor.world.pos;
    Vec3f velocity;
    s32 phi_s0;
    s32 i;

    pos.y += this->actor.depthInWater;
    Effect_SS_G_Splash_sc_cl_ct(play, &pos, NULL, NULL, 0, 400);
    for (i = 0, angle = 0; i < 15; i++, angle += 0x4E20) {
        f32 sins = sin_s(angle);
        f32 coss = cos_s(angle);

        pos.x = sins * 8.0f;
        pos.y = (fqrand() * 5.0f) + 2.0f;
        pos.z = coss * 8.0f;
        velocity.x = pos.x * 0.2f;
        velocity.y = (fqrand() * 4.0f) + 2.0f;
        velocity.z = pos.z * 0.2f;
        xyz_t_add(&pos, &this->actor.world.pos, &pos);
        phi_s0 = (fqrand() < .2f) ? 64 : 32;
        Effect_Kakera_ct2(play, &pos, &velocity, &this->actor.world.pos, -180, phi_s0, 30, 30, 0,
                             (fqrand() * 95.0f) + 15.0f, 0, 32, 70, KAKERA_COLOR_NONE,
                             TsuboBankTbl[PARAMS_GET_U(this->actor.params, 8, 1)],
                             TsuboHahenModelTbl[PARAMS_GET_U(this->actor.params, 8, 1)]);
    }
}

static void mv_dmaWait_init(ObjTsubo* this) {
    this->actionFunc = mv_dmaWait;
}

static void mv_dmaWait(ObjTsubo* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->actor.draw = Obj_Tsubo_actor_draw;
        this->actor.objectSlot = this->requiredObjectSlot;
        mv_stop_init(this);
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }
}

static void mv_stop_init(ObjTsubo* this) {
    this->actionFunc = mv_stop;
}

static void mv_stop(ObjTsubo* this, PlayState* play) {
    s32 pad;
    s16 temp_v0;
    s32 phi_v1;

    if (Actor_carry_check(&this->actor, play)) {
        mv_carry_init(this);
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->actor.depthInWater > 15.0f)) {
        eff_obj_tsubo_inWater(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EV_POT_BROKEN);
        setItem_tsubo(this, play);
        Actor_delete(&this->actor);
    } else if ((this->collider.base.acFlags & AC_HIT) &&
               (this->collider.elem.acHitElem->atDmgInfo.dmgFlags &
                (DMG_SWORD | DMG_RANGED | DMG_HAMMER | DMG_BOOMERANG | DMG_EXPLOSIVE))) {
        eff_obj_tsubo(this, play);
        setItem_tsubo(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EV_POT_BROKEN);
        Actor_delete(&this->actor);
    } else {
        if (this->actor.xzDistToPlayer < 600.0f) {
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
            this->collider.base.acFlags &= ~AC_HIT;
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
            if (this->actor.xzDistToPlayer < 150.0f) {
                CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
            }
        }
        if (this->actor.xzDistToPlayer < 100.0f) {
            temp_v0 = this->actor.yawTowardsPlayer - GET_PLAYER(play)->actor.world.rot.y;
            phi_v1 = ABS(temp_v0);
            if (phi_v1 >= 0x5556) {
                // GI_NONE in this case allows the player to lift the actor
                Actor_carry_request_set2(&this->actor, play, GI_NONE, 30.0f, 30.0f);
            }
        }
    }
}

static void mv_carry_init(ObjTsubo* this) {
    this->actionFunc = mv_carry;
    this->actor.room = -1;
    //! @bug: This is an unsafe cast, although the sound effect will still play
    player_SE_set((Player*)&this->actor, NA_SE_PL_PULL_UP_POT);
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
}

static void mv_carry(ObjTsubo* this, PlayState* play) {
    if (Actor_carry_end_check(&this->actor, play)) {
        this->actor.room = play->roomCtx.curRoom.num;
        mv_drop_init(this);
        setSpd_gravity(this);
        Actor_position_move(&this->actor);
        Actor_BGcheck2(play, &this->actor, 5.0f, 15.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_7);
    }
}

static void mv_drop_init(ObjTsubo* this) {
    this->actor.velocity.x = sin_s(this->actor.world.rot.y) * this->actor.speed;
    this->actor.velocity.z = cos_s(this->actor.world.rot.y) * this->actor.speed;
    this->actor.colChkInfo.mass = 240;
    Tsubo_AngGoalSpdX = (fqrand() - 0.7f) * 2800.0f;
    Tsubo_AngGoalSpdY = (fqrand() - 0.5f) * 2000.0f;
    Tsubo_AngSpdX = 0;
    Tsubo_AngSpdY = 0;
    this->actionFunc = mv_drop;
}

static void mv_drop(ObjTsubo* this, PlayState* play) {
    s32 pad[2];

    if ((this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_WALL)) ||
        (this->collider.base.atFlags & AT_HIT)) {
        eff_obj_tsubo(this, play);
        setItem_tsubo(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EV_POT_BROKEN);
        Actor_delete(&this->actor);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
        eff_obj_tsubo_inWater(this, play);
        setItem_tsubo(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EV_POT_BROKEN);
        Actor_delete(&this->actor);
    } else {
        setSpd_gravity(this);
        Actor_position_move(&this->actor);
        chase_s(&Tsubo_AngSpdX, Tsubo_AngGoalSpdX, 0x64);
        chase_s(&Tsubo_AngSpdY, Tsubo_AngGoalSpdY, 0x64);
        this->actor.shape.rot.x += Tsubo_AngSpdX;
        this->actor.shape.rot.y += Tsubo_AngSpdY;
        Actor_BGcheck2(play, &this->actor, 5.0f, 15.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_7);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }
}

void Obj_Tsubo_actor_move(Actor* thisx, PlayState* play) {
    ObjTsubo* this = (ObjTsubo*)thisx;

    this->actionFunc(this, play);
}

void Obj_Tsubo_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, TsuboModelTbl[PARAMS_GET_U(thisx->params, 8, 1)]);
}
