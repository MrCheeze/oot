/*
 * File: z_bg_jya_zurerukabe.c
 * Overlay: ovl_Bg_Jya_Zurerukabe
 * Description: Sliding, Climbable Brick Wall
 */

#include "z_bg_jya_zurerukabe.h"
#include "assets/objects/object_jya_obj/object_jya_obj.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Jya_Zurerukabe_actor_ct(Actor* thisx, PlayState* play);
void Bg_Jya_Zurerukabe_actor_dt(Actor* thisx, PlayState* play);
void Bg_Jya_Zurerukabe_actor_move(Actor* thisx, PlayState* play);
void Bg_Jya_Zurerukabe_actor_draw(Actor* thisx, PlayState* play);

void link_flyaway_zurerukabe(BgJyaZurerukabe* this, PlayState* play);
static void mv_stop_init(BgJyaZurerukabe* this);
static void mv_stop(BgJyaZurerukabe* this, PlayState* play);
static void mv_slide_init(BgJyaZurerukabe* this);
static void mv_slide(BgJyaZurerukabe* this, PlayState* play);

static f32 JZ_nowSpd[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

ActorProfile Bg_Jya_Zurerukabe_Profile = {
    /**/ ACTOR_BG_JYA_ZURERUKABE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_JYA_OBJ,
    /**/ sizeof(BgJyaZurerukabe),
    /**/ Bg_Jya_Zurerukabe_actor_ct,
    /**/ Bg_Jya_Zurerukabe_actor_dt,
    /**/ Bg_Jya_Zurerukabe_actor_move,
    /**/ Bg_Jya_Zurerukabe_actor_draw,
};

static s16 JZ_PosYTbl[4] = { 943, 1043, 1243, 1343 };

static s16 JZ_PosDirTbl[4] = { -1, 1, -1, 1 };

static s16 JZ_TimeTbl[4] = { 48, 48, 36, 36 };

static f32 JZ_spdTbl[4] = { 8.0f, 8.0f, 10.0f, 10.0f };

static s16 danger_section[6][2] = {
    { 0x0388, 0x0395 }, { 0x03EA, 0x03FF }, { 0x0454, 0x0467 },
    { 0x04B4, 0x04C1 }, { 0x0518, 0x0528 }, { 0x0581, 0x0590 },
};

static s16 check_idx[6] = {
    0, 0, 1, 2, 2, 3,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static void set_dynaPoly(BgJyaZurerukabe* this, PlayState* play, CollisionHeader* collision, s32 flag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, flag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_jya_zurerukabe.c", 194,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void link_flyaway_zurerukabe(BgJyaZurerukabe* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 == PLAYER_STATE1_21) && (player->actor.wallPoly != NULL)) {
        s32 i;

        for (i = 0; i < ARRAY_COUNT(danger_section); i++) {
            f32 posY = player->actor.world.pos.y;

            if ((posY >= danger_section[i][0]) && (posY <= danger_section[i][1])) {
                break;
            }
        }

        switch (i) {
            case 0:
            case 2:
            case 3:
            case 5:
                if (fabsf(JZ_nowSpd[check_idx[i]]) > 1.0f) {
                    Actor_player_power_damage_AT_set(play, &this->dyna.actor, 1.5f, this->dyna.actor.shape.rot.y, 0.0f, 0);
                }
                break;
            case 1:
            case 4:
                if (fabsf(JZ_nowSpd[check_idx[i]] - JZ_nowSpd[check_idx[i + 1]]) > 1.0f) {
                    Actor_player_power_damage_AT_set(play, &this->dyna.actor, 1.5f, this->dyna.actor.shape.rot.y, 0.0f, 0);
                }
                break;
        }
    }
}

void Bg_Jya_Zurerukabe_actor_ct(Actor* thisx, PlayState* play) {
    BgJyaZurerukabe* this = (BgJyaZurerukabe*)thisx;
    s32 i;

    set_dynaPoly(this, play, &gZurerukabeCol, 0);
    ValueSet_process(thisx, value_init);

    for (i = 0; i < ARRAY_COUNT(JZ_PosYTbl); i++) {
        if (fabsf(JZ_PosYTbl[i] - this->dyna.actor.home.pos.y) < 1.0f) {
            this->unk_168 = i;
            break;
        }
    }

    if (i == ARRAY_COUNT(JZ_PosYTbl)) {
        PRINTF_COLOR_ERROR();
        PRINTF("home pos が変更されたみたい(%s %d)(arg_data 0x%04x)\n", "../z_bg_jya_zurerukabe.c", 299,
               this->dyna.actor.params);
        PRINTF_RST();
    }

    this->unk_16E = JZ_PosDirTbl[this->unk_168];
    mv_stop_init(this);
    PRINTF("(jya ずれる壁)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void Bg_Jya_Zurerukabe_actor_dt(Actor* thisx, PlayState* play) {
    BgJyaZurerukabe* this = (BgJyaZurerukabe*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    JZ_nowSpd[this->unk_168] = 0.0f;
}

static void mv_stop_init(BgJyaZurerukabe* this) {
    this->actionFunc = mv_stop;
}

static void mv_stop(BgJyaZurerukabe* this, PlayState* play) {
    if (this->unk_16A <= 0) {
        mv_slide_init(this);
    }
    JZ_nowSpd[this->unk_168] = 0.0f;
}

static void mv_slide_init(BgJyaZurerukabe* this) {
    this->actionFunc = mv_slide;
    this->unk_16A = JZ_TimeTbl[this->unk_168];
    if (ABS(this->unk_16C) == 4) {
        this->unk_16E = -this->unk_16E;
    }
    this->unk_16C += this->unk_16E;
}

static void mv_slide(BgJyaZurerukabe* this, PlayState* play) {
    if (chase_f(&this->dyna.actor.world.pos.x, this->dyna.actor.home.pos.x + (this->unk_16C * 75),
                     JZ_spdTbl[this->unk_168])) {
        mv_stop_init(this);
    }

    JZ_nowSpd[this->unk_168] = JZ_spdTbl[this->unk_168] * this->unk_16E;
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_ELEVATOR_MOVE - SFX_FLAG);
}

void Bg_Jya_Zurerukabe_actor_move(Actor* thisx, PlayState* play) {
    BgJyaZurerukabe* this = (BgJyaZurerukabe*)thisx;

    if (this->unk_16A > 0) {
        this->unk_16A--;
    }

    this->actionFunc(this, play);

    if (this->unk_168 == 0) {
        link_flyaway_zurerukabe(this, play);
    }
}

void Bg_Jya_Zurerukabe_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gZurerukabeDL);
}
