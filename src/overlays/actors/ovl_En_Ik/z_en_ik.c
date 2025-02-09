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

void EnIk_UpdateEnemy(Actor* thisx, PlayState* play);
void EnIk_DrawEnemy(Actor* thisx, PlayState* play);

void EnIk_SetupStandUp(EnIk* this);
void EnIk_StandUp(EnIk* this, PlayState* play);
void EnIk_Idle(EnIk* this, PlayState* play);
void EnIk_SetupWalkOrRun(EnIk* this);
void EnIk_WalkOrRun(EnIk* this, PlayState* play);
void EnIk_SetupVerticalAttack(EnIk* this);
void EnIk_VerticalAttack(EnIk* this, PlayState* play);
void EnIk_SetupPullOutAxe(EnIk* this);
void EnIk_PullOutAxe(EnIk* this, PlayState* play);
void EnIk_SetupDoubleHorizontalAttack(EnIk* this);
void EnIk_DoubleHorizontalAttack(EnIk* this, PlayState* play);
void EnIk_SetupRecoverFromHorizontalAttack(EnIk* this);
void EnIk_RecoverFromHorizontalAttack(EnIk* this, PlayState* play);
void EnIk_SetupSingleHorizontalAttack(EnIk* this);
void EnIk_SingleHorizontalAttack(EnIk* this, PlayState* play);
void EnIk_SetupStopAndBlock(EnIk* this);
void EnIk_StopAndBlock(EnIk* this, PlayState* play);
void EnIk_ReactToAttack(EnIk* this, PlayState* play);
void EnIk_Die(EnIk* this, PlayState* play);

void EnIk_HandleCsCues(EnIk* this, PlayState* play);
void EnIk_ChangeToEnemy(EnIk* this, PlayState* play);
void EnIk_StartDefeatCutscene(Actor* thisx, PlayState* play);

void EnIk_Destroy(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;

    if (Actor_FindNearby(play, &this->actor, ACTOR_EN_IK, ACTORCAT_ENEMY, 8000.0f) == NULL) {
        func_800F5B58();
    }

    Collider_DestroyTris(play, &this->shieldCollider);
    Collider_DestroyCylinder(play, &this->bodyCollider);
    Collider_DestroyQuad(play, &this->axeCollider);
}

#include "z_en_ik_inFight.inc.c"

#include "z_en_ik_inConfrontion.inc.c"

void EnIk_ChangeToEnemy(EnIk* this, PlayState* play) {
    this->actor.update = EnIk_UpdateEnemy;
    this->actor.draw = EnIk_DrawEnemy;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE;
    SET_EVENTCHKINF(EVENTCHKINF_3B);
    Actor_SetScale(&this->actor, 0.012f);
    EnIk_SetupIdle(this);
}

void EnIk_StartDefeatCutscene(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;

    if (!Play_InCsMode(play)) {
        this->actor.update = EnIk_UpdateCutscene;
        this->actor.draw = EnIk_DrawCutscene;
        Cutscene_SetScript(play, gSpiritBossNabooruKnuckleDefeatCs);
        gSaveContext.cutsceneTrigger = 1;
        Actor_SetScale(&this->actor, 0.01f);
        SET_EVENTCHKINF(EVENTCHKINF_DEFEATED_NABOORU_KNUCKLE);
        EnIk_SetupCsAction3(this, play);
    }
}

void EnIk_Init(Actor* thisx, PlayState* play) {
    EnIk* this = (EnIk*)thisx;
    s32 upperParams = IK_GET_UPPER_PARAMS(&this->actor);

    if (((IK_GET_ARMOR_TYPE(&this->actor) == IK_TYPE_NABOORU) &&
         GET_EVENTCHKINF(EVENTCHKINF_DEFEATED_NABOORU_KNUCKLE)) ||
        (upperParams != 0 && Flags_GetSwitch(play, upperParams >> 8))) {
        Actor_Kill(&this->actor);
    } else {
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
        SkelAnime_InitFlex(play, &this->skelAnime, &gIronKnuckleSkel, &gIronKnuckleNabooruSummonAxeAnim,
                           this->jointTable, this->morphTable, IRON_KNUCKLE_LIMB_MAX);
        EnIk_InitImpl(&this->actor, play);
        EnIk_CsInit(this, play);
    }
}

ActorProfile En_Ik_Profile = {
    /**/ ACTOR_EN_IK,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_IK,
    /**/ sizeof(EnIk),
    /**/ EnIk_Init,
    /**/ EnIk_Destroy,
    /**/ EnIk_UpdateCutscene,
    /**/ EnIk_DrawCutscene,
};
