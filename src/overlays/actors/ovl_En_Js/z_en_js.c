/*
 * File: z_en_js.c
 * Overlay: ovl_En_Js
 * Description: Magic Carpet Salesman
 */

#include "z_en_js.h"
#include "assets/objects/object_js/object_js.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Js_Actor_ct(Actor* thisx, PlayState* play);
void En_Js_Actor_dt(Actor* thisx, PlayState* play);
void En_Js_Actor_move(Actor* thisx, PlayState* play);
void En_Js_Actor_draw(Actor* thisx, PlayState* play);

static void move_wait(EnJs* this, PlayState* play);

ActorProfile En_Js_Profile = {
    /**/ ACTOR_EN_JS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_JS,
    /**/ sizeof(EnJs),
    /**/ En_Js_Actor_ct,
    /**/ En_Js_Actor_dt,
    /**/ En_Js_Actor_move,
    /**/ En_Js_Actor_draw,
};

static ColliderCylinderInit EnJsOcInfoData = {
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

void En_Js_actor_set_process(EnJs* this, EnJsActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Js_Actor_ct(Actor* thisx, PlayState* play) {
    EnJs* this = (EnJs*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, NULL, 36.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gCarpetMerchantSkel, &gCarpetMerchantSlappingKneeAnim, this->jointTable,
                       this->morphTable, 13);
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gCarpetMerchantSlappingKneeAnim);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnJsOcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_set_scale(&this->actor, 0.01f);
    En_Js_actor_set_process(this, move_wait);
    this->unk_284 = 0;
    this->actor.gravity = -1.0f;
    Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_JSJUTAN, this->actor.world.pos.x,
                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
}

void En_Js_Actor_dt(Actor* thisx, PlayState* play) {
    EnJs* this = (EnJs*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static u8 kihon_process(EnJs* this, PlayState* play, u16 textId) {
    s16 yawDiff;

    if (Actor_talk_check(&this->actor, play)) {
        return 1;
    } else {
        this->actor.textId = textId;
        yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

        if (ABS(yawDiff) <= 0x1800 && this->actor.xzDistToPlayer < 100.0f) {
            this->unk_284 |= 1;
            Actor_talk_request2(&this->actor, play, 100.0f);
        }
        return 0;
    }
}

static void to_matsu(EnJs* this) {
    En_Js_actor_set_process(this, move_wait);
    Skeleton_Info2_init(&this->skelAnime, &gCarpetMerchantSlappingKneeAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gCarpetMerchantSlappingKneeAnim), ANIMMODE_ONCE, -4.0f);
}

void end_wait(EnJs* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        to_matsu(this);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
}

void next_talk2(EnJs* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        En_Js_actor_set_process(this, end_wait);
    } else {
        Actor_talk_request2(&this->actor, play, 1000.0f);
    }
}

static void next_talk(EnJs* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actor.textId = 0x6078;
        En_Js_actor_set_process(this, next_talk2);
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
}

static void move_give(EnJs* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        En_Js_actor_set_process(this, next_talk);
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_BOMBCHUS_10, 10000.0f, 50.0f);
    }
}

void talk_sold(EnJs* this, PlayState* play) {
    if (message_check(&play->msgCtx) == TEXT_STATE_CHOICE && pad_on_check(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // yes
                if (z_common_data.save.info.playerData.rupees < 200) {
                    message_set2(play, 0x6075);
                    to_matsu(this);
                } else {
                    lupy_increase(-200);
                    En_Js_actor_set_process(this, move_give);
                }
                break;
            case 1: // no
                message_set2(play, 0x6074);
                to_matsu(this);
        }
    }
}

void to_syobai(EnJs* this) {
    En_Js_actor_set_process(this, talk_sold);
    Skeleton_Info2_init(&this->skelAnime, &gCarpetMerchantIdleAnim, 1.0f, 0.0f,
                     Si2_anime_end_frame(&gCarpetMerchantIdleAnim), ANIMMODE_ONCE, -4.0f);
}

static void move_wait(EnJs* this, PlayState* play) {
    if (kihon_process(this, play, 0x6077)) {
        to_syobai(this);
    }
}

void En_Js_Actor_move(Actor* thisx, PlayState* play) {
    EnJs* this = (EnJs*)thisx;
    s32 pad;
    s32 pad2;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (T_BGCheck_getSoundGroundLabel(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId) ==
            SURFACE_SFX_OFFSET_SAND) {
            add_calc2(&this->actor.shape.yOffset, sREG(80) + -2000.0f, 1.0f, (sREG(81) / 10.0f) + 50.0f);
        }
    } else {
        add_calc0(&this->actor.shape.yOffset, 1.0f, (sREG(81) / 10.0f) + 50.0f);
    }
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }
    this->actionFunc(this, play);
    if (this->unk_284 & 1) {
        eye_move2(play, &this->actor, &this->unk_278, &this->unk_27E, this->actor.focus.pos);
    } else {
        add_calc_short_angle2(&this->unk_278.x, 0, 6, 0x1838, 0x64);
        add_calc_short_angle2(&this->unk_278.y, 0, 6, 0x1838, 0x64);
        add_calc_short_angle2(&this->unk_27E.x, 0, 6, 0x1838, 0x64);
        add_calc_short_angle2(&this->unk_27E.y, 0, 6, 0x1838, 0x64);
    }
    this->unk_284 &= ~0x1;

    if (DECR(this->unk_288) == 0) {
        this->unk_288 = get_random_timer(0x3C, 0x3C);
    }

    this->unk_286 = this->unk_288;

    if (this->unk_286 >= 3) {
        this->unk_286 = 0;
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnJs* this = (EnJs*)thisx;

    if (limbIndex == 12) {
        rot->y -= this->unk_278.y;
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 0.0f, 0.0f, 0.0f };
    EnJs* this = (EnJs*)thisx;

    if (limbIndex == 12) {
        Matrix_Position(&pos, &this->actor.focus.pos);
    }
}
void En_Js_Actor_draw(Actor* thisx, PlayState* play) {
    EnJs* this = (EnJs*)thisx;

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);
}
