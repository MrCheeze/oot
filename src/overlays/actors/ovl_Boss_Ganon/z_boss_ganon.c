#include "z_boss_ganon.h"
#include "overlays/actors/ovl_En_Ganon_Mant/z_en_ganon_mant.h"
#include "overlays/actors/ovl_En_Zl3/z_en_zl3.h"
#include "overlays/actors/ovl_Bg_Ganon_Otyuka/z_bg_ganon_otyuka.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"

#include "libc64/math64.h"
#include "libc64/qrand.h"
#include "attributes.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "rand.h"
#include "rumble.h"
#include "segmented_address.h"
#include "seqcmd.h"
#include "sequence.h"
#include "sfx.h"
#include "sys_math.h"
#include "sys_matrix.h"
#include "versions.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#include "global.h"

#include "assets/overlays/ovl_Boss_Ganon/z_boss_ganon_shape.h"
#include "assets/objects/object_ganon/object_ganon.h"
#include "assets/objects/object_ganon_anime1/object_ganon_anime1.h"
#include "assets/objects/object_ganon_anime2/object_ganon_anime2.h"
#include "assets/scenes/dungeons/ganon_boss/ganon_boss_scene.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Boss_Ganon_Actor_ct(Actor* thisx, PlayState* play2);
void Boss_Ganon_Actor_dt(Actor* thisx, PlayState* play);
void Boss_Ganon_Actor_move(Actor* thisx, PlayState* play2);
void Boss_Ganon_Actor_draw(Actor* thisx, PlayState* play);
void gn_hikari_move(Actor* thisx, PlayState* play2); // update
void gn_big_magic_move(Actor* thisx, PlayState* play);  // update
void gn_wepon_move(Actor* thisx, PlayState* play2);
void gn_hikari_draw(Actor* thisx, PlayState* play2); // draw
void gn_big_magic_draw(Actor* thisx, PlayState* play);  // draw
void gn_wepon_draw(Actor* thisx, PlayState* play);

static void mode_startdemo_init(BossGanon* this, PlayState* play);
void mode_enddemo2_init(BossGanon* this, PlayState* play);
static void mode_startdemo(BossGanon* this, PlayState* play);
static void mode_enddemo(BossGanon* this, PlayState* play);
static void mode_flying(BossGanon* this, PlayState* play);
void mode_preatack(BossGanon* this, PlayState* play);
static void mode_atack2(BossGanon* this, PlayState* play);
void mode_Patack(BossGanon* this, PlayState* play);
void mode_Batack(BossGanon* this, PlayState* play);
static void mode_guard(BossGanon* this, PlayState* play);
void mode_ballhit(BossGanon* this, PlayState* play);
static void mode_yahit(BossGanon* this, PlayState* play);
void mode_kenhit(BossGanon* this, PlayState* play);

static void mode_flying_init(BossGanon* this, PlayState* play);
void mode_preatack_init(BossGanon* this, PlayState* play);
static void mode_atack2_init(BossGanon* this, PlayState* play);

void Gn_Eff_disp(PlayState* play);
void Gn_Eff_move(PlayState* play);

s32 otyuka_check(BossGanon* this, PlayState* play, Vec3f* checkPos);

ActorProfile Boss_Ganon_Profile = {
    /**/ ACTOR_BOSS_GANON,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_GANON,
    /**/ sizeof(BossGanon),
    /**/ Boss_Ganon_Actor_ct,
    /**/ Boss_Ganon_Actor_dt,
    /**/ Boss_Ganon_Actor_move,
    /**/ Boss_Ganon_Actor_draw,
};

static ColliderCylinderInit GanonOcInfoData = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x10 },
        { 0xFFCFFFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 80, -50, { 0, 0, 0 } },
};

static ColliderCylinderInit WeponOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK6,
        { 0x00100700, 0x00, 0x08 },
        { 0x0D900740, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 30, -15, { 0, 0, 0 } },
};

static u8 shape_flash_time_data[] = { 0, 12, 10, 12, 14, 16, 12, 14, 16, 12, 14, 16, 12, 14, 16, 10, 16, 14 };
static Vec3f zero = { 0.0f, 0.0f, 0.0f };

#pragma increment_block_number "gc-eu:0 gc-eu-mq:0 gc-jp:0 gc-jp-ce:0 gc-jp-mq:0 gc-us:0 gc-us-mq:0 ique-cn:0" \
                               "ntsc-1.0:0 ntsc-1.1:0 ntsc-1.2:0 pal-1.0:0 pal-1.1:0 hiratsu3:0"

static EnGanonMant* mant;

// TODO: There's probably a way to match BSS ordering with less padding by spreading the variables out and moving
// data around. It would be easier if we had more options for controlling BSS ordering in debug.
#pragma increment_block_number "gc-eu:192 gc-eu-mq:192 gc-jp:192 gc-jp-ce:192 gc-jp-mq:192 gc-us:192 gc-us-mq:192" \
                               "ique-cn:128 ntsc-1.0:128 ntsc-1.1:128 ntsc-1.2:128 pal-1.0:128 pal-1.1:128 hiratsu3:192"

static s32 rnd20;
static s32 rnd21;
static s32 rnd22;

#pragma increment_block_number "gc-eu:192 gc-eu-mq:192 gc-jp:192 gc-jp-ce:192 gc-jp-mq:192 gc-us:192 gc-us-mq:192" \
                               "ique-cn:192 ntsc-1.0:128 ntsc-1.1:128 ntsc-1.2:128 pal-1.0:128 pal-1.1:128 hiratsu3:192"

static BossGanon* ganon;

static EnZl3* zelda;

#define BOSSGANON_EFFECT_COUNT 200

typedef struct GanondorfEffect {
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 timer;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ Color_RGB8 color;
    /* 0x2C */ s16 alpha;
    /* 0x2E */ s16 unk_2E;
    /* 0x30 */ s16 unk_30;
    /* 0x34 */ f32 scale;
    /* 0x38 */ f32 unk_38; // scale target mostly, but used for other things
    /* 0x3C */ f32 unk_3C; // mostly z rot
    /* 0x40 */ f32 unk_40;
    /* 0x44 */ f32 unk_44; // mostly x rot
    /* 0x48 */ f32 unk_48; // mostly y rot
} GanondorfEffect;         // size = 0x4C

GanondorfEffect gn_eff[BOSSGANON_EFFECT_COUNT];

void gn_hahen_ct(PlayState* play, Vec3f* pos, Vec3f* velocity, f32 scale) {
    static Color_RGB8 hahen_prim[] = { { 255, 175, 85 }, { 155, 205, 155 }, { 155, 125, 55 } };
    s16 i;
    GanondorfEffect* eff = play->specialEffects;
    Color_RGB8* color;

    for (i = 0; i < 200; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_WINDOW_SHARD;
            eff->pos = *pos;
            eff->velocity = *velocity;
            eff->accel = zero;
            eff->scale = scale;
            eff->accel.y = -1.5f;
            eff->unk_44 = rnd_f(6.28f);
            eff->unk_48 = rnd_f(6.28f);
            color = &hahen_prim[(s16)rnd_f(2.99f)];
            eff->color.r = color->r;
            eff->color.g = color->g;
            eff->color.b = color->b;
            eff->timer = (s16)rnd_f(20.0f);
            break;
        }
    }
}

void gn_hinoko_ct(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, s16 arg6) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_SPARKLE;
            eff->pos = *pos;
            eff->velocity = *velocity;
            eff->accel = *accel;
            eff->scale = scale / 1000.0f;
            eff->unk_2E = (s16)rnd_f(100.0f) + 0xC8;
            eff->unk_30 = arg6;
            eff->timer = (s16)rnd_f(10.0f);
            break;
        }
    }
}

void gn_hinoko_2_ct(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, f32 arg5,
                                s16 arg6) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            s32 pad;

            eff->type = GDF_EFF_LIGHT_RAY;
            eff->pos = *pos;
            eff->velocity = *velocity;
            eff->accel = *accel;
            eff->scale = scale / 1000.0f;
            eff->unk_38 = 1.0f;
            eff->unk_40 = arg5;
            eff->unk_2E = (s16)rnd_f(100.0f) + 0xC8;
            eff->unk_30 = arg6;
            eff->timer = (s16)rnd_f(10.0f);
            eff->unk_48 = atanf_table(eff->velocity.z, eff->velocity.x);
            eff->unk_44 = -atanf_table(sqrtf(SQXZ(eff->velocity)), eff->velocity.y);
            break;
        }
    }
}

void gn_denki_ct(PlayState* play, f32 scale, s16 shockType) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 75; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_SHOCK;
            eff->pos = zero;
            eff->pos.y = -2000.0f;
            eff->velocity = zero;
            eff->accel = zero;
            eff->scale = scale / 1000.0f;
            eff->unk_2E = shockType;
            eff->timer = 0;
            break;
        }
    }
}

void gn_hand_beam_ct(PlayState* play, f32 scale, f32 arg2, f32 arg3) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_LIGHTNING;
            eff->velocity = zero;
            eff->accel = zero;
            eff->unk_2E = 0;
            eff->scale = scale;
            eff->unk_48 = arg2;
            eff->unk_3C = arg3;
            eff->timer = 0;
            break;
        }
    }
}

void gn_ring_smoke_ct(PlayState* play, Vec3f* pos, f32 scale, f32 arg3) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_IMPACT_DUST_DARK;
            eff->pos = *pos;
            eff->velocity = zero;
            eff->accel = zero;
            eff->scale = scale;
            eff->unk_40 = 1.0f;
            eff->unk_38 = arg3;
            eff->unk_30 = (s16)rnd_f(100.0f);
            eff->unk_2E = eff->timer = eff->alpha = 0;
            break;
        }
    }
}

void gn_ring_flash_ct(PlayState* play, Vec3f* pos, f32 scale, f32 arg3, s16 bufIndex) {
    GanondorfEffect* effArr = play->specialEffects;

    effArr[bufIndex].type = GDF_EFF_IMPACT_DUST_LIGHT;
    effArr[bufIndex].pos = *pos;
    effArr[bufIndex].velocity = zero;
    effArr[bufIndex].accel = zero;
    effArr[bufIndex].unk_40 = 1.0f;
    effArr[bufIndex].scale = scale;
    effArr[bufIndex].unk_38 = arg3;
    effArr[bufIndex].unk_30 = rnd_f(100.0f);
    effArr[bufIndex].unk_2E = effArr[bufIndex].timer = effArr[bufIndex].alpha = 0;
}

void gn_ring_beam_ct(PlayState* play, Vec3f* pos, f32 scale, f32 arg3) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_SHOCKWAVE;
            eff->pos = *pos;
            eff->velocity = zero;
            eff->accel = zero;
            eff->alpha = 255;
            eff->unk_40 = 0.6f;
            eff->scale = scale;
            eff->unk_38 = arg3;
            eff->unk_30 = (s16)rnd_f(100.0f);
            eff->unk_2E = eff->timer = 0;
            break;
        }
    }
}

void gn_magic_ball_ct(PlayState* play, Vec3f* pos, f32 scale) {
    s16 i;
    GanondorfEffect* eff = play->specialEffects;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_NONE) {
            eff->type = GDF_EFF_BLACK_DOT;
            eff->pos = *pos;
            eff->velocity = zero;
            eff->accel = zero;
            eff->unk_38 = 0.0f;
            eff->scale = scale / 1000.0f;
            eff->timer = 0;
            eff->alpha = 0;
            eff->unk_2E = 0;
            break;
        }
    }
}

static void CollisionCheck_Uty_PosSetPipeC(Vec3f* pos, ColliderCylinder* collider) {
    collider->dim.pos.x = pos->x;
    collider->dim.pos.y = pos->y;
    collider->dim.pos.z = pos->z;
}

static void obj_bank_set(BossGanon* this, PlayState* play, s32 objectId) {
    this->animObjectSlot = Object_Exchange_bank_check(&play->objectCtx, objectId);
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->animObjectSlot].segment);
}

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_GANONDORF, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 0, ICHAIN_STOP),
};

void Boss_Ganon_Actor_ct(Actor* thisx, PlayState* play2) {
    s16 i;
    PlayState* play = play2;
    BossGanon* this = (BossGanon*)thisx;
    s32 cond;
    f32 xDistFromPlayer;
    f32 yDistFromPlayer;
    f32 zDistFromPlayer;
    Player* player = GET_PLAYER(play);

    if (thisx->params < 0x64) {
        Actor_Environment_sw_On(play, 0x14);
        play->specialEffects = gn_eff;

        for (i = 0; i < BOSSGANON_EFFECT_COUNT; i++) {
            gn_eff[i].type = GDF_EFF_NONE;
        }

        ganon = this;
        thisx->colChkInfo.health = 40;
        ValueSet_process(thisx, value_init);
        Shape_Info_init(&thisx->shape, 0, NULL, 0);
        Actor_set_scale(thisx, 0.01f);
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGanondorfSkel, NULL, NULL, NULL, 0);
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, thisx, &GanonOcInfoData);

        if (thisx->params != 1) {
            mode_startdemo_init(this, play);
            this->organAlpha = 255;
        } else {
            cond = Actor_Environment_sw_Check(play, 0x37) &&
                   ((play->sceneId == SCENE_GANON_BOSS) || (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR) ||
                    (play->sceneId == SCENE_GANONS_TOWER_COLLAPSE_INTERIOR) ||
                    (play->sceneId == SCENE_INSIDE_GANONS_CASTLE_COLLAPSE));

            if (!cond) {
                mode_enddemo2_init(this, play);
            } else {
                Actor_delete(thisx);
                return;
            }

            mode_enddemo2_init(this, play);
        }

        mant = (EnGanonMant*)Actor_info_make_child_actor(&play->actorCtx, thisx, play, ACTOR_EN_GANON_MANT, 0.0f, 0.0f, 0.0f, 0,
                                                 0, 0, 1);
        Actor_info_part_chg(play, &play->actorCtx, thisx, ACTORCAT_BOSS);
    } else {
        thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->fwork[GDF_FWORK_1] = 255.0f;

        if (thisx->params >= 0xC8) {
            if (thisx->params == 0x12C) {
                thisx->update = gn_wepon_move;
                thisx->draw = gn_wepon_draw;
                this->unk_1A8 = 2;
            } else if (thisx->params == 0x190) {
                thisx->update = gn_wepon_move;
                thisx->draw = gn_wepon_draw;
                this->unk_1A8 = 1;
            } else if (thisx->params >= 0x104) {
                // big magic light ball thrown
                thisx->update = gn_big_magic_move;
                thisx->draw = gn_big_magic_draw;
                this->unk_1C2 = 10;
                this->unk_1A2 = 520 + (-thisx->params * 2);

                for (i = 0; i < 15; i++) {
                    this->unk_2EC[i] = thisx->world.pos;
                }

                this->timers[1] = 3;
                ClObjPipe_ct(play, &this->collider);
                ClObjPipe_set5(play, &this->collider, thisx, &WeponOcInfoData);
            } else if (thisx->params >= 0xFA) {
                // big magic light ball charge
                thisx->update = gn_big_magic_move;
                thisx->draw = gn_big_magic_draw;
                this->unk_1A2 = rnd_f(10000.0f);

                for (i = 0; i < 15; i++) {
                    this->unk_2EC[i] = thisx->world.pos;
                }

                this->fwork[GDF_FWORK_1] = 0;
            } else {
                thisx->update = gn_hikari_move;
                thisx->draw = gn_hikari_draw;
                if (1) {}
                thisx->speed = 11.0f;

                if (thisx->params == 0xC8) {
                    this->timers[0] = 7;
                } else {
                    this->timers[0] = (s16)rnd_f(3.0f) + 3;
                }

                for (i = 0; i < 15; i++) {
                    this->unk_2EC[i].y = 5000.0f;
                }
            }
        } else {
            // light ball (anything from 0x64 - 0xC7)
            thisx->update = gn_wepon_move;
            thisx->draw = gn_wepon_draw;
            thisx->speed = 12.0f;

            xDistFromPlayer = player->actor.world.pos.x - thisx->world.pos.x;
            yDistFromPlayer = (player->actor.world.pos.y + 30.0f) - thisx->world.pos.y;
            zDistFromPlayer = player->actor.world.pos.z - thisx->world.pos.z;

            thisx->world.rot.y = atans_table(zDistFromPlayer, xDistFromPlayer);
            thisx->world.rot.x = atans_table(sqrtf(SQ(xDistFromPlayer) + SQ(zDistFromPlayer)), yDistFromPlayer);

            if (fqrand() < 0) {
                thisx->world.rot.y += (s16)rnd_fx(5000.0f);
                thisx->world.rot.x += (s16)rnd_fx(5000.0f);
            }

            this->timers[1] = 3;
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set5(play, &this->collider, thisx, &WeponOcInfoData);
        }
    }
}

void Boss_Ganon_Actor_dt(Actor* thisx, PlayState* play) {
    BossGanon* this = (BossGanon*)thisx;

    if ((this->actor.params < 0xC8) || (this->actor.params >= 0x104)) {
        ClObjPipe_dt(play, &this->collider);
    }

    if (this->actor.params < 0x64) {
        Skeleton_Info_dt(&this->skelAnime, play);
    }
}

static void mode_startdemo_init(BossGanon* this, PlayState* play) {
    s32 pad;
    s32 animObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_GANON_ANIME2);

    if (animObjectSlot < 0) {
        Actor_delete(&this->actor);
        return;
    }

    if (Object_Exchange_bank_dma_check(&play->objectCtx, animObjectSlot)) {
        this->actionFunc = mode_startdemo;
        this->unk_198 = 1;
        this->animObjectSlot = animObjectSlot;
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[animObjectSlot].segment);
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfPlayOrganAnim, 0.0f);
    } else {
        this->actionFunc = mode_startdemo_init;
    }
}

typedef struct CutsceneCameraPosition {
    /* 0x00 */ Vec3s eye;
    /* 0x06 */ Vec3s at;
} CutsceneCameraPosition; // size = 0x12

static CutsceneCameraPosition StartDemoCamPos[] = {
    { { 0, 40, 0 }, { 0, 50, 430 } },
    { { -20, 30, 400 }, { 10, 55, 440 } },
    { { 0, 60, 300 }, { 0, 273, -150 } },
    { { 0, 180, -260 }, { 0, 155, -300 } },
    { { -30, 60, 440 }, { 20, 25, 390 } },
    { { -50, 140, -360 }, { 50, 92, -390 } },
    { { -10, 264, -121 }, { 5, 266, -160 } },
    { { -13, 200, -310 }, { 0, 125, -410 } },
    { { 0, 40, -50 }, { 0, 35, 230 } },
    { { 0, 140, -250 }, { 0, 115, -570 } },
    { { -410, 150, -130 }, { 50, 155, -170 } },
    { { 0, 130, -230 }, { 0, 125, -2000 } },
    { { -2, 147, -293 }, { -200, 345, -2000 } },
};

void StartDemoCamPosSet(BossGanon* this, u8 camPosIndex) {
    CutsceneCameraPosition* camPos = &StartDemoCamPos[camPosIndex];

    this->csCamEye.x = camPos->eye.x;
    this->csCamEye.y = camPos->eye.y;
    this->csCamEye.z = camPos->eye.z;

    this->csCamAt.x = camPos->at.x;
    this->csCamAt.y = camPos->at.y;
    this->csCamAt.z = camPos->at.z;
}

static void mode_startdemo(BossGanon* this, PlayState* play) {
    u8 moveCam = false;
    Player* player = GET_PLAYER(play);
    s32 pad;
    f32 sin;
    f32 cos;
    Camera* mainCam;

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->animObjectSlot].segment);

    mant->backPush = -2.0f;
    mant->backSwayMagnitude = 0.25f;
    mant->sideSwayMagnitude = -1.0f;
    mant->minDist = 0.0f;

    this->csTimer++;

    Skeleton_Info2_anime_play(&this->skelAnime);

    switch (this->csState) {
        case 0:
            player->actor.world.pos.x = 0.0f;
            player->actor.world.pos.y = 0.0f;
            player->actor.world.pos.z = 430.0f;

            this->actor.world.pos.x = 0.0f;
            this->actor.world.pos.y = 112.0f;
            this->actor.world.pos.z = -333.0f;

            this->actor.shape.yOffset = -7000.0f;
            this->actor.shape.rot.y = 0;

            Demo_play_start(play, &play->csCtx);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            this->csCamIndex = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->csCamIndex, CAM_STAT_ACTIVE);
            this->csCamFov = 60.0f;

            if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_GANONDORF_BATTLE)) {
                // watched cutscene already, skip most of it
                this->csState = 17;
                this->csTimer = 0;
                player->actor.world.pos.z = 20.0f;
                this->useOpenHand = false;
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfStandBackwardsAnim, -5.0f);
                this->fwork[GDF_FWORK_1] = 1000.0f;
                StartDemoCamPosSet(this, 11);
                this->unk_198 = 2;
                this->timers[2] = 110;
                z_common_data.healthAccumulator = 0x140;
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
            } else {
                this->useOpenHand = true;
                StartDemoCamPosSet(this, 0);
                this->csState = 1;
                zelda = (EnZl3*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_ZL3, 0.0f, 220.0f,
                                                    -150.0f, 0, 0, 0, 0x2000);
            }

            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_GANON_ORGAN, 0.0f, 0.0f, 0.0f, 0, 0, 0, 1);
            mant->minY = 57.0f;
            FALLTHROUGH;
        case 1:
            this->envLightMode = 3;
            if (this->csTimer == 70) {
                this->csState = 2;
                this->csTimer = 0;
            }
            break;

        case 2:
            StartDemoCamPosSet(this, 1);

            if (this->csTimer == 10) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_5);
            }

            if (this->csTimer == 13) {
                player_SE_set(player, player->ageProperties->unk_92 + NA_SE_VO_LI_SURPRISE);
            }

            if (this->csTimer != 35) {
                break;
            }

            this->csState = 3;
            this->csTimer = 0;

            this->csCamEye.x = 0.0f;
            this->csCamEye.y = 60.0f;
            this->csCamEye.z = 300.0f;

            this->csCamAt.x = 0.0f;
            this->unk_704 = 2 * M_PI / 5;
            FALLTHROUGH;
        case 3:
            this->envLightMode = 0;
            play->envCtx.lightBlend = 0.0f;
            this->csCamAt.y = (sinf(this->unk_704) * 300.0f) + this->csCamEye.y;
            this->csCamAt.z = (cosf(this->unk_704) * -300.0f) + this->csCamEye.z;
            add_calc2(&this->unk_704, 0.25f, 0.05f, this->csCamAtMaxStep.y);
            add_calc2(&this->csCamAtMaxStep.y, 0.01f, 1.0f, 0.0001f);

            if (this->csTimer != 200) {
                break;
            }

            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            this->csState = 4;
            StartDemoCamPosSet(this, 2);
            this->csTimer = 0;
            FALLTHROUGH;
        case 4:
            if ((this->csTimer == 0) || (this->csTimer == 10) || (this->csTimer == 20)) {
                this->csCamEye.y += 68.0f;
                this->csCamEye.z -= 142.0f;
            }

            if (this->csTimer >= 20) {
                this->envLightMode = 4;
            } else {
                this->envLightMode = 35;
            }

            if (this->csTimer == 60) {
                StartDemoCamPosSet(this, 1);
                this->csState = 5;
                this->csTimer = 0;
            }
            break;

        case 5:
            this->envLightMode = 5;

            if (this->csTimer < 50) {
                play->envCtx.lightBlend = 1.0f;
            }

            if (this->csTimer == 10) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_75);
            }

            if (this->csTimer == 70) {
                StartDemoCamPosSet(this, 3);
                this->csState = 6;
                this->csTimer = 0;
                this->envLightMode = 3;
            }
            break;

        case 6:
            this->envLightMode = 3;

            if (this->csTimer != 30) {
                break;
            }

            this->csState = 7;
            this->csTimer = 0;
            StartDemoCamPosSet(this, 4);
            this->triforceType = GDF_TRIFORCE_PLAYER;
            this->fwork[GDF_TRIFORCE_SCALE] = 10.0f;
            this->fwork[GDF_TRIFORCE_PRIM_A] = 0.0f;
            this->fwork[GDF_TRIFORCE_PRIM_B] = 255.0f;
            this->fwork[GDF_TRIFORCE_ENV_G] = 100.0f;
            Na_StartSystemSe_F(NA_SE_EV_TRIFORCE_MARK);
            play->envCtx.lightBlend = 0.0f;
            FALLTHROUGH;
        case 7:
            this->envLightMode = 6;
            // fade in links triforce
            add_calc2(&this->fwork[GDF_TRIFORCE_PRIM_A], 255.0f, 1.0f, 10.0f);
            add_calc2(&this->fwork[GDF_TRIFORCE_SCALE], 0.4f, 1.0f, 0.3f);
            add_calc2(&this->fwork[GDF_TRIFORCE_PRIM_B], 170.0f, 1.0f, 2.55f);
            add_calc2(&this->fwork[GDF_TRIFORCE_ENV_G], 200.0f, 1.0f, 3.0f);

            if (this->csTimer >= 30) {
                this->envLightMode = 65;
            }

            if (this->csTimer == 30) {
                play->envCtx.lightBlend = 1.0f;
            }

            StartDemoCamPosSet(this, 4);
            this->csCamEye.x += 5.0f;
            this->csCamEye.z += -10.0f;
            this->csCamAt.x += 18.0f;

            if (this->csTimer == 60) {
                this->csState = 8;
                this->csTimer = 0;
            }
            break;

        case 8:
            this->envLightMode = 3;
            StartDemoCamPosSet(this, 5);

            if (this->csTimer != 30) {
                break;
            }

            this->csState = 9;
            this->csTimer = 0;
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            zelda->unk_3C8 = 0;
            this->triforceType = GDF_TRIFORCE_ZELDA;
            this->fwork[GDF_TRIFORCE_SCALE] = 10.0f;
            this->fwork[GDF_TRIFORCE_PRIM_A] = 0.0f;
            this->fwork[GDF_TRIFORCE_PRIM_B] = 255.0f;
            this->fwork[GDF_TRIFORCE_ENV_G] = 100.0f;
            Na_StartSystemSe_F(NA_SE_EV_TRIFORCE_MARK);
            play->envCtx.lightBlend = 0.0f;
            FALLTHROUGH;
        case 9:
            this->envLightMode = 7;
            StartDemoCamPosSet(this, 6);
            // fade in zeldas triforce
            add_calc2(&this->fwork[GDF_TRIFORCE_PRIM_A], 255.0f, 1.0f, 10.0f);
            add_calc2(&this->fwork[GDF_TRIFORCE_SCALE], 0.4f, 1.0f, 0.3f);
            add_calc2(&this->fwork[GDF_TRIFORCE_PRIM_B], 170.0f, 1.0f, 2.55f);
            add_calc2(&this->fwork[GDF_TRIFORCE_ENV_G], 200.0f, 1.0f, 3.0f);

            if (this->csTimer == 30) {
                zelda->unk_3C8 = 1;
            }

            if (this->csTimer >= 32) {
                this->envLightMode = 75;
            }

            if (this->csTimer == 32) {
                play->envCtx.lightBlend = 1.0f;
            }

            if (this->csTimer == 50) {
                this->csState = 10;
                this->csTimer = 0;
            }
            break;

        case 10: // top view of playing the organ
            this->envLightMode = 3;
            StartDemoCamPosSet(this, 7);

            if (this->csTimer == 40) {
                this->csState = 11;
                this->csTimer = 0;
                this->fwork[GDF_TRIFORCE_PRIM_A] = 0.0f;
            }
            break;

        case 11: // link is healed
            this->envLightMode = 3;
            StartDemoCamPosSet(this, 8);
            player->actor.world.pos.z = 20.0f;

            if (this->csTimer == 20) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_23);
                alpha_change(HUD_VISIBILITY_HEARTS);
            }

            if (this->csTimer == 25) {
                z_common_data.healthAccumulator = 0x140;
            }

            if (this->csTimer == 100) {
                alpha_change(HUD_VISIBILITY_NOTHING);
            }

            if (this->csTimer == 120) {
                this->csState = 12;
                this->csTimer = 0;
            }
            break;

        case 12: // first dialogue, ganondorf facing away from link
            this->envLightMode = 3;
            StartDemoCamPosSet(this, 9);

            if (this->csTimer == 30) {
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfStopPlayingOrganAnim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfStopPlayingOrganAnim, -5.0f);
            }

            if ((this->csTimer > 30) && Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfLeanOnOrganAnim, 0.0f);
                this->fwork[GDF_FWORK_1] = 1000.0f;
            }

            if (this->csTimer == 80) {
                message_set(play, 0x70C8, NULL);
            }

            if ((this->csTimer > 180) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 15;
                this->csTimer = 0;
                this->useOpenHand = false;
            }
            break;

        case 15: // side view of all 3 of them
            this->envLightMode = 0;
            play->envCtx.lightBlend = 0.0f;
            StartDemoCamPosSet(this, 10);

            if (this->csTimer == 30) {
                message_set(play, 0x70C9, NULL);
            }

            if ((this->csTimer > 100) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 16;
                this->csTimer = 0;
                StartDemoCamPosSet(this, 11);
                this->unk_198 = 2;
                zelda->unk_3C8 = 2;
                this->timers[2] = 110;
                this->envLightMode = 3;
            }
            break;

        case 16:
            this->envLightMode = 3;

            if (this->csTimer <= 20) {
                if (this->csTimer == 20) {
                    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfStandUpFromOrganAnim, -5.0f);
                    this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfStandUpFromOrganAnim);
                }
            } else if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                message_set(play, 0x70CA, NULL);
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfStandBackwardsAnim, -5.0f);
                this->fwork[GDF_FWORK_1] = 1000.0f;
            }

            if ((this->csTimer > 100) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 17;
                this->csTimer = 0;
            }
            break;

        case 17: // turns around
            this->envLightMode = 3;

            if (this->csTimer == 20) {
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfTurnAroundAnim, -5.0f);
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfTurnAroundAnim);
            }

            if (this->csTimer > 10) {
                if (this->csTimer == 62) {
                    mant->attachRightArmTimer = 20.0f;
                }

                if (this->csTimer == 57) {
                    Actor_SE_set(&this->actor, NA_SE_EV_GANON_MANTLE);
                }

                add_calc2(&this->csCamFov, 110.0f, 0.1f, this->csCamMaxStepScale * 2.0f);
                add_calc2(&this->csCamEye.z, -290.0f, 0.1f, this->csCamMaxStepScale * 2.4f);
                add_calc2(&this->csCamMaxStepScale, 0.75f, 1.0f, 0.05f);

                if (this->csTimer == 70) {
                    this->csState = 18;
                    this->csTimer = 0;
                    this->csCamFov = 60.0f;
                    StartDemoCamPosSet(this, 12);
                    message_set(play, 0x70CB, NULL);
                }
            }
            break;

        case 18: // last dialog before triforce
            this->envLightMode = 3;
            StartDemoCamPosSet(this, 12);
            this->csCamEye.y += -6.0f;
            this->csCamEye.z += 6.0f;

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1] - 5.0f)) {
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfStandIdleAnim, -5.0f);
                this->fwork[GDF_FWORK_1] = 1000.0f;
            }

            if ((this->csTimer <= 50) || (message_check(&play->msgCtx) != TEXT_STATE_NONE)) {
                break;
            }

            this->csState = 19;
            this->csTimer = 0;
            message_set(play, 0x70CC, NULL);
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfRaiseHandStartAnim, -5.0f);
            this->triforceType = GDF_TRIFORCE_DORF;
            this->fwork[GDF_TRIFORCE_SCALE] = 10.0f;
            this->fwork[GDF_TRIFORCE_PRIM_A] = 0.0f;
            this->fwork[GDF_TRIFORCE_PRIM_B] = 255.0f;
            this->fwork[GDF_TRIFORCE_ENV_G] = 100.0f;
            play->envCtx.lightBlend = 0.0f;
            FALLTHROUGH;
        case 19: // show triforce
            this->envLightMode = 8;

            if (this->csTimer >= 60) {
                this->envLightMode = 9;

                if (this->csTimer == 60) {
                    play->envCtx.lightBlend = 1.0f;
                }
            }

            StartDemoCamPosSet(this, 12);
            this->csCamEye.y += -6.0f;
            this->csCamEye.z += 6.0f;

            if (this->csTimer >= 30) {
                if (this->csTimer == 30) {
                    Na_StartSystemSe_F(NA_SE_EV_TRIFORCE_MARK);
                }

                // fade in ganondorf's triforce
                add_calc2(&this->fwork[GDF_TRIFORCE_PRIM_A], 255.0f, 1.0f, 10.0f);
                add_calc2(&this->fwork[GDF_TRIFORCE_SCALE], 0.6f, 1.0f, 0.3f);
                add_calc2(&this->fwork[GDF_TRIFORCE_PRIM_B], 170.0f, 1.0f, 2.55f);
                add_calc2(&this->fwork[GDF_TRIFORCE_ENV_G], 200.0f, 1.0f, 3.0f);
            }

            if (this->csTimer == 17) {
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfRaiseHandLoopAnim, -5.0f);
            }

            if ((this->csTimer > 80) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 20;
                this->csTimer = 0;

                this->csCamTargetEye.x = this->csCamEye.x - 50.0f;
                this->csCamTargetEye.y = this->csCamEye.y - 100.0f;
                this->csCamTargetEye.z = this->csCamEye.z + 400.0f;

                this->csCamEyeMaxStep.x = 50.0f;
                this->csCamEyeMaxStep.y = 100.0f;
                this->csCamEyeMaxStep.z = 400.0f;

                this->csCamAtMaxStep.x = 400.0f;
                this->csCamMaxStepScale = 0.0f;

                this->csCamTargetAt.x = this->csCamAt.x + 400.0f;
                this->csCamTargetAt.y = this->csCamAt.y;
                this->csCamTargetAt.z = this->csCamAt.z;

                this->csCamMovementScale = 0.2f;

                this->fwork[GDF_VORTEX_ALPHA] = 0.0f;
                this->fwork[GDF_VORTEX_SCALE] = 0.1f;

                Actor_SE_set(&this->actor, NA_SE_EN_GANON_DARKWAVE);
            }
            break;

        case 20: // zoom cam out
            this->envLightMode = 10;
            moveCam = true;
            add_calc2(&this->csCamMaxStepScale, 0.15f, 1.0f, 0.015f);

            if (this->csTimer <= 40) {
                add_calc2(&this->fwork[GDF_VORTEX_ALPHA], 255.0f, 1.0f, 6.5f);
                add_calc2(&this->fwork[GDF_VORTEX_SCALE], 0.2f, 1.0f, 0.025f);
            }

            if (this->csTimer > 20) {
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_DARKWAVE_M - SFX_FLAG);
            }

            if (this->csTimer > 20) {
                gn_denki_ct(play, 700.0f, GDF_SHOCK_PLAYER_PURPLE);
                gn_denki_ct(play, 700.0f, GDF_SHOCK_PLAYER_PURPLE);
            }

            if (this->csTimer == 30) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_74);
            }

            if (this->csTimer <= 50) {
                break;
            }

            this->csState = 21;
            this->csTimer = 0;
            this->fwork[GDF_TRIFORCE_PRIM_A] = 0.0f;
            this->fwork[GDF_VORTEX_SCALE] = 0.16f;
            goto skip_sound_and_fx;

        case 21: // purple vortex
            this->envLightMode = 11;
            Actor_SE_set(&this->actor, NA_SE_EN_GANON_DARKWAVE_M - SFX_FLAG);
            gn_denki_ct(play, 700.0f, GDF_SHOCK_PLAYER_PURPLE);
            gn_denki_ct(play, 700.0f, GDF_SHOCK_PLAYER_PURPLE);

        skip_sound_and_fx:
            this->csCamEye.x = -30.0f;
            this->csCamEye.y = 37.0f;
            this->csCamEye.z = -30.0f;

            this->csCamAt.x = -10.0f;
            this->csCamAt.y = 45.0f;
            this->csCamAt.z = 0.0f;

            if (this->csTimer == 13) {
                message_set(play, 0x70CD, NULL);
            }

            if ((this->csTimer <= 120) || (message_check(&play->msgCtx) != TEXT_STATE_NONE)) {
                break;
            }

            this->csState = 22;
            this->csTimer = 0;
            this->timers[2] = 30;
            this->organAlpha = 254;
            this->csCamAt.x = this->unk_1FC.x - 10.0f;
            this->csCamAt.y = this->unk_1FC.y + 30.0f;
            this->csCamAt.z = this->unk_1FC.z;
            this->fwork[GDF_VORTEX_ALPHA] = 255.0f;
            this->fwork[GDF_VORTEX_SCALE] = 0.2f;
            FALLTHROUGH;
        case 22: // start floating, show title card, start fight
            if (this->csTimer > 30) {
                this->envLightMode = 0;
            } else {
                this->envLightMode = 12;
            }

            add_calc0(&this->fwork[GDF_VORTEX_ALPHA], 1.0f, 10.0f);

            this->csCamEye.x = -30.0f;
            this->csCamEye.y = 137.0f;
            this->csCamEye.z = -110.0f;

            add_calc2(&this->csCamAt.y, this->unk_1FC.y + 30.0f, 0.1f, 20.0f);
            add_calc2(&this->csCamAt.x, this->unk_1FC.x - 10.0f, 0.1f, 5.0f);

            if (this->csTimer == 20) {
                obj_bank_set(this, play, OBJECT_GANON_ANIME1);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfGetUp3Anim, 0.0f);
                Skeleton_Info2_anime_play(&this->skelAnime);
                this->actor.shape.yOffset = 0.0f;
                mant->attachShouldersTimer = 18.0f;
                Actor_SE_set(&this->actor, NA_SE_EV_GANON_MANTLE);
                this->unk_198 = 0;
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_GANONDORF_BOSS);
            }

            if (this->csTimer == 50) {
                SegmentBaseAddress[6] =
                    VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[Object_Exchange_bank_check(&play->objectCtx, OBJECT_GANON)].segment);

                if (!GET_EVENTCHKINF(EVENTCHKINF_BEGAN_GANONDORF_BATTLE)) {
                    Actor_Name_Disp_Set(play, &play->actorCtx.titleCtx, SEGMENTED_TO_VIRTUAL(gGanondorfTitleCardTex),
                                           160, 180, 128, 40);
                }

                SET_EVENTCHKINF(EVENTCHKINF_BEGAN_GANONDORF_BATTLE);
            }

            if (this->csTimer >= 20) {
                this->legSwayEnabled = true;
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_FLOAT - SFX_FLAG);

                add_calc2(&this->actor.world.pos.y, 228.0f, 0.05f, 2.0f);
                add_calc2(&this->actor.world.pos.z, -230.0f, 0.05f, 4.0f);

                mant->backPush = -3.0f;
                mant->backSwayMagnitude = 0.25f;
                mant->sideSwayMagnitude = -3.0f;

                sin = sin_s(this->csTimer * 1500);
                this->actor.velocity.y = this->fwork[GDF_FWORK_0] * sin * 0.04f;
                this->actor.world.pos.y += this->actor.velocity.y;

                cos = cos_s(this->csTimer * 1800);
                this->actor.world.pos.x = this->fwork[GDF_FWORK_0] * cos * 0.5f;
                this->actor.velocity.x = this->actor.world.pos.x - this->actor.prevPos.x;

                add_calc2(&this->fwork[GDF_FWORK_0], 50.0f, 1.0f, 1.0f);
            }

            if (this->csTimer > 30) {
                this->organAlpha -= 5;

                if (this->organAlpha < 0) {
                    this->organAlpha = 0;
                }
            }

            if (this->csTimer == 120) {
                mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
                mainCam->eye = this->csCamEye;
                mainCam->eyeNext = this->csCamEye;
                mainCam->at = this->csCamAt;
                Gama_play_shift2main_camera(play, this->csCamIndex, 0);
                this->csState = this->csCamIndex = 0;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                mode_flying_init(this, play);
            }

            if (zelda != NULL) {
                zelda->actor.world.pos.x = 0.0f;
                zelda->actor.world.pos.y = 350.0f;
                zelda->actor.world.pos.z = 0.0f;
            }
    }

    if (this->csCamIndex != 0) {
        if (moveCam) {
            add_calc2(&this->csCamEye.x, this->csCamTargetEye.x, this->csCamMovementScale,
                           this->csCamEyeMaxStep.x * this->csCamMaxStepScale);
            add_calc2(&this->csCamEye.y, this->csCamTargetEye.y, this->csCamMovementScale,
                           this->csCamEyeMaxStep.y * this->csCamMaxStepScale);
            add_calc2(&this->csCamEye.z, this->csCamTargetEye.z, this->csCamMovementScale,
                           this->csCamEyeMaxStep.z * this->csCamMaxStepScale);

            add_calc2(&this->csCamAt.x, this->csCamTargetAt.x, this->csCamMovementScale,
                           this->csCamAtMaxStep.x * this->csCamMaxStepScale);
            add_calc2(&this->csCamAt.y, this->csCamTargetAt.y, this->csCamMovementScale,
                           this->csCamAtMaxStep.y * this->csCamMaxStepScale);
            add_calc2(&this->csCamAt.z, this->csCamTargetAt.z, this->csCamMovementScale,
                           this->csCamAtMaxStep.z * this->csCamMaxStepScale);
        }

        Gama_play_camera_setting(play, this->csCamIndex, &this->csCamAt, &this->csCamEye);
        Gama_play_set_camera_fovy(play, this->csCamIndex, this->csCamFov);
    }
}

static void mode_enddemo_init(BossGanon* this, PlayState* play) {
    s32 pad;
    s32 animObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_GANON_ANIME2);

    if (Object_Exchange_bank_dma_check(&play->objectCtx, animObjectSlot)) {
        this->actionFunc = mode_enddemo;
        this->csTimer = this->csState = 0;
        this->unk_198 = 1;
        this->animObjectSlot = animObjectSlot;
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[animObjectSlot].segment);
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfDefeatedStartAnim, 0.0f);
        this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfDefeatedStartAnim);
        this->unk_508 = 0.0f;
    }
}

void mode_enddemo2_init(BossGanon* this, PlayState* play) {
    s32 pad;
    s32 animObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_GANON_ANIME2);

    if (Object_Exchange_bank_dma_check(&play->objectCtx, animObjectSlot)) {
        this->animObjectSlot = animObjectSlot;
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[animObjectSlot].segment);
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfDefeatedStartAnim, 0.0f);
        this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfDefeatedStartAnim);
        this->actionFunc = mode_enddemo;
        this->csTimer = 0;
        this->csState = 100;
        this->unk_198 = 1;
        z_common_data.save.info.playerData.magic = z_common_data.magicCapacity;
        z_common_data.save.info.playerData.health = z_common_data.save.info.playerData.healthCapacity;
    } else {
        this->actionFunc = mode_enddemo2_init;
    }
}

void enddemo_kabe_break(u8 windowShatterState) {
    s16 i;
    u8* tex1 = SEGMENTED_TO_VIRTUAL(ganon_boss_sceneTex_006C18);
    u8* tex2 = SEGMENTED_TO_VIRTUAL(ganon_boss_sceneTex_007418);

    for (i = 0; i < 2048; i++) {
        if ((tex1[i] != 0) && (fqrand() < 0.03f)) {
            if ((((u8*)ganon_kabe_nuki_txt)[i] == 0) ||
                (windowShatterState == GDF_WINDOW_SHATTER_FULL)) {
                tex1[i] = tex2[i] = 1;
            }
        }
    }
}

static void mode_enddemo(BossGanon* this, PlayState* play) {
    s16 i;
    u8 moveCam = false;
    Player* player = GET_PLAYER(play);
    s16 pad;

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->animObjectSlot].segment);

    this->csTimer++;
    Skeleton_Info2_anime_play(&this->skelAnime);

    switch (this->csState) {
        case 0:
            Demo_play_start(play, &play->csCtx);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            this->csCamIndex = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->csCamIndex, CAM_STAT_ACTIVE);

            this->actor.world.pos.x = 0.0f;
            this->actor.world.pos.y = 70.0f;
            this->actor.world.pos.z = -80.0f;

            this->actor.shape.yOffset = -7000.0f;

            this->actor.shape.rot.y = 0;
            this->csState = 1;
            this->csTimer = 0;
            this->useOpenHand = true;
            FALLTHROUGH;
        case 1:
            player->actor.shape.rot.y = -0x8000;

            player->actor.world.pos.x = -10.0f;
            player->actor.world.pos.y = 0.0f;
            player->actor.world.pos.z = 115.0f;

            this->envLightMode = 13;

            if (this->csTimer < 30) {
                play->envCtx.lightBlend = 0.0f;
            }

            if (this->csTimer >= 2) {
                play->envCtx.fillScreen = false;
            }

            this->csCamEye.x = -50.0f;
            this->csCamEye.z = -50.0f;
            this->csCamEye.y = 50.0f;

            this->csCamAt.x = this->unk_1FC.x;
            this->csCamAt.y = this->unk_1FC.y + 30.0f;
            this->csCamAt.z = this->unk_1FC.z;

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfDefeatedLoopAnim, 0.0f);
                this->csState = 2;
                this->csTimer = 0;
            }
            break;

        case 2:
            this->csCamEye.x = -100.0f;
            this->csCamEye.y = 20.0f;
            this->csCamEye.z = -130.0f;

            this->envLightMode = 13;

            this->csCamAt.x = this->unk_1FC.x;
            this->csCamAt.y = this->unk_1FC.y;
            this->csCamAt.z = this->unk_1FC.z + 40.0f;

            if (this->csTimer >= 30) {
                this->csState = 3;
                this->csTimer = 0;
                message_set(play, 0x70CE, NULL);
                this->fwork[GDF_FWORK_1] = 1000.0f;
            }

            if ((this->unk_1A2 % 32) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_BREATH);
            }
            break;

        case 3:
            this->envLightMode = 14;

            if ((this->fwork[GDF_FWORK_1] > 100.0f) && ((this->unk_1A2 % 32) == 0)) {
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_BREATH);
            }

            this->csCamEye.x = 7.0f;
            this->csCamEye.y = 52.0f;
            this->csCamEye.z = -15.0f;

            this->csCamAt.x = this->unk_1FC.x - 5.0f;
            this->csCamAt.y = this->unk_1FC.y + 30.0f - 10.0f;
            this->csCamAt.z = this->unk_1FC.z;

            if ((this->fwork[GDF_FWORK_1] > 100.0f) && (this->csTimer > 100) &&
                (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfVomitStartAnim, 0.0f);
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfVomitStartAnim);
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_TOKETU);
            } else {
                if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1] - 16.0f)) {
                    Vec3f sp98;
                    Vec3f sp8C;
#if OOT_VERSION < PAL_1_0
                    Color_RGBA8 prim = { 120, 0, 0, 255 };
                    Color_RGBA8 env = { 120, 0, 0, 255 };
#else
                    static Color_RGBA8 prim = { 0, 120, 0, 255 };
                    static Color_RGBA8 env = { 0, 120, 0, 255 };
#endif
                    Vec3f sp80;

                    for (i = 0; i < 40; i++) {
                        sp98.x = rnd_fx(5.0f);
                        sp98.y = rnd_fx(1.5f) + 1.0f;
                        sp98.z = rnd_f(5.0f) + 2.0f;

                        sp8C.x = 0.0f;
                        sp8C.y = -1.0f;
                        sp8C.z = 0.0f;

                        sp80.x = this->unk_208.x;
                        sp80.y = this->unk_208.y - 10.0f;
                        sp80.z = this->unk_208.z;

                        Effect_SS_Dust_sc_cl_co_ct(play, &sp80, &sp98, &sp8C, &prim, &env,
                                      (s16)rnd_f(50.0f) + 50, 0, 17);
                    }
                }

                if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfVomitLoopAnim, 0.0f);
                    this->csState = 4;
                    this->csTimer = 0;
                }
            }
            break;

        case 4:
            this->envLightMode = 14;

            if (this->csTimer == 30) {
                message_set(play, 0x70CF, NULL);
                this->csState = 5;
                this->csTimer = 0;
            }
            break;

        case 5:
            this->envLightMode = 14;

            if ((this->csTimer > 70) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 6;
                this->csTimer = 0;
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfYellStartAnim, 0.0f);
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfYellStartAnim);

                this->csCamMovementScale = 0.05f;
                this->csCamMaxStepScale = 0.0f;

                this->csCamTargetEye.x = 7.0f;
                this->csCamTargetEye.y = 12.0f;
                this->csCamTargetEye.z = 70.0f;

                this->csCamTargetAt.x = this->unk_1FC.x - 5.0f;
                this->csCamTargetAt.y = (this->unk_1FC.y + 30.0f) - 10.0f;
                this->csCamTargetAt.z = this->unk_1FC.z;

                this->csCamEyeMaxStep.x = fabsf(this->csCamEye.x - this->csCamTargetEye.x);
                this->csCamEyeMaxStep.y = fabsf(this->csCamEye.y - this->csCamTargetEye.y);
                this->csCamEyeMaxStep.z = fabsf(this->csCamEye.z - this->csCamTargetEye.z);

                this->csCamAtMaxStep.x = fabsf(this->csCamAt.x - this->csCamTargetAt.x);
                this->csCamAtMaxStep.y = fabsf(this->csCamAt.y - this->csCamTargetAt.y);
                this->csCamAtMaxStep.z = fabsf(this->csCamAt.z - this->csCamTargetAt.z);

                Actor_SE_set(&this->actor, NA_SE_EN_GANON_CASBREAK);
            }
            break;

        case 6:
            this->envLightMode = 14;
            moveCam = true;
            add_calc2(&this->csCamMaxStepScale, 0.2f, 1.0f, 0.01f);

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfYellLoopAnim, 0.0f);
                this->csState = 7;
                this->csTimer = 0;
                this->unk_2E8 = 0;
                this->envLightMode = 15;
                this->unk_508 = 0.0f;
                this->fwork[GDF_FWORK_1] = 1000.0f;
                play->envCtx.lightBlend = 0.0f;
            }
            break;

        case 7:
            if (this->csTimer < 10) {
                play->envCtx.lightBlend = 0.0f;
            }

            if (this->csTimer == 30) {
                this->csState = 8;
                this->csTimer = 0;
                this->unk_70C = 0.0f;
            }
            goto skip_cam_and_quake;

        case 8:
            this->csCamEye.x = -60.0f;
            this->csCamEye.y = 80.0f;
            this->csCamEye.z = -130.0f;

            this->csCamAt.x = 0.0f;
            this->csCamAt.y = 0.0f;
            this->csCamAt.z = 70.0f;

            this->unk_70C = sin_s(this->csTimer * 0x6300) * 0.2f;

            Na_StartSystemSe_F(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

        skip_cam_and_quake:
            this->envLightMode = 15;
            Actor_SE_set(&this->actor, NA_SE_EN_GANON_BODY_SPARK - SFX_FLAG);

            for (i = 1; i < 15; i++) {
                this->unk_4E4[i] = 0xA;
            }

            this->unk_2E6 = 20000;
            add_calc2(&this->unk_508, 5.0f, 0.05f, 0.1f);

            if (this->csTimer == 30) {
                this->csState = 9;
                this->csTimer = 0;

                this->csCamEye.x = -30.0f;
                this->csCamEye.y = 40.0f;
                this->csCamEye.z = 60.0f;

                this->csCamAt.x = 492.0f;
                this->csCamAt.y = 43.0f;
                this->csCamAt.z = 580.0f;

                this->csCamMaxStepScale = 0.0f;
                this->unk_710 = 10.0f;
            }
            break;

        case 9:
            Actor_SE_set(&this->actor, NA_SE_EN_GANON_BODY_SPARK - SFX_FLAG);

            if (this->csTimer == 2) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_57);
            }

            if (this->csTimer > 50) {
                add_calc0(&this->unk_710, 1.0f, 0.2f);
                add_calc2(&this->csCamEye.x, 270.0f, 0.05f, this->csCamMaxStepScale * 30.0f);
                add_calc2(&this->csCamEye.z, 260.0f, 0.05f, this->csCamMaxStepScale * 20.0f);
                add_calc2(&this->csCamAt.y, 103.0f, 0.05f, this->csCamMaxStepScale * 6.0f);
                add_calc2(&this->csCamAt.z, 280.0f, 0.05f, this->csCamMaxStepScale * 20.0f);
                add_calc2(&this->csCamMaxStepScale, 1.0f, 1.0f, 0.01f);
            }

            this->unk_70C = sin_s(this->csTimer * 0x6300) * this->unk_710;
            Na_StartSystemSe_F(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            if (this->csTimer < 100) {
                this->windowShatterState = GDF_WINDOW_SHATTER_PARTIAL;
                this->envLightMode = 15;
            } else {
                this->envLightMode = 16;
                this->windowShatterState = GDF_WINDOW_SHATTER_FULL;
            }

            if (this->csTimer >= 130) {
                add_calc2(&this->whiteFillAlpha, 255.0f, 1.0f, 5.0f);
            }

            if (this->csTimer == 180) {
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->nextEntranceIndex = ENTR_GANONS_TOWER_COLLAPSE_EXTERIOR_0;
                play->transitionType = TRANS_TYPE_FADE_WHITE_FAST;
            }
            break;

        case 100:
            Demo_play_start(play, &play->csCtx);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            this->csCamIndex = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->csCamIndex, CAM_STAT_ACTIVE);
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfCollapseAnim, 0.0f);
            this->fwork[1] = Si2_anime_end_frame(&gGanondorfDefeatedStartAnim);
            this->csState = 101;
            this->skelAnime.playSpeed = 0.0f;
            zelda = (EnZl3*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_ZL3, 0.0f, 6000.0f, 0.0f,
                                                0, 0, 0, 0x2000);

            player->actor.world.pos.x = -472.0f;
            player->actor.world.pos.y = 4102.0f;
            player->actor.world.pos.z = -130.0f;

            player->actor.shape.rot.y = -0x8000;

            this->actor.world.pos.x = -472.0f;
            this->actor.world.pos.y = 4172.0f;
            this->actor.world.pos.z = -400.0f;

            this->actor.shape.yOffset = -7000.0f;
            this->actor.shape.rot.y = 0;

            this->csCamEye.x = this->csCamAt.x = -472.0f;
            this->csCamEye.y = this->csCamAt.y = 4152.0f;
            this->csCamEye.z = -160.0f;

            this->csCamAt.z = -100.0f;

            mant->backPush = -2.0f;
            mant->backSwayMagnitude = 0.25f;
            mant->sideSwayMagnitude = -1.0f;
            mant->minDist = 0.0f;
            mant->minY = 4104.0f;
            mant->tearTimer = 20;

            this->whiteFillAlpha = 255.0f;
            play->envCtx.lightBlend = 1.0f;
            FALLTHROUGH;
        case 101:
            player->actor.world.pos.y = 4102.0f;
            add_calc0(&this->whiteFillAlpha, 1.0f, 5.0f);

            if (this->csTimer > 40) {
                add_calc2(&this->csCamEye.z, -520.0f, 0.1f, this->csCamMaxStepScale);
                add_calc2(&this->csCamMaxStepScale, 5.0f, 1.0f, 0.1f);

                if (this->csTimer == 150) {
                    this->skelAnime.playSpeed = 1.0f;
                }

                if (this->csTimer == 160) {
                    Actor_SE_set(&this->actor, NA_SE_PL_BOUND_NOWEAPON);
                }

                if (this->csTimer == 187) {
                    Actor_SE_set(&this->actor, NA_SE_PL_BODY_HIT);
                }

                if (this->csTimer == 180) {
                    Actor_SE_set(&this->actor, NA_SE_EV_GANON_MANTLE);
                }

                if (this->csTimer == 190) {
                    Vec3f sp74 = this->actor.world.pos;

                    sp74.y = 4102.0f;
                    gn_ring_smoke_ct(play, &sp74, 0.2f, 0.7f);
                }

                if (this->csTimer == 230) {
                    this->csState = 102;
                    this->csTimer = 0;
                }
            }
            break;

        case 102:
            player->actor.world.pos.y = 4102.0f;

            this->csCamEye.x = -442.0f;
            this->csCamEye.y = 4152.0f;
            this->csCamEye.z = -135.0f;

            this->csCamAt.x = -472.0f;
            this->csCamAt.y = 4152.0f;
            this->csCamAt.z = -135.0f;

            if (this->csTimer == 5) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_76);
            }

            if (this->csTimer == 70) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_77);
            }

            if (this->csTimer == 90) {
                this->csState = 103;
                this->csTimer = 0;
                zelda->actor.world.pos.x = -472.0f;
                zelda->actor.world.pos.y = 4352.0f;
                zelda->actor.world.pos.z = -200.0f;
                zelda->unk_3C8 = 3;
            }
            break;

        case 103:
            Actor_SE_set(&zelda->actor, NA_SE_EV_DOWN_TO_GROUND - SFX_FLAG);
            add_calc2(&zelda->actor.world.pos.y, 4102.0f, 0.05f, 1.5f);

            this->csCamEye.x = -242.0f;
            this->csCamEye.y = 4122.0f;
            this->csCamEye.z = -190.0f;

            this->csCamAt.x = zelda->actor.world.pos.x;
            this->csCamAt.y = zelda->actor.world.pos.y + 40.0f + 5.0f;
            this->csCamAt.z = zelda->actor.world.pos.z;

            if (this->csTimer == 200) {
                zelda->actor.world.pos.y = 4102.0f;
                this->csState = 104;
                this->csTimer = 0;
            } else {
                break;
            }
            FALLTHROUGH;
        case 104:
            this->csCamEye.x = -432.0f;
            this->csCamEye.y = 4147.0f;
            this->csCamEye.z = -200.0f;

            this->csCamAt.x = zelda->actor.world.pos.x;
            this->csCamAt.y = zelda->actor.world.pos.y + 40.0f + 5.0f;
            this->csCamAt.z = zelda->actor.world.pos.z;

            if (this->csTimer >= 10) {
                add_calc0(&play->envCtx.lightBlend, 1.0f, 0.05f);
            }

            if (this->csTimer == 10) {
                zelda->unk_3C8 = 8;
            }

            if (this->csTimer == 50) {
                zelda->unk_3C8 = 4;
            }

            if (this->csTimer == 100) {
                this->csState = 105;
                this->csTimer = 0;
            }
            break;

        case 105:
            this->csCamEye.x = -450.0f;
            this->csCamEye.y = 4154.0f;
            this->csCamEye.z = -182.0f;

            this->csCamAt.x = zelda->actor.world.pos.x - 5.0f;
            this->csCamAt.y = zelda->actor.world.pos.y + 40.0f + 5.0f;
            this->csCamAt.z = zelda->actor.world.pos.z - 25.0f;

            if (this->csTimer == 10) {
                message_set(play, 0x70D0, NULL);
            }

            if ((this->csTimer > 100) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 1055;
                this->csTimer = 0;
            }
            break;

        case 1055:
            this->unk_70C = sin_s(this->csTimer * 0x6300) * 0.3f;
            Na_StartSystemSe_F(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            if (this->csTimer == 20) {
                zelda->unk_3C8 = 5;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_57);
            }

            if (this->csTimer == 40) {
                this->csState = 1056;
                this->csTimer = 0;
            }
            break;

        case 1056:
            this->unk_70C = sin_s(this->csTimer * 0x6300) * 0.3f;
            Na_StartSystemSe_F(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            this->csCamEye.x = -503.0f;
            this->csCamEye.y = 4128.0f;
            this->csCamEye.z = -162.0f;

            this->csCamAt.x = -416.0f;
            this->csCamAt.y = 4181.0f;
            this->csCamAt.z = -75.0f;

            if (this->csTimer > 40) {
                this->csState = 1057;
                this->csTimer = 0;
            }
            break;

        case 1057:
            this->unk_70C = sin_s(this->csTimer * 0x6300) * (50.0f * this->csCamMovementScale);
            Na_StartSystemSe_F(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            add_calc2(&this->csCamEye.x, -1200.0f, 0.1f, this->csCamMovementScale * 697.0f);
            add_calc2(&this->csCamEye.y, 4241.0f, 0.1f, this->csCamMovementScale * 113.0f);
            add_calc2(&this->csCamEye.z, -1048.0f, 0.1f, this->csCamMovementScale * 886.0f);

            add_calc2(&this->csCamMovementScale, 0.05f, 1.0f, 0.001f);

            if (this->csTimer > 80) {
                this->csState = 106;
                this->csTimer = 60;
            }
            break;

        case 106:
            this->csCamEye.x = -450.0f;
            this->csCamEye.y = 4154.0f;
            this->csCamEye.z = -182.0f;

            this->csCamAt.x = zelda->actor.world.pos.x - 5.0f;
            this->csCamAt.y = zelda->actor.world.pos.y + 40.0f + 5.0f;
            this->csCamAt.z = zelda->actor.world.pos.z - 25.0f;

            this->unk_70C = sin_s(this->csTimer * 0x6300) * 0.3f;
            Na_StartSystemSe_F(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            if (this->csTimer == 70) {
                zelda->unk_3C8 = 6;
            }

            if (this->csTimer == 90) {
                message_set(play, 0x70D1, NULL);
            }

            if ((this->csTimer > 150) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->csState = 107;
                this->csTimer = 0;
                message_set(play, 0x70D2, NULL);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_57);
            }
            break;

        case 107:
            this->unk_70C = sin_s(this->csTimer * 0x6300) * 0.8f;
            Na_StartSystemSe_F(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            this->csCamEye.x = -380.0f;
            this->csCamEye.y = 4154.0f;
            this->csCamEye.z = -242.0f;

            this->csCamAt.x = (zelda->actor.world.pos.x - 5.0f) - 30.0f;
            this->csCamAt.y = (zelda->actor.world.pos.y + 40.0f + 5.0f) - 20.0f;
            this->csCamAt.z = (zelda->actor.world.pos.z - 25.0f) + 80.0f;

            if ((this->csTimer > 50) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                zelda->unk_3C8 = 7;
                this->csState = 108;
                this->csTimer = 0;
            }
            break;

        case 108:
            this->unk_70C = sin_s(this->csTimer * 0x6300) * 0.8f;
            Na_StartSystemSe_F(NA_SE_EV_EARTHQUAKE - SFX_FLAG);

            this->csCamAt.x = (zelda->actor.world.pos.x - 5.0f) - 30.0f;
            this->csCamAt.y = (zelda->actor.world.pos.y + 40.0f + 5.0f) - 20.0f;
            this->csCamAt.z = (zelda->actor.world.pos.z - 25.0f) + 80.0f;

            if (this->csTimer > 50) {
                Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

                mainCam->eye = this->csCamEye;
                mainCam->eyeNext = this->csCamEye;
                mainCam->at = this->csCamAt;

                Gama_play_shift2main_camera(play, this->csCamIndex, 0);
                this->csState = 109;
                this->csCamIndex = 0;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                Actor_Environment_sw_On(play, 0x37);
            }
            break;

        case 109:
            Na_StartSystemSe_F(NA_SE_EV_EARTHQUAKE - SFX_FLAG);
            break;
    }

    if (this->csState >= 100) {
        this->envLightMode = 20;
    }

    if (this->csCamIndex != 0) {
        Vec3f sp64;

        if (moveCam) {
            add_calc2(&this->csCamEye.x, this->csCamTargetEye.x, this->csCamMovementScale,
                           this->csCamEyeMaxStep.x * this->csCamMaxStepScale);
            add_calc2(&this->csCamEye.y, this->csCamTargetEye.y, this->csCamMovementScale,
                           this->csCamEyeMaxStep.y * this->csCamMaxStepScale);
            add_calc2(&this->csCamEye.z, this->csCamTargetEye.z, this->csCamMovementScale,
                           this->csCamEyeMaxStep.z * this->csCamMaxStepScale);
            add_calc2(&this->csCamAt.x, this->csCamTargetAt.x, this->csCamMovementScale,
                           this->csCamAtMaxStep.x * this->csCamMaxStepScale);
            add_calc2(&this->csCamAt.y, this->csCamTargetAt.y, this->csCamMovementScale,
                           this->csCamAtMaxStep.y * this->csCamMaxStepScale);
            add_calc2(&this->csCamAt.z, this->csCamTargetAt.z, this->csCamMovementScale,
                           this->csCamAtMaxStep.z * this->csCamMaxStepScale);
        }

        sp64 = this->csCamAt;
        sp64.y += this->unk_70C;
        Gama_play_camera_setting(play, this->csCamIndex, &sp64, &this->csCamEye);
    }
}

void mode_Patack_init(BossGanon* this, PlayState* play) {
    this->unk_1C2 = 0;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->timers[0] = 40;
    this->actionFunc = mode_Patack;
    this->fwork[GDF_CENTER_POS] = 100.0f;
}

void mode_Patack(BossGanon* this, PlayState* play) {
    s16 i;
    f32 heightTarget;
    f32 targetPosX;
    f32 targetPosZ;
    Vec3f sp6C;
    Vec3f sp60;

    Skeleton_Info2_anime_play(&this->skelAnime);

    switch (this->unk_1C2) {
        case 0:
            targetPosX = sin_s(this->unk_1A2 * 1280) * this->fwork[GDF_CENTER_POS];
            targetPosZ = cos_s(this->unk_1A2 * 1792) * this->fwork[GDF_CENTER_POS];

            add_calc2(&this->actor.world.pos.x, targetPosX, 0.05f, this->fwork[GDF_FWORK_0]);
            add_calc2(&this->actor.world.pos.z, targetPosZ, 0.05f, this->fwork[GDF_FWORK_0]);
            add_calc2(&this->fwork[GDF_CENTER_POS], 0.0f, 1, 1.5f);

            if (this->timers[0] == 5) {
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_HIT_GND);
            }

            if (this->timers[0] < 14) {
                heightTarget = 250.0f;
                this->unk_258 += (rnd_f(M_PI / 2) + (M_PI / 2));
                add_calc2(&this->handLightBallScale, 7.0f, 0.5f, 1.0f);
                this->envLightMode = 1;
            } else {
                heightTarget = 200.0f;
            }

            add_calc2(&this->actor.world.pos.y, heightTarget, 0.1f, this->actor.velocity.y);
            add_calc2(&this->actor.velocity.y, 20.0f, 1.0f, 1.0f);

            if (this->timers[0] == 14) {
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfPoundAnim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfPoundAnim, 0.0f);
                this->actor.velocity.y = 0.0f;
            }

            if (this->timers[0] == 0) {
                this->unk_1C2 = 1;
                this->actor.velocity.y = 0.0f;
            }
            break;

        case 1:
            mant->gravity = -1.0f;
            this->envLightMode = 1;
            add_calc2(&this->actor.velocity.y, -50.0f, 1.0f, 10.0f);
            this->actor.world.pos.y += this->actor.velocity.y;

            if (this->actor.world.pos.y < 60.0f) {
                this->actor.world.pos.y = 60.0f;
                this->unk_1C2 = 2;
                this->timers[0] = 10;
                ActorQuakeRumbleSet(&this->actor, play, 10, 20);
                this->unk_19C = 35;
                this->unk_19E = 0;
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_HIT_GND_IMP);
                this->handLightBallScale = 0.0f;
                sp60 = this->unk_260;
                sp60.y = 0.0f;

                for (i = 0; i < 80; i++) {
                    sp6C.x = rnd_fx(25.0f);
                    sp6C.y = rnd_f(17.0f);
                    sp6C.z = rnd_fx(25.0f);
                    gn_hinoko_2_ct(play, &sp60, &sp6C, &zero, rnd_f(300.0f) + 500.0f, 13.0f,
                                               0x1E);
                }
            }
            break;

        case 2:
            this->envLightMode = 1;

            if (this->timers[0] == 0) {
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfPoundEndAnim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfPoundEndAnim, 0.0f);
                this->unk_1C2 = 3;
                this->unk_19F = 1;
                this->actor.velocity.y = 0.0f;
            }
            break;

        case 3:
            add_calc2(&this->actor.world.pos.y, 150.0f, 0.1f, this->actor.velocity.y);
            add_calc2(&this->actor.velocity.y, 20.0f, 1.0f, 1.0f);

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfGetUp3Anim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfGetUp3Anim, 0.0f);
                Skeleton_Info2_anime_play(&this->skelAnime);
                mant->attachShouldersTimer = 18.0f;
                Actor_SE_set(&this->actor, NA_SE_EV_GANON_MANTLE);
                this->unk_1C2 = 4;
            }
            break;

        case 4:
            add_calc2(&this->actor.world.pos.y, 150.0f, 0.1f, this->actor.velocity.y);
            add_calc2(&this->actor.velocity.y, 20.0f, 1.0f, 1.0f);

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                mode_flying_init(this, play);
            }
            break;
    }

    if ((this->unk_19C == 35) || (this->unk_19C == 30) || (this->unk_19C == 25)) {
        Vec3f sp54 = this->actor.world.pos;

        sp54.y = 0.0f;
        gn_ring_flash_ct(play, &sp54, 0, 3.0f, this->unk_19C - 25);
    }

    if (this->unk_19C == 35) {
        Vec3f sp48 = this->actor.world.pos;

        sp48.y = 0.0f;
        gn_ring_beam_ct(play, &sp48, 0, 3.0f);
    }
}

void mode_Batack_init(BossGanon* this, PlayState* play) {
    this->unk_1C2 = 0;
    this->actor.velocity.x = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->timers[0] = 30;
    this->fwork[GDF_CENTER_POS] = 100.0f;
    this->unk_1AA = rnd_f(20000.0f);
    this->unk_1AC = 0;
    this->actionFunc = mode_Batack;
}

void mode_Batack(BossGanon* this, PlayState* play) {
    s32 pad;
    f32 targetPosX;
    f32 targetPosZ;
    Vec3f sp80;
    Vec3f sp74;
    Vec3f sp68;
    s16 i;

    Skeleton_Info2_anime_play(&this->skelAnime);

    targetPosX = sin_s(this->unk_1A2 * 1280) * this->fwork[GDF_CENTER_POS];
    targetPosZ = cos_s(this->unk_1A2 * 1792) * this->fwork[GDF_CENTER_POS];

    add_calc2(&this->actor.world.pos.x, targetPosX, 0.05f, this->fwork[GDF_FWORK_0]);
    add_calc2(&this->actor.world.pos.z, targetPosZ, 0.05, this->fwork[GDF_FWORK_0]);

    add_calc2(&this->fwork[GDF_CENTER_POS], 0.0f, 1.0f, 1.5f);
    add_calc2(&this->actor.world.pos.y, 200.0f, 0.05f, this->actor.velocity.y);
    add_calc2(&this->actor.velocity.y, 20.0f, 1.0f, 1.0f);

    switch (this->unk_1C2) {
        case 0:
            if (this->timers[0] == 0) {
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfBigMagicChargeStartAnim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfBigMagicChargeStartAnim, 0.0f);
                this->unk_1C2 = 1;
            }
            break;

        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfBigMagicChargeHoldAnim);
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfBigMagicChargeHoldAnim, 0.0f);
                this->unk_1C2 = 2;
                this->timers[0] = 100;
            }
            break;

        case 2:
            this->envLightMode = 2;
            Actor_SE_set(&this->actor, NA_SE_EN_GANON_CHARGE_MASIC - SFX_FLAG);
            this->unk_278.x = this->unk_2EC[0].x;
            this->unk_278.y = this->unk_2EC[0].y + 50.0f + 30.0f;
            this->unk_278.z = this->unk_2EC[0].z;

            add_calc2(&this->unk_284, 0.25f, 0.1f, 0.006f);
            add_calc2(&this->unk_288, 255.0f, 1.0f, 255.0f);
            add_calc2(&this->unk_28C, 0.25f, 0.1f, 0.006f);

            if ((this->timers[0] > 20) && (this->timers[0] < 60)) {
                add_calc2(&this->unk_290, 255.0f, 1.0f, 15.0f);
            }

            if (this->timers[0] == 0) {
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfBigMagicWindupAnim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfBigMagicWindupAnim, 0.0f);
                this->unk_1C2 = 3;
                this->timers[0] = 6;
                this->timers[1] = 15;
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_DARKWAVE);
                break;
            }

            adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x3E8);

            if (this->timers[0] < -4) {
                for (i = 0; i < ARRAY_COUNT(this->unk_294); i++) {
                    add_calc2(&this->unk_294[i], 0.0f, 1.0f, 40.0f);
                }
            } else if ((this->timers[0] >= 7) && (this->timers[0] < 26)) {
                if (this->unk_1AC < ARRAY_COUNT(this->unk_294)) {
                    this->unk_1AC++;
                }

                for (i = 0; i < this->unk_1AC; i++) {
                    add_calc2(&this->unk_294[i], 200.0f, 1.0f, 40.0f);
                }
            }

            if (this->timers[0] <= 30) {
                add_calc2(&this->unk_284, 0.4f, 0.5f, 0.017f);
                this->unk_28C = this->unk_284;
            }

            if (this->timers[0] <= 30) {
                add_calc2(&this->unk_2D0, 45.0f, 0.1f, 10.0f);
                this->lensFlareTimer = 1;
                this->lensFlareMode = 2;
                E_lf_position = this->unk_278;
            }

            if (this->timers[0] == 47) {
                this->unk_274 = 1;
            }

            if (this->timers[0] == 46) {
                this->unk_274 = 2;
            }

            if (this->timers[0] == 45) {
                this->unk_274 = 3;
            }

            if (this->timers[0] == 44) {
                this->unk_274 = 4;
            }

            if (this->timers[0] == 43) {
                this->unk_274 = 5;
            }

            if (this->timers[0] == 42) {
                this->unk_274 = 6;
            }

            if (this->timers[0] > 30) {
                sp74.x = 0.0f;
                sp74.y = rnd_f(10.0f) + 150.0f;
                sp74.z = 0.0f;

                Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.yawTowardsPlayer), MTXMODE_NEW);
                Matrix_rotateZ(rnd_f(65536.0f), MTXMODE_APPLY);
                Matrix_Position(&sp74, &sp68);

                sp80.x = this->unk_278.x + sp68.x;
                sp80.y = this->unk_278.y + sp68.y;
                sp80.z = this->unk_278.z + sp68.z;

                gn_magic_ball_ct(play, &sp80, 20.0f);
            }
            break;

        case 3:
            this->envLightMode = 2;

            for (i = 0; i < ARRAY_COUNT(this->unk_294); i++) {
                add_calc2(&this->unk_294[i], 0.0f, 1.0f, 40.0f);
            }

            if (this->timers[0] == 1) {
                mant->attachLeftArmTimer = 15.0f;
                Actor_SE_set(&this->actor, NA_SE_EV_GANON_MANTLE);
            }

            if (this->timers[0] == 0) {
                add_calc0(&this->unk_284, 1.0f, 0.08f);
                this->unk_28C = this->unk_284;
                add_calc0(&this->unk_2D0, 1.0f, 10.0f);
                add_calc2(&this->unk_278.x, this->unk_1FC.x, 0.5f, 30.0f);
                add_calc2(&this->unk_278.y, this->unk_1FC.y, 0.5f, 30.0f);
                add_calc2(&this->unk_278.z, this->unk_1FC.z, 0.5f, 30.0f);
            }

            if (this->timers[1] == 0) {
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfBigMagicThrowAnim);
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfBigMagicThrowAnim, 0.0f);
                this->unk_1C2 = 4;
                this->unk_288 = 0.0f;
                this->unk_290 = 0.0f;
                this->unk_284 = 0.0f;
                this->unk_28C = 0.0f;
            }
            break;

        case 4:
            this->envLightMode = 2;

            if (Skeleton_Info_frame_check(&this->skelAnime, 5.0f)) {
                for (i = 0; i < 5; i++) {
                    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_GANON, this->unk_1FC.x,
                                       this->unk_1FC.y, this->unk_1FC.z, 0, this->actor.yawTowardsPlayer, 0, 0x104 + i);
                }

                Actor_SE_set(&this->actor, NA_SE_EN_GANON_BIGMASIC);
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_THROW_BIG);
            }

            if (Skeleton_Info_frame_check(&this->skelAnime, 3.0f)) {
                mant->attachShouldersTimer = 26.0f;
                Actor_SE_set(&this->actor, NA_SE_EV_GANON_MANTLE);
            }

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfBigMagicThrowEndAnim);
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfBigMagicThrowEndAnim, 0.0f);
                this->unk_1C2 = 5;
            }
            break;

        case 5:
            this->envLightMode = 2;

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                mode_flying_init(this, play);
            }
            break;
    }
}

static void mode_flying_init(BossGanon* this, PlayState* play) {
    obj_bank_set(this, play, OBJECT_GANON_ANIME1);
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfFloatAnim, -10.0f);
    this->actionFunc = mode_flying;
    this->fwork[GDF_FWORK_0] = 0.0f;
    this->timers[0] = (s16)rnd_f(64.0f) + 30;
    this->unk_1C2 = 0;
    mant->minY = 2.0f;
}

static void mode_flying(BossGanon* this, PlayState* play) {
    f32 sin;
    s32 pad;
    f32 cos;
    Player* player = GET_PLAYER(play);

    this->legSwayEnabled = true;

    mant->backPush = -3.0f;
    mant->backSwayMagnitude = 0.25f;
    mant->sideSwayMagnitude = -3.0f;
    mant->minDist = 20.0f;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((this->unk_1C2 == 0) && !(player->actor.world.pos.y < 0.0f)) {
        if (!(player->stateFlags1 & PLAYER_STATE1_13) && (fabsf(player->actor.world.pos.x) < 110.0f) &&
            (fabsf(player->actor.world.pos.z) < 110.0f)) {
            mode_Patack_init(this, play);
        } else if ((this->timers[0] == 0) && !(player->stateFlags1 & PLAYER_STATE1_13)) {
            this->timers[0] = (s16)rnd_f(30.0f) + 30;

            if ((s8)this->actor.colChkInfo.health >= 20) {
                mode_preatack_init(this, play);
            } else if (fqrand() >= 0.5f) {
                if ((fqrand() >= 0.5f) || (this->actor.xzDistToPlayer > 350.0f)) {
                    mode_Batack_init(this, play);
                } else {
                    mode_Patack_init(this, play);
                }
            } else {
                mode_preatack_init(this, play);
            }
        }
    }

    sin = sin_s(this->unk_1A2 * 1280) * 100.0f;
    cos = cos_s(this->unk_1A2 * 1792) * 100.0f;

    add_calc2(&this->actor.world.pos.x, sin, 0.05f, this->fwork[GDF_FWORK_0]);
    add_calc2(&this->actor.world.pos.y, 150.0f, 0.05f, this->fwork[GDF_FWORK_0] * 0.2f);
    add_calc2(&this->actor.world.pos.z, cos, 0.05f, this->fwork[GDF_FWORK_0]);
    add_calc2(&this->fwork[GDF_FWORK_0], 50.0f, 1.0f, 0.5f);

    this->actor.velocity.x = this->actor.world.pos.x - this->actor.prevPos.x;
    this->actor.velocity.z = this->actor.world.pos.z - this->actor.prevPos.z;

    sin = sin_s(this->unk_1A2 * 1500);
    this->actor.velocity.y = this->fwork[GDF_FWORK_0] * sin * 0.04f;
    this->actor.world.pos.y += this->actor.velocity.y;

    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0xBB8);
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EN_FANTOM_FLOAT - SFX_FLAG);
}

void mode_preatack_init(BossGanon* this, PlayState* play) {
    obj_bank_set(this, play, OBJECT_GANON_ANIME1);
    this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfChargeLightBallAnim);
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfChargeLightBallAnim, -3.0f);
    this->actionFunc = mode_preatack;
    this->timers[0] = 25;
}

void mode_preatack(BossGanon* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    mant->backPush = -3.0f;
    mant->backSwayMagnitude = 1.25f;
    mant->sideSwayMagnitude = -2.0f;
    mant->minDist = 10.0f;

    if (this->timers[0] < 17) {
        this->envLightMode = 1;
    }

    if (this->timers[0] == 17) {
        this->unk_26C = 10;
        this->unk_270 = rnd_f(M_PI);
        Actor_SE_set(&this->actor, NA_SE_EN_GANON_SPARK);
    }

    if (this->timers[0] < 10) {
        this->unk_258 += (rnd_f(M_PI / 2) + (M_PI / 2));
        add_calc2(&this->handLightBallScale, 10.0f, 0.5f, 1.25f);

        if (this->timers[0] == 0) {
            mode_atack2_init(this, play);
        }
    }

    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x7D0);

    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.z += this->actor.velocity.z;

    add_calc0(&this->actor.velocity.x, 1.0f, 0.5f);
    add_calc0(&this->actor.velocity.z, 1.0f, 0.5f);

    this->actor.velocity.y = sin_s(this->unk_1A2 * 1500) * 2.0f;
    this->actor.world.pos.y += this->actor.velocity.y;
}

static void mode_atack2_init(BossGanon* this, PlayState* play) {
    obj_bank_set(this, play, OBJECT_GANON_ANIME1);
    this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfThrowAnim);
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfThrowAnim, 0.0f);
    this->actionFunc = mode_atack2;
}

static void mode_atack2(BossGanon* this, PlayState* play) {
    static AnimationHeader* revise_anm_tbl[] = { &gGanondorfVolleyLeftAnim, &gGanondorfVolleyRightAnim };
    static s16 mant_hand_time[] = { 26, 20 };
    s16 rand;

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc0(&this->handLightBallScale, 1.0f, 0.2f);

    switch (this->unk_1C2) {
        case 0:
            this->envLightMode = 1;

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 1;
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfFloatAnim, 0.0f);
            }

            if (this->skelAnime.curFrame <= 12.0f) {
                this->lensFlareTimer = 2;
                this->lensFlareMode = 2;
                E_lf_position = this->unk_260;
            }

            if (Skeleton_Info_frame_check(&this->skelAnime, 12.0f)) {
                this->handLightBallScale = 0.0f;
            }

            if (Skeleton_Info_frame_check(&this->skelAnime, 11.0f)) {
                this->unk_25C = 1;
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_THROW);
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_THROW_MASIC);
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_GANON, this->unk_260.x,
                                   this->unk_260.y, this->unk_260.z, 0, 0, 0, 0x64);
            }
            break;

        case 1:
            if (this->startVolley) {
                rand = fqrand() * 1.99f;
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(revise_anm_tbl[rand]);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, revise_anm_tbl[rand], 0.0f);
                mant->attachRightArmTimer = mant_hand_time[rand];
                Actor_SE_set(&this->actor, NA_SE_EV_GANON_MANTLE);
                this->startVolley = false;
            }
            break;
    }

    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x7D0);

    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.z += this->actor.velocity.z;

    add_calc(&this->actor.velocity.x, 0.0f, 1.0f, 0.5f, 0.0f);
    add_calc(&this->actor.velocity.z, 0.0f, 1.0f, 0.5f, 0.0f);

    this->actor.velocity.y = sin_s(this->unk_1A2 * 1500) * 2.0f;
    this->actor.world.pos.y += this->actor.velocity.y;
}

static void mode_guard_init(BossGanon* this, PlayState* play) {
    if ((this->actionFunc != mode_guard) || (this->unk_1C2 != 0)) {
        obj_bank_set(this, play, OBJECT_GANON_ANIME1);
        this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfBlockAnim);
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfBlockAnim, 0.0f);
        this->actionFunc = mode_guard;
    }

    this->unk_1C2 = 0;
    mant->attachLeftArmTimer = this->timers[0] = 10;
    Actor_SE_set(&this->actor, NA_SE_EV_GANON_MANTLE);
    this->handLightBallScale = 0.0f;
}

static void mode_guard(BossGanon* this, PlayState* play) {
    this->collider.base.colMaterial = COL_MATERIAL_METAL;
    Skeleton_Info2_anime_play(&this->skelAnime);
    mant->backPush = -9.0f;
    mant->backSwayMagnitude = 0.25f;
    mant->sideSwayMagnitude = -2.0f;
    mant->minDist = 13.0f;

    if (this->unk_1C2 == 0) {
        if (this->timers[0] == 0) {
            this->unk_1C2 = 1;
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfBlockReleaseAnim, 0.0f);
            this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfBlockReleaseAnim);
            Skeleton_Info2_anime_play(&this->skelAnime);
            mant->attachShouldersTimer = 15.0f;
            Actor_SE_set(&this->actor, NA_SE_EV_GANON_MANTLE);
        }
    } else {
        mant->sideSwayMagnitude = -13.0f;

        if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
            mode_flying_init(this, play);
        }
    }

    this->actor.world.pos.x += this->actor.velocity.x;
    this->actor.world.pos.y += this->actor.velocity.y;
    this->actor.world.pos.z += this->actor.velocity.z;

    add_calc0(&this->actor.velocity.x, 1.0f, 0.5f);
    add_calc0(&this->actor.velocity.y, 1.0f, 0.5f);
    add_calc0(&this->actor.velocity.z, 1.0f, 0.5f);
}

void mode_ballhit_init(BossGanon* this, PlayState* play) {
    s16 i;

    obj_bank_set(this, play, OBJECT_GANON_ANIME1);
    this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfBigMagicHitAnim);
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfBigMagicHitAnim, 0);
    this->timers[0] = 70;
    mant->attachRightArmTimer = mant->attachLeftArmTimer = 0;

    for (i = 1; i < 15; i++) {
        this->unk_4E4[i] = shape_flash_time_data[i];
    }

    this->unk_2E6 = 80;
    this->unk_2E8 = 0;
    this->actionFunc = mode_ballhit;
    this->actor.velocity.x = this->actor.velocity.z = 0.0f;
    this->unk_1C2 = 0;
    this->unk_1A6 = 15;
    this->unk_508 = 6.0f;
}

void mode_ballhit(BossGanon* this, PlayState* play) {
    s16 i;
    Vec3f sp50;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->unk_1C2 == 0) {
        gn_denki_ct(play, 1500.0f, GDF_SHOCK_DORF_YELLOW);

        if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
            this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfLightArrowWaitAnim);
            Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfLightArrowWaitAnim, 0.0f);
            this->unk_1C2 = 1;
        }
    } else if (this->unk_1C2 == 1) {
        gn_denki_ct(play, 1000.0f, GDF_SHOCK_DORF_YELLOW);

        if (this->timers[0] == 0) {
            this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfGetUp3Anim);
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfGetUp3Anim, 0.0f);
            this->unk_1C2 = 2;
            Skeleton_Info2_anime_play(&this->skelAnime);
            mant->attachShouldersTimer = 18.0f;
            Actor_SE_set(&this->actor, NA_SE_EV_GANON_MANTLE);
            Actor_SE_set(&this->actor, NA_SE_EN_GANON_RESTORE);
            this->timers[2] = 130;
        }
    } else {
        if (Skeleton_Info_frame_check(&this->skelAnime, 7.0f)) {
            for (i = 0; i < 100; i++) {
                sp50.x = rnd_fx(25.0f);
                sp50.y = rnd_fx(25.0f);
                sp50.z = rnd_fx(25.0f);

                gn_hinoko_ct(play, &this->unk_1FC, &sp50, &zero, rnd_f(200.0f) + 500.0f,
                                          0x14);
            }
            Actor_SE_set(&this->actor, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_TALL_GRASS);
        }

        if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
            mode_flying_init(this, play);
        }
    }

    this->actor.velocity.y = sin_s(this->unk_1A2 * 1500) * 2.0f;
    this->actor.world.pos.y += this->actor.velocity.y;
}

static void mode_yahit_init(BossGanon* this, PlayState* play) {
    s16 i;

    if (this->actionFunc != mode_yahit) {
        obj_bank_set(this, play, OBJECT_GANON_ANIME1);
        this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfLightArrowHitAnim);
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfLightArrowHitAnim, 0.0f);
        mant->attachRightArmTimer = mant->attachLeftArmTimer = 0;
        this->actionFunc = mode_yahit;

        this->actor.velocity.x = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.z = 0.0f;

        this->unk_1C2 = 0;
        mant->backPush = -4.0f;
        mant->backSwayMagnitude = 0.75f;
        mant->sideSwayMagnitude = -3.0f;
        mant->minDist = 20.0f;

        for (i = 0; i < 10; i++) {
            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_GANON, this->unk_1FC.x, this->unk_1FC.y,
                               this->unk_1FC.z, rnd_fx(0x8000),
                               (s16)rnd_fx(0x8000) + this->actor.yawTowardsPlayer, 0, 0xC8 + i);
        }

        this->unk_1A4 = 0;
        this->unk_288 = 0.0f;
        this->unk_290 = 0.0f;
        this->unk_284 = 0.0f;
        this->unk_28C = 0.0f;
    }
}

static void mode_yahit(BossGanon* this, PlayState* play) {
    s16 i;
    Vec3f sp40;

    if (this->timers[3] == 0) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);

    this->envLightMode = 1;
    this->actor.world.pos.y += this->actor.velocity.y;

    if (this->unk_1A4 < 0x28) {
        add_calc2(&this->unk_508, 4.0f, 0.1f, 0.1f);
    }

    if ((this->unk_1A4 >= 0x28) && (this->unk_1A4 < 0x37)) {
        add_calc2(&this->unk_508, 0.0f, 1.0f, 0.5f);
    }

    if (this->unk_1A4 >= 0x37) {
        add_calc2(&this->unk_508, 5.0f, 0.1f, 0.15f);
        this->shockGlow = true;
    }

    switch (this->unk_1C2) {
        case 0:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 1;
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfLightEnergyHitAnim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfLightEnergyHitAnim, 0.0f);
            }
            break;

        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 2;
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfDownedAnim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfDownedAnim, 0.0f);
            }
            break;

        case 2:
            mant->minDist = 0.0f;
            this->actor.velocity.y -= 0.5f;

            if (this->actor.world.pos.y < 40.0f) {
                this->actor.world.pos.y = 40.0f;
                this->actor.velocity.y = 0.0f;
                this->unk_1C2 = 3;
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfLandAnim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfLandAnim, 0.0f);
                this->timers[0] = 70;
                this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
            }
            break;

        case 3:
            if (this->timers[0] == 68) {
                this->unk_19F = 1;
            }

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 4;
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfVulnerableAnim);
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfVulnerableAnim, 0.0f);
            }
            break;

        case 4:
            if (Skeleton_Info_frame_check(&this->skelAnime, 5.0f)) {
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_DOWN);
            }

            if (this->timers[0] == 0) {
                this->unk_1C2 = 5;
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfGetUp1Anim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfGetUp1Anim, 0.0f);
                this->unk_2D4 = 80;

                for (i = 1; i < 15; i++) {
                    this->unk_4E4[i] = rnd_f(10.0f);
                }

                this->unk_2E6 = 80;
                this->unk_2E8 = 0;
                this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
            }
            break;

        case 5:
            gn_denki_ct(play, 1000.0f, GDF_SHOCK_DORF_YELLOW);

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 6;
                this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfGetUp2Anim);
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfGetUp2Anim, 0.0f);
                mant->minDist = 20.0f;
                this->unk_19F = 1;
            }
            break;

        case 6:
            this->envLightMode = 0;
            add_calc2(&this->actor.world.pos.y, 200.0f, 0.1f, 1000.0f);

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                this->unk_1C2 = 7;
            }
            break;

        case 7:
            this->envLightMode = 0;
            add_calc2(&this->actor.world.pos.y, 150.0f, 0.05f, 30.0f);
            this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfGetUp3Anim);
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfGetUp3Anim, 0.0f);
            this->unk_1C2 = 8;
            Skeleton_Info2_anime_play(&this->skelAnime);
            mant->attachShouldersTimer = 18.0f;
            Actor_SE_set(&this->actor, NA_SE_EV_GANON_MANTLE);
            Actor_SE_set(&this->actor, NA_SE_EN_GANON_RESTORE);
            break;

        case 8:
            this->envLightMode = 0;

            if (Skeleton_Info_frame_check(&this->skelAnime, 7.0f)) {
                for (i = 0; i < 100; i++) {
                    sp40.x = rnd_fx(25.0f);
                    sp40.y = rnd_fx(25.0f);
                    sp40.z = rnd_fx(25.0f);
                    gn_hinoko_ct(play, &this->unk_1FC, &sp40, &zero, rnd_f(200.0f) + 500.0f,
                                              0x14);
                }

                Actor_SE_set(&this->actor, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_TALL_GRASS);
                this->timers[3] = 50;
            }

            if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
                mode_flying_init(this, play);
            }
            break;
    }
}

void mode_kenhit_init(BossGanon* this, PlayState* play) {
    obj_bank_set(this, play, OBJECT_GANON_ANIME1);
    this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfDamageAnim);
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfDamageAnim, 0.0f);
    this->actionFunc = mode_kenhit;
}

void mode_kenhit(BossGanon* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->unk_1A4 >= 0x37) {
        add_calc2(&this->unk_508, 5.0f, 0.1f, 0.15f);
        this->shockGlow = true;
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, this->fwork[GDF_FWORK_1])) {
        this->actionFunc = mode_yahit;
        this->unk_1C2 = 4;
        this->fwork[GDF_FWORK_1] = Si2_anime_end_frame(&gGanondorfVulnerableAnim);
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfVulnerableAnim, 0.0f);
    }
}

void Ganon_Damage_Check(BossGanon* this, PlayState* play) {
    s16 i;
    s16 j;
    ColliderElement* acHitElem;

    if (this->collider.base.acFlags & AC_HIT) {
        this->unk_2D4 = 2;
        this->collider.base.acFlags &= ~AC_HIT;
        acHitElem = this->collider.elem.acHitElem;

        if ((this->actionFunc == mode_ballhit) || (this->actionFunc == mode_Batack)) {
            if (acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_LIGHT) {
                mode_yahit_init(this, play);
                this->timers[2] = 0;
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_DAMAGE1);
                this->unk_1A6 = 15;
            }
        } else if ((this->actionFunc == mode_yahit) && (this->unk_1C2 >= 3)) {
            if (!(acHitElem->atDmgInfo.dmgFlags & DMG_HOOKSHOT)) {
                u8 hitWithSword = false;
                u8 damage;
                Vec3f sp50;
                u32 flags;

                for (i = 0; i < 30; i++) {
                    sp50.x = rnd_fx(20.0f);
                    sp50.y = rnd_fx(20.0f);
                    sp50.z = rnd_fx(20.0f);
                    gn_hinoko_ct(play, &this->unk_1FC, &sp50, &zero, rnd_f(200.0f) + 500.0f,
                                              0x1E);
                }

                damage = flags = GetSwordAP(acHitElem->atDmgInfo.dmgFlags);

                if (flags == 0) {
                    damage = 2;
                } else {
                    hitWithSword = true;
                }

                if (((s8)this->actor.colChkInfo.health >= 3) || hitWithSword) {
                    this->actor.colChkInfo.health -= damage;
                }

                for (i = 0; i < ARRAY_COUNT(mant->strands); i++) {
                    for (j = 1; j < 12; j++) {
                        mant->strands[i].velocities[j].x = rnd_fx(15.0f);
                        mant->strands[i].velocities[j].z = rnd_fx(15.0f);
                    }
                }

                if ((s8)this->actor.colChkInfo.health <= 0) {
                    mode_enddemo_init(this, play);
                    Actor_SE_set(&this->actor, NA_SE_EN_GANON_DEAD);
                    Actor_SE_set(&this->actor, NA_SE_EN_GANON_DD_THUNDER);
                    Na_StartObjectSe_F(&zero, NA_SE_EN_LAST_DAMAGE);
                    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
                    this->screenFlashTimer = 4;
                } else {
                    Actor_SE_set(&this->actor, NA_SE_EN_GANON_DAMAGE2);
                    Actor_SE_set(&this->actor, NA_SE_EN_GANON_CUTBODY);
                    mode_kenhit_init(this, play);
                    this->unk_1A6 = 15;
                    mant->tearTimer = 1;
                }
            }
        } else if (acHitElem->atDmgInfo.dmgFlags & DMG_RANGED) {
            Actor_SE_set(&this->actor, 0);

            for (i = 0; i < ARRAY_COUNT(mant->strands); i++) {
                for (j = 1; j < 12; j++) {
                    mant->strands[i].velocities[j].x = rnd_fx(15.0f);
                    mant->strands[i].velocities[j].z = rnd_fx(15.0f);
                }
            }
        }
    }
}

static f32 hand_beam_yad[] = {
    1.0f, 3.0f, 0.0f, 7.0f, 13.0f, 4.0f, 6.0f, 11.0f, 5.0f, 2.0f, 8.0f, 14.0f, 10.0f, 12.0f, 9.0f,
};

void Boss_Ganon_Actor_move(Actor* thisx, PlayState* play2) {
    BossGanon* this = (BossGanon*)thisx;
    PlayState* play = play2;
    f32 legRotX;
    f32 legRotY;
    f32 legRotZ;
    Player* player = GET_PLAYER(play);
    s16 i;
    f32 sin;
    f32 cos;
    Vec3f shardPos;
    Vec3f shardVel;
    Vec3f spE8;
    s16 i2;
    s16 j;
    Vec3f spD8;
    Vec3f platformCheckPos;
    Actor* explosive;
    Vec3f spBC;
    Vec3f spB0;
    Vec3f platCheckPosBomb;
    Actor* prop;
    BgGanonOtyuka* platform;
    f32 targetLensFlareScale;
    f32 xOffset;
    f32 zOffset;

    if ((this->actionFunc != mode_startdemo) && (this->actionFunc != mode_enddemo)) {
        obj_bank_set(this, play, OBJECT_GANON_ANIME1);
    } else {
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->animObjectSlot].segment);
    }

    if (this->windowShatterState != GDF_WINDOW_SHATTER_OFF) {
        enddemo_kabe_break(this->windowShatterState);
        shardVel.y = 0.0f;

        for (i = 0; i < 10; i++) {
            shardPos.y = rnd_f(240.0f) + 20.0f;

            if (fqrand() < 0.5f) {
                shardPos.x = 463;
                shardPos.z = rnd_f(463.0f);

                shardVel.x = rnd_f(2.0f);
                shardVel.z = rnd_f(1.0f);
            } else {
                shardPos.z = 463;
                shardPos.x = rnd_f(463.0f);

                shardVel.z = rnd_f(2.0f);
                shardVel.x = rnd_f(1.0f);
            }

            gn_hahen_ct(play, &shardPos, &shardVel, rnd_f(0.075f) + 0.08f);
        }
    }

    this->collider.base.colMaterial = COL_MATERIAL_HIT3;
    mant->gravity = -3.0f;
    this->shockGlow = false;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_1A2++;
    this->unk_1A4++;

    // block players attack if hes shooting something
    if ((this->actionFunc == mode_flying) || (this->actionFunc == mode_guard)) {
        if (player->unk_A73 != 0) {
            mode_guard_init(this, play);
        }
    }

    this->actionFunc(this, play);

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }

    if (this->unk_1A6 != 0) {
        this->unk_1A6--;
    }

    if (this->unk_2D4 != 0) {
        this->unk_2D4--;
    }

    if (this->unk_2E8 != 0) {
        this->unk_2E8--;
    }

    if (this->unk_2E6 != 0) {
        this->unk_2E6--;
    }

    if (this->unk_19C != 0) {
        this->unk_19C--;
    }

    if (this->csState == 0) {
        Ganon_Damage_Check(this, play);
        CollisionCheck_Uty_PosSetPipeC(&this->unk_1FC, &this->collider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

        if (this->unk_2D4 == 0) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);

            if ((this->actionFunc != mode_ballhit) && (this->actionFunc != mode_yahit) &&
                (this->actionFunc != mode_kenhit)) {
                CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
            }
        }
    }

    if (this->legSwayEnabled) {
        sin = sin_s(-this->actor.shape.rot.y);
        cos = cos_s(-this->actor.shape.rot.y);

        legRotX = ((this->actor.velocity.z * sin) + (cos * this->actor.velocity.x)) * 300.0f;
        legRotY = ((-sin * this->actor.velocity.x) + (cos * this->actor.velocity.z)) * 300.0f;
        legRotZ = (sin_s(this->unk_1A2 * 2268) * -500.0f) - 500.0f;
    } else {
        legRotY = legRotX = legRotZ = 0.0f;
    }

    this->legSwayEnabled = false;

    add_calc(&this->legRot.x, legRotX, 1.0f, 600.0f, 0.0f);
    add_calc(&this->legRot.y, legRotY, 1.0f, 600.0f, 0.0f);
    add_calc(&this->legRot.z, legRotZ, 1.0f, 100.0f, 0.0f);

    if (this->timers[2] == 1) {
        Actor_SE_set(&this->actor, NA_SE_EN_GANON_LAUGH);
    }

    if (this->timers[2] == 100) {
        Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_ST_LAUGH);
        this->timers[2] = 0;
    }

    if ((this->unk_2E6 != 0) || (this->unk_2E8 != 0)) {
        for (i = 1; i < ARRAY_COUNT(this->unk_49C); i++) {
            if (this->unk_4E4[i] != 0) {
                this->unk_4E4[i]--;
                add_calc2(&this->unk_49C[i], this->unk_508, 1.0f, 2.0f);
            } else {
                add_calc0(&this->unk_49C[i], 1.0f, 0.2f);
            }
        }

        // player hit, spawn shock and play sound effect
        if (this->unk_2E8 != 0) {
            Na_StartObjectSe_F(&player->actor.projectedPos, NA_SE_PL_SPARK - SFX_FLAG);
            gn_denki_ct(play, 700.0f, GDF_SHOCK_PLAYER_YELLOW);
        }
    }

    if (this->unk_19F != 0) {
        this->unk_19F = 0;
        spE8 = this->actor.world.pos;
        spE8.y = 0.0f;
        gn_ring_smoke_ct(play, &spE8, 0.2, 0.7f);
        gn_ring_smoke_ct(play, &spE8, 0.3f, 0.8f);
    }

    if (this->unk_26C != 0) {
        this->unk_26C--;

        if (this->unk_26C == 0) {
            gn_hand_beam_ct(play, 1.0f, 0.0f, 0.0f);
        }

        gn_hand_beam_ct(play, 1.0f, hand_beam_yad[this->unk_26C] * (M_PI / 5) + this->unk_270,
                                    rnd_fx(M_PI / 5) + (M_PI / 2));
    }

    // see if light ball hit and should knock platform down?
    if ((this->unk_19C != 0) && (this->unk_19E < 4)) {
        if ((this->unk_19A == 0) && (this->unk_19C == 20)) {
            this->unk_19A = 1;
            platformCheckPos.x = -180.0f;
            platformCheckPos.y = 0.0f;

            for (i2 = 0; i2 < 4; i2++) {
                for (j = 0, platformCheckPos.z = -180.0f; j < 4; j++) {
                    otyuka_check(this, play, &platformCheckPos);
                    platformCheckPos.z += 120.0f;
                }

                platformCheckPos.x += 120.0f;
            }
        } else if (this->unk_19C < 30) {
            spD8.x = 0.0f;
            spD8.y = 0.0f;
            spD8.z = 15.0f * (30.0f - this->unk_19C);

            Matrix_rotateY(rnd_f(6.2831855f), MTXMODE_NEW);
            Matrix_Position(&spD8, &platformCheckPos);

            this->unk_19E += otyuka_check(this, play, &platformCheckPos);
        }
    }

    // see if a bomb exploded near a group of platforms and if they should fall
    explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;

    while (explosive != NULL) {
        if (explosive->params != BOMB_EXPLOSION) {
            explosive = explosive->next;
        } else {
            for (i = 0; i < 8; i++) {
                spBC.x = 0.0f;
                spBC.y = 0.0f;
                spBC.z = 60.0f;

                Matrix_rotateY(i * (M_PI / 4), MTXMODE_NEW);
                Matrix_Position(&spBC, &spB0);

                platCheckPosBomb.x = explosive->world.pos.x + spB0.x;
                platCheckPosBomb.y = explosive->world.pos.y;
                platCheckPosBomb.z = explosive->world.pos.z + spB0.z;

                otyuka_check(this, play, &platCheckPosBomb);
            }

            explosive = explosive->next;
        }
    }

    Gn_Eff_move(play);

    prop = play->actorCtx.actorLists[ACTORCAT_PROP].head;

    // if a platform is lit up, change the room lighting
    while (prop != NULL) {
        if (prop->id != ACTOR_BG_GANON_OTYUKA) {
            prop = prop->next;
        } else {
            platform = (BgGanonOtyuka*)prop;

            if (platform->flashState != 0) {
                this->envLightMode = 1;
                break;
            }

            prop = prop->next;
        }
    }

    play->envCtx.lightSettingOverride = 0;
    play->envCtx.prevLightSetting = 0;
    play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_FULL_CONTROL;

    switch (this->envLightMode) {
        case -1:
            break;
        case 0:
            add_calc2(&play->envCtx.lightBlend, 0.0f, 1.0f, 0.02f);
            break;
        case 1:
            play->envCtx.lightSetting = 1;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.1f);
            break;
        case 2:
            play->envCtx.lightSetting = 1;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.02f);
            break;
        case 3:
            play->envCtx.lightSetting = 3;
            play->envCtx.lightBlend = 1.0f;
            break;
        case 35:
            play->envCtx.lightSetting = 0;
            play->envCtx.lightBlend = 1.0f;
            break;
        case 4:
            play->envCtx.lightSetting = 4;
            play->envCtx.lightBlend = 1.0f;
            break;
        case 5:
            play->envCtx.prevLightSetting = 5;
            play->envCtx.lightSetting = 3;
            add_calc0(&play->envCtx.lightBlend, 1.0f, 0.075f);
            break;
        case 6:
            play->envCtx.prevLightSetting = 5;
            play->envCtx.lightBlend = 0.0f;
            break;
        case 65:
            play->envCtx.prevLightSetting = 3;
            play->envCtx.lightSetting = 6;
            add_calc0(&play->envCtx.lightBlend, 1.0f, 0.05f);
            break;
        case 7:
            play->envCtx.prevLightSetting = 7;
            play->envCtx.lightBlend = 0.0f;
            break;
        case 75:
            play->envCtx.prevLightSetting = 4;
            play->envCtx.lightSetting = 8;
            add_calc0(&play->envCtx.lightBlend, 1.0f, 0.05f);
            break;
        case 8:
            play->envCtx.prevLightSetting = 3;
            play->envCtx.lightSetting = 9;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.05f);
            break;
        case 9:
            play->envCtx.prevLightSetting = 3;
            play->envCtx.lightSetting = 10;
            add_calc0(&play->envCtx.lightBlend, 1.0f, 0.05f);
            break;
        case 10:
            play->envCtx.prevLightSetting = 3;
            play->envCtx.lightSetting = 11;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.05f);
            this->unk_1A4 = 0;
            break;
        case 11:
            play->envCtx.prevLightSetting = 12;
            play->envCtx.lightSetting = 11;
            add_calc2(&play->envCtx.lightBlend, (cos_s(this->unk_1A4 * 0x1800) * 0.5f) + 0.5f, 1.0f, 1.0f);
            break;
        case 12:
            play->envCtx.prevLightSetting = 12;
            play->envCtx.lightSetting = 3;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.05f);
            break;
        case 13:
            play->envCtx.lightSetting = 13;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.025f);
            break;
        case 14:
            play->envCtx.lightSetting = 14;
            play->envCtx.lightBlend = 1.0f;
            break;
        case 15:
            play->envCtx.prevLightSetting = 14;
            play->envCtx.lightSetting = 15;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.01f);
            break;
        case 16:
            play->envCtx.prevLightSetting = 16;
            play->envCtx.lightSetting = 15;
            add_calc0(&play->envCtx.lightBlend, 1.0f, 0.05f);
            break;
        case 20:
            play->envCtx.prevLightSetting = 2;
            play->envCtx.lightSetting = 1;
            break;
        default:
            break;
    }

    this->envLightMode = 0;

    if (this->whiteFillAlpha != 0) {
        play->envCtx.screenFillColor[3] = (s8)(u8)this->whiteFillAlpha;
        play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] = play->envCtx.screenFillColor[2] = 255;
        play->envCtx.fillScreen = true;
    } else if (this->screenFlashTimer != 0) {
        play->envCtx.fillScreen = true;
        play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] = play->envCtx.screenFillColor[2] = 255;

        play->envCtx.screenFillColor[3] = ((this->screenFlashTimer % 2) != 0) ? 100 : 0;

        this->screenFlashTimer--;
    } else {
        play->envCtx.fillScreen = play->envCtx.screenFillColor[3] = 0;
    }

    if (this->lensFlareTimer != 0) {
        this->lensFlareTimer--;

        if (this->lensFlareMode == 1) {
            targetLensFlareScale = 40.0f;
        } else if (this->lensFlareMode == 4) {
            targetLensFlareScale = 25.0f;
        } else {
            targetLensFlareScale = 10.0f;
        }

        add_calc2(&this->lensFlareScale, targetLensFlareScale, 0.3f, 10.0f);
    } else {
        add_calc0(&this->lensFlareScale, 1.0f, 5.0f);

        if (this->lensFlareScale == 0.0f) {
            this->lensFlareMode = 0;
        }
    }

    if (this->lensFlareMode != 0) {
        E_lf_flag = true;

        if (this->lensFlareMode == 1) {
            E_lf_position = this->actor.world.pos;
        }

        E_lf_wscale = this->lensFlareScale;
        E_lf_alpha = 10.0f;
        E_lf_rectalpha = 0;
    } else {
        E_lf_flag = false;
    }

    if (this->unk_274 != 0) {
        i = this->unk_274 - 1;

        this->unk_278.x = this->unk_2EC[0].x;
        this->unk_278.y = this->unk_2EC[0].y + 50.0f + 30.0f;
        this->unk_278.z = this->unk_2EC[0].z;

        xOffset = (sinf(i * (2 * M_PI / 5)) * 600.0f);
        zOffset = (cosf(i * (2 * M_PI / 5)) * 600.0f);

        // 5 or 6 light balls that go into the charge. not the same as the ones that he throws
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_GANON, this->unk_1FC.x + xOffset,
                           this->unk_1FC.y, this->unk_1FC.z + zOffset, 0, (s16)(i * 13107.2f) + 0x6000, 0, 0xFA + i);
        this->unk_274 = 0;
    }
}

s32 Boss_Ganon_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossGanon* this = (BossGanon*)thisx;

    switch (limbIndex) {
        case GANONDORF_LIMB_RIGHT_HAND:
            if (this->useOpenHand) {
                *dList = gGanondorfRightHandOpenDL;
            }
            break;

        case GANONDORF_LIMB_LEFT_THIGH:
            rot->y += this->legRot.x + this->legRot.z;
            rot->z += this->legRot.y;
            break;

        case GANONDORF_LIMB_LEFT_SHIN:
            if (this->legRot.y > 0.0f) {
                rot->z += this->legRot.y;
            }
            break;

        case GANONDORF_LIMB_LEFT_FOOT:
            rot->y += this->legRot.x + this->legRot.z;
            rot->z += this->legRot.y;
            break;

        case GANONDORF_LIMB_RIGHT_THIGH:
            rot->y += this->legRot.x - this->legRot.z;
            rot->z += this->legRot.y;
            break;

        case GANONDORF_LIMB_RIGHT_SHIN:
            if (this->legRot.y > 0.0f) {
                rot->z += this->legRot.y;
            }
            break;

        case GANONDORF_LIMB_RIGHT_FOOT:
            rot->y += this->legRot.x - this->legRot.z;
            rot->z += this->legRot.y;
            break;

        default:
            break;
    }

    return 0;
}

static void Boss_Ganon_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static s8 shape_set_id[] = {
        -1, -1, 1, -1, 3, 4, 5, -1, 6, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, 2, 12, 13, 14, 9, 10, 11, -1, -1, -1, -1,
    };
    static Vec3f D_808E4DA0 = { -500.0f, 200.0f, -300.0f };
    static Vec3f D_808E4DAC = { -500.0f, 200.0f, 300.0f };
    static Vec3f center_p = { 0.0f, 0.0f, 0.0f };
    static Vec3f hand_p = { 0.0f, 0.0f, 0.0f };
    static Vec3f r_hand_p = { 0.0f, 0.0f, 0.0f };
    static Vec3f hand_beam_p = { 1300.0f, 0.0f, 0.0f };
    static Vec3f r_hand_t_p = { 600.0f, 420.0f, 100.0f };
    s8 bodyPart;
    BossGanon* this = (BossGanon*)thisx;

    bodyPart = shape_set_id[limbIndex];
    if (bodyPart >= 0) {
        Matrix_Position(&center_p, &this->unk_2EC[bodyPart]);
    }

    if (limbIndex == GANONDORF_LIMB_TORSO) {
        Matrix_Position(&center_p, &this->unk_1FC);
    } else if (limbIndex == GANONDORF_LIMB_PELVIS) {
        Matrix_Position(&center_p, &this->actor.focus.pos);
    } else if (limbIndex == GANONDORF_LIMB_JEWEL) {
        OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon.c", 7191);

        Matrix_Position(&center_p, &this->unk_208);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_ganon.c", 7196);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGanondorfEyesDL));

        CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon.c", 7198);
    } else if (limbIndex == GANONDORF_LIMB_LEFT_HAND) {
        Matrix_Position(&hand_p, &this->unk_238);
    } else if (limbIndex == GANONDORF_LIMB_RIGHT_HAND) {
        Matrix_Position(&r_hand_p, &this->unk_22C);

        if (this->unk_25C == 0) {
            Matrix_Position(&hand_beam_p, &this->unk_260);
        }

        this->unk_25C = 0;

        if (this->triforceType == GDF_TRIFORCE_DORF) {
            Matrix_Position(&r_hand_t_p, &this->triforcePos);
        }
    } else if (limbIndex == GANONDORF_LIMB_LEFT_UPPER_ARM) {
        Vec3f sp28 = D_808E4DA0;

        if (this->unk_198 == 1) {
            sp28.x += -300.0f;
            sp28.y += -300.0f;
            sp28.z += 700.0f;
        } else if (this->unk_198 == 2) {
            sp28.x += -300.0f;
            sp28.z += 700.0f;
        }

        Matrix_Position(&sp28, &this->unk_220);
    } else if (limbIndex == GANONDORF_LIMB_RIGHT_UPPER_ARM) {
        Vec3f sp1C = D_808E4DAC;

        if (this->unk_198 == 1) {
            sp1C.x += -300.0f;
            sp1C.y += -300.0f;
            sp1C.z += -700.0f;
        } else if (this->unk_198 == 2) {
            sp1C.x += -300.0f;
            sp1C.y += 100.0f;
            sp1C.z += -700.0f;
        }

        Matrix_Position(&sp1C, &this->unk_214);
    }
}

static void init_stage_rnd(s32 seedInit0, s32 seedInit1, s32 seedInit2) {
    rnd20 = seedInit0;
    rnd21 = seedInit1;
    rnd22 = seedInit2;
}

static f32 stage_rnd(void) {
    // Wichmann-Hill algorithm
    f32 randFloat;

    rnd20 = (rnd20 * 171) % 30269;
    rnd21 = (rnd21 * 172) % 30307;
    rnd22 = (rnd22 * 170) % 30323;

    randFloat = (rnd20 / 30269.0f) + (rnd21 / 30307.0f) + (rnd22 / 30323.0f);

    while (randFloat >= 1.0f) {
        randFloat -= 1.0f;
    }

    return fabsf(randFloat);
}

static void body_flash_disp(BossGanon* this, PlayState* play) {
    GameState* gameState = &play->state;
    GraphicsContext* gfxCtx = gameState->gfxCtx;
    s16 i;

    OPEN_DISPS(gfxCtx, "../z_boss_ganon.c", 7350);

    if ((this->unk_2E8 != 0) || (this->unk_2E6 != 0)) {
        _texture_z_light_fog_prim_xlu(gameState->gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
        gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_DISP_MODE);

        if (this->unk_2E8 != 0) {
            Player* player = GET_PLAYER(play);

            for (i = 0; i < PLAYER_BODYPART_MAX; i++) {
                Matrix_translate(player->bodyPartsPos[i].x, player->bodyPartsPos[i].y, player->bodyPartsPos[i].z,
                                 MTXMODE_NEW);
                Matrix_rotate_scale_exchange(&play->billboardMtxF);
                Matrix_scale(this->unk_49C[i], this->unk_49C[i], this->unk_49C[i], MTXMODE_APPLY);
                Matrix_rotateZ(rnd_fx(M_PI), MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 7384);
                gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_modelT);
            }
        } else {
            for (i = 1; i < 15; i++) {
                Matrix_translate(this->unk_2EC[i].x, this->unk_2EC[i].y, this->unk_2EC[i].z, MTXMODE_NEW);
                Matrix_rotate_scale_exchange(&play->billboardMtxF);
                Matrix_scale(this->unk_49C[i], this->unk_49C[i], this->unk_49C[i], MTXMODE_APPLY);

                if (!this->shockGlow) {
                    Matrix_rotateZ(rnd_fx(M_PI), MTXMODE_APPLY);
                }

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 7401);

                if (this->shockGlow) {
                    gSPSegment(POLY_XLU_DISP++, 0x08,
                               two_tex_scroll(gameState->gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0,
                                                (this->unk_1A2 + i) * -15, 32, 64));
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 255, 170, 255);
                    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 128);
                    gSPDisplayList(POLY_XLU_DISP++, ganon_damage_modelT);
                } else {
                    gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_modelT);
                }
            }
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_boss_ganon.c", 7465);
}

void hand_beam_disp(BossGanon* this, PlayState* play) {
    GameState* gameState = &play->state;
    GraphicsContext* gfxCtx = gameState->gfxCtx;
    s32 alpha;

    OPEN_DISPS(gfxCtx, "../z_boss_ganon.c", 7476);

    if (this->handLightBallScale > 0.0f) {
        _texture_z_light_fog_prim_xlu(gameState->gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);

        if ((this->unk_1A2 % 2) != 0) {
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
        } else {
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 255, 0, 0);
        }

        gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_DISP_MODE);

        Matrix_translate(this->unk_260.x, this->unk_260.y, this->unk_260.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(this->handLightBallScale, this->handLightBallScale, this->handLightBallScale, MTXMODE_APPLY);
        Matrix_rotateZ(this->unk_258, 1);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 7510);
        gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_modelT);

        alpha = ((this->unk_1A2 % 2) != 0) ? 100 : 80;
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 155, alpha);
        Matrix_translate(this->unk_260.x, 0.0f, this->unk_260.z, MTXMODE_NEW);
        Matrix_scale(this->handLightBallScale * 0.75f, 1.0f, this->handLightBallScale * 0.75f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gameState->gfxCtx, "../z_boss_ganon.c", 7531);
        gSPDisplayList(POLY_XLU_DISP++, gn1_yukaball_modelT);

        CLOSE_DISPS(gfxCtx, "../z_boss_ganon.c", 7534);
    }
}

void big_magic_disp(BossGanon* this, PlayState* play) {
    GameState* gameState = &play->state;
    f32 yRot;
    GraphicsContext* gfxCtx = gameState->gfxCtx;
    s16 i;

    OPEN_DISPS(gfxCtx, "../z_boss_ganon.c", 7548);

    if (this->unk_284 > 0.0f) {
        _texture_z_light_fog_prim_xlu(gameState->gfxCtx);

        // light flecks
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, (s8)this->unk_290);
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 255, 0, 128);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(gameState->gfxCtx, G_TX_RENDERTILE, this->unk_1A2 * -2, 0, 0x40, 0x40, 1, 0,
                                    this->unk_1A2 * 0xA, 0x40, 0x40));
        Matrix_translate(this->unk_278.x, this->unk_278.y, this->unk_278.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(this->unk_28C, this->unk_28C, this->unk_28C, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 7588);
        gSPDisplayList(POLY_XLU_DISP++, efc_ganon_magic_gm0_modelT);

        // background circle texture
        Matrix_translate(this->unk_278.x, this->unk_278.y, this->unk_278.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(this->unk_284, this->unk_284, this->unk_284, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 7601);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 0, 100, (s8)this->unk_288);
        gSPSegment(POLY_XLU_DISP++, 0x09,
                   two_tex_scroll(gameState->gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x20, 1, 0, this->unk_1A2 * -4,
                                    0x20, 0x20));
        gSPDisplayList(POLY_XLU_DISP++, efc_ganon_magic_gm1_modelT);

        // yellow background dot
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 150, 170, 0, (s8)this->unk_288);
        gSPSegment(POLY_XLU_DISP++, 0x0A,
                   two_tex_scroll(gameState->gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x20, 1, this->unk_1A2 * 2,
                                    this->unk_1A2 * -0x14, 0x40, 0x40));
        gSPDisplayList(POLY_XLU_DISP++, efc_ganon_magic_gm2_modelT);

        // light ball material
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 100, 0);
        gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_DISP_MODE);

        // light ball geometry
        Matrix_translate(this->unk_278.x, this->unk_278.y, this->unk_278.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(this->unk_2D0, this->unk_2D0, this->unk_2D0, MTXMODE_APPLY);
        Matrix_rotateZ((this->unk_1A2 * 10.0f) / 1000.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 7673);
        gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_modelT);

        init_stage_rnd(this->unk_1AA + 1, 0x71AC, 0x263A);
        Matrix_translate(this->unk_278.x, this->unk_278.y, this->unk_278.z, MTXMODE_NEW);
        Matrix_rotateY((this->unk_1A2 * 10.0f) / 1000.0f, MTXMODE_APPLY);
        gDPSetEnvColor(POLY_XLU_DISP++, 200, 255, 0, 0);

        yRot = BINANG_TO_RAD_ALT(this->actor.yawTowardsPlayer);

        for (i = 0; i < this->unk_1AC; i++) {
            f32 xzRot = (stage_rnd() - 0.5f) * M_PI * 1.5f;

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s8)this->unk_294[i]);
            Matrix_push();
            Matrix_rotateY(xzRot + yRot, MTXMODE_APPLY);
            Matrix_rotateX((stage_rnd() - 0.5f) * M_PI, MTXMODE_APPLY);
            Matrix_rotateZ(xzRot, MTXMODE_APPLY);
            Matrix_translate(0.0f, 0.0f, 50.0f, MTXMODE_APPLY);
            Matrix_scale(4.0f, 4.0f, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 7713);
            gSPDisplayList(POLY_XLU_DISP++, efc_g_m_sub2_modelT);

            Matrix_pull();
        }

        CLOSE_DISPS(gfxCtx, "../z_boss_ganon.c", 7721);
    }
}

static void tri_force_disp(BossGanon* this, PlayState* play) {
    GameState* gameState = &play->state;

    if (this->fwork[GDF_TRIFORCE_PRIM_A] > 0.0f) {
        OPEN_DISPS(gameState->gfxCtx, "../z_boss_ganon.c", 7732);

        Matrix_push();

        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, (u8)this->fwork[GDF_TRIFORCE_PRIM_B],
                        (s8)this->fwork[GDF_TRIFORCE_PRIM_A]);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, (u8)this->fwork[GDF_TRIFORCE_ENV_G], 0, 128);

        if (this->triforceType == GDF_TRIFORCE_PLAYER) {
            Player* player = GET_PLAYER(play);

            this->triforcePos = player->bodyPartsPos[PLAYER_BODYPART_L_HAND];

            this->triforcePos.x += -0.6f;
            this->triforcePos.y += 3.0f;
            this->triforcePos.z += -2.0f;
        } else if (this->triforceType == GDF_TRIFORCE_ZELDA) {
            this->triforcePos = zelda->unk_31C;

            this->triforcePos.y += 1.8f;
            this->triforcePos.z += 4.0f;
        }

        Matrix_translate(this->triforcePos.x, this->triforcePos.y, this->triforcePos.z, MTXMODE_NEW);

        if (this->triforceType == GDF_TRIFORCE_PLAYER) {
            Matrix_rotateX(-1.4f, MTXMODE_APPLY);
            Matrix_rotateZ(4.0f, MTXMODE_APPLY);
        } else if (this->triforceType == GDF_TRIFORCE_ZELDA) {
            Matrix_rotateY(1.5f, 1);
            Matrix_rotateX(1.1f, 1);
            Matrix_rotateZ(-0.99999994f, MTXMODE_APPLY);
        } else {
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
        }

        Matrix_scale(this->fwork[GDF_TRIFORCE_SCALE], this->fwork[GDF_TRIFORCE_SCALE], 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gameState->gfxCtx, "../z_boss_ganon.c", 7779);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(demo_ec_tfc_modelT));

        Matrix_pull();

        CLOSE_DISPS(gameState->gfxCtx, "../z_boss_ganon.c", 7782);
    }
}

void hadou_disp(BossGanon* this, PlayState* play) {
    GameState* gameState = &play->state;

    if (this->fwork[GDF_VORTEX_ALPHA] > 0.0f) {
        OPEN_DISPS(gameState->gfxCtx, "../z_boss_ganon.c", 7792);

        Matrix_push();
        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(gameState->gfxCtx, G_TX_RENDERTILE, this->unk_1A2 * -8, 0, 0x20, 0x40, 1,
                                    this->unk_1A2 * -4, this->unk_1A2 * -8, 0x20, 0x20));
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 0, 200, (s8)this->fwork[GDF_VORTEX_ALPHA]);
        gDPSetEnvColor(POLY_XLU_DISP++, 130, 0, 0, 128);

        if (this->csState != 21) {
            Matrix_translate(0.0f, 105.0f, -400.0f, MTXMODE_NEW);
            Matrix_rotateX(M_PI / 2, MTXMODE_APPLY);
        } else {
            Matrix_translate(-50.0f, 50.0f, -150.0f, MTXMODE_NEW);
            Matrix_rotateY(M_PI / 10, MTXMODE_APPLY);
            Matrix_rotateX(M_PI / 2, MTXMODE_APPLY);
        }

        Matrix_scale(this->fwork[GDF_VORTEX_SCALE], this->fwork[GDF_VORTEX_SCALE], this->fwork[GDF_VORTEX_SCALE],
                     MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gameState->gfxCtx, "../z_boss_ganon.c", 7841);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(efc_ganon_hadou_modelT));

        Matrix_pull();

        CLOSE_DISPS(gameState->gfxCtx, "../z_boss_ganon.c", 7844);
    }
}

static void shadow_tex_set(BossGanon* this, u8* tex, f32 arg2) {
    static s16 en_dataf[] = { 1, 2, 3, 3, 2, 1 };
    static s16 en_data0[] = { 2, 3, 4, 4, 4, 3, 2 };
    static s16 en_data1[] = { 2, 3, 4, 4, 4, 4, 3, 2 };
    static s16 en_data2[] = { 2, 4, 5, 5, 6, 6, 6, 6, 5, 5, 4, 2 };
    static s16 hokan_pt[] = { 1, -1, 1, 1, 3, 4, 1, 6, 7, 2, 9, 10, 2, 12, 13 };
    static u8 size_data[] = { 3, 2, 2, 1, 3, 3, 1, 3, 3, 1, 0, 3, 1, 0, 3 };
    s16 baseX;
    s16 index;
    s16 i;
    s16 baseY;
    s16 x;
    s16 addY;
    f32 lerpX;
    s16 y;
    f32 lerpY;
    f32 lerpZ;
    Vec3f sp68;
    Vec3f sp5C;

    for (i = 0; i < 15; i++) {
        if (arg2 == 0.0f || (y = hokan_pt[i]) >= 0) {
            if (arg2 > 0.0f) {
                lerpX = this->unk_2EC[i].x + (this->unk_2EC[y].x - this->unk_2EC[i].x) * arg2;
                lerpY = this->unk_2EC[i].y + (this->unk_2EC[y].y - this->unk_2EC[i].y) * arg2;
                lerpZ = this->unk_2EC[i].z + (this->unk_2EC[y].z - this->unk_2EC[i].z) * arg2;

                sp68.x = lerpX - this->actor.world.pos.x;
                sp68.y = lerpY - this->actor.world.pos.y + 76 + 30 + 30;
                sp68.z = lerpZ - this->actor.world.pos.z;
            } else {

                sp68.x = this->unk_2EC[i].x - this->actor.world.pos.x;
                sp68.y = this->unk_2EC[i].y - this->actor.world.pos.y + 76 + 30 + 30;
                sp68.z = this->unk_2EC[i].z - this->actor.world.pos.z;
            }
            Matrix_Position(&sp68, &sp5C);

            sp5C.x *= 0.4f;
            sp5C.y *= 0.4f;

            baseX = (s16)(sp5C.x + 32.0f);
            baseY = (s16)sp5C.y * 64;

            if (size_data[i] == 2) {
                for (y = 0, addY = -0x180; y < 12; y++, addY += 0x40) {
                    for (x = -en_data2[y]; x < en_data2[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < 0x1000)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else if (size_data[i] == 1) {
                for (y = 0, addY = -0x100; y < 8; y++, addY += 0x40) {
                    for (x = -en_data1[y]; x < en_data1[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < 0x1000)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else if (size_data[i] == 0) {
                for (y = 0, addY = -0xC0; y < 7; y++, addY += 0x40) {
                    for (x = -en_data0[y]; x < en_data0[y] - 1; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < 0x1000)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else {
                for (y = 0, addY = -0x80; y < 6; y++, addY += 0x40) {
                    for (x = -en_dataf[y]; x < en_dataf[y] - 1; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < 0x1000)) {
                            tex[index] = 255;
                        }
                    }
                }
            }
        }
    }
}

static void shadow_cont(u8* tex, BossGanon* this, PlayState* play) {
    s16 addY;
    s16 baseX;
    s16 baseY;
    s16 i;
    s16 j;
    s16 y;
    s16 x;
    s16 index;
    Vec3f sp7C;
    Vec3f sp70;
    s32* ptr = (s32*)tex;

    for (i = 0; i < 64 * 64 / 4; i++, ptr++) {
        *ptr = 0;
    }

    Matrix_rotateX(1.0f, MTXMODE_NEW);

    for (i = 0; i <= 5; i++) {
        shadow_tex_set(this, tex, i / 5.0f);
    }

    for (i = 0; i < 12; i++) {
        for (j = 0; j < 12; j++) {
            sp7C.x = mant->strands[i].joints[j].x - this->actor.world.pos.x;
            sp7C.y = mant->strands[i].joints[j].y - this->actor.world.pos.y + 76.0f + 100.0f + 30.0f;
            sp7C.z = mant->strands[i].joints[j].z - this->actor.world.pos.z;

            Matrix_Position(&sp7C, &sp70);

            sp70.x *= 0.28f;
            sp70.y *= 0.28f;

            baseX = (s32)(sp70.x + 32.0f);
            baseY = (s16)sp70.y * 0x40;

            if (!mant->strands[i].torn[j]) {
                for (y = -1, addY = -0x40; y <= 1; y++, addY += 0x40) {
                    for (x = -3; x <= 3; x++) {
                        index = baseX + x + baseY + addY;
                        if (0 <= index && index < 0x1000) {
                            tex[index] = 255;
                        }
                    }
                }
            } else {
                for (y = -1, addY = -0x40; y <= 1; y++, addY += 0x40) {
                    for (x = -1; x <= 1; x++) {
                        index = baseX + x + baseY + addY;
                        if (0 <= index && index < 0x1000) {
                            tex[index] = 255;
                        }
                    }
                }
            }
        }
    }
}

static void shadow_disp(void* tex, BossGanon* this, PlayState* play) {
    GameState* gameState = &play->state;
    f32 zOffset;
    GraphicsContext* gfxCtx = gameState->gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_boss_ganon.c", 8372);

    _texture_z_light_fog_prim(gameState->gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, 50);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);

    if (this->csState < 100) {
        zOffset = (((((this->actor.world.pos.y - 10) + 70.0f) * -5.0f) / 10.0f) + 10.0f);
        Matrix_translate(this->actor.world.pos.x, 0.0f, this->actor.world.pos.z + zOffset, MTXMODE_NEW);
    } else {
        Matrix_translate(this->actor.world.pos.x, 4102.0f, this->actor.world.pos.z - 20.0f, MTXMODE_NEW);
    }

    Matrix_scale(0.95000005f, 1.0f, 0.95000005f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gameState->gfxCtx, "../z_boss_ganon.c", 8396);
    gSPDisplayList(POLY_OPA_DISP++, shadow_test_MODE);
    gDPLoadTextureBlock(POLY_OPA_DISP++, tex, G_IM_FMT_I, G_IM_SIZ_8b, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPDisplayList(POLY_OPA_DISP++, shadow_test_model);

    CLOSE_DISPS(gfxCtx, "../z_boss_ganon.c", 8426);
}

void Boss_Ganon_Actor_draw(Actor* thisx, PlayState* play) {
    s32 i;
    BossGanon* this = (BossGanon*)thisx;
    void* shadowTex;

    shadowTex = GRAPH_ALLOC(play->state.gfxCtx, 64 * 64);

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon.c", 9138);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    if ((this->unk_1A6 & 2) != 0) {
        POLY_OPA_DISP = gfx_set_fog_nosync(POLY_OPA_DISP, 255, 50, 0, 0, 900, 1099);
    }

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gGanondorfNormalEyeTex));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          Boss_Ganon_draw_sub, Boss_Ganon_draw_sub2, &this->actor);

    this->unk_2EC[0].x = this->unk_2EC[1].x;
    this->unk_2EC[0].y = this->unk_2EC[1].y + 30.0f;
    this->unk_2EC[0].z = this->unk_2EC[1].z;

    POLY_OPA_DISP = game_play_set_fog(play, POLY_OPA_DISP);

    Gn_Eff_disp(play);

    mant->actor.world.pos = this->actor.world.pos;

    mant->rightForearmPos = this->unk_214;
    mant->leftForearmPos = this->unk_220;

    mant->rightShoulderPos = this->unk_22C;
    mant->leftShoulderPos = this->unk_238;

    body_flash_disp(this, play);
    hand_beam_disp(this, play);
    big_magic_disp(this, play);
    tri_force_disp(this, play);
    hadou_disp(this, play);

    shadow_cont(shadowTex, this, play);
    shadow_disp(shadowTex, this, play);

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon.c", 9393);
}

s32 otyuka_check(BossGanon* this, PlayState* play, Vec3f* checkPos) {
    Actor* prop = play->actorCtx.actorLists[ACTORCAT_PROP].head;

    while (prop != NULL) {
        if (((BossGanon*)prop == this) || (prop->id != ACTOR_BG_GANON_OTYUKA)) {
            prop = prop->next;
        } else {
            BgGanonOtyuka* platform = (BgGanonOtyuka*)prop;
            f32 xDiff = platform->dyna.actor.world.pos.x - checkPos->x;
            f32 yDiff = platform->dyna.actor.world.pos.y - checkPos->y;
            f32 zDiff = platform->dyna.actor.world.pos.z - checkPos->z;

            if ((fabsf(xDiff) < 60.0f) && (yDiff < 20.0f) && (yDiff > -20.0f) && (fabsf(zDiff) < 60.0f)) {
                s32 pad;

                platform->isFalling = true;
                platform->visibleSides = OTYUKA_SIDE_ALL;

                return 1;
            } else {
                prop = prop->next;
            }
        }
    }

    return 0;
}

void gn_wepon_move(Actor* thisx, PlayState* play2) {
    u8 hitWithBottle;
    s16 i;
    s16 spBA = 0;
    Vec3f spAC;
    Vec3f spA0;
    Vec3f sp94;
    BossGanon* this = (BossGanon*)thisx;
    PlayState* play = play2;
    f32 xDistFromLink;
    f32 yDistFromLink;
    f32 zDistFromLink;
    f32 minReflectDist;
    f32 xDistFromGanondorf;
    f32 yDistFromGanondorf;
    f32 zDistFromGanondorf;
    Player* player = GET_PLAYER(play);
    s32 pad;
    BossGanon* ganondorf = (BossGanon*)this->actor.parent;
    s32 pad1;

    this->unk_1A2++;
    ganondorf->envLightMode = 1;

    if (this->unk_1A8 != 0) {
        if (this->unk_1A8 == 2) {
            add_calc0(&this->fwork[GDF_FWORK_1], 1.0f, 10.0f);
            add_calc2(&this->actor.scale.x, 30.0f, 0.5f, 100.0f);
        } else {
            this->actor.shape.rot.y += 0x1000;
            ganondorf->lensFlareTimer = 1;
            E_lf_position = this->actor.world.pos;
            add_calc0(&this->fwork[GDF_FWORK_1], 1.0f, 30.0f);
            add_calc2(&this->actor.scale.x, 20.0f, 0.5f, 100.0f);
            this->fwork[GDF_FWORK_0] += ((M_PI / 2) + rnd_f(M_PI / 4));
        }

        Actor_set_scale(&this->actor, this->actor.scale.x);

        if (this->fwork[GDF_FWORK_1] == 0.0f) {
            Actor_delete(&this->actor);
        }
    } else {
        Actor_SE_set(&this->actor, NA_SE_EN_GANON_FIRE - SFX_FLAG);

        if ((this->unk_1A2 % 2) != 0) {
            Actor_set_scale(&this->actor, 6.0f);
        } else {
            Actor_set_scale(&this->actor, 5.25f);
        }

        this->actor.shape.rot.z += (s16)(fqrand() * 20000.0f) + 0x4000;

        for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
            if (this->timers[i] != 0) {
                this->timers[i]--;
            }
        }

        xDistFromGanondorf = ganondorf->unk_1FC.x - this->actor.world.pos.x;
        yDistFromGanondorf = ganondorf->unk_1FC.y - this->actor.world.pos.y;
        zDistFromGanondorf = ganondorf->unk_1FC.z - this->actor.world.pos.z;

        xDistFromLink = player->actor.world.pos.x - this->actor.world.pos.x;
        yDistFromLink = (player->actor.world.pos.y + 40.0f) - this->actor.world.pos.y;
        zDistFromLink = player->actor.world.pos.z - this->actor.world.pos.z;

        Actor_position_speed_set_XY(&this->actor);
        Actor_position_move(&this->actor);

        switch (this->unk_1C2) {
            case 0:
                if ((player->stateFlags1 & PLAYER_STATE1_SWINGING_BOTTLE) &&
                    (ABS((s16)(player->actor.shape.rot.y - (s16)(ganondorf->actor.yawTowardsPlayer + 0x8000))) <
                     0x2000) &&
                    (sqrtf(SQ(xDistFromLink) + SQ(yDistFromLink) + SQ(zDistFromLink)) <= 25.0f)) {
                    hitWithBottle = true;
                } else {
                    hitWithBottle = false;
                }

                if ((this->collider.base.acFlags & AC_HIT) || hitWithBottle) {
                    ColliderElement* acHitElem = this->collider.elem.acHitElem;

                    this->collider.base.acFlags &= ~AC_HIT;

                    if ((hitWithBottle == false) && (acHitElem->atDmgInfo.dmgFlags & DMG_SHIELD)) {
                        spBA = 2;
                        Nai_FxFlagEntry(NA_SE_IT_SHIELD_REFLECT_MG, &player->actor.projectedPos, 4,
                                             &_dummy_one, &_dummy_one,
                                             &_dummy_zero_s8);
                        z_vibctl2_vib_setQ(this->actor.xyzDistToPlayerSq, 255, 20, 150);
                    } else {
                        spBA = 1;
                        this->actor.world.rot.y = atans_table(zDistFromGanondorf, xDistFromGanondorf);
                        this->actor.world.rot.x =
                            atans_table(sqrtf(SQ(xDistFromGanondorf) + SQ(zDistFromGanondorf)), yDistFromGanondorf);
                        this->unk_1A4++;
                        this->timers[1] = 2;
                        Nai_FxFlagEntry(NA_SE_IT_SWORD_REFLECT_MG, &player->actor.projectedPos, 4,
                                             &_dummy_one, &_dummy_one,
                                             &_dummy_zero_s8);
                        z_vibctl2_vib_setQ(this->actor.xyzDistToPlayerSq, 180, 20, 100);

                        if (hitWithBottle == false) {
                            // if ganondorf is 250 units away from link, at least 3 volleys are required
                            if ((ganondorf->actor.xyzDistToPlayerSq > SQ(250.0f)) && (this->unk_1A4 < 3)) {
                                this->unk_1C2 = 1;
                            } else if (fqrand() < 0.7f) {
                                this->unk_1C2 = 1;
                            } else {
                                this->unk_1C2 = 3;
                            }

                            if (player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) {
                                this->actor.speed = 20.0f;
                            }
                            break;
                        } else {
                            if (fqrand() < 0.9f) {
                                this->unk_1C2 = 1;
                            } else {
                                this->unk_1C2 = 3;
                                break;
                            }
                        }
                    }
                } else {
                    if (sqrtf(SQ(xDistFromLink) + SQ(yDistFromLink) + SQ(zDistFromLink)) <= 25.0f) {
                        spBA = 5;
                        Actor_player_power_damage_AT_set(play, &this->actor, 3.0f, this->actor.world.rot.y, 0.0f, 0x30);
                        Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EN_GANON_HIT_THUNDER);
                        ganondorf->timers[2] = 20;

                        for (i = 0; i < ARRAY_COUNT(ganondorf->unk_4E4); i++) {
                            ganondorf->unk_4E4[i] = shape_flash_time_data[i];
                        }

                        ganondorf->unk_2E6 = 0;
                        ganondorf->unk_2E8 = 60;
                        ganondorf->unk_508 = 4.0f;
                    }
                }
                break;

            case 1:
                if ((ganondorf->actionFunc == mode_atack2) && (ganondorf->unk_1C2 == 1)) {
                    minReflectDist = (this->actor.speed >= 19.0f) ? 250.0f : 170.0f;

                    if (sqrtf(SQ(xDistFromGanondorf) + SQ(yDistFromGanondorf) + SQ(zDistFromGanondorf)) <
                        minReflectDist) {
                        ganondorf->startVolley = true;
                        this->timers[0] = 8;
                        this->unk_1C2 = 2;
                    }
                }
                break;

            case 2:
                if (this->timers[0] == 1) {
                    spBA = 1;
                    this->actor.world.rot.y = atans_table(zDistFromLink, xDistFromLink);
                    this->actor.world.rot.x = atans_table(sqrtf(SQ(xDistFromLink) + SQ(zDistFromLink)), yDistFromLink);
                    this->timers[1] = 2;
                    Actor_SE_set(&this->actor, NA_SE_IT_SWORD_REFLECT_MG);
                    Actor_SE_set(&this->actor, NA_SE_EN_GANON_AT_RETURN);
                    this->unk_1C2 = 0;
                    break;
                }
                FALLTHROUGH;
            case 4:
                if (sqrtf(SQ(xDistFromGanondorf) + SQ(yDistFromGanondorf) + SQ(zDistFromGanondorf)) < 30.0f) {
                    spBA = 3;
                    Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EN_GANON_DAMAGE1);
                    Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EN_GANON_HIT_THUNDER);
                }
                break;

            case 3:
                if (sqrtf(SQ(xDistFromGanondorf) + SQ(yDistFromGanondorf) + SQ(zDistFromGanondorf)) < 100.0f) {
                    ganondorf->startVolley = true;
                    this->unk_1C2 = 4;
                }
                break;
        }

        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

        if (this->timers[1] == 0) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }

        for (i = 0; i < 2; i++) {
            spA0.x = spA0.z = 0.0f;
            spA0.y = 0.2f;

            spAC.x = spAC.y = spAC.z = 0.0f;

            sp94.x = rnd_fx(30.0f) + this->actor.world.pos.x;
            sp94.y = rnd_fx(30.0f) + this->actor.world.pos.y;
            sp94.z = rnd_fx(30.0f) + this->actor.world.pos.z;

            gn_hinoko_ct(play, &sp94, &spAC, &spA0, rnd_f(500.0f) + 700.0f, 0x1E);
        }

        if (this->actor.world.pos.y < 10.0f) {
            Actor_BGcheck2(play, &this->actor, 0.0f, 20.0f, 20.0f, UPDBGCHECKINFO_FLAG_2);
        }

        if ((fabsf(this->actor.world.pos.x) > 465.0f) || (this->actor.world.pos.y > 500.0f) ||
            (fabsf(this->actor.world.pos.z) > 465.0f)) {
            spBA = 4;
        }

        if ((spBA != 0) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            f32 sp58;
            f32 sp54;
            f32 phi_f20;
            s16 sp4E;

            if (spBA == 1) {
                sp58 = rnd_f(100.0f) + 300.0f;
                sp54 = 10.0f;
                phi_f20 = 25.0f;
                sp4E = 40;
            } else {
                sp58 = rnd_f(200.0f) + 500.0f;
                sp54 = 15.0f;
                phi_f20 = 30.0f;
                sp4E = 70;
                Effect_SE_Info_new(play, &this->actor.world.pos, 80, NA_SE_EN_GANON_HIT_THUNDER);
            }

            for (i = 0; i < sp4E; i++) {
                if (spBA != 0) {
                    spAC.x = rnd_fx(phi_f20);
                    spAC.y = rnd_fx(phi_f20);
                    spAC.z = rnd_fx(phi_f20);
                } else {
                    spAC.x = rnd_fx(phi_f20);
                    spAC.y = rnd_f(25.0f);
                    spAC.z = rnd_fx(phi_f20);
                }

                gn_hinoko_2_ct(play, &this->actor.world.pos, &spAC, &zero, sp58, sp54, 0x1E);
            }

            if (spBA != 1) {
                this->unk_1A8 = 1;

                if (spBA == 0) {
                    otyuka_check(this, play, &this->actor.world.pos);
                }

                if (spBA == 3) {
                    mode_ballhit_init(ganondorf, play);
                } else if (ganondorf->actionFunc == mode_atack2) {
                    mode_flying_init(ganondorf, play);

                    if (spBA == 5) {
                        ganondorf->timers[0] = 125;
                    }
                }
            }
        }
    }
}

void gn_wepon_draw(Actor* thisx, PlayState* play) {
    BossGanon* this = (BossGanon*)thisx;
    s16 i;
    f32 alpha;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon.c", 9849);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    alpha = ((this->unk_1A2 % 2) != 0) ? this->fwork[GDF_FWORK_1] * 0.4f : this->fwork[GDF_FWORK_1] * 0.35f;

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 155, (s8)alpha);
    Matrix_push();
    Matrix_translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_scale(this->actor.scale.x * 0.75f, 1.0f, this->actor.scale.z * 0.75f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_ganon.c", 9875);
    gSPDisplayList(POLY_XLU_DISP++, gn1_yukaball_modelT);

    Matrix_pull();
    gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_DISP_MODE);

    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s8)this->fwork[GDF_FWORK_1]);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);

    if (this->unk_1A8 == 1) {
        for (i = 0; i < 8; i++) {
            Matrix_push();
            Matrix_rotateY(i * (M_PI / 8), MTXMODE_APPLY);
            Matrix_rotateZ(this->fwork[GDF_FWORK_0], MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_ganon.c", 9899);

            gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_modelT);
            Matrix_pull();
        }
    } else if (this->unk_1A8 == 0) {
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_rotateZ((this->actor.shape.rot.z / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_ganon.c", 9907);
        gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_modelT);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon.c", 9911);
}

void gn_hikari_move(Actor* thisx, PlayState* play2) {
    s16 i;
    BossGanon* this = (BossGanon*)thisx;
    PlayState* play = play2;
    BossGanon* dorf = (BossGanon*)this->actor.parent;

    this->unk_1A2++;
    dorf->envLightMode = 1;
    Actor_set_scale(&this->actor, 6.0f);
    this->actor.shape.rot.z += ((s16)(fqrand() * 20000.0f) + 0x4000);

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }

    Actor_position_speed_set_XY(&this->actor);
    Actor_position_move(&this->actor);

    this->unk_1A6++;

    if (this->unk_1A6 >= 15) {
        this->unk_1A6 = 0;
    }

    this->unk_2EC[this->unk_1A6] = this->actor.world.pos;

    if (this->unk_1C2 == 0) {
        if (1) {}
        if (this->timers[0] == 0) {
            this->unk_1C2 = 1;
        }
    } else if (this->unk_1C2 == 1) {
        f32 xDiff;
        f32 yDiff;
        f32 zDiff;
        f32 xzDist;
        s16 xRotTarget;
        s16 yRotTarget;
        Vec3f vel;
        Vec3f accel;

        xDiff = dorf->unk_1FC.x - this->actor.world.pos.x;
        yDiff = dorf->unk_1FC.y - this->actor.world.pos.y;
        zDiff = dorf->unk_1FC.z - this->actor.world.pos.z;

        yRotTarget = RAD_TO_BINANG(fatan2(xDiff, zDiff));
        xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
        xRotTarget = RAD_TO_BINANG(fatan2(yDiff, xzDist));

        adds(&this->actor.world.rot.x, xRotTarget, 1, 0x1000);
        adds(&this->actor.world.rot.y, yRotTarget, 1, 0x1000);

        if (sqrtf(SQ(xDiff) + SQ(zDiff) + SQ(yDiff)) < 40.0f) {
            this->unk_1C2 = 2;
            this->timers[0] = 30;
            this->actor.speed = 0.0f;

            if (this->actor.params == 0xC8) {
                Na_StartSystemSe_F(NA_SE_EN_GANON_DAMAGE2);
                Na_StartSystemSe_F(NA_SE_EN_GANON_DD_THUNDER);

                for (i = 0; i < 150; i++) {

                    vel.x = rnd_fx(25.0f);
                    vel.y = rnd_fx(25.0f);
                    vel.z = rnd_fx(25.0f);

                    accel.x = vel.x * -0.03f;
                    accel.y = vel.y * -0.03f;
                    accel.z = vel.z * -0.03f;

                    gn_hinoko_2_ct(play, &dorf->unk_1FC, &vel, &accel, rnd_f(500.0f) + 1000.0f,
                                               15.0f, 0x14);
                }

                for (i = 1; i < 15; i++) {
                    dorf->unk_4E4[i] = 1000;
                }

                dorf->unk_2E6 = 1000;
                dorf->unk_2E8 = 0;
                dorf->screenFlashTimer = 4;
                dorf->lensFlareTimer = 10;
                dorf->lensFlareMode = 1;
                dorf->unk_508 = 10.0f;

                Actor_info_make_child_actor(&play->actorCtx, &dorf->actor, play, ACTOR_BOSS_GANON, dorf->unk_1FC.x,
                                   dorf->unk_1FC.y, dorf->unk_1FC.z, 0, 0, 0, 0x12C);
            }

            this->actor.world.pos.y = 5000.0f;
        }
    } else if (this->timers[0] == 0) {
        Actor_delete(&this->actor);
    }
}

void gn_hikari_draw(Actor* thisx, PlayState* play2) {
    BossGanon* this = (BossGanon*)thisx;
    PlayState* play = play2;
    s16 i;
    s32 temp;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon.c", 10081);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
    gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_DISP_MODE);

    for (i = 9; i >= 0; i--) {
        temp = (s16)(((this->unk_1A6 - i) + 0xF) % 15);
        Matrix_translate(this->unk_2EC[temp].x, this->unk_2EC[temp].y, this->unk_2EC[temp].z, MTXMODE_NEW);
        Matrix_scale(this->actor.scale.x * (1.0f - (i * 0.07000001f)), this->actor.scale.y * (1.0f - (i * 0.07000001f)),
                     this->actor.scale.z * (1.0f - (i * 0.07000001f)), MTXMODE_APPLY);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_rotateZ(((2.0f * (i * M_PI)) / 10.0f) + BINANG_TO_RAD_ALT(this->actor.shape.rot.z), MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_ganon.c", 10109);
        gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_modelT);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon.c", 10113);
}

void gn_big_magic_move(Actor* thisx, PlayState* play) {
    u8 numEffects = 0;
    s16 xRot;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 xzDist;
    f32 new_var;
    f32 sp84;
    s16 i;
    s16 sp80;
    BossGanon* this = (BossGanon*)thisx;
    BossGanon* dorf = (BossGanon*)this->actor.parent;
    s32 pad;
    Player* player = GET_PLAYER(play);

    this->unk_1A2++;
    Actor_set_scale(&this->actor, 0.01f);

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }

    Actor_position_speed_set_XY(&this->actor);
    Actor_position_move(&this->actor);

    this->unk_1A6++;

    if (this->unk_1A6 >= 15) {
        this->unk_1A6 = 0;
    }

    this->unk_2EC[this->unk_1A6] = this->actor.world.pos;
    this->unk_3C4[this->unk_1A6].x = BINANG_TO_RAD_ALT(this->actor.world.rot.x);
    this->unk_3C4[this->unk_1A6].y = BINANG_TO_RAD_ALT(this->actor.world.rot.y);

    switch (this->unk_1C2) {
        if (1) {}
        case 0:
            this->actor.speed = 40.0f;
            add_calc2(&this->fwork[1], 255.0f, 1.0f, 40.0f);
            xDiff = dorf->unk_278.x - this->actor.world.pos.x;
            yDiff = dorf->unk_278.y - this->actor.world.pos.y;
            zDiff = dorf->unk_278.z - this->actor.world.pos.z;
            sp80 = RAD_TO_BINANG(fatan2(xDiff, zDiff));
            xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));

            xRot = RAD_TO_BINANG(fatan2(yDiff, xzDist));
            sp84 = (xzDist * 700.0f) / 10.0f;
            if (sp84 > 6144.0f) {
                sp84 = 6144.0f;
            }

            xRot += (cos_s(this->unk_1A2 * 0x2200) * sp84);
            this->actor.world.rot.x = xRot;
            adds(&this->actor.shape.rot.y, sp80, 1, this->csCamMaxStepScale);
            add_calc2(&this->csCamMaxStepScale, 4096.0f, 1.0f, 256.0f);
            this->actor.world.rot.y = (sin_s(this->unk_1A2 * 0x1A00) * sp84) + this->actor.shape.rot.y;

            if (sqrtf(SQ(xDiff) + SQ(zDiff) + SQ(yDiff)) < 45.0f) {
                this->unk_1C2 = 1;
                this->actor.speed = 0.0f;
            }
            break;

        case 1:
            add_calc0(&this->fwork[1], 1.0f, 40.0f);

            if (this->fwork[1] == 0.0f) {
                Actor_delete(&this->actor);
            }
            break;

        case 10:
            this->unk_1C2 = 0xB;
            this->timers[0] = 14;

            this->collider.dim.radius = 15;
            this->collider.dim.height = 20;
            this->collider.dim.yShift = -10;

            this->actor.speed = 20.0f;
            this->fwork[1] = 255.0f;
            this->unk_1F0 = player->actor.world.pos;
            new_var = this->unk_1F0.x - this->actor.world.pos.x;
            this->actor.shape.rot.y = RAD_TO_BINANG(fatan2(new_var, this->unk_1F0.z - this->actor.world.pos.z)) +
                                      (this->actor.params << 0xD) - 0x20C000;
            FALLTHROUGH;
        case 11:
            if (this->timers[0] != 0) {
                this->unk_1F0 = player->actor.world.pos;
                xDiff = this->unk_1F0.x - this->actor.world.pos.x;
                yDiff = (this->unk_1F0.y + 30.0f) - this->actor.world.pos.y;
                zDiff = this->unk_1F0.z - this->actor.world.pos.z;

                sp80 = RAD_TO_BINANG(fatan2(xDiff, zDiff));
                this->actor.shape.rot.x = RAD_TO_BINANG(fatan2(yDiff, sqrtf(SQ(xDiff) + SQ(zDiff))));
                adds(&this->actor.shape.rot.y, sp80, 1, this->csCamMaxStepScale);
                add_calc2(&this->csCamMaxStepScale, 4096.0f, 1.0f, 256.0f);
            }

            sp84 = (sqrtf(this->actor.xyzDistToPlayerSq) * 200.0f) / 10.0f;
            if (sp84 > 13824.0f) {
                sp84 = 13824.0f;
            }

            this->actor.world.rot.x = (cos_s(this->unk_1A2 * 0x3400) * sp84 * 0.1f) + this->actor.shape.rot.x;
            this->actor.world.rot.y = (sin_s(this->unk_1A2 * 0x1A00) * sp84) + this->actor.shape.rot.y;

            if ((player->meleeWeaponState != 0) && (player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) &&
                (this->actor.xzDistToPlayer < 80.0f)) {
                this->unk_1C2 = 0xC;
                this->actor.speed = -30.0f;
                Actor_position_speed_set_XY(&this->actor);
                Actor_position_move(&this->actor);
                this->unk_1F0 = dorf->unk_1FC;
                numEffects = 10;
                break;
            }

            if (this->collider.base.acFlags & AC_HIT) {
                ColliderElement* acHitElem = this->collider.elem.acHitElem;

                this->collider.base.acFlags &= ~AC_HIT;

                if (!(acHitElem->atDmgInfo.dmgFlags & DMG_SHIELD) || mirror_shield_check(play)) {
                    z_vibctl2_vib_setQ(this->actor.xyzDistToPlayerSq, 180, 20, 100);
                    this->unk_1C2 = 0xC;
                    this->actor.speed = -30.0f;

                    Actor_position_speed_set_XY(&this->actor);
                    Actor_position_move(&this->actor);

                    this->unk_1F0.x = rnd_fx(700.0f) + dorf->unk_1FC.x;
                    this->unk_1F0.y = rnd_fx(200.0f) + dorf->unk_1FC.y;
                    this->unk_1F0.z = rnd_fx(700.0f) + dorf->unk_1FC.z;

                    this->unk_1F0.x += (this->unk_1F0.x - this->actor.world.pos.x) * 100.0f;
                    this->unk_1F0.y += (this->unk_1F0.y - this->actor.world.pos.y) * 100.0f;
                    this->unk_1F0.z += (this->unk_1F0.z - this->actor.world.pos.z) * 100.0f;

                    numEffects = 10;
                    break;
                }
            }

            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

            if (this->timers[1] == 0) {
                CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
            }

            xDiff = player->actor.world.pos.x - this->actor.world.pos.x;
            yDiff = (player->actor.world.pos.y + 30.0f) - this->actor.world.pos.y;
            zDiff = player->actor.world.pos.z - this->actor.world.pos.z;

            if (sqrtf(SQ(xDiff) + SQ(zDiff) + SQ(yDiff)) < 30.0f) {
                this->unk_1C2 = 1;
                this->actor.speed = 0.0f;

                if (dorf->timers[2] == 0) {
                    Actor_player_power_damage_AT_set(play, &this->actor, 3.0f, this->actor.world.rot.y, 0.0f, 0x50);
                    Effect_SE_Info_new(play, &this->actor.world.pos, 40, NA_SE_EN_GANON_HIT_THUNDER);
                    dorf->timers[2] = 20;

                    for (i = 0; i < ARRAY_COUNT(this->unk_4E4); i++) {
                        dorf->unk_4E4[i] = shape_flash_time_data[i];
                    }

                    dorf->unk_2E6 = 0;
                    dorf->unk_2E8 = 60;
                    dorf->unk_508 = 4.0f;
                    numEffects = 40;
                }
            }
            break;

        case 12:
            this->actor.speed = 20.0f;

            xDiff = this->unk_1F0.x - this->actor.world.pos.x;
            yDiff = this->unk_1F0.y - this->actor.world.pos.y;
            zDiff = this->unk_1F0.z - this->actor.world.pos.z;

            sp80 = RAD_TO_BINANG(fatan2(xDiff, zDiff));
            xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
            xRot = RAD_TO_BINANG(fatan2(yDiff, xzDist));
            sp84 = (xzDist * 700.0f) / 10.0f;

            if (sp84 > 6144.0f) {
                sp84 = 6144.0f;
            }

            sp80 += sin_s(this->unk_1A2 * 0x2200) * sp84;

            xRot += cos_s(this->unk_1A2 * 0x1800) * sp84;

            this->actor.world.rot.x = xRot;
            this->actor.world.rot.y = sp80;

            xDiff = dorf->unk_1FC.x - this->actor.world.pos.x;
            yDiff = dorf->unk_1FC.y - this->actor.world.pos.y;
            zDiff = dorf->unk_1FC.z - this->actor.world.pos.z;

            if (sqrtf(SQ(xDiff) + SQ(zDiff) + SQ(yDiff)) < 45.0f) {
                mode_ballhit_init(dorf, play);
                this->timers[0] = 150;
                numEffects = 40;
                this->unk_1C2 = 1;
                this->actor.speed = 0.0f;
            }
            break;
    }

    if (this->unk_1C2 >= 0xB) {
        xzDist = (this->unk_1C2 == 0xC) ? -65.0f : 0.0f;

        if ((fabsf(this->actor.world.pos.x) > (465.0f + xzDist)) ||
            (fabsf(this->actor.world.pos.z) > (465.0f + xzDist)) || (this->actor.world.pos.y < 0.0f) ||
            (this->actor.world.pos.y > 450.0f)) {
            this->unk_1C2 = 1;
            this->actor.speed = 0.0f;
            numEffects = 10;
            otyuka_check(this, play, &this->actor.world.pos);
            Actor_info_make_child_actor(&play->actorCtx, &dorf->actor, play, ACTOR_BOSS_GANON, this->actor.world.pos.x,
                               this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0x190);
        }
    }

    if (numEffects) {
        Vec3f sp60;

        Effect_SE_Info_new(play, &this->actor.world.pos, 80, NA_SE_EN_FANTOM_THUNDER);

        for (i = 0; i < numEffects; i++) {
            sp60.x = rnd_fx(30.0f);
            sp60.y = rnd_fx(30.0f);
            sp60.z = rnd_fx(30.0);

            gn_hinoko_2_ct(play, &this->actor.world.pos, &sp60, &zero, rnd_f(200.0f) + 500.0f,
                                       15.0f, 0x1E);
        }
    }
}

static Gfx* big_magic_gfx[] = {
    gn1_lb01_model, gn1_lb02_model, gn1_lb03_model, gn1_lb04_model,
    gn1_lb05_model,  gn1_lb06_model,  gn1_lb07_model,  gn1_lb08_model,
    gn1_lb09_model,  gn1_lb10_model,  gn1_lb11_model,  gn1_lb12_model,
};

void gn_big_magic_draw(Actor* thisx, PlayState* play) {
    BossGanon* this = (BossGanon*)thisx;
    Mtx* mtx;
    s16 i;
    s32 temp;

    mtx = GRAPH_ALLOC(play->state.gfxCtx, 12 * sizeof(Mtx));

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon.c", 10489);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 255, 255, 255, (s8)this->fwork[GDF_FWORK_1]);
    gDPSetEnvColor(POLY_XLU_DISP++, 150, 255, 0, 128);
    gSPSegment(POLY_XLU_DISP++, 0x0D, mtx);

    for (i = 0; i < 12; i++) {
        temp = (s16)(((this->unk_1A6 - i) + 0xF) % 15);
        Matrix_translate(this->unk_2EC[temp].x, this->unk_2EC[temp].y, this->unk_2EC[temp].z, MTXMODE_NEW);
        Matrix_rotateY(this->unk_3C4[temp].y, MTXMODE_APPLY);
        Matrix_rotateX(-this->unk_3C4[temp].x, MTXMODE_APPLY);
        Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_rotateY(M_PI / 2, MTXMODE_APPLY);
        MATRIX_TO_MTX(mtx, "../z_boss_ganon.c", 10520);
        gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, big_magic_gfx[i]);
        mtx++;
    };

    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_rotate_scale_exchange(&play->billboardMtxF);
    Matrix_scale(10.0f, 10.0f, 10.0f, MTXMODE_APPLY);
    Matrix_rotateZ(rnd_fx(M_PI), MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_ganon.c", 10534);
    gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_DISP_MODE);

    gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_modelT);

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon.c", 10541);
}

void Gn_Eff_move(PlayState* play) {
    Player* player = GET_PLAYER(play);
    GanondorfEffect* eff = play->specialEffects;
    s16 i;
    s32 pad;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 yRot;
    f32 xRot;
    Vec3f spA0;
    s16 bodyPart;
    f32 distToPlayer;
    s32 pad2;
    s32 pad3;

    spA0.x = 0.0f;
    spA0.y = 0.0f;

    for (i = 0; i < BOSSGANON_EFFECT_COUNT; i++, eff++) {
        if (eff->type != GDF_EFF_NONE) {
            eff->pos.x += eff->velocity.x;
            eff->pos.y += eff->velocity.y;
            eff->pos.z += eff->velocity.z;

            eff->timer++;

            eff->velocity.x += eff->accel.x;
            eff->velocity.y += eff->accel.y;
            eff->velocity.z += eff->accel.z;

            if (eff->type == GDF_EFF_WINDOW_SHARD) {
                eff->unk_44 += 0.3f;
                eff->unk_48 += 0.5f;

                if (eff->pos.y < 0.0f) {
                    eff->type = GDF_EFF_NONE;
                }
            } else if (eff->type == GDF_EFF_SPARKLE) {
                eff->unk_3C += rnd_f(M_PI / 2) + M_PI / 2;
                eff->unk_2E -= eff->unk_30;

                if (eff->unk_2E <= 0) {
                    eff->unk_2E = 0;
                    eff->type = GDF_EFF_NONE;
                }

                eff->alpha = eff->unk_2E;

                if (eff->alpha > 255) {
                    eff->alpha = 255;
                }
            } else if (eff->type == GDF_EFF_BLACK_DOT) {
                xDiff = ganon->unk_278.x - eff->pos.x;
                yDiff = ganon->unk_278.y - eff->pos.y;
                zDiff = ganon->unk_278.z - eff->pos.z;

                yRot = fatan2(xDiff, zDiff);

                xRot = -fatan2(yDiff, sqrtf(SQ(xDiff) + SQ(zDiff)));
                spA0.z = eff->unk_38;
                Matrix_rotateY(yRot, MTXMODE_NEW);
                Matrix_rotateX(xRot, MTXMODE_APPLY);
                Matrix_Position(&spA0, &eff->velocity);
                add_calc2(&eff->unk_38, 10.0f, 1.0f, 0.5f);

                eff->alpha += 10;

                if (eff->alpha > 255) {
                    eff->alpha = 255;
                }

                if ((sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff)) < 20.0f) || (eff->timer > 70)) {
                    eff->type = GDF_EFF_NONE;
                }
            } else if (eff->type == GDF_EFF_LIGHT_RAY) {
                eff->unk_3C += rnd_f(M_PI / 2) + M_PI / 2;
                eff->unk_2E -= eff->unk_30;

                if (eff->unk_2E <= 0) {
                    eff->unk_2E = 0;
                    eff->type = GDF_EFF_NONE;
                }

                eff->alpha = eff->unk_2E;

                if (eff->alpha > 255) {
                    eff->alpha = 255;
                }

                add_calc2(&eff->unk_38, eff->unk_40, 1.0f, (eff->unk_40 / 15.0f) * 4.0f);
            } else if (eff->type == GDF_EFF_SHOCK) {
                s32 pad4;

                if (eff->unk_2E == GDF_SHOCK_DORF_YELLOW) {
                    bodyPart = (s16)rnd_f(13.9f) + 1;

                    eff->pos.x = ganon->unk_2EC[bodyPart].x + rnd_fx(20.0f);
                    eff->pos.y = ganon->unk_2EC[bodyPart].y + rnd_fx(20.0f);
                    eff->pos.z = ganon->unk_2EC[bodyPart].z + rnd_fx(20.0f);
                } else {
                    bodyPart = (s16)rnd_f(PLAYER_BODYPART_MAX - 0.1f);

                    eff->pos.x = player->bodyPartsPos[bodyPart].x + rnd_fx(10.0f);
                    eff->pos.y = player->bodyPartsPos[bodyPart].y + rnd_fx(15.0f);
                    eff->pos.z = player->bodyPartsPos[bodyPart].z + rnd_fx(10.0f);
                }

                eff->unk_3C += (rnd_f(M_PI / 2) + M_PI / 2);

                if (eff->timer > 20) {
                    eff->type = GDF_EFF_NONE;
                }
            } else if (eff->type == GDF_EFF_LIGHTNING) {
                if (eff->unk_3C == 0.0f) {
                    eff->unk_44 = BINANG_TO_RAD_ALT(getCameraAngleY(Gama_play_get_camera(play, CAM_ID_MAIN)));
                } else {
                    eff->unk_44 = M_PI / 2;
                }

                if (eff->timer > 12) {
                    eff->type = GDF_EFF_NONE;
                }
            } else if (eff->type == GDF_EFF_IMPACT_DUST_DARK) {
                eff->unk_30++; // unused

                if (eff->unk_2E == 0) {
                    eff->alpha += 26;

                    if (eff->alpha > 255) {
                        eff->alpha = 255;
                        eff->unk_2E = 1;
                    }
                } else if (eff->unk_2E == 1) {
                    eff->unk_2E = 2;
                } else if (eff->unk_2E == 2) {
                    eff->alpha -= 26;

                    if (eff->alpha < 0) {
                        eff->alpha = 0;
                        eff->type = GDF_EFF_NONE;
                    }
                }

                add_calc2(&eff->scale, eff->unk_38, 1.0f, 0.01f);
                add_calc2(&eff->unk_40, 4.0f, 1.0f, 0.15f);
            } else if (eff->type == GDF_EFF_IMPACT_DUST_LIGHT) {
                if (i == 0) {
                    Na_StartSystemSe_F(NA_SE_EN_GANON_WAVE_GND - SFX_FLAG);
                }

                eff->unk_30++; // unused

                if (eff->unk_2E == 0) {
                    eff->alpha += 100;

                    if (eff->alpha > 255) {
                        eff->alpha = 255;
                        eff->unk_2E = 1;
                    }
                } else if (eff->unk_2E == 1) {
                    if (eff->timer >= 20) {
                        eff->unk_2E = 2;
                    }
                } else if (eff->unk_2E == 2) {
                    eff->alpha -= 30;

                    if (eff->alpha < 0) {
                        eff->alpha = 0;
                        eff->type = GDF_EFF_NONE;
                    }
                }

                add_calc2(&eff->scale, eff->unk_38, 1.0f, 0.1f);
                add_calc2(&eff->unk_40, 1.0f, 1.0f, 0.15f);
            } else if (eff->type == GDF_EFF_SHOCKWAVE) {
                eff->unk_30++; // unused
                eff->alpha -= 30;

                if (eff->alpha < 0) {
                    eff->alpha = 0;
                    eff->type = GDF_EFF_NONE;
                }

                add_calc2(&eff->scale, eff->unk_38, 1.0f, 0.13f);

                if ((eff->timer < 150) && (fabsf(player->actor.world.pos.y) < 5.0f)) {
                    distToPlayer =
                        sqrtf(SQ(eff->pos.x - player->actor.world.pos.x) + SQ(eff->pos.z - player->actor.world.pos.z));

                    if (((eff->scale * 150.0f) < distToPlayer) && (distToPlayer < (eff->scale * 300.0f))) {
                        eff->timer = 150;
                        Actor_player_power_damage_AT_set(play, &ganon->actor, 7.0f,
                                                      ganon->actor.yawTowardsPlayer, 0.0f, 0x20);
                    }
                }
            }
        }
    }
}

static void* thunder2_txt[] = {
    g_fg_thunder01_txt,  g_fg_thunder01_txt, g_fg_thunder02_txt,  g_fg_thunder03_txt,
    g_fg_thunder04_txt,  g_fg_thunder05_txt, g_fg_thunder06_txt,  g_fg_thunder07_txt,
    g_fg_thunder08_txt,  g_fg_thunder09_txt, g_fg_thunder10_txt, g_fg_thunder11_txt,
    g_fg_thunder12_txt,
};

static u8 beam2_prim[13][3] = {
    { 0, 0, 0 },       { 255, 255, 255 }, { 231, 250, 231 }, { 208, 245, 208 }, { 185, 240, 185 },
    { 162, 235, 162 }, { 139, 230, 139 }, { 115, 225, 115 }, { 92, 220, 92 },   { 69, 215, 69 },
    { 46, 210, 46 },   { 23, 205, 23 },   { 0, 200, 0 },
};

static u8 beam2_env[13][3] = {
    { 0, 0, 0 },      { 255, 255, 0 },   { 240, 231, 23 },  { 226, 208, 46 }, { 212, 185, 69 },
    { 198, 162, 92 }, { 184, 139, 115 }, { 170, 115, 139 }, { 156, 92, 162 }, { 142, 69, 185 },
    { 128, 46, 208 }, { 114, 23, 231 },  { 100, 0, 255 },
};

static s32 sUnknown[3] = { 0 };

void Gn_Eff_disp(PlayState* play) {
    u8 materialFlag = 0;
    s16 i;
    s32 pad;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    GanondorfEffect* eff = play->specialEffects;
    GanondorfEffect* effFirst = eff;

    OPEN_DISPS(gfxCtx, "../z_boss_ganon.c", 10865);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < 200; i++, eff++) {
        if (eff->type == GDF_EFF_WINDOW_SHARD) {
            gDPPipeSync(POLY_OPA_DISP++);
            if (materialFlag == 0) {
                gSPDisplayList(POLY_OPA_DISP++, g_hahen_MODE);
                materialFlag++;
            }
            if ((eff->timer & 7) != 0) {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, eff->color.r, eff->color.g, eff->color.b, 255);
            } else {
                gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
            }
            Matrix_translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_scale(eff->scale, eff->scale, eff->scale, MTXMODE_APPLY);
            Matrix_rotateY(eff->unk_48, MTXMODE_APPLY);
            Matrix_rotateX(eff->unk_44, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx, "../z_boss_ganon.c", 10898);
            gSPDisplayList(POLY_OPA_DISP++, g_hahen_model);
        }
    }

    eff = effFirst;
    materialFlag = 0;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_SPARKLE) {
            gDPPipeSync(POLY_XLU_DISP++);
            if (materialFlag == 0) {
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
                gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_DISP_MODE);
                materialFlag++;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, eff->alpha);
            Matrix_translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
            Matrix_rotateZ(eff->unk_3C, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 10932);
            gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_modelT);
        }
    }

    eff = effFirst;
    materialFlag = 0;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_LIGHT_RAY) {
            gDPPipeSync(POLY_XLU_DISP++);
            if (materialFlag == 0) {
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
                gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_DISP_MODE);
                materialFlag++;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, eff->alpha);
            Matrix_translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_rotateY(eff->unk_48, MTXMODE_APPLY);
            Matrix_rotateX(eff->unk_44, MTXMODE_APPLY);
            Matrix_rotateZ(eff->unk_3C, MTXMODE_APPLY);
            Matrix_scale(eff->scale, eff->scale, eff->unk_38 * eff->scale, MTXMODE_APPLY);
            Matrix_rotateX(M_PI / 2, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 10971);
            gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder1_modelT);
        }
    }

    eff = effFirst;
    materialFlag = 0;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_SHOCK) {
            if (materialFlag == 0) {
                gDPPipeSync(POLY_XLU_DISP++);
                if (eff->unk_2E == GDF_SHOCK_PLAYER_PURPLE) {
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 0, 200, 255);
                    gDPSetEnvColor(POLY_XLU_DISP++, 130, 0, 0, 0);
                } else { // GDF_SHOCK_DORF_YELLOW or GDF_SHOCK_PLAYER_YELLOW
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
                    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 0);
                }
                materialFlag++;
            }
            Matrix_translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
            Matrix_rotateX(eff->unk_3C * 1.3f, MTXMODE_APPLY);
            Matrix_rotateZ(eff->unk_3C, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 11023);
            gSPDisplayList(POLY_XLU_DISP++, gn1_inazumaMINI2_modelT);
        }
    }

    eff = effFirst;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_LIGHTNING) {
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, beam2_prim[eff->timer][0],
                            beam2_prim[eff->timer][1], beam2_prim[eff->timer][2], 255);
            gDPSetEnvColor(POLY_XLU_DISP++, beam2_env[eff->timer][0], beam2_env[eff->timer][1],
                           beam2_env[eff->timer][2], 0);
            Matrix_translate(ganon->unk_260.x, ganon->unk_260.y, ganon->unk_260.z, MTXMODE_NEW);
            Matrix_rotateY(eff->unk_48, MTXMODE_APPLY);
            Matrix_rotateZ(eff->unk_3C, MTXMODE_APPLY);
            Matrix_scale(eff->scale, eff->scale, eff->scale, MTXMODE_APPLY);
            Matrix_rotateY(eff->unk_44, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 11074);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(thunder2_txt[eff->timer]));
            gSPDisplayList(POLY_XLU_DISP++, efc_fg_thunder2_modelT);
        }
    }

    eff = effFirst;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_IMPACT_DUST_DARK) {
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, eff->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 70, 0, 128);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, eff->timer * 4, 0, 32, 64, 1,
                                        eff->timer * 2, eff->timer * -20, 32, 32));
            Matrix_translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_scale(eff->scale, eff->unk_40 * eff->scale, eff->scale, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 11121);
            gSPDisplayList(POLY_XLU_DISP++, efc_ganon_smoke_modelT);
        }
    }

    eff = effFirst;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_IMPACT_DUST_LIGHT) {
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, eff->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 200, 100, 0, 128);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, eff->timer * 4, 0, 32, 64, 1,
                                        eff->timer * 2, eff->timer * -20, 32, 32));
            Matrix_translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_scale(eff->scale, eff->unk_40 * eff->scale, eff->scale, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 11165);
            gSPDisplayList(POLY_XLU_DISP++, efc_ganon_flash_modelT);
        }
    }

    eff = effFirst;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_SHOCKWAVE) {
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, eff->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 150, 255, 0, 128);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (eff->timer * 100), 0, 64, 32, 1,
                                        (eff->timer * 100), 0, 64, 32));
            Matrix_translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_scale((eff->scale * 200.0f) / 1500.0f, (eff->unk_40 * 200.0f) / 1500.0f,
                         (eff->scale * 200.0f) / 1500.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 11209);
            gSPDisplayList(POLY_XLU_DISP++, efc_ganon_shock_modelT);
        }
    }

    eff = effFirst;

    for (i = 0; i < 150; i++, eff++) {
        if (eff->type == GDF_EFF_BLACK_DOT) {
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 150, 170, 0, eff->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);
            gSPSegment(POLY_XLU_DISP++, 0x0A,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 32, 1, eff->timer * 2,
                                        eff->timer * -20, 64, 64));
            Matrix_translate(eff->pos.x, eff->pos.y, eff->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(eff->scale, eff->scale, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_ganon.c", 11250);
            gSPDisplayList(POLY_XLU_DISP++, efc_ganon_magic_gm2_modelT);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_boss_ganon.c", 11255);
}

#include "assets/overlays/ovl_Boss_Ganon/z_boss_ganon_shape.c"
