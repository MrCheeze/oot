#include "z_en_sa.h"
#include "overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "assets/objects/object_sa/object_sa.h"
#include "assets/scenes/overworld/spot04/spot04_scene.h"
#include "assets/scenes/overworld/spot05/spot05_scene.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_Sa_Actor_ct(Actor* thisx, PlayState* play);
void En_Sa_Actor_dt(Actor* thisx, PlayState* play);
void En_Sa_Actor_move(Actor* thisx, PlayState* play);
void En_Sa_Actor_draw(Actor* thisx, PlayState* play);

void sa_wait(EnSa* this, PlayState* play);
void sa_talking(EnSa* this, PlayState* play);
void sa_oca_wait(EnSa* this, PlayState* play);
void sa_demo(EnSa* this, PlayState* play);
void sa_demo_end(EnSa* this, PlayState* play);

typedef enum SariaEyeState {
    /* 0 */ SARIA_EYE_OPEN,
    /* 1 */ SARIA_EYE_HALF,
    /* 2 */ SARIA_EYE_CLOSED,
    /* 3 */ SARIA_EYE_SUPRISED,
    /* 4 */ SARIA_EYE_SAD
} SariaEyeState;

typedef enum SariaMouthState {
    /* 0 */ SARIA_MOUTH_CLOSED2,
    /* 1 */ SARIA_MOUTH_SUPRISED,
    /* 2 */ SARIA_MOUTH_CLOSED,
    /* 3 */ SARIA_MOUTH_SMILING_OPEN,
    /* 4 */ SARIA_MOUTH_FROWNING
} SariaMouthState;

ActorProfile En_Sa_Profile = {
    /**/ ACTOR_EN_SA,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_SA,
    /**/ sizeof(EnSa),
    /**/ En_Sa_Actor_ct,
    /**/ En_Sa_Actor_dt,
    /**/ En_Sa_Actor_move,
    /**/ En_Sa_Actor_draw,
};

static ColliderCylinderInit EnSaAtInfoData = {
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
    { 20, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 SaStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum EnSaAnimation1 {
    /*  0 */ ENSA_ANIM1_0,
    /*  1 */ ENSA_ANIM1_1,
    /*  2 */ ENSA_ANIM1_2,
    /*  3 */ ENSA_ANIM1_3,
    /*  4 */ ENSA_ANIM1_4,
    /*  5 */ ENSA_ANIM1_5,
    /*  6 */ ENSA_ANIM1_6,
    /*  7 */ ENSA_ANIM1_7,
    /*  8 */ ENSA_ANIM1_8,
    /*  9 */ ENSA_ANIM1_9,
    /* 10 */ ENSA_ANIM1_10,
    /* 11 */ ENSA_ANIM1_11
} EnSaAnimation1;

static AnimationFrameCountInfo anime_ct_data[] = {
    { &gSariaWaitArmsToSideAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
    { &gSariaLookUpArmExtendedAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaWaveAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaRunAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaWaitArmsToSideAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaLookOverShoulderAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaPlayingOcarinaAnim, 1.0f, ANIMMODE_LOOP, -10.0f },
    { &gSariaStopPlayingOcarinaAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaOcarinaToMouthAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaLinkLearnedSariasSongAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaReturnToOcarinaAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gSariaPlayingOcarinaAnim, 1.0f, ANIMMODE_LOOP, 0.0f },
};

typedef enum EnSaAnimation2 {
    /* 0 */ ENSA_ANIM2_0,
    /* 1 */ ENSA_ANIM2_1,
    /* 2 */ ENSA_ANIM2_2,
    /* 3 */ ENSA_ANIM2_3,
    /* 4 */ ENSA_ANIM2_4,
    /* 5 */ ENSA_ANIM2_5,
    /* 6 */ ENSA_ANIM2_6,
    /* 7 */ ENSA_ANIM2_7,
    /* 8 */ ENSA_ANIM2_8,
    /* 9 */ ENSA_ANIM2_9
} EnSaAnimation2;

static AnimationInfo animetbl[] = {
    { &gSariaTransitionHandsSideToChestToSideAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaTransitionHandsSideToBackAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -4.0f },
    { &gSariaRightArmExtendedWaitAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaHandsOutAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaStandHandsOnHipsAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaExtendRightArmAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaTransitionHandsSideToHipsAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaHandsBehindBackWaitAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -1.0f },
    { &gSariaHandsOnFaceAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, -1.0f },
    { &gSariaWaitArmsToSideAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, -8.0f },
};

static s16 calc_pad_on_cnt(EnSa* this, PlayState* play) {
    s16 textState = message_check(&play->msgCtx);

    if (this->unk_209 == TEXT_STATE_AWAITING_NEXT || this->unk_209 == TEXT_STATE_EVENT ||
        this->unk_209 == TEXT_STATE_CLOSING || this->unk_209 == TEXT_STATE_DONE_HAS_NEXT) {
        if (textState != this->unk_209) {
            this->unk_208++;
        }
    }
    this->unk_209 = textState;
    return textState;
}

u16 sa_set_message(PlayState* play, Actor* thisx) {
    EnSa* this = (EnSa*)thisx;
    u16 textId = get_mask_message(play, MASK_REACTION_SET_SARIA);

    if (textId != 0) {
        return textId;
    }
    if (CHECK_QUEST_ITEM(QUEST_SONG_SARIA)) {
        return 0x10AD;
    }
    if (CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
        this->unk_208 = 0;
        this->unk_209 = TEXT_STATE_NONE;
        if (GET_INFTABLE(INFTABLE_05)) {
            return 0x1048;
        } else {
            return 0x1047;
        }
    }
    if (GET_EVENTCHKINF(EVENTCHKINF_MIDO_DENIED_DEKU_TREE_ACCESS)) {
        this->unk_208 = 0;
        this->unk_209 = TEXT_STATE_NONE;
        if (GET_INFTABLE(INFTABLE_03)) {
            return 0x1032;
        } else {
            return 0x1031;
        }
    }
    if (GET_INFTABLE(INFTABLE_00)) {
        this->unk_208 = 0;
        this->unk_209 = TEXT_STATE_NONE;
        if (GET_INFTABLE(INFTABLE_01)) {
            return 0x1003;
        } else {
            return 0x1002;
        }
    }
    return 0x1001;
}

s16 sa_end_message(PlayState* play, Actor* thisx) {
    s16 talkState = NPC_TALK_STATE_TALKING;
    EnSa* this = (EnSa*)thisx;

    switch (calc_pad_on_cnt(this, play)) {
        case TEXT_STATE_CLOSING:
            switch (this->actor.textId) {
                case 0x1002:
                    SET_INFTABLE(INFTABLE_01);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x1031:
                    SET_EVENTCHKINF(EVENTCHKINF_03);
                    SET_INFTABLE(INFTABLE_03);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                case 0x1047:
                    SET_INFTABLE(INFTABLE_05);
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
                default:
                    talkState = NPC_TALK_STATE_IDLE;
                    break;
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }
    return talkState;
}

void sa_talk(EnSa* this, PlayState* play) {
    if (play->sceneId != SCENE_SACRED_FOREST_MEADOW ||
        ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) < 0x1555 ||
        this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        npc_talk(play, &this->actor, &this->interactInfo.talkState, this->collider.dim.radius + 30.0f,
                          sa_set_message, sa_end_message);
    }
}

#include "z_en_sa_anm.inc.c"

void sa_anime_ct(EnSa* this, s32 index) {
    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, 1.0f, 0.0f,
                     Si2_anime_end_frame(anime_ct_data[index].animation), anime_ct_data[index].mode,
                     anime_ct_data[index].morphFrames);
}

s32 sa_appearance_check(EnSa* this, PlayState* play) {
    if (z_common_data.save.cutsceneIndex >= 0xFFF0 && z_common_data.save.cutsceneIndex != 0xFFFD) {
        if (play->sceneId == SCENE_KOKIRI_FOREST) {
            return 4;
        }
        if (play->sceneId == SCENE_SACRED_FOREST_MEADOW) {
            return 5;
        }
    }
    if (play->sceneId == SCENE_SARIAS_HOUSE && !LINK_IS_ADULT &&
        INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY && !GET_EVENTCHKINF(EVENTCHKINF_40)) {
        return 1;
    }
    if (play->sceneId == SCENE_SACRED_FOREST_MEADOW && GET_EVENTCHKINF(EVENTCHKINF_40)) {
        return CHECK_QUEST_ITEM(QUEST_SONG_SARIA) ? 2 : 5;
    }
    if (play->sceneId == SCENE_KOKIRI_FOREST && !CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD)) {
        if (GET_INFTABLE(INFTABLE_00)) {
            return 1;
        }
        return 4;
    }
    return 0;
}

void sa_eye_move(EnSa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode = NPC_TRACKING_PLAYER_AUTO_TURN;

    if (play->sceneId == SCENE_KOKIRI_FOREST) {
        trackingMode = (this->actionFunc == sa_demo) ? NPC_TRACKING_NONE : NPC_TRACKING_FULL_BODY;
    }
    if (play->sceneId == SCENE_SACRED_FOREST_MEADOW) {
        trackingMode = (this->skelAnime.animation == &gSariaPlayingOcarinaAnim) ? NPC_TRACKING_NONE : NPC_TRACKING_HEAD;
    }
    if (play->sceneId == SCENE_SACRED_FOREST_MEADOW && this->actionFunc == sa_wait &&
        this->skelAnime.animation == &gSariaStopPlayingOcarinaAnim) {
        trackingMode = NPC_TRACKING_NONE;
    }
    if (play->sceneId == SCENE_SACRED_FOREST_MEADOW && this->actionFunc == sa_demo &&
        this->skelAnime.animation == &gSariaOcarinaToMouthAnim) {
        trackingMode = NPC_TRACKING_NONE;
    }
    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = 4.0f;
    eye_moveM(&this->actor, &this->interactInfo, 2, trackingMode);
}

s32 sa_eye_control_oca_play(EnSa* this) {
    if (this->skelAnime.animation != &gSariaPlayingOcarinaAnim &&
        this->skelAnime.animation != &gSariaOcarinaToMouthAnim) {
        return 0;
    }
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        return 0;
    }
    this->unk_20E = 0;
    if (this->rightEyeIndex != SARIA_EYE_CLOSED) {
        return 0;
    }
    return 1;
}

void sa_eye_control(EnSa* this) {
    s16 phi_v1;

    if (sa_eye_control_oca_play(this) == 0) {
        if (this->unk_20E == 0) {
            phi_v1 = 0;
        } else {
            this->unk_20E--;
            phi_v1 = this->unk_20E;
        }
        if (phi_v1 == 0) {
            this->rightEyeIndex++;
            if (this->rightEyeIndex < SARIA_EYE_SUPRISED) {
                this->leftEyeIndex = this->rightEyeIndex;
            } else {
                this->unk_20E = get_random_timer(30, 30);
                this->leftEyeIndex = SARIA_EYE_OPEN;
                this->rightEyeIndex = this->leftEyeIndex;
            }
        }
    }
}

static void demo_start_pos_set(CsCmdActorCue* cue, Vec3f* dst) {
    dst->x = cue->startPos.x;
    dst->y = cue->startPos.y;
    dst->z = cue->startPos.z;
}

static void demo_end_pos_set(CsCmdActorCue* cue, Vec3f* dst) {
    dst->x = cue->endPos.x;
    dst->y = cue->endPos.y;
    dst->z = cue->endPos.z;
}

void En_Sa_Actor_ct(Actor* thisx, PlayState* play) {
    EnSa* this = (EnSa*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 12.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gSariaSkel, NULL, this->jointTable, this->morphTable, 17);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnSaAtInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &SaStatusData);

    switch (sa_appearance_check(this, play)) {
        case 2:
            sa_anime_ct(this, ENSA_ANIM1_11);
            this->actionFunc = sa_wait;
            break;
        case 5:
            sa_anime_ct(this, ENSA_ANIM1_11);
            this->actionFunc = sa_oca_wait;
            break;
        case 1:
            this->actor.gravity = -1.0f;
            sa_anime_ct(this, ENSA_ANIM1_0);
            this->actionFunc = sa_wait;
            break;
        case 4:
            this->unk_210 = 0;
            this->actor.gravity = -1.0f;
            play->csCtx.script = SEGMENTED_TO_VIRTUAL(gKokiriForestSariaGreetingCs);
            z_common_data.cutsceneTrigger = 1;
            sa_anime_ct(this, ENSA_ANIM1_4);
            this->actionFunc = sa_demo;
            break;
        case 3:
            this->unk_210 = 0;
            this->actor.gravity = -1.0f;
            sa_anime_ct(this, ENSA_ANIM1_0);
            this->actionFunc = sa_demo;
            break;
        case 0:
            Actor_delete(&this->actor);
            return;
    }

    Actor_set_scale(&this->actor, 0.01f);

    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    this->alpha = 255;
    this->unk_21A = this->actor.shape.rot;

    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_ELF, this->actor.world.pos.x,
                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, FAIRY_KOKIRI);
}

void En_Sa_Actor_dt(Actor* thisx, PlayState* play) {
    EnSa* this = (EnSa*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void sa_wait(EnSa* this, PlayState* play) {
    if (play->sceneId == SCENE_KOKIRI_FOREST) {
        if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
            switch (this->actor.textId) {
                case 0x1002:
                    if (this->unk_208 == 0 && this->unk_20B != 1) {
                        set_play_anime_mode(this, 1);
                        this->mouthIndex = 1;
                    }
                    if (this->unk_208 == 2 && this->unk_20B != 2) {
                        set_play_anime_mode(this, 2);
                        this->mouthIndex = 1;
                    }
                    if (this->unk_208 == 5) {
                        this->mouthIndex = 0;
                    }
                    break;
                case 0x1003:
                    if (this->unk_208 == 0 && this->unk_20B != 4) {
                        set_play_anime_mode(this, 4);
                    }
                    break;
                case 0x1031:
                    if (this->unk_208 == 0 && this->unk_20B != 4 &&
                        this->skelAnime.animation == &gSariaHandsBehindBackWaitAnim) {
                        set_play_anime_mode(this, 4);
                        this->mouthIndex = 3;
                    }
                    if (this->unk_208 == 2 && this->unk_20B != 5) {
                        set_play_anime_mode(this, 5);
                        this->mouthIndex = 2;
                    }
                    if (this->unk_208 == 4 && this->unk_20B != 6) {
                        set_play_anime_mode(this, 6);
                        this->mouthIndex = 0;
                    }
                    break;
                case 0x1032:
                    if (this->unk_208 == 0 && this->unk_20B != 4 &&
                        this->skelAnime.animation == &gSariaHandsBehindBackWaitAnim) {
                        set_play_anime_mode(this, 4);
                    }
                    break;
                case 0x1047:
                    if (this->unk_208 == 1 && this->unk_20B != 7) {
                        set_play_anime_mode(this, 7);
                    }
                    break;
                case 0x1048:
                    if (this->unk_208 == 0 && this->unk_20B != 7) {
                        set_play_anime_mode(this, 7);
                    }
                    break;
            }
        } else if (!CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) &&
                   (GET_INFTABLE(INFTABLE_01) || GET_INFTABLE(INFTABLE_03))) {
            if (this->unk_20B != 3) {
                set_play_anime_mode(this, 3);
            }
        } else {
            set_play_anime_mode(this, 0);
        }
        sa_chg_anime(this);
    }
    if (this->skelAnime.animation == &gSariaStopPlayingOcarinaAnim) {
        this->skelAnime.playSpeed = -1.0f;
        if ((s32)this->skelAnime.curFrame == 0) {
            sa_anime_ct(this, ENSA_ANIM1_6);
        }
    }
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE && play->sceneId == SCENE_SACRED_FOREST_MEADOW) {
        Skeleton_Info2_init(&this->skelAnime, &gSariaStopPlayingOcarinaAnim, 1.0f, 0.0f, 10.0f, ANIMMODE_ONCE, -10.0f);
        this->actionFunc = sa_talking;
    }
}

void sa_talking(EnSa* this, PlayState* play) {
    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        return;
    }

    Skeleton_Info2_init(&this->skelAnime, &gSariaStopPlayingOcarinaAnim, 0.0f, 10.0f, 0.0f, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = sa_wait;
}

void sa_oca_wait(EnSa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!(player->actor.world.pos.z >= -2220.0f) && !Game_play_demo_mode_check(play)) {
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(gMeadowSariasSongCs);
        z_common_data.cutsceneTrigger = 1;
        this->actionFunc = sa_demo;
    }
}

void sa_demo(EnSa* this, PlayState* play) {
    s16 phi_v0;
    Vec3f startPos;
    Vec3f endPos;
    Vec3f D_80AF7448 = { 0.0f, 0.0f, 0.0f };
    CsCmdActorCue* cue;
    f32 temp_f0;
    f32 gravity;

    if (play->csCtx.state == CS_STATE_IDLE) {
        this->actionFunc = sa_demo_end;
        return;
    }

    cue = play->csCtx.actorCues[1];

    if (cue != NULL) {
        demo_start_pos_set(cue, &startPos);
        demo_end_pos_set(cue, &endPos);

        if (this->unk_210 == 0) {
            this->actor.world.pos = startPos;
        }
        if (this->unk_210 != cue->id) {
            switch (cue->id) {
                case 2:
                    this->mouthIndex = 1;
                    break;
                case 9:
                    this->mouthIndex = 1;
                    break;
                default:
                    this->mouthIndex = 0;
                    break;
            }
            sa_anime_ct(this, cue->id);
            this->unk_210 = cue->id;
        }

        if (phi_v0) {}

        if (cue->id == 3) {
            if (this->unk_20C == 0) {
                phi_v0 = 0;
            } else {
                this->unk_20C--;
                phi_v0 = this->unk_20C;
            }
            if (phi_v0 == 0) {
                Nai_FxFlagEntry(NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_DIRT, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
                this->unk_20C = 8;
            }
        }
        this->actor.shape.rot.x = cue->rot.x;
        this->actor.shape.rot.y = cue->rot.y;
        this->actor.shape.rot.z = cue->rot.z;
        this->actor.velocity = D_80AF7448;

        if (play->csCtx.curFrame < cue->endFrame) {
            temp_f0 = cue->endFrame - cue->startFrame;
            this->actor.velocity.x = (endPos.x - startPos.x) / temp_f0;
            this->actor.velocity.y = (endPos.y - startPos.y) / temp_f0;
            gravity = this->actor.gravity;
            if (play->sceneId == SCENE_SACRED_FOREST_MEADOW) {
                gravity = 0.0f;
            }
            this->actor.velocity.y += gravity;
            if (this->actor.velocity.y < this->actor.minVelocityY) {
                this->actor.velocity.y = this->actor.minVelocityY;
            }
            this->actor.velocity.z = (endPos.z - startPos.z) / temp_f0;
        }
    }
}

void sa_demo_end(EnSa* this, PlayState* play) {
    if (play->sceneId == SCENE_SACRED_FOREST_MEADOW) {
        item_get_setting(play, ITEM_SONG_SARIA);
        sa_anime_ct(this, ENSA_ANIM1_6);
    }

    if (play->sceneId == SCENE_KOKIRI_FOREST) {
        sa_anime_ct(this, ENSA_ANIM1_4);
        this->actor.world.pos = this->actor.home.pos;
        this->actor.world.rot = this->unk_21A;
        this->mouthIndex = 0;
        SET_INFTABLE(INFTABLE_00);
    }

    this->actionFunc = sa_wait;
}

void En_Sa_Actor_move(Actor* thisx, PlayState* play) {
    EnSa* this = (EnSa*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (this->skelAnime.animation == &gSariaOcarinaToMouthAnim &&
        this->skelAnime.curFrame >= Si2_anime_end_frame(&gSariaOcarinaToMouthAnim)) {
        sa_anime_ct(this, ENSA_ANIM1_6);
    }

    if (this->actionFunc != sa_demo) {
        this->alpha = kokiri_alpha_set(&this->actor, play, this->alpha, 400.0f);
    } else {
        this->alpha = 255;
    }

    this->actor.shape.shadowAlpha = this->alpha;

    if (this->actionFunc == sa_demo) {
        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.z += this->actor.velocity.z;
    } else {
        Actor_position_move(&this->actor);
    }

    if (play->sceneId != SCENE_SACRED_FOREST_MEADOW) {
        Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    }

    sa_eye_control(this);
    this->actionFunc(this, play);
    sa_talk(this, play);
    sa_eye_move(this, play);
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnSa* this = (EnSa*)thisx;
    s32 pad;
    Vec3s limbRot;

    if (limbIndex == 16) {
        Matrix_translate(900.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        limbRot = this->interactInfo.headRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_translate(-900.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == 9) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateY(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
    }

    if (play->sceneId == SCENE_SACRED_FOREST_MEADOW && limbIndex == 15) {
        *dList = gSariaRightHandAndOcarinaDL;
    }

    return 0;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnSa* this = (EnSa*)thisx;
    Vec3f D_80AF7454 = { 400.0, 0.0f, 0.0f };

    if (limbIndex == 16) {
        Matrix_Position(&D_80AF7454, &this->actor.focus.pos);
    }
}

void En_Sa_Actor_draw(Actor* thisx, PlayState* play) {
    static void* mouth_txt[] = {
        gSariaMouthClosed2Tex,  gSariaMouthSmilingOpenTex, gSariaMouthFrowningTex,
        gSariaMouthSuprisedTex, gSariaMouthClosedTex,
    };
    static void* eye_txt[] = {
        gSariaEyeOpenTex, gSariaEyeHalfTex, gSariaEyeClosedTex, gSariaEyeSuprisedTex, gSariaEyeSadTex,
    };
    EnSa* this = (EnSa*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_sa.c", 1444);

    if (this->alpha == 255) {
        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->rightEyeIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eye_txt[this->leftEyeIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouth_txt[this->mouthIndex]));
        no_clarity(play, &this->skelAnime, before_display, after_display, &this->actor, this->alpha);
    } else if (this->alpha != 0) {
        gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->rightEyeIndex]));
        gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eye_txt[this->leftEyeIndex]));
        gSPSegment(POLY_XLU_DISP++, 0x0A, SEGMENTED_TO_VIRTUAL(mouth_txt[this->mouthIndex]));
        clarity(play, &this->skelAnime, before_display, after_display, &this->actor, this->alpha);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_sa.c", 1497);
}
