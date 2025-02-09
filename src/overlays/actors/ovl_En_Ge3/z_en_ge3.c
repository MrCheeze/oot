/*
 * File: z_en_ge3.c
 * Overlay: ovl_En_Ge3
 * Description: Gerudo giving you membership card
 */

#include "z_en_ge3.h"
#include "assets/objects/object_geldb/object_geldb.h"
#include "versions.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Ge3_Actor_ct(Actor* thisx, PlayState* play2);
void En_Ge3_Actor_dt(Actor* thisx, PlayState* play);
void En_Ge3_Actor_move_Talk(Actor* thisx, PlayState* play);
void En_Ge3_Actor_draw(Actor* thisx, PlayState* play2);

static void move_frend(EnGe3* this, PlayState* play);
static void move_give(EnGe3* this, PlayState* play);
void En_Ge3_Actor_move_Frend(Actor* thisx, PlayState* play);

ActorProfile En_Ge3_Profile = {
    /**/ ACTOR_EN_GE3,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GELDB,
    /**/ sizeof(EnGe3),
    /**/ En_Ge3_Actor_ct,
    /**/ En_Ge3_Actor_dt,
    /**/ En_Ge3_Actor_move_Talk,
    /**/ En_Ge3_Actor_draw,
};

static ColliderCylinderInit EnGe3OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000722, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 50, 0, { 0, 0, 0 } },
};

static EnGe3ActionFunc prc_tbl[] = { move_frend };
static AnimationHeader* soft_anime_tbl[] = { &gGerudoRedStandAnim }; // Idle with right hand on hip and left over mouth
static u8 anime_frame_index[] = { ANIMMODE_LOOP };

void En_Ge3_actor_set_process(EnGe3* this, s32 i) {
    this->actionFunc = prc_tbl[i];

    Skeleton_Info2_init(&this->skelAnime, soft_anime_tbl[i], 1.0f, 0.0f, (f32)Si2_anime_end_frame(soft_anime_tbl[i]),
                     anime_frame_index[i], -8.0f);

    this->unk_30C &= ~2;
}

void En_Ge3_Actor_ct(Actor* thisx, PlayState* play2) {
    EnGe3* this = (EnGe3*)thisx;
    PlayState* play = play2;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 36.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGerudoRedSkel, NULL, this->jointTable, this->morphTable,
                       GELDB_LIMB_MAX);
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gGerudoRedStandAnim);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnGe3OcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_set_scale(&this->actor, 0.01f);
    this->actor.world.rot.z = 0;
    this->actor.shape.rot.z = 0;
    En_Ge3_actor_set_process(this, 0);
    this->actionFunc = move_give;
    this->unk_30C = 0;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actor.minVelocityY = -4.0f;
    this->actor.gravity = -1.0f;
}

void En_Ge3_Actor_dt(Actor* thisx, PlayState* play) {
    EnGe3* this = (EnGe3*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void ge3_search_furimuki(EnGe3* this, PlayState* play) {
    s32 pad;
    s16 angleDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

    if (ABS(angleDiff) <= 0x4000) {
        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, 4000, 100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        eye_move2(play, &this->actor, &this->headRot, &this->unk_306, this->actor.focus.pos);
    } else {
        if (angleDiff < 0) {
            add_calc_short_angle2(&this->headRot.y, -0x2000, 6, 6200, 0x100);
        } else {
            add_calc_short_angle2(&this->headRot.y, 0x2000, 6, 6200, 0x100);
        }

        add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 12, 1000, 100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }
}

void ge3_search_normal(EnGe3* this, PlayState* play) {
    if ((ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) <= 0x2300) &&
        (this->actor.xzDistToPlayer < 100.0f)) {
        eye_move2(play, &this->actor, &this->headRot, &this->unk_306, this->actor.focus.pos);
    } else {
        add_calc_short_angle2(&this->headRot.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->headRot.y, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_306.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_306.y, 0, 6, 6200, 100);
    }
}

static void talk_frend(EnGe3* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        this->actionFunc = move_frend;
        this->actor.update = En_Ge3_Actor_move_Frend;
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    }
    ge3_search_furimuki(this, play);
}

static void move_frend(EnGe3* this, PlayState* play) {
    ge3_search_normal(this, play);
}

static void move_give3(EnGe3* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = talk_frend;
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_GERUDOS_CARD, 10000.0f, 50.0f);
    }
}

static void move_give2(EnGe3* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        message_close(play);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = move_give3;
        Actor_carry_request_set2(&this->actor, play, GI_GERUDOS_CARD, 10000.0f, 50.0f);
    }
}

static void move_give(EnGe3* this, PlayState* play) {
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = move_give2;
    } else {
        if (!(this->unk_30C & 4)) {
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
            this->unk_30C |= 4;
        }
        this->actor.textId = 0x6004;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_talk_request_get_item_set(&this->actor, play, 300.0f, 300.0f, EXCH_ITEM_NONE);
    }
    ge3_search_normal(this, play);
}

static void before_process(EnGe3* this, PlayState* play) {
    s32 pad;
    s32 pad2;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_BGcheck2(play, &this->actor, 40.0f, 25.0f, 40.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);

    if (!(this->unk_30C & 2) && Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->unk_30C |= 2;
    }
}

static void after_process(EnGe3* this, PlayState* play) {

    Actor_position_moveF(&this->actor);

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = get_random_timer(60, 60);
    }

    this->eyeIndex = this->blinkTimer;

    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

void En_Ge3_Actor_move_Frend(Actor* thisx, PlayState* play) {
    EnGe3* this = (EnGe3*)thisx;

    before_process(this, play);
    this->actionFunc(this, play);

    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = talk_frend;
        this->actor.update = En_Ge3_Actor_move_Talk;
    } else {
        this->actor.textId = 0x6005;
        if (this->actor.xzDistToPlayer < 100.0f) {
            Actor_talk_request2(&this->actor, play, 100.0f);
        }
    }

    after_process(this, play);
}

void En_Ge3_Actor_move_Talk(Actor* thisx, PlayState* play) {
    EnGe3* this = (EnGe3*)thisx;

    before_process(this, play);
    this->actionFunc(this, play);
    after_process(this, play);
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGe3* this = (EnGe3*)thisx;

    switch (limbIndex) {
        // Hide swords and veil from object_geldb
        case GELDB_LIMB_VEIL:
        case GELDB_LIMB_R_SWORD:
        case GELDB_LIMB_L_SWORD:
            *dList = NULL;
            return false;
        // Turn head
        case GELDB_LIMB_HEAD:
            rot->x += this->headRot.y;
#if OOT_VERSION >= PAL_1_1
            FALLTHROUGH;
        default:
            // This is a hack to fix a bug present before PAL 1.1, where the actor's clothes can change color
            // depending on what was drawn earlier in the frame.
            OPEN_DISPS(play->state.gfxCtx, "../z_en_ge3.c", 547);
            switch (limbIndex) {
                case GELDB_LIMB_NECK:
                    break;
                case GELDB_LIMB_HEAD:
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetEnvColor(POLY_OPA_DISP++, 80, 60, 10, 255);
                    break;
                case GELDB_LIMB_R_SWORD:
                case GELDB_LIMB_L_SWORD:
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetEnvColor(POLY_OPA_DISP++, 140, 170, 230, 255);
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
                    break;
                default:
                    gDPPipeSync(POLY_OPA_DISP++);
                    gDPSetEnvColor(POLY_OPA_DISP++, 140, 0, 0, 255);
                    break;
            }
            CLOSE_DISPS(play->state.gfxCtx, "../z_en_ge3.c", 566);
#endif
            break;
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnGe3* this = (EnGe3*)thisx;
    Vec3f D_80A351C8 = { 600.0f, 700.0f, 0.0f };

    if (limbIndex == GELDB_LIMB_HEAD) {
        Matrix_Position(&D_80A351C8, &this->actor.focus.pos);
    }
}

void En_Ge3_Actor_draw(Actor* thisx, PlayState* play2) {
    static void* eye_txt[] = {
        gGerudoRedEyeOpenTex,
        gGerudoRedEyeHalfTex,
        gGerudoRedEyeShutTex,
    };
    EnGe3* this = (EnGe3*)thisx;
    PlayState* play = play2;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ge3.c", 614);

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeIndex]));
    Actor_HiliteReflect_set_init(&this->actor, play, 0);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ge3.c", 631);
}
