/*
 * File: z_en_nb.c
 * Overlay: ovl_En_Nb
 * Description: Nabooru
 */

#include "z_en_nb.h"
#include "terminal.h"
#include "assets/objects/object_nb/object_nb.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

typedef enum EnNbAction {
    /* 0x00 */ NB_CHAMBER_INIT,
    /* 0x01 */ NB_CHAMBER_UNDERGROUND,
    /* 0x02 */ NB_CHAMBER_APPEAR,
    /* 0x03 */ NB_CHAMBER_IDLE,
    /* 0x04 */ NB_CHAMBER_RAISE_ARM,
    /* 0x05 */ NB_CHAMBER_RAISE_ARM_TRANSITION,
    /* 0x06 */ NB_GIVE_MEDALLION,
    /* 0x07 */ NB_ACTION_7,
    /* 0x08 */ NB_SEAL_HIDE,
    /* 0x09 */ NB_ACTION_9,
    /* 0x0A */ NB_KIDNAPPED,
    /* 0x0B */ NB_KIDNAPPED_LOOK_AROUND,
    /* 0x0C */ NB_PORTAL_FALLTHROUGH,
    /* 0x0D */ NB_IN_CONFRONTATION,
    /* 0x0E */ NB_ACTION_14,
    /* 0x0F */ NB_KNEEL,
    /* 0x10 */ NB_LOOK_RIGHT,
    /* 0x11 */ NB_LOOK_LEFT,
    /* 0x12 */ NB_RUN,
    /* 0x13 */ NB_CONFRONTATION_DESTROYED,
    /* 0x14 */ NB_CREDITS_INIT,
    /* 0x15 */ NB_CREDITS_FADEIN,
    /* 0x16 */ NB_CREDITS_SIT,
    /* 0x17 */ NB_CREDITS_HEAD_TURN,
    /* 0x18 */ NB_CROUCH_CRAWLSPACE,
    /* 0x19 */ NB_NOTICE_PLAYER,
    /* 0x1A */ NB_IDLE_CRAWLSPACE,
    /* 0x1B */ NB_IN_DIALOG,
    /* 0x1C */ NB_IN_PATH,
    /* 0x1D */ NB_IDLE_AFTER_TALK,
    /* 0x1E */ NB_ACTION_30
} EnNbAction;

typedef enum EnNbDrawMode {
    /* 0x00 */ NB_DRAW_NOTHING,
    /* 0x01 */ NB_DRAW_DEFAULT,
    /* 0x02 */ NB_DRAW_HIDE,
    /* 0x03 */ NB_DRAW_KNEEL,
    /* 0x04 */ NB_DRAW_LOOK_DIRECTION
} EnNbDrawMode;

void EnNb_Init(Actor* thisx, PlayState* play);
void EnNb_Destroy(Actor* thisx, PlayState* play);
void EnNb_Update(Actor* thisx, PlayState* play);
void EnNb_Draw(Actor* thisx, PlayState* play);

static ColliderCylinderInitType1 sCylinderInit = {
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

static void* sEyeTextures[] = {
    gNabooruEyeOpenTex,
    gNabooruEyeHalfTex,
    gNabooruEyeClosedTex,
};

#if DEBUG_FEATURES
static s32 D_80AB4318 = 0;
#endif

#include "Demodt_Kenjyanoma.inc.c"

s32 EnNb_GetPath(EnNb* this) {
    s32 path = PARAMS_GET_U(this->actor.params, 8, 8);

    return path;
}

s32 EnNb_GetType(EnNb* this) {
    s32 type = PARAMS_GET_U(this->actor.params, 0, 8);

    return type;
}

void EnNb_UpdatePath(EnNb* this, PlayState* play) {
    Vec3s* pointPos;
    Path* pathList;
    s32 pad;
    s32 path;

    pathList = play->pathList;

    if (pathList != NULL) {
        path = EnNb_GetPath(this);
        pathList += path;
        pointPos = SEGMENTED_TO_VIRTUAL(pathList->points);
        this->initialPos.x = pointPos[0].x;
        this->initialPos.y = pointPos[0].y;
        this->initialPos.z = pointPos[0].z;
        this->finalPos.x = pointPos[1].x;
        this->finalPos.y = pointPos[1].y;
        this->finalPos.z = pointPos[1].z;
        this->pathYaw =
            RAD_TO_BINANG(Math_FAtan2F(this->finalPos.x - this->initialPos.x, this->finalPos.z - this->initialPos.z));
        // "En_Nb_Get_path_info Rail Data Get! = %d!!!!!!!!!!!!!!"
        PRINTF("En_Nb_Get_path_info レールデータをゲットだぜ = %d!!!!!!!!!!!!!!\n", path);
    } else {
        // "En_Nb_Get_path_info Rail Data Doesn't Exist!!!!!!!!!!!!!!!!!!!!"
        PRINTF("En_Nb_Get_path_info レールデータが無い!!!!!!!!!!!!!!!!!!!!\n");
    }
}

void EnNb_SetupCollider(Actor* thisx, PlayState* play) {
    EnNb* this = (EnNb*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, thisx, &sCylinderInit);
}

void EnNb_UpdateCollider(EnNb* this, PlayState* play) {
    s32 pad[4];
    ColliderCylinder* collider = &this->collider;

    Collider_UpdateCylinder(&this->actor, collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &collider->base);
}

void EnNb_Destroy(Actor* thisx, PlayState* play) {
    EnNb* this = (EnNb*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80AB0FBC(EnNb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) + 9.0f;
    Npc_TrackPoint(&this->actor, &this->interactInfo, kREG(17) + 0xC, NPC_TRACKING_HEAD_AND_TORSO);
}

void func_80AB1040(EnNb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) + 9.0f;
    Npc_TrackPoint(&this->actor, &this->interactInfo, kREG(17) + 0xC, NPC_TRACKING_FULL_BODY);
}

void func_80AB10C4(EnNb* this) {
    s32 pad2[2];
    Vec3s* headRot;
    Vec3s* torsoRot;

    headRot = &this->interactInfo.headRot;
    Math_SmoothStepToS(&headRot->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&headRot->y, 0, 20, 6200, 100);
    torsoRot = &this->interactInfo.torsoRot;
    Math_SmoothStepToS(&torsoRot->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&torsoRot->y, 0, 20, 6200, 100);
}

void EnNb_UpdateEyes(EnNb* this) {
    s32 pad[3];
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeIdx = &this->eyeIdx;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }

    *eyeIdx = *blinkTimer;
    if (*eyeIdx >= ARRAY_COUNT(sEyeTextures)) {
        *eyeIdx = 0;
    }
}

#if DEBUG_FEATURES
void func_80AB11EC(EnNb* this) {
    this->action = NB_ACTION_7;
    this->drawMode = NB_DRAW_NOTHING;
    this->alpha = 0;
    this->flag = 0;
    this->actor.shape.shadowAlpha = 0;
    this->alphaTimer = 0.0f;
}

void func_80AB1210(EnNb* this, PlayState* play) {
    s32 one; // required to match

    if (play->csCtx.state == CS_STATE_IDLE) {
        if (D_80AB4318) {
            if (this->actor.params == NB_TYPE_DEMO02) {
                func_80AB11EC(this);
            }

            D_80AB4318 = 0;
        }
    } else {
        one = 1;
        if (!D_80AB4318) {
            D_80AB4318 = one;
        }
    }
}
#endif

void func_80AB1284(EnNb* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_2);
}

s32 EnNb_UpdateSkelAnime(EnNb* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorCue* EnNb_GetCue(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

void EnNb_SetStartPosRotFromCue1(EnNb* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = EnNb_GetCue(play, cueChannel);
    Actor* thisx = &this->actor;

    if (cue != NULL) {
        thisx->world.pos.x = cue->startPos.x;
        thisx->world.pos.y = cue->startPos.y;
        thisx->world.pos.z = cue->startPos.z;

        thisx->world.rot.y = thisx->shape.rot.y = cue->rot.y;
    }
}

s32 func_80AB1390(EnNb* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[cueChannel];

        if ((cue != NULL) && (cue->id == cueId)) {
            return true;
        }
    }

    return false;
}

s32 func_80AB13D8(EnNb* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[cueChannel];

        if ((cue != NULL) && (cue->id != cueId)) {
            return true;
        }
    }

    return false;
}

void EnNb_SetStartPosRotFromCue2(EnNb* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = EnNb_GetCue(play, cueChannel);
    Actor* thisx = &this->actor;

    if (cue != NULL) {
        thisx->world.pos.x = cue->startPos.x;
        thisx->world.pos.y = cue->startPos.y;
        thisx->world.pos.z = cue->startPos.z;

        thisx->world.rot.y = thisx->shape.rot.y = cue->rot.y;
    }
}

void EnNb_SetCurrentAnim(EnNb* this, AnimationHeader* animation, u8 mode, f32 morphFrames, s32 arg4) {
    f32 frameCount = Animation_GetLastFrame(animation);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (!arg4) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        unk0 = frameCount;
        fc = 0.0f;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&this->skelAnime, animation, playbackSpeed, unk0, fc, mode, morphFrames);
}

#include "z_en_nb_inKenjyanoma.inc.c"

#include "z_en_nb_inKenjyanomaDemo02.inc.c"

#include "z_en_nb_inKidnap.inc.c"

#include "z_en_nb_inConfrontion.inc.c"

#include "z_en_nb_inEnding.inc.c"

#include "z_en_nb_inStand.inc.c"

static EnNbActionFunc sActionFuncs[] = {
    EnNb_SetupChamberCs,
    EnNb_SetupChamberWarp,
    EnNb_ComeUp,
    func_80AB193C,
    EnNb_RaiseArm,
    func_80AB19BC,
    func_80AB19FC,
    EnNb_Hide,
    EnNb_Fade,
    EnNb_CreateLightOrb,
    func_80AB23A8,
    EnNb_MovingInPortal,
    EnNb_SuckedInByPortal,
    EnNb_CheckConfrontationCsModeWrapper,
    func_80AB2C18,
    EnNb_Kneel,
    EnNb_LookRight,
    EnNb_LookLeft,
    EnNb_Run,
    EnNb_ConfrontationDestroy,
    EnNb_CheckCreditsCsMode,
    EnNb_CreditsFade,
    func_80AB3428,
    EnNb_LookUp,
    EnNb_WaitForNotice,
    EnNb_StandUpAfterNotice,
    EnNb_BlockCrawlspace,
    EnNb_InitCrawlspaceDialogue,
    EnNb_FollowPath,
    func_80AB3DB0,
    func_80AB3E10,
};

void EnNb_Update(Actor* thisx, PlayState* play) {
    EnNb* this = (EnNb*)thisx;

    if (this->action < 0 || this->action > 30 || sActionFuncs[this->action] == NULL) {
        // "Main mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sActionFuncs[this->action](this, play);
}

void EnNb_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNb* this = (EnNb*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    EnNb_SetupCollider(thisx, play);
    SkelAnime_InitFlex(play, &this->skelAnime, &gNabooruSkel, NULL, this->jointTable, this->morphTable, NB_LIMB_MAX);

    switch (EnNb_GetType(this)) {
        case NB_TYPE_DEMO02:
            EnNb_SetupLightArrowOrSealingCs(this, play);
            break;
        case NB_TYPE_KIDNAPPED:
            EnNb_InitKidnap(this, play);
            break;
        case NB_TYPE_KNUCKLE:
            EnNb_SetupConfrontation(this, play);
            break;
        case NB_TYPE_CREDITS:
            EnNb_SetupCreditsSpawn(this, play);
            break;
        case NB_TYPE_CRAWLSPACE:
            EnNb_CrawlspaceSpawnCheck(this, play);
            break;
        default: // giving medallion
            EnNb_SetChamberAnim(this, play);
            break;
    }
}

s32 EnNb_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnNb* this = (EnNb*)thisx;
    NpcInteractInfo* interactInfo = &this->interactInfo;
    s32 ret = false;

    if (this->headTurnFlag != 0) {
        if (limbIndex == NB_LIMB_TORSO) {
            s32 pad;

            rot->x += interactInfo->torsoRot.y;
            rot->y -= interactInfo->torsoRot.x;
            ret = false;
        } else if (limbIndex == NB_LIMB_HEAD) {
            rot->x += interactInfo->headRot.y;
            rot->z += interactInfo->headRot.x;
            ret = false;
        }
    }

    return ret;
}

void EnNb_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnNb* this = (EnNb*)thisx;

    if (limbIndex == NB_LIMB_HEAD) {
        Vec3f vec1 = { 0.0f, 10.0f, 0.0f };
        Vec3f vec2;

        Matrix_MultVec3f(&vec1, &vec2);
        this->actor.focus.pos.x = vec2.x;
        this->actor.focus.pos.y = vec2.y;
        this->actor.focus.pos.z = vec2.z;
        this->actor.focus.rot.x = this->actor.world.rot.x;
        this->actor.focus.rot.y = this->actor.world.rot.y;
        this->actor.focus.rot.z = this->actor.world.rot.z;
    }
}

void EnNb_DrawNothing(EnNb* this, PlayState* play) {
}

void EnNb_DrawDefault(EnNb* this, PlayState* play) {
    s32 pad;
    s16 eyeIdx = this->eyeIdx;
    SkelAnime* skelAnime = &this->skelAnime;
    void* eyeTexture = sEyeTextures[eyeIdx];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_nb.c", 992);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnNb_OverrideLimbDraw, EnNb_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_nb.c", 1013);
}

static EnNbDrawFunc sDrawFuncs[] = {
    EnNb_DrawNothing, EnNb_DrawDefault, EnNb_DrawTransparency, func_80AB2E70, func_80AB2FE4,
};

void EnNb_Draw(Actor* thisx, PlayState* play) {
    EnNb* this = (EnNb*)thisx;

    if (this->drawMode < 0 || this->drawMode >= 5 || sDrawFuncs[this->drawMode] == NULL) {
        // "Draw mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sDrawFuncs[this->drawMode](this, play);
}

ActorProfile En_Nb_Profile = {
    /**/ ACTOR_EN_NB,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_NB,
    /**/ sizeof(EnNb),
    /**/ EnNb_Init,
    /**/ EnNb_Destroy,
    /**/ EnNb_Update,
    /**/ EnNb_Draw,
};
