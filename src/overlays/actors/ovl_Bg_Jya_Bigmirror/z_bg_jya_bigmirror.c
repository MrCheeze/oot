/*
 * File: z_bg_jya_bigmirror.c
 * Overlay: ovl_Bg_Jya_Bigmirror
 * Description: Ceiling mirror and puzzle in Spirit Temple top rooms
 */

#include "z_bg_jya_bigmirror.h"
#include "assets/objects/object_jya_obj/object_jya_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Jya_Bigmirror_actor_ct(Actor* thisx, PlayState* play);
void Bg_Jya_Bigmirror_actor_dt(Actor* thisx, PlayState* play);
void Bg_Jya_Bigmirror_actor_move(Actor* thisx, PlayState* play);
void Bg_Jya_Bigmirror_actor_draw(Actor* thisx, PlayState* play);

static u8 JBM_HasseiFlag = false;

ActorProfile Bg_Jya_Bigmirror_Profile = {
    /**/ ACTOR_BG_JYA_BIGMIRROR,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_JYA_OBJ,
    /**/ sizeof(BgJyaBigmirror),
    /**/ Bg_Jya_Bigmirror_actor_ct,
    /**/ Bg_Jya_Bigmirror_actor_dt,
    /**/ Bg_Jya_Bigmirror_actor_move,
    /**/ Bg_Jya_Bigmirror_actor_draw,
};

typedef struct BigMirrorDataEntry {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 params;
    /* 0x0E */ s16 solvedRotY;
    /* 0x10 */ s16 initRotY;
} BigMirrorDataEntry; // size = 0x14

static BigMirrorDataEntry Cobra_data[] = {
    { { -560.0f, 1743.0f, -310.0f }, 0xFF01, 0x4000, 0x8000 },
    { { 60.0f, 1743.0f, -310.0f }, 0xFF02, 0x8000, 0xA000 },
};

void watch_room_bigmirror(Actor* thisx, PlayState* play) {
    BgJyaBigmirror* this = (BgJyaBigmirror*)thisx;

    this->puzzleFlags &=
        ~(BIGMIR_PUZZLE_IN_STATUE_ROOM | BIGMIR_PUZZLE_IN_1ST_TOP_ROOM | BIGMIR_PUZZLE_IN_2ND_TOP_ROOM);
    if (play->roomCtx.curRoom.num == 5) {
        this->puzzleFlags |= BIGMIR_PUZZLE_IN_STATUE_ROOM;
    } else if (play->roomCtx.curRoom.num == 0x19) {
        this->puzzleFlags |= BIGMIR_PUZZLE_IN_1ST_TOP_ROOM;
    } else if (play->roomCtx.curRoom.num == 0x1A) {
        this->puzzleFlags |= BIGMIR_PUZZLE_IN_2ND_TOP_ROOM;
    }
}

void watch_cobra_bigmirror(Actor* thisx, PlayState* play) {
    static u8 c_good_bit[] = { BIGMIR_PUZZLE_COBRA1_SOLVED, BIGMIR_PUZZLE_COBRA2_SOLVED };
    BgJyaBigmirror* this = (BgJyaBigmirror*)thisx;
    BigMirrorDataEntry* curSpawnData;
    BigmirrorCobra* curCobraInfo;
    s32 i;

    if (this->puzzleFlags & (BIGMIR_PUZZLE_IN_1ST_TOP_ROOM | BIGMIR_PUZZLE_IN_2ND_TOP_ROOM)) {
        for (i = 0; i < 2; i++) {
            curSpawnData = &Cobra_data[i];
            curCobraInfo = &this->cobraInfo[i];
            if (curCobraInfo->cobra != NULL) {
                curCobraInfo->rotY = curCobraInfo->cobra->dyna.actor.shape.rot.y;

                if (curCobraInfo->rotY == curSpawnData->solvedRotY) {
                    this->puzzleFlags |= c_good_bit[i];
                } else {
                    this->puzzleFlags &= ~c_good_bit[i];
                }

#if DEBUG_FEATURES
                if (curCobraInfo->cobra->dyna.actor.update == NULL) {
                    // "Cobra deleted"
                    PRINTF("Error : コブラ削除された (%s %d)\n", "../z_bg_jya_bigmirror.c", 203);
                }
#endif
            } else {
                curCobraInfo->cobra = (BgJyaCobra*)Actor_info_make_child_actor(
                    &play->actorCtx, &this->actor, play, ACTOR_BG_JYA_COBRA, curSpawnData->pos.x, curSpawnData->pos.y,
                    curSpawnData->pos.z, 0, curCobraInfo->rotY, 0, curSpawnData->params);
                this->actor.child = NULL;

                if (curCobraInfo->cobra == NULL) {
                    // "Cobra generation failed"
                    PRINTF("Error : コブラ発生失敗 (%s %d)\n", "../z_bg_jya_bigmirror.c", 221);
                }
            }
        }
    } else {

        for (i = 0; i < 2; i++) {
            curCobraInfo = &this->cobraInfo[i];
            if (curCobraInfo->cobra != NULL) {
                if (curCobraInfo->cobra->dyna.actor.child != NULL) {
                    Actor_delete(curCobraInfo->cobra->dyna.actor.child);
                    curCobraInfo->cobra->dyna.actor.child = NULL;
                }
                Actor_delete(&curCobraInfo->cobra->dyna.actor);
                curCobraInfo->cobra = NULL;
            }
        }
    }
}

void watch_bombiwa_bigmirror(Actor* thisx, PlayState* play) {
    BgJyaBigmirror* this = (BgJyaBigmirror*)thisx;

    if (Actor_Environment_sw_Check(play, 0x29)) {
        this->puzzleFlags |= BIGMIR_PUZZLE_BOMBIWA_DESTROYED;
    } else {
        this->puzzleFlags &= ~(BIGMIR_PUZZLE_BOMBIWA_DESTROYED);
    }
}

void watch_MirRay_bigmirror(Actor* thisx, PlayState* play) {
    static s16 arg_data[] = { 0x0005, 0x0007, 0x0008 };
    static Vec3f pos[] = {
        { 60.0f, 1802.0f, -1102.0f },
        { -560.0f, 1800.0f, -310.0f },
        { 60.0f, 1800.0f, -310.0f },
    };
    BgJyaBigmirror* this = (BgJyaBigmirror*)thisx;
    s32 puzzleSolved;
    s32 lightBeamToggles[3];
    s32 i;
    s32 mirRayObjectSlot;

    mirRayObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_MIR_RAY);

    if ((mirRayObjectSlot < 0) || (mirRayObjectSlot != this->mirRayObjectSlot)) {
        this->lightBeams[2] = NULL;
        this->lightBeams[1] = NULL;
        this->lightBeams[0] = NULL;
    } else {
        // Only spawn if puzzle solved
        lightBeamToggles[0] = (this->puzzleFlags & (BIGMIR_PUZZLE_IN_STATUE_ROOM | BIGMIR_PUZZLE_IN_1ST_TOP_ROOM)) &&
                              (this->puzzleFlags & BIGMIR_PUZZLE_COBRA2_SOLVED) &&
                              (this->puzzleFlags & BIGMIR_PUZZLE_COBRA1_SOLVED);
        lightBeamToggles[1] = lightBeamToggles[2] =
            this->puzzleFlags & (BIGMIR_PUZZLE_IN_1ST_TOP_ROOM | BIGMIR_PUZZLE_IN_2ND_TOP_ROOM);

        for (i = 0; i < 3; i++) {
            if (lightBeamToggles[i]) {
                if ((this->lightBeams[i] == NULL) && Object_Exchange_bank_dma_check(&play->objectCtx, mirRayObjectSlot)) {

                    this->lightBeams[i] =
                        Actor_info_make_actor(&play->actorCtx, play, ACTOR_MIR_RAY, pos[i].x, pos[i].y,
                                    pos[i].z, 0, 0, 0, arg_data[i]);

#if DEBUG_FEATURES
                    if (this->lightBeams[i] == NULL) {
                        // "Mir Ray generation failed"
                        PRINTF("Error : Mir Ray 発生失敗 (%s %d)\n", "../z_bg_jya_bigmirror.c", 310);
                    }
#endif
                }
            } else {
                if (this->lightBeams[i] != NULL) {
                    Actor_delete(this->lightBeams[i]);
                    this->lightBeams[i] = NULL;
                }
            }
        }
    }
    this->mirRayObjectSlot = mirRayObjectSlot;
}

void Bg_Jya_Bigmirror_actor_ct(Actor* thisx, PlayState* play) {
    BgJyaBigmirror* this = (BgJyaBigmirror*)thisx;

    if (JBM_HasseiFlag) {
        Actor_delete(&this->actor);
        return;
    }

    Actor_set_scale(&this->actor, 0.1f);
    this->cobraInfo[0].rotY = Cobra_data[0].initRotY;
    this->cobraInfo[1].rotY = Cobra_data[1].initRotY;
    this->actor.room = -1;
    JBM_HasseiFlag = true;
    this->spawned = true;
    this->mirRayObjectSlot = -1;

    // "jya Bigmirror"
    PRINTF("(jya 大鏡)(arg_data 0x%04x)\n", this->actor.params);
}

void Bg_Jya_Bigmirror_actor_dt(Actor* thisx, PlayState* play) {
    BgJyaBigmirror* this = (BgJyaBigmirror*)thisx;

    if (this->spawned) {
        JBM_HasseiFlag = false;
    }
}

void Bg_Jya_Bigmirror_actor_move(Actor* thisx, PlayState* play) {
    watch_room_bigmirror(thisx, play);
    watch_cobra_bigmirror(thisx, play);
    watch_bombiwa_bigmirror(thisx, play);
    watch_MirRay_bigmirror(thisx, play);
}

void draw_hikari_bigmirror(Actor* thisx, PlayState* play) {
    static Vec3s a = { 0, 0, 0 };
    BgJyaBigmirror* this = (BgJyaBigmirror*)thisx;
    Actor* lift;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_jya_bigmirror.c", 435);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    lift = Actor_info_name_search(&play->actorCtx, ACTOR_BG_JYA_LIFT, ACTORCAT_BG);
    if (lift != NULL) {
        this->liftHeight = lift->world.pos.y;
    }
    Matrix_softcv3_load(this->actor.world.pos.x, this->actor.world.pos.y + 40.0f, this->actor.world.pos.z,
                                 &this->actor.shape.rot);
    Matrix_scale(0.1f, (this->liftHeight * -(1.0f / 1280.0f)) + (1779.4f / 1280.0f), 0.1f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_jya_bigmirror.c", 457);
    gSPDisplayList(POLY_XLU_DISP++, gBigMirror1DL);

    if (lift != NULL) {
        Matrix_softcv3_load(lift->world.pos.x, lift->world.pos.y, lift->world.pos.z, &a);
        Matrix_scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_jya_bigmirror.c", 467);
        gSPDisplayList(POLY_XLU_DISP++, gBigMirror2DL);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_jya_bigmirror.c", 476);
}

void Bg_Jya_Bigmirror_actor_draw(Actor* thisx, PlayState* play) {
    BgJyaBigmirror* this = (BgJyaBigmirror*)thisx;

    if (this->puzzleFlags & BIGMIR_PUZZLE_IN_1ST_TOP_ROOM) {
        Cheap_gfx_display(play, gBigMirror3DL);
        Cheap_gfx_display_xlu(play, gBigMirror4DL);
    }

    if ((this->puzzleFlags &
         (BIGMIR_PUZZLE_IN_STATUE_ROOM | BIGMIR_PUZZLE_IN_1ST_TOP_ROOM | BIGMIR_PUZZLE_IN_2ND_TOP_ROOM)) &&
        (this->puzzleFlags & BIGMIR_PUZZLE_COBRA2_SOLVED) && (this->puzzleFlags & BIGMIR_PUZZLE_COBRA1_SOLVED)) {
        draw_hikari_bigmirror(&this->actor, play);
    }
}
