#include "global.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA | ACTOR_FLAG_CAN_PRESS_SWITCHES)

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ntsc-1.2:128 pal-1.1:128 hiratsu3:128"

void (*Player_actor_ct_func)(Actor* thisx, PlayState* play);
void (*Player_actor_dt_func)(Actor* thisx, PlayState* play);
void (*Player_actor_move_func)(Actor* thisx, PlayState* play);
void (*Player_actor_draw_func)(Actor* thisx, PlayState* play);

void Player_actor_ct_call(Actor* thisx, PlayState* play);
void Player_actor_dt_call(Actor* thisx, PlayState* play);
void Player_actor_move_call(Actor* thisx, PlayState* play);
void Player_actor_draw_call(Actor* thisx, PlayState* play);

void Player_actor_ct(Actor* thisx, PlayState* play);
void Player_actor_dt(Actor* thisx, PlayState* play);
void Player_actor_move(Actor* thisx, PlayState* play);
void Player_actor_draw(Actor* thisx, PlayState* play);

ActorProfile Player_Profile = {
    /**/ ACTOR_PLAYER,
    /**/ ACTORCAT_PLAYER,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(Player),
    /**/ Player_actor_ct_call,
    /**/ Player_actor_dt_call,
    /**/ Player_actor_move_call,
    /**/ Player_actor_draw_call,
};

void initfunc(void) {
    Player_actor_ct_func = KaleidoArea_dllcnv(Player_actor_ct);
    Player_actor_dt_func = KaleidoArea_dllcnv(Player_actor_dt);
    Player_actor_move_func = KaleidoArea_dllcnv(Player_actor_move);
    Player_actor_draw_func = KaleidoArea_dllcnv(Player_actor_draw);
}

void Player_actor_ct_call(Actor* thisx, PlayState* play) {
    load_player();
    initfunc();
    Player_actor_ct_func(thisx, play);
}

void Player_actor_dt_call(Actor* thisx, PlayState* play) {
    load_player();
    Player_actor_dt_func(thisx, play);
}

void Player_actor_move_call(Actor* thisx, PlayState* play) {
    load_player();
    Player_actor_move_func(thisx, play);
}

void Player_actor_draw_call(Actor* thisx, PlayState* play) {
    load_player();
    Player_actor_draw_func(thisx, play);
}
