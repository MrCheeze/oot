#include "z_en_cs.h"
#include "assets/objects/object_cs/object_cs.h"
#include "assets/objects/object_link_child/object_link_child.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Cs_Actor_ct(Actor* thisx, PlayState* play);
void En_Cs_Actor_dt(Actor* thisx, PlayState* play);
void En_Cs_Actor_move(Actor* thisx, PlayState* play);
void En_Cs_Actor_draw(Actor* thisx, PlayState* play);

void mode_move_walk(EnCs* this, PlayState* play);
void mode_move_talk(EnCs* this, PlayState* play);
static void mode_move_wait(EnCs* this, PlayState* play);
static s32 func_before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
static void func_after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx);

ActorProfile En_Cs_Profile = {
    /**/ ACTOR_EN_CS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_CS,
    /**/ sizeof(EnCs),
    /**/ En_Cs_Actor_ct,
    /**/ En_Cs_Actor_dt,
    /**/ En_Cs_Actor_move,
    /**/ En_Cs_Actor_draw,
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

static CollisionCheckInfoInit2 statusdata = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable btldata[] = {
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

typedef enum EnCsAnimation {
    /* 0 */ ENCS_ANIM_0,
    /* 1 */ ENCS_ANIM_1,
    /* 2 */ ENCS_ANIM_2,
    /* 3 */ ENCS_ANIM_3
} EnCsAnimation;

static AnimationFrameCountInfo anime_ct_data[] = {
    { &gGraveyardKidWalkAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gGraveyardKidSwingStickUpAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gGraveyardKidGrabStickTwoHandsAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
    { &gGraveyardKidIdleAnim, 1.0f, ANIMMODE_ONCE, -10.0f },
};

void cs_anime_ct(EnCs* this, s32 index, s32* currentIndex) {
    f32 morphFrames;

    if ((*currentIndex < 0) || (index == *currentIndex)) {
        morphFrames = 0.0f;
    } else {
        morphFrames = anime_ct_data[index].morphFrames;
    }

    if (anime_ct_data[index].frameCount >= 0.0f) {
        Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, anime_ct_data[index].frameCount, 0.0f,
                         Si2_anime_end_frame(anime_ct_data[index].animation), anime_ct_data[index].mode,
                         morphFrames);
    } else {
        Skeleton_Info2_init(&this->skelAnime, anime_ct_data[index].animation, anime_ct_data[index].frameCount,
                         Si2_anime_end_frame(anime_ct_data[index].animation), 0.0f, anime_ct_data[index].mode,
                         morphFrames);
    }

    *currentIndex = index;
}

void En_Cs_Actor_ct(Actor* thisx, PlayState* play) {
    EnCs* this = (EnCs*)thisx;
    s32 pad;

    if (!IS_DAY) {
        Actor_delete(&this->actor);
        return;
    }

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 19.0f);

    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGraveyardKidSkel, NULL, this->jointTable, this->morphTable, 16);

    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &atinfodata);

    CollisionCheck_Status_set3(&this->actor.colChkInfo, btldata, &statusdata);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

    Skeleton_Info2_init(&this->skelAnime, anime_ct_data[ENCS_ANIM_0].animation, 1.0f, 0.0f,
                     Si2_anime_end_frame(anime_ct_data[ENCS_ANIM_0].animation), anime_ct_data[ENCS_ANIM_0].mode,
                     anime_ct_data[ENCS_ANIM_0].morphFrames);

    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->path = PARAMS_GET_U(this->actor.params, 0, 8);
    this->unk_1EC = 0; // This variable is unused anywhere else
    this->talkState = 0;
    this->currentAnimIndex = -1;
    this->actor.gravity = -1.0f;

    cs_anime_ct(this, ENCS_ANIM_0, &this->currentAnimIndex);

    this->actionFunc = mode_move_walk;
    this->walkSpeed = 1.0f;
}

void En_Cs_Actor_dt(Actor* thisx, PlayState* play) {
    EnCs* this = (EnCs*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static s32 func_endmsg_chk(EnCs* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    s32 talkState = 1;

    switch (message_check(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (pad_on_check(play)) {
                if (play->msgCtx.choiceIndex == 0) {
                    this->actor.textId = 0x2026;
                    cs_anime_ct(this, ENCS_ANIM_3, &this->currentAnimIndex);
                    talkState = 2;
                } else {
                    this->actor.textId = 0x2024;
                    cs_anime_ct(this, ENCS_ANIM_1, &this->currentAnimIndex);
                    talkState = 2;
                }
            }
            break;
        case TEXT_STATE_DONE:
            if (pad_on_check(play)) {
                if (this->actor.textId == 0x2026) {
                    mask_cancel(play);
                    item_get_setting(play, ITEM_SOLD_OUT);
                    SET_ITEMGETINF(ITEMGETINF_3A);
                    lupy_increase(30);
                    this->actor.textId = 0x2027;
                    talkState = 2;
                } else {
                    talkState = 0;
                }
            }
            break;
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_EVENT:
            break;
    }

    return talkState;
}

static s32 func_set_msg(EnCs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 textId = get_mask_message(play, MASK_REACTION_SET_GRAVEYARD_KID);

    if (GET_ITEMGETINF(ITEMGETINF_3A)) {
        if (textId == 0) {
            textId = 0x2028;
        }
    } else if (player->currentMask == PLAYER_MASK_SPOOKY) {
        textId = 0x2023;
    } else {
        if (textId == 0) {
            textId = 0x2022;
        }
    }

    return textId;
}

static void func_talk(EnCs* this, PlayState* play) {
    s32 pad;
    s16 sp2A;
    s16 sp28;

    if (this->talkState == 2) {
        message_set2(play, this->actor.textId);
        this->talkState = 1;
    } else if (this->talkState == 1) {
        this->talkState = func_endmsg_chk(this, play);
    } else if (Actor_talk_check(&this->actor, play)) {
        if ((this->actor.textId == 0x2022) || ((this->actor.textId != 0x2022) && (this->actor.textId != 0x2028))) {
            cs_anime_ct(this, ENCS_ANIM_3, &this->currentAnimIndex);
        }

        if ((this->actor.textId == 0x2023) || (this->actor.textId == 0x2028)) {
            cs_anime_ct(this, ENCS_ANIM_1, &this->currentAnimIndex);
        }

        if (this->actor.textId == 0x2023) {
            Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
        }

        this->talkState = 1;
    } else {
        Actor_display_position_set(play, &this->actor, &sp2A, &sp28);

        if ((sp2A >= 0) && (sp2A <= 320) && (sp28 >= 0) && (sp28 <= 240) &&
            Actor_talk_request2(&this->actor, play, 100.0f)) {
            this->actor.textId = func_set_msg(this, play);
        }
    }
}

static s32 func_get_rail_number(Path* pathList, s32 pathIndex) {
    Path* path = &pathList[pathIndex];

    return path->count;
}

static s32 func_get_rail_pos(Path* pathList, Vec3f* dest, s32 pathIndex, s32 waypoint) {
    Path* path = pathList;
    Vec3s* pathPos;

    path += pathIndex;
    pathPos = &((Vec3s*)SEGMENTED_TO_VIRTUAL(path->points))[waypoint];

    dest->x = pathPos->x;
    dest->y = pathPos->y;
    dest->z = pathPos->z;

    return 0;
}

s32 func_move_rail(EnCs* this, PlayState* play) {
    f32 xDiff;
    f32 zDiff;
    Vec3f pathPos;
    s32 waypointCount;
    s16 walkAngle1;
    s16 walkAngle2;

    func_get_rail_pos(play->pathList, &pathPos, this->path, this->waypoint);
    xDiff = pathPos.x - this->actor.world.pos.x;
    zDiff = pathPos.z - this->actor.world.pos.z;
    walkAngle1 = RAD_TO_BINANG(fatan2(xDiff, zDiff));
    this->walkAngle = walkAngle1;
    this->walkDist = sqrtf((xDiff * xDiff) + (zDiff * zDiff));

    while (this->walkDist <= 10.44f) {
        this->waypoint++;
        waypointCount = func_get_rail_number(play->pathList, this->path);

        if ((this->waypoint < 0) || (!(this->waypoint < waypointCount))) {
            this->waypoint = 0;
        }

        func_get_rail_pos(play->pathList, &pathPos, this->path, this->waypoint);
        xDiff = pathPos.x - this->actor.world.pos.x;
        zDiff = pathPos.z - this->actor.world.pos.z;
        walkAngle2 = RAD_TO_BINANG(fatan2(xDiff, zDiff));
        this->walkAngle = walkAngle2;
        this->walkDist = sqrtf((xDiff * xDiff) + (zDiff * zDiff));
    }

    add_calc_short_angle2(&this->actor.shape.rot.y, this->walkAngle, 1, 2500, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.speed = this->walkSpeed;
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);

    return 0;
}

void mode_move_walk(EnCs* this, PlayState* play) {
    s32 rnd;
    s32 animIndex;
    s32 curAnimFrame;

    if (this->talkState != 0) {
        this->actionFunc = mode_move_talk;
        return;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        animIndex = this->currentAnimIndex;

        if (this->talkState == 0) {
            if (GET_ITEMGETINF(ITEMGETINF_3A)) {
                rnd = fqrand() * 10.0f;
            } else {
                rnd = fqrand() * 5.0f;
            }

            if (rnd == 0) {
                if (GET_ITEMGETINF(ITEMGETINF_3A)) {
                    animIndex = 2.0f * fqrand();
                    animIndex = (animIndex == 0) ? ENCS_ANIM_2 : ENCS_ANIM_1;
                } else {
                    animIndex = ENCS_ANIM_2;
                }

                this->actionFunc = mode_move_wait;
            } else {
                animIndex = ENCS_ANIM_0;
            }
        }

        cs_anime_ct(this, animIndex, &this->currentAnimIndex);
    }

    if (this->talkState == 0) {
        curAnimFrame = this->skelAnime.curFrame;

        if (((curAnimFrame >= 8) && (curAnimFrame < 16)) || ((curAnimFrame >= 23) && (curAnimFrame < 30)) ||
            (curAnimFrame == 0)) {
            this->walkSpeed = 0.0f;
        } else {
            this->walkSpeed = 1.0f;
        }

        func_move_rail(this, play);
    }
}

static void mode_move_wait(EnCs* this, PlayState* play) {
    s32 animIndex;

    if (this->talkState != 0) {
        this->actionFunc = mode_move_talk;
        return;
    }

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        animIndex = this->currentAnimIndex;

        if (this->talkState == 0) {
            if (this->animLoopCount > 0) {
                this->animLoopCount--;
                animIndex = this->currentAnimIndex;
            } else {
                animIndex = ENCS_ANIM_0;
                this->actionFunc = mode_move_walk;
            }
        }

        cs_anime_ct(this, animIndex, &this->currentAnimIndex);
    }
}

void mode_move_talk(EnCs* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        cs_anime_ct(this, this->currentAnimIndex, &this->currentAnimIndex);
    }

    this->flag |= 1;
    this->interactInfo.trackPos.x = player->actor.focus.pos.x;
    this->interactInfo.trackPos.y = player->actor.focus.pos.y;
    this->interactInfo.trackPos.z = player->actor.focus.pos.z;
    eye_moveM(&this->actor, &this->interactInfo, 0, NPC_TRACKING_FULL_BODY);

    if (this->talkState == 0) {
        cs_anime_ct(this, ENCS_ANIM_0, &this->currentAnimIndex);
        this->actionFunc = mode_move_walk;
        this->flag &= ~1;
    }
}

void En_Cs_Actor_move(Actor* thisx, PlayState* play) {
    static s32 eye_timer_value[] = { 70, 1, 1 };
    EnCs* this = (EnCs*)thisx;
    s32 pad;

    if (this->currentAnimIndex == 0) {
        if (((s32)this->skelAnime.curFrame == 9) || ((s32)this->skelAnime.curFrame == 23)) {
            Actor_SE_set(&this->actor, NA_SE_EV_CHIBI_WALK);
        }
    } else if (this->currentAnimIndex == 1) {
        if (((s32)this->skelAnime.curFrame == 10) || ((s32)this->skelAnime.curFrame == 25)) {
            Actor_SE_set(&this->actor, NA_SE_EV_CHIBI_WALK);
        }
    } else if ((this->currentAnimIndex == 2) && ((s32)this->skelAnime.curFrame == 20)) {
        Actor_SE_set(&this->actor, NA_SE_EV_CHIBI_WALK);
    }

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);

    this->actionFunc(this, play);

    func_talk(this, play);

    this->eyeBlinkTimer--;

    if (this->eyeBlinkTimer < 0) {
        this->eyeIndex++;

        if (this->eyeIndex >= 3) {
            this->eyeIndex = 0;
        }

        this->eyeBlinkTimer = eye_timer_value[this->eyeIndex];
    }
}

void En_Cs_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = {
        gGraveyardKidEyesOpenTex,
        gGraveyardKidEyesHalfTex,
        gGraveyardKidEyesClosedTex,
    };
    EnCs* this = (EnCs*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_cs.c", 968);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeIndex]));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          func_before_display, func_after_display, &this->actor);

    if (GET_ITEMGETINF(ITEMGETINF_3A)) {
        s32 linkChildObjectSlot = Object_Exchange_bank_check(&play->objectCtx, OBJECT_LINK_CHILD);

        // Handle attaching the Spooky Mask to the boy's face
        if (linkChildObjectSlot >= 0) {
            Mtx* mtx;

            Matrix_put(&this->spookyMaskMtx);
            mtx = MATRIX_FINALIZE(play->state.gfxCtx, "../z_en_cs.c", 1000);
            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[linkChildObjectSlot].segment);
            gSPSegment(POLY_OPA_DISP++, 0x0D, mtx - 7);
            gSPDisplayList(POLY_OPA_DISP++, gLinkChildSpookyMaskDL);
            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->actor.objectSlot].segment);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_cs.c", 1015);
}

static s32 func_before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnCs* this = (EnCs*)thisx;

    if (this->flag & 1) {
        switch (limbIndex) {
            case 8:
                rot->x += this->interactInfo.torsoRot.y;
                rot->y -= this->interactInfo.torsoRot.x;
                break;
            case 15:
                rot->x += this->interactInfo.headRot.y;
                rot->z += this->interactInfo.headRot.x;
                break;
        }
    }

    return 0;
}

static void func_after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 500.0f, 800.0f, 0.0f };
    EnCs* this = (EnCs*)thisx;

    if (limbIndex == 15) {
        Matrix_Position(&pos, &this->actor.focus.pos);
        Matrix_translate(0.0f, -200.0f, 0.0f, MTXMODE_APPLY);
        Matrix_rotateY(0.0f, MTXMODE_APPLY);
        Matrix_rotateX(0.0f, MTXMODE_APPLY);
        Matrix_rotateZ(DEG_TO_RAD(100), MTXMODE_APPLY);
        Matrix_get(&this->spookyMaskMtx);
    }
}
