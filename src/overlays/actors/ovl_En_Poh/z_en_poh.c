/*
 * File: z_en_poh.c
 * Overlay: ovl_En_Poh
 * Description: Graveyard Poe
 */

#include "z_en_poh.h"
#include "assets/objects/object_poh/object_poh.h"
#include "assets/objects/object_po_composer/object_po_composer.h"

#define FLAGS \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_IGNORE_QUAKE)

void En_Poh_actor_ct(Actor* thisx, PlayState* play);
void En_Poh_actor_dt(Actor* thisx, PlayState* play);
void En_Poh_actor_dma_wait(Actor* thisx, PlayState* play);

void En_Poh_actor_move(Actor* thisx, PlayState* play);
void En_Poh_actor_after_move(Actor* thisx, PlayState* play);
void En_Poh_actor_draw(Actor* thisx, PlayState* play);
void En_Poh_actor_draw2(Actor* thisx, PlayState* play);
void En_Poh_actor_after_draw(Actor* thisx, PlayState* play);

static void mode_wait(EnPoh* this, PlayState* play);
static void mode_fly(EnPoh* this, PlayState* play);
static void mode_chase(EnPoh* this, PlayState* play);
static void mode_attack(EnPoh* this, PlayState* play);
static void mode_damage(EnPoh* this, PlayState* play);
static void mode_escape(EnPoh* this, PlayState* play);
void mode_start_composer(EnPoh* this, PlayState* play);
static void mode_start(EnPoh* this, PlayState* play);
static void mode_down(EnPoh* this, PlayState* play);
void mode_attack_diffence(EnPoh* this, PlayState* play);
static void mode_reverse(EnPoh* this, PlayState* play);
static void mode_disappear(EnPoh* this, PlayState* play);
static void mode_appear(EnPoh* this, PlayState* play);
static void mode_kantera_drop(EnPoh* this, PlayState* play);
static void mode_soul_appear(EnPoh* this, PlayState* play);
static void mode_soul(EnPoh* this, PlayState* play);
static void mode_soul_delete(EnPoh* this, PlayState* play);
static void mode_soul_get(EnPoh* this, PlayState* play);
static void mode_talk(EnPoh* this, PlayState* play);

static s16 haka_appear_cnt = 0;

ActorProfile En_Poh_Profile = {
    /**/ ACTOR_EN_POH,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnPoh),
    /**/ En_Poh_actor_ct,
    /**/ En_Poh_actor_dt,
    /**/ En_Poh_actor_dma_wait,
    /**/ NULL,
};

static ColliderCylinderInit PohAcOcPipeData = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 40, 20, { 0, 0, 0 } },
};

static ColliderJntSphElementInit PohAtJntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xFFCFFFFF, 0x00, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 18, { { 0, 1400, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit PohAtJntSphData = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    PohAtJntSphElemData,
};

static CollisionCheckInfoInit PohStatusData = { 4, 25, 50, 40 };

static DamageTable PohBtlData = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(1, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(2, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
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

static EnPohInfo poh_status_data[2] = {
    {
        { 255, 170, 255 },
        { 100, 0, 150 },
        18,
        5,
        248,
        &gPoeDisappearAnim,
        &gPoeFloatAnim,
        &gPoeDamagedAnim,
        &gPoeFleeAnim,
        gPoeLanternDL,
        gPoeBurnDL,
        gPoeSoulDL,
    },
    {
        { 255, 255, 170 },
        { 0, 150, 0 },
        9,
        1,
        244,
        &gPoeComposerDisappearAnim,
        &gPoeComposerFloatAnim,
        &gPoeComposerDamagedAnim,
        &gPoeComposerFleeAnim,
        gPoeComposerLanternDL,
        gPoeComposerBurnDL,
        gPoeComposerSoulDL,
    },
};

static Color_RGBA8 anm_3_env = { 75, 20, 25, 255 };
static Color_RGBA8 D_80AE1B50 = { 80, 110, 90, 255 };
static Color_RGBA8 anm_4_env = { 90, 85, 50, 255 };
static Color_RGBA8 D_80AE1B58 = { 100, 90, 100, 255 };

static InitChainEntry value_init[] = {
    ICHAIN_F32(lockOnArrowOffset, 3200, ICHAIN_STOP),
};

static Vec3f pvec = { 0.0f, 3.0f, 0.0f };
static Vec3f pacc = { 0.0f, 0.0f, 0.0f };

void En_Poh_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnItem00* collectible;
    EnPoh* this = (EnPoh*)thisx;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    ClObjJntSph_ct(play, &this->colliderSph);
    ClObjJntSph_set5_nzm(play, &this->colliderSph, &this->actor, &PohAtJntSphData, &this->colliderSphItem);
    this->colliderSph.elements[0].dim.worldSphere.radius = 0;
    this->colliderSph.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
    this->colliderSph.elements[0].dim.worldSphere.center.y = this->actor.world.pos.y;
    this->colliderSph.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;
    ClObjPipe_ct(play, &this->colliderCyl);
    ClObjPipe_set5(play, &this->colliderCyl, &this->actor, &PohAcOcPipeData);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, &PohBtlData, &PohStatusData);
    this->unk_194 = 0;
    this->unk_195 = 32;
    this->visibilityTimer = get_random_timer(700, 300);
    this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
    Light_point2_ct(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z,
                            255, 255, 255, 0);
    if (this->actor.params >= 4) {
        this->actor.params = EN_POH_NORMAL;
    }
    if (this->actor.params == EN_POH_RUPEE) {
        haka_appear_cnt++;
        if (haka_appear_cnt >= 3) {
            Actor_delete(&this->actor);
        } else {
            collectible = Item_set0(play, &this->actor.world.pos, 0x4000 | ITEM00_RUPEE_BLUE);
            if (collectible != NULL) {
                collectible->actor.speed = 0.0f;
            }
        }
    } else if (this->actor.params == EN_POH_FLAT) {
        if (Actor_Environment_sw_Check(play, 0x28) || Actor_Environment_sw_Check(play, 0x9)) {
            Actor_delete(&this->actor);
        } else {
            Actor_Environment_sw_On(play, 0x28);
        }
    } else if (this->actor.params == EN_POH_SHARP) {
        if (Actor_Environment_sw_Check(play, 0x29) || Actor_Environment_sw_Check(play, 0x9)) {
            Actor_delete(&this->actor);
        } else {
            Actor_Environment_sw_On(play, 0x29);
        }
    }
    if (this->actor.params < EN_POH_SHARP) {
        this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_POH);
        this->infoIdx = EN_POH_INFO_NORMAL;
        this->actor.naviEnemyId = NAVI_ENEMY_POE;
    } else {
        this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_PO_COMPOSER);
        this->infoIdx = EN_POH_INFO_COMPOSER;
        this->actor.naviEnemyId = NAVI_ENEMY_POE_COMPOSER;
    }
    this->info = &poh_status_data[this->infoIdx];
    if (this->requiredObjectSlot < 0) {
        Actor_delete(&this->actor);
    }
}

void En_Poh_actor_dt(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;

    Global_light_list_delete(play, &play->lightCtx, this->lightNode);
    ClObjJntSph_dt_nzf(play, &this->colliderSph);
    ClObjPipe_dt(play, &this->colliderCyl);
    if (this->actor.params == EN_POH_RUPEE) {
        haka_appear_cnt--;
    }
}

static void mode_wait_init(EnPoh* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, this->info->idleAnim);
    this->unk_198 = get_random_timer(2, 3);
    this->actionFunc = mode_wait;
    this->actor.speed = 0.0f;
}

static void mode_fly_init(EnPoh* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, this->info->idleAnim2);
    this->unk_198 = get_random_timer(15, 3);
    this->actionFunc = mode_fly;
}

static void mode_chase_init(EnPoh* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, this->info->idleAnim2);
    this->actionFunc = mode_chase;
    this->unk_198 = 0;
    this->actor.speed = 2.0f;
}

static void mode_attack_init(EnPoh* this) {
    if (this->infoIdx == EN_POH_INFO_NORMAL) {
        Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gPoeAttackAnim, -6.0f);
    } else {
        Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gPoeComposerAttackAnim);
    }
    this->unk_198 = 12;
    this->actor.speed = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
    this->actionFunc = mode_attack;
}

static void mode_damage_init(EnPoh* this) {
    if (this->infoIdx == EN_POH_INFO_NORMAL) {
        Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gPoeDamagedAnim, -6.0f);
    } else {
        Skeleton_Info2_init_standard_stop(&this->skelAnime, &gPoeComposerDamagedAnim);
    }
    if (this->colliderCyl.elem.acHitElem->atDmgInfo.dmgFlags & (DMG_ARROW | DMG_SLINGSHOT)) {
        this->actor.world.rot.y = this->colliderCyl.base.ac->world.rot.y;
    } else {
        this->actor.world.rot.y = Actor_search_actor_angleY(&this->actor, this->colliderCyl.base.ac) + 0x8000;
    }
    this->colliderCyl.base.acFlags &= ~AC_ON;
    this->actor.speed = 5.0f;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 16);
    this->actionFunc = mode_damage;
}

static void mode_escape_init(EnPoh* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, this->info->fleeAnim, -5.0f);
    this->actor.speed = 5.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y + 0x8000;
    this->colliderCyl.base.acFlags |= AC_ON;
    this->unk_198 = 200;
    this->actionFunc = mode_escape;
}

static void mode_start_init(EnPoh* this) {
    this->lightColor.a = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    if (this->infoIdx == EN_POH_INFO_NORMAL) {
        Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gPoeAppearAnim, 0.0f);
        this->actionFunc = mode_start;
    } else {
        Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gPoeComposerAppearAnim, 1.0f);
        this->actor.world.pos.y = this->actor.home.pos.y + 20.0f;
        Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
        Actor_SE_set(&this->actor, NA_SE_EN_PO_APPEAR);
        this->actionFunc = mode_start_composer;
    }
}

static void mode_down_init(EnPoh* this) {
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->unk_198 = 0;
    this->actor.naviEnemyId = NAVI_ENEMY_NONE;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = mode_down;
}

void mode_attack_diffence_init(EnPoh* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, this->info->idleAnim2);
    this->actionFunc = mode_attack_diffence;
    this->actor.speed = -5.0f;
}

static void mode_reverse_init(EnPoh* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, this->info->idleAnim);
    this->unk_19C = this->actor.world.rot.y + 0x8000;
    this->actionFunc = mode_reverse;
    this->actor.speed = 0.0f;
}

static void mode_disappear_init(EnPoh* this) {
    this->unk_194 = 32;
    this->actor.speed = 0.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    Actor_SE_set(&this->actor, NA_SE_EN_PO_DISAPPEAR);
    Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
    this->actionFunc = mode_disappear;
}

static void mode_appear_init(EnPoh* this) {
    this->unk_194 = 0;
    this->actor.speed = 0.0f;
    Actor_SE_set(&this->actor, NA_SE_EN_PO_APPEAR);
    Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
    this->actionFunc = mode_appear;
}

static void mode_kantera_drop_init(EnPoh* this, PlayState* play) {
    this->actor.update = En_Poh_actor_after_move;
    this->actor.draw = En_Poh_actor_after_draw;
    this->actor.shape.shadowDraw = NULL;
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.gravity = -1.0f;
    this->actor.shape.yOffset = 1500.0f;
    this->actor.world.pos.y -= 15.0f;
    if (this->infoIdx != EN_POH_INFO_COMPOSER) {
        this->actor.shape.rot.x = -0x8000;
    }
    Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_MISC);
    this->unk_198 = 60;
    this->actionFunc = mode_kantera_drop;
}

static void mode_soul_appear_init(EnPoh* this) {
    Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 0, 0, 0, 0);
    this->visibilityTimer = 0;
    this->actor.shape.rot.y = 0;
    this->lightColor.r = 0;
    this->lightColor.a = 0;
    this->actor.shape.yOffset = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    if (this->actor.params >= EN_POH_SHARP) {
        this->lightColor.g = 200;
        this->lightColor.b = 0;
    } else {
        this->lightColor.g = 0;
        this->lightColor.b = 200;
    }
    this->actor.scale.x = 0.0f;
    this->actor.scale.y = 0.0f;
    this->actor.shape.rot.x = 0;
    this->actor.home.pos.y = this->actor.world.pos.y;
    Actor_SE_set(&this->actor, NA_SE_EV_METAL_BOX_BOUND);
    this->actionFunc = mode_soul_appear;
}

static void mode_soul_init(EnPoh* this, PlayState* play) {
    this->actor.home.pos.y = this->actor.world.pos.y;
    Actor_world_to_eye(&this->actor, -10.0f);
    this->colliderCyl.dim.radius = 13;
    this->colliderCyl.dim.height = 30;
    this->colliderCyl.dim.yShift = 0;
    this->colliderCyl.dim.pos.x = this->actor.world.pos.x;
    this->colliderCyl.dim.pos.y = this->actor.world.pos.y - 20.0f;
    this->colliderCyl.dim.pos.z = this->actor.world.pos.z;
    this->colliderCyl.base.ocFlags1 = OC1_ON | OC1_TYPE_PLAYER;
    if (this->actor.params == EN_POH_FLAT || this->actor.params == EN_POH_SHARP) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_SUN)) {
            this->actor.textId = 0x5000;
        } else if (!Actor_Environment_sw_Check(play, 0xA) && !Actor_Environment_sw_Check(play, 0xB)) {
            this->actor.textId = 0x500F;
        } else if ((this->actor.params == EN_POH_FLAT && Actor_Environment_sw_Check(play, 0xA)) ||
                   (this->actor.params == EN_POH_SHARP && Actor_Environment_sw_Check(play, 0xB))) {
            this->actor.textId = 0x5013;
        } else {
            this->actor.textId = 0x5012;
        }
    } else {
        this->actor.textId = 0x5005;
    }
    this->unk_198 = 200;
    this->unk_195 = 32;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = mode_soul;
}

static void mode_soul_delete_init(EnPoh* this, s32 arg1) {
    if (arg1) {
        Nai_StopObjFlagFx(&this->actor.projectedPos, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
        Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
    }
    this->actionFunc = mode_soul_delete;
}

static void mode_soul_get_init(EnPoh* this) {
    this->actionFunc = mode_soul_get;
    this->actor.home.pos.y = this->actor.world.pos.y - 15.0f;
}

static void mode_talk_init(EnPoh* this) {
    this->actionFunc = mode_talk;
}

void set_poh_position_y(EnPoh* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    chase_f(&this->actor.world.pos.y, player->actor.world.pos.y, 1.0f);
    this->actor.world.pos.y += 2.5f * sin_s(this->unk_195 * 0x800);
    if (this->unk_195 != 0) {
        this->unk_195 -= 1;
    }
    if (this->unk_195 == 0) {
        this->unk_195 = 32;
    }
}

void set_poh_aim_angle(EnPoh* this) {
    if (Actor_search_position_distanceXZ(&this->actor, &this->actor.home.pos) > 400.0f) {
        this->unk_19C = Actor_search_position_angleY(&this->actor, &this->actor.home.pos);
    }
    chase_angle(&this->actor.world.rot.y, this->unk_19C, 0x71C);
}

static void mode_wait(EnPoh* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && this->unk_198 != 0) {
        this->unk_198--;
    }
    set_poh_position_y(this, play);
    if (this->actor.xzDistToPlayer < 200.0f) {
        mode_chase_init(this);
    } else if (this->unk_198 == 0) {
        mode_fly_init(this);
    }
    if (this->lightColor.a == 255) {
        Actor_level_SE_set(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    }
}

static void mode_fly(EnPoh* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    chase_f(&this->actor.speed, 1.0f, 0.2f);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && this->unk_198 != 0) {
        this->unk_198--;
    }
    set_poh_aim_angle(this);
    set_poh_position_y(this, play);
    if (this->actor.xzDistToPlayer < 200.0f && this->unk_198 < 19) {
        mode_chase_init(this);
    } else if (this->unk_198 == 0) {
        if (fqrand() < 0.1f) {
            mode_reverse_init(this);
        } else {
            mode_wait_init(this);
        }
    }
    if (this->lightColor.a == 255) {
        Actor_level_SE_set(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    }
}

static void mode_chase(EnPoh* this, PlayState* play) {
    Player* player;
    s16 facingDiff;

    player = GET_PLAYER(play);
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_198 != 0) {
        this->unk_198--;
    }
    facingDiff = this->actor.yawTowardsPlayer - player->actor.shape.rot.y;
    if (facingDiff >= 0x3001) {
        chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x3000, 0x71C);
    } else if (facingDiff < -0x3000) {
        chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer - 0x3000, 0x71C);
    } else {
        chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x71C);
    }
    set_poh_position_y(this, play);
    if (this->actor.xzDistToPlayer > 280.0f) {
        mode_fly_init(this);
    } else if (this->unk_198 == 0 && this->actor.xzDistToPlayer < 140.0f &&
               !Actor_player_look_direction_check(&this->actor, 0x2AAA, play)) {
        mode_attack_init(this);
    }
    if (this->lightColor.a == 255) {
        Actor_level_SE_set(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
    }
}

static void mode_attack(EnPoh* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_KANTERA);
        if (this->unk_198 != 0) {
            this->unk_198--;
        }
    }
    set_poh_position_y(this, play);
    if (this->unk_198 >= 10) {
        chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0xE38);
    } else if (this->unk_198 == 9) {
        this->actor.speed = 5.0f;
        this->skelAnime.playSpeed = 2.0f;
    } else if (this->unk_198 == 0) {
        mode_fly_init(this);
        this->unk_198 = 23;
    }
}

static void mode_damage(EnPoh* this, PlayState* play) {
    chase_f(&this->actor.speed, 0.0f, 0.5f);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.colChkInfo.health != 0) {
            mode_escape_init(this);
        } else {
            mode_down_init(this);
        }
    }
}

static void mode_start(EnPoh* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->lightColor.a = 255;
        this->visibilityTimer = get_random_timer(700, 300);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        mode_fly_init(this);
    } else if (this->skelAnime.curFrame > 10.0f) {
        this->lightColor.a = ((this->skelAnime.curFrame - 10.0f) * 0.05f) * 255.0f;
    }
    if (this->skelAnime.playSpeed < 0.5f && this->actor.xzDistToPlayer < 280.0f) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_APPEAR);
        this->skelAnime.playSpeed = 1.0f;
    }
}

void mode_start_composer(EnPoh* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->lightColor.a = 255;
        this->visibilityTimer = get_random_timer(700, 300);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        mode_fly_init(this);
    } else {
        this->lightColor.a = CLAMP_MAX((s32)(this->skelAnime.curFrame * 25.5f), 255);
    }
}

static void mode_down(EnPoh* this, PlayState* play) {
    Vec3f vec;
    f32 multiplier;
    f32 newScale;
    s32 pad;
    s32 pad1;

    this->unk_198++;
    if (this->unk_198 < 8) {
        if (this->unk_198 < 5) {
            vec.y = sin_s((this->unk_198 * 0x1000) - 0x4000) * 23.0f + (this->actor.world.pos.y + 40.0f);
            multiplier = cos_s((this->unk_198 * 0x1000) - 0x4000) * 23.0f;
            vec.x =
                sin_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * multiplier + this->actor.world.pos.x;
            vec.z =
                cos_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * multiplier + this->actor.world.pos.z;
        } else {
            vec.y = (this->actor.world.pos.y + 40.0f) + (15.0f * (this->unk_198 - 5));
            vec.x = sin_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * 23.0f + this->actor.world.pos.x;
            vec.z = cos_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * 23.0f + this->actor.world.pos.z;
        }
        _Effect_SS_Db_ct(play, &vec, &pvec, &pacc, this->unk_198 * 10 + 80, 0, 255, 255, 255, 255, 0, 0,
                             255, 1, 9, 1);
        vec.x = (this->actor.world.pos.x + this->actor.world.pos.x) - vec.x;
        vec.z = (this->actor.world.pos.z + this->actor.world.pos.z) - vec.z;
        _Effect_SS_Db_ct(play, &vec, &pvec, &pacc, this->unk_198 * 10 + 80, 0, 255, 255, 255, 255, 0, 0,
                             255, 1, 9, 1);
        vec.x = this->actor.world.pos.x;
        vec.z = this->actor.world.pos.z;
        _Effect_SS_Db_ct(play, &vec, &pvec, &pacc, this->unk_198 * 10 + 80, 0, 255, 255, 255, 255, 0, 0,
                             255, 1, 9, 1);
        if (this->unk_198 == 1) {
            Actor_SE_set(&this->actor, NA_SE_EN_EXTINCT);
        }
    } else if (this->unk_198 == 28) {
        mode_kantera_drop_init(this, play);
    } else if (this->unk_198 >= 19) {
        newScale = (28 - this->unk_198) * 0.001f;
        this->actor.world.pos.y += 5.0f;
        this->actor.scale.z = newScale;
        this->actor.scale.y = newScale;
        this->actor.scale.x = newScale;
    }
    if (this->unk_198 == 18) {
        Actor_SE_set(&this->actor, NA_SE_EN_PO_DEAD2);
    }
}

void mode_attack_diffence(EnPoh* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        mode_fly_init(this);
        this->unk_198 = 23;
    } else {
        chase_f(&this->actor.speed, 0.0f, 0.5f);
        this->actor.shape.rot.y += 0x1000;
    }
}

static void mode_reverse(EnPoh* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (chase_angle(&this->actor.world.rot.y, this->unk_19C, 1820) != 0) {
        mode_fly_init(this);
    }
    if (this->actor.xzDistToPlayer < 200.0f) {
        mode_chase_init(this);
    }
    set_poh_position_y(this, play);
}

static void mode_disappear(EnPoh* this, PlayState* play) {
    if (this->unk_194 != 0) {
        this->unk_194--;
    }
    this->actor.world.rot.y += 0x1000;
    set_poh_position_y(this, play);
    this->lightColor.a = this->unk_194 * 7.96875f;
    if (this->unk_194 == 0) {
        this->visibilityTimer = get_random_timer(100, 50);
        mode_fly_init(this);
    }
}

static void mode_appear(EnPoh* this, PlayState* play) {
    this->unk_194++;
    this->actor.world.rot.y -= 0x1000;
    set_poh_position_y(this, play);
    this->lightColor.a = this->unk_194 * 7.96875f;
    if (this->unk_194 == 32) {
        this->visibilityTimer = get_random_timer(700, 300);
        this->unk_194 = 0;
        mode_fly_init(this);
    }
}

static void mode_escape(EnPoh* this, PlayState* play) {
    f32 multiplier;

    Skeleton_Info2_anime_play(&this->skelAnime);
    multiplier = sin_s(this->unk_195 * 0x800) * 3.0f;
    this->actor.world.pos.x -= multiplier * cos_s(this->actor.shape.rot.y);
    this->actor.world.pos.z += multiplier * sin_s(this->actor.shape.rot.y);
    chase_angle(&this->actor.world.rot.y, this->actor.yawTowardsPlayer + 0x8000, 0x71C);
    set_poh_position_y(this, play);
    if (this->unk_198 == 0 || this->actor.xzDistToPlayer > 250.0f) {
        this->actor.world.rot.y = this->actor.shape.rot.y;
        mode_fly_init(this);
    }
    Actor_level_SE_set(&this->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
}

static void mode_kantera_drop(EnPoh* this, PlayState* play) {
    s32 objectId;

    if (this->unk_198 != 0) {
        this->unk_198--;
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        objectId = (this->infoIdx == EN_POH_INFO_COMPOSER) ? OBJECT_PO_COMPOSER : OBJECT_POH;
        Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 6.0f, 0, 1, 1, 15, objectId, 10,
                                 this->info->lanternDisplayList);
        mode_soul_appear_init(this);
    } else if (this->unk_198 == 0) {
        Actor_delete(&this->actor);
        return;
    }
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 10.0f, 10.0f, 10.0f, UPDBGCHECKINFO_FLAG_2);
}

void poh_soul_set(EnPoh* this, s32 arg1) {
    f32 multiplier;

    this->lightColor.a = CLAMP(this->lightColor.a + arg1, 0, 255);
    if (arg1 < 0) {
        multiplier = this->lightColor.a * (1.0f / 255);
        this->actor.scale.x = this->actor.scale.z = 0.0056000002f * multiplier + 0.0014000001f;
        this->actor.scale.y = (0.007f - 0.007f * multiplier) + 0.007f;
    } else {
        multiplier = 1.0f;
        this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = this->lightColor.a * (0.007f / 0xFF);
        this->actor.world.pos.y = this->actor.home.pos.y + (1.0f / 17.0f) * this->lightColor.a;
    }
    this->lightColor.r = this->info->lightColor.r * multiplier;
    this->lightColor.g = this->info->lightColor.g * multiplier;
    this->lightColor.b = this->info->lightColor.b * multiplier;
    Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, this->info->lightColor.r, this->info->lightColor.g,
                              this->info->lightColor.b, this->lightColor.a * (200.0f / 255));
}

static void mode_soul_appear(EnPoh* this, PlayState* play) {
    this->actor.home.pos.y += 2.0f;
    poh_soul_set(this, 20);
    if (this->lightColor.a == 255) {
        mode_soul_init(this, play);
    }
}

static void mode_soul(EnPoh* this, PlayState* play) {
    if (this->unk_198 != 0) {
        this->unk_198--;
    }
    if (Actor_talk_check(&this->actor, play)) {
        if (this->actor.params >= EN_POH_SHARP) {
            mode_talk_init(this);
        } else {
            mode_soul_get_init(this);
        }
        return;
    }
    if (this->unk_198 == 0) {
        mode_soul_delete_init(this, 1);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        return;
    }
    if (this->colliderCyl.base.ocFlags1 & OC1_HIT) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_talk_request(&this->actor, play);
    } else {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderCyl.base);
    }
    this->actor.world.pos.y = sin_s(this->unk_195 * 0x800) * 5.0f + this->actor.home.pos.y;
    if (this->unk_195 != 0) {
        this->unk_195 -= 1;
    }
    if (this->unk_195 == 0) {
        this->unk_195 = 32;
    }
    this->colliderCyl.dim.pos.y = this->actor.world.pos.y - 20.0f;
    Actor_world_to_eye(&this->actor, -10.0f);
    Light_point_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, this->info->lightColor.r, this->info->lightColor.g,
                              this->info->lightColor.b, this->lightColor.a * (200.0f / 255));
}

static void mode_soul_delete(EnPoh* this, PlayState* play) {
    poh_soul_set(this, -13);
    if (this->lightColor.a == 0) {
        Actor_delete(&this->actor);
    }
}

static void mode_soul_get(EnPoh* this, PlayState* play) {
    if (this->actor.textId != 0x5005) {
        poh_soul_set(this, -13);
    } else {
        Actor_level_SE_set(&this->actor, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
    }
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE) {
        if (pad_on_check(play)) {
            Nai_StopObjFlagFx(&this->actor.projectedPos, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
            if (play->msgCtx.choiceIndex == 0) {
                if (findEmptyBottle()) {
                    this->actor.textId = 0x5008;
                    item_get_setting(play, ITEM_BOTTLE_POE);
                    Actor_SE_set(&this->actor, NA_SE_EN_PO_BIG_GET);
                } else {
                    this->actor.textId = 0x5006;
                    Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
                }
            } else {
                this->actor.textId = 0x5007;
                Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
            }
            message_set2(play, this->actor.textId);
        }
    } else if (Actor_talk_end_check(&this->actor, play)) {
        mode_soul_delete_init(this, 0);
    }
}

static void mode_talk(EnPoh* this, PlayState* play) {
    Actor_level_SE_set(&this->actor, NA_SE_EN_PO_BIG_CRY - SFX_FLAG);
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE) {
        if (pad_on_check(play)) {
            if (play->msgCtx.choiceIndex == 0) {
                if (!Actor_Environment_sw_Check(play, 0xB) && !Actor_Environment_sw_Check(play, 0xA)) {
                    this->actor.textId = 0x5010;
                } else {
                    this->actor.textId = 0x5014;
                }
                message_set2(play, this->actor.textId);
            } else {
                if (this->actor.params == EN_POH_SHARP) {
                    Actor_Environment_sw_On(play, 0xB);
                } else {
                    Actor_Environment_sw_On(play, 0xA);
                }
                mode_soul_delete_init(this, 1);
            }
        }
    } else if (Actor_talk_end_check(&this->actor, play)) {
        if (this->actor.textId == 0x5000) {
            Actor_Environment_sw_On(play, 9);
        }
        mode_soul_delete_init(this, 1);
    }
}

void En_Poh_damage_proc(EnPoh* this, PlayState* play) {
    if (this->colliderCyl.base.acFlags & AC_HIT) {
        this->colliderCyl.base.acFlags &= ~AC_HIT;
        if (this->actor.colChkInfo.damageEffect != 0 || this->actor.colChkInfo.damage != 0) {
            if (hp_down(&this->actor) == 0) {
                Actor_info_finish(play, &this->actor);
                Actor_SE_set(&this->actor, NA_SE_EN_PO_DEAD);
            } else {
                Actor_SE_set(&this->actor, NA_SE_EN_PO_DAMAGE);
            }
            mode_damage_init(this);
        }
    }
}

void check_disappear_mode(EnPoh* this) {
    if (this->actionFunc != mode_appear && this->actionFunc != mode_disappear && this->actionFunc != mode_start &&
        this->actionFunc != mode_start_composer) {
        if (this->visibilityTimer != 0) {
            this->visibilityTimer--;
        }
        if (this->lightColor.a == 255) {
            if (this->actor.isLockedOn) {
                this->unk_194++;
                this->unk_194 = CLAMP_MAX(this->unk_194, 20);
            } else {
                this->unk_194 = 0;
            }
            if ((this->unk_194 == 20 || this->visibilityTimer == 0) &&
                (this->actionFunc == mode_wait || this->actionFunc == mode_fly ||
                 this->actionFunc == mode_chase || this->actionFunc == mode_escape ||
                 this->actionFunc == mode_reverse)) {
                mode_disappear_init(this);
            }
        } else if (this->lightColor.a == 0 && this->visibilityTimer == 0 &&
                   (this->actionFunc == mode_wait || this->actionFunc == mode_fly ||
                    this->actionFunc == mode_chase || this->actionFunc == mode_reverse)) {
            mode_appear_init(this);
        }
    }
}

void En_Poh_actor_dma_wait(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;

    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->actor.objectSlot = this->requiredObjectSlot;
        this->actor.update = En_Poh_actor_move;
        Actor_set_segment(play, &this->actor);
        if (this->infoIdx == EN_POH_INFO_NORMAL) {
            Skeleton_Info2_M_ct(play, &this->skelAnime, &gPoeSkel, &gPoeFloatAnim, this->jointTable, this->morphTable, 21);
            this->actor.draw = En_Poh_actor_draw;
        } else {
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gPoeComposerSkel, &gPoeComposerFloatAnim, this->jointTable,
                               this->morphTable, 12);
            this->actor.draw = En_Poh_actor_draw2;
            this->colliderSph.elements[0].dim.limb = 9;
            this->colliderSph.elements[0].dim.modelSphere.center.y *= -1;
            this->actor.shape.rot.y = this->actor.world.rot.y = -0x4000;
            this->colliderCyl.dim.radius = 20;
            this->colliderCyl.dim.height = 55;
            this->colliderCyl.dim.yShift = 15;
        }
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        mode_start_init(this);
    }
}

void set_poh_color_data(EnPoh* this) {
    s16 temp_var;

    if (this->actionFunc == mode_attack) {
        this->lightColor.r = CLAMP_MAX((s16)(this->lightColor.r + 5), 255);
        this->lightColor.g = CLAMP_MIN((s16)(this->lightColor.g - 5), 50);
        temp_var = this->lightColor.b - 5;
        this->lightColor.b = CLAMP_MIN(temp_var, 0);
    } else if (this->actionFunc == mode_escape) {
        this->lightColor.r = CLAMP_MAX((s16)(this->lightColor.r + 5), 80);
        this->lightColor.g = CLAMP_MAX((s16)(this->lightColor.g + 5), 255);
        temp_var = this->lightColor.b + 5;
        this->lightColor.b = CLAMP_MAX(temp_var, 225);
    } else if (this->actionFunc == mode_damage) {
        if (this->actor.colorFilterTimer & 2) {
            this->lightColor.r = 0;
            this->lightColor.g = 0;
            this->lightColor.b = 0;
        } else {
            this->lightColor.r = 80;
            this->lightColor.g = 255;
            this->lightColor.b = 225;
        }
    } else {
        this->lightColor.r = CLAMP_MAX((s16)(this->lightColor.r + 5), 255);
        this->lightColor.g = CLAMP_MAX((s16)(this->lightColor.g + 5), 255);
        if (this->lightColor.b >= 211) {
            temp_var = this->lightColor.b - 5;
            this->lightColor.b = CLAMP_MIN(temp_var, 210);
        } else {
            temp_var = this->lightColor.b + 5;
            this->lightColor.b = CLAMP_MAX(temp_var, 210);
        }
    }
}

void set_poh_kantera_color_data(EnPoh* this) {
    f32 rand;

    if ((this->actionFunc == mode_start || this->actionFunc == mode_start_composer) &&
        this->skelAnime.curFrame < 12.0f) {
        this->envColor.r = this->envColor.g = this->envColor.b = (s16)(this->skelAnime.curFrame * 16.66f) + 55;
        this->envColor.a = this->skelAnime.curFrame * (100.0f / 6.0f);
    } else {
        rand = fqrand();
        this->envColor.r = (s16)(rand * 30.0f) + 225;
        this->envColor.g = (s16)(rand * 100.0f) + 155;
        this->envColor.b = (s16)(rand * 160.0f) + 95;
        this->envColor.a = 200;
    }
}

void En_Poh_actor_move(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;
    s32 pad;
    Vec3f checkPos;
    s32 bgId;

    if (this->colliderSph.base.atFlags & AT_HIT) {
        this->colliderSph.base.atFlags &= ~AT_HIT;
        mode_attack_diffence_init(this);
    }
    En_Poh_damage_proc(this, play);
    check_disappear_mode(this);
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    if (this->actionFunc == mode_attack && this->unk_198 < 10) {
        this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderSph.base);
    }
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->colliderCyl);
    if ((this->colliderCyl.base.acFlags & AC_ON) && this->lightColor.a == 255) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->colliderCyl.base);
    }
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderCyl.base);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->colliderSph.base);
    Actor_world_to_eye(&this->actor, 42.0f);
    if (this->actionFunc != mode_damage && this->actionFunc != mode_attack_diffence) {
        if (this->actionFunc == mode_escape) {
            this->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
        } else {
            this->actor.shape.rot.y = this->actor.world.rot.y;
        }
    }
    checkPos.x = this->actor.world.pos.x;
    checkPos.y = this->actor.world.pos.y + 20.0f;
    checkPos.z = this->actor.world.pos.z;
    this->actor.floorHeight =
        T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &checkPos);
    set_poh_kantera_color_data(this);
    this->actor.shape.shadowAlpha = this->lightColor.a;
}

s32 en_poh_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                           Gfx** gfxP) {
    EnPoh* this = (EnPoh*)thisx;

    if ((this->lightColor.a == 0 || limbIndex == this->info->unk_6) ||
        (this->actionFunc == mode_down && this->unk_198 >= 2)) {
        *dList = NULL;
    } else if (this->actor.params == EN_POH_FLAT && limbIndex == 0xA) {
        // Replace Sharp's head with Flat's
        *dList = gPoeComposerFlatHeadDL;
    }
    if (limbIndex == 0x13 && this->infoIdx == EN_POH_INFO_NORMAL) {
        gDPPipeSync((*gfxP)++);
        gDPSetEnvColor((*gfxP)++, this->lightColor.r, this->lightColor.g, this->lightColor.b, this->lightColor.a);
    }
    return false;
}

void en_poh_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfxP) {
    EnPoh* this = (EnPoh*)thisx;

    CollisionCheck_Uty_convJntSphL2G(limbIndex, &this->colliderSph);
    if (this->actionFunc == mode_down && this->unk_198 >= 2 && limbIndex == this->info->unk_7) {
        MATRIX_FINALIZE_AND_LOAD((*gfxP)++, play->state.gfxCtx, "../z_en_poh.c", 2460);
        gSPDisplayList((*gfxP)++, this->info->burnDisplayList);
    }
    if (limbIndex == this->info->unk_6) {
        if (this->actionFunc == mode_down && this->unk_198 >= 19 && 0.0f != this->actor.scale.x) {
            f32 mtxScale = 0.01f / this->actor.scale.x;
            Matrix_scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
        }
        Matrix_get(&this->unk_368);
        if (this->actionFunc == mode_down && this->unk_198 == 27) {
            this->actor.world.pos.x = this->unk_368.xw;
            this->actor.world.pos.y = this->unk_368.yw;
            this->actor.world.pos.z = this->unk_368.zw;
        }
        Light_point2_ct(&this->lightInfo, this->colliderSph.elements[0].dim.worldSphere.center.x,
                                this->colliderSph.elements[0].dim.worldSphere.center.y,
                                this->colliderSph.elements[0].dim.worldSphere.center.z, this->envColor.r,
                                this->envColor.g, this->envColor.b, this->envColor.a * (200.0f / 255));
    }
}

void En_Poh_actor_draw(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_poh.c", 2629);
    set_poh_color_data(this);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    if (this->lightColor.a == 255 || this->lightColor.a == 0) {
        gDPSetEnvColor(POLY_OPA_DISP++, this->lightColor.r, this->lightColor.g, this->lightColor.b, this->lightColor.a);
        gSPSegment(POLY_OPA_DISP++, 0x08, Actor_change_render_mode + 2);
        POLY_OPA_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       en_poh_display1, en_poh_display2, &this->actor, POLY_OPA_DISP);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->lightColor.a);
        gSPSegment(POLY_XLU_DISP++, 0x08, Actor_change_render_mode);
        POLY_XLU_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       en_poh_display1, en_poh_display2, &this->actor, POLY_XLU_DISP);
    }
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, this->envColor.r, this->envColor.g, this->envColor.b, 255);
    Matrix_put(&this->unk_368);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_poh.c", 2676);
    gSPDisplayList(POLY_OPA_DISP++, this->info->lanternDisplayList);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_poh.c", 2681);
}

void En_Poh_actor_draw2(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;
    Color_RGBA8* sp90;
    Color_RGBA8* phi_t0;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_poh.c", 2694);
    set_poh_color_data(this);
    if (this->actor.params == EN_POH_SHARP) {
        sp90 = &anm_3_env;
        phi_t0 = &anm_4_env;
    } else {
        sp90 = &D_80AE1B50;
        phi_t0 = &D_80AE1B58;
    }
    if (this->lightColor.a == 255 || this->lightColor.a == 0) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gSPSegment(POLY_OPA_DISP++, 0x08,
                   anime_envcolor(play->state.gfxCtx, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                                this->lightColor.a));
        gSPSegment(POLY_OPA_DISP++, 0x0A,
                   anime_envcolor(play->state.gfxCtx, sp90->r, sp90->g, sp90->b, this->lightColor.a));
        gSPSegment(POLY_OPA_DISP++, 0x0B,
                   anime_envcolor(play->state.gfxCtx, phi_t0->r, phi_t0->g, phi_t0->b, this->lightColor.a));
        gSPSegment(POLY_OPA_DISP++, 0x0C, Actor_change_render_mode + 2);
        POLY_OPA_DISP =
            Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               en_poh_display1, en_poh_display2, &this->actor, POLY_OPA_DISP);
    } else {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   anime_envcolor(play->state.gfxCtx, this->lightColor.r, this->lightColor.g, this->lightColor.b,
                                this->lightColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x0A,
                   anime_envcolor(play->state.gfxCtx, sp90->r, sp90->g, sp90->b, this->lightColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x0B,
                   anime_envcolor(play->state.gfxCtx, phi_t0->r, phi_t0->g, phi_t0->b, this->lightColor.a));
        gSPSegment(POLY_XLU_DISP++, 0x0C, Actor_change_render_mode);
        POLY_XLU_DISP =
            Si2_draw2_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                               en_poh_display1, en_poh_display2, &this->actor, POLY_XLU_DISP);
    }
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, this->envColor.r, this->envColor.g, this->envColor.b, 255);
    Matrix_put(&this->unk_368);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_poh.c", 2787);
    gSPDisplayList(POLY_OPA_DISP++, this->info->lanternDisplayList);
    gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternBottomDL);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetEnvColor(POLY_OPA_DISP++, sp90->r, sp90->g, sp90->b, 255);
    gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternTopDL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_poh.c", 2802);
}

void En_Poh_actor_after_move(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;

    this->actionFunc(this, play);
    if (this->actionFunc != mode_kantera_drop) {
        this->visibilityTimer++;
    }
    set_poh_kantera_color_data(this);
}

void En_Poh_actor_after_draw(Actor* thisx, PlayState* play) {
    EnPoh* this = (EnPoh*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_poh.c", 2833);

    if (this->actionFunc == mode_kantera_drop) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        gDPSetEnvColor(POLY_OPA_DISP++, this->envColor.r, this->envColor.g, this->envColor.b, 255);
        Light_point2_ct(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                this->actor.world.pos.z, this->envColor.r, this->envColor.g, this->envColor.b, 200);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_poh.c", 2854);
        gSPDisplayList(POLY_OPA_DISP++, this->info->lanternDisplayList);
        if (this->infoIdx == EN_POH_INFO_COMPOSER) {
            Color_RGBA8* envColor = (this->actor.params == EN_POH_SHARP) ? &anm_3_env : &D_80AE1B50;
            s32 pad;

            gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternBottomDL);
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, envColor->r, envColor->g, envColor->b, 255);
            gSPDisplayList(POLY_OPA_DISP++, gPoeComposerLanternTopDL);
        }
    } else {
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x40, 1, 0,
                                    (this->visibilityTimer * this->info->unk_8) % 512U, 0x20, 0x80));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, this->info->primColor.r, this->info->primColor.g,
                        this->info->primColor.b, this->lightColor.a);
        gDPSetEnvColor(POLY_XLU_DISP++, this->lightColor.r, this->lightColor.g, this->lightColor.b, 255);
        Matrix_rotateY((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000) * 9.58738e-05f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_poh.c", 2910);
        gSPDisplayList(POLY_XLU_DISP++, this->info->soulDisplayList);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_poh.c", 2916);
}
