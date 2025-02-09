/*
 * File: z_en_mk.c
 * Overlay: ovl_En_Mk
 * Description: Lakeside Professor
 */

#include "z_en_mk.h"
#include "assets/objects/object_mk/object_mk.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Mk_Actor_ct(Actor* thisx, PlayState* play);
void En_Mk_Actor_dt(Actor* thisx, PlayState* play);
void En_Mk_Actor_move(Actor* thisx, PlayState* play);
void En_Mk_Actor_draw(Actor* thisx, PlayState* play);

static void move_wait(EnMk* this, PlayState* play);

ActorProfile En_Mk_Profile = {
    /**/ ACTOR_EN_MK,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MK,
    /**/ sizeof(EnMk),
    /**/ En_Mk_Actor_ct,
    /**/ En_Mk_Actor_dt,
    /**/ En_Mk_Actor_move,
    /**/ En_Mk_Actor_draw,
};

static ColliderCylinderInit EnMkOcInfoData = {
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

void En_Mk_Actor_ct(Actor* thisx, PlayState* play) {
    EnMk* this = (EnMk*)thisx;
    s32 swimFlag;

    this->actor.minVelocityY = -4.0f;
    this->actor.gravity = -1.0f;
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 36.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_mk_Skel_005DF0, &object_mk_Anim_000D88, this->jointTable,
                       this->morphTable, 13);
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_mk_Anim_000D88);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnMkOcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_set_scale(&this->actor, 0.01f);

    this->actionFunc = move_wait;
    this->flags = 0;
    this->swimFlag = 0;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;

    if (GET_ITEMGETINF(ITEMGETINF_10)) {
        this->flags |= 4;
    }
}

void En_Mk_Actor_dt(Actor* thisx, PlayState* play) {
    EnMk* this = (EnMk*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void talk_wait(EnMk* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = move_wait;
    }

    this->flags |= 1;
}

void talk_give2(EnMk* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play) != 0) {
        this->actor.parent = NULL;
        this->actionFunc = talk_wait;
        total_event_timer_set(240);
        CLEAR_EVENTINF(EVENTINF_MARATHON_ACTIVE);
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_EYE_DROPS, 10000.0f, 50.0f);
    }
}

void talk_give2_b(EnMk* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = talk_give2;
        Actor_carry_request_set2(&this->actor, play, GI_EYE_DROPS, 10000.0f, 50.0f);
    }
}

void talk_give(EnMk* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = talk_give2_b;
    }

    this->flags |= 1;
}

static void move_demo2(EnMk* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
        this->actor.shape.rot.y -= 0x800;
    } else {
        this->actionFunc = talk_give;
        message_set2(play, 0x4030);
    }
}

static void move_demo1(EnMk* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else {
        this->timer = 16;
        this->actionFunc = move_demo2;
        Skeleton_Info2_init(&this->skelAnime, &object_mk_Anim_000D88, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_mk_Anim_000D88), ANIMMODE_LOOP, -4.0f);
        this->flags &= ~2;
    }
}

static void move_demo0(EnMk* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
        this->actor.shape.rot.y += 0x800;
    } else {
        this->timer = 120;
        this->actionFunc = move_demo1;
        Skeleton_Info2_init(&this->skelAnime, &object_mk_Anim_000724, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_mk_Anim_000724), ANIMMODE_LOOP, -4.0f);
        this->flags &= ~2;
    }
}

void talk_demo2(EnMk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        this->actionFunc = move_demo0;
        play->msgCtx.msgMode = MSGMODE_PAUSED;
        player->exchangeItemId = EXCH_ITEM_NONE;
        this->timer = 16;
        Skeleton_Info2_init(&this->skelAnime, &object_mk_Anim_000D88, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_mk_Anim_000D88), ANIMMODE_LOOP, -4.0f);
        this->flags &= ~2;
    }

    this->flags |= 1;
}

void talk_demo1(EnMk* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_set2(play, 0x4001);
        Skeleton_Info2_init(&this->skelAnime, &object_mk_Anim_000AC0, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_mk_Anim_000AC0), ANIMMODE_ONCE, -4.0f);
        this->flags &= ~2;
        this->actionFunc = talk_demo2;
    }

    this->flags |= 1;
}

void talk_demo0(EnMk* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_set2(play, 0x4000);
        Skeleton_Info2_init(&this->skelAnime, &object_mk_Anim_000AC0, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_mk_Anim_000AC0), ANIMMODE_LOOP, -4.0f);
        this->flags &= ~2;
        this->actionFunc = talk_demo1;
    }

    this->flags |= 1;
}

void move_givebin(EnMk* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = talk_wait;
        SET_ITEMGETINF(ITEMGETINF_10);
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_HEART_PIECE, 10000.0f, 50.0f);
    }
}

void talk_givebin(EnMk* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = move_givebin;
        Actor_carry_request_set2(&this->actor, play, GI_HEART_PIECE, 10000.0f, 50.0f);
    }

    this->flags |= 1;
}

static void move_wait(EnMk* this, PlayState* play) {
    s16 angle;
    s32 swimFlag;
    Player* player = GET_PLAYER(play);
    s32 playerExchangeItem;

    if (Actor_talk_check(&this->actor, play)) {
        playerExchangeItem = Actor_get_item_check(play);

        if (this->actor.textId != 0x4018) {
            player->actor.textId = this->actor.textId;
            this->actionFunc = talk_wait;
        } else {
            if (INV_CONTENT(ITEM_ODD_MUSHROOM) == ITEM_EYE_DROPS) {
                player->actor.textId = 0x4032;
                this->actionFunc = talk_wait;
            } else {
                switch (playerExchangeItem) {
                    case EXCH_ITEM_NONE:
                        if (this->swimFlag >= 8) {
                            if (GET_ITEMGETINF(ITEMGETINF_10)) {
                                player->actor.textId = 0x4075;
                                this->actionFunc = talk_wait;
                            } else {
                                player->actor.textId = 0x4074;
                                this->actionFunc = talk_givebin;
                                this->swimFlag = 0;
                            }
                        } else {
                            if (this->swimFlag == 0) {
                                player->actor.textId = 0x4018;
                                this->actionFunc = talk_wait;
                            } else {
                                player->actor.textId = 0x406C + this->swimFlag;
                                this->actionFunc = talk_wait;
                            }
                        }
                        break;
                    case EXCH_ITEM_EYEBALL_FROG:
                        player->actor.textId = 0x4019;
                        this->actionFunc = talk_demo0;
                        Skeleton_Info2_init(&this->skelAnime, &object_mk_Anim_000368, 1.0f, 0.0f,
                                         Si2_anime_end_frame(&object_mk_Anim_000368), ANIMMODE_ONCE, -4.0f);
                        this->flags &= ~2;
                        z_common_data.subTimerState = SUBTIMER_STATE_OFF;
                        Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
                        break;
                    default:
                        player->actor.textId = 0x4018;
                        this->actionFunc = talk_wait;
                        break;
                }
            }
        }
    } else {
        this->actor.textId = get_mask_message(play, MASK_REACTION_SET_LAKESIDE_PROFESSOR);

        if (this->actor.textId == 0) {
            this->actor.textId = 0x4018;
        }

        angle = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

        if ((ABS(angle) < 0x2151) && (this->actor.xzDistToPlayer < 100.0f)) {
            Actor_talk_request_get_item(&this->actor, play, 100.0f, EXCH_ITEM_EYEBALL_FROG);
            this->flags |= 1;
        }
    }
}

void En_Mk_Actor_move(Actor* thisx, PlayState* play) {
    EnMk* this = (EnMk*)thisx;
    s32 pad;
    Vec3s vec;
    Player* player;
    s16 swimFlag;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

    if (!(this->flags & 2) && Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->flags |= 2;
    }

    this->actionFunc(this, play);

    if (this->flags & 1) {
        eye_move2(play, &this->actor, &this->headRotation, &vec, this->actor.focus.pos);
    } else {
        add_calc_short_angle2(&this->headRotation.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->headRotation.y, 0, 6, 6200, 100);
    }

    player = GET_PLAYER(play);

    if (this->flags & 8) {
        if (!(player->stateFlags2 & PLAYER_STATE2_10)) {
            this->flags &= ~8;
        }
    } else {
        if (player->currentBoots == PLAYER_BOOTS_IRON) {
            this->flags |= 8;
        } else if (player->stateFlags2 & PLAYER_STATE2_10) {
            swimFlag = player->actor.depthInWater;

            if (swimFlag > 0) {
                if (swimFlag >= 320) {
                    if (swimFlag >= 355) {
                        swimFlag = 8;
                    } else {
                        swimFlag = 7;
                    }
                } else if (swimFlag < 80) {
                    swimFlag = 1;
                } else {
                    swimFlag *= 0.025f;
                }

                if (this->swimFlag < swimFlag) {
                    this->swimFlag = swimFlag;

                    if (!(this->flags & 4) && (this->swimFlag >= 8)) {
                        this->flags |= 4;
                        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
                    }
                }
            }
        }
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMk* this = (EnMk*)thisx;

    if (limbIndex == 11) {
        rot->y -= this->headRotation.y;
        rot->z += this->headRotation.x;
    }

    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 1000.0f, -100.0f, 0.0f };
    EnMk* this = (EnMk*)thisx;

    if (limbIndex == 11) {
        Matrix_Position(&pos, &this->actor.focus.pos);
    }
}

void En_Mk_Actor_draw(Actor* thisx, PlayState* play) {
    EnMk* this = (EnMk*)thisx;

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, &this->actor);
}
