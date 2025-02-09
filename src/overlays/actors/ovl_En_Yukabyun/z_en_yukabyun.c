/*
 * File: z_en_yukabyun.c
 * Overlay: ovl_En_Yukabyun
 * Description: Flying floor tile
 */

#include "z_en_yukabyun.h"
#include "assets/objects/object_yukabyun/object_yukabyun.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Yukabyun_actor_ct(Actor* thisx, PlayState* play);
void En_Yukabyun_actor_dt(Actor* thisx, PlayState* play);
void En_Yukabyun_actor_move(Actor* thisx, PlayState* play);
void En_Yukabyun_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(EnYukabyun* this, PlayState* play);
static void mode_up(EnYukabyun* this, PlayState* play);
static void mode_attack(EnYukabyun* this, PlayState* play);

ActorProfile En_Yukabyun_Profile = {
    /**/ ACTOR_EN_YUKABYUN,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_YUKABYUN,
    /**/ sizeof(EnYukabyun),
    /**/ En_Yukabyun_actor_ct,
    /**/ En_Yukabyun_actor_dt,
    /**/ En_Yukabyun_actor_move,
    /**/ En_Yukabyun_actor_draw,
};

static ColliderCylinderInit YukabyunAllPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_NO_PUSH | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 28, 8, 0, { 0, 0, 0 } },
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 16, ICHAIN_STOP),
};

static void* yukabyun_txt[] = { gFloorTileEnemyTopTex, gFloorTileEnemyBottomTex };

void En_Yukabyun_actor_ct(Actor* thisx, PlayState* play) {
    EnYukabyun* this = (EnYukabyun*)thisx;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 0.4f);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &YukabyunAllPipeData);
    this->actor.params++;
    this->unk_152 = 0;
    this->unk_150 = (u8)(this->actor.params) * 0xA + 0x14;
    this->actionfunc = mode_wait;
}

void En_Yukabyun_actor_dt(Actor* thisx, PlayState* play) {
    EnYukabyun* this = (EnYukabyun*)thisx;
    ClObjPipe_dt(play, &this->collider);
}

static void mode_wait(EnYukabyun* this, PlayState* play) {
    if (this->unk_150 != 0) {
        this->unk_150--;
    }
    if (this->unk_150 == 0) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_IGNORE_QUAKE;
        this->actionfunc = mode_up;
    }
}

static void mode_up(EnYukabyun* this, PlayState* play) {
    this->unk_150 += 0xA0;
    this->actor.shape.rot.y += this->unk_150;
    if (this->unk_150 >= 0x2000) {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer;
        this->actor.speed = 10.0f;
        this->actionfunc = mode_attack;
    }
    chase_f(&this->actor.world.pos.y, this->actor.home.pos.y + 30.0f, 1.0f);
    Actor_level_SE_set(&this->actor, NA_SE_EN_YUKABYUN_FLY - SFX_FLAG);
}

static void mode_attack(EnYukabyun* this, PlayState* play) {
    this->actor.shape.rot.y += this->unk_150;
    if (this->actor.xzDistToPlayer > 5000.0f) {
        Actor_delete(&this->actor);
        return;
    }
    Actor_level_SE_set(&this->actor, NA_SE_EN_YUKABYUN_FLY - SFX_FLAG);
}

static void mode_break(EnYukabyun* this, PlayState* play) {
    Effect_Hahen_Kakusan_ct3(play, &this->actor.world.pos, 8.0f, 0, 1300, 300, 15, OBJECT_YUKABYUN, 10,
                             gFloorTileEnemyFragmentDL);
    Actor_delete(&this->actor);
}

void En_Yukabyun_actor_move(Actor* thisx, PlayState* play) {
    EnYukabyun* this = (EnYukabyun*)thisx;
    s32 pad;

    if (((this->collider.base.atFlags & AT_HIT) || (this->collider.base.acFlags & AC_HIT) ||
         ((this->collider.base.ocFlags1 & OC1_HIT) && !(this->collider.base.oc->id == ACTOR_EN_YUKABYUN))) ||
        ((this->actionfunc == mode_attack) && (this->actor.bgCheckFlags & BGCHECKFLAG_WALL))) {
        this->collider.base.atFlags &= ~AT_HIT;
        this->collider.base.acFlags &= ~AC_HIT;
        this->collider.base.ocFlags1 &= ~OC1_HIT;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
        Effect_SE_Info_new(play, &this->actor.world.pos, 30, NA_SE_EN_OCTAROCK_ROCK);
        this->actionfunc = mode_break;
    }

    this->actionfunc(this, play);
    Actor_position_moveF(&this->actor);

    if (!(this->actionfunc == mode_wait || this->actionfunc == mode_break)) {
        Actor_BGcheck2(play, &this->actor, 5.0f, 20.0f, 8.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

        this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;

        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
    Actor_world_to_eye(&this->actor, 4.0f);
}

void En_Yukabyun_actor_draw(Actor* thisx, PlayState* play) {
    EnYukabyun* this = (EnYukabyun*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_yukabyun.c", 366);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(yukabyun_txt[this->unk_152]));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_yukabyun.c", 373);
    gSPDisplayList(POLY_OPA_DISP++, gFloorTileEnemyDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_yukabyun.c", 378);
}
