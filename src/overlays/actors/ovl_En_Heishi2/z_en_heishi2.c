/*
 * File: z_en_heishi2.c
 * Overlay: ovl_En_Heishi2
 * Description: Hyrulian Guards
 */

#include "terminal.h"
#include "z_en_heishi2.h"
#include "assets/objects/object_sd/object_sd.h"
#include "assets/objects/object_link_child/object_link_child.h"
#include "overlays/actors/ovl_Bg_Gate_Shutter/z_bg_gate_shutter.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "overlays/actors/ovl_Bg_Spot15_Saku/z_bg_spot15_saku.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Heishi2_actor_ct(Actor* thisx, PlayState* play);
void En_Heishi2_actor_dt(Actor* thisx, PlayState* play);
void En_Heishi2_actor_move(Actor* thisx, PlayState* play);
void En_Heishi2_actor_draw(Actor* thisx, PlayState* play2);

void En_Heishi2_actor_draw2(Actor* thisx, PlayState* play);
void demo_hei_init(EnHeishi2* this, PlayState* play);
static void mode_no_move(EnHeishi2* this, PlayState* play);
void mode_gaisenmon_demo_init(EnHeishi2* this, PlayState* play);
void mode_gai_demo_message_select(EnHeishi2* this, PlayState* play);
void mode_gmon_select_check(EnHeishi2* this, PlayState* play);
void mode_message_end_init(EnHeishi2* this, PlayState* play);
void mode_gmon_demo_wait(EnHeishi2* this, PlayState* play);
void mode_gmon_d_open_init(EnHeishi2* this, PlayState* play);
void mode_ano_demo_message_select(EnHeishi2* this, PlayState* play);
void mode_gmon_d_open(EnHeishi2* this, PlayState* play);
void mode_gmon_d_after_init(EnHeishi2* this, PlayState* play);
static void mode_initial_mes_wait(EnHeishi2* this, PlayState* play);
void mode_another_demo(EnHeishi2* this, PlayState* play);
void mode_ano_letter_up(EnHeishi2* this, PlayState* play);
void mode_ano_demo_open_init(EnHeishi2* this, PlayState* play);
void mode_ano_demo_open(EnHeishi2* this, PlayState* play);
void mode_ukkari_up(EnHeishi2* this, PlayState* play);
void mode_another_demo_init(EnHeishi2* this, PlayState* play);
void mode_ano_camera_demo(EnHeishi2* this, PlayState* play);
void mode_ano_camera_ende(EnHeishi2* this, PlayState* play);
void mode_mask_ende(EnHeishi2* this, PlayState* play);
void mode_message_end(EnHeishi2* this, PlayState* play);
void mode_gmon_d_after(EnHeishi2* this, PlayState* play);
void mode_mask_out_init(EnHeishi2* this, PlayState* play);
void mode_mask_out(EnHeishi2* this, PlayState* play);
void mode_ukkari_mes(EnHeishi2* this, PlayState* play);
void mode_ukkari_down(EnHeishi2* this, PlayState* play);
void mode_mask_thanks(EnHeishi2* this, PlayState* play);
void mode_ano_camera_demo_init(EnHeishi2* this, PlayState* play);

ActorProfile En_Heishi2_Profile = {
    /**/ ACTOR_EN_HEISHI2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SD,
    /**/ sizeof(EnHeishi2),
    /**/ En_Heishi2_actor_ct,
    /**/ En_Heishi2_actor_dt,
    /**/ En_Heishi2_actor_move,
    /**/ En_Heishi2_actor_draw,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 33, 40, 0, { 0, 0, 0 } },
};

void En_Heishi2_actor_ct(Actor* thisx, PlayState* play) {
    ColliderCylinder* collider;
    EnHeishi2* this = (EnHeishi2*)thisx;

    Actor_set_scale(&this->actor, 0.01f);
    this->type = PARAMS_GET_U(this->actor.params, 0, 8);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    if ((this->type == 6) || (this->type == 9)) {
        this->actor.draw = En_Heishi2_actor_draw2;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_PROP);
        if (this->type == 6) {
            this->actionFunc = demo_hei_init;

        } else {
            PRINTF("\n\n");
            // "No, I'm completely disappointed" (message for when shooting guard window in courtyard)
            PRINTF(VT_FGCOL(MAGENTA) " ☆☆☆☆☆ いやー ついうっかり ☆☆☆☆☆ \n" VT_RST);

            Actor_set_scale(&this->actor, 0.02f);

            this->unk_274 = this->actor.world.pos;
            this->actor.world.rot.y = 0x7918;
            this->actor.world.pos.x += 90.0f;
            this->actor.world.pos.y -= 60.0f;
            this->actor.world.pos.z += 90.0f;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            ClObjPipe_dt(play, &this->collider);
            player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->actionFunc = mode_ukkari_up;
        }
    } else {
        this->unk_2E0 = 60.0f;
        Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
        Skeleton_Info2_M_ct(play, &this->skelAnime, &gEnHeishiSkel, &gEnHeishiIdleAnim, this->jointTable, this->morphTable,
                       17);
        collider = &this->collider;
        ClObjPipe_ct(play, collider);
        ClObjPipe_set5(play, collider, &this->actor, &OcInfoData);
        this->collider.dim.yShift = 0;
        this->collider.dim.radius = 15;
        this->collider.dim.height = 70;
        this->actor.attentionRangeType = ATTENTION_RANGE_6;

        switch (this->type) {

            case 2:
                this->actionFunc = mode_gaisenmon_demo_init;
                this->actor.gravity = -1.0f;
                break;
            case 5:
                this->actionFunc = mode_another_demo_init;
                this->actor.gravity = -1.0f;
                break;
            case 6:
                PRINTF("\n\n");
                // "Peep hole soldier!"
                PRINTF(VT_FGCOL(GREEN) " ☆☆☆☆☆ 覗き穴奥兵士ふぃ〜 ☆☆☆☆☆ \n" VT_RST);
                ClObjPipe_dt(play, collider);
                this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
                this->actionFunc = mode_no_move;
                break;
        }

        this->unk_2F0 = PARAMS_GET_U(this->actor.params, 8, 8);
        PRINTF("\n\n");
        // "Soldier Set 2 Completed!"
        PRINTF(VT_FGCOL(GREEN) " ☆☆☆☆☆ 兵士２セット完了！ ☆☆☆☆☆ %d\n" VT_RST, this->actor.params);
        // "Identification Completed!"
        PRINTF(VT_FGCOL(YELLOW) " ☆☆☆☆☆ 識別完了！         ☆☆☆☆☆ %d\n" VT_RST, this->type);
        // "Message completed!"
        PRINTF(VT_FGCOL(MAGENTA) " ☆☆☆☆☆ メッセージ完了！   ☆☆☆☆☆ %x\n\n" VT_RST,
               PARAMS_GET_U(this->actor.params, 8, 4));
    }
}

void En_Heishi2_actor_dt(Actor* thisx, PlayState* play) {
    EnHeishi2* this = (EnHeishi2*)thisx;

    if ((this->collider.dim.radius != 0) || (this->collider.dim.height != 0)) {
        ClObjPipe_dt(play, &this->collider);
    }
}

void demo_hei_init(EnHeishi2* this, PlayState* play) {
}

static void mode_no_move(EnHeishi2* this, PlayState* play) {
}

void mode_gaisenmon_demo_init(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gEnHeishiIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_gai_demo_message_select;
}

void mode_gai_demo_message_select(EnHeishi2* this, PlayState* play) {
    this->unk_30B = 0;
    this->unk_30E = 0;
    if (get_mask_message(play, MASK_REACTION_SET_HYRULIAN_GUARD) != 0) {
        this->actor.textId = get_mask_message(play, MASK_REACTION_SET_HYRULIAN_GUARD);
        this->unk_30B = 1;
        this->unk_300 = TEXT_STATE_DONE;
        this->actionFunc = mode_initial_mes_wait;
    } else if (GET_EVENTCHKINF(EVENTCHKINF_09) && GET_EVENTCHKINF(EVENTCHKINF_25) && GET_EVENTCHKINF(EVENTCHKINF_37)) {
        // "Get all spiritual stones!"
        PRINTF(VT_FGCOL(GREEN) " ☆☆☆☆☆ 全部の精霊石GET！ ☆☆☆☆☆ \n" VT_RST);
        this->unk_300 = TEXT_STATE_DONE;
        this->actor.textId = 0x7006;
        this->actionFunc = mode_initial_mes_wait;
    } else if (!IS_DAY) {
        // "Sleep early for children!"
        PRINTF(VT_FGCOL(YELLOW) " ☆☆☆☆☆ 子供ははやくネロ！ ☆☆☆☆☆ \n" VT_RST);
        this->unk_300 = TEXT_STATE_DONE;
        this->actor.textId = 0x7002;
        this->actionFunc = mode_initial_mes_wait;

    } else if (this->unk_30C != 0) {
        // "Anything passes"
        PRINTF(VT_FGCOL(BLUE) " ☆☆☆☆☆ なんでも通るよ ☆☆☆☆☆ \n" VT_RST);
        this->unk_300 = TEXT_STATE_DONE;
        this->actor.textId = 0x7099;
        this->actionFunc = mode_initial_mes_wait;
    } else if (GET_EVENTCHKINF(EVENTCHKINF_RECEIVED_WEIRD_EGG)) {
        if (this->unk_30E == 0) {
            // "Start under the first sleeve!"
            PRINTF(VT_FGCOL(MAGENTA) " ☆☆☆☆☆ １回目袖の下開始！ ☆☆☆☆☆ \n" VT_RST);
            this->actor.textId = 0x7071;
            this->unk_30E = 1;
        } else {
            // "Start under the second sleeve!"
            PRINTF(VT_FGCOL(MAGENTA) " ☆☆☆☆☆ ２回目袖の下開始！ ☆☆☆☆☆ \n" VT_RST);
            this->actor.textId = 0x7072;
        }
        this->unk_300 = TEXT_STATE_CHOICE;
        this->actionFunc = mode_initial_mes_wait;

    } else {
        // "That's okay"
        PRINTF(VT_FGCOL(CYAN) " ☆☆☆☆☆ それはとおらんよぉ ☆☆☆☆☆ \n" VT_RST);
        this->unk_300 = TEXT_STATE_DONE;
        this->actor.textId = 0x7029;
        this->actionFunc = mode_initial_mes_wait;
    }
}

void mode_gmon_select_check(EnHeishi2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        this->unk_300 = TEXT_STATE_EVENT;
        switch (play->msgCtx.choiceIndex) {
            case 0:
                if (z_common_data.save.info.playerData.rupees >= 10) {
                    lupy_increase(-10);
                    this->actor.textId = 0x7098;
                    this->actionFunc = mode_gmon_demo_wait;
                    break;
                } else {
                    this->actor.textId = 0x7097;
                    this->actionFunc = mode_message_end_init;
                    break;
                }
            case 1:
                this->actor.textId = 0x7096;
                this->actionFunc = mode_message_end_init;
                break;

            default:
                break;
        }
        message_set2(play, this->actor.textId);
    }
}

void mode_gmon_demo_wait(EnHeishi2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_300 == message_check(&play->msgCtx) && pad_on_check(play)) {
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        this->actionFunc = mode_gmon_d_open_init;
    }
}

void mode_gmon_d_open_init(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gEnHeishiSlamSpearAnim);

    this->unk_2EC = frameCount;
    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiSlamSpearAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = mode_gmon_d_open;
}

void mode_gmon_d_open(EnHeishi2* this, PlayState* play) {
    s32 pad;
    f32 frameCount = this->skelAnime.curFrame;

    BgSpot15Saku* actor = (BgSpot15Saku*)play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((frameCount >= 12.0f) && (!this->audioFlag)) {
        Actor_SE_set(&this->actor, NA_SE_EV_SPEAR_HIT);
        this->audioFlag = 1;
    }
    if (this->unk_2EC <= frameCount) {
        while (actor != NULL) {
            if (actor->dyna.actor.id != ACTOR_BG_SPOT15_SAKU) {
                actor = (BgSpot15Saku*)(actor->dyna.actor.next);
            } else {
                this->gate = &actor->dyna.actor;
                actor->unk_168 = 1;
                break;
            }
        }
        // "I've come!"
        PRINTF(VT_FGCOL(MAGENTA) "☆☆☆ きたきたきたぁ！ ☆☆☆ %x\n" VT_RST, actor->dyna.actor.next);
        this->actionFunc = mode_gmon_d_after_init;
    }
}

void mode_gmon_d_after_init(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gEnHeishiIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->unk_2F2[0] = 200;
    this->subCamId = Gama_play_make_camera(play);
    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
    Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
    this->subCamEye.x = 947.0f;
    this->subCamEye.y = 1195.0f;
    this->subCamEye.z = 2682.0f;

    this->subCamAt.x = 1164.0f;
    this->subCamAt.y = 1145.0f;
    this->subCamAt.z = 3014.0f;

    Gama_play_camera_setting(play, this->subCamId, &this->subCamEye, &this->subCamAt);
    this->actionFunc = mode_gmon_d_after;
}

void mode_gmon_d_after(EnHeishi2* this, PlayState* play) {
    BgSpot15Saku* gate;

    Skeleton_Info2_anime_play(&this->skelAnime);
    Gama_play_camera_setting(play, this->subCamId, &this->subCamEye, &this->subCamAt);
    gate = (BgSpot15Saku*)this->gate;
    if ((this->unk_2F2[0] == 0) || (gate->unk_168 == 0)) {
        Gama_play_clear_camera(play, this->subCamId);
        Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_ACTIVE);
        message_close(play);
        this->unk_30C = 1;
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        this->actionFunc = mode_gaisenmon_demo_init;
    }
}

void mode_another_demo_init(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gEnHeishiIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_ano_demo_message_select;
}

void mode_ano_demo_message_select(EnHeishi2* this, PlayState* play) {
    s16 var;

    this->unk_30B = 0;
    var = 0;
    if (GET_INFTABLE(INFTABLE_76)) {
        if (!GET_INFTABLE(INFTABLE_77)) {
            if (mask_check(play) == PLAYER_MASK_KEATON) {
                if (this->unk_309 == 0) {
                    this->actor.textId = 0x200A;
                } else {
                    this->actor.textId = 0x200B;
                }
                this->unk_300 = TEXT_STATE_CHOICE;
                this->unk_30B = 1;
                var = 1;
            } else {
                this->actor.textId = 0x2016;
                this->unk_300 = TEXT_STATE_DONE;
                var = 1;
            }
        } else {
            this->actor.textId = 0x2020;
            this->unk_300 = TEXT_STATE_EVENT;
            this->unk_30E = 0;
        }
        if (get_mask_message(play, MASK_REACTION_SET_HYRULIAN_GUARD) != 0) {
            if (var == 0) {
                this->actor.textId = get_mask_message(play, MASK_REACTION_SET_HYRULIAN_GUARD);
                this->unk_30B = 1;
                this->unk_300 = TEXT_STATE_DONE;
                this->unk_30E = 0;
            }
        }
        this->actionFunc = mode_initial_mes_wait;
    } else {
        // "I don't know"
        PRINTF(VT_FGCOL(MAGENTA) " ☆☆☆☆☆ とおしゃしねぇちゅーの ☆☆☆☆☆ \n" VT_RST);
        this->actionFunc = mode_another_demo;
    }
}

void mode_another_demo(EnHeishi2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiffTemp;
    s16 yawDiff;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (get_mask_message(play, MASK_REACTION_SET_HYRULIAN_GUARD) != 0) {
        this->actor.textId = get_mask_message(play, MASK_REACTION_SET_HYRULIAN_GUARD);
    } else {
        this->actor.textId = 0x200E;
    }
    this->unk_300 = TEXT_STATE_DONE;

    if (Actor_talk_check(&this->actor, play)) {
        s32 exchangeItemId = Actor_get_item_check(play);

        if (exchangeItemId == EXCH_ITEM_ZELDAS_LETTER) {
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
            player->actor.textId = 0x2010;
            this->unk_300 = TEXT_STATE_EVENT;
            this->actionFunc = mode_ano_letter_up;
        } else if (exchangeItemId != EXCH_ITEM_NONE) {
            player->actor.textId = 0x200F;
        }
        return;
    }

    yawDiffTemp = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    yawDiff = ABS(yawDiffTemp);

    if (!(120.0f < this->actor.xzDistToPlayer) && (yawDiff < 0x4300)) {
        Actor_talk_request_get_item(&this->actor, play, 100.0f, EXCH_ITEM_ZELDAS_LETTER);
    }
}

void mode_ano_letter_up(EnHeishi2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((this->unk_300 == message_check(&play->msgCtx)) && pad_on_check(play)) {
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        this->actionFunc = mode_ano_demo_open_init;
    }
}

void mode_ano_demo_open_init(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gEnHeishiSlamSpearAnim);

    this->unk_2EC = frameCount;
    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiSlamSpearAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = mode_ano_demo_open;
}

void mode_ano_demo_open(EnHeishi2* this, PlayState* play) {
    s32 pad;
    f32 frameCount;
    BgGateShutter* gate;

    frameCount = this->skelAnime.curFrame;
    gate = (BgGateShutter*)play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (12.0f <= frameCount) {
        if (this->audioFlag == 0) {
            Actor_SE_set(&this->actor, NA_SE_EV_SPEAR_HIT);
            this->audioFlag = 1;
        }
    }
    if (this->unk_2EC <= frameCount) {
        while (gate != NULL) {
            if (gate->dyna.actor.id != ACTOR_BG_GATE_SHUTTER) {
                gate = (BgGateShutter*)gate->dyna.actor.next;
            } else {
                this->gate = &gate->dyna.actor;
                gate->openingState = 1;
                break;
            }
        }
        // "I've come!"
        PRINTF(VT_FGCOL(MAGENTA) "☆☆☆ きたきたきたぁ！ ☆☆☆ %x\n" VT_RST, gate->dyna.actor.next);
        this->actionFunc = mode_ano_camera_demo_init;
    }
}

void mode_ano_camera_demo_init(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gEnHeishiIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->unk_2F2[0] = 200;
    this->subCamId = Gama_play_make_camera(play);
    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
    Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
    this->subCamEyeInit.x = -71.0f;
    this->subCamEye.x = -71.0f;
    this->subCamEyeInit.y = 571.0f;
    this->subCamEye.y = 571.0f;
    this->subCamEyeInit.z = -1487.0f;
    this->subCamEye.z = -1487.0f;
    this->subCamAtInit.x = 181.0f;
    this->subCamAt.x = 181.0f;
    this->subCamAtInit.y = 417.0f;
    this->subCamAt.y = 417.0f;
    this->subCamAtInit.z = -1079.0f;
    this->subCamAt.z = -1079.0f;
    Gama_play_camera_setting(play, this->subCamId, &this->subCamEye, &this->subCamAt);
    this->actionFunc = mode_ano_camera_demo;
}

void mode_ano_camera_demo(EnHeishi2* this, PlayState* play) {
    BgGateShutter* gate;

    Skeleton_Info2_anime_play(&this->skelAnime);
    Gama_play_camera_setting(play, this->subCamId, &this->subCamEye, &this->subCamAt);
    gate = (BgGateShutter*)this->gate;
    if ((this->unk_2F2[0] == 0) || (gate->openingState == 0)) {
        Gama_play_clear_camera(play, this->subCamId);
        Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_ACTIVE);
        if (this->unk_30A != 2) {
            if (this->unk_30A == 0) {
                this->actor.textId = 0x2015;
                message_set2(play, this->actor.textId);
                this->actionFunc = mode_ano_camera_ende;
            } else {
                message_close(play);
                player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
                this->actionFunc = mode_another_demo_init;
            }
        } else {
            this->unk_30E = 0;
            this->actor.textId = 0x2021;
            lupy_increase(15);
            message_set2(play, this->actor.textId);
            this->actionFunc = mode_mask_ende;
        }
    }
}

void mode_ano_camera_ende(EnHeishi2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        SET_INFTABLE(INFTABLE_76);
        message_close(play);
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        this->actionFunc = mode_another_demo_init;
    }
}

void mode_mask_check(EnHeishi2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                this->actor.textId = 0x2020;
                message_set2(play, this->actor.textId);
                mask_cancel(play);
                SET_INFTABLE(INFTABLE_77);
                SET_ITEMGETINF(ITEMGETINF_38);
                item_get_setting(play, ITEM_SOLD_OUT);
                if (this->unk_30A != 0) {
                    this->unk_30A = 2;
                    this->unk_30E = 1;
                    this->actionFunc = mode_mask_ende;
                } else {
                    this->unk_30E = 0;
                    this->actionFunc = mode_mask_thanks;
                }
                break;
            case 1:
                this->unk_30E = 1;
                this->actor.textId = 0x200C;
                message_set2(play, this->actor.textId);
                this->unk_300 = TEXT_STATE_EVENT;
                if (this->unk_30A == 0) {
                    this->actionFunc = mode_mask_ende;
                } else {
                    this->actionFunc = mode_message_end_init;
                }
        }
    }
}

void mode_mask_thanks(EnHeishi2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        this->actor.textId = 0x2021;
        lupy_increase(15);
        message_set2(play, this->actor.textId);
        this->actionFunc = mode_mask_ende;
    }
}

void mode_mask_ende(EnHeishi2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        if (this->unk_30E == 0) {
            this->unk_30E = 0;
            this->unk_30A = this->unk_30E;
            message_close(play);
            this->actionFunc = mode_another_demo_init;
        } else {
            play->msgCtx.msgMode = MSGMODE_PAUSED;
            this->actionFunc = mode_mask_out_init;
        }
    }
}

void mode_mask_out_init(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gEnHeishiSlamSpearAnim);

    this->unk_2EC = frameCount;
    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiSlamSpearAnim, 1.0f, 0.0f, frameCount, ANIMMODE_ONCE, -10.0f);
    this->audioFlag = 0;
    this->actionFunc = mode_mask_out;
}

void mode_mask_out(EnHeishi2* this, PlayState* play) {
    s32 pad;
    f32 frameCount = this->skelAnime.curFrame;
    BgGateShutter* gate = (BgGateShutter*)(play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((frameCount >= 12.0f) && (!this->audioFlag)) {
        Actor_SE_set(&this->actor, NA_SE_EV_SPEAR_HIT);
        this->audioFlag = 1;
    }

    if (this->unk_2EC <= frameCount) {
        while (gate != NULL) {
            if (ACTOR_BG_GATE_SHUTTER != gate->dyna.actor.id) {
                gate = (BgGateShutter*)(gate->dyna.actor.next);
            } else {
                this->gate = &gate->dyna.actor;
                if (this->unk_30A != 2) {
                    gate->openingState = -1;
                    break;
                } else {
                    gate->openingState = 2;
                    break;
                }
            }
        }
        if (this->unk_30A == 0) {
            this->unk_30A = 1;
        }
        this->actionFunc = mode_ano_camera_demo_init;
    }
}

void mode_ukkari_up(EnHeishi2* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.z, -6100, 5, this->unk_2E4, 0);
    add_calc2(&this->unk_2E4, 3000.0f, 1.0f, 500.0f);
    this->actor.world.rot.z = this->actor.shape.rot.z;
    if (this->actor.shape.rot.z < -6000) {
        message_set(play, 0x708F, NULL);
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = mode_ukkari_mes;
        this->unk_2E4 = 0.0f;
    }
}

void mode_ukkari_mes(EnHeishi2* this, PlayState* play) {
    s32 pad;
    Vec3f pos;
    f32 rotY;
    EnBom* bomb;

    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        message_close(play);

        pos.x = rnd_fx(20.0f) + this->unk_274.x;
        pos.y = rnd_fx(20.0f) + (this->unk_274.y - 40.0f);
        pos.z = rnd_fx(20.0f) + (this->unk_274.z - 20.0f);
        rotY = rnd_fx(7000.0f) + this->actor.yawTowardsPlayer;
        bomb = (EnBom*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOM, pos.x, pos.y, pos.z, 0, rotY, 0, 0);
        if (bomb != NULL) {
            bomb->actor.speed = rnd_fx(5.0f) + 10.0f;
            bomb->actor.velocity.y = rnd_fx(5.0f) + 10.0f;
        }

        // "This is down!"
        PRINTF(VT_FGCOL(YELLOW) " ☆☆☆☆☆ これでダウンだ！ ☆☆☆☆☆ \n" VT_RST);
        this->actionFunc = mode_ukkari_down;
    }
}

void mode_ukkari_down(EnHeishi2* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.shape.rot.z, 200, 5, this->unk_2E4, 0);
    add_calc2(&this->unk_2E4, 3000.0f, 1.0f, 500.0f);
    this->actor.world.rot.z = this->actor.shape.rot.z;
    if (this->actor.shape.rot.z > 0) {
        Actor_delete(&this->actor);
    }
}

static void mode_initial_mes_wait(EnHeishi2* this, PlayState* play) {
    s16 yawDiff;

    Skeleton_Info2_anime_play(&this->skelAnime);

    if (get_mask_message(play, MASK_REACTION_SET_HYRULIAN_GUARD) != 0) {
        if (this->unk_30B == 0) {
            if (this->type == 2) {
                this->actionFunc = mode_gai_demo_message_select;
                return;
            }
            if (this->type == 5) {
                this->actionFunc = mode_ano_demo_message_select;
                return;
            }
        }
    } else if (this->unk_30B != 0) {
        if (this->type == 2) {
            this->actionFunc = mode_gai_demo_message_select;
            return;
        }
        if (this->type == 5) {
            this->actionFunc = mode_ano_demo_message_select;
            return;
        }
    }

    if (Actor_talk_check(&this->actor, play)) {
        if (this->type == 2) {
            if (this->unk_30E == 1) {
                this->actionFunc = mode_gmon_select_check;
                return;
            } else {
                this->actionFunc = mode_gai_demo_message_select;
                return;
            }
        } else if (this->type == 5) {
            if (this->unk_300 == TEXT_STATE_DONE) {
                this->actionFunc = mode_ano_demo_message_select;
            }

            if (this->unk_300 == TEXT_STATE_EVENT) {
                this->actionFunc = mode_message_end_init;
            }

            if (this->unk_300 == TEXT_STATE_CHOICE) {
                this->unk_309 = 1;
                Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
                this->actionFunc = mode_mask_check;
            }
            return;
        }
    }

    if (((this->type != 2) && (this->type != 5)) ||
        ((yawDiff = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)),
          !(this->actor.xzDistToPlayer > 120.0f)) &&
         (yawDiff < 0x4300))) {
        Actor_talk_request(&this->actor, play);
    }
}

void mode_message_end_init(EnHeishi2* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gEnHeishiIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_message_end;
}

void mode_message_end(EnHeishi2* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_300 == message_check(&play->msgCtx) && pad_on_check(play)) {
        message_close(play);
        if (this->type == 2) {
            this->actionFunc = mode_gaisenmon_demo_init;
        }
        if (this->type == 5) {
            this->actionFunc = mode_another_demo_init;
        }
    }
}

void En_Heishi2_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHeishi2* this = (EnHeishi2*)thisx;
    s32 i;

    Actor_world_to_eye(&this->actor, this->unk_2E0);
    if ((this->type == 2) || (this->type == 5)) {
        this->actor.focus.pos.y = 70.0f;
        Actor_world_to_eye(&this->actor, 70.0f);
        eye_move2(play, &this->actor, &this->unk_260, &this->unk_26C, this->actor.focus.pos);
    }

    this->unk_2FC++;

    for (i = 0; i != 5; i++) {
        if (this->unk_2F2[i] != 0) {
            this->unk_2F2[i]--;
        }
    }
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    switch (this->type) {
        case 6:
            break;
        case 9:
            break;
        default:
            Actor_BGcheck2(play, &this->actor, 10.0f, 10.0f, 30.0f,
                                    UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                        UPDBGCHECKINFO_FLAG_4);
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
            break;
    }
}

s32 En_Heishi2_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHeishi2* this = (EnHeishi2*)thisx;

    switch (this->type) {
        case 1:
            break;
        case 7:
            break;
        default:
            if (limbIndex == 9) {
                rot->x += this->unk_26C.y;
            }
            if (limbIndex == 16) {
                rot->x += this->unk_260.y;
                rot->z += this->unk_260.z;
            }
    }

    return false;
}

void En_Heishi2_draw_after(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnHeishi2* this = (EnHeishi2*)thisx;

    if (limbIndex == 16) {
        Matrix_get(&this->mtxf_330);
    }
}

void En_Heishi2_actor_draw2(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_en_heishi2.c", 1772);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_heishi2.c", 1774);
    gSPDisplayList(POLY_OPA_DISP++, gHeishiKingGuardDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_heishi2.c", 1777);
}

void En_Heishi2_actor_draw(Actor* thisx, PlayState* play2) {
    PlayState* play = (PlayState*)play2;
    EnHeishi2* this = (EnHeishi2*)thisx;
    s32 linkChildObjectSlot;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_heishi2.c", 1792);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, En_Heishi2_draw_sub,
                      En_Heishi2_draw_after, this);
    if ((this->type == 5) && GET_INFTABLE(INFTABLE_77)) {
        linkChildObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_LINK_CHILD);
        if (linkChildObjectSlot >= 0) {
            Mtx* mtx;

            Matrix_put(&this->mtxf_330);
            Matrix_translate(-570.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            Matrix_rotateZ(DEG_TO_RAD(70), MTXMODE_APPLY);
            mtx = MATRIX_FINALIZE(play->state.gfxCtx, "../z_en_heishi2.c", 1820) - 7;

            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[linkChildObjectSlot].segment);
            gSPSegment(POLY_OPA_DISP++, 0x0D, mtx);
            gSPDisplayList(POLY_OPA_DISP++, gLinkChildKeatonMaskDL);
            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->actor.objectSlot].segment);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_heishi2.c", 1834);
}
