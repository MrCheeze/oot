#include "z_en_box.h"
#include "global.h"
#include "overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo.h"
#include "assets/objects/object_box/object_box.h"

#define FLAGS 0

// movement flags

/*
set on init unless treasure flag is set
if clear, chest moves (Actor_position_moveF) (falls, likely)
ends up cleared from SWITCH_FLAG_FALL types when switch flag is set
*/
#define ENBOX_MOVE_IMMOBILE (1 << 0)
/*
set in the logic for SWITCH_FLAG_FALL types
otherwise unused
*/
#define ENBOX_MOVE_UNUSED (1 << 1)
/*
set with 50% chance on init for SWITCH_FLAG_FALL types
only used for SWITCH_FLAG_FALL types
ends up "blinking" (set/clear every frame) once switch flag is set,
if some collision-related condition (?) is met
only used for signum of z rotation
*/
#define ENBOX_MOVE_FALL_ANGLE_SIDE (1 << 2)
/*
when set, gets cleared next En_Box_actor_move call and clip to the floor
*/
#define ENBOX_MOVE_STICK_TO_GROUND (1 << 4)

typedef enum EnBoxStateUnk1FB {
    ENBOX_STATE_0, // waiting for player near / player available / player ? (IDLE)
    ENBOX_STATE_1, // used only temporarily, maybe "player is ready" ?
    ENBOX_STATE_2  // waiting for something message context-related
} EnBoxStateUnk1FB;

void En_Box_actor_ct(Actor* thisx, PlayState* play2);
void En_Box_actor_dt(Actor* thisx, PlayState* play);
void En_Box_actor_move(Actor* thisx, PlayState* play);
void En_Box_actor_draw(Actor* thisx, PlayState* play);

void move_drop_wait(EnBox* this, PlayState* play);
void move_ocarina_wait(EnBox* this, PlayState* play);
void move_wakidashi_sw_wait(EnBox* this, PlayState* play);
static void move_room_clear_wait(EnBox* this, PlayState* play);
static void move_waitcamera(EnBox* this, PlayState* play);
static void move_appeare(EnBox* this, PlayState* play);
static void move_wait(EnBox* this, PlayState* play);
static void move_demo(EnBox* this, PlayState* play);

ActorProfile En_Box_Profile = {
    /**/ ACTOR_EN_BOX,
    /**/ ACTORCAT_CHEST,
    /**/ FLAGS,
    /**/ OBJECT_BOX,
    /**/ sizeof(EnBox),
    /**/ En_Box_actor_ct,
    /**/ En_Box_actor_dt,
    /**/ En_Box_actor_move,
    /**/ En_Box_actor_draw,
};

static AnimationHeader* anim_tbl[4] = { &gTreasureChestAnim_00024C, &gTreasureChestAnim_000128,
                                           &gTreasureChestAnim_00043C, &gTreasureChestAnim_00043C };

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_0, ICHAIN_STOP),
};

static UNK_TYPE SetType;

void En_Box_actor_set_process(EnBox* this, EnBoxActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void set_ground(EnBox* this, PlayState* play) {
    f32 newY;
    CollisionPoly* poly;
    s32 bgId;
    Vec3f checkPos;

    checkPos = this->dyna.actor.world.pos;
    checkPos.y += 1.0f;
    newY = T_BGCheck_ObjGroundCheck_aiac(&play->colCtx, &poly, &bgId, &this->dyna.actor, &checkPos);
    if (newY != BGCHECK_Y_MIN) {
        this->dyna.actor.world.pos.y = newY;
    }
}

void En_Box_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBox* this = (EnBox*)thisx;
    AnimationHeader* anim;
    CollisionHeader* colHeader;
    f32 animFrameStart;
    f32 endFrame;

    animFrameStart = 0.0f;
    anim = anim_tbl[((void)0, z_common_data.save.linkAge)];
    colHeader = NULL;
    endFrame = Si2_anime_end_frame(anim);
    ValueSet_process(&this->dyna.actor, value_init);

    MoveBG_ct(&this->dyna, 0);
    DynaPolyUty_bgdi_SG2KSG(&gTreasureChestCol, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    DynaPolygonInfo_setRoofOff(play, &play->colCtx.dyna, this->dyna.bgId);

    this->movementFlags = 0;
    this->type = PARAMS_GET_U(thisx->params, 12, 4);
    this->iceSmokeTimer = 0;
    this->unk_1FB = ENBOX_STATE_0;
    this->dyna.actor.gravity = -5.5f;
    this->switchFlag = this->dyna.actor.world.rot.z;
    this->dyna.actor.minVelocityY = -50.0f;

    if (play) {} // helps the compiler store play2 into s1

    if (Actor_Environment_Tbox_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 5))) {
        this->alpha = 255;
        this->iceSmokeTimer = 100;
        En_Box_actor_set_process(this, move_demo);
        this->movementFlags |= ENBOX_MOVE_STICK_TO_GROUND;
        animFrameStart = endFrame;
    } else if ((this->type == ENBOX_TYPE_SWITCH_FLAG_FALL_BIG || this->type == ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL) &&
               !Actor_Environment_sw_Check(play, this->switchFlag)) {
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        if (fqrand() < 0.5f) {
            this->movementFlags |= ENBOX_MOVE_FALL_ANGLE_SIDE;
        }
        this->unk_1A8 = -12;
        En_Box_actor_set_process(this, move_drop_wait);
        this->alpha = 0;
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    } else if ((this->type == ENBOX_TYPE_ROOM_CLEAR_BIG || this->type == ENBOX_TYPE_ROOM_CLEAR_SMALL) &&
               !Actor_Environment_room_clear_Check(play, this->dyna.actor.room)) {
        En_Box_actor_set_process(this, move_room_clear_wait);
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 50.0f;
        this->alpha = 0;
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    } else if (this->type == ENBOX_TYPE_9 || this->type == ENBOX_TYPE_10) {
        En_Box_actor_set_process(this, move_ocarina_wait);
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 50.0f;
        this->alpha = 0;
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    } else if (this->type == ENBOX_TYPE_SWITCH_FLAG_BIG && !Actor_Environment_sw_Check(play, this->switchFlag)) {
        En_Box_actor_set_process(this, move_wakidashi_sw_wait);
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - 50.0f;
        this->alpha = 0;
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    } else {
        if (this->type == ENBOX_TYPE_4 || this->type == ENBOX_TYPE_6) {
            this->dyna.actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
        }
        En_Box_actor_set_process(this, move_wait);
        this->movementFlags |= ENBOX_MOVE_IMMOBILE;
        this->movementFlags |= ENBOX_MOVE_STICK_TO_GROUND;
    }

    this->dyna.actor.world.rot.y += 0x8000;
    this->dyna.actor.home.rot.z = this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z = 0;

    Skeleton_Info2_M_ct(play, &this->skelanime, &gTreasureChestSkel, anim, this->jointTable, this->morphTable, 5);
    Skeleton_Info2_init(&this->skelanime, anim, 1.5f, animFrameStart, endFrame, ANIMMODE_ONCE, 0.0f);

    switch (this->type) {
        case ENBOX_TYPE_SMALL:
        case ENBOX_TYPE_6:
        case ENBOX_TYPE_ROOM_CLEAR_SMALL:
        case ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL:
            Actor_set_scale(&this->dyna.actor, 0.005f);
            Actor_world_to_eye(&this->dyna.actor, 20.0f);
            break;
        default:
            Actor_set_scale(&this->dyna.actor, 0.01f);
            Actor_world_to_eye(&this->dyna.actor, 40.0f);
    }
}

void En_Box_actor_dt(Actor* thisx, PlayState* play) {
    EnBox* this = (EnBox*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

static void calc_dust_pos(EnBox* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel) {
    f32 randomRadius = fqrand() * 25.0f;
    s16 randomAngle = fqrand() * 0x10000;

    *pos = this->dyna.actor.world.pos;
    pos->x += sin_s(randomAngle) * randomRadius;
    pos->z += cos_s(randomAngle) * randomRadius;

    velocity->y = 1.0f;
    velocity->x = sin_s(randomAngle);
    velocity->z = cos_s(randomAngle);

    accel->x = 0.0f;
    accel->y = 0.0f;
    accel->z = 0.0f;
}

/**
 * Spawns dust randomly around the chest when the chest hits the ground after falling (FALL types)
 */
void disp_rebound_dust(EnBox* this, PlayState* play) {
    s32 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    for (i = 0; i < 20; i++) {
        calc_dust_pos(this, &pos, &velocity, &accel);
        Effect_SS_Dust_sc_co_ct(play, &pos, &velocity, &accel, 100, 30, 15);
    }
}

/**
 * Used while the chest is falling (FALL types)
 */
static void move_drop(EnBox* this, PlayState* play) {
    f32 yDiff;

    this->alpha = 255;
    this->movementFlags &= ~ENBOX_MOVE_IMMOBILE;
    if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->movementFlags |= ENBOX_MOVE_UNUSED;
        if (this->movementFlags & ENBOX_MOVE_FALL_ANGLE_SIDE) {
            this->movementFlags &= ~ENBOX_MOVE_FALL_ANGLE_SIDE;
        } else {
            this->movementFlags |= ENBOX_MOVE_FALL_ANGLE_SIDE;
        }
        if (this->type == ENBOX_TYPE_SWITCH_FLAG_FALL_BIG) {
            this->dyna.actor.velocity.y = -this->dyna.actor.velocity.y * 0.55f;
        } else {
            this->dyna.actor.velocity.y = -this->dyna.actor.velocity.y * 0.65f;
        }
        if (this->dyna.actor.velocity.y < 5.5f) {
            this->dyna.actor.shape.rot.z = 0;
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
            En_Box_actor_set_process(this, move_wait);
            deleteOnepointDemo(play, this->subCamId);
        }
        Nai_FxFlagEntry(NA_SE_EV_COFFIN_CAP_BOUND, &this->dyna.actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
        disp_rebound_dust(this, play);
    }
    yDiff = this->dyna.actor.world.pos.y - this->dyna.actor.floorHeight;
    if (this->movementFlags & ENBOX_MOVE_FALL_ANGLE_SIDE) {
        this->dyna.actor.shape.rot.z = yDiff * 50.0f;
    } else {
        this->dyna.actor.shape.rot.z = -yDiff * 50.0f;
    }
}

void move_drop_wait(EnBox* this, PlayState* play) {
    s32 treasureFlag = PARAMS_GET_U(this->dyna.actor.params, 0, 5);

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        Actor_dowsing_request(&this->dyna.actor, play);
    }

    if (this->unk_1A8 >= 0) {
        En_Box_actor_set_process(this, move_drop);
        this->subCamId = makeOnepointDemo(play, 4500, 9999, &this->dyna.actor, CAM_ID_MAIN);
        DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    } else if (this->unk_1A8 >= -11) {
        this->unk_1A8++;
    } else if (Actor_Environment_sw_Check(play, this->switchFlag)) {
        this->unk_1A8++;
    }
}

// used for types 9, 10
void move_ocarina_wait(EnBox* this, PlayState* play) {
    s32 treasureFlag = PARAMS_GET_U(this->dyna.actor.params, 0, 5);
    Player* player = GET_PLAYER(play);

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        Actor_dowsing_request(&this->dyna.actor, play);
    }

    if (Math3DLengthSquare(&this->dyna.actor.world.pos, &player->actor.world.pos) > SQ(150.0f)) {
        this->unk_1FB = ENBOX_STATE_0;
    } else {
        if (this->unk_1FB == ENBOX_STATE_0) {
            if (!(player->stateFlags2 & PLAYER_STATE2_24)) {
                player->stateFlags2 |= PLAYER_STATE2_23;
                return;
            }
            this->unk_1FB = ENBOX_STATE_1;
        }

        if (this->unk_1FB == ENBOX_STATE_1) {
            ocarina_set(play, OCARINA_ACTION_FREE_PLAY);
            this->unk_1FB = ENBOX_STATE_2;
        } else if (this->unk_1FB == ENBOX_STATE_2 && play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
            if ((play->msgCtx.lastPlayedSong == OCARINA_SONG_LULLABY && this->type == ENBOX_TYPE_9) ||
                (play->msgCtx.lastPlayedSong == OCARINA_SONG_SUNS && this->type == ENBOX_TYPE_10)) {
                this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_DURING_OCARINA;
                En_Box_actor_set_process(this, move_waitcamera);
                makeActorAttentionDemo(play, &this->dyna.actor);
                this->unk_1A8 = 0;
                this->unk_1FB = ENBOX_STATE_0;
            } else {
                this->unk_1FB = ENBOX_STATE_0;
            }
        }
    }
}

void move_wakidashi_sw_wait(EnBox* this, PlayState* play) {
    s32 treasureFlag = PARAMS_GET_U(this->dyna.actor.params, 0, 5);

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        Actor_dowsing_request(&this->dyna.actor, play);
    }

    if (Actor_Environment_sw_Check(play, this->switchFlag)) {
        makeActorAttentionDemo(play, &this->dyna.actor);
        En_Box_actor_set_process(this, move_waitcamera);
        this->unk_1A8 = -30;
    }
}

static void move_room_clear_wait(EnBox* this, PlayState* play) {
    s32 treasureFlag = PARAMS_GET_U(this->dyna.actor.params, 0, 5);

    if (treasureFlag >= ENBOX_TREASURE_FLAG_UNK_MIN && treasureFlag < ENBOX_TREASURE_FLAG_UNK_MAX) {
        Actor_dowsing_request(&this->dyna.actor, play);
    }

    if (Actor_Environment_no_enemy_Check(play, this->dyna.actor.room) && !player_demo_check(play)) {
        Actor_Environment_room_clear_On(play, this->dyna.actor.room);
        En_Box_actor_set_process(this, move_waitcamera);
        makeActorAttentionDemo(play, &this->dyna.actor);
        if (checkPartrActorAttentionDemo(play, this->dyna.actor.category)) {
            this->unk_1A8 = 0;
        } else {
            this->unk_1A8 = -30;
        }
    }
}

/**
 * The chest is ready to appear, possibly waiting for camera/cutscene-related stuff to happen
 */
static void move_waitcamera(EnBox* this, PlayState* play) {
    if (getAttentionDemoPart() == this->dyna.actor.category || this->unk_1A8 != 0) {
        En_Box_actor_set_process(this, move_appeare);
        this->unk_1A8 = 0;
        Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_KANKYO, this->dyna.actor.home.pos.x, this->dyna.actor.home.pos.y,
                    this->dyna.actor.home.pos.z, 0, 0, 0, DEMOKANKYO_SPARKLES);
        Nai_FxFlagEntry(NA_SE_EV_TRE_BOX_APPEAR, &this->dyna.actor.projectedPos, 4, &_dummy_one,
                             &_dummy_one, &_dummy_zero_s8);
    }
}

static void move_appeare(EnBox* this, PlayState* play) {
    DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);

    if (this->unk_1A8 < 0) {
        this->unk_1A8++;
    } else if (this->unk_1A8 < 40) {
        this->unk_1A8++;
        this->dyna.actor.world.pos.y += 1.25f;
    } else if (this->unk_1A8 < 60) {
        this->alpha += 12;
        this->unk_1A8++;
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y;
    } else {
        En_Box_actor_set_process(this, move_wait);
    }
}

/**
 * Chest is ready to be open
 */
static void move_wait(EnBox* this, PlayState* play) {
    f32 frameCount;
    AnimationHeader* anim;
    s32 linkAge;
    s32 pad;
    Vec3f sp4C;
    Player* player;

    this->alpha = 255;
    this->movementFlags |= ENBOX_MOVE_IMMOBILE;
    if (this->unk_1F4 != 0) { // unk_1F4 is modified by player code
        linkAge = z_common_data.save.linkAge;
        anim = anim_tbl[(this->unk_1F4 < 0 ? 2 : 0) + linkAge];
        frameCount = Si2_anime_end_frame(anim);
        Skeleton_Info2_init(&this->skelanime, anim, 1.5f, 0, frameCount, ANIMMODE_ONCE, 0.0f);
        En_Box_actor_set_process(this, move_demo);
        if (this->unk_1F4 > 0) {
            switch (this->type) {
                case ENBOX_TYPE_SMALL:
                case ENBOX_TYPE_6:
                case ENBOX_TYPE_ROOM_CLEAR_SMALL:
                case ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL:
                    break;
                default:
                    Actor_info_make_child_actor(&play->actorCtx, &this->dyna.actor, play, ACTOR_DEMO_TRE_LGT,
                                       this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                       this->dyna.actor.world.pos.z, this->dyna.actor.shape.rot.x,
                                       this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z, 0xFFFF);
                    Na_StartFanfare(NA_BGM_OPEN_TRE_BOX | 0x900);
            }
        }
        PRINTF("Actor_Environment_Tbox_On() %d\n", PARAMS_GET_U(this->dyna.actor.params, 0, 5));
        Actor_Environment_Tbox_On(play, PARAMS_GET_U(this->dyna.actor.params, 0, 5));
    } else {
        player = GET_PLAYER(play);
        Actor_search_position_project_distanceXZ(&this->dyna.actor, &sp4C, &player->actor.world.pos);
        if (sp4C.z > -50.0f && sp4C.z < 0.0f && fabsf(sp4C.y) < 10.0f && fabsf(sp4C.x) < 20.0f &&
            Actor_player_look_direction_check(&this->dyna.actor, 0x3000, play)) {
            Actor_carry_request_set(&this->dyna.actor, play, -PARAMS_GET_U(this->dyna.actor.params, 5, 7));
        }
        if (Actor_Environment_Tbox_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 5))) {
            En_Box_actor_set_process(this, move_demo);
        }
    }
}

/**
 * Plays an animation to its end, playing sound effects at key points
 */
static void move_demo(EnBox* this, PlayState* play) {
    u16 sfxId;

    this->dyna.actor.flags &= ~ACTOR_FLAG_REACT_TO_LENS;

    if (Skeleton_Info2_anime_play(&this->skelanime)) {
        if (this->unk_1F4 > 0) {
            if (this->unk_1F4 < 120) {
                this->unk_1F4++;
            } else {
                chase_f(&this->unk_1B0, 0.0f, 0.05f);
            }
        } else {
            if (this->unk_1F4 > -120) {
                this->unk_1F4--;
            } else {
                chase_f(&this->unk_1B0, 0.0f, 0.05f);
            }
        }
    } else {
        sfxId = 0;

        if (Skeleton_Info_frame_check(&this->skelanime, 30.0f)) {
            sfxId = NA_SE_EV_TBOX_UNLOCK;
        } else if (Skeleton_Info_frame_check(&this->skelanime, 90.0f)) {
            sfxId = NA_SE_EV_TBOX_OPEN;
        }

        if (sfxId != 0) {
            Nai_FxFlagEntry(sfxId, &this->dyna.actor.projectedPos, 4, &_dummy_one,
                                 &_dummy_one, &_dummy_zero_s8);
        }

        if (this->skelanime.jointTable[3].z > 0) {
            this->unk_1B0 = (0x7D00 - this->skelanime.jointTable[3].z) * 0.00006f;
            if (this->unk_1B0 < 0.0f) {
                this->unk_1B0 = 0.0f;
            } else if (this->unk_1B0 > 1.0f) {
                this->unk_1B0 = 1.0f;
            }
        }
    }
}

void En_Box_Actor_move_Ice_Trap_Effect(EnBox* this, PlayState* play) {
    Vec3f pos;
    Vec3f vel = { 0.0f, 1.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };
    f32 f0;

    this->iceSmokeTimer++;
    Actor_level_SE_set(&this->dyna.actor, NA_SE_EN_MIMICK_BREATH - SFX_FLAG);
    if (fqrand() < 0.3f) {
        f0 = 2.0f * fqrand() - 1.0f;
        pos = this->dyna.actor.world.pos;
        if (this->type == ENBOX_TYPE_SMALL || this->type == ENBOX_TYPE_6 || this->type == ENBOX_TYPE_ROOM_CLEAR_SMALL ||
            this->type == ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL) {
            pos.x += f0 * 10.0f * sin_s(this->dyna.actor.world.rot.y + 0x4000);
            pos.z += f0 * 10.0f * cos_s(this->dyna.actor.world.rot.y + 0x4000);
            f0 = 2.0f * fqrand() - 1.0f;
            vel.x = f0 * 0.8f * sin_s(this->dyna.actor.world.rot.y);
            vel.y = 1.8f;
            vel.z = f0 * 0.8f * cos_s(this->dyna.actor.world.rot.y);
        } else {
            pos.x += f0 * 20.0f * sin_s(this->dyna.actor.world.rot.y + 0x4000);
            pos.z += f0 * 20.0f * cos_s(this->dyna.actor.world.rot.y + 0x4000);
            f0 = 2.0f * fqrand() - 1.0f;
            vel.x = f0 * 1.6f * sin_s(this->dyna.actor.world.rot.y);
            vel.y = 1.8f;
            vel.z = f0 * 1.6f * cos_s(this->dyna.actor.world.rot.y);
        }
        Effect_SS_Ice_Smoke_make(play, &pos, &vel, &accel, 150);
    }
}

void En_Box_actor_move(Actor* thisx, PlayState* play) {
    EnBox* this = (EnBox*)thisx;

    if (this->movementFlags & ENBOX_MOVE_STICK_TO_GROUND) {
        this->movementFlags &= ~ENBOX_MOVE_STICK_TO_GROUND;
        set_ground(this, play);
    }

    this->actionFunc(this, play);

    if (!(this->movementFlags & ENBOX_MOVE_IMMOBILE)) {
        Actor_position_moveF(&this->dyna.actor);
        Actor_BGcheck2(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f,
                                UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 | UPDBGCHECKINFO_FLAG_4);
    }

    switch (this->type) {
        case ENBOX_TYPE_SMALL:
        case ENBOX_TYPE_6:
        case ENBOX_TYPE_ROOM_CLEAR_SMALL:
        case ENBOX_TYPE_SWITCH_FLAG_FALL_SMALL:
            Actor_world_to_eye(&this->dyna.actor, 20.0f);
            break;
        default:
            Actor_world_to_eye(&this->dyna.actor, 40.0f);
    }

    if (PARAMS_GET_U(this->dyna.actor.params, 5, 7) == GI_ICE_TRAP && this->actionFunc == move_demo &&
        this->skelanime.curFrame > 45 && this->iceSmokeTimer < 100) {
        En_Box_Actor_move_Ice_Trap_Effect(this, play);
    }
}

void after_draw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    EnBox* this = (EnBox*)thisx;
    s32 pad;

    if (limbIndex == 1) {
        MATRIX_FINALIZE_AND_LOAD((*gfx)++, play->state.gfxCtx, "../z_en_box.c", 1492);
        if (this->type != ENBOX_TYPE_DECORATED_BIG) {
            gSPDisplayList((*gfx)++, gTreasureChestChestFrontDL);
        } else {
            gSPDisplayList((*gfx)++, gTreasureChestBossKeyChestFrontDL);
        }
    } else if (limbIndex == 3) {
        MATRIX_FINALIZE_AND_LOAD((*gfx)++, play->state.gfxCtx, "../z_en_box.c", 1502);
        if (this->type != ENBOX_TYPE_DECORATED_BIG) {
            gSPDisplayList((*gfx)++, gTreasureChestChestSideAndLidDL);
        } else {
            gSPDisplayList((*gfx)++, gTreasureChestBossKeyChestSideAndTopDL);
        }
    }
}

Gfx* set_rendermode_nothing(GraphicsContext* gfxCtx) {
    Gfx* dListHead;
    Gfx* dList;

    dList = GRAPH_ALLOC(gfxCtx, sizeof(Gfx));
    ASSERT(dList != NULL, "gfxp != NULL", "../z_en_box.c", 1528);

    dListHead = dList;
    gSPEndDisplayList(dListHead++);

    return dList;
}

// set render mode with a focus on transparency
Gfx* set_rendermode_xlu(GraphicsContext* gfxCtx) {
    Gfx* dList;
    Gfx* dListHead;

    dListHead = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));
    ASSERT(dListHead != NULL, "gfxp != NULL", "../z_en_box.c", 1546);

    dList = dListHead;
    gDPSetRenderMode(dListHead++, G_RM_FOG_SHADE_A,
                     AA_EN | Z_CMP | Z_UPD | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_XLU | FORCE_BL |
                         GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA));
    gSPEndDisplayList(dListHead++);

    return dList;
}

Gfx* set_rendermode_opa_surf(GraphicsContext* gfxCtx) {
    Gfx* dList;
    Gfx* dListHead;

    dListHead = GRAPH_ALLOC(gfxCtx, 2 * sizeof(Gfx));
    ASSERT(dListHead != NULL, "gfxp != NULL", "../z_en_box.c", 1564);

    dList = dListHead;
    gDPSetRenderMode(dListHead++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2);
    gSPEndDisplayList(dListHead++);

    return dList;
}

void En_Box_actor_draw(Actor* thisx, PlayState* play) {
    EnBox* this = (EnBox*)thisx;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_box.c", 1581);

    /*
    this->dyna.actor.flags & ACTOR_FLAG_REACT_TO_LENS is set by Init (if type is 4 or 6)
    and cleared by Open
    */
    if ((this->alpha == 255 && !(this->type == ENBOX_TYPE_4 || this->type == ENBOX_TYPE_6)) ||
        (!CHECK_FLAG_ALL(this->dyna.actor.flags, ACTOR_FLAG_REACT_TO_LENS) &&
         (this->type == ENBOX_TYPE_4 || this->type == ENBOX_TYPE_6))) {
        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
        gSPSegment(POLY_OPA_DISP++, 0x08, set_rendermode_nothing(play->state.gfxCtx));
        _texture_z_light_fog_prim(play->state.gfxCtx);
        POLY_OPA_DISP = Si2_draw2(play, this->skelanime.skeleton, this->skelanime.jointTable, NULL,
                                       after_draw, this, POLY_OPA_DISP);
    } else if (this->alpha != 0) {
        gDPPipeSync(POLY_XLU_DISP++);
        _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
        if (this->type == ENBOX_TYPE_4 || this->type == ENBOX_TYPE_6) {
            gSPSegment(POLY_XLU_DISP++, 0x08, set_rendermode_opa_surf(play->state.gfxCtx));
        } else {
            gSPSegment(POLY_XLU_DISP++, 0x08, set_rendermode_xlu(play->state.gfxCtx));
        }
        POLY_XLU_DISP = Si2_draw2(play, this->skelanime.skeleton, this->skelanime.jointTable, NULL,
                                       after_draw, this, POLY_XLU_DISP);
    }

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_box.c", 1639);
}
