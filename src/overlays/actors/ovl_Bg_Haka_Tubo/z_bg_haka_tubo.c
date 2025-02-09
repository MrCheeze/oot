/*
 * File: z_bg_haka_tubo.c
 * Overlay: ovl_Bg_Haka_Tubo
 * Description: Shadow Temple Giant Skull Jar
 */

#include "z_bg_haka_tubo.h"

#include "libc64/qrand.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "ichain.h"
#include "sfx.h"
#include "sys_matrix.h"
#include "z_lib.h"
#include "z64effect.h"
#include "z64play.h"

#include "global.h"

#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_haka_objects/object_haka_objects.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Bg_Haka_Tubo_actor_ct(Actor* thisx, PlayState* play);
void Bg_Haka_Tubo_actor_dt(Actor* thisx, PlayState* play);
void Bg_Haka_Tubo_actor_move(Actor* thisx, PlayState* play);
void Bg_Haka_Tubo_actor_draw(Actor* thisx, PlayState* play);

static void mode_wait(BgHakaTubo* this, PlayState* play);
static void mode_delete(BgHakaTubo* this, PlayState* play);

ActorProfile Bg_Haka_Tubo_Profile = {
    /**/ ACTOR_BG_HAKA_TUBO,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKA_OBJECTS,
    /**/ sizeof(BgHakaTubo),
    /**/ Bg_Haka_Tubo_actor_ct,
    /**/ Bg_Haka_Tubo_actor_dt,
    /**/ Bg_Haka_Tubo_actor_move,
    /**/ Bg_Haka_Tubo_actor_draw,
};

static ColliderCylinderInit HakaTuboAcPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 25, 60, 30, { 0, 0, 0 } },
};

static ColliderCylinderInit HakaTuboAtOcPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 60, 45, 235, { 0, 0, 0 } },
};

static s32 break_cnt = 0;

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Bg_Haka_Tubo_actor_ct(Actor* thisx, PlayState* play) {
    BgHakaTubo* this = (BgHakaTubo*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    ValueSet_process(&this->dyna.actor, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS | DYNA_TRANSFORM_ROT_Y);
    DynaPolyUty_bgdi_SG2KSG(&object_haka_objects_Col_0108B8, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    ClObjPipe_ct(play, &this->potCollider);
    ClObjPipe_set5(play, &this->potCollider, &this->dyna.actor, &HakaTuboAcPipeData);
    ClObjPipe_ct(play, &this->flamesCollider);
    ClObjPipe_set5(play, &this->flamesCollider, &this->dyna.actor, &HakaTuboAtOcPipeData);
    this->fireScroll = fqrand() * 15.0f;
    break_cnt = 0;
    this->actionFunc = mode_wait;
}

void Bg_Haka_Tubo_actor_dt(Actor* thisx, PlayState* play) {
    BgHakaTubo* this = (BgHakaTubo*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    ClObjPipe_dt(play, &this->potCollider);
    ClObjPipe_dt(play, &this->flamesCollider);
}

static void mode_wait(BgHakaTubo* this, PlayState* play) {
    static Vec3f zero_vec = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;

    if (this->dyna.actor.room == 12) { // 3 spinning pots room in Shadow Temple
        this->dyna.actor.shape.rot.y += 0x180;
        this->dyna.actor.world.pos.x = sin_s(this->dyna.actor.shape.rot.y - 0x4000) * 145.0f + -5559.0f;
        this->dyna.actor.world.pos.z = cos_s(this->dyna.actor.shape.rot.y - 0x4000) * 145.0f + -1587.0f;
    }
    // Colliding with flame circle
    if (this->flamesCollider.base.atFlags & AT_HIT) {
        this->flamesCollider.base.atFlags &= ~AT_HIT;
        Actor_player_power_damage_set(play, &this->dyna.actor, 5.0f, this->dyna.actor.yawTowardsPlayer, 5.0f);
    }
    // Colliding with collider inside the pot
    if (this->potCollider.base.acFlags & AC_HIT) {
        this->potCollider.base.acFlags &= ~AC_HIT;
        // If the colliding actor is within a 50 unit radius and 50 unit height cylinder centered
        // on the actor's position, break the pot
        if (Actor_search_position_distanceXZ(&this->dyna.actor, &this->potCollider.base.ac->world.pos) < 50.0f &&
            (this->potCollider.base.ac->world.pos.y - this->dyna.actor.world.pos.y) < 50.0f) {
            pos.x = this->dyna.actor.world.pos.x;
            pos.z = this->dyna.actor.world.pos.z;
            pos.y = this->dyna.actor.world.pos.y + 80.0f;
            Effect_SS_Bomb2_2_ct(play, &pos, &zero_vec, &zero_vec, 100, 45);
            Effect_SE_Info_new(play, &this->dyna.actor.world.pos, 50, NA_SE_EV_BOX_BREAK);
            Effect_Hahen_Kakusan_ct3(play, &pos, 20.0f, 0, 350, 100, 50, OBJECT_HAKA_OBJECTS, 40, gEffFragments2DL);
            this->dropTimer = 5;
            this->dyna.actor.draw = NULL;
            Actor_set_scale(&this->dyna.actor, 0.0f);
            this->actionFunc = mode_delete;
        }
    } else {
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->flamesCollider);
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->potCollider);
        CollisionCheck_setAC(play, &play->colChkCtx, &this->potCollider.base);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->flamesCollider.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->flamesCollider.base);
    }
}

static void mode_delete(BgHakaTubo* this, PlayState* play) {
    EnItem00* collectible;
    f32 rnd;
    Vec3f spawnPos;
    s32 i;
    s32 collectibleParams;

    this->dropTimer--;
    if (this->dropTimer == 0) { // Creates a delay between destroying the pot and dropping the collectible
        spawnPos.x = this->dyna.actor.world.pos.x;
        spawnPos.y = this->dyna.actor.world.pos.y + 200.0f;
        spawnPos.z = this->dyna.actor.world.pos.z;
        if (this->dyna.actor.room == 12) { // 3 spinning pots room in Shadow Temple
            rnd = fqrand();
            break_cnt++;
            if (break_cnt == 3) {
                // All 3 pots destroyed
                collectibleParams = -1;
                Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
                // Drop rupees
                for (i = 0; i < 9; i++) {
                    collectible = Item_set0(
                        play, &spawnPos, (i % (ITEM00_RUPEE_RED - ITEM00_RUPEE_GREEN + 1)) + ITEM00_RUPEE_GREEN);
                    if (collectible != NULL) {
                        collectible->actor.velocity.y = 15.0f;
                        collectible->actor.world.rot.y = this->dyna.actor.shape.rot.y + (i * 0x1C71);
                    }
                }
            } else if (rnd < 0.2f) {
                // Unlucky, no reward and spawn keese
                collectibleParams = -1;
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_FIREFLY, this->dyna.actor.world.pos.x,
                            this->dyna.actor.world.pos.y + 80.0f, this->dyna.actor.world.pos.z, 0,
                            this->dyna.actor.shape.rot.y, 0, 2);
                Na_StartSystemSe_F(NA_SE_SY_ERROR);
            } else {
                // Random rewards
                if (rnd < 0.4f) {
                    collectibleParams = ITEM00_BOMBS_A;
                } else if (rnd < 0.6f) {
                    collectibleParams = ITEM00_MAGIC_LARGE;
                } else if (rnd < 0.8f) {
                    collectibleParams = ITEM00_MAGIC_SMALL;
                } else {
                    collectibleParams = ITEM00_ARROWS_SMALL;
                }
                Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
            }
        } else if (Actor_Environment_item_Check(play, this->dyna.actor.params) != 0) {
            // If small key already collected, drop recovery heart instead
            collectibleParams = ITEM00_RECOVERY_HEART;
            Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
        } else {
            // Drops a small key and sets a collect flag
            collectibleParams = (PARAMS_GET_U(this->dyna.actor.params, 0, 6) << 8) | ITEM00_SMALL_KEY;
            Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        }
        if (collectibleParams != -1) {
            collectible = Item_set0(play, &spawnPos, collectibleParams);
            if (collectible != NULL) {
                collectible->actor.velocity.y = 15.0f;
                collectible->actor.world.rot.y = this->dyna.actor.shape.rot.y;
            }
        }
        Actor_delete(&this->dyna.actor);
    }
}

void Bg_Haka_Tubo_actor_move(Actor* thisx, PlayState* play) {
    BgHakaTubo* this = (BgHakaTubo*)thisx;

    this->actionFunc(this, play);
    this->fireScroll++;
}

static void draw_fire(BgHakaTubo* this, PlayState* play) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_bg_haka_tubo.c", 476);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Matrix_translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y + 235.0f, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_rotateY(BINANG_TO_RAD(this->dyna.actor.shape.rot.y), MTXMODE_APPLY);
    Matrix_scale(0.07f, 0.04f, 0.07f, MTXMODE_APPLY);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 0, 170, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 255, 255);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, this->fireScroll & 127, 0, 32, 64, 1, 0,
                                (this->fireScroll * -15) & 0xFF, 32, 64));
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_bg_haka_tubo.c", 497);
    gSPDisplayList(POLY_XLU_DISP++, gEffFireCircleDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_bg_haka_tubo.c", 501);
}

void Bg_Haka_Tubo_actor_draw(Actor* thisx, PlayState* play) {
    BgHakaTubo* this = (BgHakaTubo*)thisx;

    Cheap_gfx_display(play, object_haka_objects_DL_00FE40);
    draw_fire(this, play);
}
