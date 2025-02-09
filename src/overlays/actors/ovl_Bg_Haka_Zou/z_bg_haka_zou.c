/*
 * File: z_bg_haka_zou.c
 * Overlay: ovl_Bg_Haka_Zou
 * Description: Statue and Wall (Shadow Temple)
 */

#include "z_bg_haka_zou.h"

#include "libc64/qrand.h"
#include "ichain.h"
#include "one_point_cutscene.h"
#include "rand.h"
#include "sfx.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"

#include "assets/objects/object_hakach_objects/object_hakach_objects.h"
#include "assets/objects/object_haka_objects/object_haka_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef enum ShadowTempleAssetsType {
    /* 0x0 */ STA_GIANT_BIRD_STATUE,
    /* 0x1 */ STA_BOMBABLE_SKULL_WALL,
    /* 0x2 */ STA_BOMBABLE_RUBBLE,
    /* 0x3 */ STA_UNKNOWN
} ShadowTempleAssetsType;

void Bg_Haka_Zou_actor_ct(Actor* thisx, PlayState* play);
void Bg_Haka_Zou_actor_dt(Actor* thisx, PlayState* play);
void Bg_Haka_Zou_actor_move(Actor* thisx, PlayState* play);
void Bg_Haka_Zou_actor_draw(Actor* thisx, PlayState* play);

static void mode_dma_wait(BgHakaZou* this, PlayState* play);
void mode_hahen(BgHakaZou* this, PlayState* play);
static void mode_wait(BgHakaZou* this, PlayState* play);
static void mode_delete(BgHakaZou* this, PlayState* play);
static void mode_ready(BgHakaZou* this, PlayState* play);
static void mode_zure(BgHakaZou* this, PlayState* play);
static void mode_down(BgHakaZou* this, PlayState* play);
static void mode_yure(BgHakaZou* this, PlayState* play);
static void mode_stop(BgHakaZou* this, PlayState* play);

static ColliderCylinderInit HakaZouAcPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 5, 60, 0, { 0, 0, 0 } },
};

static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };

ActorProfile Bg_Haka_Zou_Profile = {
    /**/ ACTOR_BG_HAKA_ZOU,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(BgHakaZou),
    /**/ Bg_Haka_Zou_actor_ct,
    /**/ Bg_Haka_Zou_actor_dt,
    /**/ Bg_Haka_Zou_actor_move,
    /**/ NULL,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Haka_Zou_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakaZou* this = (BgHakaZou*)thisx;

    ValueSet_process(thisx, value_init);

    this->switchFlag = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params &= 0xFF;

    if (thisx->params == STA_UNKNOWN) {
        Actor_set_scale(thisx, (fqrand() * 0.005f) + 0.025f);

        thisx->speed = fqrand();
        thisx->world.rot.y = thisx->shape.rot.y * ((fqrand() < 0.5f) ? -1 : 1) + rnd_fx(0x1000);
        this->timer = 20;
        thisx->world.rot.x = get_random_timer(0x100, 0x300) * ((fqrand() < 0.5f) ? -1 : 1);
        thisx->world.rot.z = get_random_timer(0x400, 0x800) * ((fqrand() < 0.5f) ? -1 : 1);
    } else {
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, thisx, &HakaZouAcPipeData);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider);

        MoveBG_ct(&this->dyna, 0);

        if (thisx->params == STA_GIANT_BIRD_STATUE) {
            thisx->cullingVolumeDistance = 2000.0f;
            thisx->cullingVolumeScale = 3000.0f;
            thisx->cullingVolumeDownward = 3000.0f;
        }
    }

    this->requiredObjectSlot = (thisx->params == STA_BOMBABLE_RUBBLE)
                                   ? Object_Exchange_bank_check(&play->objectCtx, OBJECT_HAKACH_OBJECTS)
                                   : Object_Exchange_bank_check(&play->objectCtx, OBJECT_HAKA_OBJECTS);

    if (this->requiredObjectSlot < 0) {
        Actor_delete(thisx);
    } else if ((thisx->params != STA_UNKNOWN) && Actor_Environment_sw_Check(play, this->switchFlag)) {
        if (thisx->params != STA_GIANT_BIRD_STATUE) {
            Actor_delete(thisx);
        } else {
            thisx->shape.rot.x = -0x4000;
            thisx->world.pos.z -= 80.0f;
            thisx->world.pos.y -= 54.0f;
        }
    }

    this->actionFunc = mode_dma_wait;
}

void Bg_Haka_Zou_actor_dt(Actor* thisx, PlayState* play) {
    BgHakaZou* this = (BgHakaZou*)thisx;

    if (this->dyna.actor.params != STA_UNKNOWN) {
        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
        ClObjPipe_dt(play, &this->collider);
    }
}

void set_zou_dust(BgHakaZou* this, PlayState* play) {
    Vec3f effectPos;
    Vec3f effectVelocity;
    f32 rand;
    s32 i;

    effectVelocity.x = 0.0f;
    effectVelocity.y = 1.0f;
    effectVelocity.z = 2.0f;

    for (i = 0; i < 2; i++) {
        if (i == 0) {
            effectPos.x = this->dyna.actor.world.pos.x - (rnd_fx(10.0f) + 112.0f);
        } else {
            effectPos.x = rnd_fx(10.0f) + this->dyna.actor.world.pos.x;
        }

        rand = fqrand();
        effectPos.y = this->dyna.actor.world.pos.y + (60.0f * rand);
        effectPos.z = this->dyna.actor.world.pos.z + (112.0f * rand);

        Effect_SS_Dust_sc_li_ct(play, &effectPos, &effectVelocity, &zero_vec, (fqrand() * 200.0f) + 1000.0f, 100);
    }
}

static void mode_dma_wait(BgHakaZou* this, PlayState* play) {
    CollisionHeader* colHeader;

    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->dyna.actor.objectSlot = this->requiredObjectSlot;
        this->dyna.actor.draw = Bg_Haka_Zou_actor_draw;

        if (this->dyna.actor.params == STA_UNKNOWN) {
            this->actionFunc = mode_hahen;
        } else {
            Actor_set_segment(play, &this->dyna.actor);

            colHeader = NULL;

            if (this->dyna.actor.params == STA_GIANT_BIRD_STATUE) {
                DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_006F70, &colHeader);
                this->collider.dim.radius = 80;
                this->collider.dim.height = 100;
                this->collider.dim.yShift = -30;
                this->collider.dim.pos.x -= 56;
                this->collider.dim.pos.z += 56;
                this->dyna.actor.cullingVolumeScale = 1500.0f;
            } else if (this->dyna.actor.params == STA_BOMBABLE_SKULL_WALL) {
                DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_005E30, &colHeader);
                this->collider.dim.yShift = -50;
            } else {
                DynaPolyUty_bgdi_SG2KSG(&gBotwBombSpotCol, &colHeader);
                this->collider.dim.radius = 55;
                this->collider.dim.height = 20;
            }

            this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

            if ((this->dyna.actor.params == STA_GIANT_BIRD_STATUE) && Actor_Environment_sw_Check(play, this->switchFlag)) {
                this->actionFunc = mode_stop;
            } else {
                this->actionFunc = mode_wait;
            }
        }
    }
}
void mode_hahen(BgHakaZou* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    this->dyna.actor.shape.rot.x += this->dyna.actor.world.rot.x;
    this->dyna.actor.shape.rot.z += this->dyna.actor.world.rot.z;

    if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        if (this->dyna.actor.velocity.y < -8.0f) {
            this->dyna.actor.velocity.y *= -0.6f;
            this->dyna.actor.velocity.y = CLAMP_MAX(this->dyna.actor.velocity.y, 10.0f);
            this->dyna.actor.bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH);
            this->dyna.actor.speed = 2.0f;
        } else {
            Actor_delete(&this->dyna.actor);
        }
    }

    if (this->timer == 0) {
        Actor_delete(&this->dyna.actor);
    }
}

void haka_zou_set_hahen_actor(BgHakaZou* this, PlayState* play) {
    s32 i;
    s32 j;
    Vec3f actorSpawnPos;
    f32 sin;
    f32 cos;
    s32 pad;

    sin = sin_s(this->dyna.actor.shape.rot.y - 0x4000) * 40.0f;
    cos = cos_s(this->dyna.actor.shape.rot.y - 0x4000) * 40.0f;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            actorSpawnPos.x = this->dyna.actor.world.pos.x + (j - 1) * sin;
            actorSpawnPos.z = this->dyna.actor.world.pos.z + (j - 1) * cos;
            actorSpawnPos.y = this->dyna.actor.world.pos.y + (i - 1) * 55;

            Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_HAKA_ZOU, actorSpawnPos.x, actorSpawnPos.y, actorSpawnPos.z, 0,
                        this->dyna.actor.shape.rot.y, 0, this->dyna.actor.params + 2);
            Effect_SS_Dust_sc_li_ct(play, &actorSpawnPos, &zero_vec, &zero_vec, 1000, 50);
        }
    }
}

void haka_zou_set_hahen_effect(BgHakaZou* this, PlayState* play) {
    Vec3f fragmentPos;
    s32 i;
    s32 j;
    s32 num = 25;

    fragmentPos.x = this->collider.dim.pos.x;
    fragmentPos.y = this->collider.dim.pos.y;
    fragmentPos.z = this->collider.dim.pos.z;

    Effect_Hahen_Kakusan_ct3(play, &fragmentPos, 10.0f, 0, 10, 10, 4, 141, 40, gBotwBombSpotDL);

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            fragmentPos.x = this->collider.dim.pos.x + (((j * 2) - 1) * num);
            fragmentPos.z = this->collider.dim.pos.z + (((i * 2) - 1) * num);
            Effect_Hahen_Kakusan_ct3(play, &fragmentPos, 10.0f, 0, 10, 10, 4, 141, 40, gBotwBombSpotDL);
            Effect_SS_Dust_sc_li_ct(play, &fragmentPos, &zero_vec, &zero_vec, 1000, 50);
        }
    }
}

static void mode_wait(BgHakaZou* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        Actor_Environment_sw_On(play, this->switchFlag);

        if (this->dyna.actor.params == STA_GIANT_BIRD_STATUE) {
            this->timer = 20;
            this->actionFunc = mode_ready;
            makeOnepointDemo(play, 3400, 999, &this->dyna.actor, CAM_ID_MAIN);
        } else if (this->dyna.actor.params == 2) {
            haka_zou_set_hahen_effect(this, play);
            this->dyna.actor.draw = NULL;
            this->timer = 1;
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_EXPLOSION);
            this->actionFunc = mode_delete;
        } else {
            haka_zou_set_hahen_actor(this, play);
            this->timer = 1;
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_WALL_BROKEN);
            this->actionFunc = mode_delete;
        }
    } else {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

static void mode_delete(BgHakaZou* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        Actor_delete(&this->dyna.actor);
    }
}

static void mode_ready(BgHakaZou* this, PlayState* play) {
    Vec3f explosionPos;

    if (this->timer != 0) {
        this->timer--;
    }

    if (!(this->timer % 4)) {
        explosionPos.x = rnd_fx(200.0f) + (this->dyna.actor.world.pos.x - 56.0f);
        explosionPos.y = (fqrand() * 80.0f) + this->dyna.actor.world.pos.y;
        explosionPos.z = rnd_fx(200.0f) + (this->dyna.actor.world.pos.z + 56.0f);

        Effect_SS_Bomb2_2_ct(play, &explosionPos, &zero_vec, &zero_vec, 150, 70);
        Actor_SE_set(&this->dyna.actor, NA_SE_IT_BOMB_EXPLOSION);
    }

    if (this->timer == 0) {
        this->timer = 20;
        this->actionFunc = mode_zure;
    }
}

static void mode_zure(BgHakaZou* this, PlayState* play) {
    f32 moveDist = (fqrand() * 0.5f) + 0.5f;

    chase_f(&this->dyna.actor.world.pos.z, this->dyna.actor.home.pos.z - 80.0f, 2.0f * moveDist);

    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 40.0f, moveDist)) {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            this->timer = 60;
            this->dyna.actor.world.rot.x = 8;
            this->actionFunc = mode_down;
        }
    } else {
        set_zou_dust(this, play);
    }
}

static void mode_down(BgHakaZou* this, PlayState* play) {
    Vec3f effectPos;
    s32 i;
    s32 j;

    this->dyna.actor.world.rot.x += this->dyna.actor.world.rot.x / 8.0f;

    if (chase_angle(&this->dyna.actor.shape.rot.x, -0x4000, this->dyna.actor.world.rot.x)) {
        effectPos.x = this->dyna.actor.world.pos.x;
        effectPos.y = this->dyna.actor.world.pos.y;

        for (j = 0; j < 2; j++) {
            effectPos.z = this->dyna.actor.world.pos.z;

            for (i = 0; i < 4; i++) {
                effectPos.z -= (i == 2) ? 550.0f : 50.0f;
                Effect_SS_Dust_sc_li_ct(play, &effectPos, &zero_vec, &zero_vec, (fqrand() * 200.0f) + 1000.0f, 200);
            }

            effectPos.x -= 112.0f;
        }

        Actor_SE_set(&this->dyna.actor, NA_SE_EV_STONE_BOUND);
        this->timer = 25;
        this->actionFunc = mode_yure;
    }
}

static void mode_yure(BgHakaZou* this, PlayState* play) {
    f32 moveDist;

    if (this->timer != 0) {
        this->timer--;
    }

    moveDist = (this->timer % 2) ? 15.0f : -15.0f;
    this->dyna.actor.world.pos.y += ((this->timer & 0xFE) * 0.04f * moveDist);

    if (this->timer == 0) {
        this->actionFunc = mode_stop;
    }
}

static void mode_stop(BgHakaZou* this, PlayState* play) {
}

void Bg_Haka_Zou_actor_move(Actor* thisx, PlayState* play) {
    BgHakaZou* this = (BgHakaZou*)thisx;

    this->actionFunc(this, play);

    if (this->dyna.actor.params == 3) {
        Actor_position_moveF(&this->dyna.actor);
    }
}

void Bg_Haka_Zou_actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* shape_model[] = {
        object_haka_objects_DL_0064E0,
        object_haka_objects_DL_005CE0,
        gBotwBombSpotDL,
        object_haka_objects_DL_005CE0,
    };

    Cheap_gfx_display(play, shape_model[thisx->params]);
}
