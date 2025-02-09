/*
 * File: z_en_blkobj.c
 * Overlay: ovl_En_Blkobj
 * Description: Dark Link's Illusion Room
 */

#include "z_en_blkobj.h"
#include "assets/objects/object_blkobj/object_blkobj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Blkobj_Actor_ct(Actor* thisx, PlayState* play);
void En_Blkobj_Actor_dt(Actor* thisx, PlayState* play);
void En_Blkobj_Actor_move(Actor* thisx, PlayState* play);
void En_Blkobj_Actor_draw(Actor* thisx, PlayState* play);

static void move_wait(EnBlkobj* this, PlayState* play);
void move_start_wait(EnBlkobj* this, PlayState* play);
void move_fight_wait(EnBlkobj* this, PlayState* play);
static void move_stop(EnBlkobj* this, PlayState* play);

ActorProfile En_Blkobj_Profile = {
    /**/ ACTOR_EN_BLKOBJ,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_BLKOBJ,
    /**/ sizeof(EnBlkobj),
    /**/ En_Blkobj_Actor_ct,
    /**/ En_Blkobj_Actor_dt,
    /**/ En_Blkobj_Actor_move,
    /**/ En_Blkobj_Actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F(scale, 1, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_STOP),
};

static Gfx render_mode_opa[] = {
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
    gsSPEndDisplayList(),
};

static Gfx render_mode_xlu[] = {
    gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_SURF2),
    gsSPEndDisplayList(),
};

void En_Blkobj_actor_set_process(EnBlkobj* this, EnBlkobjActionFunc actionFunc) {
    this->actionFunc = actionFunc;
    this->timer = 0;
}

void En_Blkobj_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBlkobj* this = (EnBlkobj*)thisx;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, 0);
    if (Actor_Environment_room_clear_Check(play, this->dyna.actor.room)) {
        this->alpha = 255;
        En_Blkobj_actor_set_process(this, move_stop);
    } else {
        DynaPolyUty_bgdi_SG2KSG(&gIllusionRoomCol, &colHeader);
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
        En_Blkobj_actor_set_process(this, move_wait);
    }
}

void En_Blkobj_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBlkobj* this = (EnBlkobj*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void move_wait(EnBlkobj* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->dyna.actor.xzDistToPlayer < 120.0f) {
        En_Blkobj_actor_set_process(this, move_start_wait);
    }
    player->stateFlags2 |= PLAYER_STATE2_26;
}

void move_start_wait(EnBlkobj* this, PlayState* play) {
    if (!(this->dyna.actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME)) {
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_TORCH2, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                    this->dyna.actor.world.pos.z, 0, this->dyna.actor.yawTowardsPlayer, 0, 0);
        En_Blkobj_actor_set_process(this, move_fight_wait);
    }
}

void move_fight_wait(EnBlkobj* this, PlayState* play) {
    s32 alphaMod;

    if (this->timer == 0) {
        if (Actor_info_name_search(&play->actorCtx, ACTOR_EN_TORCH2, ACTORCAT_BOSS) == NULL) {
            Actor_Environment_room_clear_On(play, this->dyna.actor.room);
            this->timer++;
        }
    } else if (this->timer++ > 100) {
        alphaMod = (this->timer - 100) >> 2;
        if (alphaMod > 5) {
            alphaMod = 5;
        }
        this->alpha += alphaMod;
        if (this->alpha > 255) {
            this->alpha = 255;
            En_Blkobj_actor_set_process(this, move_stop);
            DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
        }
    }
}

static void move_stop(EnBlkobj* this, PlayState* play) {
}

void En_Blkobj_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBlkobj* this = (EnBlkobj*)thisx;

    this->actionFunc(this, play);
}

void shape_display_set(PlayState* play, Gfx* dList, s32 alpha) {
    Gfx* segment;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_blkobj.c", 322);

    if (alpha == 255) {
        segment = render_mode_opa;
    } else {
        segment = render_mode_xlu;
    }

    gSPSegment(POLY_XLU_DISP++, 0x08, segment);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, alpha);
    gSPDisplayList(POLY_XLU_DISP++, dList);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_blkobj.c", 330);
}

void En_Blkobj_Actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBlkobj* this = (EnBlkobj*)thisx;
    s32 illusionAlpha;
    u32 gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_blkobj.c", 349);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    gameplayFrames = play->gameplayFrames % 128;

    gSPSegment(
        POLY_XLU_DISP++, 0x0D,
        two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames, 0, 32, 32, 1, gameplayFrames, 0, 32, 32));
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_blkobj.c", 363);

    if (this->alpha != 0) {
        shape_display_set(play, gIllusionRoomNormalDL, this->alpha);
    }
    illusionAlpha = 255 - this->alpha;
    if (illusionAlpha != 0) {
        shape_display_set(play, gIllusionRoomIllusionDL, illusionAlpha);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_blkobj.c", 375);
}
