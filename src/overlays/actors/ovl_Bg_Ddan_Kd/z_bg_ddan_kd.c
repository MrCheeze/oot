/*
 * File: z_bg_ddan_kd.c
 * Overlay: ovl_Bg_Ddan_Kd
 * Description: Falling stairs in Dodongo's Cavern
 */

#include "z_bg_ddan_kd.h"

#include "libc64/qrand.h"
#include "ichain.h"
#include "one_point_cutscene.h"
#include "rand.h"
#include "rumble.h"
#include "sfx.h"
#include "z_lib.h"
#include "z64play.h"

#include "assets/objects/object_ddan_objects/object_ddan_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Ddan_Kd_actor_ct(Actor* thisx, PlayState* play);
void Bg_Ddan_Kd_actor_dt(Actor* thisx, PlayState* play);
void Bg_Ddan_Kd_actor_move(Actor* thisx, PlayState* play);
void Bg_Ddan_Kd_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgDdanKd* this, PlayState* play);
static void mode_down(BgDdanKd* this, PlayState* play);
static void mode_stop(BgDdanKd* this, PlayState* play);

ActorProfile Bg_Ddan_Kd_Profile = {
    /**/ ACTOR_BG_DDAN_KD,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_DDAN_OBJECTS,
    /**/ sizeof(BgDdanKd),
    /**/ Bg_Ddan_Kd_actor_ct,
    /**/ Bg_Ddan_Kd_actor_dt,
    /**/ Bg_Ddan_Kd_actor_move,
    /**/ Bg_Ddan_Kd_actor_draw,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ALL,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 245, 180, -400, { 0, 0, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 32767, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 32767, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 32767, ICHAIN_STOP),
};

void Bg_Ddan_Kd_actor_set_process(BgDdanKd* this, BgDdanKdActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Bg_Ddan_Kd_actor_ct(Actor* thisx, PlayState* play) {
    BgDdanKd* this = (BgDdanKd*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    this->prevExplosive = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->dyna.actor, &OcInfoData);
    DynaPolyUty_bgdi_SG2KSG(&gDodongoFallingStairsCol, &colHeader);

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (!Actor_Environment_sw_Check(play, this->dyna.actor.params)) {
        Bg_Ddan_Kd_actor_set_process(this, mode_wait);
    } else {
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 200.0f - 20.0f;
        Bg_Ddan_Kd_actor_set_process(this, mode_stop);
    }
}

void Bg_Ddan_Kd_actor_dt(Actor* thisx, PlayState* play) {
    BgDdanKd* this = (BgDdanKd*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjPipe_dt(play, &this->collider);
}

static void mode_wait(BgDdanKd* this, PlayState* play) {
    Actor* explosive;

    explosive = BlastVsMyCheck(play, &this->collider.base);
    if (explosive != NULL) {
        PRINTF("dam    %d\n", this->dyna.actor.colChkInfo.damage);
        explosive->params = 2;
    }

    if ((explosive != NULL) && (this->prevExplosive != NULL) && (explosive != this->prevExplosive) &&
        (search_position_distanceXZ(&this->prevExplosivePos, &explosive->world.pos) > 80.0f)) {
        Bg_Ddan_Kd_actor_set_process(this, mode_down);
        makeOnepointDemo(play, 3050, 999, &this->dyna.actor, CAM_ID_MAIN);
    } else {
        if (this->timer != 0) {
            this->timer--;
        } else {
            this->prevExplosive = explosive;
            if (explosive != NULL) {
                this->timer = 13;
                this->prevExplosivePos = explosive->world.pos;
            }
        }
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->collider);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

static void mode_down(BgDdanKd* this, PlayState* play) {
    static Vec3f s_vec = { 0.0f, 5.0f, 0.0f };
    static Vec3f s_acc = { 0.0f, -0.45f, 0.0f };
    Vec3f pos1;
    Vec3f pos2;
    f32 effectStrength;

    add_calc(&this->dyna.actor.speed, 4.0f, 0.5f, 0.025f, 0.0f);
    z_vibctl2_vib_setQ(500.0f, 120, 20, 10);

    if (add_calc(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 200.0f - 20.0f, 0.075f,
                           this->dyna.actor.speed, 0.0075f) == 0.0f) {
        Actor_Environment_sw_On(play, this->dyna.actor.params);
        Bg_Ddan_Kd_actor_set_process(this, mode_stop);
    } else {
        effectStrength = (this->dyna.actor.prevPos.y - this->dyna.actor.world.pos.y) + (this->dyna.actor.speed * 0.25f);

        if (play->state.frames & 1) {
            pos1 = pos2 = this->dyna.actor.world.pos;

            if (play->state.frames & 2) {
                pos1.z += 210.0f + fqrand() * 230.0f;
                pos2.z += 210.0f + fqrand() * 230.0f;
            } else {
                pos1.z += 330.0f + fqrand() * 240.0f;
                pos2.z += 330.0f + fqrand() * 240.0f;
            }
            pos1.x += 80.0f + fqrand() * 10.0f;
            pos2.x -= 80.0f + fqrand() * 10.0f;
            pos1.y = this->dyna.actor.floorHeight + 20.0f + fqrand();
            pos2.y = this->dyna.actor.floorHeight + 20.0f + fqrand();

            dust_fly_set2(play, &pos1, 20.0f, 1, effectStrength * 135.0f, 60, 1);
            dust_fly_set2(play, &pos2, 20.0f, 1, effectStrength * 135.0f, 60, 1);

            s_vec.x = rnd_fx(3.0f);
            s_vec.z = rnd_fx(3.0f);

            suna_set0(play, &pos1, &s_vec, &s_acc);
            suna_set0(play, &pos2, &s_vec, &s_acc);

            pos1 = this->dyna.actor.world.pos;
            pos1.z += 560.0f + fqrand() * 5.0f;
            pos1.x += (fqrand() - 0.5f) * 160.0f;
            pos1.y = fqrand() * 3.0f + (this->dyna.actor.floorHeight + 20.0f);

            dust_fly_set2(play, &pos1, 20.0f, 1, effectStrength * 135.0f, 60, 1);
            suna_set0(play, &pos1, &s_vec, &s_acc);
        }
        setDamageCamera(&play->mainCamera, 0, effectStrength * 0.6f, 3);
        Nai_FxFlagEntry(NA_SE_EV_PILLAR_SINK - SFX_FLAG, &this->dyna.actor.projectedPos, 4,
                             &_dummy_one, &_dummy_one, &_dummy_zero_s8);
    }
}

static void mode_stop(BgDdanKd* this, PlayState* play) {
}

void Bg_Ddan_Kd_actor_move(Actor* thisx, PlayState* play) {
    BgDdanKd* this = (BgDdanKd*)thisx;

    this->actionFunc(this, play);
}

void Bg_Ddan_Kd_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gDodongoFallingStairsDL);
}
