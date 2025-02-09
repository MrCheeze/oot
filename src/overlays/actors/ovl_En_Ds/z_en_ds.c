/*
 * File: z_en_ds.c
 * Overlay: ovl_En_Ds
 * Description: Potion Shop Granny
 */

#include "z_en_ds.h"
#include "assets/objects/object_ds/object_ds.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Ds_Actor_ct(Actor* thisx, PlayState* play);
void En_Ds_Actor_dt(Actor* thisx, PlayState* play);
void En_Ds_Actor_move(Actor* thisx, PlayState* play);
void En_Ds_Actor_draw(Actor* thisx, PlayState* play);

static void move_matsu(EnDs* this, PlayState* play);

ActorProfile En_Ds_Profile = {
    /**/ ACTOR_EN_DS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DS,
    /**/ sizeof(EnDs),
    /**/ En_Ds_Actor_ct,
    /**/ En_Ds_Actor_dt,
    /**/ En_Ds_Actor_move,
    /**/ En_Ds_Actor_draw,
};

void En_Ds_Actor_ct(Actor* thisx, PlayState* play) {
    EnDs* this = (EnDs*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 36.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gPotionShopLadySkel, &gPotionShopLadyAnim, this->jointTable,
                       this->morphTable, 6);
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gPotionShopLadyAnim);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    Actor_set_scale(&this->actor, 0.013f);

    this->actionFunc = move_matsu;
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    this->unk_1E8 = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_1E4 = 0.0f;
}

void En_Ds_Actor_dt(Actor* thisx, PlayState* play) {
}

static void talk_matsu(EnDs* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = move_matsu;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
    this->unk_1E8 |= 1;
}

static void end_matsu(EnDs* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        this->actionFunc = move_matsu;
    }
    this->unk_1E8 |= 1;
}

void request_matsu(EnDs* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = talk_matsu;
    } else {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_talk_request2(&this->actor, play, 1000.0f);
    }
}

void demo5_matsu(EnDs* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actor.textId = 0x504F;
        this->actionFunc = request_matsu;
        this->actor.flags &= ~ACTOR_FLAG_TALK;
        SET_ITEMGETINF(ITEMGETINF_30);
    }
}

void demo4_matsu(EnDs* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = demo5_matsu;
        z_common_data.subTimerState = SUBTIMER_STATE_OFF;
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_ODD_POTION, 10000.0f, 50.0f);
    }
}

void demo3_matsu(EnDs* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        this->actionFunc = demo4_matsu;
        Actor_carry_request_set2(&this->actor, play, GI_ODD_POTION, 10000.0f, 50.0f);
    }
}

static void move_demo2(EnDs* this, PlayState* play) {
    if (this->brewTimer > 0) {
        this->brewTimer--;
    } else {
        this->actionFunc = demo3_matsu;
        message_set2(play, 0x504D);
    }

    chase_f(&this->unk_1E4, 0, 0.03f);
    set_add_light_global(play, this->unk_1E4 * (2.0f - this->unk_1E4), 0.0f, 0.1f, 1.0f);
}

static void move_demo1(EnDs* this, PlayState* play) {
    if (this->brewTimer > 0) {
        this->brewTimer--;
    } else {
        this->actionFunc = move_demo2;
        this->brewTimer = 60;
        Actor_Environment_sw_Off(play, 0x3F);
    }
}

static void move_demo0(EnDs* this, PlayState* play) {
    if (this->brewTimer > 0) {
        this->brewTimer--;
    } else {
        this->actionFunc = move_demo1;
        this->brewTimer = 20;
    }

    chase_f(&this->unk_1E4, 1.0f, 0.01f);
    set_add_light_global(play, this->unk_1E4 * (2.0f - this->unk_1E4), 0.0f, 0.1f, 1.0f);
}

void which_matsu(EnDs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // yes
                this->actionFunc = move_demo0;
                this->brewTimer = 60;
                Actor_Environment_sw_On(play, 0x3F);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                player->exchangeItemId = EXCH_ITEM_NONE;
                break;
            case 1: // no
                message_set2(play, 0x504C);
                this->actionFunc = talk_matsu;
        }
    }
}

s32 BLDrcCheck(void) {
    if (z_common_data.save.info.playerData.rupees < 100) {
        return 0;
    } else if (findEmptyBottle() == 0) {
        return 1;
    } else {
        return 2;
    }
}

void handshake(EnDs* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = talk_matsu;
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_BOTTLE_POTION_BLUE, 10000.0f, 50.0f);
    }
}

void which_syobai0(EnDs* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // yes
                switch (BLDrcCheck()) {
                    case 0: // have less than 100 rupees
                        message_set2(play, 0x500E);
                        break;
                    case 1: // have 100 rupees but no empty bottle
                        message_set2(play, 0x96);
                        this->actionFunc = end_matsu;
                        return;
                    case 2: // have 100 rupees and empty bottle
                        lupy_increase(-100);
                        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                        Actor_carry_request_set2(&this->actor, play, GI_BOTTLE_POTION_BLUE, 10000.0f, 50.0f);
                        this->actionFunc = handshake;
                        return;
                }
                break;
            case 1: // no
                message_set2(play, 0x500D);
        }
        this->actionFunc = talk_matsu;
    }
}

static void move_matsu(EnDs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;

    if (Actor_talk_check(&this->actor, play)) {
        if (Actor_get_item_check(play) == EXCH_ITEM_ODD_MUSHROOM) {
            Nai_FxFlagEntry(NA_SE_SY_TRE_BOX_APPEAR, &_dummy_zero_f, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
            player->actor.textId = 0x504A;
            this->actionFunc = which_matsu;
        } else if (GET_ITEMGETINF(ITEMGETINF_30)) {
            player->actor.textId = 0x500C;
            this->actionFunc = which_syobai0;
        } else {
            s16 pad;

            if (INV_CONTENT(ITEM_ODD_MUSHROOM) == ITEM_ODD_MUSHROOM) {
                player->actor.textId = 0x5049;
            } else {
                player->actor.textId = 0x5048;
            }
            this->actionFunc = talk_matsu;
        }
    } else {
        yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        this->actor.textId = 0x5048;

        if ((ABS(yawDiff) < 0x2151) && (this->actor.xzDistToPlayer < 200.0f)) {
            Actor_talk_request_get_item(&this->actor, play, 100.0f, EXCH_ITEM_ODD_MUSHROOM);
            this->unk_1E8 |= 1;
        }
    }
}

void En_Ds_Actor_move(Actor* thisx, PlayState* play) {
    EnDs* this = (EnDs*)thisx;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }

    this->actionFunc(this, play);

    if (this->unk_1E8 & 1) {
        eye_move2(play, &this->actor, &this->unk_1D8, &this->unk_1DE, this->actor.focus.pos);
    } else {
        add_calc_short_angle2(&this->unk_1D8.x, 0, 6, 0x1838, 100);
        add_calc_short_angle2(&this->unk_1D8.y, 0, 6, 0x1838, 100);
        add_calc_short_angle2(&this->unk_1DE.x, 0, 6, 0x1838, 100);
        add_calc_short_angle2(&this->unk_1DE.y, 0, 6, 0x1838, 100);
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDs* this = (EnDs*)thisx;

    if (limbIndex == 5) {
        rot->x += this->unk_1D8.y;
        rot->z += this->unk_1D8.x;
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 1100.0f, 500.0f, 0.0f };
    EnDs* this = (EnDs*)thisx;

    if (limbIndex == 5) {
        Matrix_Position(&pos, &this->actor.focus.pos);
    }
}

void En_Ds_Actor_draw(Actor* thisx, PlayState* play) {
    EnDs* this = (EnDs*)thisx;

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);
}
