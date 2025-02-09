/*
 * File: z_en_jj.c
 * Overlay: ovl_En_Jj
 * Description: Lord Jabu-Jabu
 */

#include "z_en_jj.h"
#include "assets/objects/object_jj/object_jj.h"
#include "overlays/actors/ovl_Eff_Dust/z_eff_dust.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

typedef enum EnJjEyeState {
    /* 0 */ JABUJABU_EYE_OPEN,
    /* 1 */ JABUJABU_EYE_HALF,
    /* 2 */ JABUJABU_EYE_CLOSED,
    /* 3 */ JABUJABU_EYE_MAX
} EnJjEyeState;

void En_Jj_actor_ct(Actor* thisx, PlayState* play2);
void En_Jj_actor_dt(Actor* thisx, PlayState* play);
void En_Jj_actor_move(Actor* thisx, PlayState* play);
void En_Jj_actor_draw(Actor* thisx, PlayState* play2);

void En_Jj_actor_mvbg(Actor* thisx, PlayState* play);
static void move_wait3(EnJj* this, PlayState* play);
static void move_wait(EnJj* this, PlayState* play);
static void move_wait2(EnJj* this, PlayState* play);
static void move_open(EnJj* this, PlayState* play);

ActorProfile En_Jj_Profile = {
    /**/ ACTOR_EN_JJ,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_JJ,
    /**/ sizeof(EnJj),
    /**/ En_Jj_actor_ct,
    /**/ En_Jj_actor_dt,
    /**/ En_Jj_actor_move,
    /**/ En_Jj_actor_draw,
};

static s32 dummy = 0;

#include "z_en_jj.inc.c"

static s32 sUnused2[] = { 0, 0 };

static ColliderCylinderInit EnJjOcInfoData = {
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
        { 0x00000004, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 170, 150, 0, { 0, 0, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 87, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 3300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1100, ICHAIN_STOP),
};

void En_Jj_actor_set_process(EnJj* this, EnJjActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_Jj_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnJj* this = (EnJj*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    Shape_Info_init(&this->dyna.actor.shape, 0.0f, NULL, 0.0f);

    switch (this->dyna.actor.params) {
        case JABUJABU_MAIN:
            Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gJabuJabuSkel, &gJabuJabuAnim, this->jointTable,
                               this->morphTable, 22);
            Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gJabuJabuAnim);
            this->unk_30A = 0;
            this->eyeIndex = 0;
            this->blinkTimer = 0;
            this->extraBlinkCounter = 0;
            this->extraBlinkTotal = 0;

            if (GET_EVENTCHKINF(EVENTCHKINF_OPENED_JABU_JABU)) {
                En_Jj_actor_set_process(this, move_wait3);
            } else {
                En_Jj_actor_set_process(this, move_wait);
            }

            this->bodyCollisionActor = (DynaPolyActor*)Actor_info_make_child_actor(
                &play->actorCtx, &this->dyna.actor, play, ACTOR_EN_JJ, this->dyna.actor.world.pos.x - 10.0f,
                this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z, 0, this->dyna.actor.world.rot.y, 0,
                JABUJABU_COLLISION);
            MoveBG_ct(&this->dyna, 0);
            DynaPolyUty_bgdi_SG2KSG(&gJabuJabuHeadCol, &colHeader);
            this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set5(play, &this->collider, &this->dyna.actor, &EnJjOcInfoData);
            this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;
            break;

        case JABUJABU_COLLISION:
            MoveBG_ct(&this->dyna, 0);
            DynaPolyUty_bgdi_SG2KSG(&gJabuJabuBodyCol, &colHeader);
            this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            DynaPolygonInfo_setRoofOff(play, &play->colCtx.dyna, this->dyna.bgId);
            this->dyna.actor.update = En_Jj_actor_mvbg;
            this->dyna.actor.draw = NULL;
            Actor_set_scale(&this->dyna.actor, 0.087f);
            break;

        case JABUJABU_UNUSED_COLLISION:
            MoveBG_ct(&this->dyna, 0);
            DynaPolyUty_bgdi_SG2KSG(&gJabuJabuUnusedCol, &colHeader);
            this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
            this->dyna.actor.update = En_Jj_actor_mvbg;
            this->dyna.actor.draw = NULL;
            Actor_set_scale(&this->dyna.actor, 0.087f);
            break;
    }
}

void En_Jj_actor_dt(Actor* thisx, PlayState* play) {
    EnJj* this = (EnJj*)thisx;

    switch (this->dyna.actor.params) {
        case JABUJABU_MAIN:
            DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
            ClObjPipe_dt(play, &this->collider);
            break;

        case JABUJABU_COLLISION:
        case JABUJABU_UNUSED_COLLISION:
            DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
            break;
    }
}

/**
 * Blink routine. Blinks at the end of each randomised blinkTimer cycle. If extraBlinkCounter is not zero, blink that
 * many more times before resuming random blinkTimer cycles. extraBlinkTotal can be set to a positive number to blink
 * that many extra times at the end of every blinkTimer cycle, but the actor always sets it to zero, so only one
 * multiblink happens when extraBlinkCounter is nonzero.
 */
void eye_process(EnJj* this) {
    if (this->blinkTimer > 0) {
        this->blinkTimer--;
    } else {
        this->eyeIndex++;
        if (this->eyeIndex >= JABUJABU_EYE_MAX) {
            this->eyeIndex = JABUJABU_EYE_OPEN;
            if (this->extraBlinkCounter > 0) {
                this->extraBlinkCounter--;
            } else {
                this->blinkTimer = get_random_timer(20, 20);
                this->extraBlinkCounter = this->extraBlinkTotal;
            }
        }
    }
}

void move_open2(EnJj* this, PlayState* play) {
    DynaPolyActor* bodyCollisionActor = this->bodyCollisionActor;

    if (this->mouthOpenAngle >= -5200) {
        this->mouthOpenAngle -= 102;

        if (this->mouthOpenAngle < -2600) {
            DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, bodyCollisionActor->bgId);
        }
    }
}

static void move_wait3(EnJj* this, PlayState* play) {
    if (this->dyna.actor.xzDistToPlayer < 300.0f) {
        En_Jj_actor_set_process(this, move_open2);
    }
}

static void move_wait(EnJj* this, PlayState* play) {
    static Vec3f pos = { -1589.0f, 53.0f, -43.0f };
    Player* player = GET_PLAYER(play);

    if ((search_position_distanceXZ(&pos, &player->actor.world.pos) < 300.0f) && play->isPlayerDroppingFish(play)) {
        this->cutsceneCountdownTimer = 100;
        En_Jj_actor_set_process(this, move_wait2);
    }

    this->collider.dim.pos.x = -1245;
    this->collider.dim.pos.y = 20;
    this->collider.dim.pos.z = -48;
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

static void move_wait2(EnJj* this, PlayState* play) {
    DynaPolyActor* bodyCollisionActor = this->bodyCollisionActor;

    if (this->cutsceneCountdownTimer > 0) {
        this->cutsceneCountdownTimer--;
    } else {
        En_Jj_actor_set_process(this, move_open);
        play->csCtx.script = JyabaJyaba_Demo_data;
        z_common_data.cutsceneTrigger = 1;
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, bodyCollisionActor->bgId);
        restartCameraStoped(GET_ACTIVE_CAM(play));
        SET_EVENTCHKINF(EVENTCHKINF_OPENED_JABU_JABU);
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
    }
}

static void move_demo(EnJj* this, PlayState* play) {
    switch (play->csCtx.actorCues[2]->id) {
        case 1:
            if (this->unk_30A & 2) {
                this->eyeIndex = 0;
                this->blinkTimer = get_random_timer(20, 20);
                this->extraBlinkCounter = 0;
                this->extraBlinkTotal = 0;
                this->unk_30A ^= 2;
            }
            break;

        case 2:
            this->unk_30A |= 1;

            if (!(this->unk_30A & 8)) {
                this->dust = Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_EFF_DUST, -1100.0f,
                                                105.0f, -27.0f, 0, 0, 0, EFF_DUST_TYPE_0);
                this->unk_30A |= 8;
            }
            break;

        case 3:
            if (!(this->unk_30A & 2)) {
                this->eyeIndex = 0;
                this->blinkTimer = 0;
                this->extraBlinkCounter = 1;
                this->extraBlinkTotal = 0;
                this->unk_30A |= 2;
            }
            break;
    }

    if (this->unk_30A & 1) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_JABJAB_BREATHE - SFX_FLAG);

        if (this->mouthOpenAngle >= -5200) {
            this->mouthOpenAngle -= 102;
        }
    }
}

static void move_open(EnJj* this, PlayState* play) {
    Actor* dust;

    if (!(this->unk_30A & 4)) {
        this->unk_30A |= 4;
        dust = this->dust;

        if (dust != NULL) {
            Actor_delete(dust);
            this->dyna.actor.child = NULL;
        }
    }
}

void En_Jj_actor_mvbg(Actor* thisx, PlayState* play) {
}

void En_Jj_actor_move(Actor* thisx, PlayState* play) {
    EnJj* this = (EnJj*)thisx;

    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[2] != NULL)) {
        move_demo(this, play);
    } else {
        this->actionFunc(this, play);

        if (this->skelAnime.curFrame == 41.0f) {
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_JABJAB_GROAN);
        }
    }

    eye_process(this);
    Skeleton_Info2_anime_play(&this->skelAnime);
    Actor_set_scale(&this->dyna.actor, 0.087f);

    // Head
    this->skelAnime.jointTable[10].z = this->mouthOpenAngle;
}

void En_Jj_actor_draw(Actor* thisx, PlayState* play2) {
    static void* jj_eye[] = { gJabuJabuEyeOpenTex, gJabuJabuEyeHalfTex, gJabuJabuEyeClosedTex };
    PlayState* play = play2;
    EnJj* this = (EnJj*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_jj.c", 879);

    _polygon_z_light_fog_prim(play->state.gfxCtx);
    Matrix_translate(0.0f, (cosf(this->skelAnime.curFrame * (M_PI / 41.0f)) * 10.0f) - 10.0f, 0.0f, MTXMODE_APPLY);
    Matrix_scale(10.0f, 10.0f, 10.0f, MTXMODE_APPLY);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(jj_eye[this->eyeIndex]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_jj.c", 898);
}
