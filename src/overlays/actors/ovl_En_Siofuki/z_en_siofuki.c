/*
 * File: z_en_siofuki.c
 * Overlay: ovl_En_Siofuki
 * Description: Water Spout
 */

#include "z_en_siofuki.h"
#include "assets/objects/object_siofuki/object_siofuki.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void En_Siofuki_actor_ct(Actor* thisx, PlayState* play);
void En_Siofuki_actor_dt(Actor* thisx, PlayState* play);
void En_Siofuki_actor_move(Actor* thisx, PlayState* play);
void En_Siofuki_actor_draw(Actor* thisx, PlayState* play);

void mode_wait_normal(EnSiofuki* this, PlayState* play);
void mode_wait_tbox_open(EnSiofuki* this, PlayState* play);
void mode_wait_tbox_noopen(EnSiofuki* this, PlayState* play);

ActorProfile En_Siofuki_Profile = {
    /**/ ACTOR_EN_SIOFUKI,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SIOFUKI,
    /**/ sizeof(EnSiofuki),
    /**/ En_Siofuki_actor_ct,
    /**/ En_Siofuki_actor_dt,
    /**/ En_Siofuki_actor_move,
    /**/ En_Siofuki_actor_draw,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void En_Siofuki_actor_ct(Actor* thisx, PlayState* play) {
    EnSiofuki* this = (EnSiofuki*)thisx;
    s32 type;
    CollisionHeader* colHeader = NULL;
    s32 pad;

    if ((thisx->room == 10) && Actor_Environment_sw_Check(play, 0x1E)) {
        Actor_delete(thisx);
        return;
    }

    ValueSet_process(thisx, value_init);
    MoveBG_ct(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyUty_bgdi_SG2KSG(&object_siofuki_Col_000D78, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
    this->sfxFlags |= 1;

    type = PARAMS_GET_U((u16)thisx->params, 12, 4);
    if (!((type == 0) || (type == 1))) {
        Actor_delete(thisx);
        return;
    }

    this->initPosY = thisx->world.pos.y;
    this->unk_174 = 35.0f;
    this->unk_170 = -6058.0f + this->unk_174;

    if (thisx->shape.rot.x != 0) {
        this->maxHeight = thisx->shape.rot.x * 40.0f;
        this->currentHeight = this->maxHeight;
    }
    this->activeTime = 0;
    if (thisx->shape.rot.y != 0) {
        this->activeTime = thisx->shape.rot.y;
    }
    if (thisx->shape.rot.z != 0) {
        thisx->scale.x = thisx->shape.rot.z * (1.0f / 1.73f) * 0.1f;
        thisx->scale.z = thisx->shape.rot.z * 0.5f * 0.1f;
    }

    thisx->world.rot.x = 0;
    thisx->world.rot.y = 0;
    thisx->world.rot.z = 0;
    thisx->shape.rot.x = 0;
    thisx->shape.rot.y = 0;
    thisx->shape.rot.z = 0;

    type = PARAMS_GET_U((u16)thisx->params, 12, 4);
    if (type == EN_SIOFUKI_RAISING) {
        this->currentHeight = 10.0f;
        this->targetHeight = 10.0f;
        this->actionFunc = mode_wait_normal;
    } else if (type == EN_SIOFUKI_LOWERING) {
        if (Actor_Environment_Tbox_Check(play, PARAMS_GET_U((u16)thisx->params, 0, 6))) {
            this->currentHeight = -45.0f;
            this->targetHeight = -45.0f;
            this->actionFunc = mode_wait_tbox_open;
        } else {
            this->targetHeight = this->currentHeight;
            this->actionFunc = mode_wait_tbox_noopen;
        }
    }
}

void En_Siofuki_actor_dt(Actor* thisx, PlayState* play) {
    EnSiofuki* this = (EnSiofuki*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_pos_set(EnSiofuki* this, PlayState* play) {
    this->oscillation = sinf((play->gameplayFrames & 0x1F) / 32.0f * M_PI * 2.0f) * 4.0f;
    this->unk_170 = this->unk_174 * 10.0f + -6058.0f - this->oscillation * 10.0f;
    this->unk_174 = 35.0f;
    this->dyna.actor.world.pos.y = this->initPosY + this->currentHeight + this->oscillation;
}

void func_player_touch(EnSiofuki* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 dX;
    f32 dY;
    f32 dZ;
    s16 angle;
    s16 dAngle;

    dX = player->actor.world.pos.x - this->dyna.actor.world.pos.x;
    dY = player->actor.world.pos.y - this->dyna.actor.world.pos.y;
    dZ = player->actor.world.pos.z - this->dyna.actor.world.pos.z;

    if ((dX > (this->dyna.actor.scale.x * -346.0f)) && (dX < (this->dyna.actor.scale.x * 346.0f)) &&
        (dZ > (this->dyna.actor.scale.z * -400.0f)) && (dZ < (this->dyna.actor.scale.z * 400.0f)) && (dY < 0.0f)) {
        if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
            if (this->splashTimer <= 0) {
                Effect_SS_G_Splash_sc_cl_ct(play, &player->actor.world.pos, NULL, NULL, 1, 1);
                this->splashTimer = 10;
            } else {
                this->splashTimer--;
            }

            this->applySpeed = false;
            this->appliedSpeed = 0.0f;
            this->targetAppliedSpeed = 0.0f;
        } else {
            f32 dist2d = sqrtf(SQ(dX) + SQ(dZ));

            this->applySpeed = true;
            this->splashTimer = 0;
            angle = RAD_TO_BINANG(fatan2(dX, dZ));
            dAngle = (player->actor.world.rot.y ^ 0x8000) - angle;
            player->actor.gravity = 0.0f;
            player->actor.velocity.y = 0.0f;
            add_calc(&player->actor.world.pos.y, this->dyna.actor.world.pos.y, 0.5f, 4.0f, 1.0f);

            if ((dAngle < 0x4000) && (dAngle > -0x4000)) {
                f32 speedScale;

                this->appliedYaw = player->actor.world.rot.y ^ 0x8000;
                speedScale = dist2d / (this->dyna.actor.scale.x * 40.0f * 10.0f);
                speedScale = CLAMP_MIN(speedScale, 0.0f);
                speedScale = CLAMP_MAX(speedScale, 1.0f);
                player->speedXZ *= speedScale;
                add_calc2(&this->targetAppliedSpeed, 3.0f, 1.0f, 1.0f);
                add_calc2(&this->appliedSpeed, this->targetAppliedSpeed, 1.0f, 0.3f * speedScale);
            } else {
                this->appliedYaw = player->actor.world.rot.y;
                player->speedXZ /= 2.0f;
                add_calc2(&this->targetAppliedSpeed, 3.0f, 1.0f, 1.0f);
                add_calc2(&this->appliedSpeed, this->targetAppliedSpeed, 1.0f, 0.1f);
            }

            player->pushedYaw = this->appliedYaw;
            player->pushedSpeed = this->appliedSpeed;
        }
    } else {
        if (this->applySpeed) {
            player->speedXZ = this->appliedSpeed + player->speedXZ;
            player->yaw = this->appliedYaw;
        }

        this->targetAppliedSpeed = 0.0f;
        this->appliedSpeed = 0.0f;
        this->applySpeed = false;
    }
}

void func_normal_move(EnSiofuki* this, PlayState* play) {
    add_calc(&this->currentHeight, this->targetHeight, 0.8f, 3.0f, 0.01f);
}

static void mode_move(EnSiofuki* this, PlayState* play) {
    func_pos_set(this, play);
    func_player_touch(this, play);
    func_normal_move(this, play);

    this->timer--;
    if (this->timer < 0) {
        Actor_Environment_sw_Off(play, PARAMS_GET_U((u16)this->dyna.actor.params, 6, 6));
        switch (PARAMS_GET_U((u16)this->dyna.actor.params, 12, 4)) {
            case EN_SIOFUKI_RAISING:
                this->targetHeight = 10.0f;
                this->actionFunc = mode_wait_normal;
                break;
            case EN_SIOFUKI_LOWERING:
                this->targetHeight = this->maxHeight;
                this->actionFunc = mode_wait_tbox_noopen;
                break;
        }
    } else {
        Actor_timer_level_SE_set(&this->dyna.actor, this->timer);
    }

    if ((PARAMS_GET_U((u16)this->dyna.actor.params, 12, 4) == EN_SIOFUKI_LOWERING) &&
        Actor_Environment_Tbox_Check(play, PARAMS_GET_U((u16)this->dyna.actor.params, 0, 6))) {
        this->currentHeight = -45.0f;
        this->targetHeight = -45.0f;
        Actor_Environment_sw_Off(play, PARAMS_GET_U((u16)this->dyna.actor.params, 6, 6));
        this->actionFunc = mode_wait_tbox_open;
    }
}

void mode_wait_normal(EnSiofuki* this, PlayState* play) {
    func_pos_set(this, play);
    func_player_touch(this, play);
    func_normal_move(this, play);

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U((u16)this->dyna.actor.params, 6, 6))) {
        this->targetHeight = 400.0f;
        this->timer = 300;
        this->actionFunc = mode_move;
    }
}

void mode_move_tbox_demo_wait(EnSiofuki* this, PlayState* play) {
    func_pos_set(this, play);
    func_player_touch(this, play);
    func_normal_move(this, play);

    this->timer--;
    if (this->timer < 0) {
        this->timer = this->activeTime * 20;
        this->targetHeight = -45.0f;
        this->actionFunc = mode_move;
    }

    if (Actor_Environment_Tbox_Check(play, PARAMS_GET_U((u16)this->dyna.actor.params, 0, 6))) {
        this->currentHeight = -45.0f;
        this->targetHeight = -45.0f;
        this->actionFunc = mode_wait_tbox_open;
    }
}

void mode_wait_tbox_noopen(EnSiofuki* this, PlayState* play) {
    func_pos_set(this, play);
    func_player_touch(this, play);
    func_normal_move(this, play);

    if (PARAMS_GET_U((u16)this->dyna.actor.params, 12, 4) == EN_SIOFUKI_LOWERING) {
        if (Actor_Environment_sw_Check(play, PARAMS_GET_U((u16)this->dyna.actor.params, 6, 6))) {
            this->timer = 20;
            this->actionFunc = mode_move_tbox_demo_wait;
            makeOnepointDemo(play, 5010, 40, &this->dyna.actor, CAM_ID_MAIN);
        }

        if (Actor_Environment_Tbox_Check(play, PARAMS_GET_U((u16)this->dyna.actor.params, 0, 6))) {
            this->currentHeight = -45.0f;
            this->targetHeight = -45.0f;
            this->actionFunc = mode_wait_tbox_open;
        }
    }
}

void mode_wait_tbox_open(EnSiofuki* this, PlayState* play) {
    func_pos_set(this, play);
    func_normal_move(this, play);
}

void En_Siofuki_actor_move(Actor* thisx, PlayState* play) {
    EnSiofuki* this = (EnSiofuki*)thisx;

    this->actionFunc(this, play);
}

void En_Siofuki_actor_draw(Actor* thisx, PlayState* play) {
    EnSiofuki* this = (EnSiofuki*)thisx;
    u32 x;
    u32 y;
    u32 gameplayFrames = play->gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_siofuki.c", 654);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Matrix_translate(0.0f, this->unk_170, 0.0f, MTXMODE_APPLY);
    Matrix_scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_en_siofuki.c", 662);
    x = gameplayFrames * 15;
    y = gameplayFrames * -15;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, x, y, 64, 64, 1, x, y, 64, 64));
    gSPDisplayList(POLY_XLU_DISP++, object_siofuki_DL_000B70);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_siofuki.c", 674);

    if (this->sfxFlags & 1) {
        f32 heightRatio;
        switch (PARAMS_GET_U((u16)thisx->params, 12, 4)) {
            case EN_SIOFUKI_RAISING:
                heightRatio = (this->currentHeight - 10.0f) / (400.0f - 10.0f);
                Na_SetMotorSe(&thisx->projectedPos, NA_SE_EV_FOUNTAIN - SFX_FLAG, 1.0f + heightRatio);
                break;
            case EN_SIOFUKI_LOWERING:
                if (this->currentHeight > -35.0f) {
                    heightRatio = (this->currentHeight - -35.0f) / (this->maxHeight - -35.0f);
                    Na_SetMotorSe(&thisx->projectedPos, NA_SE_EV_FOUNTAIN - SFX_FLAG, 1.0f + heightRatio);
                }
                break;
        }
    }
}
