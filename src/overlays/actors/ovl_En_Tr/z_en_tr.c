/*
 * File: z_en_tr.c
 * Overlay: ovl_En_Tr
 * Description: Koume and Kotake
 */

#include "z_en_tr.h"
#include "assets/objects/object_tr/object_tr.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Tr_Actor_ct(Actor* thisx, PlayState* play);
void En_Tr_Actor_dt(Actor* thisx, PlayState* play);
void En_Tr_Actor_move(Actor* thisx, PlayState* play);
void En_Tr_Actor_draw(Actor* thisx, PlayState* play);

static void move_wait(EnTr* this, PlayState* play);
void move_demo_make_dust_hidden(EnTr* this, PlayState* play);
void move_demo_hidden(EnTr* this, PlayState* play);
static void move_start(EnTr* this, PlayState* play);

void En_Tr_AngleYProc_local(EnTr* this, PlayState* play, s32 cueChannel);
void En_Tr_MoveProc_local(EnTr* this, PlayState* play, s32 cueChannel);
void En_Tr_StartProc_local(EnTr* this, PlayState* play, s32 cueChannel);

ActorProfile En_Tr_Profile = {
    /**/ ACTOR_EN_TR,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TR,
    /**/ sizeof(EnTr),
    /**/ En_Tr_Actor_ct,
    /**/ En_Tr_Actor_dt,
    /**/ En_Tr_Actor_move,
    /**/ En_Tr_Actor_draw,
};

// The first elements of these animation arrays are for Koume, the second for Kotake

static AnimationHeader* anime_table[] = {
    &gKotakeKoumeStandingBroomOverRightShoulderAnim,
    &gKotakeKoumeStandingBroomOverLeftShoulderAnim,
};

static AnimationHeader* D_80B24368[] = {
    &gKotakeKoumeLookOverLeftShoulderAnim,
    &gKotakeKoumeLookOverRightShoulderAnim,
};

static AnimationHeader* D_80B24370[] = {
    &gKotakeKoumeLookingOverLeftShoulderAnim,
    &gKotakeKoumeLookingOverRightShoulderAnim,
};

static AnimationHeader* D_80B24378[] = {
    &gKotakeKoumeFlyAnim,
    &gKotakeKoumeFlyAnim,
};

static AnimationHeader* D_80B24380[] = {
    &gKotakeKoumeCastMagicAnim,
    &gKotakeKoumeCastMagicAnim,
};

static f32 x[] = { 0.0f, 20.0f, -30.0f, 20.0f, -20.0f, -20.0f, 30.0f };

static f32 y[] = { 0.0f, 30.0f, 0.0f, -30.0f, 30.0f, -30.0f, 0.0f };

// Has to be 1-dimensional to match
static Color_RGBA8 color_table[4] = {
    { 255, 200, 0, 255 },
    { 255, 0, 0, 255 },
    { 255, 255, 255, 255 },
    { 0, 0, 255, 255 },
};

static void* tr_eye[] = {
    gKotakeKoumeEyeOpenTex,
    gKotakeKoumeEyeHalfTex,
    gKotakeKoumeEyeClosedTex,
};

void En_Tr_actor_set_process(EnTr* this, EnTrActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Tr_Actor_ct(Actor* thisx, PlayState* play) {
    EnTr* this = (EnTr*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    En_Tr_actor_set_process(this, move_wait);
    this->unk_2D4 = 0; // Set and not used
    this->actor.child = NULL;
    Actor_set_scale(&this->actor, 0.01f);

    switch (this->actor.params) {
        case TR_KOUME:
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gKoumeSkel, &gKotakeKoumeStandingBroomOverRightShoulderAnim,
                               this->jointTable, this->morphTable, KOTAKE_KOUME_LIMB_MAX);
            Skeleton_Info2_init_standard_stop(&this->skelAnime, &gKotakeKoumeStandingBroomOverRightShoulderAnim);
            this->animation = NULL;
            En_Tr_actor_set_process(this, move_start);
            this->cueChannel = 3;
            break;

        case TR_KOTAKE:
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gKotakeSkel, &gKotakeKoumeStandingBroomOverLeftShoulderAnim,
                               this->jointTable, this->morphTable, KOTAKE_KOUME_LIMB_MAX);
            Skeleton_Info2_init_standard_stop(&this->skelAnime, &gKotakeKoumeStandingBroomOverLeftShoulderAnim);
            this->animation = NULL;
            En_Tr_actor_set_process(this, move_start);
            this->cueChannel = 2;
            break;

        default:
            ASSERT(0, "0", "../z_en_tr.c", 277);
            break;
    }
}

void En_Tr_Actor_dt(Actor* thisx, PlayState* play) {
}

void move_shoot(EnTr* this, PlayState* play) {
    if (this->timer == 11) {
        // Both cry in the title screen cutscene, but only Kotake in the in-game cutscene
        if ((this->actor.params != TR_KOUME) || (z_common_data.sceneLayer == 6)) {
            Nai_FxFlagEntry(NA_SE_EN_TWINROBA_SHOOT_VOICE, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    }

    if (this->timer > 0) {
        this->timer--;
    } else if (this->actor.child != NULL) {
        this->actor.child = NULL;
    }
    Actor_level_SE_set(&this->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
}

static void move_wait(EnTr* this, PlayState* play) {
}

void move_demo_fly(EnTr* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.actorCues[this->cueChannel] != NULL) {
            switch (play->csCtx.actorCues[this->cueChannel]->id) {

                case 4:
                    Actor_set_scale(&this->actor, 0.01f);
                    En_Tr_actor_set_process(this, move_demo_make_dust_hidden);
                    this->timer = 24;
                    Actor_SE_set(&this->actor, NA_SE_EN_PO_DEAD2);
                    break;

                case 6:
                    Skeleton_Info2_init(&this->skelAnime, D_80B24380[this->actor.params], 1.0f, 0.0f,
                                     Si2_anime_end_frame(D_80B24380[this->actor.params]), ANIMMODE_ONCE, -5.0f);
                    En_Tr_actor_set_process(this, move_shoot);
                    this->animation = D_80B24378[this->actor.params];
                    this->timer = 39;
                    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0,
                                       this->actor.params + 9);
                    Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_MASIC1);
                    break;

                default:
                    En_Tr_MoveProc_local(this, play, this->cueChannel);
                    En_Tr_AngleYProc_local(this, play, this->cueChannel);
                    break;
            }
            Actor_level_SE_set(&this->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
        }
    }
}

void move_demo_rolling(EnTr* this, PlayState* play) {
    Vec3f originalPos = this->actor.world.pos;

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.actorCues[this->cueChannel] != NULL) {
            if (play->csCtx.actorCues[this->cueChannel]->id == 8) {
                En_Tr_MoveProc_local(this, play, this->cueChannel);
                this->actor.world.rot.y = atans_table(this->actor.velocity.z, this->actor.velocity.x);
                add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.world.rot.y, 10, 0x400, 0x100);
                this->actor.world.rot.y = this->actor.shape.rot.y;
            } else {
                En_Tr_StartProc_local(this, play, this->cueChannel);
                this->actor.world.pos.x += sin_s(this->timer) * 150.0f;
                this->actor.world.pos.y += -100.0f;
                this->actor.world.pos.z += cos_s(this->timer) * 150.0f;

                this->actor.shape.rot.y = (s16)(this->timer) + 0x4000;
                this->timer += 0x400;

                this->actor.velocity.x = this->actor.world.pos.x - originalPos.x;
                this->actor.velocity.y = this->actor.world.pos.y - originalPos.y;
                this->actor.velocity.z = this->actor.world.pos.z - originalPos.z;
            }

            if (play->csCtx.curFrame < 670) {
                Actor_level_SE_set(&this->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
            }
        }
    }
}

void make_dust_local(EnTr* this, PlayState* play, s32 arg2, f32 arg3, f32 scale) {
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    Vec3f sp58;
    Color_RGBA8* primColor;
    Color_RGBA8* envColor;
    Vec3f cameraEye = GET_ACTIVE_CAM(play)->eye;
    s16 yaw = search_position_angleY(&cameraEye, &this->actor.world.pos);
    s16 reversePitch = -search_position_angleX(&cameraEye, &this->actor.world.pos);
    f32 sp3C;

    accel.x = accel.z = 0.0f;
    sp3C = sin_s(yaw);
    velocity.x = cos_s(reversePitch) * (arg3 * sp3C);
    velocity.y = sin_s(reversePitch) * arg3;
    sp3C = cos_s(yaw);
    velocity.z = cos_s(reversePitch) * (arg3 * sp3C);
    accel.y = 0.5f;

    primColor = &color_table[2 * this->actor.params];
    envColor = &color_table[2 * this->actor.params + 1];

    sp58 = this->actor.world.pos;
    sp58.x -= velocity.x * 10.0f;
    sp58.y -= velocity.y * 10.0f;
    sp58.z -= velocity.z * 10.0f;

    pos.x = sp58.x + ((x[arg2] * scale) * cos_s(yaw));
    pos.y = sp58.y + (y[arg2] * scale);
    pos.z = sp58.z - ((x[arg2] * scale) * sin_s(yaw));
    Effect_SS_Dust_sc_cl_ct(play, &pos, &velocity, &accel, primColor, envColor, (s32)(800.0f * scale), (s32)(80.0f * scale));
}

void move_demo_make_dust_hidden(EnTr* this, PlayState* play) {
    s32 temp_hi;

    if (this->timer >= 17) {
        this->actor.shape.rot.y = (this->actor.shape.rot.y - (this->timer * 0x28F)) + 0x3D68;
    } else {
        if (this->timer >= 5) {
            Actor_set_scale(&this->actor, this->actor.scale.x * 0.9f);
            this->actor.shape.rot.y = (this->actor.shape.rot.y - (this->timer * 0x28F)) + 0x3D68;
        } else if (this->timer > 0) {
            temp_hi = (this->timer * 2) % 7;

            make_dust_local(this, play, temp_hi, 5.0f, 0.2f);
            make_dust_local(this, play, (temp_hi + 1) % 7, 5.0f, 0.2f);
            Actor_set_scale(&this->actor, this->actor.scale.x * 0.9f);
            this->actor.shape.rot.y = (this->actor.shape.rot.y - (this->timer * 0x28F)) + 0x3D68;
        } else {
            En_Tr_actor_set_process(this, move_demo_hidden);
            this->actor.draw = NULL;
        }
    }

    if (this->timer == 4) {
        Actor_SE_set(&this->actor, NA_SE_EN_BUBLE_DOWN);
    }

    if (this->timer > 0) {
        this->timer--;
    }
}

void move_demo_make_dust_fly(EnTr* this, PlayState* play) {
    if (this->timer >= 31) {
        s32 temp_hi = (this->timer * 2) % 7;

        make_dust_local(this, play, temp_hi, 5.0f, 1.0f);
        make_dust_local(this, play, (temp_hi + 1) % 7, 5.0f, 1.0f);
    } else if (this->timer == 30) {
        this->actor.draw = En_Tr_Actor_draw;
        this->actor.shape.rot.y += this->timer * 0x1A6;
    } else if (this->timer > 0) {
        this->actor.shape.rot.y += this->timer * 0x1A6;
        Actor_set_scale(&this->actor, (this->actor.scale.x * 0.8f) + 0.002f);
    } else {
        En_Tr_actor_set_process(this, move_demo_fly);
        Actor_set_scale(&this->actor, 0.01f);
    }

    if (this->timer > 0) {
        this->timer--;
    }
    Actor_level_SE_set(&this->actor, NA_SE_EN_TWINROBA_FLY_DEMO - SFX_FLAG);
}

void move_demo_hidden(EnTr* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        if ((play->csCtx.actorCues[this->cueChannel] != NULL) && ((play->csCtx.actorCues[this->cueChannel]->id == 3) ||
                                                                  (play->csCtx.actorCues[this->cueChannel]->id == 5))) {
            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_TRANSFORM);
            this->timer = 34;
            En_Tr_StartProc_local(this, play, this->cueChannel);
            En_Tr_actor_set_process(this, move_demo_make_dust_fly);
            Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gKotakeKoumeFlyAnim);
            this->animation = NULL;
            Actor_set_scale(&this->actor, 0.003f);
        }
    }
}

void move_demo_furimuki(EnTr* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(D_80B24378[this->actor.params]);

    if (play->csCtx.state != CS_STATE_IDLE) {
        if ((play->csCtx.actorCues[this->cueChannel] != NULL) && (play->csCtx.actorCues[this->cueChannel]->id == 3)) {
            Skeleton_Info2_init(&this->skelAnime, D_80B24378[this->actor.params], 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP,
                             -10.0f);
            this->animation = NULL;
            En_Tr_actor_set_process(this, move_demo_fly);
        }
    }
}

static void move_demo_wait(EnTr* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(D_80B24368[this->actor.params]);

    if (play->csCtx.state != CS_STATE_IDLE) {
        if ((play->csCtx.actorCues[this->cueChannel] != NULL) && (play->csCtx.actorCues[this->cueChannel]->id == 2)) {
            Skeleton_Info2_init(&this->skelAnime, D_80B24368[this->actor.params], 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE,
                             -4.0f);
            this->animation = D_80B24370[this->actor.params];
            En_Tr_actor_set_process(this, move_demo_furimuki);
        }
    }
}

static void move_start(EnTr* this, PlayState* play) {
    u32 frames = play->gameplayFrames;

    if (play->csCtx.state != CS_STATE_IDLE) {
        if (play->csCtx.actorCues[this->cueChannel] != NULL) {
            switch (play->csCtx.actorCues[this->cueChannel]->id) {
                case 1:
                    En_Tr_StartProc_local(this, play, this->cueChannel);
                    En_Tr_actor_set_process(this, move_demo_wait);
                    break;

                case 3:
                    En_Tr_StartProc_local(this, play, this->cueChannel);
                    En_Tr_actor_set_process(this, move_demo_fly);
                    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gKotakeKoumeFlyAnim);
                    this->animation = NULL;
                    break;

                case 4:
                    En_Tr_actor_set_process(this, move_demo_hidden);
                    this->actor.draw = NULL;
                    break;

                case 7:
                    En_Tr_actor_set_process(this, move_demo_rolling);
                    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gKotakeKoumeFlyAnim);
                    this->animation = NULL;
                    this->timer =
                        ((this->actor.params != TR_KOUME) ? ((u8)frames * 0x400) + 0x8000 : (u8)frames * 0x400);
                    break;
            }
        }
    }
}

void En_Tr_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTr* this = (EnTr*)thisx;

    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    this->actionFunc(this, play);

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->animation != NULL) {
            if ((this->animation == &gKotakeKoumeLookingOverLeftShoulderAnim) ||
                (this->animation == &gKotakeKoumeLookingOverRightShoulderAnim)) {
                if (this->actor.params != TR_KOUME) {
                    Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_LAUGH2);
                } else {
                    Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_LAUGH);
                }
                Skeleton_Info2_init_standard_repeat(&this->skelAnime, this->animation);
            } else if (this->animation == &gKotakeKoumeFlyAnim) {
                En_Tr_actor_set_process(this, move_demo_fly);
                Skeleton_Info2_init(&this->skelAnime, &gKotakeKoumeFlyAnim, 1.0f, 0.0f,
                                 Si2_anime_end_frame(&gKotakeKoumeFlyAnim), ANIMMODE_LOOP, -5.0f);
            } else {
                Skeleton_Info2_init_standard_repeat(&this->skelAnime, this->animation);
            }
            this->animation = NULL;
        } else {
            this->skelAnime.curFrame = 0.0f;
        }
    }
    Actor_world_to_eye(&this->actor, 0.0f);

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = get_random_timer(60, 60);
    }
    this->eyeIndex = this->blinkTimer;
    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

s32 en_tr_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Vec3f src = { 2300.0f, 0.0f, -600.0f };
    Vec3f dest = { 0.0f, 0.0f, 0.0f };
    EnTr* this = (EnTr*)thisx;
    Actor* child = this->actor.child;

    if ((child != NULL) && (limbIndex == KOTAKE_KOUME_LIMB_RIGHT_HAND)) {
        Matrix_Position(&src, &dest);
        dest.x -= (10.0f * sin_s(getRealCameraAngleY(GET_ACTIVE_CAM(play))));
        dest.z -= (10.0f * cos_s(getRealCameraAngleY(GET_ACTIVE_CAM(play))));
        child->world.pos = dest;
    }
    return 0;
}

void En_Tr_Actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnTr* this = (EnTr*)thisx;

    if (1) {}

    if ((play->csCtx.state == CS_STATE_IDLE) || (play->csCtx.actorCues[this->cueChannel] == NULL)) {
        this->actor.shape.shadowDraw = NULL;
    } else {
        this->actor.shape.shadowDraw = Actor_shadow_circle;

        OPEN_DISPS(play->state.gfxCtx, "../z_en_tr.c", 840);
        _polygon_z_light_fog_prim(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tr_eye[this->eyeIndex]));
        Actor_HiliteReflect_set_init(&this->actor, play, 0);
        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              en_tr_display1, NULL, this);
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_tr.c", 854);
    }
}

static f32 get_demo_effect_parcent(PlayState* play, s32 cueChannel) {
    f32 phi_f2 = get_parcent(play->csCtx.actorCues[cueChannel]->endFrame,
                                        play->csCtx.actorCues[cueChannel]->startFrame, play->csCtx.curFrame);
    phi_f2 = CLAMP_MAX(phi_f2, 1.0f);
    return phi_f2;
}

void En_Tr_MoveProc_local(EnTr* this, PlayState* play, s32 cueChannel) {
    Vec3f startPos;
    Vec3f endPos;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 phi_f12;

    startPos.x = play->csCtx.actorCues[cueChannel]->startPos.x;
    startPos.y = play->csCtx.actorCues[cueChannel]->startPos.y;
    startPos.z = play->csCtx.actorCues[cueChannel]->startPos.z;

    endPos.x = play->csCtx.actorCues[cueChannel]->endPos.x;
    endPos.y = play->csCtx.actorCues[cueChannel]->endPos.y;
    endPos.z = play->csCtx.actorCues[cueChannel]->endPos.z;

    temp_f0 = get_demo_effect_parcent(play, cueChannel);

    startPos.x = ((endPos.x - startPos.x) * temp_f0) + startPos.x;
    startPos.y = ((endPos.y - startPos.y) * temp_f0) + startPos.y;
    startPos.z = ((endPos.z - startPos.z) * temp_f0) + startPos.z;

    endPos.x = (startPos.x - this->actor.world.pos.x) * 0.1f;
    endPos.y = (startPos.y - this->actor.world.pos.y) * 0.1f;
    endPos.z = (startPos.z - this->actor.world.pos.z) * 0.1f;

    temp_f0_2 = sqrtf(SQ(endPos.x) + SQ(endPos.y) + SQ(endPos.z));
    phi_f12 = CLAMP(temp_f0_2, 0.0f, 20.0f);

    if ((temp_f0_2 != phi_f12) && (temp_f0_2 != 0.0f)) {
        endPos.x *= phi_f12 / temp_f0_2;
        endPos.y *= phi_f12 / temp_f0_2;
        endPos.z *= phi_f12 / temp_f0_2;
    }

    chase_f(&this->actor.velocity.x, endPos.x, 1.0f);
    chase_f(&this->actor.velocity.y, endPos.y, 1.0f);
    chase_f(&this->actor.velocity.z, endPos.z, 1.0f);
    Actor_position_move(&this->actor);
}

void En_Tr_AngleYProc_local(EnTr* this, PlayState* play, s32 cueChannel) {
    s16 rotY = play->csCtx.actorCues[cueChannel]->rot.y;
    s32 rotDiff = this->actor.world.rot.y - rotY;
    s32 rotSign;

    if (rotDiff < 0) {
        rotDiff = -rotDiff;
        rotSign = 1;
    } else {
        rotSign = -1;
    }

    if (rotDiff >= 0x8000) {
        rotSign = -rotSign;
        rotDiff = 0x10000 - rotDiff;
    }

    rotDiff *= 0.1f;

    this->actor.world.rot.y += rotDiff * rotSign;
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void En_Tr_StartProc_local(EnTr* this, PlayState* play, s32 cueChannel) {
    Vec3f startPos;

    startPos.x = play->csCtx.actorCues[cueChannel]->startPos.x;
    startPos.y = play->csCtx.actorCues[cueChannel]->startPos.y;
    startPos.z = play->csCtx.actorCues[cueChannel]->startPos.z;

    this->actor.world.pos = startPos;
    this->actor.world.rot.y = this->actor.shape.rot.y = play->csCtx.actorCues[cueChannel]->rot.y;
}
