#ifndef Z64HORSE_H
#define Z64HORSE_H

#include "ultra64.h"
#include "z64math.h"

struct PlayState;
struct Actor;
struct Player;

void Horse_goto_Default(struct PlayState* play);
void Horse_escape_Spot06Water(struct PlayState* play);
void Horse_Set_Check(struct PlayState* play, struct Player* player);
void horse_rot_trace_pos(struct Actor* actor, Vec3f* pos, s16 turnAmount);

#endif
