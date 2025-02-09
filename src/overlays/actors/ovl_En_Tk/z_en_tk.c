/*
 * File: z_en_tk.c
 * Overlay: ovl_En_Tk
 * Description: Dampe NPC from "Dampe's Heart-Pounding Gravedigging Tour"
 */

#include "z_en_tk.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_tk/object_tk.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Tk_Actor_ct(Actor* thisx, PlayState* play);
void En_Tk_Actor_dt(Actor* thisx, PlayState* play);
void En_Tk_Actor_move(Actor* thisx, PlayState* play);
void En_Tk_Actor_draw(Actor* thisx, PlayState* play);

s32 tk_tag_search(EnTk* this, PlayState* play);
void tk_matsu(EnTk* this, PlayState* play);
void tk_aruku(EnTk* this, PlayState* play);
void tk_demo(EnTk* this, PlayState* play);

ActorProfile En_Tk_Profile = {
    /**/ ACTOR_EN_TK,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TK,
    /**/ sizeof(EnTk),
    /**/ En_Tk_Actor_ct,
    /**/ En_Tk_Actor_dt,
    /**/ En_Tk_Actor_move,
    /**/ En_Tk_Actor_draw,
};

#include "z_en_tk_eff.inc.c"

/** z_en_tk_eff.c ends here probably **/

static ColliderCylinderInit EnTkAtInfoData = {
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
    { 30, 52, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 TkStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

void tk_matsu_ct(EnTk* this, PlayState* play) {
    AnimationHeader* anim = &gDampeRestAnim;

    Skeleton_Info2_init(&this->skelAnime, anim, 1.0f, 0.0f, Si2_anime_end_frame(&gDampeRestAnim), ANIMMODE_LOOP,
                     -10.0f);

    this->actionCountdown = get_random_timer(60, 60);
    this->actor.speed = 0.0f;
}

void tk_aruku_ct(EnTk* this, PlayState* play) {
    AnimationHeader* anim = &gDampeWalkAnim;

    Skeleton_Info2_init(&this->skelAnime, anim, 1.0f, 0.0f, Si2_anime_end_frame(&gDampeRestAnim), ANIMMODE_LOOP,
                     -10.0f);

    this->actionCountdown = get_random_timer(240, 240);
}

void tk_demo_ct(EnTk* this, PlayState* play) {
    AnimationHeader* anim = &gDampeDigAnim;

    Skeleton_Info2_init(&this->skelAnime, anim, 1.0f, 0.0f, Si2_anime_end_frame(&gDampeDigAnim), ANIMMODE_LOOP, -10.0f);

    if (tk_tag_search(this, play) >= 0) {
        this->validDigHere = 1;
    }
}

void tk_eye_control(EnTk* this) {
    if (DECR(this->blinkCountdown) == 0) {
        this->eyeTextureIdx++;
        if (this->eyeTextureIdx > 2) {
            this->blinkCycles--;
            if (this->blinkCycles < 0) {
                this->blinkCountdown = get_random_timer(30, 30);
                this->blinkCycles = 2;
                if (fqrand() > 0.5f) {
                    this->blinkCycles++;
                }
            }
            this->eyeTextureIdx = 0;
        }
    }
}

s32 tk_player_search(EnTk* this) {
    s16 v0;
    s16 v1;

    if (this->actor.xyzDistToPlayerSq > SQ(100.0f)) {
        return 0;
    }

    v0 = this->actor.shape.rot.y;
    v0 -= this->h_21E;
    v0 -= this->headRot;

    v1 = this->actor.yawTowardsPlayer - v0;
    if (ABS(v1) < 0x1554) {
        return 1;
    } else {
        return 0;
    }
}

s32 tk_tag_search(EnTk* this, PlayState* play) {
    Actor* prop;
    f32 dxz;
    f32 dy;

    prop = play->actorCtx.actorLists[ACTORCAT_PROP].head;

    while (prop != NULL) {
        if (prop->id != ACTOR_EN_IT) {
            prop = prop->next;
            continue;
        }

        if (prop == this->currentSpot) {
            prop = prop->next;
            continue;
        }

        dy = prop->world.pos.y - this->actor.floorHeight;
        dxz = Actor_search_actor_distanceXZ(&this->actor, prop);
        if (dxz > 40.0f || dy > 10.0f) {
            prop = prop->next;
            continue;
        }

        this->currentSpot = prop;
        return prop->params;
    }

    return -1;
}

void tk_tag_check(EnTk* this) {
    f32 dxz;
    f32 dy;

    if (this->currentSpot != NULL) {
        dy = this->currentSpot->world.pos.y - this->actor.floorHeight;
        dxz = Actor_search_actor_distanceXZ(&this->actor, this->currentSpot);
        if (dxz > 40.0f || dy > 10.0f) {
            this->currentSpot = NULL;
        }
    }
}

f32 tk_get_speed(EnTk* this, PlayState* play) {
    f32 stepFrames[] = { 36.0f, 10.0f };
    f32 a1_;
    s32 i;

    if (this->skelAnime.curFrame == 0.0f || this->skelAnime.curFrame == 25.0f) {
        Actor_SE_set(&this->actor, NA_SE_EN_MORIBLIN_WALK);
    }

    if (this->skelAnime.animation != &gDampeWalkAnim) {
        return 0.0f;
    }

    a1_ = this->skelAnime.curFrame;
    for (i = 0; i < ARRAY_COUNT(stepFrames); i++) {
        if (a1_ < stepFrames[i] + 12.0f && a1_ >= stepFrames[i]) {
            break;
        }
    }
    if (i >= ARRAY_COUNT(stepFrames)) {
        return 0.0f;
    } else {
        a1_ = (0x8000 / 12.0f) * (a1_ - stepFrames[i]);
        return sin_s(a1_) * 2.0f;
    }
}

s32 tk_path_move(EnTk* this, PlayState* play) {
    Path* path;
    Vec3s* point;
    f32 dx;
    f32 dz;

    if (this->actor.params < 0) {
        return 1;
    }

    path = &play->pathList[0];
    point = SEGMENTED_TO_VIRTUAL(path->points);
    point += this->currentWaypoint;

    dx = point->x - this->actor.world.pos.x;
    dz = point->z - this->actor.world.pos.z;

    add_calc_short_angle2(&this->actor.shape.rot.y, RAD_TO_BINANG(fatan2(dx, dz)), 10, 1000, 1);
    this->actor.world.rot = this->actor.shape.rot;

    if (SQ(dx) + SQ(dz) < 10.0f) {
        this->currentWaypoint++;
        if (this->currentWaypoint >= path->count) {
            this->currentWaypoint = 0;
        }

        return 0;
    } else {
        return 1;
    }
}

u16 tk_set_message(PlayState* play, Actor* thisx) {
    u16 textId = get_mask_message(play, MASK_REACTION_SET_DAMPE);

    if (textId != 0) {
        return textId;
    }

    if (GET_INFTABLE(INFTABLE_D9)) {
        /* "Do you want me to dig here? ..." */
        return 0x5019;
    } else {
        /* "Hey kid! ..." */
        return 0x5018;
    }
}

s16 tk_end_message(PlayState* play, Actor* thisx) {
    s32 talkState = NPC_TALK_STATE_TALKING;

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
            break;
        case TEXT_STATE_CLOSING:
            /* "I am the boss of the carpenters ..." (wtf?) */
            if (thisx->textId == 0x5028) {
                SET_INFTABLE(INFTABLE_D8);
            }
            talkState = NPC_TALK_STATE_IDLE;
            break;
        case TEXT_STATE_DONE_FADING:
            break;
        case TEXT_STATE_CHOICE:
            if (pad_on_check(play) && (thisx->textId == 0x5018 || thisx->textId == 0x5019)) {
                if (play->msgCtx.choiceIndex == 1) {
                    /* "Thanks a lot!" */
                    thisx->textId = 0x0084;
                } else if (z_common_data.save.info.playerData.rupees < 10) {
                    /* "You don't have enough Rupees!" */
                    thisx->textId = 0x0085;
                } else {
                    play->msgCtx.msgMode = MSGMODE_PAUSED;
                    lupy_increase(-10);
                    SET_INFTABLE(INFTABLE_D9);
                    return NPC_TALK_STATE_ACTION;
                }
                message_set2(play, thisx->textId);
                SET_INFTABLE(INFTABLE_D9);
            }
            break;
        case TEXT_STATE_EVENT:
            if (pad_on_check(play) && (thisx->textId == 0x0084 || thisx->textId == 0x0085)) {
                message_close(play);
                talkState = NPC_TALK_STATE_IDLE;
            }
            break;
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            break;
    }

    return talkState;
}

s32 tk_item_check(EnTk* this) {
    f32 luck;
    s32 reward;

    luck = fqrand();

    if (luck < 0.4f) {
        reward = 0;
    } else if (luck < 0.7) {
        reward = 1;
    } else if (luck < 0.9) {
        reward = 2;
    } else {
        reward = 3;
    }

    switch (reward) {
        case 0:
            if (this->rewardCount[0] < 8) {
                this->rewardCount[0] += 1;
                return reward;
            }
            break;
        case 1:
            if (this->rewardCount[1] < 4) {
                this->rewardCount[1] += 1;
                return reward;
            }
            break;
        case 2:
            if (this->rewardCount[2] < 2) {
                this->rewardCount[2] += 1;
                return reward;
            }
            break;
        case 3:
            if (this->rewardCount[3] < 1) {
                this->rewardCount[3] += 1;
                return reward;
            }
            break;
    }

    if (this->rewardCount[0] < 8) {
        this->rewardCount[0] += 1;
        reward = 0;
    } else if (this->rewardCount[1] < 4) {
        this->rewardCount[1] += 1;
        reward = 1;
    } else if (this->rewardCount[2] < 2) {
        this->rewardCount[2] += 1;
        reward = 2;
    } else if (this->rewardCount[3] < 1) {
        this->rewardCount[3] += 1;
        reward = 3;
    } else {
        reward = 0;
        this->rewardCount[0] = 1;
        this->rewardCount[1] = 0;
        this->rewardCount[2] = 0;
        this->rewardCount[3] = 0;
    }

    return reward;
}

void tk_set_dust_eff(EnTk* this) {
    Vec3f pos = { 0.0f, 0.0f, 0.0f };
    Vec3f speed = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.3f, 0.0f };

    if (this->skelAnime.curFrame >= 32.0f && this->skelAnime.curFrame < 40.0f) {
        pos.x = (fqrand() - 0.5f) * 12.0f + this->v3f_304.x;
        pos.y = (fqrand() - 0.5f) * 8.0f + this->v3f_304.y;
        pos.z = (fqrand() - 0.5f) * 12.0f + this->v3f_304.z;
        set_dust_effect(this, &pos, 12, 0.2f, 0.1f, 0.0f);
    }
}

void En_Tk_Actor_ct(Actor* thisx, PlayState* play) {
    EnTk* this = (EnTk*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0, Actor_shadow_circle, 24.0f);

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gDampeSkel, NULL, this->jointTable, this->morphTable, 18);
    Skeleton_Info2_init(&this->skelAnime, &gDampeRestAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gDampeRestAnim),
                     ANIMMODE_LOOP, 0.0f);

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnTkAtInfoData);

    CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &TkStatusData);

    if (z_common_data.save.dayTime <= CLOCK_TIME(18, 0) || z_common_data.save.dayTime >= CLOCK_TIME(21, 0) ||
        LINK_IS_ADULT || play->sceneId != SCENE_GRAVEYARD) {
        Actor_delete(&this->actor);
        return;
    }

    Actor_set_scale(&this->actor, 0.01f);

    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.gravity = -0.1f;
    this->currentReward = -1;
    this->currentSpot = NULL;
    this->actionFunc = tk_matsu;
}

void En_Tk_Actor_dt(Actor* thisx, PlayState* play) {
    EnTk* this = (EnTk*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void tk_matsu(EnTk* this, PlayState* play) {
    s16 v1;
    s16 a1_;

    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        v1 = this->actor.shape.rot.y;
        v1 -= this->h_21E;
        v1 = this->actor.yawTowardsPlayer - v1;

        if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
            tk_demo_ct(this, play);
            this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
            this->actionFunc = tk_demo;
            return;
        }

        npc_talk(play, &this->actor, &this->interactInfo.talkState, this->collider.dim.radius + 30.0f,
                          tk_set_message, tk_end_message);
    } else if (tk_player_search(this)) {
        v1 = this->actor.shape.rot.y;
        v1 -= this->h_21E;
        v1 = this->actor.yawTowardsPlayer - v1;

        this->actionCountdown = 0;
        npc_talk(play, &this->actor, &this->interactInfo.talkState, this->collider.dim.radius + 30.0f,
                          tk_set_message, tk_end_message);
    } else if (Actor_talk_check(&this->actor, play)) {
        v1 = this->actor.shape.rot.y;
        v1 -= this->h_21E;
        v1 = this->actor.yawTowardsPlayer - v1;

        this->actionCountdown = 0;
        this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
    } else if (DECR(this->actionCountdown) == 0) {
        tk_aruku_ct(this, play);
        this->actionFunc = tk_aruku;

        /*! @bug v1 is uninitialized past this branch */
    } else {
        v1 = 0;
    }

    a1_ = CLAMP(-v1, 1270, 10730);
    add_calc_short_angle2(&this->headRot, a1_, 6, 1000, 1);
}

void tk_aruku(EnTk* this, PlayState* play) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_ACTION) {
        tk_demo_ct(this, play);
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        this->actionFunc = tk_demo;
    } else {
        this->actor.speed = tk_get_speed(this, play);
        tk_path_move(this, play);
        add_calc_short_angle2(&this->headRot, 0, 6, 1000, 1);
        tk_tag_check(this);

        DECR(this->actionCountdown);
        if (tk_player_search(this) || this->actionCountdown == 0) {
            tk_matsu_ct(this, play);
            this->actionFunc = tk_matsu;
        }
    }
}

void tk_demo(EnTk* this, PlayState* play) {
    Vec3f rewardOrigin;
    Vec3f rewardPos;
    s32 rewardParams[] = {
        ITEM00_RUPEE_GREEN, ITEM00_RUPEE_BLUE, ITEM00_RUPEE_RED, ITEM00_RUPEE_PURPLE, ITEM00_HEART_PIECE,
    };

    tk_set_dust_eff(this);

    if (this->skelAnime.curFrame == 32.0f) {
        /* What's gonna come out? */
        Actor_SE_set(&this->actor, NA_SE_EV_DIG_UP);

        this->rewardTimer = 0;

        if (this->validDigHere == 1) {
            rewardOrigin.x = 0.0f;
            rewardOrigin.y = 0.0f;
            rewardOrigin.z = -40.0f;

            Matrix_rotateY(this->actor.shape.rot.y, MTXMODE_NEW);
            Matrix_Position(&rewardOrigin, &rewardPos);

            rewardPos.x += this->actor.world.pos.x;
            rewardPos.y += this->actor.world.pos.y;
            rewardPos.z += this->actor.world.pos.z;

            this->currentReward = tk_item_check(this);
            if (this->currentReward == 3) {
                /*
                 * Upgrade the purple rupee reward to the heart piece if this
                 * is the first grand prize dig.
                 */
                if (!GET_ITEMGETINF(ITEMGETINF_1C)) {
                    SET_ITEMGETINF(ITEMGETINF_1C);
                    this->currentReward = 4;
                }
            }

            Item_set0(play, &rewardPos, rewardParams[this->currentReward]);
        }
    }

    if (this->skelAnime.curFrame >= 32.0f && this->rewardTimer == 10) {
        /* Play a reward sound effect shortly after digging */
        if (this->validDigHere == 0) {
            /* Bad dig spot */
            Actor_SE_set(&this->actor, NA_SE_SY_ERROR);
        } else if (this->currentReward == 4) {
            /* Heart piece */
            Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        } else {
            /* Rupee */
            Actor_SE_set(&this->actor, NA_SE_SY_TRE_BOX_APPEAR);
        }
    }
    this->rewardTimer++;

    if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
        if (this->currentReward < 0) {
            /* "Nope, nothing here!" */
            message_set(play, 0x501A, NULL);
        } else {
            message_close(play);
        }

        tk_matsu_ct(this, play);

        this->currentReward = -1;
        this->validDigHere = 0;
        this->actionFunc = tk_matsu;
    }
}

void En_Tk_Actor_move(Actor* thisx, PlayState* play) {
    EnTk* this = (EnTk*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

    Skeleton_Info2_anime_play(&this->skelAnime);

    Actor_position_moveF(&this->actor);

    Actor_BGcheck2(play, &this->actor, 40.0f, 10.0f, 0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);

    this->actionFunc(this, play);

    tk_eff_dust_mv(this);

    tk_eye_control(this);
}

void tk_display_parts(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_en_tk.c", 1188);

    gSPDisplayList(POLY_OPA_DISP++, gDampeShovelDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_tk.c", 1190);
}

s32 en_tk_actor_draw_before(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnTk* this = (EnTk*)thisx;

    switch (limbIndex) {
        /* Limb 15 - Head */
        case 15:
            this->h_21E = rot->y;
            break;
        /* Limb 16 - Jaw */
        case 16:
            this->h_21E += rot->y;
            rot->y += this->headRot;
            break;
    }

    return false;
}

void en_tk_actor_draw_after(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnTk* this = (EnTk*)thisx;
    Vec3f sp28 = { 0.0f, 0.0f, 4600.0f };
    Vec3f sp1C = { 0.0f, 0.0f, 0.0f };

    /* Limb 16 - Jaw */
    if (limbIndex == 16) {
        Matrix_Position(&sp1C, &this->actor.focus.pos);
    }

    /* Limb 14 - Neck */
    if (limbIndex == 14) {
        Matrix_Position(&sp28, &this->v3f_304);
        tk_display_parts(play);
    }
}

void En_Tk_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = {
        gDampeEyeOpenTex,
        gDampeEyeHalfTex,
        gDampeEyeClosedTex,
    };
    EnTk* this = (EnTk*)thisx;

    Matrix_push();
    tk_eff_dust_dr(this, play);
    Matrix_pull();

    OPEN_DISPS(play->state.gfxCtx, "../z_en_tk.c", 1294);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTextureIdx]));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          en_tk_actor_draw_before, en_tk_actor_draw_after, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_tk.c", 1312);
}
