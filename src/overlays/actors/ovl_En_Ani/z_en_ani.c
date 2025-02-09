/*
 * File: z_en_ani.c
 * Overlay: ovl_En_Ani
 * Description: Kakariko Roof Guy
 */

#include "z_en_ani.h"
#include "assets/objects/object_ani/object_ani.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void En_Ani_Actor_ct(Actor* thisx, PlayState* play);
void En_Ani_Actor_dt(Actor* thisx, PlayState* play);
void En_Ani_Actor_move(Actor* thisx, PlayState* play);
void En_Ani_Actor_draw(Actor* thisx, PlayState* play);

static s32 kihon_process(EnAni* this, PlayState* play, u16 textId);
void talk_wait_child(EnAni* this, PlayState* play);
void talk_wait_boy(EnAni* this, PlayState* play);
void give_common(EnAni* this, PlayState* play);
void give_common_before(EnAni* this, PlayState* play);
void move_wait_child(EnAni* this, PlayState* play);
void move_wait_boy(EnAni* this, PlayState* play);
void demo_proc4(EnAni* this, PlayState* play);
void demo_proc2(EnAni* this, PlayState* play);
void demo_proc1(EnAni* this, PlayState* play);
void demo_proc0(EnAni* this, PlayState* play);

ActorProfile En_Ani_Profile = {
    /**/ ACTOR_EN_ANI,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ANI,
    /**/ sizeof(EnAni),
    /**/ En_Ani_Actor_ct,
    /**/ En_Ani_Actor_dt,
    /**/ En_Ani_Actor_move,
    /**/ En_Ani_Actor_draw,
};

static ColliderCylinderInit EnAniOcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 40, 0, { 0 } },
};

void En_Ani_actor_set_process(EnAni* this, EnAniActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 850, ICHAIN_STOP),
};

void En_Ani_Actor_ct(Actor* thisx, PlayState* play) {
    s32 pad;
    EnAni* this = (EnAni*)thisx;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, -2800.0f, Actor_shadow_circle, 36.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gRoofManSkel, &gRoofManIdleAnim, this->jointTable, this->morphTable,
                       0x10);
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gRoofManIdleAnim);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &EnAniOcInfoData);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    if (!LINK_IS_ADULT) {
        En_Ani_actor_set_process(this, move_wait_child);
    } else {
        En_Ani_actor_set_process(this, move_wait_boy);
    }
    this->unk_2AA = 0;
    this->unk_2A8 = 0;
    this->actor.minVelocityY = -1.0f;
    this->actor.velocity.y = -1.0f;
}

void En_Ani_Actor_dt(Actor* thisx, PlayState* play) {
    EnAni* this = (EnAni*)thisx;

    ClObjPipe_dt(play, &this->collider);
}

static s32 kihon_process(EnAni* this, PlayState* play, u16 textId) {
    this->actor.textId = textId;
    this->unk_2A8 |= 1;
    Actor_talk_request2(&this->actor, play, 100.0f);
    return 0;
}

void talk_wait_child(EnAni* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        En_Ani_actor_set_process(this, move_wait_child);
    }
}

void talk_wait_boy(EnAni* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        En_Ani_actor_set_process(this, move_wait_boy);
    }
}

void give_common(EnAni* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
        this->actor.parent = NULL;
        if (!LINK_IS_ADULT) {
            En_Ani_actor_set_process(this, talk_wait_child);
        } else {
            En_Ani_actor_set_process(this, talk_wait_boy);
        }
        SET_ITEMGETINF(ITEMGETINF_15);
    } else {
        Actor_carry_request_set2(&this->actor, play, GI_HEART_PIECE, 10000.0f, 200.0f);
    }
}

void give_common_before(EnAni* this, PlayState* play) {
    if (Actor_talk_end_check(&this->actor, play)) {
        En_Ani_actor_set_process(this, give_common);
    }
    Actor_carry_request_set2(&this->actor, play, GI_HEART_PIECE, 10000.0f, 200.0f);
}

void move_wait_child(EnAni* this, PlayState* play) {
    u16 textId = get_mask_message(play, MASK_REACTION_SET_KAKARIKO_ROOF_MAN);
    s16 yawDiff;

    if (textId == 0) {
        textId = !IS_DAY ? 0x5051 : 0x5050;
    }

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if (Actor_talk_check(&this->actor, play)) {
        if (this->actor.textId == 0x5056) {
            En_Ani_actor_set_process(this, talk_wait_child);
        } else if (this->actor.textId == 0x5055) {
            En_Ani_actor_set_process(this, give_common_before);
        } else {
            En_Ani_actor_set_process(this, talk_wait_child);
        }
    } else if (yawDiff >= -0x36AF && yawDiff < 0 && this->actor.xzDistToPlayer < 150.0f &&
               -80.0f < this->actor.yDistToPlayer) {
        if (GET_ITEMGETINF(ITEMGETINF_15)) {
            kihon_process(this, play, 0x5056);
        } else {
            kihon_process(this, play, 0x5055);
        }
    } else if (yawDiff >= -0x3E7 && yawDiff < 0x36B0 && this->actor.xzDistToPlayer < 350.0f) {
        kihon_process(this, play, textId);
    }
}

void move_wait_boy(EnAni* this, PlayState* play) {
    s16 pad;
    s16 yawDiff;
    u16 textId;

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    if (Actor_talk_check(&this->actor, play)) {
        if (this->actor.textId == 0x5056) {
            En_Ani_actor_set_process(this, talk_wait_boy);
        } else if (this->actor.textId == 0x5055) {
            En_Ani_actor_set_process(this, give_common_before);
        } else {
            En_Ani_actor_set_process(this, talk_wait_boy);
        }
    } else if (yawDiff > -0x36B0 && yawDiff < 0 && this->actor.xzDistToPlayer < 150.0f &&
               -80.0f < this->actor.yDistToPlayer) {
        if (GET_ITEMGETINF(ITEMGETINF_15)) {
            kihon_process(this, play, 0x5056);
        } else {
            kihon_process(this, play, 0x5055);
        }
    } else if (yawDiff > -0x3E8 && yawDiff < 0x36B0 && this->actor.xzDistToPlayer < 350.0f) {
        if (!GET_EVENTCHKINF(EVENTCHKINF_2F)) {
            textId = 0x5052;
        } else {
            textId = GET_ITEMGETINF(ITEMGETINF_15) ? 0x5054 : 0x5053;
        }
        kihon_process(this, play, textId);
    }
}

void demo_proc4(EnAni* this, PlayState* play) {
}

void demo_proc2(EnAni* this, PlayState* play) {
    if (play->csCtx.actorCues[0]->id == 4) {
        Skeleton_Info2_init(&this->skelAnime, &gRoofManGettingUpAfterKnockbackAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gRoofManGettingUpAfterKnockbackAnim), ANIMMODE_ONCE, -4.0f);
        this->unk_2AA++;
        this->actor.shape.shadowDraw = Actor_shadow_circle;
    }
}

void demo_proc1(EnAni* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->unk_2AA++;
    }
}

void demo_proc0(EnAni* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->skelAnime.curFrame = 0.0f;
    }
    if (play->csCtx.actorCues[0]->id == 2) {
        Skeleton_Info2_init(&this->skelAnime, &gRoofManKnockbackAnim, 1.0f, 0.0f,
                         Si2_anime_end_frame(&gRoofManKnockbackAnim), ANIMMODE_ONCE, 0.0f);
        this->actor.shape.shadowDraw = NULL;
        this->unk_2AA++;
    }
}

void En_Ani_Actor_move(Actor* thisx, PlayState* play) {
    EnAni* this = (EnAni*)thisx;
    s32 pad[2];

    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_position_moveF(&this->actor);
    Actor_BGcheck2(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_2);
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.actorCues[0] != NULL)) {
        switch (this->unk_2AA) {
            case 0:
                demo_proc0(this, play);
                break;
            case 1:
                demo_proc1(this, play);
                break;
            case 2:
                demo_proc2(this, play);
                break;
            case 3:
                demo_proc1(this, play);
                break;
            case 4:
                demo_proc4(this, play);
                break;
        }

        if (play->csCtx.curFrame == 100) {
            Na_StartFixSe_F(NA_SE_IT_EARTHQUAKE);
        }
    } else {
        if (Skeleton_Info2_anime_play(&this->skelAnime)) {
            this->skelAnime.curFrame = 0.0f;
        }
        this->actionFunc(this, play);
    }

    if (this->unk_2A8 & 1) {
        eye_move2(play, &this->actor, &this->unk_29C, &this->unk_2A2, this->actor.focus.pos);
        this->unk_2A2.z = 0;
        this->unk_2A2.y = this->unk_2A2.z;
        this->unk_2A2.x = this->unk_2A2.z;
    } else {
        add_calc_short_angle2(&this->unk_29C.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_29C.y, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_2A2.x, 0, 6, 6200, 100);
        add_calc_short_angle2(&this->unk_2A2.y, 0, 6, 6200, 100);
    }

    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = get_random_timer(60, 60);
    }
    this->eyeIndex = this->blinkTimer;
    if (this->eyeIndex >= 3) {
        this->eyeIndex = 0;
    }
}

static s32 before_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnAni* this = (EnAni*)thisx;

    if (limbIndex == 15) {
        rot->x += this->unk_29C.y;
        rot->z += this->unk_29C.x;
    }
    return false;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    static Vec3f pos = { 800.0f, 500.0f, 0.0f };
    EnAni* this = (EnAni*)thisx;

    if (limbIndex == 15) {
        Matrix_Position(&pos, &this->actor.focus.pos);
    }
}

void En_Ani_Actor_draw(Actor* thisx, PlayState* play) {
    static void* eye_txt[] = {
        gRoofManEyeOpenTex,
        gRoofManEyeHalfTex,
        gRoofManEyeClosedTex,
    };
    EnAni* this = (EnAni*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_ani.c", 719);

    _polygon_z_light_fog_prim(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeIndex]));

    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          before_display, after_display, this);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_ani.c", 736);
}
