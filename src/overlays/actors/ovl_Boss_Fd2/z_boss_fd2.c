/*
 * File: z_boss_fd2.c
 * Overlay: ovl_Boss_Fd2
 * Description: Volvagia, hole form
 */

#include "z_boss_fd2.h"
#include "assets/objects/object_fd2/object_fd2.h"
#include "overlays/actors/ovl_Boss_Fd/z_boss_fd.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "terminal.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

typedef enum BossFd2CutsceneState {
    /* 0 */ DEATH_START,
    /* 1 */ DEATH_RETREAT,
    /* 2 */ DEATH_HANDOFF,
    /* 3 */ DEATH_FD_BODY,
    /* 4 */ DEATH_FD_SKULL,
    /* 5 */ DEATH_FINISH
} BossFd2CutsceneState;

typedef enum BossFd2EyeState {
    /* 0 */ EYE_OPEN,
    /* 1 */ EYE_HALF,
    /* 2 */ EYE_CLOSED
} BossFd2EyeState;

void Boss_Fd2_actor_ct(Actor* thisx, PlayState* play);
void Boss_Fd2_actor_dt(Actor* thisx, PlayState* play);
void Boss_Fd2_actor_move(Actor* thisx, PlayState* play2);
void Boss_Fd2_actor_draw(Actor* thisx, PlayState* play);

static void mode_up_init(BossFd2* this, PlayState* play);
static void mode_up(BossFd2* this, PlayState* play);
static void mode_search_init(BossFd2* this, PlayState* play);
static void mode_search(BossFd2* this, PlayState* play);
static void mode_down(BossFd2* this, PlayState* play);
static void mode_atack_init(BossFd2* this, PlayState* play);
static void mode_atack(BossFd2* this, PlayState* play);
static void mode_atack2_init(BossFd2* this, PlayState* play);
static void mode_atack2(BossFd2* this, PlayState* play);
static void mode_damage(BossFd2* this, PlayState* play);
void mode_damage2(BossFd2* this, PlayState* play);
static void mode_enddemo(BossFd2* this, PlayState* play);
static void mode_wait(BossFd2* this, PlayState* play);

ActorProfile Boss_Fd2_Profile = {
    /**/ ACTOR_BOSS_FD2,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_FD2,
    /**/ sizeof(BossFd2),
    /**/ Boss_Fd2_actor_ct,
    /**/ Boss_Fd2_actor_dt,
    /**/ Boss_Fd2_actor_move,
    /**/ Boss_Fd2_actor_draw,
};

#include "z_boss_fd2.inc.c"

static Vec3f hole_pos[] = {
    { 0.0f, 90.0f, -243.0f },    { 0.0f, 90.0f, 0.0f },    { 0.0f, 90.0f, 243.0f },
    { -243.0f, 90.0f, -243.0f }, { -243.0f, 90.0f, 0.0f }, { -243.0f, 90.0f, 243.0f },
    { 243.0f, 90.0f, -243.0f },  { 243.0f, 90.0f, 0.0f },  { 243.0f, 90.0f, 243.0f },
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_VOLVAGIA, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 0, ICHAIN_STOP),
};

void Effect_Hahen_ct_IN_2(PlayState* play, BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration,
                         f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DEBRIS;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->vFdFxRotX = rnd_f(100.0f);
            effect->vFdFxRotY = rnd_f(100.0f);
            break;
        }
    }
}

void Effect_vb_fire_ct_IN_2(PlayState* play, BossFdEffect* effect, Vec3f* position, Vec3f* velocity,
                             Vec3f* acceleration, f32 scale, s16 alpha, s16 kbAngle) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_FIRE_BREATH;
            effect->timer1 = 0;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->pos.x -= effect->velocity.x;
            effect->pos.y -= effect->velocity.y;
            effect->pos.z -= effect->velocity.z;
            effect->vFdFxScaleMod = 0.0f;
            effect->alpha = alpha;
            effect->vFdFxYStop = rnd_f(10.0f);
            effect->timer2 = 0;
            effect->scale = scale / 400.0f;
            effect->kbAngle = kbAngle;
            break;
        }
    }
}

void Effect_vb_hinoko_ct_IN_2(PlayState* play, BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration,
                        f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == 0) {
            effect->type = BFD_FX_EMBER;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->alpha = 255;
            effect->timer1 = (s16)rnd_f(10.0f);
            break;
        }
    }
}

void Effect_vb_hm_ct_IN_2(PlayState* play, BossFdEffect* effect, Vec3f* position, Vec3f* velocity,
                             Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_SKULL_PIECE;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->scale = scale / 1000.0f;
            effect->vFdFxRotX = rnd_f(100.0f);
            effect->vFdFxRotY = rnd_f(100.0f);
            break;
        }
    }
}

void Effect_vb_smoke_ct_IN_2(BossFdEffect* effect, Vec3f* position, Vec3f* velocity, Vec3f* acceleration, f32 scale) {
    s16 i;

    for (i = 0; i < 180; i++, effect++) {
        if (effect->type == BFD_FX_NONE) {
            effect->type = BFD_FX_DUST;
            effect->pos = *position;
            effect->velocity = *velocity;
            effect->accel = *acceleration;
            effect->timer2 = 0;
            effect->scale = scale / 400.0f;
            break;
        }
    }
}

void Boss_Fd2_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BossFd2* this = (BossFd2*)thisx;

    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, 0.0069999993f);
    this->actor.world.pos.y = -850.0f;
    Shape_Info_init(&this->actor.shape, -580.0f / this->actor.scale.y, NULL, 0.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gHoleVolvagiaSkel, &gHoleVolvagiaIdleAnim, NULL, NULL, 0);
    if (this->actor.params == BFD_CS_NONE) {
        mode_up_init(this, play);
    } else {
        this->actionFunc = mode_wait;
    }
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &Fd2AcOcInfoJntSphData, this->elements);
}

void Boss_Fd2_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BossFd2* this = (BossFd2*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjJntSph_dt_nzf(play, &this->collider);
}

static void mode_up_init(BossFd2* this, PlayState* play) {
    BossFd* bossFd = (BossFd*)this->actor.parent;
    s16 temp_rand;
    s8 health;

    PRINTF("UP INIT 1\n");
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gHoleVolvagiaEmergeAnim);
    this->actionFunc = mode_up;
    this->skelAnime.playSpeed = 0.0f;
    temp_rand = rnd_f(8.9f);
    this->actor.world.pos.x = hole_pos[temp_rand].x;
    this->actor.world.pos.z = hole_pos[temp_rand].z;
    this->work[FD2_ACTION_STATE] = 0;
    PRINTF("UP INIT 2\n");
    this->timers[0] = 10;
    if (bossFd != NULL) {
        health = bossFd->actor.colChkInfo.health;
        if (health >= 18) {
            this->work[FD2_FAKEOUT_COUNT] = 0;
        } else if (health >= 12) {
            this->work[FD2_FAKEOUT_COUNT] = 1;
        } else if (health >= 6) {
            this->work[FD2_FAKEOUT_COUNT] = 2;
        } else {
            this->work[FD2_FAKEOUT_COUNT] = 3;
        }
    }
}

static void mode_up(BossFd2* this, PlayState* play) {
    s16 holeTime;
    BossFd* bossFd = (BossFd*)this->actor.parent;
    Player* player = GET_PLAYER(play);
    s16 i;

    PRINTF("UP 1    mode %d\n", this->work[FD2_ACTION_STATE]);
    Skeleton_Info2_anime_play(&this->skelAnime);
    PRINTF("UP 1.5 \n");
    switch (this->work[FD2_ACTION_STATE]) {
        case 0:
            PRINTF("UP time %d \n", this->timers[0]);
            PRINTF("PL time %x \n", player);
            PRINTF("MT time %x \n", bossFd);
            if ((this->timers[0] == 0) && (player->actor.world.pos.y > 70.0f)) {
                s8 health;

                PRINTF("UP 1.6 \n");
                bossFd->faceExposed = 0;
                bossFd->holePosition.x = this->actor.world.pos.x;
                bossFd->holePosition.z = this->actor.world.pos.z;
                set_jisin_2(play, 1, 50, 0x5000);
                this->work[FD2_ACTION_STATE] = 1;
                this->work[FD2_HOLE_COUNTER]++;
                this->actor.world.pos.y = -200.0f;
                health = bossFd->actor.colChkInfo.health;
                if (health == 24) {
                    holeTime = 30;
                } else if (health >= 18) {
                    holeTime = 25;
                } else if (health >= 12) {
                    holeTime = 20;
                } else if (health >= 6) {
                    holeTime = 10;
                } else {
                    holeTime = 5;
                }
                this->timers[0] = holeTime;
                bossFd->timers[4] = this->timers[0] + 10;
                PRINTF("UP 1.7 \n");
            }
            break;
        case 1:
            if (this->timers[0] == 0) {
                if (this->work[FD2_FAKEOUT_COUNT] != 0) {
                    this->work[FD2_FAKEOUT_COUNT]--;
                    i = rnd_f(8.9f);
                    this->actor.world.pos.x = hole_pos[i].x;
                    this->actor.world.pos.z = hole_pos[i].z;
                    this->work[FD2_ACTION_STATE] = 0;
                    this->timers[0] = 10;
                } else {
                    this->skelAnime.playSpeed = 1.0f;
                    this->fwork[FD2_END_FRAME] = Si2_anime_end_frame(&gHoleVolvagiaEmergeAnim);
                    this->work[FD2_ACTION_STATE] = 2;
                    Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_ROAR);
                    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
                    this->timers[0] = 15;
                    this->actor.world.pos.y = 150.0f;
                    for (i = 0; i < 10; i++) {
                        this->rightMane.pos[i].x += rnd_fx(100.0f);
                        this->rightMane.pos[i].z += rnd_fx(100.0f);
                        this->leftMane.pos[i].x += rnd_fx(100.0f);
                        this->leftMane.pos[i].z += rnd_fx(100.0f);
                    }
                    bossFd->work[BFD_SPLASH_TIMER] = 5;
                }
            }
            break;
        case 2:
            adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x7D0);
            if ((this->timers[0] == 1) && (this->actor.xzDistToPlayer < 120.0f)) {
                Actor_player_power_damage_AT_set(play, &this->actor, 3.0f, this->actor.yawTowardsPlayer, 2.0f, 0x20);
                Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
            }
            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[FD2_END_FRAME])) {
                mode_search_init(this, play);
            }
            break;
    }
    PRINTF("UP 2\n");
}

static void mode_search_init(BossFd2* this, PlayState* play) {
    BossFd* bossFd = (BossFd*)this->actor.parent;
    s8 health;
    s16 idleTime;

    PRINTF("UP INIT 1\n");
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gHoleVolvagiaTurnAnim);
    this->actionFunc = mode_search;
    health = bossFd->actor.colChkInfo.health;
    if (health == 24) {
        idleTime = 50;
    } else if (health >= 18) {
        idleTime = 40;
    } else if (health >= 12) {
        idleTime = 40;
    } else if (health >= 6) {
        idleTime = 30;
    } else {
        idleTime = 20;
    }
    this->timers[0] = idleTime;
}

static void mode_search(BossFd2* this, PlayState* play) {
    s16 prevToLink;

    Skeleton_Info2_anime_play(&this->skelAnime);
    prevToLink = this->work[FD2_TURN_TO_LINK];
    this->work[FD2_TURN_TO_LINK] =
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x7D0, 0);
    PRINTF("SW1 = %d\n", prevToLink);
    PRINTF("SW2 = %d\n", this->work[FD2_TURN_TO_LINK]);
    if ((fabsf(prevToLink) <= 1000.0f) && (1000.0f < fabsf(this->work[FD2_TURN_TO_LINK]))) {
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gHoleVolvagiaTurnAnim, -5.0f);
    }
    if ((1000.0f < fabsf(prevToLink)) && (fabsf(this->work[FD2_TURN_TO_LINK]) <= 1000.0f)) {
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gHoleVolvagiaIdleAnim, -5.0f);
    }
    if (this->timers[0] == 0) {
        if (this->actor.xzDistToPlayer < 200.0f) {
            mode_atack2_init(this, play);
        } else {
            mode_atack_init(this, play);
        }
    }
}

static void mode_down_init(BossFd2* this, PlayState* play) {
    BossFd* bossFd = (BossFd*)this->actor.parent;

    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gHoleVolvagiaBurrowAnim, -5.0f);
    this->actionFunc = mode_down;
    this->fwork[FD2_END_FRAME] = Si2_anime_end_frame(&gHoleVolvagiaBurrowAnim);
    bossFd->timers[4] = 30;
    this->work[FD2_ACTION_STATE] = 0;
}

static void mode_down(BossFd2* this, PlayState* play) {
    BossFd* bossFd = (BossFd*)this->actor.parent;

    if (this->work[FD2_ACTION_STATE] == 0) {
        Skeleton_Info2_anime_play(&this->skelAnime);
        if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[FD2_END_FRAME])) {
            this->work[FD2_ACTION_STATE] = 1;
            this->timers[0] = 25;
        }
    } else {
        add_calc2(&this->actor.world.pos.y, -100.0f, 1.0f, 10.0f);
        if (this->timers[0] == 0) {
            if ((this->work[FD2_HOLE_COUNTER] >= 3) && ((s8)bossFd->actor.colChkInfo.health < 24)) {
                this->work[FD2_HOLE_COUNTER] = 0;
                this->actionFunc = mode_wait;
                bossFd->handoffSignal = FD2_SIGNAL_FLY;
            } else {
                mode_up_init(this, play);
            }
        }
    }
}

static void mode_atack_init(BossFd2* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gHoleVolvagiaBreatheFireAnim, -5.0f);
    this->actionFunc = mode_atack;
    this->fwork[FD2_END_FRAME] = Si2_anime_end_frame(&gHoleVolvagiaBreatheFireAnim);
    this->work[FD2_ACTION_STATE] = 0;
}

static Vec3f fd2_se_pos = { 0.0f, 0.0f, 50.0f }; // Unused? BossFd uses a similar array for its fire breath sfx.

static void mode_atack(BossFd2* this, PlayState* play) {
    s16 i;
    Vec3f toLink;
    s16 angleX;
    s16 angleY;
    s16 breathOpacity = 0;
    BossFd* bossFd = (BossFd*)this->actor.parent;
    Player* player = GET_PLAYER(play);
    f32 tempX;
    f32 tempY;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[FD2_END_FRAME])) {
        mode_down_init(this, play);
    }
    if ((25.0f <= this->skelAnime.curFrame) && (this->skelAnime.curFrame < 70.0f)) {
        if (this->skelAnime.curFrame == 25.0f) {
            play->envCtx.lightBlend = 0.0f;
        }
        Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_FIRE - SFX_FLAG);
        if (this->skelAnime.curFrame > 50) {
            breathOpacity = (70.0f - this->skelAnime.curFrame) * 12.0f;
        } else {
            breathOpacity = 255;
        }
        toLink.x = player->actor.world.pos.x - this->headPos.x;
        toLink.y = player->actor.world.pos.y - this->headPos.y;
        toLink.z = player->actor.world.pos.z - this->headPos.z;
        angleY = atans_table(toLink.z, toLink.x);
        angleX = -atans_table(sqrtf(SQ(toLink.x) + SQ(toLink.z)), toLink.y);
        angleY -= this->actor.shape.rot.y;
        if (angleY > 0x1F40) {
            angleY = 0x1F40;
        }
        if (angleY < -0x1F40) {
            angleY = -0x1F40;
        }
        angleX += (-0x1B58);
        if (angleX > 0x3E8) {
            angleX = 0x3E8;
        }
        if (angleX < -0xFA0) {
            angleX = -0xFA0;
        }
        adds(&this->headRot.y, angleY, 5, 0x7D0);
        adds(&this->headRot.x, angleX, 5, 0x7D0);
    } else {
        adds(&this->headRot.y, 0, 5, 0x7D0);
        adds(&this->headRot.x, 0, 5, 0x7D0);
    }
    if (breathOpacity != 0) {
        f32 breathScale;
        Vec3f spawnSpeed = { 0.0f, 0.0f, 0.0f };
        Vec3f spawnVel;
        Vec3f spawnAccel = { 0.0f, 0.0f, 0.0f };
        Vec3f spawnPos;

        bossFd->fogMode = 2;
        spawnSpeed.z = 30.0f;
        spawnPos = this->headPos;

        tempY = BINANG_TO_RAD_ALT(this->actor.shape.rot.y + this->headRot.y);
        tempX = BINANG_TO_RAD_ALT(this->headRot.x) + 1.0f / 2;
        Matrix_rotateY(tempY, MTXMODE_NEW);
        Matrix_rotateX(tempX, MTXMODE_APPLY);
        Matrix_Position(&spawnSpeed, &spawnVel);

        breathScale = 300.0f + 50.0f * sin_s(this->work[FD2_VAR_TIMER] * 0x2000);
        Effect_vb_fire_ct_IN_2(play, bossFd->effects, &spawnPos, &spawnVel, &spawnAccel, breathScale, breathOpacity,
                                this->actor.shape.rot.y + this->headRot.y);

        spawnPos.x += spawnVel.x * 0.5f;
        spawnPos.y += spawnVel.y * 0.5f;
        spawnPos.z += spawnVel.z * 0.5f;

        breathScale = 300.0f + 50.0f * sin_s(this->work[FD2_VAR_TIMER] * 0x2000);
        Effect_vb_fire_ct_IN_2(play, bossFd->effects, &spawnPos, &spawnVel, &spawnAccel, breathScale, breathOpacity,
                                this->actor.shape.rot.y + this->headRot.y);

        spawnSpeed.x = 0.0f;
        spawnSpeed.y = 17.0f;
        spawnSpeed.z = 0.0f;

        for (i = 0; i < 6; i++) {
            tempY = rnd_f(2.0f * M_PI);
            tempX = rnd_f(2.0f * M_PI);
            Matrix_rotateY(tempY, MTXMODE_NEW);
            Matrix_rotateX(tempX, MTXMODE_APPLY);
            Matrix_Position(&spawnSpeed, &spawnVel);

            spawnAccel.x = (spawnVel.x * -10.0f) / 100.0f;
            spawnAccel.y = (spawnVel.y * -10.0f) / 100.0f;
            spawnAccel.z = (spawnVel.z * -10.0f) / 100.0f;

            Effect_vb_hinoko_ct_IN_2(play, bossFd->effects, &this->headPos, &spawnVel, &spawnAccel,
                               (s16)rnd_f(2.0f) + 8);
        }
    }
}

static void mode_atack2_init(BossFd2* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gHoleVolvagiaClawSwipeAnim, -5.0f);
    this->actionFunc = mode_atack2;
    this->fwork[FD2_END_FRAME] = Si2_anime_end_frame(&gHoleVolvagiaClawSwipeAnim);
}

static void mode_atack2(BossFd2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 5.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_ROAR);
        Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_SW_NAIL);
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[FD2_END_FRAME])) {
        mode_down_init(this, play);
    }
}

static void mode_damage_init(BossFd2* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gHoleVolvagiaKnockoutAnim);
    this->fwork[FD2_END_FRAME] = Si2_anime_end_frame(&gHoleVolvagiaKnockoutAnim);
    this->actionFunc = mode_damage;
    this->work[FD2_ACTION_STATE] = 0;
}

static void mode_damage(BossFd2* this, PlayState* play) {
    BossFd* bossFd = (BossFd*)this->actor.parent;
    s16 i;

    this->disableAT = true;
    this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    Skeleton_Info2_anime_play(&this->skelAnime);
    switch (this->work[FD2_ACTION_STATE]) {
        case 0:
            if (Skeleton_Info_frame_check(&this->skelAnime, 13.0f)) {
                Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_MAHI2);
            }
            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[FD2_END_FRAME] - 3.0f)) {
                for (i = 0; i < 25; i++) {
                    Vec3f spawnVel;
                    Vec3f spawnAccel = { 0.0f, 0.0f, 0.0f };
                    Vec3f spawnPos;

                    spawnVel.x = rnd_fx(8.0f);
                    spawnVel.y = rnd_f(1.0f);
                    spawnVel.z = rnd_fx(8.0f);

                    spawnAccel.y = 0.5f;

                    spawnPos.x = rnd_fx(10.0f) + this->actor.focus.pos.x;
                    spawnPos.y = rnd_fx(10.0f) + this->actor.focus.pos.y;
                    spawnPos.z = rnd_fx(10.0f) + this->actor.focus.pos.z;

                    Effect_vb_smoke_ct_IN_2(bossFd->effects, &spawnPos, &spawnVel, &spawnAccel,
                                      rnd_f(100.0f) + 300.0f);
                }
                Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_LAND);
            }
            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[FD2_END_FRAME])) {
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gHoleVolvagiaVulnerableAnim, -5.0f);
                this->work[FD2_ACTION_STATE] = 1;
                this->timers[0] = 60;
            }
            break;
        case 1:
            if ((this->work[FD2_VAR_TIMER] & 0xF) == 0xF) {
                Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_KNOCKOUT);
            }
            if (this->timers[0] == 0) {
                mode_down_init(this, play);
            }
            break;
    }
}

void mode_damage2_init(BossFd2* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gHoleVolvagiaHitAnim);
    this->fwork[FD2_END_FRAME] = Si2_anime_end_frame(&gHoleVolvagiaHitAnim);
    this->actionFunc = mode_damage2;
    this->work[FD2_ACTION_STATE] = 0;
}

void mode_damage2(BossFd2* this, PlayState* play) {
    BossFd* bossFd = (BossFd*)this->actor.parent;

    Skeleton_Info2_anime_play(&this->skelAnime);
    this->disableAT = true;
    if (this->work[FD2_ACTION_STATE] == 0) {
        if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[FD2_END_FRAME])) {
            Skeleton_Info2_init_standard_stop(&this->skelAnime, &gHoleVolvagiaDamagedAnim);
            this->fwork[FD2_END_FRAME] = Si2_anime_end_frame(&gHoleVolvagiaDamagedAnim);
            this->work[FD2_ACTION_STATE] = 1;
        }
    } else if (this->work[FD2_ACTION_STATE] == 1) {
        if (Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
            Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_DAMAGE2);
        }
        if (Skeleton_Info_frame_check(&this->skelAnime, 20.0f)) {
            bossFd->timers[4] = 30;
        }
        if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[FD2_END_FRAME])) {
            this->work[FD2_ACTION_STATE] = 2;
            this->timers[0] = 25;
        }
    } else {
        add_calc2(&this->actor.world.pos.y, -100.0f, 1.0f, 10.0f);
        if (this->timers[0] == 0) {
            this->actionFunc = mode_wait;
            bossFd->handoffSignal = FD2_SIGNAL_FLY;
        }
    }
}

static void mode_enddemo_init(BossFd2* this, PlayState* play) {
    this->fwork[FD2_END_FRAME] = Si2_anime_end_frame(&gHoleVolvagiaDamagedAnim);
    Skeleton_Info2_init(&this->skelAnime, &gHoleVolvagiaDamagedAnim, 1.0f, 0.0f, this->fwork[FD2_END_FRAME],
                     ANIMMODE_ONCE_INTERP, -3.0f);
    this->actionFunc = mode_enddemo;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->deathState = DEATH_START;
}

static void demo_camera_set(BossFd2* this, PlayState* play) {
    if (this->subCamId != SUB_CAM_ID_DONE) {
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
        this->subCamAt.y += this->subCamAtYOffset;
        Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
        add_calc2(&this->subCamAtYOffset, 0.0f, 1.0f, 0.1f);
    }
}

static void mode_enddemo(BossFd2* this, PlayState* play) {
    f32 retreatSpeed;
    Vec3f sp70;
    Vec3f sp64;
    BossFd* bossFd = (BossFd*)this->actor.parent;
    Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
    f32 pad3;
    f32 pad2;
    f32 pad1;
    SkelAnime* skelAnime = &this->skelAnime;

    Skeleton_Info2_anime_play(skelAnime);
    switch (this->deathState) {
        case DEATH_START:
            this->deathState = DEATH_RETREAT;
            Demo_play_start(play, &play->csCtx);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
            this->subCamId = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            this->subCamEye = mainCam->eye;
            this->subCamAt = mainCam->at;
            this->subCamEyeVel.x = 100.0f;
            this->subCamEyeVel.y = 100.0f;
            this->subCamEyeVel.z = 100.0f;
            this->subCamAtVel.x = 100.0f;
            this->subCamAtVel.y = 100.0f;
            this->subCamAtVel.z = 100.0f;
            this->subCamAccel = 0.02f;
            this->timers[0] = 0;
            this->work[FD2_HOLE_COUNTER] = 0;
            this->subCamEyeMaxVelFrac.x = 0.1f;
            this->subCamEyeMaxVelFrac.y = 0.1f;
            this->subCamEyeMaxVelFrac.z = 0.1f;
            this->subCamAtMaxVelFrac.x = 0.1f;
            this->subCamAtMaxVelFrac.y = 0.1f;
            this->subCamAtMaxVelFrac.z = 0.1f;
            FALLTHROUGH;
        case DEATH_RETREAT:
            this->work[FD2_HOLE_COUNTER]++;
            if (this->work[FD2_HOLE_COUNTER] < 15) {
                retreatSpeed = 1.0f;
            } else if (this->work[FD2_HOLE_COUNTER] < 20) {
                retreatSpeed = 0.5f;
            } else {
                retreatSpeed = 0.25f;
            }
            if ((this->work[FD2_HOLE_COUNTER] == 1) || (this->work[FD2_HOLE_COUNTER] == 40)) {
                this->work[FD2_SCREAM_TIMER] = 20;
                if (this->work[FD2_HOLE_COUNTER] == 40) {
                    Nai_StopFx(NA_SE_EN_VALVAISA_DEAD);
                }

                Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_DAMAGE2);
            }
            add_calc2(&this->skelAnime.playSpeed, retreatSpeed, 1.0f, 1.0f);
            Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.yawTowardsPlayer) + 0.2f, MTXMODE_NEW);
            sp70.x = 0.0f;
            sp70.y = 0.0f;
            sp70.z = 250.0f;
            Matrix_Position(&sp70, &sp64);
            this->subCamEyeNext.x = this->actor.world.pos.x + sp64.x;
            this->subCamEyeNext.y = 140.0f;
            this->subCamEyeNext.z = this->actor.world.pos.z + sp64.z;
            if (this->actor.focus.pos.y >= 90.0f) {
                this->subCamAtNext.y = this->actor.focus.pos.y;
                this->subCamAtNext.x = this->actor.focus.pos.x;
                this->subCamAtNext.z = this->actor.focus.pos.z;
            }
            if (this->timers[0] == 0) {
                if (Skeleton_Info_frame_check(skelAnime, 20.0f)) {
                    bossFd->timers[4] = 60;
                }
                if (this->work[FD2_HOLE_COUNTER] >= 100) {
                    this->deathState = DEATH_HANDOFF;
                    this->timers[0] = 50;
                }
            } else if (Skeleton_Info_frame_check(skelAnime, 15.0f)) {
                Skeleton_Info2_init_standard_stop_morf(skelAnime, &gHoleVolvagiaDamagedAnim, -10.0f);
            }
            break;
        case DEATH_HANDOFF:
            if (this->timers[0] == 0) {
                this->actor.draw = NULL;
                this->deathState = DEATH_FD_BODY;
                bossFd->handoffSignal = FD2_SIGNAL_DEATH;
                this->work[FD2_ACTION_STATE] = 0;
                this->subCamVelFactor = 0.0f;
            } else {
                add_calc2(&this->actor.world.pos.y, -100.0f, 1.0f, 5.0f);
            }
            break;
        case DEATH_FD_BODY:
            if (bossFd->actor.world.pos.y < 80.0f) {
                if (bossFd->actor.world.rot.x > 0x3000) {
                    this->subCamAtNext = bossFd->actor.world.pos;
                    this->subCamAtNext.y = 80.0f;
                    this->subCamEyeNext.x = bossFd->actor.world.pos.x;
                    this->subCamEyeNext.y = 150.0f;
                    this->subCamEyeNext.z = bossFd->actor.world.pos.z + 300.0f;
                }
            } else {
                this->subCamAtNext = bossFd->actor.world.pos;
                this->subCamEyeNext.x = this->actor.world.pos.x;
                add_calc2(&this->subCamEyeNext.y, 200.0f, 1.0f, 2.0f);
                add_calc2(&this->subCamEyeNext.z, bossFd->actor.world.pos.z + 200.0f, 1.0f, 3.0f);
                if (this->work[FD2_ACTION_STATE] == 0) {
                    this->work[FD2_ACTION_STATE]++;
                    this->subCamVelFactor = 0.0f;
                    this->subCamAccel = 0.02f;
                    player_demo_mode_set(play, &bossFd->actor, PLAYER_CSACTION_1);
                }
            }
            if ((bossFd->work[BFD_ACTION_STATE] == BOSSFD_BONES_FALL) && (bossFd->timers[0] == 5)) {
                this->deathState = DEATH_FD_SKULL;
                this->subCamVelFactor = 0.0f;
                this->subCamAccel = 0.02f;
                this->subCamEyeNext.y = 150.0f;
                this->subCamEyeNext.z = bossFd->actor.world.pos.z + 300.0f;
            }
            break;
        case DEATH_FD_SKULL:
            add_calc2(&this->subCamAtNext.y, 100.0, 1.0f, 100.0f);
            this->subCamAtNext.x = 0.0f;
            this->subCamAtNext.z = 0.0f;
            this->subCamEyeNext.x = 0.0f;
            this->subCamEyeNext.y = 140.0f;
            add_calc2(&this->subCamEyeNext.z, 220.0f, 0.5f, 1.15f);
            if (bossFd->work[BFD_CAM_SHAKE_TIMER] != 0) {
                f32 cameraShake;

                bossFd->work[BFD_CAM_SHAKE_TIMER]--;
                cameraShake = bossFd->work[BFD_CAM_SHAKE_TIMER] / 0.5f;
                if (cameraShake >= 20.0f) {
                    cameraShake = 20.0f;
                }
                this->subCamAtYOffset = (bossFd->work[BFD_CAM_SHAKE_TIMER] & 1) ? cameraShake : -cameraShake;
            }
            if (bossFd->work[BFD_ACTION_STATE] == BOSSFD_SKULL_BURN) {
                this->deathState = DEATH_FINISH;
                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, 0.0f, 100.0f, 0.0f, 0, 0, 0,
                                   WARP_DUNGEON_ADULT);
                Actor_Environment_room_clear_On(play, play->roomCtx.curRoom.num);
            }
            break;
        case DEATH_FINISH:
            break;
    }
    demo_camera_set(this, play);
}

static void mode_wait(BossFd2* this, PlayState* play) {
    BossFd* bossFd = (BossFd*)this->actor.parent;

    if (bossFd->handoffSignal == FD2_SIGNAL_GROUND) {
        bossFd->handoffSignal = FD2_SIGNAL_NONE;
        mode_up_init(this, play);
        this->timers[0] = 20;
        this->work[FD2_HOLE_COUNTER] = 0;
    }
}

void Boss_Fd2_Damage_check(BossFd2* this, PlayState* play) {
    s16 i;
    ColliderElement* acHitElem;
    BossFd* bossFd = (BossFd*)this->actor.parent;

    if (this->actionFunc == mode_atack2) {
        Player* player = GET_PLAYER(play);

        for (i = 0; i < ARRAY_COUNT(this->elements); i++) {
            if (this->collider.elements[i].base.atElemFlags & ATELEM_HIT) {
                this->collider.elements[i].base.atElemFlags &= ~ATELEM_HIT;
                Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
            }
        }
    }
    if (!bossFd->faceExposed) {
        this->collider.elements[0].base.elemMaterial = ELEM_MATERIAL_UNK2;
        this->collider.base.colMaterial = COL_MATERIAL_METAL;
    } else {
        this->collider.elements[0].base.elemMaterial = ELEM_MATERIAL_UNK3;
        this->collider.base.colMaterial = COL_MATERIAL_HIT3;
    }

    if (this->collider.elements[0].base.acElemFlags & ACELEM_HIT) {
        this->collider.elements[0].base.acElemFlags &= ~ACELEM_HIT;

        acHitElem = this->collider.elements[0].base.acHitElem;
        if (!bossFd->faceExposed) {
            if (acHitElem->atDmgInfo.dmgFlags & DMG_HAMMER) {
                bossFd->actor.colChkInfo.health -= 2;
                if ((s8)bossFd->actor.colChkInfo.health <= 2) {
                    bossFd->actor.colChkInfo.health = 1;
                }
                bossFd->faceExposed = true;
                mode_damage_init(this, play);
                this->work[FD2_INVINC_TIMER] = 30;
                this->work[FD2_DAMAGE_FLASH_TIMER] = 5;
                Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_MAHI1);
                for (i = 0; i < 30; i++) {
                    Vec3f debrisVel = { 0.0f, 0.0f, 0.0f };
                    Vec3f debrisAccel = { 0.0f, -1.0f, 0.0f };
                    Vec3f debrisPos;

                    debrisVel.x = rnd_fx(10.0f);
                    debrisVel.y = rnd_f(5.0f) + 8.0f;
                    debrisVel.z = rnd_fx(10.0f);

                    debrisPos.x = this->actor.focus.pos.x;
                    debrisPos.y = this->actor.focus.pos.y;
                    debrisPos.z = this->actor.focus.pos.z;

                    Effect_Hahen_ct_IN_2(play, bossFd->effects, &debrisPos, &debrisVel, &debrisAccel,
                                        (s16)rnd_f(10.0) + 10);
                }
            }
        } else {
            u8 canKill = false;
            u8 damage;

            if ((damage = GetSwordAP(acHitElem->atDmgInfo.dmgFlags)) == 0) {
                damage = (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_ICE) ? 4 : 2;
            } else {
                canKill = true;
            }
            if (acHitElem->atDmgInfo.dmgFlags & DMG_HOOKSHOT) {
                damage = 0;
            }
            if (((s8)bossFd->actor.colChkInfo.health > 2) || canKill) {
                bossFd->actor.colChkInfo.health -= damage;
                PRINTF_COLOR_GREEN();
                PRINTF("damage   %d\n", damage);
            }
            PRINTF_RST();
            PRINTF("hp %d\n", bossFd->actor.colChkInfo.health);

            if ((s8)bossFd->actor.colChkInfo.health <= 0) {
                bossFd->actor.colChkInfo.health = 0;
                mode_enddemo_init(this, play);
                this->work[FD2_DAMAGE_FLASH_TIMER] = 10;
                this->work[FD2_INVINC_TIMER] = 30000;
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
                Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_DEAD);
                Actor_info_finish(play, &this->actor);
            } else if (damage) {
                mode_damage2_init(this, play);
                this->work[FD2_DAMAGE_FLASH_TIMER] = 10;
                this->work[FD2_INVINC_TIMER] = 100;
                Actor_SE_set(&this->actor, NA_SE_EN_VALVAISA_DAMAGE1);
            }
            if (damage) {
                for (i = 0; i < 30; i++) {
                    Vec3f pieceVel = { 0.0f, 0.0f, 0.0f };
                    Vec3f pieceAccel = { 0.0f, -1.0f, 0.0f };
                    Vec3f piecePos;

                    pieceVel.x = rnd_fx(6.0f);
                    pieceVel.y = rnd_f(4.0f) + 6.0f;
                    pieceVel.z = rnd_fx(6.0f);

                    piecePos.x = this->actor.focus.pos.x;
                    piecePos.y = this->actor.focus.pos.y;
                    piecePos.z = this->actor.focus.pos.z;

                    Effect_vb_hm_ct_IN_2(play, bossFd->effects, &piecePos, &pieceVel, &pieceAccel,
                                            (s16)rnd_f(6.0f) + 10);
                }
            }
        }
    }
}

static void another_cont(BossFd2* this, PlayState* play) {
    f32 maxOpen;
    f32 openRate;
    s16 eyeStates[5] = { EYE_OPEN, EYE_HALF, EYE_CLOSED, EYE_CLOSED, EYE_HALF };

    if (((this->work[FD2_VAR_TIMER] % 8) == 0) && (fqrand() < 0.3f)) {
        this->work[FD2_BLINK_TIMER] = 4;
    }
    if ((this->actionFunc == mode_damage) || (this->actionFunc == mode_damage2)) {
        if (this->work[FD2_VAR_TIMER] & 0x10) {
            this->eyeState = EYE_HALF;
        } else {
            this->eyeState = EYE_CLOSED;
        }
    } else {
        this->eyeState = eyeStates[this->work[FD2_BLINK_TIMER]];
    }

    if (this->work[FD2_BLINK_TIMER] != 0) {
        this->work[FD2_BLINK_TIMER]--;
    }

    if (this->work[FD2_SCREAM_TIMER] != 0) {
        maxOpen = 6000.0f;
        openRate = 1300.0f;
    } else {
        maxOpen = (this->work[FD2_VAR_TIMER] & 0x10) ? 1000.0f : 0.0f;
        openRate = 700.0f;
    }
    add_calc2(&this->jawOpening, maxOpen, 0.3f, openRate);

    if (this->work[FD2_SCREAM_TIMER] != 0) {
        this->work[FD2_SCREAM_TIMER]--;
    }
}

void Boss_Fd2_actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossFd2* this = (BossFd2*)thisx;
    s16 i;

    PRINTF("FD2 move start \n");
    this->disableAT = false;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    this->work[FD2_VAR_TIMER]++;
    this->work[FD2_UNK_TIMER]++;

    this->actionFunc(this, play);

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }
    if (this->work[FD2_DAMAGE_FLASH_TIMER] != 0) {
        this->work[FD2_DAMAGE_FLASH_TIMER]--;
    }
    if (this->work[FD2_INVINC_TIMER] != 0) {
        this->work[FD2_INVINC_TIMER]--;
    }

    if (this->deathState == DEATH_START) {
        if (this->work[FD2_INVINC_TIMER] == 0) {
            Boss_Fd2_Damage_check(this, play);
        }
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        if (!this->disableAT) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }
    }

    another_cont(this, play);
    this->fwork[FD2_TEX1_SCROLL_X] += 4.0f;
    this->fwork[FD2_TEX1_SCROLL_Y] = 120.0f;
    this->fwork[FD2_TEX2_SCROLL_X] += 3.0f;
    this->fwork[FD2_TEX2_SCROLL_Y] -= 2.0f;
    if (this->actor.focus.pos.y < 90.0f) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

s32 Boss_Fd2_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossFd2* this = (BossFd2*)thisx;
    BossFd* bossFd = (BossFd*)this->actor.parent;

    if (limbIndex == 31) {
        rot->y -= (f32)this->headRot.y;
        rot->z += (f32)this->headRot.x;
    }
    switch (limbIndex) {
        case 35:
        case 36:
            rot->z -= this->jawOpening * 0.1f;
            break;
        case 32:
            rot->z += this->jawOpening;
            break;
    }
    if ((bossFd->faceExposed == 1) && (limbIndex == 35)) {
        *dList = gHoleVolvagiaBrokenFaceDL;
    }

    if ((limbIndex == 32) || (limbIndex == 35) || (limbIndex == 36)) {
        OPEN_DISPS(play->state.gfxCtx, "../z_boss_fd2.c", 2165);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, (s8)bossFd->fwork[BFD_HEAD_TEX2_ALPHA]);
        CLOSE_DISPS(play->state.gfxCtx, "../z_boss_fd2.c", 2172);
    } else {
        OPEN_DISPS(play->state.gfxCtx, "../z_boss_fd2.c", 2174);
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, (s8)bossFd->fwork[BFD_BODY_TEX2_ALPHA]);
        CLOSE_DISPS(play->state.gfxCtx, "../z_boss_fd2.c", 2181);
    }
    if ((0 < limbIndex) && (limbIndex < 16)) {
        *dList = NULL;
    }
    return false;
}

void Boss_Fd2_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f center_p = { 4500.0f, 0.0f, 0.0f };
    static Vec3f fire_p = { 4000.0f, 0.0f, 0.0f };
    static Vec3f sok_p = { 4000.0f, -2900.0, 2000.0f };
    static Vec3f sok_pl = { 4000.0f, -1600.0, 0.0f };
    static Vec3f sok_pr = { 4000.0f, -1600.0, -2000.0f };
    BossFd2* this = (BossFd2*)thisx;

    if (limbIndex == 35) {
        Matrix_Position(&center_p, &this->actor.focus.pos);
        Matrix_Position(&fire_p, &this->headPos);
        Matrix_Position(&sok_p, &this->centerMane.head);
        Matrix_Position(&sok_pl, &this->rightMane.head);
        Matrix_Position(&sok_pr, &this->leftMane.head);
    }
    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->collider);
}

void sok_sub(BossFd2* this, PlayState* play, Vec3f* head, Vec3f* pos, Vec3f* rot, Vec3f* pull, f32* scale) {
    f32 sp138[10] = { 0.0f, 100.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    f32 sp110[10] = { 0.0f, 5.0f, -10.0f, 500.0f, 500.0f, 500.0f, 500.0f, 500.0f, 500.0f, 500.0f };
    f32 spE8[10] = { 0.4f, 0.6f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
    s16 i;
    Vec3f temp_vec;
    f32 temp_radius;
    f32 phi_f0;
    f32 temp_angleX;
    f32 temp_angleY;
    Vec3f spBC;
    Vec3f spB0;
    f32 xyScale;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_fd2.c", 2389);
    Matrix_push();
    gDPPipeSync(POLY_OPA_DISP++);

    for (i = 0; i < 10; i++) {
        if (i == 0) {
            (pos + i)->x = head->x;
            (pos + i)->y = head->y;
            (pos + i)->z = head->z;
        } else {
            add_calc2(&(pull + i)->x, 0.0f, 1.0f, 1.0f);
            add_calc2(&(pull + i)->y, 0.0f, 1.0f, 1.0f);
            add_calc2(&(pull + i)->z, 0.0f, 1.0f, 1.0f);
        }
    }

    for (i = 1; i < 10; i++) {
        temp_vec.x = (pos + i)->x + (pull + i)->x - (pos + i - 1)->x;

        phi_f0 = (pos + i)->y + (pull + i)->y - 2.0f + sp138[i];
        if (phi_f0 > (pos + i - 1)->y + sp110[i]) {
            phi_f0 = (pos + i - 1)->y + sp110[i];
        }
        if ((head->y >= -910.0f) && (phi_f0 < 110.0f)) {
            phi_f0 = 110.0f;
        }
        temp_vec.y = phi_f0 - (pos + i - 1)->y;

        temp_vec.z = (pos + i)->z + (pull + i)->z - (pos + i - 1)->z;
        temp_angleY = atanf_table(temp_vec.z, temp_vec.x);
        temp_radius = sqrtf(SQ(temp_vec.x) + SQ(temp_vec.z));
        temp_angleX = -atanf_table(temp_radius, temp_vec.y);
        (rot + i - 1)->y = temp_angleY;
        (rot + i - 1)->x = temp_angleX;
        spBC.x = 0.0f;
        spBC.y = 0.0f;
        spBC.z = spE8[i] * 25.0f;
        Matrix_rotateY(temp_angleY, MTXMODE_NEW);
        Matrix_rotateX(temp_angleX, MTXMODE_APPLY);
        Matrix_Position(&spBC, &spB0);
        temp_vec.x = (pos + i)->x;
        temp_vec.y = (pos + i)->y;
        temp_vec.z = (pos + i)->z;
        (pos + i)->x = (pos + i - 1)->x + spB0.x;
        (pos + i)->y = (pos + i - 1)->y + spB0.y;
        (pos + i)->z = (pos + i - 1)->z + spB0.z;
        (pull + i)->x = (((pos + i)->x - temp_vec.x) * 88.0f) / 100.0f;
        (pull + i)->y = (((pos + i)->y - temp_vec.y) * 88.0f) / 100.0f;
        (pull + i)->z = (((pos + i)->z - temp_vec.z) * 88.0f) / 100.0f;
        if ((pull + i)->x > 30.0f) {
            (pull + i)->x = 30.0f;
        }
        if ((pull + i)->x < -30.0f) {
            (pull + i)->x = -30.0f;
        }
        if ((pull + i)->y > 30.0f) {
            (pull + i)->y = 30.0f;
        }
        if ((pull + i)->y < -30.0f) {
            (pull + i)->y = -30.0f;
        }
        if ((pull + i)->z > 30.0f) {
            (pull + i)->z = 30.0f;
        }
        if ((pull + i)->z < -30.0f) {
            (pull + i)->z = -30.0f;
        }
    }

    for (i = 0; i < 9; i++) {
        Matrix_translate((pos + i)->x, (pos + i)->y, (pos + i)->z, MTXMODE_NEW);
        Matrix_rotateY((rot + i)->y, MTXMODE_APPLY);
        Matrix_rotateX((rot + i)->x, MTXMODE_APPLY);
        xyScale = (0.01f - (i * 0.0009f)) * spE8[i] * scale[i];
        Matrix_scale(xyScale, xyScale, 0.01f * spE8[i], MTXMODE_APPLY);
        Matrix_rotateX(M_PI / 2.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_fd2.c", 2498);
        gSPDisplayList(POLY_XLU_DISP++, gHoleVolvagiaManeModelDL);
    }
    Matrix_pull();
    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_fd2.c", 2503);
}

void sok_test(BossFd2* this, PlayState* play) {
    s32 pad;
    BossFd* bossFd = (BossFd*)this->actor.parent;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_fd2.c", 2515);
    if (1) {}
    for (i = 0; i < 10; i++) {
        this->centerMane.scale[i] = 1.5f + 0.3f * sin_s(5596.0f * this->work[FD2_VAR_TIMER] + i * 0x3200);
        this->rightMane.scale[i] = 1.5f + 0.3f * sin_s(5496.0f * this->work[FD2_VAR_TIMER] + i * 0x3200);
        this->leftMane.scale[i] = 1.5f + 0.3f * cos_s(5696.0f * this->work[FD2_VAR_TIMER] + i * 0x3200);
    }

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gSPDisplayList(POLY_XLU_DISP++, gHoleVolvagiaManeMaterialDL);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, bossFd->fwork[BFD_MANE_COLOR_CENTER], 0, 255);
    sok_sub(this, play, &this->centerMane.head, this->centerMane.pos, this->centerMane.rot,
                       this->centerMane.pull, this->centerMane.scale);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, bossFd->fwork[BFD_MANE_COLOR_RIGHT], 0, 255);
    sok_sub(this, play, &this->rightMane.head, this->rightMane.pos, this->rightMane.rot,
                       this->rightMane.pull, this->rightMane.scale);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, bossFd->fwork[BFD_MANE_COLOR_LEFT], 0, 255);
    sok_sub(this, play, &this->leftMane.head, this->leftMane.pos, this->leftMane.rot, this->leftMane.pull,
                       this->leftMane.scale);

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_fd2.c", 2601);
}

void Boss_Fd2_actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_tex_no[] = { gHoleVolvagiaEyeOpenTex, gHoleVolvagiaEyeHalfTex, gHoleVolvagiaEyeClosedTex };
    s32 pad;
    BossFd2* this = (BossFd2*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_fd2.c", 2617);
    PRINTF("FD2 draw start \n");
    if (this->actionFunc != mode_wait) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        if (this->work[FD2_DAMAGE_FLASH_TIMER] & 2) {
            POLY_OPA_DISP = gfx_set_fog_nosync(POLY_OPA_DISP, 255, 255, 255, 0, 900, 1099);
        }
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eye_tex_no[this->eyeState]));

        gSPSegment(POLY_OPA_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (s16)this->fwork[FD2_TEX1_SCROLL_X],
                                    (s16)this->fwork[FD2_TEX1_SCROLL_Y], 0x20, 0x20, 1,
                                    (s16)this->fwork[FD2_TEX2_SCROLL_X], (s16)this->fwork[FD2_TEX2_SCROLL_Y], 0x20,
                                    0x20));
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 128);

        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              Boss_Fd2_draw_sub, Boss_Fd2_draw_sub2, &this->actor);
        sok_test(this, play);
        POLY_OPA_DISP = game_play_set_fog(play, POLY_OPA_DISP);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_fd2.c", 2688);
}
