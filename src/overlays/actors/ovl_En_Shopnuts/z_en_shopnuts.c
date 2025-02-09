/*
 * File: z_en_shopnuts.c
 * Overlay: En_Shopnuts
 * Description: Deku Salesman - Attack Phase
 */

#include "z_en_shopnuts.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void En_Shopnuts_actor_ct(Actor* thisx, PlayState* play);
void En_Shopnuts_actor_dt(Actor* thisx, PlayState* play);
void En_Shopnuts_actor_move(Actor* thisx, PlayState* play);
void En_Shopnuts_actor_draw(Actor* thisx, PlayState* play);

static void mode_head_up_init(EnShopnuts* this);
static void mode_head_up(EnShopnuts* this, PlayState* play);
static void mode_look_around(EnShopnuts* this, PlayState* play);
static void mode_attack_wait(EnShopnuts* this, PlayState* play);
static void mode_attack(EnShopnuts* this, PlayState* play);
static void mode_head_down(EnShopnuts* this, PlayState* play);
static void mode_jump(EnShopnuts* this, PlayState* play);

ActorProfile En_Shopnuts_Profile = {
    /**/ ACTOR_EN_SHOPNUTS,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_SHOPNUTS,
    /**/ sizeof(EnShopnuts),
    /**/ En_Shopnuts_actor_ct,
    /**/ En_Shopnuts_actor_dt,
    /**/ En_Shopnuts_actor_move,
    /**/ En_Shopnuts_actor_draw,
};

static ColliderCylinderInit ShopnutsAcOcPipeData = {
    {
        COL_MATERIAL_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit ShopnutsStatusData = { 1, 20, 40, MASS_HEAVY };

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_BUSINESS_SCRUB, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2600, ICHAIN_STOP),
};

void En_Shopnuts_actor_ct(Actor* thisx, PlayState* play) {
    EnShopnuts* this = (EnShopnuts*)thisx;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 35.0f);

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBusinessScrubSkel, &gBusinessScrubPeekAnim, this->jointTable,
                       this->morphTable, BUSINESS_SCRUB_LIMB_MAX);

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &ShopnutsAcOcPipeData);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &ShopnutsStatusData);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

    if (((SHOPNUTS_GET_TYPE(&this->actor) == DNS_TYPE_HEART_PIECE) && GET_ITEMGETINF(ITEMGETINF_DEKU_HEART_PIECE)) ||
        ((SHOPNUTS_GET_TYPE(&this->actor) == DNS_TYPE_DEKU_STICK_UPGRADE) &&
         GET_INFTABLE(INFTABLE_HAS_DEKU_STICK_UPGRADE)) ||
        ((SHOPNUTS_GET_TYPE(&this->actor) == DNS_TYPE_DEKU_NUT_UPGRADE) &&
         GET_INFTABLE(INFTABLE_HAS_DEKU_NUT_UPGRADE))) {
        Actor_delete(&this->actor);
    } else {
        mode_head_up_init(this);
    }
}

void En_Shopnuts_actor_dt(Actor* thisx, PlayState* play) {
    EnShopnuts* this = (EnShopnuts*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void mode_head_up_init(EnShopnuts* this) {
    Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gBusinessScrubInitialAnim, 0.0f);
    this->animFlagAndTimer = get_random_timer(100, 50);
    this->collider.dim.height = 5;
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_head_up;
}

static void mode_look_around_init(EnShopnuts* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gBusinessScrubLookAroundAnim);
    this->animFlagAndTimer = 2;
    this->actionFunc = mode_look_around;
}

static void mode_attack_init(EnShopnuts* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gBusinessScrubThrowNutAnim);
    this->actionFunc = mode_attack;
}

static void mode_attack_wait_init(EnShopnuts* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gBusinessScrubPeekAnim, -3.0f);

    if (this->actionFunc == mode_attack) {
        this->animFlagAndTimer = 2 | 0x1000; // sets timer and flag
    } else {
        this->animFlagAndTimer = 1;
    }

    this->actionFunc = mode_attack_wait;
}

static void mode_head_down_init(EnShopnuts* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gBusinessScrubPeekBurrowAnim, -5.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DOWN);
    this->actionFunc = mode_head_down;
}

static void mode_jump_init(EnShopnuts* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gBusinessScrubRotateAnim, -3.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DAMAGE);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_jump;
}

static void mode_head_up(EnShopnuts* this, PlayState* play) {
    s32 hasSlowPlaybackSpeed = false;

    if (this->skelAnime.playSpeed < 0.5f) {
        hasSlowPlaybackSpeed = true;
    }

    if (hasSlowPlaybackSpeed && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, 9.0f)) {
        this->collider.base.acFlags |= AC_ON;
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 8.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_UP);
    }

    this->collider.dim.height = ((CLAMP(this->skelAnime.curFrame, 9.0f, 13.0f) - 9.0f) * 9.0f) + 5.0f;
    if (!hasSlowPlaybackSpeed && (this->actor.xzDistToPlayer < 120.0f)) {
        mode_head_down_init(this);
    } else if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.xzDistToPlayer < 120.0f) {
            mode_head_down_init(this);
        } else if ((this->animFlagAndTimer == 0) && (this->actor.xzDistToPlayer > 320.0f)) {
            mode_look_around_init(this);
        } else {
            mode_attack_wait_init(this);
        }
    }
    if (hasSlowPlaybackSpeed &&
        ((this->actor.xzDistToPlayer > 160.0f) && (fabsf(this->actor.yDistToPlayer) < 120.0f)) &&
        ((this->animFlagAndTimer == 0) || (this->actor.xzDistToPlayer < 480.0f))) {
        this->skelAnime.playSpeed = 1.0f;
    }
}

static void mode_look_around(EnShopnuts* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }

    if ((this->actor.xzDistToPlayer < 120.0f) || (this->animFlagAndTimer == 0)) {
        mode_head_down_init(this);
    }
}

static void mode_attack_wait(EnShopnuts* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }

    if (!(this->animFlagAndTimer & 0x1000)) {
        adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    }

    if ((this->actor.xzDistToPlayer < 120.0f) || (this->animFlagAndTimer == 0x1000)) {
        mode_head_down_init(this);
    } else if (this->animFlagAndTimer == 0) {
        mode_attack_init(this);
    }
}

static void mode_attack(EnShopnuts* this, PlayState* play) {
    Vec3f spawnPos;

    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);

    if (this->actor.xzDistToPlayer < 120.0f) {
        mode_head_down_init(this);
    } else if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_attack_wait_init(this);
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
        spawnPos.x = this->actor.world.pos.x + (sin_s(this->actor.shape.rot.y) * 23.0f);
        spawnPos.y = this->actor.world.pos.y + 12.0f;
        spawnPos.z = this->actor.world.pos.z + (cos_s(this->actor.shape.rot.y) * 23.0f);

        if (Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_NUTSBALL, spawnPos.x, spawnPos.y, spawnPos.z,
                        this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z,
                        EN_NUTSBALL_TYPE_SHOPNUTS) != NULL) {
            Actor_SE_set(&this->actor, NA_SE_EN_NUTS_THROW);
        }
    }
}

static void mode_head_down(EnShopnuts* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_head_up_init(this);
    } else {
        this->collider.dim.height = ((4.0f - CLAMP_MAX(this->skelAnime.curFrame, 4.0f)) * 10.0f) + 5.0f;
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, 4.0f)) {
        this->collider.base.acFlags &= ~AC_ON;
    }
}

static void mode_jump(EnShopnuts* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_DNS, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z,
                    SHOPNUTS_GET_TYPE(&this->actor));
        Actor_delete(&this->actor);
    } else {
        adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    }
}

void En_Shopnuts_damage_proc(EnShopnuts* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elem, true);
        mode_jump_init(this);
    } else if (play->actorCtx.unk_02 != 0) {
        mode_jump_init(this);
    }
}

void En_Shopnuts_actor_move(Actor* thisx, PlayState* play) {
    EnShopnuts* this = (EnShopnuts*)thisx;

    En_Shopnuts_damage_proc(this, play);

    this->actionFunc(this, play);

    Actor_BGcheck2(play, &this->actor, 20.0f, this->collider.dim.radius, this->collider.dim.height,
                            UPDBGCHECKINFO_FLAG_2);

    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }

    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

    if (this->actionFunc == mode_head_up) {
        Actor_world_to_eye(&this->actor, this->skelAnime.curFrame);
    } else if (this->actionFunc == mode_head_down) {
        Actor_world_to_eye(&this->actor, 20.0f - ((this->skelAnime.curFrame * 20.0f) /
                                              Si2_anime_end_frame(&gBusinessScrubPeekBurrowAnim)));
    } else {
        Actor_world_to_eye(&this->actor, 20.0f);
    }
}

s32 en_shopnuts_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnShopnuts* this = (EnShopnuts*)thisx;

    if ((limbIndex == BUSINESS_SCRUB_LIMB_NOSE) && (this->actionFunc == mode_attack)) {
        *dList = NULL;
    }

    return 0;
}

void en_shopnuts_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnShopnuts* this = (EnShopnuts*)thisx;
    f32 curFrame;
    f32 x;
    f32 y;
    f32 z;

    if ((limbIndex != BUSINESS_SCRUB_LIMB_NOSE) || (this->actionFunc != mode_attack)) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx, "../z_en_shopnuts.c", 682);

    curFrame = this->skelAnime.curFrame;
    if (curFrame <= 6.0f) {
        y = 1.0f - (curFrame * 0.0833f);
        x = z = (curFrame * 0.1167f) + 1.0f;
    } else if (curFrame <= 7.0f) {
        curFrame -= 6.0f;
        y = 0.5f + curFrame;
        x = z = 1.7f - (curFrame * 0.7f);
    } else if (curFrame <= 10.0f) {
        y = 1.5f - ((curFrame - 7.0f) * 0.1667f);
        x = z = 1.0f;
    } else {
        x = y = z = 1.0f;
    }

    Matrix_scale(x, y, z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_shopnuts.c", 714);
    gSPDisplayList(POLY_OPA_DISP++, gBusinessScrubNoseDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_shopnuts.c", 717);
}

void En_Shopnuts_actor_draw(Actor* thisx, PlayState* play) {
    EnShopnuts* this = (EnShopnuts*)thisx;

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          en_shopnuts_display1, en_shopnuts_display2, this);
}
