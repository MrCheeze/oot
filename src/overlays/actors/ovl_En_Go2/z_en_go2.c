#include "z_en_go2.h"
#include "overlays/actors/ovl_En_Bom/z_en_bom.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_oF1d_map/object_oF1d_map.h"
#include "quake.h"
#include "versions.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

/*
FLAGS

EVENTCHKINF_23 - DC entrance boulder blown up as child

InfTable

INFTABLE_B4
INFTABLE_E0 - Talked to DMT Goron at DC entrance (Before DC is opened as child)
INFTABLE_E3 - Talked to GC Goron in bottom level stairwell
INFTABLE_E6 - Talked to GC Goron at LW entrance (Before LW shortcut is opened)
INFTABLE_EB - Talked to DMT Goron at Bomb Flower with goron bracelet
INFTABLE_F0 - Talked to Goron at GC Entrance (Before goron ruby is obtained)
INFTABLE_F4 - Talked to Goron at GC Island (Before goron ruby is obtained)
INFTABLE_F8 - (not on cloud modding) Talked to GC Goron outside Darunias door (after opening door,
before getting goron bracelet)
INFTABLE_109 - Obtained Fire Tunic from Goron Link
INFTABLE_10A - (not on cloud modding)
INFTABLE_10B - Spoke to Goron Link About Volvagia
INFTABLE_10C - Stopped Goron Link's Rolling
INFTABLE_10D - EnGo Exclusive
INFTABLE_10E - Spoke to Goron Link
INFTABLE_10F - (not on cloud modding)

INFTABLE_11E - Bomb bag upgrade obtained from rolling Goron

Gorons only move when this->interactInfo.talkState == NPC_TALK_STATE_IDLE
*/

void En_Go2_actor_ct(Actor* thisx, PlayState* play);
void En_Go2_actor_dt(Actor* thisx, PlayState* play);
void En_Go2_actor_move(Actor* thisx, PlayState* play);
void En_Go2_actor_draw(Actor* thisx, PlayState* play);

void go_end_run_ct(EnGo2* this, PlayState* play);
void go_iwa_wait(EnGo2* this, PlayState* play);

static void go_stand_up(EnGo2* this, PlayState* play);
void go_slide(EnGo2* this, PlayState* play);
void go_start_run(EnGo2* this, PlayState* play);
void go_run_wait(EnGo2* this, PlayState* play);
void go_run(EnGo2* this, PlayState* play);
void go_end_run(EnGo2* this, PlayState* play);

void go_brake(EnGo2* this, PlayState* play);
static void go_carry_request(EnGo2* this, PlayState* play);
static void go_carry_end(EnGo2* this, PlayState* play);
void go_megusuri_demo(EnGo2* this, PlayState* play);
void go_yattana_demo(EnGo2* this, PlayState* play);
void go_nige_demo(EnGo2* this, PlayState* play);

#include "z_en_go2_eff.inc.c"

static ColliderCylinderInit Go2ClPipeData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 40, 65, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 Go2StatusData = { 0, 0, 0, 0, MASS_IMMOVABLE };

ActorProfile En_Go2_Profile = {
    /**/ ACTOR_EN_GO2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_OF1D_MAP,
    /**/ sizeof(EnGo2),
    /**/ En_Go2_actor_ct,
    /**/ En_Go2_actor_dt,
    /**/ En_Go2_actor_move,
    /**/ En_Go2_actor_draw,
};

static EnGo2DataStruct1 collision_ct_data[14] = {
    { 0, 0, 0, 68, 148 }, { 0, 0, 0, 24, 52 }, { 0, 320, 380, 400, 120 }, { 0, 0, 0, 30, 68 }, { 0, 0, 0, 46, 90 },
    { 0, 0, 0, 30, 68 },  { 0, 0, 0, 30, 68 }, { 0, 0, 0, 30, 68 },       { 0, 0, 0, 30, 68 }, { 0, 0, 0, 30, 68 },
    { 0, 0, 0, 30, 68 },  { 0, 0, 0, 30, 68 }, { 0, 0, 0, 30, 68 },       { 0, 0, 0, 30, 68 },
};

static EnGo2DataStruct2 actor_ct_data[14] = {
    { 30.0f, 0.026f, 6, 60.0f }, { 24.0f, 0.008f, 6, 30.0f }, { 28.0f, 0.16f, 5, 380.0f }, { 28.0f, 0.01f, 7, 40.0f },
    { 30.0f, 0.015f, 6, 30.0f }, { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },
    { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },
    { 28.0f, 0.01f, 6, 30.0f },  { 28.0f, 0.01f, 6, 30.0f },
};

static f32 eye_offset[14][2] = {
    { 80.0f, 80.0f }, { -10.0f, -10.0f }, { 800.0f, 800.0f }, { 0.0f, 0.0f },   { 20.0f, 40.0f },
    { 20.0f, 20.0f }, { 20.0f, 20.0f },   { 20.0f, 20.0f },   { 20.0f, 20.0f }, { 20.0f, 20.0f },
    { 20.0f, 20.0f }, { 20.0f, 20.0f },   { 20.0f, 20.0f },   { 20.0f, 20.0f },
};

typedef enum EnGo2Animation {
    /*  0 */ ENGO2_ANIM_0,
    /*  1 */ ENGO2_ANIM_1,
    /*  2 */ ENGO2_ANIM_2,
    /*  3 */ ENGO2_ANIM_3,
    /*  4 */ ENGO2_ANIM_4,
    /*  5 */ ENGO2_ANIM_5,
    /*  6 */ ENGO2_ANIM_6,
    /*  7 */ ENGO2_ANIM_7,
    /*  8 */ ENGO2_ANIM_8,
    /*  9 */ ENGO2_ANIM_9,
    /* 10 */ ENGO2_ANIM_10,
    /* 11 */ ENGO2_ANIM_11,
    /* 12 */ ENGO2_ANIM_12
} EnGo2Animation;

static AnimationInfo animetbl[] = {
    { &gGoronAnim_004930, 0.0f, 0.0f, -1.0f, 0x00, 0.0f },  { &gGoronAnim_004930, 0.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_0029A8, 1.0f, 0.0f, -1.0f, 0x00, -8.0f }, { &gGoronAnim_010590, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_003768, 1.0f, 0.0f, -1.0f, 0x00, -8.0f }, { &gGoronAnim_0038E4, 1.0f, 0.0f, -1.0f, 0x02, -8.0f },
    { &gGoronAnim_002D80, 1.0f, 0.0f, -1.0f, 0x02, -8.0f }, { &gGoronAnim_00161C, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_001A00, 1.0f, 0.0f, -1.0f, 0x00, -8.0f }, { &gGoronAnim_0021D0, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_004930, 0.0f, 0.0f, -1.0f, 0x01, -8.0f }, { &gGoronAnim_000750, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
    { &gGoronAnim_000D5C, 1.0f, 0.0f, -1.0f, 0x00, -8.0f },
};

static EnGo2DustEffectData dust_arg[2][4] = {
    {
        { 12, 0.2f, 0.2f, 1, 18.0f, 0.0f },
        { 12, 0.1f, 0.2f, 12, 26.0f, 0.0f },
        { 12, 0.1f, 0.3f, 4, 10.0f, 0.0f },
        { 12, 0.2f, 0.2f, 1, 18.0f, 0.0f },
    },
    {
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
        { 12, 0.5f, 0.4f, 3, 42.0f, 0.0f },
    },
};

static Vec3f go2_sp = { 0.0f, 0.0f, 0.0f };

#include "z_en_go2_spk.inc.c"

u16 go2_set_message(PlayState* play, Actor* thisx) {
    EnGo2* this = (EnGo2*)thisx;
    u16 textId = get_mask_message(play, MASK_REACTION_SET_GORON);

    if (textId != 0) {
        return textId;
    } else {
        switch (PARAMS_GET_S(this->actor.params, 0, 5)) {
            case GORON_CITY_ROLLING_BIG:
                return boso_go_set_message(play, this);
            case GORON_CITY_LINK:
                return gorokid_set_message(play, this);
            case GORON_DMT_BIGGORON:
                return daigoro_set_message(play, this);
            case GORON_FIRE_GENERIC:
                return help_go_set_message(play, this);
            case GORON_DMT_BOMB_FLOWER:
                return tome_go_set_message(play, this);
            case GORON_DMT_ROLLING_SMALL:
                return b_goron_set_message(play, this);
            case GORON_DMT_DC_ENTRANCE:
                return a_goron_set_message(play, this);
            case GORON_CITY_ENTRANCE:
                return d_goron_set_message(play, this);
            case GORON_CITY_ISLAND:
                return e_goron_set_message(play, this);
            case GORON_CITY_LOWEST_FLOOR:
                return f_goron_set_message(play, this);
            case GORON_CITY_STAIRWELL:
                return i_goron_set_message(play, this);
            case GORON_CITY_LOST_WOODS:
                return j_goron_set_message(play, this);
            case GORON_DMT_FAIRY_HINT:
                return k_goron_set_message(play, this);
            case GORON_MARKET_BAZAAR:
                return hiya_go_set_message(play, this);
        }
    }
#ifdef AVOID_UB
    return textId; // textId is always in the v0 return value register at this point
#endif
}

s16 go2_end_message(PlayState* play, Actor* thisx) {
    EnGo2* this = (EnGo2*)thisx;
    switch (PARAMS_GET_S(this->actor.params, 0, 5)) {
        case GORON_CITY_ROLLING_BIG:
            return boso_go_end_message(play, this);
        case GORON_CITY_LINK:
            return gorokid_end_message(play, this);
        case GORON_DMT_BIGGORON:
            return daigoro_end_message(play, this);
        case GORON_FIRE_GENERIC:
            return help_go_end_message(play, this);
        case GORON_DMT_BOMB_FLOWER:
            return tome_go_end_message(play, this);
        case GORON_DMT_ROLLING_SMALL:
            return b_goron_end_message(play, this);
        case GORON_DMT_DC_ENTRANCE:
            return a_goron_end_message(play, this);
        case GORON_CITY_ENTRANCE:
            return d_goron_end_message(play, this);
        case GORON_CITY_ISLAND:
            return e_goron_end_message(play, this);
        case GORON_CITY_LOWEST_FLOOR:
            return f_goron_end_message(play, this);
        case GORON_CITY_STAIRWELL:
            return i_goron_end_message(play, this);
        case GORON_CITY_LOST_WOODS:
            return j_goron_end_message(play, this);
        case GORON_DMT_FAIRY_HINT:
            return k_goron_end_message(play, this);
        case GORON_MARKET_BAZAAR:
            return hiya_go_end_message(play, this);
    }
#ifdef AVOID_UB
    // The v0 register isn't set in this function, the last value in v0 is the return value of Actor_talk_check
    // called in the function below, which must be false for this function to be called
    return false;
#endif
}

s32 go2_talk(EnGo2* this, PlayState* play) {
    if (PARAMS_GET_S(this->actor.params, 0, 5) != GORON_DMT_BIGGORON &&
        PARAMS_GET_S(this->actor.params, 0, 5) != GORON_CITY_ROLLING_BIG) {
        return npc_talk(play, &this->actor, &this->interactInfo.talkState, this->interactRange,
                                 go2_set_message, go2_end_message);
    } else if ((PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON) &&
               !(this->collider.base.ocFlags2 & OC2_HIT_PLAYER)) {
        return false;
    } else {
        if (Actor_talk_check(&this->actor, play)) {
            this->interactInfo.talkState = NPC_TALK_STATE_TALKING;
            return true;
        } else if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
            this->interactInfo.talkState = go2_end_message(play, &this->actor);
            return false;
        } else if (Actor_talk_request2(&this->actor, play, this->interactRange)) {
            this->actor.textId = go2_set_message(play, &this->actor);
        }
        return false;
    }
}

static void separate_collision_data_set(EnGo2* this) {
    u8 index = PARAMS_GET_S(this->actor.params, 0, 5);

    this->collider.dim.radius = collision_ct_data[index].radius;
    this->collider.dim.height = collision_ct_data[index].height;
}

static void separate_actor_data_set(EnGo2* this) {
    u8 index = PARAMS_GET_S(this->actor.params, 0, 5);

    this->actor.shape.shadowScale = actor_ct_data[index].shape_unk_10;
    Actor_set_scale(&this->actor, actor_ct_data[index].scale);
    this->actor.attentionRangeType = actor_ct_data[index].actor_unk_1F;
    this->interactRange = actor_ct_data[index].interactRange;
    this->interactRange += this->collider.dim.radius;
}

static void set_collision(EnGo2* this, PlayState* play) {
    Vec3s pos;
    f32 xzDist;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y;
    pos.z = this->actor.world.pos.z;
    xzDist = collision_ct_data[PARAMS_GET_S(this->actor.params, 0, 5)].xzDist;
    pos.x += (s16)(xzDist * sin_s(this->actor.shape.rot.y));
    pos.z += (s16)(xzDist * cos_s(this->actor.shape.rot.y));
    pos.y += collision_ct_data[PARAMS_GET_S(this->actor.params, 0, 5)].yDist;
    this->collider.dim.pos = pos;
    CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_setAC(play, &play->colChkCtx, &this->collider.base);
}

static void anime_revers(EnGo2* this) {
    f32 initialFrame;

    initialFrame = this->skelAnime.startFrame;
    this->skelAnime.startFrame = this->skelAnime.endFrame;
    this->skelAnime.endFrame = initialFrame;
}

static s32 hitcheck(EnGo2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 arg2;

    if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON) {
        return false;
    } else {
        if ((this->actionFunc != go_run) && (this->actionFunc != go_brake) &&
            (this->actionFunc != go_run_wait)) {
            return false;
        } else {
            if (this->collider.base.acFlags & AC_HIT) {
                Nai_FxFlagEntry(NA_SE_SY_CORRECT_CHIME, &_dummy_zero_f, 4, &_dummy_one,
                                     &_dummy_one, &_dummy_zero_s8);
                this->actor.flags &= ~ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
                this->collider.base.acFlags &= ~AC_HIT;
                go_end_run_ct(this, play);
                return true;
            }
            if (player->invincibilityTimer <= 0) {
                this->collider.base.ocFlags1 |= OC1_TYPE_PLAYER;
            } else {
                return false;
            }
            if (this->collider.base.ocFlags2 & OC2_HIT_PLAYER) {
                this->collider.base.ocFlags2 &= ~OC2_HIT_PLAYER;

                arg2 = this->actionFunc == go_run_wait ? 1.5f : this->actor.speed * 1.5f;

                play->damagePlayer(play, -4);
                Actor_player_power_damage_set(play, &this->actor, arg2, this->actor.yawTowardsPlayer, 6.0f);
                Actor_SE_set(&player->actor, NA_SE_PL_BODY_HIT);
                this->collider.base.ocFlags1 &= ~OC1_TYPE_PLAYER;
            }
        }
    }
    return false;
}

static s32 get_next_path_point(EnGo2* this, PlayState* play) {
    s32 change;

    if (this->path == NULL) {
        return 0;
    }

    change = (u8)(this->path->count - 1);
    if (this->reverse) {
        this->waypoint--;
        if (this->waypoint < 0) {
            this->waypoint = change - 1;
        }
    } else {
        this->waypoint++;
        if (this->waypoint >= change) {
            this->waypoint = 0;
        }
    }

    return 1;
}

static s32 go_path_move(EnGo2* this, PlayState* play) {
    s16 targetYaw;
    f32 waypointDistSq = path_move(&this->actor, this->path, this->waypoint, &targetYaw);

    add_calc_short_angle2(&this->actor.world.rot.y, targetYaw, 6, 4000, 1);
    if (waypointDistSq > 0.0f && waypointDistSq < SQ(30.0f)) {
        return get_next_path_point(this, play);
    } else {
        return 0;
    }
}

s32 go_turn_to_path_point(EnGo2* this) {
    s16 targetYaw;

    path_move(&this->actor, this->path, this->waypoint, &targetYaw);
    this->actor.world.rot.y = targetYaw;
    return 1;
}

static s32 pl_approach_check(EnGo2* this) {
    s16 yawDiff;
    f32 xyzDist = PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON ? 800.0f : 200.0f;
    f32 yDist = PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON ? 400.0f : 60.0f;
    s16 yawDiffAbs;

    if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON) {
        if (!(this->collider.base.ocFlags2 & OC2_HIT_PLAYER)) {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            return false;
        } else {
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            return true;
        }
    }

    xyzDist = SQ(xyzDist);
    yawDiff = (f32)this->actor.yawTowardsPlayer - (f32)this->actor.shape.rot.y;
    yawDiffAbs = ABS(yawDiff);
    if (this->actor.xyzDistToPlayerSq <= xyzDist && fabsf(this->actor.yDistToPlayer) < yDist && yawDiffAbs < 0x2AA8) {
        return true;
    } else {
        return false;
    }
}

static s32 go_bound(EnGo2* this, s16 arg1, f32 arg2, s16 arg3) {
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) || this->actor.velocity.y > 0.0f) {
        return false;
    }

    if (DECR(this->unk_590)) {
        if (!arg3) {
            return true;
        } else {
            this->actor.world.pos.y =
                (this->unk_590 & 1) ? this->actor.world.pos.y + 1.5f : this->actor.world.pos.y - 1.5f;
            Actor_SE_set(&this->actor, NA_SE_EV_BIGBALL_ROLL - SFX_FLAG);
            return true;
        }
    }

    if (this->unk_59C >= 2) {
        Actor_SE_set(&this->actor, (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_CITY_ROLLING_BIG)
                                        ? NA_SE_EN_GOLON_LAND_BIG
                                        : NA_SE_EN_DODO_M_GND);
    }

    this->unk_59C--;
    if (this->unk_59C <= 0) {
        if (this->unk_59C == 0) {
            this->unk_590 = get_random_timer(60, 30);
            this->unk_59C = 0;
            this->actor.velocity.y = 0.0f;
            return true;
        } else {
            this->unk_59C = arg1;
        }
    }

    this->actor.velocity.y = ((f32)this->unk_59C / (f32)arg1) * arg2;
    return true;
}

void go_item_exchange(EnGo2* this, PlayState* play, Player* player) {
    u16 textId;

    if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON) {
        if (z_common_data.save.info.playerData.bgsFlag) {
            if (Actor_get_item_check(play) == EXCH_ITEM_CLAIM_CHECK) {
                this->actor.textId = 0x3003;
            } else {
                this->actor.textId = 0x305E;
            }
            player->actor.textId = this->actor.textId;

        } else if (!z_common_data.save.info.playerData.bgsFlag && (INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_CLAIM_CHECK)) {
            if (Actor_get_item_check(play) == EXCH_ITEM_CLAIM_CHECK) {
                if (Get_Eventday() >= 3) {
                    textId = 0x305E;
                } else {
                    textId = 0x305D;
                }
                this->actor.textId = textId;
            } else {
                if (Get_Eventday() >= 3) {
                    textId = 0x3002;
                } else {
                    textId = 0x305D;
                }
                this->actor.textId = textId;
            }
            player->actor.textId = this->actor.textId;

        } else if ((INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_PRESCRIPTION) &&
                   (INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_CLAIM_CHECK)) {
            if (Actor_get_item_check(play) == EXCH_ITEM_EYE_DROPS) {
                this->actor.textId = 0x3059;
            } else {
                this->actor.textId = 0x3058;
            }
            if (this->actor.textId == 0x3059) {
                z_common_data.subTimerState = SUBTIMER_STATE_OFF;
            }
            player->actor.textId = this->actor.textId;

        } else if (INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_BROKEN_GORONS_SWORD) {
            if (Actor_get_item_check(play) == EXCH_ITEM_BROKEN_GORONS_SWORD) {
                if (GET_INFTABLE(INFTABLE_B4)) {
                    textId = 0x3055;
                } else {
                    textId = 0x3054;
                }
                this->actor.textId = textId;
            } else {
                this->actor.textId = 0x3053;
            }
            player->actor.textId = this->actor.textId;
        } else {
            this->actor.textId = 0x3053;
            player->actor.textId = this->actor.textId;
        }
    }
}

void go_sub_action(EnGo2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actionFunc != go_nige_demo) {
        this->interactInfo.trackPos = player->actor.world.pos;
        this->interactInfo.yOffset =
            eye_offset[PARAMS_GET_S(this->actor.params, 0, 5)][((void)0, z_common_data.save.linkAge)];
        eye_moveM(&this->actor, &this->interactInfo, 4, this->trackingMode);
    }
    if ((this->actionFunc != go_carry_end) && (this->isAwake == true)) {
        if (go2_talk(this, play)) {
            go_item_exchange(this, play, player);
        }
    }
}

void calc_shadow_alpha(EnGo2* this, f32* alpha) {
    f32 alphaTarget =
        (this->skelAnime.animation == &gGoronAnim_004930) && (this->skelAnime.curFrame <= 32.0f) ? 0.0f : 255.0f;

    add_calc2(alpha, alphaTarget, 0.4f, 100.0f);
    this->actor.shape.shadowAlpha = (u8)(u32)*alpha;
}

void goron_position_move(EnGo2* this) {
    f32 speedXZ = this->actor.speed;

    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        this->actor.speed = 0.0f;
    }

    if (this->actionFunc != go_run_wait) {
        Actor_position_moveF(&this->actor);
    }

    this->actor.speed = speedXZ;
}

void go_set_wait_anime(EnGo2* this) {
    switch (PARAMS_GET_S(this->actor.params, 0, 5)) {
        case GORON_CITY_ROLLING_BIG:
        case GORON_DMT_DC_ENTRANCE:
        case GORON_CITY_ENTRANCE:
        case GORON_CITY_STAIRWELL:
        case GORON_DMT_FAIRY_HINT:
            npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_9);
            break;
        case GORON_DMT_BIGGORON:
            if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_BROKEN_GORONS_SWORD &&
                INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_EYE_DROPS) {
                npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_4);
                break;
            }
            FALLTHROUGH;
        default:
            this->skelAnime.playSpeed = 0.0f;
            break;
    }
}

f32 go_run_speed(EnGo2* this) {
    f32 yDist = PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON ? 400.0f : 60.0f;
    s32 index = PARAMS_GET_S(this->actor.params, 0, 5);

    if (index == GORON_CITY_LINK && (fabsf(this->actor.yDistToPlayer) < yDist) &&
        (this->actor.xzDistToPlayer < 400.0f)) {
        return 9.0f;
    } else {
        return index == GORON_CITY_ROLLING_BIG ? 3.6000001f : 6.0f;
    }
}

s32 keep_stand_check(EnGo2* this, PlayState* play) {
    Camera* mainCam = play->cameraPtrs[CAM_ID_MAIN];

    if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON) {
        if (pl_approach_check(this)) {
            changeCameraSet(mainCam, CAM_SET_DIRECTED_YAW);
            clearCameraFlag(mainCam, CAM_STATE_CHECK_BG);
        } else if (!pl_approach_check(this) && (mainCam->setting == CAM_SET_DIRECTED_YAW)) {
            changeCameraSet(mainCam, CAM_SET_DUNGEON1);
            setCameraFlag(mainCam, CAM_STATE_CHECK_BG);
        }
    }

    if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_FIRE_GENERIC ||
        PARAMS_GET_S(this->actor.params, 0, 5) == GORON_CITY_ROLLING_BIG ||
        PARAMS_GET_S(this->actor.params, 0, 5) == GORON_CITY_STAIRWELL ||
        PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON ||
        PARAMS_GET_S(this->actor.params, 0, 5) == GORON_MARKET_BAZAAR) {
        return true;
    } else if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_GORON)) {
        return true;
    } else {
        return false;
    }
}

void lookmode_sub1(EnGo2* this) {
    if (pl_approach_check(this)) {
        this->trackingMode = NPC_TRACKING_HEAD_AND_TORSO;
    } else {
        this->trackingMode = NPC_TRACKING_NONE;
    }

    if (this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        this->trackingMode = NPC_TRACKING_FULL_BODY;
    }

    this->isAwake = true;
}

void lookmode_sub2(EnGo2* this) {
    f32 xyzDist = PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON ? 800.0f : 200.0f;
    s32 isTrue = true;

    xyzDist = SQ(xyzDist);
    this->trackingMode = NPC_TRACKING_NONE;
    if ((this->actor.xyzDistToPlayerSq <= xyzDist) || (this->interactInfo.talkState != NPC_TALK_STATE_IDLE)) {
        this->trackingMode = NPC_TRACKING_FULL_BODY;
    }

    this->isAwake = isTrue;
}

void lookmode_sub3(EnGo2* this) {
    if (pl_approach_check(this) || this->interactInfo.talkState != NPC_TALK_STATE_IDLE) {
        this->trackingMode = NPC_TRACKING_HEAD_AND_TORSO;
        this->isAwake = true;
    } else {
        this->trackingMode = NPC_TRACKING_NONE;
        this->isAwake = false;
    }
}

void set_lookmode(EnGo2* this) {
    switch (PARAMS_GET_S(this->actor.params, 0, 5)) {
        case GORON_DMT_BOMB_FLOWER:
            this->isAwake = true;
            this->trackingMode = pl_approach_check(this) ? NPC_TRACKING_HEAD_AND_TORSO : NPC_TRACKING_NONE;
            break;
        case GORON_FIRE_GENERIC:
            lookmode_sub2(this);
            break;
        case GORON_DMT_BIGGORON:
            lookmode_sub3(this);
            break;
        case GORON_CITY_LINK:
            if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_GORON)) {
                lookmode_sub2(this);
                break;
            }
            FALLTHROUGH;
        default:
            lookmode_sub1(this);
            break;
    }
}

static void eye_paci2(EnGo2* this) {
    switch (this->eyeMouthTexState) {
        case 1:
            this->blinkTimer = 0;
            this->eyeTexIndex = 0;
            this->mouthTexIndex = 0;
            break;
        case 2:
            this->blinkTimer = 0;
            this->eyeTexIndex = 1;
            this->mouthTexIndex = 0;
            break;
        // case 3 only when biggoron is given eyedrops. Biggoron smiles. (only use of second mouth texture)
        case 3:
            this->blinkTimer = 0;
            this->eyeTexIndex = 0;
            this->mouthTexIndex = 1;
            break;
        default:
            if (DECR(this->blinkTimer) == 0) {
                this->eyeTexIndex++;
                if (this->eyeTexIndex >= 4) {
                    this->blinkTimer = get_random_timer(30, 30);
                    this->eyeTexIndex = 1;
                }
            }
    }
}

void set_se(EnGo2* this) {
    if ((this->skelAnime.playSpeed != 0.0f) && (this->skelAnime.animation == &gGoronAnim_004930)) {
        if (this->skelAnime.playSpeed > 0.0f && this->skelAnime.curFrame == 14.0f) {
            if (PARAMS_GET_S(this->actor.params, 0, 5) != GORON_DMT_BIGGORON) {
                Actor_SE_set(&this->actor, NA_SE_EN_GOLON_SIT_DOWN);
            } else {
                Na_StartFxmixSe(&_dummy_zero_f, NA_SE_EN_GOLON_SIT_DOWN, 60);
            }
        }
        if (this->skelAnime.playSpeed < 0.0f) {
            if (this->skelAnime.curFrame == 1.0f) {
                Actor_SE_set(&this->actor, NA_SE_EN_DODO_M_GND);
            }
            if (this->skelAnime.curFrame == 40.0f) {
                Actor_SE_set(&this->actor, NA_SE_EN_GOLON_SIT_DOWN);
            }
        }
    }
}

void set_dust_effect_main(EnGo2* this, s32 index2) {
    s32 index1 = PARAMS_GET_S(this->actor.params, 0, 5) == GORON_CITY_ROLLING_BIG ? 1 : 0;
    EnGo2DustEffectData* dustEffectData = &dust_arg[index1][index2];

    set_dust_effect(this, dustEffectData->initialTimer, dustEffectData->scale, dustEffectData->scaleStep,
                    dustEffectData->numDustEffects, dustEffectData->radius, dustEffectData->yAccel);
}

void go_iwa_wait_ct(EnGo2* this, PlayState* play) {
    if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_10);
        this->skelAnime.playSpeed = -0.5f;
    } else {
        npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_1);
        this->skelAnime.playSpeed = -1.0f;
    }
    anime_revers(this);
    this->trackingMode = NPC_TRACKING_NONE;
    this->unk_211 = false;
    this->isAwake = false;
    this->actionFunc = go_iwa_wait;
}

void go_stand_up_ct1(EnGo2* this, PlayState* play) {
    if (this->skelAnime.playSpeed == 0.0f) {
        if (PARAMS_GET_S(this->actor.params, 0, 5) != GORON_DMT_BIGGORON) {
            Actor_SE_set(&this->actor, NA_SE_EN_GOLON_WAKE_UP);
        } else {
            Na_StartFxmixSe(&_dummy_zero_f, NA_SE_EN_GOLON_WAKE_UP, 60);
        }
    }
    if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON) {
        makeOnepointDemo(play, 4200, -99, &this->actor, CAM_ID_MAIN);
        npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_10);
        this->skelAnime.playSpeed = 0.5f;
    } else {
        npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_1);
        this->skelAnime.playSpeed = 1.0f;
    }
    this->actionFunc = go_stand_up;
}

void go_stand_up_ct2(EnGo2* this, PlayState* play) {
    npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_1);
    this->unk_211 = true;
    this->actionFunc = go_stand_up;
    this->skelAnime.playSpeed = 0.0f;
    this->actor.speed = 0.0f;
    this->skelAnime.curFrame = this->skelAnime.endFrame;
}

void go_run_wait_ct(EnGo2* this, PlayState* play) {
    if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_CITY_ROLLING_BIG ||
        PARAMS_GET_S(this->actor.params, 0, 5) == GORON_CITY_LINK) {
        this->collider.elem.acElemFlags = ACELEM_ON;
        this->actor.speed = GET_INFTABLE(INFTABLE_11E) ? 6.0f : 3.6000001f;
    } else {
        this->actor.speed = 6.0f;
    }
    this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
    this->animTimer = 10;
    this->actor.shape.yOffset = 1800.0f;
    this->actor.speed *= 2.0f; // Speeding up
    this->actionFunc = go_run_wait;
}

void go_end_run_ct(EnGo2* this, PlayState* play) {
    EnBom* bomb;

    if ((PARAMS_GET_S(this->actor.params, 0, 5) != GORON_CITY_ROLLING_BIG) &&
        (PARAMS_GET_S(this->actor.params, 0, 5) != GORON_CITY_LINK)) {
        if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_ROLLING_SMALL) {
            bomb = (EnBom*)Actor_info_make_actor(&play->actorCtx, play, ACTOR_EN_BOM, this->actor.world.pos.x,
                                       this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0);
            if (bomb != NULL) {
                bomb->timer = 0;
            }
        }
    } else {
        this->collider.elem.acElemFlags = ACELEM_NONE;
    }

    this->actor.shape.rot = this->actor.world.rot;
    this->unk_59C = 0;
    this->unk_590 = 0;
    this->actionFunc = go_end_run;
    this->actor.shape.yOffset = 0.0f;
    this->actor.speed = 0.0f;
}

s32 help_go_iwa_wait(EnGo2* this, PlayState* play) {
    if (PARAMS_GET_S(this->actor.params, 0, 5) != GORON_FIRE_GENERIC) {
        return false;
    }

    // shaking curled up
    this->actor.world.pos.x += (play->state.frames & 1) ? 1.0f : -1.0f;
    if (Actor_Environment_sw_Check(play, PARAMS_GET_S(this->actor.params, 10, 6))) {
        return true;
    }
    return false;
}

s32 tome_go_move_check(EnGo2* this) {
    if (PARAMS_GET_S(this->actor.params, 0, 5) != GORON_DMT_BOMB_FLOWER ||
        this->interactInfo.talkState != NPC_TALK_STATE_ACTION) {
        return false;
    }

    npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_3);
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    this->isAwake = false;
    this->trackingMode = NPC_TRACKING_NONE;
    this->actionFunc = go_slide;
    return true;
}

s32 go_run_start_check(EnGo2* this, PlayState* play) {
    if (PARAMS_GET_S(this->actor.params, 0, 5) != GORON_CITY_ROLLING_BIG ||
        (this->interactInfo.talkState != NPC_TALK_STATE_ACTION)) {
        return false;
    }
    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    go_iwa_wait_ct(this, play);
    this->actionFunc = go_start_run;
    return true;
}

s32 nige_demo_start_check(EnGo2* this) {
    if (PARAMS_GET_S(this->actor.params, 0, 5) != GORON_FIRE_GENERIC ||
        this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        return false;
    }
    this->actionFunc = go_nige_demo;
    return true;
}

s32 go_brake_check(EnGo2* this) {
    if (PARAMS_GET_S(this->actor.params, 0, 5) != GORON_CITY_LINK || (this->waypoint >= this->unk_216) ||
        !pl_approach_check(this)) {
        return false;
    }
    return true;
}

s32 go_talk_action(EnGo2* this) {
    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE || this->actor.speed < 1.0f) {
        return false;
    }
    if (go_bound(this, 2, 20.0 / 3.0f, 0)) {
        if ((this->unk_590 >= 9) && (this->unk_59C == 0)) {
            this->unk_590 = 8;
        }
        set_dust_effect_main(this, 0);
    }
    return true;
}

void gorokid_chg_anime(EnGo2* this, PlayState* play) {
    s32 animation = ARRAY_COUNT(animetbl);

    if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_CITY_LINK) {
        if ((this->actor.textId == 0x3035 && this->unk_20C == 0) ||
            (this->actor.textId == 0x3036 && this->unk_20C == 0)) {
            if (this->skelAnime.animation != &gGoronAnim_000D5C) {
                animation = ENGO2_ANIM_12;
                this->eyeMouthTexState = 0;
            }
        }

        if ((this->actor.textId == 0x3032 && this->unk_20C == 12) || (this->actor.textId == 0x3033) ||
            (this->actor.textId == 0x3035 && this->unk_20C == 6)) {
            if (this->skelAnime.animation != &gGoronAnim_000750) {
                animation = ENGO2_ANIM_11;
                this->eyeMouthTexState = 1;
            }
        }

        if (this->skelAnime.animation == &gGoronAnim_000750) {
            if (this->skelAnime.curFrame == 20.0f) {
                Actor_SE_set(&this->actor, NA_SE_EN_GOLON_CRY);
            }
        }

        if (animation != ARRAY_COUNT(animetbl)) {
            npc_anime_ct(&this->skelAnime, animetbl, animation);
        }
    }
}

void nige_demo_camera_set(EnGo2* this, PlayState* play) {
    s16 yaw;

    this->subCamId = Gama_play_make_camera(play);
    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_WAIT);
    Gama_play_set_camera_status(play, this->subCamId, CAM_STAT_ACTIVE);
    get_path_goal_position(this->path, &this->subCamAt);
    yaw = search_position_angleY(&this->actor.world.pos, &this->subCamAt) + 0xE38;
    this->subCamEye.x = sin_s(yaw) * 100.0f + this->actor.world.pos.x;
    this->subCamEye.z = cos_s(yaw) * 100.0f + this->actor.world.pos.z;
    this->subCamEye.y = this->actor.world.pos.y + 20.0f;
    this->subCamAt.x = this->actor.world.pos.x;
    this->subCamAt.y = this->actor.world.pos.y + 40.0f;
    this->subCamAt.z = this->actor.world.pos.z;
    Gama_play_camera_setting(play, this->subCamId, &this->subCamAt, &this->subCamEye);
}

void nige_demo_camera_del(EnGo2* this, PlayState* play) {
    Gama_play_set_camera_status(play, CAM_ID_MAIN, CAM_STAT_ACTIVE);
    Gama_play_clear_camera(play, this->subCamId);
}

void daigoro_kaii_se_set(EnGo2* this) {
    if (INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_BROKEN_GORONS_SWORD && INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_EYE_DROPS &&
        PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON &&
        this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        if (DECR(this->animTimer) == 0) {
            this->animTimer = get_random_timer(30, 30);
            Na_StartFxmixSe(&_dummy_zero_f, NA_SE_EN_GOLON_EYE_BIG, 60);
        }
    }
}

void En_Go2_actor_ct(Actor* thisx, PlayState* play) {
    EnGo2* this = (EnGo2*)thisx;
    s32 pad;

    Shape_Info_init(&this->actor.shape, 0.0f, Actor_shadow_circle, 28.0f);
    Skeleton_Info2_SV_M_ct(play, &this->skelAnime, &gGoronSkel, NULL, this->jointTable, this->morphTable, 18);
    ClObjPipe_ct(play, &this->collider);
    ClObjPipe_set5(play, &this->collider, &this->actor, &Go2ClPipeData);
    CollisionCheck_Status_set3(&this->actor.colChkInfo, NULL, &Go2StatusData);

    // Not GORON_CITY_ROLLING_BIG, GORON_CITY_LINK, GORON_DMT_BIGGORON
    switch (PARAMS_GET_S(this->actor.params, 0, 5)) {
        case GORON_FIRE_GENERIC:
        case GORON_DMT_BOMB_FLOWER:
        case GORON_DMT_ROLLING_SMALL:
        case GORON_DMT_DC_ENTRANCE:
        case GORON_CITY_ENTRANCE:
        case GORON_CITY_ISLAND:
        case GORON_CITY_LOWEST_FLOOR:
        case GORON_CITY_STAIRWELL:
        case GORON_CITY_LOST_WOODS:
        case GORON_DMT_FAIRY_HINT:
        case GORON_MARKET_BAZAAR:
            this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            this->actor.flags &= ~ACTOR_FLAG_DRAW_CULLING_DISABLED;
    }

    separate_collision_data_set(this);
    separate_actor_data_set(this);
    npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_0);
    this->actor.gravity = -1.0f;
    this->alpha = this->actor.shape.shadowAlpha = 0;
    this->reverse = 0;
    this->isAwake = false;
    this->unk_211 = false;
    this->goronState = 0;
    this->waypoint = 0;
    this->unk_216 = this->actor.shape.rot.z;
    this->trackingMode = NPC_TRACKING_NONE;
    this->path = get_path_data(play, PARAMS_GET_S(this->actor.params, 5, 5), 0x1F);
    switch (PARAMS_GET_S(this->actor.params, 0, 5)) {
        case GORON_CITY_ENTRANCE:
        case GORON_CITY_ISLAND:
        case GORON_CITY_LOWEST_FLOOR:
        case GORON_CITY_STAIRWELL:
        case GORON_CITY_LOST_WOODS:
            if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) && LINK_IS_ADULT) {
                Actor_delete(&this->actor);
            }
            this->actionFunc = go_iwa_wait;
            break;
        case GORON_MARKET_BAZAAR:
            if ((LINK_IS_ADULT) || !CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
                Actor_delete(&this->actor);
            }
            go_stand_up_ct2(this, play);
            break;
        case GORON_CITY_LINK:
            if (GET_INFTABLE(INFTABLE_109)) {
                get_path_goal_position(this->path, &this->actor.world.pos);
                this->actor.home.pos = this->actor.world.pos;
                if (!CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) &&
                    CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_GORON)) {
                    go_stand_up_ct2(this, play);
                } else {
                    this->actionFunc = go_iwa_wait;
                }
            } else {
#if OOT_VERSION >= PAL_1_1
                CLEAR_INFTABLE(INFTABLE_10C);
#endif
                this->collider.dim.height = (collision_ct_data[PARAMS_GET_S(this->actor.params, 0, 5)].height * 0.6f);
                go_run_wait_ct(this, play);
                this->isAwake = true;
            }
            break;
        case GORON_CITY_ROLLING_BIG:
        case GORON_DMT_ROLLING_SMALL:
            this->collider.dim.height = (collision_ct_data[PARAMS_GET_S(this->actor.params, 0, 5)].height * 0.6f);
            go_run_wait_ct(this, play);
            break;
        case GORON_FIRE_GENERIC:
            if (Actor_Environment_sw_Check(play, PARAMS_GET_S(this->actor.params, 10, 6))) {
                Actor_delete(&this->actor);
            } else {
                this->isAwake = true;
                this->actionFunc = go_iwa_wait;
            }
            break;
        case GORON_DMT_BIGGORON:
            this->actor.shape.shadowDraw = NULL;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            if ((INV_CONTENT(ITEM_TRADE_ADULT) >= ITEM_BROKEN_GORONS_SWORD) &&
                (INV_CONTENT(ITEM_TRADE_ADULT) <= ITEM_EYE_DROPS)) {
                this->eyeMouthTexState = 1;
            }
            this->collider.base.acFlags = AC_NONE;
            this->collider.base.ocFlags1 = OC1_ON | OC1_NO_PUSH | OC1_TYPE_PLAYER;
            this->actionFunc = go_iwa_wait;
            break;
        case GORON_DMT_BOMB_FLOWER:
            if (GET_INFTABLE(INFTABLE_EB)) {
                get_path_goal_position(this->path, &this->actor.world.pos);
                this->actor.home.pos = this->actor.world.pos;
            }
            FALLTHROUGH;
        case GORON_DMT_DC_ENTRANCE:
        case GORON_DMT_FAIRY_HINT:
        default:
            this->actionFunc = go_iwa_wait;
            break;
    }
}

void En_Go2_actor_dt(Actor* thisx, PlayState* play) {
}

void go_iwa_wait(EnGo2* this, PlayState* play) {
    u8 index = PARAMS_GET_S(this->actor.params, 0, 5);
    s16 height;
    s32 quakeIndex;

    if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
        if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON) {
            quakeIndex = startQuake(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
            setSpeedQuake(quakeIndex, -0x3CB0);
            setScaleQuake(quakeIndex, 8, 0, 0, 0);
            setTimerQuake(quakeIndex, 16);
        } else {
            set_dust_effect_main(this, 1);
        }
        this->skelAnime.playSpeed = 0.0f;
    }

    if ((s32)this->skelAnime.curFrame == 0) {
        this->collider.dim.height = (collision_ct_data[index].height * 0.6f);
    } else {
        height = collision_ct_data[index].height;
        this->collider.dim.height =
            ((collision_ct_data[index].height * 0.4f * (this->skelAnime.curFrame / this->skelAnime.startFrame)) +
             (height * 0.6f));
    }
    if (help_go_iwa_wait(this, play)) {
        this->isAwake = false;
        go_stand_up_ct1(this, play);
    }
    if ((PARAMS_GET_S(this->actor.params, 0, 5) != GORON_FIRE_GENERIC) && pl_approach_check(this)) {
        go_stand_up_ct1(this, play);
    }
}

static void go_stand_up(EnGo2* this, PlayState* play) {
    u8 index = PARAMS_GET_S(this->actor.params, 0, 5);
    f32 height;

    if (this->unk_211 == true) {
        daigoro_kaii_se_set(this);
        gorokid_chg_anime(this, play);
        set_lookmode(this);

        if (!go_run_start_check(this, play) && !nige_demo_start_check(this)) {
            if (tome_go_move_check(this)) {
                return;
            }
        } else {
            return;
        }
    } else {
        if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
            if (PARAMS_GET_S(this->actor.params, 0, 5) == GORON_DMT_BIGGORON) {
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            }
            go_set_wait_anime(this);
            this->unk_211 = true;
            this->collider.dim.height = collision_ct_data[index].height;
        } else {
            height = collision_ct_data[index].height;
            this->collider.dim.height =
                (s16)((height * 0.4f * (this->skelAnime.curFrame / this->skelAnime.endFrame)) + (height * 0.6f));
        }
    }
    if ((!keep_stand_check(this, play)) && (!pl_approach_check(this))) {
        go_iwa_wait_ct(this, play);
    }
}

void go_slide(EnGo2* this, PlayState* play) {
    f32 float1 = this->skelAnime.endFrame;
    f32 float2 = this->skelAnime.curFrame * ((f32)0x8000 / float1);

    this->actor.speed = sin_s(float2);
    if ((go_path_move(this, play)) && (this->waypoint == 0)) {
        go_stand_up_ct2(this, play);
    }
}

void go_start_run(EnGo2* this, PlayState* play) {
    if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
        set_dust_effect_main(this, 1);
        this->skelAnime.playSpeed = 0.0f;
        go_run_wait_ct(this, play);
    }
}

void go_run_wait(EnGo2* this, PlayState* play) {
    f32 float1 = 1000.0f;

    if ((PARAMS_GET_S(this->actor.params, 0, 5) != GORON_DMT_ROLLING_SMALL ||
         !(this->actor.xyzDistToPlayerSq > SQ(float1))) &&
        DECR(this->animTimer) == 0) {
        this->actionFunc = go_run;
        this->actor.speed *= 0.5f; // slowdown
    }
    set_dust_effect_main(this, 2);
}

void go_run(EnGo2* this, PlayState* play) {
    s32 orientation;
    s32 index;

    if (!go_talk_action(this)) {
        if (go_bound(this, 4, 8.0f, 1) == true) {
            if (go_brake_check(this)) {
                this->actionFunc = go_brake;
                return;
            }
            set_dust_effect_main(this, 3);
        }
        orientation = go_path_move(this, play);
        index = PARAMS_GET_S(this->actor.params, 0, 5);
        if (index != GORON_CITY_LINK) {
            if ((index == GORON_DMT_ROLLING_SMALL) && (orientation == 1) && (this->waypoint == 0)) {
                go_end_run_ct(this, play);
                return;
            }
        } else if ((orientation == 2) && (this->waypoint == 1)) {
            go_end_run_ct(this, play);
            return;
        }
        add_calc2(&this->actor.speed, go_run_speed(this), 0.4f, 0.6f);
        this->actor.shape.rot = this->actor.world.rot;
    }
}

void go_end_run(EnGo2* this, PlayState* play) {
    if (go_bound(this, 4, 8.0f, 0)) {
        set_dust_effect_main(this, 0);
        if (this->unk_59C == 0) {
            switch (PARAMS_GET_S(this->actor.params, 0, 5)) {
                case GORON_CITY_LINK:
                    this->goronState = 0;
                    this->actionFunc = go_yattana_demo;
                    break;
                case GORON_CITY_ROLLING_BIG:
                    go_stand_up_ct1(this, play);
                    break;
                default:
                    this->actionFunc = go_iwa_wait;
            }
        }
    }
}

void go_brake(EnGo2* this, PlayState* play) {
    if (!go_talk_action(this)) {
        add_calc2(&this->actor.speed, 0.0f, 0.6f, 0.8f);
        if (this->actor.speed >= 1.0f) {
            set_dust_effect_main(this, 3);
        }
        if ((s32)this->actor.speed == 0) {
            this->actor.world.rot.y ^= 0x8000;
            this->actor.shape.rot.y = this->actor.world.rot.y;
            this->reverse ^= 1;
            get_next_path_point(this, play);
            go_run_wait_ct(this, play);
        }
    }
}

static void go_carry_request(EnGo2* this, PlayState* play) {
    if (Actor_carry_check(&this->actor, play)) {
#if OOT_VERSION >= PAL_1_0
        this->actor.parent = NULL;
#endif
        this->actionFunc = go_carry_end;
    } else {
        Actor_carry_request_set2(&this->actor, play, this->getItemId, this->actor.xzDistToPlayer + 1.0f,
                           fabsf(this->actor.yDistToPlayer) + 1.0f);
    }
}

static void go_carry_end(EnGo2* this, PlayState* play) {
    if ((message_check(&play->msgCtx) == TEXT_STATE_DONE) && pad_on_check(play)) {
        this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
        switch (this->getItemId) {
            case GI_CLAIM_CHECK:
                Clr_Eventday();
                go_stand_up_ct2(this, play);
                return;
            case GI_TUNIC_GORON:
                SET_INFTABLE(INFTABLE_109);
                go_stand_up_ct2(this, play);
                return;
            case GI_SWORD_BIGGORON:
                z_common_data.save.info.playerData.bgsFlag = true;
                break;
            case GI_BOMB_BAG_30:
            case GI_BOMB_BAG_40:
                go_iwa_wait_ct(this, play);
                this->actionFunc = go_start_run;
                return;
        }
        this->actionFunc = go_stand_up;
    }
}

void go_megusuri_demo(EnGo2* this, PlayState* play) {
    switch (this->goronState) {
        case 0:
            npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_5);
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.shape.rot.y += 0x5B0;
            this->trackingMode = NPC_TRACKING_NONE;
            this->animTimer = this->skelAnime.endFrame + 60.0f + 60.0f; // eyeDrops animation timer
            this->eyeMouthTexState = 2;
            this->unk_20C = 0;
            this->goronState++;
            Na_SetForceBgmVol(0x28, 5);
            makeOnepointDemo(play, 4190, -99, &this->actor, CAM_ID_MAIN);
            break;
        case 1:
            if (DECR(this->animTimer)) {
                if (this->animTimer == 60 || this->animTimer == 120) {
                    restartCameraStoped(GET_ACTIVE_CAM(play));
                    Na_StartFxmixSe(&_dummy_zero_f, NA_SE_EV_GORON_WATER_DROP, 60);
                }
            } else {
                Na_StartFxmixSe(&_dummy_zero_f, NA_SE_EN_GOLON_GOOD_BIG, 60);
                npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_6);
                message_set2(play, 0x305A);
                this->eyeMouthTexState = 3;
                this->goronState++;
                Na_SetForceBgmVol(0x7F, 5);
            }
            break;
        case 2:
            if (Skeleton_Info_frame_check(&this->skelAnime, this->skelAnime.endFrame)) {
                this->eyeMouthTexState = 0;
            }
            if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
                npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_1);
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                this->trackingMode = NPC_TRACKING_HEAD_AND_TORSO;
                this->skelAnime.playSpeed = 0.0f;
                this->skelAnime.curFrame = this->skelAnime.endFrame;
                carry_request_set(this, play, GI_CLAIM_CHECK);
                this->actionFunc = go_carry_request;
                this->goronState = 0;
            }
            break;
    }
}

void go_yattana_demo(EnGo2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->goronState) {
        case 0:
            if (message_check(&play->msgCtx) != TEXT_STATE_NONE) {
                return;
            } else {
                message_set(play, 0x3031, NULL);
                player->actor.freezeTimer = 10;
                this->goronState++;
            }
        case 1:
            break;
        default:
            return;
    }

    if (message_check(&play->msgCtx) != TEXT_STATE_CLOSING) {
        player->actor.freezeTimer = 10;
    } else {
        SET_INFTABLE(INFTABLE_10C);
        this->trackingMode = NPC_TRACKING_NONE;
        this->unk_211 = false;
        this->isAwake = false;
        this->actionFunc = go_iwa_wait;
    }
}

void go_nige_demo(EnGo2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3s zeroVec = { 0x00, 0x00, 0x00 };

    switch (this->goronState) {
        case 0: // Wake up
            if (message_check(&play->msgCtx) == TEXT_STATE_CLOSING) {
                nige_demo_camera_set(this, play);
                play->msgCtx.msgMode = MSGMODE_PAUSED;
                npc_anime_ct(&this->skelAnime, animetbl, ENGO2_ANIM_2);
                this->waypoint = 1;
                this->skelAnime.playSpeed = 2.0f;
                go_turn_to_path_point(this);
                this->actor.shape.rot = this->actor.world.rot;
                this->animTimer = 60;
                this->actor.gravity = 0.0f;
                this->actor.speed = 2.0f;
                this->interactInfo.headRot = zeroVec;
                this->interactInfo.torsoRot = zeroVec;
                this->goronState++;
                this->goronState++;
                player->actor.world.rot.y = this->actor.world.rot.y;
                player->actor.shape.rot.y = this->actor.world.rot.y;
                player->actor.world.pos.x =
                    (f32)((sin_s(this->actor.world.rot.y) * -30.0f) + this->actor.world.pos.x);
                player->actor.world.pos.z =
                    (f32)((cos_s(this->actor.world.rot.y) * -30.0f) + this->actor.world.pos.z);
                player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_8);
                Na_StartFanfare(NA_BGM_APPEAR);
            }
            break;
        case 2: // Walking away
            if (DECR(this->animTimer)) {
                if (!(this->animTimer % 8)) {
                    Actor_SE_set(&this->actor, NA_SE_EN_MORIBLIN_WALK);
                }
                Actor_position_moveF(&this->actor);
            } else {
                this->animTimer = 0;
                this->actor.speed = 0.0f;
                if ((PARAMS_GET_S(this->actor.params, 10, 6) != 1) && (PARAMS_GET_S(this->actor.params, 10, 6) != 2) &&
                    (PARAMS_GET_S(this->actor.params, 10, 6) != 4) && (PARAMS_GET_S(this->actor.params, 10, 6) != 5) &&
                    (PARAMS_GET_S(this->actor.params, 10, 6) != 9) && (PARAMS_GET_S(this->actor.params, 10, 6) != 11)) {
                    this->goronState++;
                }
                this->goronState++;
            }
            break;
        case 3: // Walking away
            this->animTimer++;
            if (!(this->animTimer % 8) && (this->animTimer < 10)) {
                Actor_SE_set(&this->actor, NA_SE_EN_MORIBLIN_WALK);
            }
            if (this->animTimer == 10) {
                Actor_SE_set(&this->actor, NA_SE_EV_IRON_DOOR_OPEN);
            }
            if (this->animTimer > 44) {
                Effect_SE_Info_new(play, &this->actor.world.pos, 20, NA_SE_EV_IRON_DOOR_CLOSE);
            } else {
                break;
            }
        case 4: // Finalize walking away
            message_close(play);
            nige_demo_camera_del(this, play);
            player_demo_mode_set(play, &this->actor, PLAYER_CSACTION_7);
            Actor_delete(&this->actor);
            break;
        case 1:
            break;
    }
}

void En_Go2_actor_move(Actor* thisx, PlayState* play) {
    EnGo2* this = (EnGo2*)thisx;

    calc_shadow_alpha(this, &this->alpha);
    set_se(this);
    Skeleton_Info2_anime_play(&this->skelAnime);
    goron_position_move(this);
    Actor_BGcheck2(play, &this->actor, this->collider.dim.height * 0.5f, this->collider.dim.radius * 0.6f,
                            0.0f, UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2);
#if OOT_VERSION < PAL_1_0
    hitcheck(this, play);
#else
    if (this->interactInfo.talkState == NPC_TALK_STATE_IDLE) {
        hitcheck(this, play);
    }
#endif
    this->actionFunc(this, play);
    if (this->unk_211 == true) {
        program_wait(play, this->fidgetTableY, this->fidgetTableZ, 18);
    }
    go_sub_action(this, play);
    eye_paci2(this);
    set_collision(this, play);
}

static s32 go_iwa1_disp(EnGo2* this, PlayState* play) {
    Vec3f D_80A48554 = { 0.0f, 0.0f, 0.0f };

    OPEN_DISPS(play->state.gfxCtx, "../z_en_go2.c", 2881);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_go2.c", 2884);
    gSPDisplayList(POLY_OPA_DISP++, gGoronDL_00BD80);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_go2.c", 2889);
    Matrix_Position(&D_80A48554, &this->actor.focus.pos);

    return 1;
}

static s32 go_iwa2_disp(EnGo2* this, PlayState* play) {
    s32 pad;
    Vec3f D_80A48560 = { 0.0f, 0.0f, 0.0f };
    f32 speedXZ;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_go2.c", 2914);
    _texture_z_light_fog_prim(play->state.gfxCtx);
    speedXZ = this->actionFunc == go_brake ? 0.0f : this->actor.speed;
    Matrix_rotateXYZ((play->state.frames * ((s16)speedXZ * 1400)), 0, this->actor.shape.rot.z, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_go2.c", 2926);
    gSPDisplayList(POLY_OPA_DISP++, gGoronDL_00C140);
    CLOSE_DISPS(play->state.gfxCtx, "../z_en_go2.c", 2930);
    Matrix_Position(&D_80A48560, &this->actor.focus.pos);
    return 1;
}

static s32 before_display(PlayState* play, s32 limb, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnGo2* this = (EnGo2*)thisx;
    Vec3s limbRot;

    if (limb == 17) {
        Matrix_translate(2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        limbRot = this->interactInfo.headRot;
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateZ(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
        Matrix_translate(-2800.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }
    if (limb == 10) {
        limbRot = this->interactInfo.torsoRot;
        Matrix_rotateY(BINANG_TO_RAD_ALT(limbRot.y), MTXMODE_APPLY);
        Matrix_rotateX(BINANG_TO_RAD_ALT(limbRot.x), MTXMODE_APPLY);
    }
    if ((limb == 10) || (limb == 11) || (limb == 14)) {
        rot->y += sin_s(this->fidgetTableY[limb]) * FIDGET_AMPLITUDE;
        rot->z += cos_s(this->fidgetTableZ[limb]) * FIDGET_AMPLITUDE;
    }
    return 0;
}

static void after_display(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnGo2* this = (EnGo2*)thisx;
    Vec3f D_80A4856C = { 600.0f, 0.0f, 0.0f };

    if (limbIndex == 17) {
        Matrix_Position(&D_80A4856C, &this->actor.focus.pos);
    }
}

void En_Go2_actor_draw(Actor* thisx, PlayState* play) {
    EnGo2* this = (EnGo2*)thisx;
    void* eyeTextures[] = { gGoronCsEyeClosed2Tex, gGoronCsEyeOpenTex, gGoronCsEyeHalfTex, gGoronCsEyeClosedTex };
    void* mouthTextures[] = { gGoronCsMouthNeutralTex, gGoronCsMouthSmileTex };

    go_eff_dust_mv(this);
    Matrix_push();
    go_eff_dust_dr(this, play);
    Matrix_pull();

    if ((this->actionFunc == go_iwa_wait) && (this->skelAnime.playSpeed == 0.0f) &&
        (this->skelAnime.curFrame == 0.0f)) {
        if (1) {}
        go_iwa1_disp(this, play);
    } else if (this->actionFunc == go_run || this->actionFunc == go_brake ||
               this->actionFunc == go_run_wait) {
        go_iwa2_disp(this, play);
    } else {
        OPEN_DISPS(play->state.gfxCtx, "../z_en_go2.c", 3063);
        _texture_z_light_fog_prim(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeTexIndex]));
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(mouthTextures[this->mouthTexIndex]));

        Si2_draw_SV(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              before_display, after_display, this);
        CLOSE_DISPS(play->state.gfxCtx, "../z_en_go2.c", 3081);
    }
}
