/*
 * File: z_magic_dark.c
 * Overlay: ovl_Magic_Dark
 * Description: Nayru's Love
 */

#include "z_magic_dark.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void Magic_Dark_Actor_ct(Actor* thisx, PlayState* play);
void Magic_Dark_Actor_dt(Actor* thisx, PlayState* play);
void Magic_Dark_Actor_move(Actor* thisx, PlayState* play);
void Magic_Dark_Actor_draw_ball(Actor* thisx, PlayState* play);
void Magic_Dark_Actor_move2(Actor* thisx, PlayState* play);
void Magic_Dark_Actor_draw(Actor* thisx, PlayState* play);

void set_add_magic_dark_lighting(PlayState* play, f32 intensity);

ActorProfile Magic_Dark_Profile = {
    /**/ ACTOR_MAGIC_DARK,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(MagicDark),
    /**/ Magic_Dark_Actor_ct,
    /**/ Magic_Dark_Actor_dt,
    /**/ Magic_Dark_Actor_move,
    /**/ Magic_Dark_Actor_draw_ball,
};

#include "assets/overlays/ovl_Magic_Dark/z_magic_dark.c"

// unused
static Color_RGBA8 color_table[] = { { 50, 100, 150, 200 }, { 255, 200, 150, 100 } };

void Magic_Dark_Actor_ct(Actor* thisx, PlayState* play) {
    MagicDark* this = (MagicDark*)thisx;
    Player* player = GET_PLAYER(play);

    if (!LINK_IS_ADULT) {
        this->scale = 0.4f;
    } else {
        this->scale = 0.6f;
    }

    thisx->world.pos = player->actor.world.pos;
    Actor_set_scale(&this->actor, 0.0f);
    thisx->room = -1;

    if (z_common_data.nayrusLoveTimer != 0) {
        thisx->update = Magic_Dark_Actor_move2;
        thisx->draw = Magic_Dark_Actor_draw;
        thisx->scale.x = thisx->scale.z = this->scale * 1.6f;
        thisx->scale.y = this->scale * 0.8f;
        this->timer = 0;
        this->primAlpha = 0;
    } else {
        this->timer = 0;
        z_common_data.nayrusLoveTimer = 0;
    }
}

void Magic_Dark_Actor_dt(Actor* thisx, PlayState* play) {
    if (z_common_data.nayrusLoveTimer == 0) {
        magic_cancel_check(play);
    }
}

void Magic_Dark_Actor_move2(Actor* thisx, PlayState* play) {
    MagicDark* this = (MagicDark*)thisx;
    u8 phi_a0;
    Player* player = GET_PLAYER(play);
    s16 pad;
    s16 nayrusLoveTimer = z_common_data.nayrusLoveTimer;
    s32 msgMode = play->msgCtx.msgMode;

    // See `ACTOROVL_ALLOC_ABSOLUTE`
    //! @bug This condition is too broad, the actor will also be killed by warp songs. But warp songs do not use an
    //! actor which uses `ACTOROVL_ALLOC_ABSOLUTE`. There is no reason to kill the actor in this case.
    //! This happens with all magic effects actors, but is especially visible with Nayru's Love as it lasts longer than
    //! other magic actors, and the Nayru's Love actor is supposed to be spawned back after ocarina effects actors are
    //! done. But with warp songs, whether the player warps away or not, the actor won't be spawned back.
    if ((msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK) || (msgMode == MSGMODE_SONG_PLAYED)) {
        Actor_delete(thisx);
        return;
    }

    if (nayrusLoveTimer >= 1200) {
        if (1) {}

        player->invincibilityTimer = 0;
        z_common_data.nayrusLoveTimer = 0;
        Actor_delete(thisx);
        return;
    }

    player->invincibilityTimer = -100;
    thisx->scale.x = thisx->scale.z = this->scale;

    if (this->timer < 20) {
        thisx->scale.x = thisx->scale.z = (1.6f - (this->timer * 0.03f)) * this->scale;
        thisx->scale.y = ((this->timer * 0.01f) + 0.8f) * this->scale;
    } else {
        thisx->scale.x = thisx->scale.z = this->scale;
        thisx->scale.y = this->scale;
    }

    thisx->scale.x *= 1.3f;
    thisx->scale.z *= 1.3f;

    phi_a0 = (this->timer < 20) ? (this->timer * 12) : 255;

    if (nayrusLoveTimer >= 1180) {
        this->primAlpha = 15595 - (nayrusLoveTimer * 13);
        if (nayrusLoveTimer & 1) {
            this->primAlpha = this->primAlpha >> 1;
        }
    } else if (nayrusLoveTimer >= 1100) {
        this->primAlpha = (u8)(nayrusLoveTimer << 7) + 127;
    } else {
        this->primAlpha = 255;
    }

    if (this->primAlpha > phi_a0) {
        this->primAlpha = phi_a0;
    }

    thisx->world.rot.y += 0x3E8;
    thisx->shape.rot.y = thisx->world.rot.y + getRealCameraAngleY(GET_ACTIVE_CAM(play));
    this->timer++;
    z_common_data.nayrusLoveTimer = nayrusLoveTimer + 1;

    if (nayrusLoveTimer < 1100) {
        Actor_level_SE_set(thisx, NA_SE_PL_MAGIC_SOUL_NORMAL - SFX_FLAG);
    } else {
        Actor_level_SE_set(thisx, NA_SE_PL_MAGIC_SOUL_FLASH - SFX_FLAG);
    }
}

void set_add_magic_dark_lighting(PlayState* play, f32 intensity) {
    s32 i;
    f32 colorScale;
    f32 fogScale;

    if (play->roomCtx.curRoom.type != ROOM_TYPE_BOSS) {
        intensity = CLAMP_MIN(intensity, 0.0f);
        intensity = CLAMP_MAX(intensity, 1.0f);
        fogScale = intensity - 0.2f;

        if (intensity < 0.2f) {
            fogScale = 0.0f;
        }

        play->envCtx.adjFogNear = (850.0f - play->envCtx.lightSettings.fogNear) * fogScale;

        if (intensity == 0.0f) {
            for (i = 0; i < ARRAY_COUNT(play->envCtx.adjFogColor); i++) {
                play->envCtx.adjFogColor[i] = 0;
            }
        } else {
            colorScale = intensity * 5.0f;

            if (colorScale > 1.0f) {
                colorScale = 1.0f;
            }

            for (i = 0; i < ARRAY_COUNT(play->envCtx.adjFogColor); i++) {
                play->envCtx.adjFogColor[i] = -(s16)(play->envCtx.lightSettings.fogColor[i] * colorScale);
            }
        }
    }
}

void Magic_Dark_Actor_move(Actor* thisx, PlayState* play) {
    MagicDark* this = (MagicDark*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    Actor_level_SE_set(&this->actor, NA_SE_PL_MAGIC_SOUL_BALL - SFX_FLAG);
    if (this->timer < 35) {
        set_add_magic_dark_lighting(play, this->timer * (1 / 45.0f));
        add_calc(&thisx->scale.x, this->scale * (1 / 12.000001f), 0.05f, 0.01f, 0.0001f);
        Actor_set_scale(&this->actor, thisx->scale.x);
    } else if (this->timer < 55) {
        Actor_set_scale(&this->actor, thisx->scale.x * 0.9f);
        add_calc(&this->orbOffset.y, player->bodyPartsPos[PLAYER_BODYPART_WAIST].y, 0.5f, 3.0f, 1.0f);
        if (this->timer > 48) {
            set_add_magic_dark_lighting(play, (54 - this->timer) * 0.2f);
        }
    } else {
        thisx->update = Magic_Dark_Actor_move2;
        thisx->draw = Magic_Dark_Actor_draw;
        thisx->scale.x = thisx->scale.z = this->scale * 1.6f;
        thisx->scale.y = this->scale * 0.8f;
        this->timer = 0;
        this->primAlpha = 0;
    }

    this->timer++;
}

void Magic_Dark_Actor_draw(Actor* thisx, PlayState* play) {
    MagicDark* this = (MagicDark*)thisx;
    s32 pad;
    u16 gameplayFrames = play->gameplayFrames;

    OPEN_DISPS(play->state.gfxCtx, "../z_magic_dark.c", 525);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);

    {
        Player* player = GET_PLAYER(play);
        f32 heightDiff;

        this->actor.world.pos.x = player->bodyPartsPos[PLAYER_BODYPART_WAIST].x;
        this->actor.world.pos.z = player->bodyPartsPos[PLAYER_BODYPART_WAIST].z;
        heightDiff = player->bodyPartsPos[PLAYER_BODYPART_WAIST].y - this->actor.world.pos.y;
        if (heightDiff < -2.0f) {
            this->actor.world.pos.y = player->bodyPartsPos[PLAYER_BODYPART_WAIST].y + 2.0f;
        } else if (heightDiff > 2.0f) {
            this->actor.world.pos.y = player->bodyPartsPos[PLAYER_BODYPART_WAIST].y - 2.0f;
        }
        Matrix_translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
        Matrix_rotateY(BINANG_TO_RAD(this->actor.shape.rot.y), MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_magic_dark.c", 553);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 170, 255, 255, (s32)(this->primAlpha * 0.6f) & 0xFF);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 100, 255, 128);
        gSPDisplayList(POLY_XLU_DISP++, m_shield_2_modelT);
        gSPDisplayList(POLY_XLU_DISP++,
                       two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, gameplayFrames * 2, gameplayFrames * -4,
                                        32, 32, 1, 0, gameplayFrames * -16, 64, 32));
        gSPDisplayList(POLY_XLU_DISP++, m_shield_2_modelT2);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_magic_dark.c", 570);
}

void Magic_Dark_Actor_draw_ball(Actor* thisx, PlayState* play) {
    PlayState* play2 = (PlayState*)play;
    Vec3f pos;
    Player* player = GET_PLAYER(play2);
    MagicDark* this = (MagicDark*)thisx;
    f32 sp6C = play->state.frames & 0x1F;

    if (this->timer < 32) {
        pos.x =
            (player->bodyPartsPos[PLAYER_BODYPART_L_HAND].x + player->bodyPartsPos[PLAYER_BODYPART_R_HAND].x) * 0.5f;
        pos.y =
            (player->bodyPartsPos[PLAYER_BODYPART_L_HAND].y + player->bodyPartsPos[PLAYER_BODYPART_R_HAND].y) * 0.5f;
        pos.z =
            (player->bodyPartsPos[PLAYER_BODYPART_L_HAND].z + player->bodyPartsPos[PLAYER_BODYPART_R_HAND].z) * 0.5f;
        if (this->timer > 20) {
            pos.y += (this->timer - 20) * 1.4f;
        }
        this->orbOffset = pos;
    } else if (this->timer < 130) {
        pos = this->orbOffset;
    } else {
        return;
    }

    pos.x -= (this->actor.scale.x * 300.0f * sin_s(getRealCameraAngleY(GET_ACTIVE_CAM(play2))) *
              cos_s(getRealCameraAngleX(GET_ACTIVE_CAM(play2))));
    pos.y -= (this->actor.scale.x * 300.0f * sin_s(getRealCameraAngleX(GET_ACTIVE_CAM(play2))));
    pos.z -= (this->actor.scale.x * 300.0f * cos_s(getRealCameraAngleY(GET_ACTIVE_CAM(play2))) *
              cos_s(getRealCameraAngleX(GET_ACTIVE_CAM(play2))));

    OPEN_DISPS(play->state.gfxCtx, "../z_magic_dark.c", 619);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 150, 255, 255);
    Matrix_translate(pos.x, pos.y, pos.z, MTXMODE_NEW);
    Matrix_scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    Matrix_mult(&play2->billboardMtxF, MTXMODE_APPLY);
    Matrix_push();
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_magic_dark.c", 632);
    Matrix_rotateZ(sp6C * (M_PI / 32), MTXMODE_APPLY);
    gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);
    Matrix_pull();
    Matrix_rotateZ(-sp6C * (M_PI / 32), MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_magic_dark.c", 639);
    gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_magic_dark.c", 643);
}
