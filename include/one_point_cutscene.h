#ifndef ONE_POINT_CUTSCENE_H
#define ONE_POINT_CUTSCENE_H

#include "ultra64.h"

struct Actor;
struct PlayState;

s16 makeOnepointDemo(struct PlayState* play, s16 csId, s16 timer, struct Actor* actor, s16 parentCamId);
s16 deleteOnepointDemo(struct PlayState* play, s16 subCamId);
s32 makeActorAttentionDemo(struct PlayState* play, struct Actor* actor);
s32 makeActorAttentionDemoSE(struct PlayState* play, struct Actor* actor, s32 sfxId);
void allowActorAttentionDemo(void);
void denyActorAttentionDemo(void);
s32 checkPartrActorAttentionDemo(struct PlayState* play, s32 actorCategory);
void makeDoorDemo(struct PlayState* play, s32 arg1);

#endif
