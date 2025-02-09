#include "z_en_ossan.h"
#include "terminal.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_ossan/object_ossan.h"
#include "assets/objects/object_oF1d_map/object_oF1d_map.h"
#include "assets/objects/object_os/object_os.h"
#include "assets/objects/object_zo/object_zo.h"
#include "assets/objects/object_rs/object_rs.h"
#include "assets/objects/object_ds2/object_ds2.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "assets/objects/object_masterkokiri/object_masterkokiri.h"
#include "assets/objects/object_km1/object_km1.h"
#include "assets/objects/object_mastergolon/object_mastergolon.h"
#include "assets/objects/object_masterzoora/object_masterzoora.h"
#include "assets/objects/object_masterkokirihead/object_masterkokirihead.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#if !PLATFORM_GC || OOT_VERSION == HIRATSU3
#define CURSOR_COLOR_R 0
#define CURSOR_COLOR_G 80
#define CURSOR_COLOR_B 255
#define CURSOR_COLOR_A 255
#else
#define CURSOR_COLOR_R 0
#define CURSOR_COLOR_G 255
#define CURSOR_COLOR_B 80
#define CURSOR_COLOR_A 255
#endif

void En_OSSAN_Actor_ct(Actor* thisx, PlayState* play);
void En_OSSAN_Actor_dt(Actor* thisx, PlayState* play);
void En_OSSAN_Actor_move(Actor* thisx, PlayState* play);
void MasterKokiri_draw(Actor* thisx, PlayState* play);
void Obachan_draw(Actor* thisx, PlayState* play);
void Rs_draw(Actor* thisx, PlayState* play);
void En_OSSAN_Actor_draw(Actor* thisx, PlayState* play);
void MasterZoora_draw(Actor* thisx, PlayState* play);
void MasterGolon_draw(Actor* thisx, PlayState* play);
void Os_draw(Actor* thisx, PlayState* play);

void Ossan_init(EnOssan* this, PlayState* play);
void Ossan_move(EnOssan* this, PlayState* play);

void First_MessageSet_Normal(PlayState* play);
void First_MessageSet_Kokiri(PlayState* play);
void First_MessageSet_Drug(PlayState* play);
void First_MessageSet_Night(PlayState* play);
void First_MessageSet_Drug2(PlayState* play);
void First_MessageSet_Nandemo(PlayState* play);
void First_MessageSet_Zoora(PlayState* play);
void First_MessageSet_Goron(PlayState* play);
void First_MessageSet_Mask(PlayState* play);

s16 itemSetNotCheck(s16 v);
s16 RedeadMaskSetCheck(s16 v);
s16 SkjMaskSetCheck(s16 v);
s16 RabitMaskSetCheck(s16 v);
s16 ZooraMaskSetCheck(s16 v);
s16 GolonMaskSetCheck(s16 v);
s16 GerudoMaskSetCheck(s16 v);

void MasterKokiri_dsplay_init(EnOssan* this, PlayState* play);
void Obachan_dsplay_init(EnOssan* this, PlayState* play);
void Rs_dsplay_init(EnOssan* this, PlayState* play);
void Ossan_dsplay_init(EnOssan* this, PlayState* play);
void MasterZoora_dsplay_init(EnOssan* this, PlayState* play);
void MasterGolon_dsplay_init(EnOssan* this, PlayState* play);
void Os_dsplay_init(EnOssan* this, PlayState* play);

void Ossan_TalkStartWait(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellWait(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellWait2(EnOssan* this, PlayState* play, Player* player);
void Ossan_Chatter(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellLeft(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellRight(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellLeftWait(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellRightWait(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellReturn(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellItem(EnOssan* this, PlayState* play2, Player* player);
void Ossan_SellItem2(EnOssan* this, PlayState* play2, Player* player);
void Ossan_SellItem3(EnOssan* this, PlayState* play2, Player* player);
void Ossan_SellItem4(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellItem5(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellCancel(EnOssan* this, PlayState* play, Player* player);
void Ossan_ItemCarry(EnOssan* this, PlayState* play, Player* player);
void Ossan_ItemCarry2(EnOssan* this, PlayState* play, Player* player);
void Ossan_ItemCarryFinish(EnOssan* this, PlayState* play, Player* player);
void Ossan_FirstRonronMilk(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellReturnBomb(EnOssan* this, PlayState* play, Player* player);
void Ossan_OutOfStockBomb(EnOssan* this, PlayState* play, Player* player);
void Ossan_AnikiMessageOK(EnOssan* this, PlayState* play, Player* player);
void Ossan_OutOfStockBombAfter(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellNext(EnOssan* this, PlayState* play, Player* player);
void Ossan_SellMask(EnOssan* this, PlayState* play, Player* player);
void Ossan_TruthMaskPresent(EnOssan* this, PlayState* play, Player* player);
void Ossan_H_Shield_Message(EnOssan* this, PlayState* play, Player* player);

void MasterKokiri_subProc(EnOssan* this, PlayState* play);

void change_OssanWait2(PlayState* play, EnOssan* this);

static void Ossan_glare(EnOssan* this);
static void Ossan_wink(EnOssan* this);

u16 first_talk_message(EnOssan* this);

s32 Ossan_item_zoom_in2(EnOssan* this);
s32 Ossan_item_zoom_out2(EnOssan* this);
void Ossan_item_zoom_reset(EnOssan* this);
void CHG_Ossan_H_Shield_Message(PlayState* play, EnOssan* this);

#define CURSOR_INVALID 0xFF

ActorProfile En_Ossan_Profile = {
    /**/ ACTOR_EN_OSSAN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnOssan),
    /**/ En_OSSAN_Actor_ct,
    /**/ En_OSSAN_Actor_dt,
    /**/ En_OSSAN_Actor_move,
    /**/ NULL,
};

// Unused collider
static ColliderCylinderInitType1 EnOssanOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 30, 80, 0, { 0, 0, 0 } },
};

// Rupees to pay back to Happy Mask Shop
static s16 Mask_Fee[] = { 10, 30, 20, 50 };

// item yaw offsets
static s16 OssanItemAngle[] = { 0xEAAC, 0xEAAC, 0xEAAC, 0xEAAC, 0x1554, 0x1554, 0x1554, 0x1554 };

// unused values?
static s16 KinsutaMSG[] = { 0x001E, 0x001F, 0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025 };

#if DEBUG_FEATURES
static char* sShopkeeperPrintName[] = {
    "コキリの店  ", // "Kokiri Shop"
    "薬屋        ", // "Potion Shop"
    "夜の店      ", // "Night Shop"
    "路地裏の店  ", // "Back Alley Shop"
    "盾の店      ", // "Shield Shop"
    "大人の店    ", // "Adult Shop"
    "タロンの店  ", // "Talon Shop"
    "ゾーラの店  ", // "Zora Shop"
    "ゴロン夜の店", // "Goron Night Shop"
    "インゴーの店", // "Ingo Store"
    "お面屋      ", // "Mask Shop"
};
#endif

typedef struct ShopkeeperObjInfo {
    /* 0x00 */ s16 objId;
    /* 0x02 */ s16 unk_02;
    /* 0x04 */ s16 unk_04;
} ShopkeeperObjInfo;

static s16 MasterBankData[][3] = {
    { OBJECT_KM1, OBJECT_MASTERKOKIRIHEAD, OBJECT_MASTERKOKIRI },
    { OBJECT_DS2, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_RS, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_DS2, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_OSSAN, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_OSSAN, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_OSSAN, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_ZO, OBJECT_ID_MAX, OBJECT_MASTERZOORA },
    { OBJECT_OF1D_MAP, OBJECT_ID_MAX, OBJECT_MASTERGOLON },
    { OBJECT_OSSAN, OBJECT_ID_MAX, OBJECT_ID_MAX },
    { OBJECT_OS, OBJECT_ID_MAX, OBJECT_ID_MAX },
};

static EnOssanTalkOwnerFunc OssanFirstMessage[] = {
    First_MessageSet_Kokiri,       First_MessageSet_Drug, First_MessageSet_Night,
    First_MessageSet_Drug2, First_MessageSet_Nandemo,         First_MessageSet_Normal,
    First_MessageSet_Normal,      First_MessageSet_Zoora,           First_MessageSet_Goron,
    First_MessageSet_Normal,      First_MessageSet_Mask,
};

static f32 oB1_Scale[] = {
    0.01f, 0.011f, 0.0105f, 0.011f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f,
};

typedef struct ShopItem {
    /* 0x00 */ s16 shopItemIndex;
    /* 0x02 */ s16 xOffset;
    /* 0x04 */ s16 yOffset;
    /* 0x06 */ s16 zOffset;
} ShopItem; // size 0x08

static ShopItem OssanItemData[][8] = {
    { { SI_DEKU_SHIELD, 50, 52, -20 },
      { SI_DEKU_NUTS_5, 50, 76, -20 },
      { SI_DEKU_NUTS_10, 80, 52, -3 },
      { SI_DEKU_STICK, 80, 76, -3 },
      { SI_DEKU_SEEDS_30, -50, 52, -20 },
      { SI_ARROWS_10, -50, 76, -20 },
      { SI_ARROWS_30, -80, 52, -3 },
      { SI_RECOVERY_HEART, -80, 76, -3 } },

    { { SI_GREEN_POTION, 50, 52, -20 },
      { SI_BLUE_FIRE, 50, 76, -20 },
      { SI_RED_POTION_R30, 80, 52, -3 },
      { SI_FAIRY, 80, 76, -3 },
      { SI_DEKU_NUTS_5, -50, 52, -20 },
      { SI_BUGS, -50, 76, -20 },
      { SI_POE, -80, 52, -3 },
      { SI_FISH, -80, 76, -3 } },

    { { SI_BOMBCHU_10_2, 50, 52, -20 },
      { SI_BOMBCHU_10_4, 50, 76, -20 },
      { SI_BOMBCHU_10_3, 80, 52, -3 },
      { SI_BOMBCHU_10_1, 80, 76, -3 },
      { SI_BOMBCHU_20_3, -50, 52, -20 },
      { SI_BOMBCHU_20_1, -50, 76, -20 },
      { SI_BOMBCHU_20_4, -80, 52, -3 },
      { SI_BOMBCHU_20_2, -80, 76, -3 } },

    { { SI_GREEN_POTION, 50, 52, -20 },
      { SI_BLUE_FIRE, 50, 76, -20 },
      { SI_RED_POTION_R30, 80, 52, -3 },
      { SI_FAIRY, 80, 76, -3 },
      { SI_DEKU_NUTS_5, -50, 52, -20 },
      { SI_BUGS, -50, 76, -20 },
      { SI_POE, -80, 52, -3 },
      { SI_FISH, -80, 76, -3 } },

    { { SI_HYLIAN_SHIELD, 50, 52, -20 },
      { SI_BOMBS_5_R35, 50, 76, -20 },
      { SI_DEKU_NUTS_5, 80, 52, -3 },
      { SI_RECOVERY_HEART, 80, 76, -3 },
      { SI_ARROWS_10, -50, 52, -20 },
      { SI_ARROWS_50, -50, 76, -20 },
      { SI_DEKU_STICK, -80, 52, -3 },
      { SI_ARROWS_30, -80, 76, -3 } },

    { { SI_HYLIAN_SHIELD, 50, 52, -20 },
      { SI_BOMBS_5_R25, 50, 76, -20 },
      { SI_DEKU_NUTS_5, 80, 52, -3 },
      { SI_RECOVERY_HEART, 80, 76, -3 },
      { SI_ARROWS_10, -50, 52, -20 },
      { SI_ARROWS_50, -50, 76, -20 },
      { SI_DEKU_STICK, -80, 52, -3 },
      { SI_ARROWS_30, -80, 76, -3 } },

    { { SI_MILK_BOTTLE, 50, 52, -20 },
      { SI_DEKU_NUTS_5, 50, 76, -20 },
      { SI_DEKU_NUTS_10, 80, 52, -3 },
      { SI_RECOVERY_HEART, 80, 76, -3 },
      { SI_WEIRD_EGG, -50, 52, -20 },
      { SI_DEKU_STICK, -50, 76, -20 },
      { SI_RECOVERY_HEART, -80, 52, -3 },
      { SI_RECOVERY_HEART, -80, 76, -3 } },

    { { SI_ZORA_TUNIC, 50, 52, -20 },
      { SI_ARROWS_10, 50, 76, -20 },
      { SI_RECOVERY_HEART, 80, 52, -3 },
      { SI_ARROWS_30, 80, 76, -3 },
      { SI_DEKU_NUTS_5, -50, 52, -20 },
      { SI_ARROWS_50, -50, 76, -20 },
      { SI_FISH, -80, 52, -3 },
      { SI_RED_POTION_R50, -80, 76, -3 } },

    { { SI_BOMBS_5_R25, 50, 52, -20 },
      { SI_BOMBS_10, 50, 76, -20 },
      { SI_BOMBS_20, 80, 52, -3 },
      { SI_BOMBS_30, 80, 76, -3 },
      { SI_GORON_TUNIC, -50, 52, -20 },
      { SI_RECOVERY_HEART, -50, 76, -20 },
      { SI_RED_POTION_R40, -80, 52, -3 },
      { SI_RECOVERY_HEART, -80, 76, -3 } },

    { { SI_19, 50, 52, -20 },
      { SI_19, 50, 76, -20 },
      { SI_19, 80, 52, -3 },
      { SI_19, 80, 76, -3 },
      { SI_20, -50, 52, -20 },
      { SI_20, -50, 76, -20 },
      { SI_20, -80, 52, -3 },
      { SI_20, -80, 76, -3 } },

    { { SI_GERUDO_MASK, 50, 52, -20 },
      { SI_ZORA_MASK, 50, 76, -20 },
      { SI_MASK_OF_TRUTH, 80, 52, -3 },
      { SI_GORON_MASK, 80, 76, -3 },
      { SI_SKULL_MASK, -50, 52, -20 },
      { SI_KEATON_MASK, -50, 76, -20 },
      { SI_BUNNY_HOOD, -80, 52, -3 },
      { SI_SPOOKY_MASK, -80, 76, -3 } },
};
static EnOssanGetGirlAParamsFunc oichk[] = {
    itemSetNotCheck,   itemSetNotCheck,    itemSetNotCheck, itemSetNotCheck,
    itemSetNotCheck,   itemSetNotCheck,    itemSetNotCheck, itemSetNotCheck,
    itemSetNotCheck,   itemSetNotCheck,    itemSetNotCheck, itemSetNotCheck,
    itemSetNotCheck,   itemSetNotCheck,    itemSetNotCheck, itemSetNotCheck,
    itemSetNotCheck,   itemSetNotCheck,    itemSetNotCheck, itemSetNotCheck,
    itemSetNotCheck,   itemSetNotCheck,    itemSetNotCheck, itemSetNotCheck,
    itemSetNotCheck,   itemSetNotCheck,    itemSetNotCheck, itemSetNotCheck,
    itemSetNotCheck,   itemSetNotCheck,    itemSetNotCheck, RedeadMaskSetCheck,
    SkjMaskSetCheck, RabitMaskSetCheck,  itemSetNotCheck, ZooraMaskSetCheck,
    GolonMaskSetCheck, GerudoMaskSetCheck, itemSetNotCheck, itemSetNotCheck,
    itemSetNotCheck,   itemSetNotCheck,    itemSetNotCheck, itemSetNotCheck,
    itemSetNotCheck,   itemSetNotCheck,    itemSetNotCheck, itemSetNotCheck,
    itemSetNotCheck,   itemSetNotCheck,
};

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 500, ICHAIN_STOP),
};

// When selecting an item to buy, this is the position the item moves to
static Vec3f zoom_pos[] = { { 17.0f, 58.0f, 30.0f }, { -17.0f, 58.0f, 30.0f } };

static EnOssanInitFunc dispinit[] = {
    MasterKokiri_dsplay_init, Obachan_dsplay_init,    Rs_dsplay_init,
    Obachan_dsplay_init, Ossan_dsplay_init,    Ossan_dsplay_init,
    Ossan_dsplay_init, MasterZoora_dsplay_init,      MasterGolon_dsplay_init,
    Ossan_dsplay_init, Os_dsplay_init,
};

static Vec3f pos_hosei[] = {
    { 0.0f, 0.0f, 33.0f }, { 0.0f, 0.0f, 31.0f }, { 0.0f, 0.0f, 31.0f }, { 0.0f, 0.0f, 31.0f },
    { 0.0f, 0.0f, 0.0f },  { 0.0f, 0.0f, 0.0f },  { 0.0f, 0.0f, 0.0f },  { 0.0f, 0.0f, 36.0f },
    { 0.0f, 0.0f, 15.0f }, { 0.0f, 0.0f, 0.0f },  { 0.0f, 0.0f, 26.0f },
};

static EnOssanStateFunc Ossan_Talk[] = {
    Ossan_TalkStartWait,
    Ossan_SellWait,
    Ossan_SellWait2,
    Ossan_Chatter,
    Ossan_SellLeft,
    Ossan_SellRight,
    Ossan_SellLeftWait,
    Ossan_SellRightWait,
    Ossan_SellReturn,
    Ossan_SellItem,
    Ossan_SellItem2,
    Ossan_SellItem3,
    Ossan_SellItem4,
    Ossan_SellItem5,
    Ossan_SellCancel,
    Ossan_ItemCarry,
    Ossan_ItemCarry2,
    Ossan_ItemCarryFinish,
    Ossan_FirstRonronMilk,
    Ossan_SellReturnBomb,
    Ossan_OutOfStockBomb,
    Ossan_AnikiMessageOK,
    Ossan_OutOfStockBombAfter,
    Ossan_SellNext,
    Ossan_SellMask,
    Ossan_TruthMaskPresent,
    Ossan_H_Shield_Message,
};

void En_OSSAN_actor_set_process(EnOssan* this, EnOssanActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

s16 itemSetNotCheck(s16 v) {
    return v;
}

s16 RedeadMaskSetCheck(s16 v) {
    // Sold Skull Mask
    if (GET_ITEMGETINF(ITEMGETINF_39)) {
        return v;
    }
    return -1;
}

s16 SkjMaskSetCheck(s16 v) {
    // Sold Keaton Mask
    if (GET_ITEMGETINF(ITEMGETINF_38)) {
        return v;
    }
    return -1;
}

s16 RabitMaskSetCheck(s16 v) {
    // Sold Spooky Mask
    if (GET_ITEMGETINF(ITEMGETINF_3A)) {
        return v;
    }
    return -1;
}

s16 ZooraMaskSetCheck(s16 v) {
    // Obtained Mask of Truth
    if (GET_ITEMGETINF(ITEMGETINF_3F)) {
        return v;
    }
    return -1;
}

s16 GolonMaskSetCheck(s16 v) {
    // Obtained Mask of Truth
    if (GET_ITEMGETINF(ITEMGETINF_3F)) {
        return v;
    }
    return -1;
}

s16 GerudoMaskSetCheck(s16 v) {
    // Obtained Mask of Truth
    if (GET_ITEMGETINF(ITEMGETINF_3F)) {
        return v;
    }
    return -1;
}

void OssanItemSet(EnOssan* this, PlayState* play, ShopItem* shopItems) {
    EnTana* shelves;
    s16 itemParams;
    s32 i;

    for (i = 0; i < 8; i++, shopItems++) {
        if (shopItems->shopItemIndex < 0) {
            this->shelfSlots[i] = NULL;
        } else {
            itemParams = oichk[shopItems->shopItemIndex](shopItems->shopItemIndex);

            if (itemParams < 0) {
                this->shelfSlots[i] = NULL;
            } else {
                shelves = this->shelves;
                this->shelfSlots[i] = (EnGirlA*)Actor_info_make_actor(
                    &play->actorCtx, play, ACTOR_EN_GIRLA, shelves->actor.world.pos.x + shopItems->xOffset,
                    shelves->actor.world.pos.y + shopItems->yOffset, shelves->actor.world.pos.z + shopItems->zOffset,
                    shelves->actor.shape.rot.x, shelves->actor.shape.rot.y + OssanItemAngle[i],
                    shelves->actor.shape.rot.z, itemParams);
            }
        }
    }
}

void MaskShopItemSet(EnOssan* this, PlayState* play) {
    s32 i;
    ShopItem* storeItems;
    ShopItem* shopItem;

    if (this->actor.params == OSSAN_TYPE_MASK) {
        storeItems = OssanItemData[this->actor.params];
        if (1) {}
        for (i = 0; i < 8; i++) {
            shopItem = &storeItems[i];
            if (shopItem->shopItemIndex >= 0 && this->shelfSlots[i] == NULL) {
                s16 params = oichk[shopItem->shopItemIndex](shopItem->shopItemIndex);

                if (params >= 0) {
                    this->shelfSlots[i] = (EnGirlA*)Actor_info_make_actor(
                        &play->actorCtx, play, ACTOR_EN_GIRLA, this->shelves->actor.world.pos.x + shopItem->xOffset,
                        this->shelves->actor.world.pos.y + shopItem->yOffset,
                        this->shelves->actor.world.pos.z + shopItem->zOffset, this->shelves->actor.shape.rot.x,
                        this->shelves->actor.shape.rot.y + OssanItemAngle[i], this->shelves->actor.shape.rot.z, params);
                }
            }
        }
    }
}

void First_MessageSet_Normal(PlayState* play) {
    message_set2(play, 0x9E);
}

void First_MessageSet_Drug(PlayState* play) {
    if (play->spawn == 0) {
        message_set2(play, 0x5046);
    } else {
        message_set2(play, 0x504E);
    }
}

void First_MessageSet_Drug2(PlayState* play) {
    message_set2(play, 0x504E);
}

void First_MessageSet_Kokiri(PlayState* play) {
    message_set2(play, 0x10BA);
}

void First_MessageSet_Nandemo(PlayState* play) {
    if (play->spawn == 0) {
        message_set2(play, 0x9D);
    } else {
        message_set2(play, 0x9C);
    }
}

void First_MessageSet_Night(PlayState* play) {
    message_set2(play, 0x7076);
}

void First_MessageSet_Zoora(PlayState* play) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        message_set2(play, 0x403A);
    } else {
        message_set2(play, 0x403B);
    }
}

// Goron City, Goron
void First_MessageSet_Goron(PlayState* play) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        if (GET_EVENTCHKINF(EVENTCHKINF_25)) {
            message_set2(play, 0x3028);
        } else if (CUR_UPG_VALUE(UPG_STRENGTH) != 0) {
            message_set2(play, 0x302D);
        } else {
            message_set2(play, 0x300F);
        }
    } else if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) {
        message_set2(play, 0x3057);
    } else {
        message_set2(play, 0x305B);
    }
}

// Happy Mask Shop
void First_MessageSet_Mask(PlayState* play) {
    if (GET_ITEMGETINF(ITEMGETINF_38)       // Sold Keaton Mask
        && GET_ITEMGETINF(ITEMGETINF_39)    // Sold Skull Mask
        && GET_ITEMGETINF(ITEMGETINF_3A)    // Sold Spooky Mask
        && GET_ITEMGETINF(ITEMGETINF_3B)) { // Sold Bunny Hood
        message_set2(play, 0x70AE);
    } else {
        switch (play->msgCtx.choiceIndex) {
            case 1:
                message_set2(play, 0x70A4);
                break;
            case 0:
                message_set2(play, 0x70A3);
                break;
        }
    }
}

void Set_Ossan_CameraAngle(EnOssan* this, PlayState* play, f32 cameraFaceAngle) {
    this->cameraFaceAngle = cameraFaceAngle;
    setCameraData(GET_ACTIVE_CAM(play), 0xC, NULL, NULL, cameraFaceAngle, 0, 0);
}

s32 Get_Ossan_bank_ID(EnOssan* this, PlayState* play, s16* objectIds) {
    if (objectIds[1] != OBJECT_ID_MAX) {
        this->objectSlot2 = Object_Exchange_bank_check(&play->objectCtx, objectIds[1]);
        if (this->objectSlot2 < 0) {
            return false;
        }
    } else {
        this->objectSlot2 = -1;
    }
    if (objectIds[2] != OBJECT_ID_MAX) {
        this->objectSlot3 = Object_Exchange_bank_check(&play->objectCtx, objectIds[2]);
        if (this->objectSlot3 < 0) {
            return false;
        }
    } else {
        this->objectSlot3 = -1;
    }
    return true;
}

void En_OSSAN_Actor_ct(Actor* thisx, PlayState* play) {
    EnOssan* this = (EnOssan*)thisx;
    s32 pad;
    s16* objectIds;

    if (this->actor.params == OSSAN_TYPE_TALON && (LINK_AGE_IN_YEARS != YEARS_CHILD)) {
        this->actor.params = OSSAN_TYPE_INGO;
    }

    //! @bug This check will always evaluate to false, it should be || not &&
    if (this->actor.params > OSSAN_TYPE_MASK && this->actor.params < OSSAN_TYPE_KOKIRI) {
        Actor_delete(&this->actor);
        PRINTF_COLOR_ERROR();
        PRINTF("引数がおかしいよ(arg_data=%d)！！\n", this->actor.params);
        PRINTF_RST();
        ASSERT(0, "0", "../z_en_oB1.c", 1246);
        return;
    }

    // If you've given Zelda's Letter to the Kakariko Guard
    if (this->actor.params == OSSAN_TYPE_MASK && !GET_INFTABLE(INFTABLE_76)) {
        Actor_delete(&this->actor);
        return;
    }

    if (this->actor.params == OSSAN_TYPE_KAKARIKO_POTION && (LINK_AGE_IN_YEARS == YEARS_CHILD)) {
        Actor_delete(&this->actor);
        return;
    }

    // Completed Dodongo's Cavern
    if (this->actor.params == OSSAN_TYPE_BOMBCHUS && !GET_EVENTCHKINF(EVENTCHKINF_25)) {
        Actor_delete(&this->actor);
        return;
    }

    objectIds = MasterBankData[this->actor.params];
    this->objectSlot1 = Object_Exchange_bank_check(&play->objectCtx, objectIds[0]);

    if (this->objectSlot1 < 0) {
        Actor_delete(&this->actor);
        PRINTF_COLOR_ERROR();
        PRINTF("バンクが無いよ！！(%s)\n", sShopkeeperPrintName[this->actor.params]);
        PRINTF_RST();
        ASSERT(0, "0", "../z_en_oB1.c", 1284);
        return;
    }

    if (Get_Ossan_bank_ID(this, play, objectIds) == 0) {
        Actor_delete(&this->actor);
        PRINTF_COLOR_ERROR();
        PRINTF("予備バンクが無いよ！！(%s)\n", sShopkeeperPrintName[this->actor.params]);
        PRINTF_RST();
        ASSERT(0, "0", "../z_en_oB1.c", 1295);
        return;
    }

    ValueSet_process(&this->actor, value_init);
    En_OSSAN_actor_set_process(this, Ossan_init);
}

void En_OSSAN_Actor_dt(Actor* thisx, PlayState* play) {
    EnOssan* this = (EnOssan*)thisx;
    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjPipe_dt(play, &this->collider);
}

void Ossan_CursolPosition(PlayState* play, EnOssan* this) {
    s16 x;
    s16 y;

    Actor_display_position_set(play, &this->shelfSlots[this->cursorIndex]->actor, &x, &y);
    this->cursorX = x;
    this->cursorY = y;
}

void Ossan_TalkFinish(PlayState* play, EnOssan* this) {
    Player* player = GET_PLAYER(play);

    // "End of conversation!"
    PRINTF(VT_FGCOL(YELLOW) "%s[%d]:★★★ 会話終了！！ ★★★" VT_RST "\n", "../z_en_oB1.c", 1337);
    YREG(31) = 0;
    Actor_talk_check(&this->actor, play);
    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    play->msgCtx.stateTimer = 4;
    player->stateFlags2 &= ~PLAYER_STATE2_29;
    Game_play_pr_vr_switch_pr_set(play, VIEWPOINT_LOCKED);
    alpha_change(HUD_VISIBILITY_ALL);
    this->drawCursor = 0;
    this->stickLeftPrompt.isEnabled = false;
    this->stickRightPrompt.isEnabled = false;
    Set_Ossan_CameraAngle(this, play, 0.0f);
    this->actor.textId = first_talk_message(this);
    this->stateFlag = OSSAN_STATE_IDLE;
}

s32 OssanExitButtonCheck(EnOssan* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        Ossan_TalkFinish(play, this);
        return true;
    } else {
        return false;
    }
}

s32 OssanReturnButtonCheck(EnOssan* this, PlayState* play, Input* input) {
    if (CHECK_BTN_ALL(input->press.button, BTN_B)) {
        this->stateFlag = this->tempStateFlag;
        message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
        return true;
    } else {
        return false;
    }
}

void change_OssanWait(PlayState* play, EnOssan* this, u8 skipHelloState) {
    YREG(31) = 1;
    this->headRot = this->headTargetRot = 0;
    do_action_point_set(play, DO_ACTION_NEXT);
    Set_Ossan_CameraAngle(this, play, 0);

    if (!skipHelloState) {
        this->stateFlag = OSSAN_STATE_START_CONVERSATION;
    } else {
        change_OssanWait2(play, this);
    }
}

void change_OssanWait2(PlayState* play, EnOssan* this) {
    this->stateFlag = OSSAN_STATE_FACING_SHOPKEEPER;

    if (this->actor.params == OSSAN_TYPE_MASK) {
        // if all masks have been sold, give the option to ask about the mask of truth
        if (GET_ITEMGETINF(ITEMGETINF_38) && GET_ITEMGETINF(ITEMGETINF_39) && GET_ITEMGETINF(ITEMGETINF_3A) &&
            GET_ITEMGETINF(ITEMGETINF_3B)) {
            message_set2(play, 0x70AD);
        } else {
            message_set2(play, 0x70A2);
        }
    } else {
        message_set2(play, 0x83);
    }

    do_action_point_set(play, DO_ACTION_DECIDE);
    this->stickRightPrompt.isEnabled = true;
    this->stickLeftPrompt.isEnabled = true;
    Set_Ossan_CameraAngle(this, play, 0.0f);
}

void change_OssanChatter(PlayState* play, EnOssan* this) {
    this->stateFlag = OSSAN_STATE_TALKING_TO_SHOPKEEPER;
    OssanFirstMessage[this->actor.params](play);
    do_action_point_set(play, DO_ACTION_DECIDE);
    this->stickLeftPrompt.isEnabled = false;
    this->stickRightPrompt.isEnabled = false;
}

void change_OssanSellReturn(PlayState* play, EnOssan* this) {
    Na_StartSystemSe_F(NA_SE_SY_CURSOR);
    this->drawCursor = 0;
    this->stateFlag = OSSAN_STATE_LOOK_SHOPKEEPER;
}

void Ossan_TalkStartWait(EnOssan* this, PlayState* play, Player* player) {
    this->headTargetRot = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (Actor_talk_check(&this->actor, play)) {
        // "Start conversation!!"
        PRINTF(VT_FGCOL(YELLOW) "★★★ 会話開始！！ ★★★" VT_RST "\n");
        player->stateFlags2 |= PLAYER_STATE2_29;
        Game_play_shop_pr_vr_switch_set(play);
        change_OssanWait(play, this, false);
    } else if (this->actor.xzDistToPlayer < 100.0f) {
        Actor_talk_request2(&this->actor, play, 100);
    }
}

void Ossan_StickCHK(PlayState* play, EnOssan* this) {
    Input* input = &play->state.input[0];
    s8 stickX = input->rel.stick_x;
    s8 stickY = input->rel.stick_y;

    this->moveHorizontal = this->moveVertical = false;

    if (this->stickAccumX == 0) {
        if (stickX > 30 || stickX < -30) {
            this->stickAccumX = stickX;
            this->moveHorizontal = true;
        }
    } else if (stickX <= 30 && stickX >= -30) {
        this->stickAccumX = 0;
    } else if (this->stickAccumX * stickX < 0) { // Stick has swapped directions
        this->stickAccumX = stickX;
        this->moveHorizontal = true;
    } else {
        this->stickAccumX += stickX;

        if (this->stickAccumX > 2000) {
            this->stickAccumX = 2000;
        } else if (this->stickAccumX < -2000) {
            this->stickAccumX = -2000;
        }
    }

    if (this->stickAccumY == 0) {
        if (stickY > 30 || stickY < -30) {
            this->stickAccumY = stickY;
            this->moveVertical = true;
        }
    } else if (stickY <= 30 && stickY >= -30) {
        this->stickAccumY = 0;
    } else if (this->stickAccumY * stickY < 0) { // Stick has swapped directions
        this->stickAccumY = stickY;
        this->moveVertical = true;
    } else {
        this->stickAccumY += stickY;

        if (this->stickAccumY > 2000) {
            this->stickAccumY = 2000;
        } else if (this->stickAccumY < -2000) {
            this->stickAccumY = -2000;
        }
    }
}

u8 Ossan_CursorIdxCheck(EnOssan* this, u8 shelfOffset) {
    u8 i;

    // if cursor is on the top shelf
    if (this->cursorIndex & 1) {
        // scan top shelf for non-null item
        for (i = shelfOffset + 1; i < shelfOffset + 4; i += 2) {
            if (this->shelfSlots[i] != NULL) {
                return i;
            }
        }
        // scan bottom shelf for non-null item
        for (i = shelfOffset; i < shelfOffset + 4; i += 2) {
            if (this->shelfSlots[i] != NULL) {
                return i;
            }
        }
    } else {
        // scan bottom shelf for non-null item
        for (i = shelfOffset; i < shelfOffset + 4; i += 2) {
            if (this->shelfSlots[i] != NULL) {
                return i;
            }
        }
        // scan top shelf for non-null item
        for (i = shelfOffset + 1; i < shelfOffset + 4; i += 2) {
            if (this->shelfSlots[i] != NULL) {
                return i;
            }
        }
    }
    return CURSOR_INVALID;
}

u8 Ossan_CursorIdxCheck2(EnOssan* this, u8 cursorIndex, u8 shelfSlotMin) {
    u8 c = shelfSlotMin + 4;

    while (cursorIndex >= shelfSlotMin && cursorIndex < c) {
        cursorIndex -= 2;
        if (cursorIndex >= shelfSlotMin && cursorIndex < c) {
            if (this->shelfSlots[cursorIndex] != NULL) {
                return cursorIndex;
            }
        }
    }
    return CURSOR_INVALID;
}

u8 Ossan_CursorIdxCheck3(EnOssan* this, u8 cursorIndex, u8 shelfSlotMax) {

    while (cursorIndex < shelfSlotMax) {
        cursorIndex += 2;
        if ((cursorIndex < shelfSlotMax) && this->shelfSlots[cursorIndex] != NULL) {
            return cursorIndex;
        }
    }
    return CURSOR_INVALID;
}

// pay salesman back
void Mask_Payment(EnOssan* this, PlayState* play) {
    s16 price = Mask_Fee[this->happyMaskShopState];

    if (z_common_data.save.info.playerData.rupees < price) {
        message_set2(play, 0x70A8);
        this->happyMaskShopkeeperEyeIdx = 1;
        this->happyMaskShopState = OSSAN_HAPPY_STATE_ANGRY;
    } else {
        lupy_increase(-price);

        if (this->happyMaskShopState == OSSAN_HAPPY_STATE_REQUEST_PAYMENT_BUNNY_HOOD) {
            SET_EVENTCHKINF(EVENTCHKINF_PAID_BACK_BUNNY_HOOD);
            message_set2(play, 0x70A9);
            this->happyMaskShopState = OSSAN_HAPPY_STATE_ALL_MASKS_SOLD;
            return;
        }

        if (this->happyMaskShopState == OSSAN_HAPPY_STATE_REQUEST_PAYMENT_KEATON_MASK) {
            SET_EVENTCHKINF(EVENTCHKINF_PAID_BACK_KEATON_MASK);
        } else if (this->happyMaskShopState == OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SPOOKY_MASK) {
            SET_EVENTCHKINF(EVENTCHKINF_PAID_BACK_SPOOKY_MASK);
        } else if (this->happyMaskShopState == OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SKULL_MASK) {
            SET_EVENTCHKINF(EVENTCHKINF_PAID_BACK_SKULL_MASK);
        }

        message_set2(play, 0x70A7);
        this->happyMaskShopState = OSSAN_HAPPY_STATE_NONE;
    }
    this->stateFlag = OSSAN_STATE_START_CONVERSATION;
}

void Ossan_SellWait(EnOssan* this, PlayState* play, Player* player) {
    u8 dialogState = message_check(&play->msgCtx);

    if (this->actor.params == OSSAN_TYPE_MASK && dialogState == TEXT_STATE_CHOICE) {
        if (!OssanExitButtonCheck(this, play, &play->state.input[0]) && pad_on_check(play)) {
            s32 pad;

            switch (play->msgCtx.choiceIndex) {
                case 0:
                    change_OssanWait2(play, this);
                    break;
                case 1:
                    Ossan_TalkFinish(play, this);
                    break;
            }
        }
    } else if (dialogState == TEXT_STATE_EVENT && pad_on_check(play)) {
        Na_StartSystemSe_F(NA_SE_SY_MESSAGE_PASS);

        switch (this->happyMaskShopState) {
            case OSSAN_HAPPY_STATE_ALL_MASKS_SOLD:
                message_set2(play, 0x70AA);
                this->stateFlag = OSSAN_STATE_LEND_MASK_OF_TRUTH;
                return;
            case OSSAN_HAPPY_STATE_BORROWED_FIRST_MASK:
                Ossan_TalkFinish(play, this);
                return;
            case OSSAN_HAPPY_STATE_REQUEST_PAYMENT_KEATON_MASK:
            case OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SPOOKY_MASK:
            case OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SKULL_MASK:
            case OSSAN_HAPPY_STATE_REQUEST_PAYMENT_BUNNY_HOOD:
                Mask_Payment(this, play);
                return;
            case OSSAN_HAPPY_STATE_ANGRY:
                play->nextEntranceIndex = ENTR_MARKET_DAY_9;
                play->transitionTrigger = TRANS_TRIGGER_START;
                play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_WHITE, TCS_FAST);
                return;
        }

        if (!OssanExitButtonCheck(this, play, &play->state.input[0])) {
            // "Shop around by moving the stick left and right"
            PRINTF("「スティック左右で品物みてくれ！」\n");
            change_OssanWait2(play, this);
        }
    }

    if (1) {}
}

s32 SelectMenu(EnOssan* this, PlayState* play) {
    switch (play->msgCtx.choiceIndex) {
        case 0:
            change_OssanChatter(play, this);
            return true;
        case 1:
            Ossan_TalkFinish(play, this);
            return true;
        default:
            return false;
    }
}

void Ossan_SellWait2(EnOssan* this, PlayState* play, Player* player) {
    u8 nextIndex;

    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) &&
        !OssanExitButtonCheck(this, play, &play->state.input[0])) {
        if (pad_on_check(play) && SelectMenu(this, play)) {
            Na_StartSystemSe_F(NA_SE_SY_DECIDE);
            return;
        }
        // Stick Left
        if (this->stickAccumX < 0) {
            nextIndex = Ossan_CursorIdxCheck(this, 4);
            if (nextIndex != CURSOR_INVALID) {
                this->cursorIndex = nextIndex;
                this->stateFlag = OSSAN_STATE_LOOK_SHELF_LEFT;
                do_action_point_set(play, DO_ACTION_DECIDE);
                this->stickLeftPrompt.isEnabled = false;
                Na_StartSystemSe_F(NA_SE_SY_CURSOR);
            }
        } else if (this->stickAccumX > 0) {
            nextIndex = Ossan_CursorIdxCheck(this, 0);
            if (nextIndex != CURSOR_INVALID) {
                this->cursorIndex = nextIndex;
                this->stateFlag = OSSAN_STATE_LOOK_SHELF_RIGHT;
                do_action_point_set(play, DO_ACTION_DECIDE);
                this->stickRightPrompt.isEnabled = false;
                Na_StartSystemSe_F(NA_SE_SY_CURSOR);
            }
        }
    }
}

void Ossan_Chatter(EnOssan* this, PlayState* play, Player* player) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        change_OssanWait2(play, this);
    }
}

void Ossan_SellLeft(EnOssan* this, PlayState* play, Player* player) {
    add_calc2(&this->cameraFaceAngle, 30.0f, 0.5f, 10.0f);

    if (this->cameraFaceAngle > 29.5f) {
        Set_Ossan_CameraAngle(this, play, 30.0f);
    }

    Set_Ossan_CameraAngle(this, play, this->cameraFaceAngle);

    if (this->cameraFaceAngle >= 30.0f) {
        Set_Ossan_CameraAngle(this, play, 30.0f);
        Ossan_CursolPosition(play, this);
        this->stateFlag = OSSAN_STATE_BROWSE_LEFT_SHELF;
        message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
    } else {
        this->stickAccumX = 0;
    }
}

void Ossan_SellRight(EnOssan* this, PlayState* play, Player* player) {
    add_calc2(&this->cameraFaceAngle, -30.0f, 0.5f, 10.0f);

    if (this->cameraFaceAngle < -29.5f) {
        Set_Ossan_CameraAngle(this, play, -30.0f);
    }

    Set_Ossan_CameraAngle(this, play, this->cameraFaceAngle);

    if (this->cameraFaceAngle <= -30.0f) {
        Set_Ossan_CameraAngle(this, play, -30.0f);
        Ossan_CursolPosition(play, this);
        this->stateFlag = OSSAN_STATE_BROWSE_RIGHT_SHELF;
        message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
    } else {
        this->stickAccumX = 0;
    }
}

void Ossan_Csl_Hmove(EnOssan* this) {
    u8 curTemp = this->cursorIndex;
    u8 curScanTemp;

    if (this->stickAccumY < 0) {
        curTemp &= 0xFE;
        if (this->shelfSlots[curTemp] != NULL) {
            this->cursorIndex = curTemp;
            return;
        }
        // cursorIndex on right shelf
        if (curTemp < 4) {
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 4) {
                curScanTemp = 0;
            }
            while (curScanTemp != curTemp) {
                if (this->shelfSlots[curScanTemp] != NULL) {
                    this->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 4) {
                    curScanTemp = 0;
                }
            }
        } else {
            // cursorIndex on left shelf
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 8) {
                curScanTemp = 4;
            }
            while (curScanTemp != curTemp) {
                if (this->shelfSlots[curScanTemp] != NULL) {
                    this->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 8) {
                    curScanTemp = 4;
                }
            }
        }
    } else if (this->stickAccumY > 0) {
        curTemp |= 1;
        if (this->shelfSlots[curTemp] != NULL) {
            this->cursorIndex = curTemp;
            return;
        }
        // cursorIndex on right shelf
        if (curTemp < 4) {
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 4) {
                curScanTemp = 1;
            }
            while (curScanTemp != curTemp) {
                if (this->shelfSlots[curScanTemp] != NULL) {
                    this->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 4) {
                    curScanTemp = 1;
                }
            }
        } else {
            // cursorIndex on left shelf
            curScanTemp = curTemp + 2;
            if (curScanTemp >= 8) {
                curScanTemp = 5;
            }
            while (curScanTemp != curTemp) {
                if (this->shelfSlots[curScanTemp] != NULL) {
                    this->cursorIndex = curScanTemp;
                    return;
                }
                curScanTemp += 2;
                if (curScanTemp >= 8) {
                    curScanTemp = 5;
                }
            }
        }
    }
}

s32 Ossan_ItemSelectCHK(PlayState* play, EnOssan* this, Input* input) {
    EnGirlA* selectedItem = this->shelfSlots[this->cursorIndex];

    if (OssanExitButtonCheck(this, play, input)) {
        return true;
    }
    if (pad_on_check(play)) {
        if (selectedItem->actor.params != SI_SOLD_OUT && selectedItem->isInvisible == 0) {
            this->tempStateFlag = this->stateFlag;
            message_set2(play, this->shelfSlots[this->cursorIndex]->itemBuyPromptTextId);
            this->stickLeftPrompt.isEnabled = false;
            this->stickRightPrompt.isEnabled = false;
            switch (selectedItem->actor.params) {
                case SI_KEATON_MASK:
                case SI_SPOOKY_MASK:
                case SI_SKULL_MASK:
                case SI_BUNNY_HOOD:
                case SI_MASK_OF_TRUTH:
                case SI_ZORA_MASK:
                case SI_GORON_MASK:
                case SI_GERUDO_MASK:
                    Na_StartSystemSe_F(NA_SE_SY_DECIDE);
                    this->drawCursor = 0;
                    this->stateFlag = OSSAN_STATE_SELECT_ITEM_MASK;
                    return true;
                case SI_MILK_BOTTLE:
                    Na_StartSystemSe_F(NA_SE_SY_DECIDE);
                    this->drawCursor = 0;
                    this->stateFlag = OSSAN_STATE_SELECT_ITEM_BOTTLE_MILK_FULL;
                    return true;
                case SI_WEIRD_EGG:
                    Na_StartSystemSe_F(NA_SE_SY_DECIDE);
                    this->drawCursor = 0;
                    this->stateFlag = OSSAN_STATE_SELECT_ITEM_WEIRD_EGG;
                    return true;
                case SI_19:
                case SI_20:
                    Na_StartSystemSe_F(NA_SE_SY_ERROR);
                    this->drawCursor = 0;
                    this->stateFlag = OSSAN_STATE_SELECT_ITEM_UNIMPLEMENTED;
                    return true;
                case SI_BOMBS_5_R25:
                case SI_BOMBS_10:
                case SI_BOMBS_20:
                case SI_BOMBS_30:
                case SI_BOMBS_5_R35:
                    Na_StartSystemSe_F(NA_SE_SY_DECIDE);
                    this->drawCursor = 0;
                    this->stateFlag = OSSAN_STATE_SELECT_ITEM_BOMBS;
                    return true;
                default:
                    Na_StartSystemSe_F(NA_SE_SY_DECIDE);
                    this->drawCursor = 0;
                    this->stateFlag = OSSAN_STATE_SELECT_ITEM;
                    return true;
            }
        }
        Na_StartSystemSe_F(NA_SE_SY_ERROR);
        return true;
    }
    return false;
}

void Ossan_SellLeftWait(EnOssan* this, PlayState* play, Player* player) {
    s32 a;
    s32 b;
    u8 prevIndex = this->cursorIndex;
    s32 c;
    s32 d;

    if (!Ossan_item_zoom_out2(this)) {
        PRINTF("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2152);
        this->delayTimer = 3;
        return;
    }
    if (this->delayTimer != 0) {
        this->delayTimer--;
        return;
    }
    this->drawCursor = 0xFF;
    this->stickRightPrompt.isEnabled = true;
    Ossan_CursolPosition(play, this);
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) &&
        !Ossan_ItemSelectCHK(play, this, &play->state.input[0])) {
        if (this->moveHorizontal) {
            if (this->stickAccumX > 0) {
                a = Ossan_CursorIdxCheck2(this, this->cursorIndex, 4);
                if (a != CURSOR_INVALID) {
                    this->cursorIndex = a;
                } else {
                    change_OssanSellReturn(play, this);
                    return;
                }
            } else if (this->stickAccumX < 0) {
                b = Ossan_CursorIdxCheck3(this, this->cursorIndex, 8);
                if (b != CURSOR_INVALID) {
                    this->cursorIndex = b;
                }
            }
        } else {
            if (this->stickAccumX > 0 && this->stickAccumX > 500) {
                c = Ossan_CursorIdxCheck2(this, this->cursorIndex, 4);
                if (c != CURSOR_INVALID) {
                    this->cursorIndex = c;
                } else {
                    change_OssanSellReturn(play, this);
                    return;
                }
            } else if (this->stickAccumX < 0 && this->stickAccumX < -500) {
                d = Ossan_CursorIdxCheck3(this, this->cursorIndex, 8);
                if (d != CURSOR_INVALID) {
                    this->cursorIndex = d;
                }
            }
        }
        Ossan_Csl_Hmove(this);
        if (this->cursorIndex != prevIndex) {
            message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
            Na_StartSystemSe_F(NA_SE_SY_CURSOR);
        }
    }
}

void Ossan_SellRightWait(EnOssan* this, PlayState* play, Player* player) {
    s32 pad[2];
    u8 prevIndex;
    u8 nextIndex;

    prevIndex = this->cursorIndex;
    if (!Ossan_item_zoom_out2(this)) {
        PRINTF("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2244);
        this->delayTimer = 3;
        return;
    }
    if (this->delayTimer != 0) {
        this->delayTimer--;
        return;
    }
    this->drawCursor = 0xFF;
    this->stickLeftPrompt.isEnabled = true;
    Ossan_CursolPosition(play, this);
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) &&
        !Ossan_ItemSelectCHK(play, this, &play->state.input[0])) {
        if (this->moveHorizontal) {
            if (this->stickAccumX < 0) {
                nextIndex = Ossan_CursorIdxCheck2(this, this->cursorIndex, 0);
                if (nextIndex != CURSOR_INVALID) {
                    this->cursorIndex = nextIndex;
                } else {
                    change_OssanSellReturn(play, this);
                    return;
                }
            } else if (this->stickAccumX > 0) {
                nextIndex = Ossan_CursorIdxCheck3(this, this->cursorIndex, 4);
                if (nextIndex != CURSOR_INVALID) {
                    this->cursorIndex = nextIndex;
                }
            }
        } else {
            if (this->stickAccumX < 0 && this->stickAccumX < -500) {
                nextIndex = Ossan_CursorIdxCheck2(this, this->cursorIndex, 0);
                if (nextIndex != CURSOR_INVALID) {
                    this->cursorIndex = nextIndex;
                } else {
                    change_OssanSellReturn(play, this);
                    return;
                }
            } else if (this->stickAccumX > 0 && this->stickAccumX > 500) {
                nextIndex = Ossan_CursorIdxCheck3(this, this->cursorIndex, 4);
                if (nextIndex != CURSOR_INVALID) {
                    this->cursorIndex = nextIndex;
                }
            }
        }
        Ossan_Csl_Hmove(this);
        if (this->cursorIndex != prevIndex) {
            message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
            Na_StartSystemSe_F(NA_SE_SY_CURSOR);
        }
    }
}

void Ossan_SellReturn(EnOssan* this, PlayState* play, Player* player) {
    add_calc2(&this->cameraFaceAngle, 0.0f, 0.5f, 10.0f);
    if ((this->cameraFaceAngle < 0.5f) && (this->cameraFaceAngle > -0.5f)) {
        Set_Ossan_CameraAngle(this, play, 0.0f);
    }
    Set_Ossan_CameraAngle(this, play, this->cameraFaceAngle);
    if (this->cameraFaceAngle == 0.0f) {
        change_OssanWait2(play, this);
    }
}

void Ossan_SellReturnBomb(EnOssan* this, PlayState* play, Player* player) {
    if (!Ossan_item_zoom_out2(this)) {
        PRINTF("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2355);
        return;
    }
    add_calc2(&this->cameraFaceAngle, 0.0f, 0.5f, 10.0f);
    if (this->cameraFaceAngle < 0.5f && this->cameraFaceAngle > -0.5f) {
        Set_Ossan_CameraAngle(this, play, 0.0f);
    }
    Set_Ossan_CameraAngle(this, play, this->cameraFaceAngle);
    if (this->cameraFaceAngle == 0.0f) {
        message_set2(play, 0x3010);
        this->stateFlag = OSSAN_STATE_WAIT_FOR_DISPLAY_ONLY_BOMB_DIALOG;
    }
}

void Ossan_ItemCarryInit(PlayState* play, EnOssan* this) {
    Player* player = GET_PLAYER(play);

    PRINTF("\n" VT_FGCOL(YELLOW) "初めて手にいれた！！" VT_RST "\n\n");
    Actor_carry_request_set2(&this->actor, play, this->shelfSlots[this->cursorIndex]->getItemId, 120.0f, 120.0f);
    play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
    play->msgCtx.stateTimer = 4;
    player->stateFlags2 &= ~PLAYER_STATE2_29;
    Game_play_pr_vr_switch_pr_set(play, VIEWPOINT_LOCKED);
    alpha_change(HUD_VISIBILITY_ALL);
    this->drawCursor = 0;
    Set_Ossan_CameraAngle(this, play, 0.0f);
    this->stateFlag = OSSAN_STATE_GIVE_ITEM_FANFARE;
    PRINTF(VT_FGCOL(YELLOW) "持ち上げ開始！！" VT_RST "\n\n");
}

void CHG_Ossan_Sellcancel(PlayState* play, EnOssan* this, u16 textId) {
    message_set2(play, textId);
    this->stateFlag = OSSAN_STATE_CANT_GET_ITEM;
}

void CHG_Ossan_Sellnext(PlayState* play, EnOssan* this, u16 textId) {
    message_set2(play, textId);
    this->stateFlag = OSSAN_STATE_QUICK_BUY;
}

void Ossan_Utteageru(PlayState* play, EnOssan* this) {
    EnGirlA* selectedItem = this->shelfSlots[this->cursorIndex];

    switch (selectedItem->canBuyFunc(play, selectedItem)) {
        case CANBUY_RESULT_SUCCESS_FANFARE:
            if (selectedItem->actor.params == SI_HYLIAN_SHIELD && GET_INFTABLE(INFTABLE_76)) {
                CHG_Ossan_H_Shield_Message(play, this);
            } else {
                Ossan_ItemCarryInit(play, this);
                this->drawCursor = 0;
                this->shopItemSelectedTween = 0.0f;
                selectedItem->setOutOfStockFunc(play, selectedItem);
            }
            break;
        case CANBUY_RESULT_SUCCESS:
            selectedItem->itemGiveFunc(play, selectedItem);
            CHG_Ossan_Sellnext(play, this, 0x84);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            selectedItem->setOutOfStockFunc(play, selectedItem);
            break;
        case CANBUY_RESULT_CANT_GET_NOW:
            Na_StartSystemSe_F(NA_SE_SY_ERROR);
            CHG_Ossan_Sellcancel(play, this, 0x86);
            break;
        case CANBUY_RESULT_NEED_BOTTLE:
            Na_StartSystemSe_F(NA_SE_SY_ERROR);
            CHG_Ossan_Sellcancel(play, this, 0x96);
            break;
        case CANBUY_RESULT_NEED_RUPEES:
            Na_StartSystemSe_F(NA_SE_SY_ERROR);
            CHG_Ossan_Sellcancel(play, this, 0x85);
            break;
        case CANBUY_RESULT_CANT_GET_NOW_5:
            Na_StartSystemSe_F(NA_SE_SY_ERROR);
            CHG_Ossan_Sellcancel(play, this, 0x86);
            break;
    }
}

void Ossan_UtteageruRonronMilk(PlayState* play, EnOssan* this) {
    EnGirlA* item = this->shelfSlots[this->cursorIndex];

    switch (item->canBuyFunc(play, item)) {
        case CANBUY_RESULT_SUCCESS_FANFARE:
            message_set2(play, 0x9C);
            this->stateFlag = OSSAN_STATE_GIVE_LON_LON_MILK;
            this->drawCursor = 0;
            break;
        case CANBUY_RESULT_SUCCESS:
            item->itemGiveFunc(play, item);
            CHG_Ossan_Sellnext(play, this, 0x98);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item->setOutOfStockFunc(play, item);
            break;
        case CANBUY_RESULT_NEED_BOTTLE:
            CHG_Ossan_Sellcancel(play, this, 0x96);
            break;
        case CANBUY_RESULT_NEED_RUPEES:
            CHG_Ossan_Sellcancel(play, this, 0x85);
            break;
    }
}

void Ossan_UtteageruNiwatoriNoTamago(PlayState* play, EnOssan* this) {
    EnGirlA* item = this->shelfSlots[this->cursorIndex];

    switch (item->canBuyFunc(play, item)) {
        case CANBUY_RESULT_SUCCESS_FANFARE:
            Ossan_ItemCarryInit(play, this);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item->setOutOfStockFunc(play, item);
            break;
        case CANBUY_RESULT_SUCCESS:
            item->itemGiveFunc(play, item);
            CHG_Ossan_Sellnext(play, this, 0x9A);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item->setOutOfStockFunc(play, item);
            break;
        case CANBUY_RESULT_CANT_GET_NOW:
            Na_StartSystemSe_F(NA_SE_SY_ERROR);
            CHG_Ossan_Sellcancel(play, this, 0x9D);
            break;
        case CANBUY_RESULT_NEED_RUPEES:
            Na_StartSystemSe_F(NA_SE_SY_ERROR);
            CHG_Ossan_Sellcancel(play, this, 0x85);
            break;
    }
}

void Ossan_UtteageruBomb2(PlayState* play, EnOssan* this) {
    EnGirlA* item = this->shelfSlots[this->cursorIndex];

    switch (item->canBuyFunc(play, item)) {
        case CANBUY_RESULT_SUCCESS_FANFARE:
        case CANBUY_RESULT_SUCCESS:
            item->itemGiveFunc(play, item);
            CHG_Ossan_Sellnext(play, this, 0x84);
            this->drawCursor = 0;
            this->shopItemSelectedTween = 0.0f;
            item->setOutOfStockFunc(play, item);
            break;
        case CANBUY_RESULT_CANT_GET_NOW:
            Na_StartSystemSe_F(NA_SE_SY_ERROR);
            CHG_Ossan_Sellcancel(play, this, 0x86);
            break;
        case CANBUY_RESULT_NEED_RUPEES:
            Na_StartSystemSe_F(NA_SE_SY_ERROR);
            CHG_Ossan_Sellcancel(play, this, 0x85);
            break;
    }
}

void Ossan_UtteageruBomb(PlayState* play, EnOssan* this) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        if (!GET_EVENTCHKINF(EVENTCHKINF_25)) {
            if (GET_INFTABLE(INFTABLE_FC)) {
                CHG_Ossan_Sellcancel(play, this, 0x302E);
            } else {
                this->stickLeftPrompt.isEnabled = false;
                this->stickRightPrompt.isEnabled = false;
                this->drawCursor = 0;
                this->stateFlag = OSSAN_STATE_DISPLAY_ONLY_BOMB_DIALOG;
            }
        } else {
            Ossan_UtteageruBomb2(play, this);
        }
    } else {
        Ossan_UtteageruBomb2(play, this);
    }
}

void Ossan_SellItem(EnOssan* this, PlayState* play2, Player* player) {
    PlayState* play = play2; // Necessary for OKs

    if (!Ossan_item_zoom_in2(this)) {
        PRINTF("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2654);
        return;
    }
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE &&
        !OssanReturnButtonCheck(this, play, &play->state.input[0]) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                Ossan_Utteageru(play, this);
                break;
            case 1:
                this->stateFlag = this->tempStateFlag;
                message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
                break;
        }
    }
}

void Ossan_SellItem2(EnOssan* this, PlayState* play2, Player* player) {
    PlayState* play = play2; // Need for OK

    if (!Ossan_item_zoom_in2(this)) {
        PRINTF("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2693);
        return;
    }
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE &&
        !OssanReturnButtonCheck(this, play, &play->state.input[0]) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                Ossan_UtteageruRonronMilk(play, this);
                break;
            case 1:
                this->stateFlag = this->tempStateFlag;
                message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
                break;
        }
    }
}

void Ossan_SellItem3(EnOssan* this, PlayState* play2, Player* player) {
    PlayState* play = play2; // Needed for OK

    if (!Ossan_item_zoom_in2(this)) {
        PRINTF("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2732);
        return;
    }
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE &&
        !OssanReturnButtonCheck(this, play, &play->state.input[0]) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                Ossan_UtteageruNiwatoriNoTamago(play, this);
                break;
            case 1:
                this->stateFlag = this->tempStateFlag;
                message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
                break;
        }
    }
}

void Ossan_SellItem4(EnOssan* this, PlayState* play, Player* player) {
    if (!Ossan_item_zoom_in2(this)) {
        PRINTF("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2771);
        return;
    }
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        this->stateFlag = this->tempStateFlag;
        message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
    }
}

void Ossan_SellItem5(EnOssan* this, PlayState* play, Player* player) {
    if (!Ossan_item_zoom_in2(this)) {
        PRINTF("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2798);
        return;
    }
    PRINTF("店主の依頼 ( %d )\n", GET_INFTABLE(INFTABLE_FC));
    if (this->actor.params != OSSAN_TYPE_GORON) {
        Ossan_SellItem(this, play, player);
        return;
    }
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE &&
        !OssanReturnButtonCheck(this, play, &play->state.input[0]) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                Ossan_UtteageruBomb(play, this);
                break;
            case 1:
                this->stateFlag = this->tempStateFlag;
                message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
                break;
        }
    }
}

void Ossan_SellMask(EnOssan* this, PlayState* play, Player* player) {
    u8 talkState = message_check(&play->msgCtx);
    EnGirlA* item = this->shelfSlots[this->cursorIndex];

    if (!Ossan_item_zoom_in2(this)) {
        PRINTF("%s[%d]:" VT_FGCOL(GREEN) "ズーム中！！" VT_RST "\n", "../z_en_oB1.c", 2845);
        return;
    }
    if (talkState == TEXT_STATE_EVENT) {
        if (pad_on_check(play)) {
            this->stateFlag = this->tempStateFlag;
            message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
        }
    } else if (talkState == TEXT_STATE_CHOICE && !OssanReturnButtonCheck(this, play, &play->state.input[0]) &&
               pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                switch (item->actor.params) {
                    case SI_KEATON_MASK:
                        SET_ITEMGETINF(ITEMGETINF_23);
                        break;
                    case SI_SPOOKY_MASK:
                        SET_ITEMGETINF(ITEMGETINF_25);
                        break;
                    case SI_SKULL_MASK:
                        SET_ITEMGETINF(ITEMGETINF_24);
                        break;
                    case SI_BUNNY_HOOD:
                        SET_ITEMGETINF(ITEMGETINF_26);
                        break;
                    case SI_MASK_OF_TRUTH:
                    case SI_ZORA_MASK:
                    case SI_GORON_MASK:
                    case SI_GERUDO_MASK:
                        break;
                }
                Ossan_ItemCarryInit(play, this);
                this->drawCursor = 0;
                this->shopItemSelectedTween = 0.0f;
                item->setOutOfStockFunc(play, item);
                break;
            case 1:
                this->stateFlag = this->tempStateFlag;
                message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
                break;
        }
    }
}

void Ossan_SellCancel(EnOssan* this, PlayState* play, Player* player) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        this->stateFlag = this->tempStateFlag;
        message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
    }
}

void Ossan_SellNext(EnOssan* this, PlayState* play, Player* player) {
    EnGirlA* item;

    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        this->shopItemSelectedTween = 0.0f;
        Ossan_item_zoom_reset(this);
        item = this->shelfSlots[this->cursorIndex];
        item->updateStockedItemFunc(play, item);
        this->stateFlag = this->tempStateFlag;
        message_set2(play, this->shelfSlots[this->cursorIndex]->actor.textId);
    }
}

void Ossan_ItemCarry(EnOssan* this, PlayState* play, Player* player) {
    // The player sets itself as the parent actor to signal that it has obtained the give item request
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->stateFlag = OSSAN_STATE_ITEM_PURCHASED;
        return;
    }
    Actor_carry_request_set2(&this->actor, play, this->shelfSlots[this->cursorIndex]->getItemId, 120.0f, 120.0f);
}

void Ossan_ItemCarry2(EnOssan* this, PlayState* play, Player* player) {
    EnGirlA* item;
    EnGirlA* itemTemp;

    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        if (this->actor.params == OSSAN_TYPE_MASK) {
            itemTemp = this->shelfSlots[this->cursorIndex];
            Ossan_item_zoom_reset(this);
            item = this->shelfSlots[this->cursorIndex];
            item->updateStockedItemFunc(play, item);
            if (itemTemp->actor.params == SI_MASK_OF_TRUTH && !GET_ITEMGETINF(ITEMGETINF_3F)) {
                SET_ITEMGETINF(ITEMGETINF_3F);
                message_set2(play, 0x70AB);
                this->happyMaskShopState = OSSAN_HAPPY_STATE_BORROWED_FIRST_MASK;
                MaskShopItemSet(this, play);
                this->stateFlag = OSSAN_STATE_START_CONVERSATION;
                return;
            } else {
                Ossan_TalkFinish(play, this);
                return;
            }
        }
        item = this->shelfSlots[this->cursorIndex];
        item->buyEventFunc(play, item);
        this->stateFlag = OSSAN_STATE_CONTINUE_SHOPPING_PROMPT;
        message_set2(play, 0x6B);
    }
}

void Ossan_ItemCarryFinish(EnOssan* this, PlayState* play, Player* player) {
    EnGirlA* selectedItem;
    u8 talkState = message_check(&play->msgCtx);

    if (talkState == TEXT_STATE_CHOICE) {
        if (pad_on_check(play)) {
            Ossan_item_zoom_reset(this);
            selectedItem = this->shelfSlots[this->cursorIndex];
            selectedItem->updateStockedItemFunc(play, selectedItem);
            if (!OssanExitButtonCheck(this, play, &play->state.input[0])) {
                s32 pad;

                switch (play->msgCtx.choiceIndex) {
                    case 0:
                        PRINTF(VT_FGCOL(YELLOW) "★★★ 続けるよ！！ ★★★" VT_RST "\n");
                        player->actor.shape.rot.y += 0x8000;
                        player->stateFlags2 |= PLAYER_STATE2_29;
                        Game_play_pr_vr_switch_pr_set(play, VIEWPOINT_PIVOT);
                        message_set(play, this->actor.textId, &this->actor);
                        change_OssanWait(play, this, true);
                        Actor_talk_request_get_item(&this->actor, play, 100.0f, -1);
                        break;
                    case 1:
                    default:
                        PRINTF(VT_FGCOL(YELLOW) "★★★ やめるよ！！ ★★★" VT_RST "\n");
                        Ossan_TalkFinish(play, this);
                        break;
                }
            }
        }
    } else if (talkState == TEXT_STATE_EVENT && pad_on_check(play)) {
        Ossan_item_zoom_reset(this);
        selectedItem = this->shelfSlots[this->cursorIndex];
        selectedItem->updateStockedItemFunc(play, selectedItem);
        player->actor.shape.rot.y += 0x8000;
        player->stateFlags2 |= PLAYER_STATE2_29;
        Game_play_pr_vr_switch_pr_set(play, VIEWPOINT_PIVOT);
        message_set(play, this->actor.textId, &this->actor);
        change_OssanWait(play, this, true);
        Actor_talk_request_get_item(&this->actor, play, 100.0f, -1);
    }
}

void Ossan_OutOfStockBomb(EnOssan* this, PlayState* play, Player* player) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        SET_INFTABLE(INFTABLE_FC);
        change_OssanWait2(play, this);
    }
}

// Unreachable
void Ossan_AnikiMessageOK(EnOssan* this, PlayState* play, Player* player) {
    if (message_check(&play->msgCtx) == TEXT_STATE_DONE_HAS_NEXT && pad_on_check(play)) {
        this->stateFlag = OSSAN_STATE_22;
        message_set2(play, 0x3012);
        SET_INFTABLE(INFTABLE_FC);
    }
}

// Unreachable
void Ossan_OutOfStockBombAfter(EnOssan* this, PlayState* play, Player* player) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        change_OssanWait2(play, this);
    }
}

void Ossan_FirstRonronMilk(EnOssan* this, PlayState* play, Player* player) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        Ossan_ItemCarryInit(play, this);
    }
}

// For giving Mask of Truth when you first sell all masks
void Ossan_TruthMaskPresent(EnOssan* this, PlayState* play, Player* player) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        SET_ITEMGETINF(ITEMGETINF_2A);
        this->cursorIndex = 2;
        Ossan_ItemCarryInit(play, this);
    }
}

// Hylian Shield discount dialog
void CHG_Ossan_H_Shield_Message(PlayState* play, EnOssan* this) {
    message_set2(play, 0x71B2);
    this->stateFlag = OSSAN_STATE_DISCOUNT_DIALOG;
}

void Ossan_H_Shield_Message(EnOssan* this, PlayState* play, Player* player) {
    EnGirlA* selectedItem;

    if (message_check(&play->msgCtx) == TEXT_STATE_DONE && pad_on_check(play)) {
        selectedItem = this->shelfSlots[this->cursorIndex];
        Ossan_ItemCarryInit(play, this);
        this->drawCursor = 0;
        this->shopItemSelectedTween = 0.0f;
        selectedItem->setOutOfStockFunc(play, selectedItem);
    }
}

void Set_item_position(EnOssan* this) {
    EnGirlA* item;
    u8 i;
    u8 i2;
    ShopItem* shopItem;
    f32 tx;
    f32 ty;
    f32 tz;

    i = this->cursorIndex;
    shopItem = &OssanItemData[this->actor.params][i];
    item = this->shelfSlots[i];

    i2 = i >> 2;
    tx = (zoom_pos[i2].x - shopItem->xOffset) * this->shopItemSelectedTween + shopItem->xOffset;
    ty = (zoom_pos[i2].y - shopItem->yOffset) * this->shopItemSelectedTween + shopItem->yOffset;
    tz = (zoom_pos[i2].z - shopItem->zOffset) * this->shopItemSelectedTween + shopItem->zOffset;

    item->actor.world.pos.x = this->shelves->actor.world.pos.x + tx;
    item->actor.world.pos.y = this->shelves->actor.world.pos.y + ty;
    item->actor.world.pos.z = this->shelves->actor.world.pos.z + tz;
}

void Ossan_item_zoom_reset(EnOssan* this) {
    this->shopItemSelectedTween = 0.0f;
    Set_item_position(this);
}

// returns true if animation has completed
s32 Ossan_item_zoom_in2(EnOssan* this) {
    add_calc2(&this->shopItemSelectedTween, 1.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween >= 0.85f) {
        this->shopItemSelectedTween = 1.0f;
    }
    Set_item_position(this);
    if (this->shopItemSelectedTween == 1.0f) {
        return true;
    } else {
        return false;
    }
}

// returns true if animation has completed
s32 Ossan_item_zoom_out2(EnOssan* this) {
    add_calc2(&this->shopItemSelectedTween, 0.0f, 1.0f, 0.15f);
    if (this->shopItemSelectedTween <= 0.15f) {
        this->shopItemSelectedTween = 0.0f;
    }
    Set_item_position(this);
    if (this->shopItemSelectedTween == 0.0f) {
        return true;
    } else {
        return false;
    }
}

void Ossan_ItemRevolveCHK(EnOssan* this) {
    EnGirlA** temp_a1 = this->shelfSlots;
    s32 i;

    for (i = 0; i < 8; i++) {
        if (temp_a1[0] != NULL) {
            if (this->stateFlag != OSSAN_STATE_SELECT_ITEM &&
                this->stateFlag != OSSAN_STATE_SELECT_ITEM_BOTTLE_MILK_FULL &&
                this->stateFlag != OSSAN_STATE_SELECT_ITEM_WEIRD_EGG &&
                this->stateFlag != OSSAN_STATE_SELECT_ITEM_UNIMPLEMENTED &&
                this->stateFlag != OSSAN_STATE_SELECT_ITEM_BOMBS && this->stateFlag != OSSAN_STATE_SELECT_ITEM_MASK &&
                this->stateFlag != OSSAN_STATE_CANT_GET_ITEM && this->drawCursor == 0) {
                temp_a1[0]->isSelected = false;
            } else {
                if (this->cursorIndex == i) {
                    temp_a1[0]->isSelected = true;
                } else {
                    temp_a1[0]->isSelected = false;
                }
            }
        }
        temp_a1++;
    }
}

void Ossan_CursolAnimation(EnOssan* this) {
    f32 t;

    t = this->cursorAnimTween;
    if (this->cursorAnimState == 0) {
        t += 0.05f;
        if (t >= 1.0f) {
            t = 1.0f;
            this->cursorAnimState = 1;
        }
    } else {
        t -= 0.05f;
        if (t <= 0.0f) {
            t = 0.0f;
            this->cursorAnimState = 0;
        }
    }
    this->cursorColorR = ColChanMix(CURSOR_COLOR_R, 0.0f, t);
    this->cursorColorG = ColChanMix(CURSOR_COLOR_G, 80.0f, t);
    this->cursorColorB = ColChanMix(CURSOR_COLOR_B, 0.0f, t);
    this->cursorColorA = ColChanMix(CURSOR_COLOR_A, 0.0f, t);
    this->cursorAnimTween = t;
}

void Ossan_YajirusiAnimation(EnOssan* this) {
    f32 arrowAnimTween;
    f32 new_var3;       // likely fake temp
    s32 new_var2 = 255; // likely fake temp
    f32 stickAnimTween;

    arrowAnimTween = this->arrowAnimTween;
    stickAnimTween = this->stickAnimTween;
    if (this->arrowAnimState == 0) {
        arrowAnimTween += 0.05f;
        if (arrowAnimTween > 1.0f) {
            arrowAnimTween = 1.0f;
            this->arrowAnimState = 1;
        }

    } else {
        arrowAnimTween -= 0.05f;
        if (arrowAnimTween < 0.0f) {
            arrowAnimTween = 0.0f;
            this->arrowAnimState = 0;
        }
    }

    this->arrowAnimTween = arrowAnimTween;
    if (this->stickAnimState == 0) {
        stickAnimTween += 0.1f;
        if (stickAnimTween > 1.0f) {
            stickAnimTween = 1.0f;
            this->stickAnimState = 1;
        }

    } else {
        stickAnimTween = 0.0f;
        this->stickAnimState = 0;
    }

    this->stickAnimTween = stickAnimTween;
    this->stickLeftPrompt.arrowColorR = (u8)(255 - ((s32)(155.0f * arrowAnimTween)));
    this->stickLeftPrompt.arrowColorG = (u8)(new_var2 - (s32)(155.0f * arrowAnimTween));
    new_var3 = (155.0f * arrowAnimTween);
    this->stickLeftPrompt.arrowColorB = (u8)(0 - ((s32)((-100.0f) * arrowAnimTween)));
    this->stickLeftPrompt.arrowColorA = (u8)(200 - ((s32)(50.0f * arrowAnimTween)));
    this->stickRightPrompt.arrowColorR = (u8)(new_var2 - (s32)new_var3);
    this->stickRightPrompt.arrowColorG = (u8)(255 - (s32)new_var3);
    this->stickRightPrompt.arrowColorB = (u8)(0 - ((s32)((-100.0f) * arrowAnimTween)));
    this->stickRightPrompt.arrowColorA = (u8)(200 - ((s32)(50.0f * arrowAnimTween)));
    this->stickRightPrompt.arrowTexX = 290.0f;
    this->stickLeftPrompt.arrowTexX = 33.0f;
    this->stickRightPrompt.stickTexX = 274.0f;
    this->stickLeftPrompt.stickTexX = 49.0f;
    this->stickRightPrompt.stickTexX += (8.0f * stickAnimTween);
    this->stickLeftPrompt.stickTexX -= (8.0f * stickAnimTween);
    this->stickLeftPrompt.arrowTexY = this->stickRightPrompt.arrowTexY = 91.0f;
    this->stickLeftPrompt.stickTexY = this->stickRightPrompt.stickTexY = 95.0f;
}

static void Ossan_glare(EnOssan* this) {
    s16 decr = this->blinkTimer - 1;

    if (decr != 0) {
        this->blinkTimer = decr;
    } else {
        this->blinkFunc = Ossan_wink;
    }
}

static void Ossan_wink(EnOssan* this) {
    s16 decr;
    s16 eyeTextureIdxTemp;

    decr = this->blinkTimer - 1;
    if (decr != 0) {
        this->blinkTimer = decr;
        return;
    }
    eyeTextureIdxTemp = this->eyeTextureIdx + 1;
    if (eyeTextureIdxTemp > 2) {
        this->eyeTextureIdx = 0;
        this->blinkTimer = (s32)(fqrand() * 60.0f) + 20;
        this->blinkFunc = Ossan_glare;
    } else {
        this->eyeTextureIdx = eyeTextureIdxTemp;
        this->blinkTimer = 1;
    }
}

s32 CHK_Exchange_bank_dma(EnOssan* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->objectSlot1)) {
        if (this->objectSlot2 >= 0 && !Object_Exchange_bank_dma_check(&play->objectCtx, this->objectSlot2)) {
            return false;
        }
        if (this->objectSlot3 >= 0 && !Object_Exchange_bank_dma_check(&play->objectCtx, this->objectSlot3)) {
            return false;
        }
        return true;
    }
    return false;
}

void Ossan_dsplay_init(EnOssan* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gObjectOssanSkel, &gObjectOssanAnim_000338, NULL, NULL, 0);
    this->actor.draw = En_OSSAN_Actor_draw;
    this->obj3ToSeg6Func = NULL;
}

void MasterKokiri_dsplay_init(EnOssan* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gKm1Skel, NULL, NULL, NULL, 0);
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot3].segment);
    Skeleton_Info2_init(&this->skelAnime, &object_masterkokiri_Anim_0004A8, 1.0f, 0.0f,
                     Si2_anime_end_frame(&object_masterkokiri_Anim_0004A8), 0, 0.0f);
    this->actor.draw = MasterKokiri_draw;
    this->obj3ToSeg6Func = MasterKokiri_subProc;
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_ELF, this->actor.world.pos.x,
                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, FAIRY_KOKIRI);
}

void MasterGolon_dsplay_init(EnOssan* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGoronSkel, NULL, NULL, NULL, 0);
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot3].segment);
    Skeleton_Info2_init(&this->skelAnime, &gGoronShopkeeperAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gGoronShopkeeperAnim),
                     0, 0.0f);
    this->actor.draw = MasterGolon_draw;
    this->obj3ToSeg6Func = MasterKokiri_subProc;
}

void MasterZoora_dsplay_init(EnOssan* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gZoraSkel, NULL, NULL, NULL, 0);
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot3].segment);
    Skeleton_Info2_init(&this->skelAnime, &gZoraShopkeeperAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gZoraShopkeeperAnim),
                     0, 0.0f);
    this->actor.draw = MasterZoora_draw;
    this->obj3ToSeg6Func = MasterKokiri_subProc;
}

void Obachan_dsplay_init(EnOssan* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_ds2_Skel_004258, &object_ds2_Anim_0002E4, NULL, NULL, 0);
    this->actor.draw = Obachan_draw;
    this->obj3ToSeg6Func = NULL;
}

void Os_dsplay_init(EnOssan* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gHappyMaskSalesmanSkel, &gHappyMaskSalesmanIdleAnim, NULL, NULL, 0);
    this->actor.draw = Os_draw;
    this->obj3ToSeg6Func = NULL;
}

void Rs_dsplay_init(EnOssan* this, PlayState* play) {
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBombchuShopkeeperSkel, &gBombchuShopkeeperIdleAnim, NULL, NULL, 0);
    this->actor.draw = Rs_draw;
    this->obj3ToSeg6Func = NULL;
}

u16 first_talk_message(EnOssan* this) {
    this->happyMaskShopState = OSSAN_HAPPY_STATE_NONE;
    // mask shop messages
    if (this->actor.params == OSSAN_TYPE_MASK) {
        if (INV_CONTENT(ITEM_TRADE_CHILD) == ITEM_SOLD_OUT) {
            if (GET_ITEMGETINF(ITEMGETINF_3B)) {
                if (!GET_EVENTCHKINF(EVENTCHKINF_PAID_BACK_BUNNY_HOOD)) {
                    // Pay back Bunny Hood
                    this->happyMaskShopState = OSSAN_HAPPY_STATE_REQUEST_PAYMENT_BUNNY_HOOD;
                    return 0x70C6;
                } else {
                    return 0x70AC;
                }
            }
            if (GET_ITEMGETINF(ITEMGETINF_3A)) {
                if (!GET_EVENTCHKINF(EVENTCHKINF_PAID_BACK_SPOOKY_MASK)) {
                    // Pay back Spooky Mask
                    this->happyMaskShopState = OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SPOOKY_MASK;
                    return 0x70C5;
                } else {
                    return 0x70AC;
                }
            }
            if (GET_ITEMGETINF(ITEMGETINF_39)) {
                if (!GET_EVENTCHKINF(EVENTCHKINF_PAID_BACK_SKULL_MASK)) {
                    // Pay back Skull Mask
                    this->happyMaskShopState = OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SKULL_MASK;
                    return 0x70C4;
                } else {
                    return 0x70AC;
                }
            }
            if (GET_ITEMGETINF(ITEMGETINF_38)) {
                if (!GET_EVENTCHKINF(EVENTCHKINF_PAID_BACK_KEATON_MASK)) {
                    // Pay back Keaton Mask
                    this->happyMaskShopState = OSSAN_HAPPY_STATE_REQUEST_PAYMENT_KEATON_MASK;
                    return 0x70A5;
                } else {
                    return 0x70AC;
                }
            }
        } else {
            if (GET_ITEMGETINF(ITEMGETINF_3B)) {
                return 0x70AC;
            } else if (!GET_ITEMGETINF(ITEMGETINF_3A) && !GET_ITEMGETINF(ITEMGETINF_24) &&
                       !GET_ITEMGETINF(ITEMGETINF_38)) {
                // Haven't borrowed the Keaton Mask
                if (!GET_ITEMGETINF(ITEMGETINF_23)) {
                    return 0x70A1;
                } else {
                    // Haven't sold the Keaton Mask
                    this->happyMaskShopState = OSSAN_HAPPY_STATE_BORROWED_FIRST_MASK;
                    return 0x70A6;
                }
            } else {
                return 0x70C7;
            }
        }
    }

    return 0x9E;
}

void Ossan_init(EnOssan* this, PlayState* play) {
    ShopItem* items;

    if (CHK_Exchange_bank_dma(this, play)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.objectSlot = this->objectSlot1;
        Actor_set_segment(play, &this->actor);

        this->shelves = (EnTana*)Actor_info_name_search(&play->actorCtx, ACTOR_EN_TANA, ACTORCAT_PROP);

        if (this->shelves == NULL) {
            PRINTF_COLOR_ERROR();
            // "Warning!! There are no shelves!!"
            PRINTF("★★★ 警告！！ 棚がないよ！！ ★★★\n");
            PRINTF_RST();
            return;
        }

        // "Shopkeeper (params) init"
        PRINTF(VT_FGCOL(YELLOW) "◇◇◇ 店のおやじ( %d ) 初期設定 ◇◇◇" VT_RST "\n", this->actor.params);

        this->actor.world.pos.x += pos_hosei[this->actor.params].x;
        this->actor.world.pos.y += pos_hosei[this->actor.params].y;
        this->actor.world.pos.z += pos_hosei[this->actor.params].z;

        items = OssanItemData[this->actor.params];

        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 20.0f);
        dispinit[this->actor.params](this, play);
        this->actor.textId = first_talk_message(this);
        this->cursorY = this->cursorX = 100.0f;
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->actor.colChkInfo.cylRadius = 50;
        this->stateFlag = OSSAN_STATE_IDLE;
        this->stickAccumX = this->stickAccumY = 0;

        this->cursorIndex = 0;
        this->cursorZ = 1.5f;
        this->cursorColorR = CURSOR_COLOR_R;
        this->cursorColorG = CURSOR_COLOR_G;
        this->cursorColorB = CURSOR_COLOR_B;
        this->cursorColorA = CURSOR_COLOR_A;
        this->cursorAnimTween = 0;

        this->cursorAnimState = 0;
        this->drawCursor = 0;
        this->happyMaskShopkeeperEyeIdx = 0;

        this->stickLeftPrompt.stickColorR = 200;
        this->stickLeftPrompt.stickColorG = 200;
        this->stickLeftPrompt.stickColorB = 200;
        this->stickLeftPrompt.stickColorA = 180;
        this->stickLeftPrompt.stickTexX = 49;
        this->stickLeftPrompt.stickTexY = 95;
        this->stickLeftPrompt.arrowColorR = 255;
        this->stickLeftPrompt.arrowColorG = 255;
        this->stickLeftPrompt.arrowColorB = 0;
        this->stickLeftPrompt.arrowColorA = 200;
        this->stickLeftPrompt.arrowTexX = 33;
        this->stickLeftPrompt.arrowTexY = 91;
        this->stickLeftPrompt.z = 1;
        this->stickLeftPrompt.isEnabled = false;

        this->stickRightPrompt.stickColorR = 200;
        this->stickRightPrompt.stickColorG = 200;
        this->stickRightPrompt.stickColorB = 200;
        this->stickRightPrompt.stickColorA = 180;
        this->stickRightPrompt.stickTexX = 274;
        this->stickRightPrompt.stickTexY = 95;
        this->stickRightPrompt.arrowColorR = 255;
        this->stickRightPrompt.arrowColorG = 255;
        this->stickRightPrompt.arrowColorB = 0;
        this->stickRightPrompt.arrowColorA = 200;
        this->stickRightPrompt.arrowTexX = 290;
        this->stickRightPrompt.arrowTexY = 91;
        this->stickRightPrompt.z = 1;
        this->stickRightPrompt.isEnabled = false;

        this->arrowAnimState = 0;
        this->stickAnimState = 0;
        this->arrowAnimTween = 0;
        this->stickAnimTween = 0;
        this->shopItemSelectedTween = 0;
        Actor_set_scale(&this->actor, oB1_Scale[this->actor.params]);
        OssanItemSet(this, play, items);
        this->headRot = this->headTargetRot = 0;
        this->blinkTimer = 20;
        this->eyeTextureIdx = 0;
        this->blinkFunc = Ossan_glare;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        En_OSSAN_actor_set_process(this, Ossan_move);
    }
}

void MasterKokiri_subProc(EnOssan* this, PlayState* play) {
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot3].segment);
}

void Ossan_move(EnOssan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->blinkFunc(this);
    Ossan_StickCHK(play, this);
    Ossan_ItemRevolveCHK(this);
    Ossan_YajirusiAnimation(this);
    Ossan_CursolAnimation(this);
    chase_s(&this->headRot, this->headTargetRot, 0x190);

    if (player != NULL) {
        Ossan_Talk[this->stateFlag](this, play, player);
    }

    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 26.0f, 10.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    Actor_world_to_eye(&this->actor, 90.0f);
    Actor_set_scale(&this->actor, oB1_Scale[this->actor.params]);

    // use animation object if needed
    if (this->obj3ToSeg6Func != NULL) {
        this->obj3ToSeg6Func(this, play);
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
}

void En_OSSAN_Actor_move(Actor* thisx, PlayState* play) {
    EnOssan* this = (EnOssan*)thisx;

    this->timer++;
    this->actionFunc(this, play);
}

s32 en_ossan_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                              void* thisx) {
    EnOssan* this = (EnOssan*)thisx;

    if (limbIndex == 8) {
        rot->x += this->headRot;
    }
    return 0;
}

void En_OSSAN_DrawCursor(PlayState* play, EnOssan* this, f32 x, f32 y, f32 z, u8 drawCursor) {
    s32 ulx, uly, lrx, lry;
    f32 w;
    s32 dsdx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4192);
    if (drawCursor != 0) {
        overlay_rectangle_a_prim(play->state.gfxCtx);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, this->cursorColorR, this->cursorColorG, this->cursorColorB,
                        this->cursorColorA);
        gDPLoadTextureBlock_4b(OVERLAY_DISP++, gSelectionCursorTex, G_IM_FMT_IA, 16, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                               G_TX_MIRROR | G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD);
        w = 16.0f * z;
        ulx = (x - w) * 4.0f;
        uly = (y - w) * 4.0f;
        lrx = (x + w) * 4.0f;
        lry = (y + w) * 4.0f;
        dsdx = (1.0f / z) * 1024.0f;
        gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, 0, 0, dsdx, dsdx);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4215);
}

void Ossan_Rectangle(PlayState* play, s32 r, s32 g, s32 b, s32 a, f32 x, f32 y, f32 z, s32 s, s32 t, f32 dx,
                         f32 dy) {
    f32 texCoordScale;
    s32 ulx, uly, lrx, lry;
    f32 w, h;
    s32 dsdx, dtdy;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4228);
    gDPPipeSync(OVERLAY_DISP++);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, r, g, b, a);

    w = 8.0f * z;
    ulx = (x - w) * 4.0f;
    lrx = (x + w) * 4.0f;

    h = 12.0f * z;
    uly = (y - h) * 4.0f;
    lry = (y + h) * 4.0f;

    texCoordScale = (1.0f / z) * 1024;
    dsdx = texCoordScale * dx;
    dtdy = dy * texCoordScale;

    gSPTextureRectangle(OVERLAY_DISP++, ulx, uly, lrx, lry, G_TX_RENDERTILE, s, t, dsdx, dtdy);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4242);
}

void En_OSSAN_DrawYajirushi(PlayState* play, EnOssan* this) {
    s32 drawStickLeftPrompt = this->stickLeftPrompt.isEnabled;
    s32 drawStickRightPrompt = this->stickRightPrompt.isEnabled;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4252);
    if (drawStickLeftPrompt || drawStickRightPrompt) {
        overlay_rectangle_a_prim(play->state.gfxCtx);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPLoadTextureBlock(OVERLAY_DISP++, gArrowCursorTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 24, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickLeftPrompt) {
            Ossan_Rectangle(play, this->stickLeftPrompt.arrowColorR, this->stickLeftPrompt.arrowColorG,
                                this->stickLeftPrompt.arrowColorB, this->stickLeftPrompt.arrowColorA,
                                this->stickLeftPrompt.arrowTexX, this->stickLeftPrompt.arrowTexY,
                                this->stickLeftPrompt.z, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickRightPrompt) {
            Ossan_Rectangle(play, this->stickRightPrompt.arrowColorR, this->stickRightPrompt.arrowColorG,
                                this->stickRightPrompt.arrowColorB, this->stickRightPrompt.arrowColorA,
                                this->stickRightPrompt.arrowTexX, this->stickRightPrompt.arrowTexY,
                                this->stickRightPrompt.z, 0, 0, 1.0f, 1.0f);
        }
        gDPLoadTextureBlock(OVERLAY_DISP++, gControlStickTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 4, G_TX_NOMASK, G_TX_NOLOD,
                            G_TX_NOLOD);
        if (drawStickLeftPrompt) {
            Ossan_Rectangle(play, this->stickLeftPrompt.stickColorR, this->stickLeftPrompt.stickColorG,
                                this->stickLeftPrompt.stickColorB, this->stickLeftPrompt.stickColorA,
                                this->stickLeftPrompt.stickTexX, this->stickLeftPrompt.stickTexY,
                                this->stickLeftPrompt.z, 0, 0, -1.0f, 1.0f);
        }
        if (drawStickRightPrompt) {
            Ossan_Rectangle(play, this->stickRightPrompt.stickColorR, this->stickRightPrompt.stickColorG,
                                this->stickRightPrompt.stickColorB, this->stickRightPrompt.stickColorA,
                                this->stickRightPrompt.stickTexX, this->stickRightPrompt.stickTexY,
                                this->stickRightPrompt.z, 0, 0, 1.0f, 1.0f);
        }
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4300);
}

void En_OSSAN_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = { gOssanEyeOpenTex, gOssanEyeHalfTex, gOssanEyeClosedTex };
    EnOssan* this = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4320);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTextureIdx]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          en_ossan_display1, NULL, this);
    En_OSSAN_DrawCursor(play, this, this->cursorX, this->cursorY, this->cursorZ, this->drawCursor);
    En_OSSAN_DrawYajirushi(play, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4340);
}

s32 masterkokiri_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                             void* thisx) {
    static void* eye_txt[] = {
        gKokiriShopkeeperEyeDefaultTex,
        gKokiriShopkeeperEyeHalfTex,
        gKokiriShopkeeperEyeOpenTex,
    };
    EnOssan* this = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4354);

    if (limbIndex == 15) {
        gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->objectSlot2].segment);
        SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot2].segment);
        *dList = gKokiriShopkeeperHeadDL;
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTextureIdx]));
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4374);

    return 0;
}

static Gfx* npc_nothing(GraphicsContext* gfxCtx) {
    Gfx* disp = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));

    gSPEndDisplayList(disp);
    return disp;
}

static Gfx* pa(GraphicsContext* gfxCtx, u8 r, u8 g, u8 b, u8 a) {
    Gfx* disp = GRAPH_ALLOC(gfxCtx, sizeof(Gfx) * 2);

    gDPSetEnvColor(disp, r, g, b, a);
    gSPEndDisplayList(disp + 1);
    return disp;
}

void MasterKokiri_draw(Actor* thisx, PlayState* play) {
    EnOssan* this = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4409);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x08, pa(play->state.gfxCtx, 0, 130, 70, 255));
    gSPSegment(POLY_OPA_DISP++, 0x09, pa(play->state.gfxCtx, 110, 170, 20, 255));
    gSPSegment(POLY_OPA_DISP++, 0x0C, npc_nothing(play->state.gfxCtx));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          masterkokiri_display1, NULL, this);
    En_OSSAN_DrawCursor(play, this, this->cursorX, this->cursorY, this->cursorZ, this->drawCursor);
    En_OSSAN_DrawYajirushi(play, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4434);
}

void MasterGolon_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = { gGoronCsEyeOpenTex, gGoronCsEyeHalfTex, gGoronCsEyeClosedTex };
    EnOssan* this = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4455);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTextureIdx]));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gGoronCsMouthNeutralTex));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, this);
    En_OSSAN_DrawCursor(play, this, this->cursorX, this->cursorY, this->cursorZ, this->drawCursor);
    En_OSSAN_DrawYajirushi(play, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4476);
}

s32 masterzoora_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                           void* thisx) {
    EnOssan* this = (EnOssan*)thisx;

    if (limbIndex == 15) {
        rot->x += this->headRot;
    }
    return 0;
}

void MasterZoora_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = { gZoraEyeOpenTex, gZoraEyeHalfTex, gZoraEyeClosedTex };
    EnOssan* this = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4506);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, npc_nothing(play->state.gfxCtx));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTextureIdx]));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          masterzoora_display1, NULL, this);
    En_OSSAN_DrawCursor(play, this, this->cursorX, this->cursorY, this->cursorZ, this->drawCursor);
    En_OSSAN_DrawYajirushi(play, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4531);
}

void Obachan_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = {
        gPotionShopkeeperEyeOpenTex,
        gPotionShopkeeperEyeHalfTex,
        gPotionShopkeeperEyeClosedTex,
    };
    EnOssan* this = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4544);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTextureIdx]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, this);
    En_OSSAN_DrawCursor(play, this, this->cursorX, this->cursorY, this->cursorZ, this->drawCursor);
    En_OSSAN_DrawYajirushi(play, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4564);
}

void Os_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = { gHappyMaskSalesmanEyeClosedTex, gHappyMaskSalesmanEyeOpenTex };
    EnOssan* this = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4578);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               SEGMENTED_TO_VIRTUAL(eye_txt[this->happyMaskShopkeeperEyeIdx]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, this);
    En_OSSAN_DrawCursor(play, this, this->cursorX, this->cursorY, this->cursorZ, this->drawCursor);
    En_OSSAN_DrawYajirushi(play, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4598);
}

void Rs_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = {
        gBombchuShopkeeperEyeOpenTex,
        gBombchuShopkeeperEyeHalfTex,
        gBombchuShopkeeperEyeClosedTex,
    };
    EnOssan* this = (EnOssan*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4611);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTextureIdx]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, this);
    En_OSSAN_DrawCursor(play, this, this->cursorX, this->cursorY, this->cursorZ, this->drawCursor);
    En_OSSAN_DrawYajirushi(play, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_oB1.c", 4631);
}
