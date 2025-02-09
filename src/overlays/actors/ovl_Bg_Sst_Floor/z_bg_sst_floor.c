/*
 * File: z_bg_sst_floor.c
 * Overlay: ovl_Bg_Sst_Floor
 * Description: Bongo Bongo's drum
 */

#include "z_bg_sst_floor.h"
#include "assets/objects/object_sst/object_sst.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Bg_Sst_Floor_actor_ct(Actor* thisx, PlayState* play);
void Bg_Sst_Floor_actor_dt(Actor* thisx, PlayState* play);
void Bg_Sst_Floor_actor_move(Actor* thisx, PlayState* play);
void Bg_Sst_Floor_actor_draw(Actor* thisx, PlayState* play);

static s32 map_center_pos[] = { 0, 0, 0 }; // Unused, probably a zero vector

ActorProfile Bg_Sst_Floor_Profile = {
    /**/ ACTOR_BG_SST_FLOOR,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SST,
    /**/ sizeof(BgSstFloor),
    /**/ Bg_Sst_Floor_actor_ct,
    /**/ Bg_Sst_Floor_actor_dt,
    /**/ Bg_Sst_Floor_actor_move,
    /**/ Bg_Sst_Floor_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale.x, 100, ICHAIN_STOP),
};

void Bg_Sst_Floor_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    BgSstFloor* this = (BgSstFloor*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyUty_bgdi_SG2KSG(&gBongoDrumCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
}

void Bg_Sst_Floor_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    BgSstFloor* this = (BgSstFloor*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void Bg_Sst_Floor_actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    BgSstFloor* this = (BgSstFloor*)thisx;
    Player* player = GET_PLAYER(play);
    CollisionHeader* colHeader = SEGMENTED_TO_VIRTUAL(&gBongoDrumCol);

    colHeader->vtxList = SEGMENTED_TO_VIRTUAL(colHeader->vtxList);

    if (1) {}

    if (MoveBG_checkOverPlayerStatus(&this->dyna) && (this->dyna.actor.yDistToPlayer < 1000.0f)) {
        changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_BOSS_BONGO);
    } else {
        changeCameraSet(play->cameraPtrs[CAM_ID_MAIN], CAM_SET_DUNGEON0);
    }

    if (MoveBG_checkRidePlayerStatus(&this->dyna) && (player->fallDistance > 1000.0f)) {
        this->dyna.actor.params = 1;
        Actor_SE_set(&this->dyna.actor, NA_SE_EN_SHADEST_TAIKO_HIGH);
    }

    if (this->dyna.actor.params == BONGOFLOOR_HIT) {
        Actor* item00 = play->actorCtx.actorLists[ACTORCAT_MISC].head;
        f32 distFromRim;
        f32 xzDist;

        this->drumAmp = 80;
        this->dyna.actor.params = BONGOFLOOR_REST;
        this->drumPhase = 28;

        if (MoveBG_checkRidePlayerStatus(&this->dyna) &&
            !(player->stateFlags1 & (PLAYER_STATE1_13 | PLAYER_STATE1_14))) {
            distFromRim = 600.0f - this->dyna.actor.xzDistToPlayer;
            if (distFromRim > 0.0f) {
                if (distFromRim > 350.0f) {
                    distFromRim = 350.0f;
                }
                player->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
                player->actor.velocity.y = 9.0f * distFromRim * (1.0f / 350.0f);
            }
        }

        while (item00 != NULL) {
            if ((item00->id == ACTOR_EN_ITEM00) && (item00->world.pos.y == 0.0f)) {
                xzDist = Actor_search_actor_distanceXZ(&this->dyna.actor, item00);
                distFromRim = 600.0f - xzDist;
                if (xzDist < 600.0f) {
                    if (distFromRim > 350.0f) {
                        distFromRim = 350.0f;
                    }
                    item00->bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH);
                    item00->velocity.y = 9.0f * distFromRim * (1.0f / 350.0f);
                }
            }
            item00 = item00->next;
        }
    }
    this->drumHeight = sinf(this->drumPhase * (M_PI / 2)) * (-this->drumAmp);
    chase_s(&this->drumAmp, 0, 5);

    colHeader->vtxList[1].y = colHeader->vtxList[0].y = colHeader->vtxList[2].y = colHeader->vtxList[3].y =
        colHeader->vtxList[4].y = colHeader->vtxList[7].y = colHeader->vtxList[9].y = colHeader->vtxList[11].y =
            colHeader->vtxList[13].y = this->dyna.actor.home.pos.y + this->drumHeight;

    if (this->drumPhase != 0) {
        this->drumPhase--;
    }
    if (1) {}
    DynaPolygonInfo_setExpand(play, &play->colCtx.dyna);
}

void Bg_Sst_Floor_actor_draw(Actor* thisx, PlayState* play) {
    BgSstFloor* this = (BgSstFloor*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_sst_floor.c", 277);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    Matrix_scale(1.0f, this->drumHeight * -0.0025f, 1.0f, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_bg_sst_floor.c", 283);

    gSPDisplayList(POLY_OPA_DISP++, gBongoDrumDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_sst_floor.c", 287);
}
