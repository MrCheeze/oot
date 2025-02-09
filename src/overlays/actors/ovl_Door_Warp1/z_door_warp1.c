#include "z_door_warp1.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "one_point_cutscene.h"
#include "seqcmd.h"
#include "sequence.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64light.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_warp1/object_warp1.h"

#define FLAGS 0

void Door_Warp1_actor_ct(Actor* thisx, PlayState* play);
void Door_Warp1_actor_dt(Actor* thisx, PlayState* play);
void Door_Warp1_actor_move(Actor* thisx, PlayState* play);
void Door_Warp1_actor_draw(Actor* thisx, PlayState* play);

void move_barth(DoorWarp1* this, PlayState* play);
void move_warp6(DoorWarp1* this, PlayState* play);
void move_warp7(DoorWarp1* this, PlayState* play);
void move_Nonproc(DoorWarp1* this, PlayState* play);
void crystal_move_barth(DoorWarp1* this, PlayState* play);
void crystal_in_move_timer(DoorWarp1* this, PlayState* play);
void crystalzd_in_move(DoorWarp1* this, PlayState* play);
void crystal_in_move_barth(DoorWarp1* this, PlayState* play);
void crystal_in_non_proc(DoorWarp1* this, PlayState* play);
void move_Nonproc2(DoorWarp1* this, PlayState* play);
void kenjyamove_wait(DoorWarp1* this, PlayState* play);
static void move_wait(DoorWarp1* this, PlayState* play);
void move_wait_ruto(DoorWarp1* this, PlayState* play);
void move_warp(DoorWarp1* this, PlayState* play);
void move_warp_ruto(DoorWarp1* this, PlayState* play);
void move_warp_ruto2(DoorWarp1* this, PlayState* play);
void move_warp_ruto3(DoorWarp1* this, PlayState* play);
void crystal_move_wait(DoorWarp1* this, PlayState* play);
void crystal_link_wait(DoorWarp1* this, PlayState* play);
void crystal_move_warp(DoorWarp1* this, PlayState* play);
void move_warp6_end(DoorWarp1* this, PlayState* play);
static void move_dma_wait(DoorWarp1* this, PlayState* play);
void crystal_in_link_down(DoorWarp1* this, PlayState* play);

ActorProfile Door_Warp1_Profile = {
    /**/ ACTOR_DOOR_WARP1,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_WARP1,
    /**/ sizeof(DoorWarp1),
    /**/ Door_Warp1_actor_ct,
    /**/ Door_Warp1_actor_dt,
    /**/ Door_Warp1_actor_move,
    /**/ Door_Warp1_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 4000, ICHAIN_STOP),
};

static s16 E_PREG70;

void Door_Warp1_actor_set_process(DoorWarp1* this, DoorWarp1ActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Door_Warp1_actor_ct(Actor* thisx, PlayState* play) {
    DoorWarp1* this = (DoorWarp1*)thisx;
    PlayState* play2 = play;

    this->unk_1B8 = 0;
    this->unk_1B4 = 0.0f;
    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, NULL, 0.0f);

    if (this->actor.params != WARP_SAGES && this->actor.params != WARP_BLUE_CRYSTAL &&
        this->actor.params != WARP_YELLOW && this->actor.params != WARP_DESTINATION) {
        Light_point_ct(&this->upperLightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 0, 0, 0, 0);
        this->upperLight = Global_light_list_new(play2, &play2->lightCtx, &this->upperLightInfo);

        Light_point_ct(&this->lowerLightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 0, 0, 0, 0);
        this->lowerLight = Global_light_list_new(play2, &play2->lightCtx, &this->lowerLightInfo);
    }
    PRINTF("\nBOSSWARP arg_data=[%d]", this->actor.params);

    move_dma_wait(this, play2);
}

void Door_Warp1_actor_dt(Actor* thisx, PlayState* play) {
    u8 i;
    DoorWarp1* this = (DoorWarp1*)thisx;

    Global_light_list_delete(play, &play->lightCtx, this->upperLight);
    Global_light_list_delete(play, &play->lightCtx, this->lowerLight);

    for (i = 0; i < 3; i++) {
        play->envCtx.adjAmbientColor[i] = play->envCtx.adjFogColor[i] = play->envCtx.adjLight1Color[i] = 0;
    }
    //! @bug Skeleton_Info_dt is not called for crystal variants
}

void move_dma_wait_boss(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->scale = 0;
    this->unk_1AE = -140;
    this->unk_1B0 = -80;
    E_PREG70 = 100;
    this->unk_1BC = 1.0f;
    this->lightRayAlpha = 0.0f;
    this->warpAlpha = 0.0f;
    this->crystalAlpha = 0.0f;

    switch (this->actor.params) {
        case WARP_YELLOW:
        case WARP_ORANGE:
        case WARP_GREEN:
        case WARP_RED:
            this->unk_194 = 0.23f;
            this->unk_198 = 0.6f;
            break;
        case WARP_DESTINATION:
            this->unk_194 = 0.0f;
            this->unk_198 = 0.0f;
            break;
        case WARP_UNK_7:
            this->scale = 100;
            this->unk_1AE = 120;
            this->unk_1B0 = 230;
            this->unk_194 = 0.3f;
            this->unk_198 = 0.3f;
            break;
        case WARP_BLUE_RUTO:
        default:
            this->unk_194 = 0.3f;
            this->unk_198 = 0.3f;
            break;
    }

    this->unk_19C = 0.0f;
    this->actor.shape.yOffset = 1.0f;
    this->warpTimer = 0;

    switch (this->actor.params) {
        case WARP_PURPLE_CRYSTAL:
        case WARP_BLUE_RUTO:
        case WARP_UNK_7:
        default:
            Light_point_ct(&this->upperLightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                      this->actor.world.pos.z, 200, 255, 255, 255);
            Light_point_ct(&this->lowerLightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                      this->actor.world.pos.z, 200, 255, 255, 255);
            break;
        case WARP_CLEAR_FLAG:
        case WARP_SAGES:
        case WARP_YELLOW:
        case WARP_DESTINATION:
        case WARP_ORANGE:
        case WARP_GREEN:
        case WARP_RED:
            break;
    }

    switch (this->actor.params) {
        case WARP_CLEAR_FLAG:
            Door_Warp1_actor_set_process(this, move_Nonproc);
            break;
        case WARP_DESTINATION:
            if ((!(z_common_data.save.entranceIndex == ENTR_SACRED_FOREST_MEADOW_3 ||
                   z_common_data.save.entranceIndex == ENTR_DEATH_MOUNTAIN_CRATER_5 ||
                   z_common_data.save.entranceIndex == ENTR_LAKE_HYLIA_9 ||
                   z_common_data.save.entranceIndex == ENTR_DESERT_COLOSSUS_8 ||
                   z_common_data.save.entranceIndex == ENTR_GRAVEYARD_8) &&
                 !IS_CUTSCENE_LAYER) ||
                PARAMS_GET_NOSHIFT(GET_PLAYER(play)->actor.params, 8, 4) != 0x200) {
                Actor_delete(&this->actor);
            }
            if (Actor_search_actor_distanceXZ(&player->actor, &this->actor) > 100.0f) {
                Actor_delete(&this->actor);
            }
            Door_Warp1_actor_set_process(this, move_warp6);
            break;
        case WARP_UNK_7:
            Door_Warp1_actor_set_process(this, move_warp7);
            break;
        default:
            Door_Warp1_actor_set_process(this, move_barth);
            break;
    }
}

void move_dma_wait_crystal(DoorWarp1* this, PlayState* play) {
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gWarpCrystalSkel, &gWarpCrystalAnim, NULL, NULL, 0);
    Skeleton_Info2_init2(&this->skelAnime, &gWarpCrystalAnim, 1.0f, 1.0f, 1.0f, ANIMMODE_ONCE, 40.0f, ANIMTAPER_ACCEL);

    this->scale = 0;
    this->unk_1AE = -140;
    this->unk_1B0 = -80;
    E_PREG70 = 160;
    this->actor.shape.yOffset = -400.0f;
    this->warpTimer = 0;
    this->unk_1BC = 1.0f;
    this->unk_194 = 0.3f;
    this->unk_198 = 0.3f;
    this->lightRayAlpha = 0.0f;
    this->warpAlpha = 0.0f;
    this->crystalAlpha = 0.0f;
    this->unk_19C = 0.0f;

    Light_point_ct(&this->upperLightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 200, 255, 255, 255);
    Light_point_ct(&this->lowerLightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                              this->actor.world.pos.z, 200, 255, 255, 255);

    Door_Warp1_actor_set_process(this, crystal_move_barth);
}

void move_dma_wait_crystal_in(DoorWarp1* this, PlayState* play) {
    s16 i;

    Skeleton_Info2_M_ct(play, &this->skelAnime, &gWarpCrystalSkel, &gWarpCrystalAnim, NULL, NULL, 0);
    Skeleton_Info2_init2(&this->skelAnime, &gWarpCrystalAnim, 0, Si2_anime_end_frame(&gWarpCrystalAnim),
                         Si2_anime_end_frame(&gWarpCrystalAnim), ANIMMODE_ONCE, 0.0f, ANIMTAPER_ACCEL);

    this->skelAnime.curFrame = Si2_anime_end_frame(&gWarpCrystalAnim);
    this->scale = 10;
    this->unk_1AE = 120;
    this->unk_1B0 = 230;
    this->warpTimer = 0;
    this->unk_194 = 0.3f;
    this->unk_198 = 0.3f;
    this->lightRayAlpha = 0.0f;
    this->warpAlpha = 0.0f;
    this->crystalAlpha = 0.0f;
    this->unk_19C = 0.0f;
    this->unk_1BC = 1.0f;
    this->actor.shape.yOffset = 800.0f;

    for (i = 0; i < 3; i++) {
        play->envCtx.adjAmbientColor[i] = play->envCtx.adjFogColor[i] = play->envCtx.adjLight1Color[i] = -255;
    }

    play->envCtx.adjFogNear = -500;
    this->warpTimer = 30;
    this->unk_1B8 = 4000;
    Door_Warp1_actor_set_process(this, crystal_in_move_timer);
}

void move_dma_wait_crystal_zd(DoorWarp1* this, PlayState* play) {
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gWarpCrystalSkel, &gWarpCrystalAnim, NULL, NULL, 0);
    Skeleton_Info2_init2(&this->skelAnime, &gWarpCrystalAnim, 0, Si2_anime_end_frame(&gWarpCrystalAnim),
                         Si2_anime_end_frame(&gWarpCrystalAnim), ANIMMODE_ONCE, 0.0f, ANIMTAPER_ACCEL);

    this->skelAnime.curFrame = Si2_anime_end_frame(&gWarpCrystalAnim);
    this->unk_1AE = 120;
    this->unk_1B0 = 230;
    this->warpTimer = 200;
    this->unk_1B8 = 4000;
    this->actor.scale.x = this->actor.scale.y = this->actor.scale.z = 1.0f;
    this->unk_194 = 0.3f;
    this->unk_198 = 0.3f;
    this->lightRayAlpha = 0.0f;
    this->warpAlpha = 0.0f;
    this->crystalAlpha = 0.0f;
    this->unk_19C = 0.0f;
    this->unk_1BC = 1.f;
    this->actor.shape.yOffset = 800.0f;

    if (z_common_data.save.entranceIndex != ENTR_TEMPLE_OF_TIME_0) {
        this->actor.scale.x = 0.0499f;
        this->actor.scale.y = 0.077f;
        this->actor.scale.z = 0.09f;
        this->crystalAlpha = 255.0f;
    } else {
        Actor_SE_set(&this->actor, NA_SE_EV_SHUT_BY_CRYSTAL);
    }
    Door_Warp1_actor_set_process(this, crystalzd_in_move);
}

void crystal_in_link_move(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->actor.velocity.y = 0.0f;
    player->actor.world.pos.x = this->actor.world.pos.x;
    player->actor.world.pos.y = this->actor.world.pos.y + 55.0f;
    player->actor.world.pos.z = this->actor.world.pos.z;
}

void crystal_in_move_timer(DoorWarp1* this, PlayState* play) {
    if (this->warpTimer != 0) {
        this->warpTimer--;
    } else {
        Door_Warp1_actor_set_process(this, crystal_in_move_barth);
    }
    crystal_in_link_move(this, play);
}

void crystal_in_move_barth(DoorWarp1* this, PlayState* play) {
    s32 temp_f4;
    f32 darkness;
    s16 i;

    add_calc(&this->crystalAlpha, 255.0f, 0.2f, 5.0f, 0.1f);

    darkness = (f32)(40 - this->warpTimer) / 40.0f;
    darkness = CLAMP_MIN(darkness, 0);

    for (i = 0; i < 3; i++) {
        play->envCtx.adjAmbientColor[i] = play->envCtx.adjFogColor[i] = play->envCtx.adjLight1Color[i] =
            -255.0f * darkness;
    }
    play->envCtx.adjFogNear = -500.0f * darkness;

    this->warpTimer++;
    if (darkness <= 0) {
        Door_Warp1_actor_set_process(this, crystal_in_non_proc);
    }
    this->actor.shape.rot.y += 0x320;
    crystal_in_link_move(this, play);
}

void crystal_in_non_proc(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    crystal_in_link_move(this, play);

    if (this->warpTimer == 0) {
        add_calc(&this->crystalAlpha, 0.0f, 0.1f, 4.0f, 1.0f);
        if (this->crystalAlpha <= 150.0f) {
            player->actor.gravity = -0.1f;
        }
        if (this->crystalAlpha <= 0.0f) {
            Door_Warp1_actor_set_process(this, crystal_in_link_down);
        }
    } else {
        this->warpTimer--;
    }
    this->actor.shape.rot.y += 0x320;
}

void crystal_in_link_down(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->actor.gravity = -0.1f;
}

void crystalzd_in_move(DoorWarp1* this, PlayState* play) {
    if (this->warpTimer != 0) {
        this->warpTimer--;
        add_calc(&this->actor.scale.x, 0.0499f, 0.2f, 0.05f, 0.001f);
        add_calc(&this->actor.scale.y, 0.077f, 0.2f, 0.05f, 0.001f);
        add_calc(&this->actor.scale.z, 0.09f, 0.2f, 0.05f, 0.001f);
        add_calc(&this->crystalAlpha, 255.0f, 0.2f, 5.0f, 0.1f);
    }
}

static void move_dma_wait(DoorWarp1* this, PlayState* play) {
    switch (this->actor.params) {
        case WARP_DUNGEON_CHILD:
        case WARP_CLEAR_FLAG:
        case WARP_SAGES:
        case WARP_YELLOW:
        case WARP_BLUE_RUTO:
        case WARP_DESTINATION:
        case WARP_UNK_7:
        case WARP_ORANGE:
        case WARP_GREEN:
        case WARP_RED:
            move_dma_wait_boss(this, play);
            break;
        case WARP_DUNGEON_ADULT:
            move_dma_wait_crystal(this, play);
            break;
        case WARP_BLUE_CRYSTAL:
            move_dma_wait_crystal_in(this, play);
            break;
        case WARP_PURPLE_CRYSTAL:
            move_dma_wait_crystal_zd(this, play);
            break;
    }
}

void move_Nonproc(DoorWarp1* this, PlayState* play) {
    if (Actor_Environment_no_enemy_Check(play, this->actor.room)) {
        this->warpTimer = 200;
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS_CLEAR);
        Door_Warp1_actor_set_process(this, move_Nonproc2);
    }
}

void move_Nonproc2(DoorWarp1* this, PlayState* play) {
    if (this->warpTimer == 0) {
        if (this->actor.xzDistToPlayer < 100.0f) {
            this->actor.world.pos.x = -98.0f;
            this->actor.world.pos.y = 827.0f;
            this->actor.world.pos.z = -3228.0f;
        }
        Light_point_ct(&this->upperLightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 200, 255, 255, 255);
        Light_point_ct(&this->lowerLightInfo, this->actor.world.pos.x, this->actor.world.pos.y,
                                  this->actor.world.pos.z, 200, 255, 255, 255);
        Door_Warp1_actor_set_process(this, move_barth);
    }
    this->warpTimer--;
}

void move_barth(DoorWarp1* this, PlayState* play) {
    Actor_SE_set(&this->actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);
    add_calc(&this->lightRayAlpha, 255.0f, 0.4f, 10.0f, 0.01f);
    add_calc(&this->warpAlpha, 255.0f, 0.4f, 10.0f, 0.01f);

    if (this->actor.params != WARP_YELLOW && this->actor.params != WARP_ORANGE && this->actor.params != WARP_GREEN &&
        this->actor.params != WARP_RED) {
        if (this->scale < 100) {
            this->scale += 2;
        }
        if (this->unk_1AE < 120) {
            this->unk_1AE += 4;
        }
        if (this->unk_1B0 < 230) {
            this->unk_1B0 += 4;
        } else if (this->actor.params == WARP_BLUE_RUTO) {
            Door_Warp1_actor_set_process(this, move_wait_ruto);
        } else if (this->actor.params != WARP_SAGES && this->actor.params != WARP_YELLOW) {
            Door_Warp1_actor_set_process(this, move_wait);
        } else {
            Door_Warp1_actor_set_process(this, kenjyamove_wait);
        }
    } else {
        if (this->unk_1AE < -50) {
            this->unk_1AE += 4;
        }
        if (this->unk_1B0 < 70) {
            this->unk_1B0 += 4;
        } else {
            Door_Warp1_actor_set_process(this, kenjyamove_wait);
        }
    }
}

void kenjyamove_wait(DoorWarp1* this, PlayState* play) {
    if (this->lightRayAlpha != 0.0f) {
        Actor_SE_set(&this->actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);
    }
    add_calc(&this->lightRayAlpha, 0.0f, 0.1f, 2.0f, 0.01f);
    add_calc(&this->warpAlpha, 0.0f, 0.1f, 2.0f, 0.01f);
}

s32 link_enter_check(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 ret = false;

    if (fabsf(this->actor.xzDistToPlayer) < 60.0f) {
        if ((player->actor.world.pos.y - 20.0f) < this->actor.world.pos.y) {
            if (this->actor.world.pos.y < (player->actor.world.pos.y + 20.0f)) {
                ret = true;
            }
        }
    }
    return ret;
}

static void move_wait(DoorWarp1* this, PlayState* play) {
    Actor_SE_set(&this->actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);

    if (link_enter_check(this, play)) {
        Player* player = GET_PLAYER(play);

        Nai_FxFlagEntry(NA_SE_EV_LINK_WARP, &player->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        makeOnepointDemo(play, 0x25E7, 999, &this->actor, CAM_ID_MAIN);
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_10);

        player->unk_450.x = this->actor.world.pos.x;
        player->unk_450.z = this->actor.world.pos.z;
        this->unk_1B2 = 1;
        Door_Warp1_actor_set_process(this, move_warp);
    }
}

void move_warp(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->unk_1B2 >= 101) {
        if (player->actor.velocity.y < 10.0f) {
            player->actor.gravity = 0.1f;
        } else {
            player->actor.gravity = 0.0f;
        }
    } else {
        this->unk_1B2++;
    }

    add_calc(&this->lightRayAlpha, 0.0f, 0.2f, 6.0f, 0.01f);
    this->warpTimer++;

    if (E_PREG70 < this->warpTimer && z_common_data.nextCutsceneIndex == 0xFFEF) {
        PRINTF("\n\n\nじかんがきたからおーしまい fade_direction=[%d]", play->transitionTrigger, TRANS_TRIGGER_START);

        if (play->sceneId == SCENE_DODONGOS_CAVERN_BOSS) {
            if (!event_check(EVENTCHKINF_25)) {
                event_set(EVENTCHKINF_25);
                item_get_setting(play, ITEM_GORON_RUBY);
                play->nextEntranceIndex = ENTR_DEATH_MOUNTAIN_TRAIL_0;
                z_common_data.nextCutsceneIndex = 0xFFF1;
            } else {
                play->nextEntranceIndex = ENTR_DEATH_MOUNTAIN_TRAIL_5;
                z_common_data.nextCutsceneIndex = 0;
            }
        } else if (play->sceneId == SCENE_DEKU_TREE_BOSS) {
            if (!event_check(EVENTCHKINF_07)) {
                event_set(EVENTCHKINF_07);
                event_set(EVENTCHKINF_09);
                item_get_setting(play, ITEM_KOKIRI_EMERALD);
                play->nextEntranceIndex = ENTR_KOKIRI_FOREST_0;
                z_common_data.nextCutsceneIndex = 0xFFF1;
            } else {
                play->nextEntranceIndex = ENTR_KOKIRI_FOREST_11;
                z_common_data.nextCutsceneIndex = 0;
            }
        } else if (play->sceneId == SCENE_JABU_JABU_BOSS) {
            play->nextEntranceIndex = ENTR_ZORAS_FOUNTAIN_0;
            z_common_data.nextCutsceneIndex = 0;
        }
        PRINTF("\n\n\nおわりおわり");
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_FADE_WHITE_SLOW;
        z_common_data.nextTransitionType = TRANS_TYPE_FADE_WHITE;
    }

    chase_f(&this->unk_194, 2.0f, 0.01f);
    chase_f(&this->unk_198, 10.0f, 0.02f);
    Light_point_ct(&this->upperLightInfo, (s16)player->actor.world.pos.x + 10.0f,
                              (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z + 10.0f, 235, 255,
                              255, 255);
    Light_point_ct(&this->lowerLightInfo, (s16)player->actor.world.pos.x - 10.0f,
                              (s16)player->actor.world.pos.y - 10.0f, (s16)player->actor.world.pos.z - 10.0f, 235, 255,
                              255, 255);
    add_calc(&this->actor.shape.yOffset, 0.0f, 0.5f, 2.0f, 0.1f);
}

void move_wait_ruto(DoorWarp1* this, PlayState* play) {
    Actor_SE_set(&this->actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);

    if (this->rutoWarpState != WARP_BLUE_RUTO_STATE_INITIAL && link_enter_check(this, play)) {
        this->rutoWarpState = WARP_BLUE_RUTO_STATE_ENTERED;
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_10);
        this->unk_1B2 = 1;
        Door_Warp1_actor_set_process(this, move_warp_ruto);
    }
}

static s16 E_rutodemo_cno;

void move_warp_ruto(DoorWarp1* this, PlayState* play) {
    Vec3f at;
    Vec3f eye;
    Player* player = GET_PLAYER(play);

    if (this->rutoWarpState == WARP_BLUE_RUTO_STATE_3) {
        Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
        E_rutodemo_cno = Gama_play_make_camera(play);

        Gama_play_set_camera_status(play, E_rutodemo_cno, CAM_STAT_ACTIVE);
        at.x = this->actor.world.pos.x;
        at.y = 49.0f;
        at.z = this->actor.world.pos.z;
        eye.x = player->actor.world.pos.x;
        eye.y = 43.0f;
        eye.z = player->actor.world.pos.z;

        Gama_play_camera_setting(play, E_rutodemo_cno, &at, &eye);
        Gama_play_set_camera_fovy(play, E_rutodemo_cno, 90.0f);
        this->rutoWarpState = WARP_BLUE_RUTO_STATE_TALKING;
        message_set(play, 0x4022, NULL);
        Door_Warp1_actor_set_process(this, move_warp_ruto2);
    }
}

void move_warp_ruto2(DoorWarp1* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_NONE) {
        Nai_FxFlagEntry(NA_SE_EV_LINK_WARP, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        makeOnepointDemo(play, 0x25E9, 999, &this->actor, CAM_ID_MAIN);
        // Using `CAM_ID_NONE` here defaults to the active camera
        Gama_play_copy_camera_position(play, CAM_ID_NONE, E_rutodemo_cno);
        Gama_play_set_camera_status(play, E_rutodemo_cno, CAM_STAT_WAIT);
        this->rutoWarpState = WARP_BLUE_RUTO_STATE_WARPING;
        Door_Warp1_actor_set_process(this, move_warp_ruto3);
    }
}

void move_warp_ruto3(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->unk_1B2 >= 61) {
        if (player->actor.velocity.y < 10.f) {
            player->actor.gravity = 0.02f;
        } else {
            player->actor.gravity = 0.0f;
        }
    } else {
        this->unk_1B2++;
    }
    add_calc(&this->lightRayAlpha, 0.0f, 0.2f, 6.0f, 0.01f);
    this->warpTimer++;

    if (this->warpTimer > E_PREG70 && z_common_data.nextCutsceneIndex == 0xFFEF) {
        SET_EVENTCHKINF(EVENTCHKINF_37);
        item_get_setting(play, ITEM_ZORA_SAPPHIRE);
        play->nextEntranceIndex = ENTR_ZORAS_FOUNTAIN_0;
        z_common_data.nextCutsceneIndex = 0xFFF0;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_FADE_WHITE_SLOW;
    }

    chase_f(&this->unk_194, 2.0f, 0.01f);
    chase_f(&this->unk_198, 10.f, 0.02f);
    Light_point_ct(&this->upperLightInfo, (s16)player->actor.world.pos.x + 10.0f,
                              (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z + 10.0f, 235, 255,
                              255, 255);
    Light_point_ct(&this->lowerLightInfo, (s16)player->actor.world.pos.x - 10.0f,
                              (s16)player->actor.world.pos.y - 10.0f, (s16)player->actor.world.pos.z - 10.0f, 235, 255,
                              255, 255);
    add_calc(&this->actor.shape.yOffset, 0.0f, 0.5f, 2.0f, 0.1f);
}

void crystal_move_barth(DoorWarp1* this, PlayState* play) {
    Actor_SE_set(&this->actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);
    add_calc(&this->lightRayAlpha, 255.0f, 0.2f, 2.0f, 0.1f);
    add_calc(&this->warpAlpha, 255.0f, 0.2f, 2.0f, 0.1f);

    if (this->scale < 10) {
        this->scale += 2;
    }
    if (this->unk_1AE < 120) {
        this->unk_1AE += 4;
    }
    if (this->unk_1B0 < 230) {
        this->unk_1B0 += 4;
    } else {
        Door_Warp1_actor_set_process(this, crystal_move_wait);
    }
}

void crystal_move_wait(DoorWarp1* this, PlayState* play) {
    Player* player;

    Actor_SE_set(&this->actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);

    if (link_enter_check(this, play)) {
        player = GET_PLAYER(play);

        makeOnepointDemo(play, 0x25E8, 999, &this->actor, CAM_ID_MAIN);
        player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_10);
        player->unk_450.x = this->actor.world.pos.x;
        player->unk_450.z = this->actor.world.pos.z;
        this->unk_1B2 = 20;
        Door_Warp1_actor_set_process(this, crystal_link_wait);
    }
}

void crystal_link_wait(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->unk_1B2 != 0) {
        this->unk_1B2--;
        return;
    }
    Nai_FxFlagEntry(NA_SE_EV_LINK_WARP, &player->actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
    Skeleton_Info2_init2(&this->skelAnime, &gWarpCrystalAnim, 1.0f, Si2_anime_end_frame(&gWarpCrystalAnim),
                         Si2_anime_end_frame(&gWarpCrystalAnim), ANIMMODE_ONCE, 40.0f, ANIMTAPER_ACCEL);

    this->unk_1B2 = 0x32;
    Door_Warp1_actor_set_process(this, crystal_move_warp);
}

void crystal_move_warp(DoorWarp1* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 temp_f0_2;

    if (this->unk_1B2 != 0) {
        this->unk_1B2--;
    }

    if (this->unk_1B2 < 31) {
        u32 phi_v0 = (LINK_IS_ADULT) ? 35 : 45;

        if ((player->actor.world.pos.y - this->actor.world.pos.y) <= phi_v0) {
            player->actor.gravity = 0.0139999995f;
        } else {
            player->actor.gravity = 0.0f;
            player->actor.velocity.y = 0.0f;
        }
        if (this->unk_1B2 <= 0) {
            if (this->unk_1B8 < 4000) {
                this->unk_1B8 += 40;
            }
            player->actor.world.rot.y -= this->unk_1B8;
            player->actor.shape.rot.y -= this->unk_1B8;
        }
        add_calc(&player->actor.world.pos.x, this->actor.world.pos.x, 0.5f, 0.1f, 0.01f);
        add_calc(&player->actor.world.pos.z, this->actor.world.pos.z, 0.5f, 0.1f, 0.01f);
    }
    this->warpTimer++;

    if (this->warpTimer > E_PREG70 && z_common_data.nextCutsceneIndex == 0xFFEF) {
        if (play->sceneId == SCENE_FOREST_TEMPLE_BOSS) {
            if (!GET_EVENTCHKINF(EVENTCHKINF_48)) {
                SET_EVENTCHKINF(EVENTCHKINF_48);
                item_get_setting(play, ITEM_MEDALLION_FOREST);
                play->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
                z_common_data.nextCutsceneIndex = 0;
                z_common_data.chamberCutsceneNum = CHAMBER_CS_FOREST;
            } else {
                if (!LINK_IS_ADULT) {
                    play->nextEntranceIndex = ENTR_SACRED_FOREST_MEADOW_2;
                } else {
                    play->nextEntranceIndex = ENTR_SACRED_FOREST_MEADOW_3;
                }
                z_common_data.nextCutsceneIndex = 0;
            }
        } else if (play->sceneId == SCENE_FIRE_TEMPLE_BOSS) {
            if (!GET_EVENTCHKINF(EVENTCHKINF_49)) {
                SET_EVENTCHKINF(EVENTCHKINF_49);
                item_get_setting(play, ITEM_MEDALLION_FIRE);
                play->nextEntranceIndex = ENTR_KAKARIKO_VILLAGE_0;
                z_common_data.nextCutsceneIndex = 0xFFF3;
            } else {
                if (!LINK_IS_ADULT) {
                    play->nextEntranceIndex = ENTR_DEATH_MOUNTAIN_CRATER_4;
                } else {
                    play->nextEntranceIndex = ENTR_DEATH_MOUNTAIN_CRATER_5;
                }
                z_common_data.nextCutsceneIndex = 0;
            }
        } else if (play->sceneId == SCENE_WATER_TEMPLE_BOSS) {
            if (!GET_EVENTCHKINF(EVENTCHKINF_4A)) {
                SET_EVENTCHKINF(EVENTCHKINF_4A);
                item_get_setting(play, ITEM_MEDALLION_WATER);
                play->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
                z_common_data.nextCutsceneIndex = 0;
                z_common_data.chamberCutsceneNum = CHAMBER_CS_WATER;
            } else {
                if (!LINK_IS_ADULT) {
                    play->nextEntranceIndex = ENTR_LAKE_HYLIA_8;
                } else {
                    play->nextEntranceIndex = ENTR_LAKE_HYLIA_9;
                }
                z_common_data.nextCutsceneIndex = 0;
            }
        } else if (play->sceneId == SCENE_SPIRIT_TEMPLE_BOSS) {
            if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                item_get_setting(play, ITEM_MEDALLION_SPIRIT);
                play->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
                z_common_data.nextCutsceneIndex = 0;
                z_common_data.chamberCutsceneNum = CHAMBER_CS_SPIRIT;
            } else {
                if (!LINK_IS_ADULT) {
                    play->nextEntranceIndex = ENTR_DESERT_COLOSSUS_5;
                } else {
                    play->nextEntranceIndex = ENTR_DESERT_COLOSSUS_8;
                }
                z_common_data.nextCutsceneIndex = 0;
            }
        } else if (play->sceneId == SCENE_SHADOW_TEMPLE_BOSS) {
            if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)) {
                item_get_setting(play, ITEM_MEDALLION_SHADOW);
                play->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
                z_common_data.nextCutsceneIndex = 0;
                z_common_data.chamberCutsceneNum = CHAMBER_CS_SHADOW;
            } else {
                if (!LINK_IS_ADULT) {
                    play->nextEntranceIndex = ENTR_GRAVEYARD_7;
                } else {
                    play->nextEntranceIndex = ENTR_GRAVEYARD_8;
                }
                z_common_data.nextCutsceneIndex = 0;
            }
        }
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_FADE_WHITE;
        z_common_data.nextTransitionType = TRANS_TYPE_FADE_WHITE_SLOW;
    }
    if (this->warpTimer >= 141) {
        f32 screenFillAlpha;

        play->envCtx.fillScreen = true;
        screenFillAlpha = (f32)(this->warpTimer - 140) / 20.0f;

        if (screenFillAlpha > 1.0f) {
            screenFillAlpha = 1.0f;
        }
        play->envCtx.screenFillColor[0] = 160;
        play->envCtx.screenFillColor[1] = 160;
        play->envCtx.screenFillColor[2] = 160;
        play->envCtx.screenFillColor[3] = (u32)(255.0f * screenFillAlpha);

        PRINTF("\nparcent=[%f]", screenFillAlpha);
    }
    Light_point_ct(&this->upperLightInfo, (s16)player->actor.world.pos.x + 10.0f,
                              (s16)player->actor.world.pos.y + 10.0f, (s16)player->actor.world.pos.z + 10.0f, 235, 255,
                              255, 255);
    Light_point_ct(&this->lowerLightInfo, (s16)player->actor.world.pos.x - 10.0f,
                              (s16)player->actor.world.pos.y - 10.0f, (s16)player->actor.world.pos.z - 10.0f, 235, 255,
                              255, 255);

    add_calc(&this->actor.shape.yOffset, 800.0f, 0.5f, 15.0f, 0.1f);
    this->actor.shape.rot.y += 0x320;

    add_calc(&this->unk_1BC, 1.13f, 0.2f, 0.1f, 0.01f);
    chase_f(&this->unk_194, 2.0f, 0.003f);
    chase_f(&this->unk_198, 10.0f, 0.006f);
    add_calc(&this->lightRayAlpha, 0.0f, 0.2f, 3.0f, 0.01f);
    add_calc(&this->warpAlpha, 0.0f, 0.2f, 2.0f, 0.01f);
    add_calc(&this->crystalAlpha, 255.0f, 0.1f, 1.0f, 0.01f);

    temp_f0_2 = 1.0f - (f32)(E_PREG70 - this->warpTimer) / (E_PREG70 - (E_PREG70 - 100));
    if (temp_f0_2 > 0.0f) {
        s16 i;

        for (i = 0; i < 3; i++) {
            play->envCtx.adjAmbientColor[i] = play->envCtx.adjFogColor[i] = play->envCtx.adjLight1Color[i] =
                -255.0f * temp_f0_2;
        }

        play->envCtx.adjFogNear = -500.0f * temp_f0_2;
        if (play->envCtx.adjFogNear < -300) {
            play->roomCtx.curRoom.segment = NULL;
        }
    }
}

void move_warp6(DoorWarp1* this, PlayState* play) {
    f32 alphaFrac = 1.0f;

    this->unk_194 = 5.0f;
    this->warpTimer++;
    if (this->warpTimer < 20) {
        alphaFrac = this->warpTimer / 20.f;
    } else if (this->warpTimer >= 60) {
        alphaFrac = 1.0f - ((this->warpTimer - 60.0f) / 20.f);
    }
    this->warpAlpha = 255.0f * alphaFrac;
    this->lightRayAlpha = 0.0f;

    if (this->warpTimer >= 80.0f) {
        this->warpAlpha = 0.0f;
        Door_Warp1_actor_set_process(this, move_warp6_end);
    }
    Actor_SE_set(&this->actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);
}

void move_warp6_end(DoorWarp1* this, PlayState* play) {
}

void move_warp7(DoorWarp1* this, PlayState* play) {
    add_calc(&this->lightRayAlpha, 128.0f, 0.2f, 2.0f, 0.1f);
    add_calc(&this->warpAlpha, 128.0f, 0.2f, 2.0f, 0.1f);

    if (this->lightRayAlpha >= 128.0f) {
        chase_f(&this->unk_194, 2.0f, 0.01f);
        chase_f(&this->unk_198, 10.0f, 0.02f);
    }
    Actor_SE_set(&this->actor, NA_SE_EV_WARP_HOLE - SFX_FLAG);
}

void Door_Warp1_actor_move(Actor* thisx, PlayState* play) {
    DoorWarp1* this = (DoorWarp1*)thisx;

    this->actionFunc(this, play);

    if (this->actor.params != WARP_PURPLE_CRYSTAL) {
        Actor_set_scale(&this->actor, this->scale / 100.0f);
    }
}

void Door_Warp1_actor_draw_crystal(DoorWarp1* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_door_warp1.c", 2078);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0xFF, 0xFF, 200, 255, 255, (u8)this->crystalAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 255, (u8)this->crystalAlpha);

    POLY_XLU_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, &this->actor,
                                   POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_door_warp1.c", 2098);

    Skeleton_Info2_anime_play(&this->skelAnime);
}

void Door_Warp1_actor_draw_crystal_zd(DoorWarp1* this, PlayState* play) {
    s32 pad[2];
    Vec3f eye;

    eye.x = -(sin_s(play->state.frames * 200) * 120.0f) * 80.0f;
    eye.y = (cos_s(play->state.frames * 200) * 120.0f) * 80.0f;
    eye.z = (cos_s(play->state.frames * 200) * 120.0f) * 80.0f;

    OPEN_DISPS(play->state.gfxCtx, "../z_door_warp1.c", 2122);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    HiliteReflect_xlu_init(&this->actor.world.pos, &eye, &eye, play->state.gfxCtx);

    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)this->crystalAlpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 150, 0, 100, (u8)this->crystalAlpha);

    POLY_XLU_DISP = Si2_draw2(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, &this->actor,
                                   POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx, "../z_door_warp1.c", 2152);

    Skeleton_Info2_anime_play(&this->skelAnime);
}

void Door_Warp1_actor_draw_boss(DoorWarp1* this, PlayState* play) {
    s32 pad;
    u32 pad1;
    u32 spEC = play->state.frames * 10;
    f32 spE8 = (this->unk_194 >= 1.0f) ? 0.0f : 1.0f - this->unk_194;
    f32 spE4 = (this->unk_198 >= 1.0f) ? 0.0f : 1.0f - this->unk_198;
    f32 xzScale;
    f32 temp_f0;

    OPEN_DISPS(play->state.gfxCtx, "../z_door_warp1.c", 2173);

    temp_f0 = 1.0f - (2.0f - this->unk_194) / 1.7f;
    if (this->actor.params != WARP_YELLOW && this->actor.params != WARP_DESTINATION &&
        this->actor.params != WARP_ORANGE && this->actor.params != WARP_GREEN && this->actor.params != WARP_RED) {
        this->unk_19C += (s16)(temp_f0 * 15.0f);
    }
    if (this->actor.params == WARP_DESTINATION) {
        this->unk_19C -= (s16)(temp_f0 * 2.0f);
    }
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    switch (this->actor.params) {
        case WARP_YELLOW:
            gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 255, 255, (u8)this->warpAlpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 200, 255, 0, 255);
            break;
        case WARP_ORANGE:
            gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 255, 255, (u8)this->warpAlpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 150, 0, 255);
            break;
        case WARP_GREEN:
            gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 255, 255, (u8)this->warpAlpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 200, 0, 255);
            break;
        case WARP_RED:
            gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 255, 255, (u8)this->warpAlpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 0, 255);
            break;
        default:
            gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255 * temp_f0, 255, 255, (u8)this->warpAlpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 255 * temp_f0, 255, 255);
            break;
    }
    gDPSetColorDither(POLY_XLU_DISP++, G_CD_DISABLE);
    gDPSetColorDither(POLY_XLU_DISP++, G_AD_NOTPATTERN | G_CD_MAGICSQ);

    Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y + 1.0f, this->actor.world.pos.z, MTXMODE_NEW);
    gSPSegment(POLY_XLU_DISP++, 0x0A, MATRIX_FINALIZE(play->state.gfxCtx, "../z_door_warp1.c", 2247));
    Matrix_push();

    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, spEC & 0xFF,
                                -((s16)(this->unk_19C + this->unk_19C) & 511), 0x100, 0x100, 1, spEC & 0xFF,
                                -((s16)(this->unk_19C + this->unk_19C) & 511), 0x100, 0x100));

    Matrix_translate(0.0f, this->unk_194 * 230.0f, 0.0f, MTXMODE_APPLY);
    xzScale = (((f32)this->unk_1AE * spE8) / 100.0f) + 1.0f;
    Matrix_scale(xzScale, 1.0f, xzScale, MTXMODE_APPLY);
    gSPSegment(POLY_XLU_DISP++, 0x09, MATRIX_FINALIZE(play->state.gfxCtx, "../z_door_warp1.c", 2267));
    gSPDisplayList(POLY_XLU_DISP++, gWarpPortalDL);
    Matrix_pull();

    if (this->lightRayAlpha > 0.0f) {
        switch (this->actor.params) {
            case WARP_YELLOW:
                gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 255, 255, (u8)this->warpAlpha);
                gDPSetEnvColor(POLY_XLU_DISP++, 200, 255, 0, 255);
                break;
            case WARP_ORANGE:
                gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 255, 255, (u8)this->warpAlpha);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 150, 0, 255);
                break;
            case WARP_GREEN:
                gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 255, 255, (u8)this->warpAlpha);
                gDPSetEnvColor(POLY_XLU_DISP++, 0, 200, 0, 255);
                break;
            case WARP_RED:
                gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 255, 255, (u8)this->warpAlpha);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 0, 255);
                break;
            default:
                gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255 * temp_f0, 255, 255, (u8)this->lightRayAlpha);
                gDPSetEnvColor(POLY_XLU_DISP++, 0, 255 * temp_f0, 255, 255);
                break;
        }
        spEC *= 2;

        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, spEC & 0xFF, -((s16)this->unk_19C & 511),
                                    0x100, 0x100, 1, spEC & 0xFF, -((s16)this->unk_19C & 511), 0x100, 0x100));

        Matrix_translate(0.0f, this->unk_198 * 60.0f, 0.0f, MTXMODE_APPLY);

        xzScale = (((f32)this->unk_1B0 * spE4) / 100.0f) + 1.0f;
        Matrix_scale(xzScale, 1.0f, xzScale, MTXMODE_APPLY);

        gSPSegment(POLY_XLU_DISP++, 0x09, MATRIX_FINALIZE(play->state.gfxCtx, "../z_door_warp1.c", 2336));
        gSPDisplayList(POLY_XLU_DISP++, gWarpPortalDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_door_warp1.c", 2340);
}

void Door_Warp1_actor_draw(Actor* thisx, PlayState* play) {
    DoorWarp1* this = (DoorWarp1*)thisx;

    switch (this->actor.params) {
        case WARP_DUNGEON_ADULT:
            Door_Warp1_actor_draw_crystal(this, play);
            Door_Warp1_actor_draw_boss(this, play);
            break;
        case WARP_DUNGEON_CHILD:
        case WARP_CLEAR_FLAG:
        case WARP_SAGES:
        case WARP_YELLOW:
        case WARP_BLUE_RUTO:
        case WARP_DESTINATION:
        case WARP_UNK_7:
        case WARP_ORANGE:
        case WARP_GREEN:
        case WARP_RED:
            Door_Warp1_actor_draw_boss(this, play);
            break;
        case WARP_BLUE_CRYSTAL:
            Door_Warp1_actor_draw_crystal(this, play);
            break;
        case WARP_PURPLE_CRYSTAL:
            Door_Warp1_actor_draw_crystal_zd(this, play);
            break;
    }
}
