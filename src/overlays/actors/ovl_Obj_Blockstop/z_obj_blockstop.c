/*
 * File: z_obj_blockstop.c
 * Overlay: ovl_Obj_Blockstop
 * Description: Stops blocks and sets relevant flags when the block is in position.
 */

#include "z_obj_blockstop.h"
#include "overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.h"

#define FLAGS 0

void Obj_Blockstop_actor_ct(Actor* thisx, PlayState* play);
void Obj_Blockstop_actor_dt(Actor* thisx, PlayState* play);
void Obj_Blockstop_actor_move(Actor* thisx, PlayState* play);

ActorProfile Obj_Blockstop_Profile = {
    /**/ ACTOR_OBJ_BLOCKSTOP,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(ObjBlockstop),
    /**/ Obj_Blockstop_actor_ct,
    /**/ Obj_Blockstop_actor_dt,
    /**/ Obj_Blockstop_actor_move,
    /**/ NULL,
};

void Obj_Blockstop_actor_ct(Actor* thisx, PlayState* play) {
    ObjBlockstop* this = (ObjBlockstop*)thisx;

    if (Actor_Environment_sw_Check(play, this->actor.params)) {
        Actor_delete(&this->actor);
    } else {
        this->actor.world.pos.y++;
    }
}

void Obj_Blockstop_actor_dt(Actor* thisx, PlayState* play) {
}

void Obj_Blockstop_actor_move(Actor* thisx, PlayState* play) {
    ObjBlockstop* this = (ObjBlockstop*)thisx;
    DynaPolyActor* dynaPolyActor;
    Vec3f sp4C;
    s32 bgId;
    s32 pad;

    if (T_BGCheck_ObjLineCheck_poly_chgrp_aiac(&play->colCtx, &this->actor.home.pos, &this->actor.world.pos, &sp4C,
                                &this->actor.floorPoly, false, false, true, true, &bgId, &this->actor)) {
        dynaPolyActor = DynaPolyInfo_actor_index2pointer(&play->colCtx, bgId);

        if (dynaPolyActor != NULL && dynaPolyActor->actor.id == ACTOR_OBJ_OSHIHIKI) {
            if (PARAMS_GET_U(dynaPolyActor->actor.params, 0, 4) == PUSHBLOCK_HUGE_START_ON ||
                PARAMS_GET_U(dynaPolyActor->actor.params, 0, 4) == PUSHBLOCK_HUGE_START_OFF) {
                Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
            } else {
                Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
            }

            Actor_Environment_sw_On(play, this->actor.params);
            Actor_delete(&this->actor);
        }
    }
}
