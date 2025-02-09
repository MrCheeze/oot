/*
 * File: z_boss_va.c
 * Overlay: ovl_Boss_Va
 * Description: Barinade
 */

#include "z_boss_va.h"
#include "assets/objects/object_bv/object_bv.h"
#include "overlays/actors/ovl_En_Boom/z_en_boom.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#pragma increment_block_number "gc-eu:128 gc-eu-mq:128 gc-jp:128 gc-jp-ce:128 gc-jp-mq:128 gc-us:128 gc-us-mq:128" \
                               "ntsc-1.2:128 pal-1.0:128 pal-1.1:128 hiratsu3:128"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define GET_BODY(this) ((BossVa*)(this)->actor.parent)
#define vaGorePulse offset.x
#define vaGorePulseRate offset.y
#define vaBariUnused headRot
#define vaCamRotMod headRot.x
#define vaBodySpinRate headRot.y

#define PHASE_2 3
#define PHASE_3 9
#define PHASE_4 15
#define PHASE_DEATH 18

#define BOSS_VA_EFFECT_COUNT 400

typedef struct BossVaEffect {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ Vec3f accel;
    /* 0x24 */ u8 type;
    /* 0x26 */ u16 timer;
    /* 0x28 */ s16 mode;
    /* 0x2A */ Vec3s rot;
    /* 0x30 */ s16 primColor[4];
    /* 0x38 */ s16 envColor[4];
    /* 0x40 */ f32 scale;
    /* 0x44 */ f32 scaleMod;
    /* 0x48 */ Vec3f offset;
    /* 0x54 */ struct BossVa* parent;
} BossVaEffect; // size = 0x58

typedef enum BossVaEffectType {
    /* 0 */ VA_NONE,
    /* 1 */ VA_LARGE_SPARK,
    /* 2 */ VA_BLAST_SPARK,
    /* 3 */ VA_SMALL_SPARK,
    /* 4 */ VA_SPARK_BALL,
    /* 5 */ VA_ZAP_CHARGE,
    /* 6 */ VA_BLOOD,
    /* 7 */ VA_TUMOR,
    /* 8 */ VA_GORE
} BossVaEffectType;

typedef enum BossVaSparkMode {
    /* 1 */ SPARK_TETHER = 1,
    /* 2 */ SPARK_BARI,
    /* 3 */ SPARK_BLAST,
    /* 4 */ SPARK_UNUSED,
    /* 5 */ SPARK_BODY,
    /* 6 */ SPARK_LINK
} BossVaSparkMode;

typedef enum BossVaBloodMode {
    /* 0 */ BLOOD_DROPLET,
    /* 1 */ BLOOD_SPLATTER,
    /* 2 */ BLOOD_SPOT
} BossVaBloodMode;

typedef enum BossVaTumorMode {
    /* 0 */ TUMOR_UNUSED,
    /* 1 */ TUMOR_BODY,
    /* 2 */ TUMOR_ARM
} BossVaTumorMode;

typedef enum BossVaGoreMode {
    /* 0 */ GORE_PERMANENT,
    /* 1 */ GORE_FLOOR,
    /* 2 */ GORE_FADING
} BossVaGoreMode;

typedef enum BossVaCutscene {
    /* -5 */ INTRO_UNUSED_START = -5,
    /* -4 */ INTRO_START,
    /* -3 */ INTRO_LOOK_DOOR,
    /* -2 */ INTRO_CLOSE_DOOR,
    /* -1 */ INTRO_DOOR_SHUT,
    /*  0 */ INTRO_CRACKLE,
    /*  1 */ INTRO_SPAWN_BARI,
    /*  2 */ INTRO_LOOK_BARI,
    /*  3 */ INTRO_REVERSE_CAMERA,
    /*  4 */ INTRO_SUPPORT_CAMERA,
    /*  5 */ INTRO_BODY_SOUND,
    /*  6 */ INTRO_LOOK_SUPPORT,
    /*  7 */ INTRO_UNUSED_CALL_BARI,
    /*  8 */ INTRO_CALL_BARI,
    /*  9 */ INTRO_ATTACH_BARI,
    /* 10 */ INTRO_TITLE,
    /* 11 */ INTRO_BRIGHTEN,
    /* 12 */ INTRO_FINISH,
    /* 13 */ BOSSVA_BATTLE,
    /* 14 */ DEATH_START,
    /* 15 */ DEATH_BODY_TUMORS,
    /* 16 */ DEATH_ZAPPER_1,
    /* 17 */ DEATH_ZAPPER_2,
    /* 18 */ DEATH_ZAPPER_3,
    /* 19 */ DEATH_SHELL_BURST,
    /* 20 */ DEATH_CORE_TUMORS,
    /* 21 */ DEATH_CORE_DEAD,
    /* 22 */ DEATH_CORE_BURST,
    /* 23 */ DEATH_MUSIC,
    /* 24 */ DEATH_FINISH
} BossVaCutscene;

void Boss_va_Actor_ct(Actor* thisx, PlayState* play2);
void Boss_va_Actor_dt(Actor* thisx, PlayState* play);
void Boss_va_move(Actor* thisx, PlayState* play2);
void Boss_va_display(Actor* thisx, PlayState* play);

static void Boss_Eff_move(PlayState* play);
static void Boss_Eff_disp(BossVaEffect* effect, PlayState* play);
void bdan_bdoor_draw(PlayState* play, s16 scale);

void Boss_va_Actor_mode_core_demo00_init(BossVa* this);
void Boss_va_Actor_mode_core_move00_init(BossVa* this);
void Boss_va_Actor_mode_core_move01_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_core_move02_init(BossVa* this);
void Boss_va_Actor_mode_core_move03_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_core_move04_init(BossVa* this, PlayState* play);

void Boss_va_Actor_mode_arm_demo00_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_arm_move00_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_arm_move01_init(BossVa* this, PlayState* play);

void Boss_va_Actor_mode_earm_demo00_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_earm_move00_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_earm_move03_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_earm_move01_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_earm_move04_init(BossVa* this, PlayState* play);

void Boss_va_Actor_mode_arm_move02_init(BossVa* this, PlayState* play);

void Boss_va_Actor_mode_door_move00_init(BossVa* this, PlayState* play);

void Boss_va_Actor_mode_bl2_demo00_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_bl2_move0_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_bl2_move_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_bl2_move2_init(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_bl2_move3_init(BossVa* this);

void Boss_va_Actor_mode_core_demo00(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_core_move00(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_core_move01(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_core_move02(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_core_move03(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_core_move04(BossVa* this, PlayState* play);

void Boss_va_Actor_mode_arm_demo00(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_arm_move00(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_arm_move01(BossVa* this, PlayState* play);

void Boss_va_Actor_mode_earm_demo00(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_earm_move00(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_earm_move03(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_earm_move01(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_earm_move04(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_earm_move02(BossVa* this, PlayState* play);

void Boss_va_Actor_mode_arm_move02(BossVa* this, PlayState* play);

void Boss_va_Actor_mode_door_move00(BossVa* this, PlayState* play);

void Boss_va_Actor_mode_bl2_demo00(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_bl2_move(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_bl2_move0(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_bl2_move2(BossVa* this, PlayState* play);
void Boss_va_Actor_mode_bl2_move3(BossVa* this, PlayState* play);

void Effect_Blood_ct2(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 yaw, s16 scale);
void Effect_meet_ct(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 yaw, s16 scale);
void Effect_flash_mini_ct(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale, u8 mode);
void Effect_l_cv_ct(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* pos, Vec3s* rot, s16 scale,
                              u8 mode);
void Effect_s_core_ct(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale, u8 mode);
void Effect_lightning_ct(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale, u8 mode);
void Effect_Blood_ct(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 scale, s16 phase, s16 yaw);
void s_core(PlayState* play, BossVa* this, s32 count, s16 scale, f32 xzSpread, f32 ySpread, u8 mode, f32 range,
                  u8 fixed);

ActorProfile Boss_Va_Profile = {
    /**/ ACTOR_BOSS_VA,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_BV,
    /**/ sizeof(BossVa),
    /**/ Boss_va_Actor_ct,
    /**/ Boss_va_Actor_dt,
    /**/ Boss_va_move,
    /**/ Boss_va_display,
};

static ColliderCylinderInit OcInfoCoreData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFEF, 0x03, 0x08 },
        { 0x00000010, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 85, 120, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit JntSphArmElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000010, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 25 }, 100 },
    },
};

static ColliderJntSphInit JntSphArmData = {
    {
        COL_MATERIAL_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(JntSphArmElemData),
    JntSphArmElemData,
};

static ColliderJntSphElementInit JntSphBl2ElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x03, 0x04 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 30 }, 100 },
    },
};

static ColliderJntSphInit JntSphBl2Data = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(JntSphBl2ElemData),
    JntSphBl2ElemData,
};

static ColliderQuadInit AtInfoData = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x03, 0x04 },
        { 0x00000010, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static Vec3f parts_pos[] = {
    { 0.0f, 175.35f, 0.0f },       // Support 1
    { 0.0f, 175.35f, 0.0f },       // Support 2
    { 0.0f, 175.35f, 0.0f },       // Support 3
    { 120.0f, 103.425f, -67.0f },  // Zapper 1
    { 0.0f, 103.425f, 140.0f },    // Zapper 2
    { -120.0f, 103.425f, -70.0f }, // Zapper 3
    { -2.0f, 16.0f, 50.0f },       // Upper Bari 1
    { 48.0f, 16.0f, 15.0f },       // Upper Bari 2
    { 25.0f, 16.0f, -36.0f },      // Upper Bari 3
    { -29.0f, 16.0f, -36.0f },     // Upper Bari 4
    { -63.0f, 16.0f, 22.0f },      // Upper Bari 5
    { 0.0f, -10.0f, -64.0f },      // Lower Bari 1
    { 63.0f, -10.0f, -22.0f },     // Lower Bari 2
    { 35.0f, -10.0f, 46.0f },      // Lower Bari 3
    { -36.0f, -10.0f, 46.0f },     // Lower Bari 4
    { -49.0f, -10.0f, -17.0f },    // Lower Bari 5
    { 0.0f, 160.0f, 370.0f },      // Upper Bari 1 CS
    { 65.0f, 35.0f, 370.0f },      // Upper Bari 2 CS
    { 80.0f, 70.0f, -130.0f },     // Upper Bari 3 CS
    { -160.0f, 100.0f, -130.0f },  // Upper Bari 4 CS
    { -150.0f, 130.0f, 0.0f },     // Upper Bari 5 CS
    { 230.0f, 0.0f, 0.0f },        // Lower Bari 1 CS
    { 60.0f, 140.0f, 0.0f },       // Lower Bari 2 CS
    { 0.0f, 40.0f, 270.0f },       // Lower Bari 3 CS
    { -100.0f, 10.0f, 200.0f },    // Lower Bari 4 CS
    { -90.0f, 70.0f, -310.0f },    // Lower Bari 5 CS
};

static Vec3s parts_ang[] = {
    { 0x1FFE, 0x0000, 0x0000 }, { 0x1FFE, 0x5550, 0x0000 }, { 0x1FFE, 0xAAB0, 0x0000 }, { 0xD558, 0x5550, 0x0000 },
    { 0xD558, 0x0000, 0x0000 }, { 0xD558, 0xAAB0, 0x0000 }, { 0x2AA8, 0xFCCC, 0x0000 }, { 0x2AA8, 0x3330, 0x0000 },
    { 0x2AA8, 0x6660, 0x0000 }, { 0x2AA8, 0x99A0, 0x0000 }, { 0x2AA8, 0xCCD0, 0x0000 }, { 0x4C98, 0x81D0, 0x0000 },
    { 0x4C98, 0x4F70, 0x0000 }, { 0x4C98, 0x1758, 0x0000 }, { 0x4C98, 0xE8A8, 0x0000 }, { 0x4C98, 0xB648, 0x0000 },
};

static Vec3f warp_pos[] = {
    { 10.0f, 0.0f, 30.0f },
    { 260.0f, 0.0f, -470.0f },
    { -240.0f, 0.0f, -470.0f },
};

static DamageTable btl_data[] = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0xE),
    /* Ice magic     */ DMG_ENTRY(0, 0x6),
    /* Light magic   */ DMG_ENTRY(0, 0xD),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static Vec3f dam_d = { 0.0f, 0.0f, 0.0f };
static u8 bl2_d = 0;
static u8 bl2_disp_flg[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static s16 c_no = SUB_CAM_ID_DONE;

static BossVaEffect boss_eff[BOSS_VA_EFFECT_COUNT];
static u8 arm_b;
static u8 bl2_flg;
static s8 demo_idx;
static Vec3f demo_camera_eye;
static Vec3f demo_camera_way;
static Vec3f demo_camera_eye2;
static Vec3f demo_camera_way2;
static Vec3f cam_spd;
static Vec3f cam_spd2;
static s16 d_para;
static u8 spd_flg;
static Vec3s e_ang_wk5;
static u16 m_timer;
static s8 hit_cnt;

void Boss_Va_actor_set_process(BossVa* this, BossVaActionFunc func) {
    this->actionFunc = func;
}

static void pos_set(BossVa* this) {
    BossVa* vaBody = GET_BODY(this);

    Matrix_translate(vaBody->actor.world.pos.x, vaBody->actor.world.pos.y, vaBody->actor.world.pos.z, MTXMODE_NEW);
    Matrix_rotateXYZ(vaBody->actor.shape.rot.x, 0, vaBody->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_Position(&parts_pos[this->actor.params], &this->actor.world.pos);

    switch (this->actor.params) {
        case BOSSVA_SUPPORT_1:
        case BOSSVA_SUPPORT_2:
        case BOSSVA_SUPPORT_3:
            if (!this->onCeiling) {
                this->actor.shape.rot.x = parts_ang[this->actor.params].x + vaBody->actor.shape.rot.x;
                this->actor.shape.rot.y = parts_ang[this->actor.params].y;
                this->actor.shape.rot.z = parts_ang[this->actor.params].z + vaBody->actor.shape.rot.z;
            }
            break;

        case BOSSVA_ZAPPER_1:
        case BOSSVA_ZAPPER_2:
        case BOSSVA_ZAPPER_3:
            this->actor.shape.rot.y = parts_ang[this->actor.params].y;
            this->actor.shape.rot.x = (parts_ang[this->actor.params].x +
                                       (s16)(cos_s(parts_ang[this->actor.params].y) * vaBody->actor.shape.rot.x)) -
                                      (s16)(sin_s(parts_ang[this->actor.params].y) * vaBody->actor.shape.rot.z);
            this->actor.shape.rot.z = (s16)(cos_s(parts_ang[this->actor.params].y) * vaBody->actor.shape.rot.z) +
                                      (parts_ang[this->actor.params].z +
                                       (s16)(sin_s(parts_ang[this->actor.params].y) * vaBody->actor.shape.rot.x));
            break;
    }

    this->actor.world.rot = this->actor.shape.rot;
    this->actor.shape.yOffset = GET_BODY(this)->actor.shape.yOffset;
}

void setGreenBlood(PlayState* play, Vec3f* pos, s16 phase, s16 yaw) {
    s32 i;
    Vec3f spawnPos;

    for (i = 2; i > 0; i--) {
        spawnPos.x = rnd_fx(10.0f) + pos->x;
        spawnPos.y = pos->y - (fqrand() * 15.0f);
        spawnPos.z = rnd_fx(10.0f) + pos->z;
        Effect_Blood_ct(play, boss_eff, &spawnPos, 65, phase, yaw);
    }
}

void setGreenBlood2(PlayState* play, BossVaEffect* src, s16 yaw, s16 scale, s32 count) {
    s32 i;
    Vec3f pos;

    for (i = count; i > 0; i--) {
        pos.x = rnd_fx(10.0f) + src->pos.x;
        pos.y = src->pos.y - (fqrand() * 15.0f);
        pos.z = rnd_fx(10.0f) + src->pos.z;
        Effect_Blood_ct2(play, boss_eff, &pos, (s16)rnd_fx(0x6590) + yaw, scale);
    }
}

void setMeet(PlayState* play, BossVaEffect* src, s16 yaw, s16 scale) {
    s32 i;
    Vec3f pos;

    for (i = (demo_idx <= DEATH_SHELL_BURST) ? 2 : 1; i > 0; i--) {
        pos.x = rnd_fx(10.0f) + src->pos.x;
        pos.y = rnd_fx(10.0f) + src->pos.y;
        pos.z = rnd_fx(10.0f) + src->pos.z;
        Effect_meet_ct(play, boss_eff, &pos, (s16)rnd_fx(0x6590) + yaw, scale);
    }
}

void lightning2(PlayState* play, BossVa* this, s32 count, s16 scale, f32 xzSpread, f32 ySpread, u8 mode, f32 range,
                  u8 fixed) {
    s32 i;
    s16 index;
    Vec3f offset;

    for (i = count; i > 0; i--) {
        if (!fixed) {
            index = fqrand() * (range - 0.6f);
        } else {
            index = range - 0.6f;
        }
        offset.x = rnd_fx(xzSpread) + this->effectPos[index].x - this->actor.world.pos.x;
        offset.y = rnd_fx(ySpread) + this->effectPos[index].y - this->actor.world.pos.y;
        offset.z = rnd_fx(xzSpread) + this->effectPos[index].z - this->actor.world.pos.z;
        Effect_flash_mini_ct(play, boss_eff, this, &offset, scale, mode);
    }
}

void s_core(PlayState* play, BossVa* this, s32 count, s16 scale, f32 xzSpread, f32 ySpread, u8 mode, f32 range,
                  u8 fixed) {
    s16 index;
    s32 i;
    Vec3f offset;

    for (i = count; i > 0; i--) {
        if (!fixed) {
            index = fqrand() * (range - 0.6f);
        } else {
            index = range - 0.6f;
        }

        offset.x = rnd_fx(xzSpread) + this->effectPos[index].x - this->actor.world.pos.x;
        offset.y = rnd_fx(ySpread) + this->effectPos[index].y - this->actor.world.pos.y;
        offset.z = rnd_fx(xzSpread) + this->effectPos[index].z - this->actor.world.pos.z;
        Effect_s_core_ct(play, boss_eff, this, &offset, scale, mode);
    }
}

void light_set(PlayState* play) {
    play->envCtx.adjAmbientColor[0] = 10;
    play->envCtx.adjAmbientColor[1] = 10;
    play->envCtx.adjAmbientColor[2] = 10;
    play->envCtx.adjLight1Color[0] = 115;
    play->envCtx.adjLight1Color[1] = 65;
    play->envCtx.adjLight1Color[2] = 100;
    play->envCtx.adjFogColor[0] = 120;
    play->envCtx.adjFogColor[1] = 120;
    play->envCtx.adjFogColor[2] = 70;
}

void light_set2(PlayState* play) {
    play->envCtx.adjFogColor[0] = 220;
    play->envCtx.adjFogColor[1] = 220;
    play->envCtx.adjFogColor[2] = 150;
    play->envCtx.adjFogNear = -1000;
    play->envCtx.adjZFar = -900;
    play->envCtx.adjAmbientColor[0] = 200;
    play->envCtx.adjAmbientColor[1] = 200;
    play->envCtx.adjAmbientColor[2] = 200;
    play->envCtx.adjLight1Color[0] = 215;
    play->envCtx.adjLight1Color[1] = 165;
    play->envCtx.adjLight1Color[2] = 200;
    play->envCtx.screenFillColor[0] = 220;
    play->envCtx.screenFillColor[1] = 220;
    play->envCtx.screenFillColor[2] = 150;
    play->envCtx.screenFillColor[3] = 100;
}

EnBoom* BoomCheck(PlayState* play) {
    Actor* actorIt = play->actorCtx.actorLists[ACTORCAT_MISC].head;

    while (actorIt != NULL) {
        if (actorIt->id != ACTOR_EN_BOOM) {
            actorIt = actorIt->next;
            continue;
        }
        return (EnBoom*)actorIt;
    }
    return NULL;
}

void bl2_d_set(BossVa* this, PlayState* play) {
    s32 i;
    s16 scale;
    Vec3f pos;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };

    for (i = 7; i >= 0; i--) {
        pos.x = rnd_fx(60.0f) + this->actor.world.pos.x;
        pos.y =
            rnd_fx(50.0f) + (this->actor.world.pos.y + (this->actor.shape.yOffset * this->actor.scale.y));
        pos.z = rnd_fx(60.0f) + this->actor.world.pos.z;
        velocity.y = fqrand() + 1.0f;
        scale = get_random_timer(80, 100);
        if (fqrand() < 0.7f) {
            Effect_SS_Dt_Bubble_sc_co_ct(play, &pos, &velocity, &accel, scale, 25, 2, 1);
        } else {
            Effect_SS_Dt_Bubble_sc_co_ct(play, &pos, &velocity, &accel, scale, 25, 0, 1);
        }
    }

    bl2_flg++;
    Boss_va_Actor_mode_bl2_move3_init(this);
}

void Boss_va_Actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossVa* this = (BossVa*)thisx;
    s32 i;
    s16 warpId;

    Actor_set_scale(&this->actor, 0.1f);
    this->actor.attentionRangeType = ATTENTION_RANGE_5;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    switch (this->actor.params) {
        case BOSSVA_BODY:
            Skeleton_Info2_M_ct(play, &this->skelAnime, &gBarinadeBodySkel, &gBarinadeBodyAnim, NULL, NULL, 0);
            this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
            break;
        case BOSSVA_SUPPORT_1:
        case BOSSVA_SUPPORT_2:
        case BOSSVA_SUPPORT_3:
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBarinadeSupportSkel, &gBarinadeSupportAttachedAnim, NULL, NULL,
                               0);
            break;
        case BOSSVA_ZAPPER_1:
        case BOSSVA_ZAPPER_2:
        case BOSSVA_ZAPPER_3:
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBarinadeZapperSkel, &gBarinadeZapperIdleAnim, NULL, NULL, 0);
            break;
        case BOSSVA_STUMP_1:
        case BOSSVA_STUMP_2:
        case BOSSVA_STUMP_3:
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBarinadeStumpSkel, &gBarinadeStumpAnim, NULL, NULL, 0);
            break;
        default:
            this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
            Skeleton_Info2_M_ct(play, &this->skelAnime, &gBarinadeBariSkel, &gBarinadeBariAnim, NULL, NULL, 0);
            this->actor.shape.yOffset = 400.0f;
            break;
        case BOSSVA_DOOR:
            break;
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->onCeiling = false;
    this->actor.naviEnemyId = NAVI_ENEMY_BARINADE;

    switch (this->actor.params) {
        case BOSSVA_BODY:
            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_VA, 0.0f, 80.0f, 400.0f, 0, 0, 0,
                               BOSSVA_DOOR);
            if (Actor_Environment_room_clear_Check(play, play->roomCtx.curRoom.num)) {
                warpId = ACTOR_EN_RU1;
                if (GET_EVENTCHKINF(EVENTCHKINF_37)) {
                    warpId = ACTOR_DOOR_WARP1;
                }
                Actor_info_make_actor(&play->actorCtx, play, warpId, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, 0, 0,
                            0); //! params could be WARP_DUNGEON_CHILD however this can also spawn Ru1
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, this->actor.world.pos.x + 160.0f,
                            this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
                d_para = 100;
                Actor_delete(&this->actor);
            } else {
                this->actor.colChkInfo.damageTable = btl_data;
                m_timer = 0xFFFF;
                if (GET_EVENTCHKINF(EVENTCHKINF_BEGAN_BARINADE_BATTLE)) {
                    demo_idx = INTRO_CALL_BARI;
                    d_para = 100;
                    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
                    play->envCtx.screenFillColor[0] = 0xDC;
                    play->envCtx.screenFillColor[1] = 0xDC;
                    play->envCtx.screenFillColor[2] = 0xBE;
                    play->envCtx.screenFillColor[3] = 0xD2;
                    Demo_play_start(play, &play->csCtx);
                    c_no = Gama_play_make_camera(play);
                    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
                    Gama_play_set_camera_status(play, c_no, CAM_STAT_ACTIVE);
                    demo_camera_eye2.x = demo_camera_eye.x = 140.0f;
                    demo_camera_eye2.y = demo_camera_eye.y = 205.0f;
                    demo_camera_eye2.z = demo_camera_eye.z = -20.0f;
                    demo_camera_way2.x = demo_camera_way.x = 10.0f;
                    demo_camera_way2.y = demo_camera_way.y = 50.0f;
                    demo_camera_way2.z = demo_camera_way.z = -220.0f;
                    Gama_play_camera_setting(play, c_no, &demo_camera_way, &demo_camera_eye);
                    this->timer = 20;

                    for (i = BOSSVA_BARI_LOWER_5; i >= BOSSVA_BARI_UPPER_1; i--) {
                        Actor_info_make_child_actor(
                            &play->actorCtx, &this->actor, play, ACTOR_BOSS_VA,
                            parts_pos[i].x + this->actor.world.pos.x,
                            parts_pos[i].y + this->actor.world.pos.y,
                            parts_pos[i].z + this->actor.world.pos.z, parts_ang[i].x + this->actor.world.rot.x,
                            parts_ang[i].y + this->actor.world.rot.y, parts_ang[i].z + this->actor.world.rot.z, i);
                    }

                    cam_spd2 = cam_spd = dam_d;

                } else {
                    demo_idx = INTRO_START;
                    d_para = 5;
                }

                this->zapHeadPos.x = 1.0f;
                ClObjPipe_ct(play, &this->colliderBody);
                ClObjPipe_set5(play, &this->colliderBody, &this->actor, &OcInfoCoreData);

                for (i = BOSSVA_ZAPPER_3; i >= BOSSVA_SUPPORT_1; i--) {
                    Actor_info_make_child_actor(
                        &play->actorCtx, &this->actor, play, ACTOR_BOSS_VA,
                        parts_pos[i].x + this->actor.world.pos.x, parts_pos[i].y + this->actor.world.pos.y,
                        parts_pos[i].z + this->actor.world.pos.z, parts_ang[i].x + this->actor.world.rot.x,
                        parts_ang[i].y + this->actor.world.rot.y, parts_ang[i].z + this->actor.world.rot.z, i);
                }

                mem_clear((u8*)boss_eff, BOSS_VA_EFFECT_COUNT * sizeof(BossVaEffect), 0);
                if (demo_idx < BOSSVA_BATTLE) {
                    Boss_va_Actor_mode_core_demo00_init(this);
                } else {
                    Boss_va_Actor_mode_core_move00_init(this);
                }
            }
            break;
        case BOSSVA_SUPPORT_1:
        case BOSSVA_SUPPORT_2:
        case BOSSVA_SUPPORT_3:
            ClObjJntSph_ct(play, &this->colliderSph);
            ClObjJntSph_set5_nzm(play, &this->colliderSph, &this->actor, &JntSphArmData, this->elements);
            if (demo_idx < BOSSVA_BATTLE) {
                Boss_va_Actor_mode_arm_demo00_init(this, play);
            } else {
                Boss_va_Actor_mode_arm_move00_init(this, play);
            }
            this->onCeiling++;
            break;
        case BOSSVA_ZAPPER_1:
        case BOSSVA_ZAPPER_2:
        case BOSSVA_ZAPPER_3:
            ClObjSwrd_ct(play, &this->colliderLightning);
            ClObjSwrd_set5(play, &this->colliderLightning, &this->actor, &AtInfoData);
            if (demo_idx < BOSSVA_BATTLE) {
                Boss_va_Actor_mode_earm_demo00_init(this, play);
            } else {
                Boss_va_Actor_mode_earm_move00_init(this, play);
            }
            break;
        case BOSSVA_STUMP_1:
        case BOSSVA_STUMP_2:
        case BOSSVA_STUMP_3:
            Boss_va_Actor_mode_arm_move02_init(this, play);
            break;
        case BOSSVA_DOOR:
            Boss_va_Actor_mode_door_move00_init(this, play);
            break;
        default:
            ClObjJntSph_ct(play, &this->colliderSph);
            ClObjJntSph_set5_nzm(play, &this->colliderSph, &this->actor, &JntSphBl2Data, this->elements);
            ClObjSwrd_ct(play, &this->colliderLightning);
            ClObjSwrd_set5(play, &this->colliderLightning, &this->actor, &AtInfoData);
            this->unk_1D8.x = 1.0f;
            this->unk_1D8.y = 1.0f;
            if (demo_idx < BOSSVA_BATTLE) {
                Boss_va_Actor_mode_bl2_demo00_init(this, play);
            } else if (bl2_flg >= PHASE_3) {
                Boss_va_Actor_mode_bl2_move_init(this, play);
            } else {
                Boss_va_Actor_mode_bl2_move0_init(this, play);
            }
            break;
    }
}

void Boss_va_Actor_dt(Actor* thisx, PlayState* play) {
    BossVa* this = (BossVa*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjJntSph_dt_nzf(play, &this->colliderSph);
    ClObjPipe_dt(play, &this->colliderBody);
}

void Boss_va_Actor_mode_core_demo00_init(BossVa* this) {
    f32 lastFrame = Si2_anime_end_frame(&gBarinadeBodyAnim);

    Skeleton_Info2_init(&this->skelAnime, &gBarinadeBodyAnim, 1.0f, lastFrame, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->actor.shape.yOffset = -450.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_core_demo00);
}

void Boss_va_Actor_mode_core_demo00(BossVa* this, PlayState* play) {
    s32 i;
    Player* player = GET_PLAYER(play);

    this->unk_1AC += 0xC31;
    this->unk_1A0 = (cos_s(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (sin_s(this->unk_1AC) * 0.05f) + 1.0f;

    switch (demo_idx) {
        case INTRO_UNUSED_START:
            this->timer--;
            if (this->timer == 0) {
                demo_idx = INTRO_CLOSE_DOOR;
                this->timer = 10;
            }
            break;
        case INTRO_START:
            play->envCtx.screenFillColor[0] = 0xDC;
            play->envCtx.screenFillColor[1] = 0xDC;
            play->envCtx.screenFillColor[2] = 0xBE;
            play->envCtx.screenFillColor[3] = 0xD2;
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            player->actor.world.rot.y = player->actor.shape.rot.y = 0x7FFF;
            demo_idx++;
            break;
        case INTRO_LOOK_DOOR:
            Demo_play_start(play, &play->csCtx);
            if (c_no == SUB_CAM_ID_DONE) {
                c_no = Gama_play_make_camera(play);
            }
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, c_no, CAM_STAT_ACTIVE);

            demo_camera_eye2.x = demo_camera_eye.x = 13.0f;
            demo_camera_eye2.y = demo_camera_eye.y = 124.0f;
            demo_camera_eye2.z = demo_camera_eye.z = 167.0f;

            demo_camera_way2.x = demo_camera_way.x = player->actor.world.pos.x;
            demo_camera_way2.y = demo_camera_way.y = player->actor.world.pos.y;
            demo_camera_way2.z = demo_camera_way.z = player->actor.world.pos.z;

            cam_spd2 = cam_spd = dam_d;

            this->timer = 10;
            demo_idx++;
            break;
        case INTRO_CLOSE_DOOR:
            this->timer--;
            if (this->timer == 0) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_2);
                demo_idx++;
                this->timer = 30;
            }
            break;
        case INTRO_DOOR_SHUT:
            this->timer--;
            if (this->timer == 0) {
                demo_idx++;
            }
            if (fqrand() < 0.1f) {
                Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
            }
            break;
        case INTRO_CRACKLE:
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
            demo_idx++;
            break;
        case INTRO_SPAWN_BARI:
            Demo_play_start(play, &play->csCtx);
            if (c_no == SUB_CAM_ID_DONE) {
                c_no = Gama_play_make_camera(play);
            }
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, c_no, CAM_STAT_ACTIVE);

            demo_camera_eye2.x = demo_camera_eye.x = 13.0f;
            demo_camera_eye2.y = demo_camera_eye.y = 124.0f;
            demo_camera_eye2.z = demo_camera_eye.z = 167.0f;

            demo_camera_way2.x = demo_camera_way.x = player->actor.world.pos.x;
            demo_camera_way2.y = demo_camera_way.y = player->actor.world.pos.y;
            demo_camera_way2.z = demo_camera_way.z = player->actor.world.pos.z;

            cam_spd2 = cam_spd = dam_d;

            for (i = BOSSVA_BARI_LOWER_5; i >= BOSSVA_BARI_UPPER_1; i--) {
                Actor_info_make_child_actor(
                    &play->actorCtx, &this->actor, play, ACTOR_BOSS_VA, parts_pos[i].x + this->actor.world.pos.x,
                    parts_pos[i].y + this->actor.world.pos.y, parts_pos[i].z + this->actor.world.pos.z,
                    parts_ang[i].x + this->actor.world.rot.x, parts_ang[i].y + this->actor.world.rot.y,
                    parts_ang[i].z + this->actor.world.rot.z, i);
            }

            this->timer = 90;
            demo_idx++;
            break;
        case INTRO_REVERSE_CAMERA:
            demo_camera_eye2.x = -92.0f;
            demo_camera_eye2.y = 22.0f;
            demo_camera_eye2.z = 360.0f;
            demo_camera_way2.x = 63.0f;
            demo_camera_way2.y = 104.0f;
            demo_camera_way2.z = 248.0f;
            add_calc(&cam_spd.x, 7.0f, 0.3f, 0.7f, 0.05f);
            cam_spd.z = cam_spd.x;
            cam_spd.y = cam_spd.z;
            cam_spd2 = cam_spd;

            this->timer--;
            if (this->timer == 0) {
                demo_idx++;
                this->timer = 60;
            }
            break;
        case INTRO_SUPPORT_CAMERA:
            demo_camera_eye2.x = demo_camera_eye.x = 140.0f;
            demo_camera_eye2.y = demo_camera_eye.y = 205.0f;
            demo_camera_eye2.z = demo_camera_eye.z = -20.0f;

            demo_camera_way2.x = demo_camera_way.x = 10.0f;
            demo_camera_way2.y = demo_camera_way.y = 247.0f;
            demo_camera_way2.z = demo_camera_way.z = -220.0f;

            demo_idx++;
            this->timer = 1;
            break;
        case INTRO_BODY_SOUND:
            demo_camera_way2.x = 10.0f;
            demo_camera_way2.y = 247.0f;
            demo_camera_way2.z = -220.0f;
            add_calc(&cam_spd.x, 7.0f, 0.3f, 0.7f, 0.05f);
            cam_spd.z = cam_spd.x;
            cam_spd.y = cam_spd.z;
            cam_spd2 = cam_spd;

            this->timer--;
            if (this->timer == 0) {
                demo_idx++;
                this->timer = 40;
            }
            break;
        case INTRO_LOOK_SUPPORT:
            this->timer--;
            if (this->timer == 0) {
                demo_camera_way2.x = 10.0f;
                demo_camera_way2.y = 50.0f;
                demo_camera_way2.z = -220.0f;

                cam_spd2 = cam_spd = dam_d;

                demo_idx++;
                demo_idx++;
                this->timer = 20;
            }
            break;
        case INTRO_CALL_BARI:
            add_calc(&cam_spd.x, 14.0f, 0.3f, 1.0f, 0.25f);

            cam_spd.y = cam_spd.x * 0.7f;
            cam_spd.z = cam_spd.x;

            cam_spd2 = cam_spd;
            cam_spd2.z *= 1.75f;

            this->timer--;
            if (this->timer == 0) {
                demo_idx++;
                this->timer = 7500;
                this->unk_1F2 = 0;
            }
            break;
        case INTRO_ATTACH_BARI:
            for (i = 10; i >= 1; i--) {
                if (bl2_disp_flg[i - 1]) {
                    if (bl2_disp_flg[i - 1] == 1) {
                        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_STICK);
                        light_set(play);
                        if (this->onCeiling == 0) {
                            this->onCeiling = 2; // Not used by body
                        }
                    } else if (bl2_disp_flg[i - 1] == 2) {
                        lightning2(play, this, 6, 140, 50.0f, 30.0f, SPARK_BARI, i, true);
                    }

                    if (bl2_disp_flg[i - 1] <= 2) {
                        bl2_disp_flg[i - 1]++;
                    }
                }
            }
            add_calc_short_angle2(&this->unk_1F2, 0x280, 1, 0x32, 0);
            add_calc(&cam_spd.x, 14.0f, 0.3f, 1.0f, 0.25f);
            cam_spd.z = cam_spd.x;
            cam_spd2 = cam_spd;
            if (this->timer >= 45000) {
                play->envCtx.lightSettingOverride = 1;
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            } else if (this->timer >= 35000) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS);
            }

            this->timer += this->unk_1F2;
            if (this->timer >= 65536) {
                cam_spd.y = cam_spd2.y = 9.8f;
                demo_idx++;

                demo_camera_eye2.x = 10.0f;
                demo_camera_eye2.z = 0.0f;

                demo_camera_way2.x = 10.0f;
                demo_camera_way2.y = 140.0f;
                demo_camera_way2.z = -200.0f;

                if (!GET_EVENTCHKINF(EVENTCHKINF_BEGAN_BARINADE_BATTLE)) {
                    Actor_Name_Disp_Set(play, &play->actorCtx.titleCtx, SEGMENTED_TO_VIRTUAL(gBarinadeTitleCardTex),
                                           160, 180, 128, 40);
                }

                if (fqrand() < 0.1f) {
                    Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
                }

                this->timer = 40;
            } else {
                cam_spd.y = 1.6f;
                demo_camera_eye2.y = 5.0f;
                demo_camera_eye2.x = sin_s(this->timer) * 200.0f;
                demo_camera_eye2.z = (cos_s(this->timer) * 200.0f) + -200.0f;
            }
            break;
        case INTRO_TITLE:
            lightning2(play, this, 3, 140, 50.0f, 30.0f, SPARK_BARI, 10.0f, false);
            this->timer--;
            if (this->timer == 0) {
                demo_idx++;
                this->timer = 45;
            }
            break;
        case INTRO_BRIGHTEN:
            lightning2(play, this, 3, 140, 50.0f, 30.0f, SPARK_BARI, 10.0f, false);
            this->timer--;
            if (this->timer == 0) {
                demo_idx++;
                this->timer = 11;
            }
            break;
        case INTRO_FINISH:
            this->timer--;
            if (this->timer == 0) {
                Gama_play_clear_camera(play, c_no);
                c_no = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_ACTIVE);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                demo_idx++;
                SET_EVENTCHKINF(EVENTCHKINF_BEGAN_BARINADE_BATTLE);
                player->actor.shape.rot.y = player->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
            }
            break;
        case BOSSVA_BATTLE:
            Boss_va_Actor_mode_core_move00_init(this);
            break;
    }

    if (demo_idx >= INTRO_BODY_SOUND) {
        Na_SetMotorSe(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG, 1.0f);
        if ((demo_idx >= INTRO_CALL_BARI) && ((play->gameplayFrames % 4) == 0)) {
            lightning2(play, this, 1, 100, 50.0f, 10.0f, SPARK_BODY, 10.0f, false);
        }
    }

    this->unk_1B0 += 0xCE4;
    this->bodyGlow = (s16)(sin_s(this->unk_1B0) * 50.0f) + 150;
    if ((c_no != SUB_CAM_ID_DONE) && (demo_idx <= INTRO_TITLE)) {
        add_calc(&demo_camera_eye.x, demo_camera_eye2.x, 0.3f, cam_spd.x, 0.075f);
        add_calc(&demo_camera_eye.y, demo_camera_eye2.y, 0.3f, cam_spd.y, 0.075f);
        add_calc(&demo_camera_eye.z, demo_camera_eye2.z, 0.3f, cam_spd.z, 0.075f);
        add_calc(&demo_camera_way.x, demo_camera_way2.x, 0.3f, cam_spd2.x, 0.075f);
        add_calc(&demo_camera_way.y, demo_camera_way2.y, 0.3f, cam_spd2.y, 0.075f);
        add_calc(&demo_camera_way.z, demo_camera_way2.z, 0.3f, cam_spd2.z, 0.075f);
        Gama_play_camera_setting(play, c_no, &demo_camera_way, &demo_camera_eye);
    }
}

void Boss_va_Actor_mode_core_move00_init(BossVa* this) {
    f32 lastFrame = Si2_anime_end_frame(&gBarinadeBodyAnim);

    Skeleton_Info2_init(&this->skelAnime, &gBarinadeBodyAnim, 1.0f, lastFrame, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->actor.shape.yOffset = -450.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->timer = 25;
    arm_b = 0x80;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_core_move00);
}

void Boss_va_Actor_mode_core_move00(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk_1B0 += 0xCE4;
    this->bodyGlow = (s16)(sin_s(this->unk_1B0) * 50.0f) + 150;
    if (this->timer != 0) {
        this->timer--;
        if (this->timer == 0) {
            arm_b &= (u8)~0x80;
        }
    }

    if (this->colliderBody.base.atFlags & AT_HIT) {
        this->colliderBody.base.atFlags &= ~AT_HIT;
        if (this->colliderBody.base.at == &player->actor) {
            Actor_player_power_damage_set(play, &this->actor, 8.0f, this->actor.yawTowardsPlayer, 8.0f);
        }
    }

    if (arm_b & 0x7F) {
        this->skelAnime.curFrame = 0.0f;
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 12);
        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_DAMAGE);
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime) && (bl2_flg >= PHASE_2)) {
        Boss_va_Actor_mode_core_move01_init(this, play);
    }

    add_calc_short_angle2(&this->actor.shape.rot.x, this->actor.world.rot.x, 1, 0xC8, 0);
    add_calc_short_angle2(&this->actor.shape.rot.z, this->actor.world.rot.z, 1, 0xC8, 0);
    this->unk_1AC += 0xC31;
    this->unk_1A0 = (cos_s(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (sin_s(this->unk_1AC) * 0.05f) + 1.0f;
    if ((play->gameplayFrames % 4) == 0) {
        lightning2(play, this, 1, 100, 50.0f, 10.0f, SPARK_BARI, 10.0f, false);
    }

    if (fqrand() < 0.1f) {
        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colliderBody);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderBody.base);
    CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderBody.base);
    Na_SetMotorSe(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG, 1.0f);
}

void Boss_va_Actor_mode_core_move01_init(BossVa* this, PlayState* play) {
    s32 i;

    bl2_flg++;
    for (i = BOSSVA_BARI_UPPER_5; i >= BOSSVA_BARI_UPPER_1; i--) {
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_VA,
                           parts_pos[i].x + this->actor.world.pos.x,
                           parts_pos[i].y + this->actor.world.pos.y,
                           parts_pos[i].z + this->actor.world.pos.z, parts_ang[i].x + this->actor.world.rot.x,
                           parts_ang[i].y + this->actor.world.rot.y, parts_ang[i].z + this->actor.world.rot.z, i);
    }

    this->invincibilityTimer = 0;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_core_move01);
}

void Boss_va_Actor_mode_core_move01(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.colorFilterTimer == 0) {
        m_timer++;
        if ((this->invincibilityTimer != 0) && (this->actor.colorFilterParams & 0x4000)) {
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 160);
            this->actor.colorFilterTimer = this->invincibilityTimer;
        } else {
            this->colliderBody.elem.acDmgInfo.dmgFlags = DMG_BOOMERANG;
        }
    }

    if (this->colliderBody.base.acFlags & AC_HIT) {
        this->colliderBody.base.acFlags &= ~AC_HIT;

        if (this->colliderBody.base.ac->id == ACTOR_EN_BOOM) {
            m_timer &= 0xFE00;
            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 160);
            this->colliderBody.elem.acDmgInfo.dmgFlags = DMG_SWORD | DMG_BOOMERANG | DMG_DEKU_STICK;
        } else {
            bl2_d++;
            if ((this->actor.colorFilterTimer != 0) && !(this->actor.colorFilterParams & 0x4000)) {
                this->invincibilityTimer = this->actor.colorFilterTimer - 5;
                //! @bug This condition is always false as this->invincibilityTimer is an s8 so can never
                //! be larger than 160.
                if (this->invincibilityTimer > 160) {
                    this->invincibilityTimer = 0;
                }
            }

            Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 12);
        }

        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_FAINT);
    }

    if (this->colliderBody.base.atFlags & AT_HIT) {
        this->colliderBody.base.atFlags &= ~AT_HIT;

        m_timer = (m_timer + 0x18) & 0xFFF0;
        if (this->colliderBody.base.at == &player->actor) {
            Actor_player_power_damage_set(play, &this->actor, 8.0f, this->actor.yawTowardsPlayer, 8.0f);
            Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }

    if ((m_timer > 10) && !(m_timer & 7) && (this->actor.speed == 1.0f)) {
        Vec3f sp48 = this->actor.world.pos;

        sp48.y += 310.0f + (this->actor.shape.yOffset * this->actor.scale.y);
        sp48.x += -10.0f;
        sp48.z += 220.0f;
        Effect_lightning_ct(play, boss_eff, this, &sp48, 4, 0);
    }

    if (fqrand() < 0.1f) {
        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    add_calc_short_angle2(&this->actor.shape.rot.x, this->actor.world.rot.x, 1, 0xC8, 0);
    add_calc_short_angle2(&this->actor.shape.rot.z, this->actor.world.rot.z, 1, 0xC8, 0);
    add_calc(&this->actor.shape.yOffset, -1000.0f, 1.0f, 20.0f, 0.0f);
    if (!(m_timer & 0x100)) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.speed = 1.0f;
    } else {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.speed = 0.0f;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime) && (bl2_flg >= PHASE_3)) {
        Boss_va_Actor_mode_core_move02_init(this);
    }

    this->unk_1AC += 0xC31;
    this->unk_1A0 = (cos_s(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (sin_s(this->unk_1AC) * 0.05f) + 1.0f;
    if ((play->gameplayFrames % 4) == 0) {
        lightning2(play, this, 1, 100, 50.0f, 10.0f, SPARK_BODY, 10.0f, false);
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 45.0f;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colliderBody);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderBody.base);
    if (this->actor.colorFilterTimer == 0) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderBody.base);
    }

    if ((this->actor.colorFilterTimer == 0) || !(this->actor.colorFilterParams & 0x4000)) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderBody.base);
    }

    Na_SetMotorSe(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG,
                  (this->vaBodySpinRate * 0.00025f) + 1.0f);
}

void Boss_va_Actor_mode_core_move02_init(BossVa* this) {
    this->colliderBody.elem.acDmgInfo.dmgFlags = DMG_BOOMERANG;
    this->actor.speed = 0.0f;
    spd_flg = false;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_core_move02);
}

void Boss_va_Actor_mode_core_move02(BossVa* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 i;
    s16 sp62;

    sp62 = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);
    this->unk_1B0 += 0xCE4;
    this->bodyGlow = (s16)(sin_s(this->unk_1B0) * 50.0f) + 150;
    if (this->colliderBody.base.atFlags & AT_HIT) {
        this->colliderBody.base.atFlags &= ~AT_HIT;
        if (this->colliderBody.base.at == &player->actor) {
            Actor_player_power_damage_set(play, &this->actor, 8.0f, this->actor.yawTowardsPlayer, 8.0f);
            this->actor.world.rot.y += (s16)rnd_fx(0x2EE0) + 0x8000;
            Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }

    if (this->colliderBody.base.acFlags & AC_HIT) {
        this->skelAnime.curFrame = 0.0f;
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 12);
        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_FAINT);
        arm_b = 1;
        this->timer = 131;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        arm_b = 0;
        if (this->timer == 0) {
            if (add_calc_short_angle2(&this->vaBodySpinRate, 0xFA0, 1, 0x12C, 0) == 0) {
                if (this->actor.speed == 0.0f) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                }
                add_calc(&this->actor.speed, 3.0f, 1.0f, 0.15f, 0.0f);
            }
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        } else {
            this->timer--;
            if (this->timer < 35) {
                arm_b = 0x80;
            }
            add_calc_short_angle2(&this->vaBodySpinRate, 0, 1, 0x12C, 0);
            add_calc(&this->actor.speed, 0.0f, 1.0f, 0.2f, 0.0f);
            add_calc(&this->actor.shape.yOffset, -1420.0f, 1.0f, 30.0f, 0.0f);
        }
    }

    if (search_position_distanceXZ(&this->actor.world.pos, &this->actor.home.pos) >= 400.0f) {
        add_calc_short_angle2(&this->actor.world.rot.y, sp62, 1, 0x3E8, 0);
    } else if (player->invincibilityTimer != 0) {
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x8000, 1, 0x12C, 0);
    } else if ((play->gameplayFrames & 0x80) == 0) {
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x12C, 0);
    } else {
        add_calc_short_angle2(&this->actor.world.rot.y, sp62, 1, 0x258, 0);
    }

    if (spd_flg) {
        this->actor.speed = 0.0f;
    }

    Actor_position_moveF(&this->actor);
    if (Skeleton_Info2_anime_play(&this->skelAnime) && (bl2_flg >= PHASE_4)) {
        Boss_va_Actor_mode_core_move03_init(this, play);
    }

    this->actor.shape.rot.y += this->vaBodySpinRate;
    if (bl2_flg == PHASE_3) {
        add_calc(&this->actor.shape.yOffset, -450.0f, 1.0f, 15.0f, 0.0f);
    } else {
        add_calc(&this->actor.shape.yOffset, -810.0f, 1.0f, 15.0f, 0.0f);
    }

    if ((this->actor.shape.yOffset >= -500.0f) && (bl2_flg == PHASE_3)) {
        for (i = BOSSVA_BARI_LOWER_5; i >= BOSSVA_BARI_LOWER_1; i--) {
            Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_BOSS_VA,
                               parts_pos[i].x + this->actor.world.pos.x,
                               parts_pos[i].y + this->actor.world.pos.y,
                               parts_pos[i].z + this->actor.world.pos.z, parts_ang[i].x + this->actor.world.rot.x,
                               parts_ang[i].y + this->actor.world.rot.y, parts_ang[i].z + this->actor.world.rot.z, i);
        }
        bl2_flg++;
    }

    this->unk_1AC += 0xC31;
    this->unk_1A0 = (cos_s(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (sin_s(this->unk_1AC) * 0.05f) + 1.0f;
    if ((play->gameplayFrames % 4) == 0) {
        lightning2(play, this, 1, 0x64, 50.0f, 10.0f, SPARK_BODY, 10.0f, false);
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 20.0f;
    if (fqrand() < 0.1f) {
        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colliderBody);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderBody.base);
    CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderBody.base);
    if (this->timer == 0) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderBody.base);
    }

    Na_SetMotorSe(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG,
                  (this->vaBodySpinRate * 0.00025f) + 1.0f);
}

void Boss_va_Actor_mode_core_move03_init(BossVa* this, PlayState* play) {
    this->unk_1AC = 0;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->vaBodySpinRate = this->unk_1AC;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->timer2 = (s16)(fqrand() * 150.0f) + 300;
    arm_b = 1;
    hit_cnt = 4;
    if (this->actor.shape.yOffset != 0.0f) {
        this->timer = -30;
    }

    this->colliderBody.dim.radius = 55;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_core_move03);
}

void Boss_va_Actor_mode_core_move03(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 tmpf1;
    EnBoom* boomerang;

    this->unk_1B0 = (this->unk_1B0 + (s16)((bl2_flg - PHASE_4 + 1) * 1000.0f)) + 0xCE4;
    this->bodyGlow = (s16)(sin_s(this->unk_1B0) * 50.0f) + 150;
    if (this->colliderBody.base.atFlags & AT_HIT) {
        this->colliderBody.base.atFlags &= ~AT_HIT;
        if (this->colliderBody.base.at == &player->actor) {
            Actor_player_power_damage_set(play, &this->actor, 8.0f, this->actor.yawTowardsPlayer, 8.0f);
            this->actor.world.rot.y += (s16)rnd_fx(0x2EE0) + 0x8000;
            Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
        }
    }
    if (fqrand() < 0.1f) {
        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    if (this->colliderBody.base.acFlags & AC_HIT) {
        this->colliderBody.base.acFlags &= ~AC_HIT;
        this->skelAnime.curFrame = 0.0f;
        if (this->timer >= 0) {
            if (this->invincibilityTimer == 0) {
                this->invincibilityTimer = 8;
                if (this->actor.colChkInfo.damageEffect != 1) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                    Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_DAMAGE);
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 12);
                    hit_cnt -= this->actor.colChkInfo.damage;
                    if (hit_cnt <= 0) {
                        this->timer = 0;
                        bl2_flg++;
                        hit_cnt += 3;
                        if (bl2_flg >= PHASE_DEATH) {
                            Boss_va_Actor_mode_core_move04_init(this, play);
                            Actor_info_finish(play, &this->actor);
                            return;
                        }
                        this->actor.speed = -10.0f;
                        this->timer = -170 - (s16)(fqrand() * 150.0f);
                    }
                } else {
                    this->timer = (s16)rnd_fx(40.0f) + 160;
                    this->vaBodySpinRate = 0;
                    this->actor.speed = 0.0f;
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 125, COLORFILTER_BUFFLAG_OPA, 255);
                    Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_FAINT);
                }
            }
        } else if (this->colliderBody.base.ac->id == ACTOR_EN_BOOM) {
            boomerang = (EnBoom*)this->colliderBody.base.ac;
            boomerang->returnTimer = 0;
            boomerang->moveTo = &player->actor;
            boomerang->actor.world.rot.y = boomerang->actor.yawTowardsPlayer;
            Actor_SE_set(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
        }
    } else if ((this->timer2 == 0) && (this->actor.shape.yOffset == 0.0f)) {
        this->timer = -220 - (s16)(fqrand() * 200.0f);
    } else if (this->timer2 != 0) {
        this->timer2--;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer == 0) {
        add_calc(&this->actor.shape.yOffset, 0.0f, 1.0f, ((bl2_flg - PHASE_4 + 1) * 5.0f) + 10.0f, 0.0f);
        if (add_calc_short_angle2(&this->vaBodySpinRate, (s16)((bl2_flg - PHASE_4 + 1) * 500.0f) + 0xFA0, 1, 0x12C,
                               0) == 0) {
            if (this->actor.speed == 0.0f) {
                this->actor.colorFilterTimer = 0;
                this->actor.world.rot.y = this->actor.yawTowardsPlayer;
                this->timer2 = (s16)(fqrand() * 150.0f) + 300;
            }
            add_calc(&this->actor.speed, ((bl2_flg - PHASE_4 + 1) * 1.5f) + 4.0f, 1.0f, 0.25f, 0.0f);
        }
        this->colliderBody.elem.acDmgInfo.dmgFlags = DMG_BOOMERANG;
    } else {
        add_calc_short_angle2(&this->vaBodySpinRate, 0, 1, 0x96, 0);
        if (this->timer > 0) {
            if ((player->stateFlags1 & PLAYER_STATE1_26) && (this->timer > 35)) {
                this->timer = 35;
            }
            add_calc(&this->actor.shape.yOffset, -480.0f, 1.0f, 30.0f, 0.0f);
            this->colliderBody.elem.acDmgInfo.dmgFlags = DMG_SWORD | DMG_BOOMERANG | DMG_DEKU_STICK;
            this->timer--;
        } else {
            if ((player->stateFlags1 & PLAYER_STATE1_26) && (this->timer < -60)) {
                this->timer = -59;
            }
            if ((play->gameplayFrames % 4) == 0) {
                lightning2(play, this, 2, 0x64, 220.0f, 5.0f, SPARK_BODY, 12.0f, true);
            }
            if (this->timer < -30) {
                if (this->actor.speed > 0.0f) {
                    add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
                }
                add_calc(&this->actor.shape.yOffset, -1400.0f, 1.0f, 60.0f, 0.0f);
            } else {
                if (this->actor.speed == 0.0f) {
                    this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
                    this->timer2 = (s16)(fqrand() * 150.0f) + 330;
                }
                add_calc_short_angle2(&this->vaBodySpinRate, 0xFA0, 1, 0x1F4, 0);
                tmpf1 = bl2_flg - PHASE_4 + 1;
                add_calc(&this->actor.speed, (tmpf1 + tmpf1) + 4.0f, 1.0f, 0.25f, 0.0f);
                add_calc(&this->actor.shape.yOffset, 0.0f, 1.0f, 20.0f, 0.0f);
            }
            this->timer++;
        }
    }

    this->actor.shape.rot.y += this->vaBodySpinRate;
    if (this->actor.speed < 0.0f) {
        add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
    }

    this->unk_1AC += 0xC31;
    this->unk_1A0 = (cos_s(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (sin_s(this->unk_1AC) * 0.05f) + 1.0f;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
        this->actor.world.rot.y = (s16)rnd_fx(30 * (0x10000 / 360)) + this->actor.wallYaw;
    }

    if (bl2_flg <= PHASE_4) {
        if (search_position_distanceXZ(&this->actor.world.pos, &this->actor.home.pos) >= 400.0f) {
            add_calc_short_angle2(&this->actor.world.rot.y, search_position_angleY(&this->actor.world.pos, &this->actor.home.pos),
                               1, 0x5DC, 0);
        } else if (player->invincibilityTimer != 0) {
            add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x8000, 1, 0x12C, 0);
        } else if ((play->gameplayFrames & 0x80) == 0) {
            add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1,
                               (s16)((bl2_flg - PHASE_4 + 1) * 100.0f) + 0x64, 0);
        }
    }

    Actor_position_moveF(&this->actor);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 60.0f;
    if (((play->gameplayFrames % 2) == 0) && (this->timer == 0)) {
        lightning2(play, this, 2, 125, 40.0f, 10.0f, SPARK_BODY, 10.0f, false);
        lightning2(play, this, 1, 100, 15.0f, 10.0f, SPARK_BARI, 11.0f, true);
    }

    Actor_BGcheck2(play, &this->actor, 30.0f, 70.0f, 0.0f, UPDBGCHECKINFO_FLAG_0);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colliderBody);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderBody.base);
    if (this->invincibilityTimer == 0) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderBody.base);
    }
    if ((this->vaBodySpinRate > 0x3E8) || (this->actor.shape.yOffset < -1200.0f)) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderBody.base);
    }
    Na_SetMotorSe(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG,
                  (this->vaBodySpinRate * 0.00025f) + 1.0f);
    if (this->invincibilityTimer != 0) {
        this->invincibilityTimer--;
        arm_b = (arm_b & 0x80) | 2;
    } else {
        arm_b = (arm_b & 0x80) | 1;
    }
}

void Boss_va_Actor_mode_core_move04_init(BossVa* this, PlayState* play) {
    Na_SetMotorSe(&this->actor.projectedPos, NA_SE_EN_BALINADE_LEVEL - SFX_FLAG, 1.0f);
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
    this->vaCamRotMod = 0xC31;
    demo_idx = DEATH_START;
    this->actor.speed = 0.0f;
    this->unk_1A8 = 0.0f;
    Actor_Environment_room_clear_On(play, play->roomCtx.curRoom.num);
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_core_move04);
}

void Boss_va_Actor_mode_core_move04(BossVa* this, PlayState* play) {
    s32 i;
    Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
    s32 sp7C;
    Player* player = GET_PLAYER(play);
    s16 tmp16;

    switch (demo_idx) {
        case DEATH_START:
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
            Demo_play_start(play, &play->csCtx);
            c_no = Gama_play_make_camera(play);
            Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
            Gama_play_set_camera_status(play, c_no, CAM_STAT_ACTIVE);

            demo_camera_way2.x = this->actor.world.pos.x;
            demo_camera_way2.y = this->actor.world.pos.y;
            demo_camera_way2.z = this->actor.world.pos.z;

            demo_camera_way = mainCam->at;

            demo_camera_eye2 = demo_camera_eye = mainCam->eye;

            demo_camera_eye2.y = 40.0f;
            demo_camera_way2.y = 140.0f;

            cam_spd2 = cam_spd = dam_d;

            this->unk_1AC = search_position_angleY(&demo_camera_eye, &demo_camera_way2) - 0x100;
            this->unk_1B0 = 15;
            play->envCtx.screenFillColor[0] = play->envCtx.screenFillColor[1] = play->envCtx.screenFillColor[2] = 0xFF;
            play->envCtx.screenFillColor[3] = 0;
            play->envCtx.fillScreen = true;
            demo_idx++;
            FALLTHROUGH;
        case DEATH_BODY_TUMORS:
            this->unk_1AC += 0x100;
            demo_camera_eye2.x = (sin_s(this->unk_1AC) * (160.0f + this->unk_1A8)) + demo_camera_way2.x;
            demo_camera_eye2.z = (cos_s(this->unk_1AC) * (160.0f + this->unk_1A8)) + demo_camera_way2.z;
            add_calc(&cam_spd.x, 16.0f, 0.4f, 1.5f, 0.5f);
            cam_spd.z = cam_spd.x;
            cam_spd.y = cam_spd.x * 0.5f;
            cam_spd2 = cam_spd;
            tmp16 = rnd_fx(0.5f) + ((cam_spd.x * 0.5f) + 0.6f);
            if (((play->gameplayFrames % 4) == 0) && (this->unk_1B0 != 0)) {
                for (i = 6; i > 1; i--) {
                    s_core(play, this, 1, tmp16, 0.0f, 0.0f, TUMOR_BODY, i, true);
                }

                s_core(play, this, 1, tmp16, 0.0f, 0.0f, TUMOR_BODY, 11.0f, true);
                this->unk_1B0--;
            }

            if (this->unk_1B0 == 0) {
                demo_idx++;

                cam_spd2 = cam_spd = dam_d;
            }
            break;
        case DEATH_CORE_DEAD:
            this->unk_1AC += 0x1862;
            this->unk_1A0 = (cos_s(this->unk_1AC) * 0.12f) + 1.0f;
            this->unk_1A4 = (sin_s(this->unk_1AC) * 0.075f) + 1.0f;
            if (!this->isDead) {
                this->burst++;
                this->isDead++;
                this->timer = 30;
                demo_idx++;
                Effect_sound_ct(play, &this->actor.projectedPos, NA_SE_EN_BALINADE_DEAD, 1, 1, 0x28);
                this->onCeiling = 2; // Not used by body
                light_set2(play);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
            }
            break;
        case DEATH_CORE_BURST:
            if (this->timer == 13) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS_CLEAR);
            }

            this->timer--;
            if (this->timer == 0) {
                demo_camera_way2.x = this->actor.world.pos.x;
                demo_camera_way2.y = this->actor.world.pos.y + 30.0f;
                demo_camera_way2.z = this->actor.world.pos.z;

                demo_camera_eye2.x = (sin_s(player->actor.shape.rot.y) * -130.0f) + player->actor.world.pos.x;
                demo_camera_eye2.z = (cos_s(player->actor.shape.rot.y) * -130.0f) + player->actor.world.pos.z;
                demo_camera_eye2.y = player->actor.world.pos.y + 55.0f;

                cam_spd2 = cam_spd = dam_d;

                demo_idx++;
                this->timer = 133;
            }
            break;
        case DEATH_MUSIC:
            add_calc(&cam_spd.x, 1.5f, 0.3f, 0.05f, 0.015f);
            cam_spd.z = cam_spd.x;
            cam_spd.y = cam_spd.z;
            cam_spd2 = cam_spd;

            this->timer--;
            if (this->timer == 0) {
                Gama_play_clear_camera(play, c_no);
                c_no = SUB_CAM_ID_DONE;
                Demo_play_end(play, &play->csCtx);
                Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_ACTIVE);

                mainCam->eyeNext = mainCam->eye = demo_camera_eye;

                mainCam->at = demo_camera_way;

                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
                demo_idx++;

                Actor_info_make_actor(&play->actorCtx, play, ACTOR_ITEM_B_HEART, this->actor.world.pos.x, this->actor.world.pos.y,
                            this->actor.world.pos.z, 0, 0, 0, 0);

                for (i = 2, sp7C = 2; i > 0; i--) {
                    if (search_position_distance(&warp_pos[i], &player->actor.world.pos) <
                        search_position_distance(&warp_pos[i - 1], &player->actor.world.pos)) {
                        sp7C = i - 1;
                    }
                }

                Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_RU1, warp_pos[sp7C].x, warp_pos[sp7C].y, warp_pos[sp7C].z,
                            0, 0, 0, 0);
            }
            FALLTHROUGH;
        case DEATH_FINISH:
            rnd_fx(0.5f);
            play->envCtx.fillScreen = false;
            break;
    }

    if (c_no != SUB_CAM_ID_DONE) {
        add_calc(&demo_camera_eye.x, demo_camera_eye2.x, 0.3f, cam_spd.x, 0.15f);
        add_calc(&demo_camera_eye.y, demo_camera_eye2.y, 0.3f, cam_spd.y, 0.15f);
        add_calc(&demo_camera_eye.z, demo_camera_eye2.z, 0.3f, cam_spd.z, 0.15f);
        add_calc(&demo_camera_way.x, demo_camera_way2.x, 0.3f, cam_spd2.x, 0.15f);
        add_calc(&demo_camera_way.y, demo_camera_way2.y, 0.3f, cam_spd2.y, 0.15f);
        add_calc(&demo_camera_way.z, demo_camera_way2.z, 0.3f, cam_spd2.z, 0.15f);
        Gama_play_camera_setting(play, c_no, &demo_camera_way, &demo_camera_eye);
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc(&this->actor.shape.yOffset, -480.0f, 1.0f, 30.0f, 0.0f);
    add_calc_short_angle2(&this->vaBodySpinRate, 0, 1, 0xC8, 0);
    add_calc_short_angle2(&this->vaCamRotMod, 0, 1, 0xC8, 0);
    add_calc_short_angle2(&this->bodyGlow, 200, 1, 10, 0);
    if (play->envCtx.screenFillColor[3] != 0) {
        play->envCtx.screenFillColor[3] -= 50;
    }

    add_calc(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
    this->actor.shape.rot.y += this->vaBodySpinRate;
    this->unk_1AC += this->vaCamRotMod;

    this->unk_1A0 = (cos_s(this->unk_1AC) * 0.1f) + 1.0f;
    this->unk_1A4 = (sin_s(this->unk_1AC) * 0.05f) + 1.0f;
}

void Boss_va_Actor_mode_arm_demo00_init(BossVa* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gBarinadeSupportAttachedAnim, 0.0f, 0.0f,
                     Si2_anime_end_frame(&gBarinadeSupportAttachedAnim), ANIMMODE_LOOP_INTERP, 0.0f);
    this->timer = 0;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_arm_demo00);
}

void Boss_va_Actor_mode_arm_demo00(BossVa* this, PlayState* play) {
    pos_set(this);
    if (demo_idx == BOSSVA_BATTLE) {
        Boss_va_Actor_mode_arm_move00_init(this, play);
    } else if (demo_idx >= INTRO_REVERSE_CAMERA) {
        this->timer++;
        if ((this->timer % 2) == 0) {
            lightning2(play, this, 2, 90, 5.0f, 0.0f, SPARK_BODY, ((this->timer & 0x20) >> 5) + 1, true);
        }

        Skeleton_Info2_anime_play(&this->skelAnime);
        add_calc(&this->skelAnime.playSpeed, 1.0f, 1.0f, 0.05f, 0.0f);
        if (fqrand() < 0.1f) {
            Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
        }
    }
}

void Boss_va_Actor_mode_arm_move00_init(BossVa* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gBarinadeSupportAttachedAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gBarinadeSupportAttachedAnim), ANIMMODE_LOOP, 0.0f);
    this->timer = this->actor.params * 10;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_arm_move00);
}

void Boss_va_Actor_mode_arm_move00(BossVa* this, PlayState* play) {
    this->timer++;
    if (arm_b & 0x7F) {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 12);
        if (fqrand() > 0.5f) {
            Skeleton_Info2_init(&this->skelAnime, &gBarinadeSupportDamage1Anim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gBarinadeSupportDamage1Anim), ANIMMODE_ONCE, 0.0f);
        } else {
            Skeleton_Info2_init(&this->skelAnime, &gBarinadeSupportDamage2Anim, 1.0f, 0.0f,
                             Si2_anime_end_frame(&gBarinadeSupportDamage2Anim), ANIMMODE_ONCE, 0.0f);
        }
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        Skeleton_Info2_init(&this->skelAnime, &gBarinadeSupportAttachedAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gBarinadeSupportAttachedAnim), ANIMMODE_LOOP, 0.0f);
    }

    pos_set(this);
    if (fqrand() < 0.1f) {
        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    if (this->colliderSph.base.acFlags & AC_HIT) {
        Boss_va_Actor_mode_arm_move01_init(this, play);
    } else {
        if (this->actor.colorFilterTimer == 0) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderSph.base);
        }

        if ((this->timer % 2) == 0) {
            lightning2(play, this, 1, 100, 5.0f, 0.0f, SPARK_BODY, ((this->timer & 0x20) >> 5) + 1, true);
        }
    }
}

void Boss_va_Actor_mode_arm_move01_init(BossVa* this, PlayState* play) {
    s32 stumpParams = this->actor.params + BOSSVA_STUMP_1;

    arm_b++;
    bl2_flg++;
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_BOSS_VA, this->armTip.x, this->armTip.y + 20.0f, this->armTip.z, 0,
                this->actor.shape.rot.y, 0, stumpParams);
    setDamageCamera(&play->mainCamera, 2, 11, 8);
    this->burst = false;
    this->timer2 = 0;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_arm_move01);
}

void Boss_va_Actor_mode_arm_move01(BossVa* this, PlayState* play) {
    BossVa* vaBody = GET_BODY(this);
    f32 lastFrame;

    pos_set(this);

    if (this->onCeiling) {
        lastFrame = Si2_anime_end_frame(&gBarinadeSupportCutAnim);
        this->onCeiling = false;
        this->timer = (s32)(fqrand() * 10.0f) + 5;
        Skeleton_Info_dt(&this->skelAnime, play);
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBarinadeCutSupportSkel, &gBarinadeSupportCutAnim, NULL, NULL, 0);
        Skeleton_Info2_init(&this->skelAnime, &gBarinadeSupportCutAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
        arm_b = 0;
        GET_BODY(this)->actor.shape.yOffset -= 60.0f;

        switch (this->actor.params) {
            case BOSSVA_SUPPORT_1:
                GET_BODY(this)->actor.world.rot.x += 0x4B0;
                break;
            case BOSSVA_SUPPORT_2:
                GET_BODY(this)->actor.world.rot.x -= 0x258;
                GET_BODY(this)->actor.world.rot.z -= 0x4E2;
                break;
            case BOSSVA_SUPPORT_3:
                GET_BODY(this)->actor.world.rot.x -= 0x258;
                GET_BODY(this)->actor.world.rot.z += 0x4E2;
                break;
        }
    }

    add_calc_short_angle2(&this->headRot.x, vaBody->vaBodySpinRate * -3, 1, 0x4B0, 0);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        lastFrame = Si2_anime_end_frame(&gBarinadeSupportDetachedAnim);
        Skeleton_Info2_init(&this->skelAnime, &gBarinadeSupportDetachedAnim, 1.0f, 0.0f, lastFrame, ANIMMODE_LOOP_INTERP,
                         0.0f);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }

    if ((this->timer == 0) && (demo_idx < DEATH_START)) {
        this->timer = (s32)(fqrand() * 10.0f) + 10;
        setGreenBlood(play, &this->armTip, this->headRot.x, this->actor.shape.rot.y);
    }

    if (demo_idx >= DEATH_START) {
        add_calc(&this->skelAnime.playSpeed, 0.0f, 0.3f, 0.25f, 0.125f);
    }

    switch (demo_idx) {
        case DEATH_SHELL_BURST:
            demo_camera_eye = demo_camera_eye2;
            demo_camera_way = demo_camera_way2;
            add_calc(&demo_camera_eye.x, demo_camera_way2.x, 1.0f, 10.0f, 0.0f);
            add_calc(&demo_camera_eye.z, demo_camera_way2.z, 1.0f, 10.0f, 0.0f);
            demo_camera_eye.y += 20.0f;
            demo_idx++;
            FALLTHROUGH;
        case DEATH_CORE_TUMORS:
        case DEATH_CORE_DEAD:
        case DEATH_CORE_BURST:
            if (!this->burst) {
                if ((play->gameplayFrames % 2) != 0) {
                    s_core(play, this, 1, (s16)rnd_fx(5.0f) + 6, 7.0f, 5.0f, TUMOR_ARM,
                                 (this->timer2 >> 3) + 1, true);
                }

                this->timer2++;
                if (this->timer2 >= 32) {
                    this->burst++;
                    this->isDead = true;
                    Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BREAK2);
                    if (this->actor.params == BOSSVA_SUPPORT_3) {
                        demo_idx++;
                    }
                }
            } else {
                this->timer2--;
                if (this->timer2 == 0) {
                    Actor_delete(&this->actor);
                }
            }
            break;
    }

    this->timer--;
}

void Boss_va_Actor_mode_arm_move02_init(BossVa* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gBarinadeStumpAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gBarinadeStumpAnim),
                     ANIMMODE_ONCE, 0.0f);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_arm_move02);
}

void Boss_va_Actor_mode_arm_move02(BossVa* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime) && (fqrand() < 0.3f)) {
        this->skelAnime.curFrame -= fqrand() * 3.0f;
    }

    if (demo_idx >= DEATH_START) {
        Actor_delete(&this->actor);
    }
}

void Boss_va_Actor_mode_earm_demo00_init(BossVa* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(&gBarinadeZapperIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gBarinadeZapperIdleAnim, 1.0f, lastFrame - 1.0f, lastFrame,
                     ANIMMODE_LOOP_INTERP, -6.0f);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_earm_demo00);
}

void Boss_va_Actor_mode_earm_demo00(BossVa* this, PlayState* play) {
    pos_set(this);

    switch (demo_idx) {
        case INTRO_TITLE:
        case INTRO_BRIGHTEN:
        case INTRO_FINISH:
            Skeleton_Info2_anime_play(&this->skelAnime);
            break;
        case BOSSVA_BATTLE:
            Boss_va_Actor_mode_earm_move00_init(this, play);
            break;
    }

    add_calc_short_angle2(&this->unk_1F2, this->actor.shape.rot.y - this->actor.shape.rot.x, 1, 0x2EE, 0);
    add_calc_short_angle2(&this->unk_1F0, this->skelAnime.jointTable[7].z, 1, 0x2EE, 0);
}

void Boss_va_Actor_mode_earm_move00_init(BossVa* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(&gBarinadeZapperIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gBarinadeZapperIdleAnim, 1.0f, lastFrame - 1.0f, lastFrame,
                     ANIMMODE_LOOP_INTERP, -6.0f);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_earm_move00);
}

void Boss_va_Actor_mode_earm_move00(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnBoom* boomerang;
    Actor* boomTarget;
    s16 yaw;
    s16 sp98;
    s16 sp96;
    s16 sp94;
    s16 tmp17;
    s16 sp90 = 0x1F4;
    s16 sp8E;
    u32 sp88;
    Vec3f sp7C;
    s32 pad3;

    boomerang = BoomCheck(play);

    if ((boomerang == NULL) || (boomerang->moveTo == NULL) || (boomerang->moveTo == &player->actor)) {
        sp7C = player->actor.world.pos;
        sp7C.y += 10.0f;
        sp8E = 0x3E80;
    } else {
        f32 sp74;
        s32 i;
        s16 sp6E;
        s16 sp6C;
        f32 sp68;
        f32 sp64;
        f32 sp60;
        f32 sp5C;
        s16 sp5A;
        s16 sp58;
        s16 sp56;
        s16 sp54;
        f32 sp50;

        sp74 = R_UPDATE_RATE * 0.5f;
        sp8E = 0x4650;

        boomTarget = boomerang->moveTo;
        sp7C = boomerang->actor.world.pos;
        sp6C = boomerang->actor.world.rot.y;
        sp56 = boomerang->actor.world.rot.x;

        for (i = boomerang->returnTimer; i >= 3; i--) {
            sp6E = search_position_angleY(&sp7C, &boomTarget->focus.pos);
            sp5A = sp6C - sp6E;
            sp58 = search_position_angleX(&sp7C, &boomTarget->focus.pos);
            sp54 = sp56 - sp58;

            sp50 = (200.0f - search_position_distance(&sp7C, &boomTarget->focus.pos)) * 0.005f;
            if (sp50 < 0.12f) {
                sp50 = 0.12f;
            }

            if (sp5A < 0) {
                sp5A = -sp5A;
            }

            if (sp54 < 0) {
                sp54 = -sp54;
            }

            chase_angle(&sp6C, sp6E, sp5A * sp50);
            chase_angle(&sp56, sp58, sp54 * sp50);

            sp68 = -sin_s(sp56) * 12.0f;
            sp5C = cos_s(sp56) * 12.0f;
            sp64 = sin_s(sp6C) * sp5C;
            sp60 = cos_s(sp6C);
            sp7C.x += sp64 * sp74;
            sp7C.y += sp68 * sp74;
            sp7C.z += sp60 * sp5C * sp74;
        }
        sp90 = 0x3E80;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    pos_set(this);
    if (bl2_flg >= PHASE_4) {
        Boss_va_Actor_mode_earm_move03_init(this, play);
        return;
    }

    if (arm_b & 0x7F) {
        Boss_va_Actor_mode_earm_move01_init(this, play);
        return;
    }

    if ((bl2_flg < PHASE_4) && (GET_BODY(this)->actor.speed != 0.0f)) {
        Boss_va_Actor_mode_earm_move04_init(this, play);
        return;
    }

    sp98 = search_position_angleY(&sp7C, &this->armTip);
    tmp17 = sp98 - this->actor.shape.rot.y;

    if ((sp8E >= ABS(tmp17) || this->burst) && !(arm_b & 0x80) && !(player->stateFlags1 & PLAYER_STATE1_26)) {

        if (!this->burst) {
            sp94 = sp98 - this->actor.shape.rot.y;

            if (ABS(sp94) > 0x1770) {
                sp94 = (sp94 > 0) ? 0x1770 : -0x1770;
            }

            tmp17 = add_calc_short_angle2(&this->unk_1E6, sp94, 1, 0x6D6, 0);
            sp88 = ABS(tmp17);

            sp94 = sp98 - sp94;

            if (ABS(sp94) > 0x1770) {
                sp94 = (sp94 > 0) ? 0x1770 : -0x1770;
            }

            tmp17 = add_calc_short_angle2(&this->unk_1EC, sp94, 1, 0x6D6, 0);
            sp88 += ABS(tmp17);

            yaw = search_position_angleY(&this->zapHeadPos, &sp7C);
            tmp17 = add_calc_short_angle2(&this->unk_1F2, yaw - 0x4000, 1, 0x9C4, 0);
            sp88 += ABS(tmp17);

            sp96 = this->actor.shape.rot.x + this->skelAnime.jointTable[1].z + this->skelAnime.jointTable[2].z +
                   this->skelAnime.jointTable[3].z + this->skelAnime.jointTable[4].z + this->skelAnime.jointTable[5].z;

            yaw = search_position_angleX(&sp7C, &this->zapNeckPos);
            tmp17 = add_calc_short_angle2(&this->unk_1EA, yaw - sp96, 1, 0xFA0, 0);
            sp88 += ABS(tmp17);

            yaw = search_position_angleX(&this->zapHeadPos, &sp7C);
            tmp17 = add_calc_short_angle2(&this->unk_1F0, -yaw, 1, 0xFA0, 0);
            sp88 += ABS(tmp17);

            this->skelAnime.playSpeed = 0.0f;
            if (add_calc(&this->skelAnime.curFrame, 0.0f, 1.0f, 2.0f, 0.0f) == 0.0f) {
                if (sp88 < (u32)sp90) {
                    this->timer2 = 0;
                    this->burst++;
                    this->unk_1D8 = sp7C;
                }

                if (fqrand() < 0.1f) {
                    Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
                }
            }
        }
    } else {
        if (this->burst || (this->timer2 < 0)) {
            if (this->colliderLightning.base.atFlags & AT_HIT) {
                if (this->timer2 > 0) {
                    Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_HIT_RINK);
                    light_set(play);
                    this->timer2 = -1;
                    GET_BODY(this)->onCeiling = 6; // not used by body
                }
            } else if (this->timer2 > 0) {
                this->timer2 = 0;
            }

            if ((this->timer2 < 0) && (player->stateFlags1 & PLAYER_STATE1_26)) {
                lightning2(play, this, 1, 30, 0.0f, 0.0f, SPARK_LINK, 0.0f, true);
            }
        }

        add_calc_short_angle2(&this->unk_1E6, 0, 1, 0x6D6, 0);
        add_calc_short_angle2(&this->unk_1EC, 0, 1, 0x6D6, 0);
        add_calc_short_angle2(&this->unk_1EA, 0, 1, 0x6D6, 0);
        add_calc_short_angle2(&this->unk_1F2, this->actor.shape.rot.y - this->actor.shape.rot.x, 1, 0x6D6, 0);
        add_calc_short_angle2(&this->unk_1F0, this->skelAnime.jointTable[7].z, 1, 0x6D6, 0);
        add_calc(&this->skelAnime.playSpeed, 1.0f, 1.0f, 0.05f, 0.0f);
        this->burst = false;
    }

    if (this->burst && (this->burst != 2)) { // burst can never be 2
        if (this->timer2 >= 32) {
            if (this->timer2 == 32) {
                Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_THUNDER);
            }
            lightning2(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 5.0f, true);
            lightning2(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 6.0f, true);
            lightning2(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 7.0f, true);
            CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderLightning.base);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderLightning.base);
        } else {
            lightning2(play, this, 2, 50, 15.0f, 0.0f, SPARK_BODY, (this->timer2 >> 3) + 1, true);
            if (this->timer2 == 30) {
                light_set(play);
            }
            if (this->timer2 == 20) {
                Vec3f sp44 = this->zapHeadPos;

                Effect_l_cv_ct(play, boss_eff, this, &sp44, &this->headRot, 100, 0);
            }
        }

        this->timer2++;
        if (this->timer2 >= 40) {
            this->burst = false;
        }
    }
}

void Boss_va_Actor_mode_earm_move01_init(BossVa* this, PlayState* play) {
    if (fqrand() > 0.5f) {
        Skeleton_Info2_init(&this->skelAnime, &gBarinadeZapperDamage1Anim, 0.5f, 0.0f,
                         Si2_anime_end_frame(&gBarinadeZapperDamage1Anim), ANIMMODE_ONCE_INTERP, 4.0f);
    } else {
        Skeleton_Info2_init(&this->skelAnime, &gBarinadeZapperDamage2Anim, 0.5f, 0.0f,
                         Si2_anime_end_frame(&gBarinadeZapperDamage2Anim), ANIMMODE_ONCE_INTERP, 4.0f);
    }

    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 12);
    this->burst = false;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_earm_move01);
}

void Boss_va_Actor_mode_earm_move01(BossVa* this, PlayState* play) {
    pos_set(this);
    add_calc_short_angle2(&this->unk_1E6, 0, 1, 0xFA0, 0);
    add_calc_short_angle2(&this->unk_1E4, 0, 1, 0xFA0, 0);
    add_calc_short_angle2(&this->unk_1EC, 0, 1, 0xFA0, 0);
    add_calc_short_angle2(&this->unk_1EA, 0, 1, 0xFA0, 0);
    add_calc_short_angle2(&this->unk_1F2, this->actor.shape.rot.y - this->actor.shape.rot.x, 1, 0x2EE, 0);
    add_calc_short_angle2(&this->unk_1F0, this->skelAnime.jointTable[7].z, 1, 0x2EE, 0);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (bl2_flg >= PHASE_4) {
            Boss_va_Actor_mode_earm_move03_init(this, play);
        } else {
            Boss_va_Actor_mode_earm_move00_init(this, play);
        }
    }
}

void Boss_va_Actor_mode_earm_move02_init(BossVa* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(&gBarinadeZapperIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gBarinadeZapperIdleAnim, fqrand() + 0.25f, fqrand() * 3.0f,
                     lastFrame, ANIMMODE_LOOP_INTERP, -6.0f);
    this->burst = false;
    this->timer2 = (this->actor.params * -6) + 18;
    this->unk_1B0 = 0;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_earm_move02);
}

void Boss_va_Actor_mode_earm_move02(BossVa* this, PlayState* play) {
    f32 sp3C = 55.0f;
    f32 tmpf1;
    f32 tmpf2;

    pos_set(this);
    if (((play->gameplayFrames % 32) == 0) && (demo_idx <= DEATH_BODY_TUMORS)) {
        this->unk_1E8 = rnd_fx(0x4000);
        this->unk_1EE = rnd_fx(0x4000);
        this->unk_1F4 = (s16)rnd_fx(0x4000) + this->actor.shape.rot.y - this->actor.shape.rot.x;
    } else {
        add_calc(&this->skelAnime.playSpeed, 0.0f, 1.0f, 0.025f, 0.0f);
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc_short_angle2(&this->unk_1E6, this->unk_1E8, 1, (s16)rnd_fx(500.0f) + 0x1F4, 0);
    add_calc_short_angle2(&this->unk_1E4, 0, 1, 0x1F4, 0);
    add_calc_short_angle2(&this->unk_1EC, this->unk_1EE, 1, (s16)rnd_fx(500.0f) + 0x1F4, 0);
    add_calc_short_angle2(&this->unk_1EA, 0, 1, 0x1F4, 0);
    add_calc_short_angle2(&this->unk_1F2, this->unk_1F4, 1, (s16)rnd_fx(500.0f) + 0x1F4, 0);

    switch (demo_idx) {
        case DEATH_ZAPPER_2:
            sp3C = -55.0f;
            FALLTHROUGH;
        case DEATH_ZAPPER_1:
        case DEATH_ZAPPER_3:
            if (!this->burst) {
                if (((this->actor.params == BOSSVA_ZAPPER_1) && (this->timer2 < 16)) ||
                    ((this->actor.params == BOSSVA_ZAPPER_2) && (this->timer2 < 24)) ||
                    (this->actor.params == BOSSVA_ZAPPER_3)) {

                    if ((this->timer2 % 2) == 0 && (this->timer2 >= 0)) {
                        if (this->timer2 < 8) {
                            s_core(play, this, 1, (s16)rnd_fx(5.0f) + 0xD, 0.0f, 0.0f, TUMOR_ARM,
                                         0.6f, true);
                        } else {
                            s_core(play, this, 1, (s16)rnd_fx(5.0f) + 6, 0.0f, 7.0f, TUMOR_ARM,
                                         (this->timer2 >> 3) + 1, true);
                        }

                        lightning2(play, this, 2, 50, 15.0f, 0.0f, SPARK_BODY, (this->timer2 >> 3) + 1, true);
                    }

                    this->timer2++;
                    if (this->timer2 >= 32) {
                        this->burst++;
                        this->isDead = true;
                        light_set2(play);
                        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BREAK2);
                    }
                } else {
                    this->burst++;
                    this->isDead = true;
                    this->timer2 = 32;
                    demo_idx++;
                }

                if ((this->actor.params - BOSSVA_ZAPPER_1 + DEATH_ZAPPER_1) == demo_idx) {
                    demo_camera_way.x = this->zapNeckPos.x;
                    demo_camera_eye.y = demo_camera_way.y = this->zapNeckPos.y;
                    demo_camera_way.z = this->zapNeckPos.z;
                    demo_camera_eye.x = (cos_s(-(this->actor.shape.rot.y + this->unk_1B0)) * sp3C) + this->zapNeckPos.x;
                    demo_camera_eye.z = (sin_s(-(this->actor.shape.rot.y + this->unk_1B0)) * sp3C) + this->zapNeckPos.z;
                    this->unk_1B0 += 0x15E;
                }
            } else {
                this->timer2--;
                if (this->timer2 == 0) {
                    if (this->actor.params == BOSSVA_ZAPPER_3) {
                        demo_idx++;
                    }
                    Actor_delete(&this->actor);
                }
            }
            break;
    }
}

void Boss_va_Actor_mode_earm_move03_init(BossVa* this, PlayState* play) {
    f32 lastFrame = Si2_anime_end_frame(&gBarinadeZapperIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gBarinadeZapperIdleAnim, 1.0f, lastFrame - 1.0f, lastFrame,
                     ANIMMODE_LOOP_INTERP, -6.0f);
    this->burst = false;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_earm_move03);
}

void Boss_va_Actor_mode_earm_move03(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad;
    s16 tmp16;
    s16 sp6C;
    s16 sp6A;
    s16 sp68;
    s16 yaw;
    u32 sp60;
    Vec3f sp54 = player->actor.world.pos;

    sp54.y += 10.0f;
    Skeleton_Info2_anime_play(&this->skelAnime);
    pos_set(this);
    if (bl2_flg >= PHASE_DEATH) {
        Boss_va_Actor_mode_earm_move02_init(this, play);
        return;
    }

    if (arm_b & 0x7E) {
        Boss_va_Actor_mode_earm_move01_init(this, play);
        return;
    }

    sp54.y += 25.0;

    sp6C = search_position_angleY(&sp54, &this->armTip);
    tmp16 = sp6C - this->actor.shape.rot.y;

    if ((ABS(tmp16) <= 0x4650 || this->burst) && !(arm_b & 0x80) && !(player->stateFlags1 & PLAYER_STATE1_26)) {
        if (!this->burst) {

            sp68 = sp6C - this->actor.shape.rot.y;
            if (ABS(sp68) > 0x1770) {
                sp68 = (sp68 > 0) ? 0x1770 : -0x1770;
            }

            tmp16 = add_calc_short_angle2(&this->unk_1E6, sp68, 1, 0xDAC, 0);
            sp60 = ABS(tmp16);

            sp68 = sp6C - sp68;
            if (ABS(sp68) > 0x1770) {
                sp68 = sp68 > 0 ? 0x1770 : -0x1770;
            }

            tmp16 = add_calc_short_angle2(&this->unk_1EC, sp68, 1, 0xDAC, 0);
            sp60 += ABS(tmp16);

            yaw = search_position_angleY(&this->zapHeadPos, &sp54);
            tmp16 = add_calc_short_angle2(&this->unk_1F2, yaw - 0x4000, 1, 0xEA6, 0);
            sp60 += ABS(tmp16);

            sp6A = this->actor.shape.rot.x + this->skelAnime.jointTable[1].x + this->skelAnime.jointTable[2].x +
                   this->skelAnime.jointTable[3].x + this->skelAnime.jointTable[4].x + this->skelAnime.jointTable[5].x;

            yaw = search_position_angleX(&sp54, &this->zapNeckPos);
            tmp16 = add_calc_short_angle2(&this->unk_1EA, yaw - sp6A, 1, 0x1B58, 0);
            sp60 += ABS(tmp16);

            yaw = search_position_angleX(&this->zapHeadPos, &sp54);
            tmp16 = add_calc_short_angle2(&this->unk_1F0, -yaw, 1, 0x1B58, 0);
            sp60 += ABS(tmp16);

            this->skelAnime.playSpeed = 0.0f;
            if ((add_calc(&this->skelAnime.curFrame, 0.0f, 1.0f, 3.0f, 0.0f) == 0.0f) && (sp60 < 0x258)) {
                this->timer2 = 0;
                this->burst++;
                this->unk_1D8 = sp54;
                if (fqrand() < 0.1f) {
                    Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
                }
            }
        }
    } else {
        if (this->burst || (this->timer2 < 0)) {
            if (this->colliderLightning.base.atFlags & AT_HIT) {
                if (this->timer2 > 0) {
                    Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_HIT_RINK);
                    light_set(play);
                    this->timer2 = -1;
                    GET_BODY(this)->onCeiling = 6; // not used by body
                }
            } else if (this->timer2 > 0) {
                this->timer2 = 0;
            }

            if ((this->timer2 < 0) && (player->stateFlags1 & PLAYER_STATE1_26)) {
                lightning2(play, this, 1, 30, 0.0f, 0, SPARK_LINK, 0.0f, true);
            }
        }

        add_calc_short_angle2(&this->unk_1E6, 0, 1, 0xEA6, 0);
        add_calc_short_angle2(&this->unk_1EC, 0, 1, 0xEA6, 0);
        add_calc_short_angle2(&this->unk_1EA, 0, 1, 0xEA6, 0);
        add_calc_short_angle2(&this->unk_1F2, this->actor.shape.rot.y - this->actor.shape.rot.x, 1, 0xEA6, 0);
        add_calc_short_angle2(&this->unk_1F0, this->skelAnime.jointTable[7].z, 1, 0xEA6, 0);
        add_calc(&this->skelAnime.playSpeed, 1.0f, 1.0f, 0.05f, 0.0f);
        this->burst = false;
    }

    if (this->burst && (this->burst != 2)) { // burst can never be 2
        if (this->timer2 >= 16) {
            if (this->timer2 == 18) {
                Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_THUNDER);
            }

            lightning2(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 5.0f, true);
            lightning2(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 6.0f, true);
            lightning2(play, this, 2, 110, 15.0f, 15.0f, SPARK_BLAST, 7.0f, true);
            CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderLightning.base);
            CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderLightning.base);
        } else {
            lightning2(play, this, 2, 50, 15.0f, 0.0f, SPARK_BODY, (this->timer2 >> 1) + 1, true);
            if (this->timer2 == 14) {
                light_set(play);
            }
            if (this->timer2 == 4) {
                Vec3f sp48 = this->zapHeadPos;

                Effect_l_cv_ct(play, boss_eff, this, &sp48, &this->headRot, 100, 0);
            }
        }

        this->timer2++;
        if (this->timer2 >= 24) {
            this->burst = false;
        }
    }
}

void Boss_va_Actor_mode_earm_move04_init(BossVa* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gBarinadeZapperDamage2Anim, 0.0f, 0.0f,
                     Si2_anime_end_frame(&gBarinadeZapperDamage2Anim), ANIMMODE_ONCE_INTERP, -6.0f);
    this->burst = false;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_earm_move04);
}

void Boss_va_Actor_mode_earm_move04(BossVa* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    pos_set(this);
    add_calc_short_angle2(&this->unk_1E6, 0, 1, 0x1770, 0);
    add_calc_short_angle2(&this->unk_1E4, 0, 1, 0x1770, 0);
    add_calc_short_angle2(&this->unk_1EC, 0, 1, 0x1770, 0);
    add_calc_short_angle2(&this->unk_1EA, 0, 1, 0x1770, 0);
    add_calc_short_angle2(&this->unk_1F2, this->actor.shape.rot.y - 0x4000, 1, 0x2710, 0);
    add_calc_short_angle2(&this->unk_1F0, this->skelAnime.jointTable[7].z - 0x1388, 1, 0x1770, 0);
    if (GET_BODY(this)->actor.speed == 0.0f) {
        Boss_va_Actor_mode_earm_move00_init(this, play);
    }
}

void Boss_va_Actor_mode_bl2_demo00_init(BossVa* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gBarinadeBariAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gBarinadeBariAnim),
                     ANIMMODE_LOOP, 0.0f);
    this->unk_1A0 = 60.0f;
    this->unk_1A4 = fqrand() * 360.0f;
    this->timer2 = 64;
    this->unk_1F0 = 120;
    this->unk_1A8 = 0.0f;
    this->actor.world.pos.x = parts_pos[this->actor.params + 10].x + this->actor.home.pos.x;
    this->actor.world.pos.y = parts_pos[this->actor.params + 10].y + this->actor.home.pos.y;
    this->actor.world.pos.z = parts_pos[this->actor.params + 10].z + this->actor.home.pos.z;
    this->timer = 45;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_bl2_demo00);
}

void Boss_va_Actor_mode_bl2_demo00(BossVa* this, PlayState* play) {
    Vec3f sp54 = this->actor.home.pos;
    f32 sp50 = 40.0f;
    s16 sp4E;
    s16 tmp;

    if (this->actor.home.pos.y >= 0.0f) {
        sp54.y += 25.0f;
    }

    this->unk_1A4 += fqrand() * 0.25f;

    switch (demo_idx) {
        case INTRO_LOOK_BARI:
            if (this->actor.params == BOSSVA_BARI_UPPER_1) {
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
                if (add_calc(&this->actor.world.pos.y, 60.0f, 0.3f, 1.0f, 0.15f) == 0.0f) {
                    this->timer--;
                    if (this->timer == 0) {
                        demo_idx++;
                    }
                }
            }
            this->actor.shape.rot.x = 0;
            break;
        case INTRO_REVERSE_CAMERA:
        case INTRO_SUPPORT_CAMERA:
        case INTRO_BODY_SOUND:
        case INTRO_LOOK_SUPPORT:
            if (this->actor.params != BOSSVA_BARI_UPPER_1) {
                add_calc(&this->actor.world.pos.y,
                                   parts_pos[this->actor.params + 10].y + this->actor.home.pos.y, 0.3f, 1.0f,
                                   0.15f);
                this->actor.world.pos.x += (sinf_table(this->unk_1A4 * 0.25f) * 0.5f);
            } else {
                add_calc(&this->actor.world.pos.y, 60.0f, 0.3f, 1.0f, 0.15f);
            }
            this->actor.world.pos.y += sinf_table(this->unk_1A4) * (2.0f - sinf_table(this->unk_1A4));
            break;
        case INTRO_CALL_BARI:
        case INTRO_ATTACH_BARI:
            if ((this->timer2 > 15) && (this->timer < 0)) {
                add_calc(&this->actor.world.pos.x, sp54.x, 1.0f, 6.5f, 0.0f);
                add_calc(&this->actor.world.pos.y, sp54.y, 1.0f, 6.5f, 0.0f);
                add_calc(&this->actor.world.pos.z, sp54.z, 1.0f, 6.5f, 0.0f);

                sp50 = search_position_distance(&sp54, &this->actor.world.pos);
                if (sp50 <= 60.0f) {
                    tmp = add_calc_short_angle2(&this->actor.shape.rot.x, this->actor.home.rot.x, 1, 0x7D0, 0);
                    sp4E = ABS(tmp);

                    tmp = add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.home.rot.y, 1, 0x7D0, 0);
                    sp4E += ABS(tmp);

                    if ((sp50 == 0.0f) && (sp4E == 0)) {
                        if (!this->isDead) {
                            if (this->actor.params >= BOSSVA_BARI_LOWER_1) {
                                if (this->actor.params == BOSSVA_BARI_LOWER_1) {
                                    bl2_disp_flg[0]++;
                                } else {
                                    bl2_disp_flg[this->actor.params - BOSSVA_BARI_UPPER_1]++;
                                }
                            } else {
                                bl2_disp_flg[this->actor.params - BOSSVA_BARI_UPPER_1 + 1]++;
                            }
                            this->timer = -30;
                            this->isDead++;
                        } else {
                            this->timer++;
                            if (this->timer == 0) {
                                Actor_delete(&this->actor);
                            }
                        }
                        return;
                    }
                }
            }
            FALLTHROUGH;
        case INTRO_UNUSED_CALL_BARI:
            this->timer--;
            if (this->timer == 0) {
                this->timer2 = 0;
            } else {
                search_position_angleXY(&GET_BODY(this)->actor.world.pos, &this->actor.world.pos, &this->actor.world.rot, false);
                this->unk_1A0 = search_position_distance(&GET_BODY(this)->actor.world.pos, &this->actor.world.pos);
                if (sp50 > 30.0f) {
                    lightning2(play, this, 1, 80, 15.0f, 0.0f, SPARK_BARI, 1.0f, true);
                }
            }
            break;
        case BOSSVA_BATTLE:
            this->timer++;
            if (this->timer == 0) {
                Actor_delete(&this->actor);
            }
            return;
        case INTRO_TITLE:
        case INTRO_BRIGHTEN:
        case INTRO_FINISH:
            break;
    }

    if (((play->gameplayFrames % 4) == 0) && (demo_idx < INTRO_ATTACH_BARI)) {
        lightning2(play, this, 1, 70, 25.0f, 20.0f, SPARK_BARI, 2.0f, true);
    }

    if (fqrand() < 0.1f) {
        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }
}

void Boss_va_Actor_mode_bl2_move_init(BossVa* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gBarinadeBariAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gBarinadeBariAnim),
                     ANIMMODE_LOOP, 0.0f);
    this->timer2 = 0x80;
    this->unk_1F0 = 0x78;
    this->unk_1A0 = 60.0f;
    this->unk_1A8 = 0.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_bl2_move);
}

void Boss_va_Actor_mode_bl2_move(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnBoom* boomerang;
    Vec3f sp54 = GET_BODY(this)->unk_1D8;
    s16 sp52;
    s32 pad;

    this->unk_1A4 += fqrand() * 0.5f;
    sp52 = this->timer2 & 0x1FF;

    if ((play->gameplayFrames % 128) == 0) {
        this->vaBariUnused.x = (s16)(fqrand() * 100.0f) + 100;
    }

    add_calc_short_angle2(&this->vaBariUnused.z, this->vaBariUnused.x, 1, 0x1E, 0);
    this->vaBariUnused.y += this->vaBariUnused.z;
    if ((this->colliderLightning.base.atFlags & AT_HIT) || (this->colliderSph.base.atFlags & AT_HIT)) {
        if ((this->colliderLightning.base.at == &player->actor) || (this->colliderSph.base.at == &player->actor)) {
            Actor_player_power_damage_set(play, &this->actor, 8.0f, GET_BODY(this)->actor.yawTowardsPlayer,
                                                  8.0f);
            Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
            this->colliderSph.base.at = NULL;
            this->colliderLightning.base.at = NULL;
        }

        this->colliderLightning.base.atFlags &= ~AT_HIT;
        this->colliderSph.base.atFlags &= ~AT_HIT;
    }

    if (this->colliderSph.base.acFlags & AC_HIT) {
        this->colliderSph.base.acFlags &= ~AC_HIT;
        if ((this->colliderSph.base.ac->id == ACTOR_EN_BOOM) && (sp52 >= 128)) {
            boomerang = (EnBoom*)this->colliderSph.base.ac;
            boomerang->returnTimer = 0;
            boomerang->moveTo = &player->actor;
            boomerang->actor.world.rot.y = boomerang->actor.yawTowardsPlayer;
            Actor_SE_set(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
        }
    }

    this->actor.world.pos.x = (sin_s(this->actor.world.rot.y) * this->unk_1A0) + sp54.x;
    this->actor.world.pos.z = (cos_s(this->actor.world.rot.y) * this->unk_1A0) + sp54.z;
    add_calc(&this->actor.world.pos.y, 4.0f, 1.0f, 2.0f, 0.0f);
    this->actor.world.pos.y += 2.0f * sinf_table(this->unk_1A4);
    this->actor.world.rot.x = search_position_angleX(&sp54, &this->actor.world.pos);
    add_calc(&this->unk_1A0, 160.0f, 1.0f, 2.0f, 0.0f);
    add_calc_short_angle2(&this->actor.shape.rot.x, 0, 1, 0x5DC, 0);
    if (!(this->timer2 & 0x200)) {
        this->unk_1AC = 0xBB8;
    } else {
        this->unk_1AC = -0xBB8;
    }

    if (sp52 >= 128) {
        lightning2(play, this, 1, 75, 15.0f, 7.0f, SPARK_TETHER, 1.0f, true);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderSph.base);
        spd_flg = false;
    } else {
        spd_flg = true;
    }

    CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderLightning.base);
    CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderSph.base);
    if ((play->gameplayFrames % 4) == 0) {
        add_calc_short_angle2(&this->unk_1F0, 0x78, 1, 0xA, 0);
    }

    if (fqrand() < 0.1f) {
        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    this->actor.world.rot.y += this->unk_1AC;
    if (arm_b & 0x7F) {
        Boss_va_Actor_mode_bl2_move2_init(this, play);
    }
}

void Boss_va_Actor_mode_bl2_move0_init(BossVa* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gBarinadeBariAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gBarinadeBariAnim),
                     ANIMMODE_LOOP, 0.0f);
    this->timer2 = 0x40;
    this->unk_1F0 = 0x78;
    this->unk_1A0 = 60.0f;
    this->unk_1A8 = 0.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_bl2_move0);
}

void Boss_va_Actor_mode_bl2_move0(BossVa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnBoom* boomerang;
    Vec3f sp54 = GET_BODY(this)->unk_1D8;
    s16 sp52;
    s16 sp50;
    f32 sp4C;
    s32 pad;

    this->unk_1A4 += fqrand() * 0.5f;
    sp52 = this->timer2 & 0x1FF;
    if ((play->gameplayFrames % 128) == 0) {
        this->vaBariUnused.x = (s16)(fqrand() * 100.0f) + 100;
    }

    sp50 = (bl2_flg * 70) - 280;
    add_calc_short_angle2(&this->vaBariUnused.z, this->vaBariUnused.x, 1, 0x1E, 0);
    this->vaBariUnused.y += this->vaBariUnused.z;
    if (bl2_d != 0) {
        bl2_d--;
        bl2_d_set(this, play);
        return;
    }

    if ((this->colliderLightning.base.atFlags & AT_HIT) || (this->colliderSph.base.atFlags & AT_HIT)) {
        if ((this->colliderLightning.base.at == &player->actor) || (this->colliderSph.base.at == &player->actor)) {
            Actor_player_power_damage_set(play, &this->actor, 8.0f, GET_BODY(this)->actor.yawTowardsPlayer,
                                                  8.0f);
            Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
            this->colliderSph.base.at = NULL;
            this->colliderLightning.base.at = NULL;
        }

        this->colliderLightning.base.atFlags &= ~AT_HIT;
        this->colliderSph.base.atFlags &= ~AT_HIT;
    }

    add_calc(&this->actor.world.pos.y, 4.0f, 1.0f, 2.0f, 0.0f);
    this->actor.world.rot.x = search_position_angleX(&sp54, &this->actor.world.pos);
    if ((play->gameplayFrames % 8) == 0) {
        add_calc_short_angle2(&this->unk_1F0, 0x28, 1, 0xA, 0);
        lightning2(play, this, 1, this->unk_1F0, 25.0f, 20.0f, 2, 2.0f, true);
    }

    if (!(m_timer & 0x100) && (GET_BODY(this)->actor.colorFilterTimer == 0)) {
        sp4C = 200.0f;
        lightning2(play, this, 1, 125, 15.0f, 7.0f, SPARK_TETHER, 1.0f, true);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        if (PARAMS_GET_U(this->actor.params, 0, 1)) {
            sp4C = -200.0f;
        }

        add_calc(&this->unk_1A0, (sin_s(m_timer * 0x190) * sp4C) + 320.0f, 1.0f, 10.0f, 0.0f);
        add_calc_short_angle2(&this->unk_1AC, sp50 + 0x1F4, 1, 0x3C, 0);
        this->actor.world.pos.y += 2.0f * sinf_table(this->unk_1A4);
        if (this->colliderSph.base.acFlags & AC_HIT) {
            this->colliderSph.base.acFlags &= ~AC_HIT;

            if ((this->colliderSph.base.ac->id == ACTOR_EN_BOOM) && (sp52 >= 64)) {
                boomerang = (EnBoom*)this->colliderSph.base.ac;
                boomerang->returnTimer = 0;
                boomerang->moveTo = &player->actor;
                boomerang->actor.world.rot.y = boomerang->actor.yawTowardsPlayer;
                Actor_SE_set(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
            }
        }

        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderLightning.base);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderSph.base);
    } else {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        add_calc_short_angle2(&this->unk_1AC, sp50 + 150, 1, 0x3C, 0);
        if (GET_BODY(this)->actor.colorFilterTimer == 0) {
            add_calc(&this->unk_1A0, 180.0f, 1.0f, 1.5f, 0.0f);
        } else {
            this->unk_1AC = 0;
            if (this->actor.colorFilterTimer == 0) {
                Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_XLU,
                                     GET_BODY(this)->actor.colorFilterTimer);
            }
        }

        this->actor.world.pos.y += sinf_table(this->unk_1A4) * 4.0f;
        if (this->colliderSph.base.acFlags & AC_HIT) {
            bl2_d_set(this, play);
        }
    }

    add_calc_short_angle2(&this->actor.shape.rot.x, 0, 1, 0x5DC, 0);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderSph.base);
    if ((play->gameplayFrames % 4) == 0) {
        add_calc_short_angle2(&this->unk_1F0, 0x78, 1, 0xA, 0);
    }

    if (fqrand() < 0.1f) {
        Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_SPARK - SFX_FLAG);
    }

    if (GET_BODY(this)->actor.colorFilterTimer == 0) {
        if (!(this->timer2 & 0x400)) {
            this->actor.world.rot.y += this->unk_1AC;
        } else {
            this->actor.world.rot.y -= this->unk_1AC;
        }

        this->actor.world.pos.x = (sin_s(this->actor.world.rot.y) * this->unk_1A0) + sp54.x;
        this->actor.world.pos.z = (cos_s(this->actor.world.rot.y) * this->unk_1A0) + sp54.z;
    }
}

void Boss_va_Actor_mode_bl2_move2_init(BossVa* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->timer = GET_BODY(this)->timer;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_XLU, this->timer);
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_bl2_move2);
}

void Boss_va_Actor_mode_bl2_move2(BossVa* this, PlayState* play) {
    s32 sp44_pad;
    Vec3f sp40 = GET_BODY(this)->unk_1D8;

    this->actor.world.rot.x = search_position_angleX(&GET_BODY(this)->actor.world.pos, &this->actor.world.pos);
    if (this->colliderSph.base.acFlags & AC_HIT) {
        bl2_d_set(this, play);
        return;
    }

    this->unk_1A4 += fqrand() * 0.5f;
    add_calc(&this->actor.world.pos.y, 4.0f, 1.0f, 2.0f, 0.0f);
    this->actor.world.pos.y += sinf_table(this->unk_1A4) * 3.0f;
    CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderSph.base);
    if ((play->gameplayFrames % 4) == 0) {
        add_calc_short_angle2(&this->unk_1F0, 0x28, 1, 0xA, 0);
        lightning2(play, this, 1, this->unk_1F0, 25.0f, 20.0f, SPARK_BARI, 2.0f, true);
    }

    this->timer--;
    this->actor.world.rot.x = search_position_angleX(&sp40, &this->actor.world.pos);
    if (this->timer <= 0) {
        if (this->timer == 0) {
            this->timer2 = 0;
        } else {
            lightning2(play, this, 1, 85, 15.0f, 0.0f, SPARK_TETHER, 1.0f, true);
            if (this->timer2 >= 0x10) {
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->timer2 = 0x80;
                Boss_Va_actor_set_process(this, Boss_va_Actor_mode_bl2_move);
            }
        }
    }
}

void Boss_va_Actor_mode_bl2_move3_init(BossVa* this) {
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->timer = 30;
    Actor_SE_set(&this->actor, NA_SE_EN_BALINADE_BL_DEAD);
    this->isDead++;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_bl2_move3);
}

void Boss_va_Actor_mode_bl2_move3(BossVa* this, PlayState* play) {
    this->timer--;
    if (this->timer == 0) {
        Actor_delete(&this->actor);
    }
}

void Boss_va_Actor_mode_door_move00_init(BossVa* this, PlayState* play) {
    if (demo_idx >= INTRO_SPAWN_BARI) {
        d_para = 100;
    }
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Boss_Va_actor_set_process(this, Boss_va_Actor_mode_door_move00);
}

void Boss_va_Actor_mode_door_move00(BossVa* this, PlayState* play) {
    if (d_para == 29) {
        Actor_SE_set(&this->actor, NA_SE_EV_BUYODOOR_CLOSE);
    }

    if (demo_idx <= INTRO_DOOR_SHUT) {
        if (d_para < 100) {
            d_para += 8;
        } else {
            d_para = 100;
        }
    }
}

void Boss_va_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BossVa* this = (BossVa*)thisx;
    EnBoom* boomerang;
    s32 i;

    this->actionFunc(this, play);

    switch (this->actor.params) {
        case BOSSVA_BODY:
            if (this->colliderBody.base.acFlags & AC_HIT) {
                this->colliderBody.base.acFlags &= ~AC_HIT;
                if (this->colliderBody.base.ac->id == ACTOR_EN_BOOM) {
                    boomerang = (EnBoom*)this->colliderBody.base.ac;
                    boomerang->returnTimer = 0;
                }
            }

            Boss_Eff_move(play);

            for (i = 2; i >= 0; i--) {
                play->envCtx.adjAmbientColor[i] = MAX(play->envCtx.adjAmbientColor[i] - 1, 0);
                play->envCtx.adjLight1Color[i] = MAX(play->envCtx.adjLight1Color[i] - 10, 0);
                play->envCtx.adjFogColor[i] = MAX(play->envCtx.adjFogColor[i] - 10, 0);
            }

            if (this->onCeiling > 0) {
                this->onCeiling--; // not used by body
            }
            break;

        default:
            this->timer2++;
            this->actor.focus.pos = this->actor.world.pos;
            this->actor.focus.pos.y += 45.0f;
            this->unk_1D8.y = (cos_s(this->timer2 * 0xFA4) * 0.24f) + 0.76f;
            this->unk_1D8.x = (sin_s(this->timer2 * 0xFA4) * 0.2f) + 1.0f;
            break;

        case BOSSVA_SUPPORT_1:
        case BOSSVA_SUPPORT_2:
        case BOSSVA_SUPPORT_3:
        case BOSSVA_ZAPPER_1:
        case BOSSVA_ZAPPER_2:
        case BOSSVA_ZAPPER_3:
        case BOSSVA_DOOR:
            break;
    }
}

s32 boss_va_core_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_va.c", 4156);

    if (limbIndex == 20) {
        gDPPipeSync(POLY_OPA_DISP++);
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 8, 16, 1, 0,
                                    (play->gameplayFrames * -2) % 64, 16, 16));
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, this->bodyGlow);
        Matrix_rotateX(-M_PI / 2, MTXMODE_APPLY);
    } else if ((limbIndex >= 10) && (limbIndex < 20)) {
        rot->x -= 0x4000;
        *dList = NULL;
    } else if (limbIndex == 6) {
        Matrix_scale(this->unk_1A4, this->unk_1A4, this->unk_1A4, MTXMODE_APPLY);
    } else if (limbIndex == 61) {
        Matrix_scale(this->unk_1A0, this->unk_1A0, this->unk_1A0, MTXMODE_APPLY);
    } else if (limbIndex == 7) {
        rot->x -= 0xCCC;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_va.c", 4183);
    return false;
}

void boss_va_core_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;
    Vec3f sp78 = { 0.0f, 0.0f, 0.0f };
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_va.c", 4192);

    if (limbIndex == 6) {
        if (bl2_flg < PHASE_3) {
            sp78.x = -1000.0f;
        } else {
            sp78.x = 200.0f;
        }
        Matrix_Position(&sp78, &this->unk_1D8);
    } else if ((limbIndex >= 10) && (limbIndex < 20) && (bl2_disp_flg[limbIndex - 10] != 0)) {
        if (((limbIndex >= 16) || (limbIndex == 10)) && (bl2_flg <= PHASE_3)) {
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_va.c", 4208);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_008BB8);
        } else if ((limbIndex >= 11) && (bl2_flg <= PHASE_2)) {
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_va.c", 4212);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_008BB8);
        }

        if (demo_idx >= DEATH_START) {
            sp78.x = rnd_fx(530.0f);
            sp78.y = rnd_fx(530.0f);
            sp78.z = -60.0f;
        }
        Matrix_Position(&sp78, &this->effectPos[limbIndex - 10]);
    } else if (limbIndex == 25) {
        gSPSegment(POLY_XLU_DISP++, 0x09,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (play->gameplayFrames * 10) % 128, 16, 32,
                                    1, 0, (play->gameplayFrames * 5) % 128, 16, 32));
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_va.c", 4232);
        gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_008D70);
    } else if ((*dList != NULL) && (limbIndex >= 29) && (limbIndex < 56)) {
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_va.c", 4236);
        gSPDisplayList(POLY_XLU_DISP++, *dList);
    } else if ((limbIndex == 24) && (demo_idx < DEATH_START)) {
        sp78.x = (this->actor.shape.yOffset + 450.0f) + -140.0f;
        Matrix_Position(&sp78, &this->unk_280);
        sp78.x = 200.0f;
        Matrix_Position(&sp78, &this->unk_274);
    }

    if ((limbIndex == 7) && (demo_idx >= DEATH_START)) {
        sp78.x = rnd_fx(320.0f) + -250.0f;
        sp78.y = rnd_fx(320.0f);
        sp78.z = rnd_fx(320.0f);

        if (sp78.y < 0.0f) {
            sp78.y -= 150.0f;
        } else {
            sp78.y += 150.0f;
        }

        if (sp78.z < 0.0f) {
            sp78.z -= 150.0f;
        } else {
            sp78.z += 150.0f;
        }
        Matrix_Position(&sp78, &this->unk_274);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_va.c", 4264);
}

s32 boss_arm_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;

    if (!this->onCeiling && (limbIndex == 4)) {
        rot->z += this->headRot.x;
    }
    return false;
}

void boss_arm_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;
    Vec3f sp20 = { 0.0f, 0.0f, 0.0f };
    s32 pad;

    if (this->onCeiling) {
        switch (limbIndex) {
            case 4:
                Matrix_Position(&dam_d, &this->actor.focus.pos);
                CollisionCheck_Uty_convJntSphL2G(0, &this->colliderSph);
                break;
            case 7:
                Matrix_Position(&dam_d, &this->armTip);
                sp20.x = ((this->timer & 0x1F) >> 1) * -40.0f;
                sp20.y = ((this->timer & 0x1F) >> 1) * -7.0f;
                Matrix_Position(&sp20, &this->effectPos[0]);
                break;
            case 9:
                sp20.x = ((this->timer & 0x1F) >> 1) * -60.0f;
                sp20.y = ((this->timer & 0x1F) >> 1) * -45.0f;
                Matrix_Position(&sp20, &this->effectPos[1]);
                break;
        }
    } else {
        switch (limbIndex) {
            case 5:
                Matrix_Position(&dam_d, &this->armTip);
                break;
            case 8:
                sp20.x = (this->timer2 & 7) * 90.0f;
                Matrix_Position(&sp20, &this->effectPos[2]);
                break;
            case 9:
                sp20.x = (this->timer2 & 7) * 50.0f;
                Matrix_Position(&sp20, &this->effectPos[1]);
                break;
            case 10:
                sp20.x = (this->timer2 & 7) * 46.0f;
                Matrix_Position(&sp20, &this->effectPos[0]);
                break;
        }
    }
}

s32 boss_e_arm_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;
    MtxF zapperMtx;

    switch (limbIndex) {
        case 4:
            rot->y += this->unk_1E6;
            rot->z += this->unk_1E4;
            break;
        case 5:
            rot->y += this->unk_1EC;
            rot->z += this->unk_1EA;
            break;
        case 7:
            Matrix_translate(pos->x, pos->y, pos->z, MTXMODE_APPLY);
            Matrix_get(&zapperMtx);
            Matrix_to_rotate2_new(&zapperMtx, &e_ang_wk5, false);
            Matrix_rotateX(BINANG_TO_RAD(-e_ang_wk5.x), MTXMODE_APPLY);
            Matrix_rotateY(BINANG_TO_RAD(-e_ang_wk5.y), MTXMODE_APPLY);
            Matrix_rotateZ(BINANG_TO_RAD(-e_ang_wk5.z), MTXMODE_APPLY);
            Matrix_rotateY(BINANG_TO_RAD(this->unk_1F2), MTXMODE_APPLY);
            Matrix_rotateZ(BINANG_TO_RAD(this->unk_1F0), MTXMODE_APPLY);
            pos->x = pos->y = pos->z = 0.0f;
            rot->x = rot->y = rot->z = 0;
            break;
    }
    return false;
}

void boss_e_arm_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;
    Vec3f sp70 = { 0.0f, 0.0f, 0.0f };
    Vec3f sp64 = { 15.0f, 0.0f, 0.0f };
    Vec3f sp58 = { -15.0f, 0.0f, 0.0f };
    Vec3f sp4C = { 15.0f, 0.0f, 0.0f };
    Vec3f sp40 = { -15.0f, 0.0f, 0.0f };
    s16 sp3E;
    s16 sp3C;

    switch (limbIndex) {
        case 3:
            sp70.x = (this->timer2 & 7) * 30.0f;
            Matrix_Position(&sp70, &this->effectPos[0]);
            break;

        case 4:
            Matrix_Position(&dam_d, &this->armTip);
            sp70.x = (this->timer2 & 7) * 30.0f;
            Matrix_Position(&sp70, &this->effectPos[1]);
            break;

        case 5:
            Matrix_Position(&dam_d, &this->zapNeckPos);
            sp70.x = (this->timer2 & 7) * 46.0f;
            Matrix_Position(&sp70, &this->effectPos[2]);
            break;

        case 7:
            Matrix_Position(&dam_d, &this->zapHeadPos);
            sp70.x = (this->timer2 & 7) * 46.0f;
            Matrix_Position(&sp70, &this->effectPos[3]);
            sp70.x = 20.0f;
            Matrix_Position(&sp70, &this->effectPos[9]);
            search_position_angleXY(&this->effectPos[9], &this->unk_1D8, &this->headRot, false);
            sp3E = this->headRot.x;
            sp3C = this->headRot.y;
            Matrix_push();
            Matrix_translate(this->effectPos[9].x, this->effectPos[9].y, this->effectPos[9].z, MTXMODE_NEW);
            Matrix_rotateXYZ(sp3E, sp3C, 0, MTXMODE_APPLY);
            sp70.x = 0.0f;
            if (bl2_flg >= PHASE_4) {
                sp70.z = ((this->timer2 - 16) & 7) * 120.0f;
            } else {
                sp70.z = ((this->timer2 - 32) & 0xF) * 80.0f;
            }
            sp4C.z = sp40.z = sp70.z += 40.0f;
            sp70.z += 50.0f;
            Matrix_Position(&sp70, &this->effectPos[4]);
            if (bl2_flg >= PHASE_4) {
                sp70.z -= 33.0f;
                if (sp70.z < 0.0f) {
                    sp70.z = 0.0f;
                }
                Matrix_Position(&sp70, &this->effectPos[6]);
                sp70.z -= 33.0f;
                if (sp70.z < 0.0f) {
                    sp70.z = 0.0f;
                }
            } else {
                sp70.z -= 22.0f;
                if (sp70.z < 0.0f) {
                    sp70.z = 0.0f;
                }
                Matrix_Position(&sp70, &this->effectPos[6]);
                sp70.z -= 22.0f;
                if (sp70.z < 0.0f) {
                    sp70.z = 0.0f;
                }
            }
            Matrix_Position(&sp70, &this->effectPos[5]);
            Matrix_Position(&sp64, &this->colliderLightning.dim.quad[1]);
            Matrix_Position(&sp58, &this->colliderLightning.dim.quad[0]);
            Matrix_Position(&sp4C, &this->colliderLightning.dim.quad[3]);
            Matrix_Position(&sp40, &this->colliderLightning.dim.quad[2]);
            CollisionCheck_Uty_setSword4Pos(&this->colliderLightning, &this->colliderLightning.dim.quad[0],
                                     &this->colliderLightning.dim.quad[1], &this->colliderLightning.dim.quad[2],
                                     &this->colliderLightning.dim.quad[3]);
            Matrix_pull();
            break;
    }
}

s32 boss_bl2_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;

    switch (limbIndex) {
        case 2:
            *dList = NULL;
            break;
        case 3:
            Matrix_scale(this->unk_1D8.x, 1.0f, this->unk_1D8.x, MTXMODE_APPLY);
            break;
        case 4:
            Matrix_scale(1.0f, this->unk_1D8.y, 1.0f, MTXMODE_APPLY);
            break;
    }
    return false;
}

void boss_bl2_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    BossVa* this = (BossVa*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_va.c", 4494);

    if (limbIndex == 2) {
        gSPSegment(POLY_XLU_DISP++, 0x0A,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (play->gameplayFrames * 10) % 32, 16, 32, 1,
                                    0, (play->gameplayFrames * -5) % 32, 16, 32));
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_va.c", 4508);
        gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_000FA0);
    } else if ((limbIndex == 3) || (limbIndex == 4)) {
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_boss_va.c", 4512);
        gSPDisplayList(POLY_XLU_DISP++, *dList);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_va.c", 4517);
}

void Boss_va_display(Actor* thisx, PlayState* play) {
    s16* paramsPtr; // This stack slot is almost certainly actually play2, but can't make it match
    BossVa* this = (BossVa*)thisx;
    Vec3f spBC;
    Vec3f spB0 = { 0.0f, 45.0f, 0.0f };
    Vec3f spA4 = { 0.4f, 0.4f, 0.4f };
    Vec3f sp98 = { 15.0f, 40.0f, 0.0f };
    Vec3f sp8C = { -15.0f, 40.0f, 0.0f };
    Vec3f sp80 = { 15.0f, 40.0f, 0.0f };
    Vec3f sp74 = { -15.0f, 40.0f, 0.0f };
    Color_RGBA8 unused = { 250, 250, 230, 200 };

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_va.c", 4542);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    paramsPtr = &this->actor.params;
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    switch (this->actor.params) {
        case BOSSVA_BODY:
            if (play->envCtx.adjFogNear != 0) {
                play->envCtx.adjFogNear += 350;
                if (play->envCtx.adjFogNear > 0) {
                    play->envCtx.adjFogNear = 0;
                }
            }

            if (play->envCtx.adjZFar != 0) {
                play->envCtx.adjZFar += 350;
                if (play->envCtx.adjZFar > 0) {
                    play->envCtx.adjZFar = 0;
                }
            }

            if (!this->isDead) {
                gSPSegment(POLY_OPA_DISP++, 0x08,
                           two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 8, 16, 1, 0,
                                            (play->gameplayFrames * -10) % 16, 16, 16));
                gSPSegment(POLY_OPA_DISP++, 0x09,
                           two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, (play->gameplayFrames * -10) % 32,
                                            16, 0x20, 1, 0, (play->gameplayFrames * -5) % 32, 16, 32));
                Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  boss_va_core_display1, boss_va_core_display2, this);
            }
            break;
        case BOSSVA_SUPPORT_1:
        case BOSSVA_SUPPORT_2:
        case BOSSVA_SUPPORT_3:
            if (!this->isDead) {
                Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                      this->skelAnime.dListCount, boss_arm_display1,
                                      boss_arm_display2, this);
            }
            break;
        case BOSSVA_ZAPPER_1:
        case BOSSVA_ZAPPER_2:
        case BOSSVA_ZAPPER_3:
            if (!this->isDead) {
                Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                      this->skelAnime.dListCount, boss_e_arm_display1,
                                      boss_e_arm_display2, this);
            }
            break;
        case BOSSVA_STUMP_1:
        case BOSSVA_STUMP_2:
        case BOSSVA_STUMP_3:
            Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  this->skelAnime.dListCount, NULL, NULL, NULL);
            break;
        default:
            if (!this->isDead) {
                Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  boss_bl2_display1, boss_bl2_display2, this);
                CollisionCheck_Uty_convJntSphL2G(0, &this->colliderSph);
                if (demo_idx < BOSSVA_BATTLE) {
                    spBC = GET_BODY(this)->actor.world.pos;
                } else {
                    spBC = GET_BODY(this)->unk_1D8;
                }
                Matrix_Position(&dam_d, &this->effectPos[1]);
                Matrix_push();
                Matrix_translate(spBC.x, spBC.y, spBC.z, MTXMODE_NEW);
                Matrix_rotateXYZ(this->actor.world.rot.x, this->actor.world.rot.y, 0, MTXMODE_APPLY);
                sp80.z = sp74.z = this->unk_1A0;
                spB0.z = (this->timer2 & 0xF) * (this->unk_1A0 * 0.0625f);
                Matrix_Position(&spB0, &this->effectPos[0]);
                Matrix_Position(&sp98, &this->colliderLightning.dim.quad[1]);
                Matrix_Position(&sp8C, &this->colliderLightning.dim.quad[0]);
                Matrix_Position(&sp80, &this->colliderLightning.dim.quad[3]);
                Matrix_Position(&sp74, &this->colliderLightning.dim.quad[2]);
                CollisionCheck_Uty_setSword4Pos(&this->colliderLightning, &this->colliderLightning.dim.quad[0],
                                         &this->colliderLightning.dim.quad[1], &this->colliderLightning.dim.quad[2],
                                         &this->colliderLightning.dim.quad[3]);
                Matrix_pull();
                spBC = this->actor.world.pos;
                spBC.y += 9.0f;
                if (this->actor.colorFilterTimer != 0) {
                    Eff_Off_Fog_xlu(play);
                }
                Shadow_draw(&spBC, &spA4, 0xFF, play);
                if (this->actor.colorFilterTimer != 0) {
                    Color_RGBA8 blue = { 0, 0, 255, 255 };

                    Eff_Set_Fog3_xlu(play, &blue, this->actor.colorFilterTimer, this->actor.colorFilterParams & 0xFF);
                }
            }
            break;
        case BOSSVA_DOOR:
            break;
    }

    if (*paramsPtr == BOSSVA_BODY) {
        Boss_Eff_disp(boss_eff, play);
    } else if (*paramsPtr == BOSSVA_DOOR) {
        bdan_bdoor_draw(play, d_para);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_va.c", 4673);
}

static s32 fog_color = 0x009B0000; // Unreferenced? Possibly a color

static void Boss_Eff_move(PlayState* play) {
    BossVaEffect* effect = boss_eff;
    Player* player = GET_PLAYER(play);
    s16 spB6;
    s16 i;
    f32 spB0;

    for (i = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_NONE) {
            continue;
        }

        effect->timer--;

        effect->pos.x += effect->velocity.x;
        effect->pos.y += effect->velocity.y;
        effect->pos.z += effect->velocity.z;

        effect->velocity.x += effect->accel.x;
        effect->velocity.y += effect->accel.y;
        effect->velocity.z += effect->accel.z;

        if ((effect->type == VA_LARGE_SPARK) || (effect->type == VA_SMALL_SPARK)) {
            BossVa* refActor = effect->parent;

            effect->rot.z += (s16)(fqrand() * 0x4E20) + 0x2000;
            effect->rot.y += (s16)(fqrand() * 0x2710) + 0x2000;

            if ((effect->mode == SPARK_TETHER) || (effect->mode == SPARK_UNUSED)) {
                s16 pitch = effect->rot.x - search_position_angleX(&refActor->actor.world.pos, &GET_BODY(refActor)->unk_1D8);

                spB0 = sin_s(refActor->actor.world.rot.y);
                effect->pos.x = refActor->actor.world.pos.x - (effect->offset.x * spB0);
                spB0 = cos_s(refActor->actor.world.rot.y);
                effect->pos.z = refActor->actor.world.pos.z - (effect->offset.x * spB0);
                spB0 = cos_s(-pitch);
                effect->pos.y = (effect->offset.y * spB0) + refActor->actor.world.pos.y;
            } else if ((effect->mode == SPARK_BARI) || (effect->mode == SPARK_BODY)) {
                effect->pos.x = effect->offset.x + refActor->actor.world.pos.x;
                effect->pos.y = effect->offset.y + refActor->actor.world.pos.y;
                effect->pos.z = effect->offset.z + refActor->actor.world.pos.z;
            } else {
                spB6 = rnd_f(PLAYER_BODYPART_MAX - 0.1f);
                effect->pos.x = player->bodyPartsPos[spB6].x + rnd_fx(10.0f);
                effect->pos.y = player->bodyPartsPos[spB6].y + rnd_fx(15.0f);
                effect->pos.z = player->bodyPartsPos[spB6].z + rnd_fx(10.0f);
            }

            if (effect->timer < 100) {
                effect->primColor[3] -= 50;
                if (effect->primColor[3] < 0) {
                    effect->primColor[3] = 0;
                    effect->timer = 0;
                    effect->type = VA_NONE;
                }
            }
        }

        if (effect->type == VA_BLAST_SPARK) {
            effect->rot.z += (s16)(fqrand() * 0x4E20) + 0x4000;
            if (effect->timer < 100) {
                effect->primColor[3] -= 50;
                if (effect->primColor[3] < 0) {
                    effect->primColor[3] = 0;
                    effect->timer = 0;
                    effect->type = VA_NONE;
                }
            }
        }

        if (effect->type == VA_SPARK_BALL) {
            BossVa* refActor = effect->parent;

            effect->rot.z += (s16)(fqrand() * 0x2710) + 0x24A8;
            effect->pos.x = effect->offset.x + refActor->actor.world.pos.x;
            effect->pos.y =
                refActor->actor.world.pos.y + 310.0f + (refActor->actor.shape.yOffset * refActor->actor.scale.y);
            effect->pos.z = effect->offset.z + refActor->actor.world.pos.z;
            effect->mode = (effect->mode + 1) & 7;

            if (effect->timer < 100) {
                effect->primColor[3] -= 50;
                if (effect->primColor[3] < 0) {
                    effect->primColor[3] = 0;
                    effect->timer = 0;
                    effect->type = VA_NONE;
                }
            }
        }

        if (effect->type == VA_ZAP_CHARGE) {
            effect->mode = (effect->mode + 1) & 7;
            effect->primColor[3] -= 20;
            if (effect->primColor[3] <= 0) {
                effect->primColor[3] = 0;
                effect->timer = 0;
                effect->type = VA_NONE;
            }
        }

        if (effect->type == VA_BLOOD) {
            if (effect->mode < BLOOD_SPOT) {
                f32 floorY;
                Vec3f checkPos;
                CollisionPoly* groundPoly;

                checkPos = effect->pos;
                checkPos.y -= effect->velocity.y + 4.0f;
                floorY = T_BGCheck_ObjGroundCheck(&play->colCtx, &groundPoly, &checkPos);
                if ((groundPoly != NULL) && (effect->pos.y <= floorY)) {
                    effect->mode = BLOOD_SPOT;
                    effect->pos.y = floorY + 1.0f;
                    if (demo_idx <= DEATH_SHELL_BURST) {
                        effect->timer = 80;
                    } else {
                        effect->timer = 60000;
                    }

                    effect->accel = effect->velocity = dam_d;
                }
                if (!effect->timer) {
                    effect->type = VA_NONE;
                }
            } else {
                if (effect->timer < 20) {
                    effect->envColor[3] = effect->timer * 5;
                    effect->primColor[3] = effect->timer * 10;
                } else if (effect->timer > 50000) {
                    effect->timer++;
                }
            }

            if (!effect->timer) {
                effect->type = VA_NONE;
            }
        }

        if (effect->type == VA_GORE) {
            if (effect->mode == GORE_PERMANENT) {
                f32 floorY;
                Vec3f checkPos;
                CollisionPoly* groundPoly;

                checkPos = effect->pos;
                checkPos.y -= effect->velocity.y + 4.0f;
                effect->rot.x += 0x1770;
                floorY = T_BGCheck_ObjGroundCheck(&play->colCtx, &groundPoly, &checkPos);
                if ((groundPoly != NULL) && (effect->pos.y <= floorY)) {
                    effect->mode = GORE_FLOOR;
                    effect->timer = 30;
                    effect->pos.y = floorY + 1.0f;
                    effect->accel = effect->velocity = dam_d;
                    effect->rot.x = -0x4000;
                }

                if (!effect->timer) {
                    effect->type = VA_NONE;
                }

            } else if (effect->mode == GORE_FADING) {
                if (effect->timer == 0) {
                    effect->type = VA_NONE;
                    if (1) {}
                }

            } else {
                add_calc(&effect->scaleMod, 0.075f, 1.0f, 0.005f, 0.0f);
                add_calc(&effect->vaGorePulseRate, 0.0f, 0.6f, 0.005f, 0.0013f);
                if ((play->gameplayFrames % 4) == 0) {
                    add_calc_short_angle2(&effect->primColor[0], 95, 1, 1, 0);
                }
            }
            effect->vaGorePulse += effect->vaGorePulseRate;
        }

        if (effect->type == VA_TUMOR) {
            BossVa* refActor = effect->parent;
            s16 yaw;

            effect->rot.z += 0x157C;
            effect->envColor[3] = (s16)(sin_s(effect->rot.z) * 50.0f) + 80;
            add_calc(&effect->scale, effect->scaleMod, 1.0f, 0.01f, 0.005f);
            effect->pos.x = effect->offset.x + refActor->actor.world.pos.x;
            effect->pos.y = effect->offset.y + refActor->actor.world.pos.y;
            effect->pos.z = effect->offset.z + refActor->actor.world.pos.z;

            switch (effect->mode) {
                case TUMOR_UNUSED:
                    if (effect->timer == 0) {
                        yaw = search_position_angleY(&refActor->actor.world.pos, &effect->pos);
                        effect->type = VA_NONE;
                        setGreenBlood2(play, effect, yaw, effect->scale * 4500.0f, 1);
                        setMeet(play, effect, yaw, effect->scale * 1.2f);
                    }
                    break;
                case TUMOR_BODY:
                case TUMOR_ARM:
                    if (refActor->burst) {
                        effect->type = VA_NONE;
                        yaw = search_position_angleY(&refActor->actor.world.pos, &effect->pos);
                        setGreenBlood2(play, effect, yaw, effect->scale * 4500.0f, 1);
                        setMeet(play, effect, yaw, effect->scale * 1.2f);
                    }
                    break;
            }
        }
    }
}

static void Boss_Eff_disp(BossVaEffect* effect, PlayState* play) {
    static void* lightning_txt[] = {
        gBarinadeSparkBall1Tex, gBarinadeSparkBall2Tex, gBarinadeSparkBall3Tex, gBarinadeSparkBall4Tex,
        gBarinadeSparkBall5Tex, gBarinadeSparkBall6Tex, gBarinadeSparkBall7Tex, gBarinadeSparkBall8Tex,
    };
    s16 i;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    u8 materialFlag = 0;
    BossVaEffect* effectHead = effect;
    Camera* subCam = Gama_play_get_camera(play, c_no);

    OPEN_DISPS(gfxCtx, "../z_boss_va.c", 4953);

    for (i = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_LARGE_SPARK) {
            if (materialFlag == 0) {
                _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
                gDPSetEnvColor(POLY_XLU_DISP++, 130, 130, 30, 0);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_0156A0);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 230, 230, 230, effect->primColor[3]);
            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_rotateZ((effect->rot.z / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
            Matrix_scale(effect->scale * 0.0185f, effect->scale * 0.0185f, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_va.c", 4976);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_015710);
        }
    }

    effect = effectHead;
    for (i = 0, materialFlag = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_SPARK_BALL) {
            if (materialFlag == 0) {
                _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_011738);
                materialFlag++;
            }
            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_rotateZ((effect->rot.z / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_va.c", 5002);
            gDPPipeSync(POLY_XLU_DISP++);
            gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(lightning_txt[effect->mode]));
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, effect->primColor[0], effect->primColor[1], effect->primColor[2],
                            effect->primColor[3]);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, effect->envColor[0], effect->envColor[1], effect->envColor[2],
                           effect->envColor[3]);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_011768);
        }
    }

    effect = effectHead;
    for (i = 0, materialFlag = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_BLOOD) {
            if (materialFlag == 0) {
                _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_009430);
                gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gEffBubble1Tex));
                materialFlag++;
            }

            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 0, effect->envColor[3]);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 150, 0, effect->primColor[3]);

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            if (effect->mode == BLOOD_SPOT) {
                Matrix_rotateX(M_PI / 2, MTXMODE_APPLY);
            } else {
                Matrix_rotate_scale_exchange(&play->billboardMtxF);
            }

            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            gDPPipeSync(POLY_XLU_DISP++);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_va.c", 5052);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_009468);
        }
    }

    effect = effectHead;
    for (i = 0, materialFlag = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_TUMOR) {
            BossVa* parent = effect->parent;

            if (materialFlag == 0) {
                _texture_z_light_fog_prim(play->state.gfxCtx);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, effect->envColor[3]);
                gSPDisplayList(POLY_OPA_DISP++, gBarinadeDL_0128B8);
                materialFlag++;
            }

            if ((effect->mode != TUMOR_BODY) || ((search_position_distanceXZ(&subCam->eye, &effect->pos) -
                                                  search_position_distanceXZ(&subCam->eye, &parent->actor.world.pos)) < 10.0f)) {
                Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
                Matrix_scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx, "../z_boss_va.c", 5080);
                gSPDisplayList(POLY_OPA_DISP++, gBarinadeDL_012948);
            }
        }
    }

    effect = effectHead;
    for (i = 0, materialFlag = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_GORE) {
            if (materialFlag == 0) {
                _texture_z_light_fog_prim(play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gBarinadeDL_012BA0);
                materialFlag++;
            }

            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, effect->primColor[3]);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, effect->primColor[0], effect->primColor[1], effect->primColor[2],
                            effect->primColor[3]);

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotateXYZ(effect->rot.x, effect->rot.y, 0, MTXMODE_APPLY);
            Matrix_scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_rotateX(effect->offset.x * 0.115f, MTXMODE_APPLY);
            Matrix_rotateY(effect->offset.x * 0.13f, MTXMODE_APPLY);
            Matrix_rotateZ(effect->offset.x * 0.1f, MTXMODE_APPLY);
            Matrix_scale(1.0f - effect->scaleMod, effect->scaleMod + 1.0f, 1.0f - effect->scaleMod, MTXMODE_APPLY);
            Matrix_rotateZ(-(effect->offset.x * 0.1f), MTXMODE_APPLY);
            Matrix_rotateY(-(effect->offset.x * 0.13f), MTXMODE_APPLY);
            Matrix_rotateX(-(effect->offset.x * 0.115f), MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx, "../z_boss_va.c", 5124);
            gSPDisplayList(POLY_OPA_DISP++, gBarinadeDL_012C50);
        }
    }

    effect = effectHead;
    for (i = 0, materialFlag = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_ZAP_CHARGE) {
            if (materialFlag == 0) {
                _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_0135B0);
                materialFlag++;
            }

            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 50, effect->primColor[3]);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, effect->primColor[3]);

            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotateXYZ(effect->rot.x, effect->rot.y, 0, MTXMODE_APPLY);
            Matrix_scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_va.c", 5152);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_013638);
        }
    }

    effect = effectHead;
    for (i = 0, materialFlag = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_BLAST_SPARK) {
            if (materialFlag == 0) {
                texture_z_light_prim_xlu_disp(play->state.gfxCtx);
                gDPSetEnvColor(POLY_XLU_DISP++, 130, 130, 30, 0);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_0156A0);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 230, 230, 230, effect->primColor[3]);
            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotate_scale_exchange(&play->billboardMtxF);
            Matrix_rotateZ((effect->rot.z / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
            Matrix_scale(effect->scale * 0.02f, effect->scale * 0.02f, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_va.c", 5180);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_015710);
        }
    }

    effect = effectHead;
    for (i = 0, materialFlag = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_SMALL_SPARK) {
            if (materialFlag == 0) {
                _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 100, 0);
                gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_008F08);
                materialFlag++;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, effect->primColor[3]);
            Matrix_translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_rotateZ((effect->rot.z / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
            Matrix_rotateY((effect->rot.y / (f32)0x8000) * 3.1416f, MTXMODE_APPLY);
            Matrix_scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx, "../z_boss_va.c", 5208);
            gSPDisplayList(POLY_XLU_DISP++, gBarinadeDL_008F70);
        }
    }

    CLOSE_DISPS(gfxCtx, "../z_boss_va.c", 5215);
}

void Effect_flash_mini_ct(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale, u8 mode) {
    Player* player = GET_PLAYER(play);
    s16 index;
    Vec3f pos = { 0.0f, -1000.0f, 0.0f };
    Vec3f tempVec;
    s16 i;

    for (i = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_LARGE_SPARK;
            effect->parent = this;
            effect->pos = pos;
            effect->timer = (s16)(fqrand() * 10.0f) + 111;
            effect->velocity = effect->accel = dam_d;
            effect->mode = mode;

            switch (mode) {
                case SPARK_UNUSED:
                    effect->type = VA_SMALL_SPARK;
                    FALLTHROUGH;
                case SPARK_TETHER:
                    tempVec = *offset;
                    tempVec.x += this->actor.world.pos.x;
                    tempVec.z += this->actor.world.pos.z;
                    effect->offset.x = search_position_distanceXZ(&this->actor.world.pos, &tempVec);
                    effect->rot.x = search_position_angleX(&this->actor.world.pos, &GET_BODY(this)->unk_1D8);
                    break;

                case SPARK_BODY:
                    effect->type = VA_SMALL_SPARK;
                    FALLTHROUGH;
                case SPARK_BARI:
                    effect->offset.x = offset->x;
                    effect->offset.z = offset->z;
                    break;

                case SPARK_BLAST:
                    effect->type = VA_BLAST_SPARK;
                    effect->pos.x = offset->x + this->actor.world.pos.x;
                    effect->pos.y = offset->y + this->actor.world.pos.y;
                    effect->pos.z = offset->z + this->actor.world.pos.z;
                    effect->timer = 111;
                    break;

                case SPARK_LINK:
                    effect->type = VA_SMALL_SPARK;
                    index = rnd_f(PLAYER_BODYPART_MAX - 0.1f);
                    effect->pos.x = player->bodyPartsPos[index].x + rnd_fx(10.0f);
                    effect->pos.y = player->bodyPartsPos[index].y + rnd_fx(15.0f);
                    effect->pos.z = player->bodyPartsPos[index].z + rnd_fx(10.0f);
                    break;
            }

            effect->offset.y = offset->y;
            effect->scale = (rnd_f(scale) + scale) * 0.01f;
            effect->primColor[3] = 255;
            break;
        }
    }
}

void Effect_lightning_ct(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale, u8 mode) {
    Vec3f pos = { 0.0f, -1000.0f, 0.0f };
    s16 i;

    for (i = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_SPARK_BALL;
            effect->parent = this;

            effect->pos = pos;

            effect->velocity = effect->accel = dam_d;

            effect->mode = 0;
            effect->offset.x = offset->x;
            effect->offset.z = offset->z;
            effect->offset.y = offset->y;
            effect->timer = (s16)(fqrand() * 10.0f) + 111;
            effect->primColor[0] = effect->primColor[1] = effect->primColor[2] = effect->primColor[3] = 230;
            effect->envColor[0] = 0;
            effect->envColor[1] = 100;
            effect->envColor[2] = 220;
            effect->envColor[3] = 160;

            effect->scale = (rnd_f(scale) + scale) * 0.01f;
            return;
        }
    }
}

void Effect_Blood_ct(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 scale, s16 phase, s16 yaw) {
    s32 i;
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    f32 xzVel;

    for (i = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_BLOOD;
            effect->pos = *pos;
            effect->mode = BLOOD_DROPLET;

            xzVel = sin_s(phase) * 6.0f;
            velocity.x = rnd_fx(1.0f) + (-sin_s(yaw) * xzVel);
            velocity.z = rnd_fx(1.0f) + (-cos_s(yaw) * xzVel);

            effect->velocity = velocity;

            accel.y = rnd_fx(0.3f) - 1.0f;
            effect->accel = accel;

            effect->timer = 20;
            effect->envColor[3] = 100;
            effect->primColor[3] = 200;
            effect->scale = (rnd_f(scale) + scale) * 0.01f;
            break;
        }
    }
}

void Effect_Blood_ct2(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 yaw, s16 scale) {
    s32 i;
    f32 xzVel;
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity;

    for (i = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_BLOOD;
            effect->pos = *pos;

            effect->mode = BLOOD_SPLATTER;

            xzVel = fqrand() * 7.0f;
            velocity.x = sin_s(yaw) * xzVel;
            velocity.y = rnd_fx(4.0f) + 4.0f;
            velocity.z = cos_s(yaw) * xzVel;
            effect->velocity = velocity;

            accel.y = rnd_fx(0.3f) - 1.0f;
            effect->accel = accel;

            if (demo_idx <= DEATH_SHELL_BURST) {
                effect->timer = 20;
            } else {
                effect->timer = 60;
            }
            effect->envColor[3] = 100;
            effect->primColor[3] = 200;
            effect->scale = scale * 0.01f;
            break;
        }
    }
}

void Effect_s_core_ct(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* offset, s16 scale, u8 mode) {
    Vec3f pos = { 0.0f, -1000.0f, 0.0f };
    s16 i;

    for (i = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_TUMOR;
            effect->parent = this;
            effect->pos = pos;

            effect->velocity = effect->accel = dam_d;

            effect->mode = mode;
            effect->rot.z = 0;

            effect->offset.x = offset->x;
            effect->offset.z = offset->z;
            effect->offset.y = offset->y;

            effect->timer = (s16)(fqrand() * 10.0f) + 10;
            effect->envColor[3] = 100;
            effect->scaleMod = scale * 0.01f;
            effect->scale = 0.0f;

            if (((i % 4) == 0) || (mode == 2)) {
                Nai_FxFlagEntry(NA_SE_EN_BALINADE_BREAK, &effect->pos, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
            }
            break;
        }
    }
}

void Effect_meet_ct(PlayState* play, BossVaEffect* effect, Vec3f* pos, s16 yaw, s16 scale) {
    s32 i;
    f32 xzVel;
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f velocity;

    for (i = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_GORE;
            effect->pos = *pos;
            effect->scaleMod = 0.0f;

            xzVel = (fqrand() * 4.0f) + 4.0f;
            velocity.x = sin_s(yaw) * xzVel;
            velocity.y = rnd_fx(8.0f);
            velocity.z = cos_s(yaw) * xzVel;
            effect->velocity = velocity;

            accel.y = rnd_fx(0.3f) - 1.0f;
            effect->accel = accel;

            effect->timer = 20;
            if (demo_idx <= DEATH_SHELL_BURST) {
                effect->mode = GORE_FADING;
            } else {
                effect->mode = GORE_PERMANENT;
            }

            effect->primColor[3] = effect->envColor[0] = effect->envColor[1] = effect->envColor[2] =
                effect->envColor[3] = 255;

            effect->primColor[0] = 155;
            effect->primColor[1] = effect->primColor[2] = 55;

            effect->rot.x = rnd_fx(0x10000);
            effect->rot.y = rnd_fx(0x10000);
            effect->scale = (rnd_f(scale) + scale) * 0.01f;
            effect->vaGorePulseRate = (fqrand() * 0.25f) + 0.9f;
            break;
        }
    }
}

void Effect_l_cv_ct(PlayState* play, BossVaEffect* effect, BossVa* this, Vec3f* pos, Vec3s* rot, s16 scale,
                              u8 mode) {
    Vec3f unused = { 0.0f, -1000.0f, 0.0f };
    s16 i;

    for (i = 0; i < BOSS_VA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == VA_NONE) {
            effect->type = VA_ZAP_CHARGE;
            effect->parent = this;
            effect->pos = *pos;

            effect->velocity = effect->accel = dam_d;

            effect->mode = mode;
            effect->rot.x = rot->x + 0x4000;
            effect->rot.y = rot->y;
            effect->timer = (s16)(fqrand() * 10.0f) + 10;
            effect->primColor[3] = 240;
            effect->scale = scale * 0.01f;
            break;
        }
    }
}

void bdan_bdoor_draw(PlayState* play, s16 scale) {
    static Gfx* bdan_bdoor_model[] = {
        gBarinadeDoorPiece1DL, gBarinadeDoorPiece2DL, gBarinadeDoorPiece3DL, gBarinadeDoorPiece4DL,
        gBarinadeDoorPiece5DL, gBarinadeDoorPiece6DL, gBarinadeDoorPiece7DL, gBarinadeDoorPiece8DL,
    };
    static s16 trans_y[] = { 836, 900, 836, 1016, 800, 1016, 836, 900 };
    MtxF doorMtx;
    f32 yScale;
    f32 segAngle = 0.0f;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_boss_va.c", 5600);

    Matrix_translate(0.0f, 80.0f, 400.0f, MTXMODE_NEW);
    Matrix_rotateY(M_PI, MTXMODE_APPLY);
    yScale = (scale * 0.01f) * 0.1f;
    Matrix_scale(0.1f, yScale, 0.1f, MTXMODE_APPLY);

    if (yScale != 0.0f) {
        yScale = 0.1f / yScale;
    } else {
        yScale = 0.0f;
    }

    Matrix_get(&doorMtx);

    for (i = 0; i < 8; i++) {
        Matrix_put(&doorMtx);
        Matrix_rotateZ(segAngle, MTXMODE_APPLY);
        Matrix_translate(0.0f, trans_y[i] * yScale, 0.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_boss_va.c", 5621);
        gSPDisplayList(POLY_OPA_DISP++, bdan_bdoor_model[i]);
        segAngle -= M_PI / 4;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_boss_va.c", 5629);
}
