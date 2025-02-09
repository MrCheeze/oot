/*
 * File: z_en_po_sisters.c
 * Overlay: ovl_En_Po_Sisters
 * Description: Forest Temple Four Poe Sisters
 */

#include "z_en_po_sisters.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_po_sisters/object_po_sisters.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_CAN_ATTACH_TO_ARROW)

void En_Po_Sisters_actor_ct(Actor* thisx, PlayState* play);
void En_Po_Sisters_actor_dt(Actor* thisx, PlayState* play);
void En_Po_Sisters_actor_move(Actor* thisx, PlayState* play);
void En_Po_Sisters_actor_draw(Actor* thisx, PlayState* play);

static void mode_event_wait_init(EnPoSisters* this, PlayState* play);
static void mode_wait(EnPoSisters* this, PlayState* play);
static void mode_fly(EnPoSisters* this, PlayState* play);
static void mode_chase(EnPoSisters* this, PlayState* play);
static void mode_ready(EnPoSisters* this, PlayState* play);
static void mode_attack(EnPoSisters* this, PlayState* play);
static void mode_after(EnPoSisters* this, PlayState* play);
static void mode_damage(EnPoSisters* this, PlayState* play);
static void mode_escape(EnPoSisters* this, PlayState* play);
static void mode_disappear(EnPoSisters* this, PlayState* play);
static void mode_appear(EnPoSisters* this, PlayState* play);
static void mode_down(EnPoSisters* this, PlayState* play);
static void mode_fire(EnPoSisters* this, PlayState* play);
void mode_meg_cry(EnPoSisters* this, PlayState* play);
void mode_meg_dummy(EnPoSisters* this, PlayState* play);
static void mode_start(EnPoSisters* this, PlayState* play);
void mode_meg_start(EnPoSisters* this, PlayState* play);
void mode_meg_bunsin(EnPoSisters* this, PlayState* play);
void mode_meg_rotate(EnPoSisters* this, PlayState* play);
void mode_jo_beth_start(EnPoSisters* this, PlayState* play);
void mode_jo_beth_wait(EnPoSisters* this, PlayState* play);
static void mode_event_wait(EnPoSisters* this, PlayState* play);
void mode_event_fire(EnPoSisters* this, PlayState* play);
void mode_event_appear(EnPoSisters* this, PlayState* play);
void mode_event_po_fire(EnPoSisters* this, PlayState* play);
void mode_event_escape(EnPoSisters* this, PlayState* play);

void mode_meg_cry_init(EnPoSisters* this, PlayState* play);
void mode_meg_dummy_init(EnPoSisters* this, PlayState* play);

static void mode_start_init(EnPoSisters* this);

static Color_RGBA8 fire_prim[4] = {
    { 255, 170, 255, 255 },
    { 255, 200, 0, 255 },
    { 0, 170, 255, 255 },
    { 170, 255, 0, 255 },
};

static Color_RGBA8 fire_env[4] = {
    { 100, 0, 255, 255 },
    { 255, 0, 0, 255 },
    { 0, 0, 255, 255 },
    { 0, 150, 0, 255 },
};

ActorProfile En_Po_Sisters_Profile = {
    /**/ ACTOR_EN_PO_SISTERS,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_PO_SISTERS,
    /**/ sizeof(EnPoSisters),
    /**/ En_Po_Sisters_actor_ct,
    /**/ En_Po_Sisters_actor_dt,
    /**/ En_Po_Sisters_actor_move,
    /**/ En_Po_Sisters_actor_draw,
};

static ColliderCylinderInit PoSistersAllPipeData = {
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
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0x4FC7FFEA, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 18, 60, 15, { 0, 0, 0 } },
};

static CollisionCheckInfoInit PoSistersStatusData = { 10, 25, 60, 40 };

static DamageTable PoSistersBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, 0xF),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(1, 0xE),
    /* Master sword  */ DMG_ENTRY(2, 0xE),
    /* Giant's Knife */ DMG_ENTRY(4, 0xE),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0xE),
    /* Giant spin    */ DMG_ENTRY(4, 0xE),
    /* Master spin   */ DMG_ENTRY(2, 0xE),
    /* Kokiri jump   */ DMG_ENTRY(2, 0xE),
    /* Giant jump    */ DMG_ENTRY(8, 0xE),
    /* Master jump   */ DMG_ENTRY(4, 0xE),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

static s32 work_num = 0;

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 7, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 6000, ICHAIN_STOP),
};

static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };

static s16 aim_angle_y[4] = { 0xB000, 0xD000, 0x5000, 0x3000 };

static Vec3s syokudai_pos[4] = {
    { -22, 337, -1704 },
    { -431, 879, -3410 },
    { 549, 879, -3410 },
    { 1717, 515, -1340 },
};

static Vec3f sound_dummy_pos = { 120.0f, 250.0f, -1420.0f };

static Gfx* body_model[4] = {
    gPoeSistersMegBodyDL,
    gPoeSistersJoelleBodyDL,
    gPoeSistersBethBodyDL,
    gPoeSistersAmyBodyDL,
};

static Gfx* face_model[4] = {
    gPoeSistersMegFaceDL,
    gPoeSistersJoelleFaceDL,
    gPoeSistersBethFaceDL,
    gPoSistersAmyFaceDL,
};

static Color_RGBA8 tail_color[4] = {
    { 80, 0, 100, 0 },
    { 80, 15, 0, 0 },
    { 0, 70, 50, 0 },
    { 70, 70, 0, 0 },
};

static Vec3f fire_pos = { 1000.0f, -1700.0f, 0.0f };

void En_Po_Sisters_actor_ct(Actor* thisx, PlayState* play) {
    EnPoSisters* this = (EnPoSisters*)thisx;
    s32 pad;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 50.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gPoeSistersSkel, &gPoeSistersSwayAnim, this->jointTable, this->morphTable,
                   12);
    this->unk_22E.r = 255;
    this->unk_22E.g = 255;
    this->unk_22E.b = 210;
    this->unk_22E.a = 255;
    this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
    Light_point2_ct(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z, 0,
                            0, 0, 0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &PoSistersAllPipeData);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, &PoSistersBtlData, &PoSistersStatusData);
    this->unk_194 = PARAMS_GET_U(thisx->params, 8, 2);
    this->actor.naviEnemyId = this->unk_194 + NAVI_ENEMY_POE_SISTER_MEG;
    if (1) {}
    this->unk_195 = PARAMS_GET_U(thisx->params, 10, 2);
    this->unk_196 = 32;
    this->unk_197 = 20;
    this->unk_198 = 1;
    this->unk_199 = 32;
    this->unk_294 = 110.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (PARAMS_GET_NOSHIFT(this->actor.params, 12, 1)) {
        mode_event_wait_init(this, play);
    } else if (this->unk_194 == 0) {
        if (this->unk_195 == 0) {
            this->collider.base.ocFlags1 = OC1_ON | OC1_TYPE_PLAYER;
            mode_meg_cry_init(this, play);
        } else {
            this->actor.flags &= ~(ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR | ACTOR_FLAG_CAN_ATTACH_TO_ARROW);
            this->collider.elem.elemMaterial = ELEM_MATERIAL_UNK4;
            this->collider.elem.acDmgInfo.dmgFlags |= DMG_DEKU_NUT;
            this->collider.base.ocFlags1 = OC1_NONE;
            mode_meg_dummy_init(this, NULL);
        }
    } else {
        mode_start_init(this);
    }
    this->actor.params &= 0x3F;
}

void En_Po_Sisters_actor_dt(Actor* thisx, PlayState* play) {
    EnPoSisters* this = (EnPoSisters*)thisx;

    Global_light_list_delete(play, &play->lightCtx, this->lightNode);
    if (this->unk_194 == 0 && this->unk_195 == 0) {
        Na_StopMiddleBossBgm();
    }
    ClObjPipe_dt(play, &this->collider);
}

static void set_fire_pos(EnPoSisters* this, s32 arg1, Vec3f* arg2) {
    f32 temp_f20 = SQ(arg1) * 0.1f;
    Vec3f* vec;
    s32 i;

    for (i = 0; i < this->unk_198; i++) {
        vec = &this->unk_234[i];
        vec->x = arg2->x + sin_s((s16)(this->actor.shape.rot.y + (this->unk_19A * 0x800) + i * 0x2000)) * temp_f20;
        vec->z = arg2->z + cos_s((s16)(this->actor.shape.rot.y + (this->unk_19A * 0x800) + i * 0x2000)) * temp_f20;
        vec->y = arg2->y + arg1;
    }
}

static void mode_wait_init(EnPoSisters* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeSistersSwayAnim, -3.0f);
    this->unk_19A = get_random_timer(2, 3);
    this->actionFunc = mode_wait;
    this->actor.speed = 0.0f;
}

static void mode_fly_init(EnPoSisters* this) {
    if (this->actionFunc != mode_chase) {
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    }
    this->unk_19A = get_random_timer(0xF, 3);
    this->unk_199 |= 7;
    this->actionFunc = mode_fly;
}

static void mode_chase_init(EnPoSisters* this) {
    this->actionFunc = mode_chase;
}

static void mode_ready_init(EnPoSisters* this) {
    if (this->unk_22E.a != 0) {
        this->collider.base.colMaterial = COL_MATERIAL_METAL;
        this->collider.base.acFlags |= AC_HARD;
    }
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeSistersAttackAnim, -5.0f);
    this->actor.speed = 0.0f;
    this->unk_19A = Si2_anime_end_frame(&gPoeSistersAttackAnim) * 3 + 3;
    this->unk_199 &= ~2;
    this->actionFunc = mode_ready;
}

static void mode_attack_init(EnPoSisters* this) {
    this->actor.speed = 5.0f;
    if (this->unk_194 == 0) {
        this->collider.base.colMaterial = COL_MATERIAL_METAL;
        this->collider.base.acFlags |= AC_HARD;
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeSistersAttackAnim, -5.0f);
    }
    this->unk_19A = 5;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->unk_199 |= 8;
    this->actionFunc = mode_attack;
}

static void mode_after_init(EnPoSisters* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
    if (this->unk_194 != 0) {
        this->collider.base.colMaterial = COL_MATERIAL_HIT3;
        this->collider.base.acFlags &= ~AC_HARD;
    }
    this->actionFunc = mode_after;
}

static void mode_damage_init(EnPoSisters* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPoeSistersDamagedAnim, -3.0f);
    if (this->collider.base.ac != NULL) {
        this->actor.world.rot.y = (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & (DMG_ARROW | DMG_SLINGSHOT))
                                      ? this->collider.base.ac->world.rot.y
                                      : Actor_search_actor_angleY(&this->actor, this->collider.base.ac) + 0x8000;
    }
    if (this->unk_194 != 0) {
        this->actor.speed = 10.0f;
    }
    this->unk_199 &= ~0xB;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 16);
    this->actionFunc = mode_damage;
}

static void mode_escape_init(EnPoSisters* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeSistersFleeAnim, -3.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
    this->unk_19A = 5;
    this->unk_199 |= 0xB;
    this->actor.speed = 5.0f;
    this->actionFunc = mode_escape;
}

static void mode_disappear_init(EnPoSisters* this) {
    Skeleton_Info2_init(&this->skelAnime, &gPoeSistersAppearDisappearAnim, 1.5f, 0.0f,
                     Si2_anime_end_frame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE, -3.0f);
    this->actor.speed = 0.0f;
    this->unk_19C = 100;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->unk_199 &= ~5;
    Actor_SE_set(&this->actor, NA_SE_EN_PO_DISAPPEAR);
    Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH2);
    this->actionFunc = mode_disappear;
}

void set_po_meg_position(EnPoSisters* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 sp20;

    if (this->unk_195 == 0 || this->actionFunc != mode_damage) {
        if ((player->meleeWeaponState == 0 || player->meleeWeaponAnimation >= PLAYER_MWA_SPIN_ATTACK_1H) &&
            player->actor.world.pos.y - player->actor.floorHeight < 1.0f) {
            chase_f(&this->unk_294, 110.0f, 3.0f);
        } else {
            chase_f(&this->unk_294, 170.0f, 10.0f);
        }
        sp20 = this->unk_294;
    } else if (this->unk_195 != 0) {
        sp20 = this->actor.parent->xzDistToPlayer;
    }
    this->actor.world.pos.x = (sin_s(this->actor.shape.rot.y + 0x8000) * sp20) + player->actor.world.pos.x;
    this->actor.world.pos.z = (cos_s(this->actor.shape.rot.y + 0x8000) * sp20) + player->actor.world.pos.z;
}

static void mode_appear_init(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gPoeSistersAppearDisappearAnim, 1.5f, 0.0f,
                     Si2_anime_end_frame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE, -3.0f);
    if (this->unk_194 == 0) {
        this->unk_294 = 110.0f;
        set_po_meg_position(this, play);
        this->unk_22E.a = 0;
        this->actor.draw = En_Po_Sisters_actor_draw;
    } else {
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
    this->unk_19A = 15;
    this->actor.speed = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_PO_APPEAR);
    this->unk_199 &= ~1;
    this->actionFunc = mode_appear;
}

static void mode_down_init(EnPoSisters* this, PlayState* play) {
    this->unk_19A = 0;
    this->actor.speed = 0.0f;
    this->actor.world.pos.y += 42.0f;
    this->actor.shape.yOffset = -6000.0f;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_199 = 0;
    this->actionFunc = mode_down;
    makeOnepointDemo(play, 3190, 999, &this->actor, CAM_ID_MAIN);
}

static void mode_fire_init(EnPoSisters* this, PlayState* play) {
    this->unk_19A = 0;
    this->actor.world.pos.y = this->unk_234[0].y;
    Item_Set_Std(play, &this->actor, &this->actor.world.pos, 0x80);
    this->actionFunc = mode_fire;
}

// Meg spawning fakes
void mode_meg_cry_init(EnPoSisters* this, PlayState* play) {
    Actor* actor1 = Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_PO_SISTERS, this->actor.world.pos.x,
                                this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0x400);
    Actor* actor2 = Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_PO_SISTERS, this->actor.world.pos.x,
                                this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0x800);
    Actor* actor3 = Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_PO_SISTERS, this->actor.world.pos.x,
                                this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0xC00);
    s32 pad;
    s32 pad1;

    if (actor1 == NULL || actor2 == NULL || actor3 == NULL) {
        if (actor1 != NULL) {
            Actor_delete(actor1);
        }
        if (actor2 != NULL) {
            Actor_delete(actor2);
        }
        if (actor3 != NULL) {
            Actor_delete(actor3);
        }
        Actor_delete(&this->actor);
    } else {
        actor3->parent = &this->actor;
        actor2->parent = &this->actor;
        actor1->parent = &this->actor;
        Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gPoeSistersMegCryAnim);
        this->unk_198 = 0;
        this->unk_199 = 160;
        this->actionFunc = mode_meg_cry;
    }
}

void mode_meg_dummy_init(EnPoSisters* this, PlayState* play) {
    Vec3f vec;

    this->actor.draw = NULL;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_19C = 100;
    this->unk_199 = 32;
    this->collider.base.colMaterial = COL_MATERIAL_HIT3;
    this->collider.base.acFlags &= ~AC_HARD;
    if (play != NULL) {
        vec.x = this->actor.world.pos.x;
        vec.y = this->actor.world.pos.y + 45.0f;
        vec.z = this->actor.world.pos.z;
        _Effect_SS_Db_ct(play, &vec, &zero_vec, &zero_vec, 150, 0, 255, 255, 255, 155, 150, 150, 150, 1, 9,
                             0);
    }
    Light_point_color_set(&this->lightInfo, 0, 0, 0, 0);
    this->actionFunc = mode_meg_dummy;
}

static void mode_start_init(EnPoSisters* this) {
    if (this->unk_194 == 3) {
        Skeleton_Info2_init_standard_stop(&this->skelAnime, &gPoeSistersAppearDisappearAnim);
        Actor_SE_set(&this->actor, NA_SE_EN_PO_APPEAR);
    } else {
        Skeleton_Info2_init(&this->skelAnime, &gPoeSistersAppearDisappearAnim, 0.5f, 0.0f,
                         Si2_anime_end_frame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE_INTERP, 0.0f);
    }
    this->unk_22E.a = 0;
    this->unk_199 = 32;
    this->actionFunc = mode_start;
}

void mode_meg_start_init(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPoeSistersAppearDisappearAnim, -5.0f);
    this->unk_198 = 1;
    this->unk_199 &= ~0x80;
    this->actionFunc = mode_meg_start;
    makeOnepointDemo(play, 3180, 156, &this->actor, CAM_ID_MAIN);
}

void mode_meg_bunsin_init(EnPoSisters* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    this->unk_19A = Si2_anime_end_frame(&gPoeSistersFloatAnim) * 7 + 7;
    if (this->actor.parent != NULL) {
        this->actor.world.pos = this->actor.parent->world.pos;
        this->actor.shape.rot.y = this->actor.parent->shape.rot.y;
    } else {
        this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->unk_19A++;
    }
    if (this->unk_195 == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH2);
    }
    this->actionFunc = mode_meg_bunsin;
}

void mode_meg_rotate_init(EnPoSisters* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    this->unk_22E.a = 255;
    this->unk_19A = 300;
    this->unk_19C = 3;
    this->unk_199 |= 9;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = mode_meg_rotate;
}

void mode_jo_beth_start_init(EnPoSisters* this) {
    if (this->unk_194 == 1) {
        this->actor.home.pos.x = -632.0f;
        this->actor.home.pos.z = -3440.0f;
    } else {
        this->actor.home.pos.x = 752.0f;
        this->actor.home.pos.z = -3440.0f;
    }
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gPoeSistersFloatAnim);
    this->unk_199 |= 0xA;
    this->actionFunc = mode_jo_beth_start;
    this->actor.speed = 5.0f;
}

void mode_jo_beth_wait_init(EnPoSisters* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeSistersSwayAnim, -3.0f);
    this->unk_22E.a = 255;
    this->unk_199 |= 0x15;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = mode_jo_beth_wait;
    this->actor.speed = 0.0f;
}

static void mode_event_wait_init(EnPoSisters* this, PlayState* play) {
    work_num = 0;
    this->unk_22E.a = 0;
    this->unk_199 = 128;
    this->unk_19A = 50;
    this->unk_234[0] = this->actor.home.pos;
    Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
    this->actionFunc = mode_event_wait;
}

void mode_event_fire_init(EnPoSisters* this) {
    s32 i;

    this->unk_198 = ARRAY_COUNT(this->unk_234);
    for (i = 0; i < ARRAY_COUNT(this->unk_234); i++) {
        this->unk_234[i] = this->unk_234[0];
    }
    this->actionFunc = mode_event_fire;
}

void mode_event_appear_init(EnPoSisters* this) {
    Skeleton_Info2_init(&this->skelAnime, &gPoeSistersAppearDisappearAnim, 0.833f, 0.0f,
                     Si2_anime_end_frame(&gPoeSistersAppearDisappearAnim), ANIMMODE_ONCE_INTERP, 0.0f);
    if (this->unk_194 == 0 || this->unk_194 == 1) {
        this->unk_19A = 40;
    } else {
        this->unk_19A = 76;
    }
    this->unk_198 = 0;
    work_num = 0;
    this->actionFunc = mode_event_appear;
}

void mode_event_po_fire_init(EnPoSisters* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gPoeSistersSwayAnim);
    this->unk_198 = 8;
    this->unk_19A = 32;
    set_fire_pos(this, this->unk_19A, &this->actor.home.pos);
    this->actionFunc = mode_event_po_fire;
}

void mode_event_escape_init(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeSistersFloatAnim, -3.0f);
    this->unk_198 = 0;
    this->unk_199 = 40;
    this->unk_19A = 90;
    this->unk_196 = 32;
    this->actor.world.rot.y = aim_angle_y[this->unk_194];
    this->actor.home.pos.y = this->actor.world.pos.y;
    if (this->unk_194 == 0) {
        Actor_Environment_sw_On(play, 0x1B);
    }
    Actor_SE_set(&this->actor, NA_SE_EV_FLAME_IGNITION);
    this->actionFunc = mode_event_escape;
}

void set_po_sisters_position_y(EnPoSisters* this, PlayState* play) {
    f32 targetY;
    Player* player = GET_PLAYER(play);

    if (this->actionFunc == mode_event_escape) {
        targetY = this->actor.home.pos.y;
    } else if (this->unk_194 == 0 || this->unk_194 == 3) {
        targetY = player->actor.world.pos.y + 5.0f;
    } else {
        targetY = 832.0f;
    }
    add_calc2(&this->actor.world.pos.y, targetY, 0.5f, 3.0f);
    if (!this->unk_196) {
        this->unk_196 = 32;
    }
    if (this->unk_196 != 0) {
        this->unk_196--;
    }
    this->actor.world.pos.y += (2.0f + 0.5f * fqrand()) * sin_s(this->unk_196 * 0x800);
    if (this->unk_22E.a == 255 && this->actionFunc != mode_attack && this->actionFunc != mode_ready) {
        if (this->actionFunc == mode_escape) {
            Actor_level_SE_set(&this->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
        } else {
            Actor_level_SE_set(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
        }
    }
}

static void mode_wait(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && this->unk_19A != 0) {
        this->unk_19A--;
    }
    if (this->unk_19A == 0 || this->actor.xzDistToPlayer < 200.0f) {
        mode_fly_init(this);
    }
}

static void mode_fly(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_f(&this->actor.speed, 1.0f, 0.2f);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && this->unk_19A != 0) {
        this->unk_19A--;
    }
    if (this->actor.xzDistToPlayer < 200.0f && fabsf(this->actor.yDistToPlayer + 5.0f) < 30.0f) {
        mode_chase_init(this);
    } else if (this->unk_19A == 0 && chase_f(&this->actor.speed, 0.0f, 0.2f) != 0) {
        mode_wait_init(this);
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        chase_angle(&this->actor.world.rot.y, Actor_search_position_angleY(&this->actor, &this->actor.home.pos),
                           0x71C);
    } else if (Actor_search_position_distanceXZ(&this->actor, &this->actor.home.pos) > 300.0f) {
        chase_angle(&this->actor.world.rot.y, Actor_search_position_angleY(&this->actor, &this->actor.home.pos),
                           0x71C);
    }
}

static void mode_chase(EnPoSisters* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 temp_v0;

    Skeleton_Info2_anime_play(&this->skelAnime);
    temp_v0 = this->actor.yawTowardsPlayer - player->actor.shape.rot.y;
    chase_f(&this->actor.speed, 2.0f, 0.2f);
    if (temp_v0 > 0x3000) {
        chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x3000, 0x71C);
    } else if (temp_v0 < -0x3000) {
        chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer - 0x3000, 0x71C);
    } else {
        chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x71C);
    }
    if (this->actor.xzDistToPlayer < 160.0f && fabsf(this->actor.yDistToPlayer + 5.0f) < 30.0f) {
        mode_ready_init(this);
    } else if (this->actor.xzDistToPlayer > 240.0f) {
        mode_fly_init(this);
    }
}

static void mode_ready(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_19A != 0) {
        this->unk_19A--;
    }
    this->actor.shape.rot.y += 384.0f * ((this->skelAnime.endFrame + 1.0f) * 3.0f - this->unk_19A);
    if (this->unk_19A == 18 || this->unk_19A == 7) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_ROLL);
    }
    if (this->unk_19A == 0) {
        mode_attack_init(this);
    }
}

static void mode_attack(EnPoSisters* this, PlayState* play) {
    s32 pad;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && this->unk_19A != 0) {
        this->unk_19A--;
    }
    this->actor.shape.rot.y += (384.0f * this->skelAnime.endFrame) * 3.0f;
    if (this->unk_19A == 0 && ABS((s16)(this->actor.shape.rot.y - this->actor.world.rot.y)) < 0x1000) {
        if (this->unk_194 != 0) {
            this->collider.base.colMaterial = COL_MATERIAL_HIT3;
            this->collider.base.acFlags &= ~AC_HARD;
            mode_fly_init(this);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH2);
            mode_meg_dummy_init(this, play);
        }
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 1.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_ROLL);
    }
}

static void mode_after(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actor.shape.rot.y -= (this->actor.speed * 10.0f) * 128.0f;
    if (chase_f(&this->actor.speed, 0.0f, 0.1f) != 0) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->unk_194 != 0) {
            mode_fly_init(this);
        } else {
            Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH2);
            mode_meg_dummy_init(this, play);
        }
    }
}

static void mode_damage(EnPoSisters* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime) && !(this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW)) {
        if (this->actor.colChkInfo.health != 0) {
            if (this->unk_194 != 0) {
                mode_escape_init(this);
            } else if (this->unk_195 != 0) {
                mode_meg_dummy_init(this, NULL);
            } else {
                mode_meg_dummy_init(this, play);
            }
        } else {
            mode_down_init(this, play);
        }
    }
    if (this->unk_195 != 0) {
        chase_angle(&this->actor.shape.rot.y, this->actor.parent->shape.rot.y,
                           (this->unk_195 == 2) ? 0x800 : 0x400);
        this->unk_22E.a = ((this->skelAnime.endFrame - this->skelAnime.curFrame) * 255.0f) / this->skelAnime.endFrame;
        this->actor.world.pos.y = this->actor.parent->world.pos.y;
        set_po_meg_position(this, play);
    } else if (this->unk_194 != 0) {
        chase_f(&this->actor.speed, 0.0f, 0.5f);
    }
}

static void mode_escape(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x8000, 1820);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && this->unk_19A != 0) {
        this->unk_19A--;
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->unk_199 |= 2;
        mode_disappear_init(this);
    } else if (this->unk_19A == 0 && 240.0f < this->actor.xzDistToPlayer) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        mode_fly_init(this);
    }
}

static void mode_disappear(EnPoSisters* this, PlayState* play) {
    s32 endFrame;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->unk_22E.a = 0;
        this->collider.elem.acDmgInfo.dmgFlags = DMG_MAGIC_ICE | DMG_MAGIC_FIRE | DMG_DEKU_NUT;
        mode_fly_init(this);
    } else {
        endFrame = this->skelAnime.endFrame;
        this->unk_22E.a = ((endFrame - this->skelAnime.curFrame) * 255.0f) / endFrame;
    }
}

static void mode_appear(EnPoSisters* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->unk_22E.a = 255;
        if (this->unk_194 != 0) {
            this->unk_199 |= 1;
            this->collider.elem.acDmgInfo.dmgFlags = (DMG_SWORD | DMG_ARROW | DMG_HAMMER | DMG_MAGIC_ICE |
                                                      DMG_MAGIC_FIRE | DMG_HOOKSHOT | DMG_EXPLOSIVE | DMG_DEKU_STICK);
            if (this->unk_19A != 0) {
                this->unk_19A--;
            }
            if (this->unk_19A == 0) {
                this->unk_197 = 20;
                mode_fly_init(this);
            }
        } else {
            mode_meg_rotate_init(this);
        }
    } else {
        this->unk_22E.a = (this->skelAnime.curFrame * 255.0f) / this->skelAnime.endFrame;
        if (this->unk_194 == 0) {
            set_po_meg_position(this, play);
        }
    }
}

static void mode_down(EnPoSisters* this, PlayState* play) {
    s32 i;

    this->unk_19A++;
    this->unk_198 = CLAMP_MAX(this->unk_198 + 1, 8);
    for (i = this->unk_198 - 1; i > 0; i--) {
        this->unk_234[i] = this->unk_234[i - 1];
    }
    this->unk_234[0].x =
        (sin_s((this->actor.shape.rot.y + this->unk_19A * 0x3000) - 0x4000) * (3000.0f * this->actor.scale.x)) +
        this->actor.world.pos.x;
    this->unk_234[0].z =
        (cos_s((this->actor.shape.rot.y + this->unk_19A * 0x3000) - 0x4000) * (3000.0f * this->actor.scale.x)) +
        this->actor.world.pos.z;
    if (this->unk_19A < 8) {
        this->unk_234[0].y = this->unk_234[1].y - 9.0f;
    } else {
        this->unk_234[0].y = this->unk_234[1].y + 2.0f;
        if (this->unk_19A >= 16) {
            if (chase_f(&this->actor.scale.x, 0.0f, 0.001f) != 0) {
                mode_fire_init(this, play);
            }
            this->actor.scale.z = this->actor.scale.x;
            this->actor.scale.y = this->actor.scale.x;
        }
    }
    if (this->unk_19A == 16) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_DEAD2);
    }
}

static void mode_fire(EnPoSisters* this, PlayState* play) {
    this->unk_19A++;
    if (this->unk_19A == 64) {
        Actor_Environment_sw_On(play, this->actor.params);
        Effect_SE_Info_new(play, &this->actor.world.pos, 30, NA_SE_EV_FLAME_IGNITION);
        if (this->unk_194 == 0) {
            Actor_Environment_sw_Off(play, 0x1B);
        }
        play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        Actor_delete(&this->actor);
    } else if (this->unk_19A < 32) {
        set_fire_pos(this, this->unk_19A, &this->actor.world.pos);
    } else {
        set_fire_pos(this, 64 - this->unk_19A, &this->actor.world.pos);
    }
    if (this->unk_19A == 32) {
        this->actor.world.pos.x = syokudai_pos[this->unk_194].x;
        this->actor.world.pos.y = syokudai_pos[this->unk_194].y;
        this->actor.world.pos.z = syokudai_pos[this->unk_194].z;
    }
}

void mode_meg_cry(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->actor.xzDistToPlayer < 130.0f) {
        mode_meg_start_init(this, play);
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_CRY);
    }
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
}

void mode_meg_dummy(EnPoSisters* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnPoSisters* realMeg = (EnPoSisters*)this->actor.parent;

    if (this->unk_195 == 0) {
        if (Actor_search_position_distanceXZ(&player->actor, &this->actor.home.pos) < 600.0f) {
            if (this->unk_19C != 0) {
                this->unk_19C--;
            }
        } else {
            this->unk_19C = 100;
        }
        if (this->unk_19C == 0) {
            this->actor.shape.rot.y = (s32)(4.0f * fqrand()) * 0x4000 + this->actor.yawTowardsPlayer;
            this->actor.world.pos.y = player->actor.world.pos.y + 5.0f;
            mode_appear_init(this, play);
        }
    } else {
        if (realMeg->actionFunc == mode_meg_bunsin) {
            this->actor.draw = En_Po_Sisters_actor_draw;
            mode_meg_bunsin_init(this);
        } else if (realMeg->actionFunc == mode_appear) {
            this->actor.shape.rot.y = this->actor.parent->shape.rot.y + this->unk_195 * 0x4000;
            this->actor.world.pos.y = player->actor.world.pos.y + 5.0f;
            mode_appear_init(this, play);
        } else if (realMeg->actionFunc == mode_down) {
            Actor_delete(&this->actor);
        }
    }
}

void mode_meg_start(EnPoSisters* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_meg_bunsin_init(this);
    }
    set_po_meg_position(this, play);
    this->actor.world.pos.y += 1.0f;
    Actor_world_to_eye(&this->actor, 40.0f);
}

void mode_meg_bunsin(EnPoSisters* this, PlayState* play) {
    f32 temp_f2;
    s16 phi_v0;
    s16 phi_a2;
    u8 temp;

    Skeleton_Info2_anime_play(&this->skelAnime);
    temp_f2 = this->skelAnime.endFrame * 0.5f;
    this->unk_22E.a = (fabsf(temp_f2 - this->skelAnime.curFrame) * 255.0f) / temp_f2;
    if (this->unk_19A != 0) {
        this->unk_19A -= 1;
    }
    if (this->unk_19A == 0) {
        this->actor.world.rot.y = this->actor.shape.rot.y += 0x4000 * (s32)(fqrand() * 4.0f);
        if (this->unk_195 == 0) {
            Na_StartMiddleBossBgm(NA_BGM_MINI_BOSS);
        }
        mode_meg_rotate_init(this);
    } else {
        this->actor.world.pos.y += 0.1f;
        temp = this->unk_195;
        if (temp != 0) {
            if (this->unk_19A > 90) {
                phi_v0 = 1;
                phi_a2 = 64;
            } else if (this->unk_19A > 70) {
                phi_v0 = 0;
                phi_a2 = 64;
            } else if (this->unk_19A > 55) {
                phi_v0 = 1;
                phi_a2 = 96;
            } else if (this->unk_19A > 40) {
                phi_v0 = 0;
                phi_a2 = 96;
            } else {
                phi_v0 = 1;
                phi_a2 = 256;
            }
            if (this->unk_195 == 2) {
                phi_a2 *= 2;
            }
            chase_angle(&this->actor.shape.rot.y,
                               this->actor.parent->shape.rot.y + (this->unk_195 * 0x4000) * phi_v0, phi_a2);
        } else if (this->unk_19A == 70 || this->unk_19A == 40) {
            Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH2);
        }
    }
    set_po_meg_position(this, play);
    Actor_world_to_eye(&this->actor, 40.0f);
}

void mode_meg_rotate(EnPoSisters* this, PlayState* play) {
    s32 temp_v0;
    s32 phi_a0;

    if (this->unk_19A != 0) {
        this->unk_19A--;
    }
    if (this->unk_19C > 0) {
        if (this->unk_19A >= 16) {
            Skeleton_Info2_anime_play(&this->skelAnime);
            if (this->unk_195 == 0) {
                if (ABS((s16)(16 - this->unk_196)) < 14) {
                    this->actor.shape.rot.y +=
                        (0x580 - (this->unk_19C * 0x180)) * fabsf(sin_s(this->unk_196 * 0x800));
                }
                if (this->unk_19A >= 284 || this->unk_19A < 31) {
                    this->unk_199 |= 0x40;
                } else {
                    this->unk_199 &= ~0x40;
                }
            } else {
                this->actor.shape.rot.y = (s16)(this->actor.parent->shape.rot.y + (this->unk_195 * 0x4000));
            }
        }
    }
    if (this->unk_195 == 0) {
        if (this->unk_19A >= 284 || (this->unk_19A < 31 && this->unk_19A >= 16)) {
            this->unk_199 |= 0x40;
        } else {
            this->unk_199 &= ~0x40;
        }
    }
    if (Actor_search_position_distanceXZ(&GET_PLAYER(play)->actor, &this->actor.home.pos) > 600.0f) {
        this->unk_199 &= ~0x40;
        mode_meg_dummy_init(this, play);
    } else if (this->unk_19A == 0) {
        if (this->unk_195 == 0) {
            mode_attack_init(this);
        } else {
            mode_meg_dummy_init(this, play);
        }
    } else if (this->unk_195 != 0) {
        EnPoSisters* realMeg = (EnPoSisters*)this->actor.parent;

        if (realMeg->actionFunc == mode_damage) {
            mode_damage_init(this);
        }
    } else if (this->unk_19C == 0) {
        this->unk_19C = -15;
    } else if (this->unk_19C < 0) {
        this->unk_19C++;
        if (this->unk_19C == 0) {
            mode_attack_init(this);
        }
    }
    set_po_meg_position(this, play);
}

static void mode_start(EnPoSisters* this, PlayState* play) {
    f32 div;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->unk_22E.a = 255;
        if (this->unk_194 == 3) {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.home.pos.x = 1992.0f;
            this->actor.home.pos.z = -1440.0f;
            this->unk_199 |= 0x18;
            mode_fly_init(this);
        } else {
            mode_jo_beth_start_init(this);
        }
    } else {
        div = this->skelAnime.curFrame / this->skelAnime.endFrame;
        this->unk_22E.a = 255.0f * div;
    }
    if (this->unk_194 != 3 && Skeleton_Info_frame_check(&this->skelAnime, 1.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_APPEAR);
    }
    Actor_world_to_eye(&this->actor, 40.0f);
}

void mode_jo_beth_start(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Actor_search_position_distanceXZ(&this->actor, &this->actor.home.pos) < 10.0f) {
        mode_jo_beth_wait_init(this);
    } else {
        chase_angle(&this->actor.world.rot.y, Actor_search_position_angleY(&this->actor, &this->actor.home.pos),
                           1820);
    }
}

void mode_jo_beth_wait(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1820);
    if (this->actor.xzDistToPlayer < 240.0f && fabsf(this->actor.yDistToPlayer + 5.0f) < 30.0f) {
        mode_fly_init(this);
    }
}

static void mode_event_wait(EnPoSisters* this, PlayState* play) {
    if (work_num != 0 || !player_demo_check(play)) {
        if (this->unk_19A != 0) {
            this->unk_19A--;
        }
        if (this->unk_19A == 30) {
            if (this->unk_194 == 0) {
                makeOnepointDemo(play, 3140, 999, NULL, CAM_ID_MAIN);
            }
            work_num = 1;
        }
        if (this->unk_19A == 0) {
            mode_event_fire_init(this);
        }
    }
    Actor_level_SE_set(&this->actor, NA_SE_EV_TORCH - SFX_FLAG);
}

void mode_event_fire(EnPoSisters* this, PlayState* play) {
    this->unk_19A++;
    set_fire_pos(this, this->unk_19A, &this->actor.home.pos);
    if (this->unk_19A == 32) {
        mode_event_appear_init(this);
    }
}

void mode_event_appear(EnPoSisters* this, PlayState* play) {
    this->unk_19A--;
    if (this->unk_19A == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_APPEAR);
        this->unk_199 &= ~0x80;
    }
    if (this->unk_19A <= 0) {
        if (Skeleton_Info2_anime_play(&this->skelAnime)) {
            this->unk_22E.a = 255;
            work_num |= (1 << this->unk_194);
        } else {
            this->unk_22E.a = (this->skelAnime.curFrame * 255.0f) / this->skelAnime.endFrame;
        }
    }
    if (work_num == 15) {
        mode_event_po_fire_init(this);
    }
}

void mode_event_po_fire(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_19A != 0) {
        this->unk_19A--;
    }
    set_fire_pos(this, this->unk_19A, &this->actor.home.pos);
    if (this->unk_19A == 0) {
        mode_event_escape_init(this, play);
    }
}

void mode_event_escape(EnPoSisters* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->unk_19A--;
    chase_angle(&this->actor.shape.rot.y, this->actor.world.rot.y, 0x500);
    if (this->unk_19A == 0 && this->unk_194 == 0) {
        play->envCtx.lightSettingOverride = 4;
    }
    if (this->unk_19A < 0) {
        chase_f(&this->actor.speed, 5.0f, 0.2f);
    }
    if (this->unk_19A == -70 && this->unk_194 == 1) {
        Effect_SE_Info_new(play, &sound_dummy_pos, 40, NA_SE_EN_PO_LAUGH);
    }
    if (this->unk_19A < -120) {
        Actor_delete(&this->actor);
    }
}

void check_po_sisters_disappear(EnPoSisters* this, PlayState* play) {
    if (this->actor.isLockedOn && this->unk_22E.a == 255) {
        if (this->unk_197 != 0) {
            this->unk_197--;
        }
    } else {
        this->unk_197 = 20;
    }
    if (this->unk_22E.a == 0) {
        if (this->unk_19C != 0) {
            this->unk_19C--;
        }
    }
    if (this->actionFunc != mode_ready && this->actionFunc != mode_attack && this->actionFunc != mode_damage) {
        if (this->unk_197 == 0) {
            mode_disappear_init(this);
        } else if (this->unk_19C == 0 && this->unk_22E.a == 0) {
            mode_appear_init(this, play);
        }
    }
}

void En_Po_Sisters_damage_proc(EnPoSisters* this, PlayState* play) {
    Vec3f sp24;

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elem, true);
        if (this->unk_195 != 0) {
            ((EnPoSisters*)this->actor.parent)->unk_19C--;
            Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH2);
            mode_meg_dummy_init(this, play);
            if (fqrand() < 0.2f) {
                sp24.x = this->actor.world.pos.x;
                sp24.y = this->actor.world.pos.y;
                sp24.z = this->actor.world.pos.z;
                Item_set0(play, &sp24, ITEM00_ARROWS_SMALL);
            }
        } else if (this->collider.base.colMaterial == COL_MATERIAL_METAL ||
                   (this->actor.colChkInfo.damageEffect == 0 && this->actor.colChkInfo.damage == 0)) {
            if (this->unk_194 == 0) {
                this->actor.freezeTimer = 0;
            }
        } else if (this->actor.colChkInfo.damageEffect == 0xF) {
            this->actor.world.rot.y = this->actor.shape.rot.y;
            this->unk_199 |= 2;
            mode_appear_init(this, play);
        } else if (this->unk_194 == 0 && this->actor.colChkInfo.damageEffect == 0xE &&
                   this->actionFunc == mode_meg_rotate) {
            if (this->unk_19C == 0) {
                this->unk_19C = -45;
            }
        } else {
            if (hp_down(&this->actor) != 0) {
                Actor_SE_set(&this->actor, NA_SE_EN_PO_DAMAGE);
            } else {
                Actor_info_finish(play, &this->actor);
                Actor_SE_set(&this->actor, NA_SE_EN_PO_SISTER_DEAD);
            }
            mode_damage_init(this);
        }
    }
}

void En_Po_Sisters_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnPoSisters* this = (EnPoSisters*)thisx;
    s16 temp;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        mode_after_init(this);
    }
    En_Po_Sisters_damage_proc(this, play);
    if (this->unk_199 & 4) {
        check_po_sisters_disappear(this, play);
    }
    this->actionFunc(this, play);
    if (this->unk_199 & 0x1F) {
        if (this->unk_199 & 8) {
            set_po_sisters_position_y(this, play);
        }
        Actor_position_moveF(&this->actor);

        if (this->unk_199 & 0x10) {
            Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 0.0f,
                                    UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        } else {
            Vec3f checkPos;
            s32 bgId;

            checkPos.x = this->actor.world.pos.x;
            checkPos.y = this->actor.world.pos.y + 10.0f;
            checkPos.z = this->actor.world.pos.z;
            this->actor.floorHeight =
                T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &checkPos);
        }

        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        if (this->actionFunc == mode_attack || this->actionFunc == mode_ready) {
            this->unk_198++;
            this->unk_198 = CLAMP_MAX(this->unk_198, 8);
        } else if (this->actionFunc != mode_down) {
            temp = this->unk_198 - 1;
            this->unk_198 = CLAMP_MIN(temp, 1);
        }
        if (this->actionFunc == mode_attack) {
            this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
            CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->unk_199 & 1) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
        if (this->actionFunc != mode_meg_dummy) {
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        }
        Actor_world_to_eye(&this->actor, 40.0f);
        if (this->actionFunc == mode_escape) {
            this->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
        } else if (this->unk_199 & 2) {
            this->actor.shape.rot.y = this->actor.world.rot.y;
        }
    }
}

void set_po_sisters_eye_color(EnPoSisters* this) {
    s16 temp_var;

    if (this->skelAnime.animation == &gPoeSistersAttackAnim) {
        this->unk_22E.r = CLAMP_MAX((s16)(this->unk_22E.r + 5), 255);
        this->unk_22E.g = CLAMP_MIN((s16)(this->unk_22E.g - 5), 50);
        temp_var = this->unk_22E.b - 5;
        this->unk_22E.b = CLAMP_MIN(temp_var, 0);
    } else if (this->skelAnime.animation == &gPoeSistersFleeAnim) {
        this->unk_22E.r = CLAMP_MAX((s16)(this->unk_22E.r + 5), 80);
        this->unk_22E.g = CLAMP_MAX((s16)(this->unk_22E.g + 5), 255);
        temp_var = this->unk_22E.b + 5;
        this->unk_22E.b = CLAMP_MAX(temp_var, 225);
    } else if (this->skelAnime.animation == &gPoeSistersDamagedAnim) {
        if (this->actor.colorFilterTimer & 2) {
            this->unk_22E.r = 0;
            this->unk_22E.g = 0;
            this->unk_22E.b = 0;
        } else {
            this->unk_22E.r = 80;
            this->unk_22E.g = 255;
            this->unk_22E.b = 225;
        }
    } else {
        this->unk_22E.r = CLAMP_MAX((s16)(this->unk_22E.r + 5), 255);
        this->unk_22E.g = CLAMP_MAX((s16)(this->unk_22E.g + 5), 255);
        if (this->unk_22E.b > 210) {
            temp_var = this->unk_22E.b - 5;
            this->unk_22E.b = CLAMP_MIN(temp_var, 210);
        } else {
            temp_var = this->unk_22E.b + 5;
            this->unk_22E.b = CLAMP_MAX(temp_var, 210);
        }
    }
}

s32 en_po_sisters_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                                 Gfx** gfxP) {
    EnPoSisters* this = (EnPoSisters*)thisx;
    Color_RGBA8* color;

    if (limbIndex == 1 && (this->unk_199 & 0x40)) {
        if (this->unk_19A >= 284) {
            rot->x += (this->unk_19A * 0x1000) - 0x11C000;
        } else {
            rot->x += (this->unk_19A * 0x1000) - 0xF000;
        }
    }
    if (this->unk_22E.a == 0 || limbIndex == 8 || (this->actionFunc == mode_down && this->unk_19A >= 8)) {
        *dList = NULL;
    } else if (limbIndex == 9) {
        *dList = body_model[this->unk_194];
    } else if (limbIndex == 10) {
        *dList = face_model[this->unk_194];
        gDPPipeSync((*gfxP)++);
        gDPSetEnvColor((*gfxP)++, this->unk_22E.r, this->unk_22E.g, this->unk_22E.b, this->unk_22E.a);
    } else if (limbIndex == 11) {
        color = &tail_color[this->unk_194];
        gDPPipeSync((*gfxP)++);
        gDPSetEnvColor((*gfxP)++, color->r, color->g, color->b, this->unk_22E.a);
    }
    return false;
}

void en_po_sisters_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfxP) {
    EnPoSisters* this = (EnPoSisters*)thisx;
    s32 i;
    s32 pad;

    if (this->actionFunc == mode_down && this->unk_19A >= 8 && limbIndex == 9) {
        MATRIX_FINALIZE_AND_LOAD((*gfxP)++, play->state.gfxCtx, "../z_en_po_sisters.c", 2876);
        gSPDisplayList((*gfxP)++, gPoSistersBurnDL);
    }
    if (limbIndex == 8 && this->actionFunc != mode_meg_cry) {
        if (this->unk_199 & 0x20) {
            for (i = this->unk_198 - 1; i > 0; i--) {
                this->unk_234[i] = this->unk_234[i - 1];
            }
            Matrix_Position(&fire_pos, &this->unk_234[0]);
        } else if (this->actionFunc == mode_event_appear) {
            Matrix_Position(&fire_pos, &this->actor.home.pos);
        }
        if (this->unk_198 > 0) {
            Color_RGBA8* color = &fire_prim[this->unk_194];
            f32 temp_f2 = fqrand() * 0.3f + 0.7f;

            if (this->actionFunc == mode_fire || this->actionFunc == mode_event_fire ||
                this->actionFunc == mode_event_po_fire) {
                Light_point_ct(&this->lightInfo, this->unk_234[0].x, this->unk_234[0].y + 15.0f,
                                          this->unk_234[0].z, color->r * temp_f2, color->g * temp_f2,
                                          color->b * temp_f2, 200);
            } else {
                Light_point2_ct(&this->lightInfo, this->unk_234[0].x, this->unk_234[0].y + 15.0f,
                                        this->unk_234[0].z, color->r * temp_f2, color->g * temp_f2, color->b * temp_f2,
                                        200);
            }
        } else {
            Light_point_color_set(&this->lightInfo, 0, 0, 0, 0);
        }
        if (!(this->unk_199 & 0x80)) {
            Matrix_get(&this->unk_2F8);
        }
    }
}

void En_Po_Sisters_actor_draw(Actor* thisx, PlayState* play) {
    EnPoSisters* this = (EnPoSisters*)thisx;
    s32 pad1;
    f32 phi_f20;
    s32 i;
    u8 phi_s5;
    Color_RGBA8* temp_s1 = &fire_env[this->unk_194];
    Color_RGBA8* temp_s7 = &fire_prim[this->unk_194];
    s32 pad2;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_po_sisters.c", 2989);
    set_po_sisters_eye_color(this);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    if (this->unk_22E.a == 255 || this->unk_22E.a == 0) {
        gDPSetEnvColor(POLY_OPA_DISP++, this->unk_22E.r, this->unk_22E.g, this->unk_22E.b, this->unk_22E.a);
        gSPSegment(POLY_OPA_DISP++, 0x09, Actor_change_render_mode + 2);
        POLY_OPA_DISP =
            Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_po_sisters_display1,
                           en_po_sisters_display2, &this->actor, POLY_OPA_DISP);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->unk_22E.a);
        gSPSegment(POLY_XLU_DISP++, 0x09, Actor_change_render_mode);
        POLY_XLU_DISP =
            Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_po_sisters_display1,
                           en_po_sisters_display2, &this->actor, POLY_XLU_DISP);
    }
    if (!(this->unk_199 & 0x80)) {
        Matrix_put(&this->unk_2F8);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_po_sisters.c", 3034);
        gSPDisplayList(POLY_OPA_DISP++, gPoSistersTorchDL);
    }
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1, 0,
                                (play->gameplayFrames * -20) % 512, 0x20, 0x80));
    gDPSetEnvColor(POLY_XLU_DISP++, temp_s1->r, temp_s1->g, temp_s1->b, temp_s1->a);
    if (this->actionFunc == mode_fire) {
        if (this->unk_19A < 32) {
            phi_s5 = ((32 - this->unk_19A) * 255) / 32;
            phi_f20 = 0.0056000003f;
        } else {
            phi_s5 = (this->unk_19A * 255 - 8160) / 32;
            phi_f20 = 0.0027f;
        }
    } else if (this->actionFunc == mode_event_fire) {
        phi_s5 = ((32 - this->unk_19A) * 255) / 32;
        phi_f20 = 0.0027f;
    } else if (this->actionFunc == mode_event_po_fire) {
        phi_s5 = ((32 - this->unk_19A) * 255) / 32;
        phi_f20 = 0.0035f;
    } else if (this->actionFunc == mode_event_wait) {
        // phi_s5 initialized in loop below
        phi_f20 = 0.0027f;
    } else {
        // phi_s5 initialized in loop below
        phi_f20 = this->actor.scale.x * 0.5f;
    }
    for (i = 0; i < this->unk_198; i++) {
        if (this->actionFunc != mode_fire && this->actionFunc != mode_event_fire &&
            this->actionFunc != mode_event_po_fire) {
            phi_s5 = -i * 31 + 248;
        }
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, temp_s7->r, temp_s7->g, temp_s7->b, phi_s5);
        Matrix_translate(this->unk_234[i].x, this->unk_234[i].y, this->unk_234[i].z, MTXMODE_NEW);
        Matrix_rotateXYZ(0, (s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000), 0, MTXMODE_APPLY);
        if (this->actionFunc == mode_down) {
            phi_f20 = (this->unk_19A - i) * 0.025f + 0.5f;
            phi_f20 = CLAMP(phi_f20, 0.5f, 0.8f) * 0.007f;
        }
        Matrix_scale(phi_f20, phi_f20, phi_f20, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_po_sisters.c", 3132);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_po_sisters.c", 3139);
}
