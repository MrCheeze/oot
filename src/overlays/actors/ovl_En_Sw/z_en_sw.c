#include "z_en_sw.h"
#include "assets/objects/object_st/object_st.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Sw_Actor_ct(Actor* thisx, PlayState* play);
void En_Sw_Actor_dt(Actor* thisx, PlayState* play);
void En_Sw_Actor_move(Actor* thisx, PlayState* play);
void En_Sw_Actor_draw(Actor* thisx, PlayState* play);
s32 sw_bgcheck(EnSw* this, PlayState* play);
void go_out_init(EnSw* this, PlayState* play);
void sw_matsu(EnSw* this, PlayState* play);
void normal_wait(EnSw* this, PlayState* play);
void sw_stop(EnSw* this, PlayState* play);
void sw_modoru(EnSw* this, PlayState* play);
void sw_totugeki(EnSw* this, PlayState* play);
static void dead(EnSw* this, PlayState* play);
s32 paste_check(EnSw* this, PlayState* play, s32);
void go_out(EnSw* this, PlayState* play);
void fall(EnSw* this, PlayState* play);
void madness(EnSw* this, PlayState* play);

ActorProfile En_Sw_Profile = {
    /**/ ACTOR_EN_SW,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ST,
    /**/ sizeof(EnSw),
    /**/ En_Sw_Actor_ct,
    /**/ En_Sw_Actor_dt,
    /**/ En_Sw_Actor_move,
    /**/ En_Sw_Actor_draw,
};

static ColliderJntSphElementInit SwJntSphElemData[1] = {
    {
        { ELEM_MATERIAL_UNK0, { 0xFFCFFFFF, 0x00, 0x08 }, { 0xFFC3FFFE, 0x00, 0x00 }, 0x01, 0x05, 0x01 },
        { 2, { { 0, -300, 0 }, 21 }, 100 },
    },
};

static ColliderJntSphInit SwAllJntSphData = {
    { COL_MATERIAL_HIT6, 0x11, 0x09, 0x39, 0x10, COLSHAPE_JNTSPH },
    1,
    SwJntSphElemData,
};

static CollisionCheckInfoInit2 SwStatusData = { 1, 2, 25, 25, MASS_IMMOVABLE };

typedef enum EnSwAnimation {
    /* 0 */ ENSW_ANIM_0,
    /* 1 */ ENSW_ANIM_1,
    /* 2 */ ENSW_ANIM_2,
    /* 3 */ ENSW_ANIM_3
} EnSwAnimation;

static AnimationInfo anime_ct_data[] = {
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, 0x01, 0.0f },
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, 0x01, -8.0f },
    { &object_st_Anim_0055A8, 1.0f, 0.0f, -1.0f, 0x01, -8.0f },
    { &object_st_Anim_005B98, 1.0f, 0.0f, -1.0f, 0x01, -8.0f },
};

char projection[0x80]; // unused

static void vector_mult(Vec3f* a, Vec3f* b, Vec3f* dst) {
    dst->x = (a->y * b->z) - (a->z * b->y);
    dst->y = (a->z * b->x) - (a->x * b->z);
    dst->z = (a->x * b->y) - (a->y * b->x);
}

static s32 set_new_shape_angle(EnSw* this, CollisionPoly* poly) {
    Vec3f polyNormal;
    Vec3f sp38;
    f32 sp34;
    f32 temp_f0;
    s32 pad;

    this->actor.floorPoly = poly;
    polyNormal.x = COLPOLY_GET_NORMAL(poly->normal.x);
    polyNormal.y = COLPOLY_GET_NORMAL(poly->normal.y);
    polyNormal.z = COLPOLY_GET_NORMAL(poly->normal.z);
    sp34 = facos(DOTXYZ(polyNormal, this->unk_364));
    vector_mult(&this->unk_364, &polyNormal, &sp38);
    Matrix_rotateVector(sp34, &sp38, MTXMODE_NEW);
    Matrix_Position(&this->unk_370, &sp38);
    this->unk_370 = sp38;
    vector_mult(&this->unk_370, &polyNormal, &this->unk_37C);
    temp_f0 = Math3DVecLength(&this->unk_37C);
    if (temp_f0 < 0.001f) {
        return 0;
    }
    this->unk_37C.x *= 1.0f / temp_f0;
    this->unk_37C.y *= 1.0f / temp_f0;
    this->unk_37C.z *= 1.0f / temp_f0;
    this->unk_364 = polyNormal;
    this->unk_3D8.xx = this->unk_370.x;
    this->unk_3D8.yx = this->unk_370.y;
    this->unk_3D8.zx = this->unk_370.z;
    this->unk_3D8.wx = 0.0f;
    this->unk_3D8.xy = this->unk_364.x;
    this->unk_3D8.yy = this->unk_364.y;
    this->unk_3D8.zy = this->unk_364.z;
    this->unk_3D8.wy = 0.0f;
    this->unk_3D8.xz = this->unk_37C.x;
    this->unk_3D8.yz = this->unk_37C.y;
    this->unk_3D8.zz = this->unk_37C.z;
    this->unk_3D8.wz = 0.0f;
    this->unk_3D8.xw = 0.0f;
    this->unk_3D8.yw = 0.0f;
    this->unk_3D8.zw = 0.0f;
    this->unk_3D8.ww = 1.0f;
    Matrix_to_rotate_new(&this->unk_3D8, &this->actor.world.rot, 0);
    //! @bug Does not return, but the return value is not used by any caller so it doesn't matter.
}

CollisionPoly* StBGCheck(PlayState* play, Vec3f* arg1, Vec3f* arg2, Vec3f* arg3, s32* arg4) {
    CollisionPoly* sp3C;
    s32 pad;

    if (!T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, arg1, arg2, arg3, &sp3C, true, true, true, false, arg4)) {
        return NULL;
    }

    if (T_BGCheck_getWallCodeBit_ai(&play->colCtx, sp3C, *arg4) & WALL_FLAG_CRAWLSPACE) {
        return NULL;
    }

    if (T_BGCheck_CheckArrowNoHit(&play->colCtx, sp3C, *arg4)) {
        return NULL;
    }

    return sp3C;
}

s32 paste_check(EnSw* this, PlayState* play, s32 arg2) {
    CollisionPoly* temp_v0_2;
    CollisionPoly* temp_s1;
    Vec3f sp9C;
    Vec3f sp90;
    Vec3f sp84;
    Vec3f sp78;
    s32 pad;
    s32 sp70;
    s32 sp6C;
    s32 phi_s1;
    s32 sp64;

    sp64 = 0;
    this->unk_42C = 1;
    sp84 = sp78 = this->actor.world.pos;
    sp84.x += this->unk_364.x * 18.0f;
    sp84.y += this->unk_364.y * 18.0f;
    sp84.z += this->unk_364.z * 18.0f;
    sp78.x -= this->unk_364.x * 18.0f;
    sp78.y -= this->unk_364.y * 18.0f;
    sp78.z -= this->unk_364.z * 18.0f;
    temp_s1 = StBGCheck(play, &sp84, &sp78, &sp90, &sp70);

    if ((temp_s1 != NULL) && (this->unk_360 == 0)) {
        sp78.x = sp84.x + (this->unk_37C.x * 24);
        sp78.y = sp84.y + (this->unk_37C.y * 24);
        sp78.z = sp84.z + (this->unk_37C.z * 24);
        temp_v0_2 = StBGCheck(play, &sp84, &sp78, &sp9C, &sp6C);
        if (temp_v0_2 != NULL) {
            if (arg2 == 1) {
                set_new_shape_angle(this, temp_v0_2);
                this->actor.world.pos = sp9C;
                this->actor.floorBgId = sp6C;
            }
        } else {
            if (this->actor.floorPoly != temp_s1) {
                set_new_shape_angle(this, temp_s1);
            }
            this->actor.world.pos = sp90;
            this->actor.floorBgId = sp70;
        }
        sp64 = 1;
    } else {
        sp84 = sp78;
        for (phi_s1 = 0; phi_s1 < 3; phi_s1++) {
            if (phi_s1 == 0) {
                sp78.x = sp84.x - (this->unk_37C.x * 24.0f);
                sp78.y = sp84.y - (this->unk_37C.y * 24.0f);
                sp78.z = sp84.z - (this->unk_37C.z * 24.0f);
            } else if (phi_s1 == 1) {
                sp78.x = sp84.x + (this->unk_370.x * 24.0f);
                sp78.y = sp84.y + (this->unk_370.y * 24.0f);
                sp78.z = sp84.z + (this->unk_370.z * 24.0f);
            } else {
                sp78.x = sp84.x - (this->unk_370.x * 24.0f);
                sp78.y = sp84.y - (this->unk_370.y * 24.0f);
                sp78.z = sp84.z - (this->unk_370.z * 24.0f);
            }
            temp_v0_2 = StBGCheck(play, &sp84, &sp78, &sp9C, &sp6C);
            if (temp_v0_2 == NULL) {
                continue;
            }

            if (arg2 == 1) {
                set_new_shape_angle(this, temp_v0_2);
                this->actor.world.pos = sp9C;
                this->actor.floorBgId = sp6C;
            }
            sp64 = 1;
            break;
        }
    }

    add_calc_short_angle2(&this->actor.shape.rot.x, this->actor.world.rot.x, 8, 0xFA0, 1);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.world.rot.y, 8, 0xFA0, 1);
    add_calc_short_angle2(&this->actor.shape.rot.z, this->actor.world.rot.z, 8, 0xFA0, 1);

    return sp64;
}

void En_Sw_Actor_ct(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;
    s32 phi_v0;
    Vec3f sp4C = { 0.0f, 0.0f, 0.0f };
    s32 pad;

    if (PARAMS_GET_NOSHIFT(thisx->params, 15, 1)) {
        phi_v0 = PARAMS_GET_S(thisx->params - 0x8000, 13, 3) + 1;
        thisx->params = PARAMS_GET_S(thisx->params, 0, 13) | (phi_v0 << 0xD);
    }

    if (PARAMS_GET_S(thisx->params, 13, 3) > 0) {
        phi_v0 = PARAMS_GET_S(thisx->params, 8, 5) - 1;
        thisx->params = (thisx->params & ~(0x1F << 8)) | (phi_v0 << 8);
    }

    // Check to see if this gold skull token has already been retrieved.
    if (GET_GS_FLAGS(PARAMS_GET_S(thisx->params, 8, 5)) & PARAMS_GET_S(thisx->params, 0, 8)) {
        Actor_delete(&this->actor);
        return;
    }

    Skeleton_Info2_M_ct(play, &this->skelAnime, &object_st_Skel_005298, NULL, this->jointTable, this->morphTable, 30);
    npc_anime_ct(&this->skelAnime, anime_ct_data, ENSW_ANIM_0);
    Shape_Info_init(&thisx->shape, 0.0f, NULL, 0.0f);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &SwAllJntSphData, this->sphs);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, CollisionBtlTbl_get(0xE), &SwStatusData);
    this->actor.scale.x = 0.02f;

    if (PARAMS_GET_S(thisx->params, 13, 3) == 0) {
        this->actor.world.rot.x = 0;
        this->actor.world.rot.z = 0;
        thisx->shape.rot = this->actor.world.rot;
        this->unk_484.y = this->actor.world.pos.y;
        this->unk_484.x = this->actor.world.pos.x + (sin_s(this->actor.world.rot.y) * -60.0f);
        this->unk_484.z = this->actor.world.pos.z + (cos_s(this->actor.world.rot.y) * -60.0f);
        sw_bgcheck(this, play);
        this->actor.home.pos = this->actor.world.pos;
    } else {
        this->unk_370.x = sin_s(thisx->shape.rot.y + 0x4000);
        this->unk_370.y = 0.0f;
        this->unk_370.z = cos_s(thisx->shape.rot.y + 0x4000);
        this->unk_364.x = 0.0f;
        this->unk_364.y = 1.0f;
        this->unk_364.z = 0.0f;
        this->unk_37C.x = sin_s(thisx->shape.rot.y);
        this->unk_37C.y = 0.0f;
        this->unk_37C.z = cos_s(thisx->shape.rot.y);
        paste_check(this, play, 1);
    }

    if (PARAMS_GET_S(thisx->params, 13, 3) >= 3) {
        Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }

    switch (PARAMS_GET_S(thisx->params, 13, 3)) {
        case 3:
        case 4:
            this->unk_360 = 1;
            this->actor.velocity.y = 8.0f;
            this->actor.speed = 4.0f;
            this->actor.gravity = -1.0f;
            FALLTHROUGH;
        case 2:
            this->actor.scale.x = 0.0f;
            FALLTHROUGH;
        case 1:
            this->collider.elements[0].base.atDmgInfo.damage *= 2;
            this->actor.naviEnemyId = NAVI_ENEMY_GOLD_SKULLTULA;
            this->actor.colChkInfo.health *= 2;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            break;
        default:
            Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
            this->actor.naviEnemyId = NAVI_ENEMY_SKULLWALLTULA;
            break;
    }

    this->unk_38E = get_random_timer(0xF, 0x1E);
    Actor_set_scale(&this->actor, this->actor.scale.x);
    this->actor.home.pos = this->actor.world.pos;
    thisx->shape.rot = this->actor.world.rot;

    if (PARAMS_GET_S(thisx->params, 13, 3) >= 3) {
        this->unk_38C = 0x28;
        this->unk_394 = 1;
        this->actionFunc = go_out_init;
    } else if (PARAMS_GET_S(thisx->params, 13, 3) == 0) {
        this->actionFunc = sw_matsu;
    } else {
        this->actionFunc = normal_wait;
    }
}

void En_Sw_Actor_dt(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

static s32 hitcheck(EnSw* this, PlayState* play) {
    s32 phi_v1 = false;

    if (this->actor.xyzDistToPlayerSq < SQ(400.0f) && PARAMS_GET_S(this->actor.params, 13, 3) == 0 &&
        play->actorCtx.unk_02 != 0) {

        this->actor.colChkInfo.damage = this->actor.colChkInfo.health;
        phi_v1 = true;
    }

    if (this->unk_392 == 0) {
        if ((this->collider.base.acFlags & AC_HIT) || phi_v1) {
            this->collider.base.acFlags &= ~AC_HIT;
            this->unk_392 = 0x10;
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA, this->unk_392);
            if (hp_down(&this->actor) != 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_STALTU_DAMAGE);
                return true;
            }
            Actor_info_finish(play, &this->actor);
            if (PARAMS_GET_S(this->actor.params, 13, 3) != 0) {
                this->skelAnime.playSpeed = 8.0f;
                if ((play->state.frames & 1) == 0) {
                    this->unk_420 = 0.1f;
                } else {
                    this->unk_420 = -0.1f;
                }
                this->unk_394 = 0xA;
                this->unk_38A = 1;
                this->unk_420 *= 4.0f;
                this->actionFunc = madness;
            } else {
                this->actor.shape.shadowDraw = Actor_shadow_circle;
                this->actor.shape.shadowAlpha = 0xFF;
                this->unk_38A = 2;
                this->actor.shape.shadowScale = 16.0f;
                this->actor.gravity = -1.0f;
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->actionFunc = fall;
            }

            Actor_SE_set(&this->actor, NA_SE_EN_STALWALL_DEAD);
            return true;
        }
    }

    if ((this->unk_390 == 0) && (this->collider.base.atFlags & AT_HIT)) {
        this->unk_390 = 30;
    }

    return false;
}

void collision_set(EnSw* this, PlayState* play) {
    if ((PARAMS_GET_S(this->actor.params, 13, 3) > 0) && (this->actionFunc != normal_wait)) {
        if (this->unk_392 != 0) {
            this->unk_392--;
        }
    } else {
        if ((DECR(this->unk_390) == 0) && (this->actor.colChkInfo.health != 0)) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }

        if ((DECR(this->unk_392) == 0) && (this->actor.colChkInfo.health != 0)) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }

        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

static s32 turn(EnSw* this, f32* arg1) {
    CollisionPoly* floorPoly;
    f32 temp_f0;
    Vec3f floorPolyNormal;
    MtxF sp2C;

    if (this->actor.floorPoly == NULL) {
        return false;
    }

    floorPoly = this->actor.floorPoly;
    floorPolyNormal.x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
    floorPolyNormal.y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
    floorPolyNormal.z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
    Matrix_rotateVector(*arg1, &floorPolyNormal, MTXMODE_NEW);
    Matrix_Position(&this->unk_370, &floorPolyNormal);
    this->unk_370 = floorPolyNormal;
    vector_mult(&this->unk_370, &this->unk_364, &this->unk_37C);
    temp_f0 = Math3DVecLength(&this->unk_37C);
    if (temp_f0 < 0.001f) {
        return false;
    }
    temp_f0 = 1.0f / temp_f0;
    this->unk_37C.x *= temp_f0;
    this->unk_37C.y *= temp_f0;
    this->unk_37C.z *= temp_f0;
    sp2C.xx = this->unk_370.x;
    sp2C.yx = this->unk_370.y;
    sp2C.zx = this->unk_370.z;
    sp2C.wx = 0.0f;
    sp2C.xy = this->unk_364.x;
    sp2C.yy = this->unk_364.y;
    sp2C.zy = this->unk_364.z;
    sp2C.wy = 0.0f;
    sp2C.xz = this->unk_37C.x;
    sp2C.yz = this->unk_37C.y;
    sp2C.zz = this->unk_37C.z;
    sp2C.wz = 0.0f;
    sp2C.xw = 0.0f;
    sp2C.yw = 0.0f;
    sp2C.zw = 0.0f;
    sp2C.ww = 1.0f;
    Matrix_to_rotate_new(&sp2C, &this->actor.world.rot, 0);
    return true;
}

void sw_roll_se_set(EnSw* this, PlayState* play) {
    if (!(this->actor.scale.x < 0.0139999995f)) {
        Camera* activeCam = GET_ACTIVE_CAM(play);

        if (!(search_position_distance(&this->actor.world.pos, &activeCam->eye) >= 380.0f)) {
            Actor_SE_set(&this->actor, (PARAMS_GET_S(this->actor.params, 13, 3) > 0) ? NA_SE_EN_STALGOLD_ROLL
                                                                                      : NA_SE_EN_STALWALL_ROLL);
        }
    }
}

void set_dust_effect1(EnSw* this, PlayState* play, s32 cnt) {
    Color_RGBA8 primColor = { 80, 80, 50, 255 };
    Color_RGBA8 envColor = { 100, 100, 80, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = (fqrand() - 0.5f) * 65536.0f;
    s32 i;

    for (i = cnt; i >= 0; i--, angle += (s16)(0x10000 / cnt)) {
        accel.x = (fqrand() - 0.5f) * 2.0f;
        accel.z = (fqrand() - 0.5f) * 2.0f;
        pos.x = this->actor.world.pos.x + (sin_s(angle) * 2.0f);
        pos.y = this->actor.world.pos.y;
        pos.z = this->actor.world.pos.z + (cos_s(angle) * 2.0f);
        Effect_SS_Dust_sc_cl_co_ct(play, &pos, &velocity, &accel, &primColor, &envColor, 20, 30, 12);
    }
}

void set_dust_effect2(EnSw* this, PlayState* play, s32 cnt) {
    Color_RGBA8 primColor = { 80, 80, 50, 255 };
    Color_RGBA8 envColor = { 100, 100, 80, 0 };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos;
    s16 angle = (fqrand() - 0.5f) * 65536.0f;
    s32 i;

    for (i = cnt; i >= 0; i--, angle += (s16)(0x10000 / cnt)) {
        accel.x = (fqrand() - 0.5f) * 2.0f;
        accel.z = (fqrand() - 0.5f) * 2.0f;
        pos.x = this->actor.world.pos.x + (sin_s(angle) * 14.0f);
        pos.y = this->actor.world.pos.y;
        pos.z = this->actor.world.pos.z + (cos_s(angle) * 14.0f);
        Effect_SS_Dust_sc_cl_co_ct(play, &pos, &velocity, &accel, &primColor, &envColor, 20, 40, 10);
    }
}

void go_out_init(EnSw* this, PlayState* play) {
    if (PARAMS_GET_S(this->actor.params, 13, 3) == 4) {
        this->unk_38C = 0;
        this->actionFunc = go_out;
    } else {
        this->unk_38C = 10;
        this->actionFunc = go_out;
    }
}

void go_out(EnSw* this, PlayState* play) {
    if (this->unk_38C != 0) {
        if ((this->unk_38C & 4) != 0) {
            set_dust_effect1(this, play, 5);
        }
        this->unk_38C--;
        if (this->unk_38C == 0) {
            Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EN_STALGOLD_UP_CRY);
            Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EN_DODO_M_UP);
        } else {
            return;
        }
    }

    add_calc2(&this->actor.scale.x, 0.02f, 0.2f, 0.01f);
    Actor_set_scale(&this->actor, this->actor.scale.x);
    this->actor.world.pos.x += this->unk_364.x * this->actor.velocity.y;
    this->actor.world.pos.y += this->unk_364.y * this->actor.velocity.y;
    this->actor.world.pos.z += this->unk_364.z * this->actor.velocity.y;
    this->actor.world.pos.x += this->unk_37C.x * this->actor.speed;
    this->actor.world.pos.y += this->unk_37C.y * this->actor.speed;
    this->actor.world.pos.z += this->unk_37C.z * this->actor.speed;
    this->actor.velocity.y += this->actor.gravity;
    this->actor.velocity.y = CLAMP_MIN(this->actor.velocity.y, this->actor.minVelocityY);

    if (this->actor.velocity.y < 0.0f) {
        this->unk_360 = 0;
    }

    if (paste_check(this, play, 1) == 1) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        set_dust_effect2(this, play, 8);
        this->actor.scale.x = 0.02f;
        Actor_set_scale(&this->actor, 0.02f);
        this->actionFunc = normal_wait;
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
        this->actor.gravity = 0.0f;
    }
}

void normal_wait(EnSw* this, PlayState* play) {
    f32 sp2C;

    if (PARAMS_GET_S(this->actor.params, 13, 3) == 2) {
        if (this->actor.scale.x < 0.0139999995f) {
            this->collider.elements[0].base.atElemFlags = ATELEM_NONE;
            this->collider.elements[0].base.acElemFlags = ACELEM_NONE;
            this->collider.elements[0].base.ocElemFlags = OCELEM_NONE;
        }

        if (this->actor.scale.x >= 0.0139999995f) {
            this->collider.elements[0].base.atElemFlags = ATELEM_ON;
            this->collider.elements[0].base.acElemFlags = ACELEM_ON;
            this->collider.elements[0].base.ocElemFlags = OCELEM_ON;
        }

        add_calc2(&this->actor.scale.x, !IS_DAY ? 0.02f : 0.0f, 0.2f, 0.01f);
        Actor_set_scale(&this->actor, this->actor.scale.x);
    }

    if (this->unk_38E != 0) {
        this->unk_38E--;
        if (this->unk_38E == 0) {
            sw_roll_se_set(this, play);
            this->unk_420 = ((play->state.frames % 2) == 0) ? 0.1f : -0.1f;
            this->unk_38A = 1;
            this->unk_38C = get_random_timer(30, 60);
            if (PARAMS_GET_S(this->actor.params, 13, 3) != 0) {
                this->unk_38C *= 2;
                this->unk_420 *= 2.0f;
            }
        }
    } else {
        this->unk_38C--;
        if (this->unk_38C == 0) {
            this->unk_38E = get_random_timer(15, 30);
            this->unk_38A = 0;
            this->skelAnime.playSpeed = 0.0f;
            if (PARAMS_GET_S(this->actor.params, 13, 3) != 0) {
                this->unk_38E /= 2;
            }
        } else if (this->unk_38A != 0) {
            this->unk_38A--;
            this->skelAnime.playSpeed = (this->unk_38A == 0) ? 4.0f : 0.0f;

            if (this->skelAnime.playSpeed > 0.0f) {
                sw_roll_se_set(this, play);
            }
            if (PARAMS_GET_S(this->actor.params, 13, 3) != 0) {
                this->skelAnime.playSpeed *= 2.0f;
            }
        } else {
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame) == 1) {
                this->unk_38A = 2;
            }
            sp2C = 32768.0f / this->skelAnime.endFrame;
            sp2C *= this->skelAnime.curFrame;
            sp2C = sin_s(sp2C) * this->unk_420;
            turn(this, &sp2C);
            this->actor.shape.rot = this->actor.world.rot;
        }
    }
}

void madness(EnSw* this, PlayState* play) {
    Actor* temp_v0;
    Vec3f pos;
    Vec3f velAndAccel = { 0.0f, 0.5f, 0.0f };
    f32 x;
    f32 y;
    f32 z;

    if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame) == 1) {
        sw_roll_se_set(this, play);
    }

    turn(this, &this->unk_420);
    this->actor.shape.rot = this->actor.world.rot;

    if ((this->unk_394 == 0) && (this->unk_392 == 0)) {
        Nai_FxFlagEntry(NA_SE_SY_KINSTA_MARK_APPEAR, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        x = (this->unk_364.x * 10.0f);
        y = (this->unk_364.y * 10.0f);
        z = (this->unk_364.z * 10.0f);
        temp_v0 =
            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_SI, this->actor.world.pos.x + x,
                               this->actor.world.pos.y + y, this->actor.world.pos.z + z, 0, 0, 0, this->actor.params);
        if (temp_v0 != NULL) {
            temp_v0->parent = NULL;
        }
        Actor_delete(&this->actor);
        return;
    }

    if ((this->unk_392 == 0) && (DECR(this->unk_394) != 0)) {
        pos = this->actor.world.pos;
        pos.y += 10.0f + ((fqrand() - 0.5f) * 6.0f);
        pos.x += (fqrand() - 0.5f) * 32.0f;
        pos.z += (fqrand() - 0.5f) * 32.0f;
        _Effect_SS_Db_ct(play, &pos, &velAndAccel, &velAndAccel, 42, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, true);
    }
}

void fall(EnSw* this, PlayState* play) {
    Actor_position_moveF(&this->actor);
    this->actor.shape.rot.x += 0x1000;
    this->actor.shape.rot.z += 0x1000;
    Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && !(0.0f <= this->actor.velocity.y)) {
        if (this->actor.floorHeight <= BGCHECK_Y_MIN || this->actor.floorHeight >= 32000.0f) {
            Actor_delete(&this->actor);
            return;
        }

        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;

        if (this->unk_38A == 0) {
            this->actionFunc = dead;
            this->unk_394 = 10;
        } else {
            this->actor.velocity.y = ((this->unk_38A--) * 8.0f) * 0.5f;
        }

        Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
        _dust_ground_set(play, &this->actor, &this->actor.world.pos, 16.0f, 12, 2.0f, 120, 10, false);
    }
}

static void dead(EnSw* this, PlayState* play) {
    Vec3f velAndAccel = { 0.0f, 0.5f, 0.0f };
    Vec3f pos = { 0.0f, 0.0f, 0.0f };

    if (DECR(this->unk_394) != 0) {
        pos.y = ((fqrand() - 0.5f) * 6.0f) + (this->actor.world.pos.y + 10.0f);
        pos.x = ((fqrand() - 0.5f) * 32.0f) + this->actor.world.pos.x;
        pos.z = ((fqrand() - 0.5f) * 32.0f) + this->actor.world.pos.z;
        _Effect_SS_Db_ct(play, &pos, &velAndAccel, &velAndAccel, 42, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, 1);
        this->actor.shape.rot.x += 0x1000;
        this->actor.shape.rot.z += 0x1000;
    } else {
        Item_Set_Std(play, NULL, &this->actor.world.pos, 0x30);
        Actor_delete(&this->actor);
    }
}

s16 change_muki(EnSw* this, Vec3f* arg1) {
    s16 pitch;
    s16 yaw;

    yaw = search_position_angleY(&this->actor.world.pos, arg1) - this->actor.wallYaw;
    pitch = search_position_angleX(&this->actor.world.pos, arg1) - 0x4000;
    return pitch * (yaw >= 0 ? -1 : 1);
}

s32 check_player_status(EnSw* this, PlayState* play, s32 arg2) {
    Player* player = GET_PLAYER(play);
    CollisionPoly* sp58;
    s32 sp54;
    Vec3f sp48;

    if (!(player->stateFlags1 & PLAYER_STATE1_21) && arg2) {
        return false;
    } else if (player_climb_stop_check(play) && arg2) {
        return false;
    } else if (ABS(change_muki(this, &player->actor.world.pos) - this->actor.shape.rot.z) >= 0x1FC2) {
        return false;
    } else if (search_position_distance(&this->actor.world.pos, &player->actor.world.pos) >= 130.0f) {
        return false;
    } else if (!T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &player->actor.world.pos, &sp48, &sp58,
                                        true, false, false, true, &sp54)) {
        return true;
    } else {
        return false;
    }
}

s32 sw_bgcheck(EnSw* this, PlayState* play) {
    s32 pad;
    CollisionPoly* sp60;
    s32 sp5C;
    Vec3f sp50;
    s32 sp4C = true;

    if (this->collider.base.ocFlags1 & OC1_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        sp4C = false;
    } else if (((play->state.frames % 4) == 0) &&
               !T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &this->unk_454, &sp50, &sp60, true,
                                        false, false, true, &sp5C)) {
        sp4C = false;
    } else if (((play->state.frames % 4) == 1) &&
               T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &this->unk_460, &sp50, &sp60, true, false,
                                       false, true, &sp5C)) {
        sp4C = false;
    } else if (((play->state.frames % 4) == 2) &&
               !T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &this->unk_46C, &sp50, &sp60, true,
                                        false, false, true, &sp5C)) {
        if (0) {}
        sp4C = false;
    } else if (((play->state.frames % 4) == 3) &&
               T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &this->unk_478, &sp50, &sp60, true, false,
                                       false, true, &sp5C)) {
        sp4C = false;
    }

    if (T_BGCheck_ObjLineCheck_poly_chgrp_ai(&play->colCtx, &this->actor.world.pos, &this->unk_484, &sp50, &this->unk_430, true,
                                false, false, true, &sp5C)) {
        this->actor.wallYaw = RAD_TO_BINANG(fatan2(this->unk_430->normal.x, this->unk_430->normal.z));
        this->actor.world.pos = sp50;
        this->actor.world.pos.x += 6.0f * sin_s(this->actor.world.rot.y);
        this->actor.world.pos.z += 6.0f * cos_s(this->actor.world.rot.y);
        this->unk_434 = sp50;
        this->unk_434.x += sin_s(this->actor.world.rot.y);
        this->unk_434.z += cos_s(this->actor.world.rot.y);
    }

    return sp4C;
}

void wall_move(EnSw* this, Vec3f arg1, f32 arg4) {
    f32 xDist;
    f32 yDist;
    f32 zDist;
    f32 dist;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;

    add_calc(&this->actor.speed, arg4, 0.3f, 100.0f, 0.1f);
    xDiff = arg1.x - this->actor.world.pos.x;
    yDiff = arg1.y - this->actor.world.pos.y;
    zDiff = arg1.z - this->actor.world.pos.z;
    dist = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));
    if (dist == 0.0f) {
        xDist = yDist = zDist = 0.0f;
    } else {
        xDist = xDiff / dist;
        yDist = yDiff / dist;
        zDist = zDiff / dist;
    }
    xDist *= this->actor.speed;
    yDist *= this->actor.speed;
    zDist *= this->actor.speed;
    this->actor.world.pos.x += xDist;
    this->actor.world.pos.y += yDist;
    this->actor.world.pos.z += zDist;
}

s32 sw_turn(EnSw* this, f32 arg1, s16 arg2, s32 arg3, PlayState* play) {
    Camera* activeCam;
    f32 lastFrame = Si2_anime_end_frame(&object_st_Anim_000304);

    if (DECR(this->unk_388) != 0) {
        add_calc(&this->skelAnime.playSpeed, 0.0f, 0.6f, 1000.0f, 0.01f);
        return 0;
    }

    add_calc(&this->skelAnime.playSpeed, arg1, 0.6f, 1000.0f, 0.01f);

    if ((arg3 == 1) && (lastFrame < (this->skelAnime.curFrame + this->skelAnime.playSpeed))) {
        return 0;
    }

    activeCam = GET_ACTIVE_CAM(play);

    if (search_position_distance(&this->actor.world.pos, &activeCam->eye) < 380.0f) {
        if (DECR(this->unk_440) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_STALWALL_ROLL);
            this->unk_440 = 4;
        }
    } else {
        this->unk_440 = 0;
    }
    add_calc_short_angle2(&this->actor.shape.rot.z, this->unk_444, 4, arg2, arg2);
    this->actor.world.rot = this->actor.shape.rot;
    if (this->actor.shape.rot.z == this->unk_444) {
        return 1;
    }
    return 0;
}

void sw_matsu(EnSw* this, PlayState* play) {
    s32 pad[2];
    f32 rand;

    if (sw_turn(this, 6.0f, 0x3E8, 1, play)) {
        rand = fqrand();
        this->unk_444 =
            ((s16)(20000.0f * rand) + 0x2EE0) * (fqrand() >= 0.5f ? 1.0f : -1.0f) + this->actor.world.rot.z;
        this->unk_388 = get_random_timer(10, 30);
    }

    if ((DECR(this->unk_442) == 0) && (check_player_status(this, play, 1))) {
        Actor_SE_set(&this->actor, NA_SE_EN_STALWALL_LAUGH);
        this->unk_442 = 20;
        this->actionFunc = sw_totugeki;
    }
}

void sw_totugeki(EnSw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;

    if (DECR(this->unk_442) != 0) {
        if (check_player_status(this, play, 1)) {
            this->unk_448 = player->actor.world.pos;
            this->unk_448.y += 30.0f;
            this->unk_444 = change_muki(this, &this->unk_448);
            sw_turn(this, 6.0f, (u16)0xFA0, 0, play);
        } else {
            this->actionFunc = sw_matsu;
        }
    } else {
        if (!sw_bgcheck(this, play)) {
            this->unk_442 = get_random_timer(20, 10);
            this->unk_444 = change_muki(this, &this->actor.home.pos);
            this->unk_448 = this->actor.home.pos;
            this->actionFunc = sw_modoru;
        } else {
            wall_move(this, this->unk_448, 8.0f);

            if (DECR(this->unk_440) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_STALWALL_DASH);
                this->unk_440 = 4;
            }

            if (!(search_position_distance(&this->actor.world.pos, &this->unk_448) > 13.0f) || player_climb_stop_check(play)) {
                this->actionFunc = sw_stop;
            }
        }
    }
}

void sw_stop(EnSw* this, PlayState* play) {
    s32 pad;

    wall_move(this, this->unk_448, 0.0f);
    if (this->actor.speed == 0.0f) {
        this->unk_444 = change_muki(this, &this->actor.home.pos);
        this->unk_448 = this->actor.home.pos;
        this->actionFunc = sw_modoru;
    }
}

void sw_modoru(EnSw* this, PlayState* play) {
    s32 pad;

    if (sw_turn(this, 6.0f, 0x3E8, 0, play)) {
        wall_move(this, this->unk_448, 2.0f);
        if (!(search_position_distance(&this->actor.world.pos, &this->unk_448) > 4.0f)) {
            this->actionFunc = sw_matsu;
        }
    }
}

void En_Sw_Actor_move(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;

    Skeleton_Info2_anime_play(&this->skelAnime);
    hitcheck(this, play);
    this->actionFunc(this, play);
    collision_set(this, play);
}

s32 before_disp(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Vec3f sp7C = { 1400.0f, -2600.0f, -800.0f };
    Vec3f sp70 = { 1400.0f, -1600.0f, 0.0f };
    Vec3f sp64 = { -1400.0f, -2600.0f, -800.0f };
    Vec3f sp58 = { -1400.0f, -1600.0f, 0.0f };
    Vec3f sp4C = { 0.0, 0.0f, -600.0f };
    EnSw* this = (EnSw*)thisx;
    Vec3f sp3C = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_sw.c", 2084);

    if (PARAMS_GET_S(this->actor.params, 13, 3) != 0) {
        switch (limbIndex) {
            case 23:
                *dList = object_st_DL_004788;
                break;
            case 8:
                *dList = object_st_DL_0046F0;
                break;
            case 14:
                *dList = object_st_DL_004658;
                break;
            case 11:
                *dList = object_st_DL_0045C0;
                break;
            case 26:
                *dList = object_st_DL_004820;
                break;
            case 20:
                *dList = object_st_DL_0048B8;
                break;
            case 17:
                *dList = object_st_DL_004950;
                break;
            case 29:
                *dList = object_st_DL_0049E8;
                break;
            case 5:
                *dList = object_st_DL_003FB0;
                break;
            case 4:
                *dList = object_st_DL_0043D8;
                break;
        }
    }

    if (limbIndex == 1) {
        Matrix_Position(&sp7C, &this->unk_454);
        Matrix_Position(&sp70, &this->unk_460);
        Matrix_Position(&sp64, &this->unk_46C);
        Matrix_Position(&sp58, &this->unk_478);
        Matrix_Position(&sp4C, &this->unk_484);
    }

    if (limbIndex == 5) {
        Matrix_Position(&sp3C, &this->actor.focus.pos);
    }

    if (limbIndex == 4) {
        gDPSetEnvColor(POLY_OPA_DISP++, this->unk_1F4.r, this->unk_1F4.g, this->unk_1F4.b, 0);
    }

    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->collider);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_sw.c", 2145);

    return false;
}

void after_disp(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

void eff_set_fog(PlayState* play, Color_RGBA8* arg1, s16 arg2, s16 arg3) {
    f32 temp_f2;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_sw.c", 2181);

    temp_f2 = (11500.0f / arg3) * (arg3 - arg2);

    if (0.0f == temp_f2) {
        temp_f2 = 11500;
    }

    POLY_OPA_DISP = set_fog(POLY_OPA_DISP, arg1->r, arg1->g, arg1->b, arg1->a, 0, (s16)temp_f2);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_sw.c", 2197);
}

void eff_off_fog(PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_sw.c", 2205);

    POLY_OPA_DISP = game_play_set_fog(play, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_sw.c", 2207);
}

void En_Sw_Actor_draw(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;
    Color_RGBA8 sp30 = { 184, 0, 228, 255 };

    if (PARAMS_GET_S(this->actor.params, 13, 3) != 0) {
        Matrix_rotateX(DEG_TO_RAD(-80), MTXMODE_APPLY);
        if (this->actor.colChkInfo.health != 0) {
            Matrix_translate(0.0f, 0.0f, 200.0f, MTXMODE_APPLY);
        }
        Actor_HiliteReflect_set_init(&this->actor, play, 0);
    } else if (this->actionFunc == sw_totugeki) {
        eff_set_fog(play, &sp30, 0x14, 0x1E);
    }

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, before_disp,
                      after_disp, this);
    if (this->actionFunc == sw_totugeki) {
        eff_off_fog(play);
    }
}
