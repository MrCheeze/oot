#include "global.h"
#include "terminal.h"
#include "versions.h"

#define SLOT_SIZE (sizeof(SaveContext) + 0x28)
#define CHECKSUM_SIZE (sizeof(Save) / 2)

#define DEATHS offsetof(SaveContext, save.info.playerData.deaths)
#define NAME offsetof(SaveContext, save.info.playerData.playerName)
#define N64DD offsetof(SaveContext, save.info.playerData.n64ddFlag)
#define HEALTH_CAP offsetof(SaveContext, save.info.playerData.healthCapacity)
#define QUEST offsetof(SaveContext, save.info.inventory.questItems)
#define DEFENSE offsetof(SaveContext, save.info.inventory.defenseHearts)
#if OOT_PAL
#define HEALTH offsetof(SaveContext, save.info.playerData.health)
#endif

#define SLOT_OFFSET(index) (SRAM_HEADER_SIZE + 0x10 + (index * SLOT_SIZE))

#if !PLATFORM_IQUE

#define SRAM_READ(addr, dramAddr, size) ssSRAMReadWrite(addr, dramAddr, size, OS_READ)
#define SRAM_WRITE(addr, dramAddr, size) ssSRAMReadWrite(addr, dramAddr, size, OS_WRITE)

#else

void Sram_ReadWriteIQue(s32 addr, void* dramAddr, size_t size, s32 direction) {
    void* sramAddr;

    addr -= OS_K1_TO_PHYSICAL(0xA8000000);
    sramAddr = (void*)(__osBbSramAddress + addr);
    if (direction == OS_READ) {
        bcopy(sramAddr, dramAddr, size);
    } else if (direction == OS_WRITE) {
        bcopy(dramAddr, sramAddr, size);
    }
}

#define SRAM_READ(addr, dramAddr, size) Sram_ReadWriteIQue(addr, dramAddr, size, OS_READ)
#define SRAM_WRITE(addr, dramAddr, size) Sram_ReadWriteIQue(addr, dramAddr, size, OS_WRITE)

#endif

u16 sram_save_address[] = {
    SLOT_OFFSET(0),
    SLOT_OFFSET(1),
    SLOT_OFFSET(2),
    // the latter three saves are backup saves for the former saves
    SLOT_OFFSET(3),
    SLOT_OFFSET(4),
    SLOT_OFFSET(5),
};

static u8 sram_check_data[] = {
    // TODO: use enums for these
    0, // SRAM_HEADER_SOUND
    0, // SRAM_HEADER_ZTARGET
    0, // SRAM_HEADER_LANGUAGE

    // SRAM_HEADER_MAGIC
    0x98,
    0x09,
    0x10,
    0x21,
    'Z',
    'E',
    'L',
    'D',
    'A',
};

static SavePlayerData zelda_save_Privatef = {
    { '\0', '\0', '\0', '\0', '\0', '\0' }, // newf
    0,                                      // deaths
    {
        FILENAME_SPACE,
        FILENAME_SPACE,
        FILENAME_SPACE,
        FILENAME_SPACE,
        FILENAME_SPACE,
        FILENAME_SPACE,
        FILENAME_SPACE,
        FILENAME_SPACE,
    },                  // playerName
    0,                  // n64ddFlag
    0x30,               // healthCapacity
    0x30,               // defense
    0,                  // magicLevel
    MAGIC_NORMAL_METER, // magic
    0,                  // rupees
    0,                  // swordHealth
    0,                  // naviTimer
    false,              // isMagicAcquired
    0,                  // unk_1F
    false,              // isDoubleMagicAcquired
    false,              // isDoubleDefenseAcquired
    0,                  // bgsFlag
    0,                  // ocarinaGameRoundNum
    {
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE }, // buttonItems
        { SLOT_NONE, SLOT_NONE, SLOT_NONE },            // cButtonSlots
        0,                                              // equipment
    },                                                  // childEquips
    {
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE }, // buttonItems
        { SLOT_NONE, SLOT_NONE, SLOT_NONE },            // cButtonSlots
        0,                                              // equipment
    },                                                  // adultEquips
    0,                                                  // unk_38
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },       // unk_3C
    SCENE_LINKS_HOUSE,                                  // savedSceneId
};

static ItemEquips zelda_save_Used = {
    { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE }, // buttonItems
    { SLOT_NONE, SLOT_NONE, SLOT_NONE },            // cButtonSlots
    0x1100,                                         // equipment
};

static Inventory zelda_save_Table = {
    // items
    {
        ITEM_NONE, // SLOT_DEKU_STICK
        ITEM_NONE, // SLOT_DEKU_NUT
        ITEM_NONE, // SLOT_BOMB
        ITEM_NONE, // SLOT_BOW
        ITEM_NONE, // SLOT_ARROW_FIRE
        ITEM_NONE, // SLOT_DINS_FIRE
        ITEM_NONE, // SLOT_SLINGSHOT
        ITEM_NONE, // SLOT_OCARINA
        ITEM_NONE, // SLOT_BOMBCHU
        ITEM_NONE, // SLOT_HOOKSHOT
        ITEM_NONE, // SLOT_ARROW_ICE
        ITEM_NONE, // SLOT_FARORES_WIND
        ITEM_NONE, // SLOT_BOOMERANG
        ITEM_NONE, // SLOT_LENS_OF_TRUTH
        ITEM_NONE, // SLOT_MAGIC_BEAN
        ITEM_NONE, // SLOT_HAMMER
        ITEM_NONE, // SLOT_ARROW_LIGHT
        ITEM_NONE, // SLOT_NAYRUS_LOVE
        ITEM_NONE, // SLOT_BOTTLE_1
        ITEM_NONE, // SLOT_BOTTLE_2
        ITEM_NONE, // SLOT_BOTTLE_3
        ITEM_NONE, // SLOT_BOTTLE_4
        ITEM_NONE, // SLOT_TRADE_ADULT
        ITEM_NONE, // SLOT_TRADE_CHILD
    },
    // ammo
    {
        0, // SLOT_DEKU_STICK
        0, // SLOT_DEKU_NUT
        0, // SLOT_BOMB
        0, // SLOT_BOW
        0, // SLOT_ARROW_FIRE
        0, // SLOT_DINS_FIRE
        0, // SLOT_SLINGSHOT
        0, // SLOT_OCARINA
        0, // SLOT_BOMBCHU
        0, // SLOT_HOOKSHOT
        0, // SLOT_ARROW_ICE
        0, // SLOT_FARORES_WIND
        0, // SLOT_BOOMERANG
        0, // SLOT_LENS_OF_TRUTH
        0, // SLOT_MAGIC_BEAN
        0, // SLOT_HAMMER
    },
    // equipment
    (((1 << EQUIP_INV_TUNIC_KOKIRI) << (EQUIP_TYPE_TUNIC * 4)) |
     ((1 << EQUIP_INV_BOOTS_KOKIRI) << (EQUIP_TYPE_BOOTS * 4))),
    0,                                                              // upgrades
    0,                                                              // questItems
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // dungeonItems
    {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    }, // dungeonKeys
    0, // defenseHearts
    0, // gsTokens
};

static Checksum zelda_save_Save = { 0 };

/**
 *  Initialize new save.
 *  This save has an empty inventory with 3 hearts and single magic.
 */
void save_initialize(void) {
    bzero(&z_common_data.save.info, sizeof(SaveInfo));
    z_common_data.save.totalDays = 0;
    z_common_data.save.bgsDayCount = 0;

    z_common_data.save.info.playerData = zelda_save_Privatef;
    z_common_data.save.info.equips = zelda_save_Used;
    z_common_data.save.info.inventory = zelda_save_Table;
    z_common_data.save.info.checksum = zelda_save_Save;

    z_common_data.save.info.horseData.sceneId = SCENE_HYRULE_FIELD;
    z_common_data.save.info.horseData.pos.x = -1840;
    z_common_data.save.info.horseData.pos.y = 72;
    z_common_data.save.info.horseData.pos.z = 5497;
    z_common_data.save.info.horseData.angle = -0x6AD9;
    z_common_data.save.info.playerData.magicLevel = 0;
    z_common_data.save.info.infTable[INFTABLE_INDEX_1DX] = 1;
    z_common_data.save.info.sceneFlags[SCENE_WATER_TEMPLE].swch = 0x40000000;
}

static SavePlayerData zelda_save_Privatef999 = {
    { 'Z', 'E', 'L', 'D', 'A', 'Z' }, // newf
    0,                                // deaths
#if OOT_VERSION < PAL_1_0
    {
        0x81, // リ
        0x87, // ン
        0x61, // ク
        FILENAME_SPACE,
        FILENAME_SPACE,
        FILENAME_SPACE,
        FILENAME_SPACE,
        FILENAME_SPACE,
    }, // playerName
#else
    {
        FILENAME_UPPERCASE('L'),
        FILENAME_UPPERCASE('I'),
        FILENAME_UPPERCASE('N'),
        FILENAME_UPPERCASE('K'),
        FILENAME_SPACE,
        FILENAME_SPACE,
        FILENAME_SPACE,
        FILENAME_SPACE,
    }, // playerName
#endif
    0,                  // n64ddFlag
    0xE0,               // healthCapacity
    0xE0,               // health
    0,                  // magicLevel
    MAGIC_NORMAL_METER, // magic
    150,                // rupees
    8,                  // swordHealth
    0,                  // naviTimer
    true,               // isMagicAcquired
    0,                  // unk_1F
    false,              // isDoubleMagicAcquired
    false,              // isDoubleDefenseAcquired
    0,                  // bgsFlag
    0,                  // ocarinaGameRoundNum
    {
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE }, // buttonItems
        { SLOT_NONE, SLOT_NONE, SLOT_NONE },            // cButtonSlots
        0,                                              // equipment
    },                                                  // childEquips
    {
        { ITEM_NONE, ITEM_NONE, ITEM_NONE, ITEM_NONE }, // buttonItems
        { SLOT_NONE, SLOT_NONE, SLOT_NONE },            // cButtonSlots
        0,                                              // equipment
    },                                                  // adultEquips
    0,                                                  // unk_38
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },       // unk_3C
    SCENE_HYRULE_FIELD,                                 // savedSceneId
};

static ItemEquips zelda_save_Used999 = {
    { ITEM_SWORD_MASTER, ITEM_BOW, ITEM_BOMB, ITEM_OCARINA_FAIRY }, // buttonItems
    { SLOT_BOW, SLOT_BOMB, SLOT_OCARINA },                          // cButtonSlots
    // equipment
    (EQUIP_VALUE_SWORD_MASTER << (EQUIP_TYPE_SWORD * 4)) | (EQUIP_VALUE_SHIELD_HYLIAN << (EQUIP_TYPE_SHIELD * 4)) |
        (EQUIP_VALUE_TUNIC_KOKIRI << (EQUIP_TYPE_TUNIC * 4)) | (EQUIP_VALUE_BOOTS_KOKIRI << (EQUIP_TYPE_BOOTS * 4)),
};

static Inventory zelda_save_Table999 = {
    // items
    {
        ITEM_DEKU_STICK,          // SLOT_DEKU_STICK
        ITEM_DEKU_NUT,            // SLOT_DEKU_NUT
        ITEM_BOMB,                // SLOT_BOMB
        ITEM_BOW,                 // SLOT_BOW
        ITEM_ARROW_FIRE,          // SLOT_ARROW_FIRE
        ITEM_DINS_FIRE,           // SLOT_DINS_FIRE
        ITEM_SLINGSHOT,           // SLOT_SLINGSHOT
        ITEM_OCARINA_FAIRY,       // SLOT_OCARINA
        ITEM_BOMBCHU,             // SLOT_BOMBCHU
        ITEM_HOOKSHOT,            // SLOT_HOOKSHOT
        ITEM_ARROW_ICE,           // SLOT_ARROW_ICE
        ITEM_FARORES_WIND,        // SLOT_FARORES_WIND
        ITEM_BOOMERANG,           // SLOT_BOOMERANG
        ITEM_LENS_OF_TRUTH,       // SLOT_LENS_OF_TRUTH
        ITEM_MAGIC_BEAN,          // SLOT_MAGIC_BEAN
        ITEM_HAMMER,              // SLOT_HAMMER
        ITEM_ARROW_LIGHT,         // SLOT_ARROW_LIGHT
        ITEM_NAYRUS_LOVE,         // SLOT_NAYRUS_LOVE
        ITEM_BOTTLE_EMPTY,        // SLOT_BOTTLE_1
        ITEM_BOTTLE_POTION_RED,   // SLOT_BOTTLE_2
        ITEM_BOTTLE_POTION_GREEN, // SLOT_BOTTLE_3
        ITEM_BOTTLE_POTION_BLUE,  // SLOT_BOTTLE_4
        ITEM_POCKET_EGG,          // SLOT_TRADE_ADULT
        ITEM_WEIRD_EGG,           // SLOT_TRADE_CHILD
    },
    // ammo
    {
        50, // SLOT_DEKU_STICK
        50, // SLOT_DEKU_NUT
        10, // SLOT_BOMB
        30, // SLOT_BOW
        1,  // SLOT_ARROW_FIRE
        1,  // SLOT_DINS_FIRE
        30, // SLOT_SLINGSHOT
        1,  // SLOT_OCARINA
        50, // SLOT_BOMBCHU
        1,  // SLOT_HOOKSHOT
        1,  // SLOT_ARROW_ICE
        1,  // SLOT_FARORES_WIND
        1,  // SLOT_BOOMERANG
        1,  // SLOT_LENS_OF_TRUTH
        1,  // SLOT_MAGIC_BEAN
        1   // SLOT_HAMMER
    },
    // equipment
    ((((1 << EQUIP_INV_SWORD_KOKIRI) << (EQUIP_TYPE_SWORD * 4)) |
      ((1 << EQUIP_INV_SWORD_MASTER) << (EQUIP_TYPE_SWORD * 4)) |
      ((1 << EQUIP_INV_SWORD_BIGGORON) << (EQUIP_TYPE_SWORD * 4))) |
     (((1 << EQUIP_INV_SHIELD_DEKU) << (EQUIP_TYPE_SHIELD * 4)) |
      ((1 << EQUIP_INV_SHIELD_HYLIAN) << (EQUIP_TYPE_SHIELD * 4)) |
      ((1 << EQUIP_INV_SHIELD_MIRROR) << (EQUIP_TYPE_SHIELD * 4))) |
     (((1 << EQUIP_INV_TUNIC_KOKIRI) << (EQUIP_TYPE_TUNIC * 4)) |
      ((1 << EQUIP_INV_TUNIC_GORON) << (EQUIP_TYPE_TUNIC * 4)) |
      ((1 << EQUIP_INV_TUNIC_ZORA) << (EQUIP_TYPE_TUNIC * 4))) |
     (((1 << EQUIP_INV_BOOTS_KOKIRI) << (EQUIP_TYPE_BOOTS * 4)) |
      ((1 << EQUIP_INV_BOOTS_IRON) << (EQUIP_TYPE_BOOTS * 4)) |
      ((1 << EQUIP_INV_BOOTS_HOVER) << (EQUIP_TYPE_BOOTS * 4)))),
    0x125249,                                                       // upgrades
    0x1E3FFFF,                                                      // questItems
    { 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // dungeonItems
    { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 },    // dungeonKeys
    0,                                                              // defenseHearts
    0,                                                              // gsTokens
};

static Checksum zelda_save_Save999 = { 0 };

/**
 *  Initialize debug save. This is also used on the Title Screen
 *  This save has a mostly full inventory with 10 hearts and single magic.
 *
 *  Some noteable flags that are set:
 *  Showed Mido sword/shield, met Deku Tree, Deku Tree mouth opened,
 *  used blue warp in Gohmas room, Zelda fled castle, light arrow cutscene watched,
 *  and set water level in Water Temple to lowest level.
 */
void save_initialize999(void) {
    bzero(&z_common_data.save.info, sizeof(SaveInfo));
    z_common_data.save.totalDays = 0;
    z_common_data.save.bgsDayCount = 0;

    z_common_data.save.info.playerData = zelda_save_Privatef999;
    z_common_data.save.info.equips = zelda_save_Used999;
    z_common_data.save.info.inventory = zelda_save_Table999;
    z_common_data.save.info.checksum = zelda_save_Save999;

    z_common_data.save.info.horseData.sceneId = SCENE_HYRULE_FIELD;
    z_common_data.save.info.horseData.pos.x = -1840;
    z_common_data.save.info.horseData.pos.y = 72;
    z_common_data.save.info.horseData.pos.z = 5497;
    z_common_data.save.info.horseData.angle = -0x6AD9;
    z_common_data.save.info.infTable[INFTABLE_INDEX_0] |= INFTABLE_MASK(INFTABLE_00) | INFTABLE_MASK(INFTABLE_03) |
                                                         INFTABLE_MASK(INFTABLE_0C) | INFTABLE_MASK(INFTABLE_0E);

    z_common_data.save.info.eventChkInf[EVENTCHKINF_INDEX_0] |=
        EVENTCHKINF_MASK(EVENTCHKINF_00_UNUSED) | EVENTCHKINF_MASK(EVENTCHKINF_01_UNUSED) |
        EVENTCHKINF_MASK(EVENTCHKINF_MIDO_DENIED_DEKU_TREE_ACCESS) | EVENTCHKINF_MASK(EVENTCHKINF_03) |
        EVENTCHKINF_MASK(EVENTCHKINF_04) | EVENTCHKINF_MASK(EVENTCHKINF_05) | EVENTCHKINF_MASK(EVENTCHKINF_09) |
        EVENTCHKINF_MASK(EVENTCHKINF_0C);

    SET_EVENTCHKINF(EVENTCHKINF_80);
    SET_EVENTCHKINF(EVENTCHKINF_C4);

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        z_common_data.save.info.equips.buttonItems[0] = ITEM_SWORD_KOKIRI;
        SetEquip_Item(EQUIP_TYPE_SWORD, EQUIP_VALUE_SWORD_KOKIRI);
        if (z_common_data.fileNum == 0xFF) {
            z_common_data.save.info.equips.buttonItems[1] = ITEM_SLINGSHOT;
            z_common_data.save.info.equips.cButtonSlots[0] = SLOT_SLINGSHOT;
            SetEquip_Item(EQUIP_TYPE_SHIELD, EQUIP_VALUE_SHIELD_DEKU);
        }
    }

    z_common_data.save.entranceIndex = ENTR_HYRULE_FIELD_0;
    z_common_data.save.info.playerData.magicLevel = 0;
    z_common_data.save.info.sceneFlags[SCENE_WATER_TEMPLE].swch = 0x40000000;
}

static s16 scene_table[] = {
    ENTR_DEKU_TREE_0,                      // SCENE_DEKU_TREE
    ENTR_DODONGOS_CAVERN_0,                // SCENE_DODONGOS_CAVERN
    ENTR_JABU_JABU_0,                      // SCENE_JABU_JABU
    ENTR_FOREST_TEMPLE_0,                  // SCENE_FOREST_TEMPLE
    ENTR_FIRE_TEMPLE_0,                    // SCENE_FIRE_TEMPLE
    ENTR_WATER_TEMPLE_0,                   // SCENE_WATER_TEMPLE
    ENTR_SPIRIT_TEMPLE_0,                  // SCENE_SPIRIT_TEMPLE
    ENTR_SHADOW_TEMPLE_0,                  // SCENE_SHADOW_TEMPLE
    ENTR_BOTTOM_OF_THE_WELL_0,             // SCENE_BOTTOM_OF_THE_WELL
    ENTR_ICE_CAVERN_0,                     // SCENE_ICE_CAVERN
    ENTR_GANONS_TOWER_0,                   // SCENE_GANONS_TOWER
    ENTR_GERUDO_TRAINING_GROUND_0,         // SCENE_GERUDO_TRAINING_GROUND
    ENTR_THIEVES_HIDEOUT_0,                // SCENE_THIEVES_HIDEOUT
    ENTR_INSIDE_GANONS_CASTLE_0,           // SCENE_INSIDE_GANONS_CASTLE
    ENTR_GANONS_TOWER_COLLAPSE_INTERIOR_0, // SCENE_GANONS_TOWER_COLLAPSE_INTERIOR
    ENTR_INSIDE_GANONS_CASTLE_COLLAPSE_0,  // SCENE_INSIDE_GANONS_CASTLE_COLLAPSE
};

/**
 *  Copy save currently on the buffer to Save Context and complete various tasks to open the save.
 *  This includes:
 *  - Set proper entrance depending on where the game was saved
 *  - If health is less than 3 hearts, give 3 hearts
 *  - If either scarecrow song is set, copy them from save context to the proper location
 *  - Handle a case where the player saved and quit after zelda cutscene but didnt get the song
 *  - Give and equip master sword if player is adult and doesn't have master sword
 *  - Revert any trade items that spoil
 */
void sram_load_check(SramContext* sramCtx) {
    u16 i;
    u16 j;
    u8* ptr;

    PRINTF(T("個人Ｆｉｌｅ作成\n", "Create personal file\n"));
    i = sram_save_address[z_common_data.fileNum];
    PRINTF(T("ぽいんと＝%x(%d)\n", "Point=%x(%d)\n"), i, z_common_data.fileNum);

    Memcpy(&z_common_data, sramCtx->readBuff + i, sizeof(Save));

    PRINTF_COLOR_YELLOW();
    PRINTF("SCENE_DATA_ID = %d   SceneNo = %d\n", z_common_data.save.info.playerData.savedSceneId,
           ((void)0, z_common_data.save.entranceIndex));

    switch (z_common_data.save.info.playerData.savedSceneId) {
        case SCENE_DEKU_TREE:
        case SCENE_DODONGOS_CAVERN:
        case SCENE_JABU_JABU:
        case SCENE_FOREST_TEMPLE:
        case SCENE_FIRE_TEMPLE:
        case SCENE_WATER_TEMPLE:
        case SCENE_SPIRIT_TEMPLE:
        case SCENE_SHADOW_TEMPLE:
        case SCENE_BOTTOM_OF_THE_WELL:
        case SCENE_ICE_CAVERN:
        case SCENE_GANONS_TOWER:
        case SCENE_GERUDO_TRAINING_GROUND:
        case SCENE_THIEVES_HIDEOUT:
        case SCENE_INSIDE_GANONS_CASTLE:
            z_common_data.save.entranceIndex = scene_table[z_common_data.save.info.playerData.savedSceneId];
            break;

        case SCENE_DEKU_TREE_BOSS:
            z_common_data.save.entranceIndex = ENTR_DEKU_TREE_0;
            break;

        case SCENE_DODONGOS_CAVERN_BOSS:
            z_common_data.save.entranceIndex = ENTR_DODONGOS_CAVERN_0;
            break;

        case SCENE_JABU_JABU_BOSS:
            z_common_data.save.entranceIndex = ENTR_JABU_JABU_0;
            break;

        case SCENE_FOREST_TEMPLE_BOSS:
            z_common_data.save.entranceIndex = ENTR_FOREST_TEMPLE_0;
            break;

        case SCENE_FIRE_TEMPLE_BOSS:
            z_common_data.save.entranceIndex = ENTR_FIRE_TEMPLE_0;
            break;

        case SCENE_WATER_TEMPLE_BOSS:
            z_common_data.save.entranceIndex = ENTR_WATER_TEMPLE_0;
            break;

        case SCENE_SPIRIT_TEMPLE_BOSS:
            z_common_data.save.entranceIndex = ENTR_SPIRIT_TEMPLE_0;
            break;

        case SCENE_SHADOW_TEMPLE_BOSS:
            z_common_data.save.entranceIndex = ENTR_SHADOW_TEMPLE_0;
            break;

        case SCENE_GANONS_TOWER_COLLAPSE_INTERIOR:
        case SCENE_INSIDE_GANONS_CASTLE_COLLAPSE:
        case SCENE_GANONDORF_BOSS:
        case SCENE_GANONS_TOWER_COLLAPSE_EXTERIOR:
        case SCENE_GANON_BOSS:
            z_common_data.save.entranceIndex = ENTR_GANONS_TOWER_0;
            break;

        default:
            if (z_common_data.save.info.playerData.savedSceneId != SCENE_LINKS_HOUSE) {
                if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
                    z_common_data.save.entranceIndex = ENTR_LINKS_HOUSE_0;
                } else {
                    z_common_data.save.entranceIndex = ENTR_TEMPLE_OF_TIME_7;
                }
            } else {
                z_common_data.save.entranceIndex = ENTR_LINKS_HOUSE_0;
            }
            break;
    }

    PRINTF("scene_no = %d\n", z_common_data.save.entranceIndex);
    PRINTF_RST();

    if (z_common_data.save.info.playerData.health < 0x30) {
        z_common_data.save.info.playerData.health = 0x30;
    }

    if (z_common_data.save.info.scarecrowLongSongSet) {
        PRINTF_COLOR_BLUE();
        PRINTF("\n====================================================================\n");

        Memcpy(na_oca_rec_buf_ptr, z_common_data.save.info.scarecrowLongSong,
               sizeof(z_common_data.save.info.scarecrowLongSong));

        ptr = (u8*)na_oca_rec_buf_ptr;
        for (i = 0; i < ARRAY_COUNT(z_common_data.save.info.scarecrowLongSong); i++, ptr++) {
            PRINTF("%d, ", *ptr);
        }

        PRINTF("\n====================================================================\n");
        PRINTF_RST();
    }

    if (z_common_data.save.info.scarecrowSpawnSongSet) {
        PRINTF_COLOR_GREEN();
        PRINTF("\n====================================================================\n");

        Memcpy(na_oca_rec_buf2_ptr, z_common_data.save.info.scarecrowSpawnSong,
               sizeof(z_common_data.save.info.scarecrowSpawnSong));

        ptr = na_oca_rec_buf2_ptr;
        for (i = 0; i < ARRAY_COUNT(z_common_data.save.info.scarecrowSpawnSong); i++, ptr++) {
            PRINTF("%d, ", *ptr);
        }

        PRINTF("\n====================================================================\n");
        PRINTF_RST();
    }

    // if zelda cutscene has been watched but lullaby was not obtained, restore cutscene and take away letter
    if (GET_EVENTCHKINF(EVENTCHKINF_40) && !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
        i = z_common_data.save.info.eventChkInf[EVENTCHKINF_INDEX_40];
        i &= ~EVENTCHKINF_MASK(EVENTCHKINF_40);
        z_common_data.save.info.eventChkInf[EVENTCHKINF_INDEX_40] = i;

        INV_CONTENT(ITEM_ZELDAS_LETTER) = ITEM_CHICKEN;

        for (j = 1; j < 4; j++) {
            if (z_common_data.save.info.equips.buttonItems[j] == ITEM_ZELDAS_LETTER) {
                z_common_data.save.info.equips.buttonItems[j] = ITEM_CHICKEN;
            }
        }
    }

    if (LINK_AGE_IN_YEARS == YEARS_ADULT && !CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER)) {
        z_common_data.save.info.inventory.equipment |= OWNED_EQUIP_FLAG(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER);
#if OOT_VERSION >= NTSC_1_1
        z_common_data.save.info.equips.buttonItems[0] = ITEM_SWORD_MASTER;
        z_common_data.save.info.equips.equipment &= ~(0xF << (EQUIP_TYPE_SWORD * 4));
        z_common_data.save.info.equips.equipment |= EQUIP_VALUE_SWORD_MASTER << (EQUIP_TYPE_SWORD * 4);
#endif
    }

    for (i = 0; i < ARRAY_COUNT(Warashibe_item); i++) {
        if (INV_CONTENT(ITEM_TRADE_ADULT) == Warashibe_item[i]) {
            INV_CONTENT(Warashibe_before_item[i]) = Warashibe_before_item[i];

            for (j = 1; j < 4; j++) {
                if (z_common_data.save.info.equips.buttonItems[j] == Warashibe_item[i]) {
                    z_common_data.save.info.equips.buttonItems[j] = Warashibe_before_item[i];
                }
            }
        }
    }

    z_common_data.save.info.playerData.magicLevel = 0;
}

/**
 *  Write the contents of the Save Context to a main and backup slot in SRAM.
 *  Note: The whole Save Context is written even though only the `save` substruct is read back later
 */
void sram_save(SramContext* sramCtx) {
    u16 offset;
    u16 checksum;
    u16 j;
    u16* ptr;

    z_common_data.save.info.checksum.value = 0;

    ptr = (u16*)&z_common_data;
    checksum = j = 0;

    for (offset = 0; offset < CHECKSUM_SIZE; offset++) {
        if (++j == 0x20) {
            j = 0;
        }
        checksum += *ptr++;
    }

    z_common_data.save.info.checksum.value = checksum;

    ptr = (u16*)&z_common_data;
    checksum = 0;

    for (offset = 0; offset < CHECKSUM_SIZE; offset++) {
        if (++j == 0x20) {
            j = 0;
        }
        checksum += *ptr++;
    }

    offset = sram_save_address[z_common_data.fileNum];
    SRAM_WRITE(OS_K1_TO_PHYSICAL(0xA8000000) + offset, &z_common_data, SLOT_SIZE);

    ptr = (u16*)&z_common_data;
    checksum = 0;

    for (offset = 0; offset < CHECKSUM_SIZE; offset++) {
        if (++j == 0x20) {
            j = 0;
        }
        checksum += *ptr++;
    }

    offset = sram_save_address[z_common_data.fileNum + 3];
    SRAM_WRITE(OS_K1_TO_PHYSICAL(0xA8000000) + offset, &z_common_data, SLOT_SIZE);
}

/**
 *  For all 3 slots, verify that the checksum is correct. If corrupted, attempt to load a backup save.
 *  If backup is also corrupted, default to a new save (or debug save for slot 0 on debug rom).
 *
 *  After verifying all 3 saves, pass relevant data to File Select to be displayed.
 */
void sram_start_load(FileSelectState* fileSelect, SramContext* sramCtx) {
    u16 i;
    u16 newChecksum;
    u16 slotNum;
    u16 offset;
    u16 j;
    u16 oldChecksum;
    u16* ptr;
    u16 dayTime;

    PRINTF("ＳＲＡＭ ＳＴＡＲＴ─ＬＯＡＤ\n");
    bzero(sramCtx->readBuff, SRAM_SIZE);
    SRAM_READ(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE);

    dayTime = ((void)0, z_common_data.save.dayTime);

    for (slotNum = 0; slotNum < 3; slotNum++) {
        offset = sram_save_address[slotNum];
        PRINTF(T("ぽいんと＝%x(%d)    SAVE_MAX=%d\n", "Point=%x(%d)    SAVE_MAX=%d\n"), offset, z_common_data.fileNum,
               sizeof(Save));
        Memcpy(&z_common_data, sramCtx->readBuff + offset, sizeof(Save));

        oldChecksum = z_common_data.save.info.checksum.value;
        z_common_data.save.info.checksum.value = 0;
        ptr = (u16*)&z_common_data;
        PRINTF("\n＝＝＝＝＝＝＝＝＝＝＝＝＝  Ｓ（%d） ＝＝＝＝＝＝＝＝＝＝＝＝＝\n", slotNum);

        for (i = newChecksum = j = 0; i < CHECKSUM_SIZE; i++, offset += 2) {
#if OOT_VERSION < PAL_1_0
            if (j) {}
            j += 2;
            if (j == 0x20) {
                j = 0;
            }
#endif
            newChecksum += *ptr++;
        }

        PRINTF(T("\nＳＡＶＥチェックサム計算  j=%x  mmm=%x  ", "\nSAVE checksum calculation  j=%x  mmm=%x  "),
               newChecksum, oldChecksum);

        if (newChecksum != oldChecksum) {
            // checksum didnt match, try backup save
            PRINTF("ＥＲＲＯＲ！！！ ＝ %x(%d)\n", sram_save_address[slotNum], slotNum);
            offset = sram_save_address[slotNum + 3];
            Memcpy(&z_common_data, sramCtx->readBuff + offset, sizeof(Save));

            oldChecksum = z_common_data.save.info.checksum.value;
            z_common_data.save.info.checksum.value = 0;
            ptr = (u16*)&z_common_data;
            PRINTF("================= ＢＡＣＫ─ＵＰ ========================\n");

            for (i = newChecksum = j = 0; i < CHECKSUM_SIZE; i++, offset += 2) {
#if OOT_VERSION < PAL_1_0
                if (j) {}
                j += 2;
                if (j == 0x20) {
                    j = 0;
                }
#endif
                newChecksum += *ptr++;
            }
            PRINTF(T("\n（Ｂ）ＳＡＶＥチェックサム計算  j=%x  mmm=%x  ",
                     "\n(B) SAVE checksum calculation  j=%x  mmm=%x  "),
                   newChecksum, oldChecksum);

            if (newChecksum != oldChecksum) {
                // backup save didnt work, make new save
                PRINTF("ＥＲＲＯＲ！！！ ＝ %x(%d+3)\n", sram_save_address[slotNum + 3], slotNum);
                bzero(&z_common_data.save.entranceIndex, sizeof(s32));
                bzero(&z_common_data.save.linkAge, sizeof(s32));
                bzero(&z_common_data.save.cutsceneIndex, sizeof(s32));
                //! @bug z_common_data.save.dayTime is a u16 but is cleared as a 32-bit value. This is harmless as-is
                //! since it is followed by nightFlag which is also reset here, but can become an issue if the save
                //! layout is changed.
                bzero(&z_common_data.save.dayTime, sizeof(s32));
                bzero(&z_common_data.save.nightFlag, sizeof(s32));
                bzero(&z_common_data.save.totalDays, sizeof(s32));
                bzero(&z_common_data.save.bgsDayCount, sizeof(s32));

#if DEBUG_FEATURES
                if (!slotNum) {
                    save_initialize999();
                    z_common_data.save.info.playerData.newf[0] = 'Z';
                    z_common_data.save.info.playerData.newf[1] = 'E';
                    z_common_data.save.info.playerData.newf[2] = 'L';
                    z_common_data.save.info.playerData.newf[3] = 'D';
                    z_common_data.save.info.playerData.newf[4] = 'A';
                    z_common_data.save.info.playerData.newf[5] = 'Z';
                    PRINTF("newf=%x,%x,%x,%x,%x,%x\n", z_common_data.save.info.playerData.newf[0],
                           z_common_data.save.info.playerData.newf[1], z_common_data.save.info.playerData.newf[2],
                           z_common_data.save.info.playerData.newf[3], z_common_data.save.info.playerData.newf[4],
                           z_common_data.save.info.playerData.newf[5]);
                } else {
                    save_initialize();
                }
#else
                save_initialize();
#endif

                ptr = (u16*)&z_common_data;
                PRINTF("\n--------------------------------------------------------------\n");

                for (i = newChecksum = j = 0; i < CHECKSUM_SIZE; i++) {
                    PRINTF("%x ", *ptr);
                    if (++j == 0x20) {
                        PRINTF("\n");
                        j = 0;
                    }
                    newChecksum += *ptr++;
                }

                z_common_data.save.info.checksum.value = newChecksum;
                PRINTF("\nCheck_Sum=%x(%x)\n", z_common_data.save.info.checksum.value, newChecksum);

                i = sram_save_address[slotNum + 3];
                SRAM_WRITE(OS_K1_TO_PHYSICAL(0xA8000000) + i, &z_common_data, SLOT_SIZE);

                //! @bug The ??= below is interpreted as a trigraph for # by IDO
                PRINTF("??????=%x,%x,%x,%x,%x,%x\n", z_common_data.save.info.playerData.newf[0],
                       z_common_data.save.info.playerData.newf[1], z_common_data.save.info.playerData.newf[2],
                       z_common_data.save.info.playerData.newf[3], z_common_data.save.info.playerData.newf[4],
                       z_common_data.save.info.playerData.newf[5]);
                PRINTF(T("\nぽいんと＝%x(%d+3)  check_sum=%x(%x)\n", "\npoints=%x(%d+3) check_sum=%x(%x)\n"), i,
                       slotNum, z_common_data.save.info.checksum.value, newChecksum);
            }

            i = sram_save_address[slotNum];
            SRAM_WRITE(OS_K1_TO_PHYSICAL(0xA8000000) + i, &z_common_data, SLOT_SIZE);

            PRINTF(T("ぽいんと＝%x(%d)  check_sum=%x(%x)\n", "point=%x(%d) check_sum=%x(%x)\n"), i, slotNum,
                   z_common_data.save.info.checksum.value, newChecksum);
        } else {
            PRINTF(T("\nＳＡＶＥデータ ＯＫ！！！！\n", "\nSAVE data OK!!!!\n"));
        }
    }

    bzero(sramCtx->readBuff, SRAM_SIZE);
    SRAM_READ(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE);
    z_common_data.save.dayTime = dayTime;

    PRINTF("SAVECT=%x, NAME=%x, LIFE=%x, ITEM=%x,  64DD=%x,  HEART=%x\n", DEATHS, NAME, HEALTH_CAP, QUEST, N64DD,
           DEFENSE);

    Memcpy(&fileSelect->deaths[0], sramCtx->readBuff + SLOT_OFFSET(0) + DEATHS, sizeof(fileSelect->deaths[0]));
    Memcpy(&fileSelect->deaths[1], sramCtx->readBuff + SLOT_OFFSET(1) + DEATHS, sizeof(fileSelect->deaths[0]));
    Memcpy(&fileSelect->deaths[2], sramCtx->readBuff + SLOT_OFFSET(2) + DEATHS, sizeof(fileSelect->deaths[0]));

    Memcpy(&fileSelect->fileNames[0], sramCtx->readBuff + SLOT_OFFSET(0) + NAME, sizeof(fileSelect->fileNames[0]));
    Memcpy(&fileSelect->fileNames[1], sramCtx->readBuff + SLOT_OFFSET(1) + NAME, sizeof(fileSelect->fileNames[0]));
    Memcpy(&fileSelect->fileNames[2], sramCtx->readBuff + SLOT_OFFSET(2) + NAME, sizeof(fileSelect->fileNames[0]));

    Memcpy(&fileSelect->healthCapacities[0], sramCtx->readBuff + SLOT_OFFSET(0) + HEALTH_CAP,
           sizeof(fileSelect->healthCapacities[0]));
    Memcpy(&fileSelect->healthCapacities[1], sramCtx->readBuff + SLOT_OFFSET(1) + HEALTH_CAP,
           sizeof(fileSelect->healthCapacities[0]));
    Memcpy(&fileSelect->healthCapacities[2], sramCtx->readBuff + SLOT_OFFSET(2) + HEALTH_CAP,
           sizeof(fileSelect->healthCapacities[0]));

    Memcpy(&fileSelect->questItems[0], sramCtx->readBuff + SLOT_OFFSET(0) + QUEST, sizeof(fileSelect->questItems[0]));
    Memcpy(&fileSelect->questItems[1], sramCtx->readBuff + SLOT_OFFSET(1) + QUEST, sizeof(fileSelect->questItems[0]));
    Memcpy(&fileSelect->questItems[2], sramCtx->readBuff + SLOT_OFFSET(2) + QUEST, sizeof(fileSelect->questItems[0]));

    Memcpy(&fileSelect->n64ddFlags[0], sramCtx->readBuff + SLOT_OFFSET(0) + N64DD, sizeof(fileSelect->n64ddFlags[0]));
    Memcpy(&fileSelect->n64ddFlags[1], sramCtx->readBuff + SLOT_OFFSET(1) + N64DD, sizeof(fileSelect->n64ddFlags[0]));
    Memcpy(&fileSelect->n64ddFlags[2], sramCtx->readBuff + SLOT_OFFSET(2) + N64DD, sizeof(fileSelect->n64ddFlags[0]));

    Memcpy(&fileSelect->defense[0], sramCtx->readBuff + SLOT_OFFSET(0) + DEFENSE, sizeof(fileSelect->defense[0]));
    Memcpy(&fileSelect->defense[1], sramCtx->readBuff + SLOT_OFFSET(1) + DEFENSE, sizeof(fileSelect->defense[0]));
    Memcpy(&fileSelect->defense[2], sramCtx->readBuff + SLOT_OFFSET(2) + DEFENSE, sizeof(fileSelect->defense[0]));

#if OOT_PAL
    Memcpy(&fileSelect->health[0], sramCtx->readBuff + SLOT_OFFSET(0) + HEALTH, sizeof(fileSelect->health[0]));
    Memcpy(&fileSelect->health[1], sramCtx->readBuff + SLOT_OFFSET(1) + HEALTH, sizeof(fileSelect->health[0]));
    Memcpy(&fileSelect->health[2], sramCtx->readBuff + SLOT_OFFSET(2) + HEALTH, sizeof(fileSelect->health[0]));
#endif

    PRINTF("f_64dd=%d, %d, %d\n", fileSelect->n64ddFlags[0], fileSelect->n64ddFlags[1], fileSelect->n64ddFlags[2]);
    PRINTF("heart_status=%d, %d, %d\n", fileSelect->defense[0], fileSelect->defense[1], fileSelect->defense[2]);
#if OOT_PAL
    PRINTF("now_life=%d, %d, %d\n", fileSelect->health[0], fileSelect->health[1], fileSelect->health[2]);
#endif
}

void sram_start_save(FileSelectState* fileSelect, SramContext* sramCtx) {
    u16 offset;
    u16 j;
    u16* ptr;
    u16 checksum;

#if DEBUG_FEATURES
    if (fileSelect->buttonIndex != 0) {
        save_initialize();
    } else {
        save_initialize999();
    }
#else
    save_initialize();
#endif

    z_common_data.save.entranceIndex = ENTR_LINKS_HOUSE_0;
    z_common_data.save.linkAge = LINK_AGE_CHILD;
    z_common_data.save.dayTime = CLOCK_TIME(10, 0);
    z_common_data.save.cutsceneIndex = 0xFFF1;

#if DEBUG_FEATURES
    if (fileSelect->buttonIndex == 0) {
        z_common_data.save.cutsceneIndex = 0;
    }
#endif

    for (offset = 0; offset < 8; offset++) {
#if !PLATFORM_IQUE
        z_common_data.save.info.playerData.playerName[offset] = fileSelect->fileNames[fileSelect->buttonIndex][offset];
#else
        // Workaround for EGCS internal compiler error (see docs/compilers.md)
        u8* fileName = fileSelect->fileNames[fileSelect->buttonIndex];

        z_common_data.save.info.playerData.playerName[offset] = fileName[offset];
#endif
    }

    z_common_data.save.info.playerData.newf[0] = 'Z';
    z_common_data.save.info.playerData.newf[1] = 'E';
    z_common_data.save.info.playerData.newf[2] = 'L';
    z_common_data.save.info.playerData.newf[3] = 'D';
    z_common_data.save.info.playerData.newf[4] = 'A';
    z_common_data.save.info.playerData.newf[5] = 'Z';

    z_common_data.save.info.playerData.n64ddFlag = fileSelect->n64ddFlag;
    PRINTF(T("６４ＤＤフラグ=%d\n", "64DD flags=%d\n"), fileSelect->n64ddFlag);
    PRINTF("newf=%x,%x,%x,%x,%x,%x\n", z_common_data.save.info.playerData.newf[0],
           z_common_data.save.info.playerData.newf[1], z_common_data.save.info.playerData.newf[2],
           z_common_data.save.info.playerData.newf[3], z_common_data.save.info.playerData.newf[4],
           z_common_data.save.info.playerData.newf[5]);
    PRINTF("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");

    ptr = (u16*)&z_common_data;

    for (j = 0, checksum = 0, offset = 0; offset < CHECKSUM_SIZE; offset++) {
        PRINTF("%x ", *ptr);
        checksum += *ptr++;
        if (++j == 0x20) {
            PRINTF("\n");
            j = 0;
        }
    }

    z_common_data.save.info.checksum.value = checksum;
    PRINTF(T("\nチェックサム＝%x\n", "\nChecksum = %x\n"), z_common_data.save.info.checksum.value);

    offset = sram_save_address[z_common_data.fileNum];
    PRINTF("I=%x no=%d\n", offset, z_common_data.fileNum);
    Memcpy(sramCtx->readBuff + offset, &z_common_data, sizeof(Save));

    offset = sram_save_address[z_common_data.fileNum + 3];
    PRINTF("I=%x no=%d\n", offset, z_common_data.fileNum + 3);
    Memcpy(sramCtx->readBuff + offset, &z_common_data, sizeof(Save));

    SRAM_WRITE(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE);

    PRINTF(T("ＳＡＶＥ終了\n", "SAVE end\n"));
    PRINTF("z_common_data.file_no = %d\n", z_common_data.fileNum);
    PRINTF("SAVECT=%x, NAME=%x, LIFE=%x, ITEM=%x,  SAVE_64DD=%x\n", DEATHS, NAME, HEALTH_CAP, QUEST, N64DD);

    j = sram_save_address[z_common_data.fileNum];

    Memcpy(&fileSelect->deaths[z_common_data.fileNum], sramCtx->readBuff + j + DEATHS, sizeof(fileSelect->deaths[0]));
    Memcpy(&fileSelect->fileNames[z_common_data.fileNum], sramCtx->readBuff + j + NAME,
           sizeof(fileSelect->fileNames[0]));
    Memcpy(&fileSelect->healthCapacities[z_common_data.fileNum], sramCtx->readBuff + j + HEALTH_CAP,
           sizeof(fileSelect->healthCapacities[0]));
    Memcpy(&fileSelect->questItems[z_common_data.fileNum], sramCtx->readBuff + j + QUEST,
           sizeof(fileSelect->questItems[0]));
    Memcpy(&fileSelect->n64ddFlags[z_common_data.fileNum], sramCtx->readBuff + j + N64DD,
           sizeof(fileSelect->n64ddFlags[0]));
    Memcpy(&fileSelect->defense[z_common_data.fileNum], sramCtx->readBuff + j + DEFENSE, sizeof(fileSelect->defense[0]));
#if OOT_PAL
    Memcpy(&fileSelect->health[z_common_data.fileNum], sramCtx->readBuff + j + HEALTH, sizeof(fileSelect->health[0]));
#endif

    PRINTF("f_64dd[%d]=%d\n", z_common_data.fileNum, fileSelect->n64ddFlags[z_common_data.fileNum]);
    PRINTF("heart_status[%d]=%d\n", z_common_data.fileNum, fileSelect->defense[z_common_data.fileNum]);
#if OOT_PAL
    PRINTF("now_life[%d]=%d\n", z_common_data.fileNum, fileSelect->health[z_common_data.fileNum]);
#endif
}

void sram_start_clear(FileSelectState* fileSelect, SramContext* sramCtx) {
    u16 offset;

    save_initialize();

    offset = sram_save_address[fileSelect->selectedFileIndex];
    Memcpy(sramCtx->readBuff + offset, &z_common_data, sizeof(Save));
    SRAM_WRITE(OS_K1_TO_PHYSICAL(0xA8000000) + offset, &z_common_data, SLOT_SIZE);

    Memcpy(&fileSelect->n64ddFlags[fileSelect->selectedFileIndex], sramCtx->readBuff + offset + N64DD,
           sizeof(fileSelect->n64ddFlags[0]));

    offset = sram_save_address[fileSelect->selectedFileIndex + 3];
    Memcpy(sramCtx->readBuff + offset, &z_common_data, sizeof(Save));
    SRAM_WRITE(OS_K1_TO_PHYSICAL(0xA8000000) + offset, &z_common_data, SLOT_SIZE);

    PRINTF(T("ＣＬＥＡＲ終了\n", "CLEAR END\n"));
}

void sram_start_copy(FileSelectState* fileSelect, SramContext* sramCtx) {
    u16 offset;

    PRINTF("ＲＥＡＤ=%d(%x)  ＣＯＰＹ=%d(%x)\n", fileSelect->selectedFileIndex,
           sram_save_address[fileSelect->selectedFileIndex], fileSelect->copyDestFileIndex,
           sram_save_address[fileSelect->copyDestFileIndex]);

    offset = sram_save_address[fileSelect->selectedFileIndex];
    Memcpy(&z_common_data, sramCtx->readBuff + offset, sizeof(Save));

    offset = sram_save_address[fileSelect->copyDestFileIndex];
    Memcpy(sramCtx->readBuff + offset, &z_common_data, sizeof(Save));

    offset = sram_save_address[fileSelect->copyDestFileIndex + 3];
    Memcpy(sramCtx->readBuff + offset, &z_common_data, sizeof(Save));

    SRAM_WRITE(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE);

    offset = sram_save_address[fileSelect->copyDestFileIndex];

    Memcpy(&fileSelect->deaths[fileSelect->copyDestFileIndex], sramCtx->readBuff + offset + DEATHS,
           sizeof(fileSelect->deaths[0]));
    Memcpy(&fileSelect->fileNames[fileSelect->copyDestFileIndex], sramCtx->readBuff + offset + NAME,
           sizeof(fileSelect->fileNames[0]));
    Memcpy(&fileSelect->healthCapacities[fileSelect->copyDestFileIndex], sramCtx->readBuff + offset + HEALTH_CAP,
           sizeof(fileSelect->healthCapacities[0]));
    Memcpy(&fileSelect->questItems[fileSelect->copyDestFileIndex], sramCtx->readBuff + offset + QUEST,
           sizeof(fileSelect->questItems[0]));
    Memcpy(&fileSelect->n64ddFlags[fileSelect->copyDestFileIndex], sramCtx->readBuff + offset + N64DD,
           sizeof(fileSelect->n64ddFlags[0]));
    Memcpy(&fileSelect->defense[fileSelect->copyDestFileIndex], sramCtx->readBuff + offset + DEFENSE,
           sizeof(fileSelect->defense[0]));
#if OOT_PAL
    Memcpy(&fileSelect->health[fileSelect->copyDestFileIndex], (sramCtx->readBuff + offset) + HEALTH,
           sizeof(fileSelect->health[0]));
#endif

    PRINTF("f_64dd[%d]=%d\n", z_common_data.fileNum, fileSelect->n64ddFlags[z_common_data.fileNum]);
    PRINTF("heart_status[%d]=%d\n", z_common_data.fileNum, fileSelect->defense[z_common_data.fileNum]);
    PRINTF(T("ＣＯＰＹ終了\n", "Copy end\n"));
}

/**
 *  Write the first 16 bytes of the read buffer to the SRAM header
 */
void sram_sound_save(SramContext* sramCtx) {
    SRAM_WRITE(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_HEADER_SIZE);
}

void sram_initialize(GameState* gameState, SramContext* sramCtx) {
    u16 i;

    PRINTF("sram_initialize( Game *game, Sram *sram )\n");
    SRAM_READ(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE);

    for (i = 0; i < ARRAY_COUNTU(sram_check_data) - SRAM_HEADER_MAGIC; i++) {
        if (sram_check_data[i + SRAM_HEADER_MAGIC] != sramCtx->readBuff[i + SRAM_HEADER_MAGIC]) {
            PRINTF(T("ＳＲＡＭ破壊！！！！！！\n", "SRAM destruction!!!!!!\n"));
#if PLATFORM_GC && OOT_PAL
            z_common_data.language = sramCtx->readBuff[SRAM_HEADER_LANGUAGE];
#endif

            Memcpy(sramCtx->readBuff, sram_check_data, sizeof(sram_check_data));

#if PLATFORM_GC && OOT_PAL
            sramCtx->readBuff[SRAM_HEADER_LANGUAGE] = z_common_data.language;
#endif
            sram_sound_save(sramCtx);
        }
    }

    z_common_data.audioSetting = sramCtx->readBuff[SRAM_HEADER_SOUND] & 3;
    z_common_data.zTargetSetting = sramCtx->readBuff[SRAM_HEADER_ZTARGET] & 1;

#if OOT_PAL
    z_common_data.language = sramCtx->readBuff[SRAM_HEADER_LANGUAGE];
    if (z_common_data.language >= LANGUAGE_MAX) {
        z_common_data.language = LANGUAGE_ENG;
        sramCtx->readBuff[SRAM_HEADER_LANGUAGE] = z_common_data.language;
        sram_sound_save(sramCtx);
    }
#endif

#if DEBUG_FEATURES
    if (CHECK_BTN_ANY(gameState->input[2].cur.button, BTN_DRIGHT)) {
        bzero(sramCtx->readBuff, SRAM_SIZE);
        for (i = 0; i < CHECKSUM_SIZE; i++) {
            sramCtx->readBuff[i] = i;
        }
        SRAM_WRITE(OS_K1_TO_PHYSICAL(0xA8000000), sramCtx->readBuff, SRAM_SIZE);
        PRINTF(T("ＳＲＡＭ破壊！！！！！！\n", "SRAM destruction!!!!!!\n"));
    }
#endif

    PRINTF(T("ＧＯＯＤ！ＧＯＯＤ！ サイズ＝%d + %d ＝ %d\n", "GOOD! GOOD! Size = %d + %d = %d\n"), sizeof(SaveInfo), 4,
           sizeof(SaveInfo) + 4);
    PRINTF_COLOR_BLUE();
    PRINTF("Na_SetSoundOutputMode = %d\n", z_common_data.audioSetting);
    PRINTF("Na_SetSoundOutputMode = %d\n", z_common_data.audioSetting);
    PRINTF("Na_SetSoundOutputMode = %d\n", z_common_data.audioSetting);
    PRINTF_RST();
    Na_SetSoundOutputMode(z_common_data.audioSetting);
}

void sram_title_ct(GameState* gameState, SramContext* sramCtx) {
    sramCtx->readBuff = GAME_STATE_ALLOC(gameState, SRAM_SIZE, "../z_sram.c", 1294);
    ASSERT(sramCtx->readBuff != NULL, "sram->read_buff != NULL", "../z_sram.c", 1295);
}

void sram_ct(PlayState* play, SramContext* sramCtx) {
}
