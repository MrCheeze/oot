/*
 * File: z_en_hs2.c
 * Overlay: ovl_En_Hs2
 * Description: Carpenter's Son (Child Link version)
 */

#include "z_en_hs2.h"
#include "terminal.h"
#include "assets/objects/object_hs/object_hs.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Hs2_Actor_ct(Actor* thisx, PlayState* play);
void En_Hs2_Actor_dt(Actor* thisx, PlayState* play);
void En_Hs2_Actor_move(Actor* thisx, PlayState* play);
void En_Hs2_Actor_draw(Actor* thisx, PlayState* play);
static void matsu(EnHs2* this, PlayState* play);

ActorProfile En_Hs2_Profile = {
    /**/ ACTOR_EN_HS2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_HS,
    /**/ sizeof(EnHs2),
    /**/ En_Hs2_Actor_ct,
    /**/ En_Hs2_Actor_dt,
    /**/ En_Hs2_Actor_move,
    /**/ En_Hs2_Actor_draw,
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

void En_Hs2_Actor_ct(Actor* thisx, PlayState* play) {
    EnHs2* this = (EnHs2*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 36.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_hs_Skel_006260, &object_hs_Anim_0005C0, this->jointTable,
                       this->morphTable, 16);
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &object_hs_Anim_0005C0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnHsOcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_set_scale(&this->actor, 0.01f);
    PRINTF(VT_FGCOL(CYAN) " ヒヨコの店(子人の時) \n" VT_RST);
    this->actionFunc = matsu;
    this->unk_2A8 = 0;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
}

void En_Hs2_Actor_dt(Actor* thisx, PlayState* play) {
    EnHs2* this = (EnHs2*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static s32 kihon_process(EnHs2* this, PlayState* play, u16 textId, EnHs2ActionFunc actionFunc) {
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = actionFunc;
        return 1;
    }

    this->actor.textId = textId;
    if (ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) < 0x2151 &&
        this->actor.xzDistToPlayer < 100.0f) {
        this->unk_2A8 |= 0x1;
        Actor_talk_request2(&this->actor, play, 100.0f);
    }
    return 0;
}

static void talk_matsu(EnHs2* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = matsu;
    }
    this->unk_2A8 |= 0x1;
}

static void matsu(EnHs2* this, PlayState* play) {
    u16 textId = get_mask_message(play, MASK_REACTION_SET_CARPENTERS_SON);

    if (textId == 0) {
        textId = 0x5069;
    }

    kihon_process(this, play, textId, talk_matsu);
}

void En_Hs2_Actor_move(Actor* thisx, PlayState* play) {
    EnHs2* this = (EnHs2*)thisx;
    s32 pad;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }
    this->actionFunc(this, play);
    if (this->unk_2A8 & 0x1) {
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
    EnHs2* this = (EnHs2*)thisx;

    switch (limbIndex) {
        case 12:
        case 13:
            *dList = NULL;
            return false;
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
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 300.0f, 1000.0f, 0.0f };
    EnHs2* this = (EnHs2*)thisx;

    if (limbIndex == 9) {
        Matrix_Position(&pos, &this->actor.focus.pos);
    }
}

void En_Hs2_Actor_draw(Actor* thisx, PlayState* play) {
    EnHs2* this = (EnHs2*)thisx;

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);
}
