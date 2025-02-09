#include "z_en_dodongo.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_En_Bombf/z_en_bombf.h"
#include "assets/objects/object_dodongo/object_dodongo.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

typedef enum EnDodongoActionState {
    DODONGO_SWEEP_TAIL,
    DODONGO_SWALLOW_BOMB,
    DODONGO_DEATH,
    DODONGO_BREATHE_FIRE,
    DODONGO_IDLE,
    DODONGO_END_BREATHE_FIRE,
    DODONGO_UNUSED,
    DODONGO_STUNNED,
    DODONGO_WALK
} EnDodongoActionState;

void En_Dodongo_actor_ct(Actor* thisx, PlayState* play);
void En_Dodongo_actor_dt(Actor* thisx, PlayState* play);
void En_Dodongo_actor_move(Actor* thisx, PlayState* play);
void En_Dodongo_actor_draw(Actor* thisx, PlayState* play2);

void En_Da_Actor_mode_down_init(EnDodongo* this, PlayState* play);
void XZ_offset_add(s16 yaw, f32 radius, Vec3f* vec);
static s32 MouseVsBombCheck(EnDodongo* this, PlayState* play);
static void mode_wait_init(EnDodongo* this);

static void mode_wait(EnDodongo* this, PlayState* play);
void mode_fire_wait(EnDodongo* this, PlayState* play);
static void mode_fire(EnDodongo* this, PlayState* play);
void mode_bomb_e(EnDodongo* this, PlayState* play);
static void mode_walk(EnDodongo* this, PlayState* play);
static void mode_paralyze(EnDodongo* this, PlayState* play);
void En_Da_Actor_mode_down(EnDodongo* this, PlayState* play);
void En_Da_Actor_mode_damage(EnDodongo* this, PlayState* play);

ActorProfile En_Dodongo_Profile = {
    /**/ ACTOR_EN_DODONGO,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DODONGO,
    /**/ sizeof(EnDodongo),
    /**/ En_Dodongo_actor_ct,
    /**/ En_Dodongo_actor_dt,
    /**/ En_Dodongo_actor_move,
    /**/ En_Dodongo_actor_draw,
};

static ColliderJntSphElementInit JntSphElemData[6] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 15, { { 0, 0, 0 }, 17 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 14, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 10 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 21, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 28, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0D800691, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON | OCELEM_UNK3,
        },
        { 6, { { 0, 0, 0 }, 35 }, 100 },
    },
};

static ColliderJntSphInit JntSphData = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    6,
    JntSphElemData,
};

static ColliderTrisElementInit AcInfoDataElem[3] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xF24BF96E, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_HOOKABLE | ACELEM_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, 14.0f, 2.0f }, { -10.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCBF96E, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_HOOKABLE | ACELEM_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCBF96E, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON | ACELEM_HOOKABLE | ACELEM_NO_AT_INFO,
            OCELEM_NONE,
        },
        { { { -10.0f, -6.0f, 2.0f }, { 9.0f, -6.0f, 2.0f }, { 9.0f, 14.0f, 2.0f } } },
    },
};

static ColliderTrisInit AcInfoData = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    3,
    AcInfoDataElem,
};

static ColliderQuadInit AtInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x01, 0x10 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(1, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(4, 0xF),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x6),
    /* Ice magic     */ DMG_ENTRY(3, 0xF),
    /* Light magic   */ DMG_ENTRY(0, 0x6),
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

void En_Da_actor_set_process(EnDodongo* this, EnDodongoActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void bsmk_set(EnDodongo* this, PlayState* play) {
    s32 pad;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };
    Vec3f pos = this->headPos;
    s16 randAngle = rnd_fx(0x4000);
    f32 randCos;
    f32 randSin;

    randCos = cos_s(this->actor.shape.rot.y + randAngle);
    randSin = sin_s(this->actor.shape.rot.y + randAngle);
    if (this->bombSmokePrimColor.r > 30) {
        this->bombSmokePrimColor.r -= 16;
        this->bombSmokePrimColor.g -= 16;
    }

    if (this->bombSmokePrimColor.b < 30) {
        this->bombSmokePrimColor.b += 5;
        this->bombSmokePrimColor.a += 8;
        this->bombSmokeEnvColor.a += 8;
    }
    if (this->bombSmokeEnvColor.r != 0) {
        this->bombSmokeEnvColor.r -= 15;
    }
    if (this->bombSmokeEnvColor.g != 0) {
        this->bombSmokeEnvColor.g--;
    }
    velocity.x = randSin * 3.5f;
    velocity.y = this->bombSmokeEnvColor.r * 0.02f;
    velocity.z = randCos * 3.5f;
    accel.x = ((fqrand() * 0.1f) + 0.15f) * -randSin;
    accel.z = ((fqrand() * 0.1f) + 0.15f) * -randCos;
    Effect_SS_Dust_sc_cl_co_ct(play, &pos, &velocity, &accel, &this->bombSmokePrimColor, &this->bombSmokeEnvColor, 100, 25, 20);

    randAngle = fqrand() * 0x2000;
    randCos = cos_s(this->actor.shape.rot.y + randAngle);
    randSin = sin_s(this->actor.shape.rot.y + randAngle);
    pos.x -= randCos * 6.0f;
    pos.z += randSin * 6.0f;
    velocity.x = -randCos * 3.5f;
    velocity.y = this->bombSmokeEnvColor.r * 0.02f;
    velocity.z = randSin * 3.5f;
    accel.x = ((fqrand() * 0.1f) + 0.15f) * randCos;
    accel.z = ((fqrand() * 0.1f) + 0.15f) * -randSin;
    Effect_SS_Dust_sc_cl_co_ct(play, &pos, &velocity, &accel, &this->bombSmokePrimColor, &this->bombSmokeEnvColor, 100, 25, 20);

    randAngle = fqrand() * 0x2000;
    randCos = cos_s(this->actor.shape.rot.y + randAngle);
    randSin = sin_s(this->actor.shape.rot.y + randAngle);

    pos.x = this->headPos.x + (randCos * 6.0f);
    pos.z = this->headPos.z - (randSin * 6.0f);
    velocity.x = randCos * 3.5f;
    velocity.y = this->bombSmokeEnvColor.r * 0.02f;
    velocity.z = -randSin * 3.5f;
    accel.x = ((fqrand() * 0.1f) + 0.15f) * -randCos;
    accel.z = ((fqrand() * 0.1f) + 0.15f) * randSin;
    Effect_SS_Dust_sc_cl_co_ct(play, &pos, &velocity, &accel, &this->bombSmokePrimColor, &this->bombSmokeEnvColor, 100, 25, 20);
}

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_DODONGO, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2800, ICHAIN_STOP),
};

void En_Dodongo_actor_ct(Actor* thisx, PlayState* play) {
    EnDodongo* this = (EnDodongo*)thisx;
    EffectBlureInit1 blureInit;

    this->actor.attentionRangeType = ATTENTION_RANGE_3;
    ValueSet_process(&this->actor, value_init);
    this->bombSmokePrimColor.r = this->bombSmokePrimColor.g = this->bombSmokeEnvColor.r = 255;
    this->bombSmokePrimColor.a = this->bombSmokeEnvColor.a = 200;
    this->bombSmokeEnvColor.g = 10;
    this->bodyScale.x = this->bodyScale.y = this->bodyScale.z = 1.0f;
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 48.0f);
    Actor_set_scale(&this->actor, 0.01875f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gDodongoSkel, &gDodongoWaitAnim, this->jointTable, this->morphTable, 31);
    this->actor.colChkInfo.health = 4;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actor.colChkInfo.damageTable = &btl_data;
    ClObjSwrd_ct(play, &this->colliderAT);
    ClObjTris_ct(play, &this->colliderHard);
    ClObjJntSph_ct(play, &this->colliderBody);
    ClObjSwrd_set5(play, &this->colliderAT, &this->actor, &AtInfoData);
    ClObjTris_set5_nzm(play, &this->colliderHard, &this->actor, &AcInfoData, this->trisElements);
    ClObjJntSph_set5_nzm(play, &this->colliderBody, &this->actor, &JntSphData, this->sphElements);

    blureInit.p1StartColor[0] = blureInit.p1StartColor[1] = blureInit.p1StartColor[2] = blureInit.p1StartColor[3] =
        blureInit.p2StartColor[0] = blureInit.p2StartColor[1] = blureInit.p2StartColor[2] = blureInit.p1EndColor[0] =
            blureInit.p1EndColor[1] = blureInit.p1EndColor[2] = blureInit.p2EndColor[0] = blureInit.p2EndColor[1] =
                blureInit.p2EndColor[2] = 255;

    blureInit.p1EndColor[3] = blureInit.p2EndColor[3] = 0;
    blureInit.p2StartColor[3] = 64;
    blureInit.elemDuration = 8;
    blureInit.unkFlag = false;
    blureInit.calcMode = 2;

    EffectAdd(play, &this->blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
    Actor_BGcheck2(play, &this->actor, 75.0f, 60.0f, 70.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    mode_wait_init(this);
}

void En_Dodongo_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDodongo* this = (EnDodongo*)thisx;

    EffectFreeIndex(play, this->blureIdx);
    ClObjTris_dt_nzf(play, &this->colliderHard);
    ClObjJntSph_dt_nzf(play, &this->colliderBody);
    ClObjSwrd_dt(play, &this->colliderAT);
}

static void mode_wait_init(EnDodongo* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gDodongoWaitAnim, -4.0f);
    this->actor.speed = 0.0f;
    this->timer = get_random_timer(30, 50);
    this->actionState = DODONGO_IDLE;
    En_Da_actor_set_process(this, mode_wait);
}

static void mode_walk_init(EnDodongo* this) {
    f32 frames = Si2_anime_end_frame(&gDodongoWalkAnim);

    Skeleton_Info2_init(&this->skelAnime, &gDodongoWalkAnim, 0.0f, 0.0f, frames, ANIMMODE_LOOP, -4.0f);
    this->actor.speed = 1.5f;
    this->timer = get_random_timer(50, 70);
    this->rightFootStep = true;
    this->actionState = DODONGO_WALK;
    En_Da_actor_set_process(this, mode_walk);
}

static void mode_fire_init(EnDodongo* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gDodongoBreatheFireAnim, -4.0f);
    this->actionState = DODONGO_BREATHE_FIRE;
    this->actor.speed = 0.0f;
    En_Da_actor_set_process(this, mode_fire);
}

void mode_fire_wait_init(EnDodongo* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gDodongoAfterBreatheFireAnim);
    this->actionState = DODONGO_END_BREATHE_FIRE;
    this->actor.speed = 0.0f;
    En_Da_actor_set_process(this, mode_fire_wait);
}

void mode_bomb_e_init(EnDodongo* this) {
    Skeleton_Info2_init(&this->skelAnime, &gDodongoBreatheFireAnim, -1.0f, 35.0f, 0.0f, ANIMMODE_ONCE, -4.0f);
    this->actionState = DODONGO_SWALLOW_BOMB;
    this->timer = 25;
    this->actor.speed = 0.0f;
    En_Da_actor_set_process(this, mode_bomb_e);
}

static void mode_paralyze_init(EnDodongo* this) {
    Skeleton_Info2_init(&this->skelAnime, &gDodongoBreatheFireAnim, 0.0f, 25.0f, 0.0f, ANIMMODE_ONCE, -4.0f);
    this->actionState = DODONGO_STUNNED;
    this->actor.speed = 0.0f;
    if (this->damageEffect == 0xF) {
        this->iceTimer = 36;
    }
    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
    En_Da_actor_set_process(this, mode_paralyze);
}

static void mode_wait(EnDodongo* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((DECR(this->timer) == 0) && Skeleton_Info_frame_check(&this->skelAnime, 0.0f)) {
        mode_walk_init(this);
    }
}

void mode_fire_wait(EnDodongo* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_wait_init(this);
        this->timer = get_random_timer(10, 20);
    }
}

static void mode_fire(EnDodongo* this, PlayState* play) {
    s32 pad;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;
    s16 pad2;
    s16 fireFrame;

    if ((s32)this->skelAnime.curFrame == 24) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_J_CRY);
    }
    if ((29.0f <= this->skelAnime.curFrame) && (this->skelAnime.curFrame <= 43.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_J_FIRE - SFX_FLAG);
        fireFrame = this->skelAnime.curFrame - 29.0f;
        pos = this->actor.world.pos;
        pos.y += 35.0f;
        XZ_offset_add(this->actor.world.rot.y, 30.0f, &pos);
        XZ_offset_add(this->actor.world.rot.y, 2.5f, &accel);
        Effect_SS_Dfire_ct(play, &pos, &velocity, &accel, 255 - (fireFrame * 10), fireFrame + 3);
    } else if ((2.0f <= this->skelAnime.curFrame) && (this->skelAnime.curFrame <= 20.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_J_BREATH - SFX_FLAG);
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_fire_wait_init(this);
    }
}

void mode_bomb_e(EnDodongo* this, PlayState* play) {
    Vec3f smokeVel = { 0.0f, 0.0f, 0.0f };
    Vec3f smokeAccel = { 0.0f, 0.6f, 0.0f };
    Color_RGBA8 white = { 255, 255, 255, 255 };
    Vec3f deathFireVel = { 0.0f, 0.0f, 0.0f };
    Vec3f deathFireAccel = { 0.0f, 1.0f, 0.0f };
    s16 i;
    Vec3f pos;
    s32 pad;

    if (this->actor.child != NULL) {
        this->actor.child->world.pos = this->mouthPos;
        ((EnBom*)this->actor.child)->timer++;
    } else if (this->actor.parent != NULL) {
        this->actor.parent->world.pos = this->mouthPos;
        ((EnBombf*)this->actor.parent)->timer++;
        //! @bug An explosive can also be a bombchu, not always a bomb, which leads to a serious bug. ->timer (0x1F8) is
        //! outside the bounds of the bombchu actor, and the memory it writes to happens to be one of the pointers in
        //! the next arena node. When this value is written to, massive memory corruption occurs.
    }

    if ((s32)this->skelAnime.curFrame == 28) {
        Actor_SE_set(&this->actor, NA_SE_EN_DODO_J_EAT);
        if (this->actor.child != NULL) {
            Actor_delete(this->actor.child);
            this->actor.child = NULL;
        } else if (this->actor.parent != NULL) {
            Actor_delete(this->actor.parent);
            this->actor.parent = NULL;
        }
    } else if ((s32)this->skelAnime.curFrame == 24) {
        this->timer--;
        if (this->timer != 0) {
            this->skelAnime.curFrame++;
            if (this->timer == 10) {
                for (i = 10; i >= 0; i--) {
                    deathFireVel.x = rnd_fx(10.0f);
                    deathFireVel.y = rnd_fx(10.0f);
                    deathFireVel.z = rnd_fx(10.0f);
                    deathFireAccel.x = deathFireVel.x * -0.1f;
                    deathFireAccel.y = deathFireVel.y * -0.1f;
                    deathFireAccel.z = deathFireVel.z * -0.1f;
                    pos.x = this->sphElements[0].dim.worldSphere.center.x + deathFireVel.x;
                    pos.y = this->sphElements[0].dim.worldSphere.center.y + deathFireVel.y;
                    pos.z = this->sphElements[0].dim.worldSphere.center.z + deathFireVel.z;
                    Effect_SS_Dust_sc_cl_co_ct(play, &pos, &deathFireVel, &deathFireAccel, &this->bombSmokePrimColor,
                                  &this->bombSmokeEnvColor, 400, 10, 10);
                }
                Actor_SE_set(&this->actor, NA_SE_IT_BOMB_EXPLOSION);
                Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 120, COLORFILTER_BUFFLAG_OPA, 8);
            }
        }
    }
    if ((s32)this->skelAnime.curFrame < 28) {
        if (((s32)this->skelAnime.curFrame < 26) && (this->timer <= 10)) {
            bsmk_set(this, play);
        } else {
            pos = this->headPos;
            Effect_SS_Dust_sc_cl_ct(play, &pos, &smokeVel, &smokeAccel, &white, &white, 50, 5);
            pos.x -= (cos_s(this->actor.shape.rot.y) * 6.0f);
            pos.z += (sin_s(this->actor.shape.rot.y) * 6.0f);
            Effect_SS_Dust_sc_cl_ct(play, &pos, &smokeVel, &smokeAccel, &white, &white, 50, 5);
            pos.x = this->headPos.x + (cos_s(this->actor.shape.rot.y) * 6.0f);
            pos.z = this->headPos.z - (sin_s(this->actor.shape.rot.y) * 6.0f);
            Effect_SS_Dust_sc_cl_ct(play, &pos, &smokeVel, &smokeAccel, &white, &white, 50, 5);
        }
    }
    this->bodyScale.y = this->bodyScale.z = (sin_s(this->actor.colorFilterTimer * 0x1000) * 0.5f) + 1.0f;
    this->bodyScale.x = sin_s(this->actor.colorFilterTimer * 0x1000) + 1.0f;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->timer == 0) {
        En_Da_Actor_mode_down_init(this, play);
    }
}

static void mode_walk(EnDodongo* this, PlayState* play) {
    s32 pad;
    f32 playbackSpeed;
    Player* player = GET_PLAYER(play);
    s16 yawDiff = (s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y);

    yawDiff = ABS(yawDiff);

    add_calc(&this->actor.speed, 1.5f, 0.1f, 1.0f, 0.0f);

    playbackSpeed = this->actor.speed * 0.75f;
    if (this->actor.speed >= 0.0f) {
        if (playbackSpeed > 3.0f / 2) {
            playbackSpeed = 3.0f / 2;
        }
    } else {
        if (playbackSpeed < -3.0f / 2) {
            playbackSpeed = -3.0f / 2;
        }
    }
    this->skelAnime.playSpeed = playbackSpeed;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((s32)this->skelAnime.curFrame < 21) {
        if (!this->rightFootStep) {
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_J_WALK);
            _dust_ground_set(play, &this->actor, &this->leftFootPos, 10.0f, 3, 2.0f, 200, 15, false);
            this->rightFootStep = true;
        }
    } else {
        if (this->rightFootStep) {
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_J_WALK);
            _dust_ground_set(play, &this->actor, &this->rightFootPos, 10.0f, 3, 2.0f, 200, 15, false);
            this->rightFootStep = false;
        }
    }

    if (search_position_distanceXZ(&this->actor.home.pos, &player->actor.world.pos) < 400.0f) {
        add_calc_short_angle2(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x1F4, 0);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        if ((this->actor.xzDistToPlayer < 100.0f) && (yawDiff < 0x1388) && (this->actor.yDistToPlayer < 60.0f)) {
            mode_fire_init(this);
        }
    } else {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        if ((search_position_distanceXZ(&this->actor.world.pos, &this->actor.home.pos) > 150.0f) || (this->retreatTimer != 0)) {
            s16 yawToHome = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos);

            add_calc_short_angle2(&this->actor.world.rot.y, yawToHome, 1, 0x1F4, 0);
        }
        if (this->retreatTimer != 0) {
            this->retreatTimer--;
        }
        this->timer--;
        if (this->timer == 0) {
            if (fqrand() > 0.7f) {
                this->timer = get_random_timer(50, 70);
                this->retreatTimer = get_random_timer(15, 40);
            } else {
                mode_wait_init(this);
            }
        }
    }

    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void En_Da_Actor_mode_damage_init(EnDodongo* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gDodongoDamageAnim, -4.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_DODO_J_DAMAGE);
    this->actionState = DODONGO_SWEEP_TAIL;
    this->timer = 0;
    this->actor.speed = 0.0f;
    En_Da_actor_set_process(this, En_Da_Actor_mode_damage);
}

void En_Da_Actor_mode_damage(EnDodongo* this, PlayState* play) {
    s16 yawDiff1 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if ((this->timer != 0) || (ABS(yawDiff1) < 0x4000)) {
            this->sphElements[2].base.atElemFlags = ATELEM_NONE;
            this->sphElements[1].base.atElemFlags = ATELEM_NONE;
            this->colliderBody.base.atFlags = AT_NONE;
            this->sphElements[2].base.atDmgInfo.dmgFlags = 0;
            this->sphElements[1].base.atDmgInfo.dmgFlags = 0;
            this->sphElements[2].base.atDmgInfo.damage = 0;
            this->sphElements[1].base.atDmgInfo.damage = 0;
            mode_fire_init(this);
            this->timer = get_random_timer(5, 10);
        } else {
            s16 yawDiff2 = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
            AnimationHeader* animation;

            this->tailSwipeSpeed = (0xFFFF - ABS(yawDiff2)) / 0xF;
            if ((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y) >= 0) {
                this->tailSwipeSpeed = -this->tailSwipeSpeed;
                animation = &gDodongoSweepTailLeftAnim;
            } else {
                animation = &gDodongoSweepTailRightAnim;
            }
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_J_TAIL);
            Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, animation, 2.0f);
            this->timer = 18;
            this->colliderBody.base.atFlags = this->sphElements[1].base.atElemFlags =
                this->sphElements[2].base.atElemFlags = AT_ON | AT_TYPE_ENEMY; // also ATELEM_ON | ATELEM_SFX_WOOD
            this->sphElements[1].base.atDmgInfo.dmgFlags = this->sphElements[2].base.atDmgInfo.dmgFlags = DMG_DEFAULT;
            this->sphElements[1].base.atDmgInfo.damage = this->sphElements[2].base.atDmgInfo.damage = 8;
        }
    } else if (this->timer > 1) {
        Vec3f tailPos;

        this->timer--;
        this->actor.shape.rot.y = this->actor.world.rot.y += this->tailSwipeSpeed;
        tailPos.x = this->sphElements[1].dim.worldSphere.center.x;
        tailPos.y = this->sphElements[1].dim.worldSphere.center.y;
        tailPos.z = this->sphElements[1].dim.worldSphere.center.z;
        _dust_ground_set(play, &this->actor, &tailPos, 5.0f, 2, 2.0f, 100, 15, false);
        tailPos.x = this->sphElements[2].dim.worldSphere.center.x;
        tailPos.y = this->sphElements[2].dim.worldSphere.center.y;
        tailPos.z = this->sphElements[2].dim.worldSphere.center.z;
        _dust_ground_set(play, &this->actor, &tailPos, 5.0f, 2, 2.0f, 100, 15, false);

        if (this->colliderBody.base.atFlags & AT_HIT) {
            Player* player = GET_PLAYER(play);

            if (this->colliderBody.base.at == &player->actor) {
                Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
            }
        }
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderBody.base);
    }
}

void En_Da_Actor_mode_down_init(EnDodongo* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gDodongoDieAnim, -8.0f);
    this->timer = 0;
    Actor_SE_set(&this->actor, NA_SE_EN_DODO_J_DEAD);
    this->actionState = DODONGO_DEATH;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.speed = 0.0f;
    En_Da_actor_set_process(this, En_Da_Actor_mode_down);
}

void En_Da_Actor_mode_down(EnDodongo* this, PlayState* play) {
    if (this->skelAnime.curFrame < 35.0f) {
        if (this->actor.params == EN_DODONGO_SMOKE_DEATH) {
            bsmk_set(this, play);
        }
    } else if (this->actor.colorFilterTimer == 0) {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 120, COLORFILTER_BUFFLAG_OPA, 4);
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->timer == 0) {
            EnBom* bomb = (EnBom*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                              this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 6, BOMB_BODY);
            if (bomb != NULL) {
                bomb->timer = 0;
                this->timer = 8;
            }
        }
    } else if ((s32)this->skelAnime.curFrame == 52) {
        Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DOWN);
    }
    if (this->timer != 0) {
        this->timer--;
        if (this->timer == 0) {
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x40);
            Actor_delete(&this->actor);
        }
    }
}

static void mode_paralyze(EnDodongo* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->actor.colorFilterTimer == 0) {
        if (this->actor.colChkInfo.health == 0) {
            En_Da_Actor_mode_down_init(this, play);
        } else {
            mode_wait_init(this);
        }
    }
}

void En_Da_damage_proc(EnDodongo* this, PlayState* play) {
    if (this->colliderHard.base.acFlags & AC_BOUNCED) {
        this->colliderHard.base.acFlags &= ~AC_BOUNCED;
        this->colliderBody.base.acFlags &= ~AC_HIT;
    } else if ((this->colliderBody.base.acFlags & AC_HIT) && (this->actionState > DODONGO_DEATH)) {
        this->colliderBody.base.acFlags &= ~AC_HIT;
        Hit_bit_set_sph(&this->actor, &this->colliderBody, false);
        if (this->actor.colChkInfo.damageEffect != 0xE) {
            this->damageEffect = this->actor.colChkInfo.damageEffect;
            if ((this->actor.colChkInfo.damageEffect == 1) || (this->actor.colChkInfo.damageEffect == 0xF)) {
                if (this->actionState != DODONGO_STUNNED) {
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 80);
                    hp_down(&this->actor);
                    mode_paralyze_init(this);
                }
            } else {
                Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 120, COLORFILTER_BUFFLAG_OPA, 8);
                if (hp_down(&this->actor) == 0) {
                    En_Da_Actor_mode_down_init(this, play);
                } else {
                    En_Da_Actor_mode_damage_init(this);
                }
            }
        }
    }
}

void en_da_attack_proc(EnDodongo* this, PlayState* play) {
    Vec3f sp94 = { -1000.0f, -1500.0f, 0.0f };
    Vec3f sp88 = { -1000.0f, -200.0f, 1500.0f };
    Vec3f sp7C = { -1000.0f, -200.0f, -1500.0f };
    Vec3f sp70 = { 0.0f, 0.0f, 0.0f };
    s32 pad4C[9]; // Possibly 3 more Vec3fs?
    s32 a = 0;
    s32 b = 1; // These indices are needed to match.
    s32 c = 2; // Might be a way to quickly test vertex arrangements
    s32 d = 3;
    f32 xMod = sinf_table((this->skelAnime.curFrame - 28.0f) * 0.08f) * 5500.0f;

    sp7C.x -= xMod;
    sp94.x -= xMod;
    sp88.x -= xMod;

    Matrix_Position(&sp94, &this->colliderAT.dim.quad[b]);
    Matrix_Position(&sp88, &this->colliderAT.dim.quad[a]);
    Matrix_Position(&sp7C, &this->colliderAT.dim.quad[d]);
    Matrix_Position(&sp70, &this->colliderAT.dim.quad[c]);

    CollisionCheck_Uty_setSword4Pos(&this->colliderAT, &this->colliderAT.dim.quad[a], &this->colliderAT.dim.quad[b],
                             &this->colliderAT.dim.quad[c], &this->colliderAT.dim.quad[d]);
}

void En_Dodongo_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDodongo* this = (EnDodongo*)thisx;

    En_Da_damage_proc(this, play);
    if (this->actor.colChkInfo.damageEffect != 0xE) {
        this->actionFunc(this, play);
        Actor_position_moveF(&this->actor);
        Actor_BGcheck2(play, &this->actor, 75.0f, 60.0f, 70.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                    UPDBGCHECKINFO_FLAG_4);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            Actor_SE_set(&this->actor, NA_SE_EN_RIZA_DOWN);
        }
    }
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderBody.base);
    if (this->actionState != DODONGO_DEATH) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderHard.base);
    }
    if (this->actionState > DODONGO_DEATH) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderBody.base);
    }
    if ((this->actionState >= DODONGO_IDLE) && MouseVsBombCheck(this, play)) {
        mode_bomb_e_init(this);
    }
    if (this->actionState == DODONGO_BREATHE_FIRE) {
        if ((29.0f < this->skelAnime.curFrame) && (this->skelAnime.curFrame < 43.0f)) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderAT.base);
        }
    }
    this->actor.focus.pos.x = this->actor.world.pos.x + sin_s(this->actor.shape.rot.y) * -30.0f;
    this->actor.focus.pos.y = this->actor.world.pos.y + 20.0f;
    this->actor.focus.pos.z = this->actor.world.pos.z + cos_s(this->actor.shape.rot.y) * -30.0f;
}

s32 en_dodongo_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDodongo* this = (EnDodongo*)thisx;

    if ((limbIndex == 15) || (limbIndex == 16)) {
        Matrix_scale(this->bodyScale.x, this->bodyScale.y, this->bodyScale.z, MTXMODE_APPLY);
    }
    return false;
}

void en_dodongo_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f local_eye[3] = {
        { 1100.0f, -700.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { 2190.0f, 0.0f, 0.0f },
    };
    Vec3f tailTipOffset = { 3000.0f, 0.0f, 0.0f };
    Vec3f baseOffset = { 0.0f, 0.0f, 0.0f };
    s32 i;
    Vec3f hardTris0Vtx[3];
    Vec3f hardTris1Vtx[3];
    Vec3f hardTris2Vtx[3];
    Vec3f tailTip;
    Vec3f tailBase;
    EnDodongo* this = (EnDodongo*)thisx;
    Vec3f hardTris0VtxOffset[] = {
        { -300.0f, -2500.0f, 0.0f },
        { -300.0f, 1200.0f, -2700.0f },
        { 3000.0f, 1200.0f, 0.0f },
    };
    Vec3f hardTris1VtxOffset[] = {
        { -300.0f, -2500.0f, 0.0f },
        { -300.0f, 1200.0f, 2700.0f },
        { 3000.0f, 1200.0f, 0.0f },
    };
    Vec3f hardTris2VtxOffset[] = {
        { -600.0f, 1200.0f, -2800.0f },
        { -600.0f, 1200.0f, 2800.0f },
        { 3000.0f, 1200.0f, 0.0f },
    };
    Vec3f mouthOffset = { 1800.0f, 1200.0f, 0.0f };
    Vec3f headOffset = { 1500.0f, 300.0f, 0.0f };

    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->colliderBody);

    switch (limbIndex) {
        case 2:
            if ((this->actionState == DODONGO_BREATHE_FIRE) && (29.0f < this->skelAnime.curFrame) &&
                (this->skelAnime.curFrame < 43.0f)) {
                en_da_attack_proc(this, play);
            }
            break;
        case 7:
            for (i = 0; i < 3; i++) {
                Matrix_Position(&hardTris0VtxOffset[i], &hardTris0Vtx[i]);
                Matrix_Position(&hardTris1VtxOffset[i], &hardTris1Vtx[i]);
                Matrix_Position(&hardTris2VtxOffset[i], &hardTris2Vtx[i]);
            }
            CollisionCheck_Uty_setTrisPos(&this->colliderHard, 0, &hardTris0Vtx[0], &hardTris0Vtx[1], &hardTris0Vtx[2]);
            CollisionCheck_Uty_setTrisPos(&this->colliderHard, 1, &hardTris1Vtx[0], &hardTris1Vtx[1], &hardTris1Vtx[2]);
            CollisionCheck_Uty_setTrisPos(&this->colliderHard, 2, &hardTris2Vtx[0], &hardTris2Vtx[1], &hardTris2Vtx[2]);
            Matrix_Position(&mouthOffset, &this->mouthPos);
            Matrix_Position(&headOffset, &this->headPos);
            break;
        case 15:
            if ((this->actionState == DODONGO_SWEEP_TAIL) && (this->timer >= 2)) {
                Matrix_Position(&tailTipOffset, &tailTip);
                Matrix_Position(&baseOffset, &tailBase);
                EffectBlure_edge_add(Effect_GetEffectMemoryPointer(this->blureIdx), &tailTip, &tailBase);
            } else if ((this->actionState == DODONGO_SWEEP_TAIL) && (this->timer != 0)) {
                EffectBlure_space_add(Effect_GetEffectMemoryPointer(this->blureIdx));
            }
            break;
        case 21:
            Matrix_Position(&local_eye[1], &this->leftFootPos);
            break;
        case 28:
            Matrix_Position(&local_eye[1], &this->rightFootPos);
            break;
    }
    if (this->iceTimer != 0) {
        i = -1;
        switch (limbIndex) {
            case 7:
                baseOffset.x = 1200.0f;
                i = 0;
                break;
            case 13:
                i = 1;
                break;
            case 14:
                i = 2;
                break;
            case 15:
                i = 3;
                break;
            case 16:
                i = 4;
                break;
            case 22:
                i = 5;
                break;
            case 29:
                i = 6;
                break;
            case 21:
                i = 7;
                break;
            case 28:
                i = 8;
                break;
        }
        if (i >= 0) {
            Matrix_Position(&baseOffset, &this->icePos[i]);
        }
    }
}

void En_Dodongo_actor_draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnDodongo* this = (EnDodongo*)thisx;
    s32 index;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_dodongo_display1,
                      en_dodongo_display2, this);

    if (this->iceTimer != 0) {
        this->actor.colorFilterTimer++;
        if (1) {}
        this->iceTimer--;
        if ((this->iceTimer % 4) == 0) {
            index = this->iceTimer >> 2;
            Effect_En_Ice_ct0(play, &this->actor, &this->icePos[index], 150, 150, 150, 250, 235, 245, 255,
                                           1.8f);
        }
    }
}

void XZ_offset_add(s16 yaw, f32 radius, Vec3f* vec) {
    vec->x += sin_s(yaw) * radius;
    vec->z += cos_s(yaw) * radius;
}

static s32 MouseVsBombCheck(EnDodongo* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVE].head;
    f32 dx;
    f32 dy;
    f32 dz;

    while (actor != NULL) {
        if ((actor->params != 0) || (actor->parent != NULL)) {
            actor = actor->next;
            continue;
        }
        dx = actor->world.pos.x - this->mouthPos.x;
        dy = actor->world.pos.y - this->mouthPos.y;
        dz = actor->world.pos.z - this->mouthPos.z;
        if ((fabsf(dx) < 20.0f) && (fabsf(dy) < 10.0f) && (fabsf(dz) < 20.0f)) {
            if (actor->id == ACTOR_EN_BOM) {
                this->actor.child = actor;
            } else {
                this->actor.parent = actor;
            }
            actor->parent = &this->actor;
            return true;
        }
        actor = actor->next;
    }
    return false;
}
