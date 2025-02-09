/*
 * File: z_obj_bean.c
 * Overlay: ovl_Obj_Bean
 * Description: Bean plant spot
 */

#include "z_obj_bean.h"
#include "assets/objects/object_mamenoki/object_mamenoki.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_IGNORE_POINT_LIGHTS

void Obj_Bean_actor_ct(Actor* thisx, PlayState* play);
void Obj_Bean_actor_dt(Actor* thisx, PlayState* play);
void Obj_Bean_actor_move(Actor* thisx, PlayState* play);
void Obj_Bean_actor_draw(Actor* thisx, PlayState* play);

void mv_ad_stop(ObjBean* this, PlayState* play);
void mv_ad_walk(ObjBean* this, PlayState* play);
void mv_ad_walk_init(ObjBean* this);
void mv_cn_wakame(ObjBean* this, PlayState* play);
void mv_cn_wakame_init(ObjBean* this);
void mv_cn_arashi1_init(ObjBean* this);
void mv_cn_arashi1(ObjBean* this, PlayState* play);
void mv_cn_arashi2(ObjBean* this, PlayState* play);
void mv_cn_arashi2_init(ObjBean* this);
void mv_cn_arashi3(ObjBean* this, PlayState* play);
void mv_cn_arashi3_init(ObjBean* this);
void pc_pon2_init(ObjBean* this);
void pc_pon1(ObjBean* this);
void pc_pon2(ObjBean* this);
void pc_largo(ObjBean* this);
void pc_allegro_init(ObjBean* this);
void pc_allegro(ObjBean* this);
void mv_cn_request_init(ObjBean* this);
void mv_cn_request(ObjBean* this, PlayState* play);
void mv_cn_mebae0(ObjBean* this, PlayState* play);
void mv_cn_mebae0_init(ObjBean* this);
void mv_cn_mebae1_init(ObjBean* this);
void mv_cn_mebae1(ObjBean* this, PlayState* play);
void mv_cn_mebae2_init(ObjBean* this);
void mv_cn_arashi4_init(ObjBean* this);
void mv_cn_mebae2(ObjBean* this, PlayState* play);
void mv_cn_mebae3(ObjBean* this, PlayState* play);
void mv_cn_mebae3_init(ObjBean* this);
void mv_ad_revivalDropPlayer_init(ObjBean* this);
void mv_ad_revivalDropPlayer(ObjBean* this, PlayState* play);
void mv_ad_revivalWait_init(ObjBean* this);
void mv_ad_revivalWait(ObjBean* this, PlayState* play);
void mv_ad_revivalScaleUp_init(ObjBean* this);
void mv_ad_revivalScaleUp(ObjBean* this, PlayState* play);
void mv_ad_stop_init(ObjBean* this);
void mv_cn_arashi4(ObjBean* this, PlayState* play);
void mv_cn_arashi5(ObjBean* this, PlayState* play);
void mv_cn_arashi5_init(ObjBean* this);
void pc_agitato_init(ObjBean* this);
void pc_agitato(ObjBean* this);
void pc_shibomu(ObjBean* this);
void pc_shibomu_init(ObjBean* this);
void mv_ad_end_init(ObjBean* this);
void mv_ad_end(ObjBean* this, PlayState* play);

#define BEAN_STATE_DRAW_LEAVES (1 << 0)
#define BEAN_STATE_DRAW_SOIL (1 << 1)
#define BEAN_STATE_DRAW_PLANT (1 << 2)
#define BEAN_STATE_DRAW_STALK (1 << 3)
#define BEAN_STATE_COLLIDER_SET (1 << 4)
#define BEAN_STATE_DYNAPOLY_SET (1 << 5)
#define BEAN_STATE_BEEN_WATERED (1 << 6)
#define BEAN_STATE_PLAYER_ON_TOP (1 << 7)

static ObjBean* KukiNobi_actor = NULL;

ActorProfile Obj_Bean_Profile = {
    /**/ ACTOR_OBJ_BEAN,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MAMENOKI,
    /**/ sizeof(ObjBean),
    /**/ Obj_Bean_actor_ct,
    /**/ Obj_Bean_actor_dt,
    /**/ Obj_Bean_actor_move,
    /**/ Obj_Bean_actor_draw,
};

static ColliderCylinderInit ClPipeDt_bean = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 64, 30, -31, { 0, 0, 0 } },
};

typedef struct BeenSpeedInfo {
    f32 velocity;
    f32 accel;
} BeenSpeedInfo;

static BeenSpeedInfo Bean_Speed_Data[] = {
    { 3.0f, 0.3f },
    { 10.0f, 0.5f },
    { 30.0f, 0.5f },
    { 3.0f, 0.3f },
};

static Gfx* model[] = { gCuttableShrubStalkDL, gCuttableShrubTipDL };

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1600, ICHAIN_STOP),
};

void set_collision_bean(Actor* thisx, PlayState* play) {
    ObjBean* this = (ObjBean*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->dyna.actor, &ClPipeDt_bean);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->collider);
}

static void set_dynaPoly(ObjBean* this, PlayState* play, CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, moveFlag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_obj_bean.c", 374,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void set_ground_y_bean(ObjBean* this, PlayState* play) {
    Vec3f checkPos;
    s32 bgId;

    checkPos.x = this->dyna.actor.world.pos.x;
    checkPos.y = this->dyna.actor.world.pos.y + 29.999998f;
    checkPos.z = this->dyna.actor.world.pos.z;
    this->dyna.actor.floorHeight =
        T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->dyna.actor.floorPoly, &bgId, &this->dyna.actor, &checkPos);
}

void fuwafuwaSwingNormal_bean_init(ObjBean* this) {
    this->unk_1B6.x = this->unk_1B6.y = this->unk_1B6.z = 0;
    this->unk_1E4 = 0.0f;
}

void fuwafuwaSwingNormal_bean(ObjBean* this) {
    f32 temp_f20;

    this->unk_1B6.x += 0xB6;
    this->unk_1B6.y += 0xFB;
    this->unk_1B6.z += 0x64;

    chase_f(&this->unk_1E4, 2.0f, 0.1f);
    temp_f20 = sin_s(this->unk_1B6.x * 3);
    this->posOffsetX = (sin_s(this->unk_1B6.y * 3) + temp_f20) * this->unk_1E4;
    temp_f20 = cos_s(this->unk_1B6.x * 4);
    this->posOffsetZ = (cos_s(this->unk_1B6.y * 4) + temp_f20) * this->unk_1E4;
    temp_f20 = sin_s(this->unk_1B6.z * 5);

    this->dyna.actor.scale.x = this->dyna.actor.scale.z =
        ((sin_s(this->unk_1B6.y * 8) * 0.01f) + (temp_f20 * 0.06f) + 1.07f) * 0.1f;

    this->dyna.actor.scale.y = ((cos_s(this->unk_1B6.z * 10) * 0.2f) + 1.0f) * 0.1f;
    temp_f20 = sin_s(this->unk_1B6.x * 3);
    this->dyna.actor.shape.rot.y =
        (sin_s((s16)(this->unk_1B6.z * 2)) * 2100.0f) + ((f32)this->dyna.actor.home.rot.y + (temp_f20 * 1000.0f));
}

void fuwafuwaSwingScaleUp_bean_init(ObjBean* this) {
    this->unk_1B6.x = this->unk_1B6.y = this->unk_1B6.z = 0;

    Actor_set_scale(&this->dyna.actor, 0.0f);
}

void fuwafuwaSwingScaleUp_bean(ObjBean* this) {
    this->unk_1B6.x += 0x384;
    if (this->unk_1B6.x > 0x5FFF) {
        this->unk_1B6.x = 0x5FFF;
    }
    this->unk_1B6.y += 0x258;
    if (this->unk_1B6.y > 0x4000) {
        this->unk_1B6.y = 0x4000;
    }
    this->dyna.actor.scale.y = sin_s(this->unk_1B6.x) * 0.16970563f;

    this->dyna.actor.scale.x = this->dyna.actor.scale.z = sin_s(this->unk_1B6.y) * 0.10700001f;

    chase_f(&this->posOffsetX, 0.0f, 0.1f);
    chase_f(&this->posOffsetZ, 0.0f, 0.1f);
    chase_angle(&this->dyna.actor.shape.rot.y, this->dyna.actor.home.rot.y, 0x64);
}

void set_world_pos_bean(ObjBean* this) {
    this->dyna.actor.world.pos.x = this->pathPoints.x + this->posOffsetX;
    this->dyna.actor.world.pos.y = this->pathPoints.y;
    this->dyna.actor.world.pos.z = this->pathPoints.z + this->posOffsetZ;
}

void reset_disp_flag(ObjBean* this, u8 drawFlag) {
    this->stateFlags &=
        ~(BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_PLANT | BEAN_STATE_DRAW_STALK | BEAN_STATE_DRAW_SOIL);
    this->stateFlags |= drawFlag;
}

void init_path_parmeter_bean(ObjBean* this, PlayState* play) {
    this->pathCount = play->pathList[PARAMS_GET_U(this->dyna.actor.params, 8, 5)].count - 1;
    this->currentPointIndex = 0;
    this->nextPointIndex = 1;
}

void init_now_pos_lift_bean(ObjBean* this, PlayState* play) {
    Path* path = &play->pathList[PARAMS_GET_U(this->dyna.actor.params, 8, 5)];
    xyz_t_move_s_xyz(&this->pathPoints, SEGMENTED_TO_VIRTUAL(path->points));
}

void set_now_pos_lift_bean(ObjBean* this, PlayState* play) {
    Path* path;
    Vec3f acell;
    Vec3f pathPointsFloat;
    f32 mag;
    Vec3s* nextPathPoint;

    chase_f(&this->dyna.actor.speed, Bean_Speed_Data[this->unk_1F6].velocity, Bean_Speed_Data[this->unk_1F6].accel);
    path = &play->pathList[PARAMS_GET_U(this->dyna.actor.params, 8, 5)];
    nextPathPoint = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[this->nextPointIndex];

    xyz_t_move_s_xyz(&pathPointsFloat, nextPathPoint);

    xyz_t_sub(&pathPointsFloat, &this->pathPoints, &acell);
    mag = Math3DVecLength(&acell);
    if (CLAMP_MIN(this->dyna.actor.speed, 0.5f) > mag) {
        Vec3s* currentPoint;
        Vec3s* sp4C;
        Vec3f sp40;
        Vec3f sp34;
        f32 sp30;

        currentPoint = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[this->currentPointIndex];

        xyz_t_move(&this->pathPoints, &pathPointsFloat);
        this->currentPointIndex = this->nextPointIndex;

        if (this->pathCount <= this->currentPointIndex) {
            this->nextPointIndex = 0;
        } else {
            this->nextPointIndex++;
        }
        sp4C = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[this->nextPointIndex];
        xyz_t_sub_ss(&sp40, nextPathPoint, currentPoint);
        xyz_t_sub_ss(&sp34, sp4C, nextPathPoint);
        if (M3D_getCos2(&sp40, &sp34, &sp30)) {
            this->dyna.actor.speed = 0.0f;
        } else {
            this->dyna.actor.speed *= (sp30 + 1.0f) * 0.5f;
        }
    } else {
        xyz_t_mult_v(&acell, this->dyna.actor.speed / mag);
        this->pathPoints.x += acell.x;
        this->pathPoints.y += acell.y;
        this->pathPoints.z += acell.z;
    }
}

s32 hit_horse_bean(ObjBean* this, PlayState* play) {
    Actor* currentActor = play->actorCtx.actorLists[ACTORCAT_BG].head;

    while (currentActor != NULL) {
        if ((currentActor->id == ACTOR_EN_HORSE) &&
            (Math3DLengthSquare(&currentActor->world.pos, &this->dyna.actor.world.pos) < SQ(100.0f))) {
            return true;
        }
        currentActor = currentActor->next;
    }

    return false;
}

void effect_bean_lift(ObjBean* this, PlayState* play) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f20;
    s16 angle;
    s32 scale;
    s32 i;
    s16 gravity;
    s16 arg5;

    angle = 0;
    for (i = 0; i < 36; i++) {
        angle += 0x4E20;
        temp_f20 = fqrand() * 60.0f;

        pos.x = (sin_s(angle) * temp_f20) + this->dyna.actor.world.pos.x;
        pos.y = this->dyna.actor.world.pos.y;
        pos.z = (cos_s(angle) * temp_f20) + this->dyna.actor.world.pos.z;

        velocity.x = sin_s(angle) * 3.5f;
        velocity.y = fqrand() * 13.0f;
        velocity.z = cos_s(angle) * 3.5f;

        velocity.x += this->dyna.actor.world.pos.x - this->dyna.actor.prevPos.x;
        velocity.y += this->dyna.actor.world.pos.y - this->dyna.actor.prevPos.y;
        velocity.z += this->dyna.actor.world.pos.z - this->dyna.actor.prevPos.z;

        scale = (s32)(fqrand() * 180.0f) + 30;
        if (scale < 90) {
            if (fqrand() < 0.1f) {
                gravity = -80;
                arg5 = 96;
            } else {
                gravity = -80;
                arg5 = 64;
            }
        } else {
            gravity = -100;
            arg5 = 64;
        }
        Effect_Kakera_ct2(play, &pos, &velocity, &pos, gravity, arg5, 40, 3, 0, scale, 0, 0,
                             (s16)((scale >> 3) + 40), -1, 1, model[i & 1]);
    }
}

void pc_set_scale_common(ObjBean* this) {
    chase_s(&this->unk_1C2, this->unk_1C4, this->unk_1C6);
    chase_s(&this->unk_1C8, this->unk_1CA, this->unk_1CC);
    this->unk_1CE += this->unk_1C8;
    this->leafRotFactor = 6372.0f - sin_s(this->unk_1CE) * (f32)this->unk_1C2;
    this->dyna.actor.scale.y = sin_s(this->leafRotFactor) * 0.17434467f;
    this->dyna.actor.scale.x = this->dyna.actor.scale.z = cos_s(this->leafRotFactor) * 0.12207746f;
}

void pc_largo_init(ObjBean* this) {
    this->transformFunc = pc_largo;
    this->unk_1C0 = get_random_timer(12, 40);
    this->unk_1C4 = get_random_timer(0xC8, 0x190);
    this->unk_1C6 = 0x14;
    this->unk_1CA = get_random_timer(0x64, 0x320);
    this->unk_1CC = 0x14;
}

void pc_largo(ObjBean* this) {
    this->unk_1C0--;
    pc_set_scale_common(this);
    if (this->unk_1C0 < 0) {
        pc_allegro_init(this);
    }
}

void pc_allegro_init(ObjBean* this) {
    this->transformFunc = pc_allegro;
    this->unk_1C0 = get_random_timer(30, 4);
    this->unk_1C4 = get_random_timer(0x7D0, 0x3E8);
    this->unk_1C6 = 0xC8;
    this->unk_1CA = get_random_timer(0x36B0, 0x1770);
    this->unk_1CC = 0xFA0;
    this->leafRotFactor = 0x18E4;
}

void pc_allegro(ObjBean* this) {
    this->unk_1C0 += -1;
    if (this->unk_1C0 == 14) {
        this->unk_1C4 = get_random_timer(0xC8, 0x190);
        this->unk_1CA = get_random_timer(0x64, 0x1F4);
        this->unk_1CC = 0x7D0;
    }
    pc_set_scale_common(this);
    if (this->unk_1C0 < 0) {
        pc_largo_init(this);
    }
}

void pc_agitato_init(ObjBean* this) {
    this->transformFunc = pc_agitato;
    this->unk_1C0 = 0x28;
    this->unk_1C4 = 0xBB8;
    this->unk_1C6 = 0x12C;
    this->unk_1CA = 0x3A98;
    this->unk_1CC = 0xFA0;
    this->leafRotFactor = 0x18E4;
}

void pc_agitato(ObjBean* this) {
    this->unk_1C0 += -1;
    if (fqrand() < 0.1f) {
        this->unk_1C4 = get_random_timer(0x898, 0x3E8);
        this->unk_1CA = get_random_timer(0x2EE0, 0x1F40);
    }
    pc_set_scale_common(this);
    if ((s32)this->unk_1C0 < 0) {
        pc_shibomu_init(this);
    }
}

void pc_shibomu_init(ObjBean* this) {
    this->transformFunc = pc_shibomu;
}

void pc_shibomu(ObjBean* this) {
    chase_s(&this->leafRotFactor, 0x33E9, 0x168);
    this->dyna.actor.scale.y = sin_s(this->leafRotFactor) * 0.17434467f;
    this->dyna.actor.scale.x = this->dyna.actor.scale.z = cos_s(this->leafRotFactor) * 0.12207746f;
    ;
}

void pc_pon1_init(ObjBean* this) {
    this->transformFunc = pc_pon1;
    this->leafRotFactor = 0x33E9;
}

void pc_pon1(ObjBean* this) {
    this->leafRotFactor -= 0x960;
    this->dyna.actor.scale.y = sin_s(this->leafRotFactor) * 0.17434467f;
    this->dyna.actor.scale.x = this->dyna.actor.scale.z = cos_s(this->leafRotFactor) * 0.12207746f;

    if (this->leafRotFactor < 0x18E4) {
        pc_pon2_init(this);
    }
}

void pc_pon2_init(ObjBean* this) {
    this->transformFunc = pc_pon2;
    this->unk_1C2 = 0xBB8;
    this->unk_1C4 = 0;
    this->unk_1C6 = 0xC8;
    this->unk_1C8 = 0x3E80;
    this->unk_1CA = 0x1F4;
    this->unk_1CC = 0;
    this->unk_1C0 = 0x10;
}

void pc_pon2(ObjBean* this) {
    this->unk_1C0--;
    if (this->unk_1C0 == 6) {
        this->unk_1CC = 0x7D0;
    }
    pc_set_scale_common(this);
    if (this->unk_1C2 <= 0) {
        pc_largo_init(this);
    }
}

void Obj_Bean_actor_ct(Actor* thisx, PlayState* play) {
    s32 path;
    ObjBean* this = (ObjBean*)thisx;

    ValueSet_process(&this->dyna.actor, value_init);
    if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
        if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6)) || (DEBUG_FEATURES && mREG(1) == 1)) {
            path = PARAMS_GET_U(this->dyna.actor.params, 8, 5);
            if (path == 0x1F) {
                PRINTF_COLOR_ERROR();
                // "No path data?"
                PRINTF("パスデータが無い？(%s %d)(arg_data %xH)\n", "../z_obj_bean.c", 909, this->dyna.actor.params);
                PRINTF_RST();
                Actor_delete(&this->dyna.actor);
                return;
            }
            if (play->pathList[path].count < 3) {
                PRINTF_COLOR_ERROR();
                // "Incorrect number of path data"
                PRINTF("パスデータ数が不正(%s %d)(arg_data %xH)\n", "../z_obj_bean.c", 921, this->dyna.actor.params);
                PRINTF_RST();
                Actor_delete(&this->dyna.actor);
                return;
            }
            init_path_parmeter_bean(this, play);
            init_now_pos_lift_bean(this, play);
            set_world_pos_bean(this);
            mv_ad_stop_init(this);

            set_dynaPoly(this, play, &gMagicBeanPlatformCol, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
            this->stateFlags |= BEAN_STATE_DYNAPOLY_SET;
            set_collision_bean(&this->dyna.actor, play);
            this->stateFlags |= BEAN_STATE_COLLIDER_SET;

            Shape_Info_init(&this->dyna.actor.shape, 0.0f, Actor_shadow_circle, 8.8f);
            set_ground_y_bean(this, play);
            this->unk_1F6 = this->dyna.actor.home.rot.z & 3;
        } else {
            Actor_delete(&this->dyna.actor);
            return;
        }
    } else if ((Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6)) != 0) ||
               (DEBUG_FEATURES && mREG(1) == 1)) {
        mv_cn_wakame_init(this);
    } else {
        mv_cn_request_init(this);
    }
    this->dyna.actor.world.rot.z = this->dyna.actor.home.rot.z = this->dyna.actor.shape.rot.z = 0;
    // "Magic bean tree lift"
    PRINTF("(魔法の豆の木リフト)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void Obj_Bean_actor_dt(Actor* thisx, PlayState* play) {
    ObjBean* this = (ObjBean*)thisx;

    if (this->stateFlags & BEAN_STATE_DYNAPOLY_SET) {
        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    }
    if (this->stateFlags & BEAN_STATE_COLLIDER_SET) {
        ClObjPipe_dt(play, &this->collider);
    }
    if (KukiNobi_actor == this) {
        KukiNobi_actor = NULL;
    }
}

void mv_cn_request_init(ObjBean* this) {
    this->actionFunc = mv_cn_request;
    reset_disp_flag(this, BEAN_STATE_DRAW_LEAVES);
    this->dyna.actor.textId = 0x2F;
}

void mv_cn_request(ObjBean* this, PlayState* play) {
    if (Actor_talk_check(&this->dyna.actor, play)) {
        if (Actor_get_item_check(play) == EXCH_ITEM_MAGIC_BEAN) {
            mv_cn_mebae0_init(this);
            Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
        }
    } else {
        Actor_talk_request_get_item(&this->dyna.actor, play, 40.0f, EXCH_ITEM_MAGIC_BEAN);
    }
}

void mv_cn_mebae0_init(ObjBean* this) {
    this->actionFunc = mv_cn_mebae0;
    reset_disp_flag(this, BEAN_STATE_DRAW_LEAVES);
    this->timer = 60;
}

// Link is looking at the soft soil
void mv_cn_mebae0(ObjBean* this, PlayState* play) {
    if (this->timer <= 0) {
        mv_cn_mebae1_init(this);
    }
}

void mv_cn_mebae1_init(ObjBean* this) {
    this->actionFunc = mv_cn_mebae1;
    reset_disp_flag(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    Actor_set_scale(&this->dyna.actor, 0.01f);
}

// The leaves are visable and growing
void mv_cn_mebae1(ObjBean* this, PlayState* play) {
    s32 temp_v1 = true;

    temp_v1 &= chase_f(&this->dyna.actor.scale.y, 0.16672663f, 0.01f);
    temp_v1 &= chase_f(&this->dyna.actor.scale.x, 0.03569199f, 0.00113f);

    this->dyna.actor.scale.z = this->dyna.actor.scale.x;
    if (temp_v1) {
        if (this->timer <= 0) {
            mv_cn_mebae2_init(this);
        }
    } else {
        this->timer = 1;
    }
    Actor_level_SE_set(&this->dyna.actor, NA_SE_PL_PLANT_GROW_UP - SFX_FLAG);
}

void mv_cn_mebae2_init(ObjBean* this) {
    this->actionFunc = mv_cn_mebae2;
    reset_disp_flag(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    this->unk_1B6.x = 0x33E9;
}

void mv_cn_mebae2(ObjBean* this, PlayState* play) {
    this->unk_1B6.x -= 0x960;
    this->dyna.actor.scale.y = sin_s(this->unk_1B6.x) * 0.17434467f;
    this->dyna.actor.scale.x = this->dyna.actor.scale.z = cos_s(this->unk_1B6.x) * 0.12207746f;
    if (this->unk_1B6.x < 0x18E4) {
        mv_cn_mebae3_init(this);
    }
}

void mv_cn_mebae3_init(ObjBean* this) {
    this->actionFunc = mv_cn_mebae3;
    reset_disp_flag(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    this->unk_1B6.x = 0;
    this->unk_1B6.y = 0xBB8;
}

// Control is returned to the player and the leaves start to flatten out
void mv_cn_mebae3(ObjBean* this, PlayState* play) {
    s16 temp_a0;
    f32 temp_f2;

    this->unk_1B6.x += 0x3E80;
    this->unk_1B6.y += -0xC8;
    temp_a0 = 6372.0f - sin_s(this->unk_1B6.x) * this->unk_1B6.y;

    this->dyna.actor.scale.y = sin_s(temp_a0) * 0.17434467f;
    this->dyna.actor.scale.x = this->dyna.actor.scale.z = cos_s(temp_a0) * 0.12207746f;
    if (this->unk_1B6.y < 0) {
        mv_cn_wakame_init(this);
    }
}

void mv_cn_wakame_init(ObjBean* this) {
    this->actionFunc = mv_cn_wakame;
    reset_disp_flag(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    Actor_set_scale(&this->dyna.actor, 0.1f);
    pc_largo_init(this);
}

void mv_cn_wakame(ObjBean* this, PlayState* play) {
    this->transformFunc(this);

    if (!(this->stateFlags & BEAN_STATE_BEEN_WATERED) && eventbit_check(play, 5) && (KukiNobi_actor == NULL) &&
        (this->dyna.actor.xzDistToPlayer < 50.0f)) {
        mv_cn_arashi1_init(this);
        KukiNobi_actor = this;
        makeOnepointDemo(play, 2210, -99, &this->dyna.actor, CAM_ID_MAIN);
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        return;
    }

    if ((KukiNobi_actor == this) && !eventbit_check(play, 5)) {
        KukiNobi_actor = NULL;
        if (KukiNobi_actor) {}
    }
}

void mv_cn_arashi1_init(ObjBean* this) {
    this->actionFunc = mv_cn_arashi1;
    reset_disp_flag(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    pc_agitato_init(this);
    this->timer = 50;
}

// Camera moves and leaves move quickly
void mv_cn_arashi1(ObjBean* this, PlayState* play) {
    this->transformFunc(this);
    if (this->timer <= 0) {
        mv_cn_arashi2_init(this);
    }
}

void mv_cn_arashi2_init(ObjBean* this) {
    this->actionFunc = mv_cn_arashi2;
    reset_disp_flag(this, BEAN_STATE_DRAW_SOIL | BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_STALK);
    this->stalkSizeMultiplier = 0.0f;
}

// BeanStalk is visable and is growing
void mv_cn_arashi2(ObjBean* this, PlayState* play) {
    this->transformFunc(this);
    this->stalkSizeMultiplier += 0.001f;
    this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y + (s16)(this->stalkSizeMultiplier * 700000.0f);
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + this->stalkSizeMultiplier * 800.0f;
    if (this->stalkSizeMultiplier >= 0.1f) { // 100 Frames
        mv_cn_arashi3_init(this);
    }
    Actor_level_SE_set(&this->dyna.actor, NA_SE_PL_PLANT_TALLER - SFX_FLAG);
}

void mv_cn_arashi3_init(ObjBean* this) {
    this->actionFunc = mv_cn_arashi3;
    reset_disp_flag(this, BEAN_STATE_DRAW_SOIL | BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_STALK);
    this->timer = 60;
}

// Fully grown and drops items
void mv_cn_arashi3(ObjBean* this, PlayState* play) {
    s32 i;
    Vec3f itemDropPos;

    this->transformFunc(this);
    if (this->timer == 40) {
        pc_pon1_init(this);
    } else if (this->timer == 30) {
        if (!(this->stateFlags & BEAN_STATE_BEEN_WATERED)) {
            itemDropPos.x = this->dyna.actor.world.pos.x;
            itemDropPos.y = this->dyna.actor.world.pos.y - 25.0f;
            itemDropPos.z = this->dyna.actor.world.pos.z;
            for (i = 0; i < 3; i++) {
                Item_set0(play, &itemDropPos, ITEM00_FLEXIBLE);
            }
            this->stateFlags |= BEAN_STATE_BEEN_WATERED;
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_BUTTERFRY_TO_FAIRY);
            Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
        }
    } else if (this->timer <= 0) {
        mv_cn_arashi4_init(this);
    }
}

void mv_cn_arashi4_init(ObjBean* this) {
    this->actionFunc = mv_cn_arashi4;
    reset_disp_flag(this, BEAN_STATE_DRAW_SOIL | BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_STALK);
    pc_shibomu_init(this);
}

// Return control back to the player and start to shrink back down
void mv_cn_arashi4(ObjBean* this, PlayState* play) {
    this->transformFunc(this);
    this->stalkSizeMultiplier -= 0.001f;
    this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y + (s16)(this->stalkSizeMultiplier * 700000.0f);
    this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + (this->stalkSizeMultiplier * 800.0f);
    if (this->stalkSizeMultiplier <= 0.0f) {
        this->stalkSizeMultiplier = 0.0f;
        this->dyna.actor.shape.rot.y = this->dyna.actor.home.rot.y;
        mv_cn_arashi5_init(this);
    }
}

void mv_cn_arashi5_init(ObjBean* this) {
    this->actionFunc = mv_cn_arashi5;
    reset_disp_flag(this, BEAN_STATE_DRAW_LEAVES | BEAN_STATE_DRAW_SOIL);
    this->timer = 30;
}

void mv_cn_arashi5(ObjBean* this, PlayState* play) {
    this->transformFunc(this);
    if (this->timer <= 0) {
        mv_cn_mebae2_init(this);
        this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    }
}

void mv_ad_stop_init(ObjBean* this) {
    this->actionFunc = mv_ad_stop;
    reset_disp_flag(this, BEAN_STATE_DRAW_PLANT);
}

void mv_ad_stop(ObjBean* this, PlayState* play) {
    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        mv_ad_walk_init(this);
        if (play->sceneId == SCENE_LOST_WOODS) {
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_BEAN_LOST_WOODS);
        } else {
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_BEAN_GENERIC);
        }
    }
    fuwafuwaSwingNormal_bean(this);
}

void mv_ad_walk_init(ObjBean* this) {
    this->actionFunc = mv_ad_walk;
    reset_disp_flag(this, BEAN_STATE_DRAW_PLANT);
    this->dyna.actor.speed = 0.0f;
    this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED; // Never stop updating
}

void mv_ad_walk(ObjBean* this, PlayState* play) {
    Camera* mainCam;

    set_now_pos_lift_bean(this, play);
    if (this->currentPointIndex == this->pathCount) {
        init_path_parmeter_bean(this, play);
        init_now_pos_lift_bean(this, play);
        mv_ad_end_init(this);

        this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED; // Never stop updating (disable)
        mainCam = play->cameraPtrs[CAM_ID_MAIN];

        if ((mainCam->setting == CAM_SET_BEAN_LOST_WOODS) || (mainCam->setting == CAM_SET_BEAN_GENERIC)) {
            changeCameraSet(mainCam, CAM_SET_NORMAL0);
        }

    } else if (MoveBG_checkRidePlayerStatus(&this->dyna)) {

        Actor_level_SE_set(&this->dyna.actor, NA_SE_PL_PLANT_MOVE - SFX_FLAG);

        if (play->sceneId == SCENE_LOST_WOODS) {
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_BEAN_LOST_WOODS);
        } else {
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_BEAN_GENERIC);
        }
    } else if (this->stateFlags & BEAN_STATE_PLAYER_ON_TOP) {
        mainCam = play->cameraPtrs[CAM_ID_MAIN];

        if ((mainCam->setting == CAM_SET_BEAN_LOST_WOODS) || (mainCam->setting == CAM_SET_BEAN_GENERIC)) {
            changeCameraSet(mainCam, CAM_SET_NORMAL0);
        }
    }

    fuwafuwaSwingNormal_bean(this);
}

void mv_ad_end_init(ObjBean* this) {
    this->actionFunc = mv_ad_end;
    reset_disp_flag(this, BEAN_STATE_DRAW_PLANT);
}

void mv_ad_end(ObjBean* this, PlayState* play) {
    if (!MoveBG_checkOverPlayerStatus(&this->dyna)) {
        mv_ad_stop_init(this);
    }
    fuwafuwaSwingNormal_bean(this);
}

void mv_ad_revivalDropPlayer_init(ObjBean* this) {
    this->actionFunc = mv_ad_revivalDropPlayer;
    reset_disp_flag(this, 0);
}

void mv_ad_revivalDropPlayer(ObjBean* this, PlayState* play) {
    if (!MoveBG_checkRidePlayerStatus(&this->dyna)) {
        init_path_parmeter_bean(this, play);
        init_now_pos_lift_bean(this, play);
        set_world_pos_bean(this);
        mv_ad_revivalWait_init(this);
    }
}

void mv_ad_revivalWait_init(ObjBean* this) {
    this->actionFunc = mv_ad_revivalWait;
    reset_disp_flag(this, 0);
    this->timer = 100;
    fuwafuwaSwingScaleUp_bean_init(this);
}

void mv_ad_revivalWait(ObjBean* this, PlayState* play) {
    if (hit_horse_bean(this, play)) {
        this->timer = 100;
    } else if (this->timer <= 0) {
        mv_ad_revivalScaleUp_init(this);
    }
}

void mv_ad_revivalScaleUp_init(ObjBean* this) {
    this->actionFunc = mv_ad_revivalScaleUp;
    reset_disp_flag(this, BEAN_STATE_DRAW_PLANT);
    this->timer = 30;
}

void mv_ad_revivalScaleUp(ObjBean* this, PlayState* play) {
    s32 trampled = hit_horse_bean(this, play);

    fuwafuwaSwingScaleUp_bean(this);
    if (trampled) {
        DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    } else {
        DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    }
    if ((this->timer <= 0) && (!trampled)) {
        fuwafuwaSwingNormal_bean_init(this);
        mv_ad_stop_init(this);
    }
}
void Obj_Bean_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjBean* this = (ObjBean*)thisx;

    if (this->timer > 0) {
        this->timer--;
    }

    this->actionFunc(this, play);

    if (this->stateFlags & BEAN_STATE_DRAW_PLANT) {
        set_world_pos_bean(this);
        if (this->dyna.actor.xzDistToPlayer < 150.0f) {
            this->collider.dim.radius = this->dyna.actor.scale.x * 640.0f + 0.5f;
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->collider);
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        }

        set_ground_y_bean(this, play);

        this->dyna.actor.shape.shadowDraw = Actor_shadow_circle;
        this->dyna.actor.shape.shadowScale = this->dyna.actor.scale.x * 88.0f;

        if (hit_horse_bean(this, play)) {
            PRINTF_COLOR_CYAN();
            // "Horse and bean tree lift collision"
            PRINTF("馬と豆の木リフト衝突！！！\n");
            PRINTF_RST();
            effect_bean_lift(this, play);
            DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
            mv_ad_revivalDropPlayer_init(this);
        }
    } else {
        this->dyna.actor.shape.shadowDraw = NULL;
    }
    Actor_world_to_eye(&this->dyna.actor, 6.0f);
    if (this->stateFlags & BEAN_STATE_DYNAPOLY_SET) {
        if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
            this->stateFlags |= BEAN_STATE_PLAYER_ON_TOP;
        } else {
            this->stateFlags &= ~BEAN_STATE_PLAYER_ON_TOP;
        }
    }
}

void bean_draw_place(ObjBean* this, PlayState* play) {
    Matrix_translate(this->dyna.actor.home.pos.x, this->dyna.actor.home.pos.y, this->dyna.actor.home.pos.z,
                     MTXMODE_NEW);
    Matrix_rotateY(BINANG_TO_RAD(this->dyna.actor.home.rot.y), MTXMODE_APPLY);
    Matrix_scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    Cheap_gfx_display(play, gMagicBeanSoftSoilDL);
}

void bean_draw_kuki(ObjBean* this, PlayState* play) {
    Matrix_translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_rotateY(BINANG_TO_RAD(this->dyna.actor.shape.rot.y), MTXMODE_APPLY);
    Matrix_scale(0.1f, this->stalkSizeMultiplier, 0.1f, MTXMODE_APPLY);
    Cheap_gfx_display(play, gMagicBeanStemDL);
}

void Obj_Bean_actor_draw(Actor* thisx, PlayState* play) {
    ObjBean* this = (ObjBean*)thisx;

    if (this->stateFlags & BEAN_STATE_DRAW_SOIL) {
        Cheap_gfx_display(play, gMagicBeanSeedlingDL);
    }
    if (this->stateFlags & BEAN_STATE_DRAW_PLANT) {
        Cheap_gfx_display(play, gMagicBeanPlatformDL);
    }
    if (this->stateFlags & BEAN_STATE_DRAW_LEAVES) {
        bean_draw_place(this, play);
    }
    if (this->stateFlags & BEAN_STATE_DRAW_STALK) {
        bean_draw_kuki(this, play);
    }
}
