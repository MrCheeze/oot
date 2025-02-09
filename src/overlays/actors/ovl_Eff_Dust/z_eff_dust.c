/**
 * File z_eff_dust.c
 * Overlay: ovl_Eff_Dust
 * Description: Dust effects
 */

#include "z_eff_dust.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Eff_Dust_Actor_ct(Actor* thisx, PlayState* play);
void Eff_Dust_Actor_dt(Actor* thisx, PlayState* play);
void Eff_Dust_Actor_move(Actor* thisx, PlayState* play);
void Eff_Dust_Actor_draw(Actor* thisx, PlayState* play);

void dust_init(EffDust* this);

void move_grip(EffDust* this, PlayState* play);
void move_whirlpool(EffDust* this, PlayState* play);
void move_sword(EffDust* this, PlayState* play);
void draw_grip(Actor* thisx, PlayState* play2);
void draw_sword(Actor* thisx, PlayState* play2);

ActorProfile Eff_Dust_Profile = {
    /**/ ACTOR_EFF_DUST,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EffDust),
    /**/ Eff_Dust_Actor_ct,
    /**/ Eff_Dust_Actor_dt,
    /**/ Eff_Dust_Actor_move,
    /**/ Eff_Dust_Actor_draw,
};

static Gfx static_null[] = {
    gsSPEndDisplayList(),
};

void Eff_Dust_actor_set_process(EffDust* this, EffDustActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Eff_Dust_actor_set_drawproc(EffDust* this, EffDustDrawFunc drawFunc) {
    this->drawFunc = drawFunc;
}

void dust_init(EffDust* this) {
    s32 i;

    for (i = 0; i < 64; i++) {
        this->initialPositions[i].z = 0.0f;
        this->initialPositions[i].y = 0.0f;
        this->initialPositions[i].x = 0.0f;

        this->distanceTraveled[i] = 1.0f;
    }
    this->index = 0;
}

void Eff_Dust_Actor_ct(Actor* thisx, PlayState* play) {
    EffDust* this = (EffDust*)thisx;
    EffDustType dustEffect = this->actor.params;

    dust_init(this);

    switch (dustEffect) {
        case EFF_DUST_TYPE_0:
            Eff_Dust_actor_set_process(this, move_grip);
            Eff_Dust_actor_set_drawproc(this, draw_grip);
            this->dy = 0.8f;
            this->dz = 0.8f;
            this->dx = 1.0f;
            this->scalingFactor = 0.1f;
            break;
        case EFF_DUST_TYPE_1:
            Eff_Dust_actor_set_process(this, move_whirlpool);
            Eff_Dust_actor_set_drawproc(this, draw_grip);
            this->dx = 0.8f;
            this->dz = 0.8f;
            this->dy = 1.0f;
            this->scalingFactor = 0.5f;
            break;
        case EFF_DUST_TYPE_2:
            Eff_Dust_actor_set_process(this, move_sword);
            Eff_Dust_actor_set_drawproc(this, draw_sword);
            this->dx = 0.5f;
            this->scalingFactor = 15.0f;
            break;
        case EFF_DUST_TYPE_3:
            Eff_Dust_actor_set_process(this, move_sword);
            Eff_Dust_actor_set_drawproc(this, draw_sword);
            this->dx = 0.5f;
            this->scalingFactor = 10.0f;
            break;
        case EFF_DUST_TYPE_4:
            Eff_Dust_actor_set_process(this, move_sword);
            Eff_Dust_actor_set_drawproc(this, draw_sword);
            this->actor.room = -1;
            this->dx = 0.5f;
            this->scalingFactor = 20.0f;
            break;
        default:
            //! @bug Actor_delete should be used, not free.
            //! Note this also frees with a function for the wrong memory arena.
            SYSTEM_ARENA_FREE(this, "../z_eff_dust.c", 202);
            break;
    }

    this->life = 10;
}

void Eff_Dust_Actor_dt(Actor* thisx, PlayState* play) {
}

void move_grip(EffDust* this, PlayState* play) {
    s16 theta;
    s16 fi;
    f32* distanceTraveled = this->distanceTraveled;
    s32 i;
    s32 j;

    for (i = 0; i < 64; i++) {
        if ((*distanceTraveled) < 1.0f) {
            *distanceTraveled += 0.05f;
        }
        distanceTraveled++;
    }

    for (j = 0; j < 3; j++) {
        i = this->index & 0x3F;
        if (this->distanceTraveled[i] >= 1.0f) {
            // Spherical coordinate system.
            fi = rnd_fx(8192.0f);
            theta = rnd_fx(4096.0f);
            this->initialPositions[i].x = -800.0f * cos_s(fi) * cos_s(theta);
            this->initialPositions[i].y = -800.0f * sin_s(theta);
            this->initialPositions[i].z = -800.0f * sin_s(fi) * cos_s(theta);
            this->distanceTraveled[i] = 0.0f;
            this->index++;
        }
    }
}

void move_whirlpool(EffDust* this, PlayState* play) {
    s16 theta;
    s16 fi;
    f32* distanceTraveled = this->distanceTraveled;
    s32 i;
    s32 j;

    for (i = 0; i < 64; i++) {
        if ((*distanceTraveled) < 1.0f) {
            *distanceTraveled += 0.03f;
        }
        distanceTraveled++;
    }

    for (j = 0; j < 2; j++) {
        i = this->index & 0x3F;
        if (this->distanceTraveled[i] >= 1.0f) {
            // Spherical coordinate system.
            fi = rnd_fx(65536.0f);
            theta = rnd_f(8192.0f);
            this->initialPositions[i].x = 400.0f * cos_s(fi) * cos_s(theta);
            this->initialPositions[i].y = 400.0f * sin_s(theta);
            this->initialPositions[i].z = 400.0f * sin_s(fi) * cos_s(theta);
            this->distanceTraveled[i] = 0.0f;
            this->index++;
        }
    }
}

void move_sword(EffDust* this, PlayState* play) {
    s16 theta;
    Player* player = GET_PLAYER(play);
    Actor* parent = this->actor.parent;
    f32* distanceTraveled = this->distanceTraveled;
    s32 i;
    s32 j;

    if (parent == NULL || parent->update == NULL || !(player->stateFlags1 & PLAYER_STATE1_CHARGING_SPIN_ATTACK)) {
        if (this->life != 0) {
            this->life--;
        } else {
            Actor_delete(&this->actor);
        }

        for (i = 0; i < 64; i++) {
            if ((*distanceTraveled) < 1.0f) {
                *distanceTraveled += 0.2f;
            }
            distanceTraveled++;
        }

        return;
    }

    for (i = 0; i < 64; i++) {
        if ((*distanceTraveled) < 1.0f) {
            *distanceTraveled += 0.1f;
        }
        distanceTraveled++;
    }

    this->actor.world.pos = parent->world.pos;

    for (j = 0; j < 3; j++) {
        i = this->index & 0x3F;
        if (this->distanceTraveled[i] >= 1.0f) {

            theta = rnd_fx(65536.0f);
            switch (this->actor.params) {
                case EFF_DUST_TYPE_2:
                    this->initialPositions[i].x = (fqrand() * 4500.0f) + 700.0f;
                    if (this->initialPositions[i].x > 3000.0f) {
                        this->initialPositions[i].y = (3000.0f * fqrand()) * sin_s(theta);
                        this->initialPositions[i].z = (3000.0f * fqrand()) * cos_s(theta);
                    } else {
                        this->initialPositions[i].y = 3000.0f * sin_s(theta);
                        this->initialPositions[i].z = 3000.0f * cos_s(theta);
                    }
                    break;

                case EFF_DUST_TYPE_3:
                    this->initialPositions[i].x = (fqrand() * 2500.0f) + 700.0f;
                    if (this->initialPositions[i].x > 2000.0f) {
                        this->initialPositions[i].y = (2000.0f * fqrand()) * sin_s(theta);
                        this->initialPositions[i].z = (2000.0f * fqrand()) * cos_s(theta);
                    } else {
                        this->initialPositions[i].y = 2000.0f * sin_s(theta);
                        this->initialPositions[i].z = 2000.0f * cos_s(theta);
                    }
                    break;

                case EFF_DUST_TYPE_4:
                    this->initialPositions[i].x = (fqrand() * 8500.0f) + 1700.0f;
                    if (this->initialPositions[i].x > 5000.0f) {
                        this->initialPositions[i].y = (4000.0f * fqrand()) * sin_s(theta);
                        this->initialPositions[i].z = (4000.0f * fqrand()) * cos_s(theta);
                    } else {
                        this->initialPositions[i].y = 4000.0f * sin_s(theta);
                        this->initialPositions[i].z = 4000.0f * cos_s(theta);
                    }

                    break;
                default:
                    break;
            }

            this->distanceTraveled[i] = 0.0f;
            this->index++;
        }
    }
}

void Eff_Dust_Actor_move(Actor* thisx, PlayState* play) {
    EffDust* this = (EffDust*)thisx;

    this->actionFunc(this, play);
}

void draw_grip(Actor* thisx, PlayState* play2) {
    EffDust* this = (EffDust*)thisx;
    PlayState* play = play2;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    Vec3f* initialPositions;
    f32* distanceTraveled;
    s32 i;
    f32 aux;

    OPEN_DISPS(gfxCtx, "../z_eff_dust.c", 425);

    _texture_z_light_fog_prim(gfxCtx);

    gDPPipeSync(POLY_XLU_DISP++);

    initialPositions = this->initialPositions;
    distanceTraveled = this->distanceTraveled;

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 128, 128, 128, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 128, 128, 128, 0);
    gSPSegment(POLY_XLU_DISP++, 0x08, static_null);

    for (i = 0; i < 64; i++) {
        if (*distanceTraveled < 1.0f) {
            aux = 1.0f - SQ(*distanceTraveled);
            Matrix_translate(this->actor.world.pos.x + (initialPositions->x * ((this->dx * aux) + (1.0f - this->dx))),
                             this->actor.world.pos.y + (initialPositions->y * ((this->dy * aux) + (1.0f - this->dy))),
                             this->actor.world.pos.z + (initialPositions->z * ((this->dz * aux) + (1.0f - this->dz))),
                             MTXMODE_NEW);

            Matrix_scale(this->scalingFactor, this->scalingFactor, this->scalingFactor, MTXMODE_APPLY);
            Matrix_mult(&play->billboardMtxF, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_eff_dust.c", 449);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gEffSparklesDL));
        }

        initialPositions++;
        distanceTraveled++;
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_dust.c", 458);
}

void draw_sword(Actor* thisx, PlayState* play2) {
    EffDust* this = (EffDust*)thisx;
    PlayState* play = play2;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    f32* distanceTraveled;
    Vec3f* initialPositions;
    s32 i;
    f32 aux;
    Player* player = GET_PLAYER(play);

    OPEN_DISPS(gfxCtx, "../z_eff_dust.c", 472);

    _texture_z_light_fog_prim(gfxCtx);

    gDPPipeSync(POLY_XLU_DISP++);

    initialPositions = this->initialPositions;
    distanceTraveled = this->distanceTraveled;

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    if (player->unk_858 >= 0.85f) {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, 0);
    }

    gSPSegment(POLY_XLU_DISP++, 0x08, static_null);

    for (i = 0; i < 64; i++) {
        if (*distanceTraveled < 1.0f) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, *distanceTraveled * 255);

            aux = 1.0f - SQ(*distanceTraveled);

            Matrix_mult(&player->mf_9E0, MTXMODE_NEW);

            Matrix_translate(initialPositions->x * ((this->dx * aux) + (1.0f - this->dx)),
                             initialPositions->y * (1.0f - *distanceTraveled) + 320.0f,
                             initialPositions->z * (1.0f - *distanceTraveled) + -20.0f, MTXMODE_APPLY);

            Matrix_scale(*distanceTraveled * this->scalingFactor, *distanceTraveled * this->scalingFactor,
                         *distanceTraveled * this->scalingFactor, MTXMODE_APPLY);

            Matrix_rotate_scale_exchange(&play->billboardMtxF);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_eff_dust.c", 506);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gEffSparklesDL));
        }

        initialPositions++;
        distanceTraveled++;
    }

    CLOSE_DISPS(gfxCtx, "../z_eff_dust.c", 515);
}

void Eff_Dust_Actor_draw(Actor* thisx, PlayState* play) {
    EffDust* this = (EffDust*)thisx;

    this->drawFunc(thisx, play);
}
