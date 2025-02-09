/*
 * File: z_bg_jya_haheniron
 * Overlay: ovl_Bg_Jya_Haheniron
 * Description: Chunks of Iron Knucle Chair and Pillar
 */

#include "z_bg_jya_haheniron.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "assets/objects/object_jya_iron/object_jya_iron.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Jya_Haheniron_actor_ct(Actor* thisx, PlayState* play);
void Bg_Jya_Haheniron_actor_dt(Actor* thisx, PlayState* play);
void Bg_Jya_Haheniron_actor_move(Actor* thisx, PlayState* play);
void Bg_Jya_Haheniron_actor_draw(Actor* thisx, PlayState* play);

void mv_type_at_init(BgJyaHaheniron* this);
void mv_type_at(BgJyaHaheniron* this, PlayState* play);
void mv_type_upper_init(BgJyaHaheniron* this);
void mv_type_upper(BgJyaHaheniron* this, PlayState* play);
void mv_type_lower_init(BgJyaHaheniron* this);
void mv_type_lower(BgJyaHaheniron* this, PlayState* play);

ActorProfile Bg_Jya_Haheniron_Profile = {
    /**/ ACTOR_BG_JYA_HAHENIRON,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_JYA_IRON,
    /**/ sizeof(BgJyaHaheniron),
    /**/ Bg_Jya_Haheniron_actor_ct,
    /**/ Bg_Jya_Haheniron_actor_dt,
    /**/ Bg_Jya_Haheniron_actor_move,
    /**/ Bg_Jya_Haheniron_actor_draw,
};

static ColliderJntSphElementInit ClSphElemDt_haheniron[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit ClSphDt_haheniron = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    ClSphElemDt_haheniron,
};

static s16 s[] = { 5, 8, 11, 14, 17 };

static InitChainEntry value_init[] = {
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),      ICHAIN_F32_DIV1000(minVelocityY, -15000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE), ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static f32 scale_data[] = { 0.13f, 0.1f, 0.1f };

void set_collision_data_haheniron(BgJyaHaheniron* this, PlayState* play) {
    s32 pad;

    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &ClSphDt_haheniron, this->colliderItems);
}

void set_effect_haheniron(PlayState* play, Vec3f* vec1, Vec3f* vec2) {
    Vec3f vel;
    Vec3f pos;
    s16 arg5;
    s32 angle;
    s32 i;
    f32 rand1;

    for (angle = 0, i = 0; i < ARRAY_COUNT(s); i++) {
        rand1 = fqrand() * 10.0f;
        vel.x = (sin_s(angle) * rand1) + vec2->x;
        vel.y = (fqrand() * 10.0f) + vec2->y;
        vel.z = (cos_s(angle) * rand1) + vec2->z;

        rand1 = fqrand();
        if (rand1 < 0.2f) {
            arg5 = 96;
        } else if (rand1 < 0.8f) {
            arg5 = 64;
        } else {
            arg5 = 32;
        }

        Effect_Kakera_ct2(play, vec1, &vel, vec1, -350, arg5, 40, 4, 0, s[i], 0, 20, 40,
                             KAKERA_COLOR_NONE, OBJECT_JYA_IRON, gObjectJyaIronDL_000880);
        angle += 0x3333;
    }
    pos.x = vec1->x + (vec2->x * 5.0f);
    pos.y = vec1->y + (vec2->y * 5.0f);
    pos.z = vec1->z + (vec2->z * 5.0f);
    dust_fly_set2(play, &pos, 100.0f, 4, 100, 160, 1);
}

void Bg_Jya_Haheniron_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgJyaHaheniron* this = (BgJyaHaheniron*)thisx;

    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, scale_data[this->actor.params]);
    if (this->actor.params == 0) {
        set_collision_data_haheniron(this, play);
        this->actor.shape.rot.z = (fqrand() * 65535.0f);
        mv_type_at_init(this);
    } else if (this->actor.params == 1) {
        mv_type_upper_init(this);
    } else if (this->actor.params == 2) {
        mv_type_lower_init(this);
    }
}

void Bg_Jya_Haheniron_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgJyaHaheniron* this = (BgJyaHaheniron*)thisx;

    if (this->actor.params == 0) {
        ClObjJntSph_dt_nzf(play, &this->collider);
    }
}

void mv_type_at_init(BgJyaHaheniron* this) {
    this->actionFunc = mv_type_at;
}

void mv_type_at(BgJyaHaheniron* this, PlayState* play) {
    Vec3f vec;

    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 5.0f, 8.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_7);
    if ((this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_WALL)) ||
        ((this->collider.base.atFlags & AT_HIT) && (this->collider.base.at != NULL) &&
         (this->collider.base.at->category == ACTORCAT_PLAYER))) {
        vec.x = -fqrand() * this->actor.velocity.x;
        vec.y = -fqrand() * this->actor.velocity.y;
        vec.z = -fqrand() * this->actor.velocity.z;
        set_effect_haheniron(play, &this->actor.world.pos, &vec);
        Actor_delete(&this->actor);
    } else if (this->timer > 60) {
        Actor_delete(&this->actor);
    } else {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }
    this->actor.shape.rot.y += 0x4B0;
    this->actor.shape.rot.x += 0xFA0;
}

void mv_type_upper_init(BgJyaHaheniron* this) {
    this->actionFunc = mv_type_upper;
}

void mv_type_upper(BgJyaHaheniron* this, PlayState* play) {
    static Vec3f spd[] = { 0.0f, 14.0f, 0.0f };

    if (this->timer >= 8) {
        Actor_position_moveF(&this->actor);
    } else if (this->timer >= 17) {
        set_effect_haheniron(play, &this->actor.world.pos, spd);
        Actor_delete(&this->actor);
    }
    this->actor.shape.rot.y += 0x258;
    this->actor.shape.rot.x += 0x3E8;
}

void mv_type_lower_init(BgJyaHaheniron* this) {
    this->actionFunc = mv_type_lower;
}

void mv_type_lower(BgJyaHaheniron* this, PlayState* play) {
    static Vec3f spd[] = { 0.0f, 8.0f, 0.0f };

    if (this->timer >= 17) {
        set_effect_haheniron(play, &this->actor.world.pos, spd);
        Effect_SE_Info_new(play, &this->actor.world.pos, 80, NA_SE_EN_IRONNACK_BREAK_PILLAR2);
        Actor_delete(&this->actor);
    }
}

void Bg_Jya_Haheniron_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgJyaHaheniron* this = (BgJyaHaheniron*)thisx;

    this->timer++;
    this->actionFunc(this, play);
}

void Bg_Jya_Haheniron_actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* model[] = {
        gObjectJyaIronDL_000880,
        gObjectJyaIronDL_000AE0,
        gObjectJyaIronDL_000600,
    };
    s32 pad;
    BgJyaHaheniron* this = (BgJyaHaheniron*)thisx;

    if (this->actor.params == 0) {
        CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
    }
    Cheap_gfx_display(play, model[this->actor.params]);
}
