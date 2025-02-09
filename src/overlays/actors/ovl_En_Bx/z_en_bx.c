/*
 * File: z_en_bx.c
 * Overlay: ovl_En_Bx
 * Description: Jabu-Jabu Electrified Tentacle
 */

#include "z_en_bx.h"
#include "assets/objects/object_bxa/object_bxa.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_bx_actor_ct(Actor* thisx, PlayState* play);
void En_bx_actor_dt(Actor* thisx, PlayState* play);
void En_bx_actor_move(Actor* thisx, PlayState* play);
void En_bx_actor_draw(Actor* thisx, PlayState* play);

ActorProfile En_Bx_Profile = {
    /**/ ACTOR_EN_BX,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BXA,
    /**/ sizeof(EnBx),
    /**/ En_bx_actor_ct,
    /**/ En_bx_actor_dt,
    /**/ En_bx_actor_move,
    /**/ En_bx_actor_draw,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0xFFCFFFFF, 0x03, 0x04 },
        { 0xFFCFFFFF, 0x01, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 60, 100, 100, { 0, 0, 0 } },
};

static ColliderQuadInit AtInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xFFCFFFFF, 0x03, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

void En_bx_actor_ct(Actor* thisx, PlayState* play) {
    EnBx* this = (EnBx*)thisx;
    Vec3f sp48 = { 0.015f, 0.015f, 0.015f };
    Vec3f sp3C = { 0.0f, 0.0f, 0.0f };
    static InitChainEntry value_init[] = {
        ICHAIN_F32(lockOnArrowOffset, 5300, ICHAIN_STOP),
    };
    s32 i;
    s32 pad;

    ValueSet_process(&this->actor, value_init);
    thisx->scale.x = thisx->scale.z = 0.01f;
    thisx->scale.y = 0.03f;

    thisx->world.pos.y -= 100.0f;

    for (i = 0; i < 4; i++) {
        this->unk_184[i] = sp48;
        if (i == 0) {
            this->unk_1B4[i].x = thisx->shape.rot.x - 0x4000;
        }
        this->unk_154[i] = thisx->world.pos;
        this->unk_154[i].y = thisx->world.pos.y + ((i + 1) * 140.0f);
    }

    Shape_Info_init(&thisx->shape, 0.0f, Actor_shadow_circle, 48.0f);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &OcInfoData);
    ClObjSwrd_ct(play, &this->colliderQuad);
    ClObjSwrd_set5(play, &this->colliderQuad, &this->actor, &AtInfoData);
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    this->unk_14C = 0;
    thisx->cullingVolumeDownward = 2000.0f;
    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(thisx->params, 8, 8))) {
        Actor_delete(&this->actor);
    }
    thisx->params &= 0xFF;
}

void En_bx_actor_dt(Actor* thisx, PlayState* play) {
    EnBx* this = (EnBx*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void en_bx_attack_proc(Actor* thisx, PlayState* play) {
    Vec3f sp5C = { 8000.0f, 15000.0f, 2500.0f };
    Vec3f sp50 = { 8000.0f, 10000.0f, 2500.0f };
    static Vec3f sword_top = { -8000.0f, 15000.0f, 2500.0f };
    static Vec3f sword_root = { -8000.0f, 10000.0f, 2500.0f };
    Vec3f sp44;
    Vec3f sp38;
    EnBx* this = (EnBx*)thisx;

    Matrix_Position(&sword_top, &sp44);
    Matrix_Position(&sword_root, &sp38);
    Matrix_Position(&sp5C, &this->colliderQuad.dim.quad[1]);
    Matrix_Position(&sp50, &this->colliderQuad.dim.quad[0]);
    CollisionCheck_Uty_setSword4Pos(&this->colliderQuad, &sp38, &sp44, &this->colliderQuad.dim.quad[0],
                             &this->colliderQuad.dim.quad[1]);
}

void En_bx_actor_move(Actor* thisx, PlayState* play) {
    EnBx* this = (EnBx*)thisx;
    Player* player = GET_PLAYER(play);
    s32 i;
    s16 tmp32;
    s32 tmp33;

    if ((thisx->xzDistToPlayer <= 70.0f) || (this->collider.base.atFlags & AT_HIT) ||
        (this->collider.base.acFlags & AC_HIT) || (this->colliderQuad.base.atFlags & AT_HIT)) {
        if ((thisx->xzDistToPlayer <= 70.0f) || (&player->actor == this->collider.base.at) ||
            (&player->actor == this->collider.base.ac) || (&player->actor == this->colliderQuad.base.at)) {
            tmp33 = player->invincibilityTimer & 0xFF;
            tmp32 = thisx->world.rot.y;
            if (!PARAMS_GET_NOSHIFT(thisx->params, 7, 1)) {
                tmp32 = thisx->yawTowardsPlayer;
            }
            if ((&player->actor != this->collider.base.at) && (&player->actor != this->collider.base.ac) &&
                (&player->actor != this->colliderQuad.base.at) && (player->invincibilityTimer <= 0)) {
                if (player->invincibilityTimer <= -40) {
                    player->invincibilityTimer = 0;
                } else {
                    player->invincibilityTimer = 0;
                    play->damagePlayer(play, -4);
                }
            }
            Actor_player_power_damage_set(play, &this->actor, 6.0f, tmp32, 6.0f);
            player->invincibilityTimer = tmp33;
        }

        this->collider.base.atFlags &= ~AT_HIT;
        this->collider.base.acFlags &= ~AC_HIT;
        this->colliderQuad.base.atFlags &= ~AT_HIT;
        this->colliderQuad.base.at = NULL;
        this->collider.base.ac = NULL;
        this->collider.base.at = NULL;
        this->unk_14C = 0x14;
    }

    if (this->unk_14C != 0) {
        this->unk_14C--;
        for (i = 0; i < 4; i++) {
            if (!((this->unk_14C + (i << 1)) % 4)) {
                static Color_RGBA8 lightning_prim = { 255, 255, 255, 255 };
                static Color_RGBA8 lightning_env = { 200, 255, 255, 255 };
                Vec3f pos;
                s16 yaw;

                yaw = (s32)rnd_fx(12288.0f);
                yaw = (yaw + (i * 0x4000)) + 0x2000;
                pos.x = rnd_fx(5.0f) + thisx->world.pos.x;
                pos.y = rnd_fx(30.0f) + thisx->world.pos.y + 170.0f;
                pos.z = rnd_fx(5.0f) + thisx->world.pos.z;
                Effect_SS_Lightning_sc_cl_co_ct(play, &pos, &lightning_prim, &lightning_env, 230, yaw, 6, 0);
            }
        }

        Actor_SE_set(thisx, NA_SE_EN_BIRI_SPARK - SFX_FLAG);
    }
    thisx->focus.pos = thisx->world.pos;
    CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_setAT(play, &play->colChkCtx, &this->collider.base);
    if (PARAMS_GET_NOSHIFT(thisx->params, 7, 1)) {
        CollisionCheck_setAT(play, &play->colChkCtx, &this->colliderQuad.base);
    }
}

void En_bx_actor_draw(Actor* thisx, PlayState* play) {
    static void* txt_dat[] = {
        object_bxa_Tex_0024F0,
        object_bxa_Tex_0027F0,
        object_bxa_Tex_0029F0,
    };
    EnBx* this = (EnBx*)thisx;
    s32 pad;
    Mtx* mtx = GRAPH_ALLOC(play->state.gfxCtx, 4 * sizeof(Mtx));
    s16 i;
    s16 off;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_bx.c", 464);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0C, mtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(txt_dat[PARAMS_GET_U(this->actor.params, 0, 7)]));
    gSPSegment(POLY_OPA_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 16, 16, 1, 0,
                                (play->gameplayFrames * -10) % 128, 32, 32));
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_bx.c", 478);

    if (PARAMS_GET_NOSHIFT(this->actor.params, 7, 1)) {
        en_bx_attack_proc(&this->actor, play);
    }

    this->unk_14E -= 0xBB8;
    thisx->scale.z = thisx->scale.x = (cos_s(this->unk_14E) * 0.0075f) + 0.015f;

    for (i = 3; i >= 0; i--) {
        off = (0x2000 * i);

        this->unk_184[i].z = this->unk_184[i].x = (cos_s(this->unk_14E + off) * 0.0075f) + 0.015f;
        this->unk_1B4[i].x = thisx->shape.rot.x;
        this->unk_1B4[i].y = thisx->shape.rot.y;
        this->unk_1B4[i].z = thisx->shape.rot.z;
    }

    for (i = 0; i < 4; i++, mtx++) {
        Matrix_translate(this->unk_154[i].x, this->unk_154[i].y, this->unk_154[i].z, MTXMODE_NEW);
        Matrix_rotateXYZ(this->unk_1B4[i].x, this->unk_1B4[i].y, this->unk_1B4[i].z, MTXMODE_APPLY);
        Matrix_scale(this->unk_184[i].x, this->unk_184[i].y, this->unk_184[i].z, MTXMODE_APPLY);
        MATRIX_TO_MTX(mtx, "../z_en_bx.c", 507);
    }

    gSPDisplayList(POLY_OPA_DISP++, object_bxa_DL_0022F0);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_bx.c", 511);
}
