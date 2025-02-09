/*
 * File: z_obj_mure3.c
 * Overlay: ovl_Obj_Mure3
 * Description: Tower of Rupees
 */

#include "z_obj_mure3.h"

#define FLAGS 0

void Obj_Mure3_actor_ct(Actor* thisx, PlayState* play);
void Obj_Mure3_actor_dt(Actor* thisx, PlayState* play);
void Obj_Mure3_actor_move(Actor* thisx, PlayState* play);

static void mv_cameraWait_init(ObjMure3* this);
static void mv_cameraWait(ObjMure3* this, PlayState* play);
static void mv_out_init(ObjMure3* this);
static void mv_out(ObjMure3* this, PlayState* play);
static void mv_in_init(ObjMure3* this);
static void mv_in(ObjMure3* this, PlayState* play);

ActorProfile Obj_Mure3_Profile = {
    /**/ ACTOR_OBJ_MURE3,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(ObjMure3),
    /**/ Obj_Mure3_actor_ct,
    /**/ Obj_Mure3_actor_dt,
    /**/ Obj_Mure3_actor_move,
    /**/ NULL,
};

static s16 M3_ChildNum[] = { 5, 5, 7, 0 };

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_STOP),
};

void make_rupee_vertical(ObjMure3* this, PlayState* play) {
    s32 i;
    Vec3f spawnPos;

    xyz_t_move(&spawnPos, &this->actor.world.pos);
    for (i = 0; i < 5; i++, spawnPos.y += 20.0f) {
        if (!((this->unk_16C >> i) & 1)) {
            this->unk_150[i] = Item_set1(play, &spawnPos, 0x4000 | ITEM00_RUPEE_BLUE);
            if (this->unk_150[i] != NULL) {
                this->unk_150[i]->actor.room = this->actor.room;
            }
        }
    }
}

void make_rupee_horizontal(ObjMure3* this, PlayState* play) {
    s32 i;
    Vec3f spawnPos;
    f32 sn = sin_s(this->actor.world.rot.y);
    f32 cos = cos_s(this->actor.world.rot.y);
    f32 radius;

    spawnPos.y = this->actor.world.pos.y;

    for (i = 0, radius = -40.0f; i < 5; i++, radius += 20.0f) {
        if (!((this->unk_16C >> i) & 1)) {
            spawnPos.x = this->actor.world.pos.x + (sn * radius);
            spawnPos.z = this->actor.world.pos.z + (cos * radius);
            this->unk_150[i] = Item_set1(play, &spawnPos, 0x4000 | ITEM00_RUPEE_GREEN);
            if (this->unk_150[i] != NULL) {
                this->unk_150[i]->actor.room = this->actor.room;
            }
        }
    }
}

void make_rupee_circle(ObjMure3* this, PlayState* play) {
    s32 i;
    Vec3f spawnPos;
    s16 yRot;

    spawnPos.y = this->actor.world.pos.y;
    yRot = this->actor.world.rot.y;
    for (i = 0; i < 6; i++) {
        if (!((this->unk_16C >> i) & 1)) {
            spawnPos.x = (sin_s(yRot) * 40.0f) + this->actor.world.pos.x;
            spawnPos.z = (cos_s(yRot) * 40.0f) + this->actor.world.pos.z;
            this->unk_150[i] = Item_set1(play, &spawnPos, 0x4000 | ITEM00_RUPEE_GREEN);
            if (this->unk_150[i] != NULL) {
                this->unk_150[i]->actor.room = this->actor.room;
            }
        }
        yRot += 0x2AAA;
    }
    if (!((this->unk_16C >> 6) & 1)) {
        spawnPos.x = this->actor.world.pos.x;
        spawnPos.z = this->actor.world.pos.z;
        this->unk_150[6] = Item_set1(play, &spawnPos, 0x4000 | ITEM00_RUPEE_RED);
        if (this->unk_150[6] != NULL) {
            this->unk_150[6]->actor.room = this->actor.room;
        }
    }
}

void delete_child_actor_mure3(ObjMure3* this, PlayState* play) {
    s16 count = M3_ChildNum[PARAMS_GET_U(this->actor.params, 13, 3)];
    s32 i;
    EnItem00** collectible;

    for (i = 0; i < count; i++) {
        collectible = &this->unk_150[i];

        if (!((this->unk_16C >> i) & 1) && (*collectible != NULL)) {
            if (Actor_carry_check(&(*collectible)->actor, play) || ((*collectible)->actor.update == NULL)) {
                this->unk_16C |= (1 << i);
            } else {
                Actor_delete(&(*collectible)->actor);
            }
        }
        *collectible = NULL;
    }
}

void watch_child_mure3(ObjMure3* this, PlayState* play) {
    s16 count = M3_ChildNum[PARAMS_GET_U(this->actor.params, 13, 3)];
    s32 i;

    for (i = 0; i < count; i++) {
        EnItem00** collectible = &this->unk_150[i];

        if ((*collectible != NULL) && !((this->unk_16C >> i) & 1)) {
            if (Actor_carry_check(&(*collectible)->actor, play)) {
                Actor_Environment_sw_On(play, PARAMS_GET_U(this->actor.params, 0, 6));
            }
            if ((*collectible)->actor.update == NULL) {
                this->unk_16C |= (1 << i);
                this->unk_150[i] = NULL;
            }
        }
    }
}

void Obj_Mure3_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjMure3* this = (ObjMure3*)thisx;

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 0, 6))) {
        Actor_delete(&this->actor);
        return;
    }
    ValueSet_process(&this->actor, value_init);
    mv_cameraWait_init(this);
}

void Obj_Mure3_actor_dt(Actor* thisx, PlayState* play) {
}

static void mv_cameraWait_init(ObjMure3* this) {
    this->actionFunc = mv_cameraWait;
}

static void mv_cameraWait(ObjMure3* this, PlayState* play) {
    mv_out_init(this);
}

static void mv_out_init(ObjMure3* this) {
    this->actionFunc = mv_out;
}

static void mv_out(ObjMure3* this, PlayState* play) {
    static ObjMure3SpawnFunc make_rupee_proc[] = { make_rupee_vertical, make_rupee_horizontal, make_rupee_circle };

    if (Math3DVecLengthSquare2D(this->actor.projectedPos.x, this->actor.projectedPos.z) < SQ(1150.0f)) {
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        make_rupee_proc[PARAMS_GET_U(this->actor.params, 13, 3)](this, play);
        mv_in_init(this);
    }
}

static void mv_in_init(ObjMure3* this) {
    this->actionFunc = mv_in;
}

static void mv_in(ObjMure3* this, PlayState* play) {
    watch_child_mure3(this, play);
    if (Math3DVecLengthSquare2D(this->actor.projectedPos.x, this->actor.projectedPos.z) >= SQ(1450.0f)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        delete_child_actor_mure3(this, play);
        mv_out_init(this);
    }
}

void Obj_Mure3_actor_move(Actor* thisx, PlayState* play) {
    ObjMure3* this = (ObjMure3*)thisx;

    this->actionFunc(this, play);
}
