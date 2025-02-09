/*
 * File: z_en_mm.c
 * Overlay: ovl_En_Mm
 * Description: Running Man (child)
 */

#include "z_en_mm.h"
#include "assets/objects/object_mm/object_mm.h"
#include "assets/objects/object_link_child/object_link_child.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

typedef enum RunningManAnimIndex {
    /* 0 */ RM_ANIM_RUN,
    /* 1 */ RM_ANIM_SIT,
    /* 2 */ RM_ANIM_SIT_WAIT,
    /* 3 */ RM_ANIM_STAND,
    /* 4 */ RM_ANIM_SPRINT,
    /* 5 */ RM_ANIM_EXCITED, // plays when talking to him with bunny hood on
    /* 6 */ RM_ANIM_HAPPY    // plays when you sell him the bunny hood
} RunningManAnimIndex;

typedef enum RunningManMouthTex {
    /* 0 */ RM_MOUTH_CLOSED,
    /* 1 */ RM_MOUTH_OPEN
} RunningManMouthTex;

void En_Mm_Actor_ct(Actor* thisx, PlayState* play);
void En_Mm_Actor_dt(Actor* thisx, PlayState* play);
void En_Mm_Actor_move(Actor* thisx, PlayState* play);
void En_Mm_Actor_draw(Actor* thisx, PlayState* play);

static void mode_move_sit(EnMm* this, PlayState* play);
static void mode_move_run(EnMm* this, PlayState* play);
static void mode_move_sit2run(EnMm* this, PlayState* play);
void mode_move_run2sit(EnMm* this, PlayState* play);
s32 func_runtime_check(void);

static s32 func_before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
static void func_after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void*);

ActorProfile En_Mm_Profile = {
    /**/ ACTOR_EN_MM,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MM,
    /**/ sizeof(EnMm),
    /**/ En_Mm_Actor_ct,
    /**/ En_Mm_Actor_dt,
    /**/ En_Mm_Actor_move,
    /**/ En_Mm_Actor_draw,
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
    { 18, 63, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 statusdata[] = { 0, 0, 0, 0, MASS_IMMOVABLE };

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

static AnimationSpeedInfo anime_ct_data[] = {
    { &gRunningManRunAnim, 1.0f, ANIMMODE_LOOP, -7.0f },     { &gRunningManSitStandAnim, -1.0f, ANIMMODE_ONCE, -7.0f },
    { &gRunningManSitWaitAnim, 1.0f, ANIMMODE_LOOP, -7.0f }, { &gRunningManSitStandAnim, 1.0f, ANIMMODE_ONCE, -7.0f },
    { &gRunningManSprintAnim, 1.0f, ANIMMODE_LOOP, -7.0f },  { &gRunningManExcitedAnim, 1.0f, ANIMMODE_LOOP, -12.0f },
    { &gRunningManHappyAnim, 1.0f, ANIMMODE_LOOP, -12.0f },
};

typedef struct EnMmPathInfo {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
} EnMmPathInfo;

static EnMmPathInfo rail_info[] = {
    { 0, 1, 0, 0 },
    { 1, 1, 0, 1 },
    { 1, 3, 2, 1 },
    { -1, 0, 2, 0 },
};

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_STOP),
};

void mm_anime_ct(EnMm* this, s32 index, s32* currentIndex) {
    f32 morphFrames;

    if ((*currentIndex < 0) || (index == *currentIndex)) {
        morphFrames = 0.0f;
    } else {
        morphFrames = anime_ct_data[index].morphFrames;
    }

    if (anime_ct_data[index].playSpeed >= 0.0f) {
        Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, anime_ct_data[index].playSpeed, 0.0f,
                         Si2_anime_end_frame(anime_ct_data[index].animation), anime_ct_data[index].mode,
                         morphFrames);
    } else {
        Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, anime_ct_data[index].playSpeed,
                         Si2_anime_end_frame(anime_ct_data[index].animation), 0.0f, anime_ct_data[index].mode,
                         morphFrames);
    }

    *currentIndex = index;
}

void En_Mm_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMm* this = (EnMm*)thisx;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 21.0f);

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gRunningManSkel, NULL, this->jointTable, this->morphTable, 16);

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &atinfodata);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, &btldata, statusdata);

    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[RM_ANIM_RUN].animation, 1.0f, 0.0f,
                     Si2_anime_end_frame(anime_ct_data[RM_ANIM_RUN].animation), anime_ct_data[RM_ANIM_RUN].mode,
                     anime_ct_data[RM_ANIM_RUN].morphFrames);

    this->path = PARAMS_GET_U(this->actor.params, 0, 8);
    this->unk_1F0 = 2;
    this->unk_1E8 = 0;
    this->actor.attentionRangeType = ATTENTION_RANGE_2;
    this->actor.gravity = -1.0f;
    this->speedXZ = 3.0f;
    this->unk_204 = this->actor.objectSlot;

    if (func_runtime_check() == 1) {
        this->mouthTexIndex = RM_MOUTH_OPEN;
        mm_anime_ct(this, RM_ANIM_RUN, &this->curAnimIndex);
        this->actionFunc = mode_move_sit;
    } else {
        this->mouthTexIndex = RM_MOUTH_CLOSED;
        mm_anime_ct(this, RM_ANIM_SIT_WAIT, &this->curAnimIndex);
        this->actionFunc = mode_move_run;
    }
}

void En_Mm_Actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMm* this = (EnMm*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

s32 func_runtime_check(void) {
    s32 isDay = false;

    if ((z_common_data.save.dayTime > CLOCK_TIME(5, 0)) && (z_common_data.save.dayTime <= CLOCK_TIME(20, 0) + 1)) {
        isDay = true;
    }

    return isDay;
}

static s32 func_endmsg_chk(EnMm* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 sp1C = 1;

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
            break;
        case TEXT_STATE_CHOICE:
            if (pad_on_check(play)) {
                if (play->msgCtx.choiceIndex == 0) {
                    player->actor.textId = 0x202D;
                    this->unk_254 &= ~1;
                    mm_anime_ct(this, RM_ANIM_HAPPY, &this->curAnimIndex);
                } else {
                    player->actor.textId = 0x202C;
                    SET_INFTABLE(INFTABLE_17C);
                }
                sp1C = 2;
            }
            break;
        case TEXT_STATE_EVENT:
            if (pad_on_check(play)) {
                mask_cancel(play);
                item_get_setting(play, ITEM_SOLD_OUT);
                SET_ITEMGETINF(ITEMGETINF_3B);
                lupy_increase(500);
                player->actor.textId = 0x202E;
                sp1C = 2;
            }
            break;
        case TEXT_STATE_DONE:
            if (pad_on_check(play)) {
                if ((player->actor.textId == 0x202E) || (player->actor.textId == 0x202C)) {
                    this->unk_254 |= 1;
                    mm_anime_ct(this, RM_ANIM_SIT_WAIT, &this->curAnimIndex);
                }
                sp1C = 0;
            }
            break;
    }

    return sp1C;
}

static s32 func_set_msg(EnMm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 textId = get_mask_message(play, MASK_REACTION_SET_RUNNING_MAN);

    if (GET_ITEMGETINF(ITEMGETINF_3B)) {
        if (textId == 0) {
            textId = 0x204D;
        }
    } else if (player->currentMask == PLAYER_MASK_BUNNY) {
        textId = GET_INFTABLE(INFTABLE_17C) ? 0x202B : 0x202A;
    } else if (textId == 0) {
        textId = 0x2029;
    }

    return textId;
}

static void func_talk(EnMm* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 yawDiff;
    s16 sp26;
    s16 sp24;

    if (this->unk_1E0 == 2) {
        message_set2(play, player->actor.textId);
        this->unk_1E0 = 1;
    } else if (this->unk_1E0 == 1) {
        this->unk_1E0 = func_endmsg_chk(this, play);
    } else {
        if (Actor_talk_check(&this->actor, play)) {
            this->unk_1E0 = 1;

            if (this->curAnimIndex != 5) {
                if ((this->actor.textId == 0x202A) || (this->actor.textId == 0x202B)) {
                    mm_anime_ct(this, RM_ANIM_EXCITED, &this->curAnimIndex);
                    Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
                }
            }
        } else {
            Actor_display_position_set(play, &this->actor, &sp26, &sp24);
            yawDiff = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

            if ((sp26 >= 0) && (sp26 <= 0x140) && (sp24 >= 0) && (sp24 <= 0xF0) && (yawDiff <= 17152.0f) &&
                (this->unk_1E0 != 3) && Actor_talk_request2(&this->actor, play, 100.0f)) {
                this->actor.textId = func_set_msg(this, play);
            }
        }
    }
}

static s32 func_get_rail_number(Path* pathList, s32 pathNum) {
    return (pathList + pathNum)->count;
}

static s32 func_get_rail_pos(Path* pathList, Vec3f* pos, s32 pathNum, s32 waypoint) {
    Vec3s* pointPos;
    pointPos = &((Vec3s*)SEGMENTED_TO_VIRTUAL((pathList + pathNum)->points))[waypoint];

    pos->x = pointPos->x;
    pos->y = pointPos->y;
    pos->z = pointPos->z;

    return 0;
}

s32 func_rail_run(EnMm* this, PlayState* play) {
    f32 xDiff;
    f32 zDiff;
    Vec3f waypointPos;
    s32 phi_a2;
    s32 phi_v1;

    func_get_rail_pos(play->pathList, &waypointPos, this->path, this->waypoint);

    xDiff = waypointPos.x - this->actor.world.pos.x;
    zDiff = waypointPos.z - this->actor.world.pos.z;

    this->yawToWaypoint = RAD_TO_BINANG2(fatan2(xDiff, zDiff));
    this->distToWaypoint = sqrtf(SQ(xDiff) + SQ(zDiff));

    while ((this->distToWaypoint <= 10.44f) && (this->unk_1E8 != 0)) {
        this->waypoint += rail_info[this->unk_1E8].unk_00;

        phi_a2 = rail_info[this->unk_1E8].unk_08;

        switch (phi_a2) {
            case 0:
                phi_a2 = 0;
                break;
            case 1:
                phi_a2 = func_get_rail_number(play->pathList, this->path) - 1;
                break;
            case 2:
                phi_a2 = this->unk_1F0;
                break;
        }

        phi_v1 = rail_info[this->unk_1E8].unk_0C;

        switch (phi_v1) {
            case 0:
                phi_v1 = 0;
                break;
            case 1:
                phi_v1 = func_get_rail_number(play->pathList, this->path) - 1;
                break;
            case 2:
                phi_v1 = this->unk_1F0;
                break;
        }

        if ((rail_info[this->unk_1E8].unk_00 >= 0 && (this->waypoint < phi_a2 || phi_v1 < this->waypoint)) ||
            (rail_info[this->unk_1E8].unk_00 < 0 && (phi_a2 < this->waypoint || this->waypoint < phi_v1))) {
            this->unk_1E8 = rail_info[this->unk_1E8].unk_04;
            this->waypoint = rail_info[this->unk_1E8].unk_08;
        }

        func_get_rail_pos(play->pathList, &waypointPos, this->path, this->waypoint);

        xDiff = waypointPos.x - this->actor.world.pos.x;
        zDiff = waypointPos.z - this->actor.world.pos.z;

        this->yawToWaypoint = RAD_TO_BINANG2(fatan2(xDiff, zDiff));
        this->distToWaypoint = sqrtf(SQ(xDiff) + SQ(zDiff));
    }

    add_calc_short_angle2(&this->actor.shape.rot.y, this->yawToWaypoint, 1, 2500, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    add_calc(&this->actor.speed, this->speedXZ, 0.6f, this->distToWaypoint, 0.0f);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

    return 0;
}

void mode_move_run2sit(EnMm* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->actionFunc = mode_move_sit;
        this->unk_1E8 = 0;
        this->mouthTexIndex = RM_MOUTH_CLOSED;
        this->unk_254 |= 1;
        this->unk_1E0 = 0;
        this->actor.speed = 0.0f;
        mm_anime_ct(this, RM_ANIM_SIT_WAIT, &this->curAnimIndex);
    }
}

static void mode_move_run(EnMm* this, PlayState* play) {
    f32 floorPolyNormalY;
    Vec3f dustPos;

    if (!player_demo_check(play)) {
        Skeleton_Info2_anime_play(&this->skelAnime);

        if (this->curAnimIndex == 0) {
            if (((s32)this->skelAnime.curFrame == 1) || ((s32)this->skelAnime.curFrame == 6)) {
                Actor_SE_set(&this->actor, NA_SE_PL_WALK_GROUND);
            }
        }

        if (this->curAnimIndex == 4) {
            if (((this->skelAnime.curFrame - this->skelAnime.playSpeed < 9.0f) && (this->skelAnime.curFrame >= 9.0f)) ||
                ((this->skelAnime.curFrame - this->skelAnime.playSpeed < 19.0f) &&
                 (this->skelAnime.curFrame >= 19.0f))) {
                Actor_SE_set(&this->actor, NA_SE_EN_MORIBLIN_WALK);
            }
        }

        if (GET_ITEMGETINF(ITEMGETINF_3B)) {
            this->speedXZ = 10.0f;
            this->skelAnime.playSpeed = 2.0f;
        } else {
            this->speedXZ = 3.0f;
            this->skelAnime.playSpeed = 1.0f;
        }

        func_rail_run(this, play);

        if (func_runtime_check() == 0) {
            if (this->actor.floorPoly != NULL) {
                floorPolyNormalY = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.y);

                if ((floorPolyNormalY > 0.9848f) || (floorPolyNormalY < -0.9848f)) {
                    if (this->sitTimer > 30) {
                        mm_anime_ct(this, RM_ANIM_SIT, &this->curAnimIndex);
                        this->actionFunc = mode_move_run2sit;
                    } else {
                        this->sitTimer++;
                    }
                } else {
                    this->sitTimer = 0;
                }
            }
        }

        if (GET_ITEMGETINF(ITEMGETINF_3B)) {
            dustPos.x = this->actor.world.pos.x;
            dustPos.y = this->actor.world.pos.y;
            dustPos.z = this->actor.world.pos.z;

            if (z_common_data.gameMode != GAMEMODE_END_CREDITS) {
                dust_fly_set2(play, &dustPos, 50.0f, 2, 350, 20, 0);
            }

            if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
                Actor_player_power_damage_set(play, &this->actor, 3.0f, this->actor.yawTowardsPlayer, 4.0f);
            }
        }
    }
}

static void mode_move_sit2run(EnMm* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->sitTimer = 0;
        this->actionFunc = mode_move_run;

        if (GET_ITEMGETINF(ITEMGETINF_3B)) {
            mm_anime_ct(this, RM_ANIM_SPRINT, &this->curAnimIndex);
            this->mouthTexIndex = RM_MOUTH_CLOSED;
        } else {
            mm_anime_ct(this, RM_ANIM_RUN, &this->curAnimIndex);
            this->mouthTexIndex = RM_MOUTH_OPEN;
        }

        this->unk_1E8 = 1;
    }
}

static void mode_move_sit(EnMm* this, PlayState* play) {
    eye_move2(play, &this->actor, &this->unk_248, &this->unk_24E, this->actor.focus.pos);
    Skeleton_Info2_anime_play(&this->skelAnime);

    if ((func_runtime_check() != 0) && (this->unk_1E0 == 0)) {
        this->unk_1E0 = 3;
        this->actionFunc = mode_move_sit2run;
        this->unk_254 &= ~1;
        mm_anime_ct(this, RM_ANIM_STAND, &this->curAnimIndex);
    }
}

void En_Mm_Actor_move(Actor* thisx, PlayState* play) {
    s32 pad;
    EnMm* this = (EnMm*)thisx;

    this->actionFunc(this, play);
    func_talk(this, play);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
}

void En_Mm_Actor_draw(Actor* thisx, PlayState* play) {
    static void* mouth_txt[] = { gRunningManMouthOpenTex, gRunningManMouthClosedTex };
    s32 pad;
    EnMm* this = (EnMm*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_mm.c", 1065);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(mouth_txt[this->mouthTexIndex]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          func_before_display, func_after_display, this);

    if (GET_ITEMGETINF(ITEMGETINF_3B)) {
        s32 linkChildObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_LINK_CHILD);

        // Draw Bunny Hood
        if (linkChildObjectSlot >= 0) {
            Mtx* mtx;
            Vec3s earRot;
            Mtx* mtx2;

            mtx = GRAPH_ALLOC(play->state.gfxCtx, sizeof(Mtx) * 2);

            Matrix_put(&this->unk_208);
            mtx2 = MATRIX_FINALIZE(play->state.gfxCtx, "../z_en_mm.c", 1111);

            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[linkChildObjectSlot].segment);
            gSPSegment(POLY_OPA_DISP++, 0x0B, mtx);
            gSPSegment(POLY_OPA_DISP++, 0x0D, mtx2 - 7);

            // Draw the ears in the neutral position (unlike Player, no flopping physics)

            // Right ear
            earRot.x = 0x3E2;
            earRot.y = 0xDBE;
            earRot.z = -0x348A;
            Matrix_softcv3_load(97.0f, -1203.0f, -240.0f, &earRot);
            MATRIX_TO_MTX(mtx++, "../z_en_mm.c", 1124);

            // Left ear
            earRot.x = -0x3E2;
            earRot.y = -0xDBE;
            earRot.z = -0x348A;
            Matrix_softcv3_load(97.0f, -1203.0f, 240.0f, &earRot);
            MATRIX_TO_MTX(mtx, "../z_en_mm.c", 1131);

            gSPDisplayList(POLY_OPA_DISP++, gLinkChildBunnyHoodDL);
            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->actor.objectSlot].segment);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_mm.c", 1141);
}

static s32 func_before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnMm* this = (EnMm*)thisx;

    if (this->unk_254 & 1) {
        switch (limbIndex) {
            case 8:
                rot->x += this->unk_24E.y;
                rot->y -= this->unk_24E.x;
                break;
            case 15:
                rot->x += this->unk_248.y;
                rot->z += (this->unk_248.x + 0xFA0);
                break;
            default:
                break;
        }
    }

    return 0;
}

static void func_after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 200.0f, 800.0f, 0.0f };
    EnMm* this = (EnMm*)thisx;

    if (limbIndex == 15) {
        Matrix_Position(&pos, &this->actor.focus.pos);
        Matrix_translate(260.0f, 20.0f, 0.0f, MTXMODE_APPLY);
        Matrix_rotateY(0.0f, MTXMODE_APPLY);
        Matrix_rotateX(0.0f, MTXMODE_APPLY);
        Matrix_rotateZ(4.0f * M_PI / 5.0f, MTXMODE_APPLY);
        Matrix_translate(-260.0f, 58.0f, 10.0f, MTXMODE_APPLY);
        Matrix_get(&this->unk_208);
    }
}
