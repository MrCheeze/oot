/**
 * File: z_demo_kekkai.c
 * Overlay: ovl_Demo_Kekkai
 * Description: Ganon's castle barriers
 */

#include "z_demo_kekkai.h"
#include "assets/objects/object_demo_kekkai/object_demo_kekkai.h"
#include "assets/scenes/dungeons/ganontika/ganontika_scene.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Demo_Kekkai_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Kekkai_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Kekkai_Actor_move(Actor* thisx, PlayState* play2);
void Demo_Kekkai_Actor_draw(Actor* thisx, PlayState* play);

void Demo_Kekkai_Actor_move_2(Actor* thisx, PlayState* play);
void Demo_Kekkai_Actor_move_1(Actor* thisx, PlayState* play);
void Demo_Kekkai_Actor_draw_1(Actor* thisx, PlayState* play2);

void move_kekkai_center(DemoKekkai* this, PlayState* play);

ActorProfile Demo_Kekkai_Profile = {
    /**/ ACTOR_DEMO_KEKKAI,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_DEMO_KEKKAI,
    /**/ sizeof(DemoKekkai),
    /**/ Demo_Kekkai_Actor_ct,
    /**/ Demo_Kekkai_Actor_dt,
    /**/ Demo_Kekkai_Actor_move,
    /**/ Demo_Kekkai_Actor_draw,
};

static ColliderCylinderInit DemoKekkai2OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x07, 0x04 },
        { 0x00002000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 680, 220, 120, { 0, 0, 0 } },
};

static u8 color_table[] = {
    /* Water   prim */ 170, 255, 255, /* env */ 0,   50,  255,
    /* Light   prim */ 255, 255, 170, /* env */ 200, 255, 0,
    /* Fire    prim */ 255, 255, 170, /* env */ 200, 0,   0,
    /* Shadow  prim */ 255, 170, 255, /* env */ 100, 0,   200,
    /* Spirit  prim */ 255, 255, 170, /* env */ 255, 120, 0,
    /* Forest  prim */ 255, 255, 170, /* env */ 0,   200, 0,
};

s32 check_kekkai_event_bit(s32 params) {
    static s32 kekkai_event_tbl[] = {
        EVENTCHKINF_C3, EVENTCHKINF_BC, EVENTCHKINF_BF, EVENTCHKINF_BE, EVENTCHKINF_BD, EVENTCHKINF_AD, EVENTCHKINF_BB,
    };

    if ((params < KEKKAI_TOWER) || (params > KEKKAI_FOREST)) {
        return true;
    }
    return event_check(kekkai_event_tbl[params]);
}

void Demo_Kekkai_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoKekkai* this = (DemoKekkai*)thisx;

    this->sfxFlag = 0;
    this->energyAlpha = 1.0f;
    Actor_set_scale(thisx, 0.1f);
    thisx->colChkInfo.mass = MASS_IMMOVABLE;
    ClObjPipe_ct(play, &this->collider1);
    ClObjPipe_set5(play, &this->collider1, thisx, &DemoKekkai2OcInfoData);
    ClObjPipe_ct(play, &this->collider2);
    ClObjPipe_set5(play, &this->collider2, thisx, &DemoKekkai2OcInfoData);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider1);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider2);
    this->timer = 0;
    this->barrierScrollRate = 1.0f;
    this->barrierScroll = 0.0f;
    switch (thisx->params) {
        case KEKKAI_TOWER:
            this->updateFunc = move_kekkai_center;
            this->collider2.dim.radius = thisx->scale.x * 6100.0f;
            this->collider2.dim.height = thisx->scale.y * 5000.0f;
            this->collider2.dim.yShift = 300;
            break;
        case KEKKAI_WATER:
        case KEKKAI_LIGHT:
        case KEKKAI_FIRE:
        case KEKKAI_SHADOW:
        case KEKKAI_SPIRIT:
        case KEKKAI_FOREST:
            this->energyAlpha = 1.0f;
            this->orbScale = 1.0f;
            Actor_set_scale(thisx, 0.1f);
            thisx->update = Demo_Kekkai_Actor_move_1;
            thisx->draw = Demo_Kekkai_Actor_draw_1;
            this->collider1.dim.radius = thisx->scale.x * 120.0f;
            this->collider1.dim.height = thisx->scale.y * 2000.0f;
            this->collider1.dim.yShift = 0;
            this->collider2.dim.radius = thisx->scale.x * 320.0f;
            this->collider2.dim.height = thisx->scale.y * 510.0f;
            this->collider2.dim.yShift = 95;
            break;
    }
    if (check_kekkai_event_bit(thisx->params)) {
        if (thisx->params == KEKKAI_TOWER) {
            play->envCtx.lightSettingOverride = 1;
        }
        Actor_delete(thisx);
    }
}

void Demo_Kekkai_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoKekkai* this = (DemoKekkai*)thisx;

    ClObjPipe_dt(play, &this->collider1);
    ClObjPipe_dt(play, &this->collider2);
}

void demo_kekka_kirakira(DemoKekkai* this, PlayState* play) {
    static Vec3f kirakira_vec = { 0.0f, 0.0f, 0.0f };
    static Vec3f kirakira_acc = { 0.0f, 0.0f, 0.0f };
    static Color_RGBA8 kirakira_prim = { 255, 255, 170, 0 };
    static Color_RGBA8 kirakira_env = { 200, 0, 0, 0 };
    Vec3f pos;
    s32 i;

    for (i = 0; i < 85; i++) {
        s16 roll = rnd_f(65535.0f);
        s16 yaw = rnd_f(65535.0f);

        kirakira_vec.x = sin_s(yaw) * cos_s(roll) * rnd_f(8.0f);
        kirakira_vec.z = cos_s(yaw) * cos_s(roll) * rnd_f(8.0f);
        kirakira_vec.y = sin_s(roll) * rnd_f(3.0f);

        pos.x = (kirakira_vec.x * 7.0f) + this->actor.world.pos.x;
        pos.y = (kirakira_vec.y * 20.0f) + this->actor.world.pos.y + 120.0f;
        pos.z = (kirakira_vec.z * 7.0f) + this->actor.world.pos.z;

        Effect_SS_KiraKira_soul_sc_ct_ct(play, &pos, &kirakira_vec, &kirakira_acc, &kirakira_prim, &kirakira_env, 3000,
                                      (s32)rnd_f(40.0f) + 45);
    }
}

void move_kekkai_center(DemoKekkai* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[0] != NULL) &&
        (play->csCtx.actorCues[0]->id != 1) && (play->csCtx.actorCues[0]->id == 2)) {
        if (!(this->sfxFlag & 1)) {
            Na_StartDemoSe(SEQ_CS_EFFECTS_DISPEL_BARRIER);
            this->sfxFlag |= 1;
        }
        if (this->barrierScrollRate < 7.0f) {
            this->barrierScrollRate += 0.2f;
        } else {
            this->timer++;
            if (this->timer > 100) {
                event_set(EVENTCHKINF_C3);
                Actor_delete(&this->actor);
                return;
            } else if (this->timer > 40) {
                this->actor.scale.z = this->actor.scale.x += 0.003f;
            }
        }
    }
    if (!(this->sfxFlag & 1)) {
        Actor_level_SE_set(&this->actor, NA_SE_EV_TOWER_BARRIER - SFX_FLAG);
    }
}

void Demo_Kekkai_Actor_move(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    DemoKekkai* this = (DemoKekkai*)thisx;

    if (this->energyAlpha > 0.99f) {
        if ((this->collider1.base.atFlags & AT_HIT) || (this->collider2.base.atFlags & AT_HIT)) {
            Actor_player_power_damage_set(play, &this->actor, 6.0f, this->actor.yawTowardsPlayer, 6.0f);
        }
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_setAT(play, &play->colChkCtx, &this->collider2.base);
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider2.base);
    }
    this->updateFunc(this, play);
    this->barrierScroll += this->barrierScrollRate;
    if (this->barrierScroll > 65536.0f) {
        this->barrierScroll -= 65536.0f;
    }
}

void Demo_Kekkai_Actor_move_2(Actor* thisx, PlayState* play) {
    static u16 se_timing[] = { 0, 280, 280, 280, 280, 280, 280 };
    s32 pad;
    DemoKekkai* this = (DemoKekkai*)thisx;

    if (play->csCtx.curFrame == se_timing[this->actor.params]) {
        Na_StartDemoSe(SEQ_CS_EFFECTS_TRIAL_WARP);
    }
    if (this->energyAlpha >= 0.05f) {
        this->energyAlpha -= 0.05f;
    } else {
        this->energyAlpha = 0.0f;
    }
    if (this->timer < 40) {
        this->orbScale = ((80 - this->timer) * (f32)this->timer * 0.000625f) + 1.0f;
    } else if (this->timer < 50) {
        this->orbScale = 2.0f;
    } else if (this->timer == 50) {
        Actor_SE_set(&this->actor, NA_SE_IT_DM_RING_EXPLOSION);
        demo_kekka_kirakira(this, play);
    } else {
        this->orbScale = 0.0f;
    }
    if (this->orbScale != 0.0f) {
        Actor_level_SE_set(&this->actor, NA_SE_EV_TOWER_ENERGY - SFX_FLAG);
    }
    this->timer++;
}

static CutsceneData* demo_kekkai_tbl[] = {
    NULL,
    gWaterTrialSageCs,
    gLightTrialSageCs,
    gFireTrialSageCs,
    gShadowTrialSageCs,
    gSpiritTrialSageCs,
    gForestTrialSageCs,
};

void Demo_Kekkai_Actor_move_1(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoKekkai* this = (DemoKekkai*)thisx;

    if (this->collider1.base.atFlags & AT_HIT) {
        Actor_player_power_damage_set(play, &this->actor, 5.0f, this->actor.yawTowardsPlayer, 5.0f);
    }
    CollisionCheck_setAT(play, &play->colChkCtx, &this->collider1.base);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider1.base);
    if (this->collider2.base.acFlags & AC_HIT) {
        Na_StartSystemSe_F(NA_SE_SY_CORRECT_CHIME);
        // "I got it"
        LOG_STRING("当ったよ", "../z_demo_kekkai.c", 572);
        this->actor.update = Demo_Kekkai_Actor_move_2;
        this->timer = 0;
        play->csCtx.script = SEGMENTED_TO_VIRTUAL(demo_kekkai_tbl[this->actor.params]);
        z_common_data.cutsceneTrigger = 1;
    }
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider2.base);
    Actor_level_SE_set(&this->actor, NA_SE_EV_TOWER_ENERGY - SFX_FLAG);
}

void Demo_Kekkai_Actor_draw_1(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    s32 frames = play->gameplayFrames & 0xFFFF;
    u8 alphaIndex[102] = {
        1, 1, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 1, 0, 0, 1, 2, 2,
        1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        1, 1, 0, 0, 0, 1, 1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1, 0, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 0, 0,
    };
    s32 colorIndex;
    DemoKekkai* this = (DemoKekkai*)thisx;
    u8 alphas[3];
    Vtx* energyVtx = SEGMENTED_TO_VIRTUAL(gTrialBarrierEnergyVtx);
    s32 i;

    if (this->orbScale == 0.0f) {
        return;
    }

    alphas[2] = (s32)(this->energyAlpha * 202.0f);
    alphas[1] = (s32)(this->energyAlpha * 126.0f);
    alphas[0] = 0;

    for (i = 0; i < 102; i++) {
        energyVtx[i].v.cn[3] = alphas[alphaIndex[i]];
    }

    colorIndex = (this->actor.params - 1) * 6;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_kekkai.c", 632);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    Matrix_push();
    Matrix_translate(0.0f, 1200.0f, 0.0f, MTXMODE_APPLY);
    Matrix_scale(this->orbScale, this->orbScale, this->orbScale, MTXMODE_APPLY);
    Matrix_translate(0.0f, -1200.0f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_kekkai.c", 639);
    gSPSegment(POLY_XLU_DISP++, 0x09,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, frames * 5, frames * -10, 0x20, 0x20, 1,
                                frames * 5, frames * -10, 0x20, 0x20));
    gSPDisplayList(POLY_XLU_DISP++, gTrialBarrierOrbDL);
    Matrix_pull();
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_kekkai.c", 656);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 50, 0, 100, 255);
    gSPSegment(POLY_XLU_DISP++, 0x0A,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 0x20, 0x20, 1, frames, frames, 0x20, 0x20));
    gSPDisplayList(POLY_XLU_DISP++, gTrialBarrierFloorDL);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, color_table[colorIndex + 0], color_table[colorIndex + 1],
                    color_table[colorIndex + 2], 255);
    gDPSetEnvColor(POLY_XLU_DISP++, color_table[colorIndex + 3], color_table[colorIndex + 4],
                   color_table[colorIndex + 5], 128);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, frames * 5, frames * -10, 0x20, 0x20, 1,
                                frames * 5, frames * -10, 0x20, 0x40));
    gSPDisplayList(POLY_XLU_DISP++, gTrialBarrierEnergyDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_kekkai.c", 696);
}

void Demo_Kekkai_Actor_draw(Actor* thisx, PlayState* play) {
    s32 pad;
    DemoKekkai* this = (DemoKekkai*)thisx;
    s32 scroll;

    scroll = (s32)this->barrierScroll & 0xFFFF;
    OPEN_DISPS(play->state.gfxCtx, "../z_demo_kekkai.c", 705);
    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_demo_kekkai.c", 707);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x00, 0x80, 255, 170, 255, 255);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, scroll * 2, scroll * -4, 0x20, 0x40, 1, scroll * 2,
                                scroll * -4, 0x20, 0x40));
    gSPDisplayList(POLY_XLU_DISP++, gTowerBarrierDL);
    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_kekkai.c", 722);
}
