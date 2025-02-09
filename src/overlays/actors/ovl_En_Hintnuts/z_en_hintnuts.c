/*
 * File: z_en_hintnuts.c
 * Overlay: ovl_En_Hintnuts
 * Description: Hint Deku Scrubs (Deku Tree)
 */

#include "z_en_hintnuts.h"
#include "assets/objects/object_hintnuts/object_hintnuts.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void En_Hintnuts_actor_ct(Actor* thisx, PlayState* play);
void En_Hintnuts_actor_dt(Actor* thisx, PlayState* play);
void En_Hintnuts_actor_move(Actor* thisx, PlayState* play);
void En_Hintnuts_actor_draw(Actor* thisx, PlayState* play);

static void mode_head_up_init(EnHintnuts* this);
static void mode_head_up(EnHintnuts* this, PlayState* play);
static void mode_look_around(EnHintnuts* this, PlayState* play);
static void mode_attack_wait(EnHintnuts* this, PlayState* play);
static void mode_attack(EnHintnuts* this, PlayState* play);
static void mode_head_down(EnHintnuts* this, PlayState* play);
static void mode_jump(EnHintnuts* this, PlayState* play);
void mode_jump_down(EnHintnuts* this, PlayState* play);
static void mode_escape(EnHintnuts* this, PlayState* play);
static void mode_talk(EnHintnuts* this, PlayState* play);
void mode_talk_after(EnHintnuts* this, PlayState* play);
static void mode_event_wait(EnHintnuts* this, PlayState* play);

ActorProfile En_Hintnuts_Profile = {
    /**/ ACTOR_EN_HINTNUTS,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_HINTNUTS,
    /**/ sizeof(EnHintnuts),
    /**/ En_Hintnuts_actor_ct,
    /**/ En_Hintnuts_actor_dt,
    /**/ En_Hintnuts_actor_move,
    /**/ En_Hintnuts_actor_draw,
};

static ColliderCylinderInit HintnutsAcOcPipeData = {
    {
        COL_MATERIAL_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
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
    { 18, 32, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit HintnutsStatusData = { 1, 18, 32, MASS_HEAVY };

static s16 work_num = 0;

static InitChainEntry value_init[] = {
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_DEKU_SCRUB, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2600, ICHAIN_STOP),
};

void En_Hintnuts_actor_ct(Actor* thisx, PlayState* play) {
    EnHintnuts* this = (EnHintnuts*)thisx;
    s32 pad;

    ValueSet_process(&this->actor, value_init);
    if (this->actor.params == 0xA) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    } else {
        Shape_Info_init(&this->actor.shape, 0x0, Actor_shadow_circle, 35.0f);
        Skeleton_Info2_M_ct(play, &this->skelAnime, &gHintNutsSkel, &gHintNutsStandAnim, this->jointTable, this->morphTable,
                       10);
        ClObjPipe_ct(play, &this->collider);
        ClObjPipe_set5(play, &this->collider, &this->actor, &HintnutsAcOcPipeData);
        CollisionCheck_Status_set2(&this->actor.colChkInfo, NULL, &HintnutsStatusData);
        mes_set(play, &this->actor, PARAMS_GET_U(this->actor.params, 8, 8));
        this->textIdCopy = this->actor.textId;
        this->actor.params &= 0xFF;
        work_num = 0;
        if (this->actor.textId == 0x109B) {
            if (Actor_Environment_room_clear_Check(play, 0x9) != 0) {
                Actor_delete(&this->actor);
                return;
            }
        }
        mode_head_up_init(this);
        Actor_info_make_child_actor(&play->actorCtx, &this->actor, play, ACTOR_EN_HINTNUTS, this->actor.world.pos.x,
                           this->actor.world.pos.y, this->actor.world.pos.z, 0, this->actor.world.rot.y, 0, 0xA);
    }
}

void En_Hintnuts_actor_dt(Actor* thisx, PlayState* play) {
    EnHintnuts* this = (EnHintnuts*)thisx;

    if (this->actor.params != 0xA) {
        ClObjPipe_dt(play, &this->collider);
    }
}

void hintnuts_change_talk_mode(EnHintnuts* this, PlayState* play) {
    if (this->actor.textId != 0 && this->actor.category == ACTORCAT_ENEMY &&
        ((this->actor.params == 0) || (work_num == 2))) {
        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
        Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_BG);
    }
}

static void mode_head_up_init(EnHintnuts* this) {
    Skeleton_Info2_init_standard_speedset_stop(&this->skelAnime, &gHintNutsUpAnim, 0.0f);
    this->animFlagAndTimer = get_random_timer(100, 50);
    this->collider.dim.height = 5;
    this->actor.world.pos = this->actor.home.pos;
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = mode_head_up;
}

static void mode_look_around_init(EnHintnuts* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gHintNutsLookAroundAnim);
    this->animFlagAndTimer = 2;
    this->actionFunc = mode_look_around;
}

static void mode_attack_init(EnHintnuts* this) {
    Skeleton_Info2_init_standard_stop(&this->skelAnime, &gHintNutsSpitAnim);
    this->actionFunc = mode_attack;
}

static void mode_attack_wait_init(EnHintnuts* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gHintNutsStandAnim, -3.0f);
    if (this->actionFunc == mode_attack) {
        this->animFlagAndTimer = 2 | 0x1000; // sets timer and flag
    } else {
        this->animFlagAndTimer = 1;
    }
    this->actionFunc = mode_attack_wait;
}

static void mode_head_down_init(EnHintnuts* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gHintNutsBurrowAnim, -5.0f);
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DOWN);
    this->actionFunc = mode_head_down;
}

static void mode_jump_init(EnHintnuts* this) {
    Skeleton_Info2_init_standard_stop_morf(&this->skelAnime, &gHintNutsUnburrowAnim, -3.0f);
    this->collider.dim.height = 37;
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DAMAGE);
    this->collider.base.acFlags &= ~AC_ON;

    if (this->actor.params > 0 && this->actor.params < 4 && this->actor.category == ACTORCAT_ENEMY) {
        if (work_num == -4) {
            work_num = 0;
        }
        if (this->actor.params == work_num + 1) {
            work_num++;
        } else {
            if (work_num > 0) {
                work_num = -work_num;
            }
            work_num--;
        }
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actionFunc = mode_jump_down;
    } else {
        this->actionFunc = mode_jump;
    }
}

static void mode_escape_init(EnHintnuts* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gHintNutsRunAnim);
    this->animFlagAndTimer = 5;
    this->actionFunc = mode_escape;
}

static void mode_talk_init(EnHintnuts* this) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gHintNutsTalkAnim, -5.0f);
    this->actionFunc = mode_talk;
    this->actor.speed = 0.0f;
}

void mode_talk_after_init(EnHintnuts* this, PlayState* play) {
    Skeleton_Info2_init_standard_repeat_morf(&this->skelAnime, &gHintNutsRunAnim, -5.0f);
    this->actor.speed = 3.0f;
    this->animFlagAndTimer = 100;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_DAMAGE);
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ITEM00, this->actor.world.pos.x, this->actor.world.pos.y,
                this->actor.world.pos.z, 0x0, 0x0, 0x0, 0x3); // recovery heart
    this->actionFunc = mode_talk_after;
}

static void mode_event_wait_init(EnHintnuts* this) {
    Skeleton_Info2_init_standard_repeat(&this->skelAnime, &gHintNutsFreezeAnim);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Set_Fog(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 100);
    this->actor.colorFilterTimer = 1;
    this->animFlagAndTimer = 0;
    Actor_SE_set(&this->actor, NA_SE_EN_NUTS_FAINT);
    if (work_num == -3) {
        Na_StartSystemSe_F(NA_SE_SY_ERROR);
        work_num = -4;
    }
    this->actionFunc = mode_event_wait;
}

static void mode_head_up(EnHintnuts* this, PlayState* play) {
    s32 hasSlowPlaybackSpeed = false;

    if (this->skelAnime.playSpeed < 0.5f) {
        hasSlowPlaybackSpeed = true;
    }
    if (hasSlowPlaybackSpeed && (this->animFlagAndTimer != 0)) {
        this->animFlagAndTimer--;
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 9.0f)) {
        this->collider.base.acFlags |= AC_ON;
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 8.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_UP);
    }

    this->collider.dim.height = 5.0f + ((CLAMP(this->skelAnime.curFrame, 9.0f, 12.0f) - 9.0f) * 9.0f);
    if (!hasSlowPlaybackSpeed && (this->actor.xzDistToPlayer < 120.0f)) {
        mode_head_down_init(this);
    } else if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        if (this->actor.xzDistToPlayer < 120.0f) {
            mode_head_down_init(this);
        } else if ((this->animFlagAndTimer == 0) && (this->actor.xzDistToPlayer > 320.0f)) {
            mode_look_around_init(this);
        } else {
            mode_attack_wait_init(this);
        }
    }
    if (hasSlowPlaybackSpeed && 160.0f < this->actor.xzDistToPlayer && fabsf(this->actor.yDistToPlayer) < 120.0f &&
        ((this->animFlagAndTimer == 0) || (this->actor.xzDistToPlayer < 480.0f))) {
        this->skelAnime.playSpeed = 1.0f;
    }
}

static void mode_look_around(EnHintnuts* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && this->animFlagAndTimer != 0) {
        this->animFlagAndTimer--;
    }
    if ((this->actor.xzDistToPlayer < 120.0f) || (this->animFlagAndTimer == 0)) {
        mode_head_down_init(this);
    }
}

static void mode_attack_wait(EnHintnuts* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) && this->animFlagAndTimer != 0) {
        this->animFlagAndTimer--;
    }
    if (!(this->animFlagAndTimer & 0x1000)) {
        adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    }
    if (this->actor.xzDistToPlayer < 120.0f || this->animFlagAndTimer == 0x1000) {
        mode_head_down_init(this);
    } else if (this->animFlagAndTimer == 0) {
        mode_attack_init(this);
    }
}

static void mode_attack(EnHintnuts* this, PlayState* play) {
    Vec3f nutPos;

    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
    if (this->actor.xzDistToPlayer < 120.0f) {
        mode_head_down_init(this);
    } else if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_attack_wait_init(this);
    } else if (Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
        nutPos.x = this->actor.world.pos.x + (sin_s(this->actor.shape.rot.y) * 23.0f);
        nutPos.y = this->actor.world.pos.y + 12.0f;
        nutPos.z = this->actor.world.pos.z + (cos_s(this->actor.shape.rot.y) * 23.0f);
        if (Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_NUTSBALL, nutPos.x, nutPos.y, nutPos.z, this->actor.shape.rot.x,
                        this->actor.shape.rot.y, this->actor.shape.rot.z, 1) != NULL) {
            Actor_SE_set(&this->actor, NA_SE_EN_NUTS_THROW);
        }
    }
}

static void mode_head_down(EnHintnuts* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_head_up_init(this);
    } else {
        this->collider.dim.height = 5.0f + ((3.0f - CLAMP(this->skelAnime.curFrame, 1.0f, 3.0f)) * 12.0f);
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 4.0f)) {
        this->collider.base.acFlags &= ~AC_ON;
    }

    add_calc2(&this->actor.world.pos.x, this->actor.home.pos.x, 0.5f, 3.0f);
    add_calc2(&this->actor.world.pos.z, this->actor.home.pos.z, 0.5f, 3.0f);
}

static void mode_jump(EnHintnuts* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        this->unk_196 = this->actor.yawTowardsPlayer + 0x8000;
        mode_escape_init(this);
    }
    adds(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0xE38);
}

void mode_jump_down(EnHintnuts* this, PlayState* play) {
    if (Skeleton_Info2_anime_play(&this->skelAnime)) {
        mode_event_wait_init(this);
    }
}

void hintnuts_set_talk_request(EnHintnuts* this, PlayState* play) {
    if (this->actor.category != ACTORCAT_ENEMY) {
        if ((this->collider.base.ocFlags1 & OC1_HIT) || this->actor.isLockedOn) {
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        } else {
            this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        }
        if (this->actor.xzDistToPlayer < 130.0f) {
            this->actor.textId = this->textIdCopy;
            Actor_talk_request(&this->actor, play);
        }
    }
}

static void mode_escape(EnHintnuts* this, PlayState* play) {
    s32 temp_ret;
    s16 diffRotInit;
    s16 diffRot;
    f32 phi_f0;

    Skeleton_Info2_anime_play(&this->skelAnime);
    temp_ret = Skeleton_Info_frame_check(&this->skelAnime, 0.0f);
    if (temp_ret != 0 && this->animFlagAndTimer != 0) {
        this->animFlagAndTimer--;
    }
    if ((temp_ret != 0) || (Skeleton_Info_frame_check(&this->skelAnime, 6.0f))) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_WALK);
    }

    chase_f(&this->actor.speed, 7.5f, 1.0f);
    if (add_calc_short_angle2(&this->actor.world.rot.y, this->unk_196, 1, 0xE38, 0xB6) == 0) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
            this->unk_196 = Actor_search_position_angleY(&this->actor, &this->actor.home.pos);
        } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            this->unk_196 = this->actor.wallYaw;
        } else if (this->animFlagAndTimer == 0) {
            diffRotInit = Actor_search_position_angleY(&this->actor, &this->actor.home.pos);
            diffRot = diffRotInit - this->actor.yawTowardsPlayer;
            if (ABS(diffRot) >= 0x2001) {
                this->unk_196 = diffRotInit;
            } else {
                phi_f0 = (0.0f <= (f32)diffRot) ? 1.0f : -1.0f;
                this->unk_196 = (s16)((phi_f0 * -8192.0f) + (f32)this->actor.yawTowardsPlayer);
            }
        } else {
            this->unk_196 = (s16)(this->actor.yawTowardsPlayer + 0x8000);
        }
    }

    this->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    if (Actor_talk_check(&this->actor, play)) {
        mode_talk_init(this);
    } else if (this->animFlagAndTimer == 0 && Actor_search_position_distanceXZ(&this->actor, &this->actor.home.pos) < 20.0f &&
               fabsf(this->actor.world.pos.y - this->actor.home.pos.y) < 2.0f) {
        this->actor.speed = 0.0f;
        if (this->actor.category == ACTORCAT_BG) {
            this->actor.flags &=
                ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED);
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE;
            Actor_info_part_chg(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
        }
        mode_head_down_init(this);
    } else {
        hintnuts_set_talk_request(this, play);
    }
}

static void mode_talk(EnHintnuts* this, PlayState* play) {
    Skeleton_Info2_anime_play(&this->skelAnime);
    add_calc_short_angle2(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x3, 0x400, 0x100);
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT) {
        mode_talk_after_init(this, play);
    }
}

void mode_talk_after(EnHintnuts* this, PlayState* play) {
    s16 temp_a1;

    Skeleton_Info2_anime_play(&this->skelAnime);
    if (this->animFlagAndTimer != 0) {
        this->animFlagAndTimer--;
    }
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f) || Skeleton_Info_frame_check(&this->skelAnime, 6.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_WALK);
    }
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        temp_a1 = this->actor.wallYaw;
    } else {
        temp_a1 = this->actor.yawTowardsPlayer - getRealCameraAngleY(GET_ACTIVE_CAM(play)) - 0x8000;
        if (ABS(temp_a1) >= 0x4001) {
            temp_a1 = getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x8000;
        } else {
            temp_a1 = getRealCameraAngleY(GET_ACTIVE_CAM(play)) - (temp_a1 >> 1) + 0x8000;
        }
    }
    chase_angle(&this->actor.shape.rot.y, temp_a1, 0x800);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if ((this->animFlagAndTimer == 0) || (this->actor.projectedPos.z < 0.0f)) {
        message_close(play);
        if (this->actor.params == 3) {
            Actor_Environment_room_clear_On(play, this->actor.room);
            work_num = 3;
        }
        if (this->actor.child != NULL) {
            Actor_info_part_chg(play, &play->actorCtx, this->actor.child, ACTORCAT_PROP);
        }
        Actor_delete(&this->actor);
    }
}

static void mode_event_wait(EnHintnuts* this, PlayState* play) {
    this->actor.colorFilterTimer = 1;
    Skeleton_Info2_anime_play(&this->skelAnime);
    if (Skeleton_Info_frame_check(&this->skelAnime, 0.0f)) {
        Actor_SE_set(&this->actor, NA_SE_EN_NUTS_FAINT);
    }
    if (this->animFlagAndTimer == 0) {
        if (work_num == 3) {
            if (this->actor.child != NULL) {
                Actor_info_part_chg(play, &play->actorCtx, this->actor.child, ACTORCAT_PROP);
            }
            this->animFlagAndTimer = 1;
        } else if (work_num == -4) {
            this->animFlagAndTimer = 2;
        }
    } else if (chase_f(&this->actor.world.pos.y, this->actor.home.pos.y - 35.0f, 7.0f) != 0) {
        if (this->animFlagAndTimer == 1) {
            Actor_delete(&this->actor);
        } else {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->actor.colChkInfo.health = HintnutsStatusData.health;
            this->actor.colorFilterTimer = 0;
            mode_head_up_init(this);
        }
    }
}

void En_Hintnuts_damage_proc(EnHintnuts* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Hit_bit_set(&this->actor, &this->collider.elem, true);
        if (this->collider.base.ac->id != ACTOR_EN_NUTSBALL) {
            mode_head_down_init(this);
        } else {
            hintnuts_change_talk_mode(this, play);
            mode_jump_init(this);
        }
    } else if (play->actorCtx.unk_02 != 0) {
        hintnuts_change_talk_mode(this, play);
        mode_jump_init(this);
    }
}

void En_Hintnuts_actor_move(Actor* thisx, PlayState* play) {
    EnHintnuts* this = (EnHintnuts*)thisx;
    s32 pad;

    if (this->actor.params != 0xA) {
        En_Hintnuts_damage_proc(this, play);
        this->actionFunc(this, play);
        if (this->actionFunc != mode_event_wait && this->actionFunc != mode_jump_down) {
            Actor_position_moveF(&this->actor);
            Actor_BGcheck2(play, &this->actor, 20.0f, this->collider.dim.radius, this->collider.dim.height,
                                    UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                        UPDBGCHECKINFO_FLAG_4);
        }
        CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
        }
        CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
        if (this->actionFunc == mode_head_up) {
            Actor_world_to_eye(&this->actor, this->skelAnime.curFrame);
        } else if (this->actionFunc == mode_head_down) {
            Actor_world_to_eye(&this->actor,
                           20.0f - ((this->skelAnime.curFrame * 20.0f) / Si2_anime_end_frame(&gHintNutsBurrowAnim)));
        } else {
            Actor_world_to_eye(&this->actor, 20.0f);
        }
    }
}

s32 en_hintnuts_display1(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Vec3f vec;
    f32 curFrame;
    EnHintnuts* this = (EnHintnuts*)thisx;

    if (limbIndex == 5 && this->actionFunc == mode_attack) {
        curFrame = this->skelAnime.curFrame;
        if (curFrame <= 6.0f) {
            vec.y = 1.0f - (curFrame * 0.0833f);
            vec.z = 1.0f + (curFrame * 0.1167f);
            vec.x = 1.0f + (curFrame * 0.1167f);
        } else if (curFrame <= 7.0f) {
            curFrame -= 6.0f;
            vec.y = 0.5f + curFrame;
            vec.z = 1.7f - (curFrame * 0.7f);
            vec.x = 1.7f - (curFrame * 0.7f);
        } else if (curFrame <= 10.0f) {
            vec.y = 1.5f - ((curFrame - 7.0f) * 0.1667f);
            vec.z = 1.0f;
            vec.x = 1.0f;
        } else {
            return false;
        }
        Matrix_scale(vec.x, vec.y, vec.z, MTXMODE_APPLY);
    }
    return false;
}

void En_Hintnuts_actor_draw(Actor* thisx, PlayState* play) {
    EnHintnuts* this = (EnHintnuts*)thisx;

    if (this->actor.params == 0xA) {
        Cheap_gfx_display(play, gHintNutsFlowerDL);
    } else {
        Si2_draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable, en_hintnuts_display1, NULL,
                          this);
    }
}
