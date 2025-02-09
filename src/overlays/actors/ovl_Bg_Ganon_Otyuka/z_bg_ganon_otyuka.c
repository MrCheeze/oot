/*
 * File: z_bg_ganon_otyka.c
 * Overlay: ovl_Bg_Ganon_Otyka
 * Description: Falling Platform (Ganondorf Fight)
 */

#include "z_bg_ganon_otyuka.h"
#include "overlays/actors/ovl_Boss_Ganon/z_boss_ganon.h"

#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "rand.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "terminal.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"
#include "z64player.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

typedef enum FlashState {
    /* 0x00 */ FLASH_NONE,
    /* 0x01 */ FLASH_GROW,
    /* 0x02 */ FLASH_SHRINK
} FlashState;

void Bg_Ganon_Otyuka_actor_ct(Actor* thisx, PlayState* play2);
void Bg_Ganon_Otyuka_actor_dt(Actor* thisx, PlayState* play2);
void Bg_Ganon_Otyuka_actor_move(Actor* thisx, PlayState* play);
void Bg_Ganon_Otyuka_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgGanonOtyuka* this, PlayState* play);
static void mode_down(BgGanonOtyuka* this, PlayState* play);
void Bg_Ganon_Otyuka_actor_move_non(Actor* thisx, PlayState* play);

ActorProfile Bg_Ganon_Otyuka_Profile = {
    /**/ ACTOR_BG_GANON_OTYUKA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GANON,
    /**/ sizeof(BgGanonOtyuka),
    /**/ Bg_Ganon_Otyuka_actor_ct,
    /**/ Bg_Ganon_Otyuka_actor_dt,
    /**/ Bg_Ganon_Otyuka_actor_move,
    /**/ Bg_Ganon_Otyuka_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static u8 yuka_bit[] = { OTYUKA_SIDE_EAST, OTYUKA_SIDE_WEST, OTYUKA_SIDE_SOUTH, OTYUKA_SIDE_NORTH };

static Vec3f yuka_check_pos[] = {
    { 120.0f, 0.0f, 0.0f },
    { -120.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 120.0f },
    { 0.0f, 0.0f, -120.0f },
};

static Color_RGBA8 prim = { 60, 60, 0, 0 };

static Color_RGBA8 env = { 50, 20, 0, 0 };

static Vec3f yuka_offset[] = {
    { 60.0f, 0.0f, 0.0f },
    { -60.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 60.0f },
    { 0.0f, 0.0f, -60.0f },
};

static f32 yuka_yang[] = { M_PI / 2, -M_PI / 2, 0.0f, M_PI };

#include "assets/overlays/ovl_Bg_Ganon_Otyuka/z_bg_ganon_otyuka.c"

void Bg_Ganon_Otyuka_actor_ct(Actor* thisx, PlayState* play2) {
    BgGanonOtyuka* this = (BgGanonOtyuka*)thisx;
    PlayState* play = play2;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(thisx, value_init);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&ganon_otyuka_BGDataInfo, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);

    if (thisx->params != 0x23) {
        thisx->draw = NULL;
        this->actionFunc = mode_wait;
    } else {
        thisx->update = Bg_Ganon_Otyuka_actor_move_non;
    }
}

void Bg_Ganon_Otyuka_actor_dt(Actor* thisx, PlayState* play2) {
    BgGanonOtyuka* this = (BgGanonOtyuka*)thisx;
    PlayState* play = play2;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);

    PRINTF_COLOR_GREEN();
    PRINTF("WHY !!!!!!!!!!!!!!!!\n");
    PRINTF_RST();
}

static void mode_wait(BgGanonOtyuka* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    Actor* prop;
    s16 i;
    f32 dx;
    f32 dy;
    f32 dz;
    Vec3f center;

    if (this->isFalling || ((play->actorCtx.unk_02 != 0) && (this->dyna.actor.xyzDistToPlayerSq < SQ(70.0f)))) {
        PRINTF("OTC O 1\n");

        for (i = 0; i < ARRAY_COUNT(yuka_check_pos); i++) {
            prop = play->actorCtx.actorLists[ACTORCAT_PROP].head;
            while (prop != NULL) {
                if ((prop == thisx) || (prop->id != ACTOR_BG_GANON_OTYUKA)) {
                    prop = prop->next;
                    continue;
                } else {
                    BgGanonOtyuka* platform = (BgGanonOtyuka*)prop;

                    dx = platform->dyna.actor.world.pos.x - this->dyna.actor.world.pos.x + yuka_check_pos[i].x;
                    dy = platform->dyna.actor.world.pos.y - this->dyna.actor.world.pos.y;
                    dz = platform->dyna.actor.world.pos.z - this->dyna.actor.world.pos.z + yuka_check_pos[i].z;

                    if ((fabsf(dx) < 10.0f) && (fabsf(dy) < 10.0f) && (fabsf(dz) < 10.0f)) {
                        platform->visibleSides |= yuka_bit[i];
                        break;
                    }
                }
                prop = prop->next;
            }
        }

        PRINTF("OTC O 2\n");

        for (i = 0; i < ARRAY_COUNT(yuka_check_pos); i++) {
            center.x = this->dyna.actor.world.pos.x + yuka_check_pos[i].x;
            center.y = this->dyna.actor.world.pos.y;
            center.z = this->dyna.actor.world.pos.z + yuka_check_pos[i].z;
            if (T_BGCheck_SimpleCheck(&play->colCtx, &center, 50.0f)) {
                this->unwalledSides |= yuka_bit[i];
            }
        }

        PRINTF("OTC O 3\n");

        this->actionFunc = mode_down;
        this->isFalling = true;
        this->dropTimer = 20;
        this->flashState = FLASH_GROW;
        this->flashTimer = 0;
        this->flashPrimColorR = 255.0f;
        this->flashPrimColorG = 255.0f;
        this->flashPrimColorB = 255.0f;
        this->flashEnvColorR = 255.0f;
        this->flashEnvColorG = 255.0f;
        this->flashEnvColorB = 0.0f;
    }
}

static void mode_down(BgGanonOtyuka* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    PRINTF("MODE DOWN\n");
    if (this->flashState == FLASH_GROW) {
        add_calc2(&this->flashPrimColorB, 170.0f, 1.0f, 8.5f);
        add_calc2(&this->flashEnvColorR, 120.0f, 1.0f, 13.5f);
        add_calc2(&this->flashYScale, 2.5f, 1.0f, 0.25f);
        if (this->flashYScale == 2.5f) {
            this->flashState = FLASH_SHRINK;
        }
    } else if (this->flashState == FLASH_SHRINK) {
        add_calc2(&this->flashPrimColorG, 0.0f, 1.0f, 25.5f);
        add_calc2(&this->flashEnvColorR, 0.0f, 1.0f, 12.0f);
        add_calc2(&this->flashEnvColorG, 0.0f, 1.0f, 25.5f);
        add_calc0(&this->flashYScale, 1.0f, 0.25f);
        if (this->flashYScale == 0.0f) {
            this->flashState = FLASH_NONE;
        }
    }
    if (this->dropTimer == 0) {
        this->flashYScale = 0.0f;
        add_calc2(&this->dyna.actor.world.pos.y, -1000.0f, 1.0f, this->dyna.actor.speed);
        add_calc2(&this->dyna.actor.speed, 100.0f, 1.0f, 2.0f);
        if (!(this->unwalledSides & OTYUKA_SIDE_EAST)) {
            this->dyna.actor.shape.rot.z -= (s16)(this->dyna.actor.speed * 30.0f);
        }
        if (!(this->unwalledSides & OTYUKA_SIDE_WEST)) {
            this->dyna.actor.shape.rot.z += (s16)(this->dyna.actor.speed * 30.0f);
        }
        if (!(this->unwalledSides & OTYUKA_SIDE_SOUTH)) {
            this->dyna.actor.shape.rot.x += (s16)(this->dyna.actor.speed * 30.0f);
        }
        if (!(this->unwalledSides & OTYUKA_SIDE_NORTH)) {
            this->dyna.actor.shape.rot.x -= (s16)(this->dyna.actor.speed * 30.0f);
        }
        if (this->dyna.actor.world.pos.y < -750.0f) {
            if (player->actor.world.pos.y < -400.0f) {
                accel.x = accel.z = 0.0f;
                accel.y = 0.1f;
                velocity.x = velocity.y = velocity.z = 0.0f;

                for (i = 0; i < 30; i++) {
                    pos.x = rnd_fx(150.0f) + this->dyna.actor.world.pos.x;
                    pos.y = rnd_f(60.0f) + -750.0f;
                    pos.z = rnd_fx(150.0f) + this->dyna.actor.world.pos.z;
                    Effect_SS_Dust_sc_cl_co_ct(play, &pos, &velocity, &accel, &prim, &env,
                                  (s16)rnd_f(100.0f) + 250, 5, (s16)rnd_f(5.0f) + 15);
                }

                set_jisin(play, 10, 15);
                Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 40, NA_SE_EV_BOX_BREAK);
            }
            Actor_delete(&this->dyna.actor);
        }
    } else {
        if (this->dropTimer == 1) {
            Nai_FxFlagEntry(NA_SE_EV_STONEDOOR_STOP, &this->dyna.actor.projectedPos, 4,
                                 &_dummy_one, &_dummy_one, &_dummy_zero_s8);
        } else {
            Nai_FxFlagEntry(NA_SE_EV_BLOCKSINK - SFX_FLAG, &this->dyna.actor.projectedPos, 4,
                                 &_dummy_one, &_dummy_one, &_dummy_zero_s8);
        }
        add_calc2(&this->dyna.actor.world.pos.y, -1000.0f, 1.0f, this->dyna.actor.speed);
        add_calc2(&this->dyna.actor.speed, 100.0f, 1.0f, 0.1f);
    }
    PRINTF("MODE DOWN END\n");
}

void Bg_Ganon_Otyuka_actor_move_non(Actor* thisx, PlayState* play) {
}

void Bg_Ganon_Otyuka_actor_move(Actor* thisx, PlayState* play) {
    BgGanonOtyuka* this = (BgGanonOtyuka*)thisx;

    this->actionFunc(this, play);
    this->flashTimer++;
    if (this->dropTimer != 0) {
        this->dropTimer--;
    }
}

void Bg_Ganon_Otyuka_actor_draw(Actor* thisx, PlayState* play) {
    BgGanonOtyuka* this = (BgGanonOtyuka*)thisx;
    s16 i;
    Gfx* phi_s2;
    Gfx* phi_s1;
    Camera* mainCam = Gama_play_get_camera(play, CAM_ID_MAIN);
    Actor* actor;
    BgGanonOtyuka* platform;
    BossGanon* ganondorf;
    f32 spBC = -30.0f;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_ganon_otyuka.c", 702);

    actor = play->actorCtx.actorLists[ACTORCAT_BOSS].head;
    while (actor != NULL) {
        if (actor->id == ACTOR_BOSS_GANON) {
            ganondorf = (BossGanon*)actor;

            if (ganondorf->actor.params == 0) {
                if (ganondorf->unk_198 != 0) {
                    spBC = -2000.0f;
                }

                break;
            }
        }

        actor = actor->next;
    }

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, ganon_tyuka_MODE);

    actor = play->actorCtx.actorLists[ACTORCAT_PROP].head;
    while (actor != NULL) {
        if (actor->id == ACTOR_BG_GANON_OTYUKA) {
            platform = (BgGanonOtyuka*)actor;

            if (platform->dyna.actor.projectedPos.z > spBC) {
                if (mainCam->eye.y > platform->dyna.actor.world.pos.y) {
                    phi_s2 = ganon_tyuka_ue_model;
                } else {
                    phi_s2 = ganon_tyuka_sita_model;
                }
                Matrix_translate(platform->dyna.actor.world.pos.x, platform->dyna.actor.world.pos.y,
                                 platform->dyna.actor.world.pos.z, MTXMODE_NEW);
                phi_s1 = NULL;
                if (platform->isFalling) {
                    Matrix_rotateX(BINANG_TO_RAD_ALT(platform->dyna.actor.shape.rot.x), MTXMODE_APPLY);
                    Matrix_rotateZ(BINANG_TO_RAD_ALT(platform->dyna.actor.shape.rot.z), MTXMODE_APPLY);
                    if (mainCam->eye.y > platform->dyna.actor.world.pos.y) {
                        phi_s1 = ganon_tyuka_sita_model;
                    } else {
                        phi_s1 = ganon_tyuka_ue_model;
                    }
                }
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_ganon_otyuka.c", 766);
                gSPDisplayList(POLY_OPA_DISP++, phi_s2);

                if (phi_s1 != NULL) {
                    gSPDisplayList(POLY_OPA_DISP++, phi_s1);
                }

                for (i = 0; i < ARRAY_COUNT(yuka_bit); i++) {
                    if (platform->visibleSides & yuka_bit[i]) {
                        Matrix_push();
                        Matrix_translate(yuka_offset[i].x, 0.0f, yuka_offset[i].z, MTXMODE_APPLY);
                        Matrix_rotateY(yuka_yang[i], MTXMODE_APPLY);
                        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_ganon_otyuka.c", 785);
                        gSPDisplayList(POLY_OPA_DISP++, ganon_tyuka_yoko_model);
                        Matrix_pull();
                    }
                }
            }
        }

        actor = actor->next;
    }

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    actor = play->actorCtx.actorLists[ACTORCAT_PROP].head;
    while (actor != NULL) {
        if (actor->id == ACTOR_BG_GANON_OTYUKA) {
            platform = (BgGanonOtyuka*)actor;

            if ((platform->dyna.actor.projectedPos.z > -30.0f) && (platform->flashState != FLASH_NONE)) {
                gSPSegment(POLY_XLU_DISP++, 0x08,
                           two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, platform->flashTimer * 4, 0, 32, 64, 1,
                                            platform->flashTimer * 4, 0, 32, 64));
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, platform->flashPrimColorR, platform->flashPrimColorG,
                                platform->flashPrimColorB, 0);
                gDPSetEnvColor(POLY_XLU_DISP++, platform->flashEnvColorR, platform->flashEnvColorG,
                               platform->flashEnvColorB, 128);
                Matrix_translate(platform->dyna.actor.world.pos.x, 0.0f, platform->dyna.actor.world.pos.z, MTXMODE_NEW);

                for (i = 0; i < ARRAY_COUNT(yuka_bit); i++) {
                    if (platform->unwalledSides & yuka_bit[i]) {
                        Matrix_push();
                        Matrix_translate(yuka_offset[i].x, 0.0f, yuka_offset[i].z, MTXMODE_APPLY);
                        Matrix_rotateY(yuka_yang[i], MTXMODE_APPLY);
                        Matrix_scale(0.3f, platform->flashYScale * 0.3f, 0.3f, MTXMODE_APPLY);
                        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_ganon_otyuka.c", 847);
                        gSPDisplayList(POLY_XLU_DISP++, efc_ganon_floor_modelT);
                        Matrix_pull();
                    }
                }
            }
        }

        actor = actor->next;
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_ganon_otyuka.c", 857);
}
