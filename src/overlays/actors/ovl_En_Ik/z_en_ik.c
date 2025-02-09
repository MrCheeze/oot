/*
 * File: z_en_ik.c
 * Overlay: ovl_En_Ik
 * Description: Iron Knuckle
 */

#include "z_en_ik.h"
#include "assets/scenes/dungeons/jyasinboss/jyasinboss_scene.h"
#include "assets/objects/object_ik/object_ik.h"
#include "terminal.h"
#include "versions.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef void (*EnIkDrawFunc)(struct EnIk*, PlayState*);

#define ARMOR_BROKEN (1 << 0)

typedef enum EnIkCsAction {
    /* 0x00 */ IK_CS_ACTION_0,
    /* 0x01 */ IK_CS_ACTION_1,
    /* 0x02 */ IK_CS_ACTION_2,
    /* 0x03 */ IK_CS_ACTION_3,
    /* 0x04 */ IK_CS_ACTION_4,
    /* 0x05 */ IK_CS_ACTION_5
} EnIkCsAction;

typedef enum EnIkCsDrawMode {
    /* 0x00 */ IK_CS_DRAW_NOTHING,
    /* 0x01 */ IK_CS_DRAW_INTRO,
    /* 0x02 */ IK_CS_DRAW_DEFEAT
} EnIkCsDrawMode;

typedef enum EnIkDamageEffect {
    /* 0x0 */ EN_IK_DMGEFF_NONE,
    /* 0x6 */ EN_IK_DMGEFF_ELEMENTAL_MAGIC = 0x6,
    /* 0xD */ EN_IK_DMGEFF_SPARKS_NO_DMG = 0xD,
    /* 0xE */ EN_IK_DMGEFF_PROJECTILE,
    /* 0xF */ EN_IK_DMGEFF_DAMAGE
} EnIkDamageEffect;

void En_Ik_move(Actor* thisx, PlayState* play);
void En_Ik_display(Actor* thisx, PlayState* play);

static void mode_wait_init(EnIk* this);
static void mode_wait(EnIk* this, PlayState* play);
static void mode_wait2(EnIk* this, PlayState* play);
static void mode_walk_init(EnIk* this);
static void mode_walk(EnIk* this, PlayState* play);
static void mode_attack_init(EnIk* this);
static void mode_attack(EnIk* this, PlayState* play);
void mode_attack_end_init(EnIk* this);
void mode_attack_end(EnIk* this, PlayState* play);
static void mode_attack2_init(EnIk* this);
static void mode_attack2(EnIk* this, PlayState* play);
void mode_attack2_end_init(EnIk* this);
void mode_attack2_end(EnIk* this, PlayState* play);
void mode_attack3_init(EnIk* this);
static void mode_attack3(EnIk* this, PlayState* play);
static void mode_defense_init(EnIk* this);
static void mode_defense(EnIk* this, PlayState* play);
static void mode_dam(EnIk* this, PlayState* play);
static void mode_down(EnIk* this, PlayState* play);

void En_Ik_inConfrontion_Check_DemoMode(EnIk* this, PlayState* play);
void En_Ik_Chenge_DemoToFight(EnIk* this, PlayState* play);
void En_Ik_Chenge_FightToDemo(Actor* thisx, PlayState* play);

void En_Ik_Actor_dt(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;

    if (ActorSearch(play, &this->actor, ACTOR_EN_IK, ACTORCAT_ENEMY, 8000.0f) == NULL) {
        Na_StopMiddleBossBgm();
    }

    ClObjTris_dt_nzf(play, &this->shieldCollider);
    ClObjPipe_dt(play, &this->bodyCollider);
    ClObjSwrd_dt(play, &this->axeCollider);
}

#include "z_en_ik_inFight.inc.c"

#include "z_en_ik_inConfrontion.inc.c"

void En_Ik_Chenge_DemoToFight(EnIk* this, PlayState* play) {
    this->actor.update = En_Ik_move;
    this->actor.draw = En_Ik_display;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE;
    SET_EVENTCHKINF(EVENTCHKINF_3B);
    Actor_set_scale(&this->actor, 0.012f);
    mode_wait2_init(this);
}

void En_Ik_Chenge_FightToDemo(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;

    if (!Game_play_demo_mode_check(play)) {
        this->actor.update = En_Ik_inConfrontion_main;
        this->actor.draw = En_Ik_inConfrontion_draw;
        set_showdata(play, gSpiritBossNabooruKnuckleDefeatCs);
        z_common_data.cutsceneTrigger = 1;
        Actor_set_scale(&this->actor, 0.01f);
        SET_EVENTCHKINF(EVENTCHKINF_DEFEATED_NABOORU_KNUCKLE);
        En_Ik_inConfrontion_setup_Awake(this, play);
    }
}

void En_Ik_Actor_ct(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;
    s32 upperParams = IK_GET_UPPER_PARAMS(&this->actor);

    if (((IK_GET_ARMOR_TYPE(&this->actor) == IK_TYPE_NABOORU) &&
         GET_EVENTCHKINF(EVENTCHKINF_DEFEATED_NABOORU_KNUCKLE)) ||
        (upperParams != 0 && Actor_Environment_sw_Check(play, upperParams >> 8))) {
        Actor_delete(&this->actor);
    } else {
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
        Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gIronKnuckleSkel, &gIronKnuckleNabooruSummonAxeAnim,
                           this->jointTable, this->morphTable, IRON_KNUCKLE_LIMB_MAX);
        En_Ik_inFight_Init(&this->actor, play);
        En_Ik_inConfrontion_Init(this, play);
    }
}

ActorProfile En_Ik_Profile = {
    /**/ ACTOR_EN_IK,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_IK,
    /**/ sizeof(EnIk),
    /**/ En_Ik_Actor_ct,
    /**/ En_Ik_Actor_dt,
    /**/ En_Ik_inConfrontion_main,
    /**/ En_Ik_inConfrontion_draw,
};
