/*
 * File: z_en_horse_link_child.c
 * Overlay: ovl_En_Horse_Link_Child
 * Description: Young Epona
 */

#include "global.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "rand.h"
#include "regs.h"
#include "sfx.h"
#include "z64actor.h"
#include "z64horse.h"
#include "z64player.h"
#include "z64play.h"
#include "z_en_horse_link_child.h"
#include "assets/objects/object_horse_link_child/object_horse_link_child.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void En_HLC_Actor_ct(Actor* thisx, PlayState* play);
void En_HLC_Actor_dt(Actor* thisx, PlayState* play);
void En_HLC_Actor_move(Actor* thisx, PlayState* play);
void En_HLC_Actor_display(Actor* thisx, PlayState* play);

void En_HLC_Actor_mode_search_player_init_set_stop(EnHorseLinkChild* this);
void En_HLC_Actor_mode_epona1_init_set_stop(EnHorseLinkChild* this);
void En_HLC_Actor_mode_epona2_wait_init(EnHorseLinkChild* this);
void En_HLC_Actor_mode_epona2_trace_p_init(EnHorseLinkChild* this);

ActorProfile En_Horse_Link_Child_Profile = {
    /**/ ACTOR_EN_HORSE_LINK_CHILD,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HORSE_LINK_CHILD,
    /**/ sizeof(EnHorseLinkChild),
    /**/ En_HLC_Actor_ct,
    /**/ En_HLC_Actor_dt,
    /**/ En_HLC_Actor_move,
    /**/ En_HLC_Actor_display,
};

static AnimationHeader* skin_hlc_anim_tbl[] = {
    &gChildEponaIdleAnim,     &gChildEponaWhinnyAnim,    &gChildEponaWalkingAnim,
    &gChildEponaTrottingAnim, &gChildEponaGallopingAnim,
};

static ColliderCylinderInitType1 HLCOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
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
    { 20, 100, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit HLCOcInfoJntSphElemData[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_NONE,
            OCELEM_ON,
        },
        { 13, { { 0, 0, 0 }, 10 }, 100 },
    },
};

static ColliderJntSphInit HLCOcInfoJntSphData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1 | OC2_UNK1,
        COLSHAPE_JNTSPH,
    },
    1,
    HLCOcInfoJntSphElemData,
};

static CollisionCheckInfoInit HLCStatusData = { 10, 35, 100, MASS_HEAVY };

void En_HLC_Actor_setWalkSound(EnHorseLinkChild* this) {
    static s32 sound_on_frames[] = { 1, 19 };

    if ((this->skin.skelAnime.curFrame > sound_on_frames[this->unk_1F0]) &&
        !((this->unk_1F0 == 0) && (this->skin.skelAnime.curFrame > sound_on_frames[1]))) {
        Nai_FxFlagEntry(NA_SE_EV_KID_HORSE_WALK, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        this->unk_1F0++;
        if (this->unk_1F0 >= ARRAY_COUNT(sound_on_frames)) {
            this->unk_1F0 = 0;
        }
    }
}

void En_HLC_Actor_setSound(EnHorseLinkChild* this) {
    if (this->animationIdx == 2) {
        En_HLC_Actor_setWalkSound(this);
    } else if (this->skin.skelAnime.curFrame == 0.0f) {
        if ((this->animationIdx == 3) || (this->animationIdx == 4)) {
            Nai_FxFlagEntry(NA_SE_EV_KID_HORSE_RUN, &this->actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else if (this->animationIdx == 1) {
            if (fqrand() > 0.5f) {
                Nai_FxFlagEntry(NA_SE_EV_KID_HORSE_GROAN, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
            } else {
                Nai_FxFlagEntry(NA_SE_EV_KID_HORSE_NEIGH, &this->actor.projectedPos, 4,
                                     &_dummy_one, &_dummy_one, &_dummy_zero_s8);
            }
        }
    }
}

static f32 hlc_anim_speed[] = { 1.0f, 1.0f, 1.5f, 1.5f, 1.5f };

f32 En_HLC_calcAnimSpeed(EnHorseLinkChild* this) {
    f32 result;

    if (this->animationIdx == 2) {
        result = hlc_anim_speed[this->animationIdx] * this->actor.speed * (1.0f / 2.0f);
    } else if (this->animationIdx == 3) {
        result = hlc_anim_speed[this->animationIdx] * this->actor.speed * (1.0f / 3.0f);
    } else if (this->animationIdx == 4) {
        result = hlc_anim_speed[this->animationIdx] * this->actor.speed * (1.0f / 5.0f);
    } else {
        result = hlc_anim_speed[this->animationIdx];
    }

    return result;
}

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeScale, 1200, ICHAIN_STOP),
};

void En_HLC_Actor_ct(Actor* thisx, PlayState* play) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;
    s32 pad;

    ValueSet_process(&this->actor, value_init);
    Actor_set_scale(&this->actor, 0.005f);
    this->actor.gravity = -3.5f;
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_horse, 20.0f);
    this->actor.speed = 0.0f;
    this->action = 1;
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;
    Skin_AnimationWorkBuffer2_ct(play, &this->skin, &gChildEponaSkel, &gChildEponaGallopingAnim);
    this->animationIdx = 0;
    Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, skin_hlc_anim_tbl[0]);
    ClObjPipe_ct(play, &this->bodyCollider);
    ClObjPipe_set3(play, &this->bodyCollider, &this->actor, &HLCOcInfoData);
    ClObjJntSph_ct(play, &this->headCollider);
    ClObjJntSph_set5_nzm(play, &this->headCollider, &this->actor, &HLCOcInfoJntSphData, this->headElements);
    CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &HLCStatusData);
    this->unk_1F0 = 0;
    this->eyeTexIndex = 0;

    if (IS_CUTSCENE_LAYER) {
        En_HLC_Actor_mode_epona1_init_set_stop(this);
    } else if (play->sceneId == SCENE_LON_LON_RANCH) {
        if (!event_check(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE)) {
            Actor_delete(&this->actor);
            return;
        }
        this->unk_2A0 = GET_EVENTCHKINF(EVENTCHKINF_CAN_LEARN_EPONAS_SONG);
        En_HLC_Actor_mode_epona1_init_set_stop(this);
    } else {
        En_HLC_Actor_mode_epona1_init_set_stop(this);
    }

    this->actor.home.rot.z = this->actor.world.rot.z = this->actor.shape.rot.z = 0;
}

void En_HLC_Actor_dt(Actor* thisx, PlayState* play) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;

    Skin_AnimationWorkBuffer2_dt(play, &this->skin);
    ClObjPipe_dt(play, &this->bodyCollider);
    ClObjJntSph_dt_nzf(play, &this->headCollider);
}

void En_HLC_Actor_mode_anim_loop_init(EnHorseLinkChild* this) {
    this->action = 0;
    this->animationIdx++;
    if (this->animationIdx >= ARRAY_COUNT(skin_hlc_anim_tbl)) {
        this->animationIdx = 0;
    }

    Skeleton_Info2_init_standard_stop(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx]);
    this->skin.skelAnime.playSpeed = En_HLC_calcAnimSpeed(this);
}

void En_HLC_Actor_mode_anim_loop(EnHorseLinkChild* this, PlayState* play) {
    this->actor.speed = 0.0f;
    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        En_HLC_Actor_mode_anim_loop_init(this);
    }
}

void En_HLC_Actor_mode_search_wait_init(EnHorseLinkChild* this, s32 newAnimationIdx) {
    this->action = 2;
    this->actor.speed = 0.0f;

    if (!((newAnimationIdx == 0) || (newAnimationIdx == 1))) {
        newAnimationIdx = 0;
    }

    if (this->animationIdx != newAnimationIdx) {
        this->animationIdx = newAnimationIdx;
        Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                         Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
    }
}

void En_HLC_Actor_mode_search_wait(EnHorseLinkChild* this, PlayState* play) {
    f32 distFromLink;
    s32 newAnimationIdx;

    distFromLink = Actor_search_actor_distanceXZ(&this->actor, &GET_PLAYER(play)->actor);

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        if ((distFromLink < 1000.0f) && (distFromLink > 70.0f)) {
            En_HLC_Actor_mode_search_player_init_set_stop(this);
        } else {
            newAnimationIdx = this->animationIdx == 1 ? 0 : 1;
            if (this->animationIdx != newAnimationIdx) {
                this->animationIdx = newAnimationIdx;
                Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                                 Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
            } else {
                Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                                 Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0);
            }
        }
    }
}

void En_HLC_Actor_mode_search_player_init_set_stop(EnHorseLinkChild* this) {
    this->action = 1;
    this->animationIdx = 0;
    this->actor.speed = 0.0f;
    Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                     Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
}

void En_HLC_Actor_mode_search_player(EnHorseLinkChild* this, PlayState* play) {
    s16 yawDiff;
    f32 distFromLink;
    s32 newAnimationIdx;

    if ((this->animationIdx == 4) || (this->animationIdx == 3) || (this->animationIdx == 2)) {
        yawDiff = Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;

        if (yawDiff > 0x12C) {
            this->actor.world.rot.y += 0x12C;
        } else if (yawDiff < -0x12C) {
            this->actor.world.rot.y -= 0x12C;
        } else {
            this->actor.world.rot.y += yawDiff;
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        distFromLink = Actor_search_actor_distanceXZ(&this->actor, &GET_PLAYER(play)->actor);

        if (distFromLink > 1000.0f) {
            En_HLC_Actor_mode_search_wait_init(this, 0);
            return;
        } else if ((distFromLink < 1000.0f) && (distFromLink >= 300.0f)) {
            newAnimationIdx = 4;
            this->actor.speed = 6.0f;
        } else if ((distFromLink < 300.0f) && (distFromLink >= 150.0f)) {
            newAnimationIdx = 3;
            this->actor.speed = 4.0f;
        } else if ((distFromLink < 150.0f) && (distFromLink >= 70.0f)) {
            newAnimationIdx = 2;
            this->actor.speed = 2.0f;
            this->unk_1F0 = 0;
        } else {
            En_HLC_Actor_mode_search_wait_init(this, 1);
            return;
        }

        if (this->animationIdx != newAnimationIdx) {
            this->animationIdx = newAnimationIdx;
            Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                             Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
        } else {
            Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                             Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
        }
    }
}

void En_HLC_Actor_mode_epona1_init_set_stop(EnHorseLinkChild* this) {
    this->action = 3;
    this->animationIdx = 0;
    this->actor.speed = 0.0f;
    Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                     Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
}

void En_HLC_Actor_mode_epona1_calc_angle(EnHorseLinkChild* this, PlayState* play) {
    Player* player;
    s16 yawDiff;
    s32 yawSign;
    s32 yawOffset;

    if ((this->animationIdx == 4) || (this->animationIdx == 3) || (this->animationIdx == 2)) {
        player = GET_PLAYER(play);

        if (Math3DLength(&player->actor.world.pos, &this->actor.home.pos) < 250.0f) {
            yawDiff = player->actor.shape.rot.y;
            yawSign = Actor_search_actor_angleY(&this->actor, &player->actor) > 0 ? 1 : -1;
            yawOffset = yawSign << 0xE;
            yawDiff += yawOffset;
        } else {
            yawDiff = search_position_angleY(&this->actor.world.pos, &this->actor.home.pos) - this->actor.world.rot.y;
        }

        if (yawDiff > 0x12C) {
            this->actor.world.rot.y += 0x12C;
        } else if (yawDiff < -0x12C) {
            this->actor.world.rot.y -= 0x12C;
        } else {
            this->actor.world.rot.y += yawDiff;
        }

        this->actor.shape.rot.y = this->actor.world.rot.y;
    }
}

void En_HLC_Actor_mode_epona1(EnHorseLinkChild* this, PlayState* play) {
    Player* player;
    f32 distFromLink;
    s32 animationEnded;
    s32 newAnimationIdx;

    En_HLC_Actor_mode_epona1_calc_angle(this, play);
    player = GET_PLAYER(play);
    distFromLink = Actor_search_actor_distanceXZ(&this->actor, &player->actor);

    if (z_common_data.save.entranceIndex == ENTR_LON_LON_RANCH_1) {
        Nai_FxFlagEntry(NA_SE_EV_KID_HORSE_NEIGH, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        En_HLC_Actor_mode_epona2_trace_p_init(this);
        return;
    }

    if ((GET_EVENTCHKINF(EVENTCHKINF_CAN_LEARN_EPONAS_SONG) && R_EPONAS_SONG_PLAYED) ||
        ((play->sceneId == SCENE_LON_LON_RANCH) && (z_common_data.save.cutsceneIndex == 0xFFF1))) {
        En_HLC_Actor_mode_epona2_wait_init(this);
    } else {
        this->unk_2A0 = GET_EVENTCHKINF(EVENTCHKINF_CAN_LEARN_EPONAS_SONG);
    }

    newAnimationIdx = this->animationIdx;
    animationEnded = Skeleton_Info2_anime_play(&this->skin.skelAnime);
    if (animationEnded || (this->animationIdx == 1) || (this->animationIdx == 0)) {
        if (GET_EVENTCHKINF(EVENTCHKINF_TALKED_TO_CHILD_MALON_AT_RANCH)) {
            f32 distFromHome = Math3DLength(&this->actor.world.pos, &this->actor.home.pos);
            f32 distLinkFromHome = Math3DLength(&player->actor.world.pos, &this->actor.home.pos);

            if (distLinkFromHome > 250.0f) {
                if (distFromHome >= 300.0f) {
                    newAnimationIdx = 4;
                    this->actor.speed = 6.0f;
                } else if ((distFromHome < 300.0f) && (distFromHome >= 150.0f)) {
                    newAnimationIdx = 3;
                    this->actor.speed = 4.0f;
                } else if ((distFromHome < 150.0f) && (distFromHome >= 70.0f)) {
                    newAnimationIdx = 2;
                    this->actor.speed = 2.0f;
                    this->unk_1F0 = 0;
                } else {
                    this->actor.speed = 0.0f;
                    if (this->animationIdx == 0) {
                        newAnimationIdx = animationEnded == true ? 1 : 0;
                    } else {
                        newAnimationIdx = animationEnded == true ? 0 : 1;
                    }
                }
            } else {
                if (distFromLink < 200.0f) {
                    newAnimationIdx = 4;
                    this->actor.speed = 6.0f;
                } else if (distFromLink < 300.0f) {
                    newAnimationIdx = 3;
                    this->actor.speed = 4.0f;
                } else if (distFromLink < 400.0f) {
                    newAnimationIdx = 2;
                    this->actor.speed = 2.0f;
                    this->unk_1F0 = 0;
                } else {
                    this->actor.speed = 0.0f;
                    if (this->animationIdx == 0) {
                        newAnimationIdx = animationEnded == true ? 1 : 0;
                    } else {
                        newAnimationIdx = animationEnded == true ? 0 : 1;
                    }
                }
            }
        } else {
            this->actor.speed = 0.0f;
            if (this->animationIdx == 0) {
                newAnimationIdx = animationEnded == true ? 1 : 0;
            } else {
                newAnimationIdx = animationEnded == true ? 0 : 1;
            }
        }
    }

    if ((this->animationIdx != newAnimationIdx) || (animationEnded == true)) {
        this->animationIdx = newAnimationIdx;
        Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                         Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
    } else {
        Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this),
                         this->skin.skelAnime.curFrame, Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), 2,
                         0.0f);
    }
}

void En_HLC_Actor_mode_epona2_wait_init(EnHorseLinkChild* this) {
    this->action = 5;
    this->animationIdx = fqrand() > 0.5f ? 0 : 1;
    R_EPONAS_SONG_PLAYED = false;
    Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                     Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
}

void En_HLC_Actor_mode_epona2_wait(EnHorseLinkChild* this, PlayState* play) {
    s16 yawDiff;

    if (R_EPONAS_SONG_PLAYED) {
        R_EPONAS_SONG_PLAYED = false;
        Nai_FxFlagEntry(NA_SE_EV_KID_HORSE_NEIGH, &this->actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        En_HLC_Actor_mode_epona2_trace_p_init(this);
    } else {
        this->actor.speed = 0.0f;
        yawDiff = Actor_search_actor_angleY(&this->actor, &GET_PLAYER(play)->actor) - this->actor.world.rot.y;
        // 0.7071 = cos(pi/4)
        if ((cos_s(yawDiff) < 0.7071f) && (this->animationIdx == 2)) {
            horse_rot_trace_pos(&this->actor, &GET_PLAYER(play)->actor.world.pos, 300);
        }

        if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
            if (cos_s(yawDiff) < 0.0f) {
                this->animationIdx = 2;
                Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], hlc_anim_speed[this->animationIdx],
                                 0.0f, Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
            } else {
                En_HLC_Actor_mode_epona2_wait_init(this);
            }
        }
    }
}

void En_HLC_Actor_mode_epona2_trace_p_init(EnHorseLinkChild* this) {
    this->timer = 0;
    this->action = 4;
    this->animationIdx = 2;
    this->unk_1E8 = false;
    this->actor.speed = 2.0f;
    Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                     Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
}

void En_HLC_Actor_mode_epona2_trace_p(EnHorseLinkChild* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 dist;
    s32 newAnimationIdx;

    this->timer++;
    if (this->timer > 300) {
        this->unk_1E8 = true;
    }

    if ((this->animationIdx == 4) || (this->animationIdx == 3) || (this->animationIdx == 2)) {
        if (!this->unk_1E8) {
            horse_rot_trace_pos(&this->actor, &player->actor.world.pos, 300);
        } else {
            horse_rot_trace_pos(&this->actor, &this->actor.home.pos, 300);
        }
    }

    if (Skeleton_Info2_anime_play(&this->skin.skelAnime)) {
        if (!this->unk_1E8) {
            dist = Actor_search_actor_distanceXZ(&this->actor, &GET_PLAYER(play)->actor);
        } else {
            dist = Math3DLength(&this->actor.world.pos, &this->actor.home.pos);
        }

        if (!this->unk_1E8) {
            if (dist >= 300.0f) {
                newAnimationIdx = 4;
                this->actor.speed = 6.0f;
            } else if (dist >= 150.0f) {
                newAnimationIdx = 3;
                this->actor.speed = 4.0f;
            } else {
                newAnimationIdx = 2;
                this->actor.speed = 2.0f;
                this->unk_1F0 = 0;
            }
        } else {
            if (dist >= 300.0f) {
                newAnimationIdx = 4;
                this->actor.speed = 6.0f;
            } else if (dist >= 150.0f) {
                newAnimationIdx = 3;
                this->actor.speed = 4.0f;
            } else if (dist >= 70.0f) {
                newAnimationIdx = 2;
                this->actor.speed = 2.0f;
                this->unk_1F0 = 0;
            } else {
                En_HLC_Actor_mode_epona2_wait_init(this);
                return;
            }
        }

        if (this->animationIdx != newAnimationIdx) {
            this->animationIdx = newAnimationIdx;
            Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                             Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, -5.0f);
        } else {
            Skeleton_Info2_init(&this->skin.skelAnime, skin_hlc_anim_tbl[this->animationIdx], En_HLC_calcAnimSpeed(this), 0.0f,
                             Si2_anime_end_frame(skin_hlc_anim_tbl[this->animationIdx]), ANIMMODE_ONCE, 0.0f);
        }
    }
}

static EnHorseLinkChildActionFunc mode_function[] = {
    En_HLC_Actor_mode_anim_loop, En_HLC_Actor_mode_search_player, En_HLC_Actor_mode_search_wait, En_HLC_Actor_mode_epona1, En_HLC_Actor_mode_epona2_trace_p, En_HLC_Actor_mode_epona2_wait,
};

static void* eye_txt[] = { gChildEponaEyeOpenTex, gChildEponaEyeHalfTex, gChildEponaEyeCloseTex };
static u8 mepachi_tbl[] = { 0, 1, 2, 1 };

void En_HLC_Actor_move(Actor* thisx, PlayState* play) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;
    s32 pad;

    mode_function[this->action](this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 20.0f, 55.0f, 100.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);

    if ((play->sceneId == SCENE_LON_LON_RANCH) && (this->actor.world.pos.z < -2400.0f)) {
        this->actor.world.pos.z = -2400.0f;
    }

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 70.0f;

    if ((fqrand() < 0.025f) && (this->eyeTexIndex == 0)) {
        this->eyeTexIndex++;
    } else if (this->eyeTexIndex > 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= ARRAY_COUNT(mepachi_tbl)) {
            this->eyeTexIndex = 0;
        }
    }

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->bodyCollider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->bodyCollider.base);
    En_HLC_Actor_setSound(this);
}

void En_HLC_calc(Actor* thisx, PlayState* play, Skin* skin) {
    Vec3f center;
    Vec3f newCenter;
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;
    s32 i;

    for (i = 0; i < this->headCollider.count; i++) {
        center.x = this->headCollider.elements[i].dim.modelSphere.center.x;
        center.y = this->headCollider.elements[i].dim.modelSphere.center.y;
        center.z = this->headCollider.elements[i].dim.modelSphere.center.z;
        Skin_MatrixPosition2_gfx(skin, this->headCollider.elements[i].dim.limb, &center, &newCenter);
        this->headCollider.elements[i].dim.worldSphere.center.x = newCenter.x;
        this->headCollider.elements[i].dim.worldSphere.center.y = newCenter.y;
        this->headCollider.elements[i].dim.worldSphere.center.z = newCenter.z;
        this->headCollider.elements[i].dim.worldSphere.radius =
            this->headCollider.elements[i].dim.modelSphere.radius * this->headCollider.elements[i].dim.scale;
    }

    //! @bug see relevant comment in `EnHorse_SkinCallback1`
    CollisionCheck_setOC(play, &play->colChkCtx, &this->headCollider.base);
}

s32 En_HLC_joint_proc(Actor* thisx, PlayState* play, s32 arg2, Skin* skin) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_horse_link_child.c", 1467);

    if (arg2 == 0xD) {
        u8 index = mepachi_tbl[this->eyeTexIndex];

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[index]));
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_horse_link_child.c", 1479);

    return 1;
}

void En_HLC_Actor_display(Actor* thisx, PlayState* play) {
    EnHorseLinkChild* this = (EnHorseLinkChild*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Skin_disp3(&this->actor, play, &this->skin, En_HLC_calc, En_HLC_joint_proc, true);
}
