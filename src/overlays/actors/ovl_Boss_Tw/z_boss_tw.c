#include "z_boss_tw.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_tw/object_tw.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

typedef enum TwEffType {
    /*  0 */ TWEFF_NONE,
    /*  1 */ TWEFF_DOT,
    /*  2 */ TWEFF_2,
    /*  3 */ TWEFF_3,
    /*  4 */ TWEFF_RING,
    /*  5 */ TWEFF_PLYR_FRZ,
    /*  6 */ TWEFF_FLAME,
    /*  7 */ TWEFF_MERGEFLAME,
    /*  8 */ TWEFF_SHLD_BLST,
    /*  9 */ TWEFF_SHLD_DEFL,
    /* 10 */ TWEFF_SHLD_HIT
} TwEffType;

typedef enum EffectWork {
    /* 0 */ EFF_ARGS,
    /* 1 */ EFF_UNKS1,
    /* 2 */ EFF_WORK_MAX
} EffectWork;

typedef enum EffectFWork {
    /* 0 */ EFF_SCALE,
    /* 1 */ EFF_DIST,
    /* 2 */ EFF_ROLL,
    /* 3 */ EFF_YAW,
    /* 4 */ EFF_FWORK_MAX
} EffectFWork;

typedef enum TwinrovaType {
    /* 0x00 */ TW_KOTAKE,
    /* 0x01 */ TW_KOUME,
    /* 0x02 */ TW_TWINROVA,
    /* 0x64 */ TW_FIRE_BLAST = 0x64,
    /* 0x65 */ TW_FIRE_BLAST_GROUND,
    /* 0x66 */ TW_ICE_BLAST,
    /* 0x67 */ TW_ICE_BLAST_GROUND,
    /* 0x68 */ TW_DEATHBALL_KOTAKE,
    /* 0x69 */ TW_DEATHBALL_KOUME
} TwinrovaType;

#define BOSS_TW_EFFECT_COUNT 150

typedef struct BossTwEffect {
    /* 0x0000 */ u8 type;
    /* 0x0001 */ u8 frame;
    /* 0x0004 */ Vec3f pos;
    /* 0x0010 */ Vec3f curSpeed;
    /* 0x001C */ Vec3f accel;
    /* 0x0028 */ Color_RGB8 color;
    /* 0x002C */ s16 alpha;
    /* 0x002E */ s16 work[EFF_WORK_MAX];
    /* 0x0034 */ f32 workf[EFF_FWORK_MAX];
    /* 0x0044 */ Actor* target;
} BossTwEffect;

void Boss_Tw_actor_ct(Actor* thisx, PlayState* play2);
void Boss_Tw_actor_dt(Actor* thisx, PlayState* play);
void Boss_Tw_actor_move(Actor* thisx, PlayState* play);
void Boss_Tw_actor_draw(Actor* thisx, PlayState* play2);

void mode_g_damage_init(BossTw* this, PlayState* play, u8 damage);
void mode_g_move_init(BossTw* this, PlayState* play);
void Tw_Eff_disp(PlayState* play);
void mode_g_ufufu(BossTw* this, PlayState* play);
void mode_g_move(BossTw* this, PlayState* play);
void mode_g_after_damage(BossTw* this, PlayState* play);
void mode_g_after_damage_init(BossTw* this, PlayState* play);
void mode_g_ufufu_init(BossTw* this, PlayState* play);
void mode_g_after_atack(BossTw* this, PlayState* play);
void mode_g_after_atack_init(BossTw* this, PlayState* play);
void mode_g_atack_init(BossTw* this, PlayState* play);
void mode_g_tame_init(BossTw* this, PlayState* play);
void mode_g_stop(BossTw* this, PlayState* play);
void mode_g_enddemo(BossTw* this, PlayState* play);
void mode_g_startdemo(BossTw* this, PlayState* play);
void mode_startdemo(BossTw* this, PlayState* play);
void mode_enddemo(BossTw* this, PlayState* play);
void mode_g_gattai(BossTw* this, PlayState* play);
void mode_g_gattai_init(BossTw* this, PlayState* play);
void mode_gattai(BossTw* this, PlayState* play);
void mode_guard(BossTw* this, PlayState* play);
void mode_hihihi(BossTw* this, PlayState* play);
void mode_hihihi_init(BossTw* this, PlayState* play);
void mode_beam_end(BossTw* this, PlayState* play);
void mode_beam_end_init(BossTw* this, PlayState* play);
void mode_damage_init(BossTw* this, PlayState* play);
void mode_damage(BossTw* this, PlayState* play);
void mode_nomove(BossTw* this, PlayState* play);
void mode_beam(BossTw* this, PlayState* play);
void mode_fly(BossTw* this, PlayState* play);
void mode_beam_init(BossTw* this, PlayState* play);
void mode_fly_wait(BossTw* this, PlayState* play);
void tw_gattai_move(Actor* thisx, PlayState* play2);
void tw_gattai_draw(Actor* thisx, PlayState* play2);
void mode_nomove_init(BossTw* this, PlayState* play);
void mode_g_startdemo_init(BossTw* this, PlayState* play);
void mode_fly_init(BossTw* this, PlayState* play);
void mode_startdemo_init(BossTw* this, PlayState* play);
void tw_wepon_move(Actor* thisx, PlayState* play);
void tw_wepon_draw(Actor* thisx, PlayState* play2);
void tw_fire_move(BossTw* this, PlayState* play);
void tw_ice_move(BossTw* this, PlayState* play);
void tw_tamasii_move(BossTw* this, PlayState* play);
void tw_tamasii_draw(Actor* thisx, PlayState* play2);
void mode_g_damage(BossTw* this, PlayState* play);
void mode_g_guard(BossTw* this, PlayState* play);
void mode_g_atack(BossTw* this, PlayState* play);
void mode_g_tame(BossTw* this, PlayState* play);
void mode_g_guard_init(BossTw* this, PlayState* play);
void Tw_Eff_move(PlayState* play);

ActorProfile Boss_Tw_Profile = {
    /**/ ACTOR_BOSS_TW,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_TW,
    /**/ sizeof(BossTw),
    /**/ Boss_Tw_actor_ct,
    /**/ Boss_Tw_actor_dt,
    /**/ Boss_Tw_actor_move,
    /**/ Boss_Tw_actor_draw,
};

static Vec3f static_se_pos_L = { 0.0f, 0.0f, 1000.0f };
static Vec3f zero = { 0.0f, 0.0f, 0.0f };

static ColliderCylinderInit TwwAcOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ALL,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x30 },
        { 0x00100000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 25, 35, -17, { 0, 0, 0 } },
};

static ColliderCylinderInit TwAcOcInfoData = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x20 },
        { 0xFFCDFFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 45, 120, -30, { 0, 0, 0 } },
};

static ColliderCylinderInit TwgAcOcInfoData = {
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
        { 0xFFCFFFFF, 0x00, 0x20 },
        { 0xFFCDFFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 45, 120, -30, { 0, 0, 0 } },
};

static Vec3f tw_set_pos[] = {
    { 580.0f, 380.0f, 0.0f },
    { 0.0f, 380.0f, 580.0f },
    { -580.0f, 380.0f, 0.0f },
    { 0.0f, 380.0f, -580.0f },
};

static u8 init_f = false;

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_5, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 0, ICHAIN_STOP),
};

static s8 kankyo_flag;
static u8 wepon_flag;
static BossTw* kotake;
static BossTw* koume;
static BossTw* gattai;
static u8 pl_shield_charge_UME;
static u8 pl_shield_charge_TAKE;
static f32 pl_shield_charge_A;
static f32 pl_shield_charge_EFF_A;
static u8 gattai_wepon_switch;
static u8 gattai_wepon_switch2;
static u8 gattai_wepon_count;
static u8 pl_ice_flag;
static Vec3f pl_shield_pos;
static s16 pl_shield_angle_Y;
static u8 pl_shield_eff_wait;
static u8 pl_shield_charge_EFF_time;
static u8 pl_final_beam_on;
static s16 pl_shield_charge_EFF_SCR;
static s16 message_se_time;
static s16 message_se_time2;
static u8 message_se_flag;
static s16 message_se_time_2;
static s16 message_se_time2_2;
static u8 message_se_flag_2;
static BossTwEffect tw_eff[BOSS_TW_EFFECT_COUNT];

void tw_hinoko_ct(PlayState* play, Vec3f* initialPos, Vec3f* initialSpeed, Vec3f* accel, f32 scale, s16 args,
                         s16 countLimit) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = play->specialEffects; i < countLimit; i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_DOT;
            eff->pos = *initialPos;
            eff->curSpeed = *initialSpeed;
            eff->accel = *accel;
            eff->workf[EFF_SCALE] = scale / 1000.0f;
            eff->alpha = 255;
            eff->frame = (s16)rnd_f(10.0f);
            eff->work[EFF_ARGS] = args;
            break;
        }
    }
}

void tw_eff_smoke_ct(PlayState* play, s16 type, Vec3f* initialPos, Vec3f* initialSpeed, Vec3f* accel, f32 scale,
                        s16 alpha, s16 args, s16 countLimit) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = play->specialEffects; i < countLimit; i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = type;
            eff->pos = *initialPos;
            eff->curSpeed = *initialSpeed;
            eff->accel = *accel;
            eff->workf[EFF_SCALE] = scale / 1000.0f;
            eff->work[EFF_ARGS] = args;
            eff->alpha = alpha;
            eff->frame = (s16)rnd_f(100.0f);
            break;
        }
    }
}

void tw_wave_ct(PlayState* play, Vec3f* initialPos, f32 scale, f32 arg3, s16 alpha, s16 args, s16 arg6,
                          s16 countLimit) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = play->specialEffects; i < countLimit; i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_RING;
            eff->pos = *initialPos;
            eff->curSpeed = zero;
            eff->accel = zero;
            eff->workf[EFF_SCALE] = scale * 0.0025f;
            eff->workf[EFF_DIST] = arg3 * 0.0025f;
            eff->work[EFF_ARGS] = args;
            eff->work[EFF_UNKS1] = arg6;
            eff->alpha = alpha;
            eff->workf[EFF_ROLL] = rnd_f(M_PI);
            eff->frame = 0;
            break;
        }
    }
}

void tw_pl_ice_ct(PlayState* play, Actor* target) {
    BossTwEffect* eff;
    s16 i;

    for (eff = play->specialEffects, i = 0; i < BOSS_TW_EFFECT_COUNT; i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_PLYR_FRZ;
            eff->curSpeed = zero;
            eff->accel = zero;
            eff->frame = 0;
            eff->target = target;
            eff->workf[EFF_DIST] = 0.0f;
            eff->workf[EFF_SCALE] = 0.0f;
            eff->workf[EFF_ROLL] = 0.0f;
            if (target == NULL) {
                eff->work[EFF_ARGS] = 100;
            } else {
                eff->work[EFF_ARGS] = 20;
            }
            break;
        }
    }
}

void tw_magic_eff_ct(PlayState* play, Vec3f* initialPos, Vec3f* initialSpeed, Vec3f* accel, f32 scale, s16 args) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = play->specialEffects; i < BOSS_TW_EFFECT_COUNT; i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_FLAME;
            eff->pos = *initialPos;
            eff->curSpeed = *initialSpeed;
            eff->accel = *accel;
            eff->workf[EFF_SCALE] = scale / 1000.0f;
            eff->work[EFF_ARGS] = args;
            eff->work[EFF_UNKS1] = 0;
            eff->alpha = 0;
            eff->frame = (s16)rnd_f(1000.0f);
            break;
        }
    }
}

void tw_spin_eff_ct(PlayState* play, Vec3f* initialPos, f32 scale, f32 dist, s16 args) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = play->specialEffects; i < BOSS_TW_EFFECT_COUNT; i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_MERGEFLAME;
            eff->pos = *initialPos;
            eff->curSpeed = zero;
            eff->accel = zero;
            eff->workf[EFF_SCALE] = scale / 1000.0f;
            eff->work[EFF_ARGS] = args;
            eff->work[EFF_UNKS1] = 0;
            eff->workf[EFF_DIST] = dist;
            eff->workf[EFF_ROLL] = rnd_f(2.0f * M_PI);
            eff->alpha = 0;
            eff->frame = (s16)rnd_f(1000.0f);
            break;
        }
    }
}

void tw_final_beam_ct(PlayState* play, Vec3f* initialPos, Vec3f* initialSpeed, Vec3f* accel, f32 scale,
                                 f32 arg5, s16 alpha, s16 args) {
    s16 i;
    BossTwEffect* eff;

    for (i = 0, eff = play->specialEffects; i < BOSS_TW_EFFECT_COUNT; i++, eff++) {
        if (eff->type == TWEFF_NONE) {
            eff->type = TWEFF_SHLD_BLST;
            eff->pos = *initialPos;
            eff->curSpeed = *initialSpeed;
            eff->accel = *accel;
            eff->workf[EFF_SCALE] = scale / 1000.0f;
            eff->workf[EFF_DIST] = arg5 / 1000.0f;
            eff->work[EFF_ARGS] = args;
            eff->work[EFF_UNKS1] = 0;
            eff->alpha = alpha;
            eff->frame = (s16)rnd_f(1000.0f);
            break;
        }
    }
}

void tw_tate_eff_ct(PlayState* play, f32 arg1, s16 arg2) {
    s16 i;
    s16 j;
    BossTwEffect* eff;
    Player* player = GET_PLAYER(play);

    pl_shield_pos = player->bodyPartsPos[PLAYER_BODYPART_R_HAND];
    pl_shield_angle_Y = player->actor.shape.rot.y;

    for (i = 0; i < 8; i++) {
        for (eff = play->specialEffects, j = 0; j < BOSS_TW_EFFECT_COUNT; j++, eff++) {
            if (eff->type == TWEFF_NONE) {
                eff->type = TWEFF_SHLD_DEFL;
                eff->pos = pl_shield_pos;
                eff->curSpeed = zero;
                eff->accel = zero;
                eff->workf[EFF_ROLL] = i * (M_PI / 4.0f);
                eff->workf[EFF_YAW] = M_PI / 2.0f;
                eff->workf[EFF_DIST] = 0.0f;
                eff->workf[EFF_SCALE] = arg1 / 1000.0f;
                eff->work[EFF_ARGS] = arg2;
                eff->work[EFF_UNKS1] = 0;
                eff->alpha = 255;
                eff->frame = (s16)rnd_f(1000.0f);
                break;
            }
        }
    }
}

void tw_tate_eff2_ct(PlayState* play, f32 arg1, s16 arg2) {
    s16 i;
    s16 j;
    BossTwEffect* eff;
    Player* player = GET_PLAYER(play);

    pl_shield_pos = player->bodyPartsPos[PLAYER_BODYPART_R_HAND];
    pl_shield_angle_Y = player->actor.shape.rot.y;

    for (i = 0; i < 8; i++) {
        for (eff = play->specialEffects, j = 0; j < BOSS_TW_EFFECT_COUNT; j++, eff++) {
            if (eff->type == TWEFF_NONE) {
                eff->type = TWEFF_SHLD_HIT;
                eff->pos = pl_shield_pos;
                eff->curSpeed = zero;
                eff->accel = zero;
                eff->workf[EFF_ROLL] = i * (M_PI / 4.0f);
                eff->workf[EFF_YAW] = M_PI / 2.0f;
                eff->workf[EFF_DIST] = 0.0f;
                eff->workf[EFF_SCALE] = arg1 / 1000.0f;
                eff->work[EFF_ARGS] = arg2;
                eff->work[EFF_UNKS1] = 0;
                eff->alpha = 255;
                eff->frame = (s16)rnd_f(1000.0f);
                break;
            }
        }
    }
}

void Boss_Tw_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossTw* this = (BossTw*)thisx;
    s16 i;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, NULL, 0.0f);

    if (this->actor.params >= TW_FIRE_BLAST) {
        // Blasts
        Actor_set_scale(&this->actor, 0.01f);
        this->actor.update = tw_wepon_move;
        this->actor.draw = tw_wepon_draw;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &TwwAcOcInfoData);

        if (this->actor.params == TW_FIRE_BLAST || this->actor.params == TW_FIRE_BLAST_GROUND) {
            this->actionFunc = tw_fire_move;
            this->collider.elem.atDmgInfo.effect = 1;
        } else if (this->actor.params == TW_ICE_BLAST || this->actor.params == TW_ICE_BLAST_GROUND) {
            this->actionFunc = tw_ice_move;
        } else if (this->actor.params >= TW_DEATHBALL_KOTAKE) {
            this->actionFunc = tw_tamasii_move;
            this->actor.draw = tw_tamasii_draw;
            this->workf[TAIL_ALPHA] = 128.0f;

            if (thisx->params == TW_DEATHBALL_KOTAKE) {
                thisx->world.rot.y = gattai->actor.world.rot.y + 0x4000;
            } else {
                thisx->world.rot.y = gattai->actor.world.rot.y - 0x4000;
            }
        }

        this->timers[1] = 150;
        return;
    }

    Actor_set_scale(&this->actor, 2.5 * 0.01f);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.colChkInfo.health = 0;
    ClObjPipe_ct(play, &this->collider);

    if (!init_f) {
        init_f = true;
        play->envCtx.lightSettingOverride = 1;
        play->envCtx.prevLightSetting = 1;
        play->envCtx.lightSetting = 1;
        play->envCtx.lightBlend = 0.0f;

        message_se_time = message_se_time2 = message_se_flag = message_se_time_2 = message_se_time2_2 = message_se_flag_2 = pl_final_beam_on = pl_shield_charge_EFF_time =
            pl_shield_charge_EFF_SCR = pl_shield_eff_wait = kankyo_flag = wepon_flag = pl_ice_flag = gattai_wepon_switch =
                gattai_wepon_count = pl_shield_charge_UME = pl_shield_charge_TAKE = 0;

        pl_shield_charge_EFF_A = pl_shield_charge_A = 0.0f;
        gattai_wepon_switch2 = rnd_f(1.99f);
        play->specialEffects = tw_eff;

        for (i = 0; i < BOSS_TW_EFFECT_COUNT; i++) {
            tw_eff[i].type = TWEFF_NONE;
        }
    }

    if (this->actor.params == TW_KOTAKE) {
        ClObjPipe_set5(play, &this->collider, &this->actor, &TwAcOcInfoData);
        this->actor.naviEnemyId = NAVI_ENEMY_TWINROVA_KOTAKE;
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gTwinrovaKotakeSkel, &gTwinrovaKotakeKoumeFlyAnim, NULL, NULL, 0);

        if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_TWINROVA_BATTLE)) {
            // began twinrova battle
            mode_fly_init(this, play);
            this->actor.world.pos.x = -600.0f;
            this->actor.world.pos.y = 400.0f;
            this->actor.world.pos.z = 0.0f;
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS);
        } else {
            mode_startdemo_init(this, play);
        }

        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaKotakeKoumeFlyAnim, -3.0f);
        this->visible = true;
    } else if (this->actor.params == TW_KOUME) {
        ClObjPipe_set5(play, &this->collider, &this->actor, &TwAcOcInfoData);
        this->actor.naviEnemyId = NAVI_ENEMY_TWINROVA_KOUME;
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gTwinrovaKoumeSkel, &gTwinrovaKotakeKoumeFlyAnim, NULL, NULL, 0);

        if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_TWINROVA_BATTLE)) {
            // began twinrova battle
            mode_fly_init(this, play);
            this->actor.world.pos.x = 600.0f;
            this->actor.world.pos.y = 400.0f;
            this->actor.world.pos.z = 0.0f;
        } else {
            mode_startdemo_init(this, play);
        }

        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaKotakeKoumeFlyAnim, -3.0f);
        this->visible = true;
    } else {
        // Twinrova
        ClObjPipe_set5(play, &this->collider, &this->actor, &TwgAcOcInfoData);
        this->actor.naviEnemyId = NAVI_ENEMY_TWINROVA;
        this->actor.colChkInfo.health = 24;
        this->actor.update = tw_gattai_move;
        this->actor.draw = tw_gattai_draw;
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gTwinrovaSkel, &gTwinrovaTPoseAnim, NULL, NULL, 0);
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaTPoseAnim, -3.0f);

        if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_TWINROVA_BATTLE)) {
            // began twinrova battle
            mode_nomove_init(this, play);
        } else {
            mode_g_startdemo_init(this, play);
            this->actor.world.pos.x = 0.0f;
            this->actor.world.pos.y = 1000.0f;
            this->actor.world.pos.z = 0.0f;
        }

        this->actor.params = TW_TWINROVA;
        gattai = this;

        if (Actor_Environment_room_clear_Check(play, play->roomCtx.curRoom.num)) {
            // twinrova has been defeated.
            Actor_delete(&this->actor);
            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, 600.0f, 230.0f, 0.0f, 0, 0, 0,
                               WARP_DUNGEON_ADULT);
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, -600.0f, 230.0f, 0.0f, 0, 0, 0, 0);
        } else {
            kotake =
                (BossTw*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_TW, this->actor.world.pos.x,
                                            this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, TW_KOTAKE);
            koume =
                (BossTw*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_TW, this->actor.world.pos.x,
                                            this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, TW_KOUME);
            kotake->actor.parent = &koume->actor;
            koume->actor.parent = &kotake->actor;
        }
    }

    this->fogR = play->lightCtx.fogColor[0];
    this->fogG = play->lightCtx.fogColor[1];
    this->fogB = play->lightCtx.fogColor[2];
    this->fogNear = play->lightCtx.fogNear;
    this->fogFar = 1000.0f;
}

void Boss_Tw_actor_dt(Actor* thisx, PlayState* play) {
    BossTw* this = (BossTw*)thisx;

    ClObjPipe_dt(play, &this->collider);
    if (thisx->params < TW_FIRE_BLAST) {
        Skeleton_Info_dt(&this->skelAnime, play);
    }

    if (thisx->params == TW_TWINROVA) {
        init_f = false;
    }
}

void mode_fly_wait_init(BossTw* this, PlayState* play) {
    BossTw* otherTw = (BossTw*)this->actor.parent;

    this->actionFunc = mode_fly_wait;

    if ((otherTw != NULL) && (otherTw->actionFunc == mode_beam)) {
        this->timers[0] = 40;
    } else {
        this->timers[0] = 60;
    }

    this->rotateSpeed = 0.0f;
}

void mode_fly_wait(BossTw* this, PlayState* play) {
    BossTw* otherTw = (BossTw*)this->actor.parent;

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc2(&this->actor.speed, 0.0f, 1.0f, 1.0f);
    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, this->rotateSpeed);
    adds(&this->actor.shape.rot.x, 0, 5, this->rotateSpeed);
    add_calc2(&this->rotateSpeed, 4096.0f, 1.0f, 200.0f);
    Actor_position_speed_set_XY(&this->actor);
    Actor_position_move(&this->actor);
    if (this->timers[0] == 0) {
        if ((otherTw->actionFunc != mode_beam) && this->work[CAN_SHOOT]) {
            this->work[CAN_SHOOT] = false;
            mode_beam_init(this, play);
            this->actor.speed = 0.0f;
        } else {
            mode_fly_init(this, play);
        }
    }
}

void mode_fly_init(BossTw* this, PlayState* play) {
    static Vec3f tw_atack_pos[] = {
        { 600.0f, 400.0f, 0.0f }, { 0.0f, 400.0f, 600.0f }, { -600.0f, 400.0f, 0.0f }, { 0.0f, 400.0f, -600.0f }
    };
    BossTw* otherTw = (BossTw*)this->actor.parent;

    this->unk_5F8 = 1;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = mode_fly;
    this->rotateSpeed = 0.0f;
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaKotakeKoumeFlyAnim, -10.0f);
    if ((fqrand() < 0.5f) && (otherTw != NULL && otherTw->actionFunc == mode_beam)) {
        // Other Sister is shooting a beam, go near them.
        this->targetPos.x = otherTw->actor.world.pos.x + rnd_fx(200.0f);
        this->targetPos.y = rnd_f(200.0f) + 340.0f;
        this->targetPos.z = otherTw->actor.world.pos.z + rnd_fx(200.0f);
        this->timers[0] = (s16)rnd_f(50.0f) + 50;
    } else if (fqrand() < 0.5f) {
        // Fly to a random spot.
        this->targetPos.x = rnd_fx(800.0f);
        this->targetPos.y = rnd_f(200.0f) + 340.0f;
        this->targetPos.z = rnd_fx(800.0f);
        this->timers[0] = (s16)rnd_f(50.0f) + 50;
    } else {
        // fly to a random pillar.
        s16 idx = rnd_f(ARRAY_COUNT(tw_atack_pos) - 0.01f);

        this->targetPos = tw_atack_pos[idx];
        this->timers[0] = 200;
        this->work[CAN_SHOOT] = true;
    }
}

void mode_fly(BossTw* this, PlayState* play) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 pitchTarget;
    f32 yawTarget;
    f32 xzDist;

    Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
    add_calc2(&this->scepterAlpha, 0.0f, 1.0f, 10.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);

    xDiff = this->targetPos.x - this->actor.world.pos.x;
    yDiff = this->targetPos.y - this->actor.world.pos.y;
    zDiff = this->targetPos.z - this->actor.world.pos.z;

    yawTarget = RAD_TO_BINANG(fatan2(xDiff, zDiff));
    xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
    pitchTarget = RAD_TO_BINANG(fatan2(yDiff, xzDist));

    adds(&this->actor.world.rot.x, pitchTarget, 0xA, this->rotateSpeed);
    adds(&this->actor.world.rot.y, yawTarget, 0xA, this->rotateSpeed);
    adds(&this->actor.shape.rot.y, yawTarget, 0xA, this->rotateSpeed);
    adds(&this->actor.shape.rot.x, pitchTarget, 0xA, this->rotateSpeed);
    add_calc2(&this->rotateSpeed, 4096.0f, 1.0f, 100.0f);
    add_calc2(&this->actor.speed, 10.0f, 1.0f, 1.0f);
    Actor_position_speed_set_XY(&this->actor);
    Actor_position_move(&this->actor);

    if ((this->timers[0] == 0) || (xzDist < 70.0f)) {
        mode_fly_wait_init(this, play);
    }
}

void mode_beam_init(BossTw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actionFunc = mode_beam;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaKotakeKoumeChargeUpAttackStartAnim, -5.0f);
    this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaKotakeKoumeChargeUpAttackStartAnim);
    this->timers[1] = 70;
    this->targetPos = player->actor.world.pos;
    this->csState1 = 0;
    this->beamDist = 0.0f;
    this->beamReflectionDist = 0.0f;
    this->beamShootState = -1;
    this->beamScale = 0.01f;
    this->beamReflectionOrigin = this->beamOrigin;
    this->flameAlpha = 0.0f;
    this->spawnPortalAlpha = 0.0f;
    this->spawnPortalScale = 2000.0f;
    this->updateRate1 = 0.0f;
    this->portalRotation = 0.0f;
    this->updateRate2 = 0.0f;
}

void tw_wepon_gnd_set(BossTw* this, PlayState* play, s16 blastType) {
    BossTw* groundBlast;
    s16 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    for (i = 0; i < BOSS_TW_EFFECT_COUNT; i++) {
        velocity.x = rnd_fx(20.0f);
        velocity.y = rnd_f(10.0f);
        velocity.z = rnd_fx(20.0f);
        accel.y = 0.2f;
        accel.x = rnd_fx(0.25f);
        accel.z = rnd_fx(0.25f);
        pos = this->groundBlastPos;
        tw_hinoko_ct(play, &pos, &velocity, &accel, (s16)rnd_f(2.0f) + 8, blastType, 75);
    }

    if (blastType == 1) {
        wepon_flag = 1;
        groundBlast =
            (BossTw*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_TW, this->groundBlastPos.x,
                                        this->groundBlastPos.y, this->groundBlastPos.z, 0, 0, 0, TW_FIRE_BLAST_GROUND);
        if (groundBlast != NULL) {
            if (gattai->actionFunc == mode_nomove) {
                groundBlast->timers[0] = 100;
            } else {
                groundBlast->timers[0] = 50;
            }
            koume->workf[KM_GD_FLM_A] = koume->workf[KM_GD_SMOKE_A] = koume->workf[KM_GRND_CRTR_A] = 255.0f;
            koume->workf[KM_GD_FLM_SCL] = 1.0f;
            koume->workf[KM_GD_CRTR_SCL] = 0.005f;
            koume->groundBlastPos2 = groundBlast->actor.world.pos;
            kankyo_flag = 4;
        }
    } else {
        wepon_flag = 2;
        groundBlast =
            (BossTw*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_TW, this->groundBlastPos.x,
                                        this->groundBlastPos.y, this->groundBlastPos.z, 0, 0, 0, TW_ICE_BLAST_GROUND);
        if (groundBlast != NULL) {
            if (gattai->actionFunc == mode_nomove) {
                groundBlast->timers[0] = 100;
            } else {
                groundBlast->timers[0] = 50;
            }

            kotake->workf[UNK_F11] = 50.0f;
            kotake->workf[UNK_F9] = 250.0f;
            kotake->workf[UNK_F12] = 0.005f;
            kotake->workf[UNK_F14] = 1.0f;
            kotake->workf[UNK_F16] = 70.0f;
            kotake->groundBlastPos2 = groundBlast->actor.world.pos;
            kankyo_flag = 3;
        }
    }
}

s32 beam_pl_hitck(BossTw* this, PlayState* play) {
    Vec3f offset;
    Vec3f beamDistFromPlayer;
    Player* player = GET_PLAYER(play);

    offset.x = player->actor.world.pos.x - this->beamOrigin.x;
    offset.y = player->actor.world.pos.y - this->beamOrigin.y;
    offset.z = player->actor.world.pos.z - this->beamOrigin.z;

    Matrix_rotateX(-this->beamPitch, MTXMODE_NEW);
    Matrix_rotateY(-this->beamYaw, MTXMODE_APPLY);
    Matrix_Position(&offset, &beamDistFromPlayer);

    if (fabsf(beamDistFromPlayer.x) < 20.0f && fabsf(beamDistFromPlayer.y) < 50.0f && beamDistFromPlayer.z > 100.0f &&
        beamDistFromPlayer.z <= this->beamDist) {
        if (gattai->timers[2] == 0) {
            gattai->timers[2] = 150;
            this->beamDist = sqrtf(SQ(offset.x) + SQ(offset.y) + SQ(offset.z));
            Actor_player_power_damage_AT_set(play, &this->actor, 3.0f, this->actor.shape.rot.y, 0.0f, 0x20);

            if (this->actor.params == TW_KOTAKE) {
                if (pl_ice_flag == 0) {
                    pl_ice_flag = 1;
                }
            } else if (!player->bodyIsBurning) {
                s16 i;

                for (i = 0; i < PLAYER_BODYPART_MAX; i++) {
                    player->bodyFlameTimers[i] = get_random_timer(0, 200);
                }

                player->bodyIsBurning = true;
                player_SE_set(player, player->ageProperties->unk_92 + NA_SE_VO_LI_DEMO_DAMAGE);
            }
        }

        return true;
    }
    return false;
}

/**
 * Checks if the beam shot by `this` will be reflected
 * returns 0 if the beam will not be reflected,
 * returns 1 if the beam will be reflected,
 * and returns 2 if the beam will be diverted backwards
 */
s32 beam_shield_hitck(BossTw* this, PlayState* play) {
    Vec3f offset;
    Vec3f vec;
    Player* player = GET_PLAYER(play);

    if (player->stateFlags1 & PLAYER_STATE1_SHIELDING &&
        (s16)(player->actor.shape.rot.y - this->actor.shape.rot.y + 0x8000) < 0x2000 &&
        (s16)(player->actor.shape.rot.y - this->actor.shape.rot.y + 0x8000) > -0x2000) {
        // player is shielding and facing angles are less than 45 degrees in either direction
        offset.x = 0.0f;
        offset.y = 0.0f;
        offset.z = 10.0f;

        // set beam check point to 10 units in front of link.
        Matrix_rotateY(player->actor.shape.rot.y / (f32)0x8000 * M_PI, MTXMODE_NEW);
        Matrix_Position(&offset, &vec);

        // calculates a vector where the origin is at the beams origin,
        // and the positive z axis is pointing in the direction the beam
        // is shooting
        offset.x = player->actor.world.pos.x + vec.x - this->beamOrigin.x;
        offset.y = player->actor.world.pos.y + vec.y - this->beamOrigin.y;
        offset.z = player->actor.world.pos.z + vec.z - this->beamOrigin.z;

        Matrix_rotateX(-this->beamPitch, MTXMODE_NEW);
        Matrix_rotateY(-this->beamYaw, MTXMODE_APPLY);
        Matrix_Position(&offset, &vec);

        if (fabsf(vec.x) < 30.0f && fabsf(vec.y) < 70.0f && vec.z > 100.0f && vec.z <= this->beamDist) {
            // if the beam's origin is within 30 x units, 70 y units, is farther than 100 units
            // and the distance from the beams origin to 10 units in front of link is less than the beams
            // current distance (the distance of the beam is equal to or longer than the distance to 10 units
            // in front of link)
            if (mirror_shield_check(play)) {
                // player has mirror shield equipped
                this->beamDist = sqrtf(SQ(offset.x) + SQ(offset.y) + SQ(offset.z));
                return 1;
            }

            if (pl_shield_eff_wait > 10) {
                return 0;
            }

            if (pl_shield_eff_wait == 0) {
                // beam hit the shield, normal shield equipped,
                // divert the beam backwards from link's Y rotation
                tw_tate_eff_ct(play, 10.0f, this->actor.params);
                play->envCtx.lightBlend = 1.0f;
                this->timers[0] = 10;
                Na_StartSystemSe_F(NA_SE_IT_SHIELD_REFLECT_MG2);
            }

            pl_shield_eff_wait++;
            this->beamDist = sqrtf(SQ(offset.x) + SQ(offset.y) + SQ(offset.z));
            return 2;
        }
    }

    return 0;
}

s32 ref_hitck(BossTw* this, Vec3f* pos) {
    Vec3f offset;
    Vec3f beamDistFromTarget;

    offset.x = pos->x - this->beamReflectionOrigin.x;
    offset.y = pos->y - this->beamReflectionOrigin.y;
    offset.z = pos->z - this->beamReflectionOrigin.z;

    Matrix_rotateX(-this->beamReflectionPitch, MTXMODE_NEW);
    Matrix_rotateY(-this->beamReflectionYaw, MTXMODE_APPLY);
    Matrix_Position(&offset, &beamDistFromTarget);

    if (fabsf(beamDistFromTarget.x) < 50.0f && fabsf(beamDistFromTarget.y) < 50.0f && beamDistFromTarget.z > 100.0f &&
        beamDistFromTarget.z <= this->beamReflectionDist) {
        this->beamReflectionDist = sqrtf(SQ(offset.x) + SQ(offset.y) + SQ(offset.z)) * 1.1f;
        return true;
    } else {
        return false;
    }
}

f32 beam_bg_check(Vec3f* pos) {
    Vec3f posRotated;

    if (fabsf(pos->x) < 350.0f && fabsf(pos->z) < 350.0f && pos->y < 240.0f) {
        if (pos->y > 200.0f) {
            return 240.0f;
        }
        return 35.0f;
    }

    if (fabsf(pos->x) < 110.0f && ((fabsf(pos->z - 600.0f) < 110.0f) || (fabsf(pos->z + 600.0f) < 110.0f)) &&
        (pos->y < 230.0f)) {
        if (pos->y > 190.0f) {
            return 230.0f;
        }
        return 35.0f;
    }

    if (fabsf(pos->z) < 110.0f && ((fabsf(pos->x - 600.0f) < 110.0f) || (fabsf(pos->x + 600.0f) < 110.0f)) &&
        (pos->y < 230.0f)) {
        if (pos->y > 190.0f) {
            return 230.0f;
        }
        return 35.0f;
    }

    if (pos->y < -20.0f) {
        return 0.0f;
    }

    if (fabsf(pos->x) > 1140.0f || fabsf(pos->z) > 1140.0f) {
        return 35.0f;
    }

    Matrix_push();
    Matrix_rotateY(M_PI / 4, MTXMODE_NEW);
    Matrix_Position(pos, &posRotated);
    Matrix_pull();

    if (fabsf(posRotated.x) > 920.0f || fabsf(posRotated.z) > 920.0f) {
        return 35.0f;
    }

    return -100.0f;
}

void mode_beam(BossTw* this, PlayState* play) {
    s16 i;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 floorY;
    Vec3f sp130;
    Vec3s sp128;
    Player* player = GET_PLAYER(play);
    BossTw* otherTw = (BossTw*)this->actor.parent;
    Input* input = &play->state.input[0];

    add_calc2(&this->actor.world.pos.y, 400.0f, 0.05f, this->actor.speed);
    add_calc2(&this->actor.speed, 5.0f, 1.0f, 0.25f);
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->beamRoll += -0.3f;

    if (this->timers[1] != 0) {
        adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, this->rotateSpeed);
        if ((player->stateFlags1 & PLAYER_STATE1_SHIELDING) &&
            ((s16)((player->actor.shape.rot.y - this->actor.shape.rot.y) + 0x8000) < 0x2000) &&
            ((s16)((player->actor.shape.rot.y - this->actor.shape.rot.y) + 0x8000) > -0x2000)) {
            add_calc2(&this->targetPos.x, player->bodyPartsPos[PLAYER_BODYPART_R_HAND].x, 1.0f, 400.0f);
            add_calc2(&this->targetPos.y, player->bodyPartsPos[PLAYER_BODYPART_R_HAND].y, 1.0f, 400.0f);
            add_calc2(&this->targetPos.z, player->bodyPartsPos[PLAYER_BODYPART_R_HAND].z, 1.0f, 400.0f);
        } else {
            add_calc2(&this->targetPos.x, player->actor.world.pos.x, 1.0f, 400.0f);
            add_calc2(&this->targetPos.y, player->actor.world.pos.y + 30.0f, 1.0f, 400.0f);
            add_calc2(&this->targetPos.z, player->actor.world.pos.z, 1.0f, 400.0f);
        }

        this->timers[0] = 70;
        this->groundBlastPos.x = this->groundBlastPos.y = this->groundBlastPos.z = 0.0f;
        this->portalRotation += this->updateRate2 * 0.0025f;
        add_calc2(&this->spawnPortalAlpha, 255.0f, 1.0f, 10.0f);
        add_calc2(&this->updateRate2, 50.0f, 1.0f, 2.0f);

        if (this->timers[1] < 50) {
            if (this->timers[1] < 10) {
                if (this->timers[1] == 9) {
                    play->envCtx.lightBlend = 0.5f;
                    play->envCtx.lightSetting = 3 - this->actor.params;
                    Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_MASIC_SET);
                }

                if (this->timers[1] == 5) {
                    this->scepterAlpha = 255;
                }

                if (this->timers[1] > 4) {
                    s16 j;
                    for (j = 0; j < 2; j++) {
                        for (i = 0; i < ARRAY_COUNT(this->scepterFlamePos); i++) {
                            Vec3f pos;
                            Vec3f velocity;
                            Vec3f accel;

                            pos.x = this->scepterFlamePos[i].x;
                            pos.y = this->scepterFlamePos[i].y;
                            pos.z = this->scepterFlamePos[i].z;
                            velocity.x = rnd_fx(10.0f);
                            velocity.y = rnd_fx(10.0f);
                            velocity.z = rnd_fx(10.0f);
                            accel.x = 0.0f;
                            accel.y = 0.0f;
                            accel.z = 0.0f;
                            tw_magic_eff_ct(play, &pos, &velocity, &accel, rnd_f(10.0f) + 25.0f,
                                                  this->actor.params);
                        }
                    }
                }
            }

            if (this->timers[1] < 20) {
                add_calc2(&this->flameAlpha, 0, 1.0f, 20.0f);
                add_calc2(&this->spawnPortalAlpha, 0, 1.0f, 30.0f);
            } else {
                add_calc2(&this->flameAlpha, 255.0f, 1.0f, 10.0f);
                if (this->actor.params == TW_KOUME) {
                    Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_MS_FIRE - SFX_FLAG);
                } else {
                    Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_MS_FREEZE - SFX_FLAG);
                }
            }

            this->flameRotation += this->updateRate1 * 0.0025f;
            add_calc2(&this->spawnPortalScale, 0.0f, 0.1f, this->updateRate1);
            add_calc2(&this->updateRate1, 50.0f, 1.0f, 2.0f);
        }

        if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
            Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaKotakeKoumeChargeUpAttackLoopAnim, 0.0f);
            this->workf[ANIM_SW_TGT] = 10000.0f;
        }

        if (this->timers[1] == 1) {
            Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaKotakeKoumeAttackStartAnim, 0.0f);
            this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaKotakeKoumeAttackStartAnim);
            this->unk_4DC = 0.0f;
            this->spawnPortalAlpha = 0.0f;
            this->flameAlpha = 0.0f;
            pl_shield_eff_wait = 0;
        }
    } else {
        if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
            Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaKotakeKoumeAttackLoopAnim, 0.0f);
            this->workf[ANIM_SW_TGT] = 10000.0f;
        }

        if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT] - 5.0f)) {
            this->beamShootState = 0;
            kankyo_flag = this->actor.params + 1;
        }

        if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT] - 13.0f)) {
            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_THROW_MASIC);
            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_SHOOT_VOICE);
        }

        xDiff = this->targetPos.x - this->beamOrigin.x;
        yDiff = this->targetPos.y - this->beamOrigin.y;
        zDiff = this->targetPos.z - this->beamOrigin.z;

        this->beamYaw = fatan2(xDiff, zDiff);
        this->beamPitch = -fatan2(yDiff, sqrtf(SQ(xDiff) + SQ(zDiff)));

        switch (this->beamShootState) {
            case -1:
                break;
            case 0:
                if (this->timers[0] != 0) {
                    s32 beamReflection = beam_shield_hitck(this, play);

                    if (beamReflection == 1) {
                        Vec3f pos;
                        Vec3f velocity;
                        Vec3f accel = { 0.0f, 0.0f, 0.0f };

                        for (i = 0; i < 150; i++) {
                            velocity.x = rnd_fx(15.0f);
                            velocity.y = rnd_fx(15.0f);
                            velocity.z = rnd_fx(15.0f);
                            pos = player->bodyPartsPos[PLAYER_BODYPART_R_HAND];
                            tw_hinoko_ct(play, &pos, &velocity, &accel, (s16)rnd_f(2.0f) + 5,
                                                this->actor.params, 150);
                        }

                        this->beamShootState = 1;
                        Na_StartObjectSe_F(&player->actor.projectedPos, NA_SE_IT_SHIELD_REFLECT_MG);
                        Matrix_to_rotate_new(&player->shieldMf, &sp128, 0);
                        sp128.y += 0x8000;
                        sp128.x = -sp128.x;
                        this->magicDir.x = sp128.x;
                        this->magicDir.y = sp128.y;
                        this->groundBlastPos.x = 0.0f;
                        this->groundBlastPos.y = 0.0f;
                        this->groundBlastPos.z = 0.0f;
                        play->envCtx.lightBlend = 1.0f;
                        z_vibctl2_vib_setQ(0.0f, 100, 5, 4);
                    } else if (beamReflection == 0) {
                        beam_pl_hitck(this, play);

                        if (this->csState1 == 0) {
                            add_calc2(&this->beamDist, 2.0f * sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff)), 1.0f,
                                           40.0f);
                        }
                    }
                }

                Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &this->beamReflectionOrigin, &this->unk_54C,
                                             &this->actor.projectedW);

                if (this->actor.params == TW_KOUME) {
                    Nai_FxFlagEntry(NA_SE_EN_TWINROBA_SHOOT_FIRE - SFX_FLAG, &this->unk_54C, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                } else {
                    Nai_FxFlagEntry(NA_SE_EN_TWINROBA_SHOOT_FREEZE - SFX_FLAG, &this->unk_54C, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                }
                break;

            case 1:
                if (CHECK_BTN_ALL(input->cur.button, BTN_R)) {
                    Player* player = GET_PLAYER(play);

                    this->beamDist = sqrtf(SQ(xDiff) + SQ(yDiff) + SQ(zDiff));
                    add_calc2(&this->beamReflectionDist, 2000.0f, 1.0f, 40.0f);
                    add_calc2(&this->targetPos.x, player->bodyPartsPos[PLAYER_BODYPART_R_HAND].x, 1.0f, 400.0f);
                    add_calc2(&this->targetPos.y, player->bodyPartsPos[PLAYER_BODYPART_R_HAND].y, 1.0f, 400.0f);
                    add_calc2(&this->targetPos.z, player->bodyPartsPos[PLAYER_BODYPART_R_HAND].z, 1.0f, 400.0f);
                    if ((this->work[CS_TIMER_1] % 4) == 0) {
                        tw_wave_ct(play, &player->bodyPartsPos[PLAYER_BODYPART_R_HAND], 0.5f, 3.0f, 0xFF,
                                             this->actor.params, 1, BOSS_TW_EFFECT_COUNT);
                    }
                } else {
                    this->beamShootState = 0;
                    this->beamReflectionDist = 0.0f;
                }

                Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &this->unk_530, &this->unk_558,
                                             &this->actor.projectedW);

                if (this->actor.params == TW_KOUME) {
                    Nai_FxFlagEntry(NA_SE_EN_TWINROBA_SHOOT_FIRE - SFX_FLAG, &this->unk_558, 4U,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                    Nai_FxFlagEntry(NA_SE_EN_TWINROBA_REFL_FIRE - SFX_FLAG, &this->unk_558, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                } else {
                    Nai_FxFlagEntry(NA_SE_EN_TWINROBA_SHOOT_FREEZE - SFX_FLAG, &this->unk_558, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                    Nai_FxFlagEntry(NA_SE_EN_TWINROBA_REFL_FREEZE - SFX_FLAG, &this->unk_558, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                }
                break;
        }

        if (this->timers[0] == 0 && (kankyo_flag == 1 || kankyo_flag == 2)) {
            kankyo_flag = 0;
        }

        if (this->timers[0] == 0) {
            add_calc2(&this->beamScale, 0.0f, 1.0f, 0.0005f);

            if (this->beamScale == 0.0f) {
                mode_beam_end_init(this, play);
                this->beamReflectionDist = 0.0f;
                this->beamDist = 0.0f;
            }
        }
    }

    Matrix_translate(this->beamOrigin.x, this->beamOrigin.y, this->beamOrigin.z, MTXMODE_NEW);
    Matrix_rotateY(this->beamYaw, MTXMODE_APPLY);
    Matrix_rotateX(this->beamPitch, MTXMODE_APPLY);

    sp130.x = 0.0f;
    sp130.y = 0.0f;
    sp130.z = this->beamDist + -5.0f;

    Matrix_Position(&sp130, &this->beamReflectionOrigin);

    if ((this->csState1 == 0) && (this->beamShootState == 0) && (this->timers[0] != 0)) {
        this->groundBlastPos.y = beam_bg_check(&this->beamReflectionOrigin);

        if (this->groundBlastPos.y >= 0.0f) {
            this->csState1 = 1;
            this->groundBlastPos.x = this->beamReflectionOrigin.x;
            this->groundBlastPos.z = this->beamReflectionOrigin.z;
            tw_wepon_gnd_set(this, play, this->actor.params);
            this->timers[0] = 20;
        }
    }

    if (this->beamShootState == 1) {
        if (this->csState1 == 0) {
            Matrix_to_rotate_new(&player->shieldMf, &sp128, 0);
            sp128.y += 0x8000;
            sp128.x = -sp128.x;
            adds(&this->magicDir.x, sp128.x, 5, 0x2000);
            adds(&this->magicDir.y, sp128.y, 5, 0x2000);
            this->beamReflectionPitch = BINANG_TO_RAD_ALT(this->magicDir.x);
            this->beamReflectionYaw = BINANG_TO_RAD_ALT(this->magicDir.y);
        }

        Matrix_translate(this->beamReflectionOrigin.x, this->beamReflectionOrigin.y, this->beamReflectionOrigin.z,
                         MTXMODE_NEW);
        Matrix_rotateY(this->beamReflectionYaw, MTXMODE_APPLY);
        Matrix_rotateX(this->beamReflectionPitch, MTXMODE_APPLY);

        sp130.x = 0.0f;
        sp130.y = 0.0f;
        sp130.z = this->beamReflectionDist + -170.0f;

        Matrix_Position(&sp130, &this->unk_530);

        if (this->csState1 == 0) {
            sp130.z = 0.0f;

            for (i = 0; i < 200; i++) {
                Vec3f spBC;

                Matrix_Position(&sp130, &spBC);
                floorY = beam_bg_check(&spBC);
                this->groundBlastPos.y = floorY;

                if (floorY >= 0.0f) {
                    if ((this->groundBlastPos.y != 35.0f) && (0.0f < this->beamReflectionPitch) &&
                        (this->timers[0] != 0)) {
                        this->csState1 = 1;
                        this->groundBlastPos.x = spBC.x;
                        this->groundBlastPos.z = spBC.z;
                        tw_wepon_gnd_set(this, play, this->actor.params);
                        this->timers[0] = 20;
                    } else {
                        for (i = 0; i < 5; i++) {
                            Vec3f velocity;
                            Vec3f accel;

                            velocity.x = rnd_fx(20.0f);
                            velocity.y = rnd_fx(20.0f);
                            velocity.z = rnd_fx(20.0f);

                            accel.x = 0.0f;
                            accel.y = 0.0f;
                            accel.z = 0.0f;

                            tw_magic_eff_ct(play, &this->unk_530, &velocity, &accel,
                                                  rnd_f(10.0f) + 25.0f, this->actor.params);
                        }

                        this->beamReflectionDist = sp130.z;
                        add_calc2(&play->envCtx.lightBlend, 0.8f, 1.0f, 0.2f);
                    }
                    break;
                }

                sp130.z += 20.0f;

                if (this->beamReflectionDist < sp130.z) {
                    break;
                }
            }
        }

        if (ref_hitck(this, &this->actor.world.pos) && (this->work[CS_TIMER_1] % 4) == 0) {
            tw_wave_ct(play, &this->unk_530, 0.5f, 3.0f, 255, this->actor.params, 1, BOSS_TW_EFFECT_COUNT);
        }

        if (ref_hitck(this, &otherTw->actor.world.pos) && otherTw->actionFunc != mode_damage) {
            for (i = 0; i < 50; i++) {
                Vec3f pos;
                Vec3f velocity;
                Vec3f accel;

                pos.x = otherTw->actor.world.pos.x + rnd_fx(50.0f);
                pos.y = otherTw->actor.world.pos.y + rnd_fx(50.0f);
                pos.z = otherTw->actor.world.pos.z + rnd_fx(50.0f);

                velocity.x = rnd_fx(20.0f);
                velocity.y = rnd_fx(20.0f);
                velocity.z = rnd_fx(20.0f);

                accel.x = 0.0f;
                accel.y = 0.0f;
                accel.z = 0.0f;

                tw_magic_eff_ct(play, &pos, &velocity, &accel, rnd_f(10.0f) + 25.0f, this->actor.params);
            }

            mode_damage_init(otherTw, play);
            Actor_SE_set(&otherTw->actor, NA_SE_EN_TWINROBA_DAMAGE_VOICE);
            play->envCtx.lightBlend = 1.0f;
            otherTw->actor.colChkInfo.health++;
        }
    }
}

void mode_beam_end_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_beam_end;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaKotakeKoumeAttackEndAnim, 0.0f);
    this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaKotakeKoumeAttackEndAnim);
}

void mode_beam_end(BossTw* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc2(&this->scepterAlpha, 0.0f, 1.0f, 10.0f);

    if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
        if (gattai->timers[2] == 0) {
            mode_fly_init(this, play);
        } else {
            mode_hihihi_init(this, play);
        }

        this->scepterAlpha = 0.0f;
    }
}

void mode_damage_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_damage;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaKotakeKoumeDamageStartAnim, 0.0f);
    this->timers[0] = 53;
    this->actor.speed = 0.0f;

    if (this->actor.params == TW_KOTAKE) {
        this->work[FOG_TIMER] = 20;
    }
}

void mode_damage(BossTw* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((this->work[CS_TIMER_1] % 4) == 0) {
        Vec3f pos;
        Vec3f velocity;
        Vec3f accel;

        pos.x = this->actor.world.pos.x + rnd_fx(80.0f);
        pos.y = this->actor.world.pos.y + rnd_fx(80.0f);
        pos.z = this->actor.world.pos.z + rnd_fx(80.0f);

        velocity.x = 0.0f;
        velocity.y = 0.0f;
        velocity.z = 0.0f;

        accel.x = 0.0f;
        accel.y = 0.1f;
        accel.z = 0.0f;

        tw_eff_smoke_ct(play, this->actor.params + TWEFF_2, &pos, &velocity, &accel, rnd_f(10.0f) + 15.0f,
                           0, 0, 150);
    }

    if (this->actor.params == TW_KOUME) {
        add_calc2(&this->fogR, 255.0f, 1.0f, 30.0f);
        add_calc2(&this->fogG, 255.0f, 1.0f, 30.0f);
        add_calc2(&this->fogB, 255.0f, 1.0f, 30.0f);
        add_calc2(&this->fogNear, 900.0f, 1.0f, 30.0f);
        add_calc2(&this->fogFar, 1099.0f, 1.0f, 30.0f);
    }

    add_calc2(&this->actor.world.pos.y, ((sin_s(this->work[CS_TIMER_1] * 1500) * 20.0f) + 350.0f) + 50.0f,
                   0.1f, this->actor.speed);
    add_calc2(&this->actor.speed, 5.0f, 1.0f, 1.0f);

    this->actor.world.pos.y -= 50.0f;
    Actor_BGcheck2(play, &this->actor, 50.0f, 50.0f, 100.0f, UPDBGCHECKINFO_FLAG_2);
    this->actor.world.pos.y += 50.0f;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = 0.0f;
    }

    if (this->timers[0] == 1) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaKotakeKoumeDamageEndAnim, 0.0f);
        this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaKotakeKoumeDamageEndAnim);
    }

    if ((this->timers[0] == 0) && Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
        mode_fly_init(this, play);
    }
}

void mode_hihihi_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_hihihi;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaKotakeKoumeLaughAnim, 0.0f);
    this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaKotakeKoumeLaughAnim);
    this->actor.speed = 0.0f;
}

void mode_hihihi(BossTw* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Skeleton_Info_frame_check(&this->skelAnime, 10.0f)) {
        if (this->actor.params == TW_KOUME) {
            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_LAUGH);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_LAUGH2);
        }
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
        mode_fly_init(this, play);
    }
}

void mode_guard_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_guard;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaKotakeKoumeSpinAnim, -3.0f);
    this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaKotakeKoumeSpinAnim);
    this->actor.speed = 0.0f;
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->timers[0] = 20;
}

void mode_guard(BossTw* this, PlayState* play) {
    if (this->timers[0] != 0) {
        this->collider.base.colMaterial = COL_MATERIAL_METAL;
        this->actor.shape.rot.y -= 0x3000;

        if ((this->timers[0] % 4) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_ROLL);
        }
    } else {
        Skeleton_Info2_anime_play(&this->skelAnime);
        adds(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x2000);

        if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
            mode_fly_init(this, play);
        }
    }
}

void mode_gattai_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_gattai;
    this->rotateSpeed = 0.0f;
    this->actor.speed = 0.0f;
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaKotakeKoumeFlyAnim, -10.0f);
}

void mode_gattai(BossTw* this, PlayState* play) {
    add_calc2(&this->scepterAlpha, 0.0f, 1.0f, 10.0f);
    Skeleton_Info2_anime_play(&this->skelAnime);
}

void mode_nomove_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_nomove;
    this->visible = false;
    this->actor.world.pos.y = -2000.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void mode_nomove(BossTw* this, PlayState* play) {
    if ((this->actor.params == TW_TWINROVA) && (koume->actionFunc == mode_fly) &&
        (kotake->actionFunc == mode_fly) &&
        ((koume->actor.colChkInfo.health + kotake->actor.colChkInfo.health) >= 4)) {

        mode_g_gattai_init(this, play);
        mode_gattai_init(kotake, play);
        mode_gattai_init(koume, play);
    }
}

void mode_g_gattai_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_g_gattai;
    this->csState2 = 0;
    this->csState1 = 0;
}

void mode_g_gattai(BossTw* this, PlayState* play) {
    s16 i;
    Vec3f spB0;
    Vec3f spA4;
    Player* player = GET_PLAYER(play);

    switch (this->csState2) {
        case 0:
            this->csState2 = 1;
            Demo_play_start(play, &play->csCtx);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_57);
            this->subCamId = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            this->subCamDist = 800.0f;
            this->subCamYaw = M_PI;
            koume->actor.world.rot.x = 0;
            koume->actor.shape.rot.x = 0;
            kotake->actor.world.rot.x = 0;
            kotake->actor.shape.rot.x = 0;
            this->workf[UNK_F9] = 0.0f;
            this->workf[UNK_F10] = 0.0f;
            this->workf[UNK_F11] = 600.0f;
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 200);
            this->work[CS_TIMER_2] = 0;
            FALLTHROUGH;
        case 1:
            if (this->work[CS_TIMER_2] == 20) {
                message_set(play, 0x6059, NULL);
            }

            if (this->work[CS_TIMER_2] == 80) {
                message_set(play, 0x605A, NULL);
            }

            this->subCamAt.x = 0.0f;
            this->subCamAt.y = 440.0f;
            this->subCamAt.z = 0.0f;

            spB0.x = 0.0f;
            spB0.y = 0.0f;
            spB0.z = this->subCamDist;

            Matrix_rotateY(this->subCamYaw, MTXMODE_NEW);
            Matrix_Position(&spB0, &spA4);

            this->subCamEye.x = spA4.x;
            this->subCamEye.y = 300.0f;
            this->subCamEye.z = spA4.z;

            add_calc2(&this->subCamYaw, 0.3f, 0.02f, 0.03f);
            add_calc2(&this->subCamDist, 200.0f, 0.1f, 5.0f);
            break;

        case 2:
            spB0.x = 0.0f;
            spB0.y = 0.0f;
            spB0.z = this->subCamDist;
            Matrix_rotateY(this->subCamYaw, MTXMODE_NEW);
            Matrix_Position(&spB0, &spA4);
            this->subCamEye.x = spA4.x;
            this->subCamEye.z = spA4.z;
            add_calc2(&this->subCamEye.y, 420.0f, 0.1f, this->subCamUpdateRate * 20.0f);
            add_calc2(&this->subCamAt.y, 470.0f, 0.1f, this->subCamUpdateRate * 6.0f);
            add_calc2(&this->subCamYaw, 0.3f, 0.02f, 0.03f);
            add_calc2(&this->subCamDist, 60.0f, 0.1f, this->subCamUpdateRate * 32.0f);
            add_calc2(&this->subCamUpdateRate, 1, 1, 0.1f);
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        if (this->unk_5F9 == 0) {
            Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
        } else {
            Gama_play_camera_setting(play, this->subCamId, &this->subCamAt2, &this->subCamEye2);
        }
    }

    switch (this->csState1) {
        case 0:
            Actor_SE_set(&kotake->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
            Actor_SE_set(&koume->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
            spB0.x = this->workf[UNK_F11];
            spB0.y = 400.0f;
            spB0.z = 0.0f;
            Matrix_rotateY(this->workf[UNK_F9], MTXMODE_NEW);
            Matrix_Position(&spB0, &spA4);
            koume->actor.world.pos.x = spA4.x;
            koume->actor.world.pos.y = spA4.y;
            koume->actor.world.pos.z = spA4.z;
            koume->actor.shape.rot.y = (this->workf[UNK_F9] / M_PI) * (f32)0x8000;
            kotake->actor.world.pos.x = -spA4.x;
            kotake->actor.world.pos.y = spA4.y;
            kotake->actor.world.pos.z = -spA4.z;
            kotake->actor.shape.rot.y = ((this->workf[UNK_F9] / M_PI) * (f32)0x8000) + (f32)0x8000;
            add_calc2(&this->workf[UNK_F11], 0.0f, 0.1f, 7.0f);
            this->workf[UNK_F9] -= this->workf[UNK_F10];
            add_calc2(&this->workf[UNK_F10], 0.5f, 1, 0.0039999997f);
            if (this->workf[UNK_F11] < 10.0f) {
                if (!this->work[PLAYED_CHRG_SFX]) {
                    Actor_SE_set(&koume->actor, NA_SE_EN_TWINROBA_POWERUP);
                    this->work[PLAYED_CHRG_SFX] = true;
                }

                add_calc2(&koume->actor.scale.x, 0.005000001f, 1, 0.0003750001f);

                for (i = 0; i < 4; i++) {
                    Vec3f pos;
                    f32 yOffset;
                    f32 xScale;

                    xScale = koume->actor.scale.x * 3000.0f;
                    yOffset = rnd_fx(xScale * 2.0f);
                    pos.x = 3000.0f;
                    pos.y = 400.0f + yOffset;
                    pos.z = 0.0f;
                    tw_spin_eff_ct(play, &pos, rnd_f(5.0f) + 10.0f,
                                               sqrtf(SQ(xScale) - SQ(yOffset)), rnd_f(1.99f));
                }

                if (koume->actor.scale.x <= 0.0051f) {
                    Vec3f pos;
                    Vec3f velocity;
                    Vec3f accel;

                    this->actor.world.pos.y = 400.0f;

                    for (i = 0; i < 50; i++) {
                        pos = this->actor.world.pos;
                        velocity.x = rnd_fx(20.0f);
                        velocity.y = rnd_fx(20.0f);
                        velocity.z = rnd_fx(20.0f);
                        pos.x += velocity.x;
                        pos.y += velocity.y;
                        pos.z += velocity.z;
                        accel.z = accel.y = accel.x = 0.0f;
                        tw_magic_eff_ct(play, &pos, &velocity, &accel, rnd_f(10.0f) + 25.0f,
                                              velocity.x < 0.0f);
                    }

                    this->csState1 = 1;
                    this->visible = true;
                    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                    this->actor.shape.rot.y = 0;
                    mode_nomove_init(kotake, play);
                    mode_nomove_init(koume, play);
                    Actor_set_scale(&this->actor, 0.0f);
                    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaIntroAnim, 0.0f);
                    this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaIntroAnim);
                    this->timers[0] = 50;
                    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_2);
                    Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_TRANSFORM);
                    SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS);
                }
            }

            kotake->actor.scale.x = kotake->actor.scale.y = kotake->actor.scale.z =
                koume->actor.scale.y = koume->actor.scale.z = koume->actor.scale.x;
            break;

        case 1:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
                Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaHoverAnim, -15.0f);
            }

            kankyo_flag = -1;
            play->envCtx.lightSetting = 4;
            add_calc2(&play->envCtx.lightBlend, 1, 1, 0.1f);
            FALLTHROUGH;
        case 2:
            Skeleton_Info2_anime_play(&this->skelAnime);
            add_calc2(&this->actor.scale.x, 0.0069999993f, 1, 0.0006999999f);
            this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;

            if (this->timers[0] == 1) {
                this->csState2 = 2;
                this->subCamUpdateRate = 0.0f;
                this->timers[1] = 65;
                this->timers[2] = 90;
                this->timers[3] = 50;
                player->actor.world.pos.x = 0.0f;
                player->actor.world.pos.y = 240.0f;
                player->actor.world.pos.z = 270.0f;
                player->actor.world.rot.y = player->actor.shape.rot.y = -0x8000;
                this->subCamEye2.x = 0.0f;
                this->subCamEye2.y = 290.0f;
                this->subCamEye2.z = 222.0f;
                this->subCamAt2.x = player->actor.world.pos.x;
                this->subCamAt2.y = player->actor.world.pos.y + 54.0f;
                this->subCamAt2.z = player->actor.world.pos.z;
            }

            if (this->timers[3] == 19) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_5);
            }

            if (this->timers[3] == 16) {
                player_SE_set(player, player->ageProperties->unk_92 + NA_SE_VO_LI_SURPRISE);
            }

            if ((this->timers[3] != 0) && (this->timers[3] < 20)) {
                this->unk_5F9 = 1;
                add_calc2(&this->subCamEye2.z, 242.0f, 0.2f, 100.0f);
            } else {
                this->unk_5F9 = 0;
            }

            if (this->timers[1] == 8) {
                this->work[TW_BLINK_IDX] = 8;
                Na_StartSystemSe_F(NA_SE_EN_TWINROBA_YOUNG_WINK);
            }
            if (this->timers[2] == 4) {
                kankyo_flag = 0;
                play->envCtx.prevLightSetting = 5;
            }

            if (this->timers[2] == 1) {
                Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                this->csState2 = this->subCamId;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                this->work[TW_PLLR_IDX] = 0;
                this->targetPos = tw_set_pos[0];
                mode_g_move_init(this, play);
            }
            break;
    }
}

void mode_enddemo_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_enddemo;
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaKotakeKoumeIdleLoopAnim, -3.0f);
    this->unk_5F8 = 0;
    this->work[CS_TIMER_2] = rnd_f(20.0f);
}

void mode_enddemo(BossTw* this, PlayState* play) {
    if (this->timers[0] == 0) {
        Skeleton_Info2_anime_play(&this->skelAnime);
    }

    adds(&this->actor.shape.rot.y, this->work[YAW_TGT], 5, this->rotateSpeed);
    add_calc2(&this->rotateSpeed, 20480.0f, 1.0f, 1000.0f);

    if (gattai->work[CS_TIMER_2] > 140) {
        add_calc2(&this->fogR, 100.0f, 1.0f, 15.0f);
        add_calc2(&this->fogG, 255.0f, 1.0f, 15.0f);
        add_calc2(&this->fogB, 255.0f, 1.0f, 15.0f);
        add_calc2(&this->fogNear, 850.0f, 1.0f, 15.0f);
        add_calc2(&this->fogFar, 1099.0f, 1.0f, 15.0f);
    }
}

void mode_startdemo_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_startdemo;
    this->visible = false;
    this->actor.world.pos.y = -2000.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

/**
 * Do nothing while waiting for the initial cutscene to start
 */
void mode_startdemo(BossTw* this, PlayState* play) {
}

void mode_g_startdemo_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_g_startdemo;
    this->visible = false;
    this->actor.world.pos.y = -2000.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void mode_g_startdemo(BossTw* this, PlayState* play) {
    u8 updateCam = false;
    s16 i;
    Vec3f sp90;
    Vec3f sp84;
    Player* player = GET_PLAYER(play);

    if (this->csSfxTimer > 220 && this->csSfxTimer < 630) {
        Na_StartSystemSe_F(NA_SE_EN_TWINROBA_UNARI - SFX_FLAG);
    }

    if (this->csSfxTimer == 180) {
        Na_StartObjectSe_F(&static_se_pos_L, NA_SE_EN_TWINROBA_LAUGH);
        Na_StartObjectSe_F(&static_se_pos_L, NA_SE_EN_TWINROBA_LAUGH2);
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_KOTAKE_KOUME);
    }

    this->csSfxTimer++;

    switch (this->csState2) {
        case 0:
            this->csSfxTimer = 0;

            if (SQ(player->actor.world.pos.x) + SQ(player->actor.world.pos.z) < SQ(150.0f)) {
                player->actor.world.pos.x = player->actor.world.pos.z = .0f;
                this->csState2 = 1;
                Demo_play_start(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_57);
                this->subCamId = Gama_play_make_camera(play);
                Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
                Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
                this->subCamEye.x = 0.0f;
                this->subCamEye.y = 350;
                this->subCamEye.z = 200;

                this->subCamEyeNext.x = 450;
                this->subCamEyeNext.y = 900;

                this->subCamAt.x = 0;
                this->subCamAt.y = 270;
                this->subCamAt.z = 0;

                this->subCamAtNext.x = 0;
                this->subCamAtNext.y = 240;
                this->subCamAtNext.z = 140;

                this->subCamEyeNext.z = 530;
                this->subCamEyeVel.x = fabsf(this->subCamEyeNext.x - this->subCamEye.x);
                this->subCamEyeVel.y = fabsf(this->subCamEyeNext.y - this->subCamEye.y);
                this->subCamEyeVel.z = fabsf(this->subCamEyeNext.z - this->subCamEye.z);
                this->subCamAtVel.x = fabsf(this->subCamAtNext.x - this->subCamAt.x);
                this->subCamAtVel.y = fabsf(this->subCamAtNext.y - this->subCamAt.y);
                this->subCamAtVel.z = fabsf(this->subCamAtNext.z - this->subCamAt.z);

                this->subCamDistStep = 0.05f;
                this->work[CS_TIMER_1] = 0;
            }
            break;

        case 1:
            updateCam = true;

            if (this->work[CS_TIMER_1] == 30) {
                message_set(play, 0x6048, NULL);
            }

            add_calc2(&this->subCamUpdateRate, 0.01f, 1.0f, 0.0001f);

            if (this->work[CS_TIMER_1] > 100) {
                play->envCtx.lightSetting = 0;
                add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.03f);
            }

            if (this->work[CS_TIMER_1] == 180) {
                Na_StartSystemSe_F(NA_SE_EN_TWINROBA_APPEAR_MS);
            }

            if (this->work[CS_TIMER_1] > 180) {
                this->spawnPortalScale = 0.05f;
                add_calc2(&this->spawnPortalAlpha, 255.0f, 1.0f, 5.f);

                if (this->work[CS_TIMER_1] >= 236) {
                    this->csState2 = 2;
                    koume->visible = 1;
                    Skeleton_Info2_init_standard_repeat_morf(&koume->skelAnime, &gTwinrovaKotakeKoumeIdleLoopAnim, 0.0f);
                    koume->actor.world.pos.x = 0.0f;
                    koume->actor.world.pos.y = 80.0f;
                    koume->actor.world.pos.z = 600.0f;
                    koume->actor.shape.rot.y = koume->actor.world.rot.y = -0x8000;

                    this->subCamEye.x = -30;
                    this->subCamEye.y = 260;
                    this->subCamEye.z = 470;

                    this->subCamAt.x = 0.0F;
                    this->subCamAt.y = 270;
                    this->subCamAt.z = 600.0F;

                    this->work[CS_TIMER_1] = 0;

                    Actor_set_scale(&koume->actor, 0.014999999f);
                }
            }
            break;

        case 2:
            Skeleton_Info2_anime_play(&koume->skelAnime);
            add_calc2(&koume->actor.world.pos.y, 240.0f, 0.05f, 5.0f);
            this->subCamEye.x -= 0.2f;
            this->subCamEye.z += 0.2f;

            if (this->work[CS_TIMER_1] > 50) {
                this->csState2 = 3;

                this->subCamEyeNext.x = -30;
                this->subCamEyeNext.y = 260;
                this->subCamEyeNext.z = 530;

                this->subCamAtNext.x = 0.0f;
                this->subCamAtNext.y = 265;
                this->subCamAtNext.z = 580;

                this->subCamEyeVel.x = fabsf(this->subCamEyeNext.x - this->subCamEye.x);
                this->subCamEyeVel.y = fabsf(this->subCamEyeNext.y - this->subCamEye.y);
                this->subCamEyeVel.z = fabsf(this->subCamEyeNext.z - this->subCamEye.z);
                this->subCamAtVel.x = fabsf(this->subCamAtNext.x - this->subCamAt.x);
                this->subCamAtVel.y = fabsf(this->subCamAtNext.y - this->subCamAt.y);
                this->subCamAtVel.z = fabsf(this->subCamAtNext.z - this->subCamAt.z);
                this->subCamUpdateRate = 0;
                this->subCamDistStep = 0.1f;
                this->work[CS_TIMER_1] = 0;
            }
            break;

        case 3:
            Skeleton_Info2_anime_play(&koume->skelAnime);
            updateCam = true;
            add_calc2(&koume->actor.world.pos.y, 240.0f, 0.05f, 5.0f);
            add_calc2(&this->subCamUpdateRate, 1.0f, 1.0f, 0.02f);

            if (this->work[CS_TIMER_1] == 30) {
                message_set(play, 0x6049, NULL);
            }

            if (this->work[CS_TIMER_1] > 80) {
                this->csState2 = 4;
                this->actor.speed = 0;

                this->subCamEyeNext.x = -80.0f;
                this->subCamEyeNext.y = 260.0f;
                this->subCamEyeNext.z = 430.0f;

                this->subCamAtNext.x = koume->actor.world.pos.x;
                this->subCamAtNext.y = koume->actor.world.pos.y + 20.0f;
                this->subCamAtNext.z = koume->actor.world.pos.z;

                this->subCamEyeVel.x = fabsf(this->subCamEyeNext.x - this->subCamEye.x);
                this->subCamEyeVel.y = fabsf(this->subCamEyeNext.y - this->subCamEye.y);
                this->subCamEyeVel.z = fabsf(this->subCamEyeNext.z - this->subCamEye.z);
                this->subCamAtVel.x = fabsf(this->subCamAtNext.x - this->subCamAt.x);
                this->subCamAtVel.y = fabsf(this->subCamAtNext.y - this->subCamAt.y);
                this->subCamAtVel.z = fabsf(this->subCamAtNext.z - this->subCamAt.z);
                this->subCamUpdateRate = 0.0f;
                this->subCamDistStep = 0.05f;
                Skeleton_Info2_init_standard_stop_morf(&koume->skelAnime, &gTwinrovaKotakeKoumeIdleEndAnim, 0.0f);
                this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaKotakeKoumeIdleEndAnim);
                this->work[CS_TIMER_1] = 0;
            }
            break;

        case 4:
            updateCam = true;
            Skeleton_Info2_anime_play(&koume->skelAnime);
            this->subCamAtNext.y = 20.0f + koume->actor.world.pos.y;
            add_calc2(&koume->actor.world.pos.y, 350, 0.1f, this->actor.speed);
            add_calc2(&this->actor.speed, 9.0f, 1.0f, 0.9f);
            add_calc2(&this->subCamUpdateRate, 1.0f, 1.0f, 0.02f);

            if (this->work[CS_TIMER_1] >= 30) {
                if (this->work[CS_TIMER_1] < 45) {
                    play->envCtx.prevLightSetting = 0;
                    play->envCtx.lightSetting = 2;
                    play->envCtx.lightBlend = 1.0f;
                } else {
                    add_calc0(&play->envCtx.lightBlend, 1.0f, 0.1f);
                }

                if (this->work[CS_TIMER_1] == 30) {
                    for (i = 0; i < 50; i++) {
                        Vec3f pos;
                        Vec3f velocity;

                        pos.x = koume->actor.world.pos.x + rnd_fx(50.0f);
                        pos.y = koume->actor.world.pos.y + rnd_fx(50.0f);
                        pos.z = koume->actor.world.pos.z + rnd_fx(50.0f);
                        velocity.x = rnd_fx(20.0f);
                        velocity.y = rnd_fx(20.0f);
                        velocity.z = rnd_fx(20.0f);
                        tw_magic_eff_ct(play, &pos, &velocity, &zero, rnd_f(10.0f) + 25.0f, 1);
                    }

                    Actor_SE_set(&koume->actor, NA_SE_EN_TWINROBA_TRANSFORM);
                    play->envCtx.lightBlend = 0;
                }

                if (this->work[CS_TIMER_1] >= 35) {
                    if (this->work[CS_TIMER_1] < 50) {
                        add_calc2(&koume->actor.scale.x,
                                       ((sin_s(this->work[CS_TIMER_1] * 0x4200) * 20.0f) / 10000.0f) + 0.024999999f,
                                       1.0f, 0.005f);
                    } else {
                        if (this->work[CS_TIMER_1] == 50) {
                            Skeleton_Info2_init_standard_stop_morf(&koume->skelAnime, &gTwinrovaKotakeKoumeLaughAnim, -5);
                            this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaKotakeKoumeLaughAnim);
                        }

                        if (this->work[CS_TIMER_1] == 60) {
                            Actor_SE_set(&koume->actor, NA_SE_EN_TWINROBA_LAUGH);
                        }

                        if (Skeleton_Info_frame_check(&koume->skelAnime, this->workf[ANIM_SW_TGT])) {
                            Skeleton_Info2_init_standard_repeat_morf(&koume->skelAnime, &gTwinrovaKotakeKoumeFlyAnim, 0.f);
                            this->workf[ANIM_SW_TGT] = 1000.0f;
                        }

                        add_calc2(&koume->actor.scale.x, 0.024999999f, 0.1f, 0.005f);
                    }

                    Actor_set_scale(&koume->actor, koume->actor.scale.x);
                    koume->actor.shape.rot.y = -0x8000;
                    koume->unk_5F8 = 1;

                    if (this->work[CS_TIMER_1] == 0x64) {
                        this->csState2 = 10;
                        this->work[CS_TIMER_1] = 0;
                        this->subCamYawStep = 0.0f;
                        kotake->visible = 1;
                        Skeleton_Info2_init_standard_repeat_morf(&kotake->skelAnime, &gTwinrovaKotakeKoumeIdleLoopAnim, 0.0f);
                        kotake->actor.world.pos.x = 0.0f;
                        kotake->actor.world.pos.y = 80.0f;
                        kotake->actor.world.pos.z = -600.0f;
                        kotake->actor.shape.rot.y = kotake->actor.world.rot.y = 0;
                        this->work[CS_TIMER_1] = 0;

                        this->subCamEye.x = -30.0f;
                        this->subCamEye.y = 260.0f;
                        this->subCamEye.z = -470.0f;

                        this->subCamAt.x = 0;
                        this->subCamAt.y = 270.0f;
                        this->subCamAt.z = -600.0f;
                        Actor_set_scale(&kotake->actor, 0.014999999f);
                    }
                } else {
                    koume->actor.shape.rot.y += (s16)this->subCamYawStep;
                }
            } else {
                if ((this->work[CS_TIMER_1] % 8) == 0) {
                    Actor_SE_set(&koume->actor, NA_SE_EN_TWINROBA_ROLL);
                }

                koume->actor.shape.rot.y += (s16)this->subCamYawStep;
                add_calc2(&this->subCamYawStep, 12288.0f, 1.0f, 384.0f);

                if (Skeleton_Info_frame_check(&koume->skelAnime, this->workf[ANIM_SW_TGT])) {
                    Skeleton_Info2_init_standard_repeat_morf(&koume->skelAnime, &gTwinrovaKotakeKoumeFlyAnim, 0.0f);
                    this->workf[ANIM_SW_TGT] = 1000.0f;
                }
            }
            break;

        case 10:
            Skeleton_Info2_anime_play(&kotake->skelAnime);
            add_calc2(&kotake->actor.world.pos.y, 240.0f, 0.05f, 5.0f);
            this->subCamEye.x -= 0.2f;
            this->subCamEye.z -= 0.2f;

            if (this->work[CS_TIMER_1] >= 0x33) {
                this->csState2 = 11;
                this->subCamEyeNext.x = -30;
                this->subCamEyeNext.y = 260;
                this->subCamEyeNext.z = -530;
                this->subCamAtNext.x = 0;
                this->subCamAtNext.y = 265;
                this->subCamAtNext.z = -580;
                this->subCamEyeVel.x = fabsf(this->subCamEyeNext.x - this->subCamEye.x);
                this->subCamEyeVel.y = fabsf(this->subCamEyeNext.y - this->subCamEye.y);
                this->subCamEyeVel.z = fabsf(this->subCamEyeNext.z - this->subCamEye.z);
                this->subCamAtVel.x = fabsf(this->subCamAtNext.x - this->subCamAt.x);
                this->subCamAtVel.y = fabsf(this->subCamAtNext.y - this->subCamAt.y);
                this->subCamAtVel.z = fabsf(this->subCamAtNext.z - this->subCamAt.z);
                this->subCamUpdateRate = 0;
                this->subCamDistStep = 0.1f;
                this->work[CS_TIMER_1] = 0;
            }
            break;

        case 11:
            Skeleton_Info2_anime_play(&kotake->skelAnime);
            updateCam = true;
            add_calc2(&kotake->actor.world.pos.y, 240.0f, 0.05f, 5.0f);
            add_calc2(&this->subCamUpdateRate, 1.0f, 1.0f, 0.02f);

            if (this->work[CS_TIMER_1] == 30) {
                message_set(play, 0x604A, NULL);
            }

            if (this->work[CS_TIMER_1] > 80) {
                this->csState2 = 12;
                this->actor.speed = 0;

                this->subCamEyeNext.y = 260.0f;
                this->subCamEyeNext.x = -80.0f;
                this->subCamEyeNext.z = -430.0f;

                this->subCamAtNext.x = kotake->actor.world.pos.x;
                this->subCamAtNext.y = kotake->actor.world.pos.y + 20.0f;
                this->subCamAtNext.z = kotake->actor.world.pos.z;

                this->subCamEyeVel.x = fabsf(this->subCamEyeNext.x - this->subCamEye.x);
                this->subCamEyeVel.y = fabsf(this->subCamEyeNext.y - this->subCamEye.y);
                this->subCamEyeVel.z = fabsf(this->subCamEyeNext.z - this->subCamEye.z);
                this->subCamAtVel.x = fabsf(this->subCamAtNext.x - this->subCamAt.x);
                this->subCamAtVel.y = fabsf(this->subCamAtNext.y - this->subCamAt.y);
                this->subCamAtVel.z = fabsf(this->subCamAtNext.z - this->subCamAt.z);
                this->subCamUpdateRate = 0;
                this->subCamDistStep = 0.05f;
                Skeleton_Info2_init_standard_stop_morf(&kotake->skelAnime, &gTwinrovaKotakeKoumeIdleEndAnim, 0);
                this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaKotakeKoumeIdleEndAnim);
                this->work[CS_TIMER_1] = 0;
            }
            break;

        case 12:
            updateCam = true;
            Skeleton_Info2_anime_play(&kotake->skelAnime);
            this->subCamAtNext.y = kotake->actor.world.pos.y + 20.0f;
            add_calc2(&kotake->actor.world.pos.y, 350, 0.1f, this->actor.speed);
            add_calc2(&this->actor.speed, 9.0f, 1.0f, 0.9f);
            add_calc2(&this->subCamUpdateRate, 1.0f, 1.0f, 0.02f);

            if (this->work[CS_TIMER_1] >= 30) {
                if (this->work[CS_TIMER_1] < 45) {
                    play->envCtx.lightSetting = 3;
                    play->envCtx.lightBlend = 1.0f;
                } else {
                    add_calc0(&play->envCtx.lightBlend, 1.0f, 0.1f);
                }

                if (this->work[CS_TIMER_1] == 30) {
                    for (i = 0; i < 50; i++) {
                        Vec3f pos;
                        Vec3f velocity;
                        pos.x = kotake->actor.world.pos.x + rnd_fx(50.0f);
                        pos.y = kotake->actor.world.pos.y + rnd_fx(50.0f);
                        pos.z = kotake->actor.world.pos.z + rnd_fx(50.0f);
                        velocity.x = rnd_fx(20.0f);
                        velocity.y = rnd_fx(20.0f);
                        velocity.z = rnd_fx(20.0f);
                        tw_magic_eff_ct(play, &pos, &velocity, &zero, rnd_f(10.f) + 25.0f, 0);
                    }

                    Actor_SE_set(&kotake->actor, NA_SE_EN_TWINROBA_TRANSFORM);
                    play->envCtx.lightBlend = 0.0f;
                }

                if (this->work[CS_TIMER_1] >= 35) {
                    if (this->work[CS_TIMER_1] < 50) {
                        add_calc2(&kotake->actor.scale.x,
                                       ((sin_s(this->work[CS_TIMER_1] * 0x4200) * 20.0f) / 10000.0f) + 0.024999999f,
                                       1.0f, 0.005f);
                    } else {
                        if (this->work[CS_TIMER_1] == 50) {
                            Skeleton_Info2_init_standard_stop_morf(&kotake->skelAnime, &gTwinrovaKotakeKoumeLaughAnim, -5.0f);
                            this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaKotakeKoumeLaughAnim);
                        }

                        if (this->work[CS_TIMER_1] == 60) {
                            Actor_SE_set(&kotake->actor, NA_SE_EN_TWINROBA_LAUGH2);
                        }

                        if (Skeleton_Info_frame_check(&kotake->skelAnime, this->workf[ANIM_SW_TGT])) {
                            Skeleton_Info2_init_standard_repeat_morf(&kotake->skelAnime, &gTwinrovaKotakeKoumeFlyAnim, 0.0f);
                            this->workf[ANIM_SW_TGT] = 1000.0f;
                        }

                        add_calc2(&kotake->actor.scale.x, 0.024999999f, 0.1f, 0.005f);
                    }

                    Actor_set_scale(&kotake->actor, kotake->actor.scale.x);
                    kotake->actor.shape.rot.y = 0;
                    kotake->unk_5F8 = 1;

                    if (this->work[CS_TIMER_1] == 100) {
                        this->csState2 = 20;
                        this->work[CS_TIMER_1] = 0;

                        this->workf[UNK_F11] = 600.0f;

                        this->subCamEye.x = 800.0f;
                        this->subCamEye.y = 300.0f;
                        this->subCamEye.z = 0;

                        this->subCamAt.x = 0.0f;
                        this->subCamAt.y = 400.0f;
                        this->subCamAt.z = 0;

                        this->workf[UNK_F9] = -M_PI / 2.0f;
                        this->workf[UNK_F10] = 0.0f;

                        this->subCamEyeVel.x = 0.0f;
                        this->spawnPortalAlpha = 0.0f;
                    }
                } else {
                    kotake->actor.shape.rot.y += (s16)this->subCamYawStep;
                }
            } else {
                if ((this->work[CS_TIMER_1] % 8) == 0) {
                    Actor_SE_set(&kotake->actor, NA_SE_EN_TWINROBA_ROLL);
                }

                kotake->actor.shape.rot.y += (s16)this->subCamYawStep;
                add_calc2(&this->subCamYawStep, 12288.0f, 1.0f, 384.0f);

                if (Skeleton_Info_frame_check(&kotake->skelAnime, this->workf[ANIM_SW_TGT])) {
                    Skeleton_Info2_init_standard_repeat_morf(&kotake->skelAnime, &gTwinrovaKotakeKoumeFlyAnim, 0.0f);
                    this->workf[ANIM_SW_TGT] = 1000.0f;
                }
            }
            break;

        case 20:
            if (this->work[CS_TIMER_1] > 20 && this->work[CS_TIMER_1] < 120) {
                play->envCtx.lightSetting = 1;
                add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.015f);
            }

            if (this->work[CS_TIMER_1] == 90) {
                SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 90);
            }

            if (this->work[CS_TIMER_1] == 120) {
                kankyo_flag = 0;
                play->envCtx.prevLightSetting = 1;
                play->envCtx.lightSetting = 1;
                play->envCtx.lightBlend = 0.0f;
                Actor_Name_Disp_Set(play, &play->actorCtx.titleCtx, SEGMENTED_TO_VIRTUAL(gTwinrovaTitleCardTex), 160,
                                       180, 128, 40);
                SET_EVENTCHKINF(EVENTCHKINF_BEGAN_TWINROVA_BATTLE);
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS);
            }

            if (this->work[CS_TIMER_1] >= 160) {
                if (this->work[CS_TIMER_1] == 160) {
                    this->subCamEyeVel.x = 0.0f;
                }
                add_calc2(&this->subCamEye.x, 0.0f, 0.05f, this->subCamEyeVel.x * 0.5f);
                add_calc2(&this->subCamEye.z, 1000.0f, 0.05f, this->subCamEyeVel.x);
                add_calc2(&this->subCamEyeVel.x, 40.0f, 1.0f, 1);
            } else {
                add_calc2(&this->subCamEye.x, 300.0f, 0.05f, this->subCamEyeVel.x);
                add_calc2(&this->subCamEyeVel.x, 5.0f, 1.0f, 0.5f);
            }

            if (this->work[CS_TIMER_1] < 200) {
                Actor_SE_set(&koume->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
                Actor_SE_set(&kotake->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
                sp90.x = this->workf[UNK_F11];
                sp90.y = 400.0f;
                sp90.z = 0.0f;
                Matrix_rotateY(this->workf[UNK_F9], MTXMODE_NEW);
                Matrix_Position(&sp90, &sp84);
                koume->actor.world.pos.x = sp84.x;
                koume->actor.world.pos.y = sp84.y;
                koume->actor.world.pos.z = sp84.z;
                koume->actor.world.rot.y = koume->actor.shape.rot.y =
                    (this->workf[UNK_F9] / M_PI) * (f32)0x8000;
                kotake->actor.world.pos.x = -sp84.x;
                kotake->actor.world.pos.y = sp84.y;
                kotake->actor.world.pos.z = -sp84.z;
                kotake->actor.shape.rot.y = kotake->actor.world.rot.y =
                    ((this->workf[UNK_F9] / M_PI) * (f32)0x8000) + (f32)0x8000;
                add_calc2(&this->workf[UNK_F11], 80.0f, 0.1f, 5.0f);
                this->workf[UNK_F9] -= this->workf[UNK_F10];
                add_calc2(&this->workf[UNK_F10], 0.19999999f, 1.0f, 0.0019999994f);
            }

            if (this->work[CS_TIMER_1] == 200) {
                koume->actionFunc = mode_fly;
                kotake->actionFunc = mode_fly;
                koume->targetPos.x = 600.0f;
                koume->targetPos.y = 400.0f;
                koume->targetPos.z = 0.0f;
                koume->timers[0] = 100;
                kotake->targetPos.x = -600.0f;
                kotake->targetPos.y = 400.0f;
                kotake->targetPos.z = 0.0f;
                kotake->timers[0] = 100;
            }

            if (this->work[CS_TIMER_1] == 260) {
                Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                this->csState2 = this->subCamId;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                mode_nomove_init(this, play);
            }
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        if (updateCam) {
            add_calc2(&this->subCamEye.x, this->subCamEyeNext.x, this->subCamDistStep,
                           this->subCamEyeVel.x * this->subCamUpdateRate);
            add_calc2(&this->subCamEye.y, this->subCamEyeNext.y, this->subCamDistStep,
                           this->subCamEyeVel.y * this->subCamUpdateRate);
            add_calc2(&this->subCamEye.z, this->subCamEyeNext.z, this->subCamDistStep,
                           this->subCamEyeVel.z * this->subCamUpdateRate);
            add_calc2(&this->subCamAt.x, this->subCamAtNext.x, this->subCamDistStep,
                           this->subCamAtVel.x * this->subCamUpdateRate);
            add_calc2(&this->subCamAt.y, this->subCamAtNext.y, this->subCamDistStep,
                           this->subCamAtVel.y * this->subCamUpdateRate);
            add_calc2(&this->subCamAt.z, this->subCamAtNext.z, this->subCamDistStep,
                           this->subCamAtVel.z * this->subCamUpdateRate);
        }

        Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

void tw_tamasii_move(BossTw* this, PlayState* play) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    s32 pad;
    s16 i;
    s16 yaw;

    if ((this->work[CS_TIMER_1] % 16) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_FB_FLY);
    }

    if (gattai->csState2 < 2) {
        if (this->timers[0] == 0) {
            this->timers[0] = 20;
            this->targetPos.x = rnd_fx(100.0f) + gattai->actor.world.pos.x;
            this->targetPos.y = rnd_fx(50.0f) + 400.0f;
            this->targetPos.z = rnd_fx(100.0f) + gattai->actor.world.pos.z;
        }

        this->timers[1] = 10;
        this->rotateSpeed = 8192.0f;
        this->actor.speed = 5.0f;
    } else {
        if (this->timers[1] == 9) {
            this->targetPos.y = 413.0f;
            this->actor.world.pos.z = 0.0f;
            this->actor.world.pos.x = 0.0f;
            for (i = 0; i < ARRAY_COUNT(this->blastTailPos); i++) {
                this->blastTailPos[i] = this->actor.world.pos;
            }
        }

        if (this->actor.params == TW_DEATHBALL_KOUME) {
            this->targetPos.x = koume->actor.world.pos.x;
            this->targetPos.z = koume->actor.world.pos.z;
        } else {
            this->targetPos.x = kotake->actor.world.pos.x;
            this->targetPos.z = kotake->actor.world.pos.z;
        }

        add_calc2(&this->targetPos.y, 263.0f, 1.0f, 2.0f);

        if (this->targetPos.y == 263.0f) {
            add_calc2(&this->actor.speed, 0.0f, 1.0f, 0.2f);
            if (gattai->csState2 == 3) {
                Actor_delete(&this->actor);
            }
        }
    }

    xDiff = this->targetPos.x - this->actor.world.pos.x;
    yDiff = this->targetPos.y - this->actor.world.pos.y;
    zDiff = this->targetPos.z - this->actor.world.pos.z;

    yaw = RAD_TO_BINANG(fatan2(xDiff, zDiff));
    adds(&this->actor.world.rot.x, RAD_TO_BINANG(fatan2(yDiff, sqrtf(SQ(xDiff) + SQ(zDiff)))), 5,
                   this->rotateSpeed);
    adds(&this->actor.world.rot.y, yaw, 5, this->rotateSpeed);
    Actor_position_speed_set_XY(&this->actor);
    Actor_position_move(&this->actor);
}

void mode_g_enddemo_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_g_enddemo;
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaDamageAnim, -3.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->csState2 = this->csState1 = 0;
    this->work[CS_TIMER_1] = this->work[CS_TIMER_2] = 0;
    this->work[INVINC_TIMER] = 10000;
    mode_enddemo_init(koume, play);
    mode_enddemo_init(kotake, play);
    kotake->timers[0] = 8;
    this->workf[UNK_F19] = 1.0f;
}

void mess_anime_ct(BossTw* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    s32 pad3;
    s16 msgId2;
    s16 msgId1;
    u8 kotakeAnim;
    u8 koumeAnim;
    u8 sp35;

    msgId2 = 0;
    msgId1 = 0;
    kotakeAnim = 0;
    koumeAnim = 0;
    sp35 = 0;

    if (this->work[CS_TIMER_2] == 80) {
        koumeAnim = 1;
    }

    if (this->work[CS_TIMER_2] == 80) {
        msgId2 = 0x604B;
        sp35 = 50;
    }

    if (this->work[CS_TIMER_2] == 140) {
        kotakeAnim = koumeAnim = 2;
    }

    if (this->work[CS_TIMER_2] == 170) {
        kotakeAnim = 3;
        kotake->work[YAW_TGT] = -0x4000;
        kotake->rotateSpeed = 0.0f;
        Actor_SE_set(&kotake->actor, NA_SE_EN_TWINROBA_SENSE);
        msgId2 = 0x604C;
    }

    if (this->work[CS_TIMER_2] == 210) {
        message_se_time = 30;
    }

    if (this->work[CS_TIMER_2] == 270) {
        koumeAnim = 3;
        koume->work[YAW_TGT] = 0x4000;
        koume->rotateSpeed = 0.0f;
        Actor_SE_set(&koume->actor, NA_SE_EN_TWINROBA_SENSE);
    }

    if (this->work[CS_TIMER_2] == 290) {
        msgId2 = 0x604D;
        sp35 = 35;
    }

    if (this->work[CS_TIMER_2] == 350) {
        koumeAnim = kotakeAnim = 2;
        koume->work[YAW_TGT] = kotake->work[YAW_TGT] = 0;
        koume->rotateSpeed = kotake->rotateSpeed = 0.0f;
    }

    if (this->work[CS_TIMER_2] == 380) {
        koumeAnim = kotakeAnim = 3;
    }

    if (this->work[CS_TIMER_2] == 400) {
        koumeAnim = kotakeAnim = 2;
    }

    if (this->work[CS_TIMER_2] == 430) {
        koumeAnim = 4;
        message_se_time = 435;
        message_se_flag = 1;
    }

    if (this->work[CS_TIMER_2] > 440 && this->work[CS_TIMER_2] < 860) {
        Na_StartSystemSe_F(NA_SE_EN_TWINROBA_FIGHT - SFX_FLAG);
    }

    if (this->work[CS_TIMER_2] == 430) {
        msgId2 = 0x604E;
    }

    if (this->work[CS_TIMER_2] == 480) {
        kotakeAnim = 4;
        kotake->work[YAW_TGT] = -0x4000;
    }

    if (this->work[CS_TIMER_2] == 500) {
        koumeAnim = 2;
    }

    if (this->work[CS_TIMER_2] == 480) {
        msgId1 = 0x604F;
    }

    if (this->work[CS_TIMER_2] == 530) {
        koumeAnim = 4;
        koume->work[YAW_TGT] = 0x4000;
        message_se_time_2 = 335;
        message_se_flag_2 = 1;
    }

    if (this->work[CS_TIMER_2] == 530) {
        msgId2 = 0x6050;
    }

    if (this->work[CS_TIMER_2] == 580) {
        msgId1 = 0x6051;
    }

    if (this->work[CS_TIMER_2] == 620) {
        msgId2 = 0x6052;
    }

    if (this->work[CS_TIMER_2] == 660) {
        msgId1 = 0x6053;
    }

    if (this->work[CS_TIMER_2] == 700) {
        msgId2 = 0x6054;
    }

    if (this->work[CS_TIMER_2] == 740) {
        msgId1 = 0x6055;
    }

    if (this->work[CS_TIMER_2] == 780) {
        msgId2 = 0x6056;
    }

    if (this->work[CS_TIMER_2] == 820) {
        msgId1 = 0x6057;
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 80);
    }

    if (this->work[CS_TIMER_2] == 860) {
        koumeAnim = kotakeAnim = 3;
    }

    if (this->work[CS_TIMER_2] == 900) {
        Actor_SE_set(&koume->actor, NA_SE_EN_TWINROBA_DIE);
        Actor_SE_set(&kotake->actor, NA_SE_EN_TWINROBA_DIE);
    }

    if (this->work[CS_TIMER_2] == 930) {
        msgId2 = 0x6058;
    }

    if (msgId2 != 0) {
        message_set(play, msgId2, NULL);

        if (sp35) {
            message_se_time2 = 10;
            message_se_time = sp35;
            message_se_flag = 0;
        }
    }

    if (msgId1 != 0) {
        message_set(play, msgId1, NULL);
    }

    switch (kotakeAnim) {
        case 1:
            Skeleton_Info2_init_standard_repeat_morf(&kotake->skelAnime, &gTwinrovaKotakeKoumeShakeHandAnim, -5.0f);
            break;
        case 2:
            Skeleton_Info2_init_standard_repeat_morf(&kotake->skelAnime, &gTwinrovaKotakeKoumeFloatLookForwardAnim, -5.0f);
            break;
        case 3:
            Skeleton_Info2_init_standard_repeat_morf(&kotake->skelAnime, &gTwinrovaKotakeKoumeFloatLookUpAnim, -5.0f);
            break;
        case 4:
            Skeleton_Info2_init_standard_repeat_morf(&kotake->skelAnime, &gTwinrovaKotakeKoumeBickerAnim, -5.0f);
            break;
    }

    switch (koumeAnim) {
        case 1:
            Skeleton_Info2_init_standard_repeat_morf(&koume->skelAnime, &gTwinrovaKotakeKoumeShakeHandAnim, -5.0f);
            break;
        case 2:
            Skeleton_Info2_init_standard_repeat_morf(&koume->skelAnime, &gTwinrovaKotakeKoumeFloatLookForwardAnim, -5.0f);
            break;
        case 3:
            Skeleton_Info2_init_standard_repeat_morf(&koume->skelAnime, &gTwinrovaKotakeKoumeFloatLookUpAnim, -5.0f);
            break;
        case 4:
            Skeleton_Info2_init_standard_repeat_morf(&koume->skelAnime, &gTwinrovaKotakeKoumeBickerAnim, -5.0f);
            break;
    }

    if (this->work[CS_TIMER_2] >= 120 && this->work[CS_TIMER_2] < 500) {
        add_calc2(&this->workf[UNK_F18], 255.0f, 0.1f, 5.0f);
    }

    if (this->work[CS_TIMER_2] >= 150) {
        add_calc2(&koume->workf[UNK_F17], (sin_s(this->work[CS_TIMER_1] * 2000) * 0.05f) + 0.4f, 0.1f,
                       0.01f);
        add_calc2(&kotake->workf[UNK_F17], (cos_s(this->work[CS_TIMER_1] * 1700) * 0.05f) + 0.4f, 0.1f,
                       0.01f);

        if (this->work[CS_TIMER_2] >= 880) {
            add_calc2(&kotake->actor.world.pos.y, 2000.0f, 1.0f, this->actor.speed);
            add_calc2(&koume->actor.world.pos.y, 2000.0f, 1.0f, this->actor.speed);
            add_calc2(&this->actor.speed, 10.0f, 1.0f, 0.25f);

            if (this->work[CS_TIMER_2] >= 930) {
                add_calc2(&this->workf[UNK_F19], 5.0f, 1.0f, 0.05f);
                add_calc2(&this->workf[UNK_F18], 0.0f, 1.0f, 3.0f);
            }

            Actor_SE_set(&this->actor, NA_SE_EV_GOTO_HEAVEN - SFX_FLAG);
        } else {
            f32 yTarget = cos_s(this->work[CS_TIMER_2] * 1700) * 4.0f;
            add_calc2(&kotake->actor.world.pos.y, 20.0f + (263.0f + yTarget), 0.1f, this->actor.speed);
            yTarget = sin_s(this->work[CS_TIMER_2] * 1500) * 4.0f;
            add_calc2(&koume->actor.world.pos.y, 20.0f + (263.0f + yTarget), 0.1f, this->actor.speed);
            add_calc2(&this->actor.speed, 1.0f, 1.0f, 0.05f);
        }
    }
}

void mode_g_enddemo(BossTw* this, PlayState* play) {
    s16 i;
    Vec3f spD0;
    Player* player = GET_PLAYER(play);
    Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);

    Skeleton_Info2_anime_play(&this->skelAnime);
    this->work[UNK_S8] += 20;

    if (this->work[UNK_S8] > 255) {
        this->work[UNK_S8] = 255;
    }

    add_calc2(&this->workf[UNK_F12], 0.0f, 1.0f, 0.05f);
    this->unk_5F8 = 1;

    switch (this->csState1) {
        case 0:
            if (this->work[CS_TIMER_1] == 15) {
                Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaDeathAnim, -3.0f);
            }

            if (this->work[CS_TIMER_1] >= 15) {
                add_calc2(&this->actor.world.pos.y, 400.0f, 0.05f, 10.0f);
            }

            if (this->work[CS_TIMER_1] >= 55) {
                if (this->work[CS_TIMER_1] == 55) {
                    play->envCtx.lightBlend = 0;
                }

                kankyo_flag = -1;
                play->envCtx.prevLightSetting = 5;
                play->envCtx.lightSetting = 0;
                add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.015f);
                add_calc2(&this->actor.scale.x, 0.00024999998f, 0.1f, 0.00005f);
                this->actor.shape.rot.y += (s16)this->actor.speed;
                this->workf[UNK_F13] += this->actor.speed;
                if (this->workf[UNK_F13] > 65536.0f) {
                    this->workf[UNK_F13] -= 65536.0f;
                    Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_ROLL);
                }
                add_calc2(&this->actor.speed, 12288.0f, 1.0f, 256.0f);
                if (this->work[CS_TIMER_1] == 135) {
                    Vec3f spBC;
                    Vec3f spB0;
                    Vec3f spA4 = { 0.0f, 0.0f, 0.0f };
                    Na_StartSystemSe_F(NA_SE_EN_TWINROBA_TRANSFORM);
                    for (i = 0; i < 100; i++) {
                        spB0.x = rnd_fx(5.0f);
                        spB0.y = rnd_fx(5.0f);
                        spB0.z = rnd_fx(5.0f);
                        spBC = this->actor.world.pos;
                        spBC.x += spB0.x;
                        spBC.y += spB0.y;
                        spBC.z += spB0.z;
                        tw_magic_eff_ct(play, &spBC, &spB0, &spA4, rnd_f(2.0f) + 5,
                                              rnd_f(1.99f));
                    }
                    this->csState1 = 1;
                    this->visible = false;
                    this->actor.scale.x = 0.0f;
                    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_TW, this->actor.world.pos.x,
                                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, TW_DEATHBALL_KOUME);
                    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_TW, this->actor.world.pos.x,
                                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, TW_DEATHBALL_KOTAKE);
                    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                }
            }
            Actor_set_scale(&this->actor, this->actor.scale.x);
            break;
        case 1:
            break;
    }

    switch (this->csState2) {
        case 0:
            this->csState2 = 1;
            Demo_play_start(play, &play->csCtx);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            this->subCamId = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
            this->subCamEye = mainCam->eye;
            this->subCamAt = mainCam->at;
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
            break;
        case 1:
            spD0.x = sin_s(this->actor.world.rot.y) * 200.0f;
            spD0.z = cos_s(this->actor.world.rot.y) * 200.0f;
            add_calc2(&this->subCamEye.x, spD0.x + this->actor.world.pos.x, 0.1f, 50.0f);
            add_calc2(&this->subCamEye.y, 300.0f, 0.1f, 50.0f);
            add_calc2(&this->subCamEye.z, spD0.z + this->actor.world.pos.z, 0.1f, 50.0f);
            add_calc2(&this->subCamAt.x, this->actor.world.pos.x, 0.1f, 50.0f);
            add_calc2(&this->subCamAt.y, this->actor.world.pos.y, 0.1f, 50.0f);
            add_calc2(&this->subCamAt.z, this->actor.world.pos.z, 0.1f, 50.0f);
            if (this->work[CS_TIMER_1] == 170) {
                this->csState2 = 2;
                this->work[CS_TIMER_2] = 0;
                this->subCamEye.z = 170.0f;
                this->subCamDist = 170.0f;
                this->subCamEye.x = 0.0f;
                this->subCamAt.x = 0.0f;
                this->subCamAt.z = 0.0f;
                this->subCamEye.y = 260.0f;
                player->actor.shape.rot.y = -0x8000;
                player->actor.world.pos.x = -40.0f;
                player->actor.world.pos.y = 240.0f;
                player->actor.world.pos.z = 90.0f;
                koume->actor.world.pos.x = -37.0f;
                kotake->actor.world.pos.x = 37.0f;
                kotake->actor.world.pos.y = 263.0f;
                koume->actor.world.pos.y = kotake->actor.world.pos.y;
                this->subCamAt.y = koume->actor.world.pos.y + 17.0f;
                kotake->actor.world.pos.z = 0.0f;
                koume->actor.world.pos.z = kotake->actor.world.pos.z;
                koume->work[YAW_TGT] = kotake->work[YAW_TGT] = koume->actor.shape.rot.x =
                    kotake->actor.shape.rot.x = koume->actor.shape.rot.y = kotake->actor.shape.rot.y = 0;
                player_demo_mode_set(play, &koume->actor, PLAYER_CSACTION_1);
                koume->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            }
            break;
        case 2:
            if (this->work[CS_TIMER_2] == 100) {
                Vec3f pos;
                Vec3f velocity;
                Vec3f accel = { 0.0f, 0.0f, 0.0f };
                s32 zero = 0;

                for (i = 0; i < 50; i++) {
                    velocity.x = rnd_fx(3.0f);
                    velocity.y = rnd_fx(3.0f);
                    velocity.z = rnd_fx(3.0f);
                    pos = koume->actor.world.pos;
                    pos.x += velocity.x * 2.0f;
                    pos.y += velocity.y * 2.0f;
                    pos.z += velocity.z * 2.0f;
                    tw_magic_eff_ct(play, &pos, &velocity, &accel, rnd_f(2.0f) + 5, 1);

                    // fake code needed to match, tricks the compiler into allocating more stack
                    if (1) {}
                    if (zero) {
                        accel.x *= 2.0;
                    }

                    velocity.x = rnd_fx(3.0f);
                    velocity.y = rnd_fx(3.0f);
                    velocity.z = rnd_fx(3.0f);
                    pos = kotake->actor.world.pos;
                    pos.x += velocity.x * 2.0f;
                    pos.y += velocity.y * 2.0f;
                    pos.z += velocity.z * 2.0f;
                    tw_magic_eff_ct(play, &pos, &velocity, &accel, rnd_f(2.0f) + 5, 0);
                }

                Actor_set_scale(&koume->actor, 0.0f);
                Actor_set_scale(&kotake->actor, 0.0f);
                koume->visible = 1;
                kotake->visible = 1;
                Na_StartSystemSe_F(NA_SE_EN_TWINROBA_TRANSFORM);
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_KOTAKE_KOUME);
                this->csState2 = 3;
                this->work[CS_TIMER_2] = 0;
                this->subCamYaw = this->subCamYawStep = this->actor.speed = this->subCamDistStep = 0.0f;
            }
            break;
        case 3:
            mess_anime_ct(this, play);
            if (this->work[CS_TIMER_2] < 150) {
                play->envCtx.prevLightSetting = 1;
                play->envCtx.lightSetting = 0;
                add_calc0(&play->envCtx.lightBlend, 1.0f, 0.1f);
            } else {
                play->envCtx.prevLightSetting = 1;
                play->envCtx.lightSetting = 6;
                add_calc2(&play->envCtx.lightBlend, (sin_s(this->work[CS_TIMER_2] * 4096) / 4.0f) + 0.75f,
                               1.0f, 0.1f);
            }

            add_calc2(&this->subCamAt.y, koume->actor.world.pos.y + 17.0f, 0.05f, 10.0f);

            if (this->work[CS_TIMER_2] >= 50) {
                add_calc2(&this->subCamDist, 110.0f, 0.05f, this->subCamDistStep);
                add_calc2(&this->subCamDistStep, 1.0f, 1.0f, 0.025f);
                this->subCamEye.x = this->subCamDist * sinf(this->subCamYaw);
                this->subCamEye.z = this->subCamDist * cosf(this->subCamYaw);
                if (this->work[CS_TIMER_2] >= 151) {
                    this->subCamYaw += this->subCamYawStep;
                    if (this->work[CS_TIMER_2] >= 800) {
                        add_calc2(&this->subCamYawStep, 0.0f, 1.0f, 0.0001f);
                    } else {
                        add_calc2(&this->subCamYawStep, 0.015f, 1.0f, 0.0001f);
                    }
                }
            }
            add_calc2(&koume->actor.scale.x, 0.009999999f, 0.1f, 0.001f);
            Actor_set_scale(&koume->actor, koume->actor.scale.x);
            Actor_set_scale(&kotake->actor, koume->actor.scale.x);
            if (this->work[CS_TIMER_2] >= 1020) {
                mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                Gama_play_shift2main_camera(play, this->subCamId, 0);
                this->csState2 = 4;
                this->subCamId = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS_CLEAR);
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, 600.0f, 230.0f, 0.0f, 0, 0, 0,
                                   WARP_DUNGEON_ADULT);
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, -600.0f, 230.f, 0.0f, 0, 0, 0, 0);
                this->actor.world.pos.y = -2000.0f;
                this->workf[UNK_F18] = 0.0f;
                koume->visible = kotake->visible = false;
                if (&this->subCamEye) {} // fixes regalloc, may be fake
                Actor_Environment_room_clear_On(play, play->roomCtx.curRoom.num);
            }
            break;
        case 4:
            kankyo_flag = 0;
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        if (1) {}
        Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

static s16 tw_eye_pt[] = {
    0, 1, 2, 2, 1,
};

static s16 tw_eye_pt2[] = {
    0, 1, 2, 2, 2, 2, 2, 2, 1,
};

void Boss_Tw_actor_move(Actor* thisx, PlayState* play) {
    BossTw* this = (BossTw*)thisx;
    Player* player = GET_PLAYER(play);
    s16 i;
    s32 pad;

    this->collider.base.colMaterial = COL_MATERIAL_HIT3;
    add_calc2(&this->fogR, play->lightCtx.fogColor[0], 1.0f, 10.0f);
    add_calc2(&this->fogG, play->lightCtx.fogColor[1], 1.0f, 10.0f);
    add_calc2(&this->fogB, play->lightCtx.fogColor[2], 1.0f, 10.0f);
    add_calc2(&this->fogNear, play->lightCtx.fogNear, 1.0f, 10.0f);
    add_calc2(&this->fogFar, 1000.0f, 1.0f, 10.0f);
    this->work[CS_TIMER_1]++;
    this->work[CS_TIMER_2]++;
    this->work[TAIL_IDX]++;

    if (this->work[TAIL_IDX] >= ARRAY_COUNT(this->blastTailPos)) {
        this->work[TAIL_IDX] = 0;
    }

    this->blastTailPos[this->work[TAIL_IDX]] = this->actor.world.pos;

    for (i = 0; i < 5; i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }

    if (this->work[INVINC_TIMER] != 0) {
        this->work[INVINC_TIMER]--;
    }

    if (this->work[FOG_TIMER] != 0) {
        this->work[FOG_TIMER]--;
    }

    if (this->actionFunc == mode_fly || this->actionFunc == mode_guard ||
        this->actionFunc == mode_fly_wait) {
        if ((s16)(player->actor.shape.rot.y - this->actor.yawTowardsPlayer + 0x8000) < 0x1000 &&
            (s16)(player->actor.shape.rot.y - this->actor.yawTowardsPlayer + 0x8000) > -0x1000 && player->unk_A73) {
            mode_guard_init(this, play);
        }
    }

    this->actionFunc(this, play);

    if (this->actionFunc == mode_nomove) {
        return;
    }

    this->collider.dim.radius = 45;

    if (this->actionFunc == mode_guard) {
        this->collider.dim.radius *= 2;
    }

    this->collider.dim.height = 120;
    this->collider.dim.yShift = -30;

    if (this->work[INVINC_TIMER] == 0) {
        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
        }

        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->actor.params == TW_KOTAKE) {
        this->workf[OUTR_CRWN_TX_X2] += 1.0f;
        this->workf[OUTR_CRWN_TX_Y2] -= 7.0f;
        this->workf[INNR_CRWN_TX_Y1] += 1.0f;
    } else {
        this->workf[OUTR_CRWN_TX_X2] += 0.0f;
        this->workf[INNR_CRWN_TX_X2] += 0.0f;
        this->workf[OUTR_CRWN_TX_Y2] += -15.0f;
        this->workf[INNR_CRWN_TX_Y2] += -10.0f;
    }

    if (((this->work[CS_TIMER_2] % 32) == 0) && (fqrand() < 0.3f)) {
        this->work[BLINK_IDX] = 4;
    }

    this->eyeTexIdx = tw_eye_pt[this->work[BLINK_IDX]];

    if (this->work[BLINK_IDX] != 0) {
        this->work[BLINK_IDX]--;
    }

    if ((this->actionFunc == mode_gattai) || (this->unk_5F8 == 0)) {
        return;
    }

    {
        Vec3f pos;
        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
        Vec3f accel = { 0.0f, 0.0f, 0.0f };

        if (this->scepterAlpha > 0.0f) {
            for (i = 0; i <= 0; i++) {
                pos = this->scepterFlamePos[0];
                pos.x += rnd_fx(70.0f);
                pos.y += rnd_fx(70.0f);
                pos.z += rnd_fx(70.0f);
                accel.y = 0.4f;
                accel.x = rnd_fx(0.5f);
                accel.z = rnd_fx(0.5f);
                tw_hinoko_ct(play, &pos, &velocity, &accel, (s16)rnd_f(2.0f) + 8, this->actor.params,
                                    37);
            }
        }

        for (i = 0; i <= 0; i++) {
            pos = this->crownPos;
            pos.x += rnd_fx(70.0f);
            pos.y += rnd_fx(70.0f);
            pos.z += rnd_fx(70.0f);
            accel.y = 0.4f;
            accel.x = rnd_fx(0.5f);
            accel.z = rnd_fx(0.5f);
            tw_hinoko_ct(play, &pos, &velocity, &accel, (s16)rnd_f(2.0f) + 8, this->actor.params, 37);
        }
    }
}

void tw_gattai_move(Actor* thisx, PlayState* play2) {
    s16 i;
    PlayState* play = play2;
    BossTw* this = (BossTw*)thisx;
    Player* player = GET_PLAYER(play);

    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    this->unk_5F8 = 0;
    this->collider.base.colMaterial = COL_MATERIAL_HIT3;

    add_calc2(&this->fogR, play->lightCtx.fogColor[0], 1.0f, 10.0f);
    add_calc2(&this->fogG, play->lightCtx.fogColor[1], 1.0f, 10.0f);
    add_calc2(&this->fogB, play->lightCtx.fogColor[2], 1.0f, 10.0f);
    add_calc2(&this->fogNear, play->lightCtx.fogNear, 1.0f, 10.0f);
    add_calc2(&this->fogFar, 1000.0f, 1.0f, 10.0f);

    this->work[CS_TIMER_1]++;
    this->work[CS_TIMER_2]++;

    for (i = 0; i < 5; i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }

    if (this->work[INVINC_TIMER] != 0) {
        this->work[INVINC_TIMER]--;
    }

    if (this->work[FOG_TIMER] != 0) {
        this->work[FOG_TIMER]--;
    }

    this->actionFunc(this, play);

    if (this->actionFunc != mode_g_atack && this->actionFunc != mode_g_tame &&
        this->visible && this->unk_5F8 == 0 &&
        (s16)(player->actor.shape.rot.y - this->actor.yawTowardsPlayer + 0x8000) < 0x1000 &&
        (s16)(player->actor.shape.rot.y - this->actor.yawTowardsPlayer + 0x8000) > -0x1000 && player->unk_A73 != 0) {
        mode_g_guard_init(this, play);
    }

    this->eyeTexIdx = tw_eye_pt[this->work[BLINK_IDX]];
    if (this->work[BLINK_IDX] != 0) {
        this->work[BLINK_IDX]--;
    }

    if ((this->work[CS_TIMER_2] % 32) == 0) {
        if (this->actionFunc != mode_g_gattai) {
            if (fqrand() < 0.3f) {
                this->work[BLINK_IDX] = 4;
            }
        }
    }

    if (this->actionFunc == mode_g_gattai) {
        this->leftEyeTexIdx = tw_eye_pt2[this->work[TW_BLINK_IDX]];
        if (this->work[TW_BLINK_IDX] != 0) {
            this->work[TW_BLINK_IDX]--;
        }
    } else {
        if (this->actionFunc == mode_g_damage) {
            this->eyeTexIdx = 1;
        }

        if (this->actionFunc == mode_g_enddemo) {
            this->eyeTexIdx = 2;
        }

        this->leftEyeTexIdx = this->eyeTexIdx;
    }

    if (this->visible && this->unk_5F8 == 0) {
        Vec3f pos;
        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
        Vec3f accel;

        if (this->work[UNK_S8] != 0) {
            this->work[UNK_S8] -= 20;
            if (this->work[UNK_S8] < 0) {
                this->work[UNK_S8] = 0;
            }
        }

        add_calc2(&this->workf[UNK_F12], 1.0f, 1.0f, 0.05f);
        accel.y = 0.4f;

        for (i = 0; i < 2; i++) {
            pos = this->leftScepterPos;
            pos.x += rnd_fx(30.0f);
            pos.y += rnd_fx(30.0f);
            pos.z += rnd_fx(30.0f);
            accel.x = rnd_fx(0.5f);
            accel.z = rnd_fx(0.5f);
            tw_hinoko_ct(play, &pos, &velocity, &accel, (s16)rnd_f(2.0f) + 7, 0, 75);
        }

        for (i = 0; i < 2; i++) {
            pos = this->rightScepterPos;
            pos.x += rnd_fx(30.0f);
            pos.y += rnd_fx(30.0f);
            pos.z += rnd_fx(30.0f);
            accel.x = rnd_fx(0.5f);
            accel.z = rnd_fx(0.5f);
            tw_hinoko_ct(play, &pos, &velocity, &accel, (s16)rnd_f(2.0f) + 7, 1, 75);
        }
    }

    this->collider.dim.radius = 35;

    if (this->actionFunc == mode_g_guard) {
        this->collider.dim.radius *= 2;
    }

    this->collider.dim.height = 150;
    this->collider.dim.yShift = -60;
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

    if (this->work[INVINC_TIMER] == 0) {
        if (this->actionFunc != mode_g_damage) {
            if (this->twinrovaStun != 0) {
                this->twinrovaStun = 0;
                this->work[FOG_TIMER] = 10;
                mode_g_damage_init(this, play, 0);
                Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_YOUNG_DAMAGE);
            } else if (this->collider.base.acFlags & AC_HIT) {
                ColliderElement* acHitElem = this->collider.elem.acHitElem;

                this->collider.base.acFlags &= ~AC_HIT;
                if (acHitElem->atDmgInfo.dmgFlags & (DMG_SLINGSHOT | DMG_ARROW)) {}
            }
        } else if (this->collider.base.acFlags & AC_HIT) {
            u8 damage;
            u8 swordDamage;
            ColliderElement* acHitElem = this->collider.elem.acHitElem;

            this->collider.base.acFlags &= ~AC_HIT;
            swordDamage = false;
            damage = GetSwordAP(acHitElem->atDmgInfo.dmgFlags);

            if (damage == 0) {
                damage = 2;
            } else {
                swordDamage = true;
            }

            if (!(acHitElem->atDmgInfo.dmgFlags & DMG_HOOKSHOT)) {
                if (((s8)this->actor.colChkInfo.health < 3) && !swordDamage) {
                    damage = 0;
                }

                mode_g_damage_init(this, play, damage);
            }
        }
    }

    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    PRINTF("OooooooooooooooooooooooooooooooooCC\n");
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

    play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_FULL_CONTROL;

    switch (kankyo_flag) {
        case 0:
            add_calc0(&play->envCtx.lightBlend, 1.0f, 0.02f);
            break;
        case 1:
            play->envCtx.lightSetting = 3;
            add_calc2(&play->envCtx.lightBlend, 0.5f, 1.0f, 0.05f);
            break;
        case 2:
            play->envCtx.lightSetting = 2;
            add_calc2(&play->envCtx.lightBlend, (sin_s(this->work[CS_TIMER_1] * 0x3000) * 0.03f) + 0.5f, 1.0f,
                           0.05f);
            break;
        case 3:
            play->envCtx.lightSetting = 3;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.1f);
            break;
        case 4:
            play->envCtx.lightSetting = 2;
            add_calc2(&play->envCtx.lightBlend, (sin_s(this->work[CS_TIMER_1] * 0x3E00) * 0.05f) + 0.95f, 1.0f,
                           0.1f);
            break;
        case 5:
            play->envCtx.lightSetting = 0;
            add_calc2(&play->envCtx.lightBlend, 1.0f, 1.0f, 0.05f);
            break;
        case -1:
            break;
    }

    Tw_Eff_move(play);

    if (pl_ice_flag == 1) {
        pl_ice_flag = 2;
        tw_pl_ice_ct(play, NULL);
        Na_StartObjectSe_F(&player->actor.projectedPos, NA_SE_VO_LI_FREEZE);
        Na_StartObjectSe_F(&player->actor.projectedPos, NA_SE_PL_FREEZE);

        if (pl_shield_charge_UME != 0) {
            pl_shield_charge_UME = 4;
        }
    }

    if (player->bodyIsBurning && pl_shield_charge_TAKE != 0) {
        pl_shield_charge_TAKE = 4;
    }
}

s32 tw_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossTw* this = (BossTw*)thisx;

    if (limbIndex == TWINROVA_KOTAKE_KOUME_LIMB_HEAD) {
        if (this->unk_5F8 == 0) {
            if (this->actor.params == TW_KOTAKE) {
                *dList = gTwinrovaKotakeHeadDL;
            } else {
                *dList = gTwinrovaKoumeHeadDL;
            }
        }
    }

    if (limbIndex == TWINROVA_KOTAKE_KOUME_LIMB_BROOM) {
        if (this->actionFunc == mode_enddemo) {
            *dList = NULL;
        } else if (this->scepterAlpha == 0.0f) {
            if (this->actor.params == TW_KOTAKE) {
                *dList = gTwinrovaKotakeBroomDL;
            } else {
                *dList = gTwinrovaKoumeBroomDL;
            }
        }
    }

    return false;
}

void tw_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f hand_p = { 0.0f, 0.0f, 0.0f };
    static Vec3f head_p = { 0.0f, 2000.0f, -2000.0f };
    static Vec3f tail_p[] = {
        { 0.0f, 0.0f, -10000.0f }, { 0.0f, 0.0f, -8000.0f },  { 0.0f, 0.0f, -9000.0f },
        { 0.0f, 0.0f, -11000.0f }, { 0.0f, 0.0f, -12000.0f },
    };
    BossTw* this = (BossTw*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6168);

    switch (limbIndex) {
        case TWINROVA_KOTAKE_KOUME_LIMB_HEAD:
            Matrix_Position(&hand_p, &this->actor.focus.pos);
            Matrix_Position(&head_p, &this->crownPos);

            if (this->unk_5F8 != 0) {
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6190);
                if (this->actor.params == TW_KOTAKE) {
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaKotakeIceHairDL));
                } else {
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaKoumeFireHairDL));
                }
            }
            break;

        case TWINROVA_KOTAKE_KOUME_LIMB_BROOM:
            Matrix_Position(&tail_p[0], &this->scepterFlamePos[0]);
            Matrix_Position(&tail_p[1], &this->scepterFlamePos[1]);
            Matrix_Position(&tail_p[2], &this->scepterFlamePos[2]);
            Matrix_Position(&tail_p[3], &this->scepterFlamePos[3]);
            Matrix_Position(&tail_p[4], &this->scepterFlamePos[4]);

            if (this->scepterAlpha > 0.0f) {
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6221);
                if (this->actor.params == TW_KOTAKE) {
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 225, 255, (s16)this->scepterAlpha);
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaKotakeIceBroomHeadDL));
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (s16)this->scepterAlpha);
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaKotakeIceBroomHeadOuterDL));
                } else {
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 20, 0, (s16)this->scepterAlpha);
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaKotakeFireBroomHeadDL));
                    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 70, 0, (s16)this->scepterAlpha);
                    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaKotakeFireBroomHeadOuterDL));
                }
            }
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6236);
}

void ice_floor_disp(BossTw* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6341);

    Matrix_push();
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Matrix_translate(this->groundBlastPos2.x, this->groundBlastPos2.y, this->groundBlastPos2.z, MTXMODE_NEW);
    Matrix_scale(this->workf[UNK_F12], this->workf[UNK_F12], this->workf[UNK_F12], MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6358);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s16)this->workf[UNK_F11]);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 40, 30, 80);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIcePoolDL));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 215, 215, 215, (s16)this->workf[UNK_F11] * this->workf[UNK_F14]);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);
    gSPSegment(POLY_XLU_DISP++, 8,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1,
                                (u32)this->workf[UNK_F16] & 0x3F, (this->work[CS_TIMER_2] * 4) & 0x3F, 0x10, 0x10));
    Matrix_push();
    Matrix_rotateY(this->workf[UNK_F15], MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6423);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIcePoolShineDL));
    Matrix_pull();
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6427);
    gSPSegment(POLY_XLU_DISP++, 0xD,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, this->work[CS_TIMER_2] & 0x7F,
                                (this->work[CS_TIMER_2] * 8) & 0xFF, 0x20, 0x40, 1,
                                (-this->work[CS_TIMER_2] * 2) & 0x3F, 0, 0x10, 0x10));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (s16)this->workf[UNK_F9]);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);
    gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_ZB_OVL_SURF2);
    gSPSetGeometryMode(POLY_XLU_DISP++, G_CULL_BACK | G_FOG);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaEffectHaloDL));
    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6461);
}

void fire_floor_disp(BossTw* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6468);

    Matrix_push();

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Matrix_translate(this->groundBlastPos2.x, this->groundBlastPos2.y, this->groundBlastPos2.z, MTXMODE_NEW);
    Matrix_scale(this->workf[KM_GD_CRTR_SCL], this->workf[KM_GD_CRTR_SCL], this->workf[KM_GD_CRTR_SCL], MTXMODE_APPLY);
    gSPSegment(POLY_XLU_DISP++, 8,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (-this->work[CS_TIMER_1]) & 0x7F, 0, 0x20, 0x20, 1,
                                (this->work[CS_TIMER_1] * 2) & 0x7F, 0, 0x20, 0x20));
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6497);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 40, 00, (s16)this->workf[KM_GRND_CRTR_A]);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 245, 255, 128);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaFirePoolDL));

    Matrix_rotate_scale_exchange(&play->billboardMtxF);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6514);
    gSPSegment(POLY_XLU_DISP++, 8,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, this->work[CS_TIMER_1] & 0x7F,
                                (-this->work[CS_TIMER_1] * 6) & 0xFF, 0x20, 0x40, 1,
                                (this->work[CS_TIMER_1] * 2) & 0x7F, (-this->work[CS_TIMER_1] * 6) & 0xFF, 0x20, 0x40));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 80, 0, 0, (s16)this->workf[KM_GD_SMOKE_A]);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 100);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaFireSmokeDL));

    gSPSegment(POLY_XLU_DISP++, 8,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (-this->work[CS_TIMER_1] * 3) & 0x7F, 0, 0x20,
                                0x20, 1, 0, (-this->work[CS_TIMER_1] * 10) & 0xFF, 0x20, 0x40));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 50, 0, (s16)(this->workf[KM_GD_FLM_A] * 0.7f));
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetEnvColor(POLY_XLU_DISP++, 200, 235, 240, 128);
    Matrix_scale(this->workf[KM_GD_FLM_SCL], this->workf[KM_GD_FLM_SCL], this->workf[KM_GD_FLM_SCL], MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6575);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaBigFlameDL));

    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6579);
}

void magic_disp(BossTw* this, PlayState* play) {
    s32 pad;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6587);

    gSPSegment(POLY_XLU_DISP++, 8,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (u8)(-this->work[CS_TIMER_2] * 15), 0x20, 0x40,
                                1, 0, 0, 0x40, 0x40));
    Matrix_push();
    Matrix_translate(0.0f, 0.0f, 5000.0f, MTXMODE_APPLY);
    Matrix_scale(this->spawnPortalScale / 2000.0f, this->spawnPortalScale / 2000.0f, this->spawnPortalScale / 2000.0f,
                 MTXMODE_APPLY);
    Matrix_rotateZ(this->portalRotation, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6614);

    if (this->actor.params == TW_KOTAKE) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 135, 175, 165, (s16)this->spawnPortalAlpha);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaKotakeMagicSigilDL));
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, (s16)this->spawnPortalAlpha);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaKoumeMagicSigilDL));
    }

    Matrix_pull();

    if (this->actor.params == TW_KOTAKE) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (s16)this->flameAlpha);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIceMaterialDL));
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, (s16)this->flameAlpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);
    }

    for (i = 0; i < 8; i++) {
        Matrix_push();
        Matrix_translate(0.0f, 0.0f, 5000.0f, MTXMODE_APPLY);
        Matrix_rotateZ(((i * M_PI) * 2.0f * 0.125f) + this->flameRotation, MTXMODE_APPLY);
        Matrix_translate(0.0f, this->spawnPortalScale * 1.5f, 0.0f, MTXMODE_APPLY);
        gSPSegment(POLY_XLU_DISP++, 8,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE,
                                    ((this->work[CS_TIMER_2] * 3) + (i * 10)) & 0x7F,
                                    (u8)((-this->work[CS_TIMER_2] * 15) + (i * 50)), 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
        Matrix_scale(0.4f, 0.4f, 0.4f, MTXMODE_APPLY);
        Matrix_rotate_scale_exchange(&play->billboardMtxF);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6751);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaFireDL));
        Matrix_pull();
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6756);
}

void beam_disp(Actor* thisx, PlayState* play) {
    BossTw* this = (BossTw*)thisx;
    s16 alpha;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6765);

    if (this->beamDist == 0.0f) {
        goto close_disps;
    }

    Matrix_push();
    gSPSegment(POLY_XLU_DISP++, 0xC,
               tex_scroll2(play->state.gfxCtx, 0, (u8)(this->work[CS_TIMER_1] * -0xF), 0x20, 0x40));
    alpha = this->beamScale * 100.0f * 255.0f;

    if (this->actor.params == TW_KOUME) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 60, alpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 128);
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, alpha);
        gDPSetEnvColor(POLY_XLU_DISP++, 100, 100, 255, 128);
    }

    Matrix_translate(this->beamOrigin.x, this->beamOrigin.y, this->beamOrigin.z, MTXMODE_NEW);
    Matrix_rotateY(this->beamYaw, MTXMODE_APPLY);
    Matrix_rotateX(this->beamPitch, MTXMODE_APPLY);
    Matrix_rotateZ(this->beamRoll, MTXMODE_APPLY);
    Matrix_scale(this->beamScale, this->beamScale, (this->beamDist * 0.01f * 98.0f) / 20000.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6846);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaBeamDL));

    if (this->beamReflectionDist > 10.0f) {
        Matrix_translate(this->beamReflectionOrigin.x, this->beamReflectionOrigin.y, this->beamReflectionOrigin.z,
                         MTXMODE_NEW);
        Matrix_rotateY(this->beamReflectionYaw, MTXMODE_APPLY);
        Matrix_rotateX(this->beamReflectionPitch, MTXMODE_APPLY);
        Matrix_rotateZ(this->beamRoll, MTXMODE_APPLY);
        Matrix_scale(this->beamScale, this->beamScale, (this->beamReflectionDist * 0.01f * 100.0f) / 20000.0f,
                     MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6870);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaBeamDL));
    }

    Matrix_pull();

close_disps:
    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6878);
}

void angelring_draw(Actor* thisx, PlayState* play) {
    BossTw* this = (BossTw*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6885);

    Matrix_push();
    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y + 57.0f, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_scale(this->workf[UNK_F17], this->workf[UNK_F17], this->workf[UNK_F17], MTXMODE_APPLY);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, 255);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6908);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaHaloDL));
    _texture_decal_shadow(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 200);
    Matrix_translate(this->actor.world.pos.x, 240.0f, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_scale((this->actor.scale.x * 4000.0f) / 100.0f, 1.0f, (this->actor.scale.x * 4000.0f) / 100.0f,
                 MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 6926);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gCircleShadowDL));
    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6933);
}

static void* eye_tex_no[] = {
    gTwinrovaKotakeKoumeEyeOpenTex,
    gTwinrovaKotakeKoumeEyeHalfTex,
    gTwinrovaKotakeKoumeEyeClosedTex,
};

void Boss_Tw_actor_draw(Actor* thisx, PlayState* play2) {
    static Vec3f magic_p = { 0.0f, 200.0f, 2000.0f };
    PlayState* play = play2;
    BossTw* this = (BossTw*)thisx;
    Player* player = GET_PLAYER(play);

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 6947);

    if (this->visible) {
        gSPSegment(POLY_OPA_DISP++, 10, SEGMENTED_TO_VIRTUAL(eye_tex_no[this->eyeTexIdx]));
        gSPSegment(POLY_XLU_DISP++, 10, SEGMENTED_TO_VIRTUAL(eye_tex_no[this->eyeTexIdx]));
        gSPSegment(POLY_XLU_DISP++, 8,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (s16)this->workf[OUTR_CRWN_TX_X1] & 0x7F,
                                    (s16)this->workf[OUTR_CRWN_TX_Y1] & 0x7F, 0x20, 0x20, 1,
                                    (s16)this->workf[OUTR_CRWN_TX_X2] & 0x7F, (s16)this->workf[OUTR_CRWN_TX_Y2] & 0xFF,
                                    0x20, 0x40));

        if (this->actor.params == TW_KOTAKE) {
            gSPSegment(POLY_XLU_DISP++, 9,
                       tex_scroll2(play->state.gfxCtx, (s16)this->workf[INNR_CRWN_TX_X1] & 0x7F,
                                     (s16)this->workf[INNR_CRWN_TX_Y1] & 0xFF, 0x20, 0x40));
        } else {
            gSPSegment(POLY_XLU_DISP++, 9,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (s16)this->workf[INNR_CRWN_TX_X1] & 0x7F,
                                        (s16)this->workf[INNR_CRWN_TX_Y1] & 0x7F, 0x20, 0x20, 1,
                                        (s16)this->workf[INNR_CRWN_TX_X2] & 0x7F,
                                        (s16)this->workf[INNR_CRWN_TX_Y2] & 0xFF, 0x20, 0x40));
        }

        _texture_z_light_fog_prim(play->state.gfxCtx);
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

        if (this->work[FOG_TIMER] & 2) {
            POLY_OPA_DISP = gfx_set_fog_nosync(POLY_OPA_DISP, 255, 50, 0, 0, 900, 1099);
        } else {
            POLY_OPA_DISP = gfx_set_fog_nosync(POLY_OPA_DISP, (u32)this->fogR, (u32)this->fogG, (u32)this->fogB, 0,
                                       this->fogNear, this->fogFar);
        }

        Matrix_push();
        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              tw_draw_sub, tw_draw_sub2, this);
        Matrix_pull();
        POLY_OPA_DISP = game_play_set_fog(play, POLY_OPA_DISP);
    }

    if (this->actor.params == TW_KOTAKE) {
        Vec3f diff;

        if (this->workf[UNK_F9] > 0.0f) {
            if (this->workf[UNK_F11] > 0.0f) {
                diff.x = this->groundBlastPos2.x - player->actor.world.pos.x;
                diff.y = this->groundBlastPos2.y - player->actor.world.pos.y;
                diff.z = this->groundBlastPos2.z - player->actor.world.pos.z;

                if ((fabsf(diff.y) < 10.0f) && (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
                    (sqrtf(SQ(diff.x) + SQ(diff.z)) < (this->workf[UNK_F12] * 4600.0f)) && (pl_ice_flag == 0) &&
                    (this->workf[UNK_F11] > 200.0f)) {
                    pl_ice_flag = 1;
                    gattai->timers[2] = 100;
                }
            }

            ice_floor_disp(this, play);
        }
    } else {
        fire_floor_disp(this, play);
    }

    if (this->visible) {
        if (this->actionFunc == mode_enddemo) {
            angelring_draw(&this->actor, play);
        } else {
            magic_disp(this, play);
            Matrix_Position(&magic_p, &this->beamOrigin);
            beam_disp(&this->actor, play);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7123);
}

void* g_eye_tex_no[] = {
    gTwinrovaEyeOpenTex,
    gTwinrovaEyeHalfTex,
    gTwinrovaEyeClosedTex,
};

s32 twW_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossTw* this = (BossTw*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7139);

    switch (limbIndex) {
        case TWINROVA_LIMB_HEAD:
            gSPSegment(POLY_OPA_DISP++, 0xC,
                       tex_scroll2(play->state.gfxCtx, 0, (s16)(f32)this->work[CS_TIMER_1], 8, 8));
            gSPSegment(POLY_OPA_DISP++, 8, SEGMENTED_TO_VIRTUAL(g_eye_tex_no[this->eyeTexIdx]));
            gSPSegment(POLY_OPA_DISP++, 9, SEGMENTED_TO_VIRTUAL(g_eye_tex_no[this->leftEyeTexIdx]));
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, this->work[UNK_S8]);
            break;
        case TWINROVA_LIMB_HAIR_FIRE_JET:
        case TWINROVA_LIMB_BROOM_FIRE_JET:
            *dList = NULL;
            gSPSegment(POLY_XLU_DISP++, 0xA,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x20, 1, 0,
                                        -this->work[CS_TIMER_1] * 0xF, 0x20, 0x40));
            break;
        case TWINROVA_LIMB_HAIR_FIRE_TRAIL:
        case TWINROVA_LIMB_BROOM_FIRE_TRAIL:
            *dList = NULL;
            gSPSegment(POLY_XLU_DISP++, 0xB,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x20, 1, 0,
                                        -this->work[CS_TIMER_1] * 0xA, 0x20, 0x40));
            break;
        case TWINROVA_LIMB_HAIR_ICE_JET:
        case TWINROVA_LIMB_BROOM_ICE_JET:
            *dList = NULL;
            gSPSegment(POLY_XLU_DISP++, 8,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x20, 1,
                                        this->work[CS_TIMER_1], -this->work[CS_TIMER_1] * 7, 0x20, 0x40));
            break;
        case TWINROVA_LIMB_HAIR_ICE_TRAIL:
        case TWINROVA_LIMB_BROOM_ICE_TRAIL:
            *dList = NULL;
            gSPSegment(POLY_XLU_DISP++, 9, tex_scroll2(play->state.gfxCtx, 0, this->work[CS_TIMER_1], 0x20, 0x40));
            break;
        case TWINROVA_LIMB_LEFT_HAIR_BUN:
            if (this->unk_5F8 != 0) {
                *dList = gTwinrovaLeftHairBunDL;
            }
            break;

        case TWINROVA_LIMB_RIGHT_HAIR_BUN:
            if (this->unk_5F8 != 0) {
                *dList = gTwinrovaRightHairBunDL;
            }
            break;
    }

    if ((this->unk_5F8 != 0) && ((limbIndex == TWINROVA_LIMB_LEFT_BROOM) || (limbIndex == TWINROVA_LIMB_RIGHT_BROOM))) {
        *dList = NULL;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7251);

    return false;
}

void twW_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f hand_p = { 0.0f, 0.0f, 0.0f };
    static Vec3f head_p = { 0.0f, 2000.0f, -2000.0f };
    static Vec3f l_hand_p = { 13000.0f, 0.0f, 0.0f };
    static Vec3f r_hand_p = { 13000.0f, 0.0f, 0.0f };

    BossTw* this = (BossTw*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7262);

    switch (limbIndex) {
        case TWINROVA_LIMB_LEFT_BROOM:
            Matrix_Position(&l_hand_p, &this->leftScepterPos);
            break;
        case TWINROVA_LIMB_RIGHT_BROOM:
            Matrix_Position(&r_hand_p, &this->rightScepterPos);
            break;
        case TWINROVA_LIMB_HEAD:
            Matrix_Position(&hand_p, &this->actor.focus.pos);
            Matrix_Position(&head_p, &this->crownPos);
            break;
        case TWINROVA_LIMB_HAIR_ICE_TRAIL:
        case TWINROVA_LIMB_HAIR_ICE_JET:
        case TWINROVA_LIMB_HAIR_FIRE_JET:
        case TWINROVA_LIMB_HAIR_FIRE_TRAIL:
        case TWINROVA_LIMB_BROOM_ICE_TRAIL:
        case TWINROVA_LIMB_BROOM_ICE_JET:
        case TWINROVA_LIMB_BROOM_FIRE_JET:
        case TWINROVA_LIMB_BROOM_FIRE_TRAIL:
            Matrix_push();
            Matrix_scale(this->workf[UNK_F12], this->workf[UNK_F12], this->workf[UNK_F12], MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 7295);
            Matrix_pull();
            gSPDisplayList(POLY_XLU_DISP++, *dList);
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7301);
}

void pl_shield_eff_disp(BossTw* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s16 temp_t0;
    s16 temp_a0;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7311);

    Matrix_push();

    temp_t0 = pl_shield_charge_UME | pl_shield_charge_TAKE;

    if (temp_t0 == 1) {
        Na_StartSystemSe_F(NA_SE_IT_SHIELD_CHARGE_LV1 & ~SFX_FLAG);
    } else if (temp_t0 == 2) {
        Na_StartSystemSe_F(NA_SE_IT_SHIELD_CHARGE_LV2 & ~SFX_FLAG);
    } else if (temp_t0 == 3) {
        Na_StartSystemSe_F(NA_SE_IT_SHIELD_CHARGE_LV3 & ~SFX_FLAG);
    }

    if (temp_t0 != 0 && temp_t0 < 4) {
        add_calc2(&pl_shield_charge_A, 255.0f, 1.0f, 20.0f);
        if (temp_t0 == 3) {
            temp_t0 *= 3;
        }
    } else if (temp_t0 == 0) {
        pl_shield_charge_A = 0.0f;
    } else {
        add_calc2(&pl_shield_charge_A, 0.0f, 1.0f, 10.0f);
        if (pl_shield_charge_A == 0.0f) {
            pl_shield_charge_TAKE = 0;
            pl_shield_charge_UME = 0;
        }

        temp_t0 = 1;
    }

    if (mirror_shield_check(play)) {
        if (temp_t0 != 0) {
            Matrix_mult(&player->shieldMf, MTXMODE_NEW);
            Matrix_rotateX(M_PI / 2.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 7362);
            temp_a0 = (sin_s(this->work[CS_TIMER_1] * 2730 * temp_t0) * pl_shield_charge_A * 0.5f) + (pl_shield_charge_A * 0.5f);
            if (pl_shield_charge_UME != 0) {
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 245, 255, temp_a0);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaMirrorShieldFireChargeSidesDL));
                gSPSegment(POLY_XLU_DISP++, 8,
                           two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (this->work[CS_TIMER_1] * 2) * temp_t0,
                                            0, 0x20, 0x20, 1, (-this->work[CS_TIMER_1] * 2) * temp_t0, 0, 0x20, 0x20));
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 100, 20, 0, (s16)pl_shield_charge_A);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaMirrorShieldFireChargeCenterDL));
            } else {
                gDPSetEnvColor(POLY_XLU_DISP++, 225, 255, 255, temp_a0);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaMirrorShieldIceChargeSidesDL));
                gSPSegment(POLY_XLU_DISP++, 8,
                           two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0,
                                            (-this->work[CS_TIMER_1] * 5) * temp_t0, 0x20, 0x40, 1,
                                            (this->work[CS_TIMER_1] * 4) * temp_t0, 0, 0x20, 0x20));
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 175, 205, 195, (s16)pl_shield_charge_A);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaMirrorShieldIceChargeCenterDL));
            }
        }
    }

    if (pl_shield_charge_EFF_time != 0) {
        f32 step = pl_shield_charge_EFF_SCR > 0 ? 100.0f : 60.0f;

        pl_shield_charge_EFF_time--;
        add_calc2(&pl_shield_charge_EFF_A, 255.0f, 1.0f, step);
    } else {
        f32 step = pl_shield_charge_EFF_SCR > 0 ? 40.0f : 20.0f;

        add_calc2(&pl_shield_charge_EFF_A, 0.0f, 1.0f, step);
    }

    if (mirror_shield_check(play) && pl_shield_charge_EFF_A > 0.0f) {
        f32 scale = pl_shield_charge_EFF_SCR > 0 ? 1.3f : 1.0f;

        Matrix_mult(&player->shieldMf, MTXMODE_NEW);
        Matrix_rotateX(M_PI / 2.0f, MTXMODE_APPLY);
        Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 7486);
        if (pl_shield_charge_UME != 0) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 220, 20, (s16)pl_shield_charge_EFF_A);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 20, 110);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s16)pl_shield_charge_EFF_A);
            gDPSetEnvColor(POLY_XLU_DISP++, 185, 225, 205, 150);
        }

        gSPSegment(POLY_XLU_DISP++, 8,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, this->work[CS_TIMER_1] * pl_shield_charge_EFF_SCR, 0x20,
                                    0x40, 1, 0, this->work[CS_TIMER_1] * pl_shield_charge_EFF_SCR, 0x20, 0x20));
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaShieldAbsorbAndReflectEffectDL));
    }

    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7531);
}

void demo_mahoujin_draw(BossTw* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7546);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 8,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, -this->work[CS_TIMER_1] * 15, 0x20, 0x40, 1, 0,
                                0, 0x40, 0x40));

    Matrix_push();

    Matrix_translate(0.0f, 232.0f, -600.0f, MTXMODE_NEW);
    Matrix_scale(this->spawnPortalScale, this->spawnPortalScale, this->spawnPortalScale, MTXMODE_APPLY);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, (s16)this->spawnPortalAlpha);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 7582);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaSpawnPortalShadowDL));

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 135, 175, 165, (s16)this->spawnPortalAlpha);
    Matrix_translate(0.0f, 2.0f, 0.0f, MTXMODE_APPLY);
    Matrix_rotateX(M_PI / 2.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 7596);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaKotakeMagicSigilDL));

    Matrix_translate(0.0f, 232.0f, 600.0f, MTXMODE_NEW);
    Matrix_scale(this->spawnPortalScale, this->spawnPortalScale, this->spawnPortalScale, MTXMODE_APPLY);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, (s16)this->spawnPortalAlpha);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 7617);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaSpawnPortalShadowDL));

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 0, (s16)this->spawnPortalAlpha);
    Matrix_translate(0.0f, 2.0f, 0.0f, MTXMODE_APPLY);
    Matrix_rotateX(M_PI / 2.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 7631);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaKoumeMagicSigilDL));

    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7635);
}

void heaven_draw(BossTw* this, PlayState* play) {
    s32 pad;
    f32 scale;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7645);

    Matrix_push();
    Matrix_translate(0.0f, 750.0f, 0.0f, MTXMODE_NEW);
    Matrix_scale(0.35f, 0.35f, 0.35f, MTXMODE_APPLY);
    Matrix_push();
    Matrix_scale(this->workf[UNK_F19], this->workf[UNK_F19], this->workf[UNK_F19], MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 7671);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaLightCircleDL));

    Matrix_pull();
    gSPSegment(POLY_XLU_DISP++, 8,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, -koume->work[CS_TIMER_1] * 2, 0, 0x20, 0x20, 1,
                                -koume->work[CS_TIMER_1] * 2, 0, 0x20, 0x40));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s16)this->workf[UNK_F18] / 2);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 7694);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaLightRaysDL));

    gSPSegment(POLY_XLU_DISP++, 8,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, -koume->work[CS_TIMER_1] * 5,
                                -koume->work[CS_TIMER_1] * 2, 0x20, 0x40, 1, 0, -koume->work[CS_TIMER_1] * 2,
                                0x10, 0x10));
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s16)(this->workf[UNK_F18] * 0.3f));

    scale = this->workf[UNK_F18] / 150.0f;
    scale = CLAMP_MAX(scale, 1.0f);

    Matrix_scale(scale, 1.0f, scale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 7728);
    gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaLightPillarDL));
    Matrix_pull();

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7732);
}

void tw_gattai_draw(Actor* thisx, PlayState* play2) {
    static Vec3f magic_p = { 0.0f, 200.0f, 2000.0f };
    PlayState* play = play2;
    BossTw* this = (BossTw*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7748);

    if (this->visible) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

        POLY_OPA_DISP = (this->work[FOG_TIMER] & 2) ? set_fog(POLY_OPA_DISP, 255, 50, 0, 0, 900, 1099)
                                                    : set_fog(POLY_OPA_DISP, (u32)this->fogR, (u32)this->fogG,
                                                                  (u32)this->fogB, 0, this->fogNear, this->fogFar);

        Matrix_push();
        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              twW_draw_sub, twW_draw_sub2, thisx);
        Matrix_pull();

        Matrix_Position(&magic_p, &this->beamOrigin);
        POLY_OPA_DISP = set_fog(POLY_OPA_DISP, play->lightCtx.fogColor[0], play->lightCtx.fogColor[1],
                                    play->lightCtx.fogColor[2], 0, play->lightCtx.fogNear, 1000);
    }

    Tw_Eff_disp(play);
    pl_shield_eff_disp(this, play);

    if (this->spawnPortalAlpha > 0.0f) {
        demo_mahoujin_draw(this, play);
    }

    if (this->workf[UNK_F18] > 0.0f) {
        heaven_draw(this, play);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 7804);
}

void tw_fire_move(BossTw* this, PlayState* play) {
    s16 i;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 distXZ;
    Player* player = GET_PLAYER(play);
    Player* player2 = player;

    switch (this->actor.params) {
        case TW_FIRE_BLAST:
            switch (this->csState1) {
                case 0:
                    Actor_set_scale(&this->actor, 0.03f);
                    this->csState1 = 1;
                    xDiff = player->actor.world.pos.x - this->actor.world.pos.x;
                    yDiff = (player->actor.world.pos.y + 30.0f) - this->actor.world.pos.y;
                    zDiff = player->actor.world.pos.z - this->actor.world.pos.z;
                    // yaw
                    this->actor.world.rot.y = RAD_TO_BINANG(fatan2(xDiff, zDiff));
                    // pitch
                    distXZ = sqrtf(SQ(xDiff) + SQ(zDiff));
                    this->actor.world.rot.x = RAD_TO_BINANG(fatan2(yDiff, distXZ));
                    this->actor.speed = 20.0f;

                    for (i = 0; i < 50; i++) {
                        this->blastTailPos[i] = this->actor.world.pos;
                    }
                    this->workf[TAIL_ALPHA] = 255.0f;
                    FALLTHROUGH;
                case 1:
                case 10:
                    this->blastActive = true;
                    if (this->timers[0] == 0) {
                        Actor_position_speed_set_XY(&this->actor);
                        Actor_position_move(&this->actor);
                        Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_SHOOT_FIRE & ~SFX_FLAG);
                    } else {
                        Vec3f velocity;
                        Vec3f velDir;
                        Vec3s blastDir;
                        s16 alpha;

                        this->actor.world.pos = player2->bodyPartsPos[PLAYER_BODYPART_R_HAND];
                        this->actor.world.pos.y = -2000.0f;
                        Matrix_to_rotate_new(&player2->shieldMf, &blastDir, 0);
                        blastDir.x = -blastDir.x;
                        blastDir.y += 0x8000;
                        adds(&this->magicDir.x, blastDir.x, 0xA, 0x800);
                        adds(&this->magicDir.y, blastDir.y, 0xA, 0x800);

                        if (this->timers[0] == 50) {
                            pl_shield_charge_EFF_time = 10;
                            pl_shield_charge_EFF_SCR = 7;
                            play->envCtx.lightBlend = 1.0f;
                        }

                        if (this->timers[0] <= 50) {
                            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_SHOOT_FIRE & ~SFX_FLAG);
                            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_REFL_FIRE & ~SFX_FLAG);
                            Matrix_rotateY((this->magicDir.y / 32678.0f) * M_PI, MTXMODE_NEW);
                            Matrix_rotateX((this->magicDir.x / 32678.0f) * M_PI, MTXMODE_APPLY);
                            velDir.x = 0.0f;
                            velDir.y = 0.0f;
                            velDir.z = 50.0f;
                            Matrix_Position(&velDir, &velocity);
                            alpha = this->timers[0] * 10;
                            alpha = CLAMP_MAX(alpha, 255);

                            tw_final_beam_ct(play, &player2->bodyPartsPos[PLAYER_BODYPART_R_HAND], &velocity,
                                                        &zero, 10.0f, 80.0f, alpha, 1);
                        }

                        if (this->timers[0] == 1) {
                            kankyo_flag = 0;
                            pl_shield_charge_UME++;
                            Actor_delete(&this->actor);
                        }

                        return;
                    }

                    this->groundBlastPos.y = beam_bg_check(&this->actor.world.pos);

                    if (this->groundBlastPos.y >= 0.0f) {
                        if (this->groundBlastPos.y != 35.0f) {
                            this->groundBlastPos.x = this->actor.world.pos.x;
                            this->groundBlastPos.z = this->actor.world.pos.z;
                            tw_wepon_gnd_set(this, play, 1);
                        } else {
                            Vec3f velocity;
                            Vec3f accel;

                            for (i = 0; i < 50; i++) {
                                velocity.x = rnd_fx(20.0f);
                                velocity.y = rnd_fx(20.0f);
                                velocity.z = rnd_fx(20.0f);
                                accel.x = 0.0f;
                                accel.y = 0.0f;
                                accel.z = 0.0f;
                                tw_magic_eff_ct(play, &this->actor.world.pos, &velocity, &accel,
                                                      rnd_f(10.0f) + 25.0f, this->blastType);
                            }

                            play->envCtx.lightBlend = 0.5f;
                        }

                        this->csState1 = 2;
                        this->timers[0] = 20;
                    } else {
                        Vec3f pos;
                        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
                        Vec3f accel = { 0.0f, 0.0f, 0.0f };

                        for (i = 0; i < 10; i++) {
                            pos = this->blastTailPos[(s16)rnd_f(29.9f)];
                            pos.x += rnd_fx(40.0f);
                            pos.y += rnd_fx(40.0f);
                            pos.z += rnd_fx(40.0f);
                            accel.y = 0.4f;
                            accel.x = rnd_fx(0.5f);
                            accel.z = rnd_fx(0.5f);
                            tw_hinoko_ct(play, &pos, &velocity, &accel, (s16)rnd_f(2.0f) + 8, 1, 75);
                        }
                    }
                    break;
                case 2:
                    add_calc2(&this->workf[TAIL_ALPHA], 0.0f, 1.0f, 15.0f);
                    if (this->timers[0] == 0) {
                        Actor_delete(&this->actor);
                    }
                    break;
            }
            break;

        case TW_FIRE_BLAST_GROUND:
            if (this->timers[0] != 0) {
                if (this->timers[0] == 1) {
                    kankyo_flag = 0;
                }

                if (wepon_flag == 2) {
                    this->timers[0] = 0;
                }

                Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_FIRE_EXP - SFX_FLAG);

                xDiff = koume->groundBlastPos2.x - player->actor.world.pos.x;
                yDiff = koume->groundBlastPos2.y - player->actor.world.pos.y;
                zDiff = koume->groundBlastPos2.z - player->actor.world.pos.z;

                if (!player->bodyIsBurning && (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
                    (fabsf(yDiff) < 10.0f) && (sqrtf(SQ(xDiff) + SQ(zDiff)) < (koume->workf[UNK_F13] * 4550.0f))) {
                    s16 j;

                    for (j = 0; j < 18; j++) {
                        player->bodyFlameTimers[j] = get_random_timer(0, 200);
                    }

                    player->bodyIsBurning = true;

                    if (this->work[BURN_TMR] == 0) {
                        player_SE_set(player, player->ageProperties->unk_92 + NA_SE_VO_LI_DEMO_DAMAGE);
                        this->work[BURN_TMR] = 40;
                    }

                    gattai->timers[2] = 100;
                }

                add_calc2(&koume->workf[UNK_F13], 0.04f, 0.1f, 0.002f);
                break;
            }

            if (1) {
                f32 sp4C = wepon_flag == 2 ? 3.0f : 1.0f;

                add_calc2(&koume->workf[UNK_F9], 0.0f, 1.0f, 10.0f * sp4C);
                add_calc2(&koume->workf[UNK_F12], 0.0f, 1.0f, 0.03f * sp4C);
                add_calc2(&koume->workf[TAIL_ALPHA], 0.0f, 1.0f, 3.0f * sp4C);
                add_calc2(&koume->workf[UNK_F11], 0.0f, 1.0f, 6.0f * sp4C);
            }

            if (koume->workf[TAIL_ALPHA] <= 0.0f) {
                Actor_delete(&this->actor);
            }

            break;
    }
}

void tw_ice_move(BossTw* this, PlayState* play) {
    s16 i;
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    f32 xzDist;
    Player* player = GET_PLAYER(play);
    Player* player2 = player;

    switch (this->actor.params) {
        case TW_ICE_BLAST:
            switch (this->csState1) {
                case 0:
                    Actor_set_scale(&this->actor, 0.03f);
                    this->csState1 = 1;
                    xDiff = player->actor.world.pos.x - this->actor.world.pos.x;
                    yDiff = (player->actor.world.pos.y + 30.0f) - this->actor.world.pos.y;
                    zDiff = player->actor.world.pos.z - this->actor.world.pos.z;
                    this->actor.world.rot.y = RAD_TO_BINANG(fatan2(xDiff, zDiff));
                    xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
                    this->actor.world.rot.x = RAD_TO_BINANG(fatan2(yDiff, xzDist));
                    this->actor.speed = 20.0f;
                    for (i = 0; i < 50; i++) {
                        this->blastTailPos[i] = this->actor.world.pos;
                    }

                    this->workf[TAIL_ALPHA] = 255.0f;
                    FALLTHROUGH;
                case 1:
                case 10:
                    this->blastActive = true;

                    if (this->timers[0] == 0) {
                        Actor_position_speed_set_XY(&this->actor);
                        Actor_position_move(&this->actor);
                        Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_SHOOT_FREEZE - SFX_FLAG);
                    } else {
                        Vec3f velocity;
                        Vec3f spF4;
                        Vec3s reflDir;
                        s16 alpha;

                        this->actor.world.pos = player2->bodyPartsPos[PLAYER_BODYPART_R_HAND];
                        this->actor.world.pos.y = -2000.0f;
                        Matrix_to_rotate_new(&player2->shieldMf, &reflDir, 0);
                        reflDir.x = -reflDir.x;
                        reflDir.y += 0x8000;
                        adds(&this->magicDir.x, reflDir.x, 0xA, 0x800);
                        adds(&this->magicDir.y, reflDir.y, 0xA, 0x800);

                        if (this->timers[0] == 50) {
                            pl_shield_charge_EFF_time = 10;
                            pl_shield_charge_EFF_SCR = 7;
                            play->envCtx.lightBlend = 1.0f;
                        }

                        if (this->timers[0] <= 50) {
                            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_SHOOT_FREEZE - SFX_FLAG);
                            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_REFL_FREEZE - SFX_FLAG);
                            Matrix_rotateY((this->magicDir.y / 32678.0f) * M_PI, MTXMODE_NEW);
                            Matrix_rotateX((this->magicDir.x / 32678.0f) * M_PI, MTXMODE_APPLY);
                            spF4.x = 0.0f;
                            spF4.y = 0.0f;
                            spF4.z = 50.0f;
                            Matrix_Position(&spF4, &velocity);
                            alpha = this->timers[0] * 10;
                            alpha = CLAMP_MAX(alpha, 255);

                            tw_final_beam_ct(play, &player2->bodyPartsPos[PLAYER_BODYPART_R_HAND], &velocity,
                                                        &zero, 10.0f, 80.0f, alpha, 0);
                        }

                        if (this->timers[0] == 1) {
                            kankyo_flag = 0;
                            pl_shield_charge_TAKE++;
                            Actor_delete(&this->actor);
                        }

                        break;
                    }

                    this->groundBlastPos.y = beam_bg_check(&this->actor.world.pos);

                    if (this->groundBlastPos.y >= 0.0f) {
                        if (this->groundBlastPos.y != 35.0f) {
                            this->groundBlastPos.x = this->actor.world.pos.x;
                            this->groundBlastPos.z = this->actor.world.pos.z;
                            tw_wepon_gnd_set(this, play, 0);
                        } else {
                            for (i = 0; i < 50; i++) {
                                Vec3f velocity;
                                Vec3f accel;

                                velocity.x = rnd_fx(20.0f);
                                velocity.y = rnd_fx(20.0f);
                                velocity.z = rnd_fx(20.0f);
                                accel.x = 0.0f;
                                accel.y = 0.0f;
                                accel.z = 0.0f;
                                tw_magic_eff_ct(play, &this->actor.world.pos, &velocity, &accel,
                                                      rnd_f(10.0f) + 25.0f, this->blastType);
                            }

                            play->envCtx.lightBlend = 0.5f;
                        }

                        this->csState1 = 2;
                        this->timers[0] = 20;
                    } else {
                        Vec3f pos;
                        Vec3f velocity = { 0.0f, 0.0f, 0.0f };
                        Vec3f accel = { 0.0f, 0.0f, 0.0f };

                        for (i = 0; i < 10; i++) {
                            pos = this->blastTailPos[(s16)rnd_f(29.9f)];
                            pos.x += rnd_fx(40.0f);
                            pos.y += rnd_fx(40.0f);
                            pos.z += rnd_fx(40.0f);
                            accel.y = 0.4f;
                            accel.x = rnd_fx(0.5f);
                            accel.z = rnd_fx(0.5f);
                            tw_hinoko_ct(play, &pos, &velocity, &accel, ((s16)rnd_f(2.0f) + 8), 0, 75);
                        }
                    }
                    break;

                case 2:
                    add_calc2(&this->workf[TAIL_ALPHA], 0.0f, 1.0f, 15.0f);
                    if (this->timers[0] == 0) {
                        Actor_delete(&this->actor);
                    }
                    break;
            }
            break;

        case TW_ICE_BLAST_GROUND:
            if (this->timers[0] != 0) {
                s32 pad;

                if (this->timers[0] == 1) {
                    kankyo_flag = 0;
                }

                if (wepon_flag == 1) {
                    this->timers[0] = 0;
                }

                Actor_SE_set(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);

                if (this->timers[0] > (gattai->actionFunc == mode_nomove ? 70 : 20)) {
                    Vec3f pos;
                    Vec3f velocity;
                    Vec3f accel;

                    pos.x = kotake->groundBlastPos2.x + rnd_fx(320.0f);
                    pos.z = kotake->groundBlastPos2.z + rnd_fx(320.0f);
                    pos.y = kotake->groundBlastPos2.y;
                    velocity.x = 0.0f;
                    velocity.y = 0.0f;
                    velocity.z = 0.0f;
                    accel.x = 0.0f;
                    accel.y = 0.13f;
                    accel.z = 0.0f;
                    tw_eff_smoke_ct(play, TWEFF_3, &pos, &velocity, &accel, rnd_f(5.0f) + 20.0f, 0, 0, 80);
                    velocity.x = rnd_fx(10.0f);
                    velocity.z = rnd_fx(10.0f);
                    velocity.y = rnd_f(3.0f) + 3.0f;
                    pos.x = kotake->groundBlastPos2.x + (velocity.x * 0.5f);
                    pos.z = kotake->groundBlastPos2.z + (velocity.z * 0.5f);
                    tw_eff_smoke_ct(play, TWEFF_3, &pos, &velocity, &accel, rnd_f(5.0f) + 15.0f, 255, 2,
                                       130);
                }

                add_calc2(&kotake->workf[UNK_F9], 80.0f, 1.0f, 3.0f);
                add_calc2(&kotake->workf[UNK_F11], 255.0f, 1.0f, 10.0f);
                add_calc2(&kotake->workf[UNK_F12], 0.04f, 0.1f, 0.002f);
                add_calc2(&kotake->workf[UNK_F16], 70.0f, 1.0f, 5.0f);

                if ((this->timers[0] == 70) || (this->timers[0] == 30)) {
                    kotake->workf[UNK_F16] = 10.0f;
                }

                if ((this->timers[0] % 4) == 0) {
                    kotake->workf[UNK_F15] = (2.0f * (s16)rnd_f(9.9f) * M_PI) / 10.0f;
                }
            } else {
                f32 sp80;

                if (wepon_flag == 1) {
                    if (kotake->workf[UNK_F11] > 1.0f) {
                        for (i = 0; i < 3; i++) {
                            Vec3f pos;
                            Vec3f velocity;
                            Vec3f accel;
                            pos.x = rnd_fx(280.0f) + kotake->groundBlastPos2.x;
                            pos.z = rnd_fx(280.0f) + kotake->groundBlastPos2.z;
                            pos.y = kotake->groundBlastPos2.y + 30.0f;
                            velocity.x = 0.0f;
                            velocity.y = 0.0f;
                            velocity.z = 0.0f;
                            accel.x = 0.0f;
                            accel.y = 0.13f;
                            accel.z = 0.0f;
                            tw_eff_smoke_ct(play, TWEFF_3, &pos, &velocity, &accel, rnd_f(5.0f) + 20, 0, 0,
                                               80);
                        }
                    }
                    sp80 = 3.0f;
                } else {
                    sp80 = 1.0f;
                }

                add_calc2(&kotake->workf[UNK_F14], 0.0f, 1.0f, 0.2f * sp80);
                add_calc2(&kotake->workf[UNK_F11], 0.0f, 1.0f, 5.0f * sp80);
                add_calc2(&kotake->workf[UNK_F9], 0.0f, 1.0f, sp80);

                if (kotake->workf[UNK_F9] <= 0.0f) {
                    Actor_delete(&this->actor);
                }
            }
            break;
    }
}

s32 tw_wepon_hitck(BossTw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 ret = false;

    if (1) {}

    if (this->csState1 == 1) {
        if (this->collider.base.acFlags & AC_HIT) {
            ColliderElement* acHitElem;

            this->collider.base.acFlags &= ~AC_HIT;
            this->collider.base.atFlags &= ~AT_HIT;
            acHitElem = this->collider.elem.acHitElem;

            if (acHitElem->atDmgInfo.dmgFlags & DMG_SHIELD) {
                this->work[INVINC_TIMER] = 7;
                play->envCtx.lightBlend = 1.0f;
                z_vibctl2_vib_setQ(0.0f, 100, 5, 4);

                if (mirror_shield_check(play)) {
                    if (this->blastType == 1) {
                        if (pl_shield_charge_TAKE != 0) {
                            pl_shield_charge_TAKE = 0;
                            tw_tate_eff_ct(play, 10.0f, 1);
                        } else {
                            tw_tate_eff2_ct(play, 10.0f, 1);
                            pl_shield_charge_UME++;
                            pl_shield_charge_EFF_time = (pl_shield_charge_UME * 2) + 8;
                            pl_shield_charge_EFF_SCR = -7;
                        }
                    } else {
                        if (pl_shield_charge_UME != 0) {
                            pl_shield_charge_UME = 0;
                            if (1) {}
                            tw_tate_eff_ct(play, 10.0f, 0);
                        } else {
                            tw_tate_eff2_ct(play, 10.0f, 0);
                            pl_shield_charge_TAKE++;
                            pl_shield_charge_EFF_time = (pl_shield_charge_TAKE * 2) + 8;
                            pl_shield_charge_EFF_SCR = -7;
                        }
                    }

                    if ((pl_shield_charge_TAKE >= 3) || (pl_shield_charge_UME >= 3)) {
                        this->timers[0] = 80;
                        this->csState1 = 10;
                        Matrix_to_rotate_new(&player->shieldMf, &this->magicDir, 0);
                        this->magicDir.y += 0x8000;
                        this->magicDir.x = -this->magicDir.x;
                        pl_shield_charge_EFF_time = 8;
                    } else {
                        this->csState1 = 2;
                        this->timers[0] = 20;
                        kankyo_flag = 0;
                    }
                } else {
                    tw_tate_eff_ct(play, 10.0f, this->blastType);
                    this->csState1 = 2;
                    this->timers[0] = 20;
                    kankyo_flag = 0;
                    pl_shield_charge_TAKE = 0;
                    pl_shield_charge_UME = 0;
                    Na_StartSystemSe_F(NA_SE_IT_SHIELD_REFLECT_MG2);
                }

                ret = true;
            }
        }
    }

    return ret;
}

void tw_wepon_move(Actor* thisx, PlayState* play) {
    BossTw* this = (BossTw*)thisx;
    ColliderCylinder* collider;
    s16 i;

    this->work[CS_TIMER_1]++;
    this->work[CS_TIMER_2]++;
    this->work[TAIL_IDX]++;

    if (this->work[TAIL_IDX] > 29) {
        this->work[TAIL_IDX] = 0;
    }

    this->blastTailPos[this->work[TAIL_IDX]] = this->actor.world.pos;

    this->actionFunc(this, play);

    for (i = 0; i < 5; i++) {
        if (this->timers[i] != 0) {
            this->timers[i]--;
        }
    }

    if (this->work[INVINC_TIMER] != 0) {
        this->work[INVINC_TIMER]--;
    }

    if (this->work[BURN_TMR] != 0) {
        this->work[BURN_TMR]--;
    }

    this->actor.focus.pos = this->actor.world.pos;
    collider = &this->collider;
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, collider);

    if (this->blastActive && this->work[INVINC_TIMER] == 0 && !tw_wepon_hitck(this, play)) {
        CollisionCheck_setAC(play, &play->colChkCtx, &collider->base);
        CollisionCheck_setAT(play, &play->colChkCtx, &collider->base);
    }

    this->blastActive = false;
}

void tw_wepon_draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossTw* this = (BossTw*)thisx;
    f32 scaleFactor;
    s16 tailIdx;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 8818);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    switch (this->actor.params) {
        case TW_FIRE_BLAST:
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, (s8)this->workf[TAIL_ALPHA]);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);
            for (i = 9; i >= 0; i--) {
                gSPSegment(POLY_XLU_DISP++, 8,
                           two_tex_scroll(
                               play->state.gfxCtx, G_TX_RENDERTILE, ((this->work[CS_TIMER_1] * 3) + (i * 10)) & 0x7F,
                               ((-this->work[CS_TIMER_1] * 15) + (i * 50)) & 0xFF, 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
                tailIdx = ((this->work[TAIL_IDX] - i) + 30) % 30;
                Matrix_translate(this->blastTailPos[tailIdx].x, this->blastTailPos[tailIdx].y,
                                 this->blastTailPos[tailIdx].z, MTXMODE_NEW);
                scaleFactor = 1.0f - (i * 0.09f);
                Matrix_scale(this->actor.scale.x * scaleFactor, this->actor.scale.y * scaleFactor,
                             this->actor.scale.z * scaleFactor, MTXMODE_APPLY);
                Matrix_rotate_scale_exchange(&play->billboardMtxF);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 8865);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaFireDL));
            }
            break;

        case TW_FIRE_BLAST_GROUND:
            break;

        case TW_ICE_BLAST:
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (s8)this->workf[TAIL_ALPHA]);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIceMaterialDL));
            for (i = 9; i >= 0; i--) {
                gSPSegment(POLY_XLU_DISP++, 8,
                           two_tex_scroll(
                               play->state.gfxCtx, G_TX_RENDERTILE, ((this->work[CS_TIMER_1] * 3) + (i * 0xA)) & 0x7F,
                               (u8)((-this->work[CS_TIMER_1] * 0xF) + (i * 50)), 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
                tailIdx = ((this->work[TAIL_IDX] - i) + 30) % 30;
                Matrix_translate(this->blastTailPos[tailIdx].x, this->blastTailPos[tailIdx].y,
                                 this->blastTailPos[tailIdx].z, MTXMODE_NEW);
                scaleFactor = 1.0f - (i * 0.09f);
                Matrix_scale(this->actor.scale.x * scaleFactor, this->actor.scale.y * scaleFactor,
                             this->actor.scale.z * scaleFactor, MTXMODE_APPLY);
                Matrix_rotate_scale_exchange(&play->billboardMtxF);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 9004);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIceModelDL));
            }
            break;

        case TW_ICE_BLAST_GROUND:
            break;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 9013);
}

void tw_tamasii_draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossTw* this = (BossTw*)thisx;
    f32 scaleFactor;
    s16 tailIdx;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 9028);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    if (this->actor.params == TW_DEATHBALL_KOUME) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, (s8)this->workf[TAIL_ALPHA]);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);

        for (i = 9; i >= 0; i--) {
            gSPSegment(POLY_XLU_DISP++, 8,
                       two_tex_scroll(
                           play->state.gfxCtx, G_TX_RENDERTILE, (((this->work[CS_TIMER_1] * 3) + (i * 0xA))) & 0x7F,
                           (u8)((-this->work[CS_TIMER_1] * 0xF) + (i * 50)), 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
            tailIdx = ((this->work[TAIL_IDX] - i) + 30) % 30;
            Matrix_translate(this->blastTailPos[tailIdx].x, this->blastTailPos[tailIdx].y,
                             this->blastTailPos[tailIdx].z, MTXMODE_NEW);
            scaleFactor = (1.0f - (i * 0.09f));
            Matrix_scale(this->actor.scale.x * scaleFactor, this->actor.scale.y * scaleFactor,
                         this->actor.scale.z * scaleFactor, MTXMODE_APPLY);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 9071);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaFireDL));
        }
    } else {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, (s8)this->workf[TAIL_ALPHA]);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIceMaterialDL));

        for (i = 9; i >= 0; i--) {
            gSPSegment(POLY_XLU_DISP++, 8,
                       two_tex_scroll(
                           play->state.gfxCtx, G_TX_RENDERTILE, (((this->work[CS_TIMER_1] * 3) + (i * 0xA))) & 0x7F,
                           (u8)((-this->work[CS_TIMER_1] * 0xF) + (i * 50)), 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
            tailIdx = ((this->work[TAIL_IDX] - i) + 30) % 30;
            Matrix_translate(this->blastTailPos[tailIdx].x, this->blastTailPos[tailIdx].y,
                             this->blastTailPos[tailIdx].z, MTXMODE_NEW);
            scaleFactor = (1.0f - (i * 0.09f));
            Matrix_scale(this->actor.scale.x * scaleFactor, this->actor.scale.y * scaleFactor,
                         this->actor.scale.z * scaleFactor, MTXMODE_APPLY);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_tw.c", 9107);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIceModelDL));
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_tw.c", 9111);
}

void Tw_Eff_move(PlayState* play) {
    static Color_RGB8 col_d[] = {
        { 255, 128, 0 },   { 255, 0, 0 },     { 255, 255, 0 },   { 255, 0, 0 },
        { 100, 100, 100 }, { 255, 255, 255 }, { 150, 150, 150 }, { 255, 255, 255 },
    };
    Vec3f sp11C;
    BossTwEffect* eff = play->specialEffects;
    Player* player = GET_PLAYER(play);
    u8 sp113 = 0;
    s16 i;
    s16 j;
    s16 colorIdx;
    Vec3f off;

    for (i = 0; i < BOSS_TW_EFFECT_COUNT; i++) {
        if (eff->type != TWEFF_NONE) {
            eff->pos.x += eff->curSpeed.x;
            eff->pos.y += eff->curSpeed.y;
            eff->pos.z += eff->curSpeed.z;
            eff->frame++;
            eff->curSpeed.x += eff->accel.x;
            eff->curSpeed.y += eff->accel.y;
            eff->curSpeed.z += eff->accel.z;

            if (eff->type == TWEFF_DOT) {
                colorIdx = eff->frame % 4;

                if (eff->work[EFF_ARGS] == 0) {
                    colorIdx += 4;
                }

                eff->color.r = col_d[colorIdx].r;
                eff->color.g = col_d[colorIdx].g;
                eff->color.b = col_d[colorIdx].b;
                eff->alpha -= 20;

                if (eff->alpha <= 0) {
                    eff->alpha = 0;
                    eff->type = TWEFF_NONE;
                }

            } else if ((eff->type == TWEFF_3) || (eff->type == TWEFF_2)) {
                if (eff->work[EFF_ARGS] == 2) {
                    eff->alpha -= 20;
                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                } else if (eff->work[EFF_ARGS] == 0) {
                    eff->alpha += 10;
                    if (eff->alpha >= 100) {
                        eff->work[EFF_ARGS]++;
                    }
                } else {
                    eff->alpha -= 3;
                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                }
            } else if (eff->type == TWEFF_FLAME) {
                if (eff->work[EFF_UNKS1] != 0) {
                    eff->alpha = (eff->alpha - (i & 7)) - 0xD;
                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                } else {
                    eff->alpha += 300;
                    if (eff->alpha >= 255) {
                        eff->alpha = 255;
                        eff->work[EFF_UNKS1]++;
                    }
                }
            } else if (eff->type == TWEFF_SHLD_BLST) {
                pl_final_beam_on = 1;
                eff->work[EFF_UNKS1]++;
                if (eff->work[EFF_UNKS1] > 30) {
                    eff->alpha -= 10;
                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                }

                add_calc2(&eff->workf[EFF_SCALE], eff->workf[EFF_DIST], 0.1f, 0.003f);
                off.x = gattai->actor.world.pos.x - eff->pos.x;
                off.y = (gattai->actor.world.pos.y - eff->pos.y) * 0.5f;
                off.z = gattai->actor.world.pos.z - eff->pos.z;

                if (gattai->actionFunc != mode_g_damage) {
                    Vec3f spF4;
                    Vec3f spE8;
                    Vec3f spDC;

                    if ((SQ(off.x) + SQ(off.y) + SQ(off.z)) < SQ(60.0f)) {
                        for (j = 0; j < 50; j++) {
                            spF4.x = gattai->actor.world.pos.x + rnd_fx(35.0f);
                            spF4.y = gattai->actor.world.pos.y + rnd_fx(70.0f);
                            spF4.z = gattai->actor.world.pos.z + rnd_fx(35.0f);
                            spE8.x = rnd_fx(20.0f);
                            spE8.y = rnd_fx(20.0f);
                            spE8.z = rnd_fx(20.0f);
                            spDC.x = 0.0f;
                            spDC.y = 0.0f;
                            spDC.z = 0.0f;
                            tw_magic_eff_ct(play, &spF4, &spE8, &spDC, rnd_f(10.0f) + 25.0f,
                                                  eff->work[EFF_ARGS]);
                        }

                        gattai->twinrovaStun = 1;
                        play->envCtx.lightBlend = 1.0f;
                        eff->type = TWEFF_NONE;
                    }
                }
            } else if (eff->type == TWEFF_MERGEFLAME) {
                sp11C.x = 0.0f;
                sp11C.y = eff->pos.y;
                sp11C.z = eff->workf[EFF_DIST];
                Matrix_rotateY(gattai->workf[UNK_F9] + eff->workf[EFF_ROLL], MTXMODE_NEW);
                Matrix_Position(&sp11C, &eff->pos);

                if (eff->work[EFF_UNKS1] != 0) {
                    eff->alpha -= 60;
                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                } else {
                    eff->alpha += 60;
                    if (eff->alpha >= 255) {
                        eff->alpha = 255;
                        eff->work[EFF_UNKS1]++;
                    }
                }
            } else if (eff->type == TWEFF_SHLD_DEFL) {
                eff->work[EFF_UNKS1]++;
                sp11C.x = 0.0f;
                sp11C.y = 0.0f;
                sp11C.z = -eff->workf[EFF_DIST];
                Matrix_rotateY(BINANG_TO_RAD_ALT(pl_shield_angle_Y), MTXMODE_NEW);
                Matrix_rotateX(-0.2f, MTXMODE_APPLY);
                Matrix_rotateZ(eff->workf[EFF_ROLL], MTXMODE_APPLY);
                Matrix_rotateY(eff->workf[EFF_YAW], MTXMODE_APPLY);
                Matrix_Position(&sp11C, &eff->pos);
                eff->pos.x += pl_shield_pos.x;
                eff->pos.y += pl_shield_pos.y;
                eff->pos.z += pl_shield_pos.z;

                if (eff->work[EFF_UNKS1] < 10) {
                    add_calc2(&eff->workf[EFF_DIST], 50.0f, 0.5f, 100.0f);
                } else {
                    add_calc2(&eff->workf[EFF_YAW], 0.0f, 0.5f, 10.0f);
                    add_calc2(&eff->workf[EFF_DIST], 1000.0f, 1.0f, 10.0f);
                    if (eff->work[EFF_UNKS1] >= 0x10) {
                        if ((eff->work[EFF_UNKS1] == 16) && (sp113 == 0)) {
                            Vec3f spD0;

                            sp113 = 1;
                            spD0 = eff->pos;
                            if (eff->pos.y > 40.0f) {
                                spD0.y = 220.0f;
                            } else {
                                spD0.y = -50.0f;
                            }
                            if ((gattai->groundBlastPos.y = beam_bg_check(&spD0)) >= 0.0f) {
                                if (gattai->groundBlastPos.y != 35.0f) {
                                    gattai->groundBlastPos.x = eff->pos.x;
                                    gattai->groundBlastPos.z = eff->pos.z;
                                    tw_wepon_gnd_set(gattai, play, eff->work[EFF_ARGS]);
                                }
                            }
                        }
                        eff->alpha -= 300;
                        if (eff->alpha <= 0) {
                            eff->alpha = 0;
                            eff->type = TWEFF_NONE;
                        }
                    }
                }

                tw_magic_eff_ct(play, &eff->pos, &zero, &zero, 10, eff->work[EFF_ARGS]);
            } else if (eff->type == TWEFF_SHLD_HIT) {
                eff->work[EFF_UNKS1]++;
                sp11C.x = 0.0f;
                sp11C.y = 0.0f;
                sp11C.z = -eff->workf[EFF_DIST];
                Matrix_rotateY(BINANG_TO_RAD_ALT(pl_shield_angle_Y), MTXMODE_NEW);
                Matrix_rotateX(-0.2f, MTXMODE_APPLY);
                Matrix_rotateZ(eff->workf[EFF_ROLL], MTXMODE_APPLY);
                Matrix_rotateY(eff->workf[EFF_YAW], MTXMODE_APPLY);
                Matrix_Position(&sp11C, &eff->pos);
                eff->pos.x += pl_shield_pos.x;
                eff->pos.y += pl_shield_pos.y;
                eff->pos.z += pl_shield_pos.z;

                if (eff->work[EFF_UNKS1] < 5) {
                    add_calc2(&eff->workf[EFF_DIST], 40.0f, 0.5f, 100.0f);
                } else {
                    add_calc2(&eff->workf[EFF_DIST], 0.0f, 0.2f, 5.0f);
                    if (eff->work[EFF_UNKS1] >= 11) {
                        eff->alpha -= 30;
                        if (eff->alpha <= 0) {
                            eff->alpha = 0;
                            eff->type = TWEFF_NONE;
                        }
                    }
                }

                tw_magic_eff_ct(play, &eff->pos, &zero, &zero, 10, eff->work[EFF_ARGS]);
            } else if (eff->type == TWEFF_RING) {
                if (eff->work[EFF_UNKS1] == 0) {
                    add_calc2(&eff->workf[EFF_SCALE], eff->workf[EFF_DIST], 0.05f, 1.0f);

                    if (eff->frame >= 16) {
                        eff->alpha -= 10;
                        if (eff->alpha <= 0) {
                            eff->alpha = 0;
                            eff->type = TWEFF_NONE;
                        }
                    }
                } else {
                    add_calc2(&eff->workf[EFF_SCALE], eff->workf[EFF_DIST], 0.1f, 2.0f);
                    eff->alpha -= 15;

                    if (eff->alpha <= 0) {
                        eff->alpha = 0;
                        eff->type = TWEFF_NONE;
                    }
                }
            } else if (eff->type == TWEFF_PLYR_FRZ) {
                if (eff->work[EFF_ARGS] < eff->frame) {
                    f32 phi_f0 = 1.0f;

                    if (eff->target != NULL || wepon_flag == 1) {
                        phi_f0 *= 3.0f;
                    }

                    add_calc2(&eff->workf[EFF_SCALE], 0.0f, 1.0f, 0.0005f * phi_f0);

                    if (eff->workf[EFF_SCALE] == 0.0f) {
                        eff->type = TWEFF_NONE;
                        if (eff->target == NULL) {
                            player->stateFlags2 &= ~PLAYER_STATE2_15;
                            pl_ice_flag = 0;
                        }
                    }
                } else {
                    if (wepon_flag == 1) {
                        eff->frame = 100;
                    }
                    add_calc2(&eff->workf[EFF_DIST], 0.8f, 0.2f, 0.04f);

                    if (eff->target == NULL) {
                        add_calc2(&eff->workf[EFF_SCALE], 0.012f, 1.0f, 0.002f);
                        eff->workf[EFF_ROLL] += eff->workf[EFF_DIST];

                        if (eff->workf[EFF_ROLL] >= 0.8f) {
                            eff->workf[EFF_ROLL] -= 0.8f;
                            player->stateFlags2 |= PLAYER_STATE2_15;
                        } else {
                            player->stateFlags2 &= ~PLAYER_STATE2_15;
                        }

                        if ((kotake->workf[UNK_F11] > 10.0f) && (kotake->workf[UNK_F11] < 200.0f)) {
                            eff->frame = 100;
                        }

                        if (!(play->gameplayFrames & 1)) {
                            play->damagePlayer(play, -1);
                        }
                    } else {
                        add_calc2(&eff->workf[EFF_SCALE], 0.042f, 1.0f, 0.002f);
                    }

                    if ((eff->workf[EFF_DIST] > 0.4f) && ((eff->frame & 7) == 0)) {
                        Vec3f spC0;
                        Vec3f spB4;
                        Vec3f spA8;
                        s16 spA6 = rnd_f(PLAYER_BODYPART_MAX - 0.1f);
                        f32 phi_f22;

                        if (eff->target == NULL) {
                            spC0.x = player->bodyPartsPos[spA6].x + rnd_fx(5.0f);
                            spC0.y = player->bodyPartsPos[spA6].y + rnd_fx(5.0f);
                            spC0.z = player->bodyPartsPos[spA6].z + rnd_fx(5.0f);
                            phi_f22 = 10.0f;
                        } else {
                            Actor* unk44 = eff->target;

                            spC0.x = unk44->world.pos.x + rnd_fx(40.0f);
                            spC0.y = unk44->world.pos.y + rnd_fx(40.0f);
                            spC0.z = unk44->world.pos.z + rnd_fx(40.0f);
                            phi_f22 = 20.0f;
                        }

                        spB4.x = 0.0f;
                        spB4.y = 0.0f;
                        spB4.z = 0.0f;
                        spA8.x = 0.0f;
                        spA8.y = 0.1f;
                        spA8.z = 0.0f;

                        tw_eff_smoke_ct(play, TWEFF_3, &spC0, &spB4, &spA8, phi_f22 + rnd_f(phi_f22 * 0.5f),
                                           0, 0, 150);
                    }
                }
            }
        }
        eff++;
    }
}

static s32 rnd20;
static s32 rnd21;
static s32 rnd22;

void init_stage_rnd(s32 seed0, s32 seed1, s32 seed2) {
    rnd20 = seed0;
    rnd21 = seed1;
    rnd22 = seed2;
}

f32 stage_rnd(void) {
    f32 rand;

    // Wichmann-Hill algorithm
    rnd20 = (rnd20 * 171) % 30269;
    rnd21 = (rnd21 * 172) % 30307;
    rnd22 = (rnd22 * 170) % 30323;

    rand = (rnd20 / 30269.0f) + (rnd21 / 30307.0f) + (rnd22 / 30323.0f);
    while (rand >= 1.0f) {
        rand -= 1.0f;
    }

    return fabsf(rand);
}

void Tw_Eff_disp(PlayState* play) {
    u8 materialFlag = 0;
    s16 i;
    s16 j;
    s32 pad;
    Player* player = GET_PLAYER(play);
    s16 phi_s4;
    BossTwEffect* currentEffect;
    BossTwEffect* effectHead;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    currentEffect = play->specialEffects;
    effectHead = currentEffect;

    OPEN_DISPS(gfxCtx, "../z_boss_tw.c", 9592);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    for (i = 0; i < BOSS_TW_EFFECT_COUNT; i++) {
        if (currentEffect->type == TWEFF_DOT) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, gTwinrovaMagicParticleMaterialDL);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, currentEffect->color.r, currentEffect->color.g,
                            currentEffect->color.b, currentEffect->alpha);
            Matrix_translate(currentEffect->pos.x, currentEffect->pos.y, currentEffect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(currentEffect->workf[EFF_SCALE], currentEffect->workf[EFF_SCALE], 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_tw.c", 9617);
            gSPDisplayList(POLY_XLU_DISP++, gTwinrovaMagicParticleModelDL);
        }

        currentEffect++;
    }

    materialFlag = 0;
    currentEffect = effectHead;

    for (i = 0; i < BOSS_TW_EFFECT_COUNT; i++) {
        if (currentEffect->type == TWEFF_3) {
            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIceMaterialDL));
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, currentEffect->alpha);
            gSPSegment(POLY_XLU_DISP++, 8,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (currentEffect->frame * 3) & 0x7F,
                                        (currentEffect->frame * 15) & 0xFF, 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
            Matrix_translate(currentEffect->pos.x, currentEffect->pos.y, currentEffect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(currentEffect->workf[EFF_SCALE], currentEffect->workf[EFF_SCALE], 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_tw.c", 9660);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIceModelDL));
        }
        currentEffect++;
    }

    materialFlag = 0;
    currentEffect = effectHead;

    for (i = 0; i < BOSS_TW_EFFECT_COUNT; i++) {
        if (currentEffect->type == TWEFF_2) {
            if (materialFlag == 0) {
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, currentEffect->alpha);
            gSPSegment(POLY_XLU_DISP++, 8,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (currentEffect->frame * 3) & 0x7F,
                                        (currentEffect->frame * 15) & 0xFF, 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
            Matrix_translate(currentEffect->pos.x, currentEffect->pos.y, currentEffect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(currentEffect->workf[EFF_SCALE], currentEffect->workf[EFF_SCALE], 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_tw.c", 9709);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaFireDL));
        }

        currentEffect++;
    }

    materialFlag = 0;
    currentEffect = effectHead;

    for (i = 0; i < BOSS_TW_EFFECT_COUNT; i++) {
        if (currentEffect->type == TWEFF_RING) {
            if (materialFlag == 0) {
                materialFlag++;
            }

            gSPSegment(POLY_XLU_DISP++, 0xD,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, currentEffect->frame & 0x7F,
                                        (currentEffect->frame * 8) & 0xFF, 0x20, 0x40, 1,
                                        (currentEffect->frame * -2) & 0x7F, 0, 0x10, 0x10));

            if (currentEffect->work[EFF_ARGS] == 1) {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 65, 0, currentEffect->alpha);
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 0, 128);
            } else {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, currentEffect->alpha);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, 128);
            }

            Matrix_translate(currentEffect->pos.x, currentEffect->pos.y, currentEffect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);

            if (currentEffect->work[EFF_UNKS1] == 0) {
                Matrix_translate(0.0f, 0.0f, 60.0f, MTXMODE_APPLY);
            } else {
                Matrix_translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            }

            Matrix_rotateZ(currentEffect->workf[EFF_ROLL], MTXMODE_APPLY);
            Matrix_rotateX(M_PI / 2.0f, MTXMODE_APPLY);
            Matrix_scale(currentEffect->workf[EFF_SCALE], 1.0f, currentEffect->workf[EFF_SCALE], MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_tw.c", 9775);
            gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
            gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BACK | G_FOG);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaEffectHaloDL));
        }

        currentEffect++;
    }

    materialFlag = 0;
    currentEffect = effectHead;

    for (i = 0; i < BOSS_TW_EFFECT_COUNT; i++) {
        if (currentEffect->type == TWEFF_PLYR_FRZ) {
            Actor* actor;
            Vec3f off;

            if (materialFlag == 0) {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIceSurroundingPlayerMaterialDL));
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, 255);
                gSPSegment(
                    POLY_XLU_DISP++, 8,
                    two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
                materialFlag++;
                init_stage_rnd(1, 0x71AC, 0x263A);
            }

            actor = currentEffect->target;
            phi_s4 = actor == NULL ? 70 : 20;

            for (j = 0; j < phi_s4; j++) {
                off.x = (stage_rnd() - 0.5f) * 30.0f;
                off.y = currentEffect->workf[EFF_DIST] * j;
                off.z = (stage_rnd() - 0.5f) * 30.0f;

                if (actor != NULL) {
                    Matrix_translate(actor->world.pos.x + off.x, actor->world.pos.y + off.y, actor->world.pos.z + off.z,
                                     MTXMODE_NEW);
                } else {
                    Matrix_translate(player->actor.world.pos.x + off.x, player->actor.world.pos.y + off.y,
                                     player->actor.world.pos.z + off.z, MTXMODE_NEW);
                }

                Matrix_scale(currentEffect->workf[EFF_SCALE], currentEffect->workf[EFF_SCALE],
                             currentEffect->workf[EFF_SCALE], MTXMODE_APPLY);
                Matrix_rotateY(stage_rnd() * M_PI, MTXMODE_APPLY);
                Matrix_rotateX((stage_rnd() - 0.5f) * M_PI * 0.5f, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_tw.c", 9855);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIceModelDL));
            }
        }

        currentEffect++;
    }

    materialFlag = 0;
    currentEffect = effectHead;

    for (i = 0; i < BOSS_TW_EFFECT_COUNT; i++) {
        if (currentEffect->type >= TWEFF_FLAME) {
            if (currentEffect->work[EFF_ARGS] == 0) {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 195, 225, 235, currentEffect->alpha);
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIceMaterialDL));
            } else {
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, currentEffect->alpha);
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);
            }

            gSPSegment(POLY_XLU_DISP++, 8,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (currentEffect->frame * 3) & 0x7F,
                                        (-currentEffect->frame * 15) & 0xFF, 0x20, 0x40, 1, 0, 0, 0x20, 0x20));
            Matrix_translate(currentEffect->pos.x, currentEffect->pos.y, currentEffect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(currentEffect->workf[EFF_SCALE], currentEffect->workf[EFF_SCALE], 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_tw.c", 9911);

            if (currentEffect->work[EFF_ARGS] == 0) {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaIceModelDL));
            } else {
                gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gTwinrovaFireDL));
            }
        }

        currentEffect++;
    }

    CLOSE_DISPS(gfxCtx, "../z_boss_tw.c", 9920);
}

void mode_g_stop_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_g_stop;
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaHoverAnim, -3.0f);
    this->work[CS_TIMER_1] = rnd_f(100.0f);
    this->timers[1] = 25;
    this->rotateSpeed = 0.0f;
}

void mode_g_stop(BossTw* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc2(&this->actor.world.pos.x, this->targetPos.x, 0.1f, fabsf(this->actor.velocity.x) * 1.5f);
    add_calc2(&this->actor.world.pos.y, this->targetPos.y, 0.1f, fabsf(this->actor.velocity.y) * 1.5f);
    add_calc2(&this->targetPos.y, 380.0f, 1.0f, 2.0f);
    add_calc2(&this->actor.world.pos.z, this->targetPos.z, 0.1f, fabsf(this->actor.velocity.z) * 1.5f);

    if (this->timers[1] == 1) {
        mode_g_tame_init(this, play);
    }

    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, this->rotateSpeed);
    add_calc2(&this->rotateSpeed, 4096.0f, 1.0f, 350.0f);
}

void mode_g_tame_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_g_tame;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaWindUpAnim, -5.0f);
    this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaWindUpAnim);
    this->csState1 = 0;
}

void mode_g_tame(BossTw* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    add_calc2(&this->actor.world.pos.x, this->targetPos.x, 0.03f, fabsf(this->actor.velocity.x) * 1.5f);
    add_calc2(&this->actor.world.pos.y, this->targetPos.y, 0.03f, fabsf(this->actor.velocity.y) * 1.5f);
    add_calc2(&this->actor.world.pos.z, this->targetPos.z, 0.03f, fabsf(this->actor.velocity.z) * 1.5f);
    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000);

    if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
        if ((s8)this->actor.colChkInfo.health < 10) {
            gattai_wepon_switch = rnd_f(1.99f);
        } else {
            if (++gattai_wepon_count >= 4) {
                gattai_wepon_count = 1;
                gattai_wepon_switch2 = !gattai_wepon_switch2;
            }

            gattai_wepon_switch = gattai_wepon_switch2;
        }

        mode_g_atack_init(this, play);
    }
}

void mode_g_atack_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_g_atack;

    if (gattai_wepon_switch == 0) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaIceAttackAnim, 0.0f);
    } else {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaFireAttackAnim, 0.0f);
    }

    this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaFireAttackAnim);
}

void mode_g_atack(BossTw* this, PlayState* play) {
    BossTw* twMagic;
    Vec3f* magicSpawnPos;
    s32 magicParams;
    s16 i;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Skeleton_Info_frame_check(&this->skelAnime, 8.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_THROW_MASIC);
        Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_YOUNG_SHOOTVC);
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, 12.0f)) {
        if (gattai_wepon_switch != 0) {
            magicParams = TW_FIRE_BLAST;
            magicSpawnPos = &this->rightScepterPos;
        } else {
            magicParams = TW_ICE_BLAST;
            magicSpawnPos = &this->leftScepterPos;
        }

        twMagic = (BossTw*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_TW, magicSpawnPos->x,
                                              magicSpawnPos->y, magicSpawnPos->z, 0, 0, 0, magicParams);

        if (twMagic != NULL) {
            twMagic->blastType = magicParams == TW_ICE_BLAST ? 0 : 1;
        }

        kankyo_flag = twMagic->blastType + 1;

        if (1) {
            Vec3f velocity = { 0.0f, 0.0f, 0.0f };
            Vec3f accel = { 0.0f, 0.0f, 0.0f };

            for (i = 0; i < 100; i++) {
                velocity.x = rnd_fx(30.0f);
                velocity.y = rnd_fx(30.0f);
                velocity.z = rnd_fx(30.0f);
                tw_hinoko_ct(play, magicSpawnPos, &velocity, &accel, (s16)rnd_f(2.0f) + 11,
                                    twMagic->blastType, 75);
            }
        }
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
        mode_g_after_atack_init(this, play);
    }

    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000);
}

void mode_g_after_atack_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_g_after_atack;
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaHoverAnim, -10.0f);
    this->timers[1] = 60;
}

void mode_g_after_atack(BossTw* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timers[1] == 0 && pl_final_beam_on == 0) {
        if (gattai->timers[2] == 0) {
            mode_g_move_init(this, play);
        } else {
            mode_g_ufufu_init(this, play);
        }
    }

    pl_final_beam_on = 0;
    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000);
}

void mode_g_damage_init(BossTw* this, PlayState* play, u8 damage) {
    if (this->actionFunc != mode_g_damage) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaChargedAttackHitAnim, -15.0f);
        this->timers[0] = 150;
        this->timers[1] = 20;
        this->csState1 = 0;
        this->actor.velocity.y = 0.0f;
    } else {
        this->work[FOG_TIMER] = 10;
        this->work[INVINC_TIMER] = 20;
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaDamageAnim, -3.0f);
        this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaDamageAnim);
        this->csState1 = 1;

        if ((s8)(this->actor.colChkInfo.health -= damage) < 0) {
            this->actor.colChkInfo.health = 0;
        }

        if ((s8)this->actor.colChkInfo.health <= 0) {
            mode_g_enddemo_init(this, play);
            Actor_info_finish(play, &this->actor);
            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_YOUNG_DEAD);
            return;
        }

        Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_YOUNG_DAMAGE2);
        Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_CUTBODY);
    }

    this->actionFunc = mode_g_damage;
}

void mode_g_damage(BossTw* this, PlayState* play) {
    s16 cloudType;

    this->unk_5F8 = 1;
    this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;

    cloudType = gattai_wepon_switch == 0 ? TWEFF_3 : TWEFF_2;

    if ((this->work[CS_TIMER_1] % 8) == 0) {
        Vec3f pos;
        Vec3f velocity;
        Vec3f accel;
        pos.x = this->actor.world.pos.x + rnd_fx(20.0f);
        pos.y = this->actor.world.pos.y + rnd_fx(40.0f) + 20;
        pos.z = this->actor.world.pos.z + rnd_fx(20.0f);
        velocity.x = 0.0f;
        velocity.y = 0.0f;
        velocity.z = 0.0f;
        accel.x = 0.0f;
        accel.y = 0.1f;
        accel.z = 0.0f;
        tw_eff_smoke_ct(play, cloudType, &pos, &velocity, &accel, rnd_f(5.0f) + 10.0f, 0, 0, 150);
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    this->work[UNK_S8] += 20;

    if (this->work[UNK_S8] > 255) {
        this->work[UNK_S8] = 255;
    }

    add_calc2(&this->workf[UNK_F12], 0.0f, 1.0f, 0.05f);
    this->actor.world.pos.y += this->actor.velocity.y;
    add_calc2(&this->actor.velocity.y, -5.0f, 1.0f, 0.5f);
    this->actor.world.pos.y -= 30.0f;
    Actor_BGcheck2(play, &this->actor, 50.0f, 50.0f, 100.0f, UPDBGCHECKINFO_FLAG_2);
    this->actor.world.pos.y += 30.0f;

    if (this->csState1 == 0) {
        if (this->timers[1] == 0) {
            this->csState1 = 1;
            this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaStunStartAnim);
            Skeleton_Info2_init(&this->skelAnime, &gTwinrovaStunStartAnim, 1.0f, 0.0f, this->workf[ANIM_SW_TGT], 3, 0.0f);
        }
    } else if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
        this->workf[ANIM_SW_TGT] = 1000.0f;
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaStunLoopAnim, 0.0f);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.velocity.y = 0.0f;
    }

    if (this->timers[0] == 0) {
        mode_g_after_damage_init(this, play);
    }
}

void mode_g_after_damage_init(BossTw* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaStunEndAnim, 0.0f);
    this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaStunEndAnim);
    this->actionFunc = mode_g_after_damage;
    this->timers[0] = 50;
}

void mode_g_after_damage(BossTw* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc2(&this->actor.world.pos.y, this->targetPos.y, 0.05f, 5.0f);

    if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
        this->workf[ANIM_SW_TGT] = 1000.0f;
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaHoverAnim, 0.0f);
    }

    if (this->timers[0] == 0) {
        mode_g_move_init(this, play);
    }
}

void mode_g_move_init(BossTw* this, PlayState* play) {
    f32 xDiff;
    f32 zDiff;
    f32 yDiff;
    f32 xzDist;
    Player* player = GET_PLAYER(play);

    do {
        this->work[TW_PLLR_IDX] += (s16)(((s16)rnd_f(2.99f)) + 1);
        this->work[TW_PLLR_IDX] %= 4;
        this->targetPos = tw_set_pos[this->work[TW_PLLR_IDX]];
        xDiff = this->targetPos.x - player->actor.world.pos.x;
        zDiff = this->targetPos.z - player->actor.world.pos.z;
        xzDist = SQ(xDiff) + SQ(zDiff);
    } while (!(xzDist > SQ(300.0f)));

    this->targetPos.y = 480.0f;
    xDiff = this->targetPos.x - this->actor.world.pos.x;
    yDiff = this->targetPos.y - this->actor.world.pos.y;
    zDiff = this->targetPos.z - this->actor.world.pos.z;
    this->actionFunc = mode_g_move;
    this->rotateSpeed = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = RAD_TO_BINANG(fatan2(xDiff, zDiff));
    xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
    this->actor.world.rot.x = RAD_TO_BINANG(fatan2(yDiff, xzDist));
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaHoverAnim, -10.0f);
}

void mode_g_move(BossTw* this, PlayState* play) {
    f32 xDiff;
    f32 yDiff;
    f32 zDiff;
    s32 pad;
    f32 yaw;
    f32 xzDist;

    Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
    Skeleton_Info2_anime_play(&this->skelAnime);
    xDiff = this->targetPos.x - this->actor.world.pos.x;
    yDiff = this->targetPos.y - this->actor.world.pos.y;
    zDiff = this->targetPos.z - this->actor.world.pos.z;
    yaw = RAD_TO_BINANG(fatan2(xDiff, zDiff));
    xzDist = sqrtf(SQ(xDiff) + SQ(zDiff));
    adds(&this->actor.world.rot.x, (f32)RAD_TO_BINANG(fatan2(yDiff, xzDist)), 0xA, this->rotateSpeed);
    adds(&this->actor.world.rot.y, yaw, 0xA, this->rotateSpeed);
    adds(&this->actor.shape.rot.y, yaw, 0xA, this->rotateSpeed);
    add_calc2(&this->rotateSpeed, 2000.0f, 1.0f, 100.0f);
    add_calc2(&this->actor.speed, 30.0f, 1.0f, 2.0f);
    Actor_position_speed_set_XY(&this->actor);
    add_calc2(&this->actor.world.pos.x, this->targetPos.x, 0.1f, fabsf(this->actor.velocity.x) * 1.5f);
    add_calc2(&this->actor.world.pos.y, this->targetPos.y, 0.1f, fabsf(this->actor.velocity.y) * 1.5f);
    add_calc2(&this->targetPos.y, 380.0f, 1.0f, 2.0f);
    add_calc2(&this->actor.world.pos.z, this->targetPos.z, 0.1f, fabsf(this->actor.velocity.z) * 1.5f);

    if (xzDist < 200.0f) {
        mode_g_stop_init(this, play);
    }
}

void mode_g_guard_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_g_guard;
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gTwinrovaHoverAnim, 0.0f);
    this->timers[0] = 20;
    this->actor.speed = 0.0f;
}

void mode_g_guard(BossTw* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timers[0] != 0) {
        this->collider.base.colMaterial = COL_MATERIAL_METAL;
        this->actor.shape.rot.y -= 0x3000;

        if ((this->timers[0] % 4) == 0) {
            Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_ROLL);
        }
    } else {
        mode_g_move_init(this, play);
    }
}

void mode_g_ufufu_init(BossTw* this, PlayState* play) {
    this->actionFunc = mode_g_ufufu;
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gTwinrovaLaughAnim, 0.0f);
    this->workf[ANIM_SW_TGT] = Si2_anime_end_frame(&gTwinrovaLaughAnim);
    this->actor.speed = 0.0f;
}

void mode_g_ufufu(BossTw* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Skeleton_Info_frame_check(&this->skelAnime, 10.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_TWINROBA_YOUNG_LAUGH);
    }

    if (Skeleton_Info_frame_check(&this->skelAnime, this->workf[ANIM_SW_TGT])) {
        mode_g_move_init(this, play);
    }
}
