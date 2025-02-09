/*
 * File: z_en_butte.c
 * Overlay: ovl_En_Butte
 * Description: Butterfly
 */

#include "z_en_butte.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/gameplay_field_keep/gameplay_field_keep.h"

#define FLAGS 0

void En_Choo_actor_ct(Actor* thisx, PlayState* play);
void En_Choo_actor_dt(Actor* thisx, PlayState* play2);
void En_Choo_actor_move(Actor* thisx, PlayState* play);
void En_Choo_actor_draw(Actor* thisx, PlayState* play);

void mv_normal_init(EnButte* this);
void mv_normal(EnButte* this, PlayState* play);
void mv_okkake_init(EnButte* this);
void mv_okkake(EnButte* this, PlayState* play);
void mv_elf_init(EnButte* this);
void mv_elf(EnButte* this, PlayState* play);
static void mv_end_init(EnButte* this);
static void mv_end(EnButte* this, PlayState* play);

static ColliderJntSphElementInit CrossSphElemDt_choo[] = {
    { {
          ELEM_MATERIAL_UNK0,
          { 0x00000000, 0x00, 0x00 },
          { 0xFFCFFFFF, 0x000, 0x00 },
          ATELEM_NONE,
          ACELEM_NONE,
          OCELEM_ON,
      },
      { 0, { { 0, 0, 0 }, 5 }, 100 } },
};
static ColliderJntSphInit CrossSphDt_choo = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_1,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    CrossSphElemDt_choo,
};

ActorProfile En_Butte_Profile = {
    /**/ ACTOR_EN_BUTTE,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(EnButte),
    /**/ En_Choo_actor_ct,
    /**/ En_Choo_actor_dt,
    /**/ En_Choo_actor_move,
    /**/ En_Choo_actor_draw,
};

typedef struct EnButteFlightParams {
    /* 0x00 */ s16 minTime;
    /* 0x02 */ s16 maxTime;
    /* 0x04 */ f32 speedXZTarget;
    /* 0x08 */ f32 speedXZScale;
    /* 0x0C */ f32 speedXZStep;
    /* 0x10 */ s16 rotYStep;
} EnButteFlightParams; // size = 0x14

static EnButteFlightParams Normal_speed_data[] = {
    { 5, 35, 0.0f, 0.1f, 0.5f, 0 },
    { 10, 45, 1.1f, 0.1f, 0.25f, 1000 },
    { 10, 40, 1.5f, 0.1f, 0.3f, 2000 },
};
static EnButteFlightParams Okkake_speed_data[] = {
    { 3, 3, 0.8f, 0.1f, 0.2f, 0 },
    { 10, 20, 2.0f, 0.3f, 1.0f, 0 },
    { 10, 20, 2.4f, 0.3f, 1.0f, 0 },
};

void reset_speed_mode(EnButte* this, EnButteFlightParams* flightParams) {
    if (this->flightParamsIdx == 0) {
        if (fqrand() < 0.6f) {
            this->flightParamsIdx = 1;
        } else {
            this->flightParamsIdx = 2;
        }
    } else {
        this->flightParamsIdx = 0;
    }

    this->timer = get_random_timer(flightParams->minTime, flightParams->maxTime);
}

static f32 Kira_scale = 0.0f;
static s16 Kira_prim_alpha_angle = 0;

void kira_choo_ct(void) {
    Kira_scale = 0.0f;
    Kira_prim_alpha_angle = 0;
}

void kira_choo_move(void) {
    Kira_scale += 0.003f;
    Kira_prim_alpha_angle += 4000;
}

void kira_choo_draw(EnButte* this, PlayState* play) {
    static Vec3f base_vec_z = { 0.0f, 0.0f, -3.0f };
    Vec3f sp5C;
    s32 alpha;
    Vec3s camDir;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_choo.c", 295);

    texture_z_light_prim_xlu_disp(play->state.gfxCtx);

    alpha = sin_s(Kira_prim_alpha_angle) * 250;
    alpha = CLAMP(alpha, 0, 255);

    camDir = getRealCameraAngle(GET_ACTIVE_CAM(play));
    Matrix_rotateY(BINANG_TO_RAD(camDir.y), MTXMODE_NEW);
    Matrix_rotateX(BINANG_TO_RAD(camDir.x), MTXMODE_APPLY);
    Matrix_rotateZ(BINANG_TO_RAD(camDir.z), MTXMODE_APPLY);
    Matrix_Position(&base_vec_z, &sp5C);
    Matrix_softcv3_load(this->actor.focus.pos.x + sp5C.x, this->actor.focus.pos.y + sp5C.y,
                                 this->actor.focus.pos.z + sp5C.z, &camDir);
    Matrix_scale(Kira_scale, Kira_scale, Kira_scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_choo.c", 317);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 200, 200, 180, alpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 200, 200, 210, 255);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gEffFlash1DL));

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_choo.c", 326);
}

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 700, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 20, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 600, ICHAIN_STOP),
};

void En_Choo_actor_ct(Actor* thisx, PlayState* play) {
    EnButte* this = (EnButte*)thisx;

    if (this->actor.params == -1) {
        this->actor.params = 0;
    }

    ValueSet_process(&this->actor, value_init);

    if (PARAMS_GET_U(this->actor.params, 0, 1) == 1) {
        this->actor.cullingVolumeScale = 200.0f;
    }

    Skeleton_Info2_M_ct(play, &this->skelAnime, &gButterflySkel, &gButterflyAnim, this->jointTable, this->morphTable, 8);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &CrossSphDt_choo, this->colliderItems);
    this->actor.colChkInfo.mass = 0;
    this->unk_25C = fqrand() * 0xFFFF;
    this->unk_25E = fqrand() * 0xFFFF;
    this->unk_260 = fqrand() * 0xFFFF;
    Skeleton_Info2_init(&this->skelAnime, &gButterflyAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, 0.0f);
    mv_normal_init(this);
    this->actor.shape.rot.x -= 0x2320;
    this->drawSkelAnime = true;
    // "field keep butterfly"
    PRINTF("(field keep 蝶)(%x)(arg_data 0x%04x)\n", this, this->actor.params);
}

void En_Choo_actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnButte* this = (EnButte*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

void yuragi_updown_normal(EnButte* this) {
    static f32 speed_a1[] = { 50.0f, 80.0f, 100.0f };
    static f32 speed_a2[] = { 30.0f, 40.0f, 50.0f };

    this->actor.shape.yOffset += sin_s(this->unk_25C) * speed_a1[this->flightParamsIdx] +
                                 sin_s(this->unk_25E) * speed_a2[this->flightParamsIdx];
    this->actor.shape.yOffset = CLAMP(this->actor.shape.yOffset, -2000.0f, 2000.0f);
}

void yuragi_updown_okkake(EnButte* this) {
    static f32 speed_a1[] = { 15.0f, 20.0f, 25.0f };
    static f32 speed_a2[] = { 7.5f, 10.0f, 12.5f };

    this->actor.shape.yOffset += sin_s(this->unk_25C) * speed_a1[this->flightParamsIdx] +
                                 sin_s(this->unk_25E) * speed_a2[this->flightParamsIdx];
    this->actor.shape.yOffset = CLAMP(this->actor.shape.yOffset, -500.0f, 500.0f);
}

void set_shape_angle(EnButte* this) {
    s16 target = this->actor.world.rot.y + 0x8000;
    s16 diff = target - this->actor.shape.rot.y;

    chase_angle(&this->actor.shape.rot.y, target, ABS(diff) >> 3);
    this->actor.shape.rot.x = (s16)(sinf(this->unk_260) * 600.0f) - 0x2320;
}

void mv_normal_init(EnButte* this) {
    reset_speed_mode(this, &Normal_speed_data[this->flightParamsIdx]);
    this->actionFunc = mv_normal;
}

void mv_normal(EnButte* this, PlayState* play) {
    EnButteFlightParams* flightParams = &Normal_speed_data[this->flightParamsIdx];
    s16 yaw;
    Player* player = GET_PLAYER(play);
    f32 distSqFromHome;
    f32 maxDistSqFromHome;
    f32 minAnimSpeed;
    f32 animSpeed;
    s16 rotStep;

    distSqFromHome = Math3DLengthSquare2D(this->actor.world.pos.x, this->actor.world.pos.z, this->actor.home.pos.x,
                                     this->actor.home.pos.z);
    yuragi_updown_normal(this);
    add_calc(&this->actor.speed, flightParams->speedXZTarget, flightParams->speedXZScale,
                       flightParams->speedXZStep, 0.0f);

    if (this->unk_257 == 1) {
        maxDistSqFromHome = SQ(100.0f);
        rotStep = 1000;
    } else {
        maxDistSqFromHome = SQ(35.0f);
        rotStep = 600;
    }

    minAnimSpeed = 0.0f;
    this->posYTarget = this->actor.home.pos.y;

    if ((this->flightParamsIdx != 0) && ((distSqFromHome > maxDistSqFromHome) || (this->timer < 4))) {
        yaw = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);
        if (chase_angle(&this->actor.world.rot.y, yaw, flightParams->rotYStep) == 0) {
            minAnimSpeed = 0.5f;
        }
    } else if ((this->unk_257 == 0) && (this->actor.child != NULL) && (this->actor.child != &this->actor)) {
        yaw = search_position_angleY(&this->actor.world.pos, &this->actor.child->world.pos);
        if (chase_angle(&this->actor.world.rot.y, yaw, rotStep) == 0) {
            minAnimSpeed = 0.3f;
        }
    } else if (this->unk_257 == 1) {
        yaw = this->actor.yawTowardsPlayer + 0x8000 + (s16)((fqrand() - 0.5f) * 0x6000);
        if (chase_angle(&this->actor.world.rot.y, yaw, rotStep) == 0) {
            minAnimSpeed = 0.4f;
        }
    } else {
        this->actor.world.rot.y += (s16)(sinf(this->unk_25C) * 100.0f);
    }

    set_shape_angle(this);

    animSpeed = this->actor.speed / 2.0f + fqrand() * 0.2f + (1.0f - sin_s(this->unk_260)) * 0.15f +
                (1.0f - sin_s(this->unk_25E)) * 0.3f + minAnimSpeed;
    this->skelAnime.playSpeed = CLAMP(animSpeed, 0.2f, 1.5f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer <= 0) {
        reset_speed_mode(this, &Normal_speed_data[this->flightParamsIdx]);
    }

    if ((PARAMS_GET_U(this->actor.params, 0, 1) == 1) && (player->heldItemAction == PLAYER_IA_DEKU_STICK) &&
        (this->swordDownTimer <= 0) &&
        ((Math3DLengthSquare2D(player->actor.world.pos.x, player->actor.world.pos.z, this->actor.home.pos.x,
                          this->actor.home.pos.z) < SQ(120.0f)) ||
         (this->actor.xzDistToPlayer < 60.0f))) {
        mv_okkake_init(this);
        this->unk_257 = 2;
    } else if (this->actor.xzDistToPlayer < 120.0) {
        this->unk_257 = 1;
    } else {
        this->unk_257 = 0;
    }
}

void mv_okkake_init(EnButte* this) {
    reset_speed_mode(this, &Okkake_speed_data[this->flightParamsIdx]);
    this->actionFunc = mv_okkake;
}

void mv_okkake(EnButte* this, PlayState* play) {
    static s32 swing_dir = 1500;
    EnButteFlightParams* flightParams = &Okkake_speed_data[this->flightParamsIdx];
    Player* player = GET_PLAYER(play);
    f32 distSqFromHome;
    Vec3f swordTip;
    f32 animSpeed;
    f32 minAnimSpeed;
    f32 distSqFromSword;
    s16 yaw;

    yuragi_updown_okkake(this);
    add_calc(&this->actor.speed, flightParams->speedXZTarget, flightParams->speedXZScale,
                       flightParams->speedXZStep, 0.0f);
    minAnimSpeed = 0.0f;

    if ((this->flightParamsIdx != 0) && (this->timer < 12)) {
        swordTip.x = player->meleeWeaponInfo[0].tip.x + sin_s(player->actor.shape.rot.y) * 10.0f;
        swordTip.y = player->meleeWeaponInfo[0].tip.y;
        swordTip.z = player->meleeWeaponInfo[0].tip.z + cos_s(player->actor.shape.rot.y) * 10.0f;

        yaw = search_position_angleY(&this->actor.world.pos, &swordTip) + (s16)(fqrand() * swing_dir);
        if (chase_angle(&this->actor.world.rot.y, yaw, 2000) != 0) {
            if (play->gameplayFrames % 2) {
                this->actor.world.rot.y += (s16)(sinf(this->unk_25C) * 60.0f);
            }
        } else {
            minAnimSpeed = 0.3f;
        }
    }

    this->posYTarget = MAX(player->actor.world.pos.y + 30.0f, player->meleeWeaponInfo[0].tip.y);

    set_shape_angle(this);

    animSpeed = this->actor.speed / 2.0f + fqrand() * 0.2f + (1.0f - sin_s(this->unk_260)) * 0.15f +
                (1.0f - sin_s(this->unk_25E)) * 0.3f + minAnimSpeed;
    this->skelAnime.playSpeed = CLAMP(animSpeed, 0.2f, 1.5f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer <= 0) {
        reset_speed_mode(this, &Okkake_speed_data[this->flightParamsIdx]);
        swing_dir = -swing_dir;
    }

    distSqFromHome = Math3DLengthSquare2D(this->actor.world.pos.x, this->actor.world.pos.z, this->actor.home.pos.x,
                                     this->actor.home.pos.z);
    if (!((player->heldItemAction == PLAYER_IA_DEKU_STICK) && (fabsf(player->actor.speed) < 1.8f) &&
          (this->swordDownTimer <= 0) && (distSqFromHome < SQ(320.0f)))) {
        mv_normal_init(this);
    } else if (distSqFromHome > SQ(240.0f)) {
        distSqFromSword = Math3DLengthSquare2D(player->meleeWeaponInfo[0].tip.x, player->meleeWeaponInfo[0].tip.z,
                                          this->actor.world.pos.x, this->actor.world.pos.z);
        if (distSqFromSword < SQ(60.0f)) {
            mv_elf_init(this);
        }
    }
}

void mv_elf_init(EnButte* this) {
    this->timer = 9;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->skelAnime.playSpeed = 1.0f;
    kira_choo_ct();
    this->actionFunc = mv_elf;
}

void mv_elf(EnButte* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    kira_choo_move();

    if (this->timer == 5) {
        Effect_SE_Info_new(play, &this->actor.world.pos, 60, NA_SE_EV_BUTTERFRY_TO_FAIRY);
    } else if (this->timer == 4) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ELF, this->actor.focus.pos.x, this->actor.focus.pos.y,
                    this->actor.focus.pos.z, 0, this->actor.shape.rot.y, 0, FAIRY_HEAL_TIMED);
        this->drawSkelAnime = false;
    } else if (this->timer <= 0) {
        mv_end_init(this);
    }
}

static void mv_end_init(EnButte* this) {
    this->timer = 64;
    this->actionFunc = mv_end;
    this->actor.draw = NULL;
}

static void mv_end(EnButte* this, PlayState* play) {
    if (this->timer <= 0) {
        Actor_delete(&this->actor);
    }
}

void En_Choo_actor_move(Actor* thisx, PlayState* play) {
    EnButte* this = (EnButte*)thisx;

    if ((this->actor.child != NULL) && (this->actor.child->update == NULL) && (this->actor.child != &this->actor)) {
        this->actor.child = NULL;
    }

    if (this->timer > 0) {
        this->timer--;
    }

    this->unk_25C += 0x222;
    this->unk_25E += 0x1000;
    this->unk_260 += 0x600;

    if (PARAMS_GET_U(this->actor.params, 0, 1) == 1) {
        if (GET_PLAYER(play)->meleeWeaponState == 0) {
            if (this->swordDownTimer > 0) {
                this->swordDownTimer--;
            }
        } else {
            this->swordDownTimer = 80;
        }
    }

    this->actionFunc(this, play);

    if (this->actor.update != NULL) {
        Actor_position_moveF(&this->actor);
        chase_f(&this->actor.world.pos.y, this->posYTarget, 0.6f);
        if (this->actor.xyzDistToPlayerSq < 5000.0f) {
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        }
        Actor_world_to_eye(&this->actor, this->actor.shape.yOffset * this->actor.scale.y);
    }
}

void En_Choo_actor_draw(Actor* thisx, PlayState* play) {
    EnButte* this = (EnButte*)thisx;

    if (this->drawSkelAnime) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, NULL);
        CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
    }

    if ((PARAMS_GET_U(this->actor.params, 0, 1) == 1) && (this->actionFunc == mv_elf)) {
        kira_choo_draw(this, play);
    }
}
