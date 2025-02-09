/*
 * File: z_en_dnt_jiji.c
 * Overlay: ovl_En_Dnt_Jiji
 * Description: Forest Stage scrub leader
 */

#include "z_en_dnt_jiji.h"
#include "assets/objects/object_dns/object_dns.h"
#include "overlays/actors/ovl_En_Dnt_Demo/z_en_dnt_demo.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void En_Dnt_Jiji_actor_ct(Actor* thisx, PlayState* play);
void En_Dnt_Jiji_actor_dt(Actor* thisx, PlayState* play);
void En_Dnt_Jiji_actor_move(Actor* thisx, PlayState* play);
void En_Dnt_Jiji_actor_draw(Actor* thisx, PlayState* play);

static void mode_leaf_check(EnDntJiji* this, PlayState* play);

static void mode_wait_init(EnDntJiji* this, PlayState* play);
static void mode_jump_init(EnDntJiji* this, PlayState* play);
static void mode_walk_init(EnDntJiji* this, PlayState* play);
void mode_half_down_init(EnDntJiji* this, PlayState* play);
static void mode_player_item_request_check(EnDntJiji* this, PlayState* play);
void mode_half_down_drop_init(EnDntJiji* this, PlayState* play);
static void mode_runaway_init(EnDntJiji* this, PlayState* play);

static void mode_wait(EnDntJiji* this, PlayState* play);
static void mode_start(EnDntJiji* this, PlayState* play);
static void mode_jump(EnDntJiji* this, PlayState* play);
static void mode_walk(EnDntJiji* this, PlayState* play);
static void mode_down(EnDntJiji* this, PlayState* play);
static void mode_messege_check(EnDntJiji* this, PlayState* play);
void mode_berabera_init(EnDntJiji* this, PlayState* play);
void mode_player_item_request_init(EnDntJiji* this, PlayState* play);
static void mode_player_item_request_up(EnDntJiji* this, PlayState* play);
void mode_half_down_drop(EnDntJiji* this, PlayState* play);
static void mode_runaway(EnDntJiji* this, PlayState* play);

ActorProfile En_Dnt_Jiji_Profile = {
    /**/ ACTOR_EN_DNT_JIJI,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DNS,
    /**/ sizeof(EnDntJiji),
    /**/ En_Dnt_Jiji_actor_ct,
    /**/ En_Dnt_Jiji_actor_dt,
    /**/ En_Dnt_Jiji_actor_move,
    /**/ En_Dnt_Jiji_actor_draw,
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
    { 30, 80, 0, { 0, 0, 0 } },
};

void En_Dnt_Jiji_actor_ct(Actor* thisx, PlayState* play) {
    EnDntJiji* this = (EnDntJiji*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 0.0f);
    Skeleton_Info2_M_ct(play, &this->skelAnime, &gDntJijiSkel, &gDntJijiBurrowAnim, this->jointTable, this->morphTable, 13);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
    this->stage = (EnDntDemo*)this->actor.parent;
    PRINTF("\n\n");
    // "Deku Scrub mask show elder"
    PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ デグナッツお面品評会長老 ☆☆☆☆☆ %x\n" VT_RST, this->stage);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->actionFunc = mode_leaf_check;
    this->actor.gravity = -2.0f;
}

void En_Dnt_Jiji_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDntJiji* this = (EnDntJiji*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void mode_leaf_check(EnDntJiji* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->flowerPos = this->actor.world.pos;
        this->actionFunc = mode_wait_init;
    }
}

static void mode_wait_init(EnDntJiji* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntJijiBurrowAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntJijiBurrowAnim, 0.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->skelAnime.curFrame = 8.0f;
    this->isSolid = this->action = DNT_LEADER_ACTION_NONE;
    this->actionFunc = mode_wait;
}

static void mode_wait(EnDntJiji* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((this->timer == 1) && (this->actor.xzDistToPlayer < 150.0f) && !Game_play_demo_mode_check(play) &&
        !(player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR)) {
        makeOnepointDemo(play, 2230, -99, &this->actor, CAM_ID_MAIN);
        this->timer = 0;
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_8);
        this->actionFunc = mode_jump_init;
    }
}

static void mode_start_init(EnDntJiji* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntJijiUpAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntJijiUpAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
    Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 6.0f, 0, 15, 5, 20, HAHEN_OBJECT_DEFAULT, 10, NULL);
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_UP);
    this->actionFunc = mode_start;
}

static void mode_start(EnDntJiji* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
    if (this->actor.xzDistToPlayer < 150.0f) {
        this->actionFunc = mode_half_down_init;
    }
}

static void mode_jump_init(EnDntJiji* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntJijiUnburrowAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntJijiUnburrowAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
    Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 6.0f, 0, 15, 5, 20, HAHEN_OBJECT_DEFAULT, 10, NULL);
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_UP);
    this->actionFunc = mode_jump;
}

static void mode_jump(EnDntJiji* this, PlayState* play) {
    f32 frame = this->skelAnime.curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->endFrame <= frame) {
        if (this->action != DNT_LEADER_ACTION_ATTACK) {
            this->actionFunc = mode_walk_init;
        } else {
            this->actionFunc = mode_runaway_init;
        }
    }
}

static void mode_walk_init(EnDntJiji* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntJijiWalkAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntJijiWalkAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.speed = 1.0f;
    this->isSolid = true;
    this->unburrow = true;
    this->actionFunc = mode_walk;
}

static void mode_walk(EnDntJiji* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x3E8, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    add_calc2(&this->actor.speed, 1.0f, 0.2f, 0.4f);
    if (this->sfxTimer == 0) {
        this->sfxTimer = 5;
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actor.velocity.y = 9.0f;
        this->actor.speed = 3.0f;
    }
    if (this->actor.xzDistToPlayer < 100.0f) {
        if (CUR_UPG_VALUE(UPG_DEKU_STICKS) == 1) {
            this->getItemId = GI_DEKU_STICK_UPGRADE_20;
        } else {
            this->getItemId = GI_DEKU_STICK_UPGRADE_30;
        }
        this->actor.textId = 0x104D;
        message_set(play, this->actor.textId, NULL);
        this->actor.speed = 0.0f;
        this->unused = 5;
        this->actionFunc = mode_player_item_request_init;
    }
}

static void mode_down_init(EnDntJiji* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntJijiBurrowAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntJijiBurrowAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
    Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 6.0f, 0, 15, 5, 20, HAHEN_OBJECT_DEFAULT, 10, NULL);
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_UP);
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DOWN);
    this->actionFunc = mode_down;
}

static void mode_down(EnDntJiji* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
}

void mode_half_down_init(EnDntJiji* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntJijiCowerAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntJijiCowerAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
    Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 3.0f, 0, 9, 3, 10, HAHEN_OBJECT_DEFAULT, 10, NULL);
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_UP);

    if ((CUR_UPG_VALUE(UPG_DEKU_NUTS) == 1) || (CUR_UPG_VALUE(UPG_DEKU_NUTS) == 0)) {
        this->getItemId = GI_DEKU_NUT_UPGRADE_30;
    } else {
        this->getItemId = GI_DEKU_NUT_UPGRADE_40;
    }
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.textId = 0x10DB;
    this->unused = 5;
    this->actionFunc = mode_messege_check;
}

static void mode_messege_check(EnDntJiji* this, PlayState* play) {
    f32 frame = this->skelAnime.curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
    if (frame >= this->endFrame) {
        if (Actor_talk_check(&this->actor, play)) {
            this->actionFunc = mode_berabera_init;
        } else {
            Actor_talk_request2(&this->actor, play, 100.0f);
        }
    }
}

void mode_berabera_init(EnDntJiji* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntJijiTalkAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntJijiTalkAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->actionFunc = mode_player_item_request_init;
}

void mode_player_item_request_init(EnDntJiji* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 0x1388, 0);
    if ((message_check(&play->msgCtx) == TEXT_STATE_EVENT) && pad_on_check(play)) {
        restartCameraStoped(GET_ACTIVE_CAM(play));
        message_close(play);
        player_demo_mode_set(play, NULL, PLAYER_CSACTION_7);
        this->actor.parent = NULL;
        Actor_carry_request_set2(&this->actor, play, this->getItemId, 400.0f, 200.0f);
        this->actionFunc = mode_player_item_request_check;
    }
}

static void mode_player_item_request_check(EnDntJiji* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Actor_carry_check(&this->actor, play)) {
        this->actionFunc = mode_player_item_request_up;
    } else {
        Actor_carry_request_set2(&this->actor, play, this->getItemId, 400.0f, 200.0f);
    }
}

static void mode_player_item_request_up(EnDntJiji* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        if ((this->getItemId == GI_DEKU_NUT_UPGRADE_30) || (this->getItemId == GI_DEKU_NUT_UPGRADE_40)) {
            // "nut"
            PRINTF("実 \n");
            PRINTF("実 \n");
            PRINTF("実 \n");
            PRINTF("実 \n");
            PRINTF("実 \n");
            PRINTF("実 \n");
            PRINTF("実 \n");
            PRINTF("実 \n");
            SET_ITEMGETINF(ITEMGETINF_FOREST_STAGE_NUT_UPGRADE);
        } else {
            // "stick"
            PRINTF("棒 \n");
            PRINTF("棒 \n");
            PRINTF("棒 \n");
            PRINTF("棒 \n");
            PRINTF("棒 \n");
            PRINTF("棒 \n");
            SET_ITEMGETINF(ITEMGETINF_FOREST_STAGE_STICK_UPGRADE);
        }
        this->actor.textId = 0;
        if ((this->stage != NULL) && (this->stage->actor.update != NULL)) {
            this->stage->action = DNT_ACTION_NONE;
            if (!this->unburrow) {
                this->stage->leaderSignal = DNT_SIGNAL_HIDE;
            } else {
                this->stage->leaderSignal = DNT_SIGNAL_RETURN;
            }
        }
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        if (!this->unburrow) {
            this->actionFunc = mode_half_down_drop_init;
        } else {
            this->actionFunc = mode_runaway_init;
        }
    }
}

void mode_half_down_drop_init(EnDntJiji* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntJijiHideAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntJijiHideAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_ONCE, -10.0f);
    this->actionFunc = mode_half_down_drop;
}

void mode_half_down_drop(EnDntJiji* this, PlayState* play) {
    f32 frame = this->skelAnime.curFrame;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->endFrame <= frame) {
        this->actionFunc = mode_wait_init;
    }
}

static void mode_runaway_init(EnDntJiji* this, PlayState* play) {
    this->endFrame = (f32)Si2_anime_end_frame(&gDntJijiWalkAnim);
    Skeleton_Info2_init(&this->skelAnime, &gDntJijiWalkAnim, 1.0f, 0.0f, this->endFrame, ANIMMODE_LOOP, -10.0f);
    this->actor.speed = 2.0f;
    this->isSolid = this->unburrow = true;
    this->actionFunc = mode_runaway;
}

static void mode_runaway(EnDntJiji* this, PlayState* play) {
    f32 dx;
    f32 dz;

    Skeleton_Info2_anime_play(&this->skelAnime);
    dx = this->flowerPos.x - this->actor.world.pos.x;
    dz = this->flowerPos.z - this->actor.world.pos.z;
    add_calc_short_angle2(&this->actor.shape.rot.y, RAD_TO_BINANG(fatan2(dx, dz)), 1, 0xBB8, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actor.velocity.y = 9.0f;
        this->actor.speed = 3.0f;
    }
    if (this->sfxTimer == 0) {
        this->sfxTimer = 3;
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    if ((fabsf(dx) < 5.0f) && (fabsf(dz) < 5.0f)) {
        this->actor.world.pos.x = this->flowerPos.x;
        this->actor.world.pos.z = this->flowerPos.z;
        if (this->attackFlag) {
            if ((this->stage->actor.update != NULL) && (this->stage->leaderSignal == DNT_SIGNAL_NONE)) {
                this->stage->leaderSignal = DNT_SIGNAL_HIDE;
                this->stage->action = DNT_ACTION_ATTACK;
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 8, NA_BGM_ENEMY);
            }
        }
        this->actor.speed = 0.0f;
        this->isSolid = 0;
        this->actionFunc = mode_down_init;
    }
}

void En_Dnt_Jiji_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDntJiji* this = (EnDntJiji*)thisx;

    Actor_set_scale(&this->actor, 0.015f);
    this->unkTimer++;
    if (BREG(0)) {
        // "time"
        PRINTF(VT_FGCOL(YELLOW) "☆☆☆☆☆ 時間 ☆☆☆☆☆ %d\n" VT_RST, this->timer);
    }
    if ((this->timer > 1) && (this->timer != 0)) {
        this->timer--;
    }
    if (this->sfxTimer != 0) {
        this->sfxTimer--;
    }
    if (this->blinkTimer != 0) {
        this->blinkTimer--;
    }
    switch (this->stageSignal) {
        case DNT_LEADER_SIGNAL_UP:
            this->isSolid = true;
            this->action = DNT_LEADER_ACTION_UP;
            this->actionFunc = mode_start_init;
            break;
        case DNT_LEADER_SIGNAL_BURROW:
            this->isSolid = false;
            this->action = DNT_LEADER_ACTION_NONE;
            this->actionFunc = mode_down_init;
            break;
        case DNT_LEADER_SIGNAL_RETURN:
            this->actionFunc = mode_runaway_init;
            break;
        case DNT_LEADER_SIGNAL_NONE:
            break;
    }
    if (this->actor.textId != 0) {
        Actor_world_to_eye(&this->actor, 30.0f);
    }
    if (this->stageSignal != DNT_LEADER_SIGNAL_NONE) {
        this->stageSignal = DNT_LEADER_SIGNAL_NONE;
    }
    if (this->blinkTimer == 0) {
        this->eyeState++;
        if (this->eyeState > 2) {
            this->eyeState = 0;
            this->blinkTimer = (s16)rnd_f(60.0f) + 20;
        }
    }
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 20.0f, 20.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                UPDBGCHECKINFO_FLAG_4);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    if (this->isSolid != 0) {
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void En_Dnt_Jiji_actor_draw(Actor* thisx, PlayState* play) {
    static void* dns_eye_txt[] = { gDntJijiEyeOpenTex, gDntJijiEyeHalfTex, gDntJijiEyeShutTex };
    EnDntJiji* this = (EnDntJiji*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_dnt_jiji.c", 1019);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Matrix_push();
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(dns_eye_txt[this->eyeState]));
    Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, NULL, NULL, this);
    Matrix_pull();
    Matrix_translate(this->flowerPos.x, this->flowerPos.y, this->flowerPos.z, MTXMODE_NEW);
    Matrix_scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_dnt_jiji.c", 1040);
    gSPDisplayList(POLY_OPA_DISP++, gDntJijiFlowerDL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_dnt_jiji.c", 1043);
}
