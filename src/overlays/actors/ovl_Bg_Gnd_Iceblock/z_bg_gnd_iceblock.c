/*
 * File: z_bg_gnd_iceblock.c
 * Overlay: ovl_Bg_Gnd_Iceblock
 * Description: Pushable ice block (Inside Ganon's Castle)
 */

#include "z_bg_gnd_iceblock.h"

#include "libc64/qrand.h"
#include "libu64/debug.h"
#include "ichain.h"
#include "rand.h"
#include "sfx.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_demo_kekkai/object_demo_kekkai.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

typedef enum BgGndIceblockAction {
    /* 0 */ GNDICE_IDLE,
    /* 1 */ GNDICE_FALL,
    /* 2 */ GNDICE_HOLE
} BgGndIceblockAction;

void Bg_Gnd_Iceblock_Actor_ct(Actor* thisx, PlayState* play);
void Bg_Gnd_Iceblock_Actor_dt(Actor* thisx, PlayState* play);
void Bg_Gnd_Iceblock_Actor_move(Actor* thisx, PlayState* play);
void Bg_Gnd_Iceblock_Actor_draw(Actor* thisx, PlayState* play);

static void mode_block_wait(BgGndIceblock* this, PlayState* play);
static void mode_block_move(BgGndIceblock* this, PlayState* play);

ActorProfile Bg_Gnd_Iceblock_Profile = {
    /**/ ACTOR_BG_GND_ICEBLOCK,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_DEMO_KEKKAI,
    /**/ sizeof(BgGndIceblock),
    /**/ Bg_Gnd_Iceblock_Actor_ct,
    /**/ Bg_Gnd_Iceblock_Actor_dt,
    /**/ Bg_Gnd_Iceblock_Actor_move,
    /**/ Bg_Gnd_Iceblock_Actor_draw,
};

static Color_RGBA8 prim = { 250, 250, 250, 255 };
static Color_RGBA8 env = { 180, 180, 180, 255 };
static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };
static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static u8 pos_id[2];

void Bg_Gnd_Iceblock_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndIceblock* this = (BgGndIceblock*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gWaterTrialIceBlockCol, &colHeader);
    this->targetPos = this->dyna.actor.home.pos;
    this->actionFunc = mode_block_wait;
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if (this->dyna.actor.world.pos.x == 2792.0f) {
        this->dyna.actor.params = 0;
        pos_id[0] = 7;
    } else if (this->dyna.actor.world.pos.x == 3032.0f) {
        this->dyna.actor.params = 1;
        pos_id[1] = 14;
    } else {
        LOG_FLOAT("thisx->world.position.x", this->dyna.actor.world.pos.x, "../z_bg_gnd_iceblock.c", 138);
        ASSERT(0, "0", "../z_bg_gnd_iceblock.c", 139);
    }
}

void Bg_Gnd_Iceblock_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndIceblock* this = (BgGndIceblock*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

/*
 * Diagram of positions in the room:
 *            __
 *  _________|10|_________
 * |*0*     6   *13****16*|
 * |*1*     7          17 |
 * | 2           14    18 |
 * | 3     h8    15    19 |
 * | 4      9 11 XX   *20*|
 * |*5*    XX 12      *21*|
 *  ----------------------
 * XX are rocks
 * ** are pits
 * h is the hole.
 * Block 0 starts at 7 and block 1 starts at 14
 */

void set_aim_pos(BgGndIceblock* this, s32 blockPosition) {
    Actor* thisx = &this->dyna.actor;
    u8 xPosIdx[22] = {
        0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 6, 6, 6, 6, 6, 6,
    };
    u8 zPosIdx[22] = {
        5, 4, 3, 2, 1, 0, 5, 4, 2, 1, 6, 1, 0, 5, 3, 2, 5, 4, 3, 2, 1, 0,
    };

    pos_id[thisx->params] = blockPosition;
    this->targetPos.x = 2552.0f + (xPosIdx[blockPosition] * 120.0f);
    this->targetPos.z = -540.0f - (zPosIdx[blockPosition] * 120.0f);
}

s32 check_is_block(s32 blockPosition) {
    s32 i;

    for (i = 0; i < 2; i++) {
        if (blockPosition == pos_id[i]) {
            return true;
        }
    }
    return false;
}

static s32 ice_block_set_y(BgGndIceblock* this) {
    switch (pos_id[this->dyna.actor.params]) {
        case 0:
        case 1:
        case 5:
        case 13:
        case 16:
        case 20:
        case 21:
            return GNDICE_FALL;
        case 8:
            return GNDICE_HOLE;
        default:
            return GNDICE_IDLE;
    }
}

static void ice_block_check_move_distance(BgGndIceblock* this) {
    if (this->dyna.unk_158 == 0) {
        switch (pos_id[this->dyna.actor.params]) {
            case 3:
            case 4:
                set_aim_pos(this, 5);
                break;
            case 7:
                if (check_is_block(8)) {
                    set_aim_pos(this, 9);
                } else {
                    set_aim_pos(this, 8);
                }
                break;
            case 11:
                set_aim_pos(this, 12);
                break;
            case 14:
                set_aim_pos(this, 15);
                break;
            case 18:
            case 19:
                set_aim_pos(this, 20);
                break;
        }
    } else if (this->dyna.unk_158 == -0x8000) {
        switch (pos_id[this->dyna.actor.params]) {
            case 2:
            case 3:
                set_aim_pos(this, 1);
                break;
            case 7:
            case 9:
                set_aim_pos(this, 6);
                break;
            case 11:
                set_aim_pos(this, 10);
                break;
            case 14:
            case 15:
                set_aim_pos(this, 13);
                break;
            case 17:
                set_aim_pos(this, 16);
                break;
            case 18:
                if (!check_is_block(17)) {
                    set_aim_pos(this, 16);
                }
                break;
        }
    } else if (this->dyna.unk_158 == 0x4000) {
        switch (pos_id[this->dyna.actor.params]) {
            case 6:
                set_aim_pos(this, 13);
                break;
            case 7:
                set_aim_pos(this, 17);
                break;
            case 9:
                set_aim_pos(this, 11);
                break;
            case 12:
                set_aim_pos(this, 21);
                break;
            case 14:
                set_aim_pos(this, 18);
                break;
            case 15:
                set_aim_pos(this, 19);
                break;
        }
    } else if (this->dyna.unk_158 == -0x4000) {
        switch (pos_id[this->dyna.actor.params]) {
            case 6:
                set_aim_pos(this, 0);
                break;
            case 7:
                set_aim_pos(this, 1);
                break;
            case 9:
            case 11:
                set_aim_pos(this, 4);
                break;
            case 14:
                set_aim_pos(this, 2);
                break;
            case 15:
                if (check_is_block(8)) {
                    set_aim_pos(this, 3);
                } else {
                    set_aim_pos(this, 8);
                }
                break;
        }
    }
}

static void mode_block_wait(BgGndIceblock* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        if (this->dyna.unk_150 > 0.0f) {
            ice_block_check_move_distance(this);
            if (Actor_search_position_distanceXZ(&this->dyna.actor, &this->targetPos) > 1.0f) {
                player_demo_mode_set(play, &this->dyna.actor, PLAYER_CSACTION_8);
                this->actionFunc = mode_block_move;
            }
        }
        this->dyna.unk_150 = 0.0f;
    }
}

static void mode_block_up(BgGndIceblock* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* thisx = &this->dyna.actor;

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        this->dyna.unk_150 = 0.0f;
    }
    if (chase_f(&thisx->world.pos.y, thisx->home.pos.y, 1.0f)) {
        this->targetPos = thisx->home.pos;
        thisx->speed = 0.0f;
        this->actionFunc = mode_block_wait;
        switch (thisx->params) {
            case 0:
                pos_id[0] = 7;
                break;
            case 1:
                pos_id[1] = 14;
                break;
        }
    }
}

void mode_block_fall(BgGndIceblock* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    thisx->velocity.y += 1.0f;
    if (chase_f(&thisx->world.pos.y, thisx->home.pos.y - 300.0f, thisx->velocity.y)) {
        thisx->velocity.y = 0.0f;
        thisx->world.pos.x = thisx->home.pos.x;
        thisx->world.pos.y = thisx->home.pos.y - 100.0f;
        thisx->world.pos.z = thisx->home.pos.z;
        if (player_demo_check(play)) {
            player_demo_mode_set(play, thisx, PLAYER_CSACTION_7);
        }
        this->actionFunc = mode_block_up;
    }
}

void mode_block_match(BgGndIceblock* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    thisx->velocity.y += 1.0f;
    if (chase_f(&thisx->world.pos.y, thisx->home.pos.y - 100.0f, thisx->velocity.y)) {
        thisx->velocity.y = 0.0f;
        if (player_demo_check(play)) {
            player_demo_mode_set(play, thisx, PLAYER_CSACTION_7);
        }
        this->actionFunc = mode_block_wait;
    }
}

static void mode_block_move(BgGndIceblock* this, PlayState* play) {
    s32 atTarget;
    Vec3f pos;
    Vec3f velocity;
    f32 spread;
    Actor* thisx = &this->dyna.actor;

    chase_f(&thisx->speed, 10.0f, 0.5f);
    atTarget = chase_f(&thisx->world.pos.x, this->targetPos.x, thisx->speed);
    atTarget &= chase_f(&thisx->world.pos.z, this->targetPos.z, thisx->speed);
    if (atTarget) {
        thisx->speed = 0.0f;
        this->targetPos.x = thisx->world.pos.x;
        this->targetPos.z = thisx->world.pos.z;
        Actor_SE_set(thisx, NA_SE_EV_BLOCK_BOUND);
        switch (ice_block_set_y(this)) {
            case GNDICE_IDLE:
                this->actionFunc = mode_block_wait;
                player_demo_mode_set(play, thisx, PLAYER_CSACTION_7);
                break;
            case GNDICE_FALL:
                this->actionFunc = mode_block_fall;
                break;
            case GNDICE_HOLE:
                this->actionFunc = mode_block_match;
                break;
        }
    } else if (thisx->speed > 6.0f) {
        spread = rnd_fx(120.0f);
        velocity.x = -(1.5f + fqrand()) * sin_s(this->dyna.unk_158);
        velocity.y = fqrand() + 1.0f;
        velocity.z = -(1.5f + fqrand()) * cos_s(this->dyna.unk_158);
        pos.x = thisx->world.pos.x - (60.0f * sin_s(this->dyna.unk_158)) - (cos_s(this->dyna.unk_158) * spread);
        pos.z = thisx->world.pos.z - (60.0f * cos_s(this->dyna.unk_158)) + (sin_s(this->dyna.unk_158) * spread);
        pos.y = thisx->world.pos.y;
        Effect_SS_Dust_sc_cl_ct(play, &pos, &velocity, &zero_vec, &prim, &env, 250, get_random_timer(40, 15));
        spread = rnd_fx(120.0f);
        pos.x = thisx->world.pos.x - (60.0f * sin_s(this->dyna.unk_158)) + (cos_s(this->dyna.unk_158) * spread);
        pos.z = thisx->world.pos.z - (60.0f * cos_s(this->dyna.unk_158)) - (sin_s(this->dyna.unk_158) * spread);
        Effect_SS_Dust_sc_cl_ct(play, &pos, &velocity, &zero_vec, &prim, &env, 250, get_random_timer(40, 15));
        Actor_level_SE_set(thisx, NA_SE_PL_SLIP_ICE_LEVEL - SFX_FLAG);
    }
}

void Bg_Gnd_Iceblock_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndIceblock* this = (BgGndIceblock*)thisx;

    this->actionFunc(this, play);
}

void Bg_Gnd_Iceblock_Actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndIceblock* this = (BgGndIceblock*)thisx;

    Cheap_gfx_display(play, gWaterTrialIceBlockDL);
}
