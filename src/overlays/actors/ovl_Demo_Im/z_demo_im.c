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

void DemoIm_Init(Actor* thisx, PlayState* play);
void DemoIm_Destroy(Actor* thisx, PlayState* play);
void DemoIm_Update(Actor* thisx, PlayState* play);
void DemoIm_Draw(Actor* thisx, PlayState* play);
void func_809856F8(DemoIm* this, PlayState* play);
void func_80985718(DemoIm* this, PlayState* play);
void func_80985738(DemoIm* this, PlayState* play);
void func_80985770(DemoIm* this, PlayState* play);
void func_809857B0(DemoIm* this, PlayState* play);
void func_809857F0(DemoIm* this, PlayState* play);
void func_80985830(DemoIm* this, PlayState* play);
void func_80985C10(DemoIm* this, PlayState* play);
void func_80985C40(DemoIm* this, PlayState* play);
void func_80985C94(DemoIm* this, PlayState* play);
void DemoIm_DrawTranslucent(DemoIm* this, PlayState* play);
void func_809863BC(DemoIm* this, PlayState* play);
void func_809863DC(DemoIm* this, PlayState* play);
void func_80986430(DemoIm* this, PlayState* play);
void func_80986494(DemoIm* this, PlayState* play);
void func_809864D4(DemoIm* this, PlayState* play);
void func_809868E8(DemoIm* this, PlayState* play);
void func_80986908(DemoIm* this, PlayState* play);
void func_80986948(DemoIm* this, PlayState* play);
void func_80986D40(DemoIm* this, PlayState* play);
void func_80986DC8(DemoIm* this, PlayState* play);
void func_80986E20(DemoIm* this, PlayState* play);
void func_80986E40(DemoIm* this, PlayState* play);
void func_80986EAC(DemoIm* this, PlayState* play);
void func_80986F08(DemoIm* this, PlayState* play);
void func_80986F28(DemoIm* this, PlayState* play);
void func_80986F88(DemoIm* this, PlayState* play);
void func_80986FA8(DemoIm* this, PlayState* play);
void func_80987288(DemoIm* this, PlayState* play);
void func_809872A8(DemoIm* this, PlayState* play);
void func_809872F0(DemoIm* this, PlayState* play);
void func_80987330(DemoIm* this, PlayState* play);
void DemoIm_DrawNothing(DemoIm* this, PlayState* play);
void DemoIm_DrawSolid(DemoIm* this, PlayState* play);

static void* sEyeTextures[] = {
    gImpaEyeOpenTex,
    gImpaEyeHalfTex,
    gImpaEyeClosedTex,
};

#if DEBUG_FEATURES
static u32 D_8098783C = 0;
#endif

static ColliderCylinderInitType1 sCylinderInit = {
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

static DemoImActionFunc sActionFuncs[] = {
    func_809856F8, func_80985718, func_80985738, func_80985770, func_809857B0, func_809857F0, func_80985830,
    func_80985C10, func_80985C40, func_80985C94, func_809863BC, func_809863DC, func_80986430, func_80986494,
    func_809864D4, func_809868E8, func_80986908, func_80986948, func_80986D40, func_80986DC8, func_80986E20,
    func_80986E40, func_80986EAC, func_80986F08, func_80986F28, func_80986F88, func_80986FA8, func_80987288,
    func_809872A8, func_809872F0, func_80987330,
};

static Vec3f D_809887D8 = { 0.0f, 10.0f, 0.0f };

static DemoImDrawFunc sDrawFuncs[] = {
    DemoIm_DrawNothing,
    DemoIm_DrawSolid,
    DemoIm_DrawTranslucent,
};

ActorProfile Demo_Im_Profile = {
    /**/ ACTOR_DEMO_IM,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_IM,
    /**/ sizeof(DemoIm),
    /**/ DemoIm_Init,
    /**/ DemoIm_Destroy,
    /**/ DemoIm_Update,
    /**/ DemoIm_Draw,
};

void func_80984BE0(DemoIm* this) {
    s32 pad[3];
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeIndex = &this->eyeIndex;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
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

void DemoIm_InitCollider(Actor* thisx, PlayState* play) {
    DemoIm* this = (DemoIm*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
}

void DemoIm_DestroyCollider(Actor* thisx, PlayState* play) {
    DemoIm* this = (DemoIm*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void DemoIm_UpdateCollider(DemoIm* this, PlayState* play) {
    s32 pad[5];

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void func_80984DB8(DemoIm* this) {
    s32 pad[2];
    Vec3s* headRot = &this->interactInfo.headRot;
    Vec3s* torsoRot = &this->interactInfo.torsoRot;

    Math_SmoothStepToS(&headRot->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&headRot->y, 0, 20, 6200, 100);

    Math_SmoothStepToS(&torsoRot->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&torsoRot->y, 0, 20, 6200, 100);
}

void func_80984E58(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yawDiff;
    s16 trackingMode;

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) + 4.0f;

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    trackingMode = (ABS(yawDiff) < 0x18E3) ? NPC_TRACKING_HEAD_AND_TORSO : NPC_TRACKING_NONE;
    Npc_TrackPoint(&this->actor, &this->interactInfo, kREG(17) + 0xC, trackingMode);
}

void func_80984F10(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) + 12.0f;

    Npc_TrackPoint(&this->actor, &this->interactInfo, kREG(17) + 0xC, NPC_TRACKING_HEAD_AND_TORSO);
}

void func_80984F94(DemoIm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) + 4.0f;
    Npc_TrackPoint(&this->actor, &this->interactInfo, kREG(17) + 0xC, NPC_TRACKING_FULL_BODY);
}

void DemoIm_UpdateBgCheckInfo(DemoIm* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

s32 DemoIm_UpdateSkelAnime(DemoIm* this) {
    return SkelAnime_Update(&this->skelAnime);
}

s32 DemoIm_IsCutsceneIdle(PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        return true;
    } else {
        return false;
    }
}

CsCmdActorCue* DemoIm_GetCue(PlayState* play, s32 cueChannel) {
    s32 pad[2];
    CsCmdActorCue* cue = NULL;

    if (!DemoIm_IsCutsceneIdle(play)) {
        cue = play->csCtx.actorCues[cueChannel];
    }

    return cue;
}

s32 func_809850E8(DemoIm* this, PlayState* play, u16 action, s32 cueChannel) {
    CsCmdActorCue* cue = DemoIm_GetCue(play, cueChannel);

    if (cue != NULL) {
        if (cue->id == action) {
            return true;
        }
    }
    return false;
}

s32 func_80985134(DemoIm* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = DemoIm_GetCue(play, cueChannel);

    if (cue != NULL) {
        if (cue->id != cueId) {
            return true;
        }
    }
    return false;
}

void func_80985180(DemoIm* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = DemoIm_GetCue(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void func_80985200(DemoIm* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = DemoIm_GetCue(play, cueChannel);

    if (cue != NULL) {
        this->actor.world.pos.x = cue->startPos.x;
        this->actor.world.pos.y = cue->startPos.y;
        this->actor.world.pos.z = cue->startPos.z;
        this->actor.world.rot.y = this->actor.shape.rot.y = cue->rot.y;
    }
}

void DemoIm_ChangeAnim(DemoIm* this, AnimationHeader* animHeaderSeg, u8 animMode, f32 morphFrames, s32 playBackwards) {
    f32 frameCount = Animation_GetLastFrame(animHeaderSeg);
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

    Animation_Change(&this->skelAnime, animHeaderSeg, playbackSpeed, startFrame, endFrame, animMode, morphFrames);
}

#include "z_demo_im_inKenjyanoma.inc.c"

#include "z_demo_im_inKenjyanomaDemo02.inc.c"

#include "z_demo_im_inOcarina.inc.c"

#include "z_demo_im_inSpot00.inc.c"

#include "z_demo_im_inWatch.inc.c"

#include "z_demo_im_inEnding.inc.c"

void DemoIm_Update(Actor* thisx, PlayState* play) {
    DemoIm* this = (DemoIm*)thisx;

    if ((this->action < 0) || (this->action >= 31) || (sActionFuncs[this->action] == NULL)) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, play);
}

void DemoIm_Init(Actor* thisx, PlayState* play) {
    DemoIm* this = (DemoIm*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    DemoIm_InitCollider(thisx, play);
    SkelAnime_InitFlex(play, &this->skelAnime, &gImpaSkel, NULL, this->jointTable, this->morphTable, 17);
    thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    switch (this->actor.params) {
        case 2:
            func_80985860(this, play);
            break;
        case 3:
            func_80985E60(this, play);
            break;
        case 4:
            func_8098652C(this, play);
            break;
        case 5:
            func_809869B0(this, play);
            break;
        case 6:
            func_80987018(this, play);
            break;
        default:
            func_80985310(this, play);
    }
}

void DemoIm_Destroy(Actor* thisx, PlayState* play) {
    DemoIm_DestroyCollider(thisx, play);
}

s32 DemoIm_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
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

void DemoIm_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    DemoIm* this = (DemoIm*)thisx;

    if (limbIndex == IMPA_LIMB_HEAD) {
        Vec3f sp28 = D_809887D8;
        Vec3f dest;

        Matrix_MultVec3f(&sp28, &dest);
        this->actor.focus.pos.x = dest.x;
        this->actor.focus.pos.y = dest.y;
        this->actor.focus.pos.z = dest.z;
        this->actor.focus.rot.x = this->actor.world.rot.x;
        this->actor.focus.rot.y = this->actor.world.rot.y;
        this->actor.focus.rot.z = this->actor.world.rot.z;
    }
}

void DemoIm_DrawNothing(DemoIm* this, PlayState* play) {
}

void DemoIm_DrawSolid(DemoIm* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* eyeTexture = sEyeTextures[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_im.c", 904);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          DemoIm_OverrideLimbDraw, DemoIm_PostLimbDraw, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_im.c", 925);
}

void DemoIm_Draw(Actor* thisx, PlayState* play) {
    DemoIm* this = (DemoIm*)thisx;

    if ((this->drawConfig < 0) || (this->drawConfig >= 3) || (sDrawFuncs[this->drawConfig] == NULL)) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawConfig](this, play);
}
