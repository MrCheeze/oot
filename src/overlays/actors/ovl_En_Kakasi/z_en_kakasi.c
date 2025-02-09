/*
 * File: z_en_kakasi.c
 * Overlay: ovl_En_Kakasi
 * Description: Pierre the Scarecrow
 */

#include "z_en_kakasi.h"
#include "terminal.h"
#include "assets/objects/object_ka/object_ka.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_Kakasi_actor_ct(Actor* thisx, PlayState* play);
void En_Kakasi_actor_dt(Actor* thisx, PlayState* play);
void En_Kakasi_actor_move(Actor* thisx, PlayState* play);
void En_Kakasi_actor_disp(Actor* thisx, PlayState* play);

static void mode_wait_init(EnKakasi* this, PlayState* play);
static void mode_wait(EnKakasi* this, PlayState* play);
static void mode_okarina_rec_check(EnKakasi* this, PlayState* play);
void mode_demo_message_check(EnKakasi* this, PlayState* play);
static void mode_message_check_ende(EnKakasi* this, PlayState* play);
void mode_play_dance(EnKakasi* this, PlayState* play);

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
        ACELEM_NONE | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

ActorProfile En_Kakasi_Profile = {
    /**/ ACTOR_EN_KAKASI,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_KA,
    /**/ sizeof(EnKakasi),
    /**/ En_Kakasi_actor_ct,
    /**/ En_Kakasi_actor_dt,
    /**/ En_Kakasi_actor_move,
    /**/ En_Kakasi_actor_disp,
};

void En_Kakasi_actor_dt(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;

    ClObjPipe_dt(play, &this->collider);
    //! @bug Skeleton_Info_dt is not called
}

void En_Kakasi_actor_ct(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;

    PRINTF("\n\n");
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ Ｌｅｔ’ｓ ＤＡＮＣＥ！ ☆☆☆☆☆ %f\n" VT_RST, this->actor.world.pos.y);

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    Skeleton_Info2_SV_M_ct(play, &this->skelanime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);

    this->rot = this->actor.world.rot;
    this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    Actor_set_scale(&this->actor, 0.01f);
    this->actionFunc = mode_wait_init;
}

static void stop_animetion(EnKakasi* this) {
    this->unk_1A4 = 0;
    this->skelanime.playSpeed = 0.0f;
    this->unk_1A8 = this->unk_1AC = 0;

    add_calc0(&this->skelanime.curFrame, 0.5f, 1.0f);
    add_calc_short_angle2(&this->actor.shape.rot.x, this->rot.x, 5, 0x2710, 0);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->rot.y, 5, 0x2710, 0);
    add_calc_short_angle2(&this->actor.shape.rot.z, this->rot.z, 5, 0x2710, 0);
}

static void dance_to_dance_check(EnKakasi* this, PlayState* play, s16 arg) {
    s16 ocarinaNote = play->msgCtx.lastOcarinaButtonIndex;
    s16 currentFrame;

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
            if (this->unk_1AC == 0) {
                this->unk_1AC = 0x1388;
            }
            break;
        case OCARINA_BTN_C_LEFT:
            this->unk_19A++;
            if (this->unk_1A8 == 0) {
                this->unk_1A8 = 0x1388;
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
        add_calc2(&this->skelanime.playSpeed, this->unk_1B8, 0.1f, 0.2f);
        add_calc_short_angle2(&this->actor.shape.rot.x, this->unk_1A8, 5, 0x3E8, 0);
        add_calc_short_angle2(&this->actor.shape.rot.z, this->unk_1AC, 5, 0x3E8, 0);

        if (this->unk_1A8 != 0 && fabsf(this->actor.shape.rot.x - this->unk_1A8) < 50.0f) {
            this->unk_1A8 *= -1.0f;
        }
        if (this->unk_1AC != 0 && fabsf(this->actor.shape.rot.z - this->unk_1AC) < 50.0f) {
            this->unk_1AC *= -1.0f;
        }

        if (this->unk_1A4 != 0) {
            this->actor.shape.rot.y += 0x1000;
            if (this->actor.shape.rot.y == 0) {
                this->unk_1A4 = 0;
            }
        }
        currentFrame = this->skelanime.curFrame;
        if (currentFrame == 11 || currentFrame == 17) {
            Actor_SE_set(&this->actor, NA_SE_EV_KAKASHI_SWING);
        }
        Skeleton_Info2_anime_play(&this->skelanime);
    }
}

static void mode_wait_init(EnKakasi* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&object_ka_Anim_000214);

    Skeleton_Info2_init(&this->skelanime, &object_ka_Anim_000214, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);

    this->actor.textId = 0x4076;
    this->unk_196 = TEXT_STATE_DONE;
    if (!LINK_IS_ADULT) {
        this->unk_194 = false;
        if (z_common_data.save.info.scarecrowLongSongSet) {
            this->actor.textId = 0x407A;
            this->unk_196 = TEXT_STATE_EVENT;
        }
    } else {
        this->unk_194 = true;
        if (z_common_data.save.info.scarecrowLongSongSet) {
            this->actor.textId = 0x4079;
            this->unk_196 = TEXT_STATE_EVENT;
        }
    }
    this->actionFunc = mode_wait;
}

static void mode_wait(EnKakasi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    stop_animetion(this);
    Skeleton_Info2_anime_play(&this->skelanime);
    this->subCamId = CAM_ID_NONE;
    if (Actor_talk_check(&this->actor, play)) {
        if (this->unk_196 == TEXT_STATE_EVENT) {
            this->actionFunc = mode_demo_message_check;
        } else {
            this->actionFunc = mode_wait_init;
        }
    } else {
        s16 yawTowardsPlayer = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

        if (!(this->actor.xzDistToPlayer > 120.0f)) {
            s16 absyawTowardsPlayer = ABS(yawTowardsPlayer);

            if (absyawTowardsPlayer < 0x4300) {
                if (!this->unk_194) {
                    if (player->stateFlags2 & PLAYER_STATE2_24) {
                        this->subCamId = makeOnepointDemo(play, 2260, -99, &this->actor, CAM_ID_MAIN);

                        ocarina_set(play, OCARINA_ACTION_SCARECROW_LONG_RECORDING);
                        this->unk_19A = 0;
                        this->unk_1B8 = 0.0;
                        player->stateFlags2 |= PLAYER_STATE2_23;
                        this->actionFunc = mode_okarina_rec_check;
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

static void mode_okarina_rec_check(EnKakasi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04 && play->msgCtx.msgMode == MSGMODE_NONE) {
        // "end?"
        PRINTF(VT_FGCOL(BLUE) "☆☆☆☆☆ 終り？ ☆☆☆☆☆ \n" VT_RST);

        if (this->unk_19A != 0) {
            message_close(play);
            this->actor.textId = 0x4077;
            this->unk_196 = TEXT_STATE_EVENT;
            message_set(play, this->actor.textId, NULL);
            this->actionFunc = mode_demo_message_check;
        } else {
            deleteOnepointDemo(play, this->subCamId);
            this->subCamId = CAM_ID_NONE;
            this->actionFunc = mode_wait_init;
        }
    } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_01) {
        dance_to_dance_check(this, play, 0);
        player->stateFlags2 |= PLAYER_STATE2_23;
    }
}

void mode_demo_message_check(EnKakasi* this, PlayState* play) {
    stop_animetion(this);
    Skeleton_Info2_anime_play(&this->skelanime);
    player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);

    if (this->unk_196 == message_check(&play->msgCtx) && pad_on_check(play)) {

        if (this->subCamId != CAM_ID_NONE) {
            restartCameraStoped(play->cameraPtrs[this->subCamId]);
        }
        this->subCamId = makeOnepointDemo(play, 2270, -99, &this->actor, CAM_ID_MAIN);
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        ocarina_set(play, OCARINA_ACTION_SCARECROW_LONG_PLAYBACK);
        this->actionFunc = mode_play_dance;
    }
}

void mode_play_dance(EnKakasi* this, PlayState* play) {
    if (play->msgCtx.ocarinaMode != OCARINA_MODE_0F) {
        dance_to_dance_check(this, play, 1);
        return;
    }

    PRINTF("game_play->message.msg_mode=%d\n", play->msgCtx.msgMode);

    if (play->msgCtx.msgMode == MSGMODE_NONE) {
        if (this->unk_194) {
            this->actor.textId = 0x4077;
            this->unk_196 = TEXT_STATE_EVENT;
            message_set(play, this->actor.textId, NULL);
        } else {
            this->actor.textId = 0x4078;
            this->unk_196 = TEXT_STATE_EVENT;
            message_set(play, this->actor.textId, NULL);
        }
        this->actionFunc = mode_message_check_ende;
        deleteOnepointDemo(play, this->subCamId);
        this->subCamId = CAM_ID_NONE;
        this->subCamId = makeOnepointDemo(play, 2260, -99, &this->actor, CAM_ID_MAIN);
        restartCameraStoped(play->cameraPtrs[this->subCamId]);
    }
}

static void mode_message_check_ende(EnKakasi* this, PlayState* play) {
    stop_animetion(this);
    Skeleton_Info2_anime_play(&this->skelanime);

    if (this->unk_196 == message_check(&play->msgCtx) && pad_on_check(play)) {
        restartCameraStoped(play->cameraPtrs[this->subCamId]);
        message_close(play);
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        this->actionFunc = mode_wait_init;
    }
}

void En_Kakasi_actor_move(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;
    s32 pad;
    s32 i;

    this->unk_198++;
    this->actor.world.rot = this->actor.shape.rot;
    for (i = 0; i < ARRAY_COUNT(this->unk_19C); i++) {
        if (this->unk_19C[i] != 0) {
            this->unk_19C[i]--;
        }
    }

    this->height = 60.0f;
    Actor_world_to_eye(&this->actor, this->height);
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 50.0f, 50.0f, 100.0f,
                            UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void En_Kakasi_actor_disp(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;

    if (BREG(3) != 0) {
        PRINTF("\n\n");
        // "flag!"
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ フラグ！ ☆☆☆☆☆ %d\n" VT_RST, z_common_data.save.info.scarecrowLongSongSet);
    }
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelanime.skeleton, this->skelanime.jointTable, this->skelanime.dListCount, NULL,
                          NULL, this);
}
