#include "z_en_dekubaba.h"
#include "assets/objects/object_dekubaba/object_dekubaba.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void En_Dekubaba_actor_ct(Actor* thisx, PlayState* play);
void En_Dekubaba_actor_dt(Actor* thisx, PlayState* play);
void En_Dekubaba_actor_move(Actor* thisx, PlayState* play);
void En_Dekubaba_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait_init(EnDekubaba* this);
void mode_tobidasu_init(EnDekubaba* this);
static void mode_wait(EnDekubaba* this, PlayState* play);
void mode_tobidasu(EnDekubaba* this, PlayState* play);
void mode_r_tobidasu(EnDekubaba* this, PlayState* play);
static void mode_search(EnDekubaba* this, PlayState* play);
static void mode_attack(EnDekubaba* this, PlayState* play);
void mode_attack_ready(EnDekubaba* this, PlayState* play);
static void mode_escape(EnDekubaba* this, PlayState* play);
static void mode_reverse(EnDekubaba* this, PlayState* play);
static void mode_damage(EnDekubaba* this, PlayState* play);
static void mode_head_up(EnDekubaba* this, PlayState* play);
void mode_real_damage(EnDekubaba* this, PlayState* play);
static void mode_down(EnDekubaba* this, PlayState* play);
void mode_fail_down(EnDekubaba* this, PlayState* play);
static void mode_deku_stick(EnDekubaba* this, PlayState* play);

static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };

ActorProfile En_Dekubaba_Profile = {
    /**/ ACTOR_EN_DEKUBABA,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DEKUBABA,
    /**/ sizeof(EnDekubaba),
    /**/ En_Dekubaba_actor_ct,
    /**/ En_Dekubaba_actor_dt,
    /**/ En_Dekubaba_actor_move,
    /**/ En_Dekubaba_actor_draw,
};

static ColliderJntSphElementInit DekubabaAllJntSphElemData[7] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 100, 1000 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 51, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 52, { { 0, 0, 500 }, 8 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 53, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 54, { { 0, 0, 500 }, 8 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 55, { { 0, 0, 1500 }, 8 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xFFCFFFFF, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 56, { { 0, 0, 500 }, 8 }, 100 },
    },
};

static ColliderJntSphInit DekubabaAllJntSphData = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    7,
    DekubabaAllJntSphElemData,
};

static CollisionCheckInfoInit DekubabaStatusData = { 2, 25, 25, MASS_IMMOVABLE };

typedef enum DekuBabaDamageEffect {
    /* 0x0 */ DEKUBABA_DMGEFF_NONE,
    /* 0x1 */ DEKUBABA_DMGEFF_DEKUNUT,
    /* 0x2 */ DEKUBABA_DMGEFF_FIRE,
    /* 0xE */ DEKUBABA_DMGEFF_BOOMERANG = 14,
    /* 0xF */ DEKUBABA_DMGEFF_SWORD
} DekuBabaDamageEffect;

static DamageTable DekubabaBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, DEKUBABA_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_BOOMERANG),
    /* Normal arrow  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Kokiri sword  */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Master sword  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant's Knife */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Fire arrow    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Light arrow   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Light magic   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Giant spin    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Master spin   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Kokiri jump   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant jump    */ DMG_ENTRY(8, DEKUBABA_DMGEFF_SWORD),
    /* Master jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Unknown 1     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
};

// The only difference is that for Big Deku Babas, Hookshot will act the same as Deku Nuts: i.e. it will stun, but
// cannot kill.
static DamageTable BigDekubabaBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, DEKUBABA_DMGEFF_DEKUNUT),
    /* Deku stick    */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Slingshot     */ DMG_ENTRY(1, DEKUBABA_DMGEFF_NONE),
    /* Explosive     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Boomerang     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_BOOMERANG),
    /* Normal arrow  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hammer swing  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Hookshot      */ DMG_ENTRY(0, DEKUBABA_DMGEFF_DEKUNUT),
    /* Kokiri sword  */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Master sword  */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant's Knife */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Fire arrow    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Light arrow   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 1   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Light magic   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Shield        */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, DEKUBABA_DMGEFF_SWORD),
    /* Giant spin    */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Master spin   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Kokiri jump   */ DMG_ENTRY(2, DEKUBABA_DMGEFF_SWORD),
    /* Giant jump    */ DMG_ENTRY(8, DEKUBABA_DMGEFF_SWORD),
    /* Master jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_SWORD),
    /* Unknown 1     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, DEKUBABA_DMGEFF_NONE),
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 1500, ICHAIN_STOP),
};

void En_Dekubaba_actor_ct(Actor* thisx, PlayState* play) {
    EnDekubaba* this = (EnDekubaba*)thisx;
    s32 i;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 22.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gDekuBabaSkel, &gDekuBabaFastChompAnim, this->jointTable, this->morphTable,
                   8);
    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->actor, &DekubabaAllJntSphData, this->colliderElements);

    if (this->actor.params == DEKUBABA_BIG) {
        this->size = 2.5f;

        for (i = 0; i < DekubabaAllJntSphData.count; i++) {
            this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius =
                (DekubabaAllJntSphElemData[i].dim.modelSphere.radius * 2.50f);
        }

        // This and its counterpart below mean that a Deku Stick jumpslash will not trigger the Deku Stick drop route.
        // (Of course they reckoned without each age being able to use the other's items, so Stick and Master Sword
        // jumpslash can give the Stick drop as adult, and neither will as child.)
        if (!LINK_IS_ADULT) {
            BigDekubabaBtlData.table[0x1B] = DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE); // DMG_JUMP_MASTER
        }

        CollisionCheck_Status_set2(&this->actor.colChkInfo, &BigDekubabaBtlData, &DekubabaStatusData);
        this->actor.colChkInfo.health = 4;
        this->actor.naviEnemyId = NAVI_ENEMY_BIG_DEKU_BABA;
        this->actor.attentionRangeType = ATTENTION_RANGE_2;
    } else {
        this->size = 1.0f;

        for (i = 0; i < DekubabaAllJntSphData.count; i++) {
            this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius;
        }

        if (!LINK_IS_ADULT) {
            DekubabaBtlData.table[0x1B] = DMG_ENTRY(4, DEKUBABA_DMGEFF_NONE); // DMG_JUMP_MASTER
        }

        CollisionCheck_Status_set2(&this->actor.colChkInfo, &DekubabaBtlData, &DekubabaStatusData);
        this->actor.naviEnemyId = NAVI_ENEMY_DEKU_BABA;
        this->actor.attentionRangeType = ATTENTION_RANGE_1;
    }

    mode_wait_init(this);
    this->timer = 0;
    this->boundFloor = NULL;
    this->bodyPartsPos[3] = this->actor.home.pos;
}

void En_Dekubaba_actor_dt(Actor* thisx, PlayState* play) {
    EnDekubaba* this = (EnDekubaba*)thisx;

    ClObjJntSph_dt_nzf(play, &this->collider);
}

void miki_change_ac_flg(EnDekubaba* this) {
    s32 i;

    for (i = 1; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].base.acElemFlags &= ~ACELEM_ON;
    }
}

static void mode_wait_init(EnDekubaba* this) {
    s32 i;
    ColliderJntSphElement* element;

    this->actor.shape.rot.x = -0x4000;
    this->stemSectionAngle[0] = this->stemSectionAngle[1] = this->stemSectionAngle[2] = this->actor.shape.rot.x;

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actor.world.pos.y = this->actor.home.pos.y + 14.0f * this->size;

    Actor_set_scale(&this->actor, this->size * 0.01f * 0.5f);

    this->collider.base.colMaterial = COL_MATERIAL_HARD;
    this->collider.base.acFlags |= AC_HARD;
    this->timer = 45;

    for (i = 1; i < ARRAY_COUNT(this->colliderElements); i++) {
        element = &this->collider.elements[i];
        element->dim.worldSphere.center.x = this->actor.world.pos.x;
        element->dim.worldSphere.center.y = (s16)this->actor.world.pos.y - 7;
        element->dim.worldSphere.center.z = this->actor.world.pos.z;
    }

    this->actionFunc = mode_wait;
}

void mode_tobidasu_init(EnDekubaba* this) {
    s32 i;

    Skeleton_Info2_init(&this->skelAnime, &gDekuBabaFastChompAnim,
                     Si2_anime_end_frame(&gDekuBabaFastChompAnim) * (1.0f / 15), 0.0f,
                     Si2_anime_end_frame(&gDekuBabaFastChompAnim), ANIMMODE_ONCE, 0.0f);

    this->timer = 15;

    for (i = 2; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].base.ocElemFlags |= OCELEM_ON;
    }

    this->collider.base.colMaterial = COL_MATERIAL_HIT6;
    this->collider.base.acFlags &= ~AC_HARD;
    Actor_SE_set(&this->actor, NA_SE_EN_DUMMY482);
    this->actionFunc = mode_tobidasu;
}

void mode_r_tobidasu_init(EnDekubaba* this) {
    s32 i;

    Skeleton_Info2_init(&this->skelAnime, &gDekuBabaFastChompAnim, -1.5f, Si2_anime_end_frame(&gDekuBabaFastChompAnim),
                     0.0f, ANIMMODE_ONCE, -3.0f);

    this->timer = 15;

    for (i = 2; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].base.ocElemFlags &= ~OCELEM_ON;
    }

    this->actionFunc = mode_r_tobidasu;
}

static void mode_search_init(EnDekubaba* this) {
    this->timer = Si2_anime_end_frame(&gDekuBabaFastChompAnim) * 2;
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gDekuBabaFastChompAnim, -3.0f);
    this->actionFunc = mode_search;
}

void mode_attack_ready_init(EnDekubaba* this) {
    this->timer = 8;
    this->skelAnime.playSpeed = 0.0f;
    this->actionFunc = mode_attack_ready;
}

static void mode_attack_init(EnDekubaba* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gDekuBabaPauseChompAnim);
    this->timer = 0;
    this->actionFunc = mode_attack;
}

static void mode_escape_init(EnDekubaba* this) {
    Skeleton_Info2_init(&this->skelAnime, &gDekuBabaPauseChompAnim, 1.0f, 15.0f,
                     Si2_anime_end_frame(&gDekuBabaPauseChompAnim), ANIMMODE_ONCE, -3.0f);
    this->timer = 0;
    this->actionFunc = mode_escape;
}

static void mode_reverse_init(EnDekubaba* this) {
    this->timer = 9;
    this->collider.base.acFlags |= AC_ON;
    this->skelAnime.playSpeed = -1.0f;
    this->actionFunc = mode_reverse;
}

static void mode_damage_init(EnDekubaba* this, s32 arg1) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gDekuBabaPauseChompAnim, -5.0f);
    this->timer = arg1;
    this->collider.base.acFlags &= ~AC_ON;
    Actor_set_scale(&this->actor, this->size * 0.01f);

    if (arg1 == 2) {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 155, COLORFILTER_BUFFLAG_OPA, 62);
    } else {
        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 42);
    }

    this->actionFunc = mode_damage;
}

static void mode_down_init(EnDekubaba* this) {
    this->timer = 0;
    this->skelAnime.playSpeed = 0.0f;
    this->actor.gravity = -0.8f;
    this->actor.velocity.y = 4.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.speed = this->size * 3.0f;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->actionFunc = mode_down;
}

void mode_fail_down_init(EnDekubaba* this) {
    Skeleton_Info2_init(&this->skelAnime, &gDekuBabaFastChompAnim, -1.5f, Si2_anime_end_frame(&gDekuBabaFastChompAnim),
                     0.0f, ANIMMODE_ONCE, -3.0f);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_fail_down;
}

static void mode_head_up_init(EnDekubaba* this) {
    s32 i;

    for (i = 1; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].base.acElemFlags |= ACELEM_ON;
    }

    if (this->timer == 1) {
        Skeleton_Info2_init(&this->skelAnime, &gDekuBabaFastChompAnim, 4.0f, 0.0f,
                         Si2_anime_end_frame(&gDekuBabaFastChompAnim), ANIMMODE_LOOP, -3.0f);
        this->timer = 40;
    } else {
        Skeleton_Info2_init(&this->skelAnime, &gDekuBabaFastChompAnim, 0.0f, 0.0f,
                         Si2_anime_end_frame(&gDekuBabaFastChompAnim), ANIMMODE_LOOP, -3.0f);
        this->timer = 60;
    }

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.y = this->actor.home.pos.y + (60.0f * this->size);
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actionFunc = mode_head_up;
}

void mode_real_damage_init(EnDekubaba* this) {
    this->targetSwayAngle = -0x6000;
    this->stemSectionAngle[2] = -0x5000;
    this->stemSectionAngle[1] = -0x4800;

    miki_change_ac_flg(this);
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 35);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_real_damage;
}

static void mode_deku_stick_init(EnDekubaba* this, PlayState* play) {
    Actor_set_scale(&this->actor, 0.03f);
    this->actor.shape.rot.x -= 0x4000;
    this->actor.shape.yOffset = 1000.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.shape.shadowScale = 3.0f;
    Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_MISC);
    this->actor.flags &= ~ACTOR_FLAG_DRAW_CULLING_DISABLED;
    this->timer = 200;
    this->actionFunc = mode_deku_stick;
}

// Action functions

static void mode_wait(EnDekubaba* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    this->actor.world.pos.x = this->actor.home.pos.x;
    this->actor.world.pos.z = this->actor.home.pos.z;
    this->actor.world.pos.y = this->actor.home.pos.y + 14.0f * this->size;

    if ((this->timer == 0) && (this->actor.xzDistToPlayer < 200.0f * this->size) &&
        (fabsf(this->actor.yDistToPlayer) < 30.0f * this->size)) {
        mode_tobidasu_init(this);
    }
}

void mode_tobidasu(EnDekubaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 headDistHorizontal;
    f32 headDistVertical;
    f32 headShiftX;
    f32 headShiftZ;

    if (this->timer != 0) {
        this->timer--;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);

    this->actor.scale.x = this->actor.scale.y = this->actor.scale.z =
        this->size * 0.01f * (0.5f + (15 - this->timer) * 0.5f / 15.0f);
    chase_angle(&this->actor.shape.rot.x, 0x1800, 0x800);

    headDistVertical = sinf(CLAMP_MAX((15 - this->timer) * (1.0f / 15), 0.7f) * M_PI) * 32.0f + 14.0f;

    if (this->actor.shape.rot.x < -0x38E3) {
        headDistHorizontal = 0.0f;
    } else if (this->actor.shape.rot.x < -0x238E) {
        chase_angle(&this->stemSectionAngle[0], -0x5555, 0x38E);
        headDistHorizontal = cos_s(this->stemSectionAngle[0]) * 20.0f;
    } else if (this->actor.shape.rot.x < -0xE38) {
        chase_angle(&this->stemSectionAngle[0], -0xAAA, 0x38E);
        chase_angle(&this->stemSectionAngle[1], -0x5555, 0x38E);
        chase_angle(&this->stemSectionAngle[2], -0x5555, 0x222);

        headDistHorizontal = 20.0f * (cos_s(this->stemSectionAngle[0]) + cos_s(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-sin_s(this->stemSectionAngle[0]) - sin_s(this->stemSectionAngle[1]))) *
                                 cos_s(this->stemSectionAngle[2]) / -sin_s(this->stemSectionAngle[2]);
    } else {
        chase_angle(&this->stemSectionAngle[0], -0xAAA, 0x38E);
        chase_angle(&this->stemSectionAngle[1], -0x31C7, 0x222);
        chase_angle(&this->stemSectionAngle[2], -0x5555, 0x222);

        headDistHorizontal = 20.0f * (cos_s(this->stemSectionAngle[0]) + cos_s(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-sin_s(this->stemSectionAngle[0]) - sin_s(this->stemSectionAngle[1]))) *
                                 cos_s(this->stemSectionAngle[2]) / -sin_s(this->stemSectionAngle[2]);
    }

    if (this->timer < 10) {
        adds(&this->actor.shape.rot.y, search_position_angleY(&this->actor.home.pos, &player->actor.world.pos), 2,
                       0xE38);
        if (headShiftZ) {} // One way of fake-matching
    }

    this->actor.world.pos.y = this->actor.home.pos.y + (headDistVertical * this->size);
    headShiftX = headDistHorizontal * this->size * sin_s(this->actor.shape.rot.y);
    headShiftZ = headDistHorizontal * this->size * cos_s(this->actor.shape.rot.y);
    this->actor.world.pos.x = this->actor.home.pos.x + headShiftX;
    this->actor.world.pos.z = this->actor.home.pos.z + headShiftZ;

    Effect_Hahen_Kakusan_ct3(play, &this->actor.home.pos, this->size * 3.0f, 0, this->size * 12.0f, this->size * 5.0f,
                             1, HAHEN_OBJECT_DEFAULT, 10, NULL);

    if (this->timer == 0) {
        if (search_position_distanceXZ(&this->actor.home.pos, &player->actor.world.pos) < 240.0f * this->size) {
            mode_attack_ready_init(this);
        } else {
            mode_r_tobidasu_init(this);
        }
    }
}

void mode_r_tobidasu(EnDekubaba* this, PlayState* play) {
    f32 headDistHorizontal;
    f32 headDistVertical;
    f32 xShift;
    f32 zShift;

    if (this->timer != 0) {
        this->timer--;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);

    this->actor.scale.x = this->actor.scale.y = this->actor.scale.z =
        this->size * 0.01f * (0.5f + this->timer * (1.0f / 30));
    chase_angle(&this->actor.shape.rot.x, -0x4000, 0x300);

    headDistVertical = (sinf(CLAMP_MAX(this->timer * 0.033f, 0.7f) * M_PI) * 32.0f) + 14.0f;

    if (this->actor.shape.rot.x < -0x38E3) {
        headDistHorizontal = 0.0f;
    } else if (this->actor.shape.rot.x < -0x238E) {
        chase_angle(&this->stemSectionAngle[0], -0x4000, 0x555);
        headDistHorizontal = cos_s(this->stemSectionAngle[0]) * 20.0f;
    } else if (this->actor.shape.rot.x < -0xE38) {
        chase_angle(&this->stemSectionAngle[0], -0x5555, 0x555);
        chase_angle(&this->stemSectionAngle[1], -0x4000, 0x555);
        chase_angle(&this->stemSectionAngle[2], -0x4000, 0x333);

        headDistHorizontal = 20.0f * (cos_s(this->stemSectionAngle[0]) + cos_s(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-sin_s(this->stemSectionAngle[0]) - sin_s(this->stemSectionAngle[1]))) *
                                 cos_s(this->stemSectionAngle[2]) / -sin_s(this->stemSectionAngle[2]);
    } else {
        chase_angle(&this->stemSectionAngle[0], -0x5555, 0x555);
        chase_angle(&this->stemSectionAngle[1], -0x5555, 0x333);
        chase_angle(&this->stemSectionAngle[2], -0x4000, 0x333);

        headDistHorizontal = 20.0f * (cos_s(this->stemSectionAngle[0]) + cos_s(this->stemSectionAngle[1])) +
                             (headDistVertical -
                              20.0f * (-sin_s(this->stemSectionAngle[0]) - sin_s(this->stemSectionAngle[1]))) *
                                 cos_s(this->stemSectionAngle[2]) / -sin_s(this->stemSectionAngle[2]);
    }

    this->actor.world.pos.y = this->actor.home.pos.y + (headDistVertical * this->size);
    xShift = headDistHorizontal * this->size * sin_s(this->actor.shape.rot.y);
    zShift = headDistHorizontal * this->size * cos_s(this->actor.shape.rot.y);
    this->actor.world.pos.x = this->actor.home.pos.x + xShift;
    this->actor.world.pos.z = this->actor.home.pos.z + zShift;

    Effect_Hahen_Kakusan_ct3(play, &this->actor.home.pos, this->size * 3.0f, 0, this->size * 12.0f, this->size * 5.0f,
                             1, HAHEN_OBJECT_DEFAULT, 0xA, NULL);

    if (this->timer == 0) {
        mode_wait_init(this);
    }
}

void set_head_pos(EnDekubaba* this) {
    f32 horizontalHeadShift = (cos_s(this->stemSectionAngle[0]) + cos_s(this->stemSectionAngle[1]) +
                               cos_s(this->stemSectionAngle[2])) *
                              20.0f;

    this->actor.world.pos.x =
        this->actor.home.pos.x + sin_s(this->actor.shape.rot.y) * (horizontalHeadShift * this->size);
    this->actor.world.pos.y =
        this->actor.home.pos.y - (sin_s(this->stemSectionAngle[0]) + sin_s(this->stemSectionAngle[1]) +
                                  sin_s(this->stemSectionAngle[2])) *
                                     20.0f * this->size;
    this->actor.world.pos.z =
        this->actor.home.pos.z + cos_s(this->actor.shape.rot.y) * (horizontalHeadShift * this->size);
}

static void mode_search(EnDekubaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 12.0f)) {
        if (this->actor.params == DEKUBABA_BIG) {
            Actor_SE_set(&this->actor, NA_SE_EN_DEKU_MOUTH);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_DEKU_JR_MOUTH);
        }
    }

    if (this->timer != 0) {
        this->timer--;
    }

    adds(&this->actor.shape.rot.y, search_position_angleY(&this->actor.home.pos, &player->actor.world.pos), 2,
                   (this->timer % 5) * 0x222);

    if (this->timer < 10) {
        this->stemSectionAngle[0] += 0x16C;
        this->stemSectionAngle[1] += 0x16C;
        this->stemSectionAngle[2] += 0xB6;
        this->actor.shape.rot.x += 0x222;
    } else if (this->timer < 20) {
        this->stemSectionAngle[0] -= 0x16C;
        this->stemSectionAngle[1] += 0x111;
        this->actor.shape.rot.x += 0x16C;
    } else if (this->timer < 30) {
        this->stemSectionAngle[1] -= 0x111;
        this->actor.shape.rot.x -= 0xB6;
    } else {
        this->stemSectionAngle[1] -= 0xB6;
        this->stemSectionAngle[2] += 0xB6;
        this->actor.shape.rot.x -= 0x16C;
    }

    set_head_pos(this);

    if (240.0f * this->size < search_position_distanceXZ(&this->actor.home.pos, &player->actor.world.pos)) {
        mode_r_tobidasu_init(this);
    } else if ((this->timer == 0) || (this->actor.xzDistToPlayer < 80.0f * this->size)) {
        mode_attack_ready_init(this);
    }
}

static void mode_attack(EnDekubaba* this, PlayState* play) {
    static Color_RGBA8 prim = { 105, 255, 105, 255 };
    static Color_RGBA8 env = { 150, 250, 150, 0 };
    s32 allStepsDone;
    s16 curFrame10;
    Vec3f velocity;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer == 0) {
        if (Skeleton_Info_frame_check(&this->skelAnime, 1.0f)) {
            if (this->actor.params == DEKUBABA_BIG) {
                Actor_SE_set(&this->actor, NA_SE_EN_DEKU_ATTACK);
            } else {
                Actor_SE_set(&this->actor, NA_SE_EN_DEKU_JR_ATTACK);
            }
        }

        chase_angle(&this->actor.shape.rot.x, 0, 0x222);

        curFrame10 = this->skelAnime.curFrame * 10.0f;

        allStepsDone = true;
        allStepsDone &= chase_angle(&this->stemSectionAngle[0], -0xE38, curFrame10 + 0x38E);
        allStepsDone &= chase_angle(&this->stemSectionAngle[1], -0xE38, curFrame10 + 0x71C);
        allStepsDone &= chase_angle(&this->stemSectionAngle[2], -0xE38, curFrame10 + 0xE38);

        if (allStepsDone) {
            Skeleton_Info2_init_standard_speedset_repeat(&this->skelAnime, &gDekuBabaFastChompAnim, 4.0f);
            velocity.x = sin_s(this->actor.shape.rot.y) * 5.0f;
            velocity.y = 0.0f;
            velocity.z = cos_s(this->actor.shape.rot.y) * 5.0f;

            Effect_SS_Dust_sc_cl_ct(play, &this->actor.world.pos, &velocity, &zero_vec, &prim, &env, 1,
                          this->size * 100.0f);
            this->timer = 1;
            this->collider.base.acFlags |= AC_ON;
        }
    } else if (this->timer > 10) {
        mode_escape_init(this);
    } else {
        this->timer++;

        if ((this->timer >= 4) && !Actor_player_direction_check(&this->actor, 0x16C)) {
            adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xF, 0x71C);
        }

        if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 12.0f)) {
            if (this->actor.params == DEKUBABA_BIG) {
                Actor_SE_set(&this->actor, NA_SE_EN_DEKU_MOUTH);
            } else {
                Actor_SE_set(&this->actor, NA_SE_EN_DEKU_JR_MOUTH);
            }
        }
    }

    set_head_pos(this);
}

void mode_attack_ready(EnDekubaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer != 0) {
        this->timer--;
    }

    add_calc_short_angle2(&this->actor.shape.rot.x, 0x1800, 2, 0xE38, 0x71C);
    adds(&this->actor.shape.rot.y, search_position_angleY(&this->actor.home.pos, &player->actor.world.pos), 2, 0xE38);
    chase_angle(&this->stemSectionAngle[0], 0xAAA, 0x444);
    chase_angle(&this->stemSectionAngle[1], -0x4718, 0x888);
    chase_angle(&this->stemSectionAngle[2], -0x6AA4, 0x888);

    if (this->timer == 0) {
        mode_attack_init(this);
    }

    set_head_pos(this);
}

static void mode_escape(EnDekubaba* this, PlayState* play) {
    Vec3f dustPos;
    f32 xIncr;
    f32 zIncr;
    s32 i;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer == 0) {
        chase_angle(&this->actor.shape.rot.x, -0x93E, 0x38E);
        chase_angle(&this->stemSectionAngle[0], -0x888, 0x16C);
        chase_angle(&this->stemSectionAngle[1], -0x888, 0x16C);
        if (chase_angle(&this->stemSectionAngle[2], -0x888, 0x16C)) {
            xIncr = sin_s(this->actor.shape.rot.y) * 30.0f * this->size;
            zIncr = cos_s(this->actor.shape.rot.y) * 30.0f * this->size;
            dustPos = this->actor.home.pos;

            for (i = 0; i < 3; i++) {
                Effect_SS_Dust_sc_li_ct(play, &dustPos, &zero_vec, &zero_vec, this->size * 500.0f, this->size * 50.0f);
                dustPos.x += xIncr;
                dustPos.z += zIncr;
            }

            this->timer = 1;
        }
    } else if (this->timer == 11) {
        chase_angle(&this->actor.shape.rot.x, -0x93E, 0x200);
        chase_angle(&this->stemSectionAngle[0], -0xAAA, 0x200);
        chase_angle(&this->stemSectionAngle[2], -0x5C71, 0x200);

        if (chase_angle(&this->stemSectionAngle[1], 0x238C, 0x200)) {
            this->timer = 12;
        }
    } else if (this->timer == 18) {
        chase_angle(&this->actor.shape.rot.x, 0x2AA8, 0xAAA);

        if (chase_angle(&this->stemSectionAngle[0], 0x1554, 0x5B0)) {
            this->timer = 25;
        }

        chase_angle(&this->stemSectionAngle[1], -0x38E3, 0xAAA);
        chase_angle(&this->stemSectionAngle[2], -0x5C71, 0x2D8);
    } else if (this->timer == 25) {
        chase_angle(&this->actor.shape.rot.x, -0x5550, 0xAAA);

        if (chase_angle(&this->stemSectionAngle[0], -0x6388, 0x93E)) {
            this->timer = 26;
        }

        chase_angle(&this->stemSectionAngle[1], -0x3FFC, 0x4FA);
        chase_angle(&this->stemSectionAngle[2], -0x238C, 0x444);
    } else if (this->timer == 26) {
        chase_angle(&this->actor.shape.rot.x, 0x1800, 0x93E);

        if (chase_angle(&this->stemSectionAngle[0], -0x1555, 0x71C)) {
            this->timer = 27;
        }

        chase_angle(&this->stemSectionAngle[1], -0x38E3, 0x2D8);
        chase_angle(&this->stemSectionAngle[2], -0x5C71, 0x5B0);
    } else if (this->timer >= 27) {
        this->timer++;

        if (this->timer > 30) {
            if (this->actor.xzDistToPlayer < 80.0f * this->size) {
                mode_attack_ready_init(this);
            } else {
                mode_search_init(this);
            }
        }
    } else {
        this->timer++;

        if (this->timer == 10) {
            Actor_SE_set(&this->actor, NA_SE_EN_DEKU_SCRAPE);
        }

        if (this->timer >= 12) {
            chase_angle(&this->stemSectionAngle[2], -0x5C71, 0x88);
        }
    }

    set_head_pos(this);
}

static void mode_reverse(EnDekubaba* this, PlayState* play) {
    s32 anyStepsDone;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer > 8) {
        anyStepsDone = add_calc_short_angle2(&this->actor.shape.rot.x, 0x1800, 1, 0x11C6, 0x71C);
        anyStepsDone |= add_calc_short_angle2(&this->stemSectionAngle[0], -0x1555, 1, 0xAAA, 0x71C);
        anyStepsDone |= add_calc_short_angle2(&this->stemSectionAngle[1], -0x38E3, 1, 0xE38, 0x71C);
        anyStepsDone |= add_calc_short_angle2(&this->stemSectionAngle[2], -0x5C71, 1, 0x11C6, 0x71C);

        if (!anyStepsDone) {
            this->timer = 8;
        }
    } else {
        if (this->timer != 0) {
            this->timer--;
        }

        if (this->timer == 0) {
            mode_search_init(this);
        }
    }

    set_head_pos(this);
}

/**
 * Hit by a weapon or hit something when lunging.
 */
static void mode_damage(EnDekubaba* this, PlayState* play) {
    s32 allStepsDone;

    Skeleton_Info2_anime_play(&this->skelAnime);

    allStepsDone = true;
    allStepsDone &= chase_angle(&this->actor.shape.rot.x, -0x4000, 0xE38);
    allStepsDone &= chase_angle(&this->stemSectionAngle[0], -0x4000, 0xE38);
    allStepsDone &= chase_angle(&this->stemSectionAngle[1], -0x4000, 0xE38);
    allStepsDone &= chase_angle(&this->stemSectionAngle[2], -0x4000, 0xE38);

    if (allStepsDone) {
        if (this->actor.colChkInfo.health == 0) {
            mode_fail_down_init(this);
        } else {
            this->collider.base.acFlags |= AC_ON;
            if (this->timer == 0) {
                if (this->actor.xzDistToPlayer < 80.0f * this->size) {
                    mode_attack_ready_init(this);
                } else {
                    mode_reverse_init(this);
                }
            } else {
                mode_head_up_init(this);
            }
        }
    }

    set_head_pos(this);
}

static void mode_head_up(EnDekubaba* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->timer != 0) {
        this->timer--;
    }

    if (this->timer == 0) {
        miki_change_ac_flg(this);

        if (this->actor.xzDistToPlayer < 80.0f * this->size) {
            mode_attack_ready_init(this);
        } else {
            mode_reverse_init(this);
        }
    }
}

/**
 * Sway back and forth with decaying amplitude until close enough to vertical.
 */
void mode_real_damage(EnDekubaba* this, PlayState* play) {
    s16 angleToVertical;

    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_angle(&this->actor.shape.rot.x, this->stemSectionAngle[0], 0x71C);
    chase_angle(&this->stemSectionAngle[0], this->stemSectionAngle[1], 0x71C);
    chase_angle(&this->stemSectionAngle[1], this->stemSectionAngle[2], 0x71C);

    if (chase_angle(&this->stemSectionAngle[2], this->targetSwayAngle, 0x71C)) {
        this->targetSwayAngle = -0x4000 - (this->targetSwayAngle + 0x4000) * 0.8f;
    }
    angleToVertical = this->targetSwayAngle + 0x4000;

    if (ABS(angleToVertical) < 0x100) {
        this->collider.base.acFlags |= AC_ON;
        if (this->actor.xzDistToPlayer < 80.0f * this->size) {
            mode_attack_ready_init(this);
        } else {
            mode_reverse_init(this);
        }
    }

    set_head_pos(this);
}

static void mode_down(EnDekubaba* this, PlayState* play) {
    s32 i;
    Vec3f dustPos;
    f32 deltaX;
    f32 deltaZ;
    f32 deltaY;

    chase_f(&this->actor.speed, 0.0f, this->size * 0.1f);

    if (this->timer == 0) {
        chase_angle(&this->actor.shape.rot.x, 0x4800, 0x71C);
        chase_angle(&this->stemSectionAngle[0], 0x4800, 0x71C);
        chase_angle(&this->stemSectionAngle[1], 0x4800, 0x71C);

        Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, this->size * 3.0f, 0, this->size * 12.0f,
                                 this->size * 5.0f, 1, HAHEN_OBJECT_DEFAULT, 10, NULL);

        if ((this->actor.scale.x > 0.005f) &&
            ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) || (this->actor.bgCheckFlags & BGCHECKFLAG_WALL))) {
            this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 0.0f;
            this->actor.speed = 0.0f;
            this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
            Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, this->size * 3.0f, 0, this->size * 12.0f,
                                     this->size * 5.0f, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
            this->timer = 1;
        }
    } else if (this->timer == 1) {
        dustPos = this->actor.world.pos;

        deltaY = 20.0f * sin_s(this->actor.shape.rot.x);
        deltaX = -20.0f * cos_s(this->actor.shape.rot.x) * sin_s(this->actor.shape.rot.y);
        deltaZ = -20.0f * cos_s(this->actor.shape.rot.x) * cos_s(this->actor.shape.rot.y);

        for (i = 0; i < 4; i++) {
            Effect_SS_Dust_sc_li_ct(play, &dustPos, &zero_vec, &zero_vec, 500, 50);
            dustPos.x += deltaX;
            dustPos.y += deltaY;
            dustPos.z += deltaZ;
        }

        Effect_SS_Dust_sc_li_ct(play, &this->actor.home.pos, &zero_vec, &zero_vec, this->size * 500.0f, this->size * 100.0f);
        mode_deku_stick_init(this, play);
    }
}

/**
 * Die and drop Deku Nuts (Stick drop is handled elsewhere)
 */
void mode_fail_down(EnDekubaba* this, PlayState* play) {
    chase_f(&this->actor.world.pos.y, this->actor.home.pos.y, this->size * 5.0f);

    if (chase_f(&this->actor.scale.x, this->size * 0.1f * 0.01f, this->size * 0.1f * 0.01f)) {
        Effect_SS_Dust_sc_li_ct(play, &this->actor.home.pos, &zero_vec, &zero_vec, this->size * 500.0f, this->size * 100.0f);
        if (this->actor.dropFlag == 0) {
            Item_set0(play, &this->actor.world.pos, ITEM00_NUTS);

            if (this->actor.params == DEKUBABA_BIG) {
                Item_set0(play, &this->actor.world.pos, ITEM00_NUTS);
                Item_set0(play, &this->actor.world.pos, ITEM00_NUTS);
            }
        } else {
            Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x30);
        }
        Actor_delete(&this->actor);
    }

    this->actor.scale.y = this->actor.scale.z = this->actor.scale.x;
    this->actor.shape.rot.z += 0x1C70;
    Effect_Hahen_Kakusan_ct3(play, &this->actor.home.pos, this->size * 3.0f, 0, this->size * 12.0f, this->size * 5.0f,
                             1, HAHEN_OBJECT_DEFAULT, 10, NULL);
}

static void mode_deku_stick(EnDekubaba* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }

    if (Actor_carry_check(&this->actor, play) || (this->timer == 0)) {
        Actor_delete(&this->actor);
        return;
    }

    Actor_carry_request_set(&this->actor, play, GI_DEKU_STICKS_1);
}

// Update and associated functions

void En_Dekubaba_damage_proc(EnDekubaba* this, PlayState* play) {
    Vec3f* firePos;
    f32 fireScale;
    s32 phi_s0; // Used for both health and iterator

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set_sph(&this->actor, &this->collider, true);

        if ((this->collider.base.colMaterial != COL_MATERIAL_HARD) &&
            ((this->actor.colChkInfo.damageEffect != DEKUBABA_DMGEFF_NONE) || (this->actor.colChkInfo.damage != 0))) {

            phi_s0 = this->actor.colChkInfo.health - this->actor.colChkInfo.damage;

            if (this->actionFunc != mode_head_up) {
                if ((this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_BOOMERANG) ||
                    (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_DEKUNUT)) {
                    if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_BOOMERANG) {
                        phi_s0 = this->actor.colChkInfo.health;
                    }

                    mode_damage_init(this, 2);
                } else if (this->actionFunc == mode_escape) {
                    if (phi_s0 <= 0) {
                        phi_s0 = 1;
                    }

                    mode_damage_init(this, 1);
                } else {
                    mode_damage_init(this, 0);
                }
            } else if ((this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_BOOMERANG) ||
                       (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_SWORD)) {
                if (phi_s0 > 0) {
                    mode_real_damage_init(this);
                } else {
                    mode_down_init(this);
                }
            } else if (this->actor.colChkInfo.damageEffect != DEKUBABA_DMGEFF_DEKUNUT) {
                mode_damage_init(this, 0);
            } else {
                return;
            }

            this->actor.colChkInfo.health = CLAMP_MIN(phi_s0, 0);

            if (this->actor.colChkInfo.damageEffect == DEKUBABA_DMGEFF_FIRE) {
                firePos = &this->actor.world.pos;
                fireScale = (this->size * 70.0f);

                for (phi_s0 = 0; phi_s0 < 4; phi_s0++) {
                    Effect_En_Fire_ct(play, &this->actor, firePos, fireScale, 0, 0, phi_s0);
                }
            }
        } else {
            return;
        }
    } else if ((play->actorCtx.unk_02 != 0) && (this->collider.base.colMaterial != COL_MATERIAL_HARD) &&
               (this->actionFunc != mode_head_up) && (this->actionFunc != mode_damage) &&
               (this->actor.colChkInfo.health != 0)) {
        this->actor.colChkInfo.health--;
        this->actor.dropFlag = 0x00;
        mode_damage_init(this, 1);
    } else {
        return;
    }

    if (this->actor.colChkInfo.health != 0) {
        if (this->timer == 2) {
            Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_DEKU_DAMAGE);
        }
    } else {
        Actor_info_finish(play, &this->actor);
        if (this->actor.params == DEKUBABA_BIG) {
            Actor_SE_set(&this->actor, NA_SE_EN_DEKU_DEAD);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_DEKU_JR_DEAD);
        }
    }
}

void En_Dekubaba_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDekubaba* this = (EnDekubaba*)thisx;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        mode_reverse_init(this);
    }

    En_Dekubaba_damage_proc(this, play);
    this->actionFunc(this, play);

    if (this->actionFunc == mode_down) {
        Actor_position_moveF(&this->actor);
        Actor_BGcheck2(play, &this->actor, 10.0f, this->size * 15.0f, 10.0f,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    } else if (this->actionFunc != mode_deku_stick) {
        Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
        if (this->boundFloor == NULL) {
            this->boundFloor = this->actor.floorPoly;
        }
    }
    if (this->actionFunc == mode_attack) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
    }

    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->actionFunc != mode_deku_stick) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

// Draw functions

void mode_wait_draw(EnDekubaba* this, PlayState* play) {
    f32 horizontalScale;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_dekubaba.c", 2445);

    horizontalScale = this->size * 0.01f;

    Matrix_translate(this->actor.home.pos.x, this->actor.home.pos.y + (-6.0f * this->size), this->actor.home.pos.z,
                     MTXMODE_NEW);
    Matrix_rotateXYZ(this->stemSectionAngle[0], this->actor.shape.rot.y, 0, MTXMODE_APPLY);
    Matrix_scale(horizontalScale, horizontalScale, horizontalScale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_dekubaba.c", 2461);
    gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStemTopDL);

    Actor_world_to_eye(&this->actor, 0.0f);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_dekubaba.c", 2468);
}

void mode_normal_draw(EnDekubaba* this, PlayState* play) {
    static Gfx* miki_model_list[] = { gDekuBabaStemTopDL, gDekuBabaStemMiddleDL, gDekuBabaStemBaseDL };
    MtxF mtx;
    s32 i;
    f32 horizontalStepSize;
    f32 spA4;
    f32 scale;
    s32 stemSections;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_dekubaba.c", 2487);

    if (this->actionFunc == mode_down) {
        stemSections = 2;
    } else {
        stemSections = 3;
    }

    scale = this->size * 0.01f;
    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
    Matrix_get(&mtx);
    if (this->actor.colorFilterTimer != 0) {
        spA4 = this->size * 20.0f;
        this->bodyPartsPos[2].x = this->actor.world.pos.x;
        this->bodyPartsPos[2].y = this->actor.world.pos.y - spA4;
        this->bodyPartsPos[2].z = this->actor.world.pos.z;
    }

    for (i = 0; i < stemSections; i++) {
        mtx.yw += 20.0f * sin_s(this->stemSectionAngle[i]) * this->size;
        horizontalStepSize = 20.0f * cos_s(this->stemSectionAngle[i]) * this->size;
        mtx.xw -= horizontalStepSize * sin_s(this->actor.shape.rot.y);
        mtx.zw -= horizontalStepSize * cos_s(this->actor.shape.rot.y);

        Matrix_put(&mtx);
        Matrix_rotateXYZ(this->stemSectionAngle[i], this->actor.shape.rot.y, 0, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_dekubaba.c", 2533);

        gSPDisplayList(POLY_OPA_DISP++, miki_model_list[i]);

        CollisionCheck_Uty_convJntSphL2G(51 + 2 * i, &this->collider);
        CollisionCheck_Uty_convJntSphL2G(52 + 2 * i, &this->collider);

        if (i == 0) {
            if (this->actionFunc != mode_real_damage) {
                this->actor.focus.pos.x = mtx.xw;
                this->actor.focus.pos.y = mtx.yw;
                this->actor.focus.pos.z = mtx.zw;
            } else {
                this->actor.focus.pos.x = this->actor.home.pos.x;
                this->actor.focus.pos.y = this->actor.home.pos.y + (40.0f * this->size);
                this->actor.focus.pos.z = this->actor.home.pos.z;
            }
        }

        if ((i < 2) && (this->actor.colorFilterTimer != 0)) {
            // checking colorFilterTimer ensures that spA4 has been initialized earlier, so not a bug
            this->bodyPartsPos[i].x = mtx.xw;
            this->bodyPartsPos[i].y = mtx.yw - spA4;
            this->bodyPartsPos[i].z = mtx.zw;
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_dekubaba.c", 2569);
}

void mode_down_draw(EnDekubaba* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_en_dekubaba.c", 2579);

    Matrix_rotateXYZ(this->stemSectionAngle[2], this->actor.shape.rot.y, 0, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_dekubaba.c", 2586);
    gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStemBaseDL);

    CollisionCheck_Uty_convJntSphL2G(55, &this->collider);
    CollisionCheck_Uty_convJntSphL2G(56, &this->collider);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_dekubaba.c", 2596);
}

void dekubaba_draw_home_shadow(EnDekubaba* this, PlayState* play) {
    MtxF mtx;
    f32 horizontalScale;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_dekubaba.c", 2692);
    _texture_decal_shadow(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 255);

    T_Polygon_Ground_Matrix(this->boundFloor, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, &mtx);
    Matrix_mult(&mtx, MTXMODE_NEW);

    horizontalScale = this->size * 0.15f;
    Matrix_scale(horizontalScale, 1.0f, horizontalScale, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_dekubaba.c", 2710);
    gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_dekubaba.c", 2715);
}

void en_dekubaba_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnDekubaba* this = (EnDekubaba*)thisx;

    if (limbIndex == 1) {
        CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->collider);
    }
}

void En_Dekubaba_actor_draw(Actor* thisx, PlayState* play) {
    EnDekubaba* this = (EnDekubaba*)thisx;
    f32 scale;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_dekubaba.c", 2752);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    if (this->actionFunc != mode_deku_stick) {
        Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, en_dekubaba_display2,
                          this);

        if (this->actionFunc == mode_wait) {
            mode_wait_draw(this, play);
        } else {
            mode_normal_draw(this, play);
        }

        scale = this->size * 0.01f;
        Matrix_translate(this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, MTXMODE_NEW);
        Matrix_rotateY(BINANG_TO_RAD(this->actor.home.rot.y), MTXMODE_APPLY);
        Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_dekubaba.c", 2780);
        gSPDisplayList(POLY_OPA_DISP++, gDekuBabaBaseLeavesDL);

        if (this->actionFunc == mode_down) {
            mode_down_draw(this, play);
        }

        if (this->boundFloor != NULL) {
            dekubaba_draw_home_shadow(this, play);
        }

        // Display solid until 40 frames left, then blink until killed.
    } else if ((this->timer > 40) || ((this->timer % 2) != 0)) {
        Matrix_translate(0.0f, 0.0f, 200.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_dekubaba.c", 2797);
        gSPDisplayList(POLY_OPA_DISP++, gDekuBabaStickDropDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_dekubaba.c", 2804);
}
