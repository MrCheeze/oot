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

void En_Nb_Actor_ct(Actor* thisx, PlayState* play);
void En_Nb_Actor_dt(Actor* thisx, PlayState* play);
void En_Nb_Actor_main(Actor* thisx, PlayState* play);
void En_Nb_Actor_draw(Actor* thisx, PlayState* play);

static ColliderCylinderInitType1 En_Nb_OcInfoData_forStand = {
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

static void* en_nb_eye[] = {
    gNabooruEyeOpenTex,
    gNabooruEyeHalfTex,
    gNabooruEyeClosedTex,
};

#if DEBUG_FEATURES
static s32 D_80AB4318 = 0;
#endif

#include "Demodt_Kenjyanoma.inc.c"

s32 En_Nb_GetUpper_arg_data(EnNb* this) {
    s32 path = PARAMS_GET_U(this->actor.params, 8, 8);

    return path;
}

s32 En_Nb_GetLower_arg_data(EnNb* this) {
    s32 type = PARAMS_GET_U(this->actor.params, 0, 8);

    return type;
}

void En_Nb_Init_path_info(EnNb* this, PlayState* play) {
    Vec3s* pointPos;
    Path* pathList;
    s32 pad;
    s32 path;

    pathList = play->pathList;

    if (pathList != NULL) {
        path = En_Nb_GetUpper_arg_data(this);
        pathList += path;
        pointPos = SEGMENTED_TO_VIRTUAL(pathList->points);
        this->initialPos.x = pointPos[0].x;
        this->initialPos.y = pointPos[0].y;
        this->initialPos.z = pointPos[0].z;
        this->finalPos.x = pointPos[1].x;
        this->finalPos.y = pointPos[1].y;
        this->finalPos.z = pointPos[1].z;
        this->pathYaw =
            RAD_TO_BINANG(fatan2(this->finalPos.x - this->initialPos.x, this->finalPos.z - this->initialPos.z));
        // "En_Nb_Get_path_info Rail Data Get! = %d!!!!!!!!!!!!!!"
        PRINTF("En_Nb_Get_path_info レールデータをゲットだぜ = %d!!!!!!!!!!!!!!\n", path);
    } else {
        // "En_Nb_Get_path_info Rail Data Doesn't Exist!!!!!!!!!!!!!!!!!!!!"
        PRINTF("En_Nb_Get_path_info レールデータが無い!!!!!!!!!!!!!!!!!!!!\n");
    }
}

void En_Nb_ct_forCorect(Actor* thisx, PlayState* play) {
    EnNb* this = (EnNb*)thisx;

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set3(play, &this->collider, thisx, &En_Nb_OcInfoData_forStand);
}

void En_Nb_Excute_Corect(EnNb* this, PlayState* play) {
    s32 pad[4];
    ColliderCylinder* collider = &this->collider;

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &collider->base);
}

void En_Nb_Actor_dt(Actor* thisx, PlayState* play) {
    EnNb* this = (EnNb*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

void En_Nb_Calc_turn_link(EnNb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) + 9.0f;
    eye_moveM(&this->actor, &this->interactInfo, kREG(17) + 0xC, NPC_TRACKING_HEAD_AND_TORSO);
}

void En_Nb_Calc_turn_link2(EnNb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->interactInfo.trackPos = player->actor.world.pos;
    this->interactInfo.yOffset = kREG(16) + 9.0f;
    eye_moveM(&this->actor, &this->interactInfo, kREG(17) + 0xC, NPC_TRACKING_FULL_BODY);
}

void En_Nb_Calc_turn_front(EnNb* this) {
    s32 pad2[2];
    Vec3s* headRot;
    Vec3s* torsoRot;

    headRot = &this->interactInfo.headRot;
    add_calc_short_angle2(&headRot->x, 0, 20, 6200, 100);
    add_calc_short_angle2(&headRot->y, 0, 20, 6200, 100);
    torsoRot = &this->interactInfo.torsoRot;
    add_calc_short_angle2(&torsoRot->x, 0, 20, 6200, 100);
    add_calc_short_angle2(&torsoRot->y, 0, 20, 6200, 100);
}

void En_Nb_set_eye_pattern(EnNb* this) {
    s32 pad[3];
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeIdx = &this->eyeIdx;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = get_random_timer(60, 60);
    }

    *eyeIdx = *blinkTimer;
    if (*eyeIdx >= ARRAY_COUNT(en_nb_eye)) {
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

void En_Nb_BGcheck(EnNb* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_2);
}

s32 En_Nb_Animation_Base(EnNb* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

CsCmdActorCue* En_Nb_Get_npcdemopnt(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = play->csCtx.actorCues[cueChannel];

        return cue;
    }

    return NULL;
}

void En_Nb_Set_DemoStartPosAngle(EnNb* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = En_Nb_Get_npcdemopnt(play, cueChannel);
    Actor* thisx = &this->actor;

    if (cue != NULL) {
        thisx->world.pos.x = cue->startPos.x;
        thisx->world.pos.y = cue->startPos.y;
        thisx->world.pos.z = cue->startPos.z;

        thisx->world.rot.y = thisx->shape.rot.y = cue->rot.y;
    }
}

s32 En_Nb_Check_npcdemopnt(EnNb* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[cueChannel];

        if ((cue != NULL) && (cue->id == cueId)) {
            return true;
        }
    }

    return false;
}

s32 En_Nb_Check2_npcdemopnt(EnNb* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[cueChannel];

        if ((cue != NULL) && (cue->id != cueId)) {
            return true;
        }
    }

    return false;
}

void En_Nb_Set_StartPos_npcdemopnt(EnNb* this, PlayState* play, s32 cueChannel) {
    CsCmdActorCue* cue = En_Nb_Get_npcdemopnt(play, cueChannel);
    Actor* thisx = &this->actor;

    if (cue != NULL) {
        thisx->world.pos.x = cue->startPos.x;
        thisx->world.pos.y = cue->startPos.y;
        thisx->world.pos.z = cue->startPos.z;

        thisx->world.rot.y = thisx->shape.rot.y = cue->rot.y;
    }
}

void En_Nb_Change_Anime(EnNb* this, AnimationHeader* animation, u8 mode, f32 morphFrames, s32 arg4) {
    f32 frameCount = Si2_anime_end_frame(animation);
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

    Skeleton_Info2_init(&this->skelAnime, animation, playbackSpeed, unk0, fc, mode, morphFrames);
}

#include "z_en_nb_inKenjyanoma.inc.c"

#include "z_en_nb_inKenjyanomaDemo02.inc.c"

#include "z_en_nb_inKidnap.inc.c"

#include "z_en_nb_inConfrontion.inc.c"

#include "z_en_nb_inEnding.inc.c"

#include "z_en_nb_inStand.inc.c"

void En_Nb_Actor_main(Actor* thisx, PlayState* play) {
    static EnNbActionFunc proc[] = {
        En_Nb_Actor_main_wait,
        En_Nb_Actor_main_hide,
        En_Nb_Actor_main_up,
        En_Nb_Actor_main_greet,
        En_Nb_Actor_main_handup,
        En_Nb_Actor_main_cheer,
        En_Nb_Actor_main_stop,
        En_Nb_Seal_Actor_main_hide,
        En_Nb_Seal_Actor_main_fade,
        En_Nb_Seal_Actor_main_pray,
        En_Nb_Kidnap_Actor_main_hide,
        En_Nb_Kidnap_Actor_main_struggle,
        En_Nb_Kidnap_Actor_main_Fall,
        En_Nb_Confrontion_Actor_main_hide,
        En_Nb_Confrontion_Actor_main_Absence,
        En_Nb_Confrontion_Actor_main_Kneel,
        En_Nb_Confrontion_Actor_main_Turn_right,
        En_Nb_Confrontion_Actor_main_Turn_left,
        En_Nb_Confrontion_Actor_main_Away,
        En_Nb_Confrontion_Actor_main_Disappear,
        En_Nb_inEnding_main_wait,
        En_Nb_inEnding_main_alpha,
        En_Nb_inEnding_main_stand,
        En_Nb_inEnding_main_lookup,
        En_Nb_inStand_main_wait,
        En_Nb_inStand_main_up,
        En_Nb_inStand_main_blocking,
        En_Nb_inStand_main_blocking_greet,
        En_Nb_inStand_main_moving,
        En_Nb_inStand_main_welcome,
        En_Nb_inStand_main_welcome_greet,
    };

    EnNb* this = (EnNb*)thisx;

    if (this->action < 0 || this->action > 30 || proc[this->action] == NULL) {
        // "Main mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    proc[this->action](this, play);
}

void En_Nb_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNb* this = (EnNb*)thisx;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    En_Nb_ct_forCorect(thisx, play);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gNabooruSkel, NULL, this->jointTable, this->morphTable, NB_LIMB_MAX);

    switch (En_Nb_GetLower_arg_data(this)) {
        case NB_TYPE_DEMO02:
            En_Nb_KenjyanomaDemo02_Init(this, play);
            break;
        case NB_TYPE_KIDNAPPED:
            En_Nb_Kidnap_Init(this, play);
            break;
        case NB_TYPE_KNUCKLE:
            En_Nb_Confrontion_Init(this, play);
            break;
        case NB_TYPE_CREDITS:
            En_Nb_Ending_Init(this, play);
            break;
        case NB_TYPE_CRAWLSPACE:
            En_Nb_Stand_Init(this, play);
            break;
        default: // giving medallion
            En_Nb_Kenjyanoma_Init(this, play);
            break;
    }
}

s32 En_Nb_BeforeDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
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

void En_Nb_AfterDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnNb* this = (EnNb*)thisx;

    if (limbIndex == NB_LIMB_HEAD) {
        Vec3f vec1 = { 0.0f, 10.0f, 0.0f };
        Vec3f vec2;

        Matrix_Position(&vec1, &vec2);
        this->actor.focus.pos.x = vec2.x;
        this->actor.focus.pos.y = vec2.y;
        this->actor.focus.pos.z = vec2.z;
        this->actor.focus.rot.x = this->actor.world.rot.x;
        this->actor.focus.rot.y = this->actor.world.rot.y;
        this->actor.focus.rot.z = this->actor.world.rot.z;
    }
}

void En_Nb_Actor_draw_none(EnNb* this, PlayState* play) {
}

void En_Nb_Actor_draw_normal(EnNb* this, PlayState* play) {
    s32 pad;
    s16 eyeIdx = this->eyeIdx;
    SkelAnime* skelAnime = &this->skelAnime;
    void* eyeTexture = en_nb_eye[eyeIdx];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_nb.c", 992);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &Actor_change_render_mode[2]);
    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          En_Nb_BeforeDraw, En_Nb_AfterDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_nb.c", 1013);
}

void En_Nb_Actor_draw(Actor* thisx, PlayState* play) {
    static EnNbDrawFunc proc[] = {
        En_Nb_Actor_draw_none, En_Nb_Actor_draw_normal, En_Nb_Actor_draw_alpha, En_Nb_draw_surprise_eye, En_Nb_draw_surprise_head,
    };

    EnNb* this = (EnNb*)thisx;

    if (this->drawMode < 0 || this->drawMode >= 5 || proc[this->drawMode] == NULL) {
        // "Draw mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    proc[this->drawMode](this, play);
}

ActorProfile En_Nb_Profile = {
    /**/ ACTOR_EN_NB,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_NB,
    /**/ sizeof(EnNb),
    /**/ En_Nb_Actor_ct,
    /**/ En_Nb_Actor_dt,
    /**/ En_Nb_Actor_main,
    /**/ En_Nb_Actor_draw,
};
