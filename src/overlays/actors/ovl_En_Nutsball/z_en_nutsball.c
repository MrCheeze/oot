/*
 * File: z_en_nutsball.c
 * Overlay: ovl_En_Nutsball
 * Description: Projectile fired by deku scrubs
 */

#include "z_en_nutsball.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "assets/objects/object_dekunuts/object_dekunuts.h"
#include "assets/objects/object_hintnuts/object_hintnuts.h"
#include "assets/objects/object_shopnuts/object_shopnuts.h"
#include "assets/objects/object_dns/object_dns.h"
#include "assets/objects/object_dnk/object_dnk.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Nutsball_actor_ct(Actor* thisx, PlayState* play);
void En_Nutsball_actor_dt(Actor* thisx, PlayState* play);
void En_Nutsball_actor_move(Actor* thisx, PlayState* play);
void En_Nutsball_actor_draw(Actor* thisx, PlayState* play);

static void mode_dma_wait(EnNutsball* this, PlayState* play);
static void mode_move(EnNutsball* this, PlayState* play);

ActorProfile En_Nutsball_Profile = {
    /**/ ACTOR_EN_NUTSBALL,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnNutsball),
    /**/ En_Nutsball_actor_ct,
    /**/ En_Nutsball_actor_dt,
    /**/ En_Nutsball_actor_move,
    /**/ NULL,
};

static ColliderCylinderInit NutsballAllPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x00, 0x08 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_WOOD,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 13, 13, 0, { 0 } },
};

static s16 shape_bank[] = {
    OBJECT_DEKUNUTS, OBJECT_HINTNUTS, OBJECT_SHOPNUTS, OBJECT_DNS, OBJECT_DNK,
};

static Gfx* shape_model[] = {
    gDekuNutsDekuNutDL, gHintNutsNutDL, gBusinessScrubDekuNutDL, gDntJijiNutDL, gDntStageNutDL,
};

void En_Nutsball_actor_ct(Actor* thisx, PlayState* play) {
    EnNutsball* this = (EnNutsball*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 400.0f, Actor_shadow_circle, 13.0f);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &NutsballAllPipeData);
    this->requiredObjectSlot = Object_Exchange_bank_check(&play->objectCtx, shape_bank[NUTSBALL_GET_TYPE(&this->actor)]);

    if (this->requiredObjectSlot < 0) {
        Actor_delete(&this->actor);
    } else {
        this->actionFunc = mode_dma_wait;
    }
}

void En_Nutsball_actor_dt(Actor* thisx, PlayState* play) {
    EnNutsball* this = (EnNutsball*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static void mode_dma_wait(EnNutsball* this, PlayState* play) {
    if (Object_Exchange_bank_dma_check(&play->objectCtx, this->requiredObjectSlot)) {
        this->actor.objectSlot = this->requiredObjectSlot;
        this->actor.draw = En_Nutsball_actor_draw;
        this->actor.shape.rot.y = 0;
        this->timer = 30;
        this->actionFunc = mode_move;
        this->actor.speed = 10.0f;
    }
}

static void mode_move(EnNutsball* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3s sp4C;
    Vec3f sp40;

    this->timer--;

    if (this->timer == 0) {
        this->actor.gravity = -1.0f;
    }

    this->actor.home.rot.z += 0x2AA8;

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
        (this->collider.base.atFlags & AT_HIT) || (this->collider.base.acFlags & AC_HIT) ||
        (this->collider.base.ocFlags1 & OC1_HIT)) {
        // Checking if the player is using a shield that reflects projectiles
        // And if so, reflects the projectile on impact
        if ((player->currentShield == PLAYER_SHIELD_DEKU) ||
            ((player->currentShield == PLAYER_SHIELD_HYLIAN) && LINK_IS_ADULT)) {
            if ((this->collider.base.atFlags & AT_HIT) && (this->collider.base.atFlags & AT_TYPE_ENEMY) &&
                (this->collider.base.atFlags & AT_BOUNCED)) {
                this->collider.base.atFlags &= ~AT_TYPE_ENEMY & ~AT_BOUNCED & ~AT_HIT;
                this->collider.base.atFlags |= AT_TYPE_PLAYER;

                this->collider.elem.atDmgInfo.dmgFlags = DMG_DEKU_STICK;
                Matrix_to_rotate_new(&player->shieldMf, &sp4C, 0);
                this->actor.world.rot.y = sp4C.y + 0x8000;
                this->timer = 30;
                return;
            }
        }

        sp40.x = this->actor.world.pos.x;
        sp40.y = this->actor.world.pos.y + 4;
        sp40.z = this->actor.world.pos.z;

        Effect_Hahen_Kakusan_ct3(play, &sp40, 6.0f, 0, 7, 3, 15, HAHEN_OBJECT_DEFAULT, 10, NULL);
        Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EN_OCTAROCK_ROCK);
        Actor_delete(&this->actor);
    } else {
        if (this->timer == -300) {
            Actor_delete(&this->actor);
        }
    }
}

void En_Nutsball_actor_move(Actor* thisx, PlayState* play) {
    EnNutsball* this = (EnNutsball*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad;

    if (!(player->stateFlags1 & (PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_28 | PLAYER_STATE1_29)) ||
        (this->actionFunc == mode_dma_wait)) {
        this->actionFunc(this, play);

        Actor_position_moveF(&this->actor);
        Actor_BGcheck2(play, &this->actor, 10, NutsballAllPipeData.dim.radius, NutsballAllPipeData.dim.height,
                                UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

        this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;

        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void En_Nutsball_actor_draw(Actor* thisx, PlayState* play) {
    EnNutsball* this = (EnNutsball*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_nutsball.c", 327);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Matrix_mult(&play->billboardMtxF, MTXMODE_APPLY);
    Matrix_rotateZ(this->actor.home.rot.z * 9.58738e-05f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_nutsball.c", 333);
    gSPDisplayList(POLY_OPA_DISP++, shape_model[NUTSBALL_GET_TYPE(&this->actor)]);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_nutsball.c", 337);
}
