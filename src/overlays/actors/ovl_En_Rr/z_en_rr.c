/*
 * File: z_en_rr.c
 * Overlay: ovl_En_Rr
 * Description: Like Like
 */

#include "z_en_rr.h"
#include "assets/objects/object_rr/object_rr.h"
#include "terminal.h"
#include "versions.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

#define RR_MESSAGE_SHIELD (1 << 0)
#define RR_MESSAGE_TUNIC (1 << 1)
#define RR_MOUTH 4
#define RR_BASE 0

typedef enum EnRrReachState {
    /* 0 */ REACH_NONE,
    /* 1 */ REACH_EXTEND,
    /* 2 */ REACH_STOP,
    /* 3 */ REACH_OPEN,
    /* 4 */ REACH_GAPE,
    /* 5 */ REACH_CLOSE
} EnRrReachState;

typedef enum EnRrDamageEffect {
    /* 0x0 */ RR_DMG_NONE,
    /* 0x1 */ RR_DMG_STUN,
    /* 0x2 */ RR_DMG_FIRE,
    /* 0x3 */ RR_DMG_ICE,
    /* 0x4 */ RR_DMG_LIGHT_MAGIC,
    /* 0xB */ RR_DMG_LIGHT_ARROW = 11,
    /* 0xC */ RR_DMG_SHDW_ARROW,
    /* 0xD */ RR_DMG_WIND_ARROW,
    /* 0xE */ RR_DMG_SPRT_ARROW,
    /* 0xF */ RR_DMG_NORMAL
} EnRrDamageEffect;

typedef enum EnRrDropType {
    /* 0 */ RR_DROP_RANDOM_RUPEE,
    /* 1 */ RR_DROP_MAGIC,
    /* 2 */ RR_DROP_ARROW,
    /* 3 */ RR_DROP_FLEXIBLE,
    /* 4 */ RR_DROP_RUPEE_PURPLE,
    /* 5 */ RR_DROP_RUPEE_RED
} EnRrDropType;

void En_Rr_Actor_ct(Actor* thisx, PlayState* play2);
void En_Rr_Actor_dt(Actor* thisx, PlayState* play);
void En_Rr_Actor_move(Actor* thisx, PlayState* play);
void En_Rr_Actor_draw(Actor* thisx, PlayState* play);

void Rr_Uneune_init(EnRr* this, PlayState* play);

void Rr_Damage_Set(EnRr* this);
void Rr_Death_Set(EnRr* this);

void Rr_Normal(EnRr* this, PlayState* play);
void Rr_Search(EnRr* this, PlayState* play);
void Rr_Catch(EnRr* this, PlayState* play);
void Rr_Damage(EnRr* this, PlayState* play);
void Rr_Death(EnRr* this, PlayState* play);
void Rr_Escape(EnRr* this, PlayState* play);
void Rr_Stop(EnRr* this, PlayState* play);

ActorProfile En_Rr_Profile = {
    /**/ ACTOR_EN_RR,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_RR,
    /**/ sizeof(EnRr),
    /**/ En_Rr_Actor_ct,
    /**/ En_Rr_Actor_dt,
    /**/ En_Rr_Actor_move,
    /**/ En_Rr_Actor_draw,
};

#if DEBUG_FEATURES
static char* sDropNames[] = {
    // "type 7", "small magic jar", "arrow", "fairy", "20 rupees", "50 rupees"
    "タイプ７  ", "魔法の壷小", "矢        ", "妖精      ", "20ルピー  ", "50ルピー  ",
};
#endif

static ColliderCylinderInitType1 EnRrOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 30, 55, 0, { 0, 0, 0 } },
};

static ColliderCylinderInitType1 EnRrMouthOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 20, -10, { 0, 0, 0 } },
};

static DamageTable btl_data = {
    /* Deku nut      */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Deku stick    */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Slingshot     */ DMG_ENTRY(1, RR_DMG_NORMAL),
    /* Explosive     */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Boomerang     */ DMG_ENTRY(0, RR_DMG_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Hammer swing  */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Hookshot      */ DMG_ENTRY(0, RR_DMG_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, RR_DMG_NORMAL),
    /* Master sword  */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Giant's Knife */ DMG_ENTRY(4, RR_DMG_NORMAL),
    /* Fire arrow    */ DMG_ENTRY(4, RR_DMG_FIRE),
    /* Ice arrow     */ DMG_ENTRY(4, RR_DMG_ICE),
    /* Light arrow   */ DMG_ENTRY(15, RR_DMG_LIGHT_ARROW),
    /* Unk arrow 1   */ DMG_ENTRY(4, RR_DMG_WIND_ARROW),
    /* Unk arrow 2   */ DMG_ENTRY(15, RR_DMG_SHDW_ARROW),
    /* Unk arrow 3   */ DMG_ENTRY(15, RR_DMG_SPRT_ARROW),
    /* Fire magic    */ DMG_ENTRY(4, RR_DMG_FIRE),
    /* Ice magic     */ DMG_ENTRY(3, RR_DMG_ICE),
    /* Light magic   */ DMG_ENTRY(10, RR_DMG_LIGHT_MAGIC),
    /* Shield        */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, RR_DMG_NORMAL),
    /* Giant spin    */ DMG_ENTRY(4, RR_DMG_NORMAL),
    /* Master spin   */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Kokiri jump   */ DMG_ENTRY(2, RR_DMG_NORMAL),
    /* Giant jump    */ DMG_ENTRY(8, RR_DMG_NORMAL),
    /* Master jump   */ DMG_ENTRY(4, RR_DMG_NORMAL),
    /* Unknown 1     */ DMG_ENTRY(10, RR_DMG_SPRT_ARROW),
    /* Unblockable   */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Hammer jump   */ DMG_ENTRY(0, RR_DMG_NONE),
    /* Unknown 2     */ DMG_ENTRY(0, RR_DMG_NONE),
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_LIKE_LIKE, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

void En_Rr_Actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnRr* this = (EnRr*)thisx;
    s32 i;

    ValueSet_process(&this->actor, value_init);
    this->actor.colChkInfo.damageTable = &btl_data;
    this->actor.colChkInfo.health = 4;
    ClObjPipe_ct(play, &this->collider1);
    ClObjPipe_set3(play, &this->collider1, &this->actor, &EnRrOcInfoData);
    ClObjPipe_ct(play, &this->collider2);
    ClObjPipe_set3(play, &this->collider2, &this->actor, &EnRrMouthOcInfoData);
    Actor_world_to_eye(&this->actor, 30.0f);
    this->actor.scale.y = 0.013f;
    this->actor.scale.x = this->actor.scale.z = 0.014f;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.velocity.y = this->actor.speed = 0.0f;
    this->actor.gravity = -0.4f;
    this->actionTimer = 0;
    this->eatenShield = 0;
    this->eatenTunic = 0;
    this->retreat = false;
    this->grabTimer = 0;
    this->invincibilityTimer = 0;
    this->effectTimer = 0;
    this->hasPlayer = false;
    this->stopScroll = false;
    this->ocTimer = 0;
    this->reachState = this->isDead = false;
    this->actionFunc = Rr_Normal;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].height = this->bodySegs[i].heightTarget = this->bodySegs[i].scaleMod.x =
            this->bodySegs[i].scaleMod.y = this->bodySegs[i].scaleMod.z = 0.0f;
    }
    Rr_Uneune_init(this, play);
}

void En_Rr_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRr* this = (EnRr*)thisx;

    ClObjPipe_dt(play, &this->collider1);
    ClObjPipe_dt(play, &this->collider2);
}

void Rr_Speed_Set(EnRr* this, f32 speedXZ) {
    this->actor.speed = speedXZ;
    Actor_SE_set(&this->actor, NA_SE_EN_LIKE_WALK);
}

void Rr_Search_Set(EnRr* this) {
    static f32 trans_y[] = { 0.0f, 500.0f, 750.0f, 1000.0f, 1000.0f };
    s32 i;

    this->reachState = 1;
    this->actionTimer = 20;
    this->segPhaseVelTarget = 2500.0f;
    this->segMoveRate = 0.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = trans_y[i];
        this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z = 0.8f;
        this->bodySegs[i].rotTarget.x = 6000.0f;
        this->bodySegs[i].rotTarget.z = 0.0f;
    }
    this->actionFunc = Rr_Search;
    Actor_SE_set(&this->actor, NA_SE_EN_LIKE_UNARI);
}

void Rr_Search_Cancel(EnRr* this) {
    s32 i;

    this->reachState = 0;
    this->segMoveRate = 0.0f;
    this->segPhaseVelTarget = 2500.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = 0.0f;
        this->bodySegs[i].rotTarget.x = this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    if (this->retreat) {
        this->actionTimer = 100;
        this->actionFunc = Rr_Escape;
    } else {
        this->actionTimer = 60;
        this->actionFunc = Rr_Normal;
    }
}

void Rr_Catch_Set(EnRr* this, Player* player) {
    s32 i;

    this->grabTimer = 100;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->ocTimer = 8;
    this->hasPlayer = true;
    this->reachState = 0;
    this->segMoveRate = this->swallowOffset = this->actor.speed = 0.0f;
    this->pulseSizeTarget = 0.15f;
    this->segPhaseVelTarget = 5000.0f;
    this->wobbleSizeTarget = 512.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = 0.0f;
        this->bodySegs[i].rotTarget.x = this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    this->actionFunc = Rr_Catch;
    Actor_SE_set(&this->actor, NA_SE_EN_LIKE_DRINK);
}

u8 Rr_Catch_EQUIPMENT(u8 shield, u8 tunic) {
    u8 messageIndex = 0;

    if ((shield == 1 /* Deku shield */) || (shield == 2 /* Hylian shield */)) {
        messageIndex = RR_MESSAGE_SHIELD;
    }
    if ((tunic == 2 /* Goron tunic */) || (tunic == 3 /* Zora tunic */)) {
        messageIndex |= RR_MESSAGE_TUNIC;
    }

    return messageIndex;
}

void Rr_Catch_Cancel(EnRr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u8 shield;
    u8 tunic;

    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->hasPlayer = false;
    this->ocTimer = 110;
    this->segMoveRate = 0.0f;
    this->segPhaseVelTarget = 2500.0f;
    this->wobbleSizeTarget = 2048.0f;
    tunic = 0;
    shield = 0;
    if (CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) != EQUIP_VALUE_SHIELD_MIRROR) {
        shield = ClearEquip_Item(play, EQUIP_TYPE_SHIELD);
        if (shield != 0) {
            this->eatenShield = shield;
            this->retreat = true;
        }
    }
    if (CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC) != EQUIP_VALUE_TUNIC_KOKIRI) {
        tunic = ClearEquip_Item(play, EQUIP_TYPE_TUNIC);
        if (tunic != 0) {
            this->eatenTunic = tunic;
            this->retreat = true;
        }
    }
    player->actor.parent = NULL;
    switch (Rr_Catch_EQUIPMENT(shield, tunic)) {
        case RR_MESSAGE_SHIELD:
            message_set(play, 0x305F, NULL);
            break;
        case RR_MESSAGE_TUNIC:
            message_set(play, 0x3060, NULL);
            break;
        case RR_MESSAGE_TUNIC | RR_MESSAGE_SHIELD:
            message_set(play, 0x3061, NULL);
            break;
    }
    PRINTF(VT_FGCOL(YELLOW) "%s[%d] : Rr_Catch_Cancel" VT_RST "\n", "../z_en_rr.c", 650);
    Actor_player_power_damage_AT_set(play, &this->actor, 4.0f, this->actor.shape.rot.y, 12.0f, 8);
    if (this->actor.colorFilterTimer == 0) {
        this->actionFunc = Rr_Normal;
        Actor_SE_set(&this->actor, NA_SE_EN_LIKE_THROW);
    } else if (this->actor.colChkInfo.health != 0) {
        Rr_Damage_Set(this);
    } else {
        Rr_Death_Set(this);
    }
}

void Rr_Damage_Set(EnRr* this) {
    s32 i;

    this->reachState = 0;
    this->segMoveRate = 0.0f;
    this->actionTimer = 20;
    this->segPhaseVelTarget = 2500.0f;
    this->pulseSizeTarget = 0.0f;
    this->wobbleSizeTarget = 0.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = 0.0f;
        this->bodySegs[i].rotTarget.x = this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    this->actionFunc = Rr_Damage;
    Actor_SE_set(&this->actor, NA_SE_EN_LIKE_DAMAGE);
}

void Rr_Damage_Cancel(EnRr* this) {
    s32 i;

    this->segMoveRate = 0.0f;
    this->pulseSizeTarget = 0.15f;
    this->segPhaseVelTarget = 2500.0f;
    this->wobbleSizeTarget = 2048.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = 0.0f;
        this->bodySegs[i].rotTarget.x = this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    this->actionFunc = Rr_Normal;
}

void Rr_Death_Set(EnRr* this) {
    s32 i;

    this->isDead = true;
    this->frameCount = 0;
    this->shrinkRate = 0.0f;
    this->segMoveRate = 0.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].heightTarget = 0.0f;
        this->bodySegs[i].rotTarget.x = this->bodySegs[i].rotTarget.z = 0.0f;
    }
    this->actionFunc = Rr_Death;
    Actor_SE_set(&this->actor, NA_SE_EN_LIKE_DEAD);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void Rr_Stop_Set(EnRr* this) {
    s32 i;

    this->stopScroll = true;
    this->segMovePhase = 0;
    this->segPhaseVel = 0.0f;
    this->segPhaseVelTarget = 2500.0f;
    this->segPulsePhaseDiff = 0.0f;
    this->segWobblePhaseDiffX = 0.0f;
    this->segWobbleXTarget = 3.0f;
    this->segWobblePhaseDiffZ = 0.0f;
    this->segWobbleZTarget = 1.0f;
    this->pulseSize = 0.0f;
    this->pulseSizeTarget = 0.15f;
    this->wobbleSize = 0.0f;
    this->wobbleSizeTarget = 2048.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].scaleMod.y = 0.0f;
        this->bodySegs[i].rotTarget.x = 0.0f;
        this->bodySegs[i].rotTarget.y = 0.0f;
        this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scale.x = this->bodySegs[i].scale.y = this->bodySegs[i].scale.z =
            this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.y = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    this->actionFunc = Rr_Stop;
}

void Rr_CrossCheck(EnRr* this, PlayState* play) {
    Vec3f hitPos;
    Player* player = GET_PLAYER(play);

    if (this->collider2.base.acFlags & AC_HIT) {
        this->collider2.base.acFlags &= ~AC_HIT;
        // "Kakin" (not sure what this means)
        PRINTF(VT_FGCOL(GREEN) "カキン(%d)！！" VT_RST "\n", this->frameCount);
        hitPos.x = this->collider2.elem.acDmgInfo.hitPos.x;
        hitPos.y = this->collider2.elem.acDmgInfo.hitPos.y;
        hitPos.z = this->collider2.elem.acDmgInfo.hitPos.z;
        CollisionCheckSetSparkFlash(play, &hitPos);
    } else {
        if (this->collider1.base.acFlags & AC_HIT) {
            u8 dropType = RR_DROP_RANDOM_RUPEE;

            this->collider1.base.acFlags &= ~AC_HIT;
            if (this->actor.colChkInfo.damageEffect != 0) {
                hitPos.x = this->collider1.elem.acDmgInfo.hitPos.x;
                hitPos.y = this->collider1.elem.acDmgInfo.hitPos.y;
                hitPos.z = this->collider1.elem.acDmgInfo.hitPos.z;
                CollisionCheck_setBlueBlood(play, NULL, &hitPos);
            }
            switch (this->actor.colChkInfo.damageEffect) {
                case RR_DMG_LIGHT_ARROW:
                    dropType++; // purple rupee
                    FALLTHROUGH;
                case RR_DMG_SHDW_ARROW:
                    dropType++; // flexible
                    FALLTHROUGH;
                case RR_DMG_WIND_ARROW:
                    dropType++; // arrow
                    FALLTHROUGH;
                case RR_DMG_SPRT_ARROW:
                    dropType++; // magic jar
                    FALLTHROUGH;
                case RR_DMG_NORMAL:
                    // "ouch"
                    PRINTF(VT_FGCOL(RED) "いてっ( %d : LIFE %d : DAMAGE %d : %x )！！" VT_RST "\n", this->frameCount,
                           this->actor.colChkInfo.health, this->actor.colChkInfo.damage,
                           this->actor.colChkInfo.damageEffect);
                    this->stopScroll = false;
                    hp_down(&this->actor);
                    this->invincibilityTimer = 40;
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU,
                                         this->invincibilityTimer);
                    if (this->hasPlayer) {
                        Rr_Catch_Cancel(this, play);
                    } else if (this->actor.colChkInfo.health != 0) {
                        Rr_Damage_Set(this);
                    } else {
                        this->dropType = dropType;
                        Rr_Death_Set(this);
                    }
                    return;
                case RR_DMG_FIRE: // Fire Arrow and Din's Fire
                    hp_down(&this->actor);
                    if (this->actor.colChkInfo.health == 0) {
                        this->dropType = RR_DROP_RANDOM_RUPEE;
                    }
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 80);
                    this->effectTimer = 20;
                    Rr_Stop_Set(this);
                    return;
                case RR_DMG_ICE: // Ice Arrow and unused ice magic
                    hp_down(&this->actor);
                    if (this->actor.colChkInfo.health == 0) {
                        this->dropType = RR_DROP_RANDOM_RUPEE;
                    }
#if OOT_VERSION < NTSC_1_1
                    this->effectTimer = 20;
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_XLU, 80);
#else
                    if (this->actor.colorFilterTimer == 0) {
                        this->effectTimer = 20;
                        Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_XLU,
                                             80);
                    }
#endif
                    Rr_Stop_Set(this);
                    return;
                case RR_DMG_LIGHT_MAGIC: // Unused light magic
                    hp_down(&this->actor);
                    if (this->actor.colChkInfo.health == 0) {
                        this->dropType = RR_DROP_RUPEE_RED;
                    }
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_GRAY, 255, COLORFILTER_BUFFLAG_XLU, 80);
                    Rr_Stop_Set(this);
                    return;
                case RR_DMG_STUN: // Boomerang and Hookshot
                    Actor_SE_set(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
                    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_XLU, 80);
                    Rr_Stop_Set(this);
                    return;
            }
        }
        if ((this->ocTimer == 0) && (this->actor.colorFilterTimer == 0) && (player->invincibilityTimer == 0) &&
            !(player->stateFlags2 & PLAYER_STATE2_7) &&
            ((this->collider1.base.ocFlags1 & OC1_HIT) || (this->collider2.base.ocFlags1 & OC1_HIT))) {
            this->collider1.base.ocFlags1 &= ~OC1_HIT;
            this->collider2.base.ocFlags1 &= ~OC1_HIT;
            // "catch"
            PRINTF(VT_FGCOL(GREEN) "キャッチ(%d)！！" VT_RST "\n", this->frameCount);
            if (play->grabPlayer(play, player)) {
                player->actor.parent = &this->actor;
                this->stopScroll = false;
                Rr_Catch_Set(this, player);
            }
        }
    }
}

void Rr_Uneune_init(EnRr* this, PlayState* play) {
    s32 i;

    this->segMovePhase = 0;
    this->segPhaseVel = 0.0f;
    this->segPhaseVelTarget = 2500.0f;
    this->segPulsePhaseDiff = 0.0f;
    this->segWobblePhaseDiffX = 0.0f;
    this->segWobbleXTarget = 3.0f;
    this->segWobblePhaseDiffZ = 0.0f;
    this->segWobbleZTarget = 1.0f;
    this->pulseSize = 0.0f;
    this->pulseSizeTarget = 0.15f;
    this->wobbleSize = 0.0f;
    this->wobbleSizeTarget = 2048.0f;
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].scaleMod.y = 0.0f;
        this->bodySegs[i].rotTarget.x = 0.0f;
        this->bodySegs[i].rotTarget.y = 0.0f;
        this->bodySegs[i].rotTarget.z = 0.0f;
        this->bodySegs[i].scale.x = this->bodySegs[i].scale.y = this->bodySegs[i].scale.z =
            this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.y = this->bodySegs[i].scaleTarget.z = 1.0f;
    }
    for (i = 0; i < 5; i++) {
        this->bodySegs[i].scaleMod.x = this->bodySegs[i].scaleMod.z =
            cos_s(i * (u32)(s16)this->segPulsePhaseDiff * 0x1000) * this->pulseSize;
    }
    for (i = 1; i < 5; i++) {
        this->bodySegs[i].rotTarget.x = cos_s(i * (u32)(s16)this->segWobblePhaseDiffX * 0x1000) * this->wobbleSize;
        this->bodySegs[i].rotTarget.z = sin_s(i * (u32)(s16)this->segWobblePhaseDiffZ * 0x1000) * this->wobbleSize;
    }
}

void Rr_Uneune(EnRr* this, PlayState* play) {
    s32 i;
    s16 phase = this->segMovePhase;

    if (!this->isDead) {
        for (i = 0; i < 5; i++) {
            this->bodySegs[i].scaleMod.x = this->bodySegs[i].scaleMod.z =
                cos_s(phase + i * (s16)this->segPulsePhaseDiff * 0x1000) * this->pulseSize;
        }
        phase = this->segMovePhase;
        if (!this->isDead && (this->reachState == 0)) {
            for (i = 1; i < 5; i++) {
                this->bodySegs[i].rotTarget.x =
                    cos_s(phase + i * (s16)this->segWobblePhaseDiffX * 0x1000) * this->wobbleSize;
                this->bodySegs[i].rotTarget.z =
                    sin_s(phase + i * (s16)this->segWobblePhaseDiffZ * 0x1000) * this->wobbleSize;
            }
        }
    }
    if (!this->stopScroll) {
        this->segMovePhase += (s16)this->segPhaseVel;
    }
}

void Rr_Normal(EnRr* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1F4, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if ((this->actionTimer == 0) && (this->actor.xzDistToPlayer < 160.0f)) {
        Rr_Search_Set(this);
    } else if ((this->actor.xzDistToPlayer < 400.0f) && (this->actor.speed == 0.0f)) {
        Rr_Speed_Set(this, 2.0f);
    }
}

void Rr_Search(EnRr* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1F4, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    switch (this->reachState) {
        case REACH_EXTEND:
            if (this->actionTimer == 0) {
                this->reachState = REACH_STOP;
            }
            break;
        case REACH_STOP:
            if (this->actionTimer == 0) {
                this->actionTimer = 5;
                this->bodySegs[RR_MOUTH].scaleTarget.x = this->bodySegs[RR_MOUTH].scaleTarget.z = 1.5f;
                this->reachState = REACH_OPEN;
            }
            break;
        case REACH_OPEN:
            if (this->actionTimer == 0) {
                this->actionTimer = 2;
                this->bodySegs[RR_MOUTH].heightTarget = 2000.0f;
                this->reachState = REACH_GAPE;
            }
            break;
        case REACH_GAPE:
            if (this->actionTimer == 0) {
                this->actionTimer = 20;
                this->bodySegs[RR_MOUTH].scaleTarget.x = this->bodySegs[RR_MOUTH].scaleTarget.z = 0.8f;
                this->reachState = REACH_CLOSE;
            }
            break;
        case REACH_CLOSE:
            if (this->actionTimer == 0) {
                Rr_Search_Cancel(this);
            }
            break;
    }
}

void Rr_Catch(EnRr* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    z_vibctl2_vib_setQ(this->actor.xyzDistToPlayerSq, 120, 2, 120);
    if ((this->frameCount % 8) == 0) {
        Actor_SE_set(&this->actor, NA_SE_EN_LIKE_EAT);
    }
    this->ocTimer = 8;
    if ((this->grabTimer == 0) || !(player->stateFlags2 & PLAYER_STATE2_7)) {
        Rr_Catch_Cancel(this, play);
    } else {
        add_calc2(&player->actor.world.pos.x, this->mouthPos.x, 1.0f, 30.0f);
        add_calc2(&player->actor.world.pos.y, this->mouthPos.y + this->swallowOffset, 1.0f, 30.0f);
        add_calc2(&player->actor.world.pos.z, this->mouthPos.z, 1.0f, 30.0f);
        add_calc2(&this->swallowOffset, -55.0f, 1.0f, 5.0f);
    }
}

void Rr_Damage(EnRr* this, PlayState* play) {
    s32 i;

    if (this->actor.colorFilterTimer == 0) {
        Rr_Damage_Cancel(this);
    } else if ((this->actor.colorFilterTimer & 8) != 0) {
        for (i = 1; i < 5; i++) {
            this->bodySegs[i].rotTarget.z = 5000.0f;
        }
    } else {
        for (i = 1; i < 5; i++) {
            this->bodySegs[i].rotTarget.z = -5000.0f;
        }
    }
}

void Rr_Death(EnRr* this, PlayState* play) {
    s32 pad;
    s32 i;

    if (this->frameCount < 40) {
        for (i = 0; i < 5; i++) {
            add_calc2(&this->bodySegs[i].heightTarget, i + 59 - (this->frameCount * 25.0f), 1.0f, 50.0f);
            this->bodySegs[i].scaleTarget.x = this->bodySegs[i].scaleTarget.z =
                (SQ(4 - i) * (f32)this->frameCount * 0.003f) + 1.0f;
        }
    } else if (this->frameCount >= 95) {
        Vec3f dropPos;

        dropPos.x = this->actor.world.pos.x;
        dropPos.y = this->actor.world.pos.y;
        dropPos.z = this->actor.world.pos.z;
        switch (this->eatenShield) {
            case 1:
                Item_set0(play, &dropPos, ITEM00_SHIELD_DEKU);
                break;
            case 2:
                Item_set0(play, &dropPos, ITEM00_SHIELD_HYLIAN);
                break;
        }
        switch (this->eatenTunic) {
            case 2:
                Item_set0(play, &dropPos, ITEM00_TUNIC_GORON);
                break;
            case 3:
                Item_set0(play, &dropPos, ITEM00_TUNIC_ZORA);
                break;
        }
        // "dropped"
        PRINTF(VT_FGCOL(GREEN) "「%s」が出た！！" VT_RST "\n", sDropNames[this->dropType]);
        switch (this->dropType) {
            case RR_DROP_MAGIC:
                Item_set0(play, &dropPos, ITEM00_MAGIC_SMALL);
                break;
            case RR_DROP_ARROW:
                Item_set0(play, &dropPos, ITEM00_ARROWS_SINGLE);
                break;
            case RR_DROP_FLEXIBLE:
                Item_set0(play, &dropPos, ITEM00_FLEXIBLE);
                break;
            case RR_DROP_RUPEE_PURPLE:
                Item_set0(play, &dropPos, ITEM00_RUPEE_PURPLE);
                break;
            case RR_DROP_RUPEE_RED:
                Item_set0(play, &dropPos, ITEM00_RUPEE_RED);
                break;
            case RR_DROP_RANDOM_RUPEE:
            default:
                Item_Set_Std(play, &this->actor, &dropPos, 12 << 4);
                break;
        }
        Actor_delete(&this->actor);
    } else if (this->frameCount == 88) {
        Vec3f pos;
        Vec3f vel;
        Vec3f accel;

        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 20.0f;
        pos.z = this->actor.world.pos.z;
        vel.x = 0.0f;
        vel.y = 0.0f;
        vel.z = 0.0f;
        accel.x = 0.0f;
        accel.y = 0.0f;
        accel.z = 0.0f;

        _Effect_SS_Db_ct(play, &pos, &vel, &accel, 100, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, true);
    } else {
        add_calc2(&this->actor.scale.x, 0.0f, 1.0f, this->shrinkRate);
        add_calc2(&this->shrinkRate, 0.001f, 1.0f, 0.00001f);
        this->actor.scale.z = this->actor.scale.x;
    }
}

void Rr_Escape(EnRr* this, PlayState* play) {
    if (this->actionTimer == 0) {
        this->retreat = false;
        this->actionFunc = Rr_Normal;
    } else {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer + 0x8000, 0xA, 0x3E8, 0);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (this->actor.speed == 0.0f) {
            Rr_Speed_Set(this, 2.0f);
        }
    }
}

void Rr_Stop(EnRr* this, PlayState* play) {
    if (this->actor.colorFilterTimer == 0) {
        this->stopScroll = false;
        if (this->hasPlayer) {
            Rr_Catch_Cancel(this, play);
        } else if (this->actor.colChkInfo.health != 0) {
            this->actionFunc = Rr_Normal;
        } else {
            Rr_Death_Set(this);
        }
    }
}

void En_Rr_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRr* this = (EnRr*)thisx;
    s32 i;

    this->frameCount++;
    if (!this->stopScroll) {
        this->scrollTimer++;
    }
    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (this->grabTimer != 0) {
        this->grabTimer--;
    }
    if (this->ocTimer != 0) {
        this->ocTimer--;
    }
    if (this->invincibilityTimer != 0) {
        this->invincibilityTimer--;
    }
    if (this->effectTimer != 0) {
        this->effectTimer--;
    }

    Actor_world_to_eye(&this->actor, 30.0f);
    Rr_Uneune(this, play);
    if (!this->isDead && ((this->actor.colorFilterTimer == 0) || !(this->actor.colorFilterParams & 0x4000))) {
        Rr_CrossCheck(this, play);
    }

    this->actionFunc(this, play);
    if (this->hasPlayer == 0x3F80) { // checks if 1.0f has been stored to hasPlayer's address
        ASSERT(0, "0", "../z_en_rr.c", 1355);
    }

    chase_f(&this->actor.speed, 0.0f, 0.1f);
    Actor_position_moveF(&this->actor);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider1);
    this->collider2.dim.pos.x = this->mouthPos.x;
    this->collider2.dim.pos.y = this->mouthPos.y;
    this->collider2.dim.pos.z = this->mouthPos.z;
    if (!this->isDead && (this->invincibilityTimer == 0)) {
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider2.base);
        if (this->ocTimer == 0) {
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider1.base);
        }
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider2.base);
    } else {
        this->collider2.base.ocFlags1 &= ~OC1_HIT;
        this->collider2.base.acFlags &= ~AC_HIT;
        this->collider1.base.ocFlags1 &= ~OC1_HIT;
        this->collider1.base.acFlags &= ~AC_HIT;
    }
    Actor_BGcheck2(play, &this->actor, 20.0f, 30.0f, 20.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2);
    if (!this->stopScroll) {
        add_calc2(&this->segPhaseVel, this->segPhaseVelTarget, 1.0f, 50.0f);
        add_calc2(&this->segPulsePhaseDiff, 4.0f, 1.0f, 5.0f);
        add_calc2(&this->segWobblePhaseDiffX, this->segWobbleXTarget, 1.0f, 0.04f);
        add_calc2(&this->segWobblePhaseDiffZ, this->segWobbleZTarget, 1.0f, 0.01f);
        add_calc2(&this->pulseSize, this->pulseSizeTarget, 1.0f, 0.0015f);
        add_calc2(&this->wobbleSize, this->wobbleSizeTarget, 1.0f, 20.0f);
        for (i = 0; i < 5; i++) {
            add_calc_short_angle2(&this->bodySegs[i].rot.x, this->bodySegs[i].rotTarget.x, 5, this->segMoveRate * 1000.0f,
                               0);
            add_calc_short_angle2(&this->bodySegs[i].rot.z, this->bodySegs[i].rotTarget.z, 5, this->segMoveRate * 1000.0f,
                               0);
            add_calc2(&this->bodySegs[i].scale.x, this->bodySegs[i].scaleTarget.x, 1.0f, this->segMoveRate * 0.2f);
            this->bodySegs[i].scale.z = this->bodySegs[i].scale.x;
            add_calc2(&this->bodySegs[i].height, this->bodySegs[i].heightTarget, 1.0f, this->segMoveRate * 300.0f);
        }
        add_calc2(&this->segMoveRate, 1.0f, 1.0f, 0.2f);
    }
}

static Vec3f ice_pos[] = {
    { 25.0f, 0.0f, 0.0f },
    { -25.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 25.0f },
    { 0.0f, 0.0f, -25.0f },
};

void En_Rr_Actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    Vec3f zeroVec;
    EnRr* this = (EnRr*)thisx;
    s32 i;
    Mtx* segMtx = GRAPH_ALLOC(play->state.gfxCtx, 4 * sizeof(Mtx));

    OPEN_DISPS(play->state.gfxCtx, "../z_en_rr.c", 1478);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x0C, segMtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (this->scrollTimer * 0) & 0x7F,
                                (this->scrollTimer * 0) & 0x3F, 32, 16, 1, (this->scrollTimer * 0) & 0x3F,
                                (this->scrollTimer * -6) & 0x7F, 32, 16));
    Matrix_push();

    Matrix_scale((1.0f + this->bodySegs[RR_BASE].scaleMod.x) * this->bodySegs[RR_BASE].scale.x,
                 (1.0f + this->bodySegs[RR_BASE].scaleMod.y) * this->bodySegs[RR_BASE].scale.y,
                 (1.0f + this->bodySegs[RR_BASE].scaleMod.z) * this->bodySegs[RR_BASE].scale.z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_rr.c", 1501);
    Matrix_pull();

    zeroVec.x = 0.0f;
    zeroVec.y = 0.0f;
    zeroVec.z = 0.0f;

    for (i = 1; i < 5; i++) {
        Matrix_translate(0.0f, this->bodySegs[i].height + 1000.0f, 0.0f, MTXMODE_APPLY);

        Matrix_rotateXYZ(this->bodySegs[i].rot.x, this->bodySegs[i].rot.y, this->bodySegs[i].rot.z, MTXMODE_APPLY);
        Matrix_push();
        Matrix_scale((1.0f + this->bodySegs[i].scaleMod.x) * this->bodySegs[i].scale.x,
                     (1.0f + this->bodySegs[i].scaleMod.y) * this->bodySegs[i].scale.y,
                     (1.0f + this->bodySegs[i].scaleMod.z) * this->bodySegs[i].scale.z, MTXMODE_APPLY);
        MATRIX_TO_MTX(segMtx, "../z_en_rr.c", 1527);
        Matrix_pull();
        segMtx++;
        Matrix_Position(&zeroVec, &this->effectPos[i]);
    }

    this->effectPos[0] = this->actor.world.pos;
    Matrix_Position(&zeroVec, &this->mouthPos);
    gSPDisplayList(POLY_XLU_DISP++, gLikeLikeDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_rr.c", 1551);

    if (this->effectTimer != 0) {
        Vec3f effectPos;
        s16 effectTimer = this->effectTimer - 1;
        s32 segIndex;
        s32 offIndex;

        this->actor.colorFilterTimer++;
        if ((effectTimer & 1) == 0) {
            segIndex = 4 - (effectTimer >> 2);
            offIndex = (effectTimer >> 1) & 3;

            effectPos.x = this->effectPos[segIndex].x + ice_pos[offIndex].x + rnd_fx(10.0f);
            effectPos.y = this->effectPos[segIndex].y + ice_pos[offIndex].y + rnd_fx(10.0f);
            effectPos.z = this->effectPos[segIndex].z + ice_pos[offIndex].z + rnd_fx(10.0f);
            if (this->actor.colorFilterParams & 0x4000) {
                Effect_En_Fire_ct(play, &this->actor, &effectPos, 100, 0, 0, -1);
            } else {
                Effect_En_Ice_ct0(play, &this->actor, &effectPos, 150, 150, 150, 250, 235, 245, 255, 3.0f);
            }
        }
    }
}
