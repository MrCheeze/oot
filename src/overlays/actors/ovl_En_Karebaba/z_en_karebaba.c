/**
 * File: z_en_karebaba.c
 * Overlay: ovl_En_Karebaba
 * Description: Withered Deku Baba
 */

#include "z_en_karebaba.h"
#include "assets/objects/object_dekubaba/object_dekubaba.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void En_Karebaba_actor_ct(Actor* thisx, PlayState* play);
void En_Karebaba_actor_dt(Actor* thisx, PlayState* play);
void En_Karebaba_actor_move(Actor* thisx, PlayState* play);
void En_Karebaba_actor_draw(Actor* thisx, PlayState* play);

static void mode_scale_up_init(EnKarebaba* this);
static void mode_wait_init(EnKarebaba* this);
static void mode_scale_up(EnKarebaba* this, PlayState* play);
static void mode_wait(EnKarebaba* this, PlayState* play);
static void mode_up(EnKarebaba* this, PlayState* play);
static void mode_rotate(EnKarebaba* this, PlayState* play);
static void mode_down(EnKarebaba* this, PlayState* play);
static void mode_deku_stick(EnKarebaba* this, PlayState* play);
static void mode_return(EnKarebaba* this, PlayState* play);
void mode_restart_wait(EnKarebaba* this, PlayState* play);
static void mode_restart(EnKarebaba* this, PlayState* play);
static void mode_stop(EnKarebaba* this, PlayState* play);

ActorProfile En_Karebaba_Profile = {
    /**/ ACTOR_EN_KAREBABA,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DEKUBABA,
    /**/ sizeof(EnKarebaba),
    /**/ En_Karebaba_actor_ct,
    /**/ En_Karebaba_actor_dt,
    /**/ En_Karebaba_actor_move,
    /**/ En_Karebaba_actor_draw,
};

static ColliderCylinderInit KarebabaAcPipeData = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 7, 25, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit KarebabaAtOcPipeData = {
    {
        COL_MATERIAL_HARD,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 4, 25, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit KarebabaStatusData = { 1, 15, 80, MASS_HEAVY };

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 2500, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_1, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_WITHERED_DEKU_BABA, ICHAIN_STOP),
};

void En_Karebaba_actor_ct(Actor* thisx, PlayState* play) {
    EnKarebaba* this = (EnKarebaba*)thisx;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 22.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gDekuBabaSkel, &gDekuBabaFastChompAnim, this->jointTable, this->morphTable,
                   8);
    ClObjPipe_ct(play, &this->bodyCollider);
    ClObjPipe_set5(play, &this->bodyCollider, &this->actor, &KarebabaAcPipeData);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->bodyCollider);
    ClObjPipe_ct(play, &this->headCollider);
    ClObjPipe_set5(play, &this->headCollider, &this->actor, &KarebabaAtOcPipeData);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->headCollider);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, CollisionBtlTbl_get(1), &KarebabaStatusData);

    this->boundFloor = NULL;

    if (this->actor.params == 0) {
        mode_scale_up_init(this);
    } else {
        mode_wait_init(this);
    }
}

void En_Karebaba_actor_dt(Actor* thisx, PlayState* play) {
    EnKarebaba* this = (EnKarebaba*)thisx;

    ClObjPipe_dt(play, &this->bodyCollider);
    ClObjPipe_dt(play, &this->headCollider);
}

void karebaba_change_ac_data(EnKarebaba* this) {
    this->bodyCollider.dim.radius = 7;
    this->bodyCollider.dim.height = 25;
    this->bodyCollider.base.colMaterial = COL_MATERIAL_HARD;
    this->bodyCollider.base.acFlags |= AC_HARD;
    this->bodyCollider.elem.acDmgInfo.dmgFlags = DMG_DEFAULT;
    this->headCollider.dim.height = 25;
}

static void mode_scale_up_init(EnKarebaba* this) {
    Actor_set_scale(&this->actor, 0.0f);
    this->actor.shape.rot.x = -0x4000;
    this->actionFunc = mode_scale_up;
    this->actor.world.pos.y = this->actor.home.pos.y + 14.0f;
}

static void mode_wait_init(EnKarebaba* this) {
    Actor_set_scale(&this->actor, 0.005f);
    this->actor.shape.rot.x = -0x4000;
    this->actionFunc = mode_wait;
    this->actor.world.pos.y = this->actor.home.pos.y + 14.0f;
}

static void mode_up_init(EnKarebaba* this) {
    Skeleton_Info2_init(&this->skelAnime, &gDekuBabaFastChompAnim, 4.0f, 0.0f,
                     Si2_anime_end_frame(&gDekuBabaFastChompAnim), ANIMMODE_LOOP, -3.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_DUMMY482);
    this->actionFunc = mode_up;
}

static void mode_stop_init(EnKarebaba* this) {
    if (this->actionFunc != mode_rotate) {
        Actor_set_scale(&this->actor, 0.01f);
        this->bodyCollider.base.colMaterial = COL_MATERIAL_HIT6;
        this->bodyCollider.base.acFlags &= ~AC_HARD;
        this->bodyCollider.elem.acDmgInfo.dmgFlags =
            !LINK_IS_ADULT ? ((DMG_SWORD | DMG_BOOMERANG) & ~DMG_JUMP_MASTER) : (DMG_SWORD | DMG_BOOMERANG);
        this->bodyCollider.dim.radius = 15;
        this->bodyCollider.dim.height = 80;
        this->headCollider.dim.height = 80;
    }

    this->actor.params = 40;
    this->actionFunc = mode_stop;
}

static void mode_rotate_init(EnKarebaba* this) {
    this->actor.params = 40;
    this->actionFunc = mode_rotate;
}

static void mode_down_init(EnKarebaba* this) {
    this->actor.params = 0;
    this->actor.gravity = -0.8f;
    this->actor.velocity.y = 4.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
    this->actor.speed = 3.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_DEKU_JR_DEAD);
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->actionFunc = mode_down;
}

static void mode_deku_stick_init(EnKarebaba* this, PlayState* play) {
    Actor_set_scale(&this->actor, 0.03f);
    this->actor.shape.rot.x -= 0x4000;
    this->actor.shape.yOffset = 1000.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.shape.shadowScale = 3.0f;
    Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_MISC);
    this->actor.params = 200;
    this->actor.flags &= ~ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->actionFunc = mode_deku_stick;
}

static void mode_return_init(EnKarebaba* this) {
    Skeleton_Info2_init(&this->skelAnime, &gDekuBabaFastChompAnim, -3.0f, Si2_anime_end_frame(&gDekuBabaFastChompAnim),
                     0.0f, ANIMMODE_ONCE, -3.0f);
    karebaba_change_ac_data(this);
    this->actionFunc = mode_return;
}

void mode_restart_wait_init(EnKarebaba* this) {
    Skeleton_Info2_init(&this->skelAnime, &gDekuBabaFastChompAnim, 0.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f);
    karebaba_change_ac_data(this);
    this->actor.shape.rot.x = -0x4000;
    this->actor.params = 200;
    this->actor.parent = NULL;
    this->actor.shape.shadowScale = 0.0f;
    xyz_t_move(&this->actor.world.pos, &this->actor.home.pos);
    this->actionFunc = mode_restart_wait;
}

void mode_restart_init(EnKarebaba* this) {
    this->actor.shape.yOffset = 0.0f;
    this->actor.shape.shadowScale = 22.0f;
    this->headCollider.dim.radius = KarebabaAtOcPipeData.dim.radius;
    Actor_set_scale(&this->actor, 0.0f);
    this->actionFunc = mode_restart;
}

static void mode_scale_up(EnKarebaba* this, PlayState* play) {
    f32 scale;

    this->actor.params++;
    scale = this->actor.params * 0.05f;
    Actor_set_scale(&this->actor, 0.005f * scale);
    this->actor.world.pos.y = this->actor.home.pos.y + (14.0f * scale);
    if (this->actor.params == 20) {
        mode_wait_init(this);
    }
}

static void mode_wait(EnKarebaba* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 200.0f && fabsf(this->actor.yDistToPlayer) < 30.0f) {
        mode_up_init(this);
    }
}

static void mode_up(EnKarebaba* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_f(&this->actor.scale.x, 0.01f, 0.0005f);
    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;
    if (chase_f(&this->actor.world.pos.y, this->actor.home.pos.y + 60.0f, 5.0f)) {
        mode_stop_init(this);
    }
    this->actor.shape.rot.y += 0x1999;
    Effect_Hahen_Kakusan_ct3(play, &this->actor.home.pos, 3.0f, 0, 12, 5, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

static void mode_stop(EnKarebaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actor.params != 0) {
        this->actor.params--;
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 12.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_DEKU_JR_MOUTH);
    }

    if (this->bodyCollider.base.acFlags & AC_HIT) {
        mode_down_init(this);
        Actor_info_finish(play, &this->actor);
    } else if (search_position_distanceXZ(&this->actor.home.pos, &player->actor.world.pos) > 240.0f) {
        mode_return_init(this);
    } else if (this->actor.params == 0) {
        mode_rotate_init(this);
    }
}

static void mode_rotate(EnKarebaba* this, PlayState* play) {
    s32 value;
    f32 cos60;

    if (this->actor.params != 0) {
        this->actor.params--;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 12.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_DEKU_JR_MOUTH);
    }

    value = 20 - this->actor.params;
    value = 20 - ABS(value);

    if (value > 10) {
        value = 10;
    }

    this->headCollider.dim.radius = KarebabaAtOcPipeData.dim.radius + (value * 2);
    this->actor.shape.rot.x = 0xC000 - (value * 0x100);
    this->actor.shape.rot.y += value * 0x2C0;
    this->actor.world.pos.y = (sin_s(this->actor.shape.rot.x) * -60.0f) + this->actor.home.pos.y;

    cos60 = cos_s(this->actor.shape.rot.x) * 60.0f;

    this->actor.world.pos.x = (sin_s(this->actor.shape.rot.y) * cos60) + this->actor.home.pos.x;
    this->actor.world.pos.z = (cos_s(this->actor.shape.rot.y) * cos60) + this->actor.home.pos.z;

    if (this->bodyCollider.base.acFlags & AC_HIT) {
        mode_down_init(this);
        Actor_info_finish(play, &this->actor);
    } else if (this->actor.params == 0) {
        mode_stop_init(this);
    }
}

static void mode_down(EnKarebaba* this, PlayState* play) {
    static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };
    s32 i;
    Vec3f position;
    Vec3f rotation;

    chase_f(&this->actor.speed, 0.0f, 0.1f);

    if (this->actor.params == 0) {
        chase_angle(&this->actor.shape.rot.x, 0x4800, 0x71C);
        Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 3.0f, 0, 12, 5, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);

        if (this->actor.scale.x > 0.005f &&
            ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) || (this->actor.bgCheckFlags & BGCHECKFLAG_WALL))) {
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 0.0f;
            this->actor.speed = 0.0f;
            this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
            Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 3.0f, 0, 12, 5, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
            this->actor.params = 1;
        }
    } else if (this->actor.params == 1) {
        xyz_t_move(&position, &this->actor.world.pos);
        rotation.z = sin_s(this->actor.shape.rot.x) * 20.0f;
        rotation.x = -20.0f * cos_s(this->actor.shape.rot.x) * sin_s(this->actor.shape.rot.y);
        rotation.y = -20.0f * cos_s(this->actor.shape.rot.x) * cos_s(this->actor.shape.rot.y);

        for (i = 0; i < 4; i++) {
            Effect_SS_Dust_sc_li_ct(play, &position, &zero_vec, &zero_vec, 500, 50);
            position.x += rotation.x;
            position.y += rotation.z;
            position.z += rotation.y;
        }

        Effect_SS_Dust_sc_li_ct(play, &this->actor.home.pos, &zero_vec, &zero_vec, 500, 100);
        mode_deku_stick_init(this, play);
    }
}

static void mode_deku_stick(EnKarebaba* this, PlayState* play) {
    if (this->actor.params != 0) {
        this->actor.params--;
    }

    if (Actor_carry_check(&this->actor, play) || this->actor.params == 0) {
        mode_restart_wait_init(this);
    } else {
        Actor_carry_request_set(&this->actor, play, GI_DEKU_STICKS_1);
    }
}

static void mode_return(EnKarebaba* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_f(&this->actor.scale.x, 0.005f, 0.0005f);
    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;

    if (chase_f(&this->actor.world.pos.y, this->actor.home.pos.y + 14.0f, 5.0f)) {
        mode_wait_init(this);
    }

    this->actor.shape.rot.y += 0x1999;
    Effect_Hahen_Kakusan_ct3(play, &this->actor.home.pos, 3.0f, 0, 12, 5, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

void mode_restart_wait(EnKarebaba* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->actor.params != 0) {
        this->actor.params--;
    }
    if (this->actor.params == 0) {
        mode_restart_init(this);
    }
}

static void mode_restart(EnKarebaba* this, PlayState* play) {
    f32 scaleFactor;

    this->actor.params++;
    scaleFactor = this->actor.params * 0.05f;
    Actor_set_scale(&this->actor, 0.005f * scaleFactor);
    this->actor.world.pos.y = this->actor.home.pos.y + (14.0f * scaleFactor);

    if (this->actor.params == 20) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE;
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
        mode_wait_init(this);
    }
}

void En_Karebaba_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnKarebaba* this = (EnKarebaba*)thisx;
    f32 height;

    this->actionFunc(this, play);

    if (this->actionFunc != mode_restart_wait) {
        if (this->actionFunc == mode_down) {
            Actor_position_moveF(&this->actor);
            Actor_BGcheck2(play, &this->actor, 10.0f, 15.0f, 10.0f,
                                    UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        } else {
            Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
            if (this->boundFloor == NULL) {
                this->boundFloor = this->actor.floorPoly;
            }
        }
        if (this->actionFunc != mode_down && this->actionFunc != mode_deku_stick) {
            if (this->actionFunc != mode_restart && this->actionFunc != mode_scale_up) {
                CollisionCheck_setAT(play, &play->colChkCtx, &this->headCollider.base);
                CollisionCheck_setAC(play, &play->colChkCtx, &this->bodyCollider.base);
            }
            CollisionCheck_setOC(play, &play->colChkCtx, &this->headCollider.base);
            Actor_world_to_eye(&this->actor, (this->actor.scale.x * 10.0f) / 0.01f);
            height = this->actor.home.pos.y + 40.0f;
            this->actor.focus.pos.x = this->actor.home.pos.x;
            this->actor.focus.pos.y = CLAMP_MAX(this->actor.focus.pos.y, height);
            this->actor.focus.pos.z = this->actor.home.pos.z;
        }
    }
}

void karebaba_draw_home_shadow(EnKarebaba* this, PlayState* play) {
    MtxF mf;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_karebaba.c", 1013);

    _texture_decal_shadow(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);
    T_Polygon_Ground_Matrix(this->boundFloor, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, &mf);
    Matrix_mult(&mf, MTXMODE_NEW);
    Matrix_scale(0.15f, 1.0f, 0.15f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_karebaba.c", 1029);
    gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_karebaba.c", 1034);
}

void En_Karebaba_actor_draw(Actor* thisx, PlayState* play) {
    static Color_RGBA8 fog_color = { 0, 0, 0, 0 };
    static Gfx* miki_model[] = { gDekuBabaStemTopDL, gDekuBabaStemMiddleDL, gDekuBabaStemBaseDL };
    static Vec3f eye_pos = { 0.0f, 0.0f, 0.0f };
    EnKarebaba* this = (EnKarebaba*)thisx;
    s32 i;
    s32 stemSections;
    f32 scale;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_karebaba.c", 1056);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    if (this->actionFunc == mode_deku_stick) {
        if (this->actor.params > 40 || PARAMS_GET_U(this->actor.params, 0, 1)) {
            Matrix_translate(0.0f, 0.0f, 200.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_karebaba.c", 1066);
            gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStickDropDL);
        }
    } else if (this->actionFunc != mode_restart_wait) {
        Eff_Set_Fog2(play, &fog_color, 1, 2);
        Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, NULL);
        Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);

        if ((this->actionFunc == mode_restart) || (this->actionFunc == mode_scale_up)) {
            scale = this->actor.params * 0.0005f;
        } else {
            scale = 0.01f;
        }

        Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
        Matrix_rotateXYZ(this->actor.shape.rot.x, this->actor.shape.rot.y, 0, MTXMODE_APPLY);

        if (this->actionFunc == mode_down) {
            stemSections = 2;
        } else {
            stemSections = 3;
        }

        for (i = 0; i < stemSections; i++) {
            Matrix_translate(0.0f, 0.0f, -2000.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_karebaba.c", 1116);
            gSPDisplayList(POLY_OPA_DISP++, miki_model[i]);

            if (i == 0 && this->actionFunc == mode_down) {
                Matrix_Position(&eye_pos, &this->actor.focus.pos);
            }
        }

        Eff_Off_Fog(play);
    }

    Eff_Set_Fog2(play, &fog_color, 1, 2);
    Matrix_translate(this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, MTXMODE_NEW);

    if (this->actionFunc != mode_scale_up) {
        scale = 0.01f;
    }

    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_rotateY(BINANG_TO_RAD(this->actor.home.rot.y), MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_karebaba.c", 1144);
    gSPDisplayList(POLY_OPA_DISP++, gDekuBabaBaseLeavesDL);

    if (this->actionFunc == mode_down) {
        Matrix_rotateXYZ(-0x4000, (s16)(this->actor.shape.rot.y - this->actor.home.rot.y), 0, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_karebaba.c", 1155);
        gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStemBaseDL);
    }

    Eff_Off_Fog(play);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_karebaba.c", 1163);

    if (this->boundFloor != NULL) {
        karebaba_draw_home_shadow(this, play);
    }
}
