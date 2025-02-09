/*
 * File: z_bg_mori_idomizu.c
 * Overlay: ovl_Bg_Mori_Idomizu
 * Description: Square of water in Forest Temple well
 */

#include "z_bg_mori_idomizu.h"
#include "assets/objects/object_mori_objects/object_mori_objects.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Mori_Idomizu_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mori_Idomizu_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mori_Idomizu_actor_move(Actor* thisx, PlayState* play);
void Bg_Mori_Idomizu_actor_draw(Actor* thisx, PlayState* play);

static void mvSet_dmaWait(BgMoriIdomizu* this);
static void mv_dmaWait(BgMoriIdomizu* this, PlayState* play);
static void mvSet_updown(BgMoriIdomizu* this);
static void mv_updown(BgMoriIdomizu* this, PlayState* play);

static s16 Idomizu_flag = false;

ActorProfile Bg_Mori_Idomizu_Profile = {
    /**/ ACTOR_BG_MORI_IDOMIZU,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_MORI_OBJECTS,
    /**/ sizeof(BgMoriIdomizu),
    /**/ Bg_Mori_Idomizu_actor_ct,
    /**/ Bg_Mori_Idomizu_actor_dt,
    /**/ Bg_Mori_Idomizu_actor_move,
    /**/ NULL,
};

static void set_moveProc(BgMoriIdomizu* this, BgMoriIdomizuActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static void set_waterPosY(PlayState* play, s16 waterLevel) {
    WaterBox* waterBox = play->colCtx.colHeader->waterBoxes;

    waterBox[2].ySurface = waterLevel;
    waterBox[3].ySurface = waterLevel;
    waterBox[4].ySurface = waterLevel;
}

void Bg_Mori_Idomizu_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriIdomizu* this = (BgMoriIdomizu*)thisx;

    if (Idomizu_flag) {
        Actor_delete(&this->actor);
        return;
    }
    this->actor.scale.x = 1.1f;
    this->actor.scale.y = 1.0f;
    this->actor.scale.z = 1.0f;
    this->actor.world.pos.x = 119.0f;
    this->actor.world.pos.z = -1820.0f;
    this->prevSwitchFlagSet = Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 0, 6));
    if (this->prevSwitchFlagSet != 0) {
        this->actor.world.pos.y = -282.0f;
        set_waterPosY(play, -282);
    } else {
        this->actor.world.pos.y = 184.0f;
        set_waterPosY(play, 184);
    }
    this->moriTexObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_TEX);
    if (this->moriTexObjectSlot < 0) {
        Actor_delete(&this->actor);
        // "Bank danger!"
        PRINTF("Error : バンク危険！(arg_data 0x%04x)(%s %d)\n", this->actor.params, "../z_bg_mori_idomizu.c", 202);
        return;
    }
    mvSet_dmaWait(this);
    Idomizu_flag = true;
    this->isLoaded = true;
    this->actor.room = -1;
    // "Forest Temple well water"
    PRINTF("(森の神殿 井戸水)(arg_data 0x%04x)\n", this->actor.params);
}

void Bg_Mori_Idomizu_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriIdomizu* this = (BgMoriIdomizu*)thisx;

    if (this->isLoaded) {
        Idomizu_flag = false;
    }
}

static void mvSet_dmaWait(BgMoriIdomizu* this) {
    set_moveProc(this, mv_dmaWait);
}

static void mv_dmaWait(BgMoriIdomizu* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->moriTexObjectSlot)) {
        mvSet_updown(this);
        this->actor.draw = Bg_Mori_Idomizu_actor_draw;
    }
}

static void mvSet_updown(BgMoriIdomizu* this) {
    set_moveProc(this, mv_updown);
}

static void mv_updown(BgMoriIdomizu* this, PlayState* play) {
    s8 roomNum;
    Actor* thisx = &this->actor;
    s32 switchFlagSet;

    roomNum = play->roomCtx.curRoom.num;
    switchFlagSet = Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 0, 6));
    if (switchFlagSet) {
        this->targetWaterLevel = -282.0f;
    } else {
        this->targetWaterLevel = 184.0f;
    }
    if (switchFlagSet && !this->prevSwitchFlagSet) {
        makeOnepointDemo(play, 3240, 70, thisx, CAM_ID_MAIN);
        this->drainTimer = 90;
    } else if (!switchFlagSet && this->prevSwitchFlagSet) {
        makeOnepointDemo(play, 3240, 70, thisx, CAM_ID_MAIN);
        this->drainTimer = 90;
        thisx->world.pos.y = 0.0f;
    }
    this->drainTimer--;
    if ((roomNum == 7) || (roomNum == 8) || (roomNum == 9)) {
        if (this->drainTimer < 70) {
            chase_f(&thisx->world.pos.y, this->targetWaterLevel, 3.5f);
            set_waterPosY(play, thisx->world.pos.y);
            if (this->drainTimer > 0) {
                if (switchFlagSet) {
                    Na_StartFixSe_F(NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
                } else {
                    Na_StartFixSe_F(NA_SE_EV_WATER_LEVEL_DOWN - SFX_FLAG);
                }
            }
        }
    } else {
        thisx->world.pos.y = this->targetWaterLevel;
        set_waterPosY(play, thisx->world.pos.y);
        Actor_delete(thisx);
        return;
    }
    this->prevSwitchFlagSet = switchFlagSet;
}

void Bg_Mori_Idomizu_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriIdomizu* this = (BgMoriIdomizu*)thisx;

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void Bg_Mori_Idomizu_actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgMoriIdomizu* this = (BgMoriIdomizu*)thisx;
    u32 gameplayFrames = play->gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_mori_idomizu.c", 356);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_mori_idomizu.c", 360);

    gSPSegment(POLY_XLU_DISP++, 0x08, play->objectCtx.slots[this->moriTexObjectSlot].segment);

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, 128);

    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0x7F - (gameplayFrames & 0x7F),
                                gameplayFrames % 0x80, 0x20, 0x20, 1, gameplayFrames & 0x7F, gameplayFrames % 0x80,
                                0x20, 0x20));

    gSPDisplayList(POLY_XLU_DISP++, gMoriIdomizuWaterDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_mori_idomizu.c", 382);
}
