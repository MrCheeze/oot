/*
 * File: z_bg_relay_objects.c
 * Overlay: ovl_Bg_Relay_Objects
 * Description: Windmill Setpieces
 */

#include "z_bg_relay_objects.h"
#include "assets/objects/object_relay_objects/object_relay_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef enum WindmillSetpiecesMode {
    /* 0 */ WINDMILL_ROTATING_GEAR,
    /* 1 */ WINDMILL_DAMPE_STONE_DOOR
} WindmillSetpiecesMode;

void Bg_Relay_Objects_actor_ct(Actor* thisx, PlayState* play);
void Bg_Relay_Objects_actor_dt(Actor* thisx, PlayState* play);
void Bg_Relay_Objects_actor_move(Actor* thisx, PlayState* play);
void Bg_Relay_Objects_actor_draw(Actor* thisx, PlayState* play);

static void mode_wall_wait(BgRelayObjects* this, PlayState* play);
void mode_wall_cnt(BgRelayObjects* this, PlayState* play);
void mode_wall_drop(BgRelayObjects* this, PlayState* play);
static void mode_wall_stop(BgRelayObjects* this, PlayState* play);
void mode_wipe_wait(BgRelayObjects* this, PlayState* play);
void mode_usu(BgRelayObjects* this, PlayState* play);

ActorProfile Bg_Relay_Objects_Profile = {
    /**/ ACTOR_BG_RELAY_OBJECTS,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_RELAY_OBJECTS,
    /**/ sizeof(BgRelayObjects),
    /**/ Bg_Relay_Objects_actor_ct,
    /**/ Bg_Relay_Objects_actor_dt,
    /**/ Bg_Relay_Objects_actor_move,
    /**/ Bg_Relay_Objects_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(gravity, 5, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Relay_Objects_actor_ct(Actor* thisx, PlayState* play) {
    static u32 set_flg = 0;
    BgRelayObjects* this = (BgRelayObjects*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(thisx, value_init);
    this->switchFlag = PARAMS_GET_U(thisx->params, 0, 6);
    thisx->params = PARAMS_GET_U(thisx->params, 8, 8);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
    if (thisx->params == WINDMILL_ROTATING_GEAR) {
        DynaPolyUty_bgdi_SG2KSG(&gWindmillRotatingPlatformCol, &colHeader);
        if (GET_EVENTCHKINF(EVENTCHKINF_65)) {
            thisx->world.rot.y = 0x400;
        } else {
            thisx->world.rot.y = 0x80;
        }
        Na_StartWindmillBgm();
        thisx->room = -1;
        thisx->flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
        if (set_flg & 2) {
            thisx->params = 0xFF;
            Actor_delete(thisx);
        } else {
            set_flg |= 2;
            this->actionFunc = mode_usu;
        }
    } else {
        DynaPolyUty_bgdi_SG2KSG(&gDampeRaceDoorCol, &colHeader);
        if (thisx->room == 0) {
            this->unk_169 = this->switchFlag - 0x33;
        } else {
            this->unk_169 = thisx->room + 1;
        }
        thisx->room = -1;
        this->timer = 1;
        if (this->unk_169 >= 6) {
            if (set_flg & 1) {
                Actor_delete(thisx);
            } else {
                set_flg |= 1;
                this->actionFunc = mode_wall_stop;
            }
        } else if (this->unk_169 != 5) {
            Actor_Environment_sw_Off(play, this->switchFlag);
            if (set_flg & (1 << this->unk_169)) {
                Actor_delete(thisx);
            } else {
                set_flg |= (1 << this->unk_169);
                this->actionFunc = mode_wall_wait;
            }
        } else {
            Actor_Environment_sw_On(play, this->switchFlag);
            this->actionFunc = mode_wall_cnt;
            thisx->world.pos.y += 120.0f;
            set_flg |= 1;
        }
    }
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
}

void Bg_Relay_Objects_actor_dt(Actor* thisx, PlayState* play) {
    BgRelayObjects* this = (BgRelayObjects*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    if ((this->dyna.actor.params == WINDMILL_ROTATING_GEAR) && (z_common_data.save.cutsceneIndex < 0xFFF0)) {
        CLEAR_EVENTCHKINF(EVENTCHKINF_65);
    }
}

static void mode_wall_wait(BgRelayObjects* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, this->switchFlag)) {
        if (this->timer != 0) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_SLIDE_DOOR_OPEN);
            if (INV_CONTENT(ITEM_HOOKSHOT) != ITEM_NONE) {
                this->timer = 120;
            } else {
                this->timer = 160;
            }
        }
        if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 120.0f, 12.0f)) {
            this->actionFunc = mode_wall_cnt;
        }
    }
}

void mode_wall_cnt(BgRelayObjects* this, PlayState* play) {
    if (this->unk_169 != 5) {
        if (this->timer != 0) {
            this->timer--;
        }
        Actor_timer_level_SE_set(&this->dyna.actor, this->timer);
    }
    if ((this->timer == 0) || (this->unk_169 == play->roomCtx.curRoom.num)) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_SLIDE_DOOR_CLOSE);
        this->actionFunc = mode_wall_drop;
    }
}

void mode_wall_drop(BgRelayObjects* this, PlayState* play) {
    this->dyna.actor.velocity.y += this->dyna.actor.gravity;
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, this->dyna.actor.velocity.y)) {
        z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 180, 20, 100);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_STONE_BOUND);
        if (this->unk_169 != play->roomCtx.curRoom.num) {
            Na_StartFixSe_F(NA_SE_EN_PO_LAUGH);
            this->timer = 5;
            this->actionFunc = mode_wipe_wait;
            return;
        }
        Actor_Environment_sw_Off(play, this->switchFlag);
        this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        if (play->roomCtx.curRoom.num == 4) {
            z_common_data.timerState = TIMER_STATE_UP_FREEZE;
        }
        this->actionFunc = mode_wall_stop;
    }
}

static void mode_wall_stop(BgRelayObjects* this, PlayState* play) {
}

void mode_wipe_wait(BgRelayObjects* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        if (!player_demo_check(play)) {
            Na_StartSystemSe_F(NA_SE_OC_ABYSS);
            Game_play_down_restart_top(play);
            this->actionFunc = mode_wall_stop;
        }
    }
}

void mode_usu(BgRelayObjects* this, PlayState* play) {
    if (eventbit_check(play, 5)) {
        SET_EVENTCHKINF(EVENTCHKINF_65);
    }
    if (GET_EVENTCHKINF(EVENTCHKINF_65)) {
        chase_angle(&this->dyna.actor.world.rot.y, 0x400, 8);
    } else {
        chase_angle(&this->dyna.actor.world.rot.y, 0x80, 8);
    }
    this->dyna.actor.shape.rot.y += this->dyna.actor.world.rot.y;
    Na_SetMotorSe(&this->dyna.actor.projectedPos, NA_SE_EV_WOOD_GEAR - SFX_FLAG,
                  ((this->dyna.actor.world.rot.y - 0x80) * (1.0f / 0x380)) + 1.0f);
}

void Bg_Relay_Objects_actor_move(Actor* thisx, PlayState* play) {
    BgRelayObjects* this = (BgRelayObjects*)thisx;

    this->actionFunc(this, play);
}

void Bg_Relay_Objects_actor_draw(Actor* thisx, PlayState* play) {
    BgRelayObjects* this = (BgRelayObjects*)thisx;

    if (this->dyna.actor.params == WINDMILL_ROTATING_GEAR) {
        Cheap_gfx_display(play, gWindmillRotatingPlatformDL);
    } else {
        Cheap_gfx_display(play, gDampeRaceDoorDL);
    }
}
