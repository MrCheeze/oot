/*
 * File: z_magic_wind.c
 * Overlay: ovl_Magic_Wind
 * Description: Farore's Wind
 */

#include "z_magic_wind.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void Magic_Wind_Actor_ct(Actor* thisx, PlayState* play);
void Magic_Wind_Actor_dt(Actor* thisx, PlayState* play);
void Magic_Wind_Actor_move(Actor* thisx, PlayState* play);
void Magic_Wind_Actor_display(Actor* thisx, PlayState* play);

void move_process_in(MagicWind* this, PlayState* play);
void move_process_wait(MagicWind* this, PlayState* play);
void move_process_delete(MagicWind* this, PlayState* play);
void move_process_stand(MagicWind* this, PlayState* play);
void move_process_walk(MagicWind* this, PlayState* play);

ActorProfile Magic_Wind_Profile = {
    /**/ ACTOR_MAGIC_WIND,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(MagicWind),
    /**/ Magic_Wind_Actor_ct,
    /**/ Magic_Wind_Actor_dt,
    /**/ Magic_Wind_Actor_move,
    /**/ Magic_Wind_Actor_display,
};

#include "assets/overlays/ovl_Magic_Wind/z_magic_wind.c"

static u8 vtx_table[] = {
    0x00, 0x03, 0x04, 0x07, 0x09, 0x0A, 0x0D, 0x0F, 0x11, 0x12, 0x15, 0x16, 0x19, 0x1B, 0x1C, 0x1F, 0x21, 0x23,
};

void Magic_Wind_actor_set_process(MagicWind* this, MagicWindFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void Magic_Wind_Actor_ct(Actor* thisx, PlayState* play) {
    MagicWind* this = (MagicWind*)thisx;
    Player* player = GET_PLAYER(play);

    if (!FcSkeletonInfo_ct(play, &this->skelCurve, &new_magic_wind_mdl_info, &new_magic_wind_fcurve_data)) {
        // "Magic_Wind_Actor_ct (): Construct failed"
        PRINTF("Magic_Wind_Actor_ct():コンストラクト失敗\n");
    }
    this->actor.room = -1;
    switch (this->actor.params) {
        case 0:
            FcSkeletonInfo_init(&this->skelCurve, &new_magic_wind_fcurve_data, 0.0f, 60.0f, 0.0f, 1.0f);
            this->timer = 29;
            Magic_Wind_actor_set_process(this, move_process_wait);
            break;
        case 1:
            FcSkeletonInfo_init(&this->skelCurve, &new_magic_wind_fcurve_data, 60.0f, 0.0f, 60.0f, -1.0f);
            Magic_Wind_actor_set_process(this, move_process_in);
            // "Means start"
            LOG_STRING("表示開始", "../z_magic_wind.c", 486);
            player_SE_set(player, NA_SE_PL_MAGIC_WIND_WARP);
            break;
    }
}

void Magic_Wind_Actor_dt(Actor* thisx, PlayState* play) {
    MagicWind* this = (MagicWind*)thisx;
    FcSkeletonInfo_dt(play, &this->skelCurve);
    magic_cancel_check(play);
    // "wipe out"
    LOG_STRING("消滅", "../z_magic_wind.c", 505);
}

void set_alpha_vtx(f32 alpha) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(vtx_table); i++) {
        magic_wind_v[vtx_table[i]].n.a = alpha * 255.0f;
    }
}

void move_process_wait(MagicWind* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->timer > 0) {
        this->timer--;
        return;
    }

    // "Means start"
    LOG_STRING("表示開始", "../z_magic_wind.c", 539);
    player_SE_set(player, NA_SE_PL_MAGIC_WIND_NORMAL);
    set_alpha_vtx(1.0f);
    Magic_Wind_actor_set_process(this, move_process_walk);
    FcSkeletonInfo_play(play, &this->skelCurve);
}

void move_process_walk(MagicWind* this, PlayState* play) {
    if (FcSkeletonInfo_play(play, &this->skelCurve)) {
        Magic_Wind_actor_set_process(this, move_process_stand);
        this->timer = 50;
    }
}

void move_process_stand(MagicWind* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else {
        Magic_Wind_actor_set_process(this, move_process_delete);
        this->timer = 30;
    }
}

void move_process_delete(MagicWind* this, PlayState* play) {
    if (this->timer > 0) {
        set_alpha_vtx((f32)this->timer * (1.0f / 30.0f));
        this->timer--;
    } else {
        Actor_delete(&this->actor);
    }
}

void move_process_in(MagicWind* this, PlayState* play) {
    if (FcSkeletonInfo_play(play, &this->skelCurve)) {
        Actor_delete(&this->actor);
    }
}

void Magic_Wind_Actor_move(Actor* thisx, PlayState* play) {
    MagicWind* this = (MagicWind*)thisx;

    // See `ACTOROVL_ALLOC_ABSOLUTE`
    //! @bug This condition is too broad, the actor will also be killed by warp songs. But warp songs do not use an
    //! actor which uses `ACTOROVL_ALLOC_ABSOLUTE`. There is no reason to kill the actor in this case.
    if ((play->msgCtx.msgMode == MSGMODE_OCARINA_CORRECT_PLAYBACK) || (play->msgCtx.msgMode == MSGMODE_SONG_PLAYED)) {
        Actor_delete(thisx);
        return;
    }

    this->actionFunc(this, play);
}

s32 Magic_Wind_Actor_disp_proc(PlayState* play, SkelCurve* skelCurve, s32 limbIndex, void* thisx) {
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_magic_wind.c", 615);

    if (limbIndex == 1) {
        gSPSegment(POLY_XLU_DISP++, 8,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (play->state.frames * 9) & 0xFF,
                                    0xFF - ((play->state.frames * 0xF) & 0xFF), 0x40, 0x40, 1,
                                    (play->state.frames * 0xF) & 0xFF, 0xFF - ((play->state.frames * 0x1E) & 0xFF),
                                    0x40, 0x40));

    } else if (limbIndex == 2) {
        gSPSegment(POLY_XLU_DISP++, 9,
                   two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, (play->state.frames * 3) & 0xFF,
                                    0xFF - ((play->state.frames * 5) & 0xFF), 0x40, 0x40, 1,
                                    (play->state.frames * 6) & 0xFF, 0xFF - ((play->state.frames * 0xA) & 0xFF), 0x40,
                                    0x40));
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_magic_wind.c", 646);

    return true;
}

void Magic_Wind_Actor_display(Actor* thisx, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    MagicWind* this = (MagicWind*)thisx;

    OPEN_DISPS(gfxCtx, "../z_magic_wind.c", 661);

    if (this->actionFunc != move_process_wait) {
        POLY_XLU_DISP = rcp_mode_set(POLY_XLU_DISP, SETUPDL_25);
        FcSkeletonInfo_draw(&this->actor, play, &this->skelCurve, Magic_Wind_Actor_disp_proc, NULL, 1, NULL);
    }

    CLOSE_DISPS(gfxCtx, "../z_magic_wind.c", 673);
}
