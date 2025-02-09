/*
 * File: z_en_viewer.c
 * Overlay: ovl_En_Viewer
 * Description: Cutscene Actors
 */

#include "z_en_viewer.h"
#include "overlays/actors/ovl_Boss_Ganon/z_boss_ganon.h"
#include "overlays/actors/ovl_En_Ganon_Mant/z_en_ganon_mant.h"
#include "assets/objects/object_zl4/object_zl4.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_horse_zelda/object_horse_zelda.h"
#include "assets/objects/object_horse_ganon/object_horse_ganon.h"
#include "assets/objects/object_im/object_im.h"
#include "assets/objects/object_gndd/object_gndd.h"
#include "assets/objects/object_ganon/object_ganon.h"
#include "assets/objects/object_opening_demo1/object_opening_demo1.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Viewer_actor_ct(Actor* thisx, PlayState* play);
void En_Viewer_actor_dt(Actor* thisx, PlayState* play);
void En_Viewer_actor_move(Actor* thisx, PlayState* play);
void En_Viewer_actor_draw(Actor* thisx, PlayState* play);

void DemoNpcMoveProc(EnViewer* this, PlayState* play);
void Demo_GndHonoo_draw(EnViewer* this2, PlayState* play);
void mant_test(PlayState* play, EnViewer* this);
static void move_dma_wait(EnViewer* this, PlayState* play);
static void move_demo(EnViewer* this, PlayState* play);

static u8 GOUIN = false;

ActorProfile En_Viewer_Profile = {
    /**/ ACTOR_EN_VIEWER,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnViewer),
    /**/ En_Viewer_actor_ct,
    /**/ En_Viewer_actor_dt,
    /**/ En_Viewer_actor_move,
    /**/ En_Viewer_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_STOP),
};

static EnViewerInitData demo_cast_status[] = {
    /* ENVIEWER_TYPE_0_HORSE_ZELDA */
    { OBJECT_HORSE_ZELDA, OBJECT_HORSE_ZELDA, 1, 0, ENVIEWER_SHADOW_HORSE, 20, ENVIEWER_DRAW_HORSE, &gHorseZeldaSkel,
      &gHorseZeldaGallopingAnim },
    /* ENVIEWER_TYPE_1_IMPA */
    { OBJECT_IM, OBJECT_OPENING_DEMO1, 1, 0, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_IMPA, &gImpaSkel,
      &object_opening_demo1_Anim_0029CC },
    /* ENVIEWER_TYPE_2_ZELDA */
    { OBJECT_ZL4, OBJECT_OPENING_DEMO1, 1, 0, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_ZELDA, &gChildZeldaSkel,
      &object_opening_demo1_Anim_000450 },
    /* ENVIEWER_TYPE_3_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &gYoungGanondorfSkel,
      &gYoungGanondorfHorsebackIdleAnim },
    /* ENVIEWER_TYPE_4_HORSE_GANONDORF */
    { OBJECT_HORSE_GANON, OBJECT_HORSE_GANON, 1, 0, ENVIEWER_SHADOW_HORSE, 20, ENVIEWER_DRAW_HORSE, &gHorseGanonSkel,
      &gHorseGanonRearingAnim },
    /* ENVIEWER_TYPE_5_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &gYoungGanondorfSkel,
      &gYoungGanondorfHorsebackRideAnim },
    /* ENVIEWER_TYPE_6_HORSE_GANONDORF */
    { OBJECT_HORSE_GANON, OBJECT_HORSE_GANON, 1, 0, ENVIEWER_SHADOW_HORSE, 20, ENVIEWER_DRAW_HORSE, &gHorseGanonSkel,
      &gHorseGanonGallopingAnim },
    /* ENVIEWER_TYPE_7_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &gYoungGanondorfSkel,
      &gYoungGanondorfArmsCrossedAnim },
    /* ENVIEWER_TYPE_8_GANONDORF */
    { OBJECT_GNDD, OBJECT_GNDD, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &gYoungGanondorfSkel,
      &gYoungGanondorfWalkAnim },
    /* ENVIEWER_TYPE_9_GANONDORF */
    { OBJECT_GANON, OBJECT_GANON, 1, -6, ENVIEWER_SHADOW_NONE, 10, ENVIEWER_DRAW_GANONDORF, &gGanondorfSkel,
      &gGanondorfEndingFloatAnim },
};

static EnGanonMant* mant;

static Vec3f gndd_head_pos;

void En_Viewer_actor_set_process(EnViewer* this, EnViewerActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Viewer_actor_ct(Actor* thisx, PlayState* play) {
    EnViewer* this = (EnViewer*)thisx;
    u8 type;

    ValueSet_process(&this->actor, value_init);
    En_Viewer_actor_set_process(this, move_dma_wait);
    GOUIN = false;
    type = PARAMS_GET_NOMASK(this->actor.params, 8);
    this->unused = 0;
    this->state = 0;
    this->isVisible = false;
    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF || type == ENVIEWER_TYPE_7_GANONDORF ||
        type == ENVIEWER_TYPE_8_GANONDORF || type == ENVIEWER_TYPE_9_GANONDORF) {
        mant = (EnGanonMant*)Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_GANON_MANT,
                                                          0.0f, 0.0f, 0.0f, 0, 0, 0, 35);
    }
}

void En_Viewer_actor_dt(Actor* thisx, PlayState* play) {
    EnViewer* this = (EnViewer*)thisx;

    Skin_AnimationWorkBuffer2_dt(play, &this->skin);
}

void skeleton_shape_init(EnViewer* this, PlayState* play, void* skeletonHeaderSeg,
                                       AnimationHeader* anim) {
    s16 type = PARAMS_GET_NOMASK(this->actor.params, 8);

    if (type == ENVIEWER_TYPE_2_ZELDA || type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF ||
        type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF || type == ENVIEWER_TYPE_9_GANONDORF) {
        Skeleton_Info2_SV_M_ct(play, &this->skin.skelAnime, skeletonHeaderSeg, NULL, NULL, NULL, 0);
    } else {
        Skeleton_Info2_M_ct(play, &this->skin.skelAnime, skeletonHeaderSeg, NULL, NULL, NULL, 0);
    }

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->animObjectSlot].segment);
    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF ||
        type == ENVIEWER_TYPE_9_GANONDORF) {
        Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, anim, 1.0f);
    } else {
        Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, anim, 3.0f);
    }
}

void skeleton_shape_init_inpa(EnViewer* this, PlayState* play, void* skeletonHeaderSeg, AnimationHeader* anim) {
    Skeleton_Info2_SV_M_ct(play, &this->skin.skelAnime, skeletonHeaderSeg, NULL, NULL, NULL, 0);
    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->animObjectSlot].segment);
    Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, anim, 3.0f);
}

void skin_shape_init(EnViewer* this, PlayState* play, void* skeletonHeaderSeg, AnimationHeader* anim) {
    u8 type;

    Skin_AnimationWorkBuffer2_ct(play, &this->skin, skeletonHeaderSeg, anim);
    type = PARAMS_GET_NOMASK(this->actor.params, 8);
    if (!(type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_4_HORSE_GANONDORF ||
          type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF ||
          type == ENVIEWER_TYPE_9_GANONDORF)) {
        Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, anim, 3.0f);
    } else {
        Skeleton_Info2_init_standard_speedset_stop(&this->skin.skelAnime, anim, 1.0f);
    }
}

static EnViewerInitAnimFunc shape_init[] = {
    skeleton_shape_init,
    skin_shape_init,
    skeleton_shape_init,
    skeleton_shape_init_inpa,
};

static ActorShadowFunc shadow_proc[] = {
    NULL,
    Actor_shadow_circle,
    Actor_shadow_horse,
};

static void move_dma_wait(EnViewer* this, PlayState* play) {
    EnViewerInitData* initData = &demo_cast_status[PARAMS_GET_NOMASK(this->actor.params, 8)];
    s32 skelObjectSlot = Object_Exchange_bank_check(&play->objectCtx, initData->skeletonObject);

    ASSERT(skelObjectSlot >= 0, "bank_ID >= 0", "../z_en_viewer.c", 576);

    this->animObjectSlot = Object_Exchange_bank_check(&play->objectCtx, initData->animObject);
    ASSERT(this->animObjectSlot >= 0, "this->anime_bank_ID >= 0", "../z_en_viewer.c", 579);

    if (!Object_Exchange_bank_dma_check(&play->objectCtx, skelObjectSlot) ||
        !Object_Exchange_bank_dma_check(&play->objectCtx, this->animObjectSlot)) {
        this->actor.flags &= ~ACTOR_FLAG_INSIDE_CULLING_VOLUME;
        return;
    }

    this->isVisible = true;
    this->actor.objectSlot = skelObjectSlot;
    Actor_set_segment(play, &this->actor);
    Actor_set_scale(&this->actor, initData->scale / 100.0f);
    Shape_Info_init(&this->actor.shape, initData->yOffset * 100, shadow_proc[initData->shadowType],
                    initData->shadowScale);
    this->drawFuncIndex = initData->drawType;
    shape_init[this->drawFuncIndex](this, play, initData->skeletonHeaderSeg, initData->anim);
    En_Viewer_actor_set_process(this, move_demo);
}

static s16 maniawase = 0;

static void move_demo(EnViewer* this, PlayState* play) {
    u8 type = PARAMS_GET_NOMASK(this->actor.params, 8);
    u16 csCurFrame;
    s32 animationEnded;

    if (type == ENVIEWER_TYPE_2_ZELDA) {
        if (z_common_data.sceneLayer == 5) {
            csCurFrame = play->csCtx.curFrame;
            if (csCurFrame == 792) {
                Actor_SE_set(&this->actor, NA_SE_VO_Z0_SURPRISE);
            } else if (csCurFrame == 845) {
                Actor_SE_set(&this->actor, NA_SE_VO_Z0_THROW);
            }
        }
    } else if (type == ENVIEWER_TYPE_7_GANONDORF) {
        Actor_set_scale(&this->actor, 0.3f);
        this->actor.cullingVolumeDistance = 10000.0f;
        this->actor.cullingVolumeScale = 10000.0f;
        this->actor.cullingVolumeDownward = 10000.0f;
    } else if (type == ENVIEWER_TYPE_3_GANONDORF) {
        if (z_common_data.sceneLayer == 4) {
            switch (play->csCtx.curFrame) {
                case 20:
                case 59:
                case 71:
                case 129:
                case 140:
                case 219:
                case 280:
                case 320:
                case 380:
                case 409:
                case 438:
                    Nai_FxFlagEntry(NA_SE_SY_DEMO_CUT, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    break;
            }
        }
        if (z_common_data.sceneLayer == 5) {
            if (play->csCtx.curFrame == 1508) {
                Actor_SE_set(&this->actor, NA_SE_EN_FANTOM_ST_LAUGH);
            }
            if (play->csCtx.curFrame == 1545) {
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, 32.0f, 101.0f, 1226.0f, 0, 0, 0,
                                   0xC);
            }
        }
        if (play->csCtx.curFrame == 1020) {
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_FANFARE, 0, 0, NA_BGM_OPENING_GANON);
        }
        if (play->csCtx.curFrame == 960) {
            Nai_FxFlagEntry(NA_SE_EV_HORSE_GROAN, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }
    } else if (type == ENVIEWER_TYPE_6_HORSE_GANONDORF) {
        if (z_common_data.sceneLayer == 5 || z_common_data.sceneLayer == 10) {
            Actor_SE_set(&this->actor, NA_SE_EV_HORSE_RUN_LEVEL - SFX_FLAG);
        }
    } else if (type == ENVIEWER_TYPE_4_HORSE_GANONDORF) {
        s16 curFrame = this->skin.skelAnime.curFrame;

        if (this->skin.skelAnime.animation == &gHorseGanonRearingAnim) {
            if (curFrame == 8) {
                Actor_SE_set(&this->actor, NA_SE_EV_GANON_HORSE_NEIGH);
            }
            if (curFrame == 30) {
                Actor_SE_set(&this->actor, NA_SE_EV_HORSE_LAND2);
            }
        } else if (this->skin.skelAnime.animation == &gHorseGanonIdleAnim) {
            if (curFrame == 25) {
                Actor_SE_set(&this->actor, NA_SE_EV_HORSE_SANDDUST);
            }
        } else if (this->skin.skelAnime.animation == &gHorseGanonGallopingAnim) {
            Actor_SE_set(&this->actor, NA_SE_EV_HORSE_RUN_LEVEL - SFX_FLAG);
        }
    }

    if (maniawase != 0) {
        maniawase--;
    }

    DemoNpcMoveProc(this, play);
    Actor_position_moveF(&this->actor); // has no effect, speed/velocity and gravity are 0

    animationEnded = Skeleton_Info2_anime_play(&this->skin.skelAnime);
    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_4_HORSE_GANONDORF) {
        if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.actorCues[1] != NULL) {
            if (play->csCtx.actorCues[1]->id == 2 && maniawase == 0) {
                if (type == ENVIEWER_TYPE_3_GANONDORF) {
                    if (this->skin.skelAnime.animation != &gYoungGanondorfHorsebackIdleAnim) {
                        Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &gYoungGanondorfHorsebackIdleAnim, 1.0f);
                    }
                } else if (this->skin.skelAnime.animation != &gHorseGanonIdleAnim) {
                    Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &gHorseGanonIdleAnim, 1.0f);
                }
            } else if (play->csCtx.actorCues[1]->id == 1) {
                maniawase = 100;
                if (type == ENVIEWER_TYPE_3_GANONDORF) {
                    if (this->skin.skelAnime.animation != &gYoungGanondorfHorsebackRearAnim) {
                        Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &gYoungGanondorfHorsebackRearAnim, 1.0f);
                    }
                } else if (this->skin.skelAnime.animation != &gHorseGanonRearingAnim) {
                    Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &gHorseGanonRearingAnim, 1.0f);
                }
            } else if (type == ENVIEWER_TYPE_3_GANONDORF) {
                switch (this->state) {
                    case 0:
                        if (play->csCtx.actorCues[1]->id == 4) {
                            Skeleton_Info2_init_standard_stop_morf(&this->skin.skelAnime,
                                                      &gYoungGanondorfHorsebackLookSidewaysStartAnim, -5.0f);
                            this->state++;
                        }
                        break;
                    case 1:
                        if (animationEnded) {
                            Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gYoungGanondorfHorsebackLookSidewaysLoopAnim,
                                                  -5.0f);
                            this->state++;
                        }
                        break;
                    case 2:
                        if (play->csCtx.actorCues[1]->id == 5) {
                            Skeleton_Info2_init_standard_stop_morf(&this->skin.skelAnime,
                                                      &gYoungGanondorfHorsebackMagicChargeUpStartAnim, -5.0f);
                            this->state++;
                        }
                        break;
                    case 3:
                        if (animationEnded) {
                            Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gYoungGanondorfHorsebackMagicChargeUpLoopAnim,
                                                  -5.0f);
                            this->state++;
                        }
                        break;
                    case 4:
                        if (play->csCtx.actorCues[1]->id == 11) {
                            Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gYoungGanondorfHorsebackLookSidewaysLoopAnim,
                                                  -20.0f);
                            this->state++;
                        }
                        break;
                    case 5:
                        if (play->csCtx.actorCues[1]->id == 8) {
                            Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gYoungGanondorfHorsebackIdleAnim, -15.0f);
                            this->state++;
                        }
                        break;
                    case 6:
                        if (play->csCtx.actorCues[1]->id == 12) {
                            Actor_SE_set(&this->actor, NA_SE_EN_GANON_VOICE_DEMO);
                            Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &gYoungGanondorfHorsebackRideAnim, 3.0f);
                            this->state++;
                        }
                        break;
                    case 7:
                        this->state = 0;
                        break;
                }
            } else if (this->skin.skelAnime.animation != &gHorseGanonGallopingAnim &&
                       play->csCtx.actorCues[1]->id == 12) {
                Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &gHorseGanonGallopingAnim, 3.0f);
            }
        }
    } else if (type == ENVIEWER_TYPE_1_IMPA) {
        if (z_common_data.sceneLayer == 5) {
            if (play->csCtx.curFrame == 845) {
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_ITEM_OCARINA, 4.0f, 81.0f, 2600.0f, 0, 0,
                                   0, 0);
            }
        } else {
            if (play->csCtx.curFrame == 195) {
                Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_ITEM_OCARINA, 4.0f, 81.0f, 2035.0f, 0, 0,
                                   0, 1);
            }
        }
        switch (this->state) {
            case 0:
                if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.actorCues[0] != NULL &&
                    play->csCtx.actorCues[0]->id == 6 &&
                    this->skin.skelAnime.animation != &object_opening_demo1_Anim_002574) {
                    Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &object_opening_demo1_Anim_002574, 1.5f);
                    this->state++;
                }
                break;
            case 1:
                if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.actorCues[0] != NULL &&
                    play->csCtx.actorCues[0]->id == 2 &&
                    this->skin.skelAnime.animation != &object_opening_demo1_Anim_0029CC) {
                    Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &object_opening_demo1_Anim_0029CC, 3.0f);
                    this->state++;
                }
                break;
        }
    } else if (type == ENVIEWER_TYPE_2_ZELDA) {
        if (play->sceneId == SCENE_HYRULE_FIELD) {
            switch (this->state) {
                case 0:
                    if (play->csCtx.state != CS_STATE_IDLE) {
                        if (play->csCtx.actorCues[0] != NULL && play->csCtx.actorCues[0]->id == 6 &&
                            this->skin.skelAnime.animation != &object_opening_demo1_Anim_001410) {
                            Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &object_opening_demo1_Anim_001410, 1.5f);
                            this->state++;
                        }
                    }
                    break;
                case 1:
                    if (play->csCtx.state != CS_STATE_IDLE) {
                        if (play->csCtx.actorCues[0] != NULL && play->csCtx.actorCues[0]->id == 2 &&
                            this->skin.skelAnime.animation != &object_opening_demo1_Anim_000450) {
                            Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &object_opening_demo1_Anim_000450, 3.0f);
                            this->state++;
                        }
                    }
                    break;
            }
        } else {
            Na_SetDirectFir(0);
            switch (this->state) {
                case 0:
                    Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &object_opening_demo1_Anim_00504C, 1.0f);
                    this->state++;
                    break;
                case 1:
                    if (play->csCtx.actorCues[0]->id == 11) {
                        Skeleton_Info2_init_standard_stop_morf(&this->skin.skelAnime, &object_opening_demo1_Anim_00420C, -5.0f);
                        this->state++;
                    }
                    break;
                case 2:
                    if (animationEnded) {
                        Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &object_opening_demo1_Anim_0048FC, -5.0f);
                        this->state++;
                    }
                    break;
                case 3:
                    break;
            }
        }
    } else if (type == ENVIEWER_TYPE_7_GANONDORF) {
        switch (this->state) {
            case 0:
                if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[1] != NULL) &&
                    (play->csCtx.actorCues[1]->id == 7)) {
                    Nai_FxFlagEntry(NA_SE_EN_GANON_LAUGH, &_dummy_zero_f, 4, &_dummy_one,
                                         &_dummy_one, &_dummy_zero_s8);
                    Skeleton_Info2_init_standard_stop_morf(&this->skin.skelAnime, &gYoungGanondorfLaughStartAnim, -5.0f);
                    this->state++;
                }
                break;
            case 1:
                if (animationEnded) {
                    Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gYoungGanondorfLaughLoopAnim, -5.0f);
                    this->state++;
                }
                break;
        }
    } else if (type == ENVIEWER_TYPE_8_GANONDORF) {
        switch (this->state) {
            case 0:
                if (play->csCtx.state != CS_STATE_IDLE) {
                    if ((play->csCtx.actorCues[1] != NULL) && (play->csCtx.actorCues[1]->id == 9)) {
                        Skeleton_Info2_init_standard_speedset_repeat(&this->skin.skelAnime, &gYoungGanondorfWalkAnim, 1.0f);
                        this->state++;
                    }
                }
                break;
            case 1:
                if (play->csCtx.actorCues[1]->id == 10) {
                    Skeleton_Info2_init_standard_stop_morf(&this->skin.skelAnime, &gYoungGanondorfKneelStartAnim, -10.0f);
                    this->state++;
                }
                break;
            case 2:
                if (animationEnded) {
                    Skeleton_Info2_init_standard_repeat_morf(&this->skin.skelAnime, &gYoungGanondorfKneelLoopAnim, -5.0f);
                    this->state++;
                }
                break;
            case 3:
                if (play->csCtx.actorCues[1]->id == 4) {
                    Skeleton_Info2_init_standard_stop_morf(&this->skin.skelAnime, &gYoungGanondorfKneelLookSidewaysAnim, -5.0f);
                    this->state++;
                }
                break;
            default:
                this->state = 0;
                break;
        }
    }
}

void En_Viewer_actor_move(Actor* thisx, PlayState* play) {
    EnViewer* this = (EnViewer*)thisx;

    SegmentBaseAddress[6] = VIRTUAL_TO_PHYSICAL(play->objectCtx.slots[this->animObjectSlot].segment);
    this->actionFunc(this, play);
}

s32 before_gndd_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                        void* thisx) {
    if (z_common_data.sceneLayer == 4) {
        if (play->csCtx.curFrame >= 400) {
            if (limbIndex == YOUNG_GANONDORF_LIMB_LEFT_HAND) {
                *dList = gYoungGanondorfOpenLeftHandDL;
            }
        }
    } else {
        if ((play->csCtx.curFrame >= 1510) && (play->csCtx.curFrame <= 1650)) {
            if (limbIndex == YOUNG_GANONDORF_LIMB_LEFT_HAND) {
                *dList = gYoungGanondorfOpenLeftHandDL;
            }
        }
    }
    return false;
}

void Ganon_draw_sub2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    if (limbIndex == GANONDORF_LIMB_JEWEL) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1365);
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_viewer.c", 1370);
        gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_VIRTUAL(gGanondorfEyesDL));
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1372);
    }
}

void after_gndd_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f head_p = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == YOUNG_GANONDORF_LIMB_HEAD) {
        Matrix_Position(&head_p, &gndd_head_pos);
    }
}

void skeleton_shape_draw(EnViewer* this, PlayState* play) {
    s16 frames = 0;
    s16 type;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1405);

    type = PARAMS_GET_NOMASK(this->actor.params, 8);

    if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF || type == ENVIEWER_TYPE_7_GANONDORF ||
        type == ENVIEWER_TYPE_8_GANONDORF) {
        if (z_common_data.sceneLayer != 4) {
            frames = 149;
        }

        if (frames + 1127 >= play->csCtx.curFrame) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gYoungGanondorfEyeOpenTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gYoungGanondorfEyeOpenTex));

        } else if (frames + 1128 >= play->csCtx.curFrame) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gYoungGanondorfEyeHalfTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gYoungGanondorfEyeHalfTex));

        } else if (frames + 1129 >= play->csCtx.curFrame) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gYoungGanondorfEyeClosedTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gYoungGanondorfEyeClosedTex));

        } else {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gYoungGanondorfEyeLookingDownTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gYoungGanondorfEyeLookingDownTex));
        }
    } else if (type == ENVIEWER_TYPE_9_GANONDORF) {
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gGanondorfCrazedEyeTex));
    }

    if (type == ENVIEWER_TYPE_9_GANONDORF) {
        Si2_draw_SV(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable,
                              this->skin.skelAnime.dListCount, NULL, Ganon_draw_sub2, this);
    } else if (type == ENVIEWER_TYPE_3_GANONDORF) {
        Si2_draw_SV(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable,
                              this->skin.skelAnime.dListCount, before_gndd_draw,
                              after_gndd_draw, this);
        mant_test(play, this);
    } else if (type == ENVIEWER_TYPE_3_GANONDORF || type == ENVIEWER_TYPE_5_GANONDORF ||
               type == ENVIEWER_TYPE_7_GANONDORF || type == ENVIEWER_TYPE_8_GANONDORF) {
        if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[1] != NULL)) {
            Si2_draw_SV(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable,
                                  this->skin.skelAnime.dListCount, NULL, after_gndd_draw,
                                  this);
            mant_test(play, this);
        }
    } else {
        Si2_draw(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable, NULL, NULL, this);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1511);
}

void skin_shape_draw(EnViewer* this, PlayState* play) {
    Skin_disp2(&this->actor, play, &this->skin, NULL, true);
}

s32 before_zelda_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    if (play->sceneId == SCENE_HYRULE_FIELD) {
        if (limbIndex == 2) {
            *dList = gChildZeldaCutsceneDressDL;
        }
        if (limbIndex == 7) {
            *dList = NULL;
        }
        if (limbIndex == 8) {
            *dList = NULL;
        }
        if (limbIndex == 9) {
            *dList = NULL;
        }
        if (limbIndex == 3) {
            *dList = NULL;
        }
        if (limbIndex == 5) {
            *dList = NULL;
        }
    }
    return false;
}

void after_zelda_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    s32 pad;

    if (play->sceneId == SCENE_TEMPLE_OF_TIME) {
        if (limbIndex == 16) {
            OPEN_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1568);
            gSPDisplayList(POLY_OPA_DISP++, gChildZeldaOcarinaOfTimeDL);
            CLOSE_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1570);
        }
    }
}

void skeleton_shape_zelda_draw(EnViewer* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1583);
    if (play->sceneId == SCENE_HYRULE_FIELD) {
        if (play->csCtx.curFrame < 771) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeInTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeOutTex));
        } else if (play->csCtx.curFrame < 772) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
        } else if (play->csCtx.curFrame < 773) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        } else if (play->csCtx.curFrame < 791) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeWideTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeWideTex));
        } else if (play->csCtx.curFrame < 792) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeBlinkTex));
        } else if (play->csCtx.curFrame < 793) {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        } else {
            gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeInTex));
            gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeOutTex));
        }

        if (z_common_data.sceneLayer == 6) {
            gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthSurprisedTex));
        } else {
            if (play->csCtx.curFrame < 758) {
                gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthWorriedTex));
            } else if (play->csCtx.curFrame < 848) {
                gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthSurprisedTex));
            } else {
                gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthWorriedTex));
            }
        }
    } else {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gChildZeldaEyeShutTex));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(gChildZeldaMouthWorriedTex));
    }
    Si2_draw_SV(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable,
                          this->skin.skelAnime.dListCount, before_zelda_draw, after_zelda_draw,
                          this);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1690);
}

s32 before_inpa_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    if (limbIndex == 16) {
        *dList = gImpaHeadMaskedDL;
    }
    return false;
}

void skeleton_shape_inpa_draw(EnViewer* this, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1717);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gImpaEyeOpenTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gImpaEyeOpenTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);
    Si2_draw_SV(play, this->skin.skelAnime.skeleton, this->skin.skelAnime.jointTable,
                          this->skin.skelAnime.dListCount, before_inpa_draw, NULL, this);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1740);
}

static EnViewerDrawFunc shape_draw[] = {
    skeleton_shape_draw,
    skin_shape_draw,
    skeleton_shape_zelda_draw,
    skeleton_shape_inpa_draw,
};

void En_Viewer_actor_draw(Actor* thisx, PlayState* play) {
    EnViewer* this = (EnViewer*)thisx;
    s32 pad;
    s16 type;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1760);
    if (this->isVisible) {
        type = PARAMS_GET_NOMASK(this->actor.params, 8);
        if (type <= ENVIEWER_TYPE_2_ZELDA) { // zelda's horse, impa and zelda
            if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.actorCues[0] != NULL) {
                _texture_z_light_fog_prim(play->state.gfxCtx);
                shape_draw[this->drawFuncIndex](this, play);
            }
        } else if ((play->csCtx.state != CS_STATE_IDLE && play->csCtx.actorCues[1] != NULL) ||
                   type == ENVIEWER_TYPE_9_GANONDORF) {
            _texture_z_light_fog_prim(play->state.gfxCtx);
            shape_draw[this->drawFuncIndex](this, play);
        }
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1784);
}

void DemoNpcMoveProc(EnViewer* this, PlayState* play) {
    Vec3f startPos;
    Vec3f endPos;
    f32 lerpFactor;
    s16 type = PARAMS_GET_NOMASK(this->actor.params, 8);

    if (type <= ENVIEWER_TYPE_2_ZELDA) { // zelda's horse, impa and zelda
        if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.actorCues[0] != NULL &&
            play->csCtx.curFrame < play->csCtx.actorCues[0]->endFrame) {
            if (type == ENVIEWER_TYPE_0_HORSE_ZELDA) {
                if (!GOUIN) {
                    GOUIN = true;
                    Nai_FxFlagEntry(NA_SE_EV_HORSE_NEIGH, &this->actor.projectedPos, 4,
                                         &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                }
                Actor_SE_set(&this->actor, NA_SE_EV_HORSE_RUN_LEVEL - SFX_FLAG);
            }

            startPos.x = play->csCtx.actorCues[0]->startPos.x;
            startPos.y = play->csCtx.actorCues[0]->startPos.y;
            startPos.z = play->csCtx.actorCues[0]->startPos.z;
            endPos.x = play->csCtx.actorCues[0]->endPos.x;
            endPos.y = play->csCtx.actorCues[0]->endPos.y;
            endPos.z = play->csCtx.actorCues[0]->endPos.z;
            lerpFactor = get_parcent(play->csCtx.actorCues[0]->endFrame,
                                                play->csCtx.actorCues[0]->startFrame, play->csCtx.curFrame);
            this->actor.world.pos.x = (endPos.x - startPos.x) * lerpFactor + startPos.x;
            this->actor.world.pos.y = (endPos.y - startPos.y) * lerpFactor + startPos.y;
            this->actor.world.pos.z = (endPos.z - startPos.z) * lerpFactor + startPos.z;
        }
    } else { // ganondorf and ganondorf's horse
        if (play->csCtx.state != CS_STATE_IDLE && play->csCtx.actorCues[1] != NULL &&
            play->csCtx.curFrame < play->csCtx.actorCues[1]->endFrame) {
            startPos.x = play->csCtx.actorCues[1]->startPos.x;
            startPos.y = play->csCtx.actorCues[1]->startPos.y;
            startPos.z = play->csCtx.actorCues[1]->startPos.z;
            endPos.x = play->csCtx.actorCues[1]->endPos.x;
            endPos.y = play->csCtx.actorCues[1]->endPos.y;
            endPos.z = play->csCtx.actorCues[1]->endPos.z;
            lerpFactor = get_parcent(play->csCtx.actorCues[1]->endFrame,
                                                play->csCtx.actorCues[1]->startFrame, play->csCtx.curFrame);
            this->actor.world.pos.x = (endPos.x - startPos.x) * lerpFactor + startPos.x;
            this->actor.world.pos.y = (endPos.y - startPos.y) * lerpFactor + startPos.y;
            this->actor.world.pos.z = (endPos.z - startPos.z) * lerpFactor + startPos.z;

            if (play->csCtx.actorCues[1]->id == 12) {
                s16 yaw = search_position_angleY(&startPos, &endPos);

                add_calc_short_angle2(&this->actor.world.rot.y, yaw, 0xA, 0x3E8, 1);
                add_calc_short_angle2(&this->actor.shape.rot.y, yaw, 0xA, 0x3E8, 1);
            }

            if (type == ENVIEWER_TYPE_9_GANONDORF) {
                this->actor.world.rot.x = play->csCtx.actorCues[1]->rot.x;
                this->actor.world.rot.y = play->csCtx.actorCues[1]->rot.y;
                this->actor.world.rot.z = play->csCtx.actorCues[1]->rot.z;
                this->actor.shape.rot.x = play->csCtx.actorCues[1]->rot.x;
                this->actor.shape.rot.y = play->csCtx.actorCues[1]->rot.y;
                this->actor.shape.rot.z = play->csCtx.actorCues[1]->rot.z;
            }
        }
        if (type == ENVIEWER_TYPE_5_GANONDORF) {
            Nai_FxFlagEntry(NA_SE_EV_BURNING - SFX_FLAG, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            Demo_GndHonoo_draw(this, play);
        }
    }
}

void Demo_GndHonoo_init(EnViewer* this, PlayState* play, s16 i) {
    EnViewerFireEffect* eff;

    if ((i % 2) == 0) {
        eff = &this->fireEffects[i];
        eff->startPos.x = 100.0f;
        eff->startPos.y = -420.0f;
        eff->startPos.z = 400.0f;
        eff->endPos.x = 100.0f;
        eff->endPos.y = -420.0f;
        eff->endPos.z = -400.0f;
        eff->scale = (fqrand() * 5.0f + 12.0f) * 0.001f;
    } else {
        eff = &this->fireEffects[i];
        eff->startPos.x = -100.0f;
        eff->startPos.y = -420.0f;
        eff->startPos.z = 400.0f;
        eff->endPos.x = -100.0f;
        eff->endPos.y = -420.0f;
        eff->endPos.z = -400.0f;
        eff->scale = (fqrand() * 5.0f + 12.0f) * 0.001f;
    }
    if (this) {}
}

void Demo_GndHonoo_draw(EnViewer* this2, PlayState* play) {
    EnViewer* this = this2;
    s16 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 1941);
    for (i = 0; i < ARRAY_COUNT(this->fireEffects); i++) {
        switch (this->fireEffects[i].state) {
            case 0:
                Demo_GndHonoo_init(this, play, i);
                this->fireEffects[i].lerpFactor = (i >> 1) * 0.1f;
                this->fireEffects[i].lerpFactorSpeed = 0.01f;
                this->fireEffects[i].state++;
                break;
            case 1:
                add_calc(&this->fireEffects[i].lerpFactor, 1.0f, 1.0f, this->fireEffects[i].lerpFactorSpeed,
                                   this->fireEffects[i].lerpFactorSpeed);
                this->fireEffects[i].pos.x =
                    this->fireEffects[i].startPos.x +
                    (this->fireEffects[i].endPos.x - this->fireEffects[i].startPos.x) * this->fireEffects[i].lerpFactor;
                this->fireEffects[i].pos.y =
                    this->fireEffects[i].startPos.y +
                    (this->fireEffects[i].endPos.y - this->fireEffects[i].startPos.y) * this->fireEffects[i].lerpFactor;
                this->fireEffects[i].pos.z =
                    this->fireEffects[i].startPos.z +
                    (this->fireEffects[i].endPos.z - this->fireEffects[i].startPos.z) * this->fireEffects[i].lerpFactor;
                if (this->fireEffects[i].lerpFactor >= 1.0f) {
                    this->fireEffects[i].state++;
                }
                break;
            case 2:
                Demo_GndHonoo_init(this, play, i);
                this->fireEffects[i].lerpFactor = 0.0f;
                this->fireEffects[i].lerpFactorSpeed = 0.01f;
                this->fireEffects[i].state--;
                break;
        }

        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        Matrix_translate(this->fireEffects[i].pos.x, this->fireEffects[i].pos.y, this->fireEffects[i].pos.z,
                         MTXMODE_NEW);
        Matrix_scale(this->fireEffects[i].scale, this->fireEffects[i].scale, this->fireEffects[i].scale, MTXMODE_APPLY);
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0,
                                    (10 * i - 20 * play->state.frames) % 512, 32, 128));
        gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 170, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 00, 255);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_viewer.c", 2027);
        gSPMatrix(POLY_XLU_DISP++, &D_01000000, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_viewer.c", 2034);
}

void mant_test(PlayState* play, EnViewer* this) {
    static s16 ct = 0;
    Vec3f forearmModelOffset;
    Vec3f forearmWorldOffset;

    if (PARAMS_GET_NOMASK(this->actor.params, 8) != ENVIEWER_TYPE_5_GANONDORF) {
        return;
    }

    mant->backPush = BREG(54) / 10.0f;
    mant->backSwayMagnitude = (BREG(60) + 25) / 100.0f;
    mant->sideSwayMagnitude = (BREG(55) - 45) / 10.0f;
    mant->minY = -10000.0f;
    mant->minDist = 0.0f;
    mant->gravity = (BREG(67) - 10) / 10.0f;
    forearmModelOffset.x = KREG(16) - 13.0f;
    forearmModelOffset.y = KREG(17) + 3.0f + sin_s(ct) * KREG(20);
    forearmModelOffset.z = KREG(18) - 10.0f;
    ct += KREG(19) * 0x1000 + 0x2000;

    Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y), MTXMODE_NEW);
    Matrix_Position(&forearmModelOffset, &forearmWorldOffset);
    mant->rightForearmPos.x = gndd_head_pos.x + forearmWorldOffset.x;
    mant->rightForearmPos.y = gndd_head_pos.y + forearmWorldOffset.y;
    mant->rightForearmPos.z = gndd_head_pos.z + forearmWorldOffset.z;
    forearmModelOffset.x = -(KREG(16) - 13.0f);
    Matrix_Position(&forearmModelOffset, &forearmWorldOffset);
    mant->leftForearmPos.x = gndd_head_pos.x + forearmWorldOffset.x;
    mant->leftForearmPos.y = gndd_head_pos.y + forearmWorldOffset.y;
    mant->leftForearmPos.z = gndd_head_pos.z + forearmWorldOffset.z;
}
