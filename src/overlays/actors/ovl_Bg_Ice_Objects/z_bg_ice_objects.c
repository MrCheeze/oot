/*
 * File: z_bg_ice_objects.c
 * Overlay: ovl_Bg_Ice_Objects
 * Description: Pushable ice block (Ice Cavern)
 */

#include "z_bg_ice_objects.h"
#include "assets/objects/object_ice_objects/object_ice_objects.h"

#define FLAGS 0

void Bg_Ice_Objects_actor_ct(Actor* thisx, PlayState* play);
void Bg_Ice_Objects_actor_dt(Actor* thisx, PlayState* play);
void Bg_Ice_Objects_actor_move(Actor* thisx, PlayState* play);
void Bg_Ice_Objects_actor_draw(Actor* thisx, PlayState* play);

static void mode_block_wait(BgIceObjects* this, PlayState* play);
static void mode_block_move(BgIceObjects* this, PlayState* play);
static void mode_block_up(BgIceObjects* this, PlayState* play);
static void mode_block_stop(BgIceObjects* this, PlayState* play);

static Color_RGBA8 prim = { 250, 250, 250, 255 };
static Color_RGBA8 env = { 180, 180, 180, 255 };
static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };

ActorProfile Bg_Ice_Objects_Profile = {
    /**/ ACTOR_BG_ICE_OBJECTS,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_ICE_OBJECTS,
    /**/ sizeof(BgIceObjects),
    /**/ Bg_Ice_Objects_actor_ct,
    /**/ Bg_Ice_Objects_actor_dt,
    /**/ Bg_Ice_Objects_actor_move,
    /**/ Bg_Ice_Objects_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Ice_Objects_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgIceObjects* this = (BgIceObjects*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&object_ice_objects_Col_0003F0, &colHeader);
    xyz_t_move(&this->targetPos, &this->dyna.actor.home.pos);
    this->actionFunc = mode_block_wait;
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->dyna.actor.params = 0;
}

void Bg_Ice_Objects_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgIceObjects* this = (BgIceObjects*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static s16 x_base[] = {
    -1060, -1200, -1240, -1387, -1580, -1680, -1780,
};
static s16 z_base[] = {
    -580, -660, -780, -820, -860, -900, -1087,
};
static s16 z_move_pos[7][2] = {
    { -580, -1087 }, { -780, -1260 }, { -340, -820 }, { -260, -1260 }, { -340, -860 }, { -660, -1260 }, { -340, -740 },
};
static s16 x_move_pos[7][2] = {
    { -860, -1580 },  { -1240, -1780 }, { -860, -1680 }, { -860, -1680 },
    { -1387, -1680 }, { -860, -1200 },  { -860, -1800 },
};

/*
 * Checks which of the eight possible x and z positions the block is at,
 * defaulting to the maximum x wall or minimum z wall. Each x and z position
 * has only one possible wall or pit on each side of it.
 */
static void ice_block_check_move_distance(BgIceObjects* this, PlayState* play) {
    s16 x16;
    s16 z16 = 0; // needed to match
    s32 i;

    if ((this->dyna.unk_158 == 0) || (this->dyna.unk_158 == -0x8000)) {
        x16 = this->dyna.actor.world.pos.x;
        for (i = 0; i < 7; i++) {
            if (x16 == x_base[i]) {
                z16 = (this->dyna.unk_158 == 0) ? z_move_pos[i][0] : z_move_pos[i][1];
                this->targetPos.z = z16;
                return;
            }
        }
        this->targetPos.z = (this->dyna.unk_158 == 0) ? -340 : -1260;
    } else {
        z16 = this->dyna.actor.world.pos.z;
        for (i = 0; i < 7; i++) {
            if (z16 == z_base[i]) {
                x16 = (this->dyna.unk_158 == 0x4000) ? x_move_pos[i][0] : x_move_pos[i][1];
                this->targetPos.x = x16;
                return;
            }
        }
        this->targetPos.x = (this->dyna.unk_158 == 0x4000) ? -860 : -1780;
    }
}

/*
 * Checks if the block has fallen into any of the pits.
 */
static void ice_block_set_y(BgIceObjects* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if ((thisx->velocity.y > 0.0f) || ((thisx->world.pos.x <= -1660.0f) && (thisx->world.pos.z <= -1060.0f)) ||
        ((thisx->world.pos.x <= -1580.0f) && (thisx->world.pos.z >= -420.0f)) ||
        ((thisx->world.pos.x >= -980.0f) && (thisx->world.pos.z <= -1180.0f)) ||
        ((thisx->world.pos.x >= -860.0f) && (thisx->world.pos.z >= -700.0f))) {

        thisx->velocity.y += 1.0f;
        if (chase_f(&thisx->world.pos.y, -300.0f, thisx->velocity.y)) {
            thisx->velocity.y = 0.0f;
            thisx->world.pos.x = thisx->home.pos.x;
            thisx->world.pos.y = thisx->home.pos.y - 60.0f;
            thisx->world.pos.z = thisx->home.pos.z;
            if (thisx->params != 0) {
                player_demo_mode_set(play, thisx, PLAYER_CSACTION_7);
            }
            this->actionFunc = mode_block_up;
        }
    }
}

static void mode_block_wait(BgIceObjects* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* thisx = &this->dyna.actor;

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        if ((this->dyna.unk_150 > 0.0f) && !player_demo_check(play)) {
            ice_block_check_move_distance(this, play);
            if (Actor_search_position_distanceXZ(thisx, &this->targetPos) > 1.0f) {
                thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
                player_demo_mode_set(play, thisx, PLAYER_CSACTION_8);
                thisx->params = 1;
                this->actionFunc = mode_block_move;
            }
        }
        this->dyna.unk_150 = 0.0f;
    }
    if (thisx->velocity.y > 0.0f) {
        ice_block_set_y(this, play);
    }
}

static void mode_block_move(BgIceObjects* this, PlayState* play) {
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
        if (thisx->velocity.y <= 0.0f) {
            thisx->flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        }
        thisx->params = 0;
        player_demo_mode_set(play, thisx, PLAYER_CSACTION_7);
        Actor_SE_set(thisx, NA_SE_EV_BLOCK_BOUND);
        if ((fabsf(thisx->world.pos.x + 1387.0f) < 1.0f) && (fabsf(thisx->world.pos.z + 260.0f) < 1.0f)) {
            this->actionFunc = mode_block_stop;
        } else {
            this->actionFunc = mode_block_wait;
        }
    } else if ((thisx->speed > 6.0f) && (thisx->world.pos.y >= 0.0f)) {
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
    ice_block_set_y(this, play);
}

static void mode_block_up(BgIceObjects* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* thisx = &this->dyna.actor;

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        this->dyna.unk_150 = 0.0f;
    }
    if (chase_f(&thisx->world.pos.y, thisx->home.pos.y, 1.0f)) {
        thisx->flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        xyz_t_move(&this->targetPos, &thisx->home.pos);
        this->actionFunc = mode_block_wait;
        thisx->speed = 0.0f;
    }
}

static void mode_block_stop(BgIceObjects* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 != 0.0f) {
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        this->dyna.unk_150 = 0.0f;
    }
}

void Bg_Ice_Objects_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgIceObjects* this = (BgIceObjects*)thisx;

    this->actionFunc(this, play);
}

void Bg_Ice_Objects_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgIceObjects* this = (BgIceObjects*)thisx;

    Cheap_gfx_display(play, object_ice_objects_DL_000190);
}
