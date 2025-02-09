/*
 * File: z_en_girla.c
 * Overlay: En_GirlA
 * Description: Shop Items
 */

#include "z_en_girla.h"
#include "terminal.h"
#include "z64draw.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_GirlA_actor_ct(Actor* thisx, PlayState* play);
void En_GirlA_actor_dt(Actor* thisx, PlayState* play);
void En_GirlA_actor_move(Actor* thisx, PlayState* play);

void GirlA_ClearItem(PlayState* play, EnGirlA* this);
void GirlA_ResetItem(PlayState* play, EnGirlA* this);
void GirlA_initial(EnGirlA* this, PlayState* play);
void GirlA_Move(EnGirlA* this, PlayState* play);
void Actor_HiliteReflect_custam(Actor* thisx, PlayState* play, s32 flags);
void En_GirlA_actor_draw(Actor* thisx, PlayState* play);

s32 ArrowCheck(PlayState* play, EnGirlA* this);
s32 BombCheck(PlayState* play, EnGirlA* this);
s32 DNutsCheck(PlayState* play, EnGirlA* this);
s32 DStickCheck(PlayState* play, EnGirlA* this);
s32 FishCheck(PlayState* play, EnGirlA* this);
s32 RDrugCheck(PlayState* play, EnGirlA* this);
s32 GDrugCheck(PlayState* play, EnGirlA* this);
s32 BDrugCheck(PlayState* play, EnGirlA* this);
s32 G_KnifeCheck(PlayState* play, EnGirlA* this);
s32 H_ShieldCheck(PlayState* play, EnGirlA* this);
s32 D_ShieldCheck(PlayState* play, EnGirlA* this);
s32 G_DressCheck(PlayState* play, EnGirlA* this);
s32 Z_DressCheck(PlayState* play, EnGirlA* this);
s32 HeartCheck(PlayState* play, EnGirlA* this);
static s32 RRMilkCheck(PlayState* play, EnGirlA* this);
s32 NiwaEggCheck(PlayState* play, EnGirlA* this);
s32 IGMilkCheck(PlayState* play, EnGirlA* this);
s32 IGEggCheck(PlayState* play, EnGirlA* this);
s32 BomchuCheck(PlayState* play, EnGirlA* this);
s32 SeedCheck(PlayState* play, EnGirlA* this);
s32 SoldoutCheck(PlayState* play, EnGirlA* this);
s32 FireCheck(PlayState* play, EnGirlA* this);
s32 InsectCheck(PlayState* play, EnGirlA* this);
s32 GhostCheck(PlayState* play, EnGirlA* this);
s32 SoulCheck(PlayState* play, EnGirlA* this);

void DNutsPlus(PlayState* play, EnGirlA* this);
void ArrowPlus(PlayState* play, EnGirlA* this);
void BombPlus(PlayState* play, EnGirlA* this);
void DStickPlus(PlayState* play, EnGirlA* this);
void BottleItemPlus(PlayState* play, EnGirlA* this);
void G_KnifePlus(PlayState* play, EnGirlA* this);
void H_ShieldPlus(PlayState* play, EnGirlA* this);
void D_ShieldPlus(PlayState* play, EnGirlA* this);
void G_DressPlus(PlayState* play, EnGirlA* this);
void Z_DressPlus(PlayState* play, EnGirlA* this);
void HeartPlus(PlayState* play, EnGirlA* this);
void RRMilkPlus(PlayState* play, EnGirlA* this);
void NiwaEggPlus(PlayState* play, EnGirlA* this);
void IGMilkPlus(PlayState* play, EnGirlA* this);
void IGEggPlus(PlayState* play, EnGirlA* this);
void SeedPlus(PlayState* play, EnGirlA* this);
void PaymentOnly(PlayState* play, EnGirlA* this);
void BomchuFirstPlus(PlayState* play, EnGirlA* this);
void G_DressFirstPlus(PlayState* play, EnGirlA* this);
void Z_DressFirstPlus(PlayState* play, EnGirlA* this);

ActorProfile En_GirlA_Profile = {
    /**/ ACTOR_EN_GIRLA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnGirlA),
    /**/ En_GirlA_actor_ct,
    /**/ En_GirlA_actor_dt,
    /**/ En_GirlA_actor_move,
    /**/ NULL,
};

#if DEBUG_FEATURES
static char* sShopItemDescriptions[] = {
    "デクの実×5   ",  // "Deku nut x5"
    "矢×30        ",  // "Arrow x30"
    "矢×50        ",  // "Arrow x50"
    "爆弾×5       ",  // "bomb"
    "デクの実×10  ",  // "Deku nut x10"
    "デクの棒      ", // "Deku stick"
    "爆弾×10      ",  // "Bomb x10"
    "さかな        ", // "Fish"
    "赤クスリ      ", // "Red medicine"
    "緑クスリ      ", // "Green medicine"
    "青クスリ      ", // "Blue medicine"
    "巨人のナイフ  ", // "Giant knife"
    "ハイリアの盾  ", // "Hyria Shield"
    "デクの盾      ", // "Deku Shield"
    "ゴロンの服    ", // "Goron's clothes"
    "ゾ─ラの服    ",  // "Zora's clothes"
    "回復のハート  ", // "Heart of recovery"
    "ロンロン牛乳  ", // "Ron Ron milk"
    "鶏の卵        ", // "Chicken egg"
    "インゴー牛乳  ", // "Ingo milk"
    "インゴー卵    ", // "Ingo egg"
    "もだえ石      ", // "Modae stone"
    "大人の財布    ", // "Adult wallet"
    "ハートの欠片  ", // "Heart fragment"
    "ボムチュウ    ", // "Bombchu"
    "ボムチュウ    ", // "Bombchu"
    "ボムチュウ    ", // "Bombchu"
    "ボムチュウ    ", // "Bombchu"
    "ボムチュウ    ", // "Bombchu"
    "デクのタネ    ", // "Deku seeds"
    "キータンのお面", // "Ketan's mask"
    "こわそなお面  ", // "Scary face"
    "ドクロのお面  ", // "Skull mask"
    "ウサギずきん  ", // "Rabbit hood"
    "まことの仮面  ", // "True mask"
    "ゾーラのお面  ", // "Zora's mask"
    "ゴロンのお面  ", // "Goron's mask"
    "ゲルドのお面  ", // "Gerd's mask"
    "ＳＯＬＤＯＵＴ",
    "炎            ", // "Flame"
    "虫            ", // "Bugs"
    "チョウチョ    ", // "Butterfly"
    "ポウ          ", // "Poe"
    "妖精の魂      ", // "Fairy soul"
    "矢×10        ",  // "Arrow"
    "爆弾×20      ",  // "Bomb x20"
    "爆弾×30      ",  // "Bomb x30"
    "爆弾×5       ",  // "Bomb x5"
    "赤クスリ      ", // "Red medicine"
    "赤クスリ      "  // "Red medicine"
};
#endif

static s16 MaskId_Data[8] = {
    ITEM_MASK_KEATON, ITEM_MASK_SPOOKY, ITEM_MASK_SKULL, ITEM_MASK_BUNNY_HOOD,
    ITEM_MASK_TRUTH,  ITEM_MASK_ZORA,   ITEM_MASK_GORON, ITEM_MASK_GERUDO,
};

static u16 Mask_Message2Data[5] = { 0x70B6, 0x70B5, 0x70B4, 0x70B7, 0x70BB };

typedef struct ShopItemEntry {
    /* 0x00 */ s16 objID;
    /* 0x02 */ s16 giDrawId;
    /* 0x04 */ void (*hiliteFunc)(Actor*, PlayState*, s32);
    /* 0x08 */ s16 price;
    /* 0x0A */ s16 count;
    /* 0x0C */ u16 itemDescTextId;
    /* 0x0C */ u16 itemBuyPromptTextId;
    /* 0x10 */ s32 getItemId;
    /* 0x14 */ s32 (*canBuyFunc)(PlayState*, EnGirlA*);
    /* 0x18 */ void (*itemGiveFunc)(PlayState*, EnGirlA*);
    /* 0x1C */ void (*buyEventFunc)(PlayState*, EnGirlA*);
} ShopItemEntry; // size = 0x20

static ShopItemEntry GirlA_DrawData[] = {
    // SI_DEKU_NUTS_5
    { OBJECT_GI_NUTS, GID_DEKU_NUTS, Actor_HiliteReflect_xlu_set_init, 15, 5, 0x00B2, 0x007F, GI_DEKU_NUTS_5_2, DNutsCheck,
      DNutsPlus, PaymentOnly },
    /* SI_ARROWS_30 */
    { OBJECT_GI_ARROW, GID_ARROWS_10, Actor_HiliteReflect_set_init, 60, 30, 0x00C1, 0x009B, GI_ARROWS_10, ArrowCheck,
      ArrowPlus, PaymentOnly },
    /* SI_ARROWS_50 */
    { OBJECT_GI_ARROW, GID_ARROWS_30, Actor_HiliteReflect_set_init, 90, 50, 0x00B0, 0x007D, GI_ARROWS_30, ArrowCheck,
      ArrowPlus, PaymentOnly },
    /* SI_BOMBS_5_R25 */
    { OBJECT_GI_BOMB_1, GID_BOMB, Actor_HiliteReflect_set_init, 25, 5, 0x00A3, 0x008B, GI_BOMBS_5, BombCheck,
      BombPlus, PaymentOnly },
    /* SI_DEKU_NUTS_10 */
    { OBJECT_GI_NUTS, GID_DEKU_NUTS, Actor_HiliteReflect_xlu_set_init, 30, 10, 0x00A2, 0x0087, GI_DEKU_NUTS_10, DNutsCheck,
      DNutsPlus, PaymentOnly },
    /* SI_DEKU_STICK */
    { OBJECT_GI_STICK, GID_DEKU_STICK, NULL, 10, 1, 0x00A1, 0x0088, GI_DEKU_STICKS_1, DStickCheck,
      DStickPlus, PaymentOnly },
    /* SI_BOMBS_10 */
    { OBJECT_GI_BOMB_1, GID_BOMB, Actor_HiliteReflect_set_init, 50, 10, 0x00B1, 0x007C, GI_BOMBS_10, BombCheck,
      BombPlus, PaymentOnly },
    /* SI_FISH */
    { OBJECT_GI_FISH, GID_FISH, Actor_HiliteReflect_xlu_set_init, 200, 1, 0x00B3, 0x007E, GI_BOTTLE_FISH, FishCheck, NULL,
      PaymentOnly },
    /* SI_RED_POTION_R30 */
    { OBJECT_GI_LIQUID, GID_BOTTLE_POTION_RED, Actor_HiliteReflect_set_init, 30, 1, 0x00A5, 0x008E, GI_BOTTLE_POTION_RED,
      RDrugCheck, BottleItemPlus, PaymentOnly },
    /* SI_GREEN_POTION */
    { OBJECT_GI_LIQUID, GID_BOTTLE_POTION_GREEN, Actor_HiliteReflect_set_init, 30, 1, 0x00A6, 0x008F, GI_BOTTLE_POTION_GREEN,
      GDrugCheck, BottleItemPlus, PaymentOnly },
    /* SI_BLUE_POTION */
    { OBJECT_GI_LIQUID, GID_BOTTLE_POTION_BLUE, Actor_HiliteReflect_set_init, 60, 1, 0x00A7, 0x0090, GI_BOTTLE_POTION_BLUE,
      BDrugCheck, BottleItemPlus, PaymentOnly },
    /* SI_LONGSWORD */
    { OBJECT_GI_LONGSWORD, GID_SWORD_BIGGORON, Actor_HiliteReflect_set_init, 1000, 1, 0x00A8, 0x0091, GI_SWORD_KNIFE,
      G_KnifeCheck, G_KnifePlus, PaymentOnly },
    /* SI_HYLIAN_SHIELD */
    { OBJECT_GI_SHIELD_2, GID_SHIELD_HYLIAN, Actor_HiliteReflect_set_init, 80, 1, 0x00A9, 0x0092, GI_SHIELD_HYLIAN,
      H_ShieldCheck, H_ShieldPlus, PaymentOnly },
    /* SI_DEKU_SHIELD */
    { OBJECT_GI_SHIELD_1, GID_SHIELD_DEKU, Actor_HiliteReflect_set_init, 40, 1, 0x009F, 0x0089, GI_SHIELD_DEKU,
      D_ShieldCheck, D_ShieldPlus, PaymentOnly },
    /* SI_GORON_TUNIC */
    { OBJECT_GI_CLOTHES, GID_TUNIC_GORON, NULL, 200, 1, 0x00AA, 0x0093, GI_TUNIC_GORON, G_DressCheck,
      G_DressPlus, G_DressFirstPlus },
    /* SI_ZORA_TUNIC */
    { OBJECT_GI_CLOTHES, GID_TUNIC_ZORA, NULL, 300, 1, 0x00AB, 0x0094, GI_TUNIC_ZORA, Z_DressCheck,
      Z_DressPlus, Z_DressFirstPlus },
    /* SI_RECOVERY_HEART */
    { OBJECT_GI_HEART, GID_RECOVERY_HEART, NULL, 10, 16, 0x00AC, 0x0095, GI_RECOVERY_HEART,
      HeartCheck, HeartPlus, PaymentOnly },
    /* SI_MILK_BOTTLE */
    { OBJECT_GI_MILK, GID_BOTTLE_MILK_FULL, Actor_HiliteReflect_custam, 100, 1, 0x00AD, 0x0097, GI_BOTTLE_MILK_FULL,
      RRMilkCheck, RRMilkPlus, PaymentOnly },
    /* SI_WEIRD_EGG */
    { OBJECT_GI_EGG, GID_EGG, Actor_HiliteReflect_set_init, 100, 1, 0x00AE, 0x0099, GI_WEIRD_EGG, NiwaEggCheck,
      NiwaEggPlus, PaymentOnly },
    /* SI_19 */
    { OBJECT_GI_MILK, GID_BOTTLE_MILK_FULL, Actor_HiliteReflect_custam, 10000, 1, 0x00B4, 0x0085, GI_NONE, IGMilkCheck,
      IGMilkPlus, PaymentOnly },
    /* SI_20 */
    { OBJECT_GI_EGG, GID_EGG, Actor_HiliteReflect_set_init, 10000, 1, 0x00B5, 0x0085, GI_NONE, IGEggCheck,
      IGEggPlus, PaymentOnly },
    /* SI_BOMBCHU_10_1 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, Actor_HiliteReflect_set_init, 100, 10, 0x00BC, 0x008C, GI_BOMBCHUS_10, BomchuCheck,
      NULL, BomchuFirstPlus },
    /* SI_BOMBCHU_20_1 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, Actor_HiliteReflect_set_init, 180, 20, 0x0061, 0x002A, GI_BOMBCHUS_20, BomchuCheck,
      NULL, BomchuFirstPlus },
    /* SI_BOMBCHU_20_2 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, Actor_HiliteReflect_set_init, 180, 20, 0x0061, 0x002A, GI_BOMBCHUS_20, BomchuCheck,
      NULL, BomchuFirstPlus },
    /* SI_BOMBCHU_10_2 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, Actor_HiliteReflect_set_init, 100, 10, 0x00BC, 0x008C, GI_BOMBCHUS_10, BomchuCheck,
      NULL, BomchuFirstPlus },
    /* SI_BOMBCHU_10_3 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, Actor_HiliteReflect_set_init, 100, 10, 0x00BC, 0x008C, GI_BOMBCHUS_10, BomchuCheck,
      NULL, BomchuFirstPlus },
    /* SI_BOMBCHU_20_3 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, Actor_HiliteReflect_set_init, 180, 20, 0x0061, 0x002A, GI_BOMBCHUS_20, BomchuCheck,
      NULL, BomchuFirstPlus },
    /* SI_BOMBCHU_20_4 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, Actor_HiliteReflect_set_init, 180, 20, 0x0061, 0x002A, GI_BOMBCHUS_20, BomchuCheck,
      NULL, BomchuFirstPlus },
    /* SI_BOMBCHU_10_4 */
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, Actor_HiliteReflect_set_init, 100, 10, 0x00BC, 0x008C, GI_BOMBCHUS_10, BomchuCheck,
      NULL, BomchuFirstPlus },
    /* SI_DEKU_SEEDS_30 */
    { OBJECT_GI_SEED, GID_DEKU_SEEDS, Actor_HiliteReflect_set_init, 30, 30, 0x00DF, 0x00DE, GI_DEKU_SEEDS_30, SeedCheck,
      SeedPlus, PaymentOnly },
    /* SI_KEATON_MASK */
    { OBJECT_GI_KI_TAN_MASK, GID_MASK_KEATON, Actor_HiliteReflect_set_init, 0, 1, 0x70B2, 0x70BE, GI_MASK_KEATON,
      SeedCheck, SeedPlus, PaymentOnly },
    /* SI_SPOOKY_MASK */
    { OBJECT_GI_REDEAD_MASK, GID_MASK_SPOOKY, Actor_HiliteReflect_set_init, 0, 1, 0x70B1, 0x70BD, GI_MASK_SPOOKY,
      SeedCheck, SeedPlus, PaymentOnly },
    /* SI_SKULL_MASK */
    { OBJECT_GI_SKJ_MASK, GID_MASK_SKULL, Actor_HiliteReflect_set_init, 0, 1, 0x70B0, 0x70BC, GI_MASK_SKULL, SeedCheck,
      SeedPlus, PaymentOnly },
    /* SI_BUNNY_HOOD */
    { OBJECT_GI_RABIT_MASK, GID_MASK_BUNNY_HOOD, Actor_HiliteReflect_set_init, 0, 1, 0x70B3, 0x70BF, GI_MASK_BUNNY_HOOD,
      SeedCheck, SeedPlus, PaymentOnly },
    /* SI_MASK_OF_TRUTH */
    { OBJECT_GI_TRUTH_MASK, GID_MASK_TRUTH, Actor_HiliteReflect_custam, 0, 1, 0x70AF, 0x70C3, GI_MASK_TRUTH,
      SeedCheck, SeedPlus, PaymentOnly },
    /* SI_ZORA_MASK */
    { OBJECT_GI_ZORAMASK, GID_MASK_ZORA, NULL, 0, 1, 0x70B9, 0x70C1, GI_MASK_ZORA, SeedCheck,
      SeedPlus, PaymentOnly },
    /* SI_GORON_MASK */
    { OBJECT_GI_GOLONMASK, GID_MASK_GORON, NULL, 0, 1, 0x70B8, 0x70C0, GI_MASK_GORON, SeedCheck,
      SeedPlus, PaymentOnly },
    /* SI_GERUDO_MASK */
    { OBJECT_GI_GERUDOMASK, GID_MASK_GERUDO, NULL, 0, 1, 0x70BA, 0x70C2, GI_MASK_GERUDO, SeedCheck,
      SeedPlus, PaymentOnly },
    /* SI_SOLD_OUT */
    { OBJECT_GI_SOLDOUT, GID_SOLDOUT, Actor_HiliteReflect_set_init, 0, 0, 0x00BD, 0x70C2, GI_MASK_GERUDO, SoldoutCheck, NULL,
      NULL },
    /* SI_BLUE_FIRE */
    { OBJECT_GI_FIRE, GID_BLUE_FIRE, Actor_HiliteReflect_set_init, 300, 1, 0x00B9, 0x00B8, GI_BOTTLE_BLUE_FIRE,
      FireCheck, BottleItemPlus, PaymentOnly },
    /* SI_BUGS */
    { OBJECT_GI_INSECT, GID_BUG, Actor_HiliteReflect_custam, 50, 1, 0x00BB, 0x00BA, GI_BOTTLE_BUGS, InsectCheck,
      BottleItemPlus, PaymentOnly },
    /* SI_BIG_POE */
    { OBJECT_GI_GHOST, GID_BIG_POE, Actor_HiliteReflect_custam, 50, 1, 0x506F, 0x5070, GI_BOTTLE_BIG_POE, GhostCheck,
      BottleItemPlus, PaymentOnly },
    /* SI_POE */
    { OBJECT_GI_GHOST, GID_POE, Actor_HiliteReflect_custam, 30, 1, 0x506D, 0x506E, GI_BOTTLE_POE, GhostCheck,
      BottleItemPlus, PaymentOnly },
    /* SI_FAIRY */
    { OBJECT_GI_SOUL, GID_FAIRY, Actor_HiliteReflect_custam, 50, 1, 0x00B7, 0x00B6, GI_BOTTLE_FAIRY, SoulCheck,
      BottleItemPlus, PaymentOnly },
    /* SI_ARROWS_10 */
    { OBJECT_GI_ARROW, GID_ARROWS_5, Actor_HiliteReflect_set_init, 20, 10, 0x00A0, 0x008A, GI_ARROWS_5, ArrowCheck,
      ArrowPlus, PaymentOnly },
    /* SI_BOMBS_20 */
    { OBJECT_GI_BOMB_1, GID_BOMB, Actor_HiliteReflect_set_init, 80, 20, 0x001C, 0x0006, GI_BOMBS_20, BombCheck,
      BombPlus, PaymentOnly },
    /* SI_BOMBS_30 */
    { OBJECT_GI_BOMB_1, GID_BOMB, Actor_HiliteReflect_set_init, 120, 30, 0x001D, 0x001E, GI_BOMBS_30, BombCheck,
      BombPlus, PaymentOnly },
    /* SI_BOMBS_5_R35 */
    { OBJECT_GI_BOMB_1, GID_BOMB, Actor_HiliteReflect_set_init, 35, 5, 0x00CB, 0x00CA, GI_BOMBS_5, BombCheck,
      BombPlus, PaymentOnly },
    /* SI_RED_POTION_R40 */
    { OBJECT_GI_LIQUID, GID_BOTTLE_POTION_RED, Actor_HiliteReflect_set_init, 40, 1, 0x0064, 0x0062, GI_BOTTLE_POTION_RED,
      RDrugCheck, BottleItemPlus, PaymentOnly },
    /* SI_RED_POTION_R50 */
    { OBJECT_GI_LIQUID, GID_BOTTLE_POTION_RED, Actor_HiliteReflect_set_init, 50, 1, 0x0065, 0x0063, GI_BOTTLE_POTION_RED,
      RDrugCheck, BottleItemPlus, PaymentOnly }
};

// Defines the Hylian Shield discount amount
static s16 rnd_price[] = { 5, 10, 15, 20, 25, 30, 35, 40 };

void En_GirlA_actor_set_process(EnGirlA* this, EnGirlAActionFunc func) {
    this->actionFunc = func;
}

s32 En_GirlA_arg_data_set(EnGirlA* this) {
    switch (this->actor.params) {
        case SI_MILK_BOTTLE:
            if (GET_ITEMGETINF(ITEMGETINF_TALON_BOTTLE)) {
                this->actor.params = SI_RECOVERY_HEART;
                return true;
            }
            break;
        case SI_BOMBCHU_10_2:
            if (GET_ITEMGETINF(ITEMGETINF_06)) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_10_3:
            if (GET_ITEMGETINF(ITEMGETINF_07)) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_20_3:
            if (GET_ITEMGETINF(ITEMGETINF_08)) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_20_4:
            if (GET_ITEMGETINF(ITEMGETINF_09)) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_10_4:
            if (GET_ITEMGETINF(ITEMGETINF_0A)) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_10_1:
            if (GET_ITEMGETINF(ITEMGETINF_03)) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_20_1:
            if (GET_ITEMGETINF(ITEMGETINF_04)) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
        case SI_BOMBCHU_20_2:
            if (GET_ITEMGETINF(ITEMGETINF_05)) {
                this->actor.params = SI_SOLD_OUT;
                return true;
            }
            break;
    }
    return false;
}

void En_GirlA_bank_ID_set(EnGirlA* this, PlayState* play) {
    s16 params = this->actor.params;

    PRINTF("%s(%2d)初期設定\n", sShopItemDescriptions[params], params);

    if ((params >= SI_MAX) && (params < 0)) {
        Actor_delete(&this->actor);
        PRINTF_COLOR_ERROR();
        PRINTF("引数がおかしいよ(arg_data=%d)！！\n", this->actor.params);
        PRINTF_RST();
        ASSERT(0, "0", "../z_en_girlA.c", 1421);
        return;
    }

    this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, GirlA_DrawData[params].objID);

    if (this->requiredObjectSlot < 0) {
        Actor_delete(&this->actor);
        PRINTF_COLOR_ERROR();
        PRINTF("バンクが無いよ！！(%s)\n", sShopItemDescriptions[params]);
        PRINTF_RST();
        ASSERT(0, "0", "../z_en_girlA.c", 1434);
        return;
    }

    this->actor.params = params;
    this->actionFunc2 = GirlA_initial;
}

void En_GirlA_actor_ct(Actor* thisx, PlayState* play) {
    EnGirlA* this = (EnGirlA*)thisx;

    En_GirlA_arg_data_set(this);
    En_GirlA_bank_ID_set(this, play);
    PRINTF("%s(%2d)初期設定\n", sShopItemDescriptions[this->actor.params], this->actor.params);
}

void En_GirlA_actor_dt(Actor* thisx, PlayState* play) {
    EnGirlA* this = (EnGirlA*)thisx;

    if (this->isInitialized) {
        Skeleton_Info_dt(&this->skelAnime, play);
    }
}

s32 ArrowCheck(PlayState* play, EnGirlA* this) {
    if (item_get_non_setting(ITEM_BOW) == ITEM_NONE) {
        return CANBUY_RESULT_CANT_GET_NOW_5;
    }
    if (AMMO(ITEM_BOW) >= CUR_CAPACITY(UPG_QUIVER)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 BombCheck(PlayState* play, EnGirlA* this) {
    if (!CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (AMMO(ITEM_BOMB) >= CUR_CAPACITY(UPG_BOMB_BAG)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 DNutsCheck(PlayState* play, EnGirlA* this) {
    if ((CUR_CAPACITY(UPG_DEKU_NUTS) != 0) && (AMMO(ITEM_DEKU_NUT) >= CUR_CAPACITY(UPG_DEKU_NUTS))) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_DEKU_NUT) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 DStickCheck(PlayState* play, EnGirlA* this) {
    if ((CUR_CAPACITY(UPG_DEKU_STICKS) != 0) && (AMMO(ITEM_DEKU_STICK) >= CUR_CAPACITY(UPG_DEKU_STICKS))) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_DEKU_STICK) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 FishCheck(PlayState* play, EnGirlA* this) {
    if (!findEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_BOTTLE_FISH) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 RDrugCheck(PlayState* play, EnGirlA* this) {
    if (!findEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_BOTTLE_POTION_RED) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 GDrugCheck(PlayState* play, EnGirlA* this) {
    if (!findEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_BOTTLE_POTION_GREEN) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 BDrugCheck(PlayState* play, EnGirlA* this) {
    if (!findEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_BOTTLE_POTION_BLUE) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 G_KnifeCheck(PlayState* play, EnGirlA* this) {
    if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BIGGORON) &&
        !CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_SWORD_BIGGORON) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 H_ShieldCheck(PlayState* play, EnGirlA* this) {
    if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SHIELD, EQUIP_INV_SHIELD_HYLIAN)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_SHIELD_HYLIAN) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 D_ShieldCheck(PlayState* play, EnGirlA* this) {
    if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SHIELD, EQUIP_INV_SHIELD_DEKU)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_SHIELD_DEKU) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 G_DressCheck(PlayState* play, EnGirlA* this) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_GORON)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_TUNIC_GORON) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 Z_DressCheck(PlayState* play, EnGirlA* this) {
    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_ZORA)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_TUNIC_ZORA) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 HeartCheck(PlayState* play, EnGirlA* this) {
    if (z_common_data.save.info.playerData.healthCapacity == z_common_data.save.info.playerData.health) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS;
}

static s32 RRMilkCheck(PlayState* play, EnGirlA* this) {
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_BOTTLE_MILK_FULL) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 NiwaEggCheck(PlayState* play, EnGirlA* this) {
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_ZELDAS_LETTER) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 IGMilkCheck(PlayState* play, EnGirlA* this) {
    return CANBUY_RESULT_NEED_RUPEES;
}

s32 IGEggCheck(PlayState* play, EnGirlA* this) {
    return CANBUY_RESULT_NEED_RUPEES;
}

s32 BomchuCheck(PlayState* play, EnGirlA* this) {
    if (AMMO(ITEM_BOMBCHU) >= 50) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_BOMBCHU) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 SeedCheck(PlayState* play, EnGirlA* this) {
    if (AMMO(ITEM_SLINGSHOT) >= CUR_CAPACITY(UPG_BULLET_BAG)) {
        return CANBUY_RESULT_CANT_GET_NOW;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_DEKU_SEEDS) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 SoldoutCheck(PlayState* play, EnGirlA* this) {
    return CANBUY_RESULT_CANT_GET_NOW_5;
}

s32 FireCheck(PlayState* play, EnGirlA* this) {
    if (!findEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_BOTTLE_BLUE_FIRE) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 InsectCheck(PlayState* play, EnGirlA* this) {
    if (!findEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_BOTTLE_BUG) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 GhostCheck(PlayState* play, EnGirlA* this) {
    if (!findEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_BOTTLE_POE) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

s32 SoulCheck(PlayState* play, EnGirlA* this) {
    if (!findEmptyBottle()) {
        return CANBUY_RESULT_NEED_BOTTLE;
    }
    if (z_common_data.save.info.playerData.rupees < this->basePrice) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (item_get_non_setting(ITEM_BOTTLE_FAIRY) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_FANFARE;
    }
    return CANBUY_RESULT_SUCCESS;
}

void ArrowPlus(PlayState* play, EnGirlA* this) {
    item_count_vary(ITEM_BOW, this->itemCount);
    lupy_increase(-this->basePrice);
}

void BombPlus(PlayState* play, EnGirlA* this) {
    switch (this->itemCount) {
        case 5:
            item_get_setting(play, ITEM_BOMBS_5);
            break;
        case 10:
            item_get_setting(play, ITEM_BOMBS_10);
            break;
        case 20:
            item_get_setting(play, ITEM_BOMBS_20);
            break;
        case 30:
            item_get_setting(play, ITEM_BOMBS_30);
            break;
    }
    lupy_increase(-this->basePrice);
}

void DNutsPlus(PlayState* play, EnGirlA* this) {
    switch (this->itemCount) {
        case 5:
            item_get_setting(play, ITEM_DEKU_NUTS_5);
            break;
        case 10:
            item_get_setting(play, ITEM_DEKU_NUTS_10);
            break;
    }
    lupy_increase(-this->basePrice);
}

void DStickPlus(PlayState* play, EnGirlA* this) {
    item_get_setting(play, ITEM_DEKU_STICK);
    lupy_increase(-this->basePrice);
}

void G_KnifePlus(PlayState* play, EnGirlA* this) {
    sword_system_dma(play);
    z_common_data.save.info.playerData.swordHealth = 8;
    lupy_increase(-this->basePrice);
}

void H_ShieldPlus(PlayState* play, EnGirlA* this) {
    item_get_setting(play, ITEM_SHIELD_HYLIAN);
    lupy_increase(-this->basePrice);
}

void D_ShieldPlus(PlayState* play, EnGirlA* this) {
    item_get_setting(play, ITEM_SHIELD_DEKU);
    lupy_increase(-this->basePrice);
}

void G_DressPlus(PlayState* play, EnGirlA* this) {
    item_get_setting(play, ITEM_TUNIC_GORON);
    lupy_increase(-this->basePrice);
}

void Z_DressPlus(PlayState* play, EnGirlA* this) {
    item_get_setting(play, ITEM_TUNIC_ZORA);
    lupy_increase(-this->basePrice);
}

void HeartPlus(PlayState* play, EnGirlA* this) {
    life_meter_play(play, this->itemCount);
    lupy_increase(-this->basePrice);
}

void RRMilkPlus(PlayState* play, EnGirlA* this) {
    item_get_setting(play, ITEM_BOTTLE_MILK_FULL);
    lupy_increase(-this->basePrice);
}

void NiwaEggPlus(PlayState* play, EnGirlA* this) {
    item_get_setting(play, ITEM_WEIRD_EGG);
    lupy_increase(-this->basePrice);
}

void IGMilkPlus(PlayState* play, EnGirlA* this) {
    lupy_increase(-this->basePrice);
}

void IGEggPlus(PlayState* play, EnGirlA* this) {
    lupy_increase(-this->basePrice);
}

void SeedPlus(PlayState* play, EnGirlA* this) {
    item_get_setting(play, ITEM_DEKU_SEEDS_30);
    lupy_increase(-this->basePrice);
}

void BottleItemPlus(PlayState* play, EnGirlA* this) {
    switch (this->actor.params) {
        case SI_FISH:
            item_get_setting(play, ITEM_BOTTLE_FISH);
            break;
        case SI_RED_POTION_R30:
            item_get_setting(play, ITEM_BOTTLE_POTION_RED);
            break;
        case SI_GREEN_POTION:
            item_get_setting(play, ITEM_BOTTLE_POTION_GREEN);
            break;
        case SI_BLUE_POTION:
            item_get_setting(play, ITEM_BOTTLE_POTION_BLUE);
            break;
        case SI_BLUE_FIRE:
            item_get_setting(play, ITEM_BOTTLE_BLUE_FIRE);
            break;
        case SI_BUGS:
            item_get_setting(play, ITEM_BOTTLE_BUG);
            break;
        case SI_BIG_POE:
            item_get_setting(play, ITEM_BOTTLE_BIG_POE);
            break;
        case SI_POE:
            item_get_setting(play, ITEM_BOTTLE_POE);
            break;
        case SI_FAIRY:
            item_get_setting(play, ITEM_BOTTLE_FAIRY);
            break;
    }
    lupy_increase(-this->basePrice);
}

void PaymentOnly(PlayState* play, EnGirlA* this) {
    if (this->actor.params == SI_HYLIAN_SHIELD) {
        if (GET_INFTABLE(INFTABLE_76)) {
            lupy_increase(-(this->basePrice - rnd_price[(s32)rnd_f(7.9f)]));
            return;
        }
    }
    lupy_increase(-this->basePrice);
}

void G_DressFirstPlus(PlayState* play, EnGirlA* this) {
    lupy_increase(-this->basePrice);
}

void Z_DressFirstPlus(PlayState* play, EnGirlA* this) {
    lupy_increase(-this->basePrice);
}

void BomchuFirstPlus(PlayState* play, EnGirlA* this) {
    switch (this->actor.params) {
        case SI_BOMBCHU_10_2:
            SET_ITEMGETINF(ITEMGETINF_06);
            break;
        case SI_BOMBCHU_10_3:
            SET_ITEMGETINF(ITEMGETINF_07);
            break;
        case SI_BOMBCHU_20_3:
            SET_ITEMGETINF(ITEMGETINF_08);
            break;
        case SI_BOMBCHU_20_4:
            SET_ITEMGETINF(ITEMGETINF_09);
            break;
        case SI_BOMBCHU_10_4:
            SET_ITEMGETINF(ITEMGETINF_0A);
            break;
        case SI_BOMBCHU_10_1:
            SET_ITEMGETINF(ITEMGETINF_03);
            break;
        case SI_BOMBCHU_20_1:
            SET_ITEMGETINF(ITEMGETINF_04);
            break;
        case SI_BOMBCHU_20_2:
            SET_ITEMGETINF(ITEMGETINF_05);
            break;
    }
    lupy_increase(-this->basePrice);
}

void sell_item(EnGirlA* this, PlayState* play) {
}

void GirlA_SetItem(PlayState* play, EnGirlA* this) {
    ShopItemEntry* tmp = &GirlA_DrawData[this->actor.params];
    s32 params = this->actor.params;

    if ((this->actor.params >= SI_KEATON_MASK) && (this->actor.params <= SI_MASK_OF_TRUTH)) {
        s32 maskId = this->actor.params - SI_KEATON_MASK;
        s32 isMaskFreeToBorrow = false;

        switch (this->actor.params) {
            case SI_KEATON_MASK:
                if (GET_ITEMGETINF(ITEMGETINF_38)) {
                    isMaskFreeToBorrow = true;
                }
                break;
            case SI_SPOOKY_MASK:
                if (GET_ITEMGETINF(ITEMGETINF_3A)) {
                    isMaskFreeToBorrow = true;
                }
                break;
            case SI_SKULL_MASK:
                if (GET_ITEMGETINF(ITEMGETINF_39)) {
                    isMaskFreeToBorrow = true;
                }
                break;
            case SI_BUNNY_HOOD:
                if (GET_ITEMGETINF(ITEMGETINF_3B)) {
                    isMaskFreeToBorrow = true;
                }
                break;
            case SI_MASK_OF_TRUTH:
                if (GET_ITEMGETINF(ITEMGETINF_3B)) {
                    isMaskFreeToBorrow = true;
                }
                break;
        }
        if (isMaskFreeToBorrow) {
            this->actor.textId = Mask_Message2Data[maskId];
        } else {
            this->actor.textId = tmp->itemDescTextId;
        }
    } else {
        this->actor.textId = tmp->itemDescTextId;
    }
    this->isInvisible = false;
    this->actor.draw = En_GirlA_actor_draw;
}

void GirlA_ClearItem(PlayState* play, EnGirlA* this) {
    this->isInvisible = true;
    this->actor.draw = NULL;
    if ((this->actor.params >= SI_KEATON_MASK) && (this->actor.params <= SI_GERUDO_MASK)) {
        this->actor.textId = 0xBD;
    }
}

void GirlA_ResetItem(PlayState* play, EnGirlA* this) {
    ShopItemEntry* itemEntry;

    if (En_GirlA_arg_data_set(this)) {
        En_GirlA_bank_ID_set(this, play);
        itemEntry = &GirlA_DrawData[this->actor.params];
        this->actor.textId = itemEntry->itemDescTextId;
    } else {
        this->isInvisible = false;
        this->actor.draw = En_GirlA_actor_draw;
    }
}

s32 Mask_Lend_Check(EnGirlA* this, PlayState* play) {
    s32 params;

    if ((this->actor.params >= SI_KEATON_MASK) && (this->actor.params <= SI_GERUDO_MASK)) {
        params = this->actor.params - SI_KEATON_MASK;
        if (INV_CONTENT(ITEM_TRADE_CHILD) == MaskId_Data[params]) {
            GirlA_ClearItem(play, this);
        } else {
            GirlA_SetItem(play, this);
        }
        return true;
    }
    return false;
}

void GirlA_initial(EnGirlA* this, PlayState* play) {
    s16 params = this->actor.params;
    ShopItemEntry* itemEntry = &GirlA_DrawData[params];

    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.objectSlot = this->requiredObjectSlot;
        switch (this->actor.params) {
            case SI_KEATON_MASK:
                if (GET_ITEMGETINF(ITEMGETINF_38)) {
                    this->actor.textId = 0x70B6;
                } else {
                    this->actor.textId = itemEntry->itemDescTextId;
                }
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_SPOOKY_MASK:
                if (GET_ITEMGETINF(ITEMGETINF_3A)) {
                    this->actor.textId = 0x70B5;
                } else {
                    this->actor.textId = itemEntry->itemDescTextId;
                }
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_SKULL_MASK:
                if (GET_ITEMGETINF(ITEMGETINF_39)) {
                    this->actor.textId = 0x70B4;
                } else {
                    this->actor.textId = itemEntry->itemDescTextId;
                }
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_BUNNY_HOOD:
                if (GET_ITEMGETINF(ITEMGETINF_3B)) {
                    this->actor.textId = 0x70B7;
                } else {
                    this->actor.textId = itemEntry->itemDescTextId;
                }
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_MASK_OF_TRUTH:
                if (GET_ITEMGETINF(ITEMGETINF_3B)) {
                    this->actor.textId = 0x70BB;
                    this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                } else {
                    this->actor.textId = itemEntry->itemDescTextId;
                    this->itemBuyPromptTextId = 0xEB;
                }
                break;
            case SI_ZORA_MASK:
                this->actor.textId = itemEntry->itemDescTextId;
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_GORON_MASK:
                this->actor.textId = itemEntry->itemDescTextId;
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            case SI_GERUDO_MASK:
                this->actor.textId = itemEntry->itemDescTextId;
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
            default:
                this->actor.textId = itemEntry->itemDescTextId;
                this->itemBuyPromptTextId = itemEntry->itemBuyPromptTextId;
                break;
        }
        if (!Mask_Lend_Check(this, play)) {
            GirlA_SetItem(play, this);
        }

        this->setOutOfStockFunc = GirlA_ClearItem;
        this->updateStockedItemFunc = GirlA_ResetItem;
        this->getItemId = itemEntry->getItemId;
        this->canBuyFunc = itemEntry->canBuyFunc;
        this->itemGiveFunc = itemEntry->itemGiveFunc;
        this->buyEventFunc = itemEntry->buyEventFunc;
        this->basePrice = itemEntry->price;
        this->itemCount = itemEntry->count;
        this->hiliteFunc = itemEntry->hiliteFunc;
        this->giDrawId = itemEntry->giDrawId;
        PRINTF("%s(%2d)\n", sShopItemDescriptions[params], params);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        Actor_set_scale(&this->actor, 0.25f);
        this->actor.shape.yOffset = 24.0f;
        this->actor.shape.shadowScale = 4.0f;
        this->actor.floorHeight = this->actor.home.pos.y;
        this->actor.gravity = 0.0f;
        En_GirlA_actor_set_process(this, sell_item);
        this->isInitialized = true;
        this->actionFunc2 = GirlA_Move;
        this->isSelected = false;
        this->yRotation = 0;
        this->yRotationInit = this->actor.shape.rot.y;
    }
}

void GirlA_Move(EnGirlA* this, PlayState* play) {
    Actor_set_scale(&this->actor, 0.25f);
    this->actor.shape.yOffset = 24.0f;
    this->actor.shape.shadowScale = 4.0f;
    Mask_Lend_Check(this, play);
    this->actionFunc(this, play);
    Actor_world_to_eye(&this->actor, 5.0f);
    this->actor.shape.rot.x = 0.0f;
    if (this->actor.params != SI_SOLD_OUT) {
        if (this->isSelected) {
            this->yRotation += 0x1F4;
        } else {
            add_calc_short_angle2(&this->yRotation, 0, 10, 0x7D0, 0);
        }
    }
}

void En_GirlA_actor_move(Actor* thisx, PlayState* play) {
    EnGirlA* this = (EnGirlA*)thisx;

    this->actionFunc2(this, play);
}

void Actor_HiliteReflect_custam(Actor* thisx, PlayState* play, s32 flags) {
    Actor_HiliteReflect_set_init(thisx, play, 0);
    Actor_HiliteReflect_xlu_set_init(thisx, play, 0);
}

void En_GirlA_actor_draw(Actor* thisx, PlayState* play) {
    EnGirlA* this = (EnGirlA*)thisx;

    Matrix_rotateY(DEG_TO_RAD((this->yRotation * 360.0f) / 65536.0f), MTXMODE_APPLY);
    if (this->hiliteFunc != NULL) {
        this->hiliteFunc(thisx, play, 0);
    }
    Draw_GetItemType(play, this->giDrawId);
}
