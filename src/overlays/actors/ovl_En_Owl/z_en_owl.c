/*
 * File: z_en_owl.c
 * Overlay: ovl_En_Owl
 * Description: Owl
 */

#include "z_en_owl.h"
#include "assets/objects/object_owl/object_owl.h"
#include "assets/scenes/overworld/spot06/spot06_scene.h"
#include "assets/scenes/overworld/spot16/spot16_scene.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Owl_Actor_ct(Actor* thisx, PlayState* play);
void En_Owl_Actor_dt(Actor* thisx, PlayState* play);
void En_Owl_Actor_move(Actor* thisx, PlayState* play);
void En_Owl_Actor_draw(Actor* thisx, PlayState* play);
void En_Owl_actor_set_process(EnOwl* this, EnOwlActionFunc, OwlFunc, SkelAnime*, AnimationHeader*, f32);
void owl_demo_start(EnOwl* this, PlayState* play);
static void normal_anime_proc(EnOwl* this);
void owl_01_start(EnOwl* this, PlayState* play);
void owl_02_start(EnOwl* this, PlayState* play);
void owl_03_start(EnOwl* this, PlayState* play);
void owl_04_start(EnOwl* this, PlayState* play);
void owl_05_start(EnOwl* this, PlayState* play);
void owl_06_start(EnOwl* this, PlayState* play);
void owl_07_start(EnOwl* this, PlayState* play);
void owl_08_start(EnOwl* this, PlayState* play);
void owl_10_start(EnOwl* this, PlayState* play);
void owl_11_start(EnOwl* this, PlayState* play);
void owl_12_start(EnOwl* this, PlayState* play);
void En_Owl_MoveProcess_rail(EnOwl* this, PlayState* play);
void En_Owl_StartProc_local(EnOwl* this, PlayState* play, s32 idx);
void owl_demo_dousa_set(EnOwl* this, PlayState* play);
void En_Owl_MoveProcess_circle(EnOwl* this, PlayState* play);
void owl_02_prc1(EnOwl* this, PlayState* play);
void owl_03_prc0(EnOwl* this, PlayState* play);
void owl_04_prc0(EnOwl* this, PlayState* play);
void owl_05_prc0(EnOwl* this, PlayState* play);
void warp_openwing(EnOwl* this, PlayState* play);
void owl_11_prc0(EnOwl* this, PlayState* play);
void owl_12_prc0(EnOwl* this, PlayState* play);
void habataki_anime_proc(EnOwl* this);
void move_openwing(EnOwl*, PlayState*);

typedef enum EnOwlType {
    /* 0x00 */ OWL_DEFAULT,
    /* 0x01 */ OWL_OUTSIDE_KOKIRI,
    /* 0x02 */ OWL_HYRULE_CASTLE,
    /* 0x03 */ OWL_KAKARIKO,
    /* 0x04 */ OWL_HYLIA_GERUDO,
    /* 0x05 */ OWL_LAKE_HYLIA,
    /* 0x06 */ OWL_ZORA_RIVER,
    /* 0x07 */ OWL_HYLIA_SHORTCUT,
    /* 0x08 */ OWL_DEATH_MOUNTAIN,
    /* 0x09 */ OWL_DEATH_MOUNTAIN2,
    /* 0x0A */ OWL_DESERT_COLOSSUS,
    /* 0x0B */ OWL_LOST_WOODS_PRESARIA,
    /* 0x0C */ OWL_LOST_WOODS_POSTSARIA
} EnOwlType;

typedef enum EnOwlMessageChoice {
    /* 0x00 */ OWL_REPEAT,
    /* 0x01 */ OWL_OK
} EnOwlMessageChoice;

ActorProfile En_Owl_Profile = {
    /**/ ACTOR_EN_OWL,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OWL,
    /**/ sizeof(EnOwl),
    /**/ En_Owl_Actor_ct,
    /**/ En_Owl_Actor_dt,
    /**/ En_Owl_Actor_move,
    /**/ En_Owl_Actor_draw,
};

static ColliderCylinderInit EnOwlOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
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
    { 30, 40, 0, { 0, 0, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 25, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2400, ICHAIN_STOP),
};

void En_Owl_Actor_ct(Actor* thisx, PlayState* play) {
    EnOwl* this = (EnOwl*)thisx;
    ColliderCylinder* collider;
    s32 owlType;
    s32 switchFlag;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0, Actor_shadow_circle, 36.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gOwlFlyingSkel, &gOwlFlyAnim, this->jointTable, this->morphTable, 21);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime2, &gOwlPerchingSkel, &gOwlPerchAnim, this->jointTable2, this->morphTable2,
                       16);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnOwlOcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.minVelocityY = -10.0f;
    this->actor.lockOnArrowOffset = 500.0f;
    En_Owl_actor_set_process(this, owl_demo_start, normal_anime_proc, &this->skelAnime2, &gOwlPerchAnim, 0.0f);
    this->actionFlags = this->unk_406 = this->unk_409 = 0;
    this->unk_405 = 4;
    this->unk_404 = this->unk_407 = 0;
    this->unk_408 = 4;
    owlType = PARAMS_GET_S(this->actor.params, 6, 6);
    switchFlag = PARAMS_GET_S(this->actor.params, 0, 6);
    if (this->actor.params == 0xFFF) {
        owlType = OWL_OUTSIDE_KOKIRI;
        switchFlag = 0x20;
    }
    PRINTF(VT_FGCOL(CYAN) T(" 会話フクロウ %4x no = %d, sv = %d\n", " conversation owl %4x no = %d, sv = %d\n") VT_RST,
           this->actor.params, owlType, switchFlag);

    if ((owlType != OWL_DEFAULT) && (switchFlag < 0x20) && Actor_Environment_sw_Check(play, switchFlag)) {
        PRINTF(T("savebitでフクロウ退避\n", "Save owl with savebit\n"));
        Actor_delete(&this->actor);
        return;
    }

    this->unk_3EE = 0;
    this->unk_400 = this->actor.world.rot.y;

    switch (owlType) {
        case OWL_DEFAULT:
            this->actionFunc = owl_demo_start;
            this->actor.cullingVolumeDistance = 4000.0f;
            this->unk_40A = 0;
            break;
        case OWL_OUTSIDE_KOKIRI:
            this->actionFunc = owl_01_start;
            break;
        case OWL_HYRULE_CASTLE:
            this->actionFlags |= 2;
            this->unk_3EE = 0x20;
            this->actionFunc = owl_02_start;
            break;
        case OWL_KAKARIKO:
            if (GET_EVENTCHKINF(EVENTCHKINF_40)) {
                // has zelda's letter
                PRINTF(T("フクロウ退避\n", "Owl evacuation\n"));
                Actor_delete(&this->actor);
                return;
            }

            this->actionFunc = owl_03_start;
            break;
        case OWL_HYLIA_GERUDO:
            if (GET_EVENTCHKINF(EVENTCHKINF_43)) {
                // has ocarina of time
                PRINTF(T("フクロウ退避\n", "Owl evacuation\n"));
                Actor_delete(&this->actor);
                return;
            }
            this->actionFunc = owl_04_start;
            break;
        case OWL_LAKE_HYLIA:
            this->actionFunc = owl_05_start;
            break;
        case OWL_ZORA_RIVER:
            if (GET_EVENTCHKINF(EVENTCHKINF_39) || !GET_EVENTCHKINF(EVENTCHKINF_40)) {
                // opened zora's domain or has zelda's letter
                PRINTF(T("フクロウ退避\n", "Owl evacuation\n"));
                Actor_delete(&this->actor);
                return;
            }

            this->actionFunc = owl_06_start;
            break;
        case OWL_HYLIA_SHORTCUT:
            this->actionFunc = owl_07_start;
            Actor_Environment_sw_Off(play, 0x23);
            return;
        case OWL_DEATH_MOUNTAIN:
            this->actionFunc = owl_08_start;
            break;
        case OWL_DEATH_MOUNTAIN2:
            this->actionFunc = owl_08_start;
            break;
        case OWL_DESERT_COLOSSUS:
            this->actionFunc = owl_10_start;
            break;
        case OWL_LOST_WOODS_PRESARIA:
            if (!CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
                PRINTF(T("フクロウ退避\n", "Owl evacuation\n"));
                Actor_delete(&this->actor);
                return;
            }
            this->actionFunc = owl_11_start;
            break;
        case OWL_LOST_WOODS_POSTSARIA:
            if (!CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) {
                PRINTF(T("フクロウ退避\n", "Owl evacuation\n"));
                Actor_delete(&this->actor);
                return;
            }
            this->actionFunc = owl_12_start;
            break;
        default:
            // Outside kokiri forest
            PRINTF_COLOR_CYAN();
            PRINTF("no = %d  \n", owlType);
            PRINTF(T("未完成のフクロウ未完成のフクロウ未完成のフクロウ\n",
                     "Unfinished owl unfinished owl unfinished owl\n"));
            PRINTF_RST();
            this->actionFlags |= 2;
            this->unk_3EE = 0x20;
            this->actionFunc = owl_01_start;
            break;
    }
}

void En_Owl_Actor_dt(Actor* thisx, PlayState* play) {
    EnOwl* this = (EnOwl*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

/**
 * Rotates this to the player instance
 */
void owl_set_angleY(EnOwl* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.shape.rot.y = this->actor.world.rot.y =
        search_position_angleY(&this->actor.world.pos, &player->actor.world.pos);
}

/**
 * Checks if link is within `targetDist` units, initialize the camera for the owl.
 * returns 0 if the link is not within `targetDistance`, returns 1 once link is within
 * the distance, and the camera has been initialized.
 */
static s32 kihon_process(EnOwl* this, PlayState* play, u16 textId, f32 targetDist, u16 flags) {
    s32 timer;
    f32 distCheck;

    if (Actor_talk_check(&this->actor, play)) {
        if (this->actor.params == 0xFFF) {
            this->actionFlags |= 0x40;
            timer = -100;
        } else {
            if (fqrand() < 0.5f) {
                timer = (flags & 1) ? -97 : -99;
                this->actionFlags |= 0x40;
            } else {
                timer = (flags & 1) ? -96 : -98;
                this->actionFlags &= ~0x40;
            }
        }
        this->subCamId = makeOnepointDemo(play, 8700, timer, &this->actor, CAM_ID_MAIN);
        return true;
    } else {
        this->actor.textId = textId;
        distCheck = (flags & 2) ? 200.0f : 1000.0f;
        if (this->actor.xzDistToPlayer < targetDist) {
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            Actor_talk_request_get_item_set(&this->actor, play, targetDist, distCheck, EXCH_ITEM_NONE);
        }
        return false;
    }
}

s32 kihon_process2(EnOwl* this, PlayState* play, u16 textId) {
    if (Actor_talk_check(&this->actor, play)) {
        return true;
    } else {
        this->actor.textId = textId;
        if (this->actor.xzDistToPlayer < 120.0f) {
            Actor_talk_request_get_item_set(&this->actor, play, 350.0f, 1000.0f, EXCH_ITEM_NONE);
        }
        return false;
    }
}

void to_fly_process(EnOwl* this) {
    En_Owl_actor_set_process(this, move_openwing, normal_anime_proc, &this->skelAnime, &gOwlUnfoldWingsAnim, 0.0f);
    this->eyeTexIndex = 0;
    this->blinkTimer = get_random_timer(60, 60);
}

void to_fly_process2(EnOwl* this, PlayState* play) {
    s32 switchFlag = PARAMS_GET_S(this->actor.params, 0, 6);

    if (switchFlag < 0x20) {
        Actor_Environment_sw_On(play, switchFlag);
        PRINTF(VT_FGCOL(CYAN) " Actor_Environment_sw = %d\n" VT_RST, Actor_Environment_sw_Check(play, switchFlag));
    }
    to_fly_process(this);
}

void owl_tofly_wait(EnOwl* this, PlayState* play) {
    if ((this->unk_3EE & 0x3F) == 0) {
        to_fly_process2(this, play);
    }
}

void set_head_rot(EnOwl* this) {
    if (rnd_fx(1.0f) < 0.0f) {
        this->actionFlags |= 0x20;
    } else {
        this->actionFlags &= ~0x20;
    }
}

void set_head_rot2(EnOwl* this) {
    set_head_rot(this);
    this->unk_3F2 = 0;
    this->actionFlags |= 0x10;
    this->unk_408 = 4;
    this->unk_404 = 0;
    this->unk_406 = 0;
    this->unk_405 = 4;
    this->unk_407 = this->unk_3F2;
}

void owl_end2(EnOwl* this, PlayState* play) {
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);

    if (Actor_talk_end_check(&this->actor, play)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
        to_fly_process2(this, play);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
}

void owl_end(EnOwl* this, PlayState* play) {
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);

    if (Actor_talk_end_check(&this->actor, play)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
        if ((this->unk_3EE & 0x3F) == 0) {
            to_fly_process2(this, play);
        } else {
            this->actionFlags &= ~2;
            set_head_rot2(this);
            this->actionFunc = owl_tofly_wait;
        }
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
}

void owl_01_prc0(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case OWL_REPEAT:
                message_set2(play, 0x2065);
                break;
            case OWL_OK:
                message_set2(play, 0x2067);
                this->actionFunc = owl_end2;
                break;
        }
    }
}

void owl_01_start(EnOwl* this, PlayState* play) {
    owl_set_angleY(this, play);

    if (kihon_process(this, play, 0x2064, 360.0f, 0)) {
        // Sets BGM
        Na_StartFanfare(NA_BGM_OWL);

        this->actionFunc = owl_01_prc0;
        // spoke to owl by lost woods
        SET_EVENTCHKINF(EVENTCHKINF_6F);
    }
}

void owl_02_prc2(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case OWL_REPEAT:
                message_set2(play, 0x2069);
                this->actionFunc = owl_02_prc1;
                break;
            case OWL_OK:
                message_set2(play, 0x206B);
                this->actionFunc = owl_end;
                break;
        }
        this->actionFlags &= ~2;
        set_head_rot2(this);
    }
}

void owl_02_prc1(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        message_set2(play, 0x206A);
        this->actionFunc = owl_02_prc2;
        this->actionFlags |= 2;
        set_head_rot2(this);
    }
}

void owl_02_prc0(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        message_set2(play, 0x2069);
        this->actionFunc = owl_02_prc1;
        this->actionFlags &= ~2;
        set_head_rot2(this);
    }
}

void owl_02_start(EnOwl* this, PlayState* play) {
    owl_set_angleY(this, play);

    if (kihon_process(this, play, 0x2068, 540.0f, 0)) {
        Na_StartFanfare(NA_BGM_OWL);
        this->actionFunc = owl_02_prc0;
    }
}

void owl_03_prc1(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case OWL_REPEAT:
                // obtained zelda's letter
                if (GET_EVENTCHKINF(EVENTCHKINF_40)) {
                    message_set2(play, 0x206D);
                } else {
                    message_set2(play, 0x206C);
                }
                this->actionFunc = owl_03_prc0;
                break;
            case OWL_OK:
                message_set2(play, 0x206E);
                this->actionFunc = owl_end;
                break;
        }

        this->actionFlags &= ~2;
        set_head_rot2(this);
    }
}

void owl_03_prc0(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        message_set2(play, 0x206A);
        this->actionFunc = owl_03_prc1;
        this->actionFlags |= 2;
        set_head_rot2(this);
    }
}

void owl_03_start(EnOwl* this, PlayState* play) {
    owl_set_angleY(this, play);

    if (kihon_process(this, play, 0x206C, 480.0f, 0)) {
        Na_StartFanfare(NA_BGM_OWL);
        this->actionFunc = owl_03_prc0;
    }
}

void owl_04_prc1(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case OWL_REPEAT:
                message_set2(play, 0x206F);
                this->actionFunc = owl_04_prc0;
                break;
            case OWL_OK:
                message_set2(play, 0x2070);
                this->actionFunc = owl_end;
                break;
        }

        this->actionFlags &= ~2;
        set_head_rot2(this);
    }
}

void owl_04_prc0(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        message_set2(play, 0x206A);
        this->actionFunc = owl_04_prc1;
        this->actionFlags |= 2;
        set_head_rot2(this);
    }
}

void owl_04_start(EnOwl* this, PlayState* play) {
    owl_set_angleY(this, play);

    if (kihon_process(this, play, 0x206F, 360.0f, 0)) {
        Na_StartFanfare(NA_BGM_OWL);
        this->actionFunc = owl_04_prc0;
    }
}

void owl_05_prc1(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case OWL_REPEAT:
                message_set2(play, 0x2071);
                this->actionFunc = owl_05_prc0;
                break;
            case OWL_OK:
                message_set2(play, 0x2072);
                this->actionFunc = owl_end;
                break;
        }

        this->actionFlags &= ~2;
        set_head_rot2(this);
    }
}

void owl_05_prc0(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        message_set2(play, 0x206A);
        this->actionFunc = owl_05_prc1;
        this->actionFlags |= 2;
        set_head_rot2(this);
    }
}

void owl_05_start(EnOwl* this, PlayState* play) {
    owl_set_angleY(this, play);

    if (kihon_process(this, play, 0x2071, 360.0f, 0)) {
        Na_StartFanfare(NA_BGM_OWL);
        this->actionFunc = owl_05_prc0;
    }
}

void owl_06_prc0(EnOwl* this, PlayState* play) {
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);

    if (Actor_talk_end_check(&this->actor, play)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
        to_fly_process2(this, play);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
}

void owl_06_start(EnOwl* this, PlayState* play) {
    u16 textId;

    owl_set_angleY(this, play);

    if (CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) {
        if (CHECK_QUEST_ITEM(QUEST_SONG_LULLABY)) {
            textId = 0x4031;
        } else {
            textId = 0x4017;
        }
    } else {
        textId = 0x4002;
    }

    if (kihon_process(this, play, textId, 360.0f, 0)) {
        Na_StartFanfare(NA_BGM_OWL);
        this->actionFunc = owl_06_prc0;
    }
}

void owl_07_prc0(EnOwl* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
        to_fly_process(this);
        this->actionFunc = warp_openwing;
        Actor_Environment_sw_On(play, 0x23);
    }
}

void owl_07_start(EnOwl* this, PlayState* play) {
    u16 textId = GET_INFTABLE(INFTABLE_195) ? 0x4004 : 0x4003;

    // Spoke to Owl in Lake Hylia
    owl_set_angleY(this, play);
    if (kihon_process2(this, play, textId)) {
        SET_INFTABLE(INFTABLE_195);
        Na_StartFanfare(NA_BGM_OWL);
        this->actionFunc = owl_07_prc0;
    }
}

void owl_08_prc0(EnOwl* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
        to_fly_process(this);
        this->actionFunc = warp_openwing;
    }
}

void owl_08_prc_retalk(EnOwl* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
        this->actionFunc = owl_08_start;
    }
}

void owl_08_start(EnOwl* this, PlayState* play) {
    owl_set_angleY(this, play);

    if (!z_common_data.save.info.playerData.isMagicAcquired) {
        if (kihon_process2(this, play, 0x3062)) {
            Na_StartFanfare(NA_BGM_OWL);
            this->actionFunc = owl_08_prc_retalk;
            return;
        }
    } else {
        if (kihon_process2(this, play, 0x3063)) {
            Na_StartFanfare(NA_BGM_OWL);
            this->actionFunc = owl_08_prc0;
        }
    }
}

void owl_10_prc0(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case OWL_REPEAT:
                message_set2(play, 0x607A);
                break;
            case OWL_OK:
                message_set2(play, 0x607C);
                this->actionFunc = owl_end;
                break;
        }
    }
}

void owl_10_start(EnOwl* this, PlayState* play) {
    owl_set_angleY(this, play);

    if (kihon_process(this, play, 0x6079, 360.0f, 2)) {
        Na_StartFanfare(NA_BGM_OWL);
        this->actionFunc = owl_10_prc0;
    }
}

void owl_11_prc1(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case OWL_REPEAT:
                message_set2(play, 0x10C1);
                this->actionFunc = owl_11_prc0;
                break;
            case OWL_OK:
                message_set2(play, 0x10C3);
                this->actionFunc = owl_end;
        }

        this->actionFlags &= ~2;
        set_head_rot2(this);
    }
}

void owl_11_prc0(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        message_set2(play, 0x10C2);
        this->actionFunc = owl_11_prc1;
        this->actionFlags |= 2;
        set_head_rot2(this);
    }
}

void owl_11_start(EnOwl* this, PlayState* play) {
    owl_set_angleY(this, play);

    if (kihon_process(this, play, 0x10C0, 190.0f, 0)) {
        Na_StartFanfare(NA_BGM_OWL);
        this->actionFunc = owl_11_prc0;
    }
}

void owl_12_prc1(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case OWL_REPEAT:
                message_set2(play, 0x10C5);
                this->actionFunc = owl_12_prc0;
                break;
            case OWL_OK:
                message_set2(play, 0x10C7);
                this->actionFunc = owl_end;
                break;
        }

        this->actionFlags &= ~2;
        set_head_rot2(this);
    }
}

void owl_12_prc0(EnOwl* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        message_set2(play, 0x10C6);
        this->actionFunc = owl_12_prc1;
        this->actionFlags |= 2;
        set_head_rot2(this);
    }
}

void owl_12_start(EnOwl* this, PlayState* play) {
    owl_set_angleY(this, play);

    if (kihon_process(this, play, 0x10C4, 360.0f, 0)) {
        Na_StartFanfare(NA_BGM_OWL);
        this->actionFunc = owl_12_prc0;
    }
}

void Owl_Demo_Sound(EnOwl* this, PlayState* play) {
    static Vec3f zero_pos = { 0.0f, 0.0f, 0.0f };
    f32 dist;
    f32 weight;
    s32 owlType = PARAMS_GET_S(this->actor.params, 6, 6);

    dist = Math3DLength(&this->eye, &play->view.eye) / 45.0f;
    this->eye.x = play->view.eye.x;
    this->eye.y = play->view.eye.y;
    this->eye.z = play->view.eye.z;

    weight = dist;
    if (weight > 1.0f) {
        weight = 1.0f;
    }

    switch (owlType) {
        case 7:
            Na_SetMotorSe(&zero_pos, NA_SE_EV_FLYING_AIR - SFX_FLAG, weight * 2.0f);
            if ((play->csCtx.curFrame > 324) || ((play->csCtx.curFrame >= 142 && (play->csCtx.curFrame <= 266)))) {
                Na_SetAutoTriggerSe(&zero_pos, NA_SE_EN_OWL_FLUTTER, weight * 2.0f);
            }
            if (play->csCtx.curFrame == 85) {
                Na_SetMotorSe(&zero_pos, NA_SE_EV_PASS_AIR, weight * 2.0f);
            }
            break;
        case 8:
        case 9:
            Na_SetMotorSe(&zero_pos, NA_SE_EV_FLYING_AIR - SFX_FLAG, weight * 2.0f);
            if ((play->csCtx.curFrame >= 420) || ((193 < play->csCtx.curFrame && (play->csCtx.curFrame <= 280)))) {
                Na_SetAutoTriggerSe(&zero_pos, NA_SE_EN_OWL_FLUTTER, weight * 2.0f);
            }
            if (play->csCtx.curFrame == 217) {
                Na_SetMotorSe(&zero_pos, NA_SE_EV_PASS_AIR, weight * 2.0f);
            }
            break;
    }
}

void owl_demo_circle(EnOwl* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE && (play->csCtx.actorCues[7] != NULL)) {
        if (this->unk_40A != play->csCtx.actorCues[7]->id) {
            En_Owl_StartProc_local(this, play, 7);
            owl_demo_dousa_set(this, play);
        }
        En_Owl_MoveProcess_circle(this, play);
    }

    if (this->actionFlags & 0x80) {
        Owl_Demo_Sound(this, play);
    }
}

void owl_demo_rail(EnOwl* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE && (play->csCtx.actorCues[7] != NULL)) {
        if (this->unk_40A != play->csCtx.actorCues[7]->id) {
            En_Owl_StartProc_local(this, play, 7);
            owl_demo_dousa_set(this, play);
        }
        En_Owl_MoveProcess_rail(this, play);
    }

    if (this->actionFlags & 0x80) {
        Owl_Demo_Sound(this, play);
    }
}

void owl_demo_start(EnOwl* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE && (play->csCtx.actorCues[7] != NULL)) {
        if (this->unk_40A != play->csCtx.actorCues[7]->id) {
            this->actionFlags |= 4;
            En_Owl_StartProc_local(this, play, 7);
            owl_demo_dousa_set(this, play);
        } else {
            this->actor.world.rot.z = play->csCtx.actorCues[7]->rot.y;
        }
    }

    if (this->actionFlags & 0x80) {
        Owl_Demo_Sound(this, play);
    }
}

void owl_demo_dousa_set(EnOwl* this, PlayState* play) {
    switch (play->csCtx.actorCues[7]->id) {
        case 1:
            En_Owl_actor_set_process(this, owl_demo_circle, normal_anime_proc, &this->skelAnime, &gOwlFlyAnim, 0.0f);
            break;
        case 2:
            this->actor.draw = En_Owl_Actor_draw;
            En_Owl_actor_set_process(this, owl_demo_start, normal_anime_proc, &this->skelAnime, &gOwlPerchAnim, 0.0f);
            break;
        case 3:
            this->actor.draw = En_Owl_Actor_draw;
            En_Owl_actor_set_process(this, owl_demo_rail, normal_anime_proc, &this->skelAnime, &gOwlFlyAnim, 0.0f);
            break;
        case 4:
            this->actor.draw = NULL;
            this->actionFunc = owl_demo_start;
            break;
        case 5:
            Actor_delete(&this->actor);
            break;
    }

    this->unk_40A = play->csCtx.actorCues[7]->id;
}

void move_fly(EnOwl* this, PlayState* play) {
    this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;

    if (this->actor.xzDistToPlayer > 6000.0f && !(this->actionFlags & 0x80)) {
        Actor_delete(&this->actor);
    }

    add_calc_short_angle2(&this->actor.world.rot.y, this->unk_400, 2, 0x80, 0x40);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->actor.speed < 16.0f) {
        this->actor.speed += 0.5f;
    }

    if ((this->unk_3F8 + 1000.0f) < this->actor.world.pos.y) {
        if (this->actor.velocity.y > 0.0f) {
            this->actor.velocity.y -= 0.4f;
        }
    } else if (this->actor.velocity.y < 4.0f) {
        this->actor.velocity.y += 0.2f;
    }

    this->actionFlags |= 8;
}

void move_hovering(EnOwl* this, PlayState* play) {
    if (this->skelAnime.curFrame > 10.0f) {
        add_calc_short_angle2(&this->actor.world.rot.y, this->unk_400, 2, 0x400, 0x40);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if (this->skelAnime.curFrame > 45.0f) {
        this->actor.velocity.y = 2.0f;
        this->actor.gravity = 0.0f;
        this->actor.speed = 8.0f;
    } else if (this->skelAnime.curFrame > 17.0f) {
        this->actor.velocity.y = 6.0f;
        this->actor.gravity = 0.0f;
        this->actor.speed = 4.0f;
    }

    if (this->actionFlags & 1) {
        En_Owl_actor_set_process(this, move_fly, habataki_anime_proc, &this->skelAnime, &gOwlFlyAnim, 0.0f);
        this->unk_3FE = 6;
        if (this->actionFlags & 0x40) {
            this->unk_400 += 0x2000;
        } else {
            this->unk_400 -= 0x2000;
        }
    }

    this->actionFlags |= 8;
}

void move_openwing(EnOwl* this, PlayState* play) {
    if (this->actionFlags & 1) {
        this->unk_3FE = 3;
        En_Owl_actor_set_process(this, move_hovering, normal_anime_proc, &this->skelAnime, &gOwlTakeoffAnim, 0.0f);
        this->unk_3F8 = this->actor.world.pos.y;
        this->actor.velocity.y = 2.0f;
        if (this->actionFlags & 0x40) {
            this->unk_400 = this->actor.world.rot.y + 0x4000;
        } else {
            this->unk_400 = this->actor.world.rot.y - 0x4000;
        }
    }

    this->actionFlags |= 8;
}

void warp_hovering3(EnOwl* this, PlayState* play) {
    add_calc_short_angle2(&this->actor.world.rot.y, this->unk_400, 2, 0x384, 0x258);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->actionFlags & 1) {
        En_Owl_actor_set_process(this, move_fly, habataki_anime_proc, &this->skelAnime, &gOwlFlyAnim, 0.0f);
        this->unk_3FE = 6;
        this->actor.velocity.y = 2.0f;
        this->actor.gravity = 0.0f;
        this->actor.speed = 4.0f;
    }
    this->actionFlags |= 8;
}

void warp_hovering2(EnOwl* this, PlayState* play) {
    s32 owlType;
    s32 temp_v0;
    s32 temp_v0_2;

    add_calc_short_angle2(&this->actor.world.rot.y, this->unk_400, 2, 0x384, 0x258);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (this->actor.xzDistToPlayer < 50.0f) {
        if (!Game_play_demo_mode_check(play)) {
            owlType = PARAMS_GET_S(this->actor.params, 6, 6);
            PRINTF_COLOR_CYAN();
            PRINTF(T("%dのフクロウ\n", "%d owl\n"), owlType);
            PRINTF_RST();
            switch (owlType) {
                case 7:
                    PRINTF_COLOR_CYAN();
                    PRINTF(T("SPOT 06 の デモがはしった\n", "Demo of SPOT 06 has been completed\n"));
                    PRINTF_RST();
                    play->csCtx.script = SEGMENTED_TO_VIRTUAL(gLakeHyliaOwlCs);
                    this->actor.draw = NULL;
                    break;
                case 8:
                case 9:
                    play->csCtx.script = SEGMENTED_TO_VIRTUAL(gDMTOwlCs);
                    this->actor.draw = NULL;
                    break;
                default:
                    ASSERT(0, "0", "../z_en_owl.c", 1693);
                    break;
            }

            Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
            z_common_data.cutsceneTrigger = 1;
            Na_InitAutoTriggerSe(0x14, 0xA);
            this->actionFunc = owl_demo_start;
            this->unk_40A = 0;
            this->actionFlags |= 0x80;
            E_day_time_plus = 0;
        }
    }

    if (this->skelAnime.curFrame >= 37.0f) {
        if (this->unk_3FE > 0) {
            this->skelAnime.curFrame = 21.0f;
            this->unk_3FE--;
        } else {
            this->actionFunc = warp_hovering3;
        }
    }

    this->actionFlags |= 8;
}

void warp_hovering(EnOwl* this, PlayState* play) {
    if (this->skelAnime.curFrame < 20.0f) {
        this->actor.speed = 1.5f;
    } else {
        this->actor.speed = 0.0f;
        add_calc_short_angle2(&this->actor.world.rot.y, this->unk_400, 2, 0x384, 0x258);
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if (this->skelAnime.curFrame >= 37.0f) {
        this->skelAnime.curFrame = 21.0f;
        this->actionFunc = warp_hovering2;
        this->unk_3FE = 5;
        this->actor.velocity.y = 0.0f;
        this->actor.gravity = 0.0f;
        this->actor.speed = 0.0f;
    }

    this->actionFlags |= 8;
}

void warp_openwing(EnOwl* this, PlayState* play) {
    if (this->actionFlags & 1) {
        this->unk_3FE = 3;
        En_Owl_actor_set_process(this, warp_hovering, normal_anime_proc, &this->skelAnime, &gOwlTakeoffAnim, 0.0f);
        this->unk_3F8 = this->actor.world.pos.y;
        this->actor.velocity.y = 0.2f;
    }

    this->actionFlags |= 8;
}

void kakuu_anime_proc(EnOwl* this) {
    Skeleton_Info2_anime_play(this->curSkelAnime);

    if (this->unk_3FE > 0) {
        this->unk_3FE--;
        this->actor.shape.rot.z = sin_s(this->unk_3FE * 0x333) * 1000.0f;
    } else {
        this->unk_410 = habataki_anime_proc;
        this->unk_3FE = 6;
        Skeleton_Info2_init(this->curSkelAnime, &gOwlFlyAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gOwlFlyAnim), 2, 5.0f);
    }
}

void habataki_anime_proc(EnOwl* this) {
    if (Skeleton_Info2_anime_play(this->curSkelAnime)) {
        if (this->unk_3FE > 0) {
            this->unk_3FE--;
            Skeleton_Info2_init(this->curSkelAnime, this->curSkelAnime->animation, 1.0f, 0.0f,
                             Si2_anime_end_frame(this->curSkelAnime->animation), ANIMMODE_ONCE, 0.0f);
        } else {
            this->unk_3FE = 0xA0;
            this->unk_410 = kakuu_anime_proc;
            Skeleton_Info2_init(this->curSkelAnime, &gOwlGlideAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gOwlGlideAnim), 0,
                             5.0f);
        }
    }
}

static void normal_anime_proc(EnOwl* this) {
    if (Skeleton_Info2_anime_play(this->curSkelAnime)) {
        Skeleton_Info2_init(this->curSkelAnime, this->curSkelAnime->animation, 1.0f, 0.0f,
                         Si2_anime_end_frame(this->curSkelAnime->animation), ANIMMODE_ONCE, 0.0f);
        this->actionFlags |= 1;
    } else {
        this->actionFlags &= ~1;
    }
}

s32 kyoro_process(EnOwl* this) {
    s32 phi_v1 = (this->actionFlags & 2) ? 0x20 : 0;

    if (phi_v1 == (this->unk_3EE & 0x3F)) {
        return true;
    } else {
        if (this->actionFlags & 0x20) {
            this->unk_3EE += 4;
        } else {
            this->unk_3EE -= 4;
        }

        return false;
    }
}

s32 sound_ok_owl(EnOwl* this, PlayState* play) {
    s32 switchFlag = PARAMS_GET_S(this->actor.params, 6, 6);

    if (play->sceneId != SCENE_DESERT_COLOSSUS) {
        return true;
    } else if (switchFlag == 0xA) {
        return true;
    } else if (play->csCtx.curFrame >= 300 && play->csCtx.curFrame <= 430) {
        return true;
    } else if (play->csCtx.curFrame >= 1080 && play->csCtx.curFrame <= 1170) {
        return true;
    } else {
        return false;
    }
}

void En_Owl_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnOwl* this = (EnOwl*)thisx;
    s16 phi_a1;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_BGcheck2(play, &this->actor, 10.0f, 10.0f, 10.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
    this->unk_410(this);
    this->actionFlags &= ~8;
    this->actionFunc(this, play);
    if (this->actor.update == NULL) {
        PRINTF(T("フクロウ消滅!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",
                 "Owl disappears!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
        return;
    }

    if (!(this->actionFlags & 0x80) && sound_ok_owl(this, play)) {
        if ((this->skelAnime.animation == &gOwlTakeoffAnim &&
             (this->skelAnime.curFrame == 2.0f || this->skelAnime.curFrame == 9.0f ||
              this->skelAnime.curFrame == 23.0f || this->skelAnime.curFrame == 40.0f ||
              this->skelAnime.curFrame == 58.0f)) ||
            (this->skelAnime.animation == &gOwlFlyAnim && this->skelAnime.curFrame == 4.0f)) {
            Actor_SE_set(&this->actor, NA_SE_EN_OWL_FLUTTER);
        }
    }

    if (this->actor.draw != NULL) {
        Actor_position_moveF(&this->actor);
    }

    if (this->actionFlags & 2) {
        this->eyeTexIndex = 2;
    } else {
        if (DECR(this->blinkTimer) == 0) {
            this->blinkTimer = get_random_timer(60, 60);
        }

        this->eyeTexIndex = this->blinkTimer;

        if (this->eyeTexIndex >= 3) {
            this->eyeTexIndex = 0;
        }
    }

    if (!(this->actionFlags & 8)) {
        phi_a1 = 0;
        if (this->actionFlags & 0x10) {
            switch (this->unk_404) {
                case 0:
                    this->unk_404 = 1;
                    this->unk_405 = 6;
                    break;
                case 1:
                    this->unk_405--;

                    if (this->unk_405 != 0) {
                        phi_a1 = cos_s(this->unk_405 * 8192) * 4096.0f;
                    } else {
                        if (this->actionFlags & 2) {
                            this->unk_3EE = 0;
                        } else {
                            this->unk_3EE = 0x20;
                        }

                        if (this->actionFlags & 0x20) {
                            this->unk_3EE -= 4;
                        } else {
                            this->unk_3EE += 4;
                        }
                        this->unk_404++;
                    }

                    if (this->actionFlags & 0x20) {
                        phi_a1 = -phi_a1;
                    }
                    break;
                case 2:
                    if (kyoro_process(this)) {
                        this->actionFlags &= ~0x10;
                        this->unk_406 = (s32)rnd_f(20.0f) + 0x3C;
                        this->unk_404 = 0;
                        set_head_rot(this);
                    }
                    break;
                default:
                    break;
            }
        } else {
            if (this->unk_406 > 0) {
                this->unk_406--;
            } else {
                if (this->unk_404 == 0) {
                    if (fqrand() < 0.3f) {
                        this->unk_404 = 4;
                        this->unk_405 = 0xC;
                    } else {
                        this->unk_404 = 1;
                        this->unk_405 = 4;
                    }
                }

                this->unk_405--;

                switch (this->unk_404) {
                    case 1:
                        phi_a1 = sin_s((-this->unk_405 * 4096) + 0x4000) * 5000.0f;
                        if (this->unk_405 <= 0) {
                            this->unk_405 = (s32)(rnd_f(15.0f) + 5.0f);
                            this->unk_404 = 2;
                        }
                        break;
                    case 2:
                        phi_a1 = 0x1388;
                        if (this->unk_405 <= 0) {
                            this->unk_404 = 3;
                            this->unk_405 = 4;
                        }
                        break;
                    case 3:
                        phi_a1 = sin_s(this->unk_405 * 4096) * 5000.0f;
                        if (this->unk_405 <= 0) {
                            this->unk_406 = (s32)rnd_f(20.0f) + 0x3C;
                            this->unk_404 = 0;
                            set_head_rot(this);
                        }
                        break;
                    case 4:
                        phi_a1 = sin_s(this->unk_405 * 8192) * 5000.0f;
                        if (this->unk_405 <= 0) {
                            this->unk_406 = (s32)rnd_f(20.0f) + 0x3C;
                            this->unk_404 = 0;
                            set_head_rot(this);
                        }
                        break;
                    default:
                        break;
                }

                if (this->actionFlags & 0x20) {
                    phi_a1 = -phi_a1;
                }
            }

            if (this->unk_409 > 0) {
                this->unk_409--;
            } else {
                this->unk_408--;
                switch (this->unk_407) {
                    case 0:
                        this->unk_3F2 = (-this->unk_408 * 0x5DC) + 0x1770;
                        if (this->unk_408 <= 0) {
                            this->unk_407 = 1;
                            this->unk_408 = (s32)(rnd_f(15.0f) + 5.0f);
                        }
                        break;
                    case 1:
                        this->unk_3F2 = 0x1770;
                        if (this->unk_408 <= 0) {
                            this->unk_407 = 2;
                            this->unk_408 = 4;
                        }
                        break;
                    case 2:
                        this->unk_3F2 = this->unk_408 * 0x5DC;
                        if (this->unk_408 <= 0) {
                            this->unk_407 = 0;
                            this->unk_408 = 4;
                            this->unk_409 = (s32)rnd_f(40.0f) + 0xA0;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        if (phi_a1) {}
        this->unk_3F0 = (u16)((this->unk_3EE << 2) << 8) + phi_a1;
        this->unk_3EC = ABS(this->unk_3F0) >> 3;
    } else {
        this->unk_3F2 = 0;
        if (this->actionFlags & 2) {
            this->unk_3F0 = -0x8000;
        } else {
            this->unk_3F0 = 0;
        }

        this->unk_3EC = ABS(this->unk_3F0) >> 3;
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** gfx, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnOwl* this = (EnOwl*)thisx;

    switch (limbIndex) {
        case 3:
            rot->x += this->unk_3F0;
            rot->z += this->unk_3EC;
            rot->z -= this->unk_3F2;
            break;
        case 2:
            rot->z += this->unk_3F2;
            break;
        case 4:
            if (!(this->actionFlags & 8)) {
                rot->y -= (s16)(this->unk_3EC * 1.5f);
            }
            break;
        case 5:
            if (!(this->actionFlags & 8)) {
                rot->y += (s16)(this->unk_3EC * 1.5f);
            }
            break;
        default:
            break;
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** gfx, Vec3s* rot, void* thisx) {
    EnOwl* this = (EnOwl*)thisx;
    Vec3f vec;

    vec.z = 0.0f;
    if (this->actionFlags & 2) {
        vec.x = 700.0f;
        vec.y = 400.0f;
    } else {
        vec.y = 0.0f;
        vec.x = 1400.0f;
    }
    if (limbIndex == 3) {
        Matrix_Position(&vec, &this->actor.focus.pos);
    }
}

void En_Owl_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = { gObjOwlEyeOpenTex, gObjOwlEyeHalfTex, gObjOwlEyeClosedTex };
    EnOwl* this = (EnOwl*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_owl.c", 2247);

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 8, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTexIndex]));
    Si2_draw_SV(play, this->curSkelAnime->skeleton, this->curSkelAnime->jointTable,
                          this->curSkelAnime->dListCount, before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_owl.c", 2264);
}

void En_Owl_actor_set_process(EnOwl* this, EnOwlActionFunc actionFunc, OwlFunc arg2, SkelAnime* skelAnime,
                      AnimationHeader* animation, f32 morphFrames) {
    this->curSkelAnime = skelAnime;
    Skeleton_Info2_init(this->curSkelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_ONCE,
                     morphFrames);
    this->actionFunc = actionFunc;
    this->unk_410 = arg2;
}

void En_Owl_StartProc_local(EnOwl* this, PlayState* play, s32 idx) {
    Vec3f startPos;

    startPos.x = play->csCtx.actorCues[idx]->startPos.x;
    startPos.y = play->csCtx.actorCues[idx]->startPos.y;
    startPos.z = play->csCtx.actorCues[idx]->startPos.z;
    this->actor.world.pos = startPos;
    this->actor.world.rot.y = this->actor.shape.rot.y = play->csCtx.actorCues[idx]->rot.y;
    this->actor.shape.rot.z = play->csCtx.actorCues[idx]->rot.z;
}

static f32 get_demo_effect_parcent(PlayState* play, s32 idx) {
    f32 ret = get_parcent(play->csCtx.actorCues[idx]->endFrame, play->csCtx.actorCues[idx]->startFrame,
                                     play->csCtx.curFrame);

    ret = CLAMP_MAX(ret, 1.0f);
    return ret;
}

void En_Owl_MoveProcess_basic(EnOwl* this, Vec3f* arg1, f32 arg2) {
    Vec3f rpy;

    rpy.x = (arg1->x - this->actor.world.pos.x) * arg2;
    rpy.y = (arg1->y - this->actor.world.pos.y) * arg2;
    rpy.z = (arg1->z - this->actor.world.pos.z) * arg2;

    chase_f(&this->actor.velocity.y, rpy.y, 1.0f);
    this->actor.speed = sqrtf(SQ(rpy.x) + SQ(rpy.z));
    this->actor.world.rot.y = search_position_angleY(&this->actor.world.pos, arg1);
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void En_Owl_MoveProcess_circle(EnOwl* this, PlayState* play) {
    Vec3f pos;
    s32 angle;
    f32 t = get_demo_effect_parcent(play, 7);
    f32 phi_f2;

    pos.x = play->csCtx.actorCues[7]->startPos.x;
    pos.y = play->csCtx.actorCues[7]->startPos.y;
    pos.z = play->csCtx.actorCues[7]->startPos.z;
    angle = (s16)play->csCtx.actorCues[7]->rot.y - this->actor.world.rot.z;
    if (angle < 0) {
        angle += 0x10000;
    }
    angle = (s16)((t * angle) + this->actor.world.rot.z);
    angle = (u16)angle;
    if (this->actionFlags & 4) {
        phi_f2 = play->csCtx.actorCues[7]->rot.x;
        phi_f2 *= 10.0f * (360.0f / 0x10000);
        if (phi_f2 < 0.0f) {
            phi_f2 += 360.0f;
        }
        pos.x -= sin_s(angle) * phi_f2;
        pos.z += cos_s(angle) * phi_f2;
        this->unk_3F8 = phi_f2;
        this->actor.world.pos = pos;
        this->actor.draw = En_Owl_Actor_draw;
        this->actionFlags &= ~4;
        this->actor.speed = 0.0f;
    } else {
        pos.x -= sin_s(angle) * this->unk_3F8;
        pos.z += cos_s(angle) * this->unk_3F8;
        En_Owl_MoveProcess_basic(this, &pos, 1.0f);
    }
}

void En_Owl_MoveProcess_rail(EnOwl* this, PlayState* play) {
    Vec3f pos;
    Vec3f endPosf;
    f32 temp_ret = get_demo_effect_parcent(play, 7);

    pos.x = play->csCtx.actorCues[7]->startPos.x;
    pos.y = play->csCtx.actorCues[7]->startPos.y;
    pos.z = play->csCtx.actorCues[7]->startPos.z;
    endPosf.x = play->csCtx.actorCues[7]->endPos.x;
    endPosf.y = play->csCtx.actorCues[7]->endPos.y;
    endPosf.z = play->csCtx.actorCues[7]->endPos.z;
    pos.x = (endPosf.x - pos.x) * temp_ret + pos.x;
    pos.y = (endPosf.y - pos.y) * temp_ret + pos.y;
    pos.z = (endPosf.z - pos.z) * temp_ret + pos.z;
    En_Owl_MoveProcess_basic(this, &pos, 1.0f);
}
