/*
 * File: z_en_horse_zelda.c
 * Overlay: ovl_En_Horse_Zelda
 * Description: Zelda's Horse
 */

#include "z_en_horse_zelda.h"
#include "assets/objects/object_horse_zelda/object_horse_zelda.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_HZ_Actor_ct(Actor* thisx, PlayState* play);
void En_HZ_Actor_dt(Actor* thisx, PlayState* play);
void En_HZ_move(Actor* thisx, PlayState* play);
void En_HZ_display(Actor* thisx, PlayState* play);

void En_HZ_Actor_mode_anim_loop(EnHorseZelda* this, PlayState* play);
void En_HZ_Actor_mode_rail_run(EnHorseZelda* this, PlayState* play);
void En_HZ_Actor_mode_anim_loop_init(EnHorseZelda* this);

ActorProfile En_Horse_Zelda_Profile = {
    /**/ ACTOR_EN_HORSE_ZELDA,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HORSE_ZELDA,
    /**/ sizeof(EnHorseZelda),
    /**/ En_HZ_Actor_ct,
    /**/ En_HZ_Actor_dt,
    /**/ En_HZ_move,
    /**/ En_HZ_display,
};

static AnimationHeader* skin_horse_zelda_anm_tbl[] = { &gHorseZeldaGallopingAnim };

static f32 zelda_anim_speed[] = { 2.0f / 3.0f };

static ColliderCylinderInitType1 HorseZeldaOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
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

static ColliderJntSphElementInit HorseZeldaOcInfoJntSphElemData[1] = {
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

static ColliderJntSphInit HorseZeldaOcInfoJntSphData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    1,
    HorseZeldaOcInfoJntSphElemData,
};

static CollisionCheckInfoInit HorseZeldaStatusData = { 10, 35, 100, MASS_HEAVY };

typedef struct HorsePosSpeed {
    /* 0x0 */ Vec3s pos;
    /* 0x6 */ u8 speedXZ;
} HorsePosSpeed; // size = 0x8

// these seem to be valid coords on Hyrule field, along with target speeds
static HorsePosSpeed course_data[] = {
    { { -1682, -500, 12578 }, 7 }, { { -3288, -500, 13013 }, 7 }, { { -5142, -417, 11630 }, 7 },
    { { -5794, -473, 9573 }, 7 },  { { -6765, -500, 8364 }, 7 },  { { -6619, -393, 6919 }, 7 },
    { { -5193, 124, 5433 }, 7 },   { { -2970, 2, 4537 }, 7 },     { { -2949, -35, 4527 }, 7 },
    { { -1907, -47, 2978 }, 7 },   { { 2488, 294, 3628 }, 7 },    { { 3089, 378, 4713 }, 7 },
    { { 1614, -261, 7596 }, 7 },   { { 754, -187, 9295 }, 7 },
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_STOP),
};

static EnHorseZeldaActionFunc mode_function[] = {
    En_HZ_Actor_mode_anim_loop,
    En_HZ_Actor_mode_rail_run,
};

static void CourseData2xyz_set(HorsePosSpeed* data, s32 index, Vec3f* fieldPos) {
    fieldPos->x = data[index].pos.x;
    fieldPos->y = data[index].pos.y;
    fieldPos->z = data[index].pos.z;
}

void En_HZ_CourseRun(EnHorseZelda* this, PlayState* play) {
    s32 pad;
    Vec3f fieldPos;
    s16 yawDiff;

    CourseData2xyz_set(course_data, this->fieldPosIndex, &fieldPos);
    if (Math3DLength(&fieldPos, &this->actor.world.pos) <= 400.0f) {
        this->fieldPosIndex++;
        if (this->fieldPosIndex >= ARRAY_COUNT(course_data)) {
            this->fieldPosIndex = 0;
            CourseData2xyz_set(course_data, 0, &fieldPos);
        }
    }
    yawDiff = search_position_angleY(&this->actor.world.pos, &fieldPos) - this->actor.world.rot.y;
    if (yawDiff > 300) {
        this->actor.world.rot.y += 300;
    } else if (yawDiff < -300) {
        this->actor.world.rot.y -= 300;
    } else {
        this->actor.world.rot.y += yawDiff;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (Actor_search_actor_distanceXZ(&this->actor, &GET_PLAYER(play)->actor) <= 300.0f) {
        if (this->actor.speed < 12.0f) {
            this->actor.speed += 1.0f;
        } else {
            this->actor.speed -= 1.0f;
        }
    } else if (this->actor.speed < course_data[this->fieldPosIndex].speedXZ) {
        this->actor.speed += 0.5f;
    } else {
        this->actor.speed -= 0.5f;
    }
}

void En_HZ_Actor_ct(Actor* thisx, PlayState* play) {
    EnHorseZelda* this = (EnHorseZelda*)thisx;

    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, 0.0115f);
    this->actor.gravity = -3.5f;
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_horse, 20.0f);
    this->actor.speed = 0.0f;
    this->actor.focus.pos = this->actor.world.pos;
    this->action = 0;
    this->actor.focus.pos.y += 70.0f;
    Skin_AnimationWorkBuffer2_ct(play, &this->skin, &gHorseZeldaSkel, &gHorseZeldaGallopingAnim);
    this->animationIndex = 0;
    Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, skin_horse_zelda_anm_tbl[0]);
    ClObjPipe_ct(play, &this->colliderCylinder);
    ClObjPipe_set3(play, &this->colliderCylinder, &this->actor, &HorseZeldaOcInfoData);
    ClObjJntSph_ct(play, &this->colliderSphere);
    ClObjJntSph_set5_nzm(play, &this->colliderSphere, &this->actor, &HorseZeldaOcInfoJntSphData, &this->colliderSphereItem);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &HorseZeldaStatusData);
    this->animationIndex = 0;
    En_HZ_Actor_mode_anim_loop_init(this);
}

void En_HZ_Actor_dt(Actor* thisx, PlayState* play) {
    EnHorseZelda* this = (EnHorseZelda*)thisx;

    ClObjPipe_dt(play, &this->colliderCylinder);
    ClObjJntSph_dt_nzf(play, &this->colliderSphere);
    Skin_AnimationWorkBuffer2_dt(play, &this->skin);
}

void En_HZ_Actor_mode_anim_loop_init(EnHorseZelda* this) {
    this->action = 0;
    this->animationIndex++;
    if (this->animationIndex > 0) {
        this->animationIndex = 0;
    }
    Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, skin_horse_zelda_anm_tbl[this->animationIndex]);
}

void En_HZ_Actor_mode_anim_loop(EnHorseZelda* this, PlayState* play) {
    this->actor.speed = 0.0f;
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        En_HZ_Actor_mode_anim_loop_init(this);
    }
}

void En_HZ_Actor_mode_rail_run_init(EnHorseZelda* this) {
    f32 speedMod;

    this->action = 1;
    this->animationIndex = 0;
    speedMod = this->actor.speed / 6.0f;
    Nai_FxFlagEntry(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
    Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_zelda_anm_tbl[this->animationIndex],
                     zelda_anim_speed[this->animationIndex] * speedMod * 1.5f, 0.0f,
                     Si2_anime_end_frame(skin_horse_zelda_anm_tbl[this->animationIndex]), ANIMMODE_ONCE, 0.0f);
}

void En_HZ_Actor_mode_rail_run(EnHorseZelda* this, PlayState* play) {
    En_HZ_CourseRun(this, play);
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        En_HZ_Actor_mode_rail_run_init(this);
    }
}

void En_HZ_xrot_calc(EnHorseZelda* this, PlayState* play) {
    s32 pad;
    CollisionPoly* poly;
    s32 pad2;
    Vec3f checkPos;
    s32 bgId;

    checkPos.x = (sin_s(this->actor.shape.rot.y) * 30.0f) + this->actor.world.pos.x;
    checkPos.y = this->actor.world.pos.y + 60.0f;
    checkPos.z = (cos_s(this->actor.shape.rot.y) * 30.0f) + this->actor.world.pos.z;
    this->floorYForwards = T_BGCheck_ObjGroundCheck_ai(&play->colCtx, &poly, &bgId, &checkPos);
    this->actor.shape.rot.x = RAD_TO_BINANG(fatan2(this->actor.world.pos.y - this->floorYForwards, 30.0f));
}

void En_HZ_move(Actor* thisx, PlayState* play) {
    EnHorseZelda* this = (EnHorseZelda*)thisx;
    s32 pad;

    mode_function[this->action](this, play);
    this->actor.speed = 0.0f;
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 20.0f, 55.0f, 100.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colliderCylinder);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderCylinder.base);
}

void En_HZ_calc(Actor* thisx, PlayState* play, Skin* skin) {
    Vec3f offset;
    Vec3f dst;
    EnHorseZelda* this = (EnHorseZelda*)thisx;
    s32 i;

    for (i = 0; i < this->colliderSphere.count; i++) {
        offset.x = this->colliderSphere.elements[i].dim.modelSphere.center.x;
        offset.y = this->colliderSphere.elements[i].dim.modelSphere.center.y;
        offset.z = this->colliderSphere.elements[i].dim.modelSphere.center.z;

        Skin_MatrixPosition2_gfx(skin, this->colliderSphere.elements[i].dim.limb, &offset, &dst);

        this->colliderSphere.elements[i].dim.worldSphere.center.x = dst.x;
        this->colliderSphere.elements[i].dim.worldSphere.center.y = dst.y;
        this->colliderSphere.elements[i].dim.worldSphere.center.z = dst.z;

        this->colliderSphere.elements[i].dim.worldSphere.radius =
            this->colliderSphere.elements[i].dim.modelSphere.radius * this->colliderSphere.elements[i].dim.scale;
    }

    //! @bug see relevant comment in `EnHorse_SkinCallback1`
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderSphere.base);
}

void En_HZ_display(Actor* thisx, PlayState* play) {
    EnHorseZelda* this = (EnHorseZelda*)thisx;

    En_HZ_xrot_calc(this, play);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Skin_disp2(&this->actor, play, &this->skin, En_HZ_calc, true);
}
