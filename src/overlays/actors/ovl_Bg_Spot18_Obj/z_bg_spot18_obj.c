
/*
 * File: z_bg_spot18_obj.c
 * Overlay: ovl_Bg_Spot18_Obj
 * Description:
 */

#include "z_bg_spot18_obj.h"
#include "assets/objects/object_spot18_obj/object_spot18_obj.h"

#define FLAGS 0

void Bg_Spot18_Obj_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot18_Obj_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot18_Obj_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot18_Obj_actor_draw(Actor* thisx, PlayState* play);

s32 ct_common_checkAge(BgSpot18Obj* this, PlayState* play);
static s32 ct_common_setScale(BgSpot18Obj* this, PlayState* play);
static s32 ct_common_setDynaPoly(BgSpot18Obj* this, PlayState* play);
s32 ct_common_setStatus(BgSpot18Obj* this, PlayState* play);
s32 ct_original_sekizo(BgSpot18Obj* this, PlayState* play);
s32 ct_original_yari(BgSpot18Obj* this, PlayState* play);
static void mvSet_stop(BgSpot18Obj* this);
static void mv_stop(BgSpot18Obj* this, PlayState* play);
void mvSet_stop_sekizo(BgSpot18Obj* this);
void mv_stop_sekizo(BgSpot18Obj* this, PlayState* play);
void mvSet_slip_sekizo(BgSpot18Obj* this);
void mv_slip_sekizo(BgSpot18Obj* this, PlayState* play);
void mvSet_dontmove_sekizo(BgSpot18Obj* this);
void mv_dontmove_sekizo(BgSpot18Obj* this, PlayState* play);

ActorProfile Bg_Spot18_Obj_Profile = {
    /**/ ACTOR_BG_SPOT18_OBJ,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT18_OBJ,
    /**/ sizeof(BgSpot18Obj),
    /**/ Bg_Spot18_Obj_actor_ct,
    /**/ Bg_Spot18_Obj_actor_dt,
    /**/ Bg_Spot18_Obj_actor_move,
    /**/ Bg_Spot18_Obj_actor_draw,
};

static u8 argTbl[2][2] = { { 0x01, 0x01 }, { 0x01, 0x00 } };

static f32 init_scale[] = {
    0.1f,
    0.1f,
};

static CollisionHeader* BGDT_info[] = {
    &gGoronCityStatueCol,
    &gGoronCityStatueSpearCol,
};

static u32 statusTbl[] = {
    0,
    0,
};

static BgSpot18ObjInitFunc ct_commonProcTbl[] = {
    ct_common_setDynaPoly,
    ct_common_checkAge,
    ct_common_setScale,
    ct_common_setStatus,
};

s32 ct_common_checkAge(BgSpot18Obj* this, PlayState* play) {
    s32 age;

    if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
        age = 1;
    } else if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        age = 0;
    } else {
        PRINTF("Error : リンク年齢不詳 (%s %d)(arg_data 0x%04x)\n", "../z_bg_spot18_obj.c", 182,
               this->dyna.actor.params);
        return 0;
    }

    switch (argTbl[PARAMS_GET_U(this->dyna.actor.params, 0, 4)][age]) {
        case 0:
        case 1:
            if (argTbl[PARAMS_GET_U(this->dyna.actor.params, 0, 4)][age] == 0) {
                PRINTF("出現しない Object (0x%04x)\n", this->dyna.actor.params);
            }
            return argTbl[PARAMS_GET_U(this->dyna.actor.params, 0, 4)][age];
        case 2:
            PRINTF("Error : Obj出現判定が設定されていない(%s %d)(arg_data 0x%04x)\n", "../z_bg_spot18_obj.c", 202,
                   this->dyna.actor.params);
            return 0;
        default:
            PRINTF("Error : Obj出現判定失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_spot18_obj.c", 210,
                   this->dyna.actor.params);
    }
    return 0;
}

static s32 ct_common_setScale(BgSpot18Obj* this, PlayState* play) {
    Actor_set_scale(&this->dyna.actor, init_scale[PARAMS_GET_U(this->dyna.actor.params, 0, 4)]);
    return 1;
}

static s32 ct_common_setDynaPoly(BgSpot18Obj* this, PlayState* play) {
    s32 pad[2];
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(BGDT_info[PARAMS_GET_U(this->dyna.actor.params, 0, 4)], &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    return 1;
}

s32 ct_common_setStatus(BgSpot18Obj* this, PlayState* play) {
    this->dyna.actor.flags |= statusTbl[PARAMS_GET_U(this->dyna.actor.params, 0, 4)];
    return 1;
}

static s32 ct_common(BgSpot18Obj* this, PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(ct_commonProcTbl); i++) {
        if (ct_commonProcTbl[i](this, play) == 0) {
            return 0;
        }
    }
    return 1;
}

s32 ct_original_sekizo(BgSpot18Obj* this, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_F32(minVelocityY, -10, ICHAIN_CONTINUE),           ICHAIN_F32(gravity, -4, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDistance, 1400, ICHAIN_CONTINUE), ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_STOP),
    };

    ValueSet_process(&this->dyna.actor, value_init);

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        mvSet_dontmove_sekizo(this);
    } else if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
        mvSet_dontmove_sekizo(this);
        this->dyna.actor.world.pos.x = (sin_s(this->dyna.actor.world.rot.y) * 80.0f) + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = (cos_s(this->dyna.actor.world.rot.y) * 80.0f) + this->dyna.actor.home.pos.z;
    } else {
        mvSet_stop_sekizo(this);
    }
    return 1;
}

s32 ct_original_yari(BgSpot18Obj* this, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 700, ICHAIN_STOP),
    };

    ValueSet_process(&this->dyna.actor, value_init);
    mvSet_stop(this);
    return 1;
}

static BgSpot18ObjInitFunc ct_originalProcTbl[] = {
    ct_original_sekizo,
    ct_original_yari,
};

static Gfx(*modelDtTbl[]) = {
    gGoronCityStatueDL,
    gGoronCityStatueSpearDL,
};

static s32 ct_original(BgSpot18Obj* this, PlayState* play) {
    if ((ct_originalProcTbl[PARAMS_GET_U(this->dyna.actor.params, 0, 4)] != NULL) &&
        (!ct_originalProcTbl[PARAMS_GET_U(this->dyna.actor.params, 0, 4)](this, play))) {
        return 0;
    }
    return 1;
}

void Bg_Spot18_Obj_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot18Obj* this = (BgSpot18Obj*)thisx;

    PRINTF("Spot18 Object [arg_data : 0x%04x]\n", this->dyna.actor.params);
    if (!ct_common(this, play)) {
        Actor_delete(&this->dyna.actor);
    } else if (!ct_original(this, play)) {
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Spot18_Obj_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot18Obj* this = (BgSpot18Obj*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mvSet_stop(BgSpot18Obj* this) {
    this->actionFunc = mv_stop;
}

static void mv_stop(BgSpot18Obj* this, PlayState* play) {
}

void bgCheck_sekizo(BgSpot18Obj* this, PlayState* play) {
    Actor_BGcheck2(play, &this->dyna.actor, 20.0f, 46.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
}

void clr_pu_sekizo(BgSpot18Obj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (fabsf(this->dyna.unk_150) > 0.001f) {
        this->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
    }
}

void mvSet_stop_sekizo(BgSpot18Obj* this) {
    this->unk_168 = 20;
    this->actionFunc = mv_stop_sekizo;
}

void mv_stop_sekizo(BgSpot18Obj* this, PlayState* play) {
    if (this->dyna.unk_150 < -0.001f) {
        if (this->unk_168 <= 0) {
            mvSet_slip_sekizo(this);
        }
    } else {
        this->unk_168 = 20;
    }
    clr_pu_sekizo(this, play);
}

void mvSet_slip_sekizo(BgSpot18Obj* this) {
    this->actionFunc = mv_slip_sekizo;
    this->dyna.actor.speed = 0.0f;
    this->dyna.actor.velocity.x = this->dyna.actor.velocity.y = this->dyna.actor.velocity.z = 0.0f;
    this->dyna.actor.world.rot.y = 0;
}

void mv_slip_sekizo(BgSpot18Obj* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);

    chase_f(&this->dyna.actor.speed, 1.2f, 0.1f);
    Actor_position_moveF(&this->dyna.actor);
    bgCheck_sekizo(this, play);

    if (Math3DLengthSquare2D(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.z, this->dyna.actor.home.pos.x,
                        this->dyna.actor.home.pos.z) >= SQ(80.0f)) {
        mvSet_dontmove_sekizo(this);
        this->dyna.actor.world.pos.x = (sin_s(this->dyna.actor.world.rot.y) * 80.0f) + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = (cos_s(this->dyna.actor.world.rot.y) * 80.0f) + this->dyna.actor.home.pos.z;
        this->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
    } else {
        Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
    }
}

void mvSet_dontmove_sekizo(BgSpot18Obj* this) {
    this->actionFunc = mv_dontmove_sekizo;
}

void mv_dontmove_sekizo(BgSpot18Obj* this, PlayState* play) {
    clr_pu_sekizo(this, play);
}

void Bg_Spot18_Obj_actor_move(Actor* thisx, PlayState* play) {
    BgSpot18Obj* this = (BgSpot18Obj*)thisx;

    if (this->unk_168 > 0) {
        this->unk_168--;
    }
    this->actionFunc(this, play);
}

void Bg_Spot18_Obj_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, modelDtTbl[PARAMS_GET_U(thisx->params, 0, 4)]);
}
