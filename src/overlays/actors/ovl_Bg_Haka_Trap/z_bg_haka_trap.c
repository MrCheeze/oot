/*
 * File: z_bg_haka_trap.c
 * Overlay: ovl_Bg_Haka_Trap
 * Description: Shadow Temple Objects
 */

#include "z_bg_haka_trap.h"

#include "ichain.h"
#include "rand.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"
#include "z64skin_matrix.h"

#include "assets/objects/object_haka_objects/object_haka_objects.h"

#define FLAGS 0

void Bg_Haka_Trap_actor_ct(Actor* thisx, PlayState* play);
void Bg_Haka_Trap_actor_dt(Actor* thisx, PlayState* play);
void Bg_Haka_Trap_actor_move(Actor* thisx, PlayState* play);
void Bg_Haka_Trap_actor_draw(Actor* thisx, PlayState* play);

void set_haka_trap_at_pipe(BgHakaTrap* this, PlayState* play);
void mode_hasami_move(BgHakaTrap* this, PlayState* play);
void mode_hasami_fire(BgHakaTrap* this, PlayState* play);
void mode_girotin_down(BgHakaTrap* this, PlayState* play);
void mode_girotin_up(BgHakaTrap* this, PlayState* play);
void mode_kenzan_down(BgHakaTrap* this, PlayState* play);
void mode_kenzan_up(BgHakaTrap* this, PlayState* play);
void mode_fofo_stop(BgHakaTrap* this, PlayState* play);
void fofo_set_player_power(BgHakaTrap* this, PlayState* play, s16 arg2);
void mode_fofo_ready(BgHakaTrap* this, PlayState* play);
void mode_fofo_rotate(BgHakaTrap* this, PlayState* play);
void set_haka_hasami_ac_tris(BgHakaTrap* this);

static UNK_TYPE work_flg = 0;

ActorProfile Bg_Haka_Trap_Profile = {
    /**/ ACTOR_BG_HAKA_TRAP,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKA_OBJECTS,
    /**/ sizeof(BgHakaTrap),
    /**/ Bg_Haka_Trap_actor_ct,
    /**/ Bg_Haka_Trap_actor_dt,
    /**/ Bg_Haka_Trap_actor_move,
    /**/ Bg_Haka_Trap_actor_draw,
};

static ColliderCylinderInit HakaTrapAtPipeData = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 90, 0, { 0, 0, 0 } },
};

static ColliderTrisElementInit HakaHasamiAcTrisElemData[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00020000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 1800.0f, 1200.0f, 0.0f }, { -1800.0f, 1200.0f, 0.0f }, { -1800.0f, 0.0f, 0.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00020000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 1800.0f, 1200.0f, 0.0f }, { -1800.0f, 0.0f, 0.0f }, { 1800.0f, 0.0f, 0.0f } } },
    },
};

static ColliderTrisInit HakaHasamiAcTrisData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    2,
    HakaHasamiAcTrisElemData,
};

static CollisionCheckInfoInit HakaTrapStatusData = { 0, 80, 100, MASS_IMMOVABLE };

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Haka_Trap_actor_ct(Actor* thisx, PlayState* play) {
    static UNK_TYPE tmp_data = 0;
    BgHakaTrap* this = (BgHakaTrap*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(thisx, value_init);
    thisx->params &= 0xFF;

    if (thisx->params != HAKA_TRAP_PROPELLER) {
        ClObjPipe_ct(play, &this->colliderCylinder);
        ClObjPipe_set5(play, &this->colliderCylinder, thisx, &HakaTrapAtPipeData);

        if ((thisx->params == HAKA_TRAP_GUILLOTINE_SLOW) || (thisx->params == HAKA_TRAP_GUILLOTINE_FAST)) {
            this->timer = 20;
            this->colliderCylinder.dim.yShift = 10;
            thisx->velocity.y = 0.1f;

            if (thisx->params == HAKA_TRAP_GUILLOTINE_FAST) {
                thisx->params = HAKA_TRAP_GUILLOTINE_SLOW;
                this->unk_16A = 1;
            }

            this->actionFunc = mode_girotin_down;
        } else {
            MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
            thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;

            if (thisx->params == HAKA_TRAP_SPIKED_BOX) {
                DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_009CD0, &colHeader);
                this->timer = 30;

                if (tmp_data != 0) {
                    this->actionFunc = mode_kenzan_up;
                    tmp_data = 0;
                } else {
                    tmp_data = 1;
                    this->actionFunc = mode_kenzan_down;
                    thisx->velocity.y = 0.5f;
                }

                thisx->floorHeight = thisx->home.pos.y - 225.0f;
                this->unk_16A = (thisx->floorHeight + 50.0f) - 25.0f;

                this->colliderCylinder.dim.radius = 10;
                this->colliderCylinder.dim.height = 40;
            } else {
                if (thisx->params == HAKA_TRAP_SPIKED_WALL) {
                    DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_0081D0, &colHeader);
                    thisx->home.pos.x -= 200.0f;
                } else {
                    thisx->home.pos.x += 200.0f;
                    DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_008D10, &colHeader);
                }

                ClObjTris_ct(play, &this->colliderSpikes);
                ClObjTris_set5_nzm(play, &this->colliderSpikes, thisx, &HakaHasamiAcTrisData, this->colliderSpikesItem);

                this->colliderCylinder.dim.radius = 18;
                this->colliderCylinder.dim.height = 115;

                this->colliderCylinder.elem.atElemFlags &= ~ATELEM_SFX_NORMAL;
                this->colliderCylinder.elem.atElemFlags |= ATELEM_SFX_WOOD;

                this->actionFunc = mode_hasami_move;
            }

            this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
        }
    } else {
        this->timer = 40;
        this->actionFunc = mode_fofo_stop;
        thisx->cullingVolumeScale = 500.0f;
    }

    CollisionCheck_Status_set2(&thisx->colChkInfo, NULL, &HakaTrapStatusData);
}

void Bg_Haka_Trap_actor_dt(Actor* thisx, PlayState* play) {
    BgHakaTrap* this = (BgHakaTrap*)thisx;

    if (this->dyna.actor.params != HAKA_TRAP_PROPELLER) {
        if (this->dyna.actor.params != HAKA_TRAP_GUILLOTINE_SLOW) {
            DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
            if ((this->dyna.actor.params == HAKA_TRAP_SPIKED_WALL) ||
                (this->dyna.actor.params == HAKA_TRAP_SPIKED_WALL_2)) {
                ClObjTris_dt_nzf(play, &this->colliderSpikes);
            }
        }

        ClObjPipe_dt(play, &this->colliderCylinder);
    }

    Nai_StopAllObjFx(&this->unk_16C);
}

void set_haka_trap_at_pipe(BgHakaTrap* this, PlayState* play) {
    f32 cosine;
    Vec3f sp28;
    f32 sine;
    f32 zNonNegative;
    Player* player = GET_PLAYER(play);

    Actor_search_position_project_distanceXZ(&this->dyna.actor, &sp28, &player->actor.world.pos);

    sine = sin_s(this->dyna.actor.shape.rot.y);
    cosine = cos_s(this->dyna.actor.shape.rot.y);
    if (this->dyna.actor.params == HAKA_TRAP_GUILLOTINE_SLOW) {
        sp28.x = CLAMP(sp28.x, -50.0f, 50.0f);
        zNonNegative = (sp28.z >= 0.0f) ? 1.0f : -1.0f;
        sp28.z = zNonNegative * -15.0f;
    } else {
        sp28.x = -CLAMP(sp28.x, -162.0f, 162.0f);
        zNonNegative = (sp28.z >= 0.0f) ? 1.0f : -1.0f;
        sp28.z = zNonNegative * 15.0f;
    }

    this->colliderCylinder.dim.pos.x = this->dyna.actor.world.pos.x + sp28.x * cosine + sp28.z * sine;
    this->colliderCylinder.dim.pos.z = this->dyna.actor.world.pos.z + sp28.x * sine + sp28.z * cosine;
}

void mode_hasami_move(BgHakaTrap* this, PlayState* play) {
    static UNK_TYPE stop_flg = 0;
    Player* player = GET_PLAYER(play);

    if ((work_flg == 0) && (!player_demo_check(play))) {
        if (!chase_f(&this->dyna.actor.world.pos.x, this->dyna.actor.home.pos.x, 0.5f)) {
            Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_TRAP_OBJ_SLIDE - SFX_FLAG);
        } else if (this->dyna.actor.params == HAKA_TRAP_SPIKED_WALL) {
            stop_flg |= 1;
        } else if (this->dyna.actor.params == HAKA_TRAP_SPIKED_WALL_2) {
            stop_flg |= 2;
        }
    }

    set_haka_trap_at_pipe(this, play);

    if (this->colliderSpikes.base.acFlags & AC_HIT) {
        this->timer = 20;
        work_flg = 1;
        this->actionFunc = mode_hasami_fire;
    } else if (stop_flg == 3) {
        stop_flg = 4;
        player->actor.bgCheckFlags |= BGCHECKFLAG_CRUSHED;
    }
}

void mode_hasami_fire(BgHakaTrap* this, PlayState* play) {
    static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };
    Vec3f vector;
    f32 xScale;
    s32 i;

    if (this->timer != 0) {
        this->timer--;
    }

    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_BURN_OUT - SFX_FLAG);

    for (i = 0; i < 2; i++) {
        f32 rand = fqrand();

        xScale = (this->dyna.actor.params == HAKA_TRAP_SPIKED_WALL) ? -30.0f : 30.0f;

        vector.x = xScale * rand + this->dyna.actor.world.pos.x;
        vector.y = fqrand() * 10.0f + this->dyna.actor.world.pos.y + 30.0f;
        vector.z = rnd_fx(320.0f) + this->dyna.actor.world.pos.z;

        _Effect_SS_Db_ct(play, &vector, &zero_vec, &zero_vec, 130, 20, 255, 255, 150, 170, 255, 0, 0, 1, 9, false);
    }

    if (this->timer == 0) {
        work_flg = 0;
        Actor_delete(&this->dyna.actor);
    }
}

void mode_girotin_down(BgHakaTrap* this, PlayState* play) {
    s32 sp24;
    s32 timer;

    if (this->unk_16A) {
        this->dyna.actor.velocity.y *= 3.0f;
    } else {
        this->dyna.actor.velocity.y *= 2.0f;
    }

    if (this->timer != 0) {
        this->timer -= 1;
    }

    sp24 =
        chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 185.0f, this->dyna.actor.velocity.y);
    timer = this->timer;

    if ((timer == 10 && !this->unk_16A) || (timer == 13 && this->unk_16A)) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_GUILLOTINE_BOUND);
    }

    if (this->timer == 0) {
        this->dyna.actor.velocity.y = 0.0f;
        this->timer = (this->unk_16A) ? 10 : 40;
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_GUILLOTINE_UP);
        this->actionFunc = mode_girotin_up;
    }

    set_haka_trap_at_pipe(this, play);

    if (sp24 == 0) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderCylinder.base);
    }
}

void mode_girotin_up(BgHakaTrap* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->unk_16A) {
        chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 27.0f);
    } else {
        if (this->timer > 20) {
            chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 90.0f, 9.0f);
        } else {
            chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 4.5f);
        }

        if (this->timer == 20) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_GUILLOTINE_UP);
        }
    }

    if (this->timer == 0) {
        this->timer = 20;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
        this->dyna.actor.velocity.y = 0.1f;
        this->actionFunc = mode_girotin_down;
    }

    set_haka_trap_at_pipe(this, play);
}

void mode_kenzan_down(BgHakaTrap* this, PlayState* play) {
    Vec3f vector;
    f32 floorHeight;
    f32 yIntersect;
    s32 i;
    s32 bgId;

    this->dyna.actor.velocity.y *= 1.6f;

    if (this->timer != 0) {
        this->timer--;
    }

    vector.x = this->dyna.actor.world.pos.x + 90.0f;
    vector.y = (this->dyna.actor.world.pos.y + 1.0f) + 25.0f;
    vector.z = this->dyna.actor.world.pos.z;

    floorHeight = this->dyna.actor.floorHeight;

    for (i = 0; i < 3; i++) {
        yIntersect =
            T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->dyna.actor.floorPoly, &bgId, &this->dyna.actor, &vector) -
            25.0f;
        if (floorHeight < yIntersect) {
            floorHeight = yIntersect;
        }

        vector.x -= 90.0f;
    }

    if (chase_f(&this->dyna.actor.world.pos.y, floorHeight, this->dyna.actor.velocity.y)) {
        if (this->dyna.actor.velocity.y > 0.01f) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_TRAP_BOUND);
        }
        this->dyna.actor.velocity.y = 0.0f;
    }

    if (this->dyna.actor.velocity.y >= 0.01f) {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_CHINETRAP_DOWN - SFX_FLAG);
    }

    if (this->timer == 0) {
        this->dyna.actor.velocity.y = 0.0f;
        this->timer = 30;
        this->unk_16A = (s16)this->dyna.actor.world.pos.y + 50.0f;
        this->unk_16A = CLAMP_MAX(this->unk_16A, this->dyna.actor.home.pos.y);

        this->actionFunc = mode_kenzan_up;
    }
}

void mode_kenzan_up(BgHakaTrap* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer > 20) {
        this->unk_169 = chase_f(&this->dyna.actor.world.pos.y, this->unk_16A, 15.0f);
    } else {
        this->unk_169 = chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 20.0f);
    }

    if (this->timer == 0) {
        this->timer = 30;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
        this->dyna.actor.velocity.y = 0.5f;
        this->actionFunc = mode_kenzan_down;
    }
}

void mode_fofo_stop(BgHakaTrap* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer -= 1;
    }

    if (this->timer == 0) {
        this->actionFunc = mode_fofo_ready;
    }
}

void fofo_set_player_power(BgHakaTrap* this, PlayState* play, s16 arg2) {
    Player* player = GET_PLAYER(play);
    Vec3f sp18;

    Actor_search_position_project_distanceXZ(&this->dyna.actor, &sp18, &player->actor.world.pos);

    if ((fabsf(sp18.x) < 70.0f) && (fabsf(sp18.y) < 100.0f) && (sp18.z < 500.0f) &&
        (GET_PLAYER(play)->currentBoots != PLAYER_BOOTS_IRON)) {
        player->pushedSpeed = ((500.0f - sp18.z) * 0.06f + 5.0f) * arg2 * (1.0f / 0x3A00) * (2.0f / 3.0f);
        player->pushedYaw = this->dyna.actor.shape.rot.y;
    }
}

void mode_fofo_ready(BgHakaTrap* this, PlayState* play) {
    if (this->timer != 0) {
        if (chase_angle(&this->dyna.actor.world.rot.z, 0, this->dyna.actor.world.rot.z * 0.03f + 5.0f)) {
            this->timer = 40;
            this->actionFunc = mode_fofo_stop;
        }
    } else {
        if (chase_angle(&this->dyna.actor.world.rot.z, 0x3A00, this->dyna.actor.world.rot.z * 0.03f + 5.0f)) {
            this->timer = 100;
            this->actionFunc = mode_fofo_rotate;
        }
    }

    this->dyna.actor.shape.rot.z += this->dyna.actor.world.rot.z;
    if (this->dyna.actor.world.rot.z >= 0x1801) {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_WIND_TRAP - SFX_FLAG);
    }

    fofo_set_player_power(this, play, this->dyna.actor.world.rot.z);
}

void mode_fofo_rotate(BgHakaTrap* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_WIND_TRAP - SFX_FLAG);

    if (this->timer == 0) {
        this->timer = 1;
        this->actionFunc = mode_fofo_ready;
    }

    this->dyna.actor.shape.rot.z += this->dyna.actor.world.rot.z;
    fofo_set_player_power(this, play, this->dyna.actor.world.rot.z);
}

void Bg_Haka_Trap_actor_move(Actor* thisx, PlayState* play) {
    BgHakaTrap* this = (BgHakaTrap*)thisx;
    Vec3f* actorPos = &this->dyna.actor.world.pos;

    this->actionFunc(this, play);

    if ((this->dyna.actor.params != HAKA_TRAP_PROPELLER) && (thisx->params != HAKA_TRAP_SPIKED_BOX)) {
        this->colliderCylinder.dim.pos.y = actorPos->y;

        if ((thisx->params == HAKA_TRAP_GUILLOTINE_SLOW) || (thisx->params == HAKA_TRAP_GUILLOTINE_FAST)) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderCylinder.base);
            CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderCylinder.base);
        } else {
            if (this->actionFunc == mode_hasami_move) {
                CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderSpikes.base);
            }

            CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderCylinder.base);
        }
    }
}

void set_haka_hasami_ac_tris(BgHakaTrap* this) {
    Vec3f vec3;
    Vec3f vec2;
    Vec3f vec1;

    Matrix_Position(&HakaHasamiAcTrisElemData[0].dim.vtx[0], &vec1);
    Matrix_Position(&HakaHasamiAcTrisElemData[0].dim.vtx[1], &vec2);
    Matrix_Position(&HakaHasamiAcTrisElemData[0].dim.vtx[2], &vec3);
    CollisionCheck_Uty_setTrisPos(&this->colliderSpikes, 0, &vec1, &vec2, &vec3);

    Matrix_Position(&HakaHasamiAcTrisElemData[1].dim.vtx[2], &vec2);
    CollisionCheck_Uty_setTrisPos(&this->colliderSpikes, 1, &vec1, &vec3, &vec2);
}

void Bg_Haka_Trap_actor_draw(Actor* thisx, PlayState* play) {
    static Gfx* shape_model[5] = {
        object_haka_objects_DL_007610, object_haka_objects_DL_009860, object_haka_objects_DL_007EF0,
        object_haka_objects_DL_008A20, object_haka_objects_DL_0072C0,
    };
    static Color_RGBA8 hasami_fog = { 0, 0, 0, 0 };
    BgHakaTrap* this = (BgHakaTrap*)thisx;
    s32 pad;
    Vec3f sp2C;

    if (this->actionFunc == mode_hasami_fire) {
        Eff_Set_Fog2(play, &hasami_fog, this->timer + 20, 0x28);
    }

    Cheap_gfx_display(play, shape_model[this->dyna.actor.params]);

    if (this->actionFunc == mode_hasami_move) {
        set_haka_hasami_ac_tris(this);
    }

    if (this->actionFunc == mode_hasami_fire) {
        Eff_Off_Fog(play);
    }

    if ((this->actionFunc == mode_kenzan_up) && !this->unk_169) {
        sp2C.x = this->dyna.actor.world.pos.x;
        sp2C.z = this->dyna.actor.world.pos.z;
        sp2C.y = this->dyna.actor.world.pos.y + 110.0f;

        Skin_Matrix_MulVector(&play->viewProjectionMtxF, &sp2C, &this->unk_16C);
        Na_StartObjectSe_F(&this->unk_16C, NA_SE_EV_BRIDGE_CLOSE - SFX_FLAG);
    }
}
