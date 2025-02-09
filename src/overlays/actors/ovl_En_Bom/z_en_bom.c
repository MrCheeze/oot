/*
 * File: z_en_bom.c
 * Overlay: ovl_En_Bom
 * Description: Bomb
 */

#include "z_en_bom.h"
#include "overlays/effects/ovl_Effect_Ss_Dead_Sound/z_eff_ss_dead_sound.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "rumble.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Bom_actor_ct(Actor* thisx, PlayState* play);
void En_Bom_actor_dt(Actor* thisx, PlayState* play);
void En_Bom_actor_move(Actor* thisx, PlayState* play2);
void En_Bom_actor_draw(Actor* thisx, PlayState* play);

static void move_wait(EnBom* this, PlayState* play);
static void move_carry(EnBom* this, PlayState* play);

ActorProfile En_Bom_Profile = {
    /**/ ACTOR_EN_BOM,
    /**/ ACTORCAT_EXPLOSIVE,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnBom),
    /**/ En_Bom_actor_ct,
    /**/ En_Bom_actor_dt,
    /**/ En_Bom_actor_move,
    /**/ En_Bom_actor_draw,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x0003F828, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 6, 11, 14, { 0, 0, 0 } },
};

static ColliderJntSphElementInit JntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000008, 0x00, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 0 }, 100 },
    },
};

static ColliderJntSphInit JntSphData = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ALL,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    JntSphElemData,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F(scale, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -4000, ICHAIN_STOP),
};

void En_Bom_actor_set_process(EnBom* this, EnBomActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Bom_actor_ct(Actor* thisx, PlayState* play) {
    EnBom* this = (EnBom*)thisx;

    ValueSet_process(thisx, value_init);
    Shape_Info_init(&thisx->shape, 700.0f, Actor_shadow_circle, 16.0f);
    thisx->colChkInfo.mass = 200;
    thisx->colChkInfo.cylRadius = 5;
    thisx->colChkInfo.cylHeight = 10;
    this->timer = 70;
    this->flashSpeedScale = 7;
    ClObjPipe_ct(play, &this->bombCollider);
    ClObjJntSph_ct(play, &this->explosionCollider);
    ClObjPipe_set5(play, &this->bombCollider, thisx, &OcInfoData);
    ClObjJntSph_set5_nzm(play, &this->explosionCollider, thisx, &JntSphData, &this->explosionColliderItems[0]);
    this->explosionColliderItems[0].base.atDmgInfo.damage += (thisx->shape.rot.z & 0xFF00) >> 8;

    thisx->shape.rot.z &= 0xFF;
    if (thisx->shape.rot.z & 0x80) {
        thisx->shape.rot.z |= 0xFF00;
    }

    En_Bom_actor_set_process(this, move_wait);
}

void En_Bom_actor_dt(Actor* thisx, PlayState* play) {
    EnBom* this = (EnBom*)thisx;

    ClObjJntSph_dt_nzf(play, &this->explosionCollider);
    ClObjPipe_dt(play, &this->bombCollider);
}

static void move_wait(EnBom* this, PlayState* play) {
    // if bomb has a parent actor, the bomb hasnt been released yet
    if (Actor_carry_check(&this->actor, play)) {
        En_Bom_actor_set_process(this, move_carry);
        this->actor.room = -1;
        return;
    }

    if ((this->actor.velocity.y > 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING)) {
        this->actor.velocity.y = -this->actor.velocity.y;
    }

    // rebound bomb off the wall it hits
    if ((this->actor.speed != 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL)) {
        s16 yawDiff = this->actor.wallYaw - this->actor.world.rot.y;
        if (ABS(yawDiff) > 0x4000) {
            this->actor.world.rot.y = ((this->actor.wallYaw - this->actor.world.rot.y) + this->actor.wallYaw) - 0x8000;
        }
        Actor_SE_set(&this->actor, NA_SE_EV_BOMB_BOUND);
        Actor_position_moveF(&this->actor);
        this->actor.speed *= 0.7f;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
    }

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        chase_f(&this->actor.speed, 0.0f, 0.08f);
    } else {
        chase_f(&this->actor.speed, 0.0f, 1.0f);
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) && (this->actor.velocity.y < -3.0f)) {
            Actor_bound_SE_set(play, &this->actor);
            this->actor.velocity.y *= -0.3f;
            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND_TOUCH;
        } else if (this->timer >= 4) {
            Actor_carry_request(&this->actor, play);
        }
    }

    Actor_position_moveF(&this->actor);
}

static void move_carry(EnBom* this, PlayState* play) {
    // if parent is NULL bomb has been released
    if (Actor_carry_end_check(&this->actor, play)) {
        En_Bom_actor_set_process(this, move_wait);
        move_wait(this, play);
    }
}

void del_wait(EnBom* this, PlayState* play) {
    Player* player;

    if (this->explosionCollider.elements[0].dim.modelSphere.radius == 0) {
        this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
        z_vibctl2_vib_setQ(this->actor.xzDistToPlayer, 255, 20, 150);
    }

    this->explosionCollider.elements[0].dim.worldSphere.radius += this->actor.shape.rot.z + 8;

    if (this->actor.params == BOMB_EXPLOSION) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->explosionCollider.base);
    }

    if (play->envCtx.adjLight1Color[0] != 0) {
        play->envCtx.adjLight1Color[0] -= 25;
    }

    if (play->envCtx.adjLight1Color[1] != 0) {
        play->envCtx.adjLight1Color[1] -= 25;
    }

    if (play->envCtx.adjLight1Color[2] != 0) {
        play->envCtx.adjLight1Color[2] -= 25;
    }

    if (play->envCtx.adjAmbientColor[0] != 0) {
        play->envCtx.adjAmbientColor[0] -= 25;
    }

    if (play->envCtx.adjAmbientColor[1] != 0) {
        play->envCtx.adjAmbientColor[1] -= 25;
    }

    if (play->envCtx.adjAmbientColor[2] != 0) {
        play->envCtx.adjAmbientColor[2] -= 25;
    }

    if (this->timer == 0) {
        player = GET_PLAYER(play);

        if ((player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) && (player->heldActor == &this->actor)) {
            player->actor.child = NULL;
            player->heldActor = NULL;
            player->interactRangeActor = NULL;
            player->stateFlags1 &= ~PLAYER_STATE1_CARRYING_ACTOR;
        }

        Actor_delete(&this->actor);
    }
}

void En_Bom_actor_move(Actor* thisx, PlayState* play2) {
    Vec3f effVelocity = { 0.0f, 0.0f, 0.0f };
    Vec3f bomb2Accel = { 0.0f, 0.1f, 0.0f };
    Vec3f effAccel = { 0.0f, 0.0f, 0.0f };
    Vec3f effPos;
    Vec3f dustAccel = { 0.0f, 0.6f, 0.0f };
    Color_RGBA8 dustColor = { 255, 255, 255, 255 };
    s32 pad;
    PlayState* play = play2;
    EnBom* this = (EnBom*)thisx;

    thisx->gravity = -1.2f;

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 67) {
        Actor_SE_set(thisx, NA_SE_PL_TAKE_OUT_SHIELD);
        Actor_set_scale(thisx, 0.01f);
    }

    if ((thisx->xzDistToPlayer >= 20.0f) || (ABS(thisx->yDistToPlayer) >= 80.0f)) {
        this->colliderSetOC = true;
    }

    this->actionFunc(this, play);

    Actor_BGcheck2(play, thisx, 5.0f, 10.0f, 15.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 |
                                UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);

    if (thisx->params == BOMB_BODY) {
        if (this->timer < 63) {
            dustAccel.y = 0.2f;

            // spawn spark effect on even frames
            effPos = thisx->world.pos;
            effPos.y += 17.0f;
            if ((play->gameplayFrames % 2) == 0) {
                Effect_SS_Spk_ct_direct(play, thisx, &effPos, &effVelocity, &effAccel);
            }

            Actor_SE_set(thisx, NA_SE_IT_BOMB_IGNIT - SFX_FLAG);

            effPos.y += 3.0f;
            Effect_SS_Dust_sc_cl_ct(play, &effPos, &effVelocity, &dustAccel, &dustColor, &dustColor, 50, 5);
        }

        if ((this->bombCollider.base.acFlags & AC_HIT) || ((this->bombCollider.base.ocFlags1 & OC1_HIT) &&
                                                           (this->bombCollider.base.oc->category == ACTORCAT_ENEMY))) {
            this->timer = 0;
            thisx->shape.rot.z = 0;
        } else {
            // if a lit stick touches the bomb, set timer to 100
            // these bombs never have a timer over 70, so this isn't used
            if ((this->timer > 100) && player_fire_stick_hit_check(play, &thisx->world.pos, 30.0f, 50.0f)) {
                this->timer = 100;
            }
        }

        dustAccel.y = 0.2f;
        effPos = thisx->world.pos;
        effPos.y += 10.0f;

        // double bomb flash speed and adjust red color at certain times during the countdown
        if ((this->timer == 3) || (this->timer == 20) || (this->timer == 40)) {
            thisx->shape.rot.z = 0;
            this->flashSpeedScale >>= 1;
        }

        if ((this->timer < 100) && ((this->timer & (this->flashSpeedScale + 1)) != 0)) {
            add_calc(&this->flashIntensity, 140.0f, 1.0f, 140.0f / this->flashSpeedScale, 0.0f);
        } else {
            add_calc(&this->flashIntensity, 0.0f, 1.0f, 140.0f / this->flashSpeedScale, 0.0f);
        }

        if (this->timer < 3) {
            Actor_set_scale(thisx, thisx->scale.x + 0.002f);
        }

        if (this->timer == 0) {
            effPos = thisx->world.pos;

            effPos.y += 10.0f;
            if (Actor_carry_check(thisx, play)) {
                effPos.y += 30.0f;
            }

            Effect_SS_Bomb2_2_ct(play, &effPos, &effVelocity, &bomb2Accel, 100, (thisx->shape.rot.z * 6) + 19);

            effPos.y = thisx->floorHeight;
            if (thisx->floorHeight > BGCHECK_Y_MIN) {
                Effect_SS_Blast_ct_direct(play, &effPos, &effVelocity, &effAccel);
            }

            Actor_SE_set(thisx, NA_SE_IT_BOMB_EXPLOSION);

            play->envCtx.adjLight1Color[0] = play->envCtx.adjLight1Color[1] = play->envCtx.adjLight1Color[2] = 250;

            play->envCtx.adjAmbientColor[0] = play->envCtx.adjAmbientColor[1] = play->envCtx.adjAmbientColor[2] = 250;

            setDamageCamera(&play->mainCamera, 2, 11, 8);
            thisx->params = BOMB_EXPLOSION;
            this->timer = 10;
            thisx->flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
            En_Bom_actor_set_process(this, del_wait);
        }
    }

    Actor_world_to_eye(thisx, 20.0f);

    if (thisx->params <= BOMB_BODY) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->bombCollider);

        // if link is not holding the bomb anymore and conditions for OC are met, subscribe to OC
        if (!Actor_carry_check(thisx, play) && this->colliderSetOC) {
            CollisionCheck_setOC(play, &play->colChkCtx, &this->bombCollider.base);
        }

        CollisionCheck_setAC(play, &play->colChkCtx, &this->bombCollider.base);
    }

    if ((thisx->scale.x >= 0.01f) && (thisx->params != BOMB_EXPLOSION)) {
        if (thisx->depthInWater >= 20.0f) {
            Effect_sound_ct(play, &thisx->projectedPos, NA_SE_IT_BOMB_UNEXPLOSION, true,
                                              DEADSOUND_REPEAT_MODE_OFF, 10);
            Actor_delete(thisx);
            return;
        }
        if (thisx->bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
            thisx->bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
            Actor_SE_set(thisx, NA_SE_EV_BOMB_DROP_WATER);
        }
    }
}

void En_Bom_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBom* this = (EnBom*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_bom.c", 913);

    if (thisx->params == BOMB_BODY) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Actor_HiliteReflect_set_init(thisx, play, 0);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_bom.c", 928);
        gSPDisplayList(POLY_OPA_DISP++, gBombCapDL);
        Matrix_rotateXYZ(0x4000, 0, 0, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_bom.c", 934);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, (s16)this->flashIntensity, 0, 40, 255);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, (s16)this->flashIntensity, 0, 40, 255);
        gSPDisplayList(POLY_OPA_DISP++, gBombBodyDL);
        CollisionCheck_Uty_convJntSphL2G(0, &this->explosionCollider);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_bom.c", 951);
}
