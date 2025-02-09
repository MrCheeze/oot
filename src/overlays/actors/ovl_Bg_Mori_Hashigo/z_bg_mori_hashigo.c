/*
 * File: z_bg_mori_hashigo.c
 * Overlay: ovl_Bg_Mori_Hashigo
 * Description: Falling ladder and clasp that holds it. Unused.
 */

#include "z_bg_mori_hashigo.h"
#include "assets/objects/object_mori_objects/object_mori_objects.h"

#define FLAGS 0

void Bg_Mori_Hashigo_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mori_Hashigo_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mori_Hashigo_actor_move(Actor* thisx, PlayState* play);
void Bg_Mori_Hashigo_actor_draw(Actor* thisx, PlayState* play);

static void mvSet_dmaWait(BgMoriHashigo* this);
static void mv_dmaWait(BgMoriHashigo* this, PlayState* play);
void mvSet_stop_tomegane(BgMoriHashigo* this);
void mv_stop_tomegane(BgMoriHashigo* this, PlayState* play);
void mvSet_hang_hashigo(BgMoriHashigo* this);
void mv_hang_hashigo(BgMoriHashigo* this, PlayState* play);
void mvSet_drop_hashigo(BgMoriHashigo* this);
void mv_drop_hashigo(BgMoriHashigo* this, PlayState* play);
void mvSet_stop_hashigo(BgMoriHashigo* this);

ActorProfile Bg_Mori_Hashigo_Profile = {
    /**/ ACTOR_BG_MORI_HASHIGO,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MORI_OBJECTS,
    /**/ sizeof(BgMoriHashigo),
    /**/ Bg_Mori_Hashigo_actor_ct,
    /**/ Bg_Mori_Hashigo_actor_dt,
    /**/ Bg_Mori_Hashigo_actor_move,
    /**/ NULL,
};

static ColliderJntSphElementInit ClSphElemDt_base[1] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F820, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 25 }, 100 },
    },
};

static ColliderJntSphInit ClSphDt_base = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    1,
    ClSphElemDt_base,
};

static void set_dynaPoly(BgMoriHashigo* this, PlayState* play, CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, moveFlag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        // "Warning : move BG login failed"
        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_bg_mori_hashigo.c", 164,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void set_tomegane_collision(BgMoriHashigo* this, PlayState* play) {
    s32 pad;

    ClObjJntSph_ct(play, &this->collider);
    ClObjJntSph_set5_nzm(play, &this->collider, &this->dyna.actor, &ClSphDt_base, this->colliderItems);

    this->collider.elements[0].dim.worldSphere.center.x = (s16)this->dyna.actor.world.pos.x;
    this->collider.elements[0].dim.worldSphere.center.y = (s16)this->dyna.actor.world.pos.y + 21;
    this->collider.elements[0].dim.worldSphere.center.z = (s16)this->dyna.actor.world.pos.z;
    this->collider.elements[0].dim.worldSphere.radius = 19;
}

s32 set_hashigo(BgMoriHashigo* this, PlayState* play) {
    f32 sn;
    f32 cs;
    Vec3f pos;
    Actor* ladder;

    cs = cos_s(this->dyna.actor.shape.rot.y);
    sn = sin_s(this->dyna.actor.shape.rot.y);

    pos.x = 6.0f * sn + this->dyna.actor.world.pos.x;
    pos.y = -210.0f + this->dyna.actor.world.pos.y;
    pos.z = 6.0f * cs + this->dyna.actor.world.pos.z;

    ladder =
        Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_BG_MORI_HASHIGO, pos.x, pos.y, pos.z,
                           this->dyna.actor.world.rot.x, this->dyna.actor.world.rot.y, this->dyna.actor.world.rot.z, 0);
    if (ladder != NULL) {
        return true;
    } else {
        // "Ladder failure"
        PRINTF("Error : 梯子の発生失敗(%s %d)(arg_data 0x%04x)\n", "../z_bg_mori_hashigo.c", 220,
               this->dyna.actor.params);
        return false;
    }
}

s32 tomegane_ct(BgMoriHashigo* this, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_CONTINUE),
        ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_3, ICHAIN_CONTINUE),
        ICHAIN_F32(lockOnArrowOffset, 40, ICHAIN_CONTINUE),
        ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
    };

    ValueSet_process(&this->dyna.actor, value_init);
    this->dyna.actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_world_to_eye(&this->dyna.actor, 55.0f);
    set_tomegane_collision(this, play);
    if ((this->dyna.actor.params == HASHIGO_CLASP) && !set_hashigo(this, play)) {
        return false;
    } else {
        return true;
    }
}

s32 hashigo_ct(BgMoriHashigo* this, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_CONTINUE),
        ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
    };

    set_dynaPoly(this, play, &gMoriHashigoCol, 0);
    ValueSet_process(&this->dyna.actor, value_init);
    return true;
}

void Bg_Mori_Hashigo_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashigo* this = (BgMoriHashigo*)thisx;

    if (this->dyna.actor.params == HASHIGO_CLASP) {
        if (!tomegane_ct(this, play)) {
            Actor_delete(&this->dyna.actor);
            return;
        }
    } else if (this->dyna.actor.params == HASHIGO_LADDER) {
        if (!hashigo_ct(this, play)) {
            Actor_delete(&this->dyna.actor);
            return;
        }
    }
    this->moriTexObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_TEX);
    if (this->moriTexObjectSlot < 0) {
        // "Bank danger!"
        PRINTF("Error : バンク危険！(arg_data 0x%04x)(%s %d)\n", this->dyna.actor.params, "../z_bg_mori_hashigo.c",
               312);
        Actor_delete(&this->dyna.actor);
    } else {
        mvSet_dmaWait(this);
        // "(Forest Temple Ladder and its clasp)"
        PRINTF("(森の神殿 梯子とその留め金)(arg_data 0x%04x)\n", this->dyna.actor.params);
    }
}

void Bg_Mori_Hashigo_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashigo* this = (BgMoriHashigo*)thisx;

    if (this->dyna.actor.params == HASHIGO_LADDER) {
        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    }
    if (this->dyna.actor.params == HASHIGO_CLASP) {
        ClObjJntSph_dt_nzf(play, &this->collider);
    }
}

static void mvSet_dmaWait(BgMoriHashigo* this) {
    this->actionFunc = mv_dmaWait;
}

static void mv_dmaWait(BgMoriHashigo* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->moriTexObjectSlot)) {
        if (this->dyna.actor.params == HASHIGO_CLASP) {
            mvSet_stop_tomegane(this);
        } else if (this->dyna.actor.params == HASHIGO_LADDER) {
            mvSet_hang_hashigo(this);
        }
        this->dyna.actor.draw = Bg_Mori_Hashigo_actor_draw;
    }
}

void mvSet_stop_tomegane(BgMoriHashigo* this) {
    this->actionFunc = mv_stop_tomegane;
}

void mv_stop_tomegane(BgMoriHashigo* this, PlayState* play) {
    if (this->hitTimer <= 0) {
        if (this->collider.base.acFlags & AC_HIT) {
            this->collider.base.acFlags &= ~AC_HIT;
            this->hitTimer = 10;
        } else {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void mvSet_hang_hashigo(BgMoriHashigo* this) {
    this->actionFunc = mv_hang_hashigo;
}

void mv_hang_hashigo(BgMoriHashigo* this, PlayState* play) {
    BgMoriHashigo* clasp = (BgMoriHashigo*)this->dyna.actor.parent;

    if (clasp->hitTimer > 0) {
        mvSet_drop_hashigo(this);
    }
}

void mvSet_drop_hashigo(BgMoriHashigo* this) {
    this->bounceCounter = 0;
    this->actionFunc = mv_drop_hashigo;
    this->dyna.actor.gravity = -1.0f;
    this->dyna.actor.minVelocityY = -10.0f;
    this->dyna.actor.velocity.y = 2.0f;
}

void mv_drop_hashigo(BgMoriHashigo* this, PlayState* play) {
    static f32 spdY[3] = { 4.0f, 2.7f, 1.7f };
    Actor* thisx = &this->dyna.actor;

    Actor_position_moveF(thisx);
    if ((thisx->bgCheckFlags & BGCHECKFLAG_GROUND) && (thisx->velocity.y < 0.0f)) {
        if (this->bounceCounter >= ARRAY_COUNT(spdY)) {
            mvSet_stop_hashigo(this);
        } else {
            Actor_BGcheck2(play, thisx, 0.0f, 0.0f, 0.0f,
                                    UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
            thisx->velocity.y = spdY[this->bounceCounter];
            this->bounceCounter++;
        }
    } else {
        Actor_BGcheck2(play, thisx, 0.0f, 0.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    }
}

void mvSet_stop_hashigo(BgMoriHashigo* this) {
    this->dyna.actor.gravity = 0.0f;
    this->dyna.actor.velocity.y = 0.0f;
    this->actionFunc = NULL;
    this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
}

void Bg_Mori_Hashigo_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashigo* this = (BgMoriHashigo*)thisx;

    if (this->hitTimer > 0) {
        this->hitTimer--;
    }
    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void Bg_Mori_Hashigo_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriHashigo* this = (BgMoriHashigo*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_mori_hashigo.c", 516);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, play->objectCtx.slots[this->moriTexObjectSlot].segment);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mori_hashigo.c", 521);

    switch (this->dyna.actor.params) {
        case HASHIGO_CLASP:
            gSPDisplayList(POLY_OPA_DISP++, gMoriHashigoClaspDL);
            break;
        case HASHIGO_LADDER:
            gSPDisplayList(POLY_OPA_DISP++, gMoriHashigoLadderDL);
            break;
    }
    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_mori_hashigo.c", 531);
}
