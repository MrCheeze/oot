/*
 * File: z_boss_ganondrof.c
 * Overlay: ovl_Boss_Ganondrof
 * Description: Phantom Ganon
 */

#include "z_boss_ganondrof.h"
#include "assets/objects/object_gnd/object_gnd.h"
#include "overlays/actors/ovl_En_fHG/z_en_fhg.h"
#include "overlays/actors/ovl_En_Fhg_Fire/z_en_fhg_fire.h"
#include "overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

typedef enum BossGanondrofDeathState {
    /* 0 */ NOT_DEAD,
    /* 1 */ DEATH_START,
    /* 2 */ DEATH_THROES,
    /* 3 */ DEATH_WARP,
    /* 4 */ DEATH_SCREAM,
    /* 5 */ DEATH_DISINTEGRATE,
    /* 6 */ DEATH_FINISH
} BossGanondrofDeathState;

typedef enum BossGanondrofThrowAction {
    /* 0 */ THROW_NORMAL,
    /* 1 */ THROW_SLOW
} BossGanondrofThrowAction;

typedef enum BossGanondrofStunnedAction {
    /* 0 */ STUNNED_FALL,
    /* 1 */ STUNNED_GROUND
} BossGanondrofStunnedAction;

typedef enum BossGanondrofChargeAction {
    /* 0 */ CHARGE_WINDUP,
    /* 1 */ CHARGE_START,
    /* 2 */ CHARGE_ATTACK,
    /* 3 */ CHARGE_FINISH
} BossGanondrofChargeAction;

typedef enum BossGanondrofDeathAction {
    /* 0 */ DEATH_SPASM,
    /* 1 */ DEATH_LIMP,
    /* 2 */ DEATH_HUNCHED
} BossGanondrofDeathAction;

void Boss_Ganondrof_actor_ct(Actor* thisx, PlayState* play);
void Boss_Ganondrof_actor_dt(Actor* thisx, PlayState* play);
void Boss_Ganondrof_actor_move(Actor* thisx, PlayState* play);
void Boss_Ganondrof_actor_draw(Actor* thisx, PlayState* play);

static void mode_startdemo_init(BossGanondrof* this, PlayState* play);
static void mode_startdemo(BossGanondrof* this, PlayState* play);
void mode_horserun_init(BossGanondrof* this);
void mode_horserun(BossGanondrof* this, PlayState* play);
static void mode_flying_init(BossGanondrof* this, f32 arg1);
static void mode_flying(BossGanondrof* this, PlayState* play);
void mode_fly_atack_init(BossGanondrof* this, PlayState* play);
void mode_fly_atack(BossGanondrof* this, PlayState* play);
void mode_intersept_init(BossGanondrof* this, PlayState* play);
void mode_intersept(BossGanondrof* this, PlayState* play);
void mode_ball_revise_init(BossGanondrof* this, PlayState* play);
void mode_ball_revise(BossGanondrof* this, PlayState* play);
void mode_fly_atack2_init(BossGanondrof* this, PlayState* play);
void mode_fly_atack2(BossGanondrof* this, PlayState* play);
void mode_fly_damage(BossGanondrof* this, PlayState* play);
static void mode_enddemo(BossGanondrof* this, PlayState* play);

ActorProfile Boss_Ganondrof_Profile = {
    /**/ ACTOR_BOSS_GANONDROF,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_GND,
    /**/ sizeof(BossGanondrof),
    /**/ Boss_Ganondrof_actor_ct,
    /**/ Boss_Ganondrof_actor_dt,
    /**/ Boss_Ganondrof_actor_move,
    /**/ Boss_Ganondrof_actor_draw,
};

static ColliderCylinderInit GanondrofOcInfoData = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0xFFCFFFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 90, -50, { 0, 0, 0 } },
};

static ColliderCylinderInit GanondrofOcInfoData2 = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x30 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 30, -20, { 0, 0, 0 } },
};

// clang-format off
static u8 mesh_data_GND_2[16 * 16] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,
    1,0,1,1,0,0,0,0,1,1,1,1,1,1,0,1,
    1,0,1,1,1,0,0,0,0,1,1,1,1,1,0,1,
    1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,1,
    1,0,0,1,1,1,1,1,0,0,0,1,1,0,0,1,
    1,0,1,1,1,1,0,0,0,0,0,1,0,0,0,1,
    1,1,1,1,1,1,1,1,0,0,0,0,1,1,0,1,
    1,0,1,1,1,1,1,0,0,0,0,1,1,1,0,1,
    1,0,0,1,1,1,0,0,0,1,1,1,1,1,0,1,
    1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,
    1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,
    1,0,0,1,1,1,1,0,1,1,1,1,1,1,1,1,
    1,0,1,1,1,1,1,0,0,1,1,1,1,1,0,1,
    1,1,1,1,1,1,1,0,0,1,1,1,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

static u8 mesh_data_GND_1[16 * 16] = {
    1,1,1,0,1,0,0,1,0,0,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,0,
    1,0,1,1,0,0,0,0,0,1,1,0,0,1,0,0,
    1,0,0,1,1,0,0,0,0,0,1,1,1,0,0,0,
    0,0,0,1,1,1,0,0,0,0,0,1,1,0,0,1,
    0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,
    1,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
    1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,
    1,0,0,0,1,0,0,0,0,0,0,0,1,0,0,1,
    0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
    0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
    1,0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,
    1,0,0,0,0,1,0,0,0,0,1,0,1,1,0,0,
    0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,
    1,0,0,0,1,0,1,0,0,0,1,1,0,0,0,1,
    1,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,
};

static u8 mesh_data_GND_CL[16 * 16] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
// clang-format on

// These are Phantom Ganon's body textures, but I don't know which is which.
static void* tex88[] = {
    gPhantomGanonLimbTex_00A800, gPhantomGanonLimbTex_00AE80, gPhantomGanonLimbTex_00AF00,
    gPhantomGanonLimbTex_00C180, gPhantomGanonLimbTex_00C400,
};
static void* tex816[] = {
    gPhantomGanonLimbTex_00B980, gPhantomGanonLimbTex_00C480, gPhantomGanonLimbTex_00BC80,
    gPhantomGanonLimbTex_00BD80, gPhantomGanonLimbTex_00C080,
};
static void* tex1616[] = {
    gPhantomGanonLimbTex_00C200, gPhantomGanonLimbTex_00A000, gPhantomGanonLimbTex_00A200,
    gPhantomGanonLimbTex_00A400, gPhantomGanonLimbTex_00A600, gPhantomGanonLimbTex_00A880,
    gPhantomGanonLimbTex_00B780, gPhantomGanonLimbTex_00BA80, gPhantomGanonLimbTex_00BE80,
};
static void* tex3216[] = { gPhantomGanonLimbTex_00AA80, gPhantomGanonLimbTex_00AF80 };

static void* tex1616c[] = { gPhantomGanonMouthTex, gPhantomGanonSmileTex };

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_PHANTOM_GANON_PHASE_1, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 0, ICHAIN_STOP),
};

static Vec3f gnd_se_pos = { 0.0f, 0.0f, 50.0f };

void texcel_mesh_88_GND(s16* texture, u8* mask, s16 index) {
    if (mask[index]) {
        texture[index / 4] = 0;
    }
}

void texcel_mesh_816_GND(s16* texture, u8* mask, s16 index) {
    if (mask[index]) {
        texture[index / 2] = 0;
    }
}

void texcel_mesh_1616_GND(s16* texture, u8* mask, s16 index) {
    if (mask[index]) {
        texture[index] = 0;
    }
}

void texcel_mesh_1632_GND(s16* texture, u8* mask, s16 index) {
    if (mask[index]) {
        s16 i = (index & 0xF) + ((index & 0xF0) << 1);

        texture[i + 0x10] = 0;
        texture[i] = 0;
    }
}

void texcel_mesh_3216_GND(s16* texture, u8* mask, s16 index) {
    if (mask[index]) {
        s16 i = ((index & 0xF) * 2) + ((index & 0xF0) * 2);

        texture[i + 1] = 0;
        texture[i] = 0;
    }
}

void tex_mesh_GND(u8* mask, s16 index) {
    s16 i;

    for (i = 0; i < 5; i++) {
        // ARRAY_COUNT can't be used here because the arrays aren't guaranteed to be the same size.
        texcel_mesh_88_GND(SEGMENTED_TO_VIRTUAL(tex88[i]), mask, index);
        texcel_mesh_816_GND(SEGMENTED_TO_VIRTUAL(tex816[i]), mask, index);
    }

    for (i = 0; i < ARRAY_COUNT(tex1616); i++) {
        texcel_mesh_1616_GND(SEGMENTED_TO_VIRTUAL(tex1616[i]), mask, index);
    }

    for (i = 0; i < ARRAY_COUNT(tex3216); i++) {
        texcel_mesh_3216_GND(SEGMENTED_TO_VIRTUAL(tex3216[i]), mask, index);
    }

    texcel_mesh_1632_GND(SEGMENTED_TO_VIRTUAL(gPhantomGanonLimbTex_00B380), mask, index);
    texcel_mesh_3216_GND(SEGMENTED_TO_VIRTUAL(gPhantomGanonEyeTex), mask, index);
    for (i = 0; i < ARRAY_COUNT(tex1616c); i++) {
        texcel_mesh_1616_GND(SEGMENTED_TO_VIRTUAL(tex1616c[i]), mask, index);
    }
}

static void CollisionCheck_Uty_PosSetPipeC(Vec3f* pos, ColliderCylinder* collider) {
    collider->dim.pos.x = pos->x;
    collider->dim.pos.y = pos->y;
    collider->dim.pos.z = pos->z;
}

void Boss_Ganondrof_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BossGanondrof* this = (BossGanondrof*)thisx;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, NULL, 0.0f);
    Actor_set_scale(&this->actor, 0.01f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gPhantomGanonSkel, &gPhantomGanonRideAnim, NULL, NULL, 0);
    if (this->actor.params < GND_FAKE_BOSS) {
        this->actor.params = GND_REAL_BOSS;
        this->actor.colChkInfo.health = 30;
        this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
        Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 255, 255, 255, 255);
        mode_startdemo_init(this, play);
    } else {
        mode_horserun_init(this);
    }

    ClObjPipe_ct(play, &this->colliderBody);
    ClObjPipe_ct(play, &this->colliderSpear);
    ClObjPipe_set5(play, &this->colliderBody, &this->actor, &GanondrofOcInfoData);
    ClObjPipe_set5(play, &this->colliderSpear, &this->actor, &GanondrofOcInfoData2);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (Actor_Environment_room_clear_Check(play, play->roomCtx.curRoom.num)) {
        Actor_delete(&this->actor);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_WARP1, GND_BOSSROOM_CENTER_X, GND_BOSSROOM_CENTER_Y,
                    GND_BOSSROOM_CENTER_Z, 0, 0, 0, WARP_DUNGEON_ADULT);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, 200.0f + GND_BOSSROOM_CENTER_X, GND_BOSSROOM_CENTER_Y,
                    GND_BOSSROOM_CENTER_Z, 0, 0, 0, 0);
    } else {
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, this->actor.params);
    }
}

void Boss_Ganondrof_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BossGanondrof* this = (BossGanondrof*)thisx;

    PRINTF("DT1\n");
    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjPipe_dt(play, &this->colliderBody);
    ClObjPipe_dt(play, &this->colliderSpear);
    if (this->actor.params == GND_REAL_BOSS) {
        Global_light_list_delete(play, &play->lightCtx, this->lightNode);
    }

    PRINTF("DT2\n");
}

static void mode_startdemo_init(BossGanondrof* this, PlayState* play) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gPhantomGanonRidePoseAnim);
    this->actionFunc = mode_startdemo;
    this->work[GND_MASK_OFF] = true;
}

static void mode_startdemo(BossGanondrof* this, PlayState* play) {
    s16 i;
    s32 pad;
    EnfHG* horse = (EnfHG*)this->actor.child;

    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actor.world.pos = horse->actor.world.pos;
    this->actor.shape.rot.y = this->actor.world.rot.y = horse->actor.world.rot.y;

    PRINTF("SW %d------------------------------------------------\n", horse->bossGndSignal);

    if ((this->timers[1] != 0) && (this->timers[1] < 25)) {
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };

        pos.x = this->bodyPartsPos[14].x + rnd_fx(10.0f);
        pos.y = this->bodyPartsPos[14].y + rnd_f(-5.0f);
        pos.z = this->bodyPartsPos[14].z + rnd_fx(10.0f) + 5.0f;
        accel.y = 0.03f;
        Effect_k_fire_ct(play, &pos, &vel, &accel, (s16)rnd_f(10.0f) + 5, 0);
    }

    if (this->timers[1] == 20) {
        this->work[GND_MASK_OFF] = false;
    }

    if (this->timers[1] == 30) {
        Na_StartObjectSe_F(&gnd_se_pos, NA_SE_EN_FANTOM_TRANSFORM);
    }

    if (horse->bossGndSignal == FHG_LIGHTNING) {
        Skeleton_Info2_init(&this->skelAnime, &gPhantomGanonMaskOnAnim, 0.5f, 0.0f,
                         Si2_anime_end_frame(&gPhantomGanonMaskOnAnim), ANIMMODE_ONCE_INTERP, 0.0f);
        this->timers[1] = 40;
    }

    if (horse->bossGndSignal == FHG_REAR) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPhantomGanonHorseRearingAnim, -3.0f);
    }

    if (horse->bossGndSignal == FHG_RIDE) {
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonRidePoseAnim, -13.0f);
    }

    if (horse->bossGndSignal == FHG_SPUR) {
        EnfHG* horseTemp;

        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPhantomGanonRideSpearRaiseAnim, -7.0f);
        horseTemp = (EnfHG*)this->actor.child;
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->spearTip.x, this->spearTip.y,
                           this->spearTip.z, 50, FHGFIRE_LIGHT_GREEN, 0, FHGFIRE_SPEAR_LIGHT);
        this->actor.child = &horseTemp->actor;
    }

    if (horse->bossGndSignal == FHG_FINISH) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPhantomGanonRideSpearResetAnim, -5.0f);
    }

    switch (this->work[GND_EYE_STATE]) {
        case GND_EYESTATE_FADE:
            this->fwork[GND_EYE_ALPHA] += 40.0f;
            if (this->fwork[GND_EYE_ALPHA] >= 255.0f) {
                this->fwork[GND_EYE_ALPHA] = 255.0f;
            }
            break;

        case GND_EYESTATE_BRIGHTEN:
            this->fwork[GND_EYE_BRIGHTNESS] += 20.0f;
            if (this->fwork[GND_EYE_BRIGHTNESS] > 255.0f) {
                this->fwork[GND_EYE_BRIGHTNESS] = 255.0f;
            }
            break;
    }

    this->armRotY = sin_s(this->work[GND_VARIANCE_TIMER] * 0x6E8) * 0;
    this->armRotZ = cos_s(this->work[GND_VARIANCE_TIMER] * 0x8DC) * 300.0f;
    for (i = 0; i < 30; i++) {
        this->rideRotY[i] = sin_s(this->work[GND_VARIANCE_TIMER] * ((i * 50) + 0x7B0)) * 100.0f;
        this->rideRotZ[i] = cos_s(this->work[GND_VARIANCE_TIMER] * ((i * 50) + 0x8DC)) * 100.0f;
    }

    if (horse->bossGndSignal == FHG_START_FIGHT) {
        mode_horserun_init(this);
        for (i = 0; i < 30; i++) {
            this->rideRotY[i] = this->rideRotZ[i] = 0.0f;
        }
    }

    horse->bossGndSignal = FHG_NO_SIGNAL;
}

void mode_horserun_init(BossGanondrof* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonRideAnim, -5.0f);
    this->actionFunc = mode_horserun;
}

void mode_horserun(BossGanondrof* this, PlayState* play) {
    EnfHG* horse = (EnfHG*)this->actor.child;

    PRINTF("RUN 1\n");
    Skeleton_Info2_anime_play(&this->skelAnime);
    PRINTF("RUN 2\n");

    if (horse->bossGndSignal == FHG_RAISE_SPEAR) {
        EnfHG* horseTemp;

        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPhantomGanonRideSpearRaiseAnim, -2.0f);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        horseTemp = (EnfHG*)this->actor.child;
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->spearTip.x, this->spearTip.y,
                           this->spearTip.z, 30, FHGFIRE_LIGHT_GREEN, 0, FHGFIRE_SPEAR_LIGHT);
        this->actor.child = &horseTemp->actor;
    } else if (horse->bossGndSignal == FHG_LIGHTNING) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPhantomGanonRideSpearStrikeAnim, -2.0f);
    } else if (horse->bossGndSignal == FHG_RESET) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPhantomGanonRideSpearResetAnim, -2.0f);
    } else if (horse->bossGndSignal == FHG_RIDE) {
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonRideAnim, -2.0f);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }

    PRINTF("RUN 3\n");
    this->actor.world.pos = horse->actor.world.pos;
    this->actor.world.pos.y = horse->actor.world.pos.y;
    this->actor.shape.rot.y = this->actor.world.rot.y = horse->actor.world.rot.y;
    if (this->flyMode != GND_FLY_PAINTING) {
        mode_flying_init(this, -20.0f);
        this->timers[0] = 100;
        this->colliderBody.dim.radius = 20;
        this->colliderBody.dim.height = 60;
        this->colliderBody.dim.yShift = -33;
        Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_LAUGH);
        this->actor.naviEnemyId = NAVI_ENEMY_PHANTOM_GANON_PHASE_2;
    } else {
        horse->bossGndSignal = FHG_NO_SIGNAL;
        this->actor.scale.x = horse->actor.scale.x / 1.15f;
        this->actor.scale.y = horse->actor.scale.y / 1.15f;
        this->actor.scale.z = horse->actor.scale.z / 1.15f;
        PRINTF("RUN 4\n");
    }
}

static void mode_flying_init(BossGanondrof* this, f32 arg1) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonNeutralAnim, arg1);
    this->actionFunc = mode_flying;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->fwork[GND_FLOAT_SPEED] = 0.0f;
    this->timers[0] = (s16)(fqrand() * 64.0f) + 30;
}

static void mode_flying(BossGanondrof* this, PlayState* play) {
    f32 targetX;
    f32 targetY;
    f32 targetZ;
    Player* player = GET_PLAYER(play);
    Actor* playerx = &player->actor;
    Actor* thisx = &this->actor;
    f32 rand01;
    s16 i;

    Skeleton_Info2_anime_play(&this->skelAnime);
    switch (this->flyMode) {
        case GND_FLY_NEUTRAL:
            if (this->timers[0] == 0) {
                this->timers[0] = (s16)(fqrand() * 64.0f) + 30;
                rand01 = fqrand();
                if (thisx->colChkInfo.health < 5) {
                    if (rand01 < 0.25f) {
                        mode_fly_atack_init(this, play);
                    } else if (rand01 >= 0.8f) {
                        this->flyMode = GND_FLY_CHARGE;
                        this->timers[0] = 60;
                        this->fwork[GND_FLOAT_SPEED] = 0.0f;
                        Actor_SE_set(thisx, NA_SE_EN_FANTOM_LAUGH);
                    } else {
                        this->flyMode = GND_FLY_VOLLEY;
                        this->timers[0] = 60;
                        this->fwork[GND_FLOAT_SPEED] = 0.0f;
                        Actor_SE_set(thisx, NA_SE_EN_FANTOM_LAUGH);
                    }
                } else if ((rand01 < 0.5f) || (this->work[GND_THROW_COUNT] < 5)) {
                    mode_fly_atack_init(this, play);
                } else {
                    this->flyMode = GND_FLY_VOLLEY;
                    this->timers[0] = 60;
                    this->fwork[GND_FLOAT_SPEED] = 0.0f;
                    Actor_SE_set(thisx, NA_SE_EN_FANTOM_LAUGH);
                }
            }

            if (this->timers[1] != 0) {
                targetX = GND_BOSSROOM_CENTER_X;
                targetZ = GND_BOSSROOM_CENTER_Z;
            } else {
                targetX = playerx->world.pos.x + (180.0f * sin_s(playerx->shape.rot.y));
                targetZ = playerx->world.pos.z + (180.0f * cos_s(playerx->shape.rot.y));
                if (sqrtf(SQ(targetX - GND_BOSSROOM_CENTER_X) + SQ(targetZ - GND_BOSSROOM_CENTER_Z)) > 280.0f) {
                    this->timers[1] = 50;
                    this->fwork[GND_FLOAT_SPEED] = 0.0f;
                }
            }

            targetY = playerx->world.pos.y + 100.0f + 0.0f;
            targetX += sin_s(this->work[GND_VARIANCE_TIMER] * 0x500) * 100.0f;
            targetZ += cos_s(this->work[GND_VARIANCE_TIMER] * 0x700) * 100.0f;
            break;
        case GND_FLY_VOLLEY:
            targetX = GND_BOSSROOM_CENTER_X - 14.0f;
            targetZ = GND_BOSSROOM_CENTER_Z + 265.0f;

            targetY = playerx->world.pos.y + 100.0f + 100.0f;
            targetX += sin_s(this->work[GND_VARIANCE_TIMER] * 0x500) * 100.0f;
            targetZ += cos_s(this->work[GND_VARIANCE_TIMER] * 0x700) * 100.0f;
            if (this->timers[0] == 0) {
                this->flyMode = GND_FLY_RETURN;
                this->returnSuccess = false;
                mode_fly_atack_init(this, play);
                this->timers[0] = 80;
            }
            break;
        case GND_FLY_RETURN:
            targetX = GND_BOSSROOM_CENTER_X - 14.0f;
            targetZ = GND_BOSSROOM_CENTER_Z + 265.0f;

            targetY = playerx->world.pos.y + 100.0f + 100.0f;
            targetX += sin_s(this->work[GND_VARIANCE_TIMER] * 0x500) * 50.0f;
            targetZ += cos_s(this->work[GND_VARIANCE_TIMER] * 0x700) * 50.0f;
            if (this->returnSuccess) {
                this->returnSuccess = false;
                mode_ball_revise_init(this, play);
                this->timers[0] = 80;
            }

            if (this->timers[0] == 0) {
                this->flyMode = GND_FLY_NEUTRAL;
            }
            break;
        case GND_FLY_CHARGE:
            targetX = GND_BOSSROOM_CENTER_X - 14.0f;
            targetZ = GND_BOSSROOM_CENTER_Z + 215.0f;

            targetY = playerx->world.pos.y + 100.0f + 50.0f;
            targetX += sin_s(this->work[GND_VARIANCE_TIMER] * 0x500) * 100.0f;
            targetZ += cos_s(this->work[GND_VARIANCE_TIMER] * 0x700) * 100.0f;
            if (this->timers[0] == 0) {
                mode_fly_atack2_init(this, play);
            }
            break;
    }

    add_calc2(&thisx->world.pos.x, targetX, 0.05f, this->fwork[GND_FLOAT_SPEED]);
    if (this->timers[2] != 0) {
        add_calc2(&thisx->world.pos.y, targetY + 100.0f, 0.1f, 50.0f);
    } else {
        add_calc2(&thisx->world.pos.y, targetY, 0.05f, 10.0f);
    }

    add_calc2(&thisx->world.pos.z, targetZ, 0.05f, this->fwork[GND_FLOAT_SPEED]);
    add_calc2(&this->fwork[GND_FLOAT_SPEED], 50.0f, 1.0f, 0.5f);
    thisx->velocity.x = thisx->world.pos.x - thisx->prevPos.x;
    thisx->velocity.z = thisx->world.pos.z - thisx->prevPos.z;
    thisx->world.pos.y += 2.0f * sin_s(this->work[GND_VARIANCE_TIMER] * 1500);
    adds(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0xBB8);
    if ((this->work[GND_VARIANCE_TIMER] & 1) == 0) {
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };

        for (i = 0; i < 3; i++) {
            pos.x = rnd_fx(20.0f) + this->spearTip.x;
            pos.y = rnd_fx(20.0f) + this->spearTip.y;
            pos.z = rnd_fx(20.0f) + this->spearTip.z;
            accel.y = -0.08f;
            Effect_fhg_flash_ct(play, &pos, &vel, &accel, (s16)(fqrand() * 80.0f) + 150,
                                            FHGFLASH_LIGHTBALL_GREEN);
        }
    }

    if (player->unk_A73 != 0) {
        mode_intersept_init(this, play);
    }

    Actor_SE_set(thisx, NA_SE_EN_FANTOM_FLOAT - SFX_FLAG);
}

void mode_fly_atack_init(BossGanondrof* this, PlayState* play) {
    EnfHG* horseTemp;
    s16 lightTime;

    this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonThrowAnim);
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPhantomGanonThrowAnim, -5.0f);
    this->actionFunc = mode_fly_atack;
    if ((fqrand() <= 0.1f) && (this->work[GND_THROW_COUNT] >= 10) && (this->flyMode == GND_FLY_NEUTRAL)) {
        this->work[GND_ACTION_STATE] = THROW_SLOW;
        this->work[GND_THROW_FRAME] = 1000;
        lightTime = 32;
    } else {
        this->work[GND_ACTION_STATE] = THROW_NORMAL;
        this->work[GND_THROW_FRAME] = 25;
        lightTime = 25;
    }

    horseTemp = (EnfHG*)this->actor.child;
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->spearTip.x, this->spearTip.y,
                       this->spearTip.z, lightTime, FHGFIRE_LIGHT_GREEN, 0, FHGFIRE_SPEAR_LIGHT);
    this->actor.child = &horseTemp->actor;
    this->work[GND_THROW_COUNT]++;
    Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_STICK);
}

void mode_fly_atack(BossGanondrof* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    PRINTF("this->fwork[GND_END_FRAME] = %d\n", (s16)this->fwork[GND_END_FRAME]);
    PRINTF("this->work[GND_SHOT_FRAME] = %d\n", this->work[GND_THROW_FRAME]);
    if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GND_END_FRAME])) {
        mode_flying_init(this, -6.0f);
    }

    if ((this->work[GND_ACTION_STATE] != THROW_NORMAL) && Skeleton_Info_frame_check(&this->skelAnime, 21.0f)) {
        this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonThrowEndAnim);
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPhantomGanonThrowEndAnim, 0.0f);
        this->work[GND_THROW_FRAME] = 10;
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, this->work[GND_THROW_FRAME])) {
        if (this->flyMode <= GND_FLY_NEUTRAL) {
            Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_MASIC2);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_MASIC1);
        }

        Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_VOICE);
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, this->work[GND_THROW_FRAME])) {
        EnfHG* horseTemp = (EnfHG*)this->actor.child;

        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, this->spearTip.x, this->spearTip.y,
                           this->spearTip.z, this->work[GND_ACTION_STATE], 0, 0, FHGFIRE_ENERGY_BALL);
        this->actor.child = &horseTemp->actor;
    }

    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x7D0);
    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.z += this->actor.velocity.z;
    add_calc0(&this->actor.velocity.x, 1.0f, 0.5f);
    add_calc0(&this->actor.velocity.z, 1.0f, 0.5f);
    this->actor.world.pos.y += 2.0f * sin_s(this->work[GND_VARIANCE_TIMER] * 1500);
}

void mode_ball_revise_init(BossGanondrof* this, PlayState* play) {
    static AnimationHeader* revise_anm_tbl[] = { &gPhantomGanonReturn1Anim, &gPhantomGanonReturn2Anim };
    s16 rand = fqrand() * 1.99f;

    this->fwork[GND_END_FRAME] = Si2_anime_end_frame(revise_anm_tbl[rand]);
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, revise_anm_tbl[rand], 0.0f);
    this->actionFunc = mode_ball_revise;
}

void mode_ball_revise(BossGanondrof* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 5.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_VOICE);
        PRINTF("VOISE               2  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        PRINTF("VOISE               2  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GND_END_FRAME])) {
        mode_flying_init(this, 0.0f);
    }

    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.z += this->actor.velocity.z;
    add_calc0(&this->actor.velocity.x, 1.0f, 0.5f);
    add_calc0(&this->actor.velocity.z, 1.0f, 0.5f);
    this->actor.world.pos.y += 2.0f * sin_s(this->work[GND_VARIANCE_TIMER] * 1500);
    if (this->returnSuccess) {
        this->returnSuccess = false;
        mode_ball_revise_init(this, play);
        this->timers[0] = 80;
    }
}

void mode_fly_damage_init(BossGanondrof* this, PlayState* play) {
    if (this->actionFunc != mode_fly_damage) {
        this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonAirDamageAnim);
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonAirDamageAnim, 0.0f);
        this->timers[0] = 50;
        this->shockTimer = 60;
    } else {
        this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonGroundDamageAnim);
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonGroundDamageAnim, 0.0f);
    }

    this->actionFunc = mode_fly_damage;
    this->work[GND_ACTION_STATE] = STUNNED_FALL;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.z = 0.0f;
}

void mode_fly_damage(BossGanondrof* this, PlayState* play) {
    PRINTF("DAMAGE   .................................\n");
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actor.gravity = -0.2f;
    if (this->actor.world.pos.y <= 5.0f) {
        if (this->work[GND_ACTION_STATE] == STUNNED_FALL) {
            this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonStunnedAnim);
            Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonStunnedAnim, -10.0f);
            this->work[GND_ACTION_STATE] = STUNNED_GROUND;
        }

        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
        if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GND_END_FRAME])) {
            Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_DAMAGE2);
        }

        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    }

    PRINTF("TIME0 %d ********************************************\n", this->timers[0]);
    if (this->timers[0] == 0) {
        mode_flying_init(this, -5.0f);
        this->timers[0] = 30;
        this->timers[2] = 30;
        this->flyMode = GND_FLY_NEUTRAL;
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
    }

    Actor_position_moveF(&this->actor);
}

void mode_intersept_init(BossGanondrof* this, PlayState* play) {
    this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonBlockAnim);
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonBlockAnim, -3.0f);
    this->actionFunc = mode_intersept;
    this->timers[0] = 10;
    Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_STICK);
}

void mode_intersept(BossGanondrof* this, PlayState* play) {
    this->colliderBody.base.colMaterial = COL_MATERIAL_METAL;
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.z += this->actor.velocity.z;
    add_calc0(&this->actor.velocity.x, 1.0f, 0.5f);
    add_calc0(&this->actor.velocity.z, 1.0f, 0.5f);
    this->actor.world.pos.y += 2.0f * sin_s(this->work[GND_VARIANCE_TIMER] * 1500);
    if (this->timers[0] == 0) {
        mode_flying_init(this, -5.0f);
        this->timers[0] = 10;
        this->flyMode = GND_FLY_NEUTRAL;
    }
}

void mode_fly_atack2_init(BossGanondrof* this, PlayState* play) {
    this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonChargeWindupAnim);
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonChargeWindupAnim, -3.0f);
    this->actionFunc = mode_fly_atack2;
    this->timers[0] = 20;
    this->work[GND_ACTION_STATE] = CHARGE_WINDUP;
}

void mode_fly_atack2(BossGanondrof* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* playerx = &player->actor;
    Actor* thisx = &this->actor;
    f32 dxCenter = thisx->world.pos.x - GND_BOSSROOM_CENTER_X;
    f32 dzCenter = thisx->world.pos.z - GND_BOSSROOM_CENTER_Z;

    this->colliderBody.base.colMaterial = COL_MATERIAL_METAL;
    Skeleton_Info2_anime_play(&this->skelAnime);
    switch (this->work[GND_ACTION_STATE]) {
        case CHARGE_WINDUP:
            if (this->timers[0] == 218) {
                Actor_SE_set(thisx, NA_SE_EN_FANTOM_STICK);
            }

            if (this->timers[0] == 19) {
                Actor_SE_set(thisx, NA_SE_EN_FANTOM_ATTACK);
            }

            thisx->world.pos.x += thisx->velocity.x;
            thisx->world.pos.z += thisx->velocity.z;
            add_calc0(&thisx->velocity.x, 1.0f, 0.5f);
            add_calc0(&thisx->velocity.z, 1.0f, 0.5f);
            if (this->timers[0] == 0) {
                this->work[GND_ACTION_STATE] = CHARGE_START;
                this->timers[0] = 10;
                thisx->speed = 0.0f;
                this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonChargeStartAnim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPhantomGanonChargeStartAnim, 0.0f);
            }

            adds(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0x7D0);
            break;
        case CHARGE_START:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GND_END_FRAME])) {
                this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonChargeAnim);
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonChargeAnim, 0.0f);
                this->work[GND_ACTION_STATE] = CHARGE_ATTACK;
            }
            FALLTHROUGH;
        case CHARGE_ATTACK:
            if (this->timers[0] != 0) {
                Vec3f vecToLink;

                adds(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0x7D0);
                vecToLink.x = playerx->world.pos.x - thisx->world.pos.x;
                vecToLink.y = playerx->world.pos.y + 40.0f - thisx->world.pos.y;
                vecToLink.z = playerx->world.pos.z - thisx->world.pos.z;
                thisx->world.rot.y = thisx->shape.rot.y;
                thisx->world.rot.x = RAD_TO_BINANG(fatan2(vecToLink.y, sqrtf(SQ(vecToLink.x) + SQ(vecToLink.z))));
            }

            Actor_position_speed_set_XY(thisx);
            Actor_position_move(thisx);
            add_calc2(&thisx->speed, 10.0f, 1.0f, 0.5f);
            if ((sqrtf(SQ(dxCenter) + SQ(dzCenter)) > 280.0f) || (thisx->xyzDistToPlayerSq < SQ(100.0f))) {
                this->work[GND_ACTION_STATE] = CHARGE_FINISH;
                this->timers[0] = 20;
            }
            break;
        case CHARGE_FINISH:
            thisx->gravity = 0.2f;
            Actor_position_moveF(thisx);
            PRINTF("YP %f @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n", thisx->world.pos.y);
            if (thisx->world.pos.y < 5.0f) {
                thisx->world.pos.y = 5.0f;
                thisx->velocity.y = 0.0f;
            }

            if (sqrtf(SQ(dxCenter) + SQ(dzCenter)) > 280.0f) {
                add_calc0(&thisx->speed, 1.0f, 2.0f);
                this->timers[0] = 0;
            }

            if (this->timers[0] == 0) {
                add_calc0(&thisx->speed, 1.0f, 2.0f);
                add_calc0(&thisx->velocity.y, 1.0f, 2.0f);
                adds(&thisx->shape.rot.y, thisx->yawTowardsPlayer, 5, 0x7D0);
                if ((thisx->speed <= 0.5f) && (fabsf(thisx->velocity.y) <= 0.1f)) {
                    mode_flying_init(this, -10.0f);
                    this->timers[0] = 30;
                    this->flyMode = GND_FLY_NEUTRAL;
                }
            }
            break;
    }

    if (thisx->world.pos.y > (GND_BOSSROOM_CENTER_Y + 83.0f)) {
        thisx->world.pos.y += 2.0f * sin_s(this->work[GND_VARIANCE_TIMER] * 1500);
    }
    {
        s16 i;
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };
        Vec3f baseOffset = { 0.0f, 50.0f, 0.0f };
        Vec3f offset;

        baseOffset.y = 10.0f;
        for (i = 0; i < 10; i++) {
            Matrix_push();
            Matrix_rotateY(BINANG_TO_RAD_ALT(thisx->shape.rot.y), MTXMODE_NEW);
            Matrix_rotateX(BINANG_TO_RAD_ALT(thisx->shape.rot.x), MTXMODE_APPLY);
            Matrix_rotateZ(BINANG_TO_RAD_ALT(this->work[GND_EFFECT_ANGLE]), MTXMODE_APPLY);
            Matrix_Position(&baseOffset, &offset);
            Matrix_pull();
            pos.x = this->spearTip.x + offset.x;
            pos.y = this->spearTip.y + offset.y;
            pos.z = this->spearTip.z + offset.z;
            vel.x = (offset.x * 500.0f) / 1000.0f;
            vel.y = (offset.y * 500.0f) / 1000.0f;
            vel.z = (offset.z * 500.0f) / 1000.0f;
            accel.x = (offset.x * -50.0f) / 1000.0f;
            accel.y = (offset.y * -50.0f) / 1000.0f;
            accel.z = (offset.z * -50.0f) / 1000.0f;
            Effect_fhg_flash_ct(play, &pos, &vel, &accel, 150, i % 7);
            this->work[GND_EFFECT_ANGLE] += 0x1A5C;
        }
    }

    if (!(this->work[GND_VARIANCE_TIMER] & 7)) {
        EnfHG* horse = (EnfHG*)thisx->child;

        Actor_info_make_child_actor(&play->actorCtx, thisx, play, ACTOR_EN_FHG_FIRE, this->spearTip.x, this->spearTip.y,
                           this->spearTip.z, 8, FHGFIRE_LIGHT_BLUE, 0, FHGFIRE_SPEAR_LIGHT);
        thisx->child = &horse->actor;
    }
}

static void mode_enddemo_init(BossGanondrof* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gPhantomGanonDeathBlowAnim);
    this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonDeathBlowAnim);
    this->actionFunc = mode_enddemo;
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
    Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_DEAD);
    this->deathState = DEATH_START;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->work[GND_VARIANCE_TIMER] = 0;
    this->shockTimer = 50;
}

static void mode_enddemo(BossGanondrof* this, PlayState* play) {
    u8 holdCamera = false;
    u8 bodyDecayLevel = 0;
    f32 camX;
    f32 camZ;
    f32 pad;
    Player* player = GET_PLAYER(play);
    Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

    PRINTF("PYP %f\n", player->actor.floorHeight);
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->work[GND_DEATH_SFX_TIMER]++;
    if (((60 < this->work[GND_DEATH_SFX_TIMER]) && (this->work[GND_DEATH_SFX_TIMER] < 500)) ||
        ((501 < this->work[GND_DEATH_SFX_TIMER]) && (this->work[GND_DEATH_SFX_TIMER] < 620))) {
        Actor_SE_set(&this->actor, NA_SE_EN_GOMA_LAST - SFX_FLAG);
    }

    switch (this->deathState) {
        case DEATH_START:
            Demo_play_start(play, &play->csCtx);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
            this->subCamId = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            PRINTF("7\n");
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            PRINTF("8\n");
            this->deathState = DEATH_THROES;
            player->actor.speed = 0.0f;
            this->timers[0] = 50;
            this->subCamEye = mainCam->eye;
            this->subCamAt = mainCam->at;
            this->subCamEyeNext.x = this->targetPos.x;
            this->subCamEyeNext.y = GND_BOSSROOM_CENTER_Y + 83.0f;
            this->subCamEyeNext.z = (this->targetPos.z + 100.0f) + 50;
            this->subCamAtNext.x = this->targetPos.x;
            this->subCamAtNext.y = this->targetPos.y - 10.0f;
            this->subCamAtNext.z = this->targetPos.z;
            this->subCamEyeVel.x = fabsf(mainCam->eye.x - this->subCamEyeNext.x);
            this->subCamEyeVel.y = fabsf(mainCam->eye.y - this->subCamEyeNext.y);
            this->subCamEyeVel.z = fabsf(mainCam->eye.z - this->subCamEyeNext.z);
            this->subCamAtVel.x = fabsf(mainCam->at.x - this->subCamAtNext.x);
            this->subCamAtVel.y = fabsf(mainCam->at.y - this->subCamAtNext.y);
            this->subCamAtVel.z = fabsf(mainCam->at.z - this->subCamAtNext.z);
            this->subCamAccel = 0.02f;
            this->subCamEyeMaxVelFrac.x = this->subCamEyeMaxVelFrac.y = this->subCamEyeMaxVelFrac.z = 0.05f;
            this->work[GND_ACTION_STATE] = DEATH_SPASM;
            this->timers[0] = 150;
            this->subCamAtMaxVelFrac.x = 0.2f;
            this->subCamAtMaxVelFrac.y = 0.2f;
            this->subCamAtMaxVelFrac.z = 0.2f;
            FALLTHROUGH;
        case DEATH_THROES:
            switch (this->work[GND_ACTION_STATE]) {
                case DEATH_SPASM:
                    if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GND_END_FRAME])) {
                        this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonAirDamageAnim);
                        Skeleton_Info2_init(&this->skelAnime, &gPhantomGanonAirDamageAnim, 0.5f, 0.0f,
                                         this->fwork[GND_END_FRAME], ANIMMODE_ONCE_INTERP, 0.0f);
                        this->work[GND_ACTION_STATE] = DEATH_LIMP;
                    }
                    break;
                case DEATH_LIMP:
                    if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GND_END_FRAME])) {
                        this->fwork[GND_END_FRAME] = Si2_anime_end_frame(&gPhantomGanonLimpAnim);
                        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonLimpAnim, -20.0f);
                        this->work[GND_ACTION_STATE] = DEATH_HUNCHED;
                    }
                    FALLTHROUGH;
                case DEATH_HUNCHED:
                    bodyDecayLevel = 1;
                    break;
            }
            adds(&this->actor.shape.rot.y, this->work[GND_VARIANCE_TIMER] * -100, 5, 0xBB8);
            add_calc2(&this->subCamEyeNext.z, this->targetPos.z + 60.0f, 0.02f, 0.5f);
            add_calc2(&this->actor.world.pos.y, GND_BOSSROOM_CENTER_Y + 133.0f, 0.05f, 100.0f);
            this->actor.world.pos.y += sin_s(this->work[GND_VARIANCE_TIMER] * 1500);
            this->subCamAtNext.x = this->targetPos.x;
            this->subCamAtNext.y = this->targetPos.y - 10.0f;
            this->subCamAtNext.z = this->targetPos.z;
            if (this->timers[0] == 0) {
                this->deathState = DEATH_WARP;
                this->timers[0] = 350;
                this->timers[1] = 50;
                this->fwork[GND_CAMERA_ZOOM] = 300.0f;
                this->subCamEyeNext.y = GND_BOSSROOM_CENTER_Y + 233.0f;
                player->actor.world.pos.x = GND_BOSSROOM_CENTER_X - 200.0f;
                player->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
                holdCamera = true;
                bodyDecayLevel = 1;
            }
            break;
        case DEATH_WARP:
            if (this->timers[1] == 1) {
                EnfHG* horseTemp = (EnfHG*)this->actor.child;

                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_FHG_FIRE, GND_BOSSROOM_CENTER_X,
                                   GND_BOSSROOM_CENTER_Y + 3.0f, GND_BOSSROOM_CENTER_Z, 0x4000, 0, 0,
                                   FHGFIRE_WARP_DEATH);
                this->actor.child = &horseTemp->actor;
                message_set(play, 0x108E, NULL);
            }

            this->actor.shape.rot.y -= 0xC8;
            this->actor.world.pos.y += sin_s(this->work[GND_VARIANCE_TIMER] * 1500);
            this->fwork[GND_CAMERA_ANGLE] += 0x78;
            camX = sin_s(this->fwork[GND_CAMERA_ANGLE]) * this->fwork[GND_CAMERA_ZOOM];
            camZ = cos_s(this->fwork[GND_CAMERA_ANGLE]) * this->fwork[GND_CAMERA_ZOOM];
            this->subCamEye.x = GND_BOSSROOM_CENTER_X + camX;
            this->subCamEye.y = this->subCamEyeNext.y;
            this->subCamEye.z = GND_BOSSROOM_CENTER_Z + camZ;
            this->subCamAt.x = GND_BOSSROOM_CENTER_X;
            this->subCamAt.y = GND_BOSSROOM_CENTER_Y + 23.0f;
            this->subCamAt.z = GND_BOSSROOM_CENTER_Z;
            add_calc2(&this->subCamEyeNext.y, GND_BOSSROOM_CENTER_Y + 33.0f, 0.05f, 0.5f);
            add_calc2(&this->fwork[GND_CAMERA_ZOOM], 170.0f, 0.05f, 1.0f);
            add_calc2(&this->actor.world.pos.x, GND_BOSSROOM_CENTER_X, 0.05f, 1.5f);
            add_calc2(&this->actor.world.pos.y, GND_BOSSROOM_CENTER_Y + 83.0f, 0.05f, 1.0f);
            add_calc2(&this->actor.world.pos.z, GND_BOSSROOM_CENTER_Z, 0.05f, 1.5f);
            if (this->timers[0] == 0) {
                this->deathState = DEATH_SCREAM;
                this->timers[0] = 50;
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPhantomGanonScreamAnim, -10.0f);
                this->actor.world.pos.x = GND_BOSSROOM_CENTER_X;
                this->actor.world.pos.y = GND_BOSSROOM_CENTER_Y + 83.0f;
                this->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
                this->actor.shape.rot.y = 0;
                this->work[GND_BODY_DECAY_INDEX] = 0;
                Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_LAST);
            }

            holdCamera = true;
            bodyDecayLevel = 1;
            break;
        case DEATH_SCREAM:
            holdCamera = true;
            bodyDecayLevel = 2;
            this->actor.world.pos.y = GND_BOSSROOM_CENTER_Y + 83.0f;
            this->subCamEye.x = GND_BOSSROOM_CENTER_X;
            this->subCamEye.y = GND_BOSSROOM_CENTER_Y + 83.0f;
            this->subCamEye.z = GND_BOSSROOM_CENTER_Z + 50.0f;
            this->subCamAt.x = GND_BOSSROOM_CENTER_X;
            this->subCamAt.y = GND_BOSSROOM_CENTER_Y + 103.0f;
            this->subCamAt.z = GND_BOSSROOM_CENTER_Z;
            if (this->timers[0] == 0) {
                this->deathState = DEATH_DISINTEGRATE;
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPhantomGanonLastPoseAnim, -10.0f);
                this->work[GND_BODY_DECAY_INDEX] = 0;
                this->timers[0] = 40;
            }
            break;
        case DEATH_DISINTEGRATE:
            holdCamera = true;
            bodyDecayLevel = 3;
            add_calc0(&this->subCamEye.y, 0.05f, 1.0f); // approaches GND_BOSSROOM_CENTER_Y + 33.0f
            add_calc2(&this->subCamEye.z, GND_BOSSROOM_CENTER_Z + 170.0f, 0.05f, 2.0f);
            add_calc2(&this->subCamAt.y, GND_BOSSROOM_CENTER_Y + 53.0f, 0.05f, 1.0f);
            if (this->timers[0] == 0) {
                this->timers[0] = 250;
                this->deathState = DEATH_FINISH;
            }
            break;
        case DEATH_FINISH:
            holdCamera = true;
            bodyDecayLevel = 10;
            if (this->timers[0] == 150) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS_CLEAR);
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_DOOR_WARP1, GND_BOSSROOM_CENTER_X, GND_BOSSROOM_CENTER_Y,
                            GND_BOSSROOM_CENTER_Z, 0, 0, 0, WARP_DUNGEON_ADULT);
            }

            add_calc0(&this->subCamEye.y, 0.05f, 1.0f); // GND_BOSSROOM_CENTER_Y + 33.0f
            add_calc2(&this->subCamEye.z, GND_BOSSROOM_CENTER_Z + 170.0f, 0.05f, 2.0f);
            add_calc2(&this->subCamAt.y, GND_BOSSROOM_CENTER_Y + 53.0f, 0.05f, 1.0f);
            if (this->timers[0] == 0) {
                EnfHG* horse = (EnfHG*)this->actor.child;

                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, GND_BOSSROOM_CENTER_X, GND_BOSSROOM_CENTER_Y,
                            GND_BOSSROOM_CENTER_Z + 200.0f, 0, 0, 0, 0);
                this->actor.child = &horse->actor;
                this->killActor = true;
                horse->killActor = true;
                Actor_Environment_room_clear_On(play, play->roomCtx.curRoom.num);
                Actor_Environment_sw_On(play, 0x22);
            }
            break;
    }

    if (bodyDecayLevel) {
        Vec3f pos;
        Vec3f vel = { 0.0f, 0.0f, 0.0f };
        Vec3f accelKFire = { 0.0f, 0.0f, 0.0f };
        Vec3f accelHahen = { 0.0f, -0.5f, 0.0f };
        s16 limbDecayIndex;
        s16 i;

        vel.x = this->actor.world.pos.x - this->actor.prevPos.x;
        vel.z = this->actor.world.pos.z - this->actor.prevPos.z;
        if (bodyDecayLevel < 10) {
            if (this->work[GND_DEATH_ENV_TIMER] == 0) {
                if (play->envCtx.lightSettingOverride == 0) {
                    play->envCtx.lightSettingOverride = 3;
                    this->work[GND_DEATH_ENV_TIMER] = (s16)rnd_f(5.0f) + 4.0f;
                    play->envCtx.lightBlendRateOverride = 40;
                } else {
                    play->envCtx.lightSettingOverride = 0;
                    this->work[GND_DEATH_ENV_TIMER] = (s16)rnd_f(2.0f) + 2.0f;
                    play->envCtx.lightBlendRateOverride = 20;
                }
            } else {
                this->work[GND_DEATH_ENV_TIMER]--;
            }

            for (i = 0; i <= 0; i++) {
                limbDecayIndex = this->work[GND_LIMB_DECAY_INDEX];
                this->work[GND_LIMB_DECAY_INDEX]++;
                this->work[GND_LIMB_DECAY_INDEX] %= 25;
                pos.x = this->bodyPartsPos[limbDecayIndex].x + rnd_fx(5.0f);
                pos.y = this->bodyPartsPos[limbDecayIndex].y + rnd_fx(5.0f);
                pos.z = this->bodyPartsPos[limbDecayIndex].z + rnd_fx(5.0f);
                accelKFire.y = 0.0f;

                if (bodyDecayLevel == 3) {
                    accelKFire.y = -0.2f;
                    accelKFire.x = (GND_BOSSROOM_CENTER_X - pos.x) * 0.002f;
                    accelKFire.z = (GND_BOSSROOM_CENTER_Z - pos.z) * 0.002f;
                    accelHahen.x = (GND_BOSSROOM_CENTER_X - pos.x) * 0.001f;
                    accelHahen.y = -1.0f;
                    accelHahen.z = (GND_BOSSROOM_CENTER_Z - pos.z) * 0.001f;
                }

                Effect_k_fire_ct(play, &pos, &vel, &accelKFire, (s16)rnd_f(20.0f) + 15, bodyDecayLevel);
                if ((fqrand() < 0.5f) || (bodyDecayLevel == 3)) {
                    Effect_Hahen_ct3(play, &pos, &vel, &accelHahen, 0, (s16)rnd_f(4.0f) + 7,
                                        HAHEN_OBJECT_DEFAULT, 10, NULL);
                }
            }
        } else {
            play->envCtx.lightSettingOverride = 0;
            play->envCtx.lightBlendRateOverride = 20;
        }

        this->work[GND_BODY_DECAY_FLAG] = true;
        for (i = 0; i < 5; i++) {
            if (bodyDecayLevel == 1) {
                tex_mesh_GND(mesh_data_GND_1, this->work[GND_BODY_DECAY_INDEX]);
            } else if (bodyDecayLevel == 2) {
                tex_mesh_GND(mesh_data_GND_2, this->work[GND_BODY_DECAY_INDEX]);
            } else {
                tex_mesh_GND(mesh_data_GND_CL, this->work[GND_BODY_DECAY_INDEX]);
            }

            if (this->work[GND_BODY_DECAY_INDEX] < 0x100) {
                this->work[GND_BODY_DECAY_INDEX]++;
            }
        }
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        if (!holdCamera) {
            add_calc2(&this->subCamEye.x, this->subCamEyeNext.x, this->subCamEyeMaxVelFrac.x,
                           this->subCamEyeVel.x * this->subCamVelFactor);
            add_calc2(&this->subCamEye.y, this->subCamEyeNext.y, this->subCamEyeMaxVelFrac.y,
                           this->subCamEyeVel.y * this->subCamVelFactor);
            add_calc2(&this->subCamEye.z, this->subCamEyeNext.z, this->subCamEyeMaxVelFrac.z,
                           this->subCamEyeVel.z * this->subCamVelFactor);
            add_calc2(&this->subCamAt.x, this->subCamAtNext.x, this->subCamAtMaxVelFrac.x,
                           this->subCamAtVel.x * this->subCamVelFactor);
            add_calc2(&this->subCamAt.y, this->subCamAtNext.y, this->subCamAtMaxVelFrac.y,
                           this->subCamAtVel.y * this->subCamVelFactor);
            add_calc2(&this->subCamAt.z, this->subCamAtNext.z, this->subCamAtMaxVelFrac.z,
                           this->subCamAtVel.z * this->subCamVelFactor);
            add_calc2(&this->subCamVelFactor, 1.0f, 1.0f, this->subCamAccel);
        }

        Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

void Gannondrof_Damage_check(BossGanondrof* this, PlayState* play) {
    s32 acHit;
    EnfHG* horse = (EnfHG*)this->actor.child;
    ColliderElement* acHitElem;

    if (this->work[GND_INVINC_TIMER] != 0) {
        this->work[GND_INVINC_TIMER]--;
        this->returnCount = 0;
        this->colliderBody.base.acFlags &= ~AC_HIT;
    } else {
        acHit = this->colliderBody.base.acFlags & AC_HIT;
        if ((acHit && ((s8)this->actor.colChkInfo.health > 0)) || (this->returnCount != 0)) {
            if (acHit) {
                this->colliderBody.base.acFlags &= ~AC_HIT;
                acHitElem = this->colliderBody.elem.acHitElem;
            }
            if (this->flyMode != GND_FLY_PAINTING) {
                if (acHit && (this->actionFunc != mode_fly_damage) &&
                    (acHitElem->atDmgInfo.dmgFlags & DMG_RANGED)) {
                    Actor_SE_set(&this->actor, NA_SE_NONE);
                    PRINTF("hit != 0 \n");
                } else if (this->actionFunc != mode_fly_atack2) {
                    if (this->returnCount == 0) {
                        u8 dmg;
                        u8 canKill = false;
                        s32 dmgFlags = acHitElem->atDmgInfo.dmgFlags;

                        if (dmgFlags & DMG_HOOKSHOT) {
                            return;
                        }
                        dmg = GetSwordAP(dmgFlags);
                        (dmg == 0) ? (dmg = 2) : (canKill = true);
                        if (((s8)this->actor.colChkInfo.health > 2) || canKill) {
                            this->actor.colChkInfo.health -= dmg;
                        }

                        if ((s8)this->actor.colChkInfo.health <= 0) {
                            mode_enddemo_init(this, play);
                            Actor_info_finish(play, &this->actor);
                            return;
                        }
                    }
                    mode_fly_damage_init(this, play);
                    if (this->returnCount >= 2) {
                        this->timers[0] = 120;
                    }
                    this->work[GND_INVINC_TIMER] = 10;
                    horse->hitTimer = 20;
                    Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_DAMAGE);
                } else {
                    Actor_SE_set(&this->actor, NA_SE_NONE);
                }
            } else if (acHit && (acHitElem->atDmgInfo.dmgFlags & DMG_RANGED)) {
                this->work[GND_INVINC_TIMER] = 10;
                this->actor.colChkInfo.health -= 2;
                horse->hitTimer = 20;
                Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_DAMAGE);
            }
            this->returnCount = 0;
        }
    }
}

void Boss_Ganondrof_actor_move(Actor* thisx, PlayState* play) {
    f32 cs;
    f32 sn;
    f32 legRotTargetY;
    f32 legRotTargetZ;
    f32 legSplitTarget;
    s32 pad2;
    s16 i;
    s32 pad;
    EnfHG* horse;
    BossGanondrof* this = (BossGanondrof*)thisx;

    PRINTF("MOVE START %d\n", this->actor.params);
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    this->colliderBody.base.colMaterial = COL_MATERIAL_HIT3;
    if (this->killActor) {
        Actor_delete(&this->actor);
        return;
    }
    horse = (EnfHG*)this->actor.child;
    this->work[GND_VARIANCE_TIMER]++;
    PRINTF("MOVE START EEEEEEEEEEEEEEEEEEEEEE%d\n", this->actor.params);

    this->actionFunc(this, play);

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        if (this->timers[i]) {
            this->timers[i]--;
        }
    }
    if (this->work[GND_UNKTIMER_1]) {
        this->work[GND_UNKTIMER_1]--;
    }
    if (this->work[GND_UNKTIMER_2]) {
        this->work[GND_UNKTIMER_2]--;
    }

    if (this->actionFunc != mode_enddemo) {
        Gannondrof_Damage_check(this, play);
    }

    PRINTF("MOVE END\n");
    CollisionCheck_Uty_PosSetPipeC(&this->targetPos, &this->colliderBody);
    CollisionCheck_Uty_PosSetPipeC(&this->spearTip, &this->colliderSpear);
    if ((this->flyMode == GND_FLY_PAINTING) && !horse->bossGndInPainting) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderBody.base);
    }
    if ((this->actionFunc == mode_fly_damage) && (this->timers[0] > 1)) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderBody.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderBody.base);
    } else if (this->actionFunc == mode_intersept) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderBody.base);
    } else if (this->actionFunc == mode_fly_atack2) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderBody.base);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderBody.base);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderSpear.base);
    }

    this->actor.focus.pos = this->targetPos;

    sn = sin_s(-this->actor.shape.rot.y);
    cs = cos_s(-this->actor.shape.rot.y);
    legRotTargetY = ((sn * this->actor.velocity.z) + (cs * this->actor.velocity.x)) * 300.0f;
    legRotTargetZ = ((-sn * this->actor.velocity.x) + (cs * this->actor.velocity.z)) * 300.0f;
    add_calc2(&this->legRotY, legRotTargetY, 1.0f, 600.0f);
    add_calc2(&this->legRotZ, legRotTargetZ, 1.0f, 600.0f);
    if ((this->flyMode != GND_FLY_PAINTING) && (this->actionFunc != mode_fly_damage) &&
        (this->deathState == NOT_DEAD)) {
        legSplitTarget = (sin_s(this->work[GND_VARIANCE_TIMER] * 0x8DC) * -500.0f) - 500.0f;
    } else {
        legSplitTarget = 0.0f;
    }

    add_calc2(&this->legSplitY, legSplitTarget, 1.0f, 100.0f);
    if (this->shockTimer != 0) {
        s16 j;

        this->shockTimer--;
        PRINTF("F 1\n");
        for (j = 0; j < 7; j++) {
            PRINTF("F 15\n");
            Effect_fhg_flash_mini_ct(play, &this->actor, &this->actor.world.pos, 45, FHGFLASH_SHOCK_PG);
        }
        PRINTF("F 2\n");
    }

    if (this->actor.params == GND_REAL_BOSS) {
        Light_point_ct(&this->lightInfo, this->spearTip.x, this->spearTip.y, this->spearTip.z, 255, 255, 255,
                                  200);
    }
}

s32 Boss_Ganondrof_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossGanondrof* this = (BossGanondrof*)thisx;

    switch (limbIndex) {
        case 15:
            if ((this->actionFunc == mode_startdemo) && this->work[GND_MASK_OFF]) {
                *dList = gPhantomGanonFaceDL;
            }
            rot->y += this->rideRotY[limbIndex];
            rot->z += this->rideRotZ[limbIndex];
            break;

        case 19:
            rot->y += this->legRotY + this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 20:
            rot->y += this->legRotY + this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 21:
            rot->y += this->legRotY + this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 23:
            rot->y += this->legRotY - this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 24:
            rot->y += this->legRotY - this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 25:
            rot->y += this->legRotY - this->legSplitY;
            rot->z += this->legRotZ;
            break;

        case 5:
        case 6:
        case 7:
            rot->y += this->armRotY;
            rot->z += this->armRotZ;
            break;

        case 8:
        case 9:
        case 10:
            rot->y += this->armRotY;
            rot->z += this->armRotZ;
            break;

        case 13:
            if (this->deathState != NOT_DEAD) {
                *dList = NULL;
            }
            FALLTHROUGH;
        default:
            rot->y += this->rideRotY[limbIndex];
            rot->z += this->rideRotZ[limbIndex];
            break;
    }

    return 0;
}

void Boss_Ganondrof_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f center_p = { 0.0f, 0.0f, 0.0f };
    static Vec3f yari_p = { 0.0f, 0.0f, 6000.0f };

    BossGanondrof* this = (BossGanondrof*)thisx;

    if (limbIndex == 14) {
        Matrix_Position(&center_p, &this->targetPos);
    } else if (limbIndex == 13) {
        Matrix_Position(&yari_p, &this->spearTip);
    }

    if (((this->flyMode != GND_FLY_PAINTING) || (this->actionFunc == mode_startdemo)) && (limbIndex <= 25)) {
        Matrix_Position(&center_p, &this->bodyPartsPos[limbIndex - 1]);
    }
}

Gfx* gnf_mode_3(GraphicsContext* gfxCtx) {
    Gfx* dList = GRAPH_ALLOC(gfxCtx, sizeof(Gfx) * 4);
    Gfx* dListHead = dList;

    gDPPipeSync(dListHead++);
    gDPSetRenderMode(dListHead++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_TEX_EDGE2);
    gSPClearGeometryMode(dListHead++, G_CULL_BACK);
    gSPEndDisplayList(dListHead++);

    return dList;
}

Gfx* gnf_mode_1(GraphicsContext* gfxCtx) {
    Gfx* dList = GRAPH_ALLOC(gfxCtx, sizeof(Gfx) * 1);
    Gfx* dListHead = dList;

    gSPEndDisplayList(dListHead++);
    return dList;
}

void Boss_Ganondrof_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BossGanondrof* this = (BossGanondrof*)thisx;
    EnfHG* horse;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganondrof.c", 3716);
    PRINTF("MOVE P = %x\n", this->actor.update);
    PRINTF("STOP TIMER = %d ==============\n", this->actor.freezeTimer);
    horse = (EnfHG*)this->actor.child;
    if (this->flyMode == GND_FLY_PAINTING) {
        Matrix_rotateY((horse->turnRot * 3.1416f) / (f32)0x8000, MTXMODE_APPLY);
    }

    PRINTF("YP %f\n", this->actor.world.pos.y);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    if (this->work[GND_INVINC_TIMER] & 4) {
        POLY_OPA_DISP = gfx_set_fog_nosync(POLY_OPA_DISP, 255, 50, 0, 0, 900, 1099);
    } else {
        POLY_OPA_DISP = gfx_set_fog_nosync(POLY_OPA_DISP, (u32)horse->warpColorFilterR, (u32)horse->warpColorFilterG,
                                   (u32)horse->warpColorFilterB, 0, (s32)horse->warpColorFilterUnk1 + 995,
                                   (s32)horse->warpColorFilterUnk2 + 1000);
    }

    PRINTF("DRAW 11\n");
    PRINTF("EYE_COL %d\n", (s16)this->fwork[GND_EYE_BRIGHTNESS]);
    gDPSetEnvColor(POLY_OPA_DISP++, (s16)this->fwork[GND_EYE_BRIGHTNESS], (s16)this->fwork[GND_EYE_BRIGHTNESS],
                   (s16)this->fwork[GND_EYE_BRIGHTNESS], (s16)this->fwork[GND_EYE_ALPHA]);
    if (this->work[GND_BODY_DECAY_FLAG]) {
        gSPSegment(POLY_OPA_DISP++, 0x08, gnf_mode_3(play->state.gfxCtx));
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, gnf_mode_1(play->state.gfxCtx));
    }

    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, Boss_Ganondrof_draw_sub,
                      Boss_Ganondrof_draw_sub2, this);
    PRINTF("DRAW 22\n");
    POLY_OPA_DISP = game_play_set_fog(play, POLY_OPA_DISP);
    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganondrof.c", 3814);
    PRINTF("DRAW END %d\n", this->actor.params);
}
