/*
 * File: z_bg_mori_bigst.c
 * Overlay: ovl_Bg_Mori_Bigst
 * Description: Forest Temple falling platform and Stalfos fight
 */

#include "z_bg_mori_bigst.h"
#include "assets/objects/object_mori_objects/object_mori_objects.h"
#include "quake.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Mori_Bigst_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mori_Bigst_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mori_Bigst_actor_move(Actor* thisx, PlayState* play);
void Bg_Mori_Bigst_actor_draw(Actor* thisx, PlayState* play);

static void mv_dmaWait_init(BgMoriBigst* this, PlayState* play);
static void mv_dmaWait(BgMoriBigst* this, PlayState* play);
void mv_deathStNo1_init(BgMoriBigst* this, PlayState* play);
void mv_deathStNo2_init(BgMoriBigst* this, PlayState* play);
void mv_deathStNo2(BgMoriBigst* this, PlayState* play);
static void mv_drop_init(BgMoriBigst* this, PlayState* play);
static void mv_drop(BgMoriBigst* this, PlayState* play);
static void mv_vibrato_init(BgMoriBigst* this, PlayState* play);
static void mv_vibrato(BgMoriBigst* this, PlayState* play);
void mv_deathStNo3_init(BgMoriBigst* this, PlayState* play);
void mv_deathStNo3(BgMoriBigst* this, PlayState* play);
static void mv_end_init(BgMoriBigst* this, PlayState* play);

ActorProfile Bg_Mori_Bigst_Profile = {
    /**/ ACTOR_BG_MORI_BIGST,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MORI_OBJECTS,
    /**/ sizeof(BgMoriBigst),
    /**/ Bg_Mori_Bigst_actor_ct,
    /**/ Bg_Mori_Bigst_actor_dt,
    /**/ Bg_Mori_Bigst_actor_move,
    /**/ NULL,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 3000, ICHAIN_CONTINUE),  ICHAIN_F32(cullingVolumeScale, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 3000, ICHAIN_CONTINUE),  ICHAIN_F32_DIV1000(gravity, -500, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(minVelocityY, -12000, ICHAIN_CONTINUE), ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static void set_moveProc(BgMoriBigst* this, BgMoriBigstActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static void set_dynaPoly(BgMoriBigst* this, PlayState* play, CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, moveFlag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        // "Warning : move BG login failed"
        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_mori_bigst.c", 190,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void Bg_Mori_Bigst_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriBigst* this = (BgMoriBigst*)thisx;

    // "mori (bigST.keyceiling)"
    PRINTF("mori (bigST.鍵型天井)(arg : %04x)(sw %d)(noE %d)(roomC %d)(playerPosY %f)\n", this->dyna.actor.params,
           Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6)),
           Actor_Environment_no_enemy_Check(play, this->dyna.actor.room), Actor_Environment_room_clear_Check(play, this->dyna.actor.room),
           GET_PLAYER(play)->actor.world.pos.y);
    set_dynaPoly(this, play, &gMoriBigstCol, 0);
    ValueSet_process(&this->dyna.actor, value_init);
    this->moriTexObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_TEX);
    if (this->moriTexObjectSlot < 0) {
        // "【Big Stalfos key ceiling】 bank danger!"
        PRINTF("【ビッグスタルフォス鍵型天井】 バンク危険！\n");
        PRINTF("%s %d\n", "../z_bg_mori_bigst.c", 234);
        Actor_delete(&this->dyna.actor);
        return;
    }
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6))) {
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
    } else {
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 270.0f;
    }
    Actor_world_to_eye(&this->dyna.actor, 50.0f);
    mv_dmaWait_init(this, play);
}

void Bg_Mori_Bigst_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriBigst* this = (BgMoriBigst*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mv_dmaWait_init(BgMoriBigst* this, PlayState* play) {
    set_moveProc(this, mv_dmaWait);
}

static void mv_dmaWait(BgMoriBigst* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->moriTexObjectSlot)) {
        thisx->draw = Bg_Mori_Bigst_actor_draw;
        if (Actor_Environment_room_clear_Check(play, thisx->room) && (GET_PLAYER(play)->actor.world.pos.y > 700.0f)) {
            if (Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 8, 6))) {
                mv_end_init(this, play);
            } else {
                mv_deathStNo2_init(this, play);
            }
        } else {
            mv_deathStNo1_init(this, play);
        }
    }
}

void mv_deathStNo1_init(BgMoriBigst* this, PlayState* play) {
    set_moveProc(this, NULL);
}

void mv_deathStNo2_init(BgMoriBigst* this, PlayState* play) {
    Actor* stalfos;

    set_moveProc(this, mv_deathStNo2);
    Actor_Environment_room_clear_Off(play, this->dyna.actor.room);
    stalfos = Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_EN_TEST, 209.0f, 827.0f, -3320.0f, 0,
                                 0, 0, 1);
    if (stalfos != NULL) {
        this->dyna.actor.child = NULL;
        this->dyna.actor.home.rot.z++;
    } else {
        // "Second Stalfos failure"
        PRINTF("Warning : 第２スタルフォス発生失敗\n");
    }
    Actor_Environment_room_clear_On(play, this->dyna.actor.room);
}

void mv_deathStNo2(BgMoriBigst* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((this->dyna.actor.home.rot.z == 0) &&
        ((this->dyna.actor.home.pos.y - 5.0f) <= GET_PLAYER(play)->actor.world.pos.y)) {
        mv_drop_init(this, play);
        makeOnepointDemo(play, 3220, 72, &this->dyna.actor, CAM_ID_MAIN);
    }
}

static void mv_drop_init(BgMoriBigst* this, PlayState* play) {
    set_moveProc(this, mv_drop);
}

static void mv_drop(BgMoriBigst* this, PlayState* play) {
    Actor_position_moveF(&this->dyna.actor);
    if (this->dyna.actor.world.pos.y <= this->dyna.actor.home.pos.y) {
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
        mv_vibrato_init(this, play);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_STONE_BOUND);
        makeOnepointDemo(play, 1020, 8, &this->dyna.actor, CAM_ID_MAIN);
        player_demo_mode_set2(play, NULL, PLAYER_CSACTION_60);
    }
}

static void mv_vibrato_init(BgMoriBigst* this, PlayState* play) {
    s32 pad;
    s32 quakeIndex;

    set_moveProc(this, mv_vibrato);
    this->waitTimer = 18;

    quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
    setSpeedQuake(quakeIndex, 25000);
    setScaleQuake(quakeIndex, 5, 0, 0, 0);
    setTimerQuake(quakeIndex, 16);
}

static void mv_vibrato(BgMoriBigst* this, PlayState* play) {
    if (this->waitTimer <= 0) {
        mv_deathStNo3_init(this, play);
    }
}

void mv_deathStNo3_init(BgMoriBigst* this, PlayState* play) {
    Actor* stalfos1;
    Actor* stalfos2;

    set_moveProc(this, mv_deathStNo3);
    Actor_Environment_room_clear_Off(play, this->dyna.actor.room);
    stalfos1 = Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_EN_TEST, 70.0f, 827.0f, -3383.0f, 0,
                                  0, 0, 5);
    if (stalfos1 != NULL) {
        this->dyna.actor.child = NULL;
        this->dyna.actor.home.rot.z++;
    } else {
        // "Warning: 3-1 Stalfos failure"
        PRINTF("Warning : 第３-1スタルフォス発生失敗\n");
    }
    stalfos2 = Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_EN_TEST, 170.0f, 827.0f, -3260.0f, 0,
                                  0, 0, 5);
    if (stalfos2 != NULL) {
        this->dyna.actor.child = NULL;
        this->dyna.actor.home.rot.z++;
    } else {
        // "Warning: 3-2 Stalfos failure"
        PRINTF("Warning : 第３-2スタルフォス発生失敗\n");
    }
    Actor_Environment_room_clear_On(play, this->dyna.actor.room);
}

void mv_deathStNo3(BgMoriBigst* this, PlayState* play) {
    if ((this->dyna.actor.home.rot.z == 0) && !player_demo_check(play)) {
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 8, 6));
        mv_end_init(this, play);
    }
}

static void mv_end_init(BgMoriBigst* this, PlayState* play) {
    set_moveProc(this, NULL);
}

void Bg_Mori_Bigst_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriBigst* this = (BgMoriBigst*)thisx;

    Actor_world_to_eye(&this->dyna.actor, 50.0f);
    if (this->waitTimer > 0) {
        this->waitTimer--;
    }
    if (MoveBG_checkOverPlayerStatus(&this->dyna)) {
        get_pol_color(play, 6);
    }
    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void Bg_Mori_Bigst_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriBigst* this = (BgMoriBigst*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_mori_bigst.c", 541);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, play->objectCtx.slots[this->moriTexObjectSlot].segment);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mori_bigst.c", 548);

    gSPDisplayList(POLY_OPA_DISP++, gMoriBigstDL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_mori_bigst.c", 553);
}
