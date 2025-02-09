/*
 * File: z_en_mu.c
 * Overlay: ovl_En_Mu
 * Description: Haggling townspeople
 */

#include "z_en_mu.h"
#include "assets/objects/object_mu/object_mu.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Mu_actor_ct(Actor* thisx, PlayState* play);
void En_Mu_actor_dt(Actor* thisx, PlayState* play);
void En_Mu_actor_move(Actor* thisx, PlayState* play);
void En_Mu_actor_draw(Actor* thisx, PlayState* play);

static void wait(EnMu* this, PlayState* play);
s16 mu_end_message(PlayState* play, Actor* thisx);

static ColliderCylinderInit EnMuAtInfoData = {
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
    { 100, 70, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 MuStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

ActorProfile En_Mu_Profile = {
    /**/ ACTOR_EN_MU,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MU,
    /**/ sizeof(EnMu),
    /**/ En_Mu_actor_ct,
    /**/ En_Mu_actor_dt,
    /**/ En_Mu_actor_move,
    /**/ En_Mu_actor_draw,
};

void En_Mu_actor_set_process(EnMu* this, EnMuActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void next_message(EnMu* this, PlayState* play) {
    u8 textIdOffset[] = { 0x42, 0x43, 0x3F, 0x41, 0x3E };
    u8 bitmask[] = {
        EVENTINF_MASK(EVENTINF_HAGGLING_TOWNSFOLK_MESG_0), EVENTINF_MASK(EVENTINF_HAGGLING_TOWNSFOLK_MESG_1),
        EVENTINF_MASK(EVENTINF_HAGGLING_TOWNSFOLK_MESG_2), EVENTINF_MASK(EVENTINF_HAGGLING_TOWNSFOLK_MESG_3),
        EVENTINF_MASK(EVENTINF_HAGGLING_TOWNSFOLK_MESG_4),
    };
    u8 talkFlags;
    s32 randomIndex;
    s32 i;

    talkFlags = GET_EVENTINF_ENMU_TALK_FLAGS();
    RESET_EVENTINF_ENMU_TALK_FLAGS();
    randomIndex = (play->state.frames + (s32)(fqrand() * 5.0f)) % 5;

    // Starting at randomIndex, scan sequentially for the next unspoken message
    for (i = 0; i < 5; i++) {
        if (!(talkFlags & bitmask[randomIndex])) {
            break;
        }

        randomIndex++;
        if (randomIndex >= 5) {
            randomIndex = 0;
        }
    }

    // If all 5 messages have been spoken, reset but prevent the last message from being repeated
    if (i == 5) {
        if (this->defaultTextId == (textIdOffset[randomIndex] | 0x7000)) {
            randomIndex++;
            if (randomIndex >= 5) {
                randomIndex = 0;
            }
        }
        talkFlags = 0;
    }

    talkFlags |= (u8)bitmask[randomIndex];
    this->defaultTextId = textIdOffset[randomIndex] | 0x7000;
    SET_EVENTINF_ENMU_TALK_FLAGS(talkFlags);
}

u16 mu_set_message(PlayState* play, Actor* thisx) {
    EnMu* this = (EnMu*)thisx;
    u16 textId = get_mask_message(play, MASK_REACTION_SET_HAGGLING_TOWNSPEOPLE_1 + this->actor.params);

    if (textId != 0) {
        return textId;
    }
    return this->defaultTextId;
}

s16 mu_end_message(PlayState* play, Actor* thisx) {
    EnMu* this = (EnMu*)thisx;

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
        case TEXT_STATE_DONE:
        case TEXT_STATE_SONG_DEMO_DONE:
        case TEXT_STATE_8:
        case TEXT_STATE_9:
            return NPC_TALK_STATE_TALKING;
        case TEXT_STATE_CLOSING:
            next_message(this, play);
            return NPC_TALK_STATE_IDLE;
        default:
            return NPC_TALK_STATE_TALKING;
    }
}

void En_Mu_actor_ct(Actor* thisx, PlayState* play) {
    EnMu* this = (EnMu*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 160.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_mu_Skel_004F70, &object_mu_Anim_0003F4, NULL, NULL, 0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnMuAtInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &MuStatusData);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    Actor_set_scale(&this->actor, 0.01f);
    next_message(this, play);
    En_Mu_actor_set_process(this, wait);
}

void En_Mu_actor_dt(Actor* thisx, PlayState* play) {
    EnMu* this = (EnMu*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
}

static void wait(EnMu* this, PlayState* play) {
    program_wait(play, this->fidgetTableY, this->fidgetTableZ, 16);
}

void En_Mu_actor_move(Actor* thisx, PlayState* play) {
    EnMu* this = (EnMu*)thisx;
    s32 pad;
    f32 talkDist;
    Vec3s pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y;
    pos.z = this->actor.world.pos.z;

    this->collider.dim.pos = pos;

    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Skeleton_Info2_anime_play(&this->skelAnime);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    this->actionFunc(this, play);
    talkDist = this->collider.dim.radius + 30.0f;
    npc_talk(play, &this->actor, &this->npcInfo.talkState, talkDist, mu_set_message, mu_end_message);

    this->actor.focus.pos = this->actor.world.pos;
    this->actor.focus.pos.y += 60.0f;
}

static s32 draw_before(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMu* this = (EnMu*)thisx;

    if ((limbIndex == 5) || (limbIndex == 6) || (limbIndex == 7) || (limbIndex == 11) || (limbIndex == 12) ||
        (limbIndex == 13) || (limbIndex == 14)) {
        rot->y += sin_s(this->fidgetTableY[limbIndex]) * FIDGET_AMPLITUDE;
        rot->z += cos_s(this->fidgetTableZ[limbIndex]) * FIDGET_AMPLITUDE;
    }
    return false;
}

static void draw_after(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
}

static Gfx* pa(GraphicsContext* gfxCtx, u8 r, u8 g, u8 b, u8 a) {
    Gfx* dlist;

    dlist = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));
    gDPSetEnvColor(dlist, r, g, b, a);
    gSPEndDisplayList(dlist + 1);
    return dlist;
}

void En_Mu_actor_draw(Actor* thisx, PlayState* play) {
    EnMu* this = (EnMu*)thisx;
    Color_RGBA8 colors[2][5] = {
        { { 100, 130, 235, 0 }, { 160, 250, 60, 0 }, { 90, 60, 20, 0 }, { 30, 240, 200, 0 }, { 140, 70, 20, 0 } },
        { { 140, 70, 20, 0 }, { 30, 240, 200, 0 }, { 90, 60, 20, 0 }, { 160, 250, 60, 0 }, { 100, 130, 235, 0 } }
    };
    u8 segmentId[] = { 0x08, 0x09, 0x0A, 0x0B, 0x0C };
    s32 i;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_mu.c", 514);
    Matrix_translate(-1200.0f, 0.0f, -1400.0f, MTXMODE_APPLY);
    for (i = 0; i < 5; i++) {
        gSPSegment(POLY_OPA_DISP++, segmentId[i],
                   pa(play->state.gfxCtx, colors[this->actor.params][i].r,
                                            colors[this->actor.params][i].g, colors[this->actor.params][i].b,
                                            colors[this->actor.params][i].a));
    }
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          draw_before, draw_after, this);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_mu.c", 534);
}
