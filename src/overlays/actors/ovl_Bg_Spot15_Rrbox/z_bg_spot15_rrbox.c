/*
 * File: z_bg_spot15_rrbox.c
 * Overlay: ovl_Bg_Spot15_Rrbox
 * Description: Milk Crate
 */

#include "z_bg_spot15_rrbox.h"
#include "assets/objects/object_spot15_obj/object_spot15_obj.h"

#define FLAGS 0

void Bg_Spot15_Rrbox_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot15_Rrbox_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot15_Rrbox_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot15_Rrbox_actor_draw(Actor* thisx, PlayState* play);

static void mv_stopNoCheck_init(BgSpot15Rrbox* this, PlayState* play);
static void mv_stopNoCheck(BgSpot15Rrbox* this, PlayState* play);
static void mv_slip(BgSpot15Rrbox* this, PlayState* play);
static void mv_drop_init(BgSpot15Rrbox* this, PlayState* play);
static void mv_drop(BgSpot15Rrbox* this, PlayState* play);
static void mv_end_init(BgSpot15Rrbox* this, PlayState* play);
static void mv_slip_init(BgSpot15Rrbox* this, PlayState* play);
static void mv_end(BgSpot15Rrbox* this, PlayState* play);

static s16 RRB_make_cnt = 0;

ActorProfile Bg_Spot15_Rrbox_Profile = {
    /**/ ACTOR_BG_SPOT15_RRBOX,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT15_OBJ,
    /**/ sizeof(BgSpot15Rrbox),
    /**/ Bg_Spot15_Rrbox_actor_ct,
    /**/ Bg_Spot15_Rrbox_actor_dt,
    /**/ Bg_Spot15_Rrbox_actor_move,
    /**/ Bg_Spot15_Rrbox_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

static Vec3f make_pos[] = {
    { 770.0f, 1490.0f, -299.0f },
    { 770.0f, 1550.0f, -299.0f },
};

// list of ground check positions, relative to 1/10th model space
static Vec3f G_CheckPos[] = {
    { 29.99f, 0.01f, -29.99f }, { -29.99f, 0.01f, -29.99f }, { -29.99f, 0.01f, 29.99f },
    { 29.99f, 0.01f, 29.99f },  { 0.0f, 0.01f, 0.0f },
};

static void set_dynaPoly(BgSpot15Rrbox* this, PlayState* play, CollisionHeader* collision, s32 flags) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, flags);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_spot15_rrbox.c", 171,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

static void kaiten_Y(Vec3f* outPos, Vec3f* pos, f32 arg2, f32 arg3) {
    outPos->x = (pos->z * arg2) + (pos->x * arg3);
    outPos->y = pos->y;
    outPos->z = (pos->z * arg3) - (pos->x * arg2);
}

static void clear_pbg_actor_index(BgSpot15Rrbox* this) {
    this->bgId = BG_ACTOR_MAX;
}

s32 check_bingo_pos(BgSpot15Rrbox* this, PlayState* play) {
    DynaPolyActor* dynaPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, this->bgId);

    if ((dynaPolyActor != NULL) &&
        Math3DLengthSquare2D(dynaPolyActor->actor.world.pos.x, dynaPolyActor->actor.world.pos.z,
                        this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.z) < 0.01f) {
        return true;
    }
    return false;
}

static s32 checkAblMove(BgSpot15Rrbox* this, PlayState* play) {
    s16 rotY;
    Actor* actor = &this->dyna.actor;

    if (play->sceneId == SCENE_LON_LON_BUILDINGS) {
        return true;
    } else if (check_bingo_pos(this, play)) {
        return false;
    }

    if (actor->world.pos.x <= 930.0f && actor->world.pos.z >= -360.0f) {
        if (this->dyna.unk_150 >= 0.0f) {
            rotY = actor->world.rot.y;
        } else {
            rotY = actor->world.rot.y + 0x8000;
        }

        if (rotY < 0x2000 && rotY > -0x6000) {
            return GET_EVENTCHKINF(EVENTCHKINF_TALON_RETURNED_FROM_CASTLE);
        }
        return true;
    }

    return true;
}

void Bg_Spot15_Rrbox_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot15Rrbox* this = (BgSpot15Rrbox*)thisx;

    set_dynaPoly(this, play, &gLonLonMilkCrateCol, 0);
    ValueSet_process(&this->dyna.actor, value_init);
    clear_pbg_actor_index(this);
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
        mv_end_init(this, play);
        this->dyna.actor.world.pos = make_pos[RRB_make_cnt];
        RRB_make_cnt++;
    } else {
        mv_stopNoCheck_init(this, play);
    }
    PRINTF("(spot15 ロンロン木箱)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void Bg_Spot15_Rrbox_actor_dt(Actor* thisx, PlayState* play) {
    BgSpot15Rrbox* this = (BgSpot15Rrbox*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    RRB_make_cnt = 0;
}

s32 groundCheckRevise(BgSpot15Rrbox* this, PlayState* play, s32 checkIndex) {
    f32 chkDist = 0.0f;
    Vec3f checkPos;
    Vec3f relCheckPos;

    clear_pbg_actor_index(this);

    relCheckPos.x = G_CheckPos[checkIndex].x * (this->dyna.actor.scale.x * 10.0f);
    relCheckPos.y = G_CheckPos[checkIndex].y * (this->dyna.actor.scale.y * 10.0f);
    relCheckPos.z = G_CheckPos[checkIndex].z * (this->dyna.actor.scale.z * 10.0f);

    kaiten_Y(&checkPos, &relCheckPos, this->unk_16C, this->unk_170);

    checkPos.x += this->dyna.actor.world.pos.x;
    checkPos.y += this->dyna.actor.prevPos.y;
    checkPos.z += this->dyna.actor.world.pos.z;

    this->dyna.actor.floorHeight = T_BGCheck_ObjGroundCheck_aiac_skp(&play->colCtx, &this->dyna.actor.floorPoly, &this->bgId,
                                                              &this->dyna.actor, &checkPos, chkDist);

    if ((this->dyna.actor.floorHeight - this->dyna.actor.world.pos.y) >= -0.001f) {
        this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
        return true;
    }
    return false;
}

f32 groundCheck_5_highest(BgSpot15Rrbox* this, PlayState* play) {
    s32 i;
    Vec3f checkPos;
    Vec3f relCheckPos;
    Actor* actor = &this->dyna.actor;
    f32 yIntersect;
    f32 floorY = BGCHECK_Y_MIN;
    s32 bgId;

    clear_pbg_actor_index(this);
    for (i = 0; i < ARRAY_COUNT(G_CheckPos); i++) {
        relCheckPos.x = G_CheckPos[i].x * (actor->scale.x * 10.0f);
        relCheckPos.y = G_CheckPos[i].y * (actor->scale.y * 10.0f);
        relCheckPos.z = G_CheckPos[i].z * (actor->scale.z * 10.0f);

        kaiten_Y(&checkPos, &relCheckPos, this->unk_16C, this->unk_170);

        checkPos.x += actor->world.pos.x;
        checkPos.y += actor->prevPos.y;
        checkPos.z += actor->world.pos.z;

        yIntersect = T_BGCheck_ObjGroundCheck_aiac_skp(&play->colCtx, &actor->floorPoly, &bgId, actor, &checkPos, 0);

        if (floorY < yIntersect) {
            floorY = yIntersect;
            this->bgId = bgId;
        }
    }
    return floorY;
}

s32 groundCheckRevise_5(BgSpot15Rrbox* this, PlayState* play) {
    if (groundCheckRevise(this, play, 0)) {
        return true;
    }
    if (groundCheckRevise(this, play, 1)) {
        return true;
    }
    if (groundCheckRevise(this, play, 2)) {
        return true;
    }
    if (groundCheckRevise(this, play, 3)) {
        return true;
    }
    if (groundCheckRevise(this, play, 4)) {
        return true;
    }
    return false;
}

s32 wallCheck_lower2(BgSpot15Rrbox* this, PlayState* play) {
    return !MoveBG_checkMoveArea(play, &this->dyna, this->dyna.actor.scale.x * 290.0f,
                          this->dyna.actor.scale.x * 290.0f + 20.0f, 1.0f);
}

static void mv_stopNoCheck_init(BgSpot15Rrbox* this, PlayState* play) {
    this->dyna.actor.gravity = 0.0f;
    this->dyna.actor.velocity.x = this->dyna.actor.velocity.y = this->dyna.actor.velocity.z = 0.0f;
    this->actionFunc = mv_stopNoCheck;
}

static void mv_stopNoCheck(BgSpot15Rrbox* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->unk_168 <= 0 && fabsf(this->dyna.unk_150) > 0.001f) {
        if (checkAblMove(this, play) && !wallCheck_lower2(this, play)) {
            this->unk_17C = this->dyna.unk_150;
            mv_slip_init(this, play);
        } else {
            player->stateFlags2 &= ~PLAYER_STATE2_4;
            this->dyna.unk_150 = 0.0f;
        }
    } else {
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        this->dyna.unk_150 = 0.0f;
    }
}

static void mv_slip_init(BgSpot15Rrbox* this, PlayState* play) {
    this->actionFunc = mv_slip;
    this->dyna.actor.gravity = 0.0f;
}

static void mv_slip(BgSpot15Rrbox* this, PlayState* play) {
    f32 sign;
    Player* player = GET_PLAYER(play);
    f32 tempUnk178;
    s32 approxFResult;
    Actor* actor = &this->dyna.actor;

    this->unk_174 += 0.5f;

    this->unk_174 = CLAMP_MAX(this->unk_174, 2.0f);

    approxFResult = chase_f(&this->unk_178, 20.0f, this->unk_174);

    sign = this->unk_17C >= 0.0f ? 1.0f : -1.0f;

    tempUnk178 = (f32)sign * this->unk_178;
    actor->world.pos.x = actor->home.pos.x + (tempUnk178 * this->unk_16C);
    actor->world.pos.z = actor->home.pos.z + (tempUnk178 * this->unk_170);

    if (!groundCheckRevise_5(this, play)) {
        actor->home.pos.x = actor->world.pos.x;
        actor->home.pos.z = actor->world.pos.z;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        this->dyna.unk_150 = 0.0f;
        this->unk_178 = 0.0f;
        this->unk_174 = 0.0f;
        mv_drop_init(this, play);
    } else if (approxFResult) {
        player = GET_PLAYER(play);
        if (wallCheck_lower2(this, play)) {
            Actor_SE_set(actor, NA_SE_EV_WOOD_BOUND);
        }
        if (check_bingo_pos(this, play)) {
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        }
        actor->home.pos.x = actor->world.pos.x;
        actor->home.pos.z = actor->world.pos.z;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
        this->dyna.unk_150 = 0.0f;
        this->unk_178 = 0.0f;
        this->unk_174 = 0.0f;
        this->unk_168 = 10;
        mv_stopNoCheck_init(this, play);
    }
    Actor_SE_set(actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
}

static void mv_drop_init(BgSpot15Rrbox* this, PlayState* play) {
    this->dyna.actor.velocity.x = this->dyna.actor.velocity.y = this->dyna.actor.velocity.z = 0.0f;
    this->dyna.actor.gravity = -1.0f;
    this->dyna.actor.floorHeight = groundCheck_5_highest(this, play);
    this->actionFunc = mv_drop;
}

static void mv_drop(BgSpot15Rrbox* this, PlayState* play) {
    f32 floorHeight;
    Player* player = GET_PLAYER(play);
    Actor* actor = &this->dyna.actor;

    if (fabsf(this->dyna.unk_150) > 0.001f) {
        this->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
    }

    Actor_position_moveF(actor);

    if (actor->world.pos.y <= BGCHECK_Y_MIN + 10.0f) {
        // "Lon Lon wooden crate fell too much"
        PRINTF("Warning : ロンロン木箱落ちすぎた(%s %d)(arg_data 0x%04x)\n", "../z_bg_spot15_rrbox.c", 599,
               actor->params);

        Actor_delete(actor);

        return;
    }

    floorHeight = actor->floorHeight;

    if ((floorHeight - actor->world.pos.y) >= -0.001f) {
        actor->world.pos.y = floorHeight;
        mv_stopNoCheck_init(this, play);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_WOOD_BOUND);
    }
}

static void mv_end_init(BgSpot15Rrbox* this, PlayState* play) {
    this->actionFunc = mv_end;
}

static void mv_end(BgSpot15Rrbox* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    player->stateFlags2 &= ~PLAYER_STATE2_4;
    this->dyna.unk_150 = 0.0f;
}

void Bg_Spot15_Rrbox_actor_move(Actor* thisx, PlayState* play) {
    BgSpot15Rrbox* this = (BgSpot15Rrbox*)thisx;

    if (this->unk_168 > 0) {
        this->unk_168--;
    }
    this->dyna.actor.world.rot.y = this->dyna.unk_158;
    this->unk_16C = sin_s(this->dyna.actor.world.rot.y);
    this->unk_170 = cos_s(this->dyna.actor.world.rot.y);
    this->actionFunc(this, play);
}

void Bg_Spot15_Rrbox_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gLonLonMilkCrateDL);
}
