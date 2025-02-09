/*
 * File: z_en_kusa.c
 * Overlay: ovl_en_kusa
 * Description: Bush
 */

#include "z_en_kusa.h"
#include "overlays/actors/ovl_En_Insect/z_en_insect.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"
#include "assets/objects/object_kusa/object_kusa.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_THROW_ONLY)

void En_Kusa_actor_ct(Actor* thisx, PlayState* play);
void En_Kusa_actor_dt(Actor* thisx, PlayState* play2);
void En_Kusa_actor_move(Actor* thisx, PlayState* play);
void En_Kusa_actor_draw(Actor* thisx, PlayState* play);

static void mv_carry_init(EnKusa* this);
static void mv_dmaWait_init(EnKusa* this);
static void mv_stop_init(EnKusa* this);
static void mv_drop_init(EnKusa* this);
void mv_zangai_init(EnKusa* this);
void mv_mebae_init(EnKusa* this);
void mv_grow_init(EnKusa* this);

static void mv_drop(EnKusa* this, PlayState* play);
static void mv_dmaWait(EnKusa* this, PlayState* play);
static void mv_stop(EnKusa* this, PlayState* play);
static void mv_carry(EnKusa* this, PlayState* play);
void mv_zangai(EnKusa* this, PlayState* play);
void mv_zangai2(EnKusa* this, PlayState* play);
void mv_mebae(EnKusa* this, PlayState* play);
void mv_grow(EnKusa* this, PlayState* play);

static s16 Kusa_AngGoalSpdX = 0;
static s16 Kusa_AngSpdX = 0;
static s16 Kusa_AngGoalSpdY = 0;
static s16 Kusa_AngSpdY = 0;

ActorProfile En_Kusa_Profile = {
    /**/ ACTOR_EN_KUSA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnKusa),
    /**/ En_Kusa_actor_ct,
    /**/ En_Kusa_actor_dt,
    /**/ En_Kusa_actor_move,
    /**/ NULL,
};

static s16 KusaBankTbl[] = { OBJECT_GAMEPLAY_FIELD_KEEP, OBJECT_KUSA, OBJECT_KUSA };

static ColliderCylinderInit ClPipeDt_kusa = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x4FC00758, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 12, 44, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit StatusDt_kusa = { 0, 12, 30, MASS_IMMOVABLE };

static Vec3f unit_vec[] = {
    { 0.0f, 0.7071f, 0.7071f },
    { 0.7071f, 0.7071f, 0.0f },
    { 0.0f, 0.7071f, -0.7071f },
    { -0.7071f, 0.7071f, 0.0f },
};

static s16 scl[] = { 108, 102, 96, 84, 66, 55, 42, 38 };

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 400, ICHAIN_CONTINUE),         ICHAIN_F32_DIV1000(gravity, -3200, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(minVelocityY, -17000, ICHAIN_CONTINUE), ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),      ICHAIN_F32(cullingVolumeDownward, 120, ICHAIN_STOP),
};

static void set_moveProc(EnKusa* this, EnKusaActionFunc actionFunc) {
    this->timer = 0;
    this->actionFunc = actionFunc;
}

static s32 set_pos_groundY(EnKusa* this, PlayState* play, f32 yOffset) {
    s32 pad;
    CollisionPoly* groundPoly;
    Vec3f pos;
    s32 bgId;
    f32 floorY;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + 30.0f;
    pos.z = this->actor.world.pos.z;

    floorY = T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &groundPoly, &bgId, &this->actor, &pos);

    if (floorY > BGCHECK_Y_MIN) {
        this->actor.world.pos.y = floorY + yOffset;
        xyz_t_move(&this->actor.home.pos, &this->actor.world.pos);
        return true;
    } else {
        PRINTF_COLOR_WARNING();
        // "Failure attaching to ground"
        PRINTF("地面に付着失敗(%s %d)\n", "../z_en_kusa.c", 323);
        PRINTF_RST();
        return false;
    }
}

void itemSet_kusa(EnKusa* this, PlayState* play) {
    s16 dropParams;

    switch (PARAMS_GET_U(this->actor.params, 0, 2)) {
        case ENKUSA_TYPE_0:
        case ENKUSA_TYPE_2:
            dropParams = PARAMS_GET_U(this->actor.params, 8, 4);

            if (dropParams >= 0xD) {
                dropParams = 0;
            }
            Item_Set_Std(play, NULL, &this->actor.world.pos, dropParams << 4);
            break;
        case ENKUSA_TYPE_1:
            if (fqrand() < 0.5f) {
                Item_set0(play, &this->actor.world.pos, ITEM00_SEEDS);
            } else {
                Item_set0(play, &this->actor.world.pos, ITEM00_RECOVERY_HEART);
            }
            break;
    }
}

static void setSpd_gravity(EnKusa* this) {
    this->actor.velocity.y += this->actor.gravity;

    if (this->actor.velocity.y < this->actor.minVelocityY) {
        this->actor.velocity.y = this->actor.minVelocityY;
    }
}

static void set_AirResist(Vec3f* vec, f32 scale) {
    scale += ((fqrand() * 0.2f) - 0.1f) * scale;
    vec->x -= vec->x * scale;
    vec->y -= vec->y * scale;
    vec->z -= vec->z * scale;
}

void set_grow_init_scale(EnKusa* this) {
    this->actor.scale.y = 0.16000001f;
    this->actor.scale.x = 0.120000005f;
    this->actor.scale.z = 0.120000005f;
}

void effect_kusa_kakera(EnKusa* this, PlayState* play) {
    Vec3f velocity;
    Vec3f pos;
    s32 i;
    s32 scaleIndex;
    Vec3f* dir;
    s32 pad;

    for (i = 0; i < ARRAY_COUNT(unit_vec); i++) {
        dir = &unit_vec[i];

        pos.x = this->actor.world.pos.x + (dir->x * this->actor.scale.x * 20.0f);
        pos.y = this->actor.world.pos.y + (dir->y * this->actor.scale.y * 20.0f) + 10.0f;
        pos.z = this->actor.world.pos.z + (dir->z * this->actor.scale.z * 20.0f);

        velocity.x = (fqrand() - 0.5f) * 8.0f;
        velocity.y = fqrand() * 10.0f;
        velocity.z = (fqrand() - 0.5f) * 8.0f;

        scaleIndex = (s32)(fqrand() * 111.1f) & 7;

        Effect_Kakera_ct2(play, &pos, &velocity, &pos, -100, 64, 40, 3, 0, scl[scaleIndex], 0, 0, 80,
                             KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_KEEP, gCuttableShrubStalkDL);

        pos.x = this->actor.world.pos.x + (dir->x * this->actor.scale.x * 40.0f);
        pos.y = this->actor.world.pos.y + (dir->y * this->actor.scale.y * 40.0f) + 10.0f;
        pos.z = this->actor.world.pos.z + (dir->z * this->actor.scale.z * 40.0f);

        velocity.x = (fqrand() - 0.5f) * 6.0f;
        velocity.y = fqrand() * 10.0f;
        velocity.z = (fqrand() - 0.5f) * 6.0f;

        scaleIndex = (s32)(fqrand() * 111.1f) % 7;

        Effect_Kakera_ct2(play, &pos, &velocity, &pos, -100, 64, 40, 3, 0, scl[scaleIndex], 0, 0, 80,
                             KAKERA_COLOR_NONE, OBJECT_GAMEPLAY_KEEP, gCuttableShrubTipDL);
    }
}

void make_mushi_kusa(EnKusa* this, PlayState* play) {
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

void set_collision_kusa(Actor* thisx, PlayState* play) {
    EnKusa* this = (EnKusa*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &ClPipeDt_kusa);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
}

void En_Kusa_actor_ct(Actor* thisx, PlayState* play) {
    EnKusa* this = (EnKusa*)thisx;

    ValueSet_process(&this->actor, value_init);

    if (play->csCtx.state != CS_STATE_IDLE) {
        this->actor.cullingVolumeDistance += 1000.0f;
    }

    set_collision_kusa(thisx, play);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &StatusDt_kusa);

    if (this->actor.shape.rot.y == 0) {
        s16 rand = rnd_f(0x10000);

        this->actor.world.rot.y = rand;
        this->actor.home.rot.y = rand;
        this->actor.shape.rot.y = rand;
    }

    if (!set_pos_groundY(this, play, 0.0f)) {
        Actor_delete(&this->actor);
        return;
    }

    this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, KusaBankTbl[PARAMS_GET_U(thisx->params, 0, 2)]);

    if (this->requiredObjectSlot < 0) {
        // "Bank danger!"
        PRINTF("Error : バンク危険！ (arg_data 0x%04x)(%s %d)\n", thisx->params, "../z_en_kusa.c", 561);
        Actor_delete(&this->actor);
        return;
    }

    mv_dmaWait_init(this);
}

void En_Kusa_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnKusa* this = (EnKusa*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void mv_dmaWait_init(EnKusa* this) {
    set_moveProc(this, mv_dmaWait);
}

static void mv_dmaWait(EnKusa* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        if (this->actor.flags & ACTOR_FLAG_GRASS_DESTROYED) {
            mv_zangai_init(this);
        } else {
            mv_stop_init(this);
        }

        this->actor.draw = En_Kusa_actor_draw;
        this->actor.objectSlot = this->requiredObjectSlot;
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }
}

static void mv_stop_init(EnKusa* this) {
    set_moveProc(this, mv_stop);
    this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
}

static void mv_stop(EnKusa* this, PlayState* play) {
    s32 pad;

    if (Actor_carry_check(&this->actor, play)) {
        mv_carry_init(this);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_PL_PULL_UP_PLANT);
    } else if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        effect_kusa_kakera(this, play);
        itemSet_kusa(this, play);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EV_PLANT_BROKEN);

        if (PARAMS_GET_U(this->actor.params, 4, 1)) {
            make_mushi_kusa(this, play);
        }

        if (PARAMS_GET_U(this->actor.params, 0, 2) == ENKUSA_TYPE_0) {
            Actor_delete(&this->actor);
            return;
        }

        mv_zangai_init(this);
        this->actor.flags |= ACTOR_FLAG_GRASS_DESTROYED;
    } else {
        if (!(this->collider.base.ocFlags1 & OC1_TYPE_PLAYER) && (this->actor.xzDistToPlayer > 12.0f)) {
            this->collider.base.ocFlags1 |= OC1_TYPE_PLAYER;
        }

        if (this->actor.xzDistToPlayer < 600.0f) {
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);

            if (this->actor.xzDistToPlayer < 400.0f) {
                CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
                if (this->actor.xzDistToPlayer < 100.0f) {
                    Actor_carry_request(&this->actor, play);
                }
            }
        }
    }
}

static void mv_carry_init(EnKusa* this) {
    set_moveProc(this, mv_carry);
    this->actor.room = -1;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
}

static void mv_carry(EnKusa* this, PlayState* play) {
    if (Actor_carry_end_check(&this->actor, play)) {
        this->actor.room = play->roomCtx.curRoom.num;
        mv_drop_init(this);
        this->actor.velocity.x = this->actor.speed * sin_s(this->actor.world.rot.y);
        this->actor.velocity.z = this->actor.speed * cos_s(this->actor.world.rot.y);
        this->actor.colChkInfo.mass = 240;
        this->actor.gravity = -0.1f;
        setSpd_gravity(this);
        set_AirResist(&this->actor.velocity, 0.005f);
        Actor_position_move(&this->actor);
        Actor_BGcheck2(play, &this->actor, 7.5f, 35.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_6 |
                                    UPDBGCHECKINFO_FLAG_7);
        this->actor.gravity = -3.2f;
    }
}

static void mv_drop_init(EnKusa* this) {
    set_moveProc(this, mv_drop);
    Kusa_AngGoalSpdX = -0xBB8;
    Kusa_AngGoalSpdY = (fqrand() - 0.5f) * 1600.0f;
    Kusa_AngSpdX = 0;
    Kusa_AngSpdY = 0;
}

static void mv_drop(EnKusa* this, PlayState* play) {
    s32 pad;
    Vec3f contactPos;

    if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH | BGCHECKFLAG_WALL)) {
        if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
            Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EV_PLANT_BROKEN);
        }
        effect_kusa_kakera(this, play);
        itemSet_kusa(this, play);
        switch (PARAMS_GET_U(this->actor.params, 0, 2)) {
            case ENKUSA_TYPE_0:
            case ENKUSA_TYPE_2:
                Actor_delete(&this->actor);
                break;

            case ENKUSA_TYPE_1:
                mv_mebae_init(this);
                break;
        }
        return;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) {
        contactPos.x = this->actor.world.pos.x;
        contactPos.y = this->actor.world.pos.y + this->actor.depthInWater;
        contactPos.z = this->actor.world.pos.z;
        Effect_SS_G_Splash_sc_cl_ct(play, &contactPos, NULL, NULL, 0, 400);
        Effect_SS_G_Ripple_ct2(play, &contactPos, 150, 650, 0);
        Effect_SS_G_Ripple_ct2(play, &contactPos, 400, 800, 4);
        Effect_SS_G_Ripple_ct2(play, &contactPos, 500, 1100, 8);
        this->actor.minVelocityY = -3.0f;
        Kusa_AngSpdX >>= 1;
        Kusa_AngGoalSpdX >>= 1;
        Kusa_AngSpdY >>= 1;
        Kusa_AngGoalSpdY >>= 1;
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WATER_TOUCH;
        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EV_DIVE_INTO_WATER_L);
    }

    setSpd_gravity(this);
    chase_s(&Kusa_AngSpdX, Kusa_AngGoalSpdX, 0x1F4);
    chase_s(&Kusa_AngSpdY, Kusa_AngGoalSpdY, 0xAA);
    this->actor.shape.rot.x += Kusa_AngSpdX;
    this->actor.shape.rot.y += Kusa_AngSpdY;
    set_AirResist(&this->actor.velocity, 0.05f);
    Actor_position_move(&this->actor);
    Actor_BGcheck2(play, &this->actor, 7.5f, 35.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_6 |
                                UPDBGCHECKINFO_FLAG_7);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void mv_zangai_init(EnKusa* this) {
    switch (PARAMS_GET_U(this->actor.params, 0, 2)) {
        case ENKUSA_TYPE_2:
            set_moveProc(this, mv_zangai2);
            break;
        case ENKUSA_TYPE_1:
            set_moveProc(this, mv_zangai);
            break;
    }
}

void mv_zangai(EnKusa* this, PlayState* play) {
    if (this->timer >= 120) {
        mv_grow_init(this);
    }
}

void mv_zangai2(EnKusa* this, PlayState* play) {
}

void mv_mebae_init(EnKusa* this) {
    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.y = this->actor.home.pos.y - 9.0f;
    this->actor.world.pos.z = this->actor.home.pos.z;
    set_grow_init_scale(this);
    this->actor.shape.rot = this->actor.home.rot;
    set_moveProc(this, mv_mebae);
}

void mv_mebae(EnKusa* this, PlayState* play) {
    if (this->timer > 120) {
        if (chase_f(&this->actor.world.pos.y, this->actor.home.pos.y, 0.6f)) {
            if (this->timer >= 170) {
                mv_grow_init(this);
            }
        }
    }
}

void mv_grow_init(EnKusa* this) {
    set_moveProc(this, mv_grow);
    set_grow_init_scale(this);
    this->actor.shape.rot = this->actor.home.rot;
    this->actor.flags &= ~ACTOR_FLAG_GRASS_DESTROYED;
}

void mv_grow(EnKusa* this, PlayState* play) {
    s32 isFullyGrown = true;

    isFullyGrown &= chase_f(&this->actor.scale.y, 0.4f, 0.014f);
    isFullyGrown &= chase_f(&this->actor.scale.x, 0.4f, 0.011f);
    this->actor.scale.z = this->actor.scale.x;

    if (isFullyGrown) {
        Actor_set_scale(&this->actor, 0.4f);
        mv_stop_init(this);
        this->collider.base.ocFlags1 &= ~OC1_TYPE_PLAYER;
    }
}

void En_Kusa_actor_move(Actor* thisx, PlayState* play) {
    EnKusa* this = (EnKusa*)thisx;

    this->timer++;

    this->actionFunc(this, play);

    if (this->actor.flags & ACTOR_FLAG_GRASS_DESTROYED) {
        this->actor.shape.yOffset = -6.25f;
    } else {
        this->actor.shape.yOffset = 0.0f;
    }
}

void En_Kusa_actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* near_model[] = { gFieldBushDL, object_kusa_DL_000140, object_kusa_DL_000140 };
    EnKusa* this = (EnKusa*)thisx;

    if (this->actor.flags & ACTOR_FLAG_GRASS_DESTROYED) {
        Cheap_gfx_display(play, object_kusa_DL_0002E0);
    } else {
        Cheap_gfx_display(play, near_model[PARAMS_GET_U(thisx->params, 0, 2)]);
    }
}
