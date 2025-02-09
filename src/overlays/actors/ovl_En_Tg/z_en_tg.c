/*
 * File: z_en_tg.c
 * Overlay: ovl_En_Tg
 * Description: Dancing Couple
 */

#include "z_en_tg.h"
#include "assets/objects/object_mu/object_mu.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Tg_actor_ct(Actor* thisx, PlayState* play);
void En_Tg_actor_dt(Actor* thisx, PlayState* play);
void En_Tg_actor_move(Actor* thisx, PlayState* play);
void En_Tg_actor_draw(Actor* thisx, PlayState* play);

static void wait(EnTg* this, PlayState* play);

static ColliderCylinderInit EnTgAtInfoData = {
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
    { 20, 64, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 TgStatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

ActorProfile En_Tg_Profile = {
    /**/ ACTOR_EN_TG,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MU,
    /**/ sizeof(EnTg),
    /**/ En_Tg_actor_ct,
    /**/ En_Tg_actor_dt,
    /**/ En_Tg_actor_move,
    /**/ En_Tg_actor_draw,
};

u16 tg_set_message(PlayState* play, Actor* thisx) {
    EnTg* this = (EnTg*)thisx;
    u16 maskReactionTextId = get_mask_message(play, MASK_REACTION_SET_DANCING_COUPLE);
    u32 textId;

    if (maskReactionTextId != 0) {
        return maskReactionTextId;
    }
    if (play->sceneId == SCENE_KAKARIKO_VILLAGE) {
        if (this->nextDialogue % 2 != 0) {
            textId = 0x5089;
        } else {
            textId = 0x508A;
        }
        return textId;
    } else {
        if (this->nextDialogue % 2 != 0) {
            textId = 0x7025;
        } else {
            textId = 0x7026;
        }
        return textId;
    }
}

s16 tg_end_message(PlayState* play, Actor* thisx) {
    EnTg* this = (EnTg*)thisx;

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
            switch (this->actor.textId) {
                case 0x5089:
                case 0x508A:
                    this->nextDialogue++;
                    break;
                case 0x7025:
                case 0x7026:
                    this->actor.params ^= 1;
                    this->nextDialogue++;
                    break;
            }
            return NPC_TALK_STATE_IDLE;
        default:
            return NPC_TALK_STATE_TALKING;
    }
}

void En_Tg_actor_ct(Actor* thisx, PlayState* play) {
    EnTg* this = (EnTg*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 28.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gDancingCoupleSkel, &gDancingCoupleAnim, NULL, NULL, 0);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnTgAtInfoData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &TgStatusData);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    Actor_set_scale(&this->actor, 0.01f);
    this->nextDialogue = play->state.frames % 2;
    this->actionFunc = wait;
}

void En_Tg_actor_dt(Actor* thisx, PlayState* play) {
    EnTg* this = (EnTg*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
    ClObjPipe_dt(play, &this->collider);
}

static void wait(EnTg* this, PlayState* play) {
    if (!this->interactInfo.talkState) {
        this->actor.shape.rot.y += 0x800;
    }
}

void En_Tg_actor_move(Actor* thisx, PlayState* play) {
    EnTg* this = (EnTg*)thisx;
    s32 pad;
    f32 temp;
    Vec3s sp2C;

    sp2C.x = this->actor.world.pos.x;
    sp2C.y = this->actor.world.pos.y;
    sp2C.z = (s16)this->actor.world.pos.z + 3;
    this->collider.dim.pos = sp2C;
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Skeleton_Info2_anime_play(&this->skelAnime);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    this->actionFunc(this, play);
    temp = this->collider.dim.radius + 30.0f;
    npc_talk(play, &this->actor, &this->interactInfo.talkState, temp, tg_set_message, tg_end_message);
}

static s32 draw_before(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    return false;
}

static void draw_after(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnTg* this = (EnTg*)thisx;
    Vec3f targetOffset = { 0.0f, 800.0f, 0.0f };

    if (limbIndex == 9) {
        // Place the target point at the guy's head instead of the center of the actor
        Matrix_Position(&targetOffset, &this->actor.focus.pos);
    }
}

static Gfx* pa(GraphicsContext* gfxCtx, u8 r, u8 g, u8 b, u8 a) {
    Gfx* displayList = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));

    gDPSetEnvColor(displayList, r, g, b, a);
    gSPEndDisplayList(displayList + 1);
    return displayList;
}

void En_Tg_actor_draw(Actor* thisx, PlayState* play) {
    EnTg* this = (EnTg*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_tg.c", 462);
    Matrix_translate(0.0f, 0.0f, -560.0f, MTXMODE_APPLY);

    // Set the guy's shoes and shirt to royal blue
    gSPSegment(POLY_OPA_DISP++, 0x08, pa(play->state.gfxCtx, 0, 50, 160, 0));

    // Set the girl's shirt to white
    gSPSegment(POLY_OPA_DISP++, 0x09, pa(play->state.gfxCtx, 255, 255, 255, 0));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          draw_before, draw_after, this);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_tg.c", 480);
}
