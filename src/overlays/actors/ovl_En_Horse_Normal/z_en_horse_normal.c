/*
 * File: z_en_horse_normal.c
 * Overlay: ovl_En_Horse_Normal
 * Description: Non-rideable horses (Lon Lon Ranch and Stable)
 */

#include "z_en_horse_normal.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_horse_normal/object_horse_normal.h"

#define FLAGS 0

typedef struct EnHorseNormalUnkStruct1 {
    Vec3s pos;
    u8 unk_06; // this may be a s16 if the always-0 following byte is actually not padding
} EnHorseNormalUnkStruct1;

typedef struct EnHorseNormalUnkStruct2 {
    s32 len;
    EnHorseNormalUnkStruct1* items;
} EnHorseNormalUnkStruct2;

typedef enum EnHorseNormalAction {
    /* 0x00 */ HORSE_CYCLE_ANIMATIONS,
    /* 0x01 */ HORSE_WANDER,
    /* 0x02 */ HORSE_WAIT,
    /* 0x03 */ HORSE_WAIT_CLONE,
    /* 0x04 */ HORSE_FOLLOW_PATH
} EnHorseNormalAction;

void EN_HN_Actor_ct(Actor* thisx, PlayState* play);
void EN_HN_Actor_dt(Actor* thisx, PlayState* play);
void EN_HN_move(Actor* thisx, PlayState* play);
void EN_HN_display(Actor* thisx, PlayState* play2);

void EN_HN_Actor_mode_loop_path_init(EnHorseNormal* this, PlayState* play);
void EN_HN_Actor_mode_free_init(EnHorseNormal* this);
void EN_HN_Actor_mode_run_sound_set(EnHorseNormal* this);
void EN_HN_Actor_mode_wait_init(EnHorseNormal* this);
void EN_HN_Actor_mode_doppel_init(EnHorseNormal* this);

ActorProfile En_Horse_Normal_Profile = {
    /**/ ACTOR_EN_HORSE_NORMAL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HORSE_NORMAL,
    /**/ sizeof(EnHorseNormal),
    /**/ EN_HN_Actor_ct,
    /**/ EN_HN_Actor_dt,
    /**/ EN_HN_move,
    /**/ EN_HN_display,
};

static AnimationHeader* skin_horse_normal_anm_tbl[] = {
    &gHorseNormalIdleAnim,      &gHorseNormalWhinnyAnim,  &gHorseNormalRefuseAnim,
    &gHorseNormalRearingAnim,   &gHorseNormalWalkingAnim, &gHorseNormalTrottingAnim,
    &gHorseNormalGallopingAnim, &gHorseNormalJumpingAnim, &gHorseNormalJumpingHighAnim,
};

static ColliderCylinderInit HorseNormalOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
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

static ColliderCylinderInit HorseNormalDoppelOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
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
    { 60, 100, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit HorseNormalOcInfoJntSphElemData[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 11, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit HorseNormalOcInfoJntSphData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(HorseNormalOcInfoJntSphElemData),
    HorseNormalOcInfoJntSphElemData,
};

static CollisionCheckInfoInit HorseNormalStatusData = { 10, 35, 100, MASS_HEAVY };

// Unused
static EnHorseNormalUnkStruct1 spot20_race_data[] = {
    { { 1058, 1, 384 }, 7 },    { { 1653, 39, -381 }, 6 }, { { 1606, 1, -1048 }, 6 }, { { 1053, 1, -1620 }, 6 },
    { { -1012, 1, -1633 }, 7 }, { { -1655, 1, -918 }, 6 }, { { -1586, 1, -134 }, 6 }, { { -961, 1, 403 }, 7 },
};

// Unused
static EnHorseNormalUnkStruct2 spot20_race_info = { ARRAY_COUNT(spot20_race_data), spot20_race_data };

// Unused
static EnHorseNormalUnkStruct1 spot00_race_data[] = {
    { { 88, 0, 2078 }, 10 },       { { 2482, 376, 4631 }, 7 },    { { 2228, -28, 6605 }, 12 },
    { { 654, -100, 8864 }, 7 },    { { -297, -500, 10667 }, 12 }, { { -5303, -420, 10640 }, 10 },
    { { -6686, -500, 7760 }, 10 }, { { -5260, 100, 5411 }, 7 },   { { -3573, -269, 3893 }, 10 },
};

// Unused
static EnHorseNormalUnkStruct2 spot00_race_info = { ARRAY_COUNT(spot00_race_data), spot00_race_data };

void EN_HN_setWalkSound(EnHorseNormal* this) {
    static s32 sound_on_frames[] = { 0, 16 };

    if (sound_on_frames[this->unk_200] < this->skin.skelAnime.curFrame &&
        ((this->unk_200 != 0) || !(sound_on_frames[1] < this->skin.skelAnime.curFrame))) {
        Nai_FxFlagEntry(NA_SE_EV_HORSE_WALK, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->unk_200++;
        if (this->unk_200 >= ARRAY_COUNT(sound_on_frames)) {
            this->unk_200 = 0;
        }
    }
}

f32 EN_HN_Actor_calcAnimSpeed(EnHorseNormal* this) {
    static f32 anim_speed[] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 1.5f, 1.5f, 1.5f, 1.0f };
    f32 result;

    if (this->animationIdx == 4) {
        result = anim_speed[this->animationIdx] * this->actor.speed * (1 / 2.0f);
    } else if (this->animationIdx == 5) {
        result = anim_speed[this->animationIdx] * this->actor.speed * (1 / 3.0f);
    } else if (this->animationIdx == 6) {
        result = anim_speed[this->animationIdx] * this->actor.speed * (1 / 5.0f);
    } else {
        result = anim_speed[this->animationIdx];
    }

    return result;
}

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_STOP),
};

void EN_HN_Actor_ct(Actor* thisx, PlayState* play) {
    EnHorseNormal* this = (EnHorseNormal*)thisx;
    s32 pad;

    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.gravity = -3.5f;
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_horse, 20.0f);
    this->actor.speed = 0.0f;
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    this->action = HORSE_CYCLE_ANIMATIONS;
    this->animationIdx = 0;
    ClObjPipe_ct(play, &this->bodyCollider);
    ClObjPipe_set5(play, &this->bodyCollider, &this->actor, &HorseNormalOcInfoData);
    ClObjJntSph_ct(play, &this->headCollider);
    ClObjJntSph_set5_nzm(play, &this->headCollider, &this->actor, &HorseNormalOcInfoJntSphData, this->headElements);
    ClObjPipe_ct(play, &this->cloneCollider);
    ClObjPipe_set5(play, &this->cloneCollider, &this->actor, &HorseNormalDoppelOcInfoData);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &HorseNormalStatusData);
    if (play->sceneId == SCENE_LON_LON_RANCH) {
        if (this->actor.world.rot.z == 0 || !IS_DAY) {
            Actor_delete(&this->actor);
            return;
        }
        if (!LINK_IS_ADULT) {
            if (event_check(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
                if (this->actor.world.rot.z != 3) {
                    Actor_delete(&this->actor);
                    return;
                }
            } else if (this->actor.world.rot.z != 1) {
                Actor_delete(&this->actor);
                return;
            }
        } else if (event_check(EVENTCHKINF_EPONA_OBTAINED) || R_DEBUG_FORCE_EPONA_OBTAINED) {
            if (this->actor.world.rot.z != 7) {
                Actor_delete(&this->actor);
                return;
            }
        } else if (this->actor.world.rot.z != 5) {
            Actor_delete(&this->actor);
            return;
        }
        this->actor.home.rot.z = this->actor.world.rot.z = this->actor.shape.rot.z = 0;
        Skin_AnimationWorkBuffer2_ct(play, &this->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
        Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx]);
        if ((this->actor.world.pos.x == -730.0f && this->actor.world.pos.y == 0.0f &&
             this->actor.world.pos.z == -1100.0f) ||
            (this->actor.world.pos.x == 880.0f && this->actor.world.pos.y == 0.0f &&
             this->actor.world.pos.z == -1170.0f)) {
            EN_HN_Actor_mode_doppel_init(this);
            return;
        }
    } else if (play->sceneId == SCENE_STABLE) {
        if (IS_DAY) {
            Actor_delete(&this->actor);
            return;
        } else {
            Skin_AnimationWorkBuffer2_ct(play, &this->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
            Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx]);
            EN_HN_Actor_mode_doppel_init(this);
            return;
        }
    } else if (play->sceneId == SCENE_GERUDOS_FORTRESS) {
        if (this->actor.world.pos.x == 3707.0f && this->actor.world.pos.y == 1413.0f &&
            this->actor.world.pos.z == -665.0f) {
            Skin_AnimationWorkBuffer2_ct(play, &this->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
            Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx]);
            EN_HN_Actor_mode_wait_init(this);
            return;
        }
        Skin_AnimationWorkBuffer2_ct(play, &this->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
        Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx]);
    } else {
        Skin_AnimationWorkBuffer2_ct(play, &this->skin, &gHorseNormalSkel, &gHorseNormalIdleAnim);
        Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx]);
    }
    if (PARAMS_GET_NOSHIFT(this->actor.params, 4, 4) == 0x10 && PARAMS_GET_U(this->actor.params, 0, 4) != 0xF) {
        EN_HN_Actor_mode_loop_path_init(this, play);
    } else {
        EN_HN_Actor_mode_free_init(this);
    }
}

void EN_HN_Actor_dt(Actor* thisx, PlayState* play) {
    EnHorseNormal* this = (EnHorseNormal*)thisx;

    Skin_AnimationWorkBuffer2_dt(play, &this->skin);
    ClObjPipe_dt(play, &this->bodyCollider);
    ClObjPipe_dt(play, &this->cloneCollider);
    ClObjJntSph_dt_nzf(play, &this->headCollider);
}

void EN_HN_Actor_mode_loop_path_init(EnHorseNormal* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->action = HORSE_FOLLOW_PATH;
    this->animationIdx = 6;
    this->waypoint = 0;
    this->actor.speed = 7.0f;
    Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx], EN_HN_Actor_calcAnimSpeed(this), 0.0f,
                     Si2_anime_end_frame(skin_horse_normal_anm_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void EN_HN_Actor_mode_loop_path(EnHorseNormal* this, PlayState* play) {
    Path* path = &play->pathList[PARAMS_GET_U(this->actor.params, 0, 4)];
    Vec3s* pointPos = SEGMENTED_TO_VIRTUAL(path->points);
    f32 dx;
    f32 dz;
    s32 pad;

    pointPos += this->waypoint;
    dx = pointPos->x - this->actor.world.pos.x;
    dz = pointPos->z - this->actor.world.pos.z;
    add_calc_short_angle2(&this->actor.world.rot.y, RAD_TO_BINANG(fatan2(dx, dz)), 0xA, 0x7D0, 1);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (SQ(dx) + SQ(dz) < 600.0f) {
        this->waypoint++;
        if (this->waypoint >= path->count) {
            this->waypoint = 0;
        }
    }
    this->skin.skelAnime.playSpeed = EN_HN_Actor_calcAnimSpeed(this);
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx], EN_HN_Actor_calcAnimSpeed(this), 0.0f,
                         Si2_anime_end_frame(skin_horse_normal_anm_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
        EN_HN_Actor_mode_run_sound_set(this);
    }
}

void EN_HN_Actor_mode_anim_loop_init(EnHorseNormal* this) {
    this->action = HORSE_CYCLE_ANIMATIONS;
    this->animationIdx++;

    if (this->animationIdx >= ARRAY_COUNT(skin_horse_normal_anm_tbl)) {
        this->animationIdx = 0;
    }

    Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx]);
}

void EN_HN_Actor_mode_anim_loop(EnHorseNormal* this, PlayState* play) {
    this->actor.speed = 0.0f;

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        EN_HN_Actor_mode_anim_loop_init(this);
    }
}

void EN_HN_Actor_mode_free_init(EnHorseNormal* this) {
    this->action = HORSE_WANDER;
    this->animationIdx = 0;
    this->actor.speed = 0.0f;
    this->unk_218 = 0.0f;
    this->unk_21C = 0;
    this->unk_21E = 0;
    Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx], EN_HN_Actor_calcAnimSpeed(this), 0.0f,
                     Si2_anime_end_frame(skin_horse_normal_anm_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void EN_HN_Actor_mode_run_sound_set(EnHorseNormal* this) {
    if (this->animationIdx == 5) {
        Nai_FxFlagEntry(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (this->animationIdx == 6) {
        Nai_FxFlagEntry(NA_SE_EV_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

void EN_HN_Actor_mode_setWaitSound(EnHorseNormal* this) {
    f32 frame = this->skin.skelAnime.curFrame;

    if (this->animationIdx == 0 && frame > 28.0f && !(this->unk_1E4 & 1)) {
        this->unk_1E4 |= 1;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_SANDDUST, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (this->animationIdx == 3 && frame > 25.0f && !(this->unk_1E4 & 2)) {
        this->unk_1E4 |= 2;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_LAND2, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

void EN_HN_Actor_mode_free(EnHorseNormal* this, PlayState* play) {
    static s32 wait[] = { 0, 1, 4, 5, 6, 2, 3 };
    static s32 anim_type_tbl[] = { 0, 0, 2, 2, 1, 1, 1, 3, 3 };
    s32 phi_t0 = this->animationIdx;
    s32 pad;

    switch (anim_type_tbl[this->animationIdx]) {
        case 0:
            EN_HN_Actor_mode_setWaitSound(this);
            this->actor.speed = 0.0f;
            this->unk_218 = 0.0f;
            break;
        case 1:
            if (fqrand() < 0.1f) {
                this->unk_218 = 2.0f * fqrand() - 1.0f;
            }
            this->actor.speed += this->unk_218;
            if (this->actor.speed <= 0.0f) {
                this->actor.speed = 0.0f;
                this->unk_218 = 0.0f;
                phi_t0 = 0;
            } else if (this->actor.speed < 3.0f) {
                EN_HN_setWalkSound(this);
                phi_t0 = 4;
            } else if (this->actor.speed < 6.0f) {
                phi_t0 = 5;
            } else if (this->actor.speed < 8.0f) {
                phi_t0 = 6;
            } else {
                this->actor.speed = 8.0f;
                phi_t0 = 6;
            }
            if (fqrand() < 0.1f || (this->unk_21E == 0 && ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) ||
                                                                 (this->bodyCollider.base.ocFlags1 & OC1_HIT) ||
                                                                 (this->headCollider.base.ocFlags1 & OC1_HIT)))) {
                this->unk_21E += (fqrand() * 30.0f) - 15.0f;
                if (this->unk_21E > 50) {
                    this->unk_21E = 50;
                } else if (this->unk_21E < -50) {
                    this->unk_21E = -50;
                }
            }
            this->unk_21C += this->unk_21E;
            if (this->unk_21C < -300) {
                this->unk_21C = -300;
            } else if (this->unk_21C > 300) {
                this->unk_21C = 300;
            } else if (fqrand() < 0.25f && fabsf(this->unk_21C) < 100.0f) {
                this->unk_21C = 0;
                this->unk_21E = 0;
            }
            this->actor.world.rot.y += this->unk_21C;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            break;
        case 2:
        case 3:
            break;
    }

    if (phi_t0 != this->animationIdx || Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        if (phi_t0 != this->animationIdx) {
            this->animationIdx = phi_t0;
            this->unk_1E4 &= ~1;
            this->unk_1E4 &= ~2;
            if (phi_t0 == 1) {
                Nai_FxFlagEntry(NA_SE_EV_HORSE_GROAN, &this->unk_204, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            } else if (phi_t0 == 3) {
                Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_204, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            } else {
                EN_HN_Actor_mode_run_sound_set(this);
            }
            Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx], EN_HN_Actor_calcAnimSpeed(this), 0.0f,
                             Si2_anime_end_frame(skin_horse_normal_anm_tbl[this->animationIdx]), ANIMMODE_ONCE, -3.0f);
        } else {
            switch (anim_type_tbl[this->animationIdx]) {
                case 0:
                    if (fqrand() < 0.25f) {
                        this->unk_218 = 1.0f;
                        phi_t0 = 4;
                    } else {
                        phi_t0 = wait[(s32)(fqrand() * 2)];
                        this->actor.speed = 0.0f;
                        this->unk_218 = 0.0f;
                    }
                    break;
                case 1:
                case 2:
                case 3:
                    break;
            }

            this->unk_1E4 &= ~1;
            this->unk_1E4 &= ~2;
            if (phi_t0 == 1) {
                Nai_FxFlagEntry(NA_SE_EV_HORSE_GROAN, &this->unk_204, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            } else if (phi_t0 == 3) {
                Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_204, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            } else {
                EN_HN_Actor_mode_run_sound_set(this);
            }
            if (phi_t0 != this->animationIdx) {
                this->animationIdx = phi_t0;
                Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx], EN_HN_Actor_calcAnimSpeed(this), 0.0f,
                                 Si2_anime_end_frame(skin_horse_normal_anm_tbl[this->animationIdx]), ANIMMODE_ONCE, -3.0f);
            } else {
                Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx], EN_HN_Actor_calcAnimSpeed(this), 0.0f,
                                 Si2_anime_end_frame(skin_horse_normal_anm_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
            }
        }
    }
}

void EN_HN_Actor_mode_wait_init(EnHorseNormal* this) {
    this->action = HORSE_WAIT;
    this->animationIdx = 0;
    this->actor.speed = 0.0f;
    this->unk_218 = 0.0f;
    this->unk_21C = 0;
    this->unk_21E = 0;
    Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx], EN_HN_Actor_calcAnimSpeed(this), 0.0f,
                     Si2_anime_end_frame(skin_horse_normal_anm_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void EN_HN_Actor_mode_wait(EnHorseNormal* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        f32 rand = fqrand();

        if (rand < 0.4f) {
            this->animationIdx = 0;
        } else if (rand < 0.8f) {
            this->animationIdx = 1;
            Nai_FxFlagEntry(NA_SE_EV_HORSE_GROAN, &this->unk_204, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            this->animationIdx = 3;
            Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_204, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }

        Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx], EN_HN_Actor_calcAnimSpeed(this), 0.0f,
                         Si2_anime_end_frame(skin_horse_normal_anm_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
    }
}

void EN_HN_Actor_mode_doppel_init(EnHorseNormal* this) {
    this->action = HORSE_WAIT_CLONE;
    this->animationIdx = 0;
    this->actor.speed = 0.0f;
    this->unk_218 = 0.0f;
    this->unk_21C = 0;
    this->unk_21E = 0;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
    Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx], EN_HN_Actor_calcAnimSpeed(this), 0.0f,
                     Si2_anime_end_frame(skin_horse_normal_anm_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void EN_HN_Actor_mode_doppel(EnHorseNormal* this, PlayState* play) {
    EN_HN_Actor_mode_setWaitSound(this);

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        f32 rand = fqrand();

        if (rand < 0.4f) {
            this->animationIdx = 0;
        } else if (rand < 0.8f) {
            this->animationIdx = 1;
            this->unk_1E4 |= 0x20;
            Nai_FxFlagEntry(NA_SE_EV_HORSE_GROAN, &this->unk_204, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            this->animationIdx = 3;
            this->unk_1E4 |= 0x20;
            Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_204, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }

        Skeleton_Info2_init(&this->skin.skelAnime, skin_horse_normal_anm_tbl[this->animationIdx], EN_HN_Actor_calcAnimSpeed(this), 0.0f,
                         Si2_anime_end_frame(skin_horse_normal_anm_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0f);

        this->unk_1E4 &= ~1;
        this->unk_1E4 &= ~2;
        this->unk_1E4 &= ~8;
        this->unk_1E4 &= ~0x10;
    }
}

void EN_HN_xrot_calc(EnHorseNormal* this, PlayState* play) {
    s32 pad;
    CollisionPoly* groundPoly;
    s32 pad2;
    Vec3f checkPos;
    s32 bgId;

    checkPos.x = (sin_s(this->actor.shape.rot.y) * 30.0f) + this->actor.world.pos.x;
    checkPos.y = this->actor.world.pos.y + 60.0f;
    checkPos.z = (cos_s(this->actor.shape.rot.y) * 30.0f) + this->actor.world.pos.z;
    this->unk_220 = T_BGCheck_ObjGroundCheck_ai(&play->colCtx, &groundPoly, &bgId, &checkPos);
    this->actor.shape.rot.x = RAD_TO_BINANG(fatan2(this->actor.world.pos.y - this->unk_220, 30.0f));
}

static EnHorseNormalActionFunc mode_function[] = {
    EN_HN_Actor_mode_anim_loop, EN_HN_Actor_mode_free,     EN_HN_Actor_mode_wait,
    EN_HN_Actor_mode_doppel,       EN_HN_Actor_mode_loop_path,
};

void EN_HN_move(Actor* thisx, PlayState* play) {
    EnHorseNormal* this = (EnHorseNormal*)thisx;
    s32 pad;

    mode_function[this->action](this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 20.0f, 35.0f, 100.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    if (play->sceneId == SCENE_LON_LON_RANCH && this->actor.world.pos.z < -2400.0f) {
        this->actor.world.pos.z = -2400.0f;
    }
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    this->unk_204 = this->actor.projectedPos;
    this->unk_204.y += 120.0f;
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->bodyCollider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->bodyCollider.base);
    if (this->actor.speed == 0.0f) {
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    } else {
        this->actor.colChkInfo.mass = MASS_HEAVY;
    }
}

void EN_HN_calc(Actor* thisx, PlayState* play, Skin* skin) {
    Vec3f sp4C;
    Vec3f sp40;
    EnHorseNormal* this = (EnHorseNormal*)thisx;
    s32 i;

    for (i = 0; i < this->headCollider.count; i++) {
        sp4C.x = this->headCollider.elements[i].dim.modelSphere.center.x;
        sp4C.y = this->headCollider.elements[i].dim.modelSphere.center.y;
        sp4C.z = this->headCollider.elements[i].dim.modelSphere.center.z;
        Skin_MatrixPosition2_gfx(skin, this->headCollider.elements[i].dim.limb, &sp4C, &sp40);
        this->headCollider.elements[i].dim.worldSphere.center.x = sp40.x;
        this->headCollider.elements[i].dim.worldSphere.center.y = sp40.y;
        this->headCollider.elements[i].dim.worldSphere.center.z = sp40.z;
        this->headCollider.elements[i].dim.worldSphere.radius =
            this->headCollider.elements[i].dim.modelSphere.radius * this->headCollider.elements[i].dim.scale;
    }

    //! @bug see relevant comment in `EnHorse_SkinCallback1`
    CollisionCheck_setOC(play, &play->colChkCtx, &this->headCollider.base);
}

void EN_HN_Actor_mode_setSound_forDoppel(PlayState* play, EnHorseNormal* this, Vec3f* arg2) {
    f32 curFrame = this->skin.skelAnime.curFrame;
    f32 wDest;

    Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, arg2, &this->unk_1E8, &wDest);
    this->unk_1F4 = this->unk_1E8;
    this->unk_1F4.y += 120.0f;

    if (this->animationIdx == 0 && curFrame > 28.0f && !(this->unk_1E4 & 8)) {
        this->unk_1E4 |= 8;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_SANDDUST, &this->unk_1E8, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (this->animationIdx == 3 && curFrame > 25.0f && !(this->unk_1E4 & 0x10)) {
        this->unk_1E4 |= 0x10;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_LAND2, &this->unk_1E8, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (this->animationIdx == 3 && this->unk_1E4 & 0x20) {
        this->unk_1E4 &= ~0x20;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->unk_1F4, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    } else if (this->animationIdx == 1 && this->unk_1E4 & 0x20) {
        this->unk_1E4 &= ~0x20;
        Nai_FxFlagEntry(NA_SE_EV_HORSE_GROAN, &this->unk_1F4, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

void EN_HN_display(Actor* thisx, PlayState* play2) {
    EnHorseNormal* this = (EnHorseNormal*)thisx;
    PlayState* play = (PlayState*)play2;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_horse_normal.c", 2224);

    if (play->sceneId != SCENE_LON_LON_RANCH || play->sceneId != SCENE_STABLE) {
        EN_HN_xrot_calc(this, play);
    }
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Skin_disp2(&this->actor, play, &this->skin, EN_HN_calc, true);

    if (this->action == HORSE_WAIT_CLONE) {
        MtxF skinMtx;
        Mtx* mtx;
        Vec3f clonePos = { 0.0f, 0.0f, 0.0f };
        s16 cloneRotY;
        f32 distFromGround = this->actor.world.pos.y - this->actor.floorHeight;
        f32 temp_f0_4;

        if (play->sceneId == SCENE_STABLE) {
            if (this->actor.world.pos.x == 355.0f && this->actor.world.pos.y == 0.0f &&
                this->actor.world.pos.z == -245.0f) {
                clonePos.x = 235.0f;
                clonePos.y = 0.0f;
                clonePos.z = 100.0f;
                cloneRotY = 0x7FFF;
            } else if (this->actor.world.pos.x == 238.0f && this->actor.world.pos.y == 0.0f &&
                       this->actor.world.pos.z == -245.0f) {
                clonePos.x = 478.0f;
                clonePos.y = 0.0f;
                clonePos.z = 100.0f;
                cloneRotY = 0x7FFF;
            }
        } else if (play->sceneId == SCENE_LON_LON_RANCH) {
            if (this->actor.world.pos.x == -730.0f && this->actor.world.pos.y == 0.0f &&
                this->actor.world.pos.z == -1100.0f) {
                clonePos.x = 780.0f;
                clonePos.y = 0.0f;
                clonePos.z = -80.0f;
                cloneRotY = 0;
            } else if (this->actor.world.pos.x == 880.0f && this->actor.world.pos.y == 0.0f &&
                       this->actor.world.pos.z == -1170.0f) {
                clonePos.x = -1000.0f;
                clonePos.y = 0.0f;
                clonePos.z = -70.0f;
                cloneRotY = 0;
            }
        }
        EN_HN_Actor_mode_setSound_forDoppel(play, this, &clonePos);
        Skin_Matrix_SetSRzxyTMatrix(&skinMtx, this->actor.scale.x, this->actor.scale.y, this->actor.scale.z,
                                              this->actor.shape.rot.x, cloneRotY, this->actor.shape.rot.z, clonePos.x,
                                              (this->actor.shape.yOffset * this->actor.scale.y) + clonePos.y,
                                              clonePos.z);
        mtx = Skin_Matrix_to_Mtx_new(play->state.gfxCtx, &skinMtx);
        if (mtx == NULL) {
            return;
        }
        gSPMatrix(POLY_OPA_DISP++, &Mtx_clear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        Skin_disp5(&this->actor, play, &this->skin, NULL, NULL, true, 0,
                      SKIN_DRAW_FLAG_CUSTOM_TRANSFORMS | SKIN_DRAW_FLAG_CUSTOM_MATRIX);
        this->cloneCollider.dim.pos.x = clonePos.x;
        this->cloneCollider.dim.pos.y = clonePos.y;
        this->cloneCollider.dim.pos.z = clonePos.z;
        CollisionCheck_setOC(play, &play->colChkCtx, &this->cloneCollider.base);
        _texture_decal_shadow(play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);
        Matrix_translate(clonePos.x, clonePos.y, clonePos.z, MTXMODE_NEW);
        temp_f0_4 = (1.0f - (distFromGround * 0.01f)) * this->actor.shape.shadowScale;
        Matrix_scale(this->actor.scale.x * temp_f0_4, 1.0f, this->actor.scale.z * temp_f0_4, MTXMODE_APPLY);
        Matrix_rotateY(BINANG_TO_RAD(cloneRotY), MTXMODE_APPLY);
        mtx = MATRIX_FINALIZE(play->state.gfxCtx, "../z_en_horse_normal.c", 2329);
        if (mtx != NULL) {
            gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gHorseShadowDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_horse_normal.c", 2339);
}
