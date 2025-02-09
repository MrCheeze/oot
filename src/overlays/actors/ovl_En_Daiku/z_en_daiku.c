#include "z_en_daiku.h"
#include "overlays/actors/ovl_En_GeldB/z_en_geldb.h"
#include "assets/objects/object_daiku/object_daiku.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define ENDAIKU_GET_TYPE(thisx) PARAMS_GET_U((thisx)->params, 0, 2)

typedef struct EnDaikuEscapeSubCamParam {
    Vec3f eyePosDeltaLocal;
    s32 maxFramesActive;
} EnDaikuEscapeSubCamParam;

// state flags

// probably related to animating torso and head to look towards the player
#define ENDAIKU_STATEFLAG_1 (1 << 1)
// same
#define ENDAIKU_STATEFLAG_2 (1 << 2)
// the gerudo guard appeared (after talking to the carpenter)
#define ENDAIKU_STATEFLAG_GERUDOFIGHTING (1 << 3)
// the gerudo guard was defeated
#define ENDAIKU_STATEFLAG_GERUDODEFEATED (1 << 4)

typedef enum EnDaikuTalkState {
    /* 0 */ ENDAIKU_STATE_CAN_TALK,
    /* 2 */ ENDAIKU_STATE_TALKING = 2,
    /* 3 */ ENDAIKU_STATE_NO_TALK
} EnDaikuTalkState;

void En_Daiku_Actor_ct(Actor* thisx, PlayState* play);
void En_Daiku_Actor_dt(Actor* thisx, PlayState* play);
void En_Daiku_Actor_move(Actor* thisx, PlayState* play);
void En_Daiku_Actor_draw(Actor* thisx, PlayState* play);

void move_mode_wait2(EnDaiku* this, PlayState* play);
static void move_mode_wait(EnDaiku* this, PlayState* play);
void move_mode_help(EnDaiku* this, PlayState* play);
void move_mode_escape_init(EnDaiku* this, PlayState* play);
void move_mode_escape_init2(EnDaiku* this, PlayState* play);
void func_run_camera_init(EnDaiku* this, PlayState* play);
void move_mode_escape(EnDaiku* this, PlayState* play);
static s32 func_before_display(PlayState* play, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
static void after_display(PlayState* play, s32 limb, Gfx** dList, Vec3s* rot, void* thisx);

ActorProfile En_Daiku_Profile = {
    /**/ ACTOR_EN_DAIKU,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DAIKU,
    /**/ sizeof(EnDaiku),
    /**/ En_Daiku_Actor_ct,
    /**/ En_Daiku_Actor_dt,
    /**/ En_Daiku_Actor_move,
    /**/ En_Daiku_Actor_draw,
};

static ColliderCylinderInit atinfodata = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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
    { 18, 66, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 statusdata = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable btldata = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

typedef enum EnDaikuAnimation {
    /* 0 */ ENDAIKU_ANIM_SHOUT,
    /* 1 */ ENDAIKU_ANIM_STAND,
    /* 2 */ ENDAIKU_ANIM_CELEBRATE,
    /* 3 */ ENDAIKU_ANIM_RUN,
    /* 4 */ ENDAIKU_ANIM_SIT
} EnDaikuAnimation;

static AnimationFrameCountInfo anime_ct_data[] = {
    { &object_daiku_Anim_001AB0, 1.0f, 0, 0 }, { &object_daiku_Anim_007DE0, 1.0f, 0, 0 },
    { &object_daiku_Anim_00885C, 1.0f, 0, 0 }, { &object_daiku_Anim_000C44, 1.0f, 0, 0 },
    { &object_daiku_Anim_008164, 1.0f, 0, 0 },
};

static EnDaikuEscapeSubCamParam escape_camera_info[] = {
    { { 0, 130, 220 }, 100 },
    { { -20, 22, 280 }, 110 },
    { { 50, 180, 350 }, 100 },
    { { -40, 60, 60 }, 120 },
};

static void daiku_anime_ct(EnDaiku* this, s32 index, s32* currentIndex) {
    f32 morphFrames;

    if (*currentIndex < 0 || *currentIndex == index) {
        morphFrames = 0.0f;
    } else {
        morphFrames = anime_ct_data[index].morphFrames;
    }

    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, 1.0f, 0.0f,
                     Si2_anime_end_frame(anime_ct_data[index].animation), anime_ct_data[index].mode, morphFrames);

    *currentIndex = index;
}

void En_Daiku_Actor_ct(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;
    s32 pad;
    s32 noKill = true;
    s32 isRescued = false;

    if (ENDAIKU_GET_TYPE(&this->actor) == ENDAIKU_TYPE0 && GET_EVENTCHKINF(EVENTCHKINF_CARPENTER_0_RESCUED)) {
        isRescued = true;
    } else if (ENDAIKU_GET_TYPE(&this->actor) == ENDAIKU_TYPE1 && GET_EVENTCHKINF(EVENTCHKINF_CARPENTER_1_RESCUED)) {
        isRescued = true;
    } else if (ENDAIKU_GET_TYPE(&this->actor) == ENDAIKU_TYPE2 && GET_EVENTCHKINF(EVENTCHKINF_CARPENTER_2_RESCUED)) {
        isRescued = true;
    } else if (ENDAIKU_GET_TYPE(&this->actor) == ENDAIKU_TYPE3 && GET_EVENTCHKINF(EVENTCHKINF_CARPENTER_3_RESCUED)) {
        isRescued = true;
    }

    if (isRescued == true && play->sceneId == SCENE_THIEVES_HIDEOUT) {
        noKill = false;
    } else if (!isRescued && play->sceneId == SCENE_CARPENTERS_TENT) {
        noKill = false;
    }

    this->startFightSwitchFlag = this->actor.shape.rot.z & 0x3F;
    this->actor.shape.rot.z = 0;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 40.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_daiku_Skel_007958, NULL, this->jointTable, this->morphTable, 17);

    if (!noKill) {
        Actor_delete(&this->actor);
        return;
    }

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &atinfodata);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, &btldata, &statusdata);

    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[ENDAIKU_ANIM_SHOUT].animation, 1.0f, 0.0f,
                     Si2_anime_end_frame(anime_ct_data[ENDAIKU_ANIM_SHOUT].animation),
                     anime_ct_data[ENDAIKU_ANIM_SHOUT].mode, anime_ct_data[ENDAIKU_ANIM_SHOUT].morphFrames);

    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->currentAnimIndex = -1;
    this->runSpeed = 5.0f;
    this->initRot = this->actor.world.rot;
    this->initPos = this->actor.world.pos;

    if (play->sceneId == SCENE_THIEVES_HIDEOUT) {
        daiku_anime_ct(this, ENDAIKU_ANIM_STAND, &this->currentAnimIndex);
        this->stateFlags |= ENDAIKU_STATEFLAG_1 | ENDAIKU_STATEFLAG_2;
        this->actionFunc = move_mode_wait;
    } else {
        if (ENDAIKU_GET_TYPE(&this->actor) == ENDAIKU_TYPE1 || ENDAIKU_GET_TYPE(&this->actor) == ENDAIKU_TYPE3) {
            daiku_anime_ct(this, ENDAIKU_ANIM_SIT, &this->currentAnimIndex);
            this->stateFlags |= ENDAIKU_STATEFLAG_1;
        } else {
            daiku_anime_ct(this, ENDAIKU_ANIM_SHOUT, &this->currentAnimIndex);
            this->stateFlags |= ENDAIKU_STATEFLAG_1 | ENDAIKU_STATEFLAG_2;
        }

        this->skelAnime.curFrame = (s32)(fqrand() * this->skelAnime.endFrame);
        this->actionFunc = move_mode_wait2;
    }
}

void En_Daiku_Actor_dt(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static s32 func_talk_endmsg_chk(EnDaiku* this, PlayState* play) {
    s32 newTalkState = ENDAIKU_STATE_TALKING;

    if (message_check(&play->msgCtx) == TEXT_STATE_DONE) {
        if (play->sceneId == SCENE_THIEVES_HIDEOUT) {
            if (pad_on_check(play)) {
                if (this->actor.textId == 0x6007) {
                    Actor_Environment_sw_On(play, this->startFightSwitchFlag);
                    newTalkState = ENDAIKU_STATE_CAN_TALK;
                } else {
                    this->actionFunc = move_mode_escape_init;
                    newTalkState = ENDAIKU_STATE_NO_TALK;
                }
            }
        } else if (play->sceneId == SCENE_CARPENTERS_TENT) {
            if (pad_on_check(play)) {
                switch (this->actor.textId) {
                    case 0x6061:
                        SET_INFTABLE(INFTABLE_176);
                        break;
                    case 0x6064:
                        SET_INFTABLE(INFTABLE_178);
                        break;
                }

                newTalkState = ENDAIKU_STATE_CAN_TALK;
            }
        }
    }

    return newTalkState;
}

static void func_talk(EnDaiku* this, PlayState* play) {
    s32 carpenterType;
    s32 rescuedCount;
    s16 sp2E;
    s16 sp2C;

    if (this->talkState == ENDAIKU_STATE_TALKING) {
        this->talkState = func_talk_endmsg_chk(this, play);
    } else if (Actor_talk_check(&this->actor, play)) {
        this->talkState = ENDAIKU_STATE_TALKING;
    } else {
        Actor_display_position_set(play, &this->actor, &sp2E, &sp2C);
        if (sp2E >= 0 && sp2E <= 320 && sp2C >= 0 && sp2C <= 240 && this->talkState == ENDAIKU_STATE_CAN_TALK &&
            Actor_talk_request2(&this->actor, play, 100.0f) == 1) {
            if (play->sceneId == SCENE_THIEVES_HIDEOUT) {
                if (this->stateFlags & ENDAIKU_STATEFLAG_GERUDODEFEATED) {
                    rescuedCount = 0;
                    for (carpenterType = 0; carpenterType < 4; carpenterType++) {
                        if (ENDAIKU_IS_CARPENTER_RESCUED(carpenterType)) {
                            rescuedCount++;
                        }
                    }

                    switch (rescuedCount) {
                        case 0:
                            this->actor.textId = 0x605B;
                            break;
                        case 1:
                            this->actor.textId = 0x605C;
                            break;
                        case 2:
                            this->actor.textId = 0x605D;
                            break;
                        case 3:
                            this->actor.textId = 0x605E;
                            break;
                    }
                } else if (!(this->stateFlags &
                             (ENDAIKU_STATEFLAG_GERUDOFIGHTING | ENDAIKU_STATEFLAG_GERUDODEFEATED))) {
                    this->actor.textId = 0x6007;
                }
            } else if (play->sceneId == SCENE_CARPENTERS_TENT) {
                switch (ENDAIKU_GET_TYPE(&this->actor)) {
                    case ENDAIKU_TYPE0:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            this->actor.textId = 0x6060;
                        } else {
                            this->actor.textId = 0x605F;
                        }
                        break;
                    case ENDAIKU_TYPE1:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            this->actor.textId = 0x6063;
                        } else {
                            if (!GET_INFTABLE(INFTABLE_176)) {
                                this->actor.textId = 0x6061;
                            } else {
                                this->actor.textId = 0x6062;
                            }
                        }
                        break;
                    case ENDAIKU_TYPE2:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            this->actor.textId = 0x6066;
                        } else {
                            if (!GET_INFTABLE(INFTABLE_178)) {
                                this->actor.textId = 0x6064;
                            } else {
                                this->actor.textId = 0x6065;
                            }
                        }
                        break;
                    case ENDAIKU_TYPE3:
                        if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
                            this->actor.textId = 0x6068;
                        } else {
                            this->actor.textId = 0x6067;
                        }
                        break;
                }
            }
        }
    }
}

/**
 * The carpenter is idling in the tent.
 */
void move_mode_wait2(EnDaiku* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    func_talk(this, play);
}

/**
 * The carpenter is jailed in a Gerudo fortress cell, talking to him starts a fight against a gerudo guard
 */
static void move_mode_wait(EnDaiku* this, PlayState* play) {
    EnGeldB* gerudo;
    s32 temp_t9;
    s32 temp_v1;

    if (!(this->stateFlags & ENDAIKU_STATEFLAG_GERUDOFIGHTING)) {
        func_talk(this, play);
    }
    Skeleton_Info2_anime_play(&this->skelAnime);

    gerudo = (EnGeldB*)Actor_info_name_search(&play->actorCtx, ACTOR_EN_GELDB, ACTORCAT_ENEMY);
    if (gerudo == NULL) {
        this->stateFlags |= ENDAIKU_STATEFLAG_GERUDODEFEATED;
        this->stateFlags &= ~ENDAIKU_STATEFLAG_GERUDOFIGHTING;
        daiku_anime_ct(this, ENDAIKU_ANIM_CELEBRATE, &this->currentAnimIndex);
        this->actionFunc = move_mode_help;
    } else if (!(this->stateFlags & ENDAIKU_STATEFLAG_GERUDOFIGHTING) && !gerudo->invisible) {
        this->stateFlags |= ENDAIKU_STATEFLAG_GERUDOFIGHTING;
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    }
}

/**
 * The player defeated the gerudo guard and the carpenter is waiting for the cell door to be opened, and for the player
 * to then talk to him
 */
void move_mode_help(EnDaiku* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);

    if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->actor.params, 8, 6))) {
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        func_talk(this, play);
    }
}

/**
 * The carpenter is rescued, initializes his running away animation
 */
void move_mode_escape_init(EnDaiku* this, PlayState* play) {
    Path* path;
    f32 dxz;
    f32 dx;
    f32 dz;
    Vec3s* pointPos;
    s32 exitLoop;

    Na_StartFanfare(NA_BGM_APPEAR);
    daiku_anime_ct(this, ENDAIKU_ANIM_RUN, &this->currentAnimIndex);
    this->stateFlags &= ~(ENDAIKU_STATEFLAG_1 | ENDAIKU_STATEFLAG_2);

    ENDAIKU_SET_CARPENTER_RESCUED(ENDAIKU_GET_TYPE(&this->actor));

    this->actor.gravity = -1.0f;
    this->escapeSubCamTimer = escape_camera_info[ENDAIKU_GET_TYPE(&this->actor)].maxFramesActive;
    func_run_camera_init(this, play);

    exitLoop = false;
    path = &play->pathList[PARAMS_GET_U(this->actor.params, 4, 4)];
    while (!exitLoop) {
        pointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->waypoint;
        dx = pointPos->x - this->actor.world.pos.x;
        dz = pointPos->z - this->actor.world.pos.z;
        this->rotYtowardsPath = RAD_TO_BINANG(fatan2(dx, dz));
        dxz = sqrtf(SQ(dx) + SQ(dz));
        if (dxz > 10.0f) {
            exitLoop = true;
        } else {
            this->waypoint++;
        }
    }

    this->actionFunc = move_mode_escape_init2;
}

/**
 * The carpenter is rotating towards where he is going next
 */
void move_mode_escape_init2(EnDaiku* this, PlayState* play) {
    s16 diff;

    diff = add_calc_short_angle2(&this->actor.shape.rot.y, this->rotYtowardsPath, 1, 0x1388, 0);
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (diff == 0) {
        this->actionFunc = move_mode_escape;
        this->actionFunc(this, play);
    }
}

void func_run_camera_init(EnDaiku* this, PlayState* play) {
    s32 pad;
    Vec3f eyePosDeltaLocal;
    Vec3f eyePosDeltaWorld;

    this->subCamActive = true;
    this->escapeSubCamTimer = escape_camera_info[ENDAIKU_GET_TYPE(&this->actor)].maxFramesActive;

    eyePosDeltaLocal.x = escape_camera_info[ENDAIKU_GET_TYPE(&this->actor)].eyePosDeltaLocal.x;
    eyePosDeltaLocal.y = escape_camera_info[ENDAIKU_GET_TYPE(&this->actor)].eyePosDeltaLocal.y;
    eyePosDeltaLocal.z = escape_camera_info[ENDAIKU_GET_TYPE(&this->actor)].eyePosDeltaLocal.z;
    Matrix_rotateY(BINANG_TO_RAD(this->actor.world.rot.y), MTXMODE_NEW);
    Matrix_Position(&eyePosDeltaLocal, &eyePosDeltaWorld);

    this->subCamEyeInit.x = this->subCamEye.x = this->actor.world.pos.x + eyePosDeltaWorld.x;
    this->subCamEyeInit.y = this->subCamEye.y = this->actor.world.pos.y + eyePosDeltaWorld.y;
    if (1) {}
    this->subCamEyeInit.z = this->subCamEye.z = this->actor.world.pos.z + eyePosDeltaWorld.z;

    if (1) {}
    this->subCamAtNext.x = this->subCamAt.x = this->actor.world.pos.x;
    this->subCamAtNext.y = this->subCamAt.y = this->actor.world.pos.y + 60.0f;
    if (1) {}
    this->subCamAtNext.z = this->subCamAt.z = this->actor.world.pos.z;

    this->subCamId = Gama_play_make_camera(play);
    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
    Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);

    Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    Gama_play_set_camera_fovy(play, this->subCamId, play->mainCamera.fov);
    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_1);
}

void func_run_camera(EnDaiku* this, PlayState* play) {
    s32 pad;

    this->subCamAtNext.x = this->actor.world.pos.x;
    this->subCamAtNext.y = this->actor.world.pos.y + 60.0f;
    this->subCamAtNext.z = this->actor.world.pos.z;

    add_calc(&this->subCamAt.x, this->subCamAtNext.x, 1.0f, 1000.0f, 0.0f);
    add_calc(&this->subCamAt.y, this->subCamAtNext.y, 1.0f, 1000.0f, 0.0f);
    add_calc(&this->subCamAt.z, this->subCamAtNext.z, 1.0f, 1000.0f, 0.0f);

    Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
}

void func_run_camera_ed(EnDaiku* this, PlayState* play) {
    static Vec3f mtx_src = { 0.0f, 0.0f, 120.0f };

    Gama_play_clear_camera(play, this->subCamId);
    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_ACTIVE);
    this->subCamActive = false;

    if (GET_EVENTCHKINF_CARPENTERS_ALL_RESCUED()) {
        Actor* gerudoGuard;
        Vec3f vec;

        Matrix_rotateY(BINANG_TO_RAD(this->initRot.y), MTXMODE_NEW);
        Matrix_Position(&mtx_src, &vec);
        gerudoGuard = Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_GE3, this->initPos.x + vec.x, this->initPos.y + vec.y,
                                  this->initPos.z + vec.z, 0, RAD_TO_BINANG(fatan2(-vec.x, -vec.z)), 0, 2);

        if (gerudoGuard == NULL) {
            Actor_delete(&this->actor);
        }
        return;
    }

    player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
}

/**
 * The carpenter is running away
 */
void move_mode_escape(EnDaiku* this, PlayState* play) {
    s32 pad1;
    Path* path;
    s16 ry;
    f32 dx;
    f32 dz;
    s32 pad2;
    f32 dxz;
    Vec3s* pointPos;

    path = &play->pathList[PARAMS_GET_U(this->actor.params, 4, 4)];
    pointPos = (Vec3s*)SEGMENTED_TO_VIRTUAL(path->points) + this->waypoint;
    dx = pointPos->x - this->actor.world.pos.x;
    dz = pointPos->z - this->actor.world.pos.z;
    ry = RAD_TO_BINANG(fatan2(dx, dz));
    dxz = sqrtf(SQ(dx) + SQ(dz));
    if (dxz <= 20.88f) {
        this->waypoint++;
        if (this->waypoint >= path->count) {
            if (this->subCamActive) {
                func_run_camera_ed(this, play);
            }
            Actor_delete(&this->actor);
            return;
        }
    }

    add_calc_short_angle2(&this->actor.shape.rot.y, ry, 1, 0xFA0, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    add_calc(&this->actor.speed, this->runSpeed, 0.6f, dxz, 0.0f);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

    if (this->subCamActive) {
        func_run_camera(this, play);
        if (this->escapeSubCamTimer-- <= 0) {
            func_run_camera_ed(this, play);
        }
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
}

void En_Daiku_Actor_move(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;
    s32 curFrame;
    Player* player = GET_PLAYER(play);

    if (this->currentAnimIndex == ENDAIKU_ANIM_RUN) {
        curFrame = this->skelAnime.curFrame;
        if (curFrame == 6 || curFrame == 15) {
            Actor_SE_set(&this->actor, NA_SE_EN_MORIBLIN_WALK);
        }
    }

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

    this->actionFunc(this, play);

    if (this->stateFlags & ENDAIKU_STATEFLAG_1) {
        this->interactInfo.trackPos.x = player->actor.focus.pos.x;
        this->interactInfo.trackPos.y = player->actor.focus.pos.y;
        this->interactInfo.trackPos.z = player->actor.focus.pos.z;

        if (this->stateFlags & ENDAIKU_STATEFLAG_2) {
            eye_moveM(&this->actor, &this->interactInfo, 0, NPC_TRACKING_FULL_BODY);
        } else {
            eye_moveM(&this->actor, &this->interactInfo, 0, NPC_TRACKING_HEAD_AND_TORSO);
        }
    }
}

void En_Daiku_Actor_draw(Actor* thisx, PlayState* play) {
    EnDaiku* this = (EnDaiku*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_daiku.c", 1227);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    if (ENDAIKU_GET_TYPE(thisx) == ENDAIKU_TYPE0) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 10, 70, 255);
    } else if (ENDAIKU_GET_TYPE(thisx) == ENDAIKU_TYPE1) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 200, 255, 255);
    } else if (ENDAIKU_GET_TYPE(thisx) == ENDAIKU_TYPE2) {
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 230, 70, 255);
    } else if (ENDAIKU_GET_TYPE(thisx) == ENDAIKU_TYPE3) {
        gDPSetEnvColor(POLY_OPA_DISP++, 200, 0, 150, 255);
    }

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          func_before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_daiku.c", 1255);
}

static s32 func_before_display(PlayState* play, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDaiku* this = (EnDaiku*)thisx;

    switch (limb) {
        case 8: // torso
            rot->x += this->interactInfo.torsoRot.y;
            rot->y -= this->interactInfo.torsoRot.x;
            break;
        case 15: // head
            rot->x += this->interactInfo.headRot.y;
            rot->z += this->interactInfo.headRot.x;
            break;
    }

    return false;
}

static void after_display(PlayState* play, s32 limb, Gfx** dList, Vec3s* rot, void* thisx) {
    static Gfx* zura[] = { object_daiku_DL_005BD0, object_daiku_DL_005AC0, object_daiku_DL_005990,
                                 object_daiku_DL_005880 };
    static Vec3f pos = { 700, 1100, 0 };
    EnDaiku* this = (EnDaiku*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_daiku.c", 1323);

    if (limb == 15) { // head
        Matrix_Position(&pos, &this->actor.focus.pos);
        gSPDisplayList(POLY_OPA_DISP++, zura[ENDAIKU_GET_TYPE(&this->actor)]);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_daiku.c", 1330);
}
