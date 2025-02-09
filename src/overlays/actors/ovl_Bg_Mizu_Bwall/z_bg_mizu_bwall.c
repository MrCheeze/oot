/*
 * File: z_bg_mizu_bwall.c
 * Overlay: ovl_Bg_Mizu_Bwall
 * Description: Water Temple bombable walls
 */

#include "z_bg_mizu_bwall.h"
#include "overlays/actors/ovl_Bg_Mizu_Water/z_bg_mizu_water.h"
#include "assets/objects/object_mizu_objects/object_mizu_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Mizu_Bwall_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mizu_Bwall_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mizu_Bwall_actor_move(Actor* thisx, PlayState* play);
void Bg_Mizu_Bwall_actor_draw(Actor* thisx, PlayState* play2);

void mode_bomb_before(BgMizuBwall* this, PlayState* play);
void mode_bomb_wait(BgMizuBwall* this, PlayState* play);
void mode_bomb_after(BgMizuBwall* this, PlayState* play);

ActorProfile Bg_Mizu_Bwall_Profile = {
    /**/ ACTOR_BG_MIZU_BWALL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MIZU_OBJECTS,
    /**/ sizeof(BgMizuBwall),
    /**/ Bg_Mizu_Bwall_actor_ct,
    /**/ Bg_Mizu_Bwall_actor_dt,
    /**/ Bg_Mizu_Bwall_actor_move,
    /**/ Bg_Mizu_Bwall_actor_draw,
};

static ColliderTrisElementInit BWL03_ClObjTrisElemDt_base[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -40.0f, 0.0f, -40.0f }, { -40.0f, 0.0f, 40.0f }, { 40.0f, 0.0f, 40.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -40.0f, 0.0f, -40.0f }, { 40.0f, 0.0f, 40.0f }, { 40.0f, 0.0f, -40.0f } } },
    },
};

static ColliderTrisInit BWL03_ClObjTrisDt_base = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    BWL03_ClObjTrisElemDt_base,
};

static ColliderTrisElementInit BWL11_ClObjTrisElemDt_base[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 116.0f, 0.0f }, { 0.0f, 0.0f, 70.0f }, { 0.0f, 0.0f, -70.0f } } },
    },
};

static ColliderTrisInit BWL11_ClObjTrisDt_base = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    1,
    BWL11_ClObjTrisElemDt_base,
};

static ColliderTrisElementInit BWL0E_ClObjTrisElemDt_base[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 120.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 60.0f, 0.0f, 0.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000008, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 120.0f, 0.0f }, { 60.0f, 0.0f, 0.0f }, { 60.0f, 120.0f, 0.0f } } },
    },
};

static ColliderTrisInit BWL0EE_ClObjTrisDt_base = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    BWL0E_ClObjTrisElemDt_base,
};

static ColliderTrisInit BWL0EW_ClObjTrisDt_base = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    BWL0E_ClObjTrisElemDt_base,
};

static Gfx* shape_model[] = {
    gObjectMizuObjectsBwallDL_001A30, gObjectMizuObjectsBwallDL_002390, gObjectMizuObjectsBwallDL_001CD0,
    gObjectMizuObjectsBwallDL_002090, gObjectMizuObjectsBwallDL_001770,
};
static CollisionHeader* pbgdata_from_data[] = {
    &gObjectMizuObjectsBwallCol_001C58, &gObjectMizuObjectsBwallCol_0025A4, &gObjectMizuObjectsBwallCol_001DE8,
    &gObjectMizuObjectsBwallCol_001DE8, &gObjectMizuObjectsBwallCol_001DE8,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_rotY_bombwall(Vec3f* out, Vec3f* in, f32 sin, f32 cos) {
    out->x = (in->z * sin) + (in->x * cos);
    out->y = in->y;
    out->z = (in->z * cos) - (in->x * sin);
}

void Bg_Mizu_Bwall_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMizuBwall* this = (BgMizuBwall*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    this->yRot = this->dyna.actor.world.pos.y;
    this->dList = shape_model[PARAMS_GET_U((u16)this->dyna.actor.params, 0, 4)];
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyUty_bgdi_SG2KSG(pbgdata_from_data[PARAMS_GET_U((u16)this->dyna.actor.params, 0, 4)], &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    switch (PARAMS_GET_U((u16)this->dyna.actor.params, 0, 4)) {
        case MIZUBWALL_FLOOR: {
            f32 sin;
            f32 cos;
            s32 i;
            s32 j;
            Vec3f offset;
            Vec3f vtx[3];

            if (Actor_Environment_sw_Check(play, PARAMS_GET_U((u16)this->dyna.actor.params, 8, 6))) {
                DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
                this->dList = NULL;
                this->actionFunc = mode_bomb_after;
            } else {
                ClObjTris_ct(play, &this->collider);
                if (!ClObjTris_set5_nzm(play, &this->collider, &this->dyna.actor, &BWL03_ClObjTrisDt_base, this->elements)) {
                    PRINTF("Error : コリジョンデータセット失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_mizu_bwall.c", 484,
                           this->dyna.actor.params);
                    Actor_delete(&this->dyna.actor);
                } else {
                    sin = sin_s(this->dyna.actor.shape.rot.y);
                    cos = cos_s(this->dyna.actor.shape.rot.y);

                    for (i = 0; i < ARRAY_COUNT(BWL03_ClObjTrisElemDt_base); i++) {
                        for (j = 0; j < 3; j++) {
                            offset.x = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].x;
                            offset.y = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].y;
                            offset.z = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].z + 2.0f;
                            func_rotY_bombwall(&vtx[j], &offset, sin, cos);
                            vtx[j].x += this->dyna.actor.world.pos.x;
                            vtx[j].y += this->dyna.actor.world.pos.y;
                            vtx[j].z += this->dyna.actor.world.pos.z;
                        }
                        CollisionCheck_Uty_setTrisPos(&this->collider, i, &vtx[0], &vtx[1], &vtx[2]);
                    }
                    this->actionFunc = mode_bomb_before;
                }
            }
            break;
        }
        case MIZUBWALL_RUTO_ROOM: {
            f32 sin;
            f32 cos;
            s32 i;
            s32 j;
            Vec3f offset;
            Vec3f vtx[3];

            if (Actor_Environment_sw_Check(play, PARAMS_GET_U((u16)this->dyna.actor.params, 8, 6))) {
                DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
                this->dList = NULL;
                this->actionFunc = mode_bomb_after;
            } else {
                ClObjTris_ct(play, &this->collider);
                if (!ClObjTris_set5_nzm(play, &this->collider, &this->dyna.actor, &BWL11_ClObjTrisDt_base, this->elements)) {
                    PRINTF("Error : コリジョンデータセット失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_mizu_bwall.c", 558,
                           this->dyna.actor.params);
                    Actor_delete(&this->dyna.actor);
                } else {
                    sin = sin_s(this->dyna.actor.shape.rot.y);
                    cos = cos_s(this->dyna.actor.shape.rot.y);

                    for (i = 0; i < ARRAY_COUNT(BWL11_ClObjTrisElemDt_base); i++) {
                        for (j = 0; j < 3; j++) {
                            offset.x = BWL11_ClObjTrisDt_base.elements[i].dim.vtx[j].x;
                            offset.y = BWL11_ClObjTrisDt_base.elements[i].dim.vtx[j].y;
                            offset.z = BWL11_ClObjTrisDt_base.elements[i].dim.vtx[j].z + 2.0f;
                            func_rotY_bombwall(&vtx[j], &offset, sin, cos);
                            vtx[j].x += this->dyna.actor.world.pos.x;
                            vtx[j].y += this->dyna.actor.world.pos.y;
                            vtx[j].z += this->dyna.actor.world.pos.z;
                        }
                        CollisionCheck_Uty_setTrisPos(&this->collider, i, &vtx[0], &vtx[1], &vtx[2]);
                    }
                    this->actionFunc = mode_bomb_before;
                }
            }
            break;
        }
        case MIZUBWALL_UNUSED: {
            f32 sin;
            f32 cos;
            s32 i;
            s32 j;
            Vec3f offset;
            Vec3f vtx[3];

            if (Actor_Environment_sw_Check(play, PARAMS_GET_U((u16)this->dyna.actor.params, 8, 6))) {
                DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
                this->dList = NULL;
                this->actionFunc = mode_bomb_after;
            } else {
                ClObjTris_ct(play, &this->collider);
                if (!ClObjTris_set5_nzm(play, &this->collider, &this->dyna.actor, &BWL0EE_ClObjTrisDt_base, this->elements)) {
                    PRINTF("Error : コリジョンデータセット失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_mizu_bwall.c", 638,
                           this->dyna.actor.params);
                    Actor_delete(&this->dyna.actor);
                } else {
                    sin = sin_s(this->dyna.actor.shape.rot.y);
                    cos = cos_s(this->dyna.actor.shape.rot.y);

                    for (i = 0; i < ARRAY_COUNT(BWL03_ClObjTrisElemDt_base); i++) {
                        for (j = 0; j < 3; j++) {
                            //! @bug This uses the wrong set of collision triangles, causing the collider to be
                            //!      flat to the ground instead of vertical. It should use BWL0EE_ClObjTrisDt_base.
                            offset.x = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].x;
                            offset.y = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].y;
                            offset.z = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].z;
                            func_rotY_bombwall(&vtx[j], &offset, sin, cos);
                            vtx[j].x += this->dyna.actor.world.pos.x;
                            vtx[j].y += this->dyna.actor.world.pos.y;
                            vtx[j].z += this->dyna.actor.world.pos.z;
                        }
                        CollisionCheck_Uty_setTrisPos(&this->collider, i, &vtx[0], &vtx[1], &vtx[2]);
                    }
                    this->actionFunc = mode_bomb_before;
                }
            }
            break;
        }
        case MIZUBWALL_STINGER_ROOM_1: {
            f32 sin;
            f32 cos;
            s32 i;
            s32 j;
            Vec3f offset;
            Vec3f vtx[3];

            if (Actor_Environment_sw_Check(play, PARAMS_GET_U((u16)this->dyna.actor.params, 8, 6))) {
                DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
                this->dList = NULL;
                this->actionFunc = mode_bomb_after;
            } else {
                ClObjTris_ct(play, &this->collider);
                if (!ClObjTris_set5_nzm(play, &this->collider, &this->dyna.actor, &BWL0EW_ClObjTrisDt_base,
                                      this->elements)) {
                    PRINTF("Error : コリジョンデータセット失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_mizu_bwall.c", 724,
                           this->dyna.actor.params);
                    Actor_delete(&this->dyna.actor);
                } else {
                    sin = sin_s(this->dyna.actor.shape.rot.y);
                    cos = cos_s(this->dyna.actor.shape.rot.y);

                    for (i = 0; i < ARRAY_COUNT(BWL03_ClObjTrisElemDt_base); i++) {
                        for (j = 0; j < 3; j++) {
                            //! @bug This uses the wrong set of collision triangles, causing the collider to be
                            //!      flat to the ground instead of vertical. It should use BWL0EW_ClObjTrisDt_base.
                            offset.x = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].x;
                            offset.y = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].y;
                            offset.z = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].z + 2.0f;
                            func_rotY_bombwall(&vtx[j], &offset, sin, cos);
                            vtx[j].x += this->dyna.actor.world.pos.x;
                            vtx[j].y += this->dyna.actor.world.pos.y;
                            vtx[j].z += this->dyna.actor.world.pos.z;
                        }
                        CollisionCheck_Uty_setTrisPos(&this->collider, i, &vtx[0], &vtx[1], &vtx[2]);
                    }
                    this->actionFunc = mode_bomb_before;
                }
            }
            break;
        }
        case MIZUBWALL_STINGER_ROOM_2: {
            f32 sin;
            f32 cos;
            s32 i;
            s32 j;
            Vec3f offset;
            Vec3f vtx[3];

            if (Actor_Environment_sw_Check(play, PARAMS_GET_U((u16)this->dyna.actor.params, 8, 6))) {
                DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
                this->dList = NULL;
                this->actionFunc = mode_bomb_after;
            } else {
                ClObjTris_ct(play, &this->collider);
                if (!ClObjTris_set5_nzm(play, &this->collider, &this->dyna.actor, &BWL0EW_ClObjTrisDt_base,
                                      this->elements)) {
                    PRINTF("Error : コリジョンデータセット失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_mizu_bwall.c", 798,
                           this->dyna.actor.params);
                    Actor_delete(&this->dyna.actor);
                } else {
                    sin = sin_s(this->dyna.actor.shape.rot.y);
                    cos = cos_s(this->dyna.actor.shape.rot.y);

                    for (i = 0; i < ARRAY_COUNT(BWL03_ClObjTrisElemDt_base); i++) {
                        for (j = 0; j < 3; j++) {
                            //! @bug This uses the wrong set of collision triangles, causing the collider to be
                            //!      flat to the ground instead of vertical. It should use BWL0EW_ClObjTrisDt_base.
                            offset.x = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].x;
                            offset.y = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].y;
                            offset.z = BWL03_ClObjTrisDt_base.elements[i].dim.vtx[j].z + 2.0f;
                            func_rotY_bombwall(&vtx[j], &offset, sin, cos);
                            vtx[j].x += this->dyna.actor.world.pos.x;
                            vtx[j].y += this->dyna.actor.world.pos.y;
                            vtx[j].z += this->dyna.actor.world.pos.z;
                        }
                        CollisionCheck_Uty_setTrisPos(&this->collider, i, &vtx[0], &vtx[1], &vtx[2]);
                    }
                    this->actionFunc = mode_bomb_before;
                }
            }
            break;
        }
    }
}

void Bg_Mizu_Bwall_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMizuBwall* this = (BgMizuBwall*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjTris_dt_nzf(play, &this->collider);
}

static void func_two_tile_control(BgMizuBwall* this, PlayState* play) {
    WaterBox* waterBoxes = play->colCtx.colHeader->waterBoxes;
    f32 waterLevel = waterBoxes[2].ySurface;

    if (waterLevel < WATER_TEMPLE_WATER_F1_Y) {
        this->scrollAlpha1 = 255;
    } else if (waterLevel < WATER_TEMPLE_WATER_F2_Y) {
        this->scrollAlpha1 = 255 - (s32)((waterLevel - WATER_TEMPLE_WATER_F1_Y) /
                                         (WATER_TEMPLE_WATER_F2_Y - WATER_TEMPLE_WATER_F1_Y) * (255 - 160));
    } else {
        this->scrollAlpha1 = 160;
    }

    if (waterLevel < WATER_TEMPLE_WATER_F2_Y) {
        this->scrollAlpha2 = 255;
    } else if (waterLevel < WATER_TEMPLE_WATER_F3_Y) {
        this->scrollAlpha2 = 255 - (s32)((waterLevel - WATER_TEMPLE_WATER_F2_Y) /
                                         (WATER_TEMPLE_WATER_F3_Y - WATER_TEMPLE_WATER_F2_Y) * (255 - 160));
    } else {
        this->scrollAlpha2 = 160;
    }

    if (waterLevel < WATER_TEMPLE_WATER_B1_Y) {
        this->scrollAlpha3 = 255;
    } else if (waterLevel < WATER_TEMPLE_WATER_F1_Y) {
        this->scrollAlpha3 = 255 - (s32)((waterLevel - WATER_TEMPLE_WATER_B1_Y) /
                                         (WATER_TEMPLE_WATER_F1_Y - WATER_TEMPLE_WATER_B1_Y) * (255 - 160));
    } else {
        this->scrollAlpha3 = 160;
    }

    this->scrollAlpha4 = this->scrollAlpha3;
}

void func_eff_bombwall(BgMizuBwall* this, PlayState* play) {
    s32 i;
    s32 pad;
    s16 rand1;
    s16 rand2;
    Vec3f* thisPos = &this->dyna.actor.world.pos;
    Vec3f debrisPos;
    f32 tempx;
    f32 tempz;
    f32 sin = sin_s(this->dyna.actor.shape.rot.y);
    f32 cos = cos_s(this->dyna.actor.shape.rot.y);
    Vec3f debrisOffsets[15];

    for (i = 0; i < ARRAY_COUNT(debrisOffsets); i++) {
        switch (PARAMS_GET_U((u16)this->dyna.actor.params, 0, 4)) {
            case MIZUBWALL_FLOOR:
                debrisOffsets[i].x = (fqrand() * 80.0f) - 40.0f;
                debrisOffsets[i].y = fqrand() * 0;
                debrisOffsets[i].z = (fqrand() * 80.0f) - 40.0f;
                break;
            case MIZUBWALL_RUTO_ROOM:
                debrisOffsets[i].x = fqrand() * 0;
                debrisOffsets[i].y = fqrand() * 100.0f;
                debrisOffsets[i].z = (fqrand() * 80.0f) - 40.0f;
                break;
            case MIZUBWALL_UNUSED:
            case MIZUBWALL_STINGER_ROOM_1:
            default:
                debrisOffsets[i].x = (fqrand() * 120) - 60.0f;
                debrisOffsets[i].y = fqrand() * 120;
                debrisOffsets[i].z = fqrand() * 0;
                break;
        }
    }

    for (i = 0; i < ARRAY_COUNT(debrisOffsets); i++) {
        tempx = debrisOffsets[i].x;
        tempz = debrisOffsets[i].z;

        debrisPos.x = thisPos->x + tempz * sin + tempx * cos;
        debrisPos.y = thisPos->y + debrisOffsets[i].y;
        debrisPos.z = thisPos->z + tempz * cos - tempx * sin;

        rand1 = (s16)(fqrand() * 120.0f) + 20;
        rand2 = (s16)(fqrand() * 240.0f) + 20;
        dust_fly_set2(play, &debrisPos, 50.0f, 2, rand1, rand2, 0);
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_A_OBJ, debrisPos.x, debrisPos.y, debrisPos.z, 0, 0, 0, 0xB);
    }
}

void mode_bomb_before(BgMizuBwall* this, PlayState* play) {
    func_two_tile_control(this, play);
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_Environment_sw_On(play, PARAMS_GET_U((u16)this->dyna.actor.params, 8, 6));
        this->breakTimer = 1;
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        this->dList = NULL;
        func_eff_bombwall(this, play);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_WALL_BROKEN);
        Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->actionFunc = mode_bomb_wait;
    } else if (this->dyna.actor.xzDistToPlayer < 600.0f) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void mode_bomb_wait(BgMizuBwall* this, PlayState* play) {
    if (this->breakTimer > 0) {
        this->breakTimer--;
    } else {
        this->actionFunc = mode_bomb_after;
    }
}

void mode_bomb_after(BgMizuBwall* this, PlayState* play) {
}

void Bg_Mizu_Bwall_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMizuBwall* this = (BgMizuBwall*)thisx;

    this->actionFunc(this, play);
}

void Bg_Mizu_Bwall_actor_draw(Actor* thisx, PlayState* play2) {
    BgMizuBwall* this = (BgMizuBwall*)thisx;
    PlayState* play = play2;
    u32 frames;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_mizu_bwall.c", 1095);
    frames = play->gameplayFrames;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08,
               two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, 1 * frames, 0, 0x20, 0x20, 1, 0, 0, 0x20,
                                        0x20, 0, 0, 0, this->scrollAlpha1));
    gSPSegment(POLY_OPA_DISP++, 0x09,
               two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, 1 * frames, 0, 0x20, 0x20, 1, 0, 0, 0x20,
                                        0x20, 0, 0, 0, this->scrollAlpha2));
    gSPSegment(POLY_OPA_DISP++, 0x0A,
               two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, 1 * frames, 0, 0x20, 0x20, 1, 0, 0, 0x20,
                                        0x20, 0, 0, 0, this->scrollAlpha3));
    gSPSegment(POLY_OPA_DISP++, 0x0B,
               two_tex_scroll_env(play->state.gfxCtx, G_TX_RENDERTILE, 3 * frames, 0, 0x20, 0x20, 1, 0, 0, 0x20,
                                        0x20, 0, 0, 0, this->scrollAlpha4));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mizu_bwall.c", 1129);

    if (this->dList != NULL) {
        gSPDisplayList(POLY_OPA_DISP++, this->dList);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_mizu_bwall.c", 1136);
}
