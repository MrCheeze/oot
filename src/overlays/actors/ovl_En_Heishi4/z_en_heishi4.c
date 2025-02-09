#include "z_en_heishi4.h"
#include "assets/objects/object_sd/object_sd.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Heishi4_actor_ct(Actor* thisx, PlayState* play);
void En_Heishi4_actor_dt(Actor* thisx, PlayState* play);
void En_Heishi4_actor_move(Actor* thisx, PlayState* play);
void En_Heishi4_actor_draw(Actor* thisx, PlayState* play);

void mode_entrance_init(EnHeishi4* this, PlayState* play);
static void mode_dead_wait_init(EnHeishi4* this, PlayState* play);
static void mode_wait_init(EnHeishi4* this, PlayState* play);
static void mode_wait_message_select(EnHeishi4* this, PlayState* play);
static void mode_initial_mes_wait(EnHeishi4* this, PlayState* play);
void mode_entrance_message_select(EnHeishi4* this, PlayState* play);
static void mode_dead_wait(EnHeishi4* this, PlayState* play);
void mode_dead_wait_ende_init(EnHeishi4* this, PlayState* play);
void mode_dead_wait_ende(EnHeishi4* this, PlayState* play);
void mode_dead_wait_fainal_init(EnHeishi4* this, PlayState* play);
void mode_dead_wait_fainal(EnHeishi4* this, PlayState* play);

ActorProfile En_Heishi4_Profile = {
    /**/ ACTOR_EN_HEISHI4,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SD,
    /**/ sizeof(EnHeishi4),
    /**/ En_Heishi4_actor_ct,
    /**/ En_Heishi4_actor_dt,
    /**/ En_Heishi4_actor_move,
    /**/ En_Heishi4_actor_draw,
};

static u32 Mask_Data[] = { MASK_REACTION_SET_HEISHI4_1, MASK_REACTION_SET_HEISHI4_2 };

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

void En_Heishi4_actor_ct(Actor* thisx, PlayState* play) {
    EnHeishi4* this = (EnHeishi4*)thisx;

    Actor_set_scale(thisx, 0.01f);
    this->type = PARAMS_GET_U(thisx->params, 0, 8);
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    this->pos = thisx->world.pos;
    thisx->attentionRangeType = ATTENTION_RANGE_6;
    if (this->type == HEISHI4_AT_MARKET_DYING) {
        this->height = 30.0f;
        Shape_Info_init(&thisx->shape, 0.0f, NULL, 30.0f);
        Skeleton_Info2_M_ct(play, &this->skelAnime, &gEnHeishiSkel, &gEnHeishiDyingGuardAnim_00C444, this->jointTable,
                       this->morphTable, 17);
    } else {
        this->height = 60.0f;
        Shape_Info_init(&thisx->shape, 0.0f, Actor_shadow_circle, 30.0f);
        Skeleton_Info2_M_ct(play, &this->skelAnime, &gEnHeishiSkel, &gEnHeishiIdleAnim, this->jointTable, this->morphTable,
                       17);
    }
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, thisx, &OcInfoData);
    this->collider.dim.yShift = 0;
    this->collider.dim.radius = 15;
    this->collider.dim.height = 70;
    switch (this->type) {
        case HEISHI4_AT_KAKARIKO_ENTRANCE:
        case HEISHI4_AT_IMPAS_HOUSE:
            this->actionFunc = mode_wait_init;
            break;
        case HEISHI4_AT_MARKET_DYING:
            this->collider.dim.radius = 28;
            this->collider.dim.height = 5;
            this->actionFunc = mode_dead_wait_init;
            break;
        case HEISHI4_AT_MARKET_NIGHT:
            this->actionFunc = mode_entrance_init;
            break;
    }
    this->unk_27C = PARAMS_GET_U(thisx->params, 8, 8);
    PRINTF("\n\n");
    PRINTF(VT_FGCOL(GREEN) " ☆☆☆☆☆ 兵士２セット完了！ ☆☆☆☆☆ %d\n" VT_RST, thisx->params);
    PRINTF(VT_FGCOL(YELLOW) " ☆☆☆☆☆ 識別完了！\t    ☆☆☆☆☆ %d\n" VT_RST, this->type);
    PRINTF(VT_FGCOL(MAGENTA) " ☆☆☆☆☆ メッセージ完了！   ☆☆☆☆☆ %x\n\n" VT_RST, PARAMS_GET_U(thisx->params, 8, 4));
    PRINTF("\n\n");
}

void En_Heishi4_actor_dt(Actor* thisx, PlayState* play) {
    EnHeishi4* this = (EnHeishi4*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void mode_wait_init(EnHeishi4* this, PlayState* play) {
    f32 frames = Si2_anime_end_frame(&gEnHeishiIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_wait_message_select;
}

static void mode_wait_message_select(EnHeishi4* this, PlayState* play) {
    s16 reactionOffset;

    this->unk_2B4 = 0;
    reactionOffset = this->type - 4;
    if (reactionOffset < 0) {
        reactionOffset = 0;
    }
    if (reactionOffset >= 3) {
        reactionOffset = 1;
    }
    if (get_mask_message(play, Mask_Data[reactionOffset]) != 0) {
        this->actor.textId = get_mask_message(play, Mask_Data[reactionOffset]);
        this->unk_2B4 = 1;
        this->actionFunc = mode_initial_mes_wait;
    } else {
        if (GET_EVENTCHKINF(EVENTCHKINF_80)) {
            this->actor.textId = 0x5065;
            this->actionFunc = mode_initial_mes_wait;
            return;
        }
        if (GET_EVENTCHKINF(EVENTCHKINF_45)) {
            this->actor.textId = 0x5068;
            this->actionFunc = mode_initial_mes_wait;
            return;
        }
        if (this->type == HEISHI4_AT_IMPAS_HOUSE) {
            if (this->unk_284 == 0) {
                this->actor.textId = 0x5079;
            } else {
                this->actor.textId = 0x507A;
            }
        } else if (IS_DAY) {
            if (this->unk_284 == 0) {
                this->actor.textId = 0x5063;
            } else {
                this->actor.textId = 0x5064;
            }
        } else if (this->unk_284 == 0) {
            this->actor.textId = 0x5066;
        } else {
            this->actor.textId = 0x5067;
        }

        this->actionFunc = mode_initial_mes_wait;
    }
}

void mode_entrance_init(EnHeishi4* this, PlayState* play) {
    f32 frames = Si2_anime_end_frame(&gEnHeishiIdleAnim);

    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiIdleAnim, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, -10.0f);
    if (LINK_AGE_IN_YEARS != YEARS_CHILD) {
        PRINTF(VT_FGCOL(GREEN) " ☆☆☆☆☆ ぎゃぁ！オトナだー ☆☆☆☆☆ \n" VT_RST);
        Actor_delete(&this->actor);
    } else {
        this->actionFunc = mode_entrance_message_select;
    }
}

void mode_entrance_message_select(EnHeishi4* this, PlayState* play) {
    s16 reactionOffset;

    reactionOffset = this->type - 4;
    this->unk_2B4 = 0;
    if (reactionOffset < 0) {
        reactionOffset = 0;
    }
    if (reactionOffset >= 3) {
        reactionOffset = 1;
    }
    if (get_mask_message(play, Mask_Data[reactionOffset]) != 0) {
        this->actor.textId = get_mask_message(play, Mask_Data[reactionOffset]);
        this->unk_2B4 = 1;
        this->actionFunc = mode_initial_mes_wait;
        return;
    }
    if (play->sceneId == SCENE_MARKET_GUARD_HOUSE) {
        if (IS_DAY) {
            this->actor.textId = 0x7004;
        } else {
            this->actor.textId = 0x709A;
        }
    } else if (play->sceneId != SCENE_MARKET_NIGHT) {
        if (IS_DAY) {
            this->actor.textId = 0x7002;
        } else {
            this->actor.textId = 0x7003;
        }
    } else {
        this->actor.textId = 0x7003;
    }

    this->actionFunc = mode_initial_mes_wait;
}

static void mode_dead_wait_init(EnHeishi4* this, PlayState* play) {
    f32 frames;

    if (GET_EVENTCHKINF(EVENTCHKINF_45)) {
        PRINTF(VT_FGCOL(YELLOW) " ☆☆☆☆☆ マスターソード祝入手！ ☆☆☆☆☆ \n" VT_RST);
        Actor_delete(&this->actor);
        return;
    }
    this->unk_284 = 0;
    if (GET_EVENTCHKINF(EVENTCHKINF_80)) {
        if (!GET_INFTABLE(INFTABLE_6C)) {
            frames = Si2_anime_end_frame(&gEnHeishiDyingGuardAnim_00C444);
            Skeleton_Info2_init(&this->skelAnime, &gEnHeishiDyingGuardAnim_00C444, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP,
                             -10.0f);
            this->actor.textId = 0x7007;
            this->unk_282 = TEXT_STATE_EVENT;
            this->unk_284 = 1;
            PRINTF(VT_FGCOL(YELLOW) " ☆☆☆☆☆ デモ開始！ ☆☆☆☆☆ \n" VT_RST);
        } else {
            this->actor.textId = 0x7008;
            this->unk_282 = TEXT_STATE_DONE;
            PRINTF(VT_FGCOL(BLUE) " ☆☆☆☆☆ 返事なし ☆☆☆☆☆ \n" VT_RST);
        }
        this->actionFunc = mode_dead_wait;
    } else {
        Actor_delete(&this->actor);
    }
}

static void mode_dead_wait(EnHeishi4* this, PlayState* play) {
    if (this->unk_284 != 0) {
        Skeleton_Info2_anime_play(&this->skelAnime);
    }
    if (Actor_talk_check(&this->actor, play)) {
        if (this->unk_284 == 0) {
            this->actionFunc = mode_dead_wait_init;

        } else {
            this->actionFunc = mode_dead_wait_ende_init;
        }
    } else {
        Actor_talk_request2(&this->actor, play, 100.0f);
    }
}

void mode_dead_wait_ende_init(EnHeishi4* this, PlayState* play) {
    f32 frames = Si2_anime_end_frame(&gEnHeishiDyingGuardTalkAnim);

    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiDyingGuardTalkAnim, 1.0f, 0.0f, (s16)frames, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_dead_wait_ende;
}

void mode_dead_wait_ende(EnHeishi4* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    eye_move2(play, &this->actor, &this->headRot, &this->torsoRot, this->actor.focus.pos);
    if ((this->unk_282 == message_check(&play->msgCtx)) && pad_on_check(play)) {
        message_close(play);
        SET_INFTABLE(INFTABLE_6C);
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        this->actionFunc = mode_dead_wait_fainal_init;
    }
}

void mode_dead_wait_fainal_init(EnHeishi4* this, PlayState* play) {
    f32 frames = Si2_anime_end_frame(&gEnHeishiDyingGuardDieAnim);

    this->unk_288 = frames;
    Skeleton_Info2_init(&this->skelAnime, &gEnHeishiDyingGuardDieAnim, 1.0f, 0.0f, frames, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = mode_dead_wait_fainal;
}

void mode_dead_wait_fainal(EnHeishi4* this, PlayState* play) {
    f32 currentFrame = this->skelAnime.curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->unk_288 <= currentFrame) {
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        this->actionFunc = mode_dead_wait_init;
    }
}

static void mode_initial_mes_wait(EnHeishi4* this, PlayState* play) {
    s16 reactionOffset;

    Skeleton_Info2_anime_play(&this->skelAnime);
    reactionOffset = this->type - 4;
    if (reactionOffset < 0) {
        reactionOffset = 0;
    }
    if (reactionOffset >= 3) {
        reactionOffset = 1;
    }
    if (get_mask_message(play, Mask_Data[reactionOffset]) != 0) {
        if (this->unk_2B4 == 0) {
            if ((this->type == HEISHI4_AT_KAKARIKO_ENTRANCE) || (this->type == HEISHI4_AT_IMPAS_HOUSE)) {
                this->actionFunc = mode_wait_message_select;
                return;
            }
            if (this->type == HEISHI4_AT_MARKET_NIGHT) {
                this->actionFunc = mode_entrance_message_select;
                return;
            }
        }
    } else {
        if (this->unk_2B4 != 0) {
            if ((this->type == HEISHI4_AT_KAKARIKO_ENTRANCE) || (this->type == HEISHI4_AT_IMPAS_HOUSE)) {
                this->actionFunc = mode_wait_message_select;
                return;
            }
            if (this->type == HEISHI4_AT_MARKET_NIGHT) {
                this->actionFunc = mode_entrance_message_select;
                return;
            }
        }
    }
    if (Actor_talk_check(&this->actor, play)) {
        if ((this->type == HEISHI4_AT_KAKARIKO_ENTRANCE) || (this->type == HEISHI4_AT_IMPAS_HOUSE)) {
            this->unk_284 = 1;
            this->actionFunc = mode_wait_message_select;
            return;
        }
        if (this->type == HEISHI4_AT_MARKET_NIGHT) {
            this->actionFunc = mode_entrance_message_select;
            return;
        }
    }
    Actor_talk_request(&this->actor, play);
}

void En_Heishi4_actor_move(Actor* thisx, PlayState* play) {
    EnHeishi4* this = (EnHeishi4*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    thisx->world.pos.x = this->pos.x;
    thisx->world.pos.y = this->pos.y;
    thisx->world.pos.z = this->pos.z;
    Actor_world_to_eye(thisx, this->height);
    if (this->type != HEISHI4_AT_MARKET_DYING) {
        this->interactInfo.trackPos = player->actor.world.pos;
        if (!LINK_IS_ADULT) {
            this->interactInfo.trackPos.y = player->actor.world.pos.y - 10.0f;
        }
        eye_moveM(thisx, &this->interactInfo, 2, NPC_TRACKING_FULL_BODY);
        this->headRot = this->interactInfo.headRot;
        this->torsoRot = this->interactInfo.torsoRot;
    }
    this->unk_27E++;
    this->actionFunc(this, play);
    Actor_position_moveF(thisx);
    Actor_BGcheck2(play, thisx, 10.0f, 10.0f, 30.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

s32 En_Heishi4_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHeishi4* this = (EnHeishi4*)thisx;

    if (limbIndex == 9) {
        rot->x += this->torsoRot.y;
    }
    if (limbIndex == 16) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.z;
    }
    return false;
}

void En_Heishi4_actor_draw(Actor* thisx, PlayState* play) {
    EnHeishi4* this = (EnHeishi4*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, En_Heishi4_draw_sub, NULL,
                      this);
}
