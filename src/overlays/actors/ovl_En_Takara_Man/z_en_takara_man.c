/*
 * File: z_en_takara_man.c
 * Overlay: ovl_En_Takara_Man
 * Description: Treasure Chest Game Man
 */

#include "z_en_takara_man.h"
#include "terminal.h"
#include "versions.h"
#include "assets/objects/object_ts/object_ts.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_LOCK_ON_DISABLED)

void En_Takara_Man_actor_ct(Actor* thisx, PlayState* play);
void En_Takara_Man_actor_dt(Actor* thisx, PlayState* play);
void En_Takara_Man_actor_move(Actor* thisx, PlayState* play);
void En_Takara_Man_actor_disp(Actor* thisx, PlayState* play);

static void mode_wait_init(EnTakaraMan* this, PlayState* play);
static void mode_wait(EnTakaraMan* this, PlayState* play);
static void mode_message_check(EnTakaraMan* this, PlayState* play);
void mode_select_chance(EnTakaraMan* this, PlayState* play);
static void mode_player_item_request(EnTakaraMan* this, PlayState* play);
static void mode_player_item_up(EnTakaraMan* this, PlayState* play);

ActorProfile En_Takara_Man_Profile = {
    /**/ ACTOR_EN_TAKARA_MAN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TS,
    /**/ sizeof(EnTakaraMan),
    /**/ En_Takara_Man_actor_ct,
    /**/ En_Takara_Man_actor_dt,
    /**/ En_Takara_Man_actor_move,
    /**/ En_Takara_Man_actor_disp,
};

static u8 izure_sakujyo = false;

void En_Takara_Man_actor_dt(Actor* thisx, PlayState* play) {
}

void En_Takara_Man_actor_ct(Actor* thisx, PlayState* play) {
    EnTakaraMan* this = (EnTakaraMan*)thisx;

    if (izure_sakujyo) {
        Actor_delete(&this->actor);
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ もういてる原 ☆☆☆☆☆ \n" VT_RST); // "Already initialized"
        return;
    }

    izure_sakujyo = true;
    PRINTF("\n\n");
    // "Bun! %x" (needs a better translation)
    PRINTF(VT_FGCOL(MAGENTA) "☆☆☆☆☆ ばぅん！ ☆☆☆☆☆ %x\n" VT_RST, play->actorCtx.flags.chest);
    play->actorCtx.flags.chest = 0;
    z_common_data.save.info.inventory.dungeonKeys[z_common_data.mapIndex] = -1;
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_ts_Skel_004FE0, &object_ts_Anim_000498, this->jointTable,
                       this->morphTable, 10);
    thisx->focus.pos = thisx->world.pos;
    this->pos = thisx->world.pos;
    thisx->world.pos.x = 133.0f;
    thisx->world.pos.y = -12.0f;
    thisx->world.pos.z = 102.0f;
    Actor_set_scale(&this->actor, 0.013f);
    this->height = 90.0f;
    this->originalRoomNum = thisx->room;
    thisx->room = -1;
    thisx->world.rot.y = thisx->shape.rot.y = -0x4E20;
    thisx->attentionRangeType = ATTENTION_RANGE_1;
    this->actionFunc = mode_wait_init;
}

static void mode_wait_init(EnTakaraMan* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&object_ts_Anim_000498);

    Skeleton_Info2_init(&this->skelAnime, &object_ts_Anim_000498, 1.0f, 0.0f, (s16)frameCount, ANIMMODE_LOOP, -10.0f);
    if (!this->unk_214) {
        this->actor.textId = 0x6D;
        this->dialogState = TEXT_STATE_CHOICE;
    }
    this->actionFunc = mode_wait;
}

static void mode_wait(EnTakaraMan* this, PlayState* play) {
    s16 absYawDiff;
    s16 yawDiff;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Actor_talk_check(&this->actor, play) && this->dialogState != TEXT_STATE_DONE) {
        if (!this->unk_214) {
            this->actionFunc = mode_select_chance;
        } else {
            this->actionFunc = mode_message_check;
        }
    } else {
        yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        if (play->roomCtx.curRoom.num == 6 && !this->unk_21A) {
            this->actor.textId = 0x6E;
            this->unk_21A = 1;
            this->dialogState = TEXT_STATE_DONE;
        }

        if (!this->unk_21A && this->unk_214) {
            if (Actor_Environment_sw_Check(play, 0x32)) {
                this->actor.textId = 0x84;
                this->dialogState = TEXT_STATE_EVENT;
            } else {
                this->actor.textId = 0x704C;
                this->dialogState = TEXT_STATE_DONE;
            }
        }

        absYawDiff = ABS(yawDiff);
        if (absYawDiff < 0x4300) {
            if (play->roomCtx.curRoom.num != this->originalRoomNum) {
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->unk_218 = 0;
            } else {
                if (!this->unk_218) {
                    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                    this->unk_218 = 1;
                }
                Actor_talk_request2(&this->actor, play, 100.0f);
            }
        }
    }
}

void mode_select_chance(EnTakaraMan* this, PlayState* play) {
    if (this->dialogState == message_check(&play->msgCtx) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // Yes
                if (z_common_data.save.info.playerData.rupees >= 10) {
                    message_close(play);
                    lupy_increase(-10);
                    this->unk_214 = 1;
                    this->actor.parent = NULL;
                    Actor_carry_request_set2(&this->actor, play, GI_DOOR_KEY, 2000.0f, 1000.0f);
                    this->actionFunc = mode_player_item_request;
                } else {
                    message_close(play);
                    this->actor.textId = 0x85;
                    message_set2(play, this->actor.textId);
                    this->dialogState = TEXT_STATE_EVENT;
                    this->actionFunc = mode_message_check;
                }
                break;
            case 1: // No
                message_close(play);
                this->actor.textId = 0x2D;
                message_set2(play, this->actor.textId);
                this->dialogState = TEXT_STATE_EVENT;
                this->actionFunc = mode_message_check;
                break;
        }
    }
}

static void mode_player_item_request(EnTakaraMan* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actionFunc = mode_player_item_up;
    } else {
#if OOT_VERSION < NTSC_1_1
        Actor_carry_request_set2(&this->actor, play, GI_SMALL_KEY, 2000.0f, 1000.0f);
#else
        Actor_carry_request_set2(&this->actor, play, GI_DOOR_KEY, 2000.0f, 1000.0f);
#endif
    }
}

static void mode_player_item_up(EnTakaraMan* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_DONE && pad_on_check(play)) {
        this->actionFunc = mode_wait_init;
    }
}

static void mode_message_check(EnTakaraMan* this, PlayState* play) {
    if (this->dialogState == message_check(&play->msgCtx) && pad_on_check(play)) {
        message_close(play);
        this->actionFunc = mode_wait_init;
    }
}

void En_Takara_Man_actor_move(Actor* thisx, PlayState* play) {
    EnTakaraMan* this = (EnTakaraMan*)thisx;

    if (this->eyeTimer != 0) {
        this->eyeTimer--;
    }

    Actor_world_to_eye(&this->actor, this->height);
    eye_move2(play, &this->actor, &this->unk_22C, &this->unk_232, this->actor.focus.pos);
    if (this->eyeTimer == 0) {
        this->eyeTextureIdx++;
        if (this->eyeTextureIdx >= 2) {
            this->eyeTextureIdx = 0;
            this->eyeTimer = (s16)rnd_f(60.0f) + 20;
        }
    }
    this->unk_212++;
    this->actionFunc(this, play);
}

s32 En_Takara_Man_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnTakaraMan* this = (EnTakaraMan*)thisx;

    if (limbIndex == 1) {
        rot->x += this->unk_232.y;
    }
    if (limbIndex == 8) {
        rot->x += this->unk_22C.y;
        rot->z += this->unk_22C.z;
    }
    return false;
}

void En_Takara_Man_actor_disp(Actor* thisx, PlayState* play) {
    static void* ts_eye_txt[] = {
        object_ts_Tex_000970,
        object_ts_Tex_000D70,
    };
    EnTakaraMan* this = (EnTakaraMan*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_takara_man.c", 528);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(ts_eye_txt[this->eyeTextureIdx]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          En_Takara_Man_draw_sub, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_takara_man.c", 544);
}
