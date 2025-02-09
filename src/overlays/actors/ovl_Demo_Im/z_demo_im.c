/*
 * File: z_demo_im.c
 * Overlay: Demo_Im
 * Description: Impa
 */

#include "z_demo_im.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "assets/scenes/indoors/nakaniwa/nakaniwa_scene.h"
#include "assets/objects/object_im/object_im.h"
#include "terminal.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void Demo_Im_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Im_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Im_Actor_main(Actor* thisx, PlayState* play);
void Demo_Im_Actor_draw(Actor* thisx, PlayState* play);
void Demo_Im_Actor_main_wait(DemoIm* this, PlayState* play);
void Demo_Im_Actor_main_hide(DemoIm* this, PlayState* play);
void Demo_Im_Actor_main_up(DemoIm* this, PlayState* play);
void Demo_Im_Actor_main_greet(DemoIm* this, PlayState* play);
void Demo_Im_Actor_main_handup(DemoIm* this, PlayState* play);
void Demo_Im_Actor_main_cheer(DemoIm* this, PlayState* play);
void Demo_Im_Actor_main_stop(DemoIm* this, PlayState* play);
void Demo_Im_Seal_Actor_main_hide(DemoIm* this, PlayState* play);
void Demo_Im_Seal_Actor_main_fade(DemoIm* this, PlayState* play);
void Demo_Im_Seal_Actor_main_pray(DemoIm* this, PlayState* play);
void Demo_Im_Actor_draw_alpha(DemoIm* this, PlayState* play);
void Demo_Im_Ocarina_main_wait(DemoIm* this, PlayState* play);
void Demo_Im_Ocarina_main_greet(DemoIm* this, PlayState* play);
void Demo_Im_Ocarina_main_play(DemoIm* this, PlayState* play);
void Demo_Im_Ocarina_main_stop(DemoIm* this, PlayState* play);
void Demo_Im_Ocarina_main_put(DemoIm* this, PlayState* play);
void Demo_Im_Spot00_main_wait(DemoIm* this, PlayState* play);
void Demo_Im_Spot00_main_greet(DemoIm* this, PlayState* play);
void Demo_Im_Spot00_main_away(DemoIm* this, PlayState* play);
void Demo_Im_Watch_main_wait(DemoIm* this, PlayState* play);
void Demo_Im_Watch_main_stand(DemoIm* this, PlayState* play);
void Demo_Im_Watch_main_wait_warp(DemoIm* this, PlayState* play);
void Demo_Im_Watch_main_stand_warp(DemoIm* this, PlayState* play);
void Demo_Im_Watch_main_branch_warp(DemoIm* this, PlayState* play);
void Demo_Im_Watch_main_wait_ocarina(DemoIm* this, PlayState* play);
void Demo_Im_Watch_main_stand_ocarina(DemoIm* this, PlayState* play);
void Demo_Im_Watch_main_wait_spot00(DemoIm* this, PlayState* play);
void Demo_Im_Watch_main_stand_spot00(DemoIm* this, PlayState* play);
void Demo_Im_inEnding_main_wait(DemoIm* this, PlayState* play);
void Demo_Im_inEnding_main_alpha(DemoIm* this, PlayState* play);
void Demo_Im_inEnding_main_stand(DemoIm* this, PlayState* play);
void Demo_Im_inEnding_main_lookup(DemoIm* this, PlayState* play);
void Demo_Im_Actor_draw_none(DemoIm* this, PlayState* play);
void Demo_Im_Actor_draw_normal(DemoIm* this, PlayState* play);

static void* demo_im_eye[] = {
    gImpaEyeOpenTex,
    gImpaEyeHalfTex,
    gImpaEyeClosedTex,
};

#if DEBUG_FEATURES
static u32 D_8098783C = 0;
#endif

static ColliderCylinderInitType1 Demo_Im_OcInfoData_forStand = {
    {
        COL_MATERIAL_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0x00000000, 0x00, 0x00 }, 0x00, 0x00, 0x01 },
    { 25, 80, 0, { 0, 0, 0 } },
};

#include "Demodt_Kenjyanoma.inc.c"

void Demo_Im_set_eye_pattern(DemoIm* this) {
    s32 pad[3];
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeIndex = &this->eyeIndex;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = get_random_timer(60, 60);
    }

    *eyeIndex = *blinkTimer;
    if (*eyeIndex >= 3) {
        *eyeIndex = 0;
    }
}

#if DEBUG_FEATURES
void func_80984C68(DemoIm* this) {
    this->action = 7;
    this->drawConfig = 0;
    this->alpha = 0;
    this->unk_270 = 0;
    this->actor.shape.shadowAlpha = 0;
    this->unk_268 = 0.0f;
}

void func_80984C8C(DemoIm* this, PlayState* play) {
    u32* something = &D_8098783C;

    if (play->csCtx.state == CS_STATE_IDLE) {
        if (*something != 0) {
            if (this->actor.params == 2) {
                func_80984C68(this);
            }
            *something = 0;
        }
    } else {
        if (*something == 0) {
            *something = 1;
        }
    }
}
#endif

void Demo_Im_ct_forCorect(Actor* thisx, PlayState* play) {
    DemoIm* this = (DemoIm*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set3(play, &this->collider, &this->actor, &Demo_Im_OcInfoData_forStand);
}

void Demo_Im_dt_forCorect(Actor* thisx, PlayState* play) {
    DemoIm* this = (DemoIm*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void Demo_Im_Excute_Corect_forStand(DemoIm* this, PlayState* play) {
    s32 pad[5];

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void Demo_Im_Calc_turn_front(DemoIm* this) {
    s32 pad[2];
    Vec3s* headRot = &this->interactInfo.headRot;
    Vec3s* torsoRot = &this->interactInfo.torsoRot;

    add_calc_short_angle2(&headRot->x, 0, 20, 6200, 100);
    add_calc_short_angle2(&headRot->y, 0, 20, 6200, 100);

    add_calc_short_angle2(&torsoRot->x, 0, 20, 6200, 100);
    add_calc_short_angle2(&torsoRot->y, 0, 20, 6200, 100);
}

void Demo_Im_Calc_turn_link(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;
    s16 trackingMode;

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) + 4.0f;

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    trackingMode = (ABS(yawDiff) < 0x18E3) ? NPC_TRACKING_HEAD_AND_TORSO : NPC_TRACKING_NONE;
    eye_moveM(&this->actor, &this->interactInfo, kREG(17) + 0xC, trackingMode);
}

void Demo_Im_Calc_turn_link2(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) + 12.0f;

    eye_moveM(&this->actor, &this->interactInfo, kREG(17) + 0xC, NPC_TRACKING_HEAD_AND_TORSO);
}

void Demo_Im_Calc_turn_link3(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) + 4.0f;
    eye_moveM(&this->actor, &this->interactInfo, kREG(17) + 0xC, NPC_TRACKING_FULL_BODY);
}

void Demo_Im_BGcheck(DemoIm* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 Demo_Im_Animation_Base(DemoIm* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

s32 Demo_Im_Check_EndDemoMode(PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        return true;
    } else {
        return false;
    }
}

CsCmdActorCue* Demo_Im_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    s32 pad[2];
    CsCmdActorCue* cue = NULL;

    if (!Demo_Im_Check_EndDemoMode(play)) {
        cue = play->csCtx.actorCues[cueChannel];
    }

    return cue;
}

s32 Demo_Im_Check_npcdemopnt(DemoIm* this, PlayState* play, u16 action, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Im_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL) {
        if (cue->id == action) {
            return true;
        }
    }
    return false;
}

s32 Demo_Im_Check2_npcdemopnt(DemoIm* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Im_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL) {
        if (cue->id != cueId) {
            return true;
        }
    }
    return false;
}

void Demo_Im_Set_DemoStartPosAngle(DemoIm* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Im_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void Demo_Im_Set_StartPos_npcdemopnt(DemoIm* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = Demo_Im_Get_npcdemopnt(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void Demo_Im_Change_Anime(DemoIm* this, AnimationHeader* animHeaderSeg, u8 animMode, f32 morphFrames, s32 playBackwards) {
    f32 frameCount = Si2_anime_end_frame(animHeaderSeg);
    f32 playbackSpeed;
    f32 startFrame;
    f32 endFrame;

    if (!playBackwards) {
        startFrame = 0.0f;
        endFrame = frameCount;
        playbackSpeed = 1.0f;
    } else {
        endFrame = 0.0f;
        startFrame = frameCount;
        playbackSpeed = -1.0f;
    }

    Skeleton_Info2_init(&this->skelAnime, animHeaderSeg, playbackSpeed, startFrame, endFrame, animMode, morphFrames);
}

#include "z_demo_im_inKenjyanoma.inc.c"

#include "z_demo_im_inKenjyanomaDemo02.inc.c"

#include "z_demo_im_inOcarina.inc.c"

#include "z_demo_im_inSpot00.inc.c"

#include "z_demo_im_inWatch.inc.c"

#include "z_demo_im_inEnding.inc.c"

void Demo_Im_Actor_main(Actor* thisx, PlayState* play) {
    static DemoImActionFunc proc[] = {
        Demo_Im_Actor_main_wait, Demo_Im_Actor_main_hide, Demo_Im_Actor_main_up, Demo_Im_Actor_main_greet, Demo_Im_Actor_main_handup, Demo_Im_Actor_main_cheer, Demo_Im_Actor_main_stop,
        Demo_Im_Seal_Actor_main_hide, Demo_Im_Seal_Actor_main_fade, Demo_Im_Seal_Actor_main_pray, Demo_Im_Ocarina_main_wait, Demo_Im_Ocarina_main_greet, Demo_Im_Ocarina_main_play, Demo_Im_Ocarina_main_stop,
        Demo_Im_Ocarina_main_put, Demo_Im_Spot00_main_wait, Demo_Im_Spot00_main_greet, Demo_Im_Spot00_main_away, Demo_Im_Watch_main_wait, Demo_Im_Watch_main_stand, Demo_Im_Watch_main_wait_warp,
        Demo_Im_Watch_main_stand_warp, Demo_Im_Watch_main_branch_warp, Demo_Im_Watch_main_wait_ocarina, Demo_Im_Watch_main_stand_ocarina, Demo_Im_Watch_main_wait_spot00, Demo_Im_Watch_main_stand_spot00, Demo_Im_inEnding_main_wait,
        Demo_Im_inEnding_main_alpha, Demo_Im_inEnding_main_stand, Demo_Im_inEnding_main_lookup,
    };

    DemoIm* this = (DemoIm*)thisx;

    if ((this->action < 0) || (this->action >= 31) || (proc[this->action] == NULL)) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->action](this, play);
}

void Demo_Im_Actor_ct(Actor* thisx, PlayState* play) {
    DemoIm* this = (DemoIm*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    Demo_Im_ct_forCorect(thisx, play);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gImpaSkel, NULL, this->jointTable, this->morphTable, 17);
    thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    switch (this->actor.params) {
        case 2:
            Demo_Im_KenjyanomaDemo02_Init(this, play);
            break;
        case 3:
            Demo_Im_Ocarina_Init(this, play);
            break;
        case 4:
            Demo_Im_Spot00_Init(this, play);
            break;
        case 5:
            Demo_Im_Watch_Init(this, play);
            break;
        case 6:
            Demo_Im_Ending_Init(this, play);
            break;
        default:
            Demo_Im_Kenjyanoma_Init(this, play);
    }
}

void Demo_Im_Actor_dt(Actor* thisx, PlayState* play) {
    Demo_Im_dt_forCorect(thisx, play);
}

s32 Demo_Im_before_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    DemoIm* this = (DemoIm*)thisx;
    s32* unk_2D0 = &this->unk_2D0;

    if (this->unk_280 != 0) {
        Vec3s* torsoRot = &this->interactInfo.torsoRot;
        Vec3s* headRot = &this->interactInfo.headRot;

        switch (limbIndex) {
            case IMPA_LIMB_CHEST:
                rot->x += torsoRot->y;
                rot->y -= torsoRot->x;
                break;
            case IMPA_LIMB_HEAD:
                rot->x += headRot->y;
                rot->z += headRot->x;
                break;
        }
    }

    if ((*unk_2D0 != 0) && (limbIndex == IMPA_LIMB_RIGHT_HAND)) {
        *dList = gImpaHandPointingDL;
    }

    return false;
}

void Demo_Im_after_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f D_809887D8 = { 0.0f, 10.0f, 0.0f };

    DemoIm* this = (DemoIm*)thisx;

    if (limbIndex == IMPA_LIMB_HEAD) {
        Vec3f sp28 = D_809887D8;
        Vec3f dest;

        Matrix_Position(&sp28, &dest);
        this->actor.focus.pos.x = dest.x;
        this->actor.focus.pos.y = dest.y;
        this->actor.focus.pos.z = dest.z;
        this->actor.focus.rot.x = this->actor.world.rot.x;
        this->actor.focus.rot.y = this->actor.world.rot.y;
        this->actor.focus.rot.z = this->actor.world.rot.z;
    }
}

void Demo_Im_Actor_draw_none(DemoIm* this, PlayState* play) {
}

void Demo_Im_Actor_draw_normal(DemoIm* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* eyeTexture = demo_im_eye[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_im.c", 904);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);

    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          Demo_Im_before_draw, Demo_Im_after_draw, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_im.c", 925);
}

void Demo_Im_Actor_draw(Actor* thisx, PlayState* play) {
    static DemoImDrawFunc proc[] = {
        Demo_Im_Actor_draw_none,
        Demo_Im_Actor_draw_normal,
        Demo_Im_Actor_draw_alpha,
    };

    DemoIm* this = (DemoIm*)thisx;

    if ((this->drawConfig < 0) || (this->drawConfig >= 3) || (proc[this->drawConfig] == NULL)) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->drawConfig](this, play);
}

ActorProfile Demo_Im_Profile = {
    /**/ ACTOR_DEMO_IM,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_IM,
    /**/ sizeof(DemoIm),
    /**/ Demo_Im_Actor_ct,
    /**/ Demo_Im_Actor_dt,
    /**/ Demo_Im_Actor_main,
    /**/ Demo_Im_Actor_draw,
};
