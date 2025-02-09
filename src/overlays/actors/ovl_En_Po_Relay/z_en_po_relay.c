/*
 * File: z_en_po_relay.c
 * Overlay: ovl_En_Po_Relay
 * Description: Dampé's Ghost
 */

#include "z_en_po_relay.h"
#include "overlays/actors/ovl_En_Honotrap/z_en_honotrap.h"
#include "assets/objects/object_tk/object_tk.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED)

void En_Po_Relay_actor_ct(Actor* thisx, PlayState* play);
void En_Po_Relay_actor_dt(Actor* thisx, PlayState* play);
void En_Po_Relay_actor_move(Actor* thisx, PlayState* play);
void En_Po_Relay_actor_draw(Actor* thisx, PlayState* play);

void mode_first_talk_wait(EnPoRelay* this, PlayState* play);
void mode_rail_fly(EnPoRelay* this, PlayState* play);
void mode_talk_wait(EnPoRelay* this, PlayState* play);
void mode_first_talk(EnPoRelay* this, PlayState* play);
static void mode_talk(EnPoRelay* this, PlayState* play);
void mode_talk_end(EnPoRelay* this, PlayState* play);
void mode_first_talk_wait_init(EnPoRelay* this);

static Vec3s escape_pos[] = {
    { 0xFFC4, 0xFDEE, 0xF47A }, { 0x0186, 0xFE0C, 0xF47A }, { 0x0186, 0xFE0C, 0xF0F6 }, { 0x00D2, 0xFDEE, 0xF0F6 },
    { 0x00D2, 0xFD9E, 0xEEDA }, { 0x023A, 0xFDC6, 0xEEDA }, { 0x023A, 0xFDC6, 0xED18 }, { 0x00D2, 0xFDC6, 0xED18 },
    { 0x00D2, 0xFDC6, 0xEBCE }, { 0x00D2, 0xFDC6, 0xEAA2 }, { 0x023A, 0xFDC6, 0xEAA2 }, { 0x023A, 0xFDC6, 0xEBB0 },
    { 0x04EC, 0xFD9E, 0xEBB0 }, { 0x0672, 0xFD62, 0xED18 }, { 0x0672, 0xFD30, 0xEE80 }, { 0x07DA, 0xFD26, 0xEE80 },
    { 0x07DA, 0xFD26, 0xEF70 }, { 0x07DA, 0xFD26, 0xF204 }, { 0x0672, 0xFD44, 0xF204 }, { 0x0672, 0xFD6C, 0xF3C6 },
    { 0x088E, 0xFD6C, 0xF3C6 }, { 0x088E, 0xFDB2, 0xF5E2 }, { 0x099C, 0xFDD0, 0xF5E2 }, { 0x0B54, 0xFE66, 0xF772 },
    { 0x0B4E, 0xFE66, 0xF87E }, { 0x0B4A, 0xFE66, 0xF97A }, { 0x0B4A, 0xFE98, 0xF9FC }, { 0x0BAE, 0xFE98, 0xF9FC },
};

ActorProfile En_Po_Relay_Profile = {
    /**/ ACTOR_EN_PO_RELAY,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_TK,
    /**/ sizeof(EnPoRelay),
    /**/ En_Po_Relay_actor_ct,
    /**/ En_Po_Relay_actor_dt,
    /**/ En_Po_Relay_actor_move,
    /**/ En_Po_Relay_actor_draw,
};

static ColliderCylinderInit PoRelayOcPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
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
    { 30, 52, 0, { 0, 0, 0 } },
};

static s32 rail_po_relay_flg = 0;

static InitChainEntry value_init[] = {
    ICHAIN_S8(naviEnemyId, NAVI_ENEMY_DAMPES_GHOST, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 1500, ICHAIN_STOP),
};

static Vec3f pvec = { 0.0f, 1.5f, 0.0f };

static Vec3f pacc = { 0.0f, 0.0f, 0.0f };

static Vec3f local_light_pos = { 0.0f, 1200.0f, 0.0f };

static void* eye_txt[] = {
    gDampeEyeOpenTex,
    gDampeEyeHalfTex,
    gDampeEyeClosedTex,
};

void En_Po_Relay_actor_ct(Actor* thisx, PlayState* play) {
    EnPoRelay* this = (EnPoRelay*)thisx;
    s32 temp;

    ValueSet_process(&this->actor, value_init);
    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 42.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gDampeSkel, &gDampeFloatAnim, this->jointTable, this->morphTable, 18);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &PoRelayOcPipeData);
    this->lightNode = Global_light_list_new(play, &play->lightCtx, &this->lightInfo);
    Light_point_ct(&this->lightInfo, this->actor.home.pos.x, this->actor.home.pos.y, this->actor.home.pos.z,
                              255, 255, 255, 200);
    this->lightColor.a = 255;
    temp = 1;
    if (rail_po_relay_flg != 0) {
        Actor_delete(&this->actor);
    } else {
        rail_po_relay_flg = temp;
        mes_set(play, &this->actor, 65);
        this->textId = this->actor.textId;
        mode_first_talk_wait_init(this);
    }
    this->actor.params &= 0x3F;
}

void En_Po_Relay_actor_dt(Actor* thisx, PlayState* play) {
    EnPoRelay* this = (EnPoRelay*)thisx;

    Global_light_list_delete(play, &play->lightCtx, this->lightNode);
    ClObjPipe_dt(play, &this->collider);
}

void mode_first_talk_wait_init(EnPoRelay* this) {
    this->unk_195 = 32;
    this->pathIndex = 0;
    this->actor.room = -1;
    this->actor.shape.rot.y = 0;
    this->actor.world.rot.y = -0x8000;
    this->actor.colChkInfo.mass = MASS_HEAVY;
    this->actionFunc = mode_first_talk_wait;
}

void set_move_aim_pos(Vec3f* dest, Vec3s* src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

void mode_rail_fly_init(EnPoRelay* this) {
    Vec3f vec;

    set_move_aim_pos(&vec, &escape_pos[this->pathIndex]);
    this->actionTimer = ((s16)(this->actor.shape.rot.y - this->actor.world.rot.y - 0x8000) >> 0xB) % 32U;
    event_timer_set(0);
    this->hookshotSlotFull = INV_CONTENT(ITEM_HOOKSHOT) != ITEM_NONE;
    this->unk_19A = Actor_search_position_angleY(&this->actor, &vec);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    Actor_SE_set(&this->actor, NA_SE_EN_PO_LAUGH);
    this->actionFunc = mode_rail_fly;
}

void mode_talk_wait_init(EnPoRelay* this) {
    this->actor.world.rot.y = this->actor.home.rot.y + 0xC000;
    this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.speed = 0.0f;
    this->actionFunc = mode_talk_wait;
}

void set_po_relay_position_y_ground(EnPoRelay* this) {
    chase_f(&this->actor.home.pos.y, escape_pos[(this->pathIndex >= 28) ? 27 : this->pathIndex].y + 45.0f, 2.0f);
    this->actor.world.pos.y = sin_s(this->unk_195 * 0x800) * 8.0f + this->actor.home.pos.y;
}

void mode_first_talk_wait(EnPoRelay* this, PlayState* play) {
    chase_angle(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x100);
    if (Actor_talk_check(&this->actor, play)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = mode_first_talk;
    } else if (this->actor.xzDistToPlayer < 250.0f) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actor.textId = this->textId;
        Actor_talk_request2(&this->actor, play, 250.0f);
    }
    Actor_level_SE_set(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
}

void mode_first_talk(EnPoRelay* this, PlayState* play) {
    chase_angle(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x100);
    if (Actor_talk_end_check(&this->actor, play)) {
        mes_set(play, &this->actor, 0x2F);
        this->textId = this->actor.textId;
        mode_rail_fly_init(this);
    }
    Actor_level_SE_set(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
}

void mode_rail_fly(EnPoRelay* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f vec;
    f32 speedXZ;
    f32 multiplier;

    if (this->actionTimer != 0) {
        this->actionTimer--;
    }
    if (this->actionTimer == 0 && fqrand() < 0.03f) {
        this->actionTimer = 32;
        if (this->pathIndex < 23) {
            speedXZ = fqrand() * 3.0f;
            if (speedXZ < 1.0f) {
                multiplier = 1.0f;
            } else if (speedXZ < 2.0f) {
                multiplier = -1.0f;
            } else {
                multiplier = 0.0f;
            }
            speedXZ = 30.0f * multiplier;
            Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_HONOTRAP,
                        cos_s(this->unk_19A) * speedXZ + this->actor.world.pos.x, this->actor.world.pos.y,
                        sin_s(this->unk_19A) * speedXZ + this->actor.world.pos.z, 0,
                        (this->unk_19A + 0x8000) - (0x2000 * multiplier), 0, HONOTRAP_TYPE_FLAME_DROP);
        }
    }
    add_calc_short_angle2(&this->actor.world.rot.y, this->unk_19A, 2, 0x1000, 0x100);
    this->actor.shape.rot.y = this->actor.world.rot.y + (this->actionTimer * 0x800) + 0x8000;
    if (this->pathIndex < 23) {
        // If the player travels along a different path to Dampé that converges later
        if ((Math3D_Check2DInArea(660.0f, 840.0f, -4480.0f, -3760.0f, player->actor.world.pos.x,
                                    player->actor.world.pos.z) != 0) ||
            (Math3D_Check2DInArea(1560.0f, 1740.0f, -4030.0f, -3670.0f, player->actor.world.pos.x,
                                    player->actor.world.pos.z) != 0) ||
            (Math3D_Check2DInArea(1580.0f, 2090.0f, -3030.0f, -2500.0f, player->actor.world.pos.x,
                                    player->actor.world.pos.z) != 0)) {
            speedXZ = (this->hookshotSlotFull) ? player->actor.speed * 1.4f : player->actor.speed * 1.2f;
        } else if (this->actor.xzDistToPlayer < 150.0f) {
            speedXZ = (this->hookshotSlotFull) ? player->actor.speed * 1.2f : player->actor.speed;
        } else if (this->actor.xzDistToPlayer < 300.0f) {
            speedXZ = (this->hookshotSlotFull) ? player->actor.speed : player->actor.speed * 0.8f;
        } else if (this->hookshotSlotFull) {
            speedXZ = 4.5f;
        } else {
            speedXZ = 3.5f;
        }
        multiplier = 250.0f - this->actor.xzDistToPlayer;
        multiplier = CLAMP_MIN(multiplier, 0.0f);
        speedXZ += multiplier * 0.02f + 1.0f;
        add_calc2(&this->actor.speed, speedXZ, 0.5f, 1.5f);
    } else {
        add_calc2(&this->actor.speed, 3.5f, 0.5f, 1.5f);
    }
    set_move_aim_pos(&vec, &escape_pos[this->pathIndex]);
    if (Actor_search_position_distanceXZ(&this->actor, &vec) < 40.0f) {
        this->pathIndex++;
        set_move_aim_pos(&vec, &escape_pos[this->pathIndex]);
        if (this->pathIndex == 28) {
            mode_talk_wait_init(this);
        } else if (this->pathIndex == 9) {
            Actor_Environment_sw_On(play, 0x35);
        } else if (this->pathIndex == 17) {
            Actor_Environment_sw_On(play, 0x36);
        } else if (this->pathIndex == 25) {
            Actor_Environment_sw_On(play, 0x37);
        }
    }
    this->unk_19A = Actor_search_position_angleY(&this->actor, &vec);
    Actor_level_SE_set(&this->actor, NA_SE_EN_PO_AWAY - SFX_FLAG);
}

void mode_talk_wait(EnPoRelay* this, PlayState* play) {
    chase_angle(&this->actor.shape.rot.y, -0x4000, 0x800);
    if (Actor_talk_check(&this->actor, play)) {
        this->actionFunc = mode_talk;
    } else if (play->roomCtx.curRoom.num == 5) {
        Actor_delete(&this->actor);
        z_common_data.timerState = TIMER_STATE_OFF;
    } else if (Actor_player_distance_direction_check(&this->actor, 150.0f, 0x3000)) {
        this->actor.textId = this->textId;
        Actor_talk_request2(&this->actor, play, 250.0f);
    }
    Actor_level_SE_set(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
}

static void mode_talk(EnPoRelay* this, PlayState* play) {
    chase_angle(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0x100);
    if (message_check(&play->msgCtx) == TEXT_STATE_EVENT) {
        if (pad_on_check(play)) {
            if (this->hookshotSlotFull != 0) {
                mes_set(play, &this->actor, 0x2E);
            } else {
                mes_set(play, &this->actor, 0x2D);
            }
            this->textId = this->actor.textId;
            message_set2(play, this->actor.textId);
        }
    } else if (Actor_talk_end_check(&this->actor, play)) {
        z_common_data.timerState = TIMER_STATE_OFF;
        this->actionTimer = 0;
        this->actionFunc = mode_talk_end;
    }
    Actor_level_SE_set(&this->actor, NA_SE_EN_PO_FLY - SFX_FLAG);
}

void mode_talk_end(EnPoRelay* this, PlayState* play) {
    Vec3f vec;
    f32 multiplier;
    s32 pad;

    this->actionTimer++;
    if (this->actionTimer < 8) {
        if (this->actionTimer < 5) {
            vec.y = sin_s((this->actionTimer * 0x1000) - 0x4000) * 23.0f + (this->actor.world.pos.y + 40.0f);
            multiplier = cos_s((this->actionTimer * 0x1000) - 0x4000) * 23.0f;
            vec.x =
                (sin_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * multiplier) + this->actor.world.pos.x;
            vec.z =
                (cos_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * multiplier) + this->actor.world.pos.z;
        } else {
            vec.y = this->actor.world.pos.y + 40.0f + 15.0f * (this->actionTimer - 5);
            vec.x = (sin_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * 23.0f) + this->actor.world.pos.x;
            vec.z = (cos_s(getRealCameraAngleY(GET_ACTIVE_CAM(play)) + 0x4800) * 23.0f) + this->actor.world.pos.z;
        }
        _Effect_SS_Db_ct(play, &vec, &pvec, &pacc, this->actionTimer * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, true);
        vec.x = (this->actor.world.pos.x + this->actor.world.pos.x) - vec.x;
        vec.z = (this->actor.world.pos.z + this->actor.world.pos.z) - vec.z;
        _Effect_SS_Db_ct(play, &vec, &pvec, &pacc, this->actionTimer * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, true);
        vec.x = this->actor.world.pos.x;
        vec.z = this->actor.world.pos.z;
        _Effect_SS_Db_ct(play, &vec, &pvec, &pacc, this->actionTimer * 10 + 80, 0, 255, 255, 255, 255,
                             0, 0, 255, 1, 9, true);
        if (this->actionTimer == 1) {
            Actor_SE_set(&this->actor, NA_SE_EN_EXTINCT);
        }
    }
    if (chase_f(&this->actor.scale.x, 0.0f, 0.001f) != 0) {
        if (this->hookshotSlotFull != 0) {
            Vec3f sp60;
            s32 pad1;

            sp60.x = this->actor.world.pos.x;
            sp60.y = this->actor.floorHeight;
            sp60.z = this->actor.world.pos.z;
            if (z_common_data.timerSeconds < HIGH_SCORE(HS_DAMPE_RACE)) {
                HIGH_SCORE(HS_DAMPE_RACE) = z_common_data.timerSeconds;
            }
            if (!Actor_Environment_item_Check(play, this->actor.params) && (z_common_data.timerSeconds <= 60)) {
                Item_set1(play, &sp60, (this->actor.params << 8) + (0x4000 | ITEM00_HEART_PIECE));
            } else {
                Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_ITEM00, sp60.x, sp60.y, sp60.z, 0, 0, 0, 2);
            }
        } else {
            Actor_Environment_no_enemy_On(play, 4);
            HIGH_SCORE(HS_DAMPE_RACE) = z_common_data.timerSeconds;
        }
        Actor_delete(&this->actor);
    }
    this->actor.scale.y = this->actor.scale.x;
    this->actor.scale.z = this->actor.scale.x;
    this->actor.world.pos.y += 10.0f;
}

void En_Po_Relay_actor_move(Actor* thisx, PlayState* play) {
    EnPoRelay* this = (EnPoRelay*)thisx;
    s32 pad;

    Skeleton_Info2_anime_play(&this->skelAnime);
    this->actionFunc(this, play);
    Actor_position_moveF(&this->actor);
    set_po_relay_position_y_ground(this);
    Actor_BGcheck2(play, &this->actor, 0.0f, 27.0f, 60.0f, UPDBGCHECKINFO_FLAG_2);
    CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->actor, &this->collider);
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    Actor_world_to_eye(&this->actor, 50.0f);
    if (this->unk_195 != 0) {
        this->unk_195 -= 1;
    }
    if (this->unk_195 == 0) {
        this->unk_195 = 32;
    }
    this->eyeTextureIdx++;
    if (this->eyeTextureIdx == 3) {
        this->eyeTextureIdx = 0;
    }
}

void en_po_relay_display2(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnPoRelay* this = (EnPoRelay*)thisx;

    if (limbIndex == 14) {
        f32 rand;
        Vec3f vec;

        OPEN_DISPS(play->state.gfxCtx, "../z_en_po_relay.c", 885);
        rand = fqrand();
        this->lightColor.r = (s16)(rand * 30.0f) + 225;
        this->lightColor.g = (s16)(rand * 100.0f) + 155;
        this->lightColor.b = (s16)(rand * 160.0f) + 95;
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, this->lightColor.r, this->lightColor.g, this->lightColor.b, 128);
        gSPDisplayList(POLY_OPA_DISP++, gDampeLanternDL);
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_po_relay.c", 901);
        Matrix_Position(&local_light_pos, &vec);
        Light_point_ct(&this->lightInfo, vec.x, vec.y, vec.z, this->lightColor.r, this->lightColor.g,
                                  this->lightColor.b, 200);
    } else if (limbIndex == 8) {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_po_relay.c", 916);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_po_relay.c", 918);
        gSPDisplayList(POLY_OPA_DISP++, gDampeHaloDL);
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_po_relay.c", 922);
    }
}

void En_Po_Relay_actor_draw(Actor* thisx, PlayState* play) {
    EnPoRelay* this = (EnPoRelay*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_po_relay.c", 940);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eye_txt[this->eyeTextureIdx]));
    Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          en_po_relay_display2, &this->actor);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_po_relay.c", 954);
}
