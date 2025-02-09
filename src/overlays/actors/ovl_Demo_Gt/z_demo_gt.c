#include "z_demo_gt.h"

#include "z64frame_advance.h"

#include "assets/objects/object_gt/object_gt.h"
#include "assets/objects/object_geff/object_geff.h"
#include "terminal.h"
#include "overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void DemoGt_Init(Actor* thisx, PlayState* play);
void DemoGt_Destroy(Actor* thisx, PlayState* play);
void DemoGt_Update(Actor* thisx, PlayState* play);
void DemoGt_Draw(Actor* thisx, PlayState* play);

void DemoGt_Destroy(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;

    if ((this->dyna.actor.params == 1) || (this->dyna.actor.params == 2)) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void DemoGt_PlayEarthquakeSfx(void) {
    Sfx_PlaySfxCentered2(NA_SE_EV_EARTHQUAKE - SFX_FLAG);
}

void DemoGt_PlayExplosion1Sfx(PlayState* play, Vec3f* pos) {
    SfxSource_PlaySfxAtFixedWorldPos(play, pos, 60, NA_SE_IT_BOMB_EXPLOSION);
}

void DemoGt_PlayExplosion2Sfx(PlayState* play, Vec3f* pos) {
    SfxSource_PlaySfxAtFixedWorldPos(play, pos, 60, NA_SE_EV_GRAVE_EXPLOSION);
}

void DemoGt_Rumble(PlayState* play) {
    Rumble_Request(0.0f, 50, 10, 5);
}

void DemoGt_SpawnDust(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, s16 scaleStep, s16 life) {
    static Color_RGBA8 brownPrim = { 100, 80, 100, 0 };
    static Color_RGBA8 redEnv = { 255, 110, 96, 0 };

    func_8002843C(play, pos, velocity, accel, &brownPrim, &redEnv, ((Rand_ZeroOne() * (scale * 0.2f)) + scale),
                  scaleStep, life);
}

void func_8097D7D8(PlayState* play, Vec3f* pos, Vec3f* velOffset, f32 scale, s32 arg4, s32 arg5, s16 life) {
    s32 pad;

    if (!FrameAdvance_IsEnabled(play)) {
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

            DemoGt_SpawnDust(play, pos, &velocity, &accel, (300.0f * scale), (15.0f * scale), life);
        }
    }
}

Actor* DemoGt_SpawnCloudRing(PlayState* play, Vec3f* pos, s16 params) {
    return Actor_Spawn(&play->actorCtx, play, ACTOR_BG_SPOT16_DOUGHNUT, pos->x, pos->y, pos->z, 0, 0, 0, params);
}

void DemoGt_SpawnExplosionWithSound(PlayState* play, Vec3f* pos, f32 scale) {
    s32 pad;
    Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };

    EffectSsBomb2_SpawnLayered(play, pos, &velocity, &accel, (100.0f * scale), (15.0f * scale));
    DemoGt_PlayExplosion1Sfx(play, pos);
}

void DemoGt_SpawnExplosionNoSound(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale) {
    EffectSsBomb2_SpawnLayered(play, pos, velocity, accel, (100.0f * scale), (25.0f * scale));
}

void func_8097DAC8(DemoGt* this, PlayState* play, Vec3f* spawnerPos) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 12; i++) {

        pos.x = Math_SinS(angle) * 46.0f;
        pos.y = (Rand_ZeroOne() * 75.0f) + 2.0f;
        pos.z = Math_CosS(angle) * 46.0f;

        velocity.x = (pos.x * 0.1f) + 20.0f;
        velocity.y = Rand_ZeroOne() * 16.0f;
        velocity.z = pos.z * 0.1f;

        pos.x += spawnerPos->x;
        pos.y += spawnerPos->y;
        pos.z += spawnerPos->z;

        temp_f0 = Rand_ZeroOne();

        if (temp_f0 < 0.1f) {
            phi_s0 = 96;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 64;
        } else {
            phi_s0 = 32;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, spawnerPos, -247, phi_s0, 3, 0, 0,
                             (s32)(Rand_ZeroOne() * 10.0f + 30.0f), 2, 300, (s32)(Rand_ZeroOne() * 0.0f) + 30,
                             KAKERA_COLOR_NONE, OBJECT_GEFF, gGanonRubbleDL);
        angle += 0x1555;
    }
}

void func_8097DD28(DemoGt* this, PlayState* play, Vec3f* spawnerPos) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 8; i++) {

        pos.x = Math_SinS(angle) * 30.0f;
        pos.y = (Rand_ZeroOne() * 75.0f) + 2.0f;
        pos.z = Math_CosS(angle) * 30.0f;

        velocity.x = 0.0f;
        velocity.y = Rand_ZeroOne() * -4.0f;
        velocity.z = pos.z * 0.1f;

        pos.x += spawnerPos->x;
        pos.y += spawnerPos->y;
        pos.z += spawnerPos->z;

        temp_f0 = Rand_ZeroOne();

        if (temp_f0 < 0.1f) {
            phi_s0 = 96;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 64;
        } else {
            phi_s0 = 32;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, spawnerPos, -247, phi_s0, 3, 0, 0,
                             (s32)((Rand_ZeroOne() * 10.0f) + 30.0f), 2, 300, (s32)(Rand_ZeroOne() * 0.0f) + 0x1E,
                             KAKERA_COLOR_NONE, OBJECT_GEFF, gGanonRubbleDL);

        angle += 0x2000;
    }
}

void func_8097DF70(DemoGt* this, PlayState* play, Vec3f* spawnerPos) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 12; i++) {

        pos.x = Math_SinS(angle) * 16.0f;
        pos.y = (Rand_ZeroOne() * 5.0f) + 2.0f;
        pos.z = Math_CosS(angle) * 16.0f;

        velocity.x = pos.x * 0.6f;
        velocity.y = (Rand_ZeroOne() * 36.0f) + 6.0f;
        velocity.z = pos.z * 0.6f;

        pos.x += spawnerPos->x;
        pos.y += spawnerPos->y;
        pos.z += spawnerPos->z;

        temp_f0 = Rand_ZeroOne();

        if (temp_f0 < 0.1f) {
            phi_s0 = 97;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 65;
        } else {
            phi_s0 = 33;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, spawnerPos, -200, phi_s0, 10, 10, 0, Rand_ZeroOne() * 30.0f + 30.0f,
                             2, 300, (s32)(Rand_ZeroOne() * 30.0f) + 30, KAKERA_COLOR_NONE, OBJECT_GEFF,
                             gGanonRubbleDL);
        angle += 0x1555;
    }
}

void func_8097E1D4(PlayState* play, Vec3f* arg1, s16 arg2) {
    Vec3f pos;
    Vec3f velocity;
    f32 temp_f0;
    s16 angle;
    s16 phi_s0;
    s32 i;

    angle = 0;

    for (i = 0; i < 1; i++) {

        pos.x = Math_SinS(angle) * 46.0f;
        pos.y = (Rand_ZeroOne() * 75.0f) - 28.0f;
        pos.z = Math_CosS(angle) * 46.0f;

        velocity.x = Math_SinS(arg2) * 3.0f;
        velocity.y = (Rand_ZeroOne() * -4.0f) + 10.0f;
        velocity.z = Math_CosS(arg2) * 3.0f;

        pos.x += arg1->x;
        pos.y += arg1->y;
        pos.z += arg1->z;

        temp_f0 = Rand_ZeroOne();

        if (temp_f0 < 0.1f) {
            phi_s0 = 97;
        } else if (temp_f0 < 0.7f) {
            phi_s0 = 65;
        } else {
            phi_s0 = 33;
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, arg1, -247, phi_s0, 3, 0, 0,
                             (s32)((Rand_ZeroOne() * 10.0f) + 30.0f), 2, 300, (s32)(Rand_ZeroOne() * 0.0f) + 30,
                             KAKERA_COLOR_NONE, OBJECT_GEFF, gGanonRubbleDL);

        angle += 0x10000;
    }
}

void func_8097E454(PlayState* play, Vec3f* spawnerPos, Vec3f* velocity, Vec3f* accel, f32 arg4, f32 scale, s32 arg6,
                   s32 arg7, s16 life) {
    s32 pad2[3];
    s16 increment;
    s32 frames;
    s32 i;
    s16 phi_s0;
    s16 dustScaleStep = 15.0f * scale;
    f32 dustScale = 300.0f * scale;

    if ((!FrameAdvance_IsEnabled(play)) && (arg7 > 0) && (arg6 > 0)) {
        frames = (ABS((s32)play->gameplayFrames) % arg7);
        phi_s0 = 0x10000 * frames / arg6;
        increment = 0x10000 / arg6;

        for (i = frames; i < arg6; i += arg7) {
            Vec3f pos;

            pos.x = (Math_SinS(phi_s0) * arg4) + spawnerPos->x;
            pos.y = spawnerPos->y;
            pos.z = (Math_CosS(phi_s0) * arg4) + spawnerPos->z;

            DemoGt_SpawnDust(play, &pos, velocity, accel, dustScale, dustScaleStep, life);

            if (Rand_ZeroOne() <= 0.05f) {
                func_8097E1D4(play, &pos, phi_s0);
            }

            phi_s0 += increment;
        }
    }
}

u8 DemoGt_IsCutsceneIdle(PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        return true;
    } else {
        return false;
    }
}

CsCmdActorCue* DemoGt_GetCue(PlayState* play, u32 cueChannel) {
    s32 pad[2];
    CsCmdActorCue* cue = NULL;

    if (!DemoGt_IsCutsceneIdle(play)) {
        cue = play->csCtx.actorCues[cueChannel];
    }

    return cue;
}

u8 func_8097E704(PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = DemoGt_GetCue(play, cueChannel);

    if ((cue != NULL) && (cue->id == cueId)) {
        return true;
    } else {
        return false;
    }
}

void func_8097E744(DemoGt* this, PlayState* play, u32 cueChannel) {
    CsCmdActorCue* cue = DemoGt_GetCue(play, cueChannel);
    Vec3f* pos = &this->dyna.actor.world.pos;
    f32 startX;
    f32 startY;
    f32 startZ;
    f32 endX;
    f32 endY;
    f32 endZ;
    f32 lerp;

    if (cue != NULL) {
        lerp = Environment_LerpWeightAccelDecel(cue->endFrame, cue->startFrame, play->csCtx.curFrame, 8, 0);

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

void func_8097E824(DemoGt* this, s32 arg1) {
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

    tempf1 = Math_CosS(unk16C->x) * phi_f14;
    tempf2 = Math_CosS(unk16C->y) * phi_f12;
    tempf3 = Math_CosS(unk16C->z) * phi_f2;

    pos->x += tempf1;
    pos->y += tempf2;
    pos->z += tempf3;
}

void func_8097ED64(DemoGt* this, PlayState* play, s32 cueChannel) {
    func_8097E744(this, play, cueChannel);
    func_8097E824(this, cueChannel);
}

u8 DemoGt_IsCutsceneLayer(void) {
    if (DEBUG_FEATURES && (kREG(2) != 0)) {
        return true;
    } else if (!IS_CUTSCENE_LAYER) {
        return false;
    } else {
        return true;
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_8097EDD8(DemoGt* this, PlayState* play, CollisionHeader* collision) {
    s32 pad[3];
    CollisionHeader* colHeader;

    if (collision != NULL) {
        Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
        DynaPolyActor_Init(&this->dyna, 0);
        colHeader = NULL;
        CollisionHeader_GetVirtual(collision, &colHeader);
        this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    }
}

u8 func_8097EE44(DemoGt* this, PlayState* play, s32 updateMode, s32 drawConfig, CollisionHeader* colHeader) {
    if (DemoGt_IsCutsceneLayer()) {
        this->updateMode = updateMode;
        this->drawConfig = drawConfig;
        func_8097EDD8(this, play, colHeader);
        return true;
    } else {
        Actor_Kill(&this->dyna.actor);
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

static DemoGtUpdateFunc sUpdateFuncs[] = {
    DemoGt_Update0,  DemoGt_Update1,  DemoGt_Update2,  DemoGt_Update3,  DemoGt_Update4,
    DemoGt_Update5,  DemoGt_Update6,  DemoGt_Update7,  DemoGt_Update8,  DemoGt_Update9,
    DemoGt_Update10, DemoGt_Update11, DemoGt_Update12, DemoGt_Update13, DemoGt_Update14,
    DemoGt_Update15, DemoGt_Update16, DemoGt_Update17, DemoGt_Update18,
};

void DemoGt_Update(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;
    DemoGtUpdateFunc updateFunc;

    if ((this->updateMode < 0) || (this->updateMode >= 19) || (updateFunc = sUpdateFuncs[this->updateMode]) == NULL) {
        // "The main mode is strange!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    updateFunc(this, play);
}

void DemoGt_Init(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;

    switch (this->dyna.actor.params) {
        case 0:
            func_8097EEA8_Init0(this, play);
            break;
        case 1:
            func_8097F904_Init1(this, play);
            break;
        case 2:
            func_80980110_Init2(this, play);
            break;
        case 5:
            func_80980F00_Init5(this, play);
            break;
        case 6:
            func_809813CC_Init6(this, play);
            break;
        case 7:
            func_809818A4_Init7(this, play);
            break;
        case 23:
            func_80981C94_Init23(this, play);
            break;
        case 24:
            func_80982054_Init24(this, play);
            break;
        default:
            // "Demo_Gt_Actor_ct There is no such argument !"
            PRINTF("Demo_Gt_Actor_ct そんな引数は無い!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            Actor_Kill(&this->dyna.actor);
    }
}

void DemoGt_Draw0(Actor* thisx, PlayState* play) {
}

static DemoGtDrawFunc sDrawFuncs[] = {
    DemoGt_Draw0, DemoGt_Draw1, DemoGt_Draw2, DemoGt_Draw3, DemoGt_Draw4,
    DemoGt_Draw5, DemoGt_Draw6, DemoGt_Draw7, DemoGt_Draw8,
};

void DemoGt_Draw(Actor* thisx, PlayState* play) {
    DemoGt* this = (DemoGt*)thisx;
    DemoGtDrawFunc drawFunc;

    if ((this->drawConfig < 0) || (this->drawConfig >= 9) || (drawFunc = sDrawFuncs[this->drawConfig]) == NULL) {
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
    /**/ DemoGt_Init,
    /**/ DemoGt_Destroy,
    /**/ DemoGt_Update,
    /**/ DemoGt_Draw,
};
