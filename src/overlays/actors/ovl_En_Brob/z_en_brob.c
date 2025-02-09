/*
 * File: z_en_brob.c
 * Overlay: ovl_En_Brob
 * Description: Flobbery Muscle Block (Jabu-Jabu's Belly)
 */

#include "z_en_brob.h"
#include "assets/objects/object_brob/object_brob.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void En_Brob_actor_ct(Actor* thisx, PlayState* play);
void En_Brob_actor_dt(Actor* thisx, PlayState* play);
void En_Brob_actor_move(Actor* thisx, PlayState* play2);
void En_Brob_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait_init(EnBrob* this, PlayState* play);
static void mode_wait(EnBrob* this, PlayState* play);
static void mode_up(EnBrob* this, PlayState* play);
void mode_unyo(EnBrob* this, PlayState* play);
static void mode_damage(EnBrob* this, PlayState* play);
static void mode_down(EnBrob* this, PlayState* play);
void mode_elec(EnBrob* this, PlayState* play);

ActorProfile En_Brob_Profile = {
    /**/ ACTOR_EN_BROB,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BROB,
    /**/ sizeof(EnBrob),
    /**/ En_Brob_actor_ct,
    /**/ En_Brob_actor_dt,
    /**/ En_Brob_actor_move,
    /**/ En_Brob_actor_draw,
};

static ColliderCylinderInit BrobAllPipeData = {
    {
        COL_MATERIAL_HIT0,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0xFFCFFFFF, 0x03, 0x08 },
        { 0xFFCFFFFF, 0x01, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 8000, 11000, -5000, { 0, 0, 0 } },
};

static CollisionCheckInfoInit BrobStatusData = { 0, 60, 120, MASS_IMMOVABLE };

void En_Brob_actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBrob* this = (EnBrob*)thisx;
    CollisionHeader* colHeader = NULL;

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gBrobSkel, &gBrobMoveUpAnim, this->jointTable, this->morphTable, 10);
    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gBrobCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
    ClObjPipe_ct(play, &this->colliders[0]);
    ClObjPipe_set5(play, &this->colliders[0], &this->dyna.actor, &BrobAllPipeData);
    ClObjPipe_ct(play, &this->colliders[1]);
    ClObjPipe_set5(play, &this->colliders[1], &this->dyna.actor, &BrobAllPipeData);
    CollisionCheck_Status_set2(&thisx->colChkInfo, NULL, &BrobStatusData);
    if (PARAMS_GET_U(thisx->params, 8, 8) == 0) {
        Actor_set_scale(&this->dyna.actor, 0.01f);
        thisx->params &= 0xFF;
        if (thisx->params != 0xFF) {
            thisx->scale.y *= PARAMS_GET_U(thisx->params, 0, 8) * (1.0f / 30.0f);
        }
    } else {
        Actor_set_scale(&this->dyna.actor, 0.005f);
        thisx->params &= 0xFF;
        if (thisx->params != 0xFF) {
            thisx->scale.y *= PARAMS_GET_U(thisx->params, 0, 8) * (2.0f / 30.0f);
        }
    }

    this->colliders[0].dim.radius *= thisx->scale.x;
    this->colliders[0].dim.height = thisx->scale.y * 12000.0f;
    this->colliders[0].dim.yShift = 0;
    this->colliders[1].dim.radius *= thisx->scale.x;
    this->colliders[1].dim.height *= thisx->scale.y;
    this->colliders[1].dim.yShift *= thisx->scale.y;
    this->actionFunc = NULL;
    thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    mode_wait_init(this, play);
}

void En_Brob_actor_dt(Actor* thisx, PlayState* play) {
    EnBrob* this = (EnBrob*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjPipe_dt(play, &this->colliders[0]);
    ClObjPipe_dt(play, &this->colliders[1]);
}

static void mode_wait_init(EnBrob* this, PlayState* play) {
    DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    this->timer = this->actionFunc == mode_damage ? 200 : 0;
    this->modelOffsetY = 0;
    this->actionFunc = mode_wait;
}

static void mode_up_init(EnBrob* this, PlayState* play) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gBrobMoveUpAnim);
    DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    this->modelOffsetY = 1000;
    this->actionFunc = mode_up;
}

void mode_unyo_init(EnBrob* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gBrobWobbleAnim, -5.0f);
    this->modelOffsetY = 8000;
    this->timer = 1200;
    this->actionFunc = mode_unyo;
}

static void mode_damage_init(EnBrob* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gBrobStunnedAnim, -5.0f);
    this->modelOffsetY -= 125.0f;
    Set_Fog(&this->dyna.actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 80);
    Actor_SE_set(&this->dyna.actor, NA_SE_EN_GOMA_JR_FREEZE);
    this->actionFunc = mode_damage;
}

static void mode_down_init(EnBrob* this) {
    Skeleton_Info2_init(&this->skelAnime, &gBrobMoveUpAnim, -1.0f, Si2_anime_end_frame(&gBrobMoveUpAnim), 0.0f,
                     ANIMMODE_ONCE, -5.0f);
    this->modelOffsetY = 8250;
    this->actionFunc = mode_down;
}

void mode_elec_init(EnBrob* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gBrobShockAnim, -5.0f);
    this->timer = 10;
    this->actionFunc = mode_elec;
}

static void mode_wait(EnBrob* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
            Actor_player_power_damage_set(play, &this->dyna.actor, 5.0f, this->dyna.actor.yawTowardsPlayer,
                                                  1.0f);
            mode_up_init(this, play);
        } else if (this->dyna.actor.xzDistToPlayer < 300.0f) {
            mode_up_init(this, play);
        }
    } else if (this->timer >= 81) {
        this->dyna.actor.colorFilterTimer = 80;
    }
}

static void mode_up(EnBrob* this, PlayState* play) {
    f32 curFrame;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_unyo_init(this);
    } else {
        curFrame = this->skelAnime.curFrame;
        if (curFrame < 8.0f) {
            this->modelOffsetY += 1000.0f;
        } else if (curFrame < 12.0f) {
            this->modelOffsetY += 250.0f;
        } else {
            this->modelOffsetY -= 250.0f;
        }
    }
}

void mode_unyo(EnBrob* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 6.0f) || Skeleton_Info_frame_check(&this->skelAnime, 15.0f)) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EN_BROB_WAVE);
    }
    if (this->timer != 0) {
        this->timer--;
    }
    if ((this->timer == 0) && (this->dyna.actor.xzDistToPlayer > 500.0f)) {
        mode_down_init(this);
    }
}

static void mode_damage(EnBrob* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_wait_init(this, play);
    } else if (this->skelAnime.curFrame < 8.0f) {
        this->modelOffsetY -= 1250.0f;
    }
    this->dyna.actor.colorFilterTimer = 80;
}

static void mode_down(EnBrob* this, PlayState* play) {
    f32 curFrame;

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_wait_init(this, play);
    } else {
        curFrame = this->skelAnime.curFrame;
        if (curFrame < 8.0f) {
            this->modelOffsetY -= 1000.0f;
        } else if (curFrame < 12.0f) {
            this->modelOffsetY -= 250.0f;
        } else {
            this->modelOffsetY += 250.0f;
        }
    }
}

void mode_elec(EnBrob* this, PlayState* play) {
    Vec3f pos;
    f32 dist1;
    f32 dist2;
    s32 i;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0) && (this->timer != 0)) {
        this->timer--;
    }

    dist1 = play->gameplayFrames % 2 ? 0.0f : this->dyna.actor.scale.x * 5500.0f;
    dist2 = this->dyna.actor.scale.x * 5500.0f;

    for (i = 0; i < 4; i++) {
        static Color_RGBA8 lightning_prim = { 255, 255, 255, 255 };
        static Color_RGBA8 lightning_env = { 200, 255, 255, 255 };

        if (i % 2) {
            pos.x = this->dyna.actor.world.pos.x + dist1;
            pos.z = this->dyna.actor.world.pos.z + dist2;
        } else {
            pos.x = this->dyna.actor.world.pos.x + dist2;
            pos.z = this->dyna.actor.world.pos.z + dist1;
            dist1 = -dist1;
            dist2 = -dist2;
        }
        pos.y = (((fqrand() * 15000.0f) + 1000.0f) * this->dyna.actor.scale.y) + this->dyna.actor.world.pos.y;
        Effect_SS_Lightning_sc_cl_co_ct(play, &pos, &lightning_prim, &lightning_env, this->dyna.actor.scale.y * 8000.0f,
                                fqrand() * 65536.0f, 4, 1);
    }

    if (this->timer == 0) {
        mode_unyo_init(this);
    }
}

void En_Brob_actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBrob* this = (EnBrob*)thisx;
    s32 i;
    s32 acHits[2];

    acHits[0] = (this->colliders[0].base.acFlags & AC_HIT) != 0;
    acHits[1] = (this->colliders[1].base.acFlags & AC_HIT) != 0;
    if ((acHits[0] && (this->colliders[0].elem.acHitElem->atDmgInfo.dmgFlags & DMG_BOOMERANG)) ||
        (acHits[1] && (this->colliders[1].elem.acHitElem->atDmgInfo.dmgFlags & DMG_BOOMERANG))) {

        for (i = 0; i < 2; i++) {
            this->colliders[i].base.atFlags &= ~(AT_HIT | AT_BOUNCED);
            this->colliders[i].base.acFlags &= ~AC_HIT;
        }

        mode_damage_init(this);
    } else if ((this->colliders[0].base.atFlags & AT_HIT) || (this->colliders[1].base.atFlags & AT_HIT) ||
               (acHits[0] && (this->colliders[0].elem.acHitElem->atDmgInfo.dmgFlags & DMG_SLASH_KOKIRI)) ||
               (acHits[1] && (this->colliders[1].elem.acHitElem->atDmgInfo.dmgFlags & DMG_SLASH_KOKIRI))) {

        if (this->actionFunc == mode_up && !(this->colliders[0].base.atFlags & AT_BOUNCED) &&
            !(this->colliders[1].base.atFlags & AT_BOUNCED)) {
            Actor_player_power_damage_set(play, &this->dyna.actor, 5.0f, this->dyna.actor.yawTowardsPlayer,
                                                  1.0f);
        } else if (this->actionFunc != mode_up) {
            mode_elec_init(this);
        }

        for (i = 0; i < 2; i++) {
            this->colliders[i].base.atFlags &= ~(AT_HIT | AT_BOUNCED);
            this->colliders[i].base.acFlags &= ~AC_HIT;
        }
    }

    this->actionFunc(this, play);

    if (this->actionFunc != mode_wait && this->actionFunc != mode_down) {
        if (this->actionFunc != mode_damage) {
            CollisionCheck_setAT(play, &play->colChkCtx, &this->colliders[0].base);
            CollisionCheck_setAT(play, &play->colChkCtx, &this->colliders[1].base);
            if (this->actionFunc != mode_up) {
                CollisionCheck_setAC(play, &play->colChkCtx, &this->colliders[0].base);
                CollisionCheck_setAC(play, &play->colChkCtx, &this->colliders[1].base);
            }
        }
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliders[0].base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->colliders[1].base);
    }
}

void en_brob_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnBrob* this = (EnBrob*)thisx;
    MtxF mtx;

    Matrix_get(&mtx);
    if (limbIndex == 3) {
        this->colliders[0].dim.pos.x = mtx.xw;
        this->colliders[0].dim.pos.y = mtx.yw;
        this->colliders[0].dim.pos.z = mtx.zw;
    } else if (limbIndex == 8) {
        this->colliders[1].dim.pos.x = mtx.xw;
        this->colliders[1].dim.pos.y = (mtx.yw + 7.0f);
        this->colliders[1].dim.pos.z = mtx.zw;
    }
}

void En_Brob_actor_draw(Actor* thisx, PlayState* play) {
    EnBrob* this = (EnBrob*)thisx;

    _texture_z_light_fog_prim(play->state.gfxCtx);
    Matrix_translate(0.0f, this->modelOffsetY, 0.0f, MTXMODE_APPLY);
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          en_brob_display2, this);
}
