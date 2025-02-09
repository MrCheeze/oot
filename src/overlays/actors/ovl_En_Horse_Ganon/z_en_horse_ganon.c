/*
 * File: z_en_horse_ganon.c
 * Overlay: ovl_En_Horse_Ganon
 * Description: Ganondorf's Horse
 */

#include "z_en_horse_ganon.h"
#include "assets/objects/object_horse_ganon/object_horse_ganon.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef struct unk_D_80A69248 {
    /* 0x0 */ Vec3s unk_0;
    /* 0x6 */ u8 unk_6;
} unk_D_80A69248; // size = 0x8

void En_HG_Actor_ct(Actor* thisx, PlayState* play);
void En_HG_Actor_dt(Actor* thisx, PlayState* play);
void En_HG_move(Actor* thisx, PlayState* play);
void En_HG_display(Actor* thisx, PlayState* play);

void En_HG_Actor_mode_anim_loop_init(EnHorseGanon* this);
void En_HG_Actor_mode_anim_loop(EnHorseGanon* this, PlayState* play);
void En_HG_Actor_mode_rail_run(EnHorseGanon* this, PlayState* play);

ActorProfile En_Horse_Ganon_Profile = {
    /**/ ACTOR_EN_HORSE_GANON,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HORSE_GANON,
    /**/ sizeof(EnHorseGanon),
    /**/ En_HG_Actor_ct,
    /**/ En_HG_Actor_dt,
    /**/ En_HG_move,
    /**/ En_HG_display,
};

static AnimationHeader* skin_horse_ganon_anm_tbl[] = {
    &gHorseGanonIdleAnim,     &gHorseGanonWhinnyAnim,    &gHorseGanonWalkingAnim,
    &gHorseGanonTrottingAnim, &gHorseGanonGallopingAnim, &gHorseGanonRearingAnim,
};

static f32 ganon_anim_speed[] = { 2.0f / 3.0f, 2.0f / 3.0f, 1.0f, 1.0f, 1.0f, 2.0f / 3.0f };

static ColliderCylinderInit HorseGanonOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 40, 100, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit HorseGanonOcInfoJntSphElemData[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit HorseGanonOcInfoJntSphData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    1,
    HorseGanonOcInfoJntSphElemData,
};

static CollisionCheckInfoInit HorseGanonStatusData = { 10, 35, 100, MASS_HEAVY };

static unk_D_80A69248 course_data[] = {
    { 0x09B8, 0x0126, 0x0E2C, 0x07 }, { 0x0C11, 0x017A, 0x1269, 0x07 }, { 0x064E, 0xFEFB, 0x1DAC, 0x07 },
    { 0x02F2, 0xFF45, 0x244F, 0x07 }, { 0xF96E, 0xFE0C, 0x3122, 0x07 }, { 0xF328, 0xFE0C, 0x32D5, 0x07 },
    { 0xEBEA, 0xFE5F, 0x2D6E, 0x07 }, { 0xE95E, 0xFE27, 0x2565, 0x07 }, { 0xE593, 0xFE0C, 0x20AC, 0x07 },
    { 0xE625, 0xFE77, 0x1B07, 0x07 }, { 0xEBB7, 0x007C, 0x1539, 0x07 }, { 0xF466, 0x0002, 0x11B9, 0x07 },
    { 0xF47B, 0xFFDD, 0x11AF, 0x07 }, { 0xF88D, 0xFFD1, 0x0BA2, 0x07 },
};

static s32 sound_on_frames[] = { 0, 16 };

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_STOP),
};

static EnHorseGanonActionFunc mode_function[] = { En_HG_Actor_mode_anim_loop, En_HG_Actor_mode_rail_run };

static void CourseData2xyz_set(unk_D_80A69248* data, s32 index, Vec3f* vec) {
    vec->x = data[index].unk_0.x;
    vec->y = data[index].unk_0.y;
    vec->z = data[index].unk_0.z;
}

void En_HG_CourseRun(EnHorseGanon* this, PlayState* play) {
    Vec3f* tempPos;
    Vec3f vec;
    s16 y;

    CourseData2xyz_set(course_data, this->unk_1EC, &vec);
    if (Math3DLength(&vec, &this->actor.world.pos) <= 400.0f) {
        this->unk_1EC++;
        if (this->unk_1EC >= 14) {
            this->unk_1EC = 0;
            CourseData2xyz_set(course_data, 0, &vec);
        }
    }

    tempPos = &this->actor.world.pos;
    y = search_position_angleY(tempPos, &vec) - this->actor.world.rot.y;
    if (y >= 301) {
        this->actor.world.rot.y += 300;
    } else if (y < -300) {
        this->actor.world.rot.y -= 300;
    } else {
        this->actor.world.rot.y += y;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (Actor_search_actor_distanceXZ(&this->actor, &GET_PLAYER(play)->actor) <= 300.0f) {
        if (this->actor.speed < 12.0f) {
            this->actor.speed += 1.0f;
        } else {
            this->actor.speed -= 1.0f;
        }
    } else if (this->actor.speed < course_data[this->unk_1EC].unk_6) {
        this->actor.speed += 0.5f;
    } else {
        this->actor.speed -= 0.5f;
    }
}

void En_HG_setWalkSound(EnHorseGanon* this) {
    if ((this->skin.skelAnime.curFrame > sound_on_frames[this->soundCount]) &&
        (this->soundCount != 0 || !(this->skin.skelAnime.curFrame > sound_on_frames[1]))) {
        Nai_FxFlagEntry(NA_SE_EV_HORSE_WALK, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);

        this->soundCount++;
        if (this->soundCount >= 2) {
            this->soundCount = 0;
        }
    }
}

void En_HG_Actor_ct(Actor* thisx, PlayState* play) {
    EnHorseGanon* this = (EnHorseGanon*)thisx;

    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, 0.0115f);

    this->actor.gravity = -3.5f;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_horse, 20.0f);
    this->actor.speed = 0.0f;
    this->actor.focus.pos = this->actor.world.pos;
    this->action = 0;
    this->actor.focus.pos.y += 70.0f;
    Skin_AnimationWorkBuffer2_ct(play, &this->skin, &gHorseGanonSkel, &gHorseGanonIdleAnim);
    this->currentAnimation = 0;
    Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, skin_horse_ganon_anm_tbl[0]);

    ClObjPipe_ct(play, &this->colliderBody);
    ClObjPipe_set5(play, &this->colliderBody, &this->actor, &HorseGanonOcInfoData);
    ClObjJntSph_ct(play, &this->colliderHead);
    ClObjJntSph_set5_nzm(play, &this->colliderHead, &this->actor, &HorseGanonOcInfoJntSphData, this->headElements);

    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &HorseGanonStatusData);
    En_HG_Actor_mode_anim_loop_init(this);
}

void En_HG_Actor_dt(Actor* thisx, PlayState* play) {
    EnHorseGanon* this = (EnHorseGanon*)thisx;

    Skin_AnimationWorkBuffer2_dt(play, &this->skin);
    ClObjPipe_dt(play, &this->colliderBody);
    ClObjJntSph_dt_nzf(play, &this->colliderHead);
}

void En_HG_Actor_mode_anim_loop_init(EnHorseGanon* this) {
    this->action = 0;
    Skeleton_Info2_init_standard_repeat(&this->skin.skelAnime, skin_horse_ganon_anm_tbl[4]);
}

void En_HG_Actor_mode_anim_loop(EnHorseGanon* this, PlayState* play) {
    this->actor.speed = 0.0f;
    Skeleton_Info2_anime_play(&this->skin.skelAnime);
}

void En_HG_Actor_mode_rail_run_init(EnHorseGanon* this) {
    s32 animationChanged;
    f32 sp30;

    animationChanged = 0;
    this->action = 1;
    if (this->actor.speed <= 3.0f) {
        if (this->currentAnimation != 2) {
            animationChanged = 1;
        }
        this->currentAnimation = 2;
    } else if (this->actor.speed <= 6.0f) {
        if (this->currentAnimation != 3) {
            animationChanged = 1;
        }
        this->currentAnimation = 3;
    } else {
        if (this->currentAnimation != 4) {
            animationChanged = 1;
        }
        this->currentAnimation = 4;
    }

    if (this->currentAnimation == 2) {
        sp30 = this->actor.speed / 3.0f;
    } else if (this->currentAnimation == 3) {
        sp30 = this->actor.speed / 5.0f;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (this->currentAnimation == 4) {
        sp30 = this->actor.speed / 7.0f;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else {
        sp30 = 1.0f;
    }

    if (animationChanged == 1) {
        Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_ganon_anm_tbl[this->currentAnimation],
                         ganon_anim_speed[this->currentAnimation] * sp30 * 1.5f, 0.0f,
                         Si2_anime_end_frame(skin_horse_ganon_anm_tbl[this->currentAnimation]), ANIMMODE_ONCE, -3.0f);
    } else {
        Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_ganon_anm_tbl[this->currentAnimation],
                         ganon_anim_speed[this->currentAnimation] * sp30 * 1.5f, 0.0f,
                         Si2_anime_end_frame(skin_horse_ganon_anm_tbl[this->currentAnimation]), ANIMMODE_ONCE, 0.0f);
    }
}

void En_HG_Actor_mode_rail_run(EnHorseGanon* this, PlayState* play) {
    if (this->currentAnimation == 2) {
        En_HG_setWalkSound(this);
    }

    En_HG_CourseRun(this, play);

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        En_HG_Actor_mode_rail_run_init(this);
    }
}

void En_HG_xrot_calc(EnHorseGanon* this, PlayState* play) {
    s32 pad;
    CollisionPoly* poly;
    s32 pad2;
    Vec3f checkPos;
    s32 bgId;

    checkPos.x = sin_s(this->actor.shape.rot.y) * 30.0f + this->actor.world.pos.x;
    checkPos.y = this->actor.world.pos.y + 60.0f;
    checkPos.z = cos_s(this->actor.shape.rot.y) * 30.0f + this->actor.world.pos.z;
    this->unk_1F4 = T_BGCheck_ObjGroundCheck_ai(&play->colCtx, &poly, &bgId, &checkPos);
    this->actor.shape.rot.x = RAD_TO_BINANG(fatan2(this->actor.world.pos.y - this->unk_1F4, 30.0f));
}

void En_HG_move(Actor* thisx, PlayState* play) {
    EnHorseGanon* this = (EnHorseGanon*)thisx;
    s32 pad;

    mode_function[this->action](this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 20.0f, 55.0f, 100.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colliderBody);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderBody.base);
}

void En_HG_calc(Actor* thisx, PlayState* play, Skin* skin) {
    Vec3f sp4C;
    Vec3f sp40;
    EnHorseGanon* this = (EnHorseGanon*)thisx;
    s32 index;

    for (index = 0; index < this->colliderHead.count; index++) {
        sp4C.x = this->colliderHead.elements[index].dim.modelSphere.center.x;
        sp4C.y = this->colliderHead.elements[index].dim.modelSphere.center.y;
        sp4C.z = this->colliderHead.elements[index].dim.modelSphere.center.z;

        Skin_MatrixPosition2_gfx(skin, this->colliderHead.elements[index].dim.limb, &sp4C, &sp40);

        this->colliderHead.elements[index].dim.worldSphere.center.x = sp40.x;
        this->colliderHead.elements[index].dim.worldSphere.center.y = sp40.y;
        this->colliderHead.elements[index].dim.worldSphere.center.z = sp40.z;

        this->colliderHead.elements[index].dim.worldSphere.radius =
            this->colliderHead.elements[index].dim.modelSphere.radius * this->colliderHead.elements[index].dim.scale;
    }

    //! @bug see relevant comment in `EnHorse_SkinCallback1`
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderHead.base);
}

void En_HG_display(Actor* thisx, PlayState* play) {
    EnHorseGanon* this = (EnHorseGanon*)thisx;

    En_HG_xrot_calc(this, play);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Skin_disp2(&this->actor, play, &this->skin, En_HG_calc, true);
}
