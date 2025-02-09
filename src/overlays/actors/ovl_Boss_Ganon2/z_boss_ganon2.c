#include "z_boss_ganon2.h"
#include "overlays/actors/ovl_Boss_Ganon/z_boss_ganon.h"
#include "overlays/actors/ovl_Demo_Gj/z_demo_gj.h"
#include "overlays/actors/ovl_En_Zl3/z_en_zl3.h"

#include "libc64/qrand.h"
#include "attributes.h"
#include "gfx.h"
#include "gfx_setupdl.h"
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

#include "assets/objects/object_ganon/object_ganon.h"
#include "assets/objects/object_ganon2/object_ganon2.h"
#include "assets/objects/object_ganon_anime3/object_ganon_anime3.h"
#include "assets/objects/object_geff/object_geff.h"
#include "assets/overlays/ovl_Boss_Ganon2/z_boss_ganon2_shape.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define BOSS_GANON2_EFFECT_COUNT 100

typedef struct BossGanon2Effect {
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 unk_01;
    /* 0x04 */ Vec3f position;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ char unk_28[0x6];
    /* 0x2E */ s16 unk_2E;
    /* 0x30 */ char unk_30[0x4];
    /* 0x34 */ f32 scale;
    /* 0x38 */ Vec3f unk_38;
} BossGanon2Effect; // size = 0x44

void Boss_Ganon2_Actor_ct(Actor* thisx, PlayState* play);
void Boss_Ganon2_Actor_dt(Actor* thisx, PlayState* play);
void Boss_Ganon2_Actor_move(Actor* thisx, PlayState* play);
void Boss_Ganon2_Actor_draw(Actor* thisx, PlayState* play);

static void mode_startdemo_init(BossGanon2* this, PlayState* play);
static void mode_startdemo(BossGanon2* this, PlayState* play);
static void mode_wait_init(BossGanon2* this, PlayState* play);
static void mode_wait(BossGanon2* this, PlayState* play);
static void mode_mahi(BossGanon2* this, PlayState* play);
static void mode_yahit(BossGanon2* this, PlayState* play);
static void mode_damage(BossGanon2* this, PlayState* play);
static void mode_walk_init(BossGanon2* this, PlayState* play);
static void mode_walk(BossGanon2* this, PlayState* play);
static void mode_atack_init(BossGanon2* this, PlayState* play);
static void mode_atack(BossGanon2* this, PlayState* play);
void mode_damagedemo(BossGanon2* this, PlayState* play);
static void mode_enddemo(BossGanon2* this, PlayState* play);
void Gn2_Eff_move(BossGanon2* this, PlayState* play);
void Gn2_Eff_disp(PlayState* play);
static void shadow_cont(void* shadowTexture, BossGanon2* this, PlayState* play);
static void shadow_disp(void* shadowTexture, BossGanon2* this, PlayState* play);

ActorProfile Boss_Ganon2_Profile = {
    /**/ ACTOR_BOSS_GANON2,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_GANON2,
    /**/ sizeof(BossGanon2),
    /**/ Boss_Ganon2_Actor_ct,
    /**/ Boss_Ganon2_Actor_dt,
    /**/ Boss_Ganon2_Actor_move,
    /**/ Boss_Ganon2_Actor_draw,
};

static Vec3f zero = { 0.0f, 0.0f, 0.0f };

static Vec3f zero2 = { 0.0f, 0.0f, 500.0f };

static u8 gareki_break = 0;

static ColliderJntSphElementInit Ganon2AcOcInfoJntSphElemData[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 2, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 3, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 4, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 5, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 6, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 7, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 8, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 9, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 10, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 11, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 12, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x10 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 14, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 15, { { 0, 0, 0 }, 30 }, 100 },
    },
};

static ColliderJntSphInit Ganon2AcOcInfoJntSphData = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_FIRST_ONLY | OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(Ganon2AcOcInfoJntSphElemData),
    Ganon2AcOcInfoJntSphElemData,
};

static ColliderJntSphElementInit KenAcOcInfoJntSphElemData[] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x40 },
            { 0xFFDFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 15, { { 0, 0, 0 }, 45 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xFFCFFFFF, 0x00, 0x40 },
            { 0xFFDFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 16, { { 0, 0, 0 }, 45 }, 100 },
    },
};

static ColliderJntSphInit KenAcOcInfoJntSphData = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(KenAcOcInfoJntSphElemData),
    KenAcOcInfoJntSphElemData,
};

static Vec3f inazuma_se_pos;

static EnZl3* zelda;

static Actor* navy;

// unused
static UNK_TYPE D_8090EB34;

static BossGanon2Effect gn2_eff[BOSS_GANON2_EFFECT_COUNT];

static s32 rnd20;
static s32 rnd21;
static s32 rnd22;

// unused
static UNK_TYPE D_809105DC;

static Vec3f blur_base_1[4];

static Vec3f blur_base_2[4];

static s8 blur_disp;

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

static void CollisionCheck_pos_set(s32 idx, ColliderJntSph* collider, Vec3f* arg2) {
    collider->elements[idx].dim.worldSphere.center.x = arg2->x;
    collider->elements[idx].dim.worldSphere.center.y = arg2->y;
    collider->elements[idx].dim.worldSphere.center.z = arg2->z;

    collider->elements[idx].dim.worldSphere.radius =
        collider->elements[idx].dim.modelSphere.radius * collider->elements[idx].dim.scale;
}

static void obj_bank_set(BossGanon2* this, PlayState* play, s32 objectId, u8 setRSPSegment) {
    s32 pad;
    s32 objectSlot = Object_Exchange_bank_check(&play->objectCtx, objectId);

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);

    if (setRSPSegment) {
        OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 790);

        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[objectSlot].segment);
        gSPSegment(POLY_XLU_DISP++, 0x06, play->objectCtx.slots[objectSlot].segment);

        CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 799);
    }
}

void gn2_master_s_ct(PlayState* play, Vec3f* arg1) {
    BossGanon2Effect* effects = play->specialEffects;

    effects[0].type = 1;
    effects[0].position = *arg1;
    effects[0].velocity.x = 25.0f;
    effects[0].velocity.y = 15.0f;
    effects[0].velocity.z = 0.0f;
    effects[0].accel.x = 0.0f;
    effects[0].accel.y = -1.0f;
    effects[0].accel.z = 0.0f;
    effects[0].unk_2E = 0;
    effects[0].unk_01 = 0;
}

void gn2_gareki_ct(PlayState* play, Vec3f* position, Vec3f* velocity, f32 scale) {
    BossGanon2Effect* effect = play->specialEffects;
    s16 i;

    for (i = 0; i < BOSS_GANON2_EFFECT_COUNT; i++, effect++) {
        if (effect->type == 0) {
            effect->type = 2;
            effect->position = *position;
            effect->velocity = *velocity;
            effect->accel.x = 0.0;
            effect->accel.y = -1.0f;
            effect->accel.z = 0.0;
            effect->unk_38.z = rnd_f(2 * M_PI);
            effect->unk_38.y = rnd_f(2 * M_PI);
            effect->unk_38.x = rnd_f(2 * M_PI);
            effect->scale = scale;
            break;
        }
    }
}

void Boss_Ganon2_Actor_ct(Actor* thisx, PlayState* play) {
    BossGanon2* this = (BossGanon2*)thisx;
    s32 pad;
    s16 i;

    play->specialEffects = gn2_eff;

    for (i = 0; i < BOSS_GANON2_EFFECT_COUNT; i++) {
        gn2_eff[i].type = 0;
    }

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.colChkInfo.health = 30;
    ClObjJntSph_ct(play, &this->unk_424);
    ClObjJntSph_set5_nzm(play, &this->unk_424, &this->actor, &Ganon2AcOcInfoJntSphData, this->unk_464);
    ClObjJntSph_ct(play, &this->unk_444);
    ClObjJntSph_set5_nzm(play, &this->unk_444, &this->actor, &KenAcOcInfoJntSphData, this->unk_864);
    obj_bank_set(this, play, OBJECT_GANON, false);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGanondorfSkel, NULL, NULL, NULL, 0);
    mode_startdemo_init(this, play);
    this->actor.naviEnemyId = NAVI_ENEMY_GANON;
    this->actor.gravity = 0.0f;
}

void Boss_Ganon2_Actor_dt(Actor* thisx, PlayState* play) {
    BossGanon2* this = (BossGanon2*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjJntSph_dt_nzf(play, &this->unk_424);
    ClObjJntSph_dt_nzf(play, &this->unk_444);
}

void gn2_chakuchi_dust_se_set(BossGanon2* this, PlayState* play, s16 arg2, s16 arg3) {
    if ((arg2 == 0) || (arg2 == 1)) {
        _dust_ground_set(play, &this->actor, &this->unk_1D0, 25.0f, arg3, 8.0f, 500, 10, true);
    }

    if ((arg2 == 0) || (arg2 == 2)) {
        _dust_ground_set(play, &this->actor, &this->unk_1DC, 25.0f, arg3, 8.0f, 500, 10, true);
    }

    Actor_SE_set(&this->actor, NA_SE_EN_MGANON_WALK);
    ActorQuakeRumbleSet(&this->actor, play, 2, 10);
}

static void mode_startdemo_init(BossGanon2* this, PlayState* play) {
    this->actionFunc = mode_startdemo;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.world.pos.y = -3000.0f;
}

static void mode_startdemo(BossGanon2* this, PlayState* play) {
    s16 pad;
    u8 sp8D;
    Player* player;
    s32 objectSlot;
    s32 zero = 0;
    s32 pad2;

    sp8D = false;
    player = GET_PLAYER(play);
    this->unk_398++;

    switch (this->unk_39C) {
        case 0:
            objectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_GANON_ANIME3);
            if (Object_Exchange_bank_dma_check(&play->objectCtx, objectSlot)) {
                Demo_play_start(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
                this->subCamId = Gama_play_make_camera(play);
                Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
                Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
                this->unk_39C = 1;
                zelda = (EnZl3*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_ZL3, 970.0f, 1086.0f,
                                                    -200.0f, 0, 0, 0, 1);
                zelda->unk_3C8 = 0;
                zelda->actor.world.pos.x = 970.0f;
                zelda->actor.world.pos.y = 1086.0f;
                zelda->actor.world.pos.z = -214.0f;
                zelda->actor.shape.rot.y = -0x7000;
                this->subCamUp.x = 0.0f;
                this->subCamUp.y = 1.0f;
                this->subCamUp.z = 0.0f;
                this->unk_398 = 0;
                this->subCamEye.x = 0.0f;
                this->subCamEye.y = 1400.0f;
                this->subCamEye.z = 1600.0f;
                player->actor.world.pos.x = 970.0f;
                player->actor.world.pos.y = 1086.0f;
                player->actor.world.pos.z = -186.0f;
                player->actor.shape.rot.y = -0x5000;
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfBurstOutAnim, 0.0f);
                play->envCtx.lightBlend = 0.0f;
                // fake, tricks the compiler into allocating more stack
                if (zero) {
                    this->subCamEye.x *= 2.0;
                }
            } else {
                break;
            }
            FALLTHROUGH;
        case 1:
            if (this->unk_398 < 70) {
                play->envCtx.lightBlend = 0.0f;
            }
            this->unk_339 = 3;
            add_calc2(&this->subCamEye.x, 1500.0f, 0.1f, this->unk_410.x * 1500.0f);
            add_calc2(&this->subCamEye.z, -160.0f, 0.1f, this->unk_410.x * 1760.0f);
            add_calc2(&this->unk_410.x, 0.0075f, 1.0f, 0.0001f);
            this->subCamAt.x = -200.0f;
            this->subCamAt.y = 1086.0f;
            this->subCamAt.z = -200.0f;
            if (this->unk_398 == 150) {
                message_set(play, 0x70D3, NULL);
            }
            if (this->unk_398 > 250 && message_check(&play->msgCtx) == TEXT_STATE_NONE) {
                this->unk_39C = 2;
                this->unk_398 = 0;
                this->unk_410.x = 0.0f;
                play->envCtx.lightBlend = 1.0f;
            } else {
                break;
            }
            FALLTHROUGH;
        case 2:
            this->unk_339 = 4;
            player->actor.world.pos.x = 970.0f;
            player->actor.world.pos.y = 1086.0f;
            player->actor.world.pos.z = -166.0f;
            zelda->actor.world.pos.x = 974.0f;
            zelda->actor.world.pos.y = 1086.0f;
            zelda->actor.world.pos.z = -186.0f;
            player->actor.shape.rot.y = -0x5000;
            zelda->actor.shape.rot.y = -0x5000;
            if (this->unk_398 == 60) {
                message_set(play, 0x70D4, NULL);
            }
            if (this->unk_398 == 40) {
                zelda->unk_3C8 = 1;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_78);
            }
            if (this->unk_398 == 85) {
                zelda->unk_3C8 = 2;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_79);
            }
            this->subCamEye.x = 930.0f;
            this->subCamEye.y = 1129.0f;
            this->subCamEye.z = -181.0f;
            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.z = (player->actor.world.pos.z - 15.0f) + 5.0f;
            if (this->unk_398 > 104) {
                add_calc2(&this->subCamAt.y, player->actor.world.pos.y + 47.0f + 7.0f + 15.0f, 0.1f,
                               this->unk_410.x);
                add_calc2(&this->unk_410.x, 2.0f, 1.0f, 0.1f);
            } else {
                this->subCamAt.y = player->actor.world.pos.y + 47.0f + 7.0f;
            }
            if ((this->unk_398 > 170) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->unk_39C = 3;
                this->unk_398 = 0;
                this->unk_410.x = 0.0f;
            }
            break;
        case 3:
            add_calc2(&this->subCamAt.y, player->actor.world.pos.y + 47.0f + 7.0f, 0.1f, 2.0f);
            this->unk_339 = 4;
            if (this->unk_398 == 10) {
                Na_StartObjectSe_F(&zero2, NA_SE_EV_STONE_BOUND);
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0);
            }
            if (this->unk_398 == 20) {
                zelda->unk_3C8 = 3;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_80);
            }
            if (this->unk_398 == 55) {
                this->unk_39C = 4;
                this->unk_398 = 0;
                this->unk_410.x = 0.0f;
                zelda->unk_3C8 = 4;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_80);
            }
            break;
        case 4:
            this->unk_339 = 4;
            add_calc2(&this->subCamEye.x, -360.0f, 0.1f, this->unk_410.x * 1290.0f);
            add_calc2(&this->subCamEye.z, -20.0f, 0.1f, this->unk_410.x * 170.0f);
            add_calc2(&this->unk_410.x, 0.04f, 1.0f, 0.0005f);
            if (this->unk_398 == 100) {
                Camera* camera = Gama_play_get_camera(play, CAM_ID_MAIN);

                camera->eye = this->subCamEye;
                camera->eyeNext = this->subCamEye;
                camera->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                this->unk_39C = 5;
                this->unk_398 = 0;
            }
            break;
        case 5:
            this->unk_339 = 4;
            if (this->actor.xzDistToPlayer < 500.0f) {
                message_close(play);
                this->unk_39C = 10;
                this->unk_398 = 0;
                Demo_play_start(play, &play->csCtx);
                this->subCamId = Gama_play_make_camera(play);
                Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
                Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            } else {
                break;
            }
            FALLTHROUGH;
        case 10:
            player->actor.world.pos.x = 490.0f;
            player->actor.world.pos.y = 1086.0f;
            player->actor.world.pos.z = -166.0f;
            zelda->actor.world.pos.x = 724.0f;
            zelda->actor.world.pos.y = 1086.0f;
            zelda->actor.world.pos.z = -186.0f;
            player->actor.shape.rot.y = -0x4000;
            zelda->actor.shape.rot.y = -0x5000;
            this->subCamEye.x = 410.0f;
            this->subCamEye.y = 1096.0f;
            this->subCamEye.z = -110.0f;
            this->subCamAt.x = player->actor.world.pos.x + 10.0f;
            this->subCamAt.y = (player->actor.world.pos.y + 200.0f) - 160.0f;
            this->subCamAt.z = player->actor.world.pos.z;
            if (this->unk_398 >= 20) {
                Na_StartSystemSe_F(NA_SE_EN_GOMA_LAST - SFX_FLAG);
                add_calc2(&this->unk_324, 255.0f, 1.0f, 10.0f);
                this->unk_339 = 5;
                if (this->unk_398 == 20) {
                    this->unk_33C = 0.0f;
                    play->envCtx.lightBlend = 0.0f;
                }
            } else {
                this->unk_339 = 4;
            }
            if (this->unk_398 == 30) {
                zelda->unk_3C8 = 5;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_81);
            }
            if (this->unk_398 == 50) {
                this->unk_398 = 0;
                this->unk_39C = 11;
            }
            break;
        case 11:
            this->unk_339 = 5;
            Na_StartSystemSe_F(NA_SE_EN_GOMA_LAST - SFX_FLAG);
            player->actor.world.pos.x = 490.0f;
            player->actor.world.pos.y = 1086.0f;
            player->actor.world.pos.z = -166.0f;
            zelda->actor.world.pos.x = 724.0f;
            zelda->actor.world.pos.y = 1086.0f;
            zelda->actor.world.pos.z = -186.0f;
            player->actor.shape.rot.y = -0x4000;
            zelda->actor.shape.rot.y = -0x5000;
            this->subCamEye.x = 450.0f;
            this->subCamEye.y = 1121.0f;
            this->subCamEye.z = -158.0f;
            this->subCamAt.x = (player->actor.world.pos.x - 20.0f) + 2.0f;
            this->subCamAt.y = ((player->actor.world.pos.y + 200.0f) - 151.0f) - 2.0f;
            this->subCamAt.z = player->actor.world.pos.z + 2.0f;
            if (this->unk_398 == 10) {
                Na_StartObjectSe_F(&zero2, NA_SE_EV_STONE_BOUND);
            }
            if (this->unk_398 == 20) {
                Na_StartSystemSe_F(NA_SE_EV_STONE_BOUND);
            }
            if (this->unk_398 == 30) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_82);
            }
            if (this->unk_398 == 50) {
                this->unk_398 = 0;
                this->unk_39C = 12;
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfBurstOutAnim, 0.0f);
                this->unk_194 = Si2_anime_end_frame(&gGanondorfBurstOutAnim);
                this->actor.world.pos.x = this->actor.world.pos.z = -200.0f;
                this->actor.world.pos.y = 1009.0f;
                this->actor.shape.yOffset = 7000.0f;
                this->actor.world.rot.y = 0x5000;
                this->subCamEye.x = -60.0f;
                this->subCamEye.y = 1106.0f;
                this->subCamEye.z = -200.0f;
                this->subCamAt.x = this->subCamAt.z = -200.0f;
                this->subCamAt.y = this->actor.world.pos.y + 70.0f;
                play->envCtx.lightBlend = 0.0f;
                play->envCtx.prevLightSetting = play->envCtx.lightSetting = 0;
                this->unk_339 = 0;
            } else {
                break;
            }
            FALLTHROUGH;
        case 12:
        case 13:
            Skeleton_Info2_anime_play(&this->skelAnime);
            if (this->unk_398 == 30) {
                gareki_break = 1;
                this->unk_314 = 1;
                z_vibctl2_vib_force_set(0.0f, 200, 20, 20);
            }
            if (this->unk_398 == 30) {
                Na_StartSystemSe_F(NA_SE_EV_GRAVE_EXPLOSION);
            }
            if (this->unk_398 >= 30) {
                add_calc2(&this->actor.world.pos.y, 1289.0f, 0.1f, 10.0f);
                this->subCamAt.y = this->actor.world.pos.y + 70.0f;
            }
            if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfFloatingHeavyBreathingAnim, 0.0f);
                this->unk_39C = 14;
                this->unk_398 = 0;
                this->actor.world.pos.x = -200.0f;
                this->actor.world.pos.y -= 30.0f;
                this->actor.world.pos.z = -200.0f;
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_OPENING_GANON);
            } else {
                break;
            }
            FALLTHROUGH;
        case 14:
            Skeleton_Info2_anime_play(&this->skelAnime);
            add_calc2(&this->actor.world.pos.y, 1289.0f, 0.05f, 1.0f);
            player->actor.world.pos.x = 250.0f;
            player->actor.world.pos.y = 1086.0f;
            player->actor.world.pos.z = -266.0f;
            player->actor.shape.rot.y = -0x4000;
            zelda->actor.world.pos.x = 724.0f;
            zelda->actor.world.pos.y = 1086.0f;
            zelda->actor.world.pos.z = -186.0f;
            this->subCamEye.x = this->actor.world.pos.x + -10.0f;
            this->subCamEye.y = this->actor.world.pos.y + 80.0f;
            this->subCamEye.z = this->actor.world.pos.z + 50.0f;
            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = player->actor.world.pos.y;
            this->subCamAt.z = player->actor.world.pos.z - 200.0f;
            if (this->unk_398 == 20) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_30);
            }
            if (this->unk_398 == 60) {
                this->subCamEye.x = (this->actor.world.pos.x + 200.0f) - 154.0f;
                this->subCamEye.y = this->actor.world.pos.y + 60.0f;
                this->subCamEye.z = this->actor.world.pos.z - 15.0f;
                this->unk_39C = 15;
                this->unk_398 = 0;
                this->subCamAt.y = this->actor.world.pos.y + 77.0f + 100.0f;
                this->unk_314 = 2;
                this->subCamAt.z = this->actor.world.pos.z + 5.0f;
                this->subCamAt.x = this->actor.world.pos.x;
            }
            if ((play->gameplayFrames % 32) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_BREATH);
            }
            break;
        case 15:
            if (((play->gameplayFrames % 32) == 0) && (this->unk_398 < 100)) {
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_BREATH);
            }
            Skeleton_Info2_anime_play(&this->skelAnime);
            add_calc2(&this->subCamAt.y, this->actor.world.pos.y + 77.0f, 0.05f, 5.0f);
            if (this->unk_398 >= 50) {
                if (this->unk_398 == 50) {
                    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfShowTriforceStartAnim, 0.0f);
                    this->unk_194 = Si2_anime_end_frame(&gGanondorfShowTriforceStartAnim);
                    this->unk_314 = 3;
                }
                if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
                    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanondorfShowTriforceLoopAnim, 0.0f);
                    this->unk_194 = 1000.0f;
                }
            }
            if (this->unk_398 > 70) {
                add_calc2(&this->unk_1B4, 255.0f, 1.0f, 10.0f);
            }
            if (this->unk_398 == 140) {
                this->unk_39C = 16;
                this->unk_398 = 0;
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfTransformStartAnim, 0.0f);
                this->unk_194 = Si2_anime_end_frame(&gGanondorfTransformStartAnim);
                this->unk_339 = 55;
                play->envCtx.lightBlend = 1.0f;
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_CASBREAK);
            } else {
                break;
            }
            FALLTHROUGH;
        case 16:
            if (this->unk_398 < 25) {
                this->unk_339 = 55;
            } else {
                this->unk_339 = 6;
                if (this->unk_194 > 100.0f) {
                    add_calc2(&this->unk_30C, 15.0f, 1.0f, 2.0f);
                } else {
                    add_calc2(&this->unk_30C, 7.0f, 1.0f, 0.2f);
                }
            }
            this->unk_1B4 = 0.0f;
            Skeleton_Info2_anime_play(&this->skelAnime);
            if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanondorfTransformEndAnim, 0.0f);
                this->unk_194 = 1000.0f;
            }
            add_calc2(&this->subCamEye.x, (this->actor.world.pos.x + 200.0f) - 90.0f, 0.1f, 6.3999996f);
            add_calc2(&this->subCamEye.y, ((this->actor.world.pos.y + 60.0f) - 60.0f) - 70.0f, 0.1f, 13.0f);
            add_calc2(&this->subCamAt.y, this->actor.world.pos.y + 40.0f, 0.1f, 3.6999998f);
            if (this->unk_398 == 30) {
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_BIGMASIC);
                Actor_SE_set(&this->actor, NA_SE_EN_GANON_THROW_BIG);
            }
            if (this->unk_398 <= 50) {
                sp8D = true;
            }
            if (this->unk_398 >= 60) {
                Camera* camera = Gama_play_get_camera(play, CAM_ID_MAIN);

                camera->eye = this->subCamEye;
                camera->eyeNext = this->subCamEye;
                camera->at = this->subCamAt;
                this->unk_39C = 17;
                this->unk_398 = 0;
                this->unk_337 = 2;
                obj_bank_set(this, play, OBJECT_GANON2, false);
                Skeleton_Info_dt(&this->skelAnime, play);
                Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGanonSkel, NULL, NULL, NULL, 0);
                obj_bank_set(this, play, OBJECT_GANON_ANIME3, false);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_84);
                this->unk_314 = 3;
            }
            // fake, tricks the compiler into using stack the way we need it to
            if (zero) {
                add_calc2(&this->subCamAt.y, 0.0f, 0.0f, 0.0f);
            }
            break;
        case 17:
            this->unk_339 = 6;
            Skeleton_Info2_anime_play(&this->skelAnime);
            this->subCamEye.x = player->actor.world.pos.x - 40.0f;
            this->subCamEye.y = player->actor.world.pos.y + 40.0f;
            this->subCamEye.z = player->actor.world.pos.z + 20.0f;
            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = (player->actor.world.pos.y + 10.0f + 60.0f) - 30.0f;
            this->subCamAt.z = player->actor.world.pos.z;
            if (this->unk_398 == 25) {
                this->unk_39C = 18;
                this->unk_398 = 0;
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonUncurlAndFlailAnim, 0.0f);
                this->skelAnime.playSpeed = 0.0f;
                this->subCamEye.x = ((this->actor.world.pos.x + 500.0f) - 350.0f) - 50.0f;
                this->subCamEye.y = this->actor.world.pos.y;
                this->subCamEye.z = this->actor.world.pos.z;
                this->subCamAt.x = this->actor.world.pos.x + 50.0f;
                this->subCamAt.y = this->actor.world.pos.y + 60.0f;
                this->subCamAt.z = this->actor.world.pos.z;
                this->actor.world.rot.y = 0x4000;
            }
            break;
        case 18:
            this->unk_339 = 6;
            if (this->unk_398 == 30) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_GANON_BOSS);
            }
            add_calc2(&this->unk_30C, 7.0f, 1.0f, 0.1f);
            add_calc2(&this->subCamEye.x, (this->actor.world.pos.x + 500.0f) - 350.0f, 0.1f, 1.0f);
            add_calc2(&this->subCamAt.x, this->actor.world.pos.x, 0.1f, 1.0f);
            add_calc2(&this->unk_228, 1.0f, 0.1f, 0.02f);
            if (this->unk_398 == 65) {
                this->unk_39C = 19;
                this->unk_398 = 0;
            }
            break;
        case 19:
            this->unk_394 += 0.5f;
            this->unk_339 = 6;
            this->actor.world.pos.y += this->actor.velocity.y;
            this->actor.velocity.y -= 1.0f;
            if (this->unk_398 == 10) {
                this->unk_39C = 20;
                this->unk_398 = 0;
                this->actor.world.pos.x += 250;
                this->actor.world.pos.y = 1886.0f;
                this->unk_394 = 0.0f;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_83);
                this->unk_30C = 5.0f;
                this->unk_228 = 1.0f;
            }
            break;
        case 20:
            this->unk_339 = 6;
            Skeleton_Info2_anime_play(&this->skelAnime);
            this->actor.world.pos.y += this->actor.velocity.y;
            this->actor.velocity.y -= 1.0f;
            player->actor.world.pos.x = 250.0f;
            player->actor.world.pos.y = 1086.0f;
            player->actor.world.pos.z = -266.0f;
            player->actor.shape.rot.y = -0x4000;
            this->subCamEye.x = (player->actor.world.pos.x - 40.0f) - 200.0f;
            this->subCamEye.y = (player->actor.world.pos.y + 40.0f) - 30.0f;
            this->subCamEye.z = (player->actor.world.pos.z - 20.0f) + 100.0f;
            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = ((player->actor.world.pos.y + 10.0f + 60.0f) - 20.0f) + 30.0f;
            this->subCamAt.z = player->actor.world.pos.z;
            this->subCamUp.x = 0.8f;
            if (this->actor.world.pos.y <= 1099.0f) {
                this->actor.world.pos.y = 1099.0f;
                this->unk_39C = 21;
                this->unk_398 = 0;
                this->unk_420 = 10.0f;
                this->actor.velocity.y = 0.0f;
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonUncurlAndFlailAnim, 0.0f);
                gn2_chakuchi_dust_se_set(this, play, 0, 3);
                z_vibctl2_vib_force_set(0.0f, 200, 20, 20);
            }
            break;
        case 21:
            this->unk_339 = 6;
            Skeleton_Info2_anime_play(&this->skelAnime);
            this->unk_41C = cos_s(play->gameplayFrames * 0x8000) * this->unk_420;
            add_calc0(&this->unk_420, 1.0f, 0.75f);
            if (this->unk_398 == 30) {
                this->unk_39C = 22;
                this->unk_30C = 10.0f;
            } else {
                break;
            }
            FALLTHROUGH;
        case 22:
            if (this->unk_398 < 60) {
                this->unk_339 = 7;
            }
            this->subCamUp.x = 0.0f;
            this->actor.world.pos.y = 1099.0f;
            Skeleton_Info2_anime_play(&this->skelAnime);
            add_calc0(&this->unk_30C, 1.0f, 0.1f);
            if (this->unk_398 > 50) {
                add_calc2(&this->unk_224, 1.0f, 1.0f, 0.025f);
            }
            if (this->unk_398 == 60) {
                this->unk_336 = 2;
            }
            if (this->unk_398 == 80) {
                obj_bank_set(this, play, OBJECT_GANON2, false);
                Actor_Name_Disp_Set(play, &play->actorCtx.titleCtx, SEGMENTED_TO_VIRTUAL(gGanonTitleCardTex), 160,
                                       180, 128, 40);
            }
            this->subCamEye.x = ((this->actor.world.pos.x + 500.0f) - 350.0f) + 100.0f;
            this->subCamEye.y = this->actor.world.pos.y;
            this->subCamEye.z = this->actor.world.pos.z;
            this->subCamAt.x = this->actor.world.pos.x;
            this->subCamAt.z = this->actor.world.pos.z;
            this->subCamAt.y = (this->unk_1B8.y + 60.0f) - 40.0f;
            if (this->unk_398 > 166 && this->unk_398 < 173) {
                this->unk_312 = 2;
            }
            if (this->unk_398 > 186 && this->unk_398 < 196) {
                this->unk_312 = 1;
            }
            if (this->unk_398 > 202 && this->unk_398 < 210) {
                this->unk_312 = 2;
            }
            if ((this->unk_398 == 166) || (this->unk_398 == 185) || (this->unk_398 == 200)) {
                Na_StartSystemSe_F(NA_SE_EN_MGANON_SWORD);
                Na_StartSystemSe_F(NA_SE_EN_MGANON_ROAR);
            }
            if (this->unk_398 == 215) {
                this->unk_39C = 23;
                this->unk_224 = 0.0f;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_85);
            }
            break;
        case 23:
            Skeleton_Info2_anime_play(&this->skelAnime);
            if (this->unk_398 > 222 && this->unk_398 < 232) {
                this->unk_312 = 2;
            }
            if (this->unk_398 == 222) {
                Na_StartSystemSe_F(NA_SE_EN_MGANON_SWORD);
                Na_StartSystemSe_F(NA_SE_EN_MGANON_ROAR);
            }
            this->subCamEye.x = (player->actor.world.pos.x - 40.0f) + 6.0f;
            this->subCamEye.y = player->actor.world.pos.y + 40.0f;
            this->subCamEye.z = (player->actor.world.pos.z + 20.0f) - 7.0f;
            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = ((player->actor.world.pos.y + 10.0f + 60.0f) - 20.0f) - 2.0f;
            this->subCamAt.z = player->actor.world.pos.z;
            if (this->unk_398 == 228) {
                Na_StartSystemSe_F(NA_SE_IT_SHIELD_REFLECT_SW);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_86);
                z_vibctl2_vib_force_set(0.0f, 255, 10, 50);
            }
            if (this->unk_398 >= 229) {
                play->envCtx.fillScreen = true;
                play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] = play->envCtx.screenFillColor[2] =
                    255;
                play->envCtx.screenFillColor[3] = 100;
                if (this->unk_398 == 234) {
                    Vec3f sp68;

                    play->envCtx.fillScreen = false;
                    this->unk_39C = 24;
                    this->unk_398 = 0;
                    sp68 = player->actor.world.pos;
                    sp68.y += 60.0f;
                    gn2_master_s_ct(play, &sp68);
                    play->envCtx.lightBlend = 0.0f;
                    play->envCtx.prevLightSetting = 0;
                    this->unk_339 = 0;
                }
            }
            break;
        case 24:
            Skeleton_Info2_anime_play(&this->skelAnime);
            if (1) {
                BossGanon2Effect* effects = play->specialEffects;

                this->subCamAt = effects[0].position;
                this->subCamEye.x = effects[0].position.x + 70.0f;
                this->subCamEye.y = effects[0].position.y - 30.0f;
                this->subCamEye.z = effects[0].position.z + 70.0f;
                if ((this->unk_398 & 3) == 0) {
                    Na_StartSystemSe_F(NA_SE_IT_SWORD_SWING);
                }
                if (this->unk_398 == 25) {
                    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_87);
                    this->unk_39C = 25;
                    this->unk_398 = 0;
                }
                break;
            }
        case 25:
            Skeleton_Info2_anime_play(&this->skelAnime);
            this->subCamEye.x = (player->actor.world.pos.x - 40.0f) + 80.0f;
            this->subCamEye.y = player->actor.world.pos.y + 40.0f + 10.0f;
            this->subCamEye.z = player->actor.world.pos.z + 20.0f + 10.0f;
            this->subCamAt.x = player->actor.world.pos.x - 20.0f;
            this->subCamAt.y = ((player->actor.world.pos.y + 10.0f + 60.0f) - 20.0f) - 3.0f;
            this->subCamAt.z = (player->actor.world.pos.z - 40.0f) - 10.0f;
            if (this->unk_398 == 10) {
                BossGanon2Effect* effects = play->specialEffects;

                effects[0].unk_2E = 1;
                effects[0].position.x = zelda->actor.world.pos.x + 50.0f + 10.0f;
                effects[0].position.y = zelda->actor.world.pos.y + 350.0f;
                effects[0].position.z = zelda->actor.world.pos.z - 25.0f;
                effects[0].velocity.x = 0.0f;
                effects[0].velocity.z = 0.0f;
                effects[0].velocity.y = -30.0f;
                this->unk_39C = 26;
                this->unk_398 = 0;
            } else {
                break;
            }
            FALLTHROUGH;
        case 26:
            this->subCamEye.x = zelda->actor.world.pos.x + 100.0f + 30.0f;
            this->subCamEye.y = zelda->actor.world.pos.y + 10.0f;
            this->subCamEye.z = zelda->actor.world.pos.z + 5.0f;
            this->subCamAt.x = zelda->actor.world.pos.x;
            this->subCamAt.y = zelda->actor.world.pos.y + 30.0f;
            this->subCamAt.z = zelda->actor.world.pos.z - 20.0f;
            this->subCamUp.z = -0.5f;
            if (this->unk_398 == 13) {
                zelda->unk_3C8 = 6;
            }
            if (this->unk_398 == 50) {
                this->unk_39C = 27;
                this->unk_398 = 0;
            }
            break;
        case 27:
            this->subCamUp.z = 0.0f;
            if (this->unk_398 == 4) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_88);
            }
            this->subCamEye.x = player->actor.world.pos.x - 20.0f;
            this->subCamEye.y = player->actor.world.pos.y + 50.0f;
            this->subCamEye.z = player->actor.world.pos.z;
            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = player->actor.world.pos.y + 50.0f;
            this->subCamAt.z = player->actor.world.pos.z;
            if (this->unk_398 == 26) {
                navy = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
                while (navy != NULL) {
                    if (navy->id == ACTOR_EN_ELF) {
                        this->subCamEye.x = navy->world.pos.x - 30.0f;
                        this->subCamEye.y = navy->world.pos.y;
                        this->subCamEye.z = navy->world.pos.z;
                        this->subCamAt.x = navy->world.pos.x;
                        this->subCamAt.y = navy->world.pos.y;
                        this->subCamAt.z = navy->world.pos.z;
                        break;
                    }
                    navy = navy->next;
                }
                this->unk_39C = 28;
                this->unk_398 = 0;
            }
            break;
        case 28:
            if (this->unk_398 == 5) {
                message_set(play, 0x70D6, NULL);
            }
            if (navy != NULL) {
                this->subCamEye.x = navy->world.pos.x - 20.0f;
                this->subCamEye.y = navy->world.pos.y;
                this->subCamEye.z = navy->world.pos.z;
                add_calc2(&this->subCamAt.x, navy->world.pos.x, 0.2f, 50.0f);
                add_calc2(&this->subCamAt.y, navy->world.pos.y, 0.2f, 50.0f);
                add_calc2(&this->subCamAt.z, navy->world.pos.z, 0.2f, 50.0f);
                if ((this->unk_398 > 40) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                    this->unk_39C = 29;
                    this->unk_398 = 0;
                    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonRoarAnim, 0.0f);
                    this->unk_194 = Si2_anime_end_frame(&gGanonRoarAnim);
                    this->actor.shape.yOffset = 0.0f;
                    this->actor.world.pos.y = 1086.0f;
                    this->actor.gravity = -1.0f;
                    this->unk_335 = 1;
                    this->unk_224 = 1.0f;
                }
            }
            break;
        case 29:
            Skeleton_Info2_anime_play(&this->skelAnime);
            this->subCamEye.x = (((this->actor.world.pos.x + 500.0f) - 350.0f) + 100.0f) - 60.0f;
            this->subCamAt.x = this->actor.world.pos.x;
            this->subCamAt.z = this->actor.world.pos.z;
            this->subCamEye.y = this->actor.world.pos.y;
            this->subCamEye.z = this->actor.world.pos.z + 10.0f;
            this->subCamAt.y = (this->unk_1B8.y + 60.0f) - 40.0f;
            player->actor.shape.rot.y = -0x4000;
            player->actor.world.pos.x = 140.0f;
            player->actor.world.pos.z = -196.0f;
            if (this->unk_398 == 50) {
                Actor_SE_set(&this->actor, NA_SE_EN_MGANON_ROAR);
            }
            if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
                Camera* camera = Gama_play_get_camera(play, CAM_ID_MAIN);

                camera->eye = this->subCamEye;
                camera->eyeNext = this->subCamEye;
                camera->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                this->unk_39C = 0;
                this->unk_337 = 1;
                mode_wait_init(this, play);
                this->unk_1A2[1] = 50;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                zelda->unk_3C8 = 7;
            }
            break;
    }

    if ((this->unk_30C > 4.0f) && !sp8D) {
        Actor_SE_set(&this->actor, NA_SE_EN_GANON_BODY_SPARK - SFX_FLAG);
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        // fake, tricks the compiler into putting some pointers on the stack
        if (zero) {
            osSyncPrintf(NULL, 0, 0);
        }
        this->subCamAt.y += this->unk_41C;
        Gama_play_camera_lookat(play, this->subCamId, &this->subCamAt, &this->subCamEye, &this->subCamUp);
    }
}

void Ganon2_ken_gareki_check(BossGanon2* this, PlayState* play) {
    if ((this->unk_312 != 0) && (this->subCamId == SUB_CAM_ID_DONE)) {
        Actor* actor = play->actorCtx.actorLists[ACTORCAT_PROP].head;

        while (actor != NULL) {
            if (actor->id == ACTOR_DEMO_GJ) {
                DemoGj* gj = (DemoGj*)actor;

                if ((PARAMS_GET_U(actor->params, 0, 8) == 0x10) || (PARAMS_GET_U(actor->params, 0, 8) == 0x11) ||
                    (PARAMS_GET_U(actor->params, 0, 8) == 0x16)) {
                    if (SQ(this->unk_218.x - gj->dyna.actor.world.pos.x) +
                            SQ(this->unk_218.z - gj->dyna.actor.world.pos.z) <
                        SQ(100.0f)) {
                        s32 pad;
                        Vec3f sp28;

                        Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y) + 0.5f, MTXMODE_NEW);
                        sp28.x = 0.0f;
                        sp28.y = 0.0f;
                        sp28.z = 1.0f;
                        Matrix_Position(&sp28, &gj->unk_26C);
                        gj->killFlag = true;
                        z_vibctl2_vib_force_set(0.0f, 150, 20, 50);
                        this->unk_392 = 6;
                        return;
                    }
                }
            }

            actor = actor->next;
        }

        if (this->unk_392 == 4) {
            Na_StartSystemSe_F(NA_SE_EV_GRAVE_EXPLOSION);
        }

        if (this->unk_392 == 3) {
            Na_StartSystemSe_F(NA_SE_EN_MGANON_SWDIMP);
        }
    }
}

s32 Ganon2_ken_gareki_atack_check(BossGanon2* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_PROP].head;

    while (actor != NULL) {
        if (actor->id == ACTOR_DEMO_GJ) {
            DemoGj* gj = (DemoGj*)actor;

            if ((PARAMS_GET_U(actor->params, 0, 8) == 0x10) || (PARAMS_GET_U(actor->params, 0, 8) == 0x11) ||
                (PARAMS_GET_U(actor->params, 0, 8) == 0x16)) {
                if (SQ(this->actor.world.pos.x - gj->dyna.actor.world.pos.x) +
                        SQ(this->actor.world.pos.z - gj->dyna.actor.world.pos.z) <
                    SQ(200.0f)) {
                    return true;
                }
            }
        }

        actor = actor->next;
    }

    return false;
}

void kubi_search(BossGanon2* this, PlayState* play, u8 arg2) {
    s16 temp_v1;
    s16 phi_a1;

    if (this->unk_313 || (arg2 != 0)) {
        phi_a1 = this->actor.shape.rot.y - this->actor.yawTowardsPlayer;

        if (phi_a1 > 0x3000) {
            phi_a1 = 0x3000;
        } else if (phi_a1 < -0x3000) {
            phi_a1 = -0x3000;
        }
    } else if (this->unk_19C & 0x20) {
        phi_a1 = 0x3000;
    } else {
        phi_a1 = -0x3000;
    }

    adds(&this->unk_31A, phi_a1, 5, 0x7D0);

    temp_v1 = atans_table(this->actor.xzDistToPlayer, 150.0f) - 0xBB8;
    temp_v1 = CLAMP_MAX(temp_v1, 0x1B58);
    temp_v1 = CLAMP_MIN(temp_v1, -0x1B58);

    adds(&this->unk_31C, temp_v1, 5, 0x7D0);
}

void body_search(BossGanon2* this, PlayState* play, u8 arg2) {
    if (arg2 != 0 || this->unk_313) {
        f32 phi_f0;
        f32 phi_f2;

        adds(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 5, this->unk_320);

        if (this->unk_334 != 0) {
            phi_f0 = 5000.0f;
            phi_f2 = 200.0f;
        } else {
            phi_f0 = 3000.0f;
            phi_f2 = 30.0f;
        }

        add_calc2(&this->unk_320, phi_f0, 1.0f, phi_f2);
    } else {
        this->unk_320 = 0.0f;
    }
}

void look_on_set(BossGanon2* this) {
    if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) < 0x2800) {
        this->unk_313 = true;
        this->actor.focus.pos = this->unk_1B8;
    } else {
        this->unk_313 = false;
        this->actor.focus.pos = this->unk_1C4;
    }
}

void attack_check(BossGanon2* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 150.0f &&
        ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) < 0x2800) {
        this->unk_311 = false;
        mode_atack_init(this, play);
        Nai_StopFx(NA_SE_EN_MGANON_UNARI);
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && Ganon2_ken_gareki_atack_check(this, play)) {
        this->unk_311 = false;
        mode_atack_init(this, play);
        Nai_StopFx(NA_SE_EN_MGANON_UNARI);
    }
}

static void mode_wait_init(BossGanon2* this, PlayState* play) {
    s32 sp28;
    s32 objectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_GANON2);

    if (Object_Exchange_bank_dma_check(&play->objectCtx, objectSlot)) {
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanonGuardIdleAnim, -10.0f);
        this->actionFunc = mode_wait;

        if (this->unk_334 != 0) {
            this->unk_1A2[0] = rnd_f(30.0f);
        } else {
            this->unk_1A2[0] = 40;
        }

        this->unk_336 = 1;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->unk_228 = 1.0f;
        this->unk_224 = 1.0f;
    } else {
        this->actionFunc = mode_wait_init;
    }
}

static void mode_wait(BossGanon2* this, PlayState* play) {
    if (this->unk_390 == 0) {
        this->unk_390 = (s16)rnd_f(50.0f) + 30;
        Actor_SE_set(&this->actor, NA_SE_EN_MGANON_UNARI);
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc0(&this->actor.speed, 0.5f, 1.0f);

    if (this->unk_1A2[0] == 0) {
        mode_walk_init(this, play);
    } else if (this->unk_1A2[1] == 0) {
        attack_check(this, play);
    }

    kubi_search(this, play, 0);
    body_search(this, play, 0);
}

static void mode_mahi_init(BossGanon2* this, PlayState* play) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanonGuardIdleAnim, -10.0f);
    this->actionFunc = mode_mahi;
    this->unk_1A2[0] = 40;
}

static void mode_mahi(BossGanon2* this, PlayState* play) {
    s16 target;

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc0(&this->actor.speed, 0.5f, 1.0f);

    if (this->unk_1A2[0] == 0) {
        mode_walk_init(this, play);
    }

    if (this->unk_1A2[0] < 30 && this->unk_1A2[0] >= 10) {
        target = sin_s(this->unk_1A2[0] * 0x3000) * (f32)0x2000;
    } else {
        target = 0;
    }

    adds(&this->unk_31A, target, 2, 0x4000);
}

static void mode_yahit_init(BossGanon2* this, PlayState* play) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanonStunStartAnim, -2.0f);
    this->unk_194 = Si2_anime_end_frame(&gGanonStunStartAnim);
    this->unk_1AC = 0;
    this->actionFunc = mode_yahit;
}

static void mode_yahit(BossGanon2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc0(&this->actor.speed, 0.5f, 1.0f);

    switch (this->unk_1AC) {
        case 0:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
                this->unk_1AC = 1;
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanonStunLoopAnim, 0.0f);
                this->unk_1A2[0] = 80;
            }
            break;
        case 1:
            if (this->unk_1A2[0] == 0) {
                this->unk_1AC = 2;
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanonStunEndAnim, -5.0f);
                this->unk_194 = Si2_anime_end_frame(&gGanonStunEndAnim);
            }
            break;
        case 2:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
                mode_walk_init(this, play);
            }
            break;
    }
}

static void mode_damage_init(BossGanon2* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonDamageAnim, -3.0f);
    this->unk_194 = Si2_anime_end_frame(&gGanonDamageAnim);
    this->actionFunc = mode_damage;
}

static void mode_damage(BossGanon2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc0(&this->actor.speed, 0.5f, 2.0f);

    if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
        mode_walk_init(this, play);
    }
}

static void mode_walk_init(BossGanon2* this, PlayState* play) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanonGuardWalkAnim, -10.0f);
    this->actionFunc = mode_walk;
    this->unk_338 = 0;
    this->unk_1A2[0] = 100;
    this->unk_390 = (s16)rnd_f(50.0f) + 50;
}

static void mode_walk(BossGanon2* this, PlayState* play) {
    f32 phi_f0;

    if (this->unk_390 == 0) {
        this->unk_390 = (s16)rnd_f(50.0f) + 30;
        Actor_SE_set(&this->actor, NA_SE_EN_MGANON_UNARI);
    }

    add_calc2(&this->unk_324, 255.0f, 1.0f, 10.0f);

    if (this->unk_338 != 0) {
        if (Skeleton_Info_frame_check(&this->skelAnime, 13.0f)) {
            gn2_chakuchi_dust_se_set(this, play, 1, 3);
        } else if (Skeleton_Info_frame_check(&this->skelAnime, 28.0f)) {
            gn2_chakuchi_dust_se_set(this, play, 2, 3);
        }
        if (this->actor.xzDistToPlayer < 200.0f) {
            this->unk_338 = 0;
            Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanonGuardWalkAnim, -10.0f);
        } else {
            this->skelAnime.playSpeed = ((this->actor.xzDistToPlayer - 300.0f) * 0.005f) + 1.0f;
            if (this->skelAnime.playSpeed > 2.0f) {
                this->skelAnime.playSpeed = 2.0f;
            }
            if (this->unk_334 != 0) {
                this->skelAnime.playSpeed *= 1.5f;
            }
        }
        phi_f0 = this->skelAnime.playSpeed * 3.0f;
    } else {
        phi_f0 = 2.0f;
        if (this->actor.xzDistToPlayer >= 200.0f) {
            this->unk_338 = 1;
            Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanonWalkAnim, -10.0f);
        }
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc2(&this->actor.speed, phi_f0, 0.5f, 1.0f);

    if (this->unk_1A2[0] == 0) {
        mode_wait_init(this, play);
    } else {
        attack_check(this, play);
    }

    kubi_search(this, play, 1);
    body_search(this, play, 1);
}

static void mode_atack_init(BossGanon2* this, PlayState* play) {
    if (this->unk_311 == 0) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonLeftSwordSwingAnim, -5.0f);
        this->unk_194 = Si2_anime_end_frame(&gGanonLeftSwordSwingAnim);
        this->unk_198 = (this->unk_194 - 15.0f) - 5.0f;
    } else {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonRightSwordSwingAnim, -5.0f);
        this->unk_194 = Si2_anime_end_frame(&gGanonRightSwordSwingAnim);
        this->unk_198 = (this->unk_194 - 15.0f) - 5.0f;
    }

    this->actionFunc = mode_atack;
}

static void mode_atack(BossGanon2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_198)) {
        Actor_SE_set(&this->actor, NA_SE_EN_MGANON_SWORD);
        Actor_SE_set(&this->actor, NA_SE_EN_MGANON_ROAR);
    }

    if (this->unk_311 == 0) {
        if (((this->unk_198 - 4.0f) < this->skelAnime.curFrame) &&
            (this->skelAnime.curFrame < (this->unk_198 + 6.0f))) {
            this->unk_312 = 1;
        }
    } else if ((((this->unk_198 - 4.0f) + 4.0f) < this->skelAnime.curFrame) &&
               (this->skelAnime.curFrame < (this->unk_198 + 6.0f))) {
        this->unk_312 = 2;
    }

    add_calc0(&this->actor.speed, 0.5f, 1.0f);

    if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
        this->unk_311 = 1 - this->unk_311;

        if ((this->unk_311 == 1) && (this->actor.xzDistToPlayer < 250.0f) && this->unk_313) {
            mode_atack_init(this, play);
        } else {
            mode_wait_init(this, play);
        }
    }

    kubi_search(this, play, 0);

    if ((this->unk_334 == 0) && (this->unk_311 == 0)) {
        this->unk_320 = 0.0f;
    } else {
        body_search(this, play, 0);
    }
}

void mode_damagedemo_init(BossGanon2* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonDownedStartAnim, -5.0f);
    this->unk_194 = Si2_anime_end_frame(&gGanonDownedStartAnim);
    this->actionFunc = mode_damagedemo;
    this->unk_1AC = 0;
    this->unk_39C = 0;
    Actor_SE_set(&this->actor, NA_SE_EN_MGANON_DEAD1);
    this->unk_336 = 0;
}

void mode_damagedemo(BossGanon2* this, PlayState* play) {
    Vec3f sp5C;
    Vec3f sp50;
    Camera* mainCam1;
    Player* player;
    Camera* mainCam2;
    Camera* mainCam3;

    mainCam1 = Gama_play_get_camera(play, CAM_ID_MAIN);
    player = GET_PLAYER(play);
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->unk_398++;
    this->unk_339 = 20;

    switch (this->unk_39C) {
        case 0:
            Demo_play_start(play, &play->csCtx);
            this->subCamId = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            this->unk_39C = 1;
            this->subCamEye = mainCam1->eye;
            this->subCamAt = mainCam1->at;
            this->unk_1A2[0] = 300;
            this->unk_1A2[1] = 100;
            play->envCtx.lightBlend = 0.0f;
            FALLTHROUGH;
        case 1:
            if (this->unk_1A2[1] == 50) {
                Na_StartSystemSe_F(NA_SE_EN_MGANON_WALK);
            }
            Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y) + 0.3f, MTXMODE_NEW);
            sp5C.x = 0.0f;
            sp5C.y = 0.0f;
            sp5C.z = 250.0f;
            Matrix_Position(&sp5C, &sp50);
            add_calc2(&this->subCamEye.x, this->actor.world.pos.x + sp50.x, 0.2f, 100.0f);
            add_calc2(&this->subCamEye.y, 1136.0f, 0.2f, 100.0f);
            add_calc2(&this->subCamEye.z, this->actor.world.pos.z + sp50.z, 0.2f, 100.0f);
            add_calc2(&this->subCamAt.x, this->unk_1B8.x, 0.2f, 100.0f);
            add_calc2(&this->subCamAt.y, this->unk_1B8.y, 0.2f, 100.0f);
            add_calc2(&this->subCamAt.z, this->unk_1B8.z, 0.2f, 100.0f);
            if (this->unk_1A2[1] == 0) {
                this->unk_39C = 2;
                this->unk_1A2[1] = 90;
            }
            break;
        case 2:
            this->unk_1A2[0] = 300;
            this->subCamEye.x = zelda->actor.world.pos.x - 100.0f;
            this->subCamEye.y = zelda->actor.world.pos.y + 30.0f;
            this->subCamEye.z = (zelda->actor.world.pos.z + 30.0f) - 60.0f;
            this->subCamAt.x = zelda->actor.world.pos.x;
            this->subCamAt.y = zelda->actor.world.pos.y + 30.0f;
            this->subCamAt.z = zelda->actor.world.pos.z - 10.0f;
            add_calc0(&this->unk_324, 1.0f, 5.0f);
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 1.0f / 51);
            if (this->unk_1A2[1] == 80) {
                message_set(play, 0x70D7, NULL);
            }
            if ((this->unk_1A2[1] < 30) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                s32 pad;

                mainCam2 = Gama_play_get_camera(play, CAM_ID_MAIN);
                mainCam2->eye = this->subCamEye;
                mainCam2->eyeNext = this->subCamEye;
                mainCam2->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                this->unk_39C = 3;
            }
            break;
        case 10:
            Demo_play_start(play, &play->csCtx);
            this->subCamId = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            this->unk_39C = 11;
            this->unk_334 = 1;
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_96);
            this->unk_398 = 0;
            FALLTHROUGH;
        case 11:
            player->actor.world.pos.x = zelda->actor.world.pos.x + 50.0f + 10.0f;
            player->actor.world.pos.z = zelda->actor.world.pos.z - 25.0f;
            player->actor.shape.rot.y = -0x8000;
            this->subCamEye.x = (player->actor.world.pos.x + 100.0f) - 80.0f;
            this->subCamEye.y = (player->actor.world.pos.y + 60.0f) - 40.0f;
            this->subCamEye.z = player->actor.world.pos.z - 110.0f;
            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = (player->actor.world.pos.y + 60.0f) - 25.0f;
            this->subCamAt.z = player->actor.world.pos.z;
            if (this->unk_398 == 80) {
                mainCam3 = Gama_play_get_camera(play, CAM_ID_MAIN);
                mainCam3->eye = this->subCamEye;
                mainCam3->eyeNext = this->subCamEye;
                mainCam3->at = this->subCamAt;
                this->unk_39C = 3;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
            }
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }

    switch (this->unk_1AC) {
        case 0:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanonDownedLoopAnim, 0.0f);
                this->unk_1AC = 1;
            }
            break;
        case 1:
            if ((play->gameplayFrames % 32) == 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_MGANON_BREATH);
            }

            if ((this->unk_1A2[0] == 0) || (this->unk_334 != 0)) {
                f32 temp_f2 = -200.0f - player->actor.world.pos.x;
                f32 temp_f12 = -200.0f - player->actor.world.pos.z;

#if OOT_VERSION < NTSC_1_2
                if (sqrtf(SQ(temp_f2) + SQ(temp_f12)) <= 800.0f)
#else
                if (sqrtf(SQ(temp_f2) + SQ(temp_f12)) <= 784.0f)
#endif
                {
                    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonGetUpAnim, 0.0f);
                    this->unk_194 = Si2_anime_end_frame(&gGanonGetUpAnim);
                    this->unk_1AC = 2;
                    this->unk_1A2[0] = 40;
                    Actor_SE_set(&this->actor, NA_SE_EN_MGANON_ROAR);
                }
            }
            break;
        case 2:
            add_calc2(&this->unk_324, 255.0f, 1.0f, 10.0f);
            add_calc0(&play->envCtx.lightBlend, 1.0f, 2.0f / 51.0f);
            if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
                mode_wait_init(this, play);
                if (this->unk_334 == 0) {
                    this->actor.colChkInfo.health = 25;
                }
                this->unk_336 = 1;
            }
            break;
    }

    add_calc0(&this->actor.speed, 0.5f, 1.0f);
}

static void mode_enddemo_init(BossGanon2* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonDownedStartAnim, -5.0f);
    this->unk_194 = Si2_anime_end_frame(&gGanonDownedStartAnim);
    this->actionFunc = mode_enddemo;
    this->unk_1AC = 0;
    this->unk_39C = 0;
    Actor_SE_set(&this->actor, NA_SE_EN_MGANON_DEAD1);
    this->unk_314 = 4;
}

void toketu_set(BossGanon2* this, PlayState* play) {
    u8 i;
    Vec3f velocity;
    Vec3f accel;
#if OOT_VERSION < PAL_1_0
    Color_RGBA8 prim = { 120, 0, 0, 255 };
    Color_RGBA8 env = { 120, 0, 0, 255 };
#else
    static Color_RGBA8 prim = { 0, 120, 0, 255 };
    static Color_RGBA8 env = { 0, 120, 0, 255 };
#endif
    Vec3f pos;

    for (i = 0; i < 70; i++) {
        velocity.x = rnd_fx(50.0f);
        velocity.y = rnd_fx(10.0f) + 5.0f;
        velocity.z = rnd_fx(50.0f);
        accel.x = 0.0f;
        accel.y = -1.0f;
        accel.z = 0.0f;
        pos.x = this->unk_1B8.x;
        pos.y = this->unk_1B8.y;
        pos.z = this->unk_1B8.z;
        Effect_SS_Dust_sc_cl_co_ct(play, &pos, &velocity, &accel, &prim, &env, (s16)rnd_f(50.0f) + 50, 0, 17);
    }
}

static Vec3f gn2_enddemo7_eye_data[] = {
    { 10.0f, -10.0f, 0.0f },
    { 0.0f, 0.0f, -60.0f },
    { 70.0f, -30.0f, 10.0f },
};

static Vec3f gn2_enddemo7_way_data[] = {
    { -20.0f, 0.0f, 0.0f },
    { -15.0f, 0.0f, 10.0f },
    { -16.0f, -12.0f, 40.0f },
};

static void mode_enddemo(BossGanon2* this, PlayState* play) {
    Player* player;
    f32 temp_f14;
    f32 temp_f12;
    s16 temp_a0_2;

    player = GET_PLAYER(play);
    this->unk_398++;
    Skeleton_Info2_anime_play(&this->skelAnime);

    this->subCamUp.x = 0.0f;
    this->subCamUp.y = 1.0f;
    this->subCamUp.z = 0.0f;

    switch (this->unk_39C) {
        case 0:
            Demo_play_start(play, &play->csCtx);
            this->subCamId = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            this->unk_39C = 1;
            this->unk_398 = 0;
            zelda->unk_3C8 = 9;
            this->unk_31C = 0;
            this->unk_1A2[2] = 0;
            this->unk_336 = 0;
            this->unk_324 = 0.0f;
            this->actor.speed = 0.0f;
            this->unk_31A = this->unk_31C;
            play->envCtx.lightBlend = 0.0f;
            FALLTHROUGH;
        case 1:
            if (this->unk_398 < 90) {
                this->unk_339 = 20;
                add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.1f);
            } else if (this->unk_398 >= 90) {
                this->unk_339 = 21;
                add_calc0(&play->envCtx.lightBlend, 1.0f, 0.08f);
            }
            if (this->unk_398 == 50) {
                Na_StartSystemSe_F(NA_SE_EN_MGANON_WALK);
            }
            if (this->unk_398 > 90) {
                add_calc2(&this->unk_380, 0.25f, 1.0f, 0.0125f);
                this->unk_37C = 200.0f;
                Na_StartSystemSe_F(NA_SE_EV_TIMETRIP_LIGHT - SFX_FLAG);
            }
            if (this->unk_398 >= 110) {
                if (this->unk_398 == 110) {
                    Actor_SE_set(&this->actor, NA_SE_EN_GANON_HIT_THUNDER);
                    Actor_SE_set(&this->actor, NA_SE_EN_MGANON_DAMAGE);
                }
                add_calc2(&this->unk_30C, 10.0f, 0.2f, 5.0f);
                this->skelAnime.playSpeed = 3.0f;
            }
            if (this->unk_398 == 120) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_99);
            }
            this->actor.world.rot.y = 0x4000;
            this->actor.world.pos.x = this->actor.world.pos.z = 0.0f;
            player->actor.shape.rot.y = -0x4000;
            player->actor.world.pos.x = 200.0f;
            player->actor.world.pos.z = 30.0f;
            zelda->actor.world.pos.x = 340.0f;
            zelda->actor.world.pos.z = -250.0f;
            zelda->actor.world.rot.y = zelda->actor.shape.rot.y = -0x2000;
            this->subCamEye.x = 250;
            this->subCamEye.y = 1150.0f;
            this->subCamEye.z = 0.0f;
            this->subCamAt.x = this->unk_1B8.x;
            this->subCamAt.y = this->unk_1B8.y;
            this->subCamAt.z = this->unk_1B8.z;
            if (this->unk_398 > 135) {
                this->unk_39C = 2;
                this->unk_398 = 0;
            }
            break;
        case 2:
            this->unk_339 = 22;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.1f);
            Na_StartSystemSe_F(NA_SE_EV_TIMETRIP_LIGHT - SFX_FLAG);
            this->subCamEye.x = 250;
            this->subCamEye.y = 1150.0f;
            this->subCamEye.z = 0.0f;
            add_calc2(&this->subCamAt.x, zelda->actor.world.pos.x, 0.2f, 20.0f);
            add_calc2(&this->subCamAt.y, zelda->actor.world.pos.y + 50.0f, 0.2f, 10.0f);
            add_calc2(&this->subCamAt.z, zelda->actor.world.pos.z, 0.2f, 20.0f);
            if (this->unk_398 == 50) {
                this->unk_39C = 3;
                this->unk_398 = 0;
            }
            break;
        case 3:
            this->unk_339 = 22;
            Na_StartSystemSe_F(NA_SE_EV_TIMETRIP_LIGHT - SFX_FLAG);
            this->subCamEye.x = 330.0f;
            this->subCamEye.y = 1120.0f;
            this->subCamEye.z = -150.0f;
            this->subCamAt.x = zelda->actor.world.pos.x;
            this->subCamAt.y = zelda->actor.world.pos.y + 40.0f;
            this->subCamAt.z = zelda->actor.world.pos.z;
            if (this->unk_398 == 10) {
                message_set(play, 0x70D8, NULL);
            }
            if ((this->unk_398 > 80) && (message_check(&play->msgCtx) == TEXT_STATE_NONE)) {
                this->unk_39C = 4;
                this->unk_398 = 0;
            }
            break;
        case 4:
            if (this->unk_398 > 10) {
                add_calc0(&this->unk_37C, 1.0f, 10.0f);
                if (this->unk_398 == 30) {
                    zelda->unk_3C8 = 10;
                }
                this->unk_339 = 23;
                add_calc0(&play->envCtx.lightBlend, 1.0f, 0.05f);
            } else {
                this->unk_339 = 22;
            }
            if (this->unk_398 == 100) {
                this->unk_39C = 5;
                this->unk_398 = 40;
                this->skelAnime.playSpeed = 1.0f;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_100);
            }
            break;
        case 5:
            this->unk_339 = 23;
            if ((this->unk_398 >= 60) && (this->unk_398 <= 90)) {
                if (this->unk_398 == 62) {
                    Na_StartSystemSe_F(NA_SE_EV_TRIFORCE_FLASH);
                }
                add_calc2(&this->unk_38C, 200.0f, 1.0f, 8.0f);
            } else {
                add_calc0(&this->unk_38C, 1.0f, 8.0f);
            }
            if (this->unk_398 == 70) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_101);
            }
            if (this->unk_398 == 150) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_102);
            }
            this->unk_30C = 10.0f;
            player->actor.world.pos.x = 250.0f;
            player->actor.world.pos.z = 30.0f;
            this->subCamEye.x = player->actor.world.pos.x - 50.0f;
            this->subCamEye.y = player->actor.world.pos.y + 50.0f;
            this->subCamEye.z = player->actor.world.pos.z + 40.0f;
            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = player->actor.world.pos.y + 40.0f;
            this->subCamAt.z = player->actor.world.pos.z;
            if (this->unk_398 == 166) {
                Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                this->unk_39C = 6;
            }
            break;
        case 6:
            this->unk_339 = 23;
            temp_f14 = this->unk_1B8.x - player->actor.world.pos.x;
            temp_f12 = this->unk_1B8.z - player->actor.world.pos.z;
            temp_a0_2 = atans_table(temp_f12, temp_f14) - player->actor.shape.rot.y;
            if ((ABS(temp_a0_2) < 0x2000) && (sqrtf(SQ(temp_f14) + SQ(temp_f12)) < 70.0f) &&
                (player->meleeWeaponState != 0) && (player->heldItemAction == PLAYER_IA_SWORD_MASTER)) {
                Demo_play_start(play, &play->csCtx);
                this->subCamId = Gama_play_make_camera(play);
                Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
                Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
                this->unk_39C = 7;
                this->unk_398 = 0;
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonFinalBlowAnim, 0.0f);
                this->unk_194 = Si2_anime_end_frame(&gGanonFinalBlowAnim);
                play->tryPlayerCsAction(play, &this->actor, PLAYER_CSACTION_97);
            } else {
                break;
            }
            FALLTHROUGH;
        case 7:
            this->unk_339 = 23;
            add_calc0(&play->envCtx.lightBlend, 1.0f, 0.2f);
            player->actor.world.pos.x = 250.0f;
            player->actor.shape.rot.y = -0x4000;
            player->actor.world.pos.z = 30.0f;
            if ((this->unk_398 == 20) || (this->unk_398 == 30) || (this->unk_398 == 65) || (this->unk_398 == 40)) {
                Na_StartSystemSe_F(NA_SE_VO_LI_SWORD_N);
                Na_StartSystemSe_F(NA_SE_IT_SWORD_SWING_HARD);
            }
            if ((this->unk_398 == 22) || (this->unk_398 == 35) || (this->unk_398 == 72) || (this->unk_398 == 45)) {
                Na_StartSystemSe_F(NA_SE_EN_MGANON_DAMAGE);
                Na_StartSystemSe_F(NA_SE_IT_SHIELD_BOUND);
                play->envCtx.lightBlend = 1.0f;
            }
            if ((this->unk_398 == 22) || (this->unk_398 == 35) || (this->unk_398 == 72) || (this->unk_398 == 45)) {
                toketu_set(this, play);
            }
            if ((this->unk_398 >= 34) && (this->unk_398 < 40)) {
                this->subCamEye.x = 269.0f;
                this->subCamEye.y = 1112.0f;
                this->subCamEye.z = -28.0f;
                this->subCamAt.x = 234.0f;
                this->subCamAt.y = 1117.0f;
                this->subCamAt.z = -11.0f;
            } else {
                f32 phi_f0;
                s32 phi_a1;

                if (this->unk_398 < 30) {
                    phi_a1 = 0;
                } else if (this->unk_398 < 43) {
                    phi_a1 = 1;
                } else {
                    this->subCamUp.z = -0.8f;
                    player->actor.world.pos.x = 200.0f;
                    player->actor.world.pos.z = 10.0f;
                    phi_a1 = 2;
                }
                this->subCamEye.x = gn2_enddemo7_eye_data[phi_a1].x + (player->actor.world.pos.x - 50.0f);
                this->subCamEye.y = gn2_enddemo7_eye_data[phi_a1].y + (player->actor.world.pos.y + 50.0f);
                this->subCamEye.z = gn2_enddemo7_eye_data[phi_a1].z + (player->actor.world.pos.z + 40.0f);
                this->subCamAt.x = gn2_enddemo7_way_data[phi_a1].x + player->actor.world.pos.x;
                this->subCamAt.y = gn2_enddemo7_way_data[phi_a1].y + (player->actor.world.pos.y + 40.0f);
                this->subCamAt.z = gn2_enddemo7_way_data[phi_a1].z + player->actor.world.pos.z;
            }
            if (this->unk_398 > 80) {
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
                this->unk_39C = 75;
                this->unk_398 = 0;
                this->subCamEye.x = 112.0f;
                this->subCamEye.y = 1146.0f;
                this->subCamEye.z = 202.0f;
                this->subCamAt.x = 110.0f;
                this->subCamAt.y = 1144.0f;
                this->subCamAt.z = 177.0f;
                player->actor.world.pos.x = 200.0f;
                this->subCamUp.z = 0.0f;
            }
            break;
        case 75:
            this->unk_339 = 23;
            if (this->unk_398 == 55) {
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonDeadStartAnim, 0.0f);
                this->unk_194 = Si2_anime_end_frame(&gGanonDeadStartAnim);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_98);
                this->unk_39C = 8;
                this->unk_398 = 1000;
            }
            break;
        case 8:
            if (this->unk_398 == 1025) {
                Actor_SE_set(&this->actor, NA_SE_EN_MGANON_STAND);
            }
            if (this->unk_398 >= 1000) {
                if (this->unk_398 < 1040) {
                    this->unk_339 = 23;
                    add_calc0(&play->envCtx.lightBlend, 1.0f, 0.2f);
                }
            }
            if (this->unk_398 == 1040) {
                Actor_SE_set(&this->actor, NA_SE_EN_MGANON_DEAD2);
                this->unk_336 = 2;
                this->unk_339 = 0;
                play->envCtx.prevLightSetting = 0;
                play->envCtx.lightBlend = 0.0f;
            }
            if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gGanonDeadLoopAnim, 0.0f);
                this->unk_398 = 0;
                this->unk_194 = 1000.0f;
            }
            this->subCamEye.x = 250;
            this->subCamEye.y = 1150.0f;
            this->subCamEye.z = 0.0f;
            this->subCamAt.x = this->unk_1B8.x;
            this->subCamAt.y = this->unk_1B8.y;
            this->subCamAt.z = this->unk_1B8.z;
            if ((this->unk_398 < 1000) && ((this->unk_398 % 16) == 0)) {
                Actor_SE_set(&this->actor, NA_SE_EN_MGANON_SWORD);
            }
            if (this->unk_398 == 40) {
                this->unk_39C = 9;
                this->unk_398 = 0;
                zelda->unk_3C8 = 11;
                message_set(play, 0x70D9, NULL);
                this->unk_336 = 0;
                play->envCtx.lightBlend = 0.0f;
            }
            break;
        case 9:
            this->unk_339 = 24;
            this->subCamEye.x = 330.0f;
            this->subCamEye.y = 1120.0f;
            this->subCamEye.z = -150.0f;
            this->subCamAt.x = zelda->actor.world.pos.x;
            this->subCamAt.y = zelda->actor.world.pos.y + 40.0f;
            this->subCamAt.z = zelda->actor.world.pos.z;
            if (this->unk_398 > 60) {
                this->unk_39C = 10;
                this->unk_398 = 0;
                this->unk_410.x = 0.0f;
            }
            break;
        case 10:
            this->unk_339 = 24;
            add_calc2(&this->subCamEye.x, 290.0f, 0.05f, this->unk_410.x);
            add_calc2(&this->subCamEye.y, 1130.0f, 0.05f, this->unk_410.x * 0.25f);
            add_calc2(&this->subCamEye.z, -260.0f, 0.05f, this->unk_410.x * 1.25f);
            if ((this->unk_398 >= 40) && (this->unk_398 <= 110)) {
                add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.02f);
                add_calc2(&this->unk_384, 10.0f, 0.1f, 0.2f);
                Actor_SE_set(&zelda->actor, NA_SE_EV_GOD_LIGHTBALL_2 - SFX_FLAG);
            } else {
                add_calc0(&this->unk_384, 1.0f, 0.2f);
            }
            if (this->unk_398 > 130) {
                add_calc2(&this->subCamAt.y, (zelda->actor.world.pos.y + 40.0f + 10.0f) - 20.0f, 0.1f,
                               this->unk_410.x);
            } else {
                add_calc2(&this->subCamAt.y, zelda->actor.world.pos.y + 40.0f + 10.0f, 0.05f,
                               this->unk_410.x * 0.25f);
            }
            add_calc2(&this->unk_410.x, 1.0f, 1.0f, 0.01f);
            if (this->unk_398 == 10) {
                zelda->unk_3C8 = 12;
            }
            if (this->unk_398 == 110) {
                zelda->unk_3C8 = 13;
            }
            if (this->unk_398 == 140) {
                Actor_SE_set(&zelda->actor, NA_SE_EV_HUMAN_BOUND);
            }
            if (this->unk_398 < 160) {
                break;
            }
            FALLTHROUGH;
        case 20:
            play->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
            z_common_data.nextCutsceneIndex = 0xFFF2;
            play->transitionTrigger = TRANS_TRIGGER_START;
            play->transitionType = TRANS_TYPE_FADE_WHITE;
            play->linkAgeOnLoad = LINK_AGE_CHILD;
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Gama_play_camera_lookat(play, this->subCamId, &this->subCamAt, &this->subCamEye, &this->subCamUp);
    }

    switch (this->unk_1AC) {
        case 0:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->unk_194)) {
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gGanonDownedLoopAnim, 0.0f);
                this->unk_1AC = 1;
            }
            break;
        case 1:
            if ((this->unk_39C < 7) && ((play->gameplayFrames % 32) == 0)) {
                Actor_SE_set(&this->actor, NA_SE_EN_MGANON_BREATH);
            }
            break;
    }
}

void Ganon2_Ken_Check(BossGanon2* this, PlayState* play) {
    Player* player;
    f32 temp_f2;
    f32 temp_f12;
    s16 i;
    s16 j;
    s16 phi_v0_2;

    if (this->unk_316 == 0) {
        for (i = 0; i < ARRAY_COUNT(this->unk_864); i++) {
            if (this->unk_444.elements[i].base.acElemFlags & ACELEM_HIT) {
                this->unk_444.elements[i].base.acElemFlags &= ~ACELEM_HIT;
            } else if (this->unk_444.elements[i].base.atElemFlags & ATELEM_HIT) {
                this->unk_444.elements[i].base.atElemFlags &= ~ATELEM_HIT;

                if (this->unk_312 == 1) {
                    phi_v0_2 = 0x1800;
                } else {
                    phi_v0_2 = 0;
                }

                Actor_player_power_damage_AT_set(play, &this->actor, 15.0f, this->actor.yawTowardsPlayer + phi_v0_2, 2.0f,
                                              0);
                zelda->unk_3C8 = 8;
                this->unk_316 = 10;
                break;
            }
        }
    }

    if (this->unk_324 > 0.0f) {
        s16 pad;

        player = GET_PLAYER(play);
        temp_f2 = -200.0f - player->actor.world.pos.x;
        temp_f12 = -200.0f - player->actor.world.pos.z;

#if OOT_VERSION < PAL_1_0
        if (sqrtf(SQ(temp_f2) + SQ(temp_f12)) > 800.0f)
#else
        if (sqrtf(SQ(temp_f2) + SQ(temp_f12)) > 784.0f)
#endif
        {
            for (j = 0; j < PLAYER_BODYPART_MAX; j++) {
                player->bodyFlameTimers[j] = get_random_timer(0, 200);
            }

            player->bodyIsBurning = true;
            Actor_player_power_damage_AT_set(play, &this->actor, 10.0f, atans_table(temp_f12, temp_f2), 0.0f, 0x10);
            zelda->unk_3C8 = 8;
        }
    }
}

void Ganon2_Damage_Check(BossGanon2* this, PlayState* play) {
    s8 health;
    ColliderElement* acHitElem;
    s16 i;
    u8 phi_v1_2;

    PRINTF("this->no_hit_time %d\n", this->unk_316);
    if (this->unk_316 != 0 || ((this->unk_334 == 0) && (this->actionFunc == mode_damagedemo))) {
        for (i = 0; i < ARRAY_COUNT(this->unk_464); i++) {
            this->unk_424.elements[i].base.acElemFlags &= ~ACELEM_HIT;
        }
    }

    PRINTF("this->look_on %d\n", this->unk_313);
    if (this->unk_313) {
        if (this->actionFunc != mode_mahi) {
            if (this->unk_424.elements[0].base.acElemFlags & ACELEM_HIT) {
                this->unk_424.elements[0].base.acElemFlags &= ~ACELEM_HIT;
                acHitElem = this->unk_424.elements[0].base.acHitElem;
                if ((acHitElem->atDmgInfo.dmgFlags & DMG_ARROW_LIGHT) && (this->actionFunc != mode_damagedemo)) {
                    mode_yahit_init(this, play);
                    Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_HIT_THUNDER);
                    Actor_SE_set(&this->actor, NA_SE_EN_MGANON_DAMAGE);
                    Nai_StopFx(NA_SE_EN_MGANON_UNARI);
                } else if ((this->actionFunc == mode_damagedemo) &&
                           (acHitElem->atDmgInfo.dmgFlags & (DMG_JUMP_MASTER | DMG_SPIN_MASTER | DMG_SLASH_MASTER))) {
                    this->unk_316 = 60;
                    this->unk_342 = 5;
                    Actor_SE_set(&this->actor, NA_SE_EN_MGANON_DAMAGE);
                    Nai_StopFx(NA_SE_EN_MGANON_UNARI);
                    this->actor.colChkInfo.health -= 2;
                    health = this->actor.colChkInfo.health;
                    if (health <= 20 && this->unk_334 == 0) {
                        mode_damagedemo_init(this, play);
                    } else {
                        if (health <= 0) {
                            mode_enddemo_init(this, play);
                        } else {
                            mode_damage_init(this, play);
                        }
                    }
                } else if (this->actionFunc != mode_damagedemo) {
                    mode_mahi_init(this, play);
                    Actor_SE_set(&this->actor, NA_SE_IT_HOOKSHOT_REFLECT);
                }
            }
        }
    } else {
        if (this->unk_424.elements[15].base.acElemFlags & ACELEM_HIT) {
            this->unk_424.elements[15].base.acElemFlags &= ~ACELEM_HIT;
            acHitElem = this->unk_424.elements[15].base.acHitElem;
            this->unk_316 = 60;
            this->unk_344 = 0x32;
            this->unk_342 = 5;
            Actor_SE_set(&this->actor, NA_SE_EN_MGANON_DAMAGE);
            Nai_StopFx(NA_SE_EN_MGANON_UNARI);
            phi_v1_2 = 1;
            if (acHitElem->atDmgInfo.dmgFlags & (DMG_JUMP_MASTER | DMG_SPIN_MASTER | DMG_SLASH_MASTER)) {
                if (acHitElem->atDmgInfo.dmgFlags & DMG_JUMP_MASTER) {
                    phi_v1_2 = 4;
                } else {
                    phi_v1_2 = 2;
                }
            }
            this->actor.colChkInfo.health -= phi_v1_2;
            health = this->actor.colChkInfo.health;
            if ((health <= 20) && (this->unk_334 == 0)) {
                mode_damagedemo_init(this, play);
            } else if ((health <= 0) && (phi_v1_2 >= 2)) {
                mode_enddemo_init(this, play);
            } else {
                if (health <= 0) {
                    this->actor.colChkInfo.health = 1;
                }
                mode_damage_init(this, play);
            }
        }
    }
}

static s16 gn2_eye_pt[] = { 0, 1, 2, 2, 1, 0 };

void Boss_Ganon2_Actor_move(Actor* thisx, PlayState* play) {
    BossGanon2* this = (BossGanon2*)thisx;
    s32 pad;
    s16 i;

    if ((this->unk_337 == 0) || (this->unk_337 == 2)) {
        obj_bank_set(this, play, OBJECT_GANON_ANIME3, false);
    } else {
        obj_bank_set(this, play, OBJECT_GANON2, false);
        add_calc0(&this->unk_30C, 1.0f, 0.5f);
    }
    look_on_set(this);
    this->unk_312 = 0;
    this->unk_19C++;
    Actor_set_scale(&this->actor, 0.01f);
    this->actionFunc(this, play);
    for (i = 0; i < ARRAY_COUNT(this->unk_1A2); i++) {
        if (this->unk_1A2[i] != 0) {
            this->unk_1A2[i]--;
        }
    }
    if (this->unk_316 != 0) {
        this->unk_316--;
    }
    if (this->unk_342 != 0) {
        this->unk_342--;
    }
    if (this->unk_390 != 0) {
        this->unk_390--;
    }
    if (this->unk_392 != 0) {
        this->unk_392--;
    }
    Actor_position_moveF(&this->actor);
    this->actor.shape.rot = this->actor.world.rot;
    if (this->unk_335 != 0) {
        Actor_BGcheck2(play, &this->actor, 60.0f, 60.0f, 100.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            if (this->actor.velocity.y < -5.0f) {
                ActorQuakeRumbleSet(&this->actor, play, 5, 20);
                Na_StartSystemSe_F(NA_SE_IT_BOMB_EXPLOSION);
            }
            this->actor.velocity.y = 0.0f;
        }
    }
    if (((this->unk_19C & 0x1F) == 0) && (fqrand() < 0.3f)) {
        this->unk_318 = 4;
    }
    this->unk_310 = gn2_eye_pt[this->unk_318];
    if (this->unk_318 != 0) {
        this->unk_318--;
    }
    this->unk_1B0 = (sin_s(this->unk_19C * 0x2AAA) * 64.0f) + 191.0f;
    if (this->unk_344 != 0) {
        this->unk_344--;
        add_calc2(&this->unk_360.x, 5000.0f, 0.5f, 3000.0f);
        add_calc2(&this->unk_370.x, 5500.0f, 0.5f, 3000.0f);
        add_calc2(&this->unk_360.z, 8000.0f, 0.1f, 4000.0f);
        add_calc2(&this->unk_370.z, 8000.0f, 0.1f, 4000.0f);
        adds(&this->unk_346, 0xFA0, 0xA, 0x7D0);
    } else {
        this->unk_360.y = 14000.0f;
        add_calc2(&this->unk_360.x, 2000.0f, 0.1f, 100.0f);
        this->unk_370.y = 12000.0f;
        add_calc2(&this->unk_370.x, 1500.0f, 0.1f, 100.0f);
        if ((this->actionFunc == mode_wait) || (this->actionFunc == mode_mahi) ||
            (this->actionFunc == mode_yahit)) {
            add_calc2(&this->unk_360.z, 1000.0f, 0.1f, 100.0f);
            add_calc2(&this->unk_370.z, 1000.0f, 0.1f, 100.0f);
            adds(&this->unk_346, -0xFA0, 0xA, 0x64);
        } else {
            add_calc2(&this->unk_360.z, 5000.0f, 0.1f, 200.0f);
            add_calc2(&this->unk_370.z, 5000.0f, 0.1f, 200.0f);
            adds(&this->unk_346, 0, 0xA, 0x64);
        }
    }
    if (this->unk_39C != 75) {
        this->unk_35C += this->unk_360.x;
        this->unk_36C += this->unk_370.x;
    }
    if (this->unk_337 == 2) {
        this->unk_370.z = 0.0f;
        this->unk_360.z = 0.0f;
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_348); i++) {
        f32 phi_f2;

        if (i == 0) {
            phi_f2 = 0.2f;
        } else if (i == 1) {
            phi_f2 = 0.5f;
        } else {
            phi_f2 = 1.0f;
        }

        this->unk_348[i] = sin_s(((s16)this->unk_35C + (i * (s16)this->unk_360.y))) * phi_f2 * this->unk_360.z;
        this->unk_352[i] = sin_s(((s16)this->unk_36C + (i * (s16)this->unk_370.y))) * phi_f2 * this->unk_370.z;
    }

    Ganon2_ken_gareki_check(this, play);
    Ganon2_Ken_Check(this, play);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->unk_424.base);
    if (this->actionFunc != mode_enddemo) {
        Ganon2_Damage_Check(this, play);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->unk_424.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->unk_444.base);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->unk_444.base);
        if (this->subCamId == SUB_CAM_ID_DONE) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->unk_444.base);
        }
    }
    if ((this->unk_332 == 0) && (this->unk_336 != 0)) {
        if (this->unk_336 == 2) {
            this->unk_332 = (s16)rnd_f(30.0f) + 8;
        } else {
            this->unk_332 = (s16)rnd_f(60.0f) + 0xA;
        }
        this->unk_339 = 0;
        play->envCtx.prevLightSetting = 0;
        play->envCtx.lightSetting = (s8)rnd_f(1.9f) + 1;
        play->envCtx.lightBlend = 1.0f;
        inazuma_se_pos.y = 0.0f;
        inazuma_se_pos.x = inazuma_se_pos.y;
        inazuma_se_pos.z = inazuma_se_pos.x;
        if (fqrand() < 0.5f) {
            inazuma_se_pos.z = rnd_f(1000.0f);
        }
        Na_StartObjectSe_F(&inazuma_se_pos, NA_SE_EV_LIGHTNING);
        this->unk_328 = 0xFF;
        this->unk_330 = 5;
        this->unk_32C = 0.0f;
        this->unk_340 = (s16)rnd_f(10000.0f);
    } else if (this->unk_332 != 0) {
        this->unk_332--;
    }
    if ((play->envCtx.lightBlend > 0.0f) && (this->unk_336 != 0)) {
        play->envCtx.customSkyboxFilter = 1;
        play->envCtx.skyboxFilterColor[0] = 255;
        play->envCtx.skyboxFilterColor[1] = 255;
        play->envCtx.skyboxFilterColor[2] = 255;
        play->envCtx.skyboxFilterColor[3] = (s16)(play->envCtx.lightBlend * 200.0f);
    } else {
        play->envCtx.customSkyboxFilter = 0;
    }
    play->envCtx.lightSettingOverride = 0;
    play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_FULL_CONTROL;

    switch (this->unk_339) {
        case 0:
            add_calc0(&play->envCtx.lightBlend, 1.0f, 0.1f);
            break;
        case 3:
            play->envCtx.prevLightSetting = 3;
            play->envCtx.lightSetting = 4;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.0125f);
            break;
        case 4:
            play->envCtx.prevLightSetting = 5;
            play->envCtx.lightSetting = 6;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.0125f);
            break;
        case 5:
            play->envCtx.prevLightSetting = 6;
            play->envCtx.lightSetting = 7;
            add_calc2(&this->unk_33C, 0.69f, 1.0f, 0.05f);
            play->envCtx.lightBlend = (sin_s(play->gameplayFrames * 0x5000) * 0.15f) + (0.15f + this->unk_33C);
            break;
        case 55:
            play->envCtx.prevLightSetting = 2;
            play->envCtx.lightSetting = 0;
            add_calc0(&play->envCtx.lightBlend, 1.0f, 0.05f);
            break;
        case 6:
            play->envCtx.prevLightSetting = 2;
            play->envCtx.lightSetting = 8;
            add_calc2(&this->unk_33C, 0.69f, 1.0f, 0.05f);
            play->envCtx.lightBlend = (sin_s(play->gameplayFrames * 0x7000) * 0.15f) + (0.15f + this->unk_33C);
            break;
        case 7:
            play->envCtx.prevLightSetting = 0;
            play->envCtx.lightSetting = 8;
            add_calc0(&play->envCtx.lightBlend, 1.0f, 0.02f);
            break;
        case 20:
            play->envCtx.prevLightSetting = 0;
            play->envCtx.lightSetting = 9;
            break;
        case 21:
            play->envCtx.prevLightSetting = 10;
            play->envCtx.lightSetting = 9;
            break;
        case 22:
            play->envCtx.prevLightSetting = 10;
            play->envCtx.lightSetting = 11;
            break;
        case 23:
            play->envCtx.prevLightSetting = 9;
            play->envCtx.lightSetting = 11;
            break;
        case 24:
            play->envCtx.prevLightSetting = 0;
            play->envCtx.lightSetting = 12;
            break;
        case -1:
            break;
    }

    if (this->unk_339 >= 0) {
        this->unk_339 = 0;
    }
    if (gareki_break != 0) {
        u16 i2;
        Vec3f sp58;
        Vec3f sp4C;
        f32 angle;
        f32 sp44;

        gareki_break = 0;

        for (i2 = 0; i2 < 100; i2++) {
            angle = rnd_f(2 * M_PI);
            sp44 = rnd_f(40.0f) + 10.0f;
            sp58 = this->actor.world.pos;
            sp58.y = 1200.0f;
            sp4C.x = cosf(angle) * sp44;
            sp4C.z = sinf(angle) * sp44;
            sp4C.y = rnd_f(15.0f) + 15.0f;
            sp58.x += sp4C.x * 10.0f * 0.1f;
            sp58.z += sp4C.z * 10.0f * 0.1f;
            gn2_gareki_ct(play, &sp58, &sp4C, rnd_f(0.3f) + 0.2f);
        }
    }
    this->unk_388 += 0.15f;
    Gn2_Eff_move(this, play);
}

static s16 blur_al = 0;

static u8 vtx_index_1[] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
static u8 vtx_index_2[] = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21 };

void blur_calc(Vec3f* arg0, Vec3f* arg1) {
    Vtx* vtx;
    Vec3f sp2D0;
    s16 temp_s1;
    s16 temp_a1;
    s16 sp2CA;
    s16 sp2C8;
    s16 i;
    u8 phi_s2;
    u8 temp_s4;
    u8 temp_s4_2;
    f32 temp_f12;
    Vec3f temp_f20;
    Vec3f temp_f2;
    Vec3f temp_f22;
    f32 sp294;
    f32 phi_f30;
    f32 temp_f28;
    f32 temp_f26;
    s32 pad[3];
    Vec3f sp18C[20];
    Vec3f sp9C[20];

    for (i = 0; i < 20; i++) {
        sp18C[i] = *arg0;
        sp9C[i] = *arg1;
    }

    temp_s4 = 0;

    blur_base_1[3] = blur_base_1[2];
    blur_base_1[2] = blur_base_1[1];
    blur_base_1[1] = blur_base_1[0];
    blur_base_1[0] = *arg0;

    sp2D0 = blur_base_1[0];

    temp_f20.x = blur_base_1[1].x - sp2D0.x;
    temp_f20.y = blur_base_1[1].y - sp2D0.y;
    temp_f20.z = blur_base_1[1].z - sp2D0.z;

    sp2CA = atans_table(temp_f20.z, temp_f20.x);
    sp2C8 = atans_table(sqrtf(SQXZ(temp_f20)), temp_f20.y);

    temp_f2.x = blur_base_1[2].x - blur_base_1[1].x;
    temp_f2.y = blur_base_1[2].y - blur_base_1[1].y;
    temp_f2.z = blur_base_1[2].z - blur_base_1[1].z;

    temp_f22.x = blur_base_1[3].x - blur_base_1[2].x;
    temp_f22.y = blur_base_1[3].y - blur_base_1[2].y;
    temp_f22.z = blur_base_1[3].z - blur_base_1[2].z;

    temp_f12 = sqrtf(SQXYZ(temp_f20)) + sqrtf(SQXYZ(temp_f2)) + sqrtf(SQXYZ(temp_f22));
    if (temp_f12 <= 1.0f) {
        temp_f12 = 1.0f;
    }

    temp_f28 = temp_f12 * 0.083f;
    phi_f30 = sqrtf(SQXYZ(temp_f20)) / 2.0f;
    sp294 = sqrtf(SQXYZ(temp_f2)) / 2.0f;

    phi_s2 = 1;

    while (true) {
        temp_f20.x = blur_base_1[phi_s2].x - sp2D0.x;
        temp_f20.y = blur_base_1[phi_s2].y - sp2D0.y;
        temp_f20.z = blur_base_1[phi_s2].z - sp2D0.z;

        temp_s1 = atans_table(temp_f20.z, temp_f20.x);
        temp_a1 = atans_table(sqrtf(SQXZ(temp_f20)), temp_f20.y);

        adds(&sp2C8, temp_a1, 1, 0x1000);
        adds(&sp2CA, temp_s1, 1, 0x1000);

        temp_f26 = temp_f28 * cos_s(sp2C8);

        sp18C[temp_s4] = sp2D0;

        sp2D0.x += temp_f26 * sin_s(sp2CA);
        sp2D0.y += temp_f28 * sin_s(sp2C8);
        sp2D0.z += temp_f26 * cos_s(sp2CA);

        temp_f20.x = blur_base_1[phi_s2].x - sp2D0.x;
        temp_f20.y = blur_base_1[phi_s2].y - sp2D0.y;
        temp_f20.z = blur_base_1[phi_s2].z - sp2D0.z;

        if (phi_s2 < 3) {
            if (sqrtf(SQXYZ(temp_f20)) <= phi_f30) {
                phi_f30 = sp294;
                phi_s2++;
            }
        } else {
            if (sqrtf(SQXYZ(temp_f20)) <= (temp_f28 + 1.0f)) {
                phi_s2++;
            }
        }

        temp_s4++;

        if ((temp_s4 >= 20) || (phi_s2 >= 4)) {
            break;
        }
    }

    temp_s4_2 = 0;

    blur_base_2[3] = blur_base_2[2];
    blur_base_2[2] = blur_base_2[1];
    blur_base_2[1] = blur_base_2[0];
    blur_base_2[0] = *arg1;

    sp2D0 = blur_base_2[0];

    temp_f20.x = blur_base_2[1].x - sp2D0.x;
    temp_f20.y = blur_base_2[1].y - sp2D0.y;
    temp_f20.z = blur_base_2[1].z - sp2D0.z;

    sp2CA = atans_table(temp_f20.z, temp_f20.x);
    sp2C8 = atans_table(sqrtf(SQXZ(temp_f20)), temp_f20.y);

    temp_f2.x = blur_base_2[2].x - blur_base_2[1].x;
    temp_f2.y = blur_base_2[2].y - blur_base_2[1].y;
    temp_f2.z = blur_base_2[2].z - blur_base_2[1].z;

    temp_f22.x = blur_base_2[3].x - blur_base_2[2].x;
    temp_f22.y = blur_base_2[3].y - blur_base_2[2].y;
    temp_f22.z = blur_base_2[3].z - blur_base_2[2].z;

    temp_f12 = sqrtf(SQXYZ(temp_f20)) + sqrtf(SQXYZ(temp_f2)) + sqrtf(SQXYZ(temp_f22));
    if (temp_f12 <= 1.0f) {
        temp_f12 = 1.0f;
    }

    temp_f28 = temp_f12 * 0.083f;
    phi_f30 = sqrtf(SQXYZ(temp_f20)) / 2.0f;
    sp294 = sqrtf(SQXYZ(temp_f2)) / 2.0f;

    phi_s2 = 1;

    while (true) {
        temp_f20.x = blur_base_2[phi_s2].x - sp2D0.x;
        temp_f20.y = blur_base_2[phi_s2].y - sp2D0.y;
        temp_f20.z = blur_base_2[phi_s2].z - sp2D0.z;

        temp_s1 = atans_table(temp_f20.z, temp_f20.x);
        temp_a1 = atans_table(sqrtf(SQXZ(temp_f20)), temp_f20.y);

        adds(&sp2C8, temp_a1, 1, 0x1000);
        adds(&sp2CA, temp_s1, 1, 0x1000);

        temp_f26 = temp_f28 * cos_s(sp2C8);

        sp9C[temp_s4_2] = sp2D0;

        sp2D0.x += temp_f26 * sin_s(sp2CA);
        sp2D0.y += temp_f28 * sin_s(sp2C8);
        sp2D0.z += temp_f26 * cos_s(sp2CA);

        temp_f20.x = blur_base_2[phi_s2].x - sp2D0.x;
        temp_f20.y = blur_base_2[phi_s2].y - sp2D0.y;
        temp_f20.z = blur_base_2[phi_s2].z - sp2D0.z;

        if (phi_s2 < 3) {
            if (sqrtf(SQXYZ(temp_f20)) <= phi_f30) {
                phi_f30 = sp294;
                phi_s2++;
            }
        } else {
            if (sqrtf(SQXYZ(temp_f20)) <= (temp_f28 + 1.0f)) {
                phi_s2++;
            }
        }

        temp_s4_2++;

        if ((temp_s4_2 >= 20) || (phi_s2 >= 4)) {
            break;
        }
    }

    vtx = SEGMENTED_TO_VIRTUAL(gn2_kenblur_v);
    for (i = 0; i < 11; i++) {
        if ((temp_s4 - i) > 0) {
            vtx[vtx_index_1[i]].n.ob[0] = sp18C[temp_s4 - i - 1].x;
            vtx[vtx_index_1[i]].n.ob[1] = sp18C[temp_s4 - i - 1].y;
            vtx[vtx_index_1[i]].n.ob[2] = sp18C[temp_s4 - i - 1].z;
        }
        if ((temp_s4_2 - i) > 0) {
            vtx[vtx_index_2[i]].n.ob[0] = sp9C[temp_s4_2 - i - 1].x;
            vtx[vtx_index_2[i]].n.ob[1] = sp9C[temp_s4_2 - i - 1].y;
            vtx[vtx_index_2[i]].n.ob[2] = sp9C[temp_s4_2 - i - 1].z;
        }
    }
}

void kenblur_draw(BossGanon2* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 5083);

    if (this->unk_312 != 0) {
        blur_calc(&this->unk_200, &this->unk_20C);
        blur_al = 0xFF;
    }

    if (blur_disp >= 4) {
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 32, 1, play->gameplayFrames * 18, 0,
                                    32, 32));
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, blur_al);
        Matrix_translate(0.0f, 0.0f, 0.0f, MTXMODE_NEW);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_ganon2.c", 5117);
        gSPDisplayList(POLY_XLU_DISP++, gn2_kenblur_modelT);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 5122);
}

void fire_wall_disp(BossGanon2* this, PlayState* play) {
    GameState* gameState = &play->state;

    if (this->unk_324 > 0.0f) {
        OPEN_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5131);

        Matrix_push();
        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(gameState->gfxCtx, G_TX_RENDERTILE, (s32)play->gameplayFrames, 0, 32, 64, 1,
                                    -play->gameplayFrames * 2, -play->gameplayFrames * 8, 32, 32));
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 200, 0, (s8)this->unk_324);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 128);
        Matrix_translate(-200.0f, 1086.0f, -200.0f, MTXMODE_NEW);
#if OOT_VERSION < PAL_1_0
        Matrix_scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
#else
        Matrix_scale(0.098000005f, 0.1f, 0.098000005f, MTXMODE_APPLY);
#endif
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gameState->gfxCtx, "../z_boss_ganon2.c", 5183);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(ganon_fire_modelT));
        Matrix_pull();

        CLOSE_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5186);
    }
}

void inazuma_disp(BossGanon2* this, PlayState* play) {
    GameState* gameState = &play->state;
    s16 i;
    f32 sin;
    f32 cos;
    f32 rand;

    OPEN_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5196);
    Matrix_push();

    if ((this->unk_330 != 0) || (this->unk_328 != 0)) {
        if (this->unk_330 != 0) {
            this->unk_330--;
        } else {
            this->unk_328 -= 70;

            if (this->unk_328 < 0) {
                this->unk_328 = 0;
            }
        }

        add_calc2(&this->unk_32C, 0.13f, 1.0f, 0.065f);
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, this->unk_328);
        init_stage_rnd(this->unk_340 + 1, 0x71AC - this->unk_340, 0x263A);
        rand = stage_rnd() * M_PI;

        for (i = 0; i < 5; i++) {
            sin = 5000.0f * sinf((i * (2 * M_PI / 5)) + rand);
            cos = 5000.0f * cosf((i * (2 * M_PI / 5)) + rand);
            Matrix_translate(-200.0f + sin, 4786.0f, -200.0f + cos, MTXMODE_NEW);
            Matrix_scale(this->unk_32C, this->unk_32C, this->unk_32C, MTXMODE_APPLY);
            Matrix_rotateY((i * (2 * M_PI / 5)) + rand, MTXMODE_APPLY);
            Matrix_rotateZ((stage_rnd() - 0.5f) * 100.0f * 0.01f, MTXMODE_APPLY);

            if (stage_rnd() < 0.5f) {
                Matrix_rotateY(M_PI, MTXMODE_APPLY);
            }

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gameState->gfxCtx, "../z_boss_ganon2.c", 5250);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gn2_inazuma_modelT));
        }
    }

    Matrix_pull();
    CLOSE_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5255);
}

static void tri_force_disp(BossGanon2* this, PlayState* play) {
    GameState* gameState = &play->state;

    if (this->unk_1B4 > 0.0f) {
        OPEN_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5264);

        Matrix_push();
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, (s16)this->unk_1B4);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 200, 0, 128);
        Matrix_translate(this->unk_1B8.x, this->unk_1B8.y, this->unk_1B8.z, MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_rotateZ(-0.2f, MTXMODE_APPLY);
        Matrix_scale(0.6f, 0.6f, 1.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gameState->gfxCtx, "../z_boss_ganon2.c", 5290);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gn2_demo_ec_tfc_modelT));
        Matrix_pull();

        CLOSE_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5293);
    }
}

s32 Boss_Ganon2_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    s32 pad;
    BossGanon2* this = (BossGanon2*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 5355);

    if (limbIndex == GANON_LIMB_NECK) {
        rot->y += this->unk_31A;
        rot->z += this->unk_31C;
    }

    if (limbIndex >= GANON_LIMB_TAIL1) {
        rot->x += this->unk_348[limbIndex - GANON_LIMB_TAIL1] + this->unk_346;
        rot->y += this->unk_352[limbIndex - GANON_LIMB_TAIL1];

        if (this->unk_342 & 1) {
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 0, 0, 255);
        } else {
            gDPSetEnvColor(POLY_OPA_DISP++, (s16)this->unk_1B0, (s16)this->unk_1B0, (s16)(*this).unk_1B0, 255);
        }
    }

    if ((limbIndex == GANON_LIMB_LEFT_SWORD) || (limbIndex == GANON_LIMB_RIGHT_SWORD) ||
        (limbIndex == GANON_LIMB_LEFT_HORN) || (limbIndex == GANON_LIMB_RIGHT_HORN)) {
        *dList = NULL;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 5431);
    return 0;
}

static s8 ganon2_coll_index[] = {
    0xFF, 0xFF, 0x01, 0xFF, 0x03, 0x04, 0xFF, 0xFF, 0x05, 0xFF, 0x06, 0x07, 0xFF, 0xFF, 0x08, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0xFF, 0xFF, 0x02, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0xFF, 0xFF, 0xFF, 0x0F, 0xFF, 0x00,
};

static s8 shape_set_id2[] = {
    0xFF, 0xFF, 0x01, 0xFF, 0x03, 0x04, 0xFF, 0xFF, 0x05, 0xFF, 0x06, 0x07, 0xFF, 0xFF, 0x08, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0xFF, 0xFF, 0x02, 0x0C, 0x0D, 0x0E, 0x09, 0x0A, 0x0B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
};

static Vec3f ken_pos = { 0.0f, 0.0f, 5000.0f };
static Vec3f kata_Lp = { 0.0f, 2000.0f, 0.0f };
static Vec3f kata_Rp = { 0.0f, 2000.0f, 0.0f };
static Vec3f blur_pos_1 = { 0.0f, 0.0f, 17000.0f };
static Vec3f blur_pos_2 = { 0.0f, 0.0f, 3000.0f };
static Vec3f tail_p = { 0.0f, 0.0f, 0.0f };

void Boss_Ganon2_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    s8 pad;
    s8 temp_v0;
    BossGanon2* this = (BossGanon2*)thisx;
    Vec3f sp4C;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 5459);

    blur_pos_1.z = 17000.0f;
    blur_pos_2.z = 3000.0f;

    if (shape_set_id2[limbIndex] >= 0) {
        Matrix_Position(&zero, &this->unk_234[shape_set_id2[limbIndex]]);
    }

    if (limbIndex == GANON_LIMB_NECK) {
        Matrix_Position(&zero, &this->unk_1B8);
    } else if (limbIndex == GANON_LIMB_LEFT_SHOULDER) {
        Matrix_Position(&kata_Lp, &this->unk_1F4);
    } else if (limbIndex == GANON_LIMB_RIGHT_SHOULDER) {
        Matrix_Position(&kata_Rp, &this->unk_1E8);
    } else if (limbIndex == GANON_LIMB_LEFT_FOOT) {
        Matrix_Position(&zero, &this->unk_1DC);
    } else if (limbIndex == GANON_LIMB_RIGHT_FOOT) {
        Matrix_Position(&zero, &this->unk_1D0);
    } else if (limbIndex == GANON_LIMB_TAIL4) {
        Matrix_Position(&tail_p, &this->unk_1C4);
    }

    temp_v0 = ganon2_coll_index[limbIndex];
    if (temp_v0 >= 0) {
        Matrix_Position(&zero, &sp4C);
        CollisionCheck_pos_set(temp_v0, &this->unk_424, &sp4C);
    }

    if ((limbIndex == GANON_LIMB_LEFT_SWORD) || (limbIndex == GANON_LIMB_RIGHT_SWORD)) {
        Matrix_push();
        Matrix_scale(this->unk_224, this->unk_224, this->unk_224, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_boss_ganon2.c", 5522);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_pull();
    } else if ((limbIndex == GANON_LIMB_LEFT_HORN) || (limbIndex == GANON_LIMB_RIGHT_HORN)) {
        Matrix_push();
        Matrix_scale(this->unk_228, this->unk_228, this->unk_228, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_boss_ganon2.c", 5533);
        gSPDisplayList(POLY_OPA_DISP++, *dList);
        Matrix_pull();
    }

    if (*dList != NULL) {
        if ((limbIndex == GANON_LIMB_LEFT_SWORD) && (this->unk_312 == 1)) {
            Matrix_Position(&ken_pos, &this->unk_218);
            CollisionCheck_pos_set(0, &this->unk_444, &this->unk_218);
            Matrix_Position(&blur_pos_1, &this->unk_200);
            Matrix_Position(&blur_pos_2, &this->unk_20C);
        } else if ((limbIndex == GANON_LIMB_RIGHT_SWORD) && (this->unk_312 == 2)) {
            Matrix_Position(&ken_pos, &this->unk_218);
            CollisionCheck_pos_set(1, &this->unk_444, &this->unk_218);
            Matrix_Position(&blur_pos_1, &this->unk_200);
            Matrix_Position(&blur_pos_2, &this->unk_20C);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 5566);
}

static void body_flash_disp(BossGanon2* this, PlayState* play) {
    GameState* gameState = &play->state;
    s16 i;

    OPEN_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5575);

    if (this->unk_30C > 0.0f) {
        _texture_z_light_fog_prim_xlu(gameState->gfxCtx);
        if (this->unk_380 > 0.0f) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 200, 0, 0);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 100, 255, 255, 0);
        }
        gSPDisplayList(POLY_XLU_DISP++, efc_fg2_thunder1_DISP_MODE);

        for (i = 0; i < 15; i++) {
            Matrix_translate(this->unk_234[i].x, this->unk_234[i].y, this->unk_234[i].z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(this->unk_30C, this->unk_30C, this->unk_30C, MTXMODE_APPLY);
            Matrix_rotateZ(rnd_fx(M_PI), MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gameState->gfxCtx, "../z_boss_ganon2.c", 5618);
            gSPDisplayList(POLY_XLU_DISP++, efc_fg2_thunder1_modelT);
        }
    }

    CLOSE_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5622);
}

void zelda_flash_disp(BossGanon2* this, PlayState* play) {
    GameState* gameState = &play->state;

    OPEN_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5632);

    if (this->unk_384 > 0.0f) {
        _texture_z_light_fog_prim_xlu(gameState->gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 200);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 200, 0, 0);
        gSPDisplayList(POLY_XLU_DISP++, efc_fg2_thunder1_DISP_MODE);
        Matrix_translate(zelda->actor.world.pos.x, zelda->actor.world.pos.y + 80.0f, zelda->actor.world.pos.z,
                         MTXMODE_NEW);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        Matrix_scale(this->unk_384, this->unk_384, this->unk_384, MTXMODE_APPLY);
        Matrix_rotateZ(this->unk_388, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gameState->gfxCtx, "../z_boss_ganon2.c", 5661);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(efc_fg2_thunder1_modelT));
        Matrix_rotateZ(this->unk_388 * -2.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gameState->gfxCtx, "../z_boss_ganon2.c", 5664);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(efc_fg2_thunder1_modelT));
    }

    CLOSE_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5667);
}

void link_sword_flash_disp(BossGanon2* this, PlayState* play) {
    GameState* gameState = &play->state;
    f32 phi_f20;

    OPEN_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5675);

    if (this->unk_38C > 0.0f) {
        s8 i;
        Player* player;

        player = GET_PLAYER(play);
        _texture_z_light_fog_prim_xlu(gameState->gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s16)this->unk_38C);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 255, 255, 0);
        gSPDisplayList(POLY_XLU_DISP++, efc_fg2_thunder1_DISP_MODE);

        for (i = 0; i < 11; i++) {
            Matrix_mult(&player->mf_9E0, MTXMODE_NEW);
            Matrix_translate((i * 250.0f) + 900.0f, 350.0f, 0.0f, MTXMODE_APPLY);

            if (i < 7) {
                phi_f20 = 1.0f;
            } else {
                phi_f20 = 1.0f - ((i - 7) * 0.2333333f); // 7 / 30
            }

            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(200.0f * phi_f20, 200.0f * phi_f20, 1.0f, MTXMODE_APPLY);
            Matrix_rotateZ(rnd_f(2.0f * M_PI), MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gameState->gfxCtx, "../z_boss_ganon2.c", 5721);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(efc_fg2_thunder1_modelT));
        }
    }

    CLOSE_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5725);
}

static s8 shape_set_id[] = {
    0xFF, 0xFF, 0x01, 0xFF, 0x03, 0x04, 0x05, 0xFF, 0x06, 0x07, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0x02, 0x0C, 0x0D, 0x0E, 0x09, 0x0A, 0x0B, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
};

static Vec3f r_hand_t_p = { 800.0f, 420.0f, 100.0f };

static void Boss_Ganon_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    s8 temp_v1 = shape_set_id[limbIndex];
    BossGanon2* this = (BossGanon2*)thisx;

    if (temp_v1 >= 0) {
        Matrix_Position(&zero, &this->unk_234[temp_v1]);
    }
    if (limbIndex == GANONDORF_LIMB_JEWEL) {
        OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 5749);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_ganon2.c", 5752);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGanondorfEyesDL));

        CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 5754);
    } else if (limbIndex == GANONDORF_LIMB_RIGHT_HAND) {
        Matrix_Position(&r_hand_t_p, &this->unk_1B8);
    }
}

void zelda_hadou_disp(BossGanon2* this, PlayState* play) {
    GameState* gameState = &play->state;

    if (this->unk_380 > 0.0f) {
        OPEN_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5772);

        Matrix_push();
        gDPPipeSync(POLY_XLU_DISP++);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(gameState->gfxCtx, G_TX_RENDERTILE, this->unk_19C * -8, 0, 32, 64, 1,
                                    this->unk_19C * -4, this->unk_19C * -8, 32, 32));
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 170, (s16)this->unk_37C);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 200, 0, 128);
        Matrix_translate(zelda->actor.world.pos.x + 100.0f, zelda->actor.world.pos.y + 35.0f + 7.0f,
                         zelda->actor.world.pos.z - 100.0f, MTXMODE_NEW);
        Matrix_rotateY(-M_PI / 4.0f, MTXMODE_APPLY);
        Matrix_scale(0.040000003f, 0.040000003f, this->unk_380, MTXMODE_APPLY);
        Matrix_rotateX(M_PI / 2.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gameState->gfxCtx, "../z_boss_ganon2.c", 5814);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(efc_ganon2_hadou_modelT));
        Matrix_pull();

        CLOSE_DISPS(gameState->gfxCtx, "../z_boss_ganon2.c", 5817);
    }
}

static void* gn2_eye_tex_no[] = {
    gGanonEyeOpenTex,
    gGanonEyeHalfTex,
    gGanonEyeClosedTex,
};

static Vec3f pos = { 0.0f, -2000.0f, 0.0f };

void Boss_Ganon2_Actor_draw(Actor* thisx, PlayState* play) {
    void* shadowTexture = GRAPH_ALLOC(play->state.gfxCtx, 4096);
    BossGanon2* this = (BossGanon2*)thisx;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 5840);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    switch (this->unk_337) {
        case 0:
            obj_bank_set(this, play, OBJECT_GANON, true);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gGanondorfEmptyEyeTex));
            gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gGanondorfEmptyEyeTex));
            Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  this->skelAnime.dListCount, NULL, Boss_Ganon_draw_sub2, this);
            break;
        case 1:
        case 2:
            obj_bank_set(this, play, OBJECT_GANON2, true);
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gn2_eye_tex_no[this->unk_310]));
            CollisionCheck_pos_set(0, &this->unk_444, &pos);
            CollisionCheck_pos_set(1, &this->unk_444, &pos);
            this->unk_218 = pos;
            if (this->unk_342 & 1) {
                POLY_OPA_DISP = gfx_set_fog_nosync(POLY_OPA_DISP, 0xFF, 0, 0, 0xFF, 0x384, 0x44B);
            }
            Matrix_translate(0.0f, -4000.0f, 4000.0f, MTXMODE_APPLY);
            Matrix_rotateX(this->unk_394, MTXMODE_APPLY);
            Matrix_translate(0.0f, 4000.0f, -4000.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_boss_ganon2.c", 5910);
            Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  this->skelAnime.dListCount, Boss_Ganon2_draw_sub, Boss_Ganon2_draw_sub2,
                                  this);
            POLY_OPA_DISP = game_play_set_fog(play, POLY_OPA_DISP);
            shadow_cont(shadowTexture, this, play);
            shadow_disp(shadowTexture, this, play);
            break;
    }

    obj_bank_set(this, play, OBJECT_GANON2, true);
    inazuma_disp(this, play);
    fire_wall_disp(this, play);
    body_flash_disp(this, play);
    tri_force_disp(this, play);
    zelda_hadou_disp(this, play);
    zelda_flash_disp(this, play);
    link_sword_flash_disp(this, play);

    if ((this->unk_312 != 0) || (blur_al != 0)) {
        kenblur_draw(this, play);
        if (this->unk_312 == 0) {
            s32 pad;

            blur_al -= 40;
            if (blur_al <= 0) {
                blur_al = 0;
            }
        }

        blur_disp++;
    } else {
        for (i = 0; i < 3; i++) {
            blur_base_1[i] = this->unk_200;
            blur_base_2[i] = this->unk_20C;
        }

        blur_disp = 0;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 5983);

    Gn2_Eff_disp(play);
}

void Gn2_Eff_move(BossGanon2* this, PlayState* play) {
    s32 pad[4];
    Player* player = GET_PLAYER(play);
    BossGanon2Effect* effect = play->specialEffects;
    s16 i;

    for (i = 0; i < BOSS_GANON2_EFFECT_COUNT; i++, effect++) {
        if (effect->type != 0) {
            Vec3f sp78;

            effect->position.x += effect->velocity.x;
            effect->position.y += effect->velocity.y;
            effect->position.z += effect->velocity.z;
            effect->unk_01++;
            effect->velocity.x += effect->accel.x;
            effect->velocity.y += effect->accel.y;
            effect->velocity.z += effect->accel.z;
            if (effect->type == 1) {
                s32 pad2;

                if (effect->unk_2E == 0) {
                    effect->unk_38.z += 1.0f;
                    effect->unk_38.y = (2.0f * M_PI) / 5.0f;
                } else {
                    effect->unk_38.z = M_PI / 2.0f;
                    effect->unk_38.y = 0.0f;
                    if (effect->position.y <= 1098.0f) {
                        effect->position.y = 1098.0f;
                        if (effect->velocity.y < -10.0f) {
                            sp78 = effect->position;
                            sp78.y = 1086.0f;
                            Na_StartSystemSe_F(NA_SE_IT_SHIELD_REFLECT_SW);
                            CollisionCheckSetSparkFlashBlue(play, &sp78);
                        }
                        effect->velocity.y = 0.0f;
                    }
                    if ((SQ(player->actor.world.pos.x - effect->position.x) +
                         SQ(player->actor.world.pos.z - effect->position.z)) < SQ(25.0f)) {
                        effect->type = 0;
                        this->unk_39C = 10;
                    }
                }
            } else if (effect->type == 2) {
                effect->unk_38.x += 0.1f;
                effect->unk_38.y += 0.4f;
                if ((sqrtf(SQ(-200.0f - effect->position.x) + SQ(-200.0f - effect->position.z)) < 1000.0f)) {
                    if (effect->position.y < 1186.0f) {
                        if (effect->unk_2E == 0) {
                            effect->unk_2E++;
                            effect->position.y = 1186.0f;
                            effect->velocity.x *= 0.75f;
                            effect->velocity.z *= 0.75f;
                            effect->velocity.y *= -0.2f;
                        } else {
                            effect->type = 0;
                        }
                    }
                } else if (effect->position.y < 0.0f) {
                    effect->type = 0;
                }
            }
        }
    }
}

void Gn2_Eff_disp(PlayState* play) {
    s16 alpha;
    u8 objectFlag = 0;
    BossGanon2Effect* effect;
    s16 i;
    BossGanon2Effect* effects;

    effects = effect = play->specialEffects;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 6086);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    for (i = 0; i < 1; i++) {
        if (effect->type == 1) {
            Vec3f spA0;
            f32 temp_f0;
            f32 angle;
            s32 pad;

            _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
            spA0.x = play->envCtx.dirLight1.params.dir.x;
            spA0.y = play->envCtx.dirLight1.params.dir.y;
            spA0.z = play->envCtx.dirLight1.params.dir.z;
            HiliteReflect_init(&effect->position, &play->view.eye, &spA0, play->state.gfxCtx);
            Matrix_translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_scale(0.03f, 0.03f, 0.03f, MTXMODE_APPLY);
            Matrix_rotateY(effect->unk_38.z, MTXMODE_APPLY);
            Matrix_rotateX(effect->unk_38.y, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_boss_ganon2.c", 6116);
            gSPSegment(POLY_OPA_DISP++, 0x08,
                       tex_scroll2(play->state.gfxCtx, 0, 0 - (play->gameplayFrames & 0x7F), 32, 32));
            gSPDisplayList(POLY_OPA_DISP++, master_gn2_swordB_model);

            if ((play->envCtx.lightSetting == 1) || (play->envCtx.lightSetting == 2)) {
                alpha = (s16)(play->envCtx.lightBlend * 150.0f) + 50;
                angle = M_PI / 5.0f;
            } else {
                alpha = 100;
                angle = M_PI / 2.0f;
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, alpha);
            temp_f0 = effect->position.y - 1098.0f;
            Matrix_translate(effect->position.x + temp_f0, 1086.0f, (effect->position.z - 1.0f) + temp_f0, MTXMODE_NEW);
            Matrix_rotateY(angle, MTXMODE_APPLY);
            Matrix_scale(1.0f, 0.0f, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_ganon2.c", 6155);
            gSPDisplayList(POLY_XLU_DISP++, master_sword_shadow_model);
        }
    }

    effect = effects;

    for (i = 0; i < BOSS_GANON2_EFFECT_COUNT; i++, effect++) {
        if (effect->type == 2) {
            if (objectFlag == 0) {
                obj_bank_set(NULL, play, OBJECT_GEFF, true);
                objectFlag++;
            }
            Matrix_translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_rotateY(effect->unk_38.z, MTXMODE_APPLY);
            Matrix_rotateX(effect->unk_38.y, MTXMODE_APPLY);
            Matrix_rotateZ(effect->unk_38.x, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_boss_ganon2.c", 6179);
            gSPDisplayList(POLY_OPA_DISP++, gGanonRubbleDL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_ganon2.c", 6185);
}

static s16 en_dataf[] = { 1, 2, 3, 3, 2, 1 };
static s16 en_data0[] = { 2, 3, 4, 4, 4, 3, 2 };
static s16 en_data1[] = { 2, 3, 4, 4, 4, 4, 3, 2 };
static s16 en_data2[] = { 2, 4, 5, 5, 6, 6, 6, 6, 5, 5, 4, 2 };
static s16 hokan_pt[] = { 1, -1, 1, 1, 3, 4, 1, 6, 7, 2, 9, 10, 2, 12, 13 };
static u8 size_data[] = { 3, 2, 2, 1, 3, 3, 1, 3, 3, 1, 0, 3, 1, 0, 3 };

static void shadow_tex_set(BossGanon2* this, u8* shadowTexture, f32 arg2) {
    s16 temp_t0;
    s16 temp_v0;
    s16 temp_a3;
    s16 phi_v1;
    s16 phi_a1;
    s16 i;
    f32 lerpx;
    s16 j;
    f32 lerpy;
    f32 lerpz;
    Vec3f sp70;
    Vec3f sp64;

    for (i = 0; i < 15; i++) {
        if ((arg2 == 0.0f) || ((j = hokan_pt[i]) >= 0)) {
            if (arg2 > 0.0f) {
                lerpx = this->unk_234[i].x + (this->unk_234[j].x - this->unk_234[i].x) * arg2;
                lerpy = this->unk_234[i].y + (this->unk_234[j].y - this->unk_234[i].y) * arg2;
                lerpz = this->unk_234[i].z + (this->unk_234[j].z - this->unk_234[i].z) * arg2;

                sp70.x = lerpx - this->actor.world.pos.x;
                sp70.y = lerpy - this->actor.world.pos.y + 76.0f + 30.0f + 30.0f + 100.0f;
                sp70.z = lerpz - this->actor.world.pos.z;
            } else {
                sp70.x = this->unk_234[i].x - this->actor.world.pos.x;
                sp70.y = this->unk_234[i].y - this->actor.world.pos.y + 76.0f + 30.0f + 30.0f + 100.0f;
                sp70.z = this->unk_234[i].z - this->actor.world.pos.z;
            }

            Matrix_Position(&sp70, &sp64);
            sp64.x *= 0.2f;
            sp64.y *= 0.2f;
            temp_a3 = sp64.x + 32.0f;
            temp_t0 = (s16)sp64.y * 64;

            if (size_data[i] == 2) {
                for (j = 0, phi_a1 = -0x180; j < 12; j++, phi_a1 += 0x40) {
                    for (phi_v1 = -en_data2[j]; phi_v1 < en_data2[j]; phi_v1++) {
                        temp_v0 = temp_a3 + phi_v1 + temp_t0 + phi_a1;
                        if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                            shadowTexture[temp_v0] = 0xFF;
                        }
                    }
                }
            } else if (size_data[i] == 1) {
                for (j = 0, phi_a1 = -0x100; j < 8; j++, phi_a1 += 0x40) {
                    for (phi_v1 = -en_data1[j]; phi_v1 < en_data1[j]; phi_v1++) {
                        temp_v0 = temp_a3 + phi_v1 + temp_t0 + phi_a1;
                        if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                            shadowTexture[temp_v0] = 0xFF;
                        }
                    }
                }
            } else if (size_data[i] == 0) {
                for (j = 0, phi_a1 = -0xC0; j < 7; j++, phi_a1 += 0x40) {
                    for (phi_v1 = -en_data0[j]; phi_v1 < en_data0[j] - 1; phi_v1++) {
                        temp_v0 = temp_a3 + phi_v1 + temp_t0 + phi_a1;
                        if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                            shadowTexture[temp_v0] = 0xFF;
                        }
                    }
                }
            } else {
                for (j = 0, phi_a1 = -0x80; j < 6; j++, phi_a1 += 0x40) {
                    for (phi_v1 = -en_dataf[j]; phi_v1 < en_dataf[j] - 1; phi_v1++) {
                        temp_v0 = temp_a3 + phi_v1 + temp_t0 + phi_a1;
                        if ((temp_v0 >= 0) && (temp_v0 < 0x1000)) {
                            shadowTexture[temp_v0] = 0xFF;
                        }
                    }
                }
            }
        }
    }
}

static void shadow_cont(void* shadowTexture, BossGanon2* this, PlayState* play) {
    s16 i;
    u32* p = shadowTexture;

    for (i = 0; i < 1024; i++, p++) {
        *p = 0;
    }

    Matrix_rotateX(1.0f, MTXMODE_NEW);

    for (i = 0; i < 6; i++) {
        shadow_tex_set(this, shadowTexture, i / 5.0f);
    }
}

static void shadow_disp(void* shadowTexture, BossGanon2* this, PlayState* play) {
    GameState* gameState = &play->state;
    s16 alpha;
    GraphicsContext* gfxCtx = gameState->gfxCtx;

    OPEN_DISPS(gfxCtx, "../z_boss_ganon2.c", 6430);

    _texture_z_light_fog_prim(gameState->gfxCtx);

    if ((play->envCtx.lightSetting == 1) || (play->envCtx.lightSetting == 2)) {
        alpha = (s16)(play->envCtx.lightBlend * 180.0f) + 30;
    } else {
        alpha = 120;
    }

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, alpha);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    Matrix_translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z - 20.0f, MTXMODE_NEW);
    Matrix_scale(1.65f, 1.0f, 1.65f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gameState->gfxCtx, "../z_boss_ganon2.c", 6457);
    gSPDisplayList(POLY_OPA_DISP++, ganon2_shadow_MODE);
    gDPLoadTextureBlock(POLY_OPA_DISP++, shadowTexture, G_IM_FMT_I, G_IM_SIZ_8b, 64, 64, 0, G_TX_NOMIRROR | G_TX_CLAMP,
                        G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
    gSPDisplayList(POLY_OPA_DISP++, ganon2_shadow_model);

    CLOSE_DISPS(gfxCtx, "../z_boss_ganon2.c", 6479);
}

// padding
static u32 D_809071FC[2] = { 0 };

#include "assets/overlays/ovl_Boss_Ganon2/z_boss_ganon2_shape.c"
