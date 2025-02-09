/*
 * File: z_en_tubo_trap.c
 * Overlay: ovl_En_Tubo_Trap
 * Description: Flying pot enemy
 */

#include "z_en_tubo_trap.h"
#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Tubo_Trap_actor_ct(Actor* thisx, PlayState* play);
void En_Tubo_Trap_actor_dt(Actor* thisx, PlayState* play);
void En_Tubo_Trap_actor_move(Actor* thisx, PlayState* play);
void En_Tubo_Trap_actor_disp(Actor* thisx, PlayState* play);

static void mode_wait(EnTuboTrap* this, PlayState* play);
void mode_saucer(EnTuboTrap* this, PlayState* play);
static void mode_attack(EnTuboTrap* this, PlayState* play);

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 9, 23, 0, { 0 } },
};

ActorProfile En_Tubo_Trap_Profile = {
    /**/ ACTOR_EN_TUBO_TRAP,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_DANGEON_KEEP,
    /**/ sizeof(EnTuboTrap),
    /**/ En_Tubo_Trap_actor_ct,
    /**/ En_Tubo_Trap_actor_dt,
    /**/ En_Tubo_Trap_actor_move,
    /**/ En_Tubo_Trap_actor_disp,
};

void En_Tubo_Trap_actor_ct(Actor* thisx, PlayState* play) {
    EnTuboTrap* this = (EnTuboTrap*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 2.0f);
    PRINTF("\n\n");
    PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 壷トラップ ☆☆☆☆☆ %x\n" VT_RST, this->actor.params); // "Urn Trap"
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
    Actor_set_scale(&this->actor, 0.1f);
    this->actionFunc = mode_wait;
}

void En_Tubo_Trap_actor_dt(Actor* thisx, PlayState* play) {
    EnTuboTrap* this = (EnTuboTrap*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void setItem_tsubo(EnTuboTrap* this, PlayState* play) {
    s16 params = this->actor.params;
    s16 dropType = PARAMS_GET_U(params, 6, 10);

    if (dropType >= 0 && dropType < ITEM00_MAX) {
        Item_set0(play, &this->actor.world.pos, dropType | (PARAMS_GET_U(params, 0, 6) << 8));
    }
}

static void eff_obj_tsubo(EnTuboTrap* this, PlayState* play) {
    f32 rand;
    f32 cos;
    f32 sin;
    Vec3f pos;
    Vec3f velocity;
    s16 var;
    s32 arg5;
    s32 i;
    Vec3f* actorPos = &this->actor.world.pos;

    for (i = 0, var = 0; i < 15; i++, var += 20000) {
        sin = sin_s(var);
        cos = cos_s(var);
        pos.x = sin * 8.0f;
        pos.y = (fqrand() * 5.0f) + 2.0f;
        pos.z = cos * 8.0f;

        velocity.x = pos.x * 0.23f;
        velocity.y = (fqrand() * 5.0f) + 2.0f;
        velocity.z = pos.z * 0.23f;

        pos.x += actorPos->x;
        pos.y += actorPos->y;
        pos.z += actorPos->z;

        rand = fqrand();
        if (rand < 0.2f) {
            arg5 = 96;
        } else if (rand < 0.6f) {
            arg5 = 64;
        } else {
            arg5 = 32;
        }

        Effect_Kakera_ct2(play, &pos, &velocity, actorPos, -240, arg5, 10, 10, 0, (fqrand() * 65.0f) + 15.0f,
                             0, 32, 60, KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_DANGEON_KEEP, gPotFragmentDL);
    }

    dust_fly_set2(play, actorPos, 30.0f, 4, 20, 50, 0);
}

static void eff_obj_tsubo_inWater(EnTuboTrap* this, PlayState* play) {
    f32 rand;
    f32 cos;
    f32 sin;
    Vec3f pos;
    Vec3f velocity;
    s16 var;
    s32 arg5;
    s32 i;
    Vec3f* actorPos = &this->actor.world.pos;

    pos = *actorPos;
    pos.y += this->actor.depthInWater;

    Effect_SS_G_Splash_sc_cl_ct(play, &pos, NULL, NULL, 0, 400);

    for (i = 0, var = 0; i < 15; i++, var += 20000) {
        sin = sin_s(var);
        cos = cos_s(var);
        pos.x = sin * 8.0f;
        pos.y = (fqrand() * 5.0f) + 2.0f;
        pos.z = cos * 8.0f;

        velocity.x = pos.x * 0.20f;
        velocity.y = (fqrand() * 4.0f) + 2.0f;
        velocity.z = pos.z * 0.20f;

        pos.x += actorPos->x;
        pos.y += actorPos->y;
        pos.z += actorPos->z;

        rand = fqrand();
        if (rand < 0.2f) {
            arg5 = 64;
        } else {
            arg5 = 32;
        }

        Effect_Kakera_ct2(play, &pos, &velocity, actorPos, -180, arg5, 30, 30, 0, (fqrand() * 65.0f) + 15.0f,
                             0, 32, 70, KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_DANGEON_KEEP, gPotFragmentDL);
    }
}

static void mode_hit_check(EnTuboTrap* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Player* player2 = GET_PLAYER(play);

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->actor.depthInWater > 15.0f)) {
        eff_obj_tsubo_inWater(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EV_BOMB_DROP_WATER);
        setItem_tsubo(this, play);
        Actor_delete(&this->actor);
        return;
    }

    if (this->collider.base.atFlags & AT_BOUNCED) {
        this->collider.base.atFlags &= ~AT_BOUNCED;
        eff_obj_tsubo(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_IT_SHIELD_REFLECT_SW);
        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EV_POT_BROKEN);
        setItem_tsubo(this, play);
        Actor_delete(&this->actor);
        return;
    }

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        eff_obj_tsubo(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EV_EXPLOSION);
        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EV_POT_BROKEN);
        setItem_tsubo(this, play);
        Actor_delete(&this->actor);
        return;
    }

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        if (this->collider.base.at == &player->actor) {
            eff_obj_tsubo(this, play);
            Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EV_POT_BROKEN);
            Effect_SE_Info_new(play, &player2->actor.world.pos, 40, NA_SE_PL_BODY_HIT);
            setItem_tsubo(this, play);
            Actor_delete(&this->actor);
            return;
        }
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        eff_obj_tsubo(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EV_POT_BROKEN);
        setItem_tsubo(this, play);
        Actor_delete(&this->actor);
        return;
    }
}

static void mode_wait(EnTuboTrap* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 targetHeight;

    if (BREG(2) != 0) {
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ わて     ☆☆☆☆☆ %f\n" VT_RST, this->actor.world.pos.y);   // "You"
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ おいどん ☆☆☆☆☆ %f\n" VT_RST, player->actor.world.pos.y); // "Me"
        PRINTF("\n\n");
    }

    if (this->actor.xzDistToPlayer < 200.0f && this->actor.world.pos.y <= player->actor.world.pos.y) {
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        targetHeight = 40.0f + -10.0f * z_common_data.save.linkAge;

        this->targetY = player->actor.world.pos.y + targetHeight;
        if (this->targetY < this->actor.world.pos.y) {
            this->targetY = this->actor.world.pos.y + targetHeight;
        }

        this->originPos = this->actor.world.pos;
        Actor_SE_set(&this->actor, NA_SE_EV_POT_MOVE_START);
        this->actionFunc = mode_saucer;
    }
}

void mode_saucer(EnTuboTrap* this, PlayState* play) {
    this->actor.shape.rot.y += 5000;
    add_calc2(&this->actor.world.pos.y, this->targetY, 0.8f, 3.0f);

    if (fabsf(this->actor.world.pos.y - this->targetY) < 10.0f) {
        this->actor.speed = 10.0f;
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actionFunc = mode_attack;
    }
}

static void mode_attack(EnTuboTrap* this, PlayState* play) {
    f32 dx = this->originPos.x - this->actor.world.pos.x;
    f32 dy = this->originPos.y - this->actor.world.pos.y;
    f32 dz = this->originPos.z - this->actor.world.pos.z;

    Actor_SE_set(&this->actor, NA_SE_EN_TUBOOCK_FLY - SFX_FLAG);

    if (240.0f < sqrtf(SQ(dx) + SQ(dy) + SQ(dz))) {
        add_calc2(&this->actor.gravity, -3.0f, 0.2f, 0.5f);
    }

    this->actor.shape.rot.y += 5000;
    mode_hit_check(this, play);
}

void En_Tubo_Trap_actor_move(Actor* thisx, PlayState* play) {
    EnTuboTrap* this = (EnTuboTrap*)thisx;
    s32 pad;

    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 10.0f, 10.0f, 20.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    Actor_world_to_eye(&this->actor, 0.0f);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
}

void En_Tubo_Trap_actor_disp(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gPotDL);
}
