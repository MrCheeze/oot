/*
 * File: z_bg_zg.c
 * Overlay: ovl_Bg_Zg
 * Description: Metal bars (Ganon's Castle)
 */

#include "z_bg_zg.h"
#include "assets/objects/object_zg/object_zg.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Zg_actor_ct(Actor* thisx, PlayState* play);
void Bg_Zg_actor_dt(Actor* thisx, PlayState* play);
void Bg_Zg_main(Actor* thisx, PlayState* play);
void Bg_Zg_draw(Actor* thisx, PlayState* play);

void Bg_Zg_Set_OpenSound(BgZg* this);
s32 Bg_Zg_Check_SaveBit(BgZg* this, PlayState* play);
s32 Bg_Zg_GetLower_arg_data(BgZg* this);
void Bg_Zg_main_Stand(BgZg* this, PlayState* play);
void Bg_Zg_main_Open(BgZg* this, PlayState* play);
void Bg_Zg_draw_normal(BgZg* this, PlayState* play);

void Bg_Zg_actor_dt(Actor* thisx, PlayState* play) {
    BgZg* this = (BgZg*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Bg_Zg_Set_OpenSound(BgZg* this) {
    Nai_FxFlagEntry(NA_SE_EV_METALDOOR_OPEN, &this->dyna.actor.projectedPos, 4, &_dummy_one,
                         &_dummy_one, &_dummy_zero_s8);
}

s32 Bg_Zg_Check_SaveBit(BgZg* this, PlayState* play) {
    s32 flag = PARAMS_GET_U(this->dyna.actor.params, 8, 8);

    return Actor_Environment_sw_Check(play, flag);
}

s32 Bg_Zg_GetLower_arg_data(BgZg* this) {
    s32 flag = PARAMS_GET_U(this->dyna.actor.params, 0, 8);

    return flag;
}

void Bg_Zg_main_Stand(BgZg* this, PlayState* play) {
#if PLATFORM_N64
    // Anti-piracy check, bars will not open if the check fails.
    // The address 0x000002E8 is near the start of RDRAM, and is written when IPL3 copies itself to
    // RDRAM after RDRAM has been initialized. Specifically, this is an instruction from some
    // embedded RSP code at offset 0x7F8 into IPL3 (0xC86E2000 disassembles to `lqv $v14[0], ($3)`).
    if (IO_READ(0x000002E8) != 0xC86E2000) {
        return;
    }
#endif

    if (Bg_Zg_Check_SaveBit(this, play) != 0) {
        this->action = 1;
        Bg_Zg_Set_OpenSound(this);
    }
}

void Bg_Zg_main_Open(BgZg* this, PlayState* play) {
    this->dyna.actor.world.pos.y += (kREG(16) + 20.0f) * 1.2f;
    if ((((kREG(17) + 200.0f) * 1.2f) + this->dyna.actor.home.pos.y) <= this->dyna.actor.world.pos.y) {
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Zg_main(Actor* thisx, PlayState* play) {
    static BgZgActionFunc proc[] = {
        Bg_Zg_main_Stand,
        Bg_Zg_main_Open,
    };

    BgZg* this = (BgZg*)thisx;
    s32 action = this->action;

    if (((action < 0) || (1 < action)) || (proc[action] == NULL)) {
        // "Main Mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        proc[action](this, play);
    }
}

void Bg_Zg_actor_ct(Actor* thisx, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
    };

    s32 pad[2];
    BgZg* this = (BgZg*)thisx;
    CollisionHeader* colHeader;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    colHeader = NULL;
    DynaPolyUty_bgdi_SG2KSG(&gTowerCollapseBarsCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if ((Bg_Zg_GetLower_arg_data(this) == 8) || (Bg_Zg_GetLower_arg_data(this) == 9)) {
        this->dyna.actor.scale.x *= 1.3f;
        this->dyna.actor.scale.z *= 1.3f;
        this->dyna.actor.scale.y *= 1.2f;
    }

    this->action = 0;
    this->drawConfig = 0;
    if (Bg_Zg_Check_SaveBit(this, play)) {
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Zg_draw_normal(BgZg* this, PlayState* play) {
    GraphicsContext* localGfxCtx = play->state.gfxCtx;

    OPEN_DISPS(localGfxCtx, "../z_bg_zg.c", 311);

    _texture_z_light_fog_prim(localGfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, localGfxCtx, "../z_bg_zg.c", 315);
    gSPDisplayList(POLY_OPA_DISP++, gTowerCollapseBarsDL);

    CLOSE_DISPS(localGfxCtx, "../z_bg_zg.c", 320);
}

void Bg_Zg_draw(Actor* thisx, PlayState* play) {
    static BgZgDrawFunc proc[] = {
        Bg_Zg_draw_normal,
    };

    BgZg* this = (BgZg*)thisx;
    s32 drawConfig = this->drawConfig;

    if (((drawConfig < 0) || (drawConfig > 0)) || proc[drawConfig] == NULL) {
        // "Drawing mode is wrong !!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        proc[drawConfig](this, play);
    }
}

ActorProfile Bg_Zg_Profile = {
    /**/ ACTOR_BG_ZG,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ZG,
    /**/ sizeof(BgZg),
    /**/ Bg_Zg_actor_ct,
    /**/ Bg_Zg_actor_dt,
    /**/ Bg_Zg_main,
    /**/ Bg_Zg_draw,
};
