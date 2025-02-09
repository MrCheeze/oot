/*
 * File: z_en_bird.c
 * Overlay: ovl_En_Bird
 * Description: An unused brown bird
 */

#include "z_en_bird.h"
#include "assets/objects/object_bird/object_bird.h"

#define FLAGS 0

void En_Bird_Actor_ct(Actor* thisx, PlayState* play);
void En_bird_Actor_dt(Actor* thisx, PlayState* play);
void En_bird_move(Actor* thisx, PlayState* play);
void En_bird_display(Actor* thisx, PlayState* play);

void En_Bird_mode_rnd_move_init(EnBird* this, s16 params);
void En_Bird_mode_rnd_move(EnBird* this, PlayState* play);
void En_Bird_mode_wait(EnBird* this, PlayState* play);
void En_Bird_mode_wait_init(EnBird* this, s16 params);

ActorProfile En_Bird_Profile = {
    /**/ ACTOR_EN_BIRD,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_BIRD,
    /**/ sizeof(EnBird),
    /**/ En_Bird_Actor_ct,
    /**/ En_bird_Actor_dt,
    /**/ En_bird_move,
    /**/ En_bird_display,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 5600, ICHAIN_STOP),
};

void En_Bird_set_process(EnBird* this, EnBirdActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Bird_Actor_ct(Actor* thisx, PlayState* play) {
    EnBird* this = (EnBird*)thisx;

    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, 0.01);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gBirdSkel, &gBirdFlyAnim, NULL, NULL, 0);
    Shape_Info_init(&this->actor.shape, 5500, Actor_shadow_circle, 4);
    this->unk_194 = 0;
    this->timer = 0;
    this->rotYStep = 2500;
    this->actor.colChkInfo.mass = 0;
    this->speedTarget = 1.5f;
    this->speedStep = 0.5f;
    this->posYMag = 0.0f;
    this->rotYMag = 0.0f;
    this->posYPhaseStep = 0.0f;
    this->flightDistance = 40.0f;
    this->unk_1BC = 70.0f;
    En_Bird_mode_wait_init(this, this->actor.params);
}

void En_bird_Actor_dt(Actor* thisx, PlayState* play) {
}

void En_Bird_mode_wait_init(EnBird* this, s16 params) {
    f32 frameCount = Si2_anime_end_frame(&gBirdFlyAnim);
    f32 playbackSpeed = this->scaleAnimSpeed ? 0.0f : 1.0f;

    this->timer = get_random_timer(5, 35);
    Skeleton_Info2_init(&this->skelAnime, &gBirdFlyAnim, playbackSpeed, 0.0f, frameCount, ANIMMODE_LOOP, 0.0f);
    En_Bird_set_process(this, En_Bird_mode_wait);
}

void En_Bird_mode_wait(EnBird* this, PlayState* play) {
    this->actor.shape.yOffset += sinf(this->posYPhase) * this->posYMag;
    add_calc(&this->actor.speed, 0.0f, 0.1f, 0.5f, 0.0f);

    if (this->scaleAnimSpeed) {
        this->skelAnime.playSpeed = this->actor.speed * 2.0f;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    this->timer--;

    if (this->timer <= 0) {
        En_Bird_mode_rnd_move_init(this, this->actor.params);
    }
}

void En_Bird_mode_rnd_move_init(EnBird* this, s16 params) {
    this->timer = get_random_timer(20, 45);
    En_Bird_set_process(this, En_Bird_mode_rnd_move);
}

void En_Bird_mode_rnd_move(EnBird* this, PlayState* play) {
    this->actor.shape.yOffset += sinf(this->posYPhase) * this->posYMag;
    add_calc(&this->actor.speed, this->speedTarget, 0.1f, this->speedStep, 0.0f);

    if (this->flightDistance < search_position_distanceXZ(&this->actor.world.pos, &this->actor.home.pos) || this->timer < 4) {
        chase_s3(&this->actor.world.rot.y, search_position_angleY(&this->actor.world.pos, &this->actor.home.pos),
                          this->rotYStep);
    } else {
        this->actor.world.rot.y += (s16)(sinf(this->posYPhase) * this->rotYMag);
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->timer--;
    if (this->timer < 0) {
        En_Bird_mode_wait_init(this, this->actor.params);
    }
}

void En_bird_move(Actor* thisx, PlayState* play) {
    EnBird* this = (EnBird*)thisx;

    this->posYPhase += this->posYPhaseStep;
    this->actionFunc(this, play);
}

void En_bird_display(Actor* thisx, PlayState* play) {
    EnBird* this = (EnBird*)thisx;

    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, NULL);
}
