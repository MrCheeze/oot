/*
 * File: z_bg_bdan_switch.c
 * Overlay: ovl_Bg_Bdan_Switch
 * Description: Switches Inside Lord Jabu-Jabu
 */

#include "z_bg_bdan_switch.h"

#include "ichain.h"
#include "rumble.h"
#include "one_point_cutscene.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_bdan_objects/object_bdan_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Bdan_Switch_actor_ct(Actor* thisx, PlayState* play);
void Bg_Bdan_Switch_actor_dt(Actor* thisx, PlayState* play);
void Bg_Bdan_Switch_actor_move(Actor* thisx, PlayState* play);
void Bg_Bdan_Switch_actor_draw(Actor* thisx, PlayState* play);

void mvSet_off_ride(BgBdanSwitch* this);
void mv_off_ride(BgBdanSwitch* this, PlayState* play);
void mvSet_push_ride(BgBdanSwitch* this);
void mv_push_ride(BgBdanSwitch* this, PlayState* play);
void mvSet_on_ride(BgBdanSwitch* this);
void mv_on_ride(BgBdanSwitch* this, PlayState* play);
void mvSet_pop_ride(BgBdanSwitch* this);
void mv_pop_ride(BgBdanSwitch* this, PlayState* play);
void mvSet_off_pair(BgBdanSwitch* this);
void mv_off_pair(BgBdanSwitch* this, PlayState* play);
void mvSet_push_pair(BgBdanSwitch* this);
void mv_push_pair(BgBdanSwitch* this, PlayState* play);
void mv_push2_pair(BgBdanSwitch* this, PlayState* play);
void mvSet_half_pair(BgBdanSwitch* this);
void mv_half_pair(BgBdanSwitch* this, PlayState* play);
void mvSet_pop_pair(BgBdanSwitch* this);
void mv_pop_pair(BgBdanSwitch* this, PlayState* play);
void mvSet_on_pair(BgBdanSwitch* this);
void mv_on_pair(BgBdanSwitch* this, PlayState* play);
void mvSet_off_shock(BgBdanSwitch* this);
static void mv_off_shock(BgBdanSwitch* this, PlayState* play);
void mvSet_push_shock(BgBdanSwitch* this);
void mv_push_shock(BgBdanSwitch* this, PlayState* play);
void mvSet_on_shock(BgBdanSwitch* this);
static void mv_on_shock(BgBdanSwitch* this, PlayState* play);
void mvSet_pop_shock(BgBdanSwitch* this);
void mv_pop_shock(BgBdanSwitch* this, PlayState* play);

ActorProfile Bg_Bdan_Switch_Profile = {
    /**/ ACTOR_BG_BDAN_SWITCH,
    /**/ ACTORCAT_SWITCH,
    /**/ FLAGS,
    /**/ OBJECT_BDAN_OBJECTS,
    /**/ sizeof(BgBdanSwitch),
    /**/ Bg_Bdan_Switch_actor_ct,
    /**/ Bg_Bdan_Switch_actor_dt,
    /**/ Bg_Bdan_Switch_actor_move,
    /**/ Bg_Bdan_Switch_actor_draw,
};

static ColliderJntSphElementInit ClSphElemDt_bdan_switch[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xEFC1FFFE, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 120, 0 }, 370 }, 100 },
    },
};

static ColliderJntSphInit ClSphDt_bdan_switch = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    ClSphElemDt_bdan_switch,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 1400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1200, ICHAIN_STOP),
};

static Vec3f local_eye = { 0.0f, 140.0f, 0.0f };

static void set_dynaPoly(BgBdanSwitch* this, PlayState* play, CollisionHeader* collision, s32 flag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, flag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_bdan_switch.c", 325,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void set_collision_bdan_switch(BgBdanSwitch* this, PlayState* play) {
    Actor* actor = &this->dyna.actor;
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, actor, &ClSphDt_bdan_switch, this->colliderItems);
}

void set_scale_bdan_switch(BgBdanSwitch* this) {
    if (this->unk_1CC > 0) {
        this->unk_1CC += 0x5DC;
    } else {
        this->unk_1CC += 0xFA0;
    }

    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        case BLUE:
        case YELLOW_HEAVY:
        case YELLOW:
            this->unk_1D4 = ((cos_s(this->unk_1CC) * 0.5f) + (53.000004f / 6.0f)) * 0.012f;
            this->unk_1D0 = ((cos_s(this->unk_1CC) * 0.5f) + 20.5f) * (this->unk_1C8 * 0.0050000004f);
            this->dyna.actor.scale.y = this->unk_1C8 * 0.1f;
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            this->unk_1D4 = ((cos_s(this->unk_1CC) * 0.5f) + (43.0f / 6.0f)) * 0.0075000003f;
            this->unk_1D0 = ((cos_s(this->unk_1CC) * 0.5f) + 20.5f) * (this->unk_1C8 * 0.0050000004f);
            this->dyna.actor.scale.y = this->unk_1C8 * 0.1f;
    }
    this->dyna.actor.shape.yOffset = 1.2f / this->unk_1D0;
}

void Bg_Bdan_Switch_actor_ct(Actor* thisx, PlayState* play) {
    BgBdanSwitch* this = (BgBdanSwitch*)thisx;
    s32 pad;
    s16 type;
    s32 flag;

    type = PARAMS_GET_U(this->dyna.actor.params, 0, 8);
    ValueSet_process(&this->dyna.actor, value_init);
    if (type == YELLOW_TALL_1 || type == YELLOW_TALL_2) {
        this->dyna.actor.scale.z = 0.05f;
        this->dyna.actor.scale.x = 0.05f;
    } else {
        this->dyna.actor.scale.z = 0.1f;
        this->dyna.actor.scale.x = 0.1f;
    }
    this->dyna.actor.scale.y = 0.0f;
    Actor_world_to_eye(&this->dyna.actor, 10.0f);

    switch (type) {
        case BLUE:
        case YELLOW_HEAVY:
        case YELLOW:
            set_dynaPoly(this, play, &gJabuFloorSwitchCol, DYNA_TRANSFORM_POS);
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            set_collision_bdan_switch(this, play);
            this->dyna.actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->dyna.actor.attentionRangeType = ATTENTION_RANGE_4;
            break;
    }

    flag = Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));

    switch (type) {
        case BLUE:
        case YELLOW:
            if (flag) {
                mvSet_on_ride(this);
            } else {
                mvSet_off_ride(this);
            }
            break;
        case YELLOW_HEAVY:
            if (flag) {
                mvSet_on_pair(this);
            } else {
                mvSet_off_pair(this);
            }
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            if (flag) {
                mvSet_on_shock(this);
            } else {
                mvSet_off_shock(this);
            }
            break;
        default:
            PRINTF("不正な ARG_DATA(arg_data 0x%04x)(%s %d)\n", this->dyna.actor.params, "../z_bg_bdan_switch.c", 454);
            Actor_delete(&this->dyna.actor);
            return;
    }
    PRINTF("(巨大魚ダンジョン 専用スイッチ)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void Bg_Bdan_Switch_actor_dt(Actor* thisx, PlayState* play) {
    BgBdanSwitch* this = (BgBdanSwitch*)thisx;

    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        case BLUE:
        case YELLOW_HEAVY:
        case YELLOW:
            DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            ClObjJntSph_dt_nzf(play, &this->collider);
            break;
    }
}

static void set_switchON(BgBdanSwitch* this, PlayState* play) {
    s32 pad;
    s32 type;

    if (!Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
        type = PARAMS_GET_U(this->dyna.actor.params, 0, 8);
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));
        if (type == BLUE || type == YELLOW_TALL_2) {
            makeActorAttentionDemoSE(play, &this->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
        } else {
            makeActorAttentionDemoSE(play, &this->dyna.actor, NA_SE_SY_CORRECT_CHIME);
        }
    }
}

static void set_switchOFF(BgBdanSwitch* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
        Actor_Environment_sw_Off(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));
        if (PARAMS_GET_U(this->dyna.actor.params, 0, 8) == YELLOW_TALL_2) {
            makeActorAttentionDemoSE(play, &this->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
        }
    }
}

void mvSet_off_ride(BgBdanSwitch* this) {
    this->actionFunc = mv_off_ride;
    this->unk_1C8 = 1.0f;
}

void mv_off_ride(BgBdanSwitch* this, PlayState* play) {
    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        case BLUE:
            if (MoveBG_checkSwOnStatus(&this->dyna)) {
                mvSet_push_ride(this);
                set_switchON(this, play);
            }
            break;
        case YELLOW:
            if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
                mvSet_push_ride(this);
                set_switchON(this, play);
            }
            break;
    }
}

void mvSet_push_ride(BgBdanSwitch* this) {
    this->actionFunc = mv_push_ride;
    this->unk_1DA = 0x64;
}

void mv_push_ride(BgBdanSwitch* this, PlayState* play) {
    if ((getAttentionDemoPart() == this->dyna.actor.category) || (this->unk_1DA <= 0)) {
        this->unk_1C8 -= 0.2f;
        if (this->unk_1C8 <= 0.1f) {
            mvSet_on_ride(this);
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 120, 20, 10);
        }
    }
}

void mvSet_on_ride(BgBdanSwitch* this) {
    this->unk_1C8 = 0.1f;
    this->actionFunc = mv_on_ride;
    this->unk_1D8 = 6;
}

void mv_on_ride(BgBdanSwitch* this, PlayState* play) {
    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        case BLUE:
            if (!MoveBG_checkSwOnStatus(&this->dyna)) {
                if (this->unk_1D8 <= 0) {
                    mvSet_pop_ride(this);
                    set_switchOFF(this, play);
                }
            } else {
                this->unk_1D8 = 6;
            }
            break;
        case YELLOW:
            if (!Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
                mvSet_pop_ride(this);
            }
            break;
    }
}

void mvSet_pop_ride(BgBdanSwitch* this) {
    this->actionFunc = mv_pop_ride;
}

void mv_pop_ride(BgBdanSwitch* this, PlayState* play) {
    this->unk_1C8 += 0.2f;
    if (this->unk_1C8 >= 1.0f) {
        mvSet_off_ride(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
    }
}

void mvSet_off_pair(BgBdanSwitch* this) {
    this->actionFunc = mv_off_pair;
    this->unk_1C8 = 1.0f;
}

void mv_off_pair(BgBdanSwitch* this, PlayState* play) {
    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        mvSet_push_pair(this);
    }
}

void mvSet_push_pair(BgBdanSwitch* this) {
    this->actionFunc = mv_push_pair;
}

void mv_push_pair(BgBdanSwitch* this, PlayState* play) {
    this->unk_1C8 -= 0.2f;
    if (this->unk_1C8 <= 0.6f) {
        mvSet_half_pair(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 120, 20, 10);
    }
}

void mvSet_push2_pair(BgBdanSwitch* this) {
    this->actionFunc = mv_push2_pair;
    this->unk_1DA = 0x64;
}

void mv_push2_pair(BgBdanSwitch* this, PlayState* play) {
    if ((getAttentionDemoPart() == this->dyna.actor.category) || (this->unk_1DA <= 0)) {
        this->unk_1C8 -= 0.2f;
        if (this->unk_1C8 <= 0.1f) {
            mvSet_on_pair(this);
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 120, 20, 10);
        }
    }
}

void mvSet_half_pair(BgBdanSwitch* this) {
    this->unk_1C8 = 0.6f;
    this->actionFunc = mv_half_pair;
    this->unk_1D8 = 6;
}

void mv_half_pair(BgBdanSwitch* this, PlayState* play) {
    Actor* heldActor = GET_PLAYER(play)->heldActor;

    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        if (heldActor != NULL && heldActor->id == ACTOR_EN_RU1) {
            if (this->unk_1D8 <= 0) {
                mvSet_push2_pair(this);
                set_switchON(this, play);
            }
        } else {
            this->unk_1D8 = 6;
        }
    } else {
        if (this->unk_1D8 <= 0) {
            mvSet_pop_pair(this);
        }
    }
}

void mvSet_pop_pair(BgBdanSwitch* this) {
    this->actionFunc = mv_pop_pair;
}

void mv_pop_pair(BgBdanSwitch* this, PlayState* play) {
    this->unk_1C8 += 0.2f;
    if (this->unk_1C8 >= 1.0f) {
        mvSet_off_pair(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
    }
}

void mvSet_on_pair(BgBdanSwitch* this) {
    this->unk_1C8 = 0.1f;
    this->actionFunc = mv_on_pair;
}

void mv_on_pair(BgBdanSwitch* this, PlayState* play) {
}

void mvSet_off_shock(BgBdanSwitch* this) {
    this->actionFunc = mv_off_shock;
    this->unk_1C8 = 2.0f;
}

static void mv_off_shock(BgBdanSwitch* this, PlayState* play) {
    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        default:
            return;
        case YELLOW_TALL_1:
            if ((this->collider.base.acFlags & AC_HIT) && this->unk_1D8 <= 0) {
                this->unk_1D8 = 0xA;
                mvSet_push_shock(this);
                set_switchON(this, play);
            }
            break;
        case YELLOW_TALL_2:
            if ((this->collider.base.acFlags & AC_HIT) && !(this->unk_1DC & AC_HIT) && this->unk_1D8 <= 0) {
                this->unk_1D8 = 0xA;
                mvSet_push_shock(this);
                set_switchON(this, play);
            }
            break;
    }
}

void mvSet_push_shock(BgBdanSwitch* this) {
    this->actionFunc = mv_push_shock;
    this->unk_1DA = 0x64;
}

void mv_push_shock(BgBdanSwitch* this, PlayState* play) {
    if ((getAttentionDemoPart() == this->dyna.actor.category) || (this->unk_1DA <= 0)) {
        this->unk_1C8 -= 0.3f;
        if (this->unk_1C8 <= 1.0f) {
            mvSet_on_shock(this);
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void mvSet_on_shock(BgBdanSwitch* this) {
    this->actionFunc = mv_on_shock;
    this->unk_1C8 = 1.0f;
}

static void mv_on_shock(BgBdanSwitch* this, PlayState* play) {
    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        case YELLOW_TALL_1:
            if (!Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
                mvSet_pop_shock(this);
            }
            break;
        case YELLOW_TALL_2:
            if ((this->collider.base.acFlags & AC_HIT) && !(this->unk_1DC & AC_HIT) && (this->unk_1D8 <= 0)) {
                this->unk_1D8 = 0xA;
                mvSet_pop_shock(this);
                set_switchOFF(this, play);
            }
            break;
    }
}

void mvSet_pop_shock(BgBdanSwitch* this) {
    this->actionFunc = mv_pop_shock;
    this->unk_1DA = 0x64;
}

void mv_pop_shock(BgBdanSwitch* this, PlayState* play) {
    if (((PARAMS_GET_U(this->dyna.actor.params, 0, 8) != YELLOW_TALL_2) ||
         (getAttentionDemoPart() == this->dyna.actor.category)) ||
        (this->unk_1DA <= 0)) {
        this->unk_1C8 += 0.3f;
        if (this->unk_1C8 >= 2.0f) {
            mvSet_off_shock(this);
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void Bg_Bdan_Switch_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgBdanSwitch* this = (BgBdanSwitch*)thisx;
    s32 type;

    if (this->unk_1DA > 0) {
        this->unk_1DA--;
    }
    this->actionFunc(this, play);
    set_scale_bdan_switch(this);
    type = PARAMS_GET_U(this->dyna.actor.params, 0, 8);
    if (type != 3 && type != 4) {
        this->unk_1D8--;
    } else {
        if (!player_demo_check(play) && this->unk_1D8 > 0) {
            this->unk_1D8--;
        }
        this->unk_1DC = this->collider.base.acFlags;
        this->collider.base.acFlags &= ~AC_HIT;
        this->collider.elements[0].dim.modelSphere.radius = this->unk_1D4 * 370.0f;
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void draw_bdan_switch_sub(BgBdanSwitch* this, PlayState* play, Gfx* dlist) {
    Matrix_softcv3_load(this->dyna.actor.world.pos.x,
                                 this->dyna.actor.world.pos.y + (this->dyna.actor.shape.yOffset * this->unk_1D0),
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    Matrix_scale(this->unk_1D4, this->unk_1D0, this->unk_1D4, MTXMODE_APPLY);
    Cheap_gfx_display(play, dlist);
}

void Bg_Bdan_Switch_actor_draw(Actor* thisx, PlayState* play) {
    BgBdanSwitch* this = (BgBdanSwitch*)thisx;

    switch (PARAMS_GET_U(this->dyna.actor.params, 0, 8)) {
        case YELLOW_HEAVY:
        case YELLOW:
            draw_bdan_switch_sub(this, play, gJabuYellowFloorSwitchDL);
            break;
        case YELLOW_TALL_1:
        case YELLOW_TALL_2:
            draw_bdan_switch_sub(this, play, gJabuYellowFloorSwitchDL);
            CollisionCheck_Uty_convJntSphL2G(0, &this->collider);
            Matrix_Position(&local_eye, &this->dyna.actor.focus.pos);
            break;
        case BLUE:
            draw_bdan_switch_sub(this, play, gJabuBlueFloorSwitchDL);
            break;
    }
}
