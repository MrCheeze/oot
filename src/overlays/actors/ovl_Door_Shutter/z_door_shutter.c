/*
 * File: z_door_shutter.c
 * Overlay: ovl_Door_Shutter
 * Description: Sliding doors, Phantom Ganon room bars, Gohma room rock slab
 */

#include "z_door_shutter.h"
#include "overlays/actors/ovl_Boss_Goma/z_boss_goma.h"
#include "quake.h"
#include "versions.h"

#include "assets/objects/object_gnd/object_gnd.h"
#include "assets/objects/object_goma/object_goma.h"
#include "assets/objects/object_ydan_objects/object_ydan_objects.h"
#include "assets/objects/object_ddan_objects/object_ddan_objects.h"
#include "assets/objects/object_bdan_objects/object_bdan_objects.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_bdoor/object_bdoor.h"
#include "assets/objects/object_hidan_objects/object_hidan_objects.h"
#include "assets/objects/object_ganon_objects/object_ganon_objects.h"
#include "assets/objects/object_jya_door/object_jya_door.h"
#include "assets/objects/object_mizu_objects/object_mizu_objects.h"
#include "assets/objects/object_haka_door/object_haka_door.h"
#include "assets/objects/object_ice_objects/object_ice_objects.h"
#include "assets/objects/object_menkuri_objects/object_menkuri_objects.h"
#include "assets/objects/object_demo_kekkai/object_demo_kekkai.h"
#include "assets/objects/object_ouke_haka/object_ouke_haka.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Door_Shutter_actor_ct(Actor* thisx, PlayState* play2);
void Door_Shutter_actor_dt(Actor* thisx, PlayState* play);
void Door_Shutter_actor_move(Actor* thisx, PlayState* play);
void Door_Shutter_actor_draw(Actor* thisx, PlayState* play);

void door_jisin(PlayState* play, s16 quakeY, s16 quakeDuration, s16 camId);
static void move_dma_wait(DoorShutter* this, PlayState* play);
static void move_room_clear_wait(DoorShutter* this, PlayState* play);
void move_non(DoorShutter* this, PlayState* play);
static void move_wait(DoorShutter* this, PlayState* play);
void move_switch_wait(DoorShutter* this, PlayState* play);
void move_switch_close_wait(DoorShutter* this, PlayState* play);
static void move_open(DoorShutter* this, PlayState* play);
void move_switch_open(DoorShutter* this, PlayState* play);
static void move_close(DoorShutter* this, PlayState* play);
void move_bdan_close(DoorShutter* this, PlayState* play);
static void move_demo_wait(DoorShutter* this, PlayState* play);
void move_goma_door_down(DoorShutter* this, PlayState* play);
void move_goma_door_wait(DoorShutter* this, PlayState* play);
void move_fhg_saku(DoorShutter* this, PlayState* play);

ActorProfile Door_Shutter_Profile = {
    /**/ ACTOR_DOOR_SHUTTER,
    /**/ ACTORCAT_DOOR,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(DoorShutter),
    /**/ Door_Shutter_actor_ct,
    /**/ Door_Shutter_actor_dt,
    /**/ Door_Shutter_actor_move,
    /**/ Door_Shutter_actor_draw,
};

typedef enum DoorShutterGfxType {
    /*  0 */ DOORSHUTTER_GFX_DEKU_TREE_1,
    /*  1 */ DOORSHUTTER_GFX_DEKU_TREE_2,
    /*  2 */ DOORSHUTTER_GFX_DODONGOS_CAVERN,
    /*  3 */ DOORSHUTTER_GFX_JABU_JABU,
    /*  4 */ DOORSHUTTER_GFX_PHANTOM_GANON_BARS,
    /*  5 */ DOORSHUTTER_GFX_GOHMA_BLOCK,
    /*  6 */ DOORSHUTTER_GFX_SPIRIT_TEMPLE,
    /*  7 */ DOORSHUTTER_GFX_BOSS_DOOR,
    /*  8 */ DOORSHUTTER_GFX_GENERIC,
    /*  9 */ DOORSHUTTER_GFX_FIRE_TEMPLE_1,
    /* 10 */ DOORSHUTTER_GFX_FIRE_TEMPLE_2,
    /* 11 */ DOORSHUTTER_GFX_GANONS_TOWER,
    /* 12 */ DOORSHUTTER_GFX_WATER_TEMPLE_1,
    /* 13 */ DOORSHUTTER_GFX_WATER_TEMPLE_2,
    /* 14 */ DOORSHUTTER_GFX_SHADOW_TEMPLE_1,
    /* 15 */ DOORSHUTTER_GFX_SHADOW_TEMPLE_2,
    /* 16 */ DOORSHUTTER_GFX_ICE_CAVERN,
    /* 17 */ DOORSHUTTER_GFX_GERUDO_TRAINING_GROUND,
    /* 18 */ DOORSHUTTER_GFX_GANONS_CASTLE,
    /* 19 */ DOORSHUTTER_GFX_ROYAL_FAMILYS_TOMB
} DoorShutterGfxType;

typedef enum DoorShutterStyleType {
    /* -1 */ DOORSHUTTER_STYLE_FROM_SCENE = -1, // Style is taken from `scene_door_shutter_status`
    /*  0 */ DOORSHUTTER_STYLE_PHANTOM_GANON,
    /*  1 */ DOORSHUTTER_STYLE_GOHMA_BLOCK,
    /*  2 */ DOORSHUTTER_STYLE_DEKU_TREE,
    /*  3 */ DOORSHUTTER_STYLE_DODONGOS_CAVERN,
    /*  4 */ DOORSHUTTER_STYLE_JABU_JABU,
    /*  5 */ DOORSHUTTER_STYLE_FOREST_TEMPLE,
    /*  6 */ DOORSHUTTER_STYLE_BOSS_DOOR,
    /*  7 */ DOORSHUTTER_STYLE_GENERIC, // Default for some `DoorShutterType`s
    /*  8 */ DOORSHUTTER_STYLE_FIRE_TEMPLE,
    /*  9 */ DOORSHUTTER_STYLE_GANONS_TOWER,
    /* 10 */ DOORSHUTTER_STYLE_SPIRIT_TEMPLE,
    /* 11 */ DOORSHUTTER_STYLE_WATER_TEMPLE,
    /* 12 */ DOORSHUTTER_STYLE_SHADOW_TEMPLE,
    /* 13 */ DOORSHUTTER_STYLE_ICE_CAVERN,
    /* 14 */ DOORSHUTTER_STYLE_GERUDO_TRAINING_GROUND,
    /* 15 */ DOORSHUTTER_STYLE_GANONS_CASTLE,
    /* 16 */ DOORSHUTTER_STYLE_ROYAL_FAMILYS_TOMB
} DoorShutterStyleType;

typedef struct DoorShutterStyleInfo {
    s16 objectId;
    u8 gfxType1;
    u8 gfxType2;
} DoorShutterStyleInfo;

static DoorShutterStyleInfo door_shutter_shape_data[] = {
    /* DOORSHUTTER_STYLE_PHANTOM_GANON */
    {
        OBJECT_GND,
        DOORSHUTTER_GFX_PHANTOM_GANON_BARS,
        DOORSHUTTER_GFX_PHANTOM_GANON_BARS,
    },
    /* DOORSHUTTER_STYLE_GOHMA_BLOCK */
    {
        OBJECT_GOMA,
        DOORSHUTTER_GFX_GOHMA_BLOCK,
        DOORSHUTTER_GFX_GOHMA_BLOCK,
    },
    /* DOORSHUTTER_STYLE_DEKU_TREE */
    {
        OBJECT_YDAN_OBJECTS,
        DOORSHUTTER_GFX_DEKU_TREE_1,
        DOORSHUTTER_GFX_DEKU_TREE_2,
    },
    /* DOORSHUTTER_STYLE_DODONGOS_CAVERN */
    {
        OBJECT_DDAN_OBJECTS,
        DOORSHUTTER_GFX_DODONGOS_CAVERN,
        DOORSHUTTER_GFX_DODONGOS_CAVERN,
    },
    /* DOORSHUTTER_STYLE_JABU_JABU */
    {
        OBJECT_BDAN_OBJECTS,
        DOORSHUTTER_GFX_JABU_JABU,
        DOORSHUTTER_GFX_JABU_JABU,
    },
    /* DOORSHUTTER_STYLE_FOREST_TEMPLE */
    {
        OBJECT_GAMEPLAY_KEEP,
        DOORSHUTTER_GFX_GENERIC,
        DOORSHUTTER_GFX_GENERIC,
    },
    /* DOORSHUTTER_STYLE_BOSS_DOOR */
    {
        OBJECT_BDOOR,
        DOORSHUTTER_GFX_BOSS_DOOR,
        DOORSHUTTER_GFX_BOSS_DOOR,
    },
    /* DOORSHUTTER_STYLE_GENERIC */
    {
        OBJECT_GAMEPLAY_KEEP,
        DOORSHUTTER_GFX_GENERIC,
        DOORSHUTTER_GFX_GENERIC,
    },
    /* DOORSHUTTER_STYLE_FIRE_TEMPLE */
    {
        OBJECT_HIDAN_OBJECTS,
        DOORSHUTTER_GFX_FIRE_TEMPLE_1,
        DOORSHUTTER_GFX_FIRE_TEMPLE_2,
    },
    /* DOORSHUTTER_STYLE_GANONS_TOWER */
    {
        OBJECT_GANON_OBJECTS,
        DOORSHUTTER_GFX_GANONS_TOWER,
        DOORSHUTTER_GFX_GANONS_TOWER,
    },
    /* DOORSHUTTER_STYLE_SPIRIT_TEMPLE */
    {
        OBJECT_JYA_DOOR,
        DOORSHUTTER_GFX_SPIRIT_TEMPLE,
        DOORSHUTTER_GFX_SPIRIT_TEMPLE,
    },
    /* DOORSHUTTER_STYLE_WATER_TEMPLE */
    {
        OBJECT_MIZU_OBJECTS,
        DOORSHUTTER_GFX_WATER_TEMPLE_1,
        DOORSHUTTER_GFX_WATER_TEMPLE_2,
    },
    /* DOORSHUTTER_STYLE_SHADOW_TEMPLE */
    {
        OBJECT_HAKA_DOOR,
        DOORSHUTTER_GFX_SHADOW_TEMPLE_1,
        DOORSHUTTER_GFX_SHADOW_TEMPLE_2,
    },
    /* DOORSHUTTER_STYLE_ICE_CAVERN */
    {
        OBJECT_ICE_OBJECTS,
        DOORSHUTTER_GFX_ICE_CAVERN,
        DOORSHUTTER_GFX_ICE_CAVERN,
    },
    /* DOORSHUTTER_STYLE_GERUDO_TRAINING_GROUND */
    {
        OBJECT_MENKURI_OBJECTS,
        DOORSHUTTER_GFX_GERUDO_TRAINING_GROUND,
        DOORSHUTTER_GFX_GERUDO_TRAINING_GROUND,
    },
    /* DOORSHUTTER_STYLE_GANONS_CASTLE */
    {
        OBJECT_DEMO_KEKKAI,
        DOORSHUTTER_GFX_GANONS_CASTLE,
        DOORSHUTTER_GFX_GANONS_CASTLE,
    },
    /* DOORSHUTTER_STYLE_ROYAL_FAMILYS_TOMB */
    {
        OBJECT_OUKE_HAKA,
        DOORSHUTTER_GFX_ROYAL_FAMILYS_TOMB,
        DOORSHUTTER_GFX_ROYAL_FAMILYS_TOMB,
    },
};

typedef struct DoorShutterGfxInfo {
    /* 0x0000 */ Gfx* doorDL;
    /* 0x0004 */ Gfx* barsDL;
    /* 0x0008 */ u8 barsOpenOffsetY;
    /* 0x0009 */ u8 barsOffsetZ;
    /* 0x000A */ u8 rangeSides;
    /* 0x000B */ u8 rangeY;
} DoorShutterGfxInfo;

static DoorShutterGfxInfo door_shutter_shape_info[] = {
    { gDTDungeonDoor1DL, gDoorMetalBarsDL, 130, 12, 20, 15 }, // DOORSHUTTER_GFX_DEKU_TREE_1
    { gDTDungeonDoor2DL, gDoorMetalBarsDL, 130, 12, 20, 15 }, // DOORSHUTTER_GFX_DEKU_TREE_2
    { gDodongoDoorDL, gDodongoBarsDL, 240, 14, 70, 15 },      // DOORSHUTTER_GFX_DODONGOS_CAVERN
#if OOT_VERSION < NTSC_1_1
    { gJabuDoorSection1DL, gJabuWebDoorDL, 0, 110, 70, 15 }, // DOORSHUTTER_GFX_JABU_JABU
    { gPhantomGanonBarsDL, NULL, 130, 12, 70, 15 },          // DOORSHUTTER_GFX_PHANTOM_GANON_BARS
    { gGohmaDoorDL, NULL, 130, 12, 70, 15 },                 // DOORSHUTTER_GFX_GOHMA_BLOCK
    { gSpiritDoorDL, gJyaDoorMetalBarsDL, 240, 14, 50, 15 }, // DOORSHUTTER_GFX_SPIRIT_TEMPLE
    { gBossDoorDL, NULL, 130, 12, 70, 15 },                  // DOORSHUTTER_GFX_BOSS_DOOR
#else
    { gJabuDoorSection1DL, gJabuWebDoorDL, 0, 110, 50, 15 }, // DOORSHUTTER_GFX_JABU_JABU
    { gPhantomGanonBarsDL, NULL, 130, 12, 50, 15 },          // DOORSHUTTER_GFX_PHANTOM_GANON_BARS
    { gGohmaDoorDL, NULL, 130, 12, 50, 15 },                 // DOORSHUTTER_GFX_GOHMA_BLOCK
    { gSpiritDoorDL, gJyaDoorMetalBarsDL, 240, 14, 50, 15 }, // DOORSHUTTER_GFX_SPIRIT_TEMPLE
    { gBossDoorDL, NULL, 130, 12, 50, 15 },                  // DOORSHUTTER_GFX_BOSS_DOOR
#endif
    { gDungeonDoorDL, gDoorMetalBarsDL, 130, 12, 20, 15 },                         // DOORSHUTTER_GFX_GENERIC
    { gFireTempleDoorFrontDL, gDoorMetalBarsDL, 130, 12, 20, 15 },                 // DOORSHUTTER_GFX_FIRE_TEMPLE_1
    { gFireTempleDoorBackDL, gDoorMetalBarsDL, 130, 12, 20, 15 },                  // DOORSHUTTER_GFX_FIRE_TEMPLE_2
    { object_ganon_objects_DL_0000C0, gDoorMetalBarsDL, 130, 12, 20, 15 },         // DOORSHUTTER_GFX_GANONS_TOWER
    { gObjectMizuObjectsDoorShutterDL_005D90, gDoorMetalBarsDL, 130, 12, 20, 15 }, // DOORSHUTTER_GFX_WATER_TEMPLE_1
    { gObjectMizuObjectsDoorShutterDL_007000, gDoorMetalBarsDL, 130, 12, 20, 15 }, // DOORSHUTTER_GFX_WATER_TEMPLE_2
    { object_haka_door_DL_002620, gDoorMetalBarsDL, 130, 12, 20, 15 },             // DOORSHUTTER_GFX_SHADOW_TEMPLE_1
    { object_haka_door_DL_003890, gDoorMetalBarsDL, 130, 12, 20, 15 },             // DOORSHUTTER_GFX_SHADOW_TEMPLE_2
    { object_ice_objects_DL_001D10, gDoorMetalBarsDL, 130, 12, 20, 15 },           // DOORSHUTTER_GFX_ICE_CAVERN
    { gGTGDoorDL, gDoorMetalBarsDL, 130, 12, 20, 15 },                 // DOORSHUTTER_GFX_GERUDO_TRAINING_GROUND
    { gGanonsCastleDoorDL, gDoorMetalBarsDL, 130, 12, 20, 15 },        // DOORSHUTTER_GFX_GANONS_CASTLE
    { object_ouke_haka_DL_0000C0, gDoorMetalBarsDL, 130, 12, 20, 15 }, // DOORSHUTTER_GFX_ROYAL_FAMILYS_TOMB
};

static s8 door_shutter_status[] = {
    DOORSHUTTER_STYLE_FROM_SCENE,    // SHUTTER
    DOORSHUTTER_STYLE_FROM_SCENE,    // SHUTTER_FRONT_CLEAR
    DOORSHUTTER_STYLE_FROM_SCENE,    // SHUTTER_FRONT_SWITCH
    DOORSHUTTER_STYLE_FROM_SCENE,    // SHUTTER_BACK_LOCKED
    DOORSHUTTER_STYLE_PHANTOM_GANON, // SHUTTER_PG_BARS
    DOORSHUTTER_STYLE_BOSS_DOOR,     // SHUTTER_BOSS
    DOORSHUTTER_STYLE_GOHMA_BLOCK,   // SHUTTER_GOHMA_BLOCK
    DOORSHUTTER_STYLE_FROM_SCENE,    // SHUTTER_FRONT_SWITCH_BACK_CLEAR
    DOORSHUTTER_STYLE_PHANTOM_GANON, // SHUTTER_8
    DOORSHUTTER_STYLE_FROM_SCENE,    // SHUTTER_9
    DOORSHUTTER_STYLE_FROM_SCENE,    // SHUTTER_A
    DOORSHUTTER_STYLE_FROM_SCENE,    // SHUTTER_KEY_LOCKED
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 400, ICHAIN_STOP),
};

typedef struct DoorShutterSceneInfo {
    s16 sceneId;
    u8 styleType;
} DoorShutterSceneInfo;

static DoorShutterSceneInfo scene_door_shutter_status[] = {
    { SCENE_DEKU_TREE, DOORSHUTTER_STYLE_DEKU_TREE },
    { SCENE_DODONGOS_CAVERN, DOORSHUTTER_STYLE_DODONGOS_CAVERN },
    { SCENE_DODONGOS_CAVERN_BOSS, DOORSHUTTER_STYLE_DODONGOS_CAVERN },
    { SCENE_JABU_JABU, DOORSHUTTER_STYLE_JABU_JABU },
    { SCENE_FOREST_TEMPLE, DOORSHUTTER_STYLE_FOREST_TEMPLE },
    { SCENE_FIRE_TEMPLE, DOORSHUTTER_STYLE_FIRE_TEMPLE },
    { SCENE_GANONS_TOWER, DOORSHUTTER_STYLE_GANONS_TOWER },
    { SCENE_GANONDORF_BOSS, DOORSHUTTER_STYLE_GANONS_TOWER },
    { SCENE_SPIRIT_TEMPLE, DOORSHUTTER_STYLE_SPIRIT_TEMPLE },
    { SCENE_SPIRIT_TEMPLE_BOSS, DOORSHUTTER_STYLE_SPIRIT_TEMPLE },
    { SCENE_WATER_TEMPLE, DOORSHUTTER_STYLE_WATER_TEMPLE },
    { SCENE_SHADOW_TEMPLE, DOORSHUTTER_STYLE_SHADOW_TEMPLE },
    { SCENE_BOTTOM_OF_THE_WELL, DOORSHUTTER_STYLE_SHADOW_TEMPLE },
    { SCENE_ICE_CAVERN, DOORSHUTTER_STYLE_ICE_CAVERN },
    { SCENE_GERUDO_TRAINING_GROUND, DOORSHUTTER_STYLE_GERUDO_TRAINING_GROUND },
    { SCENE_INSIDE_GANONS_CASTLE, DOORSHUTTER_STYLE_GANONS_CASTLE },
    { SCENE_ROYAL_FAMILYS_TOMB, DOORSHUTTER_STYLE_ROYAL_FAMILYS_TOMB },
    { -1, DOORSHUTTER_STYLE_GENERIC },
};

typedef enum DoorShutterBossDoorTexIndex {
    /* 0 */ DOORSHUTTER_BOSSDOORTEX_0,
    /* 1 */ DOORSHUTTER_BOSSDOORTEX_FIRE,
    /* 2 */ DOORSHUTTER_BOSSDOORTEX_WATER,
    /* 3 */ DOORSHUTTER_BOSSDOORTEX_SHADOW,
    /* 4 */ DOORSHUTTER_BOSSDOORTEX_GANON,
    /* 5 */ DOORSHUTTER_BOSSDOORTEX_FOREST,
    /* 6 */ DOORSHUTTER_BOSSDOORTEX_SPIRIT
} DoorShutterBossDoorTexIndex;

typedef struct DoorShutterBossDoorInfo {
    s16 dungeonSceneId;
    s16 bossSceneId;
    u8 texIndex;
} DoorShutterBossDoorInfo;

static DoorShutterBossDoorInfo scene_door_shutter_texture[] = {
    { SCENE_FIRE_TEMPLE, SCENE_FIRE_TEMPLE_BOSS, DOORSHUTTER_BOSSDOORTEX_FIRE },
    { SCENE_WATER_TEMPLE, SCENE_WATER_TEMPLE_BOSS, DOORSHUTTER_BOSSDOORTEX_WATER },
    { SCENE_SHADOW_TEMPLE, SCENE_SHADOW_TEMPLE_BOSS, DOORSHUTTER_BOSSDOORTEX_SHADOW },
    { SCENE_GANONS_TOWER, SCENE_GANONDORF_BOSS, DOORSHUTTER_BOSSDOORTEX_GANON },
    { SCENE_FOREST_TEMPLE, SCENE_FOREST_TEMPLE_BOSS, DOORSHUTTER_BOSSDOORTEX_FOREST },
    { SCENE_SPIRIT_TEMPLE, SCENE_SPIRIT_TEMPLE_BOSS, DOORSHUTTER_BOSSDOORTEX_SPIRIT },
    { -1, -1, DOORSHUTTER_BOSSDOORTEX_0 },
};

static Gfx* bdan_shape_data[] = {
    gJabuDoorSection1DL, gJabuDoorSection2DL, gJabuDoorSection7DL, gJabuDoorSection4DL,
    gJabuDoorSection5DL, gJabuDoorSection4DL, gJabuDoorSection3DL, gJabuDoorSection2DL,
};

static void* boss_shutter_texture[] = {
    gBossDoorDefaultTex,      // DOORSHUTTER_BOSSDOORTEX_0
    gBossDoorFireTex,         // DOORSHUTTER_BOSSDOORTEX_FIRE
    gBossDoorWaterTex,        // DOORSHUTTER_BOSSDOORTEX_WATER
    gBossDoorShadowTex,       // DOORSHUTTER_BOSSDOORTEX_SHADOW
    gBossDoorGanonsCastleTex, // DOORSHUTTER_BOSSDOORTEX_GANON
    gBossDoorForestTex,       // DOORSHUTTER_BOSSDOORTEX_FOREST
    gBossDoorSpiritTex,       // DOORSHUTTER_BOSSDOORTEX_SPIRIT
};

void Door_Shutter_actor_set_process(DoorShutter* this, DoorShutterActionFunc actionFunc) {
    this->actionFunc = actionFunc;
    this->actionTimer = 0;
}

/**
 * Setup the correct action depending on the door type.
 *
 * This function is called repeatedly and not just on actor init, to check the door being barred,
 * and to handle door types that make the door behave different on the front/back sides.
 *
 * @return true if the door is barred
 */
s32 type_check_set(DoorShutter* this, PlayState* play) {
    TransitionActorEntry* transitionEntry = &play->transitionActors.list[GET_TRANSITION_ACTOR_INDEX(&this->dyna.actor)];
    s8 frontRoom = transitionEntry->sides[0].room;
    s32 doorType = this->doorType;
    DoorShutterStyleInfo* styleInfo = &door_shutter_shape_data[this->styleType];

    if (doorType != SHUTTER_KEY_LOCKED) {
        if (frontRoom == transitionEntry->sides[1].room) {
            // if player is in front
            if (ABS((s16)(this->dyna.actor.shape.rot.y - this->dyna.actor.yawTowardsPlayer)) < 0x4000) {
                frontRoom = -1;
            }
        }
        if (frontRoom == this->dyna.actor.room) {
            if (doorType == SHUTTER_FRONT_SWITCH_BACK_CLEAR) { // Swap the back clear to the front clear
                doorType = SHUTTER_FRONT_CLEAR;
            } else if (doorType == SHUTTER_BOSS) {
                doorType = SHUTTER_BACK_LOCKED;
            } else {
                doorType = SHUTTER;
            }
        }
    }
    this->gfxType = (doorType == SHUTTER) ? styleInfo->gfxType1 : styleInfo->gfxType2;

    if (doorType == SHUTTER_FRONT_CLEAR) {
        if (!Actor_Environment_room_clear_Check(play, this->dyna.actor.room)) {
            Door_Shutter_actor_set_process(this, move_room_clear_wait);
            this->barsClosedAmount = 1.0f;
            return true;
        }
    } else if (doorType == SHUTTER_FRONT_SWITCH || doorType == SHUTTER_FRONT_SWITCH_BACK_CLEAR) {
        if (!Actor_Environment_sw_Check(play, DOORSHUTTER_GET_SWITCH_FLAG(&this->dyna.actor))) {
            Door_Shutter_actor_set_process(this, move_switch_wait);
            this->barsClosedAmount = 1.0f;
            return true;
        } else {
            Door_Shutter_actor_set_process(this, move_switch_close_wait);
            return false;
        }
    } else if (doorType == SHUTTER_BACK_LOCKED) {
        Door_Shutter_actor_set_process(this, move_non);
        return false;
    }
    Door_Shutter_actor_set_process(this, move_wait);
    return false;
}

void Door_Shutter_actor_ct(Actor* thisx, PlayState* play2) {
    DoorShutter* this = (DoorShutter*)thisx;
    PlayState* play = play2;
    s32 styleType;
    s32 pad;
    s32 objectSlot;
    s32 i;

    ValueSet_process(&this->dyna.actor, value_init);
    this->dyna.actor.home.pos.z = this->dyna.actor.shape.yOffset;
    MoveBG_ct(&this->dyna, 0);
    this->doorType = DOORSHUTTER_GET_TYPE(&this->dyna.actor);

    styleType = door_shutter_status[this->doorType];
    if (styleType < 0) {
        DoorShutterSceneInfo* sceneInfo;

        for (sceneInfo = &scene_door_shutter_status[0], i = 0; i < ARRAY_COUNT(scene_door_shutter_status) - 1; i++, sceneInfo++) {
            if (play->sceneId == sceneInfo->sceneId) {
                break;
            }
        }
        styleType = sceneInfo->styleType;
    } else if (styleType == DOORSHUTTER_STYLE_BOSS_DOOR) {
        DoorShutterBossDoorInfo* bossDoorInfo;

        for (bossDoorInfo = &scene_door_shutter_texture[0], i = 0; i < ARRAY_COUNT(scene_door_shutter_texture) - 1; i++, bossDoorInfo++) {
            if (play->sceneId == bossDoorInfo->dungeonSceneId || play->sceneId == bossDoorInfo->bossSceneId) {
                break;
            }
        }
        this->bossDoorTexIndex = bossDoorInfo->texIndex;
    } else { // DOORSHUTTER_STYLE_PHANTOM_GANON, DOORSHUTTER_STYLE_GOHMA_BLOCK
        this->dyna.actor.room = -1;
    }
    if (this->requiredObjectSlot = objectSlot = Object_Exchange_bank_check(&play->objectCtx, door_shutter_shape_data[styleType].objectId),
        (s8)objectSlot < 0) {
        Actor_delete(&this->dyna.actor);
        return;
    }
    Door_Shutter_actor_set_process(this, move_dma_wait);

    this->styleType = styleType;
    if (this->doorType == SHUTTER_KEY_LOCKED || this->doorType == SHUTTER_BOSS) {
        if (!Actor_Environment_sw_Check(play, DOORSHUTTER_GET_SWITCH_FLAG(&this->dyna.actor))) {
            this->unlockTimer = 10;
        }
        Actor_world_to_eye(&this->dyna.actor, 60.0f);
    } else if (styleType == DOORSHUTTER_STYLE_JABU_JABU) {
        Actor_set_scale(&this->dyna.actor, 0.1f);
        this->jabuDoorClosedAmount = 100;
        this->dyna.actor.cullingVolumeScale = 200.0f;
        Actor_world_to_eye(&this->dyna.actor, 0.0f);
    } else {
        Actor_world_to_eye(&this->dyna.actor, 60.0f);
    }
}

void Door_Shutter_actor_dt(Actor* thisx, PlayState* play) {
    DoorShutter* this = (DoorShutter*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    if (this->dyna.actor.room >= 0) {
        s32 transitionActorId = GET_TRANSITION_ACTOR_INDEX(&this->dyna.actor);

        play->transitionActors.list[transitionActorId].id *= -1;
    }
}

static void move_dma_wait(DoorShutter* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->dyna.actor.objectSlot = this->requiredObjectSlot;
        if (this->doorType == SHUTTER_PG_BARS || this->doorType == SHUTTER_GOHMA_BLOCK) {
            // Init dynapoly for shutters of the type that uses it
            CollisionHeader* colHeader = NULL;

            Actor_set_segment(play, &this->dyna.actor);
            this->gfxType = door_shutter_shape_data[this->styleType].gfxType1;
            DynaPolyUty_bgdi_SG2KSG((this->doorType == SHUTTER_GOHMA_BLOCK) ? &gGohmaDoorCol : &gPhantomGanonBarsCol,
                                       &colHeader);
            this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            if (this->doorType == SHUTTER_GOHMA_BLOCK) {
                this->dyna.actor.velocity.y = 0.0f;
                this->dyna.actor.gravity = -2.0f;
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_SLIDE_DOOR_CLOSE);
                Door_Shutter_actor_set_process(this, move_goma_door_down);
            } else {
                Door_Shutter_actor_set_process(this, move_fhg_saku);
                this->isActive = 7;
            }
        } else {
            type_check_set(this, play);
        }
    }
}

/**
 * Compute the distance forwards to the player
 * (perpendicular to the door's left-right, and negative if the player is behind),
 * or returns the max float value if the player is too far left/right (`maxDistSides`) or above/below (`maxDistY`).
 */
static f32 player_distance_check(PlayState* play, DoorShutter* this, f32 offsetY, f32 maxDistSides, f32 maxDistY) {
    s32 pad;
    Vec3f playerPos;
    Vec3f relPlayerPos;
    Player* player = GET_PLAYER(play);

    playerPos.x = player->actor.world.pos.x;
    playerPos.y = player->actor.world.pos.y + offsetY;
    playerPos.z = player->actor.world.pos.z;

    Actor_search_position_project_distanceXZ(&this->dyna.actor, &relPlayerPos, &playerPos);

    if (fabsf(relPlayerPos.x) > maxDistSides || fabsf(relPlayerPos.y) > maxDistY) {
        return MAXFLOAT;
    } else {
        return relPlayerPos.z;
    }
}

/**
 * Returns 0 if the player is not near or is not facing the door.
 * Returns 1 if the player is near and is facing the front of the door.
 * Returns -1 if the player is near and is facing the back of the door.
 */
static s32 player_door_open_pos_check(DoorShutter* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!player_demo_check(play)) {
        DoorShutterGfxInfo* gfxInfo = &door_shutter_shape_info[this->gfxType];
        f32 playerDistForwards =
            player_distance_check(play, this, (this->gfxType != DOORSHUTTER_GFX_JABU_JABU) ? 0.0f : 80.0f,
                                          gfxInfo->rangeSides, gfxInfo->rangeY);

        if (fabsf(playerDistForwards) < 50.0f) {
            s16 yawDiff = player->actor.shape.rot.y - this->dyna.actor.shape.rot.y;

            // If the player is in front of the door (positive forwards distance)
            if (playerDistForwards > 0.0f) {
                // It is expected that, when facing the door, the player faces the opposite way compared to the door.
                // That means the corresponding yawDiff is around 0x8000 (half a turn),
                // so the yawDiff is centered on 0 here.
                yawDiff = 0x8000 - yawDiff;
            }

            if (ABS(yawDiff) < 0x3000) {
                return (playerDistForwards >= 0.0f) ? 1.0f : -1.0f;
            }
        }
    }
    return 0.0f;
}

/**
 * The door is barred and waiting for the room to be cleared.
 *
 * Either the permanent room clear flag or temporary room clear flag may be set to unbar the door.
 * When that happens, the permanent room clear flag is set.
 */
static void move_room_clear_wait(DoorShutter* this, PlayState* play) {
    if (Actor_Environment_room_clear_Check(play, this->dyna.actor.room) || Actor_Environment_no_enemy_Check(play, this->dyna.actor.room)) {
        Actor_Environment_room_clear_On(play, this->dyna.actor.room);
        Door_Shutter_actor_set_process(this, move_switch_open);
        makeActorAttentionDemo(play, &this->dyna.actor);
        makeActorAttentionDemo(play, &GET_PLAYER(play)->actor);
        this->actionTimer = -100;
    } else if (player_door_open_pos_check(this, play) != 0) {
        Player* player = GET_PLAYER(play);

        player->naviTextId = -0x202;
    }
}

/**
 * The door cannot be interacted with.
 */
void move_non(DoorShutter* this, PlayState* play) {
}

/**
 * The player may open the door.
 * If the door is a locked door, having a dungeon key is required and one will be consumed.
 * If the door is a boss door, having the boss key is required.
 *
 * Opening either a locked door or a boss door sets the switch flag taken from actor params.
 */
static void move_wait(DoorShutter* this, PlayState* play) {
    if (this->isActive) {
        Door_Shutter_actor_set_process(this, move_open);
        this->dyna.actor.velocity.y = 0.0f;
        if (this->unlockTimer != 0) {
            Actor_Environment_sw_On(play, DOORSHUTTER_GET_SWITCH_FLAG(&this->dyna.actor));
            if (this->doorType != SHUTTER_BOSS) {
                z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex] -= 1;
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_CHAIN_KEY_UNLOCK);
            } else {
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_CHAIN_KEY_UNLOCK_B);
            }
        }
    } else {
        s32 doorDirection = player_door_open_pos_check(this, play);

        if (doorDirection != 0) {
            Player* player = GET_PLAYER(play);

            if (this->unlockTimer != 0) {
                if (this->doorType == SHUTTER_BOSS) {
                    if (!CHECK_DUNGEON_ITEM(DUNGEON_BOSS_KEY, z_common_data.mapIndex)) {
                        player->naviTextId = -0x204;
                        return;
                    }
                } else if (z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex] <= 0) {
                    player->naviTextId = -0x203;
                    return;
                }
                player->doorTimer = 10;
            }
            player->doorType = PLAYER_DOORTYPE_SLIDING;
            player->doorDirection = doorDirection;
            player->doorActor = &this->dyna.actor;
        }
    }
}

void open_camera_set(DoorShutter* this, PlayState* play) {
    // The actor category is never changed from ACTORCAT_DOOR, this condition always passes
    if (this->dyna.actor.category == ACTORCAT_DOOR) {
        Player* player = GET_PLAYER(play);
        s32 savedGfxType = this->gfxType;
        s32 doorCamTimer2 = 15;

        if (type_check_set(this, play)) {
            doorCamTimer2 = 32;
        }

        // Restore action, gfxType and barsClosedAmount after the type_check_set call
        Door_Shutter_actor_set_process(this, move_open);
        this->gfxType = savedGfxType;
        this->barsClosedAmount = 0.0f;

        setDoorCameraInfo(play->cameraPtrs[CAM_ID_MAIN], &this->dyna.actor, player->cv.slidingDoorBgCamIndex, 0.0f,
                             12, doorCamTimer2, 10);
    }
}

/**
 * Open the door by progressively sliding it up 200 units.
 * Also handles opening Jabu doors, which don't slide like all the other types.
 *
 * On the first call of opening the door (determined based on y velocity being 0 for non-jabu doors),
 * also changes the camera appropriately.
 */
s32 open_check_set(DoorShutter* this, PlayState* play) {
    if (this->gfxType != DOORSHUTTER_GFX_JABU_JABU) {
        if (this->dyna.actor.velocity.y == 0.0f) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_SLIDE_DOOR_OPEN);
            open_camera_set(this, play);
        }
        chase_f(&this->dyna.actor.velocity.y, 15.0f, 3.0f);
        if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y + 200.0f,
                         this->dyna.actor.velocity.y)) {
            return true;
        }
    } else {
        if (this->jabuDoorClosedAmount == 100) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_BUYODOOR_OPEN);
            open_camera_set(this, play);
        }
        if (chase_s(&this->jabuDoorClosedAmount, 0, 10)) {
            return true;
        }
    }
    return false;
}

/**
 * Progressively bar or unbar the door.
 *
 * @param barsClosedAmountTarget 1.0f to bar the door, or 0.0f to unbar.
 *
 * @return true if the door is fully barred or fully unbarred.
 */
s32 yari_check_set(DoorShutter* this, PlayState* play, f32 barsClosedAmountTarget) {
    if (this->barsClosedAmount == (1.0f - barsClosedAmountTarget)) {
        if (this->gfxType != DOORSHUTTER_GFX_JABU_JABU) {
            if (barsClosedAmountTarget == 1.0f) {
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_CLOSE);
            } else {
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_METALDOOR_OPEN);
            }
        } else {
            if (barsClosedAmountTarget == 1.0f) {
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSHUTTER_CLOSE);
            } else {
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSHUTTER_OPEN);
            }
        }
    }

    if (chase_f(&this->barsClosedAmount, barsClosedAmountTarget, 0.2f)) {
        return true;
    } else {
        return false;
    }
}

/**
 * Bar the door and keep it barred until the switch flag taken from the params is set.
 */
void move_switch_wait(DoorShutter* this, PlayState* play) {
    if (yari_check_set(this, play, 1.0f)) {
        if (Actor_Environment_sw_Check(play, DOORSHUTTER_GET_SWITCH_FLAG(&this->dyna.actor))) {
            Door_Shutter_actor_set_process(this, move_switch_open);
            makeActorAttentionDemo(play, &this->dyna.actor);
            this->actionTimer = -100;
        } else {
            if (player_door_open_pos_check(this, play) != 0) {
                Player* player = GET_PLAYER(play);

                player->naviTextId = (play->sceneId == SCENE_JABU_JABU) ? -0x20B : -0x202;
            }
        }
    }
}

/**
 * The door has been unbarred.
 *
 * Effectively, this action is only used for switch-flag-barred door types.
 * Its purpose is to check the switch flag is still set after the door has been unbarred.
 */
void move_switch_close_wait(DoorShutter* this, PlayState* play) {
    if (!this->isActive && !Actor_Environment_sw_Check(play, DOORSHUTTER_GET_SWITCH_FLAG(&this->dyna.actor))) {
        Door_Shutter_actor_set_process(this, move_switch_wait);
    } else {
        move_wait(this, play);
    }
}

/**
 * The door is being opened.
 *
 * The door closes once the player is far enough from the door.
 */
static void move_open(DoorShutter* this, PlayState* play) {
    if (DECR(this->unlockTimer) == 0 && play->roomCtx.status == 0 && open_check_set(this, play)) {
        if (this->dyna.actor.xzDistToPlayer > ((this->doorType == SHUTTER_BOSS) ? 20.0f : 50.0f)) {
            if (type_check_set(this, play)) {
                // Close faster if the door is barred behind the player
                this->dyna.actor.velocity.y = 30.0f;
            }
            if (this->gfxType != DOORSHUTTER_GFX_JABU_JABU) {
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_SLIDE_DOOR_CLOSE);
                Door_Shutter_actor_set_process(this, move_close);
            } else {
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_BUYODOOR_CLOSE);
                if ((this->doorType == SHUTTER_FRONT_SWITCH || this->doorType == SHUTTER_FRONT_SWITCH_BACK_CLEAR) &&
                    !Actor_Environment_sw_Check(play, DOORSHUTTER_GET_SWITCH_FLAG(&this->dyna.actor))) {
                    Actor_SE_set(&this->dyna.actor, NA_SE_EV_BUYOSHUTTER_CLOSE);
                }
                Door_Shutter_actor_set_process(this, move_bdan_close);
            }
        }
    }
}

/**
 * The door is being unbarred.
 */
void move_switch_open(DoorShutter* this, PlayState* play) {
    if (this->actionTimer != 0) {
        if (this->actionTimer < 0) {
            if (play->state.frames % 2 != 0) {
                this->actionTimer++;
            }
            if (this->dyna.actor.category == getAttentionDemoPart() || this->actionTimer == 0) {
                this->actionTimer = 5;
            }
        } else {
            this->actionTimer--;
        }
    } else if (yari_check_set(this, play, 0.0f)) {
        if (!(this->doorType == SHUTTER || this->doorType == SHUTTER_FRONT_CLEAR)) {
            Door_Shutter_actor_set_process(this, move_switch_close_wait);
        } else {
            Door_Shutter_actor_set_process(this, move_wait);
        }
        Na_StopMiddleBossBgm();
    }
}

/**
 * The door has been closed.
 *
 * The previous room is unloaded.
 */
void room_change_set(DoorShutter* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s8 room = this->dyna.actor.room;

    if (this->dyna.actor.room >= 0) {
        Vec3f relPlayerPos;

        Actor_search_position_project_distanceXZ(&this->dyna.actor, &relPlayerPos, &player->actor.world.pos);
        this->dyna.actor.room = play->transitionActors.list[GET_TRANSITION_ACTOR_INDEX(&this->dyna.actor)]
                                    .sides[(relPlayerPos.z < 0.0f) ? 0 : 1]
                                    .room;
        if (room != this->dyna.actor.room) {
            Room tempRoom = play->roomCtx.curRoom;

            play->roomCtx.curRoom = play->roomCtx.prevRoom;
            play->roomCtx.prevRoom = tempRoom;
            play->roomCtx.activeBufPage ^= 1;
        }
        Room_Info_old_room_clear(play, &play->roomCtx);
        Game_play_restart_set(play, RESPAWN_MODE_DOWN,
                               PLAYER_PARAMS(PLAYER_START_MODE_MOVE_FORWARD_SLOW, PLAYER_START_BG_CAM_DEFAULT));
    }
    this->isActive = false;
    this->dyna.actor.velocity.y = 0.0f;
    if (type_check_set(this, play) && !(player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
        // The door is barred behind the player
        Door_Shutter_actor_set_process(this, move_demo_wait);
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_2);
    }
}

/**
 * The door is being closed by sliding downwards to its initial position.
 */
static void move_close(DoorShutter* this, PlayState* play) {
    s32 quakeIndex;

    if (this->dyna.actor.velocity.y < 20.0f) {
        chase_f(&this->dyna.actor.velocity.y, 20.0f, 8.0f);
    }
    if (chase_f(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, this->dyna.actor.velocity.y)) {
        if (this->dyna.actor.velocity.y > 20.0f) {
            this->dyna.actor.floorHeight = this->dyna.actor.home.pos.y;
            _dust_ground_set(play, &this->dyna.actor, &this->dyna.actor.world.pos, 45.0f, 10, 8.0f, 500, 10,
                                     false);
        }
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_STONE_BOUND);
        quakeIndex = startQuake(Gama_play_get_camera(play, CAM_ID_MAIN), 3);
        setSpeedQuake(quakeIndex, -32536);
        setScaleQuake(quakeIndex, 2, 0, 0, 0);
        setTimerQuake(quakeIndex, 10);
        z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 180, 20, 100);
        room_change_set(this, play);
    }
}

/**
 * The Jabu door is being closed.
 */
void move_bdan_close(DoorShutter* this, PlayState* play) {
    if (chase_s(&this->jabuDoorClosedAmount, 100, 10)) {
        room_change_set(this, play);
    }
}

static void move_demo_wait(DoorShutter* this, PlayState* play) {
    if (this->actionTimer++ > 30) {
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        type_check_set(this, play);
    }
}

void move_goma_door_down(DoorShutter* this, PlayState* play) {
    Actor_position_moveF(&this->dyna.actor);
    Actor_BGcheck2(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Door_Shutter_actor_set_process(this, move_goma_door_wait);
        if (!GET_EVENTCHKINF(EVENTCHKINF_BEGAN_GOHMA_BATTLE)) {
            BossGoma* parent = (BossGoma*)this->dyna.actor.parent;

            this->isActive = 10;
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_STONE_BOUND);
            door_jisin(play, 2, 10, parent->subCamId);
            _dust_ground_set(play, &this->dyna.actor, &this->dyna.actor.world.pos, 70.0f, 20, 8.0f, 500, 10,
                                     true);
        }
    }
}

void move_goma_door_wait(DoorShutter* this, PlayState* play) {
    f32 bounceFactor;

    // Bounce a bit (unnoticeable in-game)
    if (this->isActive != 0) {
        this->isActive--;
        bounceFactor = sinf(this->isActive * 250.0f / 100.0f);
        this->dyna.actor.shape.yOffset = this->isActive * 3.0f / 10.0f * bounceFactor;
    }
}

void move_fhg_saku(DoorShutter* this, PlayState* play) {
    f32 targetOffsetY;

    PRINTF("FHG SAKU START !!\n");
    DECR(this->isActive);
    targetOffsetY = (this->isActive % 2 != 0) ? -3.0f : 0.0f;
    add_calc(&this->dyna.actor.world.pos.y, -34.0f + targetOffsetY, 1.0f, 20.0f, 0.0f);
    PRINTF("FHG SAKU END !!\n");
}

void Door_Shutter_actor_move(Actor* thisx, PlayState* play) {
    DoorShutter* this = (DoorShutter*)thisx;
    Player* player = GET_PLAYER(play);

    if (!(player->stateFlags1 & (PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_10 | PLAYER_STATE1_28)) ||
        (this->actionFunc == move_dma_wait)) {
        this->actionFunc(this, play);
    }
}

Gfx* bdan_shutter_draw(PlayState* play, DoorShutter* this, Gfx* gfx) {
    MtxF mtx;
    f32 angle = 0.0f;
    f32 yScale = this->jabuDoorClosedAmount * 0.01f;
    s32 i;

    Matrix_get(&mtx);

    for (i = 0; i < ARRAY_COUNT(bdan_shape_data); i++) {
        Matrix_rotateZ(angle, MTXMODE_APPLY);

        if (i % 2 == 0) {
            Matrix_translate(0.0f, 800.0f, 0.0f, MTXMODE_APPLY);
        } else if (i == 1 || i == 7) {
            Matrix_translate(0.0f, 848.52f, 0.0f, MTXMODE_APPLY);
        } else {
            Matrix_translate(0.0f, 989.94f, 0.0f, MTXMODE_APPLY);
        }

        if (this->jabuDoorClosedAmount != 100) {
            Matrix_scale(1.0f, yScale, 1.0f, MTXMODE_APPLY);
        }

        MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx, "../z_door_shutter.c", 1991);
        gSPDisplayList(gfx++, bdan_shape_data[i]);

        angle -= 2 * M_PI / ARRAY_COUNT(bdan_shape_data);

        Matrix_put(&mtx);
    }
    return gfx;
}

s32 door_draw_check(DoorShutter* this, PlayState* play) {
    s32 relYawTowardsPlayer;
    s32 relYawTowardsViewEye;

    if (player_demo_check(play)) {
        return true;
    }

    // Return true if the view eye and the player are on the same side of the door

    relYawTowardsViewEye =
        (s16)(Actor_search_position_angleY(&this->dyna.actor, &play->view.eye) - this->dyna.actor.shape.rot.y);
    relYawTowardsPlayer = (s16)(this->dyna.actor.yawTowardsPlayer - this->dyna.actor.shape.rot.y);
    relYawTowardsViewEye = ABS(relYawTowardsViewEye);
    relYawTowardsPlayer = ABS(relYawTowardsPlayer);

    if ((relYawTowardsPlayer < 0x4000 && relYawTowardsViewEye > 0x4000) ||
        (relYawTowardsPlayer > 0x4000 && relYawTowardsViewEye < 0x4000)) {
        return false;
    } else {
        return true;
    }
}

void Door_Shutter_actor_draw(Actor* thisx, PlayState* play) {
    DoorShutter* this = (DoorShutter*)thisx;

    if (1) {}

    //! @bug This actor is not fully initialized until the required object dependency is loaded.
    //! In most cases, the check for objectSlot to equal requiredObjectSlot prevents the actor
    //! from drawing until initialization is complete. However if the required object is the same as the
    //! object dependency listed in the actor profile (gameplay_keep in this case), the check will pass even though
    //! initialization has not completed. When this happens, it will try to draw the display list of the
    //! first entry in `door_shutter_shape_info`, which will likely crash the game.
    //! This only matters in very specific scenarios, when the door is unculled on the first possible frame
    //! after spawning. It will try to draw without having run update yet.
    //!
    //! The best way to fix this issue (and what was done in Majora's Mask) is to null out the draw function in
    //! the profile for the actor, and only set draw after initialization is complete.

    if (this->dyna.actor.objectSlot == this->requiredObjectSlot &&
        (this->styleType == DOORSHUTTER_STYLE_PHANTOM_GANON || door_draw_check(this, play))) {
        s32 pad[2];
        DoorShutterGfxInfo* gfxInfo = &door_shutter_shape_info[this->gfxType];

        OPEN_DISPS(play->state.gfxCtx, "../z_door_shutter.c", 2048);

        _texture_z_light_fog_prim(play->state.gfxCtx);

        if (this->gfxType == DOORSHUTTER_GFX_JABU_JABU) {
            POLY_OPA_DISP = bdan_shutter_draw(play, this, POLY_OPA_DISP);
            if (this->barsClosedAmount != 0.0f) {
                f32 scale = (this->jabuDoorClosedAmount * 0.01f) * this->barsClosedAmount;

                _texture_z_light_fog_prim(play->state.gfxCtx);
                gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255.0f * scale); // no purpose?
                Matrix_translate(0, 0, gfxInfo->barsOffsetZ, MTXMODE_APPLY);
                Matrix_scale(scale, scale, scale, MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_door_shutter.c", 2069);
                gSPDisplayList(POLY_OPA_DISP++, gfxInfo->barsDL);
            }
        } else {
            if (gfxInfo->barsDL != NULL) {
                TransitionActorEntry* transitionEntry =
                    &play->transitionActors.list[GET_TRANSITION_ACTOR_INDEX(&this->dyna.actor)];

                if (play->roomCtx.prevRoom.num >= 0 ||
                    transitionEntry->sides[0].room == transitionEntry->sides[1].room) {
                    s32 yaw = search_position_angleY(&play->view.eye, &this->dyna.actor.world.pos);

                    if (ABS((s16)(this->dyna.actor.shape.rot.y - yaw)) < 0x4000) {
                        Matrix_rotateY(M_PI, MTXMODE_APPLY);
                    }
                } else if (this->dyna.actor.room == transitionEntry->sides[0].room) {
                    Matrix_rotateY(M_PI, MTXMODE_APPLY);
                }
            } else if (this->doorType == SHUTTER_BOSS) {
                gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(boss_shutter_texture[this->bossDoorTexIndex]));
            }
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_door_shutter.c", 2109);
            gSPDisplayList(POLY_OPA_DISP++, gfxInfo->doorDL);
            if (this->barsClosedAmount != 0.0f && gfxInfo->barsDL != NULL) {
                Matrix_translate(0, gfxInfo->barsOpenOffsetY * (1.0f - this->barsClosedAmount), gfxInfo->barsOffsetZ,
                                 MTXMODE_APPLY);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_door_shutter.c", 2119);
                gSPDisplayList(POLY_OPA_DISP++, gfxInfo->barsDL);
            }
        }

        if (this->unlockTimer != 0) {
            Matrix_scale(0.01f, 0.01f, 0.025f, MTXMODE_APPLY);
            key_draw(
                play, this->unlockTimer,
                (this->doorType == SHUTTER_BOSS)
                    ? DOORLOCK_BOSS
                    : ((this->gfxType == DOORSHUTTER_GFX_SPIRIT_TEMPLE) ? DOORLOCK_NORMAL_SPIRIT : DOORLOCK_NORMAL));
        }

        CLOSE_DISPS(play->state.gfxCtx, "../z_door_shutter.c", 2135);
    }
}

void door_jisin(PlayState* play, s16 quakeY, s16 quakeDuration, s16 camId) {
    s16 quakeIndex = startQuake(Gama_play_get_camera(play, camId), QUAKE_TYPE_3);

    z_vibctl2_vib_force_set(0.0f, 180, 20, 100);
    setSpeedQuake(quakeIndex, 20000);
    setScaleQuake(quakeIndex, quakeY, 0, 0, 0);
    setTimerQuake(quakeIndex, quakeDuration);
}
