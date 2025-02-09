/*
 * File: z_en_daiku_kakariko.c
 * Overlay: ovl_En_Daiku_Kakariko
 * Description: Kakariko Village Carpenters
 */

#include "z_en_daiku_kakariko.h"
#include "assets/objects/object_daiku/object_daiku.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

typedef enum KakarikoCarpenterType {
    /* 0x0 */ CARPENTER_ICHIRO,  // Red and purple pants, normal hair
    /* 0x1 */ CARPENTER_SABOORO, // Light blue pants
    /* 0x2 */ CARPENTER_JIRO,    // Green pants
    /* 0x3 */ CARPENTER_SHIRO    // Pink and purple pants, two-spiked hair
} KakarikoCarpenterType;

void En_Daiku_Kakariko_Actor_ct(Actor* thisx, PlayState* play);
void En_Daiku_Kakariko_Actor_dt(Actor* thisx, PlayState* play);
void En_Daiku_Kakariko_Actor_move(Actor* thisx, PlayState* play);
void En_Daiku_Kakariko_Actor_draw(Actor* thisx, PlayState* play);

static void move_mode_wait(EnDaikuKakariko* this, PlayState* play);
void move_mode_wonder(EnDaikuKakariko* this, PlayState* play);

ActorProfile En_Daiku_Kakariko_Profile = {
    /**/ ACTOR_EN_DAIKU_KAKARIKO,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DAIKU,
    /**/ sizeof(EnDaikuKakariko),
    /**/ En_Daiku_Kakariko_Actor_ct,
    /**/ En_Daiku_Kakariko_Actor_dt,
    /**/ En_Daiku_Kakariko_Actor_move,
    /**/ En_Daiku_Kakariko_Actor_draw,
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

typedef enum EnDaikuKakarikoAnimation {
    /* 0 */ ENDAIKUKAKARIKO_ANIM_0,
    /* 1 */ ENDAIKUKAKARIKO_ANIM_1,
    /* 2 */ ENDAIKUKAKARIKO_ANIM_2,
    /* 3 */ ENDAIKUKAKARIKO_ANIM_3,
    /* 4 */ ENDAIKUKAKARIKO_ANIM_4,
    /* 5 */ ENDAIKUKAKARIKO_ANIM_5
} EnDaikuKakarikoAnimation;

static AnimationFrameCountInfo anime_ct_data[] = {
    { &object_daiku_Anim_001AB0, 1.0f, 2, -7.0f }, { &object_daiku_Anim_007DE0, 1.0f, 0, -7.0f },
    { &object_daiku_Anim_00885C, 1.0f, 0, -7.0f }, { &object_daiku_Anim_000C44, 1.0f, 0, -7.0f },
    { &object_daiku_Anim_000600, 1.0f, 0, -7.0f }, { &object_daiku_Anim_008164, 1.0f, 0, -7.0f },
};

static void daiku_anime_ct(EnDaikuKakariko* this, s32 index, s32* currentIndex) {
    f32 morphFrames;

    if ((*currentIndex < 0) || (index == *currentIndex)) {
        morphFrames = 0.0f;
    } else {
        morphFrames = anime_ct_data[index].morphFrames;
    }

    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, 1.0f, 0.0f,
                     Si2_anime_end_frame(anime_ct_data[index].animation), anime_ct_data[index].mode, morphFrames);

    *currentIndex = index;
}

void En_Daiku_Kakariko_Actor_ct(Actor* thisx, PlayState* play) {
    static u16 stat_ct_set[] = { 0x0080, 0x00B0, 0x0070, 0x0470 }; // List of initial values for this->flags
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;
    s32 pad;

    if (LINK_AGE_IN_YEARS == YEARS_CHILD) {
        switch (play->sceneId) {
            case SCENE_KAKARIKO_VILLAGE:
                if (IS_DAY) {
                    this->flags |= 1;
                    this->flags |= stat_ct_set[PARAMS_GET_U(this->actor.params, 0, 2)];
                }
                break;
            case SCENE_KAKARIKO_CENTER_GUEST_HOUSE:
                if (IS_NIGHT) {
                    this->flags |= 2;
                }
                break;
            case SCENE_POTION_SHOP_KAKARIKO:
                this->flags |= 4;
                break;
        }
    }

    if (!(this->flags & 7)) {
        Actor_delete(&this->actor);
    }

    if (IS_NIGHT) {
        this->flags |= 8;
    }

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 40.0f);

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &object_daiku_Skel_007958, NULL, this->jointTable, this->morphTable, 17);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &atinfodata);

    CollisionCheck_Status_set3(&this->actor.colChkInfo, &btldata, &statusdata);

    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[ENDAIKUKAKARIKO_ANIM_0].animation, 1.0f, 0.0f,
                     Si2_anime_end_frame(anime_ct_data[ENDAIKUKAKARIKO_ANIM_0].animation),
                     anime_ct_data[ENDAIKUKAKARIKO_ANIM_0].mode, anime_ct_data[ENDAIKUKAKARIKO_ANIM_0].morphFrames);

    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

    this->actor.gravity = 0.0f;
    this->runSpeed = 3.0f;
    this->actor.cullingVolumeDistance = 1200.0f;
    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->currentAnimIndex = -1;

    if (this->flags & 0x40) {
        this->actor.gravity = -1.0f;
    }

    if (this->flags & 0x10) {
        daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_3, &this->currentAnimIndex);
        this->actionFunc = move_mode_wonder;
    } else {
        if (this->flags & 8) {
            if ((PARAMS_GET_U(this->actor.params, 0, 2) == CARPENTER_SABOORO) ||
                (PARAMS_GET_U(this->actor.params, 0, 2) == CARPENTER_SHIRO)) {
                daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_5, &this->currentAnimIndex);
                this->flags |= 0x800;
            } else {
                daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_1, &this->currentAnimIndex);
            }

            this->skelAnime.curFrame = (s32)(fqrand() * this->skelAnime.endFrame);
        } else {
            daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_0, &this->currentAnimIndex);
            this->skelAnime.curFrame = (s32)(fqrand() * this->skelAnime.endFrame);
        }

        this->flags |= 0x100;
        this->actionFunc = move_mode_wait;
    }
}

void En_Daiku_Kakariko_Actor_dt(Actor* thisx, PlayState* play) {
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static s32 func_talk_endmsg_chk(EnDaikuKakariko* this, PlayState* play) {
    s32 talkState = 2;

    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        switch (this->actor.textId) {
            case 0x6061:
                SET_INFTABLE(INFTABLE_176);
                break;
            case 0x6064:
                SET_INFTABLE(INFTABLE_178);
                break;
        }
        talkState = 0;
    }
    return talkState;
}

static void func_talk(EnDaikuKakariko* this, PlayState* play) {
    static s32 msk_msg_id[] = {
        MASK_REACTION_SET_CARPENTER_1,
        MASK_REACTION_SET_CARPENTER_2,
        MASK_REACTION_SET_CARPENTER_3,
        MASK_REACTION_SET_CARPENTER_4,
    };
    s16 sp26;
    s16 sp24;

    if (this->talkState == 2) {
        this->talkState = func_talk_endmsg_chk(this, play);
    } else if (Actor_talk_check(&this->actor, play)) {
        this->talkState = 2;
    } else {
        Actor_display_position_set(play, &this->actor, &sp26, &sp24);

        if ((sp26 >= 0) && (sp26 <= 320) && (sp24 >= 0) && (sp24 <= 240) && (this->talkState == 0) &&
            (Actor_talk_request2(&this->actor, play, 100.0f) == 1)) {
            this->actor.textId =
                get_mask_message(play, msk_msg_id[PARAMS_GET_U(this->actor.params, 0, 2)]);

            if (this->actor.textId == 0) {
                switch (PARAMS_GET_U(this->actor.params, 0, 2)) {
                    case 0:
                        if (this->flags & 8) {
                            this->actor.textId = 0x5076;
                        } else {
                            this->actor.textId = 0x5075;
                        }
                        break;
                    case 1:
                        if (this->flags & 1) {
                            this->actor.textId = 0x502A;
                        } else {
                            this->actor.textId = 0x5074;
                        }
                        break;
                    case 2:
                        if (this->flags & 1) {
                            this->actor.textId = 0x506A;
                        } else {
                            this->actor.textId = 0x506B;
                        }
                        break;
                    case 3:
                        if (this->flags & 1) {
                            this->actor.textId = 0x5077;
                        } else {
                            this->actor.textId = 0x5078;
                        }
                        break;
                }
            }
        }
    }
}

void move_mode_wait_talk(EnDaikuKakariko* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_3, &this->currentAnimIndex);
    }

    func_talk(this, play);

    if (this->talkState == 0) {
        if (this->flags & 0x10) {
            daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_3, &this->currentAnimIndex);
            this->flags &= ~0x0300;
            this->actionFunc = move_mode_wonder;
            return;
        }

        if (!(this->flags & 8)) {
            daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_0, &this->currentAnimIndex);
        }

        if ((this->flags & 0x800) == 0) {
            this->flags &= ~0x0200;
            this->flags |= 0x100;
        }

        this->actionFunc = move_mode_wait;
    }
}

static void move_mode_wait(EnDaikuKakariko* this, PlayState* play) {
    func_talk(this, play);

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_0, &this->currentAnimIndex);
    }

    if (this->talkState != 0) {
        if (!(this->flags & 8)) {
            daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_4, &this->currentAnimIndex);
        }

        if (!(this->flags & 0x800)) {
            this->flags |= 0x200;
            this->flags &= ~0x0100;
        }

        this->actionFunc = move_mode_wait_talk;
    }
}

void move_mode_rest(EnDaikuKakariko* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->timer--;

        if (this->timer <= 0) {
            daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_3, &this->currentAnimIndex);
            this->actionFunc = move_mode_wonder;
        } else {
            this->skelAnime.curFrame = this->skelAnime.startFrame;
        }
    }

    func_talk(this, play);

    if (this->talkState != 0) {
        this->flags |= 0x200;
        daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_4, &this->currentAnimIndex);
        this->actionFunc = move_mode_wait_talk;
    }
}

void move_mode_wonder(EnDaikuKakariko* this, PlayState* play) {
    s32 pad;
    Path* path;
    Vec3s* pathPos;
    f32 xDist;
    f32 zDist;
    s16 runAngle;
    f32 runDist;
    s16 angleStepDiff;
    s32 run;

    do {
        path = &play->pathList[PARAMS_GET_U(this->actor.params, 8, 8)];
        pathPos = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[this->waypoint];
        xDist = pathPos->x - this->actor.world.pos.x;
        zDist = pathPos->z - this->actor.world.pos.z;
        runAngle = RAD_TO_BINANG(fatan2(xDist, zDist));
        runDist = sqrtf((xDist * xDist) + (zDist * zDist));

        run = false;

        if (runDist <= 10.0f) {
            if (!this->pathContinue) {
                this->waypoint++;

                if (this->waypoint >= path->count) {
                    if (this->flags & 0x20) {
                        this->waypoint = path->count - 2;
                        this->pathContinue = true;
                        this->run = run = false;

                        if (this->flags & 0x400) {
                            this->timer = 2;
                            daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_0, &this->currentAnimIndex);
                            this->actionFunc = move_mode_rest;
                            return;
                        }
                    } else {
                        this->waypoint = 0;
                        run = true;
                    }
                } else {
                    this->run = run = true;
                }
            } else {
                this->waypoint--;

                if (this->waypoint < 0) {
                    this->waypoint = 1;
                    this->pathContinue = false;
                    this->run = run = false;

                    if (this->flags & 0x400) {
                        this->timer = 2;
                        daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_0, &this->currentAnimIndex);
                        this->actionFunc = move_mode_rest;
                        return;
                    }
                } else {
                    run = true;
                }
            }
        }
    } while (run);

    angleStepDiff = add_calc_short_angle2(&this->actor.shape.rot.y, runAngle, 1, 5000, 0);

    this->actor.world.rot.y = this->actor.shape.rot.y;

    if (!this->run) {
        if (angleStepDiff == 0) {
            this->run = true;
        } else {
            this->actor.speed = 0.0f;
        }
    }

    if (this->run == true) {
        add_calc(&this->actor.speed, this->runSpeed, 0.8f, runDist, 0.0f);
    }

    Actor_position_moveF(&this->actor);

    if (this->flags & 0x40) {
        Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    } else if (this->flags & 0x80) {
        this->runFlag |= 1;
        this->flags &= ~0x0080;
    } else if (this->runFlag & 1) {
        Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
        this->runFlag &= ~1;
    }

    Skeleton_Info2_anime_play(&this->skelAnime);
    func_talk(this, play);

    if (this->talkState != 0) {
        this->flags |= 0x200;
        daiku_anime_ct(this, ENDAIKUKAKARIKO_ANIM_4, &this->currentAnimIndex);
        this->actionFunc = move_mode_wait_talk;
    }
}

void En_Daiku_Kakariko_Actor_move(Actor* thisx, PlayState* play) {
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 pad2;

    if (this->currentAnimIndex == 3) {
        if (((s32)this->skelAnime.curFrame == 6) || ((s32)this->skelAnime.curFrame == 15)) {
            Actor_SE_set(&this->actor, NA_SE_EN_MORIBLIN_WALK);
        }
    }

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);

    if (this->flags & 4) {
        this->collider.dim.pos.x -= 27;
        this->collider.dim.pos.z -= 27;
        this->collider.dim.radius = 63;
    }

    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

    this->actionFunc(this, play);

    this->interactInfo.trackPos.x = player->actor.focus.pos.x;
    this->interactInfo.trackPos.y = player->actor.focus.pos.y;
    this->interactInfo.trackPos.z = player->actor.focus.pos.z;

    if (this->flags & 0x100) {
        this->neckAngleTarget.x = 5900;
        this->flags |= 0x1000;
        eye_moveM(&this->actor, &this->interactInfo, 0, NPC_TRACKING_HEAD_AND_TORSO);
    } else if (this->flags & 0x200) {
        this->neckAngleTarget.x = 5900;
        this->flags |= 0x1000;
        eye_moveM(&this->actor, &this->interactInfo, 0, NPC_TRACKING_FULL_BODY);
    }

    add_calc_short_angle2(&this->neckAngle.x, this->neckAngleTarget.x, 1, 1820, 0);
}

static s32 func_before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;
    Vec3s angle;

    switch (limbIndex) {
        case 8:
            angle = this->interactInfo.torsoRot;
            Matrix_rotateX(-BINANG_TO_RAD(angle.y), MTXMODE_APPLY);
            Matrix_rotateZ(-BINANG_TO_RAD(angle.x), MTXMODE_APPLY);
            break;
        case 15:
            Matrix_translate(1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            angle = this->interactInfo.headRot;

            if (this->flags & 0x1000) {
                PRINTF("<%d>\n", this->neckAngle.x);
                Matrix_rotateX(BINANG_TO_RAD(angle.y + this->neckAngle.y), MTXMODE_APPLY);
                Matrix_rotateZ(BINANG_TO_RAD(angle.x + this->neckAngle.x), MTXMODE_APPLY);
            } else {
                Matrix_rotateX(BINANG_TO_RAD(angle.y), MTXMODE_APPLY);
                Matrix_rotateZ(BINANG_TO_RAD(angle.x), MTXMODE_APPLY);
            }

            Matrix_translate(-1400.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            break;
    }

    return 0;
}

static void func_after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Gfx* zura[] = { object_daiku_DL_005BD0, object_daiku_DL_005AC0, object_daiku_DL_005990,
                                          object_daiku_DL_005880 };
    static Vec3f pos = { 700.0f, 1100.0f, 0.0f };
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_daiku_kakariko.c", 1104);

    if (limbIndex == 15) {
        Matrix_Position(&pos, &this->actor.focus.pos);
        gSPDisplayList(POLY_OPA_DISP++, zura[PARAMS_GET_U(this->actor.params, 0, 2)]);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_daiku_kakariko.c", 1113);
}

void En_Daiku_Kakariko_Actor_draw(Actor* thisx, PlayState* play) {
    EnDaikuKakariko* this = (EnDaikuKakariko*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_daiku_kakariko.c", 1124);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    if (PARAMS_GET_U(thisx->params, 0, 2) == CARPENTER_ICHIRO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 10, 70, 255);
    } else if (PARAMS_GET_U(thisx->params, 0, 2) == CARPENTER_SABOORO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 170, 200, 255, 255);
    } else if (PARAMS_GET_U(thisx->params, 0, 2) == CARPENTER_JIRO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 230, 70, 255);
    } else if (PARAMS_GET_U(thisx->params, 0, 2) == CARPENTER_SHIRO) {
        gDPSetEnvColor(POLY_OPA_DISP++, 200, 0, 150, 255);
    }

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          func_before_display, func_after_display, thisx);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_daiku_kakariko.c", 1151);
}
