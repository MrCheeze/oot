/*
 * File: z_en_ishi.c
 * Overlay: ovl_En_Ishi
 * Description: Small and large gray rocks
 */

#include "z_en_ishi.h"
#include "overlays/actors/ovl_En_Insect/z_en_insect.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"
#include "quake.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_THROW_ONLY

void En_Ishi_actor_ct(Actor* thisx, PlayState* play);
void En_Ishi_actor_dt(Actor* thisx, PlayState* play2);
void En_Ishi_actor_move(Actor* thisx, PlayState* play);
void En_Ishi_actor_draw(Actor* thisx, PlayState* play);

static void mv_stop_init(EnIshi* this);
static void mv_stop(EnIshi* this, PlayState* play);
static void mv_carry_init(EnIshi* this);
static void mv_carry(EnIshi* this, PlayState* play);
static void mv_drop_init(EnIshi* this);
static void mv_drop(EnIshi* this, PlayState* play);
void effect_kakera_ishi_s(EnIshi* this, PlayState* play);
void effect_kakera_ishi_gin(EnIshi* this, PlayState* play);
void effect_kemuri_ishi_s(EnIshi* this, PlayState* play);
void effect_kemuri_ishi_gin(EnIshi* this, PlayState* play);

static s16 Ishi_AngSpdX = 0;
static s16 Ishi_AngSpdY = 0;

ActorProfile En_Ishi_Profile = {
    /**/ ACTOR_EN_ISHI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(EnIshi),
    /**/ En_Ishi_actor_ct,
    /**/ En_Ishi_actor_dt,
    /**/ En_Ishi_actor_move,
    /**/ En_Ishi_actor_draw,
};

static f32 Ishi_ScaleTbl[] = { 0.1f, 0.4f };
static f32 Ishi_OffsetYTbl[] = { 58.0f, 80.0f };
static f32 Ishi_AirResistTbl[] = { 0.0f, 0.005f };

// the sizes of these arrays are very large and take up way more space than it needs to.
// coincidentally the sizes are the same as the ID for NA_SE_EV_ROCK_BROKEN, which may explain a mistake that could
// have been made here
static u16 Ishi_broken_sound_flag[0x2852] = { NA_SE_EV_ROCK_BROKEN, NA_SE_EV_WALL_BROKEN };

static u8 Ishi_broken_sound_timer[0x2852] = { 20, 40 };

static EnIshiEffectSpawnFunc Ishi_KakeraProc[] = { effect_kakera_ishi_s, effect_kakera_ishi_gin };

static EnIshiEffectSpawnFunc Ishi_KemuriProc[] = { effect_kemuri_ishi_s, effect_kemuri_ishi_gin };

static ColliderCylinderInit ClPipeDt_ishi[] = {
    {
        {
            COL_MATERIAL_HARD,
            AT_NONE,
            AC_ON | AC_HARD | AC_TYPE_PLAYER,
            OC1_ON | OC1_TYPE_ALL,
            OC2_TYPE_2,
            COLSHAPE_CYLINDER,
        },
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x4FC1FFFE, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 10, 18, -2, { 0, 0, 0 } },
    },
    {
        {
            COL_MATERIAL_HARD,
            AT_NONE,
            AC_ON | AC_HARD | AC_TYPE_PLAYER,
            OC1_ON | OC1_TYPE_ALL,
            OC2_TYPE_2,
            COLSHAPE_CYLINDER,
        },
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x4FC1FFF6, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 55, 70, 0, { 0, 0, 0 } },
    },
};

static CollisionCheckInfoInit StatusDt_ishi = { 0, 12, 60, MASS_IMMOVABLE };

void set_collision_data_ishi(Actor* thisx, PlayState* play) {
    EnIshi* this = (EnIshi*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &ClPipeDt_ishi[PARAMS_GET_U(this->actor.params, 0, 1)]);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
}

static s32 set_pos_groundY(EnIshi* this, PlayState* play, f32 arg2) {
    CollisionPoly* poly;
    Vec3f checkPos;
    s32 bgId;
    f32 floorY;

    checkPos.x = this->actor.world.pos.x;
    checkPos.y = this->actor.world.pos.y + 30.0f;
    checkPos.z = this->actor.world.pos.z;
    floorY = T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &poly, &bgId, &this->actor, &checkPos);
    if (floorY > BGCHECK_Y_MIN) {
        this->actor.world.pos.y = floorY + arg2;
        xyz_t_move(&this->actor.home.pos, &this->actor.world.pos);
        return true;
    } else {
        PRINTF_COLOR_WARNING();
        // "Failure attaching to ground"
        PRINTF("地面に付着失敗(%s %d)\n", "../z_en_ishi.c", 388);
        PRINTF_RST();
        return false;
    }
}

void effect_kakera_ishi_s(EnIshi* this, PlayState* play) {
    static s16 scl[] = { 16, 13, 11, 9, 7, 5 };
    s32 pad;
    Vec3f velocity;
    Vec3f pos;
    s16 phi_v0;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(scl); i++) {
        pos.x = this->actor.world.pos.x + (fqrand() - 0.5f) * 8.0f;
        pos.y = this->actor.world.pos.y + (fqrand() * 5.0f) + 5.0f;
        pos.z = this->actor.world.pos.z + (fqrand() - 0.5f) * 8.0f;
        xyz_t_move(&velocity, &this->actor.velocity);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            velocity.x *= 0.8f;
            velocity.y *= -0.8f;
            velocity.z *= 0.8f;
        } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            velocity.x *= -0.8f;
            velocity.y *= 0.8f;
            velocity.z *= -0.8f;
        }
        velocity.x += (fqrand() - 0.5f) * 11.0f;
        velocity.y += fqrand() * 6.0f;
        velocity.z += (fqrand() - 0.5f) * 11.0f;
        if (fqrand() < 0.5f) {
            phi_v0 = 65;
        } else {
            phi_v0 = 33;
        }
        Effect_Kakera_ct2(play, &pos, &velocity, &pos, -420, phi_v0, 30, 5, 0, scl[i], 3, 10, 40,
                             KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_FIELD_KEEP, gFieldKakeraDL);
    }
}

void effect_kakera_ishi_gin(EnIshi* this, PlayState* play) {
    static s16 scl[] = { 145, 135, 120, 100, 70, 50, 45, 40, 35 };
    Actor* thisx = &this->actor;
    Vec3f velocity;
    Vec3f pos;
    s16 angle = 0x1000;
    s32 i;
    f32 rand;
    s16 phi_v0;
    s16 phi_v1;

    for (i = 0; i < ARRAY_COUNT(scl); i++) {
        angle += 0x4E20;
        rand = fqrand() * 10.0f;
        pos.x = this->actor.world.pos.x + (sin_s(angle) * rand);
        pos.y = this->actor.world.pos.y + (fqrand() * 40.0f) + 5.0f;
        pos.z = this->actor.world.pos.z + (cos_s(angle) * rand);
        xyz_t_move(&velocity, &thisx->velocity);
        if (thisx->bgCheckFlags & BGCHECKFLAG_GROUND) {
            velocity.x *= 0.9f;
            velocity.y *= -0.8f;
            velocity.z *= 0.9f;
        } else if (thisx->bgCheckFlags & BGCHECKFLAG_WALL) {
            velocity.x *= -0.9f;
            velocity.y *= 0.8f;
            velocity.z *= -0.9f;
        }
        rand = fqrand() * 10.0f;
        velocity.x += rand * sin_s(angle);
        velocity.y += (fqrand() * 4.0f) + ((fqrand() * i) * 0.7f);
        velocity.z += rand * cos_s(angle);
        if (i == 0) {
            phi_v0 = 41;
            phi_v1 = -450;
        } else if (i < 4) {
            phi_v0 = 37;
            phi_v1 = -380;
        } else {
            phi_v0 = 69;
            phi_v1 = -320;
        }
        Effect_Kakera_ct2(play, &pos, &velocity, &this->actor.world.pos, phi_v1, phi_v0, 30, 5, 0, scl[i], 5, 2,
                             70, KAKERA_COLOR_WHITE, OBJECT_GAMEPLAY_FIELD_KEEP, gSilverRockFragmentsDL);
    }
}

void effect_kemuri_ishi_s(EnIshi* this, PlayState* play) {
    Vec3f pos;

    xyz_t_move(&pos, &this->actor.world.pos);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        pos.x += 2.0f * this->actor.velocity.x;
        pos.y -= 2.0f * this->actor.velocity.y;
        pos.z += 2.0f * this->actor.velocity.z;
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        pos.x -= 2.0f * this->actor.velocity.x;
        pos.y += 2.0f * this->actor.velocity.y;
        pos.z -= 2.0f * this->actor.velocity.z;
    }
    dust_fly_set2(play, &pos, 60.0f, 3, 0x50, 0x3C, 1);
}

void effect_kemuri_ishi_gin(EnIshi* this, PlayState* play) {
    Vec3f pos;

    xyz_t_move(&pos, &this->actor.world.pos);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        pos.x += 2.0f * this->actor.velocity.x;
        pos.y -= 2.0f * this->actor.velocity.y;
        pos.z += 2.0f * this->actor.velocity.z;
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        pos.x -= 2.0f * this->actor.velocity.x;
        pos.y += 2.0f * this->actor.velocity.y;
        pos.z -= 2.0f * this->actor.velocity.z;
    }
    dust_fly_set2(play, &pos, 140.0f, 0xA, 0xB4, 0x5A, 1);
}

void itemSet_ishi(EnIshi* this, PlayState* play) {
    s16 dropParams;

    if (PARAMS_GET_U(this->actor.params, 0, 1) == ROCK_SMALL) {
        dropParams = PARAMS_GET_U(this->actor.params, 8, 4);

        if (dropParams >= 0xD) {
            dropParams = 0;
        }

        Item_Set_Std(play, NULL, &this->actor.world.pos, dropParams << 4);
    }
}

static void setSpd_gravity(EnIshi* this) {
    this->actor.velocity.y += this->actor.gravity;

    if (this->actor.velocity.y < this->actor.minVelocityY) {
        this->actor.velocity.y = this->actor.minVelocityY;
    }
}

static void set_AirResist(Vec3f* arg0, f32 arg1) {
    arg1 += ((fqrand() * 0.2f) - 0.1f) * arg1;
    arg0->x -= arg0->x * arg1;
    arg0->y -= arg0->y * arg1;
    arg0->z -= arg0->z * arg1;
}

void make_mushi_ishi(EnIshi* this, PlayState* play) {
    s32 i;

    for (i = 0; i < 3; i++) {
        Actor* bug =
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_INSECT, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, fqrand() * 0xFFFF, 0, INSECT_TYPE_SPAWNED);

        if (bug == NULL) {
            break;
        }
    }
}

static InitChainEntry value_init[][5] = {
    {
        ICHAIN_F32_DIV1000(gravity, -1200, ICHAIN_CONTINUE),
        ICHAIN_F32_DIV1000(minVelocityY, -20000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 150, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_STOP),
    },
    {
        ICHAIN_F32_DIV1000(gravity, -2500, ICHAIN_CONTINUE),
        ICHAIN_F32_DIV1000(minVelocityY, -20000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 250, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 500, ICHAIN_STOP),
    },
};

void En_Ishi_actor_ct(Actor* thisx, PlayState* play) {
    EnIshi* this = (EnIshi*)thisx;
    s16 type = PARAMS_GET_U(this->actor.params, 0, 1);

    ValueSet_process(&this->actor, value_init[type]);
    if (play->csCtx.state != CS_STATE_IDLE) {
        this->actor.cullingVolumeDistance += 1000.0f;
    }
    if (this->actor.shape.rot.y == 0) {
        this->actor.shape.rot.y = this->actor.world.rot.y = rnd_f(0x10000);
    }
    Actor_set_scale(&this->actor, Ishi_ScaleTbl[type]);
    set_collision_data_ishi(&this->actor, play);
    if ((type == ROCK_LARGE) && Actor_Environment_sw_Check(play, ISHI_GET_SWITCH_FLAG(&this->actor))) {
        Actor_delete(&this->actor);
        return;
    }
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &StatusDt_ishi);
    this->actor.shape.yOffset = Ishi_OffsetYTbl[type];
    if (!PARAMS_GET_U(this->actor.params, 5, 1) && !set_pos_groundY(this, play, 0.0f)) {
        Actor_delete(&this->actor);
        return;
    }
    mv_stop_init(this);
}

void En_Ishi_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnIshi* this = (EnIshi*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void mv_stop_init(EnIshi* this) {
    this->actionFunc = mv_stop;
}

static void mv_stop(EnIshi* this, PlayState* play) {
    static u16 pull_up_sound_flag[] = { NA_SE_PL_PULL_UP_ROCK, NA_SE_PL_PULL_UP_BIGROCK };
    s32 pad;
    s16 type = PARAMS_GET_U(this->actor.params, 0, 1);

    if (Actor_carry_check(&this->actor, play)) {
        mv_carry_init(this);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, pull_up_sound_flag[type]);
        if (PARAMS_GET_U(this->actor.params, 4, 1)) {
            make_mushi_ishi(this, play);
        }
    } else if ((this->collider.base.acFlags & AC_HIT) && (type == ROCK_SMALL) &&
               this->collider.elem.acHitElem->atDmgInfo.dmgFlags & (DMG_HAMMER | DMG_EXPLOSIVE)) {
        itemSet_ishi(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, Ishi_broken_sound_timer[type], Ishi_broken_sound_flag[type]);
        Ishi_KakeraProc[type](this, play);
        Ishi_KemuriProc[type](this, play);
        Actor_delete(&this->actor);
    } else if (this->actor.xzDistToPlayer < 600.0f) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        this->collider.base.acFlags &= ~AC_HIT;
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        if (this->actor.xzDistToPlayer < 400.0f) {
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
            if (this->actor.xzDistToPlayer < 90.0f) {
                // GI_NONE in these cases allows the player to lift the actor
                if (type == ROCK_LARGE) {
                    Actor_carry_request_set2(&this->actor, play, GI_NONE, 80.0f, 20.0f);
                } else {
                    Actor_carry_request_set2(&this->actor, play, GI_NONE, 50.0f, 10.0f);
                }
            }
        }
    }
}

static void mv_carry_init(EnIshi* this) {
    this->actionFunc = mv_carry;
    this->actor.room = -1;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
}

static void mv_carry(EnIshi* this, PlayState* play) {
    if (Actor_carry_end_check(&this->actor, play)) {
        this->actor.room = play->roomCtx.curRoom.num;
        if (PARAMS_GET_U(this->actor.params, 0, 1) == ROCK_LARGE) {
            Actor_Environment_sw_On(play, ISHI_GET_SWITCH_FLAG(&this->actor));
        }
        mv_drop_init(this);
        setSpd_gravity(this);
        set_AirResist(&this->actor.velocity, Ishi_AirResistTbl[PARAMS_GET_U(this->actor.params, 0, 1)]);
        Actor_position_move(&this->actor);
        Actor_BGcheck2(play, &this->actor, 7.5f, 35.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_6 |
                                    UPDBGCHECKINFO_FLAG_7);
    }
}

static void mv_drop_init(EnIshi* this) {
    this->actor.velocity.x = sin_s(this->actor.world.rot.y) * this->actor.speed;
    this->actor.velocity.z = cos_s(this->actor.world.rot.y) * this->actor.speed;
    if (PARAMS_GET_U(this->actor.params, 0, 1) == ROCK_SMALL) {
        Ishi_AngSpdX = (fqrand() - 0.5f) * 16000.0f;
        Ishi_AngSpdY = (fqrand() - 0.5f) * 2400.0f;
    } else {
        Ishi_AngSpdX = (fqrand() - 0.5f) * 8000.0f;
        Ishi_AngSpdY = (fqrand() - 0.5f) * 1600.0f;
    }
    this->actor.colChkInfo.mass = 240;
    this->actionFunc = mv_drop;
}

static void mv_drop(EnIshi* this, PlayState* play) {
    s32 pad;
    s16 type = PARAMS_GET_U(this->actor.params, 0, 1);
    s32 pad2;
    s32 quakeIndex;
    Vec3f contactPos;

    if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_WALL)) {
        itemSet_ishi(this, play);
        Ishi_KakeraProc[type](this, play);
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
            Effect_SE_Info_new(play, &this->actor.world.pos, Ishi_broken_sound_timer[type],
                                             Ishi_broken_sound_flag[type]);
            Ishi_KemuriProc[type](this, play);
        }
        if (type == ROCK_LARGE) {
            quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
            setSpeedQuake(quakeIndex, -0x3CB0);
            setScaleQuake(quakeIndex, 3, 0, 0, 0);
            setTimerQuake(quakeIndex, 7);
            z_vibctl2_vib_setQ(this->actor.xyzDistToPlayerSq, 255, 20, 150);
        }
        Actor_delete(&this->actor);
        return;
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
        contactPos.x = this->actor.world.pos.x;
        contactPos.y = this->actor.world.pos.y + this->actor.depthInWater;
        contactPos.z = this->actor.world.pos.z;
        Effect_SS_G_Splash_sc_cl_ct(play, &contactPos, NULL, NULL, 0, 350);
        if (type == ROCK_SMALL) {
            Effect_SS_G_Ripple_ct2(play, &contactPos, 150, 650, 0);
            Effect_SS_G_Ripple_ct2(play, &contactPos, 400, 800, 4);
            Effect_SS_G_Ripple_ct2(play, &contactPos, 500, 1100, 8);
        } else {
            Effect_SS_G_Ripple_ct2(play, &contactPos, 300, 700, 0);
            Effect_SS_G_Ripple_ct2(play, &contactPos, 500, 900, 4);
            Effect_SS_G_Ripple_ct2(play, &contactPos, 500, 1300, 8);
        }
        this->actor.minVelocityY = -6.0f;
        Ishi_AngSpdX >>= 2;
        Ishi_AngSpdY >>= 2;
        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EV_DIVE_INTO_WATER_L);
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
    }
    chase_f(&this->actor.shape.yOffset, 0.0f, 2.0f);
    setSpd_gravity(this);
    set_AirResist(&this->actor.velocity, Ishi_AirResistTbl[type]);
    Actor_position_move(&this->actor);
    this->actor.shape.rot.x += Ishi_AngSpdX;
    this->actor.shape.rot.y += Ishi_AngSpdY;
    Actor_BGcheck2(play, &this->actor, 7.5f, 35.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_6 |
                                UPDBGCHECKINFO_FLAG_7);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void En_Ishi_actor_move(Actor* thisx, PlayState* play) {
    EnIshi* this = (EnIshi*)thisx;

    this->actionFunc(this, play);
}

void draw_s(EnIshi* this, PlayState* play) {
    Cheap_gfx_display(play, gFieldKakeraDL);
}

void draw_gin(EnIshi* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_en_ishi.c", 1050);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_ishi.c", 1055);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
    gSPDisplayList(POLY_OPA_DISP++, gSilverRockDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ishi.c", 1062);
}

static EnIshiDrawFunc draw_proc[] = { draw_s, draw_gin };

void En_Ishi_actor_draw(Actor* thisx, PlayState* play) {
    EnIshi* this = (EnIshi*)thisx;

    draw_proc[PARAMS_GET_U(this->actor.params, 0, 1)](this, play);
}
