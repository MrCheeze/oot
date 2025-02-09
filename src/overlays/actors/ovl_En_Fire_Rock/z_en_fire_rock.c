#include "z_en_fire_rock.h"
#include "overlays/actors/ovl_En_Encount2/z_en_encount2.h"

#include "attributes.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "rand.h"
#include "regs.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "global.h"

#include "assets/objects/object_efc_star_field/object_efc_star_field.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Fire_Rock_actor_ct(Actor* thisx, PlayState* play);
void En_Fire_Rock_actor_dt(Actor* thisx, PlayState* play);
void En_Fire_Rock_actor_move(Actor* thisx, PlayState* play);
void En_Fire_Rock_actor_draw(Actor* thisx, PlayState* play);

void mode_ceiling_wait(EnFireRock* this, PlayState* play);
static void mode_floor_wait(EnFireRock* this, PlayState* play);
static void move_drop(EnFireRock* this, PlayState* play);
void move_break(EnFireRock* this, PlayState* play);

ActorProfile En_Fire_Rock_Profile = {
    /**/ ACTOR_EN_FIRE_ROCK,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_EFC_STAR_FIELD,
    /**/ sizeof(EnFireRock),
    /**/ En_Fire_Rock_actor_ct,
    /**/ En_Fire_Rock_actor_dt,
    /**/ En_Fire_Rock_actor_move,
    /**/ En_Fire_Rock_actor_draw,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x09, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 30, 30, -10, { 0, 0, 0 } },
};

static ColliderCylinderInit OcInfoData2 = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x01, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 30, 30, -10, { 0, 0, 0 } },
};

void En_Fire_Rock_actor_ct(Actor* thisx, PlayState* play) {
    PlayState* play2 = play;
    Player* player = GET_PLAYER(play);
    EnFireRock* this = (EnFireRock*)thisx;
    s16 temp;

    this->type = this->actor.params;
    if (this->type != FIRE_ROCK_CEILING_SPOT_SPAWNER) {
        Shape_Info_init(&thisx->shape, 0.0f, Actor_shadow_circle, 15.0f);
        if (this->type != FIRE_ROCK_ON_FLOOR) {
            this->angularVelocity.x = (rnd_f(10.0f) + 15.0f);
            this->angularVelocity.y = (rnd_f(10.0f) + 15.0f);
            this->angularVelocity.z = (rnd_f(10.0f) + 15.0f);
        }
    }
    switch (this->type) {
        case FIRE_ROCK_CEILING_SPOT_SPAWNER:
            this->actor.draw = NULL;
            // "☆☆☆☆☆ ceiling waiting rock ☆☆☆☆☆"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 天井待ち岩 ☆☆☆☆☆ \n" VT_RST);
            this->actionFunc = mode_ceiling_wait;
            break;
        case FIRE_ROCK_ON_FLOOR:
            Actor_set_scale(&this->actor, 0.03f);
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData2);
            // "☆☆☆☆☆ floor rock ☆☆☆☆☆"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 床岩 ☆☆☆☆☆ \n" VT_RST);
            this->collider.dim.radius = 23;
            this->collider.dim.height = 37;
            this->collider.dim.yShift = -10;
            Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
            this->actor.colChkInfo.mass = MASS_IMMOVABLE;
            this->actionFunc = mode_floor_wait;
            break;
        case FIRE_ROCK_SPAWNED_FALLING1: // spawned by encount2
            // sets unused vars?
            this->unk_17C.x = (f32)(rnd_fx(50.0f) + player->actor.world.pos.x);
            this->unk_17C.z = (f32)(rnd_fx(50.0f) + player->actor.world.pos.z);
            FALLTHROUGH;
        case FIRE_ROCK_SPAWNED_FALLING2: // spawned by encount2 and by the ceilling spawner
            this->scale = (rnd_f(2.0f) / 100.0f) + 0.02f;
            Actor_set_scale(&this->actor, this->scale);
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
            this->actor.world.rot.y = this->actor.shape.rot.y = rnd_fx(65535.0f);
            this->actionFunc = move_drop;
            this->actor.shape.shadowScale = 15.0f;
            break;
        case FIRE_ROCK_BROKEN_PIECE1:
            this->actor.velocity.y = rnd_f(3.0f) + 4.0f;
            this->actor.speed = rnd_f(3.0f) + 3.0f;
            this->scale = (rnd_f(1.0f) / 100.0f) + 0.02f;
            Actor_set_scale(&this->actor, this->scale);
            this->actor.gravity = -1.5f;
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
            this->actor.shape.shadowScale = 10.0f;
            this->actor.world.rot.y = this->actor.shape.rot.y = rnd_fx(65535.0f);
            this->actionFunc = move_drop;
            break;
        case FIRE_ROCK_BROKEN_PIECE2:
            this->actor.velocity.y = rnd_f(3.0f) + 4.0f;
            this->actor.speed = rnd_f(3.0f) + 2.0f;
            this->scale = (rnd_f(1.0f) / 500.0f) + 0.01f;
            Actor_set_scale(&this->actor, this->scale);
            this->actor.gravity = -1.2f;
            this->actor.shape.shadowScale = 5.0f;
            this->actor.world.rot.y = this->actor.shape.rot.y = rnd_fx(65535.0f);
            this->actionFunc = move_drop;
            break;
        default:
            // "☆☆☆☆☆ No such rock! ERR !!!!!! ☆☆☆☆☆"
            PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ そんな岩はねぇ！ERR!!!!!! ☆☆☆☆☆ \n" VT_RST);
            Actor_delete(&this->actor);
            break;
    }
}

void En_Fire_Rock_actor_dt(Actor* thisx, PlayState* play) {
    EnFireRock* this = (EnFireRock*)thisx;

    if ((this->actor.parent != NULL) && (this->actor.parent == &this->spawner->actor)) {
        EnEncount2* spawner = (EnEncount2*)this->actor.parent;
        if ((spawner->actor.update != NULL) && (spawner->numSpawnedRocks > 0)) {
            spawner->numSpawnedRocks--;
            PRINTF("\n\n");
            // "☆☆☆☆☆ Number of spawned instances recovery ☆☆☆☆☆%d"
            PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ 発生数回復 ☆☆☆☆☆%d\n" VT_RST, spawner->numSpawnedRocks);
            PRINTF("\n\n");
        }
    }
    ClObjPipe_dt(play, &this->collider);
}

static void move_drop(EnFireRock* this, PlayState* play) {
    Player* player;
    Vec3f flamePos;
    s32 i;

    player = GET_PLAYER(play);
    if ((this->actor.floorHeight == -10000.0f) || (this->actor.world.pos.y < (player->actor.world.pos.y - 200.0f))) {
        Actor_delete(&this->actor);
        return;
    }
    switch (this->type) {
        case FIRE_ROCK_SPAWNED_FALLING1:
            if (player->actor.world.pos.y < this->actor.world.pos.y) {
                if ((player->actor.world.pos.x > -700.0f) || (player->actor.world.pos.x < 100.0f) ||
                    (player->actor.world.pos.z > -1290.0f) || (player->actor.world.pos.z < -3880.0f)) {
                    add_calc2(&this->actor.world.pos.x, player->actor.world.pos.x, 1.0f, 10.0f);
                    add_calc2(&this->actor.world.pos.z, player->actor.world.pos.z, 1.0f, 10.0f);
                }
            }
            FALLTHROUGH;
        case FIRE_ROCK_SPAWNED_FALLING2:
            flamePos.x = rnd_fx(20.0f) + this->actor.world.pos.x;
            flamePos.y = rnd_fx(20.0f) + this->actor.world.pos.y;
            flamePos.z = rnd_fx(20.0f) + this->actor.world.pos.z;
            Effect_En_Fire_ct(play, &this->actor, &flamePos, 100, 0, 0, -1);
            break;
        case FIRE_ROCK_BROKEN_PIECE1:
            if ((play->gameplayFrames & 3) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_ROCK);
            }
            break;
    }
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->timer == 0)) {
        switch (this->type) {
            case FIRE_ROCK_SPAWNED_FALLING1:
            case FIRE_ROCK_SPAWNED_FALLING2:
                ActorQuakeRumbleSet(&this->actor, play, 5, 2);
                FALLTHROUGH;
            case FIRE_ROCK_BROKEN_PIECE1:
                _dust_ground_set(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1,
                                         8.0f, 500, 10, false);
                for (i = 0; i < 5; i++) {
                    flamePos.x = rnd_fx(20.0f) + this->actor.world.pos.x;
                    flamePos.y = this->actor.floorHeight;
                    flamePos.z = rnd_fx(20.0f) + this->actor.world.pos.z;
                    Effect_En_Fire_ct(play, &this->actor, &flamePos, 300, 0, 0, -1);
                }
                this->actionFunc = move_break;
                break;
            default:
                _dust_ground_set(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 3,
                                         8.0f, 200, 10, false);
                Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EV_EXPLOSION);
                Actor_delete(&this->actor);
                break;
        }
    }
}

/**
 * After the rock has already hit the ground and started rolling, spawn two more, giving the illusion of breaking into
 * two pieces.
 */
void move_break(EnFireRock* this, PlayState* play) {
    EnFireRock* spawnedFireRock;
    s32 nextRockType;
    s32 i;
    s32 temp;

    nextRockType = FIRE_ROCK_SPAWNED_FALLING1;
    switch (this->type) {
        case FIRE_ROCK_SPAWNED_FALLING1:
        case FIRE_ROCK_SPAWNED_FALLING2:
            nextRockType = FIRE_ROCK_BROKEN_PIECE1;
            break;
        case FIRE_ROCK_BROKEN_PIECE1:
            nextRockType = FIRE_ROCK_BROKEN_PIECE2;
            break;
    }

    if (nextRockType != FIRE_ROCK_SPAWNED_FALLING1) {
        for (i = 0; i < 2; i++) {
            spawnedFireRock = (EnFireRock*)Actor_info_make_actor(
                &play->actorCtx, play, ACTOR_EN_FIRE_ROCK, rnd_fx(3.0f) + this->actor.world.pos.x,
                rnd_fx(3.0f) + (this->actor.world.pos.y + 10.0f),
                rnd_fx(3.0f) + this->actor.world.pos.z, 0, 0, 0, nextRockType);
            if (spawnedFireRock != NULL) {
                spawnedFireRock->actor.world.rot.y = this->actor.world.rot.y;
                if (i == 0) {
                    spawnedFireRock->actor.shape.rot.y = this->actor.shape.rot.y;
                }
                spawnedFireRock->scale = this->scale - 0.01f;
            } else {
                PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ イッパイデッス ☆☆☆☆☆ \n" VT_RST);
            }
        }
        Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_ROCK);
    }
    Actor_delete(&this->actor);
}

void mode_ceiling_wait(EnFireRock* this, PlayState* play) {
    EnFireRock* spawnedFireRock;

    if (this->actor.xzDistToPlayer < 200.0f) {
        if ((this->playerNearby == 0) && (this->timer2 == 0)) {
            this->timer2 = 30;
            spawnedFireRock = (EnFireRock*)Actor_info_make_actor(
                &play->actorCtx, play, ACTOR_EN_FIRE_ROCK, rnd_fx(3.0f) + this->actor.world.pos.x,
                this->actor.world.pos.y + 10.0f, rnd_fx(3.0f) + this->actor.world.pos.z, 0, 0, 0,
                FIRE_ROCK_SPAWNED_FALLING2);
            if (spawnedFireRock != NULL) {
                spawnedFireRock->timer = 10;
            } else {
                PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ イッパイデッス ☆☆☆☆☆ \n" VT_RST);
            }
        }
        this->playerNearby = 1;
    } else {
        this->playerNearby = 0;
    }

    if (DEBUG_FEATURES && BREG(0) != 0) {
        Debug_Display_new(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                               this->actor.world.rot.x, this->actor.world.rot.y, this->actor.world.rot.z, 1.0f, 1.0f,
                               1.0f, 0, 255, 0, 255, 4, play->state.gfxCtx);
    }
}

static void mode_floor_wait(EnFireRock* this, PlayState* play) {
    Vec3f flamePos;
    s16 scale;

    if (this->timer2 == 0) {
        flamePos.x = rnd_fx(20.0f) + this->actor.world.pos.x;
        flamePos.y = rnd_fx(20.0f) + this->actor.world.pos.y;
        flamePos.z = rnd_fx(20.0f) + this->actor.world.pos.z;
        scale = 130 + (s16)rnd_fx(60.0f);
        this->timer2 = 3 + (s16)rnd_f(3.0f);
        Effect_En_Fire_ct(play, &this->actor, &flamePos, scale, 0, 0, -1);
    }
}

void En_Fire_Rock_actor_move(Actor* thisx, PlayState* play) {
    EnFireRock* this = (EnFireRock*)thisx;
    s16 setCollision;
    Player* player = GET_PLAYER(play);
    Actor* playerActor = &GET_PLAYER(play)->actor;
    f32 temp;

    if (this->timer2 != 0) {
        this->timer2--;
    }
    if (this->timer != 0) {
        this->timer--;
    }
    this->actionFunc(this, play);

    if (this->type != FIRE_ROCK_CEILING_SPOT_SPAWNER) {
        this->rockRotation.x += this->angularVelocity.x;
        this->rockRotation.y += this->angularVelocity.y;
        this->rockRotation.z += this->angularVelocity.z;
        this->relativePos.y = 3.0f;

        temp = 10.0f + (this->scale * 300.0f);
        thisx->shape.shadowScale = temp;
        if (thisx->shape.shadowScale < 10.0f) {
            thisx->shape.shadowScale = 10.0f;
        }
        if (thisx->shape.shadowScale > 20.0f) {
            thisx->shape.shadowScale = 20.0f;
        }

        if ((this->type == FIRE_ROCK_SPAWNED_FALLING1) || (this->type == FIRE_ROCK_SPAWNED_FALLING2)) {
            thisx->gravity = -0.3f - (this->scale * 7.0f);
        }
        if (this->type != FIRE_ROCK_ON_FLOOR) {
            Actor_position_moveF(thisx);
            Actor_BGcheck2(play, thisx, 50.0f, 50.0f, 100.0f,
                                    UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
        }

        setCollision = false;
        if (this->actionFunc != move_break) {
            if ((this->type == FIRE_ROCK_SPAWNED_FALLING1) || (this->type == FIRE_ROCK_SPAWNED_FALLING2) ||
                (this->type == FIRE_ROCK_BROKEN_PIECE1)) {
                if (this->collider.base.atFlags & AT_BOUNCED) {
                    this->collider.base.atFlags &= ~AT_BOUNCED;
                    Actor_SE_set(thisx, NA_SE_EV_BRIDGE_OPEN_STOP);
                    thisx->velocity.y = 0.0f;
                    thisx->speed = 0.0f;
                    this->actionFunc = move_break;
                    // "☆☆☆☆☆ Shield Defense Lv1 ☆☆☆☆☆"
                    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ シールド防御 Lv１ ☆☆☆☆☆ \n" VT_RST);
                    return;
                }
                setCollision = true;
            }
        }

        if (this->type == FIRE_ROCK_ON_FLOOR) {
            if (this->collider.base.atFlags & AT_HIT) {
                this->collider.base.atFlags &= ~AT_HIT;
                if (this->collider.base.at == playerActor) {
                    if (!(player->stateFlags1 & PLAYER_STATE1_26)) {
                        Actor_player_power_damage_AT_set2(play, thisx, 2.0f, -player->actor.world.rot.y, 3.0f, 4);
                    }
                    return;
                }
            }
            setCollision = true;
        }
        if (setCollision) {
            CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider);
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void En_Fire_Rock_actor_draw(Actor* thisx, PlayState* play) {
    EnFireRock* this = (EnFireRock*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_fire_rock.c", 747);
    Matrix_translate(thisx->world.pos.x + this->relativePos.x, thisx->world.pos.y + this->relativePos.y,
                     thisx->world.pos.z + this->relativePos.z, MTXMODE_NEW);
    Matrix_rotateX(DEG_TO_RAD(this->rockRotation.x), MTXMODE_APPLY);
    Matrix_rotateY(DEG_TO_RAD(this->rockRotation.y), MTXMODE_APPLY);
    Matrix_rotateZ(DEG_TO_RAD(this->rockRotation.z), MTXMODE_APPLY);
    Matrix_scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 155, 55, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 155, 255, 55, 255);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_fire_rock.c", 768);
    gSPDisplayList(POLY_OPA_DISP++, object_efc_star_field_DL_000DE0);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_fire_rock.c", 773);
}
