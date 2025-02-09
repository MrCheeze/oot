/*
 * File: z_en_niw_girl.c
 * Overlay: ovl_En_Niw_Girl
 * Description: Girl that chases a cuckoo around in Hyrule Market and Kakariko Village
 */

#include "z_en_niw_girl.h"
#include "assets/objects/object_gr/object_gr.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Niw_Girl_actor_ct(Actor* thisx, PlayState* play);
void En_Niw_Girl_actor_dt(Actor* thisx, PlayState* play);
void En_Niw_Girl_actor_move(Actor* thisx, PlayState* play);
void En_Niw_Girl_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait_init(EnNiwGirl* this, PlayState* play);
static void mode_wait(EnNiwGirl* this, PlayState* play);
static void mode_run(EnNiwGirl* this, PlayState* play);

ActorProfile En_Niw_Girl_Profile = {
    /**/ ACTOR_EN_NIW_GIRL,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GR,
    /**/ sizeof(EnNiwGirl),
    /**/ En_Niw_Girl_actor_ct,
    /**/ En_Niw_Girl_actor_dt,
    /**/ En_Niw_Girl_actor_move,
    /**/ En_Niw_Girl_actor_draw,
};

static ColliderCylinderInit OcInfoData = {
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
    { 10, 30, 0, { 0, 0, 0 } },
};

void En_Niw_Girl_actor_ct(Actor* thisx, PlayState* play) {
    EnNiwGirl* this = (EnNiwGirl*)thisx;
    s32 pad;
    Vec3f vec1;
    Vec3f vec2;
    s32 pad2;

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gNiwGirlSkel, &gNiwGirlRunAnim, this->jointTable, this->morphTable, 17);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    if (this->actor.params < 0) {
        this->actor.params = 0;
    }
    this->path = PARAMS_GET_U(this->actor.params, 8, 8);
    this->actor.gravity = -3.0f;
    Matrix_rotateY(BINANG_TO_RAD_ALT(this->actor.shape.rot.y), MTXMODE_NEW);
    vec2.x = vec2.y = vec2.z = 0.0f;
    vec1.x = vec1.y = 0.0f;
    vec1.z = 50.0;
    Matrix_Position(&vec1, &vec2);
    this->chasedEnNiw = (EnNiw*)Actor_info_make_child_actor(
        &play->actorCtx, &this->actor, play, ACTOR_EN_NIW, this->actor.world.pos.x + vec2.x,
        this->actor.world.pos.y + vec2.y, this->actor.world.pos.z + vec2.z, 0, this->actor.world.rot.y, 0, 0xA);
    if (this->chasedEnNiw != NULL) {
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ シツレイしちゃうわね！プンプン ☆☆☆☆☆ %d\n" VT_RST, this->actor.params);
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ きゃははははは、まてー ☆☆☆☆☆ %d\n" VT_RST, this->path);
        PRINTF("\n\n");
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->actionFunc = mode_wait_init;
    } else {
        PRINTF("\n\n");
        PRINTF(VT_FGCOL(GREEN) "☆☆☆☆☆ なぜか、セットできむぅあせん ☆☆☆☆☆ %d\n" VT_RST, this->actor.params);
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ んんがくく ☆☆☆☆☆ %d\n" VT_RST, this->path);
        PRINTF("\n\n");
        Actor_delete(&this->actor);
    }
}

void En_Niw_Girl_actor_dt(Actor* thisx, PlayState* play) {
}

void mode_run_init(EnNiwGirl* this, PlayState* play) {
    f32 frameCount = Si2_anime_end_frame(&gNiwGirlRunAnim);
    Skeleton_Info2_init(&this->skelAnime, &gNiwGirlRunAnim, 1.0f, 0.0f, frameCount, 0, -10.0f);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = mode_run;
}

static void mode_run(EnNiwGirl* this, PlayState* play) {
    Path* path = &play->pathList[this->path];
    f32 xDistBetween;
    f32 zDistBetween;

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc2(&this->actor.speed, 3.0f, 0.2f, 0.4f);

    // Find the X and Z distance between the girl and the cuckoo she is chasing
    xDistBetween = this->chasedEnNiw->actor.world.pos.x - this->actor.world.pos.x;
    zDistBetween = this->chasedEnNiw->actor.world.pos.z - this->actor.world.pos.z;
    if (message_check(&play->msgCtx) != TEXT_STATE_NONE) {
        this->chasedEnNiw->path = 0;
    }
    if (sqrtf(SQ(xDistBetween) + SQ(zDistBetween)) < 70.0f) {
        this->chasedEnNiw->path = (this->path + 1);
        this->chasedEnNiw->unk_2EC = path->count;
    } else if (sqrtf(SQ(xDistBetween) + SQ(zDistBetween)) > 150.0f) {
        this->chasedEnNiw->path = 0;
    }

    // Change her angle so that she is always facing the cuckoo
    add_calc_short_angle2(&this->actor.shape.rot.y, RAD_TO_BINANG(fatan2(xDistBetween, zDistBetween)), 3,
                       this->unk_27C, 0);
    add_calc2(&this->unk_27C, 5000.0f, 30.0f, 150.0f);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    // Only allow Link to talk to her when she is playing the jumping animation
    if ((this->jumpTimer == 0) || (mask_check(play) != PLAYER_MASK_NONE)) {
        this->jumpTimer = 60;
        this->actionFunc = mode_wait_init;
    }
}

static void mode_wait_init(EnNiwGirl* this, PlayState* play) {
    Skeleton_Info2_init(&this->skelAnime, &gNiwGirlJumpAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gNiwGirlJumpAnim), 0,
                     -10.0f);
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.textId = 0x7000;
    if (GET_EVENTCHKINF(EVENTCHKINF_80) && (this->unk_27A == 0)) {
        this->actor.textId = 0x70EA;
    }
    switch (mask_check(play)) {
        case PLAYER_MASK_KEATON:
            this->actor.textId = 0x7118;
            break;
        case PLAYER_MASK_SPOOKY:
            this->actor.textId = 0x7119;
            break;
        case PLAYER_MASK_BUNNY:
        case PLAYER_MASK_ZORA:
        case PLAYER_MASK_GERUDO:
            this->actor.textId = 0x711A;
            break;
        case PLAYER_MASK_SKULL:
        case PLAYER_MASK_GORON:
        case PLAYER_MASK_TRUTH:
            this->actor.textId = 0x711B;
            break;
    }
    this->unk_270 = 6;
    this->actionFunc = mode_wait;
}

static void mode_wait(EnNiwGirl* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (message_check(&play->msgCtx) != TEXT_STATE_NONE) {
        this->chasedEnNiw->path = 0;
    }
    add_calc0(&this->actor.speed, 0.8f, 0.2f);
    if (Actor_talk_check(&this->actor, play)) {
        if (this->actor.textId == 0x70EA) {
            this->unk_27A = 1;
        }
    } else {
        if ((this->jumpTimer == 0) && message_check(&play->msgCtx) == TEXT_STATE_NONE) {
            this->jumpTimer = rnd_f(100.0f) + 250.0f;
            this->actionFunc = mode_run_init;
        } else {
            Actor_talk_request2(&this->actor, play, 100.0f);
        }
    }
}

void En_Niw_Girl_actor_move(Actor* thisx, PlayState* play) {
    EnNiwGirl* this = (EnNiwGirl*)thisx;
    EnNiwGirlActionFunc tempActionFunc;
    Player* player = GET_PLAYER(play);

    Actor_set_scale(&this->actor, 0.013f);
    this->unkUpTimer++;
    tempActionFunc = mode_wait;
    if (this->blinkTimer == 0) {
        this->eyeIndex++;
        if (this->eyeIndex >= 3) {
            this->eyeIndex = 0;
            this->blinkTimer = (s16)rnd_f(60.0f) + 20;
        }
    }
    this->unk_280 = 30.0f;
    Actor_world_to_eye(&this->actor, 30.0f);
    if (tempActionFunc == this->actionFunc) {
        this->interactInfo.trackPos = player->actor.world.pos;
        if (!LINK_IS_ADULT) {
            this->interactInfo.trackPos.y = player->actor.world.pos.y - 10.0f;
        }
        eye_moveM(&this->actor, &this->interactInfo, 2, NPC_TRACKING_FULL_BODY);
        this->headRot = this->interactInfo.headRot;
        this->torsoRot = this->interactInfo.torsoRot;
    } else {
        add_calc_short_angle2(&this->torsoRot.y, 0, 5, 3000, 0);
        add_calc_short_angle2(&this->headRot.y, 0, 5, 3000, 0);
        add_calc_short_angle2(&this->headRot.z, 0, 5, 3000, 0);
    }
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    }
    if (this->jumpTimer != 0) {
        this->jumpTimer--;
    }
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 100.0f, 100.0f, 200.0f,
                            UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

s32 En_Niw_Girl_draw_sub(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnNiwGirl* this = (EnNiwGirl*)thisx;

    if (limbIndex == 3) {
        rot->x += this->torsoRot.y;
    }
    if (limbIndex == 4) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.z;
    }
    return false;
}

static Vec3f sConstVec3f = { 0.2f, 0.2f, 0.2f };

void En_Niw_Girl_actor_draw(Actor* thisx, PlayState* play) {
    static void* bg_eye_txt[] = { gNiwGirlEyeOpenTex, gNiwGirlEyeHalfTex, gNiwGirlEyeClosedTex };
    EnNiwGirl* this = (EnNiwGirl*)thisx;
    s32 pad;
    Vec3f sp4C = sConstVec3f;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_niw_girl.c", 573);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(bg_eye_txt[this->eyeIndex]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          En_Niw_Girl_draw_sub, NULL, this);
    Shadow_draw(&this->actor.world.pos, &sp4C, 255, play);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_niw_girl.c", 592);
}
