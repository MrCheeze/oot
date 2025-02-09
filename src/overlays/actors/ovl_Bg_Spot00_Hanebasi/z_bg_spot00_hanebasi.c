/*
 * File: z_bg_spot00_hanebasi.c
 * Overlay: ovl_Bg_Spot00_Hanebasi
 * Description: Hyrule Field Drawbridge and Torches
 */

#include "z_bg_spot00_hanebasi.h"
#include "assets/objects/object_spot00_objects/object_spot00_objects.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef enum DrawbridgeType {
    /* -1 */ DT_DRAWBRIDGE = -1,
    /*  0 */ DT_CHAIN_1,
    /*  1 */ DT_CHAIN_2
} DrawbridgeType;

void Bg_Spot00_Hanebasi_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot00_Hanebasi_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot00_Hanebasi_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot00_Hanebasi_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgSpot00Hanebasi* this, PlayState* play);
static void mode_move(BgSpot00Hanebasi* this, PlayState* play);
void mode_chain(BgSpot00Hanebasi* this, PlayState* play);

ActorProfile Bg_Spot00_Hanebasi_Profile = {
    /**/ ACTOR_BG_SPOT00_HANEBASI,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT00_OBJECTS,
    /**/ sizeof(BgSpot00Hanebasi),
    /**/ Bg_Spot00_Hanebasi_actor_ct,
    /**/ Bg_Spot00_Hanebasi_actor_dt,
    /**/ Bg_Spot00_Hanebasi_actor_move,
    /**/ Bg_Spot00_Hanebasi_actor_draw,
};

static f32 torch_scale = 0.0f;

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 550, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 5000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void Bg_Spot00_Hanebasi_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot00Hanebasi* this = (BgSpot00Hanebasi*)thisx;
    s32 pad;
    Vec3f chainPos;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);

    if (this->dyna.actor.params == DT_DRAWBRIDGE) {
        DynaPolyUty_bgdi_SG2KSG(&gHyruleFieldCastleDrawbridgeCol, &colHeader);
    } else {
        DynaPolyUty_bgdi_SG2KSG(&gHyruleFieldCastleDrawbridgeChainsCol, &colHeader);
    }

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    if (this->dyna.actor.params == DT_DRAWBRIDGE) {
        if (LINK_IS_ADULT && !IS_CUTSCENE_LAYER) {
            Actor_delete(&this->dyna.actor);
            return;
        }

        if ((z_common_data.sceneLayer != 6) &&
            ((z_common_data.sceneLayer == 4) || (z_common_data.sceneLayer == 5) || (!LINK_IS_ADULT && !IS_DAY))) {
            this->dyna.actor.shape.rot.x = -0x4000;
        } else {
            this->dyna.actor.shape.rot.x = 0;
        }

        if (z_common_data.sceneLayer != 6) {
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && CHECK_QUEST_ITEM(QUEST_GORON_RUBY) &&
                CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE) && !GET_EVENTCHKINF(EVENTCHKINF_80)) {
                this->dyna.actor.shape.rot.x = -0x4000;
            }
        }

        chainPos.y =
            (10.0f * cos_s(this->dyna.actor.shape.rot.x)) - (sin_s(this->dyna.actor.shape.rot.x) * 400.0f);
        chainPos.z =
            (10.0f * sin_s(this->dyna.actor.shape.rot.x)) - (cos_s(this->dyna.actor.shape.rot.x) * 400.0f);
        chainPos.x =
            (158.0f * cos_s(this->dyna.actor.shape.rot.y)) + (sin_s(this->dyna.actor.shape.rot.y) * chainPos.z);
        chainPos.z = (-158.0f * sin_s(this->dyna.actor.shape.rot.y)) +
                     (cos_s(this->dyna.actor.shape.rot.y) * chainPos.z);

        if (Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_BG_SPOT00_HANEBASI,
                               this->dyna.actor.world.pos.x + chainPos.x, this->dyna.actor.world.pos.y + chainPos.y,
                               this->dyna.actor.world.pos.z + chainPos.z,
                               ((this->dyna.actor.shape.rot.x == 0) ? 0 : 0xF020), this->dyna.actor.shape.rot.y, 0,
                               DT_CHAIN_1) == NULL) {
            Actor_delete(&this->dyna.actor);
        }

        this->actionFunc = mode_wait;
        this->destAngle = 40;
    } else if (this->dyna.actor.params == DT_CHAIN_1) {
        if (Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_BG_SPOT00_HANEBASI,
                               this->dyna.actor.world.pos.x - (cos_s(this->dyna.actor.shape.rot.y) * 316.0f),
                               this->dyna.actor.world.pos.y,
                               this->dyna.actor.world.pos.z + (sin_s(this->dyna.actor.shape.rot.y) * 316.0f),
                               this->dyna.actor.shape.rot.x, this->dyna.actor.shape.rot.y, 0, DT_CHAIN_2) == NULL) {
            Actor_delete(&this->dyna.actor);
            Actor_delete(this->dyna.actor.parent);
        }

        this->actionFunc = mode_chain;
    } else {
        this->actionFunc = mode_chain;
    }

    if (this->dyna.actor.params >= DT_CHAIN_1) {
        this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
        Light_point2_ct(&this->lightInfo, ((this->dyna.actor.params == DT_CHAIN_1) ? 260.0f : -260.0f), 168,
                                690, 255, 255, 0, 0);
    }
}

void Bg_Spot00_Hanebasi_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot00Hanebasi* this = (BgSpot00Hanebasi*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);

    if (this->dyna.actor.params >= DT_CHAIN_1) {
        Global_light_list_delete(play, &play->lightCtx, this->lightNode);
    }
}

static void mode_wait(BgSpot00Hanebasi* this, PlayState* play) {
    BgSpot00Hanebasi* child = (BgSpot00Hanebasi*)this->dyna.actor.child;

    if (!IS_CUTSCENE_LAYER && CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && CHECK_QUEST_ITEM(QUEST_GORON_RUBY) &&
        CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE) && !GET_EVENTCHKINF(EVENTCHKINF_80)) {
        return;
    }

    if ((this->dyna.actor.shape.rot.x != 0) && (eventbit_check(play, 0) || (!IS_CUTSCENE_LAYER && IS_DAY))) {
        this->actionFunc = mode_move;
        this->destAngle = 0;
        child->destAngle = 0;
    } else if ((this->dyna.actor.shape.rot.x == 0) && !IS_CUTSCENE_LAYER && !LINK_IS_ADULT && !IS_DAY) {
        this->actionFunc = mode_move;
        this->destAngle = -0x4000;
        child->destAngle = -0xFE0;
    }
}

static void mode_stop(BgSpot00Hanebasi* this, PlayState* play) {
}

static void mode_move(BgSpot00Hanebasi* this, PlayState* play) {
    BgSpot00Hanebasi* child;
    Actor* childsChild;
    s16 angle = 80;

    if (chase_angle(&this->dyna.actor.shape.rot.x, this->destAngle, 80)) {
        this->actionFunc = mode_wait;
    }

    if (this->dyna.actor.shape.rot.x >= -0x27D8) {
        child = (BgSpot00Hanebasi*)this->dyna.actor.child;
        angle *= 0.4f;
        chase_angle(&child->dyna.actor.shape.rot.x, child->destAngle, angle);
        childsChild = child->dyna.actor.child;
        chase_angle(&childsChild->shape.rot.x, child->destAngle, angle);
    }

    if (this->destAngle < 0) {
        if (this->actionFunc == mode_wait) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_BRIDGE_CLOSE_STOP);
        } else {
            Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_BRIDGE_CLOSE - SFX_FLAG);
        }
    } else {
        if (this->actionFunc == mode_wait) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_BRIDGE_OPEN_STOP);
        } else {
            Actor_level_SE_set(&this->dyna.actor, NA_SE_EV_BRIDGE_OPEN - SFX_FLAG);
        }
    }
}

void mode_chain(BgSpot00Hanebasi* this, PlayState* play) {
    u8 lightColor = (u8)(fqrand() * 127.0f) + 128; // intensity of the red and green channels

    Light_point2_ct(&this->lightInfo, (this->dyna.actor.params == DT_CHAIN_1) ? 260.0f : -260.0f,
                            (5000.0f * torch_scale) + 128.0f, 690, lightColor, lightColor, 0,
                            torch_scale * 37500.0f);
}

void Bg_Spot00_Hanebasi_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgSpot00Hanebasi* this = (BgSpot00Hanebasi*)thisx;
    Player* player;

    this->actionFunc(this, play);

    if (this->dyna.actor.params == DT_DRAWBRIDGE) {
        if (play->sceneId == SCENE_HYRULE_FIELD) {
            if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) && CHECK_QUEST_ITEM(QUEST_GORON_RUBY) &&
                CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE) && !GET_EVENTCHKINF(EVENTCHKINF_80) && LINK_IS_CHILD) {
                player = GET_PLAYER(play);

                if ((player->actor.world.pos.x > -450.0f) && (player->actor.world.pos.x < 450.0f) &&
                    (player->actor.world.pos.z > 1080.0f) && (player->actor.world.pos.z < 1700.0f) &&
                    (!(Game_play_demo_mode_check(play)))) {
                    SET_EVENTCHKINF(EVENTCHKINF_80);
                    event_set(EVENTCHKINF_82);
                    this->actionFunc = mode_stop;
                    player_demo_mode_set(play, &player->actor, PLAYER_CSACTION_8);
                    play->nextEntranceIndex = ENTR_HYRULE_FIELD_0;
                    z_common_data.nextCutsceneIndex = 0xFFF1;
                    play->transitionTrigger = TRANS_TRIGGER_START;
                    play->transitionType = TRANS_TYPE_FADE_BLACK_FAST;
                } else if (Actor_player_distance_direction_check(&this->dyna.actor, 3000.0f, 0x7530)) {
                    play->envCtx.stormRequest = STORM_REQUEST_START;
                }
            }
        }

        if (z_common_data.sceneLayer == 5) {
            u16 dayTime;
            s32 tmp;

            if (E_day_time_plus == 50) {
                tmp = CLOCK_TIME(20, 0) + 1;

                if (z_common_data.save.dayTime > CLOCK_TIME(20, 0) + 1) {
                    tmp = CLOCK_TIME(20, 0) + 1 + 0x10000;
                }

                E_day_time_plus = (tmp - z_common_data.save.dayTime) * (1.0f / 350.0f);
            }

            dayTime = z_common_data.save.dayTime;

            if ((dayTime > CLOCK_TIME(4, 0)) && (dayTime < CLOCK_TIME(4, 30)) && (z_common_data.sceneLayer == 5)) {
                E_day_time_plus = 0;
            }
        }
    }
}

void spot00_draw_torch(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    f32 angle;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot00_hanebasi.c", 633);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    if (IS_CUTSCENE_LAYER) {
        torch_scale = 0.008f;
    } else {
        torch_scale = ((thisx->shape.rot.x * -1) - 0x2000) * (1.0f / 1024000.0f);
    }

    angle = BINANG_TO_RAD((s16)(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000));
    gDPSetPrimColor(POLY_XLU_DISP++, 128, 128, 255, 255, 0, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

    for (i = 0; i < 2; i++) {
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0,
                                    ((play->gameplayFrames + i) * -20) & 0x1FF, 32, 128));

        Matrix_translate((i == 0) ? 260.0f : -260.0f, 128.0f, 690.0f, MTXMODE_NEW);
        Matrix_rotateY(angle, MTXMODE_APPLY);
        Matrix_scale(torch_scale, torch_scale, torch_scale, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot00_hanebasi.c", 674);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot00_hanebasi.c", 681);
}

void Bg_Spot00_Hanebasi_actor_draw(Actor* thisx, PlayState* play) {
    Vec3f basePos = { 158.0f, 10.0f, 400.0f };
    Vec3f newPos;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot00_hanebasi.c", 698);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_spot00_hanebasi.c", 702);

    if (thisx->params == DT_DRAWBRIDGE) {
        gSPDisplayList(POLY_OPA_DISP++, gHyruleFieldCastleDrawbridgeDL);

        Matrix_Position(&basePos, &newPos);
        thisx->child->world.pos.x = newPos.x;
        thisx->child->world.pos.y = newPos.y;
        thisx->child->world.pos.z = newPos.z;
        basePos.x *= -1.0f;

        Matrix_Position(&basePos, &newPos);
        thisx->child->child->world.pos.x = newPos.x;
        thisx->child->child->world.pos.y = newPos.y;
        thisx->child->child->world.pos.z = newPos.z;

        if (z_common_data.sceneLayer != 12) {
            if (IS_CUTSCENE_LAYER || (!LINK_IS_ADULT && (thisx->shape.rot.x < -0x2000))) {
                spot00_draw_torch(thisx, play);
            } else {
                torch_scale = 0.0f;
            }
        }
    } else {
        gSPDisplayList(POLY_OPA_DISP++, gHyruleFieldCastleDrawbridgeChainsDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot00_hanebasi.c", 733);
}
