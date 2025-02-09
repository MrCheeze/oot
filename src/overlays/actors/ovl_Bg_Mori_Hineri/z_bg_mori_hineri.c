/*
 * File: z_bg_mori_hineri.c
 * Overlay: ovl_Bg_Mori_Hineri
 * Description: Twisting hallway in Forest Temple
 */

#include "z_bg_mori_hineri.h"
#include "versions.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_box/object_box.h"
#include "assets/objects/object_mori_hineri1/object_mori_hineri1.h"
#include "assets/objects/object_mori_hineri1a/object_mori_hineri1a.h"
#include "assets/objects/object_mori_hineri2/object_mori_hineri2.h"
#include "assets/objects/object_mori_hineri2a/object_mori_hineri2a.h"
#include "assets/objects/object_mori_tex/object_mori_tex.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Mori_Hineri_actor_ct(Actor* thisx, PlayState* play);
void Bg_Mori_Hineri_actor_dt(Actor* thisx, PlayState* play);
void Bg_Mori_Hineri_actor_move(Actor* thisx, PlayState* play);
void Bg_Mori_Hineri_actor_draw(Actor* thisx, PlayState* play);

static void mode_dma_wait(BgMoriHineri* this, PlayState* play);
void mode_demo_move(BgMoriHineri* this, PlayState* play);
static void mode_rotate(BgMoriHineri* this, PlayState* play);
void mode_make_trasure(BgMoriHineri* this, PlayState* play);
static void mode_wait(BgMoriHineri* this, PlayState* play);
static void mode_demo_wait(BgMoriHineri* this, PlayState* play);

static s16 demo_camera_no = CAM_ID_NONE;

ActorProfile Bg_Mori_Hineri_Profile = {
    /**/ ACTOR_BG_MORI_HINERI,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(BgMoriHineri),
    /**/ Bg_Mori_Hineri_actor_ct,
    /**/ Bg_Mori_Hineri_actor_dt,
    /**/ Bg_Mori_Hineri_actor_move,
    /**/ NULL,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

static Gfx* display_model[] = {
    object_mori_hineri1_DL_0024E0,
    object_mori_hineri1a_DL_001980,
    object_mori_hineri2_DL_0020F0,
    object_mori_hineri2a_DL_002B70,
};

void Bg_Mori_Hineri_actor_ct(Actor* thisx, PlayState* play) {
    BgMoriHineri* this = (BgMoriHineri*)thisx;
    s8 objectSlot;
    u32 switchFlagParam;
    s32 t6;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);

    switchFlagParam = PARAMS_GET_U(this->dyna.actor.params, 0, 6);
    t6 = PARAMS_GET_NOSHIFT(this->dyna.actor.params, 14, 1);

    if (t6 != 0) {
        this->switchFlag = switchFlagParam;
    } else {
        this->switchFlag = PARAMS_GET_U(this->dyna.actor.params, 8, 6);
        this->switchFlag = (Actor_Environment_sw_Check(play, this->switchFlag)) ? 1 : 0;
    }
    this->dyna.actor.params = TWISTED_HALLWAY_GET_PARAM_15(&this->dyna.actor);
    if (Actor_Environment_sw_Check(play, switchFlagParam)) {
        if (this->dyna.actor.params == 0) {
            this->dyna.actor.params = 1;
        } else if (this->dyna.actor.params == 2) {
            this->dyna.actor.params = 3;
        }
    }
    this->boxObjectSlot = -1;
    if (this->dyna.actor.params == 0) {
        this->moriHineriObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_HINERI1);
        if (t6 == 0) {
            this->boxObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_BOX);
        }
    } else {
        if (this->dyna.actor.params == 1) {
            objectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_HINERI1A);
        } else {
            objectSlot = (this->dyna.actor.params == 2) ? Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_HINERI2)
                                                        : Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_HINERI2A);
        }
        this->moriHineriObjectSlot = objectSlot;
    }
    this->moriTexObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_TEX);
    if (t6 != 0) {
        this->dyna.actor.params += 4;
    }
    if ((this->moriHineriObjectSlot < 0) || (this->moriTexObjectSlot < 0)) {
        Actor_delete(&this->dyna.actor);
    } else {
        this->actionFunc = mode_dma_wait;
    }
}

void Bg_Mori_Hineri_actor_dt(Actor* thisx, PlayState* play) {
    BgMoriHineri* this = (BgMoriHineri*)thisx;
    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void mode_dma_wait(BgMoriHineri* this, PlayState* play) {
    CollisionHeader* colHeader;

    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->moriHineriObjectSlot) &&
        Object_Exchange_bank_dma_check(&play->objectCtx, this->moriTexObjectSlot) &&
        ((this->boxObjectSlot < 0) || Object_Exchange_bank_dma_check(&play->objectCtx, this->boxObjectSlot))) {
        this->dyna.actor.objectSlot = this->moriHineriObjectSlot;
        if (this->dyna.actor.params >= 4) {
            this->dyna.actor.params -= 4;
            if (this->dyna.actor.params == 0) {
                this->moriHineriObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_HINERI1A);
            } else if (this->dyna.actor.params == 1) {
                this->moriHineriObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_HINERI1);
            } else {
                this->moriHineriObjectSlot = (this->dyna.actor.params == 2)
                                                 ? Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_HINERI2A)
                                                 : Object_Exchange_bank_check(&play->objectCtx, OBJECT_MORI_HINERI2);
            }
            if (this->moriHineriObjectSlot < 0) {
                Actor_delete(&this->dyna.actor);
            } else {
                this->actionFunc = mode_demo_wait;
            }
        } else {
            Actor_set_segment(play, &this->dyna.actor);
            colHeader = NULL;
            this->dyna.actor.draw = Bg_Mori_Hineri_actor_draw;
            if (this->dyna.actor.params == 0) {
                this->actionFunc = mode_rotate;
                DynaPolyUty_bgdi_SG2KSG(&object_mori_hineri1_Col_0054B8, &colHeader);
            } else if (this->dyna.actor.params == 1) {
                this->actionFunc = mode_make_trasure;
                DynaPolyUty_bgdi_SG2KSG(&object_mori_hineri1a_Col_003490, &colHeader);
            } else if (this->dyna.actor.params == 2) {
                this->actionFunc = mode_wait;
                DynaPolyUty_bgdi_SG2KSG(&object_mori_hineri2_Col_0043D0, &colHeader);
            } else {
                this->actionFunc = mode_rotate;
                DynaPolyUty_bgdi_SG2KSG(&object_mori_hineri2a_Col_006078, &colHeader);
            }
            this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
        }
    }
}

static void mode_wait(BgMoriHineri* this, PlayState* play) {
}

void mode_make_trasure(BgMoriHineri* this, PlayState* play) {
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOX, this->dyna.actor.world.pos.x + 147.0f,
                this->dyna.actor.world.pos.y + -245.0f, this->dyna.actor.world.pos.z + -453.0f, 0, 0x4000, 0, 0x27EE);
    this->actionFunc = mode_wait;
}

static void mode_rotate(BgMoriHineri* this, PlayState* play) {
    f32 f0;
    Player* player = GET_PLAYER(play);

    f0 = 1100.0f - (player->actor.world.pos.z - this->dyna.actor.world.pos.z);
    this->dyna.actor.shape.rot.z = CLAMP(f0, 0.0f, 1000.0f) * 16.384f;
    changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DUNGEON1);
    if (this->dyna.actor.params != 0) {
        this->dyna.actor.shape.rot.z = -this->dyna.actor.shape.rot.z;
    }
}

static void mode_demo_wait(BgMoriHineri* this, PlayState* play) {
    s16 mainCamChildId;

    if ((Actor_Environment_sw_Check(play, this->switchFlag) && (this->dyna.actor.params == 0 || this->dyna.actor.params == 2)) ||
        (!Actor_Environment_sw_Check(play, this->switchFlag) && (this->dyna.actor.params == 1 || this->dyna.actor.params == 3))) {
        this->dyna.actor.draw = Bg_Mori_Hineri_actor_draw;
        this->actionFunc = mode_demo_move;

        mainCamChildId = play->cameraPtrs[CAM_ID_MAIN]->childCamId;
        if ((mainCamChildId != CAM_ID_MAIN) &&
            (play->cameraPtrs[mainCamChildId]->setting == CAM_SET_CS_TWISTED_HALLWAY)) {
            deleteOnepointDemo(play, mainCamChildId);
        }
        makeOnepointDemo(play, 3260, 40, &this->dyna.actor, CAM_ID_MAIN);
        demo_camera_no = makeOnepointDemo(play, 3261, 40, &this->dyna.actor, CAM_ID_MAIN);
    }
}

void mode_demo_move(BgMoriHineri* this, PlayState* play) {
    s8 objectSlot;

    if (play->activeCamId == demo_camera_no) {
        if (demo_camera_no != SUB_CAM_ID_DONE) {
            objectSlot = this->dyna.actor.objectSlot;
            this->dyna.actor.objectSlot = this->moriHineriObjectSlot;
            this->moriHineriObjectSlot = objectSlot;
            this->dyna.actor.params ^= 1;
            demo_camera_no = SUB_CAM_ID_DONE;
#if OOT_VERSION >= PAL_1_0
            Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
#endif
        } else {
            this->dyna.actor.draw = NULL;
            this->actionFunc = mode_demo_wait;
            demo_camera_no = CAM_ID_NONE;
        }
    }
    if ((demo_camera_no >= CAM_ID_SUB_FIRST) && ((GET_ACTIVE_CAM(play)->eye.z - this->dyna.actor.world.pos.z) < 1100.0f)) {
        Actor_fix_level_SE_set(&this->dyna.actor, NA_SE_EV_FLOOR_ROLLING - SFX_FLAG);
    }
}

void Bg_Mori_Hineri_actor_move(Actor* thisx, PlayState* play) {
    BgMoriHineri* this = (BgMoriHineri*)thisx;

    this->actionFunc(this, play);
}

void Bg_Mori_Hineri_actor_draw(Actor* thisx, PlayState* play) {
    BgMoriHineri* this = (BgMoriHineri*)thisx;
    PlayState* play2 = (PlayState*)play;
    MtxF mtx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_mori_hineri.c", 611);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, play2->objectCtx.slots[this->moriTexObjectSlot].segment);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play2->state.gfxCtx, "../z_bg_mori_hineri.c", 618);
    gSPDisplayList(POLY_OPA_DISP++, display_model[this->dyna.actor.params]);
    if (this->boxObjectSlot > 0) {
        Matrix_get(&mtx);
    }
    if ((this->actionFunc == mode_rotate) && (this->dyna.actor.shape.rot.z != 0)) {
        _texture_z_light_fog_prim(play->state.gfxCtx);
        if (this->dyna.actor.params == 0) {
            Matrix_translate(-1761.0f, 1278.0f, -1821.0f, MTXMODE_NEW);
        } else {
            Matrix_translate(1999.0f, 1278.0f, -1821.0f, MTXMODE_NEW);
        }
        Matrix_rotateXYZ(0, -0x8000, this->dyna.actor.shape.rot.z, MTXMODE_APPLY);
        Matrix_translate(0.0f, -50.0f, 0.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mori_hineri.c", 652);
        gSPDisplayList(POLY_OPA_DISP++, gDungeonDoorDL);
    }
    if ((this->boxObjectSlot > 0) && ((this->boxObjectSlot = Object_Exchange_bank_check(&play2->objectCtx, OBJECT_BOX)) > 0) &&
        Object_Exchange_bank_dma_check(&play2->objectCtx, this->boxObjectSlot)) {
        gSPSegment(POLY_OPA_DISP++, 0x06, play2->objectCtx.slots[this->boxObjectSlot].segment);
        gSPSegment(POLY_OPA_DISP++, 0x08, &Actor_change_render_mode[2]);
        Matrix_put(&mtx);
        Matrix_translate(147.0f, -245.0f, -453.0f, MTXMODE_APPLY);
        Matrix_rotateY(M_PI / 2, MTXMODE_APPLY);
        Matrix_scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mori_hineri.c", 689);
        gSPDisplayList(POLY_OPA_DISP++, gTreasureChestBossKeyChestFrontDL);
        Matrix_put(&mtx);
        Matrix_translate(167.0f, -218.0f, -453.0f, MTXMODE_APPLY);
        if (Actor_Environment_Tbox_Check(play2, 0xE)) {
            Matrix_rotateZ(BINANG_TO_RAD(0x3500), MTXMODE_APPLY);
        } else {
            Matrix_rotateZ(M_PI, MTXMODE_APPLY);
        }
        Matrix_scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_mori_hineri.c", 703);
        gSPDisplayList(POLY_OPA_DISP++, gTreasureChestBossKeyChestSideAndTopDL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_mori_hineri.c", 709);
}
