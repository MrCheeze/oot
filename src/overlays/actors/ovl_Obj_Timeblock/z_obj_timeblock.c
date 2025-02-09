/*
 * File: z_obj_timeblock.c
 * Overlay: ovl_Obj_Timeblock
 * Description: Song of Time Block
 */

#include "z_obj_timeblock.h"
#include "assets/objects/object_timeblock/object_timeblock.h"

#define FLAGS                                                                                               \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA | \
     ACTOR_FLAG_LOCK_ON_DISABLED)

void Obj_Timeblock_actor_ct(Actor* thisx, PlayState* play);
void Obj_Timeblock_actor_dt(Actor* thisx, PlayState* play);
void Obj_Timeblock_actor_move(Actor* thisx, PlayState* play);
void Obj_Timeblock_actor_draw(Actor* thisx, PlayState* play);

void mv_wait_0_init(ObjTimeblock* this);
void mv_waitOcarina_1_init(ObjTimeblock* this);
void mv_waitSwitch_1_init(ObjTimeblock* this);

static s32 ocarina_check_request(ObjTimeblock* this, PlayState* play);
static s32 ocarina_check_stop(ObjTimeblock* this, PlayState* play);
static void mv_stop(ObjTimeblock* this, PlayState* play);
void mv_wait_0(ObjTimeblock* this, PlayState* play);
void mv_waitOcarina_1(ObjTimeblock* this, PlayState* play);
void mv_waitSwitch_1(ObjTimeblock* this, PlayState* play);

ActorProfile Obj_Timeblock_Profile = {
    /**/ ACTOR_OBJ_TIMEBLOCK,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_TIMEBLOCK,
    /**/ sizeof(ObjTimeblock),
    /**/ Obj_Timeblock_actor_ct,
    /**/ Obj_Timeblock_actor_dt,
    /**/ Obj_Timeblock_actor_move,
    /**/ Obj_Timeblock_actor_draw,
};

typedef struct ObjTimeblockSizeOptions {
    /* 0x00 */ f32 scale;
    /* 0x04 */ f32 height;
    /* 0x08 */ s16 demoEffectParams;
} ObjTimeblockSizeOptions; // size = 0x0C

static ObjTimeblockSizeOptions Tb_Size_Type_Data[] = {
    { 1.0, 60.0, 0x0018 },
    { 0.60, 40.0, 0x0019 },
};

static f32 Tb_Range_Type_Data[] = { 60.0, 100.0, 140.0, 180.0, 220.0, 260.0, 300.0, 300.0 };

static InitChainEntry value_init[] = {
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_2, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1500, ICHAIN_STOP),
};

static Color_RGB8 c[] = {
    { 100, 120, 140 }, { 80, 140, 200 }, { 100, 150, 200 }, { 100, 200, 240 },
    { 80, 110, 140 },  { 70, 160, 225 }, { 80, 100, 130 },  { 100, 110, 190 },
};

u32 check_appear_on(ObjTimeblock* this) {
    if (!PARAMS_GET_U(this->dyna.actor.params, 10, 1)) {
        if (this->unk_177 == 0) {
            return this->unk_175;
        } else {
            u8 temp = PARAMS_GET_U(this->dyna.actor.params, 15, 1) ? true : false;

            if (this->unk_177 == 1) {
                return this->unk_174 ^ temp;
            } else {
                u8 linkIsChild = (LINK_AGE_IN_YEARS == YEARS_CHILD) ? true : false;

                return this->unk_174 ^ temp ^ linkIsChild;
            }
        }
    } else {
        return (PARAMS_GET_U(this->dyna.actor.params, 15, 1) ? true : false) ^ this->unk_174;
    }
}

void make_effect_timeblock(ObjTimeblock* this, PlayState* play) {
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_EFFECT, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                this->dyna.actor.world.pos.z, 0, 0, 0,
                Tb_Size_Type_Data[PARAMS_GET_U(this->dyna.actor.params, 8, 1)].demoEffectParams);
}

void change_env_sw_bit(PlayState* play, s32 flag) {
    if (Actor_Environment_sw_Check(play, flag)) {
        Actor_Environment_sw_Off(play, flag);
    } else {
        Actor_Environment_sw_On(play, flag);
    }
}

void Obj_Timeblock_actor_ct(Actor* thisx, PlayState* play) {
    ObjTimeblock* this = (ObjTimeblock*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, 0);
    this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z = 0;

    DynaPolyUty_bgdi_SG2KSG(&gSongOfTimeBlockCol, &colHeader);

    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

    ValueSet_process(&this->dyna.actor, value_init);
    Actor_set_scale(&this->dyna.actor, Tb_Size_Type_Data[PARAMS_GET_U(this->dyna.actor.params, 8, 1)].scale);

    if (PARAMS_GET_U(this->dyna.actor.params, 6, 1)) {
        this->unk_177 = 0;
    } else {
        this->unk_177 = (PARAMS_GET_U(this->dyna.actor.params, 0, 6) < 0x38) ? 2 : 1;
    }

    this->songObserverFunc = ocarina_check_request;

    Actor_world_to_eye(&this->dyna.actor, Tb_Size_Type_Data[PARAMS_GET_U(this->dyna.actor.params, 8, 1)].height);

    this->unk_174 = (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) ? true : false;
    this->unk_175 = PARAMS_GET_U(this->dyna.actor.params, 15, 1) ? true : false;
    this->isVisible = check_appear_on(this);

    if (!PARAMS_GET_U(this->dyna.actor.params, 10, 1)) {
        mv_wait_0_init(this);
    } else if (this->isVisible) {
        mv_waitOcarina_1_init(this);
    } else {
        mv_waitSwitch_1_init(this);
    }

    // "Block of time"
    PRINTF("時のブロック (<arg> %04xH <type> save:%d color:%d range:%d move:%d)\n", (u16)this->dyna.actor.params,
           this->unk_177, this->dyna.actor.home.rot.z & 7, PARAMS_GET_U(this->dyna.actor.params, 11, 3),
           PARAMS_GET_U(this->dyna.actor.params, 10, 1));
}

void Obj_Timeblock_actor_dt(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjTimeblock* this = (ObjTimeblock*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
}

u8 check_ocarina_position_timeblock(ObjTimeblock* this, PlayState* play) {
    if (this->isVisible && MoveBG_checkOverPlayerStatus(&this->dyna)) {
        return false;
    }

    if (this->dyna.actor.xzDistToPlayer <= Tb_Range_Type_Data[PARAMS_GET_U(this->dyna.actor.params, 11, 3)]) {
        Vec3f playerRelativePos;
        f32 blockSize;

        Actor_search_position_project_distanceXZ(&this->dyna.actor, &playerRelativePos, &GET_PLAYER(play)->actor.world.pos);
        blockSize = this->dyna.actor.scale.x * 50.0f + 6.0f;
        // Return true if player's xz position is not inside the block
        if (blockSize < fabsf(playerRelativePos.x) || blockSize < fabsf(playerRelativePos.z)) {
            return true;
        }
    }

    return false;
}

static s32 ocarina_check_request(ObjTimeblock* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (check_ocarina_position_timeblock(this, play)) {
        if (player->stateFlags2 & PLAYER_STATE2_24) {
            ocarina_set(play, OCARINA_ACTION_FREE_PLAY);
            this->songObserverFunc = ocarina_check_stop;
        } else {
            player->stateFlags2 |= PLAYER_STATE2_23;
        }
    }
    return false;
}

static s32 ocarina_check_stop(ObjTimeblock* this, PlayState* play) {
    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        this->songObserverFunc = ocarina_check_request;
    }
    if (play->msgCtx.lastPlayedSong == OCARINA_SONG_TIME) {
        if (this->unk_172 == 254) {
            this->songEndTimer = 110;
        } else {
            this->songEndTimer--;
            if (this->songEndTimer == 0) {
                return true;
            }
        }
    }
    return false;
}

static void mv_stop_init(ObjTimeblock* this) {
    this->actionFunc = mv_stop;
}

static void mv_stop(ObjTimeblock* this, PlayState* play) {
}

void mv_wait_0_init(ObjTimeblock* this) {
    this->actionFunc = mv_wait_0;
}

void mv_wait_0(ObjTimeblock* this, PlayState* play) {
    u32 newIsVisible;

    if (this->songObserverFunc(this, play) && this->demoEffectTimer <= 0) {
        make_effect_timeblock(this, play);
        this->demoEffectTimer = 160;

        // Possibly points the camera to this actor
        makeActorAttentionDemo(play, &this->dyna.actor);
        // "◯◯◯◯ Time Block Attention Camera (frame counter  %d)\n"
        PRINTF("◯◯◯◯ Time Block 注目カメラ (frame counter  %d)\n", play->state.frames);

        this->demoEffectFirstPartTimer = 12;

        if (this->unk_177 == 0) {
            this->dyna.actor.params ^= 0x8000;
        } else {
            change_env_sw_bit(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
        }
    }

    this->unk_172 = play->msgCtx.lastPlayedSong;
    if (this->demoEffectFirstPartTimer > 0) {
        this->demoEffectFirstPartTimer--;
        if (this->demoEffectFirstPartTimer == 0) {
            if (this->unk_177 == 0) {
                this->unk_175 = PARAMS_GET_U(this->dyna.actor.params, 15, 1) ? true : false;
            } else {
                this->unk_174 = (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) ? true : false;
            }
        }
    }

    newIsVisible = check_appear_on(this);
    if (this->unk_177 == 1 && newIsVisible != this->isVisible) {
        mv_stop_init(this);
    }
    this->isVisible = newIsVisible;

    if (this->demoEffectTimer == 50) {
        Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
    }
}

void wait_1_after_proc(ObjTimeblock* this, PlayState* play) {
    s32 switchFlag = PARAMS_GET_U(this->dyna.actor.params, 0, 6);

    this->unk_172 = play->msgCtx.lastPlayedSong;

    if (this->demoEffectFirstPartTimer > 0 && --this->demoEffectFirstPartTimer == 0) {
        this->unk_174 = (Actor_Environment_sw_Check(play, switchFlag)) ? true : false;
    }

    this->isVisible = check_appear_on(this);
    this->unk_176 = (Actor_Environment_sw_Check(play, switchFlag)) ? true : false;
}

void mv_waitOcarina_1_init(ObjTimeblock* this) {
    this->actionFunc = mv_waitOcarina_1;
}

void mv_waitOcarina_1(ObjTimeblock* this, PlayState* play) {
    if (this->songObserverFunc(this, play) && this->demoEffectTimer <= 0) {
        this->demoEffectFirstPartTimer = 12;
        make_effect_timeblock(this, play);
        this->demoEffectTimer = 160;
        makeActorAttentionDemo(play, &this->dyna.actor);
        // "Time Block Attention Camera (frame counter)"
        PRINTF("◯◯◯◯ Time Block 注目カメラ (frame counter  %d)\n", play->state.frames);
        change_env_sw_bit(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
    }

    wait_1_after_proc(this, play);

    if (this->demoEffectTimer == 50) {
        Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
    }

    if (!this->isVisible && this->demoEffectTimer <= 0) {
        mv_waitSwitch_1_init(this);
    }
}

void mv_waitSwitch_1_init(ObjTimeblock* this) {
    this->actionFunc = mv_waitSwitch_1;
}

void mv_waitSwitch_1(ObjTimeblock* this, PlayState* play) {
    s32 switchFlag = PARAMS_GET_U(this->dyna.actor.params, 0, 6);
    s8 switchFlagIsSet = (Actor_Environment_sw_Check(play, switchFlag)) ? true : false;

    if (this->unk_176 ^ switchFlagIsSet &&
        switchFlagIsSet ^ (PARAMS_GET_U(this->dyna.actor.params, 15, 1) ? true : false)) {
        if (this->demoEffectTimer <= 0) {
            make_effect_timeblock(this, play);
            this->demoEffectTimer = 160;
        }
        this->demoEffectFirstPartTimer = 12;
    }

    wait_1_after_proc(this, play);

    if (this->isVisible && this->demoEffectTimer <= 0) {
        mv_waitOcarina_1_init(this);
    }
}

void Obj_Timeblock_actor_move(Actor* thisx, PlayState* play) {
    ObjTimeblock* this = (ObjTimeblock*)thisx;

    this->actionFunc(this, play);

    if (this->demoEffectTimer > 0) {
        this->demoEffectTimer--;
    }

    if (this->isVisible) {
        DynaPolygonInfo_clearThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    } else {
        DynaPolygonInfo_setThrough(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

void Obj_Timeblock_actor_draw(Actor* thisx, PlayState* play) {
    if (((ObjTimeblock*)thisx)->isVisible) {
        Color_RGB8* primColor = &c[thisx->home.rot.z & 7];

        OPEN_DISPS(play->state.gfxCtx, "../z_obj_timeblock.c", 762);

        _texture_z_light_fog_prim(play->state.gfxCtx);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_obj_timeblock.c", 766);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, primColor->r, primColor->g, primColor->b, 255);
        gSPDisplayList(POLY_OPA_DISP++, gSongOfTimeBlockDL);

        CLOSE_DISPS(play->state.gfxCtx, "../z_obj_timeblock.c", 772);
    }
}
