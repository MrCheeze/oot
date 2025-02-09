/*
 * File: z_en_xc.c
 * Overlay: ovl_En_Xc
 * Description: Sheik
 */

#include "z_en_xc.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "assets/objects/object_xc/object_xc.h"
#include "assets/scenes/overworld/spot05/spot05_scene.h"
#include "assets/scenes/overworld/spot17/spot17_scene.h"
#include "assets/scenes/indoors/tokinoma/tokinoma_scene.h"
#include "assets/scenes/dungeons/ice_doukutu/ice_doukutu_scene.h"
#include "terminal.h"

#pragma increment_block_number "gc-eu:0 gc-eu-mq:0 gc-jp:0 gc-jp-ce:0 gc-jp-mq:0 gc-us:0 gc-us-mq:0 ique-cn:128" \
                               "ntsc-1.0:0 ntsc-1.1:0 ntsc-1.2:0 pal-1.0:0 pal-1.1:0 hiratsu3:0"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_Oa2_Actor_ct(Actor* thisx, PlayState* play);
void En_Oa2_Actor_dt(Actor* thisx, PlayState* play);
void En_Oa2_Actor_main(Actor* thisx, PlayState* play);
void En_Oa2_Actor_draw(Actor* thisx, PlayState* play);

void En_Oa2_Actor_draw_none(Actor* thisx, PlayState* play);
void En_Oa2_Actor_draw_normal(Actor* thisx, PlayState* play);
void En_Oa2_Actor_draw_take(Actor* thisx, PlayState* play);
void En_Oa2_Actor_draw_harp(Actor* thisx, PlayState* play);
void En_Oa2_Actor_draw_tryforce(Actor* thisx, PlayState* play);
void En_Oa2_draw_glare(Actor* thisx, PlayState* play);

static ColliderCylinderInitType1 En_Oa2_OcInfoData_forStand = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 25, 80, 0, { 0, 0, 0 } },
};

static void* en_oa2_eye[] = {
    gSheikEyeOpenTex,
    gSheikEyeHalfClosedTex,
    gSheikEyeShutTex,
};

void En_Oa2_ct_forCorect(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set3(play, &this->collider, &this->actor, &En_Oa2_OcInfoData_forStand);
}

void En_Oa2_Excute_Corect(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;
    Collider* colliderBase = &this->collider.base;
    s32 pad[3];

    CollisionCheck_Uty_ActorWorldPosSetPipeC(thisx, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, colliderBase);
}

void En_Oa2_Actor_dt(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void En_Oa2_Calc_turn_link(EnXc* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) - 3.0f;
    eye_moveM(&this->actor, &this->interactInfo, kREG(17) + 0xC, NPC_TRACKING_HEAD_AND_TORSO);
}

void En_Oa2_set_eye_pattern(EnXc* this) {
    s32 pad[3];
    s16* blinkTimer = &this->blinkTimer;
    s16* eyePattern = &this->eyeIdx;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = get_random_timer(60, 60);
    }

    *eyePattern = *blinkTimer;
    if (*eyePattern >= ARRAY_COUNT(en_oa2_eye)) {
        *eyePattern = 0;
    }
}

void Birth_Door_Deku_In_En_Oa2(EnXc* this, PlayState* play) {
    s32 pad;
    Vec3f* pos = &this->actor.world.pos;
    s16 angle = this->actor.shape.rot.y;
    f32 x = (sin_s(angle) * 30.0f) + pos->x;
    f32 y = pos->y + 3.0f;
    f32 z = (cos_s(angle) * 30.0f) + pos->z;

    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ARROW, x, y, z, 0xFA0, this->actor.shape.rot.y, 0, ARROW_CS_NUT);
}

void En_Oa2_BGcheck(EnXc* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_2);
}

s32 En_Oa2_Animation_Basic(EnXc* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

CsCmdActorCue* En_Oa2_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = NULL;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[cueChannel];
    }
    return cue;
}

s32 En_Oa2_Check_npcdemopnt(EnXc* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = En_Oa2_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL && cue->id == cueId) {
        return true;
    }

    return false;
}

s32 En_Oa2_Check2_npcdemopnt(EnXc* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = En_Oa2_Get_npcdemopnt(play, cueChannel);

    if (cue && cue->id != cueId) {
        return true;
    }

    return false;
}

void En_Oa2_Set_DemoStartPosAngle(EnXc* this, PlayState* play, u32 cueChannel) {
    CsCmdActorCue* cue = En_Oa2_Get_npcdemopnt(play, cueChannel);
    Actor* thisx = &this->actor;

    if (cue != NULL) {
        thisx->world.pos.x = cue->startPos.x;
        thisx->world.pos.y = cue->startPos.y;
        thisx->world.pos.z = cue->startPos.z;

        thisx->world.rot.x = thisx->shape.rot.x = cue->rot.x;
        thisx->world.rot.y = thisx->shape.rot.y = cue->rot.y;
        thisx->world.rot.z = thisx->shape.rot.z = cue->rot.z;
    }
}

void En_Oa2_Movement_fromData(EnXc* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = En_Oa2_Get_npcdemopnt(play, cueChannel);
    Vec3f* worldPos = &this->actor.world.pos;
    f32 startX;
    f32 startY;
    f32 startZ;
    f32 endX;
    f32 endY;
    f32 endZ;
    f32 lerp;

    if (cue != NULL) {
        lerp = get_parcent_forAccelBrake(cue->endFrame, cue->startFrame, play->csCtx.curFrame, 0, 0);

        startX = cue->startPos.x;
        startY = cue->startPos.y;
        startZ = cue->startPos.z;

        endX = cue->endPos.x;
        endY = cue->endPos.y;
        endZ = cue->endPos.z;

        worldPos->x = ((endX - startX) * lerp) + startX;
        worldPos->y = ((endY - startY) * lerp) + startY;
        worldPos->z = ((endZ - startZ) * lerp) + startZ;
    }
}

void En_Oa2_Change_Anime(EnXc* this, AnimationHeader* animation, u8 mode, f32 morphFrames, s32 reverseFlag) {
    s32 pad[2];
    AnimationHeader* animationSeg = SEGMENTED_TO_VIRTUAL(animation);
    f32 frameCount = Si2_anime_end_frame(&animationSeg->common);
    f32 playbackSpeed;
    f32 startFrame;
    f32 endFrame;

    if (!reverseFlag) {
        startFrame = 0.0f;
        endFrame = frameCount;
        playbackSpeed = 1.0f;
    } else {
        startFrame = frameCount;
        endFrame = 0.0f;
        playbackSpeed = -1.0f;
    }

    Skeleton_Info2_init(&this->skelAnime, animationSeg, playbackSpeed, startFrame, endFrame, mode, morphFrames);
}

void En_Oa2_CheckAndSet_Mode(EnXc* this, s32 check, s32 set) {
    if (check != this->action) {
        this->action = set;
    }
}

void En_Oa2_CheckAndSet_Mode_forPlay_stopToPlay(EnXc* this, s32 action1, s32 action2, s32 action3) {
    if (action1 != this->action) {
        if (this->action == SHEIK_ACTION_PUT_HARP_AWAY) {
            this->action = action2;
        } else {
            this->action = action3;
        }
    }
}

#if DEBUG_FEATURES
s32 EnXc_NoCutscenePlaying(PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        return true;
    }
    return false;
}

void func_80B3C820(EnXc* this) {
    Skeleton_Info2_init(&this->skelAnime, &gSheikIdleAnim, 1.0f, 0.0f, Si2_anime_end_frame(&gSheikIdleAnim),
                     ANIMMODE_LOOP, 0.0f);
    this->action = SHEIK_ACTION_53;
}

void func_80B3C888(EnXc* this, PlayState* play) {
    if (EnXc_NoCutscenePlaying(play) && this->actor.params == SHEIK_TYPE_4) {
        func_80B3C820(this);
    }
}
#endif

void En_Oa2_Movement_byAnimation(EnXc* this, PlayState* play) {
    SkelAnime* skelAnime = &this->skelAnime;

    if (skelAnime->jointTable[0].y >= skelAnime->baseTransl.y) {
        skelAnime->movementFlags |= ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y;
        Skeleton_Proc_Anime_Move_init(play, &this->actor, skelAnime, 1.0f);
    }
}

void En_Oa2_Movement_byAnimation_CorrectNone(EnXc* this, PlayState* play) {
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y;
    Skeleton_Proc_Anime_Move_init(play, &this->actor, &this->skelAnime, 1.0f);
}

void En_Oa2_Start_Movement_byAnimation(EnXc* this, PlayState* play) {
    this->skelAnime.baseTransl = this->skelAnime.jointTable[0];
    this->skelAnime.prevTransl = this->skelAnime.jointTable[0];
    this->skelAnime.movementFlags |= ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y;
    Skeleton_Proc_Anime_Move_init(play, &this->actor, &this->skelAnime, 1.0f);
}

void En_Oa2_End_Movement_byAnimation(EnXc* this) {
    this->skelAnime.movementFlags &= ~(ANIM_FLAG_UPDATE_XZ | ANIM_FLAG_UPDATE_Y);
}

void En_Oa2_Setup_TokinomaToWall(EnXc* this) {
    En_Oa2_Change_Anime(this, &gSheikArmsCrossedIdleAnim, ANIMMODE_LOOP, 0.0f, false);
    this->action = SHEIK_ACTION_BLOCK_PEDESTAL;
    this->drawMode = SHEIK_DRAW_DEFAULT;
    this->unk_30C = 1;
}

#include "z_en_oA2_inWind.inc.c"

#include "z_en_oA2_inFire.inc.c"

#include "z_en_oA2_inWater.inc.c"

#include "z_en_oA2_inSpot05.inc.c"

#include "z_en_oA2_inTokinoma.inc.c"

#include "z_en_oA2_inIceShrine.inc.c"

#pragma increment_block_number "gc-eu:64 gc-eu-mq:64 gc-jp:64 gc-jp-ce:64 gc-jp-mq:64 gc-us:64 gc-us-mq:64" \
                               "ique-cn:128 ntsc-1.0:64 ntsc-1.1:64 ntsc-1.2:64 pal-1.0:64 pal-1.1:64 hiratsu3:64"

#include "z_en_oA2_inSpot06.inc.c"

#include "z_en_oA2_inMetamol.inc.c"

#include "z_en_oA2_inStalker.inc.c"

#include "z_en_oA2_inWall.inc.c"

void En_Oa2_Actor_main(Actor* thisx, PlayState* play) {
    static EnXcActionFunc proc[] = {
        En_Oa2_Actor_main_wait,
        En_Oa2_Actor_main_hide,
        En_Oa2_Actor_main_appear,
        En_Oa2_Actor_main_approach_accel,
        En_Oa2_Actor_main_approach_move,
        En_Oa2_Actor_main_approach_brake,
        En_Oa2_Actor_main_greet,
        En_Oa2_Actor_main_take,
        En_Oa2_Actor_main_ready,
        En_Oa2_Actor_main_play,
        En_Oa2_Actor_main_play_stop,
        En_Oa2_Actor_main_play_end,
        En_Oa2_Actor_main_put,
        En_Oa2_Actor_main_goodby,
        En_Oa2_Actor_main_away_accel,
        En_Oa2_Actor_main_away_move,
        En_Oa2_Actor_main_away_brake,
        En_Oa2_Actor_main_throw,
        En_Oa2_Actor_main_fade,
        En_Oa2_Actor_main_disappear,
        En_Oa2_Toki_Actor_main_wait,
        En_Oa2_Toki_Actor_main_hide,
        En_Oa2_Toki_Actor_main_greet,
        En_Oa2_Toki_Actor_main_away_accel,
        En_Oa2_Toki_Actor_main_away_move,
        En_Oa2_Toki_Actor_main_away_brake,
        En_Oa2_Toki_Actor_main_throw,
        En_Oa2_Toki_Actor_main_fade,
        En_Oa2_Toki_Actor_main_disappear,
        En_Oa2_Ice_Actor_main_wait,
        En_Oa2_Ice_Actor_main_hide,
        En_Oa2_Ice_Actor_main_greet,
        En_Oa2_Ice_Actor_main_take,
        En_Oa2_Ice_Actor_main_ready,
        En_Oa2_Ice_Actor_main_play,
        En_Oa2_Ice_Actor_main_play_stop,
        En_Oa2_Ice_Actor_main_play_end,
        En_Oa2_Ice_Actor_main_put,
        En_Oa2_Ice_Actor_main_goodby,
        En_Oa2_Ice_Actor_main_away_accel,
        En_Oa2_Ice_Actor_main_away_move,
        En_Oa2_Ice_Actor_main_away_brake,
        En_Oa2_Ice_Actor_main_throw,
        En_Oa2_Ice_Actor_main_fade,
        En_Oa2_Ice_Actor_main_disappear,
        En_Oa2_Spot06_Actor_main_wait,
        En_Oa2_Spot06_Actor_main_hide,
        En_Oa2_Spot06_Actor_main_greet,
        En_Oa2_Spot06_Actor_main_away_accel,
        En_Oa2_Spot06_Actor_main_away_move,
        En_Oa2_Spot06_Actor_main_sitdown,
        En_Oa2_Spot06_Actor_main_diving,
        En_Oa2_Spot06_Actor_main_disappear,
        En_Oa2_Metamol_main_wait,
        En_Oa2_Metamol_main_greet,
        En_Oa2_Metamol_main_chenge,
        En_Oa2_Metamol_main_posing,
        En_Oa2_Stalker_main_wait,
        En_Oa2_Stalker_main_standing,
        En_Oa2_Stalker_main_square,
        En_Oa2_Stalker_main_brandish,
        En_Oa2_Stalker_main_drop,
        En_Oa2_Stalker_main_observe,
        En_Oa2_Stalker_main_restrain,
        En_Oa2_Stalker_main_nurse,
        En_Oa2_Stalker_main_greet,
        En_Oa2_Stalker_main_take,
        En_Oa2_Stalker_main_ready,
        En_Oa2_Stalker_main_play,
        En_Oa2_Stalker_main_play_stop,
        En_Oa2_Stalker_main_play_end,
        En_Oa2_Stalker_main_put,
        En_Oa2_Stalker_main_goodby,
        En_Oa2_Stalker_main_away_accel,
        En_Oa2_Stalker_main_away_move,
        En_Oa2_Stalker_main_away_brake,
        En_Oa2_Stalker_main_throw,
        En_Oa2_Stalker_main_fade,
        En_Oa2_Stalker_main_disappear,
        En_Oa2_Wall_main_blocking,
        En_Oa2_Wall_main_greeting,
    };

    EnXc* this = (EnXc*)thisx;
    s32 action = this->action;

    if ((action < 0) || (action >= ARRAY_COUNT(proc)) || (proc[action] == NULL)) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        proc[action](this, play);
    }
}

void En_Oa2_Actor_ct(Actor* thisx, PlayState* play) {
    EnXc* this = (EnXc*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gSheikSkel, &gSheikIdleAnim, this->jointTable, this->morphTable,
                       ARRAY_COUNT(this->jointTable));
    En_Oa2_ct_forCorect(thisx, play);

    switch (this->actor.params) {
        case SHEIK_TYPE_1:
            En_Oa2_Actor_Tokinoma_Init(this, play);
            break;
        case SHEIK_TYPE_2: // Beta Serenade Cutscene or Learning Prelude
            En_Oa2_Actor_IceShrine_Init(this, play);
            break;
        case SHEIK_TYPE_3:
            En_Oa2_Actor_Spot06_Init(this, play);
            break;
        case SHEIK_TYPE_4:
            En_Oa2_Actor_Metamol_Init(this, play);
            break;
        case SHEIK_TYPE_5:
            En_Oa2_Actor_Stalker_Init(this, play);
            break;
        case SHEIK_TYPE_MINUET:
            En_Oa2_Actor_Wind_Init(this, play);
            break;
        case SHEIK_TYPE_BOLERO:
            En_Oa2_Actor_Fire_Init(this, play);
            break;
        case SHEIK_TYPE_SERENADE:
            En_Oa2_Actor_Water_Init(this, play);
            break;
        case SHEIK_TYPE_9:
            En_Oa2_Actor_Wall_Init(this, play);
            break;
#if DEBUG_FEATURES
        case SHEIK_TYPE_0:
            En_Oa2_Actor_Spot05_Init(this, play);
            break;
#endif
        default:
            PRINTF(VT_FGCOL(RED) " En_Oa2 の arg_data がおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
            En_Oa2_Actor_Spot05_Init(this, play);
    }
}

s32 En_Oa2_BeforeDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnXc* this = (EnXc*)thisx;

    if (this->unk_30C != 0) {
        if (limbIndex == 9) {
            s32 pad;

            rot->x += this->interactInfo.torsoRot.y;
            rot->y -= this->interactInfo.torsoRot.x;
        } else if (limbIndex == 16) {
            rot->x += this->interactInfo.headRot.y;
            rot->z += this->interactInfo.headRot.x;
        }
    }
    return 0;
}

void En_Zl3_AfterDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    if (limbIndex == 16) {
        EnXc* this = (EnXc*)thisx;
        Vec3f src = { 0.0f, 10.0f, 0.0f };
        Vec3f dest;

        Matrix_Position(&src, &dest);
        this->actor.focus.pos.x = dest.x;
        this->actor.focus.pos.y = dest.y;
        this->actor.focus.pos.z = dest.z;
        this->actor.focus.rot.x = this->actor.world.rot.x;
        this->actor.focus.rot.y = this->actor.world.rot.y;
        this->actor.focus.rot.z = this->actor.world.rot.z;
    }
}

void En_Oa2_Actor_draw_none(Actor* thisx, PlayState* play) {
}

void En_Oa2_Actor_draw_normal(Actor* thisx, PlayState* play) {
    s32 pad;
    EnXc* this = (EnXc*)thisx;
    s16 eyeIdx = this->eyeIdx;
    void* eyeSegment = en_oa2_eye[eyeIdx];
    SkelAnime* skelAnime = &this->skelAnime;
    GraphicsContext* localGfxCtx = play->state.gfxCtx;
    GraphicsContext* gfxCtx = localGfxCtx;

    OPEN_DISPS(gfxCtx, "../z_en_oA2.c", 1164);
    Actor_HiliteReflect_set_init(&this->actor, play, 0);
    _texture_z_light_fog_prim(gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeSegment));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeSegment));
    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          En_Oa2_BeforeDraw, En_Zl3_AfterDraw, this);
    CLOSE_DISPS(gfxCtx, "../z_en_oA2.c", 1207);
}

void En_Oa2_Actor_draw(Actor* thisx, PlayState* play) {
    static EnXcDrawFunc proc[] = {
        En_Oa2_Actor_draw_none, En_Oa2_Actor_draw_normal,  En_Oa2_Actor_draw_take,
        En_Oa2_Actor_draw_harp,    En_Oa2_Actor_draw_tryforce, En_Oa2_draw_glare,
    };

    EnXc* this = (EnXc*)thisx;

    if (this->drawMode < 0 || this->drawMode > 5 || proc[this->drawMode] == NULL) {
        // "Draw mode is abnormal!!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
    } else {
        proc[this->drawMode](thisx, play);
    }
}

ActorProfile En_Xc_Profile = {
    /**/ ACTOR_EN_XC,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_XC,
    /**/ sizeof(EnXc),
    /**/ En_Oa2_Actor_ct,
    /**/ En_Oa2_Actor_dt,
    /**/ En_Oa2_Actor_main,
    /**/ En_Oa2_Actor_draw,
};
