/*
 * File: z_obj_switch.c
 * Overlay: ovl_Obj_Switch
 * Description: Switches
 */

#include "z_obj_switch.h"
#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"
#include "terminal.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

#define OBJSWITCH_TYPE(thisx) PARAMS_GET_U((thisx)->params, 0, 3)
#define OBJSWITCH_SUBTYPE(thisx) PARAMS_GET_U((thisx)->params, 4, 3)
#define OBJSWITCH_SWITCH_FLAG(thisx) PARAMS_GET_U((thisx)->params, 8, 6)
#define OBJSWITCH_FROZEN(thisx) PARAMS_GET_U((thisx)->params, 7, 1)

#define OBJSWITCH_FROZEN_FLAG (1 << 7)

void Obj_Switch_actor_ct(Actor* thisx, PlayState* play);
void Obj_Switch_actor_dt(Actor* thisx, PlayState* play);
void Obj_Switch_actor_move(Actor* thisx, PlayState* play);
void Obj_Switch_actor_draw(Actor* thisx, PlayState* play);

void mv_off_init_fumi(ObjSwitch* this);
void mv_off_fumi(ObjSwitch* this, PlayState* play);
void mv_push_init_fumi(ObjSwitch* this);
void mv_push_fumi(ObjSwitch* this, PlayState* play);
void mv_on_init_fumi(ObjSwitch* this);
void mv_on_fumi(ObjSwitch* this, PlayState* play);
void mv_pop_init_fumi(ObjSwitch* this);
void mv_pop_fumi(ObjSwitch* this, PlayState* play);

void mv_iceWait_init_mato(ObjSwitch* this);
void mv_iceWait_mato(ObjSwitch* this, PlayState* play);
void mv_off_init_mato(ObjSwitch* this);
void mv_off_mato(ObjSwitch* this, PlayState* play);
void mv_close_init_mato(ObjSwitch* this);
void mv_close_mato(ObjSwitch* this, PlayState* play);
void mv_on_init_mato(ObjSwitch* this);
void mv_on_mato(ObjSwitch* this, PlayState* play);
void mv_open_init_mato(ObjSwitch* this);
void mv_open_mato(ObjSwitch* this, PlayState* play);

void mv_off_init_shock(ObjSwitch* this);
static void mv_off_shock(ObjSwitch* this, PlayState* play);
void mv_off_to_on_init_shock(ObjSwitch* this);
void mv_off_to_on_shock(ObjSwitch* this, PlayState* play);
void mv_on_init_shock(ObjSwitch* this);
static void mv_on_shock(ObjSwitch* this, PlayState* play);
void mv_on_to_off_init_shock(ObjSwitch* this);
void mv_on_to_off_shock(ObjSwitch* this, PlayState* play);

ActorProfile Obj_Switch_Profile = {
    /**/ ACTOR_OBJ_SWITCH,
    /**/ ACTORCAT_SWITCH,
    /**/ FLAGS,
    /**/ OBJECT_GAMEPLAY_DANGEON_KEEP,
    /**/ sizeof(ObjSwitch),
    /**/ Obj_Switch_actor_ct,
    /**/ Obj_Switch_actor_dt,
    /**/ Obj_Switch_actor_move,
    /**/ Obj_Switch_actor_draw,
};

static f32 EyeOffsetTbl[] = {
    10, // OBJSWITCH_TYPE_FLOOR
    10, // OBJSWITCH_TYPE_FLOOR_RUSTY
    0,  // OBJSWITCH_TYPE_EYE
    30, // OBJSWITCH_TYPE_CRYSTAL
    30, // OBJSWITCH_TYPE_CRYSTAL_TARGETABLE
};

static ColliderTrisElementInit ClTrisElemDt_switch_ham[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -20.0f, 19.0f, -20.0f }, { -20.0f, 19.0f, 20.0f }, { 20.0f, 19.0f, 20.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x40000040, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 20.0f, 19.0f, 20.0f }, { 20.0f, 19.0f, -20.0f }, { -20.0f, 19.0f, -20.0f } } },
    },
};

static ColliderTrisInit ClTrisDt_switch_ham = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(ClTrisElemDt_switch_ham),
    ClTrisElemDt_switch_ham,
};

static ColliderTrisElementInit ClTrisElemDt_switch_mato[2] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { -23.0f, 0.0f, 8.5f }, { 0.0f, -23.0f, 8.5f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { 0.0f, -23.0f, 8.5f }, { 23.0f, 0.0f, 8.5f } } },
    },
};

static ColliderTrisInit ClTrisDt_switch_mato = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(ClTrisElemDt_switch_mato),
    ClTrisElemDt_switch_mato,
};

static ColliderJntSphElementInit ClSphElemDt_switch_shock[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xEFC1FFFE, 0x00, 0x00 },
            ATELEM_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 300, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit ClSphDt_switch_shock = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(ClSphElemDt_switch_shock),
    ClSphElemDt_switch_shock,
};

static InitChainEntry value_init[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 2000, ICHAIN_STOP),
};

void rotY_switch(Vec3f* dest, Vec3f* src, s16 rotY) {
    f32 s = sin_s(rotY);
    f32 c = cos_s(rotY);

    dest->x = src->z * s + src->x * c;
    dest->y = src->y;
    dest->z = src->z * c - src->x * s;
}

static void set_dynaPoly(ObjSwitch* this, PlayState* play, CollisionHeader* collision, s32 moveFlag) {
    s32 pad;
    CollisionHeader* colHeader = NULL;

    MoveBG_ct(&this->dyna, moveFlag);
    DynaPolyUty_bgdi_SG2KSG(collision, &colHeader);
    this->dyna.bgId = DynaPolyInfo_setActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);

#if DEBUG_FEATURES
    if (this->dyna.bgId == BG_ACTOR_MAX) {
        s32 pad2;

        // "Warning : move BG registration failure"
        PRINTF("Warning : move BG 登録失敗(%s %d)(name %d)(arg_data 0x%04x)\n", "../z_obj_switch.c", 531,
               this->dyna.actor.id, this->dyna.actor.params);
    }
#endif
}

void set_collision_data_sph(ObjSwitch* this, PlayState* play, ColliderJntSphInit* colliderJntSphInit) {
    ColliderJntSph* colliderJntSph = &this->jntSph.col;

    ClObjJntSph_ct(play, colliderJntSph);
    ClObjJntSph_set5_nzm(play, colliderJntSph, &this->dyna.actor, colliderJntSphInit, this->jntSph.items);
    Matrix_softcv3_load(this->dyna.actor.world.pos.x,
                                 this->dyna.actor.world.pos.y +
                                     this->dyna.actor.shape.yOffset * this->dyna.actor.scale.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    Matrix_scale(this->dyna.actor.scale.x, this->dyna.actor.scale.y, this->dyna.actor.scale.z, MTXMODE_APPLY);
    CollisionCheck_Uty_convJntSphL2G(0, colliderJntSph);
}

void set_collision_data_tris(ObjSwitch* this, PlayState* play, ColliderTrisInit* colliderTrisInit) {
    ColliderTris* colliderTris = &this->tris.col;
    s32 i;
    s32 j;
    Vec3f pos[3];

    ClObjTris_ct(play, colliderTris);
    ClObjTris_set5_nzm(play, colliderTris, &this->dyna.actor, colliderTrisInit, this->tris.items);

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 3; j++) {
            rotY_switch(&pos[j], &colliderTrisInit->elements[i].dim.vtx[j], this->dyna.actor.home.rot.y);
            xyz_t_add(&pos[j], &this->dyna.actor.world.pos, &pos[j]);
        }

        CollisionCheck_Uty_setTrisPos(colliderTris, i, &pos[0], &pos[1], &pos[2]);
    }
}

Actor* set_ice_poly_switch(ObjSwitch* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;

    return Actor_info_make_child_actor(&play->actorCtx, thisx, play, ACTOR_OBJ_ICE_POLY, thisx->world.pos.x, thisx->world.pos.y,
                              thisx->world.pos.z, thisx->world.rot.x, thisx->world.rot.y, thisx->world.rot.z,
                              OBJSWITCH_SWITCH_FLAG(&this->dyna.actor) << 8);
}

static void set_switchON(ObjSwitch* this, PlayState* play) {
    s32 pad;
    s32 subType;

    if (Actor_Environment_sw_Check(play, OBJSWITCH_SWITCH_FLAG(&this->dyna.actor))) {
        this->cooldownOn = false;
    } else {
        subType = OBJSWITCH_SUBTYPE(&this->dyna.actor);
        Actor_Environment_sw_On(play, OBJSWITCH_SWITCH_FLAG(&this->dyna.actor));

        if (subType == OBJSWITCH_SUBTYPE_ONCE || subType == OBJSWITCH_SUBTYPE_SYNC) {
            makeActorAttentionDemoSE(play, &this->dyna.actor, NA_SE_SY_CORRECT_CHIME);
        } else {
            makeActorAttentionDemoSE(play, &this->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
        }

        this->cooldownOn = true;
    }
}

static void set_switchOFF(ObjSwitch* this, PlayState* play) {
    this->cooldownOn = false;

    if (Actor_Environment_sw_Check(play, OBJSWITCH_SWITCH_FLAG(&this->dyna.actor))) {
        Actor_Environment_sw_Off(play, OBJSWITCH_SWITCH_FLAG(&this->dyna.actor));

        if (OBJSWITCH_SUBTYPE(&this->dyna.actor) == OBJSWITCH_SUBTYPE_TOGGLE) {
            makeActorAttentionDemoSE(play, &this->dyna.actor, NA_SE_SY_TRE_BOX_APPEAR);
            this->cooldownOn = true;
        }
    }
}

void setup_two_tex_scroll_value_shock(ObjSwitch* this) {
    this->x1TexScroll = (this->x1TexScroll - 1) & 0x7F;
    this->y1TexScroll = (this->y1TexScroll + 1) & 0x7F;
    this->x2TexScroll = (this->x2TexScroll + 1) & 0x7F;
    this->y2TexScroll = (this->y2TexScroll - 1) & 0x7F;
}

void Obj_Switch_actor_ct(Actor* thisx, PlayState* play) {
    ObjSwitch* this = (ObjSwitch*)thisx;
    s32 isSwitchFlagSet = Actor_Environment_sw_Check(play, OBJSWITCH_SWITCH_FLAG(&this->dyna.actor));
    s32 type = OBJSWITCH_TYPE(&this->dyna.actor);

    if (type == OBJSWITCH_TYPE_FLOOR || type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        set_dynaPoly(this, play, &gFloorSwitchCol, DYNA_TRANSFORM_POS);
    }

    ValueSet_process(&this->dyna.actor, value_init);

    if (type == OBJSWITCH_TYPE_FLOOR || type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 1.0f;
    }

    Actor_world_to_eye(&this->dyna.actor, EyeOffsetTbl[type]);

    if (type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        set_collision_data_tris(this, play, &ClTrisDt_switch_ham);
    } else if (type == OBJSWITCH_TYPE_EYE) {
        set_collision_data_tris(this, play, &ClTrisDt_switch_mato);
    } else if (type == OBJSWITCH_TYPE_CRYSTAL || type == OBJSWITCH_TYPE_CRYSTAL_TARGETABLE) {
        set_collision_data_sph(this, play, &ClSphDt_switch_shock);
    }

    if (type == OBJSWITCH_TYPE_CRYSTAL_TARGETABLE) {
        this->dyna.actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->dyna.actor.attentionRangeType = ATTENTION_RANGE_4;
    }

    this->dyna.actor.colChkInfo.mass = MASS_IMMOVABLE;

    if (OBJSWITCH_FROZEN(&this->dyna.actor) && (set_ice_poly_switch(this, play) == NULL)) {
        PRINTF_COLOR_RED();
        PRINTF("Error : 氷発生失敗 (%s %d)\n", "../z_obj_switch.c", 732);
        PRINTF_RST();
        this->dyna.actor.params &= ~OBJSWITCH_FROZEN_FLAG;
    }

    if (OBJSWITCH_FROZEN(&this->dyna.actor)) {
        mv_iceWait_init_mato(this);
    } else if (type == OBJSWITCH_TYPE_FLOOR || type == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        //! @bug This condition does not account for OBJSWITCH_SUBTYPE_HOLD_INVERTED which expects
        //! the relationship between the switch position and the switch flag to be inverted
        if (isSwitchFlagSet) {
            mv_on_init_fumi(this);
        } else {
            mv_off_init_fumi(this);
        }
    } else if (type == OBJSWITCH_TYPE_EYE) {
        if (isSwitchFlagSet) {
            mv_on_init_mato(this);
        } else {
            mv_off_init_mato(this);
        }
    } else if (type == OBJSWITCH_TYPE_CRYSTAL || type == OBJSWITCH_TYPE_CRYSTAL_TARGETABLE) {
        if (isSwitchFlagSet) {
            mv_on_init_shock(this);
        } else {
            mv_off_init_shock(this);
        }
    }

    PRINTF("(Dungeon switch)(arg_data 0x%04x)\n", this->dyna.actor.params);
}

void Obj_Switch_actor_dt(Actor* thisx, PlayState* play) {
    ObjSwitch* this = (ObjSwitch*)thisx;

    switch (OBJSWITCH_TYPE(&this->dyna.actor)) {
        case OBJSWITCH_TYPE_FLOOR:
        case OBJSWITCH_TYPE_FLOOR_RUSTY:
            DynaPolyInfo_delReserve(play, &play->colCtx.dyna, this->dyna.bgId);
            break;
    }

    switch (OBJSWITCH_TYPE(&this->dyna.actor)) {
        case OBJSWITCH_TYPE_FLOOR_RUSTY:
        case OBJSWITCH_TYPE_EYE:
            ClObjTris_dt_nzf(play, &this->tris.col);
            break;

        case OBJSWITCH_TYPE_CRYSTAL:
        case OBJSWITCH_TYPE_CRYSTAL_TARGETABLE:
            ClObjJntSph_dt_nzf(play, &this->jntSph.col);
            break;
    }
}

void mv_off_init_fumi(ObjSwitch* this) {
    this->dyna.actor.scale.y = 33.0f / 200.0f;
    this->actionFunc = mv_off_fumi;
}

void mv_off_fumi(ObjSwitch* this, PlayState* play) {
    if (OBJSWITCH_TYPE(&this->dyna.actor) == OBJSWITCH_TYPE_FLOOR_RUSTY) {
        if (this->tris.col.base.acFlags & AC_HIT) {
            mv_push_init_fumi(this);
            set_switchON(this, play);
            this->tris.col.base.acFlags &= ~AC_HIT;
        } else {
            CollisionCheck_setAC(play, &play->colChkCtx, &this->tris.col.base);
        }
    } else {
        switch (OBJSWITCH_SUBTYPE(&this->dyna.actor)) {
            case OBJSWITCH_SUBTYPE_ONCE:
                if (MoveBG_checkRidePlayerStatus(&this->dyna)) {
                    mv_push_init_fumi(this);
                    set_switchON(this, play);
                }
                break;

            case OBJSWITCH_SUBTYPE_TOGGLE:
                if ((this->dyna.interactFlags & DYNA_INTERACT_PLAYER_ON_TOP) &&
                    !(this->prevColFlags & DYNA_INTERACT_PLAYER_ON_TOP)) {
                    mv_push_init_fumi(this);
                    set_switchON(this, play);
                }
                break;

            case OBJSWITCH_SUBTYPE_HOLD:
                if (MoveBG_checkSwOnStatus(&this->dyna)) {
                    mv_push_init_fumi(this);
                    set_switchON(this, play);
                }
                break;

            case OBJSWITCH_SUBTYPE_HOLD_INVERTED:
                if (MoveBG_checkSwOnStatus(&this->dyna)) {
                    mv_push_init_fumi(this);
                    set_switchOFF(this, play);
                }
                break;
        }
    }
}

void mv_push_init_fumi(ObjSwitch* this) {
    this->actionFunc = mv_push_fumi;
    this->cooldownTimer = 100;
}

void mv_push_fumi(ObjSwitch* this, PlayState* play) {
    if (OBJSWITCH_SUBTYPE(&this->dyna.actor) == OBJSWITCH_SUBTYPE_HOLD_INVERTED || !this->cooldownOn ||
        getAttentionDemoPart() == this->dyna.actor.category || this->cooldownTimer <= 0) {
        this->dyna.actor.scale.y -= 99.0f / 2000.0f;
        if (this->dyna.actor.scale.y <= 33.0f / 2000.0f) {
            mv_on_init_fumi(this);
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 120, 20, 10);
        }
    }
}

void mv_on_init_fumi(ObjSwitch* this) {
    this->dyna.actor.scale.y = 33.0f / 2000.0f;
    this->releaseTimer = 6;
    this->actionFunc = mv_on_fumi;
}

void mv_on_fumi(ObjSwitch* this, PlayState* play) {
    switch (OBJSWITCH_SUBTYPE(&this->dyna.actor)) {
        case OBJSWITCH_SUBTYPE_ONCE:
            if (!Actor_Environment_sw_Check(play, OBJSWITCH_SWITCH_FLAG(&this->dyna.actor))) {
                mv_pop_init_fumi(this);
            }
            break;

        case OBJSWITCH_SUBTYPE_TOGGLE:
            if ((this->dyna.interactFlags & DYNA_INTERACT_PLAYER_ON_TOP) &&
                !(this->prevColFlags & DYNA_INTERACT_PLAYER_ON_TOP)) {
                mv_pop_init_fumi(this);
                set_switchOFF(this, play);
            }
            break;

        case OBJSWITCH_SUBTYPE_HOLD:
        case OBJSWITCH_SUBTYPE_HOLD_INVERTED:
            if (!MoveBG_checkSwOnStatus(&this->dyna) && !player_demo_check(play)) {
                if (this->releaseTimer <= 0) {
                    mv_pop_init_fumi(this);
                    if (OBJSWITCH_SUBTYPE(&this->dyna.actor) == OBJSWITCH_SUBTYPE_HOLD) {
                        set_switchOFF(this, play);
                    } else {
                        set_switchON(this, play);
                    }
                }
            } else {
                this->releaseTimer = 6;
            }
            break;
    }
}

void mv_pop_init_fumi(ObjSwitch* this) {
    this->actionFunc = mv_pop_fumi;
    this->cooldownTimer = 100;
}

void mv_pop_fumi(ObjSwitch* this, PlayState* play) {
    s16 subType = OBJSWITCH_SUBTYPE(&this->dyna.actor);

    if (((subType != OBJSWITCH_SUBTYPE_TOGGLE) && (subType != OBJSWITCH_SUBTYPE_HOLD_INVERTED)) || !this->cooldownOn ||
        getAttentionDemoPart() == this->dyna.actor.category || this->cooldownTimer <= 0) {

        this->dyna.actor.scale.y += 99.0f / 2000.0f;
        if (this->dyna.actor.scale.y >= 33.0f / 200.0f) {
            mv_off_init_fumi(this);
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
            if (subType == OBJSWITCH_SUBTYPE_TOGGLE) {
                z_vibctl2_vib_setQ(this->dyna.actor.xyzDistToPlayerSq, 120, 20, 10);
            }
        }
    }
}

s32 check_arrowHit(ObjSwitch* this) {
    Actor* collidingActor;
    s16 yawDiff;

    if ((this->tris.col.base.acFlags & AC_HIT) && !(this->prevColFlags & AC_HIT)) {
        collidingActor = this->tris.col.base.ac;
        if (collidingActor != NULL) {
            yawDiff = collidingActor->world.rot.y - this->dyna.actor.shape.rot.y;
            if (ABS(yawDiff) > 0x5000) {
                return true;
            }
        }
    }
    return false;
}

void mv_iceWait_init_mato(ObjSwitch* this) {
    this->actionFunc = mv_iceWait_mato;
}

void mv_iceWait_mato(ObjSwitch* this, PlayState* play) {
    if (Actor_Environment_sw_Check(play, OBJSWITCH_SWITCH_FLAG(&this->dyna.actor))) {
        mv_on_init_mato(this);
    } else {
        mv_off_init_mato(this);
    }
}

void mv_off_init_mato(ObjSwitch* this) {
    this->actionFunc = mv_off_mato;
    this->eyeTexIndex = 0;
}

void mv_off_mato(ObjSwitch* this, PlayState* play) {
    if (check_arrowHit(this) || OBJSWITCH_FROZEN(&this->dyna.actor)) {
        mv_close_init_mato(this);
        set_switchON(this, play);
        this->dyna.actor.params &= ~OBJSWITCH_FROZEN_FLAG;
    }
}

void mv_close_init_mato(ObjSwitch* this) {
    this->actionFunc = mv_close_mato;
    this->cooldownTimer = 100;
}

void mv_close_mato(ObjSwitch* this, PlayState* play) {
    if (!this->cooldownOn || getAttentionDemoPart() == this->dyna.actor.category || this->cooldownTimer <= 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= 3) {
            mv_on_init_mato(this);
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void mv_on_init_mato(ObjSwitch* this) {
    this->actionFunc = mv_on_mato;
    this->eyeTexIndex = 3;
}

void mv_on_mato(ObjSwitch* this, PlayState* play) {
    switch (OBJSWITCH_SUBTYPE(&this->dyna.actor)) {
        case OBJSWITCH_SUBTYPE_ONCE:
            if (!Actor_Environment_sw_Check(play, OBJSWITCH_SWITCH_FLAG(&this->dyna.actor))) {
                mv_open_init_mato(this);
                this->dyna.actor.params &= ~OBJSWITCH_FROZEN_FLAG;
            }
            break;

        case OBJSWITCH_SUBTYPE_TOGGLE:
            if (check_arrowHit(this) || OBJSWITCH_FROZEN(&this->dyna.actor)) {
                mv_open_init_mato(this);
                set_switchOFF(this, play);
                this->dyna.actor.params &= ~OBJSWITCH_FROZEN_FLAG;
            }
            break;
    }
}

void mv_open_init_mato(ObjSwitch* this) {
    this->actionFunc = mv_open_mato;
    this->cooldownTimer = 100;
}

void mv_open_mato(ObjSwitch* this, PlayState* play) {
    if (OBJSWITCH_SUBTYPE(&this->dyna.actor) != OBJSWITCH_SUBTYPE_TOGGLE || !this->cooldownOn ||
        getAttentionDemoPart() == this->dyna.actor.category || this->cooldownTimer <= 0) {

        this->eyeTexIndex--;
        if (this->eyeTexIndex <= 0) {
            mv_off_init_mato(this);
            Actor_SE_set(&this->dyna.actor, NA_SE_EV_FOOT_SWITCH);
        }
    }
}

void mv_off_init_shock(ObjSwitch* this) {
    this->crystalColor.r = 0;
    this->crystalColor.g = 0;
    this->crystalColor.b = 0;
    this->crystalSubtype1texture = gCrstalSwitchRedTex;
    this->actionFunc = mv_off_shock;
}

static void mv_off_shock(ObjSwitch* this, PlayState* play) {
    switch (OBJSWITCH_SUBTYPE(&this->dyna.actor)) {
        case OBJSWITCH_SUBTYPE_ONCE:
            if ((this->jntSph.col.base.acFlags & AC_HIT) && this->disableAcTimer <= 0) {
                this->disableAcTimer = 10;
                set_switchON(this, play);
                mv_off_to_on_init_shock(this);
            }
            break;

        case OBJSWITCH_SUBTYPE_SYNC:
            if (((this->jntSph.col.base.acFlags & AC_HIT) && this->disableAcTimer <= 0) ||
                Actor_Environment_sw_Check(play, OBJSWITCH_SWITCH_FLAG(&this->dyna.actor))) {

                this->disableAcTimer = 10;
                set_switchON(this, play);
                mv_off_to_on_init_shock(this);
            }
            break;

        case OBJSWITCH_SUBTYPE_TOGGLE:
            if ((this->jntSph.col.base.acFlags & AC_HIT) && !(this->prevColFlags & AC_HIT) &&
                this->disableAcTimer <= 0) {
                this->disableAcTimer = 10;
                set_switchON(this, play);
                mv_off_to_on_init_shock(this);
            }
            setup_two_tex_scroll_value_shock(this);
            break;
    }
}

void mv_off_to_on_init_shock(ObjSwitch* this) {
    this->actionFunc = mv_off_to_on_shock;
    this->cooldownTimer = 100;
}

void mv_off_to_on_shock(ObjSwitch* this, PlayState* play) {
    if (!this->cooldownOn || getAttentionDemoPart() == this->dyna.actor.category || this->cooldownTimer <= 0) {
        mv_on_init_shock(this);
        if (OBJSWITCH_SUBTYPE(&this->dyna.actor) == OBJSWITCH_SUBTYPE_TOGGLE) {
            setup_two_tex_scroll_value_shock(this);
        }
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_DIAMOND_SWITCH);
    }
}

void mv_on_init_shock(ObjSwitch* this) {
    this->crystalColor.r = 255;
    this->crystalColor.g = 255;
    this->crystalColor.b = 255;
    this->crystalSubtype1texture = gCrstalSwitchBlueTex;
    this->actionFunc = mv_on_shock;
}

static void mv_on_shock(ObjSwitch* this, PlayState* play) {
    switch (OBJSWITCH_SUBTYPE(&this->dyna.actor)) {
        case OBJSWITCH_SUBTYPE_ONCE:
        case OBJSWITCH_SUBTYPE_SYNC:
            if (!Actor_Environment_sw_Check(play, OBJSWITCH_SWITCH_FLAG(&this->dyna.actor))) {
                mv_on_to_off_init_shock(this);
            }
            break;

        case OBJSWITCH_SUBTYPE_TOGGLE:
            if ((this->jntSph.col.base.acFlags & AC_HIT) && !(this->prevColFlags & AC_HIT) &&
                this->disableAcTimer <= 0) {
                this->disableAcTimer = 10;
                mv_on_to_off_init_shock(this);
                set_switchOFF(this, play);
            }
            break;
    }
    setup_two_tex_scroll_value_shock(this);
}

void mv_on_to_off_init_shock(ObjSwitch* this) {
    this->actionFunc = mv_on_to_off_shock;
    this->cooldownTimer = 100;
}

void mv_on_to_off_shock(ObjSwitch* this, PlayState* play) {
    if (OBJSWITCH_SUBTYPE(&this->dyna.actor) != OBJSWITCH_SUBTYPE_TOGGLE || !this->cooldownOn ||
        getAttentionDemoPart() == this->dyna.actor.category || this->cooldownTimer <= 0) {
        mv_off_init_shock(this);
        setup_two_tex_scroll_value_shock(this);
        Actor_SE_set(&this->dyna.actor, NA_SE_EV_DIAMOND_SWITCH);
    }
}

void Obj_Switch_actor_move(Actor* thisx, PlayState* play) {
    ObjSwitch* this = (ObjSwitch*)thisx;

    if (this->releaseTimer > 0) {
        this->releaseTimer--;
    }
    if (this->cooldownTimer > 0) {
        this->cooldownTimer--;
    }

    this->actionFunc(this, play);

    switch (OBJSWITCH_TYPE(&this->dyna.actor)) {
        case OBJSWITCH_TYPE_FLOOR:
        case OBJSWITCH_TYPE_FLOOR_RUSTY:
            this->prevColFlags = this->dyna.interactFlags;
            break;

        case OBJSWITCH_TYPE_EYE:
            this->prevColFlags = this->tris.col.base.acFlags;
            this->tris.col.base.acFlags &= ~AC_HIT;
            CollisionCheck_setAC(play, &play->colChkCtx, &this->tris.col.base);
            break;

        case OBJSWITCH_TYPE_CRYSTAL:
        case OBJSWITCH_TYPE_CRYSTAL_TARGETABLE:
            if (!player_demo_check(play) && this->disableAcTimer > 0) {
                this->disableAcTimer--;
            }
            this->prevColFlags = this->jntSph.col.base.acFlags;
            this->jntSph.col.base.acFlags &= ~AC_HIT;
            if (this->disableAcTimer <= 0) {
                CollisionCheck_setAC(play, &play->colChkCtx, &this->jntSph.col.base);
            }
            CollisionCheck_setOC(play, &play->colChkCtx, &this->jntSph.col.base);
            break;
    }
}

void draw_proc_fumi(Actor* thisx, PlayState* play) {
    static Gfx* sw_fumi_model[] = {
        gFloorSwitch1DL, // OBJSWITCH_SUBTYPE_ONCE
        gFloorSwitch3DL, // OBJSWITCH_SUBTYPE_TOGGLE
        gFloorSwitch2DL, // OBJSWITCH_SUBTYPE_HOLD
        gFloorSwitch2DL, // OBJSWITCH_SUBTYPE_HOLD_INVERTED
    };

    Cheap_gfx_display(play, sw_fumi_model[OBJSWITCH_SUBTYPE(thisx)]);
}

void draw_proc_ham(Actor* thisx, PlayState* play) {
    Cheap_gfx_display(play, gRustyFloorSwitchDL);
}

void draw_proc_mato(Actor* thisx, PlayState* play) {
    static void* mato_txt_tbl[][4] = {
        // OBJSWITCH_SUBTYPE_ONCE
        { gEyeSwitchGoldOpenTex, gEyeSwitchGoldOpeningTex, gEyeSwitchGoldClosingTex, gEyeSwitchGoldClosedTex },
        // OBJSWITCH_SUBTYPE_TOGGLE
        { gEyeSwitchSilverOpenTex, gEyeSwitchSilverHalfTex, gEyeSwitchSilverClosedTex, gEyeSwitchSilverClosedTex },
    };
    static Gfx* sw_mato_model[] = {
        gEyeSwitch1DL, // OBJSWITCH_SUBTYPE_ONCE
        gEyeSwitch2DL, // OBJSWITCH_SUBTYPE_TOGGLE
    };
    ObjSwitch* this = (ObjSwitch*)thisx;
    s32 subType = OBJSWITCH_SUBTYPE(&this->dyna.actor);

    OPEN_DISPS(play->state.gfxCtx, "../z_obj_switch.c", 1459);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_obj_switch.c", 1462);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(mato_txt_tbl[subType][this->eyeTexIndex]));
    gSPDisplayList(POLY_OPA_DISP++, sw_mato_model[subType]);

    CLOSE_DISPS(play->state.gfxCtx, "../z_obj_switch.c", 1471);
}

void draw_proc_shock(Actor* thisx, PlayState* play) {
    static Gfx* s1_model[] = {
        gCrystalSwitchCoreXluDL,    // OBJSWITCH_SUBTYPE_ONCE
        gCrystalSwitchDiamondXluDL, // OBJSWITCH_SUBTYPE_TOGGLE
        NULL,                       // OBJSWITCH_SUBTYPE_HOLD
        NULL,                       // OBJSWITCH_SUBTYPE_HOLD_INVERTED
        gCrystalSwitchCoreXluDL,    // OBJSWITCH_SUBTYPE_SYNC
    };
    static Gfx* s2_s3_model[] = {
        gCrystalSwitchCoreOpaDL,    // OBJSWITCH_SUBTYPE_ONCE
        gCrystalSwitchDiamondOpaDL, // OBJSWITCH_SUBTYPE_TOGGLE
        NULL,                       // OBJSWITCH_SUBTYPE_HOLD
        NULL,                       // OBJSWITCH_SUBTYPE_HOLD_INVERTED
        gCrystalSwitchCoreOpaDL     // OBJSWITCH_SUBTYPE_SYNC
    };
    ObjSwitch* this = (ObjSwitch*)thisx;
    s32 pad;
    s32 subType = OBJSWITCH_SUBTYPE(&this->dyna.actor);

    Actor_HiliteReflect_xlu_set_init(&this->dyna.actor, play, 0);

    OPEN_DISPS(play->state.gfxCtx, "../z_obj_switch.c", 1494);

    _texture_z_light_fog_prim_xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx, "../z_obj_switch.c", 1497);
    gSPDisplayList(POLY_XLU_DISP++, s1_model[subType]);

    CLOSE_DISPS(play->state.gfxCtx, "../z_obj_switch.c", 1502);

    OPEN_DISPS(play->state.gfxCtx, "../z_obj_switch.c", 1507);

    _texture_z_light_fog_prim(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx, "../z_obj_switch.c", 1511);

    if (subType == OBJSWITCH_SUBTYPE_TOGGLE) {
        gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(this->crystalSubtype1texture));
    }

    gDPSetEnvColor(POLY_OPA_DISP++, this->crystalColor.r, this->crystalColor.g, this->crystalColor.b, 128);
    gSPSegment(POLY_OPA_DISP++, 0x08,
               two_tex_scroll(play->state.gfxCtx, G_TX_RENDERTILE, this->x1TexScroll, this->y1TexScroll, 0x20, 0x20,
                                1, this->x2TexScroll, this->y2TexScroll, 0x20, 0x20));
    gSPDisplayList(POLY_OPA_DISP++, s2_s3_model[subType]);

    CLOSE_DISPS(play->state.gfxCtx, "../z_obj_switch.c", 1533);
}

static ObjSwitchDrawFunc draw_proc[] = {
    draw_proc_fumi,      // OBJSWITCH_TYPE_FLOOR
    draw_proc_ham, // OBJSWITCH_TYPE_FLOOR_RUSTY
    draw_proc_mato,        // OBJSWITCH_TYPE_EYE
    draw_proc_shock,    // OBJSWITCH_TYPE_CRYSTAL
    draw_proc_shock,    // OBJSWITCH_TYPE_CRYSTAL_TARGETABLE
};

void Obj_Switch_actor_draw(Actor* thisx, PlayState* play) {
    draw_proc[OBJSWITCH_TYPE(thisx)](thisx, play);
}
