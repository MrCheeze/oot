#include "z_bg_jya_bombchuiwa.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
#include "assets/objects/object_jya_obj/object_jya_obj.h"
#define FLAGS ACTOR_FLAG_ATTENTION_ENABLED

void Bg_Jya_Bombchuiwa_actor_ct(Actor* thisx, PlayState* play);
void Bg_Jya_Bombchuiwa_actor_dt(Actor* thisx, PlayState* play2);
void Bg_Jya_Bombchuiwa_actor_move(Actor* thisx, PlayState* play);
void Bg_Jya_Bombchuiwa_actor_draw(Actor* thisx, PlayState* play);

static void mv_before(BgJyaBombchuiwa* this, PlayState* play);
void mv_before_init(BgJyaBombchuiwa* this, PlayState* play);
void mv_path(BgJyaBombchuiwa* this, PlayState* play);
void mv_path_init(BgJyaBombchuiwa* this, PlayState* play);
void mv_after_init(BgJyaBombchuiwa* this, PlayState* play);

ActorProfile Bg_Jya_Bombchuiwa_Profile = {
    /**/ ACTOR_BG_JYA_BOMBCHUIWA,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_JYA_OBJ,
    /**/ sizeof(BgJyaBombchuiwa),
    /**/ Bg_Jya_Bombchuiwa_actor_ct,
    /**/ Bg_Jya_Bombchuiwa_actor_dt,
    /**/ Bg_Jya_Bombchuiwa_actor_move,
    /**/ Bg_Jya_Bombchuiwa_actor_draw,
};

static ColliderJntSphElementInit ClSphElemDt_bombchuiwa[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { -300, 0, 0 }, 40 }, 100 },
    },
};

static ColliderJntSphInit ClSphDt_bombchuiwa = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    ClSphElemDt_bombchuiwa,
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_3, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

void set_collision_data_bombchuiwa(BgJyaBombchuiwa* this, PlayState* play) {
    s32 pad;

    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &ClSphDt_bombchuiwa, &this->colliderItems);
}

void reset_flag_disp(BgJyaBombchuiwa* this, u8 drawFlags) {
    this->drawFlags &= ~7;
    this->drawFlags |= drawFlags;
}

void Bg_Jya_Bombchuiwa_actor_ct(Actor* thisx, PlayState* play) {
    BgJyaBombchuiwa* this = (BgJyaBombchuiwa*)thisx;

    ValueSet_process(&this->actor, value_init);
    set_collision_data_bombchuiwa(this, play);
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 0, 6))) {
        mv_after_init(this, play);
    } else {
        mv_before_init(this, play);
    }
    Actor_world_to_eye(&this->actor, 0.0f);
}

void Bg_Jya_Bombchuiwa_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgJyaBombchuiwa* this = (BgJyaBombchuiwa*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

void set_effect_bombchuiwa(BgJyaBombchuiwa* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity;
    s16 scale;
    s16 arg5;
    s16 arg6;
    s16 arg7;
    s32 i;

    for (i = 0; i < 20; i++) {
        pos.x = fqrand() * 10.0f + this->actor.world.pos.x - 10.0f;
        pos.y = fqrand() * 40.0f + this->actor.world.pos.y - 20.0f;
        pos.z = fqrand() * 50.0f + this->actor.world.pos.z - 25.0f;
        velocity.x = fqrand() * 3.0f - 0.3f;
        velocity.y = fqrand() * 18.0f;
        velocity.z = (fqrand() - 0.5f) * 15.0f;
        scale = (s32)(fqrand() * 20.0f) + 1;
        if (scale > 10) {
            arg5 = 5;
        } else {
            arg5 = 1;
        }
        if (fqrand() < 0.4f) {
            arg5 |= 0x40;
            arg6 = 0xC;
            arg7 = 8;
        } else {
            arg5 |= 0x20;
            arg6 = 0xC;
            arg7 = 8;
            if (scale < 8) {
                arg6 = 0x46;
                arg7 = 0x28;
            }
        }
        Effect_Kakera_ct2(play, &pos, &velocity, &pos, -300, arg5, arg6, arg7, 0, scale, 1, 15, 80,
                             KAKERA_COLOR_NONE, OBJECT_JYA_OBJ, gBombiwaEffectDL);
    }
    dust_fly_set2(play, &this->actor.world.pos, 100.0f, 8, 100, 160, 0);
}

void mv_before_init(BgJyaBombchuiwa* this, PlayState* play) {
    this->actionFunc = mv_before;
    reset_flag_disp(this, 3);
    this->timer = 0;
}

static void mv_before(BgJyaBombchuiwa* this, PlayState* play) {
    if ((this->collider.base.acFlags & AC_HIT) || (this->timer > 0)) {
        if (this->timer == 0) {
            makeOnepointDemo(play, 3410, -99, &this->actor, CAM_ID_MAIN);
        }
        this->timer++;
        if (this->timer > 10) {
            set_effect_bombchuiwa(this, play);
            mv_path_init(this, play);
            Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EV_WALL_BROKEN);
        }
    } else {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void mv_path_init(BgJyaBombchuiwa* this, PlayState* play) {
    this->actionFunc = mv_path;
    reset_flag_disp(this, 4);
    this->lightRayIntensity = 0.3f;
    this->timer = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void mv_path(BgJyaBombchuiwa* this, PlayState* play) {
    this->timer++;
    if (this->timer & 4) {
        dust_fly_set2(play, &this->actor.world.pos, 60.0f, 3, 100, 100, 0);
    }
    if (chase_f(&this->lightRayIntensity, 1.0f, 0.028)) {
        mv_after_init(this, play);
    }
}

void mv_after_init(BgJyaBombchuiwa* this, PlayState* play) {
    this->actionFunc = NULL;
    this->lightRayIntensity = 153.0f;
    reset_flag_disp(this, 4);
    if (Actor_info_make_actor(&play->actorCtx, play, ACTOR_MIR_RAY, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, 0, 0, 0) == NULL) {
        // "Occurrence failure"
        PRINTF("Ｅｒｒｏｒ : Mir_Ray 発生失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_jya_bombchuiwa.c", 410,
               this->actor.params);
    }
}

void Bg_Jya_Bombchuiwa_actor_move(Actor* thisx, PlayState* play) {
    BgJyaBombchuiwa* this = (BgJyaBombchuiwa*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void draw_tyorohikari_modelT(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 436);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 439);
    gSPDisplayList(POLY_XLU_DISP++, gBombchuiwa2DL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 443);
}

void draw_anahikari_model(Actor* thisx, PlayState* play) {
    BgJyaBombchuiwa* this = (BgJyaBombchuiwa*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 453);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 457);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, CLAMP_MAX((u32)(this->lightRayIntensity * 153.0f), 153));
    gSPDisplayList(POLY_XLU_DISP++, gBombchuiwaLight1DL);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, CLAMP_MAX((u32)(this->lightRayIntensity * 255.0f), 255));
    gSPDisplayList(POLY_XLU_DISP++, gBombchuiwaLight2DL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_jya_bombchuiwa.c", 472);
}

void Bg_Jya_Bombchuiwa_actor_draw(Actor* thisx, PlayState* play) {
    static Vec3f p = { -920.0f, 480.0f, -889.0f };
    static Vec3s a = { 0, 0, 0 };
    BgJyaBombchuiwa* this = (BgJyaBombchuiwa*)thisx;

    if (this->drawFlags & 1) {
        Cheap_gfx_display(play, gBombchuiwaDL);
        CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
    }

    if (this->drawFlags & 2) {
        draw_tyorohikari_modelT(play);
    }
    if (this->drawFlags & 4) {
        Matrix_softcv3_load(p.x, p.y, p.z, &a);
        Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        if (this->drawFlags & 4) {
            draw_anahikari_model(thisx, play);
        }
    }
}
