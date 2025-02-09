/*
 * File: z_en_vb_ball.c
 * Overlay: ovl_En_Vb_Ball
 * Description: Volvagia's rocks and bones
 */

#include "z_en_vb_ball.h"
#include "assets/objects/object_fd/object_fd.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Boss_Fd/z_boss_fd.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Vb_Ball_actor_ct(Actor* thisx, PlayState* play);
void En_Vb_Ball_actor_dt(Actor* thisx, PlayState* play);
void En_Vb_Ball_actor_move(Actor* thisx, PlayState* play2);
void En_Vb_Ball_actor_draw(Actor* thisx, PlayState* play);

ActorProfile En_Vb_Ball_Profile = {
    /**/ 0,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_FD,
    /**/ sizeof(EnVbBall),
    /**/ En_Vb_Ball_actor_ct,
    /**/ En_Vb_Ball_actor_dt,
    /**/ En_Vb_Ball_actor_move,
    /**/ En_Vb_Ball_actor_draw,
};

static ColliderCylinderInit Vb_BallOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK6,
        { 0x00100700, 0x00, 0x20 },
        { 0x00100700, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 30, 10, { 0, 0, 0 } },
};

void En_Vb_Ball_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnVbBall* this = (EnVbBall*)thisx;
    s32 pad2;
    f32 angle;

    if (this->actor.params >= 200) { // Volvagia's bones
        this->yRotVel = rnd_fx(0x300);
        this->xRotVel = rnd_fx(0x300);
        angle = fatan2(this->actor.world.pos.x, this->actor.world.pos.z);
        this->actor.velocity.y = rnd_f(3.0f);
        this->actor.velocity.x = 2.0f * sinf(angle);
        this->actor.velocity.z = 2.0f * cosf(angle);
        this->actor.gravity = -0.8f;
    } else { // Volvagia's rocks
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &Vb_BallOcInfoData);
        Actor_set_scale(&this->actor, this->actor.world.rot.z / 10000.0f);
        this->collider.dim.radius = this->actor.scale.y * 3000.0f;
        this->collider.dim.height = this->actor.scale.y * 5000.0f;
        this->collider.dim.yShift = this->actor.scale.y * -2500.0f;
        this->xRotVel = rnd_fx(0x2000);
        this->yRotVel = rnd_fx(0x2000);
        this->shadowSize = this->actor.scale.y * 68.0f;
    }
}

void En_Vb_Ball_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnVbBall* this = (EnVbBall*)thisx;

    if (this->actor.params < 200) {
        ClObjPipe_dt(play, &this->collider);
    }
}

void Effect_Hahen_ct_IN_ball(PlayState* play, BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration,
                          f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DEBRIS;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->vFdFxRotX = rnd_f(100.0f);
            effect->vFdFxRotY = rnd_f(100.0f);
            break;
        }
    }
}

void Effect_vb_smoke_ct_IN_ball(PlayState* play, BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration,
                        f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DUST;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->timer2 = 0;
            effect->scale = scale / 400.0f;
            break;
        }
    }
}

void En_Vb_Hone_move(EnVbBall* this, PlayState* play) {
    BossFd* bossFd = (BossFd*)this->actor.parent;
    f32 pad2;
    f32 pad1;
    f32 angle;
    s16 i;

    Actor_BGcheck2(play, &this->actor, 50.0f, 50.0f, 100.0f, UPDBGCHECKINFO_FLAG_2);
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.velocity.y <= 0.0f)) {
        this->xRotVel = rnd_fx((f32)0x4000);
        this->yRotVel = rnd_fx((f32)0x4000);
        angle = fatan2(this->actor.world.pos.x, this->actor.world.pos.z);
        this->actor.velocity.x = sinf(angle) * 10.0f;
        this->actor.velocity.z = cosf(angle) * 10.0f;
        this->actor.velocity.y *= -0.5f;
        if (PARAMS_GET_U(this->actor.params, 0, 1)) {
            Nai_FxFlagEntry(NA_SE_EN_VALVAISA_LAND, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
        for (i = 0; i < 10; i++) {
            Vec3f dustVel = { 0.0f, 0.0f, 0.0f };
            Vec3f dustAcc = { 0.0f, 0.0f, 0.0f };
            Vec3f dustPos;

            dustVel.x = rnd_fx(8.0f);
            dustVel.y = rnd_f(1.0f);
            dustVel.z = rnd_fx(8.0f);

            dustAcc.y = 0.3f;

            dustPos.x = rnd_fx(20.0f) + this->actor.world.pos.x;
            dustPos.y = this->actor.floorHeight + 10.0f;
            dustPos.z = rnd_fx(20.0f) + this->actor.world.pos.z;

            Effect_vb_smoke_ct_IN_ball(play, bossFd->effects, &dustPos, &dustVel, &dustAcc, rnd_f(80.0f) + 200.0f);
        }
    }
    if (this->actor.world.pos.y < 50.0f) {
        Actor_delete(&this->actor);
    }
}

void En_Vb_Ball_actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnVbBall* this = (EnVbBall*)thisx;
    BossFd* bossFd = (BossFd*)this->actor.parent;
    f32 radius;
    f32 pad2;
    s16 spawnNum;
    s16 i;

    this->unkTimer2++;
    if (this->unkTimer1 != 0) {
        this->unkTimer1--;
    }
    this->actor.shape.rot.x += (s16)this->xRotVel;
    this->actor.shape.rot.y += (s16)this->yRotVel;
    this->actor.velocity.y += -1.0f;
    this->actor.gravity = -1.0f;
    Actor_position_move(&this->actor);
    if (this->actor.params >= 200) {
        En_Vb_Hone_move(this, play);
    } else {
        add_calc2(&this->shadowOpacity, 175.0f, 1.0f, 40.0f);
        radius = this->actor.scale.y * 1700.0f;
        this->actor.world.pos.y -= radius;
        Actor_BGcheck2(play, &this->actor, 50.0f, 50.0f, 100.0f, UPDBGCHECKINFO_FLAG_2);
        this->actor.world.pos.y += radius;
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.velocity.y <= 0.0f)) {
            if ((this->actor.params == 100) || (this->actor.params == 101)) {
                Actor_delete(&this->actor);
                if (this->actor.params == 100) {
                    ActorQuakeRumbleSet(&this->actor, play, 5, 10);
                }
                if (this->actor.params == 100) {
                    spawnNum = 2;
                } else {
                    spawnNum = 2;
                }
                for (i = 0; i < spawnNum; i++) {
                    Vec3f spawnOffset;
                    EnVbBall* newActor;
                    f32 xRotVel;

                    if (this->actor.params == 100) {
                        spawnOffset.x = rnd_fx(13.0f);
                        spawnOffset.y = rnd_f(5.0f) + 6.0f;
                        spawnOffset.z = rnd_fx(13);
                    } else {
                        spawnOffset.x = rnd_fx(10.0f);
                        spawnOffset.y = rnd_f(3.0f) + 4.0f;
                        spawnOffset.z = rnd_fx(10.0f);
                    }
                    newActor = (EnVbBall*)Actor_info_make_child_actor(
                        &play->actorCtx, &this->actor, play, ACTOR_EN_VB_BALL, this->actor.world.pos.x + spawnOffset.x,
                        this->actor.world.pos.y + spawnOffset.y, this->actor.world.pos.z + spawnOffset.z, 0, 0,
                        this->actor.world.rot.z * 0.5f, this->actor.params + 1);
                    if (newActor != NULL) {
                        if ((i == 0) && (this->actor.params == 100)) {
                            Nai_FxFlagEntry(NA_SE_EN_VALVAISA_ROCK, &newActor->actor.projectedPos, 4,
                                                 &_dummy_one, &_dummy_one,
                                                 &_dummy_zero_s8);
                        }
                        newActor->actor.parent = this->actor.parent;
                        newActor->actor.velocity = spawnOffset;
                        newActor->yRotVel = 0.0f;
                        xRotVel = sqrtf(SQ(spawnOffset.x) + SQ(spawnOffset.z));
                        newActor->xRotVel = 0x1000 / 10.0f * xRotVel;
                        newActor->actor.shape.rot.y = RAD_TO_BINANG(fatan2(spawnOffset.x, spawnOffset.z));
                        newActor->shadowOpacity = 200.0f;
                    }
                }
                for (i = 0; i < 15; i++) {
                    Vec3f debrisVel1 = { 0.0f, 0.0f, 0.0f };
                    Vec3f debrisAcc1 = { 0.0f, -1.0f, 0.0f };
                    Vec3f debrisPos1;

                    debrisVel1.x = rnd_fx(25.0f);
                    debrisVel1.y = rnd_f(5.0f) + 8;
                    debrisVel1.z = rnd_fx(25.0f);

                    debrisPos1.x = rnd_fx(10.0f) + this->actor.world.pos.x;
                    debrisPos1.y = rnd_fx(10.0f) + this->actor.world.pos.y;
                    debrisPos1.z = rnd_fx(10.0f) + this->actor.world.pos.z;

                    Effect_Hahen_ct_IN_ball(play, bossFd->effects, &debrisPos1, &debrisVel1, &debrisAcc1,
                                         (s16)rnd_f(12.0f) + 15);
                }
                for (i = 0; i < 10; i++) {
                    Vec3f dustVel = { 0.0f, 0.0f, 0.0f };
                    Vec3f dustAcc = { 0.0f, 0.0f, 0.0f };
                    Vec3f dustPos;

                    dustVel.x = rnd_fx(8.0f);
                    dustVel.y = rnd_f(1.0f);
                    dustVel.z = rnd_fx(8.0f);

                    dustAcc.y = 1.0f / 2;

                    dustPos.x = rnd_fx(30.0f) + this->actor.world.pos.x;
                    dustPos.y = rnd_fx(30.0f) + this->actor.world.pos.y;
                    dustPos.z = rnd_fx(30.0f) + this->actor.world.pos.z;

                    Effect_vb_smoke_ct_IN_ball(play, bossFd->effects, &dustPos, &dustVel, &dustAcc,
                                       rnd_f(100.0f) + 350.0f);
                }
            } else {
                for (i = 0; i < 5; i++) {
                    Vec3f debrisVel2 = { 0.0f, 0.0f, 0.0f };
                    Vec3f debrisAcc2 = { 0.0f, -1.0f, 0.0f };
                    Vec3f debrisPos2;

                    debrisVel2.x = rnd_fx(10.0f);
                    debrisVel2.y = rnd_f(3.0f) + 3.0f;
                    debrisVel2.z = rnd_fx(10.0f);

                    debrisPos2.x = rnd_fx(5.0f) + this->actor.world.pos.x;
                    debrisPos2.y = rnd_fx(5.0f) + this->actor.world.pos.y;
                    debrisPos2.z = rnd_fx(5.0f) + this->actor.world.pos.z;

                    Effect_Hahen_ct_IN_ball(play, bossFd->effects, &debrisPos2, &debrisVel2, &debrisAcc2,
                                         (s16)rnd_f(12.0f) + 15);
                }
                Actor_delete(&this->actor);
            }
        }
        if (this->collider.base.atFlags & AT_HIT) {
            Player* player = GET_PLAYER(play);

            this->collider.base.atFlags &= ~AT_HIT;
            Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
        }
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }
}

void En_Vb_Ball_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnVbBall* this = (EnVbBall*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_vb_ball.c", 604);
    if (1) {} // needed for match
    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_vb_ball.c", 607);

    if (this->actor.params >= 200) {
        gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gVolvagiaRibsDL));
    } else {
        gSPDisplayList(POLY_OPA_DISP++, SEGMENTED_TO_VIRTUAL(gVolvagiaRockDL));
        _texture_decal_shadow(play->state.gfxCtx);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, (s8)this->shadowOpacity);
        Matrix_translate(this->actor.world.pos.x, 100.0f, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_scale(this->shadowSize, 1.0f, this->shadowSize, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_vb_ball.c", 626);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gCircleShadowDL));
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_vb_ball.c", 632);
}
