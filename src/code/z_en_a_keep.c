#include "global.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/object_d_hsblock/object_d_hsblock.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void En_a_obj_Actor_ct(Actor* thisx, PlayState* play);
void En_a_keep_Actor_dt(Actor* thisx, PlayState* play);
void En_a_keep_move(Actor* thisx, PlayState* play);
void En_a_keep_display(Actor* thisx, PlayState* play);

static void mode_talk(EnAObj* this, PlayState* play);
void En_a_keep_Actor_mode_non_move(EnAObj* this, PlayState* play);
void En_a_keep_Actor_mode_drop_del(EnAObj* this, PlayState* play);
void En_a_keep_Actor_mode_drop2_move(EnAObj* this, PlayState* play);
void En_a_keep_Actor_mode_pu_box(EnAObj* this, PlayState* play);

void En_a_keep_Actor_mode_non_move_init(EnAObj* this, s16 type);
void En_a_keep_Actor_mode_drop_del_init(EnAObj* this, s16 type);
void En_a_keep_Actor_mode_drop2_move_init(EnAObj* this, s16 type);
void En_a_keep_Actor_mode_pu_box_init(EnAObj* this, s16 type);

ActorProfile En_A_Obj_Profile = {
    /**/ ACTOR_EN_A_OBJ,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_KEEP,
    /**/ sizeof(EnAObj),
    /**/ En_a_obj_Actor_ct,
    /**/ En_a_keep_Actor_dt,
    /**/ En_a_keep_move,
    /**/ En_a_keep_display,
};

static ColliderCylinderInit OcInfoData = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ALL,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        ATELEM_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 25, 60, 0, { 0, 0, 0 } },
};

//! @bug gHookshotPostCol and gHookshotPostDL are referenced below for type A_OBJ_UNKNOWN_6 but they aren't available
//! since object_d_hsblock isn't a dependency of this actor.
//! This doesn't cause issues in the base game because A_OBJ_UNKNOWN_6 is never used.

static CollisionHeader* bgd_info[] = {
    &gLargerCubeCol,       // A_OBJ_GRASS_CLUMP, A_OBJ_TREE_STUMP
    &gLargerCubeCol,       // A_OBJ_BLOCK_LARGE, A_OBJ_BLOCK_HUGE
    &gSmallerFlatBlockCol, // unused
    &gLargerFlatBlockCol,  // A_OBJ_BLOCK_SMALL_ROT, A_OBJ_BLOCK_LARGE_ROT
    &gSmallerCubeCol,      // unused
    &gHookshotPostCol,     // A_OBJ_UNKNOWN_6
};

static Gfx* a_obj_tbl[] = {
    gFlatBlockDL,    gFlatBlockDL,   gFlatBlockDL, gFlatRotBlockDL,    gFlatRotBlockDL,    gSmallCubeDL,
    gHookshotPostDL, gGrassBladesDL, gTreeStumpDL, gSignRectangularDL, gSignDirectionalDL, gBoulderFragmentsDL,
};

void En_A_Keep_actor_set_process(EnAObj* this, EnAObjActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void En_a_obj_Actor_ct(Actor* thisx, PlayState* play) {
    CollisionHeader* colHeader = NULL;
    s32 pad;
    EnAObj* this = (EnAObj*)thisx;
    f32 shadowScale = 6.0f;

    this->textId = PARAMS_GET_U(thisx->params, 8, 8);
    thisx->params &= 0xFF;

    switch (thisx->params) {
        case A_OBJ_BLOCK_SMALL:
            Actor_set_scale(thisx, 0.025f);
            break;
        case A_OBJ_BLOCK_LARGE:
            Actor_set_scale(thisx, 0.05f);
            break;
        case A_OBJ_BLOCK_HUGE:
        case A_OBJ_CUBE_SMALL:
        case A_OBJ_UNKNOWN_6:
            Actor_set_scale(thisx, 0.1f);
            break;
        case A_OBJ_BLOCK_SMALL_ROT:
            Actor_set_scale(thisx, 0.005f);
            break;
        case A_OBJ_BLOCK_LARGE_ROT:
        default:
            Actor_set_scale(thisx, 0.01f);
            break;
    }

    if (thisx->params >= A_OBJ_SIGNPOST_OBLONG) {
        shadowScale = 12.0f;
    }

    Shape_Info_init(&thisx->shape, 0.0f, Actor_shadow_circle, shadowScale);

    thisx->focus.pos = thisx->world.pos;
    this->dyna.bgId = BGACTOR_NEG_ONE;
    this->dyna.interactFlags = 0;
    this->dyna.transformFlags = 0;
    thisx->cullingVolumeDownward = 1200.0f;
    thisx->cullingVolumeScale = 200.0f;

    switch (thisx->params) {
        case A_OBJ_BLOCK_LARGE:
        case A_OBJ_BLOCK_HUGE:
            this->dyna.bgId = 1;
            Actor_info_part_chg(play, &play->actorCtx, thisx, ACTORCAT_BG);
            En_a_keep_Actor_mode_pu_box_init(this, thisx->params);
            break;
        case A_OBJ_BLOCK_SMALL_ROT:
        case A_OBJ_BLOCK_LARGE_ROT:
            this->dyna.bgId = 3;
            Actor_info_part_chg(play, &play->actorCtx, thisx, ACTORCAT_BG);
            En_a_keep_Actor_mode_drop_del_init(this, thisx->params);
            break;
        case A_OBJ_UNKNOWN_6:
            this->focusYoffset = 10.0f;
            thisx->flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->dyna.bgId = 5;
            thisx->gravity = -2.0f;
            En_a_keep_Actor_mode_non_move_init(this, thisx->params);
            break;
        case A_OBJ_GRASS_CLUMP:
        case A_OBJ_TREE_STUMP:
            this->dyna.bgId = 0;
            En_a_keep_Actor_mode_non_move_init(this, thisx->params);
            break;
        case A_OBJ_SIGNPOST_OBLONG:
        case A_OBJ_SIGNPOST_ARROW:
            thisx->textId = (this->textId & 0xFF) | 0x300;
            thisx->lockOnArrowOffset = 500.0f;
            thisx->flags |= ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY;
            this->focusYoffset = 45.0f;
            En_a_keep_Actor_mode_non_move_init(this, thisx->params);
            ClObjPipe_ct(play, &this->collider);
            ClObjPipe_set5(play, &this->collider, thisx, &OcInfoData);
            thisx->colChkInfo.mass = MASS_IMMOVABLE;
            thisx->attentionRangeType = ATTENTION_RANGE_0;
            break;
        case A_OBJ_BOULDER_FRAGMENT:
            thisx->gravity = -1.5f;
            En_a_keep_Actor_mode_drop2_move_init(this, thisx->params);
            break;
        default:
            thisx->gravity = -2.0f;
            En_a_keep_Actor_mode_non_move_init(this, thisx->params);
            break;
    }

    if (thisx->params <= A_OBJ_BLOCK_LARGE_ROT) { // A_OBJ_BLOCK_*
        thisx->colChkInfo.mass = MASS_IMMOVABLE;
    }

    if (this->dyna.bgId != BGACTOR_NEG_ONE) {
        DynaPolyUty_bgdi_SG2KSG(bgd_info[this->dyna.bgId], &colHeader);
        this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, thisx, colHeader);
    }
}

void En_a_keep_Actor_dt(Actor* thisx, PlayState* play) {
    EnAObj* this = (EnAObj*)thisx;

    DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);

    switch (this->dyna.actor.params) {
        case A_OBJ_SIGNPOST_OBLONG:
        case A_OBJ_SIGNPOST_ARROW:
            ClObjPipe_dt(play, &this->collider);
            break;
    }
}

static void mode_talk(EnAObj* this, PlayState* play) {
    if (Actor_talk_end_check(&this->dyna.actor, play)) {
        En_a_keep_Actor_mode_non_move_init(this, this->dyna.actor.params);
    }
}

void En_a_keep_Actor_mode_non_move_init(EnAObj* this, s16 type) {
    En_A_Keep_actor_set_process(this, En_a_keep_Actor_mode_non_move);
}

void En_a_keep_Actor_mode_non_move(EnAObj* this, PlayState* play) {
    s16 relYawTowardsPlayer;

    if (this->dyna.actor.textId != 0) {
        relYawTowardsPlayer = this->dyna.actor.yawTowardsPlayer - this->dyna.actor.shape.rot.y;
        if (ABS(relYawTowardsPlayer) < 0x2800 ||
            (this->dyna.actor.params == A_OBJ_SIGNPOST_ARROW && ABS(relYawTowardsPlayer) > 0x5800)) {
            if (Actor_talk_check(&this->dyna.actor, play)) {
                En_A_Keep_actor_set_process(this, mode_talk);
            } else {
                Actor_talk_request(&this->dyna.actor, play);
            }
        }
    }
}

void En_a_keep_Actor_mode_drop_del_init(EnAObj* this, s16 type) {
    this->rotateState = 0;
    this->rotateWaitTimer = 10;
    this->dyna.actor.world.rot.y = 0;
    this->dyna.actor.shape.rot = this->dyna.actor.world.rot;
    En_A_Keep_actor_set_process(this, En_a_keep_Actor_mode_drop_del);
}

void En_a_keep_Actor_mode_drop_del(EnAObj* this, PlayState* play) {
    if (this->rotateState == 0) {
        if (this->dyna.interactFlags != 0) {
            this->rotateState++;
            this->rotateForTimer = 20;

            if ((s16)(this->dyna.actor.yawTowardsPlayer + 0x4000) < 0) {
                this->Kusa_AngSpdX = -0x3E8;
            } else {
                this->Kusa_AngSpdX = 0x3E8;
            }

            if (this->dyna.actor.yawTowardsPlayer < 0) {
                this->Kusa_AngSpdY = -this->Kusa_AngSpdX;
            } else {
                this->Kusa_AngSpdY = this->Kusa_AngSpdX;
            }
        }
    } else {
        if (this->rotateWaitTimer != 0) {
            this->rotateWaitTimer--;
        } else {
            this->dyna.actor.shape.rot.y += this->Kusa_AngSpdY;
            this->dyna.actor.shape.rot.x += this->Kusa_AngSpdX;
            this->rotateForTimer--;
            this->dyna.actor.gravity = -1.0f;

            if (this->rotateForTimer == 0) {
                this->dyna.actor.world.pos = this->dyna.actor.home.pos;
                this->rotateState = 0;
                this->rotateWaitTimer = 10;
                this->dyna.actor.velocity.y = 0.0f;
                this->dyna.actor.gravity = 0.0f;
                this->dyna.actor.shape.rot = this->dyna.actor.world.rot;
            }
        }
    }
}

void En_a_keep_Actor_mode_drop2_move_init(EnAObj* this, s16 type) {
    En_A_Keep_actor_set_process(this, En_a_keep_Actor_mode_drop2_move);
}

void En_a_keep_Actor_mode_drop2_move(EnAObj* this, PlayState* play) {
    add_calc(&this->dyna.actor.speed, 1.0f, 1.0f, 0.5f, 0.0f);
    this->dyna.actor.shape.rot.x += this->dyna.actor.world.rot.x >> 1;
    this->dyna.actor.shape.rot.z += this->dyna.actor.world.rot.z >> 1;

    if (this->dyna.actor.speed != 0.0f && this->dyna.actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->dyna.actor.world.rot.y =
            this->dyna.actor.wallYaw - this->dyna.actor.world.rot.y + this->dyna.actor.wallYaw - 0x8000;
        if (1) {}
        this->dyna.actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
    }

    if (this->dyna.actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        if (this->dyna.actor.velocity.y < -8.0f) {
            this->dyna.actor.velocity.y *= -0.6f;
            this->dyna.actor.speed *= 0.6f;
            this->dyna.actor.bgCheckFlags &= ~(BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH);
        } else {
            Actor_delete(&this->dyna.actor);
        }
    }
}

void En_a_keep_Actor_mode_pu_box_init(EnAObj* this, s16 type) {
    this->dyna.actor.cullingVolumeDownward = 1200.0f;
    this->dyna.actor.cullingVolumeScale = 720.0f;
    En_A_Keep_actor_set_process(this, En_a_keep_Actor_mode_pu_box);
}

void En_a_keep_Actor_mode_pu_box(EnAObj* this, PlayState* play) {
    this->dyna.actor.speed += this->dyna.unk_150;
    this->dyna.actor.world.rot.y = this->dyna.unk_158;
    this->dyna.actor.speed = CLAMP(this->dyna.actor.speed, -2.5f, 2.5f);

    add_calc(&this->dyna.actor.speed, 0.0f, 1.0f, 1.0f, 0.0f);

    if (this->dyna.actor.speed != 0.0f) {
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
    }

    this->dyna.unk_154 = 0.0f;
    this->dyna.unk_150 = 0.0f;
}

void En_a_keep_move(Actor* thisx, PlayState* play) {
    EnAObj* this = (EnAObj*)thisx;

    this->actionFunc(this, play);
    Actor_position_moveF(&this->dyna.actor);

    if (this->dyna.actor.gravity != 0.0f) {
        if (this->dyna.actor.params != A_OBJ_BOULDER_FRAGMENT) {
            Actor_BGcheck2(play, &this->dyna.actor, 5.0f, 40.0f, 0.0f,
                                    UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                        UPDBGCHECKINFO_FLAG_4);
        } else {
            Actor_BGcheck2(play, &this->dyna.actor, 5.0f, 20.0f, 0.0f,
                                    UPDBGCHECKINFO_FLAG_0 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_3 |
                                        UPDBGCHECKINFO_FLAG_4);
        }
    }

    this->dyna.actor.focus.pos = this->dyna.actor.world.pos;
    this->dyna.actor.focus.pos.y += this->focusYoffset;

    switch (this->dyna.actor.params) {
        case A_OBJ_SIGNPOST_OBLONG:
        case A_OBJ_SIGNPOST_ARROW:
            CollisionCheck_Uty_ActorWorldPosSetPipeC(&this->dyna.actor, &this->collider);
            CollisionCheck_setOC(play, &play->colChkCtx, &this->collider.base);
            break;
    }
}

void En_a_keep_display(Actor* thisx, PlayState* play) {
    s32 type = thisx->params;

    OPEN_DISPS(play->state.gfxCtx, "../z_en_a_keep.c", 701);

    _texture_z_light_fog_prim(play->state.gfxCtx);

    if (type >= A_OBJ_MAX) {
        type = A_OBJ_BOULDER_FRAGMENT;
    }

    if (thisx->params == A_OBJ_BOULDER_FRAGMENT) {
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 1, 60, 60, 60, 50);
    }

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_en_a_keep.c", 712);
    gSPDisplayList(POLY_OPA_DISP++, a_obj_tbl[type]);

    CLOSE_DISPS(play->state.gfxCtx, "../z_en_a_keep.c", 715);
}
