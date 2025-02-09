/*
 * File: z_obj_mure2.c
 * Overlay: ovl_Obj_Mure2
 * Description: Rock/Bush groups
 */

#include "z_obj_mure2.h"

#define FLAGS 0

typedef void (*ObjMure2SetPosFunc)(Vec3f* vec, ObjMure2* this);

typedef struct Mure2sScatteredShrubInfo {
    s16 radius;
    s16 angle;
} Mure2sScatteredShrubInfo;

void Obj_Mure2_actor_ct(Actor* thisx, PlayState* play);
void Obj_Mure2_actor_move(Actor* thisx, PlayState* play);

void get_child_position_kusa_en(Vec3f* vec, ObjMure2* this);
void get_child_position_kusa_bara(Vec3f* vec, ObjMure2* this);
void get_child_position_ishi(Vec3f* vec, ObjMure2* this);
static void mv_cameraWait(ObjMure2* this, PlayState* play);
static void mv_out(ObjMure2* this, PlayState* play);
static void mv_in(ObjMure2* this, PlayState* play);
static void mv_cameraWait_init(ObjMure2* this);
static void mv_out_init(ObjMure2* this);
static void mv_in_init(ObjMure2* this);

ActorProfile Obj_Mure2_Profile = {
    /**/ ACTOR_OBJ_MURE2,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(ObjMure2),
    /**/ Obj_Mure2_actor_ct,
    /**/ Cheap_non_move,
    /**/ Obj_Mure2_actor_move,
    /**/ NULL,
};

static f32 M2_IntoDist2Tbl[] = { SQ(1600.0f), SQ(1600.0f), SQ(1600.0f) };

static f32 M2_OuttoDist2Tbl[] = { SQ(1705.0f), SQ(1705.0f), SQ(1705.0f) };

static s16 M2_ChildNumTbl[] = { 9, 12, 8 };

static s16 M2_ProfileNumTbl[] = { ACTOR_EN_KUSA, ACTOR_EN_KUSA, ACTOR_EN_ISHI };

void get_child_position_kusa_en(Vec3f* vec, ObjMure2* this) {
    s32 i;

    xyz_t_move(vec, &this->actor.world.pos);
    for (i = 1; i < M2_ChildNumTbl[PARAMS_GET_U(this->actor.params, 0, 2)]; i++) {
        xyz_t_move(vec + i, &this->actor.world.pos);
        (vec + i)->x += (80.0f * sin_s((i - 1) * 0x2000));
        (vec + i)->z += (80.0f * cos_s((i - 1) * 0x2000));
    }
}

static Mure2sScatteredShrubInfo pos_data[] = {
    { 40, 0x0666 }, { 40, 0x2CCC }, { 40, 0x5999 }, { 40, 0x8666 }, { 20, 0xC000 }, { 80, 0x1333 },
    { 80, 0x4000 }, { 80, 0x6CCC }, { 80, 0x9333 }, { 80, 0xACCC }, { 80, 0xC666 }, { 60, 0xE000 },
};

void get_child_position_kusa_bara(Vec3f* vec, ObjMure2* this) {
    s32 i;

    for (i = 0; i < M2_ChildNumTbl[PARAMS_GET_U(this->actor.params, 0, 2)]; i++) {
        xyz_t_move(vec + i, &this->actor.world.pos);
        (vec + i)->x += (pos_data[i].radius * cos_s(pos_data[i].angle));
        (vec + i)->z -= (pos_data[i].radius * sin_s(pos_data[i].angle));
    }
}

void get_child_position_ishi(Vec3f* vec, ObjMure2* this) {
    s32 i;

    for (i = 0; i < M2_ChildNumTbl[PARAMS_GET_U(this->actor.params, 0, 2)]; i++) {
        xyz_t_move(vec + i, &this->actor.world.pos);
        (vec + i)->x += (80.0f * sin_s(i * 0x2000));
        (vec + i)->z += (80.0f * cos_s(i * 0x2000));
    }
}

void get_child_arg_data_mure2(s16* params, ObjMure2* this) {
    static s16 base_arg_data[] = { 0, 0, 0 };
    s16 dropTable = PARAMS_GET_U(this->actor.params, 8, 4);

    if (dropTable >= 13) {
        dropTable = 0;
    }
    *params = base_arg_data[PARAMS_GET_U(this->actor.params, 0, 2)] & 0xF0FF;
    *params |= (dropTable << 8);
}

void reset_child_actor_mure2(ObjMure2* this, PlayState* play) {
    static ObjMure2SetPosFunc ch_pos_proc[] = {
        get_child_position_kusa_en,
        get_child_position_kusa_bara,
        get_child_position_ishi,
    };
    s32 actorNum = PARAMS_GET_U(this->actor.params, 0, 2);
    s32 i;
    Vec3f spawnPos[12];
    s16 params;

    ch_pos_proc[actorNum](spawnPos, this);
    get_child_arg_data_mure2(&params, this);

    for (i = 0; i < M2_ChildNumTbl[actorNum]; i++) {
        if (this->actorSpawnPtrList[i] != NULL) {
            // "Warning : I already have a child (%s %d)(arg_data 0x%04x)"
            PRINTF("Warning : 既に子供がいる(%s %d)(arg_data 0x%04x)\n", "../z_obj_mure2.c", 269, this->actor.params);
            continue;
        }

        if (((this->currentActorNum >> i) & 1) == 0) {
            this->actorSpawnPtrList[i] =
                Actor_info_make_actor(&play->actorCtx, play, M2_ProfileNumTbl[actorNum], spawnPos[i].x, spawnPos[i].y,
                            spawnPos[i].z, this->actor.world.rot.x, 0, this->actor.world.rot.z, params);
            if (this->actorSpawnPtrList[i] != NULL) {
                this->actorSpawnPtrList[i]->room = this->actor.room;
            }
        }
    }
}

void delete_child_actor_mure2(ObjMure2* this, PlayState* play) {
    s32 i;

    for (i = 0; i < M2_ChildNumTbl[PARAMS_GET_U(this->actor.params, 0, 2)]; i++) {
        if (((this->currentActorNum >> i) & 1) == 0) {
            if (this->actorSpawnPtrList[i] != NULL) {
                if (Actor_carry_check(this->actorSpawnPtrList[i], play)) {
                    this->currentActorNum |= (1 << i);
                } else {
                    Actor_delete(this->actorSpawnPtrList[i]);
                }
                this->actorSpawnPtrList[i] = NULL;
            }
        } else {
            this->actorSpawnPtrList[i] = NULL;
        }
    }
}

void watch_child_mure2(ObjMure2* this) {
    s32 i;

    for (i = 0; i < M2_ChildNumTbl[PARAMS_GET_U(this->actor.params, 0, 2)]; i++) {
        if (this->actorSpawnPtrList[i] != NULL && (((this->currentActorNum >> i) & 1) == 0) &&
            (this->actorSpawnPtrList[i]->update == NULL)) {
            this->currentActorNum |= (1 << i);
            this->actorSpawnPtrList[i] = NULL;
        }
    }
}

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 2100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_STOP),
};

void Obj_Mure2_actor_ct(Actor* thisx, PlayState* play) {
    ObjMure2* this = (ObjMure2*)thisx;

    ValueSet_process(&this->actor, value_init);
    if (play->csCtx.state != CS_STATE_IDLE) {
        this->actor.cullingVolumeDistance += 1200.0f;
    }
    mv_cameraWait_init(this);
}

static void mv_cameraWait_init(ObjMure2* this) {
    this->actionFunc = mv_cameraWait;
}

static void mv_cameraWait(ObjMure2* this, PlayState* play) {
    mv_out_init(this);
}

static void mv_out_init(ObjMure2* this) {
    this->actionFunc = mv_out;
}

static void mv_out(ObjMure2* this, PlayState* play) {
    if (Math3DVecLengthSquare2D(this->actor.projectedPos.x, this->actor.projectedPos.z) <
        (M2_IntoDist2Tbl[PARAMS_GET_U(this->actor.params, 0, 2)] * this->unk_184)) {
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        reset_child_actor_mure2(this, play);
        mv_in_init(this);
    }
}

static void mv_in_init(ObjMure2* this) {
    this->actionFunc = mv_in;
}

static void mv_in(ObjMure2* this, PlayState* play) {
    watch_child_mure2(this);
    if ((M2_OuttoDist2Tbl[PARAMS_GET_U(this->actor.params, 0, 2)] * this->unk_184) <=
        Math3DVecLengthSquare2D(this->actor.projectedPos.x, this->actor.projectedPos.z)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        delete_child_actor_mure2(this, play);
        mv_out_init(this);
    }
}

void Obj_Mure2_actor_move(Actor* thisx, PlayState* play) {
    ObjMure2* this = (ObjMure2*)thisx;

    if (play->csCtx.state == CS_STATE_IDLE) {
        this->unk_184 = 1.0f;
    } else {
        this->unk_184 = 4.0f;
    }
    this->actionFunc(this, play);
}
