/*
 * File: z_bg_ydan_sp.c
 * Overlay: ovl_Bg_Ydan_Sp
 * Description: Webs
 */

#include "z_bg_ydan_sp.h"
#include "assets/objects/object_ydan_objects/object_ydan_objects.h"

#define FLAGS 0

void Bg_Ydan_Sp_actor_ct(Actor* thisx, PlayState* play);
void Bg_Ydan_Sp_actor_dt(Actor* thisx, PlayState* play);
void Bg_Ydan_Sp_actor_move(Actor* thisx, PlayState* play);
void Bg_Ydan_Sp_actor_draw(Actor* thisx, PlayState* play);

void mode_floor_burn(BgYdanSp* this, PlayState* play);
static void mode_floor_wait(BgYdanSp* this, PlayState* play);
void mode_wall_burn(BgYdanSp* this, PlayState* play);
static void mode_wall_wait(BgYdanSp* this, PlayState* play);

typedef enum BgYdanSpType {
    /* 0 */ WEB_FLOOR,
    /* 1 */ WEB_WALL
} BgYdanSpType;

ActorProfile Bg_Ydan_Sp_Profile = {
    /**/ ACTOR_BG_YDAN_SP,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_YDAN_OBJECTS,
    /**/ sizeof(BgYdanSp),
    /**/ Bg_Ydan_Sp_actor_ct,
    /**/ Bg_Ydan_Sp_actor_dt,
    /**/ Bg_Ydan_Sp_actor_move,
    /**/ Bg_Ydan_Sp_actor_draw,
};

static ColliderTrisElementInit YdanSpAcTrisElemData[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x00 },
            { 0x00020800, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 75.0f, -8.0f, 75.0f }, { -75.0f, -8.0f, 75.0f }, { -75.0f, -8.0f, -75.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x00 },
            { 0x00020800, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 140.0f, 288.8f, 0.0f }, { -140.0f, 288.0f, 0.0f }, { -140.0f, 0.0f, 0.0f } } },
    },
};

static ColliderTrisInit YdanSpAcTrisData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    YdanSpAcTrisElemData,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Ydan_Sp_actor_ct(Actor* thisx, PlayState* play) {
    BgYdanSp* this = (BgYdanSp*)thisx;
    ColliderTrisElementInit* ti0 = &YdanSpAcTrisElemData[0];
    Vec3f tri[3];
    s32 i;
    CollisionHeader* colHeader = NULL;
    ColliderTrisElementInit* ti1 = &YdanSpAcTrisElemData[1];
    f32 cossY;
    f32 sinsY;
    f32 cossX;
    f32 nSinsX;

    ValueSet_process(&this->dyna.actor, value_init);
    this->isDestroyedSwitchFlag = PARAMS_GET_U(thisx->params, 0, 6);
    this->burnSwitchFlag = PARAMS_GET_U(thisx->params, 6, 6);
    this->dyna.actor.params = PARAMS_GET_U(thisx->params, 12, 4);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    ClObjTris_ct(play, &this->trisCollider);
    ClObjTris_set5_nzm(play, &this->trisCollider, &this->dyna.actor, &YdanSpAcTrisData, this->trisColliderItems);
    if (this->dyna.actor.params == WEB_FLOOR) {
        DynaPolyUty_bgdi_SG2KSG(&gDTWebFloorCol, &colHeader);
        this->actionFunc = mode_floor_wait;

        for (i = 0; i < 3; i++) {
            tri[i].x = ti0->dim.vtx[i].x + this->dyna.actor.world.pos.x;
            tri[i].y = ti0->dim.vtx[i].y + this->dyna.actor.world.pos.y;
            tri[i].z = ti0->dim.vtx[i].z + this->dyna.actor.world.pos.z;
        }

        CollisionCheck_Uty_setTrisPos(&this->trisCollider, 0, &tri[0], &tri[1], &tri[2]);
        tri[1].x = tri[0].x;
        tri[1].z = tri[2].z;
        CollisionCheck_Uty_setTrisPos(&this->trisCollider, 1, &tri[0], &tri[2], &tri[1]);
        this->unk_16C = 0.0f;
    } else {
        DynaPolyUty_bgdi_SG2KSG(&gDTWebWallCol, &colHeader);
        this->actionFunc = mode_wall_wait;
        Actor_world_to_eye(&this->dyna.actor, 30.0f);
        sinsY = sin_s(this->dyna.actor.shape.rot.y);
        cossY = cos_s(this->dyna.actor.shape.rot.y);
        nSinsX = -sin_s(this->dyna.actor.shape.rot.x);
        cossX = cos_s(this->dyna.actor.shape.rot.x);

        for (i = 0; i < 3; i++) {
            tri[i].x =
                this->dyna.actor.world.pos.x + (cossY * ti1->dim.vtx[i].x) - (sinsY * ti1->dim.vtx[i].y * nSinsX);
            tri[i].y = this->dyna.actor.world.pos.y + (ti1->dim.vtx[i].y * cossX);
            tri[i].z =
                this->dyna.actor.world.pos.z - (sinsY * ti1->dim.vtx[i].x) + (ti1->dim.vtx[i].y * cossY * nSinsX);
        }

        CollisionCheck_Uty_setTrisPos(&this->trisCollider, 0, &tri[0], &tri[1], &tri[2]);

        tri[1].x = this->dyna.actor.world.pos.x + (cossY * ti1->dim.vtx[0].x) - (ti1->dim.vtx[2].y * sinsY * nSinsX);
        tri[1].y = this->dyna.actor.world.pos.y + (ti1->dim.vtx[2].y * cossX);
        tri[1].z = this->dyna.actor.world.pos.z - (sinsY * ti1->dim.vtx[0].x) + (ti1->dim.vtx[2].y * cossY * nSinsX);
        CollisionCheck_Uty_setTrisPos(&this->trisCollider, 1, &tri[0], &tri[2], &tri[1]);
    }
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->timer = 0;
    if (Actor_Environment_sw_Check(play, this->isDestroyedSwitchFlag)) {
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Ydan_Sp_actor_dt(Actor* thisx, PlayState* play) {
    BgYdanSp* this = (BgYdanSp*)thisx;
    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjTris_dt_nzf(play, &this->trisCollider);
}

void ydan_floor_bg_vtx_move(BgYdanSp* this) {
    s16 newY;
    CollisionHeader* colHeader;

    colHeader = SEGMENTED_TO_VIRTUAL(&gDTWebFloorCol);
    colHeader->vtxList = SEGMENTED_TO_VIRTUAL(colHeader->vtxList);
    newY = (this->dyna.actor.home.pos.y - this->dyna.actor.world.pos.y) * 10;
    colHeader->vtxList[14].y = newY;
    colHeader->vtxList[12].y = newY;
    colHeader->vtxList[10].y = newY;
    colHeader->vtxList[9].y = newY;
    colHeader->vtxList[6].y = newY;
    colHeader->vtxList[5].y = newY;
    colHeader->vtxList[1].y = newY;
    colHeader->vtxList[0].y = newY;
}

void mode_burn_init(BgYdanSp* this, PlayState* play) {
    this->timer = 30;
    Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
    Actor_Environment_sw_On(play, this->isDestroyedSwitchFlag);
    if (this->dyna.actor.params == WEB_FLOOR) {
        this->actionFunc = mode_floor_burn;
    } else {
        this->actionFunc = mode_wall_burn;
    }
}

void mode_floor_burn(BgYdanSp* this, PlayState* play) {
    static Vec3f zero_vec = { 0 };
    Vec3f velocity;
    Vec3f pos2;
    f32 distXZ;
    f32 sins;
    f32 coss;
    s16 rot;
    s16 rot2;
    s32 i;

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        Actor_delete(&this->dyna.actor);
        return;
    }
    if ((this->timer % 3) == 0) {
        rot2 = fqrand() * 0x2AAA;
        velocity.y = 0.0f;
        pos2.y = this->dyna.actor.world.pos.y;

        for (i = 0; i < 6; i++) {
            rot = rnd_fx(0x2800) + rot2;
            sins = sin_s(rot);
            coss = cos_s(rot);
            pos2.x = this->dyna.actor.world.pos.x + (120.0f * sins);
            pos2.z = this->dyna.actor.world.pos.z + (120.0f * coss);
            distXZ = search_position_distanceXZ(&this->dyna.actor.home.pos, &pos2) * (1.0f / 120.0f);
            if (distXZ < 0.7f) {
                sins = sin_s(rot + 0x8000);
                coss = cos_s(rot + 0x8000);
                pos2.x = this->dyna.actor.world.pos.x + (120.0f * sins);
                pos2.z = this->dyna.actor.world.pos.z + (120.0f * coss);
                distXZ = search_position_distanceXZ(&this->dyna.actor.home.pos, &pos2) * (1.0f / 120.0f);
            }
            velocity.x = (7.0f * sins) * distXZ;
            velocity.y = 0.0f;
            velocity.z = (7.0f * coss) * distXZ;
            _Effect_SS_Db_ct(play, &this->dyna.actor.home.pos, &velocity, &zero_vec, 60, 6, 255, 255, 150, 170, 255, 0,
                                 0, 1, 0xE, 1);
            rot2 += 0x2AAA;
        }
    }
}

void mode_floor_break(BgYdanSp* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        Actor_delete(&this->dyna.actor);
    }
}

void mode_floor_before(BgYdanSp* this, PlayState* play) {
    static Color_RGBA8 prim = { 250, 250, 250, 255 };
    static Color_RGBA8 env = { 180, 180, 180, 255 };
    static Vec3f zero_vec = { 0 };
    s32 i;
    Vec3f pos;
    s16 rot;

    if (this->timer != 0) {
        this->timer--;
    }

    this->dyna.actor.world.pos.y = (sinf((f32)this->timer * (M_PI / 20)) * this->unk_16C) + this->dyna.actor.home.pos.y;
    if (this->dyna.actor.home.pos.y - this->dyna.actor.world.pos.y > 190.0f) {
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        this->timer = 40;
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        Actor_Environment_sw_On(play, this->isDestroyedSwitchFlag);
        this->actionFunc = mode_floor_break;
        pos.y = this->dyna.actor.world.pos.y - 60.0f;
        rot = 0;
        for (i = 0; i < 6; i++) {
            pos.x = sin_s(rot) * 60.0f + this->dyna.actor.world.pos.x;
            pos.z = cos_s(rot) * 60.0f + this->dyna.actor.world.pos.z;
            Effect_SS_Dust_sc_cl_ct(play, &pos, &zero_vec, &zero_vec, &prim, &env, 1000, 10);

            rot += 0x2AAA;
        }
    }
    ydan_floor_bg_vtx_move(this);
}

static void mode_floor_wait(BgYdanSp* this, PlayState* play) {
    Player* player;
    Vec3f webPos;
    f32 sqrtFallDistance;
    f32 unk;

    player = GET_PLAYER(play);
    webPos.x = this->dyna.actor.world.pos.x;
    webPos.y = this->dyna.actor.world.pos.y - 50.0f;
    webPos.z = this->dyna.actor.world.pos.z;
    if (player_fire_stick_hit_check(play, &webPos, 70.0f, 50.0f) != 0) {
        this->dyna.actor.home.pos.x = player->meleeWeaponInfo[0].tip.x;
        this->dyna.actor.home.pos.z = player->meleeWeaponInfo[0].tip.z;
        mode_burn_init(this, play);
        return;
    }
    if (this->trisCollider.base.acFlags & AC_HIT) {
        mode_burn_init(this, play);
        return;
    }
    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        sqrtFallDistance = sqrtf(CLAMP_MIN(player->fallDistance, 0.0f));
        if (player->fallDistance > 750.0f) {
            if (this->dyna.actor.xzDistToPlayer < 80.0f) {
                this->unk_16C = 200.0f;
                this->dyna.actor.room = -1;
                this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
                this->timer = 40;
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_WEB_BROKEN);
                this->actionFunc = mode_floor_before;
                return;
            }
        }
        unk = sqrtFallDistance + sqrtFallDistance;
        if (this->unk_16C < unk) {
            if (unk > 2.0f) {
                this->unk_16C = unk;
                this->timer = 14;
            }
        }
        if (player->actor.speed != 0.0f) {
            if (this->unk_16C < 0.1f) {
                this->timer = 14;
            }
            this->unk_16C = CLAMP_MIN(this->unk_16C, 2.0f);
        }
    }
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        this->timer = 14;
    }
    this->dyna.actor.world.pos.y = sinf((f32)this->timer * (M_PI / 7)) * this->unk_16C + this->dyna.actor.home.pos.y;
    add_calc0(&this->unk_16C, 1.0f, 0.8f);
    if (this->timer == 13) {
        if (this->unk_16C > 3.0f) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_WEB_VIBRATION);
        } else {
            Nai_StopFx(NA_SE_EV_WEB_VIBRATION);
        }
    }
    ydan_floor_bg_vtx_move(this);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->trisCollider.base);
}

void mode_wall_burn(BgYdanSp* this, PlayState* play) {
    static Vec3f zero_vec = { 0 };
    Vec3f velocity;
    Vec3f spC8;
    f32 distXYZ;
    f32 sins;
    f32 coss;
    f32 coss2;
    s16 rot;
    s16 rot2;
    s32 i;

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        Actor_delete(&this->dyna.actor);
        return;
    }
    if ((this->timer % 3) == 0) {
        rot2 = fqrand() * 0x2AAA;

        for (i = 0; i < 6; i++) {
            rot = rnd_fx(0x2800) + rot2;
            sins = sin_s(rot);
            coss = cos_s(rot);
            coss2 = cos_s(this->dyna.actor.shape.rot.y) * sins;
            sins *= sin_s(this->dyna.actor.shape.rot.y);

            spC8.x = this->dyna.actor.world.pos.x + (140.0f * coss2);
            spC8.y = this->dyna.actor.world.pos.y + (140.0f * (1.0f + coss));
            spC8.z = this->dyna.actor.world.pos.z - (140.0f * sins);
            distXYZ = search_position_distance(&this->dyna.actor.home.pos, &spC8) * (1.0f / 140.0f);
            if (distXYZ < 0.65f) {
                sins = sin_s(rot + 0x8000);
                coss = cos_s(rot + 0x8000);
                coss2 = cos_s(this->dyna.actor.shape.rot.y) * sins;
                sins *= sin_s(this->dyna.actor.shape.rot.y);
                spC8.x = this->dyna.actor.world.pos.x + (140.0f * coss2);
                spC8.y = this->dyna.actor.world.pos.y + (140.0f * (1.0f + coss));
                spC8.z = this->dyna.actor.world.pos.z - (140.0f * sins);
                distXYZ = search_position_distance(&this->dyna.actor.home.pos, &spC8) * (1.0f / 140.0f);
            }
            velocity.x = 6.5f * coss2 * distXYZ;
            velocity.y = 6.5f * coss * distXYZ;
            velocity.z = -6.5f * sins * distXYZ;
            _Effect_SS_Db_ct(play, &this->dyna.actor.home.pos, &velocity, &zero_vec, 80, 6, 255, 255, 150, 170, 255, 0,
                                 0, 1, 0xE, 1);
            rot2 += 0x2AAA;
        }
    }
}

static void mode_wall_wait(BgYdanSp* this, PlayState* play) {
    Player* player;
    Vec3f sp30;

    player = GET_PLAYER(play);
    if (Actor_Environment_sw_Check(play, this->burnSwitchFlag) || (this->trisCollider.base.acFlags & AC_HIT)) {
        this->dyna.actor.home.pos.y = this->dyna.actor.world.pos.y + 80.0f;
        mode_burn_init(this, play);
    } else if (player->heldItemAction == PLAYER_IA_DEKU_STICK && player->unk_860 != 0) {
        Actor_search_position_project_distanceXZ(&this->dyna.actor, &sp30, &player->meleeWeaponInfo[0].tip);
        if (fabsf(sp30.x) < 100.0f && sp30.z < 1.0f && sp30.y < 200.0f) {
            makeOnepointDemo(play, 3020, 40, &this->dyna.actor, CAM_ID_MAIN);
            xyz_t_move(&this->dyna.actor.home.pos, &player->meleeWeaponInfo[0].tip);
            mode_burn_init(this, play);
        }
    }
    CollisionCheck_setAC(play, &play->colChkCtx, &this->trisCollider.base);
}

void Bg_Ydan_Sp_actor_move(Actor* thisx, PlayState* play) {
    BgYdanSp* this = (BgYdanSp*)thisx;

    this->actionFunc(this, play);
}

void Bg_Ydan_Sp_actor_draw(Actor* thisx, PlayState* play) {
    BgYdanSp* this = (BgYdanSp*)thisx;
    s32 i;
    MtxF mtxF;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_ydan_sp.c", 781);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    if (thisx->params == WEB_WALL) {
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_ydan_sp.c", 787);
        gSPDisplayList(POLY_XLU_DISP++, gDTWebWallDL);
    } else if (this->actionFunc == mode_floor_break) {
        Matrix_get(&mtxF);
        if (this->timer == 40) {
            Matrix_translate(0.0f, (thisx->home.pos.y - thisx->world.pos.y) * 10.0f, 0.0f, MTXMODE_APPLY);
            Matrix_scale(1.0f, ((thisx->home.pos.y - thisx->world.pos.y) + 10.0f) * 0.1f, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_ydan_sp.c", 808);
            gSPDisplayList(POLY_XLU_DISP++, gDTWebFloorDL);
        }
        for (i = 0; i < 8; i++) {
            Matrix_put(&mtxF);
            Matrix_rotateXYZ(-0x5A0, i * 0x2000, 0, MTXMODE_APPLY);
            Matrix_translate(0.0f, 700.0f, -900.0f, MTXMODE_APPLY);
            Matrix_scale(3.5f, 5.0f, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_ydan_sp.c", 830);
            gSPDisplayList(POLY_XLU_DISP++, gDTUnknownWebDL);
        }
    } else {
        Matrix_translate(0.0f, (thisx->home.pos.y - thisx->world.pos.y) * 10.0f, 0.0f, MTXMODE_APPLY);
        Matrix_scale(1.0f, ((thisx->home.pos.y - thisx->world.pos.y) + 10.0f) * 0.1f, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_ydan_sp.c", 849);
        gSPDisplayList(POLY_XLU_DISP++, gDTWebFloorDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_ydan_sp.c", 856);
}
