/*
 * File: z_en_part.c
 * Overlay: ovl_En_Part
 * Description: Effect spawner for enemies' death
 */

#include "z_en_part.h"
#include "assets/objects/object_tite/object_tite.h"
#include "assets/objects/object_ik/object_ik.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Part_Actor_ct(Actor* thisx, PlayState* play);
void En_Part_Actor_dt(Actor* thisx, PlayState* play);
void En_Part_Actor_move(Actor* thisx, PlayState* play);
void En_Part_Actor_draw(Actor* thisx, PlayState* play);

ActorProfile En_Part_Profile = {
    /**/ ACTOR_EN_PART,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnPart),
    /**/ En_Part_Actor_ct,
    /**/ En_Part_Actor_dt,
    /**/ En_Part_Actor_move,
    /**/ En_Part_Actor_draw,
};

void En_Part_Actor_ct(Actor* thisx, PlayState* play) {
}

void En_Part_Actor_dt(Actor* thisx, PlayState* play) {
}

void En_Part_Actor_drop_init(EnPart* this, PlayState* play) {
    f32 sign = 1.0f;

    this->action = 1;
    this->actor.world.rot.y = fqrand() * 20000.0f;

    switch (this->actor.params) {
        case 0:
            this->actor.velocity.y = 0.0f;
            this->actor.gravity = -0.3f - fqrand() * 0.5f;
            this->rotZSpeed = 0.3f;
            this->timer = 25;
            this->actor.speed = (fqrand() - 0.5f) * 2.0f;
            break;
        case 13:
            this->timer = 400;
            FALLTHROUGH;
        case 12:
            this->actor.speed = rnd_fx(6.0f);
            this->actor.home.pos = this->actor.world.pos;
            this->timer += 60;
            this->actor.velocity.y = fqrand() * 5.0f + 4.0f;
            this->actor.gravity = -0.6f - fqrand() * 0.5f;
            this->rotZSpeed = 0.15f;
            break;
        case 14:
            Effect_En_Fire_ct(play, &this->actor, &this->actor.world.pos, 40, 0x8001, 0, -1);
            FALLTHROUGH;
        case 1:
        case 4:
        case 9:
        case 10:
            this->timer += (s16)(fqrand() * 17.0f) + 5;
            FALLTHROUGH;
        case 2:
            this->actor.velocity.y = fqrand() * 5.0f + 4.0f;
            this->actor.gravity = -0.6f - fqrand() * 0.5f;
            this->rotZSpeed = 0.15f;
            break;
        case 11:
            Effect_En_Fire_ct(play, &this->actor, &this->actor.world.pos, 40, 0x8001, 0, -1);
            FALLTHROUGH;
        case 3:
            this->actor.speed = (fqrand() - 0.5f) * 3.0f;
            this->timer = (s16)(fqrand() * 17.0f) + 10;
            this->actor.velocity.y = fqrand() * 3.0f + 8.0f;
            this->actor.gravity = -0.6f - fqrand() * 0.3f;
            this->rotZSpeed = 0.15f;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            this->actor.world.rot.y = this->actor.parent->shape.rot.y;
            if (this->displayList == object_ik_DL_015380) {
                sign = -1.0f;
            }
            this->actor.velocity.y = 0.0f;
            this->actor.speed = 6.0f * sign;
            this->actor.gravity = -1.2f;
            this->rotZSpeed = 0.15f * sign;
            Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
            this->timer = 18;
            break;
    }
}

void En_Part_Actor_drop_proc(EnPart* this, PlayState* play) {
    s32 i;
    Vec3f pos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if ((this->actor.params == 12) || (this->actor.params == 13)) {
        Actor_BGcheck2(play, &this->actor, 5.0f, 15.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                    UPDBGCHECKINFO_FLAG_4);

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || (this->actor.world.pos.y <= this->actor.floorHeight)) {
            this->action = 4;
            this->actor.speed = 0.0f;
            this->actor.gravity = 0.0f;
            this->actor.velocity.y = 0.0f;
        }

        if ((this->actor.params == 13) && (this->actor.parent != NULL) && (this->actor.parent->update == NULL)) {
            this->actor.parent = NULL;
        }
    } else if (this->timer <= 0) {
        switch (this->actor.params) {
            case 1:
            case 9:
            case 10:
            case 14:
                _Effect_SS_Db_ct(play, &this->actor.world.pos, &zeroVec, &zeroVec,
                                     (s16)(this->actor.scale.y * 100.0f) * 40, 7, 255, 255, 255, 255, 0, 255, 0, 1, 9,
                                     true);
                break;
            case 3:
            case 11:
                _Effect_SS_Db_ct(play, &this->actor.world.pos, &zeroVec, &zeroVec,
                                     (s16)(this->actor.scale.y * 100.0f) * 40, 7, 255, 255, 255, 255, 0, 0, 255, 1, 9,
                                     true);
                break;
            case 4:
                for (i = 7; i >= 0; i--) {
                    pos.x = this->actor.world.pos.x + rnd_fx(60.0f);
                    pos.y = this->actor.world.pos.y + this->actor.shape.yOffset * this->actor.scale.y +
                            rnd_fx(50.0f);
                    pos.z = this->actor.world.pos.z + rnd_fx(60.0f);
                    velocity.y = fqrand() + 1.0f;
                    Effect_SS_Dt_Bubble_sc_co_ct(play, &pos, &velocity, &accel, get_random_timer(80, 100), 25, 0,
                                                       true);
                }
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                for (i = 4; i >= 0; i--) {
                    pos.x = this->actor.world.pos.x + rnd_fx(25.0f);
                    pos.y = this->actor.world.pos.y + rnd_fx(40.0f);
                    pos.z = this->actor.world.pos.z + rnd_fx(25.0f);
                    _Effect_SS_Db_ct(play, &pos, &zeroVec, &zeroVec, 40, 7, 255, 255, 255, 255, 0, 0, 255, 1, 9,
                                         true);
                }
                break;
        }

        Actor_delete(&this->actor);
        return;
    }

    this->timer--;
    this->rotZ += this->rotZSpeed;
}

void En_Part_Actor_shot_init(EnPart* this, PlayState* play) {
    this->action = 3;
}

void En_Part_Actor_shot_proc(EnPart* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->timer--;
    if (this->timer == 0) {
        Actor_delete(&this->actor);
    } else {
        Vec3f velocity = { 0.0f, 8.0f, 0.0f };
        Vec3f accel = { 0.0f, -1.5f, 0.0f };

        if (sqrt(this->actor.xyzDistToPlayerSq) <= 40.0f) {
            u8 prevInvincibilityTimer = player->invincibilityTimer;

            if (player->invincibilityTimer <= 0) {
                if (player->invincibilityTimer <= -40) {
                    player->invincibilityTimer = 0;
                } else {
                    player->invincibilityTimer = 0;
                    play->damagePlayer(play, -8);
                }
            }
            Actor_player_power_damage_set(play, this->actor.parent,
                                                  (650.0f - this->actor.parent->xzDistToPlayer) * 0.04f + 4.0f,
                                                  this->actor.parent->world.rot.y, 8.0f);
            player->invincibilityTimer = prevInvincibilityTimer;
            this->timer = 1;
        }

        dust_fly_set2(play, &this->actor.world.pos, 0.0f, 1, 300, 150, 1);
        velocity.x = rnd_fx(16.0f);
        Effect_Hahen_ct3(play, &this->actor.world.pos, &velocity, &accel, 20,
                            (s32)((fqrand() * 5.0f + 12.0f) * 2), -1, 10, NULL);
        Actor_SE_set(&this->actor, NA_SE_EN_MONBLIN_GNDWAVE - SFX_FLAG);
    }
}

void En_Part_Actor_ret_proc(EnPart* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if ((this->actor.parent == NULL) || (this->actor.parent->update == NULL)) {
        _Effect_SS_Db_ct(play, &this->actor.world.pos, &zeroVec, &zeroVec, (s16)(this->actor.scale.y * 100.0f) * 40,
                             7, 255, 255, 255, 255, 0, 255, 0, 1, 9, true);
        Actor_delete(&this->actor);
        return;
    }

    if (this->timer == 0) {
        f32 diffsSum = add_calc(&this->actor.world.pos.x, this->actor.home.pos.x, 1.0f, 5.0f, 0.0f);

        diffsSum += add_calc(&this->actor.world.pos.y, this->actor.home.pos.y, 1.0f, 5.0f, 0.0f);
        diffsSum += add_calc(&this->actor.world.pos.z, this->actor.home.pos.z, 1.0f, 5.0f, 0.0f);
        diffsSum += add_calc(&this->rotZ, 0.0f, 1.0f, 0.25f, 0.0f);
        if (diffsSum == 0.0f) {
            this->actor.parent->home.rot.x--;
            this->timer--;
            Actor_SE_set(&this->actor, NA_SE_EN_STAL_DAMAGE);
        }
    } else if (this->timer > 0) {
        this->timer--;
    }

    if (this->actor.parent->colChkInfo.health != 0) {
        Actor_delete(&this->actor);
    }
}

void En_Part_Actor_move(Actor* thisx, PlayState* play) {
    static EnPartActionFunc mode_function[] = {
        En_Part_Actor_drop_init, En_Part_Actor_drop_proc, En_Part_Actor_shot_init, En_Part_Actor_shot_proc, En_Part_Actor_ret_proc,
    };

    EnPart* this = (EnPart*)thisx;

    Actor_position_moveF(&this->actor);

    if ((this->actor.params > 4 && this->actor.params < 9) || this->actor.params < 0) {
        Actor_BGcheck2(play, &this->actor, 5.0f, 15.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        if (this->actor.params >= 0) {
            add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
            if (thisx->bgCheckFlags & BGCHECKFLAG_GROUND) {
                thisx->bgCheckFlags &= ~BGCHECKFLAG_GROUND;
                thisx->velocity.y = 6.0f;
            }
        }
    }

    mode_function[this->action](this, play);
}

static Gfx* set_col(GraphicsContext* gfxCtx, u8 primR, u8 primG, u8 primB, u8 envR, u8 envG, u8 envB) {
    Gfx* dList;
    Gfx* dListHead;

    dList = GRAPH_ALLOC(gfxCtx, 4 * sizeof(Gfx));
    dListHead = dList;

    gDPPipeSync(dListHead++);
    gDPSetPrimColor(dListHead++, 0, 0, primR, primG, primB, 255);
    gDPSetEnvColor(dListHead++, envR, envG, envB, 255);
    gSPEndDisplayList(dListHead++);

    return dList;
}

void En_Part_Actor_draw(Actor* thisx, PlayState* play) {
    EnPart* this = (EnPart*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_part.c", 647);

    if (thisx->params > 0) {
        Matrix_rotateZ(this->rotZ, MTXMODE_APPLY);
    }

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Actor_HiliteReflect_set_init(thisx, play, 0);

    if (thisx->params == 5) {
        gSPSegment(POLY_OPA_DISP++, 0x08, set_col(play->state.gfxCtx, 245, 255, 205, 30, 35, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, set_col(play->state.gfxCtx, 185, 135, 25, 20, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, set_col(play->state.gfxCtx, 255, 255, 255, 30, 40, 20));
    } else if (thisx->params == 6) {
        gSPSegment(POLY_OPA_DISP++, 0x08, set_col(play->state.gfxCtx, 55, 65, 55, 0, 0, 0));
        gSPSegment(POLY_OPA_DISP++, 0x09, set_col(play->state.gfxCtx, 205, 165, 75, 25, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, set_col(play->state.gfxCtx, 205, 165, 75, 25, 20, 0));
    } else if (thisx->params == 7) {
        gSPSegment(POLY_OPA_DISP++, 0x08, set_col(play->state.gfxCtx, 255, 255, 255, 180, 180, 180));
        gSPSegment(POLY_OPA_DISP++, 0x09, set_col(play->state.gfxCtx, 225, 205, 115, 25, 20, 0));
        gSPSegment(POLY_OPA_DISP++, 0x0A, set_col(play->state.gfxCtx, 225, 205, 115, 25, 20, 0));
    } else if ((thisx->params == 9) && (this->displayList == object_tite_DL_002FF0)) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001300));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001700));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001900));
    } else if ((thisx->params == 10) && (this->displayList == object_tite_DL_002FF0)) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001B00));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(object_tite_Tex_001F00));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(object_tite_Tex_002100));
    }

    if (this->displayList != NULL) {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_part.c", 696);
        gSPDisplayList(POLY_OPA_DISP++, this->displayList);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_part.c", 700);
}
