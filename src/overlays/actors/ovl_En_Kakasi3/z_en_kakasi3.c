/*
 * File: z_en_kakasi3.c
 * Overlay: ovl_En_Kakasi3
 * Description: Bonooru the Scarecrow
 */

#include "z_en_kakasi3.h"
#include "terminal.h"
#include "assets/objects/object_ka/object_ka.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_Kakasi3_actor_ct(Actor* thisx, PlayState* play);
void En_Kakasi3_actor_dt(Actor* thisx, PlayState* play);
void En_Kakasi3_actor_move(Actor* thisx, PlayState* play);
void En_Kakasi3_actor_disp(Actor* thisx, PlayState* play);

static void mode_wait_init(EnKakasi3* this, PlayState* play);
static void mode_wait_message_select(EnKakasi3* this, PlayState* play);
static void mode_wait(EnKakasi3* this, PlayState* play);
void mode_rec_message_check(EnKakasi3* this, PlayState* play);
static void mode_okarina_rec_check(EnKakasi3* this, PlayState* play);
void mode_once_more_play_check(EnKakasi3* this, PlayState* play);
void mode_play_go(EnKakasi3* this, PlayState* play);
void mode_rec_ret_message_check(EnKakasi3* this, PlayState* play);
void mode_okarina_rec_ret_check(EnKakasi3* this, PlayState* play);
static void mode_message_check_ende(EnKakasi3* this, PlayState* play);

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

ActorProfile En_Kakasi3_Profile = {
    /**/ ACTOR_EN_KAKASI3,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_KA,
    /**/ sizeof(EnKakasi3),
    /**/ En_Kakasi3_actor_ct,
    /**/ En_Kakasi3_actor_dt,
    /**/ En_Kakasi3_actor_move,
    /**/ En_Kakasi3_actor_disp,
};

void En_Kakasi3_actor_dt(Actor* thisx, PlayState* play) {
    EnKakasi3* this = (EnKakasi3*)thisx;

    ClObjPipe_dt(play, &this->collider);
    //! @bug Skeleton_Info_dt is not called
}

void En_Kakasi3_actor_ct(Actor* thisx, PlayState* play) {
    EnKakasi3* this = (EnKakasi3*)thisx;

    PRINTF("\n\n");
    // "Obonur" -- Related to the name of the scarecrow (Bonooru)
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ おーボヌール ☆☆☆☆☆ \n" VT_RST);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);
    this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    this->rot = this->actor.world.rot;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_set_scale(&this->actor, 0.01f);
    this->actionFunc = mode_wait_init;
}

static void stop_animetion(EnKakasi3* this) {
    this->unk_1A4 = 0;
    this->skelAnime.playSpeed = 0.0f;
    this->unk_1AA = this->unk_1AE = 0x0;

    add_calc0(&this->skelAnime.curFrame, 0.5f, 1.0f);
    add_calc_short_angle2(&this->actor.shape.rot.x, this->rot.x, 5, 0x2710, 0);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->rot.y, 5, 0x2710, 0);
    add_calc_short_angle2(&this->actor.shape.rot.z, this->rot.z, 5, 0x2710, 0);
}

static void dance_to_dance_check(EnKakasi3* this, PlayState* play, s32 arg) {
    s16 currentFrame;
    s16 ocarinaNote = play->msgCtx.lastOcarinaButtonIndex;

    if (arg != 0) {
        if (this->unk_19C[3] == 0) {
            this->unk_19C[3] = (s16)rnd_f(10.99f) + 30;
            this->unk_1A6 = (s16)rnd_f(4.99f);
        }

        this->unk_19A = (s16)rnd_f(2.99f) + 5;
        ocarinaNote = this->unk_1A6;
    }
    switch (ocarinaNote) {
        case OCARINA_BTN_A:
            this->unk_19A++;
            if (this->unk_1A4 == 0) {
                this->unk_1A4 = 1;
                Actor_SE_set(&this->actor, NA_SE_EV_KAKASHI_ROLL);
            }
            break;
        case OCARINA_BTN_C_DOWN:
            this->unk_19A++;
            this->unk_1B8 = 1.0f;
            break;
        case OCARINA_BTN_C_RIGHT:
            this->unk_19A++;
            if (this->unk_1AE == 0x0) {
                this->unk_1AE = 0x1388;
            }
            break;
        case OCARINA_BTN_C_LEFT:
            this->unk_19A++;
            if (this->unk_1AA == 0x0) {
                this->unk_1AA = 0x1388;
            }
            break;
        case OCARINA_BTN_C_UP:
            this->unk_19A++;
            this->unk_1B8 = 2.0f;
            break;
    }

    if (this->unk_19A > 8) {
        this->unk_19A = 8;
    }

    if (this->unk_19A != 0) {
        this->actor.gravity = -1.0f;
        if (this->unk_19A == 8 && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
            this->actor.velocity.y = 3.0f;
            Actor_SE_set(&this->actor, NA_SE_IT_KAKASHI_JUMP);
        }
        add_calc2(&this->skelAnime.playSpeed, this->unk_1B8, 0.1f, 0.2f);
        add_calc_short_angle2(&this->actor.shape.rot.x, this->unk_1AA, 0x5, 0x3E8, 0);
        add_calc_short_angle2(&this->actor.shape.rot.z, this->unk_1AE, 0x5, 0x3E8, 0);

        if (this->unk_1AA != 0x0 && fabsf(this->actor.shape.rot.x - this->unk_1AA) < 50.0f) {
            this->unk_1AA *= -1.0f;
        }
        if (this->unk_1AE != 0x0 && fabsf(this->actor.shape.rot.z - this->unk_1AE) < 50.0f) {
            this->unk_1AE *= -1.0f;
        }

        if (this->unk_1A4 != 0) {
            this->actor.shape.rot.y += 0x1000;
            if (this->actor.shape.rot.y == 0) {
                this->unk_1A4 = 0;
            }
        }
        currentFrame = this->skelAnime.curFrame;
        if (currentFrame == 11 || currentFrame == 17) {
            Actor_SE_set(&this->actor, NA_SE_EV_KAKASHI_SWING);
        }
        Skeleton_Info2_anime_play(&this->skelAnime);
    }
}

static void mode_wait_init(EnKakasi3* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&object_ka_Anim_000214);

    Skeleton_Info2_init(&this->skelAnime, &object_ka_Anim_000214, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_wait_message_select;
}

static void mode_wait_message_select(EnKakasi3* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    this->actor.textId = 0x40A1;
    this->dialogState = TEXT_STATE_DONE;
    this->unk_19A = 0;

    if (!LINK_IS_ADULT) {
        this->unk_194 = false;
        if (z_common_data.save.info.scarecrowSpawnSongSet) {
            this->actor.textId = 0x40A0;
            this->dialogState = TEXT_STATE_EVENT;
            this->unk_1A8 = 1;
        }
    } else {
        this->unk_194 = true;
        if (z_common_data.save.info.scarecrowSpawnSongSet) {
            if (this->unk_195) {
                this->actor.textId = 0x40A2;
            } else {
                this->actor.textId = 0x40A3;
            }
        }
    }
    this->actionFunc = mode_wait;
}

static void mode_wait(EnKakasi3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    stop_animetion(this);
    Skeleton_Info2_anime_play(&this->skelAnime);
    this->subCamId = CAM_ID_NONE;
    if (Actor_talk_check(&this->actor, play)) {
        if (!this->unk_194) {
            if (this->unk_1A8 == 0) {
                this->actionFunc = mode_wait_message_select;
            } else {
                this->actionFunc = mode_once_more_play_check;
            }
        } else {
            this->actionFunc = mode_wait_message_select;
        }
    } else {
        s16 angleTowardsLink = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

        if (!(this->actor.xzDistToPlayer > 120.0f)) {
            s16 absAngleTowardsLink = ABS(angleTowardsLink);

            if (absAngleTowardsLink < 0x4300) {
                if (!this->unk_194) {

                    if (player->stateFlags2 & PLAYER_STATE2_24) {
                        this->subCamId = makeOnepointDemo(play, 2260, -99, &this->actor, CAM_ID_MAIN);
                        play->msgCtx.msgMode = MSGMODE_PAUSED;
                        this->dialogState = TEXT_STATE_EVENT;
                        this->unk_1B8 = 0.0f;
                        message_set(play, 0x40A4, NULL);
                        player->stateFlags2 |= PLAYER_STATE2_23;
                        this->actionFunc = mode_rec_message_check;
                        return;
                    }
                    if (this->actor.xzDistToPlayer < 80.0f) {
                        player->stateFlags2 |= PLAYER_STATE2_23;
                    }
                } else if (z_common_data.save.info.scarecrowSpawnSongSet && !this->unk_195) {

                    if (player->stateFlags2 & PLAYER_STATE2_24) {
                        this->subCamId = makeOnepointDemo(play, 2260, -99, &this->actor, CAM_ID_MAIN);
                        play->msgCtx.msgMode = MSGMODE_PAUSED;
                        this->dialogState = TEXT_STATE_EVENT;
                        this->unk_1B8 = 0.0f;
                        message_set(play, 0x40A8, NULL);
                        player->stateFlags2 |= PLAYER_STATE2_23;
                        this->actionFunc = mode_rec_ret_message_check;
                        return;
                    }
                    if (this->actor.xzDistToPlayer < 80.0f) {
                        player->stateFlags2 |= PLAYER_STATE2_23;
                    }
                }
                Actor_talk_request2(&this->actor, play, 100.0f);
            }
        }
    }
}

void mode_rec_message_check(EnKakasi3* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        message_close(play);
        ocarina_set(play, OCARINA_ACTION_SCARECROW_SPAWN_RECORDING);
        this->actionFunc = mode_okarina_rec_check;
    }
}

static void mode_okarina_rec_check(EnKakasi3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((play->msgCtx.ocarinaMode == OCARINA_MODE_04 ||
         (play->msgCtx.ocarinaMode >= OCARINA_MODE_05 && play->msgCtx.ocarinaMode < OCARINA_MODE_0B)) &&
        (play->msgCtx.msgMode == MSGMODE_NONE)) {

        deleteOnepointDemo(play, this->subCamId);
        if (play->cameraPtrs[this->subCamId] == NULL) {
            this->subCamId = CAM_ID_NONE;
        }
        if (this->subCamId != CAM_ID_NONE) {
            restartCameraStoped(play->cameraPtrs[this->subCamId]);
        }
        this->actionFunc = mode_wait_init;
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_03 && play->msgCtx.msgMode == MSGMODE_NONE) {
        this->dialogState = TEXT_STATE_EVENT;
        message_set(play, 0x40A5, NULL);
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        this->actionFunc = mode_message_check_ende;
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
        dance_to_dance_check(this, play, 0);
        player->stateFlags2 |= PLAYER_STATE2_23;
    }
}

void mode_once_more_play_check(EnKakasi3* this, PlayState* play) {

    stop_animetion(this);
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->dialogState == message_check(&play->msgCtx) && pad_on_check(play)) {
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        ocarina_set(play, OCARINA_ACTION_SCARECROW_SPAWN_PLAYBACK);
        this->actionFunc = mode_play_go;
        this->subCamId = makeOnepointDemo(play, 2280, -99, &this->actor, CAM_ID_MAIN);
    }
}

void mode_play_go(EnKakasi3* this, PlayState* play) {

    if (play->msgCtx.ocarinaMode != OCARINA_MODE_0F) {
        dance_to_dance_check(this, play, 1);
    } else {
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        message_close(play);
        deleteOnepointDemo(play, this->subCamId);
        this->actionFunc = mode_wait_init;
    }
}

void mode_rec_ret_message_check(EnKakasi3* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT && pad_on_check(play)) {
        message_close(play);
        ocarina_set(play, OCARINA_ACTION_CHECK_SCARECROW_SPAWN);
        this->actionFunc = mode_okarina_rec_ret_check;
    }
}

void mode_okarina_rec_ret_check(EnKakasi3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (BREG(3) != 0) {
        // "No way!"
        PRINTF(VT_FGCOL(MAGENTA) "☆☆☆☆☆ まさか！ ☆☆☆☆☆ %d\n" VT_RST, play->msgCtx.ocarinaMode);
    }
    if ((play->msgCtx.ocarinaMode == OCARINA_MODE_04 ||
         (play->msgCtx.ocarinaMode >= OCARINA_MODE_05 && play->msgCtx.ocarinaMode < OCARINA_MODE_0B)) &&
        play->msgCtx.msgMode == MSGMODE_NONE) {

        message_set(play, 0x40A6, NULL);
        this->dialogState = TEXT_STATE_EVENT;
        deleteOnepointDemo(play, this->subCamId);
        this->subCamId = CAM_ID_NONE;
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        this->actionFunc = mode_message_check_ende;
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_03 && play->msgCtx.msgMode == MSGMODE_NONE) {
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        if (BREG(3) != 0) {
            PRINTF("\n\n");
            // "With this, other guys are OK! That's it!"
            PRINTF(VT_FGCOL(CYAN) "☆☆☆☆☆ これで、他の奴もＯＫ！だ！ ☆☆☆☆☆ %d\n" VT_RST, play->msgCtx.ocarinaMode);
        }
        this->unk_195 = true;
        message_set(play, 0x40A7, NULL);
        this->dialogState = TEXT_STATE_EVENT;
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        this->actionFunc = mode_message_check_ende;
        return;
    }

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
        dance_to_dance_check(this, play, 0);
        player->stateFlags2 |= PLAYER_STATE2_23;
    }
}

static void mode_message_check_ende(EnKakasi3* this, PlayState* play) {
    stop_animetion(this);
    Skeleton_Info2_anime_play(&this->skelAnime);
    player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);

    if (this->dialogState == message_check(&play->msgCtx) && pad_on_check(play)) {
        if (this->unk_195) {
            if (!GET_EVENTCHKINF(EVENTCHKINF_9C)) {
                SET_EVENTCHKINF(EVENTCHKINF_9C);
            }
        }
        if (play->cameraPtrs[this->subCamId] == NULL) {
            this->subCamId = CAM_ID_NONE;
        }
        if (this->subCamId != CAM_ID_NONE) {
            restartCameraStoped(play->cameraPtrs[this->subCamId]);
        }
        message_close(play);
        play->msgCtx.ocarinaMode = OCARINA_MODE_04;
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        this->actionFunc = mode_wait_init;
    }
}

void En_Kakasi3_actor_move(Actor* thisx, PlayState* play) {
    EnKakasi3* this = (EnKakasi3*)thisx;
    s32 pad;
    s32 i;

    if (BREG(2) != 0) {
        PRINTF("\n\n");
        // "flag!"
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ フラグ！ ☆☆☆☆☆ %d\n" VT_RST, z_common_data.save.info.scarecrowSpawnSongSet);
    }

    this->unk_198++;
    this->actor.world.rot = this->actor.shape.rot;
    for (i = 0; i < ARRAY_COUNT(this->unk_19C); i++) {
        if (this->unk_19C[i] != 0) {
            this->unk_19C[i]--;
        }
    }

    Actor_world_to_eye(&this->actor, 60.0f);
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 50.0f, 50.0f, 100.0f,
                            UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void En_Kakasi3_actor_disp(Actor* thisx, PlayState* play) {
    EnKakasi3* this = (EnKakasi3*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, this);
}
