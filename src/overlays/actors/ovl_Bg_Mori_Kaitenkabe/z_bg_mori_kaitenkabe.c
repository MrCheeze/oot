/*
 * File: z_bg_mori_kaitenkabe.c
 * Overlay: ovl_Bg_Mori_Kaitenkabe
 * Description: Rotating wall in Forest Temple basement
 */

#include "z_bg_mori_kaitenkabe.h"
#include "assets/objects/object_mori_objects/object_mori_objects.h"

#define FLAGS 0

void Bg_Mori_Kaitenkabe_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mori_Kaitenkabe_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mori_Kaitenkabe_actor_move(Actor* thisx, PlayState* play);
void Bg_Mori_Kaitenkabe_actor_draw(Actor* thisx, PlayState* play);

static void mv_dmaWait(BgMoriKaitenkabe* this, PlayState* play);
static void mvSet_stop(BgMoriKaitenkabe* this);
static void mv_stop(BgMoriKaitenkabe* this, PlayState* play);
void mvSet_kaiten(BgMoriKaitenkabe* this);
void mv_kaiten(BgMoriKaitenkabe* this, PlayState* play);

ActorProfile Bg_Mori_Kaitenkabe_Profile = {
    /**/ ACTOR_BG_MORI_KAITENKABE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MORI_OBJECTS,
    /**/ sizeof(BgMoriKaitenkabe),
    /**/ Bg_Mori_Kaitenkabe_actor_ct,
    /**/ Bg_Mori_Kaitenkabe_actor_dt,
    /**/ Bg_Mori_Kaitenkabe_actor_move,
    /**/ NULL,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static void gaiseki(Vec3f* dest, Vec3f* v1, Vec3f* v2) {
    dest->x = (v1->y * v2->z) - (v1->z * v2->y);
    dest->y = (v1->z * v2->x) - (v1->x * v2->z);
    dest->z = (v1->x * v2->y) - (v1->y * v2->x);
}

void Bg_Mori_Kaitenkabe_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriKaitenkabe* this = (BgMoriKaitenkabe*)thisx;
    CollisionHeader* colHeader = NULL;

    // "Forest Temple object 【Rotating Wall (arg_data: 0x% 04x)】 appears"
    PRINTF("◯◯◯森の神殿オブジェクト【回転壁(arg_data : 0x%04x)】出現 \n", this->dyna.actor.params);
    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gMoriKaitenkabeCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    this->moriTexObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_TEX);
    if (this->moriTexObjectSlot < 0) {
        Actor_delete(&this->dyna.actor);
        // "【Rotating wall】 Bank danger!"
        PRINTF("【回転壁】 バンク危険！(%s %d)\n", "../z_bg_mori_kaitenkabe.c", 176);
    } else {
        this->actionFunc = mv_dmaWait;
    }
}

void Bg_Mori_Kaitenkabe_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriKaitenkabe* this = (BgMoriKaitenkabe*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mv_dmaWait(BgMoriKaitenkabe* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->moriTexObjectSlot)) {
        mvSet_stop(this);
        this->dyna.actor.draw = Bg_Mori_Kaitenkabe_actor_draw;
    }
}

static void mvSet_stop(BgMoriKaitenkabe* this) {
    this->actionFunc = mv_stop;
    this->timer = 0;
}

static void mv_stop(BgMoriKaitenkabe* this, PlayState* play) {
    Vec3f push;
    Vec3f leverArm;
    Vec3f torque;
    Player* player = GET_PLAYER(play);

    if (this->dyna.unk_150 > 0.001f) {
        this->timer++;
        if ((this->timer > 28) && !player_demo_check(play)) {
            mvSet_kaiten(this);
            player_demo_mode_set(play, &this->dyna.actor, PLAYER_CSACTION_8);
            xyz_t_move(&this->lockedPlayerPos, &player->actor.world.pos);
            push.x = sin_s(this->dyna.unk_158);
            push.y = 0.0f;
            push.z = cos_s(this->dyna.unk_158);
            leverArm.x = this->dyna.actor.world.pos.x - player->actor.world.pos.x;
            leverArm.y = 0.0f;
            leverArm.z = this->dyna.actor.world.pos.z - player->actor.world.pos.z;
            gaiseki(&torque, &push, &leverArm);
            this->rotDirection = (torque.y > 0.0f) ? 1.0f : -1.0f;
        }
    } else {
        this->timer = 0;
    }
    if (fabsf(this->dyna.unk_150) > 0.001f) {
        this->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
    }
}

void mvSet_kaiten(BgMoriKaitenkabe* this) {
    this->actionFunc = mv_kaiten;
    this->rotSpeed = 0.0f;
    this->rotYdeg = 0.0f;
}

void mv_kaiten(BgMoriKaitenkabe* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* thisx = &this->dyna.actor;
    s16 rotY;

    chase_f(&this->rotSpeed, 0.6f, 0.02f);
    if (chase_f(&this->rotYdeg, this->rotDirection * 45.0f, this->rotSpeed)) {
        mvSet_stop(this);
        player_demo_mode_set(play, thisx, PLAYER_CSACTION_7);
        if (this->rotDirection > 0.0f) {
            thisx->home.rot.y += 0x2000;
        } else {
            thisx->home.rot.y -= 0x2000;
        }
        thisx->world.rot.y = thisx->shape.rot.y = thisx->home.rot.y;
        Na_StartFixSe_F(NA_SE_EV_STONEDOOR_STOP);
    } else {
        rotY = DEG_TO_BINANG(this->rotYdeg);
        thisx->world.rot.y = thisx->shape.rot.y = thisx->home.rot.y + rotY;
        Na_StartFixSe_F(NA_SE_EV_WALL_SLIDE - SFX_FLAG);
    }
    if (fabsf(this->dyna.unk_150) > 0.001f) {
        this->dyna.unk_150 = 0.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_4;
    }
    xyz_t_move(&player->actor.world.pos, &this->lockedPlayerPos);
}

void Bg_Mori_Kaitenkabe_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriKaitenkabe* this = (BgMoriKaitenkabe*)thisx;

    this->actionFunc(this, play);
}

void Bg_Mori_Kaitenkabe_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriKaitenkabe* this = (BgMoriKaitenkabe*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_mori_kaitenkabe.c", 347);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, play->objectCtx.slots[this->moriTexObjectSlot].segment);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mori_kaitenkabe.c", 352);

    gSPDisplayList(POLY_OPA_DISP++, gMoriKaitenkabeDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_mori_kaitenkabe.c", 356);
}
