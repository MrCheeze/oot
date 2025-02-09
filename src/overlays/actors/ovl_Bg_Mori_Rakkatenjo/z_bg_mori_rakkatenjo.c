/*
 * File: z_bg_mori_rakkatenjo.c
 * Overlay: ovl_Bg_Mori_Rakkatenjo
 * Description: Falling ceiling in Forest Temple
 */

#include "z_bg_mori_rakkatenjo.h"
#include "assets/objects/object_mori_objects/object_mori_objects.h"
#include "quake.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Mori_Rakkatenjo_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mori_Rakkatenjo_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mori_Rakkatenjo_actor_move(Actor* thisx, PlayState* play2);
void Bg_Mori_Rakkatenjo_actor_draw(Actor* thisx, PlayState* play);

static void mvSet_dmaWait(BgMoriRakkatenjo* this);
static void mv_dmaWait(BgMoriRakkatenjo* this, PlayState* play);
void mvSet_waitPlayer(BgMoriRakkatenjo* this);
void mv_waitPlayer(BgMoriRakkatenjo* this, PlayState* play);
static void mvSet_drop(BgMoriRakkatenjo* this);
static void mv_drop(BgMoriRakkatenjo* this, PlayState* play);
void mvSet_stopLower(BgMoriRakkatenjo* this);
void mv_stopLower(BgMoriRakkatenjo* this, PlayState* play);
void mvSet_rize(BgMoriRakkatenjo* this);
void mv_rize(BgMoriRakkatenjo* this, PlayState* play);

static s16 camera_changed = CAM_SET_NONE;

ActorProfile Bg_Mori_Rakkatenjo_Profile = {
    /**/ ACTOR_BG_MORI_RAKKATENJO,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MORI_OBJECTS,
    /**/ sizeof(BgMoriRakkatenjo),
    /**/ Bg_Mori_Rakkatenjo_actor_ct,
    /**/ Bg_Mori_Rakkatenjo_actor_dt,
    /**/ Bg_Mori_Rakkatenjo_actor_move,
    /**/ NULL,
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_F32(minVelocityY, -11, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void Bg_Mori_Rakkatenjo_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriRakkatenjo* this = (BgMoriRakkatenjo*)thisx;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);

#if DEBUG_FEATURES
    // "Forest Temple obj. Falling Ceiling"
    PRINTF("森の神殿 obj. 落下天井 (home posY %f)\n", this->dyna.actor.home.pos.y);
    if ((fabsf(1991.0f - this->dyna.actor.home.pos.x) > 0.001f) ||
        (fabsf(683.0f - this->dyna.actor.home.pos.y) > 0.001f) ||
        (fabsf(-2520.0f - this->dyna.actor.home.pos.z) > 0.001f)) {
        // "The set position has been changed. Let's fix the program."
        PRINTF("Warning : セット位置が変更されています。プログラムを修正しましょう。\n");
    }
    if (this->dyna.actor.home.rot.y != 0x8000) {
        // "The set Angle has changed. Let's fix the program."
        PRINTF("Warning : セット Angle が変更されています。プログラムを修正しましょう。\n");
    }
#endif

    this->moriTexObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_TEX);
    if (this->moriTexObjectSlot < 0) {
        // "Forest Temple obj Falling Ceiling Bank Danger!"
        PRINTF("Error : 森の神殿 obj 落下天井 バンク危険！(%s %d)\n", "../z_bg_mori_rakkatenjo.c", 205);
        Actor_delete(&this->dyna.actor);
        return;
    }
    ValueSet_process(&this->dyna.actor, value_init);
    DynaPolyUty_bgdi_SG2KSG(&gMoriRakkatenjoCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    mvSet_dmaWait(this);
    camera_changed = CAM_SET_NONE;
}

void Bg_Mori_Rakkatenjo_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriRakkatenjo* this = (BgMoriRakkatenjo*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

int checkPlayerUnderRakkatenjo(BgMoriRakkatenjo* this, PlayState* play) {
    Vec3f* pos = &GET_PLAYER(play)->actor.world.pos;

    return (-3300.0f < pos->z) && (pos->z < -1840.0f) && (1791.0f < pos->x) && (pos->x < 2191.0f);
}

int checkPlayerUnderRakkatenjo2(BgMoriRakkatenjo* this, PlayState* play) {
    Vec3f* pos = &GET_PLAYER(play)->actor.world.pos;

    return (-3360.0f < pos->z) && (pos->z < -1840.0f) && (1791.0f < pos->x) && (pos->x < 2191.0f);
}

static void mvSet_dmaWait(BgMoriRakkatenjo* this) {
    this->actionFunc = mv_dmaWait;
}

static void mv_dmaWait(BgMoriRakkatenjo* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->moriTexObjectSlot)) {
        mvSet_waitPlayer(this);
        this->dyna.actor.draw = Bg_Mori_Rakkatenjo_actor_draw;
    }
}

void mvSet_waitPlayer(BgMoriRakkatenjo* this) {
    this->timer = (this->fallCount > 0) ? 100 : 21;
    this->dyna.actor.world.pos.y = 683.0f;
    this->actionFunc = mv_waitPlayer;
}

void mv_waitPlayer(BgMoriRakkatenjo* this, PlayState* play) {
    if (this->fallCount == 0) {
        if (checkPlayerUnderRakkatenjo2(this, play) || (this->timer < 20)) {
            if (this->timer <= 0) {
                mvSet_drop(this);
            }
        } else {
            this->timer = 21;
        }
    } else {
        if (checkPlayerUnderRakkatenjo(this, play) || (this->timer < 20)) {
            if (this->timer <= 0) {
                mvSet_drop(this);
            }
        } else {
            this->timer = 100;
        }
    }
    if (this->timer < 20) {
        Na_StartFixSe_F(NA_SE_EV_BLOCKSINK - SFX_FLAG);
    }
}

static void mvSet_drop(BgMoriRakkatenjo* this) {
    this->actionFunc = mv_drop;
    this->bounceCount = 0;
    this->dyna.actor.velocity.y = 0.0f;
}

static void mv_drop(BgMoriRakkatenjo* this, PlayState* play) {
    static f32 boundSpd[] = { 4.0f, 1.5f, 0.4f, 0.1f };
    s32 pad;
    Actor* thisx = &this->dyna.actor;
    s32 quakeIndex;

    Actor_position_moveF(thisx);
    if ((thisx->velocity.y < 0.0f) && (thisx->world.pos.y <= 403.0f)) {
        if (this->bounceCount >= ARRAY_COUNT(boundSpd)) {
            mvSet_stopLower(this);
        } else {
            if (this->bounceCount == 0) {
                this->fallCount++;
                Na_StartFixSe_F(NA_SE_EV_STONE_BOUND);
                z_vibctl2_vib_setQ(SQ(thisx->yDistToPlayer), 255, 20, 150);
            }
            thisx->world.pos.y =
                403.0f - (thisx->world.pos.y - 403.0f) * boundSpd[this->bounceCount] / fabsf(thisx->velocity.y);
            thisx->velocity.y = boundSpd[this->bounceCount];
            this->bounceCount++;

            quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
            setSpeedQuake(quakeIndex, 50000);
            setScaleQuake(quakeIndex, 5, 0, 0, 0);
            setTimerQuake(quakeIndex, 5);
        }
    }
}

void mvSet_stopLower(BgMoriRakkatenjo* this) {
    this->actionFunc = mv_stopLower;
    this->dyna.actor.world.pos.y = 403.0f;
    this->timer = 20;
}

void mv_stopLower(BgMoriRakkatenjo* this, PlayState* play) {
    if (this->timer <= 0) {
        mvSet_rize(this);
    }
}

void mvSet_rize(BgMoriRakkatenjo* this) {
    this->actionFunc = mv_rize;
    this->dyna.actor.velocity.y = -0.1f;
}

void mv_rize(BgMoriRakkatenjo* this, PlayState* play) {
    add_calc(&this->dyna.actor.velocity.y, 5.0f, 0.06f, 0.1f, 0.0f);
    this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;
    if (this->dyna.actor.world.pos.y >= 683.0f) {
        mvSet_waitPlayer(this);
    }
}

void Bg_Mori_Rakkatenjo_actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = (PlayState*)play2;
    BgMoriRakkatenjo* this = (BgMoriRakkatenjo*)thisx;

    if (this->timer > 0) {
        this->timer--;
    }
    this->actionFunc(this, play);
    if (checkPlayerUnderRakkatenjo(this, play)) {
        if (camera_changed == CAM_SET_NONE) {
            PRINTF("camera changed (mori rakka tenjyo) ... \n");
            camera_changed = play->cameraPtrs[CAM_ID_MAIN]->setting;
            setCameraData(play->cameraPtrs[CAM_ID_MAIN], 1, &this->dyna.actor, NULL, 0, 0, 0);
            changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_FOREST_BIRDS_EYE);
        }
    } else if (camera_changed != CAM_SET_NONE) {
        PRINTF("camera changed (previous) ... \n");
        changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DUNGEON1);
        camera_changed = CAM_SET_NONE;
    }
}

void Bg_Mori_Rakkatenjo_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriRakkatenjo* this = (BgMoriRakkatenjo*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_mori_rakkatenjo.c", 497);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, play->objectCtx.slots[this->moriTexObjectSlot].segment);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mori_rakkatenjo.c", 502);

    gSPDisplayList(POLY_OPA_DISP++, gMoriRakkatenjoDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_mori_rakkatenjo.c", 506);
}
