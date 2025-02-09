/*
 * File: z_bg_spot09_obj.c
 * Overlay: ovl_Bg_Spot09_Obj
 * Description:
 */

#include "z_bg_spot09_obj.h"
#include "assets/objects/object_spot09_obj/object_spot09_obj.h"

#define FLAGS 0

void Bg_Spot09_Obj_actor_ct(Actor* thisx, PlayState* play);
void Bg_Spot09_Obj_actor_dt(Actor* thisx, PlayState* play);
void Bg_Spot09_Obj_actor_move(Actor* thisx, PlayState* play);
void Bg_Spot09_Obj_actor_draw(Actor* thisx, PlayState* play);

s32 ct_common_checkAppear(BgSpot09Obj* this, PlayState* play);
static s32 ct_common_setScale(BgSpot09Obj* this, PlayState* play);
static s32 ct_common_setDynaPoly(BgSpot09Obj* this, PlayState* play);

ActorProfile Bg_Spot09_Obj_Profile = {
    /**/ ACTOR_BG_SPOT09_OBJ,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPOT09_OBJ,
    /**/ sizeof(BgSpot09Obj),
    /**/ Bg_Spot09_Obj_actor_ct,
    /**/ Bg_Spot09_Obj_actor_dt,
    /**/ Bg_Spot09_Obj_actor_move,
    /**/ Bg_Spot09_Obj_actor_draw,
};

static CollisionHeader* BGDT_info[] = {
    NULL, &gValleyObjects1Col, &gValleyObjects2Col, &gValleyObjects3Col, &gValleyObjects4Col,
};

static s32 (*ct_commonProcTbl[])(BgSpot09Obj* this, PlayState* play) = {
    ct_common_setDynaPoly,
    ct_common_checkAppear,
    ct_common_setScale,
};

s32 ct_common_checkAppear(BgSpot09Obj* this, PlayState* play) {
    s32 carpentersRescued;

    if (IS_CUTSCENE_LAYER) {
        return this->dyna.actor.params == 0;
    }

    carpentersRescued = GET_EVENTCHKINF_CARPENTERS_ALL_RESCUED();

    if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
        switch (this->dyna.actor.params) {
            case 0:
                return 0;
            case 1:
                return !carpentersRescued;
            case 4:
                return carpentersRescued;
            case 3:
                return 1;
        }
    } else {
        return this->dyna.actor.params == 2;
    }

    return 0;
}

static s32 ct_common_setScale(BgSpot09Obj* this, PlayState* play) {
    if (this->dyna.actor.params == 3) {
        Actor_set_scale(&this->dyna.actor, 0.1f);
    } else {
        Actor_set_scale(&this->dyna.actor, 1.0f);
    }
    return 1;
}

static s32 ct_common_setDynaPoly(BgSpot09Obj* this, PlayState* play) {
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 pad2[2];

    if (BGDT_info[this->dyna.actor.params] != NULL) {
        MoveBG_ct(&this->dyna, 0);
        DynaPolyUty_bgdi_SG2KSG(BGDT_info[this->dyna.actor.params], &colHeader);
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    }
    return true;
}

static s32 ct_common(BgSpot09Obj* this, PlayState* play) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(ct_commonProcTbl); i++) {
        if (!ct_commonProcTbl[i](this, play)) {
            return false;
        }
    }
    return true;
}

s32 ct_original_hashi(BgSpot09Obj* this, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_F32(cullingVolumeDistance, 7200, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 3000, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 7200, ICHAIN_STOP),
    };

    ValueSet_process(&this->dyna.actor, value_init);
    return true;
}

s32 ct_original_tent(BgSpot09Obj* this, PlayState* play) {
    static InitChainEntry value_init[] = {
        ICHAIN_F32(cullingVolumeDistance, 7200, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeScale, 800, ICHAIN_CONTINUE),
        ICHAIN_F32(cullingVolumeDownward, 1500, ICHAIN_STOP),
    };

    ValueSet_process(&this->dyna.actor, value_init);
    return true;
}

static Gfx* shape_data[] = {
    gValleyBridgeSidesDL, gValleyBrokenBridgeDL, gValleyBridgeChildDL, gCarpentersTentDL, gValleyRepairedBridgeDL,
};

static s32 ct_original(BgSpot09Obj* this, PlayState* play) {
    if (this->dyna.actor.params == 3) {
        return ct_original_tent(this, play);
    } else {
        return ct_original_hashi(this, play);
    }
}

void Bg_Spot09_Obj_actor_ct(Actor* thisx, PlayState* play) {
    BgSpot09Obj* this = (BgSpot09Obj*)thisx;

    PRINTF("Spot09 Object [arg_data : 0x%04x](大工救出フラグ 0x%x)\n", this->dyna.actor.params,
           GET_EVENTCHKINF_CARPENTERS_RESCUED_FLAGS());
    this->dyna.actor.params &= 0xFF;
    if ((this->dyna.actor.params < 0) || (this->dyna.actor.params >= 5)) {
        PRINTF("Error : Spot 09 object の arg_data が判別出来ない(%s %d)(arg_data 0x%04x)\n", "../z_bg_spot09_obj.c",
               322, this->dyna.actor.params);
    }

    if (!ct_common(this, play)) {
        Actor_delete(&this->dyna.actor);
    } else if (!ct_original(this, play)) {
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Spot09_Obj_actor_dt(Actor* thisx, PlayState* play) {
    DynaCollisionContext* dynaColCtx = &play->colCtx.dyna;
    BgSpot09Obj* this = (BgSpot09Obj*)thisx;

    if (this->dyna.actor.params != 0) {
        DynaPolyInfo_delReserve(play, dynaColCtx, this->dyna.bgId);
    }
}

void Bg_Spot09_Obj_actor_move(Actor* thisx, PlayState* play) {
}

void Bg_Spot09_Obj_actor_draw(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, shape_data[thisx->params]);

    if (thisx->params == 3) {
        OPEN_DISPS(play->state.gfxCtx, "../z_bg_spot09_obj.c", 388);

        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_spot09_obj.c", 391);
        gSPDisplayList(POLY_XLU_DISP++, gCarpentersTentEntranceDL);

        CLOSE_DISPS(play->state.gfxCtx, "../z_bg_spot09_obj.c", 396);
    }
}
