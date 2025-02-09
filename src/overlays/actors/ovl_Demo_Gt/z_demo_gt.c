#include "z_demo_gt.h"

#include "z64frame_advance.h"

#include "assets/objects/object_gt/object_gt.h"
#include "assets/objects/object_geff/object_geff.h"
#include "terminal.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void Demo_Gt_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Gt_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Gt_main(Actor* thisx, PlayState* play);
void Demo_Gt_draw(Actor* thisx, PlayState* play);

void Demo_Gt_Actor_dt(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;

    if ((this->dyna.actor.params == 1) || (this->dyna.actor.params == 2)) {
        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void Demo_Gt_Set_EarthQuakeSound(void) {
    Na_StartFixSe_F(NA_SE_EV_EARTHQUAKE - SFX_FLAG);
}

void Demo_Gt_Set_BombSound(PlayState* play, Vec3f* pos) {
    Effect_SE_Info_new(play, pos, 60, NA_SE_IT_BOMB_EXPLOSION);
}

void Demo_Gt_Set_BombSound2(PlayState* play, Vec3f* pos) {
    Effect_SE_Info_new(play, pos, 60, NA_SE_EV_GRAVE_EXPLOSION);
}

void Demo_Gt_Set_Viblation(PlayState* play) {
    z_vibctl2_vib_setQ(0.0f, 50, 10, 5);
}

void Birth_SingleDust_In_Demo_Gt(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, s16 scaleStep, s16 life) {
    static Color_RGBA8 prim = { 100, 80, 100, 0 };
    static Color_RGBA8 env = { 255, 110, 96, 0 };

    Effect_SS_Dust_sc_cl_co_nofog_ct(play, pos, velocity, accel, &prim, &env, ((fqrand() * (scale * 0.2f)) + scale),
                  scaleStep, life);
}

void Birth_BirrarDust_In_Demo_Gt(PlayState* play, Vec3f* pos, Vec3f* velOffset, f32 scale, s32 arg4, s32 arg5, s16 life) {
    s32 pad;

    if (!_Game_play_isPause(play)) {
        s32 frames = play->gameplayFrames;

        if (ABS(frames % arg4) == arg5) {
            s32 pad[2];
            Vec3f velocity = { 0.0f, 6.0f, 0.0f };
            Vec3f accel = { 0.0f, 0.0f, 0.0f };

            velocity.x *= scale;
            velocity.y *= scale;
            velocity.z *= scale;

            velocity.x += velOffset->x;
            velocity.y += velOffset->y;
            velocity.z += velOffset->z;

            if (0) {}

            accel.x *= scale;
            accel.y *= scale;
            accel.z *= scale;

            Birth_SingleDust_In_Demo_Gt(play, pos, &velocity, &accel, (300.0f * scale), (15.0f * scale), life);
        }
    }
}

Actor* Birth_RingDust2_In_Demo_Gt(PlayState* play, Vec3f* pos, s16 params) {
    return Actor_info_make_actor(&play->actorCtx, play, ACTOR_BG_SPOT16_DOUGHNUT, pos->x, pos->y, pos->z, 0, 0, 0, params);
}

void Birth_SingleBomb_In_Demo_Gt(PlayState* play, Vec3f* pos, f32 scale) {
    s32 pad;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };

    Effect_SS_Bomb2_2_ct(play, pos, &velocity, &accel, (100.0f * scale), (15.0f * scale));
    Demo_Gt_Set_BombSound(play, pos);
}

void Birth_MoveBomb_In_Demo_Gt(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale) {
    Effect_SS_Bomb2_2_ct(play, pos, velocity, accel, (100.0f * scale), (25.0f * scale));
}

void Birth_Effect_Piece1_In_Demo_Gt(DemoGt* this, PlayState* play, Vec3f* spawnerPos) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 12; i++) {

        pos.x = sin_s(angle) * 46.0f;
        pos.y = (fqrand() * 75.0f) + 2.0f;
        pos.z = cos_s(angle) * 46.0f;

        velocity.x = (pos.x * 0.1f) + 20.0f;
        velocity.y = fqrand() * 16.0f;
        velocity.z = pos.z * 0.1f;

        pos.x += spawnerPos->x;
        pos.y += spawnerPos->y;
        pos.z += spawnerPos->z;

        temp_f0 = fqrand();

        if (temp_f0 < 0.1f) {
            phi_s0 = 96;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 64;
        } else {
            phi_s0 = 32;
        }

        Effect_Kakera_ct2(play, &pos, &velocity, spawnerPos, -247, phi_s0, 3, 0, 0,
                             (s32)(fqrand() * 10.0f + 30.0f), 2, 300, (s32)(fqrand() * 0.0f) + 30,
                             KAKERA_COLOR_NONE, OBJECT_GEFF, gGanonRubbleDL);
        angle += 0x1555;
    }
}

void Birth_Effect_Piece2_In_Demo_Gt(DemoGt* this, PlayState* play, Vec3f* spawnerPos) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 8; i++) {

        pos.x = sin_s(angle) * 30.0f;
        pos.y = (fqrand() * 75.0f) + 2.0f;
        pos.z = cos_s(angle) * 30.0f;

        velocity.x = 0.0f;
        velocity.y = fqrand() * -4.0f;
        velocity.z = pos.z * 0.1f;

        pos.x += spawnerPos->x;
        pos.y += spawnerPos->y;
        pos.z += spawnerPos->z;

        temp_f0 = fqrand();

        if (temp_f0 < 0.1f) {
            phi_s0 = 96;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 64;
        } else {
            phi_s0 = 32;
        }

        Effect_Kakera_ct2(play, &pos, &velocity, spawnerPos, -247, phi_s0, 3, 0, 0,
                             (s32)((fqrand() * 10.0f) + 30.0f), 2, 300, (s32)(fqrand() * 0.0f) + 0x1E,
                             KAKERA_COLOR_NONE, OBJECT_GEFF, gGanonRubbleDL);

        angle += 0x2000;
    }
}

void Birth_Effect_Piece3_In_Demo_Gt(DemoGt* this, PlayState* play, Vec3f* spawnerPos) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 12; i++) {

        pos.x = sin_s(angle) * 16.0f;
        pos.y = (fqrand() * 5.0f) + 2.0f;
        pos.z = cos_s(angle) * 16.0f;

        velocity.x = pos.x * 0.6f;
        velocity.y = (fqrand() * 36.0f) + 6.0f;
        velocity.z = pos.z * 0.6f;

        pos.x += spawnerPos->x;
        pos.y += spawnerPos->y;
        pos.z += spawnerPos->z;

        temp_f0 = fqrand();

        if (temp_f0 < 0.1f) {
            phi_s0 = 97;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 65;
        } else {
            phi_s0 = 33;
        }

        Effect_Kakera_ct2(play, &pos, &velocity, spawnerPos, -200, phi_s0, 10, 10, 0, fqrand() * 30.0f + 30.0f,
                             2, 300, (s32)(fqrand() * 30.0f) + 30, KAKERA_COLOR_NONE, OBJECT_GEFF,
                             gGanonRubbleDL);
        angle += 0x1555;
    }
}

void Birth_Effect_Piece4_In_Demo_Gt(PlayState* play, Vec3f* arg1, s16 arg2) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 1; i++) {

        pos.x = sin_s(angle) * 46.0f;
        pos.y = (fqrand() * 75.0f) - 28.0f;
        pos.z = cos_s(angle) * 46.0f;

        velocity.x = sin_s(arg2) * 3.0f;
        velocity.y = (fqrand() * -4.0f) + 10.0f;
        velocity.z = cos_s(arg2) * 3.0f;

        pos.x += arg1->x;
        pos.y += arg1->y;
        pos.z += arg1->z;

        temp_f0 = fqrand();

        if (temp_f0 < 0.1f) {
            phi_s0 = 97;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 65;
        } else {
            phi_s0 = 33;
        }

        Effect_Kakera_ct2(play, &pos, &velocity, arg1, -247, phi_s0, 3, 0, 0,
                             (s32)((fqrand() * 10.0f) + 30.0f), 2, 300, (s32)(fqrand() * 0.0f) + 30,
                             KAKERA_COLOR_NONE, OBJECT_GEFF, gGanonRubbleDL);

        angle += 0x10000;
    }
}

void Birth_RingDust3_In_Demo_Gt(PlayState* play, Vec3f* spawnerPos, Vec3f* velocity, Vec3f* accel, f32 arg4, f32 scale, s32 arg6,
                   s32 arg7, s16 life) {
    s32 pad2[3];
    s16 increment;
    s32 frames;
    s32 i;
    s16 phi_s0;
    s16 dustScaleStep = 15.0f * scale;
    f32 dustScale = 300.0f * scale;

    if ((!_Game_play_isPause(play)) && (arg7 > 0) && (arg6 > 0)) {
        frames = (ABS((s32)play->gameplayFrames) % arg7);
        phi_s0 = 0x10000 * frames / arg6;
        increment = 0x10000 / arg6;

        for (i = frames; i < arg6; i += arg7) {
            Vec3f pos;

            pos.x = (sin_s(phi_s0) * arg4) + spawnerPos->x;
            pos.y = spawnerPos->y;
            pos.z = (cos_s(phi_s0) * arg4) + spawnerPos->z;

            Birth_SingleDust_In_Demo_Gt(play, &pos, velocity, accel, dustScale, dustScaleStep, life);

            if (fqrand() <= 0.05f) {
                Birth_Effect_Piece4_In_Demo_Gt(play, &pos, phi_s0);
            }

            phi_s0 += increment;
        }
    }
}

u8 Demo_Gt_Check_EndDemoMode(PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        return true;
    } else {
        return false;
    }
}

CsCmdActorCue* Demo_Gt_Get_npcdemopnt(PlayState* play, u32 cueChannel) {
    s32 pad[2];
    CsCmdActorCue* cue = NULL;

    if (!Demo_Gt_Check_EndDemoMode(play)) {
        cue = play->csCtx.actorCues[cueChannel];
    }

    return cue;
}

u8 Demo_Gt_Check_npcdemopnt(PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Gt_Get_npcdemopnt(play, cueChannel);

    if ((cue != NULL) && (cue->id == cueId)) {
        return true;
    } else {
        return false;
    }
}

void Demo_Gt_SetPos_fromData(DemoGt* this, PlayState* play, u32 cueChannel) {
    CsCmdActorCue* cue = Demo_Gt_Get_npcdemopnt(play, cueChannel);
    Vec3f* pos = &this->dyna.actor.world.pos;
    f32 startX;
    f32 startY;
    f32 startZ;
    f32 endX;
    f32 endY;
    f32 endZ;
    f32 lerp;

    if (cue != NULL) {
        lerp = get_parcent_forAccelBrake(cue->endFrame, cue->startFrame, play->csCtx.curFrame, 8, 0);

        startX = cue->startPos.x;
        startY = cue->startPos.y;
        startZ = cue->startPos.z;

        endX = cue->endPos.x;
        endY = cue->endPos.y;
        endZ = cue->endPos.z;

        pos->x = ((endX - startX) * lerp) + startX;
        pos->y = ((endY - startY) * lerp) + startY;
        pos->z = ((endZ - startZ) * lerp) + startZ;
    }
}

void Demo_Gt_SetPos_fromOffset(DemoGt* this, s32 arg1) {
    s16 phi_a1;
    s16 phi_a2;
    s16 phi_a3;
    Vec3f* pos = &this->dyna.actor.world.pos;
    Vec3s* unk16C = &this->unk_16C;
    s32 pad;
    f32 tempf3;
    f32 tempf2;
    f32 tempf1;
    f32 phi_f2;
    f32 phi_f12;
    f32 phi_f14;

    if (arg1 == 1) {
        phi_a1 = kREG(19) + 0x8000;
        phi_a2 = kREG(20) + 0x8000;
        phi_a3 = kREG(21) + 0x8000;
        phi_f14 = kREG(16) * 0.1f;
        phi_f12 = (kREG(17) * 0.1f) + 0.5f;
        phi_f2 = kREG(18) * 0.1f;
    } else if (arg1 == 2) {
        phi_a1 = kREG(25) + 0x8000;
        phi_a2 = kREG(26) + 0x8000;
        phi_a3 = kREG(27) + 0x8000;
        phi_f14 = kREG(22) * 0.1f;
        phi_f12 = (kREG(23) * 0.1f) + 0.5f;
        phi_f2 = kREG(24) * 0.1f;
    } else if (arg1 == 3) {
        phi_a1 = kREG(31) + 0x8000;
        phi_a2 = kREG(32) + 0x8000;
        phi_a3 = kREG(33) + 0x8000;
        phi_f14 = kREG(28) * 0.1f;
        phi_f12 = (kREG(29) * 0.1f) + 0.5f;
        phi_f2 = kREG(30) * 0.1f;
    } else if (arg1 == 4) {
        phi_a1 = kREG(37) + 0x8000;
        phi_a2 = kREG(38) + 0x8000;
        phi_a3 = kREG(39) + 0x8000;
        phi_f14 = kREG(34) * 0.1f;
        phi_f12 = (kREG(35) * 0.1f) + 0.5f;
        phi_f2 = kREG(36) * 0.1f;
    } else if (arg1 == 5) {
        phi_a1 = kREG(43) + 0x8000;
        phi_a2 = kREG(44) + 0x8000;
        phi_a3 = kREG(45) + 0x8000;
        phi_f14 = kREG(40) * 0.1f;
        phi_f12 = (kREG(41) * 0.1f) + 0.5f;
        phi_f2 = kREG(42) * 0.1f;
    } else if (arg1 == 6) {
        phi_a1 = kREG(49) + 0x8000;
        phi_a2 = kREG(50) + 0x8000;
        phi_a3 = kREG(51) + 0x8000;
        phi_f14 = kREG(46) * 0.1f;
        phi_f12 = (kREG(47) * 0.1f) + 0.5f;
        phi_f2 = kREG(48) * 0.1f;
    } else if (arg1 == 7) {
        phi_a1 = kREG(85) + 0x8000;
        phi_a2 = kREG(86) + 0x8000;
        phi_a3 = kREG(87) + 0x8000;
        phi_f14 = kREG(82) * 0.1f;
        phi_f12 = (kREG(83) * 0.1f) + 0.5f;
        phi_f2 = kREG(84) * 0.1f;
    } else {
        phi_a1 = kREG(91) + 0x8000;
        phi_a2 = kREG(92) + 0x8000;
        phi_a3 = kREG(93) + 0x8000;
        phi_f14 = kREG(88) * 0.1f;
        phi_f12 = (kREG(89) * 0.1f) + 0.5f;
        phi_f2 = kREG(90) * 0.1f;
    }

    unk16C->x += phi_a1;
    unk16C->y += phi_a2;
    unk16C->z += phi_a3;

    tempf1 = cos_s(unk16C->x) * phi_f14;
    tempf2 = cos_s(unk16C->y) * phi_f12;
    tempf3 = cos_s(unk16C->z) * phi_f2;

    pos->x += tempf1;
    pos->y += tempf2;
    pos->z += tempf3;
}

void Demo_Gt_SetPos_forFall(DemoGt* this, PlayState* play, s32 cueChannel) {
    Demo_Gt_SetPos_fromData(this, play, cueChannel);
    Demo_Gt_SetPos_fromOffset(this, cueChannel);
}

u8 Demo_Gt_Check_DemoScene(void) {
    if (DEBUG_FEATURES && (kREG(2) != 0)) {
        return true;
    } else if (!IS_CUTSCENE_LAYER) {
        return false;
    } else {
        return true;
    }
}

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Demo_Gt_Actor_init_BGdata_common(DemoGt* this, PlayState* play, CollisionHeader* collision) {
    s32 pad[3];
    CollisionHeader* colHeader;

    if (collision != NULL) {
        ValueSet_process(&this->dyna.actor, value_init);
        MoveBG_ct(&this->dyna, 0);
        colHeader = NULL;
        DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    }
}

u8 Demo_Gt_Actor_init_part_common(DemoGt* this, PlayState* play, s32 updateMode, s32 drawConfig, CollisionHeader* colHeader) {
    if (Demo_Gt_Check_DemoScene()) {
        this->updateMode = updateMode;
        this->drawConfig = drawConfig;
        Demo_Gt_Actor_init_BGdata_common(this, play, colHeader);
        return true;
    } else {
        Actor_delete(&this->dyna.actor);
        return false;
    }
}

#include "z_demo_gt_part1.inc.c"

#include "z_demo_gt_part2.inc.c"

#include "z_demo_gt_part3.inc.c"

#include "z_demo_gt_part4_1.inc.c"

#include "z_demo_gt_part4_2.inc.c"

#include "z_demo_gt_part4_3.inc.c"

#include "z_demo_gt_part5.inc.c"

#include "z_demo_gt_part6.inc.c"

void Demo_Gt_main(Actor* thisx, PlayState* play) {
    static DemoGtUpdateFunc proc[] = {
        Demo_Gt_main_Stand_part1,  Demo_Gt_main_Stand_part2,  Demo_Gt_main_Stand_part3,  Demo_Gt_main_Stand_part4_1,  Demo_Gt_main_Stand_part4_2,
        Demo_Gt_main_Stand_part4_3,  Demo_Gt_main_Stand_part5,  Demo_Gt_main_Stand_part6,  Demo_Gt_main_Fall_part1,  Demo_Gt_main_Fall_part2,
        Demo_Gt_main_Fall_part3, Demo_Gt_main_Fall_part4_1, Demo_Gt_main_Fall_part4_2, Demo_Gt_main_Fall_part4_3, Demo_Gt_main_Fall_part5,
        Demo_Gt_main_Fall_part6, Demo_Gt_main_Lay_part4_1, Demo_Gt_main_Lay_part4_2, Demo_Gt_main_Lay_part4_3,
    };

    DemoGt* this = (DemoGt*)thisx;
    DemoGtUpdateFunc updateFunc;

    if ((this->updateMode < 0) || (this->updateMode >= 19) || (updateFunc = proc[this->updateMode]) == NULL) {
        // "The main mode is strange!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    updateFunc(this, play);
}

void Demo_Gt_Actor_ct(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;

    switch (this->dyna.actor.params) {
        case 0:
            Demo_Gt_Actor_init_part1(this, play);
            break;
        case 1:
            Demo_Gt_Actor_init_part2(this, play);
            break;
        case 2:
            Demo_Gt_Actor_init_part3(this, play);
            break;
        case 5:
            Demo_Gt_Actor_init_part4_1(this, play);
            break;
        case 6:
            Demo_Gt_Actor_init_part4_2(this, play);
            break;
        case 7:
            Demo_Gt_Actor_init_part4_3(this, play);
            break;
        case 23:
            Demo_Gt_Actor_init_part5(this, play);
            break;
        case 24:
            Demo_Gt_Actor_init_part6(this, play);
            break;
        default:
            // "Demo_Gt_Actor_ct There is no such argument !"
            PRINTF("Demo_Gt_Actor_ct そんな引数は無い!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            Actor_delete(&this->dyna.actor);
    }
}

void Demo_Gt_draw_none(Actor* thisx, PlayState* play) {
}

void Demo_Gt_draw(Actor* thisx, PlayState* play) {
    static DemoGtDrawFunc proc[] = {
        Demo_Gt_draw_none, Demo_Gt_draw_normal_part1, Demo_Gt_draw_normal_part2, Demo_Gt_draw_normal_part3, Demo_Gt_draw_normal_part4_1,
        Demo_Gt_draw_normal_part4_2, Demo_Gt_draw_normal_part4_3, Demo_Gt_draw_normal_part5, Demo_Gt_draw_normal_part6,
    };

    DemoGt* this = (DemoGt*)thisx;
    DemoGtDrawFunc drawFunc;

    if ((this->drawConfig < 0) || (this->drawConfig >= 9) || (drawFunc = proc[this->drawConfig]) == NULL) {
        // "The drawing mode is strange !!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    drawFunc(thisx, play);
}

ActorProfile Demo_Gt_Profile = {
    /**/ ACTOR_DEMO_GT,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GT,
    /**/ sizeof(DemoGt),
    /**/ Demo_Gt_Actor_ct,
    /**/ Demo_Gt_Actor_dt,
    /**/ Demo_Gt_main,
    /**/ Demo_Gt_draw,
};
