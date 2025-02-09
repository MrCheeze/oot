/*
 * File: z_en_hata.c
 * Overlay: ovl_En_Hata
 * Description: Wooden post with red cloth
 */

#include "z_en_hata.h"
#include "assets/objects/object_hata/object_hata.h"

#define FLAGS 0

void En_Hata_Actor_ct(Actor* thisx, PlayState* play);
void En_Hata_Actor_dt(Actor* thisx, PlayState* play);
void En_Hata_Actor_move(Actor* thisx, PlayState* play2);
void En_Hata_Actor_draw(Actor* thisx, PlayState* play);

ActorProfile En_Hata_Profile = {
    /**/ ACTOR_EN_HATA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_HATA,
    /**/ sizeof(EnHata),
    /**/ En_Hata_Actor_ct,
    /**/ En_Hata_Actor_dt,
    /**/ En_Hata_Actor_move,
    /**/ En_Hata_Actor_draw,
};

// Unused Collider and CollisionCheck data
static ColliderCylinderInit HataPipeInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000080, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 16, 246, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 HataStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

void En_Hata_Actor_ct(Actor* thisx, PlayState* play) {
    EnHata* this = (EnHata*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;
    f32 frameCount = Si2_anime_end_frame(&gFlagpoleFlapAnim);

    Actor_set_scale(&this->dyna.actor, 1.0f / 75.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gFlagpoleSkel, &gFlagpoleFlapAnim, NULL, NULL, 0);
    Skeleton_Info2_init(&this->skelAnime, &gFlagpoleFlapAnim, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gFlagpoleCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->dyna.actor.cullingVolumeScale = 500.0f;
    this->dyna.actor.cullingVolumeDownward = 550.0f;
    this->dyna.actor.cullingVolumeDistance = 2200.0f;
    this->invScale = 6;
    this->maxStep = 1000;
    this->minStep = 1;
    this->unk_278 = fqrand() * 0xFFFF;
}

void En_Hata_Actor_dt(Actor* thisx, PlayState* play) {
    EnHata* this = (EnHata*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void En_Hata_Actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnHata* this = (EnHata*)thisx;
    s32 pitch;
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f windVec;
    f32 sin;

    Skeleton_Info2_anime_play(&this->skelAnime);
    // Rotate to hang down by default
    this->limbs[FLAGPOLE_LIMB_FLAG_1_BASE].y = this->limbs[FLAGPOLE_LIMB_FLAG_2_BASE].y = -0x4000;
    windVec.x = play->envCtx.windDirection.x;
    windVec.y = play->envCtx.windDirection.y;
    windVec.z = play->envCtx.windDirection.z;

    if (play->envCtx.windSpeed > 255.0f) {
        play->envCtx.windSpeed = 255.0f;
    }

    if (play->envCtx.windSpeed < 0.0f) {
        play->envCtx.windSpeed = 0.0f;
    }

    if (fqrand() > 0.5f) {
        this->unk_278 += 6000;
    } else {
        this->unk_278 += 3000;
    }

    // Mimic varying wind gusts
    sin = sin_s(this->unk_278) * 80.0f;
    pitch = -search_position_angleX(&zeroVec, &windVec);
    pitch = ((s32)((15000 - pitch) * (1.0f - (play->envCtx.windSpeed / (255.0f - sin))))) + pitch;
    add_calc_short_angle2(&this->limbs[FLAGPOLE_LIMB_FLAG_1_HOIST_END_BASE].y, pitch, this->invScale, this->maxStep,
                       this->minStep);
    this->limbs[FLAGPOLE_LIMB_FLAG_2_HOIST_END_BASE].y = this->limbs[FLAGPOLE_LIMB_FLAG_1_HOIST_END_BASE].y;
    this->limbs[FLAGPOLE_LIMB_FLAG_1_HOIST_END_BASE].z = -search_position_angleY(&zeroVec, &windVec);
    this->limbs[FLAGPOLE_LIMB_FLAG_2_HOIST_END_BASE].z = this->limbs[FLAGPOLE_LIMB_FLAG_1_HOIST_END_BASE].z;
    this->skelAnime.playSpeed = (rnd_f(1.25f) + 2.75f) * (play->envCtx.windSpeed / 255.0f);
}

s32 En_Hata_before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHata* this = (EnHata*)thisx;
    Vec3s* limbs;

    if (limbIndex == FLAGPOLE_LIMB_FLAG_2_BASE || limbIndex == FLAGPOLE_LIMB_FLAG_1_BASE ||
        limbIndex == FLAGPOLE_LIMB_FLAG_2_HOIST_END_BASE || limbIndex == FLAGPOLE_LIMB_FLAG_1_HOIST_END_BASE) {
        limbs = this->limbs;
        rot->x += limbs[limbIndex].x;
        rot->y += limbs[limbIndex].y;
        rot->z += limbs[limbIndex].z;
    }
    return false;
}

void En_Hata_after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

void En_Hata_Actor_draw(Actor* thisx, PlayState* play) {
    EnHata* this = (EnHata*)thisx;

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    Matrix_scale(1.0f, 1.1f, 1.0f, MTXMODE_APPLY);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, En_Hata_before_display,
                      En_Hata_after_display, this);
}
