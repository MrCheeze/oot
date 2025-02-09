/*
 * File: z_bg_vb_sima.c
 * Overlay: ovl_Bg_Vb_Sima
 * Description: Volvagia's platform
 */

#include "z_bg_vb_sima.h"
#include "assets/objects/object_fd/object_fd.h"
#include "overlays/actors/ovl_Boss_Fd/z_boss_fd.h"

#define FLAGS 0

void Bg_Vb_Sima_actor_ct(Actor* thisx, PlayState* play);
void Bg_Vb_Sima_actor_dt(Actor* thisx, PlayState* play);
void Bg_Vb_Sima_actor_move(Actor* thisx, PlayState* play);
void Bg_Vb_Sima_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Bg_Vb_Sima_Profile = {
    /**/ ACTOR_BG_VB_SIMA,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_FD,
    /**/ sizeof(BgVbSima),
    /**/ Bg_Vb_Sima_actor_ct,
    /**/ Bg_Vb_Sima_actor_dt,
    /**/ Bg_Vb_Sima_actor_move,
    /**/ Bg_Vb_Sima_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Vb_Sima_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgVbSima* this = (BgVbSima*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyUty_bgdi_SG2KSG(&gVolvagiaPlatformCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void Bg_Vb_Sima_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgVbSima* this = (BgVbSima*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Effect_vb_hinoko_ct_IN_sima(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_EMBER;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->alpha = 255;
            effect->timer1 = (s16)rnd_f(10.0f);
            break;
        }
    }
}

void Bg_Vb_Sima_actor_move(Actor* thisx, PlayState* play) {
    static Color_RGBA8 prim = { 255, 255, 0, 255 };
    static Color_RGBA8 env = { 255, 10, 0, 255 };
    s32 pad;
    BgVbSima* this = (BgVbSima*)thisx;
    BossFd* bossFd = (BossFd*)this->dyna.actor.parent;
    f32 minus1 = -1.0f;

    this->shakeTimer++;
    if (!Actor_Environment_room_clear_Check(play, play->roomCtx.curRoom.num)) {
        s32 signal = bossFd->platformSignal;

        if (signal == VBSIMA_COLLAPSE) {
            add_calc(&this->dyna.actor.world.pos.y, -1000.0f, 1.0f, 1.5f, 0.0f);
            this->dyna.actor.world.pos.z += 2.0f * cos_s(this->shakeTimer * 0x8000);
            this->dyna.actor.shape.rot.x = (s16)sin_s(this->shakeTimer * 0x7000) * 0x37;
            this->dyna.actor.shape.rot.z = (s16)sin_s(this->shakeTimer * 0x5000) * 0x37;
            Nai_FxFlagEntry(NA_SE_EV_BLOCKSINK - SFX_FLAG, &this->dyna.actor.projectedPos, 4,
                                 &_dummy_one, &_dummy_one, &_dummy_zero_s8);
        } else if (signal == VBSIMA_KILL) {
            Actor_delete(&this->dyna.actor);
        }
        if (bossFd->platformSignal != VBSIMA_STAND) {
            s16 i2;
            s16 i1;
            Vec3f splashVel;
            Vec3f splashAcc;
            Vec3f splashPos;
            Vec3f emberPos;
            Vec3f emberVel;
            Vec3f emberAcc;
            f32 edgeX;
            f32 edgeZ;

            for (i1 = 0; i1 < 10; i1++) {
                if (fqrand() < 0.33f) {
                    edgeX = -80.0f;
                    edgeZ = rnd_fx(160.0f);
                } else {
                    edgeZ = 80.0f;
                    if (fqrand() < 0.5f) {
                        edgeZ = 80.0f * minus1;
                    }
                    edgeX = rnd_fx(160.0f);
                }

                splashVel.x = edgeX * 0.05f;
                splashVel.y = rnd_f(3.0f) + 3.0f;
                splashVel.z = edgeZ * 0.05f;

                splashAcc.y = -0.3f;
                splashAcc.x = splashVel.x;
                splashAcc.z = splashVel.z;

                splashPos.x = this->dyna.actor.world.pos.x + edgeX;
                splashPos.y = -80.0f;
                splashPos.z = this->dyna.actor.world.pos.z + edgeZ;

                Effect_SS_Dust_sc_cl_co_ct(play, &splashPos, &splashVel, &splashAcc, &prim, &env,
                              (s16)rnd_f(100.0f) + 500, 10, 20);

                for (i2 = 0; i2 < 3; i2++) {
                    emberVel.x = splashVel.x;
                    emberVel.y = rnd_f(5.0f);
                    emberVel.z = splashVel.z;

                    emberAcc.y = 0.4f;
                    emberAcc.x = rnd_fx(0.5f);
                    emberAcc.z = rnd_fx(0.5f);

                    emberPos.x = rnd_fx(60.0f) + splashPos.x;
                    emberPos.y = rnd_f(40.0f) + splashPos.y;
                    emberPos.z = rnd_fx(60.0f) + splashPos.z;

                    Effect_vb_hinoko_ct_IN_sima(bossFd->effects, &emberPos, &emberVel, &emberAcc,
                                        (s16)rnd_f(2.0f) + 8);
                }
            }
        }
    }
}

void Bg_Vb_Sima_actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_vb_sima.c", 285);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_vb_sima.c", 291);
    gSPDisplayList(POLY_OPA_DISP++, gVolvagiaPlatformDL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_vb_sima.c", 296);
}
