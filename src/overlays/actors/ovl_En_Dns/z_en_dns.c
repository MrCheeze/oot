/*
 * File: z_en_dns.c
 * Overlay: En_Dns
 * Description: Deku Salesman - Sale Phase
 */

#include "z_en_dns.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Dns_actor_ct(Actor* thisx, PlayState* play);
void En_Dns_actor_dt(Actor* thisx, PlayState* play);
void En_Dns_actor_move(Actor* thisx, PlayState* play);
void En_Dns_actor_draw(Actor* thisx, PlayState* play);

u32 Dns_KakeraCheck(EnDns* this);
u32 Dns_NutsCheck(EnDns* this);
u32 Dns_StickCheck(EnDns* this);
u32 Dns_SeedCheck(EnDns* this);
u32 Dns_ShieldCheck(EnDns* this);
u32 Dns_BombCheck(EnDns* this);
u32 Dns_ArrowCheck(EnDns* this);
u32 Dns_LiquidCheck(EnDns* this);

void Dns_LupyDec(EnDns* this);
void Dns_Nuts(EnDns* this);
void Dns_Kakera(EnDns* this);
void Dns_Bomb(EnDns* this);
void Dns_Arrow(EnDns* this);
void Dns_StickMax(EnDns* this);
void Dns_NutsMax(EnDns* this);

void Dns_enter(EnDns* this, PlayState* play);
void Dns_talk_wait(EnDns* this, PlayState* play);
void Dns_talk_start(EnDns* this, PlayState* play);
void Dns_Carry_Request_Set(EnDns* this, PlayState* play);
void Dns_carryStart(EnDns* this, PlayState* play);
void Dns_carry(EnDns* this, PlayState* play);
void Dns_carry2(EnDns* this, PlayState* play);
void Dns_talk_end(EnDns* this, PlayState* play);
void Dns_exit(EnDns* this, PlayState* play);
void Dns_exit2(EnDns* this, PlayState* play);

ActorProfile En_Dns_Profile = {
    /**/ ACTOR_EN_DNS,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SHOPNUTS,
    /**/ sizeof(EnDns),
    /**/ En_Dns_actor_ct,
    /**/ En_Dns_actor_dt,
    /**/ En_Dns_actor_move,
    /**/ En_Dns_actor_draw,
};

static ColliderCylinderInitType1 DnsPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
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
    { 18, 32, 0, { 0, 0, 0 } },
};

static u16 Dns_First_Message[] = {
    0x10A0, 0x10A1, 0x10A2, 0x10CA, 0x10CB, 0x10CC, 0x10CD, 0x10CE, 0x10CF, 0x10DC, 0x10DD,
};

#if DEBUG_FEATURES
static char* sItemDebugTxt[] = {
    "デクの実売り            ", // "Deku Nuts"
    "デクの棒売り            ", // "Deku Sticks"
    "ハートの欠片売り        ", // "Piece of Heart"
    "デクの種売り            ", // "Deku Seeds"
    "デクの盾売り            ", // "Deku Shield"
    "バクダン売り            ", // "Bombs"
    "矢売り                  ", // "Arrows"
    "赤のくすり売り          ", // "Red Potion"
    "緑のくすり売り          ", // "Green Potion"
    "デクの棒持てる数を増やす", // "Deku Stick Upgrade"
    "デクの実持てる数を増やす", // "Deku Nut Upgrade"
};
#endif

static DnsItemEntry DnsItemNuts = { 20, 5, GI_DEKU_NUTS_5_2, Dns_NutsCheck, Dns_Nuts };
static DnsItemEntry DnsItemStick = { 15, 1, GI_DEKU_STICKS_1, Dns_StickCheck, Dns_LupyDec };
static DnsItemEntry DnsItemKakera = { 10, 1, GI_HEART_PIECE, Dns_KakeraCheck, Dns_Kakera };
static DnsItemEntry DnsItemSeed = { 40, 30, GI_DEKU_SEEDS_30, Dns_SeedCheck, Dns_LupyDec };
static DnsItemEntry DnsItemShield = { 50, 1, GI_SHIELD_DEKU, Dns_ShieldCheck, Dns_LupyDec };
static DnsItemEntry DnsItemBomb = { 40, 5, GI_BOMBS_5, Dns_BombCheck, Dns_Bomb };
static DnsItemEntry DnsItemArrow = { 70, 20, GI_ARROWS_30, Dns_ArrowCheck, Dns_Arrow };
static DnsItemEntry DnsItemLiquidRed = { 40, 1, GI_BOTTLE_POTION_RED, Dns_LiquidCheck, Dns_LupyDec };
static DnsItemEntry DnsItemLiquidGreen = { 40, 1, GI_BOTTLE_POTION_GREEN, Dns_LiquidCheck, Dns_LupyDec };

static DnsItemEntry DnsItemStickMax = { 40, 1, GI_DEKU_STICK_UPGRADE_20, Dns_KakeraCheck,
                                              Dns_StickMax };
static DnsItemEntry DnsItemNutsMax = { 40, 1, GI_DEKU_NUT_UPGRADE_30, Dns_KakeraCheck,
                                            Dns_NutsMax };

static DnsItemEntry* DnsItemP[] = {
    &DnsItemNuts, &DnsItemStick, &DnsItemKakera,  &DnsItemSeed,        &DnsItemShield,     &DnsItemBomb,
    &DnsItemArrow,   &DnsItemLiquidRed,  &DnsItemLiquidGreen, &DnsItemStickMax, &DnsItemNutsMax,
};

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_BUSINESS_SCRUB, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 30, ICHAIN_STOP),
};

static AnimationMinimalInfo animData[] = {
    { &gBusinessScrubNervousIdleAnim, ANIMMODE_LOOP, 0.0f },
    { &gBusinessScrubLeaveBurrowAnim, ANIMMODE_ONCE, 0.0f },
    { &gBusinessScrubNervousTransitionAnim, ANIMMODE_ONCE, 0.0f },
};

void En_Dns_actor_ct(Actor* thisx, PlayState* play) {
    EnDns* this = (EnDns*)thisx;

    if (DNS_GET_TYPE(&this->actor) < 0) {
        // "Function Error (Deku Salesman)"
        PRINTF(VT_FGCOL(RED) "引数エラー（売りナッツ）[ arg_data = %d ]" VT_RST "\n", this->actor.params);
        Actor_delete(&this->actor);
        return;
    }

    // Sell Seeds instead of Arrows if Link is child
    if ((DNS_GET_TYPE(&this->actor) == DNS_TYPE_ARROWS_30) && (LINK_AGE_IN_YEARS == YEARS_CHILD)) {
        DNS_GET_TYPE(&this->actor) = DNS_TYPE_DEKU_SEEDS_30;
    }

    // "Deku Salesman"
    PRINTF(VT_FGCOL(GREEN) "◆◆◆ 売りナッツ『%s』 ◆◆◆" VT_RST "\n", sItemDebugTxt[DNS_GET_TYPE(&this->actor)]);

    ValueSet_process(&this->actor, value_init);

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBusinessScrubSkel, &gBusinessScrubNervousTransitionAnim,
                       this->jointTable, this->morphTable, BUSINESS_SCRUB_LIMB_MAX);

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set3(play, &this->collider, &this->actor, &DnsPipeData);

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 35.0f);
    this->actor.textId = Dns_First_Message[DNS_GET_TYPE(&this->actor)];
    Actor_set_scale(&this->actor, 0.01f);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->isColliderEnabled = true;
    this->standOnGround = true;
    this->dropCollectible = false;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -1.0f;
    this->dnsItemEntry = DnsItemP[DNS_GET_TYPE(&this->actor)];

    this->actionFunc = Dns_enter;
}

void En_Dns_actor_dt(Actor* thisx, PlayState* play) {
    EnDns* this = (EnDns*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void CHG_Dns_Animation(EnDns* this, u8 index) {
    s16 frameCount = Si2_anime_end_frame(animData[index].animation);

    this->animIndex = index;
    Skeleton_Info2_init(&this->skelAnime, animData[index].animation, 1.0f, 0.0f, frameCount,
                     animData[index].mode, animData[index].morphFrames);
}

/* Item give checking functions */

u32 Dns_NutsCheck(EnDns* this) {
    if ((CUR_CAPACITY(UPG_DEKU_NUTS) != 0) && (AMMO(ITEM_DEKU_NUT) >= CUR_CAPACITY(UPG_DEKU_NUTS))) {
        return DNS_CANBUY_RESULT_CAPACITY_FULL;
    }

    if (z_common_data.save.info.playerData.rupees < this->dnsItemEntry->itemPrice) {
        return DNS_CANBUY_RESULT_NEED_RUPEES;
    }

    if (item_get_non_setting(ITEM_DEKU_NUT) == ITEM_NONE) {
        return DNS_CANBUY_RESULT_SUCCESS_NEW_ITEM;
    }

    return DNS_CANBUY_RESULT_SUCCESS;
}

u32 Dns_StickCheck(EnDns* this) {
    if ((CUR_CAPACITY(UPG_DEKU_STICKS) != 0) && (AMMO(ITEM_DEKU_STICK) >= CUR_CAPACITY(UPG_DEKU_STICKS))) {
        return DNS_CANBUY_RESULT_CAPACITY_FULL;
    }

    if (z_common_data.save.info.playerData.rupees < this->dnsItemEntry->itemPrice) {
        return DNS_CANBUY_RESULT_NEED_RUPEES;
    }

    if (item_get_non_setting(ITEM_DEKU_STICK) == ITEM_NONE) {
        return DNS_CANBUY_RESULT_SUCCESS_NEW_ITEM;
    }

    return DNS_CANBUY_RESULT_SUCCESS;
}

u32 Dns_KakeraCheck(EnDns* this) {
    if (z_common_data.save.info.playerData.rupees < this->dnsItemEntry->itemPrice) {
        return DNS_CANBUY_RESULT_NEED_RUPEES;
    }

    return DNS_CANBUY_RESULT_SUCCESS;
}

u32 Dns_SeedCheck(EnDns* this) {
    if (INV_CONTENT(ITEM_SLINGSHOT) == ITEM_NONE) {
        return DNS_CANBUY_RESULT_CANT_GET_NOW;
    }

    if (AMMO(ITEM_SLINGSHOT) >= CUR_CAPACITY(UPG_BULLET_BAG)) {
        return DNS_CANBUY_RESULT_CAPACITY_FULL;
    }

    if (z_common_data.save.info.playerData.rupees < this->dnsItemEntry->itemPrice) {
        return DNS_CANBUY_RESULT_NEED_RUPEES;
    }

    if (item_get_non_setting(ITEM_DEKU_SEEDS) == ITEM_NONE) {
        return DNS_CANBUY_RESULT_SUCCESS_NEW_ITEM;
    }

    return DNS_CANBUY_RESULT_SUCCESS;
}

u32 Dns_ShieldCheck(EnDns* this) {
    if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SHIELD, EQUIP_INV_SHIELD_DEKU)) {
        return DNS_CANBUY_RESULT_CAPACITY_FULL;
    }

    if (z_common_data.save.info.playerData.rupees < this->dnsItemEntry->itemPrice) {
        return DNS_CANBUY_RESULT_NEED_RUPEES;
    }

    return DNS_CANBUY_RESULT_SUCCESS;
}

u32 Dns_BombCheck(EnDns* this) {
    if (!CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return DNS_CANBUY_RESULT_CANT_GET_NOW;
    }

    if (AMMO(ITEM_BOMB) >= CUR_CAPACITY(UPG_BOMB_BAG)) {
        return DNS_CANBUY_RESULT_CAPACITY_FULL;
    }

    if (z_common_data.save.info.playerData.rupees < this->dnsItemEntry->itemPrice) {
        return DNS_CANBUY_RESULT_NEED_RUPEES;
    }

    return DNS_CANBUY_RESULT_SUCCESS;
}

u32 Dns_ArrowCheck(EnDns* this) {
    if (item_get_non_setting(ITEM_BOW) == ITEM_NONE) {
        return DNS_CANBUY_RESULT_CANT_GET_NOW;
    }

    if (AMMO(ITEM_BOW) >= CUR_CAPACITY(UPG_QUIVER)) {
        return DNS_CANBUY_RESULT_CAPACITY_FULL;
    }

    if (z_common_data.save.info.playerData.rupees < this->dnsItemEntry->itemPrice) {
        return DNS_CANBUY_RESULT_NEED_RUPEES;
    }

    return DNS_CANBUY_RESULT_SUCCESS;
}

u32 Dns_LiquidCheck(EnDns* this) {
    if (!findEmptyBottle()) {
        return DNS_CANBUY_RESULT_CAPACITY_FULL;
    }

    if (z_common_data.save.info.playerData.rupees < this->dnsItemEntry->itemPrice) {
        return DNS_CANBUY_RESULT_NEED_RUPEES;
    }

    return DNS_CANBUY_RESULT_SUCCESS;
}

/* Paying and flagging functions */

void Dns_LupyDec(EnDns* this) {
    lupy_increase(-this->dnsItemEntry->itemPrice);
}

void Dns_Nuts(EnDns* this) {
    lupy_increase(-this->dnsItemEntry->itemPrice);
}

void Dns_Kakera(EnDns* this) {
    SET_ITEMGETINF(ITEMGETINF_DEKU_HEART_PIECE);
    lupy_increase(-this->dnsItemEntry->itemPrice);
}

void Dns_Bomb(EnDns* this) {
    lupy_increase(-this->dnsItemEntry->itemPrice);
}

void Dns_Arrow(EnDns* this) {
    lupy_increase(-this->dnsItemEntry->itemPrice);
}

void Dns_StickMax(EnDns* this) {
    SET_INFTABLE(INFTABLE_HAS_DEKU_STICK_UPGRADE);
    lupy_increase(-this->dnsItemEntry->itemPrice);
}

void Dns_NutsMax(EnDns* this) {
    SET_INFTABLE(INFTABLE_HAS_DEKU_NUT_UPGRADE);
    lupy_increase(-this->dnsItemEntry->itemPrice);
}

void Dns_enter(EnDns* this, PlayState* play) {
    if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
        this->actionFunc = Dns_talk_wait;
        CHG_Dns_Animation(this, DNS_ANIM_IDLE);
    }
}

void Dns_talk_wait(EnDns* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 2000, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = Dns_talk_start;
    } else {
        if ((this->collider.base.ocFlags1 & OC1_HIT) || this->actor.isLockedOn) {
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        } else {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        }
        if (this->actor.xzDistToPlayer < 130.0f) {
            Actor_talk_request(&this->actor, play);
        }
    }
}

void Dns_talk_start(EnDns* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // OK
                switch (this->dnsItemEntry->canBuy(this)) {
                    case DNS_CANBUY_RESULT_NEED_RUPEES:
                        message_set2(play, 0x10A5);
                        this->actionFunc = Dns_talk_end;
                        break;

                    case DNS_CANBUY_RESULT_CAPACITY_FULL:
                        message_set2(play, 0x10A6);
                        this->actionFunc = Dns_talk_end;
                        break;

                    case DNS_CANBUY_RESULT_CANT_GET_NOW:
                        message_set2(play, 0x10DE);
                        this->actionFunc = Dns_talk_end;
                        break;

                    case DNS_CANBUY_RESULT_SUCCESS_NEW_ITEM:
                    case DNS_CANBUY_RESULT_SUCCESS:
                        message_set2(play, 0x10A7);
                        this->actionFunc = Dns_carryStart;
                        break;
                }
                break;

            case 1: // "No"
                message_set2(play, 0x10A4);
                this->actionFunc = Dns_talk_end;
        }
    }
}

void Dns_Carry_Request_Set(EnDns* this, PlayState* play) {
    if (DNS_GET_TYPE(&this->actor) == DNS_TYPE_DEKU_STICK_UPGRADE) {
        if (CUR_UPG_VALUE(UPG_DEKU_STICKS) < 2) {
            Actor_carry_request_set2(&this->actor, play, GI_DEKU_STICK_UPGRADE_20, 130.0f, 100.0f);
        } else {
            Actor_carry_request_set2(&this->actor, play, GI_DEKU_STICK_UPGRADE_30, 130.0f, 100.0f);
        }
    } else if (DNS_GET_TYPE(&this->actor) == DNS_TYPE_DEKU_NUT_UPGRADE) {
        if (CUR_UPG_VALUE(UPG_DEKU_NUTS) < 2) {
            Actor_carry_request_set2(&this->actor, play, GI_DEKU_NUT_UPGRADE_30, 130.0f, 100.0f);
        } else {
            Actor_carry_request_set2(&this->actor, play, GI_DEKU_NUT_UPGRADE_40, 130.0f, 100.0f);
        }
    } else {
        Actor_carry_request_set2(&this->actor, play, this->dnsItemEntry->getItemId, 130.0f, 100.0f);
    }
}

void Dns_carryStart(EnDns* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        Dns_Carry_Request_Set(this, play);
        this->actionFunc = Dns_carry;
    }
}

void Dns_carry(EnDns* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = Dns_carry2;
    } else {
        Dns_Carry_Request_Set(this, play);
    }
}

void Dns_carry2(EnDns* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags1 & PLAYER_STATE1_10) {
        if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
            this->dnsItemEntry->payment(this);
            this->dropCollectible = true;
            this->isColliderEnabled = false;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            CHG_Dns_Animation(this, DNS_ANIM_BURROW);
            this->actionFunc = Dns_exit;
        }
    } else {
        this->dnsItemEntry->payment(this);
        this->dropCollectible = true;
        this->isColliderEnabled = false;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        CHG_Dns_Animation(this, DNS_ANIM_BURROW);
        this->actionFunc = Dns_exit;
    }
}

void Dns_talk_end(EnDns* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        this->isColliderEnabled = false;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        CHG_Dns_Animation(this, DNS_ANIM_BURROW);
        this->actionFunc = Dns_exit;
    }
}

void Dns_exit(EnDns* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gBusinessScrubLeaveBurrowAnim);

    if (this->skelAnime.curFrame == frameCount) {
        Actor_SE_set(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
        this->actionFunc = Dns_exit2;
        this->standOnGround = false;
        this->yInitPos = this->actor.world.pos.y;
    }
}

void Dns_exit2(EnDns* this, PlayState* play) {
    f32 depthInGround = this->yInitPos - this->actor.world.pos.y;
    Vec3f initPos;
    s32 i;

    if ((this->dustTimer & 3) == 0) {
        initPos.x = this->actor.world.pos.x;
        initPos.y = this->yInitPos;
        initPos.z = this->actor.world.pos.z;
        Effect_SS_Dust_spread20(play, 20.0f, &initPos);
    }

    this->actor.shape.rot.y += 0x2000;

    if (depthInGround > 400.0f) {
        if (this->dropCollectible) {
            initPos.x = this->actor.world.pos.x;
            initPos.y = this->yInitPos;
            initPos.z = this->actor.world.pos.z;

            for (i = 0; i < 3; i++) {
                Item_set0(play, &initPos, ITEM00_RECOVERY_HEART);
            }
        }
        Actor_delete(&this->actor);
    }
}

void En_Dns_actor_move(Actor* thisx, PlayState* play) {
    EnDns* this = (EnDns*)thisx;
    s16 pad;

    this->dustTimer++;
    this->actor.textId = Dns_First_Message[DNS_GET_TYPE(&this->actor)];

    Actor_world_to_eye(&this->actor, 60.0f);
    Actor_set_scale(&this->actor, 0.01f);
    Skeleton_Info2_anime_play(&this->skelAnime);
    Actor_position_moveF(&this->actor);

    this->actionFunc(this, play);

    if (this->standOnGround) {
        Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 20.0f, UPDBGCHECKINFO_FLAG_2);
    }

    if (this->isColliderEnabled) {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void En_Dns_actor_draw(Actor* thisx, PlayState* play) {
    EnDns* this = (EnDns*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);
}
