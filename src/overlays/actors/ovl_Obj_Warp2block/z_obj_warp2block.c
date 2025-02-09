/*
 * File: z_obj_warp2block.c
 * Overlay: ovl_Obj_Warp2Block
 * Description: Navi Infospot (Green, Time Block)
 */

#include "z_obj_warp2block.h"
#include "assets/objects/object_timeblock/object_timeblock.h"
#include "terminal.h"

#define FLAGS                                                                                               \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA | \
     ACTOR_FLAG_LOCK_ON_DISABLED)

void Obj_Warp2block_actor_ct(Actor* thisx, PlayState* play2);
void Obj_Warp2block_actor_dt(Actor* thisx, PlayState* play);
void Obj_Warp2block_actor_move(Actor* thisx, PlayState* play);
void Obj_Warp2block_actor_draw(Actor* thisx, PlayState* play);

void make_effect_warp2block(ObjWarp2block* this, PlayState* play);
s32 check_ocarina_position_warp2block(ObjWarp2block* this, PlayState* play);
void swap_warp2block(ObjWarp2block* this, PlayState* play);
static s32 ocarina_check_request(ObjWarp2block* this, PlayState* play);
static s32 ocarina_check_stop(ObjWarp2block* this, PlayState* play);
s32 ocarina_check_warp2block(ObjWarp2block* this, PlayState* play);
static void mv_clear_init(ObjWarp2block* this);
static void mv_clear(ObjWarp2block* this, PlayState* play);
void mv_actorSearch_init(ObjWarp2block* this);
void mv_actorSearch(ObjWarp2block* this, PlayState* play);
void mv_waitOcarina_init(ObjWarp2block* this);
void mv_waitOcarina(ObjWarp2block* this, PlayState* play);

ActorProfile Obj_Warp2block_Profile = {
    /**/ ACTOR_OBJ_WARP2BLOCK,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_TIMEBLOCK,
    /**/ sizeof(ObjWarp2block),
    /**/ Obj_Warp2block_actor_ct,
    /**/ Obj_Warp2block_actor_dt,
    /**/ Obj_Warp2block_actor_move,
    /**/ Obj_Warp2block_actor_draw,
};

typedef struct Warp2BlockSpawnData {
    /* 0x00 */ f32 scale;
    /* 0x04 */ f32 focus;
    /* 0x08 */ s16 params;
} Warp2BlockSpawnData; // size = 0x0C

static Warp2BlockSpawnData Tb_Size_Type_Data[] = {
    { 1.0f, 60.0f, 0x0018 },
    { 0.6f, 40.0f, 0x0019 },
};

static f32 Tb_Range_Type_Data[] = { 60.0f, 100.0f, 140.0f, 180.0f, 220.0f, 260.0f, 300.0f, 300.0f };

static InitChainEntry value_init[] = {
    ICHAIN_F32(cullingVolumeDistance, 1800, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1500, ICHAIN_STOP),
};

static Color_RGB8 c[] = {
    { 100, 120, 140 }, { 80, 140, 200 }, { 100, 150, 200 }, { 100, 200, 240 },
    { 80, 110, 140 },  { 70, 160, 225 }, { 80, 100, 130 },  { 100, 110, 190 },
};

void make_effect_warp2block(ObjWarp2block* this, PlayState* play) {
    Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_EFFECT, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                this->dyna.actor.world.pos.z, 0, 0, 0, Tb_Size_Type_Data[PARAMS_GET_U(this->dyna.actor.params, 8, 1)].params);

    Actor_info_make_actor(&play->actorCtx, play, ACTOR_DEMO_EFFECT, this->dyna.actor.child->world.pos.x,
                this->dyna.actor.child->world.pos.y, this->dyna.actor.child->world.pos.z, 0, 0, 0,
                Tb_Size_Type_Data[PARAMS_GET_U(this->dyna.actor.child->params, 8, 1)].params);
}

s32 check_ocarina_position_warp2block(ObjWarp2block* this, PlayState* play) {
    if (MoveBG_checkOverPlayerStatus(&this->dyna)) {
        return 0;
    } else {
        s32 pad;
        Actor* temp_a3 = this->dyna.actor.child;
        Player* player = GET_PLAYER(play);
        Vec3f sp20;
        f32 temp_f2;

        if ((this->dyna.actor.xzDistToPlayer <= Tb_Range_Type_Data[PARAMS_GET_U(this->dyna.actor.params, 11, 3)]) ||
            (temp_a3->xzDistToPlayer <= Tb_Range_Type_Data[PARAMS_GET_U(temp_a3->params, 11, 3)])) {

            Actor_search_position_project_distanceXZ(&this->dyna.actor, &sp20, &player->actor.world.pos);
            temp_f2 = (this->dyna.actor.scale.x * 50.0f) + 6.0f;

            if (!(temp_f2 < fabsf(sp20.x)) && !(temp_f2 < fabsf(sp20.z))) {
                return 0;
            }

            Actor_search_position_project_distanceXZ(temp_a3, &sp20, &player->actor.world.pos);
            temp_f2 = (temp_a3->scale.x * 50.0f) + 6.0f;

            if (!(temp_f2 < fabsf(sp20.x)) && !(temp_f2 < fabsf(sp20.z))) {
                return 0;
            }
        } else {
            return 0;
        }

        return 1;
    }
}

void swap_warp2block(ObjWarp2block* this, PlayState* play) {
    Vec3f tempVec;
    Vec3s tempRot;
    s32 temp;

    xyz_t_move(&tempVec, &this->dyna.actor.world.pos);
    xyz_t_move(&this->dyna.actor.world.pos, &this->dyna.actor.child->world.pos);
    xyz_t_move(&this->dyna.actor.child->world.pos, &tempVec);

    temp = this->dyna.actor.world.rot.y;
    this->dyna.actor.world.rot.y = this->dyna.actor.child->world.rot.y;
    this->dyna.actor.child->world.rot.y = temp;

    temp = this->dyna.actor.shape.rot.y;
    this->dyna.actor.shape.rot.y = this->dyna.actor.child->shape.rot.y;
    this->dyna.actor.child->shape.rot.y = temp;

    temp = this->dyna.actor.home.rot.z;
    this->dyna.actor.home.rot.z = this->dyna.actor.child->home.rot.z;
    this->dyna.actor.child->home.rot.z = temp;

    xyz_t_move(&tempVec, &this->dyna.actor.scale);
    xyz_t_move(&this->dyna.actor.scale, &this->dyna.actor.child->scale);
    xyz_t_move(&this->dyna.actor.child->scale, &tempVec);

    xyz_t_move(&tempVec, &this->dyna.actor.focus.pos);
    xyz_t_move(&this->dyna.actor.focus.pos, &this->dyna.actor.child->focus.pos);
    xyz_t_move(&this->dyna.actor.child->focus.pos, &tempVec);

    tempRot = this->dyna.actor.focus.rot;
    this->dyna.actor.focus.rot = this->dyna.actor.child->focus.rot;
    this->dyna.actor.child->focus.rot = tempRot;

    temp = PARAMS_GET_U(this->dyna.actor.params, 0, 15);
    this->dyna.actor.params =
        PARAMS_GET_NOSHIFT(this->dyna.actor.params, 15, 1) | PARAMS_GET_U(this->dyna.actor.child->params, 0, 15);
    this->dyna.actor.child->params = PARAMS_GET_NOSHIFT(this->dyna.actor.child->params, 15, 1) | (temp & 0x7FFF);

    if (Math3DLengthSquare(&this->dyna.actor.world.pos, &this->dyna.actor.home.pos) < 0.01f) {
        Actor_Environment_sw_Off(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
    } else {
        Actor_Environment_sw_On(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6));
    }
}

static s32 ocarina_check_request(ObjWarp2block* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (check_ocarina_position_warp2block(this, play)) {
        if (player->stateFlags2 & PLAYER_STATE2_24) {
            ocarina_set(play, OCARINA_ACTION_FREE_PLAY);
            this->func_168 = ocarina_check_stop;
        } else {
            player->stateFlags2 |= PLAYER_STATE2_23;
        }
    }

    return 0;
}

static s32 ocarina_check_stop(ObjWarp2block* this, PlayState* play) {
    if (play->msgCtx.ocarinaMode == OCARINA_MODE_04) {
        this->func_168 = ocarina_check_request;
    }

    if (play->msgCtx.lastPlayedSong == OCARINA_SONG_TIME) {
        if (this->unk_172 == 0xFE) {
            this->unk_16E = 0x6E;
        } else {
            this->unk_16E--;
            if (this->unk_16E == 0) {
                return 1;
            }
        }
    }
    return 0;
}

s32 ocarina_check_warp2block(ObjWarp2block* this, PlayState* play) {
    s32 ret = this->func_168(this, play);

    this->unk_172 = play->msgCtx.lastPlayedSong;

    return ret;
}

void Obj_Warp2block_actor_ct(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjWarp2block* this = (ObjWarp2block*)thisx;
    CollisionHeader* collisionHeader;

    collisionHeader = NULL;
    this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.z = 0;
    ValueSet_process(&this->dyna.actor, value_init);

    Actor_set_scale(&this->dyna.actor, Tb_Size_Type_Data[PARAMS_GET_U(this->dyna.actor.params, 8, 1)].scale);
    this->func_168 = ocarina_check_request;
    Actor_world_to_eye(&this->dyna.actor, Tb_Size_Type_Data[PARAMS_GET_U(this->dyna.actor.params, 8, 1)].focus);

    if (PARAMS_GET_U(this->dyna.actor.params, 15, 1)) {
        mv_actorSearch_init(this);
        if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
            this->dyna.actor.draw = NULL;
        }
        MoveBG_ct(&this->dyna, 0);
        DynaPolyUty_bgdi_SG2KSG(&gSongOfTimeBlockCol, &collisionHeader);
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, collisionHeader);
    } else {
        mv_clear_init(this);
    }

    PRINTF("時のブロック(ワープ２) (<arg> %04xH <type> color:%d range:%d)\n",
           PARAMS_GET_U(this->dyna.actor.params, 0, 16), this->dyna.actor.home.rot.z & 7,
           PARAMS_GET_U(this->dyna.actor.params, 11, 3));
}

void Obj_Warp2block_actor_dt(Actor* thisx, PlayState* play) {
    ObjWarp2block* this = (ObjWarp2block*)thisx;
    if (PARAMS_GET_U(this->dyna.actor.params, 15, 1)) {
        DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
    }
}

static void mv_clear_init(ObjWarp2block* this) {
    this->actionFunc = mv_clear;
    this->dyna.actor.draw = NULL;
}

static void mv_clear(ObjWarp2block* this, PlayState* play) {
}

void mv_actorSearch_init(ObjWarp2block* this) {
    this->actionFunc = mv_actorSearch;
}

void mv_actorSearch(ObjWarp2block* this, PlayState* play) {
    Actor* current = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].head;

    while (current != NULL) {
        if (current->id == ACTOR_OBJ_WARP2BLOCK && !PARAMS_GET_U(current->params, 15, 1) &&
            (PARAMS_GET_U(this->dyna.actor.params, 0, 6) == PARAMS_GET_U(current->params, 0, 6))) {
            this->dyna.actor.child = current;
            if (Actor_Environment_sw_Check(play, PARAMS_GET_U(this->dyna.actor.params, 0, 6))) {
                swap_warp2block(this, play);
                this->dyna.actor.draw = Obj_Warp2block_actor_draw;
            }
            mv_waitOcarina_init(this);
            return;
        }

        current = current->next;
    }

    this->unk_174++;
    if (this->unk_174 > 60) {
        PRINTF_COLOR_ERROR();
        PRINTF("Error : 時のブロック(ワープ２)が対でセットされていません(%s %d)\n", "../z_obj_warp2block.c", 505);
        PRINTF_RST();
        Actor_delete(&this->dyna.actor);
    }
}

void mv_waitOcarina_init(ObjWarp2block* this) {
    this->actionFunc = mv_waitOcarina;
}

void mv_waitOcarina(ObjWarp2block* this, PlayState* play) {
    if ((ocarina_check_warp2block(this, play) != 0) && (this->unk_16C <= 0)) {
        make_effect_warp2block(this, play);
        this->unk_16C = 0xA0;
        makeActorAttentionDemo(play, &this->dyna.actor);
        this->unk_170 = 0xC;
    }

    if (this->unk_170 > 0) {
        this->unk_170--;
        if (this->unk_170 == 0) {
            swap_warp2block(this, play);
        }
    }
    if (this->unk_16C == 0x32) {
        Na_StartSystemSe_F(NA_SE_SY_TRE_BOX_APPEAR);
    }
}

void Obj_Warp2block_actor_move(Actor* thisx, PlayState* play) {
    ObjWarp2block* this = (ObjWarp2block*)thisx;

    this->actionFunc(this, play);
    if (this->unk_16C > 0) {
        this->unk_16C--;
    }
}

void Obj_Warp2block_actor_draw(Actor* thisx, PlayState* play) {
    Color_RGB8* sp44;

    sp44 = &c[thisx->home.rot.z & 7];

    OPEN_DISPS(play->state.gfxCtx, "../z_obj_warp2block.c", 584);
    _texture_z_light_fog_prim(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_obj_warp2block.c", 588);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, sp44->r, sp44->g, sp44->b, 255);
    gSPDisplayList(POLY_OPA_DISP++, gSongOfTimeBlockDL);

    CLOSE_DISPS(play->state.gfxCtx, "../z_obj_warp2block.c", 594);
}
