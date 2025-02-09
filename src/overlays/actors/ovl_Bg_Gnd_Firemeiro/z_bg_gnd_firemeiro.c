/*
 * File: z_bg_gnd_firemeiro.c
 * Overlay: ovl_Bg_Gnd_Firemeiro
 * Description: Sinking lava platform (Ganon's Castle)
 */

#include "z_bg_gnd_firemeiro.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64play.h"
#include "z64player.h"

#include "assets/objects/object_demo_kekkai/object_demo_kekkai.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Gnd_Firemeiro_Actor_ct(Actor* thisx, PlayState* play);
void Bg_Gnd_Firemeiro_Actor_dt(Actor* thisx, PlayState* play2);
void Bg_Gnd_Firemeiro_Actor_move(Actor* thisx, PlayState* play);
void Bg_Gnd_Firemeiro_Actor_draw(Actor* thisx, PlayState* play);

static void move_down(BgGndFiremeiro* this, PlayState* play);
void move_queike(BgGndFiremeiro* this, PlayState* play);
static void move_wait(BgGndFiremeiro* this, PlayState* play);

ActorProfile Bg_Gnd_Firemeiro_Profile = {
    /**/ ACTOR_BG_GND_FIREMEIRO,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_DEMO_KEKKAI,
    /**/ sizeof(BgGndFiremeiro),
    /**/ Bg_Gnd_Firemeiro_Actor_ct,
    /**/ Bg_Gnd_Firemeiro_Actor_dt,
    /**/ Bg_Gnd_Firemeiro_Actor_move,
    /**/ Bg_Gnd_Firemeiro_Actor_draw,
};

void Bg_Gnd_Firemeiro_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgGndFiremeiro* this = (BgGndFiremeiro*)thisx;
    CollisionHeader* colHeader = NULL;

    Shape_Info_init(&this->dyna.actor.shape, 0.0f, NULL, 0.0f);
    Actor_set_scale(&this->dyna.actor, 0.1f);
    this->initPos = this->dyna.actor.world.pos;

    if (this->dyna.actor.params == 0) {
        MoveBG_ct(&this->dyna, 0);
        DynaPolyUty_bgdi_SG2KSG(&gFireTrialPlatformCol, &colHeader);
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
        this->actionFunc = move_wait;
    }
}

void Bg_Gnd_Firemeiro_Actor_dt(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgGndFiremeiro* this = (BgGndFiremeiro*)thisx;

    if (this->dyna.actor.params != 0) {
        return;
    }

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void move_down(BgGndFiremeiro* this, PlayState* play) {
    f32 sunkHeight = this->initPos.y - 150.0f;

    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        this->timer = 10;
    }

    if (sunkHeight < this->dyna.actor.world.pos.y) {
        this->dyna.actor.world.pos.y -= 0.5f;

        if (this->dyna.actor.world.pos.y < sunkHeight) {
            this->dyna.actor.world.pos.y = sunkHeight;
        }

        Actor_fix_level_SE_set(&this->dyna.actor, NA_SE_EV_ROLL_STAND_2 - SFX_FLAG);
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        this->actionFunc = move_wait;
    }
}

void move_queike(BgGndFiremeiro* this, PlayState* play) {
    s32 pad;
    f32 randSign;

    if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
        if (this->timer > 0) {
            this->timer--;

            randSign = ((this->timer & 1) ? 2.0f : -2.0f);

            this->dyna.actor.world.pos = this->initPos;
            this->dyna.actor.world.pos.x += randSign * sin_s(this->timer * 0x2FFF);
            this->dyna.actor.world.pos.z += randSign * cos_s(this->timer * 0x2FFF);
            this->dyna.actor.world.pos.y += cos_s(this->timer * 0x7FFF);

            if (!(this->timer % 4)) {
                Actor_SE_set(&this->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
            }
        } else {
            this->timer = 10;
            this->dyna.actor.world.pos = this->initPos;
            this->actionFunc = move_down;
        }
    } else {
        this->dyna.actor.world.pos = this->initPos;
        this->actionFunc = move_wait;
    }
}

static void move_wait(BgGndFiremeiro* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Actor* thisx = &this->dyna.actor;

    if ((player->currentBoots != PLAYER_BOOTS_HOVER) && MoveBG_checkRidePlayerStatus(&this->dyna)) {
        if (thisx->world.pos.y < this->initPos.y) {
            this->actionFunc = move_down;
            this->timer = 20;
        } else {
            this->actionFunc = move_queike;
            this->timer = 20;
        }
    } else {
        if (thisx->world.pos.y < this->initPos.y) {
            thisx->world.pos.y += 2.0f;
            if (this->initPos.y < thisx->world.pos.y) {
                thisx->world.pos.y = this->initPos.y;
            }
        }
    }
}

void Bg_Gnd_Firemeiro_Actor_move(Actor* thisx, PlayState* play) {
    BgGndFiremeiro* this = (BgGndFiremeiro*)thisx;

    this->actionFunc(this, play);
}

void Bg_Gnd_Firemeiro_Actor_draw(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx, "../z_bg_gnd_firemeiro.c", 280);
    _polygon_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_gnd_firemeiro.c", 282);
    gSPDisplayList(POLY_OPA_DISP++, gFireTrialPlatformDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_gnd_firemeiro.c", 285);
}
