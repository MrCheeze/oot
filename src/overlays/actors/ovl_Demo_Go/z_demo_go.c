/*
 * File: z_demo_go.c
 * Overlay: Demo_Go
 * Description: Gorons (Cutscene)
 */

#include "z_demo_go.h"
#include "assets/objects/object_oF1d_map/object_oF1d_map.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void Demo_Go_Actor_ct(Actor* thisx, PlayState* play);
void Demo_Go_Actor_dt(Actor* thisx, PlayState* play);
void Demo_Go_Actor_main(Actor* thisx, PlayState* play);
void Demo_Go_Actor_draw(Actor* thisx, PlayState* play);

void Demo_Go_WaitToHide(DemoGo* this, PlayState* play);
void Demo_Go_Actor_main_wait(DemoGo* this, PlayState* play);
void Demo_Go_Actor_main_hide(DemoGo* this, PlayState* play);
void Demo_Go_Actor_main_fall(DemoGo* this, PlayState* play);
void Demo_Go_Actor_main_land(DemoGo* this, PlayState* play);
void Demo_Go_Actor_main_standup(DemoGo* this, PlayState* play);
void Demo_Go_Actor_main_approach_accel(DemoGo* this, PlayState* play);
void Demo_Go_Actor_main_approach_move(DemoGo* this, PlayState* play);
void Demo_Go_Actor_draw_none(DemoGo* this, PlayState* play);
void Demo_Go_Actor_draw_normal(DemoGo* this, PlayState* play);

static void* demo_go_eye[] = { gGoronCsEyeOpenTex, gGoronCsEyeHalfTex, gGoronCsEyeClosedTex };

s32 Demo_Go_Get_npcdemopnt_index(DemoGo* this) {
    s32 ret;

    switch (this->actor.params) {
        case 0:
            ret = 3;
            break;

        case 1:
            ret = 4;
            break;

        default:
            if (1) {
                ret = 5;
            }
            break;
    }

    return ret;
}

void Demo_Go_Set_AppearSound(DemoGo* this, PlayState* play) {
    Actor* thisx = &this->actor;
    Vec3f sp20;
    f32 sp1C;

    if ((thisx->params == 0) || (thisx->params == 1)) {
        Skin_Matrix_PrjMulVector(&play->viewProjectionMtxF, &thisx->world.pos, &sp20, &sp1C);
        Effect_SE_Info_new(play, &sp20, 20, NA_SE_EV_OBJECT_FALL);
    }
}

void Demo_Go_Actor_dt(Actor* thisx, PlayState* play) {
    DemoGo* this = (DemoGo*)thisx;

    Skeleton_Info_dt(&this->skelAnime, play);
}

void Demo_Go_set_eye_pattern(DemoGo* this) {
    s16* something = &this->unk_192;
    s16* other = &this->unk_190;
    s32 pad[3];

    if (DECR(*something) == 0) {
        *something = get_random_timer(60, 60);
    }
    *other = *something;
    if (*other >= 3) {
        *other = 0;
    }
}

void Demo_Go_Cryst_Set_LandSound(DemoGo* this) {
    Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EN_DODO_M_GND);
}

void Demo_Go_Cryst_Set_WalkSound(DemoGo* this) {
    s32 pad[2];

    if (Skeleton_Info_frame_check(&this->skelAnime, 12.0f) || Skeleton_Info_frame_check(&this->skelAnime, 25.0f)) {
        Na_StartObjectSe_F(&this->actor.projectedPos, NA_SE_EN_MORIBLIN_WALK);
    }
}

void Demo_Go_BGcheck(DemoGo* this, PlayState* play) {
    Actor_BGcheck2(play, &this->actor, 75.0f, 30.0f, 30.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
}

void Birth_Dust_In_Demo_Go_Cryst_Land(DemoGo* this, PlayState* play) {
    s16 pad;
    Vec3f vec = this->actor.world.pos;
    dust_fly_set2(play, &vec, kREG(11) + 100.0f, kREG(12) + 0xA, kREG(13) + 0x12C, kREG(14), 0);
    Demo_Go_Cryst_Set_LandSound(this);
}

void Demo_Go_Cryst_Movement_Fall(DemoGo* this, PlayState* play) {
    Actor* thisx = &this->actor;
    PosRot* world = &thisx->world;
    CutsceneContext* csCtx = &play->csCtx;
    CsCmdActorCue* cue;
    f32 lerp;
    s32 pad;
    Vec3f startPos;
    Vec3f endPos;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = csCtx->actorCues[Demo_Go_Get_npcdemopnt_index(this)];

        if (cue != NULL) {
            lerp = get_parcent(cue->endFrame, cue->startFrame, csCtx->curFrame);

            startPos.x = cue->startPos.x;
            startPos.y = cue->startPos.y;
            startPos.z = cue->startPos.z;

            endPos.x = cue->endPos.x;
            endPos.y = cue->endPos.y;
            endPos.z = cue->endPos.z;

            world->pos.x = (endPos.x - startPos.x) * lerp + startPos.x;
            world->pos.y = (endPos.y - startPos.y) * lerp + startPos.y;
            world->pos.z = (endPos.z - startPos.z) * lerp + startPos.z;

            world->rot.y = thisx->shape.rot.y = cue->rot.y;
        }
    }
}

void Demo_Go_Movement_Approach_accel(DemoGo* this) {
    f32 something = this->unk_19C;

    if (something < 8.0f) {
        this->actor.speed = (((kREG(15) * 0.01f) + 1.2f) / 8.0f) * something;
    } else {
        this->actor.speed = (kREG(15) * 0.01f) + 1.2f;
    }
    Actor_position_moveF(&this->actor);
}

void Demo_Go_Movement_Approach_move(DemoGo* this) {
    Actor_position_moveF(&this->actor);
}

void Demo_Go_SetAngle_Approach_move(DemoGo* this, PlayState* play) {
    CsCmdActorCue* cue;
    Actor* thisx = &this->actor;
    s32 rotYDelta;
    s32 newRotY;
    s32 thisRotY;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.actorCues[Demo_Go_Get_npcdemopnt_index(this)];

        if (cue != NULL) {
            thisRotY = thisx->world.rot.y;
            rotYDelta = (s16)cue->rot.y - thisRotY;

            if ((rotYDelta > -(kREG(16) + 0x96)) && (rotYDelta < kREG(16) + 0x96)) {
                newRotY = (s16)cue->rot.y;
            } else if (rotYDelta > 0) {
                newRotY = (thisRotY + kREG(16)) + 0x96;
            } else {
                newRotY = (thisRotY - kREG(16)) - 0x96;
            }

            thisx->shape.rot.y = newRotY;
            thisx->world.rot.y = newRotY;
        }
    }
}

s32 Demo_Go_Animation_Base(DemoGo* this) {
    return Skeleton_Info2_anime_play(&this->skelAnime);
}

s32 Demo_Go_Check_npcdemopnt(DemoGo* this, PlayState* play, u16 cueId) {
    CutsceneContext* csCtx = &play->csCtx;
    s32 cueChannel = Demo_Go_Get_npcdemopnt_index(this);

    if (csCtx->state != CS_STATE_IDLE) {
        CsCmdActorCue* cue = csCtx->actorCues[cueChannel];

        if (cue != NULL && cue->id == cueId) {
            return true;
        }
    }

    return false;
}

void Demo_Go_WaitToHide(DemoGo* this, PlayState* play) {
    this->action = 1;
}

void Demo_Go_HideToFall(DemoGo* this, PlayState* play) {
    if (Demo_Go_Check_npcdemopnt(this, play, 2)) {
        this->action = 2;
        this->drawConfig = 1;
        Demo_Go_Cryst_Movement_Fall(this, play);
        Demo_Go_Set_AppearSound(this, play);
    }
}

void Demo_Go_FallToLand(DemoGo* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = csCtx->actorCues[Demo_Go_Get_npcdemopnt_index(this)];

        if (cue != NULL && csCtx->curFrame >= cue->endFrame) {
            Birth_Dust_In_Demo_Go_Cryst_Land(this, play);
            this->action = 3;
        }
    }
}

void Demo_Go_LandToStandup(DemoGo* this, PlayState* play) {
    if (Demo_Go_Check_npcdemopnt(this, play, 3)) {
        this->action = 4;
    }
}

void Demo_Go_StandupToApproach_accel(DemoGo* this, PlayState* play, s32 arg2) {
    AnimationHeader* animation = &gGoronAnim_0029A8;
    if (arg2 != 0) {
        Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_LOOP,
                         -8.0f);
        this->action = 5;
        this->unk_19C = 0.0f;
    }
}

void Demo_Go_Approach_accelToApproach_move(DemoGo* this) {
    this->unk_19C += 1.0f;
    if (this->unk_19C >= 8.0f) {
        this->action = 6;
    }
}

void Demo_Go_Actor_main_wait(DemoGo* this, PlayState* play) {
    Demo_Go_WaitToHide(this, play);
}

void Demo_Go_Actor_main_hide(DemoGo* this, PlayState* play) {
    Demo_Go_HideToFall(this, play);
}

void Demo_Go_Actor_main_fall(DemoGo* this, PlayState* play) {
    Demo_Go_Cryst_Movement_Fall(this, play);
    Demo_Go_BGcheck(this, play);
    Demo_Go_FallToLand(this, play);
}

void Demo_Go_Actor_main_land(DemoGo* this, PlayState* play) {
    Demo_Go_BGcheck(this, play);
    Demo_Go_LandToStandup(this, play);
}

void Demo_Go_Actor_main_standup(DemoGo* this, PlayState* play) {
    s32 something;

    Demo_Go_BGcheck(this, play);
    something = Demo_Go_Animation_Base(this);
    Demo_Go_set_eye_pattern(this);
    Demo_Go_StandupToApproach_accel(this, play, something);
}

void Demo_Go_Actor_main_approach_accel(DemoGo* this, PlayState* play) {
    Demo_Go_SetAngle_Approach_move(this, play);
    Demo_Go_Movement_Approach_move(this);
    Demo_Go_BGcheck(this, play);
    Demo_Go_Animation_Base(this);
    Demo_Go_set_eye_pattern(this);
    Demo_Go_Cryst_Set_WalkSound(this);
    Demo_Go_Approach_accelToApproach_move(this);
}
void Demo_Go_Actor_main_approach_move(DemoGo* this, PlayState* play) {
    Demo_Go_SetAngle_Approach_move(this, play);
    Demo_Go_Movement_Approach_accel(this);
    Demo_Go_BGcheck(this, play);
    Demo_Go_Animation_Base(this);
    Demo_Go_set_eye_pattern(this);
    Demo_Go_Cryst_Set_WalkSound(this);
}

void Demo_Go_Actor_main(Actor* thisx, PlayState* play) {
    static DemoGoActionFunc proc[] = {
        Demo_Go_Actor_main_wait, Demo_Go_Actor_main_hide, Demo_Go_Actor_main_fall, Demo_Go_Actor_main_land, Demo_Go_Actor_main_standup, Demo_Go_Actor_main_approach_accel, Demo_Go_Actor_main_approach_move,
    };

    DemoGo* this = (DemoGo*)thisx;

    if (this->action < 0 || this->action >= 7 || proc[this->action] == NULL) {
        PRINTF(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->action](this, play);
}

void Demo_Go_Actor_ct(Actor* thisx, PlayState* play) {
    DemoGo* this = (DemoGo*)thisx;
    AnimationHeader* animation = &gGoronAnim_004930;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 30.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGoronSkel, NULL, NULL, NULL, 0);
    Skeleton_Info2_init(&this->skelAnime, animation, 1.0f, 0.0f, Si2_anime_end_frame(animation), ANIMMODE_ONCE, 0.0f);
    this->action = 0;
}

void Demo_Go_Actor_draw_none(DemoGo* this, PlayState* play) {
}

void Demo_Go_Actor_draw_normal(DemoGo* this, PlayState* play) {
    s32 pad;
    s16 eyeTexIdx = this->unk_190;
    SkelAnime* skelAnime = &this->skelAnime;
    void* eyeTexture = demo_go_eye[eyeTexIdx];
    s32 pad2;

    OPEN_DISPS(play->state.gfxCtx, "../z_demo_go.c", 732);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gGoronCsMouthSmileTex));

    Si2_draw_SV(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_demo_go.c", 746);
}

void Demo_Go_Actor_draw(Actor* thisx, PlayState* play) {
    static DemoGoDrawFunc proc[] = {
        Demo_Go_Actor_draw_none,
        Demo_Go_Actor_draw_normal,
    };

    DemoGo* this = (DemoGo*)thisx;

    if (this->drawConfig < 0 || this->drawConfig >= 2 || proc[this->drawConfig] == NULL) {
        PRINTF(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    proc[this->drawConfig](this, play);
}

ActorProfile Demo_Go_Profile = {
    /**/ ACTOR_DEMO_GO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OF1D_MAP,
    /**/ sizeof(DemoGo),
    /**/ Demo_Go_Actor_ct,
    /**/ Demo_Go_Actor_dt,
    /**/ Demo_Go_Actor_main,
    /**/ Demo_Go_Actor_draw,
};
