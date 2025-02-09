/*
 * File: z_en_hs.c
 * Overlay: ovl_En_Hs
 * Description: Carpenter's Son
 */

#include "z_en_hs.h"
#include "terminal.h"
#include "assets/objects/object_hs/object_hs.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Hs_Actor_ct(Actor* thisx, PlayState* play);
void En_Hs_Actor_dt(Actor* thisx, PlayState* play);
void En_Hs_Actor_move(Actor* thisx, PlayState* play);
void En_Hs_Actor_draw(Actor* thisx, PlayState* play);

static void matsu(EnHs* this, PlayState* play);
static void matsu2(EnHs* this, PlayState* play);

ActorProfile En_Hs_Profile = {
    /**/ ACTOR_EN_HS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_HS,
    /**/ sizeof(EnHs),
    /**/ En_Hs_Actor_ct,
    /**/ En_Hs_Actor_dt,
    /**/ En_Hs_Actor_move,
    /**/ En_Hs_Actor_draw,
};

static ColliderCylinderInit EnHsOcInfoData = {
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
    { 40, 40, 0, { 0, 0, 0 } },
};

void En_Hs_actor_set_process(EnHs* this, EnHsActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Hs_Actor_ct(Actor* thisx, PlayState* play) {
    EnHs* this = (EnHs*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 36.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_hs_Skel_006260, &object_hs_Anim_0005C0, this->jointTable,
                       this->morphTable, 16);
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_hs_Anim_0005C0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnHsOcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_set_scale(&this->actor, 0.01f);

    if (!LINK_IS_ADULT) {
        this->actor.params = 0;
    } else {
        this->actor.params = 1;
    }

    if (this->actor.params == 1) {
        // "chicken shop (adult era)"
        PRINTF(VT_FGCOL(CYAN) " ヒヨコの店(大人の時) \n" VT_RST);
        En_Hs_actor_set_process(this, matsu);
        if (GET_ITEMGETINF(ITEMGETINF_30)) {
            // "chicken shop closed"
            PRINTF(VT_FGCOL(CYAN) " ヒヨコ屋閉店 \n" VT_RST);
            Actor_delete(&this->actor);
        }
    } else {
        // "chicken shop (child era)"
        PRINTF(VT_FGCOL(CYAN) " ヒヨコの店(子人の時) \n" VT_RST);
        En_Hs_actor_set_process(this, matsu);
    }

    this->unk_2A8 = 0;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
}

void En_Hs_Actor_dt(Actor* thisx, PlayState* play) {
    EnHs* this = (EnHs*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static s32 kihon_process(EnHs* this, PlayState* play, u16 textId, EnHsActionFunc actionFunc) {
    s16 yawDiff;

    if (Actor_talk_check(&this->actor, play)) {
        En_Hs_actor_set_process(this, actionFunc);
        return 1;
    }

    this->actor.textId = textId;
    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if ((ABS(yawDiff) <= 0x2150) && (this->actor.xzDistToPlayer < 100.0f)) {
        this->unk_2A8 |= 1;
        Actor_talk_request2(&this->actor, play, 100.0f);
    }

    return 0;
}

static void talk_matsu2(EnHs* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        En_Hs_actor_set_process(this, matsu2);
    }

    this->unk_2A8 |= 1;
}

void end_matsu2(EnHs* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        total_event_timer_set(180);
        En_Hs_actor_set_process(this, matsu2);
        CLEAR_EVENTINF(EVENTINF_MARATHON_ACTIVE);
    }

    this->unk_2A8 |= 1;
}

static void matsu2(EnHs* this, PlayState* play) {
    kihon_process(this, play, 0x10B6, talk_matsu2);
}

static void talk_matsu(EnHs* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        En_Hs_actor_set_process(this, matsu);
    }
}

static void end_matsu(EnHs* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        En_Hs_actor_set_process(this, matsu);
    }
}

void get_matsu2(EnHs* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        En_Hs_actor_set_process(this, end_matsu2);
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_ODD_MUSHROOM, 10000.0f, 50.0f);
    }

    this->unk_2A8 |= 1;
}

void talk_start1(EnHs* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_CHOICE) && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0:
                En_Hs_actor_set_process(this, get_matsu2);
                Actor_carry_request_set2(&this->actor, play, GI_ODD_MUSHROOM, 10000.0f, 50.0f);
                break;
            case 1:
                message_set2(play, 0x10B4);
                En_Hs_actor_set_process(this, end_matsu);
                break;
        }

        Skeleton_Info2_init(&this->skelAnime, &object_hs_Anim_0005C0, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_hs_Anim_0005C0), ANIMMODE_LOOP, 8.0f);
    }

    this->unk_2A8 |= 1;
}

void talk_start0(EnHs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_set2(play, 0x10B3);
        En_Hs_actor_set_process(this, talk_start1);
        Skeleton_Info2_init(&this->skelAnime, &object_hs_Anim_000528, 1.0f, 0.0f,
                         Si2_anime_end_frame(&object_hs_Anim_000528), ANIMMODE_LOOP, 8.0f);
    }

    if (this->unk_2AA > 0) {
        this->unk_2AA--;
        if (this->unk_2AA == 0) {
            player_SE_set(player, NA_SE_EV_CHICKEN_CRY_M);
        }
    }

    this->unk_2A8 |= 1;
}

static void matsu(EnHs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;

    if (Actor_talk_check(&this->actor, play)) {
        if (Actor_get_item_check(play) == EXCH_ITEM_COJIRO) {
            player->actor.textId = 0x10B2;
            En_Hs_actor_set_process(this, talk_start0);
            Skeleton_Info2_init(&this->skelAnime, &object_hs_Anim_000304, 1.0f, 0.0f,
                             Si2_anime_end_frame(&object_hs_Anim_000304), ANIMMODE_LOOP, 8.0f);
            this->unk_2AA = 40;
            Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
        } else {
            player->actor.textId = 0x10B1;
            En_Hs_actor_set_process(this, talk_matsu);
        }
    } else {
        yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        this->actor.textId = 0x10B1;
        if ((ABS(yawDiff) <= 0x2150) && (this->actor.xzDistToPlayer < 100.0f)) {
            Actor_talk_request_get_item(&this->actor, play, 100.0f, EXCH_ITEM_COJIRO);
        }
    }
}

void En_Hs_Actor_move(Actor* thisx, PlayState* play) {
    EnHs* this = (EnHs*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }

    this->actionFunc(this, play);

    if (this->unk_2A8 & 1) {
        eye_move2(play, &this->actor, &this->unk_29C, &this->unk_2A2, this->actor.focus.pos);
        this->unk_2A8 &= ~1;
    } else {
        add_calc_short_angle2(&this->unk_29C.x, 12800, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_29C.y, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_2A2.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_2A2.y, 0, 6, 6200, 100);
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnHs* this = (EnHs*)thisx;

    switch (limbIndex) {
        case 9:
            rot->x += this->unk_29C.y;
            rot->z += this->unk_29C.x;
            break;
        case 10:
            *dList = NULL;
            return false;
        case 11:
            *dList = NULL;
            return false;
        case 12:
            if (this->actor.params == 1) {
                *dList = NULL;
                return false;
            }
            break;
        case 13:
            if (this->actor.params == 1) {
                *dList = NULL;
                return false;
            }
            break;
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 300.0f, 1000.0f, 0.0f };
    EnHs* this = (EnHs*)thisx;

    if (limbIndex == 9) {
        Matrix_Position(&pos, &this->actor.focus.pos);
    }
}

void En_Hs_Actor_draw(Actor* thisx, PlayState* play) {
    EnHs* this = (EnHs*)thisx;

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);
}
